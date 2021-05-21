/* $Id$ */
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
#include <assert.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <huskylib/compiler.h>

#ifdef HAS_UNISTD_H
#   include <unistd.h>
#endif

#ifdef HAS_IO_H
#   include <io.h>
#endif

#ifdef HAS_PWD_H
#   include <pwd.h>
#endif

#ifdef HAS_GRP_H
#   include <grp.h>
#endif

#ifdef HAS_SYSEXITS_H
#include <sysexits.h>
#endif
#ifdef HAS_SYS_SYSEXITS_H
#include <sys/sysexits.h>
#endif

#ifdef HAS_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAS_PROCESS_H
#  include <process.h>
#endif

#ifdef HAS_STRINGS_H
#  include <strings.h>
#endif

#include <huskylib/huskylib.h>
/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>
/* smapi */
#include <smapi/msgapi.h>
/* fidoconfig */
#include "fidoconf.h"
#include "common.h"
#include "findtok.h"
#include "tokens.h"
#include "grptree.h"

int fc_trycreate = 0; /* Try to create nonexistant directories (defined in line.c) */
char * actualKeyword = NULL;
char * actualLine = NULL;
int actualLineNr = 0;
char wasError    = 0;
char CommentChar = '#';
int _carbonrule  = CC_AND;
static s_link linkDefined;
static ps_robot curRobot = NULL; /* current robot */
int fc_copyString(char * str, char ** pmem)
{
    if(str == NULL)
    {
        printf("Line %d: There is a parameter missing after %s!\n", actualLineNr, actualKeyword);
        return 1;
    }

    return copyString(stripRoundingChars(str, "\""), pmem);
}

int fc_copyStringWOstrip(char * str, char ** pmem)
{
    if(str == NULL)
    {
        printf("Line %d: There is a parameter missing after %s!\n", actualLineNr, actualKeyword);
        return 1;
    }

    return copyString(str, pmem);
}

/*
   create new address array by copying the old one
 */
int fc_copyAddressArray(unsigned int count, ps_addr sourceAddr, ps_addr * destAddr)
{
    int rc = 0;

    if(count > 0)             /* we have addresses to copy */
    {
        *destAddr = malloc(sizeof(hs_addr) * count);

        if(*destAddr != NULL)
        {
            memcpy(*destAddr, sourceAddr, sizeof(hs_addr) * count);
        }
        else
        {
            rc = 1;           /* error */
        }
    }
    else                      /* nothing to do */
    {
        *destAddr = NULL;     /* reset pointer */
    }

    return rc;
}

#if 0
#define getRestOfLine() stripLeadingChars(strtok(NULL, "\0"), " \t")
#else
char * getRestOfLine(void)
{
    return stripLeadingChars(strtok(NULL, "\0"), " \t");
}

#endif

void prErr(char * string, ...)
{
    va_list ap;

    printf("\"%s\", line %d: ", getCurConfName(), actualLineNr);
    va_start(ap, string);
    vprintf(string, ap);
    va_end(ap);
    putchar('\n');
}

char * getDescription(void)
{
    char * descBuf = NULL, * token;
    int quoted = 0;
    size_t length;

    while((token = strtok(NULL, " \t")) != NULL)
    {
        xstrscat(&descBuf, token, " ", NULLP);

        if(*token == '\"' && !quoted)
        {
            quoted = 1;

            if(token[1] == '\0')
            {
                continue;
            }
        }

        if(quoted && token[strlen(token) - 1] == '\"')
        {
            break;
        }

        if(!quoted)
        {
            break;
        }
    }

    if(descBuf == NULL)
    {
        prErr("Error in area description!");
        return NULL;
    }

    descBuf[length = (strlen(descBuf) - 1)] = '\0'; /*  remove trailing space */

    if(quoted)
    {
        /*  out. cut '"' */
        descBuf[--length] = '\0';
        memmove(descBuf, descBuf + 1, length);
    }

    return descBuf;
} /* getDescription */

int parseVersion(char * token, s_fidoconfig * config)
{
    char buffer[10], * temp = token;
    int i = 0;

    /*  if there is no token return error... */
    if(token == NULL)
    {
        prErr("There is a version number missing after %s!", actualKeyword);
        return 1;
    }

    while(isdigit(*temp) && i < 9)
    {
        buffer[i] = *temp;
        i++;
        temp++;
    }
    buffer[i] = 0;
    config->cfgVersionMajor = atoi(buffer);
    temp++; /*  eat . */
    i = 0;

    while(isdigit(*temp) && i < 9)
    {
        buffer[i] = *temp;
        i++;
        temp++;
    }
    buffer[i] = 0;
    config->cfgVersionMinor = atoi(buffer);
    return 0;
} /* parseVersion */

#define checkRobot() if(!curRobot)printRobotError();
void printRobotError(void)
{
    prErr("Keyword %s must appear inside robot section!", actualKeyword);
    exit(EX_CONFIG);
}

void printLinkError(void)
{
    prErr("You must define a link first before you use %s!", actualKeyword);
    exit(EX_CONFIG);
}

void printAddrError(void)
{
    prErr("The main address must be defined before any areas!");
    exit(EX_CONFIG);
}

s_link * getDescrLink(s_fidoconfig * config)
{
    if(config->describeLinkDefaults)   /*  describing defaults for links */
    {
        return config->linkDefaults;
    }
    else
    {
        if(config->linkCount)
        {
            return config->links[config->linkCount - 1];
        }
        else
        {
            printLinkError();
            return NULL;
        }
    }
}

ps_anndef getDescrAnnDef(s_fidoconfig * config)
{
    if(config->ADCount)
    {
        return &config->AnnDefs[config->ADCount - 1];
    }
    else
    {
        prErr("You must define a AnnAreaTag first before you use %s!", actualKeyword);
        exit(EX_CONFIG);
    }
}

int parseAddress(char * token, s_fidoconfig * config)
{
    char * aka;
    hs_addr parsedaddr =
    {
        0
    };

    if(token == NULL)
    {
        prErr("An address after %s is missing!", actualKeyword);
        return 1;
    }

    aka = strtok(token, " \t"); /*  only look at aka */

    if(aka == NULL)
    {
        prErr("An address after %s is missing!", actualKeyword);
        return 1;
    }

    if(parseFtnAddrZS(aka, &(parsedaddr)) & FTNADDR_ERROR)
    {
        prErr("The address after %s is invalid!", actualKeyword);
        return 1;
    }

    config->addr = srealloc(config->addr, sizeof(hs_addr) * (config->addrCount + 1));
    memcpy(&(config->addr[config->addrCount]), &parsedaddr, sizeof(hs_addr));
    config->addrCount++;
    return 0;
} /* parseAddress */

int parseRemap(char * token, s_fidoconfig * config)
{
    char * param1, * param2, * param3;
    int r = 0, r2 = 0, r3;
    s_remap remap;

    if(token == NULL)
    {
        prErr("All parameters after %s are missing!", actualKeyword);
        return 1;
    }

    memset(&remap, 0, sizeof(remap));
    param1 = strtok(token, ",\t");

    if(param1 == NULL)
    {
        prErr("Name or * (1st field) after %s is missing!", actualKeyword);
        return 1;
    }

    if(strcmp(param1, "*") == 0)
    {
        param1 = NULL;
    }

    param2 = strtok(NULL, ",\t");

    if(param2 == NULL)
    {
        prErr("OldAddress or * (2nd field) after %s is missing!", actualKeyword);
        return 1;
    }

    param3 = strtok(NULL, ",\t");

    if(param3 == NULL)
    {
        prErr("NewAddress (3rd field) after %s is missing!", actualKeyword);
        return 1;
    }

    if(strcmp(param2, "*") == 0)
    {
        if(param1 == NULL)
        {
            prErr("One of the two first parameters of %s must not be \"*\"", actualKeyword);
            r = 1;
        }
    }
    else
    {
        r2 = parseFtnAddrZS(param2, &(remap.oldaddr));
    }

    if(r2 & FTNADDR_ERROR)
    {
        prErr("Invalid address in the 2nd parameter of %s (%s)",
              actualKeyword, param2);
        r = 1;
    }

    r3 = parseFtnAddrZS(param3, &(remap.newaddr));

    if(r3 & FTNADDR_ERROR)
    {
        prErr("Invalid address in the 3rd parameter of %s (%s)",
              actualKeyword, param3);
        r = 1;
    }

    if(r == 0)   /* All OK, allocate memory and store data */
    {
        if(param1)  /*  Name for rerouting is not "*" */
        {
            remap.toname = sstrdup(param1);
        }

        config->remaps = srealloc(config->remaps, (config->remapCount + 1) * sizeof(s_remap));
        memcpy(&config->remaps[config->remapCount], &remap, sizeof(remap));
        config->remapCount++;
    }

    return r;
} /* parseRemap */

/* Parse and check/create directory
 *
 */
int parsePath(char * token, char ** var, char ** alreadyDefined)
{
    if(*var != NULL)
    {
        if(alreadyDefined == NULL || *alreadyDefined)
        {
            prErr("Duplicate path!");
            return 1;
        }

        nfree(*var);
    }

    if(token == NULL)
    {
        prErr("A path after %s is missing!", actualKeyword);
        return 1;
    }

    if(*token && token[strlen(token) - 1] == PATH_DELIM)
    {
        Strip_Trailing(token, PATH_DELIM);
    }

    xscatprintf(var, "%s%c", token, (char)PATH_DELIM);

    if(alreadyDefined)
    {
        *alreadyDefined = *var;
    }

    if(!direxist(*var))
    {
        if(fc_trycreate)
        {
            if(_createDirectoryTree(*var))
            {
                prErr("Path %s not found, can't create: %s", *var, strerror(errno));
                return 1;
            }
            else
            {
                prErr("Path %s created succesfully.", *var);
            }
        }
        else
        {
            prErr("Path %s not found!", *var);
            return 1;
        }
    }

    return 0;
} /* parsePath */

int parseAreaPath(char * token, char ** var, char ** alreadyDefined)
{
/*    char *p, *q, *osvar; */
    if(*var != NULL)
    {
        if(alreadyDefined == NULL || *alreadyDefined)
        {
            prErr("Duplicate path!");
            return 1;
        }

        nfree(*var);
    }

    if(token == NULL)
    {
        prErr("A path after %s is missing!", actualKeyword);
        return 1;
    }

    if(stricmp(token, "passthrough") == 0)
    {
        fc_copyString(token, &(*var));

        if(alreadyDefined)
        {
            *alreadyDefined = *var;
        }

        return 0;
    }

    if(*token && token[strlen(token) - 1] == PATH_DELIM)
    {
        Strip_Trailing(token, PATH_DELIM);
    }

    xscatprintf(var, "%s%c", token, (char)PATH_DELIM);

    if(alreadyDefined)
    {
        *alreadyDefined = *var;
    }

    if(!direxist(*var))
    {
        if(fc_trycreate)
        {
            if(_createDirectoryTree(*var))
            {
                prErr("Path %s not found, can't create: %s", *var, strerror(errno));
                return 1;
            }
            else
            {
                prErr("Path %s created succesfully.", *var);
            }
        }
        else
        {
            prErr("Path %s not found!", *var);
            return 1;
        }
    }

    return 0;
} /* parseAreaPath */

int parseAreaPathExpand(char * token, char ** var, char ** alreadyDefined)
{
    char * p;

    if(*var != NULL)
    {
        if(alreadyDefined == NULL || *alreadyDefined)
        {
            prErr("Duplicate path!");
            return 1;
        }

        nfree(*var);
    }

    if(token == NULL)
    {
        prErr("A path after %s is missing!", actualKeyword);
        return 1;
    }

    if(stricmp(token, "passthrough") == 0)
    {
        fc_copyString(token, &(*var));

        if(alreadyDefined)
        {
            *alreadyDefined = *var;
        }

        return 0;
    }

    p = vars_expand(sstrdup(token));

    if(*p == '\0' || p[strlen(p) - 1] != PATH_DELIM)
    {
        xscatprintf(var, "%s%c", token, (char)PATH_DELIM);
        xscatprintf(&p, "%c", (char)PATH_DELIM);
    }
    else
    {
        *var = sstrdup(token);
    }

    if(alreadyDefined)
    {
        *alreadyDefined = *var;
    }

    if(!direxist(p))
    {
        if(fc_trycreate)
        {
            if(_createDirectoryTree(p))
            {
                prErr("Path %s not found, can't create: %s", p, strerror(errno));
                nfree(p);
                return 1;
            }
            else
            {
                prErr("Path %s created succesfully.", p);
            }
        }
        else
        {
            prErr("Path %s not found!", p);
            nfree(p);
            return 1;
        }
    }

    nfree(p);
    return 0;
} /* parseAreaPathExpand */

int parsePathNoCheck(char * token, char ** var, char ** alreadyDefined)
{
    if(*var != NULL)
    {
        if(alreadyDefined == NULL || *alreadyDefined)
        {
            prErr("Duplicate path!");
            return 1;
        }

        nfree(*var);
    }

    if(token == NULL)
    {
        prErr("A path after %s is missing!", actualKeyword);
        return 1;
    }

    if(*token && token[strlen(token) - 1] == PATH_DELIM)
    {
        Strip_Trailing(token, PATH_DELIM);
    }

    xscatprintf(var, "%s%c", token, (char)PATH_DELIM);

    if(alreadyDefined)
    {
        *alreadyDefined = *var;
    }

    return 0;
} /* parsePathNoCheck */

int parsePublic(char * token, s_fidoconfig * config)
{
    if(token == NULL)
    {
        prErr("A path after %s is missing!", actualKeyword);
        return 1;
    }

    config->publicDir = srealloc(config->publicDir, sizeof(char *) * (config->publicCount + 1));
    config->publicDir[config->publicCount] = NULL;

    if(*token && token[strlen(token) - 1] == PATH_DELIM)
    {
        Strip_Trailing(token, PATH_DELIM);
    }

    xscatprintf(&(config->publicDir[config->publicCount]), "%s%c", token, (char)PATH_DELIM);

    if(!direxist(config->publicDir[config->publicCount]))
    {
        if(fc_trycreate)
        {
            if(_createDirectoryTree(token))
            {
                prErr("Path %s not found, can't create: %s", token, strerror(errno));
                return 1;
            }
            else
            {
                prErr("Path %s created succesfully.", token);
            }
        }
        else
        {
            prErr("Path %s not found!", token);
            return 1;
        }
    }

    config->publicCount++;
    return 0;
} /* parsePublic */

int parseOwner(char * token, unsigned int * uid, unsigned int * gid)
{
#ifdef __UNIX__
    struct passwd * pw;
    struct group * grp;
    char * name, * group, * p;

    if(token == NULL)
    {
        prErr("There are parameters missing after %s!", actualKeyword);
        return 1;
    }

    p = strchr(token, '.');

    if(p)
    {
        *p    = '\0';
        group = p + 1;
    }
    else
    {
        group = NULL;
    }
    name = token;

    pw = getpwnam(name);

    if(*name && pw == NULL)
    {
        prErr("User name %s is unknown to OS !", name);
        return 1;
    }

    *uid = pw ? pw->pw_uid : (unsigned int)-1;

    if(group != NULL)
    {
        grp = getgrnam(group);

        if((*group) && grp == NULL)
        {
            prErr("Group name %s is unknown to OS !", group);
            return 1;
        }

        *gid = grp ? grp->gr_gid : (unsigned int)-1;
    }

#else  /* ifdef __UNIX__ */
    unused(token);
    unused(uid);
    unused(gid);
#endif /* ifdef __UNIX__ */
    return 0;
} /* parseOwner */

int parseNumber(char * token, int radix, unsigned * level)
{
    char * end = NULL;
    unsigned long result;

    if(token == NULL)
    {
        prErr("Parameter after %s is missing!", actualKeyword);
        return 1;
    }

    result = strtoul(token, &end, radix);

    if(!(*end == '\0' && *token != '\0') || result == unsigned_long_max)
    {
        prErr("Error in number representation : %s . %s!", token, end);
        return 1;
    }

    *level = (unsigned)result;
    return 0;
}

int parseSeenBy2D(char * token, hs_addr ** addr, unsigned int * count)
{
    const char * next;
    unsigned int maxcount = *count;

    if(token == NULL)
    {
        prErr("An address after %s is missing!", actualKeyword);
        return 1;
    }

    while(*token)
    {
        while(!isdigit(*token))
        {
            token++;
        }

        if(*count >= maxcount) /* realloc once per 10 iteration */
        {
            maxcount = *count + 10;
            (*addr)  = srealloc(*addr, sizeof(hs_addr) * maxcount);
            memset(*addr + *count, 0, sizeof(hs_addr) * 10);
        }

        if(parseFtnAddrZ(token, &(*addr)[*count], FTNADDR_2D, &next) & FTNADDR_ERROR)
        {
            return 1;
        }

        token = (char *)next;
        (*count)++;

        if(*token == ')')
        {
            break;
        }
    }

    if(maxcount > *count)
    {
        (*addr) = srealloc(*addr, sizeof(hs_addr) * (*count));
    }

    return 0;
} /* parseSeenBy2D */

int getLinkRescanAccess(s_area * area, s_link * link)
{
    int rescan = 0;

    if(area->msgbType != MSGTYPE_PASSTHROUGH)
    {
        if(link->numRescanGrp > 0)
        {
            rescan = link->denyRescan;

            if(grpInArray(area->group, link->RescanGrp, link->numRescanGrp))
            {
                rescan = (!link->denyRescan);
            }
        }
        else
        {
            rescan = (!link->denyRescan);
        }
    }

    return rescan;
}

void setLinkAccess(s_fidoconfig * config, s_area * area, s_arealink * arealink)
{
    s_link * link = arealink->link;

    if(link->numOptGrp > 0)
    {
        /*  default set export on, import on, mandatory off, manual off */
        arealink->aexport   = 1;
        arealink->import    = 1;
        arealink->mandatory = 0;
        arealink->manual    = 0;

        if(grpInArray(area->group, link->optGrp, link->numOptGrp))
        {
            arealink->aexport   = link->aexport;
            arealink->import    = link->import;
            arealink->mandatory = link->mandatory;
            arealink->manual    = link->manual;
        }
    }
    else
    {
        arealink->aexport   = link->aexport;
        arealink->import    = link->import;
        arealink->mandatory = link->mandatory;
        arealink->manual    = link->manual;
    }

    arealink->rescan = getLinkRescanAccess(area, link);

    if(area->mandatory)
    {
        arealink->mandatory = 1;
    }

    if(area->manual)
    {
        arealink->manual = 1;
    }

    if((area->levelread > link->level) || ((link->Pause & area->areaType) && (!area->noPause)))
    {
        arealink->aexport = 0;
    }

    if(area->levelwrite > link->level)
    {
        arealink->import = 0;
    }

    if(area->group)
    {
        if(link->numAccessGrp)
        {
            if(config->numPublicGroup)
            {
                if(!grpInArray(area->group, link->AccessGrp,
                               link->numAccessGrp) &&
                   !grpInArray(area->group, config->PublicGroup, config->numPublicGroup))
                {
                    arealink->aexport = 0;
                    arealink->import  = 0;
                }
            }
            else if(!grpInArray(area->group, link->AccessGrp, link->numAccessGrp))
            {
                arealink->aexport = 0;
                arealink->import  = 0;
            }
        }
        else if(config->numPublicGroup)
        {
            if(!grpInArray(area->group, config->PublicGroup, config->numPublicGroup))
            {
                arealink->aexport = 0;
                arealink->import  = 0;
            }
        }
    }
} /* setLinkAccess */

int parseAreaOption(s_fidoconfig * config, char * option, s_area * area)
{
    char * error;
    char * token;
    char * iOption;
    char * iToken;
    size_t i;
    long il;

    if(option == NULL)
    {
        prErr("There are parameters missing after %s!", actualKeyword);
        return 1;
    }

    iOption = strLower(sstrdup(option));

    if(strcmp(iOption, "b") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            token = strtok(NULL, " \t");

            if(token == NULL)
            {
                prErr("An msgbase type is missing after -b in areaOptions!");
                nfree(iOption);
                return 1;
            }

            if(area->msgbType == MSGTYPE_PASSTHROUGH)
            {
                /*  MsgBase type is already defined */
                nfree(iOption);
                return 0;
            }

            iToken = strLower(sstrdup(token));

            if(strcasecmp(iToken, "squish") == 0)
            {
                area->msgbType = MSGTYPE_SQUISH;
            }
            else if(strcasecmp(iToken, "jam") == 0)
            {
                area->msgbType = MSGTYPE_JAM;
            }
            else if(strcasecmp(iToken, "msg") == 0)
            {
                area->msgbType = MSGTYPE_SDM;
            }
            else
            {
                prErr("MsgBase type %s not valid after -b in areaOptions!", token);
                nfree(iOption);
                nfree(iToken);
                return 1;
            }

            nfree(iToken);
        }
        else
        {
            prErr("Option '-b' is allowed for echoareas and localareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcasecmp(iOption, "pass") == 0)
    {
        area->msgbType = MSGTYPE_PASSTHROUGH;
    }
    else if(strcmp(iOption, "p") == 0)
    {
        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("A number after -p in areaOptions is missing!");
            nfree(iOption);
            return 1;
        }

        area->nopack = 0;
        il           = strtol(token, &error, 0);

        if((error != NULL) && (*error != '\0'))
        {
            prErr("The number after -p in areaOptions is wrong!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }

        if(area->areaType == ECHOAREA)
        {
            area->purge = il < 0 ? config->EchoAreaDefault.purge : (unsigned)il;
        }
        else if(area->areaType == FILEAREA)
        {
            area->purge = il < 0 ? config->FileAreaDefault.purge : (unsigned)il;
        }
    }
    else if(strcmp(iOption, "$m") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            token = strtok(NULL, " \t");

            if(token == NULL)
            {
                prErr("A number after -$m in areaOptions is missing!");
                nfree(iOption);
                return 1;
            }

            area->nopack = 0;
            il           = strtol(token, &error, 0);

            if((error != NULL) && (*error != '\0'))
            {
                prErr("The number after -$m in areaOptions is wrong!");
                nfree(iOption);
                return 1;     /*  error */
            }

            area->max = il < 0 ? config->EchoAreaDefault.max : (unsigned)il;
        }
        else
        {
            prErr("Option '-$m' is allowed for echoareas and localareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "a") == 0)
    {
        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("Address after -a in areaOptions is missing!");
            nfree(iOption);
            return 1;
        }

        area->useAka = getAddr(config, token);

        if(area->useAka == NULL)
        {
            prErr("%s not found as address.", token);
            nfree(iOption);
            return 1;
        }
    }
    else if(strcmp(iOption, "lr") == 0)
    {
        size_t tokenLen;

        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("A number after -lr in areaOptions is missing!");
            nfree(iOption);
            return 1;
        }

        tokenLen = strlen(token);
        for(i = 0; i < tokenLen; i++)
        {
            if(isdigit(token[i]) == 0)
            {
                break;
            }
        }

        if(i != tokenLen)
        {
            prErr("The number after -lr in areaOptions is wrong!");
            nfree(iOption);
            return 1;
        }

        il = strtol(token, &error, 0);

        if((error != NULL) && (*error != '\0'))
        {
            prErr("The number after -lr in areaOptions is wrong!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }

        if(il < 0)
        {
            prErr("The number after -lr in areaOptions is wrong (negative values not alloved)!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }

        area->levelread = (unsigned)il;

        /* if link was added before -lr setting it must be updated */
        for(i = 0; i < area->downlinkCount; ++i)
        {
            setLinkAccess(config, area, area->downlinks[i]);
        }
    }
    else if(strcmp(iOption, "lw") == 0)
    {
        size_t tokenLen;

        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("A number after -lw in areaOptions is missing!");
            nfree(iOption);
            return 1;
        }

        tokenLen = strlen(token);
        for(i = 0; i < tokenLen; i++)
        {
            if(isdigit(token[i]) == 0)
            {
                break;
            }
        }

        if(i != tokenLen)
        {
            prErr("The number after -lw in areaOptions is wrong!");
            nfree(iOption);
            return 1;
        }

        il = strtol(token, &error, 0);

        if((error != NULL) && (*error != '\0'))
        {
            prErr("The number after -lw in areaOptions is wrong!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }

        if(il < 0)
        {
            prErr("The number after -lw in areaOptions is wrong (negative values not alloved)!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }

        area->levelwrite = (unsigned)il;

        /* if link was added before -lw setting it must be updated */
        for(i = 0; i < area->downlinkCount; ++i)
        {
            setLinkAccess(config, area, area->downlinks[i]);
        }
    }
    else if(strcmp(iOption, "tooold") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            size_t tokenLen;

            token = strtok(NULL, " \t");

            if(token == NULL)
            {
                prErr("A number after %s in areaOptions is missing!", iOption);
                nfree(iOption);
                return 1;
            }

            tokenLen = strlen(token);
            for(i = 0; i < tokenLen; i++)
            {
                if(isdigit(token[i]) == 0)
                {
                    break;
                }
            }

            if(i != tokenLen)
            {
                prErr("The number after %s in areaOptions is wrong!", iOption);
                nfree(iOption);
                return 1;
            }

            il = strtol(token, &error, 0);

            if((error != NULL) && (*error != '\0'))
            {
                prErr("The number after %s in areaOptions is wrong!", iOption);
                nfree(iOption);
                return 1;     /*  error occured; */
            }

            if(il < 0)
            {
                prErr("The number after %s in areaOptions is wrong (negative values not allowed)!",
                      iOption);
                nfree(iOption);
                return 1;     /*  error occured; */
            }

            area->tooOld = (unsigned)il;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "toonew") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            size_t tokenLen;

            token = strtok(NULL, " \t");

            if(token == NULL)
            {
                prErr("A number after %s in areaOptions is missing!", iOption);
                nfree(iOption);
                return 1;
            }

            tokenLen = strlen(token);
            for(i = 0; i < tokenLen; i++)
            {
                if(isdigit(token[i]) == 0)
                {
                    break;
                }
            }

            if(i != tokenLen)
            {
                prErr("The number after %s in areaOptions is wrong!", iOption);
                nfree(iOption);
                return 1;
            }

            il = strtol(token, &error, 0);

            if((error != NULL) && (*error != '\0'))
            {
                prErr("The number after %s in areaOptions is wrong!", iOption);
                nfree(iOption);
                return 1;     /*  error occured; */
            }

            if(il < 0)
            {
                prErr("The number after %s in areaOptions is wrong (negative values not allowed)!",
                      iOption);
                nfree(iOption);
                return 1;     /*  error occured; */
            }

            area->tooNew = (unsigned)il;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "tinysb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->tinySB = 1;
        }
        else
        {
            prErr("Option '-tinysb' is allowed for echoareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "notinysb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->tinySB = 0;
        }
        else
        {
            prErr("Option '-notinysb' is allowed for echoareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "killsb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->killSB = 1;
        }
        else
        {
            prErr("Option '-killsb' is allowed for echoareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nokillsb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->killSB = 0;
        }
        else
        {
            prErr("Option '-nokillsb' is allowed for echoareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "keepunread") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->keepUnread = 1;
        }
        else
        {
            prErr("Option '-keepunread' is allowed for echoareas and localareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nokeepunread") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->keepUnread = 0;
        }
        else
        {
            prErr("Option '-nokeepunread' is allowed for echoareas and localareas only!");
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "killread") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->killRead = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nokillread") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->killRead = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "h") == 0)
    {
        area->hide = 1;
    }
    else if(strcmp(iOption, "hide") == 0)
    {
        area->hide = 1;
    }
    else if(strcmp(iOption, "nohide") == 0)
    {
        area->hide = 0;
    }
    else if(strcmp(iOption, "k") == 0)
    {
        area->killMsgBase = 1;
    }
    else if(strcmp(iOption, "kill") == 0)
    {
        area->killMsgBase = 1;
    }
    else if(strcmp(iOption, "nokill") == 0)
    {
        area->killMsgBase = 0;
    }
    else if(strcmp(iOption, "manual") == 0)
    {
        area->manual = 1;
    }
    else if(strcmp(iOption, "nomanual") == 0)
    {
        area->manual = 0;
    }
    else if(strcmp(iOption, "nopause") == 0)
    {
        area->noPause = 1;
    }
    else if(strcmp(iOption, "pause") == 0)
    {
        area->noPause = 0;
    }
    else if(strcmp(iOption, "nolink") == 0)
    {
        area->nolink = 1;
    }
    else if(strcmp(iOption, "link") == 0)
    {
        area->nolink = 0;
    }
    else if(strcmp(iOption, "mandatory") == 0)
    {
        area->mandatory = 1;
    }
    else if(strcmp(iOption, "nomandatory") == 0)
    {
        area->mandatory = 0;
    }
    else if(strcmp(iOption, "debug") == 0)
    {
        area->debug = 1;
    }
    else if(strcmp(iOption, "nodebug") == 0)
    {
        area->debug = 0;
    }
    else if(strcmp(iOption, "dosfile") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->DOSFile = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nodosfile") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->DOSFile = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "paused") == 0)
    {
        area->paused = 1;
    }
    else if(strcmp(iOption, "noautoareapause") == 0)
    {
        area->noautoareapause = 1;
    }
    else if(strcmp(iOption, "autoareapause") == 0)
    {
        area->noautoareapause = 0;
    }
    else if(strcmp(iOption, "nopack") == 0)
    {
        area->nopack = 1;
    }
    else if(strcmp(iOption, "pack") == 0)
    {
        area->nopack = 0;
    }
    else if(strcmp(iOption, "ccoff") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->ccoff = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "noccoff") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->ccoff = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "ccon") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->ccoff = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "keepsb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->keepsb = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nokeepsb") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->keepsb = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "sendorig") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->sendorig = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;                 /*  error */
        }
    }
    else if(strcmp(iOption, "nosendorig") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->sendorig = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;                     /*  error */
        }
    }
    else if(strcmp(iOption, "crc") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->noCRC = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;                         /*  error */
        }
    }
    else if(strcmp(iOption, "nocrc") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->noCRC = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "replace") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->noreplace = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "noreplace") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->noreplace = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "rename") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->rename = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "norename") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->rename = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "sbkeepall") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->sbkeep_all = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nosbkeepall") == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            area->sbkeep_all = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "diz") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->nodiz = 0;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "nodiz") == 0)
    {
        if(area->areaType == FILEAREA)
        {
            area->nodiz = 1;
        }
        else
        {
            prErr("Option '%s' is allowed for fileareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "dupecheck") == 0)
    {
        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("Missing dupeCheck parameter!");
            nfree(iOption);
            return 1;
        }

        if(stricmp(token, "off") == 0)
        {
            area->dupeCheck = dcOff;
        }
        else if(stricmp(token, "move") == 0)
        {
            area->dupeCheck = dcMove;
        }
        else if(stricmp(token, "del") == 0)
        {
            area->dupeCheck = dcDel;
        }
        else
        {
            prErr("Wrong dupeCheck parameter!");
            nfree(iOption);
            return 1; /*  error */
        }
    }
    else if(strcmp(iOption, "dupehistory") == 0)
    {
        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("Number is missing after -dupehistory in areaOptions!");
            nfree(iOption);
            return 1;
        }

        area->dupeHistory = (unsigned)strtol(token, &error, 0);

        if((error != NULL) && (*error != '\0'))
        {
            prErr("Number is wrong after -dupeHistory in areaOptions!");
            nfree(iOption);
            return 1;     /*  error occured; */
        }
    }
    /* val: -scan listed|manual|never */
    else if(strcmp(iOption, "scan") == 0)
    {
        if(area->areaType != ECHOAREA)
        {
            prErr("Option '%s' is allowed for echoareas and localareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }

        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            prErr("Scan parameter is missing!");
            nfree(iOption);
            return 1;
        }

        if(stricmp(token, "never") == 0)
        {
            area->scanMode = smNever;
        }
        else if(stricmp(token, "manual") == 0)
        {
            area->scanMode = smManual;
        }
        else if(stricmp(token, "listed") == 0)
        {
            area->scanMode = smListed;
        }
        else
        {
            prErr("Wrong scan parameter!");
            nfree(iOption);
            return 1; /* error */
        }
    }
    /* /val */
    else if(strcmp(iOption, "g") == 0)
    {
        token = strtok(NULL, " \t");

        if(token == NULL)
        {
            nfree(iOption);
            return 1;
        }

        /* dmitry: this overrides group in EchoAreaDefaults */
        nfree(area->group);
        area->group = sstrdup(token);
    }
    else if(strcmp(iOption, "$") == 0)
    {}
    else if(strcmp(iOption, "0") == 0)
    {}
    else if(strcmp(iOption, "d") == 0)
    {
        if((area->description = getDescription()) == NULL)
        {
            nfree(iOption);
            return 1;
        }
    }
    else if(strncmp(iOption, "sbadd(", 6) == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            parseSeenBy2D(iOption, &(area->sbadd), &(area->sbaddCount));
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strncmp(iOption, "sbign(", 6) == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            parseSeenBy2D(iOption, &(area->sbign), &(area->sbignCount));
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strncmp(iOption, "sbstrip(", 8) == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            parseSeenBy2D(iOption, &(area->sbstrip), &(area->sbstripCount));
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strncmp(iOption, "sbkeep(", 7) == 0)
    {
        if(area->areaType == ECHOAREA)
        {
            parseSeenBy2D(iOption, &(area->sbkeep), &(area->sbkeepCount));
        }
        else
        {
            prErr("Option '%s' is allowed for echoareas only!", iOption);
            nfree(iOption);
            return 1;     /*  error */
        }
    }
    else if(strcmp(iOption, "r") == 0)
    {
        if(area->def_subscribing != RW)
        {
            prErr("Can't mix area options \"-r\" and \"-w\"!");
            nfree(iOption);
            return 1;
        }

        area->def_subscribing = RO;
    }
    else if(strcmp(iOption, "w") == 0)
    {
        if(area->def_subscribing != RW)
        {
            prErr("Can't mix area options \"-r\" and \"-w\"!");
            nfree(iOption);
            return 1;
        }

        area->def_subscribing = WO;
    }
    else
    {
        prErr("unknown area option \"-%s\"!", option);
        nfree(iOption);
        return 1;
    }

    nfree(iOption);
    return 0;
} /* parseAreaOption */

int parseLinkOption(s_arealink * alink, char * token)
{
    char * iToken;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    iToken = strLower(sstrdup(token));

    if(strcmp(iToken, "r") == 0)
    {
        alink->import = 0;
    }
    else if(strcmp(iToken, "w") == 0)
    {
        alink->aexport = 0;
    }
    else if(strcmp(iToken, "mn") == 0)
    {
        alink->mandatory = 1;
    }
    else if(strcmp(iToken, "def") == 0)
    {
        alink->defLink = 1;
    }
    else
    {
        nfree(iToken);
        return 1;
    }

    nfree(iToken);
    return 0;
} /* parseLinkOption */

int parseAreaLink(s_fidoconfig * config, s_area * area, char * tok)
{
    s_arealink * arealink;
    s_link * link;

    if(tok == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    if((link = getLinkForArea(config, tok, area)) == NULL)
    {
        prErr("no links like \"%s\" in config!", tok);
        return 1;
    }

    if(isLinkOfArea(link, area))
    {
        prErr("link %s is subscribed twice!", tok);
        return 0;
    }

    area->downlinks = srealloc(area->downlinks, sizeof(s_arealink *) * (area->downlinkCount + 1));
    area->downlinks[area->downlinkCount]       = (s_arealink *)scalloc(1, sizeof(s_arealink));
    area->downlinks[area->downlinkCount]->link = link;
    arealink = area->downlinks[area->downlinkCount];
    area->downlinkCount++;
    setLinkAccess(config, area, arealink);
    return 0;
} /* parseAreaLink */

/*
   useDefs
   uses enum pauses { NOPAUSE, ECHOAREA, FILEAREA } for choosing  defaults
 */
int parseArea(s_fidoconfig * config, char * token, s_area * area, int useDefs)
{
    char * tok, addr[24], * ptr;
    unsigned int rc = 0, i, j;
    size_t toklen;
    grp_t * group;
    e_pauses aType = area->areaType;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    /*   memset(area, '\0', sizeof(s_area)); */
    area->scanMode = smNone;    /* val: just to be sure it's set */

    /* need add checking for area->areaType == 0 aka NOPAUSE ) */
    if(useDefs)   /* copy defaults */
    /* echo|file diffs */
    {
        if(area->areaType == ECHOAREA)
        {
            memcpy(area, &(config->EchoAreaDefault), sizeof(s_area));
        }
        else if(area->areaType == FILEAREA)
        {
            memcpy(area, &(config->FileAreaDefault), sizeof(s_area));
        }

        /* default has perhaps groups        */
        /*             perhaps a description */
        /*             perhaps downlinks     */
        /*             areaName==NULL        */
        /*             fileName==NULL        */
        /*             perhaps other settings*/
        /*             allways an useAka     */
    }
    else    /* netmail - don't copy defaults */
    {
        memset(area, 0, sizeof(s_area));
    }

    area->areaType = aType;
    tok            = strtok(token, " \t");

    if(tok == NULL)
    {
        prErr("An areaname after %s is missing!", actualKeyword);
        return 1;       /*  if there is no areaname */
    }

    if(useDefs && (group = findGroupForArea(tok)) != NULL)
    {
        memcpy(area, group->area, sizeof(s_area));
    }

    area->areaName = (char *)smalloc(strlen(tok) + 1);

    if(*tok == '\"' && tok[strlen(tok) - 1] == '\"' && tok[1])
    {
        strcpy(area->areaName, tok + 1);
        area->areaName[strlen(area->areaName) - 1] = '\0';
    }
    else
    {
        strcpy(area->areaName, tok);
    }

    if(token[strlen(tok)] == '\0')
    {
        token[strlen(tok)] = ' ';
    }

    /* copy default group
     * fc_freeEchoArea() and parseAreaOption() will free this memory */
    if(area->group != NULL)
    {
        area->group = sstrdup(area->group);
    }

    area->description = NULL;

    /* not poiting to the links of the default --> .. */
    if(area->downlinkCount)
    {
        j = area->downlinkCount;
        area->downlinkCount = 0; /* was copied from default but there were no downlinks added
                                    really */
        area->downlinks     = NULL;

        /* so now add default downlinks */
        /* echo|file diffs */
        if(area->areaType == ECHOAREA)
        {
            for(i = 0; i < j; ++i)
            {
                rc +=
                    parseAreaLink(config, area,
                                  aka2str(&(config->EchoAreaDefault).downlinks[i]->link->hisAka));
            }
        }
        else if(area->areaType == FILEAREA)
        {
            for(i = 0; i < j; ++i)
            {
                rc +=
                    parseAreaLink(config, area,
                                  aka2str(&(config->FileAreaDefault).downlinks[i]->link->hisAka));
            }
        }
    }

    /* copy SEEN-BY arrays from default */
    if(area->areaType == ECHOAREA)      /* just for echoareas */
    {
        rc +=
            fc_copyAddressArray(area->sbaddCount, (config->EchoAreaDefault).sbadd, &(area->sbadd));
        rc +=
            fc_copyAddressArray(area->sbignCount, (config->EchoAreaDefault).sbign, &(area->sbign));
        rc +=
            fc_copyAddressArray(area->sbkeepCount,
                                (config->EchoAreaDefault).sbkeep,
                                &(area->sbkeep));
        rc +=
            fc_copyAddressArray(area->sbstripCount, (config->EchoAreaDefault).sbstrip,
                                &(area->sbstrip));
    }

    /*   area->fperm = area->uid = area->gid = -1;*/
    if(!area->fperm && !area->uid && !area->gid)
    {
        area->fperm = area->uid = area->gid = (UINT)-1;
    }

    /*   area->msgbType = MSGTYPE_SDM;*/
    if(!area->msgbType)
    {
        area->msgbType = MSGTYPE_SQUISH;
    }

    /*   area->useAka = config->addr;*/
    if(area->useAka == NULL)
    {
        area->useAka = config->addr;
    }

    /*  set default parameters of dupebase */
    if(!area->dupeHistory)
    {
        area->dupeHistory = 7;
    }

    /*  set defaults for MS-DOS */
#ifdef __DOS__
    area->DOSFile = 1;
#endif

    tok = strtok(token, " \t");
    tok = strtok(NULL, " \t");

    if(tok == NULL)
    {
        /*  was default settings.. */
        if(area->msgbType == MSGTYPE_PASSTHROUGH)
        {
            return 0;
        }
        else
        {
            prErr("A pathname is missing %s!", actualLine);
            return 2;    /*  if there is no filename */
        }
    }

    toklen = strlen(tok); /* points to '\0' */

    if(stricmp(tok, "passthrough") != 0)
    {
        /* perhaps passthrough in default, so this does not have to be */
        /* a filename */
        /* is it a filename? */
        ptr = tok;

        while(*ptr && *ptr != PATH_DELIM && !isspace(*ptr))
        {
            ++ptr;
        }

        if(*ptr == PATH_DELIM)
        {
            /* yes it is a filename :=) */
            /*  msgbase on disk */
            area->fileName = (char *)scalloc(1, toklen + 2);
            strcpy(area->fileName, tok);

            if((area->areaType == FILEAREA) && (tok[toklen - 1] != PATH_DELIM))
            {
                area->fileName[toklen] = PATH_DELIM;
            }

            tok = strtok(NULL, " \t");
        }
        else if(area->msgbType != MSGTYPE_PASSTHROUGH)
        {
            /* was not a filename, and default not passthrough */
            prErr("A pathname is missing %s!", actualLine);
            return 2;        /*  if there is no filename */
        }
    }
    else
    {
        /*  passthrough area */
        /*   area->fileName = NULL;  was copied from default */
        area->msgbType = MSGTYPE_PASSTHROUGH;
        tok            = strtok(NULL, " \t");
    }

    while(tok != NULL)
    {
        if(tok[0] == '-')
        {
            rc += parseAreaOption(config, tok + 1, area);

            if(rc)
            {
                return rc;
            }
        }
        else if((isdigit(*tok) || (*tok == '*')) &&
                (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*")))
        {
            if(strchr(tok, '*'))
            {
                /* link mask present: set mandatory for all links matched. */
                j = area->downlinkCount;

                for(i = 0; i < config->linkCount; i++)
                {
                    strcpy(addr, aka2str(&config->links[i]->hisAka));

                    if(patmat(addr, tok))
                    {
                        parseAreaLink(config, area, addr);
                        area->downlinks[area->downlinkCount - 1]->mandatory = 1;
                    }
                    else if(config->links[i]->hisAka.point == 0)
                    {
                        strcat(addr, ".0");

                        if(patmat(addr, tok))
                        {
                            parseAreaLink(config, area, addr);
                            area->downlinks[area->downlinkCount - 1]->mandatory = 1;
                        }
                    }
                }
                tok = strtok(NULL, " \t");

                while(tok)
                {
                    if(tok[0] != '-')
                    {
                        break;
                    }

                    for(i = j; i < area->downlinkCount; i++)
                    {
                        if(parseLinkOption(area->downlinks[i], tok + 1))
                        {
                            break;
                        }
                    }

                    if(i < area->downlinkCount)
                    {
                        break;
                    }

                    tok = strtok(NULL, " \t");
                }
                continue;
            }

            rc += parseAreaLink(config, area, tok);

            if(rc)
            {
                return rc;
            }

            tok = strtok(NULL, " \t");

            while(tok)
            {
                if(tok[0] == '-')
                {
                    if(parseLinkOption(area->downlinks[area->downlinkCount - 1], tok + 1))
                    {
                        break;
                    }

                    tok = strtok(NULL, " \t");
                }
                else
                {
                    break;
                }
            }
            continue;
        }
        else
        {
            prErr("Error in areaOptions token=%s!", tok);
            rc += 1;
        }

        tok = strtok(NULL, " \t");
    }

    if(area->description == NULL && config->EchoAreaDefault.description != NULL)
    {
        area->description = sstrdup(config->EchoAreaDefault.description);
    }

    return rc;
} /* parseArea */

int parseAreaDefault(s_fidoconfig * config, char * token, s_area * adef, int cleanup)
{
    char * tok, addr[24];
    unsigned int rc = 0, i;

    /* default has perhaps groups        */
    /*             perhaps a description */
    /*             perhaps downlinks     */
    /*             areaName==NULL        */
    /*             fileName==NULL        */
    /*             perhaps other settings*/
    /*             allways an useAka     */
    /* cleanup */
    if(cleanup)
    {
        e_pauses aType = adef->areaType;
        fc_freeEchoArea(adef);
        memset(adef, '\0', sizeof(s_area));
        adef->useAka   = config->addr;
        adef->areaType = aType;
        adef->fperm    = adef->uid = adef->gid = (UINT)-1;
        adef->msgbType = MSGTYPE_SDM;
        /*  set default parameters of dupebase */
        adef->dupeHistory = 7; /* 7 days */
        /* val: set defaul scan mode */
        adef->scanMode = smNone;
        /*  set defaults for MS-DOS */
#ifdef __DOS__
        adef->DOSFile = 1;
#endif
    }

    if(token == NULL) /* all defaults off */
    {
        return 0;
    }

    if(!strncasecmp(token, "off", 3))
    {
        return 0; /* default off */
    }

    tok = strtok(token, " \t");

    if(tok == NULL)   /* does this ever happen?? */
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 2;
    }

    while(tok != NULL)
    {
        if(stricmp(tok, "passthrough") == 0)
        {
            /*  passthrough area */
/*           adef->fileName = NULL;*/
            adef->msgbType = MSGTYPE_PASSTHROUGH;
        }
        else if(tok[0] == '-')
        {
            rc += parseAreaOption(config, tok + 1, adef);

            if(rc)
            {
                return rc;
            }
        }
        else if((isdigit(*tok) || (*tok == '*')) &&
                (patmat(tok, "*:*/*") || patmat(tok, "*:*/*.*")))
        {
            if(strchr(tok, '*'))
            {
                for(i = 0; i < config->linkCount; i++)
                {
                    sprintf(addr, "%s", aka2str(&config->links[i]->hisAka));

                    if(patmat(addr, tok))
                    {
                        parseAreaLink(config, adef, addr);
                        adef->downlinks[adef->downlinkCount - 1]->mandatory = 1;
                    }
                }
                tok = strtok(NULL, " \t");
                continue;
            }

            rc += parseAreaLink(config, adef, tok);

            if(rc)
            {
                return rc;
            }

            tok = strtok(NULL, " \t");

            while(tok)
            {
                if(tok[0] == '-')
                {
                    if(parseLinkOption(adef->downlinks[adef->downlinkCount - 1], tok + 1))
                    {
                        break;
                    }

                    tok = strtok(NULL, " \t");
                }
                else
                {
                    break;
                }
            }
            continue;
        }
        else
        {
            prErr("Error in areaOptions token=%s!", tok);
            rc += 1;
        }

        tok = strtok(NULL, " \t");
    }
    return rc;
} /* parseAreaDefault */

int parseAreaGroup(char * values)
{
    char * ptr = NULL;
    char * name;
    char * patternList;

    if(!values || !strlen(values))
    {
        prErr("Error in areaGroup definition - no name specified!");
        return 1;
    }

    ptr = strchr(values, ' ');

    if(!ptr)
    {
        ptr = strchr(values, '\t');
    }

    if(!ptr)
    {
        prErr("Error in areaGroup definition - no patterns specified!");
        return 2;
    }

    name = scalloc(ptr - values + 1, 1);
    sstrncpy(name, values, ptr - values);

    while((*ptr == ' ') || (*ptr == '\t'))
    {
        ptr++;
    }

    if(!strlen(ptr))
    {
        prErr("Error in areaGroup definition - no patterns specified!");
        return 2;
    }

    patternList = sstrdup(ptr);
    addPatternToGrpTree(name, patternList);
    return 0;
} /* parseAreaGroup */

int parseAreaGroupDefaults(s_fidoconfig * config, char * values)
{
    char * ptr = NULL;
    char * name;
    char * params;
    grp_t * g;

    if(!values || !strlen(values))
    {
        prErr("Error in areaGroup definition - no name specified!");
        return 1;
    }

    ptr = strchr(values, ' ');

    if(!ptr)
    {
        ptr = strchr(values, '\t');
    }

    if(!ptr)
    {
        prErr("Error in areaGroupDefaults definition - no patterns specified!");
        return 2;
    }

    name = scalloc(ptr - values + 1, 1);
    sstrncpy(name, values, ptr - values);

    while((*ptr == ' ') || (*ptr == '\t'))
    {
        ptr++;
    }

    if(!strlen(ptr))
    {
        prErr("Error in areaGroupDefaults definition - no parameters specified!");
        return 2;
    }

    g = findGroupByName(name);

    if(!g)
    {
        prErr("Group %s is undefined, please define it first using 'areaGroup' token!");
        return 3;
    }

    params = sstrdup(ptr);
    memcpy(g->area, &(config->EchoAreaDefault), sizeof(s_area));
    parseAreaDefault(config, params, g->area, 0);
    nfree(params);
    nfree(name);
    return 0;
} /* parseAreaGroupDefaults */

int parseEchoArea(char * token, s_fidoconfig * config)
{
    int rc;
    s_area * area;

    if(config->addr == NULL)
    {
        printAddrError();
    }

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->echoAreas = srealloc(config->echoAreas, sizeof(s_area) * (config->echoAreaCount + 1));
    area           = &(config->echoAreas[config->echoAreaCount]);
    area->areaType = ECHOAREA;
    rc             = parseArea(config, token, area, 1);
    config->echoAreaCount++;
    return rc;
}

int parseNetMailArea(char * token, s_fidoconfig * config)
{
    int rc;
    s_area * area;

    if(config->addr == NULL)
    {
        printAddrError();
    }

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->netMailAreas =
        srealloc(config->netMailAreas, sizeof(s_area) * (config->netMailAreaCount + 1));
    area           = &(config->netMailAreas[config->netMailAreaCount]);
    area->areaType = ECHOAREA;
    rc             = parseArea(config, token, area, 0);
    config->netMailAreaCount++;
    return rc;
}

int parseFileArea(char * token, s_fidoconfig * config)
{
    int rc;
    s_area * area;

    if(config->addr == NULL)
    {
        printAddrError();
    }

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->fileAreas = srealloc(config->fileAreas, sizeof(s_area) * (config->fileAreaCount + 1));
    area           = &(config->fileAreas[config->fileAreaCount]);
    area->areaType = FILEAREA;
    rc             = parseArea(config, token, area, 1);
    config->fileAreaCount++;
    return rc;
}

int parseBbsArea(const s_fidoconfig * config, char * token, s_bbsarea * area)
{
    char * tok;
    int rc = 0;

    unused(config);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    memset(area, 0, sizeof(s_bbsarea));
    tok = strtok(token, " \t");

    if(tok == NULL)
    {
        prErr("An areaname after %s is missing!", actualKeyword);
        return 1;       /*  if there is no areaname */
    }

    area->areaName = (char *)smalloc(strlen(tok) + 1);
    strcpy(area->areaName, tok);
    tok = strtok(NULL, " \t");

    if(tok == NULL)
    {
        prErr("A pathname is missing %s!", actualLine);
        return 2;       /*  if there is no filename */
    }

    if(tok[strlen(tok) - 1] == PATH_DELIM)
    {
        area->pathName = (char *)smalloc(strlen(tok) + 1);
        strcpy(area->pathName, tok);
    }
    else
    {
        area->pathName = (char *)smalloc(strlen(tok) + 2);
        strcpy(area->pathName, tok);
        area->pathName[strlen(tok)]     = PATH_DELIM;
        area->pathName[strlen(tok) + 1] = '\0';
    }

    tok = strtok(NULL, " \t");

    while(tok != NULL)
    {
        if(stricmp(tok, "-d") == 0)
        {
            if((area->description = getDescription()) == NULL)
            {
                rc += 1;
            }
        }
        else
        {
            prErr("Error in areaOptions token=%s!", tok);
            rc += 1;
            return rc;
        }

        tok = strtok(NULL, " \t");
    }
    return rc;
} /* parseBbsArea */

int parseBbsAreaStatement(char * token, s_fidoconfig * config)
{
    int rc;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->bbsAreas = srealloc(config->bbsAreas, sizeof(s_bbsarea) * (config->bbsAreaCount + 1));
    rc = parseBbsArea(config, token, &(config->bbsAreas[config->bbsAreaCount]));
    config->bbsAreaCount++;
    return rc;
}

int parseLink(char * token, s_fidoconfig * config)
{
    s_link * clink;
    s_link * deflink;

    if(token == NULL)
    {
        prErr("A name after %s is missing!", actualKeyword);
        return 1;
    }

    config->describeLinkDefaults = 0; /*  Stop describing of link defaults if it was */

    config->links = srealloc(config->links, sizeof(ps_link) * (config->linkCount + 1));
    config->links[config->linkCount] = scalloc(1, sizeof(s_link));
    clink = config->links[config->linkCount];

    if(config->linkDefaults)
    {
        memcpy(clink, deflink = config->linkDefaults, sizeof(s_link));
        clink->name           = sstrdup(deflink->name);
        clink->defaultPwd     = sstrdup(deflink->defaultPwd);

        if(deflink->pktPwd != deflink->defaultPwd)
        {
            clink->pktPwd = sstrdup(deflink->pktPwd);
        }
        else
        {
            clink->pktPwd = clink->defaultPwd;
        }

        if(deflink->ticPwd != deflink->defaultPwd)
        {
            clink->ticPwd = sstrdup(deflink->ticPwd);
        }
        else
        {
            clink->ticPwd = clink->defaultPwd;
        }

        if(deflink->areafix.pwd != deflink->defaultPwd)
        {
            clink->areafix.pwd = sstrdup(deflink->areafix.pwd);
        }
        else
        {
            clink->areafix.pwd = clink->defaultPwd;
        }

        if(deflink->filefix.pwd != deflink->defaultPwd)
        {
            clink->filefix.pwd = sstrdup(deflink->filefix.pwd);
        }
        else
        {
            clink->filefix.pwd = clink->defaultPwd;
        }

        if(deflink->bbsPwd != deflink->defaultPwd)
        {
            clink->bbsPwd = sstrdup(deflink->bbsPwd);
        }
        else
        {
            clink->bbsPwd = clink->defaultPwd;
        }

        if(deflink->sessionPwd != deflink->defaultPwd)
        {
            clink->sessionPwd = sstrdup(deflink->sessionPwd);
        }
        else
        {
            clink->sessionPwd = clink->defaultPwd;
        }

        clink->handle    = sstrdup(deflink->handle);
        clink->email     = sstrdup(deflink->email);
        clink->emailFrom = sstrdup(deflink->emailFrom);
        clink->emailSubj = sstrdup(deflink->emailSubj);
        clink->LinkGrp   = sstrdup(deflink->LinkGrp);
        clink->AccessGrp = copyGroups(deflink->AccessGrp, deflink->numAccessGrp);
        clink->areafix.autoCreateFile     = sstrdup(deflink->areafix.autoCreateFile);
        clink->filefix.autoCreateFile     = sstrdup(deflink->filefix.autoCreateFile);
        clink->areafix.autoCreateDefaults = sstrdup(deflink->areafix.autoCreateDefaults);
        clink->filefix.autoCreateDefaults = sstrdup(deflink->filefix.autoCreateDefaults);
        clink->areafix.fwdFile            = sstrdup(deflink->areafix.fwdFile);
        clink->filefix.fwdFile            = sstrdup(deflink->filefix.fwdFile);
        clink->areafix.denyFwdFile        = sstrdup(deflink->areafix.denyFwdFile);
        clink->filefix.denyFwdFile        = sstrdup(deflink->filefix.denyFwdFile);
        clink->areafix.name         = sstrdup(deflink->areafix.name);
        clink->filefix.name         = sstrdup(deflink->filefix.name);
        clink->areafix.baseDir      = sstrdup(deflink->areafix.baseDir);
        clink->filefix.baseDir      = sstrdup(deflink->filefix.baseDir);
        clink->areafix.reportsFlags = sstrdup(deflink->areafix.reportsFlags);
        clink->filefix.reportsFlags = sstrdup(deflink->filefix.reportsFlags);
        clink->filefix.baseDir      = sstrdup(deflink->filefix.baseDir);
        clink->optGrp         = copyGroups(deflink->optGrp, deflink->numOptGrp);
        clink->areafix.frMask = copyGroups(deflink->areafix.frMask, deflink->areafix.numFrMask);
        clink->filefix.frMask = copyGroups(deflink->filefix.frMask, deflink->filefix.numFrMask);
        clink->areafix.dfMask = copyGroups(deflink->areafix.dfMask, deflink->areafix.numDfMask);
        clink->filefix.dfMask = copyGroups(deflink->filefix.dfMask, deflink->filefix.numDfMask);
    }
    else
    {
        memset(clink, 0, sizeof(s_link));
        /*  Set defaults like in parseLinkDefaults() */
        /*  set areafix default to on */
        clink->areafix.on = 1;
        clink->filefix.on = 1;
        /*  set defaults to export, import, mandatory (0), manual (0) */
        clink->aexport = 1;
        clink->import  = 1;
        clink->ourAka  = &(config->addr[0]);
        /*  set default maxUnpackedNetmail */
        clink->maxUnpackedNetmail = 100;
        /*  set FileFixFSC87Subset default to on */
        clink->FileFixFSC87Subset = 1;
    }

    clink->name = (char *)smalloc(strlen(token) + 1);
    strcpy(clink->name, token);
    clink->handle = clink->name;
    config->linkCount++;
    memset(&linkDefined, 0, sizeof(linkDefined));
    return 0;
} /* parseLink */

int parseAnnDef(char * token, s_fidoconfig * config)
{
    ps_anndef cAnnDef;

    if(token == NULL)
    {
        prErr("A name after %s is missing!", actualKeyword);
        return 1;
    }

    config->AnnDefs = srealloc(config->AnnDefs, sizeof(s_anndef) * (config->ADCount + 1));
    cAnnDef         = &(config->AnnDefs[config->ADCount]);
    memset(cAnnDef, 0, sizeof(s_anndef));
    cAnnDef->annAreaTag = sstrdup(token);
    config->ADCount++;
    return 0;
}

int parseAnnDefAddres(char * token, s_fidoconfig * config, int i)
{
    ps_anndef cAnnDef = NULL;
    hs_addr * addr;

    if(token == NULL)
    {
        prErr("A name after %s is missing!", actualKeyword);
        return 1;
    }

    cAnnDef = getDescrAnnDef(config);
    addr    = scalloc(1, sizeof(hs_addr));
    parseFtnAddrZS(token, addr);

    if(i == 1)
    {
        cAnnDef->annaddrto = addr;
    }

    if(i == 2)
    {
        cAnnDef->annaddrfrom = addr;
    }

    return 0;
} /* parseAnnDefAddres */

int parseNodelist(char * token, s_fidoconfig * config)
{
    if(token == NULL)
    {
        prErr("A name after %s is missing!", actualKeyword);
        return 1;
    }

    config->nodelists = srealloc(config->nodelists,
                                 sizeof(s_nodelist) * (config->nodelistCount + 1));
    memset(&(config->nodelists[config->nodelistCount]), 0, sizeof(s_nodelist));
    config->nodelists[config->nodelistCount].nodelistName = (char *)smalloc(strlen(token) + 1);
    strcpy(config->nodelists[config->nodelistCount].nodelistName, token);
    config->nodelists[config->nodelistCount].format         = fts5000;
    config->nodelists[config->nodelistCount].delAppliedDiff = 0;
    config->nodelists[config->nodelistCount].dailynodelist = 0;
    config->nodelistCount++;
    return 0;
}

int parseBool(char * token, unsigned int * value)
{
    char * iToken;

    if(token == NULL)
    {
        *value = 1;
        return 0;
    }

    iToken = strLower(sstrdup(token));

    if((strcmp(iToken, "on") == 0) || (strcmp(iToken, "yes") == 0) || (strcmp(iToken, "1") == 0))
    {
        *value = 1;
    }
    else if((strcmp(iToken,
                    "off") == 0) || (strcmp(iToken, "no") == 0) || (strcmp(iToken, "0") == 0))
    {
        *value = 0;
    }
    else
    {
        nfree(iToken);
        return 2;
    }

    nfree(iToken);
    return 0;
} /* parseBool */

int parseAutoPause(char * token, unsigned * autoPause)
{
    char * ptr;

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    } /* endif */

    for(ptr = token; *ptr; ptr++)
    {
        if(!isdigit(*ptr))
        {
            prErr("A parameter after %s is missing!", actualKeyword);
            return 1;
        } /* endif */
    } /* endfor */
    *autoPause = (unsigned)atoi(token);
    return 0;
}

int parsePause(char * token, unsigned * Pause)
{
    if((token == NULL) || (stricmp(token, "on") == 0))
    {
        *Pause = ECHOAREA | FILEAREA;
    }
    else if(stricmp(token, "earea") == 0)
    {
        *Pause |= ECHOAREA;
    }
    else if(stricmp(token, "farea") == 0)
    {
        *Pause |= FILEAREA;
    }
    else if(stricmp(token, "off") == 0)
    {
        *Pause = NOPAUSE;
    }
    else
    {
        prErr("Wrong Pause parameter!");
        return 1; /*  error */
    }

    return 0;
}

int parseUInt(char * token, unsigned int * uint)
{
    long var = 0;

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    sscanf(token, "%ld", &var);

    if(var < 0)
    {
        prErr("The negative value of %s is invalid!", actualKeyword);
        return 1;
    }

    *uint = (unsigned int)var;
    return 0;
}

int parseOctal(char * token, unsigned int * octal)
{
    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    sscanf(token, "%o", octal);
    return 0;
}

int parsePWD(char * token, char ** pwd)
{
    if(token == NULL)              /*  return empty password */
    {
        *pwd      = (char *)smalloc(1);
        (*pwd)[0] = '\0';
        return 0;
    }

    *pwd = sstrdup(token);

    if(*pwd == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int parseHandle(char * token, s_fidoconfig * config)
{
    s_link * clink;

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    clink         = getDescrLink(config);
    clink->handle = (char *)smalloc(strlen(token) + 1);
    strcpy(clink->handle, token);
    return 0;
}

/* Return 0 if OK, 1 if invalid parameter, 2 if another error */
int parseRoute(char * token, s_fidoconfig * config, s_route ** route, unsigned * count, e_id id)
{
    char * option;
    char * iOption;
    int rc = 0;
    s_route * actualRoute;

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    option = strtok(token, " \t");

    if(option == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    *route      = srealloc(*route, sizeof(s_route) * (*count + 1));
    actualRoute = &(*route)[*count];
    memset(actualRoute, '\0', sizeof(s_route));
    actualRoute->id      = id;
    actualRoute->flavour = flUndef;

    while(option != NULL)
    {
        iOption = strLower(sstrdup(option));

        if(strcmp(iOption, "hold") == 0)
        {
            actualRoute->flavour = flHold;
        }
        else if(strcmp(iOption, "normal") == 0)
        {
            actualRoute->flavour = flNormal;
        }
        else if(strcmp(iOption, "crash") == 0)
        {
            actualRoute->flavour = flCrash;
        }
        else if(strcmp(iOption, "direct") == 0)
        {
            actualRoute->flavour = flDirect;
        }
        else if(strcmp(iOption, "immediate") == 0)
        {
            actualRoute->flavour = flImmediate;
        }
        else if(strcmp(iOption, "hub") == 0)
        {
            actualRoute->routeVia = hub;
        }
        else if(strcmp(iOption, "host") == 0)
        {
            actualRoute->routeVia = host;
        }
        else if(strcmp(iOption, "boss") == 0)
        {
            actualRoute->routeVia = boss;
        }
        else if(strcmp(iOption, "noroute") == 0)
        {
            actualRoute->routeVia = noroute;
        }
        else if(strcmp(iOption, "no-route") == 0)
        {
            actualRoute->routeVia = noroute;
        }
        else if(strcmp(iOption, "nopack") == 0)
        {
            actualRoute->routeVia = nopack;
        }
        else if(strcmp(iOption, "no-pack") == 0)
        {
            actualRoute->routeVia = nopack;
        }
        else if(isdigit(option[0]) || (option[0] == '*') || (option[0] == '?'))
        {
            if((actualRoute->routeVia == 0) && (actualRoute->target == NULL))
            {
                actualRoute->target = getLink(config, option);
                actualRoute->viaStr = (char *)smalloc(strlen(option) + 1);
                strcpy(actualRoute->viaStr, option);
            }
            else
            {
                if(actualRoute->pattern == NULL)
                {
                    /* 2 for additional .0 if needed */
                    actualRoute->pattern = (char *)smalloc(strlen(option) + 2 + 1);
                    strcpy(actualRoute->pattern, option);

                    if((strchr(option, '.') == NULL) && (strchr(option, '*') == NULL))
                    {
                        strcat(actualRoute->pattern, ".0");
                    }

                    (*count)++;
                }
                else
                {
                    /*  add new Route for additional patterns */
                    *route      = srealloc(*route, sizeof(s_route) * (*count + 1));
                    actualRoute = &(*route)[*count];
                    memcpy(actualRoute, &(*route)[(*count) - 1], sizeof(s_route));

                    if((*route)[(*count) - 1].viaStr != NULL)
                    {
                        actualRoute->viaStr = sstrdup((*route)[(*count) - 1].viaStr);
                    }

                    /* 2 for additional .0 if needed */
                    actualRoute->pattern = (char *)smalloc(strlen(option) + 2 + 1);
                    strcpy(actualRoute->pattern, option);

                    if((strchr(option, '.') == NULL) && (strchr(option, '*') == NULL))
                    {
                        strcat(actualRoute->pattern, ".0");
                    }

                    (*count)++;
                }
            }

            if((actualRoute->target == NULL) && (actualRoute->routeVia == 0))
            {
                prErr("Link %s not found in Route statement!", actualRoute->viaStr);
                rc = 2;
            }
        }

        nfree(iOption);
        option = strtok(NULL, " \t");
    }

    /* set flavour if it isn't specified in the statement */
    /* and routeVia is not 'nopack' */
    if(actualRoute->flavour == flUndef && actualRoute->routeVia != nopack)
    {
        if(actualRoute->target == NULL)
        {
            prErr("You must either specify flavour or use defined link as target");
            rc = 2;
        }
        else
        {
            actualRoute->flavour = actualRoute->target->netMailFlavour;
        }
    }

    return rc;
} /* parseRoute */

int parsePack(char * line, s_fidoconfig * config)
{
    char * p, * c;
    s_pack * pack;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    /* check for no link definition was before */
    if(config->linkCount > 0)
    {
        prErr("Unable to add commandline for packer after link definition!");
        return 2;
    }

    p = strtok(line, " \t");
    c = getRestOfLine();

    if((p != NULL) && (c != NULL))
    {
        /*  add new pack statement */
        config->packCount++;
        config->pack = srealloc(config->pack, config->packCount * sizeof(s_pack));
        /*  fill new pack statement */
        pack         = &(config->pack[config->packCount - 1]);
        pack->packer = (char *)smalloc(strlen(p) + 1);
        strcpy(pack->packer, p);
        pack->call = (char *)smalloc(strlen(c) + 1);
        strcpy(pack->call, c);

        if(strncasecmp(pack->call, ZIPINTERNAL, strlen(ZIPINTERNAL)))
        {
            if(strstr(pack->call, "$a") == NULL)
            {
                prErr("$a missing in pack statement %s!", actualLine);
                return 2;
            }

            if(strstr(pack->call, "$f") == NULL)
            {
                prErr("$f missing in pack statement %s!", actualLine);
                return 2;
            }
        }

        return 0;
    }
    else
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }
} /* parsePack */

int parseUnpack(char * line, s_fidoconfig * config)
{
    char * p, * c;
    char * error;
    s_unpack * unpack;
    unsigned char code;
    int i;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    /*  ToDo: Create replacement for strtok which handles "str" */
    /* Skip white spaces */
    for(p = line; (*p == ' ') || (*p == '\t'); p++)
    {}

    if(*p != '\0')
    {
        if(*p == '\"')
        {
            /* skip till next double qoute */
            for(c = ++p; (*c != '\"') && (*c != '\0'); c++)
            {}
        }
        else
        {
            /* skip till next white space */
            for(c = p; (*c != ' ') && (*c != '\t') && (*c != '\0'); c++)
            {}
        }

        if(*c != '\0')
        {
            *c++ = '\0';
            stripLeadingChars(c, " \t");
        }
    }
    else
    {
        c = NULL;
    }

    if(c != NULL)
    {
        /*  add new unpack statement */
        config->unpackCount++;
        config->unpack = srealloc(config->unpack, config->unpackCount * sizeof(s_unpack));
        /*  fill new unpack statement */
        unpack       = &(config->unpack[config->unpackCount - 1]);
        unpack->call = (char *)smalloc(strlen(p) + 1);
        strcpy(unpack->call, p);

        if(strncasecmp(unpack->call, ZIPINTERNAL, strlen(ZIPINTERNAL)))
        {
            /* zipInternal is not used */
            if(strstr(unpack->call, "$a") == NULL)
            {
                prErr("$a missing in unpack statement %s!", actualLine);
                return 2;
            }
        }

        p = strtok(c, " \t");    /* p contains offset now */
        c = strtok(NULL, " \t"); /* c contains match code now */

        if((p == NULL) || (c == NULL))
        {
            prErr("offset or match code missing in unpack statement %s!", actualLine);
            return 1;
        }

        unpack->offset = (unsigned)strtol(p, &error, 0);

        if((error != NULL) && (*error != '\0'))
        {
            prErr("The number is wrong for offset in unpack!");
            return 1;     /*  error occured; */
        }

        unpack->matchCode = (unsigned char *)smalloc(strlen(c) / 2 + 1);
        unpack->mask      = (unsigned char *)smalloc(strlen(c) / 2 + 1);

        /*  parse matchcode statement */
        /*  this looks a little curvy, I know. Remember, I programmed this at 23:52 :) */
        for(i = 0, error = NULL; c[i] != '\0' && error == NULL; i++)
        {
            code = (unsigned char)toupper(c[i]);
            /*  if code equals to '?' set the corresponding bits  of  mask[] to 0 */
            unpack->mask[i / 2] = i % 2 ==
                                  0 ? (code !=
                                       '?' ? 0xF0 : 0) : unpack->mask[i / 2] |
                                  (code != '?' ? 0xF : 0);
            /*  find the numeric representation of hex code */
            /*  if this is a '?' code equals to 0 */
            code =
                (isdigit(code) ? code -
                 '0' : (isxdigit(code) ? code - 'A' +
                        10 : (code == '?' ? 0 : (error = c + i, 0xFF))));
            unpack->matchCode[i / 2] = i % 2 == 0 ? code << 4 : unpack->matchCode[i / 2] | code;
        }

        if(error)
        {
            prErr("matchCode can\'t contain %c in unpack statement %s!", *error, actualLine);
            return 1;
        }

        if(i % 2 != 0)
        {
            prErr("matchCode must be byte-aligned in unpack statement %s!", actualLine);
            return 1;
        }

        unpack->codeSize = i / 2;
        return 0;
    }
    else
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }
} /* parseUnpack */

#if 0
static int f_accessable(char * token)
{
/*  We don't need a real fexist function here, and we don't want to */
/*        be dependent on SMAPI just because of this. For us, it is enough */
/*        to see if the file is accessible */
/*  BUT WE DON'T KNOW ABOUT DIRS! */

#ifdef __UNIX__
    struct stat sb;

    if(stat(token, &sb))
    {
        return 0; /*  cannot stat the file */
    }

    if(access(token, R_OK))
    {
        return 0; /*  cannot access the file */
    }

    return 1;

#else
    FILE * f = fopen(token, "rb");

    if(f == NULL)
    {
        return 0;
    }

    fclose(f);
    return 1;

#endif
} /* f_accessable */

#endif /* if 0 */

int parseFileName(char * line, char ** name, char ** alreadyDefined)
{
    char * token;

    if(*name != NULL)
    {
        if(alreadyDefined == NULL || *alreadyDefined)
        {
            prErr("Duplicate file name!");
            return 1;
        }

        nfree(*name);
    }

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(line[0] == '\"')
    {
        token = (char *)smalloc(strlen(line) + 1);
        sscanf(line, "\"%[^\"]s", token);
    }
    else
    {
        token = strtok(line, " \t");
    }

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

/*    if (f_accessable(token)) { */
    if(fexist(token))   /*  fexist knows about dirs */
/*       (*name) = smalloc(strlen(token)+1); */
/*       strcpy((*name), token); */
    {
        xstrcat(name, token);

        if(alreadyDefined)
        {
            *alreadyDefined = *name;
        }
    }
    else
    {
        prErr("File not found or no permission: %s!", token);

        if(line[0] == '\"')
        {
            nfree(token);
        }

        return 2;
    }

    if(line[0] == '\"')
    {
        nfree(token);
    }

    return 0;
} /* parseFileName */

/* Parse loglevels string
   Expand ranges like x-y;
   Ignore spaces & etc (recognizes digits & letters only).
 */
int parseLoglevels(char * line, char ** loglevels)
{
    char * ll, * temp; /* Array for store */
    char * p = line;
    int i, k;

    if(!line)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    ll = calloc(256, sizeof(char));

    if(!ll)
    {
        prErr("Low memory!");
        return 1;
    }

    while(*p)   /* scan string */
    {
        if(isdigit(*p) || isalpha(*p))
        {
            ll[(int)*p] = 1;
        }
        else if(*p == '-' && p != *loglevels)
        {
            for(i = *(p - 1), k = *(p + 1); i && i < k; i++)
            {
                ll[i] = 1;
            }
        }

        p++;
    }
    p = temp = smalloc('z' - 'a' + 'Z' - 'A' + '9' - '0' + 4);

    for(i = '0'; i <= '9'; i++)
    {
        if(ll[i])
        {
            *(p++) = (char)i;
        }
    }

    for(i = 'A'; i <= 'Z'; i++)
    {
        if(ll[i])
        {
            *(p++) = (char)i;
        }
    }

    for(i = 'a'; i <= 'z'; i++)
    {
        if(ll[i])
        {
            *(p++) = (char)i;
        }
    }
    *p         = '\0';
    *loglevels = sstrdup(temp);
    nfree(temp);
    nfree(ll);
    return 0;
} /* parseLoglevels */

int parsePackerDef(char * line, s_fidoconfig * config, s_pack ** packerDef)
{
    unsigned int i;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(stricmp(line, "none") == 0)
    {
        (*packerDef) = NULL;
        return 0;
    }

    for(i = 0; i < config->packCount; i++)
    {
        if(stricmp(line, config->pack[i].packer) == 0)
        {
            (*packerDef) = &(config->pack[i]);
            return 0;
        }
    }
    prErr("Packer %s not found for packer statement!", line);
    return 2;
} /* parsePackerDef */

int parseFlavour(char * line, e_flavour * flavour)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "hold") == 0)
    {
        *flavour = flHold;
    }
    else if(strcmp(iLine, "normal") == 0)
    {
        *flavour = flNormal;
    }
    else if(strcmp(iLine, "direct") == 0)
    {
        *flavour = flDirect;
    }
    else if(strcmp(iLine, "crash") == 0)
    {
        *flavour = flCrash;
    }
    else if(strcmp(iLine, "immediate") == 0)
    {
        *flavour = flImmediate;
    }
    else
    {
        prErr("Unknown %s value %s!", actualKeyword, line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseFlavour */

int parseAttr(char * token, char ** attrs, long * bitattr)
{
    char * p;
    int parsed;

    nfree(*attrs);
    *bitattr = 0;
    parsed   = parseAttrString(token, attrs, bitattr, &p);
    assert(parsed >= 0); /* Should be */
    unused(parsed);

    if(*p != '\0')
    {
        prErr("Unknown flag %s!", p);
        nfree(*attrs);
        return 2;
    }

    return 0;
}

int parseUUEechoAreas(char * token, char ** grp[], unsigned int * count)
{
    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    *grp           = srealloc(*grp, sizeof(char *) * (*count + 1));
    (*grp)[*count] = sstrdup(token);
    (*count)++;
    return 0;
}

/* Parse strings like "token1, token2,token3 token4" into s_str_array */
int parseStringList(char * token, s_str_array ** ss)
{
    assert(token != NULL && ss != NULL);
    nfree(*ss);
    *ss = makeStrArray(token);
    return 0;
}

int parseGrp(char * token, char ** grp[], unsigned int * count)
{
    char * p;

    p = token;

    while(*p && strchr(" \t,", *p))
    {
        p++;
    }

    if(!*p)
    {
        return 0;
    }

    for(*count = 1; ; (*count)++)
    {
        while(*p && !strrchr(" \t,", *p))
        {
            p++;
        }

        while(*p && strchr(" \t,", *p))
        {
            p++;
        }

        if(!*p)
        {
            break;
        }
    }
    p = token;

    while(*p && strchr(" \t,", *p))
    {
        p++;
    }
    *grp      = smalloc(sizeof(char *) * (*count) + strlen(p) + 1);
    (*grp)[0] = (char *)(*grp + (*count));
    strcpy((*grp)[0], p);
    p        = (*grp)[0];
    (*count) = 1;

    for( ; ; )
    {
        while(*p && !strrchr(" \t,", *p))
        {
            p++;
        }

        if(!*p)
        {
            break;
        }

        *p++ = '\0';

        while(*p && strchr(" \t,", *p))
        {
            p++;
        }

        if(!*p)
        {
            break;
        }

        (*grp)[(*count)++] = p;
    }
    return 0;
} /* parseGrp */

/* and the parseGroup: */
/*  i make some checking... maybe it is better check if the pointer exist from */
/*  copyString function? */
/*  i removed some checking... ;-) */
/*  groups may be copied from linkDefaults */
int parseGroup(char * token, s_fidoconfig * config, int i)
{
    s_link * link     = NULL;
    ps_anndef cAnnDef = NULL;

    if(token == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(i != 2)
    {
        link = getDescrLink(config);
    }

    if(i == 6 || i == 7)
    {
        cAnnDef = getDescrAnnDef(config);
    }

    switch(i)
    {
        case 0:

            if(link->AccessGrp)
            {
                freeGroups(link->AccessGrp, link->numAccessGrp);
            }

            link->AccessGrp    = NULL;
            link->numAccessGrp = 0;
            parseGrp(token, &(link->AccessGrp), &(link->numAccessGrp));
            break;

        case 1:
            nfree(link->LinkGrp);
            fc_copyString(token, &link->LinkGrp);
            break;

        case 2:

            if(config->numPublicGroup != 0)
            {
                prErr("Duplicate parameter after %s!", actualKeyword);
                return 1;
            }

            parseGrp(token, &(config->PublicGroup), &(config->numPublicGroup));
            break;

        case 3:

            if(link->optGrp)
            {
                freeGroups(link->optGrp, link->numOptGrp);
            }

            link->optGrp    = NULL;
            link->numOptGrp = 0;
            parseGrp(token, &(link->optGrp), &(link->numOptGrp));
            break;

        case 4:

            if(link->areafix.frMask)
            {
                freeGroups(link->areafix.frMask, link->areafix.numFrMask);
            }

            link->areafix.frMask    = NULL;
            link->areafix.numFrMask = 0;
            parseGrp(token, &(link->areafix.frMask), &(link->areafix.numFrMask));
            break;

        case 14:

            if(link->filefix.frMask)
            {
                freeGroups(link->filefix.frMask, link->filefix.numFrMask);
            }

            link->filefix.frMask    = NULL;
            link->filefix.numFrMask = 0;
            parseGrp(token, &(link->filefix.frMask), &(link->filefix.numFrMask));
            break;

        case 5:

            if(link->areafix.dfMask)
            {
                freeGroups(link->areafix.dfMask, link->areafix.numDfMask);
            }

            link->areafix.dfMask    = NULL;
            link->areafix.numDfMask = 0;
            parseGrp(token, &(link->areafix.dfMask), &(link->areafix.numDfMask));
            break;

        case 15:

            if(link->filefix.dfMask)
            {
                freeGroups(link->filefix.dfMask, link->filefix.numDfMask);
            }

            link->filefix.dfMask    = NULL;
            link->filefix.numDfMask = 0;
            parseGrp(token, &(link->filefix.dfMask), &(link->filefix.numDfMask));
            break;

        case 6:

            if(cAnnDef->annInclude)
            {
                freeGroups(cAnnDef->annInclude, cAnnDef->numbI);
            }

            cAnnDef->annInclude = NULL;
            cAnnDef->numbI      = 0;
            parseGrp(token, &(cAnnDef->annInclude), &(cAnnDef->numbI));
            break;

        case 7:

            if(cAnnDef->annExclude)
            {
                freeGroups(cAnnDef->annExclude, cAnnDef->numbE);
            }

            cAnnDef->annExclude = NULL;
            cAnnDef->numbE      = 0;
            parseGrp(token, &(cAnnDef->annExclude), &(cAnnDef->numbE));
            break;

        case 8:

            if(link->RescanGrp)
            {
                freeGroups(link->RescanGrp, link->numRescanGrp);
            }

            link->RescanGrp    = NULL;
            link->numRescanGrp = 0;
            parseGrp(token, &(link->RescanGrp), &(link->numRescanGrp));
            break;

        default:
            break;
    } /* switch */
    return 0;
} /* parseGroup */

int parseLocalArea(char * token, s_fidoconfig * config)
{
    int rc;
    s_area * area;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->localAreas =
        srealloc(config->localAreas, sizeof(s_area) * (config->localAreaCount + 1));
    area           = &(config->localAreas[config->localAreaCount]);
    area->areaType = ECHOAREA;
    rc             = parseArea(config, token, area, 0);
    config->localAreaCount++;
    return rc;
}

int parseCarbonRule(char * token, s_fidoconfig * config)
{
    s_carbon * cb = &(config->carbons[config->carbonCount - 1]);

    if(token == NULL)
    {
        prErr("OR|AND|NOT after %s is missing!", actualKeyword);
        return 1;
    }

    /* rules are valid for the expressions that follow */
    /* but carbonRule AND also involves cb */
    /* expressions can start with NOT, but not with AND */
    if(stricmp(token, "NOT") == 0)
    {
        _carbonrule = CC_NOT | CC_AND;

        if(config->carbonCount > 0 && (cb->areaName == NULL && cb->move != 2)) /* no action */
        {
            cb->rule |= CC_AND; /* AND NOT .. with next expr */
        }
    }
    else if(stricmp(token, "OR") == 0)
    {
        _carbonrule = CC_OR; /* =0 */

        if(config->carbonCount)
        {
            cb->rule &= CC_NOT;
        }
    }
    else if(stricmp(token, "AND") == 0)
    {
        _carbonrule = CC_AND;

        if(config->carbonCount > 0 && (cb->areaName == NULL && cb->move != 2)) /* no action */
        {
            cb->rule |= CC_AND;
        }
    }
    else
    {
        prErr("OR|AND|NOT after %s is missing!", actualKeyword);
        return 1;
    }

    return 0;
} /* parseCarbonRule */

int parseCarbon(char * token, s_fidoconfig * config, e_carbonType ctype)
{
    int c = config->carbonCount;
    s_carbon * cb;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->carbonCount++;
    config->carbons = srealloc(config->carbons, sizeof(s_carbon) * (config->carbonCount));
    cb = &(config->carbons[c]);
    memset(cb, 0, sizeof(s_carbon));
    cb->ctype = ctype;
    cb->rule  = _carbonrule;

    if(ctype == ct_addr)
    {
        parseFtnAddrZS(token, &(cb->addr));
    }
    else
    {
        /*  strip trailing "" */
        if(token[0] == '"' && token[strlen(token) - 1] == '"')
        {
            token++;
            token[strlen(token) - 1] = '\0';
        }

        /* fc_copyString(token, &(cb->str)); */
        xstrcat(&(cb->str), token);
    }

    return 0;
} /* parseCarbon */

int parseCarbonArea(char * token, s_fidoconfig * config, int move)
{
    char * areaName, * reason;
    int c         = config->carbonCount - 1;
    s_carbon * cb = &(config->carbons[c]);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    if(!config->carbonCount || (cb->str == NULL && cb->addr.zone == 0))
    {
        prErr("No carbon codition specified before %s", actualKeyword);
        return 1;
    }

    if(cb->move == CC_delete)
    {
        prErr("CarbonDelete was specified before %s", actualKeyword);
        return 1;
    }

    if(cb->extspawn)
    {
        prErr("Extspawn was specified before %s", actualKeyword);
        return 1;
    }

    if(cb->areaName != NULL)
    {
        prErr("CarbonArea already defined before %s", actualKeyword);
        return 1;
    }

    fc_copyString(token, &(cb->areaName));
    cb->move    = move;
    _carbonrule = CC_AND; /* default */

    cb->rule &= CC_NOT; /* switch AND off */
    /* checking area*/
    /* it is possible to have several groups of expressions and each of them */
    /* should have a carbonArea in the last expression */
    /* so now the area is known, the previous expressions must be checked */
    areaName = cb->areaName;
    reason   = cb->reason;

    while(c--)
    {
        cb--;

        /* this was the end of a previous set expressions */
        if(cb->areaName != NULL)  /* carboncopy, -move or extspawn */
        {
            break;
        }

        /* this was the end of a previous set expressions */
        if(cb->move == CC_delete)         /* carbondelete */
        {
            break;
        }

        fc_copyString(areaName, &(cb->areaName));

        if(reason)
        {
            fc_copyString(reason, &(cb->reason));
        }

        cb->move = move;
    }
    return 0;
} /* parseCarbonArea */

int parseCarbonDelete(char * token, s_fidoconfig * config)
{
    unsigned int c = config->carbonCount - 1;
    s_carbon * cb  = &(config->carbons[c]);

    if(token != NULL)
    {
        prErr("There are extra parameters after %s!", actualKeyword);
        return 1;
    }

    /*   if (config->carbonCount == 0) {*/
    if(config->carbonCount == 0 || (cb->str == NULL && cb->addr.zone == 0))
    {
        prErr("No carbon condition specified before %s", actualKeyword);
        return 1;
    }

    if(cb->extspawn)
    {
        prErr("CarbonExtern was specified before %s", actualKeyword);
        return 1;
    }

    if(cb->areaName != NULL)
    {
        prErr("CarbonArea was specified before %s", actualKeyword);
        return 1;
    }

    cb->move    = CC_delete;
    _carbonrule = CC_AND;
    cb->rule   &= CC_NOT;

    /* checking area*/
    /* it is possible to have several groups of expressions and each of them */
    /* should have a carbonArea in the last expression */
    /* so now the area is known, the previous expressions must be checked */
    while(c--)
    {
        cb--;

        if(cb->areaName != NULL) /* carboncopy, -move, extern */
        {
            break; /* this was the end of a previous set expressions */
        }

        if(cb->move == CC_delete) /* delete */
        {
            break;
        }

        if((!cb->rule) & CC_AND) /* OR */
        {
            cb->move = CC_delete;
        }
    }
    return 0;
} /* parseCarbonDelete */

int parseCarbonExtern(char * token, s_fidoconfig * config)
{
    unsigned int c = config->carbonCount - 1;
    s_carbon * cb  = &(config->carbons[c]);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    if(config->carbonCount == 0 || (cb->str == NULL && cb->addr.zone == 0))
    {
        prErr("No carbon condition specified before %s", actualKeyword);
        return 1;
    }

    if(cb->extspawn)
    {
        prErr("CarbonExtern was already specified before %s", actualKeyword);
        return 1;
    }

    if(cb->areaName != NULL)
    {
        prErr("CarbonArea defined before %s!", actualKeyword);
        return 1;
    }

    if(cb->move == CC_delete)
    {
        prErr("CarbonDelete defined before %s!", actualKeyword);
        return 1;
    }

    fc_copyString(token, &(cb->areaName));
    cb->extspawn = 1;
    cb->move     = CC_copy;
    _carbonrule  = CC_AND;
    cb->rule    &= CC_NOT;

    /* checking area*/
    /* it is possible to have several groups of expressions and each of them */
    /* should have a carbonArea in the last expression */
    /* so now the area is known, the previous expressions must be checked */
    while(c--)
    {
        cb--;

        if(cb->areaName != NULL) /* carboncopy, -move, extern */
        {
            break; /* this was the end of a previous set expressions */
        }

        if(cb->move == CC_delete) /* delete */
        {
            break;
        }

        if((!cb->rule) & CC_AND) /* OR */
        {
            fc_copyString(token, &(cb->areaName));
            cb->extspawn = 1;
            cb->move     = CC_copy;
        }
    }

    /* +AS+ */
    if(tolower(*actualKeyword) == 'n')
    {
        cb->netMail = 1;
    }
    else
    {
        cb->netMail = 0;
    }

    /* -AS- */
    return 0;
} /* parseCarbonExtern */

int parseCarbonReason(char * token, s_fidoconfig * config)
{
    s_carbon * cb = &(config->carbons[config->carbonCount - 1]);

    /* I know, when count==0 this will give strange results, but */
    /* in that case, cb will not be used */
    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    /*   if (config->carbonCount == 0) {*/
    if(config->carbonCount == 0 || (cb->str == NULL && cb->addr.zone == 0))
    {
        prErr("No carbon condition specified before %s", actualKeyword);
        return 1;
    }

    fc_copyString(token, &(cb->reason));
    return 0;
}

int parseForwardPkts(char * token, s_link * link)
{
    if(token && stricmp(token, "secure") == 0)
    {
        link->forwardPkts = fSecure;
    }
    else
    {
        unsigned int bval = link->forwardPkts;
        return parseBool(token, &bval);
    }

    return 0;
}

int parseAllowEmptyPktPwd(char * token, s_fidoconfig * config, s_link * link)
{
    unsigned t;

    unused(config);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    if(stricmp(token, "secure") == 0)
    {
        link->allowEmptyPktPwd = eSecure;
    }
/*   else if (stricmp(token, "on")==0) link->allowEmptyPktPwd = eOn;*/
/*   else if (stricmp(token, "off")==0) link->allowEmptyPktPwd = eOff;*/
    else if(!parseBool(token, &t))
    {
        if(t)
        {
            link->allowEmptyPktPwd = eOn;
        }
        else
        {
            link->allowEmptyPktPwd = eOff;
        }
    }
    else
    {
        return 2;
    }

    return 0;
} /* parseAllowEmptyPktPwd */

int parseAllowPktAddrDiffer(char * token, s_fidoconfig * config, s_link * link)
{
    unused(config);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    if(stricmp(token, "on") == 0)
    {
        link->allowPktAddrDiffer = pdOn;
    }
    else if(stricmp(token, "off") == 0)
    {
        link->allowPktAddrDiffer = pdOff;
    }
    else
    {
        return 2;
    }

    return 0;
}

int parseNodelistFormat(char * token, s_fidoconfig * config, s_nodelist * nodelist)
{
    char * iToken;

    unused(config);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    iToken = strLower(sstrdup(token));

    if((strcmp(iToken, "fts5000") == 0) || (strcmp(iToken, "standard") == 0))
    {
        nodelist->format = fts5000;
    }
    else if(strcmp(iToken, "points24") == 0)
    {
        nodelist->format = points24;
    }
    else if(strcmp(iToken, "points4d") == 0)
    {
        nodelist->format = points4d;
    }
    else
    {
        nfree(iToken);
        return 2;
    }

    nfree(iToken);
    return 0;
} /* parseNodelistFormat */

int parseTypeDupes(char * line, e_typeDupeCheck * typeDupeBase, unsigned * DayAge)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "textdupes") == 0)
    {
        *typeDupeBase = textDupes;
    }
    else if(strcmp(iLine, "hashdupes") == 0)
    {
        *typeDupeBase = hashDupes;
    }
    else if(strcmp(iLine, "hashdupeswmsgid") == 0)
    {
        *typeDupeBase = hashDupesWmsgid;
    }
    else if(strcmp(iLine, "commondupebase") == 0)
    {
        *typeDupeBase = commonDupeBase;

        if(*DayAge == 0)
        {
            *DayAge = (unsigned)5;
        }
    }
    else
    {
        prErr("Unknown type base of dupes %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseTypeDupes */

int parseSaveTic(const s_fidoconfig * config, char * token, s_savetic * savetic)
{
    char * tok;
    int rc;

    unused(config);

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    memset(savetic, 0, sizeof(s_savetic));
    tok = strtok(token, " \t");

    if(tok == NULL)
    {
        prErr("An areaname mask after %s is missing!", actualKeyword);
        return 1;         /* if there is no areaname mask */
    }

    savetic->fileAreaNameMask = sstrdup(tok);
    tok = strtok(NULL, " \t");

    if(tok == NULL)
    {
        prErr("Parameters after %s are missing!", token);
        return 1;
    }

    if(*tok == '-')
    {
        if(tok[1] == 'l')
        {
            savetic->fileAction = 2;
        }
        else if(tok[1] == 'c')
        {
            savetic->fileAction = 1;
        }

        tok = strtok(NULL, " \t");
    }

    rc = parsePath(tok, &savetic->pathName, NULL);

    if(rc == 0)
    {
        tok = strtok(NULL, " \t");

        if(tok)
        {
            parseNumber(tok, 10, &(savetic->days2save));
        }
    }

    return rc;
} /* parseSaveTic */

int parseSaveTicStatement(char * token, s_fidoconfig * config)
{
    int rc;

    if(token == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    config->saveTic = srealloc(config->saveTic, sizeof(s_savetic) * (config->saveTicCount + 1));
    rc = parseSaveTic(config, token, &(config->saveTic[config->saveTicCount]));
    config->saveTicCount++;
    return rc;
}

int parseExecOnFile(char * line, s_fidoconfig * config)
{
    char * a, * f, * c;
    s_execonfile * execonfile;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    a = strtok(line, " \t");
    f = strtok(NULL, " \t");
    c = getRestOfLine();

    if((a != NULL) && (f != NULL) && (c != NULL))
    {
        /*  add new execonfile statement */
        config->execonfileCount++;
        config->execonfile =
            srealloc(config->execonfile, config->execonfileCount * sizeof(s_execonfile));
        /*  fill new execonfile statement */
        execonfile           = &(config->execonfile[config->execonfileCount - 1]);
        execonfile->filearea = (char *)smalloc(strlen(a) + 1);
        strcpy(execonfile->filearea, a);
        execonfile->filename = (char *)smalloc(strlen(f) + 1);
        strcpy(execonfile->filename, f);
        execonfile->command = (char *)smalloc(strlen(c) + 1);
        strcpy(execonfile->command, c);
        return 0;
    }
    else
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }
} /* parseExecOnFile */

void printNodelistError(void)
{
    prErr("You must define a nodelist first before you use %s!", actualKeyword);
}

int parseLinkDefaults(char * token, s_fidoconfig * config)
{
    if(token == NULL)
    {
        config->describeLinkDefaults = 1;
    }
    else
    {
        if(stricmp(token, "begin") == 0)
        {
            config->describeLinkDefaults = 1;
        }
        else if(stricmp(token, "end") == 0)
        {
            config->describeLinkDefaults = 0;
        }
        else if(stricmp(token, "destroy") == 0)
        {
            config->describeLinkDefaults = 0;
            freeLink(config->linkDefaults);
            config->linkDefaults = NULL;
        }
        else
        {
            return 2;
        }
    }

    if(config->describeLinkDefaults && config->linkDefaults == NULL)
    {
        config->linkDefaults = scalloc(1, sizeof(s_link));
        /*  Set defaults like in parseLink() */
        /*  set areafix default to on */
        config->linkDefaults->areafix.on = 1;
        config->linkDefaults->filefix.on = 1;
        /*  set defaults to export, import, mandatory (0), manual (0) */
        config->linkDefaults->aexport = 1;
        config->linkDefaults->import  = 1;
        config->linkDefaults->ourAka  = &(config->addr[0]);
        /*  set defaults maxUnpackedNetmail */
        config->linkDefaults->maxUnpackedNetmail = 100;
        config->linkDefaults->dailyBundles       = config->dailyBundles;
        /*  set FileFixFSC87Subset default to on */
        config->linkDefaults->FileFixFSC87Subset = 1;
    }

    memset(&linkDefined, 0, sizeof(linkDefined));
    return 0;
} /* parseLinkDefaults */

int parseNamesCase(char * line, e_nameCase * value)
{
    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(stricmp(line, "lower") == 0)
    {
        *value = eLower;
    }
    else if(stricmp(line, "upper") == 0)
    {
        *value = eUpper;
    }
    else
    {
        prErr("Unknown case parameter %s!", line);
        return 2;
    }

    return 0;
}

int parseNamesCaseConversion(char * line, e_nameCaseConvertion * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "lower") == 0)
    {
        *value = cLower;
    }
    else if(strcmp(iLine, "upper") == 0)
    {
        *value = cUpper;
    }
    else if(strcmp(iLine, "dont") == 0)
    {
        *value = cDontTouch;
    }
    else if(strcmp(iLine, "donttouch") == 0)
    {
        *value = cDontTouch;
    }
    else if(strcmp(iLine, "same") == 0)
    {
        *value = cDontTouch;
    }
    else
    {
        prErr("Unknown case convertion parameter %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseNamesCaseConversion */

int parseBundleNameStyle(char * line, e_bundleFileNameStyle * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "addrdiff") == 0)
    {
        *value = eAddrDiff;
    }
    else if(strcmp(iLine, "addrdiffalways") == 0)
    {
        *value = eAddrDiffAlways;
    }
    else if(strcmp(iLine, "timestamp") == 0)
    {
        *value = eTimeStamp;
    }
    else if(strcmp(iLine, "amiga") == 0)
    {
        *value = eAmiga;
    }
    else if(strcmp(iLine, "addrscrc32") == 0)
    {
        *value = eAddrsCRC32;
    }
    else if(strcmp(iLine, "addrscrc32always") == 0)
    {
        *value = eAddrsCRC32Always;
    }
    else
    {
        prErr("Unknown bundle name style %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseBundleNameStyle */

int parseLinkWithILogType(char * line, e_linkWithImportLog * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(*value != lwiNo)
    {
        prErr("LinkWithImportLog redefinition");
        return 2;
    }

    iLine = strLower(sstrdup(line));
    striptwhite(iLine);

    if(strcmp(iLine, "yes") == 0)
    {
        *value = lwiYes;
    }
    else if(strcmp(iLine, "no") == 0)
    {
        *value = lwiNo;
    }
    else if(strcmp(iLine, "kill") == 0)
    {
        *value = lwiKill;
    }
    else
    {
        prErr("Unknown LinkWithImportLog value %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseLinkWithILogType */

int parseKludgeAreaNetmailType(char * line, e_kludgeAreaNetmail * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(*value != kanKill)
    {
        prErr("kludgeAreaNetmail redefinition");
        return 2;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "kill") == 0)
    {
        *value = kanKill;
    }
    else if(strcmp(iLine, "ignore") == 0)
    {
        *value = kanIgnore;
    }
    else if(strcmp(iLine, "echomail") == 0)
    {
        *value = kanEcho;
    }
    else
    {
        prErr("Unknown klugdeAreaNetmail value %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseKludgeAreaNetmailType */

int parseSendMailCmd(char * line, char ** sendMailCmd)
{
    if(!line)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(*sendMailCmd)
    {
        prErr("sendMailCmd redefinition!");
        return 2;
    }

    *sendMailCmd = sstrdup(line);
    return 0;
}

int parseEmailEncoding(char * line, e_emailEncoding * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "uue") == 0)
    {
        *value = eeUUE;
    }
    else if(strcmp(iLine, "mime") == 0)
    {
        *value = eeMIME;
    }
    else if(strcmp(iLine, "seat") == 0)
    {
        *value = eeSEAT;
    }
    else
    {
        prErr("Unknown email encoding parameter %s!", line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseEmailEncoding */

/*  options: <flType> <destFile> <dirHdrTpl> <dirEntryTpl> <dirFtrTpl> [<globHdrTpl>
   <globFtrTpl>] */
int parseFilelist(char * line, s_fidoconfig * config)
{
    char * lineTmp;
    s_filelist * curFl;
    char * flType = NULL;
    size_t numCopied;
    ps_filelist tmp;

    if(line == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    /*  add new template */
    config->filelistCount++;
    tmp = realloc(config->filelists, config->filelistCount * sizeof(s_filelist));
    if(tmp == NULL)
    {
        nfree(config->filelists);
        prErr("No memory!");
        return 1;
    }
    config->filelists = tmp;

    curFl             = &config->filelists[config->filelistCount - 1];
    memset(curFl, 0, sizeof(s_filelist));
    /*  parse type */
    numCopied = copyStringUntilSep(line, " ", &flType);

    if(!numCopied)
    {
        return 1;
    }

    strLower(flType);

    if(!strcmp(flType, "dir"))
    {
        curFl->flType = flDir;
    }
    else if(!strcmp(flType, "global"))
    {
        curFl->flType = flGlobal;
    }
    else if(!strcmp(flType, "dirlist"))
    {
        curFl->flType = flDirList;
    }
    else
    {
        prErr("Unknown filelist type %s!", flType);
        nfree(flType);
        return 2;
    }

    nfree(flType);
    /*  parse destFile */
    lineTmp = line + numCopied;

    if(*lineTmp)
    {
        lineTmp++;
    }

    numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->destFile));

    if(!numCopied)
    {
        return 1;
    }

    if((curFl->flType == flDir) || (curFl->flType == flGlobal))
    {
        /*  parse dirHdrTpl */
        lineTmp += numCopied;

        if(*lineTmp)
        {
            lineTmp++;
        }

        numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirHdrTpl));

        if(!numCopied)
        {
            return 1;
        }

        /*  parse dirEntryTpl */
        lineTmp += numCopied;

        if(*lineTmp)
        {
            lineTmp++;
        }

        numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirEntryTpl));

        if(!numCopied)
        {
            return 1;
        }

        /*  parse dirFtrTpl */
        lineTmp += numCopied;

        if(*lineTmp)
        {
            lineTmp++;
        }

        numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirFtrTpl));

        if(!numCopied)
        {
            return 1;
        }
    }

    switch(curFl->flType)
    {
        case flGlobal:
            /*  parse globHdrTpl */
            lineTmp += numCopied;

            if(*lineTmp)
            {
                lineTmp++;
            }

            numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->globHdrTpl));

            if(!numCopied)
            {
                return 1;
            }

            /*  parse globFtrTpl */
            lineTmp += numCopied;

            if(*lineTmp)
            {
                lineTmp++;
            }

            numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->globFtrTpl));

            if(!numCopied)
            {
                return 1;
            }

            break;

        case flDirList:
            /*  parse dirListHdrTpl */
            lineTmp += numCopied;

            if(*lineTmp)
            {
                lineTmp++;
            }

            numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListHdrTpl));

            if(!numCopied)
            {
                return 1;
            }

            /*  parse dirListEntryTpl */
            lineTmp += numCopied;

            if(*lineTmp)
            {
                lineTmp++;
            }

            numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListEntryTpl));

            if(!numCopied)
            {
                return 1;
            }

            /*  parse dirListFtrTpl */
            lineTmp += numCopied;

            if(*lineTmp)
            {
                lineTmp++;
            }

            numCopied = copyStringUntilSep(lineTmp, " ", &(curFl->dirListFtrTpl));

            if(!numCopied)
            {
                return 1;
            }

            break;

        case flDir:
        default:
            /*  just avoid a warning */
            break;
    } /* switch */
    return 0;
} /* parseFilelist */

int parseSyslog(char * line, int * value)
{
    int rv = 0;

#ifndef HAVE_SYSLOG
    prErr("%s: Syslogging is not supported on your platform!", actualKeyword);
    rv = 1;
    unused(line);
    unused(value);

#else
#   include <huskylib/syslogp.h>
    int i;

    if(line == NULL || line[0] == '\0')
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(isdigit(line[0]))
    {
        *value = atoi(line);
    }
    else
    {
        /* | if you get an error about undefined symbol "facilitynames"
         | add your operating system after "sun" to the ifdef line
         | in syslogp.h which comes below the
         | "unix systems that have syslog, but not facilitynames"
         | comment
           V          */
        for(i = 0; facilitynames[i].c_name != NULL; i++)
        {
            if(!strcmp(line, facilitynames[i].c_name))
            {
                *value = facilitynames[i].c_val;
                break;
            }
        }

        if(facilitynames[i].c_name == NULL)
        {
            prErr("%s: %s is an unknown syslog facility on this system.", actualKeyword, line);
            rv = 1;
        }
    }

#endif /* ifndef HAVE_SYSLOG */
    return rv;
} /* parseSyslog */

/* Parse additional option tokens like ReadOnly/WriteOnly */
int parsePermissions(char * line, s_permissions ** perm, unsigned int * permCount)
{
    char * ptr;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    *perm = srealloc(*perm, (*permCount + 1) * sizeof(s_permissions));

    if((ptr = strtok(line, " \t")) == NULL)
    {
        prErr("An AddressMask in %s is missing!", actualKeyword);
        return 1;
    }

    (*perm)[*permCount].addrMask = strdup(ptr);

    if((ptr = strtok(NULL, " \t")) == NULL)
    {
        prErr("An AreaMask in %s is missing!", actualKeyword);
        return 1;
    }

    (*perm)[*permCount].areaMask = strdup(ptr);
    (*permCount)++;

    if(strtok(NULL, " \t") != NULL)
    {
        prErr("Extra parameters in %s", actualLine);
        return 1;
    }

    return 0;
} /* parsePermissions */

int parseSeqOutrun(char * line, unsigned long * seqoutrun)
{
    char * p;

    if(line == NULL)
    {
        prErr("Parameters after %s are missing!", actualKeyword);
        return 1;
    }

    while(isspace(*line))
    {
        line++;
    }

    if(!isdigit(*line))
    {
        prErr("Bad SeqOutrun value %s", line);
        return 1;
    }

    *seqoutrun = (unsigned long)atol(line);
    p          = line;

    while(isdigit(*p))
    {
        p++;
    }

    if(*p == '\0')
    {
        return 0;
    }

    if(p[1])
    {
        prErr("Bad SeqOutrun value %s", line);
        return 1;
    }

    switch(tolower(*p))
    {
        case 'y':
            *seqoutrun *= 365;

        case 'd':
            *seqoutrun *= 24;

        case 'h':
            *seqoutrun *= 60 * 60;
            break;

        case 'w':
            *seqoutrun *= 7l * 24 * 60 * 60;
            break;

        case 'm':
            *seqoutrun *= 31l * 24 * 60 * 60;
            break;

        default:
            prErr("Bad SeqOutrun value %s", line);
            return 1;
    }
    return 0;
} /* parseSeqOutrun */

/* Parse the 'AvailList' token value
 */
int parseAvailList(char * line, eAvailList * availlist)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    iLine = strLower(sstrdup(line));

    if(stricmp(iLine, "full") == 0)
    {
        *availlist = AVAILLIST_FULL;
    }
    else if(stricmp(iLine, "unique") == 0)
    {
        *availlist = AVAILLIST_UNIQUE;
    }
    else if(stricmp(iLine, "uniqueone") == 0)
    {
        *availlist = AVAILLIST_UNIQUEONE;
    }
    else
    {
        prErr("Unknown AvailList value %s!", line);
        nfree(iLine);
        return 1;
    }

    nfree(iLine);
    return 0;
} /* parseAvailList */

/* parse group description */
int parseGroupDesc(s_fidoconfig * config, char * line)
{
    char * n, * d;
    register char * s = line;
    register short l;
    register unsigned short i;

    /* parse line */
    while(*s && (*s == ' ' || *s == '\t'))
    {
        s++;
    }

    if(*s == '\0')
    {
        prErr("Missing group name, line %d!", actualLineNr);
        return 1;
    }

    n = s;

    while(*s && (*s != ' ' && *s != '\t'))
    {
        s++;
    }

    if(*n == '"' && *(s - 1) == '"')
    {
        n++;
        *(s - 1) = '\0';
    }

    if(*s)
    {
        *s = '\0';
        s++;
    }

    while(*s && (*s == ' ' || *s == '\t'))
    {
        s++;
    }

    if(*s == '\0')
    {
        prErr("Missing group description, line %d!", actualLineNr);
        return 1;
    }

    l = (short)(strlen(s) - 1);

    while(l > 0 && (s[l] == ' ' || s[l] == '\t'))
    {
        l--;
    }

    if(l <= 0)
    {
        prErr("Missing group description, line %d!", actualLineNr);
        return 1;
    }

    s[l + 1] = '\0';

    /* create a new group record */
    for(i = 0; i <= config->groupCount; i++)
    {
        if(i == config->groupCount)
        {
            config->groupCount++;
            config->group         = srealloc(config->group, sizeof(s_group) * config->groupCount);
            config->group[i].name = sstrdup(n);
            break;
        }
        else if(strcmp(config->group[i].name, n) == 0)
        {
            nfree(config->group[i].desc);
            break;
        }
    }

    /* make group desc */
    if(*s != '"')
    {
        config->group[i].desc = sstrdup(s);
        return 0;
    }
    else
    {
        d = smalloc(l);
        s++;
        l = 0;

        for( ; ; )
        {
            if(*s == '"')
            {
                d[l++] = '\0';
                break;
            }

            if(*s != '\\' || *(s + 1) == '\0')
            {
                d[l++] = *s;
            }
            else
            {
                switch(*(++s))
                {
                    case '"':
                        d[l++] = '"';
                        break;

                    case 'n':
                        d[l++] = '\n';
                        break;

                    case 'r':
                        d[l++] = '\r';
                        break;

                    case 't':
                        d[l++] = '\t';
                        break;

                    default:
                        d[l++] = *s;
                }
            }

            if(*s == '\0')
            {
                break;
            }
            else
            {
                s++;
            }
        }
        config->group[i].desc = d;
    }

    return 0;
} /* parseGroupDesc */

/* parse SoftEchoList <none|name|group|group,name> */
int parseListEcho(char * line, e_listEchoMode * value)
{
    char * iLine;

    if(line == NULL)
    {
        prErr("A parameter after %s is missing!", actualKeyword);
        return 1;
    }

    if(*value != lemUndef)
    {
        prErr("%s redefinition", actualKeyword);
        return 2;
    }

    iLine = strLower(sstrdup(line));

    if(strcmp(iLine, "none") == 0)
    {
        *value = lemUnsorted;
    }
    else if(strcmp(iLine, "name") == 0)
    {
        *value = lemName;
    }
    else if(strcmp(iLine, "group") == 0)
    {
        *value = lemGroup;
    }
    else if(strcmp(iLine, "group,name") == 0)
    {
        *value = lemGroupName;
    }
    else
    {
        prErr("Unknown %s value %s!", actualKeyword, line);
        nfree(iLine);
        return 2;
    }

    nfree(iLine);
    return 0;
} /* parseListEcho */

/* Parse fidoconfig line
 * Return 0 if success.
 */
int parseLine(char * line, s_fidoconfig * config)
{
    char * token, * temp, * s;
    char * iToken;
    int rc = 0, id;
    s_link * clink = NULL;
    int link_robot = -1;
    static token_list_t tl;
    static token_list_t * ptl = NULL;

    temp = (char *)smalloc(strlen(line) + 1);
    strcpy(temp, line);
    actualLine = temp = vars_expand(temp);

    if(ptl == NULL)
    {
        ptl = &tl;
        make_token_list(ptl, parseline_tokens);
    }

    actualKeyword = token = strtok(temp, " \t");

    /* printf("Parsing: %s\n", line);
       printf("token: %s - %s\n", line, strtok(NULL, "\0")); */
    if(token)
    {
        iToken = strLower(sstrdup(token));
        id     = find_token(ptl, iToken);

        /* val: handle ^(area|file)fix.* */
        if(id == -1)
        {
            link_robot =
                (!strnicmp(iToken, "areafix", 7) << 0) | (!strnicmp(iToken, "filefix", 7) << 1);

            if(link_robot)
            {
                id = find_token(ptl, iToken + 7);
            }
        }

        switch(id)
        {
            case ID_VERSION:
                rc = parseVersion(getRestOfLine(), config);
                break;

            case ID_NAME:

                if(link_robot == 1 || link_robot == 2)
                {
                    clink = getDescrLink(config);

                    if(clink)
                    {
                        if(link_robot == 1)
                        {
                            fc_copyString(getRestOfLine(), &(clink->areafix.name));
                        }
                        else
                        {
                            fc_copyString(getRestOfLine(), &(clink->filefix.name));
                        }
                    }
                }
                else
                {
                    rc = fc_copyString(getRestOfLine(), &(config->name));
                }

                break;

            case ID_LOCATION:
                rc = fc_copyString(getRestOfLine(), &(config->location));
                break;

            case ID_SYSOP:
                rc = fc_copyString(getRestOfLine(), &(config->sysop));
                break;

            case ID_ADDRESS:
                rc = parseAddress(getRestOfLine(), config);
                break;

            case ID_INBOUND:
                rc = parsePath(getRestOfLine(), &(config->inbound), NULL);
                break;

            case ID_PROTINBOUND:
                rc = parsePath(getRestOfLine(), &(config->protInbound), NULL);
                break;

            case ID_LISTINBOUND:
                rc = parsePath(getRestOfLine(), &(config->listInbound), NULL);
                break;

            case ID_LOCALINBOUND:
                rc = parsePath(getRestOfLine(), &(config->localInbound), NULL);
                break;

            case ID_TEMPINBOUND:
                rc = parsePath(getRestOfLine(), &(config->tempInbound), NULL);
                break;

            case ID_OUTBOUND:
                rc = parsePath(getRestOfLine(), &(config->outbound), NULL);
                break;

            case ID_TICOUTBOUND:
                rc = parsePath(getRestOfLine(), &(config->ticOutbound), NULL);
                break;

            case ID_PUBLIC:
                rc = parsePublic(getRestOfLine(), config);
                break;

            case ID_LOGFILEDIR:
                rc = parsePath(getRestOfLine(), &(config->logFileDir), NULL);
                break;

            case ID_DUPEHISTORYDIR:
                rc = parsePath(getRestOfLine(), &(config->dupeHistoryDir), NULL);
                break;

            case ID_NODELISTDIR:
                rc = parsePath(getRestOfLine(), &(config->nodelistDir), NULL);
                break;

            case ID_FILEAREABASEDIR:
                rc = parseAreaPath(getRestOfLine(), &(config->fileAreaBaseDir), NULL);
                break;

            case ID_PASSFILEAREADIR:
                rc = parseAreaPath(getRestOfLine(), &(config->passFileAreaDir), NULL);
                break;

            case ID_BUSYFILEDIR:
                rc = parsePath(getRestOfLine(), &(config->busyFileDir), NULL);
                break;

            case ID_MSGBASEDIR:
                rc = parseAreaPathExpand(getRestOfLine(), &(config->msgBaseDir), NULL);
                break;

            case ID_LINKMSGBASEDIR:
                rc = parseAreaPathExpand(getRestOfLine(),
                                         &(getDescrLink(config)->areafix.baseDir),
                                         &(linkDefined.areafix.baseDir));
                break;

            case ID_LINKFILEBASEDIR:
                rc =
                    parseAreaPath(getRestOfLine(), &(getDescrLink(config)->filefix.baseDir),
                                  &(linkDefined.filefix.baseDir));
                break;

            case ID_MAGIC:
                rc = parsePath(getRestOfLine(), &(config->magic), NULL);
                break;

            case ID_SEMADIR:
                rc = parsePath(getRestOfLine(), &(config->semaDir), NULL);
                break;

            case ID_BADFILESDIR:
                rc = parsePath(getRestOfLine(), &(config->badFilesDir), NULL);
                break;

            case ID_NETMAILAREA:
            case ID_NETAREA:
                rc = parseNetMailArea(getRestOfLine(), config);
                break;

            case ID_DUPEAREA:
                rc = parseArea(config, getRestOfLine(), &(config->dupeArea), 1);
                break;

            case ID_BADAREA:
                rc = parseArea(config, getRestOfLine(), &(config->badArea), 1);
                break;

            case ID_ECHOAREADEFAULT:
                rc = parseAreaDefault(config, getRestOfLine(), &(config->EchoAreaDefault), 1);
                break;

            case ID_FILEAREADEFAULT:
                rc = parseAreaDefault(config, getRestOfLine(), &(config->FileAreaDefault), 1);
                break;

            case ID_ECHOAREA:
                rc = parseEchoArea(getRestOfLine(), config);
                break;

            case ID_FILEAREA:
                rc = parseFileArea(getRestOfLine(), config);
                break;

            case ID_BBSAREA:
                rc = parseBbsAreaStatement(getRestOfLine(), config);
                break;

            case ID_LOCALAREA:
                rc = parseLocalArea(getRestOfLine(), config);
                break;

            case ID_REMAP:
                rc = parseRemap(getRestOfLine(), config);
                break;

            case ID_LINK:
                rc = parseLink(getRestOfLine(), config);

                if(rc)
                {
                    exit(EX_CONFIG); /* 'cause of parsing aka and overriding prev. aka */
                }

                break;

            case ID_PASSWORD:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parsePWD(getRestOfLine(), &clink->defaultPwd);
                    /* this way used because of redefinition   */
                    /* defaultPwd from linkdefaults (if exist) */
                    clink->pktPwd      = clink->defaultPwd;
                    clink->ticPwd      = clink->defaultPwd;
                    clink->areafix.pwd = clink->defaultPwd;
                    clink->filefix.pwd = clink->defaultPwd;
                    clink->bbsPwd      = clink->defaultPwd;
                    clink->sessionPwd  = clink->defaultPwd;
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AKA:

                if((clink = getDescrLink(config)) != NULL)
                {
                    char * restOfLine = getRestOfLine();
                    if(parseFtnAddrZS(restOfLine, &clink->hisAka) & FTNADDR_ERROR)
                    {
                        prErr("Address %s is incorrect!", restOfLine);
                        rc = 1;
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_OURAKA:
                rc = 0;

                if((clink = getDescrLink(config)) != NULL)
                {
                    char * l = getRestOfLine();
                    clink->ourAka = getAddr(config, l);

                    if(clink->ourAka == NULL)
                    {
                        prErr("Address %s is not our aka!", l);
                        rc = 2;
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_PACKAKA:
                rc = 0;

                if((clink = getDescrLink(config)) != NULL)
                {
                    parseFtnAddrZS(getRestOfLine(), &clink->hisPackAka);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AUTOCREATE:

                if((clink = getDescrLink(config)) != NULL)
                {
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc |= parseBool(s, &clink->areafix.autoCreate);
                    }

                    if(link_robot & 2)
                    {
                        rc |= parseBool(s, &clink->filefix.autoCreate);
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_FILEFIXFSC87SUBSET:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->FileFixFSC87Subset);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_FORWARDREQUESTS:

                if((clink = getDescrLink(config)) != NULL)
                {
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc = parseBool(s, &clink->areafix.forwardRequests);
                    }

                    if(link_robot & 2)
                    {
                        rc = parseBool(s, &clink->filefix.forwardRequests);
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AUTOSUBSCRIBE:

                if((clink = getDescrLink(config)) != NULL)
                {
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc = parseBool(s, &clink->areafix.autoSubscribe);
                    }

                    if(link_robot & 2)
                    {
                        rc = parseBool(s, &clink->filefix.autoSubscribe);
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_DENYFWDREQACCESS:

                if((clink = getDescrLink(config)) != NULL)
                {
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc = parseBool(s, &clink->areafix.denyFRA);
                    }

                    if(link_robot & 2)
                    {
                        rc = parseBool(s, &clink->filefix.denyFRA);
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_FORWARDPKTS:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseForwardPkts(getRestOfLine(), clink);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_ALLOWEMPTYPKTPWD:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseAllowEmptyPktPwd(getRestOfLine(), config, clink);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_PACKNETMAIL:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->packNetmail);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_SENDNOTIFYMESSAGES:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->sendNotifyMessages);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_ALLOWREMOTECONTROL:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->allowRemoteControl);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_UNSUBSCRIBEONAREADELETE:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->unsubscribeOnAreaDelete);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_ALLOWPKTADDRDIFFER:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseAllowPktAddrDiffer(getRestOfLine(), config, clink);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AUTOCREATEDEFAULTS:
                s = getRestOfLine();

                if((clink = getDescrLink(config)) != NULL)
                {
                    if(link_robot & 1)
                    {
                        rc = fc_copyStringWOstrip(s, &clink->areafix.autoCreateDefaults);
                    }

                    if(link_robot & 2)
                    {
                        rc = fc_copyStringWOstrip(s, &clink->filefix.autoCreateDefaults);
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AREAFIX:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->areafix.on);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_FILEFIX:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->filefix.on);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_PAUSE:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parsePause(getRestOfLine(), &clink->Pause);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_NOTIC:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->noTIC);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_DELNOTRECEIVEDTIC:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->delNotReceivedTIC);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_ADVANCEDAREAFIX:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->advancedAreafix);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AUTOPAUSE:
                rc = parseAutoPause(getRestOfLine(), &(getDescrLink(config)->autoPause));
                break;

            case ID_FWDPRIORITY:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc = parseUInt(s, &(getDescrLink(config)->areafix.forwardPriority));
                }

                if(link_robot & 2)
                {
                    rc = parseUInt(s, &(getDescrLink(config)->filefix.forwardPriority));
                }

                break;

            case ID_FORWARDREQUESTTIMEOUT:
                checkRobot();
                rc = parseUInt(getRestOfLine(), &(curRobot->forwardRequestTimeout));
                break;

            case ID_IDLEPASSTHRUTIMEOUT:
                checkRobot();
                rc = parseUInt(getRestOfLine(), &(curRobot->idlePassthruTimeout));
                break;

            case ID_KILLEDREQUESTTIMEOUT:
                checkRobot();
                rc = parseUInt(getRestOfLine(), &(curRobot->killedRequestTimeout));
                break;

            case ID_DENYUNCONDFWDREQACCESS:

                if((clink = getDescrLink(config)) != NULL)
                {
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc |= parseBool(s, &(clink->areafix.denyUFRA));
                    }

                    if(link_robot & 2)
                    {
                        rc |= parseBool(s, &(clink->filefix.denyUFRA));
                    }
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_REDUCEDSEENBY:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->reducedSeenBy));
                break;

            case ID_EXPORT:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->aexport);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_IMPORT:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->import);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_MANDATORY:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->mandatory);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_MANUAL:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->manual);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_OPTGRP:
                rc = parseGroup(getRestOfLine(), config, 3);
                break;

            case ID_FWDMASK:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc = parseGroup(s, config, 4);
                }

                if(link_robot & 2)
                {
                    rc = parseGroup(s, config, 14);
                }

                break;

            case ID_FWDDENYMASK:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc = parseGroup(s, config, 5);
                }

                if(link_robot & 2)
                {
                    rc = parseGroup(s, config, 15);
                }

                break;

            case ID_LEVEL:
                rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->level));
                break;

            case ID_ECHOLIMIT:
            {
                s_link * link = getDescrLink(config);
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc |= parseNumber(s, 10, &(link->areafix.echoLimit));
                }

                if(link_robot & 2)
                {
                    rc |= parseNumber(s, 10, &(link->filefix.echoLimit));
                }

                break;
            }

            case ID_ARCMAILSIZE:
                rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->arcmailSize));
                break;

            case ID_DAILYBUNDLES:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->dailyBundles);
                }
                else
                {
                    rc = parseBool(getRestOfLine(), &config->dailyBundles);
                }

                break;

            case ID_PKTSIZE:
                rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->pktSize));
                break;

            case ID_MAXUNPACKEDNETMAIL:
                rc = parseNumber(getRestOfLine(), 10, &(getDescrLink(config)->maxUnpackedNetmail));
                break;

            case ID_PKTPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->pktPwd));
                break;

            case ID_TICPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->ticPwd));
                break;

            case ID_AREAFIXPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->areafix.pwd));
                break;

            case ID_FILEFIXPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->filefix.pwd));
                break;

            case ID_BBSPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->bbsPwd));
                break;

            case ID_SESSIONPWD:
                rc = parsePWD(getRestOfLine(), &(getDescrLink(config)->sessionPwd));
                break;

            case ID_HANDLE:
                rc = parseHandle(getRestOfLine(), config);
                break;

            case ID_EMAIL:

                if(config->linkCount) /* email of link */
                {
                    rc = fc_copyString(getRestOfLine(), &(getDescrLink(config)->email));
                }
                else /* email of self */
                {
                    rc = fc_copyString(getRestOfLine(), &config->email);
                }

                break;

            case ID_EMAILFROM:
                rc = fc_copyString(getRestOfLine(), &(getDescrLink(config)->emailFrom));
                break;

            case ID_EMAILSUBJ:
                rc = fc_copyString(getRestOfLine(), &(getDescrLink(config)->emailSubj));
                break;

            case ID_EMAILENCODING:
                rc = parseEmailEncoding(getRestOfLine(), &(getDescrLink(config)->emailEncoding));
                break;

            case ID_FLAVOUR:
                rc = parseFlavour(getRestOfLine(), &(getDescrLink(config)->netMailFlavour));

                if(rc == 0)
                {
                    getDescrLink(config)->echoMailFlavour = getDescrLink(config)->fileEchoFlavour =
                        getDescrLink(config)->netMailFlavour;
                }

                break;

            case ID_NETMAILFLAVOUR:
                rc = parseFlavour(getRestOfLine(), &(getDescrLink(config)->netMailFlavour));
                break;

            case ID_ECHOMAILFLAVOUR:
                rc = parseFlavour(getRestOfLine(), &(getDescrLink(config)->echoMailFlavour));
                break;

            case ID_FILEECHOFLAVOUR:
                rc = parseFlavour(getRestOfLine(), &(getDescrLink(config)->fileEchoFlavour));
                break;

            case ID_ROBOT:

                if(config->describeLinkDefaults || config->linkCount)
                {
                    prErr("Any robots should be described before any link or linkdefaults!");
                    rc = 1;
                }

                curRobot = getRobot(config, getRestOfLine(), 1);
                break;

            case ID_ROUTE:
                rc = parseRoute(getRestOfLine(),
                                config,
                                &(config->route),
                                &(config->routeCount),
                                id_route);
                break;

            case ID_ROUTEFILE:
                rc = parseRoute(getRestOfLine(),
                                config,
                                &(config->route),
                                &(config->routeCount),
                                id_routeFile);
                break;

            case ID_ROUTEMAIL:
                rc = parseRoute(getRestOfLine(),
                                config,
                                &(config->route),
                                &(config->routeCount),
                                id_routeMail);
                break;

            case ID_PACK:
                rc = parsePack(getRestOfLine(), config);
                break;

            case ID_UNPACK:
                rc = parseUnpack(getRestOfLine(), config);
                break;

            case ID_PACKER:
                rc = parsePackerDef(getRestOfLine(), config, &(getDescrLink(config)->packerDef));
                break;

            case ID_INTAB:
                rc = parseFileName(getRestOfLine(), &(config->intab), NULL);
                break;

            case ID_OUTTAB:
                rc = parseFileName(getRestOfLine(), &(config->outtab), NULL);
                break;

            case ID_RECODEMSGBASE:
                rc = parseBool(getRestOfLine(), &config->recodeMsgBase);
                break;

            case ID_HELPFILE:
                checkRobot();
                rc = parseFileName(getRestOfLine(), &(curRobot->helpFile), NULL);
                break;

            case ID_FWDFILE:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->areafix.fwdFile),
                                      &(linkDefined.areafix.fwdFile));
                }

                if(link_robot & 2)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->filefix.fwdFile),
                                      &(linkDefined.filefix.fwdFile));
                }

                break;

            case ID_FWDDENYFILE:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->areafix.denyFwdFile),
                                      &(linkDefined.areafix.denyFwdFile));
                }

                if(link_robot & 2)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->filefix.denyFwdFile),
                                      &(linkDefined.filefix.denyFwdFile));
                }

                break;

            case ID_AUTOCREATEFILE:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->areafix.autoCreateFile),
                                      &(linkDefined.areafix.autoCreateFile));
                }

                if(link_robot & 2)
                {
                    rc =
                        parseFileName(s, &(getDescrLink(config)->filefix.autoCreateFile),
                                      &(linkDefined.filefix.autoCreateFile));
                }

                break;

            case ID_LINKBUNDLENAMESTYLE:
                rc =
                    parseBundleNameStyle(getRestOfLine(),
                                         &(getDescrLink(config)->linkBundleNameStyle));
                break;

            case ID_ECHOTOSSLOG:
                rc = fc_copyString(getRestOfLine(), &(config->echotosslog));
                break;

            case ID_STATLOG:
                rc = fc_copyString(getRestOfLine(), &(config->statlog));
                break;

            case ID_IMPORTLOG:
                rc = fc_copyString(getRestOfLine(), &(config->importlog));
                break;

            case ID_LINKWITHIMPORTLOG:
                rc = parseLinkWithILogType(getRestOfLine(), &(config->LinkWithImportlog));
                break;

            case ID_KLUDGEAREANETMAIL:
                rc = parseKludgeAreaNetmailType(getRestOfLine(), &(config->kludgeAreaNetmail));
                break;

            /* not used
               case ID_FILEAREASLOG:
                rc = parseFileName(getRestOfLine(), &(config->fileAreasLog), NULL);
                break;
               case ID_FILENEWAREASLOG:
                rc = parseFileName(getRestOfLine(), &(config->fileNewAreasLog), NULL);
                break;
               case ID_LONGNAMELIST:
                rc = parseFileName(getRestOfLine(), &(config->longNameList), NULL);
                break;
               case ID_FILEARCLIST:
                rc = parseFileName(getRestOfLine(), &(config->fileArcList), NULL);
                break;
               case ID_FILEPASSLIST:
                rc = parseFileName(getRestOfLine(), &(config->filePassList), NULL);
                break;
               case ID_FILEDUPELIST:
                rc = parseFileName(getRestOfLine(), &(config->fileDupeList), NULL);
                break;
             */
            case ID_LOGLEVELS:
                rc = parseLoglevels(getRestOfLine(), &(config->loglevels));
                break;

            case ID_SCREENLOGLEVELS:
                rc = parseLoglevels(getRestOfLine(), &(config->screenloglevels));
                break;

            case ID_LOGDATEFORMAT:
                rc = fc_copyString(getRestOfLine(), &(config->logDateFormat));
                break;

            case ID_ACCESSGRP:
                rc = parseGroup(getRestOfLine(), config, 0);
                break;

            case ID_LINKGRP:
                rc = parseGroup(getRestOfLine(), config, 1);
                break;

            case ID_CARBONTO:
                rc = parseCarbon(getRestOfLine(), config, ct_to);
                break;

            case ID_CARBONFROM:
                rc = parseCarbon(getRestOfLine(), config, ct_from);
                break;

            case ID_CARBONADDR:
                rc = parseCarbon(getRestOfLine(), config, ct_addr);
                break;

            case ID_CARBONKLUDGE:
                rc = parseCarbon(getRestOfLine(), config, ct_kludge);
                break;

            case ID_CARBONSUBJ:
                rc = parseCarbon(getRestOfLine(), config, ct_subject);
                break;

            case ID_CARBONTEXT:
                rc = parseCarbon(getRestOfLine(), config, ct_msgtext);
                break;

            case ID_CARBONFROMAREA:
                rc = parseCarbon(getRestOfLine(), config, ct_fromarea);
                break;

            case ID_CARBONGROUPS:
                rc = parseCarbon(getRestOfLine(), config, ct_group);
                break;

            case ID_CARBONCOPY:
                rc = parseCarbonArea(getRestOfLine(), config, 0);
                break;

            case ID_CARBONMOVE:
                rc = parseCarbonArea(getRestOfLine(), config, 1);
                break;

            case ID_CARBONEXTERN:
            case ID_NETMAILEXTERN:
                rc = parseCarbonExtern(getRestOfLine(), config);
                break;

             case ID_CARBONDELETE:
                rc = parseCarbonDelete(getRestOfLine(), config);
                break;

            case ID_CARBONREASON:
                rc = parseCarbonReason(getRestOfLine(), config);
                break;

            case ID_CARBONRULE:
                rc = parseCarbonRule(getRestOfLine(), config);
                break;

            case ID_EXCLUDEPASSTHROUGHCARBON:
                rc = parseBool(getRestOfLine(), &(config->exclPassCC));
                break;

            case ID_LOCKFILE:
                rc = fc_copyString(getRestOfLine(), &(config->lockfile));
                break;

            case ID_TEMPOUTBOUND:
                rc = parsePath(getRestOfLine(), &(config->tempOutbound), NULL);
                break;

            case ID_AUTOAREAPAUSE:
                checkRobot();
                rc = parseBool(getRestOfLine(), &(curRobot->autoAreaPause));
                break;

            case ID_AREAFIXFROMPKT:
                rc = parseBool(getRestOfLine(), &(config->areafixFromPkt));
                break;

            case ID_QUEUEFILE:
                checkRobot();
                rc = parseFileName(getRestOfLine(), &(curRobot->queueFile), NULL);
                break;

            case ID_REPORTSATTR:

                if(config->describeLinkDefaults || config->linkCount)
                {
                    s_link * link = getDescrLink(config);
                    s = getRestOfLine();

                    if(link_robot & 1)
                    {
                        rc |=
                            parseAttr(s,
                                      &(link->areafix.reportsFlags),
                                      &(link->areafix.reportsAttr));
                    }

                    if(link_robot & 2)
                    {
                        rc |=
                            parseAttr(s,
                                      &(link->filefix.reportsFlags),
                                      &(link->filefix.reportsAttr));
                    }
                }
                else
                {
                    checkRobot();
                    rc =
                        parseAttr(getRestOfLine(), &(curRobot->reportsFlags),
                                  &(curRobot->reportsAttr));
                }

                break;

            case ID_KILLREQUESTS:
                checkRobot();
                rc = parseBool(getRestOfLine(), &(curRobot->killRequests));
                break;

            case ID_QUERYREPORTS:
                checkRobot();
                rc = parseBool(getRestOfLine(), &(curRobot->queryReports));
                break;

            case ID_CREATEDIRS:
                rc = parseBool(getRestOfLine(), &(config->createDirs));
                break;

            case ID_LONGDIRNAMES:
                rc = parseBool(getRestOfLine(), &(config->longDirNames));
                break;

            case ID_SPLITDIRS:
                rc = parseBool(getRestOfLine(), &(config->splitDirs));
                break;

            case ID_ADDDLC:
                rc = parseBool(getRestOfLine(), &(config->addDLC));
                break;

            case ID_FILESINGLEDESCLINE:
                rc = parseBool(getRestOfLine(), &(config->fileSingleDescLine));
                break;

            case ID_FILECHECKDEST:
                rc = parseBool(getRestOfLine(), &(config->fileCheckDest));
                break;

            case ID_PUBLICGROUP:
                rc = parseGroup(getRestOfLine(), config, 2);
                break;

            case ID_LOGECHOTOSCREEN:
                rc = parseBool(getRestOfLine(), &(config->logEchoToScreen));
                break;

            case ID_SEPARATEBUNDLES:
                rc = parseBool(getRestOfLine(), &(config->separateBundles));
                break;

            case ID_CARBONANDQUIT:
                rc = parseBool(getRestOfLine(), &(config->carbonAndQuit));
                break;

            case ID_CARBONKEEPSB:
                rc = parseBool(getRestOfLine(), &(config->carbonKeepSb));
                break;

            case ID_CARBONOUT:
                rc = parseBool(getRestOfLine(), &(config->carbonOut));
                break;

            case ID_IGNORECAPWORD:
                rc = parseBool(getRestOfLine(), &(config->ignoreCapWord));
                break;

            case ID_NOPROCESSBUNDLES:
                rc = parseBool(getRestOfLine(), &(config->noProcessBundles));
                break;

            case ID_NOTVALIDFILENAMECHARS:
                rc = fc_copyString(getRestOfLine(), &(config->notValidFNChars));
                break;

            case ID_REPORTTO:
                rc = fc_copyString(getRestOfLine(), &(config->ReportTo));
                break;

            case ID_REPORTREQUESTER:
                rc = parseBool(getRestOfLine(), (unsigned int *)&(config->reportRequester));
                break;

            case ID_EXECONFILE:
                rc = parseExecOnFile(getRestOfLine(), config);
                break;

            case ID_DEFARCMAILSIZE:
                rc = parseNumber(getRestOfLine(), 10, &(config->defarcmailSize));
                break;

            case ID_MSGSIZE:
                checkRobot();
                rc = parseNumber(getRestOfLine(), 10, &(curRobot->msgSize));
                break;

            case ID_AFTERUNPACK:
                rc = fc_copyString(getRestOfLine(), &(config->afterUnpack));
                break;

            case ID_BEFOREPACK:
                rc = fc_copyString(getRestOfLine(), &(config->beforePack));
                break;

            case ID_PROCESSPKT:
                rc = fc_copyString(getRestOfLine(), &(config->processPkt));
                break;

            case ID_SPLITSTR:
                checkRobot();
                rc = fc_copyString(getRestOfLine(), &(curRobot->splitStr));
                break;

            case ID_ROBOTORIGIN:
                checkRobot();
                temp = getRestOfLine();

                if(temp[0] == '"' && temp[strlen(temp) - 1] == '"')
                {
                    temp++;
                    temp[strlen(temp) - 1] = '\0';
                }

                rc = fc_copyString(temp, &(curRobot->origin));
                break;

            case ID_ROBOTSAREA:
                rc = fc_copyString(getRestOfLine(), &(config->robotsArea));
                break;

            case ID_FILEDESCNAME:
                rc =
                    parseUUEechoAreas(getRestOfLine(), &(config->fileDescNames),
                                      &(config->fDescNameCount));
                break;

            case ID_FILEDESCRIPTION:
                rc = fc_copyString(getRestOfLine(), &(config->fileDescription));
                break;

            case ID_FILEDESCPOS:
                rc = parseUInt(getRestOfLine(), &(config->fileDescPos));
                break;

            case ID_DLCDIGITS:
                rc = parseUInt(getRestOfLine(), &(config->DLCDigits));
                break;

            /* not used
               case ID_FILEMAXDUPEAGE:
                rc = parseUInt(getRestOfLine(), &(config->fileMaxDupeAge));
                break;
               case ID_FILEFILEUMASK:
                rc = parseOctal(getRestOfLine(), &(config->fileFileUMask));
                break;
               case ID_FILEDIRUMASK:
                rc = parseOctal(getRestOfLine(), &(config->fileDirUMask));
                break;
               case ID_FILELOCALPWD:
                rc = fc_copyString(getRestOfLine(), &(config->fileLocalPwd));
                break;

             */
            case ID_ORIGININANNOUNCE:
                rc = parseBool(getRestOfLine(), &(config->originInAnnounce));
                break;

            case ID_MAXTICLINELENGTH:
                rc = parseUInt(getRestOfLine(), &(config->MaxTicLineLength));
                break;

            case ID_FILELDESCSTRING:
                rc = fc_copyString(getRestOfLine(), &(config->fileLDescString));
                break;

            case ID_SAVETIC:
                rc = parseSaveTicStatement(getRestOfLine(), config);
                break;

            case ID_AREASMAXDUPEAGE:
                rc = parseNumber(getRestOfLine(), 10, &(config->areasMaxDupeAge));
                break;

            case ID_DUPEBASETYPE:
                rc =
                    parseTypeDupes(getRestOfLine(), &(config->typeDupeBase),
                                   &(config->areasMaxDupeAge));
                break;

            case ID_FIDOUSERLIST:
                rc = fc_copyString(getRestOfLine(), &(config->fidoUserList));
                break;

            case ID_NODELIST:
                rc = parseNodelist(getRestOfLine(), config);
                break;

            case ID_DIFFUPDATE:
                rc = 0;

                if(config->nodelistCount > 0)
                {
                    rc =
                        fc_copyString(
                            getRestOfLine(),
                            &(config->nodelists[config->nodelistCount -
                                                1].diffUpdateStem));
                }
                else
                {
                    printNodelistError();
                    rc = 1;
                }

                break;

            case ID_DELAPPLIEDDIFF:
                rc =
                    parseBool(getRestOfLine(),
                              (unsigned int *)&(config->nodelists[config->nodelistCount -
                                                                  1].delAppliedDiff));
                break;

            case ID_FULLUPDATE:
                rc = 0;

                if(config->nodelistCount > 0)
                {
                    rc =
                        fc_copyString(
                            getRestOfLine(),
                            &(config->nodelists[config->nodelistCount -
                                                1].fullUpdateStem));
                }
                else
                {
                    printNodelistError();
                    rc = 1;
                }

                break;

            case ID_DAILYNODELIST:
                rc =
                    parseBool(getRestOfLine(),
                              (unsigned int *)&(config->nodelists[config->nodelistCount -
                                                                  1].dailynodelist));
                break;

            case ID_DEFAULTZONE:
                rc = 0;

                if(config->nodelistCount > 0)
                {
                    rc =
                        parseUInt(getRestOfLine(),
                                  &(config->nodelists[config->nodelistCount - 1].defaultZone));
                }
                else
                {
                    printNodelistError();
                    rc = 1;
                }

                break;

            case ID_NODELISTFORMAT:

                if(config->nodelistCount > 0)
                {
                    rc =
                        parseNodelistFormat(getRestOfLine(), config,
                                            &(config->nodelists[config->nodelistCount - 1]));
                }
                else
                {
                    printNodelistError();
                    rc = 1;
                }

                break;

            case ID_LOGOWNER:
                rc = parseOwner(getRestOfLine(), &(config->loguid), &(config->loggid));
                break;

            case ID_LOGPERM:
                rc = parseNumber(getRestOfLine(), 8, &(config->logperm));
                break;

            case ID_LINKDEFAULTS:
                rc = parseLinkDefaults(getRestOfLine(), config);
                break;

            case ID_CREATEAREASCASE:
                rc = parseNamesCase(getRestOfLine(), &(config->createAreasCase));
                break;

            case ID_AREASFILENAMECASE:
                rc = parseNamesCase(getRestOfLine(), &(config->areasFileNameCase));
                break;

            case ID_CONVERTLONGNAMES:
                rc = parseNamesCaseConversion(getRestOfLine(), &(config->convertLongNames));
                break;

            case ID_CONVERTSHORTNAMES:
                rc = parseNamesCaseConversion(getRestOfLine(), &(config->convertShortNames));
                break;

            case ID_DISABLEKLUDGERESCANNED:
                rc = parseBool(getRestOfLine(), &(config->disableKludgeRescanned));
                break;

            case ID_DISABLETID:
                rc = parseBool(getRestOfLine(), &(config->disableTID));
                break;

            case ID_DISABLEPID:
                rc = parseBool(getRestOfLine(), &(config->disablePID));
                break;

            case ID_TOSSINGEXT:

                if((temp = getRestOfLine()) != NULL)
                {
                    rc = fc_copyString(temp, &(config->tossingExt));
                }
                else
                {
                    config->tossingExt = NULL;
                }

                break;

#if defined (__NT__)
            case ID_SETCONSOLETITLE:
                rc = parseBool(getRestOfLine(), &(config->setConsoleTitle));
                break;

#endif
            case ID_ADDTOSEEN:
                rc =
                    parseSeenBy2D(getRestOfLine(), &(config->addToSeen),
                                  &(config->addToSeenCount));
                break;

            case ID_IGNORESEEN:
                rc =
                    parseSeenBy2D(getRestOfLine(),
                                  &(config->ignoreSeen),
                                  &(config->ignoreSeenCount));
                break;

            case ID_TEARLINE:
                rc = fc_copyString(getRestOfLine(), &(config->tearline));
                break;

            case ID_ORIGIN:
                temp = getRestOfLine();

                if(temp)
                {
                    if(temp[0] == '"' && temp[strlen(temp) - 1] == '"')
                    {
                        temp++;
                        temp[strlen(temp) - 1] = '\0';
                    }
                }

                rc = fc_copyString(temp, &(config->origin));
                break;

            case ID_BUNDLENAMESTYLE:
                rc = parseBundleNameStyle(getRestOfLine(), &(config->bundleNameStyle));
                break;

            case ID_KEEPTRSMAIL:
                rc = parseBool(getRestOfLine(), &(config->keepTrsMail));
                break;

            case ID_KEEPTRSFILES:
                rc = parseBool(getRestOfLine(), &(config->keepTrsFiles));
                break;

            case ID_FILELIST:
                rc = parseFilelist(getRestOfLine(), config);
                break;

            case ID_CREATEFWDNONPASS:
                rc = parseBool(getRestOfLine(), &(config->createFwdNonPass));
                break;

            case ID_AUTOPASSIVE:
                rc = parseBool(getRestOfLine(), &(config->autoPassive));
                break;

            case ID_NETMAILFLAG:
                rc = fc_copyString(getRestOfLine(), &(config->netmailFlag));
                break;

            case ID_AUTOCREATEFLAG:
                checkRobot();
                rc = fc_copyString(getRestOfLine(), &(curRobot->autoCreateFlag));
                break;

            case ID_MINDISKFREESPACE:
                rc = parseNumber(getRestOfLine(), 10, &(config->minDiskFreeSpace));
                break;

            case ID_AUTOAREACREATESUBDIRS:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->autoAreaCreateSubdirs));
                break;

            case ID_AUTOFILECREATESUBDIRS:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->autoFileCreateSubdirs));
                break;

            case ID_TICKERPACKTOBOX:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->tickerPackToBox));
                break;

            case ID_ADVISORYLOCK:
                rc = parseUInt(getRestOfLine(), &(config->advisoryLock));
                break;

            case ID_NAMES:
                checkRobot();
                rc = parseStringList(getRestOfLine(), &(curRobot->names));
                break;

            case ID_REQIDXDIR:
                rc = parsePath(getRestOfLine(), &(config->reqidxDir), NULL);
                break;

            case ID_SYSLOG_FACILITY:
                rc = parseSyslog(getRestOfLine(), &(config->syslogFacility));
                break;

            case ID_FILEBOX:
                rc =
                    parsePathNoCheck(getRestOfLine(), &(getDescrLink(config)->fileBox),
                                     &(linkDefined.fileBox));
                break;

            case ID_FILEBOXESDIR:
                rc = parsePath(getRestOfLine(), &(config->fileBoxesDir), NULL);
                break;

            case ID_FILEBOXALWAYS:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->fileBoxAlways));
                break;

            case ID_CARBONEXCLUDEFWDFROM:
                rc = parseBool(getRestOfLine(), &(config->carbonExcludeFwdFrom));
                break;

            case ID_HPTPERLFILE:
                rc = parseFileName(getRestOfLine(), &(config->hptPerlFile), NULL);
                break;

            case ID_ADVSTATISTICSFILE:
                rc = fc_copyString(getRestOfLine(), &(config->advStatisticsFile));
                break;

            case ID_READONLY:
                rc = parsePermissions(getRestOfLine(),
                                      &(config->readOnly),
                                      &(config->readOnlyCount));
                break;

            case ID_WRITEONLY:
                rc =
                    parsePermissions(getRestOfLine(), &(config->writeOnly),
                                     &(config->writeOnlyCount));
                break;

            case ID_ARCNETMAIL:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseBool(getRestOfLine(), &clink->arcNetmail);
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_RULESDIR:
                checkRobot();
                rc = parsePath(getRestOfLine(), &(curRobot->rulesDir), NULL);
                break;

            case ID_NORULES:
                s = getRestOfLine();

                if(link_robot & 1)
                {
                    rc = parseBool(s, &(getDescrLink(config)->areafix.noRules));
                }

                if(link_robot & 2)
                {
                    rc = parseBool(s, &(getDescrLink(config)->filefix.noRules));
                }

                break;

            case ID_PACKNETMAILONSCAN:
                rc = parseBool(getRestOfLine(), &(config->packNetMailOnScan));
                break;

            case ID_UUEECHOGROUP:
                rc = parseUUEechoAreas(getRestOfLine(), &(config->uuEGrp), &(config->numuuEGrp));
                break;

            case ID_SENDMAILCMD:
                rc = parseSendMailCmd(getRestOfLine(), &(config->sendmailcmd));
                break;

            case ID_TEMPDIR:
                rc = parsePath(getRestOfLine(), &(config->tempDir), NULL);
                break;

            /*  htick announcer */
            case ID_ANNOUNCESPOOL:
                rc = parsePath(getRestOfLine(), &(config->announceSpool), NULL);
                break;

            case ID_ANNAREATAG:
                rc = parseAnnDef(getRestOfLine(), config);
                break;

            case ID_ANNINCLUDE:
                rc = parseGroup(getRestOfLine(), config, 6);
                break;

            case ID_ANNEXCLUDE:
                rc = parseGroup(getRestOfLine(), config, 7);
                break;

            case ID_ANNTO:
                rc = fc_copyString(getRestOfLine(), &(getDescrAnnDef(config)->annto));
                break;

            case ID_ANNFROM:
                rc = fc_copyString(getRestOfLine(), &(getDescrAnnDef(config)->annfrom));
                break;

            case ID_ANNSUBJ:
                rc = fc_copyString(getRestOfLine(), &(getDescrAnnDef(config)->annsubj));
                break;

            case ID_ANNORIGIN:
                rc = fc_copyString(getRestOfLine(), &(getDescrAnnDef(config)->annorigin));
                break;

            case ID_ANNMESSFLAGS:
                rc = fc_copyString(getRestOfLine(), &(getDescrAnnDef(config)->annmessflags));
                break;

            case ID_ANNFILEORIGIN:
                rc = parseBool(getRestOfLine(), &(getDescrAnnDef(config)->annforigin));
                break;

            case ID_ANNFILERFROM:
                rc = parseBool(getRestOfLine(), &(getDescrAnnDef(config)->annfrfrom));
                break;

            case ID_ANNADDRTO:
                rc = parseAnnDefAddres(getRestOfLine(), config, 1);
                break;

            case ID_ANNADDRFROM:
                rc = parseAnnDefAddres(getRestOfLine(), config, 2);
                break;

            case ID_FILEAREACREATEPERMS:
                rc = parseNumber(getRestOfLine(), 10, &(config->fileAreaCreatePerms));
                config->fileAreaCreatePerms = dec2oct(config->fileAreaCreatePerms);
                break;

            case ID_NEWAREAREFUSEFILE:
                checkRobot();
                rc = fc_copyString(getRestOfLine(), &(curRobot->newAreaRefuseFile));
                break;

            case ID_FROMNAME:
                checkRobot();
                rc = fc_copyString(getRestOfLine(), &(curRobot->fromName));
                break;

            case ID_SEQDIR:
                rc = parsePath(getRestOfLine(), &(config->seqDir), NULL);
                break;

            case ID_SEQOUTRUN:
                rc = parseSeqOutrun(getRestOfLine(), &(config->seqOutrun));
                break;

            case ID_AVAILLIST:

                if((clink = getDescrLink(config)) != NULL)
                {
                    rc = parseAvailList(getRestOfLine(), &(clink->availlist));
                }
                else
                {
                    rc = 1;
                }

                break;

            case ID_AREAGROUP:
                rc = parseAreaGroup(getRestOfLine());
                break;

            case ID_AREAGROUPDEFAULTS:
                rc = parseAreaGroupDefaults(config, getRestOfLine());
                break;

            case ID_GRPDESC:
                rc = parseGroupDesc(config, getRestOfLine());
                break;

            case ID_LISTECHO:
                rc = parseListEcho(getRestOfLine(), &(config->listEcho));
                break;

            case ID_CREATEADDUPLINK:
                rc = parseBool(getRestOfLine(), &(config->createAddUplink));
                break;

            case ID_DENYRESCAN:
                rc = parseBool(getRestOfLine(), &(getDescrLink(config)->denyRescan));
                break;

            case ID_RESCANGRP:
                rc = parseGroup(getRestOfLine(), config, 8);
                break;

            case ID_RESCANLIMIT:
                rc =
                    parseNumber(getRestOfLine(), 10,
                                (unsigned int *)&(getDescrLink(config)->rescanLimit));
                break;

            default:
                prErr("unrecognized: %s", line);
                wasError = 1;
                nfree(iToken);
                nfree(actualLine);
                return 1;
        } /* switch */
        nfree(iToken);
    }

    if(rc != 0)
    {
        prErr("error %d in: %s", rc, line);
        wasError = 1;
    }

    nfree(actualLine);
    return rc;
} /* parseLine */
