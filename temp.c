/* $Id$
 ******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * tmp.c : tmp files & directories operating functions
 *
 * (c) Stas Degteff <g@grumbler.org>, 2:5080/102@fidonet
 *
 * This file is part of FIDOCONFIG library (part of the Husky FIDOnet
 * software project)
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FIDOCONFIG library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FIDOCONFIG library; see the file COPYING.  If not, write
 * to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA
 *
 * See also http://www.gnu.org
 *****************************************************************************
*/

#include <errno.h>
#include <string.h>
#if defined(__TURBOC__) || (defined (_MSC_VER) && (_MSC_VER >= 1200))
#  include <io.h>
#else
#  include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "common.h"
#include "xstr.h"
#include "log.h"
#include "temp.h"

#if defined( __WATCOMC__ ) && ( __WATCOMC__ > 1000 )
/* Watcom C 11.0 */
# define mktemp _mktemp
#endif

/* This includes commented for not create dependence from smapi
   please don't use _createDirectoryTree()
 */
/*#include <smapi/compiler.h>*/
/*#include <smapi/progprot.h>*/   /* for _createDirectoryTree() */
/*#define MKDIR(dd) (_createDirectoryTree(dd))*/


/* If mkstemps() not implemented, use this function.
 * parameters example:
 *   tempfilename = "/path/prefixXXXXX.suffix"
 *   tempfilename = "/path/prefixXXXXX"
 *  "XXXXX" replaced with random char sequence
 */
int MKSTEMPS( char *tempfilename )
{  char *ttt;
   int fd=-1;
   char *pp;

   ttt = sstrdup(tempfilename);
   pp = strrchr(ttt, '.');
   if( ttt[strlen(ttt)-1]!='X' && pp && (pp>ttt && *(pp-1)=='X') ){ /* suffix presents */
     do{
         *pp = 0;
         if( !mktemp(ttt) )
           break;
         *pp = '.';
         fd = open( ttt, O_EXCL | O_CREAT | O_RDWR, S_IREAD | S_IWRITE );
     }while( fd==-1 && errno == EEXIST );
   }else{
     do{
         if( !mktemp(ttt) )
           break;
         fd = open( ttt, O_EXCL | O_CREAT | O_RDWR, S_IREAD | S_IWRITE );
     }while( fd==-1 && errno == EEXIST );
   }
   if(fd!=-1) strcpy(tempfilename,ttt);
   nfree(ttt);
   return fd;
}

/* Create new file with random name & specified suffix in specified directory.
 * path = temporary directory
 * ext = file name suffix
 * mode = 'w' or 'b' - file open mode (text or binary); default = fopen() default
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */

FILE *createTempFileIn(const char *path, const char *ext, char mode, char **name)
{ int tempfh=-1; FILE *tempfd=NULL; char *tempfilename=NULL;
  char *ii=0;

  if( !path || !path[0] ){
    w_log(LL_ERR, "temp::createTempFileIn(): pathname is empty!");
    return NULL;
  }
  w_log( LL_FUNC, "createtempfileIn() start" );

  xstrcat( &tempfilename, (char *)path );
  ii = tempfilename + strlen(tempfilename) -1;
  if( *ii == PATH_DELIM ) *ii=0; /* strip trailing slash */

  xscatprintf( &tempfilename, "%cXXXXXX.%s", PATH_DELIM, ext);

  w_log(LL_FILENAME, "Temp. file mask: %s", tempfilename);

  if( (tempfh = MKSTEMPS( tempfilename )) == -1 )
  { w_log( LL_ERR, "Cannot create temp. file (Mask %s): %s", tempfilename, strerror(errno) );
    w_log( LL_FUNC, "createTempFileIn() rc=NULL" );
    return NULL;
  }
  if( mode == 't' )
     tempfd = fdopen(tempfh,"wt");
  else if( mode == 'b' )
     tempfd = fdopen(tempfh,"wb");
  else
     tempfd = fdopen(tempfh,"w");
  if( !tempfd )
  { w_log( LL_CRIT, "Cannot reopen file '%s': %s", tempfilename, strerror(errno) );
    return NULL;
  }
  w_log( LL_FILE, "Created temp file %s", tempfilename );
  if( name!= NULL )
  { nfree(*name);
   *name = sstrdup(tempfilename);
  }
  w_log( LL_FUNC, "createTempFileIn() OK fd=%p", tempfd );
  return tempfd;
}


/* Create new file with random name & specified suffix (text mode).
 * pconfig = fidoconfig structure pointer
 * ext = file name suffix
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */
FILE *createTempTextFile(const ps_fidoconfig pconfig, char **name)
{ if( pconfig->tempDir )
    return createTempFileIn(pconfig->tempDir, TEMPFILESUFFIX, 't', name);
  else{
    w_log(LL_ERR, "tempDir not defined in config, temp. file can't created");
    return NULL;
  }
}

/* Create new file with random name & specified suffix (binary mode).
 * pconfig = fidoconfig structure pointer
 * ext = file name suffix
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */
FILE *createTempBinFile(const ps_fidoconfig pconfig, char **name)
{ if( pconfig->tempDir )
    return createTempFileIn(pconfig->tempDir, TEMPFILESUFFIX, 'b', name);
  else{
    w_log(LL_ERR, "tempDir not defined in config, temp. file can't created");
    return NULL;
  }
}


#if 0

#include <string.h>
#include <errno.h>
#include <stdio.h>

/*  Test */
void main()
{ FILE *fd;
  s_fidoconfig *config;
  char *name=NULL;

  config = readConfig("../test.cfg");
  fd = createTempBinFile(config, &name);
  if( !fd )
    printf("error creating %s: %s\n", name, strerror(errno) );
  else
  {  printf("created: %s\n", name);
     fclose (fd);
     unlink(name);
  }
  disposeConfig(config);
}
#endif
