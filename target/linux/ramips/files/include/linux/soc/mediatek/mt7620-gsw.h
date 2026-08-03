/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SOC_MEDIATEK_MT7620_GSW_H
#define __LINUX_SOC_MEDIATEK_MT7620_GSW_H

#include <linux/errno.h>
#include <linux/kconfig.h>
#include <linux/types.h>

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
#endif

#endif
