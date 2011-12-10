#include <Grid/Util/GTimer.hh>
#include <Grid/GFiber.hh>

/********************************************* TX *********************************************/
/*                                           GTimer                                           */
/**********************************************************************************************/

Vacuum GTimer :: GTimer(GFiber* fiber):
  m_time(0),
  m_fiber(fiber),
  m_next(0),
  m_active(false)
{
  G_SETZ(m_fiber) = GFiber::current();
}

/**********************************************************************************************/

Vacuum GTimer :: ~GTimer()
{
  stop();
}

/**********************************************************************************************/

Void GTimer :: start()
{
  G_GUARD(!m_active, Vacuum);

  GTimer** self = &m_fiber->m_timer;

  while ((*self) && (*self)->time() < time())
    self = &(*self)->m_next;

  m_active  = true;
  m_next    = *self;
  *self     = this;
}

/**********************************************************************************************/

Void GTimer :: stop()
{
  G_GUARD(m_active, Vacuum);

  for (GTimer** self = &m_fiber->m_timer; *self; self = &(*self)->m_next)
  {
    if ((*self) == this)
    {
      (*self)   = (*self)->m_next;
      m_active  = false;
      m_next    = 0;
      return;
    }
  }

  printf("FAILED TO REMOVE ACTIVE TIMER!!!!\n");
  exit(0);
}

/**********************************************************************************************/

Void GTimer :: timeout()
{
  m_active = false;
}

/**********************************************************************************************/
