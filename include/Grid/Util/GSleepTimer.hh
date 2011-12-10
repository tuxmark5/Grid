#ifndef GRID_UTIL_GSLEEP_TIMER_HH
#define GRID_UTIL_GSLEEP_TIMER_HH

/**********************************************************************************************/
#include <Grid/Util/GTimer.hh>
/********************************************* TX *********************************************/
/*                                        GSleepTimer                                         */
/**********************************************************************************************/

class GSleepTimer: public GTimer
{
  public:
    ___MET Vacuum           GSleepTimer();
    ___MET Void             start(Int msecs);

  protected:
    __vMET Void             timeout();
};

/**********************************************************************************************/

#endif /* GRID_UTIL_GSLEEP_TIMER_HH */
