/*
 *  RouterBoot helper routines
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/routerboot.h>

#include "routerboot.h"

static u32 get_u32(void *buf)
{
	u8 *p = buf;

	return ((u32) p[3] + ((u32) p[2] << 8) + ((u32) p[1] << 16) +
	       ((u32) p[0] << 24));
}

static u16 get_u16(void *buf)
{
	u8 *p = buf;

	return (u16) p[1] + ((u16) p[0] << 8);
}

__init int
routerboot_find_tag(u8 *buf, unsigned int buflen, u16 tag_id,
		    u8 **tag_data, u16 *tag_len)
{
	uint32_t magic;
	int ret;

	if (buflen < 4)
		return -EINVAL;

	magic = get_u32(buf);
	switch (magic) {
	case RB_MAGIC_HARD:
		/* skip magic value */
		buf += 4;
		buflen -= 4;
		break;

	case RB_MAGIC_SOFT:
		if (buflen < 8)
			return -EINVAL;

		/* skip magic and CRC value */
		buf += 8;
		buflen -= 8;

		break;

	default:
		return -EINVAL;
	}

	ret = -ENOENT;
	while (buflen > 2) {
		u16 id;
		u16 len;

		len = get_u16(buf);
		buf += 2;
		buflen -= 2;

		if (buflen < 2)
			break;

		id = get_u16(buf);
		buf += 2;
		buflen -= 2;

		if (id == RB_ID_TERMINATOR)
			break;

		if (buflen < len)
			break;

		if (id == tag_id) {
			if (tag_len)
				*tag_len = len;
			if (tag_data)
				*tag_data = buf;
			ret = 0;
			break;
		}

		buf += len;
		buflen -= len;
	}

	return ret;
}
