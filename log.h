/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * log.h : log file maintnance routines declarations
 *
 * Compiled from hpt/log & htick/log
 * by Stas Degteff <g@grumbler.org>, 2:5080/102@fidonet
 *
 * Portions copyright (C) Matthias Tichy
 *                        Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 *                        Internet: mtt@tichy.de
 * Portions copyright (C) Max Levenkov
 *                        Fido:     2:5000/117
 *                        Internet: sackett@mail.ru
 * Portions copyright (C) Gabriel Plutzar
 *                        Fido:     2:31/1
 *                        Internet: gabriel@hit.priv.at
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

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include "fidoconf.h"

#define DefaultLogLevels "1234567890"
#define DefaultScreenLogLevels DefaultLogLevels

#define LL_START        '1'      /* Program start */
#define LL_STOP         '1'      /* Program end */
#define LL_DUPE    	'2'      /* Dupecheck */
#define LL_DUPECHECK   LL_DUPE
#define LL_LINKING      '3'      /* Link messagebase */
#define LL_SCANNING 	'4'      /* Scan messagebase */
#define LL_POSTING      '5'      /* Post message */
#define LL_EXEC  	'6'      /* system() & exec() */
#define LL_BUNDLE	'7'      /* Bundle operations */
#define LL_OUTBOUND     '7'      /* Outbound operations */
#define LL_FLO          '7'      /* ?LO file operations */
#define LL_UT           '7'      /* ?UT file operations */
#define LL_PKT          '7'      /* PKT file operations */
#define LL_LINK   	'7'      /* Link operation */
#define LL_LINKBEGIN  LL_LINK    /* Link operations begin */
#define LL_LINKEND    LL_LINK    /* Link operations end */
#define LL_FREQ   	'7'      /* FREQ processing */
#define LL_ROUTE  	'7'      /* Routing */
#define LL_FROUTE  	'7'      /* File routing */
#define LL_ATTACH 	'7'      /* File attach */
#define LL_MSGPACK    	'7'      /* Message packing */
#define LL_AREAFIX  	'8'      /* areafix & filefix operations */
#define LL_RELINK   	'8'      /* send relink message */
#define LL_AUTOCREATE	'8'      /* area auto cleate */
#define LL_CRIT    	'9'      /* Critical error: exit */
#define LL_FLAG         '0'      /* Create/remove/test flag */
#define LL_LINKBUSY  	'0'      /* .BSY exist */

#define LL_ERROR   	'A' /*9*//* Trivial error: continue */
#define LL_ERR     LL_ERROR
#define LL_WARN    	'B'      /* Warning */
#define LL_ALERT   	'B'      /* Alert */
#define LL_INFO    	'C' /*1*//* Information messages */
#define LL_STAT    	'D' /*1*//* Statistics */
#define LL_SUMMARY 	'E' /*1*//* Summary */
#define LL_PRG     	'F'      /* Program name */
#define LL_SENT  	'G'      /* Message sent */
#define LL_ENCODE       'H' /*2*//* Encode file/message */
#define LL_DECODE       'H' /*2*//* Decode file/message */
#define LL_RECODE  	'H' /*2*//* Recoding tables (codepage translations) */

#define LL_MSGID   	'I'      /* Generate/check MSGID */
#define LL_ECHOMAIL 	'J'      /* Echomail phase */
#define LL_FILEBOX 	'K'      /* Filebox phase/operations */
#define LL_NETMAIL 	'L'      /* Netmail phase */
#define LL_CREAT        'M'      /* Create file */
#define LL_DEL     	'N'      /* Delete file */
#define LL_FILE    	'O'      /* Other file operations (read, write, seek, ...) */
#define LL_TRUNC   	'R'      /* Truncate file */
#define LL_DELETE     LL_DEL
#define LL_TRUNCATE   LL_TRUNC
#define LL_FILESENT 	'S'      /* File sent */
#define LL_FILETEST 	'T'      /* Test files (exist, permittions) */
#define LL_FILENAME  	'X'      /* Filenames construct */
#define LL_FUNC         'U'      /* Functions calls */
#define LL_SRCLINE 	'Z'      /* Source lines numbers */
#define LL_DEBUG        'a'      /* Debug output */

#ifdef __cplusplus
extern "C" {
#endif

struct _log {
   char *keysAllowed;    // only log-data with one of these keys will be stored
   char *keysPrinted;    // only log-data with these keys will be printed to screen
   char *appName;        // name of the application which has created this log entry
   FILE *logFile;        // in this logFile
   unsigned char isopen; // is the log-file open?
   unsigned int logEcho; // echo log to screen?
};

typedef struct _log s_log;

FCONF_EXT s_log *openLog(char *fileName, char *appN, s_fidoconfig *config);
/*DOC
  Input:  fileName is a valid name for a file.
          appN contains the name of the application.
  Output: openLog returns a pointer to an s_log struct.
  FZ:     openLog fills the s_log struct, opens the logfile and returns the struct
*/

FCONF_EXT void closeLog();
/*DOC
  Input:  log is a pointer to a s_log
  Output: ./.
  FZ:     closes the logFile and frees all mem use by log.
*/

FCONF_EXT void w_log(char key, char *logString, ...);
/*DOC
  Input:  key is the key under which the log-entry will be stored
          logString is the logEntry
  Output: ./.
  FZ:     if the key is in keysAllowed the logString will be written to the log.
*/

//FCONF_EXT void writeLogEntry(s_log *log, char key, char *logString, ...);

#ifdef __cplusplus
}
#endif

#endif
