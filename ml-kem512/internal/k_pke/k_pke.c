#include"k_pke.h"
#include"k_pke_utils.h"
#include"ml_kem_512.h"
#include"Compact_FIPS202.h"
#include<stdio.h>

void k_pke_keygen(uint8_t* d, uint8_t* pke_public_key, uint8_t* pke_secret_key){
    // creating row and sigma whai is ashaken by 33 bytes to 64 bytes....
    uint8_t output[64];
    uint8_t input[33];
    for(size_t i=0;i<32;i++){
        input[i] = d[i];
    }input[32] = (uint8_t)ML_KEM_512_K;
    
    FIPS202_SHAKE256(input, 32+ ML_KEM_512_K, output, 64);
    uint8_t row[32];
    uint8_t sigma[32];
    for(size_t i = 0;i<32;i++){
        row[i] = output[i];
        sigma[i] = output[32+i];
    }
    // printf("row: ");
    // for(size_t i = 0;i<32;i++){
    //     printf("%02X, ",row[i]);
    // }
    // printf("\n\nsigma: ");
    // for(size_t i = 0;i<32;i++){
    //     printf("%02X, ",sigma[i]);
    // }
    printf("\n\n");
    // line 1 done
    uint8_t N = 0;
    uint16_t A[ML_KEM_512_K][ML_KEM_512_K][256]; // Z_256 ploynomial written in k x k mod q. 
    uint8_t B_34[34]={0}; // b_34 = row || j || i
    for(size_t i=0;i<32;i++)B_34[i] = row[i];
    for(size_t i = 0;i< ML_KEM_512_K;i++){
        for(size_t j=0;j<ML_KEM_512_K;j++){
            B_34[32] = j;
            B_34[33] = i;
            SampleNTT(B_34,A[i][j]);
        }
    }
    // for(size_t i = 0;i< ML_KEM_512_K;i++){
    //     for(size_t j=0;j<ML_KEM_512_K;j++){
    //         for(size_t k = 0; k<256;k++){
    //             printf("0x%04hx ",A[i][j][k]);
    //         }
    //         printf("\n ");
    //     }
    // }

    // size_t length_of_output = 64*2;
    // uint8_t prfOutput_64n[length_of_output]; // eta is 2 here.
    // PRF(sigma, 32, &N, prfOutput_64n, length_of_output, 2);
    // for(size_t i = 0;i<length_of_output;i++){
    //     printf("%02X ",prfOutput_64n[i]);
    // }



    uint16_t s[ML_KEM_512_K][256]; // k polynomials in z_256 with modulo q
    for(size_t i=0; i<ML_KEM_512_K; i++){
        //void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t* length_of_output, uint8_t eta)
        size_t len_of_prfOutput = 64*2;
        uint8_t prfOutput_64n[len_of_prfOutput]; // eta is 2 here.
        PRF(sigma, 32, &N, prfOutput_64n, len_of_prfOutput, 2);
        SamplePolyCBD(s[i],prfOutput_64n, len_of_prfOutput, 2);//output in the s[i], eta is 2 again..
        N++;
    }
    // printf("\nvalue of S:");
    // for(size_t i = 0;i< ML_KEM_512_K;i++){
    //     for(size_t k = 0; k<256;k++){
    //         printf("0x%04hx ",s[i][k]);
    //     }
    //     printf("\n ");
    // }

    uint16_t e[ML_KEM_512_K][256]; // k polynomials in z_256 with modulo q
    for(size_t i=0; i<ML_KEM_512_K; i++){
        //void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t* length_of_output, uint8_t eta)
        size_t len_of_prfOutput = 64*2;
        uint8_t prfOutput_64n[len_of_prfOutput]; // eta is 2 here.
        PRF(sigma, 32, &N, prfOutput_64n, len_of_prfOutput, 2);
        SamplePolyCBD(e[i],prfOutput_64n, len_of_prfOutput, 2);//output in the s[i], eta is 2 again..
        N++;
    }
    // printf("\nvalue of E:");
    // for(size_t i = 0;i< ML_KEM_512_K;i++){
    //     for(size_t k = 0; k<256;k++){
    //         printf("0x%04hx ",e[i][k]);
    //     }
    //     printf("\n ");
    // }
    // implemne t ntt
    uint16_t s_cap[ML_KEM_512_K][256]; // k polynomials in z_256 with modulo q
    uint16_t e_cap[ML_KEM_512_K][256];
    for(size_t i=0; i<ML_KEM_512_K;i++){
        NTT(s_cap[i],s[i]);
        NTT(e_cap[i],e[i]);
    }


    return;
    
    // uint8_t e[ML_KEM_512_K];
    // for(size_t i=0; i<ML_KEM_512_K; i++){
    //     e[i] = SamplePolyCBD(PRF(sigma,N))
    //     n++;
    // }
    // // uint8_t S_cap[] = NTT(s);// this is going to be k times on this
    // // uint8_t E_cap[] = NTT(e);// this is going to be k times on this
    // // T_cap = A . s_cap + E_cap
    // // ekPKE = ByteEncoding(T_cap)||row // public part
    // // dkPKE = ByteEncoding(T_cap) // private part

}