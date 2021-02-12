/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#ifndef GOST_GRASSHOPPER_MATH_H
#define GOST_GRASSHOPPER_MATH_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "gost_grasshopper_defines.h"

#if defined(__SSE__) || defined(__SSE2__) || defined(__SSE2_MATH__) || defined(__SSE3__) || defined(__SSE_MATH__) \
 || defined(__SSE4_1__)|| defined(__SSE4_2__)|| defined(__SSSE3__)
#define GRASSHOPPER_SSE_SUPPORTED
#endif

#define GRASSHOPPER_MIN_BITS 8
#define GRASSHOPPER_MAX_BITS 128

#if UINTPTR_MAX == 0xff
#define GRASSHOPPER_BITS 8
#elif UINTPTR_MAX == 0xffff
#define GRASSHOPPER_BITS 16
#elif UINTPTR_MAX == 0xffffffff
#define GRASSHOPPER_BITS 32
#elif UINTPTR_MAX == 0xffffffffffffffff
#define GRASSHOPPER_BITS 64
#endif

#define GRASSHOPPER_BIT_PARTS_8 (GRASSHOPPER_MAX_BITS / 8)
#define GRASSHOPPER_BIT_PARTS_16 (GRASSHOPPER_MAX_BITS / 16)
#define GRASSHOPPER_BIT_PARTS_32 (GRASSHOPPER_MAX_BITS / 32)
#define GRASSHOPPER_BIT_PARTS_64 (GRASSHOPPER_MAX_BITS / 64)

#define GRASSHOPPER_BIT_PARTS (GRASSHOPPER_MAX_BITS / GRASSHOPPER_BITS)
#define GRASSHOPPER_MAX_BIT_PARTS (GRASSHOPPER_MAX_BITS / GRASSHOPPER_MIN_BITS)

#define GRASSHOPPER_ACCESS_128_VALUE_8(key, part) ((key).b[(part)])
#define GRASSHOPPER_ACCESS_128_VALUE_16(key, part) ((key).w[(part)])
#define GRASSHOPPER_ACCESS_128_VALUE_32(key, part) ((key).d[(part)])
#define GRASSHOPPER_ACCESS_128_VALUE_64(key, part) ((key).q[(part)])

#if(GRASSHOPPER_BITS == 8)
#define GRASSHOPPER_ACCESS_128_VALUE GRASSHOPPER_ACCESS_128_VALUE_8
#elif(GRASSHOPPER_BITS == 16)
#define GRASSHOPPER_ACCESS_128_VALUE GRASSHOPPER_ACCESS_128_VALUE_16
#elif(GRASSHOPPER_BITS == 32)
#define GRASSHOPPER_ACCESS_128_VALUE GRASSHOPPER_ACCESS_128_VALUE_32
#elif(GRASSHOPPER_BITS == 64)
#define GRASSHOPPER_ACCESS_128_VALUE GRASSHOPPER_ACCESS_128_VALUE_64
#endif

static GRASSHOPPER_INLINE void grasshopper_zero128(grasshopper_w128_t* x) {
#if(GRASSHOPPER_BITS == 8 || GRASSHOPPER_BITS == 16)
    memset(&x, 0, sizeof(x));
#else
		int i;
    for (i = 0; i < GRASSHOPPER_BIT_PARTS; i++) {
        GRASSHOPPER_ACCESS_128_VALUE(*x, i) = 0;
    }
#endif
}

static GRASSHOPPER_INLINE void grasshopper_copy128(grasshopper_w128_t* to, const grasshopper_w128_t* from) {
#if(GRASSHOPPER_BITS == 8 || GRASSHOPPER_BITS == 16)
    __builtin_memcpy(&to, &from, sizeof(w128_t));
#else
		int i;
    for (i = 0; i < GRASSHOPPER_BIT_PARTS; i++) {
        GRASSHOPPER_ACCESS_128_VALUE(*to, i) = GRASSHOPPER_ACCESS_128_VALUE(*from, i);
    }
#endif
}

static GRASSHOPPER_INLINE void grasshopper_append128(grasshopper_w128_t* x, const grasshopper_w128_t* y) {
		int i;
    for (i = 0; i < GRASSHOPPER_BIT_PARTS; i++) {
        GRASSHOPPER_ACCESS_128_VALUE(*x, i) ^= GRASSHOPPER_ACCESS_128_VALUE(*y, i);
    }
}

static GRASSHOPPER_INLINE void grasshopper_plus128(grasshopper_w128_t* result, const grasshopper_w128_t* x,
                                               const grasshopper_w128_t* y) {
    grasshopper_copy128(result, x);
    grasshopper_append128(result, y);
}

// result & x must be different
static GRASSHOPPER_INLINE void grasshopper_plus128multi(grasshopper_w128_t* result, const grasshopper_w128_t* x,
                                                    const grasshopper_w128_t array[][256]) {
		int i;
    grasshopper_zero128(result);
    for (i = 0; i < GRASSHOPPER_MAX_BIT_PARTS; i++) {
        grasshopper_append128(result, &array[i][GRASSHOPPER_ACCESS_128_VALUE_8(*x, i)]);
    }
}

static GRASSHOPPER_INLINE void grasshopper_append128multi(grasshopper_w128_t* result, grasshopper_w128_t* x,
                                                      const grasshopper_w128_t array[][256]) {
    grasshopper_plus128multi(result, x, array);
    grasshopper_copy128(x, result);
}

static GRASSHOPPER_INLINE void grasshopper_convert128(grasshopper_w128_t* x, const uint8_t* array) {
		int i;
    for (i = 0; i < GRASSHOPPER_MAX_BIT_PARTS; i++) {
        GRASSHOPPER_ACCESS_128_VALUE_8(*x, i) = array[GRASSHOPPER_ACCESS_128_VALUE_8(*x, i)];
    }
}

#define GRASSHOPPER_GALOIS_POWER 8

#define GRASSHOPPER_GALOIS_FIELD_SIZE ((1 << GRASSHOPPER_GALOIS_POWER) - 1)

extern uint8_t grasshopper_galois_alpha_to[256];
extern uint8_t grasshopper_galois_index_of[256];

static GRASSHOPPER_INLINE uint8_t grasshopper_galois_mul(uint8_t x, uint8_t y) {
    if (__builtin_expect(x != 0 && y != 0, 1)) {
        return grasshopper_galois_alpha_to[(grasshopper_galois_index_of[x] + grasshopper_galois_index_of[y]) %
                                         GRASSHOPPER_GALOIS_FIELD_SIZE];
    } else {
        return 0;
    }
}

#if defined(__cplusplus)
}
#endif

#endif
