/*
 * v7apac1200.c - partially based on OpenWrt's xorimage.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include "md5.h"

#define BUF_SIZE                0x200

static uint _pattern = 0x30471688;

static int md5_file(const char *filename, uint8_t *dst)
{
        FILE *fp_src;
        MD5_CTX ctx;
        char buf[BUF_SIZE];
        size_t bytes_read;

        MD5_Init(&ctx);

        fp_src = fopen(filename, "r+b");
        if (!fp_src) {
                return -1;
        }
        while (!feof(fp_src)) {
                bytes_read = fread(&buf, 1, BUF_SIZE, fp_src);
                MD5_Update(&ctx, &buf, bytes_read);
        }
        fclose(fp_src);

        MD5_Final(dst, &ctx);

        return 0;
}

int write_header(FILE *out, char* md5sum){
	char header1[70] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
		0x47, 0x45, 0x54, 0x5f, 0x53, 0x54, 0x41, 0x47,
		0x49, 0x4e, 0x47, 0x2f, 0x61, 0x70, 0x70, 0x73,
		0x5f, 0x31, 0x2e, 0x30, 0x2e, 0x31, 0x30, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xff, 0xff, 0x00, 0x12, 0x33, 0x30,
		0x34, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65,
		0xf0, 0x00, 0x00, 0x00, 0x00, 0x00 };
	char header2[42] = {
		0xdd, 0xbf, 0x94, 0x1a, 0x02, 0x40, 0x00, 0x00,
		0x00, 0x00, 0x48, 0x4b, 0x02, 0x40, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x00, 0x00, 0x38, 0x19, 0x02, 0x40, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,	0x07, 0x18, 0x30, 0x47,
		0x16, 0x88 };

	if (!fwrite(header1, sizeof(header1), 1, out)) {
		fprintf(stderr, "fwrite error\n");
		return EXIT_FAILURE;
	}

	if (!fwrite(md5sum, 0x10, 1, out)) {
		fprintf(stderr, "fwrite error\n");
		return EXIT_FAILURE;
	}

	if (!fwrite(header2, sizeof(header2), 1, out)) {
		fprintf(stderr, "fwrite error\n");
		return EXIT_FAILURE;
	}
	return 0;
}

void usage(void) __attribute__ (( __noreturn__ ));

void usage(void)
{
	fprintf(stderr, "Usage: v7apac1200 [-i infile] [-o outfile] [-d]\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	char buf[1];	/* keep this at 1k or adjust garbage calc below */
	FILE *in = stdin;
	FILE *out = stdout;
        char decode = 0;
	char *ifn = NULL;
	char *ofn = NULL;
	int c, count;
	int v0, v1, v2;
	size_t n;
	int p_len, p_off = 0;
        uint8_t  md5sum[0x10];

	while ((c = getopt(argc, argv, "i:o:p:h")) != -1) {
		switch (c) {
			case 'i':
				ifn = optarg;
				break;
			case 'o':
				ofn = optarg;
				break;
			case 'd':
				decode = optarg;
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

	if (ifn && md5_file(ifn, md5sum)) {
		fprintf(stderr, "can not get md5sum for \"%s\"\n", ifn);
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

	if (ofn && (write_header(out, md5sum))) {
		fprintf(stderr, "can not write header to \"%s\"\n", ofn);
		usage();
	}


        count = 0;
	while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
		if (n < sizeof(buf)) {
			if (ferror(in)) {
			FREAD_ERROR:
				fprintf(stderr, "fread error\n");
				return EXIT_FAILURE;
			}
		}
		buf[0] = (buf[0] ^ (_pattern >> count)) & 0xff;

		if (!fwrite(buf, n, 1, out)) {
		FWRITE_ERROR:
			fprintf(stderr, "fwrite error\n");
			return EXIT_FAILURE;
		}
		count = (count + 1) % 8;
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
