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
						    
#ifndef FIDOCONFIG_H
#define FIDOCONFIG_H
#include <stdio.h>
#include <msgapi.h>

// #include "common.h"

#define MSGTYPE_PASSTHROUGH 0x04

#ifdef UNIX
#define PATH_DELIM        '/'
#else
#define PATH_DELIM        '\\'
#endif

#define strend(str) ((str) + strlen(str) - 1)

extern char *actualLine, *actualKeyword;
extern int  actualLineNr;
extern char wasError;
char *striptwhite(char *str);

struct addr {

   unsigned int zone, net, node, point;
   char   *domain;

};

typedef struct addr s_addr;

struct pack {
   char    *packer;
   char    *call;
};
typedef struct pack s_pack;

enum flavour {normal, hold, crash, direct, immediate};
typedef enum flavour e_flavour;
enum forward {fOff, fSecure, fOn};
typedef enum forward e_forward;

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
   int  autoFileCreate;       // 0 if not allowed for autofilecreate
   int  AreaFix;              // 0 if not allowed for areafix
   int  FileFix;              // 0 if not allowed for filefix
   int  forwardRequests;      // 0 if not allowed forward requests
   int  fReqFromUpLink;	      // 0 - ignore added unknown area (no area in cfg)
   e_forward forwardPkts;     // defines, if pkts should be forwarded to this link
   char *pktFile,*packFile;   // used only internally by hpt   
   char *floFile,*bsyFile;    // dito
   s_pack *packerDef;
   e_flavour echoMailFlavour;
   char *LinkGrp;	      // link's group for autocreate areas
   char *AccessGrp;	      // groups for echo access
   char *autoAreaCreateFile;  // file where autocreated areas are written to
   char *autoFileCreateFile;
   char *autoAreaCreateDefaults;// add default string for autocreated area here
   char *autoFileCreateDefaults;
   char *forwardRequestFile;  // list of available areas from this link
   char *RemoteRobotName;     // Name remote robot (need for ForwardRequest)
   void *msg;                 // active msg to the link (used in areafix)
   int  Pause;                // 0 if no pause (default)
   unsigned autoPause;        // in days
   unsigned level;	          // 0-65535
   unsigned arcmailSize;      // max arcmail size in kb
   // Default link's options
   char *export, *import, *mandatory, *optGrp;
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

enum dupeCheck {dcOff, dcMove, dcDel};
typedef enum dupeCheck e_dupeCheck;

struct arealink {
   s_link *link;
   char export;		// 1 - export yes, 0 - export no
   char import;		// 1 - import yes, 0 - import no
   char mandatory;	// 1 - mandatory yes, 0 - mandatory no
};
typedef struct arealink s_arealink;   

struct area {
   char *areaName;
   char *fileName;
   char *description;
   
   int msgbType;        // MSGTYPE_SDM or MSGTYPE_SQUISH or MSGTYPE_PASSTHROUGH
   s_addr *useAka;
   
   s_arealink **downlinks;  // array of pointers to s_link
   unsigned int downlinkCount;

   unsigned purge, max, dupeHistory;
   e_dupeCheck dupeCheck;
   char tinySB, manual, hide, noPause, mandatory, DOSFile;

   unsigned levelread;	      // 0-65535
   unsigned levelwrite;	      // 0-65535

   void *dupes;        // used internally by hpt. pointer to dupeDataBase
   void *newDupes;     // dito
   char imported;      // dito

   char group;                      // used by reader (and areafix soon)
   char *rwgrp, *wgrp, *rgrp;       // use for -l -w -r echo parameters

   int ccoff;          // 1 if carbon copy is not allowed from this area
};
typedef struct area s_area;

struct fileareatype {
   char *areaName;
   char *pathName;
   char *description;
   
   int pass;           // 1 - Passthrough File Area
   s_addr *useAka;
   
   s_arealink **downlinks;  // array of pointers to s_link
   unsigned int downlinkCount;

//   s_link **downlinks;  // array of pointers to s_link
//   unsigned int downlinkCount;

   char manual, hide, noPause;

   char group;                      // used by reader (and areafix soon)
   char *rwgrp, *wgrp, *rgrp;       // use for -l -w -r echo parameters
};
typedef struct fileareatype s_filearea;

enum carbonType {to, from, kludge};
typedef enum carbonType e_carbonType;

struct carbon {
   e_carbonType type;
   char         *str;     // string to compare
   s_area       *area;    // area to copy messages
   int          export;   // export copied msg?
};
typedef struct carbon s_carbon;

struct unpack {
   int     offset;
   unsigned char *matchCode;
   unsigned char *mask;
   int     codeSize;
   char    *call;
};
typedef struct unpack s_unpack;

struct remap {
   s_addr  oldaddr;
   s_addr  newaddr;
   char   *toname;
};
typedef struct remap s_remap;

struct fidoconfig {
   unsigned int    cfgVersionMajor, cfgVersionMinor;
   char     *name, *location, *sysop;

   unsigned int   addrCount;
   s_addr   *addr;

   unsigned int publicCount;
   char     **publicDir;

   unsigned int  linkCount;
   s_link   *links;

   char     *inbound, *outbound, *protInbound, *listInbound, *localInbound, *tempInbound;
   char     *logFileDir, *dupeHistoryDir, *nodelistDir, *msgBaseDir;
   char     *magic, *areafixhelp, *filefixhelp, *tempOutbound;
   char     *fileAreaBaseDir;
   char     *passFileAreaDir; //Passthrough File Area
   char     *loglevels;

   s_area   netMailArea, dupeArea, badArea;
   unsigned int   echoAreaCount;
   s_area   *echoAreas;
   unsigned int   localAreaCount;
   s_area   *localAreas;
   unsigned int   fileAreaCount;
   s_filearea   *fileAreas;

   unsigned int   routeCount;
   s_route  *route;
   unsigned int   routeFileCount;
   s_route  *routeFile;
   unsigned int   routeMailCount;
   s_route  *routeMail;

   unsigned int   packCount;
   s_pack   *pack;
  //   s_pack   *packDefault;
   unsigned int   unpackCount;
   s_unpack *unpack;
   
   char     *intab, *outtab;
   char     *echotosslog, *importlog, *LinkWithImportlog, *lockfile;

   unsigned int   carbonCount;
   s_carbon *carbons;

   char     **includeFiles;
   unsigned int includeCount;

   unsigned int  remapCount;
   s_remap  *remaps;

   unsigned int areafixFromPkt, areafixKillReports;
   char	    *PublicGroup;
   
   char     *ReportTo;

   unsigned int logEchoToScreen;
   unsigned int separateBundles;
   unsigned int defarcmailSize;
};


typedef struct fidoconfig s_fidoconfig;

s_fidoconfig *readConfig(void);

void disposeConfig(s_fidoconfig *config);

s_link *getLink(s_fidoconfig config, char *addr);
s_link *getLinkFromAddr(s_fidoconfig config, s_addr aka);
s_addr *getAddr(s_fidoconfig config, char *addr);
int    existAddr(s_fidoconfig config, s_addr aka);
s_area *getArea(s_fidoconfig *config, char *areaName);

/**
 * This function return 0 if the link is not linked to the area,
 * else it returns 1.
 */
int    isLinkOfArea(s_link *link, s_area *area);

/**
 * This function dumps the config to a file. The file is in fidoconfig format so,
 * it is possible to change the config in memory and write it to disk.
 * All formatting and comments are removed and the include structure of the config
 * cannot be recreated. So be careful. A file called <fileName> which already exists
 * will be overwritten.
 * 1 if there were problems writing the config
 * 0 else
 */
int dumpConfigToFile(s_fidoconfig *config, char *fileName);

// the following functions are for internal use.
// Only use them if you really know what you do.
char *readLine(FILE *F);
int parseLine(char *line, s_fidoconfig *config);
void parseConfig(FILE *f, s_fidoconfig *config);
char *getConfigFileName(void);
char *trimLine(char *line);

/**
 * This method can be used to get a program-specifically config-filename, in the same directories which are searched for fidoconfig.
 * envVar should be set to a string which resembles a environment-variable which should be checked if it includes the fileName.
 * configName is the filename of the config *without* any prefixes.
 * e.g.
 *      getConfigFileNameForProgram("FIDOCONFIG", "config");
 * is the call which is used for fidoconfig
 */

char *getConfigFileNameForProgram(char *envVar, char *configName);

int isLinkOfFileArea(s_link *link, s_filearea *area);
s_filearea *getFileArea(s_fidoconfig *config, char *areaName);

// this function can be used to dump config to stdout or to an already opened file.
void dumpConfig(s_fidoconfig *config, FILE *f);

#endif
