#ifndef GRID_GBIT_VECTOR_HH
#define GRID_GBIT_VECTOR_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                         GBitVector                                         */
/**********************************************************************************************/

class GBitVector
{
  public:
    ___FLD U2*      m_data;
    ___FLD Int      m_capacity;
    ___FLD Int      m_size;

  public:
    ___MET Vacuum   GBitVector(Int capcity);
    ___MET Vacuum   ~GBitVector();
    ___MET U4       get(U2* dst, U4 doff, U4 soff, U4 slen) const;
    ___MET U4       put(U2* src, U4 soff, U4 slen);
    ___MET U4       set(U2* src, U4 soff, U4 doff, U4 dlen);
};

/**********************************************************************************************/

#endif /* GRID_GBIT_VECTOR_HH */
