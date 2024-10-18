// SPDX-License-Identifier:     GPL-2.0+
/*
 * Common part for Airoha AN8855 gigabit switch
 *
 * Copyright (C) 2023 Airoha Inc. All Rights Reserved.
 *
 * Author: Min Yao <min.yao@airoha.com>
 */

#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/kernel.h>
#include <net/dsa.h>
#include "an8855.h"
#include "an8855_phy.h"

#define AN8855_EFUSE_DATA0	0x1000a500

const u8 dsa_r50ohm_table[] = {
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 126, 122, 117,
	112, 109, 104, 101,  97,  94,  90,  88,  84,  80,
	78,  74,  72,  68,  66,  64,  61,  58,  56,  53,
	51,  48,  47,  44,  42,  40,  38,  36,  34,  32,
	31,  28,  27,  24,  24,  22,  20,  18,  16,  16,
	14,  12,  11,   9
};

static u8 shift_check(u8 base)
{
	u8 i;
	u32 sz = sizeof(dsa_r50ohm_table)/sizeof(u8);

	for (i = 0; i < sz; ++i)
		if (dsa_r50ohm_table[i] == base)
			break;

	if (i < 8 || i >= sz)
		return 25; /* index of 94 */

	return (i - 8);
}

static u8 get_shift_val(u8 idx)
{
	return dsa_r50ohm_table[idx];
}

static void
an8855_switch_phy_write(struct dsa_switch *ds, u32 port_num,
				    u32 reg_addr, u32 write_data)
{
	struct an8855_priv *priv = ds->priv;

	priv->info->phy_write(ds, port_num, reg_addr, write_data);
}

static u32
an8855_switch_phy_read(struct dsa_switch *ds, u32 port_num,
				  u32 reg_addr)
{
	struct an8855_priv *priv = ds->priv;

	return priv->info->phy_read(ds, port_num, reg_addr);
}

static void
an8855_phy_setting(struct dsa_switch *ds)
{
	struct an8855_priv *priv = ds->priv;
	int i, j;
	u8 shift_sel = 0, rsel_tx_a = 0, rsel_tx_b = 0;
	u8 rsel_tx_c = 0, rsel_tx_d = 0;
	u16 cl45_data = 0;
	u32 val;

	/* Release power down */
	an8855_write(priv, RG_GPHY_AFE_PWD, 0x0);
	for (i = 0; i < AN8855_NUM_PHYS; i++) {
		/* Enable HW auto downshift */
		an8855_switch_phy_write(ds, i, 0x1f, 0x1);
		val = an8855_switch_phy_read(ds, i, PHY_EXT_REG_14);
		val |= PHY_EN_DOWN_SHFIT;
		an8855_switch_phy_write(ds, i, PHY_EXT_REG_14, val);
		an8855_switch_phy_write(ds, i, 0x1f, 0x0);

		/* Enable Asymmetric Pause Capability */
		val = an8855_switch_phy_read(ds, i, MII_ADVERTISE);
		val |= ADVERTISE_PAUSE_ASYM;
		an8855_switch_phy_write(ds, i, MII_ADVERTISE, val);
	}

	if (priv->extSurge) {
		for (i = 0; i < AN8855_NUM_PHYS; i++) {
			/* Read data */
			for (j = 0; j < AN8855_WORD_SIZE; j++) {
				val = an8855_read(priv, AN8855_EFUSE_DATA0 +
					(AN8855_WORD_SIZE * (3 + j + (4 * i))));

				shift_sel = shift_check((val & 0x7f000000) >> 24);
				switch (j) {
				case 0:
					rsel_tx_a = get_shift_val(shift_sel);
					break;
				case 1:
					rsel_tx_b = get_shift_val(shift_sel);
					break;
				case 2:
					rsel_tx_c = get_shift_val(shift_sel);
					break;
				case 3:
					rsel_tx_d = get_shift_val(shift_sel);
					break;
				default:
					continue;
				}
			}
			cl45_data = an8855_phy_cl45_read(priv, i, PHY_DEV1E, 0x174);
			cl45_data &= ~(0x7f7f);
			cl45_data |= (rsel_tx_a << 8);
			cl45_data |= rsel_tx_b;
			an8855_phy_cl45_write(priv, i, PHY_DEV1E, 0x174, cl45_data);
			cl45_data = an8855_phy_cl45_read(priv, i, PHY_DEV1E, 0x175);
			cl45_data &= ~(0x7f7f);
			cl45_data |= (rsel_tx_c << 8);
			cl45_data |= rsel_tx_d;
			an8855_phy_cl45_write(priv, i, PHY_DEV1E, 0x175, cl45_data);
		}
	}
}

static void
an8855_eee_setting(struct dsa_switch *ds, u32 port)
{
	struct an8855_priv *priv = ds->priv;

	/* Disable EEE */
	an8855_phy_cl45_write(priv, port, PHY_DEV07, PHY_DEV07_REG_03C, 0);
}

int
an8855_phy_setup(struct dsa_switch *ds)
{
	int ret = 0;
	int i;

	an8855_phy_setting(ds);

	for (i = 0; i < AN8855_NUM_PHYS; i++)
		an8855_eee_setting(ds, i);

	return ret;
}
