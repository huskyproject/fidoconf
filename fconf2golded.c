#include <stdlib.h>

#include "fidoconfig.h"

int writeArea(FILE *f, s_area *area, char netmail) {

   fprintf(f, "areadef %s \"%s\" %c ", area->areaName, area->areaName, area->group);

   if (netmail==1) fprintf(f, "net ");
   else fprintf(f, "echo ");

   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, "Squish ");
   else fprintf(f, "Opus ");

   fprintf(f, "%s ", area->fileName);

   fprintf(f, "%u:%u/%u.%u", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   
   fprintf(f, "\n");

   return 0;
}

int readDefaultConfig(char *cfg_file, char *def_file) {
   char cmd[256];
   
   sprintf(cmd, "cp -f %s %s", def_file, cfg_file);
   system (cmd);
	   
   return 0;
}

int generateMsgEdConfig(s_fidoconfig *config, char *fileName) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "a+");
   if (f!= NULL) {

      fprintf(f, "username %s\n\n", config->sysop);
      
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
      
      return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   char cmd[256];
   
   printf("fconf2golded\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2golded <goldedConfigFileName> [<default.cfg>]\n");
      printf("   (you may read config defaults from default.cfg)\n");
      printf("\nExample:\n");
      printf("   fconf2golded ~/golded/golded.cfg\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[1]);

   config = readConfig();
   if (config!= NULL) {

	  if (argv[2]!=NULL) readDefaultConfig (argv[1], argv[2]);
	  else {
		  sprintf(cmd, "rm -f %s", argv[1]);
		  system (cmd);
	  }
      generateMsgEdConfig(config, argv[1]);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
