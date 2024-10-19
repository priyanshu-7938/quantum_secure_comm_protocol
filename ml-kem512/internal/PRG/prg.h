#ifndef PRG_H
#define PRG_H

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>

// Linear Congruential Generator (LCG) constants
#define LCG_MULTIPLIER 1664525
#define LCG_INCREMENT 1013904223
#define LCG_MODULUS 4294967296

// PRNG function to generate a 32-byte array of random numbers
void prg(uint8_t *output, uint8_t size, long seed);
#endif