/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _NET_DSA_TAG_RTL_OTTO_H
#define _NET_DSA_TAG_RTL_OTTO_H

#include <linux/spinlock.h>
#include <linux/types.h>

#define RTL_OTTO_TAG_LEN	5
#define RTL_OTTO_DEVICE_LOCAL	0xff

struct rtl_otto_tagger_data {
	rwlock_t cpu_device_lock;
	bool cpu_device_changing;
	u8 cpu_device;
};

#endif /* _NET_DSA_TAG_RTL_OTTO_H */
