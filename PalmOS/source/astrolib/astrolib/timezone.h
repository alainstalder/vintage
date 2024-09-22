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

/* timezone.h */

#define PARITY_ALL    0
#define PARITY_ODD    1
#define PARITY_EVEN   2

#define DAY_START_EQUAL 0  /* rule starts at day of month (each year) */
#define DAY_START_LEQ   1  /* rule starts at weekday <= day of month */
#define DAY_START_GEQ   2  /* rule starts at weekday >= day of month */
#define DAY_START_LAST  3  /* rule starts at last weekday of month */

#define REL_UTC         0  /* time indicated in UTC */
#define REL_STANDARD    1  /* standard time */
#define REL_LOCAL       2  /* local time (standard time + rule) */

#define OFFSET_DIRECT   0 /* offset is indicated directly, hrs*60+min */
#define OFFSET_BY_RULE  1 /* offset is indicated by index in rule */

#define FORMAT_INDEX_N  0 /* offset of string "-" in format */
#define FORMAT_INDEX_S  1 /* offset of string "S" in format */

TYPE RECORD
BEGIN
  Int16 firstLoc; /* first location in country */
  Char code[3];   /* two character ISO 3166 country code */
  Char name[40]   /* country name */
END country;

TYPE RECORD
BEGIN
  Char name[20] /* area name */
END area;

TYPE RECORD
BEGIN
  Int16 nextLoc;     /* next location in same country */
  Int16 country;     /* index in country */
  Int16 area;        /* index in area */
  Int16 zone;        /* index in zone */
  Int16 lat;         /* latitude  of location, 60*deg+min, north positive */
  Int16 lon;         /* longitude of location, 60*deg+min, east positive */
  Char  name[30];    /* name of location */
  Char  comment[80]  /* comment on location */
END location;

TYPE RECORD
BEGIN
  Char name[10]  /* short zone name (e.g. CE%T, % replaced by rules) */
END format;

TYPE RECORD
BEGIN
  Int16 yearFrom;     /* first year rule applies to (except parity) */
  Int16 yearTo;       /* last year rule applies to (except parity) */
  Int16 yearParity;   /* range applies to all/only odd/only even years */
  Int16 startMonth;   /* first month (of each year) the rule applies to */
  Int16 startDayType; /* equal, leq, geq or last */
  Int16 startDay;     /* day of month */
  Int16 startWeekday; /* day of week (Sun=0), no meaning if type is equal */
  Int16 startTime;    /* hrs*60+min, secs rounded to next minute */
  Int16 startTimeRel; /* UTC/standard/local time */
  Int16 saveTime;     /* time saved (hrs+60+min, secs rounded) */
  Int16 saveString    /* index in format, replaces %s in zone format */
END rule;

TYPE RECORD
BEGIN
  Int16 offsStandard; /* offset UTC to standard, hrs*60+min, secs rounded */
  Int16 offsType;     /* type of offset to local time, direct or by rule */
  Int16 offsLocal;    /* offset standard to local, hrs*60+min, secs rounded */
  Int16 offsRule;     /* index in rule */
  Int16 format;       /* index in format */
  Int16 untilYear;    /* last year that zone info applies to */
  Int16 untilMonth;   /* last month that zone info applies to */
  Int16 untilDayType; /* equal, leq, geq or last */
  Int16 untilDay;     /* day of month */
  Int16 untilWeekday; /* day of week (Sun=0), no meaning if type is equal */
  Int16 untilTime;    /* hrs*60+min, secs rounded to next minute */
  Int16 untilTimeRel  /* UTC/standard/local time */
END zone;

TYPE RECORD
BEGIN
  Int16 nextLoc;     /* next location in same country */
  Int16 location;    /* index in location */
  Int16 lat;         /* latitude  of location2, 60*deg+min, north positive */
  Int16 lon;         /* longitude of location2, 60*deg+min, east positive */
  Char  name[30]     /* name of location2 */
END location2;

FUNCTION Int16 tzCountry(Int16 i, country* c);
/* get country with index i 
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 tzArea(Int16 i, area* a);
/* get area with index i 
 * return non-zero if successful, zero otherwise
 */
 
FUNCTION Int16 tzLocation(Int16 i, location* l);
/* get location with index i 
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 tzFormat(Int16 i, format* f);
/* get format string with index i 
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 tzRule(Int16 i, Int16 j, rule* r);
/* get rule item j of rule i
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 tzZone(Int16 i, Int16 j, zone* r);
/* get zone item j of zone i
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 tzLocation2(Int16 i, location2* l);
/* get location2 with index i 
 * return non-zero if successful, zero otherwise
 */

FUNCTION Int16 _tzCountry(Int16 i, country* c, Boolean full);
/* if full is false only firstLoc is retrieved */

FUNCTION Int16 _tzLocation(Int16 i, location* l, Boolean full);
/* if full is false only nextLoc is retrieved */

FUNCTION Int16 _tzLocation2(Int16 i, location2* l, Boolean full);
/* if full is false only firstLoc is retrieved */