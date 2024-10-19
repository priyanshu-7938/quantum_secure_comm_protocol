#ifndef K_PKE_UTILS_H
#define K_PKE_UTILS_H

#include <stdint.h>
#include <string.h> 

//some declarations for the XOF

// SHAKE128 Context Structure

#define SHAKE128_RATE 1344 // SHAKE128 rate in bits (168 bytes)

typedef struct {
    uint8_t state[200];     // The state for the sponge function (1600 bits)
    unsigned int rateInBytes; // Rate in bytes (SHAKE128 uses 168 bytes)
    unsigned int blockSize;   // Current block size (how much of the block is filled)
} XOF_CTX; // this in the backside is implementing the Shake128!!

XOF_CTX* XOF_Init();

void XOF_Absorb(XOF_CTX *ctx, const uint8_t *input, size_t inputByteLen);

void XOF_Squeeze(XOF_CTX *ctx, uint8_t *output, unsigned int blockSize);


// a 32 byte long input plus 2 indecies...
void SampleNTT(uint8_t* input, uint16_t* A_ij);// 34 bytes length of input, 256 byte length of A
//s -> is the row, single_byte_b is the N, in defination.
void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t length_of_output, uint8_t eta);

void SamplePolyCBD(uint16_t* output, uint8_t* B,size_t len_of_B, uint8_t eta);

void NTT(uint16_t* transformed, uint16_t* from);
#endif // K_PKE_UTILS_H
