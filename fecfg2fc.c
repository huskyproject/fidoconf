/*****************************************************************************
*
*       Copyright (C) 1999
*
*       Fedor Lizunkov
*
*       Fido: 2:5020/960
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <smapi/typedefs.h>
#include <smapi/compiler.h>
#include <smapi/stamp.h>
#include <smapi/progprot.h>

//#define INC_FE_TYPES
#define INC_FE_BAMPROCS
#define FALSE 0
#define TRUE 1
#include "fecfg146.h"


// convert FastEcho-GroupBitmap to GroupString for fidoconfig
// warning: returns pointer to static array!
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

   // strip last ',' if any
   if (curr != buff) curr--;

   // terminate buff
   *curr = 0;

   return buff;
}

char *aka2str(FEAddress addr)
{
   static char aka[24];

   if (addr.point) {
      sprintf(aka, "%d:%d/%d.%d", addr.zone, addr.net, addr.node, addr.point);
   } else {
      sprintf(aka, "%d:%d/%d", addr.zone, addr.net, addr.node);
   } /* endif */

   return aka;
}

char *strLwr(char *str)
{
   char *ptr;

   ptr = str;

   while (*ptr) {
      *ptr = tolower(*ptr);
      ptr++;
   }
   return str;
}

int main(int argc, char **argv)
{
   FILE            *f_cfg, *f_hpt;
   CONFIG          config;
   Area            **area;
   ExtensionHeader header;
   SysAddress      *sysaddr = NULL;
   Packers         *packers = NULL;
   GroupDefaults   **groupdef = NULL;
   Node            **node;
   ForwardAreaFix  *frequest = NULL;
   int             i, c, stop;

   if (argc == 1) {
      printf("\nUse - [path]fastecho.cfg\n");
      exit(1);
   } /* endif */

   f_cfg = fopen(argv[1], "rb");
   if (!f_cfg) {
      fprintf(stderr, "\nCan\'t open %s file.\n", argv[1]);
      exit(2);
   } /* endif */

   read_fe_config(&config, f_cfg);

   if (config.revision != REVISION) {
      fprintf(stderr, "%s file is not fastecho.cfg 1.46\n", argv[1]);
      fclose(f_cfg);
      exit(4);
   } /* endif */

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
      case EH_PACKERS2:
         packers = (Packers*)calloc(header.offset / FE_PACKERS_SIZE, 
                                    sizeof(Packers));
         for (i = 0; i < header.offset / FE_PACKERS_SIZE; i++)
             read_fe_packers(packers + i, f_cfg); 
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
                  argv[1], (long)c + FE_CONFIG_SIZE, (long)ftell(f_cfg));
          fclose(f_cfg);
          exit(4);
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

   fclose(f_cfg);
   f_hpt = fopen("hpt_temp.cfg", "wt");
   if (!f_hpt) {
      fprintf(stderr, "\nCan\'t open hpt_temp.cfg file\n");
      fclose(f_cfg);
      exit(3);
   } /* endif */

   printf ("Writing hpt_temp.cfg. Please manually check this file!\n");

   fprintf(f_hpt, "# fastecho.cfg 1.46 -> hpt_temp.cfg. (c) 2:5020/960@FidoNet\n\n");
   fprintf(f_hpt, "# Check this file, please!\n\n");

   fprintf(f_hpt, "##################################################################\n");
   fprintf(f_hpt, "# System\n\n");
   fprintf(f_hpt, "Sysop %s\n", config.sysops[0].name);
   for (i = 0; i < config.AkaCnt; i++) {
      if (*(char*)&sysaddr[i]) {
         fprintf(f_hpt, "Address %s\n", aka2str(sysaddr[i].main));
      } /* endif */
   } /* endfor */

   fprintf(f_hpt, "\n");

   fprintf(f_hpt, "Inbound\t%s\n", config.UnprotInBound);
   fprintf(f_hpt, "ProtInbound\t%s\n", config.InBound);
   fprintf(f_hpt, "TempInbound\t%s\n", config.TempInBound);
   fprintf(f_hpt, "Outbound\t%s\n", config.OutBound);
   fprintf(f_hpt, "TempOutbound\t%s\n", config.TempPath);


   fprintf(f_hpt, "\n##################################################################\n");
   fprintf(f_hpt, "# Nodes\n\n");
   for (i = 0; i < config.NodeCnt; i++) {
      fprintf(f_hpt, "Link %s\n", node[i]->name);
      fprintf(f_hpt, "Aka %s\n", aka2str(node[i]->addr));
      fprintf(f_hpt, "OurAka %s\n", aka2str(sysaddr[node[i]->aka].main));
      if (stricmp(node[i]->password, node[i]->areafixpw) == 0) {
         fprintf(f_hpt, "Password %s\n", strLwr(node[i]->password));
      } else {
         fprintf(f_hpt, "Password %s\n", strLwr(node[i]->password));
         fprintf(f_hpt, "AreafixPWD %s\n", strLwr(node[i]->areafixpw));
      } /* endif */
      fprintf(f_hpt, "Level %d\n", node[i]->sec_level);
      if (node[i]->newgroup <= 25) {
         fprintf(f_hpt, "LinkGrp %c\n", 'A'+node[i]->newgroup);
      } else {
         fprintf(f_hpt, "LinkGrp %d\n", node[i]->newgroup-25);
      } /* endif */
      fprintf(f_hpt, "AccessGrp %s\n", grp2str(node[i]->groups));
      if (node[i]->flags.allowareacreate) {
         fprintf(f_hpt, "AutoAreaCreate on\n");
         for (c = 0; c < config.GDCnt; c++) {
            if (node[i]->newgroup == groupdef[c]->group) {
               fprintf(f_hpt, "AutoAreaCreateDefaults -g");
               if (groupdef[c]->group <= 25) {
                  fprintf(f_hpt, " %c", 'A'+groupdef[c]->group);
               } else {
                  fprintf(f_hpt, " %d", groupdef[c]->group-25);
               } /* endif */
               if (groupdef[c]->area.read_sec) {
                  fprintf(f_hpt, " -lr %d", groupdef[c]->area.read_sec);
               } /* endif */
               if (groupdef[c]->area.write_sec) {
                  fprintf(f_hpt, " -lw %d", groupdef[c]->area.write_sec);
               } else {
               } /* endif */
               if (groupdef[c]->area.days) {
                  fprintf(f_hpt, " -p %d", groupdef[c]->area.days);
               } else {
                  if (groupdef[c]->area.recvdays) {
                     fprintf(f_hpt, " -p %d", groupdef[c]->area.recvdays);
                  } /* endif */
               } /* endif */
               if (groupdef[c]->area.messages) {
                  fprintf(f_hpt, " -$m %d", groupdef[c]->area.messages);
               } /* endif */
               if (config.flags & KILLDUPES) {
                  fprintf(f_hpt, " -dupeCheck del");
               } else {
                  fprintf(f_hpt, " -dupeCheck move");
               } /* endif */
               fprintf(f_hpt, " -dupeHistory 11\n");
            } /* endif */
         } /* endfor */
      } /* endif */
      for (c = 0; c < config.FWACnt; c++) {
         if (i == frequest[c].nodenr) {
            fprintf(f_hpt, "ForwardRequests on\n");
            fprintf(f_hpt, "ForwardRequestFile %s\n", strLwr(frequest[c].file));
         } else {
         } /* endif */
      } /* endfor */
      fprintf(f_hpt, "EchomailFlavour ");
      switch (node[i]->flags.arc_status) {
      case 0:
         if (node[i]->flags.arc_direct) {
            fprintf(f_hpt, "Direct\n");
         } else {
            fprintf(f_hpt, "Normal\n");
         } /* endif */
         break;
      case 1:
         fprintf(f_hpt, "Hold\n");
         break;
      case 2:
         fprintf(f_hpt, "Crash\n");
         break;
      default:
        break;
      } /* endswitch */
      if (node[i]->flags.packer != 0x0f) {
         fprintf(f_hpt, "Packer %s\n", packers[node[i]->flags.packer].tag);
      } /* endif */
      if (node[i]->flags.passive) {
         fprintf(f_hpt, "Pause\n");
      } /* endif */
      fprintf(f_hpt, "\n");
   } /* endfor */


   fprintf(f_hpt, "\n##################################################################\n");
   fprintf(f_hpt, "# Areas\n\n");
   fprintf(f_hpt, "NetmailArea NetmailArea %s\n\n", config.NetMPath);
   for (i = stop = 0; i < config.AreaCnt; i++) {
      if (area[i]->flags.storage == FE_FIDO || area[i]->flags.storage == FE_SQUISH || area[i]->flags.storage == FE_JAM || area[i]->flags.storage == FE_PASSTHRU) {
         switch (area[i]->flags.atype) {
            case AREA_ECHOMAIL:
               fprintf(f_hpt, "EchoArea");
               break;
            case AREA_NETMAIL:
               fprintf(f_hpt, "#NetmailArea");
               break;
            case AREA_LOCAL:
               fprintf(f_hpt, "LocalArea");
               break;
            default:
               stop++;
              break;
         } /* endswitch */
         if (stop) {
            stop = 0;
            continue;
         } else {
         } /* endif */
         fprintf(f_hpt, " %s", area[i]->name);
         if (area[i]->flags.storage == FE_PASSTHRU) {
            fprintf(f_hpt, " Passthrough");
         } else {
            fprintf(f_hpt, " %s", area[i]->path);
            if (area[i]->flags.storage == FE_SQUISH) {
               fprintf(f_hpt, " -b Squish");
            } else if (area[i]->flags.storage == FE_JAM) {
               fprintf(f_hpt, " -b Jam");
            } /* endif */

         } /* endif */
         if (area[i]->info.group <= 25) {
            fprintf(f_hpt, " -g %c", 'A'+area[i]->info.group);
         } else {
            fprintf(f_hpt, " -g %d", area[i]->info.group-25);
         } /* endif */
         if (area[i]->read_sec) {
            fprintf(f_hpt, " -lr %d", area[i]->read_sec);
         } /* endif */
         if (area[i]->write_sec) {
            fprintf(f_hpt, " -lw %d", area[i]->write_sec);
         } /* endif */
         if (area[i]->advflags.hide) {
            fprintf(f_hpt, " -h");
         } /* endif */
         if (area[i]->advflags.mandatory) {
            fprintf(f_hpt, " -mandatory");
         } /* endif */
         if (area[i]->advflags.tinyseen) {
            fprintf(f_hpt, " -tinysb");
         } /* endif */
         if (area[i]->advflags.disablepsv) {
            fprintf(f_hpt, " -nopause");
         } else {
         } /* endif */
         fprintf(f_hpt, " -a %s", aka2str(sysaddr[area[i]->info.aka].main));
         if (area[i]->days) {
            fprintf(f_hpt, " -p %d", area[i]->days);
         } else {
            if (area[i]->recvdays) {
               fprintf(f_hpt, " -p %d", area[i]->recvdays);
            } /* endif */
         } /* endif */
         if (area[i]->messages) {
            fprintf(f_hpt, " -$m %d", area[i]->messages);
         } /* endif */
         if (area[i]->flags.atype == AREA_ECHOMAIL) {
            if (config.flags & KILLDUPES) {
               fprintf(f_hpt, " -dupeCheck del");
            } else {
               fprintf(f_hpt, " -dupeCheck move");
            } /* endif */
            fprintf(f_hpt, " -dupeHistory 11");
         } /* endif */
         for (c = 0; c < config.NodeCnt; c++) {
            if (GetBam(node[c]->areas, area[i]->conference)) {
               fprintf(f_hpt, " %s", aka2str(node[c]->addr));
            } /* endif */
         } /* endfor */
         fprintf(f_hpt, "\n");
      } /* endif */
   } /* endfor */


   fprintf(f_hpt, "\n");

   for (i = 0; i < 10; i++) {
      if (*((char*)&config.CC[i])) {
         switch (config.CC[i].what) {
         case CC_FROM:
            fprintf(f_hpt, "CarbonFrom");
            break;
         case CC_TO:
            fprintf(f_hpt, "CarbonTo");
            break;
         case CC_SUBJECT:
            fprintf(f_hpt, "CarbonSubj");
            break;
         case CC_KLUDGE:
            fprintf(f_hpt, "CarbonKludge");
            break;
         default:
           break;
         } /* endswitch */
         fprintf(f_hpt, " %s\n", config.CC[i].object);
         for (c = 0; c < config.AreaCnt; c++) {
            if (config.CC[i].conference == area[c]->conference) {
               fprintf(f_hpt, "CarbonArea %s\n\n", area[c]->name);
            } else {
            } /* endif */
         } /* endfor */
      } else {
      } /* endif */
   } /* endfor */

   for (i = 0; i < config.AreaCnt; i++) {
      free(area[i]);
   }
   free(area);
   for (i = 0; i < config.NodeCnt; i++) {
       free_fe_node(node[i]);
       free(node[i]);
   } /* endfor */
   free(frequest);
   free(node);
   free(sysaddr);
   free(packers);
   for (i = 0; i < config.GDCnt; i++) {
      free_fe_groupdefaults(groupdef[i]);
      free(groupdef[i]);
   } /* endfor */
   free(groupdef);
   fclose(f_hpt);

   return 0;
}
