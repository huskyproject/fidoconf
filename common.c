#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef UNIX
#include <pwd.h>
#endif

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

  while ((*start != ':')&&(*start != ' ')&&(i < 31)) {    // copy zone info or preceding domain
      buffer[i] = *(start++);
      i++;
   } /* endwhile */
   buffer[i] = '\0';
   if (!isdigit(buffer[0])) {
      // Domain name could be in front of the addr, not FTS-compatbible!!!!!
      // software that such crap is generating should be xxxx
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
         addr->domain = (CHAR *) malloc(strlen(buffer)+1);
         strcpy(addr->domain, buffer);
      } else {
         addr->domain = NULL; //no domain
      } /* endif */
      break;
   default:
     break;
   } /* endswitch */
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

INT   fgetsUntil0(CHAR *str, int n, FILE *f)
{
   int i;

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
