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

/* tzzone.h */

#include "modula.h"

#define ZONE_MAX      512 /* maximal number of zones */
#define ZONE_ITEM_MAX  32 /* maximal number of zone items */

#define OFFSET_DIRECT   0 /* offset is indicated directly, hrs*60+min */
#define OFFSET_BY_RULE  1 /* offset is indicated by index in rule */

RECORD zoneType
BEGIN
  int  used;       /* used temporarily to eliminate unused elements */
  int  nZoneItem;  /* number of zone items for this zone */
  int  offsStandard[ZONE_ITEM_MAX]; /* offset UTC to standard, hrs*60+min, secs rounded */
  int  offsType    [ZONE_ITEM_MAX]; /* type of offset to local time, direct or by rule */
  int  offsLocal   [ZONE_ITEM_MAX]; /* offset standard to local, hrs*60+min, secs rounded */
  int  offsRule    [ZONE_ITEM_MAX]; /* index in rule */
  int  format      [ZONE_ITEM_MAX]; /* index in format */
  int  untilYear   [ZONE_ITEM_MAX]; /* last year that zone info applies to */
  int  untilMonth  [ZONE_ITEM_MAX]; /* last month that zone info applies to */
  int  untilDayType[ZONE_ITEM_MAX]; /* equal, leq, geq or last */
  int  untilDay    [ZONE_ITEM_MAX]; /* day of month */
  int  untilWeekday[ZONE_ITEM_MAX]; /* day of week, no meaning if type is equal */
  int  untilTime   [ZONE_ITEM_MAX]; /* hrs*60+min, secs rounded to next minute */
  int  untilTimeRel[ZONE_ITEM_MAX]  /* UTC/standard/local time */
END;

extern RECORD zoneType zone[ZONE_MAX]; /* all zones */
extern int nZone;                      /* number of zones */
extern int nLink;                      /* number of links read */

PROCEDURE zoneAddLine(char* line);
/* scan line and add a new zone or zone item to the global array 'zone' */

PROCEDURE zoneAddLink(char* line);
/* scan line for link and set zone in global array 'location' accordingly */

PROCEDURE zonePrint();
/* print list of zones to stdout */

FUNCTION int zoneItemNum();
/* returns the total number of zone items */

FUNCTION int zoneItemMax();
/* returns the largest number of zone items per zone */

PROCEDURE zoneSerialize(char* buf, int* plen);
/* serialize zones into a buffer,
 * the length of the buffer is returned in *plen
 */
