#include "fidoconfig.h"

int writeArea(FILE *f, s_area *area, char netmail) {

   fprintf(f, "areadef %s \"%s\" 0 ", area->areaName, area->areaName);

   if (netmail==1) fprintf(f, "net ");
   else fprintf(f, "echo ");

   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, "Squish ");
   else fprintf(f, "Opus ");

   fprintf(f, "%s ", area->fileName);

   fprintf(f, "%u:%u/%u.%u", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   
   fprintf(f, "\n");

   return 0;
}

int generateMsgEdConfig(s_fidoconfig *config, char *fileName) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "w");
   if (f!= NULL) {

      fprintf(f, "username %s\n\n", config->sysop);
      
      for (i=0; i<config->addrCount; i++)
         fprintf(f, "Address %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      fprintf(f, "\n");

      writeArea(f, &(config->netMailArea), 1);
      
      for (i=0; i<config->echoAreaCount; i++) {
         area = &(config->echoAreas[i]);
         if (area->msgbType != MSGTYPE_PASSTHROUGH)
             writeArea(f, area, 0);
      }
      
      return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;

   printf("fconf2golded\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2golded <goldedConfigFileName>\n");
      printf("\nExample:\n");
      printf("   fconf2golded ~/golded/golded.cfg\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[1]);

   config = readConfig();
   if (config!= NULL) {
      generateMsgEdConfig(config, argv[1]);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
