#include <Grid/App/G5Server.hh>

/********************************************* TX *********************************************/
/*                                          G5Server                                          */
/**********************************************************************************************/

Vacuum G5Server :: G5Server(Port port):
  m_port(port)
{
}

/**********************************************************************************************/

Vacuum G5Server :: ~G5Server()
{
}

/**********************************************************************************************/

Void G5Server :: run()
{
  gDebug(this, "SERVER: listening on %i", m_port);

  GSocket* server = GSocket::serve(m_port);
  GSocket* client = 0;

  while (true)
  {
    gDebug(this, "SERVER: waiting for client");
    client = server->accept();
    gDebug(this, "SERVER: client connected, spawning handler fiber");
    *this << [=]()
    {
      handler(client);
    };
  }
}

/**********************************************************************************************/
