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
 * or visit http://www.gnu.org
 *****************************************************************************
 * $Id$
 */
#ifndef	_AFIXCMN_FLAG_
#define	_AFIXCMN_FLAG_

#include "fidoconf.h"

FCONF_EXT   char* createKludges    (int disableTID, const char *area,
                                    const s_addr *ourAka, 
                                    const s_addr *destAka,
                                    const char* versionStr);

FCONF_EXT   s_message* makeMessage (s_addr *origAddr, s_addr *destAddr,
			                        char *fromName, char *toName, 
                                    char *subject,
                                    int netmail, int  killreport);

FCONF_EXT   void       freeMsgBuffers(s_message *msg);

FCONF_EXT   XMSG createXMSG        (ps_fidoconfig config, 
                                    s_message *msg, const s_pktHeader *header,
                                    dword forceattr, char* tossDir);

FCONF_EXT   int Changepause(char *confName, s_link *link, int opt, int type);

FCONF_EXT   int DelLinkFromString(char *line, s_addr linkAddr);

FCONF_EXT   int testAddr(char *addr, s_addr hisAka);

FCONF_EXT   int IsAreaAvailable(char *areaName, char *fileName, char **desc, int retd);

#endif 
