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
 * or visit http://www.gnu.org
 *****************************************************************************
*/

#include <errno.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>

#include "xstr.h"
#include "log.h"
#include "common.h"
#include "compiler.h"
#include "temp.h"


#ifndef PATH_DELIM
#if defined(SASC) || defined(UNIX)
#define PATH_DELIM  '/'
#else
#define PATH_DELIM  '\\'
#endif
#endif

/* If mkstemp() not implemented, use this function.
 *
 */
int MKSTEMP( char *tempfilename )
{  char * ttt;
   int fd=-1;

   ttt = sstrdup(tempfilename);
   while( fd==-1 && mktemp( ttt ) ){
     fd = open( ttt, O_EXCL | O_CREAT );
   };
   if(fd!=-1) strcpy(tempfilename,ttt);
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

  if( !path || !path[0] ){
    w_log(LL_ERR, "temp::createTempFileIn(): empty directory pathname!");
    return NULL;   
  }
  w_log( LL_FUNC, "createtempfileIn() start" );

  xstrcat( &tempfilename, path );

  w_log( LL_DIR,"Try to create temp. directory '%s'", tempfilename );
#if defined(__UNIX__)
  if( !mkdir(tempfilename,TempDIRMODE) )
#else
  if( !MKDIR(tempfilename) )
#endif
    w_log( LL_INFO, "Temp. directory created: (%s)", tempfilename );
  else if( errno==EEXIST )
    w_log( LL_DIR,"Temp. directory alredy exist (%s)", tempfilename );
  else
  { w_log( LL_ERR, "Can't create temp. directory '%s': %s",
                                           tempfilename, strerror(errno) );
    return NULL;
  }

  xstrscat( &tempfilename, "%cXXXXXXXX.%s", PATH_DELIM, ext);
  w_log(LL_FILENAME, "Temp. file mask: %s", tempfilename);

  if( (tempfh = MKSTEMP( tempfilename )) == -1 )
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
