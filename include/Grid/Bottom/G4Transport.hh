#ifndef GRID_BOTTOM_G4_TRANSPORT_HH
#define GRID_BOTTOM_G4_TRANSPORT_HH

/**********************************************************************************************/
#include <Grid/Bottom/G3Network.hh>
#include <Grid/Util/GFunTimer.hh>
#include <Grid/Util/GMonitor.hh>
#include <QtCore/QHash>
/**********************************************************************************************/
#define G4_HEAD_SIZE        17
#define G4_META_SIZE        G4_HEAD_SIZE
/**********************************************************************************************/
#define G4_MAX_DATA_SIZE    (G3_MAX_DATA_SIZE - G4_META_SIZE)
/**********************************************************************************************/
typedef std::tuple<Port, Address, Port> GDescriptor;
/********************************************* TX *********************************************/
/*                                          G4Header                                          */
/**********************************************************************************************/

enum G4Flag
{
  ACK   = 0x01,
  SYN   = 0x02,
  FIN   = 0x04,
  RACK  = 0x08
};

struct G4Header
{
  Port    remotePort;
  Port    localPort;
  U4      sequence;
  U4      ack;
  U2      receiveWindow;
  U1      flag;
};

/********************************************* TX *********************************************/
/*                                        G4Transport                                         */
/**********************************************************************************************/

class G4Transport
{
  public:
    FRIEND class G4TransportL;

  public:
    __TYPE QHash<GDescriptor, G4TransportL*>    Connections;
    __TYPE QHash<Port, G4TransportL*>           Listeners;
    __TYPE QMultiHash<Port, G4TransportL*>      Unclaimed;

  private:
    ___FLD GMachine*      m_machine;
    ___FLD G3Network*     m_layer3;
    ___FLD Connections    m_connections;
    ___FLD Listeners      m_listeners;
    ___FLD Unclaimed      m_unclaimed;

  public:
    ___MET Vacuum         G4Transport(GMachine* machine);
    ___MET Vacuum         ~G4Transport();
    ___MET Bool           isPortFree(Port port) const;
    ___MET G3Network*     layer3() const { return m_layer3; }
    ___MET G4TransportL*  listener(Port srcPort) const { return m_listeners.value(srcPort); }
    ___MET GMachine*      machine() const { return m_machine; }
    ___MET Int            newSrcPort();
    ___MET Bool           read(Address src, GFrame& frame);
    ___MET Void           removeProcess(GProcess* process);
    ___MET Bool           write(G4TransportL* local, GFrame& frame);

  private:
    ___MET Void           removeLocal(G4TransportL* local);
};

/********************************************* TX *********************************************/
/*                                        G4TransportL                                        */
/**********************************************************************************************/

class G4TransportL: public GObject
{
  public:
    FRIEND class          G4Transport;

  public:
    __ENUM State
    {
      Invalid,
      Listening,
      SynSent,
      SynReceived,
      Established,
      FinSent,
      FinReceived,
      Closed
    };

  public:
    struct OFrame
    {
      Time    time;
      int     numRetries;
      GFrame  frame;
    };

  public:
    __TYPE QMap<int, GFrame>    IFrameMap;
    __TYPE QMap<int, OFrame>    OFrameMap;

  private:
    ___FLD G4Transport*   m_global;
    ___FLD G4TransportL*  m_parent;
    ___FLD GSocket*       m_socket;
    ___FLD GMonitor       m_monitor;
    ___FLD GFunTimer      m_retryTimer;
    ___FLD U1             m_state;

    ___FLD Int            m_srcPort;
    ___FLD Address        m_dstAddr;
    ___FLD Int            m_dstPort;

    ___FLD IFrameMap      m_input;
    ___FLD OFrameMap      m_output;
    ___FLD GFrame         m_output0;

    ___FLD U4             m_srcSeq;
    ___FLD U2             m_receiveWindow0;
    ___FLD U2             m_receiveWindow;
    ___FLD U4             m_dstSeq;
    ___FLD U4             m_dstWindow;
    ___MET I1             m_numRefs;
    ___MET Time           m_timeout0;
    ___MET Time           m_timeout1;

  public:
    ___MET Vacuum         G4TransportL(G4Transport* global, GSocket* socket, Int srcPort, U4 dstAddr, Int dstPort);
    ___MET Vacuum         ~G4TransportL();
    ___MET G4TransportL*  accept();
    ___MET Void           close();
    ___MET Bool           connect();
    ___MET Address        dstAddr() const { return m_dstAddr; }
    ___MET Port           dstPort() const { return m_dstPort; }
    ___MET GFrame*        firstFrame();
    ___MET Void           flush();
    ___MET G4Transport*   global() const { return m_global; }
    ___MET Bool           isClosed() const { return m_state == Closed; }
    ___MET Void           purgeGlobal();
    ___MET Void           purgeSocket();
    ___MET Int            read(U1* data, U4 length);
    ___MET Void           release();
    ___MET Void           retain() { m_numRefs++; }
    ___MET Void           setSocket(GSocket* socket);
    ___MET Port           srcPort() const { return m_srcPort; }
    __sMET QByteArray     stateName(int state);
    ___MET Int            write(const U1* data, U4 length);

  private:
    ___MET Bool           acceptSequence(U4 seq);
    ___MET Void           ackOutput(U4 seq);
    ___MET Void           readFin(U4 seq);
    ___MET Void           readFrame(G4Header& header, GFrame& frame);
    ___MET Void           readFrameEstablished(G4Header& header, GFrame& frame);
    ___MET Void           readFrameFinReceived(U4 seq, GFrame& frame);
    ___MET Void           readFrameFinSent(G4Header& header, GFrame& frame);
    ___MET Void           readFrameSynReceived(G4Header& header, GFrame& frame);
    ___MET Void           readFrameSynSent(G4Header& header, GFrame& frame);
    ___MET Void           retransmit();
    ___MET Void           setClosed();
    ___MET Void           setReceiveWindow(U4 ack, U2 receiveWindow);
    ___MET Void           setState(int newState);
    ___MET Void           timeoutBegin(Time time);
    ___MET Void           timeoutCheck();
    ___MET Void           timeoutEnd();
    ___MET Bool           waitForData();
    ___MET Bool           waitForRead();
    ___MET Bool           writeControl(U2 flags = ACK);
    ___MET Bool           writeFrame(U2 flags = ACK, Int numRetries = 10);
    ___MET Bool           writeFrameEx(GFrame frame, U4 seq);
};

/**********************************************************************************************/

#endif /* GRID_BOTTOM_G4_TRANSPORT_HH */
