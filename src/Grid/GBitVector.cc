#include <Grid/GBitVector.hh>
#include <Grid/GBitCopy.hh>
#include <algorithm>
#include <string.h>

/********************************************* TX *********************************************/
/*                                         GBitVector                                         */
/**********************************************************************************************/

Vacuum GBitVector :: GBitVector(Int capacity):
  m_capacity(capacity),
  m_size(0)
{
  Int len = (capacity >> 4) + 1;
  m_data = new U2[len];
  memset((void*) m_data, 0, len * sizeof(U2));
}

/**********************************************************************************************/

Vacuum GBitVector :: ~GBitVector()
{
  printf("DIE\n");
  delete [] m_data;
}

/**********************************************************************************************/

U4 GBitVector :: get(U2* dst, U4 doff, U4 soff, U4 slen) const
{
  Int r = slen = std::min(m_capacity - soff, slen);

  while (slen > 0)
  {
    Int len   = slen > 15 ? 15 : slen - 1;
    Int spec  = (doff & 15) << 8 | (soff & 15) << 4 | len;
    Int delta = bitcopy(dst + (doff >> 4), m_data + (soff >> 4), spec);

    doff += delta;
    soff += delta;
    slen -= delta;
  }
  return r;
}

/**********************************************************************************************/

U4 GBitVector :: put(U2* src, U4 soff, U4 slen)
{
  Int r = set(src, soff, m_size, slen);

  m_size += r;
  return r;
}

/**********************************************************************************************/

U4 GBitVector :: set(U2* src, U4 soff, U4 doff, U4 dlen)
{
  Int r = dlen = std::min(m_capacity - doff, dlen);

  while (dlen > 0)
  {
    Int len   = dlen > 15 ? 15 : dlen - 1;
    Int spec  = (doff & 15) << 8 | (soff & 15) << 4 | len;
    Int delta = bitcopy(m_data + (doff >> 4), src + (soff >> 4), spec);

    doff += delta;
    soff += delta;
    dlen -= delta;
  }
  return r;
}

/**********************************************************************************************/
