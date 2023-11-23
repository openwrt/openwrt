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
#define _HAL_RX_C_
#include "hal_headers.h"

void rtw_hal_cfg_rxhci(void *hal, u8 en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s : enable %d.\n", __func__, en);

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mac_cfg_rxhci(hal_info, en))
		PHL_ERR("%s failure \n", __func__);
}

enum rtw_hal_status
rtw_hal_set_rxfltr_opt_by_mode(void *hal, u8 band, enum rtw_rx_fltr_opt_mode mode)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	enum rtw_rx_fltr_opt_mode set_mode = -1;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : band(%d), mode(%d)\n",
		__func__, band, mode);

	/* Note: @hal_com->band[band].rx_fltr_opt_mode is used to recored any mode other than
	 * sniffer mode, it effectively records the mode before entering monitor
	 * mode and the subsequent modes set after entering monitor mode.
	 */
	if ((mode == RX_FLTR_OPT_MODE_SNIFFER && hal_info->monitor_mode) ||
	    (mode == RX_FLTR_OPT_MODE_RESTORE && !hal_info->monitor_mode))
		return RTW_HAL_STATUS_FAILURE;

	if (hal_info->monitor_mode && mode != RX_FLTR_OPT_MODE_RESTORE) {
		hal_com->band[band].rx_fltr_opt_mode = mode;
		return RTW_HAL_STATUS_SUCCESS;
	}

	set_mode = (mode == RX_FLTR_OPT_MODE_RESTORE) ?
		hal_com->band[band].rx_fltr_opt_mode : mode;

	hstatus = rtw_hal_mac_set_rxfltr_opt_by_mode(hal_com, band, set_mode);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		return hstatus;

	hal_info->monitor_mode = (mode == RX_FLTR_OPT_MODE_SNIFFER);

	/* Record @hal_com->band[band].rx_fltr_opt_mode only when the mode is not monitor and
	 * restore, otherwise, it is kept intact.
	 */
	if (mode != RX_FLTR_OPT_MODE_SNIFFER &&
	    mode != RX_FLTR_OPT_MODE_RESTORE)
		hal_com->band[band].rx_fltr_opt_mode = mode;

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_rx_fltr_opt_mode rtw_hal_get_rxfltr_opt_mode(void *hal, u8 band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	return (hal_info->monitor_mode) ? RX_FLTR_OPT_MODE_SNIFFER :
		hal_com->band[band].rx_fltr_opt_mode;
}

enum rtw_hal_status rtw_hal_scan_set_rxfltr_by_mode(void *hinfo,
	enum phl_phy_idx phy_idx, bool off_channel, u8 *mode)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	if (off_channel) {
		/* backup rx filter opt mode */
		*mode = rtw_hal_get_rxfltr_opt_mode(hinfo, phy_idx);
		hal_status = rtw_hal_set_rxfltr_opt_by_mode(hinfo,
			phy_idx, RX_FLTR_OPT_MODE_SCAN);
	} else {
		/* restore rx filter opt mode */
		hal_status = rtw_hal_set_rxfltr_opt_by_mode(hinfo,
			phy_idx, *mode);
	}
	return hal_status;
}

enum rtw_hal_status
rtw_hal_enter_mon_mode(void *hinfo, enum phl_phy_idx phy_idx)
{
	return rtw_hal_set_rxfltr_opt_by_mode(hinfo, phy_idx, RX_FLTR_OPT_MODE_SNIFFER);
}

enum rtw_hal_status
rtw_hal_leave_mon_mode(void *hinfo, enum phl_phy_idx phy_idx)
{
	return rtw_hal_set_rxfltr_opt_by_mode(hinfo, phy_idx, RX_FLTR_OPT_MODE_RESTORE);
}

enum rtw_hal_status rtw_hal_acpt_crc_err_pkt(void *hal, u8 band, u8 enable)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;


	return rtw_hal_mac_set_rxfltr_acpt_crc_err(hal_com, band, enable);
}

enum rtw_hal_status rtw_hal_set_rxfltr_mpdu_size(void *hal, u8 band, u16 size)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;


	return rtw_hal_mac_set_rxfltr_mpdu_size(hal_com, band, size);
}

static const char *_hal_rxfltr_type_str(u8 type)
{
	switch (type) {
	case RTW_PHL_PKT_TYPE_CTRL:
		return "CTRL";
	case RTW_PHL_PKT_TYPE_MGNT:
		return "MGNT";
	case RTW_PHL_PKT_TYPE_DATA:
		return "DATA";
	default:
		return "UNKNOWN";
	}
}

static const char *_hal_rxfltr_stype_str_ctrl(u8 stype)
{
	switch (stype) {
	case RXFLTR_STYPE_CTRL_RSVD_0000:
		return "RSVD_0000";
	case RXFLTR_STYPE_CTRL_RSVD_0001:
		return "RSVD_0001";
	case RXFLTR_STYPE_TRIGGER:
		return "TRIGGER";
	case RXFLTR_STYPE_TACK:
		return "TACK";
	case RXFLTR_STYPE_BFRP:
		return "BFRP";
	case RXFLTR_STYPE_VHT_HE_NDPA:
		return "VHT_HE_NDPA";
	case RXFLTR_STYPE_CFE:
		return "CFE";
	case RXFLTR_STYPE_CW:
		return "CW";
	case RXFLTR_STYPE_BAR:
		return "BAR";
	case RXFLTR_STYPE_BA:
		return "BA";
	case RXFLTR_STYPE_PS_POLL:
		return "PS_POLL";
	case RXFLTR_STYPE_RTS:
		return "RTS";
	case RXFLTR_STYPE_CTS:
		return "CTS";
	case RXFLTR_STYPE_ACK:
		return "ACK";
	case RXFLTR_STYPE_CF_END:
		return "CF_END";
	case RXFLTR_STYPE_CF_END_ACK:
		return "CF_END_ACK";
	case RXFLTR_STYPE_CTRL_MAX:
	default:
		return "UNKNOWN";
	}
}

static const char *_hal_rxfltr_stype_str_mgnt(u8 stype)
{
	switch (stype) {
	case RXFLTR_STYPE_ASSOC_REQ:
		return "ASSOC_REQ";
	case RXFLTR_STYPE_ASSOC_RSP:
		return "ASSOC_RSP";
	case RXFLTR_STYPE_REASSOC_REQ:
		return "REASSOC_REQ";
	case RXFLTR_STYPE_REASSOC_RSP:
		return "REASSOC_RSP";
	case RXFLTR_STYPE_PROBE_REQ:
		return "PROBE_REQ";
	case RXFLTR_STYPE_PROBE_RSP:
		return "PROBE_RSP";
	case RXFLTR_STYPE_TIMING_ADV:
		return "TIMING_ADV";
	case RXFLTR_STYPE_MGNT_RSVD_0111:
		return "RSVD_0111";
	case RXFLTR_STYPE_BEACON:
		return "BEACON";
	case RXFLTR_STYPE_ATIM:
		return "ATIM";
	case RXFLTR_STYPE_DIS_ASSOC:
		return "DIS_ASSOC";
	case RXFLTR_STYPE_AUTH:
		return "AUTH";
	case RXFLTR_STYPE_DEAUTH:
		return "DAUTH";
	case RXFLTR_STYPE_ACTION:
		return "ACTION";
	case RXFLTR_STYPE_ACTION_NO_ACK:
		return "ACTION_NO_ACK";
	case RXFLTR_STYPE_MGNT_RSVD_1111:
		return "MGNT_RSVD_1111";
	case RXFLTR_STYPE_MGNT_MAX:
	default:
		return "UNKNOWN";
	}
}

static const char *_hal_rxfltr_stype_str_data(u8 stype)
{
	switch (stype) {
	case RXFLTR_STYPE_DATA_DATA:
		return "DATA_DATA";
	case RXFLTR_STYPE_DATA_CF_ACK:
		return "DATA_CF_ACK";
	case RXFLTR_STYPE_DATA_CF_POLL:
		return "DATA_CF_POLL";
	case RXFLTR_STYPE_DATA_CF_ACK_POLL:
		return "DATA_CF_ACK_POLL";
	case RXFLTR_STYPE_NULL_DATA:
		return "NULL_DATA";
	case RXFLTR_STYPE_CF_ACK:
		return "CF_ACK";
	case RXFLTR_STYPE_CF_POLL:
		return "CF_POLL";
	case RXFLTR_STYPE_CF_ACK_POLL:
		return "CF_ACK_POLL";
	case RXFLTR_STYPE_QOS_DATA:
		return "QOS_DATA";
	case RXFLTR_STYPE_QOS_DATA_CF_ACK:
		return "QOS_DATA_CF_ACK";
	case RXFLTR_STYPE_QOS_DATA_CF_POLL:
		return "QOS_DATA_CF_POLL";
	case RXFLTR_STYPE_QOS_DATA_CF_ACK_POLL:
		return "QOS_DATA_CF_ACK_POLL";
	case RXFLTR_STYPE_QOS_NULL_DATA:
		return "QOS_NULL_DATA";
	case RXFLTR_STYPE_DATA_RSVD_1101:
		return "RSVD_1101";
	case RXFLTR_STYPE_QOS_CF_POLL:
		return "QOS_CF_POLL";
	case RXFLTR_STYPE_QOS_CF_ACK_POLL:
		return "QOS_CF_ACK_POLL";
	case RXFLTR_STYPE_DATA_MAX:
	default:
		return "UNKNOWN";
	}
}

static const char *_hal_rxfltr_target_str(u8 target)
{
	switch (target) {
	case RXFLTR_TARGET_DROP:
		return "DROP";
	case RXFLTR_TARGET_TO_HOST:
		return "TO_HOST";
	case RXFLTR_TARGET_TO_WLCPU:
		return "TO_WLCPU";
	case RXFLTR_TARGET_MAX:
	default:
		return "UNKNOWN";
	}
}

enum rtw_hal_status rtw_hal_set_rxfltr_by_type(void *hal, u8 band, enum rtw_packet_type type,
					       enum rtw_rxfltr_target target)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;

	hstats = rtw_hal_mac_set_rxfltr_by_type(hal_info->hal_com, band, type, target);

	if (RTW_HAL_STATUS_SUCCESS == hstats)
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Set band %u rxfltr type %s target %s\n",
			band, _hal_rxfltr_type_str(type), _hal_rxfltr_target_str(target));
	else
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s : set band %u type %s target %s fail, status(%u)\n",
			__func__, band, _hal_rxfltr_type_str(type), _hal_rxfltr_target_str(target), hstats);

	return hstats;
}

enum rtw_hal_status
rtw_hal_set_rxfltr_by_stype_ctrl(void *hal, u8 band, struct rxfltr_cap_to_set_ctrl *cap_ctrl)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_SUCCESS;
	enum rtw_packet_type type = RTW_PHL_PKT_TYPE_CTRL;
	enum rtw_rxfltr_target target = RXFLTR_TARGET_MAX, trgt_chk = RXFLTR_TARGET_MAX;
	bool type_set = true, stype_set = false;
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_CTRL_MAX; i++) {
		if (cap_ctrl->stype[i].set == false) {
			type_set = false;
			break;
		}
		if ((trgt_chk != RXFLTR_TARGET_MAX) && (cap_ctrl->stype[i].target != trgt_chk)) {
			type_set = false;
			break;
		}

		trgt_chk = cap_ctrl->stype[i].target;
	}

	if (type_set == true)
		return rtw_hal_set_rxfltr_by_type(hal, band, type, trgt_chk);

	for (i = 0; i < RXFLTR_STYPE_CTRL_MAX; i++) {
		stype_set = cap_ctrl->stype[i].set;
		target = cap_ctrl->stype[i].target;
		if (stype_set == true) {
			hstats = rtw_hal_mac_set_rxfltr_by_subtype(hal_com, band, type, i, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats) {
				PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s: set band %u type %s sub-type %s target %s fail, status(%u)\n",
					__func__, band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_ctrl(i),
					_hal_rxfltr_target_str(target), hstats);
				break;
			}

			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Set band %u rxfltr type %s sub-type %s target %s\n",
				band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_ctrl(i), _hal_rxfltr_target_str(target));
		}
	}

	return hstats;
}

enum rtw_hal_status
rtw_hal_set_rxfltr_by_stype_mgnt(void *hal, u8 band, struct rxfltr_cap_to_set_mgnt *cap_mgnt)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_SUCCESS;
	enum rtw_packet_type type = RTW_PHL_PKT_TYPE_MGNT;
	enum rtw_rxfltr_target target = RXFLTR_TARGET_MAX, trgt_chk = RXFLTR_TARGET_MAX;
	bool type_set = true, stype_set = false;
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_MGNT_MAX; i++) {
		if (cap_mgnt->stype[i].set == false) {
			type_set = false;
			break;
		}
		if ((trgt_chk != RXFLTR_TARGET_MAX) && (cap_mgnt->stype[i].target != trgt_chk)) {
			type_set = false;
			break;
		}

		trgt_chk = cap_mgnt->stype[i].target;
	}

	if (type_set == true)
		return rtw_hal_set_rxfltr_by_type(hal, band, type, trgt_chk);

	for (i = 0; i < RXFLTR_STYPE_MGNT_MAX; i ++) {
		stype_set = cap_mgnt->stype[i].set;
		target = cap_mgnt->stype[i].target;
		if (stype_set == true) {
			hstats = rtw_hal_mac_set_rxfltr_by_subtype(hal_com, band, type, i, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats) {
				PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s: set band %u type %s sub-type %s target %s fail, status(%u)\n",
					__func__, band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_mgnt(i),
					_hal_rxfltr_target_str(target), hstats);
				break;
			}

			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Set band %u rxfltr type %s sub-type %s target %s\n",
				band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_mgnt(i), _hal_rxfltr_target_str(target));
		}
	}

	return hstats;
}

enum rtw_hal_status
rtw_hal_set_rxfltr_by_stype_data(void *hal, u8 band, struct rxfltr_cap_to_set_data *cap_data)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_SUCCESS;
	enum rtw_packet_type type = RTW_PHL_PKT_TYPE_DATA;
	enum rtw_rxfltr_target target = RXFLTR_TARGET_MAX, trgt_chk = RXFLTR_TARGET_MAX;
	bool type_set = true, stype_set = false;
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_DATA_MAX; i++) {
		if (cap_data->stype[i].set == false) {
			type_set = false;
			break;
		}
		if ((trgt_chk != RXFLTR_TARGET_MAX) && (cap_data->stype[i].target != trgt_chk)) {
			type_set = false;
			break;
		}

		trgt_chk = cap_data->stype[i].target;
	}

	if (type_set == true)
		return rtw_hal_set_rxfltr_by_type(hal, band, type, trgt_chk);

	for (i = 0; i < RXFLTR_STYPE_DATA_MAX; i++) {
		stype_set = cap_data->stype[i].set;
		target = cap_data->stype[i].target;
		if (stype_set == true) {
			hstats = rtw_hal_mac_set_rxfltr_by_subtype(hal_com, band, type, i, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats) {
				PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s: set band %u type %s sub-type %s target %s fail, status(%u)\n",
					__func__, band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_data(i),
					_hal_rxfltr_target_str(target), hstats);
				break;
			}

			PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "Set band %u rxfltr type %s sub-type %s target %s\n",
				band, _hal_rxfltr_type_str(type), _hal_rxfltr_stype_str_data(i), _hal_rxfltr_target_str(target));
		}
	}

	return hstats;
}

enum rtw_hal_status
rtw_hal_set_rxfltr_by_stype_all(void *hal, u8 band, struct rxfltr_type_cap_to_set *cap_all)
{
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;

	do {
		hstats = rtw_hal_set_rxfltr_by_stype_ctrl(hal, band, &(cap_all->rxfltr_ctrl));
		if (RTW_HAL_STATUS_SUCCESS != hstats)
			break;

		hstats = rtw_hal_set_rxfltr_by_stype_mgnt(hal, band, &(cap_all->rxfltr_mgnt));
		if (RTW_HAL_STATUS_SUCCESS != hstats)
			break;

		hstats = rtw_hal_set_rxfltr_by_stype_data(hal, band, &(cap_all->rxfltr_data));
		if (RTW_HAL_STATUS_SUCCESS != hstats)
			break;
	} while (0);

	return hstats;
}

enum rtw_hal_status rtw_hal_set_rxfltr_type_by_mode(void *hal, u8 band, enum rtw_rxfltr_type_mode mode)
{
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;
	struct rxfltr_cap_to_set_ctrl cap_ctrl = {0};
	enum rtw_packet_type type = RTW_PHL_PKT_TYPE_MAX;
	enum rtw_rxfltr_target target = RXFLTR_TARGET_MAX;

	if (band >= MAX_BAND_NUM)
		return RTW_HAL_STATUS_FAILURE;

	if (mode >= RX_FLTR_TYPE_MODE_MAX)
		return RTW_HAL_STATUS_FAILURE;

	switch (mode) {
	case RX_FLTR_TYPE_MODE_HAL_INIT:
		do {
			type = RTW_PHL_PKT_TYPE_CTRL;
			target = RXFLTR_TARGET_DROP;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_MGNT;
			target = RXFLTR_TARGET_TO_HOST;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_DATA;
			target = RXFLTR_TARGET_TO_HOST;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;
		} while (0);
		break;
	case RX_FLTR_TYPE_MODE_STA_WOW_INIT_PRE:
		do {
			type = RTW_PHL_PKT_TYPE_CTRL;
			target = RXFLTR_TARGET_DROP;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_MGNT;
			target = RXFLTR_TARGET_DROP;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_DATA;
			target = RXFLTR_TARGET_DROP;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;
		} while (0);
		break;
	case RX_FLTR_TYPE_MODE_STA_WOW_INIT_POST:
		do {
			cap_ctrl.stype[RXFLTR_STYPE_VHT_HE_NDPA].set = true;
			cap_ctrl.stype[RXFLTR_STYPE_VHT_HE_NDPA].target = RXFLTR_TARGET_TO_HOST;
			cap_ctrl.stype[RXFLTR_STYPE_BAR].set = true;
			cap_ctrl.stype[RXFLTR_STYPE_BAR].target = RXFLTR_TARGET_TO_HOST;
			hstats = rtw_hal_set_rxfltr_by_stype_ctrl(hal, band, &cap_ctrl);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_MGNT;
			target = RXFLTR_TARGET_TO_HOST;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;

			type = RTW_PHL_PKT_TYPE_DATA;
			target = RXFLTR_TARGET_TO_HOST;
			hstats = rtw_hal_set_rxfltr_by_type(hal, band, type, target);
			if (RTW_HAL_STATUS_SUCCESS != hstats)
				break;
		} while (0);
		break;
	default:
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s: does not handle rxfltr mode(%u)\n", __func__, mode);
		break;
	}

	return hstats;
}

enum rtw_hal_status
rtw_hal_poll_hw_rx_done(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_SUCCESS;

	sts = rtw_hal_mac_poll_hw_rx_done(hal_info);

	return sts;
}

enum rtw_hal_status
rtw_hal_hw_rx_resume(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status sts = RTW_HAL_STATUS_SUCCESS;

	sts = rtw_hal_mac_hw_rx_resume(hal_info);

	return sts;
}

#ifdef CONFIG_PCI_HCI
/**
 * rtw_hal_rx_res_query - query current HW rx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_rx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u16 res_num = 0;

	sts = rtw_hal_mac_rx_res_query(hal_info, dma_ch, host_idx, hw_idx,
	                               &res_num);

	if (sts != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_WARNING_, "%s : query rx resource fail!\n",
		          __func__);
	}

	return res_num;
}

u16 rtw_hal_get_rxbd_num(void *hal, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u16 res_num = 0;

	res_num = trx_ops->get_rxbd_num(hal_info->hal_com, dma_ch);

	return res_num;
}

u16 rtw_hal_get_rxbuf_num(void *hal, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u16 res_num = 0;

	res_num = trx_ops->get_rxbuf_num(hal_info->hal_com, dma_ch);

	return res_num;
}

u16 rtw_hal_get_rxbuf_size(void *hal, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u16 res_num = 0;

	res_num = trx_ops->get_rxbuf_size(hal_info->hal_com, dma_ch);

	return res_num;
}

/**
 * rtw_hal_query_rxch_num - query total hw rx dma channels number
 *
 * returns the number of  hw rx dma channel
 */
u8 rtw_hal_query_rxch_num(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u8 ch_num = 0;

	ch_num = trx_ops->query_rxch_num();

	return ch_num;
}

u8 rtw_hal_check_rxrdy(struct rtw_phl_com_t *phl_com, void* hal,
		       struct rtw_rx_buf *rxbuf, u8 dma_ch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u8 res = 0;

	res = trx_ops->check_rxrdy(phl_com, rxbuf, dma_ch);

	return res;
}

u8 rtw_hal_handle_rxbd_info(void* hal, u8 *rxbuf, u16 *buf_size)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u8 res = 0;

	res = trx_ops->handle_rxbd_info(hal_info, rxbuf, buf_size);

	return res;
}

enum rtw_hal_status
rtw_hal_update_rxbd(void *hal, struct rx_base_desc *rxbd,
					struct rtw_rx_buf *rxbuf, u8 ch_idx)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->update_rxbd(hal_info, rxbd, rxbuf, ch_idx);

	return hstatus;
}


enum rtw_hal_status
rtw_hal_notify_rxdone(void* hal, struct rx_base_desc *rxbd, u8 dma_ch)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	sts = rtw_hal_mac_notify_rxdone(hal_info, rxbd, dma_ch);
	if (sts != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_RECV, _PHL_INFO_, "%s : notify rx done fail!\n",
		          __func__);
	}

	return sts;
}

u16 rtw_hal_handle_wp_rpt(void *hal, u8 *rp, u16 len, u8 *sw_retry, u8 *dma_ch,
			  u16 *wp_seq, u8 *macid, u8 *ac_queue, u8 *txsts)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u16 rsize = 0;

	rsize = trx_ops->handle_wp_rpt(hal_info, rp, len, sw_retry, dma_ch,
				       wp_seq, macid, ac_queue, txsts);
	return rsize;
}

#endif /*CONFIG_PCI_HCI*/


#ifdef CONFIG_USB_HCI
enum rtw_hal_status
rtw_hal_query_info(void* hal, u8 info_id, void *value)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->query_hal_info(hal_info, info_id, value);

	return hstatus;
}

enum rtw_hal_status
hal_usb_rx_agg_cfg(struct hal_info_t *hal, u8 mode, u8 agg_mode,
		   u8 drv_define, u8 timeout, u8 size, u8 pkt_num)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	switch (mode){
	case PHL_RX_AGG_DISABLE:
		hstatus = hal_mac_usb_rx_agg_cfg(hal, MAC_AX_RX_AGG_MODE_NONE,
						 0, 0, 0, 0);
		break;
	case PHL_RX_AGG_DEFAULT:
		hstatus = hal_mac_usb_rx_agg_cfg(hal, MAC_AX_RX_AGG_MODE_USB,
						 0, 0, 0, 0);
		break;
	case PHL_RX_AGG_SMALL_PKT:
		hstatus = hal_mac_usb_rx_agg_cfg(hal, MAC_AX_RX_AGG_MODE_USB,
						 1, 0x01, 0x01, 0);
		break;
	case PHL_RX_AGG_USER_DEFINE:
		hstatus = hal_mac_usb_rx_agg_cfg(hal, agg_mode, drv_define,
						 timeout, size, pkt_num);
		break;
	default:
		hstatus = RTW_HAL_STATUS_FAILURE;
		break;
	}
	return hstatus;
}

enum rtw_hal_status
	rtw_hal_usb_rx_agg_cfg(void *hal, u8 mode, u8 agg_mode,
	u8 drv_define, u8 timeout, u8 size, u8 pkt_num)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->usb_rx_agg_cfg(hal, mode, agg_mode,
		drv_define, timeout, size, pkt_num);

	return hstatus;
}

enum rtw_hal_status
rtw_hal_usb_rx_agg_init(struct rtw_phl_com_t *phl_com, void *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct bus_sw_cap_t *bus_sw = &phl_com->bus_sw_cap;
	struct bus_hw_cap_t *bus_hw = &hal_com->bus_hw_cap;

	if (bus_sw->rx_buf_size == 0 ||
	    bus_sw->rx_buf_size > bus_hw->rx_buf_size) {
		hstatus = rtw_hal_usb_rx_agg_cfg(hal, PHL_RX_AGG_DEFAULT,
						 0, 0, 0, 0, 0);
	} else {
		/*
		 * agg_size unit is 1024 bytes. We make rx_buf_size 4096 bytes
		 * more than agg_size to avoid the case that the data size of
		 * USB Rx exceeds rx_buf_size.
		 */
		u8 agg_size = (u8)(bus_sw->rx_buf_size / 1024) - 4;

		hstatus = rtw_hal_usb_rx_agg_cfg(hal, PHL_RX_AGG_USER_DEFINE,
						 MAC_AX_RX_AGG_MODE_USB , 1, 32,
						 agg_size, 0);
	}

	return hstatus;
}

u16 rtw_hal_handle_wp_rpt_usb(void *hal, u8 *rp, u16 len, u8 *macid, u8 *ac_queue,
		u8 *txsts)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;
	u16 rsize = 0;

	rsize = trx_ops->handle_wp_rpt(hal_info, rp, len, macid, ac_queue, txsts);
	return rsize;
}

#endif

enum rtw_hal_status
rtw_hal_handle_rx_buffer(struct rtw_phl_com_t *phl_com, void* hal,
				u8 *buf, u32 buf_size,
				struct rtw_phl_rx_pkt *rxpkt)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	hstatus = trx_ops->handle_rx_buffer(phl_com, hal_info,
					      buf, buf_size, rxpkt);

	return hstatus;
}

#ifdef CONFIG_SDIO_HCI
void rtw_hal_sdio_rx_agg_cfg(void *hal, bool enable, u8 drv_define,
			     u8 timeout, u8 size, u8 pkt_num)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;


	rtw_hal_mac_sdio_rx_agg_cfg(hal_info->hal_com, enable, drv_define,
				    timeout, size, pkt_num);
}

int rtw_hal_sdio_rx(void *hal, struct rtw_rx_buf *rxbuf)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;


	return rtw_hal_mac_sdio_rx(hal_info->hal_com, rxbuf);
}

int rtw_hal_sdio_parse_rx(void *hal, struct rtw_rx_buf *rxbuf)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;


	return rtw_hal_mac_sdio_parse_rx(hal_info->hal_com, rxbuf);
}

/**
 * rtw_hal_sdio_lps_flg - get lps status
 * @hal:	pointer of struct hal_info_t
 *
 * Get driver lps status
 *
 * return lps flag state defined in enum rtw_hal_lps_flg_state
 * RTW_HAL_LPS_FLG_STATE_LPS when driver set LPS-CG/LPS-PG enable;
 * RTW_HAL_LPS_FLG_STATE_ACTIVE when driver is not in LPS-CG/LPS-PG
 */
enum rtw_hal_lps_flg_state
rtw_hal_sdio_lps_flg(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 lps_flg = 0;
	enum rtw_hal_lps_flg_state state = RTW_HAL_LPS_FLG_STATE_ACTIVE;

	rtw_hal_mac_sdio_lps_flg(hal_info->hal_com, &lps_flg);

	if (lps_flg == 1)
		state = RTW_HAL_LPS_FLG_STATE_LPS;

	return state;
}
#endif /* CONFIG_SDIO_HCI */

void
hal_rx_ppdu_sts_normal_data(struct rtw_phl_com_t *phl_com,
			    void *hdr,
			    struct rtw_r_meta_data *meta)
{
	struct rtw_phl_ppdu_sts_info *ppdu_info = NULL;
	enum phl_band_idx band = HW_BAND_0;
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
	struct rtw_chinfo_cur_parm *cur_parm = phl_com->cur_parm;
#endif

	do {
		if ((NULL == phl_com) || (NULL == meta))
			break;
		ppdu_info = &phl_com->ppdu_sts_info;
		band = (meta->bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;
		if ((ppdu_info->cur_rx_ppdu_cnt[band] == meta->ppdu_cnt) &&
		    (false == ppdu_info->latest_rx_is_psts[band])) {
			/**
			 * add condition to avoid check fail for ppdu cnt run around 0 -> 1 -> ... -> 0
			 * example :
			 * 	[frame_A(ppdu_cnt = 0)] -> [ppdu_sts(ppdu_cnt = 0)]
			 *  	->[ppdu_sts(ppdu_cnt = 1)] -> [ppdu_sts(ppdu_cnt = 2)] ...
			 * 	... ->[ppdu_sts(ppdu_cnt = 7)] -> [frame_B(ppdu_cnt = 0)] ...
			 * 	Therefore, frame_B has same ppdu_cnt with frame_A.
			 *	But they are different PPDU.
			**/
			break;
		}
		meta->ppdu_cnt_chg = true;
		/* start of the PPDU */
		ppdu_info->latest_rx_is_psts[band] = false;
		ppdu_info->sts_ent[band][meta->ppdu_cnt].addr_cam_vld = meta->addr_cam_vld;
		ppdu_info->sts_ent[band][meta->ppdu_cnt].frame_type = PHL_GET_80211_HDR_TYPE(hdr);
		ppdu_info->sts_ent[band][meta->ppdu_cnt].crc32 = meta->crc32;
		ppdu_info->sts_ent[band][meta->ppdu_cnt].rx_rate = meta->rx_rate;
		ppdu_info->sts_ent[band][meta->ppdu_cnt].ppdu_type = meta->ppdu_type;

		if(RTW_IS_BEACON_OR_PROBE_RESP_PKT(ppdu_info->sts_ent[band][meta->ppdu_cnt].frame_type)) {
			PHL_GET_80211_HDR_ADDRESS3(phl_com->drv_priv, hdr,
				ppdu_info->sts_ent[band][meta->ppdu_cnt].src_mac_addr);
		}
		else if (meta->a1_match &&
			  RTW_IS_ASOC_REQ_PKT(ppdu_info->sts_ent[band][meta->ppdu_cnt].frame_type)) {

			PHL_GET_80211_HDR_ADDRESS2(phl_com->drv_priv, hdr,
				ppdu_info->sts_ent[band][meta->ppdu_cnt].src_mac_addr);

			#ifdef DBG_AP_CLIENT_ASSOC_RSSI
			{
				u8 *src = NULL;

				src = ppdu_info->sts_ent[band][meta->ppdu_cnt].src_mac_addr;

				PHL_INFO("%s [Rx-ASOC_REQ] - MAC-Addr:%02x-%02x-%02x-%02x-%02x-%02x, a1_match:%d ppdu_cnt:%d\n",
					__func__,
					src[0], src[1], src[2], src[3], src[4], src[5],
					meta->a1_match,
					meta->ppdu_cnt);
			}
			#endif
		}
		else {
			_os_mem_cpy(phl_com->drv_priv,
				ppdu_info->sts_ent[band][meta->ppdu_cnt].src_mac_addr,
				meta->ta, MAC_ADDRESS_LENGTH);
			#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
			if (rtw_hal_ch_info_process_ack(meta, ppdu_info, cur_parm, meta->macid)) {
				if (meta->macid != 0xff) {
					struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_by_macid(phl_com->phl_priv, meta->macid);

					_os_mem_cpy(phl_com->drv_priv, ppdu_info->sts_ent[band][meta->ppdu_cnt].src_mac_addr,
						sta->mac_addr, MAC_ADDRESS_LENGTH);
					ppdu_info->sts_ent[band][meta->ppdu_cnt].freerun_cnt = meta->freerun_cnt;
				}
			}
			#endif
		}
		ppdu_info->sts_ent[band][meta->ppdu_cnt].valid = false;
		ppdu_info->cur_rx_ppdu_cnt[band] = meta->ppdu_cnt;
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
				"Start of the PPDU : band %d ; ppdu_cnt %d ; frame_type %d ; addr_cam_vld %d ; size %d ; rate 0x%x ; crc32 %d\n",
				band,
				ppdu_info->cur_rx_ppdu_cnt[band],
				ppdu_info->sts_ent[band][meta->ppdu_cnt].frame_type,
				ppdu_info->sts_ent[band][meta->ppdu_cnt].addr_cam_vld,
				meta->pktlen,
				meta->rx_rate,
				meta->crc32);
	} while (false);

}

void
hal_rx_ppdu_sts(struct rtw_phl_com_t *phl_com,
		struct rtw_phl_rx_pkt *phl_rx,
		struct hal_ppdu_sts *ppdu_sts)
{
	struct rtw_phl_ppdu_sts_info *ppdu_info = NULL;
	struct rtw_phl_rssi_stat *rssi_stat = NULL;
	struct rtw_r_meta_data *meta = &(phl_rx->r.mdata);
	struct rtw_phl_ppdu_phy_info *phy_info = &(phl_rx->r.phy_info);
	u8 i = 0;
	enum phl_band_idx band = HW_BAND_0;
	struct rtw_phl_ppdu_sts_ent *sts_ent = NULL;

	if ((NULL == phl_com) || (NULL == meta) || (NULL == ppdu_sts))
		return;

	ppdu_info = &phl_com->ppdu_sts_info;
	rssi_stat = &phl_com->rssi_stat;
	band = (meta->bb_sel > 0) ? HW_BAND_1 : HW_BAND_0;
	ppdu_info->latest_rx_is_psts[band] = true;

	if (0 == phy_info->is_valid)
		return;

	if (ppdu_info->cur_rx_ppdu_cnt[band] != meta->ppdu_cnt) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "[WARNING] ppdu cnt mis-match (band %d ; cur : %d , rxmeta : %d)\n",
			  band,
			  ppdu_info->cur_rx_ppdu_cnt[band],
			  meta->ppdu_cnt);
	}
	sts_ent = &(ppdu_info->sts_ent[band][meta->ppdu_cnt]);

	if (meta->crc32 || sts_ent->crc32) {
		UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_UNKNOWN,
			 phy_info->rssi);
		return;
	}
	if (sts_ent->rx_rate != meta->rx_rate) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "[WARNING] PPDU STS rx rate mis-match\n");
		UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_UNKNOWN,
			       phy_info->rssi);
		return;
	}
	if (sts_ent->ppdu_type != meta->ppdu_type) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "[WARNING] PPDU STS ppdu_type mis-match\n");
		UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_UNKNOWN,
			       phy_info->rssi);
		return;
	}
	if (sts_ent->valid == true) {
		PHL_TRACE(COMP_PHL_PSTS, _PHL_INFO_,
			  "[WARNING] PPDU STS is already updated, skip this ppdu status\n");
		return;
	}

	/* update ppdu_info entry */
	sts_ent->freerun_cnt = meta->freerun_cnt;
	_os_mem_cpy(phl_com->drv_priv,
		    &(sts_ent->phy_info),
		    phy_info, sizeof(struct rtw_phl_ppdu_phy_info));

	sts_ent->usr_num = ppdu_sts->usr_num;
	for (i = 0; i < ppdu_sts->usr_num; i++) {
		if (ppdu_sts->usr[i].vld) {
			sts_ent->sta[i].macid =
				ppdu_sts->usr[i].macid;
			sts_ent->sta[i].vld = 1;
		} else {
			sts_ent->sta[i].vld = 0;
		}
	}
	sts_ent->phl_done = false;
	sts_ent->valid = true;

	/* update rssi stat */
	_os_spinlock(phl_com->drv_priv, &rssi_stat->lock, _bh, NULL);
	switch (sts_ent->frame_type &
		(BIT(1) | BIT(0))) {
		case RTW_FRAME_TYPE_MGNT :
			if (sts_ent->addr_cam_vld) {
				UPDATE_MA_RSSI(rssi_stat,
					 (1 == meta->a1_match) ?
					  RTW_RSSI_MGNT_ACAM_A1M :
					  RTW_RSSI_MGNT_ACAM,
					 phy_info->rssi);
			} else {
				UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_MGNT_OTHER,
					 phy_info->rssi);
			}
		break;
		case RTW_FRAME_TYPE_CTRL :
			if (sts_ent->addr_cam_vld) {
				UPDATE_MA_RSSI(rssi_stat,
					 (1 == meta->a1_match) ?
					  RTW_RSSI_CTRL_ACAM_A1M :
					  RTW_RSSI_CTRL_ACAM,
					 phy_info->rssi);
			} else {
				UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_CTRL_OTHER,
					 phy_info->rssi);
			}
		break;
		case RTW_FRAME_TYPE_DATA :
			if (sts_ent->addr_cam_vld) {
				UPDATE_MA_RSSI(rssi_stat,
					 (1 == meta->a1_match) ?
					  RTW_RSSI_DATA_ACAM_A1M :
					  RTW_RSSI_DATA_ACAM,
					 phy_info->rssi);
			} else {
				UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_DATA_OTHER,
					 phy_info->rssi);
			}
		break;
		default:
			UPDATE_MA_RSSI(rssi_stat, RTW_RSSI_UNKNOWN,
				       phy_info->rssi);
		break;
	}
	_os_spinunlock(phl_com->drv_priv, &rssi_stat->lock, _bh, NULL);
}
