@ECHO OFF

REM This is a binary file - it must have carriage return at the end!

REM This batch file is required for building fidoconfig with Borland C TLIB.
REM It is irrelevant for the end user and for developers with other compilers.

SET LIBPROG=%1
SET LIBNAME=%2
:LOOP
IF ~%3~ == ~~ GOTO ENDE
echo %LIBPROG% %LIBNAME% -+%3 ,,
%LIBPROG% %LIBNAME% -+%3 ,,
SHIFT
GOTO LOOP

:ENDE

