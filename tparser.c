#include <stdlib.h>
#include <stdio.h>

#include "fidoconfig.h"

int main() {
   s_fidoconfig *config = readConfig();
   int i;

   if (config != NULL) {
      printf("Version: %u.%u\n", config->cfgVersionMajor, config->cfgVersionMinor);
      printf("Name: %s\n", config->name);
      printf("Sysop: %s\n", config->sysop);
      printf("Location: %s\n", config->location);
      for (i=0; i<config->addrCount; i++) {
         printf("Addr: %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      }
      printf("Inbound: %s\n", config->inbound);
      disposeConfig(config);
   } /* endif */
   return 0;
}