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

/* alibtest.c */

/* This file provides sample code of how to use astrolib and some
 * test data for verfying the correct operation of astrolib. The
 * astronomical test calls follow roughly the sequence in "Computing
 * planetary positions - a tutorial with worked examples"
 * by Paul Schlyter, see http://welcome.to/pausch. Only errors that
 * could crash the program are handled.
 *
 * There are two testing modes:
 * - External: Opens and accesses the library as one would normally.
 * - Internal: Directly calls the internal functions which are linked
 *             statically. This allows stepping into functions using
 *             the debugger.
 * If you want to use the internal testing mode, do a 
 * '#define ALIB_INTERNAL_TEST' for all astrolib related files.
 * In the project 'alibtest', for example, set it in the 
 * file 'myprefix.h'.
 */

#ifdef ALIB_INTERNAL_TEST
  #include "astrolibi.h"
  #define aLibMapGetItem         iLibMapGetItem
  #define aLibMapFindByName      iLibMapFindByName
  #define aLibTimezone           iLibTimezone
  #define aLibEventNew           iLibEventNew
  #define aLibEventFree          iLibEventFree
  #define aLibEventGetItem       iLibEventGetItem
  #define aLibEventGetObjectItem iLibEventGetObjectItem
#else
  #include <PalmOS.h>
  #include "astrolib.h"
  #define pi 3.1415926535897931160E+00D
#endif

#define rand(x) (SysRandom(0)%x)

static char a[99],b[99]; 
static aLibErr err;
static Int16 i,j,k,l;
static aLibEvent e;
static double offsStandard;
static double offsLocal;
static Char zoneName[10];
static Int16 year,month,day,hour,min;
static aLibCal cal;
static aLibRel rel;
static double lat,lon,jd;
static double sid,ecl;
static double o[3];
static aLibObjectID objID;
static aLibObjectItemID id;

aLibErr aLibTest()
{  
  #ifndef ALIB_INTERNAL_TEST
    Int16 version;
    /* try to open astrolib */
    version = aLibVersion;
    err = aLibNew(&version);
    if (err) return err;
  #else
    if (openMathLib()) return aLibErrCannotLoad;
  #endif
  
  /* retrieve areas, countries, locations */
  /* results are indicated for astrolib 1.03 (tzdata2000g) */
  err = aLibMapGetItem(mapAreas,0,&i); /* 10 */
  err = aLibMapGetItem(mapCountries,0,&i); /* 237 */
  err = aLibMapGetItem(mapLocations,0,&i); /* 2486 */
  err = aLibMapGetItem(mapLocationName,100,a); /* Douala */  
  err = aLibMapGetItem(mapLocationArea,100,&i); /* 0 */
  err = aLibMapGetItem(mapAreaName,i,a); /* Africa */
  err = aLibMapGetItem(mapLocationCountry,100,&i); /* 35 */
  err = aLibMapGetItem(mapCountryName,i,a); /* Cameroon */  
  err = aLibMapGetItem(mapCountryCode,i,a); /* CM */
  err = aLibMapGetItem(mapLocationLon,100,&lon); /* 0.16930 = 9e42 */
  err = aLibMapGetItem(mapLocationLat,100,&lat); /* 0.07069 = 4n03 */
  err = aLibMapGetItem(mapLocationInfo,77,a); /* China mountains */  
  
  /* find areas, countries and locations by name */
  err = aLibMapFindByName(mapAreaName,"ASIA",&i); /* 4 */
  err = aLibMapGetItem(mapAreaName,i,a); /* Asia */
  err = aLibMapFindByName(mapCountryName,"United States",&i); /* 221 */
  err = aLibMapGetItem(mapCountryName,i,a); /* United States */  
  err = aLibMapFindByName(mapCountryCode,"us",&i);  /* 221 */
  err = aLibMapGetItem(mapCountryCode,i,a); /* US */
  err = aLibMapFindByName(mapLocationName,"Z\xFCrich",&i); /* 365 */
  err = aLibMapGetItem(mapLocationName,i,a); /* Zurich */  
  err = aLibMapFindByName(mapLocationName,"aaaaa",&i); /* not found */
  err = aLibMapFindByName(mapLocationName,"zzzzz",&i); /* not found */
  
  /* new stuff since 1.03 */
  err = aLibMapGetItem(mapUnixLocations,0,&i); /* 366 */
  err = aLibMapFindByName(mapLocationName,"Geneve",&i); /* 1049 */
  err = aLibMapGetItem(mapLocationCountry,i,&j); /* 203 */
  err = aLibMapGetItem(mapCountryName,j,a); /* Switzerland */
  err = aLibMapGetItem(mapLocationUnixLoc,i,&i); /* 365 (Zurich) */
  err = aLibMapGetItem(mapCountryFirstLoc,j,&i); /* 365 */
  do
  {
    err = aLibMapGetItem(mapLocationNextLoc,i,&i);
  } while (!err);
  /* 597,629,822,1049,1370,1416,1422,2166 */
  err = aLibMapFindByName(mapLocationName,"Cartagena",&i); /* 797 */
  err = aLibMapGetItem(mapLocationCountry,i,&j); /* 191 */
  err = aLibMapGetItem(mapCountryName,j,a); /* Spain */
  /* (database also has a Cartagena in Colombia) */


  /* determine time zones, a few examples */

  /* example 1 */
  err = aLibMapFindByName(mapLocationName,"Istanbul",&i);
  year = 1978;
  month = 10;
  day = 15;
  hour = 23;
  min = 59;
  cal = calGregorian;
  rel = relLocal;
  err = aLibTimezone (year,month,day,hour,min,0,cal,rel,i,
    0,&offsStandard,&offsLocal,zoneName);
  /* offsStandard=2, offsLocal=3, zoneName="EEST" */
  
  /* example 2 */
  day = 16;
  hour = 0;
  min = 0;
  err = aLibTimezone (year,month,day,hour,min,0,cal,rel,i,
    0,&offsStandard,&offsLocal,zoneName);
  /* offsStandard=3, offsLocal=4, zoneName="TRST" */
  
  /* example 3 */
  err = aLibMapFindByName(mapLocationName,"Bangkok",&i);
  year = 2155;
  month = 5;
  day = 5;
  hour = 5;
  min = 55;
  cal = calGregorian;
  rel = relLocal;
  err = aLibTimezone (year,month,day,hour,min,0,cal,rel,i,
    0,&offsStandard,&offsLocal,zoneName);
  /* offsStandard=7, offsLocal=7, zoneName="ICT" */
  
  /* example 4 */
  err = aLibMapFindByName(mapLocationName,"Rome",&i);
  year = 800;
  month = 12;
  day = 25;
  hour = 12;
  min = 0;
  cal = calJulian;
  rel = relLocal;
  err = aLibTimezone (year,month,day,hour,min,0,cal,rel,i,
    0,&offsStandard,&offsLocal,zoneName);
  /* offsStandard=0.8333, offsLocal=0.8333, zoneName="LMT" (local mean time) */
  
  /* gremlins */
  err = aLibMapGetItem(mapLocations,0,&k);
  for (j=0; j<100; j++)
  {
    i = rand(k);
    err = aLibMapGetItem(mapLocationName,i,a);
    err = aLibMapGetItem(mapLocationArea,i,&l);
    err = aLibMapGetItem(mapAreaName,l,b);
    year = 2010-rand(50);
    if (rand(2)) year-=rand(50);
    if (rand(2)) year-=rand(50);
    if (rand(2)) year-=rand(50);
    month = 1+rand(12);
    day = 1+rand(28);
    hour = 1+rand(23);
    min = rand(60);
    cal = (aLibCal)rand(2);
    rel = (aLibRel)rand(3);
    err = aLibTimezone (year,month,day,hour,min,0,cal,rel,i,
      0,&offsStandard,&offsLocal,zoneName);
  }
  
      
  /* test date: April 19th, 1990 at 0:00 UTC in 15e00, 60n00 */
  /* all angles are returned in radians */
  
  /* determine Julian Day */
  err = aLibTimezone(1990,4,19,0,0,0,calGregorian,relUTC,0,&jd,0,0,0);
  /* jd = 2448000.5 */
  
  /* create event */
  lon = 15.0/(180/pi);
  lat = 60.0/(180/pi);
  err = aLibEventNew(&e,jd,lon,lat);
  if (err) return err;
  
  /* get obliquity of the ecliptic */
  err = aLibEventGetItem(e,eventEcl,&ecl); /* 0.40911 = 23.4406 deg */
  
  /* get geocentric ecliptical position of the Sun */
  err = aLibEventGetObjectItem (e,objSun,objCoordsSpheGeoEcl,o);
  /* lon = 0.50068 = 28.6870 deg */
  /* lat = 0 */
  /* r = 1.004323 */
  
  /* get geocentric equatorial position of the sun */
  err = aLibEventGetObjectItem (e,objSun,objCoordsSpheGeoEqu,o);
  /* RA = 0.46527 = 26.6581 deg */
  /* Decl = 0.19213 = 11.0084 deg */
  /* r = 1.004323 (unchanged) */
  
  /* get siderial time  */
  err = aLibEventGetItem(e,eventSid,&sid); /* 3.87182 = 14h 47m 21.3s */
  
  /* get altitude and azimuth, make sure buffer is 3 bytes */
  err = aLibEventGetObjectItem (e,objSun,objCoordsAzimuthAltitude,o);
  /* azimuth = 0.27361 = 15.67670 deg */
  /* altitude = -0.31340 = -17.95701 deg */
  
  /* get geocentric ecliptic position of the moon */
  err = aLibEventGetObjectItem (e,objMoon,objCoordsSpheGeoEcl,o);
  /* lon = 5.35726 = 306.9484 deg */
  /* lat = -0.01022 = -0.5856 deg */
  /* r = 60.6779 (in earth radii) */
  
  /* get geocentric equatorial position of the moon */
  err = aLibEventGetObjectItem (e,objMoon,objCoordsSpheGeoEqu,o);
  /* RA = 5.40181 = 309.5011 deg */
  /* Decl = -0.33341 = -19.1032 deg */
  /* r = 60.6779 (unchanged) */
  
  /* get topocentric equatorial position of the moon */
  err = aLibEventGetObjectItem (e,objMoon,objCoordsSpheTopoEqu,o);
  /* RA = 5.41055 = 310.0019 deg */
  /* Decl = -0.34696 = -19.8795 deg */
  
  /* get heliocentric positions of the planets */
  err = aLibEventGetObjectItem (e,objMercury,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objVenus,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objMars,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objJupiter,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objSaturn,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objUranus,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objNeptune,objCoordsSpheHelio,o);
  err = aLibEventGetObjectItem (e,objPluto,objCoordsSpheHelio,o);
  /* Name     lon                    lat                   r (AU)  
   * --------------------------------------------------------------
   * Mercury: 2.97702=170.5709 deg   0.10342= 5.9255 deg   0.374862
   * Venus:   4.60169=263.6570 deg  -0.00729=-0.4180 deg   0.726607
   * Mars:    5.07244=290.6296 deg  -0.02827=-1.6203 deg   1.417194
   * Jupiter: 1.83683=105.2423 deg   0.00194= 0.1113 deg   5.19508
   * Saturn:  5.05068=289.3825 deg   0.00322= 0.1845 deg  10.06118
   * Uranus:  4.83050=276.7671 deg  -0.00524=-0.3003 deg  19.39628
   * Neptune: 4.93438=282.7191 deg   0.01497= 0.8575 deg  30.19285
   * Pluto:   3.94906=226.2646 deg   0.27009= 5.4749 deg  29.73200
   */
  
  /* get geocentric equatorial position of Mercury */
  err = aLibEventGetObjectItem (e,objMercury,objCoordsSpheGeoEqu,o);
  /* RA = 0.75503 = 43.2599 deg */
  /* Decl = 0.34289 = 19.6495 deg */
  /* r = 0.748296 (AU) */
  
  /* get all object items for all objects */
  for (objID=objSun; objID<=objPluto; objID++)
    for (id=objCoordsSpheHelio; id<=objCoordsAzimuthAltitude; id++)
      err = aLibEventGetObjectItem (e,objID,id,o);
  
  /* free event */
  aLibEventFree(&e);
  
  #ifndef ALIB_INTERNAL_TEST
    /* close astrolib */
    aLibFree();
  #else
    closeMathLib();
  #endif
  
  return aLibOK;
}