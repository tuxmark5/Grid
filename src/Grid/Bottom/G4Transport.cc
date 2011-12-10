#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GSocket.hh>
#include <Grid/Util/GDebug.hh>
#include <Grid/Util/GRetainer.hh>
#include <Grid/GFiber.hh>

#define G_MSEC(x)   ((x) *    1000000ULL)
#define G_SEC(x)    ((x) * 1000000000ULL)
/**********************************************************************************************/
int   g_4_congestionWindow  = 3;    // 3
int   g_4_receiveWindow     = 10;   //
int   g_4_retransmitRetries = 10;
int   g_4_retransmitTimeout = 1000;
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
  header.receiveWindow    = frame.readFront2();  // 12-14
  header.flag             = frame.readFront1();  // 14-15
  GDescriptor   descriptor  = GDescriptor(header.localPort, l3_src, header.remotePort);

  gDebug(this, "R: remotePort = %i", header.remotePort);
  gDebug(this, "R: localPort  = %i", header.localPort);
  gDebug(this, "R: seq        = %i", header.sequence);
  gDebug(this, "R: ack        = %i", header.ack);
  gDebug(this, "R: window     = %i", header.receiveWindow);
  gDebug(this, "R: flag       = %x", header.flag);

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

/********************************************* TX *********************************************/
/*                                        G4TransportL                                        */
/**********************************************************************************************/

Vacuum G4TransportL :: G4TransportL(G4Transport* global, GSocket* socket, Int srcPort, U4 dstAddr, Int dstPort):
  m_global(global), m_parent(0),
  m_socket(socket), m_retryTimer(3000, global->m_machine->core()), m_state(Invalid),
  m_srcPort(srcPort), m_dstAddr(dstAddr), m_dstPort(dstPort),
  m_output0(0),
  m_srcSeq(0), m_receiveWindow(0), m_receiveWindow0(0),
  m_dstSeq(0), m_dstWindow(0),
  m_numRefs(1),
  m_timeout(ULONG_LONG_MAX)
{
  if (dstPort == -1)
    m_global->m_listeners.insert(srcPort, this);
  else
    m_global->m_connections.insert(GDescriptor(srcPort, dstAddr, dstPort), this);

  printf("INTL %p\n", this);
  m_retryTimer << [=]()
  {
    //GRetainer<G4TransportL> r(this);
    retransmit();
    //timeoutCheck();
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

Void G4TransportL :: close()
{
  GRetainer<G4TransportL> r(this);

  if (m_state == Established)
  {
    setState(FinSent);
    writeControl(FIN | RACK);
  }

  while (m_state != Closed)
    m_monitor.wait();
}

/**********************************************************************************************/

Bool G4TransportL :: connect()
{
  //GRetainer<G4TransportL> r(this);

  if (!writeControl(SYN | RACK))
    return false;

  //timeoutBegin(G_SEC(50));
  for (m_state = SynSent; m_state != Established; )
  {
    /*if (m_state == Closed)
    {
      gDebug(this, "unable to connect...");
      return false;
    }*/

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

Void G4TransportL :: flush()
{
  while ((m_output.size() > g_4_congestionWindow) || (m_receiveWindow == 0))
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

  auto  r           = GRetainer<G4TransportL>(this);
  U4    total       = 0;

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
  GRetainer<G4TransportL> r(this);

  gDebug(this, "R: state      = %s", stateName(m_state).constData());

  if (header.sequence >= m_srcSeq)
    setReceiveWindow(header.receiveWindow);

  if (header.flag & ACK)
    ackOutput(header.ack);

  switch (m_state)
  {
    case Established:   readFrameEstablished (header, frame);  break;
    case FinReceived:   readFrameFinReceived (header.sequence, frame);  break;
    case FinSent:       readFrameFinSent     (header, frame);           break;
    case SynReceived:   readFrameSynReceived (header, frame);  break;
    case SynSent:       readFrameSynSent     (header.sequence, frame);  break;

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
  if (header.sequence == m_srcSeq + 1)
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

Void G4TransportL :: readFrameSynSent(U4 seq, GFrame& frame)
{
  // connect
  setState(Established);
  m_dstSeq += 1;
  m_srcSeq  = seq;
  writeControl(ACK);
  m_monitor.notify();
}

/**********************************************************************************************/

Void G4TransportL :: release()
{
  if (--m_numRefs == 0)
    delete this;
}

/**********************************************************************************************/

Void G4TransportL :: retransmit()
{
  Time time = GFiber::time();

  gDebug(this, "FIRE TIMER %p", this);

  for (auto it = m_output.begin(); it != m_output.end(); ++it)
  {
    OFrame& frame = *it;

    if ((time - frame.time) < G_MSEC(g_4_retransmitTimeout))
      continue;

    gSetContext(frame.frame.context());

    if (frame.numRetries > g_4_retransmitRetries)
    {
      gDebug(this, "W: maximum retransmit count reached for frame = %i", it.key());
      return setClosed();
    }
    else
    {
      gDebug(this, "W: retransmitting frame: numRetries = %i/10", frame.numRetries);
      frame.numRetries += 1;
      frame.time        = time;
      writeFrameEx(frame.frame, it.key());
    }
  }
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

Void G4TransportL :: setReceiveWindow(U2 receiveWindow)
{
  G_GUARD(m_receiveWindow != receiveWindow, Vacuum);

  if (m_receiveWindow == 0)
    m_monitor.notify();
  m_receiveWindow = receiveWindow;
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

Void G4TransportL :: timeoutBegin(Time time)
{
  m_timeout = GFiber::time() + time;
}

/**********************************************************************************************/

Void G4TransportL :: timeoutCheck()
{
  if (m_timeout < GFiber::time())
  {
    gDebug(this, "TIMEOUT");
    setClosed();
  }
}

/**********************************************************************************************/

Void G4TransportL :: timeoutEnd()
{
  m_timeout = ULONG_LONG_MAX;
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

Bool G4TransportL :: writeFrame(U2 flags)
{
  gSetContext(m_output0.context());
  gDebug(this, "W: flag     = %x", flags);

  m_output0.writeFront1(flags);       // 14-15 flag

  if (flags & RACK)
  {
    m_output.insert(++m_dstSeq, {GFiber::time(), 0, m_output0});
  }

  Bool result = writeFrameEx(m_output0, m_dstSeq);
  m_output0 = 0;

  return result;
}

/**********************************************************************************************/

Bool G4TransportL :: writeFrameEx(GFrame frame, U4 seq)
{
  m_receiveWindow0 = (m_input.size() > 5) ? 0 : (5 - m_input.size());

  gDebug(this, "W: seq      = %i", seq);
  gDebug(this, "W: ack      = %i", m_srcSeq);
  gDebug(this, "W: window   = %i", m_receiveWindow0);

  frame.writeFront2(m_receiveWindow0);    // 12-14 rcv window
  frame.writeFront4(m_srcSeq);            //  8-12 ack
  frame.writeFront4(seq);                 //  4- 8 seq

  return m_global->write(this, frame);
}

/**********************************************************************************************/