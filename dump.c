#include <string.h>

#include "fidoconfig.h"
#include "common.h"

void dumpHeader(s_fidoconfig *config, FILE *f)
{
  int i;

  fprintf(f, "# This config was created from a memory config by fidoconfig dumpConfigToFile()!\n\n"); 

  fprintf(f, "Version %u.%02u\n", config->cfgVersionMajor, config->cfgVersionMinor);
  fprintf(f, "Name %s\n", config->name);
  fprintf(f, "Sysop %s\n", config->sysop);
  fprintf(f, "Location %s\n\n", config->location);

  fprintf(f, "LogLevels %s\n\n", config->loglevels);

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

  fprintf(f, "Inbound %s\n", config->inbound);
  fprintf(f, "ListInbound %s\n", config->listInbound);
  fprintf(f, "ProtInbound %s\n", config->protInbound);
  fprintf(f, "TempInbound %s\n", config->tempInbound);
  fprintf(f, "LocalInbound %s\n", config->localInbound); 

  fprintf(f, "Outbound %s\n", config->outbound);
  fprintf(f, "TempOutbound %s\n", config->tempOutbound);
  
  fprintf(f, "LogFileDir %s\n", config->logFileDir);
  fprintf(f, "DupeHistoryDir %s\n", config->dupeHistoryDir);
  fprintf(f, "MsgBaseDir %s\n", config->msgBaseDir);
  fprintf(f, "NodelistDir %s\n", config->nodelistDir);
  fprintf(f, "Magic %s\n", config->magic);
  for (i = 0; i < config->publicCount; i++)
    {
      fprintf(f, "Public %s\n", config->publicDir[i]);
    }
  fprintf(f, "FileAreaBaseDir %s\n", config->fileAreaBaseDir);
  
  fprintf(f, "AreaFixHelp %s\n", config->areafixhelp);
  fprintf(f, "FileFixHelp %s\n", config->filefixhelp);
  fprintf(f, "Intab %s\n", config->intab);
  fprintf(f, "Outtab %s\n", config->outtab);
  fprintf(f, "EchotossLog %s\n", config->echotosslog);
  fprintf(f, "ImportLog %s\n", config->importlog);
  fprintf(f, "LinkWithImportlog %s\n", config->LinkWithImportlog);
  fprintf(f, "Lockfile %s\n", config->lockfile);
}

void dumpLinks(s_fidoconfig *config, FILE *f)
{
  int i;
  s_link link;

  for (i = 0; i < config->linkCount; i++)
    {
      link = config->links[i];

      fprintf(f, "Link %s\n", link.name);
      fprintf(f, "Aka %s\n", aka2str(link.hisAka));
      fprintf(f, "ourAka %s\n", aka2str(*link.ourAka));
      fprintf(f, "Password %s\n", link.defaultPwd);
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
      if (link.handle != NULL)
	fprintf(f, "handle %s\n", link.handle);
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
    }
}

void dumpConfig(s_fidoconfig *config, FILE *f)
{
  dumpHeader(config, f);
  dumpPack(config, f);
  dumpPaths(config, f);
  dumpLinks(config, f);
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
