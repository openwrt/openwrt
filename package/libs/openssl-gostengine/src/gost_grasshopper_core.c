/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#if defined(__cplusplus)
extern "C" {
#endif

#include "gost_grasshopper_core.h"
#include "gost_grasshopper_math.h"
#include "gost_grasshopper_precompiled.h"
#include "gost_grasshopper_defines.h"

static GRASSHOPPER_INLINE void grasshopper_l(grasshopper_w128_t* w) {
    uint8_t x;
		unsigned int j;
		int i;

    // 16 rounds
    for (j = 0; j < sizeof(grasshopper_lvec) / sizeof(grasshopper_lvec[0]); j++) {

        // An LFSR with 16 elements from GF(2^8)
        x = w->b[15];    // since lvec[15] = 1

        for (i = 14; i >= 0; i--) {
            w->b[i + 1] = w->b[i];
            x ^= grasshopper_galois_mul(w->b[i], grasshopper_lvec[i]);
        }
        w->b[0] = x;
    }
}

static GRASSHOPPER_INLINE void grasshopper_l_inv(grasshopper_w128_t* w) {
    uint8_t x;
		unsigned int j;
		int i;

    // 16 rounds
    for (j = 0; j < sizeof(grasshopper_lvec) / sizeof(grasshopper_lvec[0]); j++) {

        x = w->b[0];
        for (i = 0; i < 15; i++) {
            w->b[i] = w->b[i + 1];
            x ^= grasshopper_galois_mul(w->b[i], grasshopper_lvec[i]);
        }
        w->b[15] = x;
    }
}

// key setup

void grasshopper_set_encrypt_key(grasshopper_round_keys_t* subkeys, const grasshopper_key_t* key) {
    grasshopper_w128_t c, x, y, z;
		int i;

    for (i = 0; i < 16; i++) {
        // this will be have to changed for little-endian systems
        x.b[i] = key->k.b[i];
        y.b[i] = key->k.b[i + 16];
    }

    grasshopper_copy128(&subkeys->k[0], &x);
    grasshopper_copy128(&subkeys->k[1], &y);

    for (i = 1; i <= 32; i++) {

        // C Value
        grasshopper_zero128(&c);
        c.b[15] = (uint8_t) i;        // load round in lsb
        grasshopper_l(&c);

        grasshopper_plus128(&z, &x, &c);
        grasshopper_convert128(&z, grasshopper_pi);
        grasshopper_l(&z);
        grasshopper_append128(&z, &y);

        grasshopper_copy128(&y, &x);
        grasshopper_copy128(&x, &z);

        if ((i & 7) == 0) {
            int k = i >> 2;
            grasshopper_copy128(&subkeys->k[k], &x);
            grasshopper_copy128(&subkeys->k[k + 1], &y);
        }
    }

    // security++
    grasshopper_zero128(&c);
    grasshopper_zero128(&x);
    grasshopper_zero128(&y);
    grasshopper_zero128(&z);
}

void grasshopper_set_decrypt_key(grasshopper_round_keys_t* subkeys, const grasshopper_key_t* key) {
		int i;
    grasshopper_set_encrypt_key(subkeys, key);

    for (i = 1; i < 10; i++) {
        grasshopper_l_inv(&subkeys->k[i]);
    }
}

void grasshopper_encrypt_block(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source,
                               grasshopper_w128_t* target, grasshopper_w128_t* buffer) {
		int i;
    grasshopper_copy128(target, source);

    for (i = 0; i < 9; i++) {
        grasshopper_append128(target, &subkeys->k[i]);
        grasshopper_append128multi(buffer, target, grasshopper_pil_enc128);
    }

    grasshopper_append128(target, &subkeys->k[9]);
}

void grasshopper_encrypt_block2(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source,
                                grasshopper_w128_t* target) {
    grasshopper_w128_t buffer;
    grasshopper_encrypt_block(subkeys, source, target, &buffer);
    grasshopper_zero128(&buffer);
}

void grasshopper_decrypt_block(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source,
                               grasshopper_w128_t* target, grasshopper_w128_t* buffer) {
		int i;
    grasshopper_copy128(target, source);

    grasshopper_append128multi(buffer, target, grasshopper_l_dec128);

    for (i = 9; i > 1; i--) {
        grasshopper_append128(target, &subkeys->k[i]);
        grasshopper_append128multi(buffer, target, grasshopper_pil_dec128);
    }

    grasshopper_append128(target, &subkeys->k[1]);
    grasshopper_convert128(target, grasshopper_pi_inv);
    grasshopper_append128(target, &subkeys->k[0]);
}

void grasshopper_decrypt_block2(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source,
                                grasshopper_w128_t* target) {
    grasshopper_w128_t buffer;
    grasshopper_decrypt_block(subkeys, source, target, &buffer);
    grasshopper_zero128(&buffer);
}

#if defined(__cplusplus)
}
#endif
