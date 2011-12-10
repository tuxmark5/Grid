#ifndef GRID_GFIBER_HH
#define GRID_GFIBER_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
#include <QtCore/QList>
#include <ucontext.h>
/********************************************* TX *********************************************/
/*                                           GFiber                                           */
/**********************************************************************************************/

class GFiber
{
  public:
    FRIEND class GMonitor;
    FRIEND class GTimer;

  public:
    __TYPE std::function<bool()>  Condition;
    __TYPE std::function<void()>  Function;

  public:
    __sFLD Time                 s_time;
    __sFLD GFiber*              s_fiber0;
    __sFLD GFiber*              s_fiber1;
    __sFLD GFiber*              s_waiting;
    __sFLD ucontext_t           s_main;

    ___FLD GFiber*              m_prev;
    ___FLD GFiber*              m_next;
    ___FLD ucontext_t           m_context;

    ___FLD bool                 m_suspended: 1;
    ___FLD Time                 m_clock;
    ___FLD GMonitor*            m_monitor;
    ___FLD GFiber*              m_monitorNext;
    ___FLD GTimer*              m_timer;

    ___FLD Function             m_function;
    ___FLD U8 id;

  public:
    ___MET Vacuum               GFiber(Function function, int stack = 4096);
    __vMET Vacuum               ~GFiber();
    __sMET GFiber*              current() { return s_fiber0; }
    __sMET Void                 exec();
    ___MET GMonitor*            monitor() const { return m_monitor; }
    ___MET Void                 resume();
    __sMET Void                 sleep(Int msecs);
    ___MET Void                 suspend();
    __sMET Time                 time();
    __sMET Void                 yield();

  private:
    ___MET Void                 activate();
    ___MET Void                 dequeue(GFiber*& queue);
    ___MET Void                 enqueue(GFiber*& queue);
    ___MET Void                 enqueueWaiting();
    __sMET Void                 run(GFiber* self);
};

/**********************************************************************************************/

#endif /* GRID_GFIBER_HH */
