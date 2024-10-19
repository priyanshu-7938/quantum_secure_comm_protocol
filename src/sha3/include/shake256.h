#ifndef SHAKE256_H
#define SHAKE256_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// The number of lanes in the Keccak state
#define KECCAK_LANE_CNT 25

// SHAKE256 Extendable Output Function: Keccak[512](M || 1111, d)
typedef struct {
    uint64_t state[KECCAK_LANE_CNT];
    size_t offset;
    int finalized; // all message bytes absorbed?
    size_t squeezable;
} shake256_t;

// Width of capacity portion of the sponge, in bits
#define CAPACITY 512

// Width of rate portion of the sponge, in bits
#define RATE (1600 - CAPACITY)

// Domain separator bits, used for finalization
#define DOM_SEP 0b00001111

// Bit-width of domain separator, starting from least significant bit
#define DOM_SEP_BW 4

// Function to absorb a message into the SHAKE256 state
void shake256_absorb(shake256_t *state, const uint8_t *msg, size_t msg_len);

// Function to finalize the SHAKE256 state
void shake256_finalize(shake256_t *state);

// Function to squeeze output bytes from the SHAKE256 state
void shake256_squeeze(shake256_t *state, uint8_t *output, size_t len);

// Function to reset the SHAKE256 state
void shake256_reset(shake256_t *state);

#endif // SHAKE256_H
