/*
 *  RLE decoding routine
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
 
#include "rle.h"

int rle_decode(const unsigned char *src, size_t srclen,
	       unsigned char *dst, size_t dstlen,
	       size_t *src_done, size_t *dst_done)
{
	size_t srcpos, dstpos;
	int ret;

	srcpos = 0;
	dstpos = 0;
	ret = 1;

	/* sanity checks */
	if (!src || !srclen || !dst || !dstlen)
		goto out;

	while (1) {
		char count;

		if (srcpos >= srclen)
			break;

		count = (char) src[srcpos++];
		if (count == 0) {
			ret = 0;
			break;
		}

		if (count > 0) {
			unsigned char c;

			if (srcpos >= srclen)
				break;

			c = src[srcpos++];

			while (count--) {
				if (dstpos >= dstlen)
					break;

				dst[dstpos++] = c;
			}
		} else {
			count *= -1;

			while (count--) {
				if (srcpos >= srclen)
					break;
				if (dstpos >= dstlen)
					break;
				dst[dstpos++] = src[srcpos++];
			}
		}
	}

out:
	if (src_done)
		*src_done = srcpos;
	if (dst_done)
		*dst_done = dstpos;

	return ret;
}
