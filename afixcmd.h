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

#include "fidoconf.h"

/*--- afixcmn.c ---*/

/* Return string contents message kludges: AREA, @INTL, FMPT, TOPT, MSGID, TID */
FCONF_EXT   char* createKludges    (ps_fidoconfig config, const char *area,
                                    const hs_addr *ourAka,
                                    const hs_addr *destAka,
                                    const char* versionStr);

/* Compose message into structure s_message & return it */
FCONF_EXT   s_message* makeMessage (hs_addr *origAddr, hs_addr *destAddr,
			                        char *fromName, char *toName,
                                    char *subject,
                                    int netmail, long attrs);

/* Free memory allocated for s_message structure */
FCONF_EXT   void       freeMsgBuffers(s_message *msg);

/* Compose XMSG structure (used by smapi) */
FCONF_EXT   XMSG createXMSG        (ps_fidoconfig config,
                                    s_message *msg, const s_pktHeader *header,
                                    dword forceattr, char* tossDir);

/*--- afixcmd.c ---*/

/* find token of "ftoken" position for link "link"
    IN : token,link
    OUT: confName,start,end
*/
FCONF_EXT   int FindTokenPos4Link(char **confName, char* ftoken, s_link *link, long* start, long*end);

FCONF_EXT   int InsertCfgLine(char *confName, char* cfgLine, long start, long end);

/* Change pause status (off|echo|feacho|on) */
FCONF_EXT   int Changepause(char *confName, s_link *link, int opt, int type);

/* Remove link address from area string */
FCONF_EXT   int DelLinkFromString(char *line, hs_addr linkAddr);

FCONF_EXT   int testAddr(char *addr, hs_addr hisAka);

FCONF_EXT   int IsAreaAvailable(char *areaName, char *fileName, char **desc, int retd);

FCONF_EXT   void RemoveLink(s_link*, s_area*);

FCONF_EXT   void Addlink(const s_fidoconfig*, s_link*, s_area*);

#endif
