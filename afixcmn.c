/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * afixcmn.c : common areafix functions
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

 /*
 * This subroutine makes up an ascending unique ^aMSGID stamp
 * taken from Su.FidoTech.FAQ [2/3]
 */

#include <time.h>
#include <stdlib.h>
#if !(defined (_MSC_VER) && (_MSC_VER >= 1200))
#include <unistd.h>
#endif
//#include "typesize.h"
#include "fidoconf.h"
#include "xstr.h"
#include "common.h"
#include "afixcmd.h"
#include <smapi/progprot.h>

static ULONG DoMakeMSGIDStamp(void)
{
    static ULONG lStampPrev;
    ULONG lStamp, lSecs, lHund, lSecStart = (ULONG) time(NULL);
#ifdef __OS2__
    static BOOL fInfoSeg = FALSE;
    static PGINFOSEG pgis;
    static PLINFOSEG plis;
    SEL selgis, sellis;
#endif
    
    // Under OS2 get pointers to the global and local info segments once
    
#ifdef __OS2__
    if (!fInfoSeg) {
        DosGetInfoSeg(&selgis, &sellis);
        pgis = MAKEPGINFOSEG(selgis);
        plis = MAKEPLINFOSEG(sellis);
        fInfoSeg = TRUE;
    }
#endif
    
    // Make up time stamp out of number of seconds since Jan 1, 1970
    // shifted 7 bits to the left OR'ed with current system clock and
    // loop untill we get a new stamp
    
    do {
#ifdef __OS2__
        lSecs = (ULONG) pgis->time;
        lHund = (ULONG) pgis->hundredths;
        DosSleep(0);
#else
        lSecs = (ULONG) time(NULL);
        lHund = (ULONG) clock();
#endif
        lStamp = (lSecs << 7) | (lHund & 0x07f);
    } while ((lStampPrev >= lStamp) && ((ULONG) time(NULL) < lSecStart + 5));
    
    // Check if we finally have unique ascending ^aMSGID kludge stamp and
    // if not, use incremented largest stamp value
    
    if (lStampPrev >= lStamp) lStamp = lStampPrev + 1;
    
    return lStampPrev = lStamp;
}

char *createKludges(int disableTID, const char *area, const s_addr *ourAka, 
                    const s_addr *destAka, const char* versionStr)
{
   char *buff = NULL;
   ULONG msgid = 0;

   if (area) xscatprintf(&buff, "AREA:%s\r", area);
   else {
	   xscatprintf(&buff, "\001INTL %u:%u/%u %u:%u/%u\r",
			   destAka->zone, destAka->net, destAka->node,
			   ourAka->zone,  ourAka->net,  ourAka->node);
      if (ourAka->point) xscatprintf(&buff, "\001FMPT %d\r", ourAka->point);
      if (destAka->point) xscatprintf(&buff, "\001TOPT %d\r", destAka->point);
   }
   sleep(1);           // will be removed
   msgid = time(NULL); // for //msgid = DoMakeMSGIDStamp();
  
   if (ourAka->point)
      xscatprintf(&buff, "\001MSGID: %u:%u/%u.%u %08lx\r",
              ourAka->zone,ourAka->net,ourAka->node,ourAka->point,msgid);
   else
      xscatprintf(&buff, "\001MSGID: %u:%u/%u %08lx\r",
              ourAka->zone,ourAka->net,ourAka->node,msgid);

   if (!disableTID) xscatprintf(&buff, "\001TID: %s\r", versionStr);

   return buff;
}

s_message *makeMessage (s_addr *origAddr, s_addr *destAddr,
			char *fromName,	char *toName, char *subject, 
            int netmail, int  killreport)
{
    // netmail == 0 - echomail
    // netmail == 1 - netmail
    time_t time_cur;
    s_message *msg;

    if (toName == NULL) toName = "sysop";
    
    time_cur = time(NULL);
    
    msg = (s_message*) scalloc(1,sizeof(s_message));
    
    msg->origAddr.zone = origAddr->zone;
    msg->origAddr.net = origAddr->net;
    msg->origAddr.node = origAddr->node;
    msg->origAddr.point = origAddr->point;

    msg->destAddr.zone = destAddr->zone;
    msg->destAddr.net = destAddr->net;
    msg->destAddr.node = destAddr->node;
    msg->destAddr.point = destAddr->point;
	
    xstrcat(&(msg->fromUserName), fromName);
    xstrcat(&(msg->toUserName), toName);
    xstrcat(&(msg->subjectLine), subject);

    msg->attributes |= MSGLOCAL;
    if (netmail) {
	msg->attributes |= MSGPRIVATE;
	msg->netMail = 1;
    }
    if (killreport) msg->attributes |= MSGKILL;

    fts_time((char*)msg->datetime, localtime(&time_cur));

    return msg;
}

