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

#ifdef __cplusplus
extern "C" {
#endif

#include <huskylib/huskylib.h>


#ifndef MSGTYPE_SDM /* see smapi/msgapi.h */
#define MSGTYPE_SDM 0x01
#endif
#ifndef MSGTYPE_SQUISH /* see smapi/msgapi.h */
#define MSGTYPE_SQUISH 0x02
#endif
#ifndef MSGTYPE_PASSTHROUGH /* see smapi/msgapi.h */
#define MSGTYPE_PASSTHROUGH 0x04
#endif
#ifndef MSGTYPE_JAM /* see smapi/msgapi.h */
#define MSGTYPE_JAM 0x08
#endif

#define AREANAMELEN 60
/* PATH_DELIM used for consruct full pathname
 */
#ifndef PATH_DELIM
#  if defined (SASC) || defined (__UNIX__)
#    define PATH_DELIM '/'
#  else
#    define PATH_DELIM '\\'
#  endif
#endif

#define strend(str) ((str) + strlen(str) - 1)

extern char * actualLine, * actualKeyword;
HUSKYEXT int actualLineNr;
HUSKYEXT char CommentChar;
HUSKYEXT int fc_trycreate;
extern char wasError;
#define TRUE_COMMENT "!#$%;"

#define ZIPINTERNAL "zipInternal"
/* availlist values */
enum _eAvailList
{
    AVAILLIST_FULL = 0, /* Default value */
    AVAILLIST_UNIQUE, AVAILLIST_UNIQUEONE
};

typedef enum _eAvailList eAvailList;
HUSKYEXT char * striptwhite(char * str);

typedef struct  pack
{
    char * packer;
    char * call;
} s_pack, *ps_pack;
typedef struct execonfile
{
    char * filearea;
    char * filename;
    char * command;
} s_execonfile, *ps_execonfile;
typedef enum flavour
{
    flNormal = 1, flHold, flCrash, flDirect, flImmediate, /*the last one:*/ flUndef = 0
}                                                             e_flavour;
typedef enum pollType {
    PKT, REQUEST,
    FLOFILE
}                                                             e_pollType;
typedef enum _forward {
    fOff, fOn,
    fSecure
}                                                             e_forward;
typedef enum emptypktpwd {
    eOff, eSecure,
    eOn
}                                                             e_emptypktpwd;
typedef enum pktheaderdiffer {
    pdOff,
    pdOn
}                                                             e_pktheaderdiffer;
typedef enum nameCase {
    eLower,
    eUpper
}                                                             e_nameCase;
typedef enum nameCaseConvertion {
    cLower, cUpper,
    cDontTouch
}                                                             e_nameCaseConvertion;
typedef enum bundleFileNameStyle {
    eUndef, eTimeStamp, eAddrDiff, eAddrDiffAlways, eAmiga, eAddrsCRC32, eAddrsCRC32Always
}                                                             e_bundleFileNameStyle;
typedef enum emailEncoding {eeMIME, eeSEAT, eeUUE}            e_emailEncoding;
typedef enum pauses {NOPAUSE = 0, ECHOAREA = 1, FILEAREA = 2} e_pauses;    /*bitmasks! ECHOAREA
                                                                              & FILEAREA use
                                                                              also in
                                                                              s_area.areatype*/
typedef struct link_robot
{
    unsigned int on;           /*  0 if not allowed for robot (was: AreaFix) */
    char *       pwd;          /* (was: areaFixPwd) */
    char *       baseDir;      /* (was: msgBaseDir, fileBaseDir) */
    long         reportsAttr;  /* (was: areafixReportsAttr) */
    char *       reportsFlags; /* (was: areafixReportsFlags) */
    unsigned int echoLimit; /* (was: afixEchoLimit) */
    unsigned int noRules;   /* don't send rules on subscribe */
    unsigned int autoCreate;/*  0 if not allowed for autoareacreate (was: autoAreaCreate) */
    char *       autoCreateFile; /*  file where autocreated areas are written to (was:
                                    autoAreaCreateFile) */
    char *       autoCreateDefaults; /* add default string for autocreated area here (was:
                                        autoAreaCreateDefaults) */
    unsigned int autoSubscribe; /* 0 if the link is not autosubscribed to an autocreated
                                   (file)echo */
    unsigned int forwardRequests; /*  0 if not allowed forward requests */
    char *       name;    /* remote robot (was: RemoteRobotName) */
    unsigned int forwardPriority; /*  Priority when requests area from uplinks (was:
                                     forwardAreaPriority) */
    char *       fwdFile; /* list of available areas from this link */
    char *       denyFwdFile;
    char **      frMask; /*  forwardRequestMask groups */
    unsigned int numFrMask;
    char **      dfMask; /*  don't forward this */
    unsigned int numDfMask;
    unsigned int denyFRA;  /*  denyFwdReqAccess */
    unsigned int denyUFRA; /*  denyUncondFwdReqAccess */
} s_link_robot, *ps_link_robot;
typedef struct link
{
    hs_addr hisAka, * ourAka, hisPackAka;
    char *  name;
    char *  defaultPwd,        /*  8 byte passwords */
         * pktPwd, * ticPwd, * bbsPwd, * sessionPwd;
    char *          handle;    /*  nickname */
    char *          email;
    char *          emailFrom; /*  sender address for outgoing emails */
    char *          emailSubj;
    e_emailEncoding emailEncoding;
    s_link_robot    areafix;          /* areafix data */
    s_link_robot    filefix;          /* filefix data */
    unsigned int    FileFixFSC87Subset; /*  1 if only FSC87-commands are allowable in TIC files
                                           */
    int             allowEmptyPktPwd; /*  1 if you want to allow empty packet password in */
    /*    PKT files found in the protected inbound */
    int allowPktAddrDiffer;    /*  1 if you want to allow the originating address */
    /*    in MSG differ from address in PKT */
    /*    (only for areafix requests) */
    e_forward    forwardPkts;  /*  defines, if pkts should be forwarded to this link */
    char *       pktFile, * packFile; /*  used only internally by hpt */
    char *       floFile, * bsyFile; /*  dito */
    ps_pack      packerDef;
    e_flavour    netMailFlavour, echoMailFlavour, fileEchoFlavour;
    char *       LinkGrp;     /*  link's group for autocreate areas */
    char **      AccessGrp;   /*  groups for echo access */
    unsigned int numAccessGrp;
    void *       msg;         /*  active msg to the link (used in areafix) */
    unsigned int noTIC;       /*  0 if TIC files should be generated */
    unsigned int Pause;       /*  0 if no pause (default) */
                              /*  1 echo pause */
                              /*  2 fecho pause */
    unsigned     autoPause;   /*  in days */
    unsigned     level;       /*  0-65535 */
    unsigned int dailyBundles; /*  start new arcmail bundle each day */
    unsigned     arcmailSize; /*  max arcmail size in kb */
    unsigned     pktSize;     /*  max .pkt size in kb */
    unsigned     maxUnpackedNetmail; /*  max size of *.?ut file in kb. If */
                              /*  more, then put it into */
                              /*  bundle. Default 100 (used in bsopack) */
    unsigned int packNetmail;    /*  allows to pack outbound */
                              /*  netmail. Default no (used in bsopack) */
    unsigned int          aexport, import, mandatory, manual; /*  Default link's options */
    char **               optGrp; /*  groups for this options */
    unsigned int          numOptGrp;
    unsigned int          delNotReceivedTIC; /* 1 - if file not recieved, then remove TIC */
    unsigned int          advancedAreafix; /*  1 - send ~areatag when area delete */
    e_bundleFileNameStyle linkBundleNameStyle; /*  Style bundle filenames (timeStamp,
                                                  addrDiff... */
    unsigned int          autoAreaCreateSubdirs;
    unsigned int          autoFileCreateSubdirs;
    char *                fileBox;
    unsigned int          fileBoxAlways;
    unsigned int          tickerPackToBox;
    unsigned int          arcNetmail; /*  1 if pack netmail into arcmail bundles */
    char                  useFileBox; /*  internal */
    char                  sb; /*  internal */
    FILE *                pkt; /*  for internal usage */
    unsigned int          reducedSeenBy; /*  reduces Seen-BYs (fsc-0093) */
    eAvailList            availlist;
    unsigned int          sendNotifyMessages; /* send netmail messages about */
                              /* forward request timeout etc... */
    unsigned int allowRemoteControl; /* allow %from command for link */
    unsigned int unsubscribeOnAreaDelete; /* send "-area" request on area delete */
    unsigned int denyRescan;  /* denies or allows link to rescan areas */
    char **      RescanGrp;   /* specifies which area groups allow/deny to rescan */
    unsigned int numRescanGrp;
    int          rescanLimit; /* absolute max of msgs link can ask for rescan */
} s_link, *ps_link;
typedef enum routing {route_zero, host, hub, boss, noroute, nopack, route_extern} e_routing;
typedef enum id {id_route, id_routeMail, id_routeFile}                            e_id;
typedef struct route
{
    e_flavour flavour;
    ps_link   target;  /*  if target = NULL use */
    e_routing routeVia; /*  this */
    char *    pattern;
    char *    viaStr;  /*  fix for realloc of config->links */
    e_id      id;
} s_route, *ps_route;
typedef struct husky_group
{
    char * name;        /* group name */
    char * desc;        /* group description */
} s_group, *ps_group;
typedef enum dupeCheck {dcOff, dcMove, dcDel}                   e_dupeCheck;
typedef enum area_def_subscribing_t {RW = 0, RO, WO}            e_area_def_subscribing;
typedef enum scanMode {smNone = 0, smNever, smManual, smListed} e_scanMode;
typedef struct arealink
{
    ps_link      link;
    unsigned int aexport;   /*  1 - export yes, 0 - export no */
    unsigned int import;    /*  1 - import yes, 0 - import no */
    unsigned int mandatory; /*  1 - mandatory yes, 0 - mandatory no */
    unsigned int manual;    /*  1 - manual yes, 0 - manual no */
    unsigned int rescan;    /*  1 - rescan yes, 0 - rescan no */
    unsigned int defLink;   /*  1 - default uplink */
} s_arealink, *ps_arealink;
typedef struct area
{
    int    areaType;    /* ECHOAREA, FILEAREA */
    char * areaName;
    char * fileName;    /* messagebase file for echoarea, directory for filearea or
                           "passthrough" for both */
    char * description;
    int    msgbType;    /*  MSGTYPE_SDM or MSGTYPE_SQUISH or */
                        /*  MSGTYPE_JAM or MSGTYPE_PASSTHROUGH */
    ps_addr       useAka;
    ps_arealink * downlinks; /*  array of pointers to s_link */
    unsigned int  downlinkCount;
    unsigned      purge, max, dupeHistory;
    char          keepUnread, killRead;
    e_dupeCheck   dupeCheck;
    e_scanMode    scanMode; /* val: conditions to scan area */
    char          tinySB, killSB, hide, noPause, mandatory, manual, DOSFile, debug;
    unsigned      levelread;           /*  0-65535 */
    unsigned      levelwrite;          /*  0-65535 */
    void *        dupes;               /*  used internally by hpt. pointer to dupeDataBase */
    void *        newDupes;            /*  dito */
    unsigned int  imported;            /*  dito */
    unsigned int  tooOld;              /* move incoming mail older than x days to BadArea */
                                       /* 0 - disabled */
    unsigned int tooNew;               /* move incoming mail newer than x days to Bad Area */
                                       /* 0 - disabled */
    char * group;                      /*  used by reader (and areafix soon) */
    int    ccoff;                      /*  1 if carbon copy is not allowed from this area */
    /*  Owner and Group options, msgbase mode */
    /*  not set if:  uid = -1 , gid = -1 , fperm = -1 */
    unsigned int           uid, gid, fperm;
    int                    nolink;     /*  do not reply-link area */
    int                    keepsb;     /*  keep seen-by's and path */
    int                    scn;        /*  1 if scanned (number of scanned messages-1) */
    int                    nopack;     /*  do not pack area */
    int                    killMsgBase; /*  kill msg base */
    int                    paused;     /*  1 if area is paused */
    int                    noautoareapause; /*  do not automatically pause area */
    int                    sbkeep_all; /*  1 - keep all SEEN BY's when zone-gating */
    ps_addr                sbadd;
    unsigned int           sbaddCount;
    ps_addr                sbign;
    unsigned int           sbignCount;
    ps_addr                sbstrip; /* AKAs to strip */
    unsigned int           sbstripCount;
    ps_addr                sbkeep;  /* AKAs to keep when zone-gating */
    unsigned int           sbkeepCount;
    e_area_def_subscribing def_subscribing; /* Default mode for new links (-r -w). */
/*   HAREA harea; */       /*   for internal usage; */
    void * harea; /* for internal usage: pointer to area handle. Store HAREA type variable (see
                     msgapi.h in smapi) */
    /* filecho options */
    int sendorig;      /*  1 - Send Original */
    int noCRC;         /*  0 if CRC check should be done on incoming files */
    int noreplace;     /*  1 - no replace files in this filearea */
    int nodiz;         /*  1 - do not try to get description from <fileDescName> */
    int rename;        /*  1 - rename file in case of duplicates */
} s_area, *ps_area;
typedef struct bbsareatype
{
    char * areaName;
    char * pathName;
    char * description;
} s_bbsarea, *ps_bbsarea;
typedef enum carbonType
{
    ct_to, ct_from, ct_kludge, ct_subject, ct_group, ct_fromarea, ct_msgtext, ct_addr
} e_carbonType;
typedef enum {CC_OR = 0, CC_AND, CC_NOT} e_carbonrule;
/* CC_extspawn is reserved to future */
typedef enum {CC_copy = 0, CC_move = 1, CC_delete = 2 /*, CC_extspawn*/} e_carbonaction;
typedef struct carbon
{
    e_carbonType   ctype;
    char *         str;   /*  string to compare */
    char *         reason; /*  reason of carbon action */
    ps_area        area;  /*  area to copy messages */
    hs_addr        addr;  /*  from addr to compare */
    char *         areaName;/*  name of area to copy messages */
    int            aexport; /*  export copied msg? */
    int            netMail; /*  do this in netmail, not echomail */
    e_carbonaction move;  /*  copy, move or delete original msg */
    int            extspawn; /*  areaName is name of external program to exec */
    e_carbonrule   rule;  /*  OR|AND|NOT with next carbon expr. */
} s_carbon, *ps_carbon;
typedef struct unpack
{
    int             offset;
    unsigned char * matchCode;
    unsigned char * mask;
    int             codeSize;
    char *          call;
} s_unpack, *ps_unpack;
typedef struct remap
{
    hs_addr oldaddr;
    hs_addr newaddr;
    char *  toname;
} s_remap, *ps_remap;
/* FTS5000 is the standard nodelist format,
   POINTS24 is the German Pointlist format
   POINTS4D is a full 4D pointlist (with 3d "boss" entries) */
typedef enum nodelistFormat {fts5000, points24, points4d} e_nodelistFormat;
typedef struct nodelist
{
    char *       nodelistName;   /* name of unpacked nodelist w/o path */
    char *       diffUpdateStem; /* with pathname */
    char *       fullUpdateStem; /* with pathname */
    unsigned int defaultZone;
    int          delAppliedDiff;
    int          format;
    int          dailynodelist;      /* on or off, deault off */
} s_nodelist, *ps_nodelist;
typedef enum typeDupeCheck
{
    hashDupes,                 /*Base bild from crc32*/
    hashDupesWmsgid,           /*Base bild from crc32+MSGID*/
    textDupes,                 /*Base bild from FromName+ToName+Subj+MSGID*/
    commonDupeBase             /*Common base for all areas bild from crc32*/
} e_typeDupeCheck;
typedef struct savetictype
{
    char * fileAreaNameMask;
    char * pathName;
    int    fileAction;         /* 0 - do nothing */
                               /* 1 - copy file    -  save tic with ticked file */
                               /* 2 - link file  */
    UINT days2save;
} s_savetic, *ps_savetic;
typedef enum linkWithImportLog {lwiNo, lwiYes, lwiKill}      e_linkWithImportLog;
typedef enum kludgeAreaNetmail {kanKill, kanIgnore, kanEcho} e_kludgeAreaNetmail;
/* val: mode to sort echolist by (as-is, by name, by group, by group and name ) */
typedef enum listEchoMode {lemUndef = 0, lemUnsorted, lemName, lemGroup,
                           lemGroupName} e_listEchoMode;
typedef enum filelistType {flDir, flGlobal,
                           flDirList}    e_filelistType;
typedef struct filelist
{
    e_filelistType flType;
    char *         destFile;    /* name of file to be written                  */
    char *         dirHdrTpl;   /* filename of directory header template       */
    char *         dirEntryTpl; /*             directory entry                 */
    char *         dirFtrTpl;   /*             directory footer                */
    char *         globHdrTpl;  /*             global header (flGlobal only)   */
    char *         globFtrTpl;  /*             global footer (flGlobal only)   */
    char *         dirListHdrTpl; /*             dirlist header (flDirlist only) */
    char *         dirListEntryTpl; /*             dirlist entry  (flDirlist only) */
    char *         dirListFtrTpl; /*             dirlist footer (flDirlist only) */
} s_filelist, *ps_filelist;
typedef struct permissions
{
    char * addrMask; /* address mask for permissions settings */
    char * areaMask;    /* area mask */
} s_permissions;
/*  htick announcer */
typedef struct anndef /* announce definition */
{
    char *  annAreaTag;  /* name of area when annouce will be placed    */
    char ** annInclude;  /* array of fileecho names|masks that will be  */
                         /* announced in this annAreaTag                */
    unsigned numbI;      /* number of annInclude masks                  */
    char **  annExclude; /* array of fileecho names|masks that won't be */
                         /* announced in this annAreaTag                */
    unsigned numbE;      /* number of annExclude masks                  */
    char *   annto;      /* field TONAME  : in announce message         */
    char *   annfrom;    /* field FROMNAME: in announce message         */
    char *   annsubj;    /* field SUBJ:     in announce message         */
    char *   annorigin;  /* Origin          in announce message         */
    ps_addr  annaddrto;  /* field ADDRTO:    in announce message         */
    ps_addr  annaddrfrom; /* field ADDRFROM:  in announce message         */
    char *   annmessflags; /* message  flags string                       */
    dword    attributes;
    unsigned annforigin; /* announce address of system who hatched file */
    unsigned annfrfrom;  /* announce address of system which file recived from */
} s_anndef, *ps_anndef;
typedef struct robot
{
    char *         name;         /* name of the robot, "*" for default */
    char *         strA;         /* string for the `area' word (area, filearea) */
    char *         strC;         /* string for the conf token (echoarea, filearea) */
    ps_area *      areas;        /* pointer to pointer to areas list */
    unsigned int * areaCount; /* pointer to areas count */
    s_str_array *  names;        /* (was: areafixNames) */
    char *         fromName;     /* robot from: name (was: areafixFromName) */
    char *         origin;       /* origin line, NULL to disable (was: areafixOrigin) */
    char *         helpFile;     /* help file (was: areafixhelp ) */
    char *         rulesDir;     /* directory where area rules reside */
    char *         newAreaRefuseFile; /* refuse to create areas from this list */
    char *         autoCreateFlag; /* auto-create flag (was: a[af]cFlag) */
    char *         queueFile;    /* queue (was: areafixQueueFile) */
    long           reportsAttr;  /* report's attrs (was: areafixReportsAttr) */
    char *         reportsFlags; /* report's ext attrs (was: areafixReportsFlags) */
    unsigned int   killRequests; /* (was: areafixKillRequests) */
    unsigned int   queryReports; /* (was: areafixQueryReports) */
    unsigned int   msgSize; /* robot's msg max size (was: areafixMsgSize) */
    char *         splitStr;     /* string to split big msgs (was: areafixSplitStr) */
    unsigned int   autoAreaPause;
    unsigned int   forwardRequestTimeout;
    unsigned int   killedRequestTimeout;
    unsigned int   idlePassthruTimeout;
} s_robot, *ps_robot;
typedef struct fidoconfig
{
    unsigned int cfgVersionMajor, cfgVersionMinor;
    char *       name, * location, * sysop, * email;
    unsigned int addrCount;
    ps_addr      addr;
    unsigned int publicCount;
    char **      publicDir;
    unsigned int linkCount;
    ps_link *    links;
    char *       inbound, * outbound, * protInbound, * listInbound, * localInbound,
         * tempInbound;
    char *               logFileDir, * dupeHistoryDir, * nodelistDir, * msgBaseDir;
    char *               magic, * tempOutbound, * ticOutbound;
    char *               tempDir; /* Common temporary files directory */
    char *               fileAreaBaseDir;
    char *               passFileAreaDir; /* Passthrough File Area */
    char *               busyFileDir;
    char *               semaDir, * badFilesDir;
    char *               loglevels, * screenloglevels;
    char *               logDateFormat;
    char *               hptPerlFile;
    char *               advStatisticsFile;
    unsigned int         fileAreaCreatePerms;
    s_area               dupeArea, badArea;
    unsigned int         netMailAreaCount;
    ps_area              netMailAreas;
    unsigned int         echoAreaCount;
    ps_area              echoAreas;
    unsigned int         localAreaCount;
    ps_area              localAreas;
    unsigned int         fileAreaCount;
    ps_area              fileAreas;
    unsigned int         bbsAreaCount;
    ps_bbsarea           bbsAreas;
    s_area               EchoAreaDefault;
    s_area               FileAreaDefault;
    unsigned int         robotCount;
    ps_robot *           robot;
    unsigned int         routeCount;
    ps_route             route;
    unsigned int         groupCount;
    ps_group             group;
    unsigned int         packCount;
    ps_pack              pack;
    unsigned int         unpackCount;
    ps_unpack            unpack;
    char *               intab, * outtab;
    unsigned int         recodeMsgBase;
    char *               echotosslog, * statlog, * importlog, * lockfile;
    unsigned             loguid, loggid, logperm;
    char *               fileAreasLog, * longNameList, * fileNewAreasLog;
    char *               fileArcList, * filePassList, * fileDupeList;
    e_linkWithImportLog  LinkWithImportlog;
    e_kludgeAreaNetmail  kludgeAreaNetmail;
    e_listEchoMode       listEcho;
    unsigned int         carbonCount;
    ps_carbon            carbons;
    unsigned int         carbonAndQuit;
    unsigned int         carbonKeepSb; /*  keep SeenBy's and PATH in carbon area */
    unsigned int         carbonOut; /*  carbon outgoing messages */
    unsigned int         exclPassCC; /*  don't carbon passthough */
    unsigned int         carbonExcludeFwdFrom; /*  don't print " * Forwarded from area" */
    unsigned int         remapCount;
    ps_remap             remaps;
    unsigned int         areafixFromPkt;
    char *               robotsArea;
    char **              PublicGroup;
    unsigned int         numPublicGroup;
    char *               ReportTo;
    int                  reportRequester;
    unsigned int         execonfileCount;
    ps_execonfile        execonfile;
    unsigned int         logEchoToScreen;
    unsigned int         separateBundles;
    unsigned int         dailyBundles;
    unsigned int         defarcmailSize;
    unsigned int         ignoreCapWord;
    unsigned int         noProcessBundles;
    unsigned int         disableTID;
    unsigned int         disablePID;
    unsigned int         disableKludgeRescanned;
    char *               afterUnpack, * beforePack;
    char *               processPkt;
    unsigned int         createDirs;
    unsigned int         longDirNames, splitDirs;
    unsigned int         addDLC, fileSingleDescLine, fileCheckDest;
    e_nameCaseConvertion convertLongNames, convertShortNames;
    unsigned int         fileDescPos, DLCDigits, fileMaxDupeAge;
    unsigned int         fileFileUMask, fileDirUMask;
    unsigned int         originInAnnounce; /* Show origin in announce (for htick) */
    unsigned int         MaxTicLineLength; /*  Maximum length line in TIC (for htick) */
    char *               fileLocalPwd, * fileLDescString;
    char *               fileDescription;
    unsigned int         saveTicCount;
    ps_savetic           saveTic;
    unsigned int         fDescNameCount;
    char **              fileDescNames;
    unsigned int         nodelistCount;
    ps_nodelist          nodelists;
    char *               fidoUserList; /* without path name - is in nodelistDir */
    e_typeDupeCheck      typeDupeBase;
    unsigned int         areasMaxDupeAge;
    ps_link              linkDefaults;
    int                  describeLinkDefaults;
    e_nameCase           createAreasCase;
    e_nameCase           areasFileNameCase;
    char *               tossingExt;

#if defined (__NT__)
    unsigned int setConsoleTitle; /* change console title */
#endif

    ps_addr               addToSeen;
    unsigned int          addToSeenCount;
    ps_addr               ignoreSeen;
    unsigned int          ignoreSeenCount;
    char *                tearline, * origin;
    e_bundleFileNameStyle bundleNameStyle;
    unsigned int          keepTrsMail; /*  Keep Transit Netmail */
    unsigned int          keepTrsFiles; /*  Keep Transit Files */
    unsigned int          createFwdNonPass;
    unsigned int          autoPassive;
    unsigned int          createAddUplink; /* add -def for uplink on autocreate */
    ps_filelist           filelists;
    unsigned int          filelistCount;
    char *                netmailFlag;
    unsigned int          minDiskFreeSpace;
    unsigned int          advisoryLock;
    char *                reqidxDir;      /* directory for herp request index files */
    int                   syslogFacility; /* facility to use when logging via syslog */
    s_permissions *       readOnly; /* temporary storage of address & area masks */
    unsigned int          readOnlyCount;
    s_permissions *       writeOnly;
    unsigned int          writeOnlyCount;
    char *                fileBoxesDir;
    char *                announceSpool;
    char *                notValidFNChars;
    unsigned int          packNetMailOnScan;
    char *                seqDir; /* for msgid generation */
    unsigned long         seqOutrun;
    char **               uuEGrp; /*  uueEchoAreaGroups; */
    unsigned int          numuuEGrp;
    /* for emailpkt */
    char * sendmailcmd; /* send e-mail command line*/
    /*  htick announcer */
    ps_anndef    AnnDefs;
    unsigned int ADCount;
} s_fidoconfig, *ps_fidoconfig;
/* flags for message.recode */
#define REC_HDR 0x0001
#define REC_TXT 0x0002

struct message
{
    /* Address block */
    hs_addr destAddr, origAddr;
/*    UINT16 attributes; */
    hINT32 attributes;
    hUCHAR datetime[22];
    hCHAR  netMail;
    hINT32 textLength;
    hINT32 ctlLength;
    int    recode;
    char * toUserName, * fromUserName, * subjectLine;
    char * text;
    char * ctl;
};

struct pktHeader
{
    /* Address block */
    hs_addr destAddr, origAddr;
    hUINT16 auxNet;
    /* product specific */
    hUCHAR hiProductCode, loProductCode;
    hUCHAR majorProductRev, minorProductRev;
    /* date */
    time_t  pktCreated;
    hUINT16 capabilityWord;
    hUINT32 prodData;
    char    pktPassword[9]; /* password + \0 */
};

typedef struct pktHeader s_pktHeader;
typedef struct message   s_message;
typedef enum
{
    M_NOTDEF = 0, M_HPT, M_HTICK, M_EMAILPKT, M_HPUCODE, M_BSOPACK, M_NLTOOLS, M_MSGED, M_HPTKILL,
    M_HPTSQFIX, M_HPTUTIL, M_HUSKMISC, M_MPOST, M_SQPACK, M_TPARSER, M_OTHER
} e_known_moduls;
typedef struct
{
    e_known_moduls module;
    ps_fidoconfig  config;
    /*  for future usage */
} sApp;


#ifndef _MAKE_DLL_MVC_
extern sApp theApp;
#else
HUSKYEXT sApp theApp;
#endif

HUSKYEXT void SetAppModule(e_known_moduls mod); /*  setup struct sApp */

/* Read fidoconfig from file into memory.
 * Parameter: filename or NULL
 * if NULL: try to find FIDOCONFIG enviroment variable, next use hardcoded path
 * Return NULL and print diagnostic message to stdout if error(s) found.
 */
HUSKYEXT ps_fidoconfig readConfig(const char * fileName);

/* Dispose fidoconfig structure: free memory.
 */
HUSKYEXT void disposeConfig(ps_fidoconfig config);
HUSKYEXT ps_link getLink(s_fidoconfig * config, char * addr);
HUSKYEXT ps_link getLinkForArea(const s_fidoconfig * config, char * addr, s_area * area);
HUSKYEXT ps_link getLinkFromAddr(s_fidoconfig * config, hs_addr aka);
HUSKYEXT ps_addr getAddr(const s_fidoconfig * config, char * addr);
int existAddr(s_fidoconfig * config, hs_addr aka);

/* find echo & local areas in config */
HUSKYEXT ps_area getArea(ps_fidoconfig config, char * areaName);

/* find only echo areas in config */
HUSKYEXT ps_area getEchoArea(ps_fidoconfig config, char * areaName);

/* find netmail areas in config */
HUSKYEXT ps_area getNetMailArea(ps_fidoconfig config, char * areaName);

/* find RobotsArea in config. If not found - selects 1st netmail area */
HUSKYEXT ps_area getRobotsArea(ps_fidoconfig config);

/**
 * This function return 0 if the link is not linked to the area,
 * else it returns 1.
 */
HUSKYEXT int isLinkOfArea(ps_link link, s_area * area);

/**
 * This function return -1 if the link is not linked to the area,
 * else it returns index of link into arealinks array.
 */
HUSKYEXT int isAreaLink(hs_addr link, s_area * area);

/**
 * This function tests if link addr is our aka.
 */
HUSKYEXT int isOurAka(ps_fidoconfig config, hs_addr link);

/**
 * This function dumps the config to a file. The file is in fidoconfig format so,
 * it is possible to change the config in memory and write it to disk.
 * All formatting and comments are removed and the include structure of the config
 * cannot be recreated. So be careful. A file called <fileName> which already exists
 * will be overwritten.
 * 1 if there were problems writing the config
 * 0 else
 */
int dumpConfigToFile(ps_fidoconfig config, char * fileName);

/*  the following functions are for internal use. */
/*  Only use them if you really know what you do. */
HUSKYEXT char * readLine(FILE * f);
HUSKYEXT int parseLine(char * line, ps_fidoconfig config);
int parsePath(char * token, char ** var, char ** alreadyDefined);
HUSKYEXT char * getConfigFileName(void);
HUSKYEXT char * trimLine(char * line);
HUSKYEXT int carbonNames2Addr(s_fidoconfig * config);
HUSKYEXT int init_conf(const char * conf_name);
HUSKYEXT void close_conf(void);
HUSKYEXT void setvar(char * name, char * value);
HUSKYEXT char * getvar(char * name);
void closeall(void);
HUSKYEXT char * configline(void);
HUSKYEXT char * stripComment(char * line);

/* Truncate line at " # " or " #\0" where # is commentchar and any number of spaces before
   commentchar
   Truncate all line if 1st non-space char is commentchar
 */
void checkIncludeLogic(ps_fidoconfig config);
void free_vars(void);
HUSKYEXT const char * getCurConfName(void);
HUSKYEXT long getCurConfPos(void);
HUSKYEXT long get_hcfgPos(void);
HUSKYEXT FILE * get_hcfg(void);
HUSKYEXT const char * cfgEol(void);

/**
 * This method can be used to get a program-specifically config-filename, in the same
 *directories which are searched for fidoconfig.
 * envVar should be set to a string which resembles a environment-variable which should be
 *checked if it includes the fileName.
 * configName is the filename of the config *without* any prefixes.
 * e.g.
 *      getConfigFileNameForProgram("FIDOCONFIG", "config");
 * is the call which is used for fidoconfig
 */
char * getConfigFileNameForProgram(char * envVar, char * configName);
HUSKYEXT ps_area getFileArea(char * areaName);

/*  this function can be used to dump config to stdout or to an already opened file. */
void dumpConfig(ps_fidoconfig config, FILE * f);

/*  return 1 if group found in array of strings, else return 0 */
HUSKYEXT int grpInArray(char * group, char ** strarray, unsigned int len);

/* delete the area from in-core config */
HUSKYEXT void fc_freeEchoArea(s_area * area);

/* returns 1 if link has right to rescan area, else returns 0 */
HUSKYEXT int getLinkRescanAccess(s_area * area, s_link * link);
HUSKYEXT void setLinkAccess(s_fidoconfig * config, s_area * area, s_arealink * arealink);
void processPermissions(s_fidoconfig * config);

/*  define exit codes for non unix systems */
#ifndef _SYSEXITS_H
#define _SYSEXITS_H
#define EX_OK 0                 /* successful termination */
#define EX_USAGE 64             /* command line usage error */
#define EX_NOINPUT 66           /* cannot open input */
#define EX_UNAVAILABLE 69       /* service unavailable */
#define EX_SOFTWARE 70          /* internal software error */
#define EX_CANTCREAT 73         /* can't create (user) output file */
#define EX_IOERR 74             /* input/output error */
#define EX_TEMPFAIL 75          /* temp failure; user is invited to retry */
#define EX_CONFIG 78            /* configuration error */
#endif

#ifdef __cplusplus
}
#endif

#endif // ifndef FIDOCONFIG_H
