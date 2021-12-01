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
 * or download licence from http://www.gnu.org.
 *****************************************************************************
 *   $Id$
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

#include <huskylib/huskylib.h>

#ifdef HAS_STRINGS_H
# include <strings.h>
#endif /* HAS_STRINGS_H */

#define DLLIMPORT
#include <huskylib/huskyext.h>
#include <huskylib/strext.h>
#include <smapi/msgapi.h>


#ifdef HAS_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAS_IO_H
# include <io.h>
#endif

#include "version.h"
#include "../cvsdate.h"
#include "fidoconf.h"
#include "common.h"
#include "grptree.h"

#ifndef VERSION_H
# define VERSION_H

#endif

static s_fidoconfig * config;

/*******************************************************/
/* Every config error increases tparser exit code by 1 */
/* Warnings do not influence the exit code             */
/*******************************************************/
/* Test for required tokens */

static int testConfig(s_fidoconfig * tconfig)
{
    int rc = 0;

    printf("\n");

    if(!tconfig->tempDir)
    {
        printf("Warning:  TempDir not defined!\n");
    }

    if(!tconfig->protInbound)
    {
        printf("Warning:  ProtInbound not defined!\n");
    }

    if(!tconfig->inbound)
    {
        printf("Warning:  Inbound not defined!\n");
    }

    if(!tconfig->localInbound)
    {
        printf(
            "Warning:  localInbound not defined. The statement \"localInbound\" is not required but recommended.\n");
    }

    if(!tconfig->tempInbound)
    {
        printf("Warning:  TempInbound not defined!\n");
    }

    if(!tconfig->outbound)
    {
        printf("Warning:  Outbound not defined!\n");
    }

    if(!tconfig->tempOutbound)
    {
        printf("Warning:  TempOutbound not defined!\n");
    }

    if(!tconfig->nodelistDir)
    {
        printf("Warning:  NodelistDir not defined!\n");
    }

    if(tconfig->netMailAreaCount == 0)
    {
        printf("You must define at least one NetmailArea!\n");
        rc = 1;
    }

    if(tconfig->dupeArea.areaName == NULL)
    {
        printf("You must define DupeArea!\n");
        rc = 1;
    }
    else if(tconfig->dupeArea.fileName == NULL)
    {
        printf("DupeArea cannot be passthrough!\n");
        rc = 1;
    }

    if(tconfig->badArea.areaName == NULL)
    {
        printf("You must define BadArea!\n");
        rc = 1;
    }
    else if(tconfig->badArea.fileName == NULL)
    {
        printf("BadArea cannot be passthrough!\n");
        rc = 1;
    }

    if(rc)
    {
        putchar('\n');
    }

    return rc;
} /* testConfig */

/* check to path accuracy */
static int testpath(const char * s, const char * t1, const char * t2, const char * t3)
{
    unsigned c;
    int rc = 0;
    /* list of invalid substrings */
    static char * invalids[] =
    {
        "/../", "\\..\\", ">", "<", "|", NULL
    };
    /* list of reasons corresponding to the previous list */
    static char * reasons[] =
    {
        "relative path",                                        "relative path",
        "'redirect to' symbol is not allowed in a path name",
        "'redirect from' symbol is not allowed in a path name",
        "pipe symbol is not allowed in a path name",            NULL
    };

    if(s)
    {
        for(c = 0; invalids[c]; c++)
        {
            if(strstr(s, invalids[c]))
            {
                printf("ERROR: mistake in %s%s%s%s%s value: %s\n",
                       (t1 ? t1 : ""),
                       (t2 ? " " : ""),
                       (t2 ? t2 : ""),
                       (t3 ? " " : ""),
                       (t3 ? t3 : ""),
                       reasons[c]);
                rc++;
            }
        }

        if(!(strncmp(s, "./",
                     2) && strncmp(s, "../", 3) && strncmp(s, ".\\", 2) && strncmp(s, "..\\", 3)))
        {
            printf("WARNING: mistake in %s%s%s%s%s value: relative path\n", (t1 ? t1 : ""),
                   (t2 ? " " : ""), (t2 ? t2 : ""), (t3 ? " " : ""), (t3 ? t3 : ""));
        }
    }

    return rc;
} /* testpath */

/* check for correctness of a plain file name */
static int testplainfile(const char * s, const char * t1, const char * t2, const char * t3)
{
    static char invalidc[] = ":/\\><|";
    register char * p;

    if(s && (p = strpbrk(s, invalidc)) != NULL)
    {
        printf("ERROR: %s%s%s%s%s can't contain %c\n",
               t1,
               (t2 ? " " : ""),
               (t2 ? t2 : ""),
               (t3 ? " " : ""),
               (t3 ? t3 : ""),
               *p);
        return -1;
    }

    return 0;
}

/* check for correctness of file names and paths. Return zero if success. */
static int testPathsAndFiles(void)
{
    int rc = 0;
    register unsigned int i;

    rc += testpath(config->inbound, "inbound", NULL, NULL);
    rc += testpath(config->outbound, "outbound", NULL, NULL);
    rc += testpath(config->protInbound, "protInbound", NULL, NULL);
    rc += testpath(config->listInbound, "listInbound", NULL, NULL);
    rc += testpath(config->localInbound, "localInbound", NULL, NULL);
    rc += testpath(config->tempInbound, "tempInbound", NULL, NULL);
    rc += testpath(config->logFileDir, "logFileDir", NULL, NULL);
    rc += testpath(config->dupeHistoryDir, "dupeHistoryDir", NULL, NULL);
    rc += testpath(config->nodelistDir, "nodelistDir", NULL, NULL);
    rc += testpath(config->msgBaseDir, "msgBaseDir", NULL, NULL);
    rc += testpath(config->magic, "magic", NULL, NULL);
    rc += testpath(config->tempOutbound, "tempOutbound", NULL, NULL);
    rc += testpath(config->ticOutbound, "ticOutbound", NULL, NULL);
    rc += testpath(config->tempDir, "tempDir", NULL, NULL);
    rc += testpath(config->fileAreaBaseDir, "fileAreaBaseDir", NULL, NULL);
    rc += testpath(config->passFileAreaDir, "passFileAreaDir", NULL, NULL);
    rc += testpath(config->busyFileDir, "busyFileDir", NULL, NULL);
    rc += testpath(config->semaDir, "semaDir", NULL, NULL);
    rc += testpath(config->badFilesDir, "badFilesDir", NULL, NULL);
    rc += testpath(config->hptPerlFile, "hptPerlFile", NULL, NULL);
    rc += testpath(config->advStatisticsFile, "advStatisticsFile", NULL, NULL);
    rc += testpath(config->intab, "intab", NULL, NULL);
    rc += testpath(config->outtab, "outtab", NULL, NULL);
    rc += testpath(config->echotosslog, "echotosslog", NULL, NULL);
    rc += testpath(config->statlog, "statlog", NULL, NULL);
    rc += testpath(config->importlog, "importlog", NULL, NULL);
    rc += testpath(config->lockfile, "lockfile", NULL, NULL);
    rc += testpath(config->fileAreasLog, "fileAreasLog", NULL, NULL);
    rc += testpath(config->fileNewAreasLog, "fileNewAreasLog", NULL, NULL);
    rc += testpath(config->longNameList, "longNameList", NULL, NULL);
    rc += testpath(config->fileArcList, "fileArcList", NULL, NULL);
    rc += testpath(config->filePassList, "filePassList", NULL, NULL);
    rc += testpath(config->fileDupeList, "fileDupeList", NULL, NULL);
    rc += testpath(config->netmailFlag, "netmailFlag", NULL, NULL);
    rc += testpath(config->reqidxDir, "reqidxDir", NULL, NULL);
    rc += testpath(config->seqDir, "seqDir", NULL, NULL);
    rc += testplainfile(config->fidoUserList, "fidoUserList", NULL, NULL);

    /* extension = file name suffix test as file name */
    rc += testplainfile(config->tossingExt, "tossingExt", NULL, NULL);
    /* checks area paths */
    rc += testpath(config->dupeArea.fileName, "Dupearea filename", NULL, NULL);
    rc += testpath(config->badArea.fileName, "Badarea filename", NULL, NULL);

    for(i = 0; i < config->netMailAreaCount; i++)
    {
        rc += testpath(config->netMailAreas[i].fileName,
                       "Netmailarea",
                       config->netMailAreas[i].areaName,
                       config->netMailAreas[i].msgbType == MSGTYPE_SDM ? "pathname" : "filename");
    }

    for(i = 0; i < config->echoAreaCount; i++)
    {
        rc += testpath(config->echoAreas[i].fileName,
                       "Echoarea",
                       config->echoAreas[i].areaName,
                       config->echoAreas[i].msgbType == MSGTYPE_SDM ? "pathname" : "filename");
    }

    for(i = 0; i < config->localAreaCount; i++)
    {
        rc += testpath(config->localAreas[i].fileName,
                       "Localarea",
                       config->localAreas[i].areaName,
                       config->localAreas[i].msgbType == MSGTYPE_SDM ? "pathname" : "filename");
    }

    for(i = 0; i < config->fileAreaCount; i++)
    {
        rc += testpath(config->fileAreas[i].fileName,
                       "Filearea",
                       config->fileAreas[i].areaName,
                       "pathname");
    }

    for(i = 0; i < config->bbsAreaCount; i++)
    {
        rc += testpath(config->bbsAreas[i].pathName,
                       "BBSarea",
                       config->bbsAreas[i].areaName,
                       "pathname");
    }

    for(i = 0; i < config->nodelistCount; i++)
    {
        rc += testplainfile(config->nodelists[i].nodelistName,
                            "Nodelist",
                            config->nodelists[i].nodelistName,
                            NULL);
        rc += testpath(config->nodelists[i].diffUpdateStem,
                       "Nodelist's",
                       config->nodelists[i].nodelistName,
                       "diffUpdateStem");
        rc += testpath(config->nodelists[i].fullUpdateStem,
                       "Nodelist's",
                       config->nodelists[i].nodelistName,
                       "fullUpdateStem");
    }

    for(i = 0; i < config->filelistCount; i++)
    {
        rc += testplainfile(config->filelists[i].destFile,
                            "Filelist",
                            config->filelists[i].destFile,
                            NULL);
        rc += testpath(config->filelists[i].dirHdrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirHdrTpl");
        rc += testpath(config->filelists[i].dirEntryTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirEntryTpl");
        rc += testpath(config->filelists[i].dirFtrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirFtrTpl");
        rc += testpath(config->filelists[i].globHdrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "globHdrTpl");
        rc += testpath(config->filelists[i].globFtrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "globFtrTpl");
        rc += testpath(config->filelists[i].dirListHdrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirListHdrTpl");
        rc += testpath(config->filelists[i].dirListEntryTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirListEntryTpl");
        rc += testpath(config->filelists[i].dirListFtrTpl,
                       "Filelist's",
                       config->filelists[i].destFile,
                       "dirListFtrTpl");
    }

    /* robots */
    for(i = 0; i < config->robotCount; i++)
    {
        rc += testpath(config->robot[i]->helpFile, "helpFile", NULL, NULL);
        rc += testpath(config->robot[i]->rulesDir, "rulesDir", NULL, NULL);
        rc += testpath(config->robot[i]->newAreaRefuseFile, "newAreaRefuseFile", NULL, NULL);
        rc += testpath(config->robot[i]->autoCreateFlag, "autoCreateFlag", NULL, NULL);
        rc += testpath(config->robot[i]->queueFile, "queueFile", NULL, NULL);
    }

    /* test links */
    for(i = 0; i < config->linkCount; i++)
    {
        rc += testpath(config->links[i]->areafix.autoCreateFile,
                       "Link",
                       config->links[i]->name,
                       "areafixAutoCreateFile");
        rc += testpath(config->links[i]->filefix.autoCreateFile,
                       "Link",
                       config->links[i]->name,
                       "filefixAutoCreateFile");
        rc += testpath(config->links[i]->areafix.fwdFile,
                       "Link",
                       config->links[i]->name,
                       "areafixFwdFile");
        rc += testpath(config->links[i]->areafix.denyFwdFile,
                       "Link",
                       config->links[i]->name,
                       "areafixFwdDenyFile");
        rc += testpath(config->links[i]->filefix.fwdFile,
                       "Link",
                       config->links[i]->name,
                       "filefixFwdFile");
        rc += testpath(config->links[i]->filefix.denyFwdFile,
                       "Link",
                       config->links[i]->name,
                       "filefixFwdDenyFile");
        rc += testpath(config->links[i]->areafix.baseDir,
                       "Link",
                       config->links[i]->name,
                       "areafixBaseDir");
        rc += testpath(config->links[i]->filefix.baseDir,
                       "Link",
                       config->links[i]->name,
                       "filefixBaseDir");
        rc += testpath(config->links[i]->fileBox, "Link", config->links[i]->name, "fileBox");
    }
/*
   rc+=testplainfile(config->,"");
   rc+=testplainfile(config->,"");
   rc+=testpath(config->,"");
   rc+=testpath(config->,"");
 */
    return rc;
} /* testPathsAndFiles */

static const char * testAddr(ps_addr addr)
{
    char * c;

    if(!addr->zone && !addr->net && !addr->node)
    {
        return "Error: Invalid (zero) address\n";
    }

    if(addr->zone < 1)
    {
        return "Error: FTN zone must be a positive integer (max 32767)";
    }

    if(addr->net < 1)
    {
        return "Error: FTN network number must be a positive integer (max 32767)";
    }

    if(addr->node < -1)
    {
        return "Error: FTN node number must be a positive integer (max 32767), zero or -1";
    }

    if(addr->point < -1)
    {
        return "Error: FTN point number must be a positive integer (max 32767), zero or -1";
    }

    if((addr->node == -1) && (addr->point == -1))
    {
            return
                "Error: -1 in address may only be used for a node or a point request\nbut the node and the point number cannot be both -1";
    }

    if(!addr->node && addr->point)
    {
        return "Warning: network host can't have points";
    }

    for(c = addr->domain; *c; c++)
    {
        if(!isalnum(*c))
        {
            if(*c == '.')
            {
                return "Warning: FTN domain should not contain '.' char";
            }
            else
            {
                char * msg =
                    "Warning: FTN domain should contain alphanumeric characters only but ' ' is not";
                char * p = strchr(msg, '\'') + 1;

                if(p)
                {
                    *p = *c;
                }

                return msg;
            }
        }
    }

    return NULL;
} /* testAddr */

static void printAddr(ps_addr addr)
{
    if(addr)
    {
        if(addr->domain[0])
        {
            if(addr->point)
            {
                printf(" %d:%d/%d.%d@%s ",
                       addr->zone,
                       addr->net,
                       addr->node,
                       addr->point,
                       addr->domain);
            }
            else
            {
                printf(" %d:%d/%d@%s ", addr->zone, addr->net, addr->node, addr->domain);
            }
        }
        else
        {
            if(addr->point)
            {
                printf(" %d:%d/%d.%d ", addr->zone, addr->net, addr->node, addr->point);
            }
            else
            {
                printf(" %d:%d/%d ", addr->zone, addr->net, addr->node);
            }
        }
    }
} /* printAddr */

static void printArea(s_area area)
{
    unsigned i;

    printf("%s \n", area.areaName ? area.areaName : "");
    printf("Description: ");

    if(area.description != NULL)
    {
        printf("%s", area.description);
    }

    printf("\n-> %s\t", area.fileName ? area.fileName : "");

    if(area.msgbType == MSGTYPE_SDM)
    {
        printf("SDM");
    }
    else if(area.msgbType == MSGTYPE_SQUISH)
    {
        printf("Squish");
    }
    else if(area.msgbType == MSGTYPE_JAM)
    {
        printf("Jam");
    }
    else
    {
        printf("Passthrough");
    }

    printf("\t Use AKA: ");

    if(area.useAka == NULL)
    {
        printf("(not configured)");
    }
    else
    {
        printAddr(area.useAka);
    }

    printf("\n");

    if(area.paused)
    {
        printf("Area is paused\n");
    }

    printf("DOS Style File (8+3) %s\n", (area.DOSFile) ? "on (-dosfile)" : "off (-nodosfile)");
    printf("Level read  (-lr): %d\n", area.levelread);
    printf("Level write (-lw): %d\n", area.levelwrite);

    if(area.group)
    {
        printf("Group       - %s\n", area.group);
    }

    if(area.nopack)
    {
        printf(
            "Never purge (option \"-nopack\") (ignoring: max (-$m): %u msgs\tpurge (-p): %u days)\tdupeHistory %u\n",
            area.max,
            area.purge,
            area.dupeHistory);
    }
    else
    {
        printf(
            "Purging enabled (option \"-pack\") max (-$m): %u msgs\tpurge (-p): %u days\tdupeHistory %u\n",
            area.max,
            area.purge,
            area.dupeHistory);
    }

    printf("Options: ");

    if(area.tinySB)
    {
        printf("tinySB ");
    }
    else
    {
        printf("noTinySB ");
    }

    if(area.killSB)
    {
        printf("killSB ");
    }
    else
    {
        printf("noKillSB ");
    }

    if(area.keepUnread)
    {
        printf("keepUnread ");
    }
    else
    {
        printf("noKeepUnread ");
    }

    if(area.killRead)
    {
        printf("killRead ");
    }
    else
    {
        printf("noKillRead ");
    }

    if(area.hide)
    {
        printf("hide ");
    }
    else
    {
        printf("noHide ");
    }

    if(area.killMsgBase)
    {
        printf("kill ");
    }
    else
    {
        printf("noKill ");
    }

    if(area.manual)
    {
        printf("manual ");
    }
    else
    {
        printf("noManual ");
    }

    if(area.noPause)
    {
        printf("noPause ");
    }
    else
    {
        printf("pause ");
    }

    if(area.nolink)
    {
        printf("nolink ");
    }
    else
    {
        printf("link ");
    }

    if(area.mandatory)
    {
        printf("mandatory ");
    }
    else
    {
        printf("noMandatory ");
    }

    if(area.debug)
    {
        printf("debug ");
    }
    else
    {
        printf("noDebug ");
    }

    if(area.DOSFile)
    {
        printf("DOSFile ");
    }
    else
    {
        printf("noDOSFile ");
    }

    if(area.nopack)
    {
        printf("noPack ");
    }
    else
    {
        printf("pack ");
    }

    if(area.ccoff)
    {
        printf("ccoff ");
    }
    else
    {
        printf("noCCoff ");
    }

    if(area.keepsb)
    {
        printf("keepSB ");
    }
    else
    {
        printf("noKeepSB ");
    }

    if(area.noautoareapause)
    {
        printf("noAutoAreaPause ");
    }
    else
    {
        printf("autoAreaPause ");
    }

    if(area.sbkeep_all)
    {
        printf("SBkeepAll ");
    }
    else
    {
        printf("noSBkeepAll ");
    }

    printf("\n");
    printf("Default subscribing option:");

    if(area.def_subscribing == RO)
    {
        printf(" read-only");
    }

    if(area.def_subscribing == WO)
    {
        printf(" write-only");
    }

    printf("\n");
    printf("DupeCheck: ");

    if(area.dupeCheck == dcOff)
    {
        printf("off");
    }

    if(area.dupeCheck == dcMove)
    {
        printf("move");
    }

    if(area.dupeCheck == dcDel)
    {
        printf("delete");
    }

    printf("\n");

    /* tooOld */
    printf("tooOld: %u days", area.tooOld);

    if(area.tooOld == 0)
    {
        printf(" (disabled)");
    }

    printf("\n");
    /* tooOld */
    printf("tooNew: %u days", area.tooNew);

    if(area.tooNew == 0)
    {
        printf(" (disabled)");
    }

    printf("\n");
/* val: scan */
    printf("ScanMode: ");

    if(area.scanMode == smNever)
    {
        printf("never");
    }
    else if(area.scanMode == smManual)
    {
        printf("manual");
    }
    else if(area.scanMode == smListed)
    {
        printf("listed");
    }
    else
    {
        printf("normal");
    }

    printf("\n");

    if(area.downlinkCount)
    {
        printf("Links:\n");
    }
    else
    {
        printf("No links\n");
    }

    for(i = 0; i < area.downlinkCount; i++)
    {
        printf("\t");
        printAddr(&(area.downlinks[i]->link->hisAka));
        printf(" level %d,", area.downlinks[i]->link->level);

/*       printf(" exp. %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" imp. %s,", (area.downlinks[i]->import) ? "on" : "off");*/
        if(area.downlinks[i]->aexport || area.downlinks[i]->import)
        {
            printf(" ");
        }
        else
        {
            printf(" no access");
        }

        if(area.downlinks[i]->aexport)
        {
            printf("read");
        }

        if(area.downlinks[i]->aexport && area.downlinks[i]->import)
        {
            printf("/");
        }

        if(area.downlinks[i]->import)
        {
            printf("write");
        }

        if(area.downlinks[i]->aexport && area.downlinks[i]->rescan)
        {
            printf("/rescan");
        }

        if((area.downlinks[i]->aexport + area.downlinks[i]->import) == 1)
        {
            printf(" only");
        }

        printf(",");
        printf(" defLink %s,", (area.downlinks[i]->defLink) ? "on" : "off");
        printf(" mand. %s.\n", (area.downlinks[i]->mandatory) ? "on" : "off");
    }

    if(area.sbaddCount)
    {
        printf("addSeenBys: ");

        for(i = 0; i < area.sbaddCount; i++)
        {
            printf("%u/%u ", area.sbadd[i].net, area.sbadd[i].node);
        }
        printf("\n");
    }

    if(area.sbignCount)
    {
        printf("IgnoreSeenBys: ");

        for(i = 0; i < area.sbignCount; i++)
        {
            printf("%u/%u ", area.sbign[i].net, area.sbign[i].node);
        }
        printf("\n");
    }

    if(area.sbstripCount)
    {
        printf("StripSeenBys: ");

        for(i = 0; i < area.sbstripCount; i++)
        {
            printf("%u/%u ", area.sbstrip[i].net, area.sbstrip[i].node);
        }
        printf("\n");
    }

    if(area.sbkeepCount)
    {
        printf("KeepSeenBys: ");

        for(i = 0; i < area.sbkeepCount; i++)
        {
            printf("%u/%u ", area.sbkeep[i].net, area.sbkeep[i].node);
        }
        printf("\n");
    }

    printf("-------\n");
} /* printArea */

static int printAreaGroup(char * group)
{
    grp_t * g = (grp_t *)group;

    if(!g)
    {
        return 1;
    }

    printf("AreaGroup: %s -> %s \n", g->name, g->patternList);
    printArea(*(g->area));
    return 1;
}

static void printFileArea(s_area area)
{
    unsigned i;

    printf("%s \n", area.areaName);
    printf("Description: %s\n", (area.description) ? area.description : "");

    if(area.msgbType != MSGTYPE_PASSTHROUGH)
    {
        printf("Path: %s\t", area.fileName);
    }
    else
    {
        printf("Passthrough filearea");
    }

    printf("\t Use AKA: ");

    if(area.useAka == NULL)
    {
        printf("(not configured)");
    }
    else
    {
        printAddr(area.useAka);
    }

    printf("\n");
    printf("Level read  (-lr): %d\n", area.levelread);
    printf("Level write (-lw): %d\n", area.levelwrite);

    if(area.purge > 0)
    {
        printf("Purge (-p): %u days\n", area.purge);
    }
    else
    {
        printf("Purging disabled (-p 0)\n");
    }

    printf("Options: ");

    if(area.hide)
    {
        printf("hide ");
    }
    else
    {
        printf("noHide ");
    }

    if(area.manual)
    {
        printf("manual ");
    }
    else
    {
        printf("noManual ");
    }

    if(area.mandatory)
    {
        printf("mandatory ");
    }
    else
    {
        printf("noMandatory ");
    }

    if(area.sendorig)
    {
        printf("sendorig ");
    }
    else
    {
        printf("noSendOrig ");
    }

    if(area.noPause)
    {
        printf("noPause ");
    }
    else
    {
        printf("pause ");
    }

    if(area.noCRC)
    {
        printf("noCRC ");
    }
    else
    {
        printf("crc ");
    }

    if(area.noreplace)
    {
        printf("noReplace ");
    }
    else
    {
        printf("replace ");
    }

    if(area.rename)
    {
        printf("rename ");
    }
    else
    {
        printf("noRename ");
    }

    if(area.nodiz)
    {
        printf("noDiz ");
    }
    else
    {
        printf("diz ");
    }

    printf("\n");

    if(area.group)
    {
        printf("Group (-g): %s\n", area.group);
    }

    if(area.downlinkCount)
    {
        printf("Links:\n");
    }
    else
    {
        printf("No links\n");
    }

    for(i = 0; i < area.downlinkCount; i++)
    {
        printf("\t");
        printAddr(&(area.downlinks[i]->link->hisAka));
        printf(" level %d,", area.downlinks[i]->link->level);

        if(area.downlinks[i]->aexport || area.downlinks[i]->import)
        {
            printf(" ");
        }
        else
        {
            printf(" no access");
        }

        if(area.downlinks[i]->aexport)
        {
            printf("receive");
        }

        if(area.downlinks[i]->aexport && area.downlinks[i]->import)
        {
            printf("/");
        }

        if(area.downlinks[i]->import)
        {
            printf("send");
        }

        if((area.downlinks[i]->aexport + area.downlinks[i]->import) == 1)
        {
            printf(" only");
        }

        printf(",");
/*       printf(" export %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" import %s,", (area.downlinks[i]->import) ? "on" : "off");    */
        printf(" mandatory %s", (area.downlinks[i]->mandatory) ? "on" : "off");

        if(area.downlinks[i]->defLink)
        {
            printf("; this is (default) uplink.\n");
        }
        else
        {
            printf(".\n");
        }
    }
    printf("-------\n");
} /* printFileArea */

static void printBbsArea(s_bbsarea area)
{
    printf("%s \n", area.areaName);
    printf("Description: %s\n", area.description);
    printf("Path: %s\t", area.pathName);
    printf("\n");
    printf("-------\n");
}

static void printFilelist(s_filelist * fl)
{
    switch(fl->flType)
    {
        case flDir:
            printf("type: directory\n");
            break;

        case flGlobal:
            printf("type: global\n");
            break;

        case flDirList:
            printf("type: dirlist\n");
            break;

        default:
            printf("internal error: unknown flType!\n");
            break;
    }
    printf("destination file: %s\n", fl->destFile);

    if((fl->flType == flGlobal) || (fl->flType == flDir))
    {
        printf("directory header template: %s\n", fl->dirHdrTpl);
        printf("directory entry template: %s\n", fl->dirEntryTpl);
        printf("directory footer template: %s\n", fl->dirFtrTpl);
    }

    switch(fl->flType)
    {
        case flGlobal:
            printf("global header template: %s\n", fl->globHdrTpl);
            printf("global footer template: %s\n", fl->globFtrTpl);
            break;

        case flDirList:
            printf("dirlist header template: %s\n", fl->dirListHdrTpl);
            printf("dirlist entry template: %s\n", fl->dirListEntryTpl);
            printf("dirlist footer template: %s\n", fl->dirListFtrTpl);
            break;

        case flDir:
        default:
            /*  just avoid a warning */
            break;
    }
    printf("-------\n");
} /* printFilelist */

static char * cvtFlavour(e_flavour flavour)
{
    switch(flavour)
    {
        case flHold:
            return "hold";

        case flNormal:
            return "normal";

        case flDirect:
            return "direct";

        case flCrash:
            return "crash";

        case flImmediate:
            return "immediate";

        case flUndef:
            return "unknown!";

        default:
            fprintf(stderr, "Unknown flavour, update tparser!\n");
            return "";
    }
} /* cvtFlavour */

static int printLink(ps_link link)
{
    unsigned int i, rc = 0;

    printf("Link: ");
    printAddr(&(link->hisAka));
    printf(" (ourAka ");
    printAddr(link->ourAka);
    printf(")\n");

    if(link->hisPackAka.zone != 0)
    {
        printf("PackAka: ");
        printAddr(&(link->hisPackAka));
        printf("\n");
    }

    printf("Name: %s\n", link->name);

    if(link->defaultPwd)
    {
        printf("defaultPwd: %s\n", link->defaultPwd);
    }

    if(link->pktPwd)
    {
        printf("pktPwd:     %s\n", link->pktPwd);
    }

    if(link->ticPwd)
    {
        printf("ticPwd:     %s\n", link->ticPwd);
    }

    if(link->areafix.pwd)
    {
        printf("areafixPwd: %s\n", link->areafix.pwd);
    }

    if(link->filefix.pwd)
    {
        printf("filefixPwd: %s\n", link->filefix.pwd);
    }

    if(link->bbsPwd)
    {
        printf("bbsPwd:     %s\n", link->bbsPwd);
    }

    if(link->sessionPwd)
    {
        printf("sessionPwd: %s\n", link->sessionPwd);

        if(strlen(link->sessionPwd) > 8)
        {
            printf(
                "WARNING: sessionPwd is too long, should not be longer than 8 chars usually.\nA longer password may cause an error in some mailers.\n");
            fprintf(stderr,
                    "WARNING: sessionPwd is too long, should not be longer than 8 chars usually.\n");
        }
    }

    if(link->handle != link->name)
    {
        printf("handle:     %s\n", link->handle);
    }

    if(link->email)
    {
        printf("email:      %s\n", link->email);

        switch(link->emailEncoding)
        {
            case eeMIME:
                printf("emailEncoding: MIME\n");
                break;

            case eeSEAT:
                printf("emailEncoding: SEAT\n");
                break;

            case eeUUE:
                printf("emailEncoding: UUE\n");
                break;

            default:
                printf("Internal error: Unknown encoding #%d!\n", link->emailEncoding);
        }
    }

    if(link->emailFrom)
    {
        printf("emailFrom:  %s\n", link->emailFrom);
    }

    if(link->emailSubj)
    {
        printf("emailSubj:  %s\n", link->emailSubj);
    }

    printf("Level:      %u\n", link->level);
    printf("Export:     %s\n", (link->aexport) ? "on" : "off");
    printf("Import:     %s\n", (link->import) ? "on" : "off");
    printf("Mandatory:  %s\n", (link->mandatory) ? "on" : "off");
    printf("AvailList: ");

    switch(link->availlist)
    {
        case AVAILLIST_FULL:
            printf("Full\n");
            break;

        case AVAILLIST_UNIQUE:
            printf("Unique\n");
            break;

        case AVAILLIST_UNIQUEONE:
            printf("UniqueOne\n");
            break;

        default:
            printf("Unknown (m.b. fidoconf <--> tparser incompatible?)");
            break;
    }

    if((link->Pause & ECHOAREA) == ECHOAREA)
    {
        printf("Link is paused for echoes, no export\n");
    }

    if((link->Pause & FILEAREA) == FILEAREA)
    {
        printf("Link is paused for fileEchoes, no export\n");
    }

    if(link->autoPause)
    {
        printf("AutoPause after %u days\n", link->autoPause);
    }

    if(link->numOptGrp > 0)
    {
        printf("OptGrp       ");

        for(i = 0; i < link->numOptGrp; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->optGrp[i]);
        }
        printf("\n");
    }

    printf("areafixAutoCreate %s\n", (link->areafix.autoCreate) ? "on" : "off");
    printf("AutoAreaCreateSubdirs %s\n", (link->autoAreaCreateSubdirs) ? "on" : "off");

    if(link->areafix.autoCreateFile)
    {
        printf("areafixAutoCreateFile: %s\n", link->areafix.autoCreateFile);
    }

    if(link->areafix.numFrMask > 0)
    {
        printf("areafixFwdMask: ");

        for(i = 0; i < link->areafix.numFrMask; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->areafix.frMask[i]);
        }
        printf("\n");
    }

    if(link->areafix.numDfMask > 0)
    {
        printf("areafixFwdDenyMask: ");

        for(i = 0; i < link->areafix.numDfMask; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->areafix.dfMask[i]);
        }
        printf("\n");
    }

    if(link->areafix.autoCreateDefaults)
    {
        printf("areafixAutoCreateDefaults: %s\n", link->areafix.autoCreateDefaults);
    }

    printf("Automatically subscribing this link to an autocreated echo is %s\n",
           (link->areafix.autoSubscribe) ? "on" : "off");
    printf("Automatically subscribing this link to an autocreated fileecho is %s\n",
           (link->filefix.autoSubscribe) ? "on" : "off");
    printf("delNotReceivedTIC: %s\n", link->delNotReceivedTIC ? "on" : "off");
    printf("FileFixFSC87Subset %s\n", (link->FileFixFSC87Subset) ? "on" : "off");
    printf("filefixAutoCreate %s\n", (link->filefix.autoCreate) ? "on" : "off");
    printf("AutoFileCreateSubdirs %s\n", (link->autoFileCreateSubdirs) ? "on" : "off");

    if(link->filefix.autoCreateFile)
    {
        printf("filefixAutoCreateFile: %s\n", link->filefix.autoCreateFile);
    }

    if(link->filefix.numFrMask > 0)
    {
        printf("filefixFwdMask: ");

        for(i = 0; i < link->filefix.numFrMask; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->filefix.frMask[i]);
        }
        printf("\n");
    }

    if(link->filefix.numDfMask > 0)
    {
        printf("filefixFwdDenyMask: ");

        for(i = 0; i < link->filefix.numDfMask; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->filefix.dfMask[i]);
        }
        printf("\n");
    }

    if(link->filefix.autoCreateDefaults)
    {
        printf("filefixAutoCreateDefaults: %s\n", link->filefix.autoCreateDefaults);
    }

    if(link->LinkGrp)
    {
        printf("LinkGrp %s\n", link->LinkGrp);
    }

    if(link->numAccessGrp)
    {
        unsigned int j;
        printf("AccessGrp ");

        for(j = 0; j < link->numAccessGrp; j++)
        {
            if(j > 0)
            {
                printf(", %s", link->AccessGrp[j]);
            }
            else
            {
                printf("%s", link->AccessGrp[0]);
            }
        }
        printf("\n");
    }

    printf("AreaFix %s\n", (link->areafix.on) ? "on" : "off");

    if(link->areafix.echoLimit)
    {
        printf("areafixEchoLimit %u\n", link->areafix.echoLimit);
    }

    if(link->areafix.reportsAttr || link->areafix.reportsFlags)
    {
        char * attrs = attr2str(link->areafix.reportsAttr);
        printf("areafixReportsAttr: %s%s%s\n",
               attrs ? strUpper(attrs) : "",
               attrs ? " " : "",
               link->areafix.reportsFlags ? link->areafix.reportsFlags : "");
        nfree(attrs);
    }

    printf("FileFix %s\n", (link->filefix.on) ? "on" : "off");

    if(link->filefix.echoLimit)
    {
        printf("filefixEchoLimit %u\n", link->filefix.echoLimit);
    }

    if(link->filefix.reportsAttr || link->filefix.reportsFlags)
    {
        char * attrs = attr2str(link->filefix.reportsAttr);
        printf("filefixReportsAttr: %s%s%s\n",
               attrs ? strUpper(attrs) : "",
               attrs ? " " : "",
               link->filefix.reportsFlags ? link->filefix.reportsFlags : "");
        nfree(attrs);
    }

    printf("Forwarding Areafix Requests to this link is %s\n",
           (link->areafix.forwardRequests) ? "on" : "off");
    printf("Forwarding Filefix Requests to this link is %s\n",
           (link->filefix.forwardRequests) ? "on" : "off");

    if(link->areafix.forwardPriority)
    {
        printf("areafixFwdPriority: %u\n", link->areafix.forwardPriority);
    }

    if(link->filefix.forwardPriority)
    {
        printf("filefixFwdPriority: %u\n", link->filefix.forwardPriority);
    }

    printf("Areafix Forward Requests Access: %s\n", (link->areafix.denyFRA) ? "off" : "on");
    printf("Filefix Forward Requests Access: %s\n", (link->filefix.denyFRA) ? "off" : "on");
    printf("Areafix Unconditional Forward Requests Access: %s\n",
           (link->areafix.denyUFRA) ? "off" : "on");
    printf("Filefix Unconditional Forward Requests Access: %s\n",
           (link->filefix.denyUFRA) ? "off" : "on");
    printf("areafixName %s\n", link->areafix.name ? link->areafix.name : "areafix");
    printf("filefixName %s\n", link->filefix.name ? link->filefix.name : "filefix");

    if(link->areafix.fwdFile)
    {
        printf("areafixFwdFile %s\n", link->areafix.fwdFile);
    }

    if(link->filefix.fwdFile)
    {
        printf("filefixFwdFile %s\n", link->filefix.fwdFile);
    }

    if(link->areafix.denyFwdFile)
    {
        printf("areafixFwdDenyFile %s\n", link->areafix.denyFwdFile);
    }

    if(link->filefix.denyFwdFile)
    {
        printf("filefixFwdDenyFile %s\n", link->filefix.denyFwdFile);
    }

    if(link->areafix.baseDir)
    {
        printf("MsgBaseDir %s\n", link->areafix.baseDir);
    }

    if(link->filefix.baseDir)
    {
        printf("LinkFileBaseDir %s\n", link->filefix.baseDir);
    }

    if(link->packerDef)
    {
        printf("PackerDefault %s\n", link->packerDef->packer);
    }
    else
    {
        printf("PackerDefault none\n");
    }

    if(link->fileBox)
    {
        printf("fileBox %s\n", link->fileBox);
        printf("fileBoxAlways: %s\n", link->fileBoxAlways ? "on" : "off");
    }

    printf("TickerPackToBox %s\n", (link->tickerPackToBox) ? "on" : "off");

    if(link->pktSize != 0)
    {
        printf("pktSize - %u kb\n", link->pktSize);
    }

    if(link->arcmailSize != 0)
    {
        printf("arcmailSize - %u kb\n", link->arcmailSize);
    }

    printf("dailyBundles %s\n", link->dailyBundles ? "on" : "off");

    if(link->packerDef)
    {
        printf("packNetmail: %s\n", (link->packNetmail) ? "on" : "off");

        if(link->packNetmail)
        {
            printf("maxUnpackedNetmail: %d kb\n", link->maxUnpackedNetmail);
        }
    }
    else if(link->packNetmail)
    {
        printf("Packer not defined but packNetmail is on\n");
    }

    printf("TIC files %s\n", (link->noTIC == 0) ? "on" : "off");
    printf("forwardPkts ");

    switch(link->forwardPkts)
    {
        case fOff:
            printf("off\n");
            break;

        case fSecure:
            printf("secure\n");
            break;

        case fOn:
            printf("on\n");
            break;

        default:
            break;
    }
    printf("allowEmptyPktPwd ");

    switch(link->allowEmptyPktPwd)
    {
        case eOff:
            printf("off\n");
            break;

        case eSecure:
            printf("secure\n");
            break;

        case eOn:
            printf("on\n");
            break;

        default:
            break;
    }
    printf("allowPktAddrDiffer ");

    switch(link->allowPktAddrDiffer)
    {
        case pdOff:
            printf("off\n");
            break;

        case pdOn:
            printf("on\n");
            break;

        default:
            fprintf(stderr, "Error in keyword allowPktAddrDiffer\n");
            break;
    }
    printf("AdvancedAreaFix %s\n", (link->advancedAreafix) ? "on" : "off");

    switch(link->linkBundleNameStyle)
    {
        case eUndef:
            /* Don't print senseless information... printf("linkBundleNameStyle: undefined (like
               BundleNameStyle)\n"); */
            break;

        case eAddrDiff:
            printf("linkBundleNameStyle: addrDiff\n");
            break;

        case eAddrDiffAlways:
            printf("linkBundleNameStyle: addrDiffAlways\n");
            break;

        case eTimeStamp:
            printf("linkBundleNameStyle: timeStamp\n");
            break;

        case eAmiga:
            printf("linkBundleNameStyle: Amiga\n");
            break;

        case eAddrsCRC32:
            printf("linkBundleNameStyle: AddrsCRC32\n");
            break;

        case eAddrsCRC32Always:
            printf("linkBundleNameStyle: AddrsCRC32Always\n");
            break;

        default:
            printf("Warning: linkBundleNameStyle is UNKNOWN! Update tparser, please!\n");
            break;
    } /* switch */
    printf("arcNetmail %s\n", (link->arcNetmail) ? "on" : "off");
    printf("netMailFlavour %s\n", cvtFlavour(link->netMailFlavour));
    printf("echoMailFlavour %s\n", cvtFlavour(link->echoMailFlavour));
    printf("fileEchoFlavour %s\n", cvtFlavour(link->fileEchoFlavour));
    printf("areafixNoRules %s\n", (link->areafix.noRules) ? "on" : "off");
    printf("filefixNoRules %s\n", (link->filefix.noRules) ? "on" : "off");
    printf("reducedSeenBy %s\n", (link->reducedSeenBy) ? "on" : "off");
    printf("sendNotifyMessages %s\n", (link->sendNotifyMessages) ? "on" : "off");
    printf("allowRemoteControl %s\n", (link->allowRemoteControl) ? "on" : "off");
    printf("unsubscribeOnAreaDelete %s\n", (link->unsubscribeOnAreaDelete) ? "on" : "off");

    if(link->numRescanGrp)
    {
        printf("RescanGrp    ");

        for(i = 0; i < link->numRescanGrp; i++)
        {
            if(i > 0)
            {
                printf(",");
            }

            printf("%s", link->RescanGrp[i]);
        }
        printf("\n");
    }

    printf("denyRescan %s%s\n",
           (link->denyRescan) ? "on" : "off",
           (link->numRescanGrp) ? " (applies to areas on RescanGrp list)" : "");

    if(link->rescanLimit)
    {
        printf("rescanLimit %d\n", link->rescanLimit);
    }

    printf("-------\n");
    return rc;
} /* printLink */

static int printRobot(ps_robot robot)
{
    printf("Robot %s\n", robot->name);

    if(robot->names)
    {
        int i;
        printf("  robotNames");

        for(i = 0; i < robot->names->count; ++i)
        {
            printf(" %s", STR_N(robot->names, i));
        }
        printf("\n");
    }

    if(robot->fromName)
    {
        printf("  fromName %s\n", robot->fromName);
    }

    if(robot->origin)
    {
        printf("  robotOrigin %s\n", robot->origin);
    }

    if(robot->helpFile)
    {
        printf("  helpFile %s\n", robot->helpFile);
    }

    if(robot->rulesDir)
    {
        printf("  rulesDir %s\n", robot->rulesDir);
    }

    if(robot->newAreaRefuseFile)
    {
        printf("  newAreaRefuseFile %s\n", robot->newAreaRefuseFile);
    }

    if(robot->autoCreateFlag)
    {
        printf("  autoCreateFlag %s\n", robot->autoCreateFlag);
    }

    if(robot->queueFile)
    {
        printf("  queueFile %s\n", robot->queueFile);
    }

    if(robot->reportsAttr || robot->reportsFlags)
    {
        char * attrs = attr2str(robot->reportsAttr);
        printf("  reportsAttr %s%s%s\n",
               attrs ? strUpper(attrs) : "",
               attrs ? " " : "",
               robot->reportsFlags ? robot->reportsFlags : "");
        nfree(attrs);
    }

    printf("  killRequests %s\n", robot->killRequests ? "on" : "off");
    printf("  queryReports %s\n", robot->queryReports ? "on" : "off");

    if(robot->msgSize)
    {
        printf(" msgSize %u\n", robot->msgSize);
    }

    if(robot->splitStr)
    {
        printf(" splitStr \"%s\"\n", robot->splitStr);
    }

    printf("  autoAreaPause %s\n", robot->autoAreaPause ? "on" : "off");
    printf("  forwardRequestTimeout %u\n", robot->forwardRequestTimeout);
    printf("  idlePassthruTimeout   %u\n", robot->idlePassthruTimeout);
    printf("  killedRequestTimeout  %u\n", robot->killedRequestTimeout);
    printf("-------\n");
    return 0;
} /* printRobot */

/* Attention! Return value is static string */
static const char * routeViaStr(e_routing routeVia)
{
    switch(routeVia)
    {
        case route_zero:
            return "zero";

        case noroute:
            return "direct";

        case nopack:
            return "nopack";

        case host:
            return "host (Z:N/0.0@domain)";

        case hub:
            return "\"hub\" (Z:N/ff00.0@domain)";

        case boss:
            return "boss (Z:N/F.0@domain)";

        case route_extern: /* internal only */
            return "";
    }
    return "UNKNOWN(error?)";
} /* routeViaStr */

static void printRouteTarget(s_route aroute)
{
    if(aroute.target)
    {
        printAddr(&(aroute.target->hisAka));
    }
    else
    {
        printf(" %s ", routeViaStr(aroute.routeVia));
    }
}

/*  Some dumb checks ;-) */
static int checkLogic(s_fidoconfig * lconfig)
{
    register unsigned i, j, m;
    register int k, rc = 0;
    int robotsarea_ok;
    s_link * link;
    s_area * area;
    register char * areaName;
    char * passthrough = "passthrough";

    robotsarea_ok = lconfig->robotsArea ? 0 : 1;

    /* Robots should not access same queueFile */
    for(i = 0; i < lconfig->robotCount; i++)
    {
        if(lconfig->robot[i]->queueFile == NULL)
        {
            continue;
        }

        for(j = i + 1; j < lconfig->robotCount; j++)
        {
            if(lconfig->robot[j]->queueFile != NULL &&
               stricmp(lconfig->robot[i]->queueFile, lconfig->robot[j]->queueFile) == 0)
            {
                printf("Error: robots %s and %s use the same queueFile %s, but they should not!\n",
                       lconfig->robot[i]->name,
                       lconfig->robot[j]->name,
                       lconfig->robot[i]->queueFile);
                rc++;
            }
        }
    }

    for(i = 0; i + 1 < lconfig->linkCount; i++)
    {
        /* Check link address */
        const char * addrerror = testAddr(&(lconfig->links[i]->hisAka));

        if(addrerror)
        {
            printf("Link %s", lconfig->links[i]->name);
            printAddr(&(lconfig->links[i]->hisAka));
            printf(":\n %s\n", addrerror);
            rc++;
        }

        /* Check links duplication */
        for(j = i + 1; j < lconfig->linkCount; j++)
        {
            if(addrComp(&(lconfig->links[i]->hisAka), &(lconfig->links[j]->hisAka)) == 0)
            {
                if(strcmp(lconfig->links[i]->name, lconfig->links[j]->name) != 0)
                {
                    printf("Warning: duplicate definition of link %s ", lconfig->links[i]->name);
                    printAddr(&(lconfig->links[i]->hisAka));
                    printf("\n");
                    continue;
                }

                printf("ERROR: duplication of link ");
                printAddr(&(lconfig->links[i]->hisAka));
                printf("\n");
                printf("remove it, or change the name!\n");
                exit(-1);
            }
        }

        /* Check file permissions */
        if(lconfig->links[i]->areafix.autoCreateFile)
        {
            k = open(lconfig->links[i]->areafix.autoCreateFile, O_RDWR | O_APPEND);

            if(k < 0)
            {
                printf("ERROR: link ");
                printAddr(&(lconfig->links[i]->hisAka));
                printf(" areafixAutoCreateFile '%s': %s\n",
                       lconfig->links[i]->areafix.autoCreateFile,
                       strerror(errno));
                exit(-1);
            }
            else
            {
                close(k);
            }
        }

        if(lconfig->links[i]->filefix.autoCreateFile)
        {
            k = open(lconfig->links[i]->filefix.autoCreateFile, O_RDWR | O_APPEND);

            if(k < 0)
            {
                printf("ERROR: link ");
                printAddr((&lconfig->links[i]->hisAka));
                printf(" filefixAutoCreateFile '%s': %s\n",
                       lconfig->links[i]->filefix.autoCreateFile,
                       strerror(errno));
                exit(-1);
            }
            else
            {
                close(k);
            }
        }

        /* Check for absence of passthrough token in autoCreateDefaults */
        if(lconfig->links[i]->areafix.autoCreateDefaults)
        {
            if(strstr(strLower(lconfig->links[i]->areafix.autoCreateDefaults), passthrough))
            {
                printf("ERROR: areafixAutoCreateDefaults for");
                printAddr((&lconfig->links[i]->hisAka));
                printf("contains 'passthrough' keyword\n");
                rc++;
            }
        }

        if(lconfig->links[i]->filefix.autoCreateDefaults)
        {
            if(strstr(strLower(lconfig->links[i]->filefix.autoCreateDefaults), passthrough))
            {
                printf("ERROR: filefixAutoCreateDefaults for");
                printAddr((&lconfig->links[i]->hisAka));
                printf("contains 'passthrough' keyword\n");
                rc++;
            }
        }
    }

    for(i = 0; i < lconfig->echoAreaCount; i++)
    {
        area     = &(lconfig->echoAreas[i]);
        areaName = area->areaName;

        if(lconfig->robotsArea && sstricmp(lconfig->robotsArea, areaName) == 0)
        {
            robotsarea_ok = 1;
        }

        /*    j=i+1 */
        for(j = i + 1; j < lconfig->echoAreaCount; j++)
        {
            if(stricmp(lconfig->echoAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of echoarea %s\n", areaName);
                exit(-1);
            }
        }

        for(j = 0; j < lconfig->localAreaCount; j++)
        {
            if(stricmp(lconfig->localAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of echoarea %s\n", areaName);
                exit(-1);
            }
        }

        for(j = 0; j < lconfig->netMailAreaCount; j++)
        {
            if(stricmp(lconfig->netMailAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of echoarea %s\n", areaName);
                exit(-1);
            }
        }

        /*  Check for area link duplication */
        for(j = 0; j + 1 < area->downlinkCount; j++)
        {
            link = area->downlinks[j]->link;

            for(m = j + 1; m < area->downlinkCount; m++)
            {
                if(link == area->downlinks[m]->link)
                {
                    printf("ERROR: duplication of link ");
                    printAddr(&(link->hisAka));
                    printf(" in echoarea %s\n", areaName);
                    exit(-1);
                }
            }
        }

        /* Check for echoloop */
        if(area->useAka->point)
        {
            hs_addr areaboss = *area->useAka;
            areaboss.point = 0;

            for(j = 0; j < area->downlinkCount; j++)
            {
                ps_link link1 = area->downlinks[j]->link;

                if((link1->hisAka.point == 0) && addrComp(&(link1->hisAka), &areaboss))
                {
                    printf("WARNING: echoarea %s is subscribed to ", areaName);
                    printAddr(&(link1->hisAka));
                    printf(". This node is not boss-node of your AKA ");
                    printAddr(area->useAka);
                    printf(" used in this echo! Echo loop or seen-by lock is possible.\n");
                }
            }
        }
    }

    for(i = 0; i < lconfig->localAreaCount; i++)
    {
        area     = &(lconfig->localAreas[i]);
        areaName = lconfig->localAreas[i].areaName;

        if(lconfig->robotsArea && sstricmp(lconfig->robotsArea, areaName) == 0)
        {
            robotsarea_ok = 1;
        }

        for(j = 0; j < lconfig->echoAreaCount; j++)
        {
            if(stricmp(lconfig->echoAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of local area %s\n", areaName);
                exit(-1);
            }
        }

        /*    j=i+1 */
        for(j = i + 1; j < lconfig->localAreaCount; j++)
        {
            if(stricmp(lconfig->localAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of local area %s\n", areaName);
                exit(-1);
            }
        }

        for(j = 0; j < lconfig->netMailAreaCount; j++)
        {
            if(stricmp(lconfig->netMailAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of local area %s\n", areaName);
                exit(-1);
            }
        }

        /*  Check for area link duplication */
        for(j = 0; j + 1 < area->downlinkCount; j++)
        {
            link = area->downlinks[j]->link;

            for(m = j + 1; m < area->downlinkCount; m++)
            {
                if(link == area->downlinks[m]->link)
                {
                    printf("ERROR: duplication of link ");
                    printAddr(&(link->hisAka));
                    printf(" in local area %s\n", areaName);
                    exit(-1);
                }
            }
        }
    }

    for(i = 0; i < lconfig->netMailAreaCount; i++)
    {
        area     = &(lconfig->netMailAreas[i]);
        areaName = lconfig->netMailAreas[i].areaName;

        if(lconfig->robotsArea && sstricmp(lconfig->robotsArea, areaName) == 0)
        {
            robotsarea_ok = 1;
        }

        for(j = 0; j < lconfig->echoAreaCount; j++)
        {
            if(stricmp(lconfig->echoAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of netmail area %s\n", areaName);
                exit(-1);
            }
        }

        for(j = 0; j < lconfig->localAreaCount; j++)
        {
            if(stricmp(lconfig->localAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of netmail area %s\n", areaName);
                exit(-1);
            }
        }

        /*    j=i+1 */
        for(j = i + 1; j < lconfig->netMailAreaCount; j++)
        {
            if(stricmp(lconfig->netMailAreas[j].areaName, areaName) == 0)
            {
                printf("ERROR: duplication of netmail area %s\n", areaName);
                exit(-1);
            }
        }

        /*  Check for area link duplication */
        for(j = 0; j + 1 < area->downlinkCount; j++)
        {
            link = area->downlinks[j]->link;

            for(m = j + 1; m < area->downlinkCount; m++)
            {
                if(link == area->downlinks[m]->link)
                {
                    printf("ERROR: duplication of link");
                    printAddr(&(link->hisAka));
                    printf("in netmail area %s\n", areaName);
                    exit(-1);
                }
            }
        }
    }

    if(!robotsarea_ok)
    {
        printf("ERROR: robotsarea value is not an existing area\n");
        exit(-1);
    }/* Check Remap rules */

    {
        unsigned int c;

        for(c = 0; c < lconfig->remapCount; c++)
        {
            const char * testresult = NULL;

            if(lconfig->remaps[c].oldaddr.zone > 0)
            {
                testresult = testAddr(&(lconfig->remaps[c].oldaddr));
            }

            if(testresult)
            {
                printf("Remap rule %i, old destination address %s\n", c + 1, testresult);
                rc++;
            }

            testresult = testAddr(&(lconfig->remaps[c].newaddr));

            if(testresult)
            {
                printf("Remap rule %i, new destination address %s\n", c + 1, testresult);
                rc++;
            }
        }
    }
    /* Check Routing rules */
    {
        ps_route aroute;
        unsigned int c, l;

        for(c = 0; c < lconfig->routeCount; c++)
        {
            aroute = &(lconfig->route[c]);

            for(l = c + 1; l < lconfig->routeCount; l++)
            {
                if((aroute->id == id_route || lconfig->route[l].id == id_route ||
                    aroute->id == lconfig->route[l].id) &&
                   (stricmp(aroute->pattern, lconfig->route[l].pattern) == 0))
                {
                    if(aroute->id == lconfig->route[l].id)
                    {
                        printf("Warning! Duplicate route%s%s %s via ",
                               aroute->id == id_routeMail ? "mail" : "",
                               aroute->id == id_routeFile ? "file" : "",
                               aroute->pattern);
                    }
                    else
                    {
                        printf("Warning! Duplicate route%s%s and route%s%s %s via ",
                               aroute->id == id_routeMail ? "mail" : "",
                               aroute->id == id_routeFile ? "file" : "",
                               lconfig->route[l].id == id_routeMail ? "mail" : "",
                               lconfig->route[l].id == id_routeFile ? "file" : "",
                               aroute->pattern);
                    }

                    if(lconfig->route[l].target != aroute->target &&
                       (!lconfig->route[l].target || !aroute->target ||
                        addrComp(&(lconfig->route[l].target->hisAka), &(aroute->target->hisAka))))
                    {
                        printf("different links (targets): ");
                        printRouteTarget(*aroute);
                        printf(" and ");
                        printRouteTarget(lconfig->route[l]);
                    }
                    else
                    {
                        printRouteTarget(*aroute);
                    }

                    printf("\n");
                }
            }
        }
    }
    return rc;
} /* checkLogic */

static void printCarbons(s_fidoconfig * pconfig)
{
    unsigned i;
    s_carbon * cb;
    char * crbKey = "", * nspc, * cbaName, * tempc = NULL;

    printf("\n=== CarbonCopy ===\n");
    printf("CarbonAndQuit %s\n", (pconfig->carbonAndQuit) ? "on" : "off");
    printf("CarbonKeepSb %s\n", (pconfig->carbonKeepSb) ? "on" : "off");
    printf("CarbonOut %s\n", (pconfig->carbonOut) ? "on" : "off");
    printf("ExcludePassthroughCarbon %s\n", (pconfig->exclPassCC) ? "on" : "off");
    printf("Exclude \" * Forward from area \" string: %s\n\n",
           (pconfig->carbonExcludeFwdFrom) ? "on" : "off");

    for(i = 0, cb = &(pconfig->carbons[0]); i < pconfig->carbonCount; i++, cb++)
    {
        if(cb->rule & CC_NOT)
        {
            nspc = "";
            printf("|NOT ");
        }
        else
        {
            nspc = "|    ";
        }

        switch(cb->ctype)
        {
            case ct_to:
                crbKey = "To:       ";
                break;

            case ct_from:
                crbKey = "From:     ";
                break;

            case ct_kludge:
                crbKey = "Kludge:   ";
                break;

            case ct_subject:
                crbKey = "Subj:     ";
                break;

            case ct_msgtext:
                crbKey = "Text:     ";
                break;

            case ct_fromarea:
                crbKey = "FromArea: ";
                break;

            case ct_group:
                crbKey = "Groups:   ";
                break;

            case ct_addr:
                crbKey = "Addr:     ";
                break;

            default:
                break;
        } /* switch */
        printf("%sCarbon%s\"%s\"\n", nspc, crbKey,
               (cb->ctype == ct_addr) ? (tempc = aka2str5d(cb->addr)) : cb->str);
        nfree(tempc);

        if(cb->rule & CC_AND)
        {
            continue;
        }

        if(cb->areaName != NULL)
        {
            cbaName = cb->areaName;

            if(*cbaName == '*')
            {
                ++cbaName;
            }
        }
        else
        {
            cbaName = "UNKNOWN";
        }

        if(cb->extspawn)
        {
            printf("CarbonExtern: \"%s\"", cbaName);
        }
        else
        {
            switch(cb->move)
            {
                case 0:
                    printf("CarbonCopy:          ");
                    break;

                case 2:
                    printf("CarbonDelete");
                    break;

                case 1:
                default:
                    printf("CarbonMove:          ");
                    break;
            }

            if(cb->areaName)
            {
                if(stricmp(cbaName, cb->area->areaName))
                {
                    printf(" !!! \"%s\" wanted !!! using \"%s\"", cbaName, cb->area->areaName);
                }
                else
                {
                    printf("\"%s\"", cb->area->areaName);
                }
            }
            else if(cb->move != 2)
            {
                printf(" !!! No area specified !!!");
            }
        }

        putchar('\n');

        if(cb->reason)
        {
            printf("CarbonReason:   %s\n", cb->reason);
        }

        if(cb->aexport)
        {
            printf("Copied messages will be exported.\n");
        }

        if(cb->netMail)
        {
            printf("Active on netMail\n");
        }

        if(cb->areaName != NULL)
        {
            if(*cb->areaName == '*')
            {
                printf("CarbonAndQuit ignored.\n");
            }
        }

        printf("-------\n");
    }
} /* printCarbons */

static void printRemaps(s_fidoconfig * rconfig)
{
    unsigned i;

    printf("\n=== Remap rconfig ===\n");

    for(i = 0; i < rconfig->remapCount; i++)
    {
        printf("ToName: \"%s\" and ToAddress:",
               sstrlen(rconfig->remaps[i].toname) ? rconfig->remaps[i].toname : "<any name>");

        if(rconfig->remaps[i].oldaddr.zone == 0)
        {
            printf(" <any address> ");
        }
        else
        {
            printAddr(&(rconfig->remaps[i].oldaddr));
        }

        printf("=>");
        printAddr(&(rconfig->remaps[i].newaddr));
        putchar('\n');
    }
}

static void printSeqOutrun(unsigned long seqOutrun)
{
    if(seqOutrun % (365l * 24 * 60 * 60) == 0)
    {
        printf("seqOutrun: %luy\n", seqOutrun / (365l * 24 * 60 * 60));
    }
    else if(seqOutrun % (31l * 24 * 60 * 60) == 0)
    {
        printf("seqOutrun: %lum\n", seqOutrun / (31l * 24 * 60 * 60));
    }
    else if(seqOutrun % (7l * 24 * 60 * 60) == 0)
    {
        printf("seqOutrun: %luw\n", seqOutrun / (7l * 24 * 60 * 60));
    }
    else if(seqOutrun % (24 * 60 * 60) == 0)
    {
        printf("seqOutrun: %lud\n", seqOutrun / (24 * 60 * 60));
    }
    else if(seqOutrun % (60 * 60) == 0)
    {
        printf("seqOutrun: %luh\n", seqOutrun / (60 * 60));
    }
    else
    {
        printf("seqOutrun: %lu\n", seqOutrun);
    }
} /* printSeqOutrun */

static char * printListEcho(e_listEchoMode mode)
{
    switch(mode)
    {
        case lemUnsorted:
            return "unsorted";

        case lemName:
            return "name";

        case lemUndef:
            return "name (default)"; /* default is equal to "name" */

        case lemGroup:
            return "group";

        case lemGroupName:
            return "group,name";

        default:
            return "unknown (try to update tparser)";
    }
}

static int dumpcfg(char * fileName)
{
    char * line;

    if(fileName == NULL)
    {
        fileName = getConfigFileName();
    }

    if(fileName == NULL)
    {
        printf("Could not find Config-file\n");
        return EX_UNAVAILABLE;
    }

    if(init_conf(fileName))
    {
        return 0;
    }

    while((line = configline()) != NULL)
    {
        line = trimLine(line);

        if(line[0] != 0)
        {
            line = shell_expand(line);
            line = vars_expand(line);
            puts(line);
        }
        else
        {
            puts(line);
        }

        nfree(line);
    }
    close_conf();

    /* nfree(fileName); */
    return 0;
} /* dumpcfg */

static void usage(void)
{
    printf("\tParses Fidoconfig, checks your Fidoconfig for errors and gives ");
    printf("you some\n\thints to solve the problems.\n\n");
    printf("run: tparser [-h|--help] [-Dvar=value] [-E] [-P] [/path/to/config/file]\n");
    printf("\tOptions:\n");
    printf("-h\tDisplay this help\n");
    printf("--help\tDisplay this help\n");
    printf("-Dvar=value\tSet the config variable 'var' to 'value'\n");
    printf("-E\tDump config to stdout\n");
    printf("-P\tTry to create non-existing directories\n");
    exit(0);
}

int main(int argc, char ** argv)
{
/*   s_fidoconfig *config = NULL;  use global variable */
    unsigned i, j, hpt = 0, preproc = 0, rc = 0;
    int k;
    char * cfgFile = NULL, * module;

#include "../cvsdate.h"

    SetAppModule(M_TPARSER);
    printf("%s\n",
           module =
               GenVersionStr("tparser", fidoconf_VER_MAJOR, fidoconf_VER_MINOR,
                             fidoconf_VER_PATCH, fidoconf_VER_BRANCH,
                             cvs_date));
    nfree(module);              /* used as a temporary variable */

    //if( !CheckFidoconfigVersion(1,9,0,BRANCH_CURRENT,cvs_date) ) {
    //  printf("Incompatible version of FIDOCONFIG library: require fidoconfig-1.9.0-current at
    // %s\n",cvs_date);
    //  exit(255);
    //}
    //if( !CheckSmapiVersion(2,5,0,smapi_cvs_date()) ) {
    //  printf("Incompatible version of SMAPI: require smapi-2.5.0-current at
    // %s\n",smapi_cvs_date());
    //  exit(255);
    //}
    //printf("using smapi-2.5.0-current at %s and fidoconfig-1.9.0-current at
    // %s\n\n",smapi_cvs_date(),cvs_date);
    for(k = 1; k < argc; k++)
    {
        if(argv[k][0] != '-')   /* is not option */
        {
            if(cfgFile)
            {
                usage();
            }
            else
            {
                xstrcat(&cfgFile, argv[k]);
            }
        }
        else if(argv[k][1] == 'D') /* -Dvar=value */
        {
            char * p = strchr(argv[k], '=');

            if(p)
            {
                *p = '\0';
                setvar(argv[k] + 2, p + 1);
                *p = '=';
            }
            else
            {
                setvar(argv[k] + 2, "");
            }
        }
        else if(argv[k][1] == 'E') /* -E */
        {
            preproc = 1;
        }
        else if(argv[k][1] == 'P') /* -P */
        {
            fc_trycreate = 1;
        }
        else if(stricmp(argv[k], "--help") == 0 || argv[k][1] == 'h' || cfgFile != NULL)
        {
            usage();
            return 0;
        }
    }

    if(cfgFile == NULL)
    {
        cfgFile = getConfigFileName();
    }

    if(cfgFile == NULL)
    {
        printf("Could not find Config-file\n");
        exit(EX_UNAVAILABLE);
    }

    module = getvar("module");
    printf("Test %s for ", cfgFile);

    if(module)
    {
        printf("module: %s\n", module);

        if(stricmp(module, "hpt") == 0)
        {
            hpt = 1;
        }
    }
    else
    {
        printf("all modules\n");
    }

    if(preproc)
    {
        return dumpcfg(cfgFile);
    }

    config = readConfig(cfgFile);

    /* nfree(cfgFile); */
    if(config != NULL)
    {
        rc += checkLogic(config);
        rc += testConfig(config);
        printf("=== MAIN CONFIG ===\n");
        printf("Version: %u.%u\n", config->cfgVersionMajor, config->cfgVersionMinor);

        if(config->name != NULL)
        {
            printf("Name:     %s\n", config->name);
        }

        if(config->sysop != NULL)
        {
            printf("Sysop:    %s\n", config->sysop);
        }

        if(config->location != NULL)
        {
            printf("Location: %s\n", config->location);
        }

        for(i = 0; i < config->addrCount; i++)
        {
            if(config->addr[i].domain[0])
            {
                printf("Addr: %u:%u/%u.%u@%s\n",
                       config->addr[i].zone,
                       config->addr[i].net,
                       config->addr[i].node,
                       config->addr[i].point,
                       config->addr[i].domain);
            }
            else
            {
                printf("Addr: %u:%u/%u.%u\n",
                       config->addr[i].zone,
                       config->addr[i].net,
                       config->addr[i].node,
                       config->addr[i].point);
            }
        }

#if defined (__UNIX__)
        printf("FileAreaCreatePerms: %o\n", config->fileAreaCreatePerms);
#endif

        if(config->loglevels)
        {
            printf("LogLevels %s\n", config->loglevels);
        }

        printf("LogEchoToScreen %s\n", (config->logEchoToScreen) ? "on" : "off");

        if(config->logEchoToScreen && config->screenloglevels)
        {
            printf("ScreenLogLevels %s\n", config->screenloglevels);
        }

        if(config->echotosslog != NULL)
        {
            printf("EchoTossLog:     %s\n", config->echotosslog);
        }

        if(config->importlog != NULL)
        {
            printf("ImportLog:       %s\n", config->importlog);
        }

        if(config->statlog != NULL)
        {
            printf("StatLog:         %s\n", config->statlog);
        }

        if(config->inbound != NULL)
        {
            printf("Inbound:         %s\n", config->inbound);
        }

        if(config->tempInbound != NULL)
        {
            printf("tempInbound:     %s\n", config->tempInbound);
        }

        if(config->protInbound != NULL)
        {
            printf("ProtInbound:     %s\n", config->protInbound);
        }

        if(config->localInbound != NULL)
        {
            printf("LocalInbound:    %s\n", config->localInbound);
        }

        if(config->listInbound != NULL)
        {
            printf("ListInbound:     %s\n", config->listInbound);
        }

        if(config->ticOutbound != NULL)
        {
            printf("TicOutbound:     %s\n", config->ticOutbound);
        }

        if(config->outbound != NULL)
        {
            printf("Outbound:        %s\n", config->outbound);
        }

        if(config->tempOutbound != NULL)
        {
            printf("tempOutbound:    %s\n", config->tempOutbound);
        }

        for(i = 0; i < config->publicCount; i++)
        {
            printf("Public: #%u %s\n", i + 1, config->publicDir[i]);
        }

        if(config->reqidxDir)
        {
            printf("ReqIdxDir:       %s\n", config->reqidxDir);
        }

        if(config->dupeHistoryDir != NULL)
        {
            printf("DupeHistoryDir:  %s\n", config->dupeHistoryDir);
        }

        if(config->logFileDir != NULL)
        {
            printf("LogFileDir:      %s\n", config->logFileDir);
        }

        if(config->tempDir != NULL)
        {
            printf("TempDir:      %s\n", config->tempDir);
        }

        if(config->msgBaseDir != NULL)
        {
            printf("MsgBaseDir:      %s\n", config->msgBaseDir);
        }

        printf("RecodeMsgBase: %s\n", config->recodeMsgBase ? "on" : "off");

        if(config->fileAreaBaseDir)
        {
            printf("FileAreaBaseDir: %s\n", config->fileAreaBaseDir);
        }

        if(config->passFileAreaDir)
        {
            printf("passFileAreaDir: %s\n", config->passFileAreaDir);
        }

        if(config->busyFileDir)
        {
            printf("busyFileDir:     %s\n", config->busyFileDir);
        }

        if(config->magic)
        {
            printf("Magic: %s\n", config->magic);
        }

        if(config->semaDir)
        {
            printf("semaDir:         %s\n", config->semaDir);
        }

        if(config->badFilesDir)
        {
            printf("BadFilesDir:     %s\n", config->badFilesDir);
        }

        if(config->advStatisticsFile)
        {
            printf("advStatisticsFile:       %s\n", config->advStatisticsFile);
        }

        if(config->hptPerlFile)
        {
            printf("hptPerlFile:     %s\n", config->hptPerlFile);
        }

        if(config->netmailFlag)
        {
            printf("NetmailFlag:     %s\n", config->netmailFlag);
        }

        if(config->minDiskFreeSpace)
        {
            printf("MinDiskFreeSpace: %u Mb\n", config->minDiskFreeSpace);
        }

        if(config->syslogFacility)
        {
            printf("SyslogFacility: %d\n", config->syslogFacility);
        }

        if(config->lockfile)
        {
            printf("LockFile: %s\n", config->lockfile);

            if(config->advisoryLock == 0)
            {
                printf("AdvisoryLock: off\n");
            }
            else
            {
                printf("AdvisoryLock: %u\n", config->advisoryLock);
            }
        }

        if(hpt == 0)
        {
            printf("LongDirNames: %s\n", (config->longDirNames) ? "on" : "off");
            printf("SplitDirs: %s\n", (config->splitDirs) ? "on" : "off");
        }

        printf("Ignore Capability Word: %s\n", (config->ignoreCapWord) ? "on" : "off");
        printf("ProcessBundles %s\n", (config->noProcessBundles) ? "off" : "on");

        switch(config->bundleNameStyle)
        {
            case eUndef:
                printf("BundleNameStyle: undefined (timeStamp)\n");
                break;

            case eAddrDiff:
                printf("BundleNameStyle: addrDiff\n");
                break;

            case eAddrDiffAlways:
                printf("BundleNameStyle: addrDiffAlways\n");
                break;

            case eTimeStamp:
                printf("BundleNameStyle: timeStamp\n");
                break;

            case eAmiga:
                printf("BundleNameStyle: Amiga\n");
                break;

            case eAddrsCRC32:
                printf("BundleNameStyle: AddrsCRC32\n");
                break;

            case eAddrsCRC32Always:
                printf("BundleNameStyle: AddrsCRC32Always\n");
                break;

            default:
                printf("Warning: BundleNameStyle is UNKNOWN! Please update tparser!\n");
                break;
        } /* switch */

        if(config->fileBoxesDir)
        {
            printf("fileBoxesDir: %s\n", config->fileBoxesDir);
        }

        printf("DupeBaseType: ");

        if(config->typeDupeBase == textDupes)
        {
            printf("textDupes\n");
        }

        if(config->typeDupeBase == hashDupes)
        {
            printf("hashDupes\n");
        }

        if(config->typeDupeBase == hashDupesWmsgid)
        {
            printf("hashDupesWmsgid\n");
        }

        if(config->typeDupeBase == commonDupeBase)
        {
            printf("commonDupeBase\n");
            printf("AreasMaxDupeAge: %d\n", config->areasMaxDupeAge);
        }

        if(config->numPublicGroup > 0)
        {
            printf("PublicGroups: ");

            for(i = 0; i < config->numPublicGroup; i++)
            {
                printf((i > 0) ? ",%s" : "%s", config->PublicGroup[i]);
            }
            printf("\n");
        }

        printf("createAreasCase: %s\n", (config->createAreasCase == eLower) ? "Lower" : "Upper");
        printf("areasFileNameCase: %s\n",
               (config->areasFileNameCase == eLower) ? "Lower" : "Upper");
        printf("disableKludgeRescanned: %s\n", (config->disableKludgeRescanned) ? "on" : "off");
        printf("DisableTID: %s\n", (config->disableTID) ? "on" : "off");
        printf("DisablePID: %s\n", (config->disablePID) ? "on" : "off");
        printf("keepTrsMail: %s\n", (config->keepTrsMail) ? "on" : "off");
        printf("keepTrsFiles: %s\n", (config->keepTrsFiles) ? "on" : "off");
        printf("createFwdNonPass: %s\n", config->createFwdNonPass ? "on" : "off");
#if defined (__NT__)
        printf("SetConsoleTitle: %s\n", (config->setConsoleTitle) ? "on" : "off");
#endif

        if(config->processPkt != NULL)
        {
            printf("processPkt: %s\n", config->processPkt);
        }

        if(config->tossingExt != NULL)
        {
            printf("tossingExt: %s\n", config->tossingExt);
        }

        if(config->seqDir != NULL)
        {
            printf("seqDir: %s\n", config->seqDir);
        }

        if(config->seqOutrun != 0)
        {
            printSeqOutrun(config->seqOutrun);
        }

        if(config->addToSeenCount)
        {
            printf("AddToSeen:");

            for(i = 0; i < config->addToSeenCount; i++)
            {
                printf(" %u/%u", config->addToSeen[i].net, config->addToSeen[i].node);
            }
            printf("\n");
        }

        if(config->ignoreSeenCount)
        {
            printf("IgnoreSeen:");

            for(i = 0; i < config->ignoreSeenCount; i++)
            {
                printf(" %u/%u", config->ignoreSeen[i].net, config->ignoreSeen[i].node);
            }
            printf("\n");
        }

        if(config->tearline || config->origin)
        {
            printf("\n");
        }

        if(config->tearline)
        {
            printf("--- %s\n", config->tearline);
        }

        if(config->origin)
        {
            printf("* Origin: %s (%s)\n", config->origin, aka2str(&config->addr[0]));
        }

        printf("AutoPassive: %s\n", config->autoPassive ? "on" : "off");
        printf("sortEchoList: %s\n", printListEcho(config->listEcho));
        printf("packNetMailOnScan: %s\n", config->packNetMailOnScan ? "on" : "off");
        printf("NotValidFileNameChars: %s\n",
               config->notValidFNChars ? config->notValidFNChars : "\"*/:;<=>?\\|%`'&+");
        printf("\n=== AREAFIX CONFIG ===\n");
        printf("areafixFromPkt: %s\n", (config->areafixFromPkt) ? "on" : "off");
        printf("RobotsArea: %s\n", (config->robotsArea) ? config->robotsArea : "all areas");

        if(config->ReportTo)
        {
            printf("ReportTo: %s\n", config->ReportTo);
        }

        printf("ReportRequester: %s\n", config->reportRequester ? "on" : "off");

        if(hpt == 0)
        {
            printf("\n=== TICKER CONFIG ===\n");
            printf("fileDescription: %s\n", config->fileDescription);

            /* not used
             * if (config->fileAreasLog) printf("FileAreasLog: %s\n", config->fileAreasLog);
             * if (config->fileNewAreasLog) printf("FileNewAreasLog: %s\n",
             *config->fileNewAreasLog);
             * if (config->fileArcList) printf("FileArcList: %s\n", config->fileArcList);
             * if (config->filePassList) printf("FileArcList: %s\n", config->filePassList);
             * if (config->fileDupeList) printf("FileArcList: %s\n", config->fileDupeList);
             */
            /* not used
             * printf("FileSingleDescLine: %s\n",(config->fileSingleDescLine) ? "on": "off");
             * printf("FileCheckDest: %s\n",(config->fileCheckDest) ? "on": "off");
             */
            if(config->fDescNameCount)
            {
                for(i = 0; i < config->fDescNameCount; i++)
                {
                    printf("FileDescName: %s\n", config->fileDescNames[i]);
                }
            }
            else
            {
                printf("FileDescName: off\n");
            }

            printf("AddDLC: %s\n", (config->addDLC) ? "on" : "off");
            printf("FileDescPos: %u\n", config->fileDescPos);

            if(config->fileLDescString)
            {
                printf("FileLDescString: %s\n", config->fileLDescString);
            }

            printf("DLCDigits: %u\n", config->DLCDigits);

            /* not used
             * printf("FileMaxDupeAge: %u\n", config->fileMaxDupeAge);
             * printf("FileFileUMask: %o\n", config->fileFileUMask);
             * printf("FileDirUMask: %o\n", config->fileDirUMask);
             * if (config->fileLocalPwd) printf("FileLocalPwd: %s\n", config->fileLocalPwd);
             */
            if(config->saveTicCount)
            {
                for(i = 0; i < config->saveTicCount; i++)
                {
                    printf("SaveTic for %s in %s\n",
                           config->saveTic[i].fileAreaNameMask,
                           config->saveTic[i].pathName);
                }
            }

            printf("\n=== FILE ANNOUNCER CONFIG ===\n");

            if(config->announceSpool)
            {
                printf("AnnounceSpool: %s\n", config->announceSpool);
            }

            if(config->ADCount)
            {
                for(i = 0; i < config->ADCount; i++)
                {
                    printf("\n----- announce group -----\n");

                    if(config->AnnDefs[i].annAreaTag)
                    {
                        printf("AnnAreaTag: %s\n", config->AnnDefs[i].annAreaTag);
                    }

                    if(config->AnnDefs[i].annInclude == NULL)
                    {
                        printf("AnnInclude: *\n");
                    }
                    else
                    {
                        printf("AnnInclude:");

                        for(j = 0; j < config->AnnDefs[i].numbI; j++)
                        {
                            printf(" %s", config->AnnDefs[i].annInclude[j]);
                        }
                        printf("\n");
                    }

                    if(config->AnnDefs[i].annExclude != NULL)
                    {
                        printf("AnnExclude:");

                        for(j = 0; j < config->AnnDefs[i].numbE; j++)
                        {
                            printf(" %s", config->AnnDefs[i].annExclude[j]);
                        }
                        printf("\n");
                    }

                    if(config->AnnDefs[i].annto)
                    {
                        printf("AnnTo     : %s\n", config->AnnDefs[i].annto);
                    }

                    if(config->AnnDefs[i].annfrom)
                    {
                        printf("AnnFrom   : %s\n", config->AnnDefs[i].annfrom);
                    }

                    if(config->AnnDefs[i].annaddrto)
                    {
                        printf("AnnAddrTo : ");
                        printAddr(config->AnnDefs[i].annaddrto);
                        printf("\n");
                    }

                    if(config->AnnDefs[i].annaddrfrom)
                    {
                        printf("AnnAddrFrom: ");
                        printAddr(config->AnnDefs[i].annaddrfrom);
                        printf("\n");
                    }

                    if(config->AnnDefs[i].annsubj)
                    {
                        printf("AnnSubj   : %s\n", config->AnnDefs[i].annsubj);
                    }

                    if(config->AnnDefs[i].annorigin)
                    {
                        printf("AnnOrigin : %s\n          \" * Origin: %s (",
                               config->AnnDefs[i].annorigin,
                               config->AnnDefs[i].annorigin);
                        printAddr(config->AnnDefs[i].annaddrfrom);
                        printf(")\"\n");
                    }

                    if(config->AnnDefs[i].annorigin)
                    {
                        printf("AnnMessFlags : %s\n", config->AnnDefs[i].annmessflags);
                    }

                    printf("AnnFileOrigin: %s\n", config->AnnDefs[i].annforigin ? "on" : "off");
                    printf("AnnFileRFrom : %s\n", config->AnnDefs[i].annfrfrom ? "on" : "off");
                }
            }
        }

        printf("\n=== FILELIST CONFIG ===\n");

        for(i = 0; i < config->filelistCount; i++)
        {
            printFilelist(&(config->filelists[i]));
        }
        printf("\n=== LINKER CONFIG ===\n");

        switch(config->LinkWithImportlog)
        {
            case lwiYes:
                printf("LinkWithImportlog   Yes\n");
                break;

            case lwiNo:
                printf("LinkWithImportlog   No\n");
                break;

            case lwiKill:
                printf("LinkWithImportlog   Kill\n");
                break;

            default:
                printf("Internal error: Unknown value #%d for LinkWithImportLog!\n",
                       config->LinkWithImportlog);
        }
        printf("\n=== ROBOT PARAMETERS ===\n");
        printf("%u robots in config\n", config->robotCount);

        for(i = 0; i < config->robotCount; i++)
        {
            printRobot(config->robot[i]);
        }
        printf("\n=== LINK CONFIG ===\n");
        printf("%u links in config\n", config->linkCount);

        for(i = 0; i < config->linkCount; i++)
        {
            printLink(config->links[i]);
        }
        printf("\n=== AREA GROUPS ===\n");

        for(i = 0; i < config->groupCount; i++)
        {
            printf("  %s - %s\n", config->group[i].name, config->group[i].desc);
        }
        printf("\n=== AREA CONFIG ===\n");
        printf("kludgeAreaNetmail ");

        switch(config->kludgeAreaNetmail)
        {
            case kanKill:
                printf("kill");
                break;

            case kanIgnore:
                printf("ignore");
                break;

            case kanEcho:
                printf("echomail");
                break;

            default:
                break;
        }
        printf("\n");
        printf("\n=== NetMailAreas ===\n");

        for(i = 0; i < config->netMailAreaCount; i++)
        {
            printArea(config->netMailAreas[i]);
        }
        printf("\n=== DupeMailArea ===\n");

        if(config->dupeArea.areaName)
        {
            printArea(config->dupeArea);
        }

        printf("\n=== BadMailArea ===\n");

        if(config->badArea.areaName)
        {
            printArea(config->badArea);
        }

        printf("\n=== AreaGroups ===\n");
        tree_trav(&groupTree, &printAreaGroup);
        printf("\n==================\n");
        printf("\n=== EchoAreas ===\n");

        for(i = 0; i < config->echoAreaCount; i++)
        {
            printArea(config->echoAreas[i]);
        }
        printf("\n=== LocalAreas ===\n");

        for(i = 0; i < config->localAreaCount; i++)
        {
            printArea(config->localAreas[i]);
        }

        if(hpt == 0)
        {
            printf("\n=== FileAreas ===\n");

            for(i = 0; i < config->fileAreaCount; i++)
            {
                printFileArea(config->fileAreas[i]);
            }
            printf("\n=== BbsAreas ===\n");

            for(i = 0; i < config->bbsAreaCount; i++)
            {
                printBbsArea(config->bbsAreas[i]);
            }
        }

        if(config->carbonCount)
        {
            printCarbons(config);
        }

        if(config->remapCount)
        {
            printRemaps(config);
        }

        printf("\n=== ROUTE CONFIG ===\n");

        for(i = 0; i < config->routeCount; i++)
        {
            if(config->route[i].routeVia == 0)
            {
                printf("Route %s via", config->route[i].pattern);
                printAddr(&(config->route[i].target->hisAka));
            }
            else
            {
                printf("Route");

                switch(config->route[i].id)
                {
                    case id_route:
                        break;

                    case id_routeMail:
                        printf("Mail");
                        break;

                    case id_routeFile:
                        printf("File");
                        break;
                }
                printf(" %s ", config->route[i].pattern);

                switch(config->route[i].routeVia)
                {
                    case route_zero:
                        printf("zero ");
                        break;

                    case noroute:
                        printf("direct ");
                        break;

                    case nopack:
                        printf("nopack ");
                        break;

                    case host:
                        printf("via host");
                        break;

                    case hub:
                        printf("via hub  ");
                        break;

                    case boss:
                        printf("via boss ");
                        break;

                    case route_extern: /* internal only */
                    default:
                        break;
                } /* switch */
            }

            if(config->route[i].routeVia != nopack)
            {
                printf("(flavour: %s)\n", cvtFlavour(config->route[i].flavour));
            }
        }

        if(hpt == 0)
        {
            printf("\n=== NODELIST CONFIG ===\n");

            if(config->nodelistDir != NULL)
            {
                printf("NodelistDir: %s\n", config->nodelistDir);
            }

            if(config->fidoUserList != NULL)
            {
                printf("Fidouser List File: %s\n", config->fidoUserList);
            }

            printf("-------\n");

            for(i = 0; i < config->nodelistCount; i++)
            {
                printf("Nodelist %s\n", config->nodelists[i].nodelistName);

                if(config->nodelists[i].diffUpdateStem != NULL)
                {
                    printf("Nodediff Update File %s\n", config->nodelists[i].diffUpdateStem);
                }

                printf("DelAppliedDiff: %s\n",
                       (config->nodelists[i].delAppliedDiff) ? "on" : "off");

                if(config->nodelists[i].fullUpdateStem != NULL)
                {
                    printf("Full Nodelist Update File %s\n", config->nodelists[i].fullUpdateStem);
                }

                printf("Dailynodelist: %s\n",
                       (config->nodelists[i].dailynodelist) ? "on" : "off");

                if(config->nodelists[i].defaultZone != 0)
                {
                    printf("Zone Number %d\n", config->nodelists[i].defaultZone);
                }

                switch(config->nodelists[i].format)
                {
                    case fts5000:
                        printf("Standard nodelist format\n");
                        break;

                    case points24:
                        printf("Points24 nodelist format\n");
                        break;

                    case points4d:
                        printf("Points4D nodelist format\n");
                        break;

                    default:
                        printf("Unknown nodelist format???\n");
                        break;
                }
                printf("-------\n");
            }
        }

        printf("\n=== PACK CONFIG ===\n");

        for(i = 0; i < config->packCount; i++)
        {
            printf("Packer: %s      Call: %s\n", config->pack[i].packer, config->pack[i].call);
        }

        if(config->defarcmailSize != 0)
        {
            printf("\nDefault Arcmail Size - %u kb\n", config->defarcmailSize);
        }

        printf("\n=== UNPACK CONFIG ===\n");

        for(i = 0; i < config->unpackCount; i++)
        {
            printf("UnPacker:  Call: %s Offset %d Match code ",
                   config->unpack[i].call,
                   config->unpack[i].offset);

            for(k = 0; k < config->unpack[i].codeSize; k++)
            {
                printf("%02x", (int)config->unpack[i].matchCode[k]);
            }
            printf(" Mask : ");

            for(k = 0; k < config->unpack[i].codeSize; k++)
            {
                printf("%02x", (int)config->unpack[i].mask[k]);
            }
            printf("\n");
        }

        if(config->beforePack)
        {
            printf("Before Pack - \"%s\"\n", config->beforePack);
        }

        if(config->afterUnpack)
        {
            printf("After Unpack - \"%s\"\n", config->afterUnpack);
        }

        if(hpt == 0)
        {
            printf("\n=== EXEC CONFIG ===\n");

            for(i = 0; i < config->execonfileCount; i++)
            {
                printf("ExecOnFile: Area %s File %s Call %s\n",
                       config->execonfile[i].filearea,
                       config->execonfile[i].filename,
                       config->execonfile[i].command);
            }
        }

        printf("\n=== EMAILPKT CONFIG ===\n");

        if(config->sendmailcmd)
        {
            printf("sendMailCmd: %s\n", config->sendmailcmd);
        }
        else
        {
            printf("sendMailCmd:\n");
        }

        printf("\n");
        rc += testPathsAndFiles();

        if(rc)
        {
            puts("============================");
            checkLogic(config);
            testConfig(config);
            puts("============================");
        }

        disposeConfig(config);

        if(rc)
        {
            fprintf(stderr, "Attention, %u errors found!\n", rc);
        }
    }                           /* endif */

    return rc;
} /* main */
