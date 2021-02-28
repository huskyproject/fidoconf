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
 *
 * See also http://www.gnu.org
 *****************************************************************************
 * $Id$
 */
/*
 * This subroutine makes up an ascending unique ^aMSGID stamp
 * taken from Su.FidoTech.FAQ [2/3]
 */

#include <string.h>
#include <stdlib.h>

#include <huskylib/huskylib.h>

#ifdef HAS_UNISTD_H
#   include <unistd.h>
#endif

#ifdef HAS_STRINGS_H
#   include <strings.h>
#endif
/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

#include "fidoconf.h"
#include "common.h"
#include "afixcmd.h"

#if 0
static hUINT32 DoMakeMSGIDStamp(void)
{
    static hUINT32 lStampPrev;
    hUINT32 lStamp, lSecs, lHund, lSecStart = (hUINT32)time(NULL);

    /*  Make up time stamp out of number of seconds since Jan 1, 1970 */
    /*  shifted 7 bits to the left OR'ed with current system clock and */
    /*  loop untill we get a new stamp */
    do
    {
        lSecs  = (hUINT32)time(NULL);
        lHund  = (hUINT32)clock();
        lStamp = (lSecs << 7) | (lHund & 0x07f);
    }
    while((lStampPrev >= lStamp) && ((hUINT32)time(NULL) < lSecStart + 5));

    /*  Check if we finally have unique ascending ^aMSGID kludge stamp and */
    /*  if not, use incremented largest stamp value */
    if(lStampPrev >= lStamp)
    {
        lStamp = lStampPrev + 1;
    }

    return lStampPrev = lStamp;
}

#endif /* if 0 */

char * createKludges(ps_fidoconfig config,
                     const char * area,
                     const hs_addr * ourAka,
                     const hs_addr * destAka,
                     const char * versionStr)
{
    char * buff   = NULL;
    hUINT32 msgid = 0;

    if(area)
    {
        xscatprintf(&buff, "AREA:%s\r", area);
        strUpper(buff);
    }
    else
    {
        xscatprintf(&buff,
                    "\001INTL %u:%u/%u %u:%u/%u\r",
                    destAka->zone,
                    destAka->net,
                    destAka->node,
                    ourAka->zone,
                    ourAka->net,
                    ourAka->node);

        if(ourAka->point)
        {
            xscatprintf(&buff, "\001FMPT %d\r", ourAka->point);
        }

        if(destAka->point)
        {
            xscatprintf(&buff, "\001TOPT %d\r", destAka->point);
        }
    }

    msgid = GenMsgId(config->seqDir, config->seqOutrun);

    if(ourAka->point)
    {
        xscatprintf(&buff,
                    "\001MSGID: %u:%u/%u.%u %08lx\r",
                    ourAka->zone,
                    ourAka->net,
                    ourAka->node,
                    ourAka->point,
                    msgid);
    }
    else
    {
        xscatprintf(&buff,
                    "\001MSGID: %u:%u/%u %08lx\r",
                    ourAka->zone,
                    ourAka->net,
                    ourAka->node,
                    msgid);
    }

    if(!config->disablePID)
    {
        xscatprintf(&buff, "\001PID: %s\r", versionStr);
    }

    return buff;
} /* createKludges */

s_message * makeMessage(hs_addr * origAddr,
                        hs_addr * destAddr,
                        char * fromName,
                        char * toName,
                        char * subject,
                        int netmail,
                        long attrs)
{
    /*  netmail == 0 - echomail */
    /*  netmail == 1 - netmail */
    time_t time_cur;
    s_message * msg;

    if(toName == NULL)
    {
        toName = "sysop";
    }

    time_cur      = time(NULL);
    msg           = (s_message *)scalloc(1, sizeof(s_message));
    msg->origAddr = *origAddr;
    msg->destAddr = *destAddr;
    xstrcat(&(msg->fromUserName), fromName);
    xstrcat(&(msg->toUserName), toName);
    xstrcat(&(msg->subjectLine), subject);
    msg->attributes = attrs;

    if(!netmail)
    {
        msg->attributes &= ~(MSGPRIVATE | MSGKILL);
    }
    else
    {
        msg->netMail = 1;
    }

    fts_time((char *)msg->datetime, localtime(&time_cur));
    return msg;
} /* makeMessage */

XMSG createXMSG(ps_fidoconfig config,
                s_message * msg,
                const s_pktHeader * header,
                dword forceattr,
                char * tossDir)
{
    char ** outbounds[4];
    XMSG msgHeader;
    struct tm * date;
    time_t currentTime;
    union stamp_combo dosdate;
    unsigned int i;
    char * newSubj = NULL, * token = NULL, * running = NULL, * p = NULL;

    /* init outbounds */
    outbounds[0] = &tossDir;
    outbounds[1] = &config->protInbound;
    outbounds[2] = &config->inbound;
    outbounds[3] = NULL;
    /*  clear msgheader */
    memset(&msgHeader, '\0', sizeof(XMSG));
    /*  attributes of netmail must be fixed */
    msgHeader.attr = msg->attributes;

    if(msg->netMail == 1)
    {
        /*  Check if we must remap */
        for(i = 0; i < config->remapCount; i++)
        {
            if((config->remaps[i].toname == NULL ||
                stricmp(config->remaps[i].toname,
                        msg->toUserName) == 0) &&
               (config->remaps[i].oldaddr.zone == 0 ||
                addrComp(config->remaps[i].oldaddr, msg->destAddr) == 0))
            {
                w_log(LL_NETMAIL,
                      "Remap destination %u:%u/%u.%u to %s",
                      msg->destAddr.zone,
                      msg->destAddr.net,
                      msg->destAddr.node,
                      msg->destAddr.point,
                      aka2str(config->remaps[i].newaddr));
                msg->destAddr = config->remaps[i].newaddr;

                /* synchronize 'INTL' kludge with new dest address */
                for(running = msg->text; (p = strchr(running, '\r')) != NULL; running = ++p)
                {
                    *p = '\0';

                    if(strlen(running) > 5 && !memcmp(running, "\001INTL ", 6))
                    {
                        /*replace INTL to new*/
                        xstrscat(&token, "\001INTL ", aka2str(msg->destAddr), NULLP);
                        xscatprintf(&token, " %s\r", aka2str(msg->origAddr));
                    }
                    else       /* copy kludge or line */
                    {
                        xscatprintf(&token, "%s\r", running);
                    }
                }
                xscatprintf(&token, "\001Replace destaddr %s", aka2str(config->remaps[i].oldaddr));
                xscatprintf(&token, " with %s", aka2str(msg->destAddr));
                xscatprintf(&token, " by %s", aka2str(config->addr[0]));
                msg->textLength = (hINT32)strlen(token);
                nfree(msg->text);
                msg->text = token;
                token     = p = NULL;
                break;
            }
        }

        /* if (to_us(msg->destAddr)==0) { */
        if(isOurAka(config, msg->destAddr))
        {
            /*  kill these flags */
            msgHeader.attr &= ~(MSGREAD | MSGKILL | MSGFRQ | MSGSCANNED | MSGLOCKED | MSGFWD);
            /*  set this flags */
            msgHeader.attr |= MSGPRIVATE;
        }
        else
        {
            if(header != NULL)
            {
                /*  set TRS flag, if the mail is not to us(default) */
                if(config->keepTrsMail)
                {
                    msgHeader.attr &= ~(MSGKILL | MSGFWD);
                }
                else
                {
                    msgHeader.attr |= MSGFWD;
                }
            }
        }
    }
    else
    {
        /*  kill these flags on echomail messages */
        msgHeader.attr &= ~(MSGREAD | MSGKILL | MSGFRQ | MSGSCANNED | MSGLOCKED);
    }

    /*  always kill crash, hold, sent & local flags on netmail & echomail */
    msgHeader.attr &= ~(MSGCRASH | MSGHOLD | MSGSENT | MSGLOCAL);
    /* FORCED ATTRIBUTES !!! */
    msgHeader.attr |= forceattr;
    sstrcpy((char *)msgHeader.from, msg->fromUserName);
    sstrcpy((char *)msgHeader.to, msg->toUserName);

    /* val: strip directories and drives from filenames */
    if(((msgHeader.attr & MSGFILE) == MSGFILE) && (msg->netMail == 1) &&
       strpbrk(msg->subjectLine, "/\\:"))
    {
        /* w_log('B', "Original subj: `%s'", msg->subjectLine); */
        running = msg->subjectLine;
        token   = strseparate(&running, " ,\t");

        while(token != NULL)
        {
            int l = (int)strlen(token) - 1;

            while(l >= 0 && token[l] != '\\' && token[l] != '/' && token[l] != ':')
            {
                l--;
            }

            if(newSubj)
            {
                xstrcat(&newSubj, " ");
            }

            xstrcat(&newSubj, token + l + 1);
            token = strseparate(&running, " ,\t");
        } /*  end while */
          /* w_log('B', "Modified subj: `%s'", newSubj); */
    }

    if(newSubj)
    {
        if(strlen(newSubj) < XMSG_SUBJ_SIZE)
        {
            strcpy((char *)msgHeader.subj, newSubj);
        }
        else
        {
            strncpy((char *)msgHeader.subj, newSubj, XMSG_SUBJ_SIZE - 1);
            w_log('9', "Long subjectLine! Some files will be not routed.");
        }

        nfree(newSubj);
    }
    else
    {
        strcpy((char *)msgHeader.subj, msg->subjectLine);
    }

    msgHeader.orig = msg->origAddr;
    msgHeader.dest = msg->destAddr;
    strncpy((char *)msgHeader.__ftsc_date, (char *)msg->datetime, FTSC_DATE_SIZE);
    ASCII_Date_To_Binary((char *)msg->datetime, (union stamp_combo *)&(msgHeader.date_written));
    currentTime = time(NULL);
    date        = localtime(&currentTime);
    TmDate_to_DosDate(date, &dosdate);
    msgHeader.date_arrived = dosdate.msg_st;
    return msgHeader;
} /* createXMSG */

void freeMsgBuffers(s_message * msg)
{
    if(!msg)
    {
        return;
    }

    nfree(msg->text);
    nfree(msg->ctl);
    nfree(msg->subjectLine);
    nfree(msg->toUserName);
    nfree(msg->fromUserName);
}
