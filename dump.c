#include <string.h>

#include "fidoconfig.h"
#include "common.h"

void dumpString(FILE *f, char *text, char *str)
{
  if (str != NULL)
    fprintf(f, text, str);
}

void dumpHeader(s_fidoconfig *config, FILE *f)
{
  int i;

  fprintf(f, "# This config was created from a memory config by fidoconfig dumpConfigToFile()!\n\n"); 

  fprintf(f, "Version %u.%02u\n", config->cfgVersionMajor, config->cfgVersionMinor);
  dumpString(f, "Name %s\n", config->name);
  dumpString(f, "Sysop %s\n", config->sysop);
  dumpString(f, "Location %s\n\n", config->location);
  
  dumpString(f, "LogLevels %s\n\n", config->loglevels);

  for (i = 0; i < config->addrCount; i++)
    fprintf(f, "Address %s\n", aka2str(config->addr[i]));
  fprintf(f, "\n");
}

void dumpPack(s_fidoconfig *config, FILE *f)
{
  int max, i, j;

  if (config->packCount > config->unpackCount) 
    max = config->packCount;
  else
    max = config->unpackCount;

  for (i = 0; i < max; i++)
    {
      if (i < config->packCount)
	fprintf(f, "Pack %s  %s\n", config->pack[i].packer, config->pack[i].call);
      if (i < config->unpackCount)
	{
	  fprintf(f, "Unpack \"%s\" %u", config->unpack[i].call, config->unpack[i].offset);
	  for (j = 0; j < config->unpack[i].codeSize; j++)
	    fprintf(f, "%02x", (int) config->unpack[i].matchCode[j]);
	  printf(" ");
	  for (j = 0; j < config->unpack[i].codeSize; j++)
	    fprintf(f, "%02x", (int) config->unpack[i].mask[j]);
	  fprintf(f, "\n");
	}
    }
  fprintf(f, "\n");
}

void dumpPaths(s_fidoconfig *config, FILE *f)
{
  int i;

  dumpString(f, "Inbound %s\n", config->inbound);
  dumpString(f, "ListInbound %s\n", config->listInbound);
  dumpString(f, "ProtInbound %s\n", config->protInbound);
  dumpString(f, "TempInbound %s\n", config->tempInbound);
  dumpString(f, "LocalInbound %s\n", config->localInbound); 

  dumpString(f, "Outbound %s\n", config->outbound);
  dumpString(f, "TempOutbound %s\n", config->tempOutbound);
  
  dumpString(f, "LogFileDir %s\n", config->logFileDir);
  dumpString(f, "DupeHistoryDir %s\n", config->dupeHistoryDir);
  dumpString(f, "MsgBaseDir %s\n", config->msgBaseDir);
  dumpString(f, "NodelistDir %s\n", config->nodelistDir);
  dumpString(f, "Magic %s\n", config->magic);
  for (i = 0; i < config->publicCount; i++)
    {
      fprintf(f, "Public %s\n", config->publicDir[i]);
    }
  dumpString(f, "FileAreaBaseDir %s\n", config->fileAreaBaseDir);
  
  dumpString(f, "AreaFixHelp %s\n", config->areafixhelp);
  dumpString(f, "FileFixHelp %s\n", config->filefixhelp);
  dumpString(f, "Intab %s\n", config->intab);
  dumpString(f, "Outtab %s\n", config->outtab);
  dumpString(f, "EchotossLog %s\n", config->echotosslog);
  dumpString(f, "ImportLog %s\n", config->importlog);
  dumpString(f, "LinkWithImportlog %s\n", config->LinkWithImportlog);
  dumpString(f, "Lockfile %s\n", config->lockfile);
}

void dumpLinks(s_fidoconfig *config, FILE *f)
{
  int i;
  s_link link;

  for (i = 0; i < config->linkCount; i++)
    {
      link = config->links[i];

      dumpString(f, "Link %s\n", link.name);
      fprintf(f, "Aka %s\n", aka2str(link.hisAka));
      fprintf(f, "ourAka %s\n", aka2str(*link.ourAka));
      dumpString(f, "Password %s\n", link.defaultPwd);
      if (stricmp(link.defaultPwd, link.pktPwd)!=0)
	fprintf(f, "pktPwd %s\n", link.pktPwd);
      if (stricmp(link.defaultPwd, link.ticPwd)!=0)
	fprintf(f, "ticPwd %s\n", link.ticPwd);
      if (stricmp(link.defaultPwd, link.areaFixPwd)!=0)
	fprintf(f, "areaFixPwd %s\n", link.areaFixPwd);
      if (stricmp(link.defaultPwd, link.fileFixPwd)!=0)
	fprintf(f, "fileFixPwd %s\n", link.fileFixPwd);
      if (stricmp(link.bbsPwd, link.defaultPwd)!=0)
	fprintf(f, "bbsPwd %s\n", link.bbsPwd);
      dumpString(f, "handle %s\n", link.handle);
      if (link.autoAreaCreate != 0)
	fprintf(f, "autoAreaCreate on\n");
      if (link.autoFileCreate != 0)
	fprintf(f, "autoFileCreate on\n");
      if (link.AreaFix != 1)
	fprintf(f, "areafix off\n");
      if (link.FileFix != 1)
	fprintf(f, "fileFix off\n");
      if (link.forwardRequests != 0)
	fprintf(f, "forwardRequests on\n");
      if (link.fReqFromUpLink != 0)
	fprintf(f, "fReqFromUpLink on\n");

      switch (link.forwardPkts) {
      case fSecure:
	fprintf(f, "forwardPkts secure\n");
	break;
      case fOn:
	fprintf(f, "forwardPkts on\n");
	break;
      default:;
      }
      
      if (link.packerDef != NULL)
	fprintf(f, "packer %s\n", link.packerDef->packer);
      
      switch (link.echoMailFlavour) {
      case hold:
	fprintf(f, "echoMailFlavour hold\n");
	break;
      case crash:
	fprintf(f, "echoMailFlavour crash\n");
	break;
      case direct:
	fprintf(f, "echoMailFlavour direct\n");
	break;
      case immediate:
	fprintf(f, "echoMailFlavour immediate\n");
	break;
      default:;
      }

      dumpString(f, "linkGrp %s\n", link.LinkGrp);
      dumpString(f, "accessGrp %s\n", link.AccessGrp);
      dumpString(f, "autoAreaCreateFile %s\n", link.autoAreaCreateFile);
      dumpString(f, "autoFileCreateFile %s\n", link.autoFileCreateFile);
      dumpString(f, "autoAreaCreateDefaults %s\n", link.autoAreaCreateDefaults);
      dumpString(f, "autoFileCreateDefaults %s\n", link.autoFileCreateDefaults);
      dumpString(f, "forwardRequestFile %s\n", link.forwardRequestFile);
      dumpString(f, "RemoteRobotName %s\n", link.RemoteRobotName);
      if (link.Pause != 0)
	fprintf(f, "pause %u\n", link.Pause);
      if (link.autoPause != 0)
	fprintf(f, "autoPause %u\n", link.autoPause);
      if (link.level != 0)
	fprintf(f, "level %u\n", link.level);
      if (link.arcmailSize != 0)
	fprintf(f, "arcmailSize %u\n", link.arcmailSize);
      
      // just add the export import etc. dump code here...

      fprintf(f, "\n");
    }
}

void dumpRoute(s_route *route, int count, char *prefix, FILE *f)
{
  int i;
  for (i = 0; i < count; i++)
    {
      fprintf(f, "%s ", prefix);
      if (route->enc != 0)
	fprintf(f, "enc ");

    }
}

void dumpConfig(s_fidoconfig *config, FILE *f)
{
  dumpHeader(config, f);
  dumpPack(config, f);
  dumpPaths(config, f);
  dumpLinks(config, f);
  dumpRoute(config->route, config->routeCount, "route", f);
  dumpRoute(config->routeMail, config->routeMailCount, "routeMail", f);
  dumpRoute(config->routeFile, config->routeFileCount, "routeFile", f);
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
