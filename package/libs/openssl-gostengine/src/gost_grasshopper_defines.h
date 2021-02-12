/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#ifndef GOST_GRASSHOPPER_DEFINES_H
#define GOST_GRASSHOPPER_DEFINES_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

# if !defined(inline) && !defined(__cplusplus)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#   define GRASSHOPPER_INLINE inline
#  elif defined(__GNUC__) && __GNUC__ >= 2
#   define GRASSHOPPER_INLINE __inline__
#  elif defined(_MSC_VER)
#   define GRASSHOPPER_INLINE __inline
#  else
#   define GRASSHOPPER_INLINE
#  endif
# else
#  define GRASSHOPPER_INLINE inline
# endif

typedef union {
    uint8_t b[16];
    uint64_t q[2];
    uint32_t d[4];
    uint16_t w[8];
} grasshopper_w128_t;

typedef union {
    uint8_t b[32];
    uint64_t q[4];
    uint32_t d[8];
    uint16_t w[16];
    grasshopper_w128_t k[2];
} grasshopper_w256_t;

typedef struct {
    grasshopper_w256_t k;
} grasshopper_key_t;

#define GRASSHOPPER_ROUND_KEYS_COUNT 10

typedef struct {
    grasshopper_w128_t k[GRASSHOPPER_ROUND_KEYS_COUNT];
} grasshopper_round_keys_t;

extern const uint8_t grasshopper_pi[0x100];

extern const uint8_t grasshopper_pi_inv[0x100];

extern const uint8_t grasshopper_lvec[16];

#define GRASSHOPPER_BLOCK_SIZE (128/8)
#define GRASSHOPPER_KEY_SIZE (256/8)

#if defined(__cplusplus)
}
#endif

#endif
