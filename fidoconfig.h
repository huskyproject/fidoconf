#ifndef FIDOCONFIG_H
#define FIDOCONFIG_H
#include "common.h"

struct link {
   s_addr hisAka, ourAka;
   char pwd[9];               // 8 byte password
   char *pktFile;             // used only internally by tossers
};

typedef struct link s_link;

struct area {
   char *areaName;
   char *fileName;
   
   int msgbType;        // MSGTYPE_SDM or MSGTYPE_SQUISH
   s_addr useAka;
   
   s_link **downlinks;  // array of pointers to s_link
   UINT downlinkCount;

   UINT purge;
};

typedef struct area s_area;

struct fidoconfig {
   UINT   cfgVersionMajor, cfgVersionMinor;
   char   *name, *location, *sysop;
   
   UINT   addrCount;
   s_addr *addr;

   UINT   publicCount;
   char   **public;

   char   *inbound, *outbound, *protInbound, *listInbound, *localInbound;
   char   *logFileDir, *dupeHistoryDir, *nodelistDir;
   char   *magic;

   s_area netMailArea, dupeArea, badArea;
   UINT   echoAreaCount;
   s_area *echoAreas;
};

typedef struct fidoconfig s_fidoconfig;

int parseLine(char *line, s_fidoconfig *config);

s_fidoconfig *readConfig();

void disposeConfig(s_fidoconfig *config);

#endif