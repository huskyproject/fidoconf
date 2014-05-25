/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998-1999
 *
 * Matthias Tichy
 *
 * Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
 * Internet: mtt@tichy.de
 *
 * Grimmestr. 12         Buchholzer Weg 4
 * 33098 Paderborn       40472 Duesseldorf
 * Germany               Germany
 *
 * This file is part of FIDOCONFIG.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see file COPYING. If not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <huskylib/huskylib.h>

#ifdef HAS_SYS_SYSEXITS_H
#include <sys/sysexits.h>
#elif defined(HAS_SYSEXITS_H)
#include <sysexits.h>
#endif

#ifdef HAS_STRINGS_H
#include <strings.h>
#endif /* HAS_STRINGS_H */

/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

/* smapi */
#include <smapi/msgapi.h>

/* fidoconfig */
#include "fidoconf.h"
#include "common.h"
#include "areatree.h"
#include "grptree.h"

/* Global for fidoconfig, do not export from DLL, please! */
/*s_fidoconfig *config;*/

/* End of line: CR+LF (wasCR==1) or LF only (wasCR==0) */
static int wasCR=0;

sApp theApp = { 0, NULL };

const char *cfgEol()
{
    return wasCR ? "\r\n" : "\n";
}

char *readLine(FILE *f)
{
    char *line=NULL;
    int len=0, i=0, stop=0;
    int ch;

    if (get_hcfg()) wasCR = 0;
    do {
	ch = getc (f);
        /*  not fgets() 'cause it concatenates lines without \r on Watcom C / WinDos */
	if (ch < 0) { /*  EOF */
	    if (i==0) {
		return NULL;
	    } else { /*  EOF without EOL */
		if (i >= len) {
		    len += 128;
		    line = srealloc (line, len);
		}
		line[i] = '\0';
		stop++;
	    }
	} else {
	    if (i >= len) {
		len += 128;
		line = srealloc (line, len);
	    }

	    if (ch=='\n') { /*  EOL */
		line[i] = '\0';
		stop++;
	    } else if (ch=='\r') { /*  CR (must be before LF), ignore */
		if (get_hcfg()) wasCR = 1;
	    } else { /*  other characters */
		line[i] = ch;
		i++;
	    }
	}
    } while (!stop);

    line = srealloc (line, strlen(line)+1);

    return line;
}

char *trimLine(char *line)
{
   char *start = line;

   while ((*start == ' ') || (*start == '\t') /*|| (*start == '\xFE')*/ ) start++; /* whats is 0xFE? */
   striptwhite(start);
   if(start != line)
      memmove(line, start, strlen(start)+1);
   return line;
}

/* Strips trailing spaces from a string. */

char *striptwhite(char *str)
{
    char *p;

    if (str == NULL)
    {
        return str;
    }
    if (*str == 0)
    {
        return str;   /*  strend is undefined for zero-length string! */
    }
    p = strend(str);
    while (p >= str && *p && isspace((unsigned char)*p))
    {
        *p = '\0';
        p--;
    }
    return str;
}

char *stripComment(char *line)
{
  char *aux;

  if (line == NULL || *line == 0) return line;

  if (line[0]==CommentChar) {
    line[0]='\0';
    return line;
  }

  aux = line;
  while ((aux=strchr(aux+1,CommentChar)) != NULL) {
    if ((*(aux-1)==' ' || *(aux-1)=='\t' ) && (isspace(aux[1]) || aux[1]=='\0'))
    {
      *(aux-1)='\0';
      break;
    }
  }

  striptwhite(line);

  return line;
}

void initConfig(s_fidoconfig *config)
{
   /* set all to 0 */
   memset(config, 0, sizeof(s_fidoconfig));

   /* set defaults */
   config -> loguid = config -> loggid = config -> logperm = (UINT)-1;
   config -> tossingExt = strdup("tos");
   config -> convertLongNames = config -> convertShortNames = cDontTouch;
   config -> typeDupeBase = hashDupesWmsgid;
   config -> packNetMailOnScan = 1;
   config -> recodeMsgBase = 1;
   config -> reportRequester = 1;
   config -> minDiskFreeSpace = 10;

   config->dupeArea.areaType = ECHOAREA;
   config->badArea.areaType = ECHOAREA;
   config->EchoAreaDefault.areaType = ECHOAREA;
   config->FileAreaDefault.areaType = FILEAREA;

   initGroupTree();
}

char *getConfigFileNameForProgram(char *envVar, char *configName)
{
   char *envFidoConfig = getenv(envVar);
   char *osSpecificName;
   int i;

   FILE *f = NULL;
   char *ret;

#ifdef CFGDIR
   char *osSpecificPrefix = CFGDIR;
#elif defined(__linux__)
   char *osSpecificPrefix = "/etc/fido/";
#elif defined(__FreeBSD__)
   char *osSpecificPrefix = "/usr/local/etc/fido/";
#elif defined(__QNXNTO__)
   char *osSpecificPrefix = "/etc/fido/";
#elif defined(__UNIX__)
   char *osSpecificPrefix = "./";
#else
   char *osSpecificPrefix = "";
#endif

   /* try env-var fidoconfig */
   if (envFidoConfig != NULL) f = fopen(envFidoConfig, "r");

   if (f == NULL) {
      if (configName == NULL) return NULL;

      /* try osSpecificName */
      osSpecificName = (char *) smalloc(strlen(osSpecificPrefix)+strlen(configName)+2); /*  +1 - for training delimiter */
      strcpy(osSpecificName, osSpecificPrefix);

      i = strlen(osSpecificName);
      if (i && osSpecificName[i - 1] != '/' && osSpecificName[i - 1] != '\\') {
         osSpecificName[i] = PATH_DELIM;
         osSpecificName[i+1] = '\0';
      }

      strcat(osSpecificName, configName);

      f = fopen(osSpecificName, "r");
      if (f==NULL) {
         if (NULL != (envFidoConfig = getenv("FIDOCONFIG"))) {
            if (strrchr(envFidoConfig, PATH_DELIM) != NULL) {
               nfree (osSpecificName);
               i = strlen(envFidoConfig) - strlen(strrchr(envFidoConfig,PATH_DELIM)) + strlen(configName)+1;
               osSpecificName = smalloc (i+1);
               strncpy (osSpecificName,envFidoConfig,i);
               strcpy (strrchr(osSpecificName,PATH_DELIM)+1,configName);
               f = fopen (osSpecificName, "r");
               if (f==NULL) return NULL; else ret = osSpecificName;
            } else return NULL;
         } else return NULL;
      } else ret =  osSpecificName;
   } else ret = envFidoConfig;

   fclose(f);

   return ret;
}

char *getConfigFileName(void) {
#ifndef CFGNAME
#define CFGNAME "config"
#endif
   return getConfigFileNameForProgram("FIDOCONFIG", CFGNAME);
}

int carbonNames2Addr(s_fidoconfig *config)
{
   unsigned int i, found, narea;
   s_carbon *cb;
   ps_area aptr;
   char *cbaName=NULL;
   int rc=0;

   if(!config->carbonCount) return 0;

   cb = &(config->carbons[0]);

   for (i=0; i<config->carbonCount; i++, cb++) {
       /* Can't use getArea() - it doesn't say about export and
	  doesn't look at localAreas */
       /* now getArea can found local areas */
       if(cb->rule&CC_AND) /* area is not used with AND */
	   continue;
       found=0;
       if(cb->areaName!=NULL){
	   cbaName=cb->areaName;
	   if (cbaName[0]=='*') cbaName++;

	   if (!(cb -> extspawn)) {
	       aptr=config->echoAreas;
	       for (narea=0; narea<config->echoAreaCount && !found; narea++,aptr++) {
		   if (stricmp(cbaName, aptr->areaName)==0) {
		       found++;
		       cb->area = aptr;
		       cb->aexport = 1;
		       cb->netMail = 0;
		   }
	       }
	       aptr=config->localAreas;
	       for (narea=0; narea<config->localAreaCount && !found; narea++,aptr++) {
		   if (stricmp(cbaName, aptr->areaName)==0) {
		       found++;
		       cb->area = aptr;
		       cb->aexport = 0;
		       cb->netMail = 0;
		   }
	       }
	       aptr=config->netMailAreas;
	       for (narea=0; narea<config->netMailAreaCount && !found; narea++,aptr++){
		   if (stricmp(cbaName, aptr->areaName)==0) {
		       found++;
		       cb->area = aptr;
		       cb->aexport = 0;
		       cb->netMail = 1;
		   }
	       }
	   }
       }

       if (!found && (cb->move != 2) && !cb->extspawn) { /*  move==2 - delete */
         if (config->badArea.areaName) {
	   printf("Could not find area \"%s\" for carbon copy. Use BadArea\n",
		  (cb->areaName) ? cb->areaName : "");
	   cb->area = &(config->badArea);
	   if (cb->areaName!=NULL) {
	       i = (*cb->areaName=='*') ? 1 : 0;
	       nfree(cb->areaName);
	   }
	   else i = 0;
	   cb->areaName = (char *) smalloc(sstrlen(config->badArea.areaName)+i+1);
	   if (i) *cb->areaName='*';
	   strcpy(cb->areaName+i,config->badArea.areaName);
	   cb->aexport = 0;
	 } else {
	   printf("Could not find area \"%s\" for carbon copy and BadArea not defined. Can't use this area for carbon copy\n", cb->areaName);
           cb->area = NULL;
           rc++;
	 }
       }
   }
   return rc;
}

void processAreaPermissions(s_fidoconfig *config, ps_area areas, unsigned areaCount)
{
    unsigned i,narea, nalink;
    ps_area aptr;
    ps_arealink *dlink;
    char *ExclMask;
    
    
    if (config->readOnlyCount) {
        for (i=0; i < config->readOnlyCount; i++) {
            if(config->readOnly[i].areaMask[0] != '!') {
                for (aptr=areas, narea=0; narea < areaCount; narea++, aptr++) {
                    if (patimat (aptr->areaName, config->readOnly[i].areaMask)) {
                        for (nalink=0, dlink=aptr->downlinks; nalink < aptr->downlinkCount; nalink++, dlink++) {
                            if (patmat (aka2str((*dlink)->link->hisAka),
                                config->readOnly[i].addrMask)) {
                                (*dlink)->import = 0;
                            }
                        }
                    }
                }
            } else {
                ExclMask = config->readOnly[i].areaMask;
                ExclMask++;
                for (aptr=areas, narea=0; narea < areaCount; narea++, aptr++) {
                    if (patimat (aptr->areaName, ExclMask)) {
                        for (nalink=0, dlink=aptr->downlinks; nalink < aptr->downlinkCount; nalink++, dlink++) {
                            if (patmat (aka2str((*dlink)->link->hisAka),
                                config->readOnly[i].addrMask)) {
                                (*dlink)->import = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (config->writeOnlyCount) {
        for (i=0; i < config->writeOnlyCount; i++) {
            if(config->writeOnly[i].areaMask[0] != '!') {
                for (aptr=areas, narea=0; narea < areaCount; narea++, aptr++) {
                    if (patimat (aptr->areaName, config->writeOnly[i].areaMask)) {
                        for (nalink=0, dlink=aptr->downlinks; nalink < aptr->downlinkCount; nalink++, dlink++) {
                            if (patmat (aka2str((*dlink)->link->hisAka),
                                config->writeOnly[i].addrMask)) {
                                (*dlink)->aexport = 0;
                            }
                        }
                    }
                }
            } else {
                ExclMask = config->writeOnly[i].areaMask;
                ExclMask++;
                for (aptr=areas, narea=0; narea < areaCount; narea++, aptr++) {
                    if (patimat (aptr->areaName, ExclMask)) {
                        for (nalink=0, dlink=aptr->downlinks; nalink < aptr->downlinkCount; nalink++, dlink++) {
                            if (patmat (aka2str((*dlink)->link->hisAka),
                                config->writeOnly[i].addrMask)) {
                                (*dlink)->aexport = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* set link-area permissions stored in readOnly[], writeOnly[] */
void processPermissions(s_fidoconfig *config)
{
    if      (theApp.module == M_HPT || theApp.module == M_TPARSER)
        processAreaPermissions(config, config->echoAreas, config->echoAreaCount);
    else if (theApp.module == M_HTICK || theApp.module == M_TPARSER)
        processAreaPermissions(config, config->fileAreas, config->fileAreaCount);
}

void fixRoute(s_fidoconfig *config)
{
    unsigned int i;

    for (i = 0; i < config->routeCount; i++) {
        if (config->route[i].viaStr != NULL)
            config->route[i].target = getLink(config, config->route[i].viaStr);
        nfree(config->route[i].viaStr);
    }
}

void stripPktPwd(s_fidoconfig *config)
{
   unsigned int i;
   for (i = 0; i < config->linkCount; i++) {
      if (config->links[i]->pktPwd && strlen(config->links[i]->pktPwd) > 8) {
         if (config->links[i]->pktPwd == config->links[i]->defaultPwd) {
            config->links[i]->pktPwd = (char *) smalloc(9);
            memcpy(config->links[i]->pktPwd, config->links[i]->defaultPwd, 8);
         }
         config->links[i]->pktPwd[8] = '\0';
/*         printf("WARNING: pktPwd too long! Truncated to 8 chars (%s)\n",aka2str(config->links[i]->hisAka));
         fprintf(stderr,"pktPwd too long! Truncated to 8 chars (%s)\n",aka2str(config->links[i]->hisAka));
*/
      }
   }
}

void setConfigDefaults(s_fidoconfig *config)
{
   ps_robot r;

   r = getRobot(config, "areafix", 1);
   r->areas = &(config->echoAreas);
   r->areaCount = &(config->echoAreaCount);
   r->strA = sstrdup("area");
   r->strC = sstrdup("echoarea");
   if (!r->names) 
   {
	   char *s =sstrdup("AreaFix AreaMgr hpt");
	   r->names = makeStrArray(s);
	   nfree(s);
   }

   r = getRobot(config, "filefix", 1);
   r->areas = &(config->fileAreas);
   r->areaCount = &(config->fileAreaCount);
   r->strA = sstrdup("filearea");
   r->strC = sstrdup("filearea");
   if (!r->names)
   {
	   char *s =sstrdup("FileFix FileMgr AllFix FileScan htick");
	   r->names = makeStrArray(s);
	   nfree(s);
   }

   if (config->sysop==NULL) xstrcat(&config->sysop,"SysOp");
   if (config->advisoryLock==0)  config->advisoryLock  = 0;
   if ( RebuildEchoAreaTree(config) == 0 || RebuildFileAreaTree(config) == 0 ) {
      printf("Please correct above error(s) first!\n");
      fflush(stdout);
      exit(EX_CONFIG);
   }

   /* defaults for htick */
   if(config->fileDescription==NULL) xstrcat(&config->fileDescription,"files.bbs");

   if (!config->tempDir) {
      char *p=NULL;
      if ((p=getenv("TEMP")) != NULL ||
          (p=getenv("TMP")) != NULL ||
          (p=getenv("TMPDIR")) != NULL)
         parsePath(p, &(config->tempDir), NULL);
      else
#if defined(__UNIX__) && !defined (__MINGW32__)
         parsePath("/tmp", &(config->tempDir), NULL);
#elif defined(WINNT) || defined (__MINGW32__)
         if ((getenv("WINDIR")) != NULL ){
            xstrscat( &p, getenv("WINDIR"), "\\TEMP", NULLP);
            parsePath(p, &(config->tempDir), NULL);
            nfree(p);
         }
#else
         parsePath("c:\\", &(config->tempDir), NULL);
#endif
   }
   { /* add all our aka to links array */
       unsigned i;
       s_link   *clink;

       for (i = 0; i < config->addrCount; i++) {
           if (!getLinkFromAddr(config,config->addr[i])) {
               config->links = srealloc(config->links, sizeof(ps_link)*(config->linkCount+1));
               config->links[config->linkCount] = scalloc(1,sizeof(s_link));
               clink = config->links[config->linkCount];
               memset(clink, 0, sizeof(s_link));
               clink->areafix.on = 1;
               clink->filefix.on = 1;
               clink->filefix.autoCreate = 1; /* needed for hpucode + htick */
               clink->aexport = 0;         /* do not export anything to virtual link */
               clink->import = 1;
               clink->maxUnpackedNetmail = 100;
               memcpy ( &(clink->hisAka), &(config->addr[i]), sizeof(hs_addr));
               clink->ourAka = &(config->addr[i]);
               xscatprintf(&(clink->name), "Our virtual link for aka: %s",aka2str(config->addr[i]));
               xscatprintf(&(clink->defaultPwd),"%X",strcrc32(clink->name, 0xFFFFFFFFL));
               clink->pktPwd = clink->defaultPwd;
               clink->ticPwd = clink->defaultPwd;
               clink->areafix.pwd = clink->defaultPwd;
               clink->filefix.pwd = clink->defaultPwd;
               clink->bbsPwd = clink->defaultPwd;
               clink->sessionPwd = clink->defaultPwd;
               config->linkCount++;
           }
       }
   }
}

/* Read fidoconfig from file into memory.
 * Parameter: filename or NULL
 * if NULL: try to find FIDOCONFIG enviroment variable, next use hardcoded path
 * Return NULL and print diagnostic message to stdout if error(s) found.
 */
s_fidoconfig *readConfig(const char *fileName)
{
   s_fidoconfig *config;  /* Use global variable ?*/
   char *line, *tmp;

   if (fileName==NULL) fileName = getConfigFileName();

   if (fileName == NULL) {
        printf("Could not find Config-file\n");
        exit(EX_UNAVAILABLE);
   }

   tmp = GetDirnameFromPathname(fileName);
   setvar("configdir", tmp);
   nfree(tmp);

   if (init_conf(fileName))
      return NULL;

   config = (s_fidoconfig *) smalloc(sizeof(s_fidoconfig));

   /* initialization and setting default values */
   initConfig(config);

   while ((line = configline()) != NULL) {
      line = trimLine(line);
      if (line[0] != 0) {
         line = shell_expand(line);
         parseLine(line, config);
      }
      nfree(line);
   }

   if (wasError == 1) {
      printf("Please correct above error(s) first!\n");
      fflush(stdout);
      exit(EX_CONFIG);
   }
   checkIncludeLogic(config);
   close_conf();
   if(carbonNames2Addr(config)) { /* Can't use carbon copy/move/delete */
      printf("Please correct above error(s) first!\n");
      fflush(stdout);
      exit(EX_CONFIG);
   }
   setConfigDefaults(config);
   processPermissions (config);
   fixRoute(config);
   stripPktPwd(config);
   theApp.config = config;
   return config;
}

void fc_freeEchoArea(s_area *area) {
    unsigned int i;
	nfree(area->areaName);
	nfree(area->fileName);
	nfree(area->description);
	nfree(area->group);
	for (i=0; i < area->downlinkCount; i++) nfree(area->downlinks[i]);
	nfree(area->downlinks);
	nfree(area->sbadd);
	nfree(area->sbign);
	nfree(area->sbstrip);
	nfree(area->sbkeep);
}

void freeBbsArea(s_bbsarea area) {
        nfree(area.areaName);
        nfree(area.pathName);
        nfree(area.description);
}

void freeSaveTic(s_savetic *savetic) {
        nfree(savetic->fileAreaNameMask);
        nfree(savetic->pathName);
}

/* Dispose fidoconfig structure: free memory.
 */
void disposeConfig(s_fidoconfig *config)
{
  unsigned int i;

   nfree(config->name);
   nfree(config->sysop);
   nfree(config->location);
   nfree(config->email);

   nfree(config->addr);

   for (i=0; i < config->publicCount; i++) nfree(config->publicDir[i]);
   nfree(config->publicDir);

   for (i = 0; i< config->linkCount; i++) freeLink(config->links[i]);
   nfree(config->links);

   freeLink(config->linkDefaults);

   nfree(config->inbound);
   nfree(config->outbound);
   nfree(config->ticOutbound);
   nfree(config->protInbound);
   nfree(config->listInbound);
   nfree(config->localInbound);
   nfree(config->tempInbound);
   nfree(config->logFileDir);
   nfree(config->tempDir);
   nfree(config->seqDir);
   nfree(config->dupeHistoryDir);
   nfree(config->nodelistDir);
   nfree(config->msgBaseDir);
   nfree(config->magic);
   nfree(config->semaDir);
   nfree(config->badFilesDir);
   nfree(config->tempOutbound);
   nfree(config->fileAreaBaseDir);
   nfree(config->passFileAreaDir);
   nfree(config->busyFileDir);
   nfree(config->hptPerlFile);
   nfree(config->sendmailcmd);
   nfree(config->PublicGroup);


   for (i = 0; i< config->netMailAreaCount; i++)
       fc_freeEchoArea(&(config->netMailAreas[i]));
   nfree(config->netMailAreas);

   fc_freeEchoArea(&(config->dupeArea));
   fc_freeEchoArea(&(config->badArea));

   for (i = 0; i< config->echoAreaCount; i++)
       fc_freeEchoArea(&(config->echoAreas[i]));
   nfree(config->echoAreas);

   for (i = 0; i< config->fileAreaCount; i++)
       fc_freeEchoArea(&(config->fileAreas[i]));
   nfree(config->fileAreas);

   for (i = 0; i< config->bbsAreaCount; i++)
       freeBbsArea(config->bbsAreas[i]);
   nfree(config->bbsAreas);
   for (i = 0; i< config->localAreaCount; i++)
       fc_freeEchoArea(&(config->localAreas[i]));
   nfree(config->localAreas);

   FreeAreaTree();
   freeGrpTree();

   fc_freeEchoArea(&(config->EchoAreaDefault));
   fc_freeEchoArea(&(config->FileAreaDefault));

   nfree(config->robotsArea);

   for (i = 0; i < config->robotCount; i++) {
     nfree(config->robot[i]->name);
     nfree(config->robot[i]->strA);
     nfree(config->robot[i]->strC);
     nfree(config->robot[i]->names);
     nfree(config->robot[i]->fromName);
     nfree(config->robot[i]->origin);
     nfree(config->robot[i]->helpFile);
     nfree(config->robot[i]->rulesDir);
     nfree(config->robot[i]->newAreaRefuseFile);
     nfree(config->robot[i]->autoCreateFlag);
     nfree(config->robot[i]->queueFile);
     nfree(config->robot[i]->reportsFlags);
     nfree(config->robot[i]->splitStr);
	 nfree(config->robot[i]);
   }
   nfree(config->robot);

   for (i = 0; i < config->routeCount; i++) nfree(config->route[i].pattern);
   nfree(config->route);
   for (i = 0; i < config->remapCount; i++)
       if (config->remaps[i].toname!=NULL)
          nfree(config->remaps[i].toname);
   nfree(config->remaps);

   for (i = 0; i < config->groupCount; i++) {
       nfree(config->group[i].name);
       nfree(config->group[i].desc);
   }
   nfree(config->group);

   for (i = 0; i < config->nodelistCount; i++)
     {
       if (config->nodelists[i].nodelistName != NULL)
         nfree(config->nodelists[i].nodelistName);
       if (config->nodelists[i].diffUpdateStem != NULL)
         nfree(config->nodelists[i].diffUpdateStem);
       if (config->nodelists[i].fullUpdateStem != NULL)
         nfree(config->nodelists[i].fullUpdateStem);
     }
   nfree(config->nodelists);
   nfree(config->fidoUserList);

   for (i = 0; i < config->packCount; i++) {
           nfree(config->pack[i].packer);
           nfree(config->pack[i].call);
   }
   nfree(config->pack);

   for (i = 0; i < config->unpackCount; i++) {
           nfree(config->unpack[i].matchCode);
           nfree(config->unpack[i].mask);
           nfree(config->unpack[i].call);
   }
   nfree(config->unpack);
   nfree(config->intab);
   nfree(config->outtab);
   nfree(config->importlog);
   nfree(config->fileAreasLog);
   nfree(config->fileNewAreasLog);
   nfree(config->longNameList);
   nfree(config->fileArcList);
   nfree(config->filePassList);
   nfree(config->fileDupeList);
   nfree(config->advStatisticsFile);
   nfree(config->loglevels);
   nfree(config->screenloglevels);
   nfree(config->echotosslog);
   nfree(config->statlog);
   nfree(config->lockfile);
   nfree(config->fileDescription);
   nfree(config->tossingExt);

   for (i = 0; i< config->carbonCount; i++) {
	   nfree(config->carbons[i].str);
	   nfree(config->carbons[i].areaName);
	   nfree(config->carbons[i].reason);
   }
   nfree(config->carbons);

   for (i=0; i < config->readOnlyCount; i++) {
       nfree (config->readOnly[i].areaMask);
       nfree (config->readOnly[i].addrMask);
   }
   nfree (config->readOnly);

   for (i=0; i < config->writeOnlyCount; i++) {
       nfree (config->writeOnly[i].areaMask);
       nfree (config->writeOnly[i].addrMask);
   }
   nfree (config->writeOnly);


   nfree(config->ReportTo);

   nfree(config->beforePack);
   nfree(config->afterUnpack);
   nfree(config->processPkt);
   nfree(config->fileLocalPwd);
   nfree(config->fileLDescString);

   for (i = 0; i< config->saveTicCount; i++) 
       freeSaveTic(&(config->saveTic[i]));
   nfree(config->saveTic);

   for (i = 0; i < config->execonfileCount; i++) {
		nfree(config->execonfile[i].filearea);
		nfree(config->execonfile[i].filename);
		nfree(config->execonfile[i].command);
   }
   nfree(config->addToSeen);
   nfree(config->ignoreSeen);
   nfree(config->tearline);
   nfree(config->origin);

   for (i = 0; i < config->filelistCount; i++)
   {
     nfree(config->filelists[i].destFile);
     nfree(config->filelists[i].dirHdrTpl);
     nfree(config->filelists[i].dirEntryTpl);
     nfree(config->filelists[i].dirFtrTpl);
     nfree(config->filelists[i].globHdrTpl);
     nfree(config->filelists[i].globFtrTpl);
     nfree(config->filelists[i].dirListHdrTpl);
     nfree(config->filelists[i].dirListEntryTpl);
     nfree(config->filelists[i].dirListFtrTpl);
   }

   for (i = 0; i < config->numuuEGrp; i++)
   {
     nfree(config->uuEGrp[i]);
   }

   nfree(config->netmailFlag);
   nfree(config->notValidFNChars);

   free_vars();

   nfree(config);
   config = NULL;
}

s_link *getLink(s_fidoconfig *config, char *addr)
{
   hs_addr aka;

   memset(&aka, 0, sizeof(hs_addr));

   parseFtnAddrZS(addr, &aka);

   return getLinkFromAddr(config, aka);
}

s_link *getLinkFromAddr(s_fidoconfig *config, hs_addr aka)
{
   unsigned i;

   for (i = 0; i <config->linkCount; i++) {
      if (addrComp(aka, config->links[i]->hisAka)==0) return config->links[i];
   }

   return NULL;
}

s_link *getLinkForArea(const s_fidoconfig *config, char *addr, s_area *area)
{
	hs_addr aka;
	unsigned i;

	memset(&aka, 0, sizeof(hs_addr));

	parseFtnAddrZS(addr, &aka);

	/*  we must find "right" link */
        for (i = 0; i< config->linkCount; i++) {
                if (!config->links[i]->ourAka) continue;
		if (addrComp(aka, config->links[i]->hisAka)==0 &&
			addrComp(*area->useAka, *(config->links[i]->ourAka))==0)
			return config->links[i];
	}

	/*  backward compatibility */
	for (i = 0; i< config->linkCount; i++) {
	    if (addrComp(aka, config->links[i]->hisAka)==0) return config->links[i];
	}

	return NULL;
}

hs_addr *getAddr(const s_fidoconfig *config, char *addr)
{
   hs_addr aka;
   unsigned i;

   memset(&aka, 0, sizeof(hs_addr));
   parseFtnAddrZS(addr, &aka);

   for (i = 0; i < config->addrCount; i++) {
      if (addrComp(aka, config->addr[i])==0) return &(config->addr[i]);
   }

   return NULL;
}

int existAddr(s_fidoconfig *config, hs_addr aka) {
   unsigned i;

   for (i=0; i< config->addrCount; i++) {
      if (addrComp(aka, config->addr[i])==0) return 1;
   }
   return 0;
}

s_area *getEchoArea(s_fidoconfig *config, char *areaName)
{
    return getArea(config, areaName);
}

s_area *getArea(s_fidoconfig *config, char *areaName)
{
    if(areaName) {
      ps_area ret = FindAreaInTree(areaName);
      if(ret)
          return ret;
    }
    return &(config->badArea); /*  if all else fails, return badArea :-) */
}

s_area *getNetMailArea(s_fidoconfig *config, char *areaName)
{
   unsigned i;

   if (areaName==NULL) return (NULL);

   for (i=0; i < config->netMailAreaCount; i++) {
      if (stricmp(config->netMailAreas[i].areaName, areaName)==0)
         return &(config->netMailAreas[i]);
   }
   return (NULL);
}

s_area *getRobotsArea(s_fidoconfig *config)
{
    s_area *area = NULL;

    if (config->robotsArea)
        area = getNetMailArea(config, config->robotsArea);
    if (area == NULL)
        area = &(config->netMailAreas[0]);
    return area;
}

s_area *getFileArea(char *areaName)
{
   return FindFileAreaInTree(areaName);
}

int isLinkOfArea(s_link *link, s_area *area)
{
   unsigned int i;

   for (i = 0; i < area->downlinkCount; i++)
   {
      if (link == area->downlinks[i]->link) return 1;
   }
   return 0;
}


int isOurAka(ps_fidoconfig config, hs_addr link)
{
    unsigned int i;
    for (i = 0; i < config->addrCount; i++) {
        if (addrComp(link, config->addr[i])==0) return 1;
    }
    return 0;
}

int isAreaLink(hs_addr link, s_area *area)
{
    unsigned int i;
    for (i = 0; i < area->downlinkCount; i++) {
        if (addrComp(link, area->downlinks[i]->link->hisAka)==0) {
            return i; /*  return index of link */
        }
    }
    return -1;
}


int grpInArray(char *group, char **strarray, unsigned int len)
{
	unsigned int i;

	if (group==NULL) return 0;

	for (i=0; i < len; i++) {
		if (strarray[i] && strcmp(group, strarray[i])==0) return 1;
	}

	return 0;
}

void SetAppModule(e_known_moduls mod) { theApp.module = mod; }
