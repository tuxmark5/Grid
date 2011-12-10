#include <Grid/Util/GFunTimer.hh>
#include <Grid/GFiber.hh>

/********************************************* TX *********************************************/
/*                                         GFunTimer                                          */
/**********************************************************************************************/

Vacuum GFunTimer :: GFunTimer(Int interval, GFiber* fiber):
  GTimer(fiber),
  m_interval(interval),
  m_singleShot(false)
{
}

/**********************************************************************************************/

Vacuum GFunTimer :: ~GFunTimer()
{
}

/**********************************************************************************************/

auto GFunTimer :: restart(Int interval) -> Callback&
{
  stop();
  start(interval);
  return m_callback;
}

/**********************************************************************************************/

Void GFunTimer :: start(Int interval)
{
  if (interval != -1)
    m_interval = interval;
  m_time = GFiber::time() + m_interval * 1000000ULL;
  GTimer::start();
}

/**********************************************************************************************/

Void GFunTimer :: timeout()
{
  GTimer::timeout();

  if (m_callback)
  {
    m_callback();
  }

  if (!m_singleShot)
  {
    m_time += m_interval * 1000000ULL;
    GTimer::start();
  }
}

/**********************************************************************************************/

Void GFunTimer :: operator << (Callback&& callback)
{
  m_callback = std::forward<Callback>(callback);
  start();
}

/**********************************************************************************************/
