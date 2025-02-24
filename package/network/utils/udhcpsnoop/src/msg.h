/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef __DHCPSNOOP_MSG_H
#define __DHCPSNOOP_MSG_H

#include <netinet/in.h>
#include <stdint.h>

enum dhcpv4_msg {
	DHCPV4_MSG_DISCOVER = 1,
	DHCPV4_MSG_OFFER = 2,
	DHCPV4_MSG_REQUEST = 3,
	DHCPV4_MSG_DECLINE = 4,
	DHCPV4_MSG_ACK = 5,
	DHCPV4_MSG_NAK = 6,
	DHCPV4_MSG_RELEASE = 7,
	DHCPV4_MSG_INFORM = 8,
	DHCPV4_MSG_FORCERENEW = 9,
};

enum dhcpv4_opt {
	DHCPV4_OPT_PAD = 0,
	DHCPV4_OPT_NETMASK = 1,
	DHCPV4_OPT_ROUTER = 3,
	DHCPV4_OPT_DNSSERVER = 6,
	DHCPV4_OPT_DOMAIN = 15,
	DHCPV4_OPT_MTU = 26,
	DHCPV4_OPT_BROADCAST = 28,
	DHCPV4_OPT_NTPSERVER = 42,
	DHCPV4_OPT_LEASETIME = 51,
	DHCPV4_OPT_MESSAGE = 53,
	DHCPV4_OPT_SERVERID = 54,
	DHCPV4_OPT_REQOPTS = 55,
	DHCPV4_OPT_RENEW = 58,
	DHCPV4_OPT_REBIND = 59,
	DHCPV4_OPT_IPADDRESS = 50,
	DHCPV4_OPT_MSG_TYPE = 53,
	DHCPV4_OPT_HOSTNAME = 12,
	DHCPV4_OPT_REQUEST = 17,
	DHCPV4_OPT_USER_CLASS = 77,
	DHCPV4_OPT_AUTHENTICATION = 90,
	DHCPV4_OPT_SEARCH_DOMAIN = 119,
	DHCPV4_OPT_FORCERENEW_NONCE_CAPABLE = 145,
	DHCPV4_OPT_END = 255,
};

struct dhcpv4_message {
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	struct in_addr ciaddr;
	struct in_addr yiaddr;
	struct in_addr siaddr;
	struct in_addr giaddr;
	uint8_t chaddr[16];
	char sname[64];
	char file[128];
	uint32_t magic;
	uint8_t options[];
} __attribute__((packed));

#define DHCPV4_MAGIC 0x63825363

enum dhcpv6_opt {
	DHCPV6_MSG_SOLICIT = 1,
	DHCPV6_MSG_ADVERTISE = 2,
	DHCPV6_MSG_REQUEST = 3,
	DHCPV6_MSG_CONFIRM = 4,
	DHCPV6_MSG_RENEW = 5,
	DHCPV6_MSG_REBIND = 6,
	DHCPV6_MSG_REPLY = 7,
	DHCPV6_MSG_RELEASE = 8,
	DHCPV6_MSG_DECLINE = 9,
	DHCPV6_MSG_RECONFIGURE = 10,
	DHCPV6_MSG_INFORMATION_REQUEST = 11,
	DHCPV6_MSG_RELAY_FORW = 12,
	DHCPV6_MSG_RELAY_REPL = 13,
};
struct dhcpv6_message {
	uint8_t msg_type;
	uint8_t transaction_id[3];
	uint8_t options[];
} __attribute__((packed));

#endif
