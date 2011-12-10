#ifndef GRID_BOTTOM_GFRAME_HH
#define GRID_BOTTOM_GFRAME_HH

/**********************************************************************************************/
#include <Grid/GSharedData.hh>
#include <Grid/GSharedPtr.hh>
/**********************************************************************************************/
#define G1_FRAME_CAPACITY   2048
/********************************************* TX *********************************************/
/*                                           GFrame                                           */
/**********************************************************************************************/

class GFrameData: public GSharedData<GFrameData>
{
  public:
    ___FLD G1Physical*      m_layer1;
    ___FLD U1*              m_data;
    ___FLD U4               m_offset0;
    ___FLD U4               m_offset1;
    ___FLD U4               m_mask;
    ___FLD U8               m_value;
    ___FLD GContext         m_context;

  public:
    ___MET Vacuum           GFrameData(Int type = 0, G1Physical* layer1 = 0);
    ___MET Vacuum           GFrameData(const GFrameData& data);
    ___MET Vacuum           ~GFrameData();

    ___MET U1               operator [](int x) const { return m_data[x & m_mask]; }
    ___MET Void             readBack(U1* buffer, U4 length);
    ___MET U1               readBack1()           { return m_data[--m_offset1 & m_mask]; }
    ___MET Void             readFront(U1* buffer, U4 length);
    ___MET U1               readFront1()          { return m_data[m_offset0++ & m_mask]; }
    ___MET Void             writeBack(const U1* buffer, U4 length);
    ___MET Void             writeBack1(U1 data)   { m_data[m_offset1++ & m_mask] = data; }
    ___MET Void             writeFront(const U1* buffer, U4 length);
    ___MET Void             writeFront1(U1 data)  { m_data[--m_offset0 & m_mask] = data; }
};

/**********************************************************************************************/

class GFrame
{
  public:
    FRIEND class G1Physical;

  public:
    __TYPE const GFrameData               ConstData;
    __TYPE GSharedPtr<GFrameData>         DataPtr;

  public: struct Iterator
  {
    ___FLD ConstData&       m_frame;
    ___FLD U4               m_offset;
    ___FLD Vacuum           Iterator(ConstData& frame, U4 offset):
      m_frame(frame), m_offset(offset) { }
    ___FLD U1               operator * () { return m_frame[m_offset]; }
    ___FLD Iterator&        operator ++() { m_offset++; return *this; }
    ___FLD bool             operator !=(const Iterator& other) { return m_offset != other.m_offset; }
  };

  private:
    ___FLD DataPtr          d;

  public:
    ___MET Vacuum           GFrame(Int type = 0, G1Physical* layer1 = 0);
    ___MET Vacuum           GFrame(const GFrame& other);// = delete;
    //___MET Vacuum           GFrame(GFrame&& other);
    ___MET Vacuum           ~GFrame();
    ___MET U1               at(int x) const { return d->m_data[(d->m_offset0 + x) & d->m_mask]; }
    ___MET Iterator         begin()   const { return Iterator(*d, d->m_offset0); }
    ___MET Iterator         end()     const { return Iterator(*d, d->m_offset1); }
    ___MET const GContext&  context() const { return d->m_context; }
    ___MET G1Physical*      layer1()  const { return d->m_layer1; }
    ___MET U4               mask()    const { return d->m_mask; }
    ___MET U4               offset0() const { return d->m_offset0; }
    ___MET U4               offset1() const { return d->m_offset1; }
    ___MET Void             padBack(U1 byte, int length);
    ___MET Void             processInput();
    ___MET Void             processOutput();
    ___MET Void             readBack(U1* buffer, U4 length) { d->readBack(buffer, length); }
    ___MET U1               readBack1()   { return d->readBack1(); }
    ___MET U2               readBack2()   { U2 data = 0; d->readBack((U1*) &data, 2); return data; }
    ___MET U4               readBack4()   { U4 data = 0; d->readBack((U1*) &data, 4); return data; }
    ___MET U8               readBack6()   { U8 data = 0; d->readBack((U1*) &data, 6); return data; }
    ___MET U8               readBack8()   { U8 data = 0; d->readBack((U1*) &data, 8); return data; }
    ___MET Void             readFront(U1* buffer, U4 length) { d->readFront(buffer, length); }
    ___MET U1               readFront1()  { return d->readFront1(); }
    ___MET U2               readFront2()  { U2 data = 0; d->readFront((U1*) &data, 2); return data; }
    ___MET U4               readFront4()  { U4 data = 0; d->readFront((U1*) &data, 4); return data; }
    ___MET U8               readFront6()  { U8 data = 0; d->readFront((U1*) &data, 6); return data; }
    ___MET U8               readFront8()  { U8 data = 0; d->readFront((U1*) &data, 8); return data; }
    ___MET Void             seekFront(I4 delta)     { d->m_offset0 += delta; }
    ___MET Void             setContext(GContext c)  { d->m_context = c; }
    ___MET Void             setLayer1(G1Physical* layer1) { d->m_layer1 = layer1; }
    ___MET Void             setValue(U8 value)      { d->m_value = value; }
    ___MET U4               size() const { return (d->m_offset1 - d->m_offset0) & 0x7FFFFFFF; }
    ___MET Void             skipBack(U4 delta)      { d->m_offset1 -= delta; }
    ___MET U8               value() const           { return d->m_value; }
    ___MET Void             writeBack(const U1* buffer, U4 length) { d->writeBack(buffer, length); }
    ___MET Void             writeBack1(U1 data)     { d->writeBack1(data); }
    ___MET Void             writeBack2(U2 data)     { d->writeBack((U1*) &data, 2); }
    ___MET Void             writeBack4(U4 data)     { d->writeBack((U1*) &data, 4); }
    ___MET Void             writeBack6(U8 data)     { d->writeBack((U1*) &data, 6); }
    ___MET Void             writeBack8(U8 data)     { d->writeBack((U1*) &data, 8); }
    ___MET Void             writeFront(const U1* buffer, U4 length) { d->writeFront(buffer, length); }
    ___MET Void             writeFront1(U1 data)    { d->writeFront1(data); }
    ___MET Void             writeFront2(U2 data)    { d->writeFront((U1*) &data, 2); }
    ___MET Void             writeFront4(U4 data)    { d->writeFront((U1*) &data, 4); }
    ___MET Void             writeFront6(U8 data)    { d->writeFront((U1*) &data, 6); }
    ___MET Void             writeFront8(U8 data)    { d->writeFront((U1*) &data, 8); }
    ___MET U1&              operator [](int x)
    { GFrameData* s = d; return s->m_data[(s->m_offset0 + x) & s->m_mask]; }
    ___MET Void             zero();
};

/**********************************************************************************************/

#endif /* GRID_BOTTOM_GFRAME_HH */
