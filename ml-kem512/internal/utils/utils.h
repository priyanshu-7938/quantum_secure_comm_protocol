#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <string.h>

// Algorithm 3: BitsToBytes
void BitsToBytes(const uint8_t* bits, size_t bitLength, uint8_t* bytes) {
    memset(bytes, 0, bitLength / 8);
    for (size_t i = 0; i < bitLength; i++) {
        bytes[i / 8] |= (bits[i] & 1) << (i % 8);
    }
}

// Algorithm 4: BytesToBits
void BytesToBits(const uint8_t* bytes, size_t byteLength, uint8_t* bits) {
    for (size_t i = 0; i < byteLength; i++) {
        uint8_t byte = bytes[i];
        for (int j = 0; j < 8; j++) {
            bits[i * 8 + j] = byte & 1;
            byte >>= 1;
        }
    }
}

#endif // UTILS_H
