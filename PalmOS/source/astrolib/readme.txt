astrolib 1.03

The astrolib library calculates planetary positions and time zones.
In its currenty form, it targets Palm OS (and was developed using
CodeWarrior on a Mac).

Positions of sun, moon and planets up to Pluto are calculated with
an accuracy of the order of one arc minute for dates less than a
century away from 2000, based on approximations by Paul Schlyter,
which are in turn based on a preprint of a paper by T. van Flandern
and K. Pulkkinen "Low precision formulae for planetary positions",
Astrophysical Journal Supplement Series, 1980.

Information is retrieved about locations and time zones on earth,
based on UNIX time zone data: http://www.twinsun.com/tz/tz-link.htm.
The data contains information for over 350 locations world-wide and
is even useful for dates centuries before UNIX machines existed.
The same information is provided for about 2000 additional locations
which have been assigned (by me) to UNIX time zone locations.

http://www.exactphilosophy.net/delphi.html
Copyright (C) 2000-05 by Alain Stalder.
(originally distributed under the pseudonym "delphi29@excite.com")

Distributed under the LGPL (see license.txt).

The first version of astrolib was distributed in October 2000,
this version (1.03) initially in May 2001.


1. How to use astrolib

Start using astrolib as follows:

  aLibErr err;
  Int16 version;
  version = aLibVersion;
  err = aLibNew(&version);
  // handle error

Some examples that retrieve information about places on earth:

  Int16 i;
  Char a[99];
  err = aLibMapGetItem(mapCountries,0,&i); // 237
  err = aLibMapGetItem(mapLocationName,100,a); // "Douala"
  err = aLibMapGetItem(mapLocationArea,100,&i); // 0
  err = aLibMapGetItem(mapAreaName,i,a); // "Africa"
  err = aLibMapGetItem(mapLocationCountry,100,&i); // 35
  err = aLibMapGetItem(mapCountryName,i,a); // "Cameroon"
  err = aLibMapGetItem(mapCountryCode,i,a); // "CM"
  err = aLibMapGetItem(mapLocationLon,100,&lon); // 0.16930 = 9e42
  err = aLibMapGetItem(mapLocationInfo,77,a); // "China mountains"

You can search areas, countries and locations by name:

  err = aLibMapFindByName(mapLocationName,"Z\xFCrich",&i); // 365
  err = aLibMapGetItem(mapLocationName,i,a); // "Zurich"

Here is how to determine time zones:

  err = aLibMapFindByName(mapLocationName,"Istanbul",&i);
  year=1978; month=10; day=15; hour=23; min=59;
  err = aLibTimezone (year,month,day,hour,min,0,calGregorian,relLocal,
          i,0,&offsStandard,&offsLocal,zoneName);
  // offsStandard=2, offsLocal=3, zoneName="EEST"

For astronomical calculations you need to specify time and place.
Determine the Julian Day for, say, 19 April 1990, 0:00 UTC:

  double jd;
  err = aLibTimezone(1990,4,19,0,0,0,calGregorian,relUTC,0,&jd,0,0,0);
  // jd = 2448000.5

Then create an event by indicating time and place:

  aLibEvent e;
  lon=15.0/(180/pi);
  lat=60.0/(180/pi);
  err = aLibEventNew(&e,jd,lon,lat);
  // handle error

Now you are ready to do some astronomical calculations:

  double o[3];

  // get obliquity of the ecliptic
  err = aLibEventGetItem(e,eventEcl,&ecl);
  // 0.40911 = 23.4406 deg

  // get geocentric ecliptical position of the Sun
  err = aLibEventGetObjectItem (e,objSun,objCoordsSpheGeoEcl,o);
  // lon = 0.50068 = 28.6870 deg, lat = 0, r = 1.004323

  // get topocentric equatorial position of the moon
  err = aLibEventGetObjectItem (e,objMoon,objCoordsSpheTopoEqu,o);
  // RA = 5.41055 = 310.0019 deg, Decl = -0.34696 = -19.8795 deg

  // get geocentric equatorial position of Mercury
  err = aLibEventGetObjectItem (e,objMercury,objCoordsSpheGeoEqu,o);
  // RA = 0.75503 = 43.2599 deg, Decl = 0.34289 = 19.6495 deg
  // r = 0.748296 (AU)

When you are done using astrolib, free any created events and
then astrolib:

  aLibEventFree(&e);
  aLibFree();
  
For a few more examples, see alibtest.c.


2. How to build astrolib

Only source files are distributed here, as it is unlikely that
anyone will want to build a PalmOS version at this time (2005).

This was the build process for PalmOS on a Mac:

First build tzgen, a normal console application, get UNIX time
zone data and let tzgen process it. Then integrate its output
tzdata.txt into astrolib, using ResEdit to paste resources ID
9000-9006 into astrolib's resources. Use alibtest for a basic
verfication of the correctness of astrolib's calculations.
