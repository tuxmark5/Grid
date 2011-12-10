#include <Grid/GBitChan.hh>

/**********************************************************************************************/
#define CHAN_SIZE 100
/********************************************* TX *********************************************/
/*                                          GBitChan                                          */
/**********************************************************************************************/
#if 0
Vacuum GBitChan :: GBitChan():
  m_chan(CHAN_SIZE)
{
}

/**********************************************************************************************/

Vacuum GBitChan :: GBitChan(const GBitChan& chan):
  m_chan(chan.m_chan)
{
}

/**********************************************************************************************/

Vacuum GBitChan :: ~GBitChan()
{
}

/**********************************************************************************************/
#include <stdio.h>
Void GBitChan :: read(U2* dst, U4 doff, U4 len)
{
  auto head = &m_chan.head();

  // jei nera data tada yield su condition; globalus offsetai

  while (true)
  {
    printf("RD  %i:%i\n", m_chan.m_offset, len);
    Int delta = head->get(dst, doff, m_chan.m_offset, len);

    doff            += delta;
    m_chan.m_offset += delta;
    len             -= delta;

    if (len > 0)
    {
      printf("FREAD %x %i %i | %i\n", head, len, delta, m_chan.m_offset);
      m_chan.m_offset = 0;
      head = &m_chan.take();
      printf("FREAD2 %x\n", head);
    }
    else
      break;
  }
}

/**********************************************************************************************/

Void GBitChan :: write(U2* src, U4 soff, U4 len)
{
  auto tail = &m_chan.tail();

  while (true)
  {
    Int delta = tail->put(src, soff, len);

    soff  += delta;
    len   -= delta;
    printf("W2 %i %i\n", delta, len);

    if (len > 0)
    {
      tail = &m_chan.put(CHAN_SIZE);
      printf("PUT %x\n", tail);
    }
    else
      break;
  }
}
#endif
/**********************************************************************************************/
