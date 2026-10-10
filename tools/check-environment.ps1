$ErrorActionPreference = 'Stop'

$keilBin = $env:KEIL_C51_BIN
if ([string]::IsNullOrWhiteSpace($keilBin)) {
    $keilBin = 'C:\Keil_v5\C51\BIN'
}

$stcIsp = $env:STC_ISP_EXE
if ([string]::IsNullOrWhiteSpace($stcIsp)) {
    $stcIsp = Join-Path $env:USERPROFILE 'Downloads\STC-ISP\STC-ISP-v6.96S.exe'
}

$uv4 = $env:KEIL_UV4_EXE
if ([string]::IsNullOrWhiteSpace($uv4)) {
    $uv4 = 'C:\Keil_v5\UV4\UV4.exe'
}

$failed = $false
foreach ($tool in @('C51.exe', 'BL51.exe', 'OH51.exe')) {
    $path = Join-Path $keilBin $tool
    if (Test-Path $path) {
        Write-Host "[OK] Keil $tool : $path" -ForegroundColor Green
    } else {
        Write-Host "[MISSING] Keil $tool : $path" -ForegroundColor Red
        $failed = $true
    }
}

if (Test-Path $uv4) {
    Write-Host "[OK] Keil uVision : $uv4" -ForegroundColor Green
} else {
    Write-Host "[OPTIONAL] Keil uVision not found : $uv4" -ForegroundColor Yellow
}

if (Test-Path $stcIsp) {
    Write-Host "[OK] STC-ISP : $stcIsp" -ForegroundColor Green
} else {
    Write-Host "[OPTIONAL] STC-ISP not found : $stcIsp" -ForegroundColor Yellow
}

try {
    $ports = Get-CimInstance Win32_SerialPort |
        Select-Object -ExpandProperty DeviceID
    if ($ports) {
        Write-Host "[INFO] Serial ports: $($ports -join ', ')"
    } else {
        Write-Host "[INFO] No serial ports detected. Connect the board before flashing."
    }
} catch {
    Write-Host "[INFO] Could not enumerate serial ports: $($_.Exception.Message)" -ForegroundColor Yellow
}

if ($failed) {
    Write-Host 'Install Keil C51 or set KEIL_C51_BIN to its BIN directory.' -ForegroundColor Yellow
    exit 1
}

Write-Host 'Build environment is ready.' -ForegroundColor Green
