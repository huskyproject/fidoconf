@ECHO OFF

REM This is a binary file - it must have carriage return at the end!

REM This batch file is required for building fidoconfig with Borland C TLIB. 
REM It is irrelevant for the end user and for developers with other compilers.

SET LIBNAME=%1
:LOOP
IF ~%2~ == ~~ GOTO ENDE
TLIB %LIBNAME% -+%2
SHIFT
GOTO LOOP

:ENDE

