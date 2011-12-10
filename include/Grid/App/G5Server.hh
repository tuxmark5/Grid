#ifndef GRID_APP_G5SERVER_HH
#define GRID_APP_G5SERVER_HH

/**********************************************************************************************/
#include <Grid/App/G5App.hh>
/********************************************* TX *********************************************/
/*                                          G5Server                                          */
/**********************************************************************************************/

class G5Server: public G5App
{
  public:
    ___FLD Port           m_port;

  public:
    ___MET Vacuum         G5Server(Port port);
    __vMET Vacuum         ~G5Server();
    __vMET Void           handler(GSocket* socket) = 0;
    ___MET Void           run();
};

/**********************************************************************************************/

template <class Agent>
class G5ServerAdapter: public G5Server
{
  public:
    ___MET Vacuum         G5ServerAdapter(Port port):
      G5Server(port) { }
    ___MET Void           handler(GSocket* socket)
    { Agent* agent = new Agent(); (*agent)(this, socket); delete agent; }
};

/**********************************************************************************************/

#endif /* GRID_APP_G5SERVER_HH */
