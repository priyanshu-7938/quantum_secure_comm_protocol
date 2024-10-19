#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ml_kem/ml_kem_768.h" // Assuming this is converted to a C-compatible header

// Convert a byte array to a hex string
static inline void to_hex(const uint8_t* bytes, size_t len, char* hex_output)
{
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_output + (i * 2), "%02x", bytes[i]);
    }
    hex_output[len * 2] = '\0';
}

int main()
{
    // Seed arrays for keypair generation
    uint8_t d[ML_KEM_768_SEED_D_BYTE_LEN] = {0};
    uint8_t z[ML_KEM_768_SEED_Z_BYTE_LEN] = {0};

    // Public/Private keypair
    uint8_t pkey[ML_KEM_768_PKEY_BYTE_LEN] = {0};
    uint8_t skey[ML_KEM_768_SKEY_BYTE_LEN] = {0};

    // Seed and cipher arrays for key encapsulation
    uint8_t m[ML_KEM_768_SEED_M_BYTE_LEN] = {0};
    uint8_t cipher[ML_KEM_768_CIPHER_TEXT_BYTE_LEN] = {0};

    // Shared secret keys
    uint8_t sender_key[ML_KEM_768_SHARED_SECRET_BYTE_LEN] = {0};
    uint8_t receiver_key[ML_KEM_768_SHARED_SECRET_BYTE_LEN] = {0};

    // Pseudo-randomness source (assume prng_t is implemented for C)
    prng_t prng;
    prng_init(&prng, 128); // Initialize PRNG with 128-bit seed or similar method

    // Fill up seeds using PRNG
    prng_read(&prng, d, ML_KEM_768_SEED_D_BYTE_LEN);
    prng_read(&prng, z, ML_KEM_768_SEED_Z_BYTE_LEN);

    // Generate keypair
    ml_kem_768_keygen(d, z, pkey, skey);

    // Fill up seed required for key encapsulation using PRNG
    prng_read(&prng, m, ML_KEM_768_SEED_M_BYTE_LEN);

    // Encapsulate key, compute cipher text, and obtain KDF
    int is_encapsulated = ml_kem_768_encapsulate(m, pkey, cipher, sender_key);
    
    // Decapsulate cipher text and obtain KDF
    ml_kem_768_decapsulate(skey, cipher, receiver_key);

    // Check that both of the communicating parties arrived at the same shared secret key
    assert(memcmp(sender_key, receiver_key, ML_KEM_768_SHARED_SECRET_BYTE_LEN) == 0);

    // Convert to hex strings for output
    char pkey_hex[ML_KEM_768_PKEY_BYTE_LEN * 2 + 1];
    char skey_hex[ML_KEM_768_SKEY_BYTE_LEN * 2 + 1];
    char cipher_hex[ML_KEM_768_CIPHER_TEXT_BYTE_LEN * 2 + 1];
    char sender_key_hex[ML_KEM_768_SHARED_SECRET_BYTE_LEN * 2 + 1];

    to_hex(pkey, ML_KEM_768_PKEY_BYTE_LEN, pkey_hex);
    to_hex(skey, ML_KEM_768_SKEY_BYTE_LEN, skey_hex);
    to_hex(cipher, ML_KEM_768_CIPHER_TEXT_BYTE_LEN, cipher_hex);
    to_hex(sender_key, ML_KEM_768_SHARED_SECRET_BYTE_LEN, sender_key_hex);

    // Output results
    printf("ML-KEM-768\n");
    printf("Pubkey         : %s\n", pkey_hex);
    printf("Seckey         : %s\n", skey_hex);
    printf("Encapsulated ? : %s\n", is_encapsulated ? "true" : "false");
    printf("Cipher         : %s\n", cipher_hex);
    printf("Shared secret  : %s\n", sender_key_hex);

    return 0;
}
