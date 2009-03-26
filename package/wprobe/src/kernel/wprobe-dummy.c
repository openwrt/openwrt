/*
 * wprobe-core.c: Wireless probe interface dummy driver
 * Copyright (C) 2008-2009 Felix Fietkau <nbd@openwrt.org>
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
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/random.h>
#include <linux/wprobe.h>

static const char local_addr[] = "\x00\x13\x37\xba\xbe\x00";

enum dummy_global_values {
	DUMMY_GLOBAL_MEDIUM_BUSY
};
enum dummy_link_values {
	DUMMY_LINK_SNR
};

struct wprobe_item dummy_perlink[] = {
	[DUMMY_LINK_SNR] = {
		.name = "snr",
		.type = WPROBE_VAL_U8,
		.flags = WPROBE_F_KEEPSTAT,
	},
};

struct wprobe_item dummy_globals[] = {
	[DUMMY_GLOBAL_MEDIUM_BUSY] = {
		.name = "medium_busy",
		.type = WPROBE_VAL_U8,
		.flags = WPROBE_F_KEEPSTAT,
	}
};

int dummy_sync(struct wprobe_iface *dev, struct wprobe_link *l, struct wprobe_value *val, bool measure)
{
	u8 intval = 0;

	get_random_bytes(&intval, 1);
	if (l) {
		WPROBE_FILL_BEGIN(val, dummy_perlink);
		WPROBE_SET(DUMMY_LINK_SNR, U8, (intval % 40));
		WPROBE_FILL_END();
	} else {
		WPROBE_FILL_BEGIN(val, dummy_globals);
		WPROBE_SET(DUMMY_GLOBAL_MEDIUM_BUSY, U8, (intval % 100));
		WPROBE_FILL_END();
	}
	return 0;
}

static struct wprobe_iface dummy_dev = {
	.name = "dummy",
	.addr = local_addr,
	.link_items = dummy_perlink,
	.n_link_items = ARRAY_SIZE(dummy_perlink),
	.global_items = dummy_globals,
	.n_global_items = ARRAY_SIZE(dummy_globals),
	.sync_data = dummy_sync,
};

static struct wprobe_link dummy_link;

static int __init
wprobe_dummy_init(void)
{
	wprobe_add_iface(&dummy_dev);
	wprobe_add_link(&dummy_dev, &dummy_link, "\x00\x13\x37\xda\xda\x00");
	return 0;
}

static void __exit
wprobe_dummy_exit(void)
{
	wprobe_remove_link(&dummy_dev, &dummy_link);
	wprobe_remove_iface(&dummy_dev);
}

module_init(wprobe_dummy_init);
module_exit(wprobe_dummy_exit);

MODULE_LICENSE("GPL");
