#ifndef GRID_APP_GWRITER_HH
#define GRID_APP_GWRITER_HH

/**********************************************************************************************/
#include <Grid/App/G5Client.hh>
#include <Grid/App/G5Server.hh>
/********************************************* TX *********************************************/
/*                                          GWriter                                           */
/**********************************************************************************************/

class GWriter
{
  public:
    ___MET Void           operator ()(G5App* app, GSocket* socket);
};

/**********************************************************************************************/
typedef G5ClientAdapter<GWriter>  GWriterClient;
typedef G5ServerAdapter<GWriter>  GWriterServer;
/**********************************************************************************************/

#endif /* GRID_APP_GWRITER_HH */
