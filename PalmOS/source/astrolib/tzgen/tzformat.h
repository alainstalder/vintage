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

/* tzformat.h */

#include "modula.h"

#define FORMAT_MAX      512 /* maximal number of format strings */

RECORD formatType
BEGIN
  int  used;     /* used temporarily to eliminate unused elements */
  char name[10]  /* format string, short zone name, e.g. CE%sT, %s replaced by rules */
END;

extern RECORD formatType format[FORMAT_MAX]; /* all format strings */
extern int nFormat;                        /* number of format strings */

PROCEDURE formatAddLine(char* line);
/* scan line and add a new format string to the global array 'format' */

PROCEDURE formatPrint();
/* print list of format strings to stdout */

PROCEDURE formatSerialize(char* buf, int* plen);
/* serialize format strings into a buffer,
 * the length of the buffer is returned in *plen
 */
