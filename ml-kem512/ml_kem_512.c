#include"ml_kem_512.h"
#include"k_pke.h"
#include"k_pke_utils.h"
#include"prg.h"
#include "aes.h" // Include the header file for AES_ctx
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


int ml_kem_512_encapsulate(const uint8_t* pubkey, uint8_t* cipher, uint8_t* shared_secret){
    uint8_t m[ML_KEM_512_SEED_M_BYTE_LEN];
    prg(m , 32, 123456789);  // wil add more secure random bytes generation...
    ml_kem_512_encapsulate_internal(m , pubkey, cipher, shared_secret);
    return 0;
}

int ml_kem_512_encapsulate_internal(uint8_t* m, const uint8_t* pubkey, uint8_t* cipher, uint8_t* shared_secret){
    // (k, r )from G(m || H(ek));
    // c <- k-pke.enncript(ek,m,r)
    //return K and c
    uint8_t r[32];
    uint8_t temp_H[32];
    FIPS202_SHA3_256(pubkey, ML_KEM_512_PKEY_BYTE_LEN, temp_H);
    uint8_t concatenated[ML_KEM_512_SEED_D_BYTE_LEN + 32];
    for (size_t i = 0; i < ML_KEM_512_SEED_D_BYTE_LEN; i++) {
        concatenated[i] = m[i];
    }
    for (size_t i = 0; i < 32; i++) {
        concatenated[ML_KEM_512_SEED_D_BYTE_LEN + i] = temp_H[i];
    }
    uint8_t sha3_output[64];
    FIPS202_SHA3_512(concatenated, sizeof(concatenated), sha3_output);  // 64-byte
    for (size_t i = 0; i < 32; i++) {
        r[i] = sha3_output[i];
        shared_secret[i] = sha3_output[32 + i];
    }
    printf("Shared secret ():\n");
    for (int i = 0; i < 32; i++) {
        printf("%x ", shared_secret[i]);
    }
    printf("\n");

    printf("r ():\n");
    for (int i = 0; i < 32; i++) {
        printf("%x ", r[i]);
    }

    k_pke_encrypt(pubkey, m, r, cipher);

    return 0;
}

void ml_kem_512_keygen_internal(uint8_t* d, uint8_t* z, uint8_t* pubKey, uint8_t* seckey){
    // create some variables:
    printf("%d",ML_KEM_512_PKEY_BYTE_LEN);
    uint8_t pke_public_key[K_PKE_PUB_KEY_LEN]; // will be k * 12 * 32 + 32;
    uint8_t pke_secret_key[K_PKE_SEC_KEY_LEN]; // will be k * 12 * 32;
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


// we create some function to call for the encriptions...
void Tripel_AES_init_ctx(struct Tripel_AES_ctx* ctx, const uint8_t* key1, const uint8_t* key2){
    AES_init_ctx(&ctx->ctx1, key1);
    AES_init_ctx(&ctx->ctx2, key2);
}

void Tripel_AES_ECB_encrypt(struct Tripel_AES_ctx* ctx, uint8_t* in){
    AES_ECB_encrypt(&ctx->ctx1, in);
    AES_ECB_decrypt(&ctx->ctx2, in);
    AES_ECB_encrypt(&ctx->ctx1, in);
}
void Tripel_AES_ECB_decrypt(struct Tripel_AES_ctx* ctx, uint8_t* in){
    AES_ECB_decrypt(&ctx->ctx1, in);
    AES_ECB_encrypt(&ctx->ctx2, in);
    AES_ECB_decrypt(&ctx->ctx1, in);
}
void Encrypt_string(char* str, struct Tripel_AES_ctx ctx){
    size_t len = strlen(str);
    for(size_t i=0;i<len;i+=16){
        Tripel_AES_ECB_encrypt(&ctx, (uint8_t*)str+i);
    }
}
void Decrypt_string(char* str, struct Tripel_AES_ctx ctx){
    size_t len = strlen(str);
    for(size_t i=0;i<len;i+=16){
        Tripel_AES_ECB_decrypt(&ctx, (uint8_t*)str+i);
    }
}




/*remove this later/... */
void print_array(uint16_t* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    // testing the aes...
    
    // we will be sharing a secrate with the ml-kem then usiing that encryption scheme to create tripel AES.


    uint8_t key1[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t key2[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x31 };
    // uint8_t check_on[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
    // uint8_t in[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
    struct Tripel_AES_ctx ctx_tripel;
    Tripel_AES_init_ctx(&ctx_tripel, key1, key2);
    char input_str[256]; 
    printf("Enter a string: ");
    scanf("%255s", input_str);  
    Encrypt_string(input_str, ctx_tripel);
    printf("the encrypted string is: %s\n", input_str);
    Decrypt_string(input_str, ctx_tripel);
    printf("the decrypted string is: %s\n", input_str);


    // uint8_t pubkey[ML_KEM_512_PKEY_BYTE_LEN]; // encapsulating key
    // uint8_t seckey[ML_KEM_512_SKEY_BYTE_LEN]; // decapsulating key
    // ml_kem_512_keygen(pubkey, seckey);
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
    // uint8_t shared_secret[ML_KEM_512_SHARED_SECRET_BYTE_LEN];
    // uint8_t cipherText[ML_KEM_512_CIPHER_TEXT_BYTE_LEN];
    // ml_kem_512_encapsulate(pubkey, shared_secret, cipherText);
    return 0;
}
