#include "fidoconfig.h"

int writeArea(FILE *f, s_area *area, char netmail) {
   switch (area->msgbType) {
      
      case (MSGTYPE_SQUISH): fprintf(f, "Squish ");
                             break;
      
      case (MSGTYPE_SDM):    fprintf(f, "Fido ");
                             break;
   }

   if (netmail == 1) fprintf(f, "np");
   else fprintf(f, "e");
   fprintf(f, "8u ");

   fprintf(f, "\"%s\" %s %s ", area->areaName, area->fileName, (netmail==1) ? area->areaName : "");

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

      fprintf(f, "Name \"%s\"\n\n", config->sysop);
      
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

   printf("fconf2msged\n");
   printf("-----------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2msged <msgedConfigFileName>\n");
      printf("\nExample:\n");
      printf("   fconf2msged ~/.msged\n\n");
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
