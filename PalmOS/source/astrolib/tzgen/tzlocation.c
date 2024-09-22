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

/* tzlocation.c */

#include "tzgen.h"

VAR RECORD locationType location[LOCATION_MAX];
    int nLocation=0;

PROCEDURE locationAddLine(char* line)
BEGIN
  VAR RECORD locationType* lp;
      char a[256],b[256];
      int i,sgn,deg,min,sec;
  
  IF nLocation>=LOCATION_MAX THEN
    printf("\nCould not add location, increase value of LOCATION_MAX...")
  ELSE
    /* set data fields */
    lp = &location[nLocation];
    lp->used = 0; /* we don't know yet */
    utilScanInit(line);
    utilScanWord(a);
    /* try to find country code in country */
    FOR i=0; i<nCountry && strcmp(a,country[i].code); i++ DO END;
    IF i==nCountry THEN
      printf("\nCould not add location: country code not found: %s...",line);
      RETURN
    END;
    lp->country = i;
    /* get latitude and longitude */
    utilScanWord(a);
    FOR i=1; i<strlen(a) && a[i]!='+' && a[i]!='-'; i++ DO END;
    IF i==strlen(a) THEN
      printf("\nCould not add location: could not decode lat/lon: %s...",line);
      RETURN
    END;
    /* lat */
    sgn = a[0]=='+'?1:-1;
    deg = atoi(utilSubString(a,1,2,b));
    min = atoi(utilSubString(a,3,2,b));
    sec = i>5?atoi(utilSubString(a,5,2,b)):0;
    IF sec>=30 THEN min++ END;
    IF min==60 THEN deg++; min=0 END;
    lp->lat = sgn*(deg*60+min);
    /* lon */
    sgn = a[i]=='+'?1:-1;
    deg = atoi(utilSubString(a,i+1,3,b));
    min = atoi(utilSubString(a,i+4,2,b));
    sec = strlen(a)-i>6?atoi(utilSubString(a,i+6,2,b)):0;
    IF sec>=30 THEN min++ END;
    IF min==60 THEN deg++; min=0 END;
    lp->lon = sgn*(deg*60+min);
    /* get area and location name */
    utilScanWord(a);
    IF !utilSplitLocation(a,&lp->area,lp->name) THEN
      printf("\nCould not add location: could not split location: %s...",line);
      RETURN
    END;      
    /* get comment */
    utilScanRest(lp->comment);
    /* zone and nextLoc will be set later */
    lp->zone=-1;
    lp->nextLoc=0;
    nLocation++
  END
END

PROCEDUREs swap(int i, int j)
BEGIN
  VAR int t;
      char tstr[256];
  strcpy(tstr,location[i].name);
  strcpy(location[i].name,location[j].name);
  strcpy(location[j].name,tstr);
  strcpy(tstr,location[i].comment);
  strcpy(location[i].comment,location[j].comment);
  strcpy(location[j].comment,tstr);
  t=location[i].used;
  location[i].used=location[j].used;
  location[j].used=t;
  t=location[i].nextLoc;
  location[i].nextLoc=location[j].nextLoc;
  location[j].nextLoc=t;
  t=location[i].country;
  location[i].country=location[j].country;
  location[j].country=t;
  t=location[i].area;
  location[i].area=location[j].area;
  location[j].area=t;
  t=location[i].zone;
  location[i].zone=location[j].zone;
  location[j].zone=t;
  t=location[i].lat;
  location[i].lat=location[j].lat;
  location[j].lat=t;
  t=location[i].lon;
  location[i].lon=location[j].lon;
  location[j].lon=t
END

PROCEDURE locationSort()
BEGIN
  VAR int i,done;
  REPEAT
    done=1;
    FOR i=0; i<nLocation-1; i++ DO
      IF utilStrCaselessCmp(location[i].name, location[i+1].name)>0 THEN
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
    WHILE l<r && utilStrCaselessCmp(location[p].name,location[r].name)<=0 DO r-- END;
    swap(r,p);
    p = r;
    WHILE l<r && utilStrCaselessCmp(location[p].name,location[l].name)>=0 DO l++ END;
    swap(l,p);
    p = l
  END;
  IF p-first>1 THEN quicksort(first,p-1) END;
  IF last-p>1  THEN quicksort(p+1,last)  END
END
      
PROCEDURE locationQuicksort()
BEGIN
  quicksort(0,nLocation-1)
END


PROCEDURE locationPrint()
BEGIN
  VAR int i;
  
  printf("\nLOCATIONS:\nindex used nextLoc country area zone lat   lon    name"
         "             comment");
  FOR i=0; i<nLocation; i++ DO
    printf("\n%-5d %-4d %-7d %-7d %-4d %-4d %-5d %-6d %-16s %s",i,location[i].used,
      location[i].nextLoc,location[i].country,location[i].area,location[i].zone,
      location[i].lat,location[i].lon,location[i].name,location[i].comment)
  END;
  printf("\nEND LOCATIONS\n")
END

PROCEDURE locationSerialize(char* buf, int* plen)
BEGIN
  VAR int i,bitpos,bitpos1,len;
  len=17;
  bitpos1=0;
  bitpos=len*nLocation;
  FOR i=0; i<nLocation; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddInt(buf, &bitpos, location[i].nextLoc, 12);
    utilAddInt(buf, &bitpos, location[i].country, 9);
    utilAddInt(buf, &bitpos, location[i].area, 4);
    utilAddInt(buf, &bitpos, location[i].zone, 9);
    utilAddFlag(buf, &bitpos, location[i].lat>=0?0:1);
    utilAddInt(buf, &bitpos, abs(location[i].lat), 13);
    utilAddFlag(buf, &bitpos, location[i].lon>=0?0:1);
    utilAddInt(buf, &bitpos, abs(location[i].lon), 14);
    utilAddString(buf, &bitpos, location[i].name);
    utilAddString(buf, &bitpos, location[i].comment)
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN printf("\nlocation: %5d (%6d bits)",*plen,bitpos) END;
END
