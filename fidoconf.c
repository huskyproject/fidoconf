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

#include "fidoconf.h"
#include "typesize.h"
#include "common.h"

char *readLine(FILE *f)
{
   char *line = NULL, temp[81];
   size_t size = 1024;

   line = (char *) smalloc(size);
   if (fgets(line, 81, f) == NULL) {
      free(line);                      // end of file...
      return NULL;
   }

   if (line[strlen(line)-1] != '\n') {
     while ((strlen(line) % 80) == 0) {
       if (fgets(temp, 81, f) == NULL) break; // eof encountered
       line = srealloc(line, strlen(line)+strlen(temp)+1);
       strcat(line, temp);
       if (temp[strlen(temp)-1] == '\n') {
	 temp[strlen(temp)-1] = 0; // kill \n
	 break;
       }
     }
   }

   if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
   return line;
}

char *trimLine(char *line)
{
   char *start = line, *temp;

   while ((*start == ' ') || (*start == '\t') || (*start == (char)0xFE)) start++;
   temp = (char *) smalloc(strlen(start)+1);
//   strcpy(temp, start);
   strcpy(temp, striptwhite(start));
   free(line);
   return temp;
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
        return str;   // strend is undefined for zero-length string! 
    }
    p = strend(str);
    while (p > str && *p && isspace((unsigned char)*p))
    {
        *p = '\0';
        p--;
    }
    return str;
}

char *stripComment(char *line)
{
  char *aux;
  
  if (line[0]=='#') {
    line[0]='\0';
    return line;
  }
  
  aux=strchr(line,'#');
  
  if (aux!=NULL) {
    if (*(aux-1)==' ')
      aux[0]='\0';
  }
  
  return line;
}

void initConfig(s_fidoconfig *config) {
   // set all to 0
   memset(config, 0, sizeof(s_fidoconfig));
   config -> loguid = config -> loggid = config -> logperm = -1;
   config -> tossingExt = "tos";
   config -> convertLongNames = config -> convertShortNames = cDontTouch;
   config -> typeDupeBase = hashDupesWmsgid;
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
#elif   defined(__linux__)
   char *osSpecificPrefix = "/etc/fido/";
#else
   char *osSpecificPrefix = "";
#endif

   //try env-var fidoconfig
   if (envFidoConfig != NULL) f = fopen(envFidoConfig, "r");
   
   if (f == NULL) {
      if (configName == NULL) return NULL;
      
      //try osSpecificName
      osSpecificName = (char *) smalloc(strlen(osSpecificPrefix)+strlen(configName)+2); // +1 - for training delimiter
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
               free (osSpecificName);
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

   return getConfigFileNameForProgram("FIDOCONFIG", "config");
}

void carbonNames2Addr(s_fidoconfig *config)
{
   int i, found, narea;
   s_carbon *cb;

   for (i=0; i<config->carbonCount; i++) {
	   /* Can't use getArea() - it doesn't say about export and
		  doesn't look at localAreas */
	   /* now getArea can found local areas */
           found=0;
	   cb = &(config->carbons[i]);
           if (cb->areaName && !(cb -> extspawn)) {
              for (narea=0; narea < config->echoAreaCount && !found; narea++) {
                 if (stricmp(cb->areaName, config->echoAreas[narea].areaName)==0) {
                    found++;
                    cb->area = &(config->echoAreas[narea]);
                    cb->export = 1;
		    cb->netMail = 0;
                 }
              }

              for (narea=0; narea < config->localAreaCount && !found; narea++) {
                 if (stricmp(cb->areaName, config->localAreas[narea].areaName)==0) {
                    found++;
                    cb->area = &(config->localAreas[narea]);
                    cb->export = 0;
		    cb->netMail = 0;
                 }
              }
              for (narea=0; narea < config->netMailAreaCount && !found; narea++) {
                 if (stricmp(cb->areaName, config->netMailAreas[narea].areaName)==0) {
                    found++;
                    cb->area = &(config->netMailAreas[narea]);
                    cb->export = 0;
		    cb->netMail = 1;
                 }
              }

           }

	   if (!found && (cb->move != 2) && !cb->extspawn) {// move==2 - delete
		printf("Could not find area \"%s\" for carbon copy. Use BadArea\n", (config->carbons[i].areaName) ? config->carbons[i].areaName : "");
		config->carbons[i].area = &(config->badArea);
		copyString(config->badArea.areaName, &(config->carbons[i].areaName));
		config->carbons[i].export = 0;
           }
   }
}

void fixRoute(s_fidoconfig *config)
{
	int i;
	
	for (i = 0; i < config->routeCount; i++) {
		if (config->route[i].viaStr != NULL)
			config->route[i].target = getLink(*config, config->route[i].viaStr);
		nfree(config->route[i].viaStr);
	}
	for (i = 0; i < config->routeFileCount; i++) {
		if (config->routeFile[i].viaStr != NULL)
			config->routeFile[i].target = getLink(*config, config->routeFile[i].viaStr);
		nfree(config->routeFile[i].viaStr);
	}
	for (i = 0; i < config->routeMailCount; i++) {
		if (config->routeMail[i].viaStr != NULL)
			config->routeMail[i].target = getLink(*config, config->routeMail[i].viaStr);
		nfree(config->routeMail[i].viaStr);
	}
}

void stripPktPwd(s_fidoconfig *config)
{
   int i;
   for (i = 0; i < config->linkCount; i++) {
      if (config->links[i].pktPwd && strlen(config->links[i].pktPwd) > 8) {
         if (config->links[i].pktPwd == config->links[i].defaultPwd) {
            config->links[i].pktPwd = (char *)malloc(9);
            memcpy(config->links[i].pktPwd, config->links[i].defaultPwd, 8);
         }
         config->links[i].pktPwd[8] = '\0';
      }
   }
}

s_fidoconfig *readConfig(char *cfgFile)
{
   s_fidoconfig *config;
   char *fileName = cfgFile;
   char *line;

   if (fileName==NULL) fileName = getConfigFileName();

   if (fileName == NULL) {
        printf("Could not find Config-file\n");
        exit(1);
   }

   if (init_conf(fileName))
      return NULL;

   config = (s_fidoconfig *) smalloc(sizeof(s_fidoconfig));

   initConfig(config);

   while ((line = configline()) != NULL) {
      line = trimLine(line);
      line = stripComment(line);
      if (line[0] != 0) {
         line = shell_expand(line);
         parseLine(line, config);
      }
      free(line);
   }

   if (wasError == 1) {
      printf("Please correct above error(s) first!\n");
      fflush(stdout);
      exit(1);
   }
   close_conf();
   carbonNames2Addr(config);
   fixRoute(config);
   stripPktPwd(config);
   return config;
}

void freeArea(s_area area) {
    int i;
	nfree(area.areaName);
	nfree(area.fileName);
	nfree(area.description);
	nfree(area.group);
	for (i=0; i < area.downlinkCount; i++) nfree(area.downlinks[i]);
	nfree(area.downlinks);
}

void freeFileArea(s_filearea area) {
    int i;
	nfree(area.areaName);
	nfree(area.pathName);
	nfree(area.description);
	nfree(area.group);
	for (i=0; i < area.downlinkCount; i++) nfree(area.downlinks[i]);
	nfree(area.downlinks);
}

void freeBbsArea(s_bbsarea area) {
        nfree(area.areaName);
        nfree(area.pathName);
        nfree(area.description);
}

void freeSaveTic(s_savetic savetic) {
        nfree(savetic.fileAreaNameMask);
        nfree(savetic.pathName);
}

void disposeConfig(s_fidoconfig *config)
{
  int i;
  unsigned int j;

   nfree(config->name);
   nfree(config->sysop);
   nfree(config->location);

   nfree(config->addr);

   for (i=0; i < config->publicCount; i++) nfree(config->publicDir[i]);
   nfree(config->publicDir);

   for (i = 0; i< config->linkCount; i++) freeLink(&config->links[i]);
   nfree(config->links);

   freeLink(config->linkDefaults);
   nfree(config->linkDefaults);

   nfree(config->inbound);
   nfree(config->outbound);
   nfree(config->ticOutbound);
   nfree(config->protInbound);
   nfree(config->listInbound);
   nfree(config->localInbound);
   nfree(config->tempInbound);
   nfree(config->logFileDir);
   nfree(config->dupeHistoryDir);
   nfree(config->nodelistDir);
   nfree(config->msgBaseDir);
   nfree(config->magic);
   nfree(config->semaDir);
   nfree(config->badFilesDir);
   nfree(config->areafixhelp);
   nfree(config->tempOutbound);
   nfree(config->fileAreaBaseDir);
   nfree(config->passFileAreaDir);
   nfree(config->busyFileDir);
   for (j = 0; j < config->numPublicGroup; j++)
     nfree(config->PublicGroup[j]);
   nfree(config->PublicGroup);


   for (i = 0; i< config->netMailAreaCount; i++)
   freeArea(config->netMailAreas[i]);
   freeArea(config->dupeArea);
   freeArea(config->badArea);
   for (i = 0; i< config->echoAreaCount; i++)
   freeArea(config->echoAreas[i]);
   nfree(config->echoAreas);
   for (i = 0; i< config->fileAreaCount; i++)
   freeFileArea(config->fileAreas[i]);
   nfree(config->fileAreas);
   for (i = 0; i< config->bbsAreaCount; i++)
   freeBbsArea(config->bbsAreas[i]);
   nfree(config->bbsAreas);
   for (i = 0; i< config->localAreaCount; i++) freeArea(config->localAreas[i]);
   nfree(config->localAreas);

   for (i = 0; i < config->routeCount; i++) nfree(config->route[i].pattern);
   nfree(config->route);
   for (i = 0; i < config->routeFileCount; i++) nfree(config->routeFile[i].pattern);
   nfree(config->routeFile);
   for (i = 0; i < config->routeMailCount; i++) nfree(config->routeMail[i].pattern);
   nfree(config->routeMail);

   for (i = 0; i < config->remapCount; i++) 
       if (config->remaps[i].toname!=NULL)
          nfree(config->remaps[i].toname);
   nfree(config->remaps);

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
   nfree(config->msgidfile);
   nfree(config->loglevels);
   nfree(config->screenloglevels);
   nfree(config->echotosslog);
   nfree(config->statlog);
   nfree(config->lockfile);

   for (i = 0; i< config->carbonCount; i++) {
	   nfree(config->carbons[i].str);
	   nfree(config->carbons[i].areaName);
	   nfree(config->carbons[i].reason);
   }
   nfree(config->carbons);

   nfree(config->ReportTo);

   nfree(config->beforePack);
   nfree(config->afterUnpack);
   /* +AS+ */
   nfree(config->processPkt);
   /* -AS- */
   nfree(config->areafixSplitStr);
   nfree(config->areafixOrigin);
   nfree(config->fileLocalPwd);
   nfree(config->fileLDescString);

   for (i = 0; i< config->saveTicCount; i++) freeSaveTic(config->saveTic[i]);
   nfree(config->saveTic);

   for (i = 0; i < config->execonfileCount; i++) {
		nfree(config->execonfile[i].filearea);
		nfree(config->execonfile[i].filename);
		nfree(config->execonfile[i].command);
   }
   nfree(config->addToSeen);
   nfree(config->tearline);
   nfree(config->origin);

   free(config);
   config = NULL;
}

s_link *getLink(s_fidoconfig config, char *addr) {
   s_addr aka;
   UINT i;

   string2addr(addr, &aka);
   for (i = 0; i< config.linkCount; i++) {
      if (addrComp(aka, config.links[i].hisAka)==0) return &(config.links[i]);
   }

   return NULL;
}

s_link *getLinkFromAddr(s_fidoconfig config, s_addr aka)
{
   UINT i;

   for (i = 0; i <config.linkCount; i++) {
      if (addrComp(aka, config.links[i].hisAka)==0) return &(config.links[i]);
   }

   return NULL;
}

s_link *getLinkForArea(s_fidoconfig config, char *addr, s_area *area) {
	s_addr aka;
	UINT i;
	
	string2addr(addr, &aka);
	
	// we must find "right" link
	for (i = 0; i< config.linkCount; i++) {
		if (addrComp(aka, config.links[i].hisAka)==0 &&
			addrComp(*area->useAka, *config.links[i].ourAka)==0)
			return &(config.links[i]);
	}
	
	// backward compatibility
	for (i = 0; i< config.linkCount; i++) {
	    if (addrComp(aka, config.links[i].hisAka)==0) return &(config.links[i]);
	}
	
	return NULL;
}

s_link *getLinkForFileArea(s_fidoconfig config, char *addr, s_filearea *area) {
	s_addr aka;
	UINT i;
	
	string2addr(addr, &aka);
	
	// we must find "right" link
	for (i = 0; i< config.linkCount; i++) {
		if (addrComp(aka, config.links[i].hisAka)==0 &&
			addrComp(*area->useAka, *config.links[i].ourAka)==0)
			return &(config.links[i]);
	}
	
	// backward compatibility
	for (i = 0; i< config.linkCount; i++) {
	    if (addrComp(aka, config.links[i].hisAka)==0) return &(config.links[i]);
	}
	
	return NULL;
}

s_addr *getAddr(s_fidoconfig config, char *addr) {
   s_addr aka;
   UINT i;

   for (i = 0; i < config.addrCount; i++) {
      string2addr(addr, &aka);
      if (addrComp(aka, config.addr[i])==0) return &(config.addr[i]);
   }

   return NULL;
}

int existAddr(s_fidoconfig config, s_addr aka) {
   UINT i;

   for (i=0; i< config.addrCount; i++) {
      if (addrComp(aka, config.addr[i])==0) return 1;
   }
   return 0;
}

s_area *getEchoArea(s_fidoconfig *config, char *areaName)
{
   UINT i;

   for (i=0; i < config->echoAreaCount; i++) {
      if (stricmp(config->echoAreas[i].areaName, areaName)==0)
         return &(config->echoAreas[i]);
   }

   return &(config->badArea); // if all else fails, return badArea :-)
}

s_area *getArea(s_fidoconfig *config, char *areaName)
{
   UINT i;

   for (i=0; i < config->echoAreaCount; i++) {
      if (stricmp(config->echoAreas[i].areaName, areaName)==0)
         return &(config->echoAreas[i]);
   }

   for (i=0; i < config->localAreaCount; i++) {
      if (stricmp(config->localAreas[i].areaName, areaName)==0)
         return &(config->localAreas[i]);
   }

   return &(config->badArea); // if all else fails, return badArea :-)
}

s_area *getNetMailArea(s_fidoconfig *config, char *areaName)
{
   UINT i;

   if (areaName==NULL) return (NULL);

   for (i=0; i < config->netMailAreaCount; i++) {
      if (stricmp(config->netMailAreas[i].areaName, areaName)==0)
         return &(config->netMailAreas[i]);
   }
   return (NULL);
}

s_filearea *getFileArea(s_fidoconfig *config, char *areaName)
{
   UINT i;

   for (i=0; i < config->fileAreaCount; i++) {
      if (stricmp(config->fileAreas[i].areaName, areaName)==0)
         return &(config->fileAreas[i]);
   }

   return (NULL); // if all else fails, return NULL
}

int isLinkOfArea(s_link *link, s_area *area)
{
   int i;

   for (i = 0; i < area->downlinkCount; i++)
   {
      if (link == area->downlinks[i]->link) return 1;
   }
   return 0;
}

int isLinkOfFileArea(s_link *link, s_filearea *area)
{
   int i;

   for (i = 0; i < area->downlinkCount; i++)
   {
      if (link == area->downlinks[i]->link) return 1;
   }
   return 0;
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
