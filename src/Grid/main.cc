#include <Grid/App/GPingPongClient.hh>
#include <Grid/App/GPingPongServer.hh>
#include <Grid/App/GReader.hh>
#include <Grid/App/GSMTPClient.hh>
#include <Grid/App/GSMTPServer.hh>
#include <Grid/App/GTerminator.hh>
#include <Grid/App/GWriter.hh>
#include <Grid/Top/GInterface.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Top/GLink.hh>
#include <Grid/Top/GSocket.hh>
#include <Grid/Util/GCollector.hh>
#include <Grid/GFiber.hh>
#include <QtCore/QCoreApplication>
#include <stdio.h>

/**********************************************************************************************/
Int   g_3_routePreemption     = 45;     // secs
Int   g_3_routeTimeout        = 90;     // secs
/**********************************************************************************************/
Int   g_4_receiveWindow       = 10;     // frames
Int   g_4_retransmitRetries   = 100;    // tries
Int   g_4_retransmitTimeout   = 1000;   // msecs
Int   g_4_softTimeout         = 30;     // secs
Int   g_4_hardTimeout         = 30000;  // secs
Int   g_4_ackOfSyn2Dilation   = 10;     // frames; for the timer problem
/**********************************************************************************************/
Int   g_d_output
  = Layer0 | Layer1 | Layer2 | Layer3 | Layer4 | Layer5
  | Transport | Undefined | Choke;
/********************************************* TX *********************************************/
/*                                            main                                            */
/**********************************************************************************************/

Int main(Int argc, Char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  GCollector  collector;
  GMachine&   m0 = machine("[0] SR");
  GMachine&   m1 = machine("[1]");
  GMachine&   m2 = machine("[2]");
  GMachine&   m3 = machine("[3] CL");
  GMachine&   m4 = machine("[4]");
  GMachine&   m5 = machine("[5]");

  m0 & m1; m1 & m2; m2 & m3;
  m0 & m4; m4 & m5; m5 & m3;

  //m0 & m1 & m3;
  //m0 & m2 & m3;

  //m0 << new GPingPongServer(123);
  //m3 << new GPingPongClient(GEndPoint(0x00, 123), 20, 5);

  m0 << new GSMTPServer(123);
  m3 << new GSMTPClient(GEndPoint(0x00, 123), 1000, 5);

  //m0 << new GWriterServer(123);
  //m3 << new GReaderClient(GEndPoint(0x00, 123), 1000, 5);

  GFiber::exec();
  printf("[[normal exit]]\n");
}

/**********************************************************************************************/
