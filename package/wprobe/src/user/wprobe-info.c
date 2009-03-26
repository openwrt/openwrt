/*
 * wprobe-test.c: Wireless probe user space test code
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdint.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wprobe.h>
#include "wprobe.h"

static const char *
wprobe_dump_value(struct wprobe_attribute *attr)
{
	static char buf[128];

#define HANDLE_TYPE(_type, _format) \
	case WPROBE_VAL_##_type: \
		snprintf(buf, sizeof(buf), _format, attr->val._type); \
		break

	switch(attr->type) {
		HANDLE_TYPE(S8, "%d");
		HANDLE_TYPE(S16, "%d");
		HANDLE_TYPE(S32, "%d");
		HANDLE_TYPE(S64, "%lld");
		HANDLE_TYPE(U8, "%d");
		HANDLE_TYPE(U16, "%d");
		HANDLE_TYPE(U32, "%d");
		HANDLE_TYPE(U64, "%lld");
		case WPROBE_VAL_STRING:
			/* FIXME: implement this */
		default:
			strncpy(buf, "<unknown>", sizeof(buf));
			break;
	}
	if ((attr->flags & WPROBE_F_KEEPSTAT) &&
		(attr->val.n > 0)) {
		int len = strlen(buf);
		snprintf(buf + len, sizeof(buf) - len, " (avg: %.02f; stdev: %.02f, n=%d)", attr->val.avg, attr->val.stdev, attr->val.n);
	}
#undef HANDLE_TYPE

	return buf;
}


static void
wprobe_dump_data(const char *ifname, struct list_head *gl, struct list_head *ll, struct list_head *ls)
{
	struct wprobe_attribute *attr;
	struct wprobe_link *link;
	bool first = true;

	fprintf(stderr, "\n");
	wprobe_request_data(ifname, gl, NULL, 2);
	list_for_each_entry(attr, gl, list) {
		fprintf(stderr, (first ?
			"Global:            %s=%s\n" :
			"                   %s=%s\n"),
			attr->name,
			wprobe_dump_value(attr)
		);
		first = false;
	}

	list_for_each_entry(link, ls, list) {
		first = true;
		wprobe_request_data(ifname, ll, link->addr, 2);
		list_for_each_entry(attr, ll, list) {
			if (first) {
				fprintf(stderr,
					"%02x:%02x:%02x:%02x:%02x:%02x: %s=%s\n",
					link->addr[0], link->addr[1], link->addr[2],
					link->addr[3], link->addr[4], link->addr[5],
					attr->name,
					wprobe_dump_value(attr));
				first = false;
			} else {
				fprintf(stderr,
					"                   %s=%s\n",
					attr->name,
					wprobe_dump_value(attr));
			}
		}
	}
}

static const char *attr_typestr[] = {
	[0] = "Unknown",
	[WPROBE_VAL_STRING] = "String",
	[WPROBE_VAL_U8] = "Unsigned 8 bit",
	[WPROBE_VAL_U16] = "Unsigned 16 bit",
	[WPROBE_VAL_U32] = "Unsigned 32 bit",
	[WPROBE_VAL_U64] = "Unsigned 64 bit",
	[WPROBE_VAL_S8] = "Signed 8 bit",
	[WPROBE_VAL_S16] = "Signed 16 bit",
	[WPROBE_VAL_S32] = "Signed 32 bit",
	[WPROBE_VAL_S64] = "Signed 64 bit",
};

static int usage(const char *prog)
{
	fprintf(stderr, "Usage: %s <interface>\n", prog);
	return 1;
}

int main(int argc, char **argv)
{
	struct wprobe_attribute *attr;
	const char *ifname;
	LIST_HEAD(global_attr);
	LIST_HEAD(link_attr);
	LIST_HEAD(links);
	int i = 0;

	if (argc < 2)
		return usage(argv[0]);

	ifname = argv[1];

	if (wprobe_init() != 0)
		return -1;

	wprobe_dump_attributes(ifname, false, &global_attr, NULL);
	wprobe_dump_attributes(ifname, true, &link_attr, NULL);

	if (list_empty(&global_attr) &&
		list_empty(&link_attr)) {
		fprintf(stderr, "Interface '%s' not found\n", ifname);
		return -1;
	}

	list_for_each_entry(attr, &global_attr, list) {
		fprintf(stderr, "Global attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}
	list_for_each_entry(attr, &link_attr, list) {
		fprintf(stderr, "Link attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}

	while (1) {
		usleep(100 * 1000);
		wprobe_measure(ifname);

		if (i-- > 0)
			continue;

		i = 10;
		wprobe_update_links(ifname, &links);
		wprobe_dump_data(ifname, &global_attr, &link_attr, &links);
	}
	wprobe_free();

	return 0;
}
