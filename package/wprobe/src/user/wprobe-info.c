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
wprobe_dump_data(struct wprobe_iface *dev)
{
	struct wprobe_attribute *attr;
	struct wprobe_link *link;
	bool first = true;

	fprintf(stderr, "\n");
	wprobe_request_data(dev, NULL);
	list_for_each_entry(attr, &dev->global_attr, list) {
		fprintf(stderr, (first ?
			"Global:            %s=%s\n" :
			"                   %s=%s\n"),
			attr->name,
			wprobe_dump_value(attr)
		);
		first = false;
	}

	list_for_each_entry(link, &dev->links, list) {
		first = true;
		wprobe_request_data(dev, link->addr);
		list_for_each_entry(attr, &dev->link_attr, list) {
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
	fprintf(stderr,
		"Usage: %s <interface> [options]\n"
		"\n"
		"Options:\n"
		"  -c:            Only apply configuration\n"
		"  -h:            This help text\n"
		"  -i <interval>: Set measurement interval\n"
		"  -m:            Run measurement loop\n"
		"\n"
		, prog);
	exit(1);
}

static void show_attributes(struct wprobe_iface *dev)
{
	struct wprobe_attribute *attr;
	list_for_each_entry(attr, &dev->global_attr, list) {
		fprintf(stderr, "Global attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}
	list_for_each_entry(attr, &dev->link_attr, list) {
		fprintf(stderr, "Link attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}
}

static void loop_measurement(struct wprobe_iface *dev)
{
	while (1) {
		sleep(1);
		wprobe_update_links(dev);
		wprobe_dump_data(dev);
	}
}

int main(int argc, char **argv)
{
	struct wprobe_iface *dev;
	const char *ifname;
	const char *prog = argv[0];
	enum {
		CMD_NONE,
		CMD_CONFIG,
		CMD_MEASURE,
	} cmd = CMD_NONE;
	int ch;

	if ((argc < 2) || (argv[1][0] == '-'))
		return usage(prog);

	ifname = argv[1];
	dev = wprobe_get_dev(ifname);
	argv++;
	argc--;

	if (!dev || (list_empty(&dev->global_attr) &&
		list_empty(&dev->link_attr))) {
		fprintf(stderr, "Interface '%s' not found\n", ifname);
		return -1;
	}

	while ((ch = getopt(argc, argv, "chi:m")) != -1) {
		switch(ch) {
		case 'c':
			cmd = CMD_CONFIG;
			break;
		case 'm':
			cmd = CMD_MEASURE;
			break;
		case 'i':
			dev->interval = strtoul(optarg, NULL, 10);
			break;
		case 'h':
		default:
			usage(prog);
			break;
		}
	}

	wprobe_apply_config(dev);
	if (cmd != CMD_CONFIG)
		show_attributes(dev);
	if (cmd == CMD_MEASURE)
		loop_measurement(dev);

	wprobe_free_dev(dev);

	return 0;
}
