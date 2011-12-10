#ifndef GRID_TOP_GLINK_HH
#define GRID_TOP_GLINK_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
#include <QtCore/QList>
/********************************************* TX *********************************************/
/*                                           GLink                                            */
/**********************************************************************************************/

class GLink
{
  public:
    __TYPE QList<G1Physical*>     ClientList;
    __TYPE ClientList::Iterator   Iterator;

  private:
    ___FLD ClientList     m_clients;

  public:
    ___MET Vacuum         GLink();
    ___MET Vacuum         ~GLink();
    ___MET Iterator       begin() { return m_clients.begin(); }
    ___MET Iterator       end() { return m_clients.end(); }
    ___MET GLink&         operator << (const GInterface& interface);
};

/**********************************************************************************************/

#endif /* GRID_TOP_GLINK_HH */
