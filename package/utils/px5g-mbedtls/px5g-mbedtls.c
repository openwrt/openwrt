/*
 * px5g - Embedded x509 key and certificate generator based on PolarSSL
 *
 *   Copyright (C) 2009 Steven Barth <steven@midlink.org>
 *   Copyright (C) 2014 Felix Fietkau <nbd@nbd.name>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <sys/types.h>
#include <sys/random.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>

#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/asn1.h>
#include <mbedtls/oid.h>
#include <psa/crypto.h>

#define SET_OID(x, oid) \
	do { x.len = MBEDTLS_OID_SIZE(oid); x.p = (unsigned char *) oid; } while (0)

#define PX5G_VERSION "0.3"
#define PX5G_COPY "Copyright (c) 2009 Steven Barth <steven@midlink.org>"
#define PX5G_LICENSE "Licensed under the GNU Lesser General Public License v2.1"

static char buf[16384];

static void write_file(const char *path, size_t len, bool pem, bool cert)
{
	mode_t mode = S_IRUSR | S_IWUSR;
	const char *buf_start = buf;
	int fd = STDERR_FILENO;
	ssize_t written;
	int err;

	if (!pem)
		buf_start += sizeof(buf) - len;

	if (!len) {
		fprintf(stderr, "No data to write\n");
		exit(1);
	}

	if (cert)
		mode |= S_IRGRP | S_IROTH;

	if (path)
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);

	if (fd < 0) {
		fprintf(stderr, "error: I/O error\n");
		exit(1);
	}

	written = write(fd, buf_start, len);
	if (written != len) {
		fprintf(stderr, "writing key failed with: %s\n", strerror(errno));
		exit(1);
	}
	err = fsync(fd);
	if (err < 0) {
		fprintf(stderr, "syncing key failed with: %s\n", strerror(errno));
		exit(1);
	}
	if (path)
		close(fd);
}

static const struct {
	const char *name;
	psa_ecc_family_t family;
	size_t bits;
} ecp_curves[] = {
#if defined(PSA_WANT_ECC_SECP_R1_256)
	{ "P-256", PSA_ECC_FAMILY_SECP_R1, 256 },
	{ "secp256r1", PSA_ECC_FAMILY_SECP_R1, 256 },
#endif
#if defined(PSA_WANT_ECC_SECP_R1_384)
	{ "P-384", PSA_ECC_FAMILY_SECP_R1, 384 },
	{ "secp384r1", PSA_ECC_FAMILY_SECP_R1, 384 },
#endif
#if defined(PSA_WANT_ECC_SECP_R1_521)
	{ "P-521", PSA_ECC_FAMILY_SECP_R1, 521 },
	{ "secp521r1", PSA_ECC_FAMILY_SECP_R1, 521 },
#endif
#if defined(PSA_WANT_ECC_SECP_K1_256)
	{ "secp256k1", PSA_ECC_FAMILY_SECP_K1, 256 },
#endif
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
	{ "brainpoolP256r1", PSA_ECC_FAMILY_BRAINPOOL_P_R1, 256 },
#endif
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
	{ "brainpoolP384r1", PSA_ECC_FAMILY_BRAINPOOL_P_R1, 384 },
#endif
#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
	{ "brainpoolP512r1", PSA_ECC_FAMILY_BRAINPOOL_P_R1, 512 },
#endif
	{ NULL, 0, 0 },
};

static int ecp_curve(const char *name, psa_ecc_family_t *family, size_t *bits)
{
	size_t i;

	for (i = 0; ecp_curves[i].name; i++) {
		if (!strcmp(name, ecp_curves[i].name)) {
			*family = ecp_curves[i].family;
			*bits = ecp_curves[i].bits;
			return 0;
		}
	}

	return -1;
}

static void write_key(mbedtls_pk_context *key, const char *path, bool pem)
{
	int len = 0;

	if (pem) {
		if (mbedtls_pk_write_key_pem(key, (void *) buf, sizeof(buf)) == 0)
			len = strlen(buf);
	} else {
		len = mbedtls_pk_write_key_der(key, (void *) buf, sizeof(buf));
		if (len < 0)
			len = 0;
	}

	write_file(path, len, pem, false);
}

static void gen_key(mbedtls_pk_context *key, bool rsa, int ksize,
		    psa_ecc_family_t family, size_t bits)
{
	psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
	mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
	psa_status_t status;

	if (!rsa && !family) {
		fprintf(stderr, "error: no EC curves enabled in this build\n");
		exit(1);
	}

	if (rsa) {
		fprintf(stderr, "Generating RSA private key, %i bit long modulus\n", ksize);
		psa_set_key_type(&attr, PSA_KEY_TYPE_RSA_KEY_PAIR);
		psa_set_key_bits(&attr, ksize);
		psa_set_key_algorithm(&attr, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
	} else {
		fprintf(stderr, "Generating EC private key\n");
		psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(family));
		psa_set_key_bits(&attr, bits);
		psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
	}
	psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);

	status = psa_generate_key(&attr, &key_id);
	if (status == PSA_ERROR_NOT_SUPPORTED) {
		fprintf(stderr, "error: key type or size not supported in this build\n");
		exit(1);
	}
	if (status != PSA_SUCCESS) {
		fprintf(stderr, "error: key generation failed (%d)\n", (int) status);
		exit(1);
	}

	mbedtls_pk_init(key);
	if (mbedtls_pk_copy_from_psa(key_id, key) != 0) {
		fprintf(stderr, "error: key import failed\n");
		psa_destroy_key(key_id);
		exit(1);
	}
	psa_destroy_key(key_id);
}

int dokey(bool rsa, char **arg)
{
	mbedtls_pk_context key;
	unsigned int ksize = 512;
	char *path = NULL;
	bool pem = true;
	psa_ecc_family_t family = ecp_curves[0].family;
	size_t bits = ecp_curves[0].bits;

	while (*arg && **arg == '-') {
		if (!strcmp(*arg, "-out") && arg[1]) {
			path = arg[1];
			arg++;
		} else if (rsa && !strcmp(*arg, "-3")) {
			fprintf(stderr, "error: -3 is not supported, RSA keys use exponent 65537\n");
			return 1;
		} else if (!strcmp(*arg, "-der")) {
			pem = false;
		}
		arg++;
	}

	if (*arg && rsa) {
		ksize = (unsigned int)atoi(*arg);
	} else if (*arg) {
		if (ecp_curve((const char *)*arg, &family, &bits)) {
			fprintf(stderr, "error: invalid curve name: %s\n", *arg);
			return 1;
		}
	}

	gen_key(&key, rsa, ksize, family, bits);
	write_key(&key, path, pem);
	mbedtls_pk_free(&key);
	return 0;
}

int selfsigned(char **arg)
{
	mbedtls_pk_context key;
	mbedtls_x509write_cert cert;
	unsigned char serial[8];
	mbedtls_x509_san_list *san_list = NULL, *san_prev = NULL, *san_cur = NULL;
	/*support
	- MBEDTLS_X509_SAN_DNS_NAME
	- MBEDTLS_X509_SAN_IP_ADDRESS
	- MBEDTLS_X509_SAN_RFC822_NAME
	- MBEDTLS_X509_SAN_UNIFORM_RESOURCE_IDENTIFIER
	*/
	mbedtls_asn1_sequence *eku = NULL, *ext_key_usage = NULL;
	char *sanval, *santype;
	uint8_t ipaddr[16] = { 0 };

	char *subject = "";
	unsigned int ksize = 512;
	unsigned int days = 30;
	char *keypath = NULL, *certpath = NULL;
	bool pem = true;
	time_t from = time(NULL), to;
	char fstr[20], tstr[20];
	int len;
	bool rsa = true;
	psa_ecc_family_t family = ecp_curves[0].family;
	size_t bits = ecp_curves[0].bits;

	while (*arg && **arg == '-') {
		if (!strcmp(*arg, "-der")) {
			pem = false;
		} else if (!strcmp(*arg, "-newkey") && arg[1]) {
			if (!strncmp(arg[1], "rsa:", 4)) {
				rsa = true;
				ksize = (unsigned int)atoi(arg[1] + 4);
			} else if (!strcmp(arg[1], "ec")) {
				rsa = false;
			} else {
				fprintf(stderr, "error: invalid algorithm\n");
				return 1;
			}
			arg++;
		} else if (!strcmp(*arg, "-days") && arg[1]) {
			days = (unsigned int)atoi(arg[1]);
			arg++;
		} else if (!strcmp(*arg, "-pkeyopt") && arg[1]) {
			if (strncmp(arg[1], "ec_paramgen_curve:", 18)) {
				fprintf(stderr, "error: invalid pkey option: %s\n", arg[1]);
				return 1;
			}
			if (ecp_curve((const char *)(arg[1] + 18), &family, &bits)) {
				fprintf(stderr, "error: invalid curve name: %s\n", arg[1] + 18);
				return 1;
			}
			arg++;
		} else if (!strcmp(*arg, "-keyout") && arg[1]) {
			keypath = arg[1];
			arg++;
		} else if (!strcmp(*arg, "-out") && arg[1]) {
			certpath = arg[1];
			arg++;
		} else if (!strcmp(*arg, "-subj") && arg[1]) {
			if (arg[1][0] != '/' || strchr(arg[1], ';')) {
				fprintf(stderr, "error: invalid subject");
				return 1;
			}
			subject = calloc(strlen(arg[1]) + 1, 1);
			char *oldc = arg[1] + 1, *newc = subject, *delim;
			do {
				delim = strchr(oldc, '=');
				if (!delim) {
					fprintf(stderr, "error: invalid subject");
					return 1;
				}
				memcpy(newc, oldc, delim - oldc + 1);
				newc += delim - oldc + 1;
				oldc = delim + 1;

				delim = strchr(oldc, '/');
				if (!delim) {
					delim = arg[1] + strlen(arg[1]);
				}
				memcpy(newc, oldc, delim - oldc);
				newc += delim - oldc;
				*newc++ = ',';
				oldc = delim + 1;
			} while(*delim);
			arg++;
		} else if (!strcmp(*arg, "-addext") && arg[1]) {
			mbedtls_asn1_sequence **tail = &eku;
			if (!strncmp(arg[1], "extendedKeyUsage=", strlen("extendedKeyUsage="))) {
				ext_key_usage = calloc(1, sizeof(mbedtls_asn1_sequence));
				ext_key_usage->buf.tag = MBEDTLS_ASN1_OID;
				if (!strncmp(arg[1] + strlen("extendedKeyUsage="), "serverAuth", strlen("serverAuth"))) {
					SET_OID(ext_key_usage->buf, MBEDTLS_OID_SERVER_AUTH);
				} else if (!strncmp(arg[1] + strlen("extendedKeyUsage="), "any", strlen("any"))) {
					SET_OID(ext_key_usage->buf, MBEDTLS_OID_ANY_EXTENDED_KEY_USAGE);
				} // there are other extendedKeyUsage OIDs but none conceivably useful here
				*tail = ext_key_usage;
				tail = &ext_key_usage->next;
				arg++;
			} else if (!strncmp(arg[1], "subjectAltName=", strlen("subjectAltName=")) && strchr(arg[1], ':') != NULL) {
				santype = strchr(arg[1], '=') + 1;
				sanval = strchr(arg[1], ':') + 1;
				//build sAN list
				san_cur = calloc(1, sizeof(mbedtls_x509_san_list));
				san_cur->next = NULL;
				if (!strncmp(santype, "DNS:", strlen("DNS:"))) {
					san_cur->node.type = MBEDTLS_X509_SAN_DNS_NAME;
					san_cur->node.san.unstructured_name.p = (unsigned char *) sanval;
					san_cur->node.san.unstructured_name.len = strlen(sanval);
				} else if (!strncmp(santype, "EMAIL:", strlen("EMAIL:"))) {
					san_cur->node.type = MBEDTLS_X509_SAN_RFC822_NAME;
					san_cur->node.san.unstructured_name.p = (unsigned char *) sanval;
					san_cur->node.san.unstructured_name.len = strlen(sanval);
				} else if (!strncmp(santype, "IP:", strlen("IP:"))) {
					san_cur->node.type = MBEDTLS_X509_SAN_IP_ADDRESS;
					mbedtls_x509_crt_parse_cn_inet_pton(sanval, ipaddr);
					san_cur->node.san.unstructured_name.p = (unsigned char *) ipaddr;
					san_cur->node.san.unstructured_name.len = sizeof(ipaddr);
				} else if (!strncmp(santype, "URI:", strlen("URI:"))) {
					san_cur->node.type = MBEDTLS_X509_SAN_UNIFORM_RESOURCE_IDENTIFIER;
					san_cur->node.san.unstructured_name.p = (unsigned char *) sanval;
					san_cur->node.san.unstructured_name.len = strlen(sanval);
				}
				else fprintf(stderr, "No match to subjectAltName content type.\n");
			arg++;
			}
		}
		arg++;

		//set the pointers in our san_list linked list
		if (san_prev == NULL) {
			san_list = san_cur;
		} else {
			san_prev->next = san_cur;
		}
		san_prev = san_cur;
	}

	gen_key(&key, rsa, ksize, family, bits);

	if (keypath)
		write_key(&key, keypath, pem);

	from = (from < 1000000000) ? 1000000000 : from;
	strftime(fstr, sizeof(fstr), "%Y%m%d%H%M%S", gmtime(&from));
	to = from + 60 * 60 * 24 * days;
	if (to < from)
		to = INT_MAX;
	strftime(tstr, sizeof(tstr), "%Y%m%d%H%M%S", gmtime(&to));

	fprintf(stderr, "Generating selfsigned certificate with subject '%s'"
			" and validity %s-%s\n", subject, fstr, tstr);

	mbedtls_x509write_crt_init(&cert);
	mbedtls_x509write_crt_set_md_alg(&cert, MBEDTLS_MD_SHA256);
	mbedtls_x509write_crt_set_issuer_key(&cert, &key);
	mbedtls_x509write_crt_set_subject_key(&cert, &key);
	mbedtls_x509write_crt_set_subject_name(&cert, subject);
	mbedtls_x509write_crt_set_issuer_name(&cert, subject);
	mbedtls_x509write_crt_set_validity(&cert, fstr, tstr);
	mbedtls_x509write_crt_set_basic_constraints(&cert, 0, -1);
	mbedtls_x509write_crt_set_subject_key_identifier(&cert);
	mbedtls_x509write_crt_set_authority_key_identifier(&cert);
	mbedtls_x509write_crt_set_subject_alternative_name(&cert, san_list);
	mbedtls_x509write_crt_set_ext_key_usage(&cert, ext_key_usage);

	if (getrandom(serial, sizeof(serial), 0) != (ssize_t) sizeof(serial)) {
		fprintf(stderr, "error: failed to generate serial number\n");
		return 1;
	}
	mbedtls_x509write_crt_set_serial_raw(&cert, serial, sizeof(serial));

	if (pem) {
		if (mbedtls_x509write_crt_pem(&cert, (void *) buf, sizeof(buf)) < 0) {
			fprintf(stderr, "Failed to generate certificate\n");
			return 1;
		}

		len = strlen(buf);
	} else {
		len = mbedtls_x509write_crt_der(&cert, (void *) buf, sizeof(buf));
		if (len < 0) {
			fprintf(stderr, "Failed to generate certificate: %d\n", len);
			return 1;
		}
	}
	write_file(certpath, len, pem, true);

	mbedtls_x509write_crt_free(&cert);
	mbedtls_pk_free(&key);

	return 0;
}

int main(int argc, char *argv[])
{
	if (psa_crypto_init() != PSA_SUCCESS) {
		fprintf(stderr, "error: failed to initialize PSA crypto\n");
		return 1;
	}

	if (!argv[1]) {
		//Usage
	} else if (!strcmp(argv[1], "eckey")) {
		return dokey(false, argv+2);
	} else if (!strcmp(argv[1], "rsakey")) {
		return dokey(true, argv+2);
	} else if (!strcmp(argv[1], "selfsigned")) {
		return selfsigned(argv+2);
	}

	fprintf(stderr,
		"PX5G X.509 Certificate Generator Utility v" PX5G_VERSION "\n" PX5G_COPY
		"\nbased on PolarSSL by Christophe Devine and Paul Bakker\n\n");
	fprintf(stderr, "Usage: %s [eckey|rsakey|selfsigned]\n", *argv);
	return 1;
}
