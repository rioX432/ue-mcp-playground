@echo off
REM Headless UE automation test runner (Windows).
REM Requires: UE_ROOT (engine root) and PROJECT (abs path to .uproject) env vars.
REM Usage: set UE_ROOT=... & set PROJECT=... & scripts\run-tests.bat [TestFilter]
setlocal
if "%UE_ROOT%"=="" ( echo set UE_ROOT to your UE 5.8 engine root & exit /b 1 )
if "%PROJECT%"=="" ( echo set PROJECT to the absolute path of MCPPlayground.uproject & exit /b 1 )
set "FILTER=%~1"
if "%FILTER%"=="" set "FILTER=MCPPlayground"

"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%PROJECT%" ^
  -nullrhi -unattended -nopause -nosplash ^
  -ExecCmds="Automation RunTests %FILTER%;quit" ^
  -testexit="Automation Test Queue Empty" ^
  -log -ReportExportPath="%~dp0..\Saved\TestReports"
endlocal
