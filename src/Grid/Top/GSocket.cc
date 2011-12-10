#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Bottom/GProcess.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GSocket.hh>

/********************************************* TX *********************************************/
/*                                          GSocket                                           */
/**********************************************************************************************/

Vacuum GSocket :: GSocket(GProcess* process, G4TransportL* local):
  m_process(process),
  m_local(local)
{
  m_local->setSocket(this);
  setOpenMode(ReadWrite);
}

/**********************************************************************************************/

Vacuum GSocket :: GSocket(GProcess* process, Int srcPort, U4 dstAddr, Int dstPort):
  m_process(process),
  m_local(new G4TransportL(process->machine()->layer4(), this, srcPort, dstAddr, dstPort))
{
}

/**********************************************************************************************/

Vacuum GSocket :: ~GSocket()
{
  if (m_local)
  {
    m_local->setSocket(0);
    delete m_local;
  }
}

/**********************************************************************************************/

GSocket* GSocket :: accept()
{
  if (G4TransportL* local = m_local->accept())
    return new GSocket(GProcess::current(), local);
  return 0;
}

/**********************************************************************************************/

Void GSocket :: close()
{
  G_GUARD(m_local, Vacuum);

  setOpenMode(NotOpen);
  m_local->setSocket(0);
  m_local->close();
  m_local = 0;
}

/**********************************************************************************************/

GSocket* GSocket :: connect(U4 dstAddr, U2 dstPort)
{
  if (GProcess* process = GProcess::current())
  {
    Int             srcPort = process->machine()->layer4()->newSrcPort();
    G4TransportL*   local   = new G4TransportL(process->machine()->layer4(), 0, srcPort, dstAddr, dstPort);

    if (local->connect())
      return new GSocket(process, local);
    delete local;
  }
  return 0;
}

/**********************************************************************************************/

Void GSocket :: flush()
{
  G_GUARD(m_local, Vacuum);

  m_local->flush();
}

/**********************************************************************************************/

qint64 GSocket :: readData(char* data, qint64 maxSize)
{
  G_GUARD(m_local, -1);

  return m_local->read((U1*) data, maxSize);
}

/**********************************************************************************************/

GSocket* GSocket :: serve(U2 srcPort)
{
  if (GProcess* process = GProcess::current())
    if (!process->machine()->layer4()->listener(srcPort))
      return new GSocket(process, srcPort);

  return 0;
}

/**********************************************************************************************/

qint64 GSocket :: writeData(const char* data, qint64 maxSize)
{
  G_GUARD(m_local, -1);

  return m_local->write((const U1*) data, maxSize);
}

/**********************************************************************************************/
