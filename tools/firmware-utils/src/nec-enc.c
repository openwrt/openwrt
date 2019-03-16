/*
 * nec-enc.c - encode/decode nec firmware with key
 *
 * based on xorimage.c in OpenWrt
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define KEY_LEN     16
#define PATTERN_LEN 251

int xor_pattern(uint8_t *data, size_t len, const char *key, int k_len, int k_off)
{
	int offset = k_off;
	while (len--) {
		*data ^= key[offset];
		data++;
		offset = (offset + 1) % k_len;
	}
	return offset;
}

void xor_data(uint8_t *data, size_t len, const uint8_t *pattern)
{
	for (int i = 0; i < len; i++) {
		*data ^= pattern[i];
		data++;
	}
}

void usage(void)
{
	fprintf(stderr, "Usage: nec-enc [-i infile] [-o outfile] [-k <key>]\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	unsigned char buf[1024];
	FILE *in = stdin;
	FILE *out = stdout;
	char *ifn = NULL;
	char *ofn = NULL;
	unsigned char buf_pattern[1024];
	const char *key = NULL;
	int c;
	size_t n;
	int k_len, k_off = 0;
	int ptn = 0;

	while ((c = getopt(argc, argv, "i:o:k:h")) != -1) {
		switch (c) {
			case 'i':
				ifn = optarg;
				break;
			case 'o':
				ofn = optarg;
				break;
			case 'k':
				key = optarg;
				break;
			case 'h':
			default:
				usage();
		}
	}

	if (optind != argc || optind == 1) {
		fprintf(stderr, "illegal arg \"%s\"\n", argv[optind]);
		usage();
	}

	if (ifn && !(in = fopen(ifn, "r"))) {
		fprintf(stderr, "can not open \"%s\" for reading\n", ifn);
		usage();
	}

	if (ofn && !(out = fopen(ofn, "w"))) {
		fprintf(stderr, "can not open \"%s\" for writing\n", ofn);
		usage();
	}

	if (!key) {
		fprintf(stderr, "key is not specified\n");
		usage();
	}

	k_len = strlen(key);
	if (k_len == 0 || k_len > KEY_LEN) {
		fprintf (stderr, "key length is incorrect\n");
		usage();
	}

	while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
		if (n < sizeof(buf)) {
			if (ferror(in)) {
			FREAD_ERROR:
				fprintf(stderr, "fread error\n");
				return EXIT_FAILURE;
			}
		}

		for (int i = 0; i < n; i++) {
			buf_pattern[i] = ptn + 1;
			ptn++;
			if (ptn > 250) ptn = 0;
		}

		k_off = xor_pattern(buf_pattern, n, key, k_len, k_off);

		xor_data(buf, n, buf_pattern);

		if (!fwrite(buf, n, 1, out)) {
		FWRITE_ERROR:
			fprintf(stderr, "fwrite error\n");
			return EXIT_FAILURE;
		}
	}

	if (ferror(in)) {
		goto FREAD_ERROR;
	}

	if (fflush(out)) {
		goto FWRITE_ERROR;
	}

	fclose(in);
	fclose(out);

	return EXIT_SUCCESS;
}
