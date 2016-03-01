/*
  Copyright (c) 2014, Matthias Schiffer <mschiffer@universe-factory.net>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
   tplink-safeloader

   Image generation tool for the TP-LINK SafeLoader as seen on
   TP-LINK Pharos devices (CPE210/220/510/520)
*/


#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "md5.h"


#define ALIGN(x,a) ({ typeof(a) __a = (a); (((x) + __a - 1) & ~(__a - 1)); })


/** An image partition table entry */
struct image_partition_entry {
	const char *name;
	size_t size;
	uint8_t *data;
};

/** A flash partition table entry */
struct flash_partition_entry {
	const char *name;
	uint32_t base;
	uint32_t size;
};


/** The content of the soft-version structure */
struct __attribute__((__packed__)) soft_version {
	uint32_t magic;
	uint32_t zero;
	uint8_t pad1;
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t version_patch;
	uint8_t year_hi;
	uint8_t year_lo;
	uint8_t month;
	uint8_t day;
	uint32_t rev;
	uint8_t pad2;
};


static const uint8_t jffs2_eof_mark[4] = {0xde, 0xad, 0xc0, 0xde};


/**
   Salt for the MD5 hash

   Fortunately, TP-LINK seems to use the same salt for most devices which use
   the new image format.
*/
static const uint8_t md5_salt[16] = {
	0x7a, 0x2b, 0x15, 0xed,
	0x9b, 0x98, 0x59, 0x6d,
	0xe5, 0x04, 0xab, 0x44,
	0xac, 0x2a, 0x9f, 0x4e,
};


/** Vendor information for CPE210/220/510/520 */
static const char cpe510_vendor[] = "CPE510(TP-LINK|UN|N300-5):1.0\r\n";


/**
    The flash partition table for CPE210/220/510/520;
    it is the same as the one used by the stock images.
*/
static const struct flash_partition_entry cpe510_partitions[] = {
	{"fs-uboot", 0x00000, 0x20000},
	{"partition-table", 0x20000, 0x02000},
	{"default-mac", 0x30000, 0x00020},
	{"product-info", 0x31100, 0x00100},
	{"signature", 0x32000, 0x00400},
	{"os-image", 0x40000, 0x170000},
	{"soft-version", 0x1b0000, 0x00100},
	{"support-list", 0x1b1000, 0x00400},
	{"file-system", 0x1c0000, 0x600000},
	{"user-config", 0x7c0000, 0x10000},
	{"default-config", 0x7d0000, 0x10000},
	{"log", 0x7e0000, 0x10000},
	{"radio", 0x7f0000, 0x10000},
	{NULL, 0, 0}
};

/**
   The support list for CPE210/220/510/520
*/
static const char cpe510_support_list[] =
	"SupportList:\r\n"
	"CPE510(TP-LINK|UN|N300-5):1.0\r\n"
	"CPE510(TP-LINK|UN|N300-5):1.1\r\n"
	"CPE520(TP-LINK|UN|N300-5):1.0\r\n"
	"CPE520(TP-LINK|UN|N300-5):1.1\r\n"
	"CPE210(TP-LINK|UN|N300-2):1.0\r\n"
	"CPE210(TP-LINK|UN|N300-2):1.1\r\n"
	"CPE220(TP-LINK|UN|N300-2):1.0\r\n"
	"CPE220(TP-LINK|UN|N300-2):1.1\r\n";

#define error(_ret, _errno, _str, ...)				\
	do {							\
		fprintf(stderr, _str ": %s\n", ## __VA_ARGS__,	\
			strerror(_errno));			\
		if (_ret)					\
			exit(_ret);				\
	} while (0)


/** Stores a uint32 as big endian */
static inline void put32(uint8_t *buf, uint32_t val) {
	buf[0] = val >> 24;
	buf[1] = val >> 16;
	buf[2] = val >> 8;
	buf[3] = val;
}

/** Allocates a new image partition */
static struct image_partition_entry alloc_image_partition(const char *name, size_t len) {
	struct image_partition_entry entry = {name, len, malloc(len)};
	if (!entry.data)
		error(1, errno, "malloc");

	return entry;
}

/** Frees an image partition */
static void free_image_partition(struct image_partition_entry entry) {
	free(entry.data);
}

/** Generates the partition-table partition */
static struct image_partition_entry make_partition_table(const struct flash_partition_entry *p) {
	struct image_partition_entry entry = alloc_image_partition("partition-table", 0x800);

	char *s = (char *)entry.data, *end = (char *)(s+entry.size);

	*(s++) = 0x00;
	*(s++) = 0x04;
	*(s++) = 0x00;
	*(s++) = 0x00;

	size_t i;
	for (i = 0; p[i].name; i++) {
		size_t len = end-s;
		size_t w = snprintf(s, len, "partition %s base 0x%05x size 0x%05x\n", p[i].name, p[i].base, p[i].size);

		if (w > len-1)
			error(1, 0, "flash partition table overflow?");

		s += w;
	}

	s++;

	memset(s, 0xff, end-s);

	return entry;
}


/** Generates a binary-coded decimal representation of an integer in the range [0, 99] */
static inline uint8_t bcd(uint8_t v) {
	return 0x10 * (v/10) + v%10;
}


/** Generates the soft-version partition */
static struct image_partition_entry make_soft_version(uint32_t rev) {
	struct image_partition_entry entry = alloc_image_partition("soft-version", sizeof(struct soft_version));
	struct soft_version *s = (struct soft_version *)entry.data;

	time_t t;

	if (time(&t) == (time_t)(-1))
		error(1, errno, "time");

	struct tm *tm = localtime(&t);

	s->magic = htonl(0x0000000c);
	s->zero = 0;
	s->pad1 = 0xff;

	s->version_major = 0;
	s->version_minor = 0;
	s->version_patch = 0;

	s->year_hi = bcd((1900+tm->tm_year)/100);
	s->year_lo = bcd(tm->tm_year%100);
	s->month = bcd(tm->tm_mon+1);
	s->day = bcd(tm->tm_mday);
	s->rev = htonl(rev);

	s->pad2 = 0xff;

	return entry;
}

/** Generates the support-list partition */
static struct image_partition_entry make_support_list(const char *support_list) {
	size_t len = strlen(support_list);
	struct image_partition_entry entry = alloc_image_partition("support-list", len + 9);

	put32(entry.data, len);
	memset(entry.data+4, 0, 4);
	memcpy(entry.data+8, support_list, len);
	entry.data[len+8] = '\xff';

	return entry;
}

/** Creates a new image partition with an arbitrary name from a file */
static struct image_partition_entry read_file(const char *part_name, const char *filename, bool add_jffs2_eof) {
	struct stat statbuf;

	if (stat(filename, &statbuf) < 0)
		error(1, errno, "unable to stat file `%s'", filename);

	size_t len = statbuf.st_size;

	if (add_jffs2_eof)
		len = ALIGN(len, 0x10000) + sizeof(jffs2_eof_mark);

	struct image_partition_entry entry = alloc_image_partition(part_name, len);

	FILE *file = fopen(filename, "rb");
	if (!file)
		error(1, errno, "unable to open file `%s'", filename);

	if (fread(entry.data, statbuf.st_size, 1, file) != 1)
		error(1, errno, "unable to read file `%s'", filename);

	if (add_jffs2_eof) {
		uint8_t *eof = entry.data + statbuf.st_size, *end = entry.data+entry.size;

		memset(eof, 0xff, end - eof - sizeof(jffs2_eof_mark));
		memcpy(end - sizeof(jffs2_eof_mark), jffs2_eof_mark, sizeof(jffs2_eof_mark));
	}

	fclose(file);

	return entry;
}


/**
   Copies a list of image partitions into an image buffer and generates the image partition table while doing so

   Example image partition table:

     fwup-ptn partition-table base 0x00800 size 0x00800
     fwup-ptn os-image base 0x01000 size 0x113b45
     fwup-ptn file-system base 0x114b45 size 0x1d0004
     fwup-ptn support-list base 0x2e4b49 size 0x000d1

   Each line of the partition table is terminated with the bytes 09 0d 0a ("\t\r\n"),
   the end of the partition table is marked with a zero byte.

   The firmware image must contain at least the partition-table and support-list partitions
   to be accepted. There aren't any alignment constraints for the image partitions.

   The partition-table partition contains the actual flash layout; partitions
   from the image partition table are mapped to the corresponding flash partitions during
   the firmware upgrade. The support-list partition contains a list of devices supported by
   the firmware image.

   The base offsets in the firmware partition table are relative to the end
   of the vendor information block, so the partition-table partition will
   actually start at offset 0x1814 of the image.

   I think partition-table must be the first partition in the firmware image.
*/
static void put_partitions(uint8_t *buffer, const struct image_partition_entry *parts) {
	size_t i;
	char *image_pt = (char *)buffer, *end = image_pt + 0x800;

	size_t base = 0x800;
	for (i = 0; parts[i].name; i++) {
		memcpy(buffer + base, parts[i].data, parts[i].size);

		size_t len = end-image_pt;
		size_t w = snprintf(image_pt, len, "fwup-ptn %s base 0x%05x size 0x%05x\t\r\n", parts[i].name, (unsigned)base, (unsigned)parts[i].size);

		if (w > len-1)
			error(1, 0, "image partition table overflow?");

		image_pt += w;

		base += parts[i].size;
	}

	image_pt++;

	memset(image_pt, 0xff, end-image_pt);
}

/** Generates and writes the image MD5 checksum */
static void put_md5(uint8_t *md5, uint8_t *buffer, unsigned int len) {
	MD5_CTX ctx;

	MD5_Init(&ctx);
	MD5_Update(&ctx, md5_salt, (unsigned int)sizeof(md5_salt));
	MD5_Update(&ctx, buffer, len);
	MD5_Final(md5, &ctx);
}


/**
   Generates the firmware image in factory format

   Image format:

     Bytes (hex)  Usage
     -----------  -----
     0000-0003    Image size (4 bytes, big endian)
     0004-0013    MD5 hash (hash of a 16 byte salt and the image data starting with byte 0x14)
     0014-0017    Vendor information length (without padding) (4 bytes, big endian)
     0018-1013    Vendor information (4092 bytes, padded with 0xff; there seem to be older
                  (VxWorks-based) TP-LINK devices which use a smaller vendor information block)
     1014-1813    Image partition table (2048 bytes, padded with 0xff)
     1814-xxxx    Firmware partitions
*/
static void * generate_factory_image(const char *vendor, const struct image_partition_entry *parts, size_t *len) {
	*len = 0x1814;

	size_t i;
	for (i = 0; parts[i].name; i++)
		*len += parts[i].size;

	uint8_t *image = malloc(*len);
	if (!image)
		error(1, errno, "malloc");

	put32(image, *len);

	size_t vendor_len = strlen(vendor);
	put32(image+0x14, vendor_len);
	memcpy(image+0x18, vendor, vendor_len);
	memset(image+0x18+vendor_len, 0xff, 4092-vendor_len);

	put_partitions(image + 0x1014, parts);
	put_md5(image+0x04, image+0x14, *len-0x14);

	return image;
}

/**
   Generates the firmware image in sysupgrade format

   This makes some assumptions about the provided flash and image partition tables and
   should be generalized when TP-LINK starts building its safeloader into hardware with
   different flash layouts.
*/
static void * generate_sysupgrade_image(const struct flash_partition_entry *flash_parts, const struct image_partition_entry *image_parts, size_t *len) {
	const struct flash_partition_entry *flash_os_image = &flash_parts[5];
	const struct flash_partition_entry *flash_soft_version = &flash_parts[6];
	const struct flash_partition_entry *flash_support_list = &flash_parts[7];
	const struct flash_partition_entry *flash_file_system = &flash_parts[8];

	const struct image_partition_entry *image_os_image = &image_parts[3];
	const struct image_partition_entry *image_soft_version = &image_parts[1];
	const struct image_partition_entry *image_support_list = &image_parts[2];
	const struct image_partition_entry *image_file_system = &image_parts[4];

	assert(strcmp(flash_os_image->name, "os-image") == 0);
	assert(strcmp(flash_soft_version->name, "soft-version") == 0);
	assert(strcmp(flash_support_list->name, "support-list") == 0);
	assert(strcmp(flash_file_system->name, "file-system") == 0);

	assert(strcmp(image_os_image->name, "os-image") == 0);
	assert(strcmp(image_soft_version->name, "soft-version") == 0);
	assert(strcmp(image_support_list->name, "support-list") == 0);
	assert(strcmp(image_file_system->name, "file-system") == 0);

	if (image_os_image->size > flash_os_image->size)
		error(1, 0, "kernel image too big (more than %u bytes)", (unsigned)flash_os_image->size);
	if (image_file_system->size > flash_file_system->size)
		error(1, 0, "rootfs image too big (more than %u bytes)", (unsigned)flash_file_system->size);

	*len = flash_file_system->base - flash_os_image->base + image_file_system->size;

	uint8_t *image = malloc(*len);
	if (!image)
		error(1, errno, "malloc");

	memset(image, 0xff, *len);

	memcpy(image, image_os_image->data, image_os_image->size);
	memcpy(image + flash_soft_version->base - flash_os_image->base, image_soft_version->data, image_soft_version->size);
	memcpy(image + flash_support_list->base - flash_os_image->base, image_support_list->data, image_support_list->size);
	memcpy(image + flash_file_system->base - flash_os_image->base, image_file_system->data, image_file_system->size);

	return image;
}


/** Generates an image for CPE210/220/510/520 and writes it to a file */
static void do_cpe510(const char *output, const char *kernel_image, const char *rootfs_image, uint32_t rev, bool add_jffs2_eof, bool sysupgrade) {
	struct image_partition_entry parts[6] = {};

	parts[0] = make_partition_table(cpe510_partitions);
	parts[1] = make_soft_version(rev);
	parts[2] = make_support_list(cpe510_support_list);
	parts[3] = read_file("os-image", kernel_image, false);
	parts[4] = read_file("file-system", rootfs_image, add_jffs2_eof);

	size_t len;
	void *image;
	if (sysupgrade)
		image = generate_sysupgrade_image(cpe510_partitions, parts, &len);
	else
		image = generate_factory_image(cpe510_vendor, parts, &len);

	FILE *file = fopen(output, "wb");
	if (!file)
		error(1, errno, "unable to open output file");

	if (fwrite(image, len, 1, file) != 1)
		error(1, 0, "unable to write output file");

	fclose(file);

	free(image);

	size_t i;
	for (i = 0; parts[i].name; i++)
		free_image_partition(parts[i]);
}


/** Usage output */
static void usage(const char *argv0) {
	fprintf(stderr,
		"Usage: %s [OPTIONS...]\n"
		"\n"
		"Options:\n"
		"  -B <board>      create image for the board specified with <board>\n"
		"  -k <file>       read kernel image from the file <file>\n"
		"  -r <file>       read rootfs image from the file <file>\n"
		"  -o <file>       write output to the file <file>\n"
		"  -V <rev>        sets the revision number to <rev>\n"
		"  -j              add jffs2 end-of-filesystem markers\n"
		"  -S              create sysupgrade instead of factory image\n"
		"  -h              show this help\n",
		argv0
	);
};


int main(int argc, char *argv[]) {
	const char *board = NULL, *kernel_image = NULL, *rootfs_image = NULL, *output = NULL;
	bool add_jffs2_eof = false, sysupgrade = false;
	unsigned rev = 0;

	while (true) {
		int c;

		c = getopt(argc, argv, "B:k:r:o:V:jSh");
		if (c == -1)
			break;

		switch (c) {
		case 'B':
			board = optarg;
			break;

		case 'k':
			kernel_image = optarg;
			break;

		case 'r':
			rootfs_image = optarg;
			break;

		case 'o':
			output = optarg;
			break;

		case 'V':
			sscanf(optarg, "r%u", &rev);
			break;

		case 'j':
			add_jffs2_eof = true;
			break;

		case 'S':
			sysupgrade = true;
			break;

		case 'h':
			usage(argv[0]);
			return 0;

		default:
			usage(argv[0]);
			return 1;
		}
	}

	if (!board)
		error(1, 0, "no board has been specified");
	if (!kernel_image)
		error(1, 0, "no kernel image has been specified");
	if (!rootfs_image)
		error(1, 0, "no rootfs image has been specified");
	if (!output)
		error(1, 0, "no output filename has been specified");

	if (strcmp(board, "CPE510") == 0)
		do_cpe510(output, kernel_image, rootfs_image, rev, add_jffs2_eof, sysupgrade);
	else
		error(1, 0, "unsupported board %s", board);

	return 0;
}
