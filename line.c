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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef UNIX
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif 

#include <limits.h>
#include <smapi/patmat.h>

#include "dirlayer.h"

#if !defined(MSDOS) || defined(__DJGPP__)
#include "fidoconfig.h"
#else
#include "fidoconf.h"
#endif
#include "common.h"
#include "typesize.h"

#include <smapi/compiler.h>
#include <smapi/stamp.h>
#include <smapi/progprot.h>

char *actualKeyword, *actualLine;
int  actualLineNr;
char wasError = 0;


char *getRestOfLine(void) {
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

char *getDescription(void) {
  char desc[81];
  char *token;
  char *tmp=NULL;
  int out=0;
  int length=0;
  
  desc[0]='\0';
  while ((out==0) && ((token=strtok(NULL," "))!=NULL)) {
    if ((length+=strlen(token))>80)
      out=1;
    else {
      strcat (desc,token);
      strcat (desc," ");
      if (token[strlen(token)-1]=='\"')
        out=2;
    }
  }
  switch (out) {
    case 0: printf ("Line %d: Error in area description!\n",actualLineNr);
            return NULL;
    case 1: printf ("Line %d: Area description too large!\n",actualLineNr);
            return NULL;
    case 2: /* '"' out... */
            desc[strlen(desc)-2]='\0';
            tmp=(char *) malloc (strlen(desc));
            strcpy(tmp,desc+1);
  }
  return tmp;
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

int parseRemap(char *token, s_fidoconfig *config)
{
   char *param;

   if (token==NULL) {
      printf("Line %d: There are all parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

 
   config->remaps=realloc(config->remaps,
                          (config->remapCount+1)*sizeof(s_remap));

   param = strtok(token, ",\t"); 
   if (param == NULL) {
      printf("Line %d: Missing Name or * after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if (strcmp(param,"*")!=0)
      { // Name for rerouting
      config->remaps[config->remapCount].toname=strdup(param);
      } 
     else
      config->remaps[config->remapCount].toname=NULL;

   param = strtok(NULL, ",\t"); 
   if (param == NULL) {
      printf("Line %d: Address or * missing after %s!\n", actualLineNr,actualKeyword);
      return 1;
   }
   
   if (strcmp(param,"*")==0)
      config->remaps[config->remapCount].oldaddr.zone=0;
     else
      string2addr(param, &(config->remaps[config->remapCount].oldaddr));

   param = strtok(NULL, " \t"); 
   if (param == NULL) {
      printf("Line %d: Address missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   string2addr(param, &(config->remaps[config->remapCount].newaddr));

   if (config->remaps[config->remapCount].toname==NULL &&
       config->remaps[config->remapCount].oldaddr.zone==0)
      {
      printf("Line %d: At least one of the Parameters must not be *\n",actualLineNr);
      return 1;
      }

   config->remapCount++;

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
   config->publicDir = realloc(config->publicDir, sizeof(char *)*(config->publicCount+1));

#ifdef UNIX
   limiter = '/';
#else
   limiter = '\\';
#endif

   if (token[strlen(token)-1] == limiter) {
      config->publicDir[config->publicCount] = (char *) malloc(strlen(token)+1);
      strcpy(config->publicDir[config->publicCount], token);
   } else {
      config->publicDir[config->publicCount] = (char *) malloc(strlen(token)+2);
      strcpy(config->publicDir[config->publicCount], token);
      (config->publicDir[config->publicCount])[strlen(token)] = limiter;
      (config->publicDir[config->publicCount])[strlen(token)+1] = '\0';
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

int parseOwner(char *token, unsigned int *uid, unsigned int *gid)
{
#ifdef UNIX
   struct passwd *pw;
   struct group *grp;
   char *name, *group, *p;
    
   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   p = strchr(token, '.');
   if (p) {
     *p = '\0';
     name = token; group = p + 1;
   } else {
     name = token; group = NULL;
   };

   if (name != NULL) {
	pw  = getpwnam(name);
   
  	if (*name && pw == NULL) {
		printf("Line %d: User name %s is unknown to OS !\n", actualLineNr, name);
		return 1;
	}
	*uid = pw ? pw -> pw_uid : -1 ;		

   };

   if (group != NULL) {
	grp = getgrnam(group);  

	if ((*group) && grp == NULL) {
		printf("Line %d: Group name %s is unknown to OS !\n", actualLineNr, group);
		return 1;
	}
	*gid = grp ? grp -> gr_gid : -1 ;		
   }
#endif   
   return 0;
}

int parseNumber(char *token, int radix, unsigned *level) {
    char *end = NULL;
    unsigned long result;

    if (token == NULL) {
	printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
	return 1;
    }

    result = strtoul(token, &end, radix);

    if (!(*end == '\0' && *token != '\0') || result == ULONG_MAX) {
	printf("Line %d: Error in number representation : %s . %s!\n", actualLineNr, token, end);
	return 1;
    }
	
    *level = (unsigned) result;
    return 0;
}

int parseAreaOption(const s_fidoconfig *config, char *option, s_area *area)
{
   char *error;
   char *token;
   int i;

   if (stricmp(option, "b")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         printf("Line %d: An msgbase type is missing after -b in areaOptions!\n", actualLineNr);
         return 1;
      }
      if ((stricmp(token, "Squish")!=0) && (stricmp(token, "Jam")!=0) && (stricmp(token, "Msg")!=0)) {
         printf ("Line %d: MsgBase type not valid after -b in areaOptions!\n", actualLineNr);
         return 1;
      }
      if (stricmp(token, "Squish")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
           printf("Line %d: Logical Defect!! You could not make a Squish Area Passthrough!\n", actualLineNr);
        }
        area->msgbType = MSGTYPE_SQUISH;
      }
      else if (stricmp(token, "Jam")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
           printf("Line %d: Logical Defect!! You could not make a Jam Area Passthrough!\n", actualLineNr);
        }
        area->msgbType = MSGTYPE_JAM;
      }
      else if (stricmp(token, "Msg")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
           printf("Line %d: Logical Defect!! You could not make a *.msg Area Passthrough!\n", actualLineNr);
        }
        area->msgbType = MSGTYPE_SDM;
      }
   }
   else if (stricmp(option, "p")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         printf("Line %d: Number is missing after -p in areaOptions!\n", actualLineNr);
         return 1;
      }
      area->purge = (UINT) strtol(token, &error, 0);
      if ((error != NULL) && (*error != '\0')) {
         printf("Line %d: Number is wrong after -p in areaOptions!\n", actualLineNr);
         return 1;     // error occured;
      }
   }
   else if (stricmp(option, "$m")==0) {
      area->max = (UINT) strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) {
         return 1;     // error
      }
   }
   else if (stricmp(option, "a")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL)
	{
	  printf("Line %d: Adress is missing after -a in areaOptions!\n", actualLineNr);
	  return 1;
	}
      area->useAka = getAddr(*config, token);
      if (area->useAka == NULL) {
         printf("Line %d: %s not found as address.\n", actualLineNr,  token);
         return 1;
      }
   }
   else if (stricmp(option, "lr")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           printf("Line %d: Number is missing after -lr in areaOptions!\n", actualLineNr);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           printf("Line %d: Number is wrong after -lr in areaOptions!\n", actualLineNr);
	   return 1;
       }
       area->levelread = (unsigned)atoi(token);
   }
   else if (stricmp(option, "lw")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           printf("Line %d: Number is missing after -lw in areaOptions!\n", actualLineNr);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           printf("Line %d: Number is wrong after -lw in areaOptions!\n", actualLineNr);
	   return 1;
       }
       area->levelwrite = (unsigned)atoi(token);
   }
   else if (stricmp(option, "tinysb")==0) area->tinySB = 1;
   else if (stricmp(option, "keepUnread")==0) area->keepUnread = 1; 
   else if (stricmp(option, "killRead")==0) area->killRead = 1; 
   else if (stricmp(option, "h")==0) area->hide = 1;
   else if (stricmp(option, "manual")==0) area->mandatory = 1;
   else if (stricmp(option, "nopause")==0) area->noPause = 1;
   else if (stricmp(option, "mandatory")==0) area->mandatory = 1;
   else if (stricmp(option, "dosfile")==0) area->DOSFile = 1;
   else if (stricmp(option, "dupeCheck")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         printf("Line %d: Missing dupeCheck parameter!\n", actualLineNr);
         return 1;
      }
      if (stricmp(token, "off")==0) area->dupeCheck = dcOff;
      else if (stricmp(token, "move")==0) area->dupeCheck = dcMove;
      else if (stricmp(token, "del")==0) area->dupeCheck = dcDel;
      else {
         printf("Line %d: Wrong dupeCheck parameter!\n", actualLineNr);
         return 1; // error
      }
   }
   else if (stricmp(option, "dupesize")==0) {
      area->dupeSize = (UINT) strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) return 1;    // error
   }
   else if (stricmp(option, "dupehistory")==0) {
      area->dupeHistory = (UINT) strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) return 1;    // error
   }
   else if (stricmp(option, "g")==0) {
          token = strtok(NULL, " \t");
//        printf("group - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
	  free(area->group);
      area->group = strdup(token);
   }
/*   else if (stricmp(option, "r")==0) {
          token = strtok(NULL, " \t");
//        printf("rgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->rgrp));
   }
   else if (stricmp(option, "w")==0) {
          token = strtok(NULL, " \t");
//        printf("wgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->wgrp));
   }
   else if (stricmp(option, "l")==0) {
          token = strtok(NULL, " \t");
//        printf("rwgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->rwgrp));
   }*/
   else if (stricmp(option, "ccoff")==0) area->ccoff=1;
   else if (stricmp(option, "keepsb")==0) area->keepsb=1;
   else if (stricmp(option, "$")==0) ;
   else if (stricmp(option, "0")==0) ;
   else if (stricmp(option, "d")==0) {
          if ((area->description=getDescription())==NULL)
            return 1;
   }
	else if (stricmp(option, "fperm")==0) {
			token = strtok(NULL, " \t");
         if (token==NULL) {
            printf("Line %d: Missing permission parameter!\n", actualLineNr);
				return 1;	
			} else
				return parseNumber(token, 8, &(area->fperm));
   }
	else if (stricmp(option, "fowner")==0) {
			token = strtok(NULL, " \t");
         if (token==NULL) 
            printf("Line %d: Missing ownership parameter!\n", actualLineNr);
			else
	   		return parseOwner(token, &(area->uid), &(area->gid));
	}
   else {
      printf("Line %d: There is an option missing after \"-\"!\n", actualLineNr);
      return 1;
   }

   return 0;
}

int parseFileAreaOption(const s_fidoconfig *config, char *option, s_filearea *area)
{
   char *token;
   int i;

   if (stricmp(option, "a")==0) {
      token = strtok(NULL, " \t");
      area->useAka = getAddr(*config, token);
      if (area->useAka == NULL) {
//         printf("!!! %s not found as address.\n", token);
         return 1;
      }
   }
   else if (stricmp(option, "lr")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           printf("Line %d: Number is missing after -lr in areaOptions!\n", actualLineNr);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           printf("Line %d: Number is wrong after -lr in areaOptions!\n", actualLineNr);
	   return 1;
       }
       area->levelread = (unsigned)atoi(token);
   }
   else if (stricmp(option, "lw")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           printf("Line %d: Number is missing after -lw in areaOptions!\n", actualLineNr);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           printf("Line %d: Number is wrong after -lw in areaOptions!\n", actualLineNr);
	   return 1;
       }
       area->levelwrite = (unsigned)atoi(token);
   }
   else if (stricmp(option, "h")==0) area->hide = 1;
   else if (stricmp(option, "manual")==0) area->mandatory = 1;
   else if (stricmp(option, "nopause")==0) area->noPause = 1;
   else if (stricmp(option, "nocrc")==0) area->noCRC = 1;
   else if (stricmp(option, "g")==0) {
          token = strtok(NULL, " \t");
//        printf("group - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
	  free(area->group);
      area->group = strdup(token);
   }
/*   else if (stricmp(option, "r")==0) {
          token = strtok(NULL, " \t");
//        printf("rgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->rgrp));
   }
   else if (stricmp(option, "w")==0) {
          token = strtok(NULL, " \t");
//        printf("wgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->wgrp));
   }
   else if (stricmp(option, "l")==0) {
          token = strtok(NULL, " \t");
//        printf("rwgrp - '%s'\n",token);
      if (token == NULL) {
                 return 1;
      }
      copyString(token, &(area->rwgrp));
   }*/
   else if (stricmp(option, "d")==0) {
          if ((area->description=getDescription())==NULL)
            return 1;
   }  
   else {
      printf("Line %d: There is an option missing after \"-\"!\n", actualLineNr);
      return 1;
   }

   return 0;
}

int parseLinkOption(s_arealink *alink, char *token)
{
    if (stricmp(token, "r")==0) alink->import = 0;
    else if (stricmp(token, "w")==0) alink->export = 0;
    else if (stricmp(token, "mn")==0) alink->mandatory = 1;
    else return 1;
    return 0;
}

int parseArea(const s_fidoconfig *config, char *token, s_area *area)
{
   s_link *link;
   s_arealink *arealink;
   char *tok;
   unsigned int rc = 0;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   memset(area, 0, sizeof(s_area));
   area->fperm = area->uid = area->gid = -1;

   area->msgbType = MSGTYPE_SDM;
   area->useAka = &(config->addr[0]);

   // set default parameters of dupebase
   area->dupeSize = 10;   /* 2^10=1024 dupes minimum*/
   area->dupeHistory = 7; /* 7 days */

   // set default group for reader
   area->group = (char*) malloc(sizeof(char)+1);
   strcpy(area->group, "0");

   // set defaults for MS-DOS
#ifdef MSDOS
   area->DOSFile = 1;
#endif

   tok = strtok(token, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a areaname missing after %s!\n", actualLineNr, actualKeyword);
      return 1;         // if there is no areaname
   }

   tok = strLower(tok);
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

   tok = strtok(NULL, " \t");
   
   while (tok != NULL) {
      if(tok[0]=='-') rc += parseAreaOption(config, tok+1, area);
      else if (isdigit(tok[0]) && (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*"))) {
         area->downlinks = realloc(area->downlinks, sizeof(s_arealink*)*(area->downlinkCount+1));
	 area->downlinks[area->downlinkCount] = (s_arealink*)calloc(1, sizeof(s_arealink));
         area->downlinks[area->downlinkCount]->link = getLink(*config, tok);
         if (area->downlinks[area->downlinkCount]->link == NULL) {
            printf("Line %d: Link for this area is not found!\n", actualLineNr);
            rc += 1;
	    return rc;
         }
	 
		 link = area->downlinks[area->downlinkCount]->link;
		 arealink = area->downlinks[area->downlinkCount];
		 
		 if (link->numOptGrp > 0) {
			 // default set export on, import on, mandatory off
			 arealink->export = 1;
			 arealink->import = 1;
			 arealink->mandatory = 0;
		 
			 if (grpInArray(area->group,link->optGrp,link->numOptGrp)) {
				 arealink->export = link->export;
				 arealink->import = link->import;
				 arealink->mandatory = link->mandatory;
			 }
			 
		 } else {
			 arealink->export = link->export;
			 arealink->import = link->import;
			 arealink->mandatory = link->mandatory;
		 }
		 if (area->mandatory) arealink->mandatory = 1;
		 if (link->level < area->levelread)	arealink->export=0;
		 if (link->level < area->levelwrite) arealink->import=0;
		 // paused link can't receive mail
		 if (link->Pause) arealink->export = 0;
		 
         area->downlinkCount++;
	 tok = strtok(NULL, " \t");
	 while (tok) {
		 if (tok[0]=='-') {
			 if (parseLinkOption(area->downlinks[area->downlinkCount-1], tok+1)) break;
			 tok = strtok(NULL, " \t");
		 } else break;
	 }
	 continue;
      }
      else {
		  printf("Line %d: Error in areaOptions token=%s!\n", actualLineNr, tok);
		  rc +=1;
      }
      tok = strtok(NULL, " \t");
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
   rc = parseArea(config, token, &(config->echoAreas[config->echoAreaCount]));
   config->echoAreaCount++;
   return rc;
}

int parseNetMailArea(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->netMailAreas = realloc(config->netMailAreas, sizeof(s_area)*(config->netMailAreaCount+1));
   rc = parseArea(config, token, &(config->netMailAreas[config->netMailAreaCount]));
   config->netMailAreaCount++;
   return rc;
}


int parseFileArea(const s_fidoconfig *config, char *token, s_filearea *area)
{
   char *tok;
   s_link *link;
   s_arealink *arealink;
   unsigned int rc = 0;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   memset(area, 0, sizeof(s_filearea));

   area->pass = 0;
   area->useAka = &(config->addr[0]);

   // set default group for reader
   area->group = (char*) malloc(sizeof(char)+1);
   strcpy(area->group, "0");

   tok = strtok(token, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a areaname missing after %s!\n", actualLineNr, actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) malloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a pathname missing %s!\n", actualLineNr, actualLine);
      return 2;         // if there is no filename
   }
   if (stricmp(tok, "Passthrough") != 0) {
      if (tok[strlen(tok)-1] == PATH_DELIM) {
         area->pathName = (char *) malloc(strlen(tok)+1);
         strcpy(area->pathName, tok);
      } else {
         area->pathName = (char *) malloc(strlen(tok)+2);
         strcpy(area->pathName, tok);
         area->pathName[strlen(tok)] = PATH_DELIM;
         area->pathName[strlen(tok)+1] = '\0';
      }
   } else {
      // passthrough area
      area->pathName = NULL;
      area->pass = 1;
   }

   tok = strtok(NULL, " \t");

   while (tok != NULL) {
      if(tok[0]=='-') rc += parseFileAreaOption(config, tok+1, area);
      else if (isdigit(tok[0]) && (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*"))) {
         area->downlinks = realloc(area->downlinks, sizeof(s_arealink*)*(area->downlinkCount+1));
         area->downlinks[area->downlinkCount] = (s_arealink*)calloc(1, sizeof(s_arealink));
         area->downlinks[area->downlinkCount]->link = getLink(*config, tok);
         if (area->downlinks[area->downlinkCount]->link == NULL) {
            printf("Line %d: Link for this area is not found!\n", actualLineNr);
            rc += 1;
            return rc;
         }
         link = area->downlinks[area->downlinkCount]->link;
		 arealink = area->downlinks[area->downlinkCount];

		 if (link->numOptGrp > 0) {
			 // default set export on, import on, mandatory off
			 arealink->export = 1;
			 arealink->import = 1;
			 arealink->mandatory = 0;
		 
			 if (grpInArray(area->group,link->optGrp,link->numOptGrp)) {
				 arealink->export = link->export;
				 arealink->import = link->import;
				 arealink->mandatory = link->mandatory;
			 }

		 } else {
			 arealink->export = link->export;
			 arealink->import = link->import;
			 arealink->mandatory = link->mandatory;
		 }
		 if (area->mandatory) arealink->mandatory = 1;
		 if (link->level < area->levelread)	arealink->export=0;
		 if (link->level < area->levelwrite) arealink->import=0;
		 // paused link can't receive mail
		 if (link->Pause) arealink->export = 0;

         area->downlinkCount++;
         tok = strtok(NULL, " \t");
         while (tok) {
            if (tok[0] == '-') {
               if (parseLinkOption(area->downlinks[area->downlinkCount-1], tok+1)) break;
               tok = strtok(NULL, " \t");
            } else break;
         } /* endwhile */
         continue;
      }
      else {
         printf("Line %d: Error in areaOptions token=%s!\n", actualLineNr, tok);
         rc +=1;
         return rc;
      }
      tok = strtok(NULL, " \t");
   }

   return rc;
}

int parseFileAreaStatement(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->fileAreas = realloc(config->fileAreas,
sizeof(s_filearea)*(config->fileAreaCount+1));
   rc = parseFileArea(config, token,
&(config->fileAreas[config->fileAreaCount]));
   config->fileAreaCount++;
   return rc;
}

int parseBbsArea(const s_fidoconfig *config, char *token, s_bbsarea *area)
{
   char *tok;
   int rc = 0;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   memset(area, 0, sizeof(s_bbsarea));

   tok = strtok(token, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a areaname missing after %s!\n", actualLineNr, actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) malloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a pathname missing %s!\n", actualLineNr, actualLine);
      return 2;         // if there is no filename
   }

   if (tok[strlen(tok)-1] == PATH_DELIM) {
      area->pathName = (char *) malloc(strlen(tok)+1);
      strcpy(area->pathName, tok);
   } else {
      area->pathName = (char *) malloc(strlen(tok)+2);
      strcpy(area->pathName, tok);
      area->pathName[strlen(tok)] = PATH_DELIM;
      area->pathName[strlen(tok)+1] = '\0';
   }

   tok = strtok(NULL, " \t");

   while (tok != NULL) {
      if (stricmp(tok, "-d")==0) {
          if ((area->description=getDescription())==NULL)
            rc += 1;
      }  
      else {
         printf("Line %d: Error in areaOptions token=%s!\n", actualLineNr, tok);
         rc +=1;
         return rc;
      }
      tok = strtok(NULL, " \t");
   }

   return rc;
}

int parseBbsAreaStatement(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->bbsAreas = realloc(config->bbsAreas,
sizeof(s_bbsarea)*(config->bbsAreaCount+1));
   rc = parseBbsArea(config, token,
&(config->bbsAreas[config->bbsAreaCount]));
   config->bbsAreaCount++;
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
   // set areafix default to on
   config->links[config->linkCount].AreaFix = 1;
   config->links[config->linkCount].FileFix = 1;

   // set defaults to export, import, mandatory
   config->links[config->linkCount].export = 1;
   config->links[config->linkCount].import = 1;
   config->links[config->linkCount].mandatory = 0;
   
   config->links[config->linkCount].fReqFromUpLink = 1;

   strcpy(config->links[config->linkCount].name, token);

   // if handle not given use name as handle
   if (config->links[config->linkCount].handle == NULL) config->links[config->linkCount].handle = config->links[config->linkCount].name;
   if (config->links[config->linkCount].ourAka == NULL) config->links[config->linkCount].ourAka = &(config->addr[0]);

   // by default headers of PKT and MSG in areafix request must be equal
   config->links[config->linkCount].allowPktAddrDiffer = 0;

   config->linkCount++;
   return 0;
}

int parseNodelist(char *token, s_fidoconfig *config)
{
   if (token == NULL) {
      printf("Line %d: There is a name missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->nodelists = realloc(config->nodelists, sizeof(s_nodelist)*(config->nodelistCount+1));
   memset(&(config->nodelists[config->nodelistCount]), 0, sizeof(s_nodelist));
   config->nodelists[config->nodelistCount].nodelistName =
     (char *) malloc (strlen(token)+1);
   strcpy(config->nodelists[config->nodelistCount].nodelistName, token);

   config->nodelists[config->nodelistCount].format = fts5000;

   config->nodelistCount++;
   return 0;
}

int parseExport(char *token, unsigned int *export) {
    if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
    }
    if (stricmp(token, "on")==0) *export = 1;
    else *export = 0;
    return 0;
}

int parseImport(char *token, unsigned int *import) {
    if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
    }
    if (stricmp(token, "on")==0) *import = 1;
    else *import = 0;
    return 0;
}

int parseAutoPause(char *token, unsigned *autoPause)
{
   char *ptr;

   if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   } /* endif */

   for (ptr = token; *ptr; ptr++) {
      if (!isdigit(*ptr)) {
         printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
         return 1;
      } /* endif */
   } /* endfor */

   *autoPause = (unsigned)atoi(token);

   return 0;
}

int parseMandatory(char *token, unsigned int *mandatory) {
    if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
    }
    if (stricmp(token, "on")==0) *mandatory = 1;
    else *mandatory = 0;
    return 0;
}

/*
int parseOptGrp(char *token, char **optgrp) {
    if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
    }
    copyString(token, optgrp);
    return 0;
}
*/

int parseUInt(char *token, unsigned int *uint) {

    if (token == NULL) {
	printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
	return 1;
    }
    sscanf(token, "%u", uint);
    return 0;
}

int parseOctal(char *token, unsigned int *octal) {

    if (token == NULL) {
	printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
	return 1;
    }
    sscanf(token, "%o", octal);
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
      else if (stricmp(option, "no-route")==0) actualRoute->routeVia = noroute;
      else if (isdigit(option[0]) || (option[0] == '*') || (option[0] == '?')) {
         if ((actualRoute->routeVia == 0) && (actualRoute->target == NULL))
            actualRoute->target = getLink(*config, option);
         else {
            if (actualRoute->pattern == NULL) {
	      actualRoute->pattern = (char *) malloc(strlen(option)+2+1); //2 for additional .0 if needed
	      strcpy(actualRoute->pattern, option);
	      if ((strchr(option, '.')==NULL) && (strchr(option, '*')==NULL)) {
		strcat(actualRoute->pattern, ".0");
	      }
	      (*count)++;
            } else {
               // add new Route for additional patterns
               *route = realloc(*route, sizeof(s_route)*(*count+1));
               actualRoute = &(*route)[*count];
               memcpy(actualRoute, &(*route)[(*count)-1], sizeof(s_route));

               actualRoute->pattern = (char *) malloc(strlen(option)+2+1);//2 for additional .0 if needed
               strcpy(actualRoute->pattern, option);
	       if ((strchr(option, '.')==NULL) && (strchr(option, '*')==NULL)) {
		 strcat(actualRoute->pattern, ".0");
	       }
               (*count)++;
            }

         }
         if ((actualRoute->target == NULL) && (actualRoute->routeVia == 0)) {
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
   s_pack *pack;

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   p = strtok(line, " \t");
   c = getRestOfLine();
   if ((p != NULL) && (c != NULL)) {

      // add new pack statement
      config->packCount++;
      config->pack = realloc(config->pack, config->packCount * sizeof(s_pack));

      // fill new pack statement
      pack = &(config->pack[config->packCount-1]);
      pack->packer = (char *) malloc(strlen(p)+1);
      strcpy(pack->packer, p);
      pack->call   = (char *) malloc(strlen(c)+1);
      strcpy(pack->call, c);
      if (strstr(pack->call, "$a")==NULL) {
         printf("Line %d: $a missing in pack statement %s!\n", actualLineNr, actualLine);
         return 2;
      }
      if (strstr(pack->call, "$f")==NULL) {
         printf("Line %d: $f missing in pack statement %s!\n", actualLineNr, actualLine);
         return 2;
      }

      return 0;
   } else {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
}

int parseUnpack(char *line, s_fidoconfig *config) {

    char   *p, *c;
    char   *error;
    s_unpack *unpack;
    UCHAR  code;
    int    i;

    if (line == NULL) {
       printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
       return 1;
    }

    // ToDo: Create replacement for strtok which handles "str"

    for (p = line; ((*p == ' ') || (*p == '\t')) && (*p != '\0'); p++);

    if (p != '\0') {
       if (*p == '\"')
          for (c = ++p; (*c != '\"') && (*c != '\0'); c++);
       else
          for (c = p; (*c != ' ') && (*c != '\t') && (*c != '\0'); c++);
       if (*c != '\0') {
          *c++ = '\0';
          stripLeadingChars(c, " \t");
       };
    } else
       c = NULL;

    if ((p != NULL) && (c != NULL)) {

       // add new pack statement
       config->unpackCount++;
       config->unpack = realloc(config->unpack, config->unpackCount * sizeof(s_unpack));

       // fill new pack statement
       unpack = &(config->unpack[config->unpackCount-1]);
       unpack->call   = (char *) malloc(strlen(p)+1);
       strcpy(unpack->call, p);

       if (strstr(unpack->call, "$a")==NULL) {
          printf("Line %d: $a missing in unpack statement %s!\n", actualLineNr, actualLine);
          return 2;
       }

       p = strtok(c, " \t"); // p is containing offset now
       c = strtok(NULL, " \t"); // t is containing match code now

       if ((p == NULL) || (c == NULL)) {
          printf("Line %d: offset or match code missing in unpack statement %s!\n", actualLineNr, actualLine);
          return 1;
       };

       unpack->offset = (UINT) strtol(p, &error, 0);

       if ((error != NULL) && (*error != '\0')) {
          printf("Line %d: Number is wrong for offset in unpack!\n", actualLineNr);
          return 1;     // error occured;
       }

       unpack->matchCode = (UCHAR *) malloc(strlen(c) / 2 + 1);
       unpack->mask      = (UCHAR *) malloc(strlen(c) / 2 + 1);

       // parse matchcode statement
       // this looks a little curvy, I know. Remember, I programmed this at 23:52 :)
       for (i = 0, error = NULL; c[i] != '\0' && error == NULL; i++) {
          code = toupper(c[i]);
          // if code equals to '?' set the corresponding bits  of  mask[] to 0
          unpack->mask[i / 2] = i % 2  == 0 ? (code != '?' ? 0xF0 : 0) :
                                unpack->mask[i / 2] | (code != '?' ? 0xF : 0);

          // find the numeric representation of hex code
          // if this is a '?' code equals to 0
          code = (isdigit(code) ? code - '0' :
                 (isxdigit(code) ? code - 'A' + 10 :
                 (code == '?' ? 0 : (error = c + i, 0xFF))));
          unpack->matchCode[i / 2] = i % 2 == 0 ? code << 4 : unpack->matchCode[i / 2] | code;
       }

       if (error) {
          printf("Line %d: matchCode can\'t contain %c in in unpack statement %s!\n", actualLineNr, *error, actualLine);
	            return 1;
       };

       if (i % 2 != 0)  {
          printf("Line %d: matchCode must be byte-aligned in unpack statement %s!\n", actualLineNr, actualLine);
          return 1;
       };

       unpack->codeSize = i / 2;

       return 0;
    } else {
       printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
       return 1;
    }
}

static int f_accessable(char *token)
{
    /* We don't need a real fexist function here, and we don't want to
       be dependent on SMAPI just because of this. For us, it is enough
       to see if the file is accessible */

#ifdef UNIX       
    struct stat sb;
    
    if (stat(token, &sb))
	return 0;  /* cannot stat the file */
    if (access(token, R_OK))
	return 0;  /* cannot access the file */
    return 1;
#else
    FILE *f = fopen(token, "rb");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
#endif
}

int parseFileName(char *line, char **name) {
   char *token;

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if (line[0]=='\"') {
     token=(char *) malloc (strlen(line)+1);
     sscanf(line,"\"%[^\"]s",token);
   }
   else
     token = strtok(line, " \t");     

   if (token == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
   if (f_accessable(token)) {
      (*name) = malloc(strlen(token)+1);
      strcpy((*name), token);
   } else {
      printf("Line %d: File not found or no permission: %s!\n", actualLineNr, token);
      if (line[0]=='\"')
        free(token);
      return 2;
   }
   if (line[0]=='\"')
     free(token);
   return 0;
}

int alreadyIncluded(char *line, s_fidoconfig *config)
{
   unsigned int i;

   for (i=0; i < config->includeCount; i++) {
      if (stricmp(config->includeFiles[i], line)==0) return 1;
   }

   return 0;
}

int parseInclude(char *line, s_fidoconfig *config)
{
   FILE *f;

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if ((f=fopen(line, "r")) == NULL) return 1;

   if (!alreadyIncluded(line, config)) {

      config->includeCount++;
      config->includeFiles = realloc(config->includeFiles, sizeof(char *) * config->includeCount);
      config->includeFiles[config->includeCount-1] = malloc(strlen(line)+1);
      strcpy(config->includeFiles[config->includeCount-1], line);

      free(actualLine);
      parseConfig(f, config);
      actualLine = NULL;  //FIXME: static Variable!!!
   } else {
      printf("Line %d: WARNING: recursive include of file %s detected and fixed!\n", actualLineNr, line);
   }

   fclose(f);
   return 0;
}

int parsePackerDef(char *line, s_fidoconfig *config, s_pack **packerDef) {

   int i;

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   for(i = 0; i < config->packCount; i++)
      if (stricmp(line, config->pack[i].packer)==0) {
         (*packerDef) = &(config->pack[i]);
         return 0;
      }

   printf("Line %d: Packer %s not found for packer statement!\n", actualLineNr, line);
   return 2;
}

int parseEchoMailFlavour(char *line, e_flavour *flavour) {

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if (stricmp(line, "hold")==0) *flavour = hold;
   else if (stricmp(line, "normal")==0) *flavour = normal;
   else if (stricmp(line, "direct")==0) *flavour = direct;
   else if (stricmp(line, "crash")==0) *flavour = crash;
   else if (stricmp(line, "immediate")==0) *flavour = immediate;
   else {
      printf("Line %d: Unknown echomail flavour %s!\n", actualLineNr, line);
      return 2;
   }
   return 0;
}

int parseFileEchoFlavour(char *line, e_flavour *flavour) {

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if (stricmp(line, "hold")==0) *flavour = hold;
   else if (stricmp(line, "normal")==0) *flavour = normal;
   else if (stricmp(line, "direct")==0) *flavour = direct;
   else if (stricmp(line, "crash")==0) *flavour = crash;
   else if (stricmp(line, "immediate")==0) *flavour = immediate;
   else {
      printf("Line %d: Unknown fileecho flavour %s!\n", actualLineNr, line);
      return 2;
   }
   return 0;
}

//and the parseGroup:
// i make some checking... maybe it is better check if the pointer exist from
// copyString function?

int parseGroup(char *token, s_fidoconfig *config, int i)
{
	unsigned int j;
	char *cpos;
	s_link *link = NULL;

	if (token == NULL)
		{
			fprintf(stderr, "Line %d: Parameter missing after %s!\n",
					actualLineNr, actualKeyword);
			return 1;
		}
	
	if ((config->linkCount < 1) && (i != 2)) {
		fprintf(stderr, "Line %d: You must define link before setting %s!\n",
				actualLineNr, actualKeyword);
		return 1;
    }
	if (i != 2) link = &config->links[config->linkCount-1];

	switch (i)
		{
		case 0:
			if (link->numAccessGrp != 0) {
				fprintf(stderr, "Line %d: Duplicate parameter after %s!\n",
						actualLineNr, actualKeyword);
				return 1;
			}
			break;

		case 1:
			if (link->LinkGrp != NULL) {
				fprintf(stderr, "Line %d: Duplicate parameter after %s!\n",
						actualLineNr, actualKeyword);
				return 1;
			}
			break;

		case 2:
			if (config->numPublicGroup != 0)
				{
					fprintf(stderr, "Line %d: Duplicate parameter after %s!\n",
							actualLineNr, actualKeyword);
					return 1;
				}
			break;

		case 3:
			if (link->numOptGrp != 0)
				{
					fprintf(stderr, "Line %d: Duplicate parameter after %s!\n",
							actualLineNr, actualKeyword);
					return 1;
				}
			break;
		}


	switch (i) {
	case 0:
		for (link->numAccessGrp = 0; *token != '\0'; link->numAccessGrp++) {
			link->AccessGrp = realloc (link->AccessGrp,
									   (link->numAccessGrp+1)*sizeof(char *));

			// strip leading spaces/tabs
			while ((*token == ' ') || (*token == '\t')) token++;

			cpos = strchr(token, ',');
			if (cpos != NULL) {
				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;
						
				link->AccessGrp[link->numAccessGrp]=malloc(cpos-token+1);
						
				for (j = 0; j < cpos - token; j++)
					link->AccessGrp[link->numAccessGrp][j] = token[j];
						
				link->AccessGrp[link->numAccessGrp][cpos - token] = '\0';
				token = cpos+1;
			}
			else {
				cpos = token + strlen(token);

				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;

				link->AccessGrp[link->numAccessGrp] = malloc(cpos - token + 1);

				for (j = 0; j < cpos - token; j++)
					link->AccessGrp[link->numAccessGrp][j] = token[j];

				link->AccessGrp[link->numAccessGrp][cpos - token] = '\0';
				token = cpos;
			}
		}
		break;

	case 1:
		copyString(token, &(config->links[config->linkCount-1].LinkGrp));
		break;
		
	case 2:
		for (config->numPublicGroup = 0; *token != '\0'; config->numPublicGroup++) {
			config->PublicGroup = realloc(config->PublicGroup,
										  (config->numPublicGroup+1)*sizeof(char *));

			// strip leading spaces/tabs
			while ((*token == ' ') || (*token == '\t')) token++;

			cpos = strchr(token, ',');
			if (cpos != NULL) {
				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;

				config->PublicGroup[config->numPublicGroup] = malloc(cpos - token + 1);

				for (j = 0; j < cpos - token; j++)
					config->PublicGroup[config->numPublicGroup][j] = token[j];

				config->PublicGroup[config->numPublicGroup][cpos - token] = '\0';
				token = cpos+1;
			} else {
				cpos = token + strlen(token);

				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;

				config->PublicGroup[config->numPublicGroup] = malloc(cpos - token + 1);

				for (j = 0; j < cpos - token; j++)
					config->PublicGroup[config->numPublicGroup][j] = token[j];

				config->PublicGroup[config->numPublicGroup][cpos - token] = '\0';
				token = cpos;
			}
		}
		break;

	case 3:
		for (link->numOptGrp = 0; *token != '\0'; link->numOptGrp++) {
			link->optGrp = realloc(link->optGrp, (link->numOptGrp+1)*sizeof(char *));

			// strip leading spaces/tabs
			while ((*token == ' ') || (*token == '\t')) token++;

			cpos = strchr(token, ',');
			if (cpos != NULL) {
				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;

				link->optGrp[link->numOptGrp] = malloc(cpos - token + 1);

				for (j = 0; j < cpos - token; j++)
					link->optGrp[link->numOptGrp][j] = token[j];

				link->optGrp[link->numOptGrp][cpos - token] = '\0';
				token = cpos+1;
			} else {
				cpos = token + strlen(token);

				// strip trailing spaces/tabs
				while (((*(cpos-1) == ' ') || (*(cpos-1) == '\t')) &&
					   (cpos > token)) cpos--;

				link->optGrp[link->numOptGrp] = malloc(cpos - token + 1);

				for (j = 0; j < cpos - token; j++)
					link->optGrp[link->numOptGrp][j] = token[j];

				link->optGrp[link->numOptGrp][cpos - token] = '\0';
				token = cpos;
			}
		}
		break;

	}
	return 0;
}

int parseLocalArea(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->localAreas = realloc(config->localAreas, sizeof(s_area)*(config->localAreaCount+1));
   rc = parseArea(config, token, &(config->localAreas[config->localAreaCount]));
   config->localAreaCount++;
   return rc;
}


int parseCarbon(char *token, s_fidoconfig *config, e_carbonType type)
{
   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->carbons = realloc(config->carbons,sizeof(s_carbon)*(config->carbonCount+1));
   config->carbonCount++;

   config->carbons[config->carbonCount-1].type = type;
   copyString(token, &(config->carbons[config->carbonCount-1].str));

   config->carbons[config->carbonCount-1].areaName = NULL;
   config->carbons[config->carbonCount-1].export = 0;
   config->carbons[config->carbonCount-1].move   = 0;
   config->carbons[config->carbonCount-1].reason = NULL;
   config->carbons[config->carbonCount-1].netMail = 0;

   return 0;
}

int parseCarbonArea(char *token, s_fidoconfig *config, int move) {

   if (token == NULL) {
	   printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
	   return 1;
   }
   
   copyString(token, &(config->carbons[config->carbonCount-1].areaName));
   config->carbons[config->carbonCount-1].extspawn = 0;
   config->carbons[config->carbonCount-1].move = move;
   return 0;
}

int parseCarbonDelete(char *token, s_fidoconfig *config) {

   if (token != NULL) {
	   printf("Line %d: There are extra parameters after %s!\n", actualLineNr, actualKeyword);
	   return 1;
   }
   config->carbons[config->carbonCount-1].areaName = NULL;
   config->carbons[config->carbonCount-1].move = 2;
   config->carbons[config->carbonCount-1].extspawn = 0;
   return 0;
}

int parseCarbonExtern(char *token, s_fidoconfig *config) {

   if (token == NULL) {
	   printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
	   return 1;
   }
   
   copyString(token, &(config->carbons[config->carbonCount-1].areaName));
   config->carbons[config->carbonCount-1].extspawn = 1;
   config->carbons[config->carbonCount-1].move = 0;
   /* +AS+ */
   if (tolower(*actualKeyword) == 'n')
     config->carbons[config->carbonCount-1].netMail = 1;
   else
     config->carbons[config->carbonCount-1].netMail = 0;
   /* -AS- */
   return 0;
}

int parseCarbonReason(char *token, s_fidoconfig *config) {

   if (token == NULL) {
	   printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
	   return 1;
   }
   
   copyString(token, &(config->carbons[config->carbonCount-1].reason));
   return 0;
}

int parseForwardPkts(char *token, s_fidoconfig *config, s_link *link)
{ 
   if (token == NULL) {
           printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
           return 1;
   }

   if (stricmp(token, "secure")==0) link->forwardPkts = fSecure;
   else if (stricmp(token, "on")==0) link->forwardPkts = fOn;
   else return 2;
   
   return 0;
}

int parseAllowEmptyPktPwd(char *token, s_fidoconfig *config, s_link *link)
{ 
   if (token == NULL) {
           printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
           return 1;
   }

   if (stricmp(token, "secure")==0) link->allowEmptyPktPwd = eSecure;
   else if (stricmp(token, "on")==0) link->allowEmptyPktPwd = eOn;
   else return 2;
   
   return 0;
}

int parseAllowPktAddrDiffer(char *token, s_fidoconfig *config, s_link *link)
{
   if (token == NULL) {
	   printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
	   return 1;
   }

   if (stricmp(token, "on")==0) link->allowPktAddrDiffer = pdOn;
   else if (stricmp(token, "off")==0) link->allowPktAddrDiffer = pdOff;
   else return 2;

   return 0;
}

int parseNodelistFormat(char *token, s_fidoconfig *config, s_nodelist *nodelist)
{
  if (token  == NULL) {
    printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
    return 1;
  }

  if (stricmp(token, "fts5000") == 0 || stricmp(token, "standard") == 0)
    nodelist->format = fts5000;
  else if (stricmp(token, "points24") == 0)
    nodelist->format = points24;
  else return 2;

  return 0;
}

int parseTypeDupes(char *line, e_typeDupeCheck *typeDupeBase, unsigned *DayAge) {

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   if (stricmp(line, "textdupes")==0) *typeDupeBase = textDupes;
   else if (stricmp(line, "hashdupes")==0) *typeDupeBase = hashDupes;
   else if (stricmp(line, "hashdupeswmsgid")==0) *typeDupeBase = hashDupesWmsgid;
   else if (stricmp(line, "commondupebase")==0) {
           *typeDupeBase = commonDupeBase;
           if (*DayAge==0) *DayAge=(unsigned) 5;
        }
   else {
      printf("Line %d: Unknown type base of dupes %s!\n", actualLineNr, line);
      return 2;
   }
   return 0;
}


int parseSaveTic(const s_fidoconfig *config, char *token, s_savetic *savetic)
{
   char *tok;
   DIR  *dirent;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   memset(savetic, 0, sizeof(s_savetic));

   tok = strtok(token, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a areaname mask missing after %s!\n", actualLineNr, actualKeyword);
      return 1;         // if there is no areaname mask
   }

   savetic->fileAreaNameMask= (char *) malloc(strlen(tok)+1);
   strcpy(savetic->fileAreaNameMask, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      printf("Line %d: There is a pathname missing %s!\n", actualLineNr, actualLine);
      return 2;         // if there is no filename
   }
      if (tok[strlen(tok)-1] == PATH_DELIM) {
         savetic->pathName = (char *) malloc(strlen(tok)+1);
         strcpy(savetic->pathName, tok);
      } else {
         savetic->pathName = (char *) malloc(strlen(tok)+2);
         strcpy(savetic->pathName, tok);
         savetic->pathName[strlen(tok)] = PATH_DELIM;
         savetic->pathName[strlen(tok)+1] = '\0';
      }

   dirent = opendir(savetic->pathName);
   if (dirent == NULL) {
      printf("Line %d: Path %s not found!\n", actualLineNr, savetic->pathName);
      return 2;
   }

   closedir(dirent);
   return 0;

}

int parseSaveTicStatement(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      printf("Line %d: There are parameters missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   config->saveTic = realloc(config->saveTic,sizeof(s_savetic)*(config->saveTicCount+1));
   rc = parseSaveTic(config, token,&(config->saveTic[config->saveTicCount]));
   config->saveTicCount++;
   return rc;
}

void printLinkError(void)
{
  printf("Line %d: You must define a link first before you use %s!\n", actualLineNr, actualKeyword);
}

int parseExecOnFile(char *line, s_fidoconfig *config) {
   char   *a, *f, *c;
   s_execonfile *execonfile;

   if (line == NULL) {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }

   a = strtok(line, " \t");
   f = strtok(NULL, " \t");
   c = getRestOfLine();
   if ((a != NULL) && (f != NULL) && (c != NULL)) {

      // add new execonfile statement
      config->execonfileCount++;
      config->execonfile = realloc(config->execonfile, config->execonfileCount * sizeof(s_execonfile));

      // fill new execonfile statement
      execonfile = &(config->execonfile[config->execonfileCount-1]);
      execonfile->filearea = (char *) malloc(strlen(a)+1);
      strcpy(execonfile->filearea, a);
      execonfile->filename = (char *) malloc(strlen(f)+1);
      strcpy(execonfile->filename, f);
      execonfile->command = (char *) malloc(strlen(c)+1);
      strcpy(execonfile->command, c);
      return 0;

   } else {
      printf("Line %d: Parameter missing after %s!\n", actualLineNr, actualKeyword);
      return 1;
   }
}

void printNodelistError(void)
{
  printf("Line %d: You must define a nodelist first before you use %s!\n", actualLineNr, actualKeyword);
}

int parseLine(char *line, s_fidoconfig *config)
{
   char *token, *temp;
   int rc = 0;
#ifdef __TURBOC__   
   int unrecognised = 0;
#endif   

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
   else if (stricmp(token, "tempinbound")==0) rc= parsePath(getRestOfLine(), &(config->tempInbound));
   else if (stricmp(token, "outbound")==0) rc = parsePath(getRestOfLine(), &(config->outbound));
   else if (stricmp(token, "ticoutbound")==0) rc = parsePath(getRestOfLine(), &(config->ticOutbound));
   else if (stricmp(token, "public")==0) rc = parsePublic(getRestOfLine(), config);
   else if (stricmp(token, "logfiledir")==0) rc = parsePath(getRestOfLine(), &(config->logFileDir));
   else if (stricmp(token, "dupehistorydir")==0) rc = parsePath(getRestOfLine(), &(config->dupeHistoryDir));
   else if (stricmp(token, "nodelistdir")==0) rc = parsePath(getRestOfLine(), &(config->nodelistDir));
   else if (stricmp(token, "fileareabasedir")==0) rc = parsePath(getRestOfLine(), &(config->fileAreaBaseDir));
   else if (stricmp(token, "passfileareadir")==0) rc = parsePath(getRestOfLine(), &(config->passFileAreaDir));
   else if (stricmp(token, "msgbasedir")==0) {
      temp = getRestOfLine();
      if (stricmp(temp, "passthrough")==0)
         copyString(temp, &(config->msgBaseDir));
      else
         rc = parsePath(temp, &(config->msgBaseDir));
   }
   else if (stricmp(token, "magic")==0) rc = parsePath(getRestOfLine(), &(config->magic));
   else if (stricmp(token, "semadir")==0) rc = parsePath(getRestOfLine(), &(config->semaDir));
   else if (stricmp(token, "badfilesdir")==0) rc = parsePath(getRestOfLine(), &(config->badFilesDir));
   else if ((stricmp(token, "netMailarea")==0) ||
	    (stricmp(token, "netarea")==0))
     rc = parseNetMailArea(getRestOfLine(), config);
   else if (stricmp(token, "dupearea")==0) rc = parseArea(config, getRestOfLine(), &(config->dupeArea));
   else if (stricmp(token, "badarea")==0) rc = parseArea(config, getRestOfLine(), &(config->badArea));
   else if (stricmp(token, "echoarea")==0) rc = parseEchoArea(getRestOfLine(), config);
   else if (stricmp(token, "filearea")==0) rc = parseFileAreaStatement(getRestOfLine(), config);
   else if (stricmp(token, "bbsarea")==0) rc = parseBbsAreaStatement(getRestOfLine(), config);
   else if (stricmp(token, "localarea")==0) rc = parseLocalArea(getRestOfLine(), config);
   else if (stricmp(token, "remap")==0) rc = parseRemap(getRestOfLine(),config);
   else if (stricmp(token, "link")==0) rc = parseLink(getRestOfLine(), config);
   else if (stricmp(token, "password")==0) {
     if (config->linkCount > 0) {
       rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].defaultPwd));
       // if another pwd is not known (yet), make it point to the defaultPWD
       if (config->links[config->linkCount-1].pktPwd == NULL) config->links[config->linkCount-1].pktPwd = config->links[config->linkCount-1].defaultPwd;
       if (config->links[config->linkCount-1].ticPwd == NULL) config->links[config->linkCount-1].ticPwd = config->links[config->linkCount-1].defaultPwd;
       if (config->links[config->linkCount-1].areaFixPwd == NULL) config->links[config->linkCount-1].areaFixPwd = config->links[config->linkCount-1].defaultPwd;
       if (config->links[config->linkCount-1].fileFixPwd == NULL) config->links[config->linkCount-1].fileFixPwd = config->links[config->linkCount-1].defaultPwd;
       if (config->links[config->linkCount-1].bbsPwd == NULL) config->links[config->linkCount-1].bbsPwd = config->links[config->linkCount-1].defaultPwd;
       if (config->links[config->linkCount-1].sessionPwd == NULL) config->links[config->linkCount-1].sessionPwd = config->links[config->linkCount-1].defaultPwd;
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "aka")==0) {
     if (config->linkCount > 0) {
       string2addr(getRestOfLine(), &(config->links[config->linkCount-1].hisAka));
       rc = 0;
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "ouraka")==0) {
      rc = 0;
      if (config->linkCount > 0) {
	config->links[config->linkCount-1].ourAka = getAddr(*config, getRestOfLine());
	if (config->links[config->linkCount-1].ourAka == NULL) rc = 2;
      }
      else {
	printLinkError();
	rc = 1;
      }
   }
   else if (stricmp(token, "autoareacreate")==0) {
      rc = 0;
      if (config->linkCount > 0) {
        temp = getRestOfLine();
	if (temp && stricmp(temp, "on")==0) config->links[config->linkCount-1].autoAreaCreate = 1;
	else if (!temp || stricmp(temp, "off")!=0) rc = 2;
      }
      else {
	printLinkError();
	rc = 1;
      }
   }
   else if (stricmp(token, "autofilecreate")==0) {
      rc = 0;
      if (config->linkCount > 0) {
        temp = getRestOfLine();
	if (temp && stricmp(temp, "on")==0) config->links[config->linkCount-1].autoFileCreate = 1;
	else if (!temp || stricmp(temp, "off")!=0) rc = 2;
      }
      else {
	printLinkError();
	rc = 1;
      }
   }
   else if (stricmp(token, "forwardrequests")==0) {
      rc = 0;
      if (config->linkCount > 0) {
	if (stricmp(getRestOfLine(), "on")==0) config->links[config->linkCount-1].forwardRequests = 1;
	else rc = 2;
      } 
      else {
	printLinkError();
	rc = 1;
      }
   }
   else if (stricmp(token, "frequestfromuplink") == 0) {
       rc = 0;
       if (config->linkCount > 0) {
           token = strtok(NULL, " \t");
	   if (token == NULL) rc = 1;
           else {
	       if (stricmp(token, "on") == 0) config->links[config->linkCount-1].fReqFromUpLink = 1;
	       else if (stricmp(token, "off") == 0) config->links[config->linkCount-1].fReqFromUpLink = 0;
	       else rc = 2;
	   }
       } else {
           printLinkError();
	   rc = 1;
       }
   }
   else if (stricmp(token, "forwardpkts")==0) {
     if (config->linkCount > 0) {
      rc = parseForwardPkts(getRestOfLine(), config, &(config->links[config->linkCount-1]));
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "allowemptypktpwd")==0) {
     if (config->linkCount > 0) {
      rc = parseAllowEmptyPktPwd(getRestOfLine(), config, &(config->links[config->linkCount-1]));
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "allowpktaddrdiffer")==0) {
     if (config->linkCount > 0) {
      rc = parseAllowPktAddrDiffer(getRestOfLine(), config, &(config->links[config->linkCount-1]));
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "autoareacreatedefaults")==0) {
     if (config->linkCount > 0){
       rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].autoAreaCreateDefaults));
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "autofilecreatedefaults")==0) {
     if (config->linkCount > 0){
       rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].autoFileCreateDefaults));
     }
     else {
       printLinkError();
       rc = 1;
     }
   }
//   else if (stricmp(token, "autoFileCreateDefaults")==0) rc = copyString(getRestOfLine(), &(config->autoFileCreateDefaults));
   else if (stricmp(token, "areafix")==0) {
          rc = 0;
          if (stricmp(getRestOfLine(), "off")==0) config->links[config->linkCount-1].AreaFix = 0;
      else rc = 2;
   }
   else if (stricmp(token, "filefix")==0) {
          rc = 0;
          if (stricmp(getRestOfLine(), "off")==0) config->links[config->linkCount-1].FileFix = 0;
      else rc = 2;
   }
   else if (stricmp(token, "pause")==0) { /* what happens with linkCount = 0??? */
     config->links[config->linkCount-1].Pause = 1;
     rc = 0;
   }
   else if (stricmp(token, "notic")==0) {
     if (config->linkCount > 0) {
       config->links[config->linkCount-1].noTIC = 1;
       rc = 0;
     } 
     else {
       printLinkError();
       rc = 1;
     }
   }
   else if (stricmp(token, "autopause")==0) rc = parseAutoPause(getRestOfLine(), &(config->links[config->linkCount-1].autoPause));
   else if (stricmp(token, "remoterobotname")==0) rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].RemoteRobotName));
   else if (stricmp(token, "export")==0) rc = parseExport(getRestOfLine(), &(config->links[config->linkCount-1].export));
   else if (stricmp(token, "import")==0) rc = parseImport(getRestOfLine(), &(config->links[config->linkCount-1].import));
   else if (stricmp(token, "mandatory")==0) rc = parseMandatory(getRestOfLine(), &(config->links[config->linkCount-1].mandatory));
   else if (stricmp(token, "manual")==0) rc = parseMandatory(getRestOfLine(), &(config->links[config->linkCount-1].mandatory));
   else if (stricmp(token, "optgrp")==0) rc = parseGroup(getRestOfLine(), config, 3);
   else if (stricmp(token, "level")==0) rc = parseNumber(getRestOfLine(), 10, &(config->links[config->linkCount-1].level));
#ifdef __TURBOC__
   else unrecognised++;
#else   
   else
#endif       
       if (stricmp(token, "arcmailsize")==0) rc = parseNumber(getRestOfLine(), 10, &(config->links[config->linkCount-1].arcmailSize));
   else if (stricmp(token, "pktpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].pktPwd));
   else if (stricmp(token, "ticpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].ticPwd));
   else if (stricmp(token, "areafixpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].areaFixPwd));
   else if (stricmp(token, "filefixpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].fileFixPwd));
   else if (stricmp(token, "bbspwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].bbsPwd));
   else if (stricmp(token, "sessionpwd")==0) rc = parsePWD(getRestOfLine(), &(config->links[config->linkCount-1].sessionPwd));
   else if (stricmp(token, "handle")==0) rc = parseHandle(getRestOfLine(), config);
       else if (stricmp(token, "email")==0) rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].email));
   else if (stricmp(token, "echomailflavour")==0) rc = parseEchoMailFlavour(getRestOfLine(), &(config->links[config->linkCount-1].echoMailFlavour));
   else if (stricmp(token, "fileechoflavour")==0) rc = parseFileEchoFlavour(getRestOfLine(), &(config->links[config->linkCount-1].fileEchoFlavour));
   else if (stricmp(token, "route")==0) rc = parseRoute(getRestOfLine(), config, &(config->route), &(config->routeCount));
   else if (stricmp(token, "routefile")==0) rc = parseRoute(getRestOfLine(), config, &(config->routeFile), &(config->routeFileCount));
   else if (stricmp(token, "routemail")==0) rc = parseRoute(getRestOfLine(), config, &(config->routeMail), &(config->routeMailCount));

   else if (stricmp(token, "pack")==0) rc = parsePack(getRestOfLine(), config);
   else if (stricmp(token, "unpack")==0) rc = parseUnpack(getRestOfLine(), config);
   else if (stricmp(token, "packer")==0) rc = parsePackerDef(getRestOfLine(), config, &(config->links[config->linkCount-1].packerDef));

   else if (stricmp(token, "intab")==0) rc = parseFileName(getRestOfLine(), &(config->intab));
   else if (stricmp(token, "outtab")==0) rc = parseFileName(getRestOfLine(), &(config->outtab));

   else if (stricmp(token, "areafixhelp")==0) rc = parseFileName(getRestOfLine(), &(config->areafixhelp));
   else if (stricmp(token, "filefixhelp")==0) rc = parseFileName(getRestOfLine(), &(config->filefixhelp));
   else if (stricmp(token, "forwardrequestfile")==0) rc = parseFileName(getRestOfLine(), &(config->links[config->linkCount-1].forwardRequestFile));
   else if (stricmp(token, "autoareacreatefile")==0) rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].autoAreaCreateFile));
   else if (stricmp(token, "autofilecreatefile")==0) rc = copyString(getRestOfLine(), &(config->links[config->linkCount-1].autoFileCreateFile));


   else if (stricmp(token, "echotosslog")==0) rc = copyString(getRestOfLine(), &(config->echotosslog));
   else if (stricmp(token, "importlog")==0) rc = copyString(getRestOfLine(), &(config->importlog));
   else if (stricmp(token, "linkwithimportlog")==0) rc = copyString(getRestOfLine(), &(config->LinkWithImportlog));
   else if (stricmp(token, "fileareaslog")==0) rc = parseFileName(getRestOfLine(), &(config->fileAreasLog));
   else if (stricmp(token, "filenewareaslog")==0) rc = parseFileName(getRestOfLine(), &(config->fileNewAreasLog));
   else if (stricmp(token, "longnamelist")==0) rc = parseFileName(getRestOfLine(), &(config->longNameList));
   else if (stricmp(token, "filearclist")==0) rc = parseFileName(getRestOfLine(), &(config->fileArcList));
   else if (stricmp(token, "filepasslist")==0) rc = parseFileName(getRestOfLine(), &(config->filePassList));
   else if (stricmp(token, "filedupelist")==0) rc = parseFileName(getRestOfLine(), &(config->fileDupeList));
   else if (stricmp(token, "msgidfile")==0) rc = parseFileName(getRestOfLine(), &(config->fileDupeList));
   else if (stricmp(token, "loglevels")==0) rc = copyString(getRestOfLine(), &(config->loglevels));
   else if (stricmp(token, "include")==0) rc = parseInclude(getRestOfLine(), config);

   else if (stricmp(token, "accessgrp")==0) rc = parseGroup(getRestOfLine(), config, 0);
   else if (stricmp(token, "linkgrp")==0) rc = parseGroup(getRestOfLine(), config, 1);

   else if (stricmp(token, "carbonto")==0) rc = parseCarbon(getRestOfLine(),config, to);
   else if (stricmp(token, "carbonfrom")==0) rc = parseCarbon(getRestOfLine(), config, from);
   else if (stricmp(token, "carbonkludge")==0) rc = parseCarbon(getRestOfLine(), config, kludge);
   else if (stricmp(token, "carbonsubj")==0) rc = parseCarbon(getRestOfLine(), config, subject);
   else if (stricmp(token, "carbontext")==0) rc = parseCarbon(getRestOfLine(), config, msgtext);
   else if (stricmp(token, "carbonarea")==0) rc = parseCarbonArea(getRestOfLine(), config, 0);
   else if (stricmp(token, "carboncopy")==0) rc = parseCarbonArea(getRestOfLine(), config, 0);
   else if (stricmp(token, "carbonmove")==0) rc = parseCarbonArea(getRestOfLine(), config, 1);
   else if (stricmp(token, "carbonextern")==0) rc = parseCarbonExtern(getRestOfLine(), config);
   /* +AS+ */
   else if (stricmp(token, "netmailextern")==0) rc = parseCarbonExtern(getRestOfLine(), config);
   /* -AS- */
   else if (stricmp(token, "carbondelete")==0) rc = parseCarbonDelete(getRestOfLine(), config);
   else if (stricmp(token, "carbonreason")==0) rc = parseCarbonReason(getRestOfLine(), config);
   
   else if (stricmp(token, "lockfile")==0) rc = copyString(getRestOfLine(), &(config->lockfile));
   else if (stricmp(token, "tempoutbound")==0) rc = parsePath(getRestOfLine(), &(config->tempOutbound));
   else if (stricmp(token, "areafixfrompkt")==0) config->areafixFromPkt = 1;
   else if (stricmp(token, "areafixkillreports")==0) config->areafixKillReports = 1;
   else if (stricmp(token, "areafixkillrequests")==0) config->areafixKillRequests = 1;
   else if (stricmp(token, "filefixkillreports")==0) config->filefixKillReports = 1;
   else if (stricmp(token, "filefixkillrequests")==0) config->filefixKillRequests = 1;
   else if (stricmp(token, "createdirs")==0) config->createDirs = 1;
   else if (stricmp(token, "longdirnames")==0) config->longDirNames = 1;
   else if (stricmp(token, "splitdirs")==0) config->splitDirs = 1;
   else if (stricmp(token, "adddlc")==0) config->addDLC = 1;
   else if (stricmp(token, "filesingledescline")==0) config->fileSingleDescLine = 1;
   else if (stricmp(token, "filecheckdest")==0) config->fileCheckDest = 1;
   else if (stricmp(token, "publicgroup")==0) rc = parseGroup(getRestOfLine(), config, 2);
   else if (stricmp(token, "logechotoscreen")==0) config->logEchoToScreen = 1;
   else if (stricmp(token, "separatebundles")==0) config->separateBundles = 1;
   else if (stricmp(token, "carbonandquit")==0) config->carbonAndQuit = 1;
   else if (stricmp(token, "carbonkeepsb")==0) config->carbonKeepSb = 1;
   else if (stricmp(token, "ignorecapword")==0) config->ignoreCapWord = 1;
   else if (stricmp(token, "noprocessbundles")==0) config->noProcessBundles = 1;
   else if (stricmp(token, "reportto")==0) rc = copyString(getRestOfLine(), &(config->ReportTo));
   else if (stricmp(token, "execonfile")==0) rc = parseExecOnFile(getRestOfLine(), config);
   else if (stricmp(token, "defarcmailsize")==0) rc = parseNumber(getRestOfLine(), 10, &(config->defarcmailSize));
   else if (stricmp(token, "areafixmsgsize")==0) rc = parseNumber(getRestOfLine(), 10, &(config->areafixMsgSize));
   else if (stricmp(token, "afterunpack")==0) rc = copyString(getRestOfLine(), &(config->afterUnpack));
   else if (stricmp(token, "beforepack")==0) rc = copyString(getRestOfLine(), &(config->beforePack));
   /* +AS+ */ else if (stricmp(token, "processpkt")==0) rc = copyString(getRestOfLine(), &(config->processPkt)); /* -AS- */
   else if (stricmp(token, "areafixsplitstr")==0) rc = copyString(getRestOfLine(), &(config->areafixSplitStr));
   else if (stricmp(token, "areafixorigin")==0) rc = copyString(getRestOfLine(), &(config->areafixOrigin));
   else if (stricmp(token, "filedescpos")==0) rc = parseUInt(getRestOfLine(), &(config->fileDescPos));
   else if (stricmp(token, "dlcdigits")==0) rc = parseUInt(getRestOfLine(), &(config->DLCDigits));
   else if (stricmp(token, "filemaxdupeage")==0) rc = parseUInt(getRestOfLine(), &(config->fileMaxDupeAge));
   else if (stricmp(token, "filefileumask")==0) rc = parseOctal(getRestOfLine(), &(config->fileFileUMask));
   else if (stricmp(token, "filedirumask")==0) rc = parseOctal(getRestOfLine(), &(config->fileDirUMask));
   else if (stricmp(token, "origininannounce")==0) config->originInAnnounce = 1;
   else if (stricmp(token, "maxticlinelength")==0) rc = parseUInt(getRestOfLine(), &(config->MaxTicLineLength));
   else if (stricmp(token, "filelocalpwd")==0) rc = copyString(getRestOfLine(), &(config->fileLocalPwd));
   else if (stricmp(token, "fileldescstring")==0) rc = copyString(getRestOfLine(), &(config->fileLDescString));
   else if (stricmp(token, "savetic")==0) rc = parseSaveTicStatement(getRestOfLine(), config);
   else if (stricmp(token, "areasmaxdupeage")==0) rc = parseNumber(getRestOfLine(), 10, &(config->areasMaxDupeAge));
   else if (stricmp(token, "dupebasetype")==0) rc = parseTypeDupes(getRestOfLine(), &(config->typeDupeBase), &(config->areasMaxDupeAge));
#ifdef __TURBOC__
   else unrecognised++;
#else   
   else
#endif       
   if (stricmp(token, "fidouserlist") ==0)
     rc = copyString(getRestOfLine(), &(config->fidoUserList));
   else if (stricmp(token, "nodelist") ==0)
     rc = parseNodelist(getRestOfLine(), config);
   else if (stricmp(token, "diffupdate") ==0) {
      rc = 0;
      if (config->nodelistCount > 0) {
        rc = copyString(getRestOfLine(),
                &(config->nodelists[config->nodelistCount-1].diffUpdateStem));
      }
      else {
	printNodelistError();
	rc = 1;
      }
   }
   else if (stricmp(token, "fullupdate") ==0) {
      rc = 0;
      if (config->nodelistCount > 0) {
        rc = copyString(getRestOfLine(),
                &(config->nodelists[config->nodelistCount-1].fullUpdateStem));
      }
      else {
	printNodelistError();
	rc = 1;
      }
   }
   else if (stricmp(token, "defaultzone") ==0) {
      rc = 0;
      if (config->nodelistCount > 0) {
        rc = parseUInt(getRestOfLine(),
                &(config->nodelists[config->nodelistCount-1].defaultZone));
      }
      else {
	printNodelistError();
	rc = 1;
      }
   }
   else if (stricmp(token, "nodelistformat") ==0) {
     if (config->nodelistCount > 0) {
      rc = parseNodelistFormat(getRestOfLine(), config,
                               &(config->nodelists[config->nodelistCount-1]));
     }
     else {
       printNodelistError();
       rc = 1;
     }
   }
   else if (stricmp(token, "logowner")==0) rc = parseOwner(getRestOfLine(), &(config->loguid), &(config->loggid));
   else if (stricmp(token, "logperm")==0) rc = parseNumber(getRestOfLine(), 8, &(config->logperm));

#ifdef __TURBOC__
   else unrecognised++;
   if (unrecognised == 3)
#else   
   else 
#endif
        printf("Unrecognized line(%d): %s\n", actualLineNr, line);

   if (rc != 0) {
      printf("Error %d (line %d): %s\n", rc, actualLineNr, line);
      wasError = 1;
      return rc;
   }

   free(actualLine);
   return 0;
}
