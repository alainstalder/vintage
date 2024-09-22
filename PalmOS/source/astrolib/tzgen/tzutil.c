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

/* tzutil.c */

#include "tzgen.h"

VAR char utilLine[256];
    int  utilLineIndex;
    char utilMonth[12][4] = { "Jan","Feb","Mar","Apr","May","Jun",
                              "Jul","Aug","Sep","Oct","Nov","Dec" };
    char utilWeekday[7][4] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

FUNCTION int utilReadLine(FILE* fp, char* line)
BEGIN
  VAR int i; char c;
 
  /* read line, remove comments */
  i=0; c = fgetc(fp);
  WHILE c!='\n' && c!=EOF DO
    line[i] = c=='\#'?'\0':c;
    i++; c = fgetc(fp)
  END;
  line[i]='\0';
  
  /* is it all whitespace ? */
  FOR i=0; line[i]!='\0' && line[i]<' '; i++ DO END;
  
  IF line[i]!='\0' THEN RETURN 0    /* normal line */
  ELSIF c==EOF THEN RETURN 1        /* end of file */
  ELSE RETURN utilReadLine(fp,line) /* empty line, return next one */
  END
END


PROCEDURE utilScanInit(char* line)
BEGIN
  strcpy(utilLine,line);
  utilLineIndex=0
END

PROCEDURE utilScanWord(char* word)
BEGIN
  VAR int i;
  /* skip whitespace */
  FOR i=utilLineIndex; utilLine[i]!='\0' && utilLine[i]<=' '; i++ DO END;
  /* scan word */
  FOR utilLineIndex=i; utilLine[i]!='\0' && utilLine[i]>' '; i++ DO END;
  strncpy(word,utilLine+utilLineIndex,i-utilLineIndex);
  word[i-utilLineIndex]='\0';
  utilLineIndex=i
END
      
PROCEDURE utilScanRest(char* rest)
BEGIN
  VAR int i;
  /* skip whitespace */
  FOR i=utilLineIndex; utilLine[i]!='\0' && utilLine[i]<=' '; i++ DO END;
  /* return rest */
  strcpy(rest,utilLine+i);
  utilLineIndex=strlen(utilLine)
END

FUNCTION char* utilSubString(char* str, int from, int len, char* substr)
BEGIN
  strncpy(substr,str+from,len);
  substr[len]='\0';
  RETURN substr
END

FUNCTION int utilStrCaselessCmp(char* str1, char* str2)
BEGIN
  VAR int i; char s1[256],s2[256];
      
  FOR i=strlen(str1); i>=0; i-- DO s1[i]=tolower(str1[i]) END;
  FOR i=strlen(str2); i>=0; i-- DO s2[i]=tolower(str2[i]) END;
  RETURN strcmp(s1,s2)
END

PROCEDURE utilCommonizeName(char* name)
BEGIN
  VAR int i; char a[256];
  
  /* replace "St_" at start of name by "St. " */
  IF !strncmp("St_",name,3) THEN
    strcpy(a,name+3);
    strcpy(name,"St. ");
    strcat(name,a)
  END;
  /* replace underscores by spaces */
  FOR i=0; name[i]!='\0'; i++ DO
    IF name[i]=='_' THEN name[i]=' ' END
  END
END

FUNCTION int utilSplitLocation(char* loc, int* parea, char* name)
BEGIN
  VAR int i; char a[256];
  
  /* scan until slash */
  FOR i=0; loc[i]!='\0' && loc[i]!='/'; i++ DO END;
  IF loc[i]=='\0' THEN RETURN 0 END; /* not in format <area>/<name> */
  /* name */
  strcpy(name,loc+i+1);
  utilCommonizeName(name);
  /* area */
  strncpy(a,loc,i); a[i]='\0';
  FOR i=0; i<nArea && strcmp(a,area[i].name); i++ DO END;
  *parea = i;
  RETURN i<nArea 
END

FUNCTION int utilGetMonth(char* str, int* pmonth)
BEGIN
  VAR int i;
  FOR i=0; i<12 && strcmp(str,utilMonth[i]); i++ DO END;
  *pmonth=i+1;
  RETURN i<12
END

FUNCTION int utilGetWeekDay(char* str, int* pweekday)
BEGIN
  VAR int i;
  FOR i=0; i<7 && strcmp(str,utilWeekday[i]); i++ DO END;
  *pweekday=i;
  RETURN i<7
END

FUNCTION int utilGetDay(char* str, int* pdaytype, int* pday, int* pweekday)
BEGIN
  VAR char a[256];
  
  IF '0'<=str[0] && str[0]<='9' THEN
    /* day of month given directly */
    *pdaytype = DAY_START_EQUAL;
    *pday = atoi(str);
    *pweekday = 9; /* does not matter */
    RETURN 1
  ELSIF strlen(str)>=4 && !strncmp("last",str,4) THEN
    /* last weekday of month */
    *pdaytype = DAY_START_LAST;
    *pday = 99; /* does not matter */
    RETURN utilGetWeekDay(str+4, pweekday)
  ELSIF strlen(str)>=5 && !strncmp("<=",str+3,2) THEN
    /* weekday <= day in month */
    *pdaytype = DAY_START_LEQ;
    *pday = atoi(str+5);
    strncpy(a,str,3); a[3]='\0';
    RETURN utilGetWeekDay(a,pweekday)
  ELSIF strlen(str)>=5 && !strncmp(">=",str+3,2) THEN
    /* weekday >= day in month */
    *pdaytype = DAY_START_GEQ;
    *pday = atoi(str+5);
    strncpy(a,str,3); a[3]='\0';
    RETURN utilGetWeekDay(a,pweekday)
  ELSE RETURN 0
  END
END

FUNCTION int utilGetTime(char* str, int* ptime, int* ptimerel)
BEGIN
  VAR int i,j,n,sign;
      char a[256];
  
  /* time relative to what */
  n=strlen(str);
  IF str[n-1]=='u' THEN n--; *ptimerel = REL_UTC
  ELSIF str[n-1]=='s' THEN n--; *ptimerel = REL_STANDARD
  ELSIF '0'<=str[n-1] && str[n-1]<='9' THEN *ptimerel = REL_LOCAL
  ELSE RETURN 0
  END;
  /* time */
  FOR i=0; i<n && str[i]!=':'; i++ DO END;
  strncpy(a,str,i); a[i]='\0';
  *ptime = atoi(a);
  sign=*ptime<0?-1:1;
  *ptime=abs(*ptime);
  IF i==n THEN *ptime*=60*sign; RETURN 1 END;
  j=i+1;
  FOR i=j; i<n && str[i]!=':'; i++ DO END;
  strncpy(a,str+j,i-j); a[i-j]='\0';
  *ptime = *ptime*60+atoi(a);
  IF i==n THEN *ptime*=sign; RETURN 1 END;
  *ptime= sign*(*ptime+((atoi(str+i+1)>=30 && *ptime<23*59+59)?1:0));
  RETURN 1
END

FUNCTION int utilCheckData()
BEGIN
  VAR int i,j,n;
  
  /* set all flags to unused */
  FOR i=0; i<nCountry; i++ DO country[i].used=0 END;
  FOR i=0; i<nLocation; i++ DO location[i].used=0 END;
  FOR i=0; i<nRule; i++ DO rule[i].used=0 END;
  FOR i=0; i<nZone; i++ DO zone[i].used=0 END;
  
  /* set all locations that have a zone to used, set those zones to used
  /* and set countries and areas that have a location with a zone to used */
  FOR i=0; i<nLocation; i++ DO
    IF location[i].zone != -1 THEN
      location[i].used=1;
      country[location[i].country].used=1;
      area[location[i].area].used=1;
      zone[location[i].zone].used=1
    END
  END;
  
  /* set all rules and format strings to used that are used by a zone */
  FOR i=0; i<nZone; i++ DO
    IF zone[i].used THEN
      FOR j=0; j<zone[i].nZoneItem; j++ DO
        rule[zone[i].offsRule[j]].used=1;
        format[zone[i].format[j]].used=1
      END
    END
  END;
  
  /* set all save strings to used that are used by a rule */
  FOR i=0; i<nRule; i++ DO
    IF rule[i].used THEN
      FOR j=0; j<rule[i].nRuleItem; j++ DO
        format[rule[i].saveString[j]].used=1
      END
    END
  END;
  
  /* print all unused items */
  n=0;
  FOR i=0; i<nCountry; i++ DO
    IF !country[i].used THEN
      n++; printf("\nunused country: %s=%s.",country[i].code,country[i].name)
    END
  END;
  FOR i=0; i<nArea; i++ DO
    IF !area[i].used THEN
      n++; printf("\nunused area: %s=%s.",country[i].code,country[i].name)
    END
  END;
  FOR i=0; i<nLocation; i++ DO
    IF !location[i].used THEN
      n++; printf("\nlocation without zone: %s",location[i].name)
    END
  END;
  FOR i=0; i<nZone; i++ DO
    IF !zone[i].used THEN
      n++; printf("\nunused zone: %d.",i) /* should not happen */
    END
  END;
  FOR i=0; i<nRule; i++ DO
    IF !rule[i].used THEN
      n++; printf("\nunused rule: %s.",rule[i].name)
    END
  END;
  FOR i=0; i<nFormat; i++ DO
    IF !format[i].used THEN
      n++; printf("\nunused format string: %s.",format[i].name)
    END
  END;
  RETURN n
END

FUNCTION int utilLinkLocations()
BEGIN
  VAR int i,j,k,n;
    
  /* find first location for each country */
  n=0;
  FOR i=0; i<nCountry; i++ DO
    FOR j=0; j<nLocation && location[j].country!=i; j++ DO END;
    IF j==nLocation THEN
      n++; printf("\ncountry without location: %s=%s.",country[i].code,country[i].name)
    ELSE country[i].firstLoc=j
    END
  END;

  /* find next location/location2 with same country for each location */
  FOR i=0; i<nLocation; i++ DO
    k = location[i].country;
    FOR j=i+1; j<nLocation && k!=location[j].country; j++ DO END;
    IF j==nLocation THEN
      /* search in location2 */
      FOR j=0; j<nLocation2 && k!=location[location2[j].location].country; j++  DO END;
      IF j==nLocation2 THEN location[i].nextLoc=0
      ELSE location[i].nextLoc=nLocation+j
      END
    ELSE location[i].nextLoc=j
    END
  END;
  
  /* find next location2 with same country for each location2 */
  FOR i=0; i<nLocation2; i++ DO
    k = location[location2[i].location].country;
    FOR j=i+1; j<nLocation2 && k!=location[location2[j].location].country; j++  DO END;
    IF j==nLocation2 THEN location2[i].nextLoc=0
    ELSE location2[i].nextLoc=nLocation+j
    END
  END;
  RETURN n
END

PROCEDURE utilAddBits(char* buf, int* pbitpos, char* bits, int bitlen)
BEGIN
  VAR int bit,mask,i;
  FOR i=0; i<bitlen; i++ DO
    bit = (bits[i/8]>>(i%8)) & 0x01;
    mask = 0xFF ^ (0x01<<((*pbitpos+i)%8));
    buf[(*pbitpos+i)/8] &= mask;
    buf[(*pbitpos+i)/8] |= bit<<((*pbitpos+i)%8)
  END;
  *pbitpos+=bitlen
END
 
PROCEDURE utilAddChar(char* buf, int* pbitpos, char ch, int nolower)
BEGIN
  VAR char x;
  IF 'A'<=ch && ch<='Z' THEN x = ch-'A'
  ELSIF ch=='-'  THEN x=26
  ELSIF ch=='_'  THEN x=27
  ELSIF ch=='%'  THEN x=28
  ELSIF ch==' '  THEN x=29
  ELSIF ch=='/'  THEN x=30 /* %s from format string */
  ELSIF ch=='\0' THEN x=31 /* end of string */
  ELSIF nolower THEN
    printf("\nutilAddChar: unexpected character '%c'.",ch)
  ELSIF 'a'<=ch && ch<='z' THEN x = 32+(ch-'a')
  ELSIF ch=='(' THEN x=58
  ELSIF ch==')' THEN x=59
  ELSIF ch=='&' THEN x=60
  ELSIF ch=='+' THEN x=61
  ELSIF ch==',' THEN x=62
  ELSE x=63
  END;
  utilAddBits(buf, pbitpos, &x, nolower?5:6);
  IF x==63 && !nolower THEN
    IF '0'<=ch && ch<='9' THEN x = ch-'0'
    ELSIF ch=='.' THEN x=10
    ELSIF ch=='\''  THEN x=11
    ELSE printf("\nutilAddChar: unexpected character '%c'.",ch)
    END;
    utilAddBits(buf, pbitpos, &x, 4);
  END
END

PROCEDURE utilAddString(char* buf, int* pbitpos, char* str)
BEGIN
  VAR int i;
  FOR i=0; i<=strlen(str); i++ DO
    utilAddChar(buf,pbitpos,str[i],0 /* has lower */)
  END
END
 
PROCEDURE utilAddUpperString(char* buf, int* pbitpos, char* str)
BEGIN
  VAR int i; char a[256];
  sprintf(a,str,"%"); /* replace %s by % */
  FOR i=0; i<=strlen(a); i++ DO
    utilAddChar(buf,pbitpos,a[i],1 /* nolower*/)
  END
END

PROCEDURE utilAddInt(char* buf, int* pbitpos, int x, int numbits)
BEGIN
  VAR int i; char a[8];
  FOR i=0; i<(numbits+7)/8; i++ DO
    a[i]= (char)(x&0xff);
    x>>= 8
  END;
  utilAddBits(buf,pbitpos,a,numbits)
END

PROCEDURE utilAddFlag(char* buf, int* pbitpos, int flag)
BEGIN
  utilAddInt(buf,pbitpos,flag!=0?1:0,1)
END

PROCEDUREs utilPrintComments(FILE *fp)
BEGIN
  fprintf(fp,"/***************************************************************/\n");
  fprintf(fp,"/* THIS IS A MACHINE GENERATED FILE -- EDITING NOT RECOMMENDED */\n");
  fprintf(fp,"/***************************************************************/\n");
  fprintf(fp,"\n");
  fprintf(fp,"/*\n");
  fprintf(fp,"astrolib: Palm OS shared library, calculates planetary positions\n");
  fprintf(fp,"          and time zones.\n");
  fprintf(fp,"\n");
  fprintf(fp,"Copyright (C) 2000-01 by delphi29@excite.com.\n");
  fprintf(fp,"\n");
  fprintf(fp,"This library is free software; you can redistribute it and/or modify it\n");
  fprintf(fp,"under the terms of the GNU Lesser General Public License as published\n");
  fprintf(fp,"by the Free Software Foundation; either version 2.1 of the License, or\n");
  fprintf(fp,"(at your option) any later version.\n");
  fprintf(fp,"\n");
  fprintf(fp,"This library is distributed in the hope that it will be useful, but\n");
  fprintf(fp,"WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  fprintf(fp,"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n");
  fprintf(fp,"Lesser General Public License for more details.\n");
  fprintf(fp,"\n");
  fprintf(fp,"You should have received a copy of the GNU Lesser General Public\n");
  fprintf(fp,"License along with this library; if not, write to the Free Software\n");
  fprintf(fp,"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n");
  fprintf(fp,"*/\n");
  fprintf(fp,"\n");
END

PROCEDUREs utilPrintBuffer(FILE* fp, char* name, char* buf, int len)
BEGIN
  VAR int i;
  fprintf(fp,"\n%s:\n",name);
  FOR i=0; i<len; i++ DO
    fprintf(fp,"%02X",(int)(buf[i]) & 0xFF);
    IF (i+1)%15000==0 THEN fprintf(fp,"\n") END /* else no output on Mac */
  END;
  fprintf(fp,"\n")
END

PROCEDURE utilExportHeader()
BEGIN
  VAR FILE* fp;
      int i,n;
  /*tzverbose=1;*/
  fp=fopen(TZPATH TZOUTDIR TZDATA ".h","w");
  IF fp==NULL THEN printf("\nCould not open output header file."); RETURN END;
  printf("\nExporting header file... ");
  utilPrintComments(fp);
  fprintf(fp,"/* " TZDATA ".h */");
  fprintf(fp,"\n\n#define NCOUNTRY   %d",nCountry);
  fprintf(fp,"\n#define NAREA      %d",nArea);
  fprintf(fp,"\n#define NLOCATION  %d",nLocation);
  fprintf(fp,"\n#define NRULE      %d",nRule);
  fprintf(fp,"\n#define NFORMAT    %d",nFormat);
  fprintf(fp,"\n#define NZONE      %d",nZone);
  fprintf(fp,"\n#define NLOCATION2 %d",nLocation2);
  fprintf(fp,"\n\n#define COUNTRY_RES_ID   9000");
  fprintf(fp,"\n#define AREA_RES_ID      9001");
  fprintf(fp,"\n#define LOCATION_RES_ID  9002");
  fprintf(fp,"\n#define RULE_RES_ID      9003");
  fprintf(fp,"\n#define FORMAT_RES_ID    9004");
  fprintf(fp,"\n#define ZONE_RES_ID      9005");
  fprintf(fp,"\n#define LOCATION2_RES_ID 9006");
  fprintf(fp,"\n\n");
  printf("done.");
  fclose(fp);
  
  fp=fopen(TZPATH TZOUTDIR TZDATA ".txt","w");
  IF fp==NULL THEN printf("\nCould not open output data file."); RETURN END;
  printf("\nExporting serialized data... ");
  n=0; i=0;
  countrySerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"COUNTRY_DATA (ID=9000)",tzserbuf,i);
  n+=i;
  areaSerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"AREA_DATA (ID=9001)",tzserbuf,i);
  n+=i;
  locationSerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"LOCATION_DATA (ID=9002)",tzserbuf,i);
  n+=i;
  ruleSerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"RULE_DATA (ID=9003)",tzserbuf,i);
  n+=i;
  formatSerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"FORMAT_DATA (ID=9004)",tzserbuf,i);
  n+=i;
  zoneSerialize(tzserbuf,&i);
  utilPrintBuffer(fp,"ZONE_DATA (ID=9005)",tzserbuf,i);
  n+=i;
  location2Serialize(tzserbuf,&i);
  utilPrintBuffer(fp,"LOCATION2_DATA (ID=9006)",tzserbuf,i);
  n+=i;
  IF tzverbose THEN printf("\n") END;
  printf("%d bytes (%dk) exported.",n,(n+512)/1024);
  fprintf(fp,"\n");
  fclose(fp)
END
