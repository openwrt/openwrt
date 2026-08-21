/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef RTL931X_STACK_NETLINK_H
#define RTL931X_STACK_NETLINK_H

#include <stdbool.h>
#include <stdint.h>
#include <linux/if_ether.h>

struct nl_sock;

struct rtl931x_stack_nl {
	struct nl_sock *sock;
	int family;
	int kernel_error;
	char extack[256];
};

struct rtl931x_stack_request {
	int cmd;
	uint32_t ifindex;
	uint32_t generation;
	uint32_t flags;
	uint8_t enabled;
	uint8_t member;
	uint8_t peer;
	uint8_t master;
	uint8_t remote_port;
};

struct rtl931x_stack_status {
	uint64_t local_port_mask;
	uint64_t local_delegated_port_mask;
	uint32_t ifindex;
	uint32_t generation;
	uint32_t flags;
	uint32_t peer_netdevs_last_error;
	uint8_t enabled;
	uint8_t member;
	uint8_t peer;
	uint8_t master;
	uint8_t state;
	uint8_t link_up;
	uint8_t peer_netdevs_desired;
	uint8_t peer_netdevs_active;
	uint8_t peer_netdevs_published;
	uint8_t peer_netdevs_fenced;
	uint8_t peer_netdevs_recovering;
};

struct rtl931x_stack_probe {
	uint64_t transaction;
	uint64_t boot_nonce;
	uint32_t ifindex;
	uint32_t status;
	uint32_t generation;
	uint32_t round_trip_us;
	uint8_t mode;
	uint8_t member;
	uint8_t master;
	uint8_t stack_port;
};

struct rtl931x_stack_peer_switch {
	uint64_t boot_nonce;
	uint64_t port_mask;
	uint64_t admin_up_mask;
	uint64_t carrier_mask;
	uint64_t delegated_port_mask;
	uint32_t ifindex;
	uint32_t generation;
	uint32_t capabilities;
	uint16_t max_body_len;
	uint8_t member;
	uint8_t master;
	uint8_t port_count;
	uint8_t cpu_port;
	uint8_t stack_port;
};

struct rtl931x_stack_peer_port {
	uint64_t boot_nonce;
	uint32_t ifindex;
	uint32_t generation;
	uint32_t flags;
	uint32_t mtu;
	uint8_t member;
	uint8_t master;
	uint8_t port;
	uint8_t mac[ETH_ALEN];
};

union rtl931x_stack_reply {
	struct rtl931x_stack_status status;
	struct rtl931x_stack_probe probe;
	struct rtl931x_stack_peer_switch peer_switch;
	struct rtl931x_stack_peer_port peer_port;
};

int rtl931x_stack_nl_open(struct rtl931x_stack_nl *nl);
void rtl931x_stack_nl_close(struct rtl931x_stack_nl *nl);
int rtl931x_stack_nl_request(struct rtl931x_stack_nl *nl,
			     const struct rtl931x_stack_request *request,
			     union rtl931x_stack_reply *reply);

#endif
