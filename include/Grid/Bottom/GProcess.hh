#ifndef GRID_BOTTOM_GPROCESS_HH
#define GRID_BOTTOM_GPROCESS_HH

/**********************************************************************************************/
#include <Grid/GFiber.hh>
#include <QtCore/QHash>
/********************************************* TX *********************************************/
/*                                          GProcess                                          */
/**********************************************************************************************/

class GProcess: public GFiber
{
  public:
    FRIEND class          GMachine;

  private:
    ___FLD GMachine*      m_machine;
    ___FLD int            m_pid;

  public:
    ___MET Vacuum         GProcess(GMachine* machine, int pid, std::function<void()> function);
    __vMET Vacuum         ~GProcess();
    __sMET GProcess*      current();
    ___MET GMachine*      machine() const { return m_machine; }
    ___MET int            pid() const { return m_pid; }
};

/**********************************************************************************************/

#endif /* GRID_BOTTOM_GPROCESS_HH */
