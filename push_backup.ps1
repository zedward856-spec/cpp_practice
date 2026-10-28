# Quick Git & GitHub Push Script
$gh = "C:\Program Files\GitHub CLI\gh.exe"
$git = "C:\Program Files\Git\cmd\git.exe"

# Check authentication
& $gh auth status
if ($LASTEXITCODE -ne 0) {
    Write-Host "`n[!] You are not logged in to GitHub yet." -ForegroundColor Yellow
    Write-Host "Run the following command to authenticate:" -ForegroundColor Cyan
    Write-Host "  gh auth login --web" -ForegroundColor White
    exit 1
}

Write-Host "Staging and committing any new changes..." -ForegroundColor Cyan
& $git add .
& $git commit -m "Auto backup: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -q
Write-Host "Pushing to https://github.com/zedward856-spec/cpp_practice..." -ForegroundColor Green
& $git push -u origin main
