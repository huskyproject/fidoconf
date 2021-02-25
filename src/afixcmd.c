/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * afixcmd.c : common areafix commands
 *
 * Compiled from hpt/areafix hpt/toss hpt/pkt
 * by Max Chernogor <mihz@mail.ru>, 2:464/108@fidonet
 *
 * This file is part of FIDOCONFIG library (part of the Husky FIDOnet
 * software project)
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * FIDOCONFIG library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FIDOCONFIG library; see the file COPYING.  If not, write
 * to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA
 *
 * See also http://www.gnu.org
 *****************************************************************************
 * $Id$
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <huskylib/huskylib.h>

#ifdef HAS_UNISTD_H
#   include <unistd.h>
#endif

#ifdef HAS_IO_H
#   include <io.h>
#endif

#ifdef HAS_STRINGS_H
#   include <strings.h>
#endif
/* export functions from DLL */
#define DLLEXPORT
#include <huskylib/huskyext.h>

#include "afixcmd.h"
#include "common.h"

char * expandCfgLine(char * cfgline)
{
    cfgline = trimLine(cfgline);
    cfgline = shell_expand(cfgline);
    cfgline = vars_expand(cfgline);
    return cfgline;
}

int FindTokenPos4Link(char ** confName,
                      char * ftoken,
                      char * fftoken,
                      s_link * link,
                      long * start,
                      long * end)
{
    char * cfgline, * line, * token, * linkConfName;
    long linkstart = 0;

    *start = 0;
    *end   = 0;

    if(init_conf(*confName))
    {
        return 0;
    }

    while((cfgline = configline()) != NULL)
    {
        cfgline = expandCfgLine(cfgline);
        line    = cfgline;
        token   = strseparate(&line, " \t");

        if(!token || strcasecmp(token, "link"))
        {
            nfree(cfgline);
            continue;
        }

    linkliner: nfree(cfgline);

        for( ; ; )
        {
            if((cfgline = configline()) == NULL)
            {
                close_conf();
                return 0;
            }

            cfgline = expandCfgLine(cfgline);

            if(!*cfgline)
            {
                nfree(cfgline);
                continue;
            }

            line  = cfgline;
            token = strseparate(&line, " \t");

            if(!token)
            {
                nfree(cfgline);
                continue;
            }

            if(stricmp(token, "link") == 0)
            {
                goto linkliner;
            }

            if(stricmp(token, "aka") == 0)
            {
                break;
            }

            nfree(cfgline);
        }
        token = strseparate(&line, " \t");

        if(!token || testAddr(token, link->hisAka) == 0)
        {
            nfree(cfgline);
            continue;
        }

        nfree(cfgline);
        linkstart    = get_hcfgPos();
        linkConfName = sstrdup(getCurConfName());

        /* try to find alternative token
           ex.: areaFixPwd should be inserted after defaultPwd */
        if(fftoken)
        {
            for( ; ; )
            {
                if((cfgline = configline()) == NULL)
                {
/* !!! val: delete this if no bugs !!!
 * start = *end   = linkstart;
 * confName = linkConfName;
               close_conf();
               return 0;*/
                    fseek(get_hcfg(), linkstart, SEEK_SET);
                    break;
                }

                cfgline = expandCfgLine(cfgline);

                if(!*cfgline)
                {
                    nfree(cfgline);
                    continue;
                }

                line  = cfgline;
                token = strseparate(&line, " \t");

                if(token && stricmp(token, "link") == 0)
                {
/* !!! val: delete this if no bugs !!!
 * start = *end   = linkstart;
 * confName = linkConfName;
               close_conf();
               return 0;*/
                    fseek(get_hcfg(), linkstart, SEEK_SET);
                    break;
                }

                if(token && stricmp(token, fftoken) == 0)
                {
                    break;
                }

                nfree(cfgline);
            }
            nfree(cfgline);
            linkstart    = get_hcfgPos();
            linkConfName = sstrdup(getCurConfName());
        }

        for( ; ; )
        {
            if((cfgline = configline()) == NULL)
            {
                *start    = *end = linkstart;
                *confName = linkConfName;
                close_conf();
                return 0;
            }

            cfgline = expandCfgLine(cfgline);

            if(!*cfgline)
            {
                nfree(cfgline);
                continue;
            }

            line  = cfgline;
            token = strseparate(&line, " \t");

            if(token && stricmp(token, "link") == 0)
            {
                *start    = *end = linkstart;
                *confName = linkConfName;
                close_conf();
                return 0;
            }

            if(token && stricmp(token, ftoken) == 0)
            {
                break;
            }

            nfree(cfgline);
        }
        /*  remove line */
        nfree(cfgline);
        *start    = getCurConfPos();
        *end      = get_hcfgPos();
        *confName = sstrdup(getCurConfName());
        close_conf();
        return 1;
    }
    return 0;
} /* FindTokenPos4Link */

int InsertCfgLine(char * confName, char * cfgLine, long strbeg, long strend)
{
    char * line = NULL, * newname = NULL, * p;
    FILE * f_conf, * f_newconf;
    long endpos, curpos, cfglen;
    int openro = 0;

    f_conf = f_newconf = NULL;

    if((strbeg == 0 && strend == 0) || confName == NULL)
    {
        return 0;
    }

    if((f_conf = fopen(confName, "r+b")) == NULL)
    {
        if((f_conf = fopen(confName, "rb")) == NULL)
        {
            w_log(LL_ERR, "Cannot open config file %s: %s\n", confName, strerror(errno));
            return 0;
        }

        openro = 1;
    }

    if(fseek(f_conf, 0L, SEEK_END) != 0)
    {
        w_log(LL_ERR, "Cannot seek config file %s: %s\n", confName, strerror(errno));
        fclose(f_conf);
        return 0;
    }

    endpos  = ftell(f_conf);
    curpos  = strend;
    cfglen  = endpos - curpos;
    newname = (char *)smalloc(strlen(confName) + 5);
    strcpy(newname, confName);
    p = strrchr(newname, '.');

    if(p == NULL || strchr(p, PATH_DELIM))
    {
        strcat(newname, ".tmp");
    }
    else
    {
        strcpy(p, ".tmp");
    }

    if((f_newconf = fopen(newname, "wb")) == NULL)
    {
        /* we have no write access to this directory? */
        /* change config "in place" */
        if(openro)
        {
            w_log(LL_ERR, "Cannot open temp file %s: %s\n", newname, strerror(errno));
            nfree(newname);
            fclose(f_conf);
            return 0;
        }

        nfree(newname);
        line = (char *)smalloc((size_t)cfglen);
        fseek(f_conf, curpos, SEEK_SET);

        if(fread(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen)
        {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
            nfree(line);
            fclose(f_conf);
            return 0;
        }

        fseek(f_conf, strbeg, SEEK_SET);
        setfsize(fileno(f_conf), strbeg);

        if(cfgLine)  /*  line not deleted */
        {
            if(fprintf(f_conf, "%s%s", cfgLine,
                       cfgEol()) != (int)(strlen(cfgLine) + strlen(cfgEol())))
            {
                w_log(LL_ERR, "Cannot write config file %s: %s\n", confName, strerror(errno));
            }
        }

        if(fwrite(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen || fflush(f_conf) != 0)
        {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", confName, strerror(errno));
        }

        fclose(f_conf);
        nfree(line);
    }
    else
    {
        /* make new config-file and rename it */
#ifdef __UNIX__
        struct stat st;

        if(fstat(fileno(f_conf), &st) == 0)
        {
            fchmod(fileno(f_newconf), (st.st_mode & 01777) | 0400);
        }

#endif
        line = (char *)smalloc(cfglen > strbeg ? cfglen : strbeg);
        fseek(f_conf, 0L, SEEK_SET);

        if(fread(line, sizeof(char), strbeg, f_conf) < (size_t)strbeg)
        {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
        errwriteconf: fclose(f_conf);
            fclose(f_newconf);
            unlink(newname);
            nfree(line);
            nfree(newname);
            return 0;
        }

        if(fwrite(line, sizeof(char), strbeg, f_newconf) < (size_t)strbeg)
        {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
            goto errwriteconf;
        }

        if(cfgLine)
        {
            if(fprintf(f_newconf, "%s%s", cfgLine,
                       cfgEol()) != (int)(strlen(cfgLine) + strlen(cfgEol())))
            {
                w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
                goto errwriteconf;
            }
        }

        fseek(f_conf, curpos, SEEK_SET);

        if(fread(line, sizeof(char), cfglen, f_conf) != (size_t)cfglen)
        {
            w_log(LL_ERR, "Cannot read config file %s: %s\n", confName, strerror(errno));
            goto errwriteconf;
        }

        if(fwrite(line, sizeof(char), cfglen,
                  f_newconf) != (size_t)cfglen || fflush(f_newconf) != 0)
        {
            w_log(LL_ERR, "Cannot write config file %s: %s\n", newname, strerror(errno));
            goto errwriteconf;
        }

        fclose(f_newconf);
        fclose(f_conf);
        nfree(line);

        /* save old config as *.bak? */
/*
 #ifndef __UNIX__
        unlink(confName);
 #endif
 */
/*        if (rename(newname, confName)) { */
        if(move_file(newname, confName, 1))
        {
            w_log(LL_ERR, "Cannot rename config file %s->%s: %s\n", newname, confName,
                  strerror(errno));
            nfree(newname);
            return 0;
        }

        nfree(newname);
    }

    return 1;
} /* InsertCfgLine */

/*  opt = 0 - AreaFix */
/*  opt = 1 - AutoPause */
int Changepause(char * confName, s_link * link, int opt, int type)
{
    long strbeg = 0;
    long strend = 0;

    FindTokenPos4Link(&confName, "pause", NULL, link, &strbeg, &strend);
    link->Pause ^= type;

    if(link->Pause == NOPAUSE)
    {
        if(InsertCfgLine(confName, "Pause off", strbeg, strend))
        {
            w_log('8', "areafix: system %s set active", aka2str(link->hisAka));
        }
    }
    else if(link->Pause == (ECHOAREA | FILEAREA))
    {
        if(InsertCfgLine(confName, "Pause on", strbeg, strend))
        {
            w_log('8', "%s: system %s set passive", opt ? "autopause" : "areafix",
                  aka2str(link->hisAka));
        }
    }
    else if(link->Pause == ECHOAREA)
    {
        if(InsertCfgLine(confName, "Pause Earea", strbeg, strend))
        {
            w_log('8',
                  "%s: system %s set passive only for echoes",
                  opt ? "autopause" : "areafix",
                  aka2str(link->hisAka));
        }
    }
    else
    {
        if(InsertCfgLine(confName, "Pause Farea", strbeg, strend))
        {
            w_log('8',
                  "%s: system %s set passive only for file echoes",
                  opt ? "autopause" : "areafix",
                  aka2str(link->hisAka));
        }
    }

    nfree(confName);
    return 1;
} /* Changepause */

int testAddr(char * addr, hs_addr hisAka)
{
    hs_addr aka =
    {
        0
    };

    parseFtnAddrZS(addr, &aka);

    if(addrComp(aka, hisAka) == 0)
    {
        return 1;
    }

    return 0;
}

int DelLinkFromString(char * line, hs_addr linkAddr)
{
    int rc     = 1;
    char * end = NULL;
    char * beg = NULL;

    w_log(LL_FUNC, "::DelLinkFromString() begin");
    beg = strrchr(line, '"'); /* seek end comment pointer (quote char) */

    if(!beg)
    {
        beg = line;           /* if not found then seek from begin */
    }

    beg++;                    /* process next token */

    while(*beg)               /* while not end of string */
    {
        while(*beg && isspace(*beg))
        {
            beg++;                          /* skip spaces */
        }

        if(*beg && isdigit(*beg) && testAddr(beg, linkAddr))
        {
            rc = 0;
            break;
        }

        while(*beg && !isspace(*beg))
        {
            beg++;                           /* skip token */
        }
    }

    if(rc == 0) /* beg points to the beginning of an unsubscribed address */
    {
        end = beg;

        while(*beg && !isspace(*beg))
        {
            beg++;                           /* skip token */
        }

        while(*beg && !isdigit(*beg))
        {
            beg++;                           /* search for the next link */
        }

        if(*beg)
        {
            memmove(end, beg, strlen(beg) + 1);
        }
        else
        {
            end--;
            *end = '\0';
        }
    }

    w_log(LL_FUNC, "%::DelLinkFromString() end");
    return rc;
} /* DelLinkFromString */

int IsAreaAvailable(char * areaName, char * fileName, char ** desc, int retd)
{
    FILE * f;
    char * line, * token, * running;

    if(fileName == NULL || areaName == NULL)
    {
        return 0;
    }

    if((f = fopen(fileName, "r")) == NULL)
    {
        w_log('8', "Allfix: cannot open file \"%s\"", fileName);
        return 0;
    }

    while((line = readLine(f)) != NULL)
    {
        line = trimLine(line);

        if(line[0] != '\0')
        {
            running = line;
            token   = strseparate(&running, " \t\r\n");

            if(token && areaName && stricmp(token, areaName) == 0)
            {
                /*  return description if needed */
                if(retd)
                {
                    *desc = NULL;

                    if(running)
                    {
                        /* strip "" at the beginning & end */
                        if(running[0] == '"' && running[strlen(running) - 1] == '"')
                        {
                            running++;
                            running[strlen(running) - 1] = '\0';
                        }

                        /* change " -> ' */
                        token = running;

                        while(*token != '\0')
                        {
                            if(*token == '"')
                            {
                                *token = '\'';
                            }

                            token++;
                        }
                        xstrcat(&(*desc), running);
                    }
                }

                nfree(line);
                fclose(f);
                return 1;
            }
        }

        nfree(line);
    }
    /*  not found */
    fclose(f);
    return 0;
} /* IsAreaAvailable */

void Addlink(s_fidoconfig * config, s_link * link, s_area * area)
{
    char * ExclMask;
    UINT i;

    if(area)
    {
        s_arealink * arealink;
        area->downlinks = srealloc(area->downlinks,
                                   sizeof(s_arealink *) * (area->downlinkCount + 1));
        arealink = area->downlinks[area->downlinkCount] =
            (s_arealink *)scalloc(1, sizeof(s_arealink));
        arealink->link = link;
        area->downlinkCount++;
        setLinkAccess(config, area, arealink);

        if(config->readOnlyCount)
        {
            for(i = 0; i < config->readOnlyCount; i++)
            {
                if(config->readOnly[i].areaMask[0] != '!')
                {
                    if(patimat(area->areaName,
                               config->readOnly[i].areaMask) &&
                       patmat(aka2str(link->hisAka), config->readOnly[i].addrMask))
                    {
                        arealink->import = 0;
                    }
                }
                else
                {
                    ExclMask = config->readOnly[i].areaMask;
                    ExclMask++;

                    if(patimat(area->areaName,
                               ExclMask) &&
                       patmat(aka2str(link->hisAka), config->readOnly[i].addrMask))
                    {
                        arealink->import = 1;
                    }
                }
            }
        }

        if(config->writeOnlyCount)
        {
            for(i = 0; i < config->writeOnlyCount; i++)
            {
                if(config->writeOnly[i].areaMask[0] != '!')
                {
                    if(patimat(area->areaName,
                               config->writeOnly[i].areaMask) &&
                       patmat(aka2str(link->hisAka), config->writeOnly[i].addrMask))
                    {
                        arealink->aexport = 0;
                    }
                }
                else
                {
                    ExclMask = config->writeOnly[i].areaMask;
                    ExclMask++;

                    if(patimat(area->areaName,
                               ExclMask) &&
                       patmat(aka2str(link->hisAka), config->writeOnly[i].addrMask))
                    {
                        arealink->aexport = 1;
                    }
                }
            }
        }
    }
} /* Addlink */

void RemoveLink(s_link * link, s_area * area)
{
    if(area) /* remove link from echoarea */
    {
        int i = isAreaLink(link->hisAka, area);

        if(i != -1)
        {
            nfree(area->downlinks[i]);
            area->downlinks[i] = area->downlinks[area->downlinkCount - 1];
            area->downlinkCount--;
        }
    }
}

/* ---------------- areafix checking stuff --------------*/
/* test area-link pair to mandatory */
int mandatoryCheck(s_area * area, s_link * link)
{
    int i;

    w_log(LL_FUNC, __FILE__ "::mandatoryCheck()");

    if(grpInArray(area->group, link->optGrp, link->numOptGrp) && link->mandatory)
    {
        w_log(LL_FUNC, __FILE__ "::mandatoryCheck() rc=1");
        return 1;
    }

    if(link->numOptGrp == 0 && link->mandatory)
    {
        w_log(LL_FUNC, __FILE__ "::mandatoryCheck() rc=1");
        return 1;
    }

    if(area->mandatory)
    {
        w_log(LL_FUNC, __FILE__ "::mandatoryCheck() rc=1");
        return 1;
    }

    if((i = isAreaLink(link->hisAka, area)) != -1)
    {
        w_log(LL_FUNC, __FILE__ "::mandatoryCheck() rc=%d", area->downlinks[i]->mandatory);
        return area->downlinks[i]->mandatory;
    }

    w_log(LL_FUNC, __FILE__ "::mandatoryCheck() rc=0");
    return 0;
} /* mandatoryCheck */

/* test area-link pair to manual */
int manualCheck(s_area * area, s_link * link)
{
    int i;

    w_log(LL_FUNC, __FILE__ "::manualCheck()");

    if(grpInArray(area->group, link->optGrp, link->numOptGrp) && link->manual)
    {
        w_log(LL_FUNC, __FILE__ "::manualCheck() rc=1");
        return 1;
    }

    if(link->numOptGrp == 0 && link->manual)
    {
        w_log(LL_FUNC, __FILE__ "::manualCheck() rc=1");
        return 1;
    }

    if(area->manual)
    {
        w_log(LL_FUNC, __FILE__ "::manualCheck() rc=1");
        return 1;
    }

    if((i = isAreaLink(link->hisAka, area)) != -1)
    {
        w_log(LL_FUNC, __FILE__ "::manualCheck() rc=%d", area->downlinks[i]->manual);
        return area->downlinks[i]->manual;
    }

    w_log(LL_FUNC, __FILE__ "::manualCheck() rc=0");
    return 0;
} /* manualCheck */

int subscribeCheck(s_area * area, s_link * link)
{
    int found             = 0;
    s_fidoconfig * config = theApp.config;

    w_log(LL_FUNC, "%s::subscribeCheck() begin", __FILE__);

    if(isLinkOfArea(link, area))
    {
        return 0;
    }

    if(area->group)
    {
        if(config->numPublicGroup)
        {
            found = grpInArray(area->group, config->PublicGroup, config->numPublicGroup);
        }

        if(!found && link->numAccessGrp)
        {
            found = grpInArray(area->group, link->AccessGrp, link->numAccessGrp);
        }
    }
    else
    {
        found = 1;
    }

    if((area->levelwrite > link->level) && (area->levelread > link->level))
    {
        found = 0;
    }

    if(!found)
    {
        w_log(LL_FUNC, "%s::subscribeCheck() end, rc=2", __FILE__);
        return 2;
    }

    if(area->hide)
    {
        return 3;
    }

    w_log(LL_FUNC, "%s::subscribeCheck() end, rc=1", __FILE__);
    return 1;
} /* subscribeCheck */

int subscribeAreaCheck(s_area * area, char * areaname, s_link * link)
{
    int rc = 4;

    w_log(LL_SRCLINE, "%s::subscribeAreaCheck()", __FILE__);

    if((!areaname) || (!areaname[0]))
    {
        w_log(LL_SRCLINE, "%s::subscribeAreaCheck() Failed (areaname empty) rc=%d", __FILE__, rc);
        return rc;
    }

    if(patimat(area->areaName, areaname) == 1)
    {
        rc = subscribeCheck(area, link);

        /*  0 - already subscribed / linked */
        /*  1 - need subscribe / not linked */
        /*  2 - no access */
        /*  3 - area is hidden */
    }

    /*  else: this is another area */
    w_log(LL_SRCLINE, "%s::subscribeAreaCheck() end rc=%d", __FILE__, rc);
    return rc;
}

/* test link for areas quantity limit exceed
 * return 0 if not limit exceed
 * else return not zero
 */
int limitCheck(s_link * link)
{
    register unsigned int i, n;
    unsigned echoLimit = 0;
    unsigned areaCount = 0;
    ps_area areas      = NULL;

    if(theApp.module == M_HPT)
    {
        echoLimit = link->areafix.echoLimit;
        areaCount = theApp.config->echoAreaCount;
        areas     = theApp.config->echoAreas;
    }
    else if(theApp.module == M_HTICK)
    {
        echoLimit = link->filefix.echoLimit;
        areaCount = theApp.config->fileAreaCount;
        areas     = theApp.config->fileAreas;
    }

    w_log(LL_FUNC, __FILE__ "::limitCheck()");

    if(echoLimit == 0)
    {
        return 0;
    }

    for(i = n = 0; i < areaCount; i++)
    {
        if(isLinkOfArea(link, &(areas[i])))
        {
            n++;
        }
    }
    i = n >= echoLimit;
    w_log(LL_FUNC, __FILE__ "::limitCheck() rc=%u", i);
    return i;
} /* limitCheck */
