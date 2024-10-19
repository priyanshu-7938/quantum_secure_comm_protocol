#ifndef ML_KEM_768_H
#define ML_KEM_768_H

#include <stddef.h>
#include <stdint.h>

// ML-KEM-768 constants
#define ML_KEM_768_K 3
#define ML_KEM_768_ETA1 2
#define ML_KEM_768_ETA2 2
#define ML_KEM_768_DU 10
#define ML_KEM_768_DV 4

// Lengths of various byte arrays
#define ML_KEM_768_SEED_D_BYTE_LEN 32
#define ML_KEM_768_SEED_Z_BYTE_LEN 32
#define ML_KEM_768_PKEY_BYTE_LEN get_kem_public_key_len(ML_KEM_768_K)
#define ML_KEM_768_SKEY_BYTE_LEN get_kem_secret_key_len(ML_KEM_768_K)
#define ML_KEM_768_SEED_M_BYTE_LEN 32
#define ML_KEM_768_CIPHER_TEXT_BYTE_LEN get_kem_cipher_text_len(ML_KEM_768_K, ML_KEM_768_DU, ML_KEM_768_DV)
#define ML_KEM_768_SHARED_SECRET_BYTE_LEN 32

// Function prototypes

// Generates a new ML-KEM-768 keypair, given seed `d` and `z`.
void ml_kem_768_keygen(const uint8_t* d, const uint8_t* z, uint8_t* pubkey, uint8_t* seckey);

// Encapsulates a key and computes a cipher text and shared secret. 
// Returns 1 on success, 0 if the public key is malformed.
int ml_kem_768_encapsulate(const uint8_t* m, const uint8_t* pubkey, uint8_t* cipher, uint8_t* shared_secret);

// Decapsulates a cipher text and computes the shared secret.
void ml_kem_768_decapsulate(const uint8_t* seckey, const uint8_t* cipher, uint8_t* shared_secret);

#endif // ML_KEM_768_H
