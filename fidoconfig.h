#ifndef FIDOCONFIG_H
#define FIDOCONFIG_H
#include <msgapi.h>

#include "common.h"

#define MSGTYPE_PASSTHROUGH 0x04

struct link {
   s_addr hisAka, *ourAka;
   char *name;
   char *defaultPwd,               // 8 byte passwords
        *pktPwd,
        *ticPwd,
        *areaFixPwd,
        *fileFixPwd,
        *bbsPwd;
   char *handle;
   char *pktFile;             // used only internally by hpt
};

typedef struct link s_link;

enum flavour {hold, normal, crash, direct, immediate};
typedef enum flavour e_flavour;

enum routing {host = 1, hub, boss, noroute};
typedef enum routing e_routing;

struct route {
   e_flavour flavour;
   char      enc;
   s_link    *target;   // if target = NULL use
   e_routing routeVia;  // this
   char      *pattern;
};

typedef struct route s_route;

enum dupeCheck {off, move, del};

typedef enum dupeCheck e_dupeCheck;

struct area {
   char *areaName;
   char *fileName;
   
   int msgbType;        // MSGTYPE_SDM or MSGTYPE_SQUISH or MSGTYPE_PASSTHROUGH
   s_addr *useAka;
   
   s_link **downlinks;  // array of pointers to s_link
   UINT downlinkCount;

   UINT purge, max, dupeHistory;
   e_dupeCheck dupeCheck;
   char noDC, tinySB, manual, hide, noPause;

   void *dupes;        // use internally for hpt pointer to dupeDataBase
};

typedef struct area s_area;

struct fidoconfig {
   UINT    cfgVersionMajor, cfgVersionMinor;
   char    *name, *location, *sysop;

   UINT    addrCount;
   s_addr  *addr;

   UINT    publicCount;
   char    **public;

   UINT    linkCount;
   s_link  *links;

   char    *inbound, *outbound, *protInbound, *listInbound, *localInbound;
   char    *logFileDir, *dupeHistoryDir, *nodelistDir;
   char    *magic;

   s_area  netMailArea, dupeArea, badArea;
   UINT    echoAreaCount;
   s_area  *echoAreas;

   UINT    routeCount;
   s_route *route;
   UINT    routeFileCount;
   s_route *routeFile;
   UINT    routeMailCount;
   s_route *routeMail;
};

typedef struct fidoconfig s_fidoconfig;

int parseLine(char *line, s_fidoconfig *config);

s_fidoconfig *readConfig();

void disposeConfig(s_fidoconfig *config);

s_link *getLink(s_fidoconfig config, char *addr);
s_link *getLinkFromAddr(s_fidoconfig, s_addr aka);
s_addr *getAddr(s_fidoconfig config, char *addr);
int    existAddr(s_fidoconfig config, s_addr aka);
s_area *getArea(s_fidoconfig config, char *areaName);

#endif
