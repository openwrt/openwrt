/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#ifndef GOST_GRASSHOPPER_CORE_H
#define GOST_GRASSHOPPER_CORE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "gost_grasshopper_defines.h"

static void grasshopper_l(grasshopper_w128_t* w);
static void grasshopper_l_inv(grasshopper_w128_t* w);

// key setup
extern void grasshopper_set_encrypt_key(grasshopper_round_keys_t* subkeys, const grasshopper_key_t* key);
extern void grasshopper_set_decrypt_key(grasshopper_round_keys_t* subkeys, const grasshopper_key_t* key);

// single-block ecp ops
extern void grasshopper_encrypt_block(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source, grasshopper_w128_t* target, grasshopper_w128_t* buffer);
extern void grasshopper_decrypt_block(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source, grasshopper_w128_t* target, grasshopper_w128_t* buffer);

extern void grasshopper_encrypt_block2(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source, grasshopper_w128_t* target);
extern void grasshopper_decrypt_block2(grasshopper_round_keys_t* subkeys, grasshopper_w128_t* source, grasshopper_w128_t* target);

#if defined(__cplusplus)
}
#endif

#endif
