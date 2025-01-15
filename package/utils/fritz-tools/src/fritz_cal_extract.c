/*
 * A tool for reading the zlib compressed calibration data
 * found in AVM Fritz!Box based devices).
 *
 * Copyright (c) 2017 Christian Lamparter <chunkeey@googlemail.com>
 *
 * Based on zpipe, which is an example of proper use of zlib's inflate().
 * that is Not copyrighted -- provided to the public domain
 * Version 1.4  11 December 2005  Mark Adler
 *
 * Modifications to also handle calibration data in reversed byte order
 * (c) 2024 by <dzsoftware@posteo.org>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <endian.h>
#include <errno.h>
#include "zlib.h"

#define CHUNK 1024
#define DEFAULT_BUFFERSIZE (129 * 1024)

#define MIN(a,b) (((a)<(b))?(a):(b))

/* Reverse byte order in data buffer.
 * 'top' is position of last valid data byte = (datasize - 1)
 */
static void buffer_reverse(unsigned char *data, unsigned int top)
{
	register unsigned char swapbyte;
	const unsigned int center = top / 2;

	for (unsigned int bottom = 0; bottom < center; ++bottom, --top) {
		swapbyte = data[bottom];
		data[bottom] = data[top];
		data[top] = swapbyte;
	}
}

/* Decompress from file source to data buffer until stream ends
 * or *limit bytes have been written to buffer.
 *
 * On call, 'limit' must reference a variable containing the intended
 * number of bytes to retrieve (must be <= allocated buffer size).
 *
 * Return values (success):
 * Z_END_STREAM if complete data was retrieved (*limit == size of complete data),
 * or Z_OK if data was retrieved up to limit (*limit == original value).
 *
 * Return values (failure):
 * Z_MEM_ERROR if memory could not be allocated for processing, 
 * Z_DATA_ERROR if the deflate data is invalid or incomplete, 
 * Z_VERSION_ERROR if the version of zlib.h and the version of the 
 * library linked do not match, or 
 * Z_ERRNO if there is an error reading or writing the files.
 */
static int inflate_to_buffer(FILE *source, unsigned char *buf, size_t *limit)
{
	int ret;
	z_stream strm;
	unsigned char in[CHUNK];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	/* set data buffer as stream output */
	strm.avail_out = *limit;
	strm.next_out = buf;

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate(), fill data buffer with all available output */
		ret = inflate(&strm, Z_FINISH);
		assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

		switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
		}
		/* done when inflate() says it's done or limit reached */
	} while (ret != Z_STREAM_END && strm.avail_out > 0);

	/* set limit to end of retrieved data */
	assert(strm.total_out <= *limit);
	*limit = strm.total_out;

	/* clean up and return */
	(void)inflateEnd(&strm);
	return (ret == Z_STREAM_END ? Z_STREAM_END : (strm.avail_out == 0 ? Z_OK : Z_DATA_ERROR));
}

/* report a zlib or i/o error */
static void zerr(int ret)
{
	switch (ret) {
	case Z_ERRNO:
		if (ferror(stdin))
			fputs("error reading stdin\n", stderr);
		if (ferror(stdout))
			fputs("error writing stdout\n", stderr);
		break;
	case Z_STREAM_ERROR:
		fputs("invalid compression level\n", stderr);
		break;
	case Z_DATA_ERROR:
		fputs("invalid or incomplete deflate data\n", stderr);
		break;
	case Z_MEM_ERROR:
		fputs("out of memory\n", stderr);
		break;
	case Z_VERSION_ERROR:
		fputs("zlib version mismatch!\n", stderr);
	}
}

static unsigned int get_num(char *str)
{
	if (!strncmp("0x", str, 2))
		return strtoul(str+2, NULL, 16);
	else
		return strtoul(str, NULL, 10);
}

static void usage(void)
{
	fprintf(stderr, "Usage: fritz_cal_extract -e entry_id [-s seek offset] [-l limit]\n"
			"\t[-r reverse extracted data] [-i skip n bytes] [-o output file] [infile]\n"
			"Finds and extracts zlib compressed calibration data in the EVA loader\n");
	exit(EXIT_FAILURE);
}

struct cal_entry {
	uint16_t id;
	uint16_t len;
} __attribute__((packed));

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv)
{
	struct cal_entry cal = { .len = 0 };
	unsigned char *buf = NULL;
	FILE *in = stdin;
	FILE *out = stdout;
	size_t datasize = DEFAULT_BUFFERSIZE;
	size_t limit = 0, skip = 0;
	int initial_offset = 0;
	int entry = -1;
	bool reversed = false, limit_was_set = true;
	int ret;
	int opt;

	while ((opt = getopt(argc, argv, "s:e:o:l:i:r")) != -1) {
		switch (opt) {
		case 's':
			initial_offset = (int)get_num(optarg);
			if (errno) {
				perror("Failed to parse seek offset");
				goto out_bad;
			}
			break;
		case 'e':
			entry = (int) htobe16(get_num(optarg));
			if (errno) {
				perror("Failed to entry id");
				goto out_bad;
			}
			break;
		case 'o':
			out = fopen(optarg, "w");
			if (!out) {
				perror("Failed to create output file");
				goto out_bad;
			}
			break;
		case 'l':
			limit = (size_t)get_num(optarg);
			if (errno) {
				perror("Failed to parse limit");
				goto out_bad;
			}
			break;
		case 'i':
			skip = (size_t)get_num(optarg);
			if (errno) {
				perror("Failed to parse skip");
				goto out_bad;
			}
			break;
		case 'r':
			reversed = true;
			break;
		default: /* '?' */
			usage();
		}
	}

	if (entry == -1)
		usage();

	if (argc > 1 && optind <= argc) {
		in = fopen(argv[optind], "r");
		if (!in) {
			perror("Failed to open input file");
			goto out_bad;
		}
	}

	if (initial_offset) {
		ret = fseek(in, initial_offset, SEEK_CUR);
		if (ret) {
			perror("Failed to seek to calibration table");
			goto out_bad;
		}
	}

	do {
		ret = fseek(in, be16toh(cal.len), SEEK_CUR);
		if (feof(in)) {
			fprintf(stderr, "Reached end of file, but didn't find the matching entry\n");
			goto out_bad;
		} else if (ferror(in)) {
			perror("Failure during seek");
			goto out_bad;
		}

		ret = fread(&cal, 1, sizeof cal, in);
		if (ret != sizeof cal)
			goto out_bad;
	} while (entry != cal.id || cal.id == 0xffff);

	if (cal.id == 0xffff) {
		fprintf(stderr, "Reached end of filesystem, but didn't find the matching entry\n");
		goto out_bad;
	}

	/* Set boundaries. Only keep default datasize if we need complete data
	 * for reversal and didn't set a higher limit. */
	if (!limit) {
		limit_was_set = false;
		limit = datasize - skip;
	}
	datasize = (reversed && datasize >= limit + skip) ? datasize : (limit + skip);

	/* Create data buffer. */
	buf = malloc(datasize);
	assert(buf != NULL);

	ret = inflate_to_buffer(in, buf, &datasize);

	if ((reversed || !limit_was_set) && ret != Z_STREAM_END) { /* didn't read to stream end */
		fprintf(stderr, "Failed: Data exceeds buffer size of %u. Refusing to reverse"
				" or store incomplete data."
				" Use a higher limit [-l] to increase buffer size.\n",
				(unsigned int) datasize);
		goto out_bad;
	}

	ret = (ret == Z_STREAM_END) ? Z_OK : ret; /* normalize return value */
	if (ret != Z_OK) {
		zerr(ret);
		goto out_bad;
	}

	if (reversed)
		buffer_reverse(buf, datasize - 1);

	if (datasize <= skip) {
		fprintf(stderr, "Failed to skip %u bytes, total data size is %u!\n", 
				(unsigned int)skip, (unsigned int)datasize);
		goto out_bad;
	}

	limit = MIN(limit, datasize - skip);
	if (fwrite(&buf[skip], limit, 1, out) != 1 || ferror(out)) {
		fprintf(stderr, "Failed to write data buffer to output file");
		goto out_bad;
	}

	goto out;

out_bad:
	ret = EXIT_FAILURE;

out:
	if (in)
		fclose(in);
	if (out)
		fclose(out);
	free(buf);
	return ret;
}
