#include <Grid/Bottom/G1Physical.hh>
#include <Grid/Bottom/G2DataLink.hh>
#include <Grid/Bottom/G3Network.hh>
#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Top/GLink.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Util/GDebug.hh>
#include <Grid/GFiber.hh>

/**********************************************************************************************/
/*template <typename Fun, typename Array, int Num>
struct Unroll;

template <typename Fun, typename Array>
struct Unroll<Fun, Array, 0>
{
  static void f(Fun fun, Array array)
  {
    fun(array[0]);
  }
};

template <typename Fun, typename Array, int Num>
struct Unroll
{
  static void f(Fun fun, Array array)
  {
    Unroll<Fun, Array, Num - 1>::f(fun, array);
    fun(array[Num]);
  }
};

template <typename Fun, typename Array>
void unroll(Fun fun, Array array, int num)
{
  switch (num)
  {
    case 0: return;
    case 1: return Unroll<Fun, Array, 1>::f(fun, array);
    case 2: return Unroll<Fun, Array, 2>::f(fun, array);
    case 3: return Unroll<Fun, Array, 3>::f(fun, array);
    case 4: return Unroll<Fun, Array, 4>::f(fun, array);
    case 5: return Unroll<Fun, Array, 5>::f(fun, array);
    case 6: return Unroll<Fun, Array, 6>::f(fun, array);
    case 7: return Unroll<Fun, Array, 7>::f(fun, array);
    case 8: return Unroll<Fun, Array, 8>::f(fun, array);
  }
}*/

template <int Num, int X> struct Expand;

template <int X> struct Expand<0, X>
{
  template <typename Class, void (Class::*Fun)(int)> static void f(Class*)
  {
  }
};

template <int Num, int X> struct Expand
{
  template <typename Class, void (Class::*Fun)(int)> static void f(Class* o)
  {
    (o->*Fun)(X & 1);
    Expand<Num - 1, (X >> 1)>::template f<Class, Fun>(o);
  }
};



template <int Num> struct ExpandD;

template <> struct ExpandD<0>
{
  template <typename Class, void (Class::*Fun)(int)> static void f(Class*, int)
  {
  }
};

template <int Num> struct ExpandD
{
  template <typename Class, void (Class::*Fun)(int)> static void f(Class* o, int x)
  {
    (o->*Fun)(x & 1);
    ExpandD<Num - 1>::template f<Class, Fun>(o, x >> 1);
  }
};

/**********************************************************************************************/
#define STATE(n, zero, zeros, one, ones)  \
  Void G1Physical :: n(int bit)           \
  {                                       \
    if (bit == 0)                         \
    {                                     \
      zero;                               \
      m_chan = &G1Physical::zeros;        \
    }                                     \
    else                                  \
    {                                     \
      one;                                \
      m_chan = &G1Physical::ones;         \
    }                                     \
  }

/**********************************************************************************************/
GContext G1Physical :: s_frameContext = { 0, Undefined };
/********************************************* TX *********************************************/
/*                                         G1Physical                                         */
/**********************************************************************************************/

Vacuum G1Physical :: G1Physical(GLink* layer0, G2DataLink* layer2):
  m_layer0(layer0),
  m_layer2(layer2),
  m_input(0),
  m_output(0),
  m_chan(&G1Physical::chan_f0_0xxx_xxxx),
  m_offset(-1),
  m_collision(false)
{
}

/**********************************************************************************************/

Vacuum G1Physical :: ~G1Physical()
{
}

/**********************************************************************************************/

Void G1Physical :: collision()
{
  for (auto it = m_layer0->begin(); it != m_layer0->end(); ++it)
  {
    G1Physical* phy = *it;

    if (phy == this)
      continue;
    for (int i = 0; i < 32; i++)
      (phy->*(phy->m_chan))(1);
  }
}

/**********************************************************************************************/

Void G1Physical :: fbegin()
{
  m_input             = GFrame(0, this);
  m_input.zero();
  m_input.setContext(s_frameContext);
  m_offset            = 0;
}

/**********************************************************************************************/

Void G1Physical :: fend()
{
  //printf("END FRAME\n");
  m_input.d->m_offset0  = 0;
  m_input.d->m_offset1  = (m_offset - 5) >> 3; // yes, 5 x read1
  read(m_input);
  m_input               = GFrame();
  m_offset              = -1;
}

/**********************************************************************************************/

Void G1Physical :: ferror()
{
  gDebug(this, "R: frame COLLISION;");
  m_input               = GFrame();
  m_offset              = -1;
}

/**********************************************************************************************/

Bool G1Physical :: read(GFrame& frame)
{
  G_GUARD(frame.size() > 0, false);

  gSetContext(frame.context());
  gDebug(this, "R: frame received;");
  gDebug(this, "R: size = %i;", frame.size());

  return m_layer2->read(frame);
}

/**********************************************************************************************/

Bool G1Physical :: rescueCollision()
{
  m_chan      = &G1Physical::chan_f0_0xxx_xxxx;
  m_collision = false;
  return false;
}

/**********************************************************************************************/

Bool G1Physical :: write(GFrame& frame)
{
  gDebug(this, "W: frame queued;");
  gDebug(this, "W: size = %i;", frame.size());

  if ((rand() % 500) == 1)
  {
    gDebug(this, "W: CORRUPTING this frame to make things more interesting;");
    frame[rand() % frame.size()] = 0xFF;
  }

  frame.setLayer1(this);
  layer2()->layer3()->layer4()->machine()->send(frame);
  return true;
}

/**********************************************************************************************/

Void G1Physical :: writeBitA(int bit)
{
 // printf("B %i\n", bit); sleep(1);

  if (bit == 0)
  {
    m_state = 0;
    writeBitB(0);
  }
  else
  {
    if (++m_state == 5)
    {
      m_state = 0;
      writeBitB(1);
      writeBitB(0);
    }
    else
    {
      writeBitB(1);
    }
  }
}

/**********************************************************************************************/

Void G1Physical :: writeBitB(int bit)
{
  //printf("A %i\n", bit); sleep(1);

  for (auto itl = m_layer0->begin(); itl != m_layer0->end(); ++itl)
  {
    G1Physical* l = *itl;
    if (l != this)
      (l->*(l->m_chan))(bit);
  }
}

/**********************************************************************************************/

Void G1Physical :: writeNow(GFrame& frame)
{
  for (int i = 0; i < 5; i++)
  {
    if (writeNow1(frame))
    {
      return;
    }
    else
    {
      int sleep = rand() % (100 << i);

      gSetContext(frame.context());
      gDebug(this, "W: frame write failure, try=%i, sleep=%i;", i, sleep);
      GFiber::sleep(sleep);
    }
  }

  gDebug(this, "W: maximum retry count reached, bailing out");
}

/**********************************************************************************************/

Bool G1Physical :: writeNow1(GFrame& frame)
{
  while (m_offset != -1)
  {
    gDebug(this, "W: *YIELD*");
    GFiber::sleep(20);
  }

  m_chan    = &G1Physical::chan_prepare;
  m_state   = 0;

  GFiber::sleep(30);
  G_GUARD(!m_collision, rescueCollision());

  s_frameContext = frame.context();

  Expand<8, 0x7E>::f<G1Physical, &G1Physical::writeBitB>(this);
  G_GUARD(!m_collision, rescueCollision());

  for (auto it = frame.begin(); it != frame.end(); ++it)
    ExpandD<8>::f<G1Physical, &G1Physical::writeBitA>(this, *it);
  Expand<8, 0x7E>::f<G1Physical, &G1Physical::writeBitB>(this);

  m_chan    = &G1Physical::chan_f0_0xxx_xxxx;

  GFiber::yield();
  return true;
}

/**********************************************************************************************/

Void G1Physical :: chan_ignore(int bit)
{
  Q_UNUSED(bit);
}

/**********************************************************************************************/

Void G1Physical :: chan_prepare(int bit)
{
  Q_UNUSED(bit);
  G_GUARD(!m_collision, Vacuum);

  m_chan      = &G1Physical::chan_ignore;
  m_collision = true;
  m_offset    = -1;
  gSetContext(s_frameContext);
  gDebug(this, "W: COLLISION detected, spamming 8 x 0xFF, this=%p", this);

  for (int i = 0; i < 4; i++)
    Expand<8, 0xFF>::f<G1Physical, &G1Physical::writeBitB>(this);
}

/**********************************************************************************************/

STATE(chan_f0_0xxx_xxxx, Vacuum,    chan_f0_01xx_xxxx, Vacuum,    chan_f0_0xxx_xxxx)
STATE(chan_f0_01xx_xxxx, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_011x_xxxx)
STATE(chan_f0_011x_xxxx, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_0111_xxxx)
STATE(chan_f0_0111_xxxx, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_0111_1xxx)
STATE(chan_f0_0111_1xxx, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_0111_11xx)
STATE(chan_f0_0111_11xx, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_0111_111x)
STATE(chan_f0_0111_111x, Vacuum,    chan_f0_0xxx_xxxx, Vacuum,    chan_f0_0111_1110)
STATE(chan_f0_0111_1110, fbegin(),  chan_f1_1xxx_xxxx, Vacuum,    chan_f0_0xxx_xxxx)

STATE(chan_f1_1xxx_xxxx, read0(),   chan_f1_1xxx_xxxx, read1(),   chan_f1_11xx_xxxx)
STATE(chan_f1_11xx_xxxx, read0(),   chan_f1_1xxx_xxxx, read1(),   chan_f1_111x_xxxx)
STATE(chan_f1_111x_xxxx, read0(),   chan_f1_1xxx_xxxx, read1(),   chan_f1_1111_xxxx)
STATE(chan_f1_1111_xxxx, read0(),   chan_f1_1xxx_xxxx, read1(),   chan_f1_1111_1xxx)
STATE(chan_f1_1111_1xxx, read0(),   chan_f1_1xxx_xxxx, read1(),   chan_f1_1111_11xx)

STATE(chan_f1_1111_11xx, Vacuum,    chan_f1_1xxx_xxxx, Vacuum,    chan_f1_1111_111x) // destuff
STATE(chan_f1_1111_111x, fend(),    chan_f0_0xxx_xxxx, ferror(),  chan_f0_0xxx_xxxx) // end frame

/**********************************************************************************************/
