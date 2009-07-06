/*
 * wprobe.h: Wireless probe user space library
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

#ifndef __WPROBE_USER_H
#define __WPROBE_USER_H
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include "list.h"

/**
 * struct wprobe_value: data structure for attribute values
 * @STRING: string value (currently unsupported)
 * @U8: unsigned 8-bit integer value
 * @U16: unsigned 16-bit integer value
 * @U32: unsigned 32-bit integer value
 * @U64: unsigned 64-bit integer value
 * @S8: signed 8-bit integer value
 * @S16: signed 16-bit integer value
 * @S32: signed 32-bit integer value
 * @S64: signed 64-bit integer value
 *
 * @n: number of sample values 
 * @avg: average value
 * @stdev: standard deviation
 * @s: sum of all sample values (internal use)
 * @ss: sum of all sample values squared (internal use)
 */
struct wprobe_value {
	/* attribute value */
	union {
		const char *STRING;
		uint8_t U8;
		uint16_t U16;
		uint32_t U32;
		uint64_t U64;
		int8_t S8;
		int16_t S16;
		int32_t S32;
		int64_t S64;
	};
	/* statistics */
	int64_t s, ss;
	float avg, stdev;
	unsigned int n;
};

/**
 * struct wprobe_attribute: data structures for attribute descriptions
 * @list: linked list data structure for a list of attributes
 * @id: attribute id
 * @type: netlink type for the attribute (see kernel api documentation)
 * @flags: attribute flags (see kernel api documentation)
 * @val: cached version of the last netlink query, will be overwritten on each request
 * @name: attribute name
 */
struct wprobe_attribute {
	struct list_head list;
	int id;
	int type;
	uint32_t flags;
	struct wprobe_value val;
	char name[];
};

/**
 * struct wprobe_link: data structure for the link description
 * @list: linked list data structure for a list of links 
 * @flags: link flags (see kernel api documentation)
 * @addr: mac address of the remote link partner
 */
struct wprobe_link {
	struct list_head list;
	uint32_t flags;
	unsigned char addr[6];
};

struct wprobe_filter_item {
	char name[32];
	uint64_t rx;
	uint64_t tx;
};

struct wprobe_iface_ops;
struct wprobe_iface {
	const struct wprobe_iface_ops *ops;

	int sockfd;
	const char *ifname;
	unsigned int genl_family;
	char addr[6];

	struct list_head global_attr;
	struct list_head link_attr;
	struct list_head links;

	/* config */
	int interval;
	int scale_min;
	int scale_max;
	int scale_m;
	int scale_d;

	/* filter */
	void *filter;

	/* filter_len:
	 *   set to -1 to drop the current filter
	 *   automatically reset to 0 after config apply
	 */
	int filter_len;
};

typedef void (*wprobe_filter_cb)(void *arg, const char *group, struct wprobe_filter_item *items, int n_items);
extern int wprobe_port;

/**
 * wprobe_update_links: get a list of all link partners
 * @dev: wprobe device structure
 * @list: linked list for storing link descriptions
 *
 * when wprobe_update_links is called multiple times, the linked list 
 * is updated with new link partners, old entries are automatically expired
 */
extern int wprobe_update_links(struct wprobe_iface *dev);

/**
 * wprobe_dump_filters: dump all layer 2 filter counters
 * @dev: wprobe device structure
 * @cb: callback (called once per filter group)
 * @arg: user argument for the callback
 */
extern int wprobe_dump_filters(struct wprobe_iface *dev, wprobe_filter_cb cb, void *arg);

/**
 * wprobe_measure: start a measurement request for all global attributes
 * @dev: wprobe device structure
 *
 * not all attributes are automatically filled with data, since for some
 * it may be desirable to control the sampling interval from user space
 * you can use this function to do that.
 */
extern int wprobe_measure(struct wprobe_iface *dev);

/**
 * wprobe_get_dev: get a handle to a local wprobe device
 * @ifname: name of the wprobe interface
 *
 * queries the wprobe interface for all attributes
 * must be freed with wprobe_free_dev
 */
extern struct wprobe_iface *wprobe_get_dev(const char *ifname);

/**
 * wprobe_get_auto: get a handle to a local or remote wprobe device
 * @arg: pointer to the wprobe device, either <dev> (local) or <host>:<dev> (remote)
 */
extern struct wprobe_iface *wprobe_get_auto(const char *arg, char **err);

/**
 * wprobe_get_dev: free all device information
 * @dev: wprobe device structure
 */
extern void wprobe_free_dev(struct wprobe_iface *dev);

/**
 * wprobe_apply_config: apply configuration data
 * @dev: wprobe device structure
 *
 * uploads all configuration values from @dev that are not set to -1
 */
extern int wprobe_apply_config(struct wprobe_iface *dev);

/**
 * wprobe_request_data: request new sampling values for the given list of attributes
 * @dev: wprobe device structure
 * @addr: (optional) mac address of the link partner
 *
 * if addr is unset, global values are stored in the global attributes list
 * if addr is set, per-link values for the given address are stored in the link attributes list
 */
extern int wprobe_request_data(struct wprobe_iface *dev, const unsigned char *addr);

/**
 * wprobe_server_init: send a wprobe server init message to a server's client socket
 * @socket: socket of the connection to the client
 */
extern int wprobe_server_init(int socket);

/**
 * wprobe_server_handle: read a request from the client socket, process it, send the response
 * @socket: socket of the connection to the client
 */
extern int wprobe_server_handle(int socket);

/**
 * wprobe_server_done: release memory allocated for the server connection
 */
extern void wprobe_server_done(void);

#endif
