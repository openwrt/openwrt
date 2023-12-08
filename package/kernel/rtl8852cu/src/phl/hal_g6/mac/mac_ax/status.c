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

#include "status.h"

u32 mac_get_ft_status(struct mac_ax_adapter *adapter,
		      enum mac_ax_feature mac_ft, enum mac_ax_status *stat,
		      u8 *buf, const u32 size, u32 *ret_size)

{
	struct mac_ax_ft_status *ft_stat;

	ft_stat = mac_get_ft(adapter, mac_ft);

	if (!ft_stat)
		return MACNOITEM;

	if (!buf)
		return MACNPTR;

	*stat = ft_stat->status;
	if (ft_stat->size) {
		if (size < ft_stat->size)
			return MACNOBUF;
		PLTFM_MEMCMP(buf, ft_stat->buf, ft_stat->size);
		*ret_size = ft_stat->size;
	}

	return MACSUCCESS;
}

struct mac_ax_ft_status *mac_get_ft(struct mac_ax_adapter *adapter,
				    enum mac_ax_feature mac_ft)
{
	struct mac_ax_ft_status *ft_stat = adapter->ft_stat;

	for (; ft_stat->mac_ft != MAC_AX_FT_MAX; ft_stat++) {
		if (ft_stat->mac_ft == mac_ft)
			return ft_stat;
	}

	PLTFM_MSG_ERR("The mac feature is not supported\n");

	return NULL;
}

u32 set_hw_ch_busy_cnt(struct mac_ax_adapter *adapter,
		       struct mac_ax_ch_busy_cnt_cfg *cfg)
{
	u8 band;
	u32 ret;
	u32 reg_addr;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	band = cfg->band;
	reg_addr = (band ? R_AX_PTCL_ATM_C1 : R_AX_PTCL_ATM);

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	switch (cfg->cnt_ctrl) {
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_EN:
		MAC_REG_W32(reg_addr,
			    MAC_REG_R32(reg_addr) | B_AX_CHNL_INFO_EN);
		break;
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_DIS:
		MAC_REG_W32(reg_addr,
			    MAC_REG_R32(reg_addr) & ~B_AX_CHNL_INFO_EN);
		break;
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_BUSY_RST:
		val32 = MAC_REG_R32(reg_addr);
		MAC_REG_W32(reg_addr, val32 | B_AX_RST_CHNL_BUSY);
		MAC_REG_W32(reg_addr, val32 & ~B_AX_RST_CHNL_BUSY);
		break;
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_IDLE_RST:
		val32 = MAC_REG_R32(reg_addr);
		MAC_REG_W32(reg_addr, val32 | B_AX_RST_CHNL_IDLE);
		MAC_REG_W32(reg_addr, val32 & ~B_AX_RST_CHNL_IDLE);
		break;
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_RST:
		val32 = MAC_REG_R32(reg_addr);
		MAC_REG_W32(reg_addr, val32 |
			    (B_AX_RST_CHNL_IDLE | B_AX_RST_CHNL_BUSY));
		MAC_REG_W32(reg_addr, val32 &
			    ~(B_AX_RST_CHNL_IDLE | B_AX_RST_CHNL_BUSY));
		break;
	case MAC_AX_CH_BUSY_CNT_CTRL_CNT_REF:
		val32 = MAC_REG_R32(reg_addr) &
			~(B_AX_CHNL_REF_RX_BASIC_NAV |
			  B_AX_CHNL_REF_RX_INTRA_NAV |
			  B_AX_CHNL_REF_DATA_ON |
			  B_AX_CHNL_REF_EDCCA_P20 |
			  B_AX_CHNL_REF_CCA_P20 |
			  B_AX_CHNL_REF_CCA_S20 |
			  B_AX_CHNL_REF_CCA_S40 |
			  B_AX_CHNL_REF_CCA_S80 |
			  B_AX_CHNL_REF_PHY_TXON);
		val32 |= (cfg->ref.basic_nav ? B_AX_CHNL_REF_RX_BASIC_NAV : 0) |
			(cfg->ref.intra_nav ? B_AX_CHNL_REF_RX_INTRA_NAV : 0) |
			(cfg->ref.data_on ? B_AX_CHNL_REF_DATA_ON : 0) |
			(cfg->ref.edcca_p20 ? B_AX_CHNL_REF_EDCCA_P20 : 0) |
			(cfg->ref.cca_p20 ? B_AX_CHNL_REF_CCA_P20 : 0) |
			(cfg->ref.cca_s20 ? B_AX_CHNL_REF_CCA_S20 : 0) |
			(cfg->ref.cca_s40 ? B_AX_CHNL_REF_CCA_S40 : 0) |
			(cfg->ref.cca_s80 ? B_AX_CHNL_REF_CCA_S80 : 0) |
			(cfg->ref.phy_txon ? B_AX_CHNL_REF_PHY_TXON : 0);
		MAC_REG_W32(reg_addr, val32);
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 get_hw_ch_stat_cnt(struct mac_ax_adapter *adapter,
		       struct mac_ax_ch_stat_cnt *cnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 band;
	u32 ret;

	band = cnt->band;
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	cnt->busy_cnt = MAC_REG_R32(band ? R_AX_CHNL_BUSY_TIME_0_C1 :
				    R_AX_CHNL_BUSY_TIME_0);
	cnt->idle_cnt = MAC_REG_R32(band ? R_AX_CHNL_IDLE_TIME_0_C1 :
				    R_AX_CHNL_IDLE_TIME_0);

	return MACSUCCESS;
}

