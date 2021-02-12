/**********************************************************************
 *                        gost_ec_keyx.c                              *
 *             Copyright (c) 2005-2013 Cryptocom LTD                  *
 *         This file is distributed under the same license as OpenSSL *
 *                                                                    *
 *   VK0 34.10-2001 key exchange and GOST R 34.10-2001                *
 *   based PKCS7/SMIME support                                        *
 *          Requires OpenSSL 0.9.9 for compilation                    *
 **********************************************************************/
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>
#include <openssl/objects.h>
#include "gost89.h"
#include "e_gost_err.h"
#include "gost_keywrap.h"
#include "gost_lcl.h"

/* Implementation of CryptoPro VKO 34.10-2001/2012 algorithm */
static int VKO_compute_key(unsigned char *shared_key, size_t shared_key_size,
                           const EC_POINT *pub_key, EC_KEY *priv_key,
                           const unsigned char *ukm, int dgst_nid)
{
    unsigned char *databuf = NULL;
    BIGNUM *UKM = NULL, *p = NULL, *order = NULL, *X = NULL, *Y = NULL;
    const BIGNUM *key = EC_KEY_get0_private_key(priv_key);
    EC_POINT *pnt = EC_POINT_new(EC_KEY_get0_group(priv_key));
    BN_CTX *ctx = BN_CTX_new();
    EVP_MD_CTX *mdctx = NULL;
    const EVP_MD *md = NULL;
    int effective_dgst_nid = (dgst_nid == NID_id_GostR3411_2012_512) ?
        NID_id_GostR3411_2012_256 : dgst_nid;
    int buf_len = (dgst_nid == NID_id_GostR3411_2012_512) ? 128 : 64,
        half_len = buf_len >> 1;
		int ret = 0;

    if (!ctx) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    BN_CTX_start(ctx);

    databuf = OPENSSL_zalloc(buf_len);
    if (!databuf) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    md = EVP_get_digestbynid(effective_dgst_nid);
    if (!md) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, GOST_R_INVALID_DIGEST_TYPE);
        goto err;
    }

    UKM = hashsum2bn(ukm, 8);
    p = BN_CTX_get(ctx);
    order = BN_CTX_get(ctx);
    X = BN_CTX_get(ctx);
    Y = BN_CTX_get(ctx);
    EC_GROUP_get_order(EC_KEY_get0_group(priv_key), order, ctx);
    BN_mod_mul(p, key, UKM, order, ctx);
    if (!EC_POINT_mul
        (EC_KEY_get0_group(priv_key), pnt, NULL, pub_key, p, ctx)) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, GOST_R_ERROR_POINT_MUL);
        goto err;
    }
    EC_POINT_get_affine_coordinates_GFp(EC_KEY_get0_group(priv_key),
                                        pnt, X, Y, ctx);
    /*
     * Serialize elliptic curve point same way as we do it when saving key
     */
    store_bignum(Y, databuf, half_len);
    store_bignum(X, databuf + half_len, half_len);
    /* And reverse byte order of whole buffer */
		BUF_reverse(databuf, NULL, buf_len);

    mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        GOSTerr(GOST_F_VKO_COMPUTE_KEY, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, databuf, buf_len);
    EVP_DigestFinal_ex(mdctx, shared_key, NULL);
		ret = 32;

 err:
    BN_free(UKM);
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);

    EC_POINT_free(pnt);

    EVP_MD_CTX_free(mdctx);

    OPENSSL_free(databuf);

    return ret;
}

/*
 * EVP_PKEY_METHOD callback derive.
 * Implements VKO R 34.10-2001/2012 algorithms
 */
int pkey_gost_ec_derive(EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen)
{
    /*
     * Public key of peer in the ctx field peerkey
     * Our private key in the ctx pkey
     * ukm is in the algorithm specific context data
     */
    EVP_PKEY *my_key = EVP_PKEY_CTX_get0_pkey(ctx);
    EVP_PKEY *peer_key = EVP_PKEY_CTX_get0_peerkey(ctx);
    struct gost_pmeth_data *data = EVP_PKEY_CTX_get_data(ctx);
    int dgst_nid = NID_undef;

    if (!data || !data->shared_ukm) {
        GOSTerr(GOST_F_PKEY_GOST_EC_DERIVE, GOST_R_UKM_NOT_SET);
        return 0;
    }

    if (key == NULL) {
        *keylen = 32;
        return 1;
    }

    EVP_PKEY_get_default_digest_nid(my_key, &dgst_nid);

    *keylen =
        VKO_compute_key(key, 32,
                        EC_KEY_get0_public_key(EVP_PKEY_get0(peer_key)),
                        (EC_KEY *)EVP_PKEY_get0(my_key), data->shared_ukm,
                        dgst_nid);
    return (*keylen) ? 1 : 0;
}

/*
 * Generates ephemeral key based on pubk algorithm computes shared key using
 * VKO and returns filled up GOST_KEY_TRANSPORT structure
 */

/*
 * EVP_PKEY_METHOD callback encrypt
 * Implementation of GOST2001 key transport, cryptopo variation
 */

int pkey_GOST_ECcp_encrypt(EVP_PKEY_CTX *pctx, unsigned char *out,
                           size_t *out_len, const unsigned char *key,
                           size_t key_len)
{
    GOST_KEY_TRANSPORT *gkt = NULL;
    EVP_PKEY *pubk = EVP_PKEY_CTX_get0_pkey(pctx);
    struct gost_pmeth_data *data = EVP_PKEY_CTX_get_data(pctx);
    int pkey_nid = EVP_PKEY_base_id(pubk);
    ASN1_OBJECT *crypt_params_obj = (pkey_nid == NID_id_GostR3410_2001) ?
        OBJ_nid2obj(NID_id_Gost28147_89_CryptoPro_A_ParamSet) :
        OBJ_nid2obj(NID_id_tc26_gost_28147_param_Z);
    const struct gost_cipher_info *param =
        get_encryption_params(crypt_params_obj);
    unsigned char ukm[8], shared_key[32], crypted_key[44];
    int ret = 0;
    int key_is_ephemeral = 1;
    gost_ctx cctx;
    EVP_PKEY *sec_key = EVP_PKEY_CTX_get0_peerkey(pctx);
    if (data->shared_ukm) {
        memcpy(ukm, data->shared_ukm, 8);
    } else if (out) {

        if (RAND_bytes(ukm, 8) <= 0) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT, GOST_R_RNG_ERROR);
            return 0;
        }
    }
    /* Check for private key in the peer_key of context */
    if (sec_key) {
        key_is_ephemeral = 0;
        if (!gost_get0_priv_key(sec_key)) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT,
                    GOST_R_NO_PRIVATE_PART_OF_NON_EPHEMERAL_KEYPAIR);
            goto err;
        }
    } else {
        key_is_ephemeral = 1;
        if (out) {
            sec_key = EVP_PKEY_new();
            if (! EVP_PKEY_assign(sec_key, EVP_PKEY_base_id(pubk), EC_KEY_new()) ||
			          ! EVP_PKEY_copy_parameters(sec_key, pubk) ||
                ! gost_ec_keygen(EVP_PKEY_get0(sec_key))) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT,
                    GOST_R_ERROR_COMPUTING_SHARED_KEY);
                goto err;
            }
        }
    }
    if (!get_gost_engine_param(GOST_PARAM_CRYPT_PARAMS)
        && param == gost_cipher_list) {
        param = gost_cipher_list;
    }
    if (out) {
        int dgst_nid = NID_undef;
        EVP_PKEY_get_default_digest_nid(pubk, &dgst_nid);

        if (!VKO_compute_key(shared_key, 32,
                             EC_KEY_get0_public_key(EVP_PKEY_get0(pubk)),
                             EVP_PKEY_get0(sec_key), ukm, dgst_nid)) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT,
                    GOST_R_ERROR_COMPUTING_SHARED_KEY);
            goto err;
        }
        gost_init(&cctx, param->sblock);
        keyWrapCryptoPro(&cctx, shared_key, ukm, key, crypted_key);
    }
    gkt = GOST_KEY_TRANSPORT_new();
    if (!gkt) {
        goto err;
    }
    if (!ASN1_OCTET_STRING_set(gkt->key_agreement_info->eph_iv, ukm, 8)) {
        goto err;
    }
    if (!ASN1_OCTET_STRING_set(gkt->key_info->imit, crypted_key + 40, 4)) {
        goto err;
    }
    if (!ASN1_OCTET_STRING_set
        (gkt->key_info->encrypted_key, crypted_key + 8, 32)) {
        goto err;
    }
    if (key_is_ephemeral) {
        if (!X509_PUBKEY_set
            (&gkt->key_agreement_info->ephem_key, out ? sec_key : pubk)) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT,
                    GOST_R_CANNOT_PACK_EPHEMERAL_KEY);
            goto err;
        }
    }
    ASN1_OBJECT_free(gkt->key_agreement_info->cipher);
    gkt->key_agreement_info->cipher = OBJ_nid2obj(param->nid);
    if (key_is_ephemeral)
        EVP_PKEY_free(sec_key);
    if (!key_is_ephemeral) {
        /* Set control "public key from client certificate used" */
        if (EVP_PKEY_CTX_ctrl(pctx, -1, -1, EVP_PKEY_CTRL_PEER_KEY, 3, NULL)
            <= 0) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_ENCRYPT, GOST_R_CTRL_CALL_FAILED);
            goto err;
        }
    }
    if ((*out_len = i2d_GOST_KEY_TRANSPORT(gkt, out ? &out : NULL)) > 0)
        ret = 1;
    GOST_KEY_TRANSPORT_free(gkt);
    return ret;
 err:
    if (key_is_ephemeral)
        EVP_PKEY_free(sec_key);
    GOST_KEY_TRANSPORT_free(gkt);
    return -1;
}

/*
 * EVP_PKEY_METHOD callback decrypt
 * Implementation of GOST2001 key transport, cryptopo variation
 */
int pkey_GOST_ECcp_decrypt(EVP_PKEY_CTX *pctx, unsigned char *key,
                           size_t *key_len, const unsigned char *in,
                           size_t in_len)
{
    const unsigned char *p = in;
    EVP_PKEY *priv = EVP_PKEY_CTX_get0_pkey(pctx);
    GOST_KEY_TRANSPORT *gkt = NULL;
    int ret = 0;
    unsigned char wrappedKey[44];
    unsigned char sharedKey[32];
    gost_ctx ctx;
    const struct gost_cipher_info *param = NULL;
    EVP_PKEY *eph_key = NULL, *peerkey = NULL;
    int dgst_nid = NID_undef;

    if (!key) {
        *key_len = 32;
        return 1;
    }
    gkt = d2i_GOST_KEY_TRANSPORT(NULL, (const unsigned char **)&p, in_len);
    if (!gkt) {
        GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT,
                GOST_R_ERROR_PARSING_KEY_TRANSPORT_INFO);
        return -1;
    }

    /* If key transport structure contains public key, use it */
    eph_key = X509_PUBKEY_get(gkt->key_agreement_info->ephem_key);
    if (eph_key) {
        if (EVP_PKEY_derive_set_peer(pctx, eph_key) <= 0) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT,
                    GOST_R_INCOMPATIBLE_PEER_KEY);
            goto err;
        }
    } else {
        /* Set control "public key from client certificate used" */
        if (EVP_PKEY_CTX_ctrl(pctx, -1, -1, EVP_PKEY_CTRL_PEER_KEY, 3, NULL)
            <= 0) {
            GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT, GOST_R_CTRL_CALL_FAILED);
            goto err;
        }
    }
    peerkey = EVP_PKEY_CTX_get0_peerkey(pctx);
    if (!peerkey) {
        GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT, GOST_R_NO_PEER_KEY);
        goto err;
    }

    param = get_encryption_params(gkt->key_agreement_info->cipher);
    if (!param) {
        goto err;
    }

    gost_init(&ctx, param->sblock);
    OPENSSL_assert(gkt->key_agreement_info->eph_iv->length == 8);
    memcpy(wrappedKey, gkt->key_agreement_info->eph_iv->data, 8);
    OPENSSL_assert(gkt->key_info->encrypted_key->length == 32);
    memcpy(wrappedKey + 8, gkt->key_info->encrypted_key->data, 32);
    OPENSSL_assert(gkt->key_info->imit->length == 4);
    memcpy(wrappedKey + 40, gkt->key_info->imit->data, 4);

    EVP_PKEY_get_default_digest_nid(priv, &dgst_nid);
    if (!VKO_compute_key(sharedKey, 32,
                         EC_KEY_get0_public_key(EVP_PKEY_get0(peerkey)),
                         EVP_PKEY_get0(priv), wrappedKey, dgst_nid)) {
        GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT,
                GOST_R_ERROR_COMPUTING_SHARED_KEY);
        goto err;
    }
    if (!keyUnwrapCryptoPro(&ctx, sharedKey, wrappedKey, key)) {
        GOSTerr(GOST_F_PKEY_GOST_ECCP_DECRYPT,
                GOST_R_ERROR_COMPUTING_SHARED_KEY);
        goto err;
    }

    ret = 1;
 err:
    EVP_PKEY_free(eph_key);
    GOST_KEY_TRANSPORT_free(gkt);
    return ret;
}
