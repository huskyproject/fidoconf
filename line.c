#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "patmat.h"

#include "dir.h"

#include "fidoconfig.h"

#include <compiler.h>
#include <stamp.h>
#include <progprot.h>

char *actualKeyword, *actualLine;
int  actualLineNr;


char *getRestOfLine() {
   return stripLeadingChars(strtok(NULL, "\0"), " \t");
}

int copyString(char *str, char **pmem)
{
   if (str==NULL) {
      printf("Line %d: There is a parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
   *pmem = (char *) malloc(strlen(str)+1);
   strcpy(*pmem, str);
   return 0;
}

int parseVersion(char *token, s_fidoconfig *config)
{
   char buffer[10], *temp = token;
   int i = 0;

   // if there is no token return error...
   if (token==NULL) {
      printf("Line %d: There is a version number missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   while (isdigit(*temp)) {
      buffer[i] = *temp;
      i++; temp++;
   }
   buffer[i] = 0;

   config->cfgVersionMajor = atoi(buffer);

   temp++; // eat .
   i = 0;

   while (isdigit(*temp)) {
      buffer[i] = *temp;
      i++; temp++;
   }
   buffer[i] = 0;

   config->cfgVersionMinor = atoi(buffer);

   return 0;
}

int parseAddress(char *token, s_fidoconfig *config)
{
   char *aka;

   if (token==NULL) {
      printf("Line %d: There is an address missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
   aka = strtok(token, " \t"); // only look at aka
   if (aka == NULL) {
      printf("Line %d: There is an address missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->addr = realloc(config->addr, sizeof(s_addr)*(config->addrCount+1));
   string2addr(aka, &(config->addr[config->addrCount]));
   config->addrCount++;
   
   return 0;
}

int parsePath(char *token, char **var)
{
   char limiter;
   DIR  *dirent;

   if (token == NULL) {
      printf("Line %d: There is a path missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
#ifdef UNIX
   limiter = '/';
#else
   limiter = '\\';
#endif
   if (token[strlen(token)-1] == limiter) {
      *var = (char *) malloc(strlen(token)+1);
      strcpy(*var, token);
   } else {
      *var = (char *) malloc(strlen(token)+2);
      strcpy(*var, token);
      (*var)[strlen(token)] = limiter;
      (*var)[strlen(token)+1] = '\0';
   }

   dirent = opendir(*var);
   if (dirent == NULL) {
      printf("Line %d: Path %s not found!\n", actualLineNr, *var);
      return 1;
   }

   closedir(dirent);
   return 0;
}

int parsePublic(char *token, s_fidoconfig *config)
{
   char limiter;
   DIR  *dirent;
   
   if (token == NULL) {
      printf("Line %d: There is a path missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   config->public = realloc(config->public, sizeof(char *)*config->publicCount);

#ifdef UNIX
   limiter = '/';
#else
   limiter = '\\';
#endif

   if (token[strlen(token)-1] == limiter) {
      config->public[config->publicCount] = (char *) malloc(strlen(token)+1);
      strcpy(config->public[config->publicCount], token);
   } else {
      config->public[config->publicCount] = (char *) malloc(strlen(token)+2);
      strcpy(config->public[config->publicCount], token);
      (config->public[config->publicCount])[strlen(token)] = limiter;
      (config->public[config->publicCount])[strlen(token)+1] = '\0';
   }

   dirent = opendir(token);

   if (dirent == NULL) {
      printf("Line %d: Path %s not found!\n", actualLineNr, token);
      return 1;
   }

   closedir(dirent);

   config->publicCount++;
   return 0;
}

int parseAreaOption(s_fidoconfig config, char *option, s_area *area)
{
   char *error;
   char *token;
   
   if (stricmp(option, "p")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         printf("Line %d: Number is missing after -p in areaOptions!\n", actualLineNr);
         return 1;
      }
      area->purge = strtol(token, &error, 0);
      if ((error != NULL) && (*error != '\0')) {
         printf("Line %d: Number is wrong after -p in areaOptions!\n", actualLineNr);
         return 1;     // error occured;
      }
   }
   else if (stricmp(option, "m")==0) {
      area->max = strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) {
         return 1;     // error
      }
   }
   else if (stricmp(option, "a")==0) {
      token = strtok(NULL, " \t");
      area->useAka = getAddr(config, token);
      if (area->useAka == NULL) {
//         printf("!!! %s not found as address.\n", token);
         return 1;
      }
   }
   else if (stricmp(option, "tinysb")==0) area->tinySB = 1;
   else if (stricmp(option, "h")==0) area->hide = 1;
   else if (stricmp(option, "manual")==0) area->manual = 1;
   else if (stricmp(option, "nopause")==0) area->noPause = 1;
   else if (stricmp(option, "dupeCheck")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         printf("Lind %d: Missing dupeCheck parameter!\n", actualLineNr);
         return 1;
      }
      if (stricmp(token, "off")==0) area->dupeCheck = off;
      else if (stricmp(token, "move")==0) area->dupeCheck = move;
      else if (stricmp(token, "del")==0) area->dupeCheck = del;
      else {
         printf("Line %d: Wrong dupeCheck parameter!\n", actualLineNr);
         return 1; // error
      }
   }
   else if (stricmp(option, "dupehistory")==0) {
      area->dupeHistory = strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) return 1;    // error
   }
   else {
      printf("Line %d: There is an option missing after \"-\"!\n", actualLineNr);
      return 1;
   }
      
   return 0;
}

int parseArea(s_fidoconfig config, char *token, s_area *area)
{
   char *tok;
   int rc = 0;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
   memset(area, 0, sizeof(s_area));

   area->msgbType = MSGTYPE_SDM;
   area->useAka = &(config.addr[0]);

   tok = strtok(token, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a areaname missing after %s!\n", actualLineNr, actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) malloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a filename missing %s!\n", actualLineNr, actualLine);
      return 2;         // if there is no filename
   }
   if (stricmp(tok, "Passthrough") != 0) {
      // msgbase on disk
      area->fileName = (char *) malloc(strlen(tok)+1);
      strcpy(area->fileName, tok);
   } else {
      // passthrough area
      area->fileName = NULL;
      area->msgbType = MSGTYPE_PASSTHROUGH;
   }

   while ((tok = strtok(NULL, " \t"))!= NULL) {
      if (stricmp(tok, "Squish")==0) {
         if (area->msgbType == MSGTYPE_PASSTHROUGH) {
            printf("Line %d: Logical Defect!! You could not make a Squish Area Passthrough!\n", actualLineNr);
            rc += 1;
         }
         area->msgbType = MSGTYPE_SQUISH;
      }
      else if(tok[0]=='-') rc += parseAreaOption(config, tok+1, area);
      else if (isdigit(tok[0]) && (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*"))) {
         area->downlinks = realloc(area->downlinks, sizeof(s_link*)*(area->downlinkCount+1));
         area->downlinks[area->downlinkCount] = getLink(config, tok);
         if (area->downlinks[area->downlinkCount] == NULL) {
            printf("Line %d: Link for this area is not found!\n", actualLineNr);
            rc += 1;
         }
         area->downlinkCount++;
      }
      else {
         printf("Line %d: Error in areaOptions token=%s!\n", actualLineNr, tok);
         rc +=1;
      }
   }
   
   return rc;
}

int parseEchoArea(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
   config->echoAreas = realloc(config->echoAreas, sizeof(s_area)*(config->echoAreaCount+1));
   rc = parseArea(*config, token, &(config->echoAreas[config->echoAreaCount]));
   config->echoAreaCount++;
   return rc;
}

int parseLink(char *token, s_fidoconfig *config)
{
   if (token == NULL) {
      printf("Line %d: There is a name missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   
   config->links = realloc(config->links, sizeof(s_link)*(config->linkCount+1));
   memset(&(config->links[config->linkCount]), 0, sizeof(s_link));
   config->links[config->linkCount].name = (char *) malloc (strlen(token)+1);
   strcpy(config->links[config->linkCount].name, token);

   // if handle not given use name as handle
   if (config->links[config->linkCount].handle == NULL) config->links[config->linkCount].handle = config->links[config->linkCount].name;
   if (config->links[config->linkCount].ourAka == NULL) config->links[config->linkCount].ourAka = &(config->addr[0]);
   
   config->linkCount++;
   return 0;
}

int parsePWD(char *token, char **pwd) {
   
   if (token == NULL) {            // return empty password
      *pwd = (char *) malloc(1);
      (*pwd)[0] = '\0';
      return 0;
   }
   
   *pwd = (char *) malloc(9);
   strncpy(*pwd, token, 8);        // only use 8 characters of password
   (*pwd)[8] = '\0';
   if (strlen(token)>8) return 1;
   else return 0;
}

int parseHandle(char *token, s_fidoconfig *config) {
   if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->links[config->linkCount-1].handle = (char *) malloc (strlen(token)+1);
   strcpy(config->links[config->linkCount-1].handle, token);
   return 0;
}

int parseRoute(char *token, s_fidoconfig *config, s_route **route, UINT *count) {
   char *option;
   int  rc = 0;
   s_route *actualRoute;

   if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   *route = realloc(*route, sizeof(s_route)*(*count+1));
   actualRoute = &(*route)[*count];
   memset(actualRoute, 0, sizeof(s_route));

   option = strtok(token, " \t");

   if (option == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   while (option != NULL) {
      if (stricmp(option, "enc")==0) actualRoute->enc = 1;
      else if (stricmp(option, "noenc")==0) actualRoute->enc = 0;
      else if (stricmp(option, "hold")==0) actualRoute->flavour = hold;
      else if (stricmp(option, "normal")==0) actualRoute->flavour = normal;
      else if (stricmp(option, "crash")==0) actualRoute->flavour = crash;
      else if (stricmp(option, "direct")==0) actualRoute->flavour = direct;
      else if (stricmp(option, "immediate")==0) actualRoute->flavour = immediate;
      else if (stricmp(option, "hub")==0) actualRoute->routeVia = hub;
      else if (stricmp(option, "host")==0) actualRoute->routeVia = host;
      else if (stricmp(option, "boss")==0) actualRoute->routeVia = boss;
      else if (stricmp(option, "noroute")==0) actualRoute->routeVia = noroute;
      else if (isdigit(option[0]) || (option[0] == '*') || (option[0] == '?')) {
         if ((actualRoute->routeVia == 0) && (actualRoute->target == NULL))
            actualRoute->target = getLink(*config, option);
         else {
            if (actualRoute->pattern == NULL) {
               actualRoute->pattern = (char *) malloc(strlen(option)+1);
               strcpy(actualRoute->pattern, option);
               (*count)++;
            } else {
               // add new Route for additional patterns
               *route = realloc(*route, sizeof(s_route)*(*count+1));
               actualRoute = &(*route)[*count];
               memcpy(actualRoute, &(*route)[(*count)-1], sizeof(s_route));

               actualRoute->pattern = (char *) malloc(strlen(option)+1);
               strcpy(actualRoute->pattern, option);
               (*count)++;
            }
            
         }
         if (actualRoute->target == NULL) {
            printf("Line %d: Link not found in Route statement!\n", actualLineNr);
            rc = 2;
         }
      }
      option = strtok(NULL, " \t");
   }

   return rc;
}

int parsePack(char *line, s_fidoconfig *config) {

   char   *p, *c;
   s_pack pack;
   
   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   p = strtok(line, " \t");
   c = getRestOfLine();
   if ((p != NULL) && (c != NULL)) {

      // add new pack statement
      config->packCount++;
      realloc(config->pack, config->packCount * sizeof(s_pack));

      // fill new pack statement
      pack = config->pack[config->packCount-1];
      pack.packer = (char *) malloc(strlen(p)+1);
      strcpy(pack.packer, p);
      pack.call   = (char *) malloc(strlen(c)+1);
      strcpy(pack.call, c);

      return 0;
   } else {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
}

int parseUnpack(char *line, s_fidoconfig *config) {
   
}

int parseFileName(char *line, char **name) {
   char *token = strtok(line, " \t");
   if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   if (fexist(token)) {
      (*name) = malloc(strlen(token)+1);
      strcpy((*name), token);
   } else {
      printf("Line %d: File not found %s!\n", actualLineNr, token);
      return 2;
   }

   return 0;
}

int parseLine(char *line, s_fidoconfig *config)
{
   char *token, *temp;
   int rc = 0;

   actualLine = temp = (char *) malloc(strlen(line)+1);
   strcpy(temp, line);
      
   actualKeyword = token = strtok(temp, " \t");

   //printf("Parsing: %s\n", line);
   //printf("token: %s - %s\n", line, strtok(NULL, "\0"));
   if (token == NULL);
   else if (stricmp(token, "version")==0) rc = parseVersion(getRestOfLine(), config);
   else if (stricmp(token, "name")==0) rc = copyString(getRestOfLine(), &(config->name));
   else if (stricmp(token, "location")==0) rc = copyString(getRestOfLine(), &(config->location));
   else if (stricmp(token, "sysop")==0) rc = copyString(getRestOfLine(), &(config->sysop));
   else if (stricmp(token, "address")==0) rc = parseAddress(getRestOfLine(), config);
   else if (stricmp(token, "inbound")==0) rc = parsePath(getRestOfLine(), &(config->inbound));
   else if (stricmp(token, "protinbound")==0) rc = parsePath(getRestOfLine(), &(config->protInbound));
   else if (stricmp(token, "listinbound")==0) rc = parsePath(getRestOfLine(), &(config->listInbound));
   else if (stricmp(token, "localinbound")==0) rc= parsePath(getRestOfLine(), &(config->localInbound));
   else if (stricmp(token, "outbound")==0) rc = parsePath(getRestOfLine(), &(config->outbound));
   else if (stricmp(token, "public")==0) rc = parsePublic(getRestOfLine(), config);
   else if (stricmp(token, "logFileDir")==0) rc = parsePath(getRestOfLine(), &(config->logFileDir));
   else if (stricmp(token, "dupeHistoryDir")==0) rc = parsePath(getRestOfLine(), &(config->dupeHistoryDir));
   else if (stricmp(token, "nodelistDir")==0) rc = parsePath(getRestOfLine(), &(config->nodelistDir));
   else if (stricmp(token, "msgbasedir")==0) rc = parsePath(getRestOfLine(), &(config->msgBaseDir));
   else if (stricmp(token, "magic")==0) rc = parsePath(getRestOfLine(), &(config->magic));
   else if (stricmp(token, "netmailArea")==0) rc = parseArea(*config,getRestOfLine(),&(config->netMailArea));
   else if (stricmp(token, "dupeArea")==0) rc = parseArea(*config, getRestOfLine(), &(config->dupeArea));
   else if (stricmp(token, "badArea")==0) rc = parseArea(*config, getRestOfLine(), &(config->badArea));
   else if (stricmp(token, "echoArea")==0) rc = parseEchoArea(getRestOfLine(), config);
   else if (stricmp(token, "link")==0) rc = parseLink(getRestOfLine(), config);
   else if (stricmp(token, "password")==0) {
      rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].defaultPwd));
      // if another pwd is not known (yet), make it point to the defaultPWD
      if (config->links[config->linkCount-1].pktPwd == NULL) config->links[config->linkCount-1].pktPwd = config->links[config->linkCount-1].defaultPwd;
      if (config->links[config->linkCount-1].ticPwd == NULL) config->links[config->linkCount-1].ticPwd = config->links[config->linkCount-1].defaultPwd;
      if (config->links[config->linkCount-1].areaFixPwd == NULL) config->links[config->linkCount-1].areaFixPwd = config->links[config->linkCount-1].defaultPwd;
      if (config->links[config->linkCount-1].fileFixPwd == NULL) config->links[config->linkCount-1].fileFixPwd = config->links[config->linkCount-1].defaultPwd;
      if (config->links[config->linkCount-1].bbsPwd == NULL) config->links[config->linkCount-1].bbsPwd = config->links[config->linkCount-1].defaultPwd;
   }
   else if (stricmp(token, "aka")==0) {
      string2addr(getRestOfLine(), &(config->links[config->linkCount-1].hisAka));
      rc = 0;
   }
   else if (stricmp(token, "ouraka")==0) {
      rc = 0;
      config->links[config->linkCount-1].ourAka = getAddr(*config, getRestOfLine());
      if (config->links[config->linkCount-1].ourAka == NULL) rc = 2;
   }
   else if (stricmp(token, "autoAreaCreate")==0) {
      rc = 0;
      if (stricmp(getRestOfLine(), "on")==0) config->links[config->linkCount-1].autoAreaCreate = 1;
      else rc = 2;
   }
   else if (stricmp(token, "pktpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].pktPwd));
   else if (stricmp(token, "ticpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].ticPwd));
   else if (stricmp(token, "araefixpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].areaFixPwd));
   else if (stricmp(token, "filefixpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].fileFixPwd));
   else if (stricmp(token, "bbspwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].bbsPwd));
   else if (stricmp(token, "handle")==0) rc = parseHandle(getRestOfLine(), config);
   else if (stricmp(token, "route")==0) rc = parseRoute(getRestOfLine(), config, &(config->route), &(config->routeCount));
   else if (stricmp(token, "routeFile")==0) rc = parseRoute(getRestOfLine(), config, &(config->routeFile), &(config->routeFileCount));
   else if (stricmp(token, "routeMail")==0) rc = parseRoute(getRestOfLine(), config, &(config->routeMail), &(config->routeMailCount));

   else if (stricmp(token, "pack")==0) rc = parsePack(getRestOfLine(), config);

   else if (stricmp(token, "intab")==0) rc = parseFileName(getRestOfLine(), &(config->intab));
   else if (stricmp(token, "outtab")==0) rc = parseFileName(getRestOfLine(), &(config->outtab));

   else if (stricmp(token, "echotosslog")==0) rc = copyString(getRestOfLine(), &(config->echotosslog));
   else if (stricmp(token, "importlog")==0) rc = copyString(getRestOfLine(), &(config->importlog));
   
   else printf("Unrecognized line(%d): %s\n", actualLineNr, line);
                                                          
   if (rc != 0) {
      printf("Error %d (line %d): %s\n", rc, actualLineNr, line);
      printf("Please correct above error in config first!\n");
      fflush(stdout);
      exit(1);
      return rc;
   }

   return 0;
}
