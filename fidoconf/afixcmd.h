/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * afixcmd.h : common areafix commands
 *
 * Compiled from hpt/areafix hpt/toss hpt/pkt
 * by Max Chernogor <mihz@mail.ru>, 2:464/108@fidonet
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
#ifndef	_AFIXCMN_FLAG_
#define	_AFIXCMN_FLAG_

/* huskylib: compiler-specific declarations */
#include <huskylib/compiler.h>

/* huskylib: HUSKYEXT declaration */
#include <huskylib/huskyext.h>

/* smapi */
#include <smapi/msgapi.h>

/* fidoconfig */
#include "fidoconf.h"

/*--- afixcmn.c ---*/

/* Return string contents message kludges: AREA, @INTL, FMPT, TOPT, MSGID, TID */
HUSKYEXT   char* createKludges    (ps_fidoconfig config, const char *area,
                                    const hs_addr *ourAka,
                                    const hs_addr *destAka,
                                    const char* versionStr);

/* Compose message into structure s_message & return it */
HUSKYEXT   s_message* makeMessage (hs_addr *origAddr, hs_addr *destAddr,
			                        char *fromName, char *toName,
                                    char *subject,
                                    int netmail, long attrs);

/* Free memory allocated for s_message structure */
HUSKYEXT   void       freeMsgBuffers(s_message *msg);

HUSKYEXT   void cvtAddr(const NETADDR aka1, ps_addr aka2);

/* Compose XMSG structure (used by smapi) */
HUSKYEXT   XMSG createXMSG        (ps_fidoconfig config,
                                    s_message *msg, const s_pktHeader *header,
                                    dword forceattr, char* tossDir);

/*--- afixcmd.c ---*/

/* find token of "ftoken" position for link "link"
    IN : ftoken,link
         fftoken     -- position should be after this token
    OUT: confName,start,end
*/
HUSKYEXT   int FindTokenPos4Link(char **confName, char* ftoken, char *fftoken, s_link *link, long* start, long*end);

HUSKYEXT   int InsertCfgLine(char *confName, char* cfgLine, long start, long end);

/* Change pause status (off|echo|feacho|on) */
HUSKYEXT   int Changepause(char *confName, s_link *link, int opt, int type);

/* Remove link address from area string */
HUSKYEXT   int DelLinkFromString(char *line, hs_addr linkAddr);

HUSKYEXT   int testAddr(char *addr, hs_addr hisAka);

HUSKYEXT   int IsAreaAvailable(char *areaName, char *fileName, char **desc, int retd);

HUSKYEXT   void RemoveLink(s_link*, s_area*);

HUSKYEXT   void Addlink(s_fidoconfig*, s_link*, s_area*);

/* ---------------- areafix checking stuff --------------*/

HUSKYEXT   int mandatoryCheck(s_area*, s_link*);

HUSKYEXT   int manualCheck(s_area*, s_link*);

HUSKYEXT   int subscribeCheck(s_area*, s_link*);

HUSKYEXT   int subscribeAreaCheck(s_area*, char*, s_link*);

HUSKYEXT   int limitCheck(s_link *link);

#endif
