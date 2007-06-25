/*

  Broadcom BCM43xx wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
  Copyright (c) 2005, 2006 Stefano Brivio <st3@riseup.net>
  Copyright (c) 2005, 2006 Michael Buesch <mb@bu3sch.de>
  Copyright (c) 2005, 2006 Danny van Dyk <kugelfang@gentoo.org>
  Copyright (c) 2005, 2006 Andreas Jaggi <andreas.jaggi@waterwave.ch>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include <linux/delay.h>
#include <linux/types.h>

#include "bcm43xx.h"
#include "bcm43xx_phy.h"
#include "bcm43xx_main.h"
#include "bcm43xx_tables.h"
#include "bcm43xx_power.h"
#include "bcm43xx_lo.h"


static const s8 bcm43xx_tssi2dbm_b_table[] = {
	0x4D, 0x4C, 0x4B, 0x4A,
	0x4A, 0x49, 0x48, 0x47,
	0x47, 0x46, 0x45, 0x45,
	0x44, 0x43, 0x42, 0x42,
	0x41, 0x40, 0x3F, 0x3E,
	0x3D, 0x3C, 0x3B, 0x3A,
	0x39, 0x38, 0x37, 0x36,
	0x35, 0x34, 0x32, 0x31,
	0x30, 0x2F, 0x2D, 0x2C,
	0x2B, 0x29, 0x28, 0x26,
	0x25, 0x23, 0x21, 0x1F,
	0x1D, 0x1A, 0x17, 0x14,
	0x10, 0x0C, 0x06, 0x00,
	  -7,   -7,   -7,   -7,
	  -7,   -7,   -7,   -7,
	  -7,   -7,   -7,   -7,
};

static const s8 bcm43xx_tssi2dbm_g_table[] = {
	 77,  77,  77,  76,
	 76,  76,  75,  75,
	 74,  74,  73,  73,
	 73,  72,  72,  71,
	 71,  70,  70,  69,
	 68,  68,  67,  67,
	 66,  65,  65,  64,
	 63,  63,  62,  61,
	 60,  59,  58,  57,
	 56,  55,  54,  53,
	 52,  50,  49,  47,
	 45,  43,  40,  37,
	 33,  28,  22,  14,
	  5,  -7, -20, -20,
	-20, -20, -20, -20,
	-20, -20, -20, -20,
};

const u8 bcm43xx_radio_channel_codes_bg[] = {
	12, 17, 22, 27,
	32, 37, 42, 47,
	52, 57, 62, 67,
	72, 84,
};


static void bcm43xx_phy_initg(struct bcm43xx_wldev *dev);

/* Reverse the bits of a 4bit value.
 * Example:  1101 is flipped 1011
 */
static u16 flip_4bit(u16 value)
{
	u16 flipped = 0x0000;

	assert((value & ~0x000F) == 0x0000);

	flipped |= (value & 0x0001) << 3;
	flipped |= (value & 0x0002) << 1;
	flipped |= (value & 0x0004) >> 1;
	flipped |= (value & 0x0008) >> 3;

	return flipped;
}

static void generate_rfatt_list(struct bcm43xx_wldev *dev,
				struct bcm43xx_rfatt_list *list)
{
	struct bcm43xx_phy *phy = &dev->phy;

	/* APHY.rev < 5 || GPHY.rev < 6 */
	static const struct bcm43xx_rfatt rfatt_0[] = {
		{ .att = 3,  .with_padmix = 0, },
		{ .att = 1,  .with_padmix = 0, },
		{ .att = 5,  .with_padmix = 0, },
		{ .att = 7,  .with_padmix = 0, },
		{ .att = 9,  .with_padmix = 0, },
		{ .att = 2,  .with_padmix = 0, },
		{ .att = 0,  .with_padmix = 0, },
		{ .att = 4,  .with_padmix = 0, },
		{ .att = 6,  .with_padmix = 0, },
		{ .att = 8,  .with_padmix = 0, },
		{ .att = 1,  .with_padmix = 1, },
		{ .att = 2,  .with_padmix = 1, },
		{ .att = 3,  .with_padmix = 1, },
		{ .att = 4,  .with_padmix = 1, },
	};
	/* Radio.rev == 8 && Radio.version == 0x2050 */
	static const struct bcm43xx_rfatt rfatt_1[] = {
		{ .att = 2,   .with_padmix = 1, },
		{ .att = 4,   .with_padmix = 1, },
		{ .att = 6,   .with_padmix = 1, },
		{ .att = 8,   .with_padmix = 1, },
		{ .att = 10,  .with_padmix = 1, },
		{ .att = 12,  .with_padmix = 1, },
		{ .att = 14,  .with_padmix = 1, },
	};
	/* Otherwise */
	static const struct bcm43xx_rfatt rfatt_2[] = {
		{ .att = 0,  .with_padmix = 1, },
		{ .att = 2,  .with_padmix = 1, },
		{ .att = 4,  .with_padmix = 1, },
		{ .att = 6,  .with_padmix = 1, },
		{ .att = 8,  .with_padmix = 1, },
		{ .att = 9,  .with_padmix = 1, },
		{ .att = 9,  .with_padmix = 1, },
	};

	if ((phy->type == BCM43xx_PHYTYPE_A && phy->rev < 5) ||
	    (phy->type == BCM43xx_PHYTYPE_G && phy->rev < 6)) {
		/* Software pctl */
		list->list = rfatt_0;
		list->len = ARRAY_SIZE(rfatt_0);
		list->min_val = 0;
		list->max_val = 9;
		return;
	}
	if (phy->radio_ver == 0x2050 && phy->radio_rev == 8) {
		/* Hardware pctl */
		list->list = rfatt_1;
		list->len = ARRAY_SIZE(rfatt_1);
		list->min_val = 2;
		list->max_val = 14;
		return;
	}
	/* Hardware pctl */
	list->list = rfatt_2;
	list->len = ARRAY_SIZE(rfatt_2);
	list->min_val = 0;
	list->max_val = 9;
}

static void generate_bbatt_list(struct bcm43xx_wldev *dev,
				struct bcm43xx_bbatt_list *list)
{
	static const struct bcm43xx_bbatt bbatt_0[] = {
		{ .att = 0, },
		{ .att = 1, },
		{ .att = 2, },
		{ .att = 3, },
		{ .att = 4, },
		{ .att = 5, },
		{ .att = 6, },
		{ .att = 7, },
		{ .att = 8, },
	};

	list->list = bbatt_0;
	list->len = ARRAY_SIZE(bbatt_0);
	list->min_val = 0;
	list->max_val = 8;
}

static void bcm43xx_shm_clear_tssi(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0068, 0x7F7F);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x006a, 0x7F7F);
		break;
	case BCM43xx_PHYTYPE_B:
	case BCM43xx_PHYTYPE_G:
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0058, 0x7F7F);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x005a, 0x7F7F);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0070, 0x7F7F);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0072, 0x7F7F);
		break;
	}
}

void bcm43xx_raw_phy_lock(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	assert(irqs_disabled());
	if (bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD) == 0) {
		phy->locked = 0;
		return;
	}
	if (dev->dev->id.revision < 3) {
		bcm43xx_mac_suspend(dev);
		spin_lock(&phy->lock);
	} else {
		if (!bcm43xx_is_mode(dev->wl, IEEE80211_IF_TYPE_AP))
			bcm43xx_power_saving_ctl_bits(dev, -1, 1);
	}
	phy->locked = 1;
}

void bcm43xx_raw_phy_unlock(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	assert(irqs_disabled());
	if (dev->dev->id.revision < 3) {
		if (phy->locked) {
			spin_unlock(&phy->lock);
			bcm43xx_mac_enable(dev);
		}
	} else {
		if (!bcm43xx_is_mode(dev->wl, IEEE80211_IF_TYPE_AP))
			bcm43xx_power_saving_ctl_bits(dev, -1, -1);
	}
	phy->locked = 0;
}

/* Different PHYs require different register routing flags.
 * This adjusts (and does sanity checks on) the routing flags.
 */
static inline u16 adjust_phyreg_for_phytype(struct bcm43xx_phy *phy,
					    u16 offset)
{
	if (phy->type == BCM43xx_PHYTYPE_A) {
		/* OFDM registers are base-registers for the A-PHY. */
		offset &= ~BCM43xx_PHYROUTE_OFDM_GPHY;
	}
	if (offset & BCM43xx_PHYROUTE_EXT_GPHY) {
		/* Ext-G registers are only available on G-PHYs */
		if (phy->type != BCM43xx_PHYTYPE_G) {
			dprintk(KERN_ERR PFX "EXT-G PHY access at "
				"0x%04X on %u type PHY\n",
				offset, phy->type);
		}
	}

	return offset;
}

u16 bcm43xx_phy_read(struct bcm43xx_wldev *dev, u16 offset)
{
	struct bcm43xx_phy *phy = &dev->phy;

	offset = adjust_phyreg_for_phytype(phy, offset);
	bcm43xx_write16(dev, BCM43xx_MMIO_PHY_CONTROL, offset);
	return bcm43xx_read16(dev, BCM43xx_MMIO_PHY_DATA);
}

void bcm43xx_phy_write(struct bcm43xx_wldev *dev, u16 offset, u16 val)
{
	struct bcm43xx_phy *phy = &dev->phy;

	offset = adjust_phyreg_for_phytype(phy, offset);
	bcm43xx_write16(dev, BCM43xx_MMIO_PHY_CONTROL, offset);
	mmiowb();
	bcm43xx_write16(dev, BCM43xx_MMIO_PHY_DATA, val);
}

static void bcm43xx_radio_set_txpower_a(struct bcm43xx_wldev *dev, u16 txpower);

/* Adjust the transmission power output (G-PHY) */
void bcm43xx_set_txpower_g(struct bcm43xx_wldev *dev,
			   const struct bcm43xx_bbatt *bbatt,
			   const struct bcm43xx_rfatt *rfatt,
			   u8 tx_control)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 bb, rf;
	u16 tx_bias, tx_magn;

	bb = bbatt->att;
	rf = rfatt->att;
	tx_bias = lo->tx_bias;
	tx_magn = lo->tx_magn;
	if (unlikely(tx_bias == 0xFF))
		tx_bias = 0;

	/* Save the values for later */
	phy->tx_control = tx_control;
	memcpy(&phy->rfatt, rfatt, sizeof(*rfatt));
	memcpy(&phy->bbatt, bbatt, sizeof(*bbatt));

	if (bcm43xx_debug(dev, BCM43xx_DBG_XMITPOWER)) {
		dprintk(KERN_DEBUG PFX "Tuning TX-power to bbatt(%u), "
			"rfatt(%u), tx_control(0x%02X), "
			"tx_bias(0x%02X), tx_magn(0x%02X)\n",
			bb, rf, tx_control, tx_bias, tx_magn);
	}

	bcm43xx_phy_set_baseband_attenuation(dev, bb);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, BCM43xx_SHM_SH_RFATT, rf);
	if (phy->radio_ver == 0x2050 && phy->radio_rev == 8) {
		bcm43xx_radio_write16(dev, 0x43,
				      (rf & 0x000F) | (tx_control & 0x0070));
	} else {
		bcm43xx_radio_write16(dev, 0x43,
				      (bcm43xx_radio_read16(dev, 0x43)
				       & 0xFFF0) | (rf & 0x000F));
		bcm43xx_radio_write16(dev, 0x52,
				      (bcm43xx_radio_read16(dev, 0x52)
				       & ~0x0070) | (tx_control & 0x0070));
	}
	if (has_tx_magnification(phy)) {
		bcm43xx_radio_write16(dev, 0x52, tx_magn | tx_bias);
	} else {
		bcm43xx_radio_write16(dev, 0x52,
				      (bcm43xx_radio_read16(dev, 0x52)
				       & 0xFFF0) | (tx_bias & 0x000F));
	}
	if (phy->type == BCM43xx_PHYTYPE_G)
		bcm43xx_lo_g_adjust(dev);
}

static void default_baseband_attenuation(struct bcm43xx_wldev *dev,
					 struct bcm43xx_bbatt *bb)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->radio_ver == 0x2050 && phy->radio_rev < 6)
		bb->att = 0;
	else
		bb->att = 2;
}

static void default_radio_attenuation(struct bcm43xx_wldev *dev,
				      struct bcm43xx_rfatt *rf)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;

	rf->with_padmix = 0;

	if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
	    bus->boardinfo.type == SSB_BOARD_BCM4309G) {
		if (bus->boardinfo.rev < 0x43) {
			rf->att = 2;
			return;
		} else if (bus->boardinfo.rev < 0x51) {
			rf->att = 3;
			return;
		}
	}

	if (phy->type == BCM43xx_PHYTYPE_A) {
		rf->att = 0x60;
		return;
	}

	switch (phy->radio_ver) {
	case 0x2053:
		switch (phy->radio_rev) {
		case 1:
			rf->att = 6;
			return;
		}
		break;
	case 0x2050:
		switch (phy->radio_rev) {
		case 0:
			rf->att = 5;
			return;
		case 1:
			if (phy->type == BCM43xx_PHYTYPE_G) {
				if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
				    bus->boardinfo.type == SSB_BOARD_BCM4309G &&
				    bus->boardinfo.rev >= 30)
					rf->att = 3;
				else if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
					 bus->boardinfo.type == SSB_BOARD_BU4306)
					rf->att = 3;
				else
					rf->att = 1;
			} else {
				if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
				    bus->boardinfo.type == SSB_BOARD_BCM4309G &&
				    bus->boardinfo.rev >= 30)
					rf->att = 7;
				else
					rf->att = 6;
			}
			return;
		case 2:
			if (phy->type == BCM43xx_PHYTYPE_G) {
				if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
				    bus->boardinfo.type == SSB_BOARD_BCM4309G &&
				    bus->boardinfo.rev >= 30)
					rf->att = 3;
				else if (bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM &&
					 bus->boardinfo.type == SSB_BOARD_BU4306)
					rf->att = 5;
				else if (bus->chip_id == 0x4320)
					rf->att = 4;
				else
					rf->att = 3;
			} else
				rf->att = 6;
			return;
		case 3:
			rf->att = 5;
			return;
		case 4:
		case 5:
			rf->att = 1;
			return;
		case 6:
		case 7:
			rf->att = 5;
			return;
		case 8:
			rf->att = 0xA;
			rf->with_padmix = 1;
			return;
		case 9:
		default:
			rf->att = 5;
			return;
		}
	}
	rf->att = 5;
}

static u16 default_tx_control(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->radio_ver != 0x2050)
		return 0;
	if (phy->radio_rev == 1)
		return BCM43xx_TXCTL_PA2DB | BCM43xx_TXCTL_TXMIX;
	if (phy->radio_rev < 6)
		return BCM43xx_TXCTL_PA2DB;
	if (phy->radio_rev == 8)
		return BCM43xx_TXCTL_TXMIX;
	return 0;
}

/* This func is called "PHY calibrate" in the specs... */
void bcm43xx_phy_early_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;

	default_baseband_attenuation(dev, &phy->bbatt);
	default_radio_attenuation(dev, &phy->rfatt);
	phy->tx_control = (default_tx_control(dev) << 4);

	bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD); /* Dummy read. */
	if (phy->type == BCM43xx_PHYTYPE_B ||
	    phy->type == BCM43xx_PHYTYPE_G) {
		generate_rfatt_list(dev, &lo->rfatt_list);
		generate_bbatt_list(dev, &lo->bbatt_list);
	}
	if (phy->type == BCM43xx_PHYTYPE_G && phy->rev == 1) {
		/* Workaround: Temporarly disable gmode through the early init
		 * phase, as the gmode stuff is not needed for phy rev 1 */
		phy->gmode = 0;
		bcm43xx_wireless_core_reset(dev, 0);
		bcm43xx_phy_initg(dev);
		phy->gmode = 1;
		bcm43xx_wireless_core_reset(dev, BCM43xx_TMSLOW_GMODE);
	}
}

/* GPHY_TSSI_Power_Lookup_Table_Init */
static void bcm43xx_gphy_tssi_power_lt_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int i;
	u16 value;

	for (i = 0; i < 32; i++)
		bcm43xx_ofdmtab_write16(dev, 0x3C20, i, phy->tssi2dbm[i]);
	for (i = 32; i < 64; i++)
		bcm43xx_ofdmtab_write16(dev, 0x3C00, i - 32, phy->tssi2dbm[i]);
	for (i = 0; i < 64; i += 2) {
		value = (u16)phy->tssi2dbm[i];
		value |= ((u16)phy->tssi2dbm[i + 1]) << 8;
		bcm43xx_phy_write(dev, 0x380 + (i / 2), value);
	}
}

/* GPHY_Gain_Lookup_Table_Init */
static void bcm43xx_gphy_gain_lt_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 nr_written = 0;
	u16 tmp;
	u8 rf, bb;

	if (!lo->lo_measured) {
		bcm43xx_phy_write(dev, 0x3FF, 0);
		return;
	}

	for (rf = 0; rf < lo->rfatt_list.len; rf++) {
		for (bb = 0; bb < lo->bbatt_list.len; bb++) {
			if (nr_written >= 0x40)
				return;
			tmp = lo->bbatt_list.list[bb].att;
			tmp <<= 8;
			if (phy->radio_rev == 8)
				tmp |= 0x50;
			else
				tmp |= 0x40;
			tmp |= lo->rfatt_list.list[rf].att;
			bcm43xx_phy_write(dev, 0x3C0 + nr_written,
					  tmp);
			nr_written++;
		}
	}
}

/* GPHY_DC_Lookup_Table */
void bcm43xx_gphy_dc_lt_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	struct bcm43xx_loctl *loctl0;
	struct bcm43xx_loctl *loctl1;
	int i;
	int rf_offset, bb_offset;
	u16 tmp;

	for (i = 0;
	     i < lo->rfatt_list.len + lo->bbatt_list.len;
	     i += 2) {
		rf_offset = i / lo->rfatt_list.len;
		bb_offset = i % lo->rfatt_list.len;

		loctl0 = bcm43xx_get_lo_g_ctl(dev, &lo->rfatt_list.list[rf_offset],
					   &lo->bbatt_list.list[bb_offset]);
		if (i + 1 < lo->rfatt_list.len * lo->bbatt_list.len) {
			rf_offset = (i + 1) / lo->rfatt_list.len;
			bb_offset = (i + 1) % lo->rfatt_list.len;

			loctl1 = bcm43xx_get_lo_g_ctl(dev, &lo->rfatt_list.list[rf_offset],
						   &lo->bbatt_list.list[bb_offset]);
		} else
			loctl1 = loctl0;

		tmp =  ((u16)loctl0->q & 0xF);
		tmp |= ((u16)loctl0->i & 0xF) << 4;
		tmp |= ((u16)loctl1->q & 0xF) << 8;
		tmp |= ((u16)loctl1->i & 0xF) << 12;//FIXME?
		bcm43xx_phy_write(dev, 0x3A0 + (i / 2),
				  tmp);
	}
}

static void hardware_pctl_init_aphy(struct bcm43xx_wldev *dev)
{
	//TODO
}

static void hardware_pctl_init_gphy(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	bcm43xx_phy_write(dev, 0x0036,
			  (bcm43xx_phy_read(dev, 0x0036) & 0xFFC0)
			  | (phy->tgt_idle_tssi - phy->cur_idle_tssi));
	bcm43xx_phy_write(dev, 0x0478,
			  (bcm43xx_phy_read(dev, 0x0478) & 0xFF00)
			  | (phy->tgt_idle_tssi - phy->cur_idle_tssi));
	bcm43xx_gphy_tssi_power_lt_init(dev);
	bcm43xx_gphy_gain_lt_init(dev);
	bcm43xx_phy_write(dev, 0x0060,
			  bcm43xx_phy_read(dev, 0x0060) & 0xFFBF);
	bcm43xx_phy_write(dev, 0x0014, 0x0000);

	assert(phy->rev >= 6);
	bcm43xx_phy_write(dev, 0x0478,
			  bcm43xx_phy_read(dev, 0x0478)
			  | 0x0800);
	bcm43xx_phy_write(dev, 0x0478,
			  bcm43xx_phy_read(dev, 0x0478)
			  & 0xFEFF);
	bcm43xx_phy_write(dev, 0x0801,
			  bcm43xx_phy_read(dev, 0x0801)
			  & 0xFFBF);

	bcm43xx_gphy_dc_lt_init(dev);
}

/* HardwarePowerControl for A and G PHY.
 * This does nothing, if the card does not have HW PCTL
 */
static void bcm43xx_hardware_pctl_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (!has_hardware_pctl(phy))
		return;
	if (phy->type == BCM43xx_PHYTYPE_A) {
		hardware_pctl_init_aphy(dev);
		return;
	}
	if (phy->type == BCM43xx_PHYTYPE_G) {
		hardware_pctl_init_gphy(dev);
		return;
	}
	assert(0);
}

static void bcm43xx_hardware_pctl_early_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (!has_hardware_pctl(phy)) {
		bcm43xx_phy_write(dev, 0x047A, 0xC111);
		return;
	}

	bcm43xx_phy_write(dev, 0x0036,
			  bcm43xx_phy_read(dev, 0x0036) & 0xFEFF);
	bcm43xx_phy_write(dev, 0x002F, 0x0202);
	bcm43xx_phy_write(dev, 0x047C,
			  bcm43xx_phy_read(dev, 0x047C) | 0x0002);
	bcm43xx_phy_write(dev, 0x047A,
			  bcm43xx_phy_read(dev, 0x047A) | 0xF000);
	if (phy->radio_ver == 0x2050 && phy->radio_rev == 8) {
		bcm43xx_phy_write(dev, 0x047A,
				  (bcm43xx_phy_read(dev, 0x047A)
				   & 0xFF0F) | 0x0010);
		bcm43xx_phy_write(dev, 0x005D,
				  bcm43xx_phy_read(dev, 0x005D)
				   | 0x8000);
		bcm43xx_phy_write(dev, 0x004E,
				  (bcm43xx_phy_read(dev, 0x004E)
				   & 0xFFC0) | 0x0010);
		bcm43xx_phy_write(dev, 0x002E, 0xC07F);
		bcm43xx_phy_write(dev, 0x0036,
				  bcm43xx_phy_read(dev, 0x0036)
				   | 0x0400);
	} else {
		bcm43xx_phy_write(dev, 0x0036,
				  bcm43xx_phy_read(dev, 0x0036)
				   | 0x0200);
		bcm43xx_phy_write(dev, 0x0036,
				  bcm43xx_phy_read(dev, 0x0036)
				   | 0x0400);
		bcm43xx_phy_write(dev, 0x005D,
				  bcm43xx_phy_read(dev, 0x005D)
				   & 0x7FFF);
		bcm43xx_phy_write(dev, 0x004F,
				  bcm43xx_phy_read(dev, 0x004F)
				   & 0xFFFE);
		bcm43xx_phy_write(dev, 0x004E,
				  (bcm43xx_phy_read(dev, 0x004E)
				   & 0xFFC0) | 0x0010);
		bcm43xx_phy_write(dev, 0x002E, 0xC07F);
		bcm43xx_phy_write(dev, 0x047A,
				  (bcm43xx_phy_read(dev, 0x047A)
				   & 0xFF0F) | 0x0010);
	}
}

/* Intialize B/G PHY power control
 * as described in http://bcm-specs.sipsolutions.net/InitPowerControl
 */
static void bcm43xx_phy_init_pctl(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_rfatt old_rfatt;
	struct bcm43xx_bbatt old_bbatt;
	u8 old_tx_control = 0;

	if ((bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM) &&
	    (bus->boardinfo.type == SSB_BOARD_BU4306))
		return;

	bcm43xx_phy_write(dev, 0x0028, 0x8018);

	/* This does something with the Analog... */
	bcm43xx_write16(dev, BCM43xx_MMIO_PHY0,
			bcm43xx_read16(dev, BCM43xx_MMIO_PHY0)
			& 0xFFDF);

	if (phy->type == BCM43xx_PHYTYPE_G && !phy->gmode)
		return;
	bcm43xx_hardware_pctl_early_init(dev);
	if (phy->cur_idle_tssi == 0) {
		if (phy->radio_ver == 0x2050 && phy->analog == 0) {
			bcm43xx_radio_write16(dev, 0x0076,
					      (bcm43xx_radio_read16(dev, 0x0076)
					       & 0x00F7) | 0x0084);
		} else {
			struct bcm43xx_rfatt rfatt;
			struct bcm43xx_bbatt bbatt;

			memcpy(&old_rfatt, &phy->rfatt, sizeof(old_rfatt));
			memcpy(&old_bbatt, &phy->bbatt, sizeof(old_bbatt));
			old_tx_control = phy->tx_control;

			bbatt.att = 11;
			if (phy->radio_rev == 8) {
				rfatt.att = 15;
				rfatt.with_padmix = 1;
			} else {
				rfatt.att = 9;
				rfatt.with_padmix = 0;
			}
			bcm43xx_set_txpower_g(dev, &bbatt, &rfatt, 0);
		}
		bcm43xx_dummy_transmission(dev);
		phy->cur_idle_tssi = bcm43xx_phy_read(dev, BCM43xx_PHY_ITSSI);
		if (BCM43xx_DEBUG) {
			/* Current-Idle-TSSI sanity check. */
			if (abs(phy->cur_idle_tssi - phy->tgt_idle_tssi) >= 20) {
				dprintk(KERN_ERR PFX "!WARNING! Idle-TSSI phy->cur_idle_tssi "
					"measuring failed. (cur=%d, tgt=%d). Disabling TX power "
					"adjustment.\n", phy->cur_idle_tssi, phy->tgt_idle_tssi);
				phy->cur_idle_tssi = 0;
			}
		}
		if (phy->radio_ver == 0x2050 && phy->analog == 0) {
			bcm43xx_radio_write16(dev, 0x0076,
					      bcm43xx_radio_read16(dev, 0x0076)
					       & 0xFF7B);
		} else {
			bcm43xx_set_txpower_g(dev, &old_bbatt,
					      &old_rfatt, old_tx_control);
		}
	}
	bcm43xx_hardware_pctl_init(dev);
	bcm43xx_shm_clear_tssi(dev);
}

static void bcm43xx_phy_agcsetup(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 offset = 0x0000;

	if (phy->rev == 1)
		offset = 0x4C00;

	bcm43xx_ofdmtab_write16(dev, offset, 0, 0x00FE);
	bcm43xx_ofdmtab_write16(dev, offset, 1, 0x000D);
	bcm43xx_ofdmtab_write16(dev, offset, 2, 0x0013);
	bcm43xx_ofdmtab_write16(dev, offset, 3, 0x0019);

	if (phy->rev == 1) {
		bcm43xx_ofdmtab_write16(dev, 0x1800, 0, 0x2710);
		bcm43xx_ofdmtab_write16(dev, 0x1801, 0, 0x9B83);
		bcm43xx_ofdmtab_write16(dev, 0x1802, 0, 0x9B83);
		bcm43xx_ofdmtab_write16(dev, 0x1803, 0, 0x0F8D);
		bcm43xx_phy_write(dev, 0x0455, 0x0004);
	}

	bcm43xx_phy_write(dev, 0x04A5,
			  (bcm43xx_phy_read(dev, 0x04A5)
			   & 0x00FF) | 0x5700);
	bcm43xx_phy_write(dev, 0x041A,
			  (bcm43xx_phy_read(dev, 0x041A)
			   & 0xFF80) | 0x000F);
	bcm43xx_phy_write(dev, 0x041A,
			  (bcm43xx_phy_read(dev, 0x041A)
			   & 0xC07F) | 0x2B80);
	bcm43xx_phy_write(dev, 0x048C,
			  (bcm43xx_phy_read(dev, 0x048C)
			   & 0xF0FF) | 0x0300);

	bcm43xx_radio_write16(dev, 0x007A,
			      bcm43xx_radio_read16(dev, 0x007A)
			      | 0x0008);

	bcm43xx_phy_write(dev, 0x04A0,
			  (bcm43xx_phy_read(dev, 0x04A0)
			   & 0xFFF0) | 0x0008);
	bcm43xx_phy_write(dev, 0x04A1,
			  (bcm43xx_phy_read(dev, 0x04A1)
			   & 0xF0FF) | 0x0600);
	bcm43xx_phy_write(dev, 0x04A2,
			  (bcm43xx_phy_read(dev, 0x04A2)
			   & 0xF0FF) | 0x0700);
	bcm43xx_phy_write(dev, 0x04A0,
			  (bcm43xx_phy_read(dev, 0x04A0)
			   & 0xF0FF) | 0x0100);

	if (phy->rev == 1) {
		bcm43xx_phy_write(dev, 0x04A2,
				  (bcm43xx_phy_read(dev, 0x04A2)
				   & 0xFFF0) | 0x0007);
	}

	bcm43xx_phy_write(dev, 0x0488,
			  (bcm43xx_phy_read(dev, 0x0488)
			   & 0xFF00) | 0x001C);
	bcm43xx_phy_write(dev, 0x0488,
			  (bcm43xx_phy_read(dev, 0x0488)
			   & 0xC0FF) | 0x0200);
	bcm43xx_phy_write(dev, 0x0496,
			  (bcm43xx_phy_read(dev, 0x0496)
			   & 0xFF00) | 0x001C);
	bcm43xx_phy_write(dev, 0x0489,
			  (bcm43xx_phy_read(dev, 0x0489)
			   & 0xFF00) | 0x0020);
	bcm43xx_phy_write(dev, 0x0489,
			  (bcm43xx_phy_read(dev, 0x0489)
			   & 0xC0FF) | 0x0200);
	bcm43xx_phy_write(dev, 0x0482,
			  (bcm43xx_phy_read(dev, 0x0482)
			   & 0xFF00) | 0x002E);
	bcm43xx_phy_write(dev, 0x0496,
			  (bcm43xx_phy_read(dev, 0x0496)
			   & 0x00FF) | 0x1A00);
	bcm43xx_phy_write(dev, 0x0481,
			  (bcm43xx_phy_read(dev, 0x0481)
			   & 0xFF00) | 0x0028);
	bcm43xx_phy_write(dev, 0x0481,
			  (bcm43xx_phy_read(dev, 0x0481)
			   & 0x00FF) | 0x2C00);

	if (phy->rev == 1) {
		bcm43xx_phy_write(dev, 0x0430, 0x092B);
		bcm43xx_phy_write(dev, 0x041B,
				  (bcm43xx_phy_read(dev, 0x041B)
				   & 0xFFE1) | 0x0002);
	} else {
		bcm43xx_phy_write(dev, 0x041B,
				  bcm43xx_phy_read(dev, 0x041B)
				  & 0xFFE1);
		bcm43xx_phy_write(dev, 0x041F, 0x287A);
		bcm43xx_phy_write(dev, 0x0420,
				  (bcm43xx_phy_read(dev, 0x0420)
				   & 0xFFF0) | 0x0004);
	}

	if (phy->rev >= 6) {
		bcm43xx_phy_write(dev, 0x0422, 0x287A);
		bcm43xx_phy_write(dev, 0x0420,
				  (bcm43xx_phy_read(dev, 0x0420)
				   & 0x0FFF) | 0x3000);
	}

	bcm43xx_phy_write(dev, 0x04A8,
			  (bcm43xx_phy_read(dev, 0x04A8)
			   & 0x8080) | 0x7874);
	bcm43xx_phy_write(dev, 0x048E, 0x1C00);

	offset = 0x0800;
	if (phy->rev == 1) {
		offset = 0x5400;
		bcm43xx_phy_write(dev, 0x04AB,
				  (bcm43xx_phy_read(dev, 0x04AB)
				   & 0xF0FF) | 0x0600);
		bcm43xx_phy_write(dev, 0x048B, 0x005E);
		bcm43xx_phy_write(dev, 0x048C,
				  (bcm43xx_phy_read(dev, 0x048C)
				   & 0xFF00) | 0x001E);
		bcm43xx_phy_write(dev, 0x048D, 0x0002);
	}
	bcm43xx_ofdmtab_write16(dev, offset, 0, 0x00);
	bcm43xx_ofdmtab_write16(dev, offset, 1, 0x07);
	bcm43xx_ofdmtab_write16(dev, offset, 2, 0x10);
	bcm43xx_ofdmtab_write16(dev, offset, 3, 0x1C);

	if (phy->rev >= 6) {
		bcm43xx_phy_write(dev, 0x0426,
				  bcm43xx_phy_read(dev, 0x0426)
				  & 0xFFFC);
		bcm43xx_phy_write(dev, 0x0426,
				  bcm43xx_phy_read(dev, 0x0426)
				  & 0xEFFF);
	}
}

static void bcm43xx_phy_setupg(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;
	u16 i;

	assert(phy->type == BCM43xx_PHYTYPE_G);
	if (phy->rev == 1) {
		bcm43xx_phy_write(dev, 0x0406, 0x4F19);
		bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
				  (bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) & 0xFC3F) | 0x0340);
		bcm43xx_phy_write(dev, 0x042C, 0x005A);
		bcm43xx_phy_write(dev, 0x0427, 0x001A);

		for (i = 0; i < BCM43xx_TAB_FINEFREQG_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x5800, i, bcm43xx_tab_finefreqg[i]);
		for (i = 0; i < BCM43xx_TAB_NOISEG1_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1800, i, bcm43xx_tab_noiseg1[i]);
		for (i = 0; i < BCM43xx_TAB_ROTOR_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x2000, i, bcm43xx_tab_rotor[i]);
	} else {
		/* nrssi values are signed 6-bit values. Not sure why we write 0x7654 here... */
		bcm43xx_nrssi_hw_write(dev, 0xBA98, (s16)0x7654);

		if (phy->rev == 2) {
			bcm43xx_phy_write(dev, 0x04C0, 0x1861);
			bcm43xx_phy_write(dev, 0x04C1, 0x0271);
		} else if (phy->rev > 2) {
			bcm43xx_phy_write(dev, 0x04C0, 0x0098);
			bcm43xx_phy_write(dev, 0x04C1, 0x0070);
			bcm43xx_phy_write(dev, 0x04C9, 0x0080);
		}
		bcm43xx_phy_write(dev, 0x042B, bcm43xx_phy_read(dev, 0x042B) | 0x800);

		for (i = 0; i < 64; i++)
			bcm43xx_ofdmtab_write16(dev, 0x4000, i, i);
		for (i = 0; i < BCM43xx_TAB_NOISEG2_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1800, i, bcm43xx_tab_noiseg2[i]);
	}

	if (phy->rev <= 2)
		for (i = 0; i < BCM43xx_TAB_NOISESCALEG_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, bcm43xx_tab_noisescaleg1[i]);
	else if ((phy->rev >= 7) && (bcm43xx_phy_read(dev, 0x0449) & 0x0200))
		for (i = 0; i < BCM43xx_TAB_NOISESCALEG_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, bcm43xx_tab_noisescaleg3[i]);
	else
		for (i = 0; i < BCM43xx_TAB_NOISESCALEG_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, bcm43xx_tab_noisescaleg2[i]);

	if (phy->rev == 2)
		for (i = 0; i < BCM43xx_TAB_SIGMASQR_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x5000, i, bcm43xx_tab_sigmasqr1[i]);
	else if ((phy->rev > 2) && (phy->rev <= 8))
		for (i = 0; i < BCM43xx_TAB_SIGMASQR_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x5000, i, bcm43xx_tab_sigmasqr2[i]);

	if (phy->rev == 1) {
		for (i = 0; i < BCM43xx_TAB_RETARD_SIZE; i++)
			bcm43xx_ofdmtab_write32(dev, 0x2400, i, bcm43xx_tab_retard[i]);
		for (i = 4; i < 20; i++)
			bcm43xx_ofdmtab_write16(dev, 0x5400, i, 0x0020);
		bcm43xx_phy_agcsetup(dev);

		if ((bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM) &&
		    (bus->boardinfo.type == SSB_BOARD_BU4306) &&
		    (bus->boardinfo.rev == 0x17))
			return;

		bcm43xx_ofdmtab_write16(dev, 0x5001, 0, 0x0002);
		bcm43xx_ofdmtab_write16(dev, 0x5002, 0, 0x0001);
	} else {
		for (i = 0; i < 0x20; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1000, i, 0x0820);
		bcm43xx_phy_agcsetup(dev);
		bcm43xx_phy_read(dev, 0x0400); /* dummy read */
		bcm43xx_phy_write(dev, 0x0403, 0x1000);
		bcm43xx_ofdmtab_write16(dev, 0x3C02, 0, 0x000F);
		bcm43xx_ofdmtab_write16(dev, 0x3C03, 0, 0x0014);

		if ((bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM) &&
		    (bus->boardinfo.type == SSB_BOARD_BU4306) &&
		    (bus->boardinfo.rev == 0x17))
			return;

		bcm43xx_ofdmtab_write16(dev, 0x0401, 0, 0x0002);
		bcm43xx_ofdmtab_write16(dev, 0x0402, 0, 0x0001);
	}
}

/* Initialize the noisescaletable for APHY */
static void bcm43xx_phy_init_noisescaletbl(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int i;

	for (i = 0; i < 12; i++) {
		if (phy->rev == 2)
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x6767);
		else
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x2323);
	}
	if (phy->rev == 2)
		bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x6700);
	else
		bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x2300);
	for (i = 0; i < 11; i++) {
		if (phy->rev == 2)
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x6767);
		else
			bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x2323);
	}
	if (phy->rev == 2)
		bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x0067);
	else
		bcm43xx_ofdmtab_write16(dev, 0x1400, i, 0x0023);
}

static void bcm43xx_phy_setupa(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 i;

	assert(phy->type == BCM43xx_PHYTYPE_A);
	switch (phy->rev) {
	case 2:
		bcm43xx_phy_write(dev, 0x008E, 0x3800);
		bcm43xx_phy_write(dev, 0x0035, 0x03FF);
		bcm43xx_phy_write(dev, 0x0036, 0x0400);

		bcm43xx_ofdmtab_write16(dev, 0x3807, 0, 0x0051);

		bcm43xx_phy_write(dev, 0x001C, 0x0FF9);
		bcm43xx_phy_write(dev, 0x0020, bcm43xx_phy_read(dev, 0x0020) & 0xFF0F);
		bcm43xx_ofdmtab_write16(dev, 0x3C0C, 0, 0x07BF);
		bcm43xx_radio_write16(dev, 0x0002, 0x07BF);

		bcm43xx_phy_write(dev, 0x0024, 0x4680);
		bcm43xx_phy_write(dev, 0x0020, 0x0003);
		bcm43xx_phy_write(dev, 0x001D, 0x0F40);
		bcm43xx_phy_write(dev, 0x001F, 0x1C00);

		bcm43xx_phy_write(dev, 0x002A,
				  (bcm43xx_phy_read(dev, 0x002A)
				   & 0x00FF) | 0x0400);
		bcm43xx_phy_write(dev, 0x002B,
				  bcm43xx_phy_read(dev, 0x002B)
				  & 0xFBFF);
		bcm43xx_phy_write(dev, 0x008E, 0x58C1);

		bcm43xx_ofdmtab_write16(dev, 0x0803, 0, 0x000F);
		bcm43xx_ofdmtab_write16(dev, 0x0804, 0, 0x001F);
		bcm43xx_ofdmtab_write16(dev, 0x0805, 0, 0x002A);
		bcm43xx_ofdmtab_write16(dev, 0x0805, 0, 0x0030);
		bcm43xx_ofdmtab_write16(dev, 0x0807, 0, 0x003A);

		bcm43xx_ofdmtab_write16(dev, 0x0000, 0, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 1, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 2, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 3, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 4, 0x0015);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 5, 0x0015);
		bcm43xx_ofdmtab_write16(dev, 0x0000, 6, 0x0019);

		bcm43xx_ofdmtab_write16(dev, 0x0404, 0, 0x0003);
		bcm43xx_ofdmtab_write16(dev, 0x0405, 0, 0x0003);
		bcm43xx_ofdmtab_write16(dev, 0x0406, 0, 0x0007);

		for (i = 0; i < 16; i++)
			bcm43xx_ofdmtab_write16(dev, 0x4000, i, (0x8 + i) & 0x000F);

		bcm43xx_ofdmtab_write16(dev, 0x3003, 0, 0x1044);
		bcm43xx_ofdmtab_write16(dev, 0x3004, 0, 0x7201);
		bcm43xx_ofdmtab_write16(dev, 0x3006, 0, 0x0040);
		bcm43xx_ofdmtab_write16(dev, 0x3001, 0, (bcm43xx_ofdmtab_read16(dev, 0x3001, 0) & 0x0010) | 0x0008);

		for (i = 0; i < BCM43xx_TAB_FINEFREQA_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x5800, i, bcm43xx_tab_finefreqa[i]);
		for (i = 0; i < BCM43xx_TAB_NOISEA2_SIZE; i++)
			bcm43xx_ofdmtab_write16(dev, 0x1800, i, bcm43xx_tab_noisea2[i]);
		for (i = 0; i < BCM43xx_TAB_ROTOR_SIZE; i++)
			bcm43xx_ofdmtab_write32(dev, 0x2000, i, bcm43xx_tab_rotor[i]);
		bcm43xx_phy_init_noisescaletbl(dev);
		for (i = 0; i < BCM43xx_TAB_RETARD_SIZE; i++)
			bcm43xx_ofdmtab_write32(dev, 0x2400, i, bcm43xx_tab_retard[i]);
		break;
	case 3:
		for (i = 0; i < 64; i++)
			bcm43xx_ofdmtab_write16(dev, 0x4000, i, i);

		bcm43xx_ofdmtab_write16(dev, 0x3807, 0, 0x0051);

		bcm43xx_phy_write(dev, 0x001C, 0x0FF9);
		bcm43xx_phy_write(dev, 0x0020,
				  bcm43xx_phy_read(dev, 0x0020) & 0xFF0F);
		bcm43xx_radio_write16(dev, 0x0002, 0x07BF);

		bcm43xx_phy_write(dev, 0x0024, 0x4680);
		bcm43xx_phy_write(dev, 0x0020, 0x0003);
		bcm43xx_phy_write(dev, 0x001D, 0x0F40);
		bcm43xx_phy_write(dev, 0x001F, 0x1C00);
		bcm43xx_phy_write(dev, 0x002A,
				  (bcm43xx_phy_read(dev, 0x002A)
				   & 0x00FF) | 0x0400);

		bcm43xx_ofdmtab_write16(dev, 0x3000, 1,
				        (bcm43xx_ofdmtab_read16(dev, 0x3000, 1)
				        & 0x0010) | 0x0008);
		for (i = 0; i < BCM43xx_TAB_NOISEA3_SIZE; i++) {
			bcm43xx_ofdmtab_write16(dev, 0x1800, i,
						bcm43xx_tab_noisea3[i]);
		}
		bcm43xx_phy_init_noisescaletbl(dev);
		for (i = 0; i < BCM43xx_TAB_SIGMASQR_SIZE; i++) {
			bcm43xx_ofdmtab_write16(dev, 0x5000, i,
						bcm43xx_tab_sigmasqr1[i]);
		}

		bcm43xx_phy_write(dev, 0x0003, 0x1808);

		bcm43xx_ofdmtab_write16(dev, 0x0803, 0, 0x000F);
		bcm43xx_ofdmtab_write16(dev, 0x0804, 0, 0x001F);
		bcm43xx_ofdmtab_write16(dev, 0x0805, 0, 0x002A);
		bcm43xx_ofdmtab_write16(dev, 0x0805, 0, 0x0030);
		bcm43xx_ofdmtab_write16(dev, 0x0807, 0, 0x003A);

		bcm43xx_ofdmtab_write16(dev, 0x0000, 0, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0001, 0, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0002, 0, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0003, 0, 0x0013);
		bcm43xx_ofdmtab_write16(dev, 0x0004, 0, 0x0015);
		bcm43xx_ofdmtab_write16(dev, 0x0005, 0, 0x0015);
		bcm43xx_ofdmtab_write16(dev, 0x0006, 0, 0x0019);

		bcm43xx_ofdmtab_write16(dev, 0x0404, 0, 0x0003);
		bcm43xx_ofdmtab_write16(dev, 0x0405, 0, 0x0003);
		bcm43xx_ofdmtab_write16(dev, 0x0406, 0, 0x0007);

		bcm43xx_ofdmtab_write16(dev, 0x3C02, 0, 0x000F);
		bcm43xx_ofdmtab_write16(dev, 0x3C03, 0, 0x0014);
		break;
	default:
		assert(0);
	}
}

/* Initialize APHY. This is also called for the GPHY in some cases. */
static void bcm43xx_phy_inita(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;
	u16 tval;

	might_sleep();

	if (phy->type == BCM43xx_PHYTYPE_A) {
		bcm43xx_phy_setupa(dev);
	} else {
		bcm43xx_phy_setupg(dev);
		if (phy->gmode &&
		    (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_PACTRL))
			bcm43xx_phy_write(dev, 0x046E, 0x03CF);
		return;
	}

	bcm43xx_phy_write(dev, BCM43xx_PHY_A_CRS,
	                  (bcm43xx_phy_read(dev, BCM43xx_PHY_A_CRS) & 0xF83C) | 0x0340);
	bcm43xx_phy_write(dev, 0x0034, 0x0001);

	TODO();//TODO: RSSI AGC
	bcm43xx_phy_write(dev, BCM43xx_PHY_A_CRS,
	                  bcm43xx_phy_read(dev, BCM43xx_PHY_A_CRS) | (1 << 14));
	bcm43xx_radio_init2060(dev);

	if ((bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM) &&
	    ((bus->boardinfo.type == SSB_BOARD_BU4306) ||
	     (bus->boardinfo.type == SSB_BOARD_BU4309))) {
		if (phy->lofcal == 0xFFFF) {
			TODO();//TODO: LOF Cal
			bcm43xx_radio_set_tx_iq(dev);
		} else
			bcm43xx_radio_write16(dev, 0x001E, phy->lofcal);
	}

	bcm43xx_phy_write(dev, 0x007A, 0xF111);

	if (phy->cur_idle_tssi == 0) {
		bcm43xx_radio_write16(dev, 0x0019, 0x0000);
		bcm43xx_radio_write16(dev, 0x0017, 0x0020);

		tval = bcm43xx_ofdmtab_read16(dev, 0x3001, 0);
		if (phy->rev == 1) {
			bcm43xx_ofdmtab_write16(dev, 0x3001, 0,
					  (bcm43xx_ofdmtab_read16(dev, 0x3001, 0) & 0xFF87)
					  | 0x0058);
		} else {
			bcm43xx_ofdmtab_write16(dev, 0x3001, 0,
					  (bcm43xx_ofdmtab_read16(dev, 0x3001, 0) & 0xFFC3)
					  | 0x002C);
		}
		bcm43xx_dummy_transmission(dev);
		phy->cur_idle_tssi = bcm43xx_phy_read(dev, BCM43xx_PHY_A_PCTL);
		bcm43xx_ofdmtab_write16(dev, 0x3001, 0, tval);

		bcm43xx_radio_set_txpower_a(dev, 0x0018);
	}
	bcm43xx_shm_clear_tssi(dev);
}

static void bcm43xx_phy_initb2(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 offset, val;

	bcm43xx_write16(dev, 0x03EC, 0x3F22);
	bcm43xx_phy_write(dev, 0x0020, 0x301C);
	bcm43xx_phy_write(dev, 0x0026, 0x0000);
	bcm43xx_phy_write(dev, 0x0030, 0x00C6);
	bcm43xx_phy_write(dev, 0x0088, 0x3E00);
	val = 0x3C3D;
	for (offset = 0x0089; offset < 0x00A7; offset++) {
		bcm43xx_phy_write(dev, offset, val);
		val -= 0x0202;
	}
	bcm43xx_phy_write(dev, 0x03E4, 0x3000);
	if (phy->channel == 0xFF)
		bcm43xx_radio_selectchannel(dev, BCM43xx_DEFAULT_CHANNEL_BG, 0);
	else
		bcm43xx_radio_selectchannel(dev, phy->channel, 0);
	if (phy->radio_ver != 0x2050) {
		bcm43xx_radio_write16(dev, 0x0075, 0x0080);
		bcm43xx_radio_write16(dev, 0x0079, 0x0081);
	}
	bcm43xx_radio_write16(dev, 0x0050, 0x0020);
	bcm43xx_radio_write16(dev, 0x0050, 0x0023);
	if (phy->radio_ver == 0x2050) {
		bcm43xx_radio_write16(dev, 0x0050, 0x0020);
		bcm43xx_radio_write16(dev, 0x005A, 0x0070);
		bcm43xx_radio_write16(dev, 0x005B, 0x007B);
		bcm43xx_radio_write16(dev, 0x005C, 0x00B0);
		bcm43xx_radio_write16(dev, 0x007A, 0x000F);
		bcm43xx_phy_write(dev, 0x0038, 0x0677);
		bcm43xx_radio_init2050(dev);
	}
	bcm43xx_phy_write(dev, 0x0014, 0x0080);
	bcm43xx_phy_write(dev, 0x0032, 0x00CA);
	bcm43xx_phy_write(dev, 0x0032, 0x00CC);
	bcm43xx_phy_write(dev, 0x0035, 0x07C2);
	bcm43xx_lo_b_measure(dev);
	bcm43xx_phy_write(dev, 0x0026, 0xCC00);
	if (phy->radio_ver != 0x2050)
		bcm43xx_phy_write(dev, 0x0026, 0xCE00);
	bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT, 0x1000);
	bcm43xx_phy_write(dev, 0x002A, 0x88A3);
	if (phy->radio_ver != 0x2050)
		bcm43xx_phy_write(dev, 0x002A, 0x88C2);
	bcm43xx_set_txpower_g(dev, &phy->bbatt, &phy->rfatt, phy->tx_control);
	bcm43xx_phy_init_pctl(dev);
}

static void bcm43xx_phy_initb4(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 offset, val;

	bcm43xx_write16(dev, 0x03EC, 0x3F22);
	bcm43xx_phy_write(dev, 0x0020, 0x301C);
	bcm43xx_phy_write(dev, 0x0026, 0x0000);
	bcm43xx_phy_write(dev, 0x0030, 0x00C6);
	bcm43xx_phy_write(dev, 0x0088, 0x3E00);
	val = 0x3C3D;
	for (offset = 0x0089; offset < 0x00A7; offset++) {
		bcm43xx_phy_write(dev, offset, val);
		val -= 0x0202;
	}
	bcm43xx_phy_write(dev, 0x03E4, 0x3000);
	if (phy->channel == 0xFF)
		bcm43xx_radio_selectchannel(dev, BCM43xx_DEFAULT_CHANNEL_BG, 0);
	else
		bcm43xx_radio_selectchannel(dev, phy->channel, 0);
	if (phy->radio_ver != 0x2050) {
		bcm43xx_radio_write16(dev, 0x0075, 0x0080);
		bcm43xx_radio_write16(dev, 0x0079, 0x0081);
	}
	bcm43xx_radio_write16(dev, 0x0050, 0x0020);
	bcm43xx_radio_write16(dev, 0x0050, 0x0023);
	if (phy->radio_ver == 0x2050) {
		bcm43xx_radio_write16(dev, 0x0050, 0x0020);
		bcm43xx_radio_write16(dev, 0x005A, 0x0070);
		bcm43xx_radio_write16(dev, 0x005B, 0x007B);
		bcm43xx_radio_write16(dev, 0x005C, 0x00B0);
		bcm43xx_radio_write16(dev, 0x007A, 0x000F);
		bcm43xx_phy_write(dev, 0x0038, 0x0677);
		bcm43xx_radio_init2050(dev);
	}
	bcm43xx_phy_write(dev, 0x0014, 0x0080);
	bcm43xx_phy_write(dev, 0x0032, 0x00CA);
	if (phy->radio_ver == 0x2050)
		bcm43xx_phy_write(dev, 0x0032, 0x00E0);
	bcm43xx_phy_write(dev, 0x0035, 0x07C2);

	bcm43xx_lo_b_measure(dev);

	bcm43xx_phy_write(dev, 0x0026, 0xCC00);
	if (phy->radio_ver == 0x2050)
		bcm43xx_phy_write(dev, 0x0026, 0xCE00);
	bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT, 0x1100);
	bcm43xx_phy_write(dev, 0x002A, 0x88A3);
	if (phy->radio_ver == 0x2050)
		bcm43xx_phy_write(dev, 0x002A, 0x88C2);
	bcm43xx_set_txpower_g(dev, &phy->bbatt, &phy->rfatt, phy->tx_control);
	if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI) {
		bcm43xx_calc_nrssi_slope(dev);
		bcm43xx_calc_nrssi_threshold(dev);
	}
	bcm43xx_phy_init_pctl(dev);
}

static void bcm43xx_phy_initb5(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;
	u16 offset, value;
	u8 old_channel;

	if (phy->analog == 1) {
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A)
				      | 0x0050);
	}
	if ((bus->boardinfo.vendor != SSB_BOARDVENDOR_BCM) &&
	    (bus->boardinfo.type != SSB_BOARD_BU4306)) {
		value = 0x2120;
		for (offset = 0x00A8 ; offset < 0x00C7; offset++) {
			bcm43xx_phy_write(dev, offset, value);
			value += 0x202;
		}
	}
	bcm43xx_phy_write(dev, 0x0035,
			  (bcm43xx_phy_read(dev, 0x0035) & 0xF0FF)
			  | 0x0700);
	if (phy->radio_ver == 0x2050)
		bcm43xx_phy_write(dev, 0x0038, 0x0667);

	if (phy->gmode || phy->rev >= 2) {
		if (phy->radio_ver == 0x2050) {
			bcm43xx_radio_write16(dev, 0x007A,
					      bcm43xx_radio_read16(dev, 0x007A)
					      | 0x0020);
			bcm43xx_radio_write16(dev, 0x0051,
					      bcm43xx_radio_read16(dev, 0x0051)
					      | 0x0004);
		}
		bcm43xx_write16(dev, BCM43xx_MMIO_PHY_RADIO, 0x0000);

		bcm43xx_phy_write(dev, 0x0802, bcm43xx_phy_read(dev, 0x0802) | 0x0100);
		bcm43xx_phy_write(dev, 0x042B, bcm43xx_phy_read(dev, 0x042B) | 0x2000);

		bcm43xx_phy_write(dev, 0x001C, 0x186A);

		bcm43xx_phy_write(dev, 0x0013, (bcm43xx_phy_read(dev, 0x0013) & 0x00FF) | 0x1900);
		bcm43xx_phy_write(dev, 0x0035, (bcm43xx_phy_read(dev, 0x0035) & 0xFFC0) | 0x0064);
		bcm43xx_phy_write(dev, 0x005D, (bcm43xx_phy_read(dev, 0x005D) & 0xFF80) | 0x000A);
	}

	if (dev->bad_frames_preempt) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RADIO_BITFIELD,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_RADIO_BITFIELD) | (1 << 11));
	}

	if (phy->analog == 1) {
		bcm43xx_phy_write(dev, 0x0026, 0xCE00);
		bcm43xx_phy_write(dev, 0x0021, 0x3763);
		bcm43xx_phy_write(dev, 0x0022, 0x1BC3);
		bcm43xx_phy_write(dev, 0x0023, 0x06F9);
		bcm43xx_phy_write(dev, 0x0024, 0x037E);
	} else
		bcm43xx_phy_write(dev, 0x0026, 0xCC00);
	bcm43xx_phy_write(dev, 0x0030, 0x00C6);
	bcm43xx_write16(dev, 0x03EC, 0x3F22);

	if (phy->analog == 1)
		bcm43xx_phy_write(dev, 0x0020, 0x3E1C);
	else
		bcm43xx_phy_write(dev, 0x0020, 0x301C);

	if (phy->analog == 0)
		bcm43xx_write16(dev, 0x03E4, 0x3000);

	old_channel = phy->channel;
	/* Force to channel 7, even if not supported. */
	bcm43xx_radio_selectchannel(dev, 7, 0);

	if (phy->radio_ver != 0x2050) {
		bcm43xx_radio_write16(dev, 0x0075, 0x0080);
		bcm43xx_radio_write16(dev, 0x0079, 0x0081);
	}

	bcm43xx_radio_write16(dev, 0x0050, 0x0020);
	bcm43xx_radio_write16(dev, 0x0050, 0x0023);

	if (phy->radio_ver == 0x2050) {
		bcm43xx_radio_write16(dev, 0x0050, 0x0020);
		bcm43xx_radio_write16(dev, 0x005A, 0x0070);
	}

	bcm43xx_radio_write16(dev, 0x005B, 0x007B);
	bcm43xx_radio_write16(dev, 0x005C, 0x00B0);

	bcm43xx_radio_write16(dev, 0x007A, bcm43xx_radio_read16(dev, 0x007A) | 0x0007);

	bcm43xx_radio_selectchannel(dev, old_channel, 0);

	bcm43xx_phy_write(dev, 0x0014, 0x0080);
	bcm43xx_phy_write(dev, 0x0032, 0x00CA);
	bcm43xx_phy_write(dev, 0x002A, 0x88A3);

	bcm43xx_set_txpower_g(dev, &phy->bbatt, &phy->rfatt, phy->tx_control);

	if (phy->radio_ver == 0x2050)
		bcm43xx_radio_write16(dev, 0x005D, 0x000D);

	bcm43xx_write16(dev, 0x03E4, (bcm43xx_read16(dev, 0x03E4) & 0xFFC0) | 0x0004);
}

static void bcm43xx_phy_initb6(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 offset, val;
	u8 old_channel;

	bcm43xx_phy_write(dev, 0x003E, 0x817A);
	bcm43xx_radio_write16(dev, 0x007A,
	                      (bcm43xx_radio_read16(dev, 0x007A) | 0x0058));
	if (phy->radio_rev == 4 || phy->radio_rev == 5) {
		bcm43xx_radio_write16(dev, 0x51, 0x37);
		bcm43xx_radio_write16(dev, 0x52, 0x70);
		bcm43xx_radio_write16(dev, 0x53, 0xB3);
		bcm43xx_radio_write16(dev, 0x54, 0x9B);
		bcm43xx_radio_write16(dev, 0x5A, 0x88);
		bcm43xx_radio_write16(dev, 0x5B, 0x88);
		bcm43xx_radio_write16(dev, 0x5D, 0x88);
		bcm43xx_radio_write16(dev, 0x5E, 0x88);
		bcm43xx_radio_write16(dev, 0x7D, 0x88);
		bcm43xx_hf_write(dev, bcm43xx_hf_read(dev)
				 | BCM43xx_HF_TSSIRPSMW);
	}
	assert(phy->radio_rev != 6 && phy->radio_rev != 7); /* We had code for these revs here...*/
	if (phy->radio_rev == 8) {
		bcm43xx_radio_write16(dev, 0x51, 0);
		bcm43xx_radio_write16(dev, 0x52, 0x40);
		bcm43xx_radio_write16(dev, 0x53, 0xB7);
		bcm43xx_radio_write16(dev, 0x54, 0x98);
		bcm43xx_radio_write16(dev, 0x5A, 0x88);
		bcm43xx_radio_write16(dev, 0x5B, 0x6B);
		bcm43xx_radio_write16(dev, 0x5C, 0x0F);
		if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_ALTIQ) {
			bcm43xx_radio_write16(dev, 0x5D, 0xFA);
			bcm43xx_radio_write16(dev, 0x5E, 0xD8);
		} else {
			bcm43xx_radio_write16(dev, 0x5D, 0xF5);
			bcm43xx_radio_write16(dev, 0x5E, 0xB8);
		}
		bcm43xx_radio_write16(dev, 0x0073, 0x0003);
		bcm43xx_radio_write16(dev, 0x007D, 0x00A8);
		bcm43xx_radio_write16(dev, 0x007C, 0x0001);
		bcm43xx_radio_write16(dev, 0x007E, 0x0008);
	}
	val = 0x1E1F;
	for (offset = 0x0088; offset < 0x0098; offset++) {
		bcm43xx_phy_write(dev, offset, val);
		val -= 0x0202;
	}
	val = 0x3E3F;
	for (offset = 0x0098; offset < 0x00A8; offset++) {
		bcm43xx_phy_write(dev, offset, val);
		val -= 0x0202;
	}
	val = 0x2120;
	for (offset = 0x00A8; offset < 0x00C8; offset++) {
		bcm43xx_phy_write(dev, offset, (val & 0x3F3F));
		val += 0x0202;
	}
	if (phy->type == BCM43xx_PHYTYPE_G) {
		bcm43xx_radio_write16(dev, 0x007A,
		                      bcm43xx_radio_read16(dev, 0x007A) | 0x0020);
		bcm43xx_radio_write16(dev, 0x0051,
		                      bcm43xx_radio_read16(dev, 0x0051) | 0x0004);
		bcm43xx_phy_write(dev, 0x0802,
		                  bcm43xx_phy_read(dev, 0x0802) | 0x0100);
		bcm43xx_phy_write(dev, 0x042B,
		                  bcm43xx_phy_read(dev, 0x042B) | 0x2000);
		bcm43xx_phy_write(dev, 0x5B, 0);
		bcm43xx_phy_write(dev, 0x5C, 0);
	}

	old_channel = phy->channel;
	if (old_channel >= 8)
		bcm43xx_radio_selectchannel(dev, 1, 0);
	else
		bcm43xx_radio_selectchannel(dev, 13, 0);

	bcm43xx_radio_write16(dev, 0x0050, 0x0020);
	bcm43xx_radio_write16(dev, 0x0050, 0x0023);
	udelay(40);
	if (phy->radio_rev < 6 || phy->radio_rev == 8) {
		bcm43xx_radio_write16(dev, 0x7C,
				      (bcm43xx_radio_read16(dev, 0x7C)
				       | 0x0002));
		bcm43xx_radio_write16(dev, 0x50, 0x20);
	}
	if (phy->radio_rev <= 2) {
		bcm43xx_radio_write16(dev, 0x7C, 0x20);
		bcm43xx_radio_write16(dev, 0x5A, 0x70);
		bcm43xx_radio_write16(dev, 0x5B, 0x7B);
		bcm43xx_radio_write16(dev, 0x5C, 0xB0);
	}
	bcm43xx_radio_write16(dev, 0x007A,
	                      (bcm43xx_radio_read16(dev, 0x007A) & 0x00F8) | 0x0007);

	bcm43xx_radio_selectchannel(dev, old_channel, 0);

	bcm43xx_phy_write(dev, 0x0014, 0x0200);
	if (phy->radio_rev >= 6)
		bcm43xx_phy_write(dev, 0x2A, 0x88C2);
	else
		bcm43xx_phy_write(dev, 0x2A, 0x8AC0);
	bcm43xx_phy_write(dev, 0x0038, 0x0668);
	bcm43xx_set_txpower_g(dev, &phy->bbatt, &phy->rfatt, phy->tx_control);
	if (phy->radio_rev <= 5) {
		bcm43xx_phy_write(dev, 0x5D,
				  (bcm43xx_phy_read(dev, 0x5D)
				   & 0xFF80) | 0x0003);
	}
	if (phy->radio_rev <= 2)
		bcm43xx_radio_write16(dev, 0x005D, 0x000D);

	if (phy->analog == 4) {
		bcm43xx_write16(dev, 0x3E4, 9);
		bcm43xx_phy_write(dev, 0x61,
				  bcm43xx_phy_read(dev, 0x61)
				  & 0x0FFF);
	} else {
		bcm43xx_phy_write(dev, 0x0002,
				  (bcm43xx_phy_read(dev, 0x0002) & 0xFFC0)
				  | 0x0004);
	}
	if (phy->type == BCM43xx_PHYTYPE_B) {
		bcm43xx_write16(dev, 0x03E6, 0x8140);
		bcm43xx_phy_write(dev, 0x0016, 0x0410);
		bcm43xx_phy_write(dev, 0x0017, 0x0820);
		bcm43xx_phy_write(dev, 0x0062, 0x0007);
		bcm43xx_radio_init2050(dev);
		bcm43xx_lo_g_measure(dev);
		if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI) {
			bcm43xx_calc_nrssi_slope(dev);
			bcm43xx_calc_nrssi_threshold(dev);
		}
		bcm43xx_phy_init_pctl(dev);
	} else if (phy->type == BCM43xx_PHYTYPE_G)
		bcm43xx_write16(dev, 0x03E6, 0x0);
}

static void bcm43xx_calc_loopback_gain(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 backup_phy[16] = {0};
	u16 backup_radio[3];
	u16 backup_bband;
	u16 i, j, loop_i_max;
	u16 trsw_rx;
	u16 loop1_outer_done, loop1_inner_done;

	backup_phy[0] = bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0);
	backup_phy[1] = bcm43xx_phy_read(dev, BCM43xx_PHY_CCKBBANDCFG);
	backup_phy[2] = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER);
	backup_phy[3] = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL);
	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		backup_phy[4] = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER);
		backup_phy[5] = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL);
	}
	backup_phy[6] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x5A));
	backup_phy[7] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x59));
	backup_phy[8] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x58));
	backup_phy[9] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x0A));
	backup_phy[10] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x03));
	backup_phy[11] = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_MASK);
	backup_phy[12] = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_CTL);
	backup_phy[13] = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x2B));
	backup_phy[14] = bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL);
	backup_phy[15] = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE);
	backup_bband = phy->bbatt.att;
	backup_radio[0] = bcm43xx_radio_read16(dev, 0x52);
	backup_radio[1] = bcm43xx_radio_read16(dev, 0x43);
	backup_radio[2] = bcm43xx_radio_read16(dev, 0x7A);

	bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0) & 0x3FFF);
	bcm43xx_phy_write(dev, BCM43xx_PHY_CCKBBANDCFG,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_CCKBBANDCFG) | 0x8000);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER) | 0x0002);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL) & 0xFFFD);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER) | 0x0001);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL) & 0xFFFE);
	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER) | 0x0001);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL) & 0xFFFE);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER) | 0x0002);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL) & 0xFFFD);
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER) | 0x000C);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL) | 0x000C);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER) | 0x0030);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
			  (bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL)
			   & 0xFFCF) | 0x10);

	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x5A), 0x0780);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x59), 0xC810);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0x000D);

	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x0A),
			  bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x0A)) | 0x2000);
	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER) | 0x0004);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL) & 0xFFFB);
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x03),
			  (bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x03))
			   & 0xFF9F) | 0x40);

	if (phy->radio_rev == 8) {
		bcm43xx_radio_write16(dev, 0x43, 0x000F);
	} else {
		bcm43xx_radio_write16(dev, 0x52, 0);
		bcm43xx_radio_write16(dev, 0x43,
				      (bcm43xx_radio_read16(dev, 0x43)
				       & 0xFFF0) | 0x9);
	}
	bcm43xx_phy_set_baseband_attenuation(dev, 11);

	if (phy->rev >= 3)
		bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0xC020);
	else
		bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0x8020);
	bcm43xx_phy_write(dev, BCM43xx_PHY_LO_CTL, 0);

	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B),
			  (bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x2B))
			   & 0xFFC0) | 0x01);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B),
			  (bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x2B))
			   & 0xC0FF) | 0x800);

	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER) | 0x0100);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
			  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL) & 0xCFFF);

	if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_EXTLNA) {
		if (phy->rev >= 7) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
					  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER)
					  | 0x0800);
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL)
					  | 0x8000);
		}
	}
	bcm43xx_radio_write16(dev, 0x7A,
			      bcm43xx_radio_read16(dev, 0x7A)
			      & 0x00F7);

	j = 0;
	loop_i_max = (phy->radio_rev == 8) ? 15 : 9;
	for (i = 0; i < loop_i_max; i++) {
		for (j = 0; j < 16; j++) {
			bcm43xx_radio_write16(dev, 0x43, i);
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL)
					   & 0xF0FF) | (j << 8));
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL,
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL)
					   & 0x0FFF) | 0xA000);
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL,
					  bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL)
					  | 0xF000);
			udelay(20);
			if (bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE) >= 0xDFC)
				goto exit_loop1;
		}
	}
exit_loop1:
	loop1_outer_done = i;
	loop1_inner_done = j;
	if (j >= 8) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL)
				  | 0x30);
		trsw_rx = 0x1B;
		for (j = j - 8; j < 16; j++) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL)
					   & 0xF0FF) | (j << 8));
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL,
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL)
					   & 0x0FFF) | 0xA000);
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL,
					  bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL)
					  | 0xF000);
			udelay(20);
			trsw_rx -= 3;
			if (bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE) >= 0xDFC)
				goto exit_loop2;
		}
	} else
		trsw_rx = 0x18;
exit_loop2:

	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER, backup_phy[4]);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL, backup_phy[5]);
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x5A), backup_phy[6]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x59), backup_phy[7]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), backup_phy[8]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x0A), backup_phy[9]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x03), backup_phy[10]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, backup_phy[11]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_LO_CTL, backup_phy[12]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B), backup_phy[13]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, backup_phy[14]);

	bcm43xx_phy_set_baseband_attenuation(dev, backup_bband);

	bcm43xx_radio_write16(dev, 0x52, backup_radio[0]);
	bcm43xx_radio_write16(dev, 0x43, backup_radio[1]);
	bcm43xx_radio_write16(dev, 0x7A, backup_radio[2]);

	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, backup_phy[2] | 0x0003);
	udelay(10);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, backup_phy[2]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, backup_phy[3]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0, backup_phy[0]);
	bcm43xx_phy_write(dev, BCM43xx_PHY_CCKBBANDCFG, backup_phy[1]);

	phy->max_lb_gain = ((loop1_inner_done * 6) - (loop1_outer_done * 4)) - 11;
	phy->trsw_rx_gain = trsw_rx * 2;
}

static void bcm43xx_phy_initg(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 tmp;

	if (phy->rev == 1)
		bcm43xx_phy_initb5(dev);
	else
		bcm43xx_phy_initb6(dev);

	if (phy->rev >= 2 || phy->gmode)
		bcm43xx_phy_inita(dev);

	if (phy->rev >= 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER, 0);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL, 0);
	}
	if (phy->rev == 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, 0);
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xC0);
	}
	if (phy->rev > 5) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, 0x400);
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xC0);
	}
	if (phy->gmode || phy->rev >= 2) {
		tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_VERSION_OFDM);
		tmp &= BCM43xx_PHYVER_VERSION;
		if (tmp == 3 || tmp == 5) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0xC2), 0x1816);
			bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0xC3), 0x8006);
		}
		if (tmp == 5) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0xCC),
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_OFDM(0xCC))
					   & 0x00FF) | 0x1F00);
		}
	}
	if ((phy->rev <= 2 && phy->gmode) || phy->rev >= 2)
		bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0x7E), 0x78);
	if (phy->radio_rev == 8) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_EXTG(0x01),
				  bcm43xx_phy_read(dev, BCM43xx_PHY_EXTG(0x01))
				  | 0x80);
		bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0x3E),
				  bcm43xx_phy_read(dev, BCM43xx_PHY_OFDM(0x3E))
				  | 0x4);
	}
	if (has_loopback_gain(phy))
		bcm43xx_calc_loopback_gain(dev);

	if (phy->radio_rev != 8) {
		if (phy->initval == 0xFFFF)
			phy->initval = bcm43xx_radio_init2050(dev);
		else
			bcm43xx_radio_write16(dev, 0x0078, phy->initval);
	}
	if (phy->lo_control->tx_bias == 0xFF) {
		bcm43xx_lo_g_measure(dev);
	} else {
		if (has_tx_magnification(phy)) {
			bcm43xx_radio_write16(dev, 0x52,
					      (bcm43xx_radio_read16(dev, 0x52) & 0xFF00) |
					      phy->lo_control->tx_bias |
					      phy->lo_control->tx_magn);
		} else {
			bcm43xx_radio_write16(dev, 0x52,
					      (bcm43xx_radio_read16(dev, 0x52) & 0xFFF0) |
					      phy->lo_control->tx_bias);
		}
		if (phy->rev >= 6) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x36),
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x36))
					   & 0x0FFF) | (phy->lo_control->tx_bias << 12));
		}
		if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_PACTRL)
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2E), 0x8075);
		else
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2E), 0x807F);
		if (phy->rev < 2)
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2F), 0x101);
		else
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2F), 0x202);
	}
	if (phy->gmode || phy->rev >= 2) {
		bcm43xx_lo_g_adjust(dev);
		bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0x8078);
	}

	if (!(dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI)) {
		/* The specs state to update the NRSSI LT with
		 * the value 0x7FFFFFFF here. I think that is some weird
		 * compiler optimization in the original driver.
		 * Essentially, what we do here is resetting all NRSSI LT
		 * entries to -32 (see the limit_value() in nrssi_hw_update())
		 */
		bcm43xx_nrssi_hw_update(dev, 0xFFFF);//FIXME?
		bcm43xx_calc_nrssi_threshold(dev);
	} else if (phy->gmode || phy->rev >= 2) {
		if (phy->nrssi[0] == -1000) {
			assert(phy->nrssi[1] == -1000);
			bcm43xx_calc_nrssi_slope(dev);
		} else
			bcm43xx_calc_nrssi_threshold(dev);
	}
	if (phy->radio_rev == 8)
		bcm43xx_phy_write(dev, BCM43xx_PHY_EXTG(0x05), 0x3230);
	bcm43xx_phy_init_pctl(dev);
	/* FIXME: The spec says in the following if, the 0 should be replaced
                  'if OFDM may not be used in the current locale'
		  but OFDM is legal everywhere */
	if ((dev->dev->bus->chip_id == 0x4306 && dev->dev->bus->chip_package == 2) || 0) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0)
				  & 0xBFFF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM(0xC3),
				  bcm43xx_phy_read(dev, BCM43xx_PHY_OFDM(0xC3))
				  & 0x7FFF);
	}
}

/* Set the baseband attenuation value on chip. */
void bcm43xx_phy_set_baseband_attenuation(struct bcm43xx_wldev *dev,
					  u16 baseband_attenuation)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->analog == 0) {
		bcm43xx_write16(dev, BCM43xx_MMIO_PHY0,
				(bcm43xx_read16(dev, BCM43xx_MMIO_PHY0)
				 & 0xFFF0) | baseband_attenuation);
	} else if (phy->analog == 1) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_DACCTL,
				  (bcm43xx_phy_read(dev, BCM43xx_PHY_DACCTL)
				   & 0xFFC3) | (baseband_attenuation << 2));
	} else {
		bcm43xx_phy_write(dev, BCM43xx_PHY_DACCTL,
				  (bcm43xx_phy_read(dev, BCM43xx_PHY_DACCTL)
				   & 0xFF87) | (baseband_attenuation << 3));
	}
}

/* http://bcm-specs.sipsolutions.net/EstimatePowerOut
 * This function converts a TSSI value to dBm in Q5.2
 */
static s8 bcm43xx_phy_estimate_power_out(struct bcm43xx_wldev *dev, s8 tssi)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s8 dbm = 0;
	s32 tmp;

	tmp = (phy->tgt_idle_tssi - phy->cur_idle_tssi + tssi);

	switch (phy->type) {
		case BCM43xx_PHYTYPE_A:
			tmp += 0x80;
			tmp = limit_value(tmp, 0x00, 0xFF);
			dbm = phy->tssi2dbm[tmp];
			TODO(); //TODO: There's a FIXME on the specs
			break;
		case BCM43xx_PHYTYPE_B:
		case BCM43xx_PHYTYPE_G:
			tmp = limit_value(tmp, 0x00, 0x3F);
			dbm = phy->tssi2dbm[tmp];
			break;
		default:
			assert(0);
	}

	return dbm;
}

void bcm43xx_put_attenuation_into_ranges(struct bcm43xx_wldev *dev,
					 int *_bbatt, int *_rfatt)
{
	int rfatt = *_rfatt;
	int bbatt = *_bbatt;
	struct bcm43xx_txpower_lo_control *lo = dev->phy.lo_control;

	/* Get baseband and radio attenuation values into their permitted ranges.
	 * Radio attenuation affects power level 4 times as much as baseband. */

	/* Range constants */
	const int rf_min = lo->rfatt_list.min_val;
	const int rf_max = lo->rfatt_list.max_val;
	const int bb_min = lo->bbatt_list.min_val;
	const int bb_max = lo->bbatt_list.max_val;

	while (1) {
		if (rfatt > rf_max &&
		    bbatt > bb_max - 4)
			break; /* Can not get it into ranges */
		if (rfatt < rf_min &&
		    bbatt < bb_min + 4)
			break; /* Can not get it into ranges */
		if (bbatt > bb_max &&
		    rfatt > rf_max - 1)
			break; /* Can not get it into ranges */
		if (bbatt < bb_min &&
		    rfatt < rf_min + 1)
			break; /* Can not get it into ranges */

		if (bbatt > bb_max) {
			bbatt -= 4;
			rfatt += 1;
			continue;
		}
		if (bbatt < bb_min) {
			bbatt += 4;
			rfatt -= 1;
			continue;
		}
		if (rfatt > rf_max) {
			rfatt -= 1;
			bbatt += 4;
			continue;
		}
		if (rfatt < rf_min) {
			rfatt += 1;
			bbatt -= 4;
			continue;
		}
		break;
	}

	*_rfatt = limit_value(rfatt, rf_min, rf_max);
	*_bbatt = limit_value(bbatt, bb_min, bb_max);
}

/* http://bcm-specs.sipsolutions.net/RecalculateTransmissionPower */
void bcm43xx_phy_xmitpower(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->cur_idle_tssi == 0)
		return;
	if ((bus->boardinfo.vendor == SSB_BOARDVENDOR_BCM) &&
	    (bus->boardinfo.type == SSB_BOARD_BU4306))
		return;
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
	if (phy->manual_txpower_control)
		return;
#endif

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A: {

		TODO(); //TODO: Nothing for A PHYs yet :-/

		break;
	}
	case BCM43xx_PHYTYPE_B:
	case BCM43xx_PHYTYPE_G: {
		u16 tmp;
		s8 v0, v1, v2, v3;
		s8 average;
		int max_pwr;
		int desired_pwr, estimated_pwr, pwr_adjust;
		int rfatt_delta, bbatt_delta;
		int rfatt, bbatt;
		u8 tx_control;
		unsigned long phylock_flags;

		tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x0058);
		v0 = (s8)(tmp & 0x00FF);
		v1 = (s8)((tmp & 0xFF00) >> 8);
		tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x005A);
		v2 = (s8)(tmp & 0x00FF);
		v3 = (s8)((tmp & 0xFF00) >> 8);
		tmp = 0;

		if (v0 == 0x7F || v1 == 0x7F || v2 == 0x7F || v3 == 0x7F) {
			tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x0070);
			v0 = (s8)(tmp & 0x00FF);
			v1 = (s8)((tmp & 0xFF00) >> 8);
			tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x0072);
			v2 = (s8)(tmp & 0x00FF);
			v3 = (s8)((tmp & 0xFF00) >> 8);
			if (v0 == 0x7F || v1 == 0x7F || v2 == 0x7F || v3 == 0x7F)
				return;
			v0 = (v0 + 0x20) & 0x3F;
			v1 = (v1 + 0x20) & 0x3F;
			v2 = (v2 + 0x20) & 0x3F;
			v3 = (v3 + 0x20) & 0x3F;
			tmp = 1;
		}
		bcm43xx_shm_clear_tssi(dev);

		average = (v0 + v1 + v2 + v3 + 2) / 4;

		if (tmp && (bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x005E) & 0x8))
			average -= 13;

		estimated_pwr = bcm43xx_phy_estimate_power_out(dev, average);

		max_pwr = dev->dev->bus->sprom.r1.maxpwr_bg;
		if ((dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_PACTRL) &&
		    (phy->type == BCM43xx_PHYTYPE_G))
			max_pwr -= 0x3;
		if (unlikely(max_pwr <= 0)) {
			printk(KERN_ERR PFX "Invalid max-TX-power value in SPROM.\n");
			max_pwr = 60; /* fake it */
			dev->dev->bus->sprom.r1.maxpwr_bg = max_pwr;
		}

		/*TODO:
		max_pwr = min(REG - dev->dev->bus->sprom.antennagain_bgphy - 0x6, max_pwr)
			where REG is the max power as per the regulatory domain
		*/

		desired_pwr = phy->power_level;
		/* Convert the desired_pwr to Q5.2 and limit it. */
		desired_pwr = limit_value((desired_pwr << 2), 0, max_pwr);
		if (bcm43xx_debug(dev, BCM43xx_DBG_XMITPOWER)) {
			dprintk(KERN_DEBUG PFX
				"Current TX power output: " Q52_FMT " dBm, "
				"Desired TX power output: " Q52_FMT " dBm\n",
				Q52_ARG(estimated_pwr), Q52_ARG(desired_pwr));
		}

		pwr_adjust = desired_pwr - estimated_pwr;
		rfatt_delta = -((pwr_adjust + 7) >> 3);
		bbatt_delta = (-(pwr_adjust >> 1)) - (4 * rfatt_delta);
		if ((rfatt_delta == 0) && (bbatt_delta == 0)) {
			bcm43xx_lo_g_ctl_mark_cur_used(dev);
			return;
		}

		/* Calculate the new attenuation values. */
		bbatt = phy->bbatt.att;
		bbatt += bbatt_delta;
		rfatt = phy->rfatt.att;
		rfatt += rfatt_delta;

		bcm43xx_put_attenuation_into_ranges(dev, &bbatt, &rfatt);
		tx_control = phy->tx_control;
		if ((phy->radio_ver == 0x2050) && (phy->radio_rev == 2)) {
			if (rfatt <= 1) {
				if (tx_control == 0) {
					tx_control = BCM43xx_TXCTL_PA2DB | BCM43xx_TXCTL_TXMIX;
					rfatt += 2;
					bbatt += 2;
				} else if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_PACTRL) {
					bbatt += 4 * (rfatt - 2);
					rfatt = 2;
				}
			} else if (rfatt > 4 && tx_control) {
				tx_control = 0;
				if (bbatt < 3) {
					rfatt -= 3;
					bbatt += 2;
				} else {
					rfatt -= 2;
					bbatt -= 2;
				}
			}
		}
		phy->tx_control = tx_control;
		bcm43xx_put_attenuation_into_ranges(dev, &bbatt, &rfatt);

		bcm43xx_phy_lock(dev, phylock_flags);
		bcm43xx_radio_lock(dev);
		bcm43xx_set_txpower_g(dev, &phy->bbatt, &phy->rfatt, phy->tx_control);
		bcm43xx_lo_g_ctl_mark_cur_used(dev);
		bcm43xx_radio_unlock(dev);
		bcm43xx_phy_unlock(dev, phylock_flags);
		break;
	}
	default:
		assert(0);
	}
}

static inline
s32 bcm43xx_tssi2dbm_ad(s32 num, s32 den)
{
	if (num < 0)
		return num/den;
	else
		return (num+den/2)/den;
}

static inline
s8 bcm43xx_tssi2dbm_entry(s8 entry [], u8 index, s16 pab0, s16 pab1, s16 pab2)
{
	s32 m1, m2, f = 256, q, delta;
	s8 i = 0;

	m1 = bcm43xx_tssi2dbm_ad(16 * pab0 + index * pab1, 32);
	m2 = max(bcm43xx_tssi2dbm_ad(32768 + index * pab2, 256), 1);
	do {
		if (i > 15)
			return -EINVAL;
		q = bcm43xx_tssi2dbm_ad(f * 4096 -
					bcm43xx_tssi2dbm_ad(m2 * f, 16) * f, 2048);
		delta = abs(q - f);
		f = q;
		i++;
	} while (delta >= 2);
	entry[index] = limit_value(bcm43xx_tssi2dbm_ad(m1 * f, 8192), -127, 128);
	return 0;
}

/* http://bcm-specs.sipsolutions.net/TSSI_to_DBM_Table */
int bcm43xx_phy_init_tssi2dbm_table(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s16 pab0, pab1, pab2;
	u8 idx;
	s8 *dyn_tssi2dbm;

	if (phy->type == BCM43xx_PHYTYPE_A) {
		pab0 = (s16)(dev->dev->bus->sprom.r1.pa1b0);
		pab1 = (s16)(dev->dev->bus->sprom.r1.pa1b1);
		pab2 = (s16)(dev->dev->bus->sprom.r1.pa1b2);
	} else {
		pab0 = (s16)(dev->dev->bus->sprom.r1.pa0b0);
		pab1 = (s16)(dev->dev->bus->sprom.r1.pa0b1);
		pab2 = (s16)(dev->dev->bus->sprom.r1.pa0b2);
	}

	if ((dev->dev->bus->chip_id == 0x4301) && (phy->radio_ver != 0x2050)) {
		phy->tgt_idle_tssi = 0x34;
		phy->tssi2dbm = bcm43xx_tssi2dbm_b_table;
		return 0;
	}

	if (pab0 != 0 && pab1 != 0 && pab2 != 0 &&
	    pab0 != -1 && pab1 != -1 && pab2 != -1) {
		/* The pabX values are set in SPROM. Use them. */
		if (phy->type == BCM43xx_PHYTYPE_A) {
			if ((s8)dev->dev->bus->sprom.r1.itssi_a != 0 &&
			    (s8)dev->dev->bus->sprom.r1.itssi_a != -1)
				phy->tgt_idle_tssi = (s8)(dev->dev->bus->sprom.r1.itssi_a);
			else
				phy->tgt_idle_tssi = 62;
		} else {
			if ((s8)dev->dev->bus->sprom.r1.itssi_bg != 0 &&
			    (s8)dev->dev->bus->sprom.r1.itssi_bg != -1)
				phy->tgt_idle_tssi = (s8)(dev->dev->bus->sprom.r1.itssi_bg);
			else
				phy->tgt_idle_tssi = 62;
		}
		dyn_tssi2dbm = kmalloc(64, GFP_KERNEL);
		if (dyn_tssi2dbm == NULL) {
			printk(KERN_ERR PFX "Could not allocate memory"
					    "for tssi2dbm table\n");
			return -ENOMEM;
		}
		for (idx = 0; idx < 64; idx++)
			if (bcm43xx_tssi2dbm_entry(dyn_tssi2dbm, idx, pab0, pab1, pab2)) {
				phy->tssi2dbm = NULL;
				printk(KERN_ERR PFX "Could not generate "
						    "tssi2dBm table\n");
				kfree(dyn_tssi2dbm);
				return -ENODEV;
			}
		phy->tssi2dbm = dyn_tssi2dbm;
		phy->dyn_tssi_tbl = 1;
	} else {
		/* pabX values not set in SPROM. */
		switch (phy->type) {
		case BCM43xx_PHYTYPE_A:
			/* APHY needs a generated table. */
			phy->tssi2dbm = NULL;
			printk(KERN_ERR PFX "Could not generate tssi2dBm "
					    "table (wrong SPROM info)!\n");
			return -ENODEV;
		case BCM43xx_PHYTYPE_B:
			phy->tgt_idle_tssi = 0x34;
			phy->tssi2dbm = bcm43xx_tssi2dbm_b_table;
			break;
		case BCM43xx_PHYTYPE_G:
			phy->tgt_idle_tssi = 0x34;
			phy->tssi2dbm = bcm43xx_tssi2dbm_g_table;
			break;
		}
	}

	return 0;
}

int bcm43xx_phy_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int err = -ENODEV;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
		if (phy->rev == 2 || phy->rev == 3) {
			bcm43xx_phy_inita(dev);
			err = 0;
		}
		break;
	case BCM43xx_PHYTYPE_B:
		switch (phy->rev) {
		case 2:
			bcm43xx_phy_initb2(dev);
			err = 0;
			break;
		case 4:
			bcm43xx_phy_initb4(dev);
			err = 0;
			break;
		case 5:
			bcm43xx_phy_initb5(dev);
			err = 0;
			break;
		case 6:
			bcm43xx_phy_initb6(dev);
			err = 0;
			break;
		}
		break;
	case BCM43xx_PHYTYPE_G:
		bcm43xx_phy_initg(dev);
		err = 0;
		break;
	}
	if (err)
		printk(KERN_WARNING PFX "Unknown PHYTYPE found!\n");

	return err;
}

void bcm43xx_set_rx_antenna(struct bcm43xx_wldev *dev, int antenna)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u32 hf;
	u16 tmp;
	int autodiv = 0;

	if (antenna == BCM43xx_ANTENNA_AUTO0 ||
	    antenna == BCM43xx_ANTENNA_AUTO1)
		autodiv = 1;

	hf = bcm43xx_hf_read(dev);
	hf &= ~BCM43xx_HF_ANTDIVHELP;
	bcm43xx_hf_write(dev, hf);

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
	case BCM43xx_PHYTYPE_G:
		tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_BBANDCFG);
		tmp &= ~BCM43xx_PHY_BBANDCFG_RXANT;
		tmp |= (autodiv ? BCM43xx_ANTENNA_AUTO0 : antenna)
			<< BCM43xx_PHY_BBANDCFG_RXANT_SHIFT;
		bcm43xx_phy_write(dev, BCM43xx_PHY_BBANDCFG, tmp);

		if (autodiv) {
			tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_ANTDWELL);
			if (antenna == BCM43xx_ANTENNA_AUTO0)
				tmp &= ~BCM43xx_PHY_ANTDWELL_AUTODIV1;
			else
				tmp |= BCM43xx_PHY_ANTDWELL_AUTODIV1;
			bcm43xx_phy_write(dev, BCM43xx_PHY_ANTDWELL, tmp);
		}
		if (phy->type == BCM43xx_PHYTYPE_G) {
			tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_ANTWRSETT);
			if (autodiv)
				tmp |= BCM43xx_PHY_ANTWRSETT_ARXDIV;
			else
				tmp &= ~BCM43xx_PHY_ANTWRSETT_ARXDIV;
			bcm43xx_phy_write(dev, BCM43xx_PHY_ANTWRSETT, tmp);
			if (phy->rev >= 2) {
				tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_OFDM61);
				tmp |= BCM43xx_PHY_OFDM61_10;
				bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM61, tmp);

				tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_DIVSRCHGAINBACK);
				tmp = (tmp & 0xFF00) | 0x15;
				bcm43xx_phy_write(dev, BCM43xx_PHY_DIVSRCHGAINBACK, tmp);

				if (phy->rev == 2) {
					bcm43xx_phy_write(dev, BCM43xx_PHY_ADIVRELATED, 8);
				} else {
					tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_ADIVRELATED);
					tmp = (tmp & 0xFF00) | 8;
					bcm43xx_phy_write(dev, BCM43xx_PHY_ADIVRELATED, tmp);
				}
			}
			if (phy->rev >= 6)
				bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM9B, 0xDC);
		} else {
			if (phy->rev < 3) {
				tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_ANTDWELL);
				tmp = (tmp & 0xFF00) | 0x24;
				bcm43xx_phy_write(dev, BCM43xx_PHY_ANTDWELL, tmp);
			} else {
				tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_OFDM61);
				tmp |= 0x10;
				bcm43xx_phy_write(dev, BCM43xx_PHY_OFDM61, tmp);
				if (phy->analog == 3) {
					bcm43xx_phy_write(dev, BCM43xx_PHY_CLIPPWRDOWNT, 0x1D);
					bcm43xx_phy_write(dev, BCM43xx_PHY_ADIVRELATED, 8);
				} else {
					bcm43xx_phy_write(dev, BCM43xx_PHY_CLIPPWRDOWNT, 0x3A);
					tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_ADIVRELATED);
					tmp = (tmp & 0xFF00) | 8;
					bcm43xx_phy_write(dev, BCM43xx_PHY_ADIVRELATED, tmp);
				}
			}
		}
		break;
	case BCM43xx_PHYTYPE_B:
		tmp = bcm43xx_phy_read(dev, BCM43xx_PHY_CCKBBANDCFG);
		tmp &= ~BCM43xx_PHY_BBANDCFG_RXANT;
		tmp |= (autodiv ? BCM43xx_ANTENNA_AUTO0 : antenna)
			<< BCM43xx_PHY_BBANDCFG_RXANT_SHIFT;
		bcm43xx_phy_write(dev, BCM43xx_PHY_CCKBBANDCFG, tmp);
		break;
	default:
		assert(0);
	}

	hf |= BCM43xx_HF_ANTDIVHELP;
	bcm43xx_hf_write(dev, hf);
}

/* Get the freq, as it has to be written to the device. */
static inline
u16 channel2freq_bg(u8 channel)
{
	assert(channel >= 1 && channel <= 14);

	return bcm43xx_radio_channel_codes_bg[channel - 1];
}

/* Get the freq, as it has to be written to the device. */
static inline
u16 channel2freq_a(u8 channel)
{
	assert(channel <= 200);

	return (5000 + 5 * channel);
}

void bcm43xx_radio_lock(struct bcm43xx_wldev *dev)
{
	u32 status;

	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	status |= BCM43xx_SBF_RADIOREG_LOCK;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, status);
	mmiowb();
	udelay(10);
}

void bcm43xx_radio_unlock(struct bcm43xx_wldev *dev)
{
	u32 status;

	bcm43xx_read16(dev, BCM43xx_MMIO_PHY_VER); /* dummy read */
	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	status &= ~BCM43xx_SBF_RADIOREG_LOCK;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, status);
	mmiowb();
}

u16 bcm43xx_radio_read16(struct bcm43xx_wldev *dev, u16 offset)
{
	struct bcm43xx_phy *phy = &dev->phy;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
		offset |= 0x0040;
		break;
	case BCM43xx_PHYTYPE_B:
		if (phy->radio_ver == 0x2053) {
			if (offset < 0x70)
				offset += 0x80;
			else if (offset < 0x80)
				offset += 0x70;
		} else if (phy->radio_ver == 0x2050) {
			offset |= 0x80;
		} else
			assert(0);
		break;
	case BCM43xx_PHYTYPE_G:
		offset |= 0x80;
		break;
	}

	bcm43xx_write16(dev, BCM43xx_MMIO_RADIO_CONTROL, offset);
	return bcm43xx_read16(dev, BCM43xx_MMIO_RADIO_DATA_LOW);
}

void bcm43xx_radio_write16(struct bcm43xx_wldev *dev, u16 offset, u16 val)
{
	bcm43xx_write16(dev, BCM43xx_MMIO_RADIO_CONTROL, offset);
	mmiowb();
	bcm43xx_write16(dev, BCM43xx_MMIO_RADIO_DATA_LOW, val);
}

static void bcm43xx_set_all_gains(struct bcm43xx_wldev *dev,
				  s16 first, s16 second, s16 third)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 i;
	u16 start = 0x08, end = 0x18;
	u16 tmp;
	u16 table;

	if (phy->rev <= 1) {
		start = 0x10;
		end = 0x20;
	}

	table = BCM43xx_OFDMTAB_GAINX;
	if (phy->rev <= 1)
		table = BCM43xx_OFDMTAB_GAINX_R1;
	for (i = 0; i < 4; i++)
		bcm43xx_ofdmtab_write16(dev, table, i, first);

	for (i = start; i < end; i++)
		bcm43xx_ofdmtab_write16(dev, table, i, second);

	if (third != -1) {
		tmp = ((u16)third << 14) | ((u16)third << 6);
		bcm43xx_phy_write(dev, 0x04A0,
		                  (bcm43xx_phy_read(dev, 0x04A0) & 0xBFBF) | tmp);
		bcm43xx_phy_write(dev, 0x04A1,
		                  (bcm43xx_phy_read(dev, 0x04A1) & 0xBFBF) | tmp);
		bcm43xx_phy_write(dev, 0x04A2,
		                  (bcm43xx_phy_read(dev, 0x04A2) & 0xBFBF) | tmp);
	}
	bcm43xx_dummy_transmission(dev);
}

static void bcm43xx_set_original_gains(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 i, tmp;
	u16 table;
	u16 start = 0x0008, end = 0x0018;

	if (phy->rev <= 1) {
		start = 0x0010;
		end = 0x0020;
	}

	table = BCM43xx_OFDMTAB_GAINX;
	if (phy->rev <= 1)
		table = BCM43xx_OFDMTAB_GAINX_R1;
	for (i = 0; i < 4; i++) {
		tmp = (i & 0xFFFC);
		tmp |= (i & 0x0001) << 1;
		tmp |= (i & 0x0002) >> 1;

		bcm43xx_ofdmtab_write16(dev, table, i, tmp);
	}

	for (i = start; i < end; i++)
		bcm43xx_ofdmtab_write16(dev, table, i, i - start);

	bcm43xx_phy_write(dev, 0x04A0,
	                  (bcm43xx_phy_read(dev, 0x04A0) & 0xBFBF) | 0x4040);
	bcm43xx_phy_write(dev, 0x04A1,
	                  (bcm43xx_phy_read(dev, 0x04A1) & 0xBFBF) | 0x4040);
	bcm43xx_phy_write(dev, 0x04A2,
	                  (bcm43xx_phy_read(dev, 0x04A2) & 0xBFBF) | 0x4000);
	bcm43xx_dummy_transmission(dev);
}

/* Synthetic PU workaround */
static void bcm43xx_synth_pu_workaround(struct bcm43xx_wldev *dev, u8 channel)
{
	struct bcm43xx_phy *phy = &dev->phy;

	might_sleep();

	if (phy->radio_ver != 0x2050 || phy->radio_rev >= 6) {
		/* We do not need the workaround. */
		return;
	}

	if (channel <= 10) {
		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL,
				channel2freq_bg(channel + 4));
	} else {
		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL,
				channel2freq_bg(1));
	}
	msleep(1);
	bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL,
			channel2freq_bg(channel));
}

u8 bcm43xx_radio_aci_detect(struct bcm43xx_wldev *dev, u8 channel)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u8 ret = 0;
	u16 saved, rssi, temp;
	int i, j = 0;

	saved = bcm43xx_phy_read(dev, 0x0403);
	bcm43xx_radio_selectchannel(dev, channel, 0);
	bcm43xx_phy_write(dev, 0x0403, (saved & 0xFFF8) | 5);
	if (phy->aci_hw_rssi)
		rssi = bcm43xx_phy_read(dev, 0x048A) & 0x3F;
	else
		rssi = saved & 0x3F;
	/* clamp temp to signed 5bit */
	if (rssi > 32)
		rssi -= 64;
	for (i = 0;i < 100; i++) {
		temp = (bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x3F;
		if (temp > 32)
			temp -= 64;
		if (temp < rssi)
			j++;
		if (j >= 20)
			ret = 1;
	}
	bcm43xx_phy_write(dev, 0x0403, saved);

	return ret;
}

u8 bcm43xx_radio_aci_scan(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u8 ret[13];
	unsigned int channel = phy->channel;
	unsigned int i, j, start, end;
	unsigned long phylock_flags;

	if (!((phy->type == BCM43xx_PHYTYPE_G) && (phy->rev > 0)))
		return 0;

	bcm43xx_phy_lock(dev, phylock_flags);
	bcm43xx_radio_lock(dev);
	bcm43xx_phy_write(dev, 0x0802,
	                  bcm43xx_phy_read(dev, 0x0802) & 0xFFFC);
	bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
	                  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) & 0x7FFF);
	bcm43xx_set_all_gains(dev, 3, 8, 1);

	start = (channel - 5 > 0) ? channel - 5 : 1;
	end = (channel + 5 < 14) ? channel + 5 : 13;

	for (i = start; i <= end; i++) {
		if (abs(channel - i) > 2)
			ret[i-1] = bcm43xx_radio_aci_detect(dev, i);
	}
	bcm43xx_radio_selectchannel(dev, channel, 0);
	bcm43xx_phy_write(dev, 0x0802,
	                  (bcm43xx_phy_read(dev, 0x0802) & 0xFFFC) | 0x0003);
	bcm43xx_phy_write(dev, 0x0403,
	                  bcm43xx_phy_read(dev, 0x0403) & 0xFFF8);
	bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
	                  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) | 0x8000);
	bcm43xx_set_original_gains(dev);
	for (i = 0; i < 13; i++) {
		if (!ret[i])
			continue;
		end = (i + 5 < 13) ? i + 5 : 13;
		for (j = i; j < end; j++)
			ret[j] = 1;
	}
	bcm43xx_radio_unlock(dev);
	bcm43xx_phy_unlock(dev, phylock_flags);

	return ret[channel - 1];
}

/* http://bcm-specs.sipsolutions.net/NRSSILookupTable */
void bcm43xx_nrssi_hw_write(struct bcm43xx_wldev *dev, u16 offset, s16 val)
{
	bcm43xx_phy_write(dev, BCM43xx_PHY_NRSSILT_CTRL, offset);
	mmiowb();
	bcm43xx_phy_write(dev, BCM43xx_PHY_NRSSILT_DATA, (u16)val);
}

/* http://bcm-specs.sipsolutions.net/NRSSILookupTable */
s16 bcm43xx_nrssi_hw_read(struct bcm43xx_wldev *dev, u16 offset)
{
	u16 val;

	bcm43xx_phy_write(dev, BCM43xx_PHY_NRSSILT_CTRL, offset);
	val = bcm43xx_phy_read(dev, BCM43xx_PHY_NRSSILT_DATA);

	return (s16)val;
}

/* http://bcm-specs.sipsolutions.net/NRSSILookupTable */
void bcm43xx_nrssi_hw_update(struct bcm43xx_wldev *dev, u16 val)
{
	u16 i;
	s16 tmp;

	for (i = 0; i < 64; i++) {
		tmp = bcm43xx_nrssi_hw_read(dev, i);
		tmp -= val;
		tmp = limit_value(tmp, -32, 31);
		bcm43xx_nrssi_hw_write(dev, i, tmp);
	}
}

/* http://bcm-specs.sipsolutions.net/NRSSILookupTable */
void bcm43xx_nrssi_mem_update(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s16 i, delta;
	s32 tmp;

	delta = 0x1F - phy->nrssi[0];
	for (i = 0; i < 64; i++) {
		tmp = (i - delta) * phy->nrssislope;
		tmp /= 0x10000;
		tmp += 0x3A;
		tmp = limit_value(tmp, 0, 0x3F);
		phy->nrssi_lt[i] = tmp;
	}
}

static void bcm43xx_calc_nrssi_offset(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 backup[20] = { 0 };
	s16 v47F;
	u16 i;
	u16 saved = 0xFFFF;

	backup[0] = bcm43xx_phy_read(dev, 0x0001);
	backup[1] = bcm43xx_phy_read(dev, 0x0811);
	backup[2] = bcm43xx_phy_read(dev, 0x0812);
	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		backup[3] = bcm43xx_phy_read(dev, 0x0814);
		backup[4] = bcm43xx_phy_read(dev, 0x0815);
	}
	backup[5] = bcm43xx_phy_read(dev, 0x005A);
	backup[6] = bcm43xx_phy_read(dev, 0x0059);
	backup[7] = bcm43xx_phy_read(dev, 0x0058);
	backup[8] = bcm43xx_phy_read(dev, 0x000A);
	backup[9] = bcm43xx_phy_read(dev, 0x0003);
	backup[10] = bcm43xx_radio_read16(dev, 0x007A);
	backup[11] = bcm43xx_radio_read16(dev, 0x0043);

	bcm43xx_phy_write(dev, 0x0429,
			  bcm43xx_phy_read(dev, 0x0429) & 0x7FFF);
	bcm43xx_phy_write(dev, 0x0001,
			  (bcm43xx_phy_read(dev, 0x0001) & 0x3FFF) | 0x4000);
	bcm43xx_phy_write(dev, 0x0811,
			  bcm43xx_phy_read(dev, 0x0811) | 0x000C);
	bcm43xx_phy_write(dev, 0x0812,
			  (bcm43xx_phy_read(dev, 0x0812) & 0xFFF3) | 0x0004);
	bcm43xx_phy_write(dev, 0x0802,
			  bcm43xx_phy_read(dev, 0x0802) & ~(0x1 | 0x2));
	if (phy->rev >= 6) {
		backup[12] = bcm43xx_phy_read(dev, 0x002E);
		backup[13] = bcm43xx_phy_read(dev, 0x002F);
		backup[14] = bcm43xx_phy_read(dev, 0x080F);
		backup[15] = bcm43xx_phy_read(dev, 0x0810);
		backup[16] = bcm43xx_phy_read(dev, 0x0801);
		backup[17] = bcm43xx_phy_read(dev, 0x0060);
		backup[18] = bcm43xx_phy_read(dev, 0x0014);
		backup[19] = bcm43xx_phy_read(dev, 0x0478);

		bcm43xx_phy_write(dev, 0x002E, 0);
		bcm43xx_phy_write(dev, 0x002F, 0);
		bcm43xx_phy_write(dev, 0x080F, 0);
		bcm43xx_phy_write(dev, 0x0810, 0);
		bcm43xx_phy_write(dev, 0x0478,
				  bcm43xx_phy_read(dev, 0x0478) | 0x0100);
		bcm43xx_phy_write(dev, 0x0801,
				  bcm43xx_phy_read(dev, 0x0801) | 0x0040);
		bcm43xx_phy_write(dev, 0x0060,
				  bcm43xx_phy_read(dev, 0x0060) | 0x0040);
		bcm43xx_phy_write(dev, 0x0014,
				  bcm43xx_phy_read(dev, 0x0014) | 0x0200);
	}
	bcm43xx_radio_write16(dev, 0x007A,
			      bcm43xx_radio_read16(dev, 0x007A) | 0x0070);
	bcm43xx_radio_write16(dev, 0x007A,
			      bcm43xx_radio_read16(dev, 0x007A) | 0x0080);
	udelay(30);

	v47F = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
	if (v47F >= 0x20)
		v47F -= 0x40;
	if (v47F == 31) {
		for (i = 7; i >= 4; i--) {
			bcm43xx_radio_write16(dev, 0x007B, i);
			udelay(20);
			v47F = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
			if (v47F >= 0x20)
				v47F -= 0x40;
			if (v47F < 31 && saved == 0xFFFF)
				saved = i;
		}
		if (saved == 0xFFFF)
			saved = 4;
	} else {
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) & 0x007F);
		if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
			bcm43xx_phy_write(dev, 0x0814,
					  bcm43xx_phy_read(dev, 0x0814) | 0x0001);
			bcm43xx_phy_write(dev, 0x0815,
					  bcm43xx_phy_read(dev, 0x0815) & 0xFFFE);
		}
		bcm43xx_phy_write(dev, 0x0811,
				  bcm43xx_phy_read(dev, 0x0811) | 0x000C);
		bcm43xx_phy_write(dev, 0x0812,
				  bcm43xx_phy_read(dev, 0x0812) | 0x000C);
		bcm43xx_phy_write(dev, 0x0811,
				  bcm43xx_phy_read(dev, 0x0811) | 0x0030);
		bcm43xx_phy_write(dev, 0x0812,
				  bcm43xx_phy_read(dev, 0x0812) | 0x0030);
		bcm43xx_phy_write(dev, 0x005A, 0x0480);
		bcm43xx_phy_write(dev, 0x0059, 0x0810);
		bcm43xx_phy_write(dev, 0x0058, 0x000D);
		if (phy->rev == 0) {
			bcm43xx_phy_write(dev, 0x0003, 0x0122);
		} else {
			bcm43xx_phy_write(dev, 0x000A,
					  bcm43xx_phy_read(dev, 0x000A)
					  | 0x2000);
		}
		if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
			bcm43xx_phy_write(dev, 0x0814,
					  bcm43xx_phy_read(dev, 0x0814) | 0x0004);
			bcm43xx_phy_write(dev, 0x0815,
					  bcm43xx_phy_read(dev, 0x0815) & 0xFFFB);
		}
		bcm43xx_phy_write(dev, 0x0003,
				  (bcm43xx_phy_read(dev, 0x0003) & 0xFF9F)
				  | 0x0040);
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) | 0x000F);
		bcm43xx_set_all_gains(dev, 3, 0, 1);
		bcm43xx_radio_write16(dev, 0x0043,
				      (bcm43xx_radio_read16(dev, 0x0043)
				       & 0x00F0) | 0x000F);
		udelay(30);
		v47F = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
		if (v47F >= 0x20)
			v47F -= 0x40;
		if (v47F == -32) {
			for (i = 0; i < 4; i++) {
				bcm43xx_radio_write16(dev, 0x007B, i);
				udelay(20);
				v47F = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
				if (v47F >= 0x20)
					v47F -= 0x40;
				if (v47F > -31 && saved == 0xFFFF)
					saved = i;
			}
			if (saved == 0xFFFF)
				saved = 3;
		} else
			saved = 0;
	}
	bcm43xx_radio_write16(dev, 0x007B, saved);

	if (phy->rev >= 6) {
		bcm43xx_phy_write(dev, 0x002E, backup[12]);
		bcm43xx_phy_write(dev, 0x002F, backup[13]);
		bcm43xx_phy_write(dev, 0x080F, backup[14]);
		bcm43xx_phy_write(dev, 0x0810, backup[15]);
	}
	if (phy->rev != 1) { /* Not in specs, but needed to prevent PPC machine check */
		bcm43xx_phy_write(dev, 0x0814, backup[3]);
		bcm43xx_phy_write(dev, 0x0815, backup[4]);
	}
	bcm43xx_phy_write(dev, 0x005A, backup[5]);
	bcm43xx_phy_write(dev, 0x0059, backup[6]);
	bcm43xx_phy_write(dev, 0x0058, backup[7]);
	bcm43xx_phy_write(dev, 0x000A, backup[8]);
	bcm43xx_phy_write(dev, 0x0003, backup[9]);
	bcm43xx_radio_write16(dev, 0x0043, backup[11]);
	bcm43xx_radio_write16(dev, 0x007A, backup[10]);
	bcm43xx_phy_write(dev, 0x0802,
			  bcm43xx_phy_read(dev, 0x0802) | 0x1 | 0x2);
	bcm43xx_phy_write(dev, 0x0429,
			  bcm43xx_phy_read(dev, 0x0429) | 0x8000);
	bcm43xx_set_original_gains(dev);
	if (phy->rev >= 6) {
		bcm43xx_phy_write(dev, 0x0801, backup[16]);
		bcm43xx_phy_write(dev, 0x0060, backup[17]);
		bcm43xx_phy_write(dev, 0x0014, backup[18]);
		bcm43xx_phy_write(dev, 0x0478, backup[19]);
	}
	bcm43xx_phy_write(dev, 0x0001, backup[0]);
	bcm43xx_phy_write(dev, 0x0812, backup[2]);
	bcm43xx_phy_write(dev, 0x0811, backup[1]);
}

void bcm43xx_calc_nrssi_slope(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 backup[18] = { 0 };
	u16 tmp;
	s16 nrssi0, nrssi1;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_B:
		backup[0] = bcm43xx_radio_read16(dev, 0x007A);
		backup[1] = bcm43xx_radio_read16(dev, 0x0052);
		backup[2] = bcm43xx_radio_read16(dev, 0x0043);
		backup[3] = bcm43xx_phy_read(dev, 0x0030);
		backup[4] = bcm43xx_phy_read(dev, 0x0026);
		backup[5] = bcm43xx_phy_read(dev, 0x0015);
		backup[6] = bcm43xx_phy_read(dev, 0x002A);
		backup[7] = bcm43xx_phy_read(dev, 0x0020);
		backup[8] = bcm43xx_phy_read(dev, 0x005A);
		backup[9] = bcm43xx_phy_read(dev, 0x0059);
		backup[10] = bcm43xx_phy_read(dev, 0x0058);
		backup[11] = bcm43xx_read16(dev, 0x03E2);
		backup[12] = bcm43xx_read16(dev, 0x03E6);
		backup[13] = bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT);

		tmp  = bcm43xx_radio_read16(dev, 0x007A);
		tmp &= (phy->rev >= 5) ? 0x007F : 0x000F;
		bcm43xx_radio_write16(dev, 0x007A, tmp);
		bcm43xx_phy_write(dev, 0x0030, 0x00FF);
		bcm43xx_write16(dev, 0x03EC, 0x7F7F);
		bcm43xx_phy_write(dev, 0x0026, 0x0000);
		bcm43xx_phy_write(dev, 0x0015,
				  bcm43xx_phy_read(dev, 0x0015) | 0x0020);
		bcm43xx_phy_write(dev, 0x002A, 0x08A3);
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) | 0x0080);

		nrssi0 = (s16)bcm43xx_phy_read(dev, 0x0027);
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) & 0x007F);
		if (phy->rev >= 2) {
			bcm43xx_write16(dev, 0x03E6, 0x0040);
		} else if (phy->rev == 0) {
			bcm43xx_write16(dev, 0x03E6, 0x0122);
		} else {
			bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT,
					bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT) & 0x2000);
		}
		bcm43xx_phy_write(dev, 0x0020, 0x3F3F);
		bcm43xx_phy_write(dev, 0x0015, 0xF330);
		bcm43xx_radio_write16(dev, 0x005A, 0x0060);
		bcm43xx_radio_write16(dev, 0x0043,
				      bcm43xx_radio_read16(dev, 0x0043) & 0x00F0);
		bcm43xx_phy_write(dev, 0x005A, 0x0480);
		bcm43xx_phy_write(dev, 0x0059, 0x0810);
		bcm43xx_phy_write(dev, 0x0058, 0x000D);
		udelay(20);

		nrssi1 = (s16)bcm43xx_phy_read(dev, 0x0027);
		bcm43xx_phy_write(dev, 0x0030, backup[3]);
		bcm43xx_radio_write16(dev, 0x007A, backup[0]);
		bcm43xx_write16(dev, 0x03E2, backup[11]);
		bcm43xx_phy_write(dev, 0x0026, backup[4]);
		bcm43xx_phy_write(dev, 0x0015, backup[5]);
		bcm43xx_phy_write(dev, 0x002A, backup[6]);
		bcm43xx_synth_pu_workaround(dev, phy->channel);
		if (phy->rev != 0)
			bcm43xx_write16(dev, 0x03F4, backup[13]);

		bcm43xx_phy_write(dev, 0x0020, backup[7]);
		bcm43xx_phy_write(dev, 0x005A, backup[8]);
		bcm43xx_phy_write(dev, 0x0059, backup[9]);
		bcm43xx_phy_write(dev, 0x0058, backup[10]);
		bcm43xx_radio_write16(dev, 0x0052, backup[1]);
		bcm43xx_radio_write16(dev, 0x0043, backup[2]);

		if (nrssi0 == nrssi1)
			phy->nrssislope = 0x00010000;
		else
			phy->nrssislope = 0x00400000 / (nrssi0 - nrssi1);

		if (nrssi0 <= -4) {
			phy->nrssi[0] = nrssi0;
			phy->nrssi[1] = nrssi1;
		}
		break;
	case BCM43xx_PHYTYPE_G:
		if (phy->radio_rev >= 9)
			return;
		if (phy->radio_rev == 8)
			bcm43xx_calc_nrssi_offset(dev);

		bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) & 0x7FFF);
		bcm43xx_phy_write(dev, 0x0802,
				  bcm43xx_phy_read(dev, 0x0802) & 0xFFFC);
		backup[7] = bcm43xx_read16(dev, 0x03E2);
		bcm43xx_write16(dev, 0x03E2,
				bcm43xx_read16(dev, 0x03E2) | 0x8000);
		backup[0] = bcm43xx_radio_read16(dev, 0x007A);
		backup[1] = bcm43xx_radio_read16(dev, 0x0052);
		backup[2] = bcm43xx_radio_read16(dev, 0x0043);
		backup[3] = bcm43xx_phy_read(dev, 0x0015);
		backup[4] = bcm43xx_phy_read(dev, 0x005A);
		backup[5] = bcm43xx_phy_read(dev, 0x0059);
		backup[6] = bcm43xx_phy_read(dev, 0x0058);
		backup[8] = bcm43xx_read16(dev, 0x03E6);
		backup[9] = bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT);
		if (phy->rev >= 3) {
			backup[10] = bcm43xx_phy_read(dev, 0x002E);
			backup[11] = bcm43xx_phy_read(dev, 0x002F);
			backup[12] = bcm43xx_phy_read(dev, 0x080F);
			backup[13] = bcm43xx_phy_read(dev, BCM43xx_PHY_G_LO_CONTROL);
			backup[14] = bcm43xx_phy_read(dev, 0x0801);
			backup[15] = bcm43xx_phy_read(dev, 0x0060);
			backup[16] = bcm43xx_phy_read(dev, 0x0014);
			backup[17] = bcm43xx_phy_read(dev, 0x0478);
			bcm43xx_phy_write(dev, 0x002E, 0);
			bcm43xx_phy_write(dev, BCM43xx_PHY_G_LO_CONTROL, 0);
			switch (phy->rev) {
			case 4: case 6: case 7:
				bcm43xx_phy_write(dev, 0x0478,
						  bcm43xx_phy_read(dev, 0x0478)
						  | 0x0100);
				bcm43xx_phy_write(dev, 0x0801,
						  bcm43xx_phy_read(dev, 0x0801)
						  | 0x0040);
				break;
			case 3: case 5:
				bcm43xx_phy_write(dev, 0x0801,
						  bcm43xx_phy_read(dev, 0x0801)
						  & 0xFFBF);
				break;
			}
			bcm43xx_phy_write(dev, 0x0060,
					  bcm43xx_phy_read(dev, 0x0060)
					  | 0x0040);
			bcm43xx_phy_write(dev, 0x0014,
					  bcm43xx_phy_read(dev, 0x0014)
					  | 0x0200);
		}
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) | 0x0070);
		bcm43xx_set_all_gains(dev, 0, 8, 0);
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) & 0x00F7);
		if (phy->rev >= 2) {
			bcm43xx_phy_write(dev, 0x0811,
					  (bcm43xx_phy_read(dev, 0x0811) & 0xFFCF) | 0x0030);
			bcm43xx_phy_write(dev, 0x0812,
					  (bcm43xx_phy_read(dev, 0x0812) & 0xFFCF) | 0x0010);
		}
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) | 0x0080);
		udelay(20);

		nrssi0 = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
		if (nrssi0 >= 0x0020)
			nrssi0 -= 0x0040;

		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) & 0x007F);
		if (phy->rev >= 2) {
			bcm43xx_phy_write(dev, 0x0003,
					  (bcm43xx_phy_read(dev, 0x0003)
					   & 0xFF9F) | 0x0040);
		}

		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT,
				bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT)
				| 0x2000);
		bcm43xx_radio_write16(dev, 0x007A,
				      bcm43xx_radio_read16(dev, 0x007A) | 0x000F);
		bcm43xx_phy_write(dev, 0x0015, 0xF330);
		if (phy->rev >= 2) {
			bcm43xx_phy_write(dev, 0x0812,
					  (bcm43xx_phy_read(dev, 0x0812) & 0xFFCF) | 0x0020);
			bcm43xx_phy_write(dev, 0x0811,
					  (bcm43xx_phy_read(dev, 0x0811) & 0xFFCF) | 0x0020);
		}

		bcm43xx_set_all_gains(dev, 3, 0, 1);
		if (phy->radio_rev == 8) {
			bcm43xx_radio_write16(dev, 0x0043, 0x001F);
		} else {
			tmp = bcm43xx_radio_read16(dev, 0x0052) & 0xFF0F;
			bcm43xx_radio_write16(dev, 0x0052, tmp | 0x0060);
			tmp = bcm43xx_radio_read16(dev, 0x0043) & 0xFFF0;
			bcm43xx_radio_write16(dev, 0x0043, tmp | 0x0009);
		}
		bcm43xx_phy_write(dev, 0x005A, 0x0480);
		bcm43xx_phy_write(dev, 0x0059, 0x0810);
		bcm43xx_phy_write(dev, 0x0058, 0x000D);
		udelay(20);
		nrssi1 = (s16)((bcm43xx_phy_read(dev, 0x047F) >> 8) & 0x003F);
		if (nrssi1 >= 0x0020)
			nrssi1 -= 0x0040;
		if (nrssi0 == nrssi1)
			phy->nrssislope = 0x00010000;
		else
			phy->nrssislope = 0x00400000 / (nrssi0 - nrssi1);
		if (nrssi0 >= -4) {
			phy->nrssi[0] = nrssi1;
			phy->nrssi[1] = nrssi0;
		}
		if (phy->rev >= 3) {
			bcm43xx_phy_write(dev, 0x002E, backup[10]);
			bcm43xx_phy_write(dev, 0x002F, backup[11]);
			bcm43xx_phy_write(dev, 0x080F, backup[12]);
			bcm43xx_phy_write(dev, BCM43xx_PHY_G_LO_CONTROL, backup[13]);
		}
		if (phy->rev >= 2) {
			bcm43xx_phy_write(dev, 0x0812,
					  bcm43xx_phy_read(dev, 0x0812) & 0xFFCF);
			bcm43xx_phy_write(dev, 0x0811,
					  bcm43xx_phy_read(dev, 0x0811) & 0xFFCF);
		}

		bcm43xx_radio_write16(dev, 0x007A, backup[0]);
		bcm43xx_radio_write16(dev, 0x0052, backup[1]);
		bcm43xx_radio_write16(dev, 0x0043, backup[2]);
		bcm43xx_write16(dev, 0x03E2, backup[7]);
		bcm43xx_write16(dev, 0x03E6, backup[8]);
		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT, backup[9]);
		bcm43xx_phy_write(dev, 0x0015, backup[3]);
		bcm43xx_phy_write(dev, 0x005A, backup[4]);
		bcm43xx_phy_write(dev, 0x0059, backup[5]);
		bcm43xx_phy_write(dev, 0x0058, backup[6]);
		bcm43xx_synth_pu_workaround(dev, phy->channel);
		bcm43xx_phy_write(dev, 0x0802,
				  bcm43xx_phy_read(dev, 0x0802) | (0x0001 | 0x0002));
		bcm43xx_set_original_gains(dev);
		bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) | 0x8000);
		if (phy->rev >= 3) {
			bcm43xx_phy_write(dev, 0x0801, backup[14]);
			bcm43xx_phy_write(dev, 0x0060, backup[15]);
			bcm43xx_phy_write(dev, 0x0014, backup[16]);
			bcm43xx_phy_write(dev, 0x0478, backup[17]);
		}
		bcm43xx_nrssi_mem_update(dev);
		bcm43xx_calc_nrssi_threshold(dev);
		break;
	default:
		assert(0);
	}
}

void bcm43xx_calc_nrssi_threshold(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s32 threshold;
	s32 a, b;
	s16 tmp16;
	u16 tmp_u16;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_B: {
		if (phy->radio_ver != 0x2050)
			return;
		if (!(dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI))
			return;

		if (phy->radio_rev >= 6) {
			threshold = (phy->nrssi[1] - phy->nrssi[0]) * 32;
			threshold += 20 * (phy->nrssi[0] + 1);
			threshold /= 40;
		} else
			threshold = phy->nrssi[1] - 5;

		threshold = limit_value(threshold, 0, 0x3E);
		bcm43xx_phy_read(dev, 0x0020); /* dummy read */
		bcm43xx_phy_write(dev, 0x0020, (((u16)threshold) << 8) | 0x001C);

		if (phy->radio_rev >= 6) {
			bcm43xx_phy_write(dev, 0x0087, 0x0E0D);
			bcm43xx_phy_write(dev, 0x0086, 0x0C0B);
			bcm43xx_phy_write(dev, 0x0085, 0x0A09);
			bcm43xx_phy_write(dev, 0x0084, 0x0808);
			bcm43xx_phy_write(dev, 0x0083, 0x0808);
			bcm43xx_phy_write(dev, 0x0082, 0x0604);
			bcm43xx_phy_write(dev, 0x0081, 0x0302);
			bcm43xx_phy_write(dev, 0x0080, 0x0100);
		}
		break;
	}
	case BCM43xx_PHYTYPE_G:
		if (!phy->gmode ||
		    !(dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI)) {
			tmp16 = bcm43xx_nrssi_hw_read(dev, 0x20);
			if (tmp16 >= 0x20)
				tmp16 -= 0x40;
			if (tmp16 < 3) {
				bcm43xx_phy_write(dev, 0x048A,
						  (bcm43xx_phy_read(dev, 0x048A)
						   & 0xF000) | 0x09EB);
			} else {
				bcm43xx_phy_write(dev, 0x048A,
						  (bcm43xx_phy_read(dev, 0x048A)
						   & 0xF000) | 0x0AED);
			}
		} else {
			if (phy->interfmode == BCM43xx_INTERFMODE_NONWLAN) {
				a = 0xE;
				b = 0xA;
			} else if (!phy->aci_wlan_automatic && phy->aci_enable) {
				a = 0x13;
				b = 0x12;
			} else {
				a = 0xE;
				b = 0x11;
			}

			a = a * (phy->nrssi[1] - phy->nrssi[0]);
			a += (phy->nrssi[0] << 6);
			if (a < 32)
				a += 31;
			else
				a += 32;
			a = a >> 6;
			a = limit_value(a, -31, 31);

			b = b * (phy->nrssi[1] - phy->nrssi[0]);
			b += (phy->nrssi[0] << 6);
			if (b < 32)
				b += 31;
			else
				b += 32;
			b = b >> 6;
			b = limit_value(b, -31, 31);

			tmp_u16 = bcm43xx_phy_read(dev, 0x048A) & 0xF000;
			tmp_u16 |= ((u32)b & 0x0000003F);
			tmp_u16 |= (((u32)a & 0x0000003F) << 6);
			bcm43xx_phy_write(dev, 0x048A, tmp_u16);
		}
		break;
	default:
		assert(0);
	}
}

/* Stack implementation to save/restore values from the
 * interference mitigation code.
 * It is save to restore values in random order.
 */
static void _stack_save(u32 *_stackptr, size_t *stackidx,
			u8 id, u16 offset, u16 value)
{
	u32 *stackptr = &(_stackptr[*stackidx]);

	assert((offset & 0xF000) == 0x0000);
	assert((id & 0xF0) == 0x00);
	*stackptr = offset;
	*stackptr |= ((u32)id) << 12;
	*stackptr |= ((u32)value) << 16;
	(*stackidx)++;
	assert(*stackidx < BCM43xx_INTERFSTACK_SIZE);
}

static u16 _stack_restore(u32 *stackptr,
			  u8 id, u16 offset)
{
	size_t i;

	assert((offset & 0xF000) == 0x0000);
	assert((id & 0xF0) == 0x00);
	for (i = 0; i < BCM43xx_INTERFSTACK_SIZE; i++, stackptr++) {
		if ((*stackptr & 0x00000FFF) != offset)
			continue;
		if (((*stackptr & 0x0000F000) >> 12) != id)
			continue;
		return ((*stackptr & 0xFFFF0000) >> 16);
	}
	assert(0);

	return 0;
}

#define phy_stacksave(offset)					\
	do {							\
		_stack_save(stack, &stackidx, 0x1, (offset),	\
			    bcm43xx_phy_read(dev, (offset)));	\
	} while (0)
#define phy_stackrestore(offset)				\
	do {							\
		bcm43xx_phy_write(dev, (offset),		\
				  _stack_restore(stack, 0x1,	\
						 (offset)));	\
	} while (0)
#define radio_stacksave(offset)						\
	do {								\
		_stack_save(stack, &stackidx, 0x2, (offset),		\
			    bcm43xx_radio_read16(dev, (offset)));	\
	} while (0)
#define radio_stackrestore(offset)					\
	do {								\
		bcm43xx_radio_write16(dev, (offset),			\
				      _stack_restore(stack, 0x2,	\
						     (offset)));	\
	} while (0)
#define ofdmtab_stacksave(table, offset)			\
	do {							\
		_stack_save(stack, &stackidx, 0x3, (offset)|(table),	\
			    bcm43xx_ofdmtab_read16(dev, (table), (offset)));	\
	} while (0)
#define ofdmtab_stackrestore(table, offset)			\
	do {							\
		bcm43xx_ofdmtab_write16(dev, (table),	(offset),	\
				  _stack_restore(stack, 0x3,	\
						 (offset)|(table)));	\
	} while (0)

static void
bcm43xx_radio_interference_mitigation_enable(struct bcm43xx_wldev *dev,
					     int mode)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 tmp, flipped;
	size_t stackidx = 0;
	u32 *stack = phy->interfstack;

	switch (mode) {
	case BCM43xx_INTERFMODE_NONWLAN:
		if (phy->rev != 1) {
			bcm43xx_phy_write(dev, 0x042B,
			                  bcm43xx_phy_read(dev, 0x042B) | 0x0800);
			bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
			                  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) & ~0x4000);
			break;
		}
		radio_stacksave(0x0078);
		tmp = (bcm43xx_radio_read16(dev, 0x0078) & 0x001E);
		flipped = flip_4bit(tmp);
		if (flipped < 10 && flipped >= 8)
			flipped = 7;
		else if (flipped >= 10)
			flipped -= 3;
		flipped = flip_4bit(flipped);
		flipped = (flipped << 1) | 0x0020;
		bcm43xx_radio_write16(dev, 0x0078, flipped);

		bcm43xx_calc_nrssi_threshold(dev);

		phy_stacksave(0x0406);
		bcm43xx_phy_write(dev, 0x0406, 0x7E28);

		bcm43xx_phy_write(dev, 0x042B,
		                  bcm43xx_phy_read(dev, 0x042B) | 0x0800);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RADIO_BITFIELD,
		                  bcm43xx_phy_read(dev, BCM43xx_PHY_RADIO_BITFIELD) | 0x1000);

		phy_stacksave(0x04A0);
		bcm43xx_phy_write(dev, 0x04A0,
		                  (bcm43xx_phy_read(dev, 0x04A0) & 0xC0C0) | 0x0008);
		phy_stacksave(0x04A1);
		bcm43xx_phy_write(dev, 0x04A1,
				  (bcm43xx_phy_read(dev, 0x04A1) & 0xC0C0) | 0x0605);
		phy_stacksave(0x04A2);
		bcm43xx_phy_write(dev, 0x04A2,
				  (bcm43xx_phy_read(dev, 0x04A2) & 0xC0C0) | 0x0204);
		phy_stacksave(0x04A8);
		bcm43xx_phy_write(dev, 0x04A8,
				  (bcm43xx_phy_read(dev, 0x04A8) & 0xC0C0) | 0x0803);
		phy_stacksave(0x04AB);
		bcm43xx_phy_write(dev, 0x04AB,
				  (bcm43xx_phy_read(dev, 0x04AB) & 0xC0C0) | 0x0605);

		phy_stacksave(0x04A7);
		bcm43xx_phy_write(dev, 0x04A7, 0x0002);
		phy_stacksave(0x04A3);
		bcm43xx_phy_write(dev, 0x04A3, 0x287A);
		phy_stacksave(0x04A9);
		bcm43xx_phy_write(dev, 0x04A9, 0x2027);
		phy_stacksave(0x0493);
		bcm43xx_phy_write(dev, 0x0493, 0x32F5);
		phy_stacksave(0x04AA);
		bcm43xx_phy_write(dev, 0x04AA, 0x2027);
		phy_stacksave(0x04AC);
		bcm43xx_phy_write(dev, 0x04AC, 0x32F5);
		break;
	case BCM43xx_INTERFMODE_MANUALWLAN:
		if (bcm43xx_phy_read(dev, 0x0033) & 0x0800)
			break;

		phy->aci_enable = 1;

		phy_stacksave(BCM43xx_PHY_RADIO_BITFIELD);
		phy_stacksave(BCM43xx_PHY_G_CRS);
		if (phy->rev < 2) {
			phy_stacksave(0x0406);
		} else {
			phy_stacksave(0x04C0);
			phy_stacksave(0x04C1);
		}
		phy_stacksave(0x0033);
		phy_stacksave(0x04A7);
		phy_stacksave(0x04A3);
		phy_stacksave(0x04A9);
		phy_stacksave(0x04AA);
		phy_stacksave(0x04AC);
		phy_stacksave(0x0493);
		phy_stacksave(0x04A1);
		phy_stacksave(0x04A0);
		phy_stacksave(0x04A2);
		phy_stacksave(0x048A);
		phy_stacksave(0x04A8);
		phy_stacksave(0x04AB);
		if (phy->rev == 2) {
			phy_stacksave(0x04AD);
			phy_stacksave(0x04AE);
		} else if (phy->rev >= 3) {
			phy_stacksave(0x04AD);
			phy_stacksave(0x0415);
			phy_stacksave(0x0416);
			phy_stacksave(0x0417);
			ofdmtab_stacksave(0x1A00, 0x2);
			ofdmtab_stacksave(0x1A00, 0x3);
		}
		phy_stacksave(0x042B);
		phy_stacksave(0x048C);

		bcm43xx_phy_write(dev, BCM43xx_PHY_RADIO_BITFIELD,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_RADIO_BITFIELD)
				  & ~0x1000);
		bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
				  (bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS)
				   & 0xFFFC) | 0x0002);

		bcm43xx_phy_write(dev, 0x0033, 0x0800);
		bcm43xx_phy_write(dev, 0x04A3, 0x2027);
		bcm43xx_phy_write(dev, 0x04A9, 0x1CA8);
		bcm43xx_phy_write(dev, 0x0493, 0x287A);
		bcm43xx_phy_write(dev, 0x04AA, 0x1CA8);
		bcm43xx_phy_write(dev, 0x04AC, 0x287A);

		bcm43xx_phy_write(dev, 0x04A0,
				  (bcm43xx_phy_read(dev, 0x04A0)
				   & 0xFFC0) | 0x001A);
		bcm43xx_phy_write(dev, 0x04A7, 0x000D);

		if (phy->rev < 2) {
			bcm43xx_phy_write(dev, 0x0406, 0xFF0D);
		} else if (phy->rev == 2) {
			bcm43xx_phy_write(dev, 0x04C0, 0xFFFF);
			bcm43xx_phy_write(dev, 0x04C1, 0x00A9);
		} else {
			bcm43xx_phy_write(dev, 0x04C0, 0x00C1);
			bcm43xx_phy_write(dev, 0x04C1, 0x0059);
		}

		bcm43xx_phy_write(dev, 0x04A1,
		                  (bcm43xx_phy_read(dev, 0x04A1)
				   & 0xC0FF) | 0x1800);
		bcm43xx_phy_write(dev, 0x04A1,
		                  (bcm43xx_phy_read(dev, 0x04A1)
				   & 0xFFC0) | 0x0015);
		bcm43xx_phy_write(dev, 0x04A8,
		                  (bcm43xx_phy_read(dev, 0x04A8)
				   & 0xCFFF) | 0x1000);
		bcm43xx_phy_write(dev, 0x04A8,
		                  (bcm43xx_phy_read(dev, 0x04A8)
				   & 0xF0FF) | 0x0A00);
		bcm43xx_phy_write(dev, 0x04AB,
		                  (bcm43xx_phy_read(dev, 0x04AB)
				   & 0xCFFF) | 0x1000);
		bcm43xx_phy_write(dev, 0x04AB,
		                  (bcm43xx_phy_read(dev, 0x04AB)
				   & 0xF0FF) | 0x0800);
		bcm43xx_phy_write(dev, 0x04AB,
		                  (bcm43xx_phy_read(dev, 0x04AB)
				   & 0xFFCF) | 0x0010);
		bcm43xx_phy_write(dev, 0x04AB,
		                  (bcm43xx_phy_read(dev, 0x04AB)
				   & 0xFFF0) | 0x0005);
		bcm43xx_phy_write(dev, 0x04A8,
		                  (bcm43xx_phy_read(dev, 0x04A8)
				   & 0xFFCF) | 0x0010);
		bcm43xx_phy_write(dev, 0x04A8,
		                  (bcm43xx_phy_read(dev, 0x04A8)
				   & 0xFFF0) | 0x0006);
		bcm43xx_phy_write(dev, 0x04A2,
		                  (bcm43xx_phy_read(dev, 0x04A2)
				   & 0xF0FF) | 0x0800);
		bcm43xx_phy_write(dev, 0x04A0,
				  (bcm43xx_phy_read(dev, 0x04A0)
				   & 0xF0FF) | 0x0500);
		bcm43xx_phy_write(dev, 0x04A2,
				  (bcm43xx_phy_read(dev, 0x04A2)
				   & 0xFFF0) | 0x000B);

		if (phy->rev >= 3) {
			bcm43xx_phy_write(dev, 0x048A,
					  bcm43xx_phy_read(dev, 0x048A)
					  & ~0x8000);
			bcm43xx_phy_write(dev, 0x0415,
					  (bcm43xx_phy_read(dev, 0x0415)
					   & 0x8000) | 0x36D8);
			bcm43xx_phy_write(dev, 0x0416,
					  (bcm43xx_phy_read(dev, 0x0416)
					   & 0x8000) | 0x36D8);
			bcm43xx_phy_write(dev, 0x0417,
					  (bcm43xx_phy_read(dev, 0x0417)
					   & 0xFE00) | 0x016D);
		} else {
			bcm43xx_phy_write(dev, 0x048A,
					  bcm43xx_phy_read(dev, 0x048A)
					  | 0x1000);
			bcm43xx_phy_write(dev, 0x048A,
					  (bcm43xx_phy_read(dev, 0x048A)
					   & 0x9FFF) | 0x2000);
			bcm43xx_hf_write(dev, bcm43xx_hf_read(dev) | BCM43xx_HF_ACIW);
		}
		if (phy->rev >= 2) {
			bcm43xx_phy_write(dev, 0x042B,
					  bcm43xx_phy_read(dev, 0x042B)
					  | 0x0800);
		}
		bcm43xx_phy_write(dev, 0x048C,
				  (bcm43xx_phy_read(dev, 0x048C)
				   & 0xF0FF) | 0x0200);
		if (phy->rev == 2) {
			bcm43xx_phy_write(dev, 0x04AE,
					  (bcm43xx_phy_read(dev, 0x04AE)
					   & 0xFF00) | 0x007F);
			bcm43xx_phy_write(dev, 0x04AD,
					  (bcm43xx_phy_read(dev, 0x04AD)
					   & 0x00FF) | 0x1300);
		} else if (phy->rev >= 6) {
			bcm43xx_ofdmtab_write16(dev, 0x1A00, 0x3, 0x007F);
			bcm43xx_ofdmtab_write16(dev, 0x1A00, 0x2, 0x007F);
			bcm43xx_phy_write(dev, 0x04AD,
					  bcm43xx_phy_read(dev, 0x04AD)
					  & 0x00FF);
		}
		bcm43xx_calc_nrssi_slope(dev);
		break;
	default:
		assert(0);
	}
}

static void
bcm43xx_radio_interference_mitigation_disable(struct bcm43xx_wldev *dev,
					      int mode)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u32 *stack = phy->interfstack;

	switch (mode) {
	case BCM43xx_INTERFMODE_NONWLAN:
		if (phy->rev != 1) {
			bcm43xx_phy_write(dev, 0x042B,
			                  bcm43xx_phy_read(dev, 0x042B) & ~0x0800);
			bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
			                  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) | 0x4000);
			break;
		}
		radio_stackrestore(0x0078);
		bcm43xx_calc_nrssi_threshold(dev);
		phy_stackrestore(0x0406);
		bcm43xx_phy_write(dev, 0x042B,
				  bcm43xx_phy_read(dev, 0x042B) & ~0x0800);
		if (!dev->bad_frames_preempt) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RADIO_BITFIELD,
					  bcm43xx_phy_read(dev, BCM43xx_PHY_RADIO_BITFIELD)
					  & ~(1 << 11));
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_G_CRS,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_G_CRS) | 0x4000);
		phy_stackrestore(0x04A0);
		phy_stackrestore(0x04A1);
		phy_stackrestore(0x04A2);
		phy_stackrestore(0x04A8);
		phy_stackrestore(0x04AB);
		phy_stackrestore(0x04A7);
		phy_stackrestore(0x04A3);
		phy_stackrestore(0x04A9);
		phy_stackrestore(0x0493);
		phy_stackrestore(0x04AA);
		phy_stackrestore(0x04AC);
		break;
	case BCM43xx_INTERFMODE_MANUALWLAN:
		if (!(bcm43xx_phy_read(dev, 0x0033) & 0x0800))
			break;

		phy->aci_enable = 0;

		phy_stackrestore(BCM43xx_PHY_RADIO_BITFIELD);
		phy_stackrestore(BCM43xx_PHY_G_CRS);
		phy_stackrestore(0x0033);
		phy_stackrestore(0x04A3);
		phy_stackrestore(0x04A9);
		phy_stackrestore(0x0493);
		phy_stackrestore(0x04AA);
		phy_stackrestore(0x04AC);
		phy_stackrestore(0x04A0);
		phy_stackrestore(0x04A7);
		if (phy->rev >= 2) {
			phy_stackrestore(0x04C0);
			phy_stackrestore(0x04C1);
		} else
			phy_stackrestore(0x0406);
		phy_stackrestore(0x04A1);
		phy_stackrestore(0x04AB);
		phy_stackrestore(0x04A8);
		if (phy->rev == 2) {
			phy_stackrestore(0x04AD);
			phy_stackrestore(0x04AE);
		} else if (phy->rev >= 3) {
			phy_stackrestore(0x04AD);
			phy_stackrestore(0x0415);
			phy_stackrestore(0x0416);
			phy_stackrestore(0x0417);
			ofdmtab_stackrestore(0x1A00, 0x2);
			ofdmtab_stackrestore(0x1A00, 0x3);
		}
		phy_stackrestore(0x04A2);
		phy_stackrestore(0x048A);
		phy_stackrestore(0x042B);
		phy_stackrestore(0x048C);
		bcm43xx_hf_write(dev, bcm43xx_hf_read(dev) & ~BCM43xx_HF_ACIW);
		bcm43xx_calc_nrssi_slope(dev);
		break;
	default:
		assert(0);
	}
}

#undef phy_stacksave
#undef phy_stackrestore
#undef radio_stacksave
#undef radio_stackrestore
#undef ofdmtab_stacksave
#undef ofdmtab_stackrestore

int bcm43xx_radio_set_interference_mitigation(struct bcm43xx_wldev *dev,
					      int mode)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int currentmode;

	if ((phy->type != BCM43xx_PHYTYPE_G) ||
	    (phy->rev == 0) ||
	    (!phy->gmode))
		return -ENODEV;

	phy->aci_wlan_automatic = 0;
	switch (mode) {
	case BCM43xx_INTERFMODE_AUTOWLAN:
		phy->aci_wlan_automatic = 1;
		if (phy->aci_enable)
			mode = BCM43xx_INTERFMODE_MANUALWLAN;
		else
			mode = BCM43xx_INTERFMODE_NONE;
		break;
	case BCM43xx_INTERFMODE_NONE:
	case BCM43xx_INTERFMODE_NONWLAN:
	case BCM43xx_INTERFMODE_MANUALWLAN:
		break;
	default:
		return -EINVAL;
	}

	currentmode = phy->interfmode;
	if (currentmode == mode)
		return 0;
	if (currentmode != BCM43xx_INTERFMODE_NONE)
		bcm43xx_radio_interference_mitigation_disable(dev, currentmode);

	if (mode == BCM43xx_INTERFMODE_NONE) {
		phy->aci_enable = 0;
		phy->aci_hw_rssi = 0;
	} else
		bcm43xx_radio_interference_mitigation_enable(dev, mode);
	phy->interfmode = mode;

	return 0;
}

static u16 bcm43xx_radio_core_calibration_value(struct bcm43xx_wldev *dev)
{
	u16 reg, index, ret;

	static const u8 rcc_table[] = {
		0x02, 0x03, 0x01, 0x0F,
		0x06, 0x07, 0x05, 0x0F,
		0x0A, 0x0B, 0x09, 0x0F,
		0x0E, 0x0F, 0x0D, 0x0F,
	};

	reg = bcm43xx_radio_read16(dev, 0x60);
	index = (reg & 0x001E) >> 1;
	ret = rcc_table[index] << 1;
	ret |= (reg & 0x0001);
	ret |= 0x0020;

	return ret;
}

#define LPD(L, P, D)	(((L) << 2) | ((P) << 1) | ((D) << 0))
static u16 radio2050_rfover_val(struct bcm43xx_wldev *dev,
				u16 phy_register,
				unsigned int lpd)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct ssb_sprom *sprom = &(dev->dev->bus->sprom);

	if (!phy->gmode)
		return 0;

	if (has_loopback_gain(phy)) {
		int max_lb_gain = phy->max_lb_gain;
		u16 extlna;
		u16 i;

		if (phy->radio_rev == 8)
			max_lb_gain += 0x3E;
		else
			max_lb_gain += 0x26;
		if (max_lb_gain >= 0x46) {
			extlna = 0x3000;
			max_lb_gain -= 0x46;
		} else if (max_lb_gain >= 0x3A) {
			extlna = 0x1000;
			max_lb_gain -= 0x3A;
		} else if (max_lb_gain >= 0x2E) {
			extlna = 0x2000;
			max_lb_gain -= 0x2E;
		} else {
			extlna = 0;
			max_lb_gain -= 0x10;
		}

		for (i = 0; i < 16; i++) {
			max_lb_gain -= (i * 6);
			if (max_lb_gain < 6)
				break;
		}

		if ((phy->rev < 7) ||
		    !(sprom->r1.boardflags_lo & BCM43xx_BFL_EXTLNA)) {
			if (phy_register == BCM43xx_PHY_RFOVER) {
				return 0x1B3;
			} else if (phy_register == BCM43xx_PHY_RFOVERVAL) {
				extlna |= (i << 8);
				switch (lpd) {
				case LPD(0, 1, 1):
					return 0x0F92;
				case LPD(0, 0, 1):
				case LPD(1, 0, 1):
					return (0x0092 | extlna);
				case LPD(1, 0, 0):
					return (0x0093 | extlna);
				}
				assert(0);
			}
			assert(0);
		} else {
			if (phy_register == BCM43xx_PHY_RFOVER) {
				return 0x9B3;
			} else if (phy_register == BCM43xx_PHY_RFOVERVAL) {
				if (extlna)
					extlna |= 0x8000;
				extlna |= (i << 8);
				switch (lpd) {
				case LPD(0, 1, 1):
					return 0x8F92;
				case LPD(0, 0, 1):
					return (0x8092 | extlna);
				case LPD(1, 0, 1):
					return (0x2092 | extlna);
				case LPD(1, 0, 0):
					return (0x2093 | extlna);
				}
				assert(0);
			}
			assert(0);
		}
	} else {
		if ((phy->rev < 7) ||
		    !(sprom->r1.boardflags_lo & BCM43xx_BFL_EXTLNA)) {
			if (phy_register == BCM43xx_PHY_RFOVER) {
				return 0x1B3;
			} else if (phy_register == BCM43xx_PHY_RFOVERVAL) {
				switch (lpd) {
				case LPD(0, 1, 1):
					return 0x0FB2;
				case LPD(0, 0, 1):
					return 0x00B2;
				case LPD(1, 0, 1):
					return 0x30B2;
				case LPD(1, 0, 0):
					return 0x30B3;
				}
				assert(0);
			}
			assert(0);
		} else {
			if (phy_register == BCM43xx_PHY_RFOVER) {
				return 0x9B3;
			} else if (phy_register == BCM43xx_PHY_RFOVERVAL) {
				switch (lpd) {
				case LPD(0, 1, 1):
					return 0x8FB2;
				case LPD(0, 0, 1):
					return 0x80B2;
				case LPD(1, 0, 1):
					return 0x20B2;
				case LPD(1, 0, 0):
					return 0x20B3;
				}
				assert(0);
			}
			assert(0);
		}
	}
	return 0;
}

struct init2050_saved_values {
	/* Core registers */
	u16 reg_3EC;
	u16 reg_3E6;
	u16 reg_3F4;
	/* Radio registers */
	u16 radio_43;
	u16 radio_51;
	u16 radio_52;
	/* PHY registers */
	u16 phy_pgactl;
	u16 phy_base_5A;
	u16 phy_base_59;
	u16 phy_base_58;
	u16 phy_base_30;
	u16 phy_rfover;
	u16 phy_rfoverval;
	u16 phy_analogover;
	u16 phy_analogoverval;
	u16 phy_crs0;
	u16 phy_classctl;
	u16 phy_lo_mask;
	u16 phy_lo_ctl;
	u16 phy_syncctl;
};

u16 bcm43xx_radio_init2050(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct init2050_saved_values sav;
	u16 rcc;
	u16 radio78;
	u16 ret;
	u16 i, j;
	u32 tmp1 = 0, tmp2 = 0;

	memset(&sav, 0, sizeof(sav)); /* get rid of "may be used uninitialized..." */

	sav.radio_43 = bcm43xx_radio_read16(dev, 0x43);
	sav.radio_51 = bcm43xx_radio_read16(dev, 0x51);
	sav.radio_52 = bcm43xx_radio_read16(dev, 0x52);
	sav.phy_pgactl = bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL);
	sav.phy_base_5A = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x5A));
	sav.phy_base_59 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x59));
	sav.phy_base_58 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x58));

	if (phy->type == BCM43xx_PHYTYPE_B) {
		sav.phy_base_30 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x30));
		sav.reg_3EC = bcm43xx_read16(dev, 0x3EC);

		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x30), 0xFF);
		bcm43xx_write16(dev, 0x3EC, 0x3F3F);
	} else if (phy->gmode || phy->rev >= 2) {
		sav.phy_rfover = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER);
		sav.phy_rfoverval = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL);
		sav.phy_analogover = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER);
		sav.phy_analogoverval = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL);
		sav.phy_crs0 = bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0);
		sav.phy_classctl = bcm43xx_phy_read(dev, BCM43xx_PHY_CLASSCTL);

		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER,
		                  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER)
				  | 0x0003);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL,
		                  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL)
				  & 0xFFFC);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0,
			          bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0)
				  & 0x7FFF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CLASSCTL,
			          bcm43xx_phy_read(dev, BCM43xx_PHY_CLASSCTL)
				  & 0xFFFC);
		if (has_loopback_gain(phy)) {
			sav.phy_lo_mask = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_MASK);
			sav.phy_lo_ctl = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_CTL);

			if (phy->rev >= 3)
				bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0xC020);
			else
				bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0x8020);
			bcm43xx_phy_write(dev, BCM43xx_PHY_LO_CTL, 0);
		}

		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
				  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
						       LPD(0, 1, 1)));
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER,
				  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVER, 0));
	}
	bcm43xx_write16(dev, 0x3E2, bcm43xx_read16(dev, 0x3E2) | 0x8000);

	sav.phy_syncctl = bcm43xx_phy_read(dev, BCM43xx_PHY_SYNCCTL);
	bcm43xx_phy_write(dev, BCM43xx_PHY_SYNCCTL,
	                  bcm43xx_phy_read(dev, BCM43xx_PHY_SYNCCTL)
			  & 0xFF7F);
	sav.reg_3E6 = bcm43xx_read16(dev, 0x3E6);
	sav.reg_3F4 = bcm43xx_read16(dev, 0x3F4);

	if (phy->analog == 0) {
		bcm43xx_write16(dev, 0x03E6, 0x0122);
	} else {
		if (phy->analog >= 2) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x03),
					  (bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x03))
					   & 0xFFBF) | 0x40);
		}
		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT,
		                (bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT) | 0x2000));
	}

	rcc = bcm43xx_radio_core_calibration_value(dev);

	if (phy->type == BCM43xx_PHYTYPE_B)
		bcm43xx_radio_write16(dev, 0x78, 0x26);
	if (phy->gmode || phy->rev >= 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
				  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
						       LPD(0, 1, 1)));
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xBFAF);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B), 0x1403);
	if (phy->gmode || phy->rev >= 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
				  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
						       LPD(0, 0, 1)));
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xBFA0);
	bcm43xx_radio_write16(dev, 0x51,
	                      bcm43xx_radio_read16(dev, 0x51)
			      | 0x0004);
	if (phy->radio_rev == 8) {
		bcm43xx_radio_write16(dev, 0x43, 0x1F);
	} else {
		bcm43xx_radio_write16(dev, 0x52, 0);
		bcm43xx_radio_write16(dev, 0x43,
				      (bcm43xx_radio_read16(dev, 0x43)
				       & 0xFFF0) | 0x0009);
	}
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0);

	for (i = 0; i < 16; i++) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x5A), 0x0480);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x59), 0xC810);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0x000D);
		if (phy->gmode || phy->rev >= 2) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
							       LPD(1, 0, 1)));
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xAFB0);
		udelay(10);
		if (phy->gmode || phy->rev >= 2) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
							       LPD(1, 0, 1)));
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xEFB0);
		udelay(10);
		if (phy->gmode || phy->rev >= 2) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
							       LPD(1, 0, 0)));
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xFFF0);
		udelay(20);
		tmp1 += bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0);
		if (phy->gmode || phy->rev >= 2) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
					  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
							       LPD(1, 0, 1)));
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xAFB0);
	}
	udelay(10);

	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0);
	tmp1++;
	tmp1 >>= 9;

	for (i = 0; i < 16; i++) {
		radio78 = ((flip_4bit(i) << 1) | 0x20);
		bcm43xx_radio_write16(dev, 0x78, radio78);
		udelay(10);
		for (j = 0; j < 16; j++) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x5A), 0x0D80);
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x59), 0xC810);
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0x000D);
			if (phy->gmode || phy->rev >= 2) {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
						  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
								       LPD(1, 0, 1)));
			}
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xAFB0);
			udelay(10);
			if (phy->gmode || phy->rev >= 2) {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
						  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
								       LPD(1, 0, 1)));
			}
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xEFB0);
			udelay(10);
			if (phy->gmode || phy->rev >= 2) {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
						  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
								       LPD(1, 0, 0)));
			}
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xFFF0);
			udelay(10);
			tmp2 += bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE);
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), 0);
			if (phy->gmode || phy->rev >= 2) {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL,
						  radio2050_rfover_val(dev, BCM43xx_PHY_RFOVERVAL,
								       LPD(1, 0, 1)));
			}
			bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xAFB0);
		}
		tmp2++;
		tmp2 >>= 8;
		if (tmp1 < tmp2)
			break;
	}

	/* Restore the registers */
	bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, sav.phy_pgactl);
	bcm43xx_radio_write16(dev, 0x51, sav.radio_51);
	bcm43xx_radio_write16(dev, 0x52, sav.radio_52);
	bcm43xx_radio_write16(dev, 0x43, sav.radio_43);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x5A), sav.phy_base_5A);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x59), sav.phy_base_59);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x58), sav.phy_base_58);
	bcm43xx_write16(dev, 0x3E6, sav.reg_3E6);
	if (phy->analog != 0)
		bcm43xx_write16(dev, 0x3F4, sav.reg_3F4);
	bcm43xx_phy_write(dev, BCM43xx_PHY_SYNCCTL, sav.phy_syncctl);
	bcm43xx_synth_pu_workaround(dev, phy->channel);
	if (phy->type == BCM43xx_PHYTYPE_B) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x30), sav.phy_base_30);
		bcm43xx_write16(dev, 0x3EC, sav.reg_3EC);
	} else if (phy->gmode) {
		bcm43xx_write16(dev, BCM43xx_MMIO_PHY_RADIO,
				bcm43xx_read16(dev, BCM43xx_MMIO_PHY_RADIO)
				& 0x7FFF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, sav.phy_rfover);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, sav.phy_rfoverval);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER, sav.phy_analogover);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL, sav.phy_analogoverval);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0, sav.phy_crs0);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CLASSCTL, sav.phy_classctl);
		if (has_loopback_gain(phy)) {
			bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, sav.phy_lo_mask);
			bcm43xx_phy_write(dev, BCM43xx_PHY_LO_CTL, sav.phy_lo_ctl);
		}
	}
	if (i > 15)
		ret = radio78;
	else
		ret = rcc;

	return ret;
}

void bcm43xx_radio_init2060(struct bcm43xx_wldev *dev)
{
	int err;

	bcm43xx_radio_write16(dev, 0x0004, 0x00C0);
	bcm43xx_radio_write16(dev, 0x0005, 0x0008);
	bcm43xx_radio_write16(dev, 0x0009, 0x0040);
	bcm43xx_radio_write16(dev, 0x0005, 0x00AA);
	bcm43xx_radio_write16(dev, 0x0032, 0x008F);
	bcm43xx_radio_write16(dev, 0x0006, 0x008F);
	bcm43xx_radio_write16(dev, 0x0034, 0x008F);
	bcm43xx_radio_write16(dev, 0x002C, 0x0007);
	bcm43xx_radio_write16(dev, 0x0082, 0x0080);
	bcm43xx_radio_write16(dev, 0x0080, 0x0000);
	bcm43xx_radio_write16(dev, 0x003F, 0x00DA);
	bcm43xx_radio_write16(dev, 0x0005, bcm43xx_radio_read16(dev, 0x0005) & ~0x0008);
	bcm43xx_radio_write16(dev, 0x0081, bcm43xx_radio_read16(dev, 0x0081) & ~0x0010);
	bcm43xx_radio_write16(dev, 0x0081, bcm43xx_radio_read16(dev, 0x0081) & ~0x0020);
	bcm43xx_radio_write16(dev, 0x0081, bcm43xx_radio_read16(dev, 0x0081) & ~0x0020);
	msleep(1); /* delay 400usec */

	bcm43xx_radio_write16(dev, 0x0081, (bcm43xx_radio_read16(dev, 0x0081) & ~0x0020) | 0x0010);
	msleep(1); /* delay 400usec */

	bcm43xx_radio_write16(dev, 0x0005, (bcm43xx_radio_read16(dev, 0x0005) & ~0x0008) | 0x0008);
	bcm43xx_radio_write16(dev, 0x0085, bcm43xx_radio_read16(dev, 0x0085) & ~0x0010);
	bcm43xx_radio_write16(dev, 0x0005, bcm43xx_radio_read16(dev, 0x0005) & ~0x0008);
	bcm43xx_radio_write16(dev, 0x0081, bcm43xx_radio_read16(dev, 0x0081) & ~0x0040);
	bcm43xx_radio_write16(dev, 0x0081, (bcm43xx_radio_read16(dev, 0x0081) & ~0x0040) | 0x0040);
	bcm43xx_radio_write16(dev, 0x0005, (bcm43xx_radio_read16(dev, 0x0081) & ~0x0008) | 0x0008);
	bcm43xx_phy_write(dev, 0x0063, 0xDDC6);
	bcm43xx_phy_write(dev, 0x0069, 0x07BE);
	bcm43xx_phy_write(dev, 0x006A, 0x0000);

	err = bcm43xx_radio_selectchannel(dev, BCM43xx_DEFAULT_CHANNEL_A, 0);
	assert(err == 0);

	msleep(1);
}

static inline
u16 freq_r3A_value(u16 frequency)
{
	u16 value;

	if (frequency < 5091)
		value = 0x0040;
	else if (frequency < 5321)
		value = 0x0000;
	else if (frequency < 5806)
		value = 0x0080;
	else
		value = 0x0040;

	return value;
}

void bcm43xx_radio_set_tx_iq(struct bcm43xx_wldev *dev)
{
	static const u8 data_high[5] = { 0x00, 0x40, 0x80, 0x90, 0xD0 };
	static const u8 data_low[5]  = { 0x00, 0x01, 0x05, 0x06, 0x0A };
	u16 tmp = bcm43xx_radio_read16(dev, 0x001E);
	int i, j;

	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (tmp == (data_high[i] << 4 | data_low[j])) {
				bcm43xx_phy_write(dev, 0x0069, (i - j) << 8 | 0x00C0);
				return;
			}
		}
	}
}

int bcm43xx_radio_selectchannel(struct bcm43xx_wldev *dev,
				u8 channel,
				int synthetic_pu_workaround)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 r8, tmp;
	u16 freq;
	u16 channelcookie;

	/* First we set the channel radio code to prevent the
	 * firmware from sending ghost packets.
	 */
	channelcookie = channel;
	if (phy->type == BCM43xx_PHYTYPE_A)
		channelcookie |= 0x100;
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_CHAN, channelcookie);

	if (phy->type == BCM43xx_PHYTYPE_A) {
		if (channel > 200)
			return -EINVAL;
		freq = channel2freq_a(channel);

		r8 = bcm43xx_radio_read16(dev, 0x0008);
		bcm43xx_write16(dev, 0x03F0, freq);
		bcm43xx_radio_write16(dev, 0x0008, r8);

		TODO();//TODO: write max channel TX power? to Radio 0x2D
		tmp = bcm43xx_radio_read16(dev, 0x002E);
		tmp &= 0x0080;
		TODO();//TODO: OR tmp with the Power out estimation for this channel?
		bcm43xx_radio_write16(dev, 0x002E, tmp);

		if (freq >= 4920 && freq <= 5500) {
			/*
			 * r8 = (((freq * 15 * 0xE1FC780F) >> 32) / 29) & 0x0F;
			 *    = (freq * 0.025862069
			 */
			r8 = 3 * freq / 116; /* is equal to r8 = freq * 0.025862 */
		}
		bcm43xx_radio_write16(dev, 0x0007, (r8 << 4) | r8);
		bcm43xx_radio_write16(dev, 0x0020, (r8 << 4) | r8);
		bcm43xx_radio_write16(dev, 0x0021, (r8 << 4) | r8);
		bcm43xx_radio_write16(dev, 0x0022,
				      (bcm43xx_radio_read16(dev, 0x0022)
				       & 0x000F) | (r8 << 4));
		bcm43xx_radio_write16(dev, 0x002A, (r8 << 4));
		bcm43xx_radio_write16(dev, 0x002B, (r8 << 4));
		bcm43xx_radio_write16(dev, 0x0008,
				      (bcm43xx_radio_read16(dev, 0x0008)
				       & 0x00F0) | (r8 << 4));
		bcm43xx_radio_write16(dev, 0x0029,
				      (bcm43xx_radio_read16(dev, 0x0029)
				       & 0xFF0F) | 0x00B0);
		bcm43xx_radio_write16(dev, 0x0035, 0x00AA);
		bcm43xx_radio_write16(dev, 0x0036, 0x0085);
		bcm43xx_radio_write16(dev, 0x003A,
				      (bcm43xx_radio_read16(dev, 0x003A)
				       & 0xFF20) | freq_r3A_value(freq));
		bcm43xx_radio_write16(dev, 0x003D,
				      bcm43xx_radio_read16(dev, 0x003D) & 0x00FF);
		bcm43xx_radio_write16(dev, 0x0081,
				      (bcm43xx_radio_read16(dev, 0x0081)
				       & 0xFF7F) | 0x0080);
		bcm43xx_radio_write16(dev, 0x0035,
				      bcm43xx_radio_read16(dev, 0x0035) & 0xFFEF);
		bcm43xx_radio_write16(dev, 0x0035,
				      (bcm43xx_radio_read16(dev, 0x0035)
				       & 0xFFEF) | 0x0010);
		bcm43xx_radio_set_tx_iq(dev);
		TODO();	//TODO:	TSSI2dbm workaround
		bcm43xx_phy_xmitpower(dev);//FIXME correct?
	} else {
		if ((channel < 1) || (channel > 14))
			return -EINVAL;

		if (synthetic_pu_workaround)
			bcm43xx_synth_pu_workaround(dev, channel);

		bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL,
				channel2freq_bg(channel));

		if (channel == 14) {
			if (dev->dev->bus->sprom.r1.country_code == SSB_SPROM1CCODE_JAPAN)
				bcm43xx_hf_write(dev, bcm43xx_hf_read(dev) & ~BCM43xx_HF_ACPR);
			else
				bcm43xx_hf_write(dev, bcm43xx_hf_read(dev) | BCM43xx_HF_ACPR);
			bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT,
					bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT)
					| (1 << 11));
		} else {
			bcm43xx_write16(dev, BCM43xx_MMIO_CHANNEL_EXT,
					bcm43xx_read16(dev, BCM43xx_MMIO_CHANNEL_EXT)
					& 0xF7BF);
		}
	}

	phy->channel = channel;
	/* Wait for the radio to tune to the channel and stabilize. */
	msleep(8);

	return 0;
}

/* http://bcm-specs.sipsolutions.net/TX_Gain_Base_Band */
static u16 bcm43xx_get_txgain_base_band(u16 txpower)
{
	u16 ret;

	assert(txpower <= 63);

	if (txpower >= 54)
		ret = 2;
	else if (txpower >= 49)
		ret = 4;
	else if (txpower >= 44)
		ret = 5;
	else
		ret = 6;

	return ret;
}

/* http://bcm-specs.sipsolutions.net/TX_Gain_Radio_Frequency_Power_Amplifier */
static u16 bcm43xx_get_txgain_freq_power_amp(u16 txpower)
{
	u16 ret;

	assert(txpower <= 63);

	if (txpower >= 32)
		ret = 0;
	else if (txpower >= 25)
		ret = 1;
	else if (txpower >= 20)
		ret = 2;
	else if (txpower >= 12)
		ret = 3;
	else
		ret = 4;

	return ret;
}

/* http://bcm-specs.sipsolutions.net/TX_Gain_Digital_Analog_Converter */
static u16 bcm43xx_get_txgain_dac(u16 txpower)
{
	u16 ret;

	assert(txpower <= 63);

	if (txpower >= 54)
		ret = txpower - 53;
	else if (txpower >= 49)
		ret = txpower - 42;
	else if (txpower >= 44)
		ret = txpower - 37;
	else if (txpower >= 32)
		ret = txpower - 32;
	else if (txpower >= 25)
		ret = txpower - 20;
	else if (txpower >= 20)
		ret = txpower - 13;
	else if (txpower >= 12)
		ret = txpower - 8;
	else
		ret = txpower;

	return ret;
}

static void bcm43xx_radio_set_txpower_a(struct bcm43xx_wldev *dev, u16 txpower)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 pamp, base, dac, t;

	txpower = limit_value(txpower, 0, 63);

	pamp = bcm43xx_get_txgain_freq_power_amp(txpower);
	pamp <<= 5;
	pamp &= 0x00E0;
	bcm43xx_phy_write(dev, 0x0019, pamp);

	base = bcm43xx_get_txgain_base_band(txpower);
	base &= 0x000F;
	bcm43xx_phy_write(dev, 0x0017, base | 0x0020);

	t = bcm43xx_ofdmtab_read16(dev, 0x3000, 1);
	t &= 0x0007;

	dac = bcm43xx_get_txgain_dac(txpower);
	dac <<= 3;
	dac |= t;

	bcm43xx_ofdmtab_write16(dev, 0x3000, 1, dac);

	phy->txpwr_offset = txpower;

	TODO();
	//TODO: FuncPlaceholder (Adjust BB loft cancel)
}

void bcm43xx_radio_turn_on(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int err;

	might_sleep();

	if (phy->radio_on)
		return;

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
		bcm43xx_radio_write16(dev, 0x0004, 0x00C0);
		bcm43xx_radio_write16(dev, 0x0005, 0x0008);
		bcm43xx_phy_write(dev, 0x0010, bcm43xx_phy_read(dev, 0x0010) & 0xFFF7);
		bcm43xx_phy_write(dev, 0x0011, bcm43xx_phy_read(dev, 0x0011) & 0xFFF7);
		bcm43xx_radio_init2060(dev);
		break;
	case BCM43xx_PHYTYPE_B:
	case BCM43xx_PHYTYPE_G:
		bcm43xx_phy_write(dev, 0x0015, 0x8000);
		bcm43xx_phy_write(dev, 0x0015, 0xCC00);
		bcm43xx_phy_write(dev, 0x0015, (phy->gmode ? 0x00C0 : 0x0000));
		err = bcm43xx_radio_selectchannel(dev, BCM43xx_DEFAULT_CHANNEL_BG, 1);
		assert(err == 0);
		break;
	default:
		assert(0);
	}
	phy->radio_on = 1;
	dprintk(KERN_INFO PFX "Radio turned on\n");
}

void bcm43xx_radio_turn_off(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->type == BCM43xx_PHYTYPE_A) {
		bcm43xx_radio_write16(dev, 0x0004, 0x00FF);
		bcm43xx_radio_write16(dev, 0x0005, 0x00FB);
		bcm43xx_phy_write(dev, 0x0010, bcm43xx_phy_read(dev, 0x0010) | 0x0008);
		bcm43xx_phy_write(dev, 0x0011, bcm43xx_phy_read(dev, 0x0011) | 0x0008);
	}
	if (phy->type == BCM43xx_PHYTYPE_G && dev->dev->id.revision >= 5) {
		bcm43xx_phy_write(dev, 0x0811, bcm43xx_phy_read(dev, 0x0811) | 0x008C);
		bcm43xx_phy_write(dev, 0x0812, bcm43xx_phy_read(dev, 0x0812) & 0xFF73);
	} else
		bcm43xx_phy_write(dev, 0x0015, 0xAA00);
	phy->radio_on = 0;
	dprintk(KERN_INFO PFX "Radio turned off\n");
}
