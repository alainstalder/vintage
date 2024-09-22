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

/* tzrule.c */

#include "tzgen.h"

VAR RECORD ruleType rule[RULE_MAX];
    int nRule=0;

PROCEDURE ruleAddLine(char* line)
BEGIN
  VAR RECORD ruleType* rp;
      char a[256];
      int i,j,dummy;
  
  /* check if really a rule */
  utilScanInit(line);
  utilScanWord(a);
  IF strcmp(a,"Rule") THEN RETURN END;
  
  /* set data fields */
  /* name, search if new rule */
  utilScanWord(a);
  FOR i=0; i<nRule && strcmp(a,rule[i].name); i++ DO END;
  rp = &rule[i];
  IF i==nRule THEN
    IF nRule>=RULE_MAX THEN
      printf("\nCould not add rule, increase value of RULE_MAX...");
      RETURN
    END;
    /* new rule */
    rp->used = 0; /* we don't know yet */
    strcpy(rp->name,a);
    rp->nRuleItem=0;
    nRule++
  END;
  j = rp->nRuleItem; /* shorter */
  IF j>=RULE_ITEM_MAX THEN
    printf("\nCould not add rule item, increase value of RULE_ITEM_MAX...");
    RETURN
  END;
  /* year from */
  utilScanWord(a);
  rp->yearFrom[j] = atoi(a);
  /* year to */
  utilScanWord(a);
  IF !strcmp(a,"only") THEN rp->yearTo[j] = rp->yearFrom[j]
  ELSIF !strcmp(a,"max") THEN rp->yearTo[j] = YEAR_MAX
  ELSE rp->yearTo[j] = atoi(a)
  END;
  /* year parity */
  utilScanWord(a);
  IF !strcmp(a,"-") THEN rp->yearParity[j] = PARITY_ALL
  ELSIF !strcmp(a,"odd") THEN rp->yearParity[j] = PARITY_ODD
  ELSIF !strcmp(a,"even") THEN rp->yearParity[j] = PARITY_EVEN
  ELSE
    printf("\nCould not add rule: unknown year parity %s...",line);
    RETURN
  END;
  /* start month */
  utilScanWord(a);
  IF !utilGetMonth(a,&rp->startMonth[j]) THEN
    printf("\nCould not add rule: unknown month %s...",line)
  END;
  /* start day type, day, weekday */
  utilScanWord(a);
  IF !utilGetDay(a,&rp->startDayType[j],&rp->startDay[j],&rp->startWeekday[j]) THEN
    printf("\nCould not add rule: unknown day rule %s...",line)
  END;
  /* start time incl. relative to what */
  utilScanWord(a);
  IF !utilGetTime(a,&rp->startTime[j],&rp->startTimeRel[j]) THEN
    printf("\nCould not add rule: unknown start time %s...",line)
  END;
  /* save time */
  utilScanWord(a);
  IF !utilGetTime(a,&rp->saveTime[j],&dummy) THEN
    printf("\nCould not add rule: unknown save time %s...",a)
  END;
  /* save time string */
  utilScanWord(a);
  FOR i=0; i<nFormat && strcmp(a,format[i].name); i++ DO END;
  IF i==nFormat THEN
    printf("\nCould not add rule: save string not found %s...",line);
    RETURN
  END;
  rp->saveString[j]=i;
  rp->nRuleItem++
END

PROCEDURE rulePrint()
BEGIN
  VAR int i,j;
  
  printf("\nRULES:\nindex1  used                         "
    "month    day weekday\n  index2   name        from to   "
    "parity  dayType     time rel save letter");
  FOR i=0; i<nRule /*&& i<70*/; i++ DO
    FOR j=0; j<rule[i].nRuleItem; j++ DO
      printf("\n%-3d %-3d %-1d  %-11s %-4d %-4d %-1d   %-2d  "
      "%-1d    %-2d  %-1d  %-4d %-1d   %-4d %-3d",i,j,rule[i].used,rule[i].name,
      rule[i].yearFrom[j],rule[i].yearTo[j],rule[i].yearParity[j],rule[i].startMonth[j],
      rule[i].startDayType[j],rule[i].startDay[j],rule[i].startWeekday[j],
      rule[i].startTime[j],rule[i].startTimeRel[j],rule[i].saveTime[j],
      rule[i].saveString[j])
    END
  END;
  printf("\nEND RULES\n")
END

FUNCTION int ruleItemNum()
BEGIN
  VAR int i,n=0;
  FOR i=0; i<nRule; i++ DO n+=rule[i].nRuleItem END;
  RETURN n
END

FUNCTION int ruleItemMax()
BEGIN
  VAR int i,n=0;
  FOR i=0; i<nRule; i++ DO
    IF rule[i].nRuleItem > n THEN n = rule[i].nRuleItem END
  END;
  RETURN n
END

PROCEDURE ruleSerialize(char* buf, int* plen)
BEGIN
  VAR int i,j,x,y,d,max,len,leni;
      int bitpos,bitpos1,bitposi,bitposi1;
  len=17;
  bitpos1=0;
  bitpos=len*nRule;
  max=0;
  FOR i=0; i<nRule; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddInt(buf,&bitpos,rule[i].nRuleItem,7);
    leni=13;
    bitposi=bitpos;
    bitposi1=bitpos;
    bitpos+=leni*rule[i].nRuleItem;
    FOR j=0; j<rule[i].nRuleItem; j++ DO
      utilAddInt(buf,&bitposi1,bitpos-bitposi,leni);
      /* year from */
      x = rule[i].yearFrom[j];
      x = x==YEAR_MAX?255:x-1800;
      utilAddInt(buf, &bitpos, x, 8);
      /* year to (relative to year from) */
      y = rule[i].yearTo[j];
      y = y==YEAR_MAX?255:y-1800;
      y = y-x;
      IF y<2 THEN
        utilAddFlag(buf, &bitpos, 1);
        utilAddFlag(buf, &bitpos, y)
      ELSIF y<16 THEN
        utilAddFlag(buf, &bitpos, 0);
        utilAddFlag(buf, &bitpos, 1);
        utilAddInt(buf, &bitpos, y, 4)
      ELSE 
        utilAddFlag(buf, &bitpos, 0);
        utilAddFlag(buf, &bitpos, 0);
        utilAddInt(buf, &bitpos, y, 8)
      END;
      /* year parity */
      x = rule[i].yearParity[j];
      IF x==PARITY_ALL THEN
        utilAddFlag(buf, &bitpos, 1)
      ELSE
        utilAddFlag(buf, &bitpos, 0);
        utilAddFlag(buf, &bitpos, x==PARITY_ODD)
      END;
      /* start month */
      utilAddInt(buf, &bitpos, rule[i].startMonth[j], 4);
      /* start day type */
      x = rule[i].startDayType[j];
      utilAddInt(buf, &bitpos, x, 2);
      IF x==DAY_START_EQUAL THEN
        /* start day */
        utilAddInt(buf, &bitpos, rule[i].startDay[j], 5)
      ELSIF x==DAY_START_LEQ || x==DAY_START_GEQ THEN
        /* start day and day of week */
        utilAddInt(buf, &bitpos, rule[i].startDay[j], 5);
        utilAddInt(buf, &bitpos, rule[i].startWeekday[j], 3)
      ELSE /* DAY_START_LAST */
        /* day of week */
        utilAddInt(buf, &bitpos, rule[i].startWeekday[j], 3)
      END;
      /* start time */
      utilAddInt(buf, &bitpos, rule[i].startTime[j], 11);
      /* start time relative to */
      utilAddInt(buf, &bitpos, rule[i].startTimeRel[j], 2);
      /* time saved */
      x = rule[i].saveTime[j];
      IF x==0 || x==60 THEN
        utilAddFlag(buf, &bitpos, 1);
        utilAddFlag(buf, &bitpos, x)
      ELSE 
        utilAddFlag(buf, &bitpos, 0);
        utilAddInt(buf, &bitpos, x, 11)
      END;
      /* save time string */
      x=rule[i].saveString[j];
      IF !strcmp(format[x].name,"S") THEN
        utilAddFlag(buf, &bitpos, 1)
      ELSE
        utilAddFlag(buf, &bitpos, 0);
        IF !strcmp(format[x].name,"-") THEN
          utilAddFlag(buf, &bitpos, 1)
        ELSE
          utilAddFlag(buf, &bitpos, 0);
          utilAddInt(buf, &bitpos, rule[i].saveString[j], 9)
        END
      END;
      d = bitpos-bitposi+7;
      IF d>max THEN max=d END
    END
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN
    printf("\nrule:     %5d (%6d bits, max=%4d bits)",*plen,bitpos,max)
  END
END
