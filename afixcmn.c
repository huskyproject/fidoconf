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

#include <string.h>
#include <stdlib.h>
#if !(defined (_MSC_VER) && (_MSC_VER >= 1200))
#include <unistd.h>
#endif

#include "fidoconf.h"
#include "xstr.h"
#include "common.h"
#include "log.h"
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

XMSG createXMSG(ps_fidoconfig config, s_message *msg, const s_pktHeader *header,
                dword forceattr, char* tossDir) 
{
    char **outbounds[4];
    XMSG  msgHeader;
    struct tm *date;
    time_t    currentTime;
    union stamp_combo dosdate;
    unsigned int i;
    char *subject=NULL, *newSubj=NULL, *token, *running;
       
    //init outbounds
    outbounds[0] = &tossDir;
    outbounds[1] = &config->protInbound;
    outbounds[2] = &config->inbound;
    outbounds[3] = NULL;

    // clear msgheader
    memset(&msgHeader, '\0', sizeof(XMSG));
	
    // attributes of netmail must be fixed
    msgHeader.attr = msg->attributes;
	
    if (msg->netMail == 1) {
	// Check if we must remap
	for (i=0;i<config->remapCount;i++)
	    if ((config->remaps[i].toname==NULL ||
		 stricmp(config->remaps[i].toname,msg->toUserName)==0) &&
		(config->remaps[i].oldaddr.zone==0 ||
		 addrComp(config->remaps[i].oldaddr,msg->destAddr)==0) ) 
		{
		    msg->destAddr.zone=config->remaps[i].newaddr.zone;
		    msg->destAddr.net=config->remaps[i].newaddr.net;
		    msg->destAddr.node=config->remaps[i].newaddr.node;
		    msg->destAddr.point=config->remaps[i].newaddr.point;
		    break;
		}
		
	//if (to_us(msg->destAddr)==0) {
    if (isOurAka(config,msg->destAddr)) {
	    // kill these flags
	    msgHeader.attr &= ~(MSGREAD | MSGKILL | MSGFRQ | MSGSCANNED | MSGLOCKED | MSGFWD);
	    // set this flags
	    msgHeader.attr |= MSGPRIVATE;
	} else
	    if (header!=NULL) {
		// set TRS flag, if the mail is not to us(default)
		if ( config->keepTrsMail ) msgHeader.attr &= ~(MSGKILL | MSGFWD);
		else msgHeader.attr |= MSGFWD;
	    }
    } else
	// kill these flags on echomail messages
	msgHeader.attr &= ~(MSGREAD | MSGKILL | MSGFRQ | MSGSCANNED | MSGLOCKED);
   
    // always kill crash, hold, sent & local flags on netmail & echomail
    msgHeader.attr &= ~(MSGCRASH | MSGHOLD | MSGSENT | MSGLOCAL);

    /* FORCED ATTRIBUTES !!! */
    msgHeader.attr |= forceattr;
   
    strcpy((char *) msgHeader.from,msg->fromUserName);
    strcpy((char *) msgHeader.to, msg->toUserName);

    if (((msgHeader.attr & MSGFILE) == MSGFILE) 
	&& (msg->netMail==1)
	&& !strchr(msg->subjectLine, PATH_DELIM)) {

	running = msg->subjectLine;
	token = strseparate(&running, " \t");

	while (token != NULL) {
	    for (i=0;i<4;i++) {
		nfree(subject);
		if (outbounds[i] && *outbounds[i]) xstrcat(&subject, *outbounds[i]);
		xstrcat (&subject, token);
		if (fexist(subject)) break;
#if defined(__linux__) || defined(UNIX)
		subject = strLower(subject);
		if (fexist(subject)) break;
#endif
	    }
	    if (newSubj) xstrcat(&newSubj, " ");
	    xstrcat (&newSubj, subject);
	    token = strseparate(&running, " \t");
	} // end while
	nfree(subject);
    }

    if (newSubj) {
	if (strlen(newSubj) < XMSG_SUBJ_SIZE)
	    strcpy((char *) msgHeader.subj, newSubj);
	else {
	    strncpy((char *) msgHeader.subj, newSubj, XMSG_SUBJ_SIZE-1);
	    w_log('9',
		  "Long subjectLine! Some files will be not routed.");
	}
	nfree(newSubj);
    } else strcpy((char *) msgHeader.subj, msg->subjectLine);

    msgHeader.orig.zone  = (word) msg->origAddr.zone;
    msgHeader.orig.node  = (word) msg->origAddr.node;
    msgHeader.orig.net   = (word) msg->origAddr.net;
    msgHeader.orig.point = (word) msg->origAddr.point;
    msgHeader.dest.zone  = (word) msg->destAddr.zone;
    msgHeader.dest.node  = (word) msg->destAddr.node;
    msgHeader.dest.net   = (word) msg->destAddr.net;
    msgHeader.dest.point = (word) msg->destAddr.point;

    strcpy((char *) msgHeader.__ftsc_date, (char *)msg->datetime);
    ASCII_Date_To_Binary((char *)msg->datetime, (union stamp_combo *) &(msgHeader.date_written));

    currentTime = time(NULL);
    date = localtime(&currentTime);
    TmDate_to_DosDate(date, &dosdate);
    msgHeader.date_arrived = dosdate.msg_st;

    return msgHeader;
}
