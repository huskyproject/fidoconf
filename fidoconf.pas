unit fidoconf;
{: FIDOCONFIG --- library for fidonet configs

   Copyright (C) 1998-1999

   Matthias Tichy

   Fido:     2:2433/1245 2:2433/1247 2:2432/605.14
   Internet: mtt@tichy.de

   Grimmestr. 12         Buchholzer Weg 4
   33098 Paderborn       40472 Duesseldorf
   Germany               Germany

   This file is part of FIDOCONFIG.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; see file COPYING. If not, write to the Free
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA }

{: NOTE: Unfortunately it turned out that the structures that are passed
         between fidoconfig and any calling programs are in no way align
         controlled, that is, how they are aligned is left to the compiler.
         That means that if you are not using the same compiler for your
         program that was used to compile the libraries, chances are that
         you will have an alignment problem resulting in fields of the
         structures being out of sync, e.g. you are reading something totally
         different from what the library put there.
         Because of this and because I don't want to chase a moving target
         like this, I will stop maintaining smapi.pas and fidoconf.pas.
         That probably means that husky has now become a project that can
         only be used by C programmers, unless somebody else wants
         to go through the trouble.
         One possible solution I can think of, would be having an interface
         layer that converts the randomly aligned structures to other
         structures that are fixed. I might do it, maybe, if I don't find
         anything else to waste my time on. But this is rather unlikely,
         because it has to be done in C and that is definitely not my
         favourite programming language.
         Please leave this comment in here, so nobody else will waste time
         trying to figure out why the data his programs reads is wrong.
         2001-07-22 twm }


interface

{ C default packing is dword }
{ TODO -otwm -ccheck : Check whether this is necessary. }
// {$PACKRECORDS 4}
{$MINENUMSIZE 4} // minimum enum size

const
  LIB_FIDOCONFIG_NAME = 'libfidoconfig.so';

const
  MSGTYPE_PASSTHROUGH = $04;

const
{$IFDEF UNIX}
  PATH_DELIM = '/';
{$ELSE}
  PATH_DELIM = '\';
{$ENDIF}

{ TODO -otwm -ccheck : Check these variables. }
//    var
//       actualLine : PChar;cvar;external;
//     actualLineNr : longint;cvar;external;
//       wasError : char;cvar;external;

function striptwhite(_Str: PChar): PChar;

type
  dword = longword;

type
  PAddress = ^TAddress;
  TAddress = packed record
    zone: dword;
    net: dword;
    node: dword;
    point: dword;
    domain: PChar;
  end;

  PPacker = ^TPacker;
  TPacker = packed record
    packer: PChar;
    call: PChar;
  end;

  PExecOnFile = ^TExecOnFile;
  TExecOnFile = packed record
    filearea: PChar;
    filename: PChar;
    command: PChar;
  end;

  TFlavourEnum = (normal, hold, crash, direct, immediate);
  TForwardEnum = (fOff, fSecure, fOn);
  TEmptyPktPwdEnum = (eOff, eSecure, eOn);
  TPktHeaderDifferEnum = (pdOff, pdOn);
  TNameCaseEnum = (eLower, eUpper);
  TNameCaseConvEnum = (cLower, cUpper, cDontTouch);

  PLink = ^TLink;
  TLink = packed record
    hisAka: TAddress;
    ourAka: PAddress;
    name: PChar;
    defaultPwd: PChar;
    pktPwd: PChar;
    ticPwd: PChar;
    areaFixPwd: PChar;
    fileFixPwd: PChar;
    bbsPwd: PChar;
    sessionPwd: PChar;
    handle: PChar;              // nickname
    email: PChar;
    autoAreaCreate: dword;      // 0 if not allowed for autoareacreate
    autoFileCreate: dword;      // 0 if not allowed for autofilecreate
    AreaFix: dword;             // 0 if not allowed for areafix
    FileFix: dword;             // 0 if not allowed for filefix
    forwardRequests: dword;     // 0 if not allowed forward requests
    forwardFileRequests: dword; // 0 if not allowed forward requests for file areas
    fReqFromUpLink: dword;      // 0 - ignore added unknown area (no area in cfg)
    allowEmptyPktPwd: longint;
    allowPktAddrDiffer: longint;
    forwardPkts: TForwardEnum;
    pktFile: PChar;
    packFile: PChar;
    floFile: PChar;
    bsyFile: PChar;
    packerDef: PPacker;
    echoMailFlavour: TFlavourEnum;
    fileEchoFlavour: TFlavourEnum;
    LinkGrp: PChar;
    AccessGrp: ^PChar;
    NumAccessGrp: dword;
    autoAreaCreateFile: PChar;
    autoFileCreateFile: PChar;
    autoAreaCreateDefaults: PChar;
    autoFileCreateDefaults: PChar;
    forwardRequestFile: PChar;
    forwardAreaPriority: dword;
    RemoteRobotName: PChar;
    forwardFileRequestFile: PChar;
    forwardFilePriority: dword;
    RemoteFileRobotName: PChar;
    msg: pointer;
    noTic: dword;
    Pause: longint;
    autoPause: dword;
    level: dword;
    arcmailSize: dword;
    pktSize: dword;
    export: PChar;
    import: PChar;
    mandatory: PChar;
    optGrp: ^PChar;
    NumOptGrp: dword;
    delNotReceivedTic: dword;         //1 - if file not recieved, then remove TIC
  end;

  TRoutingEnum = (route_zero, host, hub, boss, noroute, nopack, route_extern);

  { if target = NULL use }
  { this }
  PRoute = ^TRoute;
  TRoute = packed record
    flavour: TFlavourEnum;
    enc: char;
    target: PLink;
    routeVia: TRoutingEnum;
    pattern: PChar;
    viaStr: PChar;
  end;

  TDupeCheckEnum = (dcOff, dcMove, dcDel);

  TDupeCheckTypeEnum = (hashDupes,   // Base bild from crc32
                  hashDupesWmsgid,   // Base bild from crc32+MSGID
                        textDupes,   // Base bild from FromName+ToName+Subj+MSGID
                   commonDupeBase);  // Common base for all areas bild from crc32


  PAreaLink = ^TAreaLink;
  TAreaLink = packed record
    link: PLink;
    export: char;
    import: char;
    mandatory: char;
  end;

  PArea = ^TArea;
  TArea = packed record
    areaName: PChar;
    fileName: PChar;
    description: PChar;

    msgbType: longint;  // MSGTYPE_SDM, MSGTYPE_SQUISH, MSGTYPE_JAM or MSGTYPE_PASSTHROUGH

    useAka: PAddress;

    downlinks: ^PAreaLink;
    downlinkCount: dword;

    purge: dword;
    max: dword;
    dupeHistory: dword;
    keepUnread: char;
    killRead: char;

    filler1: array[0..1] of char; // correct alignment

    dupeCheck: TDupeCheckEnum;
    tinySB: char;
    killSB: char;
    hide: char;
    noPause: char;
    mandatory: char;
    DOSFile: char;

    filler2: array[0..1] of char; // correct alignment

    levelread: dword;
    levelwrite: dword;
    dupes: pointer;
    newDupes: pointer;
    imported: dword;

    group: PChar;

    ccoff: longint;            // 1 if carbon copy is not allowed from this area

    uid,
    gid,
    fperm: dword;

    nolink: longint;          // do not reply-link area
    keepsb: longint;          // keep seen-by's and path
    scn: longint;             // 1 if scanned
    nopack: longint;          // do not pack area
  end;

  PFileArea = ^TFileArea;
  TFileArea = packed record
    areaName: PChar;
    pathName: PChar;
    description: PChar;
    sendorig: longint;         // 1 - Send Original
    pass: longint;             // 1 - Passthrough File Area
    noCRC: longint;            // 0 if CRC check should be done on incoming files
    noreplace: longint;        // 1 - no replace files in this filearea
    useAka: PAddress;
    downlinks: ^PAreaLink;
    downlinkCount: dword;
    levelread: dword;
    levelwrite: dword;
    mandatory: char;
    hide: char;
    noPause: char;

    group: PChar;
  end;

  PBbsArea = ^TBbsArea;
  TBbsArea = packed record
    areaName: PChar;
    pathName: PChar;
    description: PChar;
  end;

  TCarbonTypeEnum = (ct_to, ct_from, ct_kludge, ct_subject,
						 ct_msgtext, ct_addr);

  PCarbon = ^TCarbon;
  TCarbon = packed record
    _type: TCarbonTypeEnum;
    _str: PChar;
    reason: PChar;
    area: PArea;
    addr: TAddress;
    areaname: PChar;
    export: longint;
    netMail: longint;
    move: longint;
    extspawn: longint;
  end;

  PUnpacker = ^TUnpacker;
  TUnpacker = packed record
    offset: longint;
    matchCode: ^byte;
    mask: ^byte;
    codeSize: longint;
    call: PChar;
  end;

  PRemap = ^TRemap;
  TRemap = packed record
    oldaddr: TAddress;
    newaddr: TAddress;
    toname: PChar;
  end;

  TNodelistFormatEnum = (fts5000, points24);

  PNodelist = ^TNodelist;
  TNodelist = packed record
    nodelistName: PChar;              // name of unpacked nodelist w/o path
    diffUpdateStem: PChar;            // with pathname
    fullUpdateStem: PChar;            // with pathname
    defaultZone: dword;
    format: dword;
  end;

  PSaveTic = ^TSaveTic;
  TSaveTic = record
    fileAreaNameMask: PChar;
    pathName: PChar;
  end;

  PFidoConfig = ^TFidoConfig;
  TFidoConfig = packed record
    cfgVersionMajor: dword;
    cfgVersionMinor: dword;
    name: PChar;
    location: PChar;
    sysop: PChar;
    addrCount: dword;
    addr: PAddress;
    publicCount: dword;
    publicDir: ^PChar;
    linkCount: dword;
    links: PLink;
    inbound: PChar;
    outbound: PChar;
    protInbound: PChar;
    listInbound: PChar;
    localInbound: PChar;
    tempInbound: PChar;
    logFileDir: PChar;
    dupeHistoryDir: PChar;
    nodelistDir: PChar;
    msgBaseDir: PChar;
    magic: PChar;
    areafixhelp: PChar;
    filefixhelp: PChar;
    tempOutbound: PChar;
    ticoutbound: PChar;
    fileAreaBaseDir: PChar;
    passFileAreaDir: PChar;
    busyFileDir: PChar;
    semaDir: PChar;
    badFilesDir: PChar;
    loglevels: PChar;
    ScreenLogLevels: PChar;
    dupeArea: TArea;
    badArea: TArea;
    netMailAreaCount: dword;
    netMailAreas: PArea;
    echoAreaCount: dword;
    echoAreas: PArea;
    localAreaCount: dword;
    localAreas: PArea;
    fileAreaCount: dword;
    fileAreas: PFileArea;
    bbsAreaCount: dword;
    bbsAreas: PBbsArea;
    routeCount: dword;
    route: PRoute;
    routeFileCount: dword;
    routeFile: PRoute;
    routeMailCount: dword;
    routeMail: PRoute;
    packCount: dword;
    pack: PPacker;
    unpackCount: dword;
    unpack: PUnpacker;
    intab: PChar;
    outtab: PChar;
    echotosslog: PChar;
    importlog: PChar;
    LinkWithImportlog: PChar;
    lockfile: PChar;
    LogUID,
    LogGID,
    LogPerm: dword;
    fileAreasLog: PChar;
    longNameList: PChar;
    fileNewAreasLog: PChar;
    fileArcList: PChar;
    filePassList: PChar;
    fileDupeList: PChar;
    msgidfile: PChar;
    carbonCount: dword;
    carbons: PCarbon;
    carbonAndQuit: dword;
    carbonKeepSb: dword;
    carbonOut: dword;
    includeFiles: ^PChar;
    includeCount: dword;
    remapCount: dword;
    remaps: PRemap;
    areafixFromPkt: dword;
    areafixKillReports: dword;
    areafixKillRequests: dword;
    areafixMsgSize: dword;
    areafixSplitStr: PChar;
    areafixOrigin: PChar;
    PublicGroup: ^PChar;
    numPublicGroup: dword;
    ReportTo: PChar;
    execOnFileCount: dword;
    execOnFile: PExecOnFile;
    logEchoToScreen: dword;
    separateBundles: dword;
    defarcmailSize: dword;
    ignoreCapWord: dword;
    noProcessBundles: dword;
    disableTID: dword;

    afterUnpack: PChar;
    beforePack: PChar;
    processPkt: PChar;
    createDirs: dword;
    longDirNames: dword;
    splitDirs: dword;
    addDLC: dword;
    fileSingleDescLine: dword;
    fileCheckDest: dword;
    filefixKillReports: dword;
    filefixKillRequests: dword;
    convertLongNames: TNameCaseConvEnum;
    convertShortNames: TNameCaseConvEnum;
    fileDescPos: dword;
    DLCDigits: dword;
    fileMaxDupeAge: dword;
    fileFileUMask: dword;
    fileDirUMask: dword;
    oritinInAnnounce: dword;
    MaxTicLineLength: dword;
    fileLocalPwd: PChar;
    fileLDescString: PChar;
    saveTicCount: dword;
    saveTic: PSaveTic;
    nodelistCount: dword;
    nodelists: PNodelist;
    fidoUserList: PChar;

    typeDupeBase: TDupeCheckTypeEnum;
    areasMaxDupeAge: dword;

    linkDefaults: PLink;
    describeLinkDefaults: integer;
    createAreasCase: TNameCaseEnum;
    areasFileNameCase: TNameCaseEnum;
    tossingExt: PChar;
{$ifdef __NT __}
    setConsoleTitle: dword;
{$endif}
    addToSeen: PAddress;
    addToSeenCount: dword;

    tearline: PChar;
    origin: PChar;
  end;

function readConfig(_Filename: PChar): PFidoConfig;

procedure disposeConfig(_Config: PFidoConfig);

function getLink(_Config: TFidoConfig; _Addr: PChar): PLink;

function getLinkFromAddr(_Config: TFidoConfig; _Aka: TAddress): PLink;

function getAddr(_Config: TFidoConfig; _Addr: PChar): PAddress;

function existAddr(_Config: TFidoConfig; _Aka: TAddress): longint;

function getArea(_Config: PFidoConfig; _AreaName: PChar): PArea;

function getNetMailArea(_Config: PFidoConfig; _AreaName: PChar): PArea;

  {: This function returns 0 if the link is not linked to the area,
     else it returns 1. }
function isLinkOfArea(_Link: PLink; _Area: PArea): longint;

  {: This function dumps the config to a file. The file is in fidoconfig format
     so, it is possible to change the config in memory and write it to disk.
     All formatting and comments are removed and the include structure of the
     config cannot be recreated. So be careful. A file called <fileName> which
     already exists will be overwritten.
     1 if there were problems writing the config
     0 else }
function dumpConfigToFile(_Config: PFidoConfig; _FileName: PChar): longint;

(*
  { the following functions are for internal use. }
  { Only use them if you really know what you do. }
  function readLine(F:pFILE):PChar;

  function parseLine(line:PChar; config:PFidoConfig):longint;

  procedure parseConfig(f:pFILE; config:PFidoConfig);
*)
  function getConfigFileName:PChar;
(*
  function trimLine(line: PChar): PChar;
*)


  {: This method can be used to get a program-specifically config-filename,
     in the same directories which are searched for fidoconfig.
     @param EnvVar should be set to a string which resembles a
            environment-variable which should be checked if it includes
            the fileName.
     @param ConfigName is the filename of the config  without  any prefixes.
            e.g. getConfigFileNameForProgram("FIDOCONFIG", "config");
            is the call which is used for fidoconfig }
function getConfigFileNameForProgram(_EnvVar: PChar; _ConfigName: PChar): PChar;

function isLinkOfFileArea(_Link: PLink; _Area: PFileArea): longint;

function getFileArea(_Config: PFidoConfig; _AreaName: PChar): PFileArea;

(*
  { this function can be used to dump config to stdout or to an already opened file. }
  procedure dumpConfig(config:PFidoConfig; f:pFILE);
*)


implementation

function striptwhite(_Str: PChar): PChar; external LIB_FIDOCONFIG_NAME name 'striptwhite';

function readConfig(_Filename: PChar): PFidoConfig; external LIB_FIDOCONFIG_NAME name 'readConfig';
procedure disposeConfig(_Config: PFidoConfig); external LIB_FIDOCONFIG_NAME name 'disposeConfig';

function getLink(_Config: TFidoConfig; _Addr: PChar): PLink; external LIB_FIDOCONFIG_NAME name 'getLink';
function getLinkFromAddr(_Config: TFidoConfig; _Aka: TAddress): PLink; external LIB_FIDOCONFIG_NAME name 'getLinkFromAddr';
function getAddr(_Config: TFidoConfig; _Addr: PChar): PAddress; external LIB_FIDOCONFIG_NAME name 'getAddr';
function existAddr(_Config: TFidoConfig; _Aka: TAddress): longint; external LIB_FIDOCONFIG_NAME name 'existAddr';
function getArea(_Config: PFidoConfig; _AreaName: PChar): PArea; external LIB_FIDOCONFIG_NAME name 'getArea';
function getNetMailArea(_Config: PFidoConfig; _AreaName: PChar): PArea; external LIB_FIDOCONFIG_NAME name 'getNetMailArea';
function isLinkOfArea(_Link: PLink; _Area: PArea): longint; external LIB_FIDOCONFIG_NAME name 'isLinkOfArea';
function dumpConfigToFile(_Config: PFidoConfig; _FileName: PChar): longint; external LIB_FIDOCONFIG_NAME name 'dumpConfigToFile';
(*
  function readLine(F:pFILE):PChar; external LIB_FIDOCONFIG_NAME;
  function parseLine(line:PChar; config:PFidoConfig):longint; external LIB_FIDOCONFIG_NAME;
  procedure parseConfig(f:pFILE; config:PFidoConfig); external LIB_FIDOCONFIG_NAME;
  function trimLine(line: PChar): PChar; external LIB_FIDOCONFIG_NAME;
*)
function getConfigFileName: PChar; external LIB_FIDOCONFIG_NAME name 'getConfigFileName';
function getConfigFileNameForProgram(_EnvVar: PChar; _ConfigName: PChar): PChar; external LIB_FIDOCONFIG_NAME name 'getConfigFileNameForProgram';
function isLinkOfFileArea(_Link: PLink; _Area: PFileArea): longint; external LIB_FIDOCONFIG_NAME name 'isLinkOfFileArea';
function getFileArea(_Config: PFidoConfig; _AreaName: PChar): PFileArea; external LIB_FIDOCONFIG_NAME name 'getFileArea';
(*
  procedure dumpConfig(config:PFidoConfig; f:pFILE); external LIB_FIDOCONFIG_NAME;
*)

end.
