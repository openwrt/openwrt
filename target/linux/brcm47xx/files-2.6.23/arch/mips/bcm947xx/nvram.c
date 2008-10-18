/*
 * BCM947xx nvram variable access
 *
 * Copyright 2005, Broadcom Corporation
 * Copyright 2006, Felix Fietkau <nbd@openwrt.org>
 * 
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ssb/ssb.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <asm/byteorder.h>
#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <nvram.h>

#define MB * 1048576
extern struct ssb_bus ssb;

static char nvram_buf[NVRAM_SPACE];
static int cfe_env;
extern char *cfe_env_get(char *nv_buf, const char *name);
		
/* Probe for NVRAM header */
static void __init early_nvram_init(void)
{
	struct ssb_mipscore *mcore = &ssb.mipscore;
	struct nvram_header *header;
	int i;
	u32 base, lim, off;
	u32 *src, *dst;
	
	base = mcore->flash_window;
	lim = mcore->flash_window_size;
	cfe_env = 0;

	
	/* XXX: hack for supporting the CFE environment stuff on WGT634U */
	if (lim >= 8 MB) {
		src = (u32 *) KSEG1ADDR(base + 8 MB - 0x2000);
		dst = (u32 *) nvram_buf;

		if ((*src & 0xff00ff) == 0x000001) {
			printk("early_nvram_init: WGT634U NVRAM found.\n");

			for (i = 0; i < 0x1ff0; i++) {
				if (*src == 0xFFFFFFFF)
					break;
				*dst++ = *src++;
			}
			cfe_env = 1;
			return;
		}
	}

	off = 0x20000;
	while (off <= lim) {
		/* Windowed flash access */
		header = (struct nvram_header *) KSEG1ADDR(base + off - NVRAM_SPACE);
		if (header->magic == NVRAM_HEADER)
			goto found;
		off <<= 1;
	}

	/* Try embedded NVRAM at 4 KB and 1 KB as last resorts */
	header = (struct nvram_header *) KSEG1ADDR(base + 4096);
	if (header->magic == NVRAM_HEADER)
		goto found;
	
	header = (struct nvram_header *) KSEG1ADDR(base + 1024);
	if (header->magic == NVRAM_HEADER)
		goto found;
	
	return;

found:
	src = (u32 *) header;
	dst = (u32 *) nvram_buf;
	for (i = 0; i < sizeof(struct nvram_header); i += 4)
		*dst++ = *src++;
	for (; i < header->len && i < NVRAM_SPACE; i += 4)
		*dst++ = le32_to_cpu(*src++);
}

char *nvram_get(const char *name)
{
	char *var, *value, *end, *eq;

	if (!name)
		return NULL;

	if (!nvram_buf[0])
		early_nvram_init();

	if (cfe_env)
		return cfe_env_get(nvram_buf, name);

	/* Look for name=value and return value */
	var = &nvram_buf[sizeof(struct nvram_header)];
	end = nvram_buf + sizeof(nvram_buf) - 2;
	end[0] = end[1] = '\0';
	for (; *var; var = value + strlen(value) + 1) {
		if (!(eq = strchr(var, '=')))
			break;
		value = eq + 1;
		if ((eq - var) == strlen(name) && strncmp(var, name, (eq - var)) == 0)
			return value;
	}

	return NULL;
}

EXPORT_SYMBOL(nvram_get);
