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

/* source to implement dir.h for ibm VisualAge C++
*/
#ifdef __IBMC__
#include "dir.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

DIR      *opendir( const char * dirName)
{
   DIR *temp;
   FILEFINDBUF3 findBuffer;
   ULONG findCount;
   char path[255];

   findCount = 1;
   temp = (DIR *) malloc(sizeof(DIR));
   temp->d_hdir = HDIR_SYSTEM;
   temp->d_first = 1;

   // make sure: <path>\*
   strcpy(path, dirName);
   if (dirName[strlen(dirName)-1] != '\\') strcat(path, "\\");
   strcat(path, "*");

   if (NO_ERROR != DosFindFirst(path, &(temp->d_hdir), FILE_NORMAL, &findBuffer, sizeof(findBuffer), &findCount ,FIL_STANDARD)) {
      free(temp);
      return NULL;
   }

   // fill struct
   temp->d_attr = findBuffer.attrFile;
   strcpy(temp->d_name,findBuffer.achName);
   temp->d_size = findBuffer.cbFile;
//   temp->d_date = findBuffer.fdateLastWrite;
//   temp->d_time = findBuffer.ftimeLastWrite;

   return temp;
}

struct dirent *readdir( DIR * dir)
{
   APIRET rc;
   FILEFINDBUF3 findBuffer;
   ULONG findCount = 1;

   if (1 == dir->d_first) {
      dir->d_first = 0;         // if d_first == 1 then the struct is already filled from DosFindFirst
   } else {
      rc = DosFindNext(dir->d_hdir, &findBuffer, sizeof(findBuffer), &findCount);
      if (rc != NO_ERROR) return NULL;

      // fill struct
      dir->d_attr = findBuffer.attrFile;
      strcpy(dir->d_name,findBuffer.achName);
      dir->d_size = findBuffer.cbFile;
//      dir->d_date = findBuffer.fdateLastWrite;
//      dir->d_time = findBuffer.ftimeLastWrite;
   } /* endif */

   return dir;
}

int      closedir( DIR * dir)
{
   APIRET rc;

   rc = DosFindClose(dir->d_hdir);
   free (dir);
   if (rc == NO_ERROR) return 0;
   else return (-1);
}
#endif

