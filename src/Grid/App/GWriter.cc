#include <Grid/App/GWriter.hh>

/********************************************* TX *********************************************/
/*                                          GWriter                                           */
/**********************************************************************************************/

Void GWriter :: operator ()(G5App* app, GSocket* socket)
{
  const char* sampleData    = "hello world";
  const int   sampleLength  = strlen(sampleData) + 1;

  for (int i = 0; i < 1000; i++)
  {
    gDebug(app, "writing i = %i", i);
    socket->writeData(sampleData, sampleLength);
    socket->flush();
  }
}

/**********************************************************************************************/
