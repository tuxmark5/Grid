#include <Grid/App/GPingPongServer.hh>

/********************************************* TX *********************************************/
/*                                      GPingPongServer                                       */
/**********************************************************************************************/

Vacuum GPingPongServer :: GPingPongServer(Port port):
  G5Server(port)
{
}

/**********************************************************************************************/

Void GPingPongServer :: handler(GSocket* socket)
{
  gDebug(this, "PING_SERVER_HANDLER: handler starting");

  for (char message0[64]; ; )
  {
    sleep(1000);
    socket->readData(message0, 64);
    gDebug(this, "PING_SERVER_HANDLER: forwarding: %s", message0);
    socket->writeData(message0, 64);
    socket->flush();
  }

  gDebug(this, "PING_SERVER_HANDLER: closing");
  socket->close();
  gDebug(this, "PING_SERVER_HANDLER: closed");
  delete socket;
}

/**********************************************************************************************/
