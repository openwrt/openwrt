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
#include "hci_fc_8852c.h"
#include "../../mac_reg.h"

#if MAC_AX_8852C_SUPPORT

u32 set_fc_page_ctrl_reg_8852c(struct mac_ax_adapter *adapter, u8 ch)
{
	struct mac_ax_hfc_ch_cfg *cfg = adapter->hfc_param->ch_cfg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32 = 0;

	switch (ch) {
	case MAC_AX_DMA_ACH0:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH0].min, B_AX_ACH0_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH0].max, B_AX_ACH0_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH0].grp ? B_AX_ACH0_GRP : 0);
		MAC_REG_W32(R_AX_ACH0_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH1:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH1].min, B_AX_ACH1_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH1].max, B_AX_ACH1_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH1].grp ? B_AX_ACH1_GRP : 0);
		MAC_REG_W32(R_AX_ACH1_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH2:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH2].min, B_AX_ACH2_MIN_PG) |
			 SET_WORD(cfg[MAC_AX_DMA_ACH2].max, B_AX_ACH2_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH2].grp ? B_AX_ACH2_GRP : 0);
		MAC_REG_W32(R_AX_ACH2_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH3:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH3].min, B_AX_ACH3_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH3].max, B_AX_ACH3_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH3].grp ? B_AX_ACH3_GRP : 0);
		MAC_REG_W32(R_AX_ACH3_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH4:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH4].min, B_AX_ACH4_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH4].max, B_AX_ACH4_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH4].grp ? B_AX_ACH4_GRP : 0);
		MAC_REG_W32(R_AX_ACH4_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH5:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH5].min, B_AX_ACH5_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH5].max, B_AX_ACH5_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH5].grp ? B_AX_ACH5_GRP : 0);
		MAC_REG_W32(R_AX_ACH5_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH6:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH6].min, B_AX_ACH6_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH6].max, B_AX_ACH6_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH6].grp ? B_AX_ACH6_GRP : 0);
		MAC_REG_W32(R_AX_ACH6_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_ACH7:
		val32 = SET_WORD(cfg[MAC_AX_DMA_ACH7].min, B_AX_ACH7_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_ACH7].max, B_AX_ACH7_MAX_PG) |
			(cfg[MAC_AX_DMA_ACH7].grp ? B_AX_ACH7_GRP : 0);
		MAC_REG_W32(R_AX_ACH7_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_B0MG:
		val32 = SET_WORD(cfg[MAC_AX_DMA_B0MG].min, B_AX_CH8_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_B0MG].max, B_AX_CH8_MAX_PG) |
			(cfg[MAC_AX_DMA_B0MG].grp ? B_AX_CH8_GRP : 0);
		MAC_REG_W32(R_AX_CH8_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_B0HI:
		val32 = SET_WORD(cfg[MAC_AX_DMA_B0HI].min, B_AX_CH9_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_B0HI].max, B_AX_CH9_MAX_PG) |
			(cfg[MAC_AX_DMA_B0HI].grp ? B_AX_CH9_GRP : 0);
		MAC_REG_W32(R_AX_CH9_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_B1MG:
		val32 = SET_WORD(cfg[MAC_AX_DMA_B1MG].min, B_AX_CH10_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_B1MG].max, B_AX_CH10_MAX_PG) |
			(cfg[MAC_AX_DMA_B1MG].grp ? B_AX_CH10_GRP : 0);
		MAC_REG_W32(R_AX_CH10_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_B1HI:
		val32 = SET_WORD(cfg[MAC_AX_DMA_B1HI].min, B_AX_CH11_MIN_PG) |
			SET_WORD(cfg[MAC_AX_DMA_B1HI].max, B_AX_CH11_MAX_PG) |
			(cfg[MAC_AX_DMA_B1HI].grp ? B_AX_CH11_GRP : 0);
		MAC_REG_W32(R_AX_CH11_PAGE_CTRL_V1, val32);
		break;

	case MAC_AX_DMA_H2C:
		break;

	default:
		PLTFM_MSG_ERR("%s wrong input: %d\n", __func__, ch);
		return MACTXCHDMA;
	}

	return MACSUCCESS;
}

u32 get_fc_page_info_8852c(struct mac_ax_adapter *adapter, u8 ch)
{
	struct mac_ax_hfc_ch_info *info = adapter->hfc_param->ch_info;
	struct mac_ax_hfc_ch_cfg *cfg = adapter->hfc_param->ch_cfg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	switch (ch) {
	case MAC_AX_DMA_ACH0:
		val32 = MAC_REG_R32(R_AX_ACH0_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH0].aval =
			GET_FIELD(val32, B_AX_ACH0_AVAL_PG);
		info[MAC_AX_DMA_ACH0].used =
			GET_FIELD(val32, B_AX_ACH0_USE_PG);
		break;

	case MAC_AX_DMA_ACH1:
		val32 = MAC_REG_R32(R_AX_ACH1_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH1].aval =
			GET_FIELD(val32, B_AX_ACH1_AVAL_PG);
		info[MAC_AX_DMA_ACH1].used =
			GET_FIELD(val32, B_AX_ACH1_USE_PG);
		break;

	case MAC_AX_DMA_ACH2:
		val32 = MAC_REG_R32(R_AX_ACH2_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH2].aval =
			GET_FIELD(val32, B_AX_ACH2_AVAL_PG);
		info[MAC_AX_DMA_ACH2].used =
			GET_FIELD(val32, B_AX_ACH2_USE_PG);
		break;

	case MAC_AX_DMA_ACH3:
		val32 = MAC_REG_R32(R_AX_ACH3_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH3].aval =
			GET_FIELD(val32, B_AX_ACH3_AVAL_PG);
		info[MAC_AX_DMA_ACH3].used =
			GET_FIELD(val32, B_AX_ACH3_USE_PG);
		break;

	case MAC_AX_DMA_ACH4:
		val32 = MAC_REG_R32(R_AX_ACH4_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH4].aval =
			GET_FIELD(val32, B_AX_ACH4_AVAL_PG);
		info[MAC_AX_DMA_ACH4].used =
			GET_FIELD(val32, B_AX_ACH4_USE_PG);
		break;

	case MAC_AX_DMA_ACH5:
		val32 = MAC_REG_R32(R_AX_ACH5_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH5].aval =
			GET_FIELD(val32, B_AX_ACH5_AVAL_PG);
		info[MAC_AX_DMA_ACH5].used =
			GET_FIELD(val32, B_AX_ACH5_USE_PG);
		break;

	case MAC_AX_DMA_ACH6:
		val32 = MAC_REG_R32(R_AX_ACH6_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH6].aval =
			GET_FIELD(val32, B_AX_ACH6_AVAL_PG);
		info[MAC_AX_DMA_ACH6].used =
			GET_FIELD(val32, B_AX_ACH6_USE_PG);
		break;

	case MAC_AX_DMA_ACH7:
		val32 = MAC_REG_R32(R_AX_ACH7_PAGE_INFO_V1);
		info[MAC_AX_DMA_ACH7].aval =
			GET_FIELD(val32, B_AX_ACH7_AVAL_PG);
		info[MAC_AX_DMA_ACH7].used =
			GET_FIELD(val32, B_AX_ACH7_USE_PG);
		break;

	case MAC_AX_DMA_B0MG:
		val32 = MAC_REG_R32(R_AX_CH8_PAGE_INFO_V1);
		info[MAC_AX_DMA_B0MG].aval =
			GET_FIELD(val32, B_AX_CH8_AVAL_PG);
		info[MAC_AX_DMA_B0MG].used =
			GET_FIELD(val32, B_AX_CH8_USE_PG);
		break;

	case MAC_AX_DMA_B0HI:
		val32 = MAC_REG_R32(R_AX_CH9_PAGE_INFO_V1);
		info[MAC_AX_DMA_B0HI].aval =
			GET_FIELD(val32, B_AX_CH9_AVAL_PG);
		info[MAC_AX_DMA_B0HI].used =
			GET_FIELD(val32, B_AX_CH9_USE_PG);
		break;

	case MAC_AX_DMA_B1MG:
		val32 = MAC_REG_R32(R_AX_CH10_PAGE_INFO_V1);
		info[MAC_AX_DMA_B1MG].aval =
			GET_FIELD(val32, B_AX_CH10_AVAL_PG);
		info[MAC_AX_DMA_B1MG].used =
			GET_FIELD(val32, B_AX_CH10_USE_PG);
		break;

	case MAC_AX_DMA_B1HI:
		val32 = MAC_REG_R32(R_AX_CH11_PAGE_INFO_V1);
		info[MAC_AX_DMA_B1HI].aval =
			GET_FIELD(val32, B_AX_CH11_AVAL_PG);
		info[MAC_AX_DMA_B1HI].used =
			GET_FIELD(val32, B_AX_CH11_USE_PG);
		break;

	case MAC_AX_DMA_H2C:
		val32 = MAC_REG_R32(R_AX_CH12_PAGE_INFO_V1);
		info[MAC_AX_DMA_H2C].aval =
			GET_FIELD(val32, B_AX_CH12_AVAL_PG);
		info[MAC_AX_DMA_H2C].used =
			cfg[MAC_AX_DMA_H2C].min - info[MAC_AX_DMA_H2C].aval;
		break;

	default:
		PLTFM_MSG_ERR("%s wrong input: %d\n", __func__, ch);
		return MACTXCHDMA;
	}

	return MACSUCCESS;
}

u32 set_fc_pubpg_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hfc_pub_cfg *cfg = adapter->hfc_param->pub_cfg;
	u32 val32;

	val32 = SET_WORD(cfg->group0, B_AX_PUBPG_G0) |
		SET_WORD(cfg->group1, B_AX_PUBPG_G1);
	MAC_REG_W32(R_AX_PUB_PAGE_CTRL1_V1, val32);

	val32 = SET_WORD(cfg->wp_thrd, B_AX_WP_THRD);
	MAC_REG_W32(R_AX_WP_PAGE_CTRL2_V1, val32);

	return MACSUCCESS;
}

u32 get_fc_mix_info_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hfc_param *param = adapter->hfc_param;
	struct mac_ax_hfc_pub_cfg *pub_cfg = param->pub_cfg;
	struct mac_ax_hfc_prec_cfg *prec_cfg = param->prec_cfg;
	struct mac_ax_hfc_pub_info *info = param->pub_info;
	u32 val32;

	val32 = MAC_REG_R32(R_AX_PUB_PAGE_INFO1_V1);
	info->g0_used = GET_FIELD(val32, B_AX_G0_USE_PG);
	info->g1_used = GET_FIELD(val32, B_AX_G1_USE_PG);

	val32 = MAC_REG_R32(R_AX_PUB_PAGE_INFO3_V1);
	info->g0_aval = GET_FIELD(val32, B_AX_G0_AVAL_PG);
	info->g1_aval = GET_FIELD(val32, B_AX_G1_AVAL_PG);
	info->pub_aval =
		GET_FIELD(MAC_REG_R32(R_AX_PUB_PAGE_INFO2_V1), B_AX_PUB_AVAL_PG);
	info->wp_aval =
		GET_FIELD(MAC_REG_R32(R_AX_WP_PAGE_INFO1_V1), B_AX_WP_AVAL_PG);

	val32 = MAC_REG_R32(R_AX_HCI_FC_CTRL_V1);
	param->en = val32 & B_AX_HCI_FC_EN ? 1 : 0;
	param->h2c_en = val32 & B_AX_HCI_FC_CH12_EN ? 1 : 0;
	param->mode = GET_FIELD(val32, B_AX_HCI_FC_MODE);
	prec_cfg->ch011_full_cond = GET_FIELD(val32, B_AX_HCI_FC_WD_FULL_COND);
	prec_cfg->h2c_full_cond = GET_FIELD(val32, B_AX_HCI_FC_CH12_FULL_COND);
	prec_cfg->wp_ch07_full_cond =
		GET_FIELD(val32, B_AX_HCI_FC_WP_CH07_FULL_COND);
	prec_cfg->wp_ch811_full_cond =
		GET_FIELD(val32, B_AX_HCI_FC_WP_CH811_FULL_COND);

	val32 = MAC_REG_R32(R_AX_CH_PAGE_CTRL_V1);
	prec_cfg->ch011_prec = GET_FIELD(val32, B_AX_PREC_PAGE_CH011);
	prec_cfg->h2c_prec = GET_FIELD(val32, B_AX_PREC_PAGE_CH12);

	val32 = MAC_REG_R32(R_AX_PUB_PAGE_CTRL2_V1);
	pub_cfg->pub_max = GET_FIELD(val32, B_AX_PUBPG_ALL);

	val32 = MAC_REG_R32(R_AX_WP_PAGE_CTRL1_V1);
	prec_cfg->wp_ch07_prec = GET_FIELD(val32, B_AX_PREC_PAGE_WP_CH07);
	prec_cfg->wp_ch811_prec = GET_FIELD(val32, B_AX_PREC_PAGE_WP_CH811);

	val32 = MAC_REG_R32(R_AX_WP_PAGE_CTRL2_V1);
	pub_cfg->wp_thrd = GET_FIELD(val32, B_AX_WP_THRD);

	val32 = MAC_REG_R32(R_AX_PUB_PAGE_CTRL1_V1);
	pub_cfg->group0 = GET_FIELD(val32, B_AX_PUBPG_G0);
	pub_cfg->group1 = GET_FIELD(val32, B_AX_PUBPG_G1);

	return MACSUCCESS;
}

u32 set_fc_h2c_8852c(struct mac_ax_adapter *adapter)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hfc_param *param = adapter->hfc_param;
	struct mac_ax_hfc_prec_cfg *prec_cfg = param->prec_cfg;

	val32 = SET_WORD(prec_cfg->h2c_prec, B_AX_PREC_PAGE_CH12);

	MAC_REG_W32(R_AX_CH_PAGE_CTRL_V1, val32);

	val32 = SET_CLR_WORD(MAC_REG_R32(R_AX_HCI_FC_CTRL_V1),
			     prec_cfg->h2c_full_cond,
			     B_AX_HCI_FC_CH12_FULL_COND);
	MAC_REG_W32((R_AX_HCI_FC_CTRL_V1), val32);

	return MACSUCCESS;
}

u32 set_fc_mix_cfg_8852c(struct mac_ax_adapter *adapter)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hfc_param *param = adapter->hfc_param;
	struct mac_ax_hfc_pub_cfg *pub_cfg = param->pub_cfg;
	struct mac_ax_hfc_prec_cfg *prec_cfg = param->prec_cfg;

	val32 = SET_WORD(prec_cfg->ch011_prec, B_AX_PREC_PAGE_CH011) |
		SET_WORD(prec_cfg->h2c_prec, B_AX_PREC_PAGE_CH12);
	MAC_REG_W32(R_AX_CH_PAGE_CTRL_V1, val32);

	val32 = SET_WORD(pub_cfg->pub_max, B_AX_PUBPG_ALL);
	MAC_REG_W32(R_AX_PUB_PAGE_CTRL2_V1, val32);

	val32 = SET_WORD(prec_cfg->wp_ch07_prec, B_AX_PREC_PAGE_WP_CH07) |
		SET_WORD(prec_cfg->wp_ch811_prec, B_AX_PREC_PAGE_WP_CH811);
	MAC_REG_W32(R_AX_WP_PAGE_CTRL1_V1, val32);

	val32 = SET_CLR_WORD(MAC_REG_R32(R_AX_HCI_FC_CTRL_V1),
			     param->mode, B_AX_HCI_FC_MODE);
	val32 = SET_CLR_WORD(val32, prec_cfg->ch011_full_cond,
			     B_AX_HCI_FC_WD_FULL_COND);
	val32 = SET_CLR_WORD(val32, prec_cfg->h2c_full_cond,
			     B_AX_HCI_FC_CH12_FULL_COND);
	val32 = SET_CLR_WORD(val32, prec_cfg->wp_ch07_full_cond,
			     B_AX_HCI_FC_WP_CH07_FULL_COND);
	val32 = SET_CLR_WORD(val32, prec_cfg->wp_ch811_full_cond,
			     B_AX_HCI_FC_WP_CH811_FULL_COND);
	MAC_REG_W32(R_AX_HCI_FC_CTRL_V1, val32);

	return MACSUCCESS;
}

u32 set_fc_func_en_8852c(struct mac_ax_adapter *adapter, u8 en, u8 h2c_en)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_hfc_param *param = adapter->hfc_param;

	val32 = MAC_REG_R32(R_AX_HCI_FC_CTRL_V1);
	param->en = en ? 1 : 0;
	param->h2c_en = h2c_en ? 1 : 0;
	val32 = en ? (val32 | B_AX_HCI_FC_EN) : (val32 & ~B_AX_HCI_FC_EN);
	val32 = h2c_en ? (val32 | B_AX_HCI_FC_CH12_EN) :
			 (val32 & ~B_AX_HCI_FC_CH12_EN);
	MAC_REG_W32(R_AX_HCI_FC_CTRL_V1, val32);

	return MACSUCCESS;
}

#endif /* #if MAC_AX_8852C_SUPPORT */

