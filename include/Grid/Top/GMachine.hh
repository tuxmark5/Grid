#ifndef GRID_TOP_GMACHINE_HH
#define GRID_TOP_GMACHINE_HH

/**********************************************************************************************/
#include <Grid/Util/GMonitor.hh>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QRunnable>
/********************************************* TX *********************************************/
/*                                          GMachine                                          */
/**********************************************************************************************/

class GMachine
{
  private:
    ___FLD QByteArray               m_name;
    ___FLD QHash<int, GProcess*>    m_processes;
    ___FLD bool                     m_destroying: 1;

    ___FLD G4Transport*             m_layer4;

    ___FLD GFiber*                  m_core;
    ___FLD GMonitor                 m_omon;
    ___FLD QList<GFrame>            m_oqueue;

  public:
    ___MET Vacuum                   GMachine(const QByteArray& name);
    ___MET Vacuum                   GMachine(const GMachine& other);
    ___MET Vacuum                   ~GMachine();
    ___MET GFiber*                  core() const { return m_core; }
    ___MET QList<G2DataLink*>       layer2() const;
    ___MET G4Transport*             layer4() const { return m_layer4; }
    ___MET QByteArray               name() const { return m_name; }
    __sMET Void                     printRoutingTables();
    ___MET Void                     removeProcess(GProcess* process);
    ___MET Void                     send(GFrame& frame);
    ___MET GInterface               operator () (U8 mac, U4 address, U1 length);
    ___MET GMachine&                operator << (std::function<void()> process);
    ___MET GMachine&                operator << (G5App* process);

  private:
    ___MET Void                     run();
};

/**********************************************************************************************/
GMachine& machine(const QByteArray& name);
/**********************************************************************************************/

#endif /* GRID_TOP_GMACHINE_HH */
