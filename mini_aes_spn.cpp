// ============================================================================
// Author: zedward856-spec
// File: mini_aes_spn.cpp
// Description: Hands-on 16-bit SPN / Mini-AES Single-Round Playground
//              (Key XOR -> 4-bit S-Box Substitution -> Bit/Nibble Permutation)
// Created: 2026-11-04
// ============================================================================

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <array>

using namespace std;

// 1. SUBSTITUTION LAYER (Confusion): 4-bit S-Box & Inverse S-Box
// Maps every 4-bit nibble (0x0..0xF) non-linearly to another 4-bit nibble
constexpr array<uint8_t, 16> SBOX = {
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
    0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7
};

constexpr array<uint8_t, 16> INV_SBOX = {
    0xE, 0x3, 0x4, 0x8, 0x1, 0xC, 0xA, 0xF,
    0x7, 0xD, 0x9, 0x6, 0xB, 0x2, 0x0, 0x5
};

// Apply 4-bit S-Box across all 4 nibbles of a 16-bit state
uint16_t sub_nibbles(uint16_t state, const array<uint8_t, 16>& box) {
    uint16_t out = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t nibble = (state >> (i * 4)) & 0xF;
        out |= static_cast<uint16_t>(box[nibble]) << (i * 4);
    }
    return out;
}

// 2. PERMUTATION LAYER (Diffusion): Mini-AES ShiftRow on 2x2 Nibble Matrix
// State layout: [ n3 (bits 15..12) | n1 (bits 7..4)  ]  <-- Row 0 (unchanged)
//               [ n2 (bits 11..8)  | n0 (bits 3..0)  ]  <-- Row 1 (swapped!)
uint16_t shift_row(uint16_t s) {
    uint16_t n3 = (s >> 12) & 0xF; // Top-left
    uint16_t n2 = (s >> 8)  & 0xF; // Bottom-left
    uint16_t n1 = (s >> 4)  & 0xF; // Top-right
    uint16_t n0 =  s        & 0xF; // Bottom-right
    // Swap bottom row (n2 <-> n0)
    return (n3 << 12) | (n0 << 8) | (n1 << 4) | n2;
}

int main() {
    // 16-bit block ("Hi" = 0x4869) and 16-bit round keys
    uint16_t plaintext = 0x4869;
    uint16_t k0 = 0xA53C;
    uint16_t k1 = 0x7F19;

    cout << hex << uppercase << setfill('0');
    cout << "Plaintext         : 0x" << setw(4) << plaintext << "\n";

    // --- ENCRYPTION (1 SPN Round: AddKey -> SubNibbles -> ShiftRow -> AddKey) ---
    uint16_t state = plaintext ^ k0;                  // Step 1: Pre-whitening Key XOR
    state = sub_nibbles(state, SBOX);                 // Step 2: Non-linear S-Box (Confusion)
    state = shift_row(state);                         // Step 3: Permutation (Diffusion)
    uint16_t ciphertext = state ^ k1;                 // Step 4: Final Round Key XOR

    cout << "Encrypted (SPN)   : 0x" << setw(4) << ciphertext << "\n";

    // --- DECRYPTION (Reverse order with Inverse Operations) ---
    uint16_t dec = ciphertext ^ k1;                   // Undo Final Key XOR
    dec = shift_row(dec);                             // ShiftRow is its own inverse!
    dec = sub_nibbles(dec, INV_SBOX);                 // Inverse S-Box
    dec = dec ^ k0;                                   // Undo Initial Key XOR

    cout << "Decrypted (SPN)   : 0x" << setw(4) << dec << "\n";
    return 0;
}
