/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef C58HP_VALIDATE_H
#define C58HP_VALIDATE_H

#include <stddef.h>
#include <stdint.h>

#define C58HP_IMAGE_MAGIC 0x4f4b4c49U
#define C58HP_KERNEL_LOAD 0x80060000U
#define C58HP_LOADER_START 0x81800000U
#define C58HP_IMAGE_CAPACITY 0x006c0000U
#define C58HP_WORKSPACE_SIZE 0x10000U

static uint32_t c58hp_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | p[3];
}

static uint32_t c58hp_le32(const unsigned char *p)
{
	return ((uint32_t)p[3] << 24) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[1] << 8) | p[0];
}

/* The header checksum treats its own four-byte field as zero. */
static uint32_t c58hp_crc32(const unsigned char *p, size_t size, int header)
{
	uint32_t crc = ~0U;
	size_t i;
	unsigned int bit;

	for (i = 0; i < size; i++) {
		crc ^= (header && i >= 4 && i < 8) ? 0 : p[i];
		for (bit = 0; bit < 8; bit++)
			crc = (crc >> 1) ^ (0xedb88320U & (0U - (crc & 1)));
	}
	return ~crc;
}

/* Validate before any length, destination or decoder property is trusted. */
static const char *c58hp_validate(const unsigned char *image, size_t available)
{
	const unsigned char *data;
	uint32_t size, output, prop, lc, lp, probabilities;

	if (available < 64)
		return "short image header";
	if (c58hp_be32(image) != C58HP_IMAGE_MAGIC)
		return "incorrect image magic";
	if (c58hp_crc32(image, 64, 1) != c58hp_be32(image + 4))
		return "header CRC mismatch";
	size = c58hp_be32(image + 12);
	if (size < 13 || size > C58HP_IMAGE_CAPACITY - 64 ||
	    size > available - 64)
		return "compressed kernel outside image bounds";
	if (c58hp_be32(image + 16) != C58HP_KERNEL_LOAD ||
	    c58hp_be32(image + 20) != C58HP_KERNEL_LOAD)
		return "unexpected kernel load or entry address";
	/* Linux, MIPS, kernel, LZMA. Multi-image parsing stays in stock U-Boot. */
	if (image[28] != 5 || image[29] != 5 || image[30] != 2 || image[31] != 3)
		return "unsupported image format";
	data = image + 64;
	if (c58hp_crc32(data, size, 0) != c58hp_be32(image + 24))
		return "kernel data CRC mismatch";
	output = c58hp_le32(data + 5);
	if (c58hp_le32(data + 9) != 0 || output == 0 ||
	    output > C58HP_LOADER_START - C58HP_KERNEL_LOAD)
		return "decompressed kernel overlaps loader or has unknown size";
	prop = data[0];
	if (prop >= 9 * 5 * 5)
		return "invalid LZMA properties";
	lc = prop % 9;
	lp = (prop / 9) % 5;
	/* Decoder uses 32-bit probabilities and the output as its dictionary. */
	probabilities = 1846U + (768U << (lc + lp));
	if (probabilities > C58HP_WORKSPACE_SIZE / sizeof(uint32_t))
		return "LZMA probability workspace exceeded";
	return NULL;
}

#endif
