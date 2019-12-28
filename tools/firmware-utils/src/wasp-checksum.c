/*
 * Copyright (C) 2020 Andreas Boehler <dev@aboehler.at>
 *
 * This tool was based on:
 * 	firmware-crc.pl by Atheros Communications
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>     /* for getopt() */

char *infile = NULL;
char *outfile = NULL;
char *progname;

#define CHUNK_SIZE 256

static void usage(int status)
{
	fprintf(stderr, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stderr,
"\n"
"Options:\n"
"  -i              input file name\n"
"  -o              output file name\n"
"  -h              show this screen\n"
	);

	exit(status);
}

int main(int argc, char *argv[]) {
	uint32_t crc = 0;
	FILE *in_fp;
	FILE *out_fp;
	uint32_t buf[CHUNK_SIZE];
	ssize_t read;

	progname = argv[0];

	while ( 1 ) {
		int c;

		c = getopt(argc, argv, "i:o:h");
		if (c == -1)
			break;

		switch (c) {
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if(!infile || !outfile) {
		usage(EXIT_FAILURE);
	}

	in_fp = fopen(infile, "r");
	if(!in_fp) {
		fprintf(stderr, "Error opening input file: %s\n", infile);
		return EXIT_FAILURE;
	}
	out_fp = fopen(outfile, "w");
	if(!out_fp) {
		fprintf(stderr, "Error opening output file: %s\n", outfile);
		fclose(in_fp);
		return EXIT_FAILURE;
	}

	while(!feof(in_fp)) {
		read = fread(buf, sizeof(uint32_t), CHUNK_SIZE, in_fp);
		for(int i=0; i<read; i++) {
			crc = crc ^ buf[i];
		}
		fwrite(buf, sizeof(uint32_t), read, out_fp);
	}
	fwrite(&crc, sizeof(uint32_t), 1, out_fp);
	fclose(in_fp);
	fclose(out_fp);

	return EXIT_SUCCESS;
}
