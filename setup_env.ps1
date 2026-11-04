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
    Write-Host "`n[1/5] Installing w64devkit (GCC + GDB) to C:\tools\w64devkit..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path "C:\tools" | Out-Null
    $release = Invoke-RestMethod -Uri "https://api.github.com/repos/skeeto/w64devkit/releases/latest"
    $asset = $release.assets | Where-Object { $_.name -like "w64devkit-x64-*.7z.exe" } | Select-Object -First 1
    $installer = "$env:TEMP\$($asset.name)"
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $installer -UseBasicParsing
    & $installer -o"C:\tools" -y | Out-Null
    Remove-Item $installer -Force -ErrorAction SilentlyContinue
    Write-Host "      w64devkit installed: $($release.tag_name)" -ForegroundColor Green
} else {
    Write-Host "`n[1/5] w64devkit already present at C:\tools\w64devkit." -ForegroundColor Green
}

# 2. Install Git & GitHub CLI via winget
if (-not (Test-Path "C:\Program Files\Git\cmd\git.exe")) {
    Write-Host "`n[2/5] Installing Git via winget..." -ForegroundColor Yellow
    winget install --id Git.Git -e --source winget --accept-package-agreements --accept-source-agreements --silent
} else {
    Write-Host "`n[2/5] Git already installed." -ForegroundColor Green
}

if (-not (Test-Path "C:\Program Files\GitHub CLI\gh.exe")) {
    Write-Host "`n[3/5] Installing GitHub CLI (gh) via winget..." -ForegroundColor Yellow
    winget install --id GitHub.cli -e --source winget --accept-package-agreements --accept-source-agreements --silent
} else {
    Write-Host "`n[3/5] GitHub CLI already installed." -ForegroundColor Green
}

# 3. Install Visual Studio Code & C++ Extensions
$vscodeCmd = "$env:LOCALAPPDATA\Programs\Microsoft VS Code\bin\code.cmd"
if (-not (Test-Path $vscodeCmd)) {
    Write-Host "`n[4/5] Installing Visual Studio Code via winget..." -ForegroundColor Yellow
    winget install --id Microsoft.VisualStudioCode -e --source winget --accept-package-agreements --accept-source-agreements --silent
} else {
    Write-Host "`n[4/5] Visual Studio Code already installed." -ForegroundColor Green
}

if (Test-Path $vscodeCmd) {
    Write-Host "      Installing C/C++ and Code Runner extensions..." -ForegroundColor Cyan
    & $vscodeCmd --install-extension ms-vscode.cpptools --force | Out-Null
    & $vscodeCmd --install-extension formulahendry.code-runner --force | Out-Null
}

# 4. Update PATH for current session & user profile
$toolPaths = @(
    "C:\tools\w64devkit\bin",
    "C:\Program Files\Git\cmd",
    "C:\Program Files\GitHub CLI",
    "$env:LOCALAPPDATA\Programs\Microsoft VS Code\bin"
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

# 5. Clone repository to Desktop if not already present & launch VS Code
$repoDir = "$env:USERPROFILE\Desktop\cpp_practice"
if (-not (Test-Path $repoDir)) {
    Write-Host "`n[5/5] Cloning cpp_practice to $repoDir..." -ForegroundColor Yellow
    & "C:\Program Files\Git\cmd\git.exe" clone "https://github.com/zedward856-spec/cpp_practice.git" $repoDir
} else {
    Write-Host "`n[5/5] Repository already exists at $repoDir." -ForegroundColor Green
}

Set-Location $repoDir
if (Test-Path $vscodeCmd) {
    Write-Host "Launching VS Code in $repoDir..." -ForegroundColor Green
    & $vscodeCmd $repoDir
}

Write-Host "`n[READY] Environment setup complete in $repoDir!" -ForegroundColor Green
Write-Host "  - Test labs : .\run.ps1 all" -ForegroundColor Cyan
Write-Host "  - Save/Push : .\push_backup.ps1" -ForegroundColor Cyan
