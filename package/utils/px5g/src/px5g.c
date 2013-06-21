/*
 * px5g - Embedded x509 key and certificate generator based on PolarSSL
 *
 *   Copyright (C) 2009 Steven Barth <steven@midlink.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "polarssl/havege.h"
#include "polarssl/bignum.h"
#include "polarssl/x509.h"
#include "polarssl/rsa.h"

#define PX5G_VERSION "0.1"
#define PX5G_COPY "Copyright (c) 2009 Steven Barth <steven@midlink.org>"
#define PX5G_LICENSE "Licensed under the GNU Lesser General Public License v2.1"

int rsakey(char **arg) {
	havege_state hs;
	rsa_context rsa;

	unsigned int ksize = 512;
	int exp = 65537;
	char *path = NULL;
	int flag = X509_OUTPUT_PEM;

	while (*arg && **arg == '-') {
		if (!strcmp(*arg, "-out") && arg[1]) {
			path = arg[1];
			arg++;
		} else if (!strcmp(*arg, "-3")) {
			exp = 3;
		} else if (!strcmp(*arg, "-der")) {
			flag = X509_OUTPUT_DER;
		}
		arg++;
	}

	if (*arg) {
		ksize = (unsigned int)atoi(*arg);
	}

	havege_init(&hs);
	rsa_init(&rsa, RSA_PKCS_V15, 0, havege_rand, &hs);

	fprintf(stderr, "Generating RSA private key, %i bit long modulus\n", ksize);
	if (rsa_gen_key(&rsa, ksize, exp)) {
		fprintf(stderr, "error: key generation failed\n");
		return 1;
	}

	if (x509write_keyfile(&rsa, path, flag)) {
		fprintf(stderr, "error: I/O error\n");
		return 1;
	}

	rsa_free(&rsa);
	return 0;
}

int selfsigned(char **arg) {
	havege_state hs;
	rsa_context rsa;
	x509_node node;

	char *subject = "";
	unsigned int ksize = 512;
	int exp = 65537;
	unsigned int days = 30;
	char *keypath = NULL, *certpath = NULL;
	int flag = X509_OUTPUT_PEM;
	time_t from = time(NULL), to;
	char fstr[20], tstr[20];

	while (*arg && **arg == '-') {
		if (!strcmp(*arg, "-der")) {
			flag = X509_OUTPUT_DER;
		} else if (!strcmp(*arg, "-newkey") && arg[1]) {
			if (strncmp(arg[1], "rsa:", 4)) {
				fprintf(stderr, "error: invalid algorithm");
				return 1;
			}
			ksize = (unsigned int)atoi(arg[1] + 4);
			arg++;
		} else if (!strcmp(*arg, "-days") && arg[1]) {
			days = (unsigned int)atoi(arg[1]);
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
				*newc++ = ';';
				oldc = delim + 1;
			} while(*delim);
			arg++;
		}
		arg++;
	}

	havege_init(&hs);
	rsa_init(&rsa, RSA_PKCS_V15, 0, havege_rand, &hs);
	x509write_init_node(&node);
	fprintf(stderr, "Generating RSA private key, %i bit long modulus\n", ksize);
	if (rsa_gen_key(&rsa, ksize, exp)) {
		fprintf(stderr, "error: key generation failed\n");
		return 1;
	}

	if (keypath) {
		if (x509write_keyfile(&rsa, keypath, flag)) {
			fprintf(stderr, "error: I/O error\n");
			return 1;
		}
	}

	from = (from < 1000000000) ? 1000000000 : from;
	strftime(fstr, sizeof(fstr), "%F %H:%M:%S", gmtime(&from));
	to = from + 60 * 60 * 24 * days;
	strftime(tstr, sizeof(tstr), "%F %H:%M:%S", gmtime(&to));

	x509_raw cert;
	x509write_init_raw(&cert);
	x509write_add_pubkey(&cert, &rsa);
	x509write_add_subject(&cert, (unsigned char*)subject);
	x509write_add_validity(&cert, (unsigned char*)fstr, (unsigned char*)tstr);
	fprintf(stderr, "Generating selfsigned certificate with subject '%s'"
			" and validity %s-%s\n", subject, fstr, tstr);
	if (x509write_create_selfsign(&cert, &rsa)) {
		fprintf(stderr, "error: certificate generation failed\n");
	}

	if (x509write_crtfile(&cert, (unsigned char*)certpath, flag)) {
		fprintf(stderr, "error: I/O error\n");
		return 1;
	}

	x509write_free_raw(&cert);
	rsa_free(&rsa);
	return 0;
}

int main(int argc, char *argv[]) {
	if (!argv[1]) {
		//Usage
	} else if (!strcmp(argv[1], "rsakey")) {
		return rsakey(argv+2);
	} else if (!strcmp(argv[1], "selfsigned")) {
		return selfsigned(argv+2);
	}

	fprintf(stderr,
		"PX5G X.509 Certificate Generator Utility v" PX5G_VERSION "\n" PX5G_COPY
		"\nbased on PolarSSL by Christophe Devine and Paul Bakker\n\n");
	fprintf(stderr, "Usage: %s [rsakey|selfsigned]\n", *argv);
	return 1;
}
