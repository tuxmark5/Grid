#ifndef GRID_UTIL_GRETAINER_HH
#define GRID_UTIL_GRETAINER_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                         GRetainer                                          */
/**********************************************************************************************/

template <class _Value>
class GRetainer
{
  public:
    __TYPE _Value           Value;

  protected:
    ___MET Value*           m_value;

  public:
    ___MET Vacuum           GRetainer(Value* value):
      m_value(value) { m_value->retain(); }
    ___MET Vacuum           ~GRetainer()
    { m_value->release(); }
};

/**********************************************************************************************/

#endif /* GRID_UTIL_GRETAINER_HH */
