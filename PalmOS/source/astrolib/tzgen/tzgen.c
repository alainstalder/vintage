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

/* tzgen.c */

#include "tzgen.h"

int tzverbose = 1;
char tzserbuf[65536]; /* buffer for serialized data */

FUNCTION int main()
BEGIN
  VAR FILE* fp; FILE* fp2;
      char line[256],a[256];
      int i,n;
      char continent[7][20] = { "africa","antarctica","asia",
        "australasia","europe","northamerica","southamerica" };
  
  printf("Creating data structures from timezone data...");
  
  printf("\nStep 1: Reading countries from file 'iso3166.tab'... ");
  fp = fopen(TZPATH TZINDIR "iso3166.tab","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 1 END;
  WHILE !utilReadLine(fp,line) DO
    countryAddLine(line)
  END;
  fclose(fp);
  printf("%d countries read.",nCountry);
  /*countryPrint();*/
  
  printf("\nStep 2: Reading areas from file 'zone.tab'... ");
  fp = fopen(TZPATH TZINDIR "zone.tab","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 2 END;
  WHILE !utilReadLine(fp,line) DO
    areaAddLine(line)
  END;
  fclose(fp);
  printf("%d areas read.",nArea);
  /*areaPrint();*/
  
  printf("\nStep 3: Reading locations from file 'zone.tab'... ");
  fp = fopen(TZPATH TZINDIR "zone.tab","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 3 END;
  WHILE !utilReadLine(fp,line) DO
    locationAddLine(line)
  END;
  fclose(fp);
  locationQuicksort();
  printf("%d locations read.",nLocation);
  /*locationPrint();*/
  
  printf("\nStep 4: Removing countries without locations... ");
  n = countryTrim();
  IF tzverbose && n!=0 THEN printf("\n") END;
  printf("%d countries removed.",n);
  /*countryPrint();*/
  /*locationPrint();*/
  
  printf("\nStep 5: Concating continents to file 'world'... ");
  fp2 = fopen(TZPATH TZOUTDIR "world","w");
  IF fp2==NULL THEN printf("\nCould not open file - exiting."); RETURN 4 END;
  /* collect links, zones and rules from all continents */
  i=0;
  FOR n=0; n<7; n++ DO
    strcpy(a,TZPATH TZINDIR);
    strcat(a,continent[n]);
    fp = fopen(a,"r");
    IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 5 END;
    WHILE !utilReadLine(fp,(char*)&line) DO
      fprintf(fp2,"%s\n",line); i++
    END;
    fclose(fp)
  END;
  fclose(fp2);
  printf("%d files concated (%d lines).",n,i);

  printf("\nStep 6: Reading format strings from file 'world'... ");
  fp = fopen(TZPATH TZOUTDIR "world","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 8 END;
  WHILE !utilReadLine(fp,line) DO
    formatAddLine(line)
  END;
  fclose(fp);
  IF tzverbose THEN printf("\n") END;
  printf("%d strings read.",nFormat);
  /*formatPrint();*/

  printf("\nStep 7: Reading rules from file 'world'... ");
  fp = fopen(TZPATH TZOUTDIR "world","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 7 END;
  WHILE !utilReadLine(fp,line) DO
    ruleAddLine(line)
  END;
  fclose(fp);
  IF tzverbose THEN printf("\n") END;
  printf("%d rules read (%i items).",nRule,ruleItemNum());
  IF tzverbose THEN
    printf("\nMaximal number of items per rule: %i.",ruleItemMax())
  END;
  /*rulePrint();*/
  
  printf("\nStep 8: Reading zones from file 'world'... ");
  fp = fopen(TZPATH TZOUTDIR "world","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 9 END;
  WHILE !utilReadLine(fp,line) DO
    zoneAddLine(line)
  END;
  fclose(fp);
  IF tzverbose THEN printf("\n") END;
  printf("%d zones read (%i items).",nZone,zoneItemNum());
  IF tzverbose THEN
    printf("\nMaximal number of items per zone: %i.",zoneItemMax())
  END;
  /*zonePrint();*/

  printf("\nStep 9: Reading links from file 'world'... ");
  fp = fopen(TZPATH TZOUTDIR "world","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 6 END;
  WHILE !utilReadLine(fp,line) DO
    zoneAddLink(line)
  END;
  fclose(fp);
  IF tzverbose THEN printf("\n") END;
  printf("%d links read.",nLink);

  printf("\nStep 10:Reading locations2 from file 'moreplaces'... ");
  fp = fopen(TZPATH TZINDIR "moreplaces","r");
  IF fp==NULL THEN printf("\nCould not open file - exiting."); RETURN 3 END;
  WHILE !utilReadLine(fp,line) DO
    location2AddLine(line)
  END;
  fclose(fp);
  location2Quicksort();
  printf("%d locations read.",nLocation2);
  /*location2Print();*/

  printf("\nChecking data consistency... ");
  n = utilCheckData();
  IF n==0 THEN printf("OK.")
  ELSE printf("%d unused items found.",n)
  END;

  printf("\nLinking locations of same country... ");
  n = utilLinkLocations();
  IF n==0 THEN printf("OK.")
  ELSE printf("%d errors found.",n)
  END;
  /*countryPrint();*/
  /*locationPrint();*/
  /*location2Print();*/

  utilExportHeader();
  
  printf("\n\nFinished.");
  RETURN 0
END
