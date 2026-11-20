// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 Felix Fietkau <nbd@nbd.name>
 */
#include <sys/types.h>
#include <sys/random.h>

#include <stdint.h>
#include <limits.h>
#include <errno.h>

#include <mbedtls/entropy.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/ecp.h>
#include <mbedtls/rsa.h>

#include <ucode/module.h>

#include "pk.h"

/* mbedtls < 3.x compat */
#ifdef MBEDTLS_LEGACY
#define mbedtls_pk_parse_key(pk, key, keylen, passwd, passwdlen, random, random_ctx) \
	mbedtls_pk_parse_key(pk, key, keylen, passwd, passwdlen)
#endif

static uc_resource_type_t *uc_pk_type, *uc_crt_type;
static uc_value_t *registry;
char buf[32 * 1024];
int mbedtls_errno;

struct uc_cert_wr {
	mbedtls_x509write_cert crt; /* must be first */
	mbedtls_mpi mpi;
	unsigned int reg;
};

static unsigned int uc_reg_add(uc_value_t *val)
{
	size_t i = 0;

	while (ucv_array_get(registry, i))
		i++;

	ucv_array_set(registry, i, ucv_get(val));

	return i;
}

int random_cb(void *ctx, unsigned char *out, size_t len)
{
#ifdef linux
	if (getrandom(out, len, 0) != (ssize_t) len)
		return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
#else
	static FILE *f;

	if (!f)
		f = fopen("/dev/urandom", "r");
	if (fread(out, len, 1, f) != 1)
		return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
#endif

	return 0;
}

int64_t get_int_arg(uc_value_t *obj, const char *key, int64_t defval)
{
	uc_value_t *uval = ucv_object_get(obj, key, NULL);
	int64_t val;

	if (!uval)
		return defval;

	val = ucv_int64_get(uval);
	if (errno || val < 0 || val > INT_MAX)
		return INT_MIN;

	return val ? val : defval;
}

static int
gen_rsa_key(mbedtls_pk_context *pk, uc_value_t *arg)
{
	int64_t key_size, exp;

	key_size = get_int_arg(arg, "size", 2048);
	exp = get_int_arg(arg, "exponent", 65537);
	if (key_size < 0 || exp < 0)
		return -1;

	return mbedtls_rsa_gen_key(mbedtls_pk_rsa(*pk), random_cb, NULL, key_size, exp);
}

static int
gen_ec_key(mbedtls_pk_context *pk, uc_value_t *arg)
{
	mbedtls_ecp_group_id curve;
	const char *c_name;
	uc_value_t *c_arg;

	c_arg = ucv_object_get(arg, "curve", NULL);
	if (c_arg && ucv_type(c_arg) != UC_STRING)
		return -1;

	c_name = ucv_string_get(c_arg);
	if (!c_name)
		curve = MBEDTLS_ECP_DP_SECP256R1;
	else {
		const mbedtls_ecp_curve_info *curve_info;
		curve_info = mbedtls_ecp_curve_info_from_name(c_name);
		if (!curve_info)
			return MBEDTLS_ERR_PK_UNKNOWN_NAMED_CURVE;

		curve = curve_info->grp_id;
	}

	return mbedtls_ecp_gen_key(curve, mbedtls_pk_ec(*pk), random_cb, NULL);
}

static void free_pk(void *pk)
{
	if (!pk)
		return;

	mbedtls_pk_free(pk);
	free(pk);
}

static void free_crt(void *ptr)
{
	struct uc_cert_wr *crt = ptr;

	if (!crt)
		return;

	mbedtls_x509write_crt_free(&crt->crt);
	mbedtls_mpi_free(&crt->mpi);
	ucv_array_set(registry, crt->reg, NULL);
	free(crt);
}

static uc_value_t *
uc_generate_key(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *cur, *arg = uc_fn_arg(0);
	mbedtls_pk_type_t pk_type;
	mbedtls_pk_context *pk;
	const char *type;
	int ret;

	if (ucv_type(arg) != UC_OBJECT)
		INVALID_ARG();

	cur = ucv_object_get(arg, "type", NULL);
	type = ucv_string_get(cur);
	if (!type)
		INVALID_ARG();

	if (!strcmp(type, "rsa"))
		pk_type = MBEDTLS_PK_RSA;
	else if (!strcmp(type, "ec"))
		pk_type = MBEDTLS_PK_ECKEY;
	else
		INVALID_ARG();

	pk = calloc(1, sizeof(*pk));
	mbedtls_pk_init(pk);
	mbedtls_pk_setup(pk, mbedtls_pk_info_from_type(pk_type));
	switch (pk_type) {
	case MBEDTLS_PK_RSA:
		ret = C(gen_rsa_key(pk, arg));
		break;
	case MBEDTLS_PK_ECKEY:
		ret = C(gen_ec_key(pk, arg));
		break;
	default:
		ret = -1;
	}

	if (ret) {
		free_pk(pk);
		return NULL;
	}

	return uc_resource_new(uc_pk_type, pk);
}

static uc_value_t *
uc_load_key(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *keystr = uc_fn_arg(0);
	uc_value_t *pub = uc_fn_arg(1);
	uc_value_t *passwd = uc_fn_arg(2);
	mbedtls_pk_context *pk;
	int ret;

	if (ucv_type(keystr) != UC_STRING ||
	    (pub && ucv_type(pub) != UC_BOOLEAN) ||
	    (passwd && ucv_type(passwd) != UC_STRING))
		INVALID_ARG();

	pk = calloc(1, sizeof(*pk));
	mbedtls_pk_init(pk);
	if (ucv_is_truish(pub))
		ret = C(mbedtls_pk_parse_public_key(pk, (const uint8_t *)ucv_string_get(keystr),
						    ucv_string_length(keystr) + 1));
	else
		ret = C(mbedtls_pk_parse_key(pk, (const uint8_t *)ucv_string_get(keystr),
					     ucv_string_length(keystr) + 1,
					     (const uint8_t *)ucv_string_get(passwd),
					     ucv_string_length(passwd) + 1,
					     random_cb, NULL));
	if (ret) {
		free_pk(pk);
		return NULL;
	}

	return uc_resource_new(uc_pk_type, pk);
}

static void
uc_cert_info_add(uc_value_t *info, const char *name, int len)
{
	uc_value_t *val;

	if (len < 0)
		return;

	val = ucv_string_new_length(buf, len);
	ucv_object_add(info, name, ucv_get(val));
}

static void
uc_cert_info_add_name(uc_value_t *info, const char *name, mbedtls_x509_name *dn)
{
	int len;

	len = mbedtls_x509_dn_gets(buf, sizeof(buf), dn);
	uc_cert_info_add(info, name, len);
}

static void
uc_cert_info_add_time(uc_value_t *info, const char *name, mbedtls_x509_time *t)
{
	int len;

	len = snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02d",
		       t->year, t->mon, t->day, t->hour, t->min, t->sec);
	uc_cert_info_add(info, name, len);
}

static uc_value_t *
uc_cert_info(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *arg = uc_fn_arg(0);
	mbedtls_x509_crt crt, *cur;
	uc_value_t *ret = NULL;

	if (ucv_type(arg) != UC_STRING)
		return NULL;

	mbedtls_x509_crt_init(&crt);
	if (C(mbedtls_x509_crt_parse(&crt, (const void *)ucv_string_get(arg), ucv_string_length(arg) + 1)) < 0)
		goto out;

	ret = ucv_array_new(vm);
	for (cur = &crt; cur && cur->version != 0; cur = cur->next) {
		uc_value_t *info = ucv_object_new(vm);
		int len;

		ucv_array_push(ret, info);
		ucv_object_add(info, "version", ucv_int64_new(cur->version));

		uc_cert_info_add_name(info, "issuer", &cur->issuer);
		uc_cert_info_add_name(info, "subject", &cur->issuer);
		uc_cert_info_add_time(info, "valid_from", &cur->valid_from);
		uc_cert_info_add_time(info, "valid_to", &cur->valid_to);

		len = mbedtls_x509_serial_gets(buf, sizeof(buf), &cur->serial);
		uc_cert_info_add(info, "serial", len);
	}

out:
	mbedtls_x509_crt_free(&crt);
	return ret;
}

static uc_value_t *
uc_pk_pem(uc_vm_t *vm, size_t nargs)
{
	mbedtls_pk_context *pk = uc_fn_thisval("mbedtls.pk");
	uc_value_t *pub = uc_fn_arg(0);

	if (!pk)
		return NULL;

	if (ucv_is_truish(pub))
		CHECK(mbedtls_pk_write_pubkey_pem(pk, (void *)buf, sizeof(buf)));
	else
		CHECK(mbedtls_pk_write_key_pem(pk, (void *)buf, sizeof(buf)));

	return ucv_string_new(buf);
}

static uc_value_t *
uc_pk_der(uc_vm_t *vm, size_t nargs)
{
	mbedtls_pk_context *pk = uc_fn_thisval("mbedtls.pk");
	uc_value_t *pub = uc_fn_arg(0);
	int len;

	if (!pk)
		return NULL;

	if (ucv_is_truish(pub))
		len = mbedtls_pk_write_pubkey_der(pk, (void *)buf, sizeof(buf));
	else
		len = mbedtls_pk_write_key_der(pk, (void *)buf, sizeof(buf));
	if (len < 0)
		CHECK(len);

	return ucv_string_new_length(buf + sizeof(buf) - len, len);
}

static uc_value_t *
uc_crt_pem(uc_vm_t *vm, size_t nargs)
{
	mbedtls_x509write_cert *crt = uc_fn_thisval("mbedtls.crt");
	if (!crt)
		return NULL;

	CHECK(mbedtls_x509write_crt_pem(crt, (void *)buf, sizeof(buf), random_cb, NULL));

	return ucv_string_new(buf);
}

static uc_value_t *
uc_crt_der(uc_vm_t *vm, size_t nargs)
{
	mbedtls_x509write_cert *crt = uc_fn_thisval("mbedtls.crt");
	int len;

	if (!crt)
		return NULL;

	len = mbedtls_x509write_crt_der(crt, (void *)buf, sizeof(buf), random_cb, NULL);
	if (len < 0)
		CHECK(len);

	return ucv_string_new_length(buf, len);
}

static int
uc_cert_set_validity(mbedtls_x509write_cert *crt, uc_value_t *arg)
{
	uc_value_t *from = ucv_array_get(arg, 0);
	uc_value_t *to = ucv_array_get(arg, 1);

	if (ucv_type(from) != UC_STRING || ucv_type(to) != UC_STRING)
		return -1;

	return mbedtls_x509write_crt_set_validity(crt, ucv_string_get(from), ucv_string_get(to));
}

static int
uc_cert_init(mbedtls_x509write_cert *crt, mbedtls_mpi *mpi, uc_value_t *reg, uc_value_t *arg)
{
	uc_value_t *cur;
	int64_t serial;
	int path_len;
	int version;
	bool ca;

	mbedtls_mpi_init(mpi);
	mbedtls_x509write_crt_init(crt);
	mbedtls_x509write_crt_set_md_alg(crt, MBEDTLS_MD_SHA256);

	ca = ucv_is_truish(ucv_object_get(arg, "ca", NULL));
	path_len = get_int_arg(arg, "max_pathlen", ca ? -1 : 0);
	if (path_len < -1)
		return -1;

	version = get_int_arg(arg, "version", 3);
	if (version < 0 || version > 3)
		return -1;

	serial = get_int_arg(arg, "serial", 1);
	if (serial < 0)
		return -1;

	mbedtls_mpi_lset(mpi, serial);
	mbedtls_x509write_crt_set_serial(crt, mpi);
	mbedtls_x509write_crt_set_version(crt, version - 1);
	CHECK_INT(mbedtls_x509write_crt_set_basic_constraints(crt, ca, path_len));

	cur = ucv_object_get(arg, "subject_name", NULL);
	if (ucv_type(cur) == UC_STRING)
		CHECK_INT(mbedtls_x509write_crt_set_subject_name(crt, ucv_string_get(cur)));
	else
		return -1;
	cur = ucv_object_get(arg, "subject_key", NULL);
	if (cur) {
		mbedtls_pk_context *key = ucv_resource_data(cur, "mbedtls.pk");
		if (!key)
			return -1;

		ucv_array_set(reg, 0, ucv_get(cur));
		mbedtls_x509write_crt_set_subject_key(crt, key);
		mbedtls_x509write_crt_set_subject_key_identifier(crt);
	} else
		return -1;

	cur = ucv_object_get(arg, "issuer_name", NULL);
	if (ucv_type(cur) == UC_STRING)
		CHECK_INT(mbedtls_x509write_crt_set_issuer_name(crt, ucv_string_get(cur)));
	else
		return -1;
	cur = ucv_object_get(arg, "issuer_key", NULL);
	if (cur) {
		mbedtls_pk_context *key = ucv_resource_data(cur, "mbedtls.pk");
		if (!key)
			return -1;

		ucv_array_set(reg, 1, ucv_get(cur));
		mbedtls_x509write_crt_set_issuer_key(crt, key);
		mbedtls_x509write_crt_set_authority_key_identifier(crt);
	} else
		return -1;

	cur = ucv_object_get(arg, "validity", NULL);
	if (ucv_type(cur) != UC_ARRAY || ucv_array_length(cur) != 2)
		return -1;
	if (uc_cert_set_validity(crt, cur))
		return -1;

	cur = ucv_object_get(arg, "key_usage", NULL);
	if (ucv_type(cur) == UC_ARRAY) {
		static const struct {
			const char *name;
			uint8_t val;
		} key_flags[] = {
			{ "digital_signature", MBEDTLS_X509_KU_DIGITAL_SIGNATURE },
			{ "non_repudiation", MBEDTLS_X509_KU_NON_REPUDIATION },
			{ "key_encipherment", MBEDTLS_X509_KU_KEY_ENCIPHERMENT },
			{ "data_encipherment", MBEDTLS_X509_KU_DATA_ENCIPHERMENT },
			{ "key_agreement", MBEDTLS_X509_KU_KEY_AGREEMENT },
			{ "key_cert_sign", MBEDTLS_X509_KU_KEY_CERT_SIGN },
			{ "crl_sign", MBEDTLS_X509_KU_CRL_SIGN },
		};
		uint8_t key_usage = 0;
		size_t len = ucv_array_length(cur);

		for (size_t i = 0; i < len; i++) {
			uc_value_t *val = ucv_array_get(cur, i);
			const char *str;
			size_t k;

			str = ucv_string_get(val);
			if (!str)
				return -1;

			for (k = 0; k < ARRAY_SIZE(key_flags); k++)
				if (!strcmp(str, key_flags[k].name))
					break;
			if (k == ARRAY_SIZE(key_flags))
				return -1;

			key_usage |= key_flags[k].val;
		}
		CHECK_INT(mbedtls_x509write_crt_set_key_usage(crt, key_usage));
	} else if (cur)
		return -1;

#ifndef MBEDTLS_LEGACY
	cur = ucv_object_get(arg, "ext_key_usage", NULL);
	if (ucv_type(cur) == UC_ARRAY && ucv_array_length(cur)) {
		static const struct {
			const char *name;
			struct mbedtls_asn1_buf val;
		} key_flags[] = {
#define __oid(name, val)					\
	{							\
		name,						\
		{						\
			.tag = MBEDTLS_ASN1_OID,		\
			.len = sizeof(MBEDTLS_OID_##val),	\
			.p = (uint8_t *)MBEDTLS_OID_##val,	\
		}						\
	}
			__oid("server_auth", SERVER_AUTH),
			__oid("client_auth", CLIENT_AUTH),
			__oid("code_signing", CODE_SIGNING),
			__oid("email_protection", EMAIL_PROTECTION),
			__oid("time_stamping", TIME_STAMPING),
			__oid("ocsp_signing", OCSP_SIGNING),
			__oid("any", ANY_EXTENDED_KEY_USAGE)
		};
		struct mbedtls_asn1_sequence *elem;
		size_t len = ucv_array_length(cur);

		elem = calloc(len, sizeof(*elem));
		for (size_t i = 0; i < len; i++) {
			uc_value_t *val = ucv_array_get(cur, i);
			const char *str;
			size_t k;

			str = ucv_string_get(val);
			if (!str)
				return -1;

			for (k = 0; k < ARRAY_SIZE(key_flags); k++)
				if (!strcmp(str, key_flags[k].name))
					break;

			if (k == ARRAY_SIZE(key_flags)) {
				free(elem);
				return -1;
			}
			elem[i].buf = key_flags[k].val;
			if (i + 1 < len)
				elem[i].next = &elem[i + 1];
		}

		CHECK_INT(mbedtls_x509write_crt_set_ext_key_usage(crt, elem));
	} else if (cur)
		return -1;
#endif

	return 0;
}

static uc_value_t *
uc_generate_cert(uc_vm_t *vm, size_t nargs)
{
	struct uc_cert_wr *crt;
	uc_value_t *arg = uc_fn_arg(0);
	uc_value_t *reg;

	if (ucv_type(arg) != UC_OBJECT)
		return NULL;

	reg = ucv_array_new(vm);
	crt = calloc(1, sizeof(*crt));
	if (C(uc_cert_init(&crt->crt, &crt->mpi, reg, arg))) {
		free(crt);
		return NULL;
	}

	crt->reg = uc_reg_add(reg);

	return uc_resource_new(uc_crt_type, crt);
}

static uc_value_t *
uc_mbedtls_error(uc_vm_t *vm, size_t nargs)
{
	mbedtls_strerror(mbedtls_errno, buf, sizeof(buf));

	return ucv_string_new(buf);
}

static uc_value_t *
uc_mbedtls_errno(uc_vm_t *vm, size_t nargs)
{
	return ucv_int64_new(mbedtls_errno);
}


static const uc_function_list_t pk_fns[] = {
	{ "pem", uc_pk_pem },
	{ "der", uc_pk_der },
};

static const uc_function_list_t crt_fns[] = {
	{ "pem", uc_crt_pem },
	{ "der", uc_crt_der },
};

static const uc_function_list_t global_fns[] = {
	{ "load_key", uc_load_key },
	{ "cert_info", uc_cert_info },
	{ "generate_key", uc_generate_key },
	{ "generate_cert", uc_generate_cert },
	{ "generate_pkcs12", uc_generate_pkcs12 },
	{ "errno", uc_mbedtls_errno },
	{ "error", uc_mbedtls_error },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	uc_pk_type = uc_type_declare(vm, "mbedtls.pk", pk_fns, free_pk);
	uc_crt_type = uc_type_declare(vm, "mbedtls.crt", crt_fns, free_crt);
	uc_function_list_register(scope, global_fns);

	registry = ucv_array_new(vm);
	uc_vm_registry_set(vm, "pkgen.registry", registry);
}
