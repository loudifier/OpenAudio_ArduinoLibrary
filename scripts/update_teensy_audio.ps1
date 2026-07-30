param(
    [string]$teensyDir = "$env:LOCALAPPDATA\Arduino15\packages\teensy\hardware\avr\1.62.0",
    [string]$repoRoot = "$PSScriptRoot\..",
    [switch]$noCacheClear
)

$ErrorActionPreference = "Stop"

$coreDir = "$teensyDir\cores\teensy4"
$configDir = $teensyDir

if (-not (Test-Path -LiteralPath $teensyDir)) {
    Write-Error "Teensy installation not found at: $teensyDir"
    exit 1
}

Write-Host "Updating Teensy audio files from: $repoRoot"

# 1. Copy patched core files (including subdirectories like util/)
Write-Host "`nCopying core files..."
$patchDir = (Resolve-Path "$repoRoot\patched_teensy_core").Path
if (Test-Path -LiteralPath $patchDir) {
    $coreFiles = Get-ChildItem -LiteralPath $patchDir -Recurse -File
    $prefixLen = $patchDir.Length + 1
    foreach ($f in $coreFiles) {
        $rel = $f.FullName.Substring($prefixLen)
        if ($rel -notmatch "^config\\") {
            $dest = Join-Path $coreDir $rel
            $parent = Split-Path -Parent $dest
            if (-not (Test-Path -LiteralPath $parent)) {
                New-Item -ItemType Directory -Path $parent -Force | Out-Null
            }
            Copy-Item -LiteralPath $f.FullName -Destination $dest -Force
            Write-Host "  $rel"
        }
    }
}

# 2. Copy config files
Write-Host "`nCopying config files..."
$configPatchDir = "$patchDir\config"
if (Test-Path -LiteralPath $configPatchDir) {
    $configFiles = Get-ChildItem -LiteralPath $configPatchDir -File
    $platformCopied = $false
    foreach ($f in $configFiles) {
        $name = $f.Name
        if ($name -eq "BM-platform.txt") {
            $dest = "$configDir\platform.txt"
            Copy-Item -LiteralPath $f.FullName -Destination $dest -Force
            Write-Host "  $name -> platform.txt (Boards Manager)"
            $platformCopied = $true
        } elseif ($name -eq "TD-platform.txt") {
            if (-not $platformCopied) {
                $dest = "$configDir\platform.txt"
                Copy-Item -LiteralPath $f.FullName -Destination $dest -Force
                Write-Host "  $name -> platform.txt (Teensyduino Installer)"
            }
        } else {
            $dest = "$configDir\$name"
            Copy-Item -LiteralPath $f.FullName -Destination $dest -Force
            Write-Host "  $name -> $([System.IO.Path]::GetFileName($dest))"
        }
    }
}

# 3. Append menu entries to boards.txt (IDE 2.x may not read boards.local.txt)
$boardsFile = "$configDir\boards.txt"
$sentinel = "# OpenAudio menu additions"
$menuContent = Get-Content "$configPatchDir\boards.local.txt" -Raw
if ((Get-Content $boardsFile -Raw) -notmatch [regex]::Escape($sentinel)) {
    Write-Host "`nAppending OpenAudio menus to boards.txt..."
    Add-Content -Path $boardsFile -Value "`r`n`r`n$sentinel`r`n$menuContent"
    Write-Host "  Done."
} else {
    Write-Host "`nOpenAudio menus already present in boards.txt."
}

# 4. Kill stale backend processes and clear caches
if (-not $noCacheClear) {
    # Kill running arduino-cli/language-server so they re-read boards.txt
    Write-Host "`nStopping stale IDE backend processes..."
    Get-Process -Name "arduino-cli","arduino-language-server" -ErrorAction SilentlyContinue | Stop-Process -Force
    Write-Host "  Done."

    # Arduino IDE 2.x cache
    $ideCache = "$env:APPDATA\arduino-ide"
    if (Test-Path -LiteralPath $ideCache) {
        Write-Host "Clearing Arduino IDE 2.x cache: $ideCache"
        Remove-Item -LiteralPath "$ideCache\*" -Recurse -Force -ErrorAction SilentlyContinue
    }
    Write-Host "  Cache cleared."
}

Write-Host "`nUpdate complete."
exit 0
