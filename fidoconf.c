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

#if !defined(SHORTNAMES)
#include "fidoconf.h"
#else
#include "fidoconf.h"
#endif

#include "typesize.h"
#include "common.h"

char *readLine(FILE *f)
{
   char *line = NULL, temp[81];
   size_t size = 1024;

   line = (char *) malloc(size);
   if (fgets(line, 81, f) == NULL) {
      free(line);                      // end of file...
      return NULL;
   }

   if (line[strlen(line)-1] != '\n') {
     while ((strlen(line) % 80) == 0) {
       if (fgets(temp, 81, f) == NULL) break; // eof encountered
       line = realloc(line, strlen(line)+strlen(temp)+1);
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
   temp = (char *) malloc(strlen(start)+1);
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

void parseConfig(FILE *f, s_fidoconfig *config)
{
   char *line;

   actualLineNr = 1;
   while ((line = readLine(f)) != NULL) {
      line = trimLine(line);
      line = stripComment(line);
      if (line[0] != 0) {
	//	printf(line);
	//	printf("\n");
         line = shell_expand(line);
         parseLine(line, config);
      }
      actualLineNr++;
      free(line);
   }
}

void initConfig(s_fidoconfig *config) {
   // set all to 0
   memset(config, 0, sizeof(s_fidoconfig));
   config -> loguid = config -> loggid = config -> logperm = -1;
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
      osSpecificName = (char *) malloc(strlen(osSpecificPrefix)+strlen(configName)+2); // +1 - for training delimiter
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
               osSpecificName = malloc (i+1);
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

           if (!found) {
	      if ((cb->move != 2) && !cb->extspawn) // move==2 - delete
	              printf("Could not find area \"%s\" for carbon copy. Use BadArea\n", (config->carbons[i].areaName) ? config->carbons[i].areaName : "");
              config->carbons[i].area = &(config->badArea);
              config->carbons[i].export = 0;
           }
   }
}

s_fidoconfig *readConfig()
{
   FILE *f;
   s_fidoconfig *config;
   char *fileName = getConfigFileName();

   if (fileName == NULL) {
        printf("Could not find Config-file\n");
        exit(1);
   }

   f = fopen(fileName, "r");

   if (f != NULL) {
      config = (s_fidoconfig *) malloc(sizeof(s_fidoconfig));

      initConfig(config);

      config->includeCount = 1;
      config->includeFiles = realloc(config->includeFiles, sizeof(char *));
      config->includeFiles[0] = malloc(strlen(fileName)+1);
      strcpy(config->includeFiles[config->includeCount-1], fileName);

      parseConfig(f, config);

      if (wasError == 1) {
         printf("Please correct above error(s) first!\n");
         fflush(stdout);
         exit(1);
      }
      fclose(f);
      carbonNames2Addr(config);
      return config;
   } else {
      printf("Could not find config-file!\n");
      return NULL;
   }
}

void freeArea(s_area area) {
    int i;
	free(area.areaName);
	free(area.fileName);
	free(area.description);
	free(area.group);
	for (i=0; i < area.downlinkCount; i++) free(area.downlinks[i]);
	free(area.downlinks);
}

void freeFileArea(s_filearea area) {
    int i;
	free(area.areaName);
	free(area.pathName);
	free(area.description);
	free(area.group);
	for (i=0; i < area.downlinkCount; i++) free(area.downlinks[i]);
	free(area.downlinks);
}

void freeBbsArea(s_bbsarea area) {
        free(area.areaName);
        free(area.pathName);
        free(area.description);
}

void freeSaveTic(s_savetic savetic) {
        free(savetic.fileAreaNameMask);
        free(savetic.pathName);
}

void disposeConfig(s_fidoconfig *config)
{
  int i;
  unsigned int j;

   free(config->name);
   free(config->sysop);
   free(config->location);

   free(config->addr);

   for (i=0; i < config->publicCount; i++) free(config->publicDir[i]);
   free(config->publicDir);

   for (i = 0; i< config->linkCount; i++) freeLink(&config->links[i]);
   free(config->links);

   freeLink(config->linkDefaults);
   free(config->linkDefaults);

   free(config->inbound);
   free(config->outbound);
   free(config->ticOutbound);
   free(config->protInbound);
   free(config->listInbound);
   free(config->localInbound);
   free(config->tempInbound);
   free(config->logFileDir);
   free(config->dupeHistoryDir);
   free(config->nodelistDir);
   free(config->msgBaseDir);
   free(config->magic);
   free(config->semaDir);
   free(config->badFilesDir);
   free(config->areafixhelp);
   free(config->tempOutbound);
   free(config->fileAreaBaseDir);
   free(config->passFileAreaDir);
   for (j = 0; j < config->numPublicGroup; j++)
     free(config->PublicGroup[j]);
   free(config->PublicGroup);


   for (i = 0; i< config->netMailAreaCount; i++)
   freeArea(config->netMailAreas[i]);
   freeArea(config->dupeArea);
   freeArea(config->badArea);
   for (i = 0; i< config->echoAreaCount; i++)
   freeArea(config->echoAreas[i]);
   free(config->echoAreas);
   for (i = 0; i< config->fileAreaCount; i++)
   freeFileArea(config->fileAreas[i]);
   free(config->fileAreas);
   for (i = 0; i< config->bbsAreaCount; i++)
   freeBbsArea(config->bbsAreas[i]);
   free(config->bbsAreas);
   for (i = 0; i< config->localAreaCount; i++) freeArea(config->localAreas[i]);
   free(config->localAreas);

   for (i = 0; i < config->routeCount; i++) free(config->route[i].pattern);
   free(config->route);
   for (i = 0; i < config->routeFileCount; i++) free(config->routeFile[i].pattern);
   free(config->routeFile);

   for (i = 0; i < config->remapCount; i++) 
       if (config->remaps[i].toname!=NULL)
          free(config->remaps[i].toname);
   free(config->remaps);

   for (i = 0; i < config->nodelistCount; i++)
     {
       if (config->nodelists[i].nodelistName != NULL)
         free(config->nodelists[i].nodelistName);
       if (config->nodelists[i].diffUpdateStem != NULL)
         free(config->nodelists[i].diffUpdateStem);
       if (config->nodelists[i].fullUpdateStem != NULL)
         free(config->nodelists[i].fullUpdateStem);
     }
   free(config->nodelists);
   free(config->fidoUserList);

   for (i = 0; i < config->packCount; i++) {
           free(config->pack[i].packer);
           free(config->pack[i].call);
   }
   free(config->pack);

   for (i = 0; i < config->unpackCount; i++) {
           free(config->unpack[i].matchCode);
           free(config->unpack[i].mask);
           free(config->unpack[i].call);
   }
   free(config->unpack);

   for (i= 0; i < config->includeCount; i++) free(config->includeFiles[i]);
   free(config->includeFiles);

   free(config->intab);
   free(config->outtab);
   free(config->importlog);
   free(config->LinkWithImportlog);
   free(config->fileAreasLog);
   free(config->fileNewAreasLog);
   free(config->longNameList);
   free(config->fileArcList);
   free(config->filePassList);
   free(config->fileDupeList);
   free(config->msgidfile);
   free(config->loglevels);
   free(config->echotosslog);
   free(config->lockfile);

   for (i = 0; i< config->carbonCount; i++) {
	free(config->carbons[i].str);
	if (config->carbons[i].reason) free(config->carbons[i].reason);
   }
   free(config->carbons);

   free(config->ReportTo);

   free(config->beforePack);
   free(config->afterUnpack);
   /* +AS+ */
   free(config->processPkt);
   /* -AS- */
   free(config->areafixSplitStr);
   free(config->areafixOrigin);
   free(config->fileLocalPwd);
   free(config->fileLDescString);

   for (i = 0; i< config->saveTicCount; i++) freeSaveTic(config->saveTic[i]);
   free(config->saveTic);

   for (i = 0; i < config->execonfileCount; i++) {
		free(config->execonfile[i].filearea);
		free(config->execonfile[i].filename);
		free(config->execonfile[i].command);
   }

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

	for (i=0; i < len; i++) {
		if (strcmp(group, strarray[i])==0) return 1;
	}
	
	return 0;
}
