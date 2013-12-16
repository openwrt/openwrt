/*
 *  RouterBoot helper routines
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define pr_fmt(fmt) "rb: " fmt

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/routerboot.h>
#include <linux/rle.h>

#include "routerboot.h"

#define RB_BLOCK_SIZE		0x1000
#define RB_ART_SIZE		0x10000

static struct rb_info rb_info;

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
routerboot_find_magic(u8 *buf, unsigned int buflen, u32 *offset, bool hard)
{
	u32 magic_ref = hard ? RB_MAGIC_HARD : RB_MAGIC_SOFT;
	u32 magic;
	u32 cur = *offset;

	while (cur < buflen) {
		magic = get_u32(buf + cur);
		if (magic == magic_ref) {
			*offset = cur;
			return 0;
		}

		cur += 0x1000;
	}

	return -ENOENT;
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

static inline int
rb_find_hard_cfg_tag(u16 tag_id, u8 **tag_data, u16 *tag_len)
{
	if (!rb_info.hard_cfg_data ||
	    !rb_info.hard_cfg_size)
		return -ENOENT;

	return routerboot_find_tag(rb_info.hard_cfg_data,
				   rb_info.hard_cfg_size,
				   tag_id, tag_data, tag_len);
}

__init const char *
rb_get_board_name(void)
{
	u16 tag_len;
	u8 *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_BOARD_NAME, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

__init u32
rb_get_hw_options(void)
{
	u16 tag_len;
	u8 *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_HW_OPTIONS, &tag, &tag_len);
	if (err)
		return 0;

	return get_u32(tag);
}

__init void *
rb_get_wlan_data(void)
{
	u16 tag_len;
	u8 *tag;
	void *buf;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_WLAN_DATA, &tag, &tag_len);
	if (err) {
		pr_err("no calibration data found\n");
		goto err;
	}

	buf = kmalloc(RB_ART_SIZE, GFP_KERNEL);
	if (buf == NULL) {
		pr_err("no memory for calibration data\n");
		goto err;
	}

	err = rle_decode((char *) tag, tag_len, buf, RB_ART_SIZE,
			 NULL, NULL);
	if (err) {
		pr_err("unable to decode calibration data\n");
		goto err_free;
	}

	return buf;

err_free:
	kfree(buf);
err:
	return NULL;
}

__init const struct rb_info *
rb_init_info(void *data, unsigned int size)
{
	unsigned int offset;

	if (size == 0 || (size % RB_BLOCK_SIZE) != 0)
		return NULL;

	for (offset = 0; offset < size; offset += RB_BLOCK_SIZE) {
		u32 magic;

		magic = get_u32(data + offset);
		switch (magic) {
		case RB_MAGIC_HARD:
			rb_info.hard_cfg_offs = offset;
			break;

		case RB_MAGIC_SOFT:
			rb_info.soft_cfg_offs = offset;
			break;
		}
	}

	if (!rb_info.hard_cfg_offs) {
		pr_err("could not find a valid RouterBOOT hard config\n");
		return NULL;
	}

	if (!rb_info.soft_cfg_offs) {
		pr_err("could not find a valid RouterBOOT soft config\n");
		return NULL;
	}

	rb_info.hard_cfg_size = RB_BLOCK_SIZE;
	rb_info.hard_cfg_data = kmemdup(data + rb_info.hard_cfg_offs,
					RB_BLOCK_SIZE, GFP_KERNEL);
	if (!rb_info.hard_cfg_data)
		return NULL;

	rb_info.board_name = rb_get_board_name();
	rb_info.hw_options = rb_get_hw_options();

	return &rb_info;
}
