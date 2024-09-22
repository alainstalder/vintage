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

/* timezone.c */
#include "astrolibi.h"

PROCEDUREs getInt(Char* a, Int32* ppos, Int16* px, Int16 len)
BEGIN
  VAR int i; 
  *px=0;
  FOR i=0; i<len; i++ DO
    *px |= (( a[(*ppos+i)>>3] >> ((*ppos+i)&0x07) ) & 1) << i
  END;
  *ppos+=len
END

PROCEDUREs getInt32(Char* a, Int32* ppos, Int32* px, Int16 len)
BEGIN
  VAR int i; 
  *px=0;
  FOR i=0; i<len; i++ DO
    *px |= (( ((Int32)a[(*ppos+i)>>3]) >> ((*ppos+i)&0x07) ) & 1) << i
  END;
  *ppos+=len
END

PROCEDUREs getChar(Char* a, Int32* ppos, Char* pc, Int16 nolower)
BEGIN
  VAR Int16 x;
  getInt(a,ppos,&x,nolower?5:6);
  IF 0<=x && x<=25 THEN *pc='A'+x
  ELSIF x==26 THEN *pc='-'
  ELSIF x==27 THEN *pc='_'
  ELSIF x==28 THEN *pc='%'
  ELSIF x==29 THEN *pc=' '
  ELSIF x==30 THEN *pc='/'
  ELSIF x==31 THEN *pc='\0'
  ELSIF 32<=x && x<=57 THEN *pc='a'+(x-32)
  ELSIF x==58 THEN *pc='('
  ELSIF x==59 THEN *pc=')'
  ELSIF x==60 THEN *pc='&'
  ELSIF x==61 THEN *pc='+'
  ELSIF x==62 THEN *pc=','
  ELSIF x==63 THEN
    getInt(a,ppos,&x,4);
    IF 0<=x && x<=9 THEN *pc='0'+x
    ELSIF x==10 THEN *pc='.'
    ELSIF x==11 THEN *pc='\''
    ELSE *pc='?' /* should not happen */
    END
  END
END

PROCEDUREs getString(Char* a, Int32* ppos, Char* ps, Int16 nolower)
BEGIN
  VAR Char c; Int16 i;
  i=0;
  REPEAT
    getChar(a,ppos,&c,nolower);
    ps[i]=c; i++
  UNTIL c=='\0' DONE
END


FUNCTION Int16 tzCountry(Int16 i, country* c)
BEGIN
  RETURN _tzCountry(i,c,TRUE)
END

FUNCTION Int16 _tzCountry(Int16 i, country* c, Boolean full)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,x;
  IF i<0 || i>=NCOUNTRY THEN RETURN 0 END;
  h = DmGetResource('aLib',COUNTRY_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=15*(Int32)i;
  getInt32(a,&pos,&x,15);
  pos=x;
  getInt(a,&pos,&c->firstLoc,9);
  IF full THEN
    getChar(a,&pos,&(c->code[0]),1 /*nolower*/);
    getChar(a,&pos,&(c->code[1]),1 /*nolower*/);
    c->code[2]='\0';
    getString(a,&pos,c->name,0 /*has lower*/)
  END;
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzArea(Int16 i, area* ar)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,x;
  IF i<0 || i>=NAREA THEN RETURN 0 END;
  h = DmGetResource('aLib',AREA_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=10*(Int32)i;
  getInt32(a,&pos,&x,10);
  pos=x;
  getString(a,&pos,ar->name,0 /*has lower*/);
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzLocation(Int16 i, location* l)
BEGIN
  RETURN _tzLocation(i,l,TRUE)
END

FUNCTION Int16 _tzLocation(Int16 i, location* l, Boolean full)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,x; Int16 sign;
  IF i<0 || i>=NLOCATION THEN RETURN 0 END;
  h = DmGetResource('aLib',LOCATION_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=17*(Int32)i;
  getInt32(a,&pos,&x,17);
  pos=x;
  getInt(a,&pos,&l->nextLoc,12);
  IF full THEN
    getInt(a,&pos,&l->country,9);
    getInt(a,&pos,&l->area,4);
    getInt(a,&pos,&l->zone,9);
    getInt(a,&pos,&sign,1);
    getInt(a,&pos,&l->lat,13);
    IF sign==1 THEN l->lat=-l->lat END;
    getInt(a,&pos,&sign,1);
    getInt(a,&pos,&l->lon,14);
    IF sign==1 THEN l->lon=-l->lon END;
    getString(a,&pos,l->name,0 /*has lower*/);
    getString(a,&pos,l->comment,0 /*has lower*/)
  END;
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzFormat(Int16 i, format* f)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,x;
  IF i<0 || i>=NFORMAT THEN RETURN 0 END;
  h = DmGetResource('aLib',FORMAT_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=14*(Int32)i;
  getInt32(a,&pos,&x,14);
  pos=x;
  getString(a,&pos,f->name,1 /*nolower*/);
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzRule(Int16 i, Int16 j, rule* r)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,tpos; Int16 len,flag,t;
  IF i<0 || i>=NRULE || j<0 THEN RETURN 0 END;
  h = DmGetResource('aLib',RULE_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=17*(Int32)i;
  getInt32(a,&pos,&tpos,17);
  getInt(a,&tpos,&len,7);
  IF j>=len THEN MemHandleUnlock(h); RETURN 0 END;
  pos=tpos;
  tpos+=13*(Int32)j;
  getInt(a,&tpos,&len,13);
  pos+=len;
  /* year from */
  getInt(a,&pos,&t,8);
  r->yearFrom=t==255?32000:1800+t;
  /* year to */
  getInt(a,&pos,&flag,1);
  IF flag THEN getInt(a,&pos,&t,1)
  ELSE
    getInt(a,&pos,&flag,1);
    IF flag THEN getInt(a,&pos,&t,4)
    ELSE getInt(a,&pos,&t,8)
    END
  END;
  r->yearTo=r->yearFrom+t;
  IF r->yearTo==2055 THEN r->yearTo=32000 END;
  /* year parity */
  getInt(a,&pos,&flag,1);
  IF flag THEN r->yearParity=PARITY_ALL
  ELSE
    getInt(a,&pos,&flag,1);
    IF flag THEN r->yearParity=PARITY_ODD
    ELSE r->yearParity=PARITY_EVEN
    END
  END;
  /* start month */
  getInt(a,&pos,&r->startMonth,4);
  /* start day type */
  getInt(a,&pos,&t,2);
  r->startDayType=t;
  /* start day */
  IF t!=DAY_START_LAST THEN getInt(a,&pos,&r->startDay,5) END;
  /* start weekday */
  IF t!=DAY_START_EQUAL THEN getInt(a,&pos,&r->startWeekday,3) END;
  /* start time */
  getInt(a,&pos,&r->startTime,11);
  /* start time relative to */
  getInt(a,&pos,&r->startTimeRel,2);
  /* time saved */
  getInt(a,&pos,&flag,1);
  IF flag THEN
    getInt(a,&pos,&t,1);
    r->saveTime=t*60
  ELSE getInt(a,&pos,&r->saveTime,11)
  END;
  /* save string */
  getInt(a,&pos,&flag,1);
  IF flag THEN r->saveString = FORMAT_INDEX_S /* "S" */
  ELSE
    getInt(a,&pos,&flag,1);
    IF flag THEN r->saveString = FORMAT_INDEX_N /* "-" */
    ELSE getInt(a,&pos,&r->saveString,9)
    END
  END;
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzZone(Int16 i, Int16 j, zone* z)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,tpos; Int16 len,flag,t,sign;
  IF i<0 || i>=NZONE || j<0 THEN RETURN 0 END;
  h = DmGetResource('aLib',ZONE_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=17*(Int32)i;
  getInt32(a,&pos,&tpos,17);
  getInt(a,&tpos,&len,4);
  IF j>=len THEN MemHandleUnlock(h); RETURN 0 END;
  pos=tpos;
  tpos+=10*(Int32)j;
  getInt(a,&tpos,&len,10);
  pos+=len;  
  /* offset to standard time */
  getInt(a,&pos,&sign,1);
  getInt(a,&pos,&flag,1);
  IF flag THEN
    getInt(a,&pos,&t,3);
    t=t*60
  ELSE getInt(a,&pos,&t,11)
  END;
  IF sign THEN t=-t END;
  z->offsStandard=t;
  /* offset to local time */
  getInt(a,&pos,&z->offsType,1);
  IF z->offsType THEN /* offset by rule */
    getInt(a,&pos,&z->offsRule,10)
  ELSE /* direct offset */
    getInt(a,&pos,&flag,1);
    IF flag THEN
      getInt(a,&pos,&t,1);
      z->offsLocal=t*60
    ELSE getInt(a,&pos,&z->offsLocal,11)
    END
  END;
  /* format */
  getInt(a,&pos,&z->format,10);
  /* until year */
  getInt(a,&pos,&t,8);
  z->untilYear=t==255?32000:1800+t;
  /* until month */
  getInt(a,&pos,&z->untilMonth,4);
  /* until day type */
  getInt(a,&pos,&t,2);
  z->untilDayType=t;
  /* until day */
  IF t!=DAY_START_LAST THEN getInt(a,&pos,&z->untilDay,5) END;
  /* until weekday */
  IF t!=DAY_START_EQUAL THEN getInt(a,&pos,&z->untilWeekday,3) END;
  /* until time */
  getInt(a,&pos,&z->untilTime,11);
  /* until time relative to */
  getInt(a,&pos,&z->untilTimeRel,2);
  MemHandleUnlock(h);
  RETURN 1
END

FUNCTION Int16 tzLocation2(Int16 i, location2* l2)
BEGIN
  RETURN _tzLocation2(i,l2,TRUE)
END

FUNCTION Int16 _tzLocation2(Int16 i, location2* l2, Boolean full)
BEGIN
  VAR MemHandle h; Char* a; Int32 pos,x; Int16 sign;
  IF i<0 || i>=NLOCATION2 THEN RETURN 0 END;
  h = DmGetResource('aLib',LOCATION2_RES_ID);
  IF !h THEN RETURN 0 END;
  a = MemHandleLock(h);
  pos=19*(Int32)i;
  getInt32(a,&pos,&x,19);
  pos=x;
  getInt(a,&pos,&l2->nextLoc,12);
  IF full THEN
    getInt(a,&pos,&l2->location,9);
    getInt(a,&pos,&sign,1);
    getInt(a,&pos,&l2->lat,13);
    IF sign==1 THEN l2->lat=-l2->lat END;
    getInt(a,&pos,&sign,1);
    getInt(a,&pos,&l2->lon,14);
    IF sign==1 THEN l2->lon=-l2->lon END;
    getString(a,&pos,l2->name,0 /*has lower*/)
  END;
  MemHandleUnlock(h);
  RETURN 1
END
