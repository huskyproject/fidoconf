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
#include "typesize.h"
#include "fidoconf.h"
#include "xstr.h"
#include "afixcmd.h"

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

char *createKludges(ps_fidoconfig config, const char *area, const s_addr *ourAka, 
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

   if (!config->disableTID) xscatprintf(&buff, "\001PID: %s\r", versionStr);
   xstrcat(&buff, "\001FLAGS NPD\r");

   return buff;
}
