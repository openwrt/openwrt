/*
 * switch.h: Switch configuration API
 *
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
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

#ifndef __LINUX_SWITCH_H
#define __LINUX_SWITCH_H

#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#ifndef __KERNEL__
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#else
#include <net/genetlink.h>
#endif

/* main attributes */
enum {
	SWITCH_ATTR_UNSPEC,
	/* global */
	SWITCH_ATTR_TYPE,
	/* device */
	SWITCH_ATTR_ID,
	SWITCH_ATTR_NAME,
	SWITCH_ATTR_DEV_NAME,
	SWITCH_ATTR_VLANS,
	SWITCH_ATTR_PORTS,
	/* attributes */
	SWITCH_ATTR_OP_ID,
	SWITCH_ATTR_OP_TYPE,
	SWITCH_ATTR_OP_NAME,
	SWITCH_ATTR_OP_PORT,
	SWITCH_ATTR_OP_VLAN,
	SWITCH_ATTR_OP_VALUE_INT,
	SWITCH_ATTR_OP_VALUE_STR,
	SWITCH_ATTR_OP_VALUE_PORTS,
	SWITCH_ATTR_OP_DESCRIPTION,
	/* port lists */
	SWITCH_ATTR_PORT,
	SWITCH_ATTR_MAX
};

/* commands */
enum {
	SWITCH_CMD_UNSPEC,
	SWITCH_CMD_GET_SWITCH,
	SWITCH_CMD_NEW_ATTR,
	SWITCH_CMD_LIST_GLOBAL,
	SWITCH_CMD_GET_GLOBAL,
	SWITCH_CMD_SET_GLOBAL,
	SWITCH_CMD_LIST_PORT,
	SWITCH_CMD_GET_PORT,
	SWITCH_CMD_SET_PORT,
	SWITCH_CMD_LIST_VLAN,
	SWITCH_CMD_GET_VLAN,
	SWITCH_CMD_SET_VLAN
};

/* data types */
enum switch_val_type {
	SWITCH_TYPE_UNSPEC,
	SWITCH_TYPE_INT,
	SWITCH_TYPE_STRING,
	SWITCH_TYPE_PORTS,
	SWITCH_TYPE_NOVAL,
};

/* port nested attributes */
enum {
	SWITCH_PORT_UNSPEC,
	SWITCH_PORT_ID,
	SWITCH_PORT_FLAG_TAGGED,
	SWITCH_PORT_ATTR_MAX
};

#define SWITCH_ATTR_DEFAULTS_OFFSET	0x1000

#ifdef __KERNEL__

struct switch_dev;
struct switch_op;
struct switch_val;
struct switch_attr;
struct switch_attrlist;

int register_switch(struct switch_dev *dev, struct net_device *netdev);
void unregister_switch(struct switch_dev *dev);

struct switch_attrlist {
	/* filled in by the driver */
	int n_attr;
	const struct switch_attr *attr;
};


struct switch_dev {
	int id;
	void *priv;
	const char *name;

	/* NB: either devname or netdev must be set */
	const char *devname;
	struct net_device *netdev;

	int ports;
	int vlans;
	int cpu_port;
	struct switch_attrlist attr_global, attr_port, attr_vlan;

	spinlock_t lock;
	struct switch_port *portbuf;
	struct list_head dev_list;
	unsigned long def_global, def_port, def_vlan;

	int (*get_vlan_ports)(struct switch_dev *dev, struct switch_val *val);
	int (*set_vlan_ports)(struct switch_dev *dev, struct switch_val *val);
	int (*get_port_pvid)(struct switch_dev *dev, int port, int *val);
	int (*set_port_pvid)(struct switch_dev *dev, int port, int val);
	int (*apply_config)(struct switch_dev *dev);
	int (*reset_switch)(struct switch_dev *dev);
};

struct switch_port {
	u32 id;
	u32 flags;
};

struct switch_val {
	const struct switch_attr *attr;
	int port_vlan;
	int len;
	union {
		const char *s;
		u32 i;
		struct switch_port *ports;
	} value;
};

struct switch_attr {
	int disabled;
	int type;
	const char *name;
	const char *description;

	int (*set)(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val);
	int (*get)(struct switch_dev *dev, const struct switch_attr *attr, struct switch_val *val);

	/* for driver internal use */
	int id;
	int ofs;
	int max;
};

#endif

#endif
