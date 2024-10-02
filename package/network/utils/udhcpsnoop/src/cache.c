// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */

#include <libubox/avl.h>

#include "dhcpsnoop.h"
#include "msg.h"

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_VAR(x) x[0], x[1], x[2], x[3], x[4], x[5]

#define IP_FMT  "%d.%d.%d.%d"
#define IP_VAR(x) x[0], x[1], x[2], x[3]

struct mac {
        struct avl_node avl;
	uint8_t mac[6];
	uint8_t ip[4];
	struct uloop_timeout rebind;
};

static int
avl_mac_cmp(const void *k1, const void *k2, void *ptr)
{
	return memcmp(k1, k2, 6);
}

static struct avl_tree mac_tree = AVL_TREE_INIT(mac_tree, avl_mac_cmp, false, NULL);

static void
cache_expire(struct uloop_timeout *t)
{
	struct mac *mac = container_of(t, struct mac, rebind);

	avl_delete(&mac_tree, &mac->avl);
	free(mac);
}

void
cache_entry(void *_msg, uint32_t rebind)
{
	struct dhcpv4_message *msg = (struct dhcpv4_message *) _msg;
	struct mac *mac;

	mac = avl_find_element(&mac_tree, msg->chaddr, mac, avl);

	if (!mac) {
		mac = malloc(sizeof(*mac));
		if (!mac)
			return;
		memset(mac, 0, sizeof(*mac));
		memcpy(mac->mac, msg->chaddr, 6);
		mac->avl.key = mac->mac;
		mac->rebind.cb = cache_expire;
		avl_insert(&mac_tree, &mac->avl);
	}
	memcpy(mac->ip, &msg->yiaddr.s_addr, 4);
	uloop_timeout_set(&mac->rebind, rebind * 1000);
}

void
cache_dump(struct blob_buf *b)
{
	struct mac *mac;

	avl_for_each_element(&mac_tree, mac, avl) {
		char addr[18];
		char ip[16];

		snprintf(addr, sizeof(addr), MAC_FMT, MAC_VAR(mac->mac));
		snprintf(ip, sizeof(ip), IP_FMT, IP_VAR(mac->ip));

		blobmsg_add_string(b, addr, ip);
	}
}
