/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LINUX_RTL838X_ETH_H
#define _LINUX_RTL838X_ETH_H

struct net_device;

/* Caller must hold RTNL and stop all transmit queues before draining. */
int rtl838x_eth_tx_drain(struct net_device *dev);

#endif /* _LINUX_RTL838X_ETH_H */
