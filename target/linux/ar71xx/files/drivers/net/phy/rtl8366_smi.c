/*
 * Platform driver for the Realtek RTL8366 ethernet switch
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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
#include <linux/gpio.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/switch.h>
#include <linux/phy.h>
#include <linux/rtl8366_smi.h>

/* #define DEBUG 1 */

#ifdef DEBUG
#include <linux/debugfs.h>
#endif

#define RTL8366_SMI_DRIVER_NAME	"rtl8366-smi"
#define RTL8366_SMI_DRIVER_DESC	"Realtek RTL8366 switch driver"
#define RTL8366_SMI_DRIVER_VER	"0.1.1"

#define RTL8366S_PHY_NO_MAX                 4
#define RTL8366S_PHY_PAGE_MAX               7
#define RTL8366S_PHY_ADDR_MAX               31

#define RTL8366_CHIP_GLOBAL_CTRL_REG        0x0000
#define RTL8366_CHIP_CTRL_VLAN              (1 << 13)

#define RTL8366_RESET_CTRL_REG              0x0100
#define RTL8366_CHIP_CTRL_RESET_HW          1
#define RTL8366_CHIP_CTRL_RESET_SW          (1 << 1)

#define RTL8366S_CHIP_VERSION_CTRL_REG      0x0104
#define RTL8366S_CHIP_VERSION_MASK          0xf
#define RTL8366S_CHIP_ID_REG                0x0105
#define RTL8366S_CHIP_ID_8366               0x8366

/* PHY registers control */
#define RTL8366S_PHY_ACCESS_CTRL_REG        0x8028
#define RTL8366S_PHY_ACCESS_DATA_REG        0x8029

#define RTL8366S_PHY_CTRL_READ              1
#define RTL8366S_PHY_CTRL_WRITE             0

#define RTL8366S_PHY_REG_MASK               0x1f
#define RTL8366S_PHY_PAGE_OFFSET            5
#define RTL8366S_PHY_PAGE_MASK              (0x7 << 5)
#define RTL8366S_PHY_NO_OFFSET              9
#define RTL8366S_PHY_NO_MASK                (0x1f << 9)

#define RTL8366_SMI_ACK_RETRY_COUNT         5
#define RTL8366_SMI_CLK_DELAY               10 /* nsec */

/* LED control registers */
#define RTL8366_LED_BLINKRATE_REG           0x0420
#define RTL8366_LED_BLINKRATE_BIT           0
#define RTL8366_LED_BLINKRATE_MASK          0x0007

#define RTL8366_LED_CTRL_REG                0x0421
#define RTL8366_LED_0_1_CTRL_REG            0x0422
#define RTL8366_LED_2_3_CTRL_REG            0x0423

#define RTL8366S_MIB_COUNT                  33
#define RTL8366S_GLOBAL_MIB_COUNT           1
#define RTL8366S_MIB_COUNTER_PORT_OFFSET    0x0040
#define RTL8366S_MIB_COUNTER_BASE           0x1000
#define RTL8366S_MIB_CTRL_REG               0x11F0
#define RTL8366S_MIB_CTRL_USER_MASK         0x01FF
#define RTL8366S_MIB_CTRL_BUSY_MASK         0x0001
#define RTL8366S_MIB_CTRL_RESET_MASK        0x0002

#define RTL8366S_MIB_CTRL_GLOBAL_RESET_MASK 0x0004
#define RTL8366S_MIB_CTRL_PORT_RESET_BIT    0x0003
#define RTL8366S_MIB_CTRL_PORT_RESET_MASK   0x01FC


#define RTL8366S_PORT_VLAN_CTRL_BASE        0x0058
#define RTL8366S_VLAN_TABLE_READ_BASE       0x018B
#define RTL8366S_VLAN_TABLE_WRITE_BASE      0x0185

#define RTL8366S_VLAN_TB_CTRL_REG           0x010F

#define RTL8366S_TABLE_ACCESS_CTRL_REG      0x0180
#define RTL8366S_TABLE_VLAN_READ_CTRL       0x0E01
#define RTL8366S_TABLE_VLAN_WRITE_CTRL      0x0F01

#define RTL8366S_VLAN_MEMCONF_BASE          0x0016


#define RTL8366S_PORT_LINK_STATUS_BASE      0x0060
#define RTL8366S_PORT_STATUS_SPEED_MASK     0x0003
#define RTL8366S_PORT_STATUS_DUPLEX_MASK    0x0004
#define RTL8366S_PORT_STATUS_LINK_MASK      0x0010
#define RTL8366S_PORT_STATUS_TXPAUSE_MASK   0x0020
#define RTL8366S_PORT_STATUS_RXPAUSE_MASK   0x0040
#define RTL8366S_PORT_STATUS_AN_MASK        0x0080


#define RTL8366_PORT_NUM_CPU                5
#define RTL8366_NUM_PORTS                   6
#define RTL8366_NUM_VLANS                   16
#define RTL8366_NUM_LEDGROUPS               4
#define RTL8366_NUM_VIDS                    4096
#define RTL8366S_PRIORITYMAX                7
#define RTL8366S_FIDMAX	                    7


#define RTL8366_PORT_1                      (1 << 0) /* In userspace port 0 */
#define RTL8366_PORT_2                      (1 << 1) /* In userspace port 1 */
#define RTL8366_PORT_3                      (1 << 2) /* In userspace port 2 */
#define RTL8366_PORT_4                      (1 << 3) /* In userspace port 3 */

#define RTL8366_PORT_UNKNOWN                (1 << 4) /* No known connection */
#define RTL8366_PORT_CPU                    (1 << 5) /* CPU port */

#define RTL8366_PORT_ALL                    (RTL8366_PORT_1 |       \
					     RTL8366_PORT_2 |       \
					     RTL8366_PORT_3 |       \
					     RTL8366_PORT_4 |       \
					     RTL8366_PORT_UNKNOWN | \
					     RTL8366_PORT_CPU)

#define RTL8366_PORT_ALL_BUT_CPU            (RTL8366_PORT_1 |       \
					     RTL8366_PORT_2 |       \
					     RTL8366_PORT_3 |       \
					     RTL8366_PORT_4 |       \
					     RTL8366_PORT_UNKNOWN)

#define RTL8366_PORT_ALL_EXTERNAL           (RTL8366_PORT_1 |       \
					     RTL8366_PORT_2 |       \
					     RTL8366_PORT_3 |       \
					     RTL8366_PORT_4)

#define RTL8366_PORT_ALL_INTERNAL           (RTL8366_PORT_UNKNOWN | \
					     RTL8366_PORT_CPU)

struct rtl8366s_vlanconfig {
	u16 	reserved2:1;
	u16 	priority:3;
	u16 	vid:12;

	u16 	reserved1:1;
	u16 	fid:3;
	u16 	untag:6;
	u16 	member:6;
};

struct rtl8366s_vlan4kentry {
	u16 	reserved1:4;
	u16 	vid:12;

	u16 	reserved2:1;
	u16 	fid:3;
	u16 	untag:6;
	u16 	member:6;
};

static const char *MIBCOUNTERS[] = { "IfInOctets                        ",
				     "EtherStatsOctets                  ",
				     "EtherStatsUnderSizePkts           ",
				     "EtherFregament                    ",
				     "EtherStatsPkts64Octets            ",
				     "EtherStatsPkts65to127Octets       ",
				     "EtherStatsPkts128to255Octets      ",
				     "EtherStatsPkts256to511Octets      ",
				     "EtherStatsPkts512to1023Octets     ",
				     "EtherStatsPkts1024to1518Octets    ",
				     "EtherOversizeStats                ",
				     "EtherStatsJabbers                 ",
				     "IfInUcastPkts                     ",
				     "EtherStatsMulticastPkts           ",
				     "EtherStatsBroadcastPkts           ",
				     "EtherStatsDropEvents              ",
				     "Dot3StatsFCSErrors                ",
				     "Dot3StatsSymbolErrors             ",
				     "Dot3InPauseFrames                 ",
				     "Dot3ControlInUnknownOpcodes       ",
				     "IfOutOctets                       ",
				     "Dot3StatsSingleCollisionFrames    ",
				     "Dot3StatMultipleCollisionFrames   ",
				     "Dot3sDeferredTransmissions        ",
				     "Dot3StatsLateCollisions           ",
				     "EtherStatsCollisions              ",
				     "Dot3StatsExcessiveCollisions      ",
				     "Dot3OutPauseFrames                ",
				     "Dot1dBasePortDelayExceededDiscards",
				     "Dot1dTpPortInDiscards             ",
				     "IfOutUcastPkts                    ",
				     "IfOutMulticastPkts                ",
				     "IfOutBroadcastPkts                ",
				     NULL };

struct rtl8366_smi {
	struct platform_device		   *pdev;
	struct rtl8366_smi_platform_data   *pdata;
	spinlock_t			   lock;
	struct mii_bus			   *mii_bus;
	struct switch_dev                  dev;
	int				   mii_irq[PHY_MAX_ADDR];
#ifdef DEBUG
	struct dentry                      *debugfs_root;
#endif
};

#ifdef DEBUG
u16 g_dbg_reg;
#endif

#define to_rtl8366(_dev) container_of(_dev, struct rtl8366_smi, dev)

static inline void rtl8366_smi_clk_delay(struct rtl8366_smi *smi)
{
	ndelay(RTL8366_SMI_CLK_DELAY);
}

static void rtl8366_smi_start(struct rtl8366_smi *smi)
{
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

	/*
	 * Set GPIO pins to output mode, with initial state:
	 * SCK = 0, SDA = 1
	 */
	gpio_direction_output(sck, 0);
	gpio_direction_output(sda, 1);
	rtl8366_smi_clk_delay(smi);

	/* CLK 1: 0 -> 1, 1 -> 0 */
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);

	/* CLK 2: */
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
}

static void rtl8366_smi_stop(struct rtl8366_smi *smi)
{
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* add a click */
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8366_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* set GPIO pins to input mode */
	gpio_direction_input(sda);
	gpio_direction_input(sck);
}

static void rtl8366_smi_write_bits(struct rtl8366_smi *smi, u32 data, u32 len)
{
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

	for (; len > 0; len--) {
		rtl8366_smi_clk_delay(smi);

		/* prepare data */
		if ( data & ( 1 << (len - 1)) )
			gpio_set_value(sda, 1);
		else
			gpio_set_value(sda, 0);
		rtl8366_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8366_smi_clk_delay(smi);
		gpio_set_value(sck, 0);
	}
}

static void rtl8366_smi_read_bits(struct rtl8366_smi *smi, u32 len, u32 *data)
{
	unsigned int sda = smi->pdata->gpio_sda;
	unsigned int sck = smi->pdata->gpio_sck;

	gpio_direction_input(sda);

	for (*data = 0; len > 0; len--) {
		u32 u;

		rtl8366_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8366_smi_clk_delay(smi);
		u = gpio_get_value(sda);
		gpio_set_value(sck, 0);

		*data |= (u << (len - 1));
	}

	gpio_direction_output(sda, 0);
}

static int rtl8366_smi_wait_for_ack(struct rtl8366_smi *smi)
{
	int retry_cnt;

	retry_cnt = 0;
	do {
		u32 ack;

		rtl8366_smi_read_bits(smi, 1, &ack);
		if (ack == 0)
			break;

		if (++retry_cnt > RTL8366_SMI_ACK_RETRY_COUNT)
			return -EIO;
	} while (1);

	return 0;
}

static int rtl8366_smi_write_byte(struct rtl8366_smi *smi, u8 data)
{
	rtl8366_smi_write_bits(smi, data, 8);
	return rtl8366_smi_wait_for_ack(smi);
}

static int rtl8366_smi_read_byte0(struct rtl8366_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8366_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8366_smi_write_bits(smi, 0x00, 1);

	return 0;
}

static int rtl8366_smi_read_byte1(struct rtl8366_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8366_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8366_smi_write_bits(smi, 0x01, 1);

	return 0;
}

static int rtl8366_smi_read_reg(struct rtl8366_smi *smi, u32 addr, u32 *data)
{
	unsigned long flags;
	u8 lo = 0;
	u8 hi = 0;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	rtl8366_smi_start(smi);

	/* send READ command */
	ret = rtl8366_smi_write_byte(smi, 0x0a << 4 | 0x04 << 1 | 0x01);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8366_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8366_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* read DATA[7:0] */
	rtl8366_smi_read_byte0(smi, &lo);
	/* read DATA[15:8] */
	rtl8366_smi_read_byte1(smi, &hi);

	*data = ((u32) lo) | (((u32) hi) << 8);

	ret = 0;

 out:
	rtl8366_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}

static int rtl8366_smi_write_reg(struct rtl8366_smi *smi, u32 addr, u32 data)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&smi->lock, flags);

	rtl8366_smi_start(smi);

	/* send WRITE command */
	ret = rtl8366_smi_write_byte(smi, 0x0a << 4 | 0x04 << 1 | 0x00);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8366_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8366_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* write DATA[7:0] */
	ret = rtl8366_smi_write_byte(smi, data & 0xff);
	if (ret)
		goto out;

	/* write DATA[15:8] */
	ret = rtl8366_smi_write_byte(smi, data >> 8);
	if (ret)
		goto out;

	ret = 0;

 out:
	rtl8366_smi_stop(smi);
	spin_unlock_irqrestore(&smi->lock, flags);

	return ret;
}

static int rtl8366_smi_read_phy_reg(struct rtl8366_smi *smi,
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

static int rtl8366_smi_write_phy_reg(struct rtl8366_smi *smi,
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
	u32 addr, data, regoffset;
	u64 mibvalue;

	/* address offset to MIBs counter */
	const u16 mibLength[RTL8366S_MIB_COUNT] = {4, 4, 2, 2, 2, 2, 2, 2, 2,
						   2, 2, 2, 2, 2, 2, 2, 2, 2,
						   2, 2, 4, 2, 2, 2, 2, 2, 2,
						   2, 2, 2, 2, 2, 2};

	if (port > RTL8366_NUM_PORTS || counter >= RTL8366S_MIB_COUNT)
		return -EINVAL;

	i = 0;
	regoffset = RTL8366S_MIB_COUNTER_PORT_OFFSET * (port);

	while (i < counter) {
		regoffset += mibLength[i];
		i++;
	}

	addr = RTL8366S_MIB_COUNTER_BASE + regoffset;


	/* writing access counter address first */
	/* then ASIC will prepare 64bits counter wait for being retrived */
	data = 0;/* writing data will be discard by ASIC */
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
	addr = addr + mibLength[counter] - 1;
	i = mibLength[counter];

	while (i) {
		err = rtl8366_smi_read_reg(smi, addr, &data);
		if (err)
			return err;

		mibvalue = (mibvalue << 16) | (data & 0xFFFF);

		addr--;
		i--;
	}

	*val = mibvalue;
	return 0;
}

static int rtl8366s_get_vlan_4k_entry(struct rtl8366_smi *smi, u32 vid,
				      struct rtl8366s_vlan4kentry *vlan4k)
{
	int err;
	u32 data;
	u16 *tableaddr;

	memset(vlan4k, '\0', sizeof(struct rtl8366s_vlan4kentry));
	vlan4k->vid = vid;

	if (vid >= RTL8366_NUM_VIDS)
		return -EINVAL;

	tableaddr = (u16 *)vlan4k;

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

	return 0;
}

static int rtl8366s_set_vlan_4k_entry(struct rtl8366_smi *smi,
				      const struct rtl8366s_vlan4kentry *vlan4k)
{
	int err;
	u32 data;
	u16 *tableaddr;

	if (vlan4k->vid >= RTL8366_NUM_VIDS ||
	    vlan4k->member > RTL8366_PORT_ALL ||
	    vlan4k->untag > RTL8366_PORT_ALL ||
	    vlan4k->fid > RTL8366S_FIDMAX)
		return -EINVAL;

	tableaddr = (u16 *)vlan4k;

	data = *tableaddr;

	err = rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TABLE_WRITE_BASE, data);
	if (err)
		return err;

	tableaddr++;

	data = *tableaddr;

	rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TABLE_WRITE_BASE + 1, data);


	/* write table access control word */
	err = rtl8366_smi_write_reg(smi, RTL8366S_TABLE_ACCESS_CTRL_REG,
				    RTL8366S_TABLE_VLAN_WRITE_CTRL);
	if (err)
		return err;

	return 0;
}

static int rtl8366s_get_vlan_member_config(struct rtl8366_smi *smi, u32 index,
					   struct rtl8366s_vlanconfig
					   *vlanmconf)
{
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	if (index >= RTL8366_NUM_VLANS)
		return -EINVAL;

	tableaddr = (u16 *)vlanmconf;

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

	return 0;
}

static int rtl8366s_set_vlan_member_config(struct rtl8366_smi *smi, u32 index,
					   const struct rtl8366s_vlanconfig
					   *vlanmconf)
{
	int err;
	u32 addr;
	u32 data;
	u16 *tableaddr;

	if (index >= RTL8366_NUM_VLANS ||
	   vlanmconf->vid >= RTL8366_NUM_VIDS ||
	   vlanmconf->priority > RTL8366S_PRIORITYMAX ||
	   vlanmconf->member > RTL8366_PORT_ALL ||
	   vlanmconf->untag > RTL8366_PORT_ALL ||
	   vlanmconf->fid > RTL8366S_FIDMAX)
		return -EINVAL;

	addr = RTL8366S_VLAN_MEMCONF_BASE + (index << 1);


	tableaddr = (u16 *)vlanmconf;
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

static int rtl8366_get_port_vlan_index(struct rtl8366_smi *smi, int port,
				       int *val)
{
	int err;
	u32 addr;
	u32 data;

	/* bits mapping to port vlan control register of port n */
	const u16 bits[RTL8366_NUM_PORTS] = { 0x000F, 0x00F0, 0x0F00,
					     0xF000, 0x000F, 0x00F0 };
	/* bits offset to port vlan control register of port n */
	const u16 bitoffset[RTL8366_NUM_PORTS] = { 0, 4, 8, 12, 0, 4 };
	/* address offset to port vlan control register of port n */
	const u16 addroffset[RTL8366_NUM_PORTS] = { 0, 0, 0, 0, 1, 1 };

	if (port >= RTL8366_NUM_PORTS)
		return -EINVAL;

	addr = RTL8366S_PORT_VLAN_CTRL_BASE + addroffset[port];

	err = rtl8366_smi_read_reg(smi, addr, &data);
	if (err)
		return err;

	*val = (data & bits[port]) >> bitoffset[port];

	return 0;

}

static int rtl8366_get_vlan_port_pvid(struct rtl8366_smi *smi, int port,
				      int *val)
{
	int err;
	int index;
	struct rtl8366s_vlanconfig vlanMC;

	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	err = rtl8366_get_port_vlan_index(smi, port, &index);
	if (err)
		return err;

	err = rtl8366s_get_vlan_member_config(smi, index, &vlanMC);
	if (err)
		return err;

	*val = vlanMC.vid;
	return 0;
}

static int rtl8366_set_port_vlan_index(struct rtl8366_smi *smi, int port,
				       int index)
{
	int err;
	u32 addr;
	u32 data;
	u32 vlan_data;
	u32 bits;

	/* bits mapping to port vlan control register of port n */
	const u16 bitmasks[6] = { 0x000F, 0x00F0, 0x0F00,
				 0xF000, 0x000F, 0x00F0 };
	/* bits offset to port vlan control register of port n */
	const u16 bitOff[6] = { 0, 4, 8, 12, 0, 4 };
	/* address offset to port vlan control register of port n */
	const u16 addrOff[6] = { 0, 0, 0, 0, 1, 1 };

	if (port >= RTL8366_NUM_PORTS || index >= RTL8366_NUM_VLANS)
		return -EINVAL;

	addr = RTL8366S_PORT_VLAN_CTRL_BASE + addrOff[port];

	bits = bitmasks[port];

	data = (index << bitOff[port]) & bits;

	err = rtl8366_smi_read_reg(smi, addr, &vlan_data);
	if (err)
		return err;

	vlan_data &= ~(vlan_data & bits);
	vlan_data |= data;

	err = rtl8366_smi_write_reg(smi, addr, vlan_data);
	if (err)
		return err;

	return 0;
}

static int rtl8366_set_vlan_port_pvid(struct rtl8366_smi *smi, int port,
				      int val)
{
	int i;
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;

	if (port >= RTL8366_NUM_PORTS || val >= RTL8366_NUM_VIDS)
		return -EINVAL;



	/* Updating the 4K entry; lookup it and change the port member set */
	rtl8366s_get_vlan_4k_entry(smi, val, &vlan4K);
	vlan4K.member |= ((1 << port) | RTL8366_PORT_CPU);
	vlan4K.untag = RTL8366_PORT_ALL_BUT_CPU;
	rtl8366s_set_vlan_4k_entry(smi, &vlan4K);

	/* For the 16 entries more work needs to be done. First see if such
	   VID is already there and change it */
	for (i = 0; i < RTL8366_NUM_VLANS; ++i) {
		memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));
		rtl8366s_get_vlan_member_config(smi, i, &vlanMC);

		/* Try to find an existing vid and update port member set */
		if (val == vlanMC.vid) {
			vlanMC.member |= ((1 << port) | RTL8366_PORT_CPU);
			rtl8366s_set_vlan_member_config(smi, i, &vlanMC);

			/* Now update PVID register settings */
			rtl8366_set_port_vlan_index(smi, port, i);

			return 0;
		}
	}

	/* PVID could not be found from vlan table. Replace unused (one that
	   has no member ports) with new one */
	for (i = 0; i < RTL8366_NUM_VLANS; ++i) {
		memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));
		rtl8366s_get_vlan_member_config(smi, i, &vlanMC);

		/* See if this vlan member configuration is unused. It is
		   unused if member set contains no ports or CPU port only */
		if (!vlanMC.member || vlanMC.member == RTL8366_PORT_CPU) {
			vlanMC.vid = val;
			vlanMC.priority = 0;
			vlanMC.untag = RTL8366_PORT_ALL_BUT_CPU;
			vlanMC.member = ((1 << port) | RTL8366_PORT_CPU);
			vlanMC.fid = 0;

			rtl8366s_set_vlan_member_config(smi, i, &vlanMC);

			/* Now update PVID register settings */
			rtl8366_set_port_vlan_index(smi, port, i);

			return 0;
		}
	}

	dev_err(&smi->pdev->dev, "All 16 vlan member configurations are in "
		"use\n");
	return -EINVAL;
}


static int rtl8366_vlan_set_vlan(struct rtl8366_smi *smi, int enable)
{
	u32 data = 0;
	rtl8366_smi_read_reg(smi, RTL8366_CHIP_GLOBAL_CTRL_REG, &data);

	data &= ~(data & RTL8366_CHIP_CTRL_VLAN);
	if (enable)
		data |= RTL8366_CHIP_CTRL_VLAN;

	return rtl8366_smi_write_reg(smi, RTL8366_CHIP_GLOBAL_CTRL_REG, data);
}

static int rtl8366_vlan_set_4ktable(struct rtl8366_smi *smi, int enable)
{
	u32 data = 0;
	rtl8366_smi_read_reg(smi, RTL8366S_VLAN_TB_CTRL_REG, &data);

	data &= ~(data & 1);
	if (enable)
		data |= 1;

	return rtl8366_smi_write_reg(smi, RTL8366S_VLAN_TB_CTRL_REG, data);
}

static int rtl8366s_reset_vlan(struct rtl8366_smi *smi)
{
	int i;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366s_vlanconfig vlanMC;

	/* clear 16 VLAN member configuration */
	for (i = 0; i < RTL8366_NUM_VLANS; i++) {
		vlanMC.vid = 0;
		vlanMC.priority = 0;
		vlanMC.member = 0;
		vlanMC.untag = 0;
		vlanMC.fid = 0;
		if (rtl8366s_set_vlan_member_config(smi, i, &vlanMC) != 0)
			return -EIO;
	}

	/* Set a default VLAN with vid 1 to 4K table for all ports */
	vlan4K.vid = 1;
	vlan4K.member = RTL8366_PORT_ALL;
	vlan4K.untag = RTL8366_PORT_ALL;
	vlan4K.fid = 0;
	if (rtl8366s_set_vlan_4k_entry(smi, &vlan4K) != 0)
		return -EIO;

	/* Set all ports PVID to default VLAN */
	for (i = 0; i < RTL8366_NUM_PORTS; i++) {
		if (rtl8366_set_vlan_port_pvid(smi, i, 0) != 0)
			return -EIO;
	}

	return 0;
}

#ifdef DEBUG
static int rtl8366_debugfs_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static ssize_t rtl8366_read_debugfs_mibs(struct file *file,
					 char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	char buf[4096];
	int i, j, len = 0;
	struct rtl8366_smi *smi = (struct rtl8366_smi *)file->private_data;

	len += snprintf(buf + len, sizeof(buf) - len, "MIB Counters:\n");
	len += snprintf(buf + len, sizeof(buf) - len, "Counter"
			"                            "
			"Port 0 \t\t Port 1 \t\t Port 2 \t\t Port 3 \t\t "
			"Port 4\n");

	for (i = 0; i < 33; ++i) {

		len += snprintf(buf + len, sizeof(buf) - len, "%d:%s ",
				i, MIBCOUNTERS[i]);
		for (j = 0; j < RTL8366_NUM_PORTS; ++j) {
			unsigned long long counter = 0;

			if (!rtl8366_get_mib_counter(smi, i, j, &counter))
				len += snprintf(buf + len, sizeof(buf) - len,
						"[%llu]", counter);
			else
				len += snprintf(buf + len, sizeof(buf) - len,
						"[error]");

			if (j != RTL8366_NUM_PORTS - 1) {
				if (counter < 100000)
					len += snprintf(buf + len,
							sizeof(buf) - len,
							"\t");

				len += snprintf(buf + len, sizeof(buf) - len,
						"\t");
			}
		}
		len += snprintf(buf + len, sizeof(buf) - len, "\n");
	}

	len += snprintf(buf + len, sizeof(buf) - len, "\n");

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366_read_debugfs_vlan(struct file *file,
					 char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	char buf[4096];
	int i, j, len = 0;
	struct rtl8366_smi *smi = (struct rtl8366_smi *)file->private_data;

	len += snprintf(buf + len, sizeof(buf) - len, "VLAN Member Config:\n");
	len += snprintf(buf + len, sizeof(buf) - len,
			"\t id \t vid \t prio \t member \t untag  \t fid "
			"\tports\n");

	for (i = 0; i < RTL8366_NUM_VLANS; ++i) {
		struct rtl8366s_vlanconfig vlanMC;
		memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));
		rtl8366s_get_vlan_member_config(smi, i, &vlanMC);

		len += snprintf(buf + len, sizeof(buf) - len,
				"\t[%d] \t %d \t %d \t 0x%04x \t 0x%04x \t %d "
				"\t", i, vlanMC.vid, vlanMC.priority,
				vlanMC.member, vlanMC.untag, vlanMC.fid);

		for (j = 0; j < RTL8366_NUM_PORTS; ++j) {
			int index = 0;
			if (!rtl8366_get_port_vlan_index(smi, j, &index)) {
				if (index == i)
					len += snprintf(buf + len,
							sizeof(buf) - len,
							"%d", j);
			}
		}
		len += snprintf(buf + len, sizeof(buf) - len, "\n");
	}

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366_read_debugfs_reg(struct file *file,
					char __user *user_buf,
					size_t count, loff_t *ppos)
{
	u32 t, reg = g_dbg_reg;
	int err, len = 0;
	char buf[512];
	struct rtl8366_smi *smi = (struct rtl8366_smi *)file->private_data;

	memset(buf, '\0', sizeof(buf));

	err = rtl8366_smi_read_reg(smi, reg, &t);
	if (err) {
		len += snprintf(buf, sizeof(buf),
				"Read failed (reg: 0x%04x)\n", reg);
		return simple_read_from_buffer(user_buf, count, ppos, buf, len);
	}

	len += snprintf(buf, sizeof(buf), "reg = 0x%04x, val = 0x%04x\n",
			reg, t);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t rtl8366_write_debugfs_reg(struct file *file,
					 const char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	unsigned long data;
	u32 reg = g_dbg_reg;
	int err;
	char buf[50];
	size_t len;
	struct rtl8366_smi *smi = (struct rtl8366_smi *)file->private_data;

	len = min(count, sizeof(buf) - 1);
	if (copy_from_user(buf, user_buf, len)) {
		dev_err(&smi->pdev->dev, "copy from user failed\n");
		return -EFAULT;
	}

	buf[len] = '\0';
	if (len > 0 && buf[len - 1] == '\n')
		buf[len - 1] = '\0';


	if (strict_strtoul(buf, 16, &data)) {
		dev_err(&smi->pdev->dev, "Invalid reg value %s\n", buf);
	} else {
		err = rtl8366_smi_write_reg(smi, reg, data);
		if (err) {
			dev_err(&smi->pdev->dev,
				"writing reg 0x%04x val 0x%04lx failed\n",
				reg, data);
		}
	}

	return count;
}

static const struct file_operations fops_rtl8366_regs = {
	.read = rtl8366_read_debugfs_reg,
	.write = rtl8366_write_debugfs_reg,
	.open = rtl8366_debugfs_open,
	.owner = THIS_MODULE
};

static const struct file_operations fops_rtl8366_vlan = {
	.read = rtl8366_read_debugfs_vlan,
	.open = rtl8366_debugfs_open,
	.owner = THIS_MODULE
};

static const struct file_operations fops_rtl8366_mibs = {
	.read = rtl8366_read_debugfs_mibs,
	.open = rtl8366_debugfs_open,
	.owner = THIS_MODULE
};

static void rtl8366_debugfs_init(struct rtl8366_smi *smi)
{
	struct dentry *node;
	struct dentry *root;

	if (!smi->debugfs_root)
		smi->debugfs_root = debugfs_create_dir("rtl8366s", NULL);

	if (!smi->debugfs_root) {
		dev_err(&smi->pdev->dev, "Unable to create debugfs dir\n");
		return;
	}
	root = smi->debugfs_root;

	node = debugfs_create_x16("reg", S_IRUGO | S_IWUSR, root, &g_dbg_reg);
	if (!node) {
		dev_err(&smi->pdev->dev, "Creating debugfs file reg failed\n");
		return;
	}

	node = debugfs_create_file("val", S_IRUGO | S_IWUSR, root, smi,
				   &fops_rtl8366_regs);
	if (!node) {
		dev_err(&smi->pdev->dev, "Creating debugfs file val failed\n");
		return;
	}

	node = debugfs_create_file("vlan", S_IRUSR, root, smi,
				   &fops_rtl8366_vlan);
	if (!node) {
		dev_err(&smi->pdev->dev, "Creating debugfs file vlan "
			"failed\n");
		return;
	}

	node = debugfs_create_file("mibs", S_IRUSR, root, smi,
				   &fops_rtl8366_mibs);
	if (!node) {
		dev_err(&smi->pdev->dev, "Creating debugfs file mibs "
			"xfailed\n");
		return;
	}
}

static void rtl8366_debugfs_remove(struct rtl8366_smi *smi)
{
	if (smi->debugfs_root) {
		debugfs_remove_recursive(smi->debugfs_root);
		smi->debugfs_root = NULL;
	}
}

#else
static inline void rtl8366_debugfs_init(struct rtl8366_smi *smi) {}
static inline void rtl8366_debugfs_remove(struct rtl8366_smi *smi) {}
#endif

static int rtl8366_global_reset_mibs(struct switch_dev *dev,
				     const struct switch_attr *attr,
				     struct switch_val *val)
{
	u32 data = 0;
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (val->value.i == 1) {
		rtl8366_smi_read_reg(smi, RTL8366S_MIB_CTRL_REG, &data);
		data |= (1 << 2);
		rtl8366_smi_write_reg(smi, RTL8366S_MIB_CTRL_REG, data);
	}

	return 0;
}

static int rtl8366_get_vlan(struct switch_dev *dev,
			    const struct switch_attr *attr,
			    struct switch_val *val)
{
	u32 data;
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (attr->ofs == 1) {
		rtl8366_smi_read_reg(smi, RTL8366_CHIP_GLOBAL_CTRL_REG, &data);

		if (data & RTL8366_CHIP_CTRL_VLAN)
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

static int rtl8366_global_get_blinkrate(struct switch_dev *dev,
					const struct switch_attr *attr,
					struct switch_val *val)
{
	u32 data;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	rtl8366_smi_read_reg(smi, RTL8366_LED_BLINKRATE_REG, &data);

	val->value.i = (data & (RTL8366_LED_BLINKRATE_MASK));

	return 0;
}

static int rtl8366_global_set_blinkrate(struct switch_dev *dev,
					const struct switch_attr *attr,
					struct switch_val *val)
{
	u32 data;
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (val->value.i >= 6)
		return -EINVAL;

	rtl8366_smi_read_reg(smi, RTL8366_LED_BLINKRATE_REG, &data);

	data &= ~(data & RTL8366_LED_BLINKRATE_MASK);
	data |= val->value.i;

	rtl8366_smi_write_reg(smi, RTL8366_LED_BLINKRATE_REG, data);

	return 0;
}

static int rtl8366_set_vlan(struct switch_dev *dev,
			    const struct switch_attr *attr,
			    struct switch_val *val)
{
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (attr->ofs == 1)
		return rtl8366_vlan_set_vlan(smi, val->value.i);
	else
		return rtl8366_vlan_set_4ktable(smi, val->value.i);
}

static int rtl8366_init_vlan(struct switch_dev *dev,
			     const struct switch_attr *attr,
			     struct switch_val *val)
{
	struct rtl8366_smi *smi = to_rtl8366(dev);
	return rtl8366s_reset_vlan(smi);
}

static int rtl8366_attr_get_port_link(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	char buf[1024];
	u32 len = 0, data = 0;
	int speed, duplex, link, txpause, rxpause, nway;
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (val->port_vlan >= RTL8366_NUM_PORTS)
		return -EINVAL;

	memset(buf, '\0', sizeof(buf));
	rtl8366_smi_read_reg(smi, RTL8366S_PORT_LINK_STATUS_BASE +
			     (val->port_vlan >> 1),
			     &data);

	if (val->port_vlan & 0x1)
		data = data >> 8;

	speed = (data & RTL8366S_PORT_STATUS_SPEED_MASK);
	duplex = (data & RTL8366S_PORT_STATUS_DUPLEX_MASK) >> 2;
	link = (data & RTL8366S_PORT_STATUS_LINK_MASK) >> 4;
	txpause = (data & RTL8366S_PORT_STATUS_TXPAUSE_MASK) >> 5;
	rxpause = (data & RTL8366S_PORT_STATUS_RXPAUSE_MASK) >> 6;
	nway = (data & RTL8366S_PORT_STATUS_AN_MASK) >> 7;

	len += snprintf(buf + len, sizeof(buf) - len, "Port %d: ",
			val->port_vlan);

	if (link)
		len += snprintf(buf + len, sizeof(buf) - len,
				"Link UP, Speed: ");
	else
		len += snprintf(buf + len, sizeof(buf) - len,
				"Link DOWN, Speed: ");

	if (speed == 0)
		len += snprintf(buf + len, sizeof(buf) - len, "10Base-TX ");
	else if (speed == 1)
		len += snprintf(buf + len, sizeof(buf) - len, "100Base-TX ");
	else if (speed == 2)
		len += snprintf(buf + len, sizeof(buf) - len, "1000Base-TX ");

	if (duplex)
		len += snprintf(buf + len, sizeof(buf) - len, "Full-Duplex, ");
	else
		len += snprintf(buf + len, sizeof(buf) - len, "Half-Duplex, ");

	if (txpause)
		len += snprintf(buf + len, sizeof(buf) - len, "TX-Pause ");
	if (rxpause)
		len += snprintf(buf + len, sizeof(buf) - len, "RX-Pause ");
	if (nway)
		len += snprintf(buf + len, sizeof(buf) - len, "nway ");

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int rtl8366_attr_get_vlan_info(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	int i;
	char buf[1024];
	u32 len = 0;
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366_smi *smi = to_rtl8366(dev);

	if (val->port_vlan >= RTL8366_NUM_PORTS)
		return -EINVAL;

	memset(buf, '\0', sizeof(buf));
	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	rtl8366s_get_vlan_member_config(smi, val->port_vlan, &vlanMC);
	rtl8366s_get_vlan_4k_entry(smi, vlanMC.vid, &vlan4K);

	len += snprintf(buf + len, sizeof(buf) - len, "VLAN %d: Ports: ",
			val->port_vlan);

	for (i = 0; i < RTL8366_NUM_PORTS; ++i) {
		int index = 0;
		if (!rtl8366_get_port_vlan_index(smi, i, &index) &&
		    index == val->port_vlan)
			len += snprintf(buf + len, sizeof(buf) - len, "%d", i);
	}
	len += snprintf(buf + len, sizeof(buf) - len, "\n");

	len += snprintf(buf + len, sizeof(buf) - len,
			"\t\t vid \t prio \t member \t untag \t fid\n");
	len += snprintf(buf + len, sizeof(buf) - len, "\tMC:\t");
	len += snprintf(buf + len, sizeof(buf) - len,
			"%d \t %d \t 0x%04x \t 0x%04x \t %d\n",
			vlanMC.vid, vlanMC.priority, vlanMC.member,
			vlanMC.untag, vlanMC.fid);
	len += snprintf(buf + len, sizeof(buf) - len, "\t4K:\t");
	len += snprintf(buf + len, sizeof(buf) - len,
			"%d \t  \t 0x%04x \t 0x%04x \t %d",
			vlan4K.vid, vlan4K.member, vlan4K.untag, vlan4K.fid);

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int rtl8366_set_port_led(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	u32 data = 0;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	if (val->port_vlan >= RTL8366_NUM_PORTS ||
	    (1 << val->port_vlan) == RTL8366_PORT_UNKNOWN)
		return -EINVAL;

	if (val->port_vlan == RTL8366_PORT_NUM_CPU) {
		rtl8366_smi_read_reg(smi, RTL8366_LED_BLINKRATE_REG, &data);
		data = (data & (~(0xF << 4))) | (val->value.i << 4);
		rtl8366_smi_write_reg(smi, RTL8366_LED_BLINKRATE_REG, data);
	} else {
		rtl8366_smi_read_reg(smi, RTL8366_LED_CTRL_REG, &data);
		data = (data & (~(0xF << (val->port_vlan * 4)))) |
			(val->value.i << (val->port_vlan * 4));
		rtl8366_smi_write_reg(smi, RTL8366_LED_CTRL_REG, data);
	}

	return 0;
}

static int rtl8366_get_port_led(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	u32 data = 0;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	if (val->port_vlan >= RTL8366_NUM_LEDGROUPS)
		return -EINVAL;

	rtl8366_smi_read_reg(smi, RTL8366_LED_CTRL_REG, &data);
	val->value.i = (data >> (val->port_vlan * 4)) & 0x000F;

	return 0;
}

static int rtl8366_reset_port_mibs(struct switch_dev *dev,
				   const struct switch_attr *attr,
				   struct switch_val *val)
{
	u32 data = 0;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	if (val->port_vlan >= RTL8366_NUM_PORTS)
		return -EINVAL;

	rtl8366_smi_read_reg(smi, RTL8366S_MIB_CTRL_REG, &data);
	data |= (1 << (val->port_vlan + 3));
	rtl8366_smi_write_reg(smi, RTL8366S_MIB_CTRL_REG, data);

	return 0;
}

static int rtl8366_get_port_mib(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	char buf[2048];
	int i, len = 0;
	unsigned long long counter = 0;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	if (val->port_vlan >= RTL8366_NUM_PORTS)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(buf) - len, "Port %d MIB counters\n",
			val->port_vlan);
	for (i = 0; i < RTL8366S_MIB_COUNT; ++i) {

		len += snprintf(buf + len, sizeof(buf) - len,
				"%d:%s\t", i, MIBCOUNTERS[i]);
		if (!rtl8366_get_mib_counter(smi, i, val->port_vlan, &counter))
			len += snprintf(buf + len, sizeof(buf) - len,
					"[%llu]\n", counter);
		else
			len += snprintf(buf + len, sizeof(buf) - len,
					"[error]\n");
	}

	val->value.s = buf;
	val->len = len;
	return 0;
}

static int rtl8366_set_member(struct switch_dev *dev,
			      const struct switch_attr *attr,
			      struct switch_val *val)
{
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	if (val->port_vlan >= RTL8366_NUM_VLANS)
		return -EINVAL;

	rtl8366s_get_vlan_member_config(smi, val->port_vlan, &vlanMC);

	rtl8366s_get_vlan_4k_entry(smi, vlanMC.vid, &vlan4K);

	vlan4K.member = vlanMC.member = val->value.i;
	rtl8366s_set_vlan_member_config(smi, val->port_vlan, &vlanMC);
	rtl8366s_set_vlan_4k_entry(smi, &vlan4K);

	return 0;
}

static int rtl8366_get_member(struct switch_dev *dev,
			      const struct switch_attr *attr,
			      struct switch_val *val)
{
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	if (val->port_vlan >= RTL8366_NUM_VLANS)
		return -EINVAL;

	rtl8366s_get_vlan_member_config(smi, val->port_vlan, &vlanMC);

	rtl8366s_get_vlan_4k_entry(smi, vlanMC.vid, &vlan4K);

	val->value.i = vlanMC.member;

	return 0;
}

static int rtl8366_set_untag(struct switch_dev *dev,
			     const struct switch_attr *attr,
			     struct switch_val *val)
{
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	if (val->port_vlan >= RTL8366_NUM_VLANS)
		return -EINVAL;

	rtl8366s_get_vlan_member_config(smi, val->port_vlan, &vlanMC);
	rtl8366s_get_vlan_4k_entry(smi, vlanMC.vid, &vlan4K);

	vlan4K.untag = vlanMC.untag = val->value.i;
	rtl8366s_set_vlan_member_config(smi, val->port_vlan, &vlanMC);
	rtl8366s_set_vlan_4k_entry(smi, &vlan4K);

	return 0;
}

static int rtl8366_get_untag(struct switch_dev *dev,
			     const struct switch_attr *attr,
			     struct switch_val *val)
{
	struct rtl8366s_vlanconfig vlanMC;
	struct rtl8366s_vlan4kentry vlan4K;
	struct rtl8366_smi *smi = to_rtl8366(dev);
	memset(&vlanMC, '\0', sizeof(struct rtl8366s_vlanconfig));

	if (val->port_vlan >= RTL8366_NUM_VLANS)
		return -EINVAL;

	rtl8366s_get_vlan_member_config(smi, val->port_vlan, &vlanMC);
	rtl8366s_get_vlan_4k_entry(smi, vlanMC.vid, &vlan4K);


	val->value.i = vlanMC.untag;

	return 0;
}

static int rtl8366_get_port_pvid(struct switch_dev *dev, int port, int *val)
{
	struct rtl8366_smi *smi = to_rtl8366(dev);
	return rtl8366_get_vlan_port_pvid(smi, port, val);
}

static int rtl8366_set_port_pvid(struct switch_dev *dev, int port, int val)
{
	struct rtl8366_smi *smi = to_rtl8366(dev);
	return rtl8366_set_vlan_port_pvid(smi, port, val);
}

static int rtl8366_reset_switch(struct switch_dev *dev)
{
	struct rtl8366_smi *smi = to_rtl8366(dev);
	rtl8366_smi_write_reg(smi, RTL8366_RESET_CTRL_REG,
			      RTL8366_CHIP_CTRL_RESET_HW);
	return 0;
}

static struct switch_attr rtl8366_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = rtl8366_set_vlan,
		.get = rtl8366_get_vlan,
		.max = 1,
		.ofs = 1
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan4k",
		.description = "Enable VLAN 4K mode",
		.set = rtl8366_set_vlan,
		.get = rtl8366_get_vlan,
		.max = 1,
		.ofs = 2
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "init_vlan",
		.description = "Initialize VLAN tables to defaults",
		.set = rtl8366_init_vlan,
		.get = NULL,
		.max = 1
	},

	{
		.type = SWITCH_TYPE_INT,
		.name = "reset_mibs",
		.description = "Reset all MIB counters",
		.set = rtl8366_global_reset_mibs,
		.get = NULL,
		.max = 1
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "blinkrate",
		.description = "Get/Set LED blinking rate (0 = 43ms, 1 = 84ms,"
		" 2 = 120ms, 3 = 170ms, 4 = 340ms, 5 = 670ms)",
		.set = rtl8366_global_set_blinkrate,
		.get = rtl8366_global_get_blinkrate,
		.max = 5
	},
};

static struct switch_attr rtl8366_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "link",
		.description = "Get port link information",
		.max = 1,
		.set = NULL,
		.get = rtl8366_attr_get_port_link
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "reset_mib",
		.description = "Reset single port MIB counters",
		.max = 1,
		.set = rtl8366_reset_port_mibs,
		.get = NULL
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.max = 33,
		.set = NULL,
		.get = rtl8366_get_port_mib
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "led",
		.description = "Get/Set port group (0 - 3) led mode (0 - 15)",
		.max = 15,
		.set = rtl8366_set_port_led,
		.get = rtl8366_get_port_led
	},
};

static struct switch_attr rtl8366_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "untag",
		.description = "Get/Set VLAN untag port set (bitmask)",
		.set = rtl8366_set_untag,
		.get = rtl8366_get_untag,
		.max = 63,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "member",
		.description = "Get/Set VLAN member port set (bitmask)",
		.set = rtl8366_set_member,
		.get = rtl8366_get_member,
		.max = 63,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "info",
		.description = "Get vlan information",
		.max = 1,
		.set = NULL,
		.get = rtl8366_attr_get_vlan_info
	},
};


/* template */
static struct switch_dev rtldev = {
	.name = "RTL8366S",
	.cpu_port = RTL8366_PORT_NUM_CPU,
	.ports = RTL8366_NUM_PORTS,
	.vlans = RTL8366_NUM_VLANS,
	.attr_global = {
		.attr = rtl8366_globals,
		.n_attr = ARRAY_SIZE(rtl8366_globals),
	},
	.attr_port = {
		.attr = rtl8366_port,
		.n_attr = ARRAY_SIZE(rtl8366_port),
	},
	.attr_vlan = {
		.attr = rtl8366_vlan,
		.n_attr = ARRAY_SIZE(rtl8366_vlan),
	},

	.get_port_pvid = rtl8366_get_port_pvid,
	.set_port_pvid = rtl8366_set_port_pvid,
	.reset_switch = rtl8366_reset_switch,
};

static int rtl8366_smi_mii_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 val = 0;
	int err;

	err = rtl8366_smi_read_phy_reg(smi, addr, 0, reg, &val);
	if (err)
		return 0xffff;

	return val;
}

static int rtl8366_smi_mii_write(struct mii_bus *bus, int addr, int reg,
				     u16 val)
{
	struct rtl8366_smi *smi = bus->priv;
	u32 t;
	int err;

	err = rtl8366_smi_write_phy_reg(smi, addr, 0, reg, val);
	/* flush write */
	(void) rtl8366_smi_read_phy_reg(smi, addr, 0, reg, &t);

	return err;
}

static int rtl8366_smi_mii_init(struct rtl8366_smi *smi)
{
	int ret;
	int i;

	smi->mii_bus = mdiobus_alloc();
	if (smi->mii_bus == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	spin_lock_init(&smi->lock);
	smi->mii_bus->priv = (void *) smi;
	smi->mii_bus->name = "rtl8366-smi";
	smi->mii_bus->read = rtl8366_smi_mii_read;
	smi->mii_bus->write = rtl8366_smi_mii_write;
	snprintf(smi->mii_bus->id, MII_BUS_ID_SIZE, "%s",
			dev_name(&smi->pdev->dev));
	smi->mii_bus->parent = &smi->pdev->dev;
	smi->mii_bus->phy_mask = ~(0x1f);
	smi->mii_bus->irq = smi->mii_irq;
	for (i = 0; i < PHY_MAX_ADDR; i++)
		smi->mii_irq[i] = PHY_POLL;

	ret = mdiobus_register(smi->mii_bus);
	if (ret)
		goto err_free;

	return 0;

 err_free:
	mdiobus_free(smi->mii_bus);
 err:
	return ret;
}

static void rtl8366_smi_mii_cleanup(struct rtl8366_smi *smi)
{
	mdiobus_unregister(smi->mii_bus);
	mdiobus_free(smi->mii_bus);
}

static int rtl8366_smi_setup(struct rtl8366_smi *smi)
{
	u32 chip_id = 0;
	u32 chip_ver = 0;
	int ret;

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_ID_REG, &chip_id);
	if (ret) {
		dev_err(&smi->pdev->dev, "unable to read chip id\n");
		return ret;
	}

	switch (chip_id) {
	case RTL8366S_CHIP_ID_8366:
		break;
	default:
		dev_err(&smi->pdev->dev, "unknown chip id (%04x)\n", chip_id);
		return -ENODEV;
	}

	ret = rtl8366_smi_read_reg(smi, RTL8366S_CHIP_VERSION_CTRL_REG,
				   &chip_ver);
	if (ret) {
		dev_err(&smi->pdev->dev, "unable to read chip version\n");
		return ret;
	}

	dev_info(&smi->pdev->dev, "RTL%04x ver. %u chip found\n",
		 chip_id, chip_ver & RTL8366S_CHIP_VERSION_MASK);

	rtl8366_debugfs_init(smi);

	return 0;
}

static int __init rtl8366_smi_probe(struct platform_device *pdev)
{
	static int rtl8366_smi_version_printed;
	struct rtl8366_smi_platform_data *pdata;
	struct rtl8366_smi *smi;
	int err;

	if (!rtl8366_smi_version_printed++)
		printk(KERN_NOTICE RTL8366_SMI_DRIVER_DESC
		       " version " RTL8366_SMI_DRIVER_VER"\n");

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		err = -EINVAL;
		goto err_out;
	}

	smi = kzalloc(sizeof(struct rtl8366_smi), GFP_KERNEL);
	if (!smi) {
		dev_err(&pdev->dev, "no memory for private data\n");
		err = -ENOMEM;
		goto err_out;
	}

	err = gpio_request(pdata->gpio_sda, dev_name(&pdev->dev));
	if (err) {
		dev_err(&pdev->dev, "gpio_request failed for %u, err=%d\n",
			pdata->gpio_sda, err);
		goto err_free_smi;
	}

	err = gpio_request(pdata->gpio_sck, dev_name(&pdev->dev));
	if (err) {
		dev_err(&pdev->dev, "gpio_request failed for %u, err=%d\n",
			pdata->gpio_sck, err);
		goto err_free_sda;
	}

	smi->pdev = pdev;
	smi->pdata = pdata;
	spin_lock_init(&smi->lock);

	platform_set_drvdata(pdev, smi);

	dev_info(&pdev->dev, "using GPIO pins %u (SDA) and %u (SCK)\n",
		 pdata->gpio_sda, pdata->gpio_sck);

	err = rtl8366_smi_setup(smi);
	if (err)
		goto err_clear_drvdata;

	err = rtl8366_smi_mii_init(smi);
	if (err)
		goto err_clear_drvdata;

	return 0;

 err_clear_drvdata:
	platform_set_drvdata(pdev, NULL);
	gpio_free(pdata->gpio_sck);
 err_free_sda:
	gpio_free(pdata->gpio_sda);
 err_free_smi:
	kfree(smi);
 err_out:
	return err;
}

int rtl8366_phy_config_init(struct phy_device *phydev)
{
	int err;
	struct net_device *netdev = phydev->attached_dev;
	struct rtl8366_smi *smi = phydev->bus->priv;
	struct switch_dev *dev = &smi->dev;

	/* Only init the switch for the primary PHY */
	if (phydev->addr != 4) {
		printk(KERN_INFO "Discarding address: %d\n", phydev->addr);
		return 0;
	}

	memcpy(&smi->dev, &rtldev, sizeof(struct switch_dev));
	dev->priv = smi;
	dev->netdev = netdev;

	err = register_switch(dev, netdev);
	if (err < 0) {
		printk(KERN_INFO "Switch registration failed\n");
		return err;
	}

	return 0;
}

int rtl8366_phy_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver rtl8366_smi_phy_driver = {
	.phy_id		= 0x001cc960,
	.name		= "Realtek RTL8366",
	.phy_id_mask	= 0x1ffffff0,
	.features	= PHY_GBIT_FEATURES,
	.config_aneg	= rtl8366_phy_config_aneg,
	.config_init    = rtl8366_phy_config_init,
	.read_status	= genphy_read_status,
	.driver		= {
		.owner = THIS_MODULE,
	},
};

static int __devexit rtl8366_smi_remove(struct platform_device *pdev)
{
	struct rtl8366_smi *smi = platform_get_drvdata(pdev);

	if (smi) {
		struct rtl8366_smi_platform_data *pdata;

		pdata = pdev->dev.platform_data;

		rtl8366_debugfs_remove(smi);
		phy_driver_unregister(&rtl8366_smi_phy_driver);
		rtl8366_smi_mii_cleanup(smi);
		platform_set_drvdata(pdev, NULL);
		gpio_free(pdata->gpio_sck);
		gpio_free(pdata->gpio_sda);
		kfree(smi);
	}

	return 0;
}

static struct platform_driver rtl8366_smi_driver = {
	.driver = {
		.name		= RTL8366_SMI_DRIVER_NAME,
		.owner		= THIS_MODULE,
	},
	.probe		= rtl8366_smi_probe,
	.remove		= __devexit_p(rtl8366_smi_remove),
};

static int __init rtl8366_smi_init(void)
{
	int ret;
	ret = platform_driver_register(&rtl8366_smi_driver);
	if (ret)
		return ret;

	ret = phy_driver_register(&rtl8366_smi_phy_driver);
	if (ret)
		goto err_platform_unregister;

	return 0;

 err_platform_unregister:
	platform_driver_unregister(&rtl8366_smi_driver);
	return ret;
}
module_init(rtl8366_smi_init);

static void __exit rtl8366_smi_exit(void)
{
	phy_driver_unregister(&rtl8366_smi_phy_driver);
	platform_driver_unregister(&rtl8366_smi_driver);
}
module_exit(rtl8366_smi_exit);

MODULE_DESCRIPTION(RTL8366_SMI_DRIVER_DESC);
MODULE_VERSION(RTL8366_SMI_DRIVER_VER);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Antti Seppälä <a.seppala@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" RTL8366_SMI_DRIVER_NAME);
