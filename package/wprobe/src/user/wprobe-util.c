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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdint.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>

#include <linux/wprobe.h>
#include "wprobe.h"

static bool simple_mode = false;

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
		if (simple_mode)
			snprintf(buf + len, sizeof(buf) - len, ";%.02f;%.02f;%d;%lld;%lld", attr->val.avg, attr->val.stdev, attr->val.n, attr->val.s, attr->val.ss);
		else
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

	if (!simple_mode)
		fprintf(stderr, "\n");
	wprobe_request_data(dev, NULL);
	list_for_each_entry(attr, &dev->global_attr, list) {
		if (simple_mode) {
			if (first)
				fprintf(stdout, "[global]\n");
			fprintf(stdout, "%s=%s\n", attr->name, wprobe_dump_value(attr));
		} else {
			fprintf(stderr, (first ?
				"Global:            %s=%s\n" :
				"                   %s=%s\n"),
				attr->name,
				wprobe_dump_value(attr)
			);
		}
		first = false;
	}

	list_for_each_entry(link, &dev->links, list) {
		first = true;
		wprobe_request_data(dev, link->addr);
		list_for_each_entry(attr, &dev->link_attr, list) {
			if (first) {
				fprintf((simple_mode ? stdout : stderr),
					(simple_mode ? 
					 "[%02x:%02x:%02x:%02x:%02x:%02x]\n%s=%s\n" :
					 "%02x:%02x:%02x:%02x:%02x:%02x: %s=%s\n"),
					link->addr[0], link->addr[1], link->addr[2],
					link->addr[3], link->addr[4], link->addr[5],
					attr->name,
					wprobe_dump_value(attr));
				first = false;
			} else {
				fprintf((simple_mode ? stdout : stderr),
					(simple_mode ? "%s=%s\n" :
					 "                   %s=%s\n"),
					attr->name,
					wprobe_dump_value(attr));
			}
		}
	}
	fflush(stdout);
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
#ifndef NO_LOCAL_ACCESS 
		"Usage: %s <interface>|<host>:<device>|-P [options]\n"
#else
		"Usage: %s <host>:<device> [options]\n"
#endif
		"\n"
		"Options:\n"
		"  -c:            Only apply configuration\n"
		"  -d:            Delay between measurement dumps (in milliseconds, default: 1000)\n"
		"  -f:            Dump contents of layer 2 filter counters during measurement\n"
		"  -F <file>:     Apply layer 2 filters from <file>\n"
		"  -h:            This help text\n"
		"  -i <interval>: Set measurement interval\n"
		"  -m:            Run measurement loop\n"
		"  -p:            Set the TCP port for server/client (default: 17990)\n"
#ifndef NO_LOCAL_ACCESS 
		"  -P:            Run in proxy mode (listen on network)\n"
#endif
		"\n"
		, prog);
	exit(1);
}

static void show_attributes(struct wprobe_iface *dev)
{
	struct wprobe_attribute *attr;
	if (simple_mode)
		return;
	list_for_each_entry(attr, &dev->global_attr, list) {
		fprintf(stderr, "Global attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}
	list_for_each_entry(attr, &dev->link_attr, list) {
		fprintf(stderr, "Link attribute: '%s' (%s)\n",
			attr->name, attr_typestr[attr->type]);
	}
}

static void show_filter_simple(void *arg, const char *group, struct wprobe_filter_item *items, int n_items)
{
	int i;

	fprintf(stdout, "[filter:%s]\n", group);
	for (i = 0; i < n_items; i++) {
		fprintf(stdout, "%s=%lld;%lld\n",
			items[i].name, items[i].tx, items[i].rx);
	}
	fflush(stdout);
}


static void show_filter(void *arg, const char *group, struct wprobe_filter_item *items, int n_items)
{
	int i;
	fprintf(stderr, "Filter group: '%s' (tx/rx)\n", group);
	for (i = 0; i < n_items; i++) {
		fprintf(stderr, " - %s (%lld/%lld)\n",
			items[i].name, items[i].tx, items[i].rx);
	}
}

static void loop_measurement(struct wprobe_iface *dev, bool print_filters, unsigned long delay)
{
	while (1) {
		usleep(delay * 1000);
		wprobe_update_links(dev);
		wprobe_dump_data(dev);
		if (print_filters)
			wprobe_dump_filters(dev, simple_mode ? show_filter_simple : show_filter, NULL);
	}
}

static void set_filter(struct wprobe_iface *dev, const char *filename)
{
	unsigned char *buf = NULL;
	unsigned int buflen = 0;
	unsigned int len = 0;
	int fd;

	/* clear filter */
	if (filename[0] == 0) {
		dev->filter_len = -1;
		return;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open filter");
		return;
	}

	do {
		int rlen;

		if (!buf) {
			len = 0;
			buflen = 1024;
			buf = malloc(1024);
		} else {
			buflen *= 2;
			buf = realloc(buf, buflen);
		}
		rlen = read(fd, buf + len, buflen - len);
		if (rlen < 0)
			break;

		len += rlen;
	} while (len == buflen);

	dev->filter = buf;
	dev->filter_len = len;
	close(fd);
}

#ifndef NO_LOCAL_ACCESS

static void sigchld_handler(int s)
{
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

static int run_proxy(int port)
{
	struct sockaddr_in sa;
	struct sigaction sig;
	int v = 1;
	int s;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		return 1;
	}

	sig.sa_handler = sigchld_handler;  // Signal Handler fuer Zombie Prozesse
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &sig, NULL);

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(wprobe_port);

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
	if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind");
		return 1;
	}
	if (listen(s, 10)) {
		perror("listen");
		return 1;
	}
	while(1) {
		unsigned int addrlen = sizeof(struct sockaddr_in);
		int ret, c;

		c = accept(s, (struct sockaddr *)&sa, &addrlen);
		if (c < 0) {
			if (errno == EINTR)
				continue;

			perror("accept");
			return 1;
		}
		if (fork() == 0) {
			/* close server socket, stdin, stdout, stderr */
			close(s);
			close(0);
			close(1);
			close(2);

			wprobe_server_init(c);
			do {
				ret = wprobe_server_handle(c);
			} while (ret >= 0);
			wprobe_server_done();
			close(c);
			exit(0);
		}
		close(c);
	}

	return 0;
}
#endif

int main(int argc, char **argv)
{
	struct wprobe_iface *dev = NULL;
	const char *ifname;
	const char *prog = argv[0];
	char *err = NULL;
	enum {
		CMD_NONE,
		CMD_CONFIG,
		CMD_MEASURE,
		CMD_PROXY,
	} cmd = CMD_NONE;
	const char *filter = NULL;
	bool print_filters = false;
	unsigned long delay = 1000;
	int interval = -1;
	int ch;

	if (argc < 2)
		return usage(prog);

#ifndef NO_LOCAL_ACCESS 
	if (!strcmp(argv[1], "-P")) {
		while ((ch = getopt(argc - 1, argv + 1, "p:")) != -1) {
			switch(ch) {
			case 'p':
				/* set port */
				wprobe_port = strtoul(optarg, NULL, 0);
				break;
			default:
				return usage(prog);
			}
		}
		return run_proxy(wprobe_port);
	}
#endif

	if (argv[1][0] == '-')
		return usage(prog);

	ifname = argv[1];
	argv++;
	argc--;

	while ((ch = getopt(argc, argv, "cd:fF:hi:msp:")) != -1) {
		switch(ch) {
		case 'c':
			cmd = CMD_CONFIG;
			break;
		case 'd':
			delay = strtoul(optarg, NULL, 10);
			break;
		case 'm':
			cmd = CMD_MEASURE;
			break;
		case 'i':
			interval = strtoul(optarg, NULL, 10);
			break;
		case 'f':
			print_filters = true;
			break;
		case 'F':
			if (filter) {
				fprintf(stderr, "Cannot set multiple filters\n");
				return usage(prog);
			}
			filter = optarg;
			break;
		case 's':
			simple_mode = true;
			break;
		case 'p':
			/* set port */
			wprobe_port = strtoul(optarg, NULL, 0);
			break;
		case 'h':
		default:
			usage(prog);
			break;
		}
	}

	dev = wprobe_get_auto(ifname, &err);
	if (!dev || (list_empty(&dev->global_attr) &&
		list_empty(&dev->link_attr))) {
		if (err)
			fprintf(stderr, "%s\n", err);
		else
			fprintf(stderr, "Interface '%s' not found\n", ifname);
		return 1;
	}

	if (filter || interval >= 0) {
		if (filter)
			set_filter(dev, filter);
		if (interval >= 0)
			dev->interval = interval;

		wprobe_apply_config(dev);
	}

	if (cmd != CMD_CONFIG)
		show_attributes(dev);
	if (cmd == CMD_MEASURE)
		loop_measurement(dev, print_filters, delay);

	wprobe_free_dev(dev);

	return 0;
}
