@echo off
setlocal

set "FIRMWARE=%~dp0QX-MINI51-Time-Marquee.hex"
if not exist "%FIRMWARE%" (
    echo Firmware HEX not found. Run build.bat first.
    exit /b 1
)

if not defined STC_ISP_EXE set "STC_ISP_EXE=C:\Users\%USERNAME%\Downloads\STC-ISP\STC-ISP-v6.96S.exe"
if not exist "%STC_ISP_EXE%" (
    echo STC-ISP not found: "%STC_ISP_EXE%"
    echo Install STC-ISP, or set STC_ISP_EXE to its executable path.
    exit /b 1
)

echo Opening STC-ISP.
echo In STC-ISP, select STC89C52RC/LE52RC and the board's COM port.
echo Open this firmware file, then click Download/Program:
echo "%FIRMWARE%"
start "" "%STC_ISP_EXE%"
