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
 *
 * See also http://www.gnu.org
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


/* CRC32 initial value */
#define CRC32INIT ((UINT32)0xFFFFFFFFUL)
/* CRC16 initial value */
#define CRC16INIT ((UINT16)0)


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
FCONF_EXT   UINT32 memcrc32(const char *str, int size, UINT32 initcrc);

/* Alias for memcrc32() */
#define crc32(x,y,z) memcrc32(x,y,z)

/* Calculate CRC32 for ASCIIZ string
   str: string
   initcrc: initial value (start from 0xFFFFFFFFUL)
 */
FCONF_EXT   UINT32 strcrc32(const char *str, UINT32 initcrc);

/* Calculate CRC32 for file
   filename: file name for calculate CRC32
 */
FCONF_EXT   UINT32 filecrc32(const char *filename);

/* Calculate CRC16 for memory array
   str: array
   size: array size
   initcrc: initial value (start from 0x0000)
 */
FCONF_EXT   UINT16 memcrc16(const char *str, int size, UINT16 initcrc);

/* Alias for memcrc16() */
#define crc16(x,y,z) memcrc32(x,y,z)

/* Calculate CRC16 for ASCIIZ string
   str: string
   initcrc: initial value (start from 0x0000)
 */
FCONF_EXT   UINT16 strcrc16(const char *str, UINT16 initcrc);

/* Calculate CRC16 for file
   filename: file name for calculate CRC16
 */
FCONF_EXT   UINT16 filecrc16(const char *filename);


/*=======================================================================
 * Calculating 16-bit checksum, rotating right before each addition;
 * overflow is discarded.
 * (This algorithm is the algorithm used by historic BSD UNIX systems as
 * the `sum` algorithm and by historic AT&T System V UNIX systems
 * as the `sum -r` algorithm.)
 *=======================================================================
 */

/* 16-bit checksum (sum -r) for ASCIIZ string */
FCONF_EXT UINT16 strsum16( const char *str );
#define strsumr strsum16

/* 16-bit checksum (sum -r) for array of bytes */
FCONF_EXT UINT16 memsum16( const char *str, unsigned size );
#define memsumr memsum16

/* 16-bit checksum (sum -r) for file */
FCONF_EXT UINT16 filesum16(const char *filename);
#define filesumr(fn) filesum16((fn),NULL)


/*=======================================================================
 * Calculating 32-bit checksum described in Draft 8 POSIX 1003.2
 * (This algorithm is the algorithm used by historic AT&T System V UNIX
 * systems as the `sum` algorithm.)
 *=======================================================================
 */

/* 32bit checksum for ASCIIZ string */
FCONF_EXT UINT32 strsum32( const char *str );

/* 32bit checksum for array of bytes */
FCONF_EXT UINT32 memsum32( const char *str, unsigned size );

/* 32bit checksum for file
 * plen: pointer to return file lenght, unuse if plen is NULL
 */
FCONF_EXT UINT32 filesum32( const char *filename, unsigned long *plen );


#ifdef __cplusplus
}
#endif

#endif
