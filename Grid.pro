############################################## TX ##############################################
#                                             Grid                                             #
################################################################################################

QT              += core
QT              -= gui

TARGET           = Grid
CONFIG          += console
CONFIG          -= app_bundle

TEMPLATE         = app

################################################################################################

QMAKE_CXXFLAGS  += -DCOMPILE -std=gnu++0x -g
LIBS            += -g

INCLUDEPATH     += include

################################################################################################

SOURCES         +=                              \
  src/Grid/App/G5App.cc                         \
  src/Grid/App/G5Client.cc                      \
  src/Grid/App/G5Server.cc                      \
  src/Grid/App/GPingPongClient.cc               \
  src/Grid/App/GPingPongServer.cc               \
  src/Grid/App/GReader.cc                       \
  src/Grid/App/GSMTPClient.cc                   \
  src/Grid/App/GSMTPServer.cc                   \
  src/Grid/App/GTerminator.cc                   \
  src/Grid/App/GWriter.cc                       \
  src/Grid/Bottom/G1Physical.cc                 \
  src/Grid/Bottom/G2DataLink.cc                 \
  src/Grid/Bottom/G3Network.cc                  \
  src/Grid/Bottom/G4Transport.cc                \
  src/Grid/Bottom/GFrame.cc                     \
  src/Grid/Bottom/GProcess.cc                   \
  src/Grid/Top/GInterface.cc                    \
  src/Grid/Top/GLink.cc                         \
  src/Grid/Top/GMachine.cc                      \
  src/Grid/Top/GSocket.cc                       \
  src/Grid/Util/GCollector.cc                   \
  src/Grid/Util/GDebug.cc                       \
  src/Grid/Util/GFunTimer.cc                    \
  src/Grid/Util/GMonitor.cc                     \
  src/Grid/Util/GSleepTimer.cc                  \
  src/Grid/Util/GTimer.cc                       \
  src/Grid/GBitChan.cc                          \
  src/Grid/GBitVector.cc                        \
  src/Grid/GFiber.cc                            \
  src/Grid/main.cc

################################################################################################

HEADERS         +=                              \
  include/Grid/App/G5App.hh                     \
  include/Grid/App/G5Client.hh                  \
  include/Grid/App/G5Server.hh                  \
  include/Grid/App/GPingPongClient.hh           \
  include/Grid/App/GPingPongServer.hh           \
  include/Grid/App/GReader.hh                   \
  include/Grid/App/GSMTPClient.hh               \
  include/Grid/App/GSMTPServer.hh               \
  include/Grid/App/GTerminator.hh               \
  include/Grid/App/GWriter.hh                   \
  include/Grid/Bottom/G1Physical.hh             \
  include/Grid/Bottom/G2DataLink.hh             \
  include/Grid/Bottom/G3Network.hh              \
  include/Grid/Bottom/G4Transport.hh            \
  include/Grid/Bottom/GFrame.hh                 \
  include/Grid/Bottom/GProcess.hh               \
  include/Grid/Top/GInterface.hh                \
  include/Grid/Top/GLink.hh                     \
  include/Grid/Top/GMachine.hh                  \
  include/Grid/Top/GSocket.hh                   \
  include/Grid/Util/GCollector.hh               \
  include/Grid/Util/GDebug.hh                   \
  include/Grid/Util/GFunTimer.hh                \
  include/Grid/Util/GMonitor.hh                 \
  include/Grid/Util/GRetainer.hh                \
  include/Grid/Util/GSleepTimer.hh              \
  include/Grid/Util/GTimer.hh                   \
  include/Grid/GBitChan.hh                      \
  include/Grid/GBitCopy.hh                      \
  include/Grid/GBitVector.hh                    \
  include/Grid/GByteChan.hh                     \
  include/Grid/GChan.hh                         \
  include/Grid/GContChan.hh                     \
  include/Grid/GFiber.hh                        \
  include/Grid/GGrid.hh                         \
  include/Grid/GSharedData.hh                   \
  include/Grid/GSharedPtr.hh

################################################################################################
