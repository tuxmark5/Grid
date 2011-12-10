#ifndef GRID_UTIL_GFUN_TIMER_HH
#define GRID_UTIL_GFUN_TIMER_HH

/**********************************************************************************************/
#include <Grid/Util/GTimer.hh>
/********************************************* TX *********************************************/
/*                                         GFunTimer                                          */
/**********************************************************************************************/

class GFunTimer: public GTimer
{
  public:
    __TYPE std::function<void()>  Callback;

  private:
    ___MET Callback         m_callback;
    ___MET Int              m_interval;
    ___MET Bool             m_singleShot;

  public:
    ___MET Vacuum           GFunTimer(Int interval = -1, GFiber* fiber = 0);
    __vMET Vacuum           ~GFunTimer();
    ___MET Callback&        restart(Int interval = -1);
    ___MET Void             start(Int interval = -1);
    ___MET Void             operator << (Callback&& callback);

  protected:
    ___MET Void             timeout();
};

/**********************************************************************************************/

#endif /* GRID_UTIL_GFUN_TIMER_HH */
