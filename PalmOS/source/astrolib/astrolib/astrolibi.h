/*
astrolib: Palm OS shared library, calculates planetary positions
          and time zones.

Copyright (C) 2000-05 by Alain Stalder.
(originally distributed under the pseudonym "delphi29@excite.com")

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* astrolibi.h */

#include <PalmOS.h>
#include "astrolib.h"
#include "modula.h"
#include "timezone.h"
#include "tzdata.h"
#include "mathlib.h"


TYPE RECORD
BEGIN
  Int16 year,month,day,time,weekday; /* time: hrs*60+min */
  aLibCal cal
END date;

TYPE RECORD lonLatRDesc
BEGIN
  double N,i,w,a,e,M,lon,lat,r
END lonLatR;

TYPE RECORD
BEGIN
  double d;            /* Julian Day - 2451543.5 */
  double lon;          /* longitude on earth */
  double lat;          /* latitude on earth */
  long flags;
  double plaCoord[11][3];
  double sid;
  double ecl;
  double sunw;
  double sunM;
  double sunx;
  double suny
END aLibEventStruct;

#define FLAG_sun 0x00000001
#define FLAG_moo 0x00000002
#define FLAG_mer 0x00000004
#define FLAG_ven 0x00000008
#define FLAG_mar 0x00000010
#define FLAG_jup 0x00000020
#define FLAG_sat 0x00000040
#define FLAG_ura 0x00000080
#define FLAG_nep 0x00000100
#define FLAG_plu 0x00000200

#define pi 3.1415926535897931160E+00D


/* The pointer to global data is stored in register A4, because
 * it is (almost) impossible to write to global/static variables
 * declared in a library.
 *
 * Part of the globally allocated data is used as a "stack" for
 * functions that would otherwise require a large portion of
 * the stack, which would be likely to overflow the stack in a
 * typical application under Palm OS 2.0.
 */

TYPE RECORD
BEGIN
  UInt16 mathLibRef; /* reference to MathLib */
  UInt16 usecount;   /* number of times the library has been opened */
  DmOpenRef db;      /* astrolib's resources */
  date d[8];         /* date records */
  lonLatR llr;       /* record used for planetary calculations */
  double stack[20]   /* replacement stack */
END aLibData;

FUNCTION Err openMathLib();
PROCEDURE closeMathLib();
FUNCTION aLibData* getData();


/************************************************************/
/* internal functions matching those declared in astrolib.h */
/************************************************************/

FUNCTION aLibErr iLibMapGetItem (aLibMapItemID id, Int16 index, MemPtr item);
FUNCTION aLibErr iLibMapFindByName (aLibMapItemID id, Char* name, Int16* pindex);

FUNCTION aLibErr iLibTimezone (Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex,
  double* julianDay, double* offsStandard, double* offsLocal, Char* zoneName);

FUNCTION aLibErr iLibEventNew (aLibEvent* pe, double julianDay,
  double lon, double lat);
PROCEDURE iLibEventFree (aLibEvent* pe);
FUNCTION aLibErr iLibEventGetItem (aLibEvent e, aLibEventItemID id,
  MemPtr item);
FUNCTION aLibErr iLibEventGetObjectItem (aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item);
