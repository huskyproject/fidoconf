/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * crc.h : crc calculation routines (declarations)
 *
 * Collected by Stas Degteff <g@grumbler.org> 2:5080/102
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
 * $Id$
 */

#ifndef _CRC_H
#define _CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fidoconf.h"
#include "typesize.h"

/* Define read() buffer */
#if OSTYPE==UNIX
#define CRC_BUFFER_SIZE 80000
#else
/* DOS-like OS (64K memory segment) */
#define CRC_BUFFER_SIZE 32767
#endif

/* Calculate CRC32 for memory array
   str: array
   size: array size
   initcrc: initial value (start from 0xFFFFFFFFUL)
 */
FCONF_EXT   UINT32 memcrc32(char *str, int size, UINT32 initcrc);

/* Alias for memcrc32() */
#define crc32(x,y,z) memcrc32(x,y,z)

/* Calculate CRC32 for ASCIIZ string
   str: string
   initcrc: initial value (start from 0xFFFFFFFFUL)
 */
FCONF_EXT   UINT32 strcrc32(char *str, UINT32 initcrc);

/* Calculate CRC32 for file
   filename: file name for calculate CRC32
 */
FCONF_EXT   UINT32 filecrc32(const char *filename);

/* Calculate CRC16 for memory array
   str: array
   size: array size
   initcrc: initial value (start from 0x0000)
 */
FCONF_EXT   UINT16 memcrc16(char *str, int size, UINT16 initcrc);

/* Alias for memcrc16() */
#define crc16(x,y,z) memcrc32(x,y,z)

/* Calculate CRC16 for ASCIIZ string
   str: string
   initcrc: initial value (start from 0x0000)
 */
FCONF_EXT   UINT16 strcrc16(char *str, UINT16 initcrc);

/* Calculate CRC16 for file
   filename: file name for calculate CRC16
 */
FCONF_EXT   UINT16 filecrc16(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
