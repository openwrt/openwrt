/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LINUX_RTL838X_ETH_H
#define _LINUX_RTL838X_ETH_H

#include <linux/bits.h>
#include <linux/types.h>

struct net_device;
struct sk_buff;

enum rtl838x_eth_device_talk_mode {
	RTL838X_ETH_DEVICE_TALK_ONE_HOP,
	RTL838X_ETH_DEVICE_TALK_UNICAST,
};

#define RTL838X_ETH_DEVICE_TALK_METADATA	BIT(31)
#define RTL838X_ETH_DEVICE_TALK_SOURCE_DEVICE	GENMASK(11, 8)
#define RTL838X_ETH_DEVICE_TALK_INGRESS_PORT	GENMASK(5, 0)

/* Caller must hold RTNL and stop all transmit queues before draining. */
int rtl838x_eth_tx_drain(struct net_device *dev);

/* Send a complete 802.1 Local Experimental 1 frame; consumes skb always. */
int rtl838x_eth_device_talk_xmit(struct net_device *dev, struct sk_buff *skb,
				 enum rtl838x_eth_device_talk_mode mode,
				 u8 target);

/* Send a frame to a physical port on a stacked RTL931x; consumes skb always. */
int rtl838x_eth_stack_port_xmit(struct net_device *dev, struct sk_buff *skb,
				u8 device, u8 port);

#endif /* _LINUX_RTL838X_ETH_H */
