/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _NET_DSA_TAG_RTL_OTTO_H
#define _NET_DSA_TAG_RTL_OTTO_H

#include <linux/spinlock.h>
#include <linux/types.h>

#define RTL_OTTO_TAG_LEN	5
#define RTL_OTTO_DEVICE_LOCAL	0xff
#define RTL_OTTO_MAX_PORTS	56

struct net_device;

struct rtl_otto_remote_port_map {
	struct net_device *ports[RTL_OTTO_MAX_PORTS];
	bool active;
	u8 device;
	u8 fabric_port;
};

struct rtl_otto_tagger_data {
	rwlock_t cpu_device_lock;
	struct rtl_otto_remote_port_map __rcu *remote_ports;
	struct net_device *stack_cpu_dev;
	bool cpu_device_changing;
	bool stack_cpu_active;
	u8 cpu_device;
	u8 stack_cpu_peer_device;
	u8 stack_cpu_fabric_port;
	u32 cpu_device_generation;
};

#endif /* _NET_DSA_TAG_RTL_OTTO_H */
