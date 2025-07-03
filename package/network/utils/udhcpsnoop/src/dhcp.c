// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */

#include "dhcpsnoop.h"
#include "msg.h"

const char *dhcpsnoop_parse_ipv4(const void *buf, size_t len, uint16_t port, uint32_t *expire)
{
	const struct dhcpv4_message *msg = buf;
	const uint8_t *pos, *end;
	uint32_t leasetime = 0, rebind = 0, renew = 0;
	char type = 0;

	if (port != 67 && port != 68)
		return NULL;

	if (len < sizeof(*msg))
		return NULL;

	if (ntohl(msg->magic) != DHCPV4_MAGIC)
		return NULL;

	pos = msg->options;
	end = buf + len;

	while (pos < end) {
		const uint8_t *opt = pos++;

		if (*opt == DHCPV4_OPT_END)
			break;

		if (*opt == DHCPV4_OPT_PAD)
			continue;

		if (pos >= end || 1 + *pos > end - pos)
			break;

		pos += *pos + 1;
		if (pos >= end)
			break;

		switch (*opt) {
		case DHCPV4_OPT_MSG_TYPE:
			if (!opt[1])
				continue;
			type = opt[2];
			break;
		case DHCPV4_OPT_LEASETIME:
			if (opt[1] != 4)
				continue;
			leasetime = *((uint32_t *) &opt[2]);
			break;
		case DHCPV4_OPT_REBIND:
			if (opt[1] != 4)
				continue;
			rebind = *((uint32_t *) &opt[2]);
			break;
		case DHCPV4_OPT_RENEW:
			if (opt[1] != 4)
				continue;
			renew = *((uint32_t *) &opt[2]);
			break;
		}
	}

	if (renew)
		*expire = renew;
	else if (rebind)
		*expire = rebind;
	else if (leasetime)
		*expire = leasetime;
	else
		*expire = 24 * 60 * 60;
	*expire = ntohl(*expire);

	switch(type) {
	case DHCPV4_MSG_ACK:
		return "ack";
	case DHCPV4_MSG_DISCOVER:
		return "discover";
	case DHCPV4_MSG_OFFER:
		return "offer";
	case DHCPV4_MSG_REQUEST:
		return "request";
	}

	return NULL;
}

const char *dhcpsnoop_parse_ipv6(const void *buf, size_t len, uint16_t port)
{
	const struct dhcpv6_message *msg = buf;

	if (port != 546 && port != 547)
		return NULL;

	switch(msg->msg_type) {
	case DHCPV6_MSG_SOLICIT:
		return "solicit";
	case DHCPV6_MSG_REPLY:
		return "reply";
	case DHCPV6_MSG_RENEW:
		return "renew";
	default:
		return NULL;
	}
}


