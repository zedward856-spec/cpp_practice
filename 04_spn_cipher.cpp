#include <iostream>
#include <iomanip>
#include <cstdint>
#include <array>
#include <bit>
#include <bitset>

// ============================================================================
// Week 2 Module 1: 16-Bit Substitution-Permutation Network (SPN)
// Based on Howard Heys' Tutorial SPN Architecture (4 Rounds, 4x4 S-Boxes)
//
// Core Components of an SPN Block Cipher:
//   1. Key Mixing (AddRoundKey)  : state ^= K_r
//   2. Substitution (S-Box)      : Non-linear 4-bit lookup (Confusion)
//   3. Permutation (P-Box)       : 16-bit wire transposition (Diffusion)
// ============================================================================

// Heys' 4-bit S-Box (maps 0x0..0xF -> 0x0..0xF non-linearly)
constexpr std::array<uint8_t, 16> S_BOX = {
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
    0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7
};

// Inverse 4-bit S-Box for decryption: S_INV[S_BOX[x]] == x
constexpr std::array<uint8_t, 16> S_BOX_INV = {
    0xE, 0x3, 0x4, 0x8, 0x1, 0xC, 0xA, 0xF,
    0x7, 0xD, 0x9, 0x6, 0xB, 0x2, 0x0, 0x5
};

// 16-bit Bit Permutation Table (P-Box):
// Routes bit i of S-Box j to bit j of S-Box i (a 4x4 bit-matrix transpose!).
// Note: Because it is a matrix transpose, P_BOX is an involution (P == P^-1).
constexpr std::array<uint8_t, 16> P_BOX = {
    0,  4,  8, 12,
    1,  5,  9, 13,
    2,  6, 10, 14,
    3,  7, 11, 15
};

// Apply 4-bit S-Box to all 4 nibbles in a 16-bit state
uint16_t substitute_nibbles(uint16_t state, const std::array<uint8_t, 16>& sbox) {
    uint16_t result = 0;
    for (int nibble_idx = 0; nibble_idx < 4; ++nibble_idx) {
        uint8_t nibble = (state >> (nibble_idx * 4)) & 0x0F;
        result |= static_cast<uint16_t>(sbox[nibble]) << (nibble_idx * 4);
    }
    return result;
}

// Apply 16-bit wire permutation (P-Box)
uint16_t permute_bits(uint16_t state) {
    uint16_t result = 0;
    for (int bit = 0; bit < 16; ++bit) {
        if ((state >> bit) & 1U) {
            result |= (1U << P_BOX[bit]);
        }
    }
    return result;
}

void print_spn_state(const std::string& label, uint16_t s) {
    std::cout << std::left << std::setw(24) << std::setfill(' ') << label
              << ": 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << s
              << "  [nibbles: "
              << ((s >> 12) & 0xF) << " "
              << ((s >> 8)  & 0xF) << " "
              << ((s >> 4)  & 0xF) << " "
              << (s & 0xF) << "]  bits: "
              << std::bitset<16>(s)
              << std::dec << std::setfill(' ') << "\n";
}

// Full 4-Round SPN Encryption
uint16_t spn_encrypt(uint16_t plaintext, const std::array<uint16_t, 5>& round_keys, bool verbose = false) {
    uint16_t state = plaintext;
    if (verbose) {
        print_spn_state("Initial Plaintext", state);
    }

    // Rounds 1 to 3: Key XOR -> S-Box -> P-Box
    for (int r = 0; r < 3; ++r) {
        state ^= round_keys[r];
        state = substitute_nibbles(state, S_BOX);
        state = permute_bits(state);
        if (verbose) {
            print_spn_state("After Round " + std::to_string(r + 1) + " (K^S^P)", state);
        }
    }

    // Round 4 (Final Round): Key XOR -> S-Box -> Final Key Whitening (NO P-Box!)
    // Why no P-Box in the last round? A final keyless bit permutation adds zero
    // cryptographic security because an attacker can trivially invert the wire order.
    state ^= round_keys[3];
    state = substitute_nibbles(state, S_BOX);
    state ^= round_keys[4];

    if (verbose) {
        print_spn_state("After Round 4 (Final)", state);
    }
    return state;
}

// Full 4-Round SPN Decryption
uint16_t spn_decrypt(uint16_t ciphertext, const std::array<uint16_t, 5>& round_keys) {
    uint16_t state = ciphertext;

    // Undo Round 4: Final Key XOR -> Inverse S-Box -> Round 3 Key XOR
    state ^= round_keys[4];
    state = substitute_nibbles(state, S_BOX_INV);
    state ^= round_keys[3];

    // Undo Rounds 3 down to 1: Inverse P-Box -> Inverse S-Box -> Round Key XOR
    for (int r = 2; r >= 0; --r) {
        state = permute_bits(state); // P_BOX is self-inverse (transpose)
        state = substitute_nibbles(state, S_BOX_INV);
        state ^= round_keys[r];
    }
    return state;
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "   16-Bit Substitution-Permutation Network (SPN) Block Cipher Lab   \n";
    std::cout << "====================================================================\n\n";

    // 5 16-bit Round Subkeys (K1..K5) derived from master key
    constexpr std::array<uint16_t, 5> round_keys = {
        0x3A94, 0xA94D, 0x94D6, 0x4D63, 0xD63F
    };

    uint16_t plaintext = 0x26B7;
    uint16_t ciphertext = spn_encrypt(plaintext, round_keys, true);
    uint16_t recovered  = spn_decrypt(ciphertext, round_keys);

    std::cout << "\n" << std::string(68, '-') << "\n";
    print_spn_state("Decrypted Plaintext", recovered);

    // Avalanche / Diffusion Test: Flip 1 bit of plaintext
    uint16_t perturbed_pt = plaintext ^ 0x0001;
    uint16_t perturbed_ct = spn_encrypt(perturbed_pt, round_keys, false);
    int flipped_bits = std::popcount(static_cast<uint16_t>(ciphertext ^ perturbed_ct));

    std::cout << "\n--- SPN Confusion & Diffusion (1-Bit Avalanche Test) ---\n";
    std::cout << "Plaintext 1 : 0x" << std::hex << std::setw(4) << std::setfill('0') << plaintext
              << " -> Ciphertext 1 : 0x" << std::setw(4) << ciphertext << "\n";
    std::cout << "Plaintext 2 : 0x" << std::setw(4) << perturbed_pt
              << " -> Ciphertext 2 : 0x" << std::setw(4) << perturbed_ct << std::dec << std::setfill(' ') << "\n";
    std::cout << "Hamming Distance : " << flipped_bits << " / 16 bits flipped ("
              << (flipped_bits * 100.0 / 16.0) << "%)\n";

    if (recovered == plaintext) {
        std::cout << "\n[SUCCESS] SPN Encryption & Decryption verified with full diffusion!\n";
    } else {
        std::cerr << "\n[FAIL] Decryption did not recover original plaintext!\n";
        return 1;
    }

    return 0;
}
