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
#define AREASONLY 0x1
#define NETMAIL 0x2
#define ECHOMAIL 0x4
#define LOCAL 0x8
#define DUPE 0x10
#define BAD 0x20

#include "fidoconf.h"
#include "common.h"

int writeArea(FILE *f, s_area *area, char netMail) {
   switch (area->msgbType) {
      
      case (MSGTYPE_SQUISH): fprintf(f, "Squish ");
                             break;
      
      case (MSGTYPE_SDM):    fprintf(f, "Fido ");
                             break;

      case (MSGTYPE_JAM):    fprintf(f, "Jam ");
                             break;
   }

   if (netMail == 1) fprintf(f, "mp");
   else fprintf(f, "e");
   fprintf(f, "8u ");

   fprintf(f, "\"%s\" %s %s ", area->areaName, area->fileName, (netMail!=1) ? area->areaName : "");

   fprintf(f, "%u:%u/%u.%u", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   
   fprintf(f, "\n");

   return 0;
}

int generateMsgEdConfig(s_fidoconfig *config, char *fileName, int options) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "w");
   if (f!= NULL) {
    if (!(options & AREASONLY)) {

      fprintf(f, "Name \"%s\"\n\n", config->sysop);
      
      for (i=0; i<config->addrCount; i++)
         fprintf(f, "Address %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      
      if (config->echotosslog != NULL) fprintf(f, "tossLog %s\n", config->echotosslog);
      if (config->nodelistDir != NULL && config->fidoUserList != NULL)
      {
          fprintf(f, "Userlist %s%s\n",
                  config->nodelistDir, config->fidoUserList);
      }

      fprintf(f, "\n");
	}
   if (!(options & NETMAIL)) {
      for (i=0; i<config->netMailAreaCount; i++) {
         writeArea(f, &(config->netMailAreas[i]), 1);
      }
   }
   if (!(options & DUPE)) {
	    writeArea(f, &(config->dupeArea), 0);
   }
   if (!(options & BAD)) {
		writeArea(f, &(config->badArea), 0);
   }
   if (!(options & LOCAL)) {
		for (i=0; i<config->localAreaCount; i++) {
         area = &(config->localAreas[i]);
         if (area->msgbType != MSGTYPE_PASSTHROUGH)
             writeArea(f, area, 0);
		}
   }
   if (!(options & ECHOMAIL)) {      
      for (i=0; i<config->echoAreaCount; i++) {
         area = &(config->echoAreas[i]);
         if (area->msgbType != MSGTYPE_PASSTHROUGH)
             writeArea(f, area, 0);
      }
   }    
   return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int parseOptions(char *line){
int options=0;
char chr=0;

if (strcmp(line,"-a")==0) chr='a';
else  (chr=line[2]);

 switch (chr){

	case 'a':	{
					options^=AREASONLY;
					break;
	}
	case 'n':	{
					options^=NETMAIL;
					break;
	}
	case 'e':	{
					options^=ECHOMAIL;
					break;
	}

	case 'l':	{
					options^=LOCAL;
					break;
	}
	case 'd':	{
					options^=DUPE;
					break;
	}
	case 'b':	{
					options^=BAD;
					break;
	}

 }
return options;
}


int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   int options=0;
   int cont=1;

   printf("fconf2msged\n");
   printf("-----------\n");
   while ((cont<argc)&&(*argv[cont]=='-')){
	options|=parseOptions(argv[cont]);	
	cont++;
   }
   if (!(cont<argc)){
      printf("\nUsage:\n");
      printf("   fconf2msged  [-a][-sn][-se][-sl][-sb][-sd] <msgedConfigFileName>\n");
      printf("   (-a exports areas only)\n");
	  printf("   (-sn skip netmail areas)\n");
	  printf("   (-se skip echomail areas)\n");
	  printf("   (-sl skip local areas, and so on...)\n");
       printf("\nExample:\n");
      printf("   fconf2msged ~/.msged\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[cont]);

   config = readConfig(NULL);
   if (config!= NULL) {
      generateMsgEdConfig(config, argv[cont], options);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
