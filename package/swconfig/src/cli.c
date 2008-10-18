/*
 * swconfig.c: Switch configuration utility
 *
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundatio.
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
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/switch.h>
#include "swlib.h"

#define GET		1
#define SET		2

void print_attrs(struct switch_attr *attr)
{
	int i = 0;
	while (attr) {
		const char *type;
		switch(attr->type) {
			case SWITCH_TYPE_INT:
				type = "int";
				break;
			case SWITCH_TYPE_STRING:
				type = "string";
				break;
			case SWITCH_TYPE_PORTS:
				type = "ports";
				break;
			case SWITCH_TYPE_NOVAL:
				type = "none";
				break;
			default:
				type = "unknown";
				break;
		}
		printf("\tAttribute %d (%s): %s (%s)\n", ++i, type, attr->name, attr->description);
		attr = attr->next;
	}
}

void list_attributes(struct switch_dev *dev)
{
	printf("Switch %d: %s(%s), ports: %d, vlans: %d\n", dev->id, dev->dev_name, dev->name, dev->ports, dev->vlans);
	printf("     --switch\n");
	print_attrs(dev->ops);
	printf("     --vlan\n");
	print_attrs(dev->vlan_ops);
	printf("     --port\n");
	print_attrs(dev->port_ops);
}

void print_usage(void)
{
	printf("swconfig dev <dev> [port <port>|vlan <vlan>] (help|set <key> <value>|get <key>)\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int retval = 0;
	struct switch_dev *dev;
	struct switch_attr *a;
	struct switch_val val;
	int err;
	int i;

	struct switch_port *ports;

	int cmd = 0;
	char *cdev = NULL;
	int cport = -1;
	int cvlan = -1;
	char *ckey = NULL;
	char *cvalue = NULL;
	int chelp = 0;

	if(argc < 4)
		print_usage();

	if(strcmp(argv[1], "dev"))
		print_usage();

	cdev = argv[2];

	for(i = 3; i < argc; i++)
	{
		int p;
		if(!strcmp(argv[i], "help"))
		{
			chelp = 1;
			continue;
		}
		if(i + 1 >= argc)
			print_usage();
		p = atoi(argv[i + 1]);
		if(!strcmp(argv[i], "port"))
		{
			cport = p;
		} else if(!strcmp(argv[i], "vlan"))
		{
			cvlan = p;
		} else if(!strcmp(argv[i], "set"))
		{
			if(argc <= i + 1)
				print_usage();
			cmd = SET;
			ckey = argv[i + 1];
			if (argc > i + 2)
				cvalue = argv[i + 2];
			else
				cvalue = NULL;
			i++;
		} else if(!strcmp(argv[i], "get"))
		{
			cmd = GET;
			ckey = argv[i + 1];
		} else{
			print_usage();
		}
		i++;
	}

	if(cport > -1 && cvlan > -1)
		print_usage();

	dev = swlib_connect(cdev);
	if (!dev) {
		fprintf(stderr, "Failed to connect to the switch\n");
		return 1;
	}

	ports = malloc(sizeof(struct switch_port) * dev->ports);
	memset(ports, 0, sizeof(struct switch_port) * dev->ports);
	swlib_scan(dev);

	if(chelp)
	{
		list_attributes(dev);
		goto out;
	}

	if(cport > -1)
		a = swlib_lookup_attr(dev, SWLIB_ATTR_GROUP_PORT, ckey);
	else if(cvlan > -1)
		a = swlib_lookup_attr(dev, SWLIB_ATTR_GROUP_VLAN, ckey);
	else
		a = swlib_lookup_attr(dev, SWLIB_ATTR_GROUP_GLOBAL, ckey);

	if(!a)
	{
		fprintf(stderr, "Unknown attribute \"%s\"\n", ckey);
		goto out;
	}

	switch(cmd)
	{
	case SET:
		if ((a->type != SWITCH_TYPE_NOVAL) &&
				(cvalue == NULL))
			print_usage();

		switch(a->type) {
		case SWITCH_TYPE_INT:
			val.value.i = atoi(cvalue);
			break;
		case SWITCH_TYPE_STRING:
			val.value.s = cvalue;
			break;
		case SWITCH_TYPE_PORTS:
			val.len = 0;
			while(cvalue && *cvalue)
			{
				ports[val.len].flags = 0;
				ports[val.len].id = strtol(cvalue, &cvalue, 10);
				while(*cvalue && !isspace(*cvalue)) {
					if (*cvalue == 't')
						ports[val.len].flags |= SWLIB_PORT_FLAG_TAGGED;
					cvalue++;
				}
				if (*cvalue)
					cvalue++;
				val.len++;
			}
			val.value.ports = ports;
			break;
		default:
			break;
		}
		if(cvlan > -1)
			val.port_vlan = cvlan;
		if(cport > -1)
			val.port_vlan = cport;
		if(swlib_set_attr(dev, a, &val) < 0)
		{
			fprintf(stderr, "failed\n");
			retval = -1;
			goto out;
		}
		break;
	case GET:
		if(cvlan > -1)
			val.port_vlan = cvlan;
		if(cport > -1)
			val.port_vlan = cport;
		if(swlib_get_attr(dev, a, &val) < 0)
		{
			fprintf(stderr, "failed\n");
			retval = -1;
			goto out;
		}
		switch(a->type) {
		case SWITCH_TYPE_INT:
			printf("%d\n", val.value.i);
			break;
		case SWITCH_TYPE_STRING:
			printf("%s\n", val.value.s);
			break;
		case SWITCH_TYPE_PORTS:
			for(i = 0; i < val.len; i++)
				printf("%d ", val.value.ports[i]);
			printf("\n");
			break;
		}
	}

out:
	swlib_free_all(dev);
	free(ports);

	return 0;
}
