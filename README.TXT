
                           FIDOCONFIG LIBRARY
                           ~~~~~~~~~~~~~~~~~~

WHAT IS IT
----------

This library is a core part of HUSKY portable fidonet software which
parsing and manipulating config file.
For details see docs/fidoconfig.*


SUPPORTED COMPILERS
-------------------

The following makefiles can be used to compile this release of FIDOCONFIG:

Makefile       Target library   Compiler
------------   --------------   -------------------------------------------
Makefile       libfidoconfig.*       Any (huskymak.cfg required), use GNU make
makefile.bcd   fconfbcd.lib          Borland C++ 3.1 for DOS
makefile.bco   fconfbco.lib          Borland C++ 1.0 for OS/2
makefile.bcw   fconfbco.lib          Borland C++ 4.0 for Windows
makefile.be    libfidoconfigbe.a     GNU C 2.7.x for BeOS
makefile.bsd   libfidoconfigbsd.a    GNU C 2.9.x for BSD clones
makefile.bsd4  libfidoconfigbsd.a    GNU C 2.9.x for FreeBSD 4.*
makefile.cyg   libfidoconfigcyg.a    GNU C 2.9.3 - 3.2.1 for Cygwin (mingw32
               fidoconf.dll          library and/or dll and binaries)
makefile.djg   fconfdjg.a            GNU C 2.7.x for MS-DOS (DJGPP)
makefile.emo   fconfemo.lib          GNU C 2.7.x for OS/2 (EMX 0.9) with
                                     OMF-Style linkage
makefile.emx   fconfemx.a            GNU C 2.7.x for OS/2 (EMX 0.9) a.out-style
                                     linkage (EMX Runtime)
makefile.hco   fconfhco.lib          MetaWare High C 3.2 for OS/2
makefile.ibo   fconfibo.lib          IBM C/Set++ 2.0 for OS/2
makefile.lnx   fconflnx.a            GNU C 2.7.x for Linux
makefile.qnx   libfidconfigqnx.*     GNU C 2.95.3 for QNX 6.x
makefile.mgw   libfidoconfigmgw.a    Mingw32 for NT
makefile.mvc   fidoconfigmvc.lib     Microsoft Visual C/C++ 6.0
makefile.mvcdll fidoconfigmvc.dll    Microsoft Visual C/C++ 6.0
                                     (shared library produced)
makefile.qcd   fconfqcd.lib          Microsoft QuickC 2.5 for DOS (makefile
                                     requires either the Microsoft Macro
                                     Assembler, MASM, or the Borland Turbo
                                     Assembler, TASM)
makefile.rxw   fconfrxw.lib          GNU C 2.7.x for Windows NT (RSXNT/EMX)
makefile.sun   libfidoconfigsun.a    GNU C 2.7.x for Solaris
makefile.unx   libfidoconfigunix.a   Generic Unix Makefile
makefile.wcd   fconfwcd.lib          WATCOM C/C++ 10.x for 16-bit DOS
makefile.wco   fconfwco.lib          WATCOM C/C++ 10.x for 32-bit OS/2
makefile.wcw   fconfwcw.lib          WATCOM C/C++ 10.x for 32-bit Windows
makefile.wcx   fconfwcx.lib          WATCOM C/C++ 10.x for 32-bit DOS

Note that the Linux, BSD and other unix-like makefiles must be in UNIX text
file format (linefeeds only; no carriage returns). Other must be in DOS text
file format usually (CRLF ends of lines).

The Makefile creates a shared library and depends on GCC, while the
generic Unix Makefile (makefile.unx) should work on any Unix system with
any set of cc, ld and ranlib, and creates a static libarry.
