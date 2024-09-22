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

/* astrolibi.c */

#include "astrolibi.h"


/**********************/
/* access to map data */
/**********************/

FUNCTION aLibErr iLibMapGetItem (aLibMapItemID id, Int16 index, MemPtr item)
BEGIN
  VAR country c; area a; location l; location2 l2;
      Int16 i;
  
  IF item==0 THEN RETURN aLibErrIllegalParameters END;
  CASE id OF
    case mapAreas:           *((Int16*)item) = NAREA; break;
    case mapAreaName:        IF !tzArea(index,&a) THEN RETURN aLibErrNotFound END;
                             StrCopy((Char*)item,a.name); break;
    case mapCountries :      *((Int16*)item) = NCOUNTRY; break;
    case mapCountryName:     IF !tzCountry(index,&c) THEN RETURN aLibErrNotFound END;
                             StrCopy((Char*)item,c.name); break;
    case mapCountryCode:     IF !tzCountry(index,&c) THEN RETURN aLibErrNotFound END;
                             StrCopy((Char*)item,c.code); break;
    case mapCountryFirstLoc: IF !_tzCountry(index,&c,FALSE) THEN RETURN aLibErrNotFound END;
                             *((Int16*)item) = c.firstLoc; break;
    case mapLocations:       *((Int16*)item) = NLOCATION+NLOCATION2; break;
    case mapUnixLocations:   *((Int16*)item) = NLOCATION; break;
    
    case mapLocationName:
    case mapLocationArea:
    case mapLocationCountry:
    case mapLocationLon:
    case mapLocationLat:
    case mapLocationInfo:
    case mapLocationUnixLoc:
      IF index<NLOCATION THEN
        IF !tzLocation(index,&l) THEN RETURN aLibErrNotFound END;
        CASE id OF
          case mapLocationName:    StrCopy((Char*)item,l.name); break;
          case mapLocationArea:    *((Int16*)item) = l.area; break;
          case mapLocationCountry: *((Int16*)item) = l.country; break;
          case mapLocationLon:     *((double*)item) = l.lon/(60.0*(180/pi)); break;
          case mapLocationLat:     *((double*)item) = l.lat/(60.0*(180/pi)); break;
          case mapLocationInfo:    StrCopy((Char*)item,l.comment); break;
          case mapLocationUnixLoc: *((Int16*)item) = index; break;
        END; break;
      ELSE
        IF !tzLocation2(index-NLOCATION,&l2) THEN RETURN aLibErrNotFound END;
        i = l2.location;
        IF !tzLocation(i,&l) THEN RETURN aLibErrNotFound END;
        CASE id OF
          case mapLocationName:    StrCopy((Char*)item,l2.name); break;
          case mapLocationArea:    *((Int16*)item) = l.area; break;
          case mapLocationCountry: *((Int16*)item) = l.country; break;
          case mapLocationLon:     *((double*)item) = l2.lon/(60.0*(180/pi)); break;
          case mapLocationLat:     *((double*)item) = l2.lat/(60.0*(180/pi)); break;
          case mapLocationInfo:    StrCopy((Char*)item,l.comment); break;
          case mapLocationUnixLoc: *((Int16*)item) = i; break;
        END 
      END; break;     
    case mapLocationNextLoc:
      IF index<NLOCATION THEN
        IF !_tzLocation(index,&l,FALSE) THEN RETURN aLibErrNotFound END;
        IF !l.nextLoc THEN RETURN aLibErrNotFound END;
        *((Int16*)item) = l.nextLoc
      ELSE
        IF !_tzLocation2(index-NLOCATION,&l2,FALSE) THEN RETURN aLibErrNotFound END;
        IF !l2.nextLoc THEN RETURN aLibErrNotFound END;
         *((Int16*)item) = l2.nextLoc
      END; break;     
    default: RETURN aLibErrNotFound
  END;
  RETURN aLibOK
END

FUNCTION aLibErr iLibMapFindByName (aLibMapItemID id, Char* name, Int16* pindex)
BEGIN
  VAR country c; area a; location l; location2 l2;
      Int16 i,j,k,r;
      
  IF name==0 || pindex==0 THEN RETURN aLibErrIllegalParameters END;  
  CASE id OF
  case mapAreaName:
    /* linear search (it's only 10 areas) */
    FOR i=0; i<NAREA; i++ DO
      IF tzArea(i,&a) THEN
        IF !StrCaselessCompare(name,a.name) THEN
          *pindex=i; RETURN aLibOK
        END
      END
    END;
    break;
  case mapCountryName:
    /* binary search */
    i=-1; j=NCOUNTRY;
    REPEAT
      k=(i+j)/2;
      IF tzCountry(k,&c) THEN
        r = StrCaselessCompare(name,c.name);
        IF !r THEN *pindex=k; RETURN aLibOK
        ELSIF r>0 THEN i=k ELSE j=k
        END
      END
    UNTIL j-i<=1 DONE;
    break;  
  case mapCountryCode:
    /* not sorted, linear search */
    IF StrLen(name)<2 THEN RETURN aLibErrIllegalParameters END;
    FOR i=0; i<NCOUNTRY; i++ DO
      IF tzCountry(i,&c) THEN
        IF !StrNCaselessCompare(name,c.code,2) THEN
          *pindex=i; RETURN aLibOK
        END
      END
    END;
    break;
  case mapLocationName:
    /* binary search in location */
    i=-1; j=NLOCATION;
    REPEAT
      k=(i+j)/2;
      IF tzLocation(k,&l) THEN
        r = StrCaselessCompare(name,l.name);
        IF !r THEN *pindex=k; RETURN aLibOK
        ELSIF r>0 THEN i=k ELSE j=k
        END
      END
    UNTIL j-i<=1 DONE;
    /* binary search in location2 */
    i=-1; j=NLOCATION2;
    REPEAT
      k=(i+j)/2;
      IF tzLocation2(k,&l2) THEN
        r = StrCaselessCompare(name,l2.name);
        IF !r THEN *pindex=NLOCATION+k; RETURN aLibOK
        ELSIF r>0 THEN i=k ELSE j=k
        END
      END
    UNTIL j-i<=1 DONE;
    break
  END;  
  RETURN aLibErrNotFound
END


/***************************/
/* access to timezone data */
/***************************/

/* static functions */
FUNCTIONs  double julDay(date* d);
PROCEDUREs setWeekday(date* d);
FUNCTIONs  Int16 daysInMonth(date* d);
FUNCTIONs  Int16 firstBeforeOrEqual(date* d, Int16 weekday, Int16 day);
FUNCTIONs  Int16 firstAfterOrEqual(date* d, Int16 weekday, Int16 day);
PROCEDUREs dateSet(date* d, Int16 year, Int16 month, Int16 dayType,
             Int16 day, Int16 weekday, Int16 time, aLibCal cal);
PROCEDUREs dateAddTime(date* d, Int16 t);
FUNCTIONs  Int16 dateCompare(date* d1, date* d2);
PROCEDUREs getZoneName(Char* zn, Int16 offsName, Int16 offsReplace);
FUNCTIONs  aLibErr getTimezone(Int16 zoneOffs, date* du, date* ds,
             date* dl, Int16* offsUS, Int16* offsSL, Char* zn);


/* Timezones are resolved as follows:
 * 1) The given date is treated as UTC with time differences to standard and
 *    to local time set to 0. Then time differences are resolved for the date
 *    plus and minus a day. If resolved time differences are equal for both
 *    dates, they are returned.
 * 2) If time differences or names of timezones differ, time differences are
 *    resolved for the given date and both candidates for time differences.
 *    If only one result is self-consistent, it is returned, else the first
 *    result is returned.
 */
   
FUNCTION aLibErr iLibTimezone (Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex,
  double* julianDay, double* offsStandard, double* offsLocal, Char* zoneName)
BEGIN
  VAR aLibData* data;
      location l; location2 l2;
      Int16 offsUS1,offsSL1,offsUS2,offsSL2;
      Int16 offsUS1n,offsSL1n,offsUS2n,offsSL2n;
      Char zn1[10],zn2[10];
      Boolean match1,match2,unixLoc;
      aLibErr err;
      
      #define du1 (data->d[0])
      #define ds1 (data->d[1])
      #define dl1 (data->d[2])
      #define du2 (data->d[3])
      #define ds2 (data->d[4])
      #define dl2 (data->d[5])

  IF month<1 || month>12 || day<1 || day>31 || hour<0 || hour>24
    || min<0 || min>59 || sec<0 || sec>59 
    || (julianDay==0 && offsStandard==0 && offsLocal==0 && zoneName==0) THEN
    RETURN aLibErrIllegalParameters
  END;
  unixLoc=TRUE;
  IF locIndex>=NLOCATION THEN
    unixLoc=FALSE;
    IF !tzLocation2(locIndex-NLOCATION,&l2) THEN RETURN aLibErrNotFound END;
    locIndex = l2.location
  END; 
  IF !tzLocation(locIndex,&l) || cal>calJulian THEN RETURN aLibErrNotFound END;
  
  data = getData();
  dateSet(&du1,year,month,DAY_START_EQUAL,day,0,hour*60+min,cal);
    
  /* determine julian day only ? */
  IF rel==relUTC && !offsStandard && !offsLocal && !zoneName THEN
    *julianDay = julDay(&du1);
    RETURN aLibOK
  END;

  /* step 1: resolve for dates as UTC and +/- a day */
  MemMove(&du2,&du1,sizeof(date));
  dateAddTime(&du1,-(24*60-1)); /* sets weekday */
  dateAddTime(&du2,+(24*60-1)); /* sets weekday */
  MemMove(&ds1,&du1,sizeof(date));
  MemMove(&dl1,&du1,sizeof(date));
  MemMove(&ds2,&du2,sizeof(date));
  MemMove(&dl2,&du2,sizeof(date));
  err = getTimezone(l.zone,&du1,&ds1,&dl1,&offsUS1,&offsSL1,zn1);
  IF err THEN RETURN err END;
  err = getTimezone(l.zone,&du2,&ds2,&dl2,&offsUS2,&offsSL2,zn2);
  IF err THEN RETURN err END;
  
  /* step 2: resolve again if necessary */
  /* set dates also when Julian Day is to be determined */
  IF offsUS1!=offsUS2 || offsSL1!=offsSL2 || StrCompare(zn1,zn2)
    || julianDay!=0 THEN
    
    dateSet(&du1,year,month,DAY_START_EQUAL,day,0,hour*60+min,cal);

    MemMove(&ds1,&du1,sizeof(date));
    MemMove(&dl1,&du1,sizeof(date));
    MemMove(&du2,&du1,sizeof(date));
    MemMove(&ds2,&du1,sizeof(date));
    MemMove(&dl2,&du1,sizeof(date));
    
    CASE rel OF
      case REL_UTC:
        dateAddTime(&ds1,offsUS1);
        dateAddTime(&dl1,offsUS1+offsSL1);
        dateAddTime(&ds2,offsUS2);
        dateAddTime(&dl2,offsUS1+offsSL2);
        break;
      case REL_STANDARD:
        dateAddTime(&du1,-offsUS1);
        dateAddTime(&dl1,offsSL1);
        dateAddTime(&du2,-offsUS2);
        dateAddTime(&dl2,offsSL2);
        break;
      default: /* REL_LOCAL */
        dateAddTime(&du1,-offsUS1-offsSL1);
        dateAddTime(&ds1,-offsSL1);
        dateAddTime(&du2,-offsUS2-offsSL2);
        dateAddTime(&ds2,-offsSL2);
    END
  END;

  IF offsUS1!=offsUS2 || offsSL1!=offsSL2 || StrCompare(zn1,zn2) THEN
    err = getTimezone(l.zone,&du1,&ds1,&dl1,&offsUS1n,&offsSL1n,zn1);
    IF err THEN RETURN err END;
    err = getTimezone(l.zone,&du2,&ds2,&dl2,&offsUS2n,&offsSL2n,zn2);
    IF err THEN RETURN err END;
    match1 = offsUS1==offsUS1n && offsSL1==offsSL1n;
    match2 = offsUS2==offsUS2n && offsSL2==offsSL2n;
    IF match2 && !match1 THEN
      offsUS1 = offsUS2;
      offsSL1 = offsSL2;
      StrCopy(zn1,zn2);
      MemMove(&du1,&du2,sizeof(date))
    END
  END;
  
  /* adjust local mean time if not a UNIX location */
  IF !unixLoc && !StrCompare(zn1,"LMT") THEN
    offsUS1 = (l2.lon+7)/15;
    offsSL1 = 0
  END;
  
  IF julianDay!=0 THEN *julianDay=julDay(&du1) END;
  IF offsStandard!=0 THEN *offsStandard=((double)offsUS1)/60 END;
  IF offsLocal!=0 THEN *offsLocal=((double)(offsUS1+offsSL1))/60 END;
  IF zoneName!=0 THEN StrCopy(zoneName,zn1) END;
   
  RETURN aLibOK
  
  #undef du1
  #undef ds1
  #undef dl1
  #undef du2
  #undef ds2
  #undef dl2 
END


FUNCTIONs double julDay(date* d)
BEGIN
  VAR Int16 month,year,b;
  month = d->month; year = d->year;
  IF d->month<=2 THEN month+= 12; year-- END;
  IF d->cal==calGregorian THEN b = (year/400) - (year/100) + (year/4)
  ELSE b = -2 + ((year+4716)/4) - 1179 /* calJulian */
  END;
  RETURN (2400000.5D - 679004.0D) + 365.0D*year + b
    + (Int16)(30.6001*(month+1)) + d->day + d->time/(24.0D*60)
END

PROCEDUREs setWeekday(date* d)
BEGIN
  double t;
  t = (julDay(d)-5.499999999L)/7;
  t = 7*(t-floor(t));
  d->weekday = t
END

FUNCTIONs Int16 daysInMonth(date* d)
BEGIN
  IF d->month==2 THEN
    IF d->cal==calJulian THEN RETURN 28+(d->year%4?0:1)
    ELSE RETURN 28+(d->year%4?0:1)-(d->year%100?0:1)+(d->year%400?0:1)
    END
  ELSIF d->month<=7 THEN RETURN 30+(d->month%2)
  ELSE RETURN 31-(d->month%2)
  END
END

FUNCTIONs Int16 firstBeforeOrEqual(date* d, Int16 weekday, Int16 day)
BEGIN
  VAR Int16 w;
  w = (d->weekday+(day-d->day)+70)%7; /* weekday of 'day' */
  RETURN day - (w-weekday+7)%7
END

FUNCTIONs Int16 firstAfterOrEqual(date* d, Int16 weekday, Int16 day)
BEGIN
  VAR Int16 w;
  w = (d->weekday+(day-d->day)+70)%7; /* weekday of 'day' */
  RETURN day + (weekday-w+7)%7
END

PROCEDUREs dateSet(date* d, Int16 year, Int16 month, Int16 dayType,
  Int16 day, Int16 weekday, Int16 time, aLibCal cal)
BEGIN
  d->year = year;
  d->month = month;
  d->cal = cal;
  IF dayType==DAY_START_EQUAL THEN
    d->day = day;
    setWeekday(d)
  ELSE
    d->day = 1;
    setWeekday(d);
    CASE dayType OF
      case DAY_START_LEQ:  d->day = firstBeforeOrEqual(d,weekday,day); break;
      case DAY_START_GEQ:  d->day = firstAfterOrEqual(d,weekday,day); break;
      case DAY_START_LAST: d->day = firstBeforeOrEqual(d,weekday,daysInMonth(d)); break;
    END;
    d->weekday = weekday
  END;
  d->time = time
END

PROCEDUREs dateAddTime(date* d, Int16 t)
BEGIN
  d->time += t;
  IF d->time>=24*60 THEN
    d->time-=24*60; d->day++;
    IF d->day>daysInMonth(d) THEN
      d->day = 1; d->month++;
      IF d->month>12 THEN d->month=1; d->year++ END
    END
  ELSIF d->time<0 THEN
    d->time+=24*60; d->day--;
    IF d->day<1 THEN
      d->month--;
      IF d->month<1 THEN d->month=12; d->year-- END;
      d->day = daysInMonth(d)
    END
  END;
  setWeekday(d)
END

FUNCTIONs Int16 dateCompare(date* d1, date* d2)
BEGIN
  IF d1->year!=d2->year THEN RETURN d1->year-d2->year END;
  IF d1->month!=d2->month THEN RETURN d1->month-d2->month END;
  IF d1->day!=d2->day THEN RETURN d1->day-d2->day END;
  RETURN d1->time-d2->time
END

PROCEDUREs getZoneName(Char* zn, Int16 offsName, Int16 offsReplace)
BEGIN
  VAR format f;
      Char a[10],b[10];
      Int16 i,j;
  IF tzFormat(offsName,&f) THEN StrCopy(a,f.name)
  ELSE StrCopy(a,"?")
  END;
  IF offsReplace==FORMAT_INDEX_N THEN b[0]=0
  ELSIF tzFormat(offsReplace,&f) THEN StrCopy(b,f.name)
  ELSE b[0]=0
  END;
  j=0;
  FOR i=0; i<=StrLen(a); i++ DO
    IF a[i]!='%' THEN zn[j]=a[i]; j++
    ELSE StrNCopy(&zn[j],b,StrLen(b)); j+=StrLen(b)
    END
  END
END

FUNCTIONs aLibErr getTimezone(Int16 zoneOffs, date* du, date* ds, date* dl,
  Int16* offsUS, Int16* offsSL, Char* zn)
BEGIN
  VAR zone z; rule r;
      Int16 i,j,y,saveIndex;
      date* d;
      date* dc;
      date* dr;
      Boolean foundZone;
      aLibData* data;
      
  data = getData();
  dc = &(data->d[6]);
  dr = &(data->d[7]);
  
  foundZone=FALSE;
  FOR i=0; tzZone(zoneOffs,i,&z) && !foundZone; i++ DO
    CASE z.untilTimeRel OF
      case REL_UTC: d = du; break;
      case REL_STANDARD: d = ds; break;
      default: /* REL_LOCAL */ d = dl
    END;
    dateSet(dc,z.untilYear,z.untilMonth,z.untilDayType,
      z.untilDay,z.untilWeekday,z.untilTime,d->cal);
    IF dateCompare(d,dc)<=0 THEN
      foundZone=TRUE;  
      *offsUS = z.offsStandard;
      
      IF z.offsType==OFFSET_DIRECT THEN
        *offsSL = z.offsLocal;
        saveIndex = FORMAT_INDEX_N
      ELSE
        /* offset by rule: find rule containing latest date <= given date */
 
        /* candidate for latest date in rules <= given date */
        /* will be the result if no rule is going to apply */
        dc->year = 0;
        *offsSL = 0;
        saveIndex = FORMAT_INDEX_N;
        /* search rules */
        FOR j=0; tzRule(z.offsRule,j,&r); j++ DO
          CASE r.startTimeRel OF
            case REL_UTC: d = du; break;
            case REL_STANDARD: d = ds; break;
            default: /* REL_LOCAL */ d = dl
          END;
          IF r.yearTo<d->year THEN
            /* rule applies with last year */
            /* check if later than current candidate */
            /* ignore whether local, standard or UTC */
            dateSet(dr,r.yearTo,r.startMonth,r.startDayType,
              r.startDay,r.startWeekday,r.startTime,d->cal);
            IF dateCompare(dr,dc)>=0 THEN
              MemMove(dc,dr,sizeof(date));
              *offsSL = r.saveTime;
              saveIndex = r.saveString
            END
          ELSIF d->year>=r.yearFrom && d->year<=r.yearTo
            && (r.yearParity==PARITY_ALL 
            || r.yearParity!=PARITY_ALL && (d->year-r.yearParity)%2==0) THEN
            /* check if rule applies with same year */
            dateSet(dr,d->year,r.startMonth,r.startDayType,
              r.startDay,r.startWeekday,r.startTime,d->cal);
            IF dateCompare(d,dr)>=0 && dateCompare(dr,dc)>=0 THEN
              MemMove(dc,dr,sizeof(date));
              *offsSL = r.saveTime;
              saveIndex = r.saveString
            ELSE
              /* check if rule applies with latest previous year */
              IF r.yearParity==PARITY_ALL THEN y=d->year-1
              ELSE y=d->year-2
              END;
              IF y>=r.yearFrom THEN
                dateSet(dr,y,r.startMonth,r.startDayType,
                  r.startDay,r.startWeekday,r.startTime,d->cal);
                IF dateCompare(d,dr)>=0 && dateCompare(dr,dc)>=0 THEN
                  MemMove(dc,dr,sizeof(date));
                  *offsSL = r.saveTime;
                  saveIndex = r.saveString
                END
              END
            END
          END
        END
      END;
      getZoneName(zn,z.format,saveIndex)
    END
  END;
  RETURN aLibOK
END

/************************/
/* events in space-time */
/************************/

/* to range 0..2pi */
FUNCTIONs double rev(double d);
PROCEDUREs sphe2rect(double* o);
PROCEDUREs rect2sphe(double* o);
PROCEDUREs ecl2equ(double* o, aLibEventStruct* p);
PROCEDUREs equ2ecl(double* o, aLibEventStruct* p);
PROCEDUREs geo2topo(double* o, aLibEventStruct* p);
PROCEDUREs topo2local(double* o, aLibEventStruct* p);

PROCEDUREs getLonLatR();
PROCEDUREs getSun(aLibEventStruct* p);
PROCEDUREs getMoon(aLibEventStruct* p);
PROCEDUREs getMercury(aLibEventStruct* p);
PROCEDUREs getVenus(aLibEventStruct* p);
PROCEDUREs getMars(aLibEventStruct* p);
PROCEDUREs getJupiter(aLibEventStruct* p);
PROCEDUREs getSaturn(aLibEventStruct* p);
PROCEDUREs getUranus(aLibEventStruct* p);
PROCEDUREs getNeptune(aLibEventStruct* p);
PROCEDUREs getPluto(aLibEventStruct* p);

FUNCTION aLibErr iLibEventNew (aLibEvent* pe, double julianDay,
  double lon, double lat)
BEGIN
  VAR aLibEventStruct* p;
  IF pe==0 THEN RETURN aLibErrIllegalParameters END;
  *pe = MemHandleNew(sizeof(aLibEventStruct));
  IF *pe==0 THEN RETURN aLibErrNoMem END;
  p = (aLibEventStruct*)MemHandleLock(*pe);
  IF p==0 THEN RETURN aLibErrNoMem END;
  p->d = julianDay - 2451543.5D;
  IF lon==0 THEN lon=1E-8 END; p->lon = lon;
  IF lat==0 THEN lat=1E-8 END; p->lat = lat;
  p->flags = 0;
  p->ecl = (23.4393D - 3.563e-7D * p->d)/(180/pi);
  getSun(p);
  MemHandleUnlock(*pe);
  RETURN aLibOK
END


PROCEDURE iLibEventFree (aLibEvent* pe)
BEGIN
  IF pe==0 THEN RETURN END;
  IF *pe!=0 THEN MemHandleFree(*pe); *pe=0 END
END


FUNCTION aLibErr iLibEventGetItem (aLibEvent e, aLibEventItemID id, 
  MemPtr item)
BEGIN
  VAR aLibEventStruct* p;
      aLibErr err;
      
  IF e==0 || item==0 THEN RETURN aLibErrIllegalParameters END;
  p = (aLibEventStruct*)MemHandleLock(e);
  IF p==0 THEN RETURN aLibErrNoMem END;
  err = aLibOK;
  CASE id OF
    case eventSid: *((double*)item) = p->sid; break;
    case eventEcl: *((double*)item) = p->ecl; break;
    default: err = aLibErrNotFound
  END;
  MemHandleUnlock(e);
  RETURN err
END


FUNCTION aLibErr iLibEventGetObjectItem (aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item)
BEGIN
  VAR aLibEventStruct* p;
      double* o;
      aLibErr err;
  IF e==0 || item==0 THEN RETURN aLibErrIllegalParameters END;
  IF objID<0 || objID>objPluto || id<0 || id>objCoordsAzimuthAltitude THEN
    RETURN aLibErrNotFound
  END;
  p = (aLibEventStruct*)MemHandleLock(e);
  IF p==0 THEN RETURN aLibErrNoMem END;
  
  CASE objID OF
    case objSun:      break;
    case objMoon:     getMoon(p); break;
    case objMercury:  getMercury(p); break;
    case objVenus:    getVenus(p); break;
    case objEarth:    break;
    case objMars:     getMars(p); break;
    case objJupiter:  getJupiter(p); break;
    case objSaturn:   getSaturn(p); break;
    case objUranus:   getUranus(p); break;
    case objNeptune:  getNeptune(p); break;
    case objPluto:    getPluto(p); break;
  END;
  
  o = (double*)item;
  MemMove(o,p->plaCoord[objID],3*sizeof(double));
  
  CASE objID OF
  
    case objSun: /* cached data is spherical geocentric ecliptic */
      CASE id OF
        case objCoordsSpheHelio:
        case objCoordsRectHelio:
          o[0]=o[1]=o[2] = 0;
          break;
        case objCoordsSpheGeoEcl:
        case objCoordsSpheTopoEcl:
        case objCoordsRectGeoEcl:
        case objCoordsRectGeoEqu:
        case objCoordsSpheGeoEqu:
        case objCoordsSpheTopoEqu:
          IF id==objCoordsSpheGeoEcl || id==objCoordsSpheTopoEcl THEN break END;
          sphe2rect(o); IF id==objCoordsRectGeoEcl THEN break END;
          ecl2equ(o,p); IF id==objCoordsRectGeoEqu THEN break END;
          rect2sphe(o);
          break; /* objCoordsSpheGeoEqu || objCoordsSpheTopoEqu */
      END;
      break;

    case objMoon: /* cached data is spherical geocentric ecliptic */
      CASE id OF
        case objCoordsSpheHelio:
        case objCoordsRectHelio:
          o[2] *= (1.371e6D/1.496e11D); /* radius to AU */
          sphe2rect(o);
          o[0]-= p->sunx;
          o[1]-= p->suny;
          IF id==objCoordsRectHelio THEN break END;
          rect2sphe(o);
          break; /* objCoordsSpheHelio */
        case objCoordsSpheGeoEcl:
        case objCoordsRectGeoEcl:
        case objCoordsRectGeoEqu:
        case objCoordsSpheGeoEqu:
        case objCoordsSpheTopoEqu:
        case objCoordsSpheTopoEcl:
          IF id==objCoordsSpheGeoEcl THEN break END;
          sphe2rect(o);  IF id==objCoordsRectGeoEcl THEN break END;
          ecl2equ(o,p);  IF id==objCoordsRectGeoEqu THEN break END;
          rect2sphe(o);  IF id==objCoordsSpheGeoEqu THEN break END;
          geo2topo(o,p); IF id==objCoordsSpheTopoEqu THEN break END;
          sphe2rect(o);
          equ2ecl(o,p);
          rect2sphe(o);
          break; /* objCoordsSpheTopoEcl */
      END;
      break;
      
    case objMercury:
    case objVenus:
    case objEarth:
    case objMars:
    case objJupiter:
    case objSaturn:
    case objUranus:
    case objNeptune:
    case objPluto:   /* cached data is spherical heliocentric ecliptic */
      CASE id OF
        case objCoordsSpheHelio:
        case objCoordsRectHelio:
          IF id==objCoordsSpheHelio THEN break END;
          sphe2rect(o);
          break;
        case objCoordsSpheGeoEcl:
        case objCoordsSpheTopoEcl:
        case objCoordsRectGeoEcl:
        case objCoordsSpheGeoEqu:
        case objCoordsSpheTopoEqu:
        case objCoordsRectGeoEqu:
          IF objID==objEarth THEN o[0]=o[1]=o[2]=0; break END;
          sphe2rect(o);
          o[0]+= p->sunx;
          o[1]+= p->suny;
          IF id==objCoordsRectGeoEcl THEN break END;
          IF id==objCoordsSpheGeoEcl || id==objCoordsSpheTopoEcl THEN
            rect2sphe(o);
            break;
          END;
          ecl2equ(o,p);
          IF id==objCoordsRectGeoEqu THEN break END;
          rect2sphe(o);
          break; /* objCoordsSpheGeoEqu || objCoordsSpheTopoEqu */
      END;
      break;
  END;
  
  IF id==objCoordsAzimuthAltitude THEN
    err = iLibEventGetObjectItem(e,objID,objCoordsSpheTopoEqu,item);
    IF err THEN MemHandleUnlock(e); RETURN err END;
    IF objID!=objEarth THEN topo2local(o,p) END;
  END;
  
  MemHandleUnlock(e);
  RETURN aLibOK
END


/* to range 0..2pi */
FUNCTIONs double rev(double d)
BEGIN
  RETURN d-floor(d/(2*pi))*(2*pi)
END

/* to range -pi..pi */
FUNCTIONs double rev2(double d)
BEGIN
  d-= floor(d/(2*pi))*(2*pi);
  RETURN d<=pi?d:d-2*pi
END

PROCEDUREs sphe2rect(double* o)
BEGIN
  VAR double r[3];
  r[0] = o[2] * cos(o[0]) * cos(o[1]);
  r[1] = o[2] * sin(o[0]) * cos(o[1]);
  r[2] = o[2] * sin(o[1]);
  MemMove(o,r,3*sizeof(double))
END

PROCEDUREs rect2sphe(double* o)
BEGIN
  VAR double s[3];
  s[0] = rev(atan2(o[1],o[0]));
  s[1] = rev2(atan2(o[2],sqrt(o[0]*o[0]+o[1]*o[1])));
  s[2] = sqrt(o[0]*o[0]+o[1]*o[1]+o[2]*o[2]);
  MemMove(o,s,3*sizeof(double))
END

PROCEDUREs ecl2equ(double* o, aLibEventStruct* p)
BEGIN
  VAR double t[3];
  t[0] = o[0];
  t[1] = o[1] * cos(p->ecl) - o[2] * sin(p->ecl);
  t[2] = o[1] * sin(p->ecl) + o[2] * cos(p->ecl);
  MemMove(o,t,3*sizeof(double))
END

PROCEDUREs equ2ecl(double* o, aLibEventStruct* p)
BEGIN
  VAR double t[3];
  t[0] = o[0];
  t[1] = o[1] * cos(p->ecl) + o[2] * sin(p->ecl);
  t[2] =-o[1] * sin(p->ecl) + o[2] * cos(p->ecl);
  MemMove(o,t,3*sizeof(double))
END

PROCEDUREs geo2topo(double* o, aLibEventStruct* p)
BEGIN
  VAR aLibData* data = getData();
      double* stack = (double*)&data->stack;

  #define gclat stack[0]
  #define rho   stack[1]
  #define ha    stack[2]
  #define mpar  stack[3]
  #define g     stack[4]

  gclat = p->lat - 0.1924D/(180/pi) * sin(2*p->lat);
  rho = 0.99883 + 0.00167 * cos (2*p->lat);
  ha = p->sid - o[0];
  mpar = asin(1/o[2]);
  g = atan(tan(gclat)/cos(ha));
  o[0] = rev(o[0] - mpar * rho * cos(gclat) * sin(ha) / cos(o[1]));
  o[1] = rev2(o[1] - mpar * rho * sin(gclat) * sin(g-o[1]) / sin(g));
  
  #undef gclat
  #undef rho
  #undef ha
  #undef mpar
  #undef g
END

PROCEDUREs topo2local(double* o, aLibEventStruct* p)
BEGIN
  VAR aLibData* data = getData();
      double* stack = (double*)&data->stack;

  #define ha   stack[0]
  #define x    stack[1]
  #define y    stack[2]
  #define z    stack[3]
  #define xhor stack[4]
  #define yhor stack[5]
  #define zhor stack[6]

  ha = p->sid - o[0];
  x = cos(ha)*cos(o[1]);
  y = sin(ha)*cos(o[1]);
  z = sin(o[1]);
  xhor = x*sin(p->lat) - z*cos(p->lat);
  yhor = y;
  zhor = x*cos(p->lat) + z*sin(p->lat);
  o[0] = rev(atan2(yhor,xhor) + pi);
  IF zhor>pi/4-0.02 THEN
    o[1] = rev2(atan2(zhor,sqrt(xhor*xhor+yhor*yhor)))
  ELSE o[1] = asin(zhor)
  END;
  o[2] = 0;
  
  #undef ha
  #undef x
  #undef y
  #undef z
  #undef xhor
  #undef yhor
  #undef zhor
END


#define llrM    pllr->M
#define llrN    pllr->N
#define llri    pllr->i
#define llrw    pllr->w
#define llra    pllr->a
#define llre    pllr->e
#define llrM    pllr->M
#define llrlon  pllr->lon
#define llrlat  pllr->lat
#define llrr    pllr->r


PROCEDUREs getSun(aLibEventStruct* p)
BEGIN
  VAR double* stack;
      
  #define e   stack[0]
  #define E   stack[1]
  #define xv  stack[2]
  #define yv  stack[3]
  #define v   stack[4]
  #define r   stack[5]
  #define t (&stack[6])
    
  IF !(p->flags & FLAG_sun) THEN
    stack = (double*)&getData()->stack;
    p->sunw = (282.9404D + 4.70935e-5D * p->d)/(180/pi);
    e = 0.016709D - 1.151e-9D * p->d;
    p->sunM = (356.0470D + 0.9856002585D * p->d)/(180/pi); 
    E = p->sunM + e*sin(p->sunM)*(1.0+e*cos(p->sunM));
    xv = cos(E)-e;
    yv = sqrt(1.0-e*e)*sin(E);
    v = atan2(yv,xv);
    r = sqrt(yv*yv+xv*xv);
    p->plaCoord[objSun][0] = rev(v+p->sunw);
    p->plaCoord[objSun][1] = 0;
    p->plaCoord[objSun][2] = r;
    p->plaCoord[objEarth][0] = rev(pi+v+p->sunw);
    p->plaCoord[objEarth][1] = 0;
    p->plaCoord[objEarth][2] = r;
    MemMove(t,p->plaCoord[objSun],3*sizeof(double));
    sphe2rect(t);
    p->sunx = t[0];
    p->suny = t[1];
    p->flags |= FLAG_sun;
    /* sideral time */
    p->sid = rev(p->sunw + p->sunM + pi + (p->d - floor(p->d))/(12/pi) + p->lon)
  END
  
  #undef e
  #undef E
  #undef xv
  #undef yv
  #undef v
  #undef r
  #undef t
END

PROCEDUREs getLonLatR()
BEGIN
  VAR aLibData* data = getData();
      lonLatR* pllr = &data->llr;
      double* stack = (double*)&data->stack;

  #define E   stack[0]
  #define E0  stack[1]
  #define xv  stack[2]
  #define yv  stack[3]
  #define v   stack[4]
  #define xh  stack[5]
  #define yh  stack[6]
  #define zh  stack[7]
  #define sN  stack[8]
  #define cN  stack[9]
  #define si  stack[10]
  #define ci  stack[11]
  #define svw stack[12]
  #define cvw stack[13]
  
  E = llrM + llre*sin(llrM)*(1.0+llre*cos(llrM));
  REPEAT
    E0 = E;
    E = E0 - (E0 - llre*sin(E0) - llrM) / (1-llre*cos(E0))
  UNTIL fabs(E-E0)<0.05 DONE;
  xv = llra * (cos(E)-llre);
  yv = llra * sqrt(1.0-llre*llre)*sin(E);
  v = atan2(yv,xv);
  llrr = sqrt(yv*yv+xv*xv);
  sincos(llrN,&sN,&cN);
  sincos(llri,&si,&ci);
  sincos(v+llrw,&svw,&cvw);
  xh = llrr * (cN*cvw-sN*svw*ci);
  yh = llrr * (sN*cvw+cN*svw*ci);
  zh = llrr * svw*si;
  
  llrlon = rev(atan2(yh,xh));
  llrlat = rev2(atan2(zh,sqrt(xh*xh+yh*yh)));

  #undef E
  #undef E0
  #undef xv
  #undef yv
  #undef v
  #undef xh
  #undef yh
  #undef zh
  #undef sN
  #undef cN
  #undef si
  #undef ci
  #undef svw
  #undef cvw
END


PROCEDUREs getMoon(aLibEventStruct* p)
BEGIN
  VAR double D,F,t;
      lonLatR* pllr;
    
  getSun(p);
  IF !(p->flags & FLAG_moo) THEN
    pllr = &getData()->llr;
    llrN =  rev((125.1228D - 0.0529538083D * p->d)/(180/pi));
    llri =  rev(5.1454D/(180/pi));
    llrw =  rev((318.0634D + 0.1643573223D * p->d)/(180/pi));
    llra = 60.2666D;
    llre = 0.054900D;
    llrM = rev((115.3654D + 13.0649929509D * p->d)/(180/pi));
    getLonLatR();
  
    D = llrM+llrw+llrN-(p->sunM+p->sunw);
    F = llrM+llrw;
  
    t  = -1.274 * sin(llrM-2*D);
    t += +0.658 * sin(2*D);
    t += -0.186 * sin(p->sunM);
    t += -0.059 * sin(2*llrM-2*D);
    t += -0.057 * sin(llrM-2*D+p->sunM);
    t += +0.053 * sin(llrM+2*D);
    t += +0.046 * sin(2*D-p->sunM);
    t += +0.041 * sin(llrM-p->sunM);
    t += -0.035 * sin(D);
    t += -0.031 * sin(llrM+p->sunM);
    t += -0.015 * sin(2*F-2*D);
    t += +0.011 * sin(llrM-4*D);
    p->plaCoord[objMoon][0] = rev(llrlon+t/(180/pi));
    
    t = -0.173 * sin(F-2*D);
    t+= -0.055 * sin(llrM-F-2*D);
    t+= -0.046 * sin(llrM+F-2*D);
    t+= +0.033 * sin(F+2*D);
    t+= +0.017 * sin(2*llrM+F);
    p->plaCoord[objMoon][1] = rev2(llrlat+t/(180/pi));
              
    p->plaCoord[objMoon][2] = llrr
         -0.58 * cos(llrM-2*D)
         -0.46 * cos(2*D);
    
    p->flags |= FLAG_moo
  END
END

PROCEDUREs getMercury(aLibEventStruct* p)
BEGIN
  VAR lonLatR* pllr;
  
  IF !(p->flags & FLAG_mer) THEN
    pllr = &getData()->llr;
    llrN = rev((48.3313D + 3.24587E-5D * p->d)/(180/pi));
    llri = rev((7.0047D + 5.00E-8 * p->d)/(180/pi));
    llrw = rev((29.1241D + 1.01444E-5D * p->d)/(180/pi));
    llra = 0.387098D;
    llre = 0.205635D + 5.59E-10 * p->d;
    llrM = rev((168.6562D + 4.0923344368D * p->d)/(180/pi));
    getLonLatR();
    p->plaCoord[objMercury][0] = llrlon;
    p->plaCoord[objMercury][1] = llrlat;
    p->plaCoord[objMercury][2] = llrr;
    p->flags |= FLAG_mer
  END
END

PROCEDUREs getVenus(aLibEventStruct* p)
BEGIN
  VAR lonLatR* pllr;
  
  IF !(p->flags & FLAG_ven) THEN
    pllr = &getData()->llr;
    llrN = rev((76.6799D + 2.46590E-5D * p->d)/(180/pi));
    llri = rev((3.3946D + 2.75E-8 * p->d)/(180/pi));
    llrw = rev((54.8910D + 1.38374E-5D * p->d)/(180/pi));
    llra = 0.723330D;
    llre = 0.006773D - 1.302E-9 * p->d;
    llrM = rev((48.0052D + 1.6021302244D * p->d)/(180/pi));
    getLonLatR();
    p->plaCoord[objVenus][0] = llrlon;
    p->plaCoord[objVenus][1] = llrlat;
    p->plaCoord[objVenus][2] = llrr;
    p->flags |= FLAG_ven
  END
END

PROCEDUREs getMars(aLibEventStruct* p)
BEGIN
  VAR lonLatR* pllr;
    
  IF !(p->flags & FLAG_mar) THEN
    pllr = &getData()->llr;
    llrN = rev((49.5574D + 2.11081E-5D * p->d)/(180/pi));
    llri = rev((1.8497D - 1.78E-8 * p->d)/(180/pi));
    llrw = rev((286.5016D + 2.92961E-5D * p->d)/(180/pi));
    llra = 1.523688D;
    llre = 0.093405D + 2.516E-9 * p->d;
    llrM = rev((18.6021D + 0.5240207766D * p->d)/(180/pi));
    getLonLatR();
    p->plaCoord[objMars][0] = llrlon;
    p->plaCoord[objMars][1] = llrlat;
    p->plaCoord[objMars][2] = llrr;
    p->flags |= FLAG_mar
  END
END

PROCEDUREs getJupiter(aLibEventStruct* p)
BEGIN
  VAR double Ms,t;
      lonLatR* pllr;
    
  IF !(p->flags & FLAG_jup) THEN
    pllr = &getData()->llr;
    llrN = rev((100.4542D + 2.76854E-5D * p->d)/(180/pi));
    llri = rev((1.3030D - 1.557E-7 * p->d)/(180/pi));
    llrw = rev((273.8777D + 1.64505E-5D * p->d)/(180/pi));
    llra = 5.20256D;
    llre = 0.048498D + 4.469E-9 * p->d;
    llrM = rev((19.8950D + 0.0830853001D * p->d)/(180/pi));
    getLonLatR();
      
    Ms = rev((316.9670D + 0.0334442282D * p->d)/(180/pi));
    
    t = -0.332 * sin(2*llrM - 5*Ms - 67.6/(180/pi));
    t+= -0.056 * sin(2*llrM - 2*Ms + 21.0/(180/pi));
    t+= +0.042 * sin(3*llrM - 5*Ms + 21.0/(180/pi));
    t+= -0.036 * sin(llrM - 2*Ms);
    t+= +0.022 * cos(llrM - Ms);
    t+= +0.023 * sin(2*llrM - 3*Ms + 52.0/(180/pi));
    t+= -0.016 * sin(llrM - 5*Ms - 69.0/(180/pi));
    p->plaCoord[objJupiter][0] = rev(llrlon+t/(180/pi));
        
    p->plaCoord[objJupiter][1] = llrlat;
    p->plaCoord[objJupiter][2] = llrr;
    
    p->flags |= FLAG_jup
  END
END

PROCEDUREs getSaturn(aLibEventStruct* p)
BEGIN
  VAR double Mj,t;
      lonLatR* pllr;
    
  IF !(p->flags & FLAG_sat) THEN
    pllr = &getData()->llr;
    llrN = rev((113.6634D + 2.38980E-5D * p->d)/(180/pi));
    llri = rev((2.4886D - 1.081E-7 * p->d)/(180/pi));
    llrw = rev((339.3939D + 2.97661E-5D * p->d)/(180/pi));
    llra = 9.55475D;
    llre = 0.055546D - 9.499E-9 * p->d;
    llrM = rev((316.9670D + 0.0334442282D * p->d)/(180/pi));
    getLonLatR();
      
    Mj = rev((19.8950D + 0.0830853001D * p->d)/(180/pi));
    
    t = +0.812 * sin(2*Mj - 5*llrM - 67.6/(180/pi));
    t+= -0.229 * cos(2*Mj - 4*llrM - 2.0/(180/pi));
    t+= +0.119 * sin(Mj - 2*llrM - 3.0/(180/pi));
    t+= +0.046 * sin(2*Mj - 6*llrM - 69.0/(180/pi));
    t+= +0.014 * sin(Mj - 3*llrM + 32.0/(180/pi));
    p->plaCoord[objSaturn][0] = rev(llrlon+t/(180/pi));
    
    t = -0.020 * cos(2*Mj - 4*llrM - 2.0/(180/pi));
    t+= +0.018 * sin(2*Mj - 6*llrM - 49.0/(180/pi));
    p->plaCoord[objSaturn][1] = rev2(llrlat+t/(180/pi));
        
    p->plaCoord[objSaturn][2] = llrr;
    
    p->flags |= FLAG_sat
  END
END

PROCEDUREs getUranus(aLibEventStruct* p)
BEGIN
  VAR double Mj,Ms,t;
      lonLatR* pllr;
    
  IF !(p->flags & FLAG_ura) THEN
    pllr = &getData()->llr;
    llrN = rev((74.0005D + 1.3978E-5D * p->d)/(180/pi));
    llri = rev((0.7733D + 1.9E-8 * p->d)/(180/pi));
    llrw = rev((96.6612D + 3.0565E-5D * p->d)/(180/pi));
    llra = 19.18171D - 1.55E-8 * p->d;
    llre = 0.047318D + 7.45E-9 * p->d;
    llrM = rev((142.5905D + 0.011725806D * p->d)/(180/pi));
    getLonLatR();
        
    Mj = rev((19.8950D + 0.0830853001D * p->d)/(180/pi));
    Ms = rev((316.9670D + 0.0334442282D * p->d)/(180/pi));
    
    t = +0.040 * sin(Ms - 2*llrM + 6.0/(180/pi));
    t+= +0.035 * sin(Ms - 3*llrM + 33.0/(180/pi));
    t+= -0.015 * sin(Mj - llrM + 20.0/(180/pi));
    p->plaCoord[objUranus][0] = rev(llrlon+t/(180/pi));

    p->plaCoord[objUranus][1] = llrlat;
    p->plaCoord[objUranus][2] = llrr; 
      
    p->flags |= FLAG_ura
  END
END

PROCEDUREs getNeptune(aLibEventStruct* p)
BEGIN
  VAR lonLatR* pllr;
  IF !(p->flags & FLAG_nep) THEN
    pllr = &getData()->llr;
    llrN = rev((131.7806D + 3.0173E-5D * p->d)/(180/pi)),
    llri = rev((1.7700D - 2.55E-7 * p->d)/(180/pi)),
    llrw = rev((272.8461D - 6.027E-6D * p->d)/(180/pi)),
    llra = 30.05826D + 3.313E-8 * p->d,
    llre = 0.008606D + 2.15E-9 * p->d,
    llrM = rev((260.2471D + 0.005995147D * p->d)/(180/pi)),
    getLonLatR();
    p->plaCoord[objNeptune][0] = llrlon;
    p->plaCoord[objNeptune][1] = llrlat;
    p->plaCoord[objNeptune][2] = llrr;
    p->flags |= FLAG_nep
  END
END

PROCEDUREs getPluto(aLibEventStruct* p)
BEGIN
  VAR double t;
      double* stack;
      
  #define S   stack[0]
  #define P   stack[1]
  #define sP  stack[2]
  #define cP  stack[3]
  #define s2P stack[4]
  #define c2P stack[5]
  #define s3P stack[6]
  #define c3P stack[7]
  #define s4P stack[8]
  #define c4P stack[9]
  #define s5P stack[10]
  #define c5P stack[11]
  #define s6P stack[12]
  #define c6P stack[13]
  #define sSP stack[14]
  #define cSP stack[15]
      
  IF !(p->flags & FLAG_plu) THEN
    stack = (double*)&getData()->stack;
  
    S = rev((50.03D + 0.033459652D * p->d)/(180/pi));
    P = rev((238.95D + 0.003968789D * p->d)/(180/pi));
    
    sincos(P,&sP,&cP);
    sincos(2*P,&s2P,&c2P);
    sincos(3*P,&s3P,&c3P);
    sincos(4*P,&s4P,&c4P);
    sincos(5*P,&s5P,&c5P);
    sincos(6*P,&s6P,&c6P);
    sincos(S-P,&sSP,&cSP);
    
    t = 238.9508D + 0.00400703D * p->d;
    t+= -19.799*sP + 19.848*cP;
    t+= +0.897*s2P - 4.956*c2P;
    t+= +0.610*s3P + 1.211*c3P;
    t+= -0.341*s4P - 0.190*c4P;
    t+= +0.128*s5P - 0.034*c5P;
    t+= -0.038*s6P + 0.031*c6P;
    t+= +0.020*sSP - 0.010*cSP;
    p->plaCoord[objPluto][0] = rev(t/(180/pi));
    
    t = -3.9082;
    t+= -5.453*sP  - 14.975*cP;
    t+= +3.527*s2P + 1.673*c2P;
    t+= -1.051*s3P + 0.328*c3P;
    t+= +0.179*s4P - 0.292*c4P;
    t+= +0.019*s5P + 0.100*c5P;
    t+= -0.031*s6P - 0.026*c6P;
    t+=            + 0.011*cSP;
    p->plaCoord[objPluto][1] = rev2(t/(180/pi));

    t = 40.72;
    t+= +6.68*sP  + 6.90*cP;
    t+= -1.18*s2P - 0.03*c2P;
    t+= +0.15*s3P - 0.14*c3P;
    p->plaCoord[objPluto][2] = t;
       
    p->flags |= FLAG_plu
  END
  
  #undef S
  #undef P
  #undef sP
  #undef cP
  #undef s2P
  #undef c2P
  #undef s3P
  #undef c3P
  #undef s4P
  #undef c4P
  #undef s5P
  #undef c5P
  #undef s6P
  #undef c6P
  #undef sSP
  #undef cSP
END
