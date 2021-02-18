# FIDOCONFIG LIBRARY
[![Build Status](https://travis-ci.org/huskyproject/fidoconf.svg?branch=master)](https://travis-ci.org/huskyproject/fidoconf)
[![Build status](https://ci.appveyor.com/api/projects/status/rmxqn7rmveb5am4i/branch/master?svg=true)](https://ci.appveyor.com/project/dukelsky/fidoconf/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8c3df1b7e10c4ebfae005e60cf533fd5)](https://www.codacy.com/app/dukelsky/fidoconf?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=huskyproject/fidoconf&amp;utm_campaign=Badge_Grade)

Warning: see "LAST CHANGES" section in this file!

## WHAT IS IT
----------

This library is a core part of HUSKY portable fidonet software which
parsing and manipulating config file.
For details see docs/fidoconfig.*


## SUPPORTED COMPILERS
-------------------

The following makefiles can be used to compile this release of FIDOCONFIG:
```
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
```
Note that the Linux, BSD and other unix-like makefiles must be in UNIX text
file format (linefeeds only; no carriage returns). Other must be in DOS text
file format usually (CRLF ends of lines).

The Makefile creates a shared library and depends on GCC, while the
generic Unix Makefile (makefile.unx) should work on any Unix system with
any set of cc, ld and ranlib, and creates a static libarry.


## LAST CHANGES
------------

This russian text is describing undocumented changes.

Замечания о (недокументированных) новшествах в current.
Подборку сделал Serge Travin, 2:5030/1080.18

─────────────────────────────────────────────────────────────────────

От  : val khokhlov                          2:550/180       02 янв 05  22:28

Тема: husky-current
─────────────────────────────────────────────────────────────────────

  Greetings, All!

        завершена первая (и наибольшая) очередь работ, связанных с переделкой
системы роботов. hpt-current и htick-current собираются и даже, вероятно,
работают. я проверял информационные команды и подписку/отписку - работает. не
проверялись форвард-реквесты, автосоздание эх и работа с очередью.

большая просьба не использовать current на рабочих системах, но по мере
сил помочь с тестированием.

изменения токенов произошли по двум большим направлениям:

1. некоторые глобальные токены перенесены в секции robot

        пример описания секции:
```
robot default\|areafix\|filefix\|<что-то другое>

helpfile <имя файла>
        
robotorigin <ориджин для данного робота> и т.п.
```
последовательность играет роль - default копируется тем роботам, которые
описаны после него (но не до него!). hpt использует имя робота "areafix", htick
\- "filefix". обратите внимание - имя может быть произвольным (на будущее)
        
2. некоторые токены из секций link переименованы и допускают префиксы
это значит, что, к примеру, "areafixecholimit" определяет граничное
число эх, которые может подписать данный линк у areafix'а, а "echolimit" (без
префикса) определяет аналогичное значение и для areafix, и для filefix.
префиксами могут быть предопределенные имена роботов "areafix" и
"filefix". роботы, которые реально определены в конфиге, никак не влияют на это
(т.е., реально можно не обязательно иметь пустую секцию "robot areafix")

вот список измененных токенов:
```
[robot]

areafixfromname                        fromname
areafixhelp                            helpfile
areafixkillrequests                    killrequests
areafixmsgsize                         msgsize
areafixnames                           robotnames
areafixorigin                          robotorigin
areafixqueryreports                    queryreports
areafixqueuefile                       queuefile
areafixreportsattr                     reportsattr
areafixsplitstr                        splitstr
autoareacreateflag                     autocreateflag
autofilecreateflag                     autocreateflag
filefixfromname                        fromname
filefixhelp                            helpfile
filefixkillrequests                    killrequests
filefixnames                           robotnames
filefixreportsattr                     reportsattr

[link]

areafixecholimit                      *echolimit
autoareacreate                        *autocreate
autoareacreatedefaults                *autocreatedefaults
autoareacreatefile                    *autocreatefile
autofilecreate                        *autocreate
autofilecreatedefaults                *autocreatedefaults
autofilecreatefile                    *autocreatefile
denyfwdfile                           *fwddenyfile
denyfwdmask                           *fwddenymask
denyfwdreqaccess                      *
denyuncondfwdreqaccess                *
filefixecholimit                      *echolimit
forwardareapriority                   *fwdpriority
forwardfilepriority                   *fwdpriority
forwardfilerequestfile                *fwdfile
forwardfilerequests                   *fwdmask
forwardrequestfile                    *fwdfile
forwardrequestmask                    *fwdmask
remotefilerobotname                    filefixname
remoterobotname                        areafixname
```
\* здесь означает возможность добавления префикса areafix или filefix к данному
токену для ограничения области действия соответствующим роботом

  Good luck!
             val

-*- System uptime: 17 days, 10 hours, 33 minutes, 28 seconds

 * 0rigin: I noore uu alasseo (2:550/180)

────────────────────────────────────────────────────────────────────

От  : val khokhlov                          2:550/180       02 янв 05  22:42

Тема: htick-current

────────────────────────────────────────────────────────────────────

  Greetings, All!

дополнение к письму "husky-current" по поводу htick:

htick теперь использует единую библиотеку роботов areafix, поэтому
почти все функции работают теперь так же, как в hpt (кроме %resend). реально
это сразу повлияет на формат %list и подобных команд, которые могут сортировать
и группировать области в списке. также, для части команд возможно указание
масок файлэх - при подписке/отписке, %list, %query, %unlinked, %avail (в общем,
смотрите формат этих команд в hpt/misc/areafix.hlp). новые команды в filefix не
добавлялись

подобно hpt, для htick появилась функциональность очереди для
форвард-реквестов. полная аналогия того, что было в hpt. правда, пока без
наличия соответствущюих ключиков запуска qupd/qrep.

  Good luck!
             val

-*- System uptime: 17 days, 10 hours, 47 minutes, 35 seconds

 * 0rigin: I noore uu alasseo (2:550/180)

─────────────────────────────────────────────────────────────────────

От  : val khokhlov                          2:550/180       10 янв 05  19:50

Тема: htick-current

─────────────────────────────────────────────────────────────────────

  Greetings, All!

subj умеет отсылать правила файлэх. точнее, должен - я еще не проверял.
в связи с этим токен rulesdir перенесен в секцию роботов (определяет
имя каталога с правилами для эх и файлэх в соотв. роботе), а к токену norules
может быть добавлен перфикс, который ограничивает его действие одним роботом
(areafix или filefix)

  Good luck!
             val

-*- System uptime: 25 days, 7 hours, 55 minutes, 43 seconds
 * 0rigin: I noore uu alasseo (2:550/180)

─────────────────────────────────────────────────────────────────────

От  : val khokhlov                          2:550/180       22 янв 05  17:02

Кому: Max Chernogor

Тема: husky-current

─────────────────────────────────────────────────────────────────────

  Greetings, Max!

22 Jan 05 14:34, Max Chernogor wrote to val khokhlov:

 MC> ForwardRequestTimeout
 
 MC> IdlePassthruTimeout
 
 MC> KilledRequestTimeout

сорри, забыл указать. эти токены описываются в секции robot

tearline - пока что нет. перенесу.

  Good luck!
             val
  
-*- System uptime: 2 days, 5 hours, 43 minutes, 36 seconds
 * 0rigin: I noore uu alasseo (2:550/180)

 . Sermon - No Place [From Death To Death/1997]
 
... даже если вас съели, то у вас есть два выхода ...

-+- [СПбГЭТУ гр. 0461] --- [death&black metal] --- [Ленинградская область] ---

 + Origin: Like a moving truck (2:5030/2404)
