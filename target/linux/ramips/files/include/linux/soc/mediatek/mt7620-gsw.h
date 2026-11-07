/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SOC_MEDIATEK_MT7620_GSW_H
#define __LINUX_SOC_MEDIATEK_MT7620_GSW_H

#include <linux/bits.h>
#include <linux/errno.h>
#include <linux/kconfig.h>
#include <linux/types.h>

#define MT7620_GSW_PFC			0x0004
#define MT7620_GSW_PFC_PPE_PORT		GENMASK(2, 0)
#define MT7620_GSW_PFC_PPE_ENABLE	BIT(3)
#define MT7620_GSW_PPE_PORT		7
#define MT7620_GSW_PSC(port)		(0x200c + ((port) * 0x100))
#define MT7620_GSW_TPF(port)		(0x2030 + ((port) * 0x100))
#define MT7620_GSW_TPF_IPV4_MYUC	BIT(0)
#define MT7620_GSW_TPF_IPV4_UC		BIT(4)
#define MT7620_GSW_TPF_IPV4_UN		BIT(5)
#define MT7620_GSW_TPF_IPV6_MYUC	BIT(8)
#define MT7620_GSW_TPF_IPV6_UC		BIT(12)
#define MT7620_GSW_TPF_IPV6_UN		BIT(13)
#define MT7620_GSW_TPF_PPE_MASK		(MT7620_GSW_TPF_IPV4_MYUC | \
					 MT7620_GSW_TPF_IPV4_UC | \
					 MT7620_GSW_TPF_IPV4_UN | \
					 MT7620_GSW_TPF_IPV6_MYUC | \
					 MT7620_GSW_TPF_IPV6_UC | \
					 MT7620_GSW_TPF_IPV6_UN)
#define MT7620_GSW_LAST_USER_PORT	4

struct device;
struct net_device;
struct mt7620_gsw;

#if IS_REACHABLE(CONFIG_NET_RALINK_GSW_MT7620)
int mt7620_gsw_upstream_init(struct device *parent, struct net_device *conduit);
void mt7620_gsw_upstream_cleanup(struct device *parent);
struct mt7620_gsw *mt7620_gsw_upstream_get(struct device *parent);
void mt7620_gsw_upstream_put(struct mt7620_gsw *gsw);
void mt7620_gsw_reg_lock(struct mt7620_gsw *gsw);
void mt7620_gsw_reg_unlock(struct mt7620_gsw *gsw);
u32 mt7620_gsw_reg_read(struct mt7620_gsw *gsw, unsigned int reg);
void mt7620_gsw_reg_write(struct mt7620_gsw *gsw, u32 val,
			  unsigned int reg);
void mt7620_gsw_port_set_mtu(struct mt7620_gsw *gsw, unsigned int port,
			     unsigned int mtu);
void mt7620_gsw_ppe_port_set(struct mt7620_gsw *gsw, unsigned int port,
			     bool enable);
#else
static inline int
mt7620_gsw_upstream_init(struct device *parent, struct net_device *conduit)
{
	return -EOPNOTSUPP;
}

static inline void mt7620_gsw_upstream_cleanup(struct device *parent)
{
}

static inline struct mt7620_gsw *
mt7620_gsw_upstream_get(struct device *parent)
{
	return NULL;
}

static inline void mt7620_gsw_upstream_put(struct mt7620_gsw *gsw)
{
}

static inline void mt7620_gsw_reg_lock(struct mt7620_gsw *gsw)
{
}

static inline void mt7620_gsw_reg_unlock(struct mt7620_gsw *gsw)
{
}

static inline u32 mt7620_gsw_reg_read(struct mt7620_gsw *gsw,
				      unsigned int reg)
{
	return 0;
}

static inline void mt7620_gsw_reg_write(struct mt7620_gsw *gsw, u32 val,
					unsigned int reg)
{
}

static inline void mt7620_gsw_port_set_mtu(struct mt7620_gsw *gsw,
					   unsigned int port,
					   unsigned int mtu)
{
}

static inline void mt7620_gsw_ppe_port_set(struct mt7620_gsw *gsw,
					   unsigned int port,
					   bool enable)
{
}
#endif

#endif
