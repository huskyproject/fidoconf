/* $Id$
******************************************************************************
* Fastecho config to fidoconfig convertion program (main module).
*
* This file is part of FIDOCONFIG.
*
*       Copyright (C) 1999
*       Fedor Lizunkov
*       Fido: 2:5020/960
*
*       Copyright (C) Husky developers team
******************************************************************************/

char Revision[] = "$Revision$";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <smapi/compiler.h>
#include <smapi/typedefs.h>
#include <smapi/stamp.h>
#include <smapi/progprot.h>

#include "common.h"

#ifndef VERSION_H
#define VERSION_H

#include "version.h"
#include "cvsdate.h"

#endif

/* #define INC_FE_TYPES */
#define INC_FE_BAMPROCS
#define FALSE 0
#define TRUE 1
#include "fecfg146.h"

#define OUTPUT_FILE "hpt_temp.cfg"


/* Global variables */
   char            *FEconfig=NULL;
   CONFIG          config;
   Area            **area;
   ExtensionHeader header;
   SysAddress      *sysaddr = NULL;
   int             packers_count = 0, packers2_count = 0;
   Packers         *packers = NULL, *packers2 = NULL;
   int             unpackers_count = 0, unpackers2_count = 0;
   Unpackers       *unpackers = NULL, *unpackers2 = NULL;
   GroupDefaults   **groupdef = NULL;
   Node            **node;
   ForwardAreaFix  *frequest = NULL;
   FILE            *f_cfg=NULL, *f_hpt=NULL;


char parseFEgroup(register unsigned short FEgroup)
{
  if (FEgroup <= 25) /* Letters */
     FEgroup +='A';
  else if (FEgroup <= 35) /* Numbers */
     FEgroup +='0';
  else FEgroup=0;
  return FEgroup;
}

/*  convert FastEcho-GroupBitmap to GroupString for fidoconfig */
/*  warning: returns pointer to static array! */
char *grp2str(dword bitmap)
{
   static char buff[66];
   char   *curr, *ptr, key, ch, tmp;

   ptr = (char*)&bitmap;
   curr = buff;
   for (key = 'A', ch = *ptr; key <= 'Z'; key++, ch<<=1) {
      if (key == 'I' || key == 'Q' || key == 'Y') {
         ptr++;
         ch = *ptr;
      } /* endif */
      tmp = ch|0x7f;
      tmp ^=0xff;
      if (tmp) {
         *curr = key;
         curr++;
         *curr = ',';
         curr++;
      } /* endif */
   } /* endfor */
   for (key = '1'; key <= '6'; key++, ch <<=1) {
      tmp = ch|0x7f;
      tmp ^=0xff;
      if (tmp) {
         *curr = key;
         curr++;
         *curr = ',';
         curr++;
      } /* endif */
   } /* endfor */

   /*  strip last ',' if any */
   if (curr != buff) curr--;

   /*  terminate buff */
   *curr = 0;

   return buff;
}

char *FEaka2str(FEAddress addr)
{
   static char aka[24];

   if (addr.point)
      sprintf(aka, "%d:%d/%d.%d", addr.zone, addr.net, addr.node, addr.point);
   else
      sprintf(aka, "%d:%d/%d", addr.zone, addr.net, addr.node);

   return aka;
}


/* return string: zone:net/node.point@domain */
char *sysAddress2str(SysAddress sysaddr)
{
   static char aka[24+sizeof(sysaddr.domain)];

   if (sysaddr.main.point && sysaddr.domain && sysaddr.domain[0] ) {
      sprintf(aka, "%d:%d/%d.%d@%s", sysaddr.main.zone, sysaddr.main.net,
                                     sysaddr.main.node, sysaddr.main.point,
                                     sysaddr.domain);
   } else if (sysaddr.main.point && (sysaddr.domain==NULL || sysaddr.domain[0]) ) {
      sprintf(aka, "%d:%d/%d.%d", sysaddr.main.zone, sysaddr.main.net,
                                  sysaddr.main.node, sysaddr.main.point);
   } else if (sysaddr.main.point==0 && sysaddr.domain && sysaddr.domain[0] ) {
      sprintf(aka, "%d:%d/%d@%s", sysaddr.main.zone, sysaddr.main.net,
                                  sysaddr.main.node, sysaddr.domain);
   } else {
      sprintf(aka, "%d:%d/%d", sysaddr.main.zone, sysaddr.main.net,
                               sysaddr.main.node);
   } /* endif */

   return aka;
}


void Usage(const char *program)
{
  char *temp;
  printf("%s\n", temp=GenVersionStr( "fecfg2fconf", FC_VER_MAJOR,
		FC_VER_MINOR, FC_VER_PATCH, FC_VER_BRANCH, cvs_date ));
  nfree(temp);
  
  printf("\nUsage:\n"
         "\t%s [path]fastecho.cfg [output file]\n",
          basename(program));
}


void print_packers( Packers *packers, int packers_count )
{  int i;
   for (i = 0; i < packers_count; i++)
     if ( strlen(packers[i].tag) && strlen(packers[i].command) )
       fprintf( f_hpt,
                " Pack %-10s  %s $a $f\n",
                packers[i].tag,
                packers[i].command );
}


void print_unpackers( Unpackers *unpackers, int unpackers_count )
{  int i;

   for (i = 0; i < unpackers_count; i++) {

     if ( strlen(unpackers[i].command) ){
         fprintf( f_hpt, "# Unpack  \"" );
         switch(unpackers[i].callingconvention){
         case 0: /*default*/
             fprintf( f_hpt, "%-30s $a $f $p", unpackers[i].command);
             break;
         case 1: /*cd path*/
             fprintf( f_hpt, "cd $p ; %-22s $a", unpackers[i].command);
             break;
         case 2:
             fprintf( f_hpt, "%-30s $a $p $f", unpackers[i].command);
             break;
         case 3:
             fprintf( f_hpt, "%-30s $a $f $p", unpackers[i].command);
             break;
         case 4:
             fprintf( f_hpt, "%-30s $a $f #$p", unpackers[i].command);
             break;
         case 5:
             fprintf( f_hpt, "%-30s $a $f -d $p", unpackers[i].command);
             break;
         }
         fprintf( f_hpt, "\" ");
         switch(i){
         case ARC_SeaArc:
             fprintf( f_hpt, " 0 XX       # SeaARC");
             break;
         case ARC_PkArc:
             fprintf( f_hpt, " 0 1a       # PKARC");
             break;
         case ARC_Pak:
             fprintf( f_hpt, " -2 fe      # PAK");
             break;
         case ARC_ArcPlus:
             fprintf( f_hpt, " 0 XX       # ArcPlus");
             break;
         case ARC_Zoo:
             fprintf( f_hpt, " 0 5a4f4f   # ZOO");
             break;
         case ARC_PkZip:
             fprintf( f_hpt, " 0 504b0304 # PKZIP");
             break;
         case ARC_Lha:
             fprintf( f_hpt, " 2 2d6c68   # LHA");
             break;
         case ARC_Arj:
             fprintf( f_hpt, " 0 60ea     # ARJ");
             break;
         case ARC_Sqz:
             fprintf( f_hpt, " 0 XX       # SQZ");
             break;
         case ARC_RAR:
             fprintf( f_hpt, " 0 52617221 # RAR");
             break;
         case ARC_UC2:
             fprintf( f_hpt, " 0 XX       # UC2");
             break;
         case ARC_Unknown:
         default:
             fprintf( f_hpt, " 0 ??       # Unknown");
             break;
         }
         fprintf( f_hpt, "\n");
    }
  }
}


void print_carbon()
{  int i,c;

   for (i = 0; i < 10; i++) {
      if (*((char*)&config.CC[i])) {
         switch (config.CC[i].what) {
         case CC_FROM:
            fprintf(f_hpt, "CarbonFrom      ");
            break;
         case CC_TO:
            fprintf(f_hpt, "CarbonTo        ");
            break;
         case CC_SUBJECT:
            fprintf(f_hpt, "CarbonSubj      ");
            break;
         case CC_KLUDGE:
            fprintf(f_hpt, "CarbonKludge    ");
            break;
         default:
           break;
         } /* endswitch */
         fprintf(f_hpt, " %s\n", config.CC[i].object);
         for (c = 0; c < config.AreaCnt; c++) {
            if (config.CC[i].conference == area[c]->conference)
               fprintf(f_hpt, "CarbonCopy       %s\n\n", area[c]->name);
         } /* endfor */
      }
   } /* endfor */
}


void print_areas()
{  int i,ii,c;
   int a[] = { AREA_BADMAILBOARD, AREA_DUPEBOARD, AREA_NETMAIL,
               AREA_LOCAL, AREA_ECHOMAIL };

  for (ii=0; ii<5; ii++)
  {
    switch (a[ii]) {
       case AREA_ECHOMAIL:
          fprintf(f_hpt, "\n# Echo Areas\n");
          break;
       case AREA_NETMAIL:
          fprintf(f_hpt, "\n# Additional Netmail Areas\n");
          break;
       case AREA_LOCAL:
          fprintf(f_hpt, "\n# Local Areas\n");
          break;
       case AREA_BADMAILBOARD:
          fprintf(f_hpt, "\n# Badmail Area\n");
          break;
       case AREA_DUPEBOARD:
          fprintf(f_hpt, "\n# Dupemail Area\n");
          break;
    } /* endswitch */


    for (i = 0; i < config.AreaCnt; i++)
    {
       if( area[i]->flags.atype!=a[ii] )
         continue;

       if (area[i]->flags.storage == FE_FIDO || area[i]->flags.storage == FE_SQUISH || area[i]->flags.storage == FE_JAM || area[i]->flags.storage == FE_PASSTHRU)
       {
          switch (area[i]->flags.atype) {
             case AREA_ECHOMAIL:
                fprintf(f_hpt, "EchoArea   ");
                break;
             case AREA_NETMAIL:
                fprintf(f_hpt, "NetmailArea");
                break;
             case AREA_LOCAL:
                fprintf(f_hpt, "LocalArea  ");
                break;
             case AREA_BADMAILBOARD:
                fprintf(f_hpt, "BadArea    ");
                break;
             case AREA_DUPEBOARD:
                fprintf(f_hpt, "DupeArea   ");
                break;
             default:
               continue;
          } /* endswitch */

          fprintf(f_hpt, " %-16s", area[i]->name);

          if (area[i]->flags.storage == FE_PASSTHRU) {
             fprintf(f_hpt, " Passthrough");
          } else {
             fprintf(f_hpt, " %s", area[i]->path);
             if (area[i]->flags.storage == FE_SQUISH)
                fprintf(f_hpt, " -b Squish");
             else if (area[i]->flags.storage == FE_JAM)
                fprintf(f_hpt, " -b Jam");
          } /* endif */

          if (area[i]->info.group <= 25) /* A..Z */
             fprintf(f_hpt, " -g %c", 'A'+area[i]->info.group);
          else if (area[i]->info.group <= 35) /* 0..6, possible 0..9 */
             fprintf(f_hpt, " -g %d", area[i]->info.group-25);

          if (area[i]->desc && *area[i]->desc )
             fprintf(f_hpt, "\t-d \"%s\"\t", area[i]->desc);

          fprintf(f_hpt, " -lr %d", area[i]->read_sec);

          fprintf(f_hpt, " -lw %d", area[i]->write_sec);

          if (area[i]->advflags.hide)
             fprintf(f_hpt, " -hide");

          if (area[i]->advflags.mandatory)
             fprintf(f_hpt, " -mandatory");

          if (area[i]->advflags.manual)
             fprintf(f_hpt, " -manual");

          if (area[i]->advflags.tinyseen)
             fprintf(f_hpt, " -tinysb");

          if (area[i]->advflags.disablepsv)
             fprintf(f_hpt, " -nopause");

          fprintf(f_hpt, " -a %s", sysAddress2str(sysaddr[area[i]->info.aka]));

          if (area[i]->days>0)
             fprintf(f_hpt, " -p %d", area[i]->days);
          else if (area[i]->days==0 && config.def_days )
             fprintf(f_hpt, " -p %u", config.def_days);
          else if (area[i]->recvdays>0)
             fprintf(f_hpt, " -p %d", area[i]->recvdays);
          else if (area[i]->recvdays==0 && config.def_recvdays )
             fprintf(f_hpt, " -p %u", config.def_recvdays);
          else if (area[i]->recvdays==-1 || area[i]->days==-1 )
             fprintf(f_hpt, " -p 0");

          if (area[i]->messages )
             fprintf(f_hpt, " -$m %d", area[i]->messages );
          else if( area[i]->messages==0 && config.def_messages )
             fprintf(f_hpt, " -$m %d", config.def_messages );
          else if( area[i]->messages==-1 )
             fprintf(f_hpt, " -$m 0" );

          if (area[i]->flags.atype == AREA_ECHOMAIL && config.flags & KILLDUPES)
                fprintf(f_hpt, " -dupeCheck del");

          for (c = 0, fprintf(f_hpt, "\t"); c < config.NodeCnt; c++)
             if (GetBam(node[c]->areas, area[i]->conference))
                fprintf(f_hpt, " %s", FEaka2str(node[c]->addr));

          fprintf(f_hpt, "\n");

       } /* endif */
    } /* endfor */
  }
}


void  print_links()
{  int i, c;

   for (i = 0; i < config.NodeCnt; i++) {
      fprintf(f_hpt, "\nLink                     %s\n", node[i]->name);

      if ( node[i]->addr.zone == sysaddr[node[i]->aka].main.zone )
        fprintf(f_hpt, "Aka                      %s@%s\n", FEaka2str(node[i]->addr), sysaddr[node[i]->aka].domain);
      else
        fprintf(f_hpt, "Aka                      %s\n", FEaka2str(node[i]->addr));

      fprintf( f_hpt, "OurAka                   %s\n", sysAddress2str(sysaddr[node[i]->aka]) );

      if ( node[i]->password && node[i]->password[0] )
         fprintf(f_hpt, "PktPwd                   %s\n", strLower(node[i]->password));

      if ( node[i]->areafixpw && node[i]->areafixpw[0] )
         fprintf(f_hpt, "AreafixPWD               %s\n", strLower(node[i]->areafixpw));

      fprintf(f_hpt, "Level                    %d\n", node[i]->sec_level);

      if (node[i]->newgroup <= 25)
         fprintf(f_hpt, "LinkGrp                  %c\n", 'A'+node[i]->newgroup);
      else
         fprintf(f_hpt, "LinkGrp                  %d\n", node[i]->newgroup-25);

      fprintf(f_hpt, "AccessGrp                %s\n", grp2str(node[i]->groups));

      if (node[i]->flags.allowareacreate) {
         fprintf(f_hpt, "AutoAreaCreate           on\n");
         for (c = 0; c < config.GDCnt; c++) {

            if (node[i]->newgroup == groupdef[c]->group)
            {
               fprintf(f_hpt, "AutoAreaCreateDefaults  ");

/*
               if (groupdef[c]->group <= 25)
                  fprintf(f_hpt, " -g %c", 'A'+groupdef[c]->group);
               else if (groupdef[c]->group <= 35)
                  fprintf(f_hpt, " -g %d", groupdef[c]->group-25);
*/
               if( parseFEgroup(groupdef[c]->group) )
                  fprintf(f_hpt, " -g %c", parseFEgroup(groupdef[c]->group) );

               if (groupdef[c]->area.read_sec)
                  fprintf(f_hpt, " -lr %d", groupdef[c]->area.read_sec);

               if (groupdef[c]->area.write_sec)
                  fprintf(f_hpt, " -lw %d", groupdef[c]->area.write_sec);

               if (groupdef[c]->area.days)
                  fprintf(f_hpt, " -p %d", groupdef[c]->area.days);
               else if (groupdef[c]->area.recvdays)
                     fprintf(f_hpt, " -p %d", groupdef[c]->area.recvdays);

               if (groupdef[c]->area.messages)
                  fprintf(f_hpt, " -$m %d", groupdef[c]->area.messages);

               fprintf(f_hpt, "\n");
            } /* endif */
         } /* endfor */
      } /* endif */

      if (node[i]->maxarcsize)  /* -1 - don't set; 0 - use default; >0 = size */
         fprintf(f_hpt, "ArcmailSize              %u\n", node[i]->maxarcsize>0 ? node[i]->maxarcsize : 0);

      fprintf(f_hpt, "EchomailFlavour          ");
      switch (node[i]->flags.arc_status) {
      case NetNormal:
         if (node[i]->flags.arc_direct) {
            fprintf(f_hpt, "Direct\n");
         } else {
            fprintf(f_hpt, "Normal\n");
         } /* endif */
         break;
      case NetHold:
         fprintf(f_hpt, "Hold%s\n", node[i]->flags.arc_direct? "     # +Direct" : "");
         break;
      case NetCrash:
         fprintf(f_hpt, "Crash%s\n", node[i]->flags.arc_direct? "     # +Direct" : "");
         break;
/* Not implemented yet */
/*      case NetImm;
         fprintf(f_hpt, "Immediate%s\n", node[i]->flags.arc_direct? "     # +Direct" : "");
         break;
*/
      default:
        break;
      } /* endswitch */

      if (node[i]->flags.noattach)
        fprintf(f_hpt, "Export                   off   # This is not precision usage \"Echomail flavour: No attach\" from fastecho's configuration");

/* To future */
      fprintf(f_hpt, "#EchomailPackTo           %s   # Will be implement in hpt\n", FEaka2str(node[i]->arcdest));

/* To future */
      fprintf(f_hpt, "#AreafixFlavour           ");
      switch (node[i]->flags.mgr_status) {
      case NetNormal:
         if (node[i]->flags.mgr_direct) {
            fprintf(f_hpt, "Direct\n");
         } else {
            fprintf(f_hpt, "Normal\n");
         } /* endif */
         break;
      case NetHold:
         fprintf(f_hpt, "Hold%s\n", node[i]->flags.mgr_direct? "     # +Direct" : "");
         break;
      case NetCrash:
         fprintf(f_hpt, "Crash%s\n", node[i]->flags.mgr_direct? "     # +Direct" : "");
         break;
/* Not implemented yet */
/*      case NetImm;
         fprintf(f_hpt, "Immediate%s\n", node[i]->flags.mgr_direct? "     # +Direct" : "");
         break;
*/
      default:
        break;
      } /* endswitch */


/* To future */
/*
      fprintf(f_hpt, "### Fastecho's areafix flags for node     ##\n");
      fprintf(f_hpt, "# areafixtype %d\n", (int)node[i]->afixflags.bits.areafixtype);
      fprintf(f_hpt, "# allowremote %d\n", (int)node[i]->afixflags.bits.allowremote);
      fprintf(f_hpt, "# allowdelete %d\n", (int)node[i]->afixflags.bits.allowdelete);
      fprintf(f_hpt, "# allowrename %d\n", (int)node[i]->afixflags.bits.allowrename);
      fprintf(f_hpt, "# binarylist  %d\n", (int)node[i]->afixflags.bits.binarylist);
      fprintf(f_hpt, "# addplus     %d\n", (int)node[i]->afixflags.bits.addplus);
      fprintf(f_hpt, "# addtear     %d\n", (int)node[i]->afixflags.bits.addtear);

      fprintf(f_hpt, "# sendto      %d\n", (int)node[i]->afixflags.bits.sendto);
      fprintf(f_hpt, "# forward     %d\n", (int)node[i]->afixflags.bits.forward);
      fprintf(f_hpt, "# nosendrules %d\n", (int)node[i]->afixflags.bits.nosendrules);
      fprintf(f_hpt, "# resv        %d\n", (int)node[i]->afixflags.bits.resv);
      fprintf(f_hpt, "### Fastecho's areafix flags for node end ##\n");
*/
      if ( node[i]->afixflags.bits.areafixtype == FSC57AreaFix )
        fprintf(f_hpt, "AdvancedAreafix          on\n");

      if (node[i]->afixflags.bits.forward)
        fprintf(f_hpt, "ForwardRequests          on\n");

      for (c = 0; c < config.FWACnt; c++)
         if (i == frequest[c].nodenr) {
            if (!node[i]->afixflags.bits.forward)
              fprintf(f_hpt, "ForwardRequests          off\n");
            fprintf(f_hpt, "ForwardRequestFile       %s\n", strLower(frequest[c].file));
         }

      if (node[i]->afixflags.bits.nosendrules)
        fprintf(f_hpt, "NoRules                  on\n");

      switch (node[i]->afixflags.bits.sendto) {
      case AreaMgr:
         fprintf(f_hpt, "RemoteRobotName          AreaMgr\n");
         break;
      case AreaLink:
         fprintf(f_hpt, "RemoteRobotName          AreaLink\n");
         break;
      case EchoMgr:
         fprintf(f_hpt, "RemoteRobotName          EchoMgr\n");
         break;
      case AreaFix:
      default:
/* This is HPT's hardcoded default value
         fprintf(f_hpt, "RemoteRobotName          AreaFix\n");
*/
         break;
      } /* endswitch */


/*      fprintf(f_hpt, "AllowPktAddrDiffer       %s\n", ); */

      if (node[i]->flags.packer != 0x0f)
         fprintf(f_hpt, "Packer                   %s\n", packers[node[i]->flags.packer].tag);

      if (node[i]->autopassive)
         fprintf(f_hpt, "AutoPause                %u\n", (unsigned)node[i]->autopassive);

      if (node[i]->flags.passive)
         fprintf(f_hpt, "Pause                    earea\n");

      fprintf(f_hpt, "\n");
   } /* endfor */
}


int parseFEconfig()
{
   int c, i;

   c = 0;
   while (c < config.offset) {
      read_fe_extension_header(&header, f_cfg);
      switch (header.type) {
      case EH_AKAS:
         sysaddr = (SysAddress*)calloc((header.offset / FE_SYS_ADDRESS_SIZE),
                                       sizeof(SysAddress));
         for (i = 0; i < header.offset / FE_SYS_ADDRESS_SIZE; i++)
         {
             read_fe_sysaddress(sysaddr+i, f_cfg);
         }

         break;
      case EH_PACKERS:
         packers = (Packers*)calloc(header.offset / FE_PACKERS_SIZE,
                                    sizeof(Packers));
         for (i = 0; i < header.offset / FE_PACKERS_SIZE; i++)
             read_fe_packers(packers + i, f_cfg);
         packers_count = i;
         break;
      case EH_PACKERS2:
         packers2 = (Packers*)calloc(header.offset / FE_PACKERS_SIZE,
                                    sizeof(Packers));
         for (i = 0; i < header.offset / FE_PACKERS_SIZE; i++)
             read_fe_packers(packers2 + i, f_cfg);
         packers2_count = i;
         break;
      case EH_UNPACKERS:
         unpackers = (Unpackers*)calloc(header.offset / FE_UNPACKERS_SIZE,
                                    sizeof(Unpackers));
         for (i = 0; i < header.offset / FE_UNPACKERS_SIZE; i++)
             read_fe_unpackers(unpackers + i, f_cfg);
         unpackers_count = i;
         break;
      case EH_UNPACKERS2:
         unpackers2 = (Unpackers*)calloc(header.offset / FE_UNPACKERS_SIZE,
                                    sizeof(Unpackers));
         for (i = 0; i < header.offset / FE_UNPACKERS_SIZE; i++)
             read_fe_unpackers(unpackers2 + i, f_cfg);
         unpackers2_count = i;
         break;
      case EH_GRPDEFAULTS:
         groupdef = (GroupDefaults**)calloc(config.GDCnt,
                                            sizeof(GroupDefaults*));
         for (i = 0; i < config.GDCnt; i++) {
            groupdef[i] = (GroupDefaults*)malloc(sizeof(GroupDefaults));
            read_fe_groupdefaults(groupdef[i], f_cfg, config.GrpDefRecSize);
         } /* endfor */
         break;
      case EH_AREAFIX: /* 0x000d */
         frequest = (ForwardAreaFix*)calloc(header.offset /
                                            FE_FORWARD_AREAFIX_SIZE,
                                            sizeof(ForwardAreaFix));
         for (i = 0; i < header.offset / FE_FORWARD_AREAFIX_SIZE; i++)
             read_fe_frequest(frequest + i, f_cfg);
         break;
      default:
         fseek(f_cfg, header.offset, SEEK_CUR);
        break;
      } /* endswitch */
      c += header.offset+FE_EXTHEADER_SIZE;
      if (ftell(f_cfg) != c + FE_CONFIG_SIZE)
      {
          fprintf(stderr, "%s file seems to be currupt (exp %ld, found %ld)\n",
                  FEconfig, (long)c + FE_CONFIG_SIZE, (long)ftell(f_cfg));
          fclose(f_cfg);
          return 4;
      }
   } /* endwhile */

   fseek(f_cfg, FE_CONFIG_SIZE+config.offset, SEEK_SET);

   node = (Node**)calloc(config.NodeCnt, sizeof(Node*));
   for (i = 0; i < config.NodeCnt; i++) {
      node[i] = (Node*)malloc(sizeof(Node));
      assert(!read_fe_node(node[i], f_cfg, config.NodeRecSize));
   } /* endfor */

   fseek(f_cfg, FE_CONFIG_SIZE+config.offset+
         (config.NodeRecSize*config.NodeCnt), SEEK_SET);

   area = (Area**)calloc(config.AreaCnt, sizeof(Area*));
   for (i = 0; i < config.AreaCnt; i++) {
      area[i] = (Area*)malloc(sizeof(Area));
      read_fe_area(area[i], f_cfg);
   } /* endfor */

  return 0;
}


void disposeFEconfig()
{
   int i;

   for (i = 0; i < config.AreaCnt; i++) {
      nfree(area[i]);
   }
   nfree(area);
   for (i = 0; i < config.NodeCnt; i++) {
       free_fe_node(node[i]);
       nfree(node[i]);
   } /* endfor */
   nfree(frequest);
   nfree(node);
   nfree(sysaddr);
   nfree(packers);
   nfree(unpackers);
   nfree(packers2);
   nfree(unpackers2);
   for (i = 0; i < config.GDCnt; i++) {
      free_fe_groupdefaults(groupdef[i]);
      nfree(groupdef[i]);
   } /* endfor */
   nfree(groupdef);
}


int main(int argc, char **argv)
{
   int   i;
   char  *pp, *Version, *output_file = OUTPUT_FILE;

   if (argc == 1) {
      Usage(argv[0]);
      exit(1);
   } /* endif */

   f_cfg = fopen( (FEconfig = argv[1]), "rb" );
   if (!f_cfg) {
      fprintf(stderr, "\nCan\'t open %s file.\n", argv[1]);
      exit(2);
   } /* endif */

   if(argc>2)
     output_file = argv[2];

   read_fe_config(&config, f_cfg);

   if (config.revision != REVISION) {
      fprintf(stderr, "%s file is not fastecho.cfg 1.46\n", argv[1]);
      fclose(f_cfg);
      exit(4);
   } /* endif */

   i = parseFEconfig();
   if( i ) exit(i);

   fclose(f_cfg);

   /* Extract program version from $Revision$ */
   for(Version = Revision; *Version && !isdigit(*Version); Version++); /* Skip to digit */
   for(pp=Version; *pp && (isdigit(*pp) || *pp=='.'); pp++);           /* Seek for number end */
   *pp = '\0'; /* Trim after number */

   f_hpt = fopen(output_file, "wt");
   if (!f_hpt) {
      fprintf(stderr, "\nCan\'t open %s file\n", output_file);
      fclose(f_cfg);
      exit(3);
   } /* endif */

   printf ("Writing %s. Please manually check this file!\n", output_file);

   fprintf(f_hpt, "# fastecho v1.46 config (fastecho.cfg) -> %s. (c) 2:5020/960@FidoNet\n", output_file);
   fprintf(f_hpt, "# Check this file, please!\n\n");

   fprintf(f_hpt, "Version %s\t# Program version\n\n", Version);

   fprintf(f_hpt, "##################################################################\n");
   fprintf(f_hpt, "# System\n\n");
   fprintf(f_hpt, "Sysop                    %s\n", config.sysops[0].name);
   for (i = 0; i < config.AkaCnt; i++) {
      if (*(char*)&sysaddr[i]) {
        fprintf(f_hpt, "Address                  %s\n", sysAddress2str(sysaddr[i]));
      } /* endif */
   } /* endfor */

   fprintf(f_hpt, "\n");

   if( config.UnprotInBound && *config.UnprotInBound )
     fprintf(f_hpt, "Inbound                  %s\n", config.UnprotInBound);
   if( config.InBound && *config.InBound )
     fprintf(f_hpt, "ProtInbound              %s\n", config.InBound);
   if( config.TempInBound && *config.TempInBound )
     fprintf(f_hpt, "TempInbound              %s\n", config.TempInBound);
   if( config.OutBound && *config.OutBound )
     fprintf(f_hpt, "Outbound                 %s\n", config.OutBound);
   if( config.TempPath && *config.TempPath )
     fprintf(f_hpt, "TempOutbound             %s\n", config.TempPath);
   if(config.SwapPath && *config.SwapPath)
     fprintf(f_hpt, "TempDir                  %s\n", config.SwapPath);
   if( config.SemaphorePath && *config.SemaphorePath )
     fprintf(f_hpt, "busyFileDir              %s\n", config.SemaphorePath);
   if( config.LocalInBound && *config.LocalInBound )
     fprintf(f_hpt, "LocalInBound             %s\n", config.LocalInBound);
   if( config.RulesPrefix && *config.RulesPrefix )
     fprintf(f_hpt, "%sRulesDir                 %s\n",
                    config.AreaFixFlags & SENDCONFERENCERULES? "": "# ",
                    config.RulesDir);
   pp = strrchr(config.LogFile, '\\');
   if(pp){
     *pp=0;
     fprintf(f_hpt, "Logfiledir               %s\n", config.LogFile);
   }

   fprintf(f_hpt, "\n");
   switch( config.loglevel ){
   case 2:
           fprintf(f_hpt, "LogLevels                0-z   # loglevel Full\n");
           fprintf(f_hpt, "ScreenLogLevels          0-z\n");
           break;
   case 1:
           fprintf(f_hpt, "LogLevels                0-C   # loglevel Norm\n");
           fprintf(f_hpt, "ScreenLogLevels          0-C\n");
           break;
   case 0:
           fprintf(f_hpt, "LogLevels \t\t# loglevel None\n");
           fprintf(f_hpt, "ScreenLogLevels\n");
           break;
   }
    /* Graphical tossing ? (Norm|Full) : None */
   fprintf(f_hpt, "LogEchoToScreen          %s\n", config.graphics ? "on" : "off");

   fprintf(f_hpt, "\n");
   if( config.ExtAfter && *config.ExtAfter )
     fprintf(f_hpt, "AfterUnpack              %s\n", config.ExtAfter);
   if( config.ExtBefore && *config.ExtBefore )
     fprintf(f_hpt, "BeforePack               %s\n", config.ExtBefore);

   if( config.AreaFixHelp && *config.AreaFixHelp )
     fprintf(f_hpt, "AreaFixHelp              %s\n", config.AreaFixHelp);

   if( config.compressfree )
     fprintf(f_hpt, "MinDiskFreeSpace         %d\n", config.compressfree);

   if( config.maxPKT )
     fprintf(f_hpt, "AreafixMsgSize           %u\n", config.maxPKT);

/*
   if( config. && *config. )
     fprintf(f_hpt, " \t%s\n", config.);
*/

   fprintf( f_hpt, "\n");
   fprintf( f_hpt, "AreafixKillRequests      %s\n",
            config.AreaFixFlags & KEEPREQUEST ? "on" : "off" );
   fprintf( f_hpt, "AreafixKillReports       %s\n",
            config.AreaFixFlags & KEEPRECEIPT ? "on" : "off" );
   fprintf( f_hpt, "areafixQueryReports      %s\n",
            config.AreaFixFlags & ADDRECEIPTLIST ? "on" : "off" );

   if( packers_count ){
     fprintf(f_hpt, "\n##################################################################\n");
     fprintf(f_hpt, "# Packers (DOS)\n\n");
     print_packers(packers,packers_count);
   }
   if( packers2_count ){
     fprintf(f_hpt, "\n##################################################################\n");
     fprintf(f_hpt, "# Packers (OS/2)\n\n");
     print_packers(packers2,packers2_count);
   }

   if( unpackers_count ){
     fprintf(f_hpt, "\n##################################################################\n");
     fprintf(f_hpt, "# Unpackers (DOS)\n\n");

     if( sstrlen(config.Unpacker) )
       fprintf( f_hpt, "# Unpack  \"%-30s $a $f $p\"  # Default unpacker", config.Unpacker );

     print_unpackers(unpackers,unpackers_count);

   }

   if( unpackers2_count ){
     fprintf(f_hpt, "\n##################################################################\n");
     fprintf(f_hpt, "# Unpackers (OS/2)\n\n");

     if( sstrlen(config.Unpacker2) )
       fprintf( f_hpt, "# Unpack  \"%-30s $a $f $p\"  # Default unpacker", config.Unpacker2 );

     print_unpackers(unpackers2,unpackers2_count);

   }

   fprintf(f_hpt, "\n##################################################################\n");
   fprintf(f_hpt, "# Nodes\n\n");

   fprintf(f_hpt, "\nLinkDefaults\n");

   if( config.maxarcsize )
     fprintf(f_hpt, "ArcmailSize              %u\n", config.maxarcsize);
   if( config.maxPKT )
     fprintf(f_hpt, "PktSize                  %u\n", config.maxPKT);
   fprintf(f_hpt, "AllowEmptyPktPwd         %s\n", config.security>1 ? "off" : "on");
/*   if( config. && *config. )
     fprintf(f_hpt, "                         %s\n", config.);
   if( config. && *config. )
     fprintf(f_hpt, "                         %s\n", config.);
*/
   fprintf(f_hpt, "LinkDefaults end\n");


   print_links();



   fprintf(f_hpt, "\n##################################################################\n");
   fprintf(f_hpt, "# Areas\n\n");

   fprintf(f_hpt, "EchoAreaDefaults  -dupeCheck move -dupeHistory 11");
/* Use default values in each echoarea if echoarea falue is -1 */
/* if( config.def_days )
     fprintf(f_hpt, "-p %u ", config.def_days);
   else if( config.def_recvdays )
     fprintf(f_hpt, "-p %u ", config.def_recvdays);
   if( config.def_messages )
     fprintf(f_hpt, "-$m %u ", config.def_messages);
*/
   fprintf(f_hpt, "\n\n");

   fprintf(f_hpt, "# Main netmail\nNetmailArea Netmail          %s\n", config.NetMPath);

   print_areas();

   fprintf(f_hpt, "\n");
   fprintf(f_hpt, "\n##################################################################\n");
   fprintf(f_hpt, "# Carbon copy\n\n");

   print_carbon();

   fclose(f_hpt);

   disposeFEconfig();

   return 0;
}
