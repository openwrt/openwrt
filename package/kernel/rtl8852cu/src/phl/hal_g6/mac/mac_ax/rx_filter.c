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
#include "rx_filter.h"

/* control frames: drop, mgnt frames: to host, data frame: to host */
static enum mac_ax_fwd_target trxcfg_rx_fltr_mgnt_frame = MAC_AX_FWD_TO_HOST;
static enum mac_ax_fwd_target trxcfg_rx_fltr_ctrl_frame = MAC_AX_FWD_TO_HOST;
static enum mac_ax_fwd_target trxcfg_rx_fltr_data_frame = MAC_AX_FWD_TO_HOST;

static struct mac_ax_rx_fltr_ctrl_t rx_fltr_init_opt = {
	1, /* sniffer_mode */
	1, /* acpt_a1_match_pkt */
	1, /* acpt_bc_pkt */
	1, /* acpt_mc_pkt */
	1, /* uc_pkt_chk_cam_match */
	1, /* bc_pkt_chk_cam_match */
	0, /* mc_pkt_white_lst_mode */
	0, /* bcn_chk_en */
	0, /* bcn_chk_rule */
	0, /* acpt_pwr_mngt_pkt */
	0, /* acpt_crc32_err_pkt */
	0, /* acpt_unsupport_pkt */
	0, /* acpt_mac_hdr_content_err_pkt */
	0, /* acpt_ftm_req_pkt */
	0, /* rsvd */
	0, /* pkt_len_fltr */
	0, /* unsp_pkt_target */
	8, /* uid_fltr */
	0, /* rsvd */
	1, /* cck_crc_chk_enable */
	1, /* cck_sig_chk_enable */
	1, /* lsig_parity_chk_enable */
	1, /* siga_crc_chk_enable */
	1, /* vht_su_sigb_crc_chk_enable */
	1, /* vht_mu_sigb_crc_chk_enable */
	1, /* he_sigb_crc_chk_enable */
	0, /* min_len_chk_disable */
};

u32 rx_fltr_init(struct mac_ax_adapter *adapter, enum mac_ax_band band)
{
	u32 ret;
	struct mac_ax_ops *mac_ax_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_rx_fltr_ctrl_t opt_msk = {
		0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0x1, 0x1, 0x1,
		0x1, 0x1, 0x1, 0x3F, 0x3, 0xf, 0xf, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
		0x1, 0x1
	};

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	ret = mac_ax_ops->set_rx_fltr_typ_opt(adapter,
					      MAC_AX_PKT_MGNT,
					      trxcfg_rx_fltr_mgnt_frame,
					      band);
	if (ret != MACSUCCESS)
		return ret;
	ret = mac_ax_ops->set_rx_fltr_typ_opt(adapter,
					      MAC_AX_PKT_CTRL,
					      trxcfg_rx_fltr_ctrl_frame,
					      band);
	if (ret != MACSUCCESS)
		return ret;
	ret = mac_ax_ops->set_rx_fltr_typ_opt(adapter,
					      MAC_AX_PKT_DATA,
					      trxcfg_rx_fltr_data_frame,
					      band);
	if (ret != MACSUCCESS)
		return ret;
	ret = mac_ax_ops->set_rx_fltr_opt(adapter,
					  &rx_fltr_init_opt,
					  &opt_msk,
					  band);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

static void rx_fltr_opt_2_uint(struct mac_ax_adapter *adapter,
			       struct mac_ax_rx_fltr_ctrl_t *fltr_opt,
			       u32 *mac_fltr_value,
			       u16 *plcp_fltr_value)
{
	u32 val32 = 0x0;
	u8 val16 = 0x0;

	val32 =
		((fltr_opt->sniffer_mode) ?
				B_AX_SNIFFER_MODE : 0) |
		((fltr_opt->acpt_a1_match_pkt) ?
				B_AX_A_A1_MATCH : 0) |
		((fltr_opt->acpt_bc_pkt) ?
				B_AX_A_BC : 0) |
		((fltr_opt->acpt_mc_pkt) ?
				B_AX_A_MC : 0) |
		((fltr_opt->uc_pkt_chk_cam_match) ?
				B_AX_A_UC_CAM_MATCH : 0) |
		((fltr_opt->bc_pkt_chk_cam_match) ?
				B_AX_A_BC_CAM_MATCH : 0) |
		((fltr_opt->mc_pkt_white_lst_mode) ?
				B_AX_A_MC_LIST_CAM_MATCH : 0) |
		((fltr_opt->bcn_chk_en) ?
				B_AX_A_BCN_CHK_EN : 0) |
		((fltr_opt->acpt_pwr_mngt_pkt) ?
				B_AX_A_PWR_MGNT : 0) |
		((fltr_opt->acpt_crc32_err_pkt) ?
				B_AX_A_CRC32_ERR : 0) |
		((fltr_opt->acpt_mac_hdr_content_err_pkt) ?
				B_AX_A_ERR_PKT : 0) |
		((fltr_opt->acpt_unsupport_pkt) ?
				B_AX_A_UNSUP_PKT : 0) |
		((fltr_opt->acpt_ftm_req_pkt) ?
				B_AX_A_FTM_REQ : 0) |
		SET_WORD(fltr_opt->bcn_chk_rule,
			 RX_FLTR_BCN_CHK_RULE) |
		SET_WORD(fltr_opt->pkt_len_fltr,
			 RX_FLTR_PKT_LEN_FLTR) |
		SET_WORD(fltr_opt->unsp_pkt_target,
			 RX_FLTR_UNSUPPORT_PKT_FLTR) |
		SET_WORD(fltr_opt->uid_fltr,
			 RX_FLTR_UID_FLTR);

	*mac_fltr_value = val32;

	val16 =
		((fltr_opt->cck_crc_chk_enable) ?
				B_AX_CCK_CRC_CHK : 0) |
		((fltr_opt->cck_sig_chk_enable) ?
				B_AX_CCK_SIG_CHK : 0) |
		((fltr_opt->lsig_parity_chk_enable) ?
				B_AX_LSIG_PARITY_CHK_EN : 0) |
		((fltr_opt->siga_crc_chk_enable) ?
				B_AX_SIGA_CRC_CHK : 0) |
		((fltr_opt->vht_su_sigb_crc_chk_enable) ?
				B_AX_VHT_SU_SIGB_CRC_CHK : 0) |
		((fltr_opt->vht_mu_sigb_crc_chk_enable) ?
				B_AX_VHT_MU_SIGB_CRC_CHK : 0) |
		((fltr_opt->he_sigb_crc_chk_enable) ?
				B_AX_HE_SIGB_CRC_CHK : 0) |
		((fltr_opt->min_len_chk_disable) ?
				B_AX_DIS_CHK_MIN_LEN : 0);

	*plcp_fltr_value = val16;
}

u32 mac_get_rx_fltr_opt(struct mac_ax_adapter *adapter,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt,
			enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	if (!fltr_opt) {
		PLTFM_MSG_ERR("[ERR]%s opt is null\n", __func__);
		return MACNPTR;
	}

	val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
			    R_AX_RX_FLTR_OPT_C1 : R_AX_RX_FLTR_OPT);
	fltr_opt->sniffer_mode = ((val32 & B_AX_SNIFFER_MODE) != 0);
	fltr_opt->acpt_a1_match_pkt = ((val32 & B_AX_A_A1_MATCH) != 0);
	fltr_opt->acpt_bc_pkt = ((val32 & B_AX_A_BC) != 0);
	fltr_opt->acpt_mc_pkt = ((val32 & B_AX_A_MC) != 0);
	fltr_opt->uc_pkt_chk_cam_match = ((val32 & B_AX_A_UC_CAM_MATCH) != 0);
	fltr_opt->bc_pkt_chk_cam_match = ((val32 & B_AX_A_BC_CAM_MATCH) != 0);
	fltr_opt->mc_pkt_white_lst_mode = ((val32 &
					   B_AX_A_MC_LIST_CAM_MATCH) != 0);
	fltr_opt->bcn_chk_en = ((val32 & B_AX_A_BCN_CHK_EN) != 0);
	fltr_opt->acpt_pwr_mngt_pkt = ((val32 & B_AX_A_PWR_MGNT) != 0);
	fltr_opt->acpt_crc32_err_pkt = ((val32 & B_AX_A_CRC32_ERR) != 0);
	fltr_opt->acpt_mac_hdr_content_err_pkt = ((val32 &
						  B_AX_A_ERR_PKT) != 0);
	fltr_opt->acpt_unsupport_pkt = ((val32 & B_AX_A_UNSUP_PKT) != 0);
	fltr_opt->acpt_ftm_req_pkt = ((val32 & B_AX_A_FTM_REQ) != 0);
	fltr_opt->bcn_chk_rule = GET_FIELD(val32, RX_FLTR_BCN_CHK_RULE);
	fltr_opt->pkt_len_fltr = GET_FIELD(val32, RX_FLTR_PKT_LEN_FLTR);
	fltr_opt->unsp_pkt_target = GET_FIELD(val32,
					      RX_FLTR_UNSUPPORT_PKT_FLTR);
	fltr_opt->uid_fltr = GET_FIELD(val32, RX_FLTR_UID_FLTR);

	val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
			    R_AX_PLCP_HDR_FLTR_C1 : R_AX_PLCP_HDR_FLTR);
	fltr_opt->cck_crc_chk_enable = ((val32 & B_AX_CCK_CRC_CHK) != 0);
	fltr_opt->cck_sig_chk_enable = ((val32 & B_AX_CCK_SIG_CHK) != 0);
	fltr_opt->lsig_parity_chk_enable = ((val32 &
					 B_AX_LSIG_PARITY_CHK_EN) != 0);
	fltr_opt->siga_crc_chk_enable = ((val32 & B_AX_SIGA_CRC_CHK) != 0);
	fltr_opt->vht_su_sigb_crc_chk_enable = ((val32 &
						 B_AX_VHT_SU_SIGB_CRC_CHK)
						 != 0);
	fltr_opt->vht_mu_sigb_crc_chk_enable = ((val32 &
						 B_AX_VHT_MU_SIGB_CRC_CHK)
						 != 0);
	fltr_opt->he_sigb_crc_chk_enable = ((val32 & B_AX_HE_SIGB_CRC_CHK)
					    != 0);
	fltr_opt->min_len_chk_disable = ((val32 & B_AX_DIS_CHK_MIN_LEN)
					 != 0);
	return MACSUCCESS;
}

u32 mac_set_rx_fltr_opt(struct mac_ax_adapter *adapter,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt,
			struct mac_ax_rx_fltr_ctrl_t *fltr_opt_msk,
			enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_rx_fltr_ctrl_t opt = {0};
	u32 mac_opt;
	u16 plcp_opt;
	u32 mac_opt_msk;
	u16 plcp_opt_msk;
	u32 mac_opt_value;
	u16 plcp_opt_value;
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return ret;
	}

	if (!fltr_opt || !fltr_opt_msk) {
		PLTFM_MSG_ERR("[ERR]%s opt is null\n", __func__);
		return MACNPTR;
	}

	mac_get_rx_fltr_opt(adapter, &opt, band);

	rx_fltr_opt_2_uint(adapter,
			   fltr_opt,
			   &mac_opt,
			   &plcp_opt);

	rx_fltr_opt_2_uint(adapter,
			   fltr_opt_msk,
			   &mac_opt_msk,
			   &plcp_opt_msk);

	rx_fltr_opt_2_uint(adapter,
			   &opt,
			   &mac_opt_value,
			   &plcp_opt_value);

	mac_opt_value = (mac_opt & mac_opt_msk) |
			(~(~mac_opt & mac_opt_msk) & mac_opt_value);

	plcp_opt_value = (plcp_opt & plcp_opt_msk) |
			(~(~plcp_opt & plcp_opt_msk) & plcp_opt_value);

	// add fw offload later
	if (band == MAC_AX_BAND_0) {
		MAC_REG_W32(R_AX_RX_FLTR_OPT, mac_opt_value);
		MAC_REG_W16(R_AX_PLCP_HDR_FLTR, plcp_opt_value);
	} else {
		MAC_REG_W32(R_AX_RX_FLTR_OPT_C1, mac_opt_value);
		MAC_REG_W16(R_AX_PLCP_HDR_FLTR_C1, plcp_opt_value);
	}

	return ret;
}

u32 mac_set_typ_fltr_opt(struct mac_ax_adapter *adapter,
			 enum mac_ax_pkt_t type,
			 enum mac_ax_fwd_target fwd_target,
			 enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	switch (fwd_target) {
	case MAC_AX_FWD_DONT_CARE:
		val32 = RX_FLTR_FRAME_DROP;
		break;
	case MAC_AX_FWD_TO_HOST:
		val32 = RX_FLTR_FRAME_TO_HOST;
		break;
	case MAC_AX_FWD_TO_WLAN_CPU:
		val32 = RX_FLTR_FRAME_TO_WLCPU;
		break;
	default:
		PLTFM_MSG_ERR("[ERR]%s fwd target err\n", __func__);
		return MACNOITEM;
	}

	switch (type) {
	case MAC_AX_PKT_MGNT:
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_MGNT_FLTR_C1 : R_AX_MGNT_FLTR, val32);
		break;
	case MAC_AX_PKT_CTRL:
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_CTRL_FLTR_C1 : R_AX_CTRL_FLTR, val32);
		break;
	case MAC_AX_PKT_DATA:
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_DATA_FLTR_C1 : R_AX_DATA_FLTR, val32);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]%s type err\n", __func__);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_set_typsbtyp_fltr_opt(struct mac_ax_adapter *adapter,
			      enum mac_ax_pkt_t type,
			      u8 subtype,
			      enum mac_ax_fwd_target fwd_target,
			      enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	if (fwd_target != MAC_AX_FWD_DONT_CARE &&
	    fwd_target != MAC_AX_FWD_TO_HOST &&
	    fwd_target != MAC_AX_FWD_TO_WLAN_CPU) {
		PLTFM_MSG_ERR("[ERR]%s fwd target err\n", __func__);
		return MACNOITEM;
	}

	switch (type) {
	case MAC_AX_PKT_MGNT:
		val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
				    R_AX_MGNT_FLTR_C1 : R_AX_MGNT_FLTR);
		val32 = SET_CLR_WOR2(val32, fwd_target,
				     ((subtype & 0xf) * 2), 0x3);
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_MGNT_FLTR_C1 : R_AX_MGNT_FLTR, val32);
		break;
	case MAC_AX_PKT_CTRL:
		val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
				    R_AX_CTRL_FLTR_C1 : R_AX_CTRL_FLTR);
		val32 = SET_CLR_WOR2(val32, fwd_target,
				     ((subtype & 0xf) * 2), 0x3);
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_CTRL_FLTR_C1 : R_AX_CTRL_FLTR, val32);
		break;
	case MAC_AX_PKT_DATA:
		val32 = MAC_REG_R32((band == MAC_AX_BAND_1) ?
				    R_AX_DATA_FLTR_C1 : R_AX_DATA_FLTR);
		val32 = SET_CLR_WOR2(val32, fwd_target,
				     ((subtype & 0xf) * 2), 0x3);
		MAC_REG_W32((band == MAC_AX_BAND_1) ?
			    R_AX_DATA_FLTR_C1 : R_AX_DATA_FLTR, val32);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]%s type err\n", __func__);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_set_typsbtyp_fltr_detail(struct mac_ax_adapter *adapter,
				 enum mac_ax_pkt_t type,
				 struct mac_ax_rx_fltr_elem *elem,
				 enum mac_ax_band band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 cmp_bit = 1;
	u16 reg;
	u8 idx;

	val32 = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, band);
		return val32;
	}

	switch (type) {
	case MAC_AX_PKT_CTRL:
		reg = (band == MAC_AX_BAND_0) ? R_AX_CTRL_FLTR : R_AX_CTRL_FLTR_C1;
		break;
	case MAC_AX_PKT_MGNT:
		reg = (band == MAC_AX_BAND_0) ? R_AX_MGNT_FLTR : R_AX_MGNT_FLTR_C1;
		break;
	case MAC_AX_PKT_DATA:
		reg = (band == MAC_AX_BAND_0) ? R_AX_DATA_FLTR : R_AX_DATA_FLTR_C1;
		break;
	default:
		PLTFM_MSG_ERR("[ERR]%s type err\n", __func__);
		return MACNOITEM;
	}

	val32 = MAC_REG_R32(reg);

	for (idx = 0; idx < RX_FLTR_SUBTYPE_NUM; idx++) {
		if ((elem->subtype_mask & cmp_bit) != 0)
			val32 = SET_CLR_WOR2(val32, elem->target_arr[idx],
					     ENTRY_SH(idx), ENTRY_MSK);
		cmp_bit = cmp_bit << ENTRY_IDX;
	}
	MAC_REG_W32(reg, val32);

	return MACSUCCESS;
}