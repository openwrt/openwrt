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

/**
 * wprobe_init: initialize internal data structures and connect to the wprobe netlink api
 */
extern int wprobe_init(void);

/**
 * wprobe_free: free all internally allocated data structures
 */
extern void wprobe_free(void);

/**
 * wprobe_update_links: get a list of all link partners
 * @ifname: name of the wprobe interface
 * @list: linked list for storing link descriptions
 *
 * when wprobe_update_links is called multiple times, the linked list 
 * is updated with new link partners, old entries are automatically expired
 */
extern int wprobe_update_links(const char *ifname, struct list_head *list);

/**
 * wprobe_measure: start a measurement request for all global attributes
 * @ifname: name of the wprobe interface
 *
 * not all attributes are automatically filled with data, since for some
 * it may be desirable to control the sampling interval from user space
 * you can use this function to do that.
 */
extern void wprobe_measure(const char *ifname);

/**
 * wprobe_dump_attributes: create a linked list of available attributes
 * @ifname: name of the wprobe interface
 * @link: false: get the list of global attributes; true: get the list of per-link attributes
 * @list: linked list to store the attributes in
 * @addr: buffer to store the interface's mac address in (optional)
 *
 * attributes must be freed by the caller
 */
extern int wprobe_dump_attributes(const char *ifname, bool link, struct list_head *list, char *addr);

/**
 * wprobe_request_data: request new sampling values for the given list of attributes
 * @ifname: name of the wprobe interface
 * @attrs: attribute list
 * @addr: (optional) mac address of the link partner
 * @scale: scale down values by a factor (scale < 0: reset statistics entirely)
 *
 * if addr is unset, attrs must point to the list of global attributes,
 * if addr is set, attrs must point to the list of per-link attributes
 */
extern int wprobe_request_data(const char *ifname, struct list_head *attrs, const unsigned char *addr, int scale);

#endif
