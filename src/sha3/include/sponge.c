#include "sponge.h"
#include "keccak.h"  // Ensure this header exists for Keccak functions
#include <stdlib.h>
#include <string.h>

// Pad10*1 implementation
void pad10x1(uint8_t domain_separator, size_t ds_bits, size_t rate, size_t offset, uint8_t* out) {
    memset(out, 0, rate / 8); // Clear the output buffer
    uint8_t mask = (1 << ds_bits) - 1;
    uint8_t pad_byte = (1 << ds_bits) | (domain_separator & mask);
    out[offset] = pad_byte;
    out[(rate / 8) - 1] ^= 0x80;
}

// Absorb implementation
void absorb(uint64_t* state, size_t* offset, const uint8_t* msg, size_t mlen, size_t rate) {
    size_t rbytes = rate >> 3; // # of bytes
    size_t rwords = rbytes >> 3; // # of 64-bit words

    uint8_t blk_bytes[rbytes];
    uint64_t blk_words[rwords];

    size_t blk_cnt = (*offset + mlen) / rbytes;
    size_t moff = 0;

    for (size_t i = 0; i < blk_cnt; i++) {
        size_t readable = rbytes - *offset;
        memcpy(blk_bytes + *offset, msg + moff, readable);

        // Convert bytes to words (little-endian)
        for (size_t j = 0; j < rwords; j++) {
            blk_words[j] = ((uint64_t)blk_bytes[j * 8]) |
                           ((uint64_t)blk_bytes[j * 8 + 1] << 8) |
                           ((uint64_t)blk_bytes[j * 8 + 2] << 16) |
                           ((uint64_t)blk_bytes[j * 8 + 3] << 24) |
                           ((uint64_t)blk_bytes[j * 8 + 4] << 32) |
                           ((uint64_t)blk_bytes[j * 8 + 5] << 40) |
                           ((uint64_t)blk_bytes[j * 8 + 6] << 48) |
                           ((uint64_t)blk_bytes[j * 8 + 7] << 56);
        }

        for (size_t j = 0; j < rwords; j++) {
            state[j] ^= blk_words[j];
        }

        keccak_permute(state); // Ensure this function exists in keccak.c

        moff += readable;
        *offset = 0;
    }

    size_t rm_bytes = mlen - moff;
    memcpy(blk_bytes + *offset, msg + moff, rm_bytes);
    memset(blk_bytes + *offset + rm_bytes, 0, rbytes - (*offset + rm_bytes));

    for (size_t j = 0; j < rwords; j++) {
        blk_words[j] = ((uint64_t)blk_bytes[j * 8]) |
                       ((uint64_t)blk_bytes[j * 8 + 1] << 8) |
                       ((uint64_t)blk_bytes[j * 8 + 2] << 16) |
                       ((uint64_t)blk_bytes[j * 8 + 3] << 24) |
                       ((uint64_t)blk_bytes[j * 8 + 4] << 32) |
                       ((uint64_t)blk_bytes[j * 8 + 5] << 40) |
                       ((uint64_t)blk_bytes[j * 8 + 6] << 48) |
                       ((uint64_t)blk_bytes[j * 8 + 7] << 56);
    }

    for (size_t j = 0; j < rwords; j++) {
        state[j] ^= blk_words[j];
    }

    *offset += rm_bytes;
}

// Finalize implementation
void finalize(uint64_t* state, size_t* offset, uint8_t domain_separator, size_t ds_bits, size_t rate) {
    size_t rbytes = rate >> 3; // # of bytes
    size_t rwords = rbytes >> 3; // # of 64-bit words

    uint8_t padb[rbytes];
    pad10x1(domain_separator, ds_bits, rate, *offset, padb);

    uint64_t padw[rwords];
    for (size_t j = 0; j < rwords; j++) {
        padw[j] = ((uint64_t)padb[j * 8]) |
                  ((uint64_t)padb[j * 8 + 1] << 8) |
                  ((uint64_t)padb[j * 8 + 2] << 16) |
                  ((uint64_t)padb[j * 8 + 3] << 24) |
                  ((uint64_t)padb[j * 8 + 4] << 32) |
                  ((uint64_t)padb[j * 8 + 5] << 40) |
                  ((uint64_t)padb[j * 8 + 6] << 48) |
                  ((uint64_t)padb[j * 8 + 7] << 56);
    }

    for (size_t j = 0; j < rwords; j++) {
        state[j] ^= padw[j];
    }

    keccak_permute(state); // Ensure this function exists in keccak.c
    *offset = 0;
}

// Squeeze implementation
void squeeze(uint64_t* state, size_t* squeezable, uint8_t* out, size_t olen, size_t rate) {
    size_t rbytes = rate >> 3; // # of bytes
    size_t rwords = rbytes >> 3; // # of 64-bit words

    uint8_t blk_bytes[rbytes];
    size_t off = 0;

    while (off < olen) {
        size_t read = (*squeezable < olen - off) ? *squeezable : olen - off;
        size_t soff = rbytes - *squeezable;

        // Convert state to bytes (little-endian)
        for (size_t j = 0; j < rwords; j++) {
            blk_bytes[j * 8] = (state[j] & 0xFF);
            blk_bytes[j * 8 + 1] = (state[j] >> 8) & 0xFF;
            blk_bytes[j * 8 + 2] = (state[j] >> 16) & 0xFF;
            blk_bytes[j * 8 + 3] = (state[j] >> 24) & 0xFF;
            blk_bytes[j * 8 + 4] = (state[j] >> 32) & 0xFF;
            blk_bytes[j * 8 + 5] = (state[j] >> 40) & 0xFF;
            blk_bytes[j * 8 + 6] = (state[j] >> 48) & 0xFF;
            blk_bytes[j * 8 + 7] = (state[j] >> 56) & 0xFF;
        }

        memcpy(out + off, blk_bytes + soff, read);

        *squeezable -= read;
        off += read;

        if (*squeezable == 0) {
            keccak_permute(state); // Ensure this function exists in keccak.c
            *squeezable = rbytes;
        }
    }
}
