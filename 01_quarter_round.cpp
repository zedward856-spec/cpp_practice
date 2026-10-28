#include <iostream>
#include <iomanip>
#include <cstdint>
#include <bit> // C++20 std::rotl

// ChaCha20 Quarter Round operation on 4 32-bit words
void quarter_round(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    a += b; d ^= a; d = std::rotl(d, 16);
    c += d; b ^= c; b = std::rotl(b, 12);
    a += b; d ^= a; d = std::rotl(d, 8);
    c += d; b ^= c; b = std::rotl(b, 7);
}

// Helper to print words cleanly in hex
void print_state(const std::string& label, uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    std::cout << std::left << std::setw(16) << std::setfill(' ') << label << ": "
              << "a=0x" << std::hex << std::setw(8) << std::setfill('0') << a << "  "
              << "b=0x" << std::hex << std::setw(8) << std::setfill('0') << b << "  "
              << "c=0x" << std::hex << std::setw(8) << std::setfill('0') << c << "  "
              << "d=0x" << std::hex << std::setw(8) << std::setfill('0') << d
              << std::dec << std::setfill(' ') << "\n";
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "   ChaCha20 Quarter Round Engine (RFC 7539 / RFC 8439 Test Vector)   \n";
    std::cout << "====================================================================\n\n";

    // RFC 7539 Section 2.1.1 Test Vector
    uint32_t a = 0x11111111;
    uint32_t b = 0x01020304;
    uint32_t c = 0x9b8d6f43;
    uint32_t d = 0x01234567;

    print_state("Initial", a, b, c, d);

    // Step-by-step breakdown of the ARX sequence
    std::cout << "\n[1] a += b; d ^= a; d <<<= 16\n";
    a += b; d ^= a; d = std::rotl(d, 16);
    print_state("After Step 1", a, b, c, d);

    std::cout << "\n[2] c += d; b ^= c; b <<<= 12\n";
    c += d; b ^= c; b = std::rotl(b, 12);
    print_state("After Step 2", a, b, c, d);

    std::cout << "\n[3] a += b; d ^= a; d <<<= 8\n";
    a += b; d ^= a; d = std::rotl(d, 8);
    print_state("After Step 3", a, b, c, d);

    std::cout << "\n[4] c += d; b ^= c; b <<<= 7\n";
    c += d; b ^= c; b = std::rotl(b, 7);
    print_state("After Step 4", a, b, c, d);

    // Verification against RFC expected values
    const uint32_t exp_a = 0xea2a92f4;
    const uint32_t exp_b = 0xcb1cf8ce;
    const uint32_t exp_c = 0x4581472e;
    const uint32_t exp_d = 0x5881c4bb;

    std::cout << "\n" << std::string(68, '-') << "\n";
    print_state("Final Result", a, b, c, d);
    print_state("RFC Expected", exp_a, exp_b, exp_c, exp_d);

    if (a == exp_a && b == exp_b && c == exp_c && d == exp_d) {
        std::cout << "\n[SUCCESS] All 4 words match RFC 7539 / RFC 8439 exactly!\n";
    } else {
        std::cerr << "\n[FAIL] Output does not match RFC specification!\n";
        return 1;
    }

    return 0;
}
