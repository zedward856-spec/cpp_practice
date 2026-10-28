# ChaCha20 C++ Lab Runner
$compiler = "C:\tools\w64devkit\bin\g++.exe"

if (-not (Test-Path $compiler)) {
    Write-Error "Compiler not found at $compiler"
    exit 1
}

$target = $args[0]
if (-not $target) {
    Write-Host "Usage: .\run.ps1 <1|2|3|all>"
    Write-Host "  1: 01_quarter_round.cpp (RFC 7539 QR Test Vector)"
    Write-Host "  2: 02_avalanche_test.cpp (Hamming distance & diffusion)"
    Write-Host "  3: 03_chacha20_block.cpp (Full 4x4 matrix block permutation)"
    Write-Host "  all: Build and run all three"
    exit 0
}

function BuildAndRun($src, $exe) {
    Write-Host "`n>>> Compiling $src with C++20..." -ForegroundColor Cyan
    & $compiler -std=c++20 -O2 $src -o $exe
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Compilation failed for $src"
        return
    }
    Write-Host ">>> Running ${exe}:`n" -ForegroundColor Green
    & ".\$exe"
}

if ($target -eq "1" -or $target -eq "all") {
    BuildAndRun "01_quarter_round.cpp" "01_quarter_round.exe"
}
if ($target -eq "2" -or $target -eq "all") {
    BuildAndRun "02_avalanche_test.cpp" "02_avalanche_test.exe"
}
if ($target -eq "3" -or $target -eq "all") {
    BuildAndRun "03_chacha20_block.cpp" "03_chacha20_block.exe"
}
