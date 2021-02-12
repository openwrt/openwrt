/**********************************************************************
 *                          gost_md2012.c                             *
 *             Copyright (c) 2013 Cryptocom LTD.                      *
 *         This file is distributed under the same license as OpenSSL *
 *                                                                    *
 *          GOST R 34.11-2012 interface to OpenSSL engine.            *
 *                                                                    *
 * Author: Alexey Degtyarev <alexey@renatasystems.org>                *
 *                                                                    *
 **********************************************************************/

#include <openssl/evp.h>
#include "gosthash2012.h"

static int gost_digest_init512(EVP_MD_CTX *ctx);
static int gost_digest_init256(EVP_MD_CTX *ctx);
static int gost_digest_update(EVP_MD_CTX *ctx, const void *data,
                              size_t count);
static int gost_digest_final(EVP_MD_CTX *ctx, unsigned char *md);
static int gost_digest_copy(EVP_MD_CTX *to, const EVP_MD_CTX *from);
static int gost_digest_cleanup(EVP_MD_CTX *ctx);
static int gost_digest_ctrl_256(EVP_MD_CTX *ctx, int type, int arg,
                                void *ptr);
static int gost_digest_ctrl_512(EVP_MD_CTX *ctx, int type, int arg,
                                void *ptr);

const char micalg_256[] = "gostr3411-2012-256";
const char micalg_512[] = "gostr3411-2012-512";

static EVP_MD *_hidden_GostR3411_2012_256_md = NULL;
static EVP_MD *_hidden_GostR3411_2012_512_md = NULL;

EVP_MD *digest_gost2012_256(void)
{
    if (_hidden_GostR3411_2012_256_md == NULL) {
        EVP_MD *md;

        if ((md =
             EVP_MD_meth_new(NID_id_GostR3411_2012_256, NID_undef)) == NULL
            || !EVP_MD_meth_set_result_size(md, 32)
            || !EVP_MD_meth_set_input_blocksize(md, 64)
            || !EVP_MD_meth_set_app_datasize(md, sizeof(gost2012_hash_ctx))
            || !EVP_MD_meth_set_init(md, gost_digest_init256)
            || !EVP_MD_meth_set_update(md, gost_digest_update)
            || !EVP_MD_meth_set_final(md, gost_digest_final)
            || !EVP_MD_meth_set_copy(md, gost_digest_copy)
            || !EVP_MD_meth_set_ctrl(md, gost_digest_ctrl_256)
            || !EVP_MD_meth_set_cleanup(md, gost_digest_cleanup)) {
            EVP_MD_meth_free(md);
            md = NULL;
        }
        _hidden_GostR3411_2012_256_md = md;
    }
    return _hidden_GostR3411_2012_256_md;
}

void digest_gost2012_256_destroy(void)
{
    EVP_MD_meth_free(_hidden_GostR3411_2012_256_md);
    _hidden_GostR3411_2012_256_md = NULL;
}

EVP_MD *digest_gost2012_512(void)
{
    if (_hidden_GostR3411_2012_512_md == NULL) {
        EVP_MD *md;

        if ((md =
             EVP_MD_meth_new(NID_id_GostR3411_2012_512, NID_undef)) == NULL
            || !EVP_MD_meth_set_result_size(md, 64)
            || !EVP_MD_meth_set_input_blocksize(md, 64)
            || !EVP_MD_meth_set_app_datasize(md, sizeof(gost2012_hash_ctx))
            || !EVP_MD_meth_set_init(md, gost_digest_init512)
            || !EVP_MD_meth_set_update(md, gost_digest_update)
            || !EVP_MD_meth_set_final(md, gost_digest_final)
            || !EVP_MD_meth_set_copy(md, gost_digest_copy)
            || !EVP_MD_meth_set_ctrl(md, gost_digest_ctrl_512)
            || !EVP_MD_meth_set_cleanup(md, gost_digest_cleanup)) {
            EVP_MD_meth_free(md);
            md = NULL;
        }
        _hidden_GostR3411_2012_512_md = md;
    }
    return _hidden_GostR3411_2012_512_md;
}

void digest_gost2012_512_destroy(void)
{
    EVP_MD_meth_free(_hidden_GostR3411_2012_512_md);
    _hidden_GostR3411_2012_512_md = NULL;
}

static int gost_digest_init512(EVP_MD_CTX *ctx)
{
    init_gost2012_hash_ctx((gost2012_hash_ctx *) EVP_MD_CTX_md_data(ctx),
                           512);
    return 1;
}

static int gost_digest_init256(EVP_MD_CTX *ctx)
{
    init_gost2012_hash_ctx((gost2012_hash_ctx *) EVP_MD_CTX_md_data(ctx),
                           256);
    return 1;
}

static int gost_digest_update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
    gost2012_hash_block((gost2012_hash_ctx *) EVP_MD_CTX_md_data(ctx), data,
                        count);
    return 1;
}

static int gost_digest_final(EVP_MD_CTX *ctx, unsigned char *md)
{
    gost2012_finish_hash((gost2012_hash_ctx *) EVP_MD_CTX_md_data(ctx), md);
    return 1;
}

static int gost_digest_copy(EVP_MD_CTX *to, const EVP_MD_CTX *from)
{
    if (EVP_MD_CTX_md_data(to) && EVP_MD_CTX_md_data(from))
        memcpy(EVP_MD_CTX_md_data(to), EVP_MD_CTX_md_data(from),
               sizeof(gost2012_hash_ctx));

    return 1;
}

static int gost_digest_cleanup(EVP_MD_CTX *ctx)
{
    if (EVP_MD_CTX_md_data(ctx))
        memset(EVP_MD_CTX_md_data(ctx), 0x00, sizeof(gost2012_hash_ctx));

    return 1;
}

static int gost_digest_ctrl_256(EVP_MD_CTX *ctx, int type, int arg, void *ptr)
{
    switch (type) {
    case EVP_MD_CTRL_MICALG:
        {
            *((char **)ptr) = OPENSSL_malloc(strlen(micalg_256) + 1);
            if (*((char **)ptr) != NULL) {
                strcpy(*((char **)ptr), micalg_256);
                return 1;
            }
            return 0;
        }
    default:
        return 0;
    }
}

static int gost_digest_ctrl_512(EVP_MD_CTX *ctx, int type, int arg, void *ptr)
{
    switch (type) {
    case EVP_MD_CTRL_MICALG:
        {
            *((char **)ptr) = OPENSSL_malloc(strlen(micalg_512) + 1);
            if (*((char **)ptr) != NULL) {
                strcpy(*((char **)ptr), micalg_512);
                return 1;
            }
            return 0;
        }
        return 1;
    default:
        return 0;
    }
}
