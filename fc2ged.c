/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998
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
#include <string.h>

#include "fidoconf.h"
#include "common.h"

int writeArea(FILE *f, s_area *area, char type) {

   if (area->group == NULL) {
      area->group = malloc(1);
      *(area->group) = '\0';
   }

   fprintf(f, "areadef %s \"%s\" %s ", area->areaName,
             (area->description!=NULL) ? area->description : area->areaName, area->group);

   switch (type) {
     case 0: fprintf(f, "echo ");
             break;
     case 1: fprintf(f, "net ");
             break;
     case 2: fprintf(f, "local ");
   }

   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, "Squish ");
   else if (area->msgbType == MSGTYPE_JAM) fprintf(f, "Jam ");
   else fprintf(f, "Opus ");

   fprintf(f, "%s ", area->fileName);

   fprintf(f, "%u:%u/%u.%u", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   
   fprintf(f, "\n");

   return 0;
}

int readDefaultConfig(char *cfg_file, char *def_file) {
  FILE *f1,*f2;
  char buffer[2048];

  if ((f1=fopen(def_file,"rt"))==NULL) {
    perror("Orig. file not found!");
    return -1;
  }
  else {
    if ((f2=fopen (cfg_file,"wt"))==NULL) {
      perror("Can't create dest. file!");
      return -2;
    }
    else {
      while (fgets(buffer,sizeof(buffer),f1))
        fputs (buffer,f2);
    }
    fclose(f1);
    fclose(f2);
  }
  return 0;
}

int generateMsgEdConfig(s_fidoconfig *config, char *fileName, int areasOnly) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "a+");
   if (f!= NULL) {

     if (!areasOnly) {
       fprintf(f, "username %s\n\n", config->sysop);
      
       for (i=0; i<config->addrCount; i++)
         fprintf(f, "Address %u:%u/%u.%u\n", 
                    config->addr[i].zone, 
                    config->addr[i].net, 
                    config->addr[i].node, 
                    config->addr[i].point);
                    
       fprintf(f, "\n");
     }

     for (i=0; i<config->netMailAreaCount; i++) {
         writeArea(f, &(config->netMailAreas[i]), 1);
     }
     writeArea(f, &(config->dupeArea), 2);
     writeArea(f, &(config->badArea), 2);

     for (i=0; i<config->echoAreaCount; i++) {
       area = &(config->echoAreas[i]);
       if (area->msgbType != MSGTYPE_PASSTHROUGH)
           writeArea(f, area, 0);
     }
      
     for (i=0; i<config->localAreaCount; i++) {
       area = &(config->localAreas[i]);
       writeArea(f, area, 2);
     }
      
     return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   int cont=1;
   
   printf("fconf2golded\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2golded [-a] <goldedConfigFileName> [<default.cfg>]\n");
      printf("   (you may read config defaults from default.cfg)\n");
      printf("   (-a exports areas only)\n");
      printf("\nExample:\n");
      printf("   fconf2golded ~/golded/golded.cfg\n\n");
      return 1;
   }

   if (stricmp(argv[1],"-a")==0)
     cont++;
     
   printf("Generating Config-file %s\n", argv[cont]);

   config = readConfig(NULL);
   if (config!= NULL) {

	  if (argv[cont+1]!=NULL) readDefaultConfig (argv[cont], argv[cont+1]);
	  else
       remove (argv[cont]);

     generateMsgEdConfig(config, argv[cont], cont-1);
     disposeConfig(config);
     return 0;
   }

   return 1;
}
