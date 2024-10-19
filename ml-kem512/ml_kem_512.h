#ifndef ML_KEM_512_H
#define ML_KEM_512_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include"ml_kem_utils.h"


// ML-KEM-512 constants
#define ML_KEM_512_K 3
#define ML_KEM_512_ETA1 2
#define ML_KEM_512_ETA2 2
#define ML_KEM_512_DU 10
#define ML_KEM_512_DV 4
#define ML_KEM_PRIME_Q 3329 // 2^8*13+1

// Lengths of various byte arrays
#define ML_KEM_512_SEED_D_BYTE_LEN ((size_t)32)
#define ML_KEM_512_SEED_Z_BYTE_LEN ((size_t)32)
#define ML_KEM_512_PKEY_BYTE_LEN get_kem_public_key_len(ML_KEM_512_K)
#define ML_KEM_512_SKEY_BYTE_LEN get_kem_secret_key_len(ML_KEM_512_K)
#define ML_KEM_512_SEED_M_BYTE_LEN ((size_t)32)
#define ML_KEM_512_CIPHER_TEXT_BYTE_LEN get_kem_cipher_text_len(ML_KEM_512_K, ML_KEM_512_DU, ML_KEM_512_DV)
#define ML_KEM_512_SHARED_SECRET_BYTE_LEN ((size_t)32)

// Function prototypes

// Generates a new ML-KEM-512 keypair, given seed `d` and `z`.
//d and xz are randomly generated numbers or moreover: a 32 byte seed each.
void ml_kem_512_keygen(uint8_t* pubkey, uint8_t* seckey);
//this interanaly calls the k-pke module for the keygen function...
void ml_kem_512_keygen_internal(uint8_t* d, uint8_t* z, uint8_t* pubKey, uint8_t* seckey);

// Encapsulates a key and computes a cipher text and shared secret. 
// Returns 1 on success, 0 if the public key is malformed.
int ml_kem_512_encapsulate(const uint8_t* m, const uint8_t* pubkey, uint8_t* cipher, uint8_t* shared_secret);

// Decapsulates a cipher text and computes the shared secret.
void ml_kem_512_decapsulate(const uint8_t* seckey, const uint8_t* cipher, uint8_t* shared_secret);

#endif // ML_KEM_512_H
