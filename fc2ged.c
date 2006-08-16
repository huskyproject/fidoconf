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
#define AREASONLY 0x1
#define NETMAIL 0x2
#define ECHOMAIL 0x4
#define LOCAL 0x8
#define DUPE 0x10
#define BAD 0x20


#include <stdlib.h>
#include <string.h>

#include "fidoconf.h"
#include "common.h"

#ifndef VERSION_H
#define VERSION_H

#include "version.h"
#include "cvsdate.h"

#endif

char *fidoconfig=NULL;

void usage(){
      printf(
      "Usage: fconf2golded [options] <GoldedConfigFileName> [GoldedDefaultConfigFileName]\n"
      "Options:\n"
      "\t  -a\t- exports areas only\n"
      "\t  -cFidoconfig - specify a Husky config file <Fidoconfig>\n"
      "\t  -h\t- print usage information (this text)\n"
      "\t  -H\t- print usage information (this text)\n"
      "\t  -sb\t- skip badmail areas\n"
      "\t  -sd\t- skip dupes areas\n"
      "\t  -se\t- skip echomail areas\n"
      "\t  -sl\t- skip local areas\n"
      "\t  -sn\t- skip netmail areas\n");
      exit(1);
}

int writeArea(FILE *f, s_area *area, char type) {

   if (area->group == NULL) {
      area->group = malloc(2);
      strcpy(area->group, "0");
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

int generateGoldEdConfig(s_fidoconfig *config, char *fileName, int options) {
   FILE *f;
   int  i;
   s_area *area;
   f = fopen(fileName, "a+");
   if (f!= NULL) {
    if (!(options & AREASONLY)) {

       fprintf(f, "username %s\n\n", config->sysop);
      
       for (i=0; i<config->addrCount; i++)
         fprintf(f, "Address %u:%u/%u.%u\n", 
                    config->addr[i].zone, 
                    config->addr[i].net, 
                    config->addr[i].node, 
                    config->addr[i].point);
                    
       fprintf(f, "\n");
     }
    if (!(options & NETMAIL)) {

	     for (i=0; i<config->netMailAreaCount; i++) {
		     writeArea(f, &(config->netMailAreas[i]), 1);
		}
	}if (!(options & DUPE)) {
    
		writeArea(f, &(config->dupeArea), 2);
	}
    if (!(options & BAD)) {
		writeArea(f, &(config->badArea), 2);
	}
    if (!(options & ECHOMAIL)) {
     for (i=0; i<config->echoAreaCount; i++) {
       area = &(config->echoAreas[i]);
       if (area->msgbType != MSGTYPE_PASSTHROUGH)
           writeArea(f, area, 0);
     }
	}      
    if (!(options & LOCAL)) {
     for (i=0; i<config->localAreaCount; i++) {
       area = &(config->localAreas[i]);
       writeArea(f, area, 2);
     }
	}     
     return 0;
   } else printf("Could not write %s\n", fileName);

   return 1;
}

int parseOptions(char *line){
int options=0;
char chr=0;

 if(line[0]!='-'){
   fprintf(stderr,"parseOptions(): this is not option: \"%s\"\n",line);
   exit(1);
 }

 if (sstrcmp(line+1,"a")==0){
   options^=AREASONLY;
   return options;
 }
 else if (line[1]=='c'){
   if(line[2]!='\0') fidoconfig=sstrdup(line+2);
   return options;
 }
 else if (line[1]=='s' && line[3]=='\0')
   chr=line[2];
 else if (line[1]=='h' || line[1]=='H'){
   usage();
 }
 else{
   fprintf(stderr,"this is unknown option: \"%s\"\n",line);
   exit(1);
 }

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
	default:
	    fprintf(stderr,"this is unknown option: \"%s\"\n",line);
	    exit(1);

 }
return options;
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   int options=0;
   int cont=1;
   char *versionStr = NULL;

   versionStr = GenVersionStr( "fconf2golded", FC_VER_MAJOR,
	                 FC_VER_MINOR, FC_VER_PATCH, FC_VER_BRANCH, cvs_date);

   printf("%s\n\n", versionStr);
 
   while ((cont<argc)&&(*argv[cont]=='-')){
	options|=parseOptions(argv[cont]);	
	cont++;
   }
   if (!(cont<argc)){
     usage();
   }
   printf("Generating Config-file %s\n", argv[cont]);

   config = readConfig(fidoconfig);
   if (config!= NULL) {
     if (argv[cont+1]!=NULL)  readDefaultConfig (argv[cont], argv[cont+1]);
     else  remove (argv[cont]);
     generateGoldEdConfig(config, argv[cont], options);
     disposeConfig(config);
     return 0;
   }

   return 1;
}
