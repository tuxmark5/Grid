#ifndef GRID_BOTTOM_G2_DATA_LINK_HH
#define GRID_BOTTOM_G2_DATA_LINK_HH

/**********************************************************************************************/
#include <Grid/Bottom/G1Physical.hh>
/**********************************************************************************************/
//#define G2_PREAMBLE         0xAAAAAAAAAAAAAAAALL
#define G2_PREAMBLE         0xAA00AA11AA22AA33LL
#define G2_HEAD_SIZE        22
#define G2_TAIL_SIZE        4
#define G2_META_SIZE        (G2_HEAD_SIZE + G2_TAIL_SIZE)
/**********************************************************************************************/
#define G2_MIN_FRAME_SIZE   64
#define G2_MAX_FRAME_SIZE   1500
/**********************************************************************************************/
#define G2_MIN_DATA_SIZE    (G2_MIN_FRAME_SIZE - G2_META_SIZE)
#define G2_MAX_DATA_SIZE    (G2_MAX_FRAME_SIZE - G2_META_SIZE)
/********************************************* TX *********************************************/
/*                                         G2DataLink                                         */
/**********************************************************************************************/

class G2DataLink
{
  public:
    __ENUM Type
    {
      RoutingTable  = 0x11,
      Transport     = 0x55
    };

    sCONST int              Broadcast = 0;

  private:
    ___FLD G1Physical*      m_layer1;
    ___FLD G3Network*       m_layer3;
    ___FLD U8               m_mac;

  public:
    ___MET Vacuum           G2DataLink(GLink* link, G3Network* layer3, U8 mac);
    ___MET Vacuum           ~G2DataLink();
    __sMET U4               crc32(GFrame& frame);
    ___MET G1Physical*      layer1() const { return m_layer1; }
    ___MET G3Network*       layer3() const { return m_layer3; }
    ___MET Bool             read(GFrame& frame);
    ___MET Bool             write(U8 dst, U2 type, GFrame& frame);
};

/**********************************************************************************************/

#endif /* GRID_BOTTOM_G2_DATA_LINK_HH */
