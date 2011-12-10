#ifndef GRID_UTIL_GTIMER_HH
#define GRID_UTIL_GTIMER_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                           GTimer                                           */
/**********************************************************************************************/

class GTimer
{
  public:
    FRIEND class GFiber;

  protected:
    ___FLD Time             m_time;
    ___FLD GFiber*          m_fiber;
    ___FLD GTimer*          m_next;
    ___FLD Bool             m_active: 1;

  public:
    ___MET Vacuum           GTimer(GFiber* fiber = 0);
    ___MET Vacuum           GTimer(const GTimer& timer) = delete;
    __vMET Vacuum           ~GTimer();
    ___MET Void             stop();
    ___MET Time             time() const { return m_time; }

  protected:
    ___MET Void             start();
    __vMET Void             timeout();
};

/**********************************************************************************************/

#endif /* GRID_UTIL_GTIMER_HH */
