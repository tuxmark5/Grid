#include <Grid/App/GSMTPServer.hh>
#include <stdarg.h>

/********************************************* TX *********************************************/
/*                                        GSMTPServer                                         */
/**********************************************************************************************/

Vacuum GSMTPServer :: GSMTPServer(Port port):
  G5Server(port)
{
}

/**********************************************************************************************/

Void GSMTPServer :: handler(GSocket* socket)
{
  auto h = new GSMTPServerHandler(this, socket);

  h->run();
  delete h;
}

/**********************************************************************************************/

Int GSMTPServer :: queueMessage(GSMTPMessage* message)
{
  delete message;

  return m_numMessages++;
}

/********************************************* TX *********************************************/
/*                                     GSMTPServerHandler                                     */
/**********************************************************************************************/

Vacuum GSMTPServerHandler :: GSMTPServerHandler(GSMTPServer* server, GSocket* socket):
  m_server(server),
  m_socket(socket),
  m_message(0)
{
}

/**********************************************************************************************/

Vacuum GSMTPServerHandler :: ~GSMTPServerHandler()
{
  delete m_message;
}

/**********************************************************************************************/

Void GSMTPServerHandler :: accept()
{
  m_input = QByteArray();
}

/**********************************************************************************************/

Bool GSMTPServerHandler :: expect(const char* prefix)
{
  if (m_input.isNull())
    m_input = m_socket->readLine();

  if (m_input.startsWith(prefix))
  {
    m_input.remove(0, strlen(prefix));
    m_input.remove(m_input.size() - 1, 1);
    return true;
  }

  return false;
}

/**********************************************************************************************/

Bool GSMTPServerHandler :: expectHELO()
{
  if (expect("HELO "))
  {
    gDebug(m_server, "SMTP_S_H: Client says HELO: %s", m_input.constData());
    reply("250 Hello %s, yes yes", m_input.constData());
    return true;
  }

  reply("221 Error, bye");
  m_socket->close();
  return false;
}

/**********************************************************************************************/

Bool GSMTPServerHandler :: readMessage()
{
  while (m_socket->isConnected())
  {
    QByteArray line = m_socket->readLine();

    gDebug(m_server, "SMTP_S_H: received line: %s", line.constData());
    if (!line.isEmpty())
    {
      if (line != ".\n")
        m_message->m_content.append(line);
      else
        return true;
    }
  }

  return false;
}

/**********************************************************************************************/

Void GSMTPServerHandler :: reply(const char* fmt, ...)
{
  va_list  args;
  char    message[128];
  int     len;

  va_start(args, fmt);
  len = vsprintf(message, fmt, args);
  strcat(message, "\n");
  va_end(args);

  m_socket->writeData(message, len + 1);
  m_socket->flush();
  m_input = QByteArray();
}

/**********************************************************************************************/

Void GSMTPServerHandler :: run()
{
  gDebug(m_server, "SMTP_S_H: Saying hello");
  reply("220 server SMTP GridMail");
  G_GUARD(expectHELO(), Vacuum);

  m_message = new GSMTPMessage();

  while (true)
  {
    /**/ if (expect("MAIL FROM:"))
    {
      gDebug(m_server, "SMTP_S_H: received MAIL FROM: %s => 250 Ok", m_input.constData());
      m_message->m_src = m_input;
      reply("250 Ok");
    }
    else if (expect("RCPT TO:"))
    {
      gDebug(m_server, "SMTP_S_H: received RCPT TO: %s => 250 Ok", m_input.constData());
      m_message->m_dst.append(m_input);
      reply("250 Ok");
    }
    else if (expect("DATA"))
    {
      gDebug(m_server, "SMTP_S_H: received DATA => 354 End Data ...");
      reply("354 End Data with <LF>.<LF>");

      if (readMessage())
      {
        int id = m_server->queueMessage(m_message);

        m_message = new GSMTPMessage();
        reply("250 Ok: queued as %i", id);
      }
      else
      {
        gDebug(m_server, "SMTP_S_H: failed to read message");
      }
    }
    else if (expect("QUIT"))
    {
      gDebug(m_server, "SMTP_S_H: saying BYE => 221 Bye");
      reply("221 Bye");
      gDebug(m_server, "SMTP_S_H: closing connection => close");
      m_socket->close();
      gDebug(m_server, "SMTP_S_H: connection closed => terminate");
      break;
    }
    else if (!m_socket->isConnected())
    {
      gDebug(m_server, "SMTP_S_H: connection lost");
      break;
    }
    else
    {
      gDebug(m_server, "SMTP_S_H: invalid message received: %s", m_input.constData());
      m_input = QByteArray();
    }
  }
}

/**********************************************************************************************/
