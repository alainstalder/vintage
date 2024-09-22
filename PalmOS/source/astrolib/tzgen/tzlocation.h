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

/* tzlocation.h */

#include "modula.h"

#define LOCATION_MAX 512 /* maximal number of locations */

RECORD locationType
BEGIN
  int  used;         /* used temporarily to eliminate unused elements */
  int  nextLoc;      /* index of next location in same country, 0 means none */
  int  country;      /* index in country */
  int  area;         /* index in area */
  int  zone;         /* index in zone */
  int  lat;          /* latitude  of location, 60*deg+min, north positive */
  int  lon;          /* longitude of location, 60*deg+min, east positive */
  char name[30];     /* name of location */
  char comment[80]   /* comment on location */
END;

extern RECORD locationType location[LOCATION_MAX]; /* all locations */
extern int nLocation;                              /* number of locations */

PROCEDURE locationAddLine(char* line);
/* scan line and add a new location to the global array 'location' */

PROCEDURE locationSort();
/* sorts locations by name (insertion sort) */

PROCEDURE locationQuicksort();
/* sorts locations by name (Quicksort) */

PROCEDURE locationPrint();
/* print list of locations to stdout */

PROCEDURE locationSerialize(char* buf, int* plen);
/* serialize locations into a buffer,
 * the length of the buffer is returned in *plen
 */