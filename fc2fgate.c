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
#include <ctype.h>
#include <string.h>

#include "fidoconf.h"
#include "common.h"

#include <smapi/patmat.h>

char *areaconfig;
int areaconfiganz=0;

int writeArea(FILE *f, s_area *area, char netmail) {

   char areaupperletter[100];
   int i;
   int found;

   if (netmail) 
      return 0;

   strcpy(areaupperletter,area->areaName);
   strUpper(areaupperletter);

   found=0;
   for (i=0;i<areaconfiganz;i++)
       if (patimat(areaupperletter,areaconfig+i*60+1)==1)
          {          
          found=1;
          break;
          }

   if (!found)
      fprintf(f, "%-32s fido.%s -x\n", areaupperletter, area->areaName);
     else
      {
      if (((char *)(areaconfig+i*60))[0]=='-')
         fprintf(f, "%-32s %s -x\n", areaupperletter,area->areaName);
      }

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

int generateMsgEdConfig(s_fidoconfig *config, char *fileName) {
   FILE *f;
   int  i,j;
   s_area *area;
   char hlp[100];

   f = fopen("/etc/fido/fconf2fidogate.cfg", "r");
   if (f==NULL)
      {
      printf("You have to place the file fconf2fidogate.cfg into /etc/fido !\n");
      exit(3);
      }

   i=0;
   while (!feof(f))
         {
         fgets(hlp,100,f);
         if (hlp[0] == '!' || hlp[0] == '-')
            {
            for (j=1;hlp[j]!=0;j++)
                {
                hlp[j]=(char)toupper(hlp[j]);
                if (!(hlp[j]>='A' && hlp[j]<='Z') && hlp[j]!='.' &&
                    hlp[j]!='_' && hlp[j]!='-' && hlp[j]!='*' &&
                    hlp[j]!=0x27 && hlp[j]!=0x60 )
                   {
                   hlp[j]=0;
                   break;
                   }
                }
            }

         strcpy(areaconfig+60*areaconfiganz,hlp);
         areaconfiganz++;
         }

   fclose(f);

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
   
   printf("fconf2fidogate\n");
   printf("------------\n");
   if (argc < 2) {
      printf("\nUsage:\n");
      printf("   fconf2fidogate <FidoGateAreasFileName> [<default.cfg>]\n");
      printf("   (you may read config defaults from default.cfg)\n");
      printf("\nExample:\n");
      printf("   fconf2fidogate /usr/local/lib/fidogate/areas\n\n");
      return 1;
   }

   printf("Generating Config-file %s\n", argv[1]);

   areaconfig=(char *)malloc(65536);

   config = readConfig();
   if (config!= NULL) {

	  if (argv[2]!=NULL) readDefaultConfig (argv[1], argv[2]);
	  else
	    remove (argv[1]);

      generateMsgEdConfig(config, argv[1]);
      disposeConfig(config);
      return 0;
   }

   return 1;
}
