#ifndef GRID_GSHARED_PTR_HH
#define GRID_GSHARED_PTR_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                         GSharedPtr                                         */
/**********************************************************************************************/

template <class _Value>
class GSharedPtr
{
  public:
    __TYPE _Value               Value;
    __TYPE GSharedPtr<_Value>   Self;

  private:
    ___FLD Value*       m_value;

  public:
    ___MET Vacuum       GSharedPtr():
      m_value(0) { }

    ___MET Vacuum       GSharedPtr(Value* value):
      m_value(value) { retain(); }

    ___MET Vacuum       GSharedPtr(const Self& ptr):
      m_value(ptr.m_value) { retain(); }

    ___MET Vacuum       GSharedPtr(Self&& ptr):
      m_value(ptr.m_value) { ptr.m_value = 0; }

    ___MET Vacuum       ~GSharedPtr()
    { release(); }

    ___MET Void         detach()
    { if (m_value->m_numRefs > 1) { m_value->m_numRefs--; m_value = new Value(*m_value); m_value->m_numRefs++; } }

    ___MET Value*       get() const
    { return m_value; }

    ___MET Void         release()
    { if (m_value) if (m_value->release()) m_value = 0; }

    ___MET Void         retain() const
    { if (m_value) m_value->retain(); }

    ___MET Value*       operator -> ()
    { detach(); return m_value; }

    ___MET const Value* operator -> () const
    { return m_value; }

    ___MET Value&       operator * ()
    { detach(); return *m_value; }

    ___MET const Value& operator * () const
    { return *m_value; }

    ___MET Self&        operator = (Value* value)
    { if (value) value->retain(); release(); m_value = value; return *this; }

    ___MET Self&        operator = (const Self& ptr)
    { ptr.retain(); release(); m_value = ptr.m_value; return *this; }

    ___MET Vacuum       operator Value* ()
    { return m_value; }
};

/**********************************************************************************************/

#endif /* GRID_GSHARED_PTR_HH */
