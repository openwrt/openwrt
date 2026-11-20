/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SOC_MEDIATEK_MT7620_GSW_H
#define __LINUX_SOC_MEDIATEK_MT7620_GSW_H

#include <linux/errno.h>
#include <linux/kconfig.h>

struct device;
struct net_device;

#if IS_REACHABLE(CONFIG_NET_RALINK_GSW_MT7620)
int mt7620_gsw_upstream_init(struct device *parent, struct net_device *conduit);
void mt7620_gsw_upstream_cleanup(struct device *parent);
#else
static inline int
mt7620_gsw_upstream_init(struct device *parent, struct net_device *conduit)
{
	return -EOPNOTSUPP;
}

static inline void mt7620_gsw_upstream_cleanup(struct device *parent)
{
}
#endif

#endif
