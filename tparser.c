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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fidoconf.h"
#include "xstr.h"
#include "common.h"

void printAddr(const s_addr addr)
{
  if (addr.domain != NULL)
    printf("%d:%d/%d.%d@%s ", addr.zone, addr.net, addr.node, addr.point, addr.domain);
  else
    printf("%d:%d/%d.%d ", addr.zone, addr.net, addr.node, addr.point);
}

void printArea(s_area area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("Description: ");
   if (area.description != NULL)
     printf("%s",area.description);
   printf("\n-> %s\t", area.fileName ? area.fileName : "");
   if (area.msgbType == MSGTYPE_SDM) printf("SDM");  
   else if (area.msgbType == MSGTYPE_SQUISH) printf("Squish");
   else if (area.msgbType == MSGTYPE_JAM) printf("Jam");
   else printf("Passthrough");

   if (area.useAka->domain != NULL)
     printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   else
     printf("\t Use %d:%d/%d.%d", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point);
   printf("\n");
   printf("DOS Style File (8+3) - %s\n", (area.DOSFile) ? "on" : "off");
   printf("Level read  - %d\n", area.levelread);
   printf("Level write - %d\n", area.levelwrite);
   if (area.group) printf("Group       - %s\n", area.group);
   if (area.nopack) {
      printf("pack never (ignoging: max: %u msgs\tpurge: %u days)\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   } else {
      printf("max: %u msgs\tpurge: %u days\tdupeHistory %u\n", area.max, area.purge, area.dupeHistory);
   }
   if (area.downlinkCount) printf("Links:\n");
   else printf("No links\n");
   for (i = 0; i<area.downlinkCount;i++) { 
       printf("\t");
       printAddr(area.downlinks[i]->link->hisAka);
       printf(" level %d,", area.downlinks[i]->link->level);
/*       printf(" exp. %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" imp. %s,", (area.downlinks[i]->import) ? "on" : "off");*/
       if(area.downlinks[i]->export || area.downlinks[i]->import)
           printf(" "); else printf(" no access");
       if(area.downlinks[i]->export)
           printf("read");
       if(area.downlinks[i]->export && area.downlinks[i]->import)
           printf("/");
       if(area.downlinks[i]->import)
           printf("write");
       if((area.downlinks[i]->export + area.downlinks[i]->import)==1)
           printf(" only");
       printf(",");
       printf(" defLink %s,", (area.downlinks[i]->defLink) ? "on" : "off");
       printf(" mand. %s.\n", (area.downlinks[i]->mandatory) ? "on" : "off");
   }
   printf("Options: ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   if (area.tinySB) printf("tinySB ");
   if (area.killSB) printf("killSB ");
   if (area.mandatory) printf("mandatory ");
   if (area.nolink) printf("nolink ");
   if (area.ccoff) printf("ccoff ");
   if (area.keepsb) printf("keepsb ");
   if (area.killRead) printf("killRead ");
   if (area.keepUnread) printf("keepUnread ");
   if (area.debug) printf("debug ");
   printf("\n");
   printf("DupeCheck: ");
   if (area.dupeCheck==dcOff) printf("off");
   if (area.dupeCheck==dcMove) printf("move");
   if (area.dupeCheck==dcDel) printf("delete");
   printf("\n");
   if (area.sbaddCount) {
	   printf("addSeenBys: ");
	   for (i=0; i<area.sbaddCount; i++) 
		   printf("%u/%u ", area.sbadd[i].net,area.sbadd[i].node);
	   printf("\n");
   }
   if (area.sbignCount) {
	   printf("IgnoreSeenBys: ");
	   for (i=0; i<area.sbignCount; i++) 
		   printf("%u/%u ", area.sbign[i].net,area.sbign[i].node);
	   printf("\n");
   }
   printf("-------\n");
}

void printFileArea(s_filearea area) {
   int i;
   
   printf("%s \n", area.areaName);
   printf("Description: %s\n",(area.description) ? area.description : "");
   if (area.pass != 1)
      printf("Path: %s\t", area.pathName);
   else
      printf("Passthrough filearea");

   if (area.useAka == NULL)
     printf ("\t Use ??? (not configured)");
   else if (area.useAka->domain != NULL)
     printf("\t Use %d:%d/%d.%d@%s", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point, area.useAka->domain);
   else
     printf("\t Use %d:%d/%d.%d", area.useAka->zone, area.useAka->net, area.useAka->node, area.useAka->point);
   printf("\n");
   if (area.group) printf("Group       - %s\n", area.group);
   if (area.downlinkCount) printf("Links:\n");
   else printf("No links\n");
   for (i = 0; i<area.downlinkCount;i++) { 
       printf("\t");
       printAddr(area.downlinks[i]->link->hisAka);
       printf(" level %d,", area.downlinks[i]->link->level);
       if(area.downlinks[i]->export || area.downlinks[i]->import)
           printf(" "); else printf(" no access");
       if(area.downlinks[i]->export)
           printf("receive");
       if(area.downlinks[i]->export && area.downlinks[i]->import)
           printf("/");
       if(area.downlinks[i]->import)
           printf("send");
       if((area.downlinks[i]->export + area.downlinks[i]->import)==1)
           printf(" only");
       printf(",");
/*       printf(" export %s,", (area.downlinks[i]->export) ? "on" : "off");
       printf(" import %s,", (area.downlinks[i]->import) ? "on" : "off");    */
       printf(" mandatory %s.\n", (area.downlinks[i]->mandatory) ? "on" : "off");
   }
   printf("Options: ");
   if (area.mandatory) printf("mandatory ");
   if (area.sendorig) printf("sendorig ");
   if (area.hide) printf("hide ");
   if (area.noPause) printf("noPause ");
   printf("\n");
   printf("-------\n");
}

void printBbsArea(s_bbsarea area) {
   
   printf("%s \n", area.areaName);
   printf("Description: %s\n",area.description);
   printf("Path: %s\t", area.pathName);

   printf("\n");
   printf("-------\n");
}

void printFilelist(s_filelist *fl)
{
  switch (fl->flType)
  {
  case flDir:
    printf("type: directory\n");
    break;

  case flGlobal:
    printf("type: global\n");
    break;

  case flDirList:
    printf("type: dirlist\n");
    break;

  default:
    printf("internal error: unknown flType!\n");
    break;
  }

  printf("destination file: %s\n", fl->destFile);
  if ((fl->flType == flGlobal) || (fl->flType == flDir))
  {
    printf("directory header template: %s\n", fl->dirHdrTpl);
    printf("directory entry template: %s\n", fl->dirEntryTpl);
    printf("directory footer template: %s\n", fl->dirFtrTpl);
  }

  switch (fl->flType)
  {
  case flGlobal:
    printf("global header template: %s\n", fl->globHdrTpl);
    printf("global footer template: %s\n", fl->globFtrTpl);
    break;

  case flDirList:
    printf("dirlist header template: %s\n", fl->dirListHdrTpl);
    printf("dirlist entry template: %s\n", fl->dirListEntryTpl);
    printf("dirlist footer template: %s\n", fl->dirListFtrTpl);
    break;

  case flDir:
    // just avoid a warning
    break;
  }
  printf("-------\n");
}

static char *cvtFlavour(e_flavour flavour)
{
   switch (flavour) {
      case hold:      return "hold";
      case normal:    return "normal";
      case direct:    return "direct";
      case crash:     return "crash";
      case immediate: return "immediate";
      default:        fprintf(stderr, "Unknown flavour, update tparser!\n");
                      return "";
   }
}

void printLink(s_link link) {
	unsigned int i;

  if ((link.hisAka.domain != NULL) && (link.ourAka->domain != NULL)) {
    printf("Link: %d:%d/%d.%d@%s (ourAka %d:%d/%d.%d@%s)\n",
	   link.hisAka.zone, link.hisAka.net, link.hisAka.node, link.hisAka.point, link.hisAka.domain,
	   link.ourAka->zone, link.ourAka->net, link.ourAka->node, link.ourAka->point, link.ourAka->domain);
  }
    else
    {
      printf("Link: %d:%d/%d.%d (ourAddres %d:%d/%d.%d)\n",
	   link.hisAka.zone, link.hisAka.net, link.hisAka.node, link.hisAka.point,
	   link.ourAka->zone, link.ourAka->net, link.ourAka->node, link.ourAka->point);
    }
   printf("Name: %s\n", link.name);
   if (link.defaultPwd) printf("defaultPwd: %s\n", link.defaultPwd);
   if (link.pktPwd) printf("pktPwd:     %s\n", link.pktPwd);
   if (link.ticPwd) printf("ticPwd:     %s\n", link.ticPwd);
   if (link.areaFixPwd) printf("areafixPwd: %s\n", link.areaFixPwd);
   if (link.fileFixPwd) printf("filefixPwd: %s\n", link.fileFixPwd);
   if (link.bbsPwd) printf("bbsPwd:     %s\n", link.bbsPwd);
   if (link.sessionPwd) printf("sessionPwd: %s\n", link.sessionPwd);
   if (link.handle!=link.name) printf("handle:     %s\n", link.handle);
   if (link.email)
   {
     printf("email:      %s\n", link.email);

     switch (link.emailEncoding)
     {
     case eeMIME:
       printf("emailEncoding: MIME\n");
       break;

     case eeSEAT:
       printf("emailEncoding: SEAT\n");
       break;

     case eeUUE:
       printf("emailEncoding: UUE\n");
       break;

     default:
       printf("Internal error: Unknown encoding #%d!\n", link.emailEncoding);
     }
   }
   if (link.emailFrom) printf("emailFrom:  %s\n", link.emailFrom);
   if (link.emailSubj) printf("emailSubj:  %s\n", link.emailSubj);
   printf("Level:      %u\n", link.level);
   printf("Export:     %s\n",(link.export) ? "on" : "off");
   printf("Import:     %s\n",(link.import) ? "on" : "off");
   printf("Mandatory:  %s\n",(link.mandatory) ? "on" : "off");
   if (link.Pause) printf("Link in Pause, no export\n");
   if (link.autoPause) printf("AutoPause over %u days\n", link.autoPause);
   if (link.numOptGrp > 0) {
	   printf("OptGrp       ");
	   for (i = 0; i < link.numOptGrp; i++) {
		   if (i > 0) printf(",");
		   printf("%s", link.optGrp[i]);
	   }
	   printf("\n");
   }
   printf("AutoAreaCreate %s\n", (link.autoAreaCreate) ? "on" : "off");
   printf("FileFixFSC87Subset %s\n", (link.FileFixFSC87Subset) ? "on" : "off");
   printf("AutoAreaCreateSubdirs %s\n", (link.autoAreaCreateSubdirs) ? "on" : "off");
   if (link.autoAreaCreateFile) printf("AutoAreaCreateFile: %s\n", link.autoAreaCreateFile);
   if (link.numFrMask > 0) {
	   printf("ForwardRequestMask: ");
	   for (i = 0; i < link.numFrMask; i++) {
		   if (i > 0) printf(",");
		   printf("%s", link.frMask[i]);
	   }
	   printf("\n");
   }
   if (link.numDfMask > 0) {
	   printf("DenyFwdMask: ");
	   for (i = 0; i < link.numDfMask; i++) {
		   if (i > 0) printf(",");
		   printf("%s", link.dfMask[i]);
	   }
	   printf("\n");
   }
   if (link.autoAreaCreateDefaults) printf("AutoAreaCreateDefaults: %s\n", link.autoAreaCreateDefaults);
   printf("AutoFileCreate %s\n", (link.autoFileCreate) ? "on" : "off");
   printf("AutoFileCreateSubdirs %s\n", (link.autoFileCreateSubdirs) ? "on" : "off");
   if (link.autoFileCreateFile) printf("AutoFileCreateFile: %s\n", link.autoFileCreateFile);
   if (link.autoFileCreateDefaults) printf("AutoFileCreateDefaults: %s\n", link.autoFileCreateDefaults);
   if (link.LinkGrp) printf("LinkGrp %s\n",link.LinkGrp);
   if (link.numAccessGrp)
   {
     unsigned int i;

     printf("AccessGrp ");
     for (i = 0; i < link.numAccessGrp; i++)
     {
       if (i > 0) printf(", %s", link.AccessGrp[i]);
       else printf("%s", link.AccessGrp[0]);
     }
     printf("\n");
   }
   printf("AreaFix %s\n", (link.AreaFix) ? "on" : "off");
   if (link.afixEchoLimit) printf("AreaFixEchoLimit %u\n", link.afixEchoLimit);
   printf("FileFix %s\n", (link.FileFix) ? "on" : "off");
   printf("Forward Requests to this link is %s\n",(link.forwardRequests)?"on":"off");
   if (link.forwardAreaPriority)
	   printf("ForwardAreaPriority: %u\n", link.forwardAreaPriority);
   printf("Forward Requests Access: %s\n", (link.denyFRA) ? "off" : "on");
   printf("Unconditional Forward Requests Access: %s\n",(link.denyUFRA)?"off":"on");
   if (link.RemoteRobotName) printf("RemoteRobotName %s\n", link.RemoteRobotName);
   else printf("RemoteRobotName areafix\n");
   if (link.forwardRequestFile) printf("ForwardRequestFile %s\n",link.forwardRequestFile);
   if (link.denyFwdFile) printf("DenyFwdFile %s\n",link.denyFwdFile);
   if (link.msgBaseDir) printf("MsgBaseDir %s\n",link.msgBaseDir);
   if (link.packerDef) printf("PackerDefault %s\n", link.packerDef->packer);
   else printf("PackerDefault none\n");
   if (link.fileBox)  {
       printf("fileBox %s\n", link.fileBox);
       printf("fileBoxAlways: %s\n", link.fileBoxAlways ? "on": "off");
   }

   if (link.pktSize != 0) printf("pktSize - %u kb\n",link.pktSize);
   if (link.arcmailSize != 0) printf("arcmailSize - %u kb\n",link.arcmailSize);

   if (link.packerDef)
   {
       printf("packNetmail: %s\n",(link.packNetmail)?"on":"off");
       if (link.packNetmail)
           printf("maxUnpackedNetmail: %d kb\n", link.maxUnpackedNetmail);
   }
   else
       if (link.packNetmail)
           printf("Packer not defined but packNetmail is on\n");

   printf("TIC files %s\n", (link.noTIC == 0) ? "on" : "off");
   printf("forwardPkts ");
   switch (link.forwardPkts){
   case fOff : printf("off\n");
     break;
   case fSecure : printf("secure\n");
     break;
   case fOn : printf("on\n");
     break;
   }
   printf("allowEmptyPktPwd ");
   switch (link.allowEmptyPktPwd){
   case eOff : printf("off\n");
   break;
   case eSecure : printf("secure\n");
     break;
   case eOn : printf("on\n");
     break;
   }
   printf("allowPktAddrDiffer ");
   switch (link.allowPktAddrDiffer) {
   case pdOff : printf("off\n");
     break;
   case pdOn : printf("on\n");
     break;
   default: fprintf(stderr, "Error in keyword allowPktAddrDiffer\n");
   }
   printf("AdvancedAreaFix %s\n", (link.advancedAreafix) ? "on" : "off");
   
   switch (link.linkBundleNameStyle) {
      case eUndef:
         //Don't print senseless information... printf("linkBundleNameStyle: undefined (like BundleNameStyle)\n");
         break;
      case eAddrDiff:
         printf("linkBundleNameStyle: addrDiff\n");
         break;
      case eAddrDiffAlways:
         printf("linkBundleNameStyle: addrDiffAlways\n");
         break;
      case eTimeStamp:
         printf("linkBundleNameStyle: timeStamp\n");
         break;
      case eAmiga:
		 printf("linkBundleNameStyle: Amiga\n");
		 break;
      default:
         printf("Warning: linkBundleNameStyle is UNKNOWN! Update tparser please!\n");
         break;
   }
   printf("arcNetmail %s\n", (link.arcNetmail) ? "on" : "off");
   printf("echoMailFlavour %s\n", cvtFlavour(link.echoMailFlavour));
   printf("fileEchoFlavour %s\n", cvtFlavour(link.fileEchoFlavour));
   printf("noRules %s\n", (link.noRules) ? "on" : "off");

   printf("-------\n");
}

// Some dumb checks ;-)
void checkLogic(s_fidoconfig *config) {
	int i,j,k;
	s_link *link;
	s_area *area;
	char *areaName;

	for (i=0; i+1<config->linkCount; i++) {
		for (j=i+1; j<config->linkCount; j++) {
			if (addrComp(config->links[i].hisAka, config->links[j].hisAka) == 0) {
				
				if (strcmp(config->links[i].name,
						   config->links[j].name)!=0) continue;
				
				printf("ERROR: duplication of link %d:%d/%d.%d\n",
					   config->links[i].hisAka.zone,
					   config->links[i].hisAka.net,
					   config->links[i].hisAka.node,
					   config->links[i].hisAka.point);
				printf("remove it, or change the name!\n");
				exit(-1);
			}
		}
	}	

	for (i=0; i<config->echoAreaCount; i++) {

		area = &(config->echoAreas[i]);
		areaName = area->areaName;

		//   j=i+1
		for (j=i+1; j < config->echoAreaCount; j++) {
			if (stricmp(config->echoAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}
      
		for (j=0; j < config->localAreaCount; j++) {
			if (stricmp(config->localAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		for (j=0; j < config->netMailAreaCount; j++) {
			if (stricmp(config->netMailAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		// Check for area link duplication
		for (j=0; j+1<area->downlinkCount; j++) {
			link = area->downlinks[j]->link;
			for (k=j+1;k<area->downlinkCount; k++) {
				if (link == area->downlinks[k]->link) {
					printf("ERROR: duplication of link %d:%d/%d.%d in area %s\n",
						   link->hisAka.zone,
						   link->hisAka.net,
						   link->hisAka.node,
						   link->hisAka.point,
						   areaName);
					exit(-1);
				}
			}
		}
	}

	for (i=0; i<config->localAreaCount; i++) {

		area = &(config->localAreas[i]);
		areaName = config->localAreas[i].areaName;

		for (j=0; j < config->echoAreaCount; j++) {
			if (stricmp(config->echoAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}
      
		//   j=i+1
		for (j=i+1; j < config->localAreaCount; j++) {
			if (stricmp(config->localAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		for (j=0; j < config->netMailAreaCount; j++) {
			if (stricmp(config->netMailAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		// Check for area link duplication
		for (j=0; j+1<area->downlinkCount; j++) {
			link = area->downlinks[j]->link;
			for (k=j+1;k<area->downlinkCount; k++) {
				if (link == area->downlinks[k]->link) {
					printf("ERROR: duplication of link %d:%d/%d.%d in area %s\n",
						   link->hisAka.zone,
						   link->hisAka.net,
						   link->hisAka.node,
						   link->hisAka.point,
						   areaName);
					exit(-1);
				}
			}
		}
	}

	for (i=0; i<config->netMailAreaCount; i++) {
	   
		area = &(config->netMailAreas[i]);
		areaName = config->netMailAreas[i].areaName;

		for (j=0; j < config->echoAreaCount; j++) {
			if (stricmp(config->echoAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		for (j=0; j < config->localAreaCount; j++) {
			if (stricmp(config->localAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		//   j=i+1
		for (j=i+1; j < config->netMailAreaCount; j++) {
			if (stricmp(config->netMailAreas[j].areaName, areaName)==0) {
				printf("ERROR: duplication of area %s\n", areaName);
				exit(-1);
			}
		}

		// Check for area link duplication
		for (j=0; j+1<area->downlinkCount; j++) {
			link = area->downlinks[j]->link;
			for (k=j+1;k<area->downlinkCount; k++) {
				if (link == area->downlinks[k]->link) {
					printf("ERROR: duplication of link %d:%d/%d.%d in area %s\n",
						   link->hisAka.zone,
						   link->hisAka.net,
						   link->hisAka.node,
						   link->hisAka.point,
						   areaName);
					exit(-1);
				}
			}
		}
	}
}

void printCarbons(s_fidoconfig *config) {

    int i;
    s_carbon *cb;
    char *crbKey="", *nspc, *cbaName;

    printf("\n=== CarbonCopy ===\n");
    printf("CarbonAndQuit %s\n", (config->carbonAndQuit) ? "on" : "off");
    printf("CarbonKeepSb %s\n", (config->carbonKeepSb) ? "on" : "off");
    printf("CarbonOut %s\n", (config->carbonOut) ? "on" : "off");
    printf("ExcludePassthroughCarbon %s\n", (config->exclPassCC) ? "on" : "off");
    printf("Exclude \" * Forward from area \" string: %s\n\n", 
	   (config->carbonExcludeFwdFrom) ? "on" : "off");

    for (i = 0, cb=&(config->carbons[0]); i< config->carbonCount; i++, cb++) {

        if (cb->rule&CC_NOT){
            nspc="";
            printf("|NOT ");
        }else
            nspc="|    ";

        switch(cb->ctype){
        case ct_to:
            crbKey= "To:       ";
            break;
        case ct_from:
            crbKey= "From:     ";
            break;
        case ct_kludge:
            crbKey= "Kludge:   ";
            break;
        case ct_subject:
            crbKey= "Subj:     ";
            break;
        case ct_msgtext:
            crbKey= "Text:     ";
            break;
        case ct_fromarea:
            crbKey= "FromArea: ";
            break;
        case ct_group:
            crbKey= "Groups:   ";
            break;
        case ct_addr:
            crbKey= "Addr:     ";
            break;
        }

        printf("%sCarbon%s\"%s\"\n",
	       nspc, crbKey, (cb->ctype==ct_addr) ? aka2str(cb->addr) : cb->str);
        if (cb->rule&CC_AND)
            continue;

        if(cb->areaName!=NULL){
            cbaName = cb->areaName;
            if(*cbaName=='*')
                ++cbaName;
        } else cbaName = "UNKNOWN";
        if (cb->extspawn)
            printf("CarbonExtern: \"%s\"", cbaName);
        else {

            switch (cb->move) {
            case 0:
                printf("CarbonCopy:          ");
                break;
            case 2:
                printf("CarbonDelete");
                break;
            case 1:
            default:
                printf("CarbonMove:          ");
                break;
            }
            if (cb->areaName) {
                if(stricmp(cbaName, cb->area->areaName)) {
                    printf(" !!! \"%s\" wanted !!! using \"%s\"", cbaName, cb->area->areaName);
                } else {
                    printf("\"%s\"", cb->area->areaName);
                }
            } else
                if (cb->move != 2) printf(" !!! No area specified !!!");
        }
        putchar('\n');
        if (cb->reason) printf("CarbonReason:   %s\n", cb->reason);
        if (cb->export) printf("Copied messages will be exported.\n");
        if (cb->netMail) printf("Active on netMail\n");
        if(cb->areaName!=NULL)
            if (*cb->areaName=='*') printf("CarbonAndQuit ignored.\n");
        printf("-------\n");
    }
}


int main(int argc, char **argv) {
   s_fidoconfig *config = NULL;
   int i, j, hpt=0;
   char *cfgFile=NULL, *module;

   for (i=1; i<argc; i++)
   {
       if (argv[i][0]=='-' && argv[i][1]=='D') {
           char *p=strchr(argv[i], '=');
           if (p) {
               *p='\0';
               setvar(argv[i]+2, p+1);
               *p='=';
           } else {
               setvar(argv[i]+2, "");
           }
       }
       else if (stricmp(argv[i], "--help") == 0 ||
                stricmp(argv[i], "-h") == 0 ||
                cfgFile != NULL) {
           printf("run: tparser [-Dvar=value] [/path/to/config/file]\n");
	   return 0;
       } else
           xstrcat(&cfgFile, argv[i]);
   }

   module = getvar("module");
   printf("module: ");
   if (module) {
       printf("%s\n", module);
       if (stricmp(module,"hpt")==0) hpt=1;
   } else printf("all modules\n");

   config = readConfig(cfgFile);
   nfree(cfgFile);

   if (config != NULL) {
	  checkLogic(config);
      printf("=== MAIN CONFIG ===\n");
      printf("Comment character: '%c'\n", config->CommentChar);
      printf("Version: %u.%u\n", config->cfgVersionMajor, config->cfgVersionMinor);
      if (config->name != NULL)	printf("Name:     %s\n", config->name);
      if (config->sysop != NULL) printf("Sysop:    %s\n", config->sysop);
      if (config->location != NULL)printf("Location: %s\n", config->location);
      for (i=0; i<config->addrCount; i++) {
	 if (config->addr[i].domain != NULL)
            printf("Addr: %u:%u/%u.%u@%s\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point, config->addr[i].domain);
	 else
            printf("Addr: %u:%u/%u.%u\n", config->addr[i].zone, config->addr[i].net, config->addr[i].node, config->addr[i].point);
      }

      if (config->loglevels) printf("LogLevels %s\n", config->loglevels);
      printf("LogEchoToScreen %s\n", (config->logEchoToScreen) ? "on" : "off");
      if (config->logEchoToScreen && config->screenloglevels)
	 printf("ScreenLogLevels %s\n", config->screenloglevels);

      if (config->echotosslog != NULL) printf("EchoTossLog:     %s\n", config->echotosslog);
      if (config->statlog != NULL) printf("StatLog:         %s\n", config->statlog);

      if (config->inbound != NULL) printf("Inbound:         %s\n", config->inbound);
      if (config->tempInbound != NULL) printf("tempInbound:     %s\n", config->tempInbound);
      if (config->protInbound != NULL) printf("ProtInbound:     %s\n", config->protInbound);
      if (config->localInbound != NULL) printf("LocalInbound:    %s\n", config->localInbound);
      if (config->listInbound != NULL) printf("ListInbound:     %s\n", config->listInbound);
      if (config->ticOutbound != NULL) printf("TicOutbound:     %s\n", config->ticOutbound);
      if (config->outbound != NULL) printf("Outbound:        %s\n", config->outbound);
      if (config->tempOutbound != NULL) printf("tempOutbound:    %s\n", config->tempOutbound);
      for (i=0; i< config->publicCount; i++) printf("Public: #%u %s\n", i+1, config->publicDir[i]);
      if (config->reqidxDir) printf ("ReqIdxDir:       %s\n", config->reqidxDir);
      if (config->dupeHistoryDir != NULL) printf("DupeHistoryDir:  %s\n", config->dupeHistoryDir);
      if (config->logFileDir != NULL) printf("LogFileDir:      %s\n", config->logFileDir);
      if (config->msgBaseDir != NULL) printf("MsgBaseDir:      %s\n", config->msgBaseDir);
      if (config->fileAreaBaseDir) printf("FileAreaBaseDir: %s\n", config->fileAreaBaseDir);
      if (config->passFileAreaDir) printf("passFileAreaDir: %s\n", config->passFileAreaDir);
      if (config->busyFileDir) printf("busyFileDir:     %s\n", config->busyFileDir);
      if (config->magic) printf("Magic: %s\n", config->magic);
      if (config->semaDir) printf("semaDir:         %s\n", config->semaDir);
      if (config->badFilesDir) printf("BadFilesDir:     %s\n", config->badFilesDir);
      if (config->rulesDir) printf("rulesDir:        %s\n", config->rulesDir);
      if (config->msgidfile) printf("MsgIDFile:       %s\n", config->msgidfile);
      if (config->hptPerlFile) printf("hptPerlFile:     %s\n", config->hptPerlFile);

//      printf("CreateDirs: %s\n",(config->createDirs) ? "on": "off");
      if (config->netmailFlag) printf("NetmailFlag:     %s\n",config->netmailFlag);
      if (config->aacFlag) printf("AutoAreaCreateFlag: %s\n",config->aacFlag);
      if (config->minDiskFreeSpace) 
		  printf("MinDiskFreeSpace: %u Mb\n", config->minDiskFreeSpace);
      if (config->syslogFacility)
          printf ("SyslogFacility: %d\n", config->syslogFacility);

	  if (config->lockfile) {
		  printf("LockFile: %s\n",config->lockfile);
		  printf("AdvisoryLock: %s\n", config->advisoryLock ? "on" : "off");
	  }

      if (hpt==0) {
          printf("LongDirNames: %s\n",(config->longDirNames) ? "on": "off");
          printf("SplitDirs: %s\n",(config->splitDirs) ? "on": "off");
      }

      printf("Ignore Capability Word: %s\n",(config->ignoreCapWord) ? "on": "off");
      printf("ProcessBundles %s\n",(config->noProcessBundles) ? "off" : "on");
      switch (config->bundleNameStyle) {
	  case eUndef:
		  printf("BundleNameStyle: undefined (timeStamp)\n");
		  break;
	  case eAddrDiff:
		  printf("BundleNameStyle: addrDiff\n");
		  break;
	  case eAddrDiffAlways:
	          printf("BundleNameStyle: addrDiffAlways\n");
	          break;
	  case eTimeStamp:
		  printf("BundleNameStyle: timeStamp\n");
		  break;
	  case eAmiga:
		  printf("BundleNameStyle: Amiga\n");
		  break;
	  default:
		  printf("Warning: BundleNameStyle is UNKNOWN! Update tparser please!\n");
		  break;
		  
      }		  

      if (config->fileBoxesDir) printf ("fileBoxesDir: %s\n", config->fileBoxesDir);
      printf("DupeBaseType: ");
      if (config->typeDupeBase==textDupes) printf("textDupes\n");
      if (config->typeDupeBase==hashDupes) printf("hashDupes\n");
      if (config->typeDupeBase==hashDupesWmsgid) printf("hashDupesWmsgid\n");
      if (config->typeDupeBase==commonDupeBase) {
            printf("commonDupeBase\n");
            printf("AreasMaxDupeAge: %d\n",config->areasMaxDupeAge);
      }

      if (config->numPublicGroup > 0) {
          printf("PublicGroups: ");
          for (i = 0; i < config->numPublicGroup; i++)
              printf( (i>0) ? ",%s" : "%s", config->PublicGroup[i]);
          printf("\n");
      }

      printf("createAreasCase: %s\n", (config->createAreasCase == eLower) ? "Lower" : "Upper");
      printf("areasFileNameCase: %s\n", (config->areasFileNameCase == eLower) ? "Lower" : "Upper");
      printf("DisableTID: %s\n", (config->disableTID) ? "on" : "off");
      printf("keepTrsMail: %s\n", (config->keepTrsMail) ? "on" : "off");
      printf("keepTrsFiles: %s\n", (config->keepTrsFiles) ? "on" : "off");
	  printf("createFwdNonPass: %s\n", config->createFwdNonPass ? "on" : "off");
#if defined ( __NT__ )
      printf("SetConsoleTitle: %s\n", (config->setConsoleTitle) ? "on" : "off");
#endif
      if (config->processPkt != NULL) printf("processPkt: %s\n", config->processPkt);
      if (config->tossingExt != NULL) printf("tossingExt: %s\n", config->tossingExt);

	  if (config->addToSeenCount) {
		  printf("AddToSeen:");
		  for (i=0; i<config->addToSeenCount; i++) {
			  printf(" %u/%u", config->addToSeen[i].net,config->addToSeen[i].node );
		  }
		  printf("\n");
	  }
	  if (config->ignoreSeenCount) {
		  printf("IgnoreSeen:");
		  for (i=0; i<config->ignoreSeenCount; i++) {
			  printf(" %u/%u", config->ignoreSeen[i].net,config->ignoreSeen[i].node );
		  }
		  printf("\n");
	  }
	  
	  if (config->tearline || config->origin) printf("\n");
	  if (config->tearline) printf("--- %s\n", config->tearline);
	  if (config->origin) printf("* Origin: %s (%s)\n", config->origin, aka2str(config->addr[0]));
	  printf("AutoPassive: %s\n", config->autoPassive ? "on" : "off");
	  printf("NotValidFileNameChars: %s\n", config->notValidFNChars ?
		 config->notValidFNChars : "\"*/:;<=>?\\|%`'&+");

      printf("\n=== AREAFIX CONFIG ===\n");
	  if (config->areafixNames) printf("AreafixNames: %s\n", config->areafixNames);
	  printf("areafixFromPkt: %s\n",(config->areafixFromPkt) ? "on": "off");
	  printf("areafixQueryReports: %s\n",(config->areafixQueryReports)?"on":"off");
          printf("areafixQueryRequests: %s\n",(config->areafixQueryRequests)?"on":"off");
	  printf("areafixKillReports: %s\n",(config->areafixKillReports)?"on":"off");
	  printf("areafixKillRequests: %s\n",(config->areafixKillRequests)?"on":"off");
	  if (config->areafixMsgSize) printf("areafixMsgSize - %u\n", config->areafixMsgSize);
	  if (config->areafixSplitStr) printf("areafixSplitStr - \"%s\"\n", config->areafixSplitStr);
	  if (config->areafixOrigin) printf("areafixOrigin - \"%s\"\n", config->areafixOrigin);
	  printf("RobotsArea: %s\n",(config->robotsArea)?config->robotsArea:"all areas");
	  if (config->areafixhelp) printf("areafixHelp: %s\n",config->areafixhelp);

  if (hpt==0) {
      printf("\n=== FILEFIX CONFIG ===\n");
	  printf("filefixKillReports: %s\n",(config->filefixKillReports)?"on":"off");
	  printf("filefixKillRequests: %s\n",(config->filefixKillRequests)?"on":"off");

      printf("\n=== TICKER CONFIG ===\n");
      if (config->fileAreasLog) printf("FileAreasLog: %s\n", config->fileAreasLog);
      if (config->fileNewAreasLog) printf("FileNewAreasLog: %s\n", config->fileNewAreasLog);
      if (config->fileArcList) printf("FileArcList: %s\n", config->fileArcList);
      if (config->filePassList) printf("FileArcList: %s\n", config->filePassList);
      if (config->fileDupeList) printf("FileArcList: %s\n", config->fileDupeList);
      printf("AddDLC: %s\n",(config->addDLC) ? "on": "off");
      printf("FileSingleDescLine: %s\n",(config->fileSingleDescLine) ? "on": "off");
      printf("FileCheckDest: %s\n",(config->fileCheckDest) ? "on": "off");
      printf("FileDescPos: %u\n", config->fileDescPos);
      if (config->fileLDescString) printf("FileLDescString: %s\n", config->fileLDescString);
      printf("DLCDigits: %u\n", config->DLCDigits);
      printf("FileMaxDupeAge: %u\n", config->fileMaxDupeAge);
      printf("FileFileUMask: %o\n", config->fileFileUMask);
      printf("FileDirUMask: %o\n", config->fileDirUMask);
      if (config->fileLocalPwd) printf("FileLocalPwd: %s\n", config->fileLocalPwd);
  }

      printf("\n=== FILELIST CONFIG ===\n");
      for (i = 0; i < config->filelistCount; i++) printFilelist(&(config->filelists[i]));

      printf("\n=== LINKER CONFIG ===\n");
      switch (config->LinkWithImportlog)
      {
      case lwiYes:
	printf("LinkWithImportlog   Yes\n");
	break;

      case lwiNo:
	printf("LinkWithImportlog   No\n");
	break;

      case lwiKill:
	printf("LinkWithImportlog   Kill\n");
	break;

      default:
	printf("Internal error: Unknown value #%d for LinkWithImportLog!\n", config->LinkWithImportlog);
      }

      printf("\n=== LINK CONFIG ===\n");
      printf("%u links in config\n", config->linkCount);
      for (i = 0; i < config->linkCount; i++) printLink(config->links[i]);
      
      printf("\n=== AREA CONFIG ===\n");
	  
	  printf("kludgeAreaNetmail ");
	  switch (config->kludgeAreaNetmail) {
	  case kanKill: printf("kill");
		  break;
	  case kanIgnore: printf("ignore");
		  break;
	  case kanEcho: printf ("echomail");
		  break;
	  }
	  printf("\n");

      if (config->netMailAreaCount == 0) { printf("you must define at least one NetmailArea!\n"); return 1; }
      printf("\n=== Net&EchoAreas ===\n");
      for (i = 0; i< config->netMailAreaCount; i++) {
         printArea(config->netMailAreas[i]);
      }
      if (config->dupeArea.areaName == NULL)
	{ printf("you must define DupeArea!\n"); return 1; }
      if (config->dupeArea.fileName != NULL) printArea(config->dupeArea);
	else { printf("DupeArea can not be passthrough!\n"); return 1; }
      if (config->badArea.areaName == NULL)
	{ printf("you must define BadArea!\n"); return 1; }
      if (config->badArea.fileName != NULL) printArea(config->badArea);
	else { printf("BadArea can not be passthrough!\n"); return 1; }
      for (i = 0; i< config->echoAreaCount; i++) {
         printArea(config->echoAreas[i]);
      }
      printf("\n=== LocalAreas ===\n");
      for (i = 0; i< config->localAreaCount; i++) {
         printArea(config->localAreas[i]);
      }

   if (hpt==0) {
      printf("\n=== FileAreas ===\n");
      for (i=0; i<config->fileAreaCount; i++) {
        printFileArea(config->fileAreas[i]);
      }
      printf("\n=== BbsAreas ===\n");
      for (i=0; i<config->bbsAreaCount; i++) {
        printBbsArea(config->bbsAreas[i]);
      }
   }


      if(config->carbonCount)
         printCarbons(config);

      printf("\n=== ROUTE CONFIG ===\n");
      for (i = 0; i < config->routeCount; i++) {
         if (config->route[i].routeVia == 0)
            printf("Route %s via %u:%u/%u.%u\n", config->route[i].pattern, config->route[i].target->hisAka.zone, config->route[i].target->hisAka.net, config->route[i].target->hisAka.node, config->route[i].target->hisAka.point);
         else {
			 printf("Route");
			 switch (config->route[i].id) {
			 case id_route : break;
			 case id_routeMail : printf("Mail"); break;
			 case id_routeFile : printf("File"); break;
			 }
			 printf(" %s ", config->route[i].pattern);
			 switch (config->route[i].routeVia) {
			 case route_zero: printf("zero\n"); break;
			 case noroute:  printf("direct\n"); break;
			 case nopack:   printf("nopack\n"); break;
			 case host:     printf("via host\n"); break;
			 case hub:      printf("via hub\n"); break;
			 case boss:     printf("via boss\n"); break;
			 case route_extern: break; /* internal only */
			 }
         }
      }

   if (hpt==0) {
      printf("\n=== NODELIST CONFIG ===\n");
      if (config->nodelistDir != NULL)
        {
          printf("NodelistDir: %s\n", config->nodelistDir);
        }
      if (config->fidoUserList != NULL)
        {
          printf("Fidouser List File: %s\n", config->fidoUserList);
        }
      printf("-------\n");
   
      for (i = 0; i < config->nodelistCount; i++)
        {
          printf("Nodelist %s\n", config->nodelists[i].nodelistName);
          if (config->nodelists[i].diffUpdateStem != NULL)
            printf("Nodediff Update File %s\n",
                   config->nodelists[i].diffUpdateStem);
          if (config->nodelists[i].fullUpdateStem != NULL)
            printf("Full Nodelist Update File %s\n",
                   config->nodelists[i].fullUpdateStem);
          if (config->nodelists[i].defaultZone != 0)
            printf("Zone Number %d\n",
                   config->nodelists[i].defaultZone);
          switch (config->nodelists[i].format)
            {
            case fts5000:
              printf ("Standard nodelist format\n");
              break;
            case points24:
              printf ("Points24 nodelist format\n");
              break;
            case points4d:
              printf ("Points4D nodelist format\n");
              break;
            default:
              printf ("Unknown nodelist format???\n");
              break;
            }
          printf("-------\n");
        }
   }
      printf("\n=== PACK CONFIG ===\n");
      for (i = 0; i < config->packCount; i++) {
         printf("Packer: %s      Call: %s\n", config->pack[i].packer, config->pack[i].call);
      }
      if (config->defarcmailSize!=0) printf("\nDefault Arcmail Size - %u kb\n",config->defarcmailSize);
      printf("\n=== UNPACK CONFIG ===\n");
      for (i = 0; i < config->unpackCount; i++) {
         printf("UnPacker:  Call: %s Offset %d Match code ", config->unpack[i].call, config->unpack[i].offset);
         for (j = 0; j < config->unpack[i].codeSize; j++)
           printf("%02x", (int) config->unpack[i].matchCode[j]);
         printf(" Mask : ");
         for (j = 0; j < config->unpack[i].codeSize; j++)
           printf("%02x", (int) config->unpack[i].mask[j]);
         printf("\n");
      }
	  
      if (config->beforePack) printf("Before Pack - \"%s\"\n",config->beforePack);
      if (config->afterUnpack) printf("After Unpack - \"%s\"\n",config->afterUnpack);

      if (config->ReportTo) printf("ReportTo\t%s\n", config->ReportTo);

   if (hpt==0) {
      printf("\n=== EXEC CONFIG ===\n");
      for (i = 0; i < config->execonfileCount; i++) {
         printf("ExecOnFile: Area %s File %s Call %s\n",
                 config->execonfile[i].filearea,
                 config->execonfile[i].filename,
                 config->execonfile[i].command);
      }
   }

      disposeConfig(config);
   } /* endif */
   return 0;
}
