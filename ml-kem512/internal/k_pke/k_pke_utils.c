#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include"k_pke_utils.h"
#include"utils.h"
#include"Compact_FIPS202.h"
#include"ml_kem_512.h"



const int16_t zetas[128] = {
    -1044, -758,  -359,  -1517, 1493,  1422,  287,   202,  -171,  622,   1577,
    182,   962,   -1202, -1474, 1468,  573,   -1325, 264,  383,   -829,  1458,
    -1602, -130,  -681,  1017,  732,   608,   -1542, 411,  -205,  -1571, 1223,
    652,   -552,  1015,  -1293, 1491,  -282,  -1544, 516,  -8,    -320,  -666,
    -1618, -1162, 126,   1469,  -853,  -90,   -271,  830,  107,   -1421, -247,
    -951,  -398,  961,   -1508, -725,  448,   -1065, 677,  -1275, -1103, 430,
    555,   843,   -1251, 871,   1550,  105,   422,   587,  177,   -235,  -291,
    -460,  1574,  1653,  -246,  778,   1159,  -147,  -777, 1483,  -602,  1119,
    -1590, 644,   -872,  349,   418,   329,   -156,  -75,  817,   1097,  603,
    610,   1322,  -1285, -1465, 384,   -1215, -136,  1218, -1335, -874,  220,
    -1187, -1659, -1185, -1530, -1278, 794,   -1510, -854, -870,  478,   -108,
    -308,  996,   991,   958,   -1460, 1522,  1628};


void SampleNTT(uint8_t* input, uint16_t* A_ij){
    XOF_CTX *ctx = XOF_Init();
    XOF_Absorb(ctx, input, 34); // length of the 34 bytes is fixed by standard....
    size_t j =0;
    uint8_t squeezed_C[3]={0}; //3 is standard fixed
    while(j<256){
        XOF_Squeeze(ctx, squeezed_C, 3);
        uint16_t d1 = squeezed_C[0] + 256*(squeezed_C[1]%16); // always going to be between: 0 - 2^12
        uint16_t d2 = (uint8_t)(squeezed_C[1]/16) + 16*squeezed_C[2]; // same it is between: 0 - 2^12
        if(d1<ML_KEM_PRIME_Q){
            A_ij[j++] = d1 % ML_KEM_PRIME_Q;
        }
        if(d2<ML_KEM_PRIME_Q && j<256){
            A_ij[j++]= d2 % ML_KEM_PRIME_Q;
        }
    }
}
// implemnet here.... CURRENT

void NTT(uint16_t* transformed, uint16_t* from){
    for(size_t i=0;i<256; i++){
        transformed[i] = from[i];
    }
    uint8_t k=1,t;
    uint16_t zeta;
    for(size_t len=128;len>=2;len>>=1){
        for(size_t start=0;start<256;start+=2*len){
            zeta = zetas[k++];
            for(size_t j = start;j< start+len;j++){
                t = (((uint32_t)transformed[j+len] * (uint32_t)zeta) % ML_KEM_PRIME_Q);
                transformed[j+len] = (transformed[j] - t + ML_KEM_PRIME_Q) % ML_KEM_PRIME_Q;
                transformed[j] = ((uint32_t)transformed[j] + t)% ML_KEM_PRIME_Q;
            }
        }
    }
}

void NTT_inv(uint16_t* transformed, uint16_t* from){
    for(size_t i=0;i<256; i++){
        transformed[i] = from[i];
    }
    uint8_t k=127,t;
    uint16_t zeta;
    for(size_t len=2;len<=128;len<<=1){
        for(size_t start=0;start<256;start+=2*len){
            zeta = zetas[k--];
            for(size_t j = start;j< start+len;j++){
                t =transformed[j];
                transformed[j] = ((uint32_t)transformed[j+len] + t) % ML_KEM_PRIME_Q;
                transformed[j+len] = ((uint32_t)zeta*(transformed[j+len] - t)) % ML_KEM_PRIME_Q;
            }
        }
    }
    for(size_t i=0;i<256; i++){
        transformed[i] = ((uint32_t)transformed[i] * 3303)% ML_KEM_PRIME_Q;
    }
}







// output is a 256 array 
void SamplePolyCBD(uint16_t* output, uint8_t* B,size_t len_of_B, uint8_t eta){
    // here the 
    uint8_t bits[len_of_B*8];
    BytesToBits(B, bits, len_of_B);
    for(size_t i=0;i<256;i++){
        uint8_t x=0,y=0;
        for(size_t j=0;j<eta;j++){
            x+=bits[2*i*eta+j];
            y+=bits[2*i*eta+j+eta];
            output[i] = ((uint16_t)(x-y)) % ML_KEM_PRIME_Q;
        }
    }

}

void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t length_of_output, uint8_t eta){
    // s||b is a  33 
    //implements the shake256( s||b,64*n) n = {2,3}
    if(length_of_output != 64*eta){printf("error in PRF function...");return;}
    uint8_t input[33];
    for(size_t i=0;i<32;i++)input[i]=s[i];
    input[31]=*single_byte_b;
    FIPS202_SHAKE256(input, length_of_s, output, length_of_output);
    return;
    //toddo here....
}

XOF_CTX* XOF_Init() {
    static XOF_CTX ctx;  // Using static to ensure ctx is not destroyed after function returns
    memset(ctx.state, 0, sizeof(ctx.state));  // Zero the state
    ctx.rateInBytes = SHAKE128_RATE / 8;      // 168 bytes rate for SHAKE128
    ctx.blockSize = 0;                        // No data absorbed yet
    return &ctx;  // Return the pointer to the context
}

void XOF_Absorb(XOF_CTX *ctx, const uint8_t *input, size_t inputByteLen) {
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

void XOF_Squeeze(XOF_CTX *ctx, uint8_t *output, unsigned int blockSize) {
    unsigned int i;
    // unsigned int blockSize = 3;  // We are squeezing out 3 bytes

    // Copy 3 bytes from the state to the output
    for (i = 0; i < blockSize; i++) {
        output[i] = ctx->state[i];
    }

    // Apply the permutation to get new squeezed output for future calls
    KeccakF1600_StatePermute(ctx->state);
}



// int main() {
//     // Step 1: Initialize XOF context
//     XOF_CTX *ctx = XOF_Init();

//     // Step 2: Absorb a 34-byte input array
//     uint8_t input[34] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
//                          0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22};
//     XOF_Absorb(ctx, input);

//     // Step 3: Squeeze 3 bytes into the output pointer
//     uint8_t squeezed_output[3];
//     for(size_t i=0;i<4;i++){
//         XOF_Squeeze(ctx, squeezed_output, 3);
//         printf("%02X, %02X, %02X\n",squeezed_output[0],squeezed_output[1],squeezed_output[2]);
//     }

//     // Print the squeezed output
//     // printf("Squeezed output (3 bytes):\n");
//     // for (int i = 0; i < 3; i++) {
//     //     printf("%02x ", squeezed_output[i]);
//     // }
//     printf("\n");

//     return 0;
// }