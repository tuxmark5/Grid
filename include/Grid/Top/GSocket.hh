#ifndef GRID_TOP_GSOCKET_HH
#define GRID_TOP_GSOCKET_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
#include <QtCore/QIODevice>
/********************************************* TX *********************************************/
/*                                          GSocket                                           */
/**********************************************************************************************/

class GSocket: public QIODevice
{
  public:
    FRIEND class          GMachine;
    FRIEND class          GProcess;
    FRIEND class          G4TransportL;

  private:
    ___FLD GProcess*      m_process;
    ___FLD G4TransportL*  m_local;

  private:
    ___MET Vacuum         GSocket(GProcess* process, G4TransportL* local);
    ___MET Vacuum         GSocket(GProcess* process, Int srcPort, U4 dstAddr = 0, Int dstPort = -1);

  public:
    __vMET Vacuum         ~GSocket();
    ___MET GSocket*       accept();
    __vMET Void           close();
    ___MET Void           flush();
    ___MET Bool           isConnected() const { return m_local; }
    ___MET GProcess*      process() const { return m_process; }
    __vMET qint64         readData(char* data, qint64 maxSize);
    __vMET qint64         writeData(const char* data, qint64 maxSize);

  public:
    __sMET GSocket*       connect(U4 dstAddr, U2 dstPort);
    __sMET GSocket*       serve(U2 srcPort);
};

/**********************************************************************************************/

#endif /* GRID_TOP_GSOCKET_HH */
