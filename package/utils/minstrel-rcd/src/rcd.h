// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2021 Felix Fietkau <nbd@nbd.name> */
#ifndef __MINSTREL_RCD_H
#define __MINSTREL_RCD_H

#include <libubox/list.h>
#include <libubox/vlist.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define RCD_PORT 0x5243

struct phy {
	struct vlist_node node;

	struct uloop_fd event_fd;
	int control_fd;
};

struct client {
	struct list_head list;
	struct ustream_fd sfd;
	bool init_done;
};

struct server {
	struct list_head list;
	struct uloop_fd fd;
	const char *addr;
};

static inline const char *phy_name(struct phy *phy)
{
	return phy->node.avl.key;
}

extern struct vlist_tree phy_list;

void rcd_server_add(const char *addr);
void rcd_server_init(void);

void rcd_client_accept(int fd);
void rcd_client_phy_event(struct phy *phy, const char *str);
void rcd_client_set_phy_state(struct client *cl, struct phy *phy, bool add);

void rcd_phy_init(void);
void rcd_phy_init_client(struct client *cl);
void rcd_phy_dump(struct client *cl, struct phy *phy);
void rcd_phy_control(struct client *cl, char *data);

#define client_printf(cl, ...) ustream_printf(&(cl)->sfd.stream, __VA_ARGS__)
#define client_phy_printf(cl, phy, fmt, ...) client_printf(cl, "%s;" fmt, phy_name(phy), ## __VA_ARGS__)

#endif
