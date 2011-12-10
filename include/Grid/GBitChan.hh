#ifndef GRID_GBIT_CHAN_HH
#define GRID_GBIT_CHAN_HH

/**********************************************************************************************/
#include <Grid/GBitVector.hh>
#include <Grid/GContChan.hh>
/********************************************* TX *********************************************/
/*                                          GBitChan                                          */
/**********************************************************************************************/

class GBitChan
{
  private:
    ___FLD GContChan<GBitVector> m_chan;

  public:
    ___MET Vacuum     GBitChan();
    ___MET Vacuum     GBitChan(const GBitChan& chan);
    ___MET Vacuum     ~GBitChan();
    ___MET Void       read(U2* dst, U4 doff, U4 len);
    ___MET Void       write(U2* src, U4 soff, U4 len);
};

/**********************************************************************************************/

#endif /* GRID_GBIT_CHAN_HH */
