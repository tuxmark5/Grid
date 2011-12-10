#ifndef GRID_TOP_GINTERFACE_HH
#define GRID_TOP_GINTERFACE_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                         GInterface                                         */
/**********************************************************************************************/

class GInterface
{
  private:
    ___FLD GMachine*      m_machine;
    ___FLD U8             m_mac;
    ___FLD Address        m_address;
    ___FLD U1             m_length;

  public:
    ___MET Vacuum         GInterface(GMachine* machine, U8 mac, Address address, U1 length);
    ___MET Address        address() const { return m_address; }
    ___MET U1             length() const { return m_length; }
    ___MET U8             mac() const { return m_mac; }
    ___MET GMachine*      machine() const { return m_machine; }
};

/**********************************************************************************************/

#endif /* GRID_TOP_GINTERFACE_HH */
