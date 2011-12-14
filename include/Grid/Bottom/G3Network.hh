#ifndef GRID_BOTTOM_G3_NETWORK_HH
#define GRID_BOTTOM_G3_NETWORK_HH

/**********************************************************************************************/
#include <Grid/Bottom/G2DataLink.hh>
/**********************************************************************************************/
#define G3_HEAD_SIZE        9
#define G3_META_SIZE        G3_HEAD_SIZE
#define G3_HOP_LIMIT        10
/**********************************************************************************************/
#define G3_MAX_DATA_SIZE    (G2_MAX_DATA_SIZE - G3_META_SIZE)
/********************************************* TX *********************************************/
/*                                         G3Network                                          */
/**********************************************************************************************/

class G3Link
{
  public:
    ___FLD G2DataLink*      m_layer2;
    ___FLD Address          m_address;
    ___FLD U1               m_length;
    ___FLD Int              m_metric;

  public:
    ___MET Vacuum           G3Link(GLink* link, G3Network* layer3, U8 mac, U4 address, U1 length, Int metric = 0);
    ___MET Vacuum           ~G3Link();
    ___MET G2DataLink*      layer2() const { return m_layer2; }
    ___MET Int              metric() const { return m_metric; }
};

/**********************************************************************************************/

class G3Route
{
  public:
    ___FLD G3Link*          m_link;
    ___FLD U8               m_hwAddress;
    ___FLD Int              m_distance;
    ___FLD Time             m_age;

  public:
    ___MET Vacuum           G3Route();
    ___MET G3Link*          link() const { return m_link; }
    ___MET Bool             write(U2 type, GFrame& frame);
};

/**********************************************************************************************/

class G3Network
{
  public:
    __TYPE QList<G3Link*>             LinkList;
    __TYPE QHash<Address, G3Route>    RouteTable;

  private:
    ___FLD LinkList         m_links;
    ___FLD RouteTable       m_routeTable;
    ___FLD G4Transport*     m_layer4;
    ___FLD GFunTimer*       m_routeTableTimer;

  public:
    ___MET Vacuum           G3Network(G4Transport* layer4);
    ___MET Vacuum           ~G3Network();
    ___MET G3Link*          addLink(GLink* link, U8 mac, Address address, U1 length);
    ___MET Bool             broadcast(U2 type, const GFrame& frame);
    ___MET G4Transport*     layer4() const { return m_layer4; }
    ___MET const LinkList&  links() const { return m_links; }
    ___MET Bool             read(G2DataLink* layer2, U8 src, U2 type, GFrame& frame);
    ___MET Bool             readRoutingTable(G3Link* link, U8 src, GFrame& frame);
    ___MET Bool             readTransport(G3Link* link, U2 type, GFrame& frame);
    ___MET G3Route*         routeFor(Address address);
    ___MET Bool             write(Address dst, GFrame& frame, U2 type = G2DataLink::Transport);
    ___MET Bool             writeChoke(Address dst);
    ___MET Bool             writeRoutingTable();

  private:
    __sMET U1               compare(U4 a, U4 b, U1 l);
    ___MET Bool             drop(GFrame& frame);
    ___MET G3Link*          findLink(G2DataLink* layer2);
    ___MET Bool             forward(Address dst, GFrame& frame);
    ___MET Void             updateRoute(G3Link* link, U8 hwSrc, Address dst, Int distance, U8 age);

};

/**********************************************************************************************/

#endif /* GRID_BOTTOM_G3_NETWORK_HH */
