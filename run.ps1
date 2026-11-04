# C++20 Cryptography Lab Runner (ChaCha20 ARX + Mini-AES & SPN)
$compiler = "C:\tools\w64devkit\bin\g++.exe"

if (-not (Test-Path $compiler)) {
    Write-Error "Compiler not found at $compiler"
    exit 1
}

$target = $args[0]
if (-not $target) {
    Write-Host "Usage: .\run.ps1 <1|2|3|4|5|spn|chacha|all>" -ForegroundColor Yellow
    Write-Host "  --- Week 1: ChaCha20 & ARX Stream Cipher ---" -ForegroundColor Cyan
    Write-Host "  1      : 01_quarter_round.cpp (RFC 7539 QR Test Vector)"
    Write-Host "  2      : 02_avalanche_test.cpp (Hamming distance & diffusion)"
    Write-Host "  3      : 03_chacha20_block.cpp (Full 4x4 matrix block permutation)"
    Write-Host "  chacha : Run all Week 1 ChaCha20 modules + chacha20.cpp"
    Write-Host ""
    Write-Host "  --- Week 2: Mini-AES & Substitution-Permutation Network (SPN) ---" -ForegroundColor Cyan
    Write-Host "  4      : 04_spn_cipher.cpp (16-bit SPN: S-Box, Bit Permutation, Avalanche)"
    Write-Host "  5      : 05_mini_aes.cpp (16-bit Mini-AES over GF(2^4): NibbleSub, ShiftRow, MixColumn)"
    Write-Host "  spn    : Run mini_aes_spn.cpp + 04_spn_cipher.cpp + 05_mini_aes.cpp"
    Write-Host ""
    Write-Host "  all    : Build and run all modules"
    exit 0
}

function BuildAndRun($src, $exe) {
    Write-Host "`n>>> Compiling $src with C++20..." -ForegroundColor Cyan
    & $compiler -std=c++20 -O2 -Wall $src -o $exe
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Compilation failed for $src"
        return
    }
    Write-Host ">>> Running ${exe}:`n" -ForegroundColor Green
    & ".\$exe"
}

if ($target -eq "1" -or $target -eq "chacha" -or $target -eq "all") {
    BuildAndRun "01_quarter_round.cpp" "01_quarter_round.exe"
}
if ($target -eq "2" -or $target -eq "chacha" -or $target -eq "all") {
    BuildAndRun "02_avalanche_test.cpp" "02_avalanche_test.exe"
}
if ($target -eq "3" -or $target -eq "chacha" -or $target -eq "all") {
    BuildAndRun "03_chacha20_block.cpp" "03_chacha20_block.exe"
}
if ($target -eq "chacha") {
    BuildAndRun "chacha20.cpp" "chacha20.exe"
}
if ($target -eq "spn" -or $target -eq "all") {
    BuildAndRun "mini_aes_spn.cpp" "mini_aes_spn.exe"
}
if ($target -eq "4" -or $target -eq "spn" -or $target -eq "all") {
    BuildAndRun "04_spn_cipher.cpp" "04_spn_cipher.exe"
}
if ($target -eq "5" -or $target -eq "spn" -or $target -eq "all") {
    BuildAndRun "05_mini_aes.cpp" "05_mini_aes.exe"
}
