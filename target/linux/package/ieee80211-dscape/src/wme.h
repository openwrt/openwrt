/*
 * IEEE 802.11 driver (80211.o) - QoS datatypes
 * Copyright 2004, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <asm/byteorder.h>
#include <net/pkt_sched.h>

#define QOS_CONTROL_LEN 2

#define QOS_CONTROL_ACK_POLICY_NORMAL 0
#define QOS_CONTROL_ACK_POLICY_NOACK 1

#define QOS_CONTROL_TID_MASK 0x0f
#define QOS_CONTROL_ACK_POLICY_SHIFT 5

/* This bit field structure should not be used; it can cause compiler to
 * generate unaligned accesses and inefficient code. */
struct qos_control {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	u8	tag1d:3, /* bits 0-2 */
  		reserved1:1,
		eosp:1,
		ack_policy:2,
  		reserved2:1;
#elif defined (__BIG_ENDIAN_BITFIELD)
	u8	reserved2:1,
		ack_policy:2,
		eosp:1,
		reserved1:1,
		tag1d:3; /* bits 0-2 */
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	u8 reserved;
} __attribute__ ((packed));

ieee80211_txrx_result
ieee80211_rx_h_parse_qos(struct ieee80211_txrx_data *rx);

ieee80211_txrx_result
ieee80211_rx_h_remove_qos_control(struct ieee80211_txrx_data *rx);

void ieee80211_install_qdisc(struct net_device *dev);

int ieee80211_wme_register(void);
void ieee80211_wme_unregister(void);

