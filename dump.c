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
  if (config->processPkt != NULL) fprintf(f, "processPkt %s\n", config->processPkt);
  if (config->tossingExt != NULL) fprintf(f, "tossingExt %s\n", config->tossingExt);
  
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
  
  dumpString(f, "AreaFixHelp         %s\n", config->areafixhelp);
  dumpString(f, "FileFixHelp         %s\n", config->filefixhelp);
  dumpString(f, "Intab               %s\n", config->intab);
  dumpString(f, "Outtab              %s\n", config->outtab);
  dumpString(f, "EchotossLog         %s\n", config->echotosslog);
  dumpString(f, "ImportLog           %s\n", config->importlog);
  dumpString(f, "LinkWithImportlog   %s\n", config->LinkWithImportlog);
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
      dumpString(f, "Password            %s\n", link.defaultPwd);
      if (stricmp(link.defaultPwd, link.pktPwd)!=0)
	fprintf(f, "pktPwd              %s\n", link.pktPwd);
      if (stricmp(link.defaultPwd, link.ticPwd)!=0)
	fprintf(f, "ticPwd              %s\n", link.ticPwd);
      if (stricmp(link.defaultPwd, link.areaFixPwd)!=0)
	fprintf(f, "areaFixPwd          %s\n", link.areaFixPwd);
      if (stricmp(link.defaultPwd, link.fileFixPwd)!=0)
	fprintf(f, "fileFixPwd          %s\n", link.fileFixPwd);
      if (stricmp(link.bbsPwd, link.defaultPwd)!=0)
	fprintf(f, "bbsPwd              %s\n", link.bbsPwd);
      if (stricmp(link.sessionPwd, link.defaultPwd)!=0)
	fprintf(f, "sessionPwd              %s\n", link.sessionPwd);
      dumpString(f, "handle              %s\n", link.handle);
      if (link.autoAreaCreate != 0)
	fprintf(f, "autoAreaCreate      on\n");
      if (link.autoFileCreate != 0)
	fprintf(f, "autoFileCreate      on\n");
      if (link.AreaFix != 1)
	fprintf(f, "areafix             off\n");
      if (link.FileFix != 1)
	fprintf(f, "fileFix             off\n");
      if (link.forwardRequests != 0)
	fprintf(f, "forwardRequests     on\n");
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
      if (link.numAccessGrp > 0)
      {
	fprintf(f, "accessGrp           ");
	for (j = 0; j < link.numAccessGrp; j++)
	{
	  if (j > 0) fprintf(f, ", %s", link.AccessGrp[j]);
	  else fprintf(f, "%s", link.AccessGrp[0]);
	}
	fprintf(f, "\n");
      }
      dumpString(f, "autoAreaCreateFile  %s\n", link.autoAreaCreateFile);
      dumpString(f, "autoFileCreateFile  %s\n", link.autoFileCreateFile);
      dumpString(f, "autoAreaCreateDefaults %s\n", link.autoAreaCreateDefaults);
      dumpString(f, "autoFileCreateDefaults %s\n", link.autoFileCreateDefaults);
      dumpString(f, "forwardRequestFile  %s\n", link.forwardRequestFile);
      dumpString(f, "RemoteRobotName     %s\n", link.RemoteRobotName);
      if (link.Pause != 0)
	fprintf(f, "pause\n");
      if (link.autoPause != 0)
	fprintf(f, "autoPause            %u\n", link.autoPause);
      if (link.level != 0)
	fprintf(f, "level                %u\n", link.level);
      if (link.arcmailSize != 0)
	fprintf(f, "arcmailSize          %u\n", link.arcmailSize);
      if (link.pktSize != 0)
	fprintf(f, "pktSize          %u\n", link.pktSize);

	  if (link.export) fprintf(f, "export              on\n");
	  else fprintf(f, "export              off\n");
	  
	  if (link.import) fprintf(f, "import              on\n");
	  else fprintf(f, "import              off\n");

      if (link.mandatory) fprintf(f, "mandatory          on\n");
	  else fprintf(f, "mandatory           off\n");
	  
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
      
      if (route[i].enc != 0)
	  fprintf(f, "enc       ");
      else fprintf(f, "noenc     ");

      if (route[i].target != NULL) fprintf(f, "%-19s ", aka2str(route[i].target->hisAka));
      else
      {
	  switch(route[i].routeVia)
	  {
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

    case MSGTYPE_PASSTHROUGH:
	fprintf(f, "Passthrough         ");
	break;
    }

    if (area->nopack) fprintf(f, "-nopack ");
    if (area->purge != 0) fprintf(f, "-p %d ", area->purge);
    if (area->max != 0) fprintf(f, "-$m %d ", area->max);
    if (area->dupeSize != 0) fprintf(f, "-dupeSize %d ", area->dupeSize);
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
    if (area->hide != 0) fprintf(f, "-hide ");
    if (area->noPause != 0) fprintf(f, "-noPause ");
    if (area->mandatory != 0) fprintf(f, "-mandatory ");
    if (area->DOSFile != 0) fprintf(f, "-DOSFile ");

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
    }
    
    fprintf(f, "%s\n", carbon->str);

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

    dumpString(f, "ReportTo            %s\n", config->ReportTo);

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
    dumpString(f, "FileLocalPwd        %s\n", config->fileLocalPwd);

    for (i = 0; i < config->execonfileCount; i++) {
       fprintf(f, "ExecOnFile: Area %s File %s Call %s\n",
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
        fprintf (f, "DefaultZone          %d\n",
                 config->nodelists[i].defaultZone);
      switch (config->nodelists[i].format)
        {
        case fts5000:
          fprintf (f, "NodelistFormat      Standard\n");
          break;
        case points24:
          fprintf (f, "NodelistFormat      Points24\n");
          break;
        }
      fprintf(f, "\n");
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
  dumpCarbons(config, f);
  dumpAreafix(config, f);
  dumpFilefix(config, f);
  dumpTicker(config, f);
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
