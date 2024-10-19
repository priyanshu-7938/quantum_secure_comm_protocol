#ifndef ML_KEM_UTILS_H
#define ML_KEM_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Function to compare two byte arrays in constant time.
// Returns 0xFFFFFFFF if equal, otherwise 0x00000000.
static inline uint32_t ct_memcmp(const uint8_t* bytes0, const uint8_t* bytes1, size_t n) {
    uint32_t flag = ~0u;  // Set all bits to 1 (0xFFFFFFFF)
    
    for (size_t i = 0; i < n; i++) {
        flag &= (bytes0[i] == bytes1[i]) ? ~0u : 0u;  // If not equal, clear flag
    }
    
    return flag;
}

// Function for conditionally copying bytes from one of two source arrays to a sink array.
// `cond` should be either 0x00000000 (false) or 0xFFFFFFFF (true).
static inline void ct_cond_memcpy(uint32_t cond, uint8_t* sink, const uint8_t* source0, const uint8_t* source1, size_t n) {
    for (size_t i = 0; i < n; i++) {
        sink[i] = (cond) ? source0[i] : source1[i];  // Select source based on cond
    }
}

// Returns the K-PKE public key byte length.
static inline size_t get_pke_public_key_len(size_t k) {
    return k * 12 * 32 + 32;
}

// Returns the K-PKE secret key byte length.
static inline size_t get_pke_secret_key_len(size_t k) {
    return k * 12 * 32;
}

// Returns the K-PKE cipher text byte length.
static inline size_t get_pke_cipher_text_len(size_t k, size_t du, size_t dv) {
    return 32 * (k * du + dv);
}

// Returns the ML-KEM public key byte length.
static inline size_t get_kem_public_key_len(size_t k) {
    return get_pke_public_key_len(k);
}

// Returns the ML-KEM secret key byte length.
static inline size_t get_kem_secret_key_len(size_t k) {
    return get_pke_secret_key_len(k) + get_pke_public_key_len(k) + 32 + 32;
}

// Returns the ML-KEM cipher text byte length.
static inline size_t get_kem_cipher_text_len(size_t k, size_t du, size_t dv) {
    return get_pke_cipher_text_len(k, du, dv);
}

#endif // ML_KEM_UTILS_H
