/* $Id$ */
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
#include <time.h>
#include <stdio.h>
#include <smapi/msgapi.h>
#include "typesize.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MAKE_DLL
#   if defined(_MSC_VER) && (_MSC_VER >= 1200)
#		define _MAKE_DLL_MVC_
#       ifndef _FCONF_EXT
#           define FCONF_EXT __declspec(dllimport)
#       else
#           define FCONF_EXT __declspec(dllexport)
#       endif /* _FCONF_EXT */
#   else
#       define FCONF_EXT extern
#   endif
#else
#   define FCONF_EXT extern
#endif

#ifndef MSGTYPE_PASSTHROUGH /* see smapi/msgapi.h */
#define MSGTYPE_PASSTHROUGH 0x04
#endif

/* PATH_DELIM used for consruct full pathname
 */
#ifndef PATH_DELIM
#  if defined(SASC) || defined(UNIX)
#    define PATH_DELIM  '/'
#  else
#    define PATH_DELIM  '\\'
#  endif
#endif

#define strend(str) ((str) + strlen(str) - 1)

extern char *actualLine, *actualKeyword;

FCONF_EXT int  actualLineNr;
FCONF_EXT char CommentChar;
FCONF_EXT int  fc_trycreate;

extern char wasError;
#define TRUE_COMMENT	"!#$%;"

/* availlist values */
enum _eAvailList
{
  AVAILLIST_FULL = 0, /* Default value */
  AVAILLIST_UNIQUE,
  AVAILLIST_UNIQUEONE
};

typedef enum _eAvailList eAvailList;

FCONF_EXT char *striptwhite(char *str);

typedef struct addr {

   unsigned int zone, net, node, point;
   char   *domain;

} *ps_addr, hs_addr;

typedef struct  pack {
   char    *packer;
   char    *call;
} s_pack, *ps_pack;

typedef struct execonfile {
   char    *filearea;
   char    *filename;
   char    *command;
} s_execonfile, *ps_execonfile;

typedef enum flavour {normal, hold, crash, direct, immediate} e_flavour;
typedef enum pollType {PKT, REQUEST, FLOFILE} e_pollType;
typedef enum _forward {fOff, fOn, fSecure} e_forward;
typedef enum emptypktpwd {eOff, eSecure, eOn} e_emptypktpwd;
typedef enum pktheaderdiffer {pdOff, pdOn} e_pktheaderdiffer;
typedef enum nameCase { eLower, eUpper} e_nameCase;
typedef enum nameCaseConvertion { cLower, cUpper, cDontTouch } e_nameCaseConvertion;
typedef enum bundleFileNameStyle { eUndef, eTimeStamp, eAddrDiff, eAddrDiffAlways, eAmiga, eAddrsCRC32, eAddrsCRC32Always} e_bundleFileNameStyle;
typedef enum emailEncoding { eeMIME, eeSEAT, eeUUE } e_emailEncoding;
typedef enum pauses        { NOPAUSE, EPAUSE, FPAUSE } e_pauses;


typedef struct link {
    hs_addr hisAka, *ourAka;
    char *name;
    char *defaultPwd,               /*  8 byte passwords */
        *pktPwd,
        *ticPwd,
        *areaFixPwd,
        *fileFixPwd,
        *bbsPwd,
        *sessionPwd;
    char *handle;              /*  nickname */
    char *email;
    char *emailFrom; /*  sender address for outgoing emails */
    char *emailSubj;
    e_emailEncoding emailEncoding;
    unsigned int autoAreaCreate;       /*  0 if not allowed for autoareacreate */
    unsigned int autoFileCreate;       /*  0 if not allowed for autofilecreate */
    unsigned int AreaFix;              /*  0 if not allowed for areafix */
    unsigned int FileFix;              /*  0 if not allowed for filefix */
    unsigned int FileFixFSC87Subset;   /*  1 if only FSC87-commands are allowable in TIC files */
    unsigned int forwardRequests;      /*  0 if not allowed forward requests */
    unsigned int forwardFileRequests;  /*  0 if not allowed forward requests for file areas */
    unsigned int denyFRA; /*  denyFwdReqAccess */
    unsigned int denyUFRA; /*  denyUncondFwdReqAccess */

    int  allowEmptyPktPwd;     /*  1 if you want to allow empty packet password in */
    /*    PKT files found in the protected inbound */
    int  allowPktAddrDiffer;   /*  1 if you want to allow the originating address */
    /*    in MSG differ from address in PKT */
    /*    (only for areafix requests) */
    e_forward forwardPkts;     /*  defines, if pkts should be forwarded to this link */
    char *pktFile, *packFile;  /*  used only internally by hpt */
    char *floFile, *bsyFile;   /*  dito */
    ps_pack packerDef;
    e_flavour echoMailFlavour, fileEchoFlavour;
    char *LinkGrp;         /*  link's group for autocreate areas */
    char **AccessGrp;      /*  groups for echo access */
    unsigned int numAccessGrp;
    char *autoAreaCreateFile;  /*  file where autocreated areas are written to */
    char *autoFileCreateFile;
    char *autoAreaCreateDefaults;/*  add default string for autocreated area here */
    char *autoFileCreateDefaults;
    char *forwardRequestFile;  /*  list of available areas from this link */
    char *denyFwdFile;
    unsigned int forwardAreaPriority;/*  Priority when requests area from uplinks */
    char *RemoteRobotName;     /*  Name remote robot (need for ForwardRequest) */
    char *forwardFileRequestFile;  /*  list of available file-areas from this link */
    unsigned int forwardFilePriority;/*  Priority when requests file-area from uplinks */
    char *RemoteFileRobotName;     /*  Name of remote file (tic) robot (need for FileForwardRequest) */
    void *msg;                 /*  active msg to the link (used in areafix) */
    unsigned int noTIC;        /*  0 if TIC files should be generated */
    unsigned int Pause;        /*  0 if no pause (default) */
                               /*  1 echo pause */
                               /*  2 fecho pause */
    unsigned autoPause;        /*  in days */
    unsigned level;	           /*  0-65535 */
    unsigned arcmailSize;      /*  max arcmail size in kb */
    unsigned pktSize;          /*  max .pkt size in kb */
    unsigned maxUnpackedNetmail; /*  max size of *.?ut file in kb. If */
                                 /*  more, then put it into */
                                 /*  bundle. Default 100 (used in bsopack) */
    unsigned int packNetmail;    /*  allows to pack outbound */
                                 /*  netmail. Default no (used in bsopack) */
    unsigned int export, import, mandatory, manual; /*  Default link's options */
    char **optGrp; /*  groups for this options */
    unsigned int numOptGrp;
    unsigned int delNotReceivedTIC; /* 1 - if file not recieved, then remove TIC */
    unsigned int advancedAreafix;  /*  1 - send ~areatag when area delete */
    e_bundleFileNameStyle linkBundleNameStyle; /*  Style bundle filenames (timeStamp, addrDiff... */
    char *msgBaseDir;
    char *fileBaseDir;
    char **frMask; /*  forwardRequestMask groups */
    unsigned int numFrMask;
    char **dfMask; /*  don't forward this */
    unsigned int numDfMask;

    unsigned int afixEchoLimit;
    unsigned int ffixEchoLimit;

    unsigned int autoAreaCreateSubdirs;
    unsigned int autoFileCreateSubdirs;
    char  *fileBox;
    unsigned int fileBoxAlways;
    unsigned int tickerPackToBox;
    unsigned int arcNetmail; /*  1 if pack netmail into arcmail bundles */
    char useFileBox; /*  internal */
    char sb; /*  internal */
    unsigned int noRules;
    FILE *pkt;  /*  for internal usage */
    unsigned int reducedSeenBy; /*  reduces Seen-BYs (fsc-0093) */
    eAvailList availlist;

} s_link, *ps_link;

typedef enum routing {route_zero, host, hub, boss, noroute, nopack, route_extern} e_routing;

typedef enum id {id_route, id_routeMail, id_routeFile} e_id;

typedef struct route {
   e_flavour flavour;
   ps_link   target;   /*  if target = NULL use */
   e_routing routeVia; /*  this */
   char      *pattern;
   char      *viaStr;  /*  fix for realloc of config->links */
   e_id      id;
} s_route, *ps_route;

typedef enum dupeCheck {dcOff, dcMove, dcDel} e_dupeCheck;

typedef struct arealink {
   ps_link link;
   unsigned int export;		/*  1 - export yes, 0 - export no */
   unsigned int import;		/*  1 - import yes, 0 - import no */
   unsigned int mandatory;	/*  1 - mandatory yes, 0 - mandatory no */
   unsigned int manual;		/*  1 - manual yes, 0 - manual no */
   unsigned int defLink;	/*  1 - default uplink */
} s_arealink, *ps_arealink;

typedef struct area {
   char *areaName;
   char *fileName;
   char *description;

   int msgbType;        /*  MSGTYPE_SDM or MSGTYPE_SQUISH or */
                        /*  MSGTYPE_JAM or MSGTYPE_PASSTHROUGH */
   ps_addr useAka;

   ps_arealink *downlinks;  /*  array of pointers to s_link */
   unsigned int downlinkCount;

   unsigned purge, max, dupeHistory;
   char keepUnread, killRead;

   e_dupeCheck dupeCheck;
   char tinySB, killSB, hide, noPause, mandatory, manual, DOSFile, debug;

   unsigned levelread;	      /*  0-65535 */
   unsigned levelwrite;	      /*  0-65535 */
   void *dupes;        /*  used internally by hpt. pointer to dupeDataBase */
   void *newDupes;     /*  dito */
   unsigned int imported;      /*  dito */

   char *group;                      /*  used by reader (and areafix soon) */

   int ccoff;          /*  1 if carbon copy is not allowed from this area */

   /*  Owner and Group options, msgbase mode */
   /*  not set if:  uid = -1 , gid = -1 , fperm = -1 */
   unsigned int uid, gid, fperm;

   int nolink;         /*  do not reply-link area */
   int keepsb;         /*  keep seen-by's and path */
   int scn;            /*  1 if scanned (number of scanned messages-1) */
   int nopack;         /*  do not pack area */

   ps_addr       sbadd;
   unsigned int  sbaddCount;
   ps_addr       sbign;
   unsigned int  sbignCount;

   HAREA harea;        /*   for internal usage; */

} s_area, *ps_area;

typedef struct fileareatype {
   char *areaName;
   char *pathName;
   char *description;

   int sendorig;       /*  1 - Send Original */
   int pass;           /*  1 - Passthrough File Area */
   int noCRC;          /*  0 if CRC check should be done on incoming files */
   int noreplace;      /*  1 - no replace files in this filearea */
   int nodiz;          /*  1 - do not try to get description from <fileDescName> */
   ps_addr useAka;

   unsigned purge;     /*  number of days to keep files. if 0 purging is disabled */

   ps_arealink *downlinks;  /*  array of pointers to s_link */
   unsigned int downlinkCount;

   unsigned levelread;	      /*  0-65535 */
   unsigned levelwrite;	      /*  0-65535 */

   char mandatory, manual, hide, noPause;

   char *group;                      /*  used by reader (and areafix soon) */
} s_filearea, *ps_filearea;

typedef struct bbsareatype {
   char *areaName;
   char *pathName;
   char *description;
} s_bbsarea, *ps_bbsarea;

typedef enum carbonType {ct_to, ct_from, ct_kludge, ct_subject, ct_group,
                         ct_fromarea, ct_msgtext, ct_addr} e_carbonType;
enum {CC_OR=0, CC_AND, CC_NOT};

typedef struct carbon {
   e_carbonType ctype;
   char         *str;     /*  string to compare */
   char         *reason;  /*  reason of copy */
   ps_area      area;     /*  area to copy messages */
   hs_addr       addr;     /*  from addr to compare */
   char         *areaName;/*  name of area to copy messages */
   int          export;   /*  export copied msg? */
   int          netMail;  /*  do this in netmail, not echomail */
   int          move;	  /*  move (not copy) original msg */
   int          extspawn; /*  areaName is name of external program to exec */
   int          rule;     /*  OR|AND|NOT with next carbon expr. */
} s_carbon, *ps_carbon;

typedef struct unpack {
   int     offset;
   unsigned char *matchCode;
   unsigned char *mask;
   int     codeSize;
   char    *call;
} s_unpack, *ps_unpack;

typedef struct remap {
   hs_addr  oldaddr;
   hs_addr  newaddr;
   char   *toname;
} s_remap, *ps_remap;

/* FTS5000 is the standard nodelist format,
   POINTS24 is the German Pointlist format
   POINTS4D is a full 4D pointlist (with 3d "boss" entries) */

typedef enum nodelistFormat { fts5000, points24, points4d } e_nodelistFormat;

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
   int  fileAction;            /* 0 - do nothing */
                               /* 1 - copy file    -  save tic with ticked file */
                               /* 2 - link file  */

} s_savetic, *ps_savetic;

typedef enum linkWithImportLog { lwiNo, lwiYes, lwiKill } e_linkWithImportLog;
typedef enum kludgeAreaNetmail { kanKill, kanIgnore, kanEcho } e_kludgeAreaNetmail;

typedef enum filelistType { flDir, flGlobal, flDirList } e_filelistType;

typedef struct filelist
{
  e_filelistType flType;
  char *destFile;        /* name of file to be written                  */
  char *dirHdrTpl;       /* filename of directory header template       */
  char *dirEntryTpl;     /*             directory entry                 */
  char *dirFtrTpl;       /*             directory footer                */
  char *globHdrTpl;      /*             global header (flGlobal only)   */
  char *globFtrTpl;      /*             global footer (flGlobal only)   */
  char *dirListHdrTpl;   /*             dirlist header (flDirlist only) */
  char *dirListEntryTpl; /*             dirlist entry  (flDirlist only) */
  char *dirListFtrTpl;   /*             dirlist footer (flDirlist only) */
} s_filelist, *ps_filelist;

typedef struct permissions
{
  char *addrMask;	/* address mask for permissions settings */
  char *areaMask;       /* area mask */
} s_permissions;

/*  htick announcer */
typedef struct anndef /* announce definition */
{
  char  *annAreaTag;     /* name of area when annouce will be placed    */
  char **annInclude;     /* array of fileecho names|masks that will be  */
                         /* announced in this annAreaTag                */
  unsigned numbI;        /* number of annInclude masks                  */
  char **annExclude;     /* array of fileecho names|masks that won't be */
                         /* announced in this annAreaTag                */
  unsigned numbE;        /* number of annExclude masks                  */

  char *annto;           /* field TONAME  : in announce message         */
  char *annfrom;         /* field FROMNAME: in announce message         */
  char *annsubj;         /* field SUBJ:     in announce message         */
  char *annorigin;       /* Origin          in announce message         */
  ps_addr annaddrto;      /* field ADDRTO:    in announce message         */
  ps_addr annaddrfrom;    /* field ADDRFROM:  in announce message         */
  char *annmessflags;    /* message  flags string                       */
  dword  attributes;
  unsigned  annforigin;  /* announce address of system who hatched file */
  unsigned  annfrfrom;   /* announce address of system which file recived from */
} s_anndef, *ps_anndef;

typedef struct fidoconfig {
   unsigned int    cfgVersionMajor, cfgVersionMinor;
   char     *name, *location, *sysop, *email;

   unsigned int   addrCount;
   ps_addr  addr;

   unsigned int publicCount;
   char     **publicDir;

   unsigned int  linkCount;
   ps_link  links;

   char     *inbound, *outbound, *protInbound, *listInbound, *localInbound, *tempInbound;
   char     *logFileDir, *dupeHistoryDir, *nodelistDir, *msgBaseDir;
   char     *magic, *areafixhelp, *filefixhelp, *tempOutbound, *ticOutbound;
   char     *tempDir;         /* Common temporary files directory */
   char     *fileAreaBaseDir;
   char     *passFileAreaDir; /* Passthrough File Area */
   char     *busyFileDir;
   char     *semaDir, *badFilesDir;
   char     *loglevels, *screenloglevels;
   char     *hptPerlFile;
   char     *advStatisticsFile;
   char     *newAreaRefuseFile;
   unsigned int fileAreaCreatePerms;

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

   s_area  EchoAreaDefault;
   s_filearea FileAreaDefault;

   unsigned int   routeCount;
   ps_route route;

   unsigned int   packCount;
   ps_pack  pack;
   unsigned int   unpackCount;
   ps_unpack unpack;

   char     *intab, *outtab;
   char     *echotosslog, *statlog, *importlog, *lockfile;
   unsigned loguid, loggid, logperm;
   char     *fileAreasLog, *longNameList, *fileNewAreasLog;
   char     *fileArcList, *filePassList, *fileDupeList;

   e_linkWithImportLog LinkWithImportlog;
   e_kludgeAreaNetmail kludgeAreaNetmail;

   unsigned int   carbonCount;
   ps_carbon carbons;
   unsigned int   carbonAndQuit;
   unsigned int   carbonKeepSb;  /*  keep SeenBy's and PATH in carbon area */
   unsigned int   carbonOut;     /*  carbon outgoing messages */
   unsigned int   exclPassCC;    /*  don't carbon passthough */
   unsigned int   carbonExcludeFwdFrom; /*  don't print " * Forwarded from area" */

   unsigned int  remapCount;
   ps_remap remaps;

   unsigned int areafixFromPkt, areafixKillRequests;
   unsigned int areafixMsgSize, areafixQueryReports;
   long areafixReportsAttr;
   char *areafixReportsFlags;
   char *areafixFromName;
   char *filefixFromName;
   unsigned int forwardRequestTimeout;
   unsigned int killedRequestTimeout;
   int idlePassthruTimeout;
   char *areafixSplitStr, *areafixOrigin, *robotsArea;
   char *areafixQueueFile;

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
   unsigned int disablePID;

   char *afterUnpack, *beforePack;
   char *processPkt;

   unsigned int createDirs;
   unsigned int longDirNames, splitDirs;

   unsigned int addDLC, fileSingleDescLine, fileCheckDest;
   unsigned int filefixKillRequests;
   long filefixReportsAttr;
   char *filefixReportsFlags;
   e_nameCaseConvertion convertLongNames, convertShortNames;

   char     *fileDescName;
   unsigned int fileDescPos, DLCDigits, fileMaxDupeAge;
   unsigned int fileFileUMask, fileDirUMask;
   unsigned int originInAnnounce; /* Show origin in announce (for htick) */
   unsigned int MaxTicLineLength; /*  Maximum length line in TIC (for htick) */

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

#if defined ( __NT__ ) || defined(__MINGW32__) || defined(__CYGWIN__)
   unsigned int setConsoleTitle; /* change console title */
#endif

   ps_addr       addToSeen;
   unsigned int  addToSeenCount;

   ps_addr       ignoreSeen;
   unsigned int  ignoreSeenCount;

   char *tearline, *origin;

   e_bundleFileNameStyle bundleNameStyle;

   unsigned int keepTrsMail; /*  Keep Transit Netmail */
   unsigned int keepTrsFiles; /*  Keep Transit Files */
   unsigned int createFwdNonPass;
   unsigned int autoPassive;

   ps_filelist filelists;
   unsigned int filelistCount;

   char *netmailFlag;
   char *aacFlag;
   char *afcFlag;

   unsigned int minDiskFreeSpace;
   unsigned int advisoryLock;

   char *areafixNames;
   char *filefixNames;
   char *reqidxDir;         /* directory for herp request index files */

   int syslogFacility;     /* facility to use when logging via syslog */

   s_permissions *readOnly; /* temporary storage of address & area masks */
   int readOnlyCount;
   s_permissions *writeOnly;
   int writeOnlyCount;
   char *fileBoxesDir;
   char *announceSpool;
   char *notValidFNChars;
   char *rulesDir;
   unsigned int packNetMailOnScan;

   char *seqDir;	/* for msgid generation */
   unsigned long seqOutrun;

   char**   uuEGrp; /*  uueEchoAreaGroups; */
   unsigned int numuuEGrp;

   /* for emailpkt */
   char *sendmailcmd;   /* send e-mail command line*/
   /*  htick announcer */
   ps_anndef AnnDefs;
   unsigned int ADCount;

} s_fidoconfig, *ps_fidoconfig;

struct message {
   /* Address block */
   hs_addr destAddr, origAddr;

/*    UINT16 attributes; */
   dword  attributes;
   UCHAR  datetime[22];
   CHAR   netMail;
   INT32  textLength;
   INT    recode;

   char   *toUserName, *fromUserName, *subjectLine;
   char   *text;
};

struct pktHeader {
   /* Address block */
   hs_addr destAddr, origAddr;

   UINT16 auxNet;

   /* product specific */
   UCHAR  hiProductCode,
          loProductCode;
   UCHAR  majorProductRev,
          minorProductRev;

   /* date */
   time_t pktCreated;

   UINT16 capabilityWord;

   UINT32 prodData;

   char  pktPassword[9]; /* password + \0 */
};

typedef struct pktHeader s_pktHeader;
typedef struct message   s_message;


typedef enum { M_NOTDEF=0,M_HPT, M_HTICK, M_OTHER } e_known_moduls;

typedef struct {
    int module;
/*   module = 0 - not defined */
/*   module = 1 - hpt */
/*   module = 2 - htick */
/*   module = 3 - all other */
    ps_fidoconfig config;
    /*  for future usage */
} sApp;



#ifndef _MAKE_DLL_MVC_
	extern sApp theApp;
#else
	FCONF_EXT sApp theApp;
#endif

FCONF_EXT void SetAppModule(int mod); /*  setup struct sApp */

/* Read fidoconfig from file into memory.
 * Parameter: filename or NULL
 * if NULL: try to find FIDOCONFIG enviroment variable, next use hardcoded path
 * Return NULL and print diagnostic message to stdout if error(s) found.
 */
FCONF_EXT ps_fidoconfig readConfig(const char *cfgFile);

/* Dispose fidoconfig structure: free memory.
 */
FCONF_EXT void disposeConfig(ps_fidoconfig config);

FCONF_EXT ps_link getLink(s_fidoconfig *config, char *addr);
FCONF_EXT ps_link getLinkForArea(const s_fidoconfig *config, char *addr, s_area *area);
FCONF_EXT ps_link getLinkForFileArea(const s_fidoconfig *config, char *addr, s_filearea *area);
FCONF_EXT ps_link getLinkFromAddr(s_fidoconfig *config, hs_addr aka);
FCONF_EXT ps_addr getAddr(const s_fidoconfig *config, char *addr);
int    existAddr(s_fidoconfig *config, hs_addr aka);

/* find echo & local areas in config */
FCONF_EXT ps_area getArea(ps_fidoconfig config, char *areaName);

/* find only echo areas in config */
FCONF_EXT ps_area getEchoArea(ps_fidoconfig config, char *areaName);

/* find netmail areas in config */
FCONF_EXT ps_area getNetMailArea(ps_fidoconfig config, char *areaName);

/**
 * This function return 0 if the link is not linked to the area,
 * else it returns 1.
 */
FCONF_EXT int isLinkOfArea(ps_link link, s_area *area);

/**
 * This function return -1 if the link is not linked to the area,
 * else it returns index of link into arealinks array.
 */

FCONF_EXT int isAreaLink(hs_addr link, s_area *area);

/**
 * This function tests if link addr is our aka.
 */

FCONF_EXT int isOurAka(ps_fidoconfig config, hs_addr link);



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

/*  the following functions are for internal use. */
/*  Only use them if you really know what you do. */
FCONF_EXT char *readLine(FILE *F);
FCONF_EXT int  parseLine(char *line, ps_fidoconfig config);
int parsePath(char *token, char **var, char **alreadyDefined);
FCONF_EXT char *getConfigFileName(void);
FCONF_EXT char *trimLine(char *line);
FCONF_EXT void carbonNames2Addr(s_fidoconfig *config);
FCONF_EXT int  init_conf(const char *conf_name);
FCONF_EXT void close_conf(void);
FCONF_EXT void setvar(char *name, char *value);
FCONF_EXT char *getvar(char *name);
void closeall(void);
FCONF_EXT char *configline(void);
FCONF_EXT char *stripComment(char *line);
void checkIncludeLogic(ps_fidoconfig config);

FCONF_EXT const char* getCurConfName();
FCONF_EXT long getCurConfPos();
FCONF_EXT long get_hcfgPos();
FCONF_EXT FILE *get_hcfg();
FCONF_EXT const char *cfgEol();

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
FCONF_EXT ps_filearea getFileArea(char *areaName);

/*  this function can be used to dump config to stdout or to an already opened file. */
void dumpConfig(ps_fidoconfig config, FILE *f);

/*  return 1 if group found in array of strings, else return 0 */
FCONF_EXT int grpInArray(char *group, char **strarray, unsigned int len);

/* delete the area from in-core config */
FCONF_EXT void fc_freeEchoArea(s_area     *area);
FCONF_EXT void fc_freeFileArea(s_filearea *area);


/*  define exit codes for non unix systems */
#ifndef _SYSEXITS_H
#define _SYSEXITS_H
#define EX_OK           0       /* successful termination */
#define EX_USAGE        64      /* command line usage error */
#define EX_NOINPUT      66      /* cannot open input */
#define EX_UNAVAILABLE  69      /* service unavailable */
#define EX_SOFTWARE     70      /* internal software error */
#define EX_CANTCREAT    73      /* can't create (user) output file */
#define EX_IOERR        74      /* input/output error */
#define EX_TEMPFAIL     75      /* temp failure; user is invited to retry */
#define EX_CONFIG       78      /* configuration error */
#endif

#ifdef __cplusplus
 }
#endif

#endif
