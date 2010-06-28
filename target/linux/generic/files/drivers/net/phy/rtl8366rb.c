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
#include <linux/rtl8366rb.h>

#include "rtl8366_smi.h"

#define RTL8366RB_DRIVER_DESC	"Realtek RTL8366RB ethernet switch driver"
#define RTL8366RB_DRIVER_VER	"0.2.2"

#define RTL8366RB_PHY_NO_MAX	4
#define RTL8366RB_PHY_PAGE_MAX	7
#define RTL8366RB_PHY_ADDR_MAX	31

#define RTL8366RB_CHIP_GLOBAL_CTRL_REG		0x0000
#define RTL8366RB_CHIP_CTRL_VLAN		(1 << 13)
#define RTL8366RB_CHIP_CTRL_VLAN_4KTB		(1 << 14)

/* Switch Global Configuration register */
#define RTL8366RB_SGCR				0x0000
#define RTL8366RB_SGCR_EN_BC_STORM_CTRL		BIT(0)
#define RTL8366RB_SGCR_MAX_LENGTH(_x)		(_x << 4)
#define RTL8366RB_SGCR_MAX_LENGTH_MASK		RTL8366RB_SGCR_MAX_LENGTH(0x3)
#define RTL8366RB_SGCR_MAX_LENGTH_1522		RTL8366RB_SGCR_MAX_LENGTH(0x0)
#define RTL8366RB_SGCR_MAX_LENGTH_1536		RTL8366RB_SGCR_MAX_LENGTH(0x1)
#define RTL8366RB_SGCR_MAX_LENGTH_1552		RTL8366RB_SGCR_MAX_LENGTH(0x2)
#define RTL8366RB_SGCR_MAX_LENGTH_9216		RTL8366RB_SGCR_MAX_LENGTH(0x3)

/* Port Enable Control register */
#define RTL8366RB_PECR				0x0001

/* Switch Security Control registers */
#define RTL8366RB_SSCR0				0x0002
#define RTL8366RB_SSCR1				0x0003
#define RTL8366RB_SSCR2				0x0004
#define RTL8366RB_SSCR2_DROP_UNKNOWN_DA		BIT(0)

#define RTL8366RB_RESET_CTRL_REG		0x0100
#define RTL8366RB_CHIP_CTRL_RESET_HW		1
#define RTL8366RB_CHIP_CTRL_RESET_SW		(1 << 1)

#define RTL8366RB_CHIP_VERSION_CTRL_REG		0x050A
#define RTL8366RB_CHIP_VERSION_MASK		0xf
#define RTL8366RB_CHIP_ID_REG			0x0509
#define RTL8366RB_CHIP_ID_8366			0x5937

/* PHY registers control */
#define RTL8366RB_PHY_ACCESS_CTRL_REG		0x8000
#define RTL8366RB_PHY_ACCESS_DATA_REG		0x8002

#define RTL8366RB_PHY_CTRL_READ			1
#define RTL8366RB_PHY_CTRL_WRITE		0

#define RTL8366RB_PHY_REG_MASK			0x1f
#define RTL8366RB_PHY_PAGE_OFFSET		5
#define RTL8366RB_PHY_PAGE_MASK			(0xf << 5)
#define RTL8366RB_PHY_NO_OFFSET			9
#define RTL8366RB_PHY_NO_MASK			(0x1f << 9)

/* LED control registers */
#define RTL8366RB_LED_BLINKRATE_REG		0x0430
#define RTL8366RB_LED_BLINKRATE_BIT		0
#define RTL8366RB_LED_BLINKRATE_MASK		0x0007

#define RTL8366RB_LED_CTRL_REG			0x0431
#define RTL8366RB_LED_0_1_CTRL_REG		0x0432
#define RTL8366RB_LED_2_3_CTRL_REG		0x0433

#define RTL8366RB_MIB_COUNT			33
#define RTL8366RB_GLOBAL_MIB_COUNT		1
#define RTL8366RB_MIB_COUNTER_PORT_OFFSET	0x0050
#define RTL8366RB_MIB_COUNTER_BASE		0x1000
#define RTL8366RB_MIB_CTRL_REG			0x13F0
#define RTL8366RB_MIB_CTRL_USER_MASK		0x0FFC
#define RTL8366RB_MIB_CTRL_BUSY_MASK		BIT(0)
#define RTL8366RB_MIB_CTRL_RESET_MASK		BIT(1)
#define RTL8366RB_MIB_CTRL_PORT_RESET(_p)	BIT(2 + (_p))
#define RTL8366RB_MIB_CTRL_GLOBAL_RESET		BIT(11)

#define RTL8366RB_PORT_VLAN_CTRL_BASE		0x0063
#define RTL8366RB_PORT_VLAN_CTRL_REG(_p)  \
		(RTL8366RB_PORT_VLAN_CTRL_BASE + (_p) / 4)
#define RTL8366RB_PORT_VLAN_CTRL_MASK		0xf
#define RTL8366RB_PORT_VLAN_CTRL_SHIFT(_p)	(4 * ((_p) % 4))


#define RTL8366RB_VLAN_TABLE_READ_BASE		0x018C
#define RTL8366RB_VLAN_TABLE_WRITE_BASE		0x0185


#define RTL8366RB_TABLE_ACCESS_CTRL_REG		0x0180
#define RTL8366RB_TABLE_VLAN_READ_CTRL		0x0E01
#define RTL8366RB_TABLE_VLAN_WRITE_CTRL		0x0F01

#define RTL8366RB_VLAN_MEMCONF_BASE		0x0020


#define RTL8366RB_PORT_LINK_STATUS_BASE		0x0014
#define RTL8366RB_PORT_STATUS_SPEED_MASK	0x0003
#define RTL8366RB_PORT_STATUS_DUPLEX_MASK	0x0004
#define RTL8366RB_PORT_STATUS_LINK_MASK		0x0010
#define RTL8366RB_PORT_STATUS_TXPAUSE_MASK	0x0020
#define RTL8366RB_PORT_STATUS_RXPAUSE_MASK	0x0040
#define RTL8366RB_PORT_STATUS_AN_MASK		0x0080


#define RTL8366RB_PORT_NUM_CPU		5
#define RTL8366RB_NUM_PORTS		6
#define RTL8366RB_NUM_VLANS		16
#define RTL8366RB_NUM_LEDGROUPS		4
#define RTL8366RB_NUM_VIDS		4096
#define RTL8366RB_PRIORITYMAX		7
#define RTL8366RB_FIDMAX		7


#define RTL8366RB_PORT_1		(1 << 0) /* In userspace port 0 */
#define RTL8366RB_PORT_2		(1 << 1) /* In userspace port 1 */
#define RTL8366RB_PORT_3		(1 << 2) /* In userspace port 2 */
#define RTL8366RB_PORT_4		(1 << 3) /* In userspace port 3 */
#define RTL8366RB_PORT_5		(1 << 4) /* In userspace port 4 */

#define RTL8366RB_PORT_CPU		(1 << 5) /* CPU port */

#define RTL8366RB_PORT_ALL		(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4 |	\
					 RTL8366RB_PORT_5 |	\
					 RTL8366RB_PORT_CPU)

#define RTL8366RB_PORT_ALL_BUT_CPU	(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4 |	\
					 RTL8366RB_PORT_5)

#define RTL8366RB_PORT_ALL_EXTERNAL	(RTL8366RB_PORT_1 |	\
					 RTL8366RB_PORT_2 |	\
					 RTL8366RB_PORT_3 |	\
					 RTL8366RB_PORT_4)

#define RTL8366RB_PORT_ALL_INTERNAL	 RTL8366RB_PORT_CPU

struct rtl8366rb {
	struct device		*parent;
	struct rtl8366_smi	smi;
	struct switch_dev	dev;
};

struct rtl8366rb_vlan_mc {
	u16	reserved2:1;
	u16	priority:3;
	u16	vid:12;
	u16	untag:8;
	u16	member:8;
	u16	stag_mbr:8;
	u16	stag_idx:3;
	u16	reserved1:2;
	u16	fid:3;
};

struct rtl8366rb_vlan_4k {
	u16	reserved1:4;
	u16	vid:12;
	u16	untag:8;
	u16	member:8;
	u16	reserved2:13;
	u16	fid:3;
};

static struct rtl8366_mib_counter rtl8366rb_mib_counters[] = {
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
	{ 0, 64, 2, "Dot1dTpPortInDiscards"			},
	{ 0, 66, 2, "IfOutUcastPkts"				},
	{ 0, 68, 2, "IfOutMulticastPkts"			},
	{ 0, 70, 2, "IfOutBroadcastPkts"			},
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

static inline struct rtl8366rb *smi_to_rtl8366rb(struct rtl8366_smi *smi)
{
	return container_of(smi, struct rtl8366rb, smi);
}

static inline struct rtl8366rb *sw_to_rtl8366rb(struct switch_dev *sw)
{
	return container_of(sw, struct rtl8366rb, dev);
}

static inline struct rtl8366_smi *sw_to_rtl8366_smi(struct switch_dev *sw)
{
	struct rtl8366rb *rtl = sw_to_rtl8366rb(sw);
	return &rtl->smi;
}

static int rtl8366rb_reset_chip(struct rtl8366_smi *smi)
{
	int timeout = 10;
	u32 data;

	rtl8366_smi_write_reg(smi, RTL8366RB_RESET_CTRL_REG,
			      RTL8366RB_CHIP_CTRL_RESET_HW);
	do {
		msleep(1);
		if (rtl8366_smi_read_reg(smi, RTL8366RB_RESET_CTRL_REG, &data))
			return -EIO;

		if (!(data & RTL8366RB_CHIP_CTRL_RESET_HW))
			break;
	} while (--timeout);

	if (!timeout) {
		printk("Timeout waiting for the switch to reset\n");
		return -EIO;
	}

	return 0;
}

static int rtl8366rb_hw_init(struct rtl8366_smi *smi)
{
	int err;

	/* set maximum packet length to 1536 bytes */
	REG_RMW(smi, RTL8366RB_SGCR, RTL8366RB_SGCR_MAX_LENGTH_MASK,
		RTL8366RB_SGCR_MAX_LENGTH_1536);

	/* enable all ports */
	REG_WR(smi, RTL8366RB_PECR, 0);

	/* disable learning for all ports */
	REG_WR(smi, RTL8366RB_SSCR0, RTL8366RB_PORT_ALL);

	/* disable auto ageing for all ports */
	REG_WR(smi, RTL8366RB_SSCR1, RTL8366RB_PORT_ALL);

	/* don't drop packets whose DA has not been learned */
	REG_RMW(smi, RTL8366RB_SSCR2, RTL8366RB_SSCR2_DROP_UNKNOWN_DA, 0);

	return 0;
}

static int rtl8366rb_read_phy_reg(struct rtl8366_smi *smi,
				 u32 phy_no, u32 page, u32 addr, u32 *data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366RB_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366RB_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366RB_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366RB_PHY_ACCESS_CTRL_REG,
				    RTL8366RB_PHY_CTRL_READ);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366RB_PHY_NO_OFFSET)) |
	      ((page << RTL8366RB_PHY_PAGE_OFFSET) & RTL8366RB_PHY_PAGE_MASK) |
	      (addr & RTL8366RB_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, 0);
	if (ret)
		return ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366RB_PHY_ACCESS_DATA_REG, data);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366rb_write_phy_reg(struct rtl8366_smi *smi,
				  u32 phy_no, u32 page, u32 addr, u32 data)
{
	u32 reg;
	int ret;

	if (phy_no > RTL8366RB_PHY_NO_MAX)
		return -EINVAL;

	if (page > RTL8366RB_PHY_PAGE_MAX)
		return -EINVAL;

	if (addr > RTL8366RB_PHY_ADDR_MAX)
		return -EINVAL;

	ret = rtl8366_smi_write_reg(smi, RTL8366RB_PHY_ACCESS_CTRL_REG,
				    RTL8366RB_PHY_CTRL_WRITE);
	if (ret)
		return ret;

	reg = 0x8000 | (1 << (phy_no + RTL8366RB_PHY_NO_OFFSET)) |
	      ((page << RTL8366RB_PHY_PAGE_OFFSET) & RTL8366RB_PHY_PAGE_MASK) |
	      (addr & RTL8366RB_PHY_REG_MASK);

	ret = rtl8366_smi_write_reg(smi, reg, data);
	if (ret)
		return ret;

	return 0;
}

static int rtl8366rb_get_mib_counter(struct rtl8366_smi *smi, int counter,
				     int port, unsigned long long *val)
{
	int i;
	int err;
	u32 addr, data;
	u64 mibvalue;

	if (port > RTL8366RB_NUM_PORTS || counter >= RTL8366RB_MIB_COUNT)
		return -EINVAL;

	addr = RTL8366RB_MIB_COUNTER_BASE +
	       RTL8366RB_MIB_COUNTER_PORT_OFFSET * (port) +
	       rtl8366rb_mib_counters[counter].offset;

	/*
	 * Writing access counter address first
	 * then ASIC will prepare 64bits counter wait for being retrived
	 */
	data = 0; /* writing data will be discard by ASIC */
	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	/* read MIB control register */
	err =  rtl8366_smi_read_reg(smi, RTL8366RB_MIB_CTRL_REG, &data);
	if (err)
		return err;

	if (data & RTL8366RB_MIB_CTRL_BUSY_MASK)
		return -EBUSY;

	if (data & RTL8366RB_MIB_CTRL_RESET_MASK)
		return -EIO;

	mibvalue = 0;
	for (i = rtl8366rb_mib_counters[counter].length; i > 0; i--) {
		err = rtl8366_smi_read_reg(smi, addr + (i - 1), &data);
		if (err)
			return err;

		mibvalue = (mibvalue << 16) | (data & 0xFFFF);
	}

	*val = mibvalue;
	return 0;
}

static int rtl8366rb_get_vlan_4k(struct rtl8366_smi *smi, u32 vid,
				 struct rtl8366_vlan_4k *vlan4k)
{
	struct rtl8366rb_vlan_4k vlan4k_priv;
	int err;
	u32 data;
	u16 *tableaddr;

	memset(vlan4k, '\0', sizeof(struct rtl8366_vlan_4k));
	vlan4k_priv.vid = vid;

	if (vid >= RTL8366RB_NUM_VIDS)
		return -EINVAL;

	tableaddr = (u16 *)&vlan4k_priv;

	/* write VID */
	data = *tableaddr;
	err = rtl8366_smi_write_reg(smi, RTL8366RB_VLAN_TABLE_WRITE_BASE, data);
	if (err)
		return err;

	/* write table access control word */
	err = rtl8366_smi_write_reg(smi, RTL8366RB_TABLE_ACCESS_CTRL_REG,
				    RTL8366RB_TABLE_VLAN_READ_CTRL);
	if (err)
		return err;

	err = rtl8366_smi_read_reg(smi, RTL8366RB_VLAN_TABLE_READ_BASE, &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	err = rtl8366_smi_read_reg(smi, RTL8366RB_VLAN_TABLE_READ_BASE + 1,
				   &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	err = rtl8366_smi_read_reg(smi, RTL8366RB_VLAN_TABLE_READ_BASE + 2,
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

static int rtl8366rb_set_vlan_4k(struct rtl8366_smi *smi,
				 const struct rtl8366_vlan_4k *vlan4k)
{
	struct rtl8366rb_vlan_4k vlan4k_priv;
	int err;
	u32 data;
	u16 *tableaddr;

	if (vlan4k->vid >= RTL8366RB_NUM_VIDS ||
	    vlan4k->member > RTL8366RB_PORT_ALL ||
	    vlan4k->untag > RTL8366RB_PORT_ALL ||
	    vlan4k->fid > RTL8366RB_FIDMAX)
		return -EINVAL;

	vlan4k_priv.vid = vlan4k->vid;
	vlan4k_priv.untag = vlan4k->untag;
	vlan4k_priv.member = vlan4k->member;
	vlan4k_priv.fid = vlan4k->fid;

	tableaddr = (u16 *)&vlan4k_priv;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366RB_VLAN_TABLE_WRITE_BASE, data);
	if (err)
		return err;

	tableaddr++;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366RB_VLAN_TABLE_WRITE_BASE + 1,
				    data);
	if (err)
		return err;

	tableaddr++;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366RB_VLAN_TABLE_WRITE_BASE + 2,
				    data);
	if (err)
		return err;

	/* write table access control word */
	err = rtl8366_smi_write_reg(smi, RTL8366RB_TABLE_ACCESS_CTRL_REG,
				    RTL8366RB_TABLE_VLAN_WRITE_CTRL);

	return err;
}

static int rtl8366rb_get_vlan_mc(struct rtl8366_smi *smi, u32 index,
				 struct rtl8366_vlan_mc *vlanmc)
{
	struct rtl8366rb_vlan_mc vlanmc_priv;
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	memset(vlanmc, '\0', sizeof(struct rtl8366_vlan_mc));

	if (index >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	tableaddr = (u16 *)&vlanmc_priv;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + (index * 3);
	err = rtl8366_smi_read_reg(smi, addr, &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + 1 + (index * 3);
	err = rtl8366_smi_read_reg(smi, addr, &data);
	if (err)
		return err;

	*tableaddr = data;
	tableaddr++;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + 2 + (index * 3);
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

static int rtl8366rb_set_vlan_mc(struct rtl8366_smi *smi, u32 index,
				 const struct rtl8366_vlan_mc *vlanmc)
{
	struct rtl8366rb_vlan_mc vlanmc_priv;
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	if (index >= RTL8366RB_NUM_VLANS ||
	    vlanmc->vid >= RTL8366RB_NUM_VIDS ||
	    vlanmc->priority > RTL8366RB_PRIORITYMAX ||
	    vlanmc->member > RTL8366RB_PORT_ALL ||
	    vlanmc->untag > RTL8366RB_PORT_ALL ||
	    vlanmc->fid > RTL8366RB_FIDMAX)
		return -EINVAL;

	vlanmc_priv.vid = vlanmc->vid;
	vlanmc_priv.priority = vlanmc->priority;
	vlanmc_priv.untag = vlanmc->untag;
	vlanmc_priv.member = vlanmc->member;
	vlanmc_priv.stag_mbr = 0;
	vlanmc_priv.stag_idx = 0;
	vlanmc_priv.fid = vlanmc->fid;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + (index * 3);

	tableaddr = (u16 *)&vlanmc_priv;
	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + 1 + (index * 3);

	tableaddr++;
	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;

	addr = RTL8366RB_VLAN_MEMCONF_BASE + 2 + (index * 3);

	tableaddr++;
	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, addr, data);
	if (err)
		return err;
	return 0;
}

static int rtl8366rb_get_mc_index(struct rtl8366_smi *smi, int port, int *val)
{
	u32 data;
	int err;

	if (port >= RTL8366RB_NUM_PORTS)
		return -EINVAL;

	err = rtl8366_smi_read_reg(smi, RTL8366RB_PORT_VLAN_CTRL_REG(port),
				   &data);
	if (err)
		return err;

	*val = (data >> RTL8366RB_PORT_VLAN_CTRL_SHIFT(port)) &
	       RTL8366RB_PORT_VLAN_CTRL_MASK;

	return 0;

}

static int rtl8366rb_set_mc_index(struct rtl8366_smi *smi, int port, int index)
{
	if (port >= RTL8366RB_NUM_PORTS || index >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	return rtl8366_smi_rmwr(smi, RTL8366RB_PORT_VLAN_CTRL_REG(port),
				RTL8366RB_PORT_VLAN_CTRL_MASK <<
					RTL8366RB_PORT_VLAN_CTRL_SHIFT(port),
				(index & RTL8366RB_PORT_VLAN_CTRL_MASK) <<
					RTL8366RB_PORT_VLAN_CTRL_SHIFT(port));
}

static int rtl8366rb_vlan_set_vlan(struct rtl8366_smi *smi, int enable)
{
	return rtl8366_smi_rmwr(smi, RTL8366RB_CHIP_GLOBAL_CTRL_REG,
				RTL8366RB_CHIP_CTRL_VLAN,
				(enable) ? RTL8366RB_CHIP_CTRL_VLAN : 0);
}

static int rtl8366rb_vlan_set_4ktable(struct rtl8366_smi *smi, int enable)
{
	return rtl8366_smi_rmwr(smi, RTL8366RB_CHIP_GLOBAL_CTRL_REG,
				RTL8366RB_CHIP_CTRL_VLAN_4KTB,
				(enable) ? RTL8366RB_CHIP_CTRL_VLAN_4KTB : 0);
}

static int rtl8366rb_sw_reset_mibs(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	int err = 0;

	if (val->value.i == 1)
		err = rtl8366_smi_rmwr(smi, RTL8366RB_MIB_CTRL_REG, 0,
				       RTL8366RB_MIB_CTRL_GLOBAL_RESET);

	return err;
}

static int rtl8366rb_sw_get_vlan_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;

	if (attr->ofs == 1) {
		rtl8366_smi_read_reg(smi, RTL8366RB_CHIP_GLOBAL_CTRL_REG, &data);

		if (data & RTL8366RB_CHIP_CTRL_VLAN)
			val->value.i = 1;
		else
			val->value.i = 0;
	} else if (attr->ofs == 2) {
		rtl8366_smi_read_reg(smi, RTL8366RB_CHIP_GLOBAL_CTRL_REG, &data);

		if (data & RTL8366RB_CHIP_CTRL_VLAN_4KTB)
			val->value.i = 1;
		else
			val->value.i = 0;
	}

	return 0;
}

static int rtl8366rb_sw_get_blinkrate(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;

	rtl8366_smi_read_reg(smi, RTL8366RB_LED_BLINKRATE_REG, &data);

	val->value.i = (data & (RTL8366RB_LED_BLINKRATE_MASK));

	return 0;
}

static int rtl8366rb_sw_set_blinkrate(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (val->value.i >= 6)
		return -EINVAL;

	return rtl8366_smi_rmwr(smi, RTL8366RB_LED_BLINKRATE_REG,
				RTL8366RB_LED_BLINKRATE_MASK,
				val->value.i);
}

static int rtl8366rb_sw_set_vlan_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (attr->ofs == 1)
		return rtl8366rb_vlan_set_vlan(smi, val->value.i);
	else
		return rtl8366rb_vlan_set_4ktable(smi, val->value.i);
}

static const char *rtl8366rb_speed_str(unsigned speed)
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

static int rtl8366rb_sw_get_port_link(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 len = 0, data = 0;

	if (val->port_vlan >= RTL8366RB_NUM_PORTS)
		return -EINVAL;

	memset(smi->buf, '\0', sizeof(smi->buf));
	rtl8366_smi_read_reg(smi, RTL8366RB_PORT_LINK_STATUS_BASE +
			     (val->port_vlan / 2), &data);

	if (val->port_vlan % 2)
		data = data >> 8;

	if (data & RTL8366RB_PORT_STATUS_LINK_MASK) {
		len = snprintf(smi->buf, sizeof(smi->buf),
				"port:%d link:up speed:%s %s-duplex %s%s%s",
				val->port_vlan,
				rtl8366rb_speed_str(data &
					  RTL8366RB_PORT_STATUS_SPEED_MASK),
				(data & RTL8366RB_PORT_STATUS_DUPLEX_MASK) ?
					"full" : "half",
				(data & RTL8366RB_PORT_STATUS_TXPAUSE_MASK) ?
					"tx-pause ": "",
				(data & RTL8366RB_PORT_STATUS_RXPAUSE_MASK) ?
					"rx-pause " : "",
				(data & RTL8366RB_PORT_STATUS_AN_MASK) ?
					"nway ": "");
	} else {
		len = snprintf(smi->buf, sizeof(smi->buf), "port:%d link: down",
				val->port_vlan);
	}

	val->value.s = smi->buf;
	val->len = len;

	return 0;
}

static int rtl8366rb_sw_get_vlan_info(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	int i;
	u32 len = 0;
	struct rtl8366_vlan_4k vlan4k;
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	char *buf = smi->buf;
	int err;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	memset(buf, '\0', sizeof(smi->buf));

	err = rtl8366rb_get_vlan_4k(smi, val->port_vlan, &vlan4k);
	if (err)
		return err;

	len += snprintf(buf + len, sizeof(smi->buf) - len,
			"VLAN %d: Ports: '", vlan4k.vid);

	for (i = 0; i < RTL8366RB_NUM_PORTS; i++) {
		if (!(vlan4k.member & (1 << i)))
			continue;

		len += snprintf(buf + len, sizeof(smi->buf) - len, "%d%s", i,
				(vlan4k.untag & (1 << i)) ? "" : "t");
	}

	len += snprintf(buf + len, sizeof(smi->buf) - len,
			"', members=%04x, untag=%04x, fid=%u",
			vlan4k.member, vlan4k.untag, vlan4k.fid);

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int rtl8366rb_sw_set_port_led(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data;
	u32 mask;
	u32 reg;

	if (val->port_vlan >= RTL8366RB_NUM_PORTS)
		return -EINVAL;

	if (val->port_vlan == RTL8366RB_PORT_NUM_CPU) {
		reg = RTL8366RB_LED_BLINKRATE_REG;
		mask = 0xF << 4;
		data = val->value.i << 4;
	} else {
		reg = RTL8366RB_LED_CTRL_REG;
		mask = 0xF << (val->port_vlan * 4),
		data = val->value.i << (val->port_vlan * 4);
	}

	return rtl8366_smi_rmwr(smi, RTL8366RB_LED_BLINKRATE_REG, mask, data);
}

static int rtl8366rb_sw_get_port_led(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	u32 data = 0;

	if (val->port_vlan >= RTL8366RB_NUM_LEDGROUPS)
		return -EINVAL;

	rtl8366_smi_read_reg(smi, RTL8366RB_LED_CTRL_REG, &data);
	val->value.i = (data >> (val->port_vlan * 4)) & 0x000F;

	return 0;
}

static int rtl8366rb_sw_reset_port_mibs(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);

	if (val->port_vlan >= RTL8366RB_NUM_PORTS)
		return -EINVAL;

	return rtl8366_smi_rmwr(smi, RTL8366RB_MIB_CTRL_REG, 0,
				RTL8366RB_MIB_CTRL_PORT_RESET(val->port_vlan));
}

static int rtl8366rb_sw_get_port_mib(struct switch_dev *dev,
				    const struct switch_attr *attr,
				    struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	int i, len = 0;
	unsigned long long counter = 0;
	char *buf = smi->buf;

	if (val->port_vlan >= RTL8366RB_NUM_PORTS)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(smi->buf) - len,
			"Port %d MIB counters\n",
			val->port_vlan);

	for (i = 0; i < ARRAY_SIZE(rtl8366rb_mib_counters); ++i) {
		len += snprintf(buf + len, sizeof(smi->buf) - len,
				"%-36s: ", rtl8366rb_mib_counters[i].name);
		if (!rtl8366rb_get_mib_counter(smi, i, val->port_vlan, &counter))
			len += snprintf(buf + len, sizeof(smi->buf) - len,
					"%llu\n", counter);
		else
			len += snprintf(buf + len, sizeof(smi->buf) - len,
					"%s\n", "error");
	}

	val->value.s = buf;
	val->len = len;
	return 0;
}

static int rtl8366rb_sw_get_vlan_ports(struct switch_dev *dev,
				      struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	struct switch_port *port;
	struct rtl8366_vlan_4k vlan4k;
	int i;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	rtl8366rb_get_vlan_4k(smi, val->port_vlan, &vlan4k);

	port = &val->value.ports[0];
	val->len = 0;
	for (i = 0; i < RTL8366RB_NUM_PORTS; i++) {
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

static int rtl8366rb_sw_set_vlan_ports(struct switch_dev *dev,
				      struct switch_val *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	struct switch_port *port;
	u32 member = 0;
	u32 untag = 0;
	int i;

	if (val->port_vlan == 0 || val->port_vlan >= RTL8366RB_NUM_VLANS)
		return -EINVAL;

	port = &val->value.ports[0];
	for (i = 0; i < val->len; i++, port++) {
		member |= BIT(port->id);

		if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED)))
			untag |= BIT(port->id);
	}

	return rtl8366_set_vlan(smi, val->port_vlan, member, untag, 0);
}

static int rtl8366rb_sw_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	return rtl8366_get_pvid(smi, port, val);
}

static int rtl8366rb_sw_set_port_pvid(struct switch_dev *dev, int port, int val)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	return rtl8366_set_pvid(smi, port, val);
}

static int rtl8366rb_sw_reset_switch(struct switch_dev *dev)
{
	struct rtl8366_smi *smi = sw_to_rtl8366_smi(dev);
	int err;

	err = rtl8366rb_reset_chip(smi);
	if (err)
		return err;

	err = rtl8366rb_hw_init(smi);
	if (err)
		return err;

	return rtl8366_reset_vlan(smi);
}

static struct switch_attr rtl8366rb_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = rtl8366rb_sw_set_vlan_enable,
		.get = rtl8366rb_sw_get_vlan_enable,
		.max = 1,
		.ofs = 1
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan4k",
		.description = "Enable VLAN 4K mode",
		.set = rtl8366rb_sw_set_vlan_enable,
		.get = rtl8366rb_sw_get_vlan_enable,
		.max = 1,
		.ofs = 2
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "reset_mibs",
		.description = "Reset all MIB counters",
		.set = rtl8366rb_sw_reset_mibs,
		.get = NULL,
		.max = 1
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "blinkrate",
		.description = "Get/Set LED blinking rate (0 = 43ms, 1 = 84ms,"
		" 2 = 120ms, 3 = 170ms, 4 = 340ms, 5 = 670ms)",
		.set = rtl8366rb_sw_set_blinkrate,
		.get = rtl8366rb_sw_get_blinkrate,
		.max = 5
	},
};

static struct switch_attr rtl8366rb_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "link",
		.description = "Get port link information",
		.max = 1,
		.set = NULL,
		.get = rtl8366rb_sw_get_port_link,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "reset_mib",
		.description = "Reset single port MIB counters",
		.max = 1,
		.set = rtl8366rb_sw_reset_port_mibs,
		.get = NULL,
	}, {
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.max = 33,
		.set = NULL,
		.get = rtl8366rb_sw_get_port_mib,
	}, {
		.type = SWITCH_TYPE_INT,
		.name = "led",
		.description = "Get/Set port group (0 - 3) led mode (0 - 15)",
		.max = 15,
		.set = rtl8366rb_sw_set_port_led,
		.get = rtl8366rb_sw_get_port_led,
	},
};

static struct switch_attr rtl8366rb_vlan[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "info",
		.description = "Get vlan information",
		.max = 1,
		.set = NULL,
		.get = rtl8366rb_sw_get_vlan_info,
	},
};

/* template */
static struct switch_dev rtl8366_switch_dev = {
	.name = "RTL8366S",
	.cpu_port = RTL8366RB_PORT_NUM_CPU,
	.ports = RTL8366RB_NUM_PORTS,
	.vlans = RTL8366RB_NUM_VLANS,
	.attr_global = {
		.attr = rtl8366rb_globals,
		.n_attr = ARRAY_SIZE(rtl8366rb_globals),
	},
	.attr_port = {
		.attr = rtl8366rb_port,
		.n_attr = ARRAY_SIZE(rtl8366rb_port),
	},
	.attr_vlan = {
		.attr = rtl8366rb_vlan,
		.n_attr = ARRAY_SIZE(rtl8366rb_vlan),
	},

	.get_vlan_ports = rtl8366rb_sw_get_vlan_ports,
	.set_vlan_ports = rtl8366rb_sw_set_vlan_ports,
	.get_port_pvid = rtl8366rb_sw_get_port_pvid,
	.set_port_pvid = rtl8366rb_sw_set_port_pvid,
	.reset_switch = rtl8366rb_sw_reset_switch,
};

static int rtl8366rb_switch_init(struct rtl8366rb *rtl)
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

static void rtl8366rb_switch_cleanup(struct rtl8366rb *rtl)
{
	unregister_switch(&rtl->dev);
}

static int rtl8366rb_mii_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 val = 0;
	int err;

	err = rtl8366rb_read_phy_reg(smi, addr, 0, reg, &val);
	if (err)
		return 0xffff;

	return val;
}

static int rtl8366rb_mii_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 t;
	int err;

	err = rtl8366rb_write_phy_reg(smi, addr, 0, reg, val);
	/* flush write */
	(void) rtl8366rb_read_phy_reg(smi, addr, 0, reg, &t);

	return err;
}

static int rtl8366rb_mii_bus_match(struct mii_bus *bus)
{
	return (bus->read == rtl8366rb_mii_read &&
		bus->write == rtl8366rb_mii_write);
}

static int rtl8366rb_setup(struct rtl8366rb *rtl)
{
	struct rtl8366_smi *smi = &rtl->smi;
	int ret;

	ret = rtl8366rb_reset_chip(smi);
	if (ret)
		return ret;

	ret = rtl8366rb_hw_init(smi);
	return ret;
}

static int rtl8366rb_detect(struct rtl8366_smi *smi)
{
	u32 chip_id = 0;
	u32 chip_ver = 0;
	int ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366RB_CHIP_ID_REG, &chip_id);
	if (ret) {
		dev_err(smi->parent, "unable to read chip id\n");
		return ret;
	}

	switch (chip_id) {
	case RTL8366RB_CHIP_ID_8366:
		break;
	default:
		dev_err(smi->parent, "unknown chip id (%04x)\n", chip_id);
		return -ENODEV;
	}

	ret = rtl8366_smi_read_reg(smi, RTL8366RB_CHIP_VERSION_CTRL_REG,
				   &chip_ver);
	if (ret) {
		dev_err(smi->parent, "unable to read chip version\n");
		return ret;
	}

	dev_info(smi->parent, "RTL%04x ver. %u chip found\n",
		 chip_id, chip_ver & RTL8366RB_CHIP_VERSION_MASK);

	return 0;
}

static struct rtl8366_smi_ops rtl8366rb_smi_ops = {
	.detect		= rtl8366rb_detect,
	.mii_read	= rtl8366rb_mii_read,
	.mii_write	= rtl8366rb_mii_write,

	.get_vlan_mc	= rtl8366rb_get_vlan_mc,
	.set_vlan_mc	= rtl8366rb_set_vlan_mc,
	.get_vlan_4k	= rtl8366rb_get_vlan_4k,
	.set_vlan_4k	= rtl8366rb_set_vlan_4k,
	.get_mc_index	= rtl8366rb_get_mc_index,
	.set_mc_index	= rtl8366rb_set_mc_index,
	.get_mib_counter = rtl8366rb_get_mib_counter,
};

static int __init rtl8366rb_probe(struct platform_device *pdev)
{
	static int rtl8366_smi_version_printed;
	struct rtl8366rb_platform_data *pdata;
	struct rtl8366rb *rtl;
	struct rtl8366_smi *smi;
	int err;

	if (!rtl8366_smi_version_printed++)
		printk(KERN_NOTICE RTL8366RB_DRIVER_DESC
		       " version " RTL8366RB_DRIVER_VER"\n");

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
	smi->ops = &rtl8366rb_smi_ops;
	smi->cpu_port = RTL8366RB_PORT_NUM_CPU;
	smi->num_ports = RTL8366RB_NUM_PORTS;
	smi->num_vlan_mc = RTL8366RB_NUM_VLANS;
	smi->mib_counters = rtl8366rb_mib_counters;
	smi->num_mib_counters = ARRAY_SIZE(rtl8366rb_mib_counters);

	err = rtl8366_smi_init(smi);
	if (err)
		goto err_free_rtl;

	platform_set_drvdata(pdev, rtl);

	err = rtl8366rb_setup(rtl);
	if (err)
		goto err_clear_drvdata;

	err = rtl8366rb_switch_init(rtl);
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

static int rtl8366rb_phy_config_init(struct phy_device *phydev)
{
	if (!rtl8366rb_mii_bus_match(phydev->bus))
		return -EINVAL;

	return 0;
}

static int rtl8366rb_phy_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver rtl8366rb_phy_driver = {
	.phy_id		= 0x001cc960,
	.name		= "Realtek RTL8366RB",
	.phy_id_mask	= 0x1ffffff0,
	.features	= PHY_GBIT_FEATURES,
	.config_aneg	= rtl8366rb_phy_config_aneg,
	.config_init    = rtl8366rb_phy_config_init,
	.read_status	= genphy_read_status,
	.driver		= {
		.owner = THIS_MODULE,
	},
};

static int __devexit rtl8366rb_remove(struct platform_device *pdev)
{
	struct rtl8366rb *rtl = platform_get_drvdata(pdev);

	if (rtl) {
		rtl8366rb_switch_cleanup(rtl);
		platform_set_drvdata(pdev, NULL);
		rtl8366_smi_cleanup(&rtl->smi);
		kfree(rtl);
	}

	return 0;
}

static struct platform_driver rtl8366rb_driver = {
	.driver = {
		.name		= RTL8366RB_DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe		= rtl8366rb_probe,
	.remove		= __devexit_p(rtl8366rb_remove),
};

static int __init rtl8366rb_module_init(void)
{
	int ret;
	ret = platform_driver_register(&rtl8366rb_driver);
	if (ret)
		return ret;

	ret = phy_driver_register(&rtl8366rb_phy_driver);
	if (ret)
		goto err_platform_unregister;

	return 0;

 err_platform_unregister:
	platform_driver_unregister(&rtl8366rb_driver);
	return ret;
}
module_init(rtl8366rb_module_init);

static void __exit rtl8366rb_module_exit(void)
{
	phy_driver_unregister(&rtl8366rb_phy_driver);
	platform_driver_unregister(&rtl8366rb_driver);
}
module_exit(rtl8366rb_module_exit);

MODULE_DESCRIPTION(RTL8366RB_DRIVER_DESC);
MODULE_VERSION(RTL8366RB_DRIVER_VER);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Antti Seppälä <a.seppala@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" RTL8366RB_DRIVER_NAME);
