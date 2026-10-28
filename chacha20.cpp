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
