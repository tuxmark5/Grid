#include <Grid/App/G5App.hh>
#include <Grid/GFiber.hh>

/********************************************* TX *********************************************/
/*                                            G5App                                           */
/**********************************************************************************************/

Vacuum G5App :: G5App():
  m_machine(0)
{
}

/**********************************************************************************************/

Vacuum G5App :: ~G5App()
{

}

/**********************************************************************************************/

Void G5App :: operator <<(Fiber&& fiber)
{
  new GFiber(std::forward<Fiber>(fiber));
}

/**********************************************************************************************/

Void G5App :: sleep(Int msecs)
{
  GFiber::sleep(msecs);
}

/**********************************************************************************************/
