#include <iostream>
#include <iomanip>
#include <cstdint>
#include <array>
#include <bit>

// The core Quarter-Round engine
inline void quarter_round(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    a += b; d ^= a; d = std::rotl(d, 16);
    c += d; b ^= c; b = std::rotl(b, 12);
    a += b; d ^= a; d = std::rotl(d, 8);
    c += d; b ^= c; b = std::rotl(b, 7);
}

// 4x4 matrix (16 words = 64 bytes = 512 bits)
using State = std::array<uint32_t, 16>;

void print_matrix(const std::string& title, const State& s) {
    std::cout << title << ":\n";
    for (int r = 0; r < 4; ++r) {
        std::cout << "  ";
        for (int c = 0; c < 4; ++c) {
            std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                      << s[r * 4 + c] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::dec << std::setfill(' ') << "\n";
}

// Full ChaCha20 Block Function (20 rounds = 10 double-rounds)
State chacha20_block(const State& input) {
    State x = input;

    // 10 double-rounds = 20 rounds
    for (int round = 0; round < 10; ++round) {
        // Column round: QR on each of the 4 columns
        quarter_round(x[0], x[4],  x[8], x[12]);
        quarter_round(x[1], x[5],  x[9], x[13]);
        quarter_round(x[2], x[6], x[10], x[14]);
        quarter_round(x[3], x[7], x[11], x[15]);

        // Diagonal round: QR on each of the 4 diagonals
        quarter_round(x[0], x[5], x[10], x[15]);
        quarter_round(x[1], x[6], x[11], x[12]);
        quarter_round(x[2], x[7],  x[8], x[13]);
        quarter_round(x[3], x[4],  x[9], x[14]);
    }

    // Crucial step: Feed-forward addition (x[i] = x[i] + input[i])
    // This turns an invertible permutation into a one-way pseudorandom function!
    State out;
    for (int i = 0; i < 16; ++i) {
        out[i] = x[i] + input[i];
    }
    return out;
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "    ChaCha20 Block Engine: How QR Permutes the 4x4 State Matrix      \n";
    std::cout << "====================================================================\n\n";

    // RFC 7539 Section 2.3.2 Test Vector initial state:
    // Constants ("expand 32-byte k")
    // Key (256 bits = 8 words: 0x03020100 .. 0x1f1e1d1c)
    // Counter (1 word: 1)
    // Nonce (3 words = 96 bits: 0x00000000, 0x4a000000, 0x00000000)
    State initial = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, // "expand 32-byte k"
        0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, // Key words 0..3
        0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, // Key words 4..7
        0x00000001,                                     // Block counter = 1
        0x09000000, 0x4a000000, 0x00000000              // Nonce
    };

    print_matrix("Initial 4x4 ChaCha State", initial);

    State keystream = chacha20_block(initial);

    print_matrix("Generated 512-bit Keystream Block (after 20 QR rounds + Add)", keystream);

    // Verify first row of RFC 7539 Section 2.3.2 output
    // RFC byte stream begins: 10 f1 e7 e4 d1 3b 59 15 50 0f dd 1f c4 71 20 a3 ...
    // Stored as 32-bit little-endian words:
    // word 0: 0xe4e7f110, word 1: 0x15593bd1, word 2: 0x1fdd0f50, word 3: 0xc47120a3
    if (keystream[0] == 0xe4e7f110 && keystream[1] == 0x15593bd1 &&
        keystream[2] == 0x1fdd0f50 && keystream[3] == 0xc47120a3) {
        std::cout << "[SUCCESS] Keystream block matches RFC 7539 / 8439 standard!\n";
    } else {
        std::cerr << "[FAIL] Output mismatch against RFC 7539!\n";
        return 1;
    }

    return 0;
}
