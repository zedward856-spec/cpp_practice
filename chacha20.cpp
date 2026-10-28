// ============================================================================
// Author: zedward856-spec
// File: chacha20.cpp
// Description: Custom 4-Letter ("Lucy") Stream XOR Encryption & Decryption Engine
// Created: 2026-10-28
// ============================================================================

#include <bits/stdc++.h>
#include <iomanip>
#include <bit>
using namespace std ;
int main(){
    uint32_t message = 0x4c756379 ;
    uint32_t key = 0x4b657973 ;
    uint32_t encrypted = message ^ key ;
    cout << "Encrypted :"<< hex << encrypted << '\n';
    uint32_t decrypted = encrypted ^ key ;
    cout << "Decrypted :"<< hex << decrypted ;
}
