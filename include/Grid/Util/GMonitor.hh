#ifndef GRID_UTIL_GMONITOR_HH
#define GRID_UTIL_GMONITOR_HH

/**********************************************************************************************/
#include <Grid/Util/GTimer.hh>
/********************************************* TX *********************************************/
/*                                          GMonitor                                          */
/**********************************************************************************************/

class GMonitor: public GTimer
{
  protected:
    ___MET GFiber*          m_firstFiber;

  public:
    ___MET Vacuum           GMonitor();
    ___MET Vacuum           ~GMonitor();
    ___MET Void             notify();
    ___MET Void             wait();
    ___MET Void             wait(Int timeout);
};

/**********************************************************************************************/

#endif /* GRID_UTIL_GMONITOR_HH */
