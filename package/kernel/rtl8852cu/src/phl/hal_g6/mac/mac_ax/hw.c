/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#include "hw.h"
#include "coex.h"
#include "twt.h"
#include "fwofld.h"
#include "mac_priv.h"
#include "trxcfg.h"
#include "common.h"

static struct mac_ax_host_rpr_cfg rpr_cfg_poh = {
	121, /* agg */
	255, /* tmr */
	0, /* agg_def */
	0, /* tmr_def */
	0, /* rsvd */
	MAC_AX_FUNC_EN, /* txok_en */
	MAC_AX_FUNC_EN, /* rty_lmt_en */
	MAC_AX_FUNC_EN, /* lft_drop_en */
	MAC_AX_FUNC_EN /* macid_drop_en */
};

static struct mac_ax_host_rpr_cfg rpr_cfg_stf = {
	121, /* agg */
	255, /* tmr */
	0, /* agg_def */
	0, /* tmr_def */
	0, /* rsvd */
	MAC_AX_FUNC_DIS, /* txok_en */
	MAC_AX_FUNC_DIS, /* rty_lmt_en */
	MAC_AX_FUNC_DIS, /* lft_drop_en */
	MAC_AX_FUNC_DIS /* macid_drop_en */
};

static struct mac_ax_drv_wdt_ctrl wdt_ctrl_def = {
	MAC_AX_PCIE_DISABLE,
	MAC_AX_PCIE_ENABLE
};

static void get_delay_tx_cfg(struct mac_ax_adapter *adapter,
			     struct mac_ax_delay_tx_cfg *cfg);
static void set_delay_tx_cfg(struct mac_ax_adapter *adapter,
			     struct mac_ax_delay_tx_cfg *cfg);

struct mac_ax_hw_info *mac_get_hw_info(struct mac_ax_adapter *adapter)
{
	return adapter->hw_info->done ? adapter->hw_info : NULL;
}

u32 get_block_tx_sel_msk(enum mac_ax_block_tx_sel src, u32 *msk)
{
	switch (src) {
	case MAC_AX_CCA:
		*msk = B_AX_CCA_EN;
		break;
	case MAC_AX_SEC20_CCA:
		*msk = B_AX_SEC20_EN;
		break;
	case MAC_AX_SEC40_CCA:
		*msk = B_AX_SEC40_EN;
		break;
	case MAC_AX_SEC80_CCA:
		*msk = B_AX_SEC80_EN;
		break;
	case MAC_AX_EDCCA:
		*msk = B_AX_EDCCA_EN;
		break;
	case MAC_AX_BTCCA:
		*msk = B_AX_BTCCA_EN;
		break;
	case MAC_AX_TX_NAV:
		*msk = B_AX_TX_NAV_EN;
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 cfg_block_tx(struct mac_ax_adapter *adapter,
		 enum mac_ax_block_tx_sel src, u8 band, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val, msk, ret;
	u32 reg = band == 0 ? R_AX_CCA_CFG_0 : R_AX_CCA_CFG_0_C1;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("%s: CMAC%d is NOT enabled\n", __func__, band);
		return ret;
	}

	ret = get_block_tx_sel_msk(src, &msk);
	if (ret) {
		PLTFM_MSG_ERR("%s: %d is NOT supported\n", __func__, src);
		return ret;
	}

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)reg, msk,
				     en ? 1 : 0, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);

		return ret;
	}
#endif
	val = MAC_REG_R32(reg);

	if (en)
		val = val | msk;
	else
		val = val & ~msk;
	MAC_REG_W32(reg, val);

	return MACSUCCESS;
}

u32 get_block_tx(struct mac_ax_adapter *adapter,
		 enum mac_ax_block_tx_sel src, u8 band, u8 *en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val, msk, ret;
	u32 reg = band == 0 ? R_AX_CCA_CFG_0 : R_AX_CCA_CFG_0_C1;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("%s: CMAC%d is NOT enabled", __func__, band);
		return ret;
	}

	val = MAC_REG_R32(reg);

	ret = get_block_tx_sel_msk(src, &msk);
	if (ret) {
		PLTFM_MSG_ERR("%s: %d is NOT supported\n", __func__, src);
		return ret;
	}

	*en = !!(val & msk);

	return MACSUCCESS;
}

u32 set_enable_bb_rf(struct mac_ax_adapter *adapter, u8 enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 value32, ret;
	u8 value8;
	u8 wl_rfc_s0 = 0;
	u8 wl_rfc_s1 = 0;

	if (enable == 1) {
		value8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
		value8 |= B_AX_FEN_BBRSTB | B_AX_FEN_BB_GLB_RSTN;
		MAC_REG_W8(R_AX_SYS_FUNC_EN, value8);
#if MAC_AX_8852A_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 |= B_AX_WLRF1_CTRL_7 | B_AX_WLRF1_CTRL_1 |
				   B_AX_WLRF_CTRL_7 | B_AX_WLRF_CTRL_1;
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			value8 = PHYREG_SET_ALL_CYCLE;
			MAC_REG_W8(R_AX_PHYREG_SET, value8);
			ret = MACSUCCESS;
		}
#endif
#if MAC_AX_8852B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
			/* 0x200[18:17] = 2'b01 */
			value32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
			value32 = SET_CLR_WORD(value32, 0x1, B_AX_REG_ZCDC_H);
			MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, value32);

			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			wl_rfc_s0 = 0xC7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, wl_rfc_s0,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			wl_rfc_s1 = 0xC7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, wl_rfc_s1,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			#if defined(CONFIG_RTL8852BP)
				value8 = 0xd;
				ret = mac_write_xtal_si(adapter, XTAL3, value8,
							FULL_BIT_MASK);
				if (ret) {
					PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
					return ret;
				}
			#endif	//#if defined(CONFIG_RTL8852BP)

			value8 = PHYREG_SET_XYN_CYCLE;
			MAC_REG_W8(R_AX_PHYREG_SET, value8);
		}
#endif
#if MAC_AX_8852C_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
			/* 0x200[18:17] = 2'b01 */
			value32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
			value32 = SET_CLR_WORD(value32, 0x1, B_AX_REG_ZCDC_H);
			MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, value32);

			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			// for ADC PWR setting
			value32 = MAC_REG_R32(R_AX_AFE_OFF_CTRL1);
			value32 = (value32 & ~LDO2PW_LDO_VSEL);
			value32 |= SET_WORD(0x1, B_AX_S0_LDO_VSEL_F) |
				   SET_WORD(0x1, B_AX_S1_LDO_VSEL_F);
			MAC_REG_W32(R_AX_AFE_OFF_CTRL1, value32);

			value8 = 0x7;
			ret = mac_write_xtal_si(adapter, XTAL0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0x6c;
			ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xd;
			ret = mac_write_xtal_si(adapter, XTAL3, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
#endif
#if MAC_AX_8192XB_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			value8 = 0x7;
			ret = mac_write_xtal_si(adapter, XTAL0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0x6c;
			ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xf;
			ret = mac_write_xtal_si(adapter, XTAL3, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
#endif
#if MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			/* 0x200[18:17] = 2'b01 */
			value32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
			value32 = SET_CLR_WORD(value32, 0x1, B_AX_REG_ZCDC_H);
			MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, value32);

			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			wl_rfc_s0 = 0xC7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, wl_rfc_s0,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			wl_rfc_s1 = 0xC7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, wl_rfc_s1,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = PHYREG_SET_XYN_CYCLE;
			MAC_REG_W8(R_AX_PHYREG_SET, value8);
		}
#endif
#if MAC_AX_8851E_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
			/* 0x200[18:17] = 2'b01 */
			value32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
			value32 = SET_CLR_WORD(value32, 0x1, B_AX_REG_ZCDC_H);
			MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, value32);

			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			// for ADC PWR setting
			value32 = MAC_REG_R32(R_AX_AFE_OFF_CTRL1);
			value32 = (value32 & ~LDO2PW_LDO_VSEL);
			value32 |= SET_WORD(0x1, B_AX_S0_LDO_VSEL_F) |
				   SET_WORD(0x1, B_AX_S1_LDO_VSEL_F);
			MAC_REG_W32(R_AX_AFE_OFF_CTRL1, value32);

			value8 = 0x7;
			ret = mac_write_xtal_si(adapter, XTAL0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0x6c;
			ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xd;
			ret = mac_write_xtal_si(adapter, XTAL3, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
#endif
#if MAC_AX_8852C_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			/* 0x200[18:17] = 2'b01 */
			value32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
			value32 = SET_CLR_WORD(value32, 0x1, B_AX_REG_ZCDC_H);
			MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, value32);

			/* RDC KS/BB suggest : write 1 then write 0 then write 1 */
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 | B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);

			// for ADC PWR setting
			value32 = MAC_REG_R32(R_AX_AFE_OFF_CTRL1);
			value32 = (value32 & ~LDO2PW_LDO_VSEL);
			value32 |= SET_WORD(0x1, B_AX_S0_LDO_VSEL_F) |
				   SET_WORD(0x1, B_AX_S1_LDO_VSEL_F);
			MAC_REG_W32(R_AX_AFE_OFF_CTRL1, value32);

			value8 = 0x7;
			ret = mac_write_xtal_si(adapter, XTAL0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0x6c;
			ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xc7;
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			value8 = 0xd;
			ret = mac_write_xtal_si(adapter, XTAL3, value8,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
#endif
		adapter->sm.bb0_func = MAC_AX_FUNC_ON;
	} else {
		adapter->sm.bb0_func = MAC_AX_FUNC_OFF;

		if (!is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 = (value32 & ~B_AX_AFC_AFEDIG);
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
		}

		value8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
		value8 &= (~(B_AX_FEN_BBRSTB | B_AX_FEN_BB_GLB_RSTN));
		MAC_REG_W8(R_AX_SYS_FUNC_EN, value8);
#if MAC_AX_8852A_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			value32 = MAC_REG_R32(R_AX_WLRF_CTRL);
			value32 &= (~(B_AX_WLRF1_CTRL_7 | B_AX_WLRF1_CTRL_1 |
				      B_AX_WLRF_CTRL_7 | B_AX_WLRF_CTRL_1));
			MAC_REG_W32(R_AX_WLRF_CTRL, value32);
		}
#endif
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			ret = mac_read_xtal_si(adapter, XTAL_SI_WL_RFC_S0, &wl_rfc_s0);
			if (ret) {
				PLTFM_MSG_ERR("Read XTAL_SI fail!\n");
				return ret;
			}
			wl_rfc_s0 = (wl_rfc_s0 & 0xF8);
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, wl_rfc_s0,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}

			ret = mac_read_xtal_si(adapter, XTAL_SI_WL_RFC_S1, &wl_rfc_s1);
			if (ret) {
				PLTFM_MSG_ERR("Read XTAL_SI fail!\n");
				return ret;
			}
			wl_rfc_s1 = (wl_rfc_s1 & 0xF8);
			ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, wl_rfc_s1,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			return MACSUCCESS;
		}
#endif
	}

	return MACSUCCESS;
}

static u32 set_append_fcs(struct mac_ax_adapter *adapter, u8 enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	value8 = MAC_REG_R8(R_AX_MPDU_PROC);
	value8 = enable == 1 ? value8 | B_AX_APPEND_FCS :
			value8 & ~B_AX_APPEND_FCS;
	MAC_REG_W8(R_AX_MPDU_PROC, value8);

	return MACSUCCESS;
}

static u32 set_accept_icverr(struct mac_ax_adapter *adapter, u8 enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	value8 = MAC_REG_R8(R_AX_MPDU_PROC);
	value8 = enable == 1 ? (value8 | B_AX_A_ICV_ERR) :
			(value8 & ~B_AX_A_ICV_ERR);
	MAC_REG_W8(R_AX_MPDU_PROC, value8);

	return MACSUCCESS;
}

u32 set_gt3_timer(struct mac_ax_adapter *adapter,
		  struct mac_ax_gt3_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = (cfg->count_en ? B_AX_GT_COUNT_EN : 0) |
		  (cfg->mode ? B_AX_GT_MODE : 0) |
		  (cfg->gt3_en ? B_AX_GT_EN : 0) |
		  (cfg->sort_en ? B_AX_GT_SORT_EN : 0) |
		  SET_WORD(cfg->timeout, B_AX_GT_DATA);

	switch (adapter->hw_info->chip_id) {
	case MAC_AX_CHIP_ID_8852A:
	case MAC_AX_CHIP_ID_8852B:
	case MAC_AX_CHIP_ID_8851B:
		MAC_REG_W32(R_AX_GT3_CTRL, val32);
		break;
	default:
		MAC_REG_W32(R_AX_GT3_CTRL_V1, val32);
	}

	return MACSUCCESS;
}

u32 set_cctl_rty_limit(struct mac_ax_adapter *adapter,
		       struct mac_ax_cctl_rty_lmt_cfg *cfg)
{
#define DFLT_DATA_RTY_LIMIT 32
#define DFLT_RTS_RTY_LIMIT 15
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct rtw_hal_mac_ax_cctl_info info;
	struct rtw_hal_mac_ax_cctl_info mask;

	u32 data_rty, rts_rty;

	PLTFM_MEMSET(&mask, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));
	PLTFM_MEMSET(&info, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));

	data_rty = cfg->data_lmt_val == 0 ?
		DFLT_DATA_RTY_LIMIT : cfg->data_lmt_val;
	rts_rty = cfg->rts_lmt_val == 0 ?
		DFLT_RTS_RTY_LIMIT : cfg->rts_lmt_val;
	info.data_txcnt_lmt_sel = cfg->data_lmt_sel;
	info.data_tx_cnt_lmt = data_rty;
	info.rts_txcnt_lmt_sel = cfg->rts_lmt_sel;
	info.rts_txcnt_lmt = rts_rty;

	mask.data_txcnt_lmt_sel = TXCNT_LMT_MSK;
	mask.data_tx_cnt_lmt = FWCMD_H2C_CCTRL_DATA_TX_CNT_LMT_MSK;
	mask.rts_txcnt_lmt_sel = TXCNT_LMT_MSK;
	mask.rts_txcnt_lmt = FWCMD_H2C_CCTRL_RTS_TXCNT_LMT_MSK;

	mops->upd_cctl_info(adapter, &info, &mask, cfg->macid, TBL_WRITE_OP);

	return MACSUCCESS;
}

u32 set_data_rty_limit(struct mac_ax_adapter *adapter,
		       struct mac_ax_rty_lmt *rty)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	struct mac_role_tbl *role;
	u32 offset_l, offset_s;
	u8 band = HW_BAND_0;

	if (rty->macid != MACID_NONE) {
		role = mac_role_srch(adapter, rty->macid);
		if (!role) {
			PLTFM_MSG_ERR("%s: The MACID%d does not exist\n",
				      __func__, rty->macid);
			return MACNOITEM;
		}

		if (role->info.c_info.data_txcnt_lmt_sel) {
			PLTFM_MSG_ERR("%s: MACID%d follow CMAC_TBL setting\n",
				      __func__, rty->macid);
		}

		band = role->info.wmm < 2 ? 0 : 1;
		offset_l = band == 0 ? R_AX_TXCNT + 2 : R_AX_TXCNT_C1 + 2;
		offset_s = band == 0 ? R_AX_TXCNT + 3 : R_AX_TXCNT_C1 + 3;
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret == MACSUCCESS) {
			MAC_REG_W8(offset_l, rty->tx_cnt);
			MAC_REG_W8(offset_s, rty->short_tx_cnt);
		}
	} else {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret == MACSUCCESS) {
			MAC_REG_W8(R_AX_TXCNT + 2, rty->tx_cnt);
			MAC_REG_W8(R_AX_TXCNT + 3, rty->short_tx_cnt);
		}
	}

	return ret;
}

u32 get_data_rty_limit(struct mac_ax_adapter *adapter,
		       struct mac_ax_rty_lmt *rty)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	struct mac_role_tbl *role;
	u32 offset_l, offset_s;
	u8 band;

	role = mac_role_srch(adapter, rty->macid);
	if (!role) {
		PLTFM_MSG_ERR("%s: The MACID%d does not exist\n",
			      __func__, rty->macid);
		return MACNOITEM;
	}

	if (role->info.c_info.data_txcnt_lmt_sel) {
		rty->tx_cnt = (u8)role->info.c_info.data_tx_cnt_lmt;
	} else {
		band = role->info.wmm < 2 ? 0 : 1;
		offset_l = band == 0 ? R_AX_TXCNT + 2 : R_AX_TXCNT_C1 + 2;
		offset_s = band == 0 ? R_AX_TXCNT + 3 : R_AX_TXCNT_C1 + 3;
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret == MACSUCCESS) {
			rty->tx_cnt = MAC_REG_R8(offset_l);
			rty->short_tx_cnt = MAC_REG_R8(offset_s);
		}
	}

	return ret;
}

u32 set_bacam_mode(struct mac_ax_adapter *adapter, u8 mode_sel)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_RESPBA_CAM_CTRL) & (~B_AX_BACAM_ENT_CFG);

	if (mode_sel)
		MAC_REG_W32(R_AX_RESPBA_CAM_CTRL, val32 | B_AX_BACAM_ENT_CFG);
	else
		MAC_REG_W32(R_AX_RESPBA_CAM_CTRL, val32);
	return MACSUCCESS;
#else
	return MACHWNOSUP;
#endif
}

u32 set_xtal_aac(struct mac_ax_adapter *adapter, u8 aac_mode)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

#if MAC_AX_8852A_SUPPORT
	u8 val8;

	val8 = MAC_REG_R8(R_AX_XTAL_ON_CTRL2);
	val8 &= ~(0x30);
	val8 |= ((aac_mode & B_AX_AAC_MODE_MSK) << B_AX_AAC_MODE_SH);
	MAC_REG_W8(R_AX_XTAL_ON_CTRL2, val8);
#else
	PLTFM_MSG_ERR("non Support IC for xtal acc\n");
#endif

	return MACSUCCESS;
}

u32 set_rxd_zld_en(struct mac_ax_adapter *adapter, u8 enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	if (enable) {
		val32 = MAC_REG_R32(R_AX_ZLENDEL_COUNT);
		val32 |= B_AX_RXD_DELI_EN;
		MAC_REG_W32(R_AX_ZLENDEL_COUNT, val32);
	} else {
		val32 = MAC_REG_R32(R_AX_ZLENDEL_COUNT);
		val32 &= ~B_AX_RXD_DELI_EN;
		MAC_REG_W32(R_AX_ZLENDEL_COUNT, val32);
	}

	return MACSUCCESS;
}

u32 set_partial_pld_mode(struct mac_ax_adapter *adapter, u8 enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	if (enable) {
		val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
		val32 |= B_AX_TX_PARTIAL_MODE;
		MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);
	} else {
		val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
		val32 &= ~B_AX_TX_PARTIAL_MODE;
		MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);
	}

	return MACSUCCESS;
}

u32 set_nav_padding(struct mac_ax_adapter *adapter,
		    struct mac_ax_nav_padding *nav)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_txop = nav->band ? R_AX_PTCL_NAV_PROT_LEN_C1 : R_AX_PTCL_NAV_PROT_LEN;
	u32 reg_cnt = nav->band ? R_AX_PROT_C1 : R_AX_PROT;
	u32 ret;
	u8 val8;
#if MAC_AX_FW_REG_OFLD
	u16 tmp;
#endif

	ret = check_mac_en(adapter, nav->band, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		tmp = nav->nav_pad_en ? nav->nav_padding : 0;
		ret = MAC_REG_W16_OFLD((u16)reg_txop,
				       tmp, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
			return ret;
		}
		ret = MAC_REG_W16_OFLD((u16)reg_cnt,
				       tmp, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
			return ret;
		}
		ret = MAC_REG_W_OFLD((u16)reg_cnt, B_AX_NAV_OVER_TXOP_EN,
				     nav->over_txop_en ? 1 : 0, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
			return ret;
		}

		return MACSUCCESS;
	}
#endif

	if (nav->nav_pad_en) {
		MAC_REG_W16(reg_txop, nav->nav_padding);
		MAC_REG_W16(reg_cnt, nav->nav_padding);
		val8 = MAC_REG_R8(reg_cnt + 2);
		if (nav->over_txop_en)
			val8 |= BIT(0);
		else
			val8 &= ~BIT(0);
		MAC_REG_W8(reg_cnt + 2, val8);
	} else {
		MAC_REG_W16(reg_txop, 0);
		MAC_REG_W16(reg_cnt, 0);
	}

	return MACSUCCESS;
}

static void set_delay_tx_cfg(struct mac_ax_adapter *adapter,
			     struct mac_ax_delay_tx_cfg *cfg)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_SS_CTRL);
	SET_CLR_WORD(val32, cfg->en, B_AX_SS_DELAY_TX_BAND_SEL);
	MAC_REG_W32(R_AX_SS_CTRL, val32);

	val32 = SET_WORD(cfg->vovi_to_b0, B_AX_SS_VOVI_TO_0) |
		SET_WORD(cfg->bebk_to_b0, B_AX_SS_BEBK_TO_0) |
		SET_WORD(cfg->vovi_to_b1, B_AX_SS_VOVI_TO_1) |
		SET_WORD(cfg->bebk_to_b1, B_AX_SS_BEBK_TO_1);
	MAC_REG_W32(R_AX_SS_DELAYTX_TO, val32);

	val32 = SET_WORD(cfg->vovi_len_b0, B_AX_SS_VOVI_LEN_THR_0) |
		SET_WORD(cfg->bebk_len_b0, B_AX_SS_BEBK_LEN_THR_0) |
		SET_WORD(cfg->vovi_len_b1, B_AX_SS_VOVI_LEN_THR_1) |
		SET_WORD(cfg->bebk_len_b1, B_AX_SS_BEBK_LEN_THR_1);
	MAC_REG_W32(R_AX_SS_DELAYTX_LEN_THR, val32);
}

u32 set_core_swr_volt(struct mac_ax_adapter *adapter,
		      enum mac_ax_core_swr_volt volt_sel)
{
	u8 i, j, adjust = 0;
	s8 sign = 0, v, val8;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val8 = adapter->hw_info->core_swr_volt_sel - volt_sel;
	if (val8 == 0) {
		return MACSUCCESS;
	} else if (val8 > 0) {
		adjust = adapter->hw_info->core_swr_volt_sel - volt_sel;
		sign = -1;
	} else {
		adjust = volt_sel - adapter->hw_info->core_swr_volt_sel;
		sign = 1;
	}

	for (i = 0; i < adjust; i++) {
		val8 = MAC_REG_R8(R_AX_SPS_DIG_ON_CTRL0);
		v = GET_FIELD(val8, B_AX_VOL_L1);
		v += sign;
		if (v < CORE_SWR_VOLT_MIN)
			v = CORE_SWR_VOLT_MIN;
		else if (v > CORE_SWR_VOLT_MAX)
			v = CORE_SWR_VOLT_MAX;

		val8 = SET_CLR_WORD(val8, v, B_AX_VOL_L1);
		MAC_REG_W8(R_AX_SPS_DIG_ON_CTRL0, val8);
		for (j = 0; j < POLL_SWR_VOLT_CNT; j++)
			PLTFM_DELAY_US(POLL_SWR_VOLT_US);
	}

	if (volt_sel == MAC_AX_SWR_NORM) {
		val8 = MAC_REG_R8(R_AX_SPS_DIG_ON_CTRL0);
		val8 = SET_CLR_WORD(val8, adapter->hw_info->core_swr_volt,
				    B_AX_VOL_L1);
		MAC_REG_W8(R_AX_SPS_DIG_ON_CTRL0, val8);
	}

	adapter->hw_info->core_swr_volt_sel = volt_sel;

	return MACSUCCESS;
}

u32 set_pcie_driving_mponly(struct mac_ax_adapter *adapter,
			    enum mac_ax_pcie_driving_ctrl drving_ctrl)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT

#if MAC_AX_FEATURE_HV
		if (adapter->env == HV_AX_FPGA || adapter->env == HV_AX_PXP)
			return MACSUCCESS;
#endif
		MAC_REG_W16(RAC_DIRECT_OFFSET_G1 + RAC_ANA20 * 2, drving_ctrl);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G1 + RAC_ANA21 * 2, PCIE_5G6G_PER_GEN1GEN2_ANA21_VAL);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G1 + RAC_ANA23 * 2, PCIE_5G6G_PER_GEN1GEN2_ANA23_VAL);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G1 + RAC_ANA2F * 2, PCIE_5G6G_PER_GEN1GEN2_ANA2F_VAL);

		MAC_REG_W16(RAC_DIRECT_OFFSET_G2 + RAC_ANA20 * 2, drving_ctrl);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G2 + RAC_ANA21 * 2, PCIE_5G6G_PER_GEN1GEN2_ANA21_VAL);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G2 + RAC_ANA23 * 2, PCIE_5G6G_PER_GEN1GEN2_ANA23_VAL);
		MAC_REG_W16(RAC_DIRECT_OFFSET_G2 + RAC_ANA2F * 2, PCIE_5G6G_PER_GEN1GEN2_ANA2F_VAL);

		PLTFM_MSG_WARN("[MAC] Adjust PCIe drving only for MP use\n");
#endif
	}

	return MACSUCCESS;
}

u32 set_macid_pause(struct mac_ax_adapter *adapter,
		    struct mac_ax_macid_pause_cfg *cfg)
{
	u32 val32;
	u8 macid_sh = cfg->macid & (32 - 1);
	u8 macid_grp = cfg->macid >> 5;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;
	struct mac_ax_macid_pause_grp grp = {{0}};

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		if (cfg->pause) {
			switch (macid_grp) {
			case 0:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_0);
				MAC_REG_W32(R_AX_MACID_SLEEP_0,
					    val32 | BIT(macid_sh));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_0);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_0,
					    val32 | BIT(macid_sh));
				break;
			case 1:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_1);
				MAC_REG_W32(R_AX_MACID_SLEEP_1,
					    val32 | BIT(macid_sh));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_1);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_1,
					    val32 | BIT(macid_sh));
				break;
			case 2:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_2);
				MAC_REG_W32(R_AX_MACID_SLEEP_2,
					    val32 | BIT(macid_sh));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_2);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_2,
					    val32 | BIT(macid_sh));
				break;
			case 3:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_3);
				MAC_REG_W32(R_AX_MACID_SLEEP_3,
					    val32 | BIT(macid_sh));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_3);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_3,
					    val32 | BIT(macid_sh));
				break;
			default:
				break;
			}
		} else {
			switch (macid_grp) {
			case 0:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_0);
				MAC_REG_W32(R_AX_MACID_SLEEP_0,
					    val32 & ~(BIT(macid_sh)));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_0);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_0,
					    val32 & ~(BIT(macid_sh)));
				break;
			case 1:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_1);
				MAC_REG_W32(R_AX_MACID_SLEEP_1,
					    val32 & ~(BIT(macid_sh)));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_1);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_1,
					    val32 & ~(BIT(macid_sh)));
				break;
			case 2:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_2);
				MAC_REG_W32(R_AX_MACID_SLEEP_2,
					    val32 & ~(BIT(macid_sh)));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_2);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_2,
					    val32 & ~(BIT(macid_sh)));
				break;
			case 3:
				val32 = MAC_REG_R32(R_AX_MACID_SLEEP_3);
				MAC_REG_W32(R_AX_MACID_SLEEP_3,
					    val32 & ~(BIT(macid_sh)));
				val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_3);
				MAC_REG_W32(R_AX_SS_MACID_PAUSE_3,
					    val32 & ~(BIT(macid_sh)));
				break;
			default:
				break;
			}
		}
	} else {
		grp.mask_grp[macid_grp] = BIT(macid_sh);
		grp.pause_grp[macid_grp] = cfg->pause << macid_sh;

		ret = macid_pause(adapter, &grp);

		if (ret)
			return ret;
	}
	return MACSUCCESS;
}

u32 set_cctl_preld(struct mac_ax_adapter *adapter,
		   struct mac_ax_cctl_preld_cfg *cfg)
{
#if MAC_AX_PCIE_SUPPORT
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct rtw_hal_mac_ax_cctl_info info;
	struct rtw_hal_mac_ax_cctl_info mask;

	PLTFM_MEMSET(&mask, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));
	PLTFM_MEMSET(&info, 0, sizeof(struct rtw_hal_mac_ax_cctl_info));

	info.preld_en = cfg->en;
	mask.preld_en = 1;

	mops->upd_cctl_info(adapter, &info, &mask, cfg->macid, TBL_WRITE_OP);

	return MACSUCCESS;
#else
	return MACPROCERR;
#endif
}

u32 macid_pause(struct mac_ax_adapter *adapter,
		struct mac_ax_macid_pause_grp *grp)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_macid_pause *para;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_macid_pause));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	para = (struct fwcmd_macid_pause *)buf;

	para->dword0 =
	cpu_to_le32(SET_WORD(grp->pause_grp[0],
			     FWCMD_H2C_MACID_PAUSE_PAUSE_GRP_1));
	para->dword1 =
	cpu_to_le32(SET_WORD(grp->pause_grp[1],
			     FWCMD_H2C_MACID_PAUSE_PAUSE_GRP_2));
	para->dword2 =
	cpu_to_le32(SET_WORD(grp->pause_grp[2],
			     FWCMD_H2C_MACID_PAUSE_PAUSE_GRP_3));
	para->dword3 =
	cpu_to_le32(SET_WORD(grp->pause_grp[3],
			     FWCMD_H2C_MACID_PAUSE_PAUSE_GRP_4));
	para->dword4 =
	cpu_to_le32(SET_WORD(grp->mask_grp[0],
			     FWCMD_H2C_MACID_PAUSE_MASK_GRP_1));
	para->dword5 =
	cpu_to_le32(SET_WORD(grp->mask_grp[1],
			     FWCMD_H2C_MACID_PAUSE_MASK_GRP_2));
	para->dword6 =
	cpu_to_le32(SET_WORD(grp->mask_grp[2],
			     FWCMD_H2C_MACID_PAUSE_MASK_GRP_3));
	para->dword7 =
	cpu_to_le32(SET_WORD(grp->mask_grp[3],
			     FWCMD_H2C_MACID_PAUSE_MASK_GRP_4));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_MACID_PAUSE,
			      1, 0);
	if (ret)
		goto fail;

	// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
	// The H2C shall be sent by mac_h2c_agg_tx.
	ret = h2c_agg_enqueue(adapter, h2cb);
	if (ret == MACSUCCESS)
		return MACSUCCESS;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

static u32 poll_macid_pause_sleep_done_ack(struct mac_ax_adapter *adapter)
{
#define RETRY_WAIT_PKT_US 50
#define MACID_PAUSE_SLEEP_WAIT_PKT_CNT 20000
	u32 cnt;

	cnt = TX_PAUSE_WAIT_PKT_CNT;
	while (--cnt) {
		if (adapter->sm.macid_pause_sleep == MAC_AX_OFLD_H2C_DONE)
			break;
		PLTFM_DELAY_US(RETRY_WAIT_PKT_US);
	}
	adapter->sm.macid_pause_sleep = MAC_AX_OFLD_H2C_IDLE;
	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]macid_pause_sleep DONE ACK timeout\n");
		return MACPROCERR;
	}
	return MACSUCCESS;
}

u32 macid_pause_sleep(struct mac_ax_adapter *adapter,
		      struct mac_ax_macid_pause_sleep_grp *grp)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_macid_pause_sleep *para;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_ERR("[ERR]%s fw not ready\n", __func__);
		return MACNOFW;
	}

	if (adapter->sm.macid_pause_sleep != MAC_AX_OFLD_H2C_IDLE) {
		PLTFM_MSG_ERR("[ERR]%s state machine not MAC_AX_OFLD_H2C_IDLE\n", __func__);
		return MACPROCERR;
	}

	adapter->sm.macid_pause_sleep = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_macid_pause_sleep));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	para = (struct fwcmd_macid_pause_sleep *)buf;

	para->dword0 =
	cpu_to_le32(SET_WORD(grp->pause_grp[0],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_EN_1));
	para->dword1 =
	cpu_to_le32(SET_WORD(grp->pause_grp[1],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_EN_2));
	para->dword2 =
	cpu_to_le32(SET_WORD(grp->pause_grp[2],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_EN_3));
	para->dword3 =
	cpu_to_le32(SET_WORD(grp->pause_grp[3],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_EN_4));
	para->dword4 =
	cpu_to_le32(SET_WORD(grp->pause_grp_mask[0],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_MASK_1));
	para->dword5 =
	cpu_to_le32(SET_WORD(grp->pause_grp_mask[1],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_MASK_2));
	para->dword6 =
	cpu_to_le32(SET_WORD(grp->pause_grp_mask[2],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_MASK_3));
	para->dword7 =
	cpu_to_le32(SET_WORD(grp->pause_grp_mask[3],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_PAUSE_GRP_MASK_4));

	para->dword8 =
	cpu_to_le32(SET_WORD(grp->sleep_grp[0],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_EN_1));
	para->dword9 =
	cpu_to_le32(SET_WORD(grp->sleep_grp[1],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_EN_2));
	para->dword10 =
	cpu_to_le32(SET_WORD(grp->sleep_grp[2],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_EN_3));
	para->dword11 =
	cpu_to_le32(SET_WORD(grp->sleep_grp[3],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_EN_4));
	para->dword12 =
	cpu_to_le32(SET_WORD(grp->sleep_grp_mask[0],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_MASK_1));
	para->dword13 =
	cpu_to_le32(SET_WORD(grp->sleep_grp_mask[1],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_MASK_2));
	para->dword14 =
	cpu_to_le32(SET_WORD(grp->sleep_grp_mask[2],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_MASK_3));
	para->dword15 =
	cpu_to_le32(SET_WORD(grp->sleep_grp_mask[3],
			     FWCMD_H2C_MACID_PAUSE_SLEEP_SLEEP_GRP_MASK_4));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_MACID_PAUSE_SLEEP,
			      0, 1);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return poll_macid_pause_sleep_done_ack(adapter);
fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

static u32 enable_macid_pause_sleep(struct mac_ax_adapter *adapter,
				    u8 macid, u8 enable, u32 *grp_reg)
{
#define MACID_PAUSE_SH 5
#define MACID_PAUSE_MSK 0x1F
	u32 val32;
	u8 macid_sh, macid_grp;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	macid_sh = macid & MACID_PAUSE_MSK;
	macid_grp = macid >> MACID_PAUSE_SH;
	if (macid_grp >= 4) {
		PLTFM_MSG_ERR("%s: unexpected macid %x\n",
			      __func__, macid);
		return MACNOTSUP;
	}
	val32 = MAC_REG_R32(grp_reg[macid_grp]);
	if (enable)
		MAC_REG_W32(grp_reg[macid_grp], val32 | BIT(macid_sh));
	else
		MAC_REG_W32(grp_reg[macid_grp], val32 & ~(BIT(macid_sh)));

	return MACSUCCESS;
#undef MACID_PAUSE_SH
#undef MACID_PAUSE_MSK
}

u32 set_macid_pause_sleep(struct mac_ax_adapter *adapter,
			  struct mac_ax_macid_pause_sleep_cfg *cfg)
{
#define MACID_PAUSE_SH 5
#define MACID_PAUSE_MSK 0x1F
	u8 macid_sh;
	u8 macid_grp;
	u32 ret;
	struct mac_ax_macid_pause_sleep_grp grp = {{0}};
	u32 pause_reg[4] = {
		R_AX_SS_MACID_PAUSE_0, R_AX_SS_MACID_PAUSE_1,
		R_AX_SS_MACID_PAUSE_2, R_AX_SS_MACID_PAUSE_3
	};
	u32 sleep_reg[4] = {
		R_AX_MACID_SLEEP_0, R_AX_MACID_SLEEP_1,
		R_AX_MACID_SLEEP_2, R_AX_MACID_SLEEP_3
	};

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		ret = enable_macid_pause_sleep(adapter, cfg->macid, cfg->pause, pause_reg);
		if (ret) {
			PLTFM_MSG_ERR("%s: setting pause failed: %d\n",
				      __func__, ret);
			return ret;
		}
		ret = enable_macid_pause_sleep(adapter, cfg->macid, cfg->sleep, sleep_reg);
		if (ret) {
			PLTFM_MSG_ERR("%s: setting sleep failed: %d\n",
				      __func__, ret);
			return ret;
		}
	} else {
		macid_sh = cfg->macid & MACID_PAUSE_MSK;
		macid_grp = cfg->macid >> MACID_PAUSE_SH;

		grp.pause_grp_mask[macid_grp] = BIT(macid_sh);
		grp.pause_grp[macid_grp] = (cfg->pause ? 1 : 0) << macid_sh;

		grp.sleep_grp_mask[macid_grp] = BIT(macid_sh);
		grp.sleep_grp[macid_grp] = (cfg->sleep ? 1 : 0) << macid_sh;

		ret = macid_pause_sleep(adapter, &grp);

		if (ret)
			return ret;
	}
	return MACSUCCESS;
#undef MACID_PAUSE_SH
#undef MACID_PAUSE_MSK
}

u32 set_ss_quota_mode(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_quota_mode_ctrl *ctrl)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32_wmm, val32_ul, ret;

	val32_wmm = MAC_REG_R32(R_AX_SS_DL_QUOTA_CTRL);
	val32_ul = MAC_REG_R32(R_AX_SS_UL_QUOTA_CTRL);
	switch (ctrl->wmm) {
	case MAC_AX_SS_WMM0:
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_CNT)
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm | B_AX_SS_QUOTA_MODE_0);
		else
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm & ~B_AX_SS_QUOTA_MODE_0);
		break;
	case MAC_AX_SS_WMM1:
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_CNT)
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm | B_AX_SS_QUOTA_MODE_1);
		else
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm & ~B_AX_SS_QUOTA_MODE_1);
		break;
	case MAC_AX_SS_WMM2:
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_CNT)
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm | B_AX_SS_QUOTA_MODE_2);
		else
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm & ~B_AX_SS_QUOTA_MODE_2);
		break;
	case MAC_AX_SS_WMM3:
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_CNT)
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm | B_AX_SS_QUOTA_MODE_3);
		else
			MAC_REG_W32(R_AX_SS_DL_QUOTA_CTRL,
				    val32_wmm & ~B_AX_SS_QUOTA_MODE_3);
		break;
	case MAC_AX_SS_UL:
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_CNT)
			MAC_REG_W32(R_AX_SS_UL_QUOTA_CTRL,
				    val32_ul | B_AX_SS_QUOTA_MODE_UL);
		else
			MAC_REG_W32(R_AX_SS_UL_QUOTA_CTRL,
				    val32_ul & ~B_AX_SS_QUOTA_MODE_UL);
		break;
	}
#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		switch (ctrl->wmm) {
		case MAC_AX_SS_WMM0:
		case MAC_AX_SS_WMM1:
		case MAC_AX_SS_UL:
			ret = check_mac_en(adapter, 0, MAC_AX_CMAC_SEL);
			if (ret != MACSUCCESS)
				return ret;
			if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_TIME) {
				ret = MAC_REG_W_OFLD(R_AX_PTCL_ATM,
						     B_AX_ATM_AIRTIME_EN, 1, 1);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("%s: config fail\n",
						      __func__);
					return ret;
				}
			}
			break;
		case MAC_AX_SS_WMM2:
		case MAC_AX_SS_WMM3:
			ret = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL);
			if (ret != MACSUCCESS)
				return ret;
			if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_TIME) {
				ret = MAC_REG_W_OFLD(R_AX_PTCL_ATM_C1,
						     B_AX_ATM_AIRTIME_EN, 1, 1);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("%s: config fail\n",
						      __func__);
					return ret;
				}
			}
			break;
		}

		return MACSUCCESS;
	}
#endif

	switch (ctrl->wmm) {
	case MAC_AX_SS_WMM0:
	case MAC_AX_SS_WMM1:
	case MAC_AX_SS_UL:
		ret = check_mac_en(adapter, 0, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;
		val32_wmm = MAC_REG_R32(R_AX_PTCL_ATM);
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_TIME)
			MAC_REG_W32(R_AX_PTCL_ATM,
				    val32_wmm | B_AX_ATM_AIRTIME_EN);
		break;
	case MAC_AX_SS_WMM2:
	case MAC_AX_SS_WMM3:
		ret = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;
		val32_wmm = MAC_REG_R32(R_AX_PTCL_ATM_C1);
		if (ctrl->mode == MAC_AX_SS_QUOTA_MODE_TIME)
			MAC_REG_W32(R_AX_PTCL_ATM_C1,
				    val32_wmm | B_AX_ATM_AIRTIME_EN);
		break;
	}

	return MACSUCCESS;
}

u32 ss_set_quotasetting(struct mac_ax_adapter *adapter,
			struct mac_ax_ss_quota_setting *para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 cnt = 1000;

	if (para->ul_dl == mac_ax_issue_ul) {
		val32 = (B_AX_SS_OWN |
			SET_WORD(SS_W_QUOTA_SETTING, B_AX_SS_CMD_SEL) |
			BIT(23) | SET_WORD(para->val, B_AX_SS_VALUE) |
			para->macid);
		MAC_REG_W32(R_AX_SS_SRAM_CTRL_1, val32);
	} else {
		val32 = (B_AX_SS_OWN |
			SET_WORD(SS_W_QUOTA_SETTING, B_AX_SS_CMD_SEL) |
			SET_WORD(para->ac_type, B_AX_SS_AC) |
			SET_WORD(para->val, B_AX_SS_VALUE) | para->macid);
		MAC_REG_W32(R_AX_SS_SRAM_CTRL_1, val32);
	}

	while (--cnt) {
		val32 = MAC_REG_R32(R_AX_SS_SRAM_CTRL_1);
		if ((val32 & B_AX_SS_OWN) == 0)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("SS Set quota setting fail!!\n");
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

u32 scheduler_set_prebkf(struct mac_ax_adapter *adapter,
			 struct mac_ax_prebkf_setting *para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg, val32;
#if MAC_AX_FW_REG_OFLD
	u32 ret;
#endif

	reg = para->band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_0_C1 :
			    R_AX_PREBKF_CFG_0;
#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)reg, B_AX_PREBKF_TIME_MSK, para->val,
				     1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("%s: config fail\n", __func__);
			return ret;
		}

		return MACSUCCESS;
	}
#endif
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, para->val, B_AX_PREBKF_TIME);
	MAC_REG_W32(reg, val32);

	return MACSUCCESS;
}

u32 mac_get_tx_cnt(struct mac_ax_adapter *adapter,
		   struct mac_ax_tx_cnt *cnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 txcnt_addr;
	u32 val32;
	u16 val16;
	u8 sel;

	if (cnt->band != 0 && cnt->band != 1)
		return MACNOITEM;
	if (check_mac_en(adapter, cnt->band, MAC_AX_CMAC_SEL))
		return MACHWNOTEN;
	txcnt_addr = (cnt->band == MAC_AX_BAND_0) ?
		      R_AX_TX_PPDU_CNT : R_AX_TX_PPDU_CNT_C1;
	for (sel = 0; sel < MAC_AX_TX_ALLTYPE; sel++) {
		val16 = MAC_REG_R16(txcnt_addr);
		val16 = SET_CLR_WORD(val16, sel, B_AX_PPDU_CNT_IDX);
		MAC_REG_W16(txcnt_addr, val16);
		PLTFM_DELAY_US(1000);
		val32 = MAC_REG_R32(txcnt_addr);
		cnt->txcnt[sel] = GET_FIELD(val32, B_AX_TX_PPDU_CNT);
	}
	return MACSUCCESS;
}

u32 cfg_wdt_isr_rst(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val = MAC_REG_R8(R_AX_PLATFORM_ENABLE);

	val = val & ~B_AX_APB_WRAP_EN;
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val);

	val = val | B_AX_APB_WRAP_EN;
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val);

	return MACSUCCESS;
}

u32 mac_clr_tx_cnt(struct mac_ax_adapter *adapter,
		   struct mac_ax_tx_cnt *cnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 txcnt_addr;
	u16 val16;
	u16 to;
	u8 i;

	if (cnt->band != 0 && cnt->band != 1)
		return MACNOITEM;
	if (check_mac_en(adapter, cnt->band, MAC_AX_CMAC_SEL))
		return MACHWNOTEN;
	if (cnt->sel > MAC_AX_TX_ALLTYPE)
		return MACNOITEM;

	txcnt_addr = (cnt->band == MAC_AX_BAND_0) ?
		      R_AX_TX_PPDU_CNT : R_AX_TX_PPDU_CNT_C1;

#if MAC_AX_FW_REG_OFLD
	u32 ret;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		for (i = 0; i < MAC_AX_TX_ALLTYPE; i++) {
			if (cnt->sel == MAC_AX_TX_ALLTYPE || i == cnt->sel) {
				ret = MAC_REG_W_OFLD(txcnt_addr,
						     B_AX_PPDU_CNT_RIDX_MSK <<
						     B_AX_PPDU_CNT_RIDX_SH,
						     i, 0);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("%s: write offload fail;"
						      "offset: %u, ret: %u\n",
						      __func__, txcnt_addr, ret);
					return ret;
				}
				ret = MAC_REG_W_OFLD(txcnt_addr, B_AX_RST_PPDU_CNT,
						     1, 0);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("%s: write offload fail;"
						      "offset: %u, ret: %u\n",
						      __func__, txcnt_addr, ret);
					return ret;
				}
				ret = MAC_REG_P_OFLD(txcnt_addr, B_AX_RST_PPDU_CNT, 0,
						     (cnt->sel != MAC_AX_TX_ALLTYPE ||
						     i == MAC_AX_TX_ALLTYPE - 1) ?
						     1 : 0);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("%s: poll offload fail;"
						      "offset: %u, ret: %u\n",
						      __func__, txcnt_addr, ret);
					return ret;
				}
			}
		}
		return MACSUCCESS;
	}
#endif
	to = 1000;
	for (i = 0; i < MAC_AX_TX_ALLTYPE; i++) {
		if (cnt->sel == MAC_AX_TX_ALLTYPE || i == cnt->sel) {
			val16 = MAC_REG_R16(txcnt_addr);
			val16 = SET_CLR_WORD(val16, i, B_AX_PPDU_CNT_RIDX) |
					     B_AX_RST_PPDU_CNT;
			MAC_REG_W16(txcnt_addr, val16);
			while (to--) {
				val16 = MAC_REG_R16(txcnt_addr);
				if (!(val16 & B_AX_RST_PPDU_CNT))
					break;
				PLTFM_DELAY_US(5);
			}
			if (to == 0)
				return MACPOLLTO;
		}
	}
	return MACSUCCESS;
}

u32 mac_set_adapter_info(struct mac_ax_adapter *adapter,
			 struct mac_ax_adapter_info *set)
{
#ifdef RTW_WKARD_GET_PROCESSOR_ID
	adapter->hw_info->adpt_info.cust_proc_id.proc_id.proc_id_h =
		set->cust_proc_id.proc_id.proc_id_h;
	adapter->hw_info->adpt_info.cust_proc_id.proc_id.proc_id_l =
		set->cust_proc_id.proc_id.proc_id_l;
	adapter->hw_info->adpt_info.cust_proc_id.customer_id =
		set->cust_proc_id.customer_id;
	memcpy(set->cust_proc_id.base_board_id,
	       adapter->hw_info->adpt_info.cust_proc_id.base_board_id, BASE_BOARD_ID_LEN);
#endif
	return MACSUCCESS;
}

u32 mac_set_hw_value(struct mac_ax_adapter *adapter,
		     enum mac_ax_hw_id hw_id, void *val)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	if (!val) {
		PLTFM_MSG_ERR("[ERR]: the parameter is NULL in %s\n", __func__);
		return MACNPTR;
	}

	switch (hw_id) {
	case MAC_AX_HW_SETTING:
		break;
	case MAC_AX_HW_SET_ID_PAUSE:
		ret = set_macid_pause(adapter,
				      (struct mac_ax_macid_pause_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_MULTI_ID_PAUSE:
		ret = macid_pause(adapter,
				  (struct mac_ax_macid_pause_grp *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_ID_PAUSE_SLEEP:
		ret = set_macid_pause_sleep(adapter,
					    (struct mac_ax_macid_pause_sleep_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_MULTI_ID_PAUSE_SLEEP:
		ret = macid_pause_sleep(adapter,
					(struct mac_ax_macid_pause_sleep_grp *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_SCH_TXEN_CFG:
		ret = set_hw_sch_tx_en(adapter,
				       (struct mac_ax_sch_tx_en_cfg *)val);
		break;
	case MAC_AX_HW_SET_AMPDU_CFG:
		ret = set_hw_ampdu_cfg(adapter, (struct mac_ax_ampdu_cfg *)val);
		break;
	case MAC_AX_HW_SET_USR_EDCA_PARAM:
		ret =
		set_hw_usr_edca_param(adapter,
				      (struct mac_ax_usr_edca_param *)val);
		break;
	case MAC_AX_HW_SET_USR_TX_RPT_CFG:
		ret =
		set_hw_usr_tx_rpt_cfg(adapter,
				      (struct mac_ax_usr_tx_rpt_cfg *)val);
		break;
	case MAC_AX_HW_SET_EDCA_PARAM:
		ret = set_hw_edca_param(adapter,
					(struct mac_ax_edca_param *)val);
		break;
	case MAC_AX_HW_SET_EDCCA_PARAM:
		ret = set_hw_edcca_param(adapter,
					 (struct mac_ax_edcca_param *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_MUEDCA_PARAM:
		ret = set_hw_muedca_param(adapter,
					  (struct mac_ax_muedca_param *)val);
		break;
	case MAC_AX_HW_SET_MUEDCA_CTRL:
		ret = set_hw_muedca_ctrl(adapter,
					 (struct mac_ax_muedca_cfg *)val);
		break;
	case MAC_AX_HW_SET_TBPPDU_CTRL:
		ret = set_hw_tb_ppdu_ctrl(adapter,
					  (struct mac_ax_tb_ppdu_ctrl *)val);
		break;
	case MAC_AX_HW_SET_HOST_RPR:
		set_host_rpr(adapter, (struct mac_ax_host_rpr_cfg *)val);
		break;
	case MAC_AX_HW_SET_DELAYTX_CFG:
		set_delay_tx_cfg(adapter, (struct mac_ax_delay_tx_cfg *)val);
		break;
	case MAC_AX_HW_SET_BW_CFG:
		ret = cfg_mac_bw(adapter, (struct mac_ax_cfg_bw *)val);
		break;
	case MAC_AX_HW_SET_CH_BUSY_STAT_CFG:
		ret = set_hw_ch_busy_cnt(adapter,
					 (struct mac_ax_ch_busy_cnt_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_LIFETIME_CFG:
		ret = set_hw_lifetime_cfg(adapter,
					  (struct mac_ax_lifetime_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_EN_BB_RF:
		ret = set_enable_bb_rf(adapter, *(u8 *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_APP_FCS:
		set_append_fcs(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_RX_ICVERR:
		set_accept_icverr(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_CCTL_RTY_LMT:
		set_cctl_rty_limit(adapter,
				   (struct mac_ax_cctl_rty_lmt_cfg *)val);
		break;
	case MAC_AX_HW_SET_COEX_GNT:
		ret = p_ops->cfg_gnt(adapter, (struct mac_ax_coex_gnt *)val);
		break;
	case MAC_AX_HW_SET_SCOREBOARD:
		mac_cfg_sb(adapter, *(u32 *)val);
		break;
	case MAC_AX_HW_SET_POLLUTED:
		mac_cfg_plt(adapter, (struct mac_ax_plt *)val);
		break;
	case MAC_AX_HW_SET_COEX_CTRL:
		p_ops->cfg_ctrl_path(adapter, *(u32 *)val);
		break;
	case MAC_AX_HW_SET_CLR_TX_CNT:
		ret = mac_clr_tx_cnt(adapter, (struct mac_ax_tx_cnt *)val);
		break;
	case MAC_AX_HW_SET_SLOT_TIME:
		mac_set_slot_time(adapter, *(enum mac_ax_slot_time *)val);
		break;
	case MAC_AX_HW_SET_XTAL_AAC_MODE:
		set_xtal_aac(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_NAV_PADDING:
		ret = set_nav_padding(adapter, (struct mac_ax_nav_padding *)val);
		break;
	case MAC_AX_HW_SET_MAX_TX_TIME:
		ret = mac_set_cctl_max_tx_time(adapter,
					       (struct mac_ax_max_tx_time *)
					       val);
		break;
	case MAC_AX_HW_SET_SS_QUOTA_MODE:
		ret = set_ss_quota_mode(adapter,
					(struct mac_ax_ss_quota_mode_ctrl *)val);
		break;
	case MAC_AX_HW_SET_SS_QUOTA_SETTING:
		ret = ss_set_quotasetting(adapter,
					  (struct mac_ax_ss_quota_setting *)val);
		break;
	case MAC_AX_HW_SET_SCHE_PREBKF:
		ret = scheduler_set_prebkf(adapter,
					   (struct mac_ax_prebkf_setting *)val);
		break;
	case MAC_AX_HW_SET_WDT_ISR_RST:
		ret = cfg_wdt_isr_rst(adapter);
		break;
	case MAC_AX_HW_SET_RESP_ACK:
		ret = set_mac_resp_ack(adapter, (u32 *)val);
		break;
	case MAC_AX_HW_SET_HW_RTS_TH:
		ret = mac_set_hw_rts_th(adapter,
					(struct mac_ax_hw_rts_th *)val);
		break;
	case MAC_AX_HW_SET_TX_RU26_TB:
		ret = mac_set_tx_ru26_tb(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_BACAM_MODE_SEL:
		ret = set_bacam_mode(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_CORE_SWR_VOLT:
		ret = set_core_swr_volt(adapter,
					*(enum mac_ax_core_swr_volt *)val);
		break;
	case MAC_AX_HW_SET_PARTIAL_PLD_MODE:
		ret = set_partial_pld_mode(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_GT3_TIMER:
		ret = set_gt3_timer(adapter,
				    (struct mac_ax_gt3_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_SET_RRSR_CFG:
		ret = p_ops->set_rrsr_cfg(adapter,
					  (struct mac_ax_rrsr_cfg *)val);
		break;
	case MAC_AX_HW_SET_CTS_RRSR_CFG:
		ret = p_ops->set_cts_rrsr_cfg(adapter,
					      (struct mac_ax_cts_rrsr_cfg *)val);
		break;
	case MAC_AX_HW_SET_ADAPTER:
		ret = mac_set_adapter_info(adapter,
					   (struct mac_ax_adapter_info *)val);
		break;
	case MAC_AX_HW_SET_RESP_ACK_CHK_CCA:
		ret = _patch_rsp_ack(adapter, (struct mac_ax_resp_chk_cca *)val);
		break;
	case MAC_AX_HW_SET_SIFS_R2T_T2T:
		ret =
		set_hw_sifs_r2t_t2t(adapter,
				    (struct mac_ax_sifs_r2t_t2t_ctrl *)val);
		break;
	case MAC_AX_HW_SET_RXD_ZLD_EN:
		ret = set_rxd_zld_en(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_SER_DBG_LVL:
		ret = mac_dbg_log_lvl_adjust(adapter, (struct mac_debug_log_lvl *)val);
		break;
	case MAC_AX_HW_SET_DATA_RTY_LMT:
		ret = set_data_rty_limit(adapter, (struct mac_ax_rty_lmt *)val);
		break;
	case MAC_AX_HW_SET_CTS2SELF:
		ret = set_cts2self(adapter, (struct mac_ax_cts2self_cfg *)val);
		break;
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_HW_SDIO_INFO:
		set_info_sdio(adapter, (struct mac_ax_sdio_info *)val);
		break;
	case MAC_AX_HW_SDIO_TX_MODE:
		ret = p_ops->tx_mode_cfg_sdio(adapter,
					      *(enum mac_ax_sdio_tx_mode *)val);
		break;
	case MAC_AX_HW_SDIO_RX_AGG:
		p_ops->rx_agg_cfg_sdio(adapter, (struct mac_ax_rx_agg_cfg *)val);
		break;
	case MAC_AX_HW_SDIO_TX_AGG:
		ret = tx_agg_cfg_sdio(adapter,
				      (struct mac_ax_sdio_txagg_cfg *)val);
		break;
	case MAC_AX_HW_SDIO_AVAL_PAGE:
		p_ops->aval_page_cfg_sdio(adapter, (struct mac_ax_aval_page_cfg *)val);
		break;
	case MAC_AX_HW_SDIO_MON_WT:
		p_ops->set_wt_cfg_sdio(adapter, *(u8 *)val);
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_HW_PCIE_CFGSPC_SET:
		ret = cfgspc_set_pcie(adapter,
				      (struct mac_ax_pcie_cfgspc_param *)val);
		break;
	case MAC_AX_HW_PCIE_RST_BDRAM:
		ret = p_ops->rst_bdram_pcie(adapter, *(u8 *)val);
		break;
	case MAX_AX_HW_PCIE_LTR_SW_TRIGGER:
		ret = p_ops->ltr_sw_trigger(adapter,
					    *(enum mac_ax_pcie_ltr_sw_ctrl *)val);
		break;
	case MAX_AX_HW_PCIE_MIT:
		ret = p_ops->trx_mit_pcie(adapter,
					  (struct mac_ax_pcie_trx_mitigation *)val);
		break;
	case MAX_AX_HW_PCIE_L2_LEAVE:
		ret = set_pcie_l2_leave(adapter, *(u8 *)val);
		break;
	case MAC_AX_HW_SET_CCTL_PRELD:
		set_cctl_preld(adapter, (struct mac_ax_cctl_preld_cfg *)val);
		break;
	case MAC_AX_HW_PCIE_DRIVING_MPONLY:
		set_pcie_driving_mponly(adapter, *(enum mac_ax_pcie_driving_ctrl *)val);
		break;
#endif
	default:
		return MACNOITEM;
	}

	return ret;
}

u32 get_macid_pause(struct mac_ax_adapter *adapter,
		    struct mac_ax_macid_pause_cfg *cfg)
{
	u32 val32 = 0;
	u8 macid_grp = cfg->macid >> 5;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret)
		return ret;

	switch (macid_grp) {
	case 0:
		val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_0) |
			MAC_REG_R32(R_AX_MACID_SLEEP_0);
		break;
	case 1:
		val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_1) |
			MAC_REG_R32(R_AX_MACID_SLEEP_1);
		break;
	case 2:
		val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_2) |
			MAC_REG_R32(R_AX_MACID_SLEEP_2);
		break;
	case 3:
		val32 = MAC_REG_R32(R_AX_SS_MACID_PAUSE_3) |
			MAC_REG_R32(R_AX_MACID_SLEEP_3);
		break;
	default:
		break;
	}
	cfg->pause = (u8)((val32 & BIT(cfg->macid & (32 - 1))) ? 1 : 0);

	return MACSUCCESS;
}

u32 get_ss_wmm_tbl(struct mac_ax_adapter *adapter,
		   struct mac_ax_ss_wmm_tbl_ctrl *ctrl)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_SS_CTRL);
	switch (ctrl->wmm) {
	case 0:
		ctrl->wmm_mapping =
		(enum mac_ax_ss_wmm_tbl)GET_FIELD(val32, B_AX_SS_WMM_SEL_0);
		break;
	case 1:
		ctrl->wmm_mapping =
		(enum mac_ax_ss_wmm_tbl)GET_FIELD(val32, B_AX_SS_WMM_SEL_1);
		break;
	case 2:
		ctrl->wmm_mapping =
		(enum mac_ax_ss_wmm_tbl)GET_FIELD(val32, B_AX_SS_WMM_SEL_2);
		break;
	case 3:
		ctrl->wmm_mapping =
		(enum mac_ax_ss_wmm_tbl)GET_FIELD(val32, B_AX_SS_WMM_SEL_3);
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

static void get_delay_tx_cfg(struct mac_ax_adapter *adapter,
			     struct mac_ax_delay_tx_cfg *cfg)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_SS_CTRL);
	cfg->en =
		(enum mac_ax_delay_tx_en)GET_FIELD(val32,
						   B_AX_SS_DELAY_TX_BAND_SEL);

	val32 = MAC_REG_R32(R_AX_SS_DELAYTX_TO);
	cfg->vovi_to_b0 = GET_FIELD(val32, B_AX_SS_VOVI_TO_0);
	cfg->bebk_to_b0 = GET_FIELD(val32, B_AX_SS_BEBK_TO_0);
	cfg->vovi_to_b1 = GET_FIELD(val32, B_AX_SS_VOVI_TO_1);
	cfg->bebk_to_b1 = GET_FIELD(val32, B_AX_SS_BEBK_TO_1);

	val32 = MAC_REG_R32(R_AX_SS_DELAYTX_LEN_THR);
	cfg->vovi_len_b0 = GET_FIELD(val32, B_AX_SS_VOVI_LEN_THR_0);
	cfg->bebk_len_b0 = GET_FIELD(val32, B_AX_SS_BEBK_LEN_THR_0);
	cfg->vovi_len_b1 = GET_FIELD(val32, B_AX_SS_VOVI_LEN_THR_1);
	cfg->bebk_len_b1 = GET_FIELD(val32, B_AX_SS_BEBK_LEN_THR_1);
}

static u32 get_append_fcs(struct mac_ax_adapter *adapter, u8 *enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	*enable = MAC_REG_R8(R_AX_MPDU_PROC) & B_AX_APPEND_FCS ? 1 : 0;

	return MACSUCCESS;
}

static u32 get_accept_icverr(struct mac_ax_adapter *adapter, u8 *enable)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	*enable = MAC_REG_R8(R_AX_MPDU_PROC) & B_AX_A_ICV_ERR ? 1 : 0;

	return MACSUCCESS;
}

u32 get_bacam_mode(struct mac_ax_adapter *adapter, u8 *mode_sel)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	*mode_sel = MAC_REG_R8(R_AX_RESPBA_CAM_CTRL) & B_AX_BACAM_ENT_CFG ? 1 : 0;

	return MACSUCCESS;
#else
	return MACHWNOSUP;
#endif
}

u32 get_pwr_state(struct mac_ax_adapter *adapter, enum mac_ax_mac_pwr_st *st)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val;

	val = GET_FIELD(MAC_REG_R32(R_AX_IC_PWR_STATE), B_AX_WLMAC_PWR_STE);

	if (val == MAC_AX_MAC_OFF) {
		*st = MAC_AX_MAC_OFF;
		adapter->mac_pwr_info.pwr_in_lps = 0;
		adapter->sm.fw_rst = MAC_AX_FW_RESET_IDLE;
		adapter->sm.pwr = MAC_AX_PWR_OFF;
		adapter->sm.mac_rdy = MAC_AX_MAC_NOT_RDY;
		PLTFM_MSG_WARN("WL MAC is in off state.\n");
	} else if (val == MAC_AX_MAC_ON) {
		*st = MAC_AX_MAC_ON;
	} else if (val == MAC_AX_MAC_LPS) {
		*st = MAC_AX_MAC_LPS;
	} else {
		PLTFM_MSG_ERR("Unexpected MAC state = 0x%X\n", val);
		return MACPWRSTAT;
	}

	return MACSUCCESS;
}

void get_dflt_nav(struct mac_ax_adapter *adapter, u16 *nav)
{
	/* data NAV is consist of SIFS and ACK/BA time */
	/* currently, we use SIFS + 64-bitmap BA as default NAV */
	/* we use OFDM-6M to estimate BA time */
	/* BA time = PLCP header(20us) + 32 bytes/data_rate */
	*nav = 63;
}

u32 mac_get_hw_value(struct mac_ax_adapter *adapter,
		     enum mac_ax_hw_id hw_id, void *val)
{
	u32 ret = MACSUCCESS;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (!val) {
		PLTFM_MSG_ERR("[ERR]: the parameter is NULL in %s\n", __func__);
		return MACNPTR;
	}

	switch (hw_id) {
	case MAC_AX_HW_MAPPING:
		break;
	case MAC_AX_HW_GET_EFUSE_SIZE:
		*(u32 *)val = adapter->hw_info->efuse_size +
			      adapter->hw_info->dav_efuse_size;
		break;
	case MAC_AX_HW_GET_LOGICAL_EFUSE_SIZE:
		*(u32 *)val = adapter->hw_info->log_efuse_size +
			      adapter->hw_info->dav_log_efuse_size;
		break;
	case MAC_AX_HW_GET_LIMIT_LOG_EFUSE_SIZE:
		switch (adapter->hw_info->intf) {
		case MAC_AX_INTF_PCIE:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_pcie;
			break;
		case MAC_AX_INTF_USB:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_usb;
			break;
		case MAC_AX_INTF_SDIO:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_sdio;
			break;
		default:
			*(u32 *)val = adapter->hw_info->log_efuse_size;
			break;
		}
		*(u32 *)val += adapter->hw_info->dav_log_efuse_size;
		break;
	case MAC_AX_HW_GET_BT_EFUSE_SIZE:
		*(u32 *)val = adapter->hw_info->bt_efuse_size;
		break;
	case MAC_AX_HW_GET_BT_LOGICAL_EFUSE_SIZE:
		*(u32 *)val = adapter->hw_info->bt_log_efuse_size;
		break;
	case MAC_AX_HW_GET_EFUSE_MASK_SIZE:
		*(u32 *)val = (adapter->hw_info->log_efuse_size +
			       adapter->hw_info->dav_log_efuse_size) >> 4;
		break;
	case MAC_AX_HW_GET_LIMIT_EFUSE_MASK_SIZE:
		switch (adapter->hw_info->intf) {
		case MAC_AX_INTF_PCIE:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_pcie;
			break;
		case MAC_AX_INTF_USB:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_usb;
			break;
		case MAC_AX_INTF_SDIO:
			*(u32 *)val = adapter->hw_info->limit_efuse_size_sdio;
			break;
		default:
			*(u32 *)val = adapter->hw_info->log_efuse_size;
			break;
		}
		*(u32 *)val += adapter->hw_info->dav_log_efuse_size;
		*(u32 *)val = *(u32 *)val >> 4;
		break;
	case MAC_AX_HW_GET_BT_EFUSE_MASK_SIZE:
		*(u32 *)val = adapter->hw_info->bt_log_efuse_size >> 4;
		break;
	case MAC_AX_HW_GET_DAV_LOG_EFUSE_SIZE:
		*(u32 *)val = adapter->hw_info->dav_log_efuse_size;
		break;
	case MAC_AX_HW_GET_EFUSE_VERSION_SIZE:
		*(u32 *)val = adapter->hw_info->efuse_version_size;
		break;
	case MAC_AX_HW_GET_ID_PAUSE:
		ret = get_macid_pause(adapter,
				      (struct mac_ax_macid_pause_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_SCH_TXEN_STATUS:
		ret = get_hw_sch_tx_en(adapter,
				       (struct mac_ax_sch_tx_en_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_EDCA_PARAM:
		ret = get_hw_edca_param(adapter,
					(struct mac_ax_edca_param *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_TBPPDU_CTRL:
		ret = get_hw_tb_ppdu_ctrl(adapter,
					  (struct mac_ax_tb_ppdu_ctrl *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_DELAYTX_CFG:
		get_delay_tx_cfg(adapter, (struct mac_ax_delay_tx_cfg *)val);
		break;
	case MAC_AX_HW_GET_SS_WMM_TBL:
		ret = get_ss_wmm_tbl(adapter,
				     (struct mac_ax_ss_wmm_tbl_ctrl *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_CH_STAT_CNT:
		ret = get_hw_ch_stat_cnt(adapter,
					 (struct mac_ax_ch_stat_cnt *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_LIFETIME_CFG:
		ret = get_hw_lifetime_cfg(adapter,
					  (struct mac_ax_lifetime_cfg *)val);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case MAC_AX_HW_GET_APP_FCS:
		get_append_fcs(adapter, (u8 *)val);
		break;
	case MAC_AX_HW_GET_RX_ICVERR:
		get_accept_icverr(adapter, (u8 *)val);
		break;
	case MAC_AX_HW_GET_PWR_STATE:
		get_pwr_state(adapter, (enum mac_ax_mac_pwr_st *)val);
		break;
	case MAC_AX_HW_GET_SCOREBOARD:
		*(u32 *)val = MAC_REG_R32(R_AX_SCOREBOARD);
		break;
	case MAC_AX_HW_GET_BACAM_MODE_SEL:
		get_bacam_mode(adapter, (u8 *)val);
		break;
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_HW_SDIO_TX_AGG_SIZE:
		*(u16 *)val = adapter->sdio_info.tx_align_size;
		break;
	case MAC_AX_HW_GET_SDIO_RX_REQ_LEN:
		ret = p_ops->get_sdio_rx_req_len(adapter, (u32 *)val);
		break;
	case MAC_AX_HW_GET_SDIO_LPS_FLG:
		*(u8 *)val = adapter->mac_pwr_info.pwr_in_lps;
		break;
#endif
	case MAC_AX_HW_GET_WAKE_REASON:
		ret = get_wake_reason(adapter, (u8 *)val);
		if (ret != 0)
			return ret;
		break;
	case MAC_AX_HW_GET_COEX_GNT:
		ret = p_ops->get_gnt(adapter, (struct mac_ax_coex_gnt *)val);
		break;
	case MAC_AX_HW_GET_COEX_CTRL:
		p_ops->get_ctrl_path(adapter, (u32 *)val);
		break;
	case MAC_AX_HW_GET_TX_CNT:
		ret = mac_get_tx_cnt(adapter, (struct mac_ax_tx_cnt *)val);
		if (ret != 0)
			return ret;
		break;
	case MAC_AX_HW_GET_TSF:
		ret = mac_get_tsf(adapter, (struct mac_ax_port_tsf *)val);
		break;
	case MAC_AX_HW_GET_FREERUN_CNT:
		ret = mac_get_freerun(adapter, (struct mac_ax_freerun *)val);
		break;
	case MAC_AX_HW_GET_MAX_TX_TIME:
		ret = mac_get_max_tx_time(adapter,
					  (struct mac_ax_max_tx_time *)val);
		break;
	case MAC_AX_HW_GET_POLLUTED_CNT:
		mac_get_bt_polt_cnt(adapter, (struct mac_ax_bt_polt_cnt *)val);
		break;
	case MAC_AX_HW_GET_DATA_RTY_LMT:
		get_data_rty_limit(adapter, (struct mac_ax_rty_lmt *)val);
		break;
	case MAC_AX_HW_GET_DFLT_NAV:
		get_dflt_nav(adapter, (u16 *)val);
		break;
	case MAC_AX_HW_GET_FW_CAP:
		ret = mac_get_fw_cap(adapter, (u32 *)val);
		break;
	case MAC_AX_HW_GET_RRSR_CFG:
		ret = p_ops->get_rrsr_cfg(adapter,
					  (struct mac_ax_rrsr_cfg *)val);
		break;
	case MAC_AX_HW_GET_CTS_RRSR_CFG:
		ret = p_ops->get_cts_rrsr_cfg(adapter,
					  (struct mac_ax_cts_rrsr_cfg *)val);
		break;
	case MAC_AX_HW_GET_USB_STS:
		ret = ops->get_rx_state(adapter, (u32 *)val);
		break;
	case MAC_AX_HW_GET_WD_PAGE_NUM:
		*(u32 *)val = (u32)adapter->dle_info.hif_min;
		break;
	default:
		return MACNOITEM;
	}

	return ret;
}

u32 cfg_mac_bw(struct mac_ax_adapter *adapter, struct mac_ax_cfg_bw *cfg)
{
	u32 value32 = 0;
	u8 value8 = 0;
	u8 chk_val8 = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct rtw_hal_com_t *hal_com =
		(struct rtw_hal_com_t *)adapter->drv_adapter;

	u8 txsc20 = 0, txsc40 = 0, txsc80 = 0;

	switch (cfg->cbw) {
	case CHANNEL_WIDTH_160:
		txsc80 = rtw_hal_bb_get_txsc(hal_com, cfg->pri_ch,
					     cfg->central_ch, cfg->cbw,
					     CHANNEL_WIDTH_80);
		/* fall through */
	case CHANNEL_WIDTH_80:
		txsc40 = rtw_hal_bb_get_txsc(hal_com, cfg->pri_ch,
					     cfg->central_ch, cfg->cbw,
					     CHANNEL_WIDTH_40);
		/* fall through */
	case CHANNEL_WIDTH_40:
		txsc20 = rtw_hal_bb_get_txsc(hal_com, cfg->pri_ch,
					     cfg->central_ch, cfg->cbw,
					     CHANNEL_WIDTH_20);
		break;
	case CHANNEL_WIDTH_10:
		value32 = MAC_REG_R32(R_AX_AFE_CTRL1);
		value32 = value32 | B_AX_CMAC_CLK_SEL;
		MAC_REG_W32(R_AX_AFE_CTRL1, value32);
		MAC_REG_W8(R_AX_SLOTTIME_CFG, SLOTTIME_10M);
		MAC_REG_W8(R_AX_RSP_CHK_SIG, ACK_TO_10M);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			MAC_REG_W8(R_AX_TSF_32K_SEL, US_TIME_10M);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			MAC_REG_W8(R_AX_TSF_32K_SEL_V1, US_TIME_10M);
		}
#endif
		break;
	case CHANNEL_WIDTH_5:
		value32 = MAC_REG_R32(R_AX_AFE_CTRL1);
		value32 = value32 | B_AX_CMAC_CLK_SEL;
		MAC_REG_W32(R_AX_AFE_CTRL1, value32);
		MAC_REG_W8(R_AX_SLOTTIME_CFG, SLOTTIME_5M);
		MAC_REG_W8(R_AX_RSP_CHK_SIG, ACK_TO_5M);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			MAC_REG_W8(R_AX_TSF_32K_SEL, US_TIME_5M);
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			MAC_REG_W8(R_AX_TSF_32K_SEL_V1, US_TIME_5M);
		}
#endif
		break;
	default:
		break;
	}

#if MAC_AX_FW_REG_OFLD
	u32 ret;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		if (cfg->band) {//BAND1
			value8 = MAC_REG_R8(R_AX_WMAC_RFMOD_C1);
			chk_val8 = MAC_REG_R8(R_AX_TXRATE_CHK_C1);
		} else {//BAND0
			value8 = MAC_REG_R8(R_AX_WMAC_RFMOD);
			chk_val8 = MAC_REG_R8(R_AX_TXRATE_CHK);
		}
		value8 = value8 & (~(BIT(0) | BIT(1)));
		chk_val8 = chk_val8 & (~(BIT(0) | BIT(1)));

		switch (cfg->cbw) {
		case CHANNEL_WIDTH_160:
			value8 = value8 | BIT(1) | BIT(0);
			value32 = txsc20 | (txsc40 << 4) | (txsc80 << 8); //TXSC_160M;
			break;
		case CHANNEL_WIDTH_80:
			value8 = value8 | BIT(1);
			value32 = txsc20 | (txsc40 << 4); //TXSC_80M;
			break;
		case CHANNEL_WIDTH_40:
			value8 = value8 | BIT(0);
			value32 = txsc20; //TXSC_40M;
			break;
		case CHANNEL_WIDTH_20:
			value32 = 0; //TXSC_20M;
			break;
		case CHANNEL_WIDTH_10:
			value32 = 0; //TXSC_20M;
			break;
		case CHANNEL_WIDTH_5:
			value32 = 0; //TXSC_20M;
			break;
		default:
			break;
		}

		if (cfg->pri_ch >= CHANNEL_5G)
			chk_val8 |= B_AX_CHECK_CCK_EN | B_AX_RTS_LIMIT_IN_OFDM6;

		if (cfg->band) {//BAND1
			ret = MAC_REG_W_OFLD(R_AX_WMAC_RFMOD_C1, B_AX_WMAC_RFMOD_MSK,
					     value8, 0);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
			ret = MAC_REG_W_OFLD(R_AX_TXRATE_CHK_C1, 0x3, chk_val8,
					     0);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
			ret = MAC_REG_W32_OFLD(R_AX_TX_SUB_CARRIER_VALUE_C1,
					       value32, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
		} else {
			ret = MAC_REG_W_OFLD(R_AX_WMAC_RFMOD, B_AX_WMAC_RFMOD_MSK,
					     value8, 0);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
			ret = MAC_REG_W_OFLD(R_AX_TXRATE_CHK, 0x3, chk_val8, 0);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
			ret = MAC_REG_W32_OFLD(R_AX_TX_SUB_CARRIER_VALUE,
					       value32, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: config fail\n", __func__);
				return ret;
			}
		}

	return MACSUCCESS;
	}
#endif
	if (cfg->band) {//BAND1
		value8 = MAC_REG_R8(R_AX_WMAC_RFMOD_C1);
		chk_val8 = MAC_REG_R8(R_AX_TXRATE_CHK_C1);
	} else {//BAND0
		value8 = MAC_REG_R8(R_AX_WMAC_RFMOD);
		chk_val8 = MAC_REG_R8(R_AX_TXRATE_CHK);
	}
	value8 = value8 & (~(BIT(0) | BIT(1)));
	chk_val8 = chk_val8 & (~(BIT(0) | BIT(1) | BIT(4)));

	switch (cfg->cbw) {
	case CHANNEL_WIDTH_160:
		value8 = value8 | BIT(1) | BIT(0);
		value32 = txsc20 | (txsc40 << 4) | (txsc80 << 8); //TXSC_160M;
		break;
	case CHANNEL_WIDTH_80:
		value8 = value8 | BIT(1);
		value32 = txsc20 | (txsc40 << 4); //TXSC_80M;
		break;
	case CHANNEL_WIDTH_40:
		value8 = value8 | BIT(0);
		value32 = txsc20; //TXSC_40M;
		break;
	case CHANNEL_WIDTH_20:
		value32 = 0; //TXSC_20M;
		break;
	case CHANNEL_WIDTH_10:
		value32 = 0; //TXSC_20M;
		break;
	case CHANNEL_WIDTH_5:
		value32 = 0; //TXSC_20M;
		break;
	default:
		break;
	}

	/*Setting for CCK rate in 5G/6G Channel protection*/
	if (cfg->pri_ch >= CHANNEL_5G) // remove after phl setting band_type
		chk_val8 |= B_AX_CHECK_CCK_EN | B_AX_RTS_LIMIT_IN_OFDM6;

	if (cfg->band_type == BAND_ON_24G)
		chk_val8 |= B_AX_BAND_MODE;
	else if (cfg->band_type == BAND_ON_5G)
		chk_val8 |= B_AX_CHECK_CCK_EN | B_AX_RTS_LIMIT_IN_OFDM6;
	else if (cfg->band_type == BAND_ON_6G)
		chk_val8 |= B_AX_CHECK_CCK_EN | B_AX_RTS_LIMIT_IN_OFDM6;
	else
		PLTFM_MSG_ERR("[ERR]band_type = %d\n", cfg->band_type);

	if (cfg->band) {//BAND1
		MAC_REG_W8(R_AX_WMAC_RFMOD_C1, value8);
		MAC_REG_W8(R_AX_TXRATE_CHK_C1, chk_val8);
		MAC_REG_W32(R_AX_TX_SUB_CARRIER_VALUE_C1, value32);
	} else {
		MAC_REG_W8(R_AX_WMAC_RFMOD, value8);
		MAC_REG_W8(R_AX_TXRATE_CHK, chk_val8);
		MAC_REG_W32(R_AX_TX_SUB_CARRIER_VALUE, value32);
	}

	return MACSUCCESS;
}

u32 mac_write_xtal_si(struct mac_ax_adapter *adapter,
		      u8 offset, u8 val, u8 bitmask)
{
	u32 cnt = 0;
	u32 write_val = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	cnt = XTAL_SI_POLLING_CNT;
	write_val = SET_CLR_WORD(write_val, offset, B_AX_WL_XTAL_SI_ADDR);
	write_val = SET_CLR_WORD(write_val, val, B_AX_WL_XTAL_SI_DATA);
	write_val = SET_CLR_WORD(write_val, bitmask, B_AX_WL_XTAL_SI_BITMASK);
	write_val = SET_CLR_WORD(write_val, XTAL_SI_NORMAL_WRITE,
				 B_AX_WL_XTAL_SI_MODE);
	write_val = (write_val | B_AX_WL_XTAL_SI_CMD_POLL);
	MAC_REG_W32(R_AX_WLAN_XTAL_SI_CTRL, write_val);

	while ((MAC_REG_R32(R_AX_WLAN_XTAL_SI_CTRL) & B_AX_WL_XTAL_SI_CMD_POLL)
						== B_AX_WL_XTAL_SI_CMD_POLL) {
		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]xtal si not ready(W)\n");
			return MACPOLLTO;
		}
		cnt--;
		PLTFM_DELAY_US(XTAL_SI_POLLING_DLY_US);
	}

	return MACSUCCESS;
}

u32 mac_read_xtal_si(struct mac_ax_adapter *adapter,
		     u8 offset, u8 *val)
{
	u32 cnt = 0;
	u32 write_val = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	cnt = XTAL_SI_POLLING_CNT;
	write_val = SET_CLR_WORD(write_val, offset, B_AX_WL_XTAL_SI_ADDR);
	write_val = SET_CLR_WORD(write_val, 0x00, B_AX_WL_XTAL_SI_DATA);
	write_val = SET_CLR_WORD(write_val, 0x00, B_AX_WL_XTAL_SI_BITMASK);
	write_val = SET_CLR_WORD(write_val, XTAL_SI_NORMAL_READ,
				 B_AX_WL_XTAL_SI_MODE);
	write_val = (write_val | B_AX_WL_XTAL_SI_CMD_POLL);
	MAC_REG_W32(R_AX_WLAN_XTAL_SI_CTRL, write_val);

	while ((MAC_REG_R32(R_AX_WLAN_XTAL_SI_CTRL) & B_AX_WL_XTAL_SI_CMD_POLL)
						== B_AX_WL_XTAL_SI_CMD_POLL) {
		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]xtal_si not ready(R)\n");
			return MACPOLLTO;
		}
		cnt--;
		PLTFM_DELAY_US(XTAL_SI_POLLING_DLY_US);
	}

	*val = MAC_REG_R8(R_AX_WLAN_XTAL_SI_CTRL + 1);

	return MACSUCCESS;
}

u32 set_host_rpr(struct mac_ax_adapter *adapter,
		 struct mac_ax_host_rpr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_host_rpr_cfg *def_cfg;
	enum mac_ax_host_rpr_mode mode;
	u8 is_poh;
	u32 val32, nval32;
	u32 ret = MACSUCCESS;

	if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
		ret = is_qta_poh(adapter, adapter->dle_info.qta_mode, &is_poh);
		if (ret) {
			PLTFM_MSG_ERR("is qta poh check fail %d\n", ret);
			return ret;
		}
		def_cfg = is_poh ? &rpr_cfg_poh : &rpr_cfg_stf;
		mode = is_poh ? MAC_AX_RPR_MODE_POH : MAC_AX_RPR_MODE_STF;
	} else {
		def_cfg = &rpr_cfg_stf;
		mode = MAC_AX_RPR_MODE_STF;
	}

	val32 = MAC_REG_R32(R_AX_WDRLS_CFG);
	nval32 = SET_CLR_WORD(val32, mode, B_AX_WDRLS_MODE);
	if (nval32 != val32)
		MAC_REG_W32(R_AX_WDRLS_CFG, nval32);

	val32 = MAC_REG_R32(R_AX_RLSRPT0_CFG0);
	nval32 = val32;
	if ((cfg->txok_en == MAC_AX_FUNC_DEF &&
	     def_cfg->txok_en == MAC_AX_FUNC_EN) ||
	    cfg->txok_en == MAC_AX_FUNC_EN)
		nval32 |= B_WDRLS_FLTR_TXOK;
	else
		nval32 &= ~B_WDRLS_FLTR_TXOK;
	if ((cfg->rty_lmt_en == MAC_AX_FUNC_DEF &&
	     def_cfg->rty_lmt_en == MAC_AX_FUNC_EN) ||
	    cfg->rty_lmt_en == MAC_AX_FUNC_EN)
		nval32 |= B_WDRLS_FLTR_RTYLMT;
	else
		nval32 &= ~B_WDRLS_FLTR_RTYLMT;
	if ((cfg->lft_drop_en == MAC_AX_FUNC_DEF &&
	     def_cfg->lft_drop_en == MAC_AX_FUNC_EN) ||
	    cfg->lft_drop_en == MAC_AX_FUNC_EN)
		nval32 |= B_WDRLS_FLTR_LIFTIM;
	else
		nval32 &= ~B_WDRLS_FLTR_LIFTIM;
	if ((cfg->macid_drop_en == MAC_AX_FUNC_DEF &&
	     def_cfg->macid_drop_en == MAC_AX_FUNC_EN) ||
	    cfg->macid_drop_en == MAC_AX_FUNC_EN)
		nval32 |= B_WDRLS_FLTR_MACID;
	else
		nval32 &= ~B_WDRLS_FLTR_MACID;
	if (nval32 != val32)
		MAC_REG_W32(R_AX_RLSRPT0_CFG0, nval32);

	val32 = MAC_REG_R32(R_AX_RLSRPT0_CFG1);
	nval32 = SET_CLR_WORD(val32, (cfg->agg_def ? def_cfg->agg : cfg->agg),
			      B_AX_RLSRPT0_AGGNUM);
	nval32 = SET_CLR_WORD(nval32, (cfg->tmr_def ? def_cfg->tmr : cfg->tmr),
			      B_AX_RLSRPT0_TO);
	if (nval32 != val32)
		MAC_REG_W32(R_AX_RLSRPT0_CFG1, nval32);

	return ret;
}

u32 set_l2_status(struct mac_ax_adapter *adapter)
{
	adapter->sm.l2_st = MAC_AX_L2_EN;

	return MACSUCCESS;
}

u32 mac_write_pwr_ofst_mode(struct mac_ax_adapter *adapter,
			    u8 band, struct rtw_tpu_info *tpu)
{
#if	MAC_USB_IO_ACC_ON
#else
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#endif
	u32 cr = (band == HW_BAND_0) ? R_AX_PWR_RATE_OFST_CTRL :
		 R_AX_PWR_RATE_OFST_CTRL_C1;
	u32 val32 = 0, ret = 0;
	s8 *tmp = &tpu->pwr_ofst_mode[0];

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if	MAC_USB_IO_ACC_ON
	val32 |= NIB_2_DW(0, 0, 0, tmp[4], tmp[3], tmp[2], tmp[1], tmp[0]);
	ret = MAC_REG_W_OFLD((u16)cr, 0xFFFFF, val32, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}
#else
	val32 = MAC_REG_R32(cr) & ~0xFFFFF;
	val32 |= NIB_2_DW(0, 0, 0, tmp[4], tmp[3], tmp[2], tmp[1], tmp[0]);
	MAC_REG_W32(cr, val32);
#endif

	return MACSUCCESS;
}

u32 mac_write_pwr_ofst_bw(struct mac_ax_adapter *adapter,
			  u8 band, struct rtw_tpu_info *tpu)
{
#if	MAC_USB_IO_ACC_ON
#else
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#endif
	u32 cr = (band == HW_BAND_0) ? R_AX_PWR_LMT_CTRL :
		 R_AX_PWR_LMT_CTRL_C1;
	u32 val32 = 0, ret = 0;
	s8 *tmp = &tpu->pwr_ofst_bw[0];

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if	MAC_USB_IO_ACC_ON
	val32 |= NIB_2_DW(0, 0, 0, tmp[4], tmp[3], tmp[2], tmp[1], tmp[0]);
	ret = MAC_REG_W_OFLD((u16)cr, 0xFFFFF, val32, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}
#else
	val32 = MAC_REG_R32(cr) & ~0xFFFFF;
	val32 |= NIB_2_DW(0, 0, 0, tmp[4], tmp[3], tmp[2], tmp[1], tmp[0]);
	MAC_REG_W32(cr, val32);
#endif

	return MACSUCCESS;
}

u32 mac_write_pwr_ref_reg(struct mac_ax_adapter *adapter,
			  u8 band, struct rtw_tpu_info *tpu)
{
#if	MAC_USB_IO_ACC_ON
#else
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#endif
	u32 cr = (band == HW_BAND_0) ? R_AX_PWR_RATE_CTRL :
		 R_AX_PWR_RATE_CTRL_C1;
	u32 val32 = 0, ret = 0;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if	MAC_USB_IO_ACC_ON
	val32 |= (((tpu->ref_pow_ofdm & 0x1ff) << 9) |
		 ((tpu->ref_pow_cck & 0x1ff)));
	ret = MAC_REG_W_OFLD((u16)cr, 0xFFFFC00, val32, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}
#else
	val32 = MAC_REG_R32(cr) & ~0xFFFFC00;
	val32 |= (((tpu->ref_pow_ofdm & 0x1ff) << 19) |
		 ((tpu->ref_pow_cck & 0x1ff) << 10));
	MAC_REG_W32(cr, val32);
#endif

	return MACSUCCESS;
}

u32 mac_write_pwr_limit_en(struct mac_ax_adapter *adapter,
			   u8 band, struct rtw_tpu_info *tpu)
{
#if	MAC_USB_IO_ACC_ON
#else
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#endif
	u32 val32 = 0, ret = 0, cr = 0;

	cr = (band == HW_BAND_0) ? R_AX_PWR_LMT_CTRL :
	      R_AX_PWR_LMT_CTRL_C1;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if	MAC_USB_IO_ACC_ON
	if (tpu->pwr_lmt_en)
		val32 =  3;
	ret = MAC_REG_W_OFLD((u16)cr, 0x300000, val32, 0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}

	val32 = 0;
	cr = (band == HW_BAND_0) ? R_AX_PWR_RU_LMT_CTRL : R_AX_PWR_RU_LMT_CTRL_C1;
	if (tpu->pwr_lmt_en)
		val32 =  1;
	ret = MAC_REG_W_OFLD((u16)cr, 0x40000, val32, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: config fail\n", __func__);
		return ret;
	}
#else
	cr = (band == HW_BAND_0) ? R_AX_PWR_LMT_CTRL :
	      R_AX_PWR_LMT_CTRL_C1;
	val32 = 0;

	val32 = MAC_REG_R32(cr) & ~0x300000;
	if (tpu->pwr_lmt_en)
		val32 |=  0x300000;
	MAC_REG_W32(cr, val32);

	cr = (band == HW_BAND_0) ? R_AX_PWR_RU_LMT_CTRL : R_AX_PWR_RU_LMT_CTRL_C1;
		val32 = MAC_REG_R32(cr) & ~BIT18;
	if (tpu->pwr_lmt_en)
		val32 |=  BIT18;
	MAC_REG_W32(cr, val32);
#endif

	return MACSUCCESS;
}

u32 mac_read_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
		     const u32 offset, u32 *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;
	u32 access_offset = offset;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	if (offset < R_AX_PWR_RATE_CTRL || offset > 0xFFFF) {
		PLTFM_MSG_ERR("[ERR]offset exceed pwr ctrl reg %x\n", offset);
		return MACBADDR;
	}

	if (band == MAC_AX_BAND_1)
		access_offset = offset | BIT13;

	ret = mac_check_access(adapter, access_offset);
	if (ret)
		return ret;

	*val = MAC_REG_R32(access_offset);

	return MACSUCCESS;
}

u32 mac_write_msk_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
			  const u32 offset, u32 mask, u32 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	u32 access_offset = offset;
	u32 ori_val = 0;
	u8 shift;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	if (offset < R_AX_PWR_RATE_CTRL || offset > 0xFFFF) {
		PLTFM_MSG_ERR("[ERR]offset exceed pwr ctrl reg %x\n", offset);
		return MACBADDR;
	}

	if (band == MAC_AX_BAND_1)
		access_offset = offset | BIT13;

	ret = mac_check_access(adapter, access_offset);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]check access in %x\n", access_offset);
		return ret;
	}

#if MAC_USB_IO_ACC_ON
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)access_offset, mask, val, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__,
				      access_offset);
			return ret;
		}

		return MACSUCCESS;
	}
#endif

	if (mask != 0xffffffff) {
		shift = shift_mask(mask);
		ori_val = MAC_REG_R32(access_offset);
		val = ((ori_val) & (~mask)) | (((val << shift)) & mask);
	}
	MAC_REG_W32(access_offset, val);

	return MACSUCCESS;
}

u32 mac_write_pwr_reg(struct mac_ax_adapter *adapter, u8 band,
		      const u32 offset, u32 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	u32 access_offset = offset;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	if (offset < R_AX_PWR_RATE_CTRL || offset > 0xFFFF) {
		PLTFM_MSG_ERR("[ERR]offset exceed pwr ctrl reg %x\n", offset);
		return MACBADDR;
	}

	if (band == MAC_AX_BAND_1)
		access_offset = offset | BIT13;

	ret = mac_check_access(adapter, access_offset);
	if (ret)
		return ret;

#if MAC_USB_IO_ACC_ON
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W32_OFLD((u16)access_offset, val, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__,
				      access_offset);
			return ret;
		}

		return MACSUCCESS;
	}
#endif

	MAC_REG_W32(access_offset, val);

	return MACSUCCESS;
}

u32 mac_write_pwr_limit_rua_reg(struct mac_ax_adapter *adapter,
				u8 band, struct rtw_tpu_info *tpu)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;
	u16 cr = (band == HW_BAND_0) ? R_AX_PWR_RU_LMT_TABLE0 :
		 R_AX_PWR_RU_LMT_TABLE0_C1;
	s8 *tmp;
	u8 i, j;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if MAC_USB_IO_ACC_ON
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		for (i = 0; i < HAL_MAX_PATH; i++) {
			for (j = 0; j < TPU_SIZE_RUA; j++) {
				tmp = &tpu->pwr_lmt_ru[i][j][0];
				ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp[3], tmp[2],
								   tmp[1], tmp[0]),
								   0);
				if (ret) {
					PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
						      __func__, cr);
					return ret;
				}
				cr += 4;
				ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp[7], tmp[6],
								   tmp[5], tmp[4]),
								   1);
				if (ret) {
					PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
						      __func__, cr);
					return ret;
				}
				cr += 4;
			}
		}

		return MACSUCCESS;
	}
#endif

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j < TPU_SIZE_RUA; j++) {
			tmp = &tpu->pwr_lmt_ru[i][j][0];
			MAC_REG_W32(cr, BT_2_DW(tmp[3], tmp[2], tmp[1], tmp[0]));
			cr += 4;
			MAC_REG_W32(cr, BT_2_DW(tmp[7], tmp[6], tmp[5], tmp[4]));
			cr += 4;
		}
	}

	return MACSUCCESS;
}

u32 mac_write_pwr_limit_reg(struct mac_ax_adapter *adapter,
			    u8 band, struct rtw_tpu_pwr_imt_info *tpu)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 base = (band == HW_BAND_0) ? R_AX_PWR_RATE_CTRL :
		   R_AX_PWR_RATE_CTRL_C1;
	u32 ss_ofst = 0;
	u32 ret;
	u16 cr = 0;
	s8 *tmp, *tmp_1;
	u8 i, j;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if MAC_USB_IO_ACC_ON
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		for (i = 0; i < HAL_MAX_PATH; i++) {
			tmp = &tpu->pwr_lmt_cck_20m[i][0];
			tmp_1 = &tpu->pwr_lmt_cck_40m[i][0];
			cr = (base | PWR_LMT_CCK_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			tmp = &tpu->pwr_lmt_lgcy_20m[i][0];
			tmp_1 = &tpu->pwr_lmt_20m[i][0][0];
			cr = (base | PWR_LMT_LGCY_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			cr = (base | PWR_LMT_TBL2_OFFSET) + ss_ofst;
			for (j = 1; j <= 5; j += 2) {
				tmp = &tpu->pwr_lmt_20m[i][j][0];
				tmp_1 = &tpu->pwr_lmt_20m[i][j + 1][0];
				ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
								   tmp[1], tmp[0]), 0);
				if (ret) {
					PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
						      __func__, cr);
					return ret;
				}
				cr += 4;
			}

			tmp = &tpu->pwr_lmt_20m[i][7][0];
			tmp_1 = &tpu->pwr_lmt_40m[i][0][0];
			cr = (base | PWR_LMT_TBL5_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			tmp = &tpu->pwr_lmt_40m[i][1][0];
			tmp_1 = &tpu->pwr_lmt_40m[i][2][0];
			cr = (base | PWR_LMT_TBL6_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			tmp = &tpu->pwr_lmt_40m[i][3][0];
			tmp_1 = &tpu->pwr_lmt_80m[i][0][0];
			cr = (base | PWR_LMT_TBL7_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			tmp = &tpu->pwr_lmt_80m[i][1][0];
			tmp_1 = &tpu->pwr_lmt_160m[i][0];
			cr = (base | PWR_LMT_TBL8_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			tmp = &tpu->pwr_lmt_40m_0p5[i][0];
			tmp_1 = &tpu->pwr_lmt_40m_2p5[i][0];
			cr = (base | PWR_LMT_TBL9_OFFSET) + ss_ofst;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp_1[1], tmp_1[0],
							   tmp[1], tmp[0]), 1);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}

			ss_ofst += PWR_LMT_TBL_UNIT;
		}

		return MACSUCCESS;
	}
#endif

	for (i = 0; i < HAL_MAX_PATH; i++) {
		tmp = &tpu->pwr_lmt_cck_20m[i][0];
		tmp_1 = &tpu->pwr_lmt_cck_40m[i][0];
		cr = (base | PWR_LMT_CCK_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		tmp = &tpu->pwr_lmt_lgcy_20m[i][0];
		tmp_1 = &tpu->pwr_lmt_20m[i][0][0];
		cr = (base | PWR_LMT_LGCY_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		cr = (base | PWR_LMT_TBL2_OFFSET) + ss_ofst;
		for (j = 1; j <= 5; j += 2) {
			tmp = &tpu->pwr_lmt_20m[i][j][0];
			tmp_1 = &tpu->pwr_lmt_20m[i][j + 1][0];
			MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1],
						tmp[0]));
			cr += 4;
		}

		tmp = &tpu->pwr_lmt_20m[i][7][0];
		tmp_1 = &tpu->pwr_lmt_40m[i][0][0];
		cr = (base | PWR_LMT_TBL5_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		tmp = &tpu->pwr_lmt_40m[i][1][0];
		tmp_1 = &tpu->pwr_lmt_40m[i][2][0];
		cr = (base | PWR_LMT_TBL6_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		tmp = &tpu->pwr_lmt_40m[i][3][0];
		tmp_1 = &tpu->pwr_lmt_80m[i][0][0];
		cr = (base | PWR_LMT_TBL7_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		tmp = &tpu->pwr_lmt_80m[i][1][0];
		tmp_1 = &tpu->pwr_lmt_160m[i][0];
		cr = (base | PWR_LMT_TBL8_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		tmp = &tpu->pwr_lmt_40m_0p5[i][0];
		tmp_1 = &tpu->pwr_lmt_40m_2p5[i][0];
		cr = (base | PWR_LMT_TBL9_OFFSET) + ss_ofst;
		MAC_REG_W32(cr, BT_2_DW(tmp_1[1], tmp_1[0], tmp[1], tmp[0]));

		ss_ofst += PWR_LMT_TBL_UNIT;
	}

	return MACSUCCESS;
}

u32 mac_write_pwr_by_rate_reg(struct mac_ax_adapter *adapter,
			      u8 band, struct rtw_tpu_pwr_by_rate_info *tpu)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 base = (band == HW_BAND_0) ? R_AX_PWR_RATE_CTRL :
		   R_AX_PWR_RATE_CTRL_C1;
	u32 ret;
	u32 ss_ofst = 0;
	u16 cr = 0;
	s8 *tmp;
	u8 i, j;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret  != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

#if MAC_USB_IO_ACC_ON
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		for (i = 0; i <= 8; i += 4) {
			tmp = &tpu->pwr_by_rate_lgcy[i];
			cr = (base | PWR_BY_RATE_LGCY_OFFSET) + i;
			ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp[3], tmp[2],
							   tmp[1], tmp[0]), 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}
		}

		for (i = 0; i < HAL_MAX_PATH; i++) {
			for (j = 0; j <= 12; j += 4) {
				tmp = &tpu->pwr_by_rate[i][j];
				cr = (base | PWR_BY_RATE_OFFSET) + j + ss_ofst;
				ret = MAC_REG_W32_OFLD(cr, BT_2_DW(tmp[3], tmp[2], tmp[1],
								   tmp[0]), 0);
				if (ret) {
					PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
						      __func__, cr);
					return ret;
				}
			}
			ss_ofst += 0x10; /*16*/
		}

		return MACSUCCESS;
	}
#endif

	for (i = 0; i <= 8; i += 4) {
		tmp = &tpu->pwr_by_rate_lgcy[i];
		cr = (base | PWR_BY_RATE_LGCY_OFFSET) + i;
		MAC_REG_W32(cr, BT_2_DW(tmp[3], tmp[2], tmp[1], tmp[0]));
	}

	for (i = 0; i < HAL_MAX_PATH; i++) {
		for (j = 0; j <= 12; j += 4) {
			tmp = &tpu->pwr_by_rate[i][j];
			cr = (base | PWR_BY_RATE_OFFSET) + j + ss_ofst;
			MAC_REG_W32(cr, BT_2_DW(tmp[3], tmp[2], tmp[1],
						tmp[0]));
		}
		ss_ofst += 0x10; /*16*/
	}

	return MACSUCCESS;
}

u32 mac_read_xcap_reg(struct mac_ax_adapter *adapter, u8 sc_xo, u32 *val)
{
#if MAC_AX_8852A_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (sc_xo) {
		*val = (MAC_REG_R32(R_AX_XTAL_ON_CTRL0) >> B_AX_XTAL_SC_XO_SH) &
		      B_AX_XTAL_SC_XO_MSK;
	} else {
		*val = (MAC_REG_R32(R_AX_XTAL_ON_CTRL0) >> B_AX_XTAL_SC_XI_SH) &
		      B_AX_XTAL_SC_XI_MSK;
	}
#else
	PLTFM_MSG_ERR("non Support IC for read_xcap_reg\n");
#endif

	return MACSUCCESS;
}

u32 mac_write_xcap_reg(struct mac_ax_adapter *adapter, u8 sc_xo, u32 val)
{
#if MAC_AX_8852A_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	if (sc_xo) {
		val32 = MAC_REG_R32(R_AX_XTAL_ON_CTRL0);
		val32 &= ~(0xFE0000);
		val32 |= ((val & B_AX_XTAL_SC_XO_MSK) << B_AX_XTAL_SC_XO_SH);
		MAC_REG_W32(R_AX_XTAL_ON_CTRL0, val32);
	} else {
		val32 = MAC_REG_R32(R_AX_XTAL_ON_CTRL0);
		val32 &= ~(0x1FC00);
		val32 = val32 | ((val & B_AX_XTAL_SC_XI_MSK) <<
				 B_AX_XTAL_SC_XI_SH);
		MAC_REG_W32(R_AX_XTAL_ON_CTRL0, val32);
	}
#else
	PLTFM_MSG_ERR("non Support IC for write_xcap_reg\n");
#endif

	return MACSUCCESS;
}

u32 mac_read_xcap_reg_dav(struct mac_ax_adapter *adapter, u8 sc_xo, u32 *val)
{
	u8 xtal_si_value;
	u32 ret;

	if (sc_xo) {
		ret = mac_read_xtal_si(adapter, XTAL_SI_XTAL_SC_XO, &xtal_si_value);

		if (ret) {
			PLTFM_MSG_ERR("Read XTAL_SI fail!\n");
			return ret;
		}
		*val = xtal_si_value;
	} else {
		ret = mac_read_xtal_si(adapter, XTAL_SI_XTAL_SC_XI, &xtal_si_value);

		if (ret) {
			PLTFM_MSG_ERR("Read XTAL_SI fail!\n");
			return ret;
		}
		*val = xtal_si_value;
	}

	return MACSUCCESS;
}

u32 mac_write_xcap_reg_dav(struct mac_ax_adapter *adapter, u8 sc_xo, u32 val)
{
	u8 xtal_si_value;
	u32 ret;
#ifdef MAC_8851B_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_value;
#endif

	xtal_si_value = (u8)val;
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#ifdef MAC_8851B_SUPPORT
		if (sc_xo) {
			reg_value = MAC_REG_R32(R_AX_XTAL_ON_CTRL3);
			reg_value = SET_CLR_WORD(reg_value, val, B_AX_XTAL_SC_XO_A_BLOCK);
			MAC_REG_W32(R_AX_XTAL_ON_CTRL3, reg_value);
		} else {
			reg_value = MAC_REG_R32(R_AX_XTAL_ON_CTRL3);
			reg_value = SET_CLR_WORD(reg_value, val, B_AX_XTAL_SC_XI_A_BLOCK);
			MAC_REG_W32(R_AX_XTAL_ON_CTRL3, reg_value);
		}
#endif
	} else {
		if (sc_xo) {
			ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_SC_XO, xtal_si_value,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		} else {
			ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_SC_XI, xtal_si_value,
						FULL_BIT_MASK);
			if (ret) {
				PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
				return ret;
			}
		}
	}
	return MACSUCCESS;
}

u32 mac_write_bbrst_reg(struct mac_ax_adapter *adapter, u8 val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;

	val8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
	if (val)
		MAC_REG_W8(R_AX_SYS_FUNC_EN, val8 | B_AX_FEN_BBRSTB);
	else
		MAC_REG_W8(R_AX_SYS_FUNC_EN, val8 & (~B_AX_FEN_BBRSTB));

	return MACSUCCESS;
}

static inline u32 _get_addr_range(struct mac_ax_adapter *adapter, u32 addr)
{
	u32 addr_idx;

#define IOCHKRANG(chip) do { \
		if (ADDR_IS_AON_##chip(addr)) \
			addr_idx = ADDR_AON; \
		else if (ADDR_IS_HCI_##chip(addr)) \
			addr_idx = ADDR_HCI; \
		else if (ADDR_IS_DMAC_##chip(addr)) \
			addr_idx = ADDR_DMAC; \
		else if (ADDR_IS_CMAC0_##chip(addr)) \
			addr_idx = ADDR_CMAC0; \
		else if (ADDR_IS_CMAC1_##chip(addr)) \
			addr_idx = ADDR_CMAC1; \
		else if (ADDR_IS_BB0_##chip(addr)) \
			addr_idx = ADDR_BB0; \
		else if (ADDR_IS_BB1_##chip(addr)) \
			addr_idx = ADDR_BB1; \
		else if (ADDR_IS_RF_##chip(addr)) \
			addr_idx = ADDR_RF; \
		else if (ADDR_IS_IND_ACES_##chip(addr)) \
			addr_idx = ADDR_IND_ACES; \
		else if (ADDR_IS_RSVD_##chip(addr)) \
			addr_idx = ADDR_RSVD; \
		else if (ADDR_IS_PON_##chip(addr)) \
			addr_idx = ADDR_PON; \
		else \
			addr_idx = ADDR_INVALID; \
	} while (0)

	switch (adapter->hw_info->chip_id) {
	case MAC_AX_CHIP_ID_8852A:
		IOCHKRANG(8852A);
		break;
	case MAC_AX_CHIP_ID_8852B:
		IOCHKRANG(8852B);
		break;
	case MAC_AX_CHIP_ID_8852C:
		IOCHKRANG(8852C);
		break;
	case MAC_AX_CHIP_ID_8192XB:
		IOCHKRANG(8192XB);
		break;
	case MAC_AX_CHIP_ID_8851B:
		IOCHKRANG(8851B);
		break;
	case MAC_AX_CHIP_ID_8851E:
		IOCHKRANG(8851E);
		break;
	case MAC_AX_CHIP_ID_8852D:
		IOCHKRANG(8852D);
		break;
	default:
		addr_idx = ADDR_INVALID;
		break;
	}

#undef IOCHKRANG
	return addr_idx;
}

u32 mac_io_chk_access(struct mac_ax_adapter *adapter, u32 offset)
{
	switch (_get_addr_range(adapter, offset)) {
	case ADDR_AON:
	case ADDR_HCI:
		return MACSUCCESS;
	case ADDR_PON:
		break;
	case ADDR_DMAC:
		if (adapter->sm.dmac_func != MAC_AX_FUNC_ON)
			return MACIOERRDMAC;
		break;
	case ADDR_CMAC0:
		if (adapter->sm.cmac0_func != MAC_AX_FUNC_ON)
			return MACIOERRCMAC0;
		break;
	case ADDR_CMAC1:
		if (adapter->sm.cmac1_func != MAC_AX_FUNC_ON)
			return MACIOERRCMAC1;
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			return MACHWNOSUP;
		break;
	case ADDR_BB0:
#if CHK_BBRF_IO
		if (adapter->sm.bb0_func != MAC_AX_FUNC_ON)
			return MACIOERRBB0;
#endif
		break;
	case ADDR_BB1:
#if CHK_BBRF_IO
		if (adapter->sm.bb1_func != MAC_AX_FUNC_ON)
			return MACIOERRBB1;
#endif
		break;
	case ADDR_RF:
#if CHK_BBRF_IO
		if (adapter->sm.bb0_func != MAC_AX_FUNC_ON &&
		    adapter->sm.bb1_func != MAC_AX_FUNC_ON)
			return MACIOERRRF;
#endif
		break;
	case ADDR_IND_ACES:
		if (adapter->hw_info->is_sec_ic) {
			PLTFM_MSG_ERR("[ERR]security mode ind aces\n");
			return MACIOERRIND;
		}

		if (adapter->hw_info->ind_aces_cnt > 1)
			PLTFM_MSG_ERR("[ERR]ind aces cnt %d ovf\n",
				      adapter->hw_info->ind_aces_cnt);
		if (adapter->hw_info->ind_aces_cnt != 1)
			return MACIOERRIND;
		break;
	case ADDR_RSVD:
		return MACIOERRRSVD;
	case ADDR_INVALID:
		return MACHWNOSUP;
	}

	if (adapter->sm.pwr != MAC_AX_PWR_ON)
		return MACIOERRPWR;

	if (adapter->sm.plat != MAC_AX_PLAT_ON)
		return MACIOERRPLAT;

	if (adapter->sm.io_st == MAC_AX_IO_ST_HANG)
		return MACIOERRISH;

	if ((adapter->sm.fw_rst == MAC_AX_FW_RESET_RECV_DONE ||
	     adapter->sm.fw_rst == MAC_AX_FW_RESET_PROCESS) &&
	    ADDR_NOT_ALLOW_SERL1(offset))
		return MACIOERRSERL1;

	if (adapter->sm.fw_rst == MAC_AX_FW_RESET_IDLE &&
	    adapter->mac_pwr_info.pwr_in_lps && ADDR_NOT_ALLOW_LPS(offset))
		return MACIOERRLPS;

	return MACSUCCESS;
}

u32 mac_get_bt_dis(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	return !!(MAC_REG_R32(R_AX_WL_BT_PWR_CTRL) & B_AX_BT_DISN_EN);
}

u32 mac_set_bt_dis(struct mac_ax_adapter *adapter, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val;

	val = MAC_REG_R32(R_AX_WL_BT_PWR_CTRL);
	val = en ? val | B_AX_BT_DISN_EN : val & ~B_AX_BT_DISN_EN;
	MAC_REG_W32(R_AX_WL_BT_PWR_CTRL, val);

	return MACSUCCESS;
}

u32 mac_watchdog(struct mac_ax_adapter *adapter,
		 struct mac_ax_wdt_param *wdt_param)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_drv_wdt_ctrl *ctrl_def = &wdt_ctrl_def;
	u32 ret = MACSUCCESS;

#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if ((is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	     is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) &&
	    wdt_param->drv_ctrl.autok_wdt_ctrl != MAC_AX_PCIE_IGNORE &&
	    (wdt_param->drv_ctrl.autok_wdt_ctrl != MAC_AX_PCIE_DEFAULT ||
	     (ctrl_def->autok_wdt_ctrl != MAC_AX_PCIE_IGNORE &&
	      ctrl_def->autok_wdt_ctrl != MAC_AX_PCIE_DEFAULT))) {
		ret = ops->pcie_autok_counter_avg(adapter);
		if (ret != MACSUCCESS)
			return ret;
	}

	if ((is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	     is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) &&
	    wdt_param->drv_ctrl.tp_wdt_ctrl != MAC_AX_PCIE_IGNORE &&
	    (wdt_param->drv_ctrl.tp_wdt_ctrl != MAC_AX_PCIE_DEFAULT ||
	     (ctrl_def->tp_wdt_ctrl != MAC_AX_PCIE_IGNORE &&
	      ctrl_def->tp_wdt_ctrl != MAC_AX_PCIE_DEFAULT))) {
		ret = ops->tp_adjust(adapter, wdt_param->tp);
		if (ret != MACSUCCESS)
			return ret;
	}
#endif

	return MACSUCCESS;
}

u32 mac_get_freerun(struct mac_ax_adapter *adapter,
		    struct mac_ax_freerun *freerun)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_l, reg_h, ret;

	ret = check_mac_en(adapter, freerun->band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg_l = freerun->band == MAC_AX_BAND_0 ? R_AX_FREERUN_CNT_LOW : R_AX_FREERUN_CNT_LOW_C1;
	reg_h = freerun->band == MAC_AX_BAND_0 ? R_AX_FREERUN_CNT_HIGH : R_AX_FREERUN_CNT_HIGH_C1;
	freerun->freerun_l = MAC_REG_R32(reg_l);
	freerun->freerun_h = MAC_REG_R32(reg_h);

	return MACSUCCESS;
}
