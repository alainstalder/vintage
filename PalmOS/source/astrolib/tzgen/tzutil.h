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

/* tzutil.h */

#include "modula.h"

extern char utilMonth[12][4];
extern char utilWeekday[7][4];

FUNCTION int utilReadLine(FILE* fp, char* line);
/* read one text line from file fp
 * remove comments indicated by '#'
 * skip lines that consist only of comments or whitespace
 * return non-zero value if end of file has been reached
 */

PROCEDURE utilScanInit(char* line);
/* initialize scanner with text in line */

PROCEDURE utilScanWord(char* word);
/* skip characters <= space and tabs, return word that
 * consists of following sequence of characters > space
 */

PROCEDURE utilScanRest(char* rest);
/* skip characters <= space, return the rest of the line */

FUNCTION char* utilSubString(char* str, int from, int len, char* substr);
/* extract substring 'substr' from 'str', 'len' characters
 * starting at index 'from', return substr
 */
 
FUNCTION int utilStrCaselessCmp(char* str1, char* str2);
/* use tolower() on all chars, then do strcmp() */

PROCEDURE utilCommonizeName(char* name);
/* convert location names to a more common format
 * replace underscores by space and more
 */
 
FUNCTION int utilSplitLocation(char* loc, int* parea, char* name);
/* split location in format <area>/<name> into index of area
 * in areas and commonized name, return non-zero if successful
 */

FUNCTION int utilGetMonth(char* str, int* pmonth);
/* convert 3 character month code to number, Jan=1
 * return non-zero if successful
 */

FUNCTION int utilGetWeekDay(char* str, int* pweekday);
/* convert 3 character weekday code to number, Sun=0,...,Sat=6
 * return non-zero if successful
 */

FUNCTION int utilGetDay(char* str, int* pdaytype, int* pday, int* pweekday);
/* convert day indicated by e.g. "Mon>=21","12","lastSun" to day type,
 * day of month and weekday, return non-zero if successful
 */

FUNCTION int utilGetTime(char* str, int* ptime, int* ptimerel);
/* convert time to hrs+60+min, round secs to next min
 * return non-zero if successful
 */
 
FUNCTION int utilCheckData();
/* check data for unused items, prints out these unused items
 * return the number of unused items
 */
 
FUNCTION int utilLinkLocations();
/* link locations of same country */

PROCEDURE utilAddBits(char* buf, int* pbitpos, char* bits, int bitlen);
/* add bits indicated by bits buffer and bitlen to 
 * buffer buf at bit position *pbitpos, set *pbitpos
 */
 
PROCEDURE utilAddChar(char* buf, int* pbitpos, char ch, int nolower);
/* serialize character ch and add result to buffer buf at
 * bit position *pbitpos, set *pbitpos
 * if nolower, the string contains no lower case letters and should
 * be encoded more densely
 */
 
PROCEDURE utilAddString(char* buf, int* pbitpos, char* str);
/* serialize string str and add result to buffer buf at
 * bit position *pbitpos, set *pbitpos
 * string can contain upper and lower case letters
 * and some other symbols
 */
 
PROCEDURE utilAddUpperString(char* buf, int* pbitpos, char* str);
/* serialize string str and add result to buffer buf at
 * bit position *pbitpos, set *pbitpos
 * string can contain upper case letters and %s
 */
 
PROCEDURE utilAddInt(char* buf, int* pbitpos, int x, int numbits);
/* serialize integer x using numbits and add result to buffer buf at
 * bit position *pbitpos, set *pbitpos
 */
 
PROCEDURE utilAddFlag(char* buf, int* pbitpos, int flag);
/* serialize flag to single bit 1 or 0 and add result to buffer buf at
 * bit position *pbitpos, set *pbitpos
 */

PROCEDURE utilExportHeader();
/* generates C header file 'tzdata.h' for palm access to timezone data */
