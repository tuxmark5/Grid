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
    //g_enableOutput = i >= 44;
    gDebug(app, "writing i = %i", i);
    socket->writeData(sampleData, sampleLength);
    socket->flush();
  }

  gDebug(app, "WRITER: closing connection");
  socket->close();
  gDebug(app, "WRITER: connection closed!");
}

/**********************************************************************************************/
