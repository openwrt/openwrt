// SPDX-License-Identifier: GPL-2.0-only
/*
 * Minimal string functions for lzma-loader
 *
 * Provides memcpy, memcmp, and memset for freestanding environment.
 *
 * Copyright (C) 2025 Ahmed Naseef <naseefkm@gmail.com>
 */

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	uint8_t *d = dest;
	const uint8_t *s = src;

	while (n--)
		*d++ = *s++;

	return dest;
}

void *memset(void *s, int c, size_t n)
{
	uint8_t *p = s;

	while (n--)
		*p++ = (uint8_t)c;

	return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	const uint8_t *p1 = s1;
	const uint8_t *p2 = s2;

	while (n--) {
		if (*p1 != *p2)
			return *p1 - *p2;
		p1++;
		p2++;
	}

	return 0;
}
