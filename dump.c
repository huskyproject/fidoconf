#include <string.h>

#include "fidoconf.h"
#include "common.h"

void dumpString(FILE *f, char *text, char *str)
{
  if (str != NULL)
    fprintf(f, text, str);
}

void dumpHeader(s_fidoconfig *config, FILE *f)
{
//  fprintf(f, "commentChar %c%c\n", config->CommentChar, config->CommentChar);
  fprintf(f, "# This config was created from a memory config by fidoconfig dumpConfigToFile()!\n\n");

  fprintf(f,    "Version             %u.%02u\n", config->cfgVersionMajor,
	  config->cfgVersionMinor);
  dumpString(f, "Name                %s\n", config->name);
  dumpString(f, "Sysop               %s\n", config->sysop);
  dumpString(f, "Location            %s\n\n", config->location);
  
  dumpString(f, "LogLevels           %s\n\n", config->loglevels);
  if (config->logEchoToScreen != 0) fprintf(f, "logEchoToScreen\n");
  if (config->createDirs != 0) fprintf(f, "CreateDirs\n");
  if (config->longDirNames != 0) fprintf(f, "LongDirNames\n");
  if (config->splitDirs != 0) fprintf(f, "SplitDirs\n");
  fprintf(f, "createAreasCase %s\n", (config->createAreasCase == eLower) ? "Lower" : "Upper");
  fprintf(f, "areasFileNameCase %s\n", (config->areasFileNameCase == eLower) ? "Lower" : "Upper");
  if (config->disableTID != 0) fprintf(f, "DisableTID\n");
  if (config->keepTrsMail != 0) fprintf(f, "KeepTrsMail\n");
  if (config->keepTrsFiles != 0) fprintf(f, "KeepTrsFiles\n");
  if (config->createFwdNonPass != 0) fprintf(f, "createFwdNonPass\n");
  if (config->autoPassive != 0) printf("autoPassive\n");
  if (config->processPkt != NULL) fprintf(f, "processPkt %s\n", config->processPkt);
  if (config->tossingExt != NULL) fprintf(f, "tossingExt %s\n", config->tossingExt);
  switch (config->bundleNameStyle) {
  case eUndef:
	  // Can't print undeclared value
	  break;
  case eAddrDiff:
	  fprintf(f,"BundleNameStyle: addrDiff\n");
	  break;
  case eAddrDiffAlways:
	  fprintf(f,"BundleNameStyle: addrDiffAlways\n");
	  break;
  case eTimeStamp:
	  fprintf(f,"BundleNameStyle: timeStamp\n");
	  break;
  default:
	  printf("Warning: BundleNameStyle is UNKNOWN! Update dump please!\n");
	  break;
	  
  }		  
  
  if (config->netmailFlag) fprintf(f,"NetmailFlag %s\n",config->netmailFlag);
  if (config->aacFlag) fprintf(f,"AutoAreaCreateFlag %s\n",config->aacFlag);

  fprintf(f, "\n");
}

void dumpAddrs(s_fidoconfig *config, FILE *f)
{
  int i;

  for (i = 0; i < config->addrCount; i++)
    fprintf(f, "Address             %s\n", aka2str(config->addr[i]));
  fprintf(f, "\n");
}

void dumpPack(s_fidoconfig *config, FILE *f)
{
  int i, j;

  for (i = 0; i < config->packCount; i++)
  {
    fprintf(f, "Pack                %s \"%s\"\n", config->pack[i].packer, 
            config->pack[i].call);
  }

  for (i = 0; i < config->unpackCount; i++)
  {
    fprintf(f, "Unpack              \"%s\" %u ", config->unpack[i].call,
            config->unpack[i].offset);
    for (j = 0; j < config->unpack[i].codeSize; j++)
    {
      if ((config->unpack[i].mask[j] & 0xF0) == 0) fprintf(f, "?");
      else fprintf(f, "%1x", (config->unpack[i].matchCode[j] & 0xF0) >> 4);
      if ((config->unpack[i].mask[j] & 0x0F) == 0) fprintf(f, "?");
      else fprintf(f, "%1x", config->unpack[i].matchCode[j] & 0x0F);
    }

    fprintf(f, "\n");
  }

  if (config->separateBundles != 0) fprintf(f, "separateBundles\n");
  if (config->defarcmailSize != 0) fprintf(f, "defarcmailSize      %d\n",
					  config->defarcmailSize);
  
  fprintf(f, "\n");
}

void dumpPaths(s_fidoconfig *config, FILE *f)
{
  int i;

  dumpString(f, "Inbound             %s\n", config->inbound);
  dumpString(f, "ListInbound         %s\n", config->listInbound);
  dumpString(f, "ProtInbound         %s\n", config->protInbound);
  dumpString(f, "TempInbound         %s\n", config->tempInbound);
  dumpString(f, "LocalInbound        %s\n", config->localInbound);

  dumpString(f, "Outbound            %s\n", config->outbound);
  dumpString(f, "TICOutbound         %s\n", config->ticOutbound);
  dumpString(f, "TempOutbound        %s\n", config->tempOutbound);
  
  dumpString(f, "LogFileDir          %s\n", config->logFileDir);
  dumpString(f, "DupeHistoryDir      %s\n", config->dupeHistoryDir);
  dumpString(f, "MsgBaseDir          %s\n", config->msgBaseDir);
  dumpString(f, "NodelistDir         %s\n", config->nodelistDir);
  dumpString(f, "SemaDir             %s\n", config->semaDir);
  dumpString(f, "BadFilesDir         %s\n", config->badFilesDir);
  dumpString(f, "Magic               %s\n", config->magic);
  for (i = 0; i < config->publicCount; i++)
    {
      fprintf(f, "Public              %s\n", config->publicDir[i]);
    }
  dumpString(f, "FileAreaBaseDir     %s\n", config->fileAreaBaseDir);
  dumpString(f, "PassFileAreaDir     %s\n", config->passFileAreaDir);
  dumpString(f, "BusyFileDir         %s\n", config->busyFileDir);
  
  dumpString(f, "AreaFixHelp         %s\n", config->areafixhelp);
  dumpString(f, "FileFixHelp         %s\n", config->filefixhelp);
  dumpString(f, "Intab               %s\n", config->intab);
  dumpString(f, "Outtab              %s\n", config->outtab);
  dumpString(f, "EchotossLog         %s\n", config->echotosslog);
  dumpString(f, "StatLog             %s\n", config->statlog);
  dumpString(f, "ImportLog           %s\n", config->importlog);

  switch (config->LinkWithImportlog)
  {
  case lwiYes:
    fprintf(f, "LinkWithImportlog   Yes\n");
    break;

  case lwiNo:
    fprintf(f, "LinkWithImportlog   No\n");
    break;

  case lwiKill:
    fprintf(f, "LinkWithImportlog   Kill\n");
    break;

  default:
    printf("Internal error: Unknown value #%d for LinkWithImportLog!\n", config->LinkWithImportlog);
  }

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
  
  dumpString(f, "FileAreasLog        %s\n", config->fileAreasLog);
  dumpString(f, "FileNewAreasLog     %s\n", config->fileNewAreasLog);
  dumpString(f, "Lockfile            %s\n", config->lockfile);
  dumpString(f, "LongNameList        %s\n", config->longNameList);
  dumpString(f, "fileArcList         %s\n", config->fileArcList);
  dumpString(f, "filePassList        %s\n", config->filePassList);
  dumpString(f, "fileDupeList        %s\n", config->fileDupeList);
  dumpString(f, "MsgIDFile           %s\n", config->msgidfile);
  
  fprintf(f, "\n");
}

int nstricmp(char *s1, char *s2)
{
    if (s1 == NULL && s2 != NULL)
    {
        return -1;
    }
    if (s1 != NULL && s2 == NULL)
    {
        return 1;
    }
    if (s1 == NULL && s2 == NULL)
    {
        return 0;
    }
    return (stricmp(s1, s2));
}
        

void dumpLinks(s_fidoconfig *config, FILE *f)
{
  int i, j;
  s_link link;

  for (i = 0; i < config->linkCount; i++)
    {
      link = config->links[i];
      dumpString(f, "Link                %s\n", link.name);
      fprintf(f, "Aka                 %s\n", aka2str(link.hisAka));
      fprintf(f, "ourAka              %s\n", aka2str(*link.ourAka));
      if (link.email)
      {
	fprintf(f, "Email               %s\n", link.email);
	switch (link.emailEncoding)
	{
	case eeMIME:
	  fprintf(f, "EmailEncoding       MIME\n");
	  break;

	case eeSEAT:
	  fprintf(f, "EmailEncoding       SEAT\n");
	  break;

	case eeUUE:
	  fprintf(f, "EmailEncoding       UUE\n");
	  break;

	default:
	  printf("Internal error: Unknown encoding #%d for link %s!\n",
		 link.emailEncoding, link.name);
	}
      }
      if (link.emailFrom)
	fprintf(f, "EmailFrom           %s\n", link.emailFrom);
      if (link.emailSubj)
	fprintf(f, "EmailSubj           %s\n", link.emailSubj);
      dumpString(f, "Password            %s\n", link.defaultPwd);
      if (nstricmp(link.defaultPwd, link.pktPwd)!=0)
	fprintf(f, "pktPwd              %s\n", link.pktPwd);
      if (nstricmp(link.defaultPwd, link.ticPwd)!=0)
	fprintf(f, "ticPwd              %s\n", link.ticPwd);
      if (nstricmp(link.defaultPwd, link.areaFixPwd)!=0)
	fprintf(f, "areaFixPwd          %s\n", link.areaFixPwd);
      if (nstricmp(link.defaultPwd, link.fileFixPwd)!=0)
	fprintf(f, "fileFixPwd          %s\n", link.fileFixPwd);
      if (nstricmp(link.bbsPwd, link.defaultPwd)!=0)
	fprintf(f, "bbsPwd              %s\n", link.bbsPwd);
      if (nstricmp(link.sessionPwd, link.defaultPwd)!=0)
	fprintf(f, "sessionPwd              %s\n", link.sessionPwd);
      dumpString(f, "handle              %s\n", link.handle);
      if (link.autoAreaCreate != 0)
	fprintf(f, "autoAreaCreate      on\n");
      if (link.autoFileCreate != 0)
	fprintf(f, "autoFileCreate      on\n");
      if (link.AreaFix != 1)
	fprintf(f, "areafix             off\n");
	  if (link.afixEchoLimit)
		  fprintf(f, "AreaFixEchoLimit %u\n", link.afixEchoLimit);
      if (link.FileFix != 1)
	fprintf(f, "fileFix             off\n");
      if (link.forwardRequests != 0)
	fprintf(f, "forwardRequests     on\n");
	  if (link.forwardAreaPriority)
	printf("forwardAreaPriority %u\n", link.forwardAreaPriority);
      if (link.fReqFromUpLink != 0)
	fprintf(f, "fRequestFromUpLink  on\n");

      switch (link.forwardPkts) {
      case fSecure:
	fprintf(f, "forwardPkts         secure\n");
	break;
      case fOn:
	fprintf(f, "forwardPkts         on\n");
	break;
      default:;
      }
      switch (link.allowEmptyPktPwd) {
      case eSecure:
	fprintf(f, "allowEmptyPktPwd    secure\n");
	break;
      case eOn:
	fprintf(f, "allowEmptyPktPwd    on\n");
	break;
      default:;
      }
      switch (link.allowPktAddrDiffer) {
      case pdOn:
	fprintf (f, "allowPktAddrDiffer on\n");
	break;
      case pdOff:
	break;
      default:
	fprintf (stderr, "Error in keyword allowPktAddrDiffer\n");
      }
      
      if (link.packerDef != NULL)
	fprintf(f, "packer              %s\n", link.packerDef->packer);
      
      switch (link.echoMailFlavour) {
      case hold:
	fprintf(f, "echoMailFlavour     hold\n");
	break;
      case crash:
	fprintf(f, "echoMailFlavour     crash\n");
	break;
      case direct:
	fprintf(f, "echoMailFlavour     direct\n");
	break;
      case immediate:
	fprintf(f, "echoMailFlavour     immediate\n");
	break;
      default:;
      }

      switch (link.fileEchoFlavour) {
      case hold:
	fprintf(f, "fileEchoFlavour     hold\n");
	break;
      case crash:
	fprintf(f, "fileEchoFlavour     crash\n");
	break;
      case direct:
	fprintf(f, "fileEchoFlavour     direct\n");
	break;
      case immediate:
	fprintf(f, "fileEchoFlavour     immediate\n");
	break;
      default:;
      }

      dumpString(f, "linkGrp             %s\n", link.LinkGrp);

      if (link.numAccessGrp > 0) {
		  fprintf(f, "accessGrp ");
		  for (j = 0; j < link.numAccessGrp; j++) {
			  if (j > 0) fprintf(f, ", ");
			  fprintf(f, "%s", link.AccessGrp[i]);
		  }
		  fprintf(f, "\n");
	  }
	  if (link.numOptGrp > 0) {
		  printf("OptGrp ");
		  for (i = 0; i < link.numOptGrp; i++) {
			  if (i > 0) printf(", ");
			  printf("%s", link.optGrp[i]);
		  }
		  printf("\n");
	  }
	  if (link.numFrMask > 0) {
		  printf("ForwardRequestMask ");
		  for (i = 0; i < link.numFrMask; i++) {
			  if (i > 0) printf(", ");
			  printf("%s", link.frMask[i]);
		  }
		  printf("\n");
	  }
	  if (link.numDfMask > 0) {
		  printf("DenyFwdMask ");
		  for (i = 0; i < link.numDfMask; i++) {
			  if (i > 0) printf(", ");
			  printf("%s", link.dfMask[i]);
		  }
		  printf("\n");
	  }

	  dumpString(f, "autoAreaCreateFile  %s\n", link.autoAreaCreateFile);
	  dumpString(f, "autoFileCreateFile  %s\n", link.autoFileCreateFile);
	  dumpString(f, "autoAreaCreateDefaults %s\n", link.autoAreaCreateDefaults);
	  dumpString(f, "autoFileCreateDefaults %s\n", link.autoFileCreateDefaults);
	  dumpString(f, "forwardRequestFile  %s\n", link.forwardRequestFile);
	  dumpString(f, "denyFwdFile  %s\n", link.denyFwdFile);
	  dumpString(f, "RemoteRobotName     %s\n", link.RemoteRobotName);
      if (link.Pause != 0)
	fprintf(f, "pause\n");
      if (link.autoPause != 0)
	fprintf(f, "autoPause            %u\n", link.autoPause);
      if (link.advancedAreafix != 0)
	fprintf(f, "advancedAreafix\n");
      if (link.level != 0)
	fprintf(f, "level                %u\n", link.level);
      if (link.arcmailSize != 0)
	fprintf(f, "arcmailSize          %u\n", link.arcmailSize);
      if (link.pktSize != 0)
	fprintf(f, "pktSize              %u\n", link.pktSize);
      if (link.packNetmail != 0)
      {
	fprintf(f, "packNetmail          On\n"
	           "maxUnpackedNetmail   %d\n", link.maxUnpackedNetmail);
      }
      else fprintf(f, "packNetmail          Off\n");

      if (link.export) fprintf(f, "export              on\n");
      else fprintf(f, "export              off\n");

      if (link.import) fprintf(f, "import              on\n");
      else fprintf(f, "import              off\n");

      if (link.mandatory) fprintf(f, "mandatory          on\n");
      else fprintf(f, "mandatory           off\n");

      switch (link.linkBundleNameStyle) {
      case eUndef:
	      // Can't print undeclared value
	      break;
      case eAddrDiff:
	      fprintf(f,"linkBundleNameStyle addrDiff\n");
	      break;
      case eTimeStamp:
	      fprintf(f,"linkBundleNameStyle timeStamp\n");
	      break;
      case eAddrDiffAlways:
	      fprintf(f,"linkBundleNameStyle addrDiffAlways\n");
	      break;
      default:
	      printf("Warning: linkBundleNameStyle is UNKNOWN! Update dump please!\n");
	      break;

      }		  
  
	  if (link.msgBaseDir) fprintf(f,"msgBaseDir %s\n",link.msgBaseDir);

      fprintf(f, "\n");
    }
}

void dumpRoute(s_route *route, int count, char *prefix, FILE *f)
{
  int i;
  for (i = 0; i < count; i++)
    {
      fprintf(f, "%s ", prefix);

      switch (route[i].flavour) {
      case normal:
	fprintf(f, "normal    ");
	break;
      case hold:
	fprintf(f, "hold      ");
	break;
      case crash:
	fprintf(f, "crash     ");
	break;
      case direct:
	fprintf(f, "direct    ");
	break;
      case immediate:
	fprintf(f, "immediate ");
	break;
      }
      
      if (route[i].target != NULL) fprintf(f, "%-19s ", aka2str(route[i].target->hisAka));
      else
      {
	  switch(route[i].routeVia)
	  {
	  case route_zero:
	      fprintf(f, "zero                ");
              break;
	  case host:
	      fprintf(f, "host                ");
              break;
	  case hub:
	      fprintf(f, "hub                 ");
	      break;
	  case boss:
	      fprintf(f, "boss                ");
	      break;
	  case noroute:
	      fprintf(f, "noroute             ");
	      break;
	  case nopack:
	      fprintf(f, "nopack              ");
	      break;
	  case route_extern:
	      /* internal only */
	      break;
	  }
      }

      fprintf(f, "%s\n", route[i].pattern);
    }
  
  fprintf(f, "\n");
}

void dumpMsgArea(s_area *area, char *prefix, FILE *f)
{
    int i;

    fprintf(f, "%-19s %-39s ", prefix, area->areaName);

    if (area->fileName != NULL) fprintf(f, "%-79s ", area->fileName);

    switch (area->msgbType)
    {
    case MSGTYPE_SDM:
	fprintf(f, "-b Msg              ");
	break;
	
    case MSGTYPE_SQUISH:
	fprintf(f, "-b Squish           ");
	break;

    case MSGTYPE_JAM:
	fprintf(f, "-b Jam              ");
	break;

    case MSGTYPE_PASSTHROUGH:
	fprintf(f, "Passthrough         ");
	break;
    }

    if (area->nopack) fprintf(f, "-nopack ");
    if (area->purge != 0) fprintf(f, "-p %d ", area->purge);
    if (area->max != 0) fprintf(f, "-$m %d ", area->max);
    if (area->dupeHistory != 0) fprintf(f, "-dupeHistory %d ", area->dupeHistory);

    switch (area->dupeCheck)
    {
    case dcOff:
	fprintf(f, "-dupecheck off ");
	break;
    case dcMove:
	fprintf(f, "-dupecheck move ");
	break;
    case dcDel:
	fprintf(f, "-dupecheck del ");
	break;
    }

    if (area->tinySB != 0) fprintf(f, "-tinySB ");
//    if (area->manual != 0) fprintf(f, "-manual ");
    if (area->keepUnread != 0) fprintf(f, "-keepUnread ");
    if (area->killRead != 0) fprintf(f, "-killRead ");
    if (area->hide != 0) fprintf(f, "-h ");
    if (area->noPause != 0) fprintf(f, "-noPause ");
    if (area->mandatory != 0) fprintf(f, "-mandatory ");
    if (area->DOSFile != 0) fprintf(f, "-DOSFile ");
	if (area->debug) fprintf(f,"-debug ");

    if (area->levelread != 0) fprintf(f, "-lr %d ", area->levelread);
    if (area->levelwrite != 0) fprintf(f, "-lw %d ", area->levelwrite);

    if (area->group != NULL) fprintf(f, "-g %s ", area->group);

    /*if (area->rgrp != NULL) fprintf(f, "-r %s ", area->rgrp);
    if (area->wgrp != NULL) fprintf(f, "-w %s ", area->wgrp);
    if (area->rwgrp != NULL) fprintf(f, "-l %s ", area->rwgrp);*/

    if (area->ccoff != 0) fprintf(f, "-ccoff ");
    
    if (area->useAka != NULL) fprintf(f, "-a %s ", aka2str(*area->useAka));

    if (area->description != NULL) fprintf(f, "-d \"%s\" ", area->description);

    for (i = 0; i < area->downlinkCount; i++)
    {
	fprintf(f, "%s ", aka2str(area->downlinks[i]->link->hisAka));
	if (area->downlinks[i]->export == 0) fprintf(f, "-w ");
	if (area->downlinks[i]->import == 0) fprintf(f, "-r ");
	if (area->downlinks[i]->mandatory == 1) fprintf(f, "-mn ");
	if (area->downlinks[i]->defLink == 1) fprintf(f, "-def ");
    }

    fprintf(f, "\n");
}

void dumpMsgAreas(s_fidoconfig *config, FILE *f)
{
  int i;
  
  for (i = 0; i < config->netMailAreaCount; i++)
  {
    dumpMsgArea(&config->netMailAreas[i], "netMailArea        ", f);
  }
  dumpMsgArea(&config->dupeArea, "dupeArea       ", f);
  dumpMsgArea(&config->badArea, "badArea            ", f);

  fprintf(f, "\n");

  for (i = 0; i < config->localAreaCount; i++)
  {
    dumpMsgArea(&config->localAreas[i], "localArea          ", f);
  }
    
  fprintf(f, "\n");

  for (i = 0; i < config->echoAreaCount; i++)
  {
    dumpMsgArea(&config->echoAreas[i], "echoArea           ", f);
  }

  fprintf(f, "\n");
}

void dumpBbsArea(s_bbsarea *area, FILE *f)
{
  fprintf(f, "bbsArea            %-39s %s", area->areaName, area->pathName);
  if (area->description)
    fprintf(f, " -d \"%s\"", area->description);
  fprintf(f, "\n");
}

void dumpBbsAreas(s_fidoconfig *config, FILE *f)
{
  int i;
  
  for (i = 0; i < config->bbsAreaCount; i++)
  {
    dumpBbsArea(&config->bbsAreas[i], f);
  }

  fprintf(f, "\n");
}

void dumpFileArea(s_filearea *area, FILE *f)
{
  int i;
  s_arealink *d;

  fprintf(f, "fileArea            %-39s", area->areaName);

  if (area->pass)
    fprintf(f, "Passthrough");
  else

    fprintf(f, " %s", area->pathName);
  if (area->description)
    fprintf(f, " -d \"%s\"", area->description);
  if (area->sendorig)
    fprintf(f, " -sendOrig");
  if (area->noCRC)
    fprintf(f, " -noCRC");
  if (area->noreplace)
    fprintf(f, " -noReplace");
  if (area->mandatory)
    fprintf(f, " -manual");
  if (area->hide)
    fprintf(f, " -h");
  if (area->noPause)
    fprintf(f, " -noPause");

  if (area->useAka)
    fprintf(f, " -a %s", aka2str(*area->useAka));

  if (area->group)
    fprintf(f, " -g %s", area->group);

  if (area->levelread)
    fprintf(f, " -lr %u", area->levelread);
  if (area->levelwrite)
    fprintf(f, " -lr %u", area->levelwrite);

  for (i = 0; i < area->downlinkCount; i++)
  {
    d = area->downlinks[i];
    fprintf(f, " %s", aka2str(d->link->hisAka));
    if (!d->import)
      fprintf(f, " -r");
    if (!d->export)
      fprintf(f, " -w");
    if (d->mandatory)
      fprintf(f, " -mn");
    if (d->defLink)
      fprintf(f, " -def");
  }

  fprintf(f, "\n");
}

void dumpFileAreas(s_fidoconfig *config, FILE *f)
{
  int i;
  
  for (i = 0; i < config->fileAreaCount; i++)
  {
    dumpFileArea(&config->fileAreas[i], f);
  }

  fprintf(f, "\n");
}

void dumpCarbon(s_carbon *carbon, FILE *f)
{
    switch (carbon->ctype)
    {
    case ct_to:
	fprintf(f, "CarbonTo            ");
	break;
    case ct_from:
	fprintf(f, "CarbonFrom          ");
	break;
    case ct_kludge:
	fprintf(f, "CarbonKludge        ");
	break;
    case ct_subject:
	fprintf(f, "CarbonSubj          ");
	break;
    case ct_msgtext:
	fprintf(f, "CarbonText          ");
	break;
    case ct_addr:
		fprintf(f, "CarbonAddr          ");
		break;
    }
    
    if (carbon->ctype != ct_addr) fprintf(f, "%s\n", carbon->str);
	else printf("%u/%u\n", carbon->addr.net, carbon->addr.node);

    if (carbon -> extspawn) {
	    dumpString(f, "CarbonExtern        %s\n", carbon->areaName);
    } else if (carbon->move) {
	    dumpString(f, "CarbonMove          %s\n", carbon->area->areaName);
    } else {
	    dumpString(f, "CarbonCopy          %s\n", carbon->area->areaName);
    };
}

void dumpCarbons(s_fidoconfig *config, FILE *f)
{
    int i;

    for (i = 0; i < config->carbonCount; i++)
    {
	dumpCarbon(&config->carbons[i], f);
    }

    if (config->carbonAndQuit != 0) fprintf(f, "carbonAndQuit       On\n");

    fprintf(f, "\n");
}

void dumpAreafix(s_fidoconfig *config, FILE *f)
{
    unsigned int i;

    if (config->areafixFromPkt != 0) fprintf(f, "areafixFromPkt\n");
    if (config->areafixKillReports != 0) fprintf(f, "areafixKillReports\n");
    if (config->areafixKillRequests != 0) fprintf(f, "areafixKillRequests\n");
    if (config->areafixMsgSize != 0) fprintf(f, "areafixMsgSize %i\n", config->areafixMsgSize);

    dumpString(f, "ReportTo            %s\n", config->ReportTo);
	dumpString(f, "robotsArea          %s\n", config->robotsArea);
	dumpString(f, "areafixSplitStr     %s\n", config->areafixSplitStr);
	dumpString(f, "areafixOrigin       %s\n", config->areafixOrigin);

    if (config->numPublicGroup > 0)
		{
			fprintf(f, "PublicGroup         ");
			for (i = 0; i < config->numPublicGroup; i++)
				{
					if (i > 0) fprintf(f, ", %s", config->PublicGroup[i]);
					else fprintf(f, "%s", config->PublicGroup[0]);
				}
		}

    fprintf(f, "\n");
}

void dumpFilefix(s_fidoconfig *config, FILE *f)
{
    if (config->filefixKillReports != 0) fprintf(f, "filefixKillReports\n");
    if (config->filefixKillRequests != 0) fprintf(f, "filefixKillRequests\n");

    fprintf(f, "\n");
}

void dumpTicker(s_fidoconfig *config, FILE *f)
{
    int i;

    if (config->fileSingleDescLine != 0) fprintf(f, "FileSingleDescLine\n");
    if (config->fileCheckDest != 0) fprintf(f, "FileCheckDest\n");
    fprintf(f, "FileDescPos         %u\n", config->fileDescPos);
    dumpString(f, "FileLDescString     %s\n", config->fileLDescString);
    if (config->addDLC != 0) fprintf(f, "AddDLC\n");
    fprintf(f, "DLCDigits           %u\n", config->DLCDigits);
    fprintf(f, "FileMaxDupeAge      %u\n", config->fileMaxDupeAge);
    fprintf(f, "FileFileUMask       %o\n", config->fileFileUMask);
    fprintf(f, "FileDirUMask        %o\n", config->fileDirUMask);
    fprintf(f, "ConvertLongNames    %s\n", (config->convertLongNames == cLower) ? "Lower" : ((config->convertLongNames == cUpper) ? "Upper" : "DontTouch"));
    fprintf(f, "ConvertShortNames   %s\n", (config->convertShortNames == cLower) ? "Lower" : ((config->convertShortNames == cUpper) ? "Upper" : "DontTouch"));

    dumpString(f, "FileLocalPwd        %s\n", config->fileLocalPwd);

    for (i = 0; i < config->execonfileCount; i++) {
       fprintf(f, "ExecOnFile %s %s %s\n",
                   config->execonfile[i].filearea,
                   config->execonfile[i].filename,
                   config->execonfile[i].command);
    }

    fprintf(f, "\n");
}

void dumpNodelists(s_fidoconfig *config, FILE *f)
{
  int i;

  if (config->fidoUserList != NULL)
    {
      fprintf(f, "FidoUserList        %s\n", config->fidoUserList);
      fprintf(f, "\n");
    }

  for (i = 0; i < config->nodelistCount; i++)
    {
      fprintf(f, "NodeList            %s\n",
              config->nodelists[i].nodelistName);
      if (config->nodelists[i].diffUpdateStem != NULL)
        fprintf(f, "DiffUpdate          %s\n",
                config->nodelists[i].diffUpdateStem);
      if (config->nodelists[i].fullUpdateStem != NULL)
        fprintf(f, "FullUpdate          %s\n",
                config->nodelists[i].fullUpdateStem);
      if (config->nodelists[i].defaultZone != 0)
        fprintf (f, "DefaultZone         %d\n",
                 config->nodelists[i].defaultZone);
      switch (config->nodelists[i].format)
        {
        case fts5000:
          fprintf (f, "NodelistFormat      Standard\n");
          break;
        case points24:
          fprintf (f, "NodelistFormat      Points24\n");
          break;
	case points4d:
	  fprintf (f, "NodelistFormat      Points4D\n");
        }
	fprintf(f, "\n");
    }
}

void dumpFilelist(s_filelist *fl, FILE *f)
{
  switch (fl->flType)
  {
  case flDir:
    fprintf(f, "filelist           dir %s %s %s %s\n",
	    fl->destFile,
	    fl->dirHdrTpl, fl->dirEntryTpl, fl->dirFtrTpl);
    break;

  case flGlobal:
    fprintf(f, "filelist           global %s %s %s %s %s %s\n",
	    fl->destFile,
	    fl->dirHdrTpl, fl->dirEntryTpl, fl->dirFtrTpl,
	    fl->globHdrTpl, fl->globFtrTpl);
    break;

  case flDirList:
    fprintf(f, "filelist           dirlist %s %s %s %s\n",
	    fl->destFile,
	    fl->dirListHdrTpl, fl->dirListEntryTpl, fl->dirListFtrTpl);
    break;
  }
}

void dumpFilelists(s_fidoconfig *config, FILE *f)
{
  unsigned int i;

  for (i = 0; i < config->filelistCount; i++)
  {
    dumpFilelist(&(config->filelists[i]), f);
  }
}

void dumpConfig(s_fidoconfig *config, FILE *f)
{
  dumpHeader(config, f);
  dumpAddrs(config, f);
  dumpPack(config, f);
  dumpPaths(config, f);
  dumpLinks(config, f);
  dumpRoute(config->route, config->routeCount, "route              ", f);
  dumpRoute(config->routeMail, config->routeMailCount, "routeMail          ", f);
  dumpRoute(config->routeFile, config->routeFileCount, "routeFile          ", f);
  dumpMsgAreas(config, f);
  dumpFileAreas(config, f);
  dumpBbsAreas(config, f);
  dumpCarbons(config, f);
  dumpAreafix(config, f);
  dumpFilefix(config, f);
  dumpTicker(config, f);
  dumpFilelists(config, f);
  dumpNodelists(config, f);
}

int dumpConfigToFile(s_fidoconfig *config, char *fileName)
{
  FILE *f;
  char rc = 1;

  f = fopen(fileName, "w");
  if (f != NULL)
    {
      rc = 0;      
      dumpConfig(config, f);
    }
  return rc;
}
