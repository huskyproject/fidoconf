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
#include <smapi/msgapi.h>

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct addr {

   unsigned int zone, net, node, point;
   char   *domain;

} s_addr, *ps_addr;

typedef struct pack {
   char    *packer;
   char    *call;
} s_pack, *ps_pack;

typedef struct execonfile {
   char    *filearea;
   char    *filename;
   char    *command;
} s_execonfile, *ps_execonfile;

typedef enum flavour {normal, hold, crash, direct, immediate} e_flavour;
typedef enum _forward {fOff, fSecure, fOn} e_forward;
typedef enum emptypktpwd {eOff, eSecure, eOn} e_emptypktpwd;
typedef enum pktheaderdiffer {pdOff, pdOn} e_pktheaderdiffer;
typedef enum nameCase { eLower, eUpper} e_nameCase;

typedef struct link {
   s_addr hisAka, *ourAka;
   char *name;
   char *defaultPwd,               // 8 byte passwords
        *pktPwd,
        *ticPwd,
        *areaFixPwd,
        *fileFixPwd,
        *bbsPwd,
        *sessionPwd;
   char *handle;              // nickname
   char *email;
   unsigned int autoAreaCreate;       // 0 if not allowed for autoareacreate
   unsigned int autoFileCreate;       // 0 if not allowed for autofilecreate
   unsigned int AreaFix;              // 0 if not allowed for areafix
   unsigned int FileFix;              // 0 if not allowed for filefix
   unsigned int forwardRequests;      // 0 if not allowed forward requests
   unsigned int fReqFromUpLink;	      // 0 - ignore added unknown area (no area in cfg)
   int  allowEmptyPktPwd;     // 1 if you want to allow empty packet password in
                              //   PKT files found in the protected inbound
   int  allowPktAddrDiffer;   // 1 if you want to allow the originating address
                              //   in MSG differ from address in PKT
                              //   (only for areafix requests)
   e_forward forwardPkts;     // defines, if pkts should be forwarded to this link
   char *pktFile, *packFile;  // used only internally by hpt
   char *floFile, *bsyFile;   // dito
   ps_pack packerDef;
   e_flavour echoMailFlavour, fileEchoFlavour;
   char *LinkGrp;	      // link's group for autocreate areas
   char **AccessGrp;	      // groups for echo access
   unsigned int numAccessGrp;
   char *autoAreaCreateFile;  // file where autocreated areas are written to
   char *autoFileCreateFile;
   char *autoAreaCreateDefaults;// add default string for autocreated area here
   char *autoFileCreateDefaults;
   char *forwardRequestFile;  // list of available areas from this link
   char *RemoteRobotName;     // Name remote robot (need for ForwardRequest)
   void *msg;                 // active msg to the link (used in areafix)
   unsigned int noTIC;        // 0 if TIC files should be generated
   unsigned int Pause;        // 0 if no pause (default)
   unsigned autoPause;        // in days
   unsigned level;	          // 0-65535
   unsigned arcmailSize;      // max arcmail size in kb
   unsigned pktSize;          // max .pkt size in kb
   unsigned int export, import, mandatory; // Default link's options
   char **optGrp; // groups for this options
   unsigned int numOptGrp;
} s_link, *ps_link;

typedef enum routing {host = 1, hub, boss, noroute} e_routing;

typedef struct route {
   e_flavour flavour;
   char      enc;
   ps_link   target;   // if target = NULL use
   e_routing routeVia; // this
   char      *pattern;
   char      *viaStr;  // fix for realloc of config->links
} s_route, *ps_route;

typedef enum dupeCheck {dcOff, dcMove, dcDel} e_dupeCheck;

typedef struct arealink {
   ps_link link;
   unsigned int export;		// 1 - export yes, 0 - export no
   unsigned int import;		// 1 - import yes, 0 - import no
   unsigned int mandatory;	// 1 - mandatory yes, 0 - mandatory no
} s_arealink, *ps_arealink;

typedef struct area {
   char *areaName;
   char *fileName;
   char *description;

   int msgbType;        // MSGTYPE_SDM or MSGTYPE_SQUISH or
                        // MSGTYPE_JAM or MSGTYPE_PASSTHROUGH
   ps_addr useAka;

   ps_arealink *downlinks;  // array of pointers to s_link
   unsigned int downlinkCount;

   unsigned purge, max, dupeSize, dupeHistory;
   char keepUnread, killRead;

   e_dupeCheck dupeCheck;
   char tinySB, hide, noPause, mandatory, DOSFile;

   unsigned levelread;	      // 0-65535
   unsigned levelwrite;	      // 0-65535
   void *dupes;        // used internally by hpt. pointer to dupeDataBase
   void *newDupes;     // dito
   unsigned int imported;      // dito

   char *group;                      // used by reader (and areafix soon)

   int ccoff;          // 1 if carbon copy is not allowed from this area

   // Owner and Group options, msgbase mode
   // not set if:  uid = -1 , gid = -1 , fperm = -1
   unsigned int uid, gid, fperm;

   int nolink;         // do not reply-link area
   int keepsb;         // keep seen-by's and path
   int scn;            // 1 if scanned
   int nopack;         // do not pack area
} s_area, *ps_area;

typedef struct fileareatype {
   char *areaName;
   char *pathName;
   char *description;

   int sendorig;       // 1 - Send Original
   int pass;           // 1 - Passthrough File Area
   int noCRC;          // 0 if CRC check should be done on incoming files
   ps_addr useAka;

   ps_arealink *downlinks;  // array of pointers to s_link
   unsigned int downlinkCount;

   unsigned levelread;	      // 0-65535
   unsigned levelwrite;	      // 0-65535

   char mandatory, hide, noPause;

   char *group;                      // used by reader (and areafix soon)
} s_filearea, *ps_filearea;

typedef struct bbsareatype {
   char *areaName;
   char *pathName;
   char *description;
} s_bbsarea, *ps_bbsarea;

typedef enum carbonType {ct_to, ct_from, ct_kludge, ct_subject, ct_msgtext} e_carbonType;

typedef struct carbon {
   e_carbonType ctype;
   char         *str;     // string to compare
   char         *reason;  // reason of copy
   ps_area      area;    // area to copy messages
   char         *areaName;// name of area to copy messages
   int          export;   // export copied msg?
   int          netMail;  // do this in netmail, not echomail
   int          move;	  // move (not copy) original msg
   int          extspawn; // areaName is name of external program to exec
} s_carbon, *ps_carbon;

typedef struct unpack {
   int     offset;
   unsigned char *matchCode;
   unsigned char *mask;
   int     codeSize;
   char    *call;
} s_unpack, *ps_unpack;

typedef struct remap {
   s_addr  oldaddr;
   s_addr  newaddr;
   char   *toname;
} s_remap, *ps_remap;

/* FTS5000 is the standard nodelist format,
   POINTS24 is the German Pointlist format */

typedef enum nodelistFormat { fts5000, points24 } e_nodelistFormat;

typedef struct nodelist {
   char *nodelistName;        /* name of unpacked nodelist w/o path */
   char *diffUpdateStem;      /* with pathname */
   char *fullUpdateStem;      /* with pathname */
   unsigned int defaultZone;
   int format;
} s_nodelist, *ps_nodelist;

typedef enum typeDupeCheck {
                    hashDupes, /*Base bild from crc32*/
              hashDupesWmsgid, /*Base bild from crc32+MSGID*/
                    textDupes, /*Base bild from FromName+ToName+Subj+MSGID*/
               commonDupeBase  /*Common base for all areas bild from crc32*/
} e_typeDupeCheck;

typedef struct savetictype {
   char *fileAreaNameMask;
   char *pathName;
} s_savetic, *ps_savetic;

typedef struct fidoconfig {
   unsigned int    cfgVersionMajor, cfgVersionMinor;
   char     *name, *location, *sysop;

   unsigned int   addrCount;
   ps_addr  addr;

   unsigned int publicCount;
   char     **publicDir;

   unsigned int  linkCount;
   ps_link  links;

   char     *inbound, *outbound, *protInbound, *listInbound, *localInbound, *tempInbound;
   char     *logFileDir, *dupeHistoryDir, *nodelistDir, *msgBaseDir;
   char     *magic, *areafixhelp, *filefixhelp, *tempOutbound, *ticOutbound;
   char     *fileAreaBaseDir;
   char     *passFileAreaDir; //Passthrough File Area
   char     *busyFileDir;
   char     *semaDir, *badFilesDir;
   char     *loglevels;

   s_area   dupeArea, badArea;
   unsigned int   netMailAreaCount;
   ps_area  netMailAreas;
   unsigned int   echoAreaCount;
   ps_area  echoAreas;
   unsigned int   localAreaCount;
   ps_area  localAreas;
   unsigned int   fileAreaCount;
   ps_filearea  fileAreas;
   unsigned int   bbsAreaCount;
   ps_bbsarea  bbsAreas;

   unsigned int   routeCount;
   ps_route route;
   unsigned int   routeFileCount;
   ps_route routeFile;
   unsigned int   routeMailCount;
   ps_route routeMail;

   unsigned int   packCount;
   ps_pack  pack;
  //   s_pack   *packDefault;
   unsigned int   unpackCount;
   ps_unpack unpack;

   char     *intab, *outtab;
   char     *echotosslog, *importlog, *LinkWithImportlog, *lockfile;
   unsigned loguid, loggid, logperm;
   char     *fileAreasLog, *longNameList, *fileNewAreasLog;
   char     *fileArcList, *filePassList, *fileDupeList;
   char     *msgidfile;

   unsigned int   carbonCount;
   ps_carbon carbons;
   unsigned int   carbonAndQuit;
   unsigned int   carbonKeepSb;  // keep SeenBy's and PATH in carbon area

   char     **includeFiles;
   unsigned int includeCount;

   unsigned int  remapCount;
   ps_remap remaps;

   unsigned int areafixFromPkt, areafixKillReports, areafixKillRequests, areafixMsgSize;
   char *areafixSplitStr, *areafixOrigin;

   char     **PublicGroup;
   unsigned int numPublicGroup;
   char     *ReportTo;

   unsigned int   execonfileCount;
   ps_execonfile execonfile;

   unsigned int logEchoToScreen;
   unsigned int separateBundles;
   unsigned int defarcmailSize;
   unsigned int ignoreCapWord;
   unsigned int noProcessBundles;
   unsigned int disableTID;

   char *afterUnpack, *beforePack;
   /* +AS+ */
   char *processPkt;
   /* -AS- */

   unsigned int createDirs;
   unsigned int longDirNames, splitDirs;

   unsigned int addDLC, fileSingleDescLine, fileCheckDest;
   unsigned int filefixKillReports, filefixKillRequests;

   unsigned int fileDescPos, DLCDigits, fileMaxDupeAge;
   unsigned int fileFileUMask, fileDirUMask;
   unsigned int originInAnnounce; //Show origin in announce (for htick)
   unsigned int MaxTicLineLength; // Maximum length line in TIC (for htick)

   char     *fileLocalPwd, *fileLDescString;

   unsigned int   saveTicCount;
   ps_savetic    saveTic;

   unsigned int nodelistCount;
   ps_nodelist nodelists;

   char     *fidoUserList; /* without path name - is in nodelistDir */

   e_typeDupeCheck typeDupeBase;
   unsigned int areasMaxDupeAge;

   ps_link   linkDefaults;
   int      describeLinkDefaults;
   e_nameCase createAreasCase;
   e_nameCase areasFileNameCase;
   char *tossingExt;
} s_fidoconfig, *ps_fidoconfig;


ps_fidoconfig readConfig(void);

void disposeConfig(ps_fidoconfig config);

ps_link getLink(s_fidoconfig config, char *addr);
ps_link getLinkForArea(s_fidoconfig config, char *addr, s_area *area);
ps_link getLinkFromAddr(s_fidoconfig config, s_addr aka);
ps_addr getAddr(s_fidoconfig config, char *addr);
int    existAddr(s_fidoconfig config, s_addr aka);

/* find echo & local areas in config */
ps_area getArea(ps_fidoconfig config, char *areaName);

/* find only echo areas in config */
ps_area getEchoArea(ps_fidoconfig config, char *areaName);

/* find netmail areas in config */
ps_area getNetMailArea(ps_fidoconfig config, char *areaName);

/**
 * This function return 0 if the link is not linked to the area,
 * else it returns 1.
 */
int    isLinkOfArea(ps_link link, s_area *area);

/**
 * This function dumps the config to a file. The file is in fidoconfig format so,
 * it is possible to change the config in memory and write it to disk.
 * All formatting and comments are removed and the include structure of the config
 * cannot be recreated. So be careful. A file called <fileName> which already exists
 * will be overwritten.
 * 1 if there were problems writing the config
 * 0 else
 */
int dumpConfigToFile(ps_fidoconfig config, char *fileName);

// the following functions are for internal use.
// Only use them if you really know what you do.
char *readLine(FILE *F);
int parseLine(char *line, ps_fidoconfig config);
void parseConfig(FILE *f, ps_fidoconfig config);
char *getConfigFileName(void);
char *trimLine(char *line);
void carbonNames2Addr(s_fidoconfig *config);


/**
 * This method can be used to get a program-specifically config-filename, in the same directories which are searched for fidoconfig.
 * envVar should be set to a string which resembles a environment-variable which should be checked if it includes the fileName.
 * configName is the filename of the config *without* any prefixes.
 * e.g.
 *      getConfigFileNameForProgram("FIDOCONFIG", "config");
 * is the call which is used for fidoconfig
 */

char *getConfigFileNameForProgram(char *envVar, char *configName);

int isLinkOfFileArea(ps_link link, ps_filearea area);
ps_filearea getFileArea(ps_fidoconfig config, char *areaName);

// this function can be used to dump config to stdout or to an already opened file.
void dumpConfig(ps_fidoconfig config, FILE *f);

// return 1 if group found in array of strings, else return 0
int grpInArray(char *group, char **strarray, unsigned int len);

#ifdef __cplusplus
 }
#endif

#endif
