#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fidoconfig.h"
#include "typesize.h"
#include "common.h"

char *readLine(FILE *f)
{
   char *line = NULL, temp[81];
   size_t size = 81;

   line = (char *) malloc(size);
   if (fgets(line, 81, f) == NULL) {
      free(line);                      // end of file...
      return NULL;
   }

   while ((strlen(line) % 80) == 0) {
      if (fgets(temp, 81, f) == NULL) break; // eof encountered
      if (temp[strlen(temp)-1] == '\n') {
         temp[strlen(temp)-1] = 0; // kill \n
      }
      line = realloc(line, strlen(line)+strlen(temp)+1);
      strcat(line, temp);
   }

   if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = 0;
   return line;
}

char *trimLine(char *line)
{
   char *start = line, *temp;

   while ((*start == ' ') || (*start == '\t') || (*start == '\xfe')) start++;
   temp = (char *) malloc(strlen(start)+1);
   strcpy(temp, start);
   free(line);
   return temp;
}

void parseConfig(FILE *f, s_fidoconfig *config)
{
   char *line;

   actualLineNr = 1;
   while ((line = readLine(f))) {
      line = trimLine(line);
      if ((line[0] != '#') && (line[0] != 0)) {
         line = shell_expand(line);
         parseLine(line, config);
      }
      actualLineNr++;
   }
}

void initConfig(s_fidoconfig *config) {
   // set all to 0
   memset(config, sizeof(s_fidoconfig), 0);
}

char *getConfigFileName() {

   char *envFidoConfig = getenv("FIDOCONFIG");

   FILE *f = NULL;
   
#ifdef __linux__
   char *osSpecificName = "/etc/fido/config";
#elif __FreeBSD__
   char *osSpecificName = "/usr/local/etc/fido/config";
#elif OS2
   char *osSpecificName = "c:\\fido\\config";
#else
   char *osSpecificName = "fidoconfig";
#endif

   //try env-var fidoconfig
   if (envFidoConfig != NULL) f = fopen(envFidoConfig, "r");
   if (f == NULL) {
      //try osSpecificName
      f = fopen(osSpecificName, "r");
      if (f==NULL) return NULL;
      else return osSpecificName;
   } else return envFidoConfig;
}

s_fidoconfig *readConfig()
{
   FILE *f;
   s_fidoconfig *config;
   char *fileName = getConfigFileName();

   if (fileName == NULL) printf("Could not find Config-file\n");
   
   f = fopen(fileName, "r");
   
   if (f != NULL) {
      config = (s_fidoconfig *) malloc(sizeof(s_fidoconfig));

      config->includeCount = 1;
      config->includeFiles = realloc(config->includeFiles, sizeof(char *));
      config->includeFiles[0] = malloc(strlen(fileName)+1);
      strcpy(config->includeFiles[config->includeCount-1], fileName);
      
      initConfig(config);
      parseConfig(f, config);

      if (wasError == 1) {
         printf("Please correct above error(s) first!\n");
         fflush(stdout);
         exit(1);
      }
      return config;
   } else {
      printf("Could not find config-file!\n");
      return NULL;
   }
}

void freeArea(s_area area) {
	free(area.areaName);
	free(area.fileName);
	free(area.rwgrp);
	free(area.wgrp);
	free(area.rgrp);
}

void disposeConfig(s_fidoconfig *config)
{
   int i;

   free(config->name);
   free(config->sysop);
   free(config->location);

   free(config->addr);

   free(config->public);

   for (i = 0; i< config->linkCount; i++) {
	   free(config->links[i].hisAka.domain);
	   free(config->links[i].name);
	   if (config->links[i].pktPwd != config->links[i].defaultPwd)
	     free(config->links[i].pktPwd);
	   if (config->links[i].ticPwd != config->links[i].defaultPwd)	     
	     free(config->links[i].ticPwd);
	   if (config->links[i].areaFixPwd != config->links[i].defaultPwd)
	     free(config->links[i].areaFixPwd);
	   if (config->links[i].fileFixPwd != config->links[i].defaultPwd)
	     free(config->links[i].fileFixPwd);
	   if (config->links[i].bbsPwd != config->links[i].defaultPwd)
	     free(config->links[i].bbsPwd);
	   free(config->links[i].defaultPwd);
	   free(config->links[i].handle);
	   free(config->links[i].pktFile);
	   free(config->links[i].packFile);
	   free(config->links[i].TossGrp);
	   free(config->links[i].DenyGrp);
   }
   free(config->links);

   free(config->inbound);
   free(config->outbound);
   free(config->protInbound);
   free(config->listInbound);
   free(config->localInbound);
   free(config->logFileDir);
   free(config->dupeHistoryDir);
   free(config->nodelistDir);
   free(config->msgBaseDir);
   free(config->magic);
   free(config->areafixhelp);
   free(config->autoCreateDefaults);
   free(config->tempOutbound);

   freeArea(config->netMailArea);
   freeArea(config->dupeArea);
   freeArea(config->badArea);
   for (i = 0; i< config->echoAreaCount; i++) freeArea(config->echoAreas[i]);
   free(config->echoAreas);
   for (i = 0; i< config->localAreaCount; i++) freeArea(config->localAreas[i]);
   free(config->localAreas);

   for (i = 0; i < config->routeCount; i++) free(config->route[i].pattern);
   free(config->route);
   for (i = 0; i < config->routeFileCount; i++) free(config->routeFile[i].pattern);
   free(config->routeFile);
   for (i = 0; i < config->routeMailCount; i++) free(config->routeMail[i].pattern);
   free(config->routeMail);

   for (i = 0; i < config->packCount; i++) {
	   free(config->pack[i].packer);
	   free(config->pack[i].call);
   }
   free(config->pack);

   for (i = 0; i < config->unpackCount; i++) {
	   free(config->unpack[i].matchCode);
	   free(config->unpack[i].call);
   }
   free(config->unpack);

   free(config->intab);
   free(config->outtab);
   free(config->importlog);
   free(config->echotosslog);
   free(config->lockfile);

   for (i = 0; i< config->carbonCount; i++) free(config->carbons[i].str);
   free(config->carbons);

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

   for (i=0; i< config.linkCount; i++) {
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

s_area *getArea(s_fidoconfig *config, char *areaName)
{
   UINT i;

   for (i=0; i < config->echoAreaCount; i++) {
      if (stricmp(config->echoAreas[i].areaName, areaName)==0)
         return &(config->echoAreas[i]);
   }

   return &(config->badArea); // if all else fails, return badArea :-)
}

