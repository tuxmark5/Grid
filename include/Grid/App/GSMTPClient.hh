#ifndef GRID_APP_GSMTP_CLIENT_HH
#define GRID_APP_GSMTP_CLIENT_HH

/**********************************************************************************************/
#include <Grid/App/G5Client.hh>
/********************************************* TX *********************************************/
/*                                        GSMTPClient                                         */
/**********************************************************************************************/

class GSMTPClient: public G5Client
{
  public:
    ___MET Vacuum         GSMTPClient(GEndPoint server, Int numConnections, Int numEmails);
    ___MET Bool           expect(int id);
    ___MET Void           handler();
    ___MET Void           send(const QByteArray& data);
};

/**********************************************************************************************/

#endif /* GRID_APP_GSMTP_CLIENT_HH */
