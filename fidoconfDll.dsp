# Microsoft Developer Studio Project File - Name="fidoconfDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=fidoconfDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fidoconfDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fidoconfDll.mak" CFG="fidoconfDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fidoconfDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fidoconfDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fidoconfDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\nd_r\bin"
# PROP Intermediate_Dir "..\nd_r\obj\fconf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I ".." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_WINDOWS" /D "_DLL" /D "__NT__" /D "_CONSOLE" /D "_MAKE_DLL" /D "_FCONF_EXT" /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /D "NDEBUG" /win32
# SUBTRACT MTL /nologo /mktyplib203
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 msvcrt.lib Kernel32.lib smapimvc.lib /nologo /dll /pdb:"..\nd_r\obj/fidoconf.pdb" /machine:I386 /nodefaultlib /out:"..\nd_r\bin/fconfmvc.dll" /implib:"..\nd_r\lib/fconfmvc.lib" /libpath:"..\nd_r\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "fidoconfDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\nd_d\bin"
# PROP Intermediate_Dir "..\nd_d\obj\fconf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /I ".." /D "WIN32" /D "_MBCS" /D "_WINDOWS" /D "_DLL" /D "__NT__" /D "_CONSOLE" /D "_MAKE_DLL" /D "_FCONF_EXT" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\nd_d\lib/fidoconfDll.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msvcrtd.lib Kernel32.lib smapimvc.lib /nologo /dll /profile /debug /machine:I386 /nodefaultlib /out:"..\nd_d\bin/fconfmvc.dll" /implib:"..\nd_d\lib/fconfmvc.lib" /libpath:"..\nd_d\lib"

!ENDIF 

# Begin Target

# Name "fidoconfDll - Win32 Release"
# Name "fidoconfDll - Win32 Debug"
# Begin Group "headerz"

# PROP Default_Filter ""
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

SOURCE=.\dirlayer.h
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

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\vixie.h
# End Source File
# Begin Source File

SOURCE=.\xstr.h
# End Source File
# End Group
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

SOURCE=.\getfree.c
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

SOURCE=.\version.c
# End Source File
# Begin Source File

SOURCE=.\xstr.c
# End Source File
# End Target
# End Project
