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
 *
 * See also http://www.gnu.org
 *****************************************************************************/

/* adapted from fconf2golded by Alex Bronin, 2:5049/36 */

#include <stdlib.h>
#include <string.h>

#include "fidoconf.h"
#include "common.h"

#include <smapi/unused.h>

int writeArea(FILE *f, s_area *area, char type) {
   int i;

   switch (type) {
     case 0: fprintf(f, "EchoArea ");
             break;
     case 1: fprintf(f, "NetArea ");
             break;
     case 2: fprintf(f, "LocalArea ");
             break;
     case 3: fprintf(f, "DupeArea ");
             break;
     case 4: fprintf(f, "BadArea ");
   }

   fprintf(f, "%s", area->areaName);

   if (area->msgbType != MSGTYPE_PASSTHROUGH) fprintf(f, " %s", area->fileName);
      else fprintf(f, " passthrough");

   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, " -$");
   if (area->msgbType == MSGTYPE_PASSTHROUGH) fprintf(f, " -0");

   if (area->description!=NULL) fprintf(f, " -$n\"%s\"", area->description);

   if (area->group && strcmp(area->group,"0")) fprintf(f, " -$g%s",area->group);

   if (area->purge) fprintf(f, " -$d%u", area->purge);

   if (area->max) fprintf(f, " -$m%u", area->max);

   fprintf(f, " -p%s", aka2str(*area->useAka));

   for (i=0; i<area->downlinkCount; i++)
       fprintf(f, " %s", aka2str(area->downlinks[i]->link->hisAka));

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

   unused(areasOnly);

   if (strcmp(fileName,"-") == 0)
     f = stdout;
   else
     f = fopen(fileName, "a+");
   if (f!= NULL) {

     for (i=0; i<config->netMailAreaCount; i++) {
         writeArea(f, &(config->netMailAreas[i]), 1);
     }
     writeArea(f, &(config->dupeArea), 3);
     writeArea(f, &(config->badArea), 4);

     for (i=0; i<config->echoAreaCount; i++) {
       area = &(config->echoAreas[i]);
/*       if (area->msgbType != MSGTYPE_PASSTHROUGH) */
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

   fprintf(stderr,"fconf2squish\n");
   fprintf(stderr,"------------\n");
   if (argc < 2) {
      fprintf(stderr,"\nUsage:\n");
      fprintf(stderr,"   fconf2squish <squish.cfg>|- [<default.cfg>]\n");
      fprintf(stderr,"   (- as squish.cfg means stdout)\n");
      fprintf(stderr,"   (you may read config defaults from default.cfg)\n");
      fprintf(stderr,"\nExample:\n");
      fprintf(stderr,"   fconf2squish ~/squish/squish.cfg\n");
      fprintf(stderr,"   fconf2squish - | sed \"\\/var\\/fido\\//u:\\\\\\/gi\" > /etc/fido/squish.cfg\n\n");
      return 1;
   }

   fprintf(stderr,"Generating Config-file %s\n", argv[1]);

   config = readConfig(NULL);
   if (config!= NULL) {

   if (argv[2]!=NULL) readDefaultConfig (argv[1], argv[2]);
   else
       remove (argv[1]);

     generateMsgEdConfig(config, argv[1], 0);
     disposeConfig(config);
     return 0;
   }

   return 1;
}
