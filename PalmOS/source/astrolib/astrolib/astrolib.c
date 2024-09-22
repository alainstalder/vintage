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

/* astrolib.c */

#include <PalmOS.h>
#include "astrolib.h"


/**********/
/* modula */
/**********/

#ifndef MODULA_H
  #define BEGIN  {
  #define END    ; }
  #define IF     if (
  #define THEN   ) { 
  #define ELSIF  ; } else if (
  #define ELSE   ; } else {
  #define WHILE  while (
  #define DO     ) {
  #define REPEAT do {
  #define UNTIL  ; } while (!(
  #define DONE   )) ;
  #define FOR for (
  #define CASE switch (
  #define OF ) {
  #define PROCEDURE void
  #define FUNCTION
  #define RETURN return
  #define VAR
  #define TYPE typedef
  #define CONST
  #define TRUE true
  #define FALSE false
  #define IMPORT
  #define RECORD struct
  #define PROCEDUREs static void
  #define FUNCTIONs static
#endif

/****************/
/* system traps */
/****************/

FUNCTION Err _Open  (UInt16 ref, Int16* pversion)  SYS_TRAP(sysLibTrapOpen);
FUNCTION Err _Close (UInt16 ref, Int16* pusecount) SYS_TRAP(sysLibTrapClose);
FUNCTION Err _Sleep (UInt16 ref)                   SYS_TRAP(sysLibTrapSleep);
FUNCTION Err _Wake  (UInt16 ref)			             SYS_TRAP(sysLibTrapWake);

FUNCTION Err _MapGetItem (UInt16 ref, aLibMapItemID id, Int16 index, MemPtr item) 
  SYS_TRAP(sysLibTrapCustom+0);
FUNCTION Err _MapFindByName (UInt16 ref, aLibMapItemID id, Char* name, Int16* pindex) 
  SYS_TRAP(sysLibTrapCustom+1);

FUNCTION Err _Timezone (UInt16 ref, Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex, double* julianDay,
  double* offsStandard, double* offsLocal, Char* zoneName)
  SYS_TRAP(sysLibTrapCustom+2);

FUNCTION Err _EventNew  (UInt16 ref, aLibEvent* pe, double julianDay,
  double lon, double lat) SYS_TRAP(sysLibTrapCustom+3);
FUNCTION Err _EventFree (UInt16 ref, aLibEvent* pe) SYS_TRAP(sysLibTrapCustom+4);
FUNCTION Err _EventGetItem (UInt16 ref, aLibEvent e, aLibEventItemID id, 
  MemPtr item) SYS_TRAP(sysLibTrapCustom+5);
FUNCTION Err _EventGetObjectItem(UInt16 ref, aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item) SYS_TRAP(sysLibTrapCustom+6);


/********************/
/* astrolib general */
/********************/

/*
Inside a library, no global variables can be written.
In that case implement GetRef() and SetRef() outside this file.
See glue.c and MathLib.c for an example how astrolib itself
loads MathLib internally.
*/

#ifdef INSIDE_LIB
  FUNCTION  UInt16 GetRef();
  PROCEDURE SetRef(UInt16 ref);
#else
  VAR static UInt16 aLibRef;
  FUNCTIONs  UInt16 GetRef() BEGIN RETURN aLibRef END
  PROCEDUREs SetRef(UInt16 ref) BEGIN aLibRef=ref END
#endif

FUNCTION aLibErr aLibNew (Int16* pversion)
BEGIN
  VAR Err err; UInt16 ref;
  err=SysLibFind("astrolib",&ref);
  IF err THEN err=SysLibLoad('libr','aLib',&ref) END;
  IF err THEN *pversion=0; err=aLibErrCannotLoad END;
  SetRef(ref);
  IF !err THEN err=_Open(GetRef(),pversion) END;
  RETURN (aLibErr)err
END

PROCEDURE aLibFree()
BEGIN
  VAR Err err; Int16 usecount;
  err=_Close(GetRef(),&usecount);
  IF !err && usecount==0 THEN SysLibRemove(GetRef()) END
END


/**********************/
/* access to map data */
/**********************/

FUNCTION aLibErr aLibMapGetItem (aLibMapItemID id, Int16 index, MemPtr item)
BEGIN
  RETURN (aLibErr)_MapGetItem (GetRef(),id,index,item)
END

FUNCTION aLibErr aLibMapFindByName (aLibMapItemID id, Char* name, Int16* pindex)
BEGIN
  RETURN (aLibErr)_MapFindByName (GetRef(),id,name,pindex)
END


/***************************/
/* access to timezone data */
/***************************/

FUNCTION aLibErr aLibTimezone (Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex, double* julianDay,
  double* offsStandard, double* offsLocal, Char* zoneName)
BEGIN
  RETURN (aLibErr)_Timezone (GetRef(),year,month,day,hour,min,sec,cal,rel,
    locIndex,julianDay,offsStandard,offsLocal,zoneName)
END


/************************/
/* events in space-time */
/************************/

FUNCTION aLibErr aLibEventNew (aLibEvent* pe, double julianDay,
  double lon, double lat)
BEGIN
  RETURN (aLibErr)_EventNew (GetRef(),pe,julianDay,lon,lat)
END

PROCEDURE aLibEventFree (aLibEvent* pe)
BEGIN
  _EventFree(GetRef(),pe)
END

FUNCTION aLibErr aLibEventGetItem (aLibEvent e, aLibEventItemID id, 
  MemPtr item)
BEGIN
  RETURN (aLibErr)_EventGetItem (GetRef(),e,id,item)
END

FUNCTION aLibErr aLibEventGetObjectItem(aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item)
BEGIN
  RETURN (aLibErr)_EventGetObjectItem(GetRef(),e,objID,id,item)
END
