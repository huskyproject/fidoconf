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

/*
 *  dirent.h    Defines the types and structures used by the directory routines
 */
#ifndef DIR_H
/* all other include their own dirent.h */

#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <smapi/compiler.h>

#ifdef HAS_DIR_H
#include <dir.h>
#endif

#ifdef HAS_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAS_DIRECT_H
#include <direct.h>
#endif

#include "fidoconf.h"

#if defined (__MSVC_)
#define NAME_MAX        _MAX_PATH


typedef struct dirent {
   /*  char        d_dta[ 21 ]; */           /* disk transfer area */
    char        d_attr;                 /* file's attribute */
   /*  unsigned short int d_time;     */     /* file's time */
   /*  unsigned short int d_date;     */     /* file's date */
    long        d_size;                 /* file's size */
    char        d_name[_MAX_PATH+1];  /* file's name */
   /*  unsigned short d_ino;          */     /* serial number (not used) */
   /*  char        d_first;           */     /* flag for 1st time */

   struct _finddata_t _dt;
   char               _mask[_MAX_PATH+1];
   long               _handle;
   char               _first_time;
} DIR;

FCONF_EXT DIR* opendir(const char*);
FCONF_EXT DIR* readdir(DIR*);
FCONF_EXT int  closedir(DIR*);
#endif

#if defined(__IBMC__) && !defined(__UNIX__)   /* only define it for IBM VisualAge C++ */
#define DIR_H

#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#include <os2.h>

#define NAME_MAX        255             /* maximum filename */

typedef struct dirent {
    char        d_attr;                 /* file's attribute */
/*   NOT IMPLEMENTED!!!! */
/*     unsigned short int d_time; */         /* file's time */
/*     unsigned short int d_date; */         /* file's date */
    long        d_size;                 /* file's size */
    char        d_name[ NAME_MAX + 1 ]; /* file's name */
    HDIR        d_hdir;                 /* save OS/2 hdir */
    char        d_first;                /* flag for 1st time */
} DIR;

/* File attribute constants for d_attr field */

#define _A_NORMAL       0x00    /* Normal file - read/write permitted */
#define _A_RDONLY       0x01    /* Read-only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_VOLID        0x08    /* Volume-ID entry */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */

extern int      closedir( DIR * );
extern DIR      *opendir( const char * );
extern struct dirent *readdir( DIR * );

#endif
#endif
