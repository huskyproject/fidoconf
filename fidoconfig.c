#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fidoconfig.h"
#include "typesize.h"

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

s_fidoconfig *readConfig()
{
   FILE *f;
   s_fidoconfig *config;

   // try env-var fidoconfig
   f = fopen(getenv("FIDOCONFIG"), "r");
   if (f==NULL) {
#ifdef __linux__
      f = fopen("/etc/fido/config", "r");
#elif __FreeBSD__
      f = fopen("/usr/local/etc/fido/config", "r");
#elif OS2
      f = fopen("c:\fido\config", "r");
#else
      f = fopen("fidoconfig", "r");
#endif
   }

   if (f != NULL) {
      config = (s_fidoconfig *) malloc(sizeof(s_fidoconfig));
      initConfig(config);
      parseConfig(f, config);
      if (isError == 1) {
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

void disposeConfig(s_fidoconfig *config)
{
   free(config->name);
   free(config->sysop);
   free(config->location);
   free(config->intab);
   free(config->outtab);
   free(config->importlog);
   free(config->echotosslog);
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

