#include <Grid/Bottom/GProcess.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GSocket.hh>

/********************************************* TX *********************************************/
/*                                          GProcess                                          */
/**********************************************************************************************/

Vacuum GProcess :: GProcess(GMachine* machine, int pid, std::function<void()> function):
  GFiber(std::move(function)), m_machine(machine), m_pid(pid)
{
}

/**********************************************************************************************/

Vacuum GProcess :: ~GProcess()
{
  m_machine->removeProcess(this);
}

/**********************************************************************************************/

GProcess* GProcess :: current()
{
  return dynamic_cast<GProcess*>(s_fiber0);
}

/**********************************************************************************************/
