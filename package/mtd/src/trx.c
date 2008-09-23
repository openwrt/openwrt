/*
 * trx.c
 *
 * Copyright (C) 2005 Mike Baker 
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include <sys/ioctl.h>
#include "mtd-api.h"
#include "mtd.h"

#define TRX_MAGIC       0x30524448      /* "HDR0" */
struct trx_header {
	unsigned magic;		/* "HDR0" */
	unsigned len;		/* Length of file including header */
	unsigned crc32;		/* 32-bit CRC from flag_version to end of file */
	unsigned flag_version;	/* 0:15 flags, 16:31 version */
	unsigned offsets[3];	/* Offsets of partitions from start of header */
};

static unsigned long *crc32 = NULL;

static void init_crc32()
{
	unsigned long crc;
	unsigned long poly = 0xEDB88320L;
	int n, bit;

	if (crc32)
		return;

	crc32 = (unsigned long *) malloc(256 * sizeof(unsigned long));
	if (!crc32) {
		perror("malloc");
		exit(1);
	}

	for (n = 0; n < 256; n++) {
		crc = (unsigned long) n;
		for (bit = 0; bit < 8; bit++)
			crc = (crc & 1) ? (poly ^ (crc >> 1)) : (crc >> 1);
		crc32[n] = crc;
	}
}

static unsigned int crc32buf(char *buf, size_t len)
{
	unsigned int crc = 0xFFFFFFFF;
	for (; len; len--, buf++)
		crc = crc32[(crc ^ *buf) & 0xff] ^ (crc >> 8);
	return crc;
}

int
trx_fixup(int fd, const char *name)
{
	struct mtd_info_user mtdInfo;
	unsigned long len;
	struct trx_header *trx;
	void *ptr, *scan;
	int bfd;

	if (ioctl(fd, MEMGETINFO, &mtdInfo) < 0) {
		fprintf(stderr, "Failed to get mtd info\n");
		goto err;
	}

	len = mtdInfo.size;
	if (mtdInfo.size <= 0) {
		fprintf(stderr, "Invalid MTD device size\n");
		goto err;
	}

	bfd = mtd_open(name, true);
	ptr = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, bfd, 0);
	if (!ptr || (ptr == (void *) -1)) {
		perror("mmap");
		goto err1;
	}

	trx = ptr;
	if (trx->magic != TRX_MAGIC) {
		fprintf(stderr, "TRX header not found\n");
		goto err;
	}

	init_crc32();
	scan = ptr + offsetof(struct trx_header, flag_version);
	trx->crc32 = crc32buf(scan, trx->len - (scan - ptr));
	msync(ptr, sizeof(struct trx_header), MS_SYNC|MS_INVALIDATE);
	munmap(ptr, len);
	close(bfd);
	return 0;

err1:
	close(bfd);
err:
	fprintf(stderr, "Error fixing up TRX header\n");
	return -1;
}

int
trx_check(int imagefd, const char *mtd, char *buf, int *len)
{
	const struct trx_header *trx = (const struct trx_header *) buf;
	int fd;

	if (strcmp(mtd, "linux") != 0)
		return 1;

	*len = read(imagefd, buf, 32);
	if (*len < 32) {
		fprintf(stdout, "Could not get image header, file too small (%d bytes)\n", *len);
		return 0;
	}

	if (trx->magic != TRX_MAGIC || trx->len < sizeof(struct trx_header)) {
		if (quiet < 2) {
			fprintf(stderr, "Bad trx header\n");
			fprintf(stderr, "This is not the correct file format; refusing to flash.\n"
					"Please specify the correct file or use -f to force.\n");
		}
		return 0;
	}

	/* check if image fits to mtd device */
	fd = mtd_check_open(mtd);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if(mtdsize < trx->len) {
		fprintf(stderr, "Image too big for partition: %s\n", mtd);
		close(fd);
		return 0;
	}

	close(fd);
	return 1;
}

