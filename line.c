#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#include <msgapi.h>

#include "fidoconfig.h"

int testExpression(char *expr, char *str)
{
#ifdef USEREGEXP
   regex_t cExpr;
   int rc;
   rc = regcomp(&cExpr, expr, REG_ICASE | REG_NOSUB);
   if (rc!=0) return rc;
   rc = regexec(&cExpr, str, 0, NULL, 0);

   regfree(&cExpr);
   return rc;
#else
   return 0;
#endif
}

int parseVersion(char *token, s_fidoconfig *config)
{
   char buffer[10], *temp = token;
   int i = 0;

   // test
   i = testExpression("[0-9]+\\.[0-9]+", token);
   if (i!= 0 ) return i;

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

int parseName(char *token, s_fidoconfig *config)
{
   config->name = (char *) malloc(strlen(token)+1);
   strcpy(config->name, token);
   return 0;
}

int parseLocation(char *token, s_fidoconfig *config)
{
   config->location = (char *) malloc(strlen(token)+1);
   strcpy(config->location, token);
   return 0;
}

int parseSysop(char *token, s_fidoconfig *config)
{
   config->sysop = (char *) malloc(strlen(token)+1);
   strcpy(config->sysop, token);
   return 0;
}

int parseAddress(char *token, s_fidoconfig *config)
{
   char *aka;
   int rc = testExpression("[0-9]{1,5}:[0-9]{1,5]/[0-9]{1,5}(\\.[0-9]{0,5})?(@[a-z\\.])?", token);
   if (rc!=0) return rc;

   aka = strtok(token, " \t"); // only look at aka

   config->addr = realloc(config->addr, sizeof(s_addr)*(config->addrCount+1));
   string2addr(aka, &(config->addr[config->addrCount]));
   config->addrCount++;
   
   return 0;
}

int parsePath(char *token, char **var)
{
   *var = (char *) malloc(strlen(token)+1);
   strcpy(*var, token);
   return 0;
}

int parsePublic(char *token, s_fidoconfig *config)
{
   config->public = realloc(config->public, sizeof(char **)*config->publicCount);
   
   config->public[config->publicCount] = (char *) malloc(strlen(token)+1);
   strcpy(config->public[config->publicCount], token);
   
   config->publicCount++;
   return 0;
}

int parseAreaOption(char *option, s_area *area)
{
   char *error;
   
   if (stricmp(option, "p")) {
      area->purge = strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) return 1; // error occured;
   }
   return 0;
}

int parseArea(char *token, s_area *area)
{
   char *tok;
   int rc = 0;

   area->msgbType = MSGTYPE_SDM;

   tok = strtok(token, " \t");
   if (tok == NULL) return 1;         // if there is no areaname

   area->areaName= (char *) malloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) return 2;         // if there is no filename
   area->fileName = (char *) malloc(strlen(tok)+1);
   strcpy(area->fileName, tok);

   while ((tok = strtok(NULL, " \t"))!= NULL) {
      if (stricmp(tok, "Squish")==0) area->msgbType = MSGTYPE_SQUISH;
      else if(tok[0]=='-') rc = parseAreaOption(tok+1, area);          // parseOption without leading -
   }
   
   return rc;
}

int parseEchoArea(char *token, s_fidoconfig *config)
{
   config->echoAreas = realloc(config->echoAreas, sizeof(s_area)*(config->echoAreaCount+1));
   parseArea(token, &(config->echoAreas[config->echoAreaCount]));
   config->echoAreaCount++;
   return 0;
}

int parseLine(char *line, s_fidoconfig *config)
{
   char *token = strtok(line, " \t");
   int rc = 0;

   //printf("Parsing: %s\n", line);
   //printf("token: %s - %s\n", line, strtok(NULL, "\0"));
   if (stricmp(token, "version")==0) rc = parseVersion(strtok(NULL, "\0"), config);
   else if (stricmp(token, "name")==0) rc = parseName(strtok(NULL, "\0"), config);
   else if (stricmp(token, "location")==0) rc = parseLocation(strtok(NULL, "\0"), config);
   else if (stricmp(token, "sysop")==0) rc =parseSysop(strtok(NULL, "\0"), config);
   else if (stricmp(token, "address")==0) rc = parseAddress(strtok(NULL, "\0"), config);
   else if (stricmp(token, "inbound")==0) rc = parsePath(strtok(NULL, "\0"), &(config->inbound));
   else if (stricmp(token, "protinbound")==0) rc = parsePath(strtok(NULL, "\0"), &(config->protInbound));
   else if (stricmp(token, "listinbound")==0) rc = parsePath(strtok(NULL, "\0"), &(config->listInbound));
   else if (stricmp(token, "localinbound")==0) rc= parsePath(strtok(NULL, "\0"), &(config->localInbound));
   else if (stricmp(token, "outbound")==0) rc = parsePath(strtok(NULL, "\0"), &(config->outbound));
   else if (stricmp(token, "public")==0) rc = parsePublic(strtok(NULL, "\0"), config);
   else if (stricmp(token, "logFileDir")==0) rc = parsePath(strtok(NULL, "\0"), &(config->logFileDir));
   else if (stricmp(token, "dupeHistoryDir")==0) rc = parsePath(strtok(NULL, "\0"), &(config->dupeHistoryDir));
   else if (stricmp(token, "nodelistDir")==0) rc = parsePath(strtok(NULL, "\0"), &(config->nodelistDir));
   else if (stricmp(token, "magic")==0) rc = parsePath(strtok(NULL, "\0"), &(config->magic));
   else if (stricmp(token, "netmailArea")==0) rc = parseArea(strtok(NULL, "\0"),&(config->netMailArea));
   else if (stricmp(token, "dupeArea")==0) rc = parseArea(strtok(NULL, "\0"), &(config->dupeArea));
   else if (stricmp(token, "badArea")==0) rc = parseArea(strtok(NULL, "\0"), &(config->badArea));
   else if (stricmp(token, "echoArea")==0) rc = parseEchoArea(strtok(NULL, "\0"), config);
                                                          
   if (rc != 0) {
      printf("Error %d in: %s\n", rc, line);
      return rc;
   }

   return 0;
}