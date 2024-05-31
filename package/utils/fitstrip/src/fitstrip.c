// SPDX-License-Identifier: GPL-2.0-only

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <endian.h>
#include <libfdt.h>

/* Max fdt size allowed for processing */
#define MAX_FDT_LEN			(64 << 10)

/* Min buffer size for streaming */
#define MIN_BUF_LEN			(4 << 10)

/* FIT images path */
#define FIT_IMAGES_PATH			"/images"

/* FIT image node properties */
#define FIT_DATA_POSITION_PROP		"data-position"
#define FIT_DATA_OFFSET_PROP		"data-offset"
#define FIT_DATA_SIZE_PROP		"data-size"

static FILE *infile, *outfile;
static void *cache;

static void error(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	(void)vfprintf(stderr, fmt, args);
	va_end(args);
}

static int read_chunk(void *buf, size_t size, size_t *retlen)
{
	size_t len, total = 0;

	while (size) {
		len = fread(buf, 1, size, infile);
		if (len < size) {
			*retlen = total;

			if (ferror(infile)) {
				error("Failed to read input data\n");
				return -EIO;
			}

			if (feof(infile))
				return 0;
		}

		buf += len;
		size -= len;
		total += len;
	}

	*retlen = total;
	return 0;
}

static int write_chunk(const void *buf, size_t size)
{
	size_t len;

	while (size) {
		len = fwrite(buf, 1, size, outfile);
		if (len < size) {
			if (ferror(outfile)) {
				error("Failed to write output data\n");
				return -EIO;
			}

			if (feof(outfile)) {
				error("Incomplete write to output data\n");
				return -EOF;
			}
		}

		buf += len;
		size -= len;
	}

	fflush(outfile);

	return 0;
}

static int fit_image_size(const void *fit, size_t *retsize)
{
	const uint32_t *image_offset_be, *image_len_be, *image_pos_be;
	uint32_t image_pos, image_len, image_end = 0;
	int ret, node, images;

	/* 0 means not FIT image */
	*retsize = 0;

	/* get images path in FIT image */
	images = fdt_path_offset(fit, FIT_IMAGES_PATH);
	if (images < 0)
		return 0;

	/* iterate over image nodes in FIT image */
	fdt_for_each_subnode(node, fit, images) {
		image_offset_be = fdt_getprop(fit, node, FIT_DATA_OFFSET_PROP, NULL);
		image_pos_be = fdt_getprop(fit, node, FIT_DATA_POSITION_PROP, NULL);
		image_len_be = fdt_getprop(fit, node, FIT_DATA_SIZE_PROP, NULL);

		image_len = be32toh(*image_len_be);
		if (!image_len)
			continue;

		if (image_offset_be)
			image_pos = be32toh(*image_offset_be) + fdt_totalsize(fit);
		else if (image_pos_be)
			image_pos = be32toh(*image_pos_be);
		else
			continue;

		if (image_pos + image_len > image_end)
			image_end = image_pos + image_len;
	}

	*retsize = image_end;
	return 0;
}

static int fit_image_stream_process(void)
{
	size_t len, chklen, fdtlen, fitlen, remained = 0;
	struct fdt_header hdr;
	bool unlimited = true;
	int ret;

	/* Read fdt header */
	ret = read_chunk(&hdr, sizeof(hdr), &len);
	if (ret)
		return ret;

	if (len < sizeof(hdr)) {
		/* Not a fdt header */
		return write_chunk(&hdr, len);
	}

	if (fdt_check_header(&hdr)) {
		/* Not a valid fdt header */
		ret = write_chunk(&hdr, len);
		if (ret)
			return ret;

		goto streaming_remained;
	}

	fdtlen = fdt_totalsize(&hdr);
	if (fdtlen > MAX_FDT_LEN) {
		/* Too large fdt for processing */
		ret = write_chunk(&hdr, len);
		if (ret)
			return ret;

		goto streaming_remained;
	}

	/* Make sure cache is large enough for streaming */
	len = fdtlen;
	if (len < MIN_BUF_LEN)
		len = MIN_BUF_LEN;

	cache = malloc(len);
	if (!cache) {
		error("No memory for complete fdt data\n");
		return -ENOMEM;
	}

	/* Store fdt header */
	memcpy(cache, &hdr, sizeof(hdr));

	ret = read_chunk(cache + sizeof(hdr), fdtlen - sizeof(hdr), &len);
	if (ret)
		return ret;

	if (len < fdtlen - sizeof(hdr)) {
		error("Incomplete fdt data\n");
		return write_chunk(cache, fdtlen);
	}

	/* Now calculate the total size of FIT image */
	ret = fit_image_size(cache, &fitlen);
	if (ret)
		return ret;

	if (!fitlen)
		fitlen = fdtlen;

	if (fitlen < fdtlen) {
		/* Invalid FIT image size */
		error("Invalid FIT image size\n");
		return -EINVAL;
	}

	/* Write fdt part */
	ret = write_chunk(cache, fdtlen);
	if (ret)
		return ret;

	remained = fitlen - fdtlen;
	unlimited = false;

	/* Stream remained data */
streaming_remained:
	while (unlimited || remained) {
		if (unlimited || remained >= MIN_BUF_LEN)
			chklen = MIN_BUF_LEN;
		else
			chklen = remained;

		ret = read_chunk(cache, chklen, &len);
		if (ret)
			return ret;

		ret = write_chunk(cache, len);
		if (ret)
			return ret;

		if (!unlimited)
			remained -= len;
	}

	return 0;
}

static int open_file(FILE **f, const char *file, bool read)
{
	int ret;

	if (!strcmp(file, "-")) {
		if (read)
			*f = stdin;
		else
			*f = stdout;

		return 0;
	}

	*f = fopen(file, read ? "rb" : "wb");
	if (!*f) {
		ret = errno;

		error("Unable to open %s file '%s': %s\n",
		      read ? "input" : "output", file, strerror(ret));

		return -ret;
	}

	return 0;
}

static void close_file(FILE *f, bool read)
{
	if (read) {
		if (f != stdin)
			fclose(f);
	} else {
		fflush(f);

		if (f != stdout)
			fclose(f);
	}
}

int main(int argc, char *argv[])
{
	char *end;
	int ret;

	if (argc != 3) {
		error("Remove trailing data from FIT image\n");
		error("Usage: %s <infile> <outfile>\n", argv[0]);
		return -EINVAL;
	}

	ret = open_file(&infile, argv[1], true);
	if (ret)
		return ret;

	ret = open_file(&outfile, argv[2], false);
	if (ret)
		return ret;

	ret = fit_image_stream_process();

	if (cache)
		free(cache);

	close_file(infile, true);
	close_file(outfile, false);

	return ret;
}
