/* $Id$ */
/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998-2002
 *
 * Husky Delopment Team
 *
 * Internet: http://husky.sourceforge.net
 *
 * This file is part of FIDOCONFIG.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library/Lesser General Public
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
 *
 * See also http://www.gnu.org
 *****************************************************************************
 */

#include <smapi/compiler.h>
#include "common.h"
#include "xstr.h"

#define __VERSION__C__

#include "version.h"

extern char cvs_date[]; /* defined in program/cvs_date.h or program/h/cvs_date.h */

/* Generate version string like
 * programname/platform[-compiler] <major>.<minor>.<patchlevel>-<branch> [<cvs date>]
 *
 * Examples:
 * "program/w32-MVC 1.2.3-release"
 * "program/DPMI-DJGPP 1.2.3-stable 01-10-2002"
 * "program/FreeBSD 1.3.0-current 01-10-2002"
 * Return malloc'ed pointer
 */
FCONF_EXT char *GenVersionStr( const char *name, unsigned major, unsigned minor,
                               unsigned patchlevel, unsigned branch )
{
  char *_version_str=NULL; /* compose to this var */
  char *platform;
  char *cbranch=NULL;      /* branch string */

#ifdef UNAME                          /* Use '-DUNAME' compiler parameter */
   platform = "/" UNAME;

#elif defined(__linux__)              /*  gcc on Linux                    */
   platform = "/lnx";

#elif defined(__FreeBSD__)            /*  gcc on FreeBSD                  */
   platform = "/freebsd";

#elif defined(__NetBSD__)             /*  gcc on NetBSD                   */
   platform = "/netbsd";

#elif defined(__OpenBSD__)            /*  gcc on OpenBSD                  */
   platform = "/openbsd";

#elif defined(__sun__)                /*  SunOS (Solaris)                 */
#  if defined(__GNUC__)
   platform = "/sun-gcc";
#  else
   platform = "/sun";
#  endif

#elif defined(__APPLE__) && defined(__MACH__)
   platform = "/mac";

#elif defined(_AIX)
   platform = "/aix";

#elif defined(__osf__)
   platform = "/osf";

#elif defined(__hpux)
   platform = "/hpux";

#elif defined(__OS2__) || defined(OS2)
#  if defined(__TURBOC__) /* Borland C/C++ for OS/2 */
   platform = "/os2-bc";
#  elif defined(_MSC_VER) /* Microsoft C or Microsoft QuickC for OS/2 */
   platform = "/os2-msc";
#  elif defined(__WATCOMC__)
   platform = "/os2-wc";
#  elif defined(__IBMC__) /* IBM C/Set++ for OS/2 */
   platform = "/os2-ibmc";
#  elif defined(__HIGHC__)/* MetaWare High C/C++ for OS/2 */
   platform = "/os2-hc";
#  elif defined(__EMX__)  /* EMX for 32-bit OS/2 */
   platform = "/os2-emx";
#  else
   platform = "/os2";
#  endif

#elif defined(__HIGHC__) /* MetaWare High C/C++ for OS/2 */
   platform = "/os2-hc";

#elif defined(__IBMC__) && !defined(UNIX)
/* IBM C/Set++ for OS/2 */
   platform = "/os2-ibmc";

#elif defined(__NT__)
#  if defined(_MSC_VER) && (_MSC_VER >= 1200)
#    if defined(_MAKE_DLL_MVC_)
     platform = "/w32-mvcdll";
#    else
     platform = "/w32-mvc";
#    endif
#  elif defined(__MINGW32__)
   platform = "/w32-mgw";
#  elif defined(__TURBOC__) /* Borland C/C++ for Win32 */
   platform = "/w32-bc";
#  elif defined(__WATCOMC__)
   platform = "/w32-wc";
#  elif defined(__EMX__)    /* RSX for Windows NT */
   platform = "/w32-rsx";
#  else
   platform = "/w32";
#  endif

#elif defined(_WINDOWS)
#  if defined(__WATCOMC__)
   platform = "/win-wc";
#  else
   platform = "/win";
#  endif

#  elif defined(__EMX__)    /* EMX for 32-bit OS/2 or RSX for Windows NT */
   platform = "/emx-rsx";


#elif defined(MSDOS) ||  defined(DOS) || defined(__DOS__) || defined(__MSDOS__)
#  ifdef __DJGPP__
   platform = "/dpmi-djgpp";
#  elif defined(__WATCOMC__) && defined(__FLAT__)
   platform = "/dpmi-wc";
#  elif defined(__WATCOMC__) && !defined(__FLAT__)
   platform = "/dos-wc";
#  elif defined(__TURBOC__)
   platform = "/dos-bc";
#  elif defined(_MSC_VER) /* Microsoft C or Microsoft QuickC for MS-DOS */
   platform = "/dos-msc";
#  elif defined(__FLAT__)
   platform = "/dpmi";
#  else
   platform = "/dos";
#  endif

#elif defined(__BEOS__)
   platform = "/beos";

#elif defined(SASC)                          /* SAS C for AmigaDOS */
   platform = "/amiga-sasc";

#elif defined(UNIX)
   platform = "/unix";

#else
   platform = "";
# warning Unknown platform and compiler!
#endif


  switch(branch){
  case BRANCH_CURRENT: cbranch = "-current";
                       if( !(minor & 1) ){
                         fprintf(stderr, __FILE__ ":%u: illegal usage of GenVersionStr(): minor value for current branch must be odd!\n", __LINE__);
                       }
                       if( patchlevel ){
                         fprintf(stderr, __FILE__ ":%u: illegal usage of GenVersionStr(): patchlevel value for current branch must be zero!\n", __LINE__);
/*                         pathlevel = 0;*/
                       }
                       break;
  case BRANCH_STABLE:  cbranch = "-stable";
                       if( minor & 1 ){
                         fprintf(stderr, __FILE__ ":%u: illegal usage of GenVersionStr(): minor value for stable branch must be even!\n", __LINE__);
                       }
                       break;
  case BRANCH_RELEASE: cbranch = "-release";
                       if( minor & 1 ){
                         fprintf(stderr, __FILE__ ":%u: illegal usage of GenVersionStr(): minor value for release branch must be even!\n", __LINE__);
                       }
  }

  xscatprintf( &_version_str, "%s%s %u.%u.%u%s %s",
                             name, platform, major, minor, patchlevel, cbranch,
  /* Release date are known always */ branch==BRANCH_RELEASE ? "" : cvs_date );

  return _version_str;
}


#ifdef TEST_VERSION_C

#include <stdio.h>
#include <stdlib.h>
#include "cvsdate.h"

int main(){
  char *versionStr;

  versionStr = GenVersionStr( "fidoconfig", 1, 3, 0, BRANCH_RELEASE);
  printf( "RELEASE: %s\n\n", versionStr );
  nfree(versionStr);

  versionStr = GenVersionStr( "fidoconfig", 1, 2, 1, BRANCH_RELEASE);
  printf( "RELEASE: %s\n\n", versionStr );
  nfree(versionStr);

  versionStr = GenVersionStr( "fidoconfig", 1, 2, 6, BRANCH_CURRENT);
  printf( "CURRENT: %s\n\n", versionStr );
  nfree(versionStr);

  versionStr = GenVersionStr( "fidoconfig", 1, 3, 0, BRANCH_CURRENT);
  printf( "CURRENT: %s\n\n", versionStr );
  nfree(versionStr);

  versionStr = GenVersionStr( "fidoconfig", 1, 3, 4, BRANCH_STABLE);
  printf( "STABLE: %s\n\n", versionStr );
  nfree(versionStr);

  versionStr = GenVersionStr( "fidoconfig", 1, 4, 0, BRANCH_STABLE);
  printf( "STABLE: %s\n\n", versionStr );
  nfree(versionStr);

  return 0;
}
#endif
