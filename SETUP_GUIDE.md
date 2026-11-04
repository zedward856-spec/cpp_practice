# Classroom PC Reboot Setup & Study Continuation Guide

Because this classroom PC resets on every reboot, follow this checklist at the start and end of each session.

---

## 🚀 1. Start of Class: 1-Command Environment Restore

1. Open **PowerShell** (`Win + X` $\rightarrow$ **Windows PowerShell** / **Terminal**).
2. Paste and run this single command:
   ```powershell
   irm https://raw.githubusercontent.com/zedward856-spec/cpp_practice/main/setup_env.ps1 | iex
   ```
3. **What this does automatically (~60 seconds)**:
   * Downloads and extracts **w64devkit (GCC 16.2 + GDB)** to `C:\tools\w64devkit`
   * Installs **Git** (`C:\Program Files\Git\cmd\git.exe`) and **GitHub CLI** (`C:\Program Files\GitHub CLI\gh.exe`)
   * Installs **VS Code** + **C/C++** & **Code Runner** extensions
   * Configures Windows `PATH` and your Git username/email (`zedward856-spec`)
   * Clones `https://github.com/zedward856-spec/cpp_practice` to `C:\Users\user\Desktop\cpp_practice` and opens **VS Code**

*(If you want an AI agent like Antigravity to set it up for you next time, just tell it: `"Run irm https://raw.githubusercontent.com/zedward856-spec/cpp_practice/main/setup_env.ps1 | iex"`.)*

---

## 📚 2. Where You Left Off: Week 2 Mini-AES & SPN Study Roadmap

Follow these steps in order next time to finish learning **Substitution-Permutation Networks (SPN)** and **Mini-AES**:

### Step A — Visual Overview (5 mins)
* Double-click **[`MiniAES_SPN.html`](MiniAES_SPN.html)** to open the visual diagram in your browser (or open **[`MiniAES_SPN.canvas`](MiniAES_SPN.canvas)** in Obsidian).
* Compare **Week 1 (ChaCha20 ARX)** vs. **Week 2 (Mini-AES SPN)**:
  * **Confusion**: ChaCha20 uses modular addition (`+`); SPN uses a non-linear **4-bit S-Box lookup table** (`SBOX[nibble]`).
  * **Diffusion**: ChaCha20 uses bit rotation (`rotl`); SPN uses **wire permutation (`P_BOX`)** or **`ShiftRow` + `MixColumn`**.
  * **Key Mixing**: Explicit `state ^= round_key` (`AddRoundKey`) before and after rounds.

### Step B — Hands-On Sandbox: [`mini_aes_spn.cpp`](mini_aes_spn.cpp) (15 mins)
* Open `mini_aes_spn.cpp` in VS Code and press **`F5`** to step through line-by-line in the **GDB debugger**, or run:
  ```powershell
  .\run.ps1 spn
  ```
* **Things to try next time**:
  1. Change `plaintext = 0x4869` to your own 2-letter ASCII word and trace each 4-bit nibble through `sub_nibbles` and `shift_row`.
  2. Flip 1 bit in `plaintext` (e.g. `0x4868`) and see how many output nibbles change after 1 round vs. multiple rounds.

### Step C — Full 4-Round SPN & Avalanche Effect: [`04_spn_cipher.cpp`](04_spn_cipher.cpp) (15 mins)
* Run `.\run.ps1 4` and inspect:
  * How `substitute_nibbles()` applies the 4-bit S-Box to all 4 nibbles.
  * How `permute_bits()` transposes bit $i$ of S-Box $j$ to bit $j$ of S-Box $i$ so 1 changed nibble spreads to all 4 S-Boxes in the next round.
  * Why **Round 4 omits `permute_bits()`** (a final keyless permutation adds zero cryptographic security).

### Step D — 16-Bit Mini-AES over $GF(2^4)$: [`05_mini_aes.cpp`](05_mini_aes.cpp) (20 mins)
* Run `.\run.ps1 5` and trace the 4 Mini-AES operations on the $2 \times 2$ nibble matrix $\begin{bmatrix} s_0 & s_2 \\ s_1 & s_3 \end{bmatrix}$:
  1. **`nibble_sub`**: 4-bit S-Box substitution ($\gamma$)
  2. **`shift_row`**: Swaps bottom row nibbles $s_1 \leftrightarrow s_3$ ($\pi$)
  3. **`mix_column`**: Matrix multiplication in $GF(2^4)$ modulo $x^4 + x + 1$ (`0x13`) ($\theta$)
  4. **`add_round_key` & `expand_key`**: Round key XOR ($\sigma$) using round constants `RCON`.

---

## 💾 3. End of Class: Save & Push Before Reboot

Before logging off or rebooting the PC, open the terminal in `C:\Users\user\Desktop\cpp_practice` and run:

```powershell
.\push_backup.ps1
```

* If you haven't logged into GitHub yet during that session, `push_backup.ps1` will automatically prompt you with a one-time device code at **https://github.com/login/device**, configure Git credentials, commit all your new code, and push to `main`.
