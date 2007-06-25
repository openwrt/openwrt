/*

  Broadcom BCM43xx wireless driver

  G PHY LO (LocalOscillator) Measuring and Control routines

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
  Copyright (c) 2005, 2006 Stefano Brivio <st3@riseup.net>
  Copyright (c) 2005-2007 Michael Buesch <mb@bu3sch.de>
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

#include "bcm43xx.h"
#include "bcm43xx_lo.h"
#include "bcm43xx_phy.h"
#include "bcm43xx_main.h"

#include <linux/delay.h>
#include <linux/sched.h>


/* Write the LocalOscillator Control (adjust) value-pair. */
static void bcm43xx_lo_write(struct bcm43xx_wldev *dev,
			     struct bcm43xx_loctl *control)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 value;
	u16 reg;

	if (BCM43xx_DEBUG) {
		if (unlikely(abs(control->i) > 16 ||
			     abs(control->q) > 16)) {
			printk(KERN_ERR PFX "ERROR: Invalid LO control pair "
					    "(I: %d, Q: %d)\n",
			       control->i, control->q);
			dump_stack();
			return;
		}
	}

	value = (u8)(control->q);
	value |= ((u8)(control->i)) << 8;

	reg = (phy->type == BCM43xx_PHYTYPE_B) ? 0x002F : BCM43xx_PHY_LO_CTL;
	bcm43xx_phy_write(dev, reg, value);
}

static inline
int assert_rfatt_and_bbatt(const struct bcm43xx_rfatt *rfatt,
			   const struct bcm43xx_bbatt *bbatt)
{
	int err = 0;

	/* Check the attenuation values against the LO control array sizes. */
#if BCM43xx_DEBUG
	if (rfatt->att >= BCM43xx_NR_RF) {
		dprintk(KERN_ERR PFX
			"ERROR: rfatt(%u) >= size of LO array\n",
			rfatt->att);
		err = -EINVAL;
	}
	if (bbatt->att >= BCM43xx_NR_BB) {
		dprintk(KERN_ERR PFX
			"ERROR: bbatt(%u) >= size of LO array\n",
			bbatt->att);
		err = -EINVAL;
	}
	if (err)
		dump_stack();
#endif /* BCM43xx_DEBUG */

	return err;
}

static
struct bcm43xx_loctl * bcm43xx_get_lo_g_ctl_nopadmix(struct bcm43xx_wldev *dev,
						  const struct bcm43xx_rfatt *rfatt,
						  const struct bcm43xx_bbatt *bbatt)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;

	if (assert_rfatt_and_bbatt(rfatt, bbatt))
		return &(lo->no_padmix[0][0]); /* Just prevent a crash */
	return &(lo->no_padmix[bbatt->att][rfatt->att]);
}

struct bcm43xx_loctl * bcm43xx_get_lo_g_ctl(struct bcm43xx_wldev *dev,
					 const struct bcm43xx_rfatt *rfatt,
					 const struct bcm43xx_bbatt *bbatt)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;

	if (assert_rfatt_and_bbatt(rfatt, bbatt))
		return &(lo->no_padmix[0][0]); /* Just prevent a crash */
	if (rfatt->with_padmix)
		return &(lo->with_padmix[bbatt->att][rfatt->att]);
	return &(lo->no_padmix[bbatt->att][rfatt->att]);
}

/* Call a function for every possible LO control value-pair. */
static void bcm43xx_call_for_each_loctl(struct bcm43xx_wldev *dev,
					void (*func)(struct bcm43xx_wldev *,
						     struct bcm43xx_loctl *))
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *ctl = phy->lo_control;
	int i, j;

	for (i = 0; i < BCM43xx_NR_BB; i++) {
		for (j = 0; j < BCM43xx_NR_RF; j++)
			func(dev, &(ctl->with_padmix[i][j]));
	}
	for (i = 0; i < BCM43xx_NR_BB; i++) {
		for (j = 0; j < BCM43xx_NR_RF; j++)
			func(dev, &(ctl->no_padmix[i][j]));
	}
}

static u16 lo_b_r15_loop(struct bcm43xx_wldev *dev)
{
	int i;
	u16 ret = 0;

	for (i = 0; i < 10; i++){
		bcm43xx_phy_write(dev, 0x0015, 0xAFA0);
		udelay(1);
		bcm43xx_phy_write(dev, 0x0015, 0xEFA0);
		udelay(10);
		bcm43xx_phy_write(dev, 0x0015, 0xFFA0);
		udelay(40);
		ret += bcm43xx_phy_read(dev, 0x002C);
	}

	return ret;
}

void bcm43xx_lo_b_measure(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 regstack[12] = { 0 };
	u16 mls;
	u16 fval;
	int i, j;

	regstack[0] = bcm43xx_phy_read(dev, 0x0015);
	regstack[1] = bcm43xx_radio_read16(dev, 0x0052) & 0xFFF0;

	if (phy->radio_ver == 0x2053) {
		regstack[2] = bcm43xx_phy_read(dev, 0x000A);
		regstack[3] = bcm43xx_phy_read(dev, 0x002A);
		regstack[4] = bcm43xx_phy_read(dev, 0x0035);
		regstack[5] = bcm43xx_phy_read(dev, 0x0003);
		regstack[6] = bcm43xx_phy_read(dev, 0x0001);
		regstack[7] = bcm43xx_phy_read(dev, 0x0030);

		regstack[8] = bcm43xx_radio_read16(dev, 0x0043);
		regstack[9] = bcm43xx_radio_read16(dev, 0x007A);
		regstack[10] = bcm43xx_read16(dev, 0x03EC);
		regstack[11] = bcm43xx_radio_read16(dev, 0x0052) & 0x00F0;

		bcm43xx_phy_write(dev, 0x0030, 0x00FF);
		bcm43xx_write16(dev, 0x03EC, 0x3F3F);
		bcm43xx_phy_write(dev, 0x0035, regstack[4] & 0xFF7F);
		bcm43xx_radio_write16(dev, 0x007A, regstack[9] & 0xFFF0);
	}
	bcm43xx_phy_write(dev, 0x0015, 0xB000);
	bcm43xx_phy_write(dev, 0x002B, 0x0004);

	if (phy->radio_ver == 0x2053) {
		bcm43xx_phy_write(dev, 0x002B, 0x0203);
		bcm43xx_phy_write(dev, 0x002A, 0x08A3);
	}

	phy->minlowsig[0] = 0xFFFF;

	for (i = 0; i < 4; i++) {
		bcm43xx_radio_write16(dev, 0x0052, regstack[1] | i);
		lo_b_r15_loop(dev);
	}
	for (i = 0; i < 10; i++) {
		bcm43xx_radio_write16(dev, 0x0052, regstack[1] | i);
		mls = lo_b_r15_loop(dev) / 10;
		if (mls < phy->minlowsig[0]) {
			phy->minlowsig[0] = mls;
			phy->minlowsigpos[0] = i;
		}
	}
	bcm43xx_radio_write16(dev, 0x0052, regstack[1] | phy->minlowsigpos[0]);

	phy->minlowsig[1] = 0xFFFF;

	for (i = -4; i < 5; i += 2) {
		for (j = -4; j < 5; j += 2) {
			if (j < 0)
				fval = (0x0100 * i) + j + 0x0100;
			else
				fval = (0x0100 * i) + j;
			bcm43xx_phy_write(dev, 0x002F, fval);
			mls = lo_b_r15_loop(dev) / 10;
			if (mls < phy->minlowsig[1]) {
				phy->minlowsig[1] = mls;
				phy->minlowsigpos[1] = fval;
			}
		}
	}
	phy->minlowsigpos[1] += 0x0101;

	bcm43xx_phy_write(dev, 0x002F, phy->minlowsigpos[1]);
	if (phy->radio_ver == 0x2053) {
		bcm43xx_phy_write(dev, 0x000A, regstack[2]);
		bcm43xx_phy_write(dev, 0x002A, regstack[3]);
		bcm43xx_phy_write(dev, 0x0035, regstack[4]);
		bcm43xx_phy_write(dev, 0x0003, regstack[5]);
		bcm43xx_phy_write(dev, 0x0001, regstack[6]);
		bcm43xx_phy_write(dev, 0x0030, regstack[7]);

		bcm43xx_radio_write16(dev, 0x0043, regstack[8]);
		bcm43xx_radio_write16(dev, 0x007A, regstack[9]);

		bcm43xx_radio_write16(dev, 0x0052,
		                      (bcm43xx_radio_read16(dev, 0x0052) & 0x000F)
				      | regstack[11]);

		bcm43xx_write16(dev, 0x03EC, regstack[10]);
	}
	bcm43xx_phy_write(dev, 0x0015, regstack[0]);
}

static u16 lo_measure_feedthrough(struct bcm43xx_wldev *dev,
				  u16 lna, u16 pga, u16 trsw_rx)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 rfover;
	u16 feedthrough;

	if (phy->gmode) {
		lna <<= BCM43xx_PHY_RFOVERVAL_LNA_SHIFT;
		pga <<= BCM43xx_PHY_RFOVERVAL_PGA_SHIFT;

		assert((lna & ~BCM43xx_PHY_RFOVERVAL_LNA) == 0);
		assert((pga & ~BCM43xx_PHY_RFOVERVAL_PGA) == 0);
/*FIXME This assertion fails		assert((trsw_rx & ~(BCM43xx_PHY_RFOVERVAL_TRSWRX |
				    BCM43xx_PHY_RFOVERVAL_BW)) == 0);
*/
trsw_rx &= (BCM43xx_PHY_RFOVERVAL_TRSWRX | BCM43xx_PHY_RFOVERVAL_BW);

		/* Construct the RF Override Value */
		rfover = BCM43xx_PHY_RFOVERVAL_UNK;
		rfover |= pga;
		rfover |= lna;
		rfover |= trsw_rx;
		if ((dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_EXTLNA) &&
		    phy->rev > 6)
			rfover |= BCM43xx_PHY_RFOVERVAL_EXTLNA;

		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xE300);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, rfover);
		udelay(10);
		rfover |= BCM43xx_PHY_RFOVERVAL_BW_LBW;
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, rfover);
		udelay(10);
		rfover |= BCM43xx_PHY_RFOVERVAL_BW_LPF;
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, rfover);
		udelay(10);
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xF300);
	} else {
		pga |= BCM43xx_PHY_PGACTL_UNKNOWN;
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, pga);
		udelay(10);
		pga |= BCM43xx_PHY_PGACTL_LOWBANDW;
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, pga);
		udelay(10);
		pga |= BCM43xx_PHY_PGACTL_LPF;
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, pga);
	}
	udelay(21);
	feedthrough = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_LEAKAGE);

	/* This is a good place to check if we need to relax a bit,
	 * as this is the main function called regularly
	 * in the LO calibration. */
	cond_resched();

	return feedthrough;
}

/* TXCTL Register and Value Table.
 * Returns the "TXCTL Register".
 * "value" is the "TXCTL Value".
 * "pad_mix_gain" is the PAD Mixer Gain.
 */
static u16 lo_txctl_register_table(struct bcm43xx_wldev *dev,
				   u16 *value, u16 *pad_mix_gain)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 reg, v, padmix;

	if (phy->type == BCM43xx_PHYTYPE_B) {
		v = 0x30;
		if (phy->radio_rev <= 5) {
			reg = 0x43;
			padmix = 0;
		} else {
			reg = 0x52;
			padmix = 5;
		}
	} else {
		if (phy->rev >= 2 && phy->radio_rev == 8) {
			reg = 0x43;
			v = 0x10;
			padmix = 2;
		} else {
			reg = 0x52;
			v = 0x30;
			padmix = 5;
		}
	}
	if (value)
		*value = v;
	if (pad_mix_gain)
		*pad_mix_gain = padmix;

	return reg;
}

static void lo_measure_txctl_values(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 reg, mask;
	u16 trsw_rx, pga;
	u16 radio_pctl_reg;

	static const u8 tx_bias_values[] = {
		0x09, 0x08, 0x0A, 0x01, 0x00,
		0x02, 0x05, 0x04, 0x06,
	};
	static const u8 tx_magn_values[] = {
		0x70, 0x40,
	};

	if (!has_loopback_gain(phy)) {
		radio_pctl_reg = 6;
		trsw_rx = 2;
		pga = 0;
	} else {
		int lb_gain; /* Loopback gain (in dB) */

		trsw_rx = 0;
		lb_gain = phy->max_lb_gain / 2;
		if (lb_gain > 10) {
			radio_pctl_reg = 0;
			pga = abs(10 - lb_gain) / 6;
			pga = limit_value(pga, 0, 15);
		} else {
			int cmp_val;
			int tmp;

			pga = 0;
			cmp_val = 0x24;
			if ((phy->rev >= 2) &&
			    (phy->radio_ver == 0x2050) &&
			    (phy->radio_rev == 8))
				cmp_val = 0x3C;
			tmp = lb_gain;
			if ((10 - lb_gain) < cmp_val)
				tmp = (10 - lb_gain);
			if (tmp < 0)
				tmp += 6;
			else
				tmp += 3;
			cmp_val /= 4;
			tmp /= 4;
			if (tmp >= cmp_val)
				radio_pctl_reg = cmp_val;
			else
				radio_pctl_reg = tmp;
		}
	}
	bcm43xx_radio_write16(dev, 0x43,
			      (bcm43xx_radio_read16(dev, 0x43)
			       & 0xFFF0) | radio_pctl_reg);
	bcm43xx_phy_set_baseband_attenuation(dev, 2);

	reg = lo_txctl_register_table(dev, &mask, NULL);
	mask = ~mask;
	bcm43xx_radio_write16(dev, reg,
			      bcm43xx_radio_read16(dev, reg)
			      & mask);

	if (has_tx_magnification(phy)) {
		int i, j;
		int feedthrough;
		int min_feedth = 0xFFFF;
		u8 tx_magn, tx_bias;

		for (i = 0; i < ARRAY_SIZE(tx_magn_values); i++) {
			tx_magn = tx_magn_values[i];
			bcm43xx_radio_write16(dev, 0x52,
					      (bcm43xx_radio_read16(dev, 0x52)
					       & 0xFF0F) | tx_magn);
			for (j = 0; j < ARRAY_SIZE(tx_bias_values); j++) {
				tx_bias = tx_bias_values[j];
				bcm43xx_radio_write16(dev, 0x52,
						      (bcm43xx_radio_read16(dev, 0x52)
						       & 0xFFF0) | tx_bias);
				feedthrough = lo_measure_feedthrough(dev, 0, pga, trsw_rx);
				if (feedthrough < min_feedth) {
					lo->tx_bias = tx_bias;
					lo->tx_magn = tx_magn;
					min_feedth = feedthrough;
				}
				if (lo->tx_bias == 0)
					break;
			}
			bcm43xx_radio_write16(dev, 0x52,
					      (bcm43xx_radio_read16(dev, 0x52)
					       & 0xFF00) | lo->tx_bias | lo->tx_magn);
		}
	} else {
		lo->tx_magn = 0;
		lo->tx_bias = 0;
		bcm43xx_radio_write16(dev, 0x52,
				      bcm43xx_radio_read16(dev, 0x52)
				      & 0xFFF0); /* TX bias == 0 */
	}
}

static void lo_read_power_vector(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 i;
	u64 tmp;
	u64 power_vector = 0;
	int rf_offset, bb_offset;
	struct bcm43xx_loctl *loctl;

	for (i = 0; i < 8; i += 2) {
		tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
					 0x310 + i);
		/* Clear the top byte. We get holes in the bitmap... */
		tmp &= 0xFF;
		power_vector |= (tmp << (i * 8));
		/* Clear the vector on the device. */
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
				    0x310 + i, 0);
	}

	if (power_vector)
		lo->power_vector = power_vector;
	power_vector = lo->power_vector;

	for (i = 0; i < 64; i++) {
		if (power_vector & ((u64)1ULL << i)) {
			/* Now figure out which bcm43xx_loctl corresponds
			 * to this bit.
			 */
			rf_offset = i / lo->rfatt_list.len;
			bb_offset = i % lo->rfatt_list.len;//FIXME?
			loctl = bcm43xx_get_lo_g_ctl(dev, &lo->rfatt_list.list[rf_offset],
						  &lo->bbatt_list.list[bb_offset]);
			/* And mark it as "used", as the device told us
			 * through the bitmap it is using it.
			 */
			loctl->used = 1;
		}
	}
}

/* 802.11/LO/GPHY/MeasuringGains */
static void lo_measure_gain_values(struct bcm43xx_wldev *dev,
				   s16 max_rx_gain,
				   int use_trsw_rx)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 tmp;

	if (max_rx_gain < 0)
		max_rx_gain = 0;

	if (has_loopback_gain(phy)) {
		int trsw_rx = 0;
		int trsw_rx_gain;

		if (use_trsw_rx) {
			trsw_rx_gain = phy->trsw_rx_gain / 2;
			if (max_rx_gain >= trsw_rx_gain) {
				trsw_rx_gain = max_rx_gain - trsw_rx_gain;
				trsw_rx = 0x20;
			}
		} else
			trsw_rx_gain = max_rx_gain;
		if (trsw_rx_gain < 9) {
			phy->lna_lod_gain = 0;
		} else {
			phy->lna_lod_gain = 1;
			trsw_rx_gain -= 8;
		}
		trsw_rx_gain = limit_value(trsw_rx_gain, 0, 0x2D);
		phy->pga_gain = trsw_rx_gain / 3;
		if (phy->pga_gain >= 5) {
			phy->pga_gain -= 5;
			phy->lna_gain = 2;
		} else
			phy->lna_gain = 0;
	} else {
		phy->lna_gain = 0;
		phy->trsw_rx_gain = 0x20;
		if (max_rx_gain >= 0x14) {
			phy->lna_lod_gain = 1;
			phy->pga_gain = 2;
		} else if (max_rx_gain >= 0x12) {
			phy->lna_lod_gain = 1;
			phy->pga_gain = 1;
		} else if (max_rx_gain >= 0xF) {
			phy->lna_lod_gain = 1;
			phy->pga_gain = 0;
		} else {
			phy->lna_lod_gain = 0;
			phy->pga_gain = 0;
		}
	}

	tmp = bcm43xx_radio_read16(dev, 0x7A);
	if (phy->lna_lod_gain == 0)
		tmp &= ~0x0008;
	else
		tmp |= 0x0008;
	bcm43xx_radio_write16(dev, 0x7A, tmp);
}

struct lo_g_saved_values {
	u8 old_channel;

	/* Core registers */
	u16 reg_3F4;
	u16 reg_3E2;

	/* PHY registers */
	u16 phy_lo_mask;
	u16 phy_extg_01;
	u16 phy_dacctl_hwpctl;
	u16 phy_dacctl;
	u16 phy_base_14;
	u16 phy_hpwr_tssictl;
	u16 phy_analogover;
	u16 phy_analogoverval;
	u16 phy_rfover;
	u16 phy_rfoverval;
	u16 phy_classctl;
	u16 phy_base_3E;
	u16 phy_crs0;
	u16 phy_pgactl;
	u16 phy_base_2A;
	u16 phy_syncctl;
	u16 phy_base_30;
	u16 phy_base_06;

	/* Radio registers */
	u16 radio_43;
	u16 radio_7A;
	u16 radio_52;
};

static void lo_measure_setup(struct bcm43xx_wldev *dev,
			     struct lo_g_saved_values *sav)
{
	struct ssb_sprom *sprom = &dev->dev->bus->sprom;
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 tmp;

	if (has_hardware_pctl(phy)) {
		sav->phy_lo_mask = bcm43xx_phy_read(dev, BCM43xx_PHY_LO_MASK);
		sav->phy_extg_01 = bcm43xx_phy_read(dev, BCM43xx_PHY_EXTG(0x01));
		sav->phy_dacctl_hwpctl = bcm43xx_phy_read(dev, BCM43xx_PHY_DACCTL);
		sav->phy_base_14 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x14));
		sav->phy_hpwr_tssictl = bcm43xx_phy_read(dev, BCM43xx_PHY_HPWR_TSSICTL);

		bcm43xx_phy_write(dev, BCM43xx_PHY_HPWR_TSSICTL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_HPWR_TSSICTL)
				  | 0x100);
		bcm43xx_phy_write(dev, BCM43xx_PHY_EXTG(0x01),
				  bcm43xx_phy_read(dev, BCM43xx_PHY_EXTG(0x01))
				  | 0x40);
		bcm43xx_phy_write(dev, BCM43xx_PHY_DACCTL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_DACCTL)
				  | 0x40);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x14),
				  bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x14))
				  | 0x200);
	}
	if (phy->type == BCM43xx_PHYTYPE_B &&
	    phy->radio_ver == 0x2050 &&
	    phy->radio_rev < 6) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x16), 0x410);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x17), 0x820);
	}
	if (!lo->rebuild && has_hardware_pctl(phy))
		lo_read_power_vector(dev);
	if (phy->rev >= 2) {
		sav->phy_analogover = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER);
		sav->phy_analogoverval = bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL);
		sav->phy_rfover = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVER);
		sav->phy_rfoverval = bcm43xx_phy_read(dev, BCM43xx_PHY_RFOVERVAL);
		sav->phy_classctl = bcm43xx_phy_read(dev, BCM43xx_PHY_CLASSCTL);
		sav->phy_base_3E = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x3E));
		sav->phy_crs0 = bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0);

		bcm43xx_phy_write(dev, BCM43xx_PHY_CLASSCTL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_CLASSCTL)
				  & 0xFFFC);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_CRS0)
				  & 0x7FFF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVER)
				  | 0x0003);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL,
				  bcm43xx_phy_read(dev, BCM43xx_PHY_ANALOGOVERVAL)
				  & 0xFFFC);
		if (phy->type == BCM43xx_PHYTYPE_G) {
			if ((phy->rev >= 7) &&
			    (sprom->r1.boardflags_lo & BCM43xx_BFL_EXTLNA)) {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, 0x933);
			} else {
				bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, 0x133);
			}
		} else {
			bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, 0);
		}
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x3E), 0);
	}
	sav->reg_3F4 = bcm43xx_read16(dev, 0x3F4);
	sav->reg_3E2 = bcm43xx_read16(dev, 0x3E2);
	sav->radio_43 = bcm43xx_radio_read16(dev, 0x43);
	sav->radio_7A = bcm43xx_radio_read16(dev, 0x7A);
	sav->phy_pgactl = bcm43xx_phy_read(dev, BCM43xx_PHY_PGACTL);
	sav->phy_base_2A = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x2A));
	sav->phy_syncctl = bcm43xx_phy_read(dev, BCM43xx_PHY_SYNCCTL);
	sav->phy_dacctl = bcm43xx_phy_read(dev, BCM43xx_PHY_DACCTL);

	if (!has_tx_magnification(phy)) {
		sav->radio_52 = bcm43xx_radio_read16(dev, 0x52);
		sav->radio_52 &= 0x00F0;
	}
	if (phy->type == BCM43xx_PHYTYPE_B) {
		sav->phy_base_30 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x30));
		sav->phy_base_06 = bcm43xx_phy_read(dev, BCM43xx_PHY_BASE(0x06));
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x30), 0x00FF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x06), 0x3F3F);
	} else {
		bcm43xx_write16(dev, 0x3E2,
				bcm43xx_read16(dev, 0x3E2)
				| 0x8000);
	}
	bcm43xx_write16(dev, 0x3F4,
			bcm43xx_read16(dev, 0x3F4)
			& 0xF000);

	tmp = (phy->type == BCM43xx_PHYTYPE_G) ? BCM43xx_PHY_LO_MASK : BCM43xx_PHY_BASE(0x2E);
	bcm43xx_phy_write(dev, tmp, 0x007F);

	tmp = sav->phy_syncctl;
	bcm43xx_phy_write(dev, BCM43xx_PHY_SYNCCTL, tmp & 0xFF7F);
	tmp = sav->radio_7A;
	bcm43xx_radio_write16(dev, 0x007A, tmp & 0xFFF0);

	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2A), 0x8A3);
	if (phy->type == BCM43xx_PHYTYPE_G ||
	    (phy->type == BCM43xx_PHYTYPE_B &&
	     phy->radio_ver == 0x2050 &&
	     phy->radio_rev >= 6)) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B), 0x1003);
	} else
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2B), 0x0802);
	if (phy->rev >= 2)
		bcm43xx_dummy_transmission(dev);
	bcm43xx_radio_selectchannel(dev, 6, 0);
	bcm43xx_radio_read16(dev, 0x51); /* dummy read */
	if (phy->type == BCM43xx_PHYTYPE_G)
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2F), 0);
	if (lo->rebuild)
		lo_measure_txctl_values(dev);
	if (phy->type == BCM43xx_PHYTYPE_G && phy->rev >= 3) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0xC078);
	} else {
		if (phy->type == BCM43xx_PHYTYPE_B)
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2E), 0x8078);
		else
			bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, 0x8078);
	}
}

static void lo_measure_restore(struct bcm43xx_wldev *dev,
			       struct lo_g_saved_values *sav)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	u16 tmp;

	if (phy->rev >= 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, 0xE300);
		tmp = (phy->pga_gain << 8);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, tmp | 0xA0);
		udelay(5);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, tmp | 0xA2);
		udelay(2);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, tmp | 0xA3);
	} else {
		tmp = (phy->pga_gain | 0xEFA0);
		bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, tmp);
	}
	if (has_hardware_pctl(phy)) {
		bcm43xx_gphy_dc_lt_init(dev);
	} else {
		if (lo->rebuild)
			bcm43xx_lo_g_adjust_to(dev, 3, 2, 0);
		else
			bcm43xx_lo_g_adjust(dev);
	}
	if (phy->type == BCM43xx_PHYTYPE_G) {
		if (phy->rev >= 3)
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2E), 0xC078);
		else
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2E), 0x8078);
		if (phy->rev >= 2)
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2F), 0x0202);
		else
			bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2F), 0x0101);
	}
	bcm43xx_write16(dev, 0x3F4, sav->reg_3F4);
	bcm43xx_phy_write(dev, BCM43xx_PHY_PGACTL, sav->phy_pgactl);
	bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x2A), sav->phy_base_2A);
	bcm43xx_phy_write(dev, BCM43xx_PHY_SYNCCTL, sav->phy_syncctl);
	bcm43xx_phy_write(dev, BCM43xx_PHY_DACCTL, sav->phy_dacctl);
	bcm43xx_radio_write16(dev, 0x43, sav->radio_43);
	bcm43xx_radio_write16(dev, 0x7A, sav->radio_7A);
	if (!has_tx_magnification(phy)) {
		tmp = sav->radio_52;
		bcm43xx_radio_write16(dev, 0x52,
				      (bcm43xx_radio_read16(dev, 0x52)
				       & 0xFF0F) | tmp);
	}
	bcm43xx_write16(dev, 0x3E2, sav->reg_3E2);
	if (phy->type == BCM43xx_PHYTYPE_B &&
	    phy->radio_ver == 0x2050 &&
	    phy->radio_rev <= 5) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x30), sav->phy_base_30);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x06), sav->phy_base_06);
	}
	if (phy->rev >= 2) {
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVER, sav->phy_analogover);
		bcm43xx_phy_write(dev, BCM43xx_PHY_ANALOGOVERVAL, sav->phy_analogoverval);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CLASSCTL, sav->phy_classctl);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVER, sav->phy_rfover);
		bcm43xx_phy_write(dev, BCM43xx_PHY_RFOVERVAL, sav->phy_rfoverval);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x3E), sav->phy_base_3E);
		bcm43xx_phy_write(dev, BCM43xx_PHY_CRS0, sav->phy_crs0);
	}
	if (has_hardware_pctl(phy)) {
		tmp = (sav->phy_lo_mask & 0xBFFF);
		bcm43xx_phy_write(dev, BCM43xx_PHY_LO_MASK, tmp);
		bcm43xx_phy_write(dev, BCM43xx_PHY_EXTG(0x01), sav->phy_extg_01);
		bcm43xx_phy_write(dev, BCM43xx_PHY_DACCTL, sav->phy_dacctl_hwpctl);
		bcm43xx_phy_write(dev, BCM43xx_PHY_BASE(0x14), sav->phy_base_14);
		bcm43xx_phy_write(dev, BCM43xx_PHY_HPWR_TSSICTL, sav->phy_hpwr_tssictl);
	}
	bcm43xx_radio_selectchannel(dev, sav->old_channel, 1);
}

struct bcm43xx_lo_g_statemachine {
	int current_state;
	int nr_measured;
	int state_val_multiplier;
	u16 lowest_feedth;
	struct bcm43xx_loctl min_loctl;
};

/* Loop over each possible value in this state. */
static int lo_probe_possible_loctls(struct bcm43xx_wldev *dev,
				    struct bcm43xx_loctl *probe_loctl,
				    struct bcm43xx_lo_g_statemachine *d)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	struct bcm43xx_loctl test_loctl;
	struct bcm43xx_loctl orig_loctl;
	struct bcm43xx_loctl prev_loctl = {
		.i = -100,
		.q = -100,
	};
	int i;
	int begin, end;
	int found_lower = 0;
	u16 feedth;

	static const struct bcm43xx_loctl modifiers[] = {
		{ .i =  1,  .q =  1, },
		{ .i =  1,  .q =  0, },
		{ .i =  1,  .q = -1, },
		{ .i =  0,  .q = -1, },
		{ .i = -1,  .q = -1, },
		{ .i = -1,  .q =  0, },
		{ .i = -1,  .q =  1, },
		{ .i =  0,  .q =  1, },
	};

	if (d->current_state == 0) {
		begin = 1;
		end = 8;
	} else if (d->current_state % 2 == 0) {
		begin = d->current_state - 1;
		end = d->current_state + 1;
	} else {
		begin = d->current_state - 2;
		end = d->current_state + 2;
	}
	if (begin < 1)
		begin += 8;
	if (end > 8)
		end -= 8;

	memcpy(&orig_loctl, probe_loctl, sizeof(struct bcm43xx_loctl));
	i = begin;
	d->current_state = i;
	while (1) {
		assert(i >= 1 && i <= 8);
		memcpy(&test_loctl, &orig_loctl, sizeof(struct bcm43xx_loctl));
		test_loctl.i += modifiers[i - 1].i * d->state_val_multiplier;
		test_loctl.q += modifiers[i - 1].q * d->state_val_multiplier;
		if ((test_loctl.i != prev_loctl.i ||
		     test_loctl.q != prev_loctl.q) &&
		    (abs(test_loctl.i) <= 16 &&
		     abs(test_loctl.q) <= 16)) {
			bcm43xx_lo_write(dev, &test_loctl);
			feedth = lo_measure_feedthrough(dev, phy->lna_gain,
							phy->pga_gain,
							phy->trsw_rx_gain);
			if (feedth < d->lowest_feedth) {
				memcpy(probe_loctl, &test_loctl, sizeof(struct bcm43xx_loctl));
				found_lower = 1;
				d->lowest_feedth = feedth;
				if ((d->nr_measured < 2) &&
				    (!has_loopback_gain(phy) || lo->rebuild))
					break;
			}
		}
		memcpy(&prev_loctl, &test_loctl, sizeof(prev_loctl));
		if (i == end)
			break;
		if (i == 8)
			i = 1;
		else
			i++;
		d->current_state = i;
	}

	return found_lower;
}

static void lo_probe_loctls_statemachine(struct bcm43xx_wldev *dev,
					 struct bcm43xx_loctl *loctl,
					 int *max_rx_gain)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	struct bcm43xx_lo_g_statemachine d;
	u16 feedth;
	int found_lower;
	struct bcm43xx_loctl probe_loctl;
	int max_repeat = 1, repeat_cnt = 0;

	d.nr_measured = 0;
	d.state_val_multiplier = 1;
	if (has_loopback_gain(phy) && !lo->rebuild)
		d.state_val_multiplier = 3;

	memcpy(&d.min_loctl, loctl, sizeof(struct bcm43xx_loctl));
	if (has_loopback_gain(phy) && lo->rebuild)
		max_repeat = 4;
	do {
		bcm43xx_lo_write(dev, &d.min_loctl);
		feedth = lo_measure_feedthrough(dev, phy->lna_gain,
						phy->pga_gain,
						phy->trsw_rx_gain);
		if (!lo->rebuild && feedth < 0x258) {
			if (feedth >= 0x12C)
				*max_rx_gain += 6;
			else
				*max_rx_gain += 3;
			feedth = lo_measure_feedthrough(dev, phy->lna_gain,
							phy->pga_gain,
							phy->trsw_rx_gain);
		}
		d.lowest_feedth = feedth;

		d.current_state = 0;
		do {
			assert(d.current_state >= 0 && d.current_state <= 8);
			memcpy(&probe_loctl, &d.min_loctl, sizeof(struct bcm43xx_loctl));
			found_lower = lo_probe_possible_loctls(dev, &probe_loctl, &d);
			if (!found_lower)
				break;
			if ((probe_loctl.i == d.min_loctl.i) &&
			    (probe_loctl.q == d.min_loctl.q))
				break;
			memcpy(&d.min_loctl, &probe_loctl, sizeof(struct bcm43xx_loctl));
			d.nr_measured++;
		} while (d.nr_measured < 24);
		memcpy(loctl, &d.min_loctl, sizeof(struct bcm43xx_loctl));

		if (has_loopback_gain(phy)) {
			if (d.lowest_feedth > 0x1194)
				*max_rx_gain -= 6;
			else if (d.lowest_feedth < 0x5DC)
				*max_rx_gain += 3;
			if (repeat_cnt == 0) {
				if (d.lowest_feedth <= 0x5DC) {
					d.state_val_multiplier = 1;
					repeat_cnt++;
				} else
					d.state_val_multiplier = 2;
			} else if (repeat_cnt == 2)
				d.state_val_multiplier = 1;
		}
		lo_measure_gain_values(dev, *max_rx_gain, has_loopback_gain(phy));
	} while (++repeat_cnt < max_repeat);
}

static void lo_measure(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;
	struct bcm43xx_loctl loctl = {
		.i	= 0,
		.q	= 0,
	};
	struct bcm43xx_loctl *ploctl;
	int max_rx_gain;
	int rfidx, bbidx;

	/* Values from the "TXCTL Register and Value Table" */
	u16 txctl_reg;
	u16 txctl_value;
	u16 pad_mix_gain;

	txctl_reg = lo_txctl_register_table(dev, &txctl_value, &pad_mix_gain);

	for (rfidx = 0; rfidx < lo->rfatt_list.len; rfidx++) {

		bcm43xx_radio_write16(dev, 0x43,
				      (bcm43xx_radio_read16(dev, 0x43)
				       & 0xFFF0) | lo->rfatt_list.list[rfidx].att);
		bcm43xx_radio_write16(dev, txctl_reg,
				      (bcm43xx_radio_read16(dev, txctl_reg)
				       & ~txctl_value)
				      | (lo->rfatt_list.list[rfidx].with_padmix ? txctl_value : 0));

		for (bbidx = 0; bbidx < lo->bbatt_list.len; bbidx++) {
			if (lo->rebuild) {
				ploctl = bcm43xx_get_lo_g_ctl_nopadmix(dev,
							&lo->rfatt_list.list[rfidx],
							&lo->bbatt_list.list[bbidx]);
			} else {
				ploctl = bcm43xx_get_lo_g_ctl(dev, &lo->rfatt_list.list[rfidx],
							   &lo->bbatt_list.list[bbidx]);
				if (!ploctl->used)
					continue;
			}
			memcpy(&loctl, ploctl, sizeof(loctl));

			max_rx_gain = lo->rfatt_list.list[rfidx].att * 2;
			max_rx_gain += lo->bbatt_list.list[bbidx].att / 2;
			if (lo->rfatt_list.list[rfidx].with_padmix)
				max_rx_gain -= pad_mix_gain;
			if (has_loopback_gain(phy))
				max_rx_gain += phy->max_lb_gain;
			lo_measure_gain_values(dev, max_rx_gain, has_loopback_gain(phy));

			bcm43xx_phy_set_baseband_attenuation(dev, lo->bbatt_list.list[bbidx].att);
			lo_probe_loctls_statemachine(dev, &loctl, &max_rx_gain);
			if (phy->type == BCM43xx_PHYTYPE_B) {
				loctl.i++;
				loctl.q++;
			}
			memcpy(ploctl, &loctl, sizeof(loctl));
		}
	}
}

#if BCM43xx_DEBUG
static void do_validate_loctl(struct bcm43xx_wldev *dev,
			      struct bcm43xx_loctl *control)
{
	const int is_initializing = (bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZING);

	if (unlikely(abs(control->i) > 16 ||
		     abs(control->q) > 16 ||
		     (is_initializing && control->used))) {
		printk(KERN_ERR PFX "ERROR: LO control pair validation failed "
				    "(first: %d, second: %d, used %u)\n",
		       control->i, control->q, control->used);
	}
}
static void validate_all_loctls(struct bcm43xx_wldev *dev)
{
	bcm43xx_call_for_each_loctl(dev, do_validate_loctl);
}
#else /* BCM43xx_DEBUG */
static inline void validate_all_loctls(struct bcm43xx_wldev *dev) { }
#endif /* BCM43xx_DEBUG */

void bcm43xx_lo_g_measure(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct lo_g_saved_values sav;

	assert(phy->type == BCM43xx_PHYTYPE_B ||
	       phy->type == BCM43xx_PHYTYPE_G);

	sav.old_channel = phy->channel;
	lo_measure_setup(dev, &sav);
	lo_measure(dev);
	lo_measure_restore(dev, &sav);

	validate_all_loctls(dev);

	phy->lo_control->lo_measured = 1;
	phy->lo_control->rebuild = 0;
}

void bcm43xx_lo_g_adjust(struct bcm43xx_wldev *dev)
{
	bcm43xx_lo_write(dev, bcm43xx_lo_g_ctl_current(dev));
}

static inline void fixup_rfatt_for_txcontrol(struct bcm43xx_rfatt *rf,
					     u8 tx_control)
{
	if (tx_control & BCM43xx_TXCTL_TXMIX) {
		if (rf->att < 5)
			rf->att = 4;
	}
}

void bcm43xx_lo_g_adjust_to(struct bcm43xx_wldev *dev,
			  u16 rfatt, u16 bbatt, u16 tx_control)
{
	struct bcm43xx_rfatt rf;
	struct bcm43xx_bbatt bb;
	struct bcm43xx_loctl *loctl;

	memset(&rf, 0, sizeof(rf));
	memset(&bb, 0, sizeof(bb));
	rf.att = rfatt;
	bb.att = bbatt;
	fixup_rfatt_for_txcontrol(&rf, tx_control);
	loctl = bcm43xx_get_lo_g_ctl(dev, &rf, &bb);
	bcm43xx_lo_write(dev, loctl);
}

struct bcm43xx_loctl * bcm43xx_lo_g_ctl_current(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_rfatt rf;

	memcpy(&rf, &phy->rfatt, sizeof(rf));
	fixup_rfatt_for_txcontrol(&rf, phy->tx_control);

	return bcm43xx_get_lo_g_ctl(dev, &rf, &phy->bbatt);
}

static void do_mark_unused(struct bcm43xx_wldev *dev,
			   struct bcm43xx_loctl *control)
{
	control->used = 0;
}

void bcm43xx_lo_g_ctl_mark_all_unused(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	struct bcm43xx_txpower_lo_control *lo = phy->lo_control;

	bcm43xx_call_for_each_loctl(dev, do_mark_unused);
	lo->rebuild = 1;
}

void bcm43xx_lo_g_ctl_mark_cur_used(struct bcm43xx_wldev *dev)
{
	bcm43xx_lo_g_ctl_current(dev)->used = 1;
}
