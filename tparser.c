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
   printf("-> %s\t", area.fileName);
   if (area.msgbType == MSGTYPE_SDM) printf("SDM");  
   else if (area.msgbType == MSGTYPE_SQUISH) printf("Squish");
   else printf("Passthrough");

   if (area.useAka->domain != NULL)
     printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   else
     printf("\t Use %d:%d/%d.%d", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point);
   printf("\n");
   printf("Level read  - %d\n", area.levelread);
   printf("Level write - %d\n", area.levelwrite);
   printf("Group       - %c\n", area.group);
   printf("max: %u msgs\tpurge: %u days\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   if (area.downlinkCount) printf("Links:");
   else printf("No links\n");
   for (i = 0; i<area.downlinkCount;i++) { 
       printf("\t");
       printAddr(area.downlinks[i]->link->hisAka);
       printf(" level %d,", area.downlinks[i]->link->level);
       if (area.downlinks[i]->export) printf(" export on,");
       else printf(" export off,");
       if (area.downlinks[i]->import) printf(" import on,");
       else printf(" import off,");
       if (area.downlinks[i]->mandatory) printf(" mandatory on.");
       else printf(" mandatory off.");
       printf("\n");
   }
   printf("Options: ");
   if (area.manual) printf("manual ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   if (area.tinySB) printf("tinySB ");
   if (area.mandatory) printf("mandatory ");
   printf("\n");
   printf("DupeCheck: ");
   if (area.dupeCheck==dcOff) printf("off");
   if (area.dupeCheck==dcMove) printf("move");
   if (area.dupeCheck==dcDel) printf("delete");
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
   printf("defaultPwd: %s\n", link.defaultPwd);
   printf("pktPwd:     %s\n", link.pktPwd);
   printf("ticPwd:     %s\n", link.ticPwd);
   printf("areafixPwd: %s\n", link.areaFixPwd);
   printf("filefixPwd: %s\n", link.fileFixPwd);
   printf("bbsPwd:     %s\n", link.bbsPwd);
   printf("Level:      %u\n", link.level);
   if (link.export) {
       printf("Export:     ");
       if (link.export[0] == 1) printf("on\n");
       else printf("off\n");
   }
   if (link.import) {
       printf("Import:     ");
       if (link.import[0] == 1) printf("on\n");
       else printf("off\n");
   }
   if (link.mandatory) {
       printf("Mandatory   ");
       if (link.mandatory[0] == 1) printf("on\n");
       else printf("off\n");
   }
   if (link.optGrp) printf("OptGrp       %s\n", link.optGrp);
   if (link.autoCreateFile) printf("AutoCreateFile: %s\n", link.autoCreateFile);
   if (link.LinkGrp) printf("LinkGrp %s\n",link.LinkGrp);
   if (link.AccessGrp) printf("AccessGrp %s\n",link.AccessGrp);
   if (link.autoAreaCreate) printf("AutoAreaCreate on\n");
   if (link.AreaFix) printf("AreaFix on\n"); else printf("AreaFix off\n");
   if (link.forwardRequests) printf("Forward Requests on\n");
   if (link.forwardRequestFile) printf("ForwardRequestFile %s\n",link.forwardRequestFile);
   if (link.packerDef != NULL) printf("PackerDefault %s\n", link.packerDef->packer);
   else printf("PackerDefault none\n");
   printf("forwardPkts ");
   switch (link.forwardPkts){
   case fOff : printf("off\n");
   case fSecure : printf("secure\n");
   case fOn : printf("on\n");
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
      if (config->inbound != NULL) printf("Inbound: %s\n", config->inbound);
      if (config->protInbound != NULL) printf("ProtInbound: %s\n", config->protInbound);
      if (config->localInbound != NULL) printf("LocalInbound: %s\n", config->localInbound);
      if (config->listInbound != NULL) printf("ListInbound: %s\n", config->listInbound);
      if (config->outbound != NULL) printf("Outbound: %s\n", config->outbound);
      if (config->tempOutbound != NULL) printf("tempOutbound: %s\n", config->tempOutbound);
      for (i=0; i< config->publicCount; i++) printf("Public: #%u %s\n", i+1, config->publicDir[i]);
      if (config->nodelistDir != NULL) printf("NodelistDir: %s\n", config->nodelistDir);
      if (config->dupeHistoryDir != NULL) printf("DupeHistoryDir: %s\n", config->dupeHistoryDir);
      if (config->logFileDir != NULL) printf("LogFileDir: %s\n", config->logFileDir);
      if (config->msgBaseDir != NULL) printf("MsgBaseDir: %s\n", config->msgBaseDir);
      if (config->magic != NULL) printf("Magic: %s\n", config->magic);
      printf("\n=== AREAFIX CONFIG ===\n");
	  printf("areafixFromPkt: ");
	  if (config->areafixFromPkt) printf("on\n"); else printf("off\n");
	  printf("areafixKillReports: ");
	  if (config->areafixKillReports) printf("on\n"); else printf("off\n");
      printf("\n=== LINKER CONFIG ===\n");
      if (config->LinkWithImportlog != NULL) printf("LinkWithImportlog: %s\n", config->LinkWithImportlog);
      printf("\n=== LINK CONFIG ===\n");
      printf("%u links in config\n", config->linkCount);
      for (i = 0; i < config->linkCount; i++) printLink(config->links[i]);
      
      printf("\n=== AREA CONFIG ===\n");
      if (config->netMailArea.areaName != NULL) printArea(config->netMailArea);
	else { printf("you must define NetmailArea!\n"); return 1; }
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
      printf("\n=== CarbonCopy ===\n");
      for (i = 0; i< config->carbonCount; i++) {
		  if (config->carbons[i].type == to)     printf("CarbonTo:     ");
		  if (config->carbons[i].type == from)   printf("CarbonFrom:   ");
		  if (config->carbons[i].type == kludge) printf("CarbonKludge: ");
		  printf("%s\n",config->carbons[i].str);
		  printf("CarbonArea:   %s\n",config->carbons[i].area->areaName);
		  printf("-------\n");
      }
	  printf("\nWarning! After each Carbon(to|from|kludge) write CarbonArea in config.\n");


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

      printf("\n=== PACK CONFIG ===\n");
      for (i = 0; i < config->packCount; i++) {
         printf("Packer: %s      Call: %s\n", config->pack[i].packer, config->pack[i].call);
      }
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
      disposeConfig(config);
   } /* endif */
   return 0;
}
