#include <Grid/App/GPingPongClient.hh>
#include <Grid/GFiber.hh>

/********************************************* TX *********************************************/
/*                                      GPingPongClient                                       */
/**********************************************************************************************/

Vacuum GPingPongClient :: GPingPongClient(GEndPoint endPoint, Int numConnections, Int numIterations):
  m_endPoint(endPoint),
  m_numConnections(numConnections),
  m_numIterations(numIterations)
{
}

/**********************************************************************************************/

Void GPingPongClient :: run()
{
  GFiber::sleep(5000);

  for (int i = 0; i < m_numConnections; i++)
  {
    gDebug(this, "PING_CLIENT: connecting to %08x:%i", m_endPoint.first, m_endPoint.second);

    if (GSocket* socket = GSocket::connect(m_endPoint.first, m_endPoint.second))
    {
      gDebug(this, "PING_CLIENT: connected to %08x:%i", m_endPoint.first, m_endPoint.second);

      for (int x = 0; x < m_numIterations; x++)
      {
        char  message0[64];
        char  message1[64];
        int   len;

        sprintf(message0, "This is a message %i-%i", i, x);
        gDebug(this, "PING_CLIENT: sending: %s", message0);
        socket->writeData(message0, strlen(message0) + 1);
        socket->flush();
        gDebug(this, "PING_CLIENT: waiting for reply");
        len = socket->readData(message1, 64);
        gDebug(this, "PING_CLIENT: reading message: len=%i, msg=%s", len, message1);
      }

      gDebug(this, "PING_CLIENT: closing connection");
      socket->close();
      gDebug(this, "PING_CLIENT: connection closed");
      delete socket;
    }
  }

  gDebug(this, "PING_CLIENT: TERMINATING");
}

/**********************************************************************************************/
