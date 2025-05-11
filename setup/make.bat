@echo off
REM compile a single setup script
REM
REM 2012-12-18 TTHA Initial Version
REM
if [%1]==[] goto usage
SET ISCC="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
SET BASE="D:\tt\Documents\t2ft\000_Projekte\Eigene\Programme\t2ftCalendar\setup"
echo ->log.txt
echo *** %1 ********************************************************************************************************************************************** >>log.txt
%ISCC% /sstandard="%BASE%\signFile.bat $f" "%BASE%\%1" >>log.txt
exit /B 0
:usage
@echo Usage: %0 ^<full setup script name^>
exit /B 1

