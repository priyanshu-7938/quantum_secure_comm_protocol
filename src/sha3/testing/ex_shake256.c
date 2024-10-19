#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "shake256.h"

// Function to generate random data
void random_data(uint8_t *data, size_t len) {
    // Fallback to using rand() if /dev/urandom is not available
    for (size_t i = 0; i < len; i++) {
        data[i] = (uint8_t)(rand() % 256);
    }
    printf("fallback in dev.\n");
    for(size_t i = 0; i < len; i++) {
        printf("%d, ", data[i]);
    }
}

// Function to convert byte array to hex string
void to_hex(const uint8_t *data, size_t len, char *output) {
    for (size_t i = 0; i < len; i++) {
        sprintf(output + (i * 2), "%02x", data[i]);
    }
}

int main() {
    const size_t ilen = 32; // Input length
    const size_t olen = 40; // Output length

    uint8_t msg[ilen];       // Message buffer
    uint8_t dig[olen];       // Output buffer
    char hex_msg[ilen * 2 + 1]; // Hex string for input
    char hex_dig[olen * 2 + 1]; // Hex string for output

    // Generate random data for the input message
    random_data(msg, ilen);

    // Create shake256 hasher
    shake256_t hasher;
    shake256_reset(&hasher); // Reset the state

    // Absorb message bytes into sponge state
    shake256_absorb(&hasher, msg, ilen);
    // Finalize sponge state
    shake256_finalize(&hasher);

    // Squeeze total `olen` bytes out of sponge
    shake256_squeeze(&hasher, dig, olen);

    // Convert input and output to hex format
    hex_msg[ilen * 2] = '\0';
    hex_dig[olen * 2] = '\0';
    to_hex(msg, ilen, hex_msg);
    to_hex(dig, olen, hex_dig);

    // Print the results
    printf("SHAKE-256\n\n");
    printf("Input  : %s\n", hex_msg);
    printf("Output : %s\n", hex_dig);

    return EXIT_SUCCESS;
}
