#include "prg.h"

// PRNG function to generate a 32-byte array of random numbers
void prg(uint8_t *output, uint8_t size, long seed) {
    uint32_t prn_value = (uint32_t) seed;
    
    for (int i = 0; i < size; i++) {
        // Generate the next value in the PRN sequence using LCG
        prn_value = (LCG_MULTIPLIER * prn_value + LCG_INCREMENT) % LCG_MODULUS;
        
        // Store the least significant 8 bits into the output array
        output[i] = (uint8_t)(prn_value & 0xFF);
    }
}


// int main() {
//     uint8_t random_bytes[34];   // Output array to store the 32 PRN values
//     long seed = 123456789;      // Example seed value

//     // Call the PRNG function
//     uint8_t size = 34;
//     prng(random_bytes, &size, seed);

//     // Print the generated random numbers
//     printf("Generated 34 random bytes:\n");
//     for (int i = 0; i < 34; i++) {
//         printf("%02X ", random_bytes[i]);
//         if ((i + 1) % 8 == 0) {
//             printf("\n");  // Print a newline after every 8 bytes
//         }
//     }

//     return 0;
// }
