#ifndef FIDOCONFIG_H
#define FIDOCONFIG_H
#include <msgapi.h>

#include "common.h"

#define MSGTYPE_PASSTHROUGH 0x04

extern char *actualLine, *actualKeyword;
extern int  actualLineNr;
extern char wasError;

struct pack {
   char    *packer;
   char    *call;
};
typedef struct pack s_pack;

enum flavour {normal, hold, crash, direct, immediate};
typedef enum flavour e_flavour;

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
   int  autoAreaCreate;       // 0 if not allowed for autoareacreate
   int  AreaFix;              // 0 if not allowed for areafix
   char *pktFile,*packFile;   // used only internally by hpt
   char *floFile,*bsyFile;    // see up
   s_pack *packerDef;
   e_flavour echoMailFlavour;
   char *TossGrp, *DenyGrp;   //groups for areafix & echo access
};
typedef struct link s_link;

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
   char tinySB, manual, hide, noPause;

   void *dupes;        // used internally by hpt. pointer to dupeDataBase
   void *newDupes;     // dito
   char imported;      // dito

   char group;                      // used by reader (and areafix soon)
   char *rwgrp, *wgrp, *rgrp;       // use for -l -w -r echo parameters

   int ccoff;          // 1 if carbon copy is not allowed from this area
};
typedef struct area s_area;

enum carbonType {to, from, kludge};
typedef enum carbonType e_carbonType;

struct carbon {
   e_carbonType type;
   char         *str;     //string to compare
   s_area       *area;    // area to copy messages
};
typedef struct carbon s_carbon;

struct unpack {
   UINT    offset;
   char    *matchCode;
   char    *call;
};
typedef struct unpack s_unpack;

struct fidoconfig {
   UINT     cfgVersionMajor, cfgVersionMinor;
   char     *name, *location, *sysop;

   UINT     addrCount;
   s_addr   *addr;

   UINT     publicCount;
   char     **public;

   UINT     linkCount;
   s_link   *links;

   char     *inbound, *outbound, *protInbound, *listInbound, *localInbound;
   char     *logFileDir, *dupeHistoryDir, *nodelistDir, *msgBaseDir;
   char     *magic, *areafixhelp, *autoCreateDefaults, *tempOutbound;

   s_area   netMailArea, dupeArea, badArea;
   UINT     echoAreaCount;
   s_area   *echoAreas;
   UINT     localAreaCount;
   s_area   *localAreas;

   UINT     routeCount;
   s_route  *route;
   UINT     routeFileCount;
   s_route  *routeFile;
   UINT     routeMailCount;
   s_route  *routeMail;

   UINT     packCount;
   s_pack   *pack;
   s_pack   *packDefault;
   UINT     unpackCount;
   s_unpack *unpack;
   
   char     *intab, *outtab;
   char     *echotosslog, *importlog, *lockfile;

   UINT     carbonCount;
   s_carbon *carbons;
};
typedef struct fidoconfig s_fidoconfig;

s_fidoconfig *readConfig();

void disposeConfig(s_fidoconfig *config);

s_link *getLink(s_fidoconfig config, char *addr);
s_link *getLinkFromAddr(s_fidoconfig, s_addr aka);
s_addr *getAddr(s_fidoconfig config, char *addr);
int    existAddr(s_fidoconfig config, s_addr aka);
s_area *getArea(s_fidoconfig *config, char *areaName);

// the following functions are for internal use.
// Only use them if you really know what you do
char *readLine(FILE *F);
int parseLine(char *line, s_fidoconfig *config);
void parseConfig(FILE *f, s_fidoconfig *config);
char *getConfigFileName();
char *trimLine(char *line);

#endif
