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

/********************************************* TX *********************************************/
/*                                            main                                            */
/**********************************************************************************************/


/**********************************************************************************************/

/*
3: overflow
4: threshold
* INITIAL SETUP
*/

Int main(Int argc, Char** argv)
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  GCollector  collector;
  GLink       l12, l23, l34, l14;
  GMachine&   m1 = machine("[1] SR");
  GMachine&   m2 = machine("[2]");
  GMachine&   m3 = machine("[3] CL");
  GMachine&   m4 = machine("[4]");

  l12 << m1(0x11A, 0x11, 16) << m2(0x22A, 0x22, 16);
  l23 << m3(0x33A, 0x33, 16) << m2(0x22B, 0x22, 16);
  l34 << m3(0x33B, 0x33, 16) << m4(0x44A, 0x44, 16);
  l14 << m1(0x11B, 0x11, 16) << m4(0x44B, 0x44, 16);

  //m1 << new GPingPongServer(123);
  //m3 << new GPingPongClient(GEndPoint(0x11, 123), 5, 5);

  m1 << new GSMTPServer(123);
  //m2 << new GTerminator();
  m4 << new GTerminator();
  m3 << new GSMTPClient(GEndPoint(0x11, 123), 1000, 5);

  //m1 << new GWriterServer(123);
  //m3 << new GReaderClient(GEndPoint(0x11, 123), 1000, 5);

  GFiber::exec();
  printf("[[normal exit]]\n");
}

/**********************************************************************************************/
