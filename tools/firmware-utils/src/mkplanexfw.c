/*
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>     /* for unlink() */
#include <libgen.h>
#include <getopt.h>     /* for getopt() */
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#include "sha1.h"

#if (__BYTE_ORDER == __BIG_ENDIAN)
#  define HOST_TO_BE32(x)	(x)
#  define BE32_TO_HOST(x)	(x)
#else
#  define HOST_TO_BE32(x)	bswap_32(x)
#  define BE32_TO_HOST(x)	bswap_32(x)
#endif


struct planex_hdr {
	uint8_t		sha1sum[20];
	char		version[8];
	uint8_t		unk1[2];
	uint32_t	datalen;
} __attribute__ ((packed));

/*
 * Globals
 */
static char *ifname;
static char *progname;
static char *ofname;
static char *version = "1.00.00";

/*
 * Message macros
 */
#define ERR(fmt, ...) do { \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt "\n", \
			progname, ## __VA_ARGS__ ); \
} while (0)

#define ERRS(fmt, ...) do { \
	int save = errno; \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt "\n", \
			progname, ## __VA_ARGS__, strerror(save)); \
} while (0)

void usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;
	struct board_info *board;

	fprintf(stream, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -i <file>       read input from the file <file>\n"
"  -o <file>       write output to the file <file>\n"
"  -v <version>    set image version to <version>\n"
"  -h              show this screen\n"
	);

	exit(status);
}

int main(int argc, char *argv[])
{
	int res = EXIT_FAILURE;
	int buflen;
	int err;
	struct stat st;
	char *buf;
	struct planex_hdr *hdr;
	sha1_context ctx;
	uint32_t t = HOST_TO_BE32(2);

	FILE *outfile, *infile;

	progname = basename(argv[0]);

	while ( 1 ) {
		int c;

		c = getopt(argc, argv, "i:o:v:h");
		if (c == -1)
			break;

		switch (c) {
		case 'i':
			ifname = optarg;
			break;
		case 'o':
			ofname = optarg;
			break;
		case 'v':
			version = optarg;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (ifname == NULL) {
		ERR("no input file specified");
		goto err;
	}

	if (ofname == NULL) {
		ERR("no output file specified");
		goto err;
	}

	err = stat(ifname, &st);
	if (err){
		ERRS("stat failed on %s", ifname);
		goto err;
	}

	buflen = (st.st_size + 3) & ~3;
	buflen += sizeof(*hdr);

	buf = malloc(buflen);
	if (!buf) {
		ERR("no memory for buffer\n");
		goto err;
	}

	memset(buf, 0xff, buflen);
	hdr = (struct planex_hdr *)buf;

	hdr->datalen = HOST_TO_BE32(buflen - sizeof(*hdr));
	hdr->unk1[0] = 0x04;
	hdr->unk1[1] = 0x08;

	snprintf(hdr->version, sizeof(hdr->version), "%s", version);

	infile = fopen(ifname, "r");
	if (infile == NULL) {
		ERRS("could not open \"%s\" for reading", ifname);
		goto err_free;
	}

	errno = 0;
	fread(buf +  sizeof(*hdr), st.st_size, 1, infile);
	if (errno != 0) {
		ERRS("unable to read from file %s", ifname);
		goto err_close_in;
	}

	sha1_starts(&ctx);
	sha1_update(&ctx, (uchar *) &t, sizeof(t));
	sha1_update(&ctx, buf + sizeof(*hdr), buflen - sizeof(*hdr));
	sha1_finish(&ctx, hdr->sha1sum);

	outfile = fopen(ofname, "w");
	if (outfile == NULL) {
		ERRS("could not open \"%s\" for writing", ofname);
		goto err_close_in;
	}

	errno = 0;
	fwrite(buf, buflen, 1, outfile);
	if (errno) {
		ERRS("unable to write to file %s", ofname);
		goto err_close_out;
	}

	res = EXIT_SUCCESS;

 out_flush:
	fflush(outfile);

 err_close_out:
	fclose(outfile);
	if (res != EXIT_SUCCESS) {
		unlink(ofname);
	}

 err_close_in:
	fclose(infile);

 err_free:
	free(buf);

 err:
	return res;
}

