#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GSocket.hh>
#include <Grid/Util/GDebug.hh>
#include <Grid/Util/GRetainer.hh>
#include <Grid/GFiber.hh>

#define G_RETAIN(x) GRetainer<G4TransportL> __retainer__(this); Q_UNUSED(__retainer__)
/**********************************************************************************************/
extern Int  g_4_receiveWindow;
extern Int  g_4_retransmitRetries;
extern Int  g_4_retransmitTimeout;
extern Int  g_4_softTimeout;
extern Int  g_4_hardTimeout;
extern Int  g_4_ackOfSyn2Dilation;
/**********************************************************************************************/

template <class List, class Lambda>
Void deleteIf(List& list, const Lambda& lambda)
{
  for (auto it = list.begin(); it != list.end(); )
  {
    if (lambda(*it))
    {
      delete *it;
      it = list.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

/**********************************************************************************************/

template <class List, class Lambda>
Void removeIf(List& list, const Lambda& lambda)
{
  for (auto it = list.begin(); it != list.end(); )
  {
    if (lambda(*it))
      it = list.erase(it);
    else
      ++it;
  }
}

/********************************************* TX *********************************************/
/*                                        G4Transport                                         */
/**********************************************************************************************/

Vacuum G4Transport :: G4Transport(GMachine* machine):
  m_machine(machine),
  m_layer3(new G3Network(this))
{
}

/**********************************************************************************************/

Vacuum G4Transport :: ~G4Transport()
{
  gDeleteAll(m_connections);
  gDeleteAll(m_listeners);
}

/**********************************************************************************************/

Bool G4Transport :: isPortFree(Port port) const
{
  if (m_listeners.value(port))
    return false;

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
    if (std::get<0>(it.key()) == port)
      return false;

  return true;
}

/**********************************************************************************************/

Int G4Transport :: newSrcPort()
{
  for (int i = 0; i < 100; i++)
  {
    Int port = rand() % 0x8000 + 0x8000;
    if (isPortFree(port))
      return port;
  }

  for (Int i = 0x8000; i < 0x10000; i++)
  {
    if (isPortFree(i))
      return i;
  }

  return 0;
}

/**********************************************************************************************/

Bool G4Transport :: read(Address l3_src, GFrame& frame)
{
  G4Header      header;

  header.remotePort       = frame.readFront2();  //  0- 2
  header.localPort        = frame.readFront2();  //  2- 4
  header.sequence         = frame.readFront4();  //  4- 8
  header.ack              = frame.readFront4();  //  8-12
  header.receiveWindow    = frame.readFront2();  // 12-16
  header.flag             = frame.readFront1();  // 16-17
  GDescriptor   descriptor  = GDescriptor(header.localPort, l3_src, header.remotePort);

  gDebug(this, "R: remotePort = %i",    header.remotePort);
  gDebug(this, "R: localPort  = %i",    header.localPort);
  gDebug(this, "R: seq        = %i",    header.sequence);
  gDebug(this, "R: ack        = %i",    header.ack);
  gDebug(this, "R: window     = %i",    header.receiveWindow);
  gDebug(this, "R: flag       = %x %s", header.flag, G4TransportL::flagName(header.flag));

  if (G4TransportL* connection = m_connections.value(descriptor))
  {
    gDebug(this, "R: ACCEPT frame (size=%i)", frame.size());
    connection->readFrame(header, frame);
    return true;
  }

  if (!(header.flag & SYN))
  {
    gDebug(this, "R: REJECT [not SYN, not CONNECTED]");
    return false;
  }

  if (G4TransportL* server = m_listeners.value(header.localPort))
  {
    G4TransportL* connection = new G4TransportL(this, 0, header.localPort, l3_src, header.remotePort);

    gDebug(this, "R:SERVER: new connection, state = SynReceived");
    connection->m_parent  = server;
    connection->m_state   = G4TransportL::SynReceived;
    connection->m_srcSeq  = header.sequence;
    connection->m_dstSeq  = rand();
    connection->writeControl(ACK | SYN | RACK);

    return true;
  }

  return false;
}

/**********************************************************************************************/

Bool G4Transport :: readChoke(Address src)
{
  gDebug(this, "R: >>CHOKE<< packet received, drastically reducing congestion windows");

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
  {
    G4TransportL* connection = it.value();
    if (connection->m_dstAddr == src)
      connection->choke();
  }

  return true;
}

/**********************************************************************************************/

Void G4Transport :: removeLocal(G4TransportL* local)
{
  auto matchLocal = [=](G4TransportL* l) { return l == local; };

  removeIf(m_connections, matchLocal);
  removeIf(m_listeners,   matchLocal);
}

/**********************************************************************************************/

Void G4Transport :: removeProcess(GProcess* process)
{
  auto matchProcess = [=](G4TransportL* local)
  {
    if (local->m_socket && (local->m_socket->process() == process))
      return local->m_global = 0, true;
    return false;
  };

  deleteIf(m_connections, matchProcess);
  deleteIf(m_listeners,   matchProcess);
}

/**********************************************************************************************/

Bool G4Transport :: write(G4TransportL* local, GFrame& frame)
{
  frame.writeFront2(local->m_dstPort);  //  2- 4
  frame.writeFront2(local->m_srcPort);  //  0- 2

  gDebug(this, "W: srcPort  = %i", local->m_srcPort);
  gDebug(this, "W: dstPort  = %i", local->m_dstPort);

  return m_layer3->write(local->m_dstAddr, frame);
}

/**********************************************************************************************/

Void G4Transport :: writeChoke()
{
  gDebug(this, "W: >>CONGESTION<< detected, broadcasting >>CHOKE<<");

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
  {
    G4TransportL* connection = it.value();

    gDebug(this, "W: choking %x(:%i)", connection->m_dstAddr, connection->m_dstPort);
    if (connection->m_state == G4TransportL::Established)
      m_layer3->writeChoke(connection->m_dstAddr);
  }
}

/********************************************* TX *********************************************/
/*                                        G4TransportL                                        */
/**********************************************************************************************/

Vacuum G4TransportL :: G4TransportL(G4Transport* global, GSocket* socket, Int srcPort, U4 dstAddr, Int dstPort):
  m_global(global), m_parent(0),
  m_socket(socket), m_retryTimer(3000, global->m_machine->core()), m_state(Invalid),
  m_srcPort(srcPort), m_dstAddr(dstAddr), m_dstPort(dstPort),
  m_output0(Transport),
  m_congestionWindow(1), m_receiveWindow0(0), m_receiveWindow(0),
  m_srcSeq(0), m_dstSeq(0),
  m_numRefs(1),
  m_timeout0(ULONG_LONG_MAX),
  m_timeout1(ULONG_LONG_MAX),
  m_expGrowth(true)
{
  if (dstPort == -1)
    m_global->m_listeners.insert(srcPort, this);
  else
    m_global->m_connections.insert(GDescriptor(srcPort, dstAddr, dstPort), this);

  printf("INTL %p\n", this);
  m_retryTimer << [=]()
  {
    retransmit();
    timeoutCheck();
  };
}

/**********************************************************************************************/

Vacuum G4TransportL :: ~G4TransportL()
{
  printf("DESTL %p\n", this);

  purgeGlobal();
  purgeSocket();
}

/**********************************************************************************************/

G4TransportL* G4TransportL :: accept()
{
  G4Transport::Unclaimed& unclaimed = m_global->m_unclaimed;

  while (!unclaimed.contains(m_srcPort))
    m_monitor.wait();

  return unclaimed.take(m_srcPort);
}

/**********************************************************************************************/

Bool G4TransportL :: acceptSequence(U4 seq)
{
  if (seq == m_srcSeq + 1)
  {
    m_srcSeq = seq;
    return true;
  }
  return false;
}

/**********************************************************************************************/

Void G4TransportL :: ackOutput(U4 seq)
{
  //printf("ACKING OUTPUT with=%i\n", seq);
  Int size0 = m_output.size();

  for (auto it = m_output.begin(); it != m_output.end(); )
  {
    if (it.key() <= seq)
      it = m_output.erase(it);
    else
      break;
  }

  if (size0 != m_output.size())
    m_monitor.notify();

  /*printf("NOW OUTPUT CONTAINS: %i\n", m_output.size());
  for (auto it = m_output.begin(); it != m_output.end(); ++it)
    printf("  * %i\n", it.key());*/
}

/**********************************************************************************************/

Void G4TransportL :: choke()
{
  gDebug(this, "R: >>CHOKE<< connection %x:%i", m_dstAddr, m_dstPort);
  m_congestionWindow  = 1;
  m_expGrowth         = false;
}

/**********************************************************************************************/

Void G4TransportL :: close()
{
  G_RETAIN(this);

  if (m_state == Established)
  {
    setState(FinSent);
    writeControl(FIN | RACK);
  }

  while (m_state != Closed)
    m_monitor.wait();
}

/**********************************************************************************************/

Void G4TransportL :: congestionDec()
{
  m_congestionWindow /= 2;
  m_expGrowth         = false;

  if (m_congestionWindow < 1)
    m_congestionWindow = 1;
}

/**********************************************************************************************/

Void G4TransportL :: congestionInc()
{
  U2    congestionWindow0 = m_congestionWindow;
  Bool  limited           = false;

  if (m_expGrowth)
    m_congestionWindow *= 2;
  else
    m_congestionWindow += 1;

  if (m_congestionWindow > 1000)
  {
    m_congestionWindow  = 1000;
    limited             = true;
  }

  gDebug(this, "R: cgWindow++ = %i -> %i %s %s",
    congestionWindow0, m_congestionWindow,
    m_expGrowth  ? "[EXP]" : "[LIN]",
    limited      ? "[LIM]" : "");
}

/**********************************************************************************************/

Bool G4TransportL :: connect()
{
  G_RETAIN(this);

  if (!writeControl(SYN | RACK))
    return false;

  //timeoutBegin(G_SEC(50));
  for (m_state = SynSent; m_state != Established; )
  {
    if (m_state == Closed)
    {
      gDebug(this, "unable to connect...");
      return false;
    }

    m_monitor.wait();
  }
  //timeoutEnd();

  return true;
}

/**********************************************************************************************/

GFrame* G4TransportL :: firstFrame()
{
  if (m_input.empty())
    return 0;

  if (m_input.begin().key() > m_srcSeq)
    return 0;

  return &m_input.begin().value();
}

/**********************************************************************************************/

const char* G4TransportL :: flagName(U1 flag)
{
  static char buffer[64];

  sprintf(buffer, "%s %s %s %s",
    flag & ACK  ? "ACK"   : "",
    flag & SYN  ? "SYN"   : "",
    flag & FIN  ? "FIN"   : "",
    flag & RACK ? "RACK"  : "");
  return buffer;
}

/**********************************************************************************************/

Void G4TransportL :: flush()
{
  while ((m_output.size() > m_congestionWindow) || (m_receiveWindow == 0))
    m_monitor.wait();
  m_receiveWindow--;
  writeFrame(ACK | RACK);
}

/**********************************************************************************************/

Void G4TransportL :: purgeGlobal()
{
  G_GUARD(m_global, Vacuum);

  m_global->removeLocal(this);
  m_global            = 0;
}

/**********************************************************************************************/

Void G4TransportL :: purgeSocket()
{
  G_GUARD(m_socket, Vacuum);

  m_socket->setOpenMode(QIODevice::NotOpen);
  m_socket->m_local   = 0;
  m_socket            = 0;
}

/**********************************************************************************************/

Int G4TransportL :: read(U1* data, U4 length)
{
  G_GUARD(m_state == Established, 0);
  G_RETAIN(this);

  U4 total = 0;

  while ((length > 0) && (m_state == Established))
  {
    if (GFrame* frame = firstFrame())
    {
      if (frame->size() == 0)
      {
        m_input.erase(m_input.begin());
        continue;
      }

      Int       block = qMin(length, frame->size());

      frame->readFront(data, block);
      data      += block;
      total     += block;
      length    -= block;
    }
    else if (total == 0)
    {
      if (!waitForData())
        break;
    }
    else
      break;
  }

  return total;
}

/**********************************************************************************************/

Void G4TransportL :: readFin(U4 seq)
{
  switch (m_state)
  {
    case Established:
    case SynReceived:
    case SynSent:
    {
      setState(FinReceived);
      writeControl(ACK | FIN | RACK); // timeout this
      break;
    }

    case FinSent:
      if (acceptSequence(seq))
      {
        m_dstSeq++;
        writeControl(ACK);
        setClosed();
      }
      break;

    default:
      printf("ERROR: received FIN in incorrect state %i\n", m_state);
      break;
  }
}

/**********************************************************************************************/

Void G4TransportL :: readFrame(G4Header& header, GFrame& frame)
{
  G_GUARD(m_state != Closed,    Vacuum);
  G_GUARD(m_state != Invalid,   Vacuum);
  G_RETAIN(this);

  m_timeout0 = GFiber::time() + G_SEC(g_4_softTimeout);
  m_timeout1 = m_timeout0     + G_SEC(g_4_hardTimeout);

  gDebug(this, "R: state      = %s", stateName(m_state).constData());

  if (header.sequence >= m_srcSeq)
    setReceiveWindow(header.ack, header.receiveWindow);

  if (header.flag & ACK)
  {
    ackOutput(header.ack);
    congestionInc();
  }

  switch (m_state)
  {
    case Established:   readFrameEstablished (header, frame);  break;
    case FinReceived:   readFrameFinReceived (header.sequence, frame);  break;
    case FinSent:       readFrameFinSent     (header, frame);  break;
    case SynReceived:   readFrameSynReceived (header, frame);  break;
    case SynSent:       readFrameSynSent     (header, frame);  break;

    default:
      gDebug(m_global, "R:ERROR: tried to read frame in invalid state %i\n", m_state);
      break;
  }

  if (header.flag & FIN)
    readFin(header.sequence);
}

/**********************************************************************************************/

Void G4TransportL :: readFrameEstablished(G4Header& header, GFrame& frame)
{
  G_GUARD(header.flag & RACK, Vacuum);

  if (header.sequence <= m_srcSeq)
  {
    gDebug(this, "R: re-sending ACK (fsize = %i)", frame.size());
    writeControl(ACK);
    return;
  }

  m_receiveWindow0--;

  if (m_input.contains(header.sequence))
    return;

  U4 srcSeq1 = m_srcSeq;

  for (auto it = m_input.insert(header.sequence, frame); it != m_input.end(); ++it)
  {
    if (it.key() == srcSeq1 + 1)
      srcSeq1++;
    else
      break;
  }

  // new frame block
  if (srcSeq1 != m_srcSeq)
  {
    gDebug(this, "R: sending ACK");
    m_srcSeq = srcSeq1;
    writeControl(ACK);
    m_monitor.notify();
  }
}

/**********************************************************************************************/

Void G4TransportL :: readFrameFinReceived(U4 seq, GFrame& frame)
{
  if (acceptSequence(seq))
  {
    gDebug(this, "R: received ACK for FIN+ACK");
    setClosed();
  }
}

/**********************************************************************************************/

Void G4TransportL :: readFrameFinSent(G4Header& header, GFrame& frame)
{
  if (!(header.flag & FIN) && (header.ack == m_dstSeq))
  {
    gDebug(this, "R: received ACK for FIN");
    setClosed();
  }
}

/**********************************************************************************************/

Void G4TransportL :: readFrameSynReceived(G4Header& header, GFrame& frame)
{
  // accept
  //if (header.sequence == m_srcSeq + 1)
  if ((header.sequence > m_srcSeq) && (header.sequence < m_srcSeq + g_4_ackOfSyn2Dilation))
  {
    m_global->m_unclaimed.insert(m_srcPort, this);
    setState(Established);
    m_parent->m_monitor.notify();
    readFrameEstablished(header, frame);
    m_srcSeq = header.sequence;
  }
  else
  {
    gDebug(this, "R:SERVER: invalid seq received: got=%i, expected=%i, state=SynReceived",
      header.sequence, m_srcSeq + 1);
  }
}

/**********************************************************************************************/

Void G4TransportL :: readFrameSynSent(G4Header& header, GFrame& frame)
{
  if (m_dstSeq == header.ack)
  {
    setState(Established);
    m_dstSeq += 1;
    m_srcSeq  = header.sequence;
    writeControl(ACK);
    m_monitor.notify();
  }
  else
  {
    gDebug(this, ">>LOST ACK RECEIVED<< (expected=%i, got=%i)", m_dstSeq, header.ack);
  }
}

/**********************************************************************************************/

Void G4TransportL :: release()
{
  if (--m_numRefs == 0)
    gCollect(this);
}

/**********************************************************************************************/

Void G4TransportL :: retransmit()
{
  Time  time          = GFiber::time();
  bool  retransmitted = false;

  gDebug(this, "FIRE TIMER %p", this);

  for (auto it = m_output.begin(); it != m_output.end(); ++it)
  {
    OFrame& frame = *it;

    if ((time - frame.time) < G_MSEC(g_4_retransmitTimeout))
      continue;

    gSetContext(frame.frame.context());

    if (frame.numRetries <= 0)
    {
      gDebug(this, "W: maximum retransmit count reached for frame");
      return setClosed();
    }
    else
    {
      gDebug(this, "W: retransmitting frame: numRetries = %i", frame.numRetries);
      frame.numRetries -= 1;
      frame.time        = time;
      retransmitted     = true;
      writeFrameEx(frame.frame, it.key());
    }
  }

  if (retransmitted)
    congestionDec();
}

/**********************************************************************************************/

Void G4TransportL :: setClosed()
{
  G_GUARD(m_state != Closed, Vacuum);

  setState(Closed);
  purgeSocket();
  m_monitor.notify();
  release();
}

/**********************************************************************************************/

Void G4TransportL :: setSocket(GSocket* socket)
{
  m_socket = socket;
}

/**********************************************************************************************/

Void G4TransportL :: setReceiveWindow(U4 ack, U2 receiveWindow)
{
  U2  window = qMax(0, int(ack + receiveWindow - m_dstSeq));
  //gDebug(this, "SRW %i %i %i -> %i", m_dstSeq, ack, receiveWindow, window);
  G_GUARD(m_receiveWindow != window, Vacuum);

  if (m_receiveWindow == 0)
    m_monitor.notify();
  m_receiveWindow = window;
}

/**********************************************************************************************/

Void G4TransportL :: setState(int newState)
{
  gDebug(this, "%s => %s", stateName(m_state).constData(), stateName(newState).constData());
  m_state = newState;
}  Bool  requested   = false;

/**********************************************************************************************/

QByteArray G4TransportL :: stateName(int state)
{
  switch (state)
  {
    case Invalid:     return "Invalid";
    case Listening:   return "Listening";
    case SynSent:     return "SynSent";
    case SynReceived: return "SynReceived";
    case Established: return "Established";
    case FinSent:     return "FinSent";
    case FinReceived: return "FinReceived";
    case Closed:      return "Closed";
  }

  return "[Unknown]";
}

/**********************************************************************************************/

Void G4TransportL :: timeoutCheck()
{
  Time time = GFiber::time();

  if ((m_timeout0 < time) && (m_state == Established))
  {
    m_timeout0 = ULONG_LONG_MAX;
    gDebug(this, "soft timeout");
    writeControl(ACK | RACK);
  }

  if (m_timeout1 < time)
  {
    gDebug(this, "hard timeout");
    setClosed();
  }
}

/**********************************************************************************************/

Bool G4TransportL :: waitForData()
{
  if (m_receiveWindow0 == 0)
  {
    gDebug(this, "requiesting more data with ACK+RACK");
    writeControl(ACK | RACK);
  }

  m_monitor.wait();
  return m_state != Closed;
}

/**********************************************************************************************/

Bool G4TransportL :: waitForRead()
{
  while (m_receiveWindow == 0)
  {
    m_monitor.wait();
    if (m_state == Closed)
      return false;
  }
  return true;
}

/**********************************************************************************************/

Int G4TransportL :: write(const U1* data, U4 length)
{
  G_RETAIN(this);
  G_GUARD(waitForRead(), 0);

  while (length > 0)
  {
    U4 block = qMin(length, G4_MAX_DATA_SIZE - m_output0.size());

    m_output0.writeBack(data, block);
    data      += block;
    length    -= block;

    if (m_output0.size() == G4_MAX_DATA_SIZE)
      flush();
  }

  return 0;
}

/**********************************************************************************************/

Bool G4TransportL :: writeControl(U2 flags)
{
  return writeFrame(flags);
}

/**********************************************************************************************/

Bool G4TransportL :: writeFrame(U2 flags, Int numRetries)
{
  if (numRetries == -1)
    numRetries = g_4_retransmitRetries;

  gSetContext(m_output0.context());
  gDebug(this, "W: [window] = %i",    m_receiveWindow);
  gDebug(this, "W: flag     = %x %s", flags, flagName(flags));

  m_output0.writeFront1(flags);       // 14-15 flag

  if (flags & RACK)
  {
    m_output.insert(++m_dstSeq, {GFiber::time(), numRetries, m_output0});
  }

  Bool result = writeFrameEx(m_output0, m_dstSeq);

  m_output0 = GFrame(Transport);

  return result;
}

/**********************************************************************************************/

Bool G4TransportL :: writeFrameEx(GFrame frame, U4 seq)
{
  gDebug(this, "W: seq      = %i", seq);
  gDebug(this, "W: ack      = %i", m_srcSeq);

  if (!m_input.empty())
  {
    int seq0    = m_input.begin().key();
    int window  = seq0 + 5 - m_srcSeq;
    m_receiveWindow0 = window > 0 ? window : 0;

    gDebug(this, "W: window   = %i (seq=%i, srcSeq=%i)", m_receiveWindow0, seq0, m_srcSeq);
  }
  else
  {
    m_receiveWindow0 = 5;
    gDebug(this, "W: window   = %i", m_receiveWindow0);
  }

  frame.writeFront2(m_receiveWindow0);    // 12-16 rcv window
  frame.writeFront4(m_srcSeq);            //  8-12 ack
  frame.writeFront4(seq);                 //  4- 8 seq

  return m_global->write(this, frame);
}

/**********************************************************************************************/
