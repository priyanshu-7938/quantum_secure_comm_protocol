#include"k_pke.h"
#include"k_pke_utils.h"
#include"ml_kem_512.h"
#include"utils.h"
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

    // here....
    uint16_t t[ML_KEM_512_K][256];

    /*
        A [ 1 2 3 
            4 5 6
            7 8 9 ]

     */
    // for(size_t j=0;j<ML_KEM_512_K;j++){
    //     for(size_t i=0;i< 256;i++){
    //         t[j][i] =  
    //     }
    // }
    for (int i = 0; i < ML_KEM_512_K; i++) {
        for (int j = 0; j < 256; j++) {
            t[i][j] = 0; // Initialize t[i][j]
            for (int k = 0; k < ML_KEM_512_K; k++) {
                t[i][j] += (A[i][k][j] * s[k][j]) % ML_KEM_PRIME_Q; // Element-wise multiplication
            }
            t[i][j] = (t[i][j] + e[i][j]) % ML_KEM_PRIME_Q; // Add noise and mod q
        }
    }
    

    uint8_t temp_encoded[32 * 12]; // len of private key  // Temporary array to hold the encoded result for each row

    int pke_offset = 0;  // To keep track of the offset for the pke array
    
    for (int k = 0; k < ML_KEM_512_K; k++) {
        // Run ByteEncode on row F[k]
        ByteEncode(t[k], temp_encoded, 12); // we have to run with 12 as in standard defination..

        for (int i = 0; i < 32 * 12; i++) {
            pke_public_key[pke_offset + i] = temp_encoded[i];
        }
        pke_offset += 32 * 12;  // Update the offset
    }
    for (int i = 0; i < 32; i++) {
        pke_public_key[pke_offset + i] = row[i];
    } // added row at the end.
    pke_offset = 0;
    for (int k = 0; k < ML_KEM_512_K; k++) {
        // Run ByteEncode on row F[k]
        ByteEncode(s_cap[k], temp_encoded, 12); // we have to run with 12 as in standard defination..

        for (int i = 0; i < 32 * 12; i++) {
            pke_secret_key[pke_offset + i] = temp_encoded[i];
        }
        pke_offset += 32 * 12;  // Update the offset
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

void k_pke_encrypt(const uint8_t* pubkey, uint8_t* m, uint8_t* r,uint8_t* cipher){
// pubkey = long
// m = 32
// r = 32
// cipher = similar long...

    uint8_t N = 0;
    uint16_t t_cap[ML_KEM_512_K][256];
    uint8_t row[32]; // bytes seed man...
    int pke_offset = 0;   
    for (int k = 0; k < ML_KEM_512_K; k++) {
        uint8_t temp_encoded[32 * 12]; 
        for (int i = 0; i < 32 * 12; i++) {
            temp_encoded[i] = pubkey[pke_offset + i];
        }
        ByteDecode(temp_encoded, t_cap[k], 12);
        pke_offset += 32 * 12;
    }    
    for (int i = 0; i < 32; i++) {
        row[i] = pubkey[pke_offset + i];
    }
    uint16_t A[ML_KEM_512_K][ML_KEM_512_K][256];
    uint8_t B_34[34]={0}; // b_34 = row || j || i
    for(size_t i=0;i<32;i++)B_34[i] = row[i];
    for(size_t i = 0;i< ML_KEM_512_K;i++){
        for(size_t j=0;j<ML_KEM_512_K;j++){
            B_34[32] = j;
            B_34[33] = i;
            SampleNTT(B_34,A[i][j]);
        }
    }
    uint16_t y[ML_KEM_512_K][256]; // k polynomials in z_256 with modulo q
    for(size_t i=0; i<ML_KEM_512_K; i++){
        //void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t* length_of_output, uint8_t eta)
        size_t len_of_prfOutput = 64*2;
        uint8_t prfOutput_64n[len_of_prfOutput]; // eta is 2 here.
        PRF(r, 32, &N, prfOutput_64n, len_of_prfOutput, 1);
        SamplePolyCBD(y[i],prfOutput_64n, len_of_prfOutput, 1);//output in the s[i], eta is 1 again..
        N++;
    }
    uint16_t e_1[ML_KEM_512_K][256]; // k polynomials in z_256 with modulo q
    for(size_t i=0; i<ML_KEM_512_K; i++){
        //void PRF(uint8_t* s,uint8_t length_of_s, uint8_t* single_byte_b, uint8_t* output,uint8_t* length_of_output, uint8_t eta)
        size_t len_of_prfOutput = 64*2;
        uint8_t prfOutput_64n[len_of_prfOutput]; // eta is 2 here.
        PRF(r, 32, &N, prfOutput_64n, len_of_prfOutput, 2);
        SamplePolyCBD(e_1[i],prfOutput_64n, len_of_prfOutput, 2);//output in the s[i], eta is 1 again..
        N++;
    }
    uint16_t e_2[256];
    size_t len_of_prfOutput = 64*2;
    uint8_t prfOutput_64n[len_of_prfOutput]; 
    SamplePolyCBD(e_2,prfOutput_64n, len_of_prfOutput, 2);
    uint16_t y_cap[ML_KEM_512_K][256];
    for(size_t i=0; i<ML_KEM_512_K;i++){
        NTT(y_cap[i],y[i]);
    }
    // finding u
    uint16_t u[ML_KEM_512_K][256];
    // defining Atranspose dot y_cap
    // a is of type uint16_t A[ML_KEM_512_K][ML_KEM_512_K][256];
    // and y_cap is of type uint16_t y_cap[ML_KEM_512_K][256];
    // so we have to find the dot product of these two.
    // like run a top level loop for k times in each find the respective dot product and save in the u_cap
    for (size_t k = 0; k < ML_KEM_512_K; k++) {
       uint16_t temp_dot[256] = {0};  // Temporary array to store the dot product result for each k
    
    // Compute the dot product for the k-th row
        for (size_t i = 0; i < ML_KEM_512_K; i++) {
            for (size_t j = 0; j < 256; j++) {
                temp_dot[j] += A[k][j][i] * y_cap[i][j];  // Accumulate the dot product
            }
        }
        // After computing the dot product, apply NTT_inverse to the result
        NTT_inv(u[k], temp_dot);
    }




}