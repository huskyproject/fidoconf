# Microsoft Developer Studio Project File - Name="fidoconfStatic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fidoconfStatic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fidoconfStatic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fidoconfStatic.mak" CFG="fidoconfStatic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fidoconfStatic - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fidoconfStatic - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fidoconfStatic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\nd_r\lib"
# PROP Intermediate_Dir "..\nd_r\obj\fidoconfStatic"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /Ob2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__NT__" /D "WINNT" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\nd_r\lib\fconfmvc.lib"

!ELSEIF  "$(CFG)" == "fidoconfStatic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\nd_d\lib"
# PROP Intermediate_Dir "..\nd_d\obj\fidoconfStatic"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__NT__" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\nd_d\lib\fconfmvc.lib"

!ENDIF 

# Begin Target

# Name "fidoconfStatic - Win32 Release"
# Name "fidoconfStatic - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\adcase.c
# End Source File
# Begin Source File

SOURCE=.\afixcmd.c
# End Source File
# Begin Source File

SOURCE=.\afixcmn.c
# End Source File
# Begin Source File

SOURCE=.\arealist.c
# End Source File
# Begin Source File

SOURCE=.\areatree.c
# End Source File
# Begin Source File

SOURCE=.\cfg.c
# End Source File
# Begin Source File

SOURCE=.\common.c
# End Source File
# Begin Source File

SOURCE=.\crc.c
# End Source File
# Begin Source File

SOURCE=.\dirlayer.c
# End Source File
# Begin Source File

SOURCE=.\fidoconf.c
# End Source File
# Begin Source File

SOURCE=.\findtok.c
# End Source File
# Begin Source File

SOURCE=.\line.c
# End Source File
# Begin Source File

SOURCE=.\log.c
# End Source File
# Begin Source File

SOURCE=.\recode.c
# End Source File
# Begin Source File

SOURCE=.\strsep.c
# End Source File
# Begin Source File

SOURCE=.\temp.c
# End Source File
# Begin Source File

SOURCE=.\tree.c
# End Source File
# Begin Source File

SOURCE=.\xstr.c
# End Source File
# End Group
# Begin Group "headerz"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\adcase.h
# End Source File
# Begin Source File

SOURCE=.\afixcmd.h
# End Source File
# Begin Source File

SOURCE=.\arealist.h
# End Source File
# Begin Source File

SOURCE=.\areatree.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\crc.h
# End Source File
# Begin Source File

SOURCE=.\cvsdate.h
# End Source File
# Begin Source File

SOURCE=.\dirlayer.h
# End Source File
# Begin Source File

SOURCE=.\fc2tor_g.h
# End Source File
# Begin Source File

SOURCE=.\fecfg146.h
# End Source File
# Begin Source File

SOURCE=.\fidoconf.h
# End Source File
# Begin Source File

SOURCE=.\findtok.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\recode.h
# End Source File
# Begin Source File

SOURCE=.\syslogp.h
# End Source File
# Begin Source File

SOURCE=.\temp.h
# End Source File
# Begin Source File

SOURCE=.\tokens.h
# End Source File
# Begin Source File

SOURCE=.\tree.h
# End Source File
# Begin Source File

SOURCE=.\typesize.h
# End Source File
# Begin Source File

SOURCE=.\vixie.h
# End Source File
# Begin Source File

SOURCE=.\xstr.h
# End Source File
# End Group
# End Target
# End Project
