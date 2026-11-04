# C++ Cryptography Practice: ChaCha20 (ARX) & Mini-AES (SPN)

Practice repository for learning modern symmetric cryptography in C++20: stream ciphers, **ARX** permutations (**ChaCha20**), **Substitution-Permutation Networks (SPN)**, and **Mini-AES** over $GF(2^4)$.

---

## 💻 Week 2: Mini-AES & Substitution-Permutation Networks (SPN)

* **[`mini_aes_spn.cpp`](mini_aes_spn.cpp)** *(Student Playground)*:
  * **Author**: **zedward856-spec**
  * **Description**: Hands-on 16-bit single-round SPN / Mini-AES playground (`AddRoundKey` $\rightarrow$ 4-bit `S-Box` $\rightarrow$ `ShiftRow` nibble permutation) with symmetric decryption.
* **[`04_spn_cipher.cpp`](04_spn_cipher.cpp)**:
  * Full 4-round, 16-bit **Substitution-Permutation Network (SPN)** block cipher (Heys' architecture) demonstrating **Confusion** (4-bit non-linear S-Box), **Diffusion** (16-bit wire-transpose P-Box), why the final round omits the P-Box, full decryption, and 1-bit avalanche testing.
* **[`05_mini_aes.cpp`](05_mini_aes.cpp)**:
  * Complete **16-bit Mini-AES** implementation (Phan's specification) operating on a $2 \times 2$ nibble matrix over Galois Field $GF(2^4)$ modulo $x^4 + x + 1$ (`0x13`): `NibbleSub` ($\gamma$), `ShiftRow` ($\pi$), `MixColumn` ($\theta$), and `RCON` Key Schedule verified against standard test vectors (`P = 0x9C63`, `K = 0xC3F0` $\rightarrow$ `C = 0x72C6`).
* **[`MiniAES_SPN.canvas`](MiniAES_SPN.canvas)** & **[`MiniAES_SPN.html`](MiniAES_SPN.html)**:
  * Interactive Obsidian Canvas & standalone browser visual flow diagrams comparing SPN vs. ARX and tracing every nibble through Mini-AES.

---

## 💻 Week 1: Stream Permutations & ChaCha20 (ARX)

* **[`chacha20.cpp`](chacha20.cpp)** *(Student Implementation)*:
  * **Author**: **zedward856-spec**
  * **Description**: Custom 4-letter stream encryption & decryption engine encrypting `"Lucy"` (`0x4C756379`) using a 32-bit secret key (`"Keys"`, `0x4B657973`) via bitwise XOR (`^=`).
* **[`01_quarter_round.cpp`](01_quarter_round.cpp)**: ChaCha20 Quarter-Round engine verified against RFC 7539 / RFC 8439 official test vectors.
* **[`02_avalanche_test.cpp`](02_avalanche_test.cpp)**: Avalanche effect & bit diffusion measurement (Hamming distance across 128-bit state).
* **[`03_chacha20_block.cpp`](03_chacha20_block.cpp)**: Full $4 \times 4$ ChaCha state matrix permutation (Column Rounds, Diagonal Rounds, and Davies-Meyer feed-forward addition).
* **[`ChaCha20_QuarterRound.canvas`](ChaCha20_QuarterRound.canvas)** & **[`ChaCha20_QuarterRound.html`](ChaCha20_QuarterRound.html)**: Flow diagrams mapping inputs, ARX stages, and rotation constants.

---

## 🛠️ Quick Start & Classroom PC Reboot Recovery

Because this machine resets on every reboot, run this **single PowerShell command** after logging in to automatically install **w64devkit (GCC 16.2 + GDB)**, **Git**, **GitHub CLI**, **VS Code + C++ extensions**, configure `PATH`, clone this repository to your Desktop, and open VS Code in the repo:

```powershell
irm https://raw.githubusercontent.com/zedward856-spec/cpp_practice/main/setup_env.ps1 | iex
```

*(Or, if you already cloned the repo, run `.\setup_env.ps1`.)*

### Run & Backup Commands
* **Run Lab Modules**:
  ```powershell
  .\run.ps1 spn          # Run all Week 2 Mini-AES & SPN labs
  .\run.ps1 chacha       # Run all Week 1 ChaCha20 labs
  .\run.ps1 all          # Build and run every module
  ```
* **Save & Push Work Before Reboot**:
  ```powershell
  .\push_backup.ps1      # Authenticates (if needed), commits, and pushes to GitHub
  ```
* **Build / Debug in Editor**:
  * Press **`Ctrl+Shift+B`** to compile the active `.cpp` file with GCC 16.2 (`-std=c++20 -g3 -Wall -Wextra`).
  * Press **`F5`** to compile and launch the interactive **GDB step-by-step debugger** on the currently open `.cpp` file.
