unit fidoconf;
{
FIDOCONFIG --- library for fidonet configs

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
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
}

interface

{ C default packing is dword }
{$PACKRECORDS 4}

  const
     MSGTYPE_PASSTHROUGH = $04;
{$ifdef UNIX}

  const
     PATH_DELIM = '/';
{$else}

  const
     PATH_DELIM = '\\';
{$endif}

    var
       actualLine : Pchar;cvar;external;
       actualLineNr : longint;cvar;external;
       wasError : char;cvar;external;

  function striptwhite(str:pchar):Pchar;


  type
     s_addr = record
          zone : dword;
          net : dword;
          node : dword;
          point : dword;
          domain : Pchar;
       end;
     Ps_addr = ^s_addr;

     s_pack = record
          packer : Pchar;
          call : Pchar;
       end;
     Ps_pack = ^s_pack;

     e_flavour = (normal,hold,crash,direct,immediate);

     e_forward = (fOff,fSecure,fOn);

     e_emptypktpwd = (eOff,eSecure,eOn);

     s_link = record
          hisAka : s_addr;
          ourAka : Ps_addr;
          name : Pchar;
          defaultPwd : Pchar;
          pktPwd : Pchar;
          ticPwd : Pchar;
          areaFixPwd : Pchar;
          fileFixPwd : Pchar;
          bbsPwd : Pchar;
          sessionPwd : Pchar;
          handle : Pchar;
          autoAreaCreate : longint;
          autoFileCreate : longint;
          AreaFix : longint;
          FileFix : longint;
          forwardRequests : longint;
          fReqFromUpLink : longint;
          allowEmptyPktPwd : longint;
          forwardPkts : e_forward;
          pktFile : Pchar;
          packFile : Pchar;
          floFile : Pchar;
          bsyFile : Pchar;
          packerDef : Ps_pack;
          echoMailFlavour : e_flavour;
          LinkGrp : Pchar;
          AccessGrp : Pchar;
          autoAreaCreateFile : Pchar;
          autoFileCreateFile : Pchar;
          autoAreaCreateDefaults : Pchar;
          autoFileCreateDefaults : Pchar;
          forwardRequestFile : Pchar;
          RemoteRobotName : Pchar;
          msg : pointer;
          Pause : longint;
          autoPause : dword;
          level : dword;
          arcmailSize : dword;
          export : Pchar;
          import : Pchar;
          mandatory : Pchar;
          optGrp : Pchar;
       end;
     Ps_link = ^s_link;

     e_routing = (host := 1,hub,boss,noroute);

  { if target = NULL use }
  { this }
     s_route = record
          flavour : e_flavour;
          enc : char;
          target : Ps_link;
          routeVia : e_routing;
          pattern : Pchar;
       end;
     Ps_route = ^s_route;

     e_dupeCheck = (dcOff,dcMove,dcDel);

     s_arealink = record
          link : Ps_link;
          export : char;
          import : char;
          mandatory : char;
       end;
     Ps_arealink = ^s_arealink;

     s_area = record
          areaName : Pchar;
          fileName : Pchar;
          description : Pchar;
          msgbType : longint;
          useAka : Ps_addr;
          downlinks : ^Ps_arealink;
          downlinkCount : dword;
          purge : dword;
          max : dword;
          dupeSize : dword;
          dupeHistory : dword;
          dupeCheck : e_dupeCheck;
          tinySB : char;
          manual : char;
          hide : char;
          noPause : char;
          mandatory : char;
          DOSFile : char;
          levelread : dword;
          levelwrite : dword;
          dupes : pointer;
          newDupes : pointer;
          imported : char;
          group : char;
          rwgrp : Pchar;
          wgrp : Pchar;
          rgrp : Pchar;
          ccoff : longint;
          keepsb : longint;
          scn : longint;
       end;
     Ps_area = ^s_area;

     s_fileareatype = record
          areaName : Pchar;
          pathName : Pchar;
          description : Pchar;
          pass : longint;
          useAka : Ps_addr;
          downlinks : ^Ps_arealink;
          downlinkCount : dword;
          levelread : dword;
          levelwrite : dword;
          manual : char;
          hide : char;
          noPause : char;
          group : char;
          rwgrp : Pchar;
          wgrp : Pchar;
          rgrp : Pchar;
       end;
     Ps_fileareatype = ^s_fileareatype;

     s_bbsareatype = record
          areaName : Pchar;
          pathName : Pchar;
          description : Pchar;
       end;
     Ps_bbsareatype = ^s_bbsareatype;

     e_carbonType = (c_to,c_from,c_kludge,c_subject,c_msgtext);

     s_carbon = record
          _type : e_carbonType;
          _str : Pchar;
          area : Ps_area;
          export : longint;
          netMail: longint;
          move : longint;
          extspawn : longint;
       end;
     Ps_carbon = ^s_carbon;

     s_unpack = record
          offset : longint;
          matchCode : ^byte;
          mask : ^byte;
          codeSize : longint;
          call : Pchar;
       end;
     Ps_unpack = ^s_unpack;

     s_remap = record
          oldaddr : s_addr;
          newaddr : s_addr;
          toname : Pchar;
       end;
     Ps_remap = ^s_remap;

     s_nodelist = record
          nodelistName	 : Pchar;
          diffUpdateStem : Pchar;
          fullUpdateStem : Pchar;
          defaultZone    : dword;
          format         : dword;
       end;
		     
     Ps_nodelist = ^s_nodelist;

     s_fidoconfig = record
          cfgVersionMajor : dword;
          cfgVersionMinor : dword;
          name : Pchar;
          location : Pchar;
          sysop : Pchar;
          addrCount : dword;
          addr : Ps_addr;
          publicCount : dword;
          publicDir : ^Pchar;
          linkCount : dword;
          links : Ps_link;
          inbound : Pchar;
          outbound : Pchar;
          protInbound : Pchar;
          listInbound : Pchar;
          localInbound : Pchar;
          tempInbound : Pchar;
          logFileDir : Pchar;
          dupeHistoryDir : Pchar;
          nodelistDir : Pchar;
          msgBaseDir : Pchar;
          magic : Pchar;
          areafixhelp : Pchar;
          filefixhelp : Pchar;
          tempOutbound : Pchar;
          ticoutbound : Pchar;
          fileAreaBaseDir : Pchar;
          passFileAreaDir : Pchar;
          semaDir : Pchar;
          badFilesDir : Pchar;
          loglevels : Pchar;
          dupeArea : s_area;
          badArea : s_area;
          netMailAreaCount : dword;
          netMailAreas : Ps_area;
          echoAreaCount : dword;
          echoAreas : Ps_area;
          localAreaCount : dword;
          localAreas : Ps_area;
          fileAreaCount : dword;
          fileAreas : Ps_fileareatype;
          bbsAreaCount : dword;
          bbsAreas : Ps_bbsareatype;
          routeCount : dword;
          route : Ps_route;
          routeFileCount : dword;
          routeFile : Ps_route;
          routeMailCount : dword;
          routeMail : Ps_route;
          packCount : dword;
          pack : Ps_pack;
          unpackCount : dword;
          unpack : Ps_unpack;
          intab : Pchar;
          outtab : Pchar;
          echotosslog : Pchar;
          importlog : Pchar;
          LinkWithImportlog : Pchar;
          lockfile : Pchar;
          fileAreasLog : Pchar;
          longNameList : Pchar;
          fileNewAreasLog : Pchar;
          fileArcList : Pchar;
          filePassList : Pchar;
          fileDupeList : Pchar;
          msgidfile : Pchar;
          carbonCount : dword;
          carbons : Ps_carbon;
          carbonAndQuit : dword;
          carbonKeepSb : dword;
          includeFiles : ^Pchar;
          includeCount : dword;
          remapCount : dword;
          remaps : Ps_remap;
          areafixFromPkt : dword;
          areafixKillReports : dword;
          areafixKillRequests : dword;
          areafixMsgSize : dword;
          areafixSplitStr : Pchar;
          PublicGroup : Pchar;
          ReportTo : Pchar;
          logEchoToScreen : dword;
          separateBundles : dword;
          defarcmailSize : dword;
          afterUnpack : Pchar;
          beforePack : Pchar;
          createDirs : dword;
          longDirNames : dword;
          splitDirs : dword;
          addDLC : dword;
          fileSingleDescLine : dword;
          fileCheckDest : dword;
          filefixKillReports : dword;
          filefixKillRequests : dword;
          fileDescPos : dword;
          DLCDigits : dword;
          fileMaxDupeAge : dword;
          fileFileUMask : dword;
          fileDirUMask : dword;
          fileLocalPwd : Pchar;
          fileLDescString : Pchar;
          nodelistCount : dword;
          nodelists : Ps_nodelist;
          fidoUserList : Pchar;
       end;
     Ps_fidoconfig = ^s_fidoconfig;

  function readConfig:Ps_fidoconfig;

  procedure disposeConfig(config:ps_fidoconfig);

  function getLink(config:s_fidoconfig; addr:pchar):Ps_link;

  function getLinkFromAddr(config:s_fidoconfig; aka:s_addr):Ps_link;

  function getAddr(config:s_fidoconfig; addr:pchar):Ps_addr;

  function existAddr(config:s_fidoconfig; aka:s_addr):longint;

  function getArea(config:ps_fidoconfig; areaName:pchar):Ps_area;

  function getNetMailArea(config:ps_fidoconfig; areaName:pchar):Ps_area;

  { 
     This function return 0 if the link is not linked to the area,
     else it returns 1.
    }
  function isLinkOfArea(link:ps_link; area:ps_area):longint;

  { 
     This function dumps the config to a file. The file is in fidoconfig format so,
     it is possible to change the config in memory and write it to disk.
     All formatting and comments are removed and the include structure of the config
     cannot be recreated. So be careful. A file called <fileName> which already exists
     will be overwritten.
     1 if there were problems writing the config
     0 else
    }
  function dumpConfigToFile(config:ps_fidoconfig; fileName:pchar):longint;

(*
  { the following functions are for internal use. }
  { Only use them if you really know what you do. }
  function readLine(F:pFILE):Pchar;

  function parseLine(line:pchar; config:ps_fidoconfig):longint;

  procedure parseConfig(f:pFILE; config:ps_fidoconfig);

  function getConfigFileName:Pchar;

*)

  function trimLine(line:pchar):Pchar;
  { 
     This method can be used to get a program-specifically config-filename, in the same directories which are searched for fidoconfig.
     envVar should be set to a string which resembles a environment-variable which should be checked if it includes the fileName.
     configName is the filename of the config  without  any prefixes.
     e.g.
          getConfigFileNameForProgram("FIDOCONFIG", "config");
     is the call which is used for fidoconfig
    }
  function getConfigFileNameForProgram(envVar:pchar; configName:pchar):Pchar;

  function isLinkOfFileArea(link:ps_link; area:ps_fileareatype):longint;

  function getFileArea(config:ps_fidoconfig; areaName:pchar):Ps_fileareatype;

(*
  { this function can be used to dump config to stdout or to an already opened file. }
  procedure dumpConfig(config:ps_fidoconfig; f:pFILE);
*)


implementation

  function striptwhite(str:pchar):Pchar; external 'fidoconfig';
  function readConfig:Ps_fidoconfig; external 'fidoconfig';
  procedure disposeConfig(config:ps_fidoconfig); external 'fidoconfig';
  function getLink(config:s_fidoconfig; addr:pchar):Ps_link; external 'fidoconfig';
  function getLinkFromAddr(config:s_fidoconfig; aka:s_addr):Ps_link; external 'fidoconfig';
  function getAddr(config:s_fidoconfig; addr:pchar):Ps_addr; external 'fidoconfig';
  function existAddr(config:s_fidoconfig; aka:s_addr):longint; external 'fidoconfig';
  function getArea(config:ps_fidoconfig; areaName:pchar):Ps_area; external 'fidoconfig';
  function getNetMailArea(config:ps_fidoconfig; areaName:pchar):Ps_area; external 'fidoconfig';
  function isLinkOfArea(link:ps_link; area:ps_area):longint; external 'fidoconfig';
  function dumpConfigToFile(config:ps_fidoconfig; fileName:pchar):longint; external 'fidoconfig';
(*
  function readLine(F:pFILE):Pchar; external 'fidoconfig';
  function parseLine(line:pchar; config:ps_fidoconfig):longint; external 'fidoconfig';
  procedure parseConfig(f:pFILE; config:ps_fidoconfig); external 'fidoconfig';
*)
  function getConfigFileName:Pchar; external 'fidoconfig';
  function trimLine(line:pchar):Pchar; external 'fidoconfig';
  function getConfigFileNameForProgram(envVar:pchar; configName:pchar):Pchar; external 'fidoconfig';
  function isLinkOfFileArea(link:ps_link; area:ps_fileareatype):longint; external 'fidoconfig';
  function getFileArea(config:ps_fidoconfig; areaName:pchar):Ps_fileareatype; external 'fidoconfig';
(*
  procedure dumpConfig(config:ps_fidoconfig; f:pFILE); external 'fidoconfig';
*)

end.
