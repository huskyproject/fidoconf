#include <stdlib.h>

#include "fidoconfig.h"

int writeArea(FILE *f, s_area *area, char type) {

   if (area->group == 0) area->group = '0';

   fprintf(f, "areadef %s \"%s\" %c ", area->areaName, area->areaName, area->group);

   switch (type) {
      case 0: fprintf(f, "local ");
              break;
      case 1: fprintf(f, "net ");
              break;
      case 2: fprintf(f, "local ");
              break;
   }

   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, "Squish ");
   else fprintf(f, "Opus ");

   fprintf(f, "%s ", area->fileName);

   fprintf(f, "%u:%u/%u.%u", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   
   fprintf(f, "\n");

   return 0;
}

int generateAquaedConfig(s_fidoconfig *config, char *fileName, char *includeFile) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "w");
   if (f!= NULL) {

      if (includeFile != NULL) fprintf(f, "include %s\n\n", includeFile);

      fprintf(f, "username %s\n\n", config->sysop);

      if (config->echotosslog != NULL) fprintf(f, "squishechotoss %s\n", config->echotosslog);
                                               
      for (i=0; i<config->addrCount; i++)
         fprintf(f, "Address %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      fprintf(f, "\n");

      writeArea(f, &(config->netMailArea), 1);
      writeArea(f, &(config->dupeArea), 1);
      writeArea(f, &(config->badArea), 1);

      for (i=0; i<config->echoAreaCount; i++) {
         area = &(config->echoAreas[i]);
         if (area->msgbType != MSGTYPE_PASSTHROUGH)
             writeArea(f, area, 0);
      }

      for (i=0; i<config->localAreaCount; i++) {
         area = &(config->localAreas[i]);
         if (area->msgbType != MSGTYPE_PASSTHROUGH)
             writeArea(f, area, 2);
      }
      
      return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   
   printf("fconf2aquaed\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2aquaed <aqauedConfigFileName> [<default.cfg>]\n");
      printf("   (you may include config defaults from default.cfg)\n");
      printf("\nExample:\n");
      printf("   fconf2aquaed ~/aquaed/aquaed.cfg\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[1]);

   config = readConfig();
   if (config!= NULL) {

      if (argc == 3)
         generateAquaedConfig(config, argv[1], argv[2]);
      else
         generateAquaedConfig(config, argv[1], NULL);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
