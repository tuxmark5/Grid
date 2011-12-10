#include <Grid/Util/GCollector.hh>
#include <Grid/GFiber.hh>

/**********************************************************************************************/
GCollector* GCollector :: s_collector = 0;
/********************************************* TX *********************************************/
/*                                         GCollector                                         */
/**********************************************************************************************/

Vacuum GCollector :: GCollector()
{
  m_fiber     = new GFiber(std::bind(&GCollector::run, this));
  s_collector = this;
}

/**********************************************************************************************/

Vacuum GCollector :: ~GCollector()
{
  s_collector = 0;
}

/**********************************************************************************************/

Void GCollector :: collect(GObject* object)
{
  m_collection.insert(object, true);
  m_monitor.notify();
}

/**********************************************************************************************/

Void GCollector :: collectObject(GObject* object)
{
  s_collector->collect(object);
}

/**********************************************************************************************/

Void GCollector :: run()
{
  while (true)
  {
    for (auto it = m_collection.begin(); it != m_collection.end(); it = m_collection.erase(it))
    {
      printf("COLLECT %p\n", it.key());
      delete it.key();
    }
    m_monitor.wait();
  }
}

/**********************************************************************************************/

void gCollect(GObject* object)
{
  GCollector::collectObject(object);
}

/**********************************************************************************************/
