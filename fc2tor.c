/******************************************************************************
 * FIDOCONFIG --- library for fidonet configs
 ******************************************************************************
 * Copyright (C) 1998
 *
 * Victor Anikeev
 * Original Code from: Matthias Tichy
 *
 * Fido:     2:5043/3.88 2:5043/17.5
 * Internet: mgl@pisem.net starwars@bk.ru
 *  [ please, write e-mails. fidonet doesn't work properly in our city :-( ]
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fidoconf.h"
#include "common.h"

#include "fc2tor_g.h"

#define TAG_NAME 0
#define TAG_DESC 1
#define TAG_BOTH 2

/* group delimeters */

#define DELIMS " ,;"

unsigned bbslevelsysop = 256;
unsigned bbslevelshow = 0;
unsigned bbslevellist = 0;

int export_mailareas = 0;
int export_fileareas = 0;

int export_local = 0;
int export_echo = 0;
int export_netmail = 0;

int export_file = 0;
int export_bbs = 0;

int export_group = 0;
int export_security = 0;
int export_scanprivmail = 0;
int export_scannewfiles = 0;

int tag_type = TAG_NAME;
int bbsdeffmt = 1;

char *bbsulpath = NULL;
char *groups = NULL;

int writeEchoArea(FILE *f, s_area *area, char type) {

   fprintf(f, "[MsgArea]\n");

   fprintf(f, "Name ");
   switch ( tag_type ) {
   case TAG_NAME: fprintf(f, " \"%s\"\n", area->areaName);
                  break;
   case TAG_DESC: fprintf(f, " \"%s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   case TAG_BOTH: fprintf(f, " \"%s", area->areaName);
                  fprintf(f, ": %s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   }

   fprintf(f, "UseAddress %u:%u/%u.%u\n", area->useAka->zone, area->useAka->net, area->useAka->node, area->useAka->point);
   fprintf(f, "BasePath %s\n", area->fileName);

   fprintf(f, "BaseType ");
   if (area->msgbType == MSGTYPE_SQUISH) fprintf(f, "Squish");
   else if (area->msgbType == MSGTYPE_JAM) fprintf(f, "Jam");
   else fprintf(f, "Fido");
   fprintf(f, "\n");

   if ( export_group ) fprintf(f, "Group %s\n", area->group);

   fprintf(f, "Type ");
   switch (type) {
     case 0: fprintf(f, "EchoMail");
             break;
     case 2: fprintf(f, "EchoMail");
             break;
     case 1: fprintf(f, "NetMail");
             break;
   }
   fprintf(f, "\n");

   if (export_scanprivmail) fprintf(f, "Scan_PrivMail Yes\n");

   if (export_security) {
     fprintf(f, "Read_Security  %u\n", area->levelread);
     fprintf(f, "Write_Security %u\n", area->levelwrite);
     fprintf(f, "Show_Security  %u\n", (area->hide) ? bbslevelsysop : bbslevelshow);
     fprintf(f, "Sysop_Security %u\n", bbslevelsysop);
   }
   fprintf(f, "\n\n");
   return 0;
}

int writeFileArea(FILE *f, s_filearea *area) {

   fprintf(f, "[FileArea]\n");

   fprintf(f, "Name ");
   switch ( tag_type ) {
   case TAG_NAME: fprintf(f, " \"%s\"\n", area->areaName);
                  break;
   case TAG_DESC: fprintf(f, " \"%s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   case TAG_BOTH: fprintf(f, " \"%s", area->areaName);
                  fprintf(f, ": %s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   }

   fprintf(f, "DLPath %s\n", area->pathName);
   if ( bbsulpath != NULL )
     fprintf(f, "ULPath %s\n", bbsulpath);

   fprintf(f, "FileList %sfiles.bbs\n", area->pathName);
   fprintf(f, "FList_Format %s\n", (bbsdeffmt) ? "FilesBBS" : "CD-List");
   if ( !bbsdeffmt ) fprintf(f, "Copy_Local Yes\n");

   if ( export_group ) fprintf(f, "Group %s\n", area->group);

   if (export_scannewfiles) fprintf(f, "Scan_NewFiles Yes\n");

   if (export_security) {
     fprintf(f, "DL_Security  %u\n", area->levelread);
     fprintf(f, "UL_Security %u\n", area->levelwrite);
     fprintf(f, "Show_Security  %u\n", (area->hide) ? bbslevelsysop : bbslevelshow);
     fprintf(f, "List_Security  %u\n", bbslevellist);
     fprintf(f, "Sysop_Security %u\n", bbslevelsysop);
   }
   fprintf(f, "\n\n");
   return 0;
}

int writeBBSArea(FILE *f, s_bbsarea *area) {

   fprintf(f, "[FileArea]\n");

   fprintf(f, "Name ");
   switch ( tag_type ) {
   case TAG_NAME: fprintf(f, " \"%s\"\n", area->areaName);
                  break;
   case TAG_DESC: fprintf(f, " \"%s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   case TAG_BOTH: fprintf(f, " \"%s", area->areaName);
                  fprintf(f, ": %s\"\n", (area->description!=NULL) ? area->description : area->areaName);
                  break;
   }

   fprintf(f, "DLPath %s\n", area->pathName);
   if ( bbsulpath != NULL )
     fprintf(f, "ULPath %s\n", bbsulpath);

   fprintf(f, "FileList %sfiles.bbs\n", area->pathName);
   fprintf(f, "FList_Format %s\n", (bbsdeffmt) ? "FilesBBS" : "CD-List");
   if ( !bbsdeffmt ) fprintf(f, "Copy_Local Yes\n");

   if (export_scannewfiles) fprintf(f, "Scan_NewFiles Yes\n");

   if (export_security) {
     fprintf(f, "List_Security  %u\n", bbslevellist);
     fprintf(f, "Sysop_Security %u\n", bbslevelsysop);
   }
   fprintf(f, "\n\n");
   return 0;
}

int generateBBSConfig(s_fidoconfig *config, char *fileName) {
   FILE *f;
   int  i;
   s_area *area;

   f = fopen(fileName, "a+");

   if (f!= NULL) {

    if (export_mailareas && export_netmail) {
      for (i=0; i<config->netMailAreaCount; i++) {
        if (isSubSetOfGroups(groups, config->netMailAreas[i].group, DELIMS))
         writeEchoArea(f, &(config->netMailAreas[i]), 1);
      }
    }

    if (export_mailareas && export_echo) {
     for (i=0; i<config->echoAreaCount; i++) {
       area = &(config->echoAreas[i]);
       if (area->msgbType != MSGTYPE_PASSTHROUGH)
        if (isSubSetOfGroups(groups, area->group, DELIMS))
           writeEchoArea(f, area, 0);
     }
    }

    if (export_mailareas && export_local) {
     for (i=0; i<config->localAreaCount; i++) {
       area = &(config->localAreas[i]);
        if (isSubSetOfGroups(groups, area->group, DELIMS))
         writeEchoArea(f, area, 2);
     }
    }

    if (export_fileareas && export_file) {
      for (i=0; i<config->fileAreaCount; i++) {
        if (isSubSetOfGroups(groups, config->fileAreas[i].group, DELIMS))
          writeFileArea(f, &(config->fileAreas[i]));
      }
    }

    if (export_fileareas && export_bbs) {
      for (i=0; i<config->bbsAreaCount; i++) {
/*        if (isSubSetOfGroups(groups, config->bbsAreas[i].group, DELIMS)) */
          writeBBSArea(f, &(config->bbsAreas[i]));
      }
    }

    return 0;
   }
   else printf("Could not write %s\n", fileName);

   return 1;
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
        fputs(buffer,f2);
    }
    fputs("\n",f2);
    fclose(f1);
    fclose(f2);
  }
  return 0;
}

void parseOptions(char *line) {
   int options=0;
   char chr=0;

   if ( strncasecmp(line, "-m", 2) == 0 ) {
    export_mailareas = 1;
    line+=2;
    while ( *line ) {
      switch ( tolower(*line) ) {
      case 'e': export_echo = 1; break;
      case 'l': export_local = 1; break;
      case 'n': export_netmail = 1; break;
      default: printf("Invalid flag: %c\n", *line);
      }
      line++;
    }
   } else

   if ( strncasecmp(line, "-f", 2) == 0 ) {
    export_fileareas = 1;
    line+=2;
    while ( *line ) {
      switch ( tolower(*line) ) {
      case 'f': export_file = 1; break;
      case 'b': export_bbs = 1; break;
      default: printf("Invalid flag: %c\n", *line);
      }
      line++;
    }
   } else

   if ( stricmp(line, "-sc" ) == 0 ) {
     export_scanprivmail = export_scannewfiles = 1; } else

   if ( stricmp(line, "-cd" ) == 0 ) bbsdeffmt = 0; else

   if ( stricmp(line, "-s" ) == 0 ) export_security = 1; else
   if ( stricmp(line, "-g" ) == 0 ) export_group = 1; else

   if ( stricmp(line, "-nt" ) == 0 ) tag_type = TAG_NAME; else
   if ( stricmp(line, "-nd" ) == 0 ) tag_type = TAG_DESC; else
   if ( stricmp(line, "-nb" ) == 0 ) tag_type = TAG_BOTH; else

   if ( strncasecmp(line, "-ss", 3 ) == 0 ) {
     line+=3;
     sscanf(line, "%u", &bbslevelsysop);
   } else

   if ( strncasecmp(line, "-ul", 3 ) == 0 ) {
     line+=3;
     bbsulpath = strdup(line);
   } else

   if ( strncasecmp(line, "-gr", 3 ) == 0 ) {
     line+=3;
     groups = strdup(line);
   } else

   printf("Invalid option: %s\n", line);
}

int main (int argc, char *argv[]) {
   s_fidoconfig *config;
   int cont=1;

   printf("fconf2tornado\n");
   printf("-------------\n");

   while ((cont<argc) && (*argv[cont]=='-')) {
        parseOptions(argv[cont]);
        cont++;
   }

   if (!(cont<argc)) {
      printf("\nUsage:\n");
      printf("   fconf2tornado -[command [-command...]] <tornado.ctl> [<default.ctl>]\n");
      printf("    (you may read config defaults from default.ctl)              \n");
      printf("    -m[<flags>] exports mail areas:   n  netmail areas           \n");
      printf("                                      e  echo areas              \n");
      printf("                                      l  local areas             \n");
      printf("    -f[<flags>] exports file areas:   f  file areas              \n");
      printf("                                      b  bbs areas               \n");
      printf("    -s        exports security options                           \n");
      printf("    -g        exports groups                                     \n");
      printf("    -nt       exports area tag as name                           \n");
      printf("    -nd       exports description as name                        \n");
      printf("    -nb       exports area tag and description as name           \n");
      printf("    -ss<num>  sets sysop sequrity level (default=256)            \n");
      printf("    -cd       uses CD-List as FList_Format (default id FilesBBS) \n");
      printf("    -sc       exports Scan_NewFiles and Scan_PrivMail keywords   \n");
      printf("    -ul<path> sets default UL_Path for all areas                 \n");
      printf("    -gr<grps> exports areas with <grp> groups only (default all) \n");
      printf("\nExamples:                                                      \n");
      printf("   fconf2tornado -mel -ss256 c:\\tornado\\msgarea.ctl            \n");
      printf("   fconf2tornado -ff -g -ulc:\\bbs\\upload filearea.ctl          \n\n");
      printf("   fconf2tornado -ff -g -grFido temp.ctl                         \n");
      printf("   fconf2tornado -ff -g -grLocal filearea.ctl temp.ctl           \n");
      return 1;

   }
   printf("Generating Config-file %s\n", argv[cont]);

   config = readConfig(NULL);

   config = readConfig(NULL);
   if (config!= NULL) {
    if (argv[cont+1]!=NULL) readDefaultConfig(argv[cont], argv[cont+1]);
    else  remove (argv[cont]);
     generateBBSConfig(config, argv[cont]);
     disposeConfig(config);
     return 0;
   } return 1;

}
