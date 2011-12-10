#ifndef GRID_APP_GPING_PONG_CLIENT_HH
#define GRID_APP_GPING_PONG_CLIENT_HH

/**********************************************************************************************/
#include <Grid/App/G5App.hh>
/********************************************* TX *********************************************/
/*                                      GPingPongClient                                       */
/**********************************************************************************************/

class GPingPongClient: public G5App
{
  private:
    ___FLD GEndPoint      m_endPoint;
    ___FLD Int            m_numConnections;
    ___FLD Int            m_numIterations;

  public:
    ___MET Vacuum         GPingPongClient(GEndPoint endPoint, Int numConnections, Int numIterations);
    ___MET Void           run();
};

/**********************************************************************************************/

#endif /* GRID_APP_GPING_PONG_CLIENT_HH */
