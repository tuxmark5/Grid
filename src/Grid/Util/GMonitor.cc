#include <Grid/Util/GMonitor.hh>
#include <Grid/Util/GSleepTimer.hh>
#include <Grid/GFiber.hh>

// iterate
// insert<&ss::fiber>
/// offsetof
// removeIf

template <class Node, Node* Node::*Next>
struct list
{
  template <class Lambda>
  static Void eraseWith(Node*& first, const Lambda& lambda)
  {
    while (first)
    {
      lambda(first);
      first = first->*Next;
    }
  }

  static Void insert(Node*& first, Node* node)
  {
    node->*Next = first;
    first       = node;
  }

  template <class Lambda>
  static Void removeIf(Node** self, const Lambda& lambda)
  {
    while (*self)
    {
      Node* next = (*self)->*Next;
      if (lambda(*self))
        *self = next;
      else
        self = &((*self)->*Next);
    }
  }
};

/**********************************************************************************************/
typedef list<GFiber, &GFiber::m_next> FiberList;
typedef list<GFiber, &GFiber::m_next> MonitorList;
/********************************************* TX *********************************************/
/*                                          GMonitor                                          */
/**********************************************************************************************/

Vacuum GMonitor :: GMonitor():
  m_firstFiber(0)
{
}

/**********************************************************************************************/

Vacuum GMonitor :: ~GMonitor()
{
}

/**********************************************************************************************/

Void GMonitor :: notify()
{
  list<GFiber, &GFiber::m_next>::removeIf(&GFiber::s_waiting, [=](GFiber* f) -> bool
  {
    if (f->monitor() == this)
    {
      f->resume();
      return true;
    }
    return false;
  });

  list<GFiber, &GFiber::m_monitorNext>::eraseWith(m_firstFiber, [=](GFiber* f)
  {
    f->m_monitor    = 0;
    f->m_suspended  = false;
  });
}

/**********************************************************************************************/

Void GMonitor :: wait()
{
  if (GFiber* fiber = GFiber::current())
  {
    list<GFiber, &GFiber::m_monitorNext>::insert(m_firstFiber, fiber);
    fiber->m_monitor = this;
    fiber->suspend();
    fiber->m_monitor = 0;
  }
}

/**********************************************************************************************/

Void GMonitor :: wait(int timeout)
{
  if (GFiber* fiber = GFiber::current())
  {
    list<GFiber, &GFiber::m_monitorNext>::insert(m_firstFiber, fiber);
    fiber->m_monitor = this;
    GSleepTimer().start(timeout);
    fiber->m_monitor = 0;
  }
}

/**********************************************************************************************/
