/*
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This tool was based on:
 *   TP-Link WR941 V2 firmware checksum fixing tool.
 *   Copyright (C) 2008,2009 Wang Jian <lark@linux.net.cn>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
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

#include "md5.h"

#if (__BYTE_ORDER == __BIG_ENDIAN)
#  define HOST_TO_BE32(x)	(x)
#  define BE32_TO_HOST(x)	(x)
#else
#  define HOST_TO_BE32(x)	bswap_32(x)
#  define BE32_TO_HOST(x)	bswap_32(x)
#endif

#define HEADER_VERSION_V1	0x01000000
#define HWID_TL_WR741ND_V1	0x07410001
#define HWID_TL_WR841N_V1_5	0x08410002
#define HWID_TL_WR841ND_V3	0x08410003
#define HWID_TL_WR841ND_V5	0x08410005
#define HWID_TL_WR941ND_V2	0x09410002
#define HWID_TL_WR1043ND_V1	0x10430001

#define MD5SUM_LEN	16

struct file_info {
	char		*file_name;	/* name of the file */
	uint32_t	file_size;	/* length of the file */
};

struct fw_header {
	uint32_t	version;	/* header version */
	char		vendor_name[24];
	char		fw_version[36];
	uint32_t	hw_id;		/* hardware id */
	uint32_t	hw_rev;		/* hardware revision */
	uint32_t	unk1;
	uint8_t		md5sum1[MD5SUM_LEN];
	uint32_t	unk2;
	uint8_t		md5sum2[MD5SUM_LEN];
	uint32_t	unk3;
	uint32_t	kernel_la;	/* kernel load address */
	uint32_t	kernel_ep;	/* kernel entry point */
	uint32_t	fw_length;	/* total length of the firmware */
	uint32_t	kernel_ofs;	/* kernel data offset */
	uint32_t	kernel_len;	/* kernel data length */
	uint32_t	rootfs_ofs;	/* rootfs data offset */
	uint32_t	rootfs_len;	/* rootfs data length */
	uint32_t	boot_ofs;	/* bootloader data offset */
	uint32_t	boot_len;	/* bootloader data length */
	uint8_t		pad[360];
} __attribute__ ((packed));

struct board_info {
	char		*id;
	uint32_t	hw_id;
	uint32_t	hw_rev;
	uint32_t	fw_max_len;
	uint32_t	kernel_la;
	uint32_t	kernel_ep;
	uint32_t	rootfs_ofs;
};

/*
 * Globals
 */
static char *ofname;
static char *progname;
static char *vendor = "TP-LINK Technologies";
static char *version = "ver. 1.0";

static char *board_id;
static struct board_info *board;
static struct file_info kernel_info;
static struct file_info rootfs_info;
static struct file_info boot_info;
static int combined;

char md5salt_normal[MD5SUM_LEN] = {
	0xdc, 0xd7, 0x3a, 0xa5, 0xc3, 0x95, 0x98, 0xfb,
	0xdd, 0xf9, 0xe7, 0xf4, 0x0e, 0xae, 0x47, 0x38,
};

char md5salt_boot[MD5SUM_LEN] = {
	0x8c, 0xef, 0x33, 0x5b, 0xd5, 0xc5, 0xce, 0xfa,
	0xa7, 0x9c, 0x28, 0xda, 0xb2, 0xe9, 0x0f, 0x42,
};

static struct board_info boards[] = {
	{
		.id		= "TL-WR741NDv1",
		.hw_id		= HWID_TL_WR741ND_V1,
		.hw_rev		= 1,
		.fw_max_len	= 0x3c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		.id		= "TL-WR841Nv1.5",
		.hw_id		= HWID_TL_WR841N_V1_5,
		.hw_rev		= 2,
		.fw_max_len	= 0x3c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		.id		= "TL-WR841NDv3",
		.hw_id		= HWID_TL_WR841ND_V3,
		.hw_rev		= 3,
		.fw_max_len	= 0x3c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		.id		= "TL-WR841NDv5",
		.hw_id		= HWID_TL_WR841ND_V5,
		.hw_rev		= 1,
		.fw_max_len	= 0x3c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		.id		= "TL-WR941NDv2",
		.hw_id		= HWID_TL_WR941ND_V2,
		.hw_rev		= 2,
		.fw_max_len	= 0x3c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		.id		= "TL-WR1043NDv1",
		.hw_id		= HWID_TL_WR1043ND_V1,
		.hw_rev		= 1,
		.fw_max_len	= 0x7c0000,
		.kernel_la	= 0x80060000,
		.kernel_ep	= 0x80060000,
		.rootfs_ofs	= 0x140000,
	}, {
		/* terminating entry */
	}
};

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

#define DBG(fmt, ...) do { \
	fprintf(stderr, "[%s] " fmt "\n", progname, ## __VA_ARGS__ ); \
} while (0)

static struct board_info *find_board(char *id)
{
	struct board_info *ret;
	struct board_info *board;

	ret = NULL;
	for (board = boards; board->id != NULL; board++){
		if (strcasecmp(id, board->id) == 0) {
			ret = board;
			break;
		}
	};

	return ret;
}

static void usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;
	struct board_info *board;

	fprintf(stream, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -B <board>      create image for the board specified with <board>\n"
"  -c              use combined kernel image\n"
"  -k <file>       read kernel image from the file <file>\n"
"  -r <file>       read rootfs image from the file <file>\n"
"  -o <file>       write output to the file <file>\n"
"  -v <version>    set image version to <version>\n"
"  -h              show this screen\n"
	);

	exit(status);
}

static int get_md5(char *data, int size, char *md5)
{
	MD5_CTX ctx;

	MD5_Init(&ctx);
	MD5_Update(&ctx, data, size);
	MD5_Final(md5, &ctx);
}

static int get_file_stat(struct file_info *fdata)
{
	struct stat st;
	int res;

	if (fdata->file_name == NULL)
		return 0;

	res = stat(fdata->file_name, &st);
	if (res){
		ERRS("stat failed on %s", fdata->file_name);
		return res;
	}

	fdata->file_size = st.st_size;
	return 0;
}

static int read_to_buf(struct file_info *fdata, char *buf)
{
	FILE *f;
	int ret = EXIT_FAILURE;

	f = fopen(fdata->file_name, "r");
	if (f == NULL) {
		ERRS("could not open \"%s\" for reading", fdata->file_name);
		goto out;
	}

	errno = 0;
	fread(buf, fdata->file_size, 1, f);
	if (errno != 0) {
		ERRS("unable to read from file \"%s\"", fdata->file_name);
		goto out_close;
	}

	ret = EXIT_SUCCESS;

 out_close:
	fclose(f);
 out:
	return ret;
}

static int check_options(void)
{
	int ret;

	if (board_id == NULL) {
		ERR("no board specified");
		return -1;
	}

	board = find_board(board_id);
	if (board == NULL) {
		ERR("unknown/unsupported board id \"%s\"", board_id);
		return -1;
	}

	if (kernel_info.file_name == NULL) {
		ERR("no kernel image specified");
		return -1;
	}

	ret = get_file_stat(&kernel_info);
	if (ret)
		return ret;

	if (combined) {
		if (kernel_info.file_size >
		    board->fw_max_len - sizeof(struct fw_header)) {
			ERR("kernel image is too big");
			return -1;
		}
	} else {
		if (kernel_info.file_size >
		    board->rootfs_ofs - sizeof(struct fw_header)) {
			ERR("kernel image is too big");
			return -1;
		}
		if (rootfs_info.file_name == NULL) {
			ERR("no rootfs image specified");
			return -1;
		}

		ret = get_file_stat(&rootfs_info);
		if (ret)
			return ret;

		if (rootfs_info.file_size >
                    (board->fw_max_len - board->rootfs_ofs)) {
			ERR("rootfs image is too big");
			return -1;
		}
	}

	if (ofname == NULL) {
		ERR("no output file specified");
		return -1;
	}

	return 0;
}

static void fill_header(char *buf, int len)
{
	struct fw_header *hdr = (struct fw_header *)buf;

	memset(hdr, 0, sizeof(struct fw_header));

	hdr->version = HOST_TO_BE32(HEADER_VERSION_V1);
	strncpy(hdr->vendor_name, vendor, sizeof(hdr->vendor_name));
	strncpy(hdr->fw_version, version, sizeof(hdr->fw_version));
	hdr->hw_id = HOST_TO_BE32(board->hw_id);
	hdr->hw_rev = HOST_TO_BE32(board->hw_rev);

	if (boot_info.file_size == 0)
		memcpy(hdr->md5sum1, md5salt_normal, sizeof(hdr->md5sum1));
	else
		memcpy(hdr->md5sum1, md5salt_boot, sizeof(hdr->md5sum1));

	hdr->kernel_la = HOST_TO_BE32(board->kernel_la);
	hdr->kernel_ep = HOST_TO_BE32(board->kernel_ep);
	hdr->fw_length = HOST_TO_BE32(board->fw_max_len);
	hdr->kernel_ofs = HOST_TO_BE32(sizeof(struct fw_header));
	hdr->kernel_len = HOST_TO_BE32(kernel_info.file_size);
	if (!combined) {
		hdr->rootfs_ofs = HOST_TO_BE32(board->rootfs_ofs);
		hdr->rootfs_len = HOST_TO_BE32(rootfs_info.file_size);
	}

	get_md5(buf, len, hdr->md5sum1);
}

static int write_fw(char *data, int len)
{
	FILE *f;
	int ret = EXIT_FAILURE;

	f = fopen(ofname, "w");
	if (f == NULL) {
		ERRS("could not open \"%s\" for writing", ofname);
		goto out;
	}

	errno = 0;
	fwrite(data, len, 1, f);
	if (errno) {
		ERRS("unable to write output file");
		goto out_flush;
	}

	DBG("firmware file \"%s\" completed", ofname);

	ret = EXIT_SUCCESS;

 out_flush:
	fflush(f);
	fclose(f);
	if (ret != EXIT_SUCCESS) {
		unlink(ofname);
	}
 out:
	return ret;
}

static int build_fw(void)
{
	int buflen;
	char *buf;
	char *p;
	int ret = EXIT_FAILURE;

	buflen = board->fw_max_len;

	buf = malloc(buflen);
	if (!buf) {
		ERR("no memory for buffer\n");
		goto out;
	}

	memset(buf, 0xff, buflen);
	p = buf + sizeof(struct fw_header);
	ret = read_to_buf(&kernel_info, p);
	if (ret)
		goto out_free_buf;

	if (!combined) {
		p = buf + board->rootfs_ofs;
		ret = read_to_buf(&rootfs_info, p);
		if (ret)
			goto out_free_buf;
	}

	fill_header(buf, buflen);

	ret = write_fw(buf, buflen);
	if (ret)
		goto out_free_buf;

	ret = EXIT_SUCCESS;

 out_free_buf:
	free(buf);
 out:
	return ret;
}

int main(int argc, char *argv[])
{
	int ret = EXIT_FAILURE;
	int err;

	FILE *outfile;

	progname = basename(argv[0]);

	while ( 1 ) {
		int c;

		c = getopt(argc, argv, "B:V:N:ck:r:o:v:h:");
		if (c == -1)
			break;

		switch (c) {
		case 'B':
			board_id = optarg;
			break;
		case 'V':
			version = optarg;
			break;
		case 'N':
			vendor = optarg;
			break;
		case 'c':
			combined++;
			break;
		case 'k':
			kernel_info.file_name = optarg;
			break;
		case 'r':
			rootfs_info.file_name = optarg;
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

	ret = check_options();
	if (ret)
		goto out;

	ret = build_fw();

 out:
	return ret;
}

