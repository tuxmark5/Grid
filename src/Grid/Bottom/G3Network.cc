#include <Grid/Bottom/G3Network.hh>
#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Util/GDebug.hh>
#include <Grid/Util/GFunTimer.hh>
#include <Grid/GFiber.hh>
#include <limits.h>

/**********************************************************************************************/

U4 g_masks[] =
{
  0x00000000,
  0x00000001, 0x00000003, 0x00000007, 0x0000000F,
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
  0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
  0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
  0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
  0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};

/********************************************* TX *********************************************/
/*                                           G3Link                                           */
/**********************************************************************************************/

Vacuum G3Link :: G3Link(GLink* link, G3Network* layer3, U8 mac, U4 address, U1 length, Int metric):
  m_layer2(new G2DataLink(link, layer3, mac)),
  m_address(address),
  m_length(length),
  m_metric(metric + 1)
{

}

/**********************************************************************************************/

Vacuum G3Link :: ~G3Link()
{
  delete m_layer2;
}

/********************************************* TX *********************************************/
/*                                          G3Route                                           */
/**********************************************************************************************/

Vacuum G3Route :: G3Route():
  m_link(0),
  m_hwAddress(0),
  m_distance(INT_MAX),
  m_age(ULONG_LONG_MAX)
{
}

/**********************************************************************************************/

Bool G3Route :: write(U2 type, GFrame& frame)
{
  return m_link->layer2()->write(m_hwAddress, type, frame);
}

/********************************************* TX *********************************************/
/*                                         G3Network                                          */
/**********************************************************************************************/

Vacuum G3Network :: G3Network(G4Transport* layer4):
  m_layer4(layer4),
  m_routeTableTimer(new GFunTimer(2000, layer4->machine()->core()))
{
  *m_routeTableTimer << std::bind(&G3Network::writeRoutingTable, this);
}

/**********************************************************************************************/

Vacuum G3Network :: ~G3Network()
{
  delete m_routeTableTimer;
  qDeleteAll(m_links);
}

/**********************************************************************************************/

G3Link* G3Network :: addLink(GLink* link, U8 mac, Address address, U1 length)
{
  G3Link* link3 = new G3Link(link, this, mac, address, length);

  m_links.append(link3);
  return link3;
}

/**********************************************************************************************/

Bool G3Network :: broadcast(U2 type, const GFrame& frame)
{
  gDebug(this, "W: broadcast frame");
  for (auto it = m_links.begin(); it != m_links.end(); ++it)
  {
    GFrame frame1(frame);
    (*it)->layer2()->write(G2DataLink::Broadcast, type, frame1);
  }

  return true;
}

/**********************************************************************************************/

U1 G3Network :: compare(U4 a, U4 b, U1 l)
{
  for (U1 i = 0; i < l; i++, a <<= 1, b <<= 1)
    if ((a & 0x80000000) != (b & 0x80000000))
      return i;
  return l;
}

/**********************************************************************************************/

Bool G3Network :: drop(GFrame& frame)
{
  Q_UNUSED(frame);
  gDebug(this, "R: dropping frame");
  return false;
}

/**********************************************************************************************/

G3Link* G3Network :: findLink(G2DataLink* layer2)
{
  for (auto it = m_links.begin(); it != m_links.end(); ++it)
    if ((*it)->m_layer2 == layer2)
      return *it;
  return 0;
}

/**********************************************************************************************/

Bool G3Network :: forward(Address dst, GFrame& frame)
{
  if (G3Route* route = routeFor(dst))
  {
    gDebug(this, "RW: forwarding frame through: hw=%016x", route->m_hwAddress);
    return route->write(G2DataLink::Transport, frame);
  }

  gDebug(this, "R: forwarding frame: unable to find route");
  return false;
}

/**********************************************************************************************/

Bool G3Network :: read(G2DataLink* layer2, U8 src, U2 type, GFrame& frame)
{
  G3Link* link = findLink(layer2);

  switch (type)
  {
    case G2DataLink::RoutingTable:
      return readRoutingTable(link, src, frame);

    case G2DataLink::Transport:
      return readTransport(link, frame);

    default:
      gDebug(this, "R: WARNING: unknown frame type: %04x", type);
      break;
  }

  return false;
}

/**********************************************************************************************/

Bool G3Network :: readRoutingTable(G3Link* link, U8 src, GFrame& frame)
{
  int       numEntries  = frame.readFront4();
  Address   srcAddress  = frame.readFront4();
  Time      time        = GFiber::time();

  gDebug(this, "R:RTABLE: routing table");
  gDebug(this, "R:RTABLE: numEntries = %i", numEntries);

  updateRoute(link, src, srcAddress, link->metric(), time);

  for (int i = 0; i < numEntries; i++)
  {
    Address   address     = frame.readFront4();
    Int       distance1   = frame.readFront4() + link->metric();
    U4        age         = frame.readFront4();

    updateRoute(link, src, address, distance1, Time(age) * 1000000ULL);
  }

  gDebug(this, "R:RTABLE: | %8s | %16s | %8s | %8s |", "DST", "LINK", "DISTANCE", "AGE");
  for (auto it = m_routeTable.begin(); it != m_routeTable.end(); ++it)
  {
    gDebug(this, "R:RTABLE: | %08llx | %016llx | %8i | %8i |",
           it.key(), it->m_hwAddress, it->m_distance, U4(it->m_age / 1000000ULL));
    //it->m_link->metric());
  }

  return true;
}

/**********************************************************************************************/

Bool G3Network :: readTransport(G3Link* link, GFrame& frame)
{
  U4      l3_src  = frame.readFront4();    // 0-4
  U4      l3_dst  = frame.readFront4();    // 4-8
  U1      l3_hops = frame.readFront1();    // 8-9

  gDebug(this, "R: src  = %08x",  l3_src);
  gDebug(this, "R: dst  = %08x",  l3_dst);
  gDebug(this, "R: hops = %i",    l3_hops);

  if (l3_dst == link->m_address)
  {
    return m_layer4->read(l3_src, frame);
  }

  if (--l3_hops > 0)
  {
    frame.writeFront1(l3_hops);    // 8-9
    frame.seekFront(-8);           // 0-8
    return forward(l3_dst, frame);
  }

  return drop(frame);
}

/**********************************************************************************************/

G3Route* G3Network :: routeFor(Address address)
{
  if (m_routeTable.contains(address))
    return &m_routeTable[address];
  return 0;
}

/**********************************************************************************************/
/*
G3Link* G3Network :: selectRoute(U4 dst)
{
  G3Link*   bestRoute = m_links.first();
  U1        bestMatch = 0;

  for (auto it = m_links.begin(); it != m_links.end(); ++it)
  {
    G3Link* route = *it;
    U1      match = compare(dst, route->m_address, route->m_length);

    if ((match > bestMatch) || (match == bestMatch) && (route->m_metric < bestRoute->m_metric))
    {
      bestRoute = route;
      bestMatch = match;
    }
  }
  return bestRoute;
}*/

/**********************************************************************************************/

Void G3Network :: updateRoute(G3Link* link, U8 hwSrc, Address dst, Int distance, U8 age)
{
  G_GUARD(link->m_address != dst, Vacuum);

  G3Route& route = m_routeTable[dst];

  bool r0 =   !route.m_link;
  bool r1 = distance < route.m_distance;

  if ( !route.m_link                  // initialization
    || distance < route.m_distance    // route with better distance
    /*|| route.m_hwAddress == hwSrc*/)    // updated entry
  {
    /*printf("ADDED ENTRY FROM %x TO %x = dist=%i, met=%i [%i%i]\n",
           link->m_address, dst, distance, link->metric(),
           r0, r1);*/
    route.m_link        = link;
    route.m_hwAddress   = hwSrc;
    route.m_distance    = distance;
    route.m_age         = GFiber::time();

    gDebug(this, "R:RTABLE: ACCEPT route: dst=%08x, distance=%i", dst, distance);
  }
  else
  {
    gDebug(this, "R:RTABLE: REJECT route: dst=%08x, distance=%i", dst, distance);
  }
}

/**********************************************************************************************/

Bool G3Network :: write(Address dst, GFrame& frame)
{
  if (G3Route* route = routeFor(dst))
  {
    frame.writeFront1(G3_HOP_LIMIT);               //  8- 9  hop limit
    frame.writeFront4(dst);                        //  4- 8
    frame.writeFront4(route->m_link->m_address);   //  0- 4

    gDebug(this, "W: src  = %08x",  route->m_link->m_address);
    gDebug(this, "W: dst  = %08x",  dst);
    gDebug(this, "W: hops = %i",    G3_HOP_LIMIT);

    return route->write(G2DataLink::Transport, frame);
  }
  else
  {
    gDebug(this, "W: no route to dst=%08x", dst);
  }

  return false;
}

/**********************************************************************************************/

Bool G3Network :: writeRoutingTable()
{
  GFrame frame(RoutingTable);

  frame.writeBack4(m_routeTable.size());        // 0-4: numEntries
  frame.writeBack4(m_links.at(0)->m_address);   // 4-8: address

  gSetContext(frame.context());
  gDebug(this, "W:RTABLE: routing table");
  gDebug(this, "W:RTABLE: numEntries = %i", m_routeTable.size());

  for (auto it = m_routeTable.begin(); it != m_routeTable.end(); ++it)
  {
    gDebug(this, "W:RTABLE: ** entry: to=%08x, dist=%08x", it.key(), it->m_distance);
    frame.writeBack4(it.key());
    frame.writeBack4(it->m_distance);
    frame.writeBack4(U4(it->m_age / 10000000ULL));
  }

  return broadcast(G2DataLink::RoutingTable, frame);
}

/**********************************************************************************************/
