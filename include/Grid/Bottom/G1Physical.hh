#ifndef GRID_BOTTOM_G1_PHYSICAL_HH
#define GRID_BOTTOM_G1_PHYSICAL_HH

/**********************************************************************************************/
#include <Grid/Bottom/GFrame.hh>
#include <QtCore/QList>
/********************************************* TX *********************************************/
/*                                         G1Physical                                         */
/**********************************************************************************************/

class G1Physical
{
  public:
    __TYPE Void             (G1Physical::*Channel)(int bit);

  private:
    __sFLD GContext         s_frameContext;

    ___FLD GLink*           m_layer0;
    ___FLD G2DataLink*      m_layer2;

    ___FLD GFrame           m_input;
    ___FLD GFrame           m_output;

    ___FLD Channel          m_chan;
    ___FLD I8               m_offset;
    ___FLD Int              m_state;
    ___FLD Bool             m_collision: 1;

  public:
    ___MET Vacuum           G1Physical(GLink* layer0, G2DataLink* layer2);
    ___MET Vacuum           ~G1Physical();
    ___MET Void             format(GDebug& debug, GFrame& frame);
    ___MET G2DataLink*      layer2() const { return m_layer2; }
    ___MET Bool             read(GFrame& frame);
    ___MET Bool             rescueCollision();
    ___MET Bool             write(GFrame& frame);
    ___MET Void             writeNow(GFrame& frame);
    ___MET Bool             writeNow1(GFrame& frame);

  private:
    ___MET Void             collision();
    ___MET Void             fbegin();
    ___MET Void             fend();
    ___MET Void             ferror();
    ___MET Void             read0() { m_offset++; }
    ___MET Void             read1();
    ___MET Void             writeBitA(int bit);
    ___MET Void             writeBitB(int bit);
    ___MET Void             writeTerm(int bit);

    ___MET Void             chan_ignore(int bit);
    ___MET Void             chan_prepare(int bit);

    ___MET Void             chan_f0_0xxx_xxxx(int bit);
    ___MET Void             chan_f0_01xx_xxxx(int bit);
    ___MET Void             chan_f0_011x_xxxx(int bit);
    ___MET Void             chan_f0_0111_xxxx(int bit);
    ___MET Void             chan_f0_0111_1xxx(int bit);
    ___MET Void             chan_f0_0111_11xx(int bit);
    ___MET Void             chan_f0_0111_111x(int bit);
    ___MET Void             chan_f0_0111_1110(int bit);

    ___MET Void             chan_f1_1xxx_xxxx(int bit);
    ___MET Void             chan_f1_11xx_xxxx(int bit);
    ___MET Void             chan_f1_111x_xxxx(int bit);
    ___MET Void             chan_f1_1111_xxxx(int bit);
    ___MET Void             chan_f1_1111_1xxx(int bit);

    ___MET Void             chan_f1_1111_11xx(int bit);
    ___MET Void             chan_f1_1111_111x(int bit);
    ___MET Void             chan_f1_1111_1111(int bit);

};

/**********************************************************************************************/

inline Void G1Physical :: read1()
{
  m_input.d->m_data[(m_offset >> 3) & m_input.mask()] |= 1 << (m_offset & 7);
  m_offset++;
}

/**********************************************************************************************/

#endif /* GRID_BOTTOM_G1_PHYSICAL_HH */
