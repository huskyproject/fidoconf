/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998-1999
 *
 * Matthias Tichy
 *
 * Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 * Internet: mtt@tichy.de
 *
 * Grimmestr. 12         Buchholzer Weg 4
 * 33098 Paderborn       40472 Duesseldorf
 * Germany               Germany
 *
 * This file is part of FIDOCONFIG.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see file COPYING. If not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************
 * $Id$
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>

#include <huskylib/compiler.h>
#include <huskylib/huskylib.h>
#include "fidoconf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_aka2str 24   /* Size of static variable returned by aka2str() */

/* common functions */

FCONF_EXT int copyString(char *str, char **pmem);
/*DOC
 * Copy string from *str to *pmem, allocating memory as needed
   NOTE! *pmem must be NULL, if not NULL, it will be free().
 */

int copyStringUntilSep(char *str, char *seps, char **dest);
/*DOC
 * Copy chars from str to *dest until one of the chars in seps appears
 * memory is allocated as needed
 * *dest will be freed if non-NULL
 * returns number of chars copied
 */

FCONF_EXT long str2attr(const char *str);
/*DOC
  Input:  Msg flag name
  Output: value with corresponding bit set or -1L else
*/

FCONF_EXT char *attr2str(long attr);
/*DOC
  Input:  Msg flag bitmask
  Output: flags string, dynamic allocated, space separated
*/

FCONF_EXT char *extattr(const char *line);
/*DOC
  Input:  Msg extended flag name
  Output: Uppercased flag name or NULL if flag unknown
*/

FCONF_EXT int  addrComp(const hs_addr a1, const hs_addr a2);
/*DOC
  Input:  two addresses
  Output: 0, or !0
  FZ:     0 ist returned if the two addresses are the same, !0 else
*/

FCONF_EXT void string2addr(const char *string, hs_addr *addr);
/*DOC
  Input:  string is an \0-terminated array of chars. is a pointer to a struct addr.
  Output: ./.
  FZ:     string2addr converts a char[] to an addr. If string is not an addr NULL ist returned.
*/

HUSKYEXT char *makeUniqueDosFileName(const char *dir, const char *ext, s_fidoconfig *config);
/*DOC
   Input:  dir: \0 terminated string designating the destination directory
           ext: \0 terminated string designating the file extension w/o dot
           config: is used to generate a node specific offset
   Output: a pointer to a malloc'ed \0 terminated string is returned.
   FZ:     Creates a unique DOS compatible file name inside the given directory.
           See the comments in common.c for further explanations
*/

/* will be moved to huskylib */
FCONF_EXT  char *makeMsgbFileName(ps_fidoconfig config, char *s);
/*
    makes correct file neme fot echo or fecho area
*/

FCONF_EXT char   *vars_expand(char *str);
/*DOC
   Input: str is a \0 terminated string which must have been malloc'ed
   Ouput: a pointer to a \0 terminated string is returned which must be free'd
   FZ:    vars_expand expands the strings just like [home]/etc to /home/fnet/etc
          and (under unix and os2/emx) `uname` to Linux
*/

/* will be moved to huskylib */
FCONF_EXT int  NCreateOutboundFileName(ps_fidoconfig config, s_link *link, e_flavour prio, e_pollType typ);
FCONF_EXT int  NCreateOutboundFileNameAka(ps_fidoconfig config, s_link *link, e_flavour prio, e_pollType typ, hs_addr *aka);
/*DOC
  Input:  link is the link whose OutboundFileName should be created.
          prio is some kind of CRASH, HOLD, NORMAL
          typ is some kind of PKT, REQUEST, FLOFILE
  Output: a pointer to a char is returned.
  FZ:     1 is returned if link is busy
         -1 can't create bsy file
          0 else
          */

/*  fileBoxes support */

/* will be moved to huskylib */
FCONF_EXT int needUseFileBoxForLink (ps_fidoconfig config, s_link *link);
FCONF_EXT int needUseFileBoxForLinkAka (ps_fidoconfig config, s_link *link, hs_addr *aka);
/* will be moved to huskylib */
FCONF_EXT char *makeFileBoxName     (ps_fidoconfig config, s_link *link);
FCONF_EXT char *makeFileBoxNameAka  (ps_fidoconfig config, s_link *link, hs_addr *aka);
/* will be moved to huskylib */
FCONF_EXT void fillCmdStatement(char *cmd, const char *call, const char *archiv, const char *file, const char *path);

/* will be moved to huskylib */
/*  Change file sufix (add if not present).
    inc = 1 - increment suffix of file if new file exist;
          rename file; return new file name or NULL; set errno
    inc = 0 - do not increment suffix, do not rename file, return new suffix only
    if 1st or 2nd parameter is NULL return NULL and set errno to EINVAL
*/
FCONF_EXT char* changeFileSuffix(char *fileName, char *newSuffix, int inc);

/*  this function returns the string representation of an address. */
/*  it returns a static array!!! */
FCONF_EXT char *aka2str(const hs_addr aka);

/* This function returns the string representation of an 5D address.
 * Return malloc()'ed string!
 */
FCONF_EXT char *aka2str5d(hs_addr aka);

void freeGroups(char **grps, int numGroups);
char **copyGroups(char **grps, int numGroups);
void freeLink (s_link *link);

FCONF_EXT int e_readCheck(s_fidoconfig *config, s_area *echo, s_link *link);
/*  '\x0000' access o'k */
/*  '\x0001' no access group */
/*  '\x0002' no access level */
/*  '\x0003' no access export */
/*  '\x0004' not linked */

FCONF_EXT int e_writeCheck(s_fidoconfig *config, s_area *echo, s_link *link);
/*  '\x0000' access o'k */
/*  '\x0001' no access group */
/*  '\x0002' no access level */
/*  '\x0003' no access import */
/*  '\x0004' not linked */

/* Select PackAka: link->hisPackAka if PackAka defined, link->hisAka otherwise. */
FCONF_EXT hs_addr *SelectPackAka(s_link *link);


#ifdef __cplusplus
}
#endif

#endif

