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
#include <byteswap.h>

typedef enum {
	MODEL_3390,
	MODEL_X490
} t_model;

char *infile = NULL;
char *outfile = NULL;
char *progname;
t_model model;

#define CHUNK_SIZE 256

uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

void crc32(const void *data, size_t n_bytes, uint32_t* crc) {
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    *crc = table[(uint8_t)*crc ^ ((uint8_t*)data)[i]] ^ *crc >> 8;
}

static void usage(int status)
{
	fprintf(stderr, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stderr,
"\n"
"Options:\n"
"  -i              input file name\n"
"  -o              output file name\n"
"  -m              model (3390, x490 for 3490/5490/7490)\n"
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

		c = getopt(argc, argv, "i:o:m:h");
		if (c == -1)
			break;

		switch (c) {
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'm':
			if(strcmp(optarg, "3390") == 0) {
				model = MODEL_3390;
			} else if(strcmp(optarg, "x490") == 0) {
				model = MODEL_X490;
			} else {
				usage(EXIT_FAILURE);
			}
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
		switch(model) {
		case MODEL_3390:
			read = fread(buf, sizeof(uint32_t), CHUNK_SIZE, in_fp);
			for(int i=0; i<read; i++) {
				crc = crc ^ buf[i];
			}
			fwrite(buf, sizeof(uint32_t), read, out_fp);
			break;
		case MODEL_X490:
			read = fread(buf, 1, sizeof(uint32_t) * CHUNK_SIZE, in_fp);
			crc32(buf, read, &crc);
			fwrite(buf, 1, read, out_fp);
			break;
		}
	}
	if(model == MODEL_X490)
		crc = __bswap_32(crc);
	fwrite(&crc, sizeof(uint32_t), 1, out_fp);
	fclose(in_fp);
	fclose(out_fp);
	printf("Done.\n");
	return EXIT_SUCCESS;
}
