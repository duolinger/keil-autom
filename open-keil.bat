@echo off
setlocal

if not defined KEIL_UV4_EXE set "KEIL_UV4_EXE=C:\Keil_v5\UV4\UV4.exe"
if not exist "%KEIL_UV4_EXE%" (
    echo Keil uVision 4 not found: "%KEIL_UV4_EXE%"
    echo Set KEIL_UV4_EXE to the full path of UV4.exe.
    exit /b 1
)

start "" "%KEIL_UV4_EXE%" "%~dp0QX-MINI51-Time-Marquee.uvproj"
