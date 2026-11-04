# Quick Git & GitHub Push Script
$gh = "C:\Program Files\GitHub CLI\gh.exe"
$git = "C:\Program Files\Git\cmd\git.exe"
$env:PATH = "C:\Program Files\Git\cmd;C:\Program Files\GitHub CLI;$env:PATH"

# Check authentication
& $gh auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "`n[!] You are not logged in to GitHub yet. Starting device login..." -ForegroundColor Yellow
    & $gh auth login --web -h github.com -p https
    if ($LASTEXITCODE -ne 0) {
        exit 1
    }
}

# Ensure Git uses GitHub CLI credentials and has identity configured
& $gh auth setup-git
& $git config --global user.name "zedward856-spec"
& $git config --global user.email "zedward856-spec@users.noreply.github.com"

Write-Host "Staging and committing any new changes..." -ForegroundColor Cyan
& $git add .
& $git commit -m "Auto backup: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -q
Write-Host "Pushing to https://github.com/zedward856-spec/cpp_practice..." -ForegroundColor Green
& $git push -u origin main
