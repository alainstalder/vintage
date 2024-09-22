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

/* glue.c */

#include "astrolibi.h"

/******************/
/* glue functions */
/******************/

FUNCTION Err __Open  (UInt16 ref, Int16* pversion);
FUNCTION Err __Close (UInt16 ref, Int16* pusecount);
FUNCTION Err __Sleep (UInt16 ref);
FUNCTION Err __Wake  (UInt16 ref);

FUNCTION Err __MapGetItem (UInt16 ref, aLibMapItemID id, Int16 index, MemPtr item);
FUNCTION Err __MapFindByName (UInt16 ref, aLibMapItemID id, Char* name, Int16* index);

FUNCTION Err __Timezone (UInt16 ref, Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex, double* julianDay,
  double* offsStandard, double* offsLocal, Char* zoneName);

FUNCTION Err __EventNew (UInt16 ref, aLibEvent* pe, double julianDay, double lon,
  double lat);
FUNCTION Err __EventFree (UInt16 ref, aLibEvent* pe);
FUNCTION Err __EventGetItem (UInt16 ref, aLibEvent e, aLibEventItemID id,
  MemPtr item);
FUNCTION Err __EventGetObjectItem(UInt16 ref, aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item);
  

/******************************/
/* startup and dispatch table */
/******************************/

#define NFUNC 			   11                  /* number of functions */
#define JUMP_OFFS(x)   ((1+NFUNC)*2+x*4)   /* offset to jump */
#define NAME_OFFS      (JUMP_OFFS(NFUNC))  /* offset to name */

FUNCTION MemPtr *DispatchTable();

/* must be the first function in the code */
#ifndef ALIB_INTERNAL_TEST
  FUNCTION Err __Startup__(UInt16 ref, SysLibTblEntryPtr entry)
  BEGIN
  #pragma unused(ref)
    entry->dispatchTblP = DispatchTable();
    entry->globalsP = 0; /* allocated in __Open() */
    RETURN 0
  END
#endif

FUNCTION asm MemPtr *DispatchTable()
BEGIN
  lea   table, a0
  rts
  
table:
  dc.w  NAME_OFFS
  dc.w  JUMP_OFFS(0)
  dc.w  JUMP_OFFS(1)
  dc.w  JUMP_OFFS(2)
  dc.w  JUMP_OFFS(3)
  dc.w  JUMP_OFFS(4)
  dc.w  JUMP_OFFS(5)
  dc.w  JUMP_OFFS(6)
  dc.w  JUMP_OFFS(7)
  dc.w  JUMP_OFFS(8)
  dc.w  JUMP_OFFS(9)
  dc.w  JUMP_OFFS(10)
  jmp   __Open
  jmp   __Close
  jmp   __Sleep
  jmp   __Wake
  jmp   __MapGetItem
  jmp   __MapFindByName
  jmp   __Timezone
  jmp   __EventNew
  jmp   __EventFree
  jmp   __EventGetItem
  jmp   __EventGetObjectItem
  dc.b  "astrolib"
END


/*************************/
/* access to A4 register */
/*************************/

long setA4(long:__D0):__D0 = 0xC18C;
long getA4(void) = 0x200C;

#define ENTER long oldA4 = enter(ref)
#define LEAVE MemPtrUnlock((MemPtr)setA4(oldA4))

FUNCTIONs long enter(UInt16 ref)
BEGIN
  VAR SysLibTblEntryPtr p;
  p = SysLibTblEntry(ref);
  ErrFatalDisplayIf(!p,"astrolib: sorry, no library entry");
  ErrFatalDisplayIf(!p->globalsP,"astrolib: sorry, no globals");
  RETURN setA4((long)MemHandleLock(p->globalsP))
END


/***************/
/* use MathLib */
/***************/

#ifndef ALIB_INTERNAL_TEST
  #define MATHLIB_REF ((aLibData*)getA4())->mathLibRef
#else
  static UInt16 iMathLibRef; 
  #define MATHLIB_REF iMathLibRef
#endif ALIB_INTERNAL_TEST

FUNCTION UInt16 getMathLibRef() /* for use in MathLib.c */
BEGIN
  RETURN MATHLIB_REF
END

FUNCTION Err openMathLib()
BEGIN
  VAR Err err;
      UInt16 ref;
  err=SysLibFind(MathLibName,&ref);
  IF err THEN err=SysLibLoad('libr',MathLibCreator,&ref) END;
  MATHLIB_REF = ref;
  IF !err THEN err=MathLibOpen(ref,MathLibVersion) END;
  RETURN err
END

PROCEDURE closeMathLib()
BEGIN
  VAR Err err;
      UInt16 usecount;
  err=MathLibClose(MATHLIB_REF,&usecount);
  IF !err && usecount==0 THEN SysLibRemove(MATHLIB_REF) END
END


/*************************/
/* access to global data */
/*************************/

#ifndef ALIB_INTERNAL_TEST
  #define ALIB_DATA (aLibData*)getA4()
#else
  static aLibData iData;
  #define ALIB_DATA &iData
#endif

FUNCTION aLibData* getData()
BEGIN
  RETURN ALIB_DATA
END


/******************/
/* standard calls */
/******************/

FUNCTION Err __Open (UInt16 ref, Int16* pversion)
BEGIN
  long oldA4;
  MemHandle* h;
  aLibData* data;
  Err err;
  Boolean ok;
  
  ok = *pversion/100==aLibVersion/100;
  *pversion = aLibVersion;
  IF !ok THEN RETURN aLibErrIncompatible END;
	h = &SysLibTblEntry(ref)->globalsP;
	IF *h THEN
	  data = MemHandleLock(*h);
	  data->usecount++
	ELSE
	  *h = MemHandleNew(sizeof(aLibData));
	  IF !*h THEN RETURN aLibErrNoMem END;
	  /* set owner to OS, else the handle would be deallocated  */
	  /* by the OS when the application that allocated it exits */
	  MemHandleSetOwner(*h,0);
	  data = MemHandleLock(*h);
	  data->usecount=1
	END;
	data->db = DmOpenDatabaseByTypeCreator('libr','aLib',dmModeReadOnly);
  MemHandleUnlock(*h);
  oldA4 = enter(ref);
	err = openMathLib();
	IF err THEN closeMathLib(); LEAVE; RETURN aLibErrCannotLoad END;
  LEAVE; RETURN aLibOK
END

FUNCTION Err __Close (UInt16 ref, Int16* pusecount)
BEGIN
  ENTER;
  MemHandle* h;
  aLibData* data;
  
  closeMathLib();
	h = &SysLibTblEntry(ref)->globalsP;
  data = MemHandleLock(*h);
  data->usecount--;
  *pusecount = data->usecount;
  DmCloseDatabase(data->db);
  MemHandleUnlock(*h);
  LEAVE;
  IF !*pusecount THEN MemHandleFree(*h) END;
  RETURN aLibOK
END

FUNCTION Err __Sleep (UInt16 ref) BEGIN RETURN aLibOK END
FUNCTION Err __Wake  (UInt16 ref) BEGIN RETURN aLibOK END


/**********************/
/* access to map data */
/**********************/

FUNCTION Err __MapGetItem (UInt16 ref, aLibMapItemID id, Int16 index, MemPtr item)
BEGIN
  ENTER;
  Err err = iLibMapGetItem (id,index,item);
  LEAVE; RETURN err
END

FUNCTION Err __MapFindByName (UInt16 ref, aLibMapItemID id, Char* name, Int16* pindex)
BEGIN
  ENTER;
  Err err = iLibMapFindByName (id,name,pindex);
  LEAVE; RETURN err
END


/***************************/
/* access to timezone data */
/***************************/

FUNCTION Err __Timezone (UInt16 ref, Int16 year, Int16 month, Int16 day, Int16 hour,
  Int16 min, Int16 sec, aLibCal cal, aLibRel rel, Int16 locIndex,
  double* julianDay, double* offsStandard, double* offsLocal, Char* zoneName)
BEGIN
  ENTER;
  Err err =  iLibTimezone (year,month,day,hour,min,sec,cal,rel,locIndex,
    julianDay,offsStandard,offsLocal,zoneName);
  LEAVE; RETURN err
END


/************************/
/* events in space-time */
/************************/

FUNCTION Err __EventNew (UInt16 ref, aLibEvent* pe, double julianDay, double lon,
  double lat)
BEGIN
  ENTER;
  Err err = iLibEventNew (pe,julianDay,lon,lat);
  LEAVE; RETURN err
END

FUNCTION Err __EventFree (UInt16 ref, aLibEvent* pe)
BEGIN
  ENTER;
  iLibEventFree (pe);
  LEAVE; RETURN aLibOK
END

FUNCTION Err __EventGetItem (UInt16 ref, aLibEvent e, aLibEventItemID id, 
  MemPtr item)
BEGIN
  ENTER;
  Err err = iLibEventGetItem (e,id,item);
  LEAVE; RETURN err
END

FUNCTION Err __EventGetObjectItem(UInt16 ref, aLibEvent e, aLibObjectID objID,
  aLibObjectItemID id, MemPtr item)
BEGIN
  ENTER;
  Err err = iLibEventGetObjectItem(e,objID,id,item);
  LEAVE; RETURN err
END

