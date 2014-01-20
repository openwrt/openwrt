/*
 * B53 switch driver main logic
 *
 * Copyright (C) 2011-2013 Jonas Gorski <jogo@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/delay.h>
#include <linux/export.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/switch.h>
#include <linux/platform_data/b53.h>

#include "b53_regs.h"
#include "b53_priv.h"

/* buffer size needed for displaying all MIBs with max'd values */
#define B53_BUF_SIZE	1188

struct b53_mib_desc {
	u8 size;
	u8 offset;
	const char *name;
};


/* BCM5365 MIB counters */
static const struct b53_mib_desc b53_mibs_65[] = {
	{ 8, 0x00, "TxOctets" },
	{ 4, 0x08, "TxDropPkts" },
	{ 4, 0x10, "TxBroadcastPkts" },
	{ 4, 0x14, "TxMulticastPkts" },
	{ 4, 0x18, "TxUnicastPkts" },
	{ 4, 0x1c, "TxCollisions" },
	{ 4, 0x20, "TxSingleCollision" },
	{ 4, 0x24, "TxMultipleCollision" },
	{ 4, 0x28, "TxDeferredTransmit" },
	{ 4, 0x2c, "TxLateCollision" },
	{ 4, 0x30, "TxExcessiveCollision" },
	{ 4, 0x38, "TxPausePkts" },
	{ 8, 0x44, "RxOctets" },
	{ 4, 0x4c, "RxUndersizePkts" },
	{ 4, 0x50, "RxPausePkts" },
	{ 4, 0x54, "Pkts64Octets" },
	{ 4, 0x58, "Pkts65to127Octets" },
	{ 4, 0x5c, "Pkts128to255Octets" },
	{ 4, 0x60, "Pkts256to511Octets" },
	{ 4, 0x64, "Pkts512to1023Octets" },
	{ 4, 0x68, "Pkts1024to1522Octets" },
	{ 4, 0x6c, "RxOversizePkts" },
	{ 4, 0x70, "RxJabbers" },
	{ 4, 0x74, "RxAlignmentErrors" },
	{ 4, 0x78, "RxFCSErrors" },
	{ 8, 0x7c, "RxGoodOctets" },
	{ 4, 0x84, "RxDropPkts" },
	{ 4, 0x88, "RxUnicastPkts" },
	{ 4, 0x8c, "RxMulticastPkts" },
	{ 4, 0x90, "RxBroadcastPkts" },
	{ 4, 0x94, "RxSAChanges" },
	{ 4, 0x98, "RxFragments" },
	{ },
};

/* BCM63xx MIB counters */
static const struct b53_mib_desc b53_mibs_63xx[] = {
	{ 8, 0x00, "TxOctets" },
	{ 4, 0x08, "TxDropPkts" },
	{ 4, 0x0c, "TxQoSPkts" },
	{ 4, 0x10, "TxBroadcastPkts" },
	{ 4, 0x14, "TxMulticastPkts" },
	{ 4, 0x18, "TxUnicastPkts" },
	{ 4, 0x1c, "TxCollisions" },
	{ 4, 0x20, "TxSingleCollision" },
	{ 4, 0x24, "TxMultipleCollision" },
	{ 4, 0x28, "TxDeferredTransmit" },
	{ 4, 0x2c, "TxLateCollision" },
	{ 4, 0x30, "TxExcessiveCollision" },
	{ 4, 0x38, "TxPausePkts" },
	{ 8, 0x3c, "TxQoSOctets" },
	{ 8, 0x44, "RxOctets" },
	{ 4, 0x4c, "RxUndersizePkts" },
	{ 4, 0x50, "RxPausePkts" },
	{ 4, 0x54, "Pkts64Octets" },
	{ 4, 0x58, "Pkts65to127Octets" },
	{ 4, 0x5c, "Pkts128to255Octets" },
	{ 4, 0x60, "Pkts256to511Octets" },
	{ 4, 0x64, "Pkts512to1023Octets" },
	{ 4, 0x68, "Pkts1024to1522Octets" },
	{ 4, 0x6c, "RxOversizePkts" },
	{ 4, 0x70, "RxJabbers" },
	{ 4, 0x74, "RxAlignmentErrors" },
	{ 4, 0x78, "RxFCSErrors" },
	{ 8, 0x7c, "RxGoodOctets" },
	{ 4, 0x84, "RxDropPkts" },
	{ 4, 0x88, "RxUnicastPkts" },
	{ 4, 0x8c, "RxMulticastPkts" },
	{ 4, 0x90, "RxBroadcastPkts" },
	{ 4, 0x94, "RxSAChanges" },
	{ 4, 0x98, "RxFragments" },
	{ 4, 0xa0, "RxSymbolErrors" },
	{ 4, 0xa4, "RxQoSPkts" },
	{ 8, 0xa8, "RxQoSOctets" },
	{ 4, 0xb0, "Pkts1523to2047Octets" },
	{ 4, 0xb4, "Pkts2048to4095Octets" },
	{ 4, 0xb8, "Pkts4096to8191Octets" },
	{ 4, 0xbc, "Pkts8192to9728Octets" },
	{ 4, 0xc0, "RxDiscarded" },
	{ }
};

/* MIB counters */
static const struct b53_mib_desc b53_mibs[] = {
	{ 8, 0x00, "TxOctets" },
	{ 4, 0x08, "TxDropPkts" },
	{ 4, 0x10, "TxBroadcastPkts" },
	{ 4, 0x14, "TxMulticastPkts" },
	{ 4, 0x18, "TxUnicastPkts" },
	{ 4, 0x1c, "TxCollisions" },
	{ 4, 0x20, "TxSingleCollision" },
	{ 4, 0x24, "TxMultipleCollision" },
	{ 4, 0x28, "TxDeferredTransmit" },
	{ 4, 0x2c, "TxLateCollision" },
	{ 4, 0x30, "TxExcessiveCollision" },
	{ 4, 0x38, "TxPausePkts" },
	{ 8, 0x50, "RxOctets" },
	{ 4, 0x58, "RxUndersizePkts" },
	{ 4, 0x5c, "RxPausePkts" },
	{ 4, 0x60, "Pkts64Octets" },
	{ 4, 0x64, "Pkts65to127Octets" },
	{ 4, 0x68, "Pkts128to255Octets" },
	{ 4, 0x6c, "Pkts256to511Octets" },
	{ 4, 0x70, "Pkts512to1023Octets" },
	{ 4, 0x74, "Pkts1024to1522Octets" },
	{ 4, 0x78, "RxOversizePkts" },
	{ 4, 0x7c, "RxJabbers" },
	{ 4, 0x80, "RxAlignmentErrors" },
	{ 4, 0x84, "RxFCSErrors" },
	{ 8, 0x88, "RxGoodOctets" },
	{ 4, 0x90, "RxDropPkts" },
	{ 4, 0x94, "RxUnicastPkts" },
	{ 4, 0x98, "RxMulticastPkts" },
	{ 4, 0x9c, "RxBroadcastPkts" },
	{ 4, 0xa0, "RxSAChanges" },
	{ 4, 0xa4, "RxFragments" },
	{ 4, 0xa8, "RxJumboPkts" },
	{ 4, 0xac, "RxSymbolErrors" },
	{ 4, 0xc0, "RxDiscarded" },
	{ }
};

static int b53_do_vlan_op(struct b53_device *dev, u8 op)
{
	unsigned int i;

	b53_write8(dev, B53_ARLIO_PAGE, dev->vta_regs[0], VTA_START_CMD | op);

	for (i = 0; i < 10; i++) {
		u8 vta;

		b53_read8(dev, B53_ARLIO_PAGE, dev->vta_regs[0], &vta);
		if (!(vta & VTA_START_CMD))
			return 0;

		usleep_range(100, 200);
	}

	return -EIO;
}

static void b53_set_vlan_entry(struct b53_device *dev, u16 vid, u16 members,
			       u16 untag)
{
	if (is5325(dev)) {
		u32 entry = 0;

		if (members) {
			entry = ((untag & VA_UNTAG_MASK_25) << VA_UNTAG_S_25) |
				members;
			if (dev->core_rev >= 3)
				entry |= VA_VALID_25_R4 | vid << VA_VID_HIGH_S;
			else
				entry |= VA_VALID_25;
		}

		b53_write32(dev, B53_VLAN_PAGE, B53_VLAN_WRITE_25, entry);
		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_TABLE_ACCESS_25, vid |
			    VTA_RW_STATE_WR | VTA_RW_OP_EN);
	} else if (is5365(dev)) {
		u16 entry = 0;

		if (members)
			entry = ((untag & VA_UNTAG_MASK_65) << VA_UNTAG_S_65) |
				members | VA_VALID_65;

		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_WRITE_65, entry);
		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_TABLE_ACCESS_65, vid |
			    VTA_RW_STATE_WR | VTA_RW_OP_EN);
	} else {
		b53_write16(dev, B53_ARLIO_PAGE, dev->vta_regs[1], vid);
		b53_write32(dev, B53_ARLIO_PAGE, dev->vta_regs[2],
			    (untag << VTE_UNTAG_S) | members);

		b53_do_vlan_op(dev, VTA_CMD_WRITE);
	}
}

void b53_set_forwarding(struct b53_device *dev, int enable)
{
	u8 mgmt;

	b53_read8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, &mgmt);

	if (enable)
		mgmt |= SM_SW_FWD_EN;
	else
		mgmt &= ~SM_SW_FWD_EN;

	b53_write8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, mgmt);
}

static void b53_enable_vlan(struct b53_device *dev, int enable)
{
	u8 mgmt, vc0, vc1, vc4 = 0, vc5;

	b53_read8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, &mgmt);
	b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL0, &vc0);
	b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL1, &vc1);

	if (is5325(dev) || is5365(dev)) {
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4_25, &vc4);
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5_25, &vc5);
	} else if (is63xx(dev)) {
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4_63XX, &vc4);
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5_63XX, &vc5);
	} else {
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4, &vc4);
		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5, &vc5);
	}

	mgmt &= ~SM_SW_FWD_MODE;

	if (enable) {
		vc0 |= VC0_VLAN_EN | VC0_VID_CHK_EN | VC0_VID_HASH_VID;
		vc1 |= VC1_RX_MCST_UNTAG_EN | VC1_RX_MCST_FWD_EN;
		vc4 &= ~VC4_ING_VID_CHECK_MASK;
		vc4 |= VC4_ING_VID_VIO_DROP << VC4_ING_VID_CHECK_S;
		vc5 |= VC5_DROP_VTABLE_MISS;

		if (is5325(dev))
			vc0 &= ~VC0_RESERVED_1;

		if (is5325(dev) || is5365(dev))
			vc1 |= VC1_RX_MCST_TAG_EN;

		if (!is5325(dev) && !is5365(dev)) {
			if (dev->allow_vid_4095)
				vc5 |= VC5_VID_FFF_EN;
			else
				vc5 &= ~VC5_VID_FFF_EN;
		}
	} else {
		vc0 &= ~(VC0_VLAN_EN | VC0_VID_CHK_EN | VC0_VID_HASH_VID);
		vc1 &= ~(VC1_RX_MCST_UNTAG_EN | VC1_RX_MCST_FWD_EN);
		vc4 &= ~VC4_ING_VID_CHECK_MASK;
		vc5 &= ~VC5_DROP_VTABLE_MISS;

		if (is5325(dev) || is5365(dev))
			vc4 |= VC4_ING_VID_VIO_FWD << VC4_ING_VID_CHECK_S;
		else
			vc4 |= VC4_ING_VID_VIO_TO_IMP << VC4_ING_VID_CHECK_S;

		if (is5325(dev) || is5365(dev))
			vc1 &= ~VC1_RX_MCST_TAG_EN;

		if (!is5325(dev) && !is5365(dev))
			vc5 &= ~VC5_VID_FFF_EN;
	}

	b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL0, vc0);
	b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL1, vc1);

	if (is5325(dev) || is5365(dev)) {
		/* enable the high 8 bit vid check on 5325 */
		if (is5325(dev) && enable)
			b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL3,
				   VC3_HIGH_8BIT_EN);
		else
			b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL3, 0);

		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4_25, vc4);
		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5_25, vc5);
	} else if (is63xx(dev)) {
		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_CTRL3_63XX, 0);
		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4_63XX, vc4);
		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5_63XX, vc5);
	} else {
		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_CTRL3, 0);
		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4, vc4);
		b53_write8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL5, vc5);
	}

	b53_write8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, mgmt);
}

static int b53_set_jumbo(struct b53_device *dev, int enable, int allow_10_100)
{
	u32 port_mask = 0;
	u16 max_size = JMS_MIN_SIZE;

	if (is5325(dev) || is5365(dev))
		return -EINVAL;

	if (enable) {
		port_mask = dev->enabled_ports;
		max_size = JMS_MAX_SIZE;
		if (allow_10_100)
			port_mask |= JPM_10_100_JUMBO_EN;
	}

	b53_write32(dev, B53_JUMBO_PAGE, dev->jumbo_pm_reg, port_mask);
	return b53_write16(dev, B53_JUMBO_PAGE, dev->jumbo_size_reg, max_size);
}

static int b53_flush_arl(struct b53_device *dev)
{
	unsigned int i;

	b53_write8(dev, B53_CTRL_PAGE, B53_FAST_AGE_CTRL,
		   FAST_AGE_DONE | FAST_AGE_DYNAMIC | FAST_AGE_STATIC);

	for (i = 0; i < 10; i++) {
		u8 fast_age_ctrl;

		b53_read8(dev, B53_CTRL_PAGE, B53_FAST_AGE_CTRL,
			  &fast_age_ctrl);

		if (!(fast_age_ctrl & FAST_AGE_DONE))
			return 0;

		mdelay(1);
	}

	pr_warn("time out while flushing ARL\n");

	return -EINVAL;
}

static void b53_enable_ports(struct b53_device *dev)
{
	unsigned i;

	b53_for_each_port(dev, i) {
		u8 port_ctrl;
		u16 pvlan_mask;

		/*
		 * prevent leaking packets between wan and lan in unmanaged
		 * mode through port vlans.
		 */
		if (dev->enable_vlan || is_cpu_port(dev, i))
			pvlan_mask = 0x1ff;
		else if (is531x5(dev) || is5301x(dev))
			/* BCM53115 may use a different port as cpu port */
			pvlan_mask = BIT(dev->sw_dev.cpu_port);
		else
			pvlan_mask = BIT(B53_CPU_PORT);

		/* BCM5325 CPU port is at 8 */
		if ((is5325(dev) || is5365(dev)) && i == B53_CPU_PORT_25)
			i = B53_CPU_PORT;

		if (dev->chip_id == BCM5398_DEVICE_ID && (i == 6 || i == 7))
			/* disable unused ports 6 & 7 */
			port_ctrl = PORT_CTRL_RX_DISABLE | PORT_CTRL_TX_DISABLE;
		else if (i == B53_CPU_PORT)
			port_ctrl = PORT_CTRL_RX_BCST_EN |
				    PORT_CTRL_RX_MCST_EN |
				    PORT_CTRL_RX_UCST_EN;
		else
			port_ctrl = 0;

		b53_write16(dev, B53_PVLAN_PAGE, B53_PVLAN_PORT_MASK(i),
			    pvlan_mask);

		/* port state is handled by bcm63xx_enet driver */
		if (!is63xx(dev) && !(is5301x(dev) && i == 6))
			b53_write8(dev, B53_CTRL_PAGE, B53_PORT_CTRL(i),
				   port_ctrl);
	}
}

static void b53_enable_mib(struct b53_device *dev)
{
	u8 gc;

	b53_read8(dev, B53_CTRL_PAGE, B53_GLOBAL_CONFIG, &gc);

	gc &= ~(GC_RESET_MIB | GC_MIB_AC_EN);

	b53_write8(dev, B53_CTRL_PAGE, B53_GLOBAL_CONFIG, gc);
}

static int b53_apply(struct b53_device *dev)
{
	int i;

	/* clear all vlan entries */
	if (is5325(dev) || is5365(dev)) {
		for (i = 1; i < dev->sw_dev.vlans; i++)
			b53_set_vlan_entry(dev, i, 0, 0);
	} else {
		b53_do_vlan_op(dev, VTA_CMD_CLEAR);
	}

	b53_enable_vlan(dev, dev->enable_vlan);

	/* fill VLAN table */
	if (dev->enable_vlan) {
		for (i = 0; i < dev->sw_dev.vlans; i++) {
			struct b53_vlan *vlan = &dev->vlans[i];

			if (!vlan->members)
				continue;

			b53_set_vlan_entry(dev, i, vlan->members, vlan->untag);
		}

		b53_for_each_port(dev, i)
			b53_write16(dev, B53_VLAN_PAGE,
				    B53_VLAN_PORT_DEF_TAG(i),
				    dev->ports[i].pvid);
	} else {
		b53_for_each_port(dev, i)
			b53_write16(dev, B53_VLAN_PAGE,
				    B53_VLAN_PORT_DEF_TAG(i), 1);

	}

	b53_enable_ports(dev);

	if (!is5325(dev) && !is5365(dev))
		b53_set_jumbo(dev, dev->enable_jumbo, 1);

	return 0;
}

static void b53_switch_reset_gpio(struct b53_device *dev)
{
	int gpio = dev->reset_gpio;

	if (gpio < 0)
		return;

	/*
	 * Reset sequence: RESET low(50ms)->high(20ms)
	 */
	gpio_set_value(gpio, 0);
	mdelay(50);

	gpio_set_value(gpio, 1);
	mdelay(20);

	dev->current_page = 0xff;
}

static int b53_switch_reset(struct b53_device *dev)
{
	u8 mgmt;

	b53_switch_reset_gpio(dev);

	if (is539x(dev)) {
		b53_write8(dev, B53_CTRL_PAGE, B53_SOFTRESET, 0x83);
		b53_write8(dev, B53_CTRL_PAGE, B53_SOFTRESET, 0x00);
	}

	b53_read8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, &mgmt);

	if (!(mgmt & SM_SW_FWD_EN)) {
		mgmt &= ~SM_SW_FWD_MODE;
		mgmt |= SM_SW_FWD_EN;

		b53_write8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, mgmt);
		b53_read8(dev, B53_CTRL_PAGE, B53_SWITCH_MODE, &mgmt);

		if (!(mgmt & SM_SW_FWD_EN)) {
			pr_err("Failed to enable switch!\n");
			return -EINVAL;
		}
	}

	/* enable all ports */
	b53_enable_ports(dev);

	/* configure MII port if necessary */
	if (is5325(dev)) {
		u8 mii_port_override;

		b53_read8(dev, B53_CTRL_PAGE, B53_PORT_OVERRIDE_CTRL,
			  &mii_port_override);
		/* reverse mii needs to be enabled */
		if (!(mii_port_override & PORT_OVERRIDE_RV_MII_25)) {
			b53_write8(dev, B53_CTRL_PAGE, B53_PORT_OVERRIDE_CTRL,
				   mii_port_override | PORT_OVERRIDE_RV_MII_25);
			b53_read8(dev, B53_CTRL_PAGE, B53_PORT_OVERRIDE_CTRL,
				  &mii_port_override);

			if (!(mii_port_override & PORT_OVERRIDE_RV_MII_25)) {
				pr_err("Failed to enable reverse MII mode\n");
				return -EINVAL;
			}
		}
	} else if ((is531x5(dev) || is5301x(dev)) && dev->sw_dev.cpu_port == B53_CPU_PORT) {
		u8 mii_port_override;

		b53_read8(dev, B53_CTRL_PAGE, B53_PORT_OVERRIDE_CTRL,
			  &mii_port_override);
		b53_write8(dev, B53_CTRL_PAGE, B53_PORT_OVERRIDE_CTRL,
			   mii_port_override | PORT_OVERRIDE_EN |
			   PORT_OVERRIDE_LINK);
	}

	b53_enable_mib(dev);

	return b53_flush_arl(dev);
}

/*
 * Swconfig glue functions
 */

static int b53_global_get_vlan_enable(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	val->value.i = priv->enable_vlan;

	return 0;
}

static int b53_global_set_vlan_enable(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	priv->enable_vlan = val->value.i;

	return 0;
}

static int b53_global_get_jumbo_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	val->value.i = priv->enable_jumbo;

	return 0;
}

static int b53_global_set_jumbo_enable(struct switch_dev *dev,
				       const struct switch_attr *attr,
				       struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	priv->enable_jumbo = val->value.i;

	return 0;
}

static int b53_global_get_4095_enable(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	val->value.i = priv->allow_vid_4095;

	return 0;
}

static int b53_global_set_4095_enable(struct switch_dev *dev,
				      const struct switch_attr *attr,
				      struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	priv->allow_vid_4095 = val->value.i;

	return 0;
}

static int b53_global_get_ports(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	val->len = snprintf(priv->buf, B53_BUF_SIZE, "0x%04x",
			    priv->enabled_ports);
	val->value.s = priv->buf;

	return 0;
}

static int b53_port_get_pvid(struct switch_dev *dev, int port, int *val)
{
	struct b53_device *priv = sw_to_b53(dev);

	*val = priv->ports[port].pvid;

	return 0;
}

static int b53_port_set_pvid(struct switch_dev *dev, int port, int val)
{
	struct b53_device *priv = sw_to_b53(dev);

	if (val > 15 && is5325(priv))
		return -EINVAL;
	if (val == 4095 && !priv->allow_vid_4095)
		return -EINVAL;

	priv->ports[port].pvid = val;

	return 0;
}

static int b53_vlan_get_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);
	struct switch_port *port = &val->value.ports[0];
	struct b53_vlan *vlan = &priv->vlans[val->port_vlan];
	int i;

	val->len = 0;

	if (!vlan->members)
		return 0;

	for (i = 0; i < dev->ports; i++) {
		if (!(vlan->members & BIT(i)))
			continue;


		if (!(vlan->untag & BIT(i)))
			port->flags = BIT(SWITCH_PORT_FLAG_TAGGED);
		else
			port->flags = 0;

		port->id = i;
		val->len++;
		port++;
	}

	return 0;
}

static int b53_vlan_set_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);
	struct switch_port *port;
	struct b53_vlan *vlan = &priv->vlans[val->port_vlan];
	int i;

	/* only BCM5325 and BCM5365 supports VID 0 */
	if (val->port_vlan == 0 && !is5325(priv) && !is5365(priv))
		return -EINVAL;

	/* VLAN 4095 needs special handling */
	if (val->port_vlan == 4095 && !priv->allow_vid_4095)
		return -EINVAL;

	port = &val->value.ports[0];
	vlan->members = 0;
	vlan->untag = 0;
	for (i = 0; i < val->len; i++, port++) {
		vlan->members |= BIT(port->id);

		if (!(port->flags & BIT(SWITCH_PORT_FLAG_TAGGED))) {
			vlan->untag |= BIT(port->id);
			priv->ports[port->id].pvid = val->port_vlan;
		};
	}

	/* ignore disabled ports */
	vlan->members &= priv->enabled_ports;
	vlan->untag &= priv->enabled_ports;

	return 0;
}

static int b53_port_get_link(struct switch_dev *dev, int port,
			     struct switch_port_link *link)
{
	struct b53_device *priv = sw_to_b53(dev);

	if (is_cpu_port(priv, port)) {
		link->link = 1;
		link->duplex = 1;
		link->speed = is5325(priv) || is5365(priv) ?
				SWITCH_PORT_SPEED_100 : SWITCH_PORT_SPEED_1000;
		link->aneg = 0;
	} else if (priv->enabled_ports & BIT(port)) {
		u32 speed;
		u16 lnk, duplex;

		b53_read16(priv, B53_STAT_PAGE, B53_LINK_STAT, &lnk);
		b53_read16(priv, B53_STAT_PAGE, priv->duplex_reg, &duplex);

		lnk = (lnk >> port) & 1;
		duplex = (duplex >> port) & 1;

		if (is5325(priv) || is5365(priv)) {
			u16 tmp;

			b53_read16(priv, B53_STAT_PAGE, B53_SPEED_STAT, &tmp);
			speed = SPEED_PORT_FE(tmp, port);
		} else {
			b53_read32(priv, B53_STAT_PAGE, B53_SPEED_STAT, &speed);
			speed = SPEED_PORT_GE(speed, port);
		}

		link->link = lnk;
		if (lnk) {
			link->duplex = duplex;
			switch (speed) {
			case SPEED_STAT_10M:
				link->speed = SWITCH_PORT_SPEED_10;
				break;
			case SPEED_STAT_100M:
				link->speed = SWITCH_PORT_SPEED_100;
				break;
			case SPEED_STAT_1000M:
				link->speed = SWITCH_PORT_SPEED_1000;
				break;
			}
		}

		link->aneg = 1;
	} else {
		link->link = 0;
	}

	return 0;

}

static int b53_global_reset_switch(struct switch_dev *dev)
{
	struct b53_device *priv = sw_to_b53(dev);

	/* reset vlans */
	priv->enable_vlan = 0;
	priv->enable_jumbo = 0;
	priv->allow_vid_4095 = 0;

	memset(priv->vlans, 0, sizeof(priv->vlans) * dev->vlans);
	memset(priv->ports, 0, sizeof(priv->ports) * dev->ports);

	return b53_switch_reset(priv);
}

static int b53_global_apply_config(struct switch_dev *dev)
{
	struct b53_device *priv = sw_to_b53(dev);

	/* disable switching */
	b53_set_forwarding(priv, 0);

	b53_apply(priv);

	/* enable switching */
	b53_set_forwarding(priv, 1);

	return 0;
}


static int b53_global_reset_mib(struct switch_dev *dev,
				const struct switch_attr *attr,
				struct switch_val *val)
{
	struct b53_device *priv = sw_to_b53(dev);
	u8 gc;

	b53_read8(priv, B53_MGMT_PAGE, B53_GLOBAL_CONFIG, &gc);

	b53_write8(priv, B53_MGMT_PAGE, B53_GLOBAL_CONFIG, gc | GC_RESET_MIB);
	mdelay(1);
	b53_write8(priv, B53_MGMT_PAGE, B53_GLOBAL_CONFIG, gc & ~GC_RESET_MIB);
	mdelay(1);

	return 0;
}

static int b53_port_get_mib(struct switch_dev *sw_dev,
			    const struct switch_attr *attr,
			    struct switch_val *val)
{
	struct b53_device *dev = sw_to_b53(sw_dev);
	const struct b53_mib_desc *mibs;
	int port = val->port_vlan;
	int len = 0;

	if (!(BIT(port) & dev->enabled_ports))
		return -1;

	if (is5365(dev)) {
		if (port == 5)
			port = 8;

		mibs = b53_mibs_65;
	} else if (is63xx(dev)) {
		mibs = b53_mibs_63xx;
	} else {
		mibs = b53_mibs;
	}

	dev->buf[0] = 0;

	for (; mibs->size > 0; mibs++) {
		u64 val;

		if (mibs->size == 8) {
			b53_read64(dev, B53_MIB_PAGE(port), mibs->offset, &val);
		} else {
			u32 val32;

			b53_read32(dev, B53_MIB_PAGE(port), mibs->offset,
				   &val32);
			val = val32;
		}

		len += snprintf(dev->buf + len, B53_BUF_SIZE - len,
				"%-20s: %llu\n", mibs->name, val);
	}

	val->len = len;
	val->value.s = dev->buf;

	return 0;
}

static struct switch_attr b53_global_ops_25[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = b53_global_set_vlan_enable,
		.get = b53_global_get_vlan_enable,
		.max = 1,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "ports",
		.description = "Available ports (as bitmask)",
		.get = b53_global_get_ports,
	},
};

static struct switch_attr b53_global_ops_65[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = b53_global_set_vlan_enable,
		.get = b53_global_get_vlan_enable,
		.max = 1,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "ports",
		.description = "Available ports (as bitmask)",
		.get = b53_global_get_ports,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "reset_mib",
		.description = "Reset MIB counters",
		.set = b53_global_reset_mib,
	},
};

static struct switch_attr b53_global_ops[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = b53_global_set_vlan_enable,
		.get = b53_global_get_vlan_enable,
		.max = 1,
	},
	{
		.type = SWITCH_TYPE_STRING,
		.name = "ports",
		.description = "Available Ports (as bitmask)",
		.get = b53_global_get_ports,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "reset_mib",
		.description = "Reset MIB counters",
		.set = b53_global_reset_mib,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_jumbo",
		.description = "Enable Jumbo Frames",
		.set = b53_global_set_jumbo_enable,
		.get = b53_global_get_jumbo_enable,
		.max = 1,
	},
	{
		.type = SWITCH_TYPE_INT,
		.name = "allow_vid_4095",
		.description = "Allow VID 4095",
		.set = b53_global_set_4095_enable,
		.get = b53_global_get_4095_enable,
		.max = 1,
	},
};

static struct switch_attr b53_port_ops[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get port's MIB counters",
		.get = b53_port_get_mib,
	},
};

static struct switch_attr b53_no_ops[] = {
};

static const struct switch_dev_ops b53_switch_ops_25 = {
	.attr_global = {
		.attr = b53_global_ops_25,
		.n_attr = ARRAY_SIZE(b53_global_ops_25),
	},
	.attr_port = {
		.attr = b53_no_ops,
		.n_attr = ARRAY_SIZE(b53_no_ops),
	},
	.attr_vlan = {
		.attr = b53_no_ops,
		.n_attr = ARRAY_SIZE(b53_no_ops),
	},

	.get_vlan_ports = b53_vlan_get_ports,
	.set_vlan_ports = b53_vlan_set_ports,
	.get_port_pvid = b53_port_get_pvid,
	.set_port_pvid = b53_port_set_pvid,
	.apply_config = b53_global_apply_config,
	.reset_switch = b53_global_reset_switch,
	.get_port_link = b53_port_get_link,
};

static const struct switch_dev_ops b53_switch_ops_65 = {
	.attr_global = {
		.attr = b53_global_ops_65,
		.n_attr = ARRAY_SIZE(b53_global_ops_65),
	},
	.attr_port = {
		.attr = b53_port_ops,
		.n_attr = ARRAY_SIZE(b53_port_ops),
	},
	.attr_vlan = {
		.attr = b53_no_ops,
		.n_attr = ARRAY_SIZE(b53_no_ops),
	},

	.get_vlan_ports = b53_vlan_get_ports,
	.set_vlan_ports = b53_vlan_set_ports,
	.get_port_pvid = b53_port_get_pvid,
	.set_port_pvid = b53_port_set_pvid,
	.apply_config = b53_global_apply_config,
	.reset_switch = b53_global_reset_switch,
	.get_port_link = b53_port_get_link,
};

static const struct switch_dev_ops b53_switch_ops = {
	.attr_global = {
		.attr = b53_global_ops,
		.n_attr = ARRAY_SIZE(b53_global_ops),
	},
	.attr_port = {
		.attr = b53_port_ops,
		.n_attr = ARRAY_SIZE(b53_port_ops),
	},
	.attr_vlan = {
		.attr = b53_no_ops,
		.n_attr = ARRAY_SIZE(b53_no_ops),
	},

	.get_vlan_ports = b53_vlan_get_ports,
	.set_vlan_ports = b53_vlan_set_ports,
	.get_port_pvid = b53_port_get_pvid,
	.set_port_pvid = b53_port_set_pvid,
	.apply_config = b53_global_apply_config,
	.reset_switch = b53_global_reset_switch,
	.get_port_link = b53_port_get_link,
};

struct b53_chip_data {
	u32 chip_id;
	const char *dev_name;
	const char *alias;
	u16 vlans;
	u16 enabled_ports;
	u8 cpu_port;
	u8 vta_regs[3];
	u8 duplex_reg;
	u8 jumbo_pm_reg;
	u8 jumbo_size_reg;
	const struct switch_dev_ops *sw_ops;
};

#define B53_VTA_REGS	\
	{ B53_VT_ACCESS, B53_VT_INDEX, B53_VT_ENTRY }
#define B53_VTA_REGS_9798 \
	{ B53_VT_ACCESS_9798, B53_VT_INDEX_9798, B53_VT_ENTRY_9798 }
#define B53_VTA_REGS_63XX \
	{ B53_VT_ACCESS_63XX, B53_VT_INDEX_63XX, B53_VT_ENTRY_63XX }

static const struct b53_chip_data b53_switch_chips[] = {
	{
		.chip_id = BCM5325_DEVICE_ID,
		.dev_name = "BCM5325",
		.alias = "bcm5325",
		.vlans = 16,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25,
		.duplex_reg = B53_DUPLEX_STAT_FE,
		.sw_ops = &b53_switch_ops_25,
	},
	{
		.chip_id = BCM5365_DEVICE_ID,
		.dev_name = "BCM5365",
		.alias = "bcm5365",
		.vlans = 256,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25,
		.duplex_reg = B53_DUPLEX_STAT_FE,
		.sw_ops = &b53_switch_ops_65,
	},
	{
		.chip_id = BCM5395_DEVICE_ID,
		.dev_name = "BCM5395",
		.alias = "bcm5395",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT,
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM5397_DEVICE_ID,
		.dev_name = "BCM5397",
		.alias = "bcm5397",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT,
		.vta_regs = B53_VTA_REGS_9798,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM5398_DEVICE_ID,
		.dev_name = "BCM5398",
		.alias = "bcm5398",
		.vlans = 4096,
		.enabled_ports = 0x7f,
		.cpu_port = B53_CPU_PORT,
		.vta_regs = B53_VTA_REGS_9798,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53115_DEVICE_ID,
		.dev_name = "BCM53115",
		.alias = "bcm53115",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.vta_regs = B53_VTA_REGS,
		.cpu_port = B53_CPU_PORT,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53125_DEVICE_ID,
		.dev_name = "BCM53125",
		.alias = "bcm53125",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT,
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM63XX_DEVICE_ID,
		.dev_name = "BCM63xx",
		.alias = "bcm63xx",
		.vlans = 4096,
		.enabled_ports = 0, /* pdata must provide them */
		.cpu_port = B53_CPU_PORT,
		.vta_regs = B53_VTA_REGS_63XX,
		.duplex_reg = B53_DUPLEX_STAT_63XX,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK_63XX,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE_63XX,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53010_DEVICE_ID,
		.dev_name = "BCM53010",
		.alias = "bcm53011",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25, // TODO: auto detect
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53011_DEVICE_ID,
		.dev_name = "BCM53011",
		.alias = "bcm53011",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25, // TODO: auto detect
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53012_DEVICE_ID,
		.dev_name = "BCM53012",
		.alias = "bcm53011",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25, // TODO: auto detect
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53018_DEVICE_ID,
		.dev_name = "BCM53018",
		.alias = "bcm53018",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25, // TODO: auto detect
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
	{
		.chip_id = BCM53019_DEVICE_ID,
		.dev_name = "BCM53019",
		.alias = "bcm53019",
		.vlans = 4096,
		.enabled_ports = 0x1f,
		.cpu_port = B53_CPU_PORT_25, // TODO: auto detect
		.vta_regs = B53_VTA_REGS,
		.duplex_reg = B53_DUPLEX_STAT_GE,
		.jumbo_pm_reg = B53_JUMBO_PORT_MASK,
		.jumbo_size_reg = B53_JUMBO_MAX_SIZE,
		.sw_ops = &b53_switch_ops,
	},
};

int b53_switch_init(struct b53_device *dev)
{
	struct switch_dev *sw_dev = &dev->sw_dev;
	unsigned i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(b53_switch_chips); i++) {
		const struct b53_chip_data *chip = &b53_switch_chips[i];

		if (chip->chip_id == dev->chip_id) {
			sw_dev->name = chip->dev_name;
			if (!sw_dev->alias)
				sw_dev->alias = chip->alias;
			if (!dev->enabled_ports)
				dev->enabled_ports = chip->enabled_ports;
			dev->duplex_reg = chip->duplex_reg;
			dev->vta_regs[0] = chip->vta_regs[0];
			dev->vta_regs[1] = chip->vta_regs[1];
			dev->vta_regs[2] = chip->vta_regs[2];
			dev->jumbo_pm_reg = chip->jumbo_pm_reg;
			sw_dev->ops = chip->sw_ops;
			sw_dev->cpu_port = chip->cpu_port;
			sw_dev->vlans = chip->vlans;
			break;
		}
	}

	if (!sw_dev->name)
		return -EINVAL;

	/* check which BCM5325x version we have */
	if (is5325(dev)) {
		u8 vc4;

		b53_read8(dev, B53_VLAN_PAGE, B53_VLAN_CTRL4_25, &vc4);

		/* check reserved bits */
		switch (vc4 & 3) {
		case 1:
			/* BCM5325E */
			break;
		case 3:
			/* BCM5325F - do not use port 4 */
			dev->enabled_ports &= ~BIT(4);
			break;
		default:
/* On the BCM47XX SoCs this is the supported internal switch.*/
#ifndef CONFIG_BCM47XX
			/* BCM5325M */
			return -EINVAL;
#else
			break;
#endif
		}
	} else if (dev->chip_id == BCM53115_DEVICE_ID) {
		u64 strap_value;

		b53_read48(dev, B53_STAT_PAGE, B53_STRAP_VALUE, &strap_value);
		/* use second IMP port if GMII is enabled */
		if (strap_value & SV_GMII_CTRL_115)
			sw_dev->cpu_port = 5;
	}

	/* cpu port is always last */
	sw_dev->ports = sw_dev->cpu_port + 1;
	dev->enabled_ports |= BIT(sw_dev->cpu_port);

	dev->ports = devm_kzalloc(dev->dev,
				  sizeof(struct b53_port) * sw_dev->ports,
				  GFP_KERNEL);
	if (!dev->ports)
		return -ENOMEM;

	dev->vlans = devm_kzalloc(dev->dev,
				  sizeof(struct b53_vlan) * sw_dev->vlans,
				  GFP_KERNEL);
	if (!dev->vlans)
		return -ENOMEM;

	dev->buf = devm_kzalloc(dev->dev, B53_BUF_SIZE, GFP_KERNEL);
	if (!dev->buf)
		return -ENOMEM;

	dev->reset_gpio = b53_switch_get_reset_gpio(dev);
	if (dev->reset_gpio >= 0) {
		ret = devm_gpio_request_one(dev->dev, dev->reset_gpio, GPIOF_OUT_INIT_HIGH, "robo_reset");
		if (ret)
			return ret;
	}

	return b53_switch_reset(dev);
}

struct b53_device *b53_switch_alloc(struct device *base, struct b53_io_ops *ops,
				    void *priv)
{
	struct b53_device *dev;

	dev = devm_kzalloc(base, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return NULL;

	dev->dev = base;
	dev->ops = ops;
	dev->priv = priv;
	mutex_init(&dev->reg_mutex);

	return dev;
}
EXPORT_SYMBOL(b53_switch_alloc);

int b53_switch_detect(struct b53_device *dev)
{
	u32 id32;
	u16 tmp;
	u8 id8;
	int ret;

	ret = b53_read8(dev, B53_MGMT_PAGE, B53_DEVICE_ID, &id8);
	if (ret)
		return ret;

	switch (id8) {
	case 0:
		/*
		 * BCM5325 and BCM5365 do not have this register so reads
		 * return 0. But the read operation did succeed, so assume
		 * this is one of them.
		 *
		 * Next check if we can write to the 5325's VTA register; for
		 * 5365 it is read only.
		 */

		b53_write16(dev, B53_VLAN_PAGE, B53_VLAN_TABLE_ACCESS_25, 0xf);
		b53_read16(dev, B53_VLAN_PAGE, B53_VLAN_TABLE_ACCESS_25, &tmp);

		if (tmp == 0xf)
			dev->chip_id = BCM5325_DEVICE_ID;
		else
			dev->chip_id = BCM5365_DEVICE_ID;
		break;
	case BCM5395_DEVICE_ID:
	case BCM5397_DEVICE_ID:
	case BCM5398_DEVICE_ID:
		dev->chip_id = id8;
		break;
	default:
		ret = b53_read32(dev, B53_MGMT_PAGE, B53_DEVICE_ID, &id32);
		if (ret)
			return ret;

		switch (id32) {
		case BCM53115_DEVICE_ID:
		case BCM53125_DEVICE_ID:
		case BCM53010_DEVICE_ID:
		case BCM53011_DEVICE_ID:
		case BCM53012_DEVICE_ID:
		case BCM53018_DEVICE_ID:
		case BCM53019_DEVICE_ID:
			dev->chip_id = id32;
			break;
		default:
			pr_err("unsupported switch detected (BCM53%02x/BCM%x)\n",
			       id8, id32);
			return -ENODEV;
		}
	}

	if (dev->chip_id == BCM5325_DEVICE_ID)
		return b53_read8(dev, B53_STAT_PAGE, B53_REV_ID_25,
				 &dev->core_rev);
	else
		return b53_read8(dev, B53_MGMT_PAGE, B53_REV_ID,
				 &dev->core_rev);
}
EXPORT_SYMBOL(b53_switch_detect);

int b53_switch_register(struct b53_device *dev)
{
	int ret;

	if (dev->pdata) {
		dev->chip_id = dev->pdata->chip_id;
		dev->enabled_ports = dev->pdata->enabled_ports;
		dev->sw_dev.alias = dev->pdata->alias;
	}

	if (!dev->chip_id && b53_switch_detect(dev))
		return -EINVAL;

	ret = b53_switch_init(dev);
	if (ret)
		return ret;

	pr_info("found switch: %s, rev %i\n", dev->sw_dev.name, dev->core_rev);

	return register_switch(&dev->sw_dev, NULL);
}
EXPORT_SYMBOL(b53_switch_register);

MODULE_AUTHOR("Jonas Gorski <jogo@openwrt.org>");
MODULE_DESCRIPTION("B53 switch library");
MODULE_LICENSE("Dual BSD/GPL");
