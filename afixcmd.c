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
 *
 * See also http://www.gnu.org
 *****************************************************************************
 * $Id$
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef UNIX
#include <unistd.h>
#else
#include <io.h>
#endif
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

int FindTokenPos4Link(char **confName, char* ftoken, s_link *link, long* start, long*end)
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
      /*  remove line */
      nfree(cfgline);
      *start = getCurConfPos();
      *end   = get_hcfgPos();
      *confName = sstrdup(getCurConfName());
      close_conf();
      return 1;
   }
   return 0;
}

int InsertCfgLine(char *confName, char* cfgLine, long strbeg, long strend) 
{
    char *line = NULL, *newname = NULL, *p;
    FILE *f_conf, *f_newconf;
    long endpos,curpos,cfglen;
    int  openro = 0;

    f_conf = f_newconf = NULL;
    if ((strbeg == 0 && strend == 0) || confName == NULL)
	return 0;
    
    if ((f_conf = fopen(confName, "r+b")) == NULL) {
	if ((f_conf = fopen(confName, "rb")) == NULL) {
	    w_log(LL_ERR, "Cannot open config file %s: %s\n", confName, strerror(errno));
	    return 0;
	}
	openro = 1;
    }
    fseek(f_conf, 0L, SEEK_END);
    endpos = ftell(f_conf);
    curpos = strend;
    cfglen = endpos - curpos;
    newname = (char *)smalloc(strlen(confName) + 5);
    strcpy(newname, confName);
    p=strrchr(newname, '.');
    if (p==NULL || strchr(p, PATH_DELIM))
	strcat(newname, ".tmp");
    else
	strcpy(p, ".tmp");
    if ( (f_newconf = fopen(newname, "wb")) == NULL ) {
        /* we have no write access to this directory? */
        /* change config "in place" */
        if (openro) {
            w_log(LL_ERR, "Cannot open temp file %s: %s\n", newname, strerror(errno));
            nfree(newname);
            fclose(f_conf);
            return 0;
        }
        nfree(newname);
        line = (char*) smalloc((size_t) cfglen);
        fseek(f_conf, curpos, SEEK_SET);
        if (fread(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen) {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
            nfree(line);
            fclose(f_conf);
            return 0;
        }
        fseek(f_conf, strbeg, SEEK_SET);
        setfsize( fileno(f_conf), strbeg );
        if (cfgLine) /*  line not deleted */
        {
            if (fprintf(f_conf, "%s%s", cfgLine, cfgEol()) != (int)(strlen(cfgLine)+strlen(cfgEol())))
                w_log(LL_ERR, "Cannot write config file %s: %s\n", confName, strerror(errno));
        }
        if (fwrite(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen ||
            fflush(f_conf) != 0)
        {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", confName, strerror(errno));
        }
        fclose(f_conf);
        nfree(line);
    } else {
        /* make new config-file and rename it */
#ifdef UNIX
        struct stat st;
        if (fstat(fileno(f_conf), &st) == 0)
            fchmod(fileno(f_newconf), (st.st_mode & 01777) | 0400);
#endif
        line = (char*) smalloc(cfglen > strbeg ? cfglen : strbeg);
        fseek(f_conf, 0L, SEEK_SET);
        if (fread(line, sizeof(char), strbeg, f_conf) < (size_t)strbeg) {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
errwriteconf:
            fclose(f_conf);
            fclose(f_newconf);
            unlink(newname);
            nfree(line);
            nfree(newname);
            return 0;
        }
        if (fwrite(line, sizeof(char), strbeg, f_newconf) < (size_t)strbeg) {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
            goto errwriteconf;
        }
        if (cfgLine) {
            if (fprintf(f_newconf, "%s%s", cfgLine, cfgEol()) != (int)(strlen(cfgLine)+strlen(cfgEol())))
            {
                w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
                goto errwriteconf;
            }
        }
        fseek(f_conf, curpos, SEEK_SET);
        if (fread(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen) {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
            goto errwriteconf;
        }
        if (fwrite(line, sizeof(char), cfglen, f_newconf) != (size_t)cfglen ||
            fflush(f_newconf) != 0) {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
            goto errwriteconf;
        }
        fclose(f_newconf);
        fclose(f_conf);
        nfree(line);
        /* save old config as *.bak? */
/*
#ifndef UNIX
        unlink(confName);                
#endif
*/
/*        if (rename(newname, confName)) { */
        if (move_file(newname, confName,1)) {
            w_log(LL_ERR, "Cannot rename config file %s->%s: %s\n", newname, confName, strerror(errno));
            nfree(newname);
            return 0;
        }
        nfree(newname);
    }
    return 1;
}

    /*  opt = 0 - AreaFix */
    /*  opt = 1 - AutoPause */
int Changepause(char *confName, s_link *link, int opt, int type)
{
    long  strbeg=0;
    long  strend=0; 
    
    FindTokenPos4Link(&confName, "pause", link, &strbeg, &strend);
    
    link->Pause ^= type;
    
    if       (link->Pause == NOPAUSE) {
        if(InsertCfgLine(confName, "Pause off", strbeg, strend)) 
            w_log('8', "areafix: system %s set active",	aka2str(link->hisAka));
    } else if(link->Pause == (EPAUSE|FPAUSE)) { 
        if(InsertCfgLine(confName, "Pause on", strbeg, strend)) 
            w_log('8', "%s: system %s set passive",
            opt ? "autopause" : "areafix", aka2str(link->hisAka));
    } else if(link->Pause == EPAUSE) {
        if(InsertCfgLine(confName, "Pause Earea", strbeg, strend)) 
            w_log('8', "%s: system %s set passive only for echos",
            opt ? "autopause" : "areafix", aka2str(link->hisAka));
    } else {
        if(InsertCfgLine(confName, "Pause Farea", strbeg, strend)) 
            w_log('8', "%s: system %s set passive only for file echos",
            opt ? "autopause" : "areafix", aka2str(link->hisAka));
    }
    nfree(confName);
    return 1;
}

int testAddr(char *addr, hs_addr hisAka)
{
    hs_addr aka;
    string2addr(addr, &aka);
    if (addrComp(aka, hisAka)==0) return 1;
    return 0;
}

int DelLinkFromString(char *line, hs_addr linkAddr)
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
		/*  return description if needed */
		if (retd) {
		    *desc = NULL;
		    if (running) {
			/* strip "" at the beginning & end */
			if (running[0]=='"' && running[strlen(running)-1]=='"') {
			    running++; running[strlen(running)-1]='\0';
			}
			/* change " -> ' */
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
    /*  not found */
    fclose(f);
    return 0;
}

void Addlink(s_link *link, s_area *earea, s_filearea *farea) {
    
    s_arealink *arealink;
    
    if(farea)
    {
        farea->downlinks = srealloc(farea->downlinks, sizeof(s_arealink*)*(farea->downlinkCount+1));
        arealink = farea->downlinks[farea->downlinkCount] = (s_arealink*) scalloc(1, sizeof(s_arealink));
        arealink->link = link;
        
        if (link->numOptGrp > 0) {
            /*  default set export on, import on, mandatory off, manual off */
            arealink->export = 1;
            arealink->import = 1;
            arealink->mandatory = 0;
            arealink->manual = 0;
            
            if (grpInArray(farea->group,link->optGrp,link->numOptGrp)) {
                arealink->export = link->export;
                arealink->import = link->import;
                arealink->mandatory = link->mandatory;
                arealink->manual = link->manual;
            }
            
        } else {
            arealink->export = link->export;
            arealink->import = link->import;
            arealink->mandatory = link->mandatory;
            arealink->manual = link->manual;
        }
        if (farea->mandatory) arealink->mandatory = 1;
        if (farea->manual) arealink->manual = 1;
        if (link->level < farea->levelread)	arealink->export=0;
        if (link->level < farea->levelwrite) arealink->import=0;
        /*  paused link can't receive mail */
        if ((link->Pause & FPAUSE) == FPAUSE) arealink->export = 0;
        
        farea->downlinkCount++;
    }
    
    if(earea)
    {
        earea->downlinks = srealloc(earea->downlinks, sizeof(s_arealink*)*(earea->downlinkCount+1));
        arealink = earea->downlinks[earea->downlinkCount] = (s_arealink*) scalloc(1, sizeof(s_arealink));
        arealink->link = link;
        
        if (link->numOptGrp > 0) {
            /*  default set export on, import on, mandatory off, manual off */
            arealink->export = 1;
            arealink->import = 1;
            arealink->mandatory = 0;
            arealink->manual = 0;
            
            if (grpInArray(earea->group,link->optGrp,link->numOptGrp)) {
                arealink->export = link->export;
                arealink->import = link->import;
                arealink->mandatory = link->mandatory;
                arealink->manual = link->manual;
            }
            
        } else {
            arealink->export = link->export;
            arealink->import = link->import;
            arealink->mandatory = link->mandatory;
            arealink->manual = link->manual;
        }
        if (earea->mandatory) arealink->mandatory = 1;
        if (earea->manual) arealink->manual = 1;
        if (link->level < earea->levelread)	arealink->export=0;
        if (link->level < earea->levelwrite) arealink->import=0;
        /*  paused link can't receive mail */
        if ((link->Pause & EPAUSE) == EPAUSE) arealink->export = 0;
        
        earea->downlinkCount++;
    }
}

void RemoveLink(s_link *link, s_area *earea, s_filearea *farea) {
	unsigned int i;

    if(earea) /* remove link from echoarea */
    {
        if ( (i=isAreaLink(link->hisAka, earea)) != -1) {
            nfree(earea->downlinks[i]);
            earea->downlinks[i] = earea->downlinks[earea->downlinkCount-1];
            earea->downlinkCount--;
        }
    }
    if(farea) /* remove link from fileechoarea */
    {
        s_link *links;
        
        for (i=0; i < farea->downlinkCount; i++) {
            links = farea->downlinks[i]->link;
            if (addrComp(link->hisAka, links->hisAka)==0) break;
        }
        nfree(farea->downlinks[i]);
        farea->downlinks[i] = farea->downlinks[farea->downlinkCount-1];
        farea->downlinkCount--;
    }
}


