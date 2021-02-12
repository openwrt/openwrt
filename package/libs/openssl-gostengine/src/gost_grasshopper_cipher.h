/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#ifndef GOST_GRASSHOPPER_CIPHER_H
#define GOST_GRASSHOPPER_CIPHER_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "gost_grasshopper_defines.h"

#include <openssl/evp.h>

// not thread safe
// because of buffers
typedef struct {
    uint8_t type;
    grasshopper_key_t key;
    grasshopper_round_keys_t encrypt_round_keys;
    grasshopper_round_keys_t decrypt_round_keys;
    grasshopper_w128_t buffer;
} gost_grasshopper_cipher_ctx;

typedef struct {
    gost_grasshopper_cipher_ctx c;
    grasshopper_w128_t buffer1;
    uint32_t g;
    uint32_t go;
} gost_grasshopper_cipher_ctx_ofb;

typedef struct {
    gost_grasshopper_cipher_ctx c;
    grasshopper_w128_t iv_buffer;
    grasshopper_w128_t partial_buffer;
    uint64_t counter;
} gost_grasshopper_cipher_ctx_ctr;

typedef int (* grasshopper_init_cipher_func)(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv,
                                             int enc);

typedef int (* grasshopper_do_cipher_func)(EVP_CIPHER_CTX* ctx, unsigned char* out, const unsigned char* in,
                                           size_t inl);

typedef void (* grasshopper_destroy_cipher_func)(gost_grasshopper_cipher_ctx* c);

void gost_grasshopper_cipher_key(gost_grasshopper_cipher_ctx* c, const uint8_t* k);

void gost_grasshopper_cipher_destroy(gost_grasshopper_cipher_ctx* c);

int gost_grasshopper_cipher_init_ecb(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv, int enc);

int gost_grasshopper_cipher_init_cbc(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv, int enc);

int gost_grasshopper_cipher_init_ofb(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv, int enc);

int gost_grasshopper_cipher_init_cfb(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv, int enc);

int gost_grasshopper_cipher_init_ctr(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* iv, int enc);

int gost_grasshopper_cipher_init(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                 const unsigned char* iv, int enc);

int gost_grasshopper_cipher_do(EVP_CIPHER_CTX* ctx, unsigned char* out,
                               const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_do_ecb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                   const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_do_cbc(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                   const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_do_ofb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                   const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_do_cfb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                   const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_do_ctr(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                   const unsigned char* in, size_t inl);

int gost_grasshopper_cipher_cleanup(EVP_CIPHER_CTX* ctx);

int gost_grasshopper_set_asn1_parameters(EVP_CIPHER_CTX* ctx, ASN1_TYPE* params);

int gost_grasshopper_get_asn1_parameters(EVP_CIPHER_CTX* ctx, ASN1_TYPE* params);

int gost_grasshopper_cipher_ctl(EVP_CIPHER_CTX* ctx, int type, int arg, void* ptr);

EVP_CIPHER* cipher_gost_grasshopper_create(int cipher_type, int block_size);

const int cipher_gost_grasshopper_setup(EVP_CIPHER* cipher, uint8_t mode, int iv_size, bool padding);

const EVP_CIPHER* cipher_gost_grasshopper(uint8_t mode, uint8_t num);

extern const EVP_CIPHER* cipher_gost_grasshopper_ecb();
extern const EVP_CIPHER* cipher_gost_grasshopper_cbc();
extern const EVP_CIPHER* cipher_gost_grasshopper_ofb();
extern const EVP_CIPHER* cipher_gost_grasshopper_cfb();
extern const EVP_CIPHER* cipher_gost_grasshopper_ctr();

#if defined(__cplusplus)
}
#endif

#endif
