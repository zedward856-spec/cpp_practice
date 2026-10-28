# C++ Cryptography Practice: Stream Permutations & ChaCha20

Practice repository for learning stream ciphers, bitwise ARX permutations, and ChaCha20 quarter-round mechanics.

---

## 🌟 Student Implementation (by zedward856-spec)

* **[`chacha20.cpp`](chacha20.cpp)**:
  * **Author**: **zedward856-spec**
  * **Description**: Custom 4-letter stream encryption & decryption engine.
  * **Details**: Encrypts the ASCII word `"Lucy"` (`0x4C756379`) using a 32-bit secret key (`"Keys"`, `0x4B657973`) via bitwise XOR (`^=`), demonstrating that encryption and decryption are symmetric and fully reversible in one clock cycle.

---

## 📚 Reference Implementations & Visualizations

* **[`01_quarter_round.cpp`](01_quarter_round.cpp)**: ChaCha20 Quarter-Round engine verified against RFC 7539 / RFC 8439 official test vectors.
* **[`02_avalanche_test.cpp`](02_avalanche_test.cpp)**: Avalanche effect & bit diffusion measurement (Hamming distance across 128-bit state).
* **[`03_chacha20_block.cpp`](03_chacha20_block.cpp)**: Full 4x4 ChaCha state matrix permutation (Column Rounds, Diagonal Rounds, and Davies-Meyer feed-forward addition).
* **[`ChaCha20_QuarterRound.canvas`](ChaCha20_QuarterRound.canvas)**: Interactive Obsidian Canvas flow diagram mapping inputs, ARX stages, and rotation constants.
* **[`ChaCha20_QuarterRound.html`](ChaCha20_QuarterRound.html)**: Standalone browser visualization.
* **[`run.ps1`](run.ps1)**: Automation script to compile and execute any module using modern C++20.
