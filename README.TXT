SUPPORTED COMPILERS

The following makefiles can be used to compile this release of FIDOCONFIG:

Makefile       Target library   Compiler
------------   --------------   -------------------------------------------
Makefile       libfidoconfig.*       Any (huskymak.cfg required)
makefile.bcd   fconfbcd.lib          Borland C++ for DOS 3.1
makefile.bco   fconfbco.lib          Borland C++ for OS/2 1.0
makefile.bcw   fconfbco.lib          Borland C++ for Windows 4.0
makefile.be    libfidoconfigbe.a     GNU C for BeOS 2.7.x
makefile.bsd   libfidoconfigbsd.a    GNU C for FreeBSD 2.7.x
makefile.bsd4  libfidoconfigbsd.a    GNU C for FreeBSD 2.7.x, use make
                                     (FreeBSD 4.*)
makefile.cyg   libfidoconfigcyg.a    GNU C for Cygwin 2.7.x
makefile.djg   fconfdjg.a            GNU C for MS-DOS 2.7.x (DJGPP)
makefile.emo   fconfemo.lib          GNU C for OS/2 2.7.x (EMX 0.9) with
                                     OMF-Style linkage
makefile.emx   fconfemx.a            GNU C for OS/2 2.7.x (EMX 0.9) a.out-style
                                     linkage (EMX Runtime)
makefile.hco   fconfhco.lib          MetaWare High C for OS/2 3.2
makefile.ibo   fconfibo.lib          IBM C/Set++ for OS/2 2.0
makefile.lnx   fconflnx.a            GNU C for Linux 2.7.x
makefile.mgw   libfidoconfigmgw.a    Mingw32 for NT
makefile.mvc   fidoconfigmvc.lib     Microsoft Visual C/C++ 6.0
makefile.mvcdll fidoconfigmvc.dll    Microsoft Visual C/C++ 6.0
                                     (shared library produced)
makefile.qcd   fconfqcd.lib          Microsoft QuickC for DOS 2.5 (makefile
                                     requires either the Microsoft Macro
                                     Assembler, MASM, or the Borland Turbo
                                     Assembler, TASM)
makefile.rxw   fconfrxw.lib          GNU C for Windows NT 2.7.x (RSXNT/EMX)
makefile.sun   libfidoconfigsun.a    GNU C for Solaris 2.7.x
makefile.unx   libfidoconfigunix.a   Generic Unix Makefile
makefile.wcd   fconfwcd.lib          WATCOM C/C++ for 16-bit DOS 10.x
makefile.wco   fconfwco.lib          WATCOM C/C++ for 32-bit OS/2 10.x
makefile.wcw   fconfwcw.lib          WATCOM C/C++ for 32-bit Windows NT 10.x
makefile.wcx   fconfwcx.lib          WATCOM C/C++ for 32-bit DOS 10.x

Use GNU version of make (gmake on many platforms) if don't specified other.
                                    
Note that the Linux and FreeBSD makefiles must be in UNIX text file
format (linefeeds only; no carriage returns).

The Linux Makefile creates a shared library and depends on GCC, while the
generic Unix Makefile (makefile.unx) should work on any Unix system with
any set of cc, ld and ranlib, and creates a static libarry.
