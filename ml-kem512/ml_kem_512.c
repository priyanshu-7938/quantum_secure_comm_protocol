#include"ml_kem_512.h"
#include"k_pke.h"
#include"prg.h"
// #include"sha3_256.hpp" // for the hash function...
#include"Compact_FIPS202.h"

// d and z are array or 32 size each being having value of 8bits or one byte.
//flag mmeans we have to generat eht d and z
//public key aka encapsulationkey
void ml_kem_512_keygen(uint8_t* pubkey, uint8_t* seckey) {
    uint8_t d[ML_KEM_512_SEED_D_BYTE_LEN];
    uint8_t z[ML_KEM_512_SEED_Z_BYTE_LEN];
    if (pubkey == NULL || seckey == NULL) {
        fprintf(stderr, "Error: Null pointer passed to ml_kem_512_keygen\n");
        return;
    }
    // here we generate the d and z.... 
    prg((uint8_t*)d, ML_KEM_512_SEED_D_BYTE_LEN, 4294001296);
    prg((uint8_t*)z, ML_KEM_512_SEED_Z_BYTE_LEN, 3144960123);

    //appplying internal keygen.
    ml_kem_512_keygen_internal(d,z, pubkey, seckey);
}

/*static inline size_t get_kem_secret_key_len(size_t k) {
    return K_PKE_SEC_KEY_LEN(k) + get_pke_public_key_len(k) + 32 + 32;
} */
void ml_kem_512_keygen_internal(uint8_t* d, uint8_t* z, uint8_t* pubKey, uint8_t* seckey){
    // create some variables:
    printf("%d",ML_KEM_512_PKEY_BYTE_LEN);
    uint8_t pke_public_key[K_PKE_PUB_KEY_LEN]; // will be k * 12 * 32 + 32;
    uint8_t pke_secret_key[K_PKE_SEC_KEY_LEN]; // will be k * 12 * 32;
/*
 remove this later
*/
    for (size_t i = 0; i < K_PKE_PUB_KEY_LEN; i++) {
        pke_public_key[i] = 1;
    }
    for (size_t i = 0; i < K_PKE_SEC_KEY_LEN; i++) {
        pke_secret_key[i] = 2;
    }
/*
 till here
*/

    k_pke_keygen(d, pke_public_key, pke_secret_key);
    // making changes..
    for(size_t i=0 ;i< ML_KEM_512_PKEY_BYTE_LEN ;i++){
        pubKey[i] = pke_public_key[i];
    }
    //filling the sec key which is going to be:         pke_sec | pke_pub| h(pub) | z
    // getting the lengths of each of the vairables:    K_PKE_SEC_KEY_LEN | get_pke_public_key_len | 32 | 32

    uint8_t hash_pubkey[32] = {0};
    FIPS202_SHA3_256(pubKey, ML_KEM_512_PKEY_BYTE_LEN, hash_pubkey);
    printf("\n\n");
    for(size_t i=0 ; i< K_PKE_SEC_KEY_LEN ;i++){
        seckey[i] = pke_secret_key[i];
    }
    for(size_t i=K_PKE_SEC_KEY_LEN ; i< (K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN) ; i++){
        seckey[i] = pke_public_key[i- K_PKE_SEC_KEY_LEN];
    }
    for(size_t i=(K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN) ; i< (K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN + 32); i++){
        seckey[i] = hash_pubkey[i-(K_PKE_SEC_KEY_LEN+ K_PKE_PUB_KEY_LEN)];
    }
    for(size_t i=(K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN + 32) ; i< (K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN + 32 + 32); i++){
        seckey[i] = z[i-(K_PKE_SEC_KEY_LEN + K_PKE_PUB_KEY_LEN +32)];
    }
}


int main() {
    uint8_t pubkey[ML_KEM_512_PKEY_BYTE_LEN];
    uint8_t seckey[ML_KEM_512_SKEY_BYTE_LEN];
    printf("yo");
    ml_kem_512_keygen(pubkey, seckey);
    //  printf("Public key (first 16 bytes):\n");
    // for (int i = 0; i < ML_KEM_512_PKEY_BYTE_LEN; i++) {
    //     printf("%02X ", pubkey[i]);
    // }
    // printf("\n");

    // printf("Secret key (first 16 bytes):\n");
    // for (int i = 0; i < ML_KEM_512_SKEY_BYTE_LEN; i++) {
    //     printf("%02X ", seckey[i]);
    // }
    // printf("\n");

    return 0;
}
