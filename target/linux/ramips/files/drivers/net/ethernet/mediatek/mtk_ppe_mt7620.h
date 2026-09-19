/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _MTK_PPE_MT7620_H_
#define _MTK_PPE_MT7620_H_

#include <linux/netdevice.h>

struct mtk_eth;

#if IS_ENABLED(CONFIG_NET_MEDIATEK_MT7620_PPE)
int mt7620_ppe_init(struct mtk_eth *eth);
void mt7620_ppe_deinit(struct mtk_eth *eth);
void mt7620_ppe_start(struct mtk_eth *eth);
void mt7620_ppe_stop(struct mtk_eth *eth);
void mt7620_ppe_update_mtu(struct mtk_eth *eth);
void mt7620_ppe_rx_process(struct mtk_eth *eth, u32 rxd4);
int mt7620_ppe_setup_tc(struct net_device *dev, enum tc_setup_type type,
			void *type_data);
#else
static inline int mt7620_ppe_init(struct mtk_eth *eth)
{
	return 0;
}

static inline void mt7620_ppe_deinit(struct mtk_eth *eth)
{
}

static inline void mt7620_ppe_start(struct mtk_eth *eth)
{
}

static inline void mt7620_ppe_stop(struct mtk_eth *eth)
{
}

static inline void mt7620_ppe_update_mtu(struct mtk_eth *eth)
{
}

static inline void mt7620_ppe_rx_process(struct mtk_eth *eth, u32 rxd4)
{
}

static inline int
mt7620_ppe_setup_tc(struct net_device *dev,
		    enum tc_setup_type type, void *type_data)
{
	return -EOPNOTSUPP;
}
#endif

#endif
