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

/* tzcountry.h */

#include "modula.h"

#define COUNTRY_MAX 256 /* maximal number of countries */

RECORD countryType
BEGIN
  int  used;         /* used temporarily to eliminate unused elements */
  int  firstLoc;     /* index of first location in country */
  char code[3];      /* two character ISO 3166 country code */
  char name[40]      /* country name */
END;

extern RECORD countryType country[COUNTRY_MAX]; /* all countries */
extern int nCountry;                            /* number of countries */

PROCEDURE countryAddLine(char* line);
/* scan line and add a new country to the global array 'country' */

PROCEDURE countrySort();
/* sorts countries by name */

PROCEDURE countryPrint();
/* print list of countries to stdout */

FUNCTION int countryTrim();
/* remove countries without location, update info in locations
 * return the number of removed countries
 */

PROCEDURE countrySerialize(char* buf, int* plen);
/* serialize countries into a buffer,
 * the length of the buffer is returned in *plen
 */