#include <Grid/App/G5Client.hh>

/**********************************************************************************************/
Int g_5_numIteration = 0;
/********************************************* TX *********************************************/
/*                                          G5Client                                          */
/**********************************************************************************************/

Vacuum G5Client :: G5Client(GEndPoint server, Int numConnections, Int numIterations):
  m_server(server),
  m_socket(0),
  m_numConnections(numConnections),
  m_numIterations(numIterations)
{
}

/**********************************************************************************************/

Void G5Client :: run()
{
  for (Int i = 0; i < m_numConnections; i++)
  {
    g_5_numIteration  = i;
    //g_enableOutput    = i >= 44;
    fprintf(stderr, "IT %i\n", i); fflush(stderr);

    gDebug(this, "CLIENT: connecting to %08x:%i, try=%i/%i",
      m_server.first, m_server.second, i, m_numConnections);

    if ((m_socket = GSocket::connect(m_server.first, m_server.second)))
    {
      gDebug(this, "CLIENT: connected to %08x:%i", m_server.first, m_server.second);
      handler();
      delete m_socket;
      m_socket = 0;
    }
    else
    {
      gDebug(this, "CLIENT: failed to connect to %08x:%i", m_server.first, m_server.second);
      sleep(2000);
      gDebug(this, "CLIENT: will try to connect again");
    }
  }

  gDebug(this, "CLIENT: all iterations completed");
  fflush(stdout);
}

/**********************************************************************************************/
