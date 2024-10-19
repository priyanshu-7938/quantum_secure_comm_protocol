#ifndef ML_KEM_PRNG_H
#define ML_KEM_PRNG_H

#include "force_inline.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define BIT_SECURITY_LEVEL 256  // Hardcoded for simplicity, can be 128, 192, or 256

typedef struct {
    uint8_t state[32];  // Simplified state, size based on max BIT_SECURITY_LEVEL
} prng_t;

// Default constructor equivalent
forceinline void prng_init_default(prng_t* prng) {
    // Simplified initialization with a fixed pattern
    for (int i = 0; i < 32; i++) {
        prng->state[i] = i;
    }
}

// Explicit constructor equivalent
forceinline void prng_init_seeded(prng_t* prng, const uint8_t* seed, size_t seed_len) {
    assert(seed_len == BIT_SECURITY_LEVEL / 8);
    memcpy(prng->state, seed, seed_len);
}

// Read function
forceinline void prng_read(prng_t* prng, uint8_t* bytes, size_t len) {
    // Simplified random generation, just copying state in a loop
    size_t state_len = BIT_SECURITY_LEVEL / 8;
    for (size_t i = 0; i < len; i++) {
        bytes[i] = prng->state[i % state_len];
    }
}

#endif // ML_KEM_PRNG_H


// #ifndef ML_KEM_PRNG_H
// #define ML_KEM_PRNG_H

// // #include "shake256.h"
// #include <stdint.h>
// #include <string.h>
// #include <limits>
// #include <stdlib.h>

// // Define the allowed security levels.
// #define ML_KEM_SECURITY_LEVEL_128 128
// #define ML_KEM_SECURITY_LEVEL_192 192
// #define ML_KEM_SECURITY_LEVEL_256 256

// typedef struct {
//     shake256_t state; // State of the SHAKE256
// } prng_t;

// // Function to seed the PRNG with random bytes from std::random_device.
// void prng_init_default(prng_t *prng) {
//     uint8_t seed[ML_KEM_SECURITY_LEVEL_256 / 8]; // Max security level is 256
//     size_t seed_len = sizeof(seed);

//     // Fill seed with random bytes (using /dev/urandom or equivalent)
//     FILE *fp = fopen("/dev/urandom", "rb");
//     if (fp != NULL) {
//         fread(seed, 1, seed_len, fp);
//         fclose(fp);
//     } else {
//         // Fallback to using rand() if /dev/urandom is not available
//         for (size_t i = 0; i < seed_len; i++) {
//             seed[i] = (uint8_t)(rand() % 256);
//         }
//     }

//     shake256_absorb(&prng->state, seed, seed_len);
//     shake256_finalize(&prng->state);
// }

// // Function to seed the PRNG with a specific seed.
// void prng_init_seed(prng_t *prng, const uint8_t *seed, size_t seed_len) {
//     shake256_absorb(&prng->state, seed, seed_len);
//     shake256_finalize(&prng->state);
// }

// // Function to read random bytes from the PRNG.
// void prng_read(prng_t *prng, uint8_t *bytes, size_t len) {
//     shake256_squeeze(&prng->state, bytes, len);
// }

// #endif // ML_KEM_PRNG_H