#include <Grid/Bottom/G1Physical.hh>
#include <Grid/Bottom/GFrame.hh>

/**********************************************************************************************/
U8  g_frameId = 1;
/********************************************* TX *********************************************/
/*                                         GFrameData                                         */
/**********************************************************************************************/

Vacuum GFrameData :: GFrameData(Int type, G1Physical* layer1):
  m_layer1(layer1),
  m_data(new U1[G1_FRAME_CAPACITY]),
  m_offset0(G1_FRAME_CAPACITY >> 1),
  m_offset1(G1_FRAME_CAPACITY >> 1),
  m_mask(G1_FRAME_CAPACITY - 1),
  m_context({g_frameId++, type})
{
}

/**********************************************************************************************/

Vacuum GFrameData :: GFrameData(const GFrameData& other):
  GSharedData<GFrameData>(other),
  m_layer1(other.m_layer1),
  m_data(new U1[G1_FRAME_CAPACITY]),
  m_offset0(other.m_offset0),
  m_offset1(other.m_offset1),
  m_mask(other.m_mask),
  m_context(other.m_context)
{
  memcpy((void*) m_data, (void*) other.m_data, G1_FRAME_CAPACITY); // TODO: ineffective
}

/**********************************************************************************************/

Vacuum GFrameData :: ~GFrameData()
{
  delete [] m_data;
}

/**********************************************************************************************/

Void GFrameData :: readBack(U1* buffer0, U4 length)
{
  for (U1* buffer1 = buffer0 + length; buffer0 < buffer1; )
    *(--buffer1) = m_data[--m_offset1 & m_mask];
}

/**********************************************************************************************/

Void GFrameData :: readFront(U1* buffer0, U4 length)
{
  for (U1* buffer1 = buffer0 + length; buffer0 < buffer1; )
    *(buffer0++) = m_data[m_offset0++ & m_mask];
}

/**********************************************************************************************/

Void GFrameData :: writeBack(const U1* buffer0, U4 length)
{
  for (const U1* buffer1 = buffer0 + length; buffer0 < buffer1; )
    m_data[m_offset1++ & m_mask] = *(buffer0++);
}

/**********************************************************************************************/

Void GFrameData :: writeFront(const U1* buffer0, U4 length)
{
  for (const U1* buffer1 = buffer0 + length; buffer0 < buffer1; )
    m_data[--m_offset0 & m_mask] = *(--buffer1);
}

/********************************************* TX *********************************************/
/*                                           GFrame                                           */
/**********************************************************************************************/

Vacuum GFrame :: GFrame(Int type, G1Physical* layer1):
  d(new GFrameData(type, layer1))
{
}

/**********************************************************************************************/

Vacuum GFrame :: GFrame(const GFrame& other):
  d(other.d)
{
}

/**********************************************************************************************/

/*Vacuum GFrame :: GFrame(GFrame&& other):
  m_layer1(other.m_layer1),
  m_data(other.m_data),
  m_offset0(other.m_offset0),
  m_offset1(other.m_offset1),
  m_mask(other.m_mask)
{
  other.m_data = 0;
}*/

/**********************************************************************************************/

Vacuum GFrame :: ~GFrame()
{
}

/**********************************************************************************************/

Void GFrame :: padBack(U1 byte, int length)
{
  GFrameData* s = d;
  while (length-- > 0)
    s->m_data[s->m_offset1++ & s->m_mask] = byte;
}

/**********************************************************************************************/

Void GFrame :: processInput()
{
  d->m_layer1->read(*this);
}

/**********************************************************************************************/

Void GFrame :: processOutput()
{
  d->m_layer1->writeNow(*this);
}


/**********************************************************************************************/

Void GFrame :: zero()
{
  memset((void*) d->m_data, 0, mask() + 1);
}

/**********************************************************************************************/
