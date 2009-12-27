/*
 *  Atheros AR71xx minimal nvram support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/string.h>

#include "nvram.h"

char *nvram_find_var(const char *name, const char *buf, unsigned buf_len)
{
	unsigned len = strlen(name);
	char *cur, *last;

	if (buf_len == 0 || len == 0)
		return NULL;

	if (buf_len < len)
		return NULL;

	if (len == 1)
		return memchr(buf, (int) *name, buf_len);

	last = (char *) buf + buf_len - len;
	for (cur = (char *) buf; cur <= last; cur++)
		if (cur[0] == name[0] && memcmp(cur, name, len) == 0)
			return cur + len;

	return NULL;
}

int nvram_parse_mac_addr(const char *nvram, unsigned nvram_len,
			 const char *name, char *mac)
{
	char *buf;
	char *mac_str;
	int ret;
	int t;

	buf = vmalloc(nvram_len);
	if (!buf)
		return -ENOMEM;

	memcpy(buf, nvram, nvram_len);
	buf[nvram_len - 1] = '\0';

	mac_str = nvram_find_var(name, buf, nvram_len);
	if (!mac_str) {
		ret = -EINVAL;
		goto free;
	}

	t = sscanf(mac_str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		   &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

	if (t != 6) {
		ret = -EINVAL;
		goto free;
	}

	ret = 0;

 free:
	vfree(buf);
	return ret;
}
