#include <Grid/Util/GDebug.hh>
#include <Grid/Util/GMonitor.hh>
#include <Grid/Util/GSleepTimer.hh>
#include <Grid/GFiber.hh>
#include <valgrind/memcheck.h>

/**********************************************************************************************/
Time        GFiber :: s_time    = 0;
GFiber*     GFiber :: s_fiber0  = 0;
GFiber*     GFiber :: s_fiber1  = 0;
GFiber*     GFiber :: s_waiting = 0;
ucontext_t  GFiber :: s_main;
/********************************************* TX *********************************************/
/*                                           GFiber                                           */
/**********************************************************************************************/

Vacuum GFiber :: GFiber(Function function, int stack):
  m_suspended(false),
  m_clock(0),
  m_monitor(0),
  m_monitorNext(0),
  m_timer(0),
  m_function(std::move(function))
{
  stack *= 3;
  enqueue(s_fiber1);
  getcontext(&m_context);
  m_context.uc_link           = &s_main;
  m_context.uc_stack.ss_sp    = malloc(stack);
  //memset((void*) m_context.uc_stack.ss_sp, 0, sizeof(stack * 20));
  //id = VALGRIND_STACK_REGISTER(m_context.uc_stack.ss_sp, m_context.uc_stack.ss_sp + stack * 2);
  m_context.uc_stack.ss_size  = stack;
  makecontext(&m_context, (void (*)(void)) run, 1, this);
}

/**********************************************************************************************/

Vacuum GFiber :: ~GFiber()
{
  if (m_prev && m_next)
  {
    m_prev->m_next  = m_next;
    m_next->m_prev  = m_prev;
   // VALGRIND_STACK_DEREGISTER(id);
    free(m_context.uc_stack.ss_sp);
  }

  if (s_fiber1 == this)
  {
    s_fiber1 = (m_prev == this) ? 0 : m_prev;
  }

  while (m_timer)
  {
    m_timer->m_fiber    = 0;
    m_timer->m_active   = false;
    m_timer             = m_timer->m_next;
  }
}

/**********************************************************************************************/

Void GFiber :: activate()
{
  G_GUARD(s_fiber0 != this, Vacuum);

  GFiber* prev = s_fiber0;
  GFiber* next = this;

  s_fiber0 = this;
  s_fiber1 = this;
  swapcontext(&prev->m_context, &next->m_context);
}

/**********************************************************************************************/

Void GFiber :: dequeue(GFiber*& queue)
{
  if (m_prev && m_next)
  {
    m_prev->m_next  = m_next;
    m_next->m_prev  = m_prev;
  }

  if (queue == this)
  {
    queue = (m_prev == this) ? 0 : m_prev;
  }

  m_prev = 0;
  m_next = 0;
}

/**********************************************************************************************/

Void GFiber :: enqueue(GFiber*& queue)
{
  if (queue)
  {
    m_prev          = queue;
    m_next          = queue->m_next;
    m_prev->m_next  = this;
    m_next->m_prev  = this;
  }
  else
  {
    m_prev          = this;
    m_next          = this;
    queue           = this;
  }
}

/**********************************************************************************************/

Void GFiber :: enqueueWaiting()
{
  GFiber** self = &s_waiting;

  while ((*self) && (*self)->m_clock < m_clock)
    self = &(*self)->m_next;

  m_next        = *self;
  *self         = this;
}

/**********************************************************************************************/

Void GFiber :: exec()
{
  getcontext(&s_main);

  while (s_fiber1 || s_waiting)
  {
    while (s_fiber1)
    {
      s_fiber0 = s_fiber1;
      swapcontext(&s_main, &s_fiber1->m_context);
      delete s_fiber0;
    }

    if (s_waiting)
    {
      GFiber* active = s_waiting;
      s_waiting = s_waiting->m_next;
      active->enqueue(s_fiber1);
      //printf("ENQ %lli\n", active->m_clock - s_time + 1);
      s_time   += active->m_clock - s_time + 1;
    }
  }
}

/**********************************************************************************************/

Void GFiber :: resume()
{
  m_suspended = false;

  if (s_fiber1)
  {
    m_prev          = s_fiber1;
    m_next          = s_fiber1->m_next;
    m_prev->m_next  = this;
    m_next->m_prev  = this;
  }
  else
  {
    m_prev          = this;
    m_next          = this;
    s_fiber1        = this;
  }
}

/**********************************************************************************************/

Void GFiber :: run(GFiber* self)
{
  self->m_function();
}

/**********************************************************************************************/

Void GFiber :: sleep(Int msecs)
{
  GSleepTimer().start(msecs);
}

/**********************************************************************************************/

Void GFiber :: suspend()
{
  m_suspended = true;
  m_clock     = m_timer ? m_timer->time() : ULONG_LONG_MAX;

  dequeue(s_fiber1);
  enqueueWaiting();

  if (this == s_fiber0)
    yield();
}

/**********************************************************************************************/

Time GFiber :: time()
{
  /*timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return Time(ts.tv_sec) * 1000000000ULL + ts.tv_nsec;*/
  return s_time;
}

/**********************************************************************************************/
int ff = 0;
Void GFiber :: yield()
{
  Time      clock0  = GFiber::time();
  GTimer*&  timer   = s_fiber0->m_timer;

  if (ff++ > 10000000)
  {
    printf("WOO\n");
    exit(0);
  }

  while (s_waiting && (s_waiting->m_clock < clock0))
  {
    GFiber* active = s_waiting;
    s_waiting = s_waiting->m_next;
    active->enqueue(s_fiber1);
  }

  GFiber*   next    = s_fiber1 ? s_fiber1->m_next : 0;

  if (!next && s_waiting)
  {
    next      = s_waiting;
    s_waiting = s_waiting->m_next;
    next->enqueue(s_fiber1);
    //printf("SLEEP0 %lli\n", next->m_clock - clock0);
    //usleep((next->m_clock - clock0) / 1000);
    s_time   += next->m_clock - clock0 + 1;
  }

  if (!next)
  {
    printf("[[no active threads; swapping to the main thread]]");
    swapcontext(&s_fiber1->m_context, &s_main);
  }

  gSetContext(0, Undefined);
  next->activate();

  {
    QList<GTimer*> t;
    for (GTimer* s = timer; s; s = s->m_next)
    {
      if (t.contains(s))
      {
        printf("EEEEEEER %p\n", s);
        exit(0);
      }
      t.append(s);
    }
  }

  while (timer && timer->time() < clock0)
  {
    GTimer* active  = timer;
    timer           = timer->m_next;
    active->m_next  = 0;
    active->timeout();
  }

  if (s_fiber0->m_suspended)
    return s_fiber0->suspend();
}

/**********************************************************************************************/
