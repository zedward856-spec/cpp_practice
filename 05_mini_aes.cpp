#include <iostream>
#include <iomanip>
#include <cstdint>
#include <array>

// ============================================================================
// Week 2 Module 2: Mini-AES (16-Bit Educational AES Specification by R. Phan)
//
// Mini-AES operates on a 16-bit block arranged as a 2x2 matrix of 4-bit nibbles
// over the Galois Field GF(2^4) with irreducible polynomial x^4 + x + 1 (0x13):
//
//         [ s0   s2 ]     where s0 = bits 15..12, s1 = bits 11..8,
//     S = [         ]           s2 = bits  7..4,  s3 = bits  3..0
//         [ s1   s3 ]
//
// Each Mini-AES Round applies the exact 4 AES transformations in miniature:
//   1. NibbleSub  (gamma) : 4-bit non-linear S-Box on each nibble
//   2. ShiftRow   (pi)    : Swap bottom row nibbles (s1 <-> s3)
//   3. MixColumn  (theta) : Multiply columns by [3, 2; 2, 3] in GF(2^4)
//   4. KeyAddition(sigma) : Bitwise XOR with 16-bit Round Key
// ============================================================================

using NibbleMatrix = std::array<uint8_t, 4>; // {s0, s1, s2, s3}

// Pack/unpack between uint16_t and 2x2 NibbleMatrix {s0, s1, s2, s3}
constexpr NibbleMatrix unpack_state(uint16_t block) {
    return {
        static_cast<uint8_t>((block >> 12) & 0xF),
        static_cast<uint8_t>((block >> 8)  & 0xF),
        static_cast<uint8_t>((block >> 4)  & 0xF),
        static_cast<uint8_t>( block        & 0xF)
    };
}

constexpr uint16_t pack_state(const NibbleMatrix& s) {
    return static_cast<uint16_t>(
        ((s[0] & 0xF) << 12) |
        ((s[1] & 0xF) << 8)  |
        ((s[2] & 0xF) << 4)  |
         (s[3] & 0xF)
    );
}

// Mini-AES 4-bit S-Box & Inverse S-Box (Phan Table 1)
constexpr std::array<uint8_t, 16> MINI_AES_SBOX = {
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
    0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7
};

constexpr std::array<uint8_t, 16> MINI_AES_INV_SBOX = {
    0xE, 0x3, 0x4, 0x8, 0x1, 0xC, 0xA, 0xF,
    0x7, 0xD, 0x9, 0x6, 0xB, 0x2, 0x0, 0x5
};

// Multiply two 4-bit elements in GF(2^4) modulo x^4 + x + 1 (0b10011 = 0x13)
constexpr uint8_t gf16_mul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 4; ++i) {
        if (b & 1) {
            p ^= a;
        }
        bool high_bit = (a & 0x8) != 0;
        a <<= 1;
        if (high_bit) {
            a ^= 0x13; // Reduce modulo x^4 + x + 1
        }
        a &= 0xF;
        b >>= 1;
    }
    return p;
}

// 1. NibbleSub (and Inverse NibbleSub)
NibbleMatrix nibble_sub(const NibbleMatrix& s, const std::array<uint8_t, 16>& box) {
    return { box[s[0]], box[s[1]], box[s[2]], box[s[3]] };
}

// 2. ShiftRow: Row 0 unchanged, Row 1 shifted left by 1 (swaps s1 <-> s3)
NibbleMatrix shift_row(const NibbleMatrix& s) {
    return { s[0], s[3], s[2], s[1] };
}

// 3. MixColumn: Multiply each column by constant MDS matrix M = [3 2; 2 3] in GF(2^4)
// Remarkable property: In GF(2^4), [3 2; 2 3]^2 = [1 0; 0 1], so MixColumn is its OWN inverse!
NibbleMatrix mix_column(const NibbleMatrix& s) {
    return {
        static_cast<uint8_t>(gf16_mul(3, s[0]) ^ gf16_mul(2, s[1])),
        static_cast<uint8_t>(gf16_mul(2, s[0]) ^ gf16_mul(3, s[1])),
        static_cast<uint8_t>(gf16_mul(3, s[2]) ^ gf16_mul(2, s[3])),
        static_cast<uint8_t>(gf16_mul(2, s[2]) ^ gf16_mul(3, s[3]))
    };
}

// 4. KeyAddition: XOR state with round key
NibbleMatrix add_round_key(const NibbleMatrix& s, const NibbleMatrix& k) {
    return {
        static_cast<uint8_t>(s[0] ^ k[0]),
        static_cast<uint8_t>(s[1] ^ k[1]),
        static_cast<uint8_t>(s[2] ^ k[2]),
        static_cast<uint8_t>(s[3] ^ k[3])
    };
}

// Mini-AES Key Schedule: Generates K0, K1, K2 from a 16-bit Master Key
// Uses RCON_1 = 0x1 (0001_2) and RCON_2 = 0x2 (0010_2)
std::array<NibbleMatrix, 3> expand_key(uint16_t master_key) {
    std::array<NibbleMatrix, 3> keys{};
    keys[0] = unpack_state(master_key);

    constexpr std::array<uint8_t, 2> RCON = { 0x1, 0x2 };
    for (int r = 0; r < 2; ++r) {
        const auto& prev = keys[r];
        NibbleMatrix next{};
        next[0] = prev[0] ^ MINI_AES_SBOX[prev[3]] ^ RCON[r];
        next[1] = prev[1] ^ next[0];
        next[2] = prev[2] ^ next[1];
        next[3] = prev[3] ^ next[2];
        keys[r + 1] = next;
    }
    return keys;
}

void print_2x2(const std::string& title, const NibbleMatrix& s) {
    std::cout << std::left << std::setw(34) << std::setfill(' ') << title
              << "0x" << std::right << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
              << pack_state(s) << std::setfill(' ')
              << "   | " << static_cast<int>(s[0]) << "  " << static_cast<int>(s[2]) << " |\n"
              << std::string(43, ' ')
              << "| " << static_cast<int>(s[1]) << "  " << static_cast<int>(s[3]) << " |"
              << std::dec << "\n";
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "     Mini-AES (16-Bit SPN Block Cipher over GF(2^4)) Engine Lab     \n";
    std::cout << "====================================================================\n\n";

    // Official Test Vector from R. Phan, "Mini Advanced Encryption Standard (Mini-AES)"
    // Plaintext P = 0x9C63, Master Key K = 0xC3F0 -> Expected Ciphertext C = 0x72C6
    uint16_t plaintext  = 0x9C63;
    uint16_t master_key = 0xC3F0;

    auto round_keys = expand_key(master_key);
    std::cout << "--- Expanded Round Keys ---\n";
    print_2x2("Round Key K0 (Master Key)", round_keys[0]);
    print_2x2("Round Key K1 (RCON = 0x1)", round_keys[1]);
    print_2x2("Round Key K2 (RCON = 0x2)", round_keys[2]);

    std::cout << "\n--- Encryption Trace ---\n";
    NibbleMatrix state = unpack_state(plaintext);
    print_2x2("Initial Plaintext (P)", state);

    // Round 0: Pre-whitening KeyAddition
    state = add_round_key(state, round_keys[0]);
    print_2x2("After Round 0 AddRoundKey(K0)", state);

    // Round 1: NibbleSub -> ShiftRow -> MixColumn -> AddRoundKey(K1)
    state = nibble_sub(state, MINI_AES_SBOX);
    print_2x2("Round 1.1: NibbleSub (gamma)", state);
    state = shift_row(state);
    print_2x2("Round 1.2: ShiftRow (pi)", state);
    state = mix_column(state);
    print_2x2("Round 1.3: MixColumn (theta)", state);
    state = add_round_key(state, round_keys[1]);
    print_2x2("Round 1.4: AddRoundKey (K1)", state);

    // Round 2 (Final Round): NibbleSub -> ShiftRow -> AddRoundKey(K2) (No MixColumn!)
    state = nibble_sub(state, MINI_AES_SBOX);
    print_2x2("Round 2.1: NibbleSub (gamma)", state);
    state = shift_row(state);
    print_2x2("Round 2.2: ShiftRow (pi)", state);
    state = add_round_key(state, round_keys[2]);
    print_2x2("Round 2.3: AddRoundKey (K2) [C]", state);

    uint16_t ciphertext = pack_state(state);

    // --- Decryption Trace ---
    NibbleMatrix dec = state;
    dec = add_round_key(dec, round_keys[2]);
    dec = shift_row(dec);
    dec = nibble_sub(dec, MINI_AES_INV_SBOX);

    dec = add_round_key(dec, round_keys[1]);
    dec = mix_column(dec); // MixColumn is self-inverse in Mini-AES!
    dec = shift_row(dec);
    dec = nibble_sub(dec, MINI_AES_INV_SBOX);

    dec = add_round_key(dec, round_keys[0]);
    uint16_t recovered = pack_state(dec);

    std::cout << "\n" << std::string(68, '-') << "\n";
    print_2x2("Final Ciphertext", state);
    print_2x2("Decrypted Plaintext", dec);

    const uint16_t expected_ciphertext = 0x72C6;
    if (ciphertext == expected_ciphertext && recovered == plaintext) {
        std::cout << "\n[SUCCESS] Ciphertext 0x72C6 matches Phan's Mini-AES specification\n"
                  << "          and decryption recovered 0x9C63 accurately!\n";
    } else {
        std::cerr << "\n[FAIL] Output mismatch against Mini-AES specification!\n";
        return 1;
    }

    return 0;
}
