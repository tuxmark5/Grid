#include <Grid/Bottom/G3Network.hh>
#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Top/GInterface.hh>
#include <Grid/Top/GLink.hh>
#include <Grid/Top/GMachine.hh>

/********************************************* TX *********************************************/
/*                                           GLink                                            */
/**********************************************************************************************/

Vacuum GLink :: GLink()
{
}

/**********************************************************************************************/

Vacuum GLink :: ~GLink()
{
}

/**********************************************************************************************/

GLink& GLink :: operator << (const GInterface& interface)
{
  G3Network*  layer3  = interface.machine()->layer4()->layer3();
  G3Link*     link3   = layer3->addLink(this, interface.mac(), interface.address(), interface.length());
  G1Physical* layer1  = link3->layer2()->layer1();

  m_clients.append(layer1);
  return *this;
}

/**********************************************************************************************/
