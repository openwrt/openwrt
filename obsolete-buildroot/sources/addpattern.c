/*
 * Copyright (C) 2004  Manuel Novoa III  <mjn3@codepoet.org>
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

/* July 29, 2004
 *
 * This is a hacked replacement for the 'addpattern' utility used to
 * create wrt54g .bin firmware files.  It isn't pretty, but it does
 * the job for me.
 *
 * Extensions:
 *  -v allows setting the version string on the command line.
 *  -{0|1} sets the (currently ignored) hw_ver flag in the header
 *      to 0 or 1 respectively.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

/**********************************************************************/

#define CODE_ID		"U2ND"		/* from code_pattern.h */
#define CODE_PATTERN   "W54S"	/* from code_pattern.h */

#define CYBERTAN_VERSION	"v2.07.1" /* from cyutils.h */
/* #define CYBERTAN_VERSION	"v2.04.3" */

struct code_header {			/* from cyutils.h */
	char magic[4];
	char res1[4];				/* for extra magic */
	char fwdate[3];
	char fwvern[3];
	char id[4];					/* U2ND */
#if 0
	unsigned char res2[14];
#else
	char hw_ver;    			/* 0: for 4702, 1: for 4712 -- new in 2.04.3 */
	unsigned char res2[13];
#endif
} ;

/**********************************************************************/

void usage(void) __attribute__ (( __noreturn__ ));

void usage(void)
{
	fprintf(stderr, "Usage: addpattern [-i trxfile] [-o binfile] [-p pattern] [-g] [-v v#.#.#] [-{0|1}]\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	char buf[1024];	/* keep this at 1k or adjust garbage calc below */
	struct code_header *hdr;
	FILE *in = stdin;
	FILE *out = stdout;
	char *ifn = NULL;
	char *ofn = NULL;
	char *pattern = CODE_PATTERN;
	char *version = CYBERTAN_VERSION;
	int gflag = 0;
	int c;
	int v0, v1, v2;
	size_t off, n;
	time_t t;
	struct tm *ptm;

	fprintf(stderr, "mjn3's addpattern replacement - v0.80\n");

	hdr = (struct code_header *) buf;

	while ((c = getopt(argc, argv, "i:o:p:gv:01")) != -1) {
		switch (c) {
			case 'i':
				ifn = optarg;
				break;
			case 'o':
				ofn = optarg;
				break;
			case 'p':
				pattern = optarg;
				break;
			case 'g':
				gflag = 1;
				break;
			case 'v':			/* extension to allow setting version */
				version = optarg;
				break;
			case '0':
				hdr->hw_ver = 0;
				break;
			case '1':
				hdr->hw_ver = 1;
				break;
			default:
				usage();
		}
	}

	if (optind != argc) {
		fprintf(stderr, "illegal arg \"%s\"\n", argv[optind]);
		usage();
	}

	if (strlen(pattern) != 4) {
		fprintf(stderr, "illegal pattern \"%s\": length != 4\n", pattern);
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

	if (time(&t) == (time_t)(-1)) {
		fprintf(stderr, "time call failed\n");
		return EXIT_FAILURE;
	}

	ptm = localtime(&t);

	if (3 != sscanf(version, "v%d.%d.%d", &v0, &v1, &v2)) {
		fprintf(stderr, "bad version string \"%s\"\n", version);
		return EXIT_FAILURE;
	}

	memset(hdr, 0, sizeof(struct code_header));
	memcpy(&hdr->magic, pattern, 4);
	hdr->fwdate[0] = ptm->tm_year % 100;
	hdr->fwdate[1] = ptm->tm_mon + 1;
	hdr->fwdate[2] = ptm->tm_mday;
	hdr->fwvern[0] = v0;
	hdr->fwvern[1] = v1;
	hdr->fwvern[2] = v2;
	memcpy(&hdr->id, CODE_ID, strlen(CODE_ID));

	off = sizeof(struct code_header);

	fprintf(stderr, "writing firmware v%d.%d.%d on %d/%d/%d (y/m/d)\n",
			v0, v1, v2,
			hdr->fwdate[0], hdr->fwdate[1], hdr->fwdate[2]);


	while ((n = fread(buf + off, 1, sizeof(buf)-off, in) + off) > 0) {
		off = 0;
		if (n < sizeof(buf)) {
			if (ferror(in)) {
			FREAD_ERROR:
				fprintf(stderr, "fread error\n");
				return EXIT_FAILURE;
			}
			if (gflag) {
				gflag = sizeof(buf) - n;
				memset(buf + n, 0xff, gflag);
				fprintf(stderr, "adding %d bytes of garbage\n", gflag);
				n = sizeof(buf);
			}
		}
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
