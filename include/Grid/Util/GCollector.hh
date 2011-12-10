#ifndef GRID_UTIL_GCOLLECTOR_HH
#define GRID_UTIL_GCOLLECTOR_HH

/**********************************************************************************************/
#include <Grid/Util/GMonitor.hh>
/********************************************* TX *********************************************/
/*                                         GCollector                                         */
/**********************************************************************************************/

class GCollector
{
  public:
    __TYPE QHash<GObject*, Bool>  ObjectHash;

  protected:
    __sFLD GCollector*      s_collector;
    ___MET GFiber*          m_fiber;
    ___MET ObjectHash       m_collection;
    ___MET GMonitor         m_monitor;

  public:
    ___MET Vacuum           GCollector();
    ___MET Vacuum           ~GCollector();
    ___MET Void             collect(GObject* object);
    __sMET Void             collectObject(GObject* object);
    ___MET Void             run();
};

/**********************************************************************************************/
extern GCollector* g_collector;
/**********************************************************************************************/

#endif /* GRID_UTIL_GCOLLECTOR_HH */
