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

/* tzlocation2.c */

#include "tzgen.h"

VAR RECORD location2Type location2[LOCATION2_MAX];
    int nLocation2=0;

PROCEDURE location2AddLine(char* line)
BEGIN
  VAR RECORD location2Type* lp;
      char a[256],b[256];
      int i,sgn,deg,min;
  
  IF nLocation2>=LOCATION2_MAX THEN
    printf("\nCould not add location, increase value of LOCATION2_MAX...")
  ELSE
    /* set data fields */
    lp = &location2[nLocation2];
    utilScanInit(line);
    utilScanWord(a);
    utilCommonizeName(a);
    /* try to find location code in location */
    FOR i=0; i<nLocation && strcmp(a,location[i].name); i++ DO END;
    IF i==nLocation THEN
      printf("\nCould not add location2: location code not found: %s...",line);
      RETURN
    END;
    lp->location = i;
    /* get latitude */
    utilScanWord(a);
    FOR i=0; i<strlen(a) && a[i]!='n' && a[i]!='s'; i++ DO END;
    IF i==0 || i>strlen(a)-3 THEN
      printf("\nCould not add location2: could not decode latitude: %s...",line);
      RETURN
    END;
    sgn = a[i]=='n'?1:-1;
    deg = atoi(utilSubString(a,0,i,b));
    min = atoi(utilSubString(a,i+1,strlen(a)-i+1,b));
    lp->lat = sgn*(deg*60+min);
    /* get longitude */
    utilScanWord(a);
    FOR i=0; i<strlen(a) && a[i]!='e' && a[i]!='w'; i++ DO END;
    IF i==0 || i>strlen(a)-3 THEN
      printf("\nCould not add location2: could not decode longitude: %s...",line);
      RETURN
    END;
    sgn = a[i]=='e'?1:-1;
    deg = atoi(utilSubString(a,0,i,b));
    min = atoi(utilSubString(a,i+1,strlen(a)-i+1,b));
    lp->lon = sgn*(deg*60+min);
    /* get name */
    utilScanRest(lp->name);
    /* set later */
    lp->nextLoc=0;
    nLocation2++
  END
END

PROCEDUREs swap(int i, int j)
BEGIN
  VAR int t;
      char tstr[256];
  strcpy(tstr,location2[i].name);
  strcpy(location2[i].name,location2[j].name);
  strcpy(location2[j].name,tstr);
  t=location2[i].location;
  location2[i].location=location2[j].location;
  location2[j].location=t;
  t=location2[i].lat;
  location2[i].lat=location2[j].lat;
  location2[j].lat=t;
  t=location2[i].lon;
  location2[i].lon=location2[j].lon;
  location2[j].lon=t;
  t=location2[i].nextLoc;
  location2[i].nextLoc=location2[j].nextLoc;
  location2[j].nextLoc=t
END
  
PROCEDURE location2Sort()
BEGIN
  VAR int i,done;
  REPEAT
    done=1;
    FOR i=0; i<nLocation2-1; i++ DO
      IF utilStrCaselessCmp(location2[i].name, location2[i+1].name)>0 THEN
        swap(i,i+1);
        done=0
      END
    END
  UNTIL done DONE
END

PROCEDUREs quicksort(int first, int last)
BEGIN
  VAR int l,r,p;
  l = first; r = last;
  p = l;
  WHILE l<r DO
    WHILE l<r && utilStrCaselessCmp(location2[p].name,location2[r].name)<=0 DO r-- END;
    swap(r,p);
    p = r;
    WHILE l<r && utilStrCaselessCmp(location2[p].name,location2[l].name)>=0 DO l++ END;
    swap(l,p);
    p = l
  END;
  IF p-first>1 THEN quicksort(first,p-1) END;
  IF last-p>1  THEN quicksort(p+1,last)  END
END
      
PROCEDURE location2Quicksort()
BEGIN
  quicksort(0,nLocation2-1)
END

PROCEDURE location2Print()
BEGIN
  VAR int i;
  printf("\nLOCATIONS2:\nindex nextLoc location lat   lon    name");
  FOR i=0; i<nLocation2; i++ DO
    printf("\n%-5d %-7d %-7d  %-5d %-6d %s",i,
      location2[i].nextLoc,location2[i].location,location2[i].lat,location2[i].lon,
      location2[i].name)
  END;
  printf("\nEND LOCATIONS2\n")
END

PROCEDURE location2Serialize(char* buf, int* plen)
BEGIN
  VAR int i,bitpos,bitpos1,len;
  len=19;
  bitpos1=0;
  bitpos=len*nLocation2;
  FOR i=0; i<nLocation2; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddInt(buf, &bitpos, location2[i].nextLoc, 12);
    utilAddInt(buf, &bitpos, location2[i].location, 9);
    utilAddFlag(buf, &bitpos, location2[i].lat>=0?0:1);
    utilAddInt(buf, &bitpos, abs(location2[i].lat), 13);
    utilAddFlag(buf, &bitpos, location2[i].lon>=0?0:1);
    utilAddInt(buf, &bitpos, abs(location2[i].lon), 14);
    utilAddString(buf, &bitpos, location2[i].name)
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN printf("\nlocation2:%5d (%6d bits)",*plen,bitpos) END;
END
