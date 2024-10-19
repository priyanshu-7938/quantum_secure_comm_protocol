#ifndef SHA3_UTILS_H
#define SHA3_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// Utility (or commonly used) functions for SHA3 implementation in C.
namespace sha3_utils {

// Byte swap for a 64-bit unsigned integer.
static inline uint64_t
bswap(const uint64_t a)
{
#if defined __GNUG__
    return __builtin_bswap64(a);
#else
    return ((a & 0x00000000000000ffull) << 56) |
           ((a & 0x000000000000ff00ull) << 40) |
           ((a & 0x0000000000ff0000ull) << 24) |
           ((a & 0x00000000ff000000ull) << 8) |
           ((a & 0x000000ff00000000ull) >> 8) |
           ((a & 0x0000ff0000000000ull) >> 24) |
           ((a & 0x00ff000000000000ull) >> 40) |
           ((a & 0xff00000000000000ull) >> 56);
#endif
}

// Interpret 8-byte array as a 64-bit unsigned integer (little-endian).
static inline uint64_t
le_bytes_to_u64(const uint8_t* bytes)
{
    uint64_t word = ((uint64_t)bytes[7] << 56) |
                    ((uint64_t)bytes[6] << 48) |
                    ((uint64_t)bytes[5] << 40) |
                    ((uint64_t)bytes[4] << 32) |
                    ((uint64_t)bytes[3] << 24) |
                    ((uint64_t)bytes[2] << 16) |
                    ((uint64_t)bytes[1] << 8)  |
                    ((uint64_t)bytes[0]);

    return word;
}

// Convert an array of bytes (length rate/8) into an array of 64-bit words (little-endian).
static inline void
le_bytes_to_u64_words(const uint8_t* bytes, size_t byte_len, uint64_t* words, size_t word_len)
{
    size_t off = 0;
    while (off < byte_len) {
        words[off / 8] = le_bytes_to_u64(bytes + off);
        off += 8;
    }
}

// Interpret a 64-bit unsigned integer as 8 bytes in little-endian order.
static inline void
u64_to_le_bytes(uint64_t word, uint8_t* bytes)
{
    bytes[0] = (uint8_t)(word >> 0);
    bytes[1] = (uint8_t)(word >> 8);
    bytes[2] = (uint8_t)(word >> 16);
    bytes[3] = (uint8_t)(word >> 24);
    bytes[4] = (uint8_t)(word >> 32);
    bytes[5] = (uint8_t)(word >> 40);
    bytes[6] = (uint8_t)(word >> 48);
    bytes[7] = (uint8_t)(word >> 56);
}

// Convert an array of 64-bit words into an array of bytes (little-endian).
static inline void
u64_words_to_le_bytes(const uint64_t* words, size_t word_len, uint8_t* bytes, size_t byte_len)
{
    size_t off = 0;
    while (off < word_len) {
        u64_to_le_bytes(words[off], bytes + off * 8);
        off++;
    }
}

// Generate random data of type T (assumed unsigned) for C-compatible random generation.
static inline void
random_data(uint64_t* data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        data[i] = rand();  // Simple random number generation
    }
}

// Convert a byte array to a hex string.
static inline void
to_hex(const uint8_t* bytes, size_t len, char* out_hex)
{
    for (size_t i = 0; i < len; i++) {
        sprintf(out_hex + (i * 2), "%02x", bytes[i]);
    }
}

// Convert a hex string into a byte array.
static inline void
from_hex(const char* hex, uint8_t* bytes)
{
    size_t len = strlen(hex) / 2;
    for (size_t i = 0; i < len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bytes[i]);
    }
}

#endif  // SHA3_UTILS_H
