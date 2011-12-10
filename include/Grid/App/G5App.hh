#ifndef GRID_APP_G5APP_HH
#define GRID_APP_G5APP_HH

/**********************************************************************************************/
#include <Grid/Top/GSocket.hh>
#include <Grid/Util/GDebug.hh>
#include <QtCore/QRunnable>
/********************************************* TX *********************************************/
/*                                            G5App                                           */
/**********************************************************************************************/

class G5App: public QRunnable
{
  public:
    __TYPE std::function<void ()> Fiber;

  public:
    ___FLD GMachine*      m_machine;

  public:
    ___MET Vacuum         G5App();
    __vMET Vacuum         ~G5App();
    ___MET GMachine*      machine() const { return m_machine; }
    ___MET Void           operator <<(std::function<void ()>&& fiber);
    ___MET Void           sleep(Int msecs);
};

/**********************************************************************************************/

#endif /* GRID_APP_G5APP_HH */
