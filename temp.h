/* $Id$
 ******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * tmp.h : tmp files & directories operating functions declarations
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

#ifndef __TEMP_H__
#define __TEMP_H__

#include "fidoconf.h"

/* Default temporary files suffix (==extension in DOS-like OS)
 * (re)define it in your program source if want other.
 */
#ifndef TEMPFILESUFFIX
#define TEMPFILESUFFIX "tmp"
#endif


/* Create new file with random name & specified suffix in specified directory.
 * path = temporary directory
 * ext = file name suffix
 * mode = 'w' or 'b' - file open mode (text or binary); default = fopen() default
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */
FCONF_EXT FILE *createTempFileIn(const char *path, const char *ext, char mode, char **name);


/* Create new file with random name & default suffix (tmp) in text mode.
 * pconfig = fidoconfig structure pointer
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */
FCONF_EXT FILE *createTempTextFile(const ps_fidoconfig pconfig, char **name);


/* Create new file with random name & default suffix (tmp) in binary mode.
 * pconfig = fidoconfig structure pointer
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */


FCONF_EXT FILE *createTempBinFile(const ps_fidoconfig pconfig, char **name);
/* Create new file with random name & default suffix (binary mode).
 * pconfig = fidoconfig structure pointer
 * Place to 'name' variable name of created file (from malloc() memory pool),
 * if name is not NULL its free().
 * Return file descriptor or NULL
 */
#define createTempFile(pconfig,name) (createTempBinFile(pconfig,name))

#endif
