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

/* tzgen.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "modula.h"
#include "tzutil.h"

#include "tzcountry.h"
#include "tzarea.h"
#include "tzlocation.h"
#include "tzrule.h"
#include "tzformat.h"
#include "tzzone.h"
#include "tzlocation2.h"

/* change these to fit your environment */
#define TZPATH   "Harddisk:Play:delphi:astrolib:"
#define TZINDIR  "tzdata:"
#define TZOUTDIR "generated:"
#define TZDATA "tzdata"

extern int tzverbose;
extern char tzserbuf[65536]; /* buffer for serialized data */
