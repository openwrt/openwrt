/*
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2016 Tal Keren <kooolk@gmail.com>
 *
 * Stripped down version of the regular tplink firmware that is only used
 * for compressing and booting the kernel.
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

#include <arpa/inet.h>
#include <netinet/in.h>

#define ALIGN(x,a) ({ typeof(a) __a = (a); (((x) + __a - 1) & ~(__a - 1)); })

#define HEADER_VERSION_V1	0x01000000

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
	uint16_t	ver_hi;
	uint16_t	ver_mid;
	uint16_t	ver_lo;
	uint8_t		pad[354];
} __attribute__ ((packed));


/*
 * Globals
 */
static char *ofname;
static char *progname;
static char *vendor = "TP-LINK Technologies";
static char *version = "ver. 1.0";
static char *fw_ver = "0.0.0";
static uint32_t hdr_ver = HEADER_VERSION_V1;

static char *opt_hw_id;
static uint32_t hw_id = 0;
static struct file_info kernel_info;
static uint32_t kernel_la = 0;
static uint32_t kernel_ep = 0;
static uint32_t kernel_len = 0;

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
	fprintf(stderr, "[%s] *** error: " fmt ": %s\n", \
			progname, ## __VA_ARGS__, strerror(save)); \
} while (0)

#define DBG(fmt, ...) do { \
	fprintf(stderr, "[%s] " fmt "\n", progname, ## __VA_ARGS__ ); \
} while (0)

static void usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;
	struct board_info *board;

	fprintf(stream, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -E <ep>         kernel entry point with <ep> (hexval prefixed with 0x)\n"
"  -L <la>         kernel load address with <la> (hexval prefixed with 0x)\n"
"  -H <hwid>       use hardware id specified with <hwid>\n"
"  -k <file>       read kernel image from the file <file>\n"
"  -o <file>       write output to the file <file>\n"
"  -N <vendor>     set image vendor to <vendor>\n"
"  -V <version>    set image version to <version>\n"
"  -h              show this screen\n"
	);

	exit(status);
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

	if (opt_hw_id) {
		hw_id = strtoul(opt_hw_id, NULL, 0);
	}

	if (!kernel_la || !kernel_ep) {
		ERR("kernel loading address and entry point must be specified");
		return -1;
	}

	if (kernel_info.file_name == NULL) {
		ERR("no kernel image specified");
		return -1;
	}

	ret = get_file_stat(&kernel_info);
	if (ret)
		return ret;

	kernel_len = kernel_info.file_size;

	if (ofname == NULL) {
		ERR("no output file specified");
		return -1;
	}

	return 0;
}

static void fill_header(char *buf)
{
	struct fw_header *hdr = (struct fw_header *)buf;

	memset(hdr, 0, sizeof(struct fw_header));

	hdr->version = htonl(hdr_ver);
	strncpy(hdr->vendor_name, vendor, sizeof(hdr->vendor_name));
	strncpy(hdr->fw_version, version, sizeof(hdr->fw_version));

	/**
	 * This field is ignored and not specified in stock firmware
	 * It is specified here to ensure sysupgrade hardware compatibility
	 * The hardware id of a device is in the product-info partition
	 */
	hdr->hw_id = htonl(hw_id);

	hdr->kernel_la = htonl(kernel_la);
	hdr->kernel_ep = htonl(kernel_ep);

	hdr->kernel_ofs = htonl(sizeof(struct fw_header));
	hdr->kernel_len = htonl(kernel_len);
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

	buflen = sizeof(struct fw_header) + kernel_len;
	buflen = ALIGN(buflen, 0x4);

	buf = malloc(buflen);
	if (!buf) {
		ERR("no memory for buffer\n");
		goto out;
	}

	memset(buf, 0, buflen);
	p = buf + sizeof(struct fw_header);
	ret = read_to_buf(&kernel_info, p);
	if (ret)
		goto out_free_buf;

	fill_header(buf);
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

		c = getopt(argc, argv, "H:E:L:V:N:k:o:h");
		if (c == -1)
			break;

		switch (c) {
		case 'H':
			opt_hw_id = optarg;
			break;
		case 'E':
			sscanf(optarg, "0x%x", &kernel_ep);
			break;
		case 'L':
			sscanf(optarg, "0x%x", &kernel_la);
			break;
		case 'V':
			version = optarg;
			break;
		case 'N':
			vendor = optarg;
			break;
		case 'k':
			kernel_info.file_name = optarg;
			break;
		case 'o':
			ofname = optarg;
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

