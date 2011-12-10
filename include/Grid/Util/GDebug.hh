#ifndef GRID_UTIL_GDEBUG_HH
#define GRID_UTIL_GDEBUG_HH

/**********************************************************************************************/
#include <Grid/GGrid.hh>
/********************************************* TX *********************************************/
/*                                           GDebug                                           */
/**********************************************************************************************/
Void gDebug(G1Physical*   self, const char* fmt, ...);
Void gDebug(G2DataLink*   self, const char* fmt, ...);
Void gDebug(G3Network*    self, const char* fmt, ...);
Void gDebug(G4Transport*  self, const char* fmt, ...);
Void gDebug(G4TransportL* self, const char* fmt, ...);
Void gDebug(G5App*        self, const char* fmt, ...);
Void gOutput(GMachine* machine, int layer, const char* string);
/**********************************************************************************************/
extern Bool       g_enableOutput;
extern GContext   g_context;
extern Bool       g_logTables;
/**********************************************************************************************/
inline Void gSetContext(const GContext& context)
{
  g_context         = context;
}
/**********************************************************************************************/
inline Void gSetContext(U8 currentFrame, Int currentType)
{
  g_context.id      = currentFrame;
  g_context.type    = currentType;
}
/**********************************************************************************************/

#endif /* GRID_UTIL_GDEBUG_HH */
