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

#if !defined(MSDOS) || defined(__DJGPP__)
#include "fidoconfig.h"
#else
#include "fidoconf.h"
#endif

void printAddr(const s_addr addr)
{
  if (addr.domain != NULL)
    printf("%d:%d/%d.%d@%s ", addr.zone, addr.net, addr.node, addr.point, addr.domain);
  else
    printf("%d:%d/%d.%d ", addr.zone, addr.net, addr.node, addr.point);
}

void printArea(s_area area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("Description: ");
   if (area.description != NULL)
     printf("%s",area.description);
   printf("\n-> %s\t", area.fileName ? area.fileName : "");
   if (area.msgbType == MSGTYPE_SDM) printf("SDM");  
   else if (area.msgbType == MSGTYPE_SQUISH) printf("Squish");
   else if (area.msgbType == MSGTYPE_JAM) printf("Jam");
   else printf("Passthrough");

   if (area.useAka->domain != NULL)
     printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   else
     printf("\t Use %d:%d/%d.%d", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point);
   printf("\n");
   printf("DOS Style File (8+3) - %s\n", (area.DOSFile) ? "on" : "off");
   printf("Level read  - %d\n", area.levelread);
   printf("Level write - %d\n", area.levelwrite);
   printf("Group       - %s\n", area.group);
   if (area.nopack) {
      printf("pack never (ignoging: max: %u msgs\tpurge: %u days)\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   } else {
      printf("max: %u msgs\tpurge: %u days\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   }
   if (area.downlinkCount) printf("Links:");
   else printf("No links\n");
   for (i = 0; i<area.downlinkCount;i++) { 
       printf("\t");
       printAddr(area.downlinks[i]->link->hisAka);
       printf(" level %d,", area.downlinks[i]->link->level);
       printf(" export %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" import %s,", (area.downlinks[i]->import) ? "on" : "off");
       printf(" mandatory %s.\n", (area.downlinks[i]->mandatory) ? "on" : "off");
   }
   printf("Options: ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   if (area.tinySB) printf("tinySB ");
   if (area.mandatory) printf("mandatory ");
   if (area.ccoff) printf("ccoff ");
   if (area.keepsb) printf("keepsb ");
   if (area.killRead) printf("killRead ");
   if (area.keepUnread) printf("keepUnread ");
   printf("\n");
   printf("DupeCheck: ");
   if (area.dupeCheck==dcOff) printf("off");
   if (area.dupeCheck==dcMove) printf("move");
   if (area.dupeCheck==dcDel) printf("delete");
   printf("\n");
   printf("-------\n");
}

void printFileArea(s_filearea area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("Description: %s\n",area.description);
   if (area.pass != 1)
      printf("Path: %s\t", area.pathName);
   else
      printf("Passthrough filearea");

   if (area.useAka->domain != NULL)
     printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   else
     printf("\t Use %d:%d/%d.%d", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point);
   printf("\n");
   printf("Group       - %s\n", area.group);
   if (area.downlinkCount) printf("Links:");
   else printf("No links\n");
   for (i = 0; i<area.downlinkCount;i++) { 
       printf("\t");
       printAddr(area.downlinks[i]->link->hisAka);
       printf(" level %d,", area.downlinks[i]->link->level);
       printf(" export %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" import %s,", (area.downlinks[i]->import) ? "on" : "off");
       printf(" mandatory %s.\n", (area.downlinks[i]->mandatory) ? "on" : "off");
   }
   printf("Options: ");
   if (area.mandatory) printf("mandatory ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   printf("\n");
   printf("-------\n");
}

void printBbsArea(s_bbsarea area) {
   
   printf("%s \n", area.areaName);
   printf("Description: %s\n",area.description);
   printf("Path: %s\t", area.pathName);

   printf("\n");
   printf("-------\n");
}

void printLink(s_link link) {
  if ((link.hisAka.domain != NULL) && (link.ourAka->domain != NULL)) {
    printf("Link: %d:%d/%d.%d@%s (ourAddres %d:%d/%d.%d@%s)\n",
	   link.hisAka.zone, link.hisAka.net, link.hisAka.node, link.hisAka.point, link.hisAka.domain,
	   link.ourAka->zone, link.ourAka->net, link.ourAka->node, link.ourAka->point, link.ourAka->domain);
  }
    else
    {
      printf("Link: %d:%d/%d.%d (ourAddres %d:%d/%d.%d)\n",
	   link.hisAka.zone, link.hisAka.net, link.hisAka.node, link.hisAka.point,
	   link.ourAka->zone, link.ourAka->net, link.ourAka->node, link.ourAka->point);
    }
   printf("Name: %s\n", link.name);
   if (link.defaultPwd) printf("defaultPwd: %s\n", link.defaultPwd);
   if (link.pktPwd) printf("pktPwd:     %s\n", link.pktPwd);
   if (link.ticPwd) printf("ticPwd:     %s\n", link.ticPwd);
   if (link.areaFixPwd) printf("areafixPwd: %s\n", link.areaFixPwd);
   if (link.fileFixPwd) printf("filefixPwd: %s\n", link.fileFixPwd);
   if (link.bbsPwd) printf("bbsPwd:     %s\n", link.bbsPwd);
   if (link.sessionPwd) printf("sessionPwd: %s\n", link.sessionPwd);
   if (link.handle!=link.name) printf("handle:     %s\n", link.handle);
   if (link.email) printf("email:      %s\n", link.email);
   printf("Level:      %u\n", link.level);
   printf("Export:     %s\n",(link.export) ? "on" : "off");
   printf("Import:     %s\n",(link.import) ? "on" : "off");
   printf("Mandatory:  %s\n",(link.mandatory) ? "on" : "off");
   if (link.Pause) printf("Link in Pause, no export\n");
   if (link.autoPause) printf("AutoPause over %u days\n", link.autoPause);
   if (link.numOptGrp > 0)
   {
     unsigned int i;

     printf("OptGrp       ");
     for (i = 0; i < link.numOptGrp; i++)
     {
       if (i > 0) printf(",%s", link.optGrp[i]);
       else printf("%s", link.optGrp[0]);
     }
     printf("\n");
   }
   printf("AutoAreaCreate %s\n", (link.autoAreaCreate) ? "on" : "off");
   if (link.autoAreaCreateFile) printf("AutoAreaCreateFile: %s\n", link.autoAreaCreateFile);
   if (link.autoAreaCreateDefaults) printf("AutoAreaCreateDefaults: %s\n", link.autoAreaCreateDefaults);
   printf("AutoFileCreate %s\n", (link.autoFileCreate) ? "on" : "off");
   if (link.autoFileCreateFile) printf("AutoFileCreateFile: %s\n", link.autoFileCreateFile);
   if (link.autoFileCreateDefaults) printf("AutoFileCreateDefaults: %s\n", link.autoFileCreateDefaults);
   if (link.LinkGrp) printf("LinkGrp %s\n",link.LinkGrp);
   if (link.numAccessGrp)
   {
     unsigned int i;

     printf("AccessGrp ");
     for (i = 0; i < link.numAccessGrp; i++)
     {
       if (i > 0) printf(", %s", link.AccessGrp[i]);
       else printf("%s", link.AccessGrp[0]);
     }
     printf("\n");
   }
   printf("AreaFix %s\n", (link.AreaFix) ? "on" : "off");
   printf("FileFix %s\n", (link.FileFix) ? "on" : "off");
   printf("Forward Requests from this link is %s\n",(link.forwardRequests)?"on":"off");
   printf("Forward Request to another links is %s\n",(link.fReqFromUpLink)?"on":"off");
   if (link.RemoteRobotName) printf("RemoteRobotName %s\n", link.RemoteRobotName);
   else printf("RemoteRobotName AreaFix\n");
   if (link.forwardRequestFile) printf("ForwardRequestFile %s\n",link.forwardRequestFile);
   if (link.packerDef) printf("PackerDefault %s\n", link.packerDef->packer);
   else printf("PackerDefault none\n");
   if (link.pktSize != 0) printf("pktSize - %u kb\n",link.pktSize);
   if (link.arcmailSize != 0) printf("arcmailSize - %u kb\n",link.arcmailSize);
   printf("TIC files %s\n", (link.noTIC == 0) ? "on" : "off");
   printf("forwardPkts ");
   switch (link.forwardPkts){
   case fOff : printf("off\n");
     break;
   case fSecure : printf("secure\n");
     break;
   case fOn : printf("on\n");
     break;
   }
   printf("allowEmptyPktPwd ");
   switch (link.allowEmptyPktPwd){
   case eOff : printf("off\n");
   break;
   case eSecure : printf("secure\n");
     break;
   case eOn : printf("on\n");
     break;
   }
   printf("allowPktAddrDiffer ");
   switch (link.allowPktAddrDiffer) {
   case pdOff : printf("off\n");
     break;
   case pdOn : printf("on\n");
     break;
   default: fprintf(stderr, "Error in keyword allowPktAddrDiffer\n");
   }
   
   printf("-------\n");
}

int main() {
   s_fidoconfig *config = readConfig();
   int i, j;

   if (config != NULL) {
      printf("=== MAIN CONFIG ===\n");
      printf("Version: %u.%u\n", config->cfgVersionMajor, config->cfgVersionMinor);
      if (config->name != NULL)	printf("Name: %s\n", config->name);
      if (config->sysop != NULL) printf("Sysop: %s\n", config->sysop);
      if (config->location != NULL)printf("Location: %s\n", config->location);
      for (i=0; i<config->addrCount; i++) {
	 if (config->addr[i].domain != NULL)
            printf("Addr: %u:%u/%u.%u@%s\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point, config->addr[i].domain);
	 else
            printf("Addr: %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      }

      printf("LogEchoToScreen %s\n", (config->logEchoToScreen) ? "on" : "off");

      if (config->inbound != NULL) printf("Inbound: %s\n", config->inbound);
      if (config->tempInbound != NULL) printf("tempInbound: %s\n", config->tempInbound);
      if (config->protInbound != NULL) printf("ProtInbound: %s\n", config->protInbound);
      if (config->localInbound != NULL) printf("LocalInbound: %s\n", config->localInbound);
      if (config->listInbound != NULL) printf("ListInbound: %s\n", config->listInbound);
      if (config->ticOutbound != NULL) printf("TicOutbound: %s\n", config->ticOutbound);
      if (config->outbound != NULL) printf("Outbound: %s\n", config->outbound);
      if (config->tempOutbound != NULL) printf("tempOutbound: %s\n", config->tempOutbound);
      for (i=0; i< config->publicCount; i++) printf("Public: #%u %s\n", i+1, config->publicDir[i]);
      if (config->dupeHistoryDir != NULL) printf("DupeHistoryDir: %s\n", config->dupeHistoryDir);
      if (config->logFileDir != NULL) printf("LogFileDir: %s\n", config->logFileDir);
      if (config->msgBaseDir != NULL) printf("MsgBaseDir: %s\n", config->msgBaseDir);
      if (config->fileAreaBaseDir) printf("FileAreaBaseDir: %s\n", config->fileAreaBaseDir);
      if (config->passFileAreaDir) printf("passFileAreaDir: %s\n", config->passFileAreaDir);
      if (config->magic) printf("Magic: %s\n", config->magic);
      if (config->semaDir) printf("semaDir: %s\n", config->semaDir);
      if (config->badFilesDir) printf("BadFilesDir: %s\n", config->badFilesDir);
      if (config->msgidfile) printf("MsgIDFile: %s\n", config->msgidfile);
//      printf("CreateDirs: %s\n",(config->createDirs) ? "on": "off");
      printf("LongDirNames: %s\n",(config->longDirNames) ? "on": "off");
      printf("SplitDirs: %s\n",(config->splitDirs) ? "on": "off");

      printf("Ignore Capability Word: %s\n",(config->ignoreCapWord) ? "on": "off");
      printf("ProcessBundles %s\n",(config->noProcessBundles) ? "off" : "on");
      printf("DupeBaseType: ");
      if (config->typeDupeBase==textDupes) printf("textDupes\n");
      if (config->typeDupeBase==hashDupes) printf("hashDupes\n");
      if (config->typeDupeBase==hashDupesWmsgid) printf("hashDupesWmsgid\n");
      if (config->typeDupeBase==commonDupeBase) {
            printf("commonDupeBase\n");
            printf("AreasMaxDupeAge: %d\n",config->areasMaxDupeAge);
      }

      if (config->numPublicGroup > 0) {
          printf("PublicGroups: ");
          for (i = 0; i < config->numPublicGroup; i++)
              printf( (i>0) ? ",%s" : "%s", config->PublicGroup[i]);
          printf("\n");
      }

      printf("createAreasCase: %s\n", (config->createAreasCase == eLower) ? "Lower" : "Upper");
      printf("areasFileNameCase: %s\n", (config->areasFileNameCase == eLower) ? "Lower" : "Upper");

      printf("\n=== AREAFIX CONFIG ===\n");
	  printf("areafixFromPkt: %s\n",(config->areafixFromPkt) ? "on": "off");
	  printf("areafixKillReports: %s\n",(config->areafixKillReports)?"on":"off");
	  printf("areafixKillRequests: %s\n",(config->areafixKillRequests)?"on":"off");
	  if (config->areafixMsgSize) printf("areafixMsgSize - %u\n", config->areafixMsgSize);
	  if (config->areafixSplitStr) printf("areafixSplitStr - \"%s\"\n", config->areafixSplitStr);
	  if (config->areafixOrigin) printf("areafixOrigin - \"%s\"\n", config->areafixOrigin);

      printf("\n=== FILEFIX CONFIG ===\n");
	  printf("filefixKillReports: %s\n",(config->filefixKillReports)?"on":"off");
	  printf("filefixKillRequests: %s\n",(config->filefixKillRequests)?"on":"off");

      printf("\n=== TICKER CONFIG ===\n");
      if (config->fileAreasLog) printf("FileAreasLog: %s\n", config->fileAreasLog);
      if (config->fileNewAreasLog) printf("FileNewAreasLog: %s\n", config->fileNewAreasLog);
      if (config->fileArcList) printf("FileArcList: %s\n", config->fileArcList);
      if (config->filePassList) printf("FileArcList: %s\n", config->filePassList);
      if (config->fileDupeList) printf("FileArcList: %s\n", config->fileDupeList);
      printf("AddDLC: %s\n",(config->addDLC) ? "on": "off");
      printf("FileSingleDescLine: %s\n",(config->fileSingleDescLine) ? "on": "off");
      printf("FileCheckDest: %s\n",(config->fileCheckDest) ? "on": "off");
      printf("FileDescPos: %u\n", config->fileDescPos);
      if (config->fileLDescString) printf("FileLDescString: %s\n", config->fileLDescString);
      printf("DLCDigits: %u\n", config->DLCDigits);
      printf("FileMaxDupeAge: %u\n", config->fileMaxDupeAge);
      printf("FileFileUMask: %o\n", config->fileFileUMask);
      printf("FileDirUMask: %o\n", config->fileDirUMask);
      if (config->fileLocalPwd) printf("FileLocalPwd: %s\n", config->fileLocalPwd);

      printf("\n=== LINKER CONFIG ===\n");
      if (config->LinkWithImportlog != NULL) printf("LinkWithImportlog: %s\n", config->LinkWithImportlog);
      printf("\n=== LINK CONFIG ===\n");
      printf("%u links in config\n", config->linkCount);
      for (i = 0; i < config->linkCount; i++) printLink(config->links[i]);
      
      printf("\n=== AREA CONFIG ===\n");
      if (config->netMailAreaCount == 0) { printf("you must define at least one NetmailArea!\n"); return 1; }
      printf("\n=== LocalAreas ===\n");
      for (i = 0; i< config->netMailAreaCount; i++) {
         printArea(config->netMailAreas[i]);
      }
      if (config->dupeArea.areaName != NULL) printArea(config->dupeArea);
	else { printf("you must define DupeArea!\n"); return 1; }
      if (config->badArea.areaName != NULL) printArea(config->badArea);
	else { printf("you must define BadArea!\n"); return 1; }
      for (i = 0; i< config->echoAreaCount; i++) {
         printArea(config->echoAreas[i]);
      }
      printf("\n=== LocalAreas ===\n");
      for (i = 0; i< config->localAreaCount; i++) {
         printArea(config->localAreas[i]);
      }
      printf("\n=== FileAreas ===\n");
      for (i=0; i<config->fileAreaCount; i++) {
        printFileArea(config->fileAreas[i]);
      }
      printf("\n=== BbsAreas ===\n");
      for (i=0; i<config->bbsAreaCount; i++) {
        printBbsArea(config->bbsAreas[i]);
      }
      printf("\n=== CarbonCopy ===\n");
      printf("CarbonAndQuit %s\n", (config->carbonAndQuit) ? "on" : "off");
      printf("CarbonKeepSb %s\n", (config->carbonKeepSb) ? "on" : "off");
	  printf("\n");
      for (i = 0; i< config->carbonCount; i++) {
		  if (config->carbons[i].type == to)      printf("CarbonTo:     ");
		  if (config->carbons[i].type == from)    printf("CarbonFrom:   ");
		  if (config->carbons[i].type == kludge)  printf("CarbonKludge: ");
		  if (config->carbons[i].type == subject) printf("CarbonSubj:   ");
		  if (config->carbons[i].type == msgtext) printf("CarbonText:   ");
		  printf("%s\n",config->carbons[i].str);
		  if (config->carbons[i].extspawn) {
			  printf("CarbonExtern: \"%s\"", config->carbons[i].areaName);
		  } else {
                          switch (config->carbons[i].move) {
                             case 0: printf("CarbonCopy:   ");
                             break;

                             case 2: printf("CarbonDelete");
                             break;

                             case 1:
                             default: printf("CarbonMove:  ");
                             break;
                          }
                	  if (config->carbons[i].areaName) {
	                     if(stricmp(config->carbons[i].areaName, config->carbons[i].area->areaName)) {
                                printf(" !!! \"%s\" wanted !!! using \"%s\"", config->carbons[i].areaName, config->carbons[i].area->areaName);
                             } else {
                                printf("\"%s\"", config->carbons[i].area->areaName);
                             }
        	          } else {
                	     if (config->carbons[i].move != 2) printf(" !!! No area specified !!!");
	                  }
		  };
                  putchar('\n');
		  if (config->carbons[i].reason) printf("CarbonReason: %s\n", config->carbons[i].reason);
		  if (config->carbons[i].export) printf("Copied messages will be exported.\n");
		  if (config->carbons[i].netMail) printf("Active on netMail\n");
		  printf("-------\n");
      }


      printf("\n=== ROUTE CONFIG ===\n");
      for (i = 0; i < config->routeCount; i++) {
         if (config->route[i].routeVia == 0)
            printf("Route %s via %u:%u/%u.%u\n", config->route[i].pattern, config->route[i].target->hisAka.zone, config->route[i].target->hisAka.net, config->route[i].target->hisAka.node, config->route[i].target->hisAka.point);
         else {
            printf("Route %s ", config->route[i].pattern);
            switch (config->route[i].routeVia) {
               case noroute:  printf("direct\n"); break;
               case host:     printf("via host\n"); break;
               case hub:      printf("via hub\n"); break;
               case boss:     printf("via boss\n"); break;
            }
         }
      }

      printf("\n=== NODELIST CONFIG ===\n");
      if (config->nodelistDir != NULL)
        {
          printf("NodelistDir: %s\n", config->nodelistDir);
        }
      if (config->fidoUserList != NULL)
        {
          printf("Fidouser List File: %s\n", config->fidoUserList);
        }
      printf("-------\n");

      for (i = 0; i < config->nodelistCount; i++)
        {
          printf("Nodelist %s\n", config->nodelists[i].nodelistName);
          if (config->nodelists[i].diffUpdateStem != NULL)
            printf("Nodediff Update File %s\n",
                   config->nodelists[i].diffUpdateStem);
          if (config->nodelists[i].fullUpdateStem != NULL)
            printf("Full Nodelist Update File %s\n",
                   config->nodelists[i].fullUpdateStem);
          if (config->nodelists[i].defaultZone != 0)
            printf("Zone Number %d\n",
                   config->nodelists[i].defaultZone);
          switch (config->nodelists[i].format)
            {
            case fts5000:
              printf ("Standard nodelist format\n");
              break;
            case points24:
              printf ("Points24 nodelist format\n");
              break;
            default:
              printf ("Unknown nodelist format???\n");
              break;
            }
          printf("-------\n");
        }

      printf("\n=== PACK CONFIG ===\n");
      for (i = 0; i < config->packCount; i++) {
         printf("Packer: %s      Call: %s\n", config->pack[i].packer, config->pack[i].call);
      }
      if (config->defarcmailSize!=0) printf("\nDefault Arcmail Size - %u kb\n",config->defarcmailSize);
      printf("\n=== UNPACK CONFIG ===\n");
      for (i = 0; i < config->unpackCount; i++) {
         printf("UnPacker:  Call: %s Offset %d Match code ", config->unpack[i].call, config->unpack[i].offset);
         for (j = 0; j < config->unpack[i].codeSize; j++)
           printf("%02x", (int) config->unpack[i].matchCode[j]);
         printf(" Mask : ");
         for (j = 0; j < config->unpack[i].codeSize; j++)
           printf("%02x", (int) config->unpack[i].mask[j]);
         printf("\n");
      }
	  
      if (config->beforePack) printf("Before Pack - \"%s\"\n",config->beforePack);
      if (config->afterUnpack) printf("After Unpack - \"%s\"\n",config->afterUnpack);

      if (config->ReportTo) printf("ReportTo\t%s\n", config->ReportTo);
      printf("\n=== EXEC CONFIG ===\n");
      for (i = 0; i < config->execonfileCount; i++) {
         printf("ExecOnFile: Area %s File %s Call %s\n",
                 config->execonfile[i].filearea,
                 config->execonfile[i].filename,
                 config->execonfile[i].command);
      }
      disposeConfig(config);
   } /* endif */
   return 0;
}
