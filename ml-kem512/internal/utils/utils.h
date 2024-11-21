#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>  // for size_t

// Algorithm 3: BitsToBytes
void BitsToBytes(uint8_t* bits, uint8_t* bytes, size_t bit_length);

// Algorithm 4: BytesToBits
void BytesToBits(uint8_t* bytes, uint8_t* bits, size_t byteLength);

// Function to encode bytes (ByteEncode)
void ByteEncode(uint16_t* F, uint8_t* B, int d);

void ByteDecode(uint8_t* B, uint16_t* F, int d);
#endif // UTILS_H
