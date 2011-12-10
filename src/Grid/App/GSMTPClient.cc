#include <Grid/App/GSMTPClient.hh>

/********************************************* TX *********************************************/
/*                                        GSMTPClient                                         */
/**********************************************************************************************/

Vacuum GSMTPClient :: GSMTPClient(GEndPoint server, Int numConnections, Int numEmails):
  G5Client(server, numConnections, numEmails)
{
}

/**********************************************************************************************/

Bool GSMTPClient :: expect(int id0)
{
  QByteArray  line  = m_socket->readLine();
  int         space = line.indexOf(' ');
  int         id    = line.left(space).toInt();

  line.remove(0, space);
  gDebug(this, "SMTP_C: received: got=%i, expected=%i, msg=%s", id, id0, line.constData());

  if (id != id0)
  {
    gDebug(this, "SMTP_C: unexpected");
    return false;
  }
  return true;
}

/**********************************************************************************************/

Void GSMTPClient :: handler()
{
  G_GUARD(expect(220), Vacuum);

  gDebug(this, "SMTP_C: sending HELO");
  send("HELO meeee");
  G_GUARD(expect(250), Vacuum);

  gDebug(this, "SMTP_C: sending MAIL FROM");
  send("MAIL FROM:<me@me.org>");
  G_GUARD(expect(250), Vacuum);

  gDebug(this, "SMTP_C: sending RCPT TO");
  send("RCPT TO:<you@you.org>");
  G_GUARD(expect(250), Vacuum);

  gDebug(this, "SMTP_C: sending DATA");
  send("DATA");
  G_GUARD(expect(354), Vacuum);

  gDebug(this, "SMTP_C: sending contents");
  send("Line1\nLine2\nLine3\n.");
  G_GUARD(expect(250), Vacuum);

  gDebug(this, "SMTP_C: sending QUIT");
  send("QUIT");
  G_GUARD(expect(221), Vacuum);

  gDebug(this, "SMTP_C: server confirmed QUIT, closing connection");
  m_socket->close();
  gDebug(this, "SMTP_C: connection closed");
}

/**********************************************************************************************/

Void GSMTPClient :: send(const QByteArray& data)
{
  m_socket->write(data);
  m_socket->write("\n");
  m_socket->flush();
}

/**********************************************************************************************/
