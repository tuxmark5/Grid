#ifndef GRID_APP_GREADER_HH
#define GRID_APP_GREADER_HH

/**********************************************************************************************/
#include <Grid/App/G5Client.hh>
#include <Grid/App/G5Server.hh>
/********************************************* TX *********************************************/
/*                                          GReader                                           */
/**********************************************************************************************/

class GReader
{
  public:
    ___MET Void           operator ()(G5App* app, GSocket* socket);
};

/**********************************************************************************************/
typedef G5ClientAdapter<GReader>  GReaderClient;
typedef G5ServerAdapter<GReader>  GReaderServer;
/**********************************************************************************************/

#endif /* GRID_APP_GREADER_HH */
