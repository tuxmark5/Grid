#ifndef GRID_APP_GPING_PONG_SERVER_HH
#define GRID_APP_GPING_PONG_SERVER_HH

/**********************************************************************************************/
#include <Grid/App/G5Server.hh>
/********************************************* TX *********************************************/
/*                                      GPingPongServer                                       */
/**********************************************************************************************/

class GPingPongServer: public G5Server
{
  public:
    ___MET Vacuum         GPingPongServer(Port port);
    ___MET Void           handler(GSocket* socket);
};

/**********************************************************************************************/

#endif /* GRID_APP_GPING_PONG_SERVER_HH */
