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

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef UNIX
#include <pwd.h>
#endif

#include "fidoconfig.h"
#include "common.h"

int  addrComp(const s_addr a1, const s_addr a2)
{
   int rc = 0;

   rc =  a1.zone  != a2.zone;
   rc += a1.net   != a2.net;
   rc += a1.node  != a2.node;
   rc += a1.point != a2.point;

   return rc;
}

char *strrstr(const char *HAYSTACK, const char *NEEDLE)
{
   char *start = NULL, *temp = NULL;

   temp = strstr(HAYSTACK, NEEDLE);
   while (temp  != NULL) {
      start = temp;
      temp = strstr(temp+1,NEEDLE);
   }
   return start;
}

void string2addr(const char *string, s_addr *addr)
{
  const char *start = string;
  char buffer[32];
  int  i = 0;

  addr->domain = NULL;

  while ((*start != ':')&&(*start != ' ')&&(i < 31)) {    // copy zone info or preceding domain
      buffer[i] = *(start++);
      i++;
   } /* endwhile */
   buffer[i] = '\0';
   if (!isdigit(buffer[0])) {
      // Domain name could be in front of the addr, not FTS-compatible!!!!!
      // software which is such crap generating should be xxxx
      addr->domain = (char *) malloc(strlen(buffer)+1);
      strcpy(addr->domain, buffer);
   } else addr->zone = atoi(buffer);

   i = 0;
   start++;

   if (strchr(start, '/')!= NULL) {
      while (*start != '/') {                           // copy net info
         buffer[i] = *(start++);
         i++;
      } /* endwhile */
      buffer[i] = '\0';
      addr->net = atoi(buffer);

      i = 0;
      start++;
   }

   while ((*start != '.') && (*start != '\0') && (*start != '@')) {      // copy node info
      buffer[i] = *(start++);
      i++;
   } /* endwhile */
   buffer[i] = '\0';
   addr->node = atoi(buffer);

   i = 0;

   switch (*start) {
   case '\0':                            // no point/domain info
      start++;
      addr->point = 0;
      break;
   case '@':                            // no point, but domain info
      start++;
      while ((*start != '\0')&&(i < 31)) {
         buffer[i] = *start;
         i++; start++;
      } /* endwhile */
      buffer[i] = '\0';
      free(addr->domain);
      addr->domain = (CHAR *) malloc(strlen(buffer)+1);
      strcpy(addr->domain, buffer);
      addr->point = 0;
      break;
   case '.':                            // point info / maybe domain info
      start++;
      while ((*start != '@') && (*start != '\0')) {           // copy point info
         buffer[i] = *(start++);
         i++;
      } /* endwhile */
      buffer[i] = '\0';
      addr->point = atoi(buffer);
      i = 0;
      if (*start == '@') {                                   // copy domain info
         start++;
         while ((*start != '\0')&&(i < 31)) {
            buffer[i] = *start;
            i++; start++;
         } /* endwhile */
         buffer[i] = '\0';
         free(addr->domain);
         addr->domain = (CHAR *) malloc(strlen(buffer)+1);
         strcpy(addr->domain, buffer);
      } else {
         free(addr->domain);
         addr->domain = NULL; //no domain
      } /* endif */
      break;
   default:
     break;
   } /* endswitch */
   /* all-catch for domain = NULL */
   /* if  (addr->domain == NULL) {
   	  addr->domain  = malloc(1);
        *(addr->domain) = '\0';
      };
   */
   
   return;
}

UINT16 getUINT16(FILE *in)
{
   UCHAR dummy;

   dummy = (UCHAR) getc(in);
   return (dummy + (UCHAR ) getc(in) * 256);
}

int fputUINT16(FILE *out, UINT16 word)
{
  UCHAR dummy;

  dummy = word % 256;        // write high Byte
  fputc(dummy, out);
  dummy = word / 256;        // write low Byte
  return fputc(dummy, out);
}

INT   fgetsUntil0(CHAR *str, size_t n, FILE *f)
{
   size_t i;

   for (i=0;i<n-1 ;i++ ) {
      str[i] = getc(f);

      if (feof(f)) {
         str[i+1] = 0;
         return i+2;
      } /* endif */

      if (0 == str[i]) {
         return i+1;
      } /* endif */

   } /* endfor */

   str[n-1] = 0;
   return n;
}

char *stripLeadingChars(char *str, const char *chr)
{
   char *i = str;

   if (str != NULL) {
   
      while (NULL != strchr(chr, *i)) {       // *i is in chr
         i++;
      } /* endwhile */                        // i points to the first occurences
                                              // of a character not in chr
      strcpy(str, i);
   }
   return str;
}

char *strUpper(char *str)
{
   char *temp = str;
   
   while(*str != 0) {
      *str = toupper(*str);
      str++;
   }
   return temp;
}

char *shell_expand(char *str)
{
    char *slash = NULL, *ret = NULL, c;
#ifdef UNIX
    struct passwd *pw = NULL;
#endif
    char *pfix = NULL;

    if (str == NULL)
    {
        return str;
    }
    if (*str == '\0' || str[0] != '~')
    {
        return str;
    }
    for (slash = str; *slash != '/' && *slash != '\0'
#ifndef UNIX
                     && *slash != '\\'
#endif
         ; slash++);
    c = *slash;
    *slash = 0;

    if (str[1] == '\0')
    {
        pfix = getenv("HOME");
#ifdef UNIX        
        if (pfix == NULL)
        {
            pw = getpwuid(getuid());
            if (pw != NULL)
            {
                pfix = pw->pw_dir;
            }
        }
#endif
    }
#ifdef UNIX
    else
    {
        pw = getpwnam(str + 1);
        if (pw != NULL)
        {
            pfix = pw->pw_dir;
        }
    }
#endif
    *slash = c;

    if (pfix == NULL)  /* could not find an expansion */
    {
        return str;
    }

    ret = malloc(strlen(slash) + strlen(pfix) + 1);
    strcpy(ret, pfix);
    strcat(ret, slash);
    free(str);
    return ret;
}


/* This function creates a "unique" file with a name of 8 characters in
   the given directory witht the given extension. The file is "unique" over
   a period of 194 days, and the function can generate 256 filenames per
   second. If it is called more frequently, it sleeps until a new second has
   arrived.

   This function does only guarrantee uniqueness if no two processes that
   use the function are running at the same time. I.E., a Fidonet EDITOR
   should NOT use it, because an editor runs parrallel to the tosser, while
   Tosser, Ticker, Router, etc. can use it, because they are usually called
   subsequently.

   This is only a temporary solution. The function should be rewritten to
   use a counter file (accessed via record locking serialization mechanisms),
   which would also allow processes that run parallel to use it.
   
   Note that the file stem name is unique. I.e. if you want to create three
   unique files with different extensions, you only need to call this
   function once and then you can substitute arbitrary extensions safely.

   Also note that the function does NOT test if a file of the generated name
   might already exist. If you wish to prevent this case, you have to test it
   on your own.
*/

char *makeUniqueDosFileName(const char *dir, const char *ext)
{
   char                *fileName;
   static unsigned      counter  = 0x100;
   static time_t        refTime  = 0x0;
   time_t               oldTime;
   int                  exists;

#ifdef UNIX
   char                 delim    = '/';
#else
   char                 delim    = '\\';
#endif   

   size_t               pathLen  = strlen(dir);

   if ((fileName = malloc(pathLen + 1 + 8 + 1 + strlen(ext) + 1)) == NULL)
   {                            /* delim file . ext null */
       return NULL;
   }
                           
   memcpy(fileName, dir, pathLen, pathLen + 1);

   if (pathLen && fileName[pathLen - 1] != '\\' &&
                  fileName[pathLen - 1] != '/')
   {
       fileName[pathLen + 1] = '\0';
       fileName[pathLen] = delim;
       pathLen++;
   }

   if (refTime == NULL)
   {
       time(&refTime);
   }

   do
   {
       if (counter >= 0xFF)
       {
	   counter = 0;
	   oldTime = refTime;
	   time (&refTime);
	   
	   while (oldTime == refTime)
	   {
	       sleep(1); /* wait to get a fresh time number */
	       time(&refTime);
	   }
       }
       else
       {
	   counter++;
       }
          
       sprintf(fileName + pathLen, "%06lx%02x.%s", (unsigned long)refTime,
	       counter, ext);
   } while (0); /* too slow because of readdir: fexist(fileName) == TRUE */;

   return fileName;
}
