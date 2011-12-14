#ifndef GRID_GGRID_HH
#define GRID_GGRID_HH

/**********************************************************************************************/
#include <functional>
/********************************************* TX *********************************************/
/*                                           GGrid                                            */
/**********************************************************************************************/
#define Vacuum
#define ___FLD
#define __sFLD static
#define ___MET
#define __sMET static
#define __vMET virtual
#define _ttMET template <class... Args>
#define __ENUM enum
#define __TYPE typedef
#define sCONST static const
#define FRIEND friend
/**********************************************************************************************/
#ifdef COMPILE
#  define FORWARD(t, a)     std::forward<t>(a)...
#else
#  define FORWARD(t, a)     std::forward<t>(a)
#endif
/**********************************************************************************************/
#define G_CONTINUE_IF(x)    if (x) continue
#define G_GUARD(c, r)       if (!(c)) return r
#define G_NZ(x)             if (x) (x)
#define G_SETZ(x)           if (!(x)) (x)
#define G_Z(x)              if ((x) == 0) x
/**********************************************************************************************/
#define G_USEC(x)   ((x) *       1000ULL)
#define G_MSEC(x)   ((x) *    1000000ULL)
#define G_SEC(x)    ((x) * 1000000000ULL)
/**********************************************************************************************/
typedef          char       Char;
typedef          short      Short;
typedef          int        Int;
typedef          long       Long;
typedef          long long  LongLong;
typedef unsigned char       UChar;
typedef unsigned short      UShort;
typedef unsigned int        uInt;
typedef unsigned long       ULong;
typedef unsigned long long  ULongLong;
/**********************************************************************************************/
typedef bool                Bool;
typedef double              Double;
typedef float               Float;
typedef void                Void;
/**********************************************************************************************/
typedef   signed char       I1;
typedef   signed short      I2;
typedef   signed int        I4;
typedef   signed long long  I8;
typedef float               F4;
typedef double              F8;
typedef unsigned char       U1;
typedef unsigned short      U2;
typedef unsigned int        U4;
typedef unsigned long long  U8;
/**********************************************************************************************/
typedef U4                  Address;
typedef int                 Delay;
typedef U2                  Port;
typedef U8                  Time;
/**********************************************************************************************/
class G1Physical;
class G2DataLink;
class G3Network;
class G4Transport;
class G4TransportL;
class G5App;
class GDebug;
class GFiber;
class GFrame;
class GFunTimer;
class GInterface;
class GLink;
class GMachine;
class GMonitor;
class GProcess;
class GSocket;
class GTimer;
/**********************************************************************************************/
template <typename Container>
Void gDeleteAll(Container cont)
{
  for (auto it = cont.begin(); it != cont.end(); ++it)
    delete *it;
}
/**********************************************************************************************/
#include <QtCore/QList>
/**********************************************************************************************/
inline uint qHash(const std::tuple<Port, Address, Port>& t)
{
  return std::get<0>(t) ^ std::get<1>(t) ^ std::get<2>(t);
}
/**********************************************************************************************/
#include <QtCore/QDebug>
/**********************************************************************************************/
typedef QPair<Address, Port>          GEndPoint;
typedef std::function<void(GFrame*)>  GFrameFun;
typedef QList<GFrame*>                GFrameList;
/**********************************************************************************************/
struct GContext
{
  U8    id;
  Int   type;
};
/**********************************************************************************************/
enum GFrameType
{
  Layer0        = 0x0001,
  Layer1        = 0x0002,
  Layer2        = 0x0004,
  Layer3        = 0x0008,
  Layer4        = 0x0010,
  Layer5        = 0x0020,
  Undefined     = 0x0100,
  RoutingTable  = 0x0200,
  Transport     = 0x0400,
  Choke         = 0x0800
};
/**********************************************************************************************/
class GObject
{
  public:
    ___MET Vacuum           GObject() { }
    __vMET Vacuum           ~GObject() { }
};
/**********************************************************************************************/
void gCollect(GObject* object);
/**********************************************************************************************/

#endif /* GRID_GGRID_HH */
