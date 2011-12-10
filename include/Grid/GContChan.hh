#ifndef GRID_GCONT_CHAN_HH
#define GRID_GCONT_CHAN_HH

/**********************************************************************************************/
#include <Grid/GChan.hh>
/********************************************* TX *********************************************/
/*                                         GContChan                                          */
/**********************************************************************************************/

template <class _Element>
class GContChan: public GChan<_Element>
{
  public:
    __TYPE _Element             Element;
    __TYPE GChan<Element>       Base;
    __TYPE GContChan<Element>   Self;

  public:
    ___FLD U4                   m_offset;

  public:
    _ttMET Vacuum               GContChan(Args&&... args):
      Base(FORWARD(Args, args)), m_offset(0) { }

    ___MET Vacuum               GContChan(const Self& chan):
      Base((const Base&) chan), m_offset(chan.m_data->m_last->m_element.m_size) { }
};

/**********************************************************************************************/

#endif /* GRID_GCONT_CHAN_HH */
