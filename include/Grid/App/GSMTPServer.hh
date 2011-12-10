#ifndef GRID_APP_GSMTP_SERVER_HH
#define GRID_APP_GSMTP_SERVER_HH

/**********************************************************************************************/
#include <Grid/App/G5Server.hh>
/********************************************* TX *********************************************/
/*                                        GSMTPMessage                                        */
/**********************************************************************************************/

class GSMTPMessage
{
  public:
    ___FLD QByteArray         m_src;
    ___FLD QList<QByteArray>  m_dst;
    ___FLD QByteArray         m_content;
};

/********************************************* TX *********************************************/
/*                                        GSMTPServer                                         */
/**********************************************************************************************/

class GSMTPServer: public G5Server
{
  public:
    ___FLD Int            m_numMessages;

  public:
    ___MET Vacuum         GSMTPServer(Port port);
    ___MET Void           handler(GSocket* socket);
    ___MET Int            queueMessage(GSMTPMessage* message);
};

/********************************************* TX *********************************************/
/*                                     GSMTPServerHandler                                     */
/**********************************************************************************************/

class GSMTPServerHandler
{
  public:
    ___FLD GSMTPServer*   m_server;
    ___FLD GSocket*       m_socket;
    ___FLD QByteArray     m_input;
    ___FLD GSMTPMessage*  m_message;

  public:
    ___MET Vacuum         GSMTPServerHandler(GSMTPServer* server, GSocket* socket);
    ___MET Vacuum         ~GSMTPServerHandler();
    ___MET Void           accept();
    ___MET Bool           expect(const char* prefix);
    ___MET Bool           expectHELO();
    ___MET Bool           readMessage();
    ___MET Void           reply(const char* fmt, ...);
    ___MET Void           run();
};

/**********************************************************************************************/

#endif /* GRID_APP_GSMTP_SERVER_HH */
