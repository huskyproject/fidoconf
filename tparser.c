#include <stdlib.h>
#include <stdio.h>

#include "fidoconfig.h"

void printArea(s_area area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("-> %s\t", area.fileName);
   if (area.msgbType == MSGTYPE_SDM) printf("SDM"); else printf("Squish");
   printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   printf("\n");
   printf("max: %u msgs\tpurge: %u days\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   printf("Links: ");
   for (i = 0; i<area.downlinkCount;i++) printf("%u:%u/%u.%u@%s ", area.downlinks[i]->hisAka.zone, area.downlinks[i]->hisAka.net, area.downlinks[i]->hisAka.node, area.downlinks[i]->hisAka.point, area.downlinks[i]->hisAka.domain);
   printf("\n");
   printf("Options: ");
   if (area.noDC) printf("noDC ");
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
      printf("Magic: %s\n", config->magic);
      
      printf("\n=== AREA CONFIG ===\n");
      printArea(config->netMailArea);
      printArea(config->dupeArea);
      printArea(config->badArea);
      for (i = 0; i< config->echoAreaCount; i++) {
         printArea(config->echoAreas[i]);
      }
      for (i = 0; i < config->routeCount; i++) {
         printf("Route %s via %u:%u/%u.%u\n", config->route[i].pattern, config->route[i].target->hisAka.zone, config->route[i].target->hisAka.net, config->route[i].target->hisAka.node, config->route[i].target->hisAka.point);

      }
      disposeConfig(config);
   } /* endif */
   return 0;
}