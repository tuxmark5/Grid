#ifndef GRID_APP_G5CLIENT_HH
#define GRID_APP_G5CLIENT_HH

/**********************************************************************************************/
#include <Grid/App/G5App.hh>
/********************************************* TX *********************************************/
/*                                          G5Client                                          */
/**********************************************************************************************/

class G5Client: public G5App
{
  protected:
    ___FLD GEndPoint      m_server;
    ___FLD GSocket*       m_socket;
    ___FLD Int            m_numConnections;
    ___FLD Int            m_numIterations;

  public:
    ___MET Vacuum         G5Client(GEndPoint server, Int numConnections, Int numIterations);
    __vMET Void           handler() = 0;
    ___MET Void           run();
};

/**********************************************************************************************/

template <class Agent>
class G5ClientAdapter: public G5Client, public Agent
{
  public:
    ___MET Vacuum         G5ClientAdapter(GEndPoint server, Int numConnections, Int numIterations):
      G5Client(server, numConnections, numIterations) { }
    ___MET Void           handler()
    { Agent::operator ()(this, m_socket); }
};

/**********************************************************************************************/

#endif /* GRID_APP_G5CLIENT_HH */
