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

/* tzrule.h */

#include "modula.h"

#define RULE_MAX      256 /* maximal number of rules */
#define RULE_ITEM_MAX 128 /* maximal number of rule items */

#define YEAR_MAX      9999 /* maximal year */

#define PARITY_ALL    0
#define PARITY_ODD    1
#define PARITY_EVEN   2

#define DAY_START_EQUAL 0  /* rule starts at day of month (each year) */
#define DAY_START_LEQ   1  /* rule starts at weekday <= day of month */
#define DAY_START_GEQ   2  /* rule starts at weekday >= day of month */
#define DAY_START_LAST  3  /* rule starts at last weekday of month */

#define REL_UTC         0  /* time indicated in UTC */
#define REL_STANDARD    1  /* standard time (according to zone, neglecting previous rules) */
#define REL_LOCAL       2  /* local time (according to zone plus previous rule) */

RECORD ruleType
BEGIN
  int  used;       /* used temporarily to eliminate unused elements */
  char name[40];   /* name of the rule */
  int  nRuleItem;  /* number of rule items for this rule */
  int  yearFrom    [RULE_ITEM_MAX]; /* first year rule applies to (except parity) */
  int  yearTo      [RULE_ITEM_MAX]; /* last year rule applies to (except parity) */
  int  yearParity  [RULE_ITEM_MAX]; /* range applies to all/only odd/only even years */
  int  startMonth  [RULE_ITEM_MAX]; /* first month (of each year) the rule applies to */
  int  startDayType[RULE_ITEM_MAX]; /* equal, leq, geq or last */
  int  startDay    [RULE_ITEM_MAX]; /* day of month */
  int  startWeekday[RULE_ITEM_MAX]; /* day of week, no meaning if type is equal */
  int  startTime   [RULE_ITEM_MAX]; /* hrs*60+min, secs rounded to next minute */
  int  startTimeRel[RULE_ITEM_MAX]; /* UTC/standard/local time */
  int  saveTime    [RULE_ITEM_MAX]; /* time saved (hrs+60+min, secs rounded) */
  int  saveString  [RULE_ITEM_MAX]  /* index in format, replaces %s in zone format */
END;

extern RECORD ruleType rule[RULE_MAX]; /* all rules */
extern int nRule;                      /* number of rules */

PROCEDURE ruleAddLine(char* line);
/* scan line and add a new rule or rule item to the global array 'rule' */

PROCEDURE rulePrint();
/* print list of rules to stdout */

FUNCTION int ruleItemNum();
/* returns the total number of rule items */

FUNCTION int ruleItemMax();
/* returns the largest number of rule items per rule */

PROCEDURE ruleSerialize(char* buf, int* plen);
/* serialize rules into a buffer,
 * the length of the buffer is returned in *plen
 */
