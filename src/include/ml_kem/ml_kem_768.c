#include "ml_kem_768.h"
#include "ml_kem_utils.h"  // Assuming this is a utility file needed for ML-KEM

void ml_kem_768_keygen(const uint8_t* d, const uint8_t* z, uint8_t* pubkey, uint8_t* seckey) {
    // Call the internal ML-KEM keygen function, ensuring compatibility with C.
    ml_kem_keygen(ML_KEM_768_K, ML_KEM_768_ETA1, d, z, pubkey, seckey);
}

int ml_kem_768_encapsulate(const uint8_t* m, const uint8_t* pubkey, uint8_t* cipher, uint8_t* shared_secret) {
    // Call the internal ML-KEM encapsulate function.
    return ml_kem_encapsulate(ML_KEM_768_K, ML_KEM_768_ETA1, ML_KEM_768_ETA2, ML_KEM_768_DU, ML_KEM_768_DV, m, pubkey, cipher, shared_secret);
}

void ml_kem_768_decapsulate(const uint8_t* seckey, const uint8_t* cipher, uint8_t* shared_secret) {
    // Call the internal ML-KEM decapsulate function.
    ml_kem_decapsulate(ML_KEM_768_K, ML_KEM_768_ETA1, ML_KEM_768_ETA2, ML_KEM_768_DU, ML_KEM_768_DV, seckey, cipher, shared_secret);
}
 