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

/* astrolib.h */

#ifndef _ASTROLIB_H
#define _ASTROLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define IN     /* input parameter */
#define OUT    /* output parameter */
#define INOUT  /* input and output parameter */


/************/
/* astrolib */
/************/

#define aLibVersion 103 /* major version: 1, minor version 03 */

typedef enum {
  aLibOK=0,                 /* no error */
  aLibErrNoMem,             /* not enough memory */
  aLibErrNotFound,          /* something was not found */
  aLibErrIllegalParameters, /* a parameter or combination was not allowed */
  aLibErrCannotLoad,        /* something could not be loaded */
  aLibErrIncompatible       /* available astrolib was incompatible */
} aLibErr;


aLibErr aLibNew (INOUT Int16* pversion);
/* Prepares astrolib for using, call before any other functions.
 * Usually you give your own version, and aLibNew returns the
 * version of the loaded astrolib.
 *
 * The call searches for the newest available astrolib library
 * with the same major version as indicated in the call. If only
 * astrolibs with older or newer major versions could be found,
 * the function returns with  aLibErrIncompatible and returns
 * the newest available version (or 0, if no astrolib could be
 * found or the version could not be obtained).
 *
 * If you modify astrolib, you should increase only the minor
 * version as long as the interface is forward compatible and no
 * new functions have been added. Typical examples, where only
 * the minor version should change:
 * - more precision for astronomical calculations
 * - more places on earth, corrected/updated timezone info
 * - additional IDs, for example for the calculation of additional
 *   objects in space, for new calendar systems, etc. - as long
 *   as existing IDs remain at the same numbers
 */

void aLibFree ();
/* Call when you are done using astrolib.
 * Depending on the operating system, this will do things like
 * unloading dynamic libraries, releasing allocated memory, etc.
 */


/**********************/
/* access to map data */
/**********************/

typedef enum {
  mapAreas=0,          /* Int16     number of areas in map */
  mapAreaName,         /* Char[99]  name of area */
  mapCountries,        /* Int16     number of countries in map */
  mapCountryName,      /* Char[99]  name of country */
  mapCountryCode,      /* Char[3]   ISO two character country code */
  mapLocations,        /* Int16     number of locations in map */
  mapLocationName,     /* Char[99]  name of location */
  mapLocationArea,     /* Int16     index in areas */
  mapLocationCountry,  /* Int16     index in countries */
  mapLocationLon,      /* double    longitude of location (e=+) */
  mapLocationLat,      /* double    latitude of location (n=+) */
  mapLocationInfo,     /* Char[99]  info about locations in same timezone */
  mapLocationUnixLoc,  /* Int16     index in locations of UNIX time zone loc */
  mapUnixLocations,    /* Int16     number of UNIX time zone locations */
  mapCountryFirstLoc,  /* Int16     index of first location in country */
  mapLocationNextLoc   /* Int16     index of next location in same country */
} aLibMapItemID;

aLibErr aLibMapGetItem (IN aLibMapItemID id, IN Int16 index, OUT MemPtr item);
/* Indicate the ID of the item you want to retrieve, pass the
 * index in areas, countries or locations and pass a pointer
 * to the corresponding return data type. If you retrieve the
 * number of areas, countries or locations, the index parameter
 * is ignored.
 */

aLibErr aLibMapFindByName (IN aLibMapItemID id, IN Char* name,
  OUT Int16* pindex);
/* Indicate the ID of the name by which you want to search. Currently,
 * this can be the name of an area, country or location or an ISO
 * country code. If found, the function will return the index in
 * areas, countries or locations. The search is case insensitive.
 */


/***************************/
/* access to timezone data */
/***************************/

typedef enum {
  calGregorian=0, /* today's Gregorian Calendar */
  calJulian       /* Julian Calendar */
} aLibCal;

typedef enum {
  relUTC=0,    /* relative to 0 deg latitude (Greenwich) */
  relStandard, /* relative to local time execept daylight savings */
  relLocal     /* relative to local time (standard+daylight savings) */
} aLibRel;

aLibErr aLibTimezone (IN Int16 year, IN Int16 month, IN Int16 day, IN Int16 hour,
  IN Int16 min, IN Int16 sec, IN aLibCal cal, IN aLibRel rel, IN Int16 locIndex,
  OUT double* julianDay, OUT double* offsStandard, OUT double* offsLocal,
  OUT Char* zoneName);
/* Converts date and time given by year to sec, calendar and
 * relative to either UTC, standard time or local time (at the
 * location indicated by locIndex) to:
 *
 * - Julian Day (relative to UTC)
 * - offset from standard time to UTC (hours)
 * - offset from local time to UTC (hours)
 * - the name of the timezone, e.g. "EDT" (Char[10])
 *
 * Pass 0 for output parameters you are not interested in.
 *
 * If you just want to convert date and time items to
 * Julian Day, indicate relUTC and pass 0 for locIndex
 * and for all output parameters except the Julian Day.
 */
 

/************************/
/* events in space-time */
/************************/

typedef MemHandle aLibEvent;
/* Opaque data type as object handle */

aLibErr aLibEventNew (OUT aLibEvent* pe, IN double julianDay,
  IN double lon, IN double lat);
/* Creates an event object and sets the handle. The Julian Day is
 * indicated relative to UTC, lon and lat are indicated in radians.
 */

void aLibEventFree (INOUT aLibEvent* pe);
/* Releases the event object and sets the handle to 0. */


typedef enum {
  eventSid,            /* double  local sideral time (radians) */
  eventEcl             /* double  obliquity of the ecliptic (radians) */
} aLibEventItemID;

aLibErr aLibEventGetItem (IN aLibEvent e, IN aLibEventItemID id,
  OUT MemPtr item);
/* Indicate the ID of the item you want to obtain and pass a pointer
 * to the corresponding data type for the result.
 */


typedef enum {
  objSun=0, objMoon, objMercury, objVenus, objEarth, objMars,
  objJupiter, objSaturn, objUranus, objNeptune, objPluto
} aLibObjectID; /* objects in space */

typedef enum {
  objCoordsSpheHelio=0,    /* double[3]  lon,lat,r */
  objCoordsRectHelio,      /* double[3]  x,y,z */
  objCoordsSpheGeoEcl,     /* double[3]  lon,lat,r */
  objCoordsRectGeoEcl,     /* double[3]  x,y,z */
  objCoordsSpheGeoEqu,     /* double[3]  RA,decl,r */
  objCoordsRectGeoEqu,     /* double[3]  x,y,z */
  objCoordsSpheTopoEcl,    /* double[3]  lon,lat,? */
  objCoordsSpheTopoEqu,    /* double[3]  RA,decl,? */
  objCoordsAzimuthAltitude /* double[3]  azimuth,altitude,? */
} aLibObjectItemID; /* attributes of objects in space */
/* coordinate systems:
 * - Rect/Sphe: rectangular/spherical
 * - Helio/Geo/Topo: heliocentric/geocentric/topocentric
 *   (relative to center of sun or earth or location on earth)
 * - Ecl/Equ: ecliptic/equatorial
 * Angles are returned in radians. Radii are in AUs, execpt for
 * the moon in geozentric coordinates where it is in earth radii.
 */

aLibErr aLibEventGetObjectItem (IN aLibEvent e, IN aLibObjectID objID,
  IN aLibObjectItemID id, OUT MemPtr item);
/* Indicate the ID of the object and of the item you want to
 * calculate/retrieve and pass a pointer to the corresponding
 * data type for the result.
 */


#ifdef __cplusplus
}
#endif

#endif