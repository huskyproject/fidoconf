/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * afixcmd.c : common areafix commands
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "afixcmd.h"
#include "common.h"
#include "log.h"
#include "xstr.h"
#include <smapi/compiler.h>
#include <smapi/progprot.h>

char* expandCfgLine(char* cfgline)
{
   cfgline = trimLine(cfgline);
   cfgline = stripComment(cfgline);
   cfgline = shell_expand(cfgline);
   cfgline = vars_expand(cfgline);
   return cfgline;
}

int findTokenPos4Link(char **confName, char* ftoken, s_link *link, long* start, long*end)
{
   char* cfgline, *line, *token, *linkConfName;
   long   linkstart=0;
   
   *start=0; *end=0;

   if (init_conf(*confName))
      return 0;
   
   while ((cfgline = configline()) != NULL) {
      cfgline = expandCfgLine(cfgline);
      line = cfgline;
      token = strseparate(&line, " \t");
      if (!token || strcasecmp(token, "link")) {
         nfree(cfgline);
         continue;
      }
linkliner:
      nfree(cfgline);
      for (;;) {
         if ((cfgline = configline()) == NULL) { 
            close_conf();
            return 0;
         }
         cfgline = expandCfgLine(cfgline);
         if (!*cfgline) {
            nfree(cfgline);
            continue;
         }
         line = cfgline;
         token = strseparate(&line, " \t");
         if (!token) {
            nfree(cfgline);
            continue;
         }
         if (stricmp(token, "link") == 0)
            goto linkliner;
         if (stricmp(token, "aka") == 0) break;
         nfree(cfgline);
      }
      token = strseparate(&line, " \t");
      if (!token || testAddr(token, link->hisAka) == 0) {
         nfree(cfgline);
         continue;
      }
      nfree(cfgline);
      linkstart = get_hcfgPos();
      linkConfName = sstrdup(getCurConfName());
      for (;;) {
         if ((cfgline = configline()) == NULL) { 
            *start = *end   = linkstart;
            *confName = linkConfName;
            close_conf();
            return 0;
         }
         cfgline = expandCfgLine(cfgline);
         if (!*cfgline) {
            nfree(cfgline);
            continue;
         }
         line = cfgline;
         token = strseparate(&line, " \t");
         if (token && stricmp(token, "link") == 0)
         {
            *start = *end   = linkstart;
            *confName = linkConfName;
            return 0;
         }
         if (token && stricmp(token, ftoken) == 0) break;
         nfree(cfgline);
      }
      // remove line
      nfree(cfgline);
      *start = getCurConfPos();
      *end   = get_hcfgPos();
      *confName = sstrdup(getCurConfName());
      close_conf();
      return 1;
   }
   return 0;
}

int Changepause(char *confName, s_link *link, int opt, int type)
{
    // opt = 0 - AreaFix
    // opt = 1 - AutoPause
    char *line,*pauseString;
    long curpos, endpos, cfglen;
    long  strbeg=0;
    long  strend=0; 
    FILE *f_conf;
    
    findTokenPos4Link(&confName, "pause", link, &strbeg, &strend);
    if(strbeg == 0 && strend == 0)
        return 0;
    
    f_conf = fopen(confName, "r+b");
    if (f_conf == NULL) {
        fprintf(stderr,"%s: cannot open config file %s \n", opt ? "autopause" : "areafix", confName);
        nfree(confName);
        return 0;
    }
    nfree(confName);
    fseek(f_conf, 0L, SEEK_END);
    endpos = ftell(f_conf);
    curpos = strbeg == strend ? strbeg : strend;
    cfglen = endpos - curpos;
    line = (char*) smalloc((size_t) cfglen+1);
    fseek(f_conf, curpos, SEEK_SET);
    cfglen = fread(line, sizeof(char), cfglen, f_conf);
    line[cfglen]='\0';
    fseek(f_conf, strbeg, SEEK_SET);
    setfsize( fileno(f_conf), strbeg );

    link->Pause ^= type;

    if       (link->Pause == NOPAUSE) {
        pauseString = sstrdup("Pause off");
        w_log('8', "areafix: system %s set active",	aka2str(link->hisAka));
    } else if(link->Pause == (EPAUSE|FPAUSE)) { 
        pauseString = sstrdup("Pause on");
        w_log('8', "%s: system %s set passive", opt ? "autopause" : "areafix", aka2str(link->hisAka));
    } else if(link->Pause == EPAUSE) {
        pauseString = sstrdup("Pause Earea");
        w_log('8', "%s: system %s set passive only for echos", opt ? "autopause" : "areafix", aka2str(link->hisAka));
    } else {
        pauseString = sstrdup("Pause Farea");
        w_log('8', "%s: system %s set passive only for file echos", opt ? "autopause" : "areafix", aka2str(link->hisAka));
    }
    fprintf(f_conf, "%s%s%s", pauseString, cfgEol(), line);
    //setfsize( fileno(f_conf), endpos-(remstr-curpos) );
    fclose(f_conf);
    nfree(line);
    nfree(pauseString);
    return 1;
}

int testAddr(char *addr, s_addr hisAka)
{
    s_addr aka;
    string2addr(addr, &aka);
    if (addrComp(aka, hisAka)==0) return 1;
    return 0;
}

int DelLinkFromString(char *line, s_addr linkAddr)
{
    int rc = 1;
    char *end = NULL;
    char *beg = NULL;

    w_log(LL_FUNC, "::DelLinkFromString() begin");

    beg = strrchr(line, '"'); /* seek end comment pointer (quote char) */
    if(!beg)  beg = line;     /* if not found then seek from begin */
    beg++;                    /* process next token */
    while(*beg)               /* while not end of string */
    {
        while(*beg && isspace(*beg)) beg++; /* skip spaces */
        if(*beg && isdigit(*beg) && testAddr(beg, linkAddr))
        {
            rc = 0;
            break;
        }
        while(*beg && !isspace(*beg)) beg++; /* skip token */
    }
    if(rc == 0) /* beg points to begin of unsubscribed address */
    {
        end = beg;
        while(*beg && !isspace(*beg)) beg++; /* skip token */
        while(*beg && !isdigit(*beg)) beg++; /* find for next link */
        if(beg && *beg)
        {
            strcpy(end,beg);
        }
        else
        {
            end--;
            *end = '\0';
        }
    }

    w_log(LL_FUNC, "%::DelLinkFromString() end");
    return rc;
}

int IsAreaAvailable(char *areaName, char *fileName, char **desc, int retd) {
    FILE *f;
    char *line, *token, *running;

    if (fileName==NULL || areaName==NULL) return 0;
	
    if ((f=fopen(fileName,"r")) == NULL) {
	w_log('8',"Allfix: cannot open file \"%s\"",fileName);
	return 0;
    }
	
    while ((line = readLine(f)) != NULL) {
	line = trimLine(line);
	if (line[0] != '\0') {

	    running = line;
	    token = strseparate(&running, " \t\r\n");

	    if (token && areaName && stricmp(token, areaName)==0) {
		// return description if needed
		if (retd) {
		    *desc = NULL;
		    if (running) {
			//strip "" at the beginning & end
			if (running[0]=='"' && running[strlen(running)-1]=='"') {
			    running++; running[strlen(running)-1]='\0';
			}
			//change " -> '
			token = running;
			while (*token!='\0') {
			    if (*token=='"') *token='\'';
			    token++;
			}
			xstrcat(&(*desc), running);
		    }
		}
		nfree(line);
		fclose(f);
		return 1;
	    }			
	}
	nfree(line);
    }	
    // not found
    fclose(f);
    return 0;
}
