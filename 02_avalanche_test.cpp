#include <iostream>
#include <iomanip>
#include <cstdint>
#include <bit> // std::rotl, std::popcount

void quarter_round(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    a += b; d ^= a; d = std::rotl(d, 16);
    c += d; b ^= c; b = std::rotl(b, 12);
    a += b; d ^= a; d = std::rotl(d, 8);
    c += d; b ^= c; b = std::rotl(b, 7);
}

// Compute total flipped bits across the 4 32-bit words (128 bits total)
int total_hamming_distance(uint32_t a1, uint32_t b1, uint32_t c1, uint32_t d1,
                           uint32_t a2, uint32_t b2, uint32_t c2, uint32_t d2) {
    return std::popcount(a1 ^ a2) +
           std::popcount(b1 ^ b2) +
           std::popcount(c1 ^ c2) +
           std::popcount(d1 ^ d2);
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "  Diffusion & Avalanche Effect in ChaCha20 Quarter-Round (QR) Engine \n";
    std::cout << "====================================================================\n\n";

    // Original state
    uint32_t a1 = 0x11111111, b1 = 0x01020304, c1 = 0x9b8d6f43, d1 = 0x01234567;
    // Perturbed state: flip exactly 1 bit (bit 0 of word 'a')
    uint32_t a2 = a1 ^ 0x00000001, b2 = b1, c2 = c1, d2 = d1;

    std::cout << "We start with two states differing by EXACTLY 1 BIT in word 'a'.\n";
    std::cout << "Total bits in a 4-word state = 4 x 32 = 128 bits.\n\n";

    std::cout << "Step-by-step bit flip count (Hamming Distance out of 128 bits):\n";
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << "Initial difference: " 
              << total_hamming_distance(a1, b1, c1, d1, a2, b2, c2, d2) 
              << " bit(s) (1 / 128 = 0.78%)\n\n";

    // Step 1
    a1 += b1; d1 ^= a1; d1 = std::rotl(d1, 16);
    a2 += b2; d2 ^= a2; d2 = std::rotl(d2, 16);
    std::cout << "After Step 1 (a+=b; d^=a; d<<<16): "
              << std::setw(3) << total_hamming_distance(a1, b1, c1, d1, a2, b2, c2, d2)
              << " bits flipped  (a, d affected)\n";

    // Step 2
    c1 += d1; b1 ^= c1; b1 = std::rotl(b1, 12);
    c2 += d2; b2 ^= c2; b2 = std::rotl(b2, 12);
    std::cout << "After Step 2 (c+=d; b^=c; b<<<12): "
              << std::setw(3) << total_hamming_distance(a1, b1, c1, d1, a2, b2, c2, d2)
              << " bits flipped  (all 4 words now contaminated!)\n";

    // Step 3
    a1 += b1; d1 ^= a1; d1 = std::rotl(d1, 8);
    a2 += b2; d2 ^= a2; d2 = std::rotl(d2, 8);
    std::cout << "After Step 3 (a+=b; d^=a; d<<<8):  "
              << std::setw(3) << total_hamming_distance(a1, b1, c1, d1, a2, b2, c2, d2)
              << " bits flipped\n";

    // Step 4
    c1 += d1; b1 ^= c1; b1 = std::rotl(b1, 7);
    c2 += d2; b2 ^= c2; b2 = std::rotl(b2, 7);
    int final_diff = total_hamming_distance(a1, b1, c1, d1, a2, b2, c2, d2);
    std::cout << "After Step 4 (c+=d; b^=c; b<<<7):  "
              << std::setw(3) << final_diff
              << " bits flipped  (" << (final_diff * 100.0 / 128.0) << "%)\n\n";

    std::cout << "Conclusion:\n";
    std::cout << "- In just ONE single quarter-round (4 instructions), a single bit difference\n";
    std::cout << "  ripples through ARX additions, rotations, and XORs to flip many bits across ALL words.\n";
    std::cout << "- When repeated across the full 20 rounds of ChaCha20, the difference approaches\n";
    std::cout << "  ideal randomness: ~50% (64 / 128 bits flipped) with zero statistical bias.\n";

    return 0;
}
