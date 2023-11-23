/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#include "phl_headers.h"

static void
_phl_sw_cap_para_init(
	struct rtw_phl_com_t* phl_com, struct rtw_para_info_t *para_info)
{
	para_info->para_src = RTW_PARA_SRC_INTNAL;
	para_info->para_data = NULL;
	para_info->para_data_len = 0;
}

static void
_phl_sw_cap_para_free(
	struct rtw_phl_com_t* phl_com, struct rtw_para_info_t *para_info)
{
	u32 buf_sz = MAX_HWCONFIG_FILE_CONTENT;
	void *drv = phl_com->drv_priv;

	if(para_info->para_data)
		_os_mem_free(drv, para_info->para_data, buf_sz * sizeof(u32));

	para_info->para_data = NULL;
	para_info->para_data_len = 0;
}

static void
_phl_pwrlmt_para_init(
	struct rtw_phl_com_t* phl_com, struct rtw_para_pwrlmt_info_t *para_info)
{
	para_info->para_src = RTW_PARA_SRC_INTNAL;
	para_info->para_data = NULL;
	para_info->para_data_len = 0;
	para_info->ext_regd_arridx = 0;
	para_info->ext_reg_map_num = 0;
}

static void
_phl_pwrlmt_para_free(
	struct rtw_phl_com_t* phl_com, struct rtw_para_pwrlmt_info_t *para_info)
{
	u32 file_buf_sz = MAX_HWCONFIG_FILE_CONTENT;
	u32 buf_sz = MAX_LINES_HWCONFIG_TXT;
	void *drv = phl_com->drv_priv;

	if(para_info->para_data)
		_os_mem_free(drv, para_info->para_data, file_buf_sz * sizeof(u32));
	para_info->para_data = NULL;
	para_info->para_data_len = 0;

	if(para_info->ext_reg_codemap)
		_os_mem_free(drv, para_info->ext_reg_codemap, buf_sz * sizeof(u8));
	para_info->ext_reg_codemap = NULL;
	para_info->ext_reg_map_num = 0;
}

enum channel_width _phl_sw_cap_get_hi_bw(struct phy_cap_t *phy_cap)
{
	enum channel_width bw = CHANNEL_WIDTH_20;
	do {
		if (phy_cap->bw_sup & BW_CAP_80_80M) {
			bw = CHANNEL_WIDTH_80_80;
			break;
		} else if (phy_cap->bw_sup & BW_CAP_160M) {
			bw = CHANNEL_WIDTH_160;
			break;
		} else if (phy_cap->bw_sup & BW_CAP_80M) {
			bw = CHANNEL_WIDTH_80;
			break;
		} else if (phy_cap->bw_sup & BW_CAP_40M) {
			bw = CHANNEL_WIDTH_40;
			break;
		} else if (phy_cap->bw_sup & BW_CAP_20M) {
			bw = CHANNEL_WIDTH_20;
			break;
		}
	} while (0);

	return bw;
}

static void _phl_sw_cap_init_para_from_file(struct rtw_phl_com_t* phl_com)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	struct phy_sw_cap_t *phy_sw_cap = NULL;
	u8 idx = 0;

	for (idx = 0; idx < 2 ; idx++) {
		phy_sw_cap = &phl_com->phy_sw_cap[idx];

		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->mac_reg_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->bb_phy_reg_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->bb_phy_reg_mp_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->bb_phy_reg_gain_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->rf_radio_a_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->rf_radio_b_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->rf_txpwr_byrate_info);
		_phl_sw_cap_para_init(phl_com, &phy_sw_cap->rf_txpwrtrack_info);

		_phl_pwrlmt_para_init(phl_com, &phy_sw_cap->rf_txpwrlmt_info);
		_phl_pwrlmt_para_init(phl_com, &phy_sw_cap->rf_txpwrlmt_ru_info);
		_phl_pwrlmt_para_init(phl_com, &phy_sw_cap->rf_txpwrlmt_6g_info);
		_phl_pwrlmt_para_init(phl_com, &phy_sw_cap->rf_txpwrlmt_ru_6g_info);
		phy_sw_cap->bfreed_para = false;
	}

	phl_com->dev_sw_cap.bfree_para_info = false; /* Default keep Phy file param info*/
#endif
}

enum rtw_phl_status
phl_sw_cap_init(struct rtw_phl_com_t* phl_com)
{
	_phl_sw_cap_init_para_from_file(phl_com);

	phl_com->phy_sw_cap[0].band_sup = 0xff;
	phl_com->phy_sw_cap[1].band_sup = 0xff;

	phl_com->dev_sw_cap.fw_cap.fw_src = RTW_FW_SRC_INTNAL;
	phl_com->dev_sw_cap.btc_mode = BTC_MODE_NORMAL;
	phl_com->dev_sw_cap.bypass_rfe_chk = false;
	phl_com->dev_sw_cap.rf_board_opt = PHL_UNDEFINED_SW_CAP;
	phl_com->dev_sw_cap.macid_num = PHL_MACID_MAX_NUM;
	phl_com->dev_sw_cap.band_sup = BAND_CAP_2G | BAND_CAP_5G | BAND_CAP_6G;
	phl_com->dev_sw_cap.bw_sup =
		BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M;
#ifdef CONFIG_PHL_THERMAL_PROTECT
	/* TX duty could be 0~100, 100 means no TX duty control */
	phl_com->dev_sw_cap.min_tx_duty = THERMAL_NO_TX_DUTY_CTRL;
	phl_com->dev_sw_cap.thermal_threshold = THERMAL_NO_SW_THRESHOLD;
#endif
	phl_com->phy_sw_cap[0].txagg_num = 0;
	phl_com->phy_sw_cap[1].txagg_num = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_sw_cap_deinit(struct rtw_phl_com_t* phl_com)
{
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	struct phy_sw_cap_t *phy_sw_cap = NULL;
	u8	idx=0;

	for (idx = 0; idx < 2; idx++) {
		phy_sw_cap = &phl_com->phy_sw_cap[idx];
		if (phy_sw_cap->bfreed_para == true) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "already bfreed para_info->para_data\n");
			return RTW_PHL_STATUS_SUCCESS;
		}
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "To free para_info->para_data phy %d\n", idx);

		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->mac_reg_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->bb_phy_reg_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->bb_phy_reg_mp_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->bb_phy_reg_gain_info);

		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->rf_radio_a_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->rf_radio_b_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->rf_txpwr_byrate_info);
		_phl_sw_cap_para_free(phl_com, &phy_sw_cap->rf_txpwrtrack_info);

		_phl_pwrlmt_para_free(phl_com, &phy_sw_cap->rf_txpwrlmt_info);
		_phl_pwrlmt_para_free(phl_com, &phy_sw_cap->rf_txpwrlmt_ru_info);
		_phl_pwrlmt_para_free(phl_com, &phy_sw_cap->rf_txpwrlmt_6g_info);
		_phl_pwrlmt_para_free(phl_com, &phy_sw_cap->rf_txpwrlmt_ru_6g_info);

		phy_sw_cap->bfreed_para = true;
	}
#endif

	return RTW_PHL_STATUS_SUCCESS;
}

void rtw_phl_init_free_para_buf(struct rtw_phl_com_t *phl_com)
{

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (phl_com->dev_sw_cap.bfree_para_info == true)
		phl_sw_cap_deinit(phl_com);

#endif
}


u16 _phl_sw_role_cap_bf(enum role_type rtype)
{
	u16 def_bf_cap = 0;

	if (rtw_phl_is_ap_category(rtype)) {
		/* AP mode : no MU BFee */
		def_bf_cap = (HW_CAP_BFEE_HT_SU | HW_CAP_BFER_HT_SU |
			      HW_CAP_BFEE_VHT_SU | HW_CAP_BFER_VHT_SU |
			      HW_CAP_BFER_VHT_MU |
			      HW_CAP_BFEE_HE_SU | HW_CAP_BFER_HE_SU |
			      HW_CAP_BFER_HE_MU |
			      HW_CAP_HE_NON_TB_CQI | HW_CAP_HE_TB_CQI);
	} else if (rtw_phl_is_client_category(rtype)) {
		/* STA mode : no MU BFer */
		def_bf_cap = (HW_CAP_BFEE_HT_SU | HW_CAP_BFER_HT_SU |
			      HW_CAP_BFEE_VHT_SU | HW_CAP_BFER_VHT_SU |
			      HW_CAP_BFEE_VHT_MU |
			      HW_CAP_BFEE_HE_SU | HW_CAP_BFER_HE_SU |
			      HW_CAP_BFEE_HE_MU |
			      HW_CAP_HE_NON_TB_CQI | HW_CAP_HE_TB_CQI);
	} else {
		def_bf_cap = (HW_CAP_BFEE_HT_SU | HW_CAP_BFER_HT_SU |
			      HW_CAP_BFEE_VHT_SU | HW_CAP_BFER_VHT_SU |
			      HW_CAP_BFEE_VHT_MU | HW_CAP_BFER_VHT_MU |
			      HW_CAP_BFEE_HE_SU | HW_CAP_BFER_HE_SU |
			      HW_CAP_BFEE_HE_MU | HW_CAP_BFER_HE_MU |
			      HW_CAP_HE_NON_TB_CQI | HW_CAP_HE_TB_CQI);
	}

	return def_bf_cap;
}

static void _phl_init_proto_bf_cap(struct phl_info_t *phl_info,
                                   u8 hw_band,
                                   enum role_type rtype,
                                   struct protocol_cap_t *protocol_cap)
{
#ifdef RTW_WKARD_PHY_CAP
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct role_sw_cap_t *sw_role_cap = &phl_com->role_sw_cap;
	struct protocol_cap_t proto_cap = {0};
	u16 bfcap = sw_role_cap->bf_cap;

	/* First : compare and get the bf sw_proto_cap and hw_proto_cap .*/
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_get_bf_proto_cap(
						phl_com,
						phl_info->hal,
			 			hw_band,
						&proto_cap)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			  "%s : Get SW/HW BF Cap FAIL, disable all of the BF functions.\n", __func__);
	}

	/* Second : filter bf cap with 802.11 spec */
	bfcap &= _phl_sw_role_cap_bf(rtype);

	/* Final : Compare with sw_role_cap->bf_cap to judge the final wrole's BF CAP. */
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : sw_role_cap->bf_cap = 0x%x \n",
		__func__, sw_role_cap->bf_cap);
	if (!(bfcap & HW_CAP_BFEE_HT_SU) &&
	    (proto_cap.ht_su_bfme)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HT SU BFEE by sw_role_cap.\n");
		protocol_cap->ht_su_bfme = 0;
	} else {
		protocol_cap->ht_su_bfme = proto_cap.ht_su_bfme;
	}

	if (!(bfcap & HW_CAP_BFER_HT_SU) &&
	    (proto_cap.ht_su_bfmr)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HT SU BFER by sw_role_cap.\n");
		protocol_cap->ht_su_bfmr = 0;
	} else {
		protocol_cap->ht_su_bfmr = proto_cap.ht_su_bfmr;
	}

	if (!(bfcap & HW_CAP_BFEE_VHT_SU) &&
	    (proto_cap.vht_su_bfme)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT SU BFEE by sw_role_cap.\n");
		protocol_cap->vht_su_bfme = 0;
	} else {
		protocol_cap->vht_su_bfme = proto_cap.vht_su_bfme;
	}

	if (!(bfcap & HW_CAP_BFER_VHT_SU) &&
	    (proto_cap.vht_su_bfmr)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT SU BFER by sw_role_cap.\n");
		protocol_cap->vht_su_bfmr = 0;
	} else {
		protocol_cap->vht_su_bfmr = proto_cap.vht_su_bfmr;
	}

	if (!(bfcap & HW_CAP_BFEE_VHT_MU) &&
	    (proto_cap.vht_mu_bfme)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT MU BFEE by sw_role_cap.\n");
		protocol_cap->vht_mu_bfme = 0;
	} else {
		protocol_cap->vht_mu_bfme = proto_cap.vht_mu_bfme;
	}

	if (!(bfcap & HW_CAP_BFER_VHT_MU) &&
	    (proto_cap.vht_mu_bfmr)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT MU BFER by sw_role_cap.\n");
		protocol_cap->vht_mu_bfmr = 0;
	} else {
		protocol_cap->vht_mu_bfmr = proto_cap.vht_mu_bfmr;
	}

	if (!(bfcap & HW_CAP_BFEE_HE_SU) &&
	    (proto_cap.he_su_bfme)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE SU BFEE by sw_role_cap.\n");
		protocol_cap->he_su_bfme = 0;
	} else {
		protocol_cap->he_su_bfme = proto_cap.he_su_bfme;
	}

	if (!(bfcap & HW_CAP_BFER_HE_SU) &&
	    (proto_cap.he_su_bfmr)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE SU BFER by sw_role_cap.\n");
		protocol_cap->he_su_bfmr = 0;
	} else {
		protocol_cap->he_su_bfmr = proto_cap.he_su_bfmr;
	}

	if (!(bfcap & HW_CAP_BFEE_HE_MU) &&
	    (proto_cap.he_mu_bfme)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE MU BFEE by sw_role_cap.\n");
		protocol_cap->he_mu_bfme = 0;
	} else {
		protocol_cap->he_mu_bfme = proto_cap.he_mu_bfme;
	}

	if (!(bfcap & HW_CAP_BFER_HE_MU) &&
	    (proto_cap.he_mu_bfmr)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE MU BFER by sw_role_cap.\n");
		protocol_cap->he_mu_bfmr = 0;
	} else {
		protocol_cap->he_mu_bfmr = proto_cap.he_mu_bfmr;
	}

	if (!(bfcap & HW_CAP_HE_NON_TB_CQI) &&
	    (proto_cap.non_trig_cqi_fb)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE NON-TB CQI_FB by sw_role_cap.\n");
		protocol_cap->non_trig_cqi_fb = 0;
	} else {
		protocol_cap->non_trig_cqi_fb = proto_cap.non_trig_cqi_fb;
	}

	if (!(bfcap & HW_CAP_HE_TB_CQI) &&
	    (proto_cap.trig_cqi_fb)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE TB CQI_FB by sw_role_cap.\n");
		protocol_cap->trig_cqi_fb = 0;
	} else {
		protocol_cap->trig_cqi_fb = proto_cap.trig_cqi_fb;
	}
#endif
}

static void _phl_external_cap_limit(struct phl_info_t *phl_info,
	struct protocol_cap_t *proto_role_cap)
{
#ifdef RTW_WKARD_BTC_STBC_CAP
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);

	if ((proto_role_cap->cap_option & EXT_CAP_LIMIT_2G_RX_STBC) &&
		hal_com->btc_ctrl.disable_rx_stbc) {
		proto_role_cap->stbc_he_rx = 0;
		proto_role_cap->stbc_vht_rx = 0;
		proto_role_cap->stbc_ht_rx = 0;
		PHL_INFO("%s Disable STBC RX cap for BTC request\n", __func__);
	}
#endif
}

static void _phl_update_sw_stbc_cap(struct phl_info_t *phl_info, u8 hw_band)
{
	struct phy_cap_t *phy_cap = &phl_info->phl_com->phy_cap[hw_band];
	struct protocol_cap_t *sw_cap = &phl_info->phl_com->proto_sw_cap[hw_band];

	if (phy_cap->tx_path_num == 1) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: Disable Tx Stbc\n", __func__);
		sw_cap->stbc_ht_tx = 0;
		sw_cap->stbc_vht_tx = 0;
		sw_cap->stbc_he_tx = 0;
		sw_cap->stbc_tx_greater_80mhz = 0;
	}
}

static void _phl_init_proto_stbc_cap(struct phl_info_t *phl_info,
                                     u8 hw_band,
                                     struct protocol_cap_t *protocol_cap)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct role_sw_cap_t *sw_role_cap = &phl_com->role_sw_cap;
	struct protocol_cap_t proto_cap = {0};

	/* First : compare and get the stbc sw_proto_cap and hw_proto_cap .*/
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_get_stbc_proto_cap(phl_com,
								 phl_info->hal,
			 					 hw_band,
								 &proto_cap)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			  "%s : Get SW/HW STBC proto_cap FAIL, disable all of the STBC functions.\n", __func__);
	}

	/* Final : Compare with sw_role_cap->stbc_cap to judge the final wrole's STBC CAP. */
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : sw_role_cap->stbc_cap = 0x%x \n",
		__func__, sw_role_cap->stbc_cap);

#ifdef RTW_WKARD_PHY_CAP

	/* Check sw role cap, if it is not support, set protocol_cap->xxx to 0 */
	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HT_TX) &&
	    (proto_cap.stbc_ht_tx)) {
		protocol_cap->stbc_ht_tx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HT STBC Tx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_ht_tx = proto_cap.stbc_ht_tx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_VHT_TX) &&
	    (proto_cap.stbc_vht_tx)) {
		protocol_cap->stbc_vht_tx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT STBC Tx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_vht_tx = proto_cap.stbc_vht_tx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HE_TX) &&
	    (proto_cap.stbc_he_tx)) {
		protocol_cap->stbc_he_tx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE STBC Tx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_he_tx = proto_cap.stbc_he_tx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HE_TX_GT_80M) &&
	    (proto_cap.stbc_tx_greater_80mhz)) {
		protocol_cap->stbc_tx_greater_80mhz = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable STBC Tx (greater than 80M) by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_tx_greater_80mhz = proto_cap.stbc_tx_greater_80mhz;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HT_RX) &&
	    (proto_cap.stbc_ht_rx)) {
		protocol_cap->stbc_ht_rx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HT STBC Rx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_ht_rx = proto_cap.stbc_ht_rx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_VHT_RX) &&
	    (proto_cap.stbc_vht_rx)) {
		protocol_cap->stbc_vht_rx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable VHT STBC Rx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_vht_rx = proto_cap.stbc_vht_rx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HE_RX) &&
	    (proto_cap.stbc_he_rx)) {
		protocol_cap->stbc_he_rx = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE STBC Rx by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_he_rx = proto_cap.stbc_he_rx;
	}

	if (!(sw_role_cap->stbc_cap & HW_CAP_STBC_HE_RX_GT_80M) &&
	    (proto_cap.stbc_rx_greater_80mhz)) {
		protocol_cap->stbc_rx_greater_80mhz = 0;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "Disable HE STBC Rx (greater than 80M) by sw_role_cap.\n");
	} else {
		protocol_cap->stbc_rx_greater_80mhz = proto_cap.stbc_rx_greater_80mhz;
	}
#endif

	_phl_external_cap_limit(phl_info, protocol_cap);
}

static enum rtw_phl_status
_phl_init_protocol_cap(struct phl_info_t *phl_info,
                       u8 hw_band,
                       enum role_type rtype,
                       struct protocol_cap_t *protocol_cap)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	u8 idx = 0;

	/* TODO: Get protocol cap from sw and hw cap*/
	if (rtw_phl_is_ap_category(rtype)) {
		protocol_cap->num_ampdu = rtw_hal_get_ampdu_num(phl_info->hal, hw_band);
		protocol_cap->ampdu_density = 0;
		protocol_cap->ampdu_len_exp = 0xff;
		protocol_cap->amsdu_in_ampdu = 1;
		protocol_cap->max_amsdu_len =
			phl_com->proto_sw_cap[hw_band].max_amsdu_len;
		protocol_cap->htc_rx = 1;
		protocol_cap->sm_ps = 0;
		protocol_cap->trig_padding = 0;
#ifdef CONFIG_PHL_TWT
		protocol_cap->twt =
				phl_com->dev_cap.twt_sup & RTW_PHL_TWT_RSP_SUP;
#else
		protocol_cap->twt = 0;
#endif /* CONFIG_PHL_TWT */
		protocol_cap->all_ack = 1;
		protocol_cap->a_ctrl = 0xe;
		protocol_cap->ops = 1;
		protocol_cap->ht_vht_trig_rx = 0;
		protocol_cap->bsscolor = 0x0E; /* Default BSS Color */
		protocol_cap->edca[RTW_AC_BE].ac = RTW_AC_BE;
		protocol_cap->edca[RTW_AC_BE].param = 0xA42B;
		protocol_cap->edca[RTW_AC_BK].ac = RTW_AC_BK;
		protocol_cap->edca[RTW_AC_BK].param = 0xA549;
		protocol_cap->edca[RTW_AC_VI].ac = RTW_AC_VI;
		protocol_cap->edca[RTW_AC_VI].param = 0x5E4326;
		protocol_cap->edca[RTW_AC_VO].ac = RTW_AC_VO;
		protocol_cap->edca[RTW_AC_VO].param = 0x2F3224;
		protocol_cap->ht_ldpc = 1;
		protocol_cap->vht_ldpc = 1;
		protocol_cap->he_ldpc = 1;
		protocol_cap->sgi_20 = 1;
		protocol_cap->sgi_40 = 1;
		protocol_cap->sgi_80 = 1;
		protocol_cap->sgi_160 = phl_com->dev_cap.sgi_160_sup;
		switch (phl_com->phy_cap[hw_band].rxss) {
			default:
				break;
			case 1:
				protocol_cap->ht_rx_mcs[0] = 0xff;
				protocol_cap->vht_rx_mcs[0] = 0xfe;
				protocol_cap->vht_rx_mcs[1] = 0xff;
				protocol_cap->he_rx_mcs[0] = 0xfe;
				protocol_cap->he_rx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_rx_mcs[2] = 0xfe;
					protocol_cap->he_rx_mcs[3] = 0xff;
				}
				break;
			case 2:
				protocol_cap->ht_rx_mcs[0] = 0xff;
				protocol_cap->ht_rx_mcs[1] = 0xff;
				protocol_cap->vht_rx_mcs[0] = 0xfa;
				protocol_cap->vht_rx_mcs[1] = 0xff;
				protocol_cap->he_rx_mcs[0] = 0xfa;
				protocol_cap->he_rx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_rx_mcs[2] = 0xfa;
					protocol_cap->he_rx_mcs[3] = 0xff;
				}
				break;
		}
		switch (phl_com->phy_cap[hw_band].txss) {
			default:
				break;
			case 1:
				protocol_cap->ht_tx_mcs[0] = 0xff;
				protocol_cap->vht_tx_mcs[0] = 0xfe;
				protocol_cap->vht_tx_mcs[1] = 0xff;
				protocol_cap->he_tx_mcs[0] = 0xfe;
				protocol_cap->he_tx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_tx_mcs[2] = 0xfe;
					protocol_cap->he_tx_mcs[3] = 0xff;
				}
				break;
			case 2:
				protocol_cap->ht_tx_mcs[0] = 0xff;
				protocol_cap->ht_tx_mcs[1] = 0xff;
				protocol_cap->vht_tx_mcs[0] = 0xfa;
				protocol_cap->vht_tx_mcs[1] = 0xff;
				protocol_cap->he_tx_mcs[0] = 0xfa;
				protocol_cap->he_tx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_tx_mcs[2] = 0xfa;
					protocol_cap->he_tx_mcs[3] = 0xff;
				}
				break;
		}

		protocol_cap->ltf_gi = 0x3f;	// bit-x
		protocol_cap->doppler_tx = 1;
		protocol_cap->doppler_rx = 0;
		protocol_cap->dcm_max_const_tx = 0;
		protocol_cap->dcm_max_nss_tx = 0;
		protocol_cap->dcm_max_const_rx = 3;
		protocol_cap->dcm_max_nss_rx = 0;
		protocol_cap->partial_bw_su_in_mu = 1;

		_phl_update_sw_stbc_cap(phl_info, hw_band);
		_phl_init_proto_stbc_cap(phl_info, hw_band, protocol_cap);
		_phl_init_proto_bf_cap(phl_info, hw_band, rtype, protocol_cap);
		/* All of the HT/VHT/HE BFee */
		if ((1 == protocol_cap->ht_su_bfme) ||
		    (1 == protocol_cap->vht_su_bfme) ||
		    (1 == protocol_cap->vht_mu_bfme) ||
		    (1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_mu_bfme) ||
		    (1 == protocol_cap->non_trig_cqi_fb)||
		    (1 == protocol_cap->trig_cqi_fb)) {
			protocol_cap->bfme_sts = phl_com->dev_cap.bfee_rx_ndp_sts;
			protocol_cap->max_nc = phl_com->phy_cap[hw_band].rx_path_num - 1;
			if (phl_com->phy_cap[hw_band].bw_sup & BW_CAP_160M)
				protocol_cap->bfme_sts_greater_80mhz = phl_com->dev_cap.bfee_rx_ndp_sts;
			else
				protocol_cap->bfme_sts_greater_80mhz = 0;
		} else {
			protocol_cap->bfme_sts = 0;
			protocol_cap->bfme_sts_greater_80mhz = 0;
			protocol_cap->max_nc = 0;
		}
		/* HE BFer */
		if ((1 == protocol_cap->he_su_bfmr) ||
		    (1 == protocol_cap->he_mu_bfmr)) {
			protocol_cap->num_snd_dim = 1;
			if (phl_com->phy_cap[hw_band].bw_sup & BW_CAP_160M)
				protocol_cap->num_snd_dim_greater_80mhz = 1;
			else
				protocol_cap->num_snd_dim_greater_80mhz = 0;
		} else {
			protocol_cap->num_snd_dim = 0;
			protocol_cap->num_snd_dim_greater_80mhz = 0;
		}
		/* HE BFee */
		if ((1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_mu_bfme)) {
			protocol_cap->ng_16_su_fb = 1;
			protocol_cap->ng_16_mu_fb = 1;
			protocol_cap->cb_sz_su_fb = 1;
			protocol_cap->cb_sz_mu_fb = 1;
			protocol_cap->he_rx_ndp_4x32 = 1;
		} else {
			protocol_cap->ng_16_su_fb = 0;
			protocol_cap->ng_16_mu_fb = 0;
			protocol_cap->cb_sz_su_fb = 0;
			protocol_cap->cb_sz_mu_fb = 0;
			protocol_cap->he_rx_ndp_4x32 = 0;
		}

		/*HE SU BFer or BFer*/
		if ((1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_su_bfmr)) {
			protocol_cap->trig_su_bfm_fb = 1;
		} else {
			protocol_cap->trig_su_bfm_fb = 0;
		}
		/*HE MU BFer or BFer*/
		if ((1 == protocol_cap->he_mu_bfme) ||
		    (1 == protocol_cap->he_mu_bfmr)) {
			protocol_cap->trig_mu_bfm_fb = 1;
		} else {
			protocol_cap->trig_mu_bfm_fb = 0;
		}
		/* HT/VHT BFee */
		if ((1 == protocol_cap->vht_mu_bfme) ||
		    (1 == protocol_cap->vht_su_bfme) ||
		    (1 == protocol_cap->ht_su_bfme)) {
			protocol_cap->ht_vht_ng = 0; /* vht ng = 1 */
			protocol_cap->ht_vht_cb = 1; /* vht_mu{9,7}/vht_su{6,4}/ht{4,2} */
		}

		protocol_cap->partial_bw_su_er = 1;
		protocol_cap->pkt_padding = 2;
		protocol_cap->pwr_bst_factor = 1;
		protocol_cap->dcm_max_ru = 2;
		protocol_cap->long_sigb_symbol = 1;
		protocol_cap->tx_1024q_ru = 0;
		protocol_cap->rx_1024q_ru = 1;
		protocol_cap->fbw_su_using_mu_cmprs_sigb = 1;
		protocol_cap->fbw_su_using_mu_non_cmprs_sigb = 1;
		protocol_cap->nss_tx =
			phl_com->phy_cap[hw_band].txss;
		protocol_cap->nss_rx =
			phl_com->phy_cap[hw_band].rxss;
		protocol_cap->ppe_th_present = 0;

		/* EHT Tid-To-Link mapping */
		for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
			/* Link id 15 : if the reported AP is not part of an AP MLD. */
			protocol_cap->tid2link_ul[idx]= 0x7fff;
			protocol_cap->tid2link_dl[idx]= 0x7fff;
		}
	} else if (rtw_phl_is_client_category(rtype)) {
		protocol_cap->num_ampdu = rtw_hal_get_ampdu_num(phl_info->hal, hw_band);
		protocol_cap->ampdu_density = 0;
		protocol_cap->ampdu_len_exp = 0xff;
		protocol_cap->amsdu_in_ampdu = 1;
		protocol_cap->max_amsdu_len =
			phl_com->proto_sw_cap[hw_band].max_amsdu_len;
		protocol_cap->htc_rx = 1;
		protocol_cap->sm_ps = 3;
		protocol_cap->trig_padding = 2;
#ifdef CONFIG_PHL_TWT
		protocol_cap->twt =
				phl_com->dev_cap.twt_sup & RTW_PHL_TWT_REQ_SUP;
#else
		protocol_cap->twt = 0;
#endif /* CONFIG_PHL_TWT */
		protocol_cap->all_ack = 1;
		protocol_cap->a_ctrl = 0x6;
		protocol_cap->ops = 1;
		protocol_cap->ht_vht_trig_rx = 1;
		protocol_cap->edca[RTW_AC_BE].ac = RTW_AC_BE;
		protocol_cap->edca[RTW_AC_BE].param = 0xA42B;
		protocol_cap->edca[RTW_AC_BK].ac = RTW_AC_BK;
		protocol_cap->edca[RTW_AC_BK].param = 0xA549;
		protocol_cap->edca[RTW_AC_VI].ac = RTW_AC_VI;
		protocol_cap->edca[RTW_AC_VI].param = 0x5E4326;
		protocol_cap->edca[RTW_AC_VO].ac = RTW_AC_VO;
		protocol_cap->edca[RTW_AC_VO].param = 0x2F3224;
		protocol_cap->ht_ldpc = 1;
		protocol_cap->vht_ldpc = 1;
		protocol_cap->he_ldpc = 1;
		protocol_cap->sgi_20 = 1;
		protocol_cap->sgi_40 = 1;
		protocol_cap->sgi_80 = 1;
		protocol_cap->sgi_160 = phl_com->dev_cap.sgi_160_sup;

		switch (phl_com->phy_cap[hw_band].rxss) {
			default:
				break;
			case 1:
				protocol_cap->ht_rx_mcs[0] = 0xff;
				protocol_cap->vht_rx_mcs[0] = 0xfe;
				protocol_cap->vht_rx_mcs[1] = 0xff;
				protocol_cap->he_rx_mcs[0] = 0xfe;
				protocol_cap->he_rx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_rx_mcs[2] = 0xfe;
					protocol_cap->he_rx_mcs[3] = 0xff;
				}
				break;
			case 2:
				protocol_cap->ht_rx_mcs[0] = 0xff;
				protocol_cap->ht_rx_mcs[1] = 0xff;
				protocol_cap->vht_rx_mcs[0] = 0xfa;
				protocol_cap->vht_rx_mcs[1] = 0xff;
				protocol_cap->he_rx_mcs[0] = 0xfa;
				protocol_cap->he_rx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_rx_mcs[2] = 0xfa;
					protocol_cap->he_rx_mcs[3] = 0xff;
				}
				break;
		}
		switch (phl_com->phy_cap[hw_band].txss) {
			default:
				break;
			case 1:
				protocol_cap->ht_tx_mcs[0] = 0xff;
				protocol_cap->vht_tx_mcs[0] = 0xfe;
				protocol_cap->vht_tx_mcs[1] = 0xff;
				protocol_cap->he_tx_mcs[0] = 0xfe;
				protocol_cap->he_tx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_tx_mcs[2] = 0xfe;
					protocol_cap->he_tx_mcs[3] = 0xff;
				}
				break;
			case 2:
				protocol_cap->ht_tx_mcs[0] = 0xff;
				protocol_cap->ht_tx_mcs[1] = 0xff;
				protocol_cap->vht_tx_mcs[0] = 0xfa;
				protocol_cap->vht_tx_mcs[1] = 0xff;
				protocol_cap->he_tx_mcs[0] = 0xfa;
				protocol_cap->he_tx_mcs[1] = 0xff;
				if (phl_com->dev_cap.bw_sup & BW_CAP_160M) {
					protocol_cap->he_tx_mcs[2] = 0xfa;
					protocol_cap->he_tx_mcs[3] = 0xff;
				}
				break;
		}

		protocol_cap->ltf_gi = 0x3f;	// bit-x
		protocol_cap->doppler_tx = 1;
		protocol_cap->doppler_rx = 0;
		protocol_cap->dcm_max_const_tx = 3;
		protocol_cap->dcm_max_nss_tx = 1;
		protocol_cap->dcm_max_const_rx = 3;
		protocol_cap->dcm_max_nss_rx = 0;

		_phl_update_sw_stbc_cap(phl_info, hw_band);
		_phl_init_proto_stbc_cap(phl_info, hw_band, protocol_cap);
		_phl_init_proto_bf_cap(phl_info, hw_band, rtype, protocol_cap);

		/* All of the HT/VHT/HE BFee */
		if ((1 == protocol_cap->ht_su_bfme) ||
		    (1 == protocol_cap->vht_su_bfme) ||
		    (1 == protocol_cap->vht_mu_bfme) ||
		    (1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_mu_bfme) ||
		    (1 == protocol_cap->non_trig_cqi_fb) ||
		    (1 == protocol_cap->trig_cqi_fb)) {
			protocol_cap->bfme_sts = phl_com->dev_cap.bfee_rx_ndp_sts;
			protocol_cap->max_nc = phl_com->phy_cap[hw_band].rx_path_num - 1;
			if (phl_com->phy_cap[hw_band].bw_sup & BW_CAP_160M)
				protocol_cap->bfme_sts_greater_80mhz = phl_com->dev_cap.bfee_rx_ndp_sts;
			else
				protocol_cap->bfme_sts_greater_80mhz = 0;
		} else {
			protocol_cap->bfme_sts = 0;
			protocol_cap->bfme_sts_greater_80mhz = 0;
			protocol_cap->max_nc = 0;
		}

		/* HE BFer */
		if ((1 == protocol_cap->he_su_bfmr) ||
		    (1 == protocol_cap->he_mu_bfmr)) {
			protocol_cap->num_snd_dim = 1;
			if (phl_com->phy_cap[hw_band].bw_sup & BW_CAP_160M)
				protocol_cap->num_snd_dim_greater_80mhz = 1;
			else
				protocol_cap->num_snd_dim_greater_80mhz = 0;
		} else {
			protocol_cap->num_snd_dim = 0;
			protocol_cap->num_snd_dim_greater_80mhz = 0;
		}
		/* HE BFee */
		if ((1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_mu_bfme)) {
#ifdef RTW_WKARD_BFEE_DISABLE_NG16
			protocol_cap->ng_16_su_fb = 0;
			protocol_cap->ng_16_mu_fb = 0;
#else
			protocol_cap->ng_16_su_fb = 1;
			protocol_cap->ng_16_mu_fb = 1;
#endif
			protocol_cap->cb_sz_su_fb = 1;
			protocol_cap->cb_sz_mu_fb = 1;
			protocol_cap->he_rx_ndp_4x32 = 1;
		} else {
			protocol_cap->ng_16_su_fb = 0;
			protocol_cap->ng_16_mu_fb = 0;
			protocol_cap->cb_sz_su_fb = 0;
			protocol_cap->cb_sz_mu_fb = 0;
			protocol_cap->he_rx_ndp_4x32 = 0;
		}
		/*HE SU BFer or BFer*/
		if ((1 == protocol_cap->he_su_bfme) ||
		    (1 == protocol_cap->he_su_bfmr)) {
			protocol_cap->trig_su_bfm_fb = 1;
		} else {
			protocol_cap->trig_su_bfm_fb = 0;
		}
		/*HE MU BFer or BFer*/
		if ((1 == protocol_cap->he_mu_bfme) ||
		    (1 == protocol_cap->he_mu_bfmr)) {
			protocol_cap->trig_mu_bfm_fb = 1;
		} else {
			protocol_cap->trig_mu_bfm_fb = 0;
		}
		/* HT/VHT BFee */
		if ((1 == protocol_cap->vht_mu_bfme) ||
		    (1 == protocol_cap->vht_su_bfme) ||
		    (1 == protocol_cap->ht_su_bfme)) {
			protocol_cap->ht_vht_ng = 0; /* vht ng = 1 */
			protocol_cap->ht_vht_cb = 1; /* vht_mu{9,7}/vht_su{6,4}/ht{4,2} */
		}
		protocol_cap->partial_bw_su_in_mu = 0;
		protocol_cap->partial_bw_su_er = 1;
		protocol_cap->pkt_padding = 2;
		protocol_cap->pwr_bst_factor = 1;
		protocol_cap->dcm_max_ru = 2;
		protocol_cap->long_sigb_symbol = 1;
		protocol_cap->tx_1024q_ru = 1;
		protocol_cap->rx_1024q_ru = 1;
		protocol_cap->fbw_su_using_mu_cmprs_sigb = 1;
		protocol_cap->fbw_su_using_mu_non_cmprs_sigb = 1;
		protocol_cap->nss_tx =
			phl_com->phy_cap[hw_band].txss;
		protocol_cap->nss_rx =
			phl_com->phy_cap[hw_band].rxss;
		protocol_cap->ppe_th_present = 0;

		/* EHT Tid-To-Link mapping */
		for(idx = 0; idx < WMM_AC_TID_NUM; idx++) {
			/* Link id 15 : if the reported AP is not part of an AP MLD. */
			protocol_cap->tid2link_ul[idx]= 0x7fff;
			protocol_cap->tid2link_dl[idx]= 0x7fff;
		}
	}
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_init_protocol_cap(struct phl_info_t *phl_info,
                      struct rtw_wifi_role_t *wifi_role,
                      struct rtw_wifi_role_link_t *rlink)
{

	enum rtw_phl_status ret = RTW_PHL_STATUS_SUCCESS;
	struct protocol_cap_t *protocol_cap = &rlink->protocol_cap;
	u8 hw_band = rlink->hw_band;

	_os_mem_set(phl_to_drvpriv(phl_info),
		protocol_cap, 0, sizeof(struct protocol_cap_t));

	ret = _phl_init_protocol_cap(phl_info, hw_band, wifi_role->type,
		protocol_cap);

	if (ret == RTW_PHL_STATUS_FAILURE)
		PHL_ERR("wrole:%d rlink:%d - %s failed\n",
			wifi_role->id, rlink->id, __func__);

	return ret;
}

static enum rtw_phl_status
_phl_init_rlink_cap(struct phl_info_t *phl_info,
                    u8 hw_band,
                    struct role_link_cap_t *cap)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;

#ifdef RTW_WKARD_PHY_CAP
	cap->wmode = phl_com->phy_cap[hw_band].proto_sup;
	cap->bw = _phl_sw_cap_get_hi_bw(&phl_com->phy_cap[hw_band]);
	cap->rty_lmt = 0xFF; /* default follow CR */
	cap->rty_lmt_rts = 0xFF; /* default follow CR */

	cap->tx_htc = 1;
	cap->tx_sgi = 1;
	cap->tx_ht_ldpc = 1;
	cap->tx_vht_ldpc = 1;
	cap->tx_he_ldpc = 1;
#endif
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_init_rlink_cap(struct phl_info_t *phl_info,
                   struct rtw_wifi_role_link_t *rlink)
{
	enum rtw_phl_status ret = RTW_PHL_STATUS_SUCCESS;
	struct role_link_cap_t *cap = &rlink->cap;

	_os_mem_set(phl_to_drvpriv(phl_info),
		cap, 0, sizeof(struct role_link_cap_t));

	ret = _phl_init_rlink_cap(phl_info, rlink->hw_band, cap);

	ret = phl_custom_init_role_link_cap(phl_info, rlink->hw_band, cap);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_get_dft_proto_cap(void *phl,
                          u8 hw_band,
                          enum role_type rtype,
                          struct protocol_cap_t *protocol_cap)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	_os_mem_set(phl_to_drvpriv(phl_info),
		protocol_cap, 0, sizeof(struct protocol_cap_t));

	return _phl_init_protocol_cap(phl_info, hw_band, rtype,
		protocol_cap);
}

enum rtw_phl_status
rtw_phl_get_dft_cap(void *phl,
                    u8 hw_band,
                    struct role_link_cap_t *cap)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	_os_mem_set(phl_to_drvpriv(phl_info),
		cap, 0, sizeof(struct role_link_cap_t));

	return _phl_init_rlink_cap(phl_info, hw_band, cap);
}

void rtw_phl_final_cap_decision(void * phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;

	rtw_hal_final_cap_decision(phl_com, phl_info->hal);
}

void phl_init_proto_stbc_cap(struct rtw_wifi_role_link_t *rlink,
		struct phl_info_t *phl_info,
		struct protocol_cap_t *proto_role_cap)
{
	if (rlink->chandef.band == BAND_ON_24G)
		proto_role_cap->cap_option |= EXT_CAP_LIMIT_2G_RX_STBC;
	else
		proto_role_cap->cap_option &= ~(EXT_CAP_LIMIT_2G_RX_STBC);

	_phl_init_proto_stbc_cap(phl_info, rlink->hw_band, proto_role_cap);
}
