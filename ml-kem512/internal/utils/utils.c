#include "utils.h"

void BitsToBytes(uint8_t* bits, uint8_t* bytes, size_t bit_length) {
    for (size_t i = 0; i < bit_length / 8; ++i) {
        bytes[i] = 0;
        for (size_t j = 0; j < 8; ++j) {
            bytes[i] |= (bits[i * 8 + j] << (7 - j));
        }
    }
}

void ByteDecode(uint8_t* B, uint16_t* F, int d) {
    uint8_t b[256 * d];
    BytesToBits(B, b, 256 * d);
    for (int i = 0; i < 256; i++) {
        uint32_t a = 0;  
        for (int j = 0; j < d; j++) {
            a |= (b[i * d + j] << j);
        }
        F[i] = (uint16_t)a; 
    }
}

void BytesToBits(uint8_t* bytes, uint8_t* bits, size_t bit_length) {
    for (size_t i = 0; i < bit_length / 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            bits[i * 8 + j] = (bytes[i] >> (7 - j)) & 1;  // Extract each bit
        }
    }
}

void ByteEncode(uint16_t* F, uint8_t* B, int d) {
    uint8_t b[256 * d];
    for (int i = 0; i < 256; i++) {
        uint32_t a = F[i];
        for (int j = 0; j < d; j++) {
            b[i * d + j] = a % 2;  // Extract bit
            a = (a - b[i * d + j]) / 2;  // Update 'a' for next bit
        }
    }
    
    BitsToBytes(b, B, 256 * d);
}
