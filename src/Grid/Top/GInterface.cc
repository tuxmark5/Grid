#include <Grid/Top/GInterface.hh>

/********************************************* TX *********************************************/
/*                                         GInterface                                         */
/**********************************************************************************************/

Vacuum GInterface :: GInterface(GMachine* machine, U8 mac, Address address, U1 length):
  m_machine(machine),
  m_mac(mac),
  m_address(address),
  m_length(length)
{

}

/**********************************************************************************************/
