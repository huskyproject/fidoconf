/***********************************************************************
**                                                                    **
**   Function    :  patmat                                            **
**                                                                    **
**   Purpose     :  Pattern Matching                                  **
**                                                                    **
**   Usage       :  Pass two string pointers as parameters.The first  **
**                  being a raw string & the second a pattern the raw **
**                  string is to be matched against.If the raw string **
**                  matches the pattern,then the function returns a   **
**                  1,else it returns a 0.                            **
**                                                                    **
**                  e.g  patmat("abcdefghi","*ghi") returns a 1.      **
**                       patmat("abcdefghi","??c??f*") returns a 1.   **
**                       patmat("abcdefghi","*dh*") returns a 0.      **
**                       patmat("abcdefghi","*def") returns a 0.      **
**                                                                    **
**                  The asterisk is a wild card to allow any charac-  **
**                  ters from its first appearance to the next spec-  **
**                  ific character.The character ? is a wild card     **
**                  for only one character in the position it appears.**
**                  The hash mark "#" matches a single decimal figure.**
**                  Combinations such as "*?" or "?*" or "**" are     **
**                  illegal for obvious reasons & the functions may   **
**                  goof,though I think it will still work.           **
**                                                                    **
**   Author      :  Sreenath Chary              Nov 29 1988           **
**                                                                    **
**   Logic       :  The only simple way I could devise is to use      **
**                  recursion.Each character in the pattern is        **
**                  taken & compared with the character in the raw    **
**                  string.If it matches then the remaining amount    **
**                  of the string & the remaining amount of the       **
**                  pattern are passed as parameters to patmat again  **
**                  until the end of the pattern.If at any stage      **
**                  the pattern does not match,then we go back one    **
**                  level & at this level if the previous character   **
**                  was a asterisk in the pattern,we hunt again from  **
**                  where we left off,otherwise we return back one    **
**                  more level with a not found & the process goes    **
**                  on till the first level call.                     **
**                                                                    **
**                  Only one character at a time is considered,except **
**                  when the character is an asterisk.You'll get the  **
**                  logic as the program unfolds.                     **
**                                                                    **
**                                                                    **
**   This program is dedicated to the Public Domain.                  **
**                                                                    **
***********************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

int patimat(char *raw,char *pat)
{
   char *upraw,*uppat;
   int i;

   upraw=strUpper(strdup(raw));
   uppat=strUpper(strdup(pat));
   i=patmat(upraw,uppat);
   free(upraw);
   free(uppat);

   return(i);
}
