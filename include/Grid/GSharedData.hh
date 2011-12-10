#ifndef GRID_GSHARED_DATA_HH
#define GRID_GSHARED_DATA_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                         GSharedData                                        */
/**********************************************************************************************/

template <class _Derived>
class GSharedData
{
  public:
    __TYPE _Derived               Derived;
    __TYPE GSharedData<_Derived>  Self;

  public:
    ___FLD Int          m_numRefs;

  public:
    ___MET Vacuum       GSharedData():
      m_numRefs(0) { }

    ___MET Vacuum       GSharedData(const GSharedData&):
      m_numRefs(0) { }

    ___MET Derived*     derived()
    { return static_cast<Derived*>(this); }

    ___MET bool         release()
    { if (--m_numRefs <= 0) { delete derived(); return true; } return false; }

    ___MET void         retain()
    { m_numRefs++; }
};

/**********************************************************************************************/

#endif /* GRID_GSHARED_DATA_HH */
