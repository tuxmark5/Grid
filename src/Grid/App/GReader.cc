#include <Grid/App/GReader.hh>
#include <Grid/GFiber.hh>

/********************************************* TX *********************************************/
/*                                          GReader                                           */
/**********************************************************************************************/

Void GReader :: operator ()(G5App* app, GSocket* socket)
{
  char  buffer[256];
  Int   size = 0;

  for (int i = 0; i < 1000; i++)
  {
    gDebug(app, "sleeping... i=%i", i);
    app->sleep(50 * 1000);

    gDebug(app, "begin reading (i=%i)", i);
    size = socket->read(buffer, 16);
    gDebug(app, "end reading... size=%i", size);
  }
}

/**********************************************************************************************/
