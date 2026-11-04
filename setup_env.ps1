# ============================================================================
# One-Click Classroom PC Environment Bootstrap (Resets Every Reboot)
# Usage (from a fresh reboot in PowerShell):
#   irm https://raw.githubusercontent.com/zedward856-spec/cpp_practice/main/setup_env.ps1 | iex
# ============================================================================

$ErrorActionPreference = "Stop"

Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "  Bootstrapping C++20 Cryptography Dev Environment (cpp_practice)   " -ForegroundColor Cyan
Write-Host "====================================================================" -ForegroundColor Cyan

# 1. Install w64devkit (GCC 16.2 + GDB) to C:\tools\w64devkit
if (-not (Test-Path "C:\tools\w64devkit\bin\g++.exe")) {
    Write-Host "`n[1/4] Installing w64devkit (GCC + GDB) to C:\tools\w64devkit..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path "C:\tools" | Out-Null
    $release = Invoke-RestMethod -Uri "https://api.github.com/repos/skeeto/w64devkit/releases/latest"
    $asset = $release.assets | Where-Object { $_.name -like "w64devkit-x64-*.7z.exe" } | Select-Object -First 1
    $installer = "$env:TEMP\$($asset.name)"
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $installer -UseBasicParsing
    & $installer -o"C:\tools" -y | Out-Null
    Remove-Item $installer -Force -ErrorAction SilentlyContinue
    Write-Host "      w64devkit installed: $($release.tag_name)" -ForegroundColor Green
} else {
    Write-Host "`n[1/4] w64devkit already present at C:\tools\w64devkit." -ForegroundColor Green
}

# 2. Install Git & GitHub CLI via winget
if (-not (Test-Path "C:\Program Files\Git\cmd\git.exe")) {
    Write-Host "`n[2/4] Installing Git via winget..." -ForegroundColor Yellow
    winget install --id Git.Git -e --source winget --accept-package-agreements --accept-source-agreements --silent
} else {
    Write-Host "`n[2/4] Git already installed." -ForegroundColor Green
}

if (-not (Test-Path "C:\Program Files\GitHub CLI\gh.exe")) {
    Write-Host "`n[3/4] Installing GitHub CLI (gh) via winget..." -ForegroundColor Yellow
    winget install --id GitHub.cli -e --source winget --accept-package-agreements --accept-source-agreements --silent
} else {
    Write-Host "`n[3/4] GitHub CLI already installed." -ForegroundColor Green
}

# 3. Update PATH for current session & user profile
$toolPaths = @(
    "C:\tools\w64devkit\bin",
    "C:\Program Files\Git\cmd",
    "C:\Program Files\GitHub CLI"
)
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
foreach ($tp in $toolPaths) {
    if ($env:PATH -notlike "*$tp*") { $env:PATH = "$tp;$env:PATH" }
    if ($userPath -notlike "*$tp*") { $userPath = "$userPath;$tp" }
}
[Environment]::SetEnvironmentVariable("Path", $userPath.Trim(';'), "User")

# Configure default Git identity so commits/pushes work seamlessly after a reboot
& "C:\Program Files\Git\cmd\git.exe" config --global user.name "zedward856-spec"
& "C:\Program Files\Git\cmd\git.exe" config --global user.email "zedward856-spec@users.noreply.github.com"

# 4. Clone repository to Desktop if not already present
$repoDir = "$env:USERPROFILE\Desktop\cpp_practice"
if (-not (Test-Path $repoDir)) {
    Write-Host "`n[4/4] Cloning cpp_practice to $repoDir..." -ForegroundColor Yellow
    & "C:\Program Files\Git\cmd\git.exe" clone "https://github.com/zedward856-spec/cpp_practice.git" $repoDir
} else {
    Write-Host "`n[4/4] Repository already exists at $repoDir." -ForegroundColor Green
}

Set-Location $repoDir
Write-Host "`n[READY] Environment setup complete in $repoDir!" -ForegroundColor Green
Write-Host "  - Test labs : .\run.ps1 all" -ForegroundColor Cyan
Write-Host "  - Login GH  : gh auth login --web" -ForegroundColor Cyan
Write-Host "  - Push work : .\push_backup.ps1" -ForegroundColor Cyan
