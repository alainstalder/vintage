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

/* tzzone.c */

#include "tzgen.h"

VAR RECORD zoneType zone[ZONE_MAX];
    int nZone=0;
    int nLink=0;

PROCEDURE zoneAddLine(char* line)
BEGIN
  VAR RECORD zoneType* zp;
      char a[256],b[256];
      int i,j,dummy,area1;
      
  /* check if really a zone */
  utilScanInit(line);
  utilScanWord(a);
  IF !strcmp(a,"Link") || !strcmp(a,"Rule") THEN RETURN END;
  
  /* if first item in zone, add zone */
  IF !strcmp(a,"Zone") THEN
    /* location */
    utilScanWord(a);
    IF !utilSplitLocation(a,&area1,b) THEN
      IF tzverbose THEN
        printf("\nCould not add zone: could not split location %s...",line)
      END;
      RETURN
    END;
    utilCommonizeName(b);
    FOR i=0; i<nLocation && (area1!=location[i].area 
      || strcmp(b,location[i].name)); i++ DO END;
    IF i==nLocation THEN
      printf("\nCould not add zone: could not find location %s...",line);
      RETURN
    END;
    zone[nZone].used = 0; /* we don't know yet */
    location[i].zone = nZone;
    zone[nZone].nZoneItem=0;
    nZone++;
    utilScanWord(a);
  END;
  zp = &zone[nZone-1];
  j = zp->nZoneItem; /* shorter */
  /* offset to standard time */
  IF !utilGetTime(a,&zp->offsStandard[j],&dummy) THEN
    printf("\nCould not add zone: unknown offset to standard time %s...",line);
    RETURN
  END;
  /* direct offset or by rule */
  utilScanWord(a);
  IF !strcmp(a,"-") THEN
    zp->offsType[j] = OFFSET_DIRECT;
    zp->offsLocal[j] = 0;
    zp->offsRule[j] = 999 /* does not matter */
  ELSIF '0'<=a[0] && a[0]<='9' THEN
    zp->offsType[j] = OFFSET_DIRECT;
    IF !utilGetTime(a,&zp->offsLocal[j],&dummy) THEN
      printf("\nCould not add zone: unknown offset to local time %s...",line);
      RETURN
    END;
    zp->offsRule[j] = 999 /* does not matter */
  ELSE
    zp->offsType[j] = OFFSET_BY_RULE;
    zp->offsLocal[j] = YEAR_MAX; /* does not matter */
    /* search rule */
    FOR i=0; i<nRule && strcmp(a,rule[i].name); i++ DO END;
    IF i==nRule THEN
      printf("\nCould not add zone: unknown rule %s...",line);
      RETURN
    END;
    zp->offsRule[j] = i
  END;
  /* format string for short name of time zone */
  utilScanWord(a);
  FOR i=0; i<nFormat && strcmp(a,format[i].name); i++ DO END;
  IF i==nFormat THEN
    printf("\nCould not add zone: format string not found %s...",line);
    RETURN
  END;
  zp->format[j]=i;
  /* until year */
  utilScanWord(a);
  IF !strcmp(a,"") || !strcmp(a,"max") THEN
    zp->untilYear[j] = YEAR_MAX
  ELSE zp->untilYear[j] = atoi(a)
  END;
  /* until month */
  utilScanWord(a);
  IF !strcmp(a,"") THEN
    zp->untilMonth[j] = 12 /* Dec */
  ELSIF !utilGetMonth(a,&zp->untilMonth[j]) THEN
    printf("\nCould not add zone: unknown month %s...",a);
    RETURN
  END;
  /* until day */
  utilScanWord(a);
  IF !strcmp(a,"") THEN
    zp->untilDayType[j] = DAY_START_EQUAL;
    zp->untilDay[j]     = 31;
    zp->untilWeekday[j] = 9 /* does not matter */
  ELSIF !utilGetDay(a,&zp->untilDayType[j],&zp->untilDay[j],
    &zp->untilWeekday[j]) THEN
    printf("\nCould not add zone: unknown day %s...",line);
    RETURN
  END;
  /* until time */
  utilScanWord(a);
  IF !strcmp(a,"") THEN
    zp->untilTime[j]    = 23*60+59;
    zp->untilTimeRel[j] = REL_LOCAL
  ELSIF !utilGetTime(a,&zp->untilTime[j],&zp->untilTimeRel[j]) THEN
    printf("\nCould not add zone: unknown time %s...",line);
    RETURN
  END;
  zp->nZoneItem++
END

PROCEDURE zoneAddLink(char* line)
BEGIN
  VAR char a[256],b[256];
      int i,j,area1;
  
  /* check if really a link */
  utilScanInit(line);
  utilScanWord(a);
  IF strcmp(a,"Link") THEN RETURN END;
  /* get original and try to find it in locations */
  utilScanWord(a);
  IF !utilSplitLocation(a,&area1,b) THEN
    IF tzverbose THEN
      printf("\nLink with non-existing original area: %s...",a)
    END;
    RETURN
  END;
  FOR i=0; i<nLocation && (area1!=location[i].area 
    || strcmp(b,location[i].name)); i++ DO END;
  IF i==nLocation THEN
    IF tzverbose THEN
      printf("\nLink with non-existing original location: %s...",a)
    END;
    RETURN
  END;
  /* get link and try to find it in locations */
  utilScanWord(a);
  IF !utilSplitLocation(a,&area1,b) THEN
    IF tzverbose THEN
      printf("\nLink with non-existing link area: %s...",a)
    END;
    RETURN
  END;
  FOR j=0; j<nLocation && (area1!=location[j].area 
    || strcmp(b,location[j].name)); j++ DO END;
  IF j==nLocation THEN
    IF tzverbose THEN
      printf("\nLink with non-existing link location: %s...",a)
    END;
    RETURN
  END;
  /* set linked zone in location */
  location[j].zone = location[i].zone;
  nLink++
END

PROCEDURE zonePrint()
BEGIN
  VAR int i,j;
  
  printf("\nZONES:\nindex1  used         offsType    offsRule     "
    "untilYear  dayType     time rel\n  index2       offsStandard offsLocal  "
    "format       month   day weekday");
  FOR i=0; i<nZone /*&& i<20*/; i++ DO
    FOR j=0; j<zone[i].nZoneItem; j++ DO
      printf("\n%-3d %-3d %-1d      %-4d  %-1d      %-4d %-3d   %-6d "
      "%-4d  %-2d   %-1d  %-2d  %-1d    %-4d %-1d",i,j, zone[i].used,
      zone[i].offsStandard[j], zone[i].offsType[j], zone[i].offsLocal[j],
      zone[i].offsRule[j], zone[i].format[j], zone[i].untilYear[j],
      zone[i].untilMonth[j], zone[i].untilDayType[j], zone[i].untilDay[j],
      zone[i].untilWeekday[j], zone[i].untilTime[j], zone[i].untilTimeRel[j])
    END
  END;
  printf("\nEND ZONES\n")
END

FUNCTION int zoneItemNum()
BEGIN
  VAR int i,n=0;
  FOR i=0; i<nZone; i++ DO n+=zone[i].nZoneItem END;
  RETURN n
END

FUNCTION int zoneItemMax()
BEGIN
  VAR int i,n=0;
  FOR i=0; i<nZone; i++ DO
    IF zone[i].nZoneItem > n THEN n = zone[i].nZoneItem END
  END;
  RETURN n
END

PROCEDURE zoneSerialize(char* buf, int* plen)
BEGIN
  VAR int i,j,x,d,max,len,leni;
      int bitpos,bitpos1,bitposi,bitposi1;
  len=17;
  bitpos1=0;
  bitpos=len*nZone;
  max=0;
  FOR i=0; i<nZone; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddInt(buf,&bitpos,zone[i].nZoneItem,4);
    leni=10;
    bitposi=bitpos;
    bitposi1=bitpos;
    bitpos+=leni*zone[i].nZoneItem;
    FOR j=0; j<zone[i].nZoneItem; j++ DO
      utilAddInt(buf,&bitposi1,bitpos-bitposi,leni);
      /* offset to standard time, incl. sign */
      x = zone[i].offsStandard[j];
      utilAddFlag(buf, &bitpos, x<0);
      x=abs(x);
      IF x%60==0 && x<8*60 THEN
        utilAddFlag(buf, &bitpos, 1);
        utilAddInt(buf, &bitpos, x/60, 3)
      ELSE 
        utilAddFlag(buf, &bitpos, 0);
        utilAddInt(buf, &bitpos, x, 11)
      END;
      /* direct offset or by rule */
      x = zone[i].offsType[j];
      utilAddFlag(buf, &bitpos, x);
      IF x==OFFSET_DIRECT THEN
        x = zone[i].offsLocal[j]; /* no sign */
        IF x%60==0 && x/60<2 THEN
          utilAddFlag(buf, &bitpos, 1);
          utilAddFlag(buf, &bitpos, x)
        ELSE 
          utilAddFlag(buf, &bitpos, 0);
          utilAddInt(buf, &bitpos, x, 11)
        END
      ELSE /* x==OFFSET_BY_RULE */
        utilAddInt(buf, &bitpos, zone[i].offsRule[j], 10)
      END;
      /* format */
      utilAddInt(buf, &bitpos, zone[i].format[j], 10);
      /* year until */
      x = zone[i].untilYear[j];
      x = x==YEAR_MAX?255:x-1800;
      utilAddInt(buf, &bitpos, x, 8);
      /* until month */
      utilAddInt(buf, &bitpos, zone[i].untilMonth[j], 4);
      /* until day type */
      x = zone[i].untilDayType[j];
      utilAddInt(buf, &bitpos, x, 2);
      IF x==DAY_START_EQUAL THEN
        /* until day */
        utilAddInt(buf, &bitpos, zone[i].untilDay[j], 5)
      ELSIF x==DAY_START_LEQ || x==DAY_START_GEQ THEN
        /* until day and day of week */
        utilAddInt(buf, &bitpos, zone[i].untilDay[j], 5);
        utilAddInt(buf, &bitpos, zone[i].untilWeekday[j], 3)
      ELSE /* DAY_START_LAST */
        /* day of week */
        utilAddInt(buf, &bitpos, zone[i].untilWeekday[j], 3)
      END;
      /* until time */
      utilAddInt(buf, &bitpos, zone[i].untilTime[j], 11);
      /* until time relative to */
      utilAddInt(buf, &bitpos, zone[i].untilTimeRel[j], 2);
      d = bitpos-bitposi+4;
      IF d>max THEN max=d END
    END
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN
    printf("\nzone:     %5d (%6d bits, max=%4d bits)",*plen,bitpos,max)
  END
END
