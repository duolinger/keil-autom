@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\check-environment.ps1"
exit /b %ERRORLEVEL%
