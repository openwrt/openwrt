// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 Felix Fietkau <nbd@nbd.name>
 */
#include "pk.h"
#include <mbedtls/x509_crt.h>
#include <mbedtls/ecp.h>
#include <mbedtls/rsa.h>
#include <mbedtls/asn1write.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/pkcs12.h>
#include <mbedtls/base64.h>
#include <mbedtls/sha1.h>

#define OID_TAG(n) MBEDTLS_OID_##n, MBEDTLS_OID_SIZE(MBEDTLS_OID_##n)

#ifndef MBEDTLS_OID_AES_256_CBC
#define MBEDTLS_OID_AES_256_CBC MBEDTLS_OID_AES "\x2a"
#endif

#define MBEDTLS_OID_PKCS9_LOCAL_KEY_ID		MBEDTLS_OID_PKCS9  "\x15"
#define MBEDTLS_OID_PKCS9_CERT_TYPE		MBEDTLS_OID_PKCS9  "\x16"
#define MBEDTLS_OID_PKCS9_CERT_TYPE_X509	MBEDTLS_OID_PKCS9_CERT_TYPE "\x01"

#define MBEDTLS_OID_PKCS12_KEY_BAG		MBEDTLS_OID_PKCS12 "\x0a\x01\x01"
#define MBEDTLS_OID_PKCS12_SHROUDED_KEY_BAG	MBEDTLS_OID_PKCS12 "\x0a\x01\x02"
#define MBEDTLS_OID_PKCS12_CERT_BAG		MBEDTLS_OID_PKCS12 "\x0a\x01\x03"

#ifndef MBEDTLS_OID_PKCS7
#define MBEDTLS_OID_PKCS7			MBEDTLS_OID_PKCS "\x07"
#define MBEDTLS_OID_PKCS7_DATA			MBEDTLS_OID_PKCS7 "\x01"
#define MBEDTLS_OID_PKCS7_ENCRYPTED_DATA	MBEDTLS_OID_PKCS7 "\x06"
#endif

#define NUM_ITER 2048
#define SALT_LEN 16
#define IV_LEN 16
#define IV_LEN_LEGACY 8
#define KEY_LEN 32
#define CERT_HASH_LEN 20

#define CONTEXT_TAG(n) (MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_ASN1_CONSTRUCTED | (n))
#define SEQUENCE_TAG (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)
#define SET_TAG (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SET)

#define CTX_ADD(ctx, n) \
	do {						\
		int __n = (n);				\
		if ((ctx)->p - (ctx)->start <= __n)	\
			return -1;			\
		(ctx)->p -= __n;			\
	} while (0)

struct pkcs12_ctx {
	uint8_t *p, *start;

	uint8_t cert_hash[20];
	uint8_t *key_cert_hash;

	uint8_t salt[SALT_LEN];
	uint8_t iv[IV_LEN];

	const char *password;
	uint8_t *pwd;
	size_t pwd_len;

	bool legacy;
};

#ifdef MBEDTLS_LEGACY
static inline int
mbedtls_pkcs5_pbkdf2_hmac_ext(mbedtls_md_type_t md_alg, const unsigned char *password,
			      size_t plen, const unsigned char *salt, size_t slen,
			      unsigned int iteration_count,
			      uint32_t key_length, unsigned char *output)
{
	mbedtls_md_context_t md_ctx;
	const mbedtls_md_info_t *md_info;
	int ret;

	md_info = mbedtls_md_info_from_type(md_alg);
	if (!md_info)
		return MBEDTLS_ERR_PKCS5_FEATURE_UNAVAILABLE;

	mbedtls_md_init(&md_ctx);
	ret = mbedtls_md_setup(&md_ctx, md_info, 1);
	if (ret)
		goto out;

	ret = mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, password, plen, salt, slen,
					iteration_count, key_length, output);

out:
	mbedtls_md_free(&md_ctx);
	return ret;
}
#endif

static void
uc_p12_add_tag(struct pkcs12_ctx *ctx, uint8_t *end, uint8_t tag)
{
	mbedtls_asn1_write_len(&ctx->p, ctx->start, end - ctx->p);
	mbedtls_asn1_write_tag(&ctx->p, ctx->start, tag);
}

static void
uc_p12_add_sequence(struct pkcs12_ctx *ctx, uint8_t *end)
{
	uc_p12_add_tag(ctx, end, SEQUENCE_TAG);
}

static void
uc_p12_add_algo(struct pkcs12_ctx *ctx, const char *oid, size_t oid_len, size_t par_len)
{
	mbedtls_asn1_write_algorithm_identifier(&ctx->p, ctx->start, oid, oid_len, par_len);
}

static void
uc_p12_add_attribute(struct pkcs12_ctx *ctx, uint8_t *end, const char *oid, size_t oid_len)
{
	uc_p12_add_tag(ctx, end, SET_TAG);
	mbedtls_asn1_write_oid(&ctx->p, ctx->start, oid, oid_len);
	uc_p12_add_sequence(ctx, end);
}

static void
uc_p12_add_localkeyid(struct pkcs12_ctx *ctx, bool key)
{
	uint8_t *end = ctx->p;

	ctx->p -= CERT_HASH_LEN;
	if (key)
		ctx->key_cert_hash = ctx->p;
	else if (ctx->key_cert_hash)
		memcpy(ctx->p, ctx->key_cert_hash, CERT_HASH_LEN);
	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);
	uc_p12_add_attribute(ctx, end, OID_TAG(PKCS9_LOCAL_KEY_ID));
}

static void
uc_p12_add_bag(struct pkcs12_ctx *ctx, uint8_t *data_end, uint8_t *end, const char *oid, size_t oid_len)
{
	uc_p12_add_tag(ctx, data_end, CONTEXT_TAG(0));
	mbedtls_asn1_write_oid(&ctx->p, ctx->start, oid, oid_len);
	uc_p12_add_sequence(ctx, end);
}

static int
uc_p12_enc_setup(struct pkcs12_ctx *ctx, mbedtls_cipher_context_t *cipher)
{
	const mbedtls_cipher_info_t *cipher_info;
	uint8_t key[KEY_LEN];
	int ret;

	random_cb(NULL, ctx->iv, IV_LEN);
	ret = mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA256,
					    (void *)ctx->password, strlen(ctx->password),
					    ctx->salt, SALT_LEN, NUM_ITER,
					    KEY_LEN, key);
	if (ret < 0)
		return ret;

	cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_256_CBC);
	ret = mbedtls_cipher_setup(cipher, cipher_info);
	if (ret < 0)
		return ret;

	return mbedtls_cipher_setkey(cipher, key, 8 * KEY_LEN, MBEDTLS_ENCRYPT);
}

static int
uc_p12_enc_legacy(struct pkcs12_ctx *ctx, mbedtls_cipher_context_t *cipher)
{
	const mbedtls_cipher_info_t *cipher_info;
	uint8_t key[24];
	int ret;

	ret = mbedtls_pkcs12_derivation(key, sizeof(key), ctx->pwd, ctx->pwd_len,
					ctx->salt, SALT_LEN, MBEDTLS_MD_SHA1,
					MBEDTLS_PKCS12_DERIVE_KEY, NUM_ITER);
	if (ret < 0)
		return ret;

	ret = mbedtls_pkcs12_derivation(ctx->iv, IV_LEN_LEGACY, ctx->pwd, ctx->pwd_len,
					ctx->salt, SALT_LEN, MBEDTLS_MD_SHA1,
					MBEDTLS_PKCS12_DERIVE_IV, NUM_ITER);
	if (ret < 0)
		return ret;

	cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_DES_EDE3_CBC);
	ret = mbedtls_cipher_setup(cipher, cipher_info);
	if (ret < 0)
		return ret;

	return mbedtls_cipher_setkey(cipher, key, 8 * sizeof(key), MBEDTLS_ENCRYPT);
}

static int
uc_p12_encrypt(struct pkcs12_ctx *ctx, uint8_t *end)
{
	mbedtls_cipher_context_t cipher;
	size_t iv_len = ctx->legacy ? IV_LEN_LEGACY : IV_LEN;
	size_t out_len = 0;
	int ret;

	random_cb(NULL, ctx->salt, SALT_LEN);

	if (ctx->legacy)
		ret = uc_p12_enc_legacy(ctx, &cipher);
	else
		ret = uc_p12_enc_setup(ctx, &cipher);
	if (ret < 0)
		goto out;

	ret = mbedtls_cipher_set_padding_mode(&cipher, MBEDTLS_PADDING_PKCS7);
	if (ret < 0)
		goto out;

	ret = mbedtls_cipher_crypt(&cipher, ctx->iv, iv_len, ctx->p, end - ctx->p,
				   (void *)buf, &out_len);

	if (ret < 0)
		goto out;

	CTX_ADD(ctx, out_len - (end - ctx->p));
	memcpy(ctx->p, buf, out_len);
	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);

out:
	mbedtls_cipher_free(&cipher);

	return ret;
}

static int
uc_p12_add_enc_params(struct pkcs12_ctx *ctx)
{
	uint8_t *par_end = ctx->p;
	uint8_t *kdf_end;

	if (ctx->legacy) {
		mbedtls_asn1_write_int(&ctx->p, ctx->start, NUM_ITER);

		CTX_ADD(ctx, SALT_LEN);
		memcpy(ctx->p, ctx->salt, SALT_LEN);
		uc_p12_add_tag(ctx, ctx->p + SALT_LEN, MBEDTLS_ASN1_OCTET_STRING);

		uc_p12_add_sequence(ctx, par_end);

		uc_p12_add_algo(ctx, OID_TAG(PKCS12_PBE_SHA1_DES3_EDE_CBC), par_end - ctx->p);
	} else {
		CTX_ADD(ctx, IV_LEN);
		memcpy(ctx->p, ctx->iv, IV_LEN);

		uc_p12_add_tag(ctx, par_end, MBEDTLS_ASN1_OCTET_STRING);
		uc_p12_add_algo(ctx, OID_TAG(AES_256_CBC), par_end - ctx->p);

		kdf_end = ctx->p;
		uc_p12_add_algo(ctx, OID_TAG(HMAC_SHA256), 0);
		mbedtls_asn1_write_int(&ctx->p, ctx->start, NUM_ITER);
		CTX_ADD(ctx, SALT_LEN);
		memcpy(ctx->p, ctx->salt, SALT_LEN);
		uc_p12_add_tag(ctx, ctx->p + SALT_LEN, MBEDTLS_ASN1_OCTET_STRING);
		uc_p12_add_sequence(ctx, kdf_end);

		uc_p12_add_algo(ctx, OID_TAG(PKCS5_PBKDF2), kdf_end - ctx->p);
		uc_p12_add_sequence(ctx, par_end);

		uc_p12_add_algo(ctx, OID_TAG(PKCS5_PBES2), par_end - ctx->p);
	}

	return 0;
}

static int
uc_p12_add_cert(struct pkcs12_ctx *ctx, uc_value_t *arg, bool ca)
{
	const char *str = ucv_string_get(arg), *str_end;
	uint8_t *bag_end, *end;
	size_t len;
	int ret;

#define START_TAG	"-----BEGIN CERTIFICATE-----"
#define END_TAG		"-----END CERTIFICATE-----"

	if (!str)
		return -1;

	str = strstr(str, START_TAG);
	if (!str)
		return -1;

	str += sizeof(START_TAG);
	str_end = strstr(str, END_TAG);
	if (!str_end)
		return -1;

	if ((size_t)(str_end - str) > sizeof(buf) / 2)
		return -1;

	ret = mbedtls_base64_decode((void *)buf, sizeof(buf) / 2, &len,
				    (const void *)str, str_end - str);
	if (ret)
		return ret;

	bag_end = ctx->p;
	if (!ca && ctx->key_cert_hash) {
		mbedtls_sha1((const void *)buf, len, ctx->key_cert_hash);
		uc_p12_add_localkeyid(ctx, false);
		uc_p12_add_tag(ctx, bag_end, SET_TAG);
	}

	end = ctx->p;
	CTX_ADD(ctx, len);
	memcpy(ctx->p, buf, len);
	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);

	/* CertBag */
	uc_p12_add_tag(ctx, end, CONTEXT_TAG(0));
	mbedtls_asn1_write_oid(&ctx->p, ctx->start, OID_TAG(PKCS9_CERT_TYPE_X509));
	uc_p12_add_sequence(ctx, end);

	uc_p12_add_bag(ctx, end, bag_end, OID_TAG(PKCS12_CERT_BAG));

	return 0;
}

static int
uc_p12_add_key(struct pkcs12_ctx *ctx, uc_value_t *arg)
{
	mbedtls_pk_context *pk = ucv_resource_data(arg, "mbedtls.pk");
	uint8_t *bag_end, *end;
	uint8_t *param = NULL;
	size_t param_len = 0;
	const char *oid;
	size_t oid_len = 0;
	int ret, len;

	if (!pk)
		return -1;

	bag_end = ctx->p;
	uc_p12_add_localkeyid(ctx, true);
	uc_p12_add_tag(ctx, bag_end, SET_TAG);

	end = ctx->p;
	len = mbedtls_pk_write_key_der(pk, (void *)ctx->start, end - ctx->start);
	if (len < 0)
		return len;

	ctx->p -= len;

	/* Convert EC key contents to PKCS#8 style */
	if (mbedtls_pk_get_type(pk) == MBEDTLS_PK_ECKEY) {
		mbedtls_ecp_group_id grp_id;
		mbedtls_asn1_buf tag_buf;
		uint8_t *pkey_start, *pkey_end;
		size_t seq_len, pkey_len, param_tag_len;
		uint8_t *p = ctx->p;
		uint8_t *_end = end;
		uint8_t *_start;
		int version;

		ret = mbedtls_asn1_get_tag(&p, end, &seq_len, SEQUENCE_TAG);
		if (ret < 0)
			return ret;

		_start = p;
		_end = p + seq_len;
		ret = mbedtls_asn1_get_int(&p, _end, &version);
		if (ret < 0)
			return ret;

		/* private key */
		ret = mbedtls_asn1_get_tag(&p, _end, &pkey_len, MBEDTLS_ASN1_OCTET_STRING);
		if (ret < 0)
			return ret;
		pkey_start = p;
		p += pkey_len;
		pkey_end = p;

		/* parameters */
		ret = mbedtls_asn1_get_tag(&p, _end, &param_len, CONTEXT_TAG(0));
		if (ret < 0)
			return ret;

		param = memcpy(buf, p, param_len);
		p += param_len;

		/* overwrite parameters */
		param_tag_len = p - pkey_end;
		ctx->p += param_tag_len;
		_start += param_tag_len;
		memmove(ctx->p, ctx->p - param_tag_len, p - ctx->p);

		/* replace sequence tag */
		ctx->p = _start;
		uc_p12_add_sequence(ctx, end);

		/* check for Curve25519 or Curve448 */
		tag_buf = (mbedtls_asn1_buf){
			.p = (uint8_t *)buf,
			.len = param_len,
		};
		tag_buf.tag = *tag_buf.p;
		ret = mbedtls_asn1_get_tag(&tag_buf.p, tag_buf.p + param_len, &tag_buf.len, tag_buf.tag);
		if (ret < 0)
			return ret;

		oid = MBEDTLS_OID_EC_ALG_UNRESTRICTED;
		oid_len = MBEDTLS_OID_SIZE(MBEDTLS_OID_EC_ALG_UNRESTRICTED);

#ifdef MBEDTLS_LEGACY
		(void)pkey_start;
		(void)grp_id;
#else
		ret = mbedtls_oid_get_ec_grp_algid(&tag_buf, &grp_id);
		if (!ret && (grp_id == MBEDTLS_ECP_DP_CURVE25519 ||
		             grp_id == MBEDTLS_ECP_DP_CURVE448)) {
			ctx->p = end - pkey_len;
			memmove(ctx->p, pkey_start, pkey_len);
			uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);
		}
#endif
	} else {
		mbedtls_oid_get_oid_by_pk_alg(mbedtls_pk_get_type(pk), &oid, &oid_len);
	}

	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);

	/* KeyBag */
	if (param_len) {
		CTX_ADD(ctx, param_len);
		memcpy(ctx->p, param, param_len);
	}
	uc_p12_add_algo(ctx, oid, oid_len, param_len);
	mbedtls_asn1_write_int(&ctx->p, ctx->start, 0);
	uc_p12_add_sequence(ctx, end);

	ret = uc_p12_encrypt(ctx, end);
	if (ret < 0)
		return ret;

	ret = uc_p12_add_enc_params(ctx);
	if (ret < 0)
		return ret;

	uc_p12_add_sequence(ctx, end);

	uc_p12_add_bag(ctx, end, bag_end, OID_TAG(PKCS12_SHROUDED_KEY_BAG));

	return 0;
}

static int
uc_p12_add_authsafe(struct pkcs12_ctx *ctx, uc_value_t *arg)
{
	uc_value_t *extra;
	uint8_t *end = ctx->p;
	size_t len;
	int ret;

	/* authSafe contents */
	extra = ucv_object_get(arg, "extra", NULL);
	if (extra) {
		size_t len;

		if (ucv_type(extra) != UC_ARRAY)
			return -1;

		len = ucv_array_length(extra);
		for (size_t i = 0; i < len; i++) {
			ret = uc_p12_add_cert(ctx, ucv_array_get(extra, i), true);
			if (ret < 0)
			    return ret;
		}
	}

	ret = uc_p12_add_key(ctx, ucv_object_get(arg, "key", NULL));
	if (ret < 0)
	    return ret;

	ret = uc_p12_add_cert(ctx, ucv_object_get(arg, "cert", NULL), false);
	if (ret < 0)
	    return ret;

	/* encrypted */
	uc_p12_add_sequence(ctx, end);

	ret = uc_p12_encrypt(ctx, end);
	if (ret < 0)
		 return ret;

	uc_p12_add_tag(ctx, end, CONTEXT_TAG(0));

	ret = uc_p12_add_enc_params(ctx);
	if (ret < 0)
		return ret;

	mbedtls_asn1_write_oid(&ctx->p, ctx->start, OID_TAG(PKCS7_DATA));
	uc_p12_add_sequence(ctx, end);

	mbedtls_asn1_write_int(&ctx->p, ctx->start, 0);
	uc_p12_add_sequence(ctx, end);
	uc_p12_add_tag(ctx, end, CONTEXT_TAG(0));
	mbedtls_asn1_write_oid(&ctx->p, ctx->start, OID_TAG(PKCS7_ENCRYPTED_DATA));
	uc_p12_add_sequence(ctx, end);

	/* authSafe contents */
	uc_p12_add_sequence(ctx, end);

	/* authSafe header */
	len = end - ctx->p;
	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);
	uc_p12_add_tag(ctx, end, CONTEXT_TAG(0));
	mbedtls_asn1_write_oid(&ctx->p, ctx->start, OID_TAG(PKCS7_DATA));
	uc_p12_add_sequence(ctx, end);

	return len;
}

static void *
uc_p12_add_mac_digest_info(struct pkcs12_ctx *ctx)
{
	uint8_t *end = ctx->p;
	size_t len = 20;

	ctx->p -= len;
	uc_p12_add_tag(ctx, end, MBEDTLS_ASN1_OCTET_STRING);
	uc_p12_add_algo(ctx, OID_TAG(DIGEST_ALG_SHA1), 0);
	uc_p12_add_sequence(ctx, end);

	return end - len;
}

uc_value_t *uc_generate_pkcs12(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *arg = uc_fn_arg(0), *pwd_arg;
	mbedtls_md_context_t hmac;
	uint8_t *end = (uint8_t *)&buf[sizeof(buf)];
	struct pkcs12_ctx ctx = {
		.start = (uint8_t *)&buf[sizeof(buf) / 2],
		.p = end,
	};
	uint8_t *hash, *data;
	uint8_t key[20], *salt;
	size_t salt_len = 8;
	int data_len;
	uc_value_t *ret = NULL;
	const char *passwd;

	if (ucv_type(arg) != UC_OBJECT)
		INVALID_ARG();

	ctx.legacy = ucv_is_truish(ucv_object_get(arg, "legacy", NULL));

	mbedtls_md_init(&hmac);
	pwd_arg = ucv_object_get(arg, "password", NULL);
	passwd = ucv_string_get(pwd_arg);
	if (!passwd)
		INVALID_ARG();

	/* password is expected to be a UTF-16 string */
	ctx.password = passwd;
	ctx.pwd_len = 2 * strlen(passwd) + 2;
	ctx.pwd = malloc(ctx.pwd_len);
	for (size_t i = 0; i < ctx.pwd_len; i += 2) {
		ctx.pwd[i] = 0;
		ctx.pwd[i + 1] = passwd[i / 2];
	}

	/* MacData */
	mbedtls_asn1_write_int(&ctx.p, ctx.start, NUM_ITER);

	ctx.p -= salt_len;
	salt = ctx.p;
	random_cb(NULL, salt, salt_len);
	uc_p12_add_tag(&ctx, salt + salt_len, MBEDTLS_ASN1_OCTET_STRING);

	hash = uc_p12_add_mac_digest_info(&ctx);
	uc_p12_add_sequence(&ctx, end);

	data = ctx.p;
	data_len = uc_p12_add_authsafe(&ctx, arg);
	if (C(data_len) < 0)
	    goto out;
	data -= data_len;

	mbedtls_asn1_write_int(&ctx.p, ctx.start, 3);
	uc_p12_add_sequence(&ctx, end);

	if (C(mbedtls_pkcs12_derivation(key, sizeof(key), ctx.pwd, ctx.pwd_len,
					salt, salt_len, MBEDTLS_MD_SHA1,
					MBEDTLS_PKCS12_DERIVE_MAC_KEY, NUM_ITER)))
		goto out;

	if (C(mbedtls_md_setup(&hmac, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 1)))
		goto out;
	if (C(mbedtls_md_hmac_starts(&hmac, key, sizeof(key))))
		goto out;
	if (C(mbedtls_md_hmac_update(&hmac, data, data_len)))
		goto out;
	if (C(mbedtls_md_hmac_finish(&hmac, hash)))
		goto out;

	ret = ucv_string_new_length((char *)ctx.p, end - ctx.p);

out:
	free(ctx.pwd);
	mbedtls_md_free(&hmac);
	return ret;
}
