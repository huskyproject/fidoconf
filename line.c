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
#include <errno.h>
#include <stdarg.h>

#ifdef UNIX
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#else
#include <process.h>
#include <io.h>
#endif

#include <limits.h>

#include <smapi/patmat.h>
#include <smapi/unused.h>
#include <smapi/compiler.h>
#include <smapi/stamp.h>
#include <smapi/progprot.h>

#include "dirlayer.h"
#include "fidoconf.h"
#include "common.h"
#include "typesize.h"
#include "xstr.h"

char *actualKeyword, *actualLine;
int  actualLineNr;
char wasError = 0;
char CommentChar = '#';

char *getRestOfLine(void) {
   return stripLeadingChars(strtok(NULL, "\0"), " \t");
}

void prErr ( char *string, ...)
{
    va_list ap;

    printf("\"%s\", line %d: ", curconfname, actualLineNr);
    va_start(ap, string);
    vprintf(string, ap);
    va_end(ap);
    putchar('\n');

}

char *getDescription(void)
{
  char *descBuf = NULL, *token;
  int out=0, length;

  while ((out==0) && ((token=strtok(NULL," "))!=NULL))
  {
    xstrscat (&descBuf, token, " ", NULL);
    if (token[strlen(token)-1]=='\"') out=1;
  }

  switch (out)
  {
  case 0:
    prErr( "Error in area description!");
    nfree(descBuf);
    break;
    
  case 1: // out. cut '" '
    descBuf[length=(strlen(descBuf)-2)] = '\0';
    memmove(descBuf, descBuf+1, length);
    break;
  }

  return descBuf;
}

int parseComment(char *token, s_fidoconfig *config)
{
    char *ptr;

   // if there is no token return error...
   if (token==NULL) {
      prErr( "There is a comment character missing after %s!", actualKeyword);
      return 1;
   }

   ptr = strchr(TRUE_COMMENT, *token);

   if (!ptr) {
       prErr( "CommentChar - '%c' is not valid comment characters!", *token);
   } else {
       CommentChar = *token;
       config->CommentChar = *token;
   }

   return 0;
}

int parseVersion(char *token, s_fidoconfig *config)
{
   char buffer[10], *temp = token;
   int i = 0;

   // if there is no token return error...
   if (token==NULL) {
      prErr( "There is a version number missing after %s!", actualKeyword);
      return 1;
   }

   while (isdigit(*temp) && i<9) {
      buffer[i] = *temp;
      i++; temp++;
   }
   buffer[i] = 0;

   config->cfgVersionMajor = atoi(buffer);

   temp++; // eat .
   i = 0;

   while (isdigit(*temp) && i<9) {
      buffer[i] = *temp;
      i++; temp++;
   }
   buffer[i] = 0;

   config->cfgVersionMinor = atoi(buffer);

   return 0;
}

void printLinkError(void)
{
  prErr( "You must define a link first before you use %s!", actualKeyword);
  exit(1);
}

s_link *getDescrLink(s_fidoconfig *config)
{
   if (config->describeLinkDefaults) { // describing defaults for links
      return config->linkDefaults;
   } else {
      if (config->linkCount) {
         return &config->links[config->linkCount-1];
      } else {
         printLinkError();
         return NULL;
      }
   }
}

int parseAddress(char *token, s_fidoconfig *config)
{
   char *aka;

   if (token==NULL) {
      prErr( "There is an address missing after %s!", actualKeyword);
      return 1;
   }

   aka = strtok(token, " \t"); // only look at aka
   if (aka == NULL) {
      prErr( "There is an address missing after %s!", actualKeyword);
      return 1;
   }

   config->addr = srealloc(config->addr, sizeof(s_addr)*(config->addrCount+1));
   string2addr(aka, &(config->addr[config->addrCount]));
   config->addrCount++;

   return 0;
}

int parseRemap(char *token, s_fidoconfig *config)
{
   char *param;

   if (token==NULL) {
      prErr( "There are all parameters missing after %s!", actualKeyword);
      return 1;
   }


   config->remaps = srealloc(config->remaps,
                          (config->remapCount+1)*sizeof(s_remap));

   param = strtok(token, ",\t");
   if (param == NULL) {
      prErr( "Missing Name or * after %s!", actualKeyword);
      return 1;
   }

   if (strcmp(param,"*")!=0)
      { // Name for rerouting
      config->remaps[config->remapCount].toname=sstrdup(param);
      }
     else
      config->remaps[config->remapCount].toname=NULL;

   param = strtok(NULL, ",\t");
   if (param == NULL) {
      prErr( "Address or * missing after %s!",actualKeyword);
      return 1;
   }

   if (strcmp(param,"*")==0)
      config->remaps[config->remapCount].oldaddr.zone=0;
     else
      string2addr(param, &(config->remaps[config->remapCount].oldaddr));

   param = strtok(NULL, " \t");
   if (param == NULL) {
      prErr( "Address missing after %s!", actualKeyword);
      return 1;
   }

   string2addr(param, &(config->remaps[config->remapCount].newaddr));

   if (config->remaps[config->remapCount].toname==NULL &&
       config->remaps[config->remapCount].oldaddr.zone==0)
      {
      prErr( "At least one of the Parameters must not be *");
      return 1;
      }

   config->remapCount++;

   return 0;
}

int parsePath(char *token, char **var)
{
//   char *p, *q, *osvar;

   if (*var != NULL) {
      prErr("Dublicate path!");
      return 1;
   }
   if (token == NULL) {
      prErr("There is a path missing after %s!", actualKeyword);
      return 1;
   }
   if (stricmp(token, "passthrough")==0) {
      copyString(token, &(*var));
      return 0;
   }
/*
   if (strchr(token,'[') && strchr(token,']')) {

	   osvar = strchr(token,'[');
	   osvar++;
	   q = strchr(osvar, ']');
	   if (q) *q = '\0';
	   if (NULL == (p = getvar(osvar))) {
		   *q=']';
		   p=token;
	   }
	   if (!direxist(p))
		   {
			   prErr( "Path %s not found!", p);
			   return 1;
		   }
	   xstrscat(var, "[", osvar, "]", NULL);

   } else {
*/
   if (*token && token[strlen(token)-1] == PATH_DELIM)
	   Strip_Trailing(token, PATH_DELIM);
   xscatprintf(var, "%s%c", token, (char) PATH_DELIM);

   if (!direxist(*var)) {
	   prErr( "Path %s not found!", *var);
	   return 1;
   }

//   }

   return 0;
}

int parsePublic(char *token, s_fidoconfig *config)
{
   if (token == NULL) {
      prErr( "There is a path missing after %s!", actualKeyword);
      return 1;
   }
   config->publicDir = srealloc(config->publicDir, sizeof(char *)*(config->publicCount+1));
   config->publicDir[config->publicCount] = NULL;

   if (*token && token[strlen(token)-1] == PATH_DELIM)
       Strip_Trailing(token, PATH_DELIM);
   xscatprintf(&(config->publicDir[config->publicCount]), "%s%c", token, (char) PATH_DELIM);

   if (!direxist(config->publicDir[config->publicCount])) {
      prErr( "Path %s not found!", token);
      return 1;
   }

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
      prErr( "There are parameters missing after %s!", actualKeyword);
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
		prErr( "User name %s is unknown to OS !", name);
		return 1;
	}
	*uid = pw ? pw -> pw_uid : -1 ;

   };

   if (group != NULL) {
	grp = getgrnam(group);

	if ((*group) && grp == NULL) {
		prErr( "Group name %s is unknown to OS !", group);
		return 1;
	}
	*gid = grp ? grp -> gr_gid : -1 ;
   }
#else
   unused(token); unused(uid); unused(gid);
#endif
   return 0;
}

int parseNumber(char *token, int radix, unsigned *level) {
    char *end = NULL;
    unsigned long result;

    if (token == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
	return 1;
    }

    result = strtoul(token, &end, radix);

    if (!(*end == '\0' && *token != '\0') || result == ULONG_MAX) {
	prErr("Error in number representation : %s . %s!", token, end);
	return 1;
    }

    *level = (unsigned) result;
    return 0;
}

int parseSeenBy2D(char *token, s_addr **addr, unsigned int *count)
{
	char buf[6];
	UINT net=0,node=0,i;

	if (token==NULL) {
		prErr("There is an address missing after %s!", actualKeyword);
		return 1;
	}

	while (*token) {
		while(!isdigit(*token)) token++; i=0;
		while(isdigit(*token) && i<6) { buf[i] = *token, token++; i++;}
		buf[i]='\0'; net=atoi(buf);

		if (*token == ':') continue;

		while(!isdigit(*token)) token++; i=0;
		while(isdigit(*token) && i<6) { buf[i] = *token, token++; i++;}
		buf[i]='\0'; node=atoi(buf);

		if (*token == '.') { token++; while(isdigit(*token)) token++; }

		(*addr) = srealloc(*addr, sizeof(s_addr)*(*count+1));
		(*addr)[*count].net  = net;
		(*addr)[*count].node = node;
		(*count)++;

		if (*token == ')') break;
	}
	return 0;
}

int parseAreaOption(const s_fidoconfig *config, char *option, s_area *area)
{
   char *error;
   char *token;
   char *iOption;
   char *iToken;
   int i;

   iOption = strLower(sstrdup(option));
   if (strcmp(iOption, "b")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         prErr("An msgbase type is missing after -b in areaOptions!");
         free(iOption);
         return 1;
      }
      iToken = strLower(sstrdup(token));
      if (strcmp(iToken, "squish")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
           prErr("Logical Defect!! You could not make a Squish Area Passthrough!");
	   free(iOption);
	   free(iToken);
           return 1;
        }
        area->msgbType = MSGTYPE_SQUISH;
      }
      else if (strcmp(iToken, "jam")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
           prErr("Logical Defect!! You could not make a Jam Area Passthrough!");
	   free(iOption);
	   free(iToken);
           return 1;
        }
        area->msgbType = MSGTYPE_JAM;
      }
      else if (strcmp(iToken, "msg")==0) {
        if (area->msgbType == MSGTYPE_PASSTHROUGH) {
			prErr("Logical Defect!! You could not make a *.msg Area Passthrough!");
	   free(iOption);
	   free(iToken);
           return 1;
        }
        area->msgbType = MSGTYPE_SDM;
      }
      else
      {
	prErr("MsgBase type %s not valid after -b in areaOptions!", token);
	free(iOption);
	free(iToken);
	return 1;
      }
   }
   else if (strcmp(iOption, "p")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL) {
         prErr("Number is missing after -p in areaOptions!");
	 free(iOption);
         return 1;
      }
      area->purge = (UINT) strtol(token, &error, 0);
      if ((error != NULL) && (*error != '\0')) {
         prErr("Number is wrong after -p in areaOptions!");
	 free(iOption);
         return 1;     // error occured;
      }
   }
   else if (strcmp(iOption, "$m")==0) {
      area->max = (UINT) strtol(strtok(NULL, " \t"), &error, 0);
      if ((error != NULL) && (*error != '\0')) {
	 free(iOption);
         return 1;     // error
      }
   }
   else if (strcmp(iOption, "a")==0) {
      token = strtok(NULL, " \t");
      if (token == NULL)
	{
	  prErr("Adress is missing after -a in areaOptions!");
	  free(iOption);
	  return 1;
	}
      area->useAka = getAddr(*config, token);
      if (area->useAka == NULL) {
         prErr("%s not found as address.", token);
         free(iOption);
         return 1;
      }
   }
   else if (strcmp(iOption, "lr")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           prErr("Number is missing after -lr in areaOptions!");
	   free(iOption);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           prErr("Number is wrong after -lr in areaOptions!");
	   free(iOption);
	   return 1;
       }
       area->levelread = (unsigned)atoi(token);
   }
   else if (strcmp(iOption, "lw")==0) {
       token = strtok(NULL, " \t");
       if (token == NULL) {
           prErr("Number is missing after -lw in areaOptions!");
	   free(iOption);
	   return 1;
       }
       for (i=0; i<strlen(token); i++) {
           if (isdigit(token[i]) == 0) break;
       }
       if (i != strlen(token)) {
           prErr("Number is wrong after -lw in areaOptions!");
	   free(iOption);
	   return 1;
       }
       area->levelwrite = (unsigned)atoi(token);
   }
   else if (strcmp(iOption, "tinysb")==0) area->tinySB = 1;
   else if (strcmp(iOption, "killsb")==0) area->killSB = 1;
   else if (strcmp(iOption, "keepunread")==0) area->keepUnread = 1;
   else if (strcmp(iOption, "killread")==0) area->killRead = 1;
   else if (strcmp(iOption, "h")==0) area->hide = 1;
   else if (strcmp(iOption, "manual")==0) area->mandatory = 1;
   else if (strcmp(iOption, "nopause")==0) area->noPause = 1;
   else if (strcmp(iOption, "nolink")==0) area->nolink = 1;
   else if (strcmp(iOption, "mandatory")==0) area->mandatory = 1;
   else if (strcmp(iOption, "debug")==0) area->debug = 1;
   else if (strcmp(iOption, "dosfile")==0) area->DOSFile = 1;
   else if (strcmp(iOption, "dupecheck")==0) {
     token = strtok(NULL, " \t");
     if (token == NULL) {
       prErr("Missing dupeCheck parameter!");
       free(iOption);
       return 1;
     }
     if (stricmp(token, "off")==0) area->dupeCheck = dcOff;
     else if (stricmp(token, "move")==0) area->dupeCheck = dcMove;
     else if (stricmp(token, "del")==0) area->dupeCheck = dcDel;
     else {
       prErr("Wrong dupeCheck parameter!");
       free(iOption);
       return 1; // error
     }
   }
   else if (strcmp(iOption, "dupehistory")==0) {
     area->dupeHistory = (UINT) strtol(strtok(NULL, " \t"), &error, 0);
     if ((error != NULL) && (*error != '\0')) return 1;    // error
   }
   else if (strcmp(iOption, "g")==0) {
     token = strtok(NULL, " \t");
     if (token == NULL) {
       free(iOption);
       return 1;
     }
     nfree(area->group);
     area->group = sstrdup(token);
   }
   else if (strcmp(iOption, "nopack")==0) area->nopack = 1;
   else if (strcmp(iOption, "ccoff")==0) area->ccoff=1;
   else if (strcmp(iOption, "keepsb")==0) area->keepsb=1;
   else if (strcmp(iOption, "$")==0) ;
   else if (strcmp(iOption, "0")==0) ;
   else if (strcmp(iOption, "d")==0) {
     if ((area->description=getDescription())==NULL) {
       free(iOption);
       return 1;
     }
   }
   else if (strcmp(iOption, "fperm")==0) {
     token = strtok(NULL, " \t");
     if (token==NULL) {
       prErr("Missing permission parameter!");
       free(iOption);
       return 1;
     }
     else
     {
       free(iOption);
       return parseNumber(token, 8, &(area->fperm));
     }
   }
   else if (strcmp(iOption, "fowner")==0) {
     token = strtok(NULL, " \t");
     if (token==NULL)
       prErr("Missing ownership parameter!");
     else {
       free(iOption);
       return parseOwner(token, &(area->uid), &(area->gid));
     }
   }
   else if (strncmp(iOption, "sbadd(", 6)==0) {
	   parseSeenBy2D(iOption,&(area->sbadd),&(area->sbaddCount));
   }
   else if (strncmp(iOption, "sbign(", 6)==0) {
	   parseSeenBy2D(iOption,&(area->sbign),&(area->sbignCount));
   }
   else {
     prErr("unknown area option \"-%s\"!", option);
     free(iOption);
     return 1;
   }

   free(iOption);
   return 0;
}

int parseFileAreaOption(const s_fidoconfig *config, char *option, s_filearea *area)
{
  char *token;
  char *iOption;
  int i;

  iOption = strLower(sstrdup(option));
  if (strcmp(iOption, "a")==0) {
    token = strtok(NULL, " \t");
    area->useAka = getAddr(*config, token);
    if (area->useAka == NULL) {
      prErr("%s not found as address.", token);
      free(iOption);
      return 1;
    }
  }
  else if (strcmp(iOption, "lr")==0) {
    token = strtok(NULL, " \t");
    if (token == NULL) {
      prErr("Number is missing after -lr in areaOptions!");
      free(iOption);
      return 1;
    }
    for (i=0; i<strlen(token); i++) {
      if (isdigit(token[i]) == 0) break;
    }
    if (i != strlen(token)) {
      prErr("Number is wrong after -lr in areaOptions!");
      free(iOption);
      return 1;
    }
    area->levelread = (unsigned)atoi(token);
  }
  else if (strcmp(iOption, "lw")==0) {
    token = strtok(NULL, " \t");
    if (token == NULL) {
      prErr("Number is missing after -lw in areaOptions!");
      free(iOption);
      return 1;
    }
    for (i=0; i<strlen(token); i++) {
      if (isdigit(token[i]) == 0) break;
    }
    if (i != strlen(token)) {
      prErr("Number is wrong after -lw in areaOptions!");
      free(iOption);
      return 1;
    }
    area->levelwrite = (unsigned)atoi(token);
  }
  else if (strcmp(iOption, "h")==0) area->hide = 1;
  else if (strcmp(iOption, "manual")==0) area->mandatory = 1;
  else if (strcmp(iOption, "sendorig")==0) area->sendorig = 1;
  else if (strcmp(iOption, "nopause")==0) area->noPause = 1;
  else if (strcmp(iOption, "nocrc")==0) area->noCRC = 1;
  else if (strcmp(iOption, "noreplace")==0) area->noreplace = 1;
  else if (strcmp(iOption, "g")==0) {
    token = strtok(NULL, " \t");
    if (token == NULL) {
      free(iOption);
      return 1;
    }
    nfree(area->group);
    area->group = sstrdup(token);
  }
  else if (strcmp(iOption, "d")==0) {
    if ((area->description=getDescription())==NULL) {
      free(iOption);
      return 1;
    }
  }
  else {
    prErr("unknown area option \"-%s\"!", option);
    free(iOption);
    return 1;
  }

  return 0;
}

int parseLinkOption(s_arealink *alink, char *token)
{
  char *iToken;

  iToken = strLower(sstrdup(token));
  if (strcmp(iToken, "r")==0) alink->import = 0;
  else if (strcmp(iToken, "w")==0) alink->export = 0;
  else if (strcmp(iToken, "mn")==0) alink->mandatory = 1;
  else if (strcmp(iToken, "def")==0) alink->defLink = 1;
  else {
    free(iToken);
    return 1;
  }

  free(iToken);
  return 0;
}

int parseAreaLink(const s_fidoconfig *config, s_area *area, char *tok) {
	s_link *link;
	s_arealink *arealink;
	
	area->downlinks = srealloc(area->downlinks, sizeof(s_arealink*)*(area->downlinkCount+1));
	area->downlinks[area->downlinkCount] = (s_arealink*)scalloc(1, sizeof(s_arealink));
	area->downlinks[area->downlinkCount]->link = getLinkForArea(*config,tok,area);
	
	if (area->downlinks[area->downlinkCount]->link == NULL) {
		prErr("no links like \"%s\" in config!",
			   actualLineNr, tok);
		return 1;
	}

	link = area->downlinks[area->downlinkCount]->link;
	arealink = area->downlinks[area->downlinkCount];
	area->downlinkCount++;

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
	if (e_readCheck(config, area, link)) arealink->export = 0;
	if (e_writeCheck(config, area, link)) arealink->import = 0;

	return 0;
}

int parseArea(const s_fidoconfig *config, char *token, s_area *area)
{
   char *tok, addr[24];
   unsigned int rc = 0, i;

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   memset(area, '\0', sizeof(s_area));
   area->fperm = area->uid = area->gid = -1;

   area->msgbType = MSGTYPE_SDM;
   area->useAka = config->addr;

   // set default parameters of dupebase
   area->dupeHistory = 7; /* 7 days */

   // remove after 03-Apr-01
   // set default group for reader
   //area->group = (char*) smalloc(sizeof(char)+1);
   //strcpy(area->group, "0");

   // set defaults for MS-DOS
#ifdef MSDOS
   area->DOSFile = 1;
#endif

   tok = strtok(token, " \t");
   if (tok == NULL) {
      prErr("There is an areaname missing after %s!", actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) smalloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      prErr("There is a filename missing %s!", actualLine);
      return 2;         // if there is no filename
   }
   if (stricmp(tok, "passthrough") != 0) {
      // msgbase on disk
      area->fileName = (char *) smalloc(strlen(tok)+1);
      strcpy(area->fileName, tok);
   } else {
      // passthrough area
      area->fileName = NULL;
      area->msgbType = MSGTYPE_PASSTHROUGH;
   }

   tok = strtok(NULL, " \t");

   while (tok != NULL) {
      if(tok[0]=='-') {
          rc += parseAreaOption(config, tok+1, area);
	  if (rc) return rc;
      }
      else if ((isdigit(*tok) || (*tok=='*')) && (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*"))) {
		  
		  if (strchr(tok, '*')) {
			  for (i=0; i<config->linkCount; i++) {
				  sprintf(addr, aka2str(config->links[i].hisAka));
				  if (patmat(addr, tok)) {
					  parseAreaLink(config,area,addr);
					  area->downlinks[area->downlinkCount-1]->mandatory = 1;
				  }
			  }
			  tok = strtok(NULL, " \t");
			  continue;
		  }

		  rc += parseAreaLink(config, area, tok);
		  if (rc) return rc;

		  tok = strtok(NULL, " \t");
		  while (tok) {
			  if (tok[0]=='-') {
				  if (parseLinkOption(area->downlinks[area->downlinkCount-1], tok+1))
					  break;
				  tok = strtok(NULL, " \t");
			  } else break;
		  }
		  continue;
      }
      else {
		  prErr("Error in areaOptions token=%s!", tok);
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
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->echoAreas = srealloc(config->echoAreas, sizeof(s_area)*(config->echoAreaCount+1));
   rc = parseArea(config, token, &(config->echoAreas[config->echoAreaCount]));
   config->echoAreaCount++;
   return rc;
}

int parseNetMailArea(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->netMailAreas = srealloc(config->netMailAreas, sizeof(s_area)*(config->netMailAreaCount+1));
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
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   memset(area, 0, sizeof(s_filearea));

   area->pass = 0;
   area->useAka = config->addr;

   // remove after 03-Apr-01
   // set default group for reader
   //area->group = (char*) smalloc(sizeof(char)+1);
   //strcpy(area->group, "0");

   tok = strtok(token, " \t");
   if (tok == NULL) {
      prErr("There is a areaname missing after %s!", actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) smalloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      prErr("There is a pathname missing %s!", actualLine);
      return 2;         // if there is no filename
   }
   if (stricmp(tok, "passthrough") != 0) {
      if (tok[strlen(tok)-1] == PATH_DELIM) {
         area->pathName = (char *) smalloc(strlen(tok)+1);
         strcpy(area->pathName, tok);
      } else {
         area->pathName = (char *) smalloc(strlen(tok)+2);
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
      if(tok[0]=='-') {
          rc += parseFileAreaOption(config, tok+1, area);
	  if (rc) return rc;
      }
      else if (isdigit(tok[0]) && (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*"))) {
         area->downlinks = srealloc(area->downlinks, sizeof(s_arealink*)*(area->downlinkCount+1));
         area->downlinks[area->downlinkCount] = (s_arealink*) scalloc(1, sizeof(s_arealink));
//         area->downlinks[area->downlinkCount]->link = getLink(*config, tok);
         area->downlinks[area->downlinkCount]->link = getLinkForFileArea(*config,tok,area);

         if (area->downlinks[area->downlinkCount]->link == NULL) {
            prErr("Link for this area is not found!");
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
		 if (link->Pause && area->noPause==0) arealink->export = 0;

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
         prErr("Error in areaOptions token=%s!", tok);
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
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->fileAreas = srealloc(config->fileAreas,
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

   unused(config);

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   memset(area, 0, sizeof(s_bbsarea));

   tok = strtok(token, " \t");
   if (tok == NULL) {
      prErr("There is a areaname missing after %s!", actualKeyword);
      return 1;         // if there is no areaname
   }

   area->areaName= (char *) smalloc(strlen(tok)+1);
   strcpy(area->areaName, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      prErr("There is a pathname missing %s!", actualLine);
      return 2;         // if there is no filename
   }

   if (tok[strlen(tok)-1] == PATH_DELIM) {
      area->pathName = (char *) smalloc(strlen(tok)+1);
      strcpy(area->pathName, tok);
   } else {
      area->pathName = (char *) smalloc(strlen(tok)+2);
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
         prErr("Error in areaOptions token=%s!", tok);
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
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->bbsAreas = srealloc(config->bbsAreas,
	sizeof(s_bbsarea)*(config->bbsAreaCount+1));
   rc = parseBbsArea(config, token,
	&(config->bbsAreas[config->bbsAreaCount]));
   config->bbsAreaCount++;
   return rc;
}

int parseLink(char *token, s_fidoconfig *config)
{

   s_link   *clink;
   s_link   *deflink;
   int i;

   if (token == NULL) {
      prErr("There is a name missing after %s!", actualKeyword);
      return 1;
   }

   config->describeLinkDefaults=0; // Stop describing of link defaults if it was

   config->links = srealloc(config->links, sizeof(s_link)*(config->linkCount+1));

   clink = &(config->links[config->linkCount]);

   if (config->linkDefaults) {

      memcpy ( clink, config->linkDefaults, sizeof(s_link));
      deflink = config->linkDefaults;

	  clink->hisAka.domain = sstrdup(deflink->hisAka.domain);
	  clink->name = sstrdup(deflink->name);

	  clink->defaultPwd = sstrdup(deflink->defaultPwd);

      if (deflink->pktPwd != deflink->defaultPwd ) {
		  clink->pktPwd = sstrdup (deflink->pktPwd);
      } else {
		  clink->pktPwd = clink->defaultPwd;
      }
      if (deflink->ticPwd != deflink->defaultPwd ) {
		  clink->ticPwd = sstrdup (deflink->ticPwd);
      } else {
		  clink->ticPwd = clink->defaultPwd;
      }
  	  if (deflink->areaFixPwd != deflink->defaultPwd ) {
		  clink->areaFixPwd = sstrdup (deflink->areaFixPwd);
      } else {
		  clink->areaFixPwd = clink->defaultPwd;
      }
  	  if (deflink->fileFixPwd != deflink->defaultPwd ) {
		  clink->fileFixPwd = sstrdup (deflink->fileFixPwd);
      } else {
		  clink->fileFixPwd = clink->defaultPwd;
      }
  	  if (deflink->bbsPwd != deflink->defaultPwd ) {
		  clink->bbsPwd = sstrdup(deflink->bbsPwd);
      } else {
		  clink->bbsPwd = clink->defaultPwd;
      }
  	  if (deflink->sessionPwd != deflink->defaultPwd ) {
		  clink->sessionPwd = sstrdup (deflink->sessionPwd);
      } else {
		  clink->sessionPwd = clink->defaultPwd;
      }
	  clink->handle = sstrdup (deflink->handle);
	  clink->email = sstrdup (deflink->email);
	  clink->emailFrom = sstrdup (deflink->emailFrom);
	  clink->emailSubj = sstrdup (deflink->emailSubj);
          clink->emailEncoding = deflink->emailEncoding;
	  clink->LinkGrp = sstrdup (deflink->LinkGrp);
      if (deflink->AccessGrp) {
          clink->AccessGrp = smalloc(sizeof(char *) * clink->numAccessGrp);
          for ( i=0; i < deflink->numAccessGrp; i++)
			  clink->AccessGrp[i] = sstrdup (deflink->AccessGrp[i]);
      }
	  clink->autoAreaCreateFile = sstrdup (deflink->autoAreaCreateFile);
	  clink->autoFileCreateFile = sstrdup (deflink->autoFileCreateFile);
	  clink->autoAreaCreateDefaults = sstrdup (deflink->autoAreaCreateDefaults);
	  clink->autoFileCreateDefaults = sstrdup (deflink->autoFileCreateDefaults);
	  clink->forwardRequestFile = sstrdup (deflink->forwardRequestFile);
	  clink->RemoteRobotName = sstrdup (deflink->RemoteRobotName);
	  clink->forwardFileRequestFile = sstrdup (deflink->forwardFileRequestFile);
	  clink->RemoteFileRobotName = sstrdup (deflink->RemoteFileRobotName);
	  clink->msgBaseDir = sstrdup (deflink->msgBaseDir);
      if (deflink->optGrp) {
          clink->optGrp = smalloc(sizeof(char *) * clink->numOptGrp);
          for ( i=0; i < deflink->numOptGrp; i++)
			  clink->optGrp[i] = sstrdup (deflink->optGrp[i]);
      }
      if (deflink->frMask) {
          clink->frMask = smalloc(sizeof(char *) * clink->numFrMask);
          for ( i=0; i < deflink->numFrMask; i++)
			  clink->frMask[i] = sstrdup (deflink->frMask[i]);
      }
      if (deflink->dfMask) {
          clink->dfMask = smalloc(sizeof(char *) * clink->numDfMask);
          for ( i=0; i < deflink->numDfMask; i++)
			  clink->dfMask[i] = sstrdup (deflink->dfMask[i]);
      }

   } else {

      memset(clink, 0, sizeof(s_link));

	  // Set defaults like in parseLinkDefaults()

      // set areafix default to on
      clink->AreaFix = 1;
      clink->FileFix = 1;

      // set defaults to export, import, mandatory
      clink->export = 1;
      clink->import = 1;
      clink->ourAka = &(config->addr[0]);

      // set default maxUnpackedNetmail
      clink->maxUnpackedNetmail = 100;

   }

   clink->name = (char *) smalloc (strlen(token)+1);
   strcpy(clink->name, token);
   clink->handle = clink->name;

   config->linkCount++;
   return 0;
}

int parseNodelist(char *token, s_fidoconfig *config)
{
   if (token == NULL) {
      prErr("There is a name missing after %s!", actualKeyword);
      return 1;
   }

   config->nodelists = srealloc(config->nodelists, sizeof(s_nodelist)*(config->nodelistCount+1));
   memset(&(config->nodelists[config->nodelistCount]), 0, sizeof(s_nodelist));
   config->nodelists[config->nodelistCount].nodelistName =
     (char *) smalloc (strlen(token)+1);
   strcpy(config->nodelists[config->nodelistCount].nodelistName, token);

   config->nodelists[config->nodelistCount].format = fts5000;

   config->nodelistCount++;
   return 0;
}

int parseBool (char *token, unsigned int *value)
{
  char *iToken;

  if (token == NULL) {
    *value = 1;
    return 0;
  }

  iToken = strLower(sstrdup(token));
  if ((strcmp(iToken, "on")==0) || (strcmp(iToken, "yes")==0) || (strcmp(iToken, "1")==0)) *value = 1;
  else if ((strcmp(iToken, "off")==0) || (strcmp(iToken, "no")==0) || (strcmp(iToken, "0")==0)) *value = 0;
  else {
    free(iToken);
    return 2;
  }
  free(iToken);
  return 0;
}

int parseAutoPause(char *token, unsigned *autoPause)
{
   char *ptr;

   if (token == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   } /* endif */

   for (ptr = token; *ptr; ptr++) {
      if (!isdigit(*ptr)) {
         prErr("Parameter missing after %s!", actualKeyword);
         return 1;
      } /* endif */
   } /* endfor */

   *autoPause = (unsigned)atoi(token);

   return 0;
}

int parseUInt(char *token, unsigned int *uint) {

    if (token == NULL) {
	prErr("Parameter missing after %s!", actualKeyword);
	return 1;
    }
    sscanf(token, "%u", uint);
    return 0;
}

int parseOctal(char *token, unsigned int *octal) {

    if (token == NULL) {
       prErr("Parameter missing after %s!", actualKeyword);
       return 1;
    }
    sscanf(token, "%o", octal);
    return 0;
}

int parsePWD(char *token, char **pwd) {

   if (token == NULL) {            // return empty password
      *pwd = (char *) smalloc(1);
      (*pwd)[0] = '\0';
      return 0;
   }

   *pwd = sstrdup(token);
   if (*pwd == NULL) return 1;
   else return 0;
}

int parseHandle(char *token, s_fidoconfig *config) {
   s_link   *clink;

   if (token == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   clink = getDescrLink(config);

   clink->handle = (char *) smalloc (strlen(token)+1);
   strcpy(clink->handle, token);
   return 0;
}

int parseRoute(char *token, s_fidoconfig *config, s_route **route,
			   UINT *count, e_id id) {
  char *option;
  char *iOption;
  int  rc = 0;
  s_route *actualRoute;

  if (token == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  *route = srealloc(*route, sizeof(s_route)*(*count+1));
  actualRoute = &(*route)[*count];
  memset(actualRoute, '\0', sizeof(s_route));

  actualRoute->id = id;

  option = strtok(token, " \t");

  if (option == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  while (option != NULL) {
    iOption = strLower(sstrdup(option));
	if (strcmp(iOption, "hold")==0) actualRoute->flavour = hold;
    else if (strcmp(iOption, "normal")==0) actualRoute->flavour = normal;
    else if (strcmp(iOption, "crash")==0) actualRoute->flavour = crash;
    else if (strcmp(iOption, "direct")==0) actualRoute->flavour = direct;
    else if (strcmp(iOption, "immediate")==0) actualRoute->flavour = immediate;
    else if (strcmp(iOption, "hub")==0) actualRoute->routeVia = hub;
    else if (strcmp(iOption, "host")==0) actualRoute->routeVia = host;
    else if (strcmp(iOption, "boss")==0) actualRoute->routeVia = boss;
    else if (strcmp(iOption, "noroute")==0) actualRoute->routeVia = noroute;
    else if (strcmp(iOption, "no-route")==0) actualRoute->routeVia = noroute;
    else if (strcmp(iOption, "nopack")==0) actualRoute->routeVia = nopack;
    else if (strcmp(iOption, "no-pack")==0) actualRoute->routeVia = nopack;
    else if (isdigit(option[0]) || (option[0] == '*') || (option[0] == '?')) {
      if ((actualRoute->routeVia == 0) && (actualRoute->target == NULL)) {
	actualRoute->target = getLink(*config, option);
	actualRoute->viaStr = (char *) smalloc(strlen(option)+1);
	strcpy(actualRoute->viaStr, option);
      }
      else {
	if (actualRoute->pattern == NULL) {
	  //2 for additional .0 if needed
	  actualRoute->pattern = (char *) smalloc(strlen(option)+2+1);
	  strcpy(actualRoute->pattern, option);
	  if ((strchr(option, '.')==NULL) && (strchr(option, '*')==NULL)) {
	    strcat(actualRoute->pattern, ".0");
	  }
	  (*count)++;
	} else {
	  // add new Route for additional patterns
	  *route = srealloc(*route, sizeof(s_route)*(*count+1));
	  actualRoute = &(*route)[*count];
	  memcpy(actualRoute,&(*route)[(*count)-1],sizeof(s_route));
	  if ((*route)[(*count)-1].viaStr != NULL)
	    actualRoute->viaStr = sstrdup((*route)[(*count)-1].viaStr);

	  //2 for additional .0 if needed
	  actualRoute->pattern = (char *) smalloc(strlen(option)+2+1);
	  strcpy(actualRoute->pattern, option);
	  if ((strchr(option, '.')==NULL) && (strchr(option, '*')==NULL)) {
	    strcat(actualRoute->pattern, ".0");
	  }
	  (*count)++;
	}

      }
      if ((actualRoute->target == NULL) && (actualRoute->routeVia == 0)) {
         prErr("Link not found in Route statement!");
         rc = 2;
      }
    }
    free(iOption);
    option = strtok(NULL, " \t");
  }

  return rc;
}

int parsePack(char *line, s_fidoconfig *config) {

   char   *p, *c;
   s_pack *pack;

   if (line == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   p = strtok(line, " \t");
   c = getRestOfLine();
   if ((p != NULL) && (c != NULL)) {

      // add new pack statement
      config->packCount++;
      config->pack = srealloc(config->pack, config->packCount * sizeof(s_pack));

      // fill new pack statement
      pack = &(config->pack[config->packCount-1]);
      pack->packer = (char *) smalloc(strlen(p)+1);
      strcpy(pack->packer, p);
      pack->call   = (char *) smalloc(strlen(c)+1);
      strcpy(pack->call, c);
      if (strstr(pack->call, "$a")==NULL) {
         prErr("$a missing in pack statement %s!", actualLine);
         return 2;
      }
      if (strstr(pack->call, "$f")==NULL) {
         prErr("$f missing in pack statement %s!", actualLine);
         return 2;
      }

      return 0;
   } else {
      prErr("Parameter missing after %s!", actualKeyword);
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
       prErr("Parameter missing after %s!", actualKeyword);
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
       config->unpack = srealloc(config->unpack, config->unpackCount * sizeof(s_unpack));

       // fill new pack statement
       unpack = &(config->unpack[config->unpackCount-1]);
       unpack->call   = (char *) smalloc(strlen(p)+1);
       strcpy(unpack->call, p);

       if (strstr(unpack->call, "$a")==NULL) {
          prErr("$a missing in unpack statement %s!", actualLine);
          return 2;
       }

       p = strtok(c, " \t"); // p is containing offset now
       c = strtok(NULL, " \t"); // t is containing match code now

       if ((p == NULL) || (c == NULL)) {
          prErr("offset or match code missing in unpack statement %s!", actualLine);
          return 1;
       };

       unpack->offset = (UINT) strtol(p, &error, 0);

       if ((error != NULL) && (*error != '\0')) {
          prErr("Number is wrong for offset in unpack!");
          return 1;     // error occured;
       }

       unpack->matchCode = (UCHAR *) smalloc(strlen(c) / 2 + 1);
       unpack->mask      = (UCHAR *) smalloc(strlen(c) / 2 + 1);

       // parse matchcode statement
       // this looks a little curvy, I know. Remember, I programmed this at 23:52 :)
       for (i = 0, error = NULL; c[i] != '\0' && error == NULL; i++) {
          code = (UCHAR) toupper(c[i]);
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
          prErr("matchCode can\'t contain %c in in unpack statement %s!", *error, actualLine);
	            return 1;
       };

       if (i % 2 != 0)  {
          prErr("matchCode must be byte-aligned in unpack statement %s!", actualLine);
          return 1;
       };

       unpack->codeSize = i / 2;

       return 0;
    } else {
       prErr("Parameter missing after %s!", actualKeyword);
       return 1;
    }
}
/*
static int f_accessable(char *token)
{
// We don't need a real fexist function here, and we don't want to
//       be dependent on SMAPI just because of this. For us, it is enough
//       to see if the file is accessible
// BUT WE DON'T KNOW ABOUT DIRS!

#ifdef UNIX
    struct stat sb;

    if (stat(token, &sb))
	return 0;  // cannot stat the file
    if (access(token, R_OK))
	return 0;  // cannot access the file
    return 1;
#else
    FILE *f = fopen(token, "rb");
    if (f == NULL)
        return 0;
    fclose(f);
    return 1;
#endif
}
*/

int parseFileName(char *line, char **name) {
   char *token;

   if (*name != NULL) {
      prErr("Duplicate file name!");
      return 1;
   }

   if (line == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   if (line[0]=='\"') {
     token=(char *) smalloc (strlen(line)+1);
     sscanf(line,"\"%[^\"]s",token);
   }
   else
     token = strtok(line, " \t");

   if (token == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }
//   if (f_accessable(token)) {
   if (fexist(token)) { // fexist knows about dirs
//      (*name) = smalloc(strlen(token)+1);
//      strcpy((*name), token);
	xstrcat(name, token);
   } else {
      prErr("File not found or no permission: %s!", token);
      if (line[0]=='\"')
        free(token);
      return 2;
   }
   if (line[0]=='\"')
     free(token);
   return 0;
}

int parsePackerDef(char *line, s_fidoconfig *config, s_pack **packerDef) {

   int i;

   if (line == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   if (stricmp(line,"none")==0) {
	   (*packerDef) = NULL;
	   return 0;
   }

   for(i = 0; i < config->packCount; i++)
      if (stricmp(line, config->pack[i].packer)==0) {
         (*packerDef) = &(config->pack[i]);
         return 0;
      }

   prErr("Packer %s not found for packer statement!", line);
   return 2;
}

int parseEchoMailFlavour(char *line, e_flavour *flavour)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "hold")==0) *flavour = hold;
  else if (strcmp(iLine, "normal")==0) *flavour = normal;
  else if (strcmp(iLine, "direct")==0) *flavour = direct;
  else if (strcmp(iLine, "crash")==0) *flavour = crash;
  else if (strcmp(iLine, "immediate")==0) *flavour = immediate;
  else {
    prErr("Unknown echomail flavour %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}

int parseFileEchoFlavour(char *line, e_flavour *flavour)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "hold")==0) *flavour = hold;
  else if (strcmp(iLine, "normal")==0) *flavour = normal;
  else if (strcmp(iLine, "direct")==0) *flavour = direct;
  else if (strcmp(iLine, "crash")==0) *flavour = crash;
  else if (strcmp(iLine, "immediate")==0) *flavour = immediate;
  else {
    prErr("Unknown fileecho flavour %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}

int parseGrp(char *token, char **grp[], unsigned int *count) {
	char *tok;

	tok = strtok(token, " \t,");

	while (tok) {
		*grp = srealloc(*grp, sizeof(char*)*(*count+1));
		(*grp)[*count] = sstrdup(tok);
		(*count)++;

		tok = strtok(NULL, " \t,");
	}

	return 0;
}

//and the parseGroup:
// i make some checking... maybe it is better check if the pointer exist from
// copyString function?
// i removed some checking... ;-)
// groups may be copied from linkDefaults

int parseGroup(char *token, s_fidoconfig *config, int i)
{
	s_link *link = NULL;

	if (token == NULL)
		{
			prErr("Parameter missing after %s!", actualKeyword);
			return 1;
		}

	if (i != 2) link = getDescrLink(config);

	switch (i) {
	case 0:
		if (link->AccessGrp) freeGroups(link->AccessGrp, link->numAccessGrp);
		link->AccessGrp = NULL;
		link->numAccessGrp = 0;
		parseGrp(token, &(link->AccessGrp), &(link->numAccessGrp));
		break;

	case 1:
		nfree(link->LinkGrp);
		copyString(token, &link->LinkGrp);
		break;

	case 2:
		if (config->numPublicGroup != 0) {
			prErr("Duplicate parameter after %s!", actualKeyword);
			return 1;
		}
		parseGrp(token, &(config->PublicGroup), &(config->numPublicGroup));
		break;

	case 3:
		if (link->optGrp) freeGroups(link->optGrp, link->numOptGrp);
		link->optGrp = NULL;
		link->numOptGrp = 0;
		parseGrp(token, &(link->optGrp), &(link->numOptGrp));
		break;

	case 4:
		if (link->frMask) freeGroups(link->frMask, link->numFrMask);
		link->frMask = NULL;
		link->numFrMask = 0;
		parseGrp(token, &(link->frMask), &(link->numFrMask));
		break;

	case 5:
		if (link->dfMask) freeGroups(link->dfMask, link->numDfMask);
		link->dfMask = NULL;
		link->numDfMask = 0;
		parseGrp(token, &(link->dfMask), &(link->numDfMask));
		break;
	}

   return 0;
}

int parseLocalArea(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->localAreas = srealloc(config->localAreas, sizeof(s_area)*(config->localAreaCount+1));
   rc = parseArea(config, token, &(config->localAreas[config->localAreaCount]));
   config->localAreaCount++;
   return rc;
}


int parseCarbon(char *token, s_fidoconfig *config, e_carbonType ctype)
{
   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->carbonCount++;
   config->carbons = srealloc(config->carbons,sizeof(s_carbon)*(config->carbonCount));
   memset(&(config->carbons[config->carbonCount-1]), 0, sizeof(s_carbon));

   config->carbons[config->carbonCount-1].ctype = ctype;
   copyString(token, &(config->carbons[config->carbonCount-1].str));

   if (ctype == ct_addr) {
	   string2addr(token, &(config->carbons[config->carbonCount-1].addr));
	   nfree(config->carbons[config->carbonCount-1].str);
   }

   return 0;
}

int parseCarbonArea(char *token, s_fidoconfig *config, int move) {

   if (token == NULL) {
	   prErr("There are parameters missing after %s!", actualKeyword);
	   return 1;
   }

   if (config->carbonCount == 0) {
          prErr("No carbon codition specified before %s", actualKeyword);
          return 1;
   }
   copyString(token, &(config->carbons[config->carbonCount-1].areaName));
   config->carbons[config->carbonCount-1].extspawn = 0;
   config->carbons[config->carbonCount-1].move = move;
   return 0;
}

int parseCarbonDelete(char *token, s_fidoconfig *config) {

   if (token != NULL) {
	   prErr("There are extra parameters after %s!", actualKeyword);
	   return 1;
   }
   if (config->carbonCount == 0) {
          prErr("No carbon codition specified before %s", actualKeyword);
          return 1;
   }
   config->carbons[config->carbonCount-1].areaName = NULL;
   config->carbons[config->carbonCount-1].move = 2;
   config->carbons[config->carbonCount-1].extspawn = 0;
   return 0;
}

int parseCarbonExtern(char *token, s_fidoconfig *config) {

   if (token == NULL) {
	   prErr("There are parameters missing after %s!", actualKeyword);
	   return 1;
   }
   if (config->carbonCount == 0) {
          prErr("No carbon codition specified before %s", actualKeyword);
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
	   prErr("There are parameters missing after %s!", actualKeyword);
	   return 1;
   }
   if (config->carbonCount == 0) {
          prErr("No carbon codition specified before %s", actualKeyword);
          return 1;
   }

   copyString(token, &(config->carbons[config->carbonCount-1].reason));
   return 0;
}

int parseForwardPkts(char *token, s_fidoconfig *config, s_link *link)
{
   unused(config);

   if (token == NULL) {
           prErr("There are parameters missing after %s!", actualKeyword);
           return 1;
   }

   if (stricmp(token, "secure")==0) link->forwardPkts = fSecure;
   else if (stricmp(token, "on")==0) link->forwardPkts = fOn;
   else return 2;

   return 0;
}

int parseAllowEmptyPktPwd(char *token, s_fidoconfig *config, s_link *link)
{
   unused(config);

   if (token == NULL) {
           prErr("There are parameters missing after %s!", actualKeyword);
           return 1;
   }

   if (stricmp(token, "secure")==0) link->allowEmptyPktPwd = eSecure;
   else if (stricmp(token, "on")==0) link->allowEmptyPktPwd = eOn;
   else return 2;

   return 0;
}

int parseAllowPktAddrDiffer(char *token, s_fidoconfig *config, s_link *link)
{

   unused(config);

   if (token == NULL) {
	   prErr("There are parameters missing after %s!", actualKeyword);
	   return 1;
   }

   if (stricmp(token, "on")==0) link->allowPktAddrDiffer = pdOn;
   else if (stricmp(token, "off")==0) link->allowPktAddrDiffer = pdOff;
   else return 2;

   return 0;
}

int parseNodelistFormat(char *token, s_fidoconfig *config, s_nodelist *nodelist)
{
  char *iToken;

  unused(config);

  if (token  == NULL) {
    prErr("There are parameters missing after %s!", actualKeyword);
    return 1;
  }

  iToken = strLower(sstrdup(token));
  if ((strcmp(iToken, "fts5000") == 0) || (strcmp(iToken, "standard") == 0))
    nodelist->format = fts5000;
  else if (strcmp(iToken, "points24") == 0)
    nodelist->format = points24;
  else if (strcmp(iToken, "points4d") == 0)
    nodelist->format = points4d;
  else {
    free(iToken);
    return 2;
  }

  free(iToken);
  return 0;
}

int parseTypeDupes(char *line, e_typeDupeCheck *typeDupeBase, unsigned *DayAge)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "textdupes")==0) *typeDupeBase = textDupes;
  else if (strcmp(iLine, "hashdupes")==0) *typeDupeBase = hashDupes;
  else if (strcmp(iLine, "hashdupeswmsgid")==0) *typeDupeBase = hashDupesWmsgid;
  else if (strcmp(iLine, "commondupebase")==0) {
    *typeDupeBase = commonDupeBase;
    if (*DayAge==0) *DayAge=(unsigned) 5;
  }
  else {
    prErr("Unknown type base of dupes %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}


int parseSaveTic(const s_fidoconfig *config, char *token, s_savetic *savetic)
{
   char *tok;
   DIR  *dirent;

   unused(config);

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   memset(savetic, 0, sizeof(s_savetic));

   tok = strtok(token, " \t");
   if (tok == NULL) {
      prErr("There is a areaname mask missing after %s!", actualKeyword);
      return 1;         // if there is no areaname mask
   }

   savetic->fileAreaNameMask= (char *) smalloc(strlen(tok)+1);
   strcpy(savetic->fileAreaNameMask, tok);

   tok = strtok(NULL, " \t");
   if (tok == NULL) {
      prErr("There is a pathname missing %s!", actualLine);
      return 2;         // if there is no filename
   }
      if (tok[strlen(tok)-1] == PATH_DELIM) {
         savetic->pathName = (char *) smalloc(strlen(tok)+1);
         strcpy(savetic->pathName, tok);
      } else {
         savetic->pathName = (char *) smalloc(strlen(tok)+2);
         strcpy(savetic->pathName, tok);
         savetic->pathName[strlen(tok)] = PATH_DELIM;
         savetic->pathName[strlen(tok)+1] = '\0';
      }

   dirent = opendir(savetic->pathName);
   if (dirent == NULL) {
      prErr("Path %s not found!", savetic->pathName);
      return 2;
   }

   closedir(dirent);
   return 0;

}

int parseSaveTicStatement(char *token, s_fidoconfig *config)
{
   int rc;

   if (token == NULL) {
      prErr("There are parameters missing after %s!", actualKeyword);
      return 1;
   }

   config->saveTic = srealloc(config->saveTic,sizeof(s_savetic)*(config->saveTicCount+1));
   rc = parseSaveTic(config, token,&(config->saveTic[config->saveTicCount]));
   config->saveTicCount++;
   return rc;
}

int parseExecOnFile(char *line, s_fidoconfig *config) {
   char   *a, *f, *c;
   s_execonfile *execonfile;

   if (line == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   a = strtok(line, " \t");
   f = strtok(NULL, " \t");
   c = getRestOfLine();
   if ((a != NULL) && (f != NULL) && (c != NULL)) {

      // add new execonfile statement
      config->execonfileCount++;
      config->execonfile = srealloc(config->execonfile, config->execonfileCount * sizeof(s_execonfile));

      // fill new execonfile statement
      execonfile = &(config->execonfile[config->execonfileCount-1]);
      execonfile->filearea = (char *) smalloc(strlen(a)+1);
      strcpy(execonfile->filearea, a);
      execonfile->filename = (char *) smalloc(strlen(f)+1);
      strcpy(execonfile->filename, f);
      execonfile->command = (char *) smalloc(strlen(c)+1);
      strcpy(execonfile->command, c);
      return 0;

   } else {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }
}

void printNodelistError(void)
{
  prErr("You must define a nodelist first before you use %s!", actualKeyword);
}

int parseLinkDefaults(char *token, s_fidoconfig *config)
{

   if (token==NULL) {
      config->describeLinkDefaults = 1;
   } else {

	   if (stricmp(token, "begin")==0) config->describeLinkDefaults = 1;
       else if (stricmp(token, "end")==0) config->describeLinkDefaults = 0;
       else if (stricmp(token, "destroy")==0) {
		   config->describeLinkDefaults = 0;
		   freeLink(config->linkDefaults);
		   free(config->linkDefaults);
		   config->linkDefaults = NULL;
       }
       else return 2;
   }

   if (config->describeLinkDefaults && config->linkDefaults==NULL) {

      config->linkDefaults = scalloc(1, sizeof(s_link));

      // Set defaults like in parseLink()

      // set areafix default to on
      config->linkDefaults->AreaFix = 1;
      config->linkDefaults->FileFix = 1;

      // set defaults to export, import, mandatory
      config->linkDefaults->export = 1;
      config->linkDefaults->import = 1;
      config->linkDefaults->ourAka = &(config->addr[0]);

      // set defaults maxUnpackedNetmail
      config->linkDefaults->maxUnpackedNetmail = 100;
   }


   return 0;
}

int parseNamesCase(char *line, e_nameCase *value)
{
   if (line == NULL) {
      prErr("Parameter missing after %s!", actualKeyword);
      return 1;
   }

   if (stricmp(line, "lower") == 0) *value = eLower;
   else if (stricmp(line, "upper") == 0) *value = eUpper;
   else {
      prErr("Unknown case parameter %s!", line);
      return 2;
   }
   return 0;
}

int parseNamesCaseConversion(char *line, e_nameCaseConvertion *value)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "lower") == 0) *value = cLower;
  else if (strcmp(iLine, "upper") == 0) *value = cUpper;
  else if (strcmp(iLine, "dont") == 0) *value = cDontTouch;
  else if (strcmp(iLine, "donttouch") == 0) *value = cDontTouch;
  else if (strcmp(iLine, "same") == 0) *value = cDontTouch;
  else {
    prErr("Unknown case convertion parameter %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}

int parseBundleNameStyle(char *line, e_bundleFileNameStyle *value)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "addrdiff") == 0) *value = eAddrDiff;
  else if (strcmp(iLine, "addrdiffalways") == 0) *value = eAddrDiffAlways;
  else if (strcmp(iLine, "timestamp") == 0) *value = eTimeStamp;
  else if (strcmp(iLine, "amiga") == 0) *value = eAmiga;
  else {
    prErr("Unknown bundle name style %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}

int parseLinkWithILogType(char *line, e_linkWithImportLog *value)
{
  char *iLine;

  if (line == NULL) {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  if (*value) {
    prErr("LinkWithImportLog redefinition");
    return 2;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "yes") == 0) *value = lwiYes;
  else if (strcmp(iLine, "no") == 0) *value = lwiNo;
  else if (strcmp(iLine, "kill") == 0) *value = lwiKill;
  else {
    prErr("Unknown LinkWithImportLog value %s!", line);
    free(iLine);
    return 2;
   }
  free(iLine);
  return 0;
}

int parseKludgeAreaNetmailType(char *line, e_kludgeAreaNetmail *value)
{
  char *iLine;

  if (line == NULL) {
	  prErr("Parameter missing after %s!", actualKeyword);
	  return 1;
  }

  if (*value) {
	  prErr("kludgeAreaNetmail redefinition");
	  return 2;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "kill") == 0) *value = kanKill;
  else if (strcmp(iLine, "ignore") == 0) *value = kanIgnore;
  else if (strcmp(iLine, "echomail") == 0) *value = kanEcho;
  else {
	  prErr("Unknown klugdeAreaNetmail value %s!", line);
	  free(iLine);
	  return 2;
  }
  free(iLine);
  return 0;
}

int parseEmailEncoding(char *line, e_emailEncoding *value)
{
  char *iLine;

  if (line == NULL)
  {
    prErr("Parameter missing after %s!", actualKeyword);
    return 1;
  }

  iLine = strLower(sstrdup(line));
  if (strcmp(iLine, "uue") == 0) *value = eeUUE;
  else if (strcmp(iLine, "mime") == 0) *value = eeMIME;
  else if (strcmp(iLine, "seat") == 0) *value = eeSEAT;
  else
  {
    prErr("Unknown email encoding parameter %s!", line);
    free(iLine);
    return 2;
  }
  free(iLine);
  return 0;
}

// options: <flType> <destFile> <dirHdrTpl> <dirEntryTpl> <dirFtrTpl> [<globHdrTpl> <globFtrTpl>]
int parseFilelist(char *line, s_fidoconfig *config)
{
  char *lineTmp;
  s_filelist *curFl;
  char *flType = NULL;
  unsigned int numCopied;

  // add new template
  config->filelistCount++;
  config->filelists = realloc(config->filelists, config->filelistCount * sizeof(s_filelist));
  curFl = &config->filelists[config->filelistCount - 1];
  memset(curFl, 0, sizeof(s_filelist));

  // parse type
  numCopied = copyStringUntilSep(line, " ", &flType);
  if (!numCopied) return 1;
  strLower(flType);

  if (!strcmp(flType, "dir")) curFl->flType = flDir;
  else if (!strcmp(flType, "global")) curFl->flType = flGlobal;
  else if (!strcmp(flType, "dirlist")) curFl->flType = flDirList;
  else
  {
    prErr("Unknown filelist type %s!", flType);
    nfree(flType);
    return 2;
  }
  nfree(flType);

  // parse destFile
  lineTmp = line + numCopied;
  if (*lineTmp) lineTmp++;
  numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->destFile));
  if (!numCopied) return 1;

  if ((curFl->flType == flDir) || (curFl->flType == flGlobal))
  {
    // parse dirHdrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirHdrTpl));
    if (!numCopied) return 1;

    // parse dirEntryTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirEntryTpl));
    if (!numCopied) return 1;

    // parse dirFtrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirFtrTpl));
    if (!numCopied) return 1;
  }

  switch (curFl->flType)
  {
  case flGlobal:
    // parse globHdrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->globHdrTpl));
    if (!numCopied) return 1;

    // parse globFtrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->globFtrTpl));
    if (!numCopied) return 1;
    break;

  case flDirList:
    // parse dirListHdrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListHdrTpl));
    if (!numCopied) return 1;

    // parse dirListEntryTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListEntryTpl));
    if (!numCopied) return 1;

    // parse dirListFtrTpl
    lineTmp += numCopied;
    if (*lineTmp) lineTmp++;
    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListFtrTpl));
    if (!numCopied) return 1;
    break;

  case flDir:
    // just avoid a warning
    break;
  }

  return 0;
}

int parseLine(char *line, s_fidoconfig *config)
{
   char *token, *temp;
   char *iToken;
   int rc = 0;
   s_link   *clink = NULL;

#ifdef __TURBOC__
   int unrecognised = 0;
#endif

   actualLine = temp = (char *) smalloc(strlen(line)+1);
   strcpy(temp, line);

   actualKeyword = token = strtok(temp, " \t");

   //printf("Parsing: %s\n", line);
   //printf("token: %s - %s\n", line, strtok(NULL, "\0"));
   if (token)
   {
     iToken = strLower(sstrdup(token));
     if (strcmp(iToken, "commentchar")==0) rc = parseComment(getRestOfLine(), config);
     else if (strcmp(iToken, "version")==0) rc = parseVersion(getRestOfLine(), config);
     else if (strcmp(iToken, "name")==0) rc = copyString(getRestOfLine(), &(config->name));
     else if (strcmp(iToken, "location")==0) rc = copyString(getRestOfLine(), &(config->location));
     else if (strcmp(iToken, "sysop")==0) rc = copyString(getRestOfLine(), &(config->sysop));
     else if (strcmp(iToken, "address")==0) rc = parseAddress(getRestOfLine(), config);
     else if (strcmp(iToken, "inbound")==0) rc = parsePath(getRestOfLine(), &(config->inbound));
     else if (strcmp(iToken, "protinbound")==0) rc = parsePath(getRestOfLine(), &(config->protInbound));
     else if (strcmp(iToken, "listinbound")==0) rc = parsePath(getRestOfLine(), &(config->listInbound));
     else if (strcmp(iToken, "localinbound")==0) rc= parsePath(getRestOfLine(), &(config->localInbound));
     else if (strcmp(iToken, "tempinbound")==0) rc= parsePath(getRestOfLine(), &(config->tempInbound));
     else if (strcmp(iToken, "outbound")==0) rc = parsePath(getRestOfLine(), &(config->outbound));
     else if (strcmp(iToken, "ticoutbound")==0) rc = parsePath(getRestOfLine(), &(config->ticOutbound));
     else if (strcmp(iToken, "public")==0) rc = parsePublic(getRestOfLine(), config);
     else if (strcmp(iToken, "logfiledir")==0) rc = parsePath(getRestOfLine(), &(config->logFileDir));
     else if (strcmp(iToken, "dupehistorydir")==0) rc = parsePath(getRestOfLine(), &(config->dupeHistoryDir));
     else if (strcmp(iToken, "nodelistdir")==0) rc = parsePath(getRestOfLine(), &(config->nodelistDir));
     else if (strcmp(iToken, "fileareabasedir")==0) rc = parsePath(getRestOfLine(), &(config->fileAreaBaseDir));
     else if (strcmp(iToken, "passfileareadir")==0) rc = parsePath(getRestOfLine(), &(config->passFileAreaDir));
     else if (strcmp(iToken, "busyfiledir")==0) rc = parsePath(getRestOfLine(), &(config->busyFileDir));
     else if (strcmp(iToken, "msgbasedir")==0) rc = parsePath(getRestOfLine(), &(config->msgBaseDir));
	 else if (strcmp(iToken, "linkmsgbasedir")==0) rc = parsePath(getRestOfLine(), &(getDescrLink(config)->msgBaseDir));
     else if (strcmp(iToken, "magic")==0) rc = parsePath(getRestOfLine(), &(config->magic));
     else if (strcmp(iToken, "semadir")==0) rc = parsePath(getRestOfLine(), &(config->semaDir));
     else if (strcmp(iToken, "badfilesdir")==0) rc = parsePath(getRestOfLine(), &(config->badFilesDir));
     else if ((strcmp(iToken, "netmailarea")==0) ||
	      (strcmp(iToken, "netarea")==0))
       rc = parseNetMailArea(getRestOfLine(), config);
     else if (strcmp(iToken, "dupearea")==0) rc = parseArea(config, getRestOfLine(), &(config->dupeArea));
     else if (strcmp(iToken, "badarea")==0) rc = parseArea(config, getRestOfLine(), &(config->badArea));
     else if (strcmp(iToken, "echoarea")==0) rc = parseEchoArea(getRestOfLine(), config);
     else if (strcmp(iToken, "filearea")==0) rc = parseFileAreaStatement(getRestOfLine(), config);
     else if (strcmp(iToken, "bbsarea")==0) rc = parseBbsAreaStatement(getRestOfLine(), config);
     else if (strcmp(iToken, "localarea")==0) rc = parseLocalArea(getRestOfLine(), config);
     else if (strcmp(iToken, "remap")==0) rc = parseRemap(getRestOfLine(),config);
     else if (strcmp(iToken, "link")==0) rc = parseLink(getRestOfLine(), config);
#ifdef __TURBOC__
     else unrecognised++;
#else
     else
#endif
     if (strcmp(iToken, "password")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parsePWD(getRestOfLine(), &clink->defaultPwd);
	 // this way used because of redefinition
	 // defaultPwd from linkdefaults (if exist)
	 clink->pktPwd = clink->defaultPwd;
	 clink->ticPwd = clink->defaultPwd;
	 clink->areaFixPwd = clink->defaultPwd;
	 clink->fileFixPwd = clink->defaultPwd;
	 clink->bbsPwd = clink->defaultPwd;
	 clink->sessionPwd = clink->defaultPwd;
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "aka")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 string2addr(getRestOfLine(), &clink->hisAka);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "ouraka")==0) {
       rc = 0;
       if( (clink = getDescrLink(config)) != NULL ) {
	 clink->ourAka = getAddr(*config, getRestOfLine());
	 if (clink->ourAka == NULL) rc = 2;
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "autoareacreate")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->autoAreaCreate);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "autofilecreate")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->autoFileCreate);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "forwardrequests")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->forwardRequests);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "forwardfilerequests")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->forwardFileRequests);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "denyfwdreqaccess") == 0) {
		 if( (clink = getDescrLink(config)) != NULL ) {
			 rc = parseBool (getRestOfLine(), &clink->denyFRA);
		 } else rc = 1;
     }
     else if (strcmp(iToken, "forwardpkts")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseForwardPkts(getRestOfLine(), config, clink);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "allowemptypktpwd")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseAllowEmptyPktPwd(getRestOfLine(), config, clink);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "packnetmail")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool(getRestOfLine(), &clink->packNetmail);
       }
       else rc = 1;
     }
     else if (strcmp(iToken, "allowpktaddrdiffer")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseAllowPktAddrDiffer(getRestOfLine(), config, clink);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "autoareacreatedefaults")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = copyString(getRestOfLine(), &clink->autoAreaCreateDefaults);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "autofilecreatedefaults")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = copyString(getRestOfLine(), &clink->autoFileCreateDefaults);
       }
       else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "areafix")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->AreaFix);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "filefix")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->FileFix);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "pause")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->Pause);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "notic")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->noTIC);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "delnotrecievedtic")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->delNotRecievedTIC);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "advancedareafix")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->advancedAreafix);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "autopause")==0) rc = parseAutoPause(getRestOfLine(), &(getDescrLink(config)->autoPause));
     else if (strcmp(iToken, "remoterobotname")==0) rc = copyString(getRestOfLine(), &(getDescrLink(config)->RemoteRobotName));
     else if (strcmp(iToken, "remotefilerobotname")==0) rc = copyString(getRestOfLine(), &(getDescrLink(config)->RemoteFileRobotName));
     else if (strcmp(iToken, "forwardareapriority")==0) rc = parseUInt(getRestOfLine(), &(getDescrLink(config)->forwardAreaPriority));
     else if (strcmp(iToken, "forwardfilepriority")==0) rc = parseUInt(getRestOfLine(), &(getDescrLink(config)->forwardFilePriority));
	 else if (strcmp(iToken, "denyuncondfwdreqaccess")==0) rc = parseBool(getRestOfLine(), &(getDescrLink(config)->denyUFRA));

     else if (strcmp(iToken, "export")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->export);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "import")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->import);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "mandatory")==0 || strcmp(iToken, "manual")==0) {
       if( (clink = getDescrLink(config)) != NULL ) {
	 rc = parseBool (getRestOfLine(), &clink->mandatory);
       } else {
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "optgrp")==0) rc = parseGroup(getRestOfLine(), config, 3);
     else if (strcmp(iToken, "forwardrequestmask")==0) rc = parseGroup(getRestOfLine(), config, 4);
     else if (strcmp(iToken, "denyfwdmask")==0) rc = parseGroup(getRestOfLine(), config, 5);
     else if (strcmp(iToken, "level")==0) rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->level));
     else if (strcmp(iToken, "areafixecholimit")==0) rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->afixEchoLimit));
#ifdef __TURBOC__
     else unrecognised++;
#else
     else
#endif
       if (strcmp(iToken, "arcmailsize")==0) rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->arcmailSize));
     else if (strcmp(iToken, "pktsize")==0) rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->pktSize));
     else if (strcmp(iToken, "maxunpackednetmail")==0) rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->maxUnpackedNetmail));
     else if (strcmp(iToken, "pktpwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->pktPwd));
     else if (strcmp(iToken, "ticpwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->ticPwd));
     else if (strcmp(iToken, "areafixpwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->areaFixPwd));
     else if (strcmp(iToken, "filefixpwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->fileFixPwd));
     else if (strcmp(iToken, "bbspwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->bbsPwd));
     else if (strcmp(iToken, "sessionpwd")==0) rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->sessionPwd));
     else if (strcmp(iToken, "handle")==0) rc = parseHandle(getRestOfLine(), config);
     else if (strcmp(iToken, "email")==0) rc = copyString(getRestOfLine(), &(getDescrLink(config)->email));
     else if (strcmp(iToken, "emailfrom")==0) rc = copyString(getRestOfLine(), &(getDescrLink(config)->emailFrom));
     else if (strcmp(iToken, "emailsubj")==0) rc = copyString(getRestOfLine(), &(getDescrLink(config)->emailSubj));
     else if (strcmp(iToken, "emailencoding")==0) rc = parseEmailEncoding(getRestOfLine(), &(getDescrLink(config)->emailEncoding));
     else if (strcmp(iToken, "echomailflavour")==0) rc = parseEchoMailFlavour(getRestOfLine(), &(getDescrLink(config)->echoMailFlavour));
     else if (strcmp(iToken, "fileechoflavour")==0) rc = parseFileEchoFlavour(getRestOfLine(), &(getDescrLink(config)->fileEchoFlavour));
     else if (strcmp(iToken, "route")==0) rc = parseRoute(getRestOfLine(), config, &(config->route), &(config->routeCount), id_route);
     else if (strcmp(iToken, "routefile")==0) rc = parseRoute(getRestOfLine(), config, &(config->route), &(config->routeCount), id_routeFile);
     else if (strcmp(iToken, "routemail")==0) rc = parseRoute(getRestOfLine(), config, &(config->route), &(config->routeCount), id_routeMail);
     else if (strcmp(iToken, "pack")==0) rc = parsePack(getRestOfLine(), config);
     else if (strcmp(iToken, "unpack")==0) rc = parseUnpack(getRestOfLine(), config);
     else if (strcmp(iToken, "packer")==0) rc = parsePackerDef(getRestOfLine(), config, &(getDescrLink(config)->packerDef));

     else if (strcmp(iToken, "intab")==0) rc = parseFileName(getRestOfLine(), &(config->intab));
     else if (strcmp(iToken, "outtab")==0) rc = parseFileName(getRestOfLine(), &(config->outtab));

     else if (strcmp(iToken, "areafixhelp")==0) rc = parseFileName(getRestOfLine(), &(config->areafixhelp));
     else if (strcmp(iToken, "filefixhelp")==0) rc = parseFileName(getRestOfLine(), &(config->filefixhelp));
     else if (strcmp(iToken, "forwardrequestfile")==0) rc = parseFileName(getRestOfLine(), &(getDescrLink(config)->forwardRequestFile));
     else if (strcmp(iToken, "denyfwdfile")==0) rc = parseFileName(getRestOfLine(), &(getDescrLink(config)->denyFwdFile));
     else if (strcmp(iToken, "forwardfilerequestfile")==0) rc = parseFileName(getRestOfLine(), &(getDescrLink(config)->forwardFileRequestFile));
     else if (strcmp(iToken, "autoareacreatefile")==0) rc = parseFileName(getRestOfLine(), &(getDescrLink(config)->autoAreaCreateFile));
     else if (strcmp(iToken, "autofilecreatefile")==0) rc = parseFileName(getRestOfLine(), &(getDescrLink(config)->autoFileCreateFile));
     else if (strcmp(iToken, "linkbundlenamestyle")==0) rc = parseBundleNameStyle(getRestOfLine(), &(getDescrLink(config)->linkBundleNameStyle));


     else if (strcmp(iToken, "echotosslog")==0) rc = copyString(getRestOfLine(), &(config->echotosslog));
     else if (strcmp(iToken, "statlog")==0) rc = copyString(getRestOfLine(), &(config->statlog));
     else if (strcmp(iToken, "importlog")==0) rc = copyString(getRestOfLine(), &(config->importlog));
     else if (strcmp(iToken, "linkwithimportlog")==0) rc = parseLinkWithILogType(getRestOfLine(), &(config->LinkWithImportlog));
     else if (strcmp(iToken, "kludgeareanetmail")==0) rc = parseKludgeAreaNetmailType(getRestOfLine(), &(config->kludgeAreaNetmail));
     else if (strcmp(iToken, "fileareaslog")==0) rc = parseFileName(getRestOfLine(), &(config->fileAreasLog));
     else if (strcmp(iToken, "filenewareaslog")==0) rc = parseFileName(getRestOfLine(), &(config->fileNewAreasLog));
     else if (strcmp(iToken, "longnamelist")==0) rc = parseFileName(getRestOfLine(), &(config->longNameList));
     else if (strcmp(iToken, "filearclist")==0) rc = parseFileName(getRestOfLine(), &(config->fileArcList));
     else if (strcmp(iToken, "filepasslist")==0) rc = parseFileName(getRestOfLine(), &(config->filePassList));
     else if (strcmp(iToken, "filedupelist")==0) rc = parseFileName(getRestOfLine(), &(config->fileDupeList));
     else if (strcmp(iToken, "msgidfile")==0) rc = parseFileName(getRestOfLine(), &(config->fileDupeList));
     else if (strcmp(iToken, "loglevels")==0) rc = copyString(getRestOfLine(), &(config->loglevels));
     else if (strcmp(iToken, "screenloglevels")==0) rc = copyString(getRestOfLine(), &(config->screenloglevels));

     else if (strcmp(iToken, "accessgrp")==0) rc = parseGroup(getRestOfLine(), config, 0);
     else if (strcmp(iToken, "linkgrp")==0) rc = parseGroup(getRestOfLine(), config, 1);

     else if (strcmp(iToken, "carbonto")==0) rc = parseCarbon(getRestOfLine(),config, ct_to);
     else if (strcmp(iToken, "carbonfrom")==0) rc = parseCarbon(getRestOfLine(), config, ct_from);
     else if (strcmp(iToken, "carbonaddr")==0) rc = parseCarbon(getRestOfLine(), config, ct_addr);
     else if (strcmp(iToken, "carbonkludge")==0) rc = parseCarbon(getRestOfLine(), config, ct_kludge);
     else if (strcmp(iToken, "carbonsubj")==0) rc = parseCarbon(getRestOfLine(), config, ct_subject);
     else if (strcmp(iToken, "carbontext")==0) rc = parseCarbon(getRestOfLine(), config, ct_msgtext);
     else if (strcmp(iToken, "carboncopy")==0) rc = parseCarbonArea(getRestOfLine(), config, 0);
     else if (strcmp(iToken, "carbonmove")==0) rc = parseCarbonArea(getRestOfLine(), config, 1);
     else if (strcmp(iToken, "carbonextern")==0) rc = parseCarbonExtern(getRestOfLine(), config);
/* +AS+ */
     else if (strcmp(iToken, "netmailextern")==0) rc = parseCarbonExtern(getRestOfLine(), config);
/* -AS- */
     else if (strcmp(iToken, "carbondelete")==0) rc = parseCarbonDelete(getRestOfLine(), config);
     else if (strcmp(iToken, "carbonreason")==0) rc = parseCarbonReason(getRestOfLine(), config);
     else if (strcmp(iToken, "excludepassthroughcarbon")==0) rc = parseBool(getRestOfLine(), &(config->exclPassCC));
#ifdef __TURBOC__
     else unrecognised++;
#else
     else
#endif
       if (strcmp(iToken, "lockfile")==0) rc = copyString(getRestOfLine(), &(config->lockfile));
     else if (strcmp(iToken, "tempoutbound")==0) rc = parsePath(getRestOfLine(), &(config->tempOutbound));
     else if (strcmp(iToken, "areafixfrompkt")==0) rc = parseBool(getRestOfLine(), &(config->areafixFromPkt));
     else if (strcmp(iToken, "areafixkillreports")==0) rc = parseBool(getRestOfLine(), &(config->areafixKillReports));
     else if (strcmp(iToken, "areafixkillrequests")==0) rc = parseBool(getRestOfLine(), &(config->areafixKillRequests));
     else if (strcmp(iToken, "filefixkillreports")==0) rc = parseBool(getRestOfLine(), &(config->filefixKillReports));
     else if (strcmp(iToken, "filefixkillrequests")==0) rc = parseBool(getRestOfLine(), &(config->filefixKillRequests));
     else if (strcmp(iToken, "createdirs")==0) rc = parseBool(getRestOfLine(), &(config->createDirs));
     else if (strcmp(iToken, "longdirnames")==0) rc = parseBool(getRestOfLine(), &(config->longDirNames));
     else if (strcmp(iToken, "splitdirs")==0) rc = parseBool(getRestOfLine(), &(config->splitDirs));
     else if (strcmp(iToken, "adddlc")==0) rc = parseBool(getRestOfLine(), &(config->addDLC));
     else if (strcmp(iToken, "filesingledescline")==0) rc = parseBool(getRestOfLine(), &(config->fileSingleDescLine));
     else if (strcmp(iToken, "filecheckdest")==0) rc = parseBool(getRestOfLine(), &(config->fileCheckDest));
     else if (strcmp(iToken, "publicgroup")==0) rc = parseGroup(getRestOfLine(), config, 2);
     else if (strcmp(iToken, "logechotoscreen")==0) rc = parseBool(getRestOfLine(), &(config->logEchoToScreen));
     else if (strcmp(iToken, "separatebundles")==0) rc = parseBool(getRestOfLine(), &(config->separateBundles));
     else if (strcmp(iToken, "carbonandquit")==0) rc = parseBool(getRestOfLine(), &(config->carbonAndQuit));
     else if (strcmp(iToken, "carbonkeepsb")==0) rc = parseBool(getRestOfLine(), &(config->carbonKeepSb));
     else if (strcmp(iToken, "carbonout")==0) rc = parseBool(getRestOfLine(), &(config->carbonOut));
     else if (strcmp(iToken, "ignorecapword")==0) rc = parseBool(getRestOfLine(), &(config->ignoreCapWord));
     else if (strcmp(iToken, "noprocessbundles")==0) rc = parseBool(getRestOfLine(), &(config->noProcessBundles));
     else if (strcmp(iToken, "reportto")==0) rc = copyString(getRestOfLine(), &(config->ReportTo));
     else if (strcmp(iToken, "execonfile")==0) rc = parseExecOnFile(getRestOfLine(), config);
     else if (strcmp(iToken, "defarcmailsize")==0) rc = parseNumber(getRestOfLine(), 10, &(config->defarcmailSize));
     else if (strcmp(iToken, "areafixmsgsize")==0) rc = parseNumber(getRestOfLine(), 10, &(config->areafixMsgSize));
     else if (strcmp(iToken, "afterunpack")==0) rc = copyString(getRestOfLine(), &(config->afterUnpack));
     else if (strcmp(iToken, "beforepack")==0) rc = copyString(getRestOfLine(), &(config->beforePack));
     else if (strcmp(iToken, "processpkt")==0) rc = copyString(getRestOfLine(), &(config->processPkt));
     else if (strcmp(iToken, "areafixsplitstr")==0) rc = copyString(getRestOfLine(), &(config->areafixSplitStr));
     else if (strcmp(iToken, "areafixorigin")==0) rc = copyString(getRestOfLine(), &(config->areafixOrigin));
     else if (strcmp(iToken, "robotsarea")==0) rc = copyString(getRestOfLine(), &(config->robotsArea));
     else if (strcmp(iToken, "filedescpos")==0) rc = parseUInt(getRestOfLine(), &(config->fileDescPos));
     else if (strcmp(iToken, "dlcdigits")==0) rc = parseUInt(getRestOfLine(), &(config->DLCDigits));
     else if (strcmp(iToken, "filemaxdupeage")==0) rc = parseUInt(getRestOfLine(), &(config->fileMaxDupeAge));
     else if (strcmp(iToken, "filefileumask")==0) rc = parseOctal(getRestOfLine(), &(config->fileFileUMask));
     else if (strcmp(iToken, "filedirumask")==0) rc = parseOctal(getRestOfLine(), &(config->fileDirUMask));
     else if (strcmp(iToken, "origininannounce")==0) rc = parseBool(getRestOfLine(), &(config->originInAnnounce));
     else if (strcmp(iToken, "maxticlinelength")==0) rc = parseUInt(getRestOfLine(), &(config->MaxTicLineLength));
     else if (strcmp(iToken, "filelocalpwd")==0) rc = copyString(getRestOfLine(), &(config->fileLocalPwd));
     else if (strcmp(iToken, "fileldescstring")==0) rc = copyString(getRestOfLine(), &(config->fileLDescString));
     else if (strcmp(iToken, "savetic")==0) rc = parseSaveTicStatement(getRestOfLine(), config);
     else if (strcmp(iToken, "areasmaxdupeage")==0) rc = parseNumber(getRestOfLine(), 10, &(config->areasMaxDupeAge));
     else if (strcmp(iToken, "dupebasetype")==0) rc = parseTypeDupes(getRestOfLine(), &(config->typeDupeBase), &(config->areasMaxDupeAge));
#ifdef __TURBOC__
     else unrecognised++;
#else
     else
#endif
       if (strcmp(iToken, "fidouserlist") ==0)
         rc = copyString(getRestOfLine(), &(config->fidoUserList));
     else if (strcmp(iToken, "nodelist") ==0)
       rc = parseNodelist(getRestOfLine(), config);
     else if (strcmp(iToken, "diffupdate") ==0) {
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
     else if (strcmp(iToken, "fullupdate") ==0) {
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
     else if (strcmp(iToken, "defaultzone") ==0) {
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
     else if (strcmp(iToken, "nodelistformat") ==0) {
       if (config->nodelistCount > 0) {
	 rc = parseNodelistFormat(getRestOfLine(), config,
				  &(config->nodelists[config->nodelistCount-1]));
       }
       else {
	 printNodelistError();
	 rc = 1;
       }
     }
     else if (strcmp(iToken, "logowner")==0) rc = parseOwner(getRestOfLine(), &(config->loguid), &(config->loggid));
     else if (strcmp(iToken, "logperm")==0) rc = parseNumber(getRestOfLine(), 8, &(config->logperm));
     else if (strcmp(iToken, "linkdefaults")==0) rc = parseLinkDefaults(getRestOfLine(), config);
     else if (strcmp(iToken, "createareascase")==0) rc = parseNamesCase(getRestOfLine(), &(config->createAreasCase));
     else if (strcmp(iToken, "areasfilenamecase")==0) rc = parseNamesCase(getRestOfLine(), &(config->areasFileNameCase));
     else if (strcmp(iToken, "convertlongnames")==0) rc = parseNamesCaseConversion(getRestOfLine(), &(config->convertLongNames));
     else if (strcmp(iToken, "convertshortnames")==0) rc = parseNamesCaseConversion(getRestOfLine(), &(config->convertShortNames));
     else if (strcmp(iToken, "disabletid")==0) rc = parseBool(getRestOfLine(), &(config->disableTID));
     else if (strcmp(iToken, "tossingext")==0) {
       if ((temp=getRestOfLine()) != NULL)
	 rc = copyString(temp, &(config->tossingExt));
       else config->tossingExt = NULL;
     }

#if defined ( __NT__ )
     else if (strcmp(iToken, "setconsoletitle")==0) rc = parseBool(getRestOfLine(), &(config->setConsoleTitle));
#endif
     else if (strcmp(iToken,"addtoseen")==0) rc = parseSeenBy2D(getRestOfLine(),&(config->addToSeen), &(config->addToSeenCount));
     else if (strcmp(iToken,"ignoreseen")==0) rc = parseSeenBy2D(getRestOfLine(),&(config->ignoreSeen), &(config->ignoreSeenCount));
     else if (strcmp(iToken, "tearline")==0) rc = copyString(getRestOfLine(), &(config->tearline));
     else if (strcmp(iToken, "origin")==0) rc = copyString(getRestOfLine(), &(config->origin));
     else if (strcmp(iToken, "bundlenamestyle")==0) rc = parseBundleNameStyle(getRestOfLine(), &(config->bundleNameStyle));
     else if (strcmp(iToken, "keeptrsmail")==0) rc = parseBool(getRestOfLine(), &(config->keepTrsMail));
     else if (strcmp(iToken, "keeptrsfiles")==0) rc = parseBool(getRestOfLine(), &(config->keepTrsFiles));
     else if (strcmp(iToken, "filelist")==0) rc = parseFilelist(getRestOfLine(), config);
     else if (strcmp(iToken, "createfwdnonpass")==0) rc = parseBool(getRestOfLine(), &(config->createFwdNonPass));
	 else if (strcmp(iToken, "autopassive")==0) rc = parseBool(getRestOfLine(), &(config->autoPassive));
     else if (strcmp(iToken, "netmailflag")==0) rc = copyString(getRestOfLine(), &(config->netmailFlag));
     else if (strcmp(iToken, "autoareacreateflag")==0) rc = copyString(getRestOfLine(), &(config->aacFlag));

#ifdef __TURBOC__
     else unrecognised++;
     if (unrecognised == 5) {
#else
     else {
#endif
       prErr( "unrecognized: %s", line);
       wasError = 1;
       free(iToken);
       free(actualLine);
       return 1;
     }

     free(iToken);
   }
   if (rc != 0) {
      prErr( "error %d in: %s", rc, line);
      wasError = 1;
   }

   free(actualLine);
   return rc;
}
