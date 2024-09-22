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

/* tzarea.h */

#include "modula.h"

#define AREA_MAX 16 /* maximal number of areas */

RECORD areaType
BEGIN
  int  used;         /* used temporarily to eliminate unused elements */
  char name[20]      /* area name */
END;

extern RECORD areaType area[AREA_MAX]; /* all areas */
extern int nArea;                      /* number of areas */

PROCEDURE areaAddLine(char* line);
/* scan line and add a new area to the global array 'area' */

PROCEDURE areaSort();
/* sorts areas by name */

PROCEDURE areaPrint();
/* print list of areas to stdout */

PROCEDURE areaSerialize(char* buf, int* plen);
/* serialize areas into a buffer,
 * the length of the buffer is returned in *plen
 */