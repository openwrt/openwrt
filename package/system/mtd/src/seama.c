/*
 * seama.c
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Based on the trx fixup code:
 *   Copyright (C) 2005 Mike Baker
 *   Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
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
#include <errno.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include "mtd.h"
#include "seama.h"
#include "md5.h"

#if __BYTE_ORDER == __BIG_ENDIAN
#define STORE32_LE(X)           ((((X) & 0x000000FF) << 24) | (((X) & 0x0000FF00) << 8) | (((X) & 0x00FF0000) >> 8) | (((X) & 0xFF000000) >> 24))
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define STORE32_LE(X)           (X)
#else
#error unknown endianness!
#endif

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

int
seama_fix_md5(char *buf, size_t len)
{
	struct seama_hdr *shdr;
	char *data;
	size_t msize;
	size_t isize;
	MD5_CTX ctx;
	unsigned char digest[16];
	int i;

	if (len < sizeof(struct seama_hdr))
		return -1;

	shdr = (struct seama_hdr *) buf;
	if (shdr->magic != htonl(SEAMA_MAGIC)) {
		fprintf(stderr, "no SEAMA header found\n");
		return -1;
	}

	isize = ntohl(shdr->size);
	msize = ntohs(shdr->metasize);
	if (isize == 0) {
		/* the image contains no checksum */
		return -1;
	}

	len -= sizeof(struct seama_hdr) + sizeof(digest) + msize;
	if (isize > len)
		isize = len;

	data = buf + sizeof(struct seama_hdr) + sizeof(digest) + msize;

	MD5_Init(&ctx);
	MD5_Update(&ctx, data, isize);
	MD5_Final(digest, &ctx);

	if (!memcmp(digest, &buf[sizeof(struct seama_hdr)], sizeof(digest))) {
		if (quiet < 2)
			fprintf(stderr, "the header is fixed already\n");
		return -1;
	}

	if (quiet < 2) {
		fprintf(stderr, "new size:%u, new MD5: ", isize);
		for (i = 0; i < sizeof(digest); i++)
			fprintf(stderr, "%02x", digest[i]);

		fprintf(stderr, "\n");
	}

	/* update the size in the image */
	shdr->size = htonl(isize);

	/* update the checksum in the image */
	for (i = 0; i < sizeof(digest); i++)
		buf[sizeof(struct seama_hdr) + i] = digest[i];

	return 0;
}

int
mtd_fixseama(const char *mtd, size_t offset)
{
	int fd;
	char *buf;
	ssize_t res;
	size_t block_offset;

	if (quiet < 2)
		fprintf(stderr, "Trying to fix SEAMA header in %s at 0x%x...\n",
			mtd, offset);

	block_offset = offset & ~(erasesize - 1);
	offset -= block_offset;

	fd = mtd_check_open(mtd);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if (block_offset + erasesize > mtdsize) {
		fprintf(stderr, "Offset too large, device size 0x%x\n",
			mtdsize);
		exit(1);
	}

	buf = malloc(mtdsize);
	if (!buf) {
		perror("malloc");
		exit(1);
	}

	res = pread(fd, buf, mtdsize, block_offset);
	if (res != mtdsize) {
		perror("pread");
		exit(1);
	}

	if (seama_fix_md5(buf, mtdsize))
		goto out;

	if (mtd_erase_block(fd, block_offset)) {
		fprintf(stderr, "Can't erease block at 0x%x (%s)\n",
			block_offset, strerror(errno));
		exit(1);
	}

	if (quiet < 2)
		fprintf(stderr, "Rewriting block at 0x%x\n", block_offset);

	if (pwrite(fd, buf, erasesize, block_offset) != erasesize) {
		fprintf(stderr, "Error writing block (%s)\n", strerror(errno));
		exit(1);
	}

	if (quiet < 2)
		fprintf(stderr, "Done.\n");

out:
	close (fd);
	sync();

	return 0;
}

