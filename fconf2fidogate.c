/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998
 * 
 * Gabriel Plutzar 
 * Original Code from: Matthias Tichy
 *
 * gabriel@hit.priv.at, 2:31/1
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

#ifndef MSDOS
#include "fidoconfig.h"
#else
#include "fidoconf.h"
#endif

int writeArea(FILE *f, s_area *area, char netmail) {

   if (netmail!=1) 
      fprintf(f, "%-32s fido.%s -x\n", area->areaName, area->areaName);

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

      fprintf(f, 
"#:ts=8                                                      \n"               
"#                                                           \n"               
"# $Id$             \n"               
"#                                                           \n"               
"# Echomail area <-> News newsgroup conversion               \n"               
"#                                                           \n"               
"# Options:                                                  \n"               
"#     -a Z:N/F.P         Alternate address for this area    \n"               
"#     -z ZONE            Alternate zone AKA for this area   \n"               
"#     -d DISTRIBUTION    Distribution header for this newsgroups\n"           
"#     -o ORIGIN          * Origin line for this area  \n"                     
"#     -g                 No gated messages            \n"                     
"#     -l                 Only local crosspostings     \n"                     
"#     -x                 No crosspostings             \n"                     
"#     -8                 Messages with 8bit ISO-8859-1 charset\n"             
"#     -H                 Names match entire hierarchy, names are translated\n"
"#     -!                 Don't gate area/newsgroup [hierarchy] \n"          
"#     -R LVL             ^ARFC header level  \n"                              
"#     -m MAXSIZE         MaxMsgSize for this area (0 = infinity)\n"           
"#     -X \42Xtra: xyz\42     Add extra RFC headers (multiple -X allowed)\n"       
"#                       \n"                                                   
"# All fields may be quoted in \42...\42, order is import, first area/newsgroup\n"
"# found matches!                                                     \n"
"#                                                                    \n"
"# Format:                                                            \n"
"#                                                                    \n"
"# area                        newsgroup                     [-option]\n"
"# ------------------------    --------------------------    ---------\n"
"\n"                                                                     
);
      
      for (i=0; i<config->echoAreaCount; i++) {
         area = &(config->echoAreas[i]);
             writeArea(f, area, 0);
      }
      
      return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   char cmd[256];
   
   printf("fconf2fidogate\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2golded <FidoGateAreasFileName> [<default.cfg>]\n");
      printf("   (you may read config defaults from default.cfg)\n");
      printf("\nExample:\n");
      printf("   fconf2fidogate /usr/local/lib/fidogate/areas\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[1]);

   config = readConfig();
   if (config!= NULL) {

	  if (argv[2]!=NULL) readDefaultConfig (argv[1], argv[2]);
	  else {
		  sprintf(cmd,
#ifndef MSDOS
                  "rm -f %s",
#else
	          "del %s",
#endif
                  argv[1]);
		  system (cmd);
	  }
      generateMsgEdConfig(config, argv[1]);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
