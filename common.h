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
 *****************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include "typesize.h"
#include <stdio.h>

/* common functions */

int  addrComp(const s_addr a1, const s_addr a2);
/*DOC
  Input:  two addresses
  Output: 0, or !0
  FZ:     0 ist returned if the two addresses are the same, !0 else
*/

char *strrstr(const char *HAYSTACK, const char *NEEDLE);

/*DOC
  Input:  two constant null-terminated strings
  Output: NULL or the point to a null-terminated string
  FZ:     finds the LAST occurence of NEEDLE in HAYSTACK
          (same as strstr but last occurence
*/

void string2addr(const char *string, s_addr *addr);
/*DOC
  Input:  string is an \0-terminated array of chars. is a pointer to a struct addr.
  Output: ./.
  FZ:     string2addr converts a char[] to an addr. If string is not an addr NULL ist returned.
*/

UINT16 getUINT16(FILE *in);
/*DOC
  Input:  in is an file stream opened for reading.
  Output: getUINT16 returns an UINT16
  FZ:     the UINT15 is read from the stream using the method lowByte, highByte.
*/

int    fputUINT16(FILE *out, UINT16 word);
/*DOC
  Input:  out is an file opened for writing.
          word is the UINT16 which should be written
  Output: fputUIN16 returns the return of the second fputc call.
  FZ:     fputUINT16 writes word into the stream using the order lowByte, highByte.
*/

INT    fgetsUntil0(CHAR *str, size_t n, FILE *f);
/*DOC
  Input:  n-1 chars are read at most.
          str is a buffer with the length n.
          f is a file stream opened for reading.
  Output: fgetsUntil0 returns the number of chars read including the last \0
  FZ:     fgetsUntil0 reads chars into the buffer until eof(f) || n-1 are read || a \0 is encountered.
*/

char   *stripLeadingChars(char *str, const char *chr);
/*DOC
  Input:  str is a \0-terminated string
          chr contains a list of characters.
  Output: stripLeadingChars returns a pointer to a string
  FZ:     all leading characters which are in chr are deleted.
          str is changed and returned.
*/

char   *strUpper(char *str);
/*DOC
  Input:  str is a \0 terminated string
  Output: a pointer to a \0 terminated string is returned.
  FZ:     strUpper converts the string from lower case to upper case.
  */

char   *shell_expand(char *str);
/*DOC
   Input: str is a \0 terminated string which must have been malloc'ed
   Ouput: a pointer to a \0 terminated string is returned which must be free'd
   FZ:    shell_expand expands the strings just like ~/.msged to /home/mtt/.msged
          see sh(1) for further explanations
*/

char *makeUniqueDosFileName(const char *dir, const char *ext);
/*DOC
   Input:  dir: \0 terminated string designating the destination directory
           ext: \0 terminated string designating the file extension w/o dot
   Output: a pointer to a malloc'ed \0 terminated string is returned.
   FZ:     Creates a unique DOS compatible file name inside the given directory.
           See the comments in common.c for further explanations
*/

#endif
