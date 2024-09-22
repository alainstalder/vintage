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

/* tzcountry.c */

#include "tzgen.h"

VAR RECORD countryType country[COUNTRY_MAX];
    int nCountry=0;

PROCEDURE countryAddLine(char* line)
BEGIN
  VAR RECORD countryType* cp;
  IF nCountry>=COUNTRY_MAX THEN
    printf("\nCould not add country, increase value of COUNTRY_MAX...");
    RETURN
  END;
  /* set data fields */
  cp = &country[nCountry];
  cp->used = 0; /* we don't know yet */
  cp->firstLoc = 0; /* we don't know yet */
  strncpy(cp->code,line,2); cp->code[2]='\0';
  strcpy(cp->name,line+3);
  nCountry++;
  countrySort()
END

PROCEDURE countrySort()
BEGIN
  VAR int i,done,t;
      char tstr[256];
  REPEAT
    done=1;
    FOR i=0; i<nCountry-1; i++ DO
      IF utilStrCaselessCmp(country[i].name, country[i+1].name)>0 THEN
        /* swap */
        strcpy(tstr,country[i].name);
        strcpy(country[i].name,country[i+1].name);
        strcpy(country[i+1].name,tstr);
        strcpy(tstr,country[i].code);
        strcpy(country[i].code,country[i+1].code);
        strcpy(country[i+1].code,tstr);
        t=country[i].used;
        country[i].used=country[i+1].used;
        country[i+1].used=t;
        t=country[i].firstLoc;
        country[i].firstLoc=country[i+1].firstLoc;
        country[i+1].firstLoc=t;
        done=0
      END
    END
  UNTIL done DONE
END

PROCEDURE countryPrint()
BEGIN
  VAR int i;
  
  printf("\nCOUNTRIES:\nindex used firstLoc code name");
  FOR i=0; i<nCountry; i++ DO
    printf("\n%-5d %-4d %-8d %-4s %s",i,country[i].used,country[i].firstLoc,
      country[i].code,country[i].name)
  END;
  printf("\nEND COUNTRIES\n")
END

FUNCTION int countryTrim()
BEGIN
  VAR int i,j,n;
  
  /* set used flags in country */
  n=0;
  FOR i=0; i<nLocation; i++ DO
    country[location[i].country].used = 1
  END;
  /* remove countries without location */
  FOR i=0; i<nCountry; i++ DO
    IF !country[i].used THEN
      IF tzverbose THEN
        printf("\nRemoved country %d %s %s",i+n,country[i].code,country[i].name)
      END;
      /* remove country */
      nCountry--;
      FOR j=i; j<nCountry; j++ DO
        country[j].used = country[j+1].used;
        strcpy(country[j].code,country[j+1].code);
        strcpy(country[j].name,country[j+1].name)
      END;
      /* shift down indices in location */
      FOR j=0; j<nLocation; j++ DO
        IF location[j].country>i THEN location[j].country-- END
      END;
      i--; /* decrement i, else one country would be skipped */
      n++; /* increment count of removed countries */
    END;
    /* reset used flag */
    country[i].used = 0
  END;
  RETURN n
END

PROCEDURE countrySerialize(char* buf, int* plen)
BEGIN
  VAR int i,bitpos,bitpos1,len;
  len=15;
  bitpos1=0;
  bitpos=len*nCountry;
  FOR i=0; i<nCountry; i++ DO
    utilAddInt(buf,&bitpos1,bitpos,len);
    utilAddInt(buf,&bitpos,country[i].firstLoc,9);
    utilAddChar(buf,&bitpos,country[i].code[0],1 /*nolower*/);
    utilAddChar(buf,&bitpos,country[i].code[1],1 /*nolower*/);
    utilAddString(buf,&bitpos,country[i].name)
  END;
  *plen = (bitpos+7)/8;
  IF tzverbose THEN printf("\ncountry:  %5d (%6d bits)",*plen,bitpos) END;
END
