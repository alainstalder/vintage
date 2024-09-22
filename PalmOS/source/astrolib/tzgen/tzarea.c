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

/* tzarea.c */

#include "tzgen.h"

VAR RECORD areaType area[AREA_MAX];
    int nArea=0;

PROCEDURE areaAddLine(char* line)
BEGIN
  VAR RECORD areaType* ap;
      char a[256];
      int i;
  
  IF nArea>=AREA_MAX THEN
    printf("\nCould not add area, increase value of AREA_MAX...");
    RETURN
  END;
  /* set data fields */
  utilScanInit(line);
  utilScanWord(a); utilScanWord(a); utilScanWord(a);
  /* scan until slash */
  FOR i=0; a[i]!='\0' && a[i]!='/'; i++ DO END;
  IF  a[i]=='\0' THEN
    printf("\nCould not add area: %s...",line);
    RETURN;
  END;
  a[i]='\0';
  /* check if area is alread in list */
  FOR i=0; i<nArea && strcmp(a,area[i].name); i++ DO END;
  IF i==nArea THEN
    ap = &area[nArea];
    ap->used = 0; /* we don't know yet */
    strcpy(ap->name,a);
    nArea++
  END;
  areaSort()
END

PROCEDURE areaSort()
BEGIN
  VAR int i,done,t;
      char tstr[256];
  REPEAT
    done=1;
    FOR i=0; i<nArea-1; i++ DO
      IF utilStrCaselessCmp(area[i].name, area[i+1].name)>0 THEN
        /* swap */
        strcpy(tstr,area[i].name);
        strcpy(area[i].name,area[i+1].name);
        strcpy(area[i+1].name,tstr);
        t=area[i].used;
        area[i].used=area[i+1].used;
        area[i+1].used=t;
        done=0
      END
    END
  UNTIL done DONE
END

PROCEDURE areaPrint()
BEGIN
  VAR int i;
  
  printf("\nAREAS:\nindex used name");
  FOR i=0; i<nArea; i++ DO
    printf("\n%-5d %-4d %s",i,area[i].used,area[i].name)
  END;
  printf("\nEND AREAS\n")
END

PROCEDURE areaSerialize(char* buf, int* plen)
BEGIN
  VAR int i,bitpos,bitpos1,len;
  len=10;
  bitpos1=0;
  bitpos=len*nArea;
  FOR i=0; i<nArea; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddString(buf,&bitpos,area[i].name)
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN printf("\narea:     %5d (%6d bits)",*plen,bitpos) END;
END
