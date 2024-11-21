#ifndef K_PKE_H
#define K_PKE_H

#include"ml_kem_utils.h"
// #include"ml_kem_512.h" to be added if error

#define K_PKE_PUB_KEY_LEN get_pke_public_key_len(ML_KEM_512_K)
#define K_PKE_SEC_KEY_LEN get_pke_secret_key_len(ML_KEM_512_K)

void k_pke_keygen(uint8_t* d, uint8_t* pke_public_key, uint8_t* pke_secret_key);
void k_pke_encrypt(uint8_t* pubkey, uint8_t* m, uint8_t* r,uint8_t* cipher);

#endif // K_PKE_H