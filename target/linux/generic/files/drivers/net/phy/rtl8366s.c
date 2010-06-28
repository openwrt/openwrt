/*
 * Platform driver for the Realtek RTL8366S ethernet switch
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2010 Antti Seppälä <a.seppala@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/skbuff.h>
#include <linux/switch.h>
#include <linux/rtl8366s.h>

#include "rtl8366_smi.h"

#ifdef CONFIG_RTL8366S_PHY_DEBUG_FS
#include <linux/debugfs.h>
#endif

#define RTL8366S_DRIVER_DESC	"Realtek RTL8366S ethernet switch driver"
#define RTL8366S_DRIVER_VER	"0.2.2"

#define RTL8366S_PHY_NO_MAX	4
#define RTL8366S_PHY_PAGE_MAX	7
#define RTL8366S_PHY_ADDR_MAX	31

#define RTL8366S_CHIP_GLOBAL_CTRL_REG		0x0000
#define RTL8366S_CHIP_CTRL_VLAN			(1 << 13)

/* Switch Global Configuration register */
#define RTL8366S_SGCR				0x0000
#define RTL8366S_SGCR_EN_BC_STORM_CTRL		BIT(0)
#define RTL8366S_SGCR_MAX_LENGTH(_x)		(_x << 4)
#define RTL8366S_SGCR_MAX_LENGTH_MASK		RTL8366S_SGCR_MAX_LENGTH(0x3)
#define RTL8366S_SGCR_MAX_LENGTH_1522		RTL8366S_SGCR_MAX_LENGTH(0x0)
#define RTL8366S_SGCR_MAX_LENGTH_1536		RTL8366S_SGCR_MAX_LENGTH(0x1)
#define RTL8366S_SGCR_MAX_LENGTH_1552		RTL8366S_SGCR_MAX_LENGTH(0x2)
#define RTL8366S_SGCR_MAX_LENGTH_16000		RTL8366S_SGCR_MAX_LENGTH(0x3)

/* Port Enable Control register */
#define RTL8366S_PECR				0x0001

/* Switch Security Control registers */
#define RTL8366S_SSCR0				0x0002
#define RTL8366S_SSCR1				0x0003
#define RTL8366S_SSCR2				0x0004
#define RTL8366S_SSCR2_DROP_UNKNOWN_DA		BIT(0)

#define RTL8366S_RESET_CTRL_REG			0x0100
#define RTL8366S_CHIP_CTRL_RESET_HW		1
#define RTL8366S_CHIP_CTRL_RESET_SW		(1 << 1)

#define RTL8366S_CHIP_VERSION_CTRL_REG		0x0104
#define RTL8366S_CHIP_VERSION_MASK		0xf
#define RTL8366S_CHIP_ID_REG			0x0105
#define RTL8366S_CHIP_ID_8366			0x8366

/* PHY registers control */
#define RTL8366S_PHY_ACCESS_CTRL_REG		0x8028
#define RTL8366S_PHY_ACCESS_DATA_REG		0x8029

#define RTL8366S_PHY_CTRL_READ			1
#define RTL8366S_PHY_CTRL_WRITE			0

#define RTL8366S_PHY_REG_MASK			0x1f
#define RTL8366S_PHY_PAGE_OFFSET		5
#define RTL8366S_PHY_PAGE_MASK			(0x7 << 5)
#define RTL8366S_PHY_NO_OFFSET			9
#define RTL8366S_PHY_NO_MASK			(0x1f << 9)

/* LED control registers */
#define RTL8366S_LED_BLINKRATE_REG		0x0420
#define RTL8366S_LED_BLINKRATE_BIT		0
#define RTL8366S_LED_BLINKRATE_MASK		0x0007

#define RTL8366S_LED_CTRL_REG			0x0421
#define RTL8366S_LED_0_1_CTRL_REG		0x0422
#define RTL8366S_LED_2_3_CTRL_REG		0x0423

#define RTL8366S_MIB_COUNT			33
#define RTL8366S_GLOBAL_MIB_COUNT		1
#define RTL8366S_MIB_COUNTER_PORT_OFFSET	0x0040
#define RTL8366S_MIB_COUNTER_BASE		0x1000
#define RTL8366S_MIB_COUNTER_PORT_OFFSET2	0x0008
#define RTL8366S_MIB_COUNTER_BASE2		0x1180
#define RTL8366S_MIB_CTRL_REG			0x11F0
#define RTL8366S_MIB_CTRL_USER_MASK		0x01FF
#define RTL8366S_MIB_CTRL_BUSY_MASK		0x0001
#define RTL8366S_MIB_CTRL_RESET_MASK		0x0002

#define RTL8366S_MIB_CTRL_GLOBAL_RESET_MASK	0x0004
#define RTL8366S_MIB_CTRL_PORT_RESET_BIT	0x0003
#define RTL8366S_MIB_CTRL_PORT_RESET_MASK	0x01FC


#define RTL8366S_PORT_VLAN_CTRL_BASE		0x0058
#define RTL8366S_PORT_VLAN_CTRL_REG(_p)  \
		(RTL8366S_PORT_VLAN_CTRL_BASE + (_p) / 4)
#define RTL8366S_PORT_VLAN_CTRL_MASK		0xf
#define RTL8366S_PORT_VLAN_CTRL_SHIFT(_p)	(4 * ((_p) % 4))


#define RTL8366S_VLAN_TABLE_READ_BASE		0x018B
#define RTL8366S_VLAN_TABLE_WRITE_BASE		0x0185

#define RTL8366S_VLAN_TB_CTRL_REG		0x010F

#define RTL8366S_TABLE_ACCESS_CTRL_REG		0x0180
#define RTL8366S_TABLE_VLAN_READ_CTRL		0x0E01
#define RTL8366S_TABLE_VLAN_WRITE_CTRL		0x0F01

#define RTL8366S_VLAN_MEMCONF_BASE		0x0016


#define RTL8366S_PORT_LINK_STATUS_BASE		0x0060
#define RTL8366S_PORT_STATUS_SPEED_MASK		0x0003
#define RTL8366S_PORT_STATUS_DUPLEX_MASK	0x0004
#define RTL8366S_PORT_STATUS_LINK_MASK		0x0010
#define RTL8366S_PORT_STATUS_TXPAUSE_MASK	0x0020
#define RTL8366S_PORT_STATUS_RXPAUSE_MASK	0x0040
#define RTL8366S_PORT_STATUS_AN_MASK		0x0080


#define RTL8366S_PORT_NUM_CPU		5
#define RTL8366S_NUM_PORTS		6
#define RTL8366S_NUM_VLANS		16
#define RTL8366S_NUM_LEDGROUPS		4
#define RTL8366S_NUM_VIDS		4096
#define RTL8366S_PRIORITYMAX		7
#define RTL8366S_FIDMAX			7


#define RTL8366S_PORT_1			(1 << 0) /* In userspace port 0 */
#define RTL8366S_PORT_2			(1 << 1) /* In userspace port 1 */
#define RTL8366S_PORT_3			(1 << 2) /* In userspace port 2 */
#define RTL8366S_PORT_4			(1 << 3) /* In userspace port 3 */

#define RTL8366S_PORT_UNKNOWN		(1 << 4) /* No known connection */
#define RTL8366S_PORT_CPU		(1 << 5) /* CPU port */

#define RTL8366S_PORT_ALL		(RTL8366S_PORT_1 |	\
					 RTL8366S_PORT_2 |	\
					 RTL8366S_PORT_3 |	\
					 RTL8366S_PORT_4 |	\
					 RTL8366S_PORT_UNKNOWN | \
					 RTL8366S_PORT_CPU)

#define RTL8366S_PORT_ALL_BUT_CPU	(RTL8366S_PORT_1 |	\
					 RTL8366S_PORT_2 |	\
					 RTL8366S_PORT_3 |	\
					 RTL8366S_PORT_4 |	\
					 RTL8366S_PORT_UNKNOWN)

#define RTL8366S_PORT_ALL_EXTERNAL	(RTL8366S_PORT_1 |	\
					 RTL8366S_PORT_2 |	\
					 RTL8366S_PORT_3 |	\
					 RTL8366S_PORT_4)

#define RTL8366S_PORT_ALL_INTERNAL	(RTL8366S_PORT_UNKNOWN | \
					 RTL8366S_PORT_CPU)

struct rtl8366s {
	struct device		*parent;
	struct rtl8366_smi	smi;
	struct switch_dev	dev;
	char			buf[4096];
#ifdef CONFIG_RTL8366S_PHY_DEBUG_FS
	struct dentry           *debugfs_root;
#endif
};

struct rtl8366s_vlan_mc {
	u16 	reserved2:1;
	u16 	priority:3;
	u16 	vid:12;

	u16 	reserved1:1;
	u16 	fid:3;
	u16 	untag:6;
	u16 	member:6;
};

struct rtl8366s_vlan_4k {
	u16 	reserved1:4;
	u16 	vid:12;

	u16 	reserved2:1;
	u16 	fid:3;
	u16 	untag:6;
	u16 	member:6;
};

#ifdef CONFIG_RTL8366S_PHY_DEBUG_FS
u16 g_dbg_reg;
#endif

struct mib_counter {
	unsigned	base;
	unsigned	offset;
	unsigned	length;
	const char	*name;
};

static struct mib_counter rtl8366s_mib_counters[RTL8366S_MIB_COUNT] = {
	{ 0,  0, 4, "IfInOctets"				},
	{ 0,  4, 4, "EtherStatsOctets"				},
	{ 0,  8, 2, "EtherStatsUnderSizePkts"			},
	{ 0, 10, 2, "EtherFragments"				},
	{ 0, 12, 2, "EtherStatsPkts64Octets"			},
	{ 0, 14, 2, "EtherStatsPkts65to127Octets"		},
	{ 0, 16, 2, "EtherStatsPkts128to255Octets"		},
	{ 0, 18, 2, "EtherStatsPkts256to511Octets"		},
	{ 0, 20, 2, "EtherStatsPkts512to1023Octets"		},
	{ 0, 22, 2, "EtherStatsPkts1024to1518Octets"		},
	{ 0, 24, 2, "EtherOversizeStats"			},
	{ 0, 26, 2, "EtherStatsJabbers"				},
	{ 0, 28, 2, "IfInUcastPkts"				},
	{ 0, 30, 2, "EtherStatsMulticastPkts"			},
	{ 0, 32, 2, "EtherStatsBroadcastPkts"			},
	{ 0, 34, 2, "EtherStatsDropEvents"			},
	{ 0, 36, 2, "Dot3StatsFCSErrors"			},
	{ 0, 38, 2, "Dot3StatsSymbolErrors"			},
	{ 0, 40, 2, "Dot3InPauseFrames"				},
	{ 0, 42, 2, "Dot3ControlInUnknownOpcodes"		},
	{ 0, 44, 4, "IfOutOctets"				},
	{ 0, 48, 2, "Dot3StatsSingleCollisionFrames"		},
	{ 0, 50, 2, "Dot3StatMultipleCollisionFrames"		},
	{ 0, 52, 2, "Dot3sDeferredTransmissions"		},
	{ 0, 54, 2, "Dot3StatsLateCollisions"			},
	{ 0, 56, 2, "EtherStatsCollisions"			},
	{ 0, 58, 2, "Dot3StatsExcessiveCollisions"		},
	{ 0, 60, 2, "Dot3OutPauseFrames"			},
	{ 0, 62, 2, "Dot1dBasePortDelayExceededDiscards"	},

	/*
	 * The following counters are accessible at a different
	 * base address.
	 */
	{ 1,  0, 2, "Dot1dTpPortInDiscards"			},
	{ 1,  2, 2, "IfOutUcastPkts"				},
	{ 1,  4, 2, "IfOutMulticastPkts"			},
	{ 1,  6, 2, "IfOutBroadcastPkts"			},
};

#define REG_WR(_smi, _reg, _val)					\
	do {								\
		err = rtl8366_smi_write_reg(_smi, _reg, _val);		\
		if (err)						\
			return err;					\
	} while (0)

#define REG_RMW(_smi, _reg, _mask, _val)				\
	do {								\
		err = rtl8366_smi_rmwr(_smi, _reg, _mask, _val);	\
		if (err)						\
			return err;					\
	} while (0)

static inline struct rtl8366s *smi_to_rtl8366s(struct rtl8366_smi *smi)
{
	return container_of(smi, struct rtl8366s, smi);
}

static inline struct rtl8366s *sw_to_rtl8366s(struct switch_dev *sw)
{
	return container_of(sw, struct rtl8366s, dev);
}

static inline struct rtl8366_smi *sw_to_rtl8366_smi(struct switch_dev *sw)
{
	struct rtl8366s *rtl = sw_to_rtl8366s(sw);
	return &rtl->smi;
}

static int rtl8366s_reset_chip(struct rtl8366_smi *smi)
{
	int timeout = 10;
	u32 data;

	rtl8366_smi_write_reg(smi, RTL8366S_RESET_CTRL_REG,
			      RTL8366S_CHIP_CTRL_RESET_HW);
	do {
		msleep(1);
		if (rtl8366_smi_read_reg(smi, RTL8366S_RESET_CTRL_REG, &data))
			return -EIO;

		if (!(data & RTL8366S_CHIP_CTRL_RESET_HW))
			break;
	} while (--timeout);

	if (!timeout) {
		printk("Timeout waiting for the switch to reset\n");
		return -EIO;
	}

	return 0;
}

static int rtl8366s_hw_init(struct rtl8366_smi *smi)
{
	int err;

	/* set maximum packet length to 1536 bytes */
	REG_RMW(smi, RTL8366S_SGCR, RTL8366S_SGCR_MAX_LENGTH_MASK,
		RTL8366S_SGCR_MAX_LENGTH_1536);

	/* enable all ports */
	REG_WR(smi, RTL8366S_PECR, 0);

	/* disable learning for all ports */
	REG_WR(smi, RTL8366S_SSCR0, RTL8366S_PORT_ALL);

	/* disable auto ageing for all ports */
	REG_WR(smi, RTL8366S_SSCR1, RTL8366S_PORT_ALL);

	/* don't drop packets whose DA has not been learned */
	REG_RMW(smi, RTL8366S_SSCR2, RTL8366S_SSCR2_DROP_UNKNOWN_DA, 0);

	return 0;
}

static int rtl8366s_read_phy_reg(struct rtl8366_smi *smi,
				 u32 phy_no, u32 page, u32 addr, u32 *data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366S_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366S_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366S_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366S_PHY_ACCESS_CTRL_REG,
				    RTL8366S_PHY_CTRL_READ);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366S_PHY_NO_OFFSET)) |
	      ((page << RTL8366S_PHY_PAGE_OFFSET) & RTL8366S_PHY_PAGE_MASK) |
	      (addr & RTL8366S_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, 0);
	if (ret)
		return ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366S_PHY_ACCESS_DATA_REG, data);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366s_write_phy_reg(struct rtl8366_smi *smi,
				  u32 phy_no, u32 page, u32 addr, u32 data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366S_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366S_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366S_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366S_PHY_ACCESS_CTRL_REG,
				    RTL8366S_PHY_CTRL_WRITE);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366S_PHY_NO_OFFSET)) |
	      ((page << RTL8366S_PHY_PAGE_OFFSET) & RTL8366S_PHY_PAGE_MASK) |
	      (addr & RTL8366S_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, data);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366_get_mib_counter(struct rtl8366_smi *smi, int counter,
				   int port, unsigned long long *val)
{
	int i;
	int err;
	u32 addr, data;
	u64 mibvalue;

	if (port > RTL8366S_NUM_PORTS || counter >= RTL8366S_MIB_COUNT)
		return -EINVAL;

	switch (rtl8366s_mib_counters[counter].base) {
	case 0:
		addr = RTL8366S_MIB_COUNTER_BASE +
		       RTL8366S_MIB_COUNTER_PORT_OFFSET * port;
		break;

	case 1:
		addr = RTL8366S_MIB_COUNTER_BASE2 +
			RTL8366S_MIB_COUNTER_PORT_OFFSET2 * port;
		break;

	default:
		return -EINVAL;
	}

	addr += rtl8366s_mib_counters[counter].offset;

	/*
	 * Writing access counter address first
	 * then ASIC will prepare 64bits counter wait for being retrived
	 */
	data = 0; /* writing data will be discard by ASIC */
	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	/* read MIB control register */
	err =  rtl8366_smi_read_reg(smi, RTL8366S_MIB_CTRL_REG, &data);
	if (err)
		return err;

	if (data & RTL8366S_MIB_CTRL_BUSY_MASK)
		return -EBUSY;

	if (data & RTL8366S_MIB_CTRL_RESET_MASK)
		return -EIO;

	mibvalue = 0;
	for (i = rtl8366s_mib_counters[counter].length; i > 0; i--) {
		err = rtl8366_smi_read_reg(smi, addr + (i - 1), &data);
		if (err)
			return err;

		mibvalue = (mibvalue << 16) | (data & 0xFFFF);
	}

	*val = mibvalue;
	return 0;
}

static int rtl8366s_get_vlan_4k(struct rtl8366_smi *smi, u32 vid,
				struct rtl8366_vlan_4k *vlan4k)
{
	struct rtl8366s_vlan_4k vlan4k_priv;
	int err;
	u32 data;
	u16 *tableaddr;

	memset(vlan4k, '\0', sizeof(struct rtl8366_vlan_4k));
	vlan4k_priv.vid = vid;

	if (vid >= RTL8366S_NUM_VIDS)
		return -EINVAL;

	tableaddr = (u16 *)&vlan4k_priv;

	/* write VID */
	data = *tableaddr;
	err = rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TABLE_WRITE_BASE, data);
	if (err)
		return err;

	/* write table access control word */
	err = rtl8366_smi_write_reg(smi, RTL8366S_TABLE_ACCESS_CTRL_REG,
				    RTL8366S_TABLE_VLAN_READ_CTRL);
	if (err)
		return err;

	err = rtl8366_smi_read_reg(smi, RTL8366S_VLAN_TABLE_READ_BASE, &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	err = rtl8366_smi_read_reg(smi, RTL8366S_VLAN_TABLE_READ_BASE + 1,
				   &data);
	if (err)
		return err;

	*tableaddr = data;

	vlan4k->vid = vid;
	vlan4k->untag = vlan4k_priv.untag;
	vlan4k->member = vlan4k_priv.member;
	vlan4k->fid = vlan4k_priv.fid;

	return 0;
}

static int rtl8366s_set_vlan_4k(struct rtl8366_smi *smi,
				const struct rtl8366_vlan_4k *vlan4k)
{
	struct rtl8366s_vlan_4k vlan4k_priv;
	int err;
	u32 data;
	u16 *tableaddr;

	if (vlan4k->vid >= RTL8366S_NUM_VIDS ||
	    vlan4k->member > RTL8366S_PORT_ALL ||
	    vlan4k->untag > RTL8366S_PORT_ALL ||
	    vlan4k->fid > RTL8366S_FIDMAX)
		return -EINVAL;

	vlan4k_priv.vid = vlan4k->vid;
	vlan4k_priv.untag = vlan4k->untag;
	vlan4k_priv.member = vlan4k->member;
	vlan4k_priv.fid = vlan4k->fid;

	tableaddr = (u16 *)&vlan4k_priv;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TABLE_WRITE_BASE, data);
	if (err)
		return err;

	tableaddr++;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TABLE_WRITE_BASE + 1,
				    data);
	if (err)
		return err;

	/* write table access control word */
	err = rtl8366_smi_write_reg(smi, RTL8366S_TABLE_ACCESS_CTRL_REG,
				    RTL8366S_TABLE_VLAN_WRITE_CTRL);

	return err;
}

static int rtl8366s_get_vlan_mc(struct rtl8366_smi *smi, u32 index,
				struct rtl8366_vlan_mc *vlanmc)
{
	struct rtl8366s_vlan_mc vlanmc_priv;
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	memset(vlanmc, '\0', sizeof(struct rtl8366_vlan_mc));

	if (index >= RTL8366S_NUM_VLANS)
		return -EINVAL;

	tableaddr = (u16 *)&vlanmc_priv;

	addr = RTL8366S_VLAN_MEMCONF_BASE + (index << 1);
	err = rtl8366_smi_read_reg(smi, addr, &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	addr = RTL8366S_VLAN_MEMCONF_BASE + 1 + (index << 1);
	err = rtl8366_smi_read_reg(smi, addr, &data);
	if (err)
		return err;

	*tableaddr = data;

	vlanmc->vid = vlanmc_priv.vid;
	vlanmc->priority = vlanmc_priv.priority;
	vlanmc->untag = vlanmc_priv.untag;
	vlanmc->member = vlanmc_priv.member;
	vlanmc->fid = vlanmc_priv.fid;

	return 0;
}

static int rtl8366s_set_vlan_mc(struct rtl8366_smi *smi, u32 index,
				const struct rtl8366_vlan_mc *vlanmc)
{
	struct rtl8366s_vlan_mc vlanmc_priv;
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	if (index >= RTL8366S_NUM_VLANS ||
	    vlanmc->vid >= RTL8366S_NUM_VIDS ||
	    vlanmc->priority > RTL8366S_PRIORITYMAX ||
	    vlanmc->member > RTL8366S_PORT_ALL ||
	    vlanmc->untag > RTL8366S_PORT_ALL ||
	    vlanmc->fid > RTL8366S_FIDMAX)
		return -EINVAL;

	vlanmc_priv.vid = vlanmc->vid;
	vlanmc_priv.priority = vlanmc->priority;
	vlanmc_priv.untag = vlanmc->untag;
	vlanmc_priv.member = vlanmc->member;
	vlanmc_priv.fid = vlanmc->fid;

	addr = RTL8366S_VLAN_MEMCONF_BASE + (index << 1);

	tableaddr = (u16 *)&vlanmc_priv;
	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	addr = RTL8366S_VLAN_MEMCONF_BASE + 1 + (index << 1);

	tableaddr++;
	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	return 0;
}

static int rtl8366s_get_mc_index(struct rtl8366_smi *smi, int port, int *val)
{
	u32 data;
	int err;

	if (port >= RTL8366S_NUM_PORTS)
		return -EINVAL;

	err = rtl8366_smi_read_reg(smi, RTL8366S_PORT_VLAN_CTRL_REG(port),
				   &data);
	if (err)
		return err;

	*val = (data >> RTL8366S_PORT_VLAN_CTRL_SHIFT(port)) &
	       RTL8366S_PORT_VLAN_CTRL_MASK;

	return 0;
}

static int rtl8366s_set_mc_index(struct rtl8366_smi *smi, int port, int index)
{
	if (port >= RTL8366S_NUM_PORTS || index >= RTL8366S_NUM_VLANS)
		return -EINVAL;

	return rtl8366_smi_rmwr(smi, RTL8366S_PORT_VLAN_CTRL_REG(port),
				RTL8366S_PORT_VLAN_CTRL_MASK <<
					RTL8366S_PORT_VLAN_CTRL_SHIFT(port),
				(index & RTL8366S_PORT_VLAN_CTRL_MASK) <<
					RTL8366S_PORT_VLAN_CTRL_SHIFT(port));
}

static int rtl8366s_set_vlan(struct rtl8366_smi *smi, int vid, u32 member,
			     u32 untag, u32 fid)
{
	struct rtl8366_vlan_4k vlan4k;
	int err;
	int i;

	/* Update the 4K table */
	err = rtl8366s_get_vlan_4k(smi, vid, &vlan4k);
	if (err)
		return err;

	vlan4k.member = member;
	vlan4k.untag = untag;
	vlan4k.fid = fid;
	err = rtl8366s_set_vlan_4k(smi, &vlan4k);
	if (err)
		return err;

	/* Try to find an existing MC entry for this VID */
	for (i = 0; i < RTL8366S_NUM_VLANS; i++) {
		struct rtl8366_vlan_mc vlanmc;

		err = rtl8366s_get_vlan_mc(smi, i, &vlanmc);
		if (err)
			return err;

		if (vid == vlanmc.vid) {
			/* update the MC entry */
			vlanmc.member = member;
			vlanmc.untag = untag;
			vlanmc.fid = fid;

			err = rtl8366s_set_vlan_mc(smi, i, &vlanmc);
			break;
		}
	}

	return err;
}

static int rtl8366s_get_pvid(struct rtl8366_smi *smi, int port, int *val)
{
	struct rtl8366_vlan_mc vlanmc;
	int err;
	int index;

	err = rtl8366s_get_mc_index(smi, port, &index);
	if (err)
		return err;

	err = rtl8366s_get_vlan_mc(smi, index, &vlanmc);
	if (err)
		return err;

	*val = vlanmc.vid;
	return 0;
}

static int rtl8366s_mc_is_used(struct rtl8366_smi *smi, int mc_index,
			       int *used)
{
	int err;
	int i;

	*used = 0;
	for (i = 0; i < RTL8366S_NUM_PORTS; i++) {
		int index = 0;

		err = rtl8366s_get_mc_index(smi, i, &index);
		if (err)
			return err;

		if (mc_index == index) {
			*used = 1;
			break;
		}
	}

	return 0;
}

static int rtl8366s_set_pvid(struct rtl8366_smi *smi, unsigned port,
			     unsigned vid)
{
	struct rtl8366_vlan_mc vlanmc;
	struct rtl8366_vlan_4k vlan4k;
	int err;
	int i;

	/* Try to find an existing MC entry for this VID */
	for (i = 0; i < RTL8366S_NUM_VLANS; i++) {
		err = rtl8366s_get_vlan_mc(smi, i, &vlanmc);
		if (err)
			return err;

		if (vid == vlanmc.vid) {
			err = rtl8366s_set_vlan_mc(smi, i, &vlanmc);
			if (err)
				return err;

			err = rtl8366s_set_mc_index(smi, port, i);
			return err;
		}
	}

	/* We have no MC entry for this VID, try to find an empty one */
	for (i = 0; i < RTL8366S_NUM_VLANS; i++) {
		err = rtl8366s_get_vlan_mc(smi, i, &vlanmc);
		if (err)
			return err;

		if (vlanmc.vid == 0 && vlanmc.member == 0) {
			/* Update the entry from the 4K table */
			err = rtl8366s_get_vlan_4k(smi, vid, &vlan4k);
			if (err)
				return err;

			vlanmc.vid = vid;
			vlanmc.member = vlan4k.member;
			vlanmc.untag = vlan4k.untag;
			vlanmc.fid = vlan4k.fid;
			err = rtl8366s_set_vlan_mc(smi, i, &vlanmc);
			if (err)
				return err;

			err = rtl8366s_set_mc_index(smi, port, i);
			return err;
		}
	}

	/* MC table is full, try to find an unused entry and replace it */
	for (i = 0; i < RTL8366S_NUM_VLANS; i++) {
		int used;

		err = rtl8366s_mc_is_used(smi, i, &used);
		if (err)
			return err;

		if (!used) {
			/* Update the entry from the 4K table */
			err = rtl8366s_get_vlan_4k(smi, vid, &vlan4k);
			if (err)
				return err;

			vlanmc.vid = vid;
			vlanmc.member = vlan4k.member;
			vlanmc.untag = vlan4k.untag;
			vlanmc.fid = vlan4k.fid;
			err = rtl8366s_set_vlan_mc(smi, i, &vlanmc);
			if (err)
				return err;

			err = rtl8366s_set_mc_index(smi, port, i);
			return err;
		}
	}

	dev_err(smi->parent,
		"all VLAN member configurations are in use\n");

	return -ENOSPC;
}

static int rtl8366s_vlan_set_vlan(struct rtl8366_smi *smi, int enable)
{
	return rtl8366_smi_rmwr(smi, RTL8366S_CHIP_GLOBAL_CTRL_REG,
				RTL8366S_CHIP_CTRL_VLAN,
				(enable) ? RTL8366S_CHIP_CTRL_VLAN : 0);
}

static int rtl8366s_vlan_set_4ktable(struct rtl8366_smi *smi, int enable)
{
	return rtl8366_smi_rmwr(smi, RTL8366S_VLAN_TB_CTRL_REG,
				1, (enable) ? 1 : 0);
}

static int rtl8366s_reset_vlan(struct rtl8366_smi *smi)
{
	struct rtl8366_vlan_mc vlanmc;
	int err;
	int i;

	/* clear VLAN member configurations */
	vlanmc.vid = 0;
	vlanmc.priority = 0;
	vlanmc.member = 0;
	vlanmc.untag = 0;
	vlanmc.fid = 0;
	for (i = 0; i < RTL8366S_NUM_VLANS; i++) {
		err = rtl8366s_set_vlan_mc(smi, i, &vlanmc);
		if (err)
			return err;
	}

	for (i = 0; i < RTL8366S_NUM_PORTS; i++) {
		if (i == RTL8366S_PORT_CPU)
			continue;

		err = rtl8366s_set_vlan(smi, (i + 1),
					 (1 << i) | RTL8366S_PORT_CPU,
					 (1 << i) | RTL8366S_PORT_CPU,
					 0);
		if (err)
			return err;

		err = rtl8366s_set_pvid(smi, i, (i + 1));
		if (err)
			return err;
	}

	return 0;
}

#ifdef CONFIG_RTL8366S_PHY_DEBUG_FS
static int rtl8366s_debugfs_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static ssize_t rtl8366s_read_debugfs_mibs(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct rtl8366s *rtl = (struct rtl8366s *)file->private_data;
	struct rtl8366_smi *smi = &rtl->smi;
	int i, j, len = 0;
	char *buf = rtl->buf;

	len += snprintf(buf + len, sizeof(rtl->buf) - len,
			"%-36s %12s %12s %12s %12s %12s %12s\n",
			"Counter",
			"Port 0", "Port 1", "Port 2",
			"Port 3", "Port 4", "Port 5");

	for (i = 0; i < ARRAY_SIZE(rtl8366s_mib_counters); ++i) {
		len += snprintf(buf + len, sizeof(rtl->buf) - len, "%-36s ",
				rtl8366s_mib_counters[i].name);
		for (j = 0; j < RTL8366S_NUM_PORTS; ++j) {
			unsigned long long counter = 0;

			if (!rtl8366_get_mib_counter(smi, i, j, &counter))
				len += snprintf(buf + len,
						sizeof(rtl->buf) - len,
						"%12llu ", counter);
			else
				len += snprintf(buf + len,
						sizeof(rtl->buf) - len,
						"%12s ", "error");
		}
		len += snprintf(buf + len, sizeof(rtl->buf) - len, "\n");
	}

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366s_read_debugfs_vlan_mc(struct file *file,
					     char __user *user_buf,
					     size_t count, loff_t *ppos)
{
	struct rtl8366s *rtl = (struct rtl8366s *)file->private_data;
	struct rtl8366_smi *smi = &rtl->smi;
	int i, len = 0;
	char *buf = rtl->buf;

	len += snprintf(buf + len, sizeof(rtl->buf) - len,
			"%2s %6s %4s %6s %6s %3s\n",
			"id", "vid","prio", "member", "untag", "fid");

	for (i = 0; i < RTL8366S_NUM_VLANS; ++i) {
		struct rtl8366_vlan_mc vlanmc;

		rtl8366s_get_vlan_mc(smi, i, &vlanmc);

		len += snprintf(buf + len, sizeof(rtl->buf) - len,
				"%2d %6d %4d 0x%04x 0x%04x %3d\n",
				i, vlanmc.vid, vlanmc.priority,
				vlanmc.member, vlanmc.untag, vlanmc.fid);
	}

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366s_read_debugfs_reg(struct file *file,
					 char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	struct rtl8366s *rtl = (struct rtl8366s *)file->private_data;
	struct rtl8366_smi *smi = &rtl->smi;
	u32 t, reg = g_dbg_reg;
	int err, len = 0;
	char *buf = rtl->buf;

	memset(buf, '\0', sizeof(rtl->buf));

	err = rtl8366_smi_read_reg(smi, reg, &t);
	if (err) {
		len += snprintf(buf, sizeof(rtl->buf),
				"Read failed (reg: 0x%04x)\n", reg);
		return simple_read_from_buffer(user_buf, count, ppos, buf, len);
	}

	len += snprintf(buf, sizeof(rtl->buf), "reg = 0x%04x, val = 0x%04x\n",
			reg, t);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366s_write_debugfs_reg(struct file *file,
					  const char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct rtl8366s *rtl = (struct rtl8366s *)file->private_data;
	struct rtl8366_smi *smi = &rtl->smi;
	unsigned long data;
	u32 reg = g_dbg_reg;
	int err;
	size_t len;
	char *buf = rtl->buf;

	len = min(count, sizeof(rtl->buf) - 1);
	if (copy_from_user(buf, user_buf, len)) {
		dev_err(rtl->parent, "copy from user failed\n");
		return -EFAULT;
	}

	buf[len] = '\0';
	if (len > 0 && buf[len - 1] == '\n')
		buf[len - 1] = '\0';


	if (strict_strtoul(buf, 16, &data)) {
		dev_err(rtl->parent, "Invalid reg value %s\n", buf);
	} else {
		err = rtl8366_smi_write_reg(smi, reg, data);
		if (err) {
			dev_err(rtl->parent,
				"writing reg 0x%04x val 0x%04lx failed\n",
				reg, data);
		}
	}

	return count;
}

static const struct file_operations fops_rtl8366s_regs = {
	.read = rtl8366s_read_debugfs_reg,
	.write = rtl8366s_write_debugfs_reg,
	.open = rtl8366s_debugfs_open,
	.owner = THIS_MODULE
};

static const struct file_operations fops_rtl8366s_vlan_mc = {
	.read = rtl8366s_read_debugfs_vlan_mc,
	.open = rtl8366s_debugfs_open,
	.owner = THIS_MODULE
};

static const struct file_operations fops_rtl8366s_mibs = {
	.read = rtl8366s_read_debugfs_mibs,
	.open = rtl8366s_debugfs_open,
	.owner = THIS_MODULE
};

static void rtl8366s_debugfs_init(struct rtl8366s *rtl)
{
	struct dentry *node;
	struct dentry *root;

	if (!rtl->debugfs_root)
		rtl->debugfs_root = debugfs_create_dir("rtl8366s", NULL);

	if (!rtl->debugfs_root) {
		dev_err(rtl->parent, "Unable to create debugfs dir\n");
		return;
	}
	root = rtl->debugfs_root;

	node = debugfs_create_x16("reg", S_IRUGO | S_IWUSR, root, &g_dbg_reg);
	if (!node) {
		dev_err(rtl->parent, "Creating debugfs file '%s' failed\n",
			"reg");
		return;
	}

	node = debugfs_create_file("val", S_IRUGO | S_IWUSR, root, rtl,
				   &fops_rtl8366s_regs);
	if (!node) {
		dev_err(rtl->parent, "Creating debugfs file '%s' failed\n",
			"val");
		return;
	}

	node = debugfs_create_file("vlan_mc", S_IRUSR, root, rtl,
				   &fops_rtl8366s_vlan_mc);
	if (!node) {
		dev_err(rtl->parent, "Creating debugfs file '%s' failed\n",
			"vlan_mc");
		return;
	}

	node = debugfs_create_file("mibs", S_IRUSR, root, rtl,
				   &fops_rtl8366s_mibs);
	if (!node) {
		dev_err(rtl->parent, "Creating debugfs file '%s' failed\n",
			"mibs");
		return;
	}
}

static void rtl8366s_debugfs_remove(struct rtl8366s *rtl)
{
	if (rtl->debugfs_root) {
		debugfs_remove_recursive(rtl->debugfs_root);
		rtl->debugfs_root = NULL;
	}
}

#else
static inline void rtl8366s_debugfs_init(struct rtl8366s *rtl) {}
static inline void rtl8366s_debugfs_remove(struct rtl8366s *rtl) {}
#endif /* CONFIG_RTL8366S_PHY_DEBUG_FS */

static int rtl8366s_sw_reset_mibs(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	int err = 0;

	if (val->value.i == 1)
		err = rtl8366_smi_rmwr(smi, RTL8366S_MIB_CTRL_REG, 0, (1 << 2));

	return err;
}

static int rtl8366s_sw_get_vlan_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;

	if (attr->ofs == 1) {
		rtl8366_smi_read_reg(smi, RTL8366S_CHIP_GLOBAL_CTRL_REG, &data);

		if (data & RTL8366S_CHIP_CTRL_VLAN)
			val->value.i = 1;
		else
			val->value.i = 0;
	} else if (attr->ofs == 2) {
		rtl8366_smi_read_reg(smi, RTL8366S_VLAN_TB_CTRL_REG, &data);

		if (data & 0x0001)
			val->value.i = 1;
		else
			val->value.i = 0;
	}

	return 0;
}

static int rtl8366s_sw_get_blinkrate(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;

	rtl8366_smi_read_reg(smi, RTL8366S_LED_BLINKRATE_REG, &data);

	val->value.i = (data & (RTL8366S_LED_BLINKRATE_MASK));

	return 0;
}

static int rtl8366s_sw_set_blinkrate(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (val->value.i >= 6)
		return -EINVAL;

	return rtl8366_smi_rmwr(smi, RTL8366S_LED_BLINKRATE_REG,
				RTL8366S_LED_BLINKRATE_MASK,
				val->value.i);
}

static int rtl8366s_sw_set_vlan_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (attr->ofs == 1)
		return rtl8366s_vlan_set_vlan(smi, val->value.i);
	else
		return rtl8366s_vlan_set_4ktable(smi, val->value.i);
}

static const char *rtl8366s_speed_str(unsigned speed)
{
	switch (speed) {
	case 0:
		return "10baseT";
	case 1:
		return "100baseT";
	case 2:
		return "1000baseT";
	}

	return "unknown";
}

static int rtl8366s_sw_get_port_link(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	struct rtl8366s *rtl = sw_to_rtl8366s(dev);
	struct rtl8366_smi *smi = &rtl->smi;
	u32 len = 0, data = 0;

	if (val->port_vlan >= RTL8366S_NUM_PORTS)
		return -EINVAL;

	memset(rtl->buf, '\0', sizeof(rtl->buf));
	rtl8366_smi_read_reg(smi, RTL8366S_PORT_LINK_STATUS_BASE +
			     (val->port_vlan / 2), &data);

	if (val->port_vlan % 2)
		data = data >> 8;

	if (data & RTL8366S_PORT_STATUS_LINK_MASK) {
		len = snprintf(rtl->buf, sizeof(rtl->buf),
				"port:%d link:up speed:%s %s-duplex %s%s%s",
				val->port_vlan,
				rtl8366s_speed_str(data &
					  RTL8366S_PORT_STATUS_SPEED_MASK),
				(data & RTL8366S_PORT_STATUS_DUPLEX_MASK) ?
					"full" : "half",
				(data & RTL8366S_PORT_STATUS_TXPAUSE_MASK) ?
					"tx-pause ": "",
				(data & RTL8366S_PORT_STATUS_RXPAUSE_MASK) ?
					"rx-pause " : "",
				(data & RTL8366S_PORT_STATUS_AN_MASK) ?
					"nway ": "");
	} else {
		len = snprintf(rtl->buf, sizeof(rtl->buf), "port:%d link: down",
				val->port_vlan);
	}

	val->value.s = rtl->buf;
	val->len = len;

	return 0;
}

static int rtl8366s_sw_get_vlan_info(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	int i;
	u32 len = 0;
	struct rtl8366_vlan_4k vlan4k;
	struct rtl8366s *rtl = sw_to_rtl8366s(dev);
	struct rtl8366_smi *smi = &rtl->smi;
	char *buf = rtl->buf;
	int err;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366S_NUM_VLANS)
		return -EINVAL;

	memset(buf, '\0', sizeof(rtl->buf));

	err = rtl8366s_get_vlan_4k(smi, val->port_vlan, &vlan4k);
	if (err)
		return err;

	len += snprintf(buf + len, sizeof(rtl->buf) - len,
			"VLAN %d: Ports: '", vlan4k.vid);

	for (i = 0; i < RTL8366S_NUM_PORTS; i++) {
		if (!(vlan4k.member & (1 << i)))
			continue;

		len += snprintf(buf + len, sizeof(rtl->buf) - len, "%d%s", i,
				(vlan4k.untag & (1 << i)) ? "" : "t");
	}

	len += snprintf(buf + len, sizeof(rtl->buf) - len,
			"', members=%04x, untag=%04x, fid=%u",
			vlan4k.member, vlan4k.untag, vlan4k.fid);

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int rtl8366s_sw_set_port_led(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;
	u32 mask;
	u32 reg;

	if (val->port_vlan >= RTL8366S_NUM_PORTS ||
	    (1 << val->port_vlan) == RTL8366S_PORT_UNKNOWN)
		return -EINVAL;

	if (val->port_vlan == RTL8366S_PORT_NUM_CPU) {
		reg = RTL8366S_LED_BLINKRATE_REG;
		mask = 0xF << 4;
		data = val->value.i << 4;
	} else {
		reg = RTL8366S_LED_CTRL_REG;
		mask = 0xF << (val->port_vlan * 4),
		data = val->value.i << (val->port_vlan * 4);
	}

	return rtl8366_smi_rmwr(smi, RTL8366S_LED_BLINKRATE_REG, mask, data);
}

static int rtl8366s_sw_get_port_led(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data = 0;

	if (val->port_vlan >= RTL8366S_NUM_LEDGROUPS)
		return -EINVAL;

	rtl8366_smi_read_reg(smi, RTL8366S_LED_CTRL_REG, &data);
	val->value.i = (data >> (val->port_vlan * 4)) & 0x000F;

	return 0;
}

static int rtl8366s_sw_reset_port_mibs(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (val->port_vlan >= RTL8366S_NUM_PORTS)
		return -EINVAL;


	return rtl8366_smi_rmwr(smi, RTL8366S_MIB_CTRL_REG,
				0, (1 << (val->port_vlan + 3)));
}

static int rtl8366s_sw_get_port_mib(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366s *rtl = sw_to_rtl8366s(dev);
	struct rtl8366_smi *smi = &rtl->smi;
	int i, len = 0;
	unsigned long long counter = 0;
	char *buf = rtl->buf;

	if (val->port_vlan >= RTL8366S_NUM_PORTS)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(rtl->buf) - len,
			"Port %d MIB counters\n",
			val->port_vlan);

	for (i = 0; i < ARRAY_SIZE(rtl8366s_mib_counters); ++i) {
		len += snprintf(buf + len, sizeof(rtl->buf) - len,
				"%-36s: ", rtl8366s_mib_counters[i].name);
		if (!rtl8366_get_mib_counter(smi, i, val->port_vlan, &counter))
			len += snprintf(buf + len, sizeof(rtl->buf) - len,
					"%llu\n", counter);
		else
			len += snprintf(buf + len, sizeof(rtl->buf) - len,
					"%s\n", "error");
	}

	val->value.s = buf;
	val->len = len;
	return 0;
}

static int rtl8366s_sw_get_vlan_ports(struct switch_dev *dev,
				      struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	struct switch_port *port;
	struct rtl8366_vlan_4k vlan4k;
	int i;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366S_NUM_VLANS)
		return -EINVAL;

	rtl8366s_get_vlan_4k(smi, val->port_vlan, &vlan4k);

	port = &val->value.ports[0];
	val->len = 0;
	for (i = 0; i < RTL8366S_NUM_PORTS; i++) {
		if (!(vlan4k.member & BIT(i)))
			continue;

		port->id = i;
		port->flags = (vlan4k.untag & BIT(i)) ?
					0 : BIT(SWITCH_PORT_FLAG_TAGGED);
		val->len++;
		port++;
	}
	return 0;
}

static int rtl8366s_sw_set_vlan_ports(struct switch_dev *dev,
				      struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	struct switch_port *port;
	u32 member = 0;
	u32 untag = 0;
	int i;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366S_NUM_VLANS)
		return -EINVAL;

	port = &val->value.ports[0];
	for (i = 0; i < val->len; i++, port++) {
		member |= BIT(port->id);

		if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED)))
			untag |= BIT(port->id);
	}

	return rtl8366s_set_vlan(smi, val->port_vlan, member, untag, 0);
}

static int rtl8366s_sw_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	return rtl8366s_get_pvid(smi, port, val);
}

static int rtl8366s_sw_set_port_pvid(struct switch_dev *dev, int port, int val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	return rtl8366s_set_pvid(smi, port, val);
}

static int rtl8366s_sw_reset_switch(struct switch_dev *dev)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	int err;

	err = rtl8366s_reset_chip(smi);
	if (err)
		return err;

	err = rtl8366s_hw_init(smi);
	if (err)
		return err;

	return rtl8366s_reset_vlan(smi);
}

static struct switch_attr rtl8366s_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = rtl8366s_sw_set_vlan_enable,
		.get = rtl8366s_sw_get_vlan_enable,
		.max = 1,
		.ofs = 1
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan4k",
		.description = "Enable VLAN 4K mode",
		.set = rtl8366s_sw_set_vlan_enable,
		.get = rtl8366s_sw_get_vlan_enable,
		.max = 1,
		.ofs = 2
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "reset_mibs",
		.description = "Reset all MIB counters",
		.set = rtl8366s_sw_reset_mibs,
		.get = NULL,
		.max = 1
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "blinkrate",
		.description = "Get/Set LED blinking rate (0 = 43ms, 1 = 84ms,"
		" 2 = 120ms, 3 = 170ms, 4 = 340ms, 5 = 670ms)",
		.set = rtl8366s_sw_set_blinkrate,
		.get = rtl8366s_sw_get_blinkrate,
		.max = 5
	},
};

static struct switch_attr rtl8366s_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "link",
		.description = "Get port link information",
		.max = 1,
		.set = NULL,
		.get = rtl8366s_sw_get_port_link,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "reset_mib",
		.description = "Reset single port MIB counters",
		.max = 1,
		.set = rtl8366s_sw_reset_port_mibs,
		.get = NULL,
	}, {
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.max = 33,
		.set = NULL,
		.get = rtl8366s_sw_get_port_mib,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "led",
		.description = "Get/Set port group (0 - 3) led mode (0 - 15)",
		.max = 15,
		.set = rtl8366s_sw_set_port_led,
		.get = rtl8366s_sw_get_port_led,
	},
};

static struct switch_attr rtl8366s_vlan[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "info",
		.description = "Get vlan information",
		.max = 1,
		.set = NULL,
		.get = rtl8366s_sw_get_vlan_info,
	},
};

/* template */
static struct switch_dev rtl8366_switch_dev = {
	.name = "RTL8366S",
	.cpu_port = RTL8366S_PORT_NUM_CPU,
	.ports = RTL8366S_NUM_PORTS,
	.vlans = RTL8366S_NUM_VLANS,
	.attr_global = {
		.attr = rtl8366s_globals,
		.n_attr = ARRAY_SIZE(rtl8366s_globals),
	},
	.attr_port = {
		.attr = rtl8366s_port,
		.n_attr = ARRAY_SIZE(rtl8366s_port),
	},
	.attr_vlan = {
		.attr = rtl8366s_vlan,
		.n_attr = ARRAY_SIZE(rtl8366s_vlan),
	},

	.get_vlan_ports = rtl8366s_sw_get_vlan_ports,
	.set_vlan_ports = rtl8366s_sw_set_vlan_ports,
	.get_port_pvid = rtl8366s_sw_get_port_pvid,
	.set_port_pvid = rtl8366s_sw_set_port_pvid,
	.reset_switch = rtl8366s_sw_reset_switch,
};

static int rtl8366s_switch_init(struct rtl8366s *rtl)
{
	struct switch_dev *dev = &rtl->dev;
	int err;

	memcpy(dev, &rtl8366_switch_dev, sizeof(struct switch_dev));
	dev->priv = rtl;
	dev->devname = dev_name(rtl->parent);

	err = register_switch(dev, NULL);
	if (err)
		dev_err(rtl->parent, "switch registration failed\n");

	return err;
}

static void rtl8366s_switch_cleanup(struct rtl8366s *rtl)
{
	unregister_switch(&rtl->dev);
}

static int rtl8366s_mii_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 val = 0;
	int err;

	err = rtl8366s_read_phy_reg(smi, addr, 0, reg, &val);
	if (err)
		return 0xffff;

	return val;
}

static int rtl8366s_mii_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 t;
	int err;

	err = rtl8366s_write_phy_reg(smi, addr, 0, reg, val);
	/* flush write */
	(void) rtl8366s_read_phy_reg(smi, addr, 0, reg, &t);

	return err;
}

static int rtl8366s_mii_bus_match(struct mii_bus *bus)
{
	return (bus->read == rtl8366s_mii_read &&
		bus->write == rtl8366s_mii_write);
}

static int rtl8366s_setup(struct rtl8366s *rtl)
{
	struct rtl8366_smi *smi = &rtl->smi;
	int ret;

	rtl8366s_debugfs_init(rtl);

	ret = rtl8366s_reset_chip(smi);
	if (ret)
		return ret;

	ret = rtl8366s_hw_init(smi);
	return ret;
}

static int rtl8366s_detect(struct rtl8366_smi *smi)
{
	u32 chip_id = 0;
	u32 chip_ver = 0;
	int ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_ID_REG, &chip_id);
	if (ret) {
		dev_err(smi->parent, "unable to read chip id\n");
		return ret;
	}

	switch (chip_id) {
	case RTL8366S_CHIP_ID_8366:
		break;
	default:
		dev_err(smi->parent, "unknown chip id (%04x)\n", chip_id);
		return -ENODEV;
	}

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_VERSION_CTRL_REG,
				   &chip_ver);
	if (ret) {
		dev_err(smi->parent, "unable to read chip version\n");
		return ret;
	}

	dev_info(smi->parent, "RTL%04x ver. %u chip found\n",
		 chip_id, chip_ver & RTL8366S_CHIP_VERSION_MASK);

	return 0;
}

static struct rtl8366_smi_ops rtl8366s_smi_ops = {
	.detect		= rtl8366s_detect,
	.mii_read	= rtl8366s_mii_read,
	.mii_write	= rtl8366s_mii_write,
};

static int __init rtl8366s_probe(struct platform_device *pdev)
{
	static int rtl8366_smi_version_printed;
	struct rtl8366s_platform_data *pdata;
	struct rtl8366s *rtl;
	struct rtl8366_smi *smi;
	int err;

	if (!rtl8366_smi_version_printed++)
		printk(KERN_NOTICE RTL8366S_DRIVER_DESC
		       " version " RTL8366S_DRIVER_VER"\n");

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		err = -EINVAL;
		goto err_out;
	}

	rtl = kzalloc(sizeof(*rtl), GFP_KERNEL);
	if (!rtl) {
		dev_err(&pdev->dev, "no memory for private data\n");
		err = -ENOMEM;
		goto err_out;
	}

	rtl->parent = &pdev->dev;

	smi = &rtl->smi;
	smi->parent = &pdev->dev;
	smi->gpio_sda = pdata->gpio_sda;
	smi->gpio_sck = pdata->gpio_sck;
	smi->ops = &rtl8366s_smi_ops;

	err = rtl8366_smi_init(smi);
	if (err)
		goto err_free_rtl;

	platform_set_drvdata(pdev, rtl);

	err = rtl8366s_setup(rtl);
	if (err)
		goto err_clear_drvdata;

	err = rtl8366s_switch_init(rtl);
	if (err)
		goto err_clear_drvdata;

	return 0;

 err_clear_drvdata:
	platform_set_drvdata(pdev, NULL);
	rtl8366_smi_cleanup(smi);
 err_free_rtl:
	kfree(rtl);
 err_out:
	return err;
}

static int rtl8366s_phy_config_init(struct phy_device *phydev)
{
	if (!rtl8366s_mii_bus_match(phydev->bus))
		return -EINVAL;

	return 0;
}

static int rtl8366s_phy_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver rtl8366s_phy_driver = {
	.phy_id		= 0x001cc960,
	.name		= "Realtek RTL8366S",
	.phy_id_mask	= 0x1ffffff0,
	.features	= PHY_GBIT_FEATURES,
	.config_aneg	= rtl8366s_phy_config_aneg,
	.config_init    = rtl8366s_phy_config_init,
	.read_status	= genphy_read_status,
	.driver		= {
		.owner = THIS_MODULE,
	},
};

static int __devexit rtl8366s_remove(struct platform_device *pdev)
{
	struct rtl8366s *rtl = platform_get_drvdata(pdev);

	if (rtl) {
		rtl8366s_switch_cleanup(rtl);
		rtl8366s_debugfs_remove(rtl);
		platform_set_drvdata(pdev, NULL);
		rtl8366_smi_cleanup(&rtl->smi);
		kfree(rtl);
	}

	return 0;
}

static struct platform_driver rtl8366s_driver = {
	.driver = {
		.name		= RTL8366S_DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe		= rtl8366s_probe,
	.remove		= __devexit_p(rtl8366s_remove),
};

static int __init rtl8366s_module_init(void)
{
	int ret;
	ret = platform_driver_register(&rtl8366s_driver);
	if (ret)
		return ret;

	ret = phy_driver_register(&rtl8366s_phy_driver);
	if (ret)
		goto err_platform_unregister;

	return 0;

 err_platform_unregister:
	platform_driver_unregister(&rtl8366s_driver);
	return ret;
}
module_init(rtl8366s_module_init);

static void __exit rtl8366s_module_exit(void)
{
	phy_driver_unregister(&rtl8366s_phy_driver);
	platform_driver_unregister(&rtl8366s_driver);
}
module_exit(rtl8366s_module_exit);

MODULE_DESCRIPTION(RTL8366S_DRIVER_DESC);
MODULE_VERSION(RTL8366S_DRIVER_VER);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Antti Seppälä <a.seppala@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" RTL8366S_DRIVER_NAME);
