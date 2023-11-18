/** @file */
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

#ifndef __MAC_HW_INFO_H
#define __MAC_HW_INFO_H
/*--------------------Define -------------------------------------------*/
/*--------------------Define MACRO--------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
// TRX Desc related
// MAC_REG related
// F2P CMD related

/**
 * @enum mac_ax_snd_pkt_sel
 *
 * @brief mac_ax_snd_pkt_sel
 *
 * @var mac_ax_snd_pkt_sel::MAC_AX_UNICAST_NDPA
 * Please Place Description here.
 * @var mac_ax_snd_pkt_sel::MAC_AX_BROADCAST_NDPA
 * Please Place Description here.
 * @var mac_ax_snd_pkt_sel::MAC_AX_LAST_NDPA
 * Please Place Description here.
 * @var mac_ax_snd_pkt_sel::MAC_AX_MIDDLE_NDPA
 * Please Place Description here.
 * @var mac_ax_snd_pkt_sel::MAC_AX_BF_REPORT_POLL
 * Please Place Description here.
 * @var mac_ax_snd_pkt_sel::MAC_AX_FINAL_BFRP
 * Please Place Description here.
 */
enum mac_ax_snd_pkt_sel {
	MAC_AX_UNICAST_NDPA = 0,
	MAC_AX_BROADCAST_NDPA = 1,
	MAC_AX_LAST_NDPA = 2,
	MAC_AX_MIDDLE_NDPA = 3,
	MAC_AX_BF_REPORT_POLL = 4,
	MAC_AX_FINAL_BFRP = 5,
};

/**
 * @enum mac_ax_ndpa_pkt
 *
 * @brief mac_ax_ndpa_pkt
 *
 * @var mac_ax_ndpa_pkt::MAC_AX_NORMAL_PKT
 * Please Place Description here.
 * @var mac_ax_ndpa_pkt::MAC_AX_HT_PKT
 * Please Place Description here.
 * @var mac_ax_ndpa_pkt::MAC_AX_VHT_PKT
 * Please Place Description here.
 * @var mac_ax_ndpa_pkt::MAC_AX_HE_PKT
 * Please Place Description here.
 */
enum mac_ax_ndpa_pkt {
	MAC_AX_NORMAL_PKT = 0,
	MAC_AX_HT_PKT = 1,
	MAC_AX_VHT_PKT = 2,
	MAC_AX_HE_PKT = 2,
};

/*--------------------Define Struct-------------------------------------*/
#endif
