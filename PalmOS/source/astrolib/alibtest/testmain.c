/* testmain.c */

#include <PalmOS.h>
#include "modula.h"
#include "astrolib.h"

aLibErr aLibTest();

FUNCTION UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
BEGIN
  IF cmd==sysAppLaunchCmdNormalLaunch THEN aLibTest() END;
  RETURN 0
END