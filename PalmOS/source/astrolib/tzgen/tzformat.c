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

/* tzformat.c */

#include "tzgen.h"

VAR RECORD formatType format[FORMAT_MAX];
    int nFormat=0;

PROCEDURE formatAddLine(char* line)
BEGIN
  VAR RECORD formatType* fp;
      char a[256],b[256];
      int i,area1;
  
  /* check if really a zone or a rule */
  utilScanInit(line);
  utilScanWord(a);
  IF !strcmp(a,"Link") THEN RETURN END;
  
  IF nFormat==0 THEN /* first time, add "S" and "-" */
    strcpy(format[0].name,"-");
    strcpy(format[1].name,"S");
    nFormat=2
  END;
  
  IF nFormat>=FORMAT_MAX THEN
    printf("\nCould not add format string, increase value of FORMAT_MAX...");
    RETURN
  END;
  /* if first item in zone, skip 2 items */
  IF !strcmp(a,"Zone") THEN
    utilScanWord(a);
    IF !utilSplitLocation(a,&area1,b) THEN
      RETURN
    END;
    utilScanWord(a)
  ELSIF !strcmp(a,"Rule") THEN
    FOR i=0; i<7; i++ DO utilScanWord(a) END
  END;
  /* skip one more */
  utilScanWord(a);
  /* format string */
  utilScanWord(a);
  fp = &format[nFormat];
  FOR i=0; i<nFormat && strcmp(a,format[i].name); i++ DO END;
  IF i==nFormat THEN
    /* add format string */
    fp->used = 0;  /* we don't know yet */
    strcpy(fp->name,a);
    nFormat++
  END
END

PROCEDURE formatPrint()
BEGIN
  VAR int i;
  
  printf("\nFORMAT STRINGS:\nindex1 used name");
  FOR i=0; i<nFormat; i++ DO
    printf("\n%-3d    %-1d    %s",i,format[i].used,format[i].name)
  END;
  printf("\nFORMAT STRINGS\n")
END

PROCEDURE formatSerialize(char* buf, int* plen)
BEGIN
  VAR int i,bitpos,bitpos1,len;
  len=14;
  bitpos1=0;
  bitpos=len*nFormat;
  FOR i=0; i<nFormat; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddUpperString(buf, &bitpos, format[i].name)
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN printf("\nformat:   %5d (%6d bits)",*plen,bitpos) END;
END
