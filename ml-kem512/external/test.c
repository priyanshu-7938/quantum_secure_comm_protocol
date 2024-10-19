#include <stdint.h>
#include <string.h>  // for memset and memcpy
#include"Compact_FIPS202.h"
#include<stdio.h>
#define SHAKE128_RATE 1344 // SHAKE128 rate in bits (168 bytes)

// SHAKE128 Context Structure
typedef struct {
    uint8_t state[200];     // The state for the sponge function (1600 bits)
    unsigned int rateInBytes; // Rate in bytes (SHAKE128 uses 168 bytes)
    unsigned int blockSize;   // Current block size (how much of the block is filled)
} SHAKE128_CTX;

// Initializes the SHAKE128 context (XOF.Init())
void SHAKE128_Init(SHAKE128_CTX *ctx) {
    // Initialize the state and set the rate for SHAKE128
    memset(ctx->state, 0, sizeof(ctx->state));  // Zero the state
    ctx->rateInBytes = SHAKE128_RATE / 8;       // 168 bytes rate for SHAKE128
    ctx->blockSize = 0;                         // No data absorbed yet
}

// Absorbs input into the SHAKE128 context (XOF.Absorb(ctx, str))
void SHAKE128_Absorb(SHAKE128_CTX *ctx, const uint8_t *input, size_t inputByteLen) {
    unsigned int i;
    unsigned int blockSize;

    // Absorb input into the state one block at a time
    while (inputByteLen > 0) {
        blockSize = ctx->rateInBytes - ctx->blockSize;
        blockSize = (blockSize > inputByteLen) ? inputByteLen : blockSize;
        
        for (i = 0; i < blockSize; i++) {
            ctx->state[ctx->blockSize + i] ^= input[i];  // XOR input into the state
        }
        
        input += blockSize;
        inputByteLen -= blockSize;
        ctx->blockSize += blockSize;

        // When the rate portion is full, apply the permutation
        if (ctx->blockSize == ctx->rateInBytes) {
            KeccakF1600_StatePermute(ctx->state);  // Permutation
            ctx->blockSize = 0;  // Reset blockSize for the next block
        }
    }
}

// Squeezes output from the SHAKE128 context (XOF.Squeeze(ctx, ℓ))
void SHAKE128_Squeeze(SHAKE128_CTX *ctx, uint8_t *output, size_t outputByteLen) {
    unsigned int i;
    unsigned int blockSize;

    // Squeeze the required output
    while (outputByteLen > 0) {
        blockSize = (outputByteLen > ctx->rateInBytes) ? ctx->rateInBytes : outputByteLen;

        // Copy from the state to the output
        for (i = 0; i < blockSize; i++) {
            output[i] = ctx->state[i];
        }

        output += blockSize;
        outputByteLen -= blockSize;

        // Apply the permutation if more output is needed
        if (outputByteLen > 0) {
            KeccakF1600_StatePermute(ctx->state);  // Permutation
        }
    }
}

// Example Usage: Initialize, Absorb, and Squeeze (SHAKE128 implementation)
int main() {
    SHAKE128_CTX ctx;
    uint8_t output[32];       // Example output: 32 bytes
    uint8_t input[] = "Example input string"; // Example input
    size_t inputLen = strlen((char*)input);

    // Initialize SHAKE128 context
    SHAKE128_Init(&ctx);

    // Absorb input string into SHAKE128 context
    SHAKE128_Absorb(&ctx, input, inputLen);

    // Squeeze 32 bytes of output
    SHAKE128_Squeeze(&ctx, output, 32);

    // Print the result
    printf("SHAKE128 Squeeze Output (32 bytes):\n");
    for (int i = 0; i < 32; i++) {
        printf("%02x", output[i]);
    }
    printf("\n");

    return 0;
}

