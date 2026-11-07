/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SOC_MEDIATEK_MT7620_PPE_H
#define __LINUX_SOC_MEDIATEK_MT7620_PPE_H

#include <linux/errno.h>
#include <linux/kconfig.h>

struct flow_cls_offload;
struct net_device;

#if IS_REACHABLE(CONFIG_NET_MEDIATEK_MT7620_PPE)
int mt7620_ppe_setup_dsa_tc(struct net_device *conduit,
			    struct net_device *user,
			    struct flow_cls_offload *cls);
#else
static inline int
mt7620_ppe_setup_dsa_tc(struct net_device *conduit, struct net_device *user,
			struct flow_cls_offload *cls)
{
	return -EOPNOTSUPP;
}
#endif

#endif
