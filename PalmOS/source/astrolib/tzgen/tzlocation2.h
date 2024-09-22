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

/* tzlocation2.h */

#include "modula.h"

#define LOCATION2_MAX 4096 /* maximal number of additional locations */

RECORD location2Type
BEGIN
  int  nextLoc;      /* index of next location in same country, 0 means none */
  int  location;     /* index in location */
  int  lat;          /* latitude  of location2, 60*deg+min, north positive */
  int  lon;          /* longitude of location2, 60*deg+min, east positive */
  char name[30]      /* name of location2 */
END;

extern RECORD location2Type location2[LOCATION2_MAX]; /* all additional locations */
extern int nLocation2;                                /* number of additional locations */

PROCEDURE location2AddLine(char* line);
/* scan line and add a new location to the global array 'location2' */

PROCEDURE location2Sort();
/* sorts additional locations by name (insertion sort) */

PROCEDURE location2Quicksort();
/* sorts additional locations by name (Quicksort) */

PROCEDURE location2Print();
/* print list of additional locations to stdout */

PROCEDURE location2Serialize(char* buf, int* plen);
/* serialize additional locations into a buffer,
 * the length of the buffer is returned in *plen
 */