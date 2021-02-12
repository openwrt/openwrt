/*
 * Maxim Tishkov 2016
 * This file is distributed under the same license as OpenSSL
 */

#if defined(__cplusplus)
extern "C" {
#endif

#include "gost_grasshopper_cipher.h"
#include "gost_grasshopper_defines.h"
#include "gost_grasshopper_math.h"
#include "gost_grasshopper_core.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <string.h>

#include "e_gost_err.h"

enum GRASSHOPPER_CIPHER_TYPE {
    GRASSHOPPER_CIPHER_ECB = 0,
    GRASSHOPPER_CIPHER_CBC,
    GRASSHOPPER_CIPHER_OFB,
    GRASSHOPPER_CIPHER_CFB,
    GRASSHOPPER_CIPHER_CTR
};

static EVP_CIPHER* gost_grasshopper_ciphers[5] = {
        [GRASSHOPPER_CIPHER_ECB] = NULL,
        [GRASSHOPPER_CIPHER_CBC] = NULL,
        [GRASSHOPPER_CIPHER_OFB] = NULL,
        [GRASSHOPPER_CIPHER_CFB] = NULL,
        [GRASSHOPPER_CIPHER_CTR] = NULL
};

static GRASSHOPPER_INLINE void gost_grasshopper_cipher_destroy_ofb(gost_grasshopper_cipher_ctx* c);
static GRASSHOPPER_INLINE void gost_grasshopper_cipher_destroy_ctr(gost_grasshopper_cipher_ctx* c);

struct GRASSHOPPER_CIPHER_PARAMS {
    int nid;
    grasshopper_init_cipher_func init_cipher;
    grasshopper_do_cipher_func do_cipher;
    grasshopper_destroy_cipher_func destroy_cipher;
    int block_size;
    int ctx_size;
    int iv_size;
    bool padding;
};

static struct GRASSHOPPER_CIPHER_PARAMS gost_cipher_params[5] = {
        [GRASSHOPPER_CIPHER_ECB] = {
                NID_grasshopper_ecb,
                gost_grasshopper_cipher_init_ecb,
                gost_grasshopper_cipher_do_ecb,
                NULL,
                16,
                sizeof(gost_grasshopper_cipher_ctx),
                0,
                true
        },
        [GRASSHOPPER_CIPHER_CBC] = {
                NID_grasshopper_cbc,
                gost_grasshopper_cipher_init_cbc,
                gost_grasshopper_cipher_do_cbc,
                NULL,
                16,
                sizeof(gost_grasshopper_cipher_ctx),
                16,
                true
        },
        [GRASSHOPPER_CIPHER_OFB] = {
                NID_grasshopper_ofb,
                gost_grasshopper_cipher_init_ofb,
                gost_grasshopper_cipher_do_ofb,
                gost_grasshopper_cipher_destroy_ofb,
                1,
                sizeof(gost_grasshopper_cipher_ctx_ofb),
                16,
                false
        },
        [GRASSHOPPER_CIPHER_CFB] = {
                NID_grasshopper_cfb,
                gost_grasshopper_cipher_init_cfb,
                gost_grasshopper_cipher_do_cfb,
                NULL,
                1,
                sizeof(gost_grasshopper_cipher_ctx),
                16,
                false
        },
        [GRASSHOPPER_CIPHER_CTR] = {
                NID_grasshopper_ctr,
                gost_grasshopper_cipher_init_ctr,
                gost_grasshopper_cipher_do_ctr,
                gost_grasshopper_cipher_destroy_ctr,
                1,
                sizeof(gost_grasshopper_cipher_ctx_ctr),
                8,
                false
        },
};

/* Set 256 bit  key into context */
GRASSHOPPER_INLINE void gost_grasshopper_cipher_key(gost_grasshopper_cipher_ctx* c, const uint8_t* k) {
		int i;
    for (i = 0; i < 2; i++) {
        grasshopper_copy128(&c->key.k.k[i], (const grasshopper_w128_t*) (k + i * 16));
    }
    grasshopper_set_encrypt_key(&c->encrypt_round_keys, &c->key);
    grasshopper_set_decrypt_key(&c->decrypt_round_keys, &c->key);
}

/* Cleans up key from context */
GRASSHOPPER_INLINE void gost_grasshopper_cipher_destroy(gost_grasshopper_cipher_ctx* c) {
		int i;
    for (i = 0; i < 2; i++) {
        grasshopper_zero128(&c->key.k.k[i]);
    }
    for (i = 0; i < GRASSHOPPER_ROUND_KEYS_COUNT; i++) {
        grasshopper_zero128(&c->encrypt_round_keys.k[i]);
    }
    for (i = 0; i < GRASSHOPPER_ROUND_KEYS_COUNT; i++) {
        grasshopper_zero128(&c->decrypt_round_keys.k[i]);
    }
    grasshopper_zero128(&c->buffer);
}

static GRASSHOPPER_INLINE void gost_grasshopper_cipher_destroy_ofb(gost_grasshopper_cipher_ctx* c) {
    gost_grasshopper_cipher_ctx_ofb* ctx = (gost_grasshopper_cipher_ctx_ofb*) c;

    grasshopper_zero128(&ctx->buffer1);
}

static GRASSHOPPER_INLINE void gost_grasshopper_cipher_destroy_ctr(gost_grasshopper_cipher_ctx* c) {
    gost_grasshopper_cipher_ctx_ctr* ctx = (gost_grasshopper_cipher_ctx_ctr*) c;

    grasshopper_zero128(&ctx->iv_buffer);
    grasshopper_zero128(&ctx->partial_buffer);

    ctx->counter = 0;
}

int gost_grasshopper_cipher_init(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                        const unsigned char* iv, int enc) {
    gost_grasshopper_cipher_ctx* c = EVP_CIPHER_CTX_get_cipher_data(ctx);

    if (EVP_CIPHER_CTX_get_app_data(ctx) == NULL) {
        EVP_CIPHER_CTX_set_app_data(ctx, EVP_CIPHER_CTX_get_cipher_data(ctx));
    }

    if (key != NULL) {
        gost_grasshopper_cipher_key(c, key);
    }

    if (iv != NULL) {
        memcpy((unsigned char*) EVP_CIPHER_CTX_original_iv(ctx), iv,
               EVP_CIPHER_CTX_iv_length(ctx));
    }

    memcpy(EVP_CIPHER_CTX_iv_noconst(ctx),
           EVP_CIPHER_CTX_original_iv(ctx),
           EVP_CIPHER_CTX_iv_length(ctx));

    grasshopper_zero128(&c->buffer);

    return 1;
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_init_ecb(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                                               const unsigned char* iv,
                                                               int enc) {
    gost_grasshopper_cipher_ctx* c = EVP_CIPHER_CTX_get_cipher_data(ctx);
    c->type = GRASSHOPPER_CIPHER_ECB;
    return gost_grasshopper_cipher_init(ctx, key, iv, enc);
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_init_cbc(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                                               const unsigned char* iv,
                                                               int enc) {
    gost_grasshopper_cipher_ctx* c = EVP_CIPHER_CTX_get_cipher_data(ctx);
    c->type = GRASSHOPPER_CIPHER_CBC;
    return gost_grasshopper_cipher_init(ctx, key, iv, enc);
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_init_ofb(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                                               const unsigned char* iv,
                                                               int enc) {
    gost_grasshopper_cipher_ctx_ofb* c = EVP_CIPHER_CTX_get_cipher_data(ctx);

    c->c.type = GRASSHOPPER_CIPHER_OFB;

    grasshopper_zero128(&c->buffer1);

    return gost_grasshopper_cipher_init(ctx, key, iv, enc);
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_init_cfb(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                                               const unsigned char* iv,
                                                               int enc) {
    gost_grasshopper_cipher_ctx* c = EVP_CIPHER_CTX_get_cipher_data(ctx);
    c->type = GRASSHOPPER_CIPHER_CFB;
    return gost_grasshopper_cipher_init(ctx, key, iv, enc);
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_init_ctr(EVP_CIPHER_CTX* ctx, const unsigned char* key,
                                                               const unsigned char* iv,
                                                               int enc) {
    gost_grasshopper_cipher_ctx_ctr* c = EVP_CIPHER_CTX_get_cipher_data(ctx);

    c->c.type = GRASSHOPPER_CIPHER_CTR;

    grasshopper_zero128(&c->iv_buffer);
    grasshopper_zero128(&c->partial_buffer);

    c->counter = 0;

    return gost_grasshopper_cipher_init(ctx, key, iv, enc);
}

GRASSHOPPER_INLINE int gost_grasshopper_cipher_do(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                                         const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx* c = (gost_grasshopper_cipher_ctx*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    struct GRASSHOPPER_CIPHER_PARAMS* params = &gost_cipher_params[c->type];

    return params->do_cipher(ctx, out, in, inl);
}

int gost_grasshopper_cipher_do_ecb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                          const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx* c = (gost_grasshopper_cipher_ctx*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    bool encrypting = (bool) EVP_CIPHER_CTX_encrypting(ctx);
    const unsigned char* current_in = in;
    unsigned char* current_out = out;
    size_t blocks = inl / GRASSHOPPER_BLOCK_SIZE;
    size_t i;

    for (i = 0; i < blocks; i++, current_in += GRASSHOPPER_BLOCK_SIZE, current_out += GRASSHOPPER_BLOCK_SIZE) {
        if (encrypting) {
            grasshopper_encrypt_block(&c->encrypt_round_keys, (grasshopper_w128_t*) current_in,
                                      (grasshopper_w128_t*) current_out,
                                      &c->buffer);
        } else {
            grasshopper_decrypt_block(&c->decrypt_round_keys, (grasshopper_w128_t*) current_in,
                                      (grasshopper_w128_t*) current_out,
                                      &c->buffer);
        }
    }

    return 1;
}

int gost_grasshopper_cipher_do_cbc(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                          const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx* c = (gost_grasshopper_cipher_ctx*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    unsigned char* iv = EVP_CIPHER_CTX_iv_noconst(ctx);
    bool encrypting = (bool) EVP_CIPHER_CTX_encrypting(ctx);
    const unsigned char* current_in = in;
    unsigned char* current_out = out;
    grasshopper_w128_t* currentInputBlock;
    grasshopper_w128_t* currentOutputBlock;
    size_t blocks = inl / GRASSHOPPER_BLOCK_SIZE;
    size_t i;
    grasshopper_w128_t* currentBlock;

    currentBlock = (grasshopper_w128_t*) iv;

    for (i = 0; i < blocks; i++, current_in += GRASSHOPPER_BLOCK_SIZE, current_out += GRASSHOPPER_BLOCK_SIZE) {
        currentInputBlock = (grasshopper_w128_t*) current_in;
        currentOutputBlock = (grasshopper_w128_t*) current_out;
        if (encrypting) {
            grasshopper_append128(currentBlock, currentInputBlock);
            grasshopper_encrypt_block(&c->encrypt_round_keys, currentBlock, currentOutputBlock, &c->buffer);
            grasshopper_copy128(currentBlock, currentOutputBlock);
        } else {
            grasshopper_decrypt_block(&c->decrypt_round_keys, currentInputBlock, currentOutputBlock, &c->buffer);
            grasshopper_append128(currentOutputBlock, currentBlock);
            grasshopper_copy128(currentBlock, currentInputBlock);
        }
    }

    return 1;
}

int gost_grasshopper_cipher_do_ctr(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                          const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx_ctr* c = (gost_grasshopper_cipher_ctx_ctr*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    unsigned char* iv = EVP_CIPHER_CTX_iv_noconst(ctx);
    const unsigned char* current_in = in;
    unsigned char* current_out = out;
    size_t blocks = inl / GRASSHOPPER_BLOCK_SIZE;
    grasshopper_w128_t* currentInputBlock;
    grasshopper_w128_t* currentOutputBlock;
    size_t lasted;
    size_t i;

    memcpy(&c->iv_buffer, iv, 8);

    // full parts
    for (i = 0; i < blocks; i++) {
        currentInputBlock = (grasshopper_w128_t*) current_in;
        currentOutputBlock = (grasshopper_w128_t*) current_out;
        memcpy(c->iv_buffer.b + 8, &c->counter, 8);
        grasshopper_encrypt_block(&c->c.encrypt_round_keys, &c->iv_buffer, currentOutputBlock, &c->c.buffer);
        grasshopper_append128(currentOutputBlock, currentInputBlock);
        c->counter += 1;
        current_in += GRASSHOPPER_BLOCK_SIZE;
        current_out += GRASSHOPPER_BLOCK_SIZE;
    }

    // last part
    lasted = inl - blocks * GRASSHOPPER_BLOCK_SIZE;
    if (lasted > 0) {
        currentInputBlock = (grasshopper_w128_t*) current_in;
        currentOutputBlock = (grasshopper_w128_t*) current_out;
        memcpy(c->iv_buffer.b + 8, &c->counter, 8);
        grasshopper_encrypt_block(&c->c.encrypt_round_keys, &c->iv_buffer, &c->partial_buffer, &c->c.buffer);
        for (i = 0; i < lasted; i++) {
            currentOutputBlock->b[i] = c->partial_buffer.b[i] ^ currentInputBlock->b[i];
        }
        c->counter += 1;
    }

    return 1;
}

static void gost_grasshopper_cnt_next(gost_grasshopper_cipher_ctx_ofb* ctx, grasshopper_w128_t* iv,
                                      grasshopper_w128_t* buf) {
    memcpy(&ctx->buffer1, iv, 16);
    ctx->g = ctx->buffer1.b[0] | (ctx->buffer1.b[1] << 8) | (ctx->buffer1.b[2] << 16) |
             ((uint32_t) ctx->buffer1.b[3] << 24);
    ctx->g += 0x01010101;
    ctx->buffer1.b[0] = (unsigned char) (ctx->g & 0xff);
    ctx->buffer1.b[1] = (unsigned char) ((ctx->g >> 8) & 0xff);
    ctx->buffer1.b[2] = (unsigned char) ((ctx->g >> 16) & 0xff);
    ctx->buffer1.b[3] = (unsigned char) ((ctx->g >> 24) & 0xff);
    ctx->g = ctx->buffer1.b[4] | (ctx->buffer1.b[5] << 8) | (ctx->buffer1.b[6] << 16) |
             ((uint32_t) ctx->buffer1.b[7] << 24);
    ctx->go = ctx->g;
    ctx->g += 0x01010104;
    if (ctx->go > ctx->g) {                 /* overflow */
        ctx->g++;
    }
    ctx->buffer1.b[4] = (unsigned char) (ctx->g & 0xff);
    ctx->buffer1.b[5] = (unsigned char) ((ctx->g >> 8) & 0xff);
    ctx->buffer1.b[6] = (unsigned char) ((ctx->g >> 16) & 0xff);
    ctx->buffer1.b[7] = (unsigned char) ((ctx->g >> 24) & 0xff);
    ctx->g = ctx->buffer1.b[8] | (ctx->buffer1.b[9] << 8) | (ctx->buffer1.b[10] << 16) |
             ((uint32_t) ctx->buffer1.b[11] << 24);
    ctx->go = ctx->g;
    ctx->g += 0x01010107;
    if (ctx->go > ctx->g) {                 /* overflow */
        ctx->g++;
    }
    ctx->buffer1.b[8] = (unsigned char) (ctx->g & 0xff);
    ctx->buffer1.b[9] = (unsigned char) ((ctx->g >> 8) & 0xff);
    ctx->buffer1.b[10] = (unsigned char) ((ctx->g >> 16) & 0xff);
    ctx->buffer1.b[11] = (unsigned char) ((ctx->g >> 24) & 0xff);
    ctx->g = ctx->buffer1.b[12] | (ctx->buffer1.b[13] << 8) | (ctx->buffer1.b[14] << 16) |
             ((uint32_t) ctx->buffer1.b[15] << 24);
    ctx->go = ctx->g;
    ctx->g += 0x01010110;
    if (ctx->go > ctx->g) {                 /* overflow */
        ctx->g++;
    }
    ctx->buffer1.b[12] = (unsigned char) (ctx->g & 0xff);
    ctx->buffer1.b[13] = (unsigned char) ((ctx->g >> 8) & 0xff);
    ctx->buffer1.b[14] = (unsigned char) ((ctx->g >> 16) & 0xff);
    ctx->buffer1.b[15] = (unsigned char) ((ctx->g >> 24) & 0xff);
    memcpy(iv, &ctx->buffer1, 16);
    grasshopper_encrypt_block(&ctx->c.encrypt_round_keys, &ctx->buffer1, buf, &ctx->c.buffer);
}

int gost_grasshopper_cipher_do_ofb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                          const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx_ofb* c = (gost_grasshopper_cipher_ctx_ofb*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    const unsigned char* in_ptr = in;
    unsigned char* out_ptr = out;
    unsigned char* buf = EVP_CIPHER_CTX_buf_noconst(ctx);
    unsigned char* iv = EVP_CIPHER_CTX_iv_noconst(ctx);
    int num = EVP_CIPHER_CTX_num(ctx);
    size_t i = 0;
    size_t j;

    /* process partial block if any */
    if (num > 0) {
        for (j = (size_t) num, i = 0; j < GRASSHOPPER_BLOCK_SIZE && i < inl;
             j++, i++, in_ptr++, out_ptr++) {
            *out_ptr = buf[j] ^ (*in_ptr);
        }
        if (j == GRASSHOPPER_BLOCK_SIZE) {
            EVP_CIPHER_CTX_set_num(ctx, 0);
        } else {
            EVP_CIPHER_CTX_set_num(ctx, (int) j);
            return 1;
        }
    }

    for (; i + GRASSHOPPER_BLOCK_SIZE <
           inl; i += GRASSHOPPER_BLOCK_SIZE, in_ptr += GRASSHOPPER_BLOCK_SIZE, out_ptr += GRASSHOPPER_BLOCK_SIZE) {
        /*
         * block cipher current iv
         */
        /* Encrypt */
        gost_grasshopper_cnt_next(c, (grasshopper_w128_t*) iv, (grasshopper_w128_t*) buf);

        /*
         * xor next block of input text with it and output it
         */
        /*
         * output this block
         */
        for (j = 0; j < GRASSHOPPER_BLOCK_SIZE; j++) {
            out_ptr[j] = buf[j] ^ in_ptr[j];
        }
    }

    /* Process rest of buffer */
    if (i < inl) {
        gost_grasshopper_cnt_next(c, (grasshopper_w128_t*) iv, (grasshopper_w128_t*) buf);
        for (j = 0; i < inl; j++, i++) {
            out_ptr[j] = buf[j] ^ in_ptr[j];
        }
        EVP_CIPHER_CTX_set_num(ctx, (int) j);
    } else {
        EVP_CIPHER_CTX_set_num(ctx, 0);
    }

    return 1;
}

int gost_grasshopper_cipher_do_cfb(EVP_CIPHER_CTX* ctx, unsigned char* out,
                                          const unsigned char* in, size_t inl) {
    gost_grasshopper_cipher_ctx* c = (gost_grasshopper_cipher_ctx*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    const unsigned char* in_ptr = in;
    unsigned char* out_ptr = out;
    unsigned char* buf = EVP_CIPHER_CTX_buf_noconst(ctx);
    unsigned char* iv = EVP_CIPHER_CTX_iv_noconst(ctx);
    bool encrypting = (bool) EVP_CIPHER_CTX_encrypting(ctx);
    int num = EVP_CIPHER_CTX_num(ctx);
    size_t i = 0;
    size_t j = 0;

    /* process partial block if any */
    if (num > 0) {
        for (j = (size_t) num, i = 0; j < GRASSHOPPER_BLOCK_SIZE && i < inl; j++, i++, in_ptr++, out_ptr++) {
            if (!encrypting) {
                buf[j + GRASSHOPPER_BLOCK_SIZE] = *in_ptr;
            }
            *out_ptr = buf[j] ^ (*in_ptr);
            if (encrypting) {
                buf[j + GRASSHOPPER_BLOCK_SIZE] = *out_ptr;
            }
        }
        if (j == GRASSHOPPER_BLOCK_SIZE) {
            memcpy(iv, buf + GRASSHOPPER_BLOCK_SIZE, GRASSHOPPER_BLOCK_SIZE);
            EVP_CIPHER_CTX_set_num(ctx, 0);
        } else {
            EVP_CIPHER_CTX_set_num(ctx, (int) j);
            return 1;
        }
    }

    for (; i + GRASSHOPPER_BLOCK_SIZE <
           inl; i += GRASSHOPPER_BLOCK_SIZE, in_ptr += GRASSHOPPER_BLOCK_SIZE, out_ptr += GRASSHOPPER_BLOCK_SIZE) {
        /*
         * block cipher current iv
         */
        grasshopper_encrypt_block(&c->encrypt_round_keys, (grasshopper_w128_t*) iv, (grasshopper_w128_t*) buf,
                                  &c->buffer);
        /*
         * xor next block of input text with it and output it
         */
        /*
         * output this block
         */
        if (!encrypting) {
            memcpy(iv, in_ptr, GRASSHOPPER_BLOCK_SIZE);
        }
        for (j = 0; j < GRASSHOPPER_BLOCK_SIZE; j++) {
            out_ptr[j] = buf[j] ^ in_ptr[j];
        }
        /* Encrypt */
        /* Next iv is next block of cipher text */
        if (encrypting) {
            memcpy(iv, out_ptr, GRASSHOPPER_BLOCK_SIZE);
        }
    }

    /* Process rest of buffer */
    if (i < inl) {
        grasshopper_encrypt_block(&c->encrypt_round_keys, (grasshopper_w128_t*) iv, (grasshopper_w128_t*) buf,
                                  &c->buffer);
        if (!encrypting) {
            memcpy(buf + GRASSHOPPER_BLOCK_SIZE, in_ptr, inl - i);
        }
        for (j = 0; i < inl; j++, i++) {
            out_ptr[j] = buf[j] ^ in_ptr[j];
        }
        EVP_CIPHER_CTX_set_num(ctx, (int) j);
        if (encrypting) {
            memcpy(buf + GRASSHOPPER_BLOCK_SIZE, out_ptr, j);
        }
    } else {
        EVP_CIPHER_CTX_set_num(ctx, 0);
    }

    return 1;
}

int gost_grasshopper_cipher_cleanup(EVP_CIPHER_CTX* ctx) {
    gost_grasshopper_cipher_ctx* c = (gost_grasshopper_cipher_ctx*) EVP_CIPHER_CTX_get_cipher_data(ctx);
    struct GRASSHOPPER_CIPHER_PARAMS* params = &gost_cipher_params[c->type];

    gost_grasshopper_cipher_destroy(c);
    if (params->destroy_cipher != NULL) {
        params->destroy_cipher(c);
    }

    EVP_CIPHER_CTX_set_app_data(ctx, NULL);

    return 1;
}

int gost_grasshopper_set_asn1_parameters(EVP_CIPHER_CTX* ctx, ASN1_TYPE* params) {
    int len = 0;
    unsigned char* buf = NULL;
    ASN1_OCTET_STRING* os = NULL;

    os = ASN1_OCTET_STRING_new();

    if (!os || !ASN1_OCTET_STRING_set(os, buf, len)) {
        OPENSSL_free(buf);
        GOSTerr(GOST_F_GOST89_SET_ASN1_PARAMETERS, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    OPENSSL_free(buf);

    ASN1_TYPE_set(params, V_ASN1_SEQUENCE, os);
    return 1;
}

GRASSHOPPER_INLINE int gost_grasshopper_get_asn1_parameters(EVP_CIPHER_CTX* ctx, ASN1_TYPE* params) {
    int ret = -1;

    if (ASN1_TYPE_get(params) != V_ASN1_SEQUENCE) {
        return ret;
    }

    return 1;
}

int gost_grasshopper_cipher_ctl(EVP_CIPHER_CTX* ctx, int type, int arg, void* ptr) {
    switch (type) {
        case EVP_CTRL_RAND_KEY: {
            if (RAND_bytes((unsigned char*) ptr, EVP_CIPHER_CTX_key_length(ctx)) <= 0) {
                GOSTerr(GOST_F_GOST_CIPHER_CTL, GOST_R_RNG_ERROR);
                return -1;
            }
            break;
        }
        default:
            GOSTerr(GOST_F_GOST_CIPHER_CTL, GOST_R_UNSUPPORTED_CIPHER_CTL_COMMAND);
            return -1;
    }
    return 1;
}

GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_create(int cipher_type, int block_size) {
    return EVP_CIPHER_meth_new(cipher_type,
                               block_size  /* block_size */,
                               GRASSHOPPER_KEY_SIZE /* key_size */);
}

const int cipher_gost_grasshopper_setup(EVP_CIPHER* cipher, uint8_t mode, int iv_size, bool padding) {
    return EVP_CIPHER_meth_set_iv_length(cipher, iv_size) &&
           EVP_CIPHER_meth_set_flags(cipher, (unsigned long) (
                   mode |
                   ((!padding) ? EVP_CIPH_NO_PADDING : 0) |
                   ((iv_size > 0) ? EVP_CIPH_CUSTOM_IV : 0) |
                   EVP_CIPH_RAND_KEY |
                   EVP_CIPH_ALWAYS_CALL_INIT)
           ) &&
           EVP_CIPHER_meth_set_cleanup(cipher, gost_grasshopper_cipher_cleanup) &&
           EVP_CIPHER_meth_set_set_asn1_params(cipher, gost_grasshopper_set_asn1_parameters) &&
           EVP_CIPHER_meth_set_get_asn1_params(cipher, gost_grasshopper_get_asn1_parameters) &&
           EVP_CIPHER_meth_set_ctrl(cipher, gost_grasshopper_cipher_ctl) &&
           EVP_CIPHER_meth_set_do_cipher(cipher, gost_grasshopper_cipher_do);
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper(uint8_t mode, uint8_t num) {
    EVP_CIPHER** cipher;
    struct GRASSHOPPER_CIPHER_PARAMS* params;

    cipher = &gost_grasshopper_ciphers[num];

    if (*cipher == NULL) {
        params = &gost_cipher_params[num];

        int nid = params->nid;
        grasshopper_init_cipher_func init_cipher = params->init_cipher;
        int block_size = params->block_size;
        int ctx_size = params->ctx_size;
        int iv_size = params->iv_size;
        bool padding = params->padding;

        *cipher = cipher_gost_grasshopper_create(nid, block_size);
        if (*cipher == NULL) {
            return NULL;
        }

        if (!cipher_gost_grasshopper_setup(*cipher, mode, iv_size, padding) ||
            !EVP_CIPHER_meth_set_init(*cipher, init_cipher) ||
            !EVP_CIPHER_meth_set_impl_ctx_size(*cipher, ctx_size)) {
            EVP_CIPHER_meth_free(*cipher);
            *cipher = NULL;
        }
    }

    return *cipher;
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_ecb() {
    return cipher_gost_grasshopper(EVP_CIPH_ECB_MODE, GRASSHOPPER_CIPHER_ECB);
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_cbc() {
    return cipher_gost_grasshopper(EVP_CIPH_CBC_MODE, GRASSHOPPER_CIPHER_CBC);
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_ofb() {
    return cipher_gost_grasshopper(EVP_CIPH_OFB_MODE, GRASSHOPPER_CIPHER_OFB);
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_cfb() {
    return cipher_gost_grasshopper(EVP_CIPH_CFB_MODE, GRASSHOPPER_CIPHER_CFB);
}

const GRASSHOPPER_INLINE EVP_CIPHER* cipher_gost_grasshopper_ctr() {
    return cipher_gost_grasshopper(EVP_CIPH_CTR_MODE, GRASSHOPPER_CIPHER_CTR);
}

#if defined(__cplusplus)
}
#endif
