#include <Grid/App/GTerminator.hh>
#include <Grid/Bottom/G1Physical.hh>
#include <Grid/Bottom/G2DataLink.hh>
#include <Grid/Top/GMachine.hh>

/********************************************* TX *********************************************/
/*                                        GTerminator                                         */
/**********************************************************************************************/

Vacuum GTerminator :: GTerminator()
{
}

/**********************************************************************************************/

Void GTerminator :: run()
{
  sleep(100000);

  gDebug(this, ">>TERMINATING THIS MACHINE<<");

  auto list = machine()->layer2();

  for (auto it = list.begin(); it != list.end(); ++it)
  {
    G1Physical* layer1 = (*it)->layer1();

    layer1->setEnabled(false);
  }

  gDebug(this, ">>RESURRECTING THIS MACHINE<<");
  sleep(50000);
  gDebug(this, ">>RESURRECTING THIS MACHINE2<<");
}

/**********************************************************************************************/
