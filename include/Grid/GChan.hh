#ifndef GRID_GCHAN_HH
#define GRID_GCHAN_HH

/**********************************************************************************************/
#include <Grid/GSharedData.hh>
#include <Grid/GSharedPtr.hh>
/********************************************* TX *********************************************/
/*                                           GChan                                            */
/**********************************************************************************************/

template <class _Element>
class GChanNode: public GSharedData<GChanNode<_Element> >
{
  public:
    __TYPE _Element             Element;
    __TYPE GChanNode<Element>   Self;

  public:
    ___FLD Element              m_element;
    ___FLD GSharedPtr<Self>     m_next;

  public:
    _ttMET Vacuum               GChanNode(Args&&... args):
      m_element(FORWARD(Args, args)) { }
};

/**********************************************************************************************/

template <class _Element>
class GChanData: public GSharedData<GChanData<_Element> >
{
  public:
    __TYPE _Element             Element;
    __TYPE GChanData<Element>   Self;
    __TYPE GChanNode<Element>   Node;

  public:
    ___FLD GSharedPtr<Node>     m_last;

  public:
    ___MET Vacuum               GChanData(const GSharedPtr<Node>& last):
      m_last(last) { }
};

/**********************************************************************************************/

template <class _Element>
class GChan
{
  public:
    __TYPE _Element             Element;
    __TYPE GChan<Element>       Self;
    __TYPE GChanData<Element>   Data;
    __TYPE GChanNode<Element>   Node;

  public:
    ___FLD GSharedPtr<Node>     m_first;
    ___FLD GSharedPtr<Data>     m_data;

  public:
    _ttMET Vacuum     GChan(Args&&... args):
      m_first(new Node(FORWARD(Args, args))),
      m_data(new Data(m_first)) { }

    ___MET Vacuum     GChan(const Self& chan):
      m_first(chan.m_data->m_last),
      m_data(chan.m_data) { }

    ___MET Element&   head() const
    {
      return m_first->m_element;
    }

    ___MET Element&   take()
    {
      // wait condition
      m_first = m_first->m_next;
      return head();
    }

    ___MET Element&   tail() const
    {
      return m_data->m_last->m_element;
    }

    _ttMET Element&   put(Args&&... args)
    {
      m_data->m_last->m_next  = new Node(FORWARD(Args, args));
      m_data->m_last          = m_data->m_last->m_next;
      return tail();
    }
};

/**********************************************************************************************/

#endif /* GRID_GCHAN_HH */
