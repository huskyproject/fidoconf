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

#ifndef __IBMC__     /* all other include their own dirent.h */
#ifndef __WATCOMC__
#if defined(__EMX__) || defined(__FreeBSD__)
#include <sys/types.h>
#endif
#include <dirent.h>
#endif
#endif

#ifdef __WATCOMC__
#include <direct.h>  /* watcom knows this as direct.h */
#endif

#ifdef __IBMC__   /* only define it for IBM VisualAge C++ */
#define DIR_H

#include <direct.h>   /* include the other things out of direct.h */
#ifdef OS_2
#ifdef EXPENTRY
#undef EXPENTRY
#endif
#endif
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#include <os2.h>

#define NAME_MAX        255             /* maximum filename */

typedef struct dirent {
    char        d_attr;                 /* file's attribute */
//  NOT IMPLEMENTED!!!!
//    unsigned short int d_time;          /* file's time */
//    unsigned short int d_date;          /* file's date */
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
