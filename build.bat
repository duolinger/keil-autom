@echo off
setlocal

if not defined KEIL_C51_BIN set "KEIL_C51_BIN=C:\Keil_v5\C51\BIN"
if not exist "%KEIL_C51_BIN%\C51.exe" (
    echo Keil C51 compiler not found: "%KEIL_C51_BIN%\C51.exe"
    echo Set KEIL_C51_BIN to the folder containing C51.exe, BL51.exe, and OH51.exe.
    exit /b 1
)

pushd "%~dp0"
"%KEIL_C51_BIN%\C51.exe" "QX-MINI51-Time-Marquee.c" "OBJECT(QX-MINI51-Time-Marquee.OBJ)"
if errorlevel 1 goto fail

"%KEIL_C51_BIN%\BL51.exe" "QX-MINI51-Time-Marquee.OBJ" TO "QX-MINI51-TIME-MARQUEE.ABS"
if errorlevel 1 goto fail

"%KEIL_C51_BIN%\OH51.exe" "QX-MINI51-TIME-MARQUEE.ABS" "HEXFILE(QX-MINI51-Time-Marquee.hex)"
if errorlevel 1 goto fail

popd
echo Build successful: QX-MINI51-Time-Marquee.hex
exit /b 0

:fail
set "BUILD_EXIT=%ERRORLEVEL%"
popd
exit /b %BUILD_EXIT%
