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

/* ================================================================ 

Function: makeUniqueDosFileName 

OVERVIEW:

The following function is used to create "unique" 8.3 filenames.  This is
a major concerning when creating fidonet PKT files.  If you use this
function to create PKT filenames, and your node only runs programs that use
the same revision of the fidoconfig library, it will be guranteed that your
program will emit unique PKT filenames throughout the whole epoch (!), and
in almost all cases you can also be sure that your packets will not have
name clashes with packets from other nodes that run fidoconfig programs
during a period of about two days.  (Normally, the tosser of your uplink
should not have a problem with clashes of pkt filenames from different links
anyway, but just in case he uses a brain-dead tosser ...).

CALLING:

The function takes a directory name as argument (which is prepended to the
generated file name, but has no further meaning), an extension (which again
is appended to the gernated file name, but has no furhter meaning), and a
fidoconfig structure.  The fidoconfig sturcture is used to create a basic
offset number that distinguishes your node/point from another node/point.

DETAILS:

The function guarantees to create
  - 36 filenames per second (if the average number of filenames created
    per second over the whole lifetime of the program is greater, the
    program will be delayed for the appropriate time via an atexit
    handler). That is, you can poll as many filenames out of the function
    as you wish, and you will get them quickly when you poll them, the
    worst case is that if your program ran a very short time it will be
    delayed before finally exiting.
  - If the fidoconfig file always has the same primary AKA, and no two
    programs that use that use the function run at the same time on your
    system, it will emit unique filenames during the whole epoch (!).
    That is, a Fidonet editor should NOT use this function (because a
    editor usually runs parallel to the tossertask), while the tosser,
    router, ticker, ... may safely use it as those programs usually run
    serialized.
  - If the primary AKA in the fidoconfig changes, the file names will
    change substantially, so that different nodes who both run hpt
    or other fidoconfig programs will generate substantially different file
    names.
  - ATTENTION: This function presently does not check if the file
    names that it calculates do already exist. If you are afraid of
    this (because you also use non-fidoconfig programs), you must
    check it yourself in the application.

IMPLEMENTATION NOTES:

The alogrithm for creating pkt file names works as follows:

 - Step 1:  Based on OUR AKA, a static offset number is computed.  This is
   done so that two systems which both run hpt create somewhat different
   PKT file name. The offset number is computed as follows:

      We imagine the node numbe decomposed in its digits:
         node  = node1000 * 1000 + node100 * 100 + node10 *10 + node1
      analoguous for the net number:
         net   = net1000 * 1000 + net100 * 100 + net10 * 10 + net1
      and the point number:
         point = point1000 * 1000 + point100 * 100 + point10 * 10 + point1

      Then the decimal digits of "offset" are composed as follows:

      8         7         6        5        4        3        2        1
  (I) node10    node1     net10    net1     node100  node1000 net100   net1000
 (II) node10    node1     point10  point1   node100  node1000 net100   net1000

      where line (I) is used if point!=0, and line (II) is used if point==0.

      Then the offset number is multiplied by 21 * 36. (This will overflow
      a 32 bit unsigned integer, so in the code we only multiply by 21 and
      then do other tricks).

 - Step 2: If this is the first start, the value of time() is obtained and
      increased by one. That value is the "base packet number" for the file
      name about to be created.

 - Step 3: The base packet number is added to the offset number and printed
      as a seven digit number in a 36-based number system using the digits
      0..9 and a..z. (We need all alphanumeric characters to assure all
      uniquenesses that we guranteed ... hexadezimal characters are not
      enough for this).

 - Step The last (eight) character in the generated file name is
      is a counter to allow for creating more than one filename per second.
      The initial value of the counter is:

            (net10 * 10 + net1 + point100) modulo 36

 - Step 5: The counter is printed as the eight character using characters
      0..9, and a..z (for counter values from 10 to 35).

 - Step 6: On subsequent calls of this routine, the counter value is
      increased by one. If it becomes greater than 35 it is wrapped to zero.
      If all counter values have been used up (i.E. after the increasement
      and possibly wrapping the counter value is again the initial value),
      the base packet number is increased by one.

  - Step 7: At program exit, the program sleeps until the value of time()
      is greater or at least equal to the highest base packet number that
      has been used.
      (This is done via atexit. If the registering of the atexit program
      fails, the algorithm above is modified so that every time that
      the base packet number is increased, the program immediately waits
      until time() is equal the base packet number. This is slower, but
      it is a secure fallback in case atexit fails).

The result is:

  - The routine is able to create 36 filenames per second. If more filenames
    are requested within a single second, calling the routine might delay
    program execution, but the routine will still produce as many file names
    as you request.

  - As long as the AKA that is based for calculating the offset does not
    change, and of course as long as the system clock is running continously
    without being turned backwards, the routine will create unique filenames
    during the whole epoch!

  - For different nodes that have different AKAs, there will usually be a
    considerable distance between the filenames created by the one node and
    those created by another node. Especially, different points of the same
    node will usually have different file names, and different nodes of the
    same hubs will usually have very different file names over a period
    of at least one day, and usually much more. There is no exact guarantee
    that always two different nodes create different file names within this
    period, but the chances are high. (Note that any decent tosser should
    not have any problem with receving arcmail bundles from two different
    nodes that contain pkt files with the same name; however, Fastecho
    and probably others do have this problem unless the sysop installs
    special scripts to circument it, and this is why we do the whole shit
    of sender specific offset numbers ...)

  - Remark: This code requires sizeof(unsinged long) >= 4. This is true
    for all known 16, 32 and 64 bit architectures.

   ================================================================ */

static time_t last_reftime_used;
static int may_run_ahead;

static void atexit_wait_handler_function(void)
{
    time_t t;

    time(&t);
    while (t < last_reftime_used)
    {
#ifdef UNIX
        usleep(10);
#else
        sleep(1);
#endif
        time (&t);
    }
}
    
char *makeUniqueDosFileName(const char *dir, const char *ext,
			    s_fidoconfig *config)
{
   char                *fileName;
   
   static unsigned      counter  = 0x100, refcounter = 0x100;
   static time_t        refTime  = 0x0;
   static short         reftime36[7];
   static volatile int  flag = 0;

   unsigned             icounter;
   time_t               tmpt;
   static char          digits[37]="0123456789abcdefghijklmnopqrstuvwxyz";
   int                  i, digit;
   short                offset36[7];
   unsigned long        node10, node1, digit6, digit5, node100, node1000,
                        net100, net1000, tempoffset, net10, net1, point100;
   size_t               pathLen  = strlen(dir);

   /* make it reentrant */
   while (flag)
   {
#if defined(UNIX) || defined(EMX)   
       usleep(10);       /* wait to get a fresh number */
#else
       sleep(1);
#endif
   }

   flag = 1;

   if ((fileName = malloc(pathLen + 1 + 8 + 1 + strlen(ext) + 1)) == NULL)
   {                            /* delim file . ext null */
       flag = 0;
       return NULL;
   }
                           
   memcpy(fileName, dir, pathLen + 1);

   if (pathLen && fileName[pathLen - 1] != '\\' &&
                  fileName[pathLen - 1] != '/' &&
                  fileName[pathLen - 1] != PATH_DELIM)
   {
       fileName[pathLen + 1] = '\0';
       fileName[pathLen] = PATH_DELIM;
       pathLen++;
   }

   if (refTime == 0x0)
   {
       time(&refTime);
       may_run_ahead = !atexit(atexit_wait_handler_function);
       last_reftime_used = refTime;
   }

   /* we make a node specific offset, so that two nodes that both run hpt
      each generate more or less distinct pkt file names */

   node10 = (config->addr[0].node % 100) / 10;
   node1  = (config->addr[0].node % 10);
   if (config->addr[0].point != 0)
   {
       digit6 = (config->addr[0].point % 100) / 10;
       digit5 = config->addr[0].point % 10;
   }
   else
   {
       digit6 = (config->addr[0].net % 100) / 10;
       digit5 = (config->addr[0].net % 10);
   }
   node100  = (config->addr[0].node % 1000) / 100;
   node1000 = (config->addr[0].node % 10000) / 1000;
   net100   = (config->addr[0].net % 1000) / 100;
   net1000  = (config->addr[0].net % 10000) / 1000;
   net10    = (config->addr[0].net % 100) / 10;
   net1     = config->addr[0].net % 10;
   point100 = (config->addr[0].point % 1000) / 100;
 

   tempoffset = (node10   * 10000000UL +
                 node1    * 1000000UL  +
                 digit6   * 100000UL   +
                 digit5   * 10000UL    +
                 node100  * 1000UL     +
                 node1000 * 100UL      +
                 net100   * 10UL       +
                 net1000  * 1UL          ) * 21UL;

   icounter = (unsigned)((net10 * 10U + net1 + point100) % 36U);

   offset36[0] = 0;  /* this is the multiplication by 36! */
   for (i = 1; i <= 6; i++)
   {
       offset36[i] = tempoffset % 36;
       tempoffset = tempoffset / 36;
   }

   do
   {
       if (counter == icounter || icounter != refcounter)
       {
	   counter = refcounter = icounter;
           last_reftime_used = ++refTime;

           if (!may_run_ahead)
           {
               time (&tmpt);
	   
               while (tmpt < refTime)
               {
#if defined(UNIX) || defined(EMX)   
                   usleep(50);       /* wait to get a fresh number */
#else
                   sleep(1);
#endif
                   time(&tmpt);
               }
           }

           tmpt = refTime;
           for (i = 0; i <= 6; i++)
           {
               reftime36[i] = tmpt % 36;
               tmpt         = tmpt / 36;
           }
       }

       for (i = 0, digit = 0; i < 7; i++)
       {
           digit = digit + reftime36[i] + offset36[i];
           fileName[pathLen + (6 - i)] = digits[digit % 36];
           digit = digit / 36;
       }

       sprintf(fileName + pathLen + 7, "%c.%s", digits[counter], ext);
       counter = ((counter + 1) % 36);

   } while (0); /* too slow because of readdir: fexist(fileName) == TRUE */;

   flag = 0;

   return fileName;
}

#ifdef UNIX
#define MOVE_FILE_BUFFER_SIZE 128000
#else
#define MOVE_FILE_BUFFER_SIZE 16384
#endif

int move_file(const char *from, const char *to)
{
    int rc;
    char *buffer;
    size_t read;
    FILE *fin, *fout;
	

    rc = rename(from, to);
    if (!rc) return 0;      /* rename succeeded. fine! */

    /* Rename did not succeed, probably because the move is accross
       file system boundaries. We have to copy the file. */

    buffer = malloc(MOVE_FILE_BUFFER_SIZE);
    if (buffer == NULL)	return -1;

    fin = fopen(from, "rb");
    if (fin == NULL) { free(buffer); return -1; }

    fout = fopen(to, "wb");
    if (fout == NULL) { free(buffer); return -1; }

    while ((read = fread(buffer, 1, MOVE_FILE_BUFFER_SIZE, fin)) > 0)
    {
	if (fwrite(buffer, 1, read, fout) != read)
	{
	    fclose(fout); fclose(fin); remove(to); free(buffer);
	    return -1;
	}
    }

    if (ferror(fout) || ferror(fin))
    {
	fclose(fout);
	fclose(fin);
	free(buffer);
	remove(to);
	return -1;
    }

    fclose(fout);
    fclose(fin);
    free(buffer);
    return 0;
}

	

    
	
	
    
  
