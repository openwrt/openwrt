// SPDX-License-Identifier: GPL-2.0
/*
 * Realtek RTL838x, RTL839x, RTL930x & RTL931x SerDes PHY driver
 * Copyright (c) 2024 Markus Stockhausen <markus.stockhausen@gmx.de>
 */

#ifndef _PHY_RTL_OTTO_SERDES_H
#define _PHY_RTL_OTTO_SERDES_H

#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>

#define DEFINE_SHOW_STORE_ATTRIBUTE(__name)				\
static int __name ## _open(struct inode *inode, struct file *file)	\
{									\
	return single_open(file, __name ## _show, inode->i_private);	\
}									\
									\
static const struct file_operations __name ## _fops = {			\
	.owner		= THIS_MODULE,					\
	.open		= __name ## _open,				\
	.read		= seq_read,					\
	.write		= __name ## _write,				\
	.llseek		= seq_lseek,					\
	.release	= single_release,				\
}

#define RTSDS_PAGE_SDS				0
#define RTSDS_PAGE_SDS_EXT			1
#define RTSDS_PAGE_FIB				2
#define RTSDS_PAGE_FIB_EXT			3
#define RTSDS_PAGE_NAMES			48

#define RTSDS_INV_HSO				0x100
#define RTSDS_INV_HSI				0x200

#define RTSDS_EVENT_SETUP			0
#define RTSDS_EVENT_INIT			1
#define RTSDS_EVENT_POWER_ON			2
#define RTSDS_EVENT_PRE_SET_MODE		3
#define RTSDS_EVENT_POST_SET_MODE		4
#define RTSDS_EVENT_PRE_RESET			5
#define RTSDS_EVENT_POST_RESET			6
#define RTSDS_EVENT_PRE_POWER_OFF		7
#define RTSDS_EVENT_POST_POWER_OFF		8
#define RTSDS_EVENT_MAX				8

#define RTSDS_SEQ_STOP				0
#define RTSDS_SEQ_MASK				1
#define RTSDS_SEQ_WAIT				2

#define RTSDS_SWITCH_ADDR_BASE			(0xbb000000)
#define RTSDS_REG(x)				((void __iomem __force *)RTSDS_SWITCH_ADDR_BASE + (x))
#define iomask32(mask, value, addr)		iowrite32((ioread32(addr) & ~(mask)) | (value), addr)

#define RTSDS_838X_MAX_SDS			5
#define RTSDS_838X_MAX_PAGE			3
#define RTSDS_838X_SDS_MODE_SEL			RTSDS_REG(0x0028)
#define RTSDS_838X_INT_MODE_CTRL		RTSDS_REG(0x005c)

#define RTSDS_839X_MAX_SDS			13
#define RTSDS_839X_MAX_PAGE			11
#define RTSDS_839X_MAC_SERDES_IF_CTRL		RTSDS_REG(0x0008)

#define RTSDS_930X_MAX_SDS			11
#define RTSDS_930X_MAX_PAGE			63
#define RTSDS_930X_SDS_MODE_SEL_0		RTSDS_REG(0x0194)
#define RTSDS_930X_SDS_MODE_SEL_1		RTSDS_REG(0x02a0)
#define RTSDS_930X_SDS_MODE_SEL_2		RTSDS_REG(0x02a4)
#define RTSDS_930X_SDS_MODE_SEL_3		RTSDS_REG(0x0198)
#define RTSDS_930X_SDS_SUBMODE_CTRL0		RTSDS_REG(0x01cc)
#define RTSDS_930X_SDS_SUBMODE_CTRL1		RTSDS_REG(0x02d8)

#define RTSDS_931X_MAX_SDS			13
#define RTSDS_931X_MAX_PAGE			191
#define RTSDS_931X_SERDES_MODE_CTRL		RTSDS_REG(0x13cc)
#define RTSDS_931X_PS_SERDES_OFF_MODE_CTRL	RTSDS_REG(0x13f4)
#define RTSDS_931X_SDS_FORCE_SETUP		0x80

#define RTSDS_COMBOMODE(mode, submode)		(0x10000 | (mode << 8) | submode)
#define RTSDS_MODE(combomode)			((combomode >> 8) & 0xff)
#define RTSDS_SUBMODE(combomode)		(combomode & 0xff)

struct __attribute__ ((__packed__)) rtsds_seq {
	u16 action;
	u16 ports;
	u16 page;
	u16 reg;
	u16 val;
	u16 mask;
};

struct rtsds_sds {
	struct phy *phy;
	int mode;
	int link;
	int min_port;
	int max_port;
};

struct rtsds_ctrl {
	struct device *dev;
	void __iomem *base;
	struct mutex lock;
	u32 sds_mask;
	struct rtsds_conf *conf;
	struct rtsds_sds sds[RTSDS_930X_MAX_SDS + 1];
	struct rtsds_seq *sequence[RTSDS_EVENT_MAX + 1];
};

struct rtsds_macro {
	struct rtsds_ctrl *ctrl;
	u32 sid;
};

struct rtsds_conf {
	u32 max_sds;
	u32 max_page;
	int (*read)(struct rtsds_ctrl *ctrl, u32 idx, u32 page, u32 reg);
	int (*mask)(struct rtsds_ctrl *ctrl, u32 idx, u32 page, u32 reg, u32 val, u32 mask);
	int (*reset)(struct rtsds_ctrl *ctrl, u32 idx);
	int (*set_mode)(struct rtsds_ctrl *ctrl, u32 idx, int mode);
	int (*get_mode)(struct rtsds_ctrl *ctrl, u32 idx);
	int mode_map[PHY_INTERFACE_MODE_MAX];
};

/*
 * This SerDes module should be written in quite a clean way so that direct calls are
 * not needed. The following functions are provided just in case ...
 */

int rtsds_read(struct phy *phy, u32 page, u32 reg);
int rtsds_write(struct phy *phy, u32 page, u32 reg, u32 val);
int rtsds_mask(struct phy *phy, u32 page, u32 reg, u32 val, u32 mask);

#endif /* _PHY_RTL_OTTO_SERDES_H */