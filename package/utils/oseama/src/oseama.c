/*
 * oseama
 *
 * Copyright (C) 2016 Rafał Miłecki <zajec5@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <byteswap.h>
#include <endian.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if !defined(__BYTE_ORDER)
#error "Unknown byte order"
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_be32(x)	(x)
#define be32_to_cpu(x)	(x)
#define cpu_to_be16(x)	(x)
#define be16_to_cpu(x)	(x)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_be32(x)	bswap_32(x)
#define be32_to_cpu(x)	bswap_32(x)
#define cpu_to_be16(x)	bswap_16(x)
#define be16_to_cpu(x)	bswap_16(x)
#else
#error "Unsupported endianness"
#endif

#define SEAMA_MAGIC			0x5ea3a417

struct seama_seal_header {
	uint32_t magic;
	uint16_t reserved;
	uint16_t metasize;
	uint32_t imagesize;
} __attribute__ ((packed));

struct seama_entity_header {
	uint32_t magic;
	uint16_t reserved;
	uint16_t metasize;
	uint32_t imagesize;
	uint8_t md5[16];
} __attribute__ ((packed));

char *seama_path;
int entity_idx = -1;

static inline size_t oseama_min(size_t x, size_t y) {
	return x < y ? x : y;
}

/**************************************************
 * Info
 **************************************************/

static void oseama_info_parse_options(int argc, char **argv) {
	int c;

	while ((c = getopt(argc, argv, "e:")) != -1) {
		switch (c) {
		case 'e':
			entity_idx = atoi(optarg);
			break;
		}
	}
}

static int oseama_info_entities(FILE *seama) {
	struct seama_entity_header hdr;
	size_t bytes, metasize, imagesize;
	uint8_t buf[1024];
	char *end, *tmp;
	int i = 0;
	int err = 0;

	while ((bytes = fread(&hdr, 1, sizeof(hdr), seama)) == sizeof(hdr)) {
		if (be32_to_cpu(hdr.magic) != SEAMA_MAGIC) {
			fprintf(stderr, "Invalid Seama magic: 0x%08x\n", be32_to_cpu(hdr.magic));
			err =  -EINVAL;
			goto err_out;
		}
		metasize = be16_to_cpu(hdr.metasize);
		imagesize = be32_to_cpu(hdr.imagesize);

		if (entity_idx >= 0 && i != entity_idx) {
			fseek(seama, metasize + imagesize, SEEK_CUR);
			i++;
			continue;
		}

		if (metasize >= sizeof(buf)) {
			fprintf(stderr, "Too small buffer (%zu B) to read all meta info (%zd B)\n", sizeof(buf), metasize);
			err =  -EINVAL;
			goto err_out;
		}

		if (entity_idx < 0)
			printf("\n");
		printf("Entity offset:\t%ld\n", ftell(seama) - sizeof(hdr));
		printf("Entity size:\t%zd\n", sizeof(hdr) + metasize + imagesize);
		printf("Meta size:\t%zd\n", metasize);
		printf("Image size:\t%zd\n", imagesize);

		bytes = fread(buf, 1, metasize, seama);
		if (bytes != metasize) {
			fprintf(stderr, "Couldn't read %zd B of meta\n", metasize);
			err =  -EIO;
			goto err_out;
		}

		end = (char *)&buf[metasize - 1];
		*end = '\0';
		for (tmp = (char *)buf; tmp < end && strlen(tmp); tmp += strlen(tmp) + 1) {
			printf("Meta entry:\t%s\n", tmp);
		}

		fseek(seama, imagesize, SEEK_CUR);
		i++;
	}

err_out:
	return err;
}

static int oseama_info(int argc, char **argv) {
	FILE *seama;
	struct seama_seal_header hdr;
	size_t bytes;
	uint16_t metasize;
	uint32_t imagesize;
	uint8_t buf[1024];
	int err = 0;

	if (argc < 3) {
		fprintf(stderr, "No Seama file passed\n");
		err = -EINVAL;
		goto out;
	}
	seama_path = argv[2];

	optind = 3;
	oseama_info_parse_options(argc, argv);

	seama = fopen(seama_path, "r");
	if (!seama) {
		fprintf(stderr, "Couldn't open %s\n", seama_path);
		err = -EACCES;
		goto out;
	}

	bytes = fread(&hdr, 1, sizeof(hdr), seama);
	if (bytes != sizeof(hdr)) {
		fprintf(stderr, "Couldn't read %s header\n", seama_path);
		err =  -EIO;
		goto err_close;
	}
	metasize = be16_to_cpu(hdr.metasize);
	imagesize = be32_to_cpu(hdr.imagesize);

	if (be32_to_cpu(hdr.magic) != SEAMA_MAGIC) {
		fprintf(stderr, "Invalid Seama magic: 0x%08x\n", be32_to_cpu(hdr.magic));
		err =  -EINVAL;
		goto err_close;
	}

	if (metasize >= sizeof(buf)) {
		fprintf(stderr, "Too small buffer (%zu B) to read all meta info (%d B)\n", sizeof(buf), metasize);
		err =  -EINVAL;
		goto err_close;
	}

	if (imagesize) {
		fprintf(stderr, "Invalid Seama image size: 0x%08x (should be 0)\n", imagesize);
		err =  -EINVAL;
		goto err_close;
	}

	bytes = fread(buf, 1, metasize, seama);
	if (bytes != metasize) {
		fprintf(stderr, "Couldn't read %d B of meta\n", metasize);
		err =  -EIO;
		goto err_close;
	}

	if (entity_idx < 0) {
		char *end, *tmp;

		printf("Meta size:\t%d\n", metasize);
		printf("Image size:\t%d\n", imagesize);

		end = (char *)&buf[metasize - 1];
		*end = '\0';
		for (tmp = (char *)buf; tmp < end && strlen(tmp); tmp += strlen(tmp) + 1) {
			printf("Meta entry:\t%s\n", tmp);
		}
	}

	oseama_info_entities(seama);

err_close:
	fclose(seama);
out:
	return err;
}

/**************************************************
 * Start
 **************************************************/

static void usage() {
	printf("Usage:\n");
	printf("\n");
	printf("Info about Seama seal (container):\n");
	printf("\toseama info <file> [options]\n");
	printf("\t-e\t\t\t\tprint info about specified entity only\n");
}

int main(int argc, char **argv) {
	if (argc > 1) {
		if (!strcmp(argv[1], "info"))
			return oseama_info(argc, argv);
	}

	usage();
	return 0;
}
