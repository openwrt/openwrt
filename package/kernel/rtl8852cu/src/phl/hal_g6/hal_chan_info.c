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
#define _HAL_CHAN_INFO_C_
#include "hal_headers.h"

#ifdef CONFIG_PHL_CHANNEL_INFO

#define CSI_BLOCK_SZ 1280

static enum rtw_hal_status
_hal_chinfo_query_idle_csi_buf(struct hal_info_t *hal,
				u8 chinfo_sz, u8 *buf_start, u8 *buf_end)
{
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	#if 0
	if (chinfo_sz > CSI_BLOCK_SZ*2) {
		PHL_ERR("%s chinfo_sz(%d) over hw support(%d)\n",
			__func__, chinfo_sz, CSI_BLOCK_SZ*2);
		return hsts;
	}
	#endif

	*buf_start = 1;
	*buf_end = 2;

	hsts = RTW_HAL_STATUS_SUCCESS;
	return hsts;
}

enum rtw_hal_status
rtw_hal_cfg_chinfo(void *hal, struct rtw_chinfo_action_parm *act_param)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct chinfo_bbcr_cfg bbcr = {0};
	/*enum phl_chinfo_mode ch_mode = act_param->mode;*/
	enum wlan_mode wmode = act_param->sta->wmode;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	u8 enable = act_param->enable;
	u8 hw_band = act_param->sta->rlink->hw_band;
	u8 buf_start = 0;
	u8 buf_end = 0;

	if (enable) {
		/*
		 *Below is Example code for BBCR set to Legacy channel 80MHz
		 *TODO 1: need to calculate channel raw data size to see if
		 *	it is acceptable.
		 *TODO 2: there should be 10 sets of pre-defined configuration for BBCR.
		 *	need to decide which CR to use.
		 */
		if (hw_band == 0)
			bbcr.ch_i_phy0_en = true;
		else
			bbcr.ch_i_phy1_en = true;

		bbcr.ch_i_grp_num_he = act_param->group_num;
		bbcr.ch_i_grp_num_non_he = act_param->group_num;
		bbcr.ch_i_data_src = false; /*LS, channel estimation*/
		switch (act_param->accuracy) {
		case CHINFO_ACCU_1BYTE:
			bbcr.ch_i_cmprs = false; /*8 bits*/
			break;
		case CHINFO_ACCU_2BYTES:
			bbcr.ch_i_cmprs = true; /*16 bits*/
			break;
		default:
			bbcr.ch_i_cmprs = false; /*8 bits*/
			break;
		}
#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
		if (act_param->ele_bitmap == 0) {
			/* 2T2R */
			bbcr.ch_i_ele_bitmap = 0x303;
		} else {
			bbcr.ch_i_ele_bitmap = act_param->ele_bitmap;
		}
#else
		/* 2T2R */
		bbcr.ch_i_ele_bitmap = 0x303;
#endif
		hal_status = _hal_chinfo_query_idle_csi_buf(hal_info, 0, &buf_start, &buf_end);
		if (hal_status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s: query_idle_csi_buf fail\n", __func__);
			goto exit;
		}
		bbcr.ch_i_blk_start_idx = buf_start;
		bbcr.ch_i_blk_end_idx = buf_end;

		if (act_param->mode != CHINFO_MODE_ACK) {
			if (wmode & WLAN_MD_11AX ||
				wmode & WLAN_MD_11AC ||
				wmode & WLAN_MD_11N)
				 /* MIMO channel for non legacy mode */
				bbcr.ch_i_type = CH_INFO_MIMO_CH;
			else
				/* legacy channel for legacy mode */
				bbcr.ch_i_type = CH_INFO_LEGACY_CH;
		} else {
			/* use legacy mode becasue ACK is OFDM rate */
			bbcr.ch_i_type = CH_INFO_LEGACY_CH;
		}

		bbcr.ch_i_seg_len = 0; /*12*/
		rtw_hal_bb_chan_info_cfg(hal_info, &bbcr);
	}

exit:
	return hal_status;
}

void
_hal_fill_csi_header_remain(void* hal, struct csi_header_t *csi_header
	, struct rtw_r_meta_data *mdata)
{
	/* struct hal_info_t *hal_info = (struct hal_info_t *)hal; */
	/* struct rtw_hal_com_t *h = hal_info->hal_com; */

	/* from mdata */
	csi_header->hw_assigned_timestamp = mdata->freerun_cnt;
	csi_header->rx_data_rate = mdata->rx_rate;
	csi_header->bandwidth = mdata->bw;
	csi_header->ch_matrix_report = mdata->get_ch_info;
	/* TODO: from drv define, get value from other side.*/
	csi_header->channel = 0;
	csi_header->num_sub_carrier = 0;
	csi_header->num_bit_per_tone = 0;
	/* Others: mac addres not from TA ? */
	/* hal_mem_cpy(h, &(csi_header->mac_addr[0]), &(mdata->ta[0]), MAC_ALEN); */
}

void
_hal_fill_csi_header_phy_info(void* hal, struct csi_header_t *csi_header
	, struct ch_rpt_hdr_info *ch_hdr_rpt, struct phy_info_rpt *phy_info)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *h = hal_info->hal_com;

	/* Initialize csi header */
	hal_mem_set(h, csi_header, 0, sizeof(struct csi_header_t));
	/* from phy_info_rpt */
	/* shift 1 for remove decimal point */
	csi_header->rssi[0] = phy_info->rssi[0] >> 1;
	csi_header->rssi[1] = phy_info->rssi[1] >> 1;
	csi_header->rxsc = phy_info->rxsc;
	/* from ch_rpt_hdr_info */
	csi_header->nc = ch_hdr_rpt->n_rx;
	csi_header->nr = ch_hdr_rpt->n_sts;
	/* shift 1 for remove decimal point */
	csi_header->avg_idle_noise_pwr = ch_hdr_rpt->avg_noise_pow >> 1;
	csi_header->csi_data_length = (ch_hdr_rpt->total_len_m <<16 | ch_hdr_rpt->total_len_l)
		- 16; /* header(8byte) + Phy_info_in_seg_0(8byte) */
	csi_header->csi_valid = ch_hdr_rpt->set_valid;
	/* shift 1 for remove decimal point */
	csi_header->evm[0] = ch_hdr_rpt->sts0_evm >> 2;
	csi_header->evm[1] = (phy_info->sts1_evm_m << 4 | phy_info->sts1_evm_l) >> 2;
}

enum rtw_hal_status
rtw_hal_ch_info_en(void *hal, struct rtw_chinfo_action_parm *act_param,
						u8 pkt_id)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	/*void *drv_priv = hal_to_drvpriv(hal_info);*/
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	enum phl_phy_idx hw_phy_idx = HW_PHY_0;
	u8 mode = act_param->mode, filter = 0, sg_size = 0;
	u8 macid = (u8)act_param->sta->macid;
	u8 phy_idx = act_param->sta->rlink->hw_band;
	bool en = act_param->enable;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_phl_stainfo_t *sta = act_param->sta;
	struct rtw_chinfo_cur_parm *cur_parm = phl_com->cur_parm;
	bool valid_ch_info_physts = false;
	u8 arr_idx = 0;
	enum rtw_data_rate  rate = RTW_DATA_RATE_OFDM6;
	u8 retry_cnt = 5, pkt_num = 1;
	u16 period = act_param->trig_period;

	switch (phy_idx) {
	case 0:
		hw_phy_idx = HW_PHY_0;
		break;
	case 1:
		hw_phy_idx = HW_PHY_1;
		break;
	default:
		hw_phy_idx = HW_PHY_0;
		break;
	}

	PHL_INFO("[CH INFO] %s: en=%d,num=%d\n",__func__, en, cur_parm->num);
	if (en == true) {
		/* first enable BB */
		if (cur_parm->num == 0) {
			hal_status = rtw_hal_bb_ch_info_decision(hal_info, hw_phy_idx,
						act_param->sta->chandef.bw,
						sta->wmode, &valid_ch_info_physts);

			if (hal_status != RTW_HAL_STATUS_SUCCESS) {
				PHL_ERR("%s ch info decision fail(%d)\n", __func__, hal_status);
				goto exit;
			}

			if (act_param->enable_mode == CHINFO_EN_AUTO_MODE) {
				if (valid_ch_info_physts == true)
					act_param->enable_mode = CHINFO_EN_LIGHT_MODE;
				else
					act_param->enable_mode = CHINFO_EN_RICH_MODE;
			}

			if (act_param->enable_mode == CHINFO_EN_LIGHT_MODE) {
				rtw_hal_bb_ch_trig_select(hal_info, 1);
				if (act_param->mode == CHINFO_MODE_ACK)
					rtw_hal_bb_ch_info_physts_en(hal_info, en, hw_phy_idx, sta->wmode, true);
				else if (act_param->mode == CHINFO_MODE_MACID)
					rtw_hal_bb_ch_info_physts_en(hal_info, en, hw_phy_idx, sta->wmode, false);
				else
					PHL_ERR("[CH INFO] not implement for mode(%d)\n", act_param->mode);
			} else {
				/* MAC cfg only for RICH MODE */
				hal_status = rtw_hal_mac_chan_info_cfg(hal_info, en, macid, mode, filter, sg_size);
				rtw_hal_bb_ch_info_status_en(hal_info, en, hw_phy_idx);
			}

			/* first enbale, memcpy the current parameter */
			hal_mem_cpy(hal_info->hal_com, &cur_parm->action_parm,
						act_param, sizeof(struct rtw_chinfo_action_parm));
		} else {
			/* set macid info to MAC cfg for more one client */
			if (phl_com->cur_parm->action_parm.enable_mode == CHINFO_EN_RICH_MODE)
				hal_status = rtw_hal_mac_chan_info_cfg(hal_info, en, macid, mode, filter, sg_size);
		}

		 if (phl_com->cur_parm->action_parm.mode == CHINFO_MODE_ACK) {
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
			/* record data rate to filter unexpected csi */
			cur_parm->rate = rate;
#endif
			hal_status = rtw_hal_mac_cfg_sensing_csi(hal_info, macid,
								true, period, retry_cnt, rate,
								pkt_num, &pkt_id);
		}
		/* add macid to cur_parm  */
		arr_idx = (u8)_os_division64(macid, 8);
		cur_parm->macid_bitmap[arr_idx] |= BIT(_os_modular64(macid, 8));
		cur_parm->num++;
	} else {
		/* disable BB for no client */
		if (cur_parm->num == 1) {
			if (cur_parm->action_parm.enable_mode== CHINFO_EN_LIGHT_MODE) {
				if (cur_parm->action_parm.mode == CHINFO_MODE_ACK)
					rtw_hal_bb_ch_info_physts_en(hal_info, en, hw_phy_idx, sta->wmode, true);
				else if (cur_parm->action_parm.mode == CHINFO_MODE_MACID)
					rtw_hal_bb_ch_info_physts_en(hal_info, en, hw_phy_idx, sta->wmode, false);
				else
					PHL_ERR("[CH INFO] not implement for mode(%d)\n",
								cur_parm->action_parm.mode);
			} else {
				rtw_hal_bb_ch_info_status_en(hal_info, en, hw_phy_idx);
				/* disable MAC for only for RICH MODE */
				hal_status = rtw_hal_mac_chan_info_cfg(hal_info, en,
										macid, mode, filter, sg_size);
			}

			/* disable, reset cur_parm to 0 */
			hal_mem_set(hal_info->hal_com, cur_parm, 0, sizeof(struct rtw_chinfo_cur_parm));
		} else if (cur_parm->num > 1) {
			/* remvoe macid info to MAC cfg for more one client */
			if (phl_com->cur_parm->action_parm.enable_mode == CHINFO_EN_RICH_MODE)
				hal_status = rtw_hal_mac_chan_info_cfg(hal_info, en, macid, mode, filter, sg_size);
		} else {
			PHL_ERR("[CHAN INFO] error cur_parm->num = %d\n", cur_parm->num);
		}

		if (phl_com->cur_parm->action_parm.mode == CHINFO_MODE_ACK)
			hal_status = rtw_hal_mac_cfg_sensing_csi(hal_info, macid,
								false, period, retry_cnt, rate,
								pkt_num, &pkt_id);
		/* remove macid to cur_parm */
		arr_idx = (u8)_os_division64(macid, 8);
		cur_parm->macid_bitmap[arr_idx] |= BIT(_os_modular64(macid, 8));
		cur_parm->num--;
	}

exit:
	return hal_status;
}

void
rtw_hal_get_ch_info_physts(void *hal,
							struct rtw_r_meta_data *mdata,
							struct hal_ppdu_sts_usr *usr,
							struct rtw_phl_ppdu_phy_info *phy_info)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	struct chan_info_t *chan_info_old = NULL;
	struct rtw_phl_stainfo_t * sta = NULL;
	struct csi_header_t *csi_header = NULL;
	/*struct rtw_chinfo_cur_parm *cur_parm = phl_com->cur_parm;*/
	void *drv = hal_to_drvpriv(hal_info);
	u8* buf_addr;
	u32 idle_num = CHAN_INFO_PKT_TOTAL;

	if (CHAN_INFO_MAX_SIZE < phy_info->ch_info_len) {
		PHL_ERR("chan info buffer smaller than ch info len\n");
		return;
	}

	idle_num = rtw_phl_get_chaninfo_idle_number(drv, phl_com);

	if (idle_num == CHAN_INFO_PKT_TOTAL)
		phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);

	if (phl_com->chan_info == NULL) {
		/*hstatus = RTW_HAL_STATUS_SUCCESS is expected*/
		PHL_INFO("channel info packet not avaialbe due to no pakcet handle\n");
		return;
	}

	buf_addr = phl_com->chan_info->chan_info_buffer;
	hal_mem_cpy(hal_com, buf_addr, phy_info->ch_info_addr, phy_info->ch_info_len);
	csi_header = &phl_com->chan_info->csi_header;
	phl_com->chan_info->length = phy_info->ch_info_len;

	/* Initialize csi header */
	hal_mem_set(hal_com, csi_header, 0, sizeof(struct csi_header_t));
	/* already remove decimal point in rtw_hal_bb_parse_phy_sts */
	hal_mem_cpy(hal_com, &(csi_header->rssi[0]), &(phy_info->rssi_path[0]), 2);
	csi_header->nc = phy_info->n_rx;
	csi_header->nr = phy_info->n_sts;
	/* shift 1 for remove decimal point */
	csi_header->avg_idle_noise_pwr = phy_info->avg_idle_noise_pwr >> 1;
	/* shift 2 for remove decimal point */
	csi_header->evm[0] = phy_info->evm_1_sts >> 2;
	csi_header->evm[1] = phy_info->evm_2_sts >> 2;
	csi_header->csi_data_length = phy_info->ch_info_len;
	csi_header->rxsc = phy_info->rxsc;
	csi_header->csi_valid = phy_info->is_ch_info_len_valid;

	_hal_fill_csi_header_remain(hal, csi_header, mdata);
	if (usr[0].vld) {
		sta = rtw_phl_get_stainfo_by_macid(phl_com->phl_priv, usr[0].macid);
		if (sta != NULL)
			hal_mem_cpy(hal_info->hal_com, &(csi_header->mac_addr[0]),
			sta->mac_addr, MAC_ADDRESS_LENGTH);
	}
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
	else {
		hal_mem_cpy(hal_info->hal_com, &(csi_header->mac_addr[0]),
			mdata->ta, MAC_ADDRESS_LENGTH);
	}
#endif
#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
	hal_print_csi_raw_data(phl_com->chan_info);
#endif
	/* push compelete channel info resourecs to busy queue */
	chan_info_old = rtw_phl_recycle_busy_chaninfo(drv, phl_com, phl_com->chan_info);
	if (chan_info_old)
			rtw_phl_enqueue_idle_chaninfo(drv, phl_com, chan_info_old);
	phl_com->chan_info = rtw_phl_query_idle_chaninfo(drv, phl_com);
	if(phl_com->chan_info == NULL)
		PHL_INFO("channel info packet not avaialbe after recycle\n");
}

#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
void
hal_print_csi_raw_data(struct chan_info_t *chan_info)
{
	u64 *buff_tmp = NULL;
	u32 print_len = chan_info->csi_header.csi_data_length >> 3;
	struct csi_header_t csi_header = chan_info->csi_header;
	u8 i;

	if (chan_info == NULL) {
		PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] chan_info is NULL\n");
		return;
	}

	if (chan_info->chan_info_buffer == NULL
		|| chan_info->length == 0
		|| chan_info->csi_header.csi_data_length == 0) {
		PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] chan_info_buffer is NULL\n");
		return;
	}

	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] CSI raw data: len=%d\n",
											csi_header.csi_data_length);

	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_,
		"[CH INFO] - mac addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
	         csi_header.mac_addr[0], csi_header.mac_addr[1], csi_header.mac_addr[2],
	         csi_header.mac_addr[3], csi_header.mac_addr[4], csi_header.mac_addr[5]);

	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] rxsc=%d\n",
											csi_header.rxsc);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] nc=%d\n",
											csi_header.nc);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] nr=%d\n",
											csi_header.nr);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] avg_idle_noise_pwr=%d\n",
											csi_header.avg_idle_noise_pwr >> 1);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] csi_valid=%d\n",
											csi_header.csi_valid);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] evm[0]=%d\n",
											csi_header.evm[0] >> 2);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] evm[1]=%d\n",
											csi_header.evm[1] >> 2);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] rssi[0]=%d%%\n",
											csi_header.rssi[0] >> 1);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] rssi[1]=%d%%\n",
											csi_header.rssi[1] >> 1);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] timestamp=%d\n",
											csi_header.hw_assigned_timestamp);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] rx_data_rate=0x%02x\n",
											csi_header.rx_data_rate);
	PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[CH INFO] bandwidth=%d\n",
											csi_header.bandwidth);

	if (chan_info->csi_header.csi_data_length % 8)
		print_len++;

	buff_tmp = (u64 *)chan_info->chan_info_buffer;

	for (i = 0; i < print_len; i++)
		PHL_TRACE(COMP_PHL_CHINFO, _PHL_INFO_, "[%02d]0x%016llx\n", i, buff_tmp[i]);

	PHL_DATA(COMP_PHL_CHINFO, _PHL_INFO_, "\n");
}
#endif
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
u8 rtw_hal_ch_info_process_ack(struct rtw_r_meta_data *meta,
									struct rtw_phl_ppdu_sts_info *ppdu_info,
									struct rtw_chinfo_cur_parm *cur_parm,
									u16 macid)
{
	enum phl_band_idx band = (meta->bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;
	if (cur_parm->action_parm.enable == true
		&& cur_parm->action_parm.mode == CHINFO_MODE_ACK
		&& ppdu_info->sts_ent[band][meta->ppdu_cnt].frame_type == RTW_FRAME_TYPE_ACK
		&& ((cur_parm->action_parm.chk_ack_rate == true && meta->rx_rate == cur_parm->rate) || cur_parm->action_parm.chk_ack_rate == false)
	) {
		int map_idx = (int)macid / 32;

		/* check macid match or not */
		if (cur_parm->macid_bitmap[map_idx] & BIT(macid % 32))
			return true;
		else
			return false;
	} else {
		return false;
	}
}
#endif
#endif /* CONFIG_PHL_CHANNEL_INFO */
