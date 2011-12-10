#include <Grid/App/G5App.hh>
#include <Grid/Bottom/G1Physical.hh>
#include <Grid/Bottom/G2DataLink.hh>
#include <Grid/Bottom/G3Network.hh>
#include <Grid/Bottom/G4Transport.hh>
#include <Grid/Bottom/GFrame.hh>
#include <Grid/Top/GMachine.hh>
#include <Grid/Util/GDebug.hh>
#include <stdarg.h>

/**********************************************************************************************/
Bool      g_enableOutput    = true;
GContext  g_context         = { 0, Undefined };
Bool      g_logTables       = false;
Int       g_numMessages     = 0;
/********************************************* TX *********************************************/
/*                                           GDebug                                           */
/**********************************************************************************************/

void gDebug(G1Physical* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L1: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->layer2()->layer3()->layer4()->machine(), 1, output);
}

/**********************************************************************************************/

void gDebug(G2DataLink* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L2: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->layer3()->layer4()->machine(), 2, output);
}

/**********************************************************************************************/

void gDebug(G3Network* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L3: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->layer4()->machine(), 3, output);
}

/**********************************************************************************************/

void gDebug(G4Transport* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L4: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->machine(), 4, output);
}

/**********************************************************************************************/

void gDebug(G4TransportL* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L4: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->global()->machine(), 4, output);
}

/**********************************************************************************************/

void gDebug(G5App* self, const char* fmt, ...)
{
  va_list   args;
  char      format[128];
  char      output[128];

  sprintf(format, "L5: %s\n", fmt);
  va_start(args, fmt);
  vsprintf(output, format, args);
  va_end(args);
  gOutput(self->machine(), 5, output);
}

/**********************************************************************************************/

void gOutput(GMachine* machine, int layer, const char* string)
{
  G_GUARD(g_enableOutput, Vacuum);

  /*switch (layer)
  {
    case 1: return;
    case 2: return;
    case 3: return;
  }*/

  if (g_context.type == RoutingTable)
    return;

  int indent = 5 - layer;
  printf("%-10s|#%04x|", machine->name().constData(), U4(g_context.id & 0xFFFF));
  for (int i = 0; i < indent; i++)
    printf("  ");
  fputs(string, stdout);

  /*if (g_numMessages++ > 500000)
  {
    printf("[MESSAGE LIMIT REACHED, ABORTING]\n");
    exit(0);
  }*/
}

/**********************************************************************************************/
