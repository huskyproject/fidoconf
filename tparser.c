#include <stdlib.h>
#include <stdio.h>

#include "fidoconfig.h"

void printArea(s_area area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("-> %s\t", area.fileName);
   if (area.msgbType == MSGTYPE_SDM) printf("SDM");  
   else if (area.msgbType == MSGTYPE_SQUISH) printf("Squish");
   else printf("Passthrough");

   printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   printf("\n");
   printf("max: %u msgs\tpurge: %u days\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   printf("Links: ");
   for (i = 0; i<area.downlinkCount;i++) printf("%u:%u/%u.%u@%s ", area.downlinks[i]->hisAka.zone, area.downlinks[i]->hisAka.net, area.downlinks[i]->hisAka.node, area.downlinks[i]->hisAka.point, area.downlinks[i]->hisAka.domain);
   printf("\n");
   printf("Options: ");
   if (area.manual) printf("manual ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   if (area.tinySB) printf("tinySB ");
   printf("\n");
   printf("DupeCheck: ");
   if (area.dupeCheck==off) printf("off");
   if (area.dupeCheck==move) printf("move");
   if (area.dupeCheck==del) printf("delete");
   printf("\n");
   printf("-------\n");
}

void printLink(s_link link) {
   printf("Link: %d:%d/%d.%d@%s (ourAddres %d:%d/%d.%d@%s)\n",
          link.hisAka.zone, link.hisAka.net, link.hisAka.node, link.hisAka.point, link.hisAka.domain,
          link.ourAka->zone, link.ourAka->net, link.ourAka->node, link.ourAka->point, link.ourAka->domain);
   printf("Name: %s\n", link.name);
   printf("defaultPwd: %s\n", link.defaultPwd);
   printf("pktPwd:     %s\n", link.pktPwd);
   printf("ticPwd:     %s\n", link.ticPwd);
   printf("areafixPwd: %s\n", link.areaFixPwd);
   printf("filefixPwd: %s\n", link.fileFixPwd);
   printf("bbsPwd:     %s\n", link.bbsPwd);
   if (link.autoAreaCreate) printf("AutoAreaCreate on    ");
   if (link.AreaFix) printf("AreaFix on\n");
   if (link.packerDef != NULL) printf("PackerDefault %s\n", link.packerDef->packer);
   else printf("PackerDefault none\n");
   
   printf("-------\n");
}

int main() {
   s_fidoconfig *config = readConfig();
   int i;

   if (config != NULL) {
      printf("=== MAIN CONFIG ===\n");
      printf("Version: %u.%u\n", config->cfgVersionMajor, config->cfgVersionMinor);
      printf("Name: %s\n", config->name);
      printf("Sysop: %s\n", config->sysop);
      printf("Location: %s\n", config->location);
      for (i=0; i<config->addrCount; i++) {
         printf("Addr: %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      }
      printf("Inbound: %s\n", config->inbound);
      printf("ProtInbound: %s\n", config->protInbound);
      printf("LocalInbound: %s\n", config->localInbound);
      printf("ListInbound: %s\n", config->listInbound);
      printf("Outbound: %s\n", config->outbound);
      for (i=0; i< config->publicCount; i++) printf("Public: #%u %s\n", i+1, config->public[i]);
      printf("NodelistDir: %s\n", config->nodelistDir);
      printf("DupeHistoryDir: %s\n", config->dupeHistoryDir);
      printf("LogFileDir: %s\n", config->logFileDir);
      printf("MsgBaseDir: %s\n", config->msgBaseDir);
      printf("Magic: %s\n", config->magic);

      printf("\n=== LINK CONFIG ===\n");
      for (i = 0; i < config->linkCount; i++) printLink(config->links[i]);
      
      printf("\n=== AREA CONFIG ===\n");
      printArea(config->netMailArea);
      printArea(config->dupeArea);
      printArea(config->badArea);
      for (i = 0; i< config->echoAreaCount; i++) {
         printArea(config->echoAreas[i]);
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

      printf("\n=== PACK CONFIG ===\n");
      for (i = 0; i < config->packCount; i++) {
         printf("Packer: %s      Call: %s\n", config->pack[i].packer, config->pack[i].call);
      }
      disposeConfig(config);
   } /* endif */
   return 0;
}
