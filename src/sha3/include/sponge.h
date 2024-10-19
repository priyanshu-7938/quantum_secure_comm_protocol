#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// Keccak family of sponge functions
#ifndef SPONGE_H
#define SPONGE_H

// Compile-time check to ensure that domain separator can only be 2 or 4 bits wide.
static inline bool check_domain_separator(const size_t dom_sep_bit_len) {
    return (dom_sep_bit_len == 2) || (dom_sep_bit_len == 4);
}

// Pad10*1 - generates a padding.
void pad10x1(uint8_t domain_separator, size_t ds_bits, size_t rate, size_t offset, uint8_t* out);

// Absorb the message into the sponge state.
void absorb(uint64_t* state, size_t* offset, const uint8_t* msg, size_t mlen, size_t rate);

// Finalize the sponge state.
void finalize(uint64_t* state, size_t* offset, uint8_t domain_separator, size_t ds_bits, size_t rate);

// Squeeze the output from the sponge state.
void squeeze(uint64_t* state, size_t* squeezable, uint8_t* out, size_t olen, size_t rate);

#endif // SPONGE_H
