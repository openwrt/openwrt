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

#ifndef _MAC_AX_WOWLAN_H_
#define _MAC_AX_WOWLAN_H_

#include "../type.h"
#include "fwcmd.h"
#include "role.h"
#include "gpio_cmd.h"

#define WOW_GET_STOP_TRX_C2H_CNT 10000
#define WOW_GET_STOP_TRX_C2H_DLY 10

#define WOW_GET_AOAC_RPT_C2H_CNT 1000
#define WOW_GET_AOAC_RPT_C2H_DLY 100

#define WOW_STOPTRX_H2CREG_DW_SIZE 1 /* mapping to struct wow_stoptrx_h2creg */

#define PROXY_MDNS_DUMP	0

/**
 * @struct keep_alive
 * @brief keep_alive
 *
 * @var keep_alive::keepalive_en
 * Please Place Description here.
 * @var keep_alive::rsvd0
 * Please Place Description here.
 * @var keep_alive::packet_id
 * Please Place Description here.
 * @var keep_alive::period
 * Please Place Description here.
 * @var keep_alive::mac_id
 * Please Place Description here.
 */
struct keep_alive {
	u32 keepalive_en:1;
	u32 rsvd0:7;
	u32 packet_id:8;
	u32 period:8;
	u32 mac_id:8;
};

/**
 * @struct disconnect_detect
 * @brief disconnect_detect
 *
 * @var disconnect_detect::disconnect_detect_en
 * Please Place Description here.
 * @var disconnect_detect::tryok_bcnfail_count_en
 * Please Place Description here.
 * @var disconnect_detect::disconnect_en
 * Please Place Description here.
 * @var disconnect_detect::rsvd0
 * Please Place Description here.
 * @var disconnect_detect::mac_id
 * Please Place Description here.
 * @var disconnect_detect::check_period
 * Please Place Description here.
 * @var disconnect_detect::try_pkt_count
 * Please Place Description here.
 * @var disconnect_detect::tryok_bcnfail_count_limit
 * Please Place Description here.
 * @var disconnect_detect::rsvd1
 * Please Place Description here.
 */
struct disconnect_detect {
	u32 disconnect_detect_en:1;
	u32 tryok_bcnfail_count_en:1;
	u32 disconnect_en:1;
	u32 rsvd0:5;
	u32 mac_id:8;
	u32 check_period:8;
	u32 try_pkt_count:8;
	u32 tryok_bcnfail_count_limit:8;
	u32 rsvd1:24;
};

/**
 * @struct wow_global
 * @brief wow_global
 *
 * @var wow_global::wow_en
 * Please Place Description here.
 * @var wow_global::drop_all_pkt
 * Please Place Description here.
 * @var wow_global::rx_parse_after_wake
 * Please Place Description here.
 * @var wow_global::rsvd0
 * Please Place Description here.
 * @var wow_global::mac_id
 * Please Place Description here.
 * @var wow_global::pairwise_sec_algo
 * Please Place Description here.
 * @var wow_global::group_sec_algo
 * Please Place Description here.
 * @var wow_global::remotectrl_info_content
 * Please Place Description here.
 * @var wow_global::remotectrl_info_more
 * Please Place Description here.
 */
struct wow_global {
	u32 wow_en:1;
	u32 drop_all_pkt:1;
	u32 rx_parse_after_wake:1;
	u32 rsvd0:5;
	u32 mac_id:8;
	u32 pairwise_sec_algo:8;
	u32 group_sec_algo:8;
	u32 remotectrl_info_content;
	u32 remotectrl_info_more[sizeof(struct
					mac_ax_remotectrl_info_parm_) / 4 - 1];
};

/**
 * @struct gtk_ofld
 * @brief gtk_ofld
 *
 * @var gtk_ofld::gtk_en
 * Please Place Description here.
 * @var gtk_ofld::tkip_en
 * Please Place Description here.
 * @var gtk_ofld::ieee80211w_en
 * Please Place Description here.
 * @var gtk_ofld::pairwise_wakeup
 * Please Place Description here.
 * @var gtk_ofld::rsvd0
 * Please Place Description here.
 * @var gtk_ofld::aoac_rep_id
 * Please Place Description here.
 * @var gtk_ofld::mac_id
 * Please Place Description here.
 * @var gtk_ofld::gtk_rsp_id
 * Please Place Description here.
 * @var gtk_ofld::pmf_sa_query_id
 * Please Place Description here.
 * @var gtk_ofld::bip_sec_algo
 * Please Place Description here.
 * @var gtk_ofld::rsvd1
 * Please Place Description here.
 * @var gtk_ofld::gtk_info_content
 * Please Place Description here.
 * @var gtk_ofld::gtk_info_more
 * Please Place Description here.
 */
struct gtk_ofld {
	u32 gtk_en:1;
	u32 tkip_en:1;
	u32 ieee80211w_en:1;
	u32 pairwise_wakeup:1;
	u32 norekey_wakeup:1;
	u32 rsvd0:3;
	u32 aoac_rep_id:8;
	u32 mac_id:8;
	u32 gtk_rsp_id:8;
	u32 pmf_sa_query_id:8;
	u32 bip_sec_algo:2;
	u32 algo_akm_suit: 8;
	u32 rsvd1: 14;

	u32 gtk_info_content;
	//u32 gtk_info_more[30];
	u32 gtk_info_more[sizeof(struct mac_ax_gtk_info_parm_) / 4 - 1];
};

/**
 * @struct arp_ofld
 * @brief arp_ofld
 *
 * @var arp_ofld::arp_en
 * Please Place Description here.
 * @var arp_ofld::arp_action
 * Please Place Description here.
 * @var arp_ofld::rsvd0
 * Please Place Description here.
 * @var arp_ofld::mac_id
 * Please Place Description here.
 * @var arp_ofld::arp_rsp_id
 * Please Place Description here.
 * @var arp_ofld::arp_info_content
 * Please Place Description here.
 */
struct arp_ofld {
	u32 arp_en:1;
	u32 arp_action:1;
	u32 rsvd0:14;
	u32 mac_id:8;
	u32 arp_rsp_id:8;
	u32 arp_info_content:32;
};

/**
 * @struct ndp_ofld
 * @brief ndp_ofld
 *
 * @var ndp_ofld::ndp_en
 * Please Place Description here.
 * @var ndp_ofld::rsvd0
 * Please Place Description here.
 * @var ndp_ofld::mac_id
 * Please Place Description here.
 * @var ndp_ofld::na_id
 * Please Place Description here.
 * @var ndp_ofld::ndp_info_content
 * Please Place Description here.
 * @var ndp_ofld::ndp_info_more
 * Please Place Description here.
 */
struct ndp_ofld {
	u32 ndp_en:1;
	u32 rsvd0:15;
	u32 mac_id:8;
	u32 na_id:8;
	u32 ndp_info_content;
	//u32 ndp_info_more[27];
	u32 ndp_info_more[2 * sizeof(struct mac_ax_ndp_info_parm_) / 4 - 1];
};

/**
 * @struct realwow
 * @brief realwow
 *
 * @var realwow::realwow_en
 * Please Place Description here.
 * @var realwow::auto_wakeup
 * Please Place Description here.
 * @var realwow::rsvd0
 * Please Place Description here.
 * @var realwow::mac_id
 * Please Place Description here.
 * @var realwow::keepalive_id
 * Please Place Description here.
 * @var realwow::wakeup_pattern_id
 * Please Place Description here.
 * @var realwow::ack_pattern_id
 * Please Place Description here.
 * @var realwow::rsvd1
 * Please Place Description here.
 * @var realwow::realwow_info_content
 * Please Place Description here.
 * @var realwow::realwow_info_more
 * Please Place Description here.
 */
struct realwow {
	u32 realwow_en:1;
	u32 auto_wakeup:1;
	u32 rsvd0:22;
	u32 mac_id:8;
	u32 keepalive_id:8;
	u32 wakeup_pattern_id:8;
	u32 ack_pattern_id:8;
	u32 rsvd1:8;
	u32 realwow_info_content;
	u32 realwow_info_more[sizeof(struct mac_ax_realwowv2_info_parm_)
			      / 4 - 1];
};

/**
 * @struct nlo
 * @brief nlo
 *
 * @var nlo::nlo_en
 * Please Place Description here.
 * @var nlo::nlo_32k_en
 * Please Place Description here.
 * @var nlo::ignore_cipher_type
 * Please Place Description here.
 * @var nlo::rsvd0
 * Please Place Description here.
 * @var nlo::mac_id
 * Please Place Description here.
 * @var nlo::nlo_networklistinfo_content
 * Please Place Description here.
 * @var nlo::nlo_networklistinfo_more
 * Please Place Description here.
 */
struct nlo {
	u32 nlo_en:1;
	u32 nlo_32k_en:1;
	u32 ignore_cipher_type:1;
	u32 rsvd0:21;
	u32 mac_id:8;
	u32 nlo_networklistinfo_content;
	u32 nlo_networklistinfo_more[sizeof(struct mac_ax_nlo_networklist_parm_)
				     / 4 - 1];
};

/**
 * @struct wakeup_ctrl
 * @brief wakeup_ctrl
 *
 * @var wakeup_ctrl::pattern_match_en
 * Please Place Description here.
 * @var wakeup_ctrl::magic_en
 * Please Place Description here.
 * @var wakeup_ctrl::hw_unicast_en
 * Please Place Description here.
 * @var wakeup_ctrl::fw_unicast_en
 * Please Place Description here.
 * @var wakeup_ctrl::deauth_wakeup
 * Please Place Description here.
 * @var wakeup_ctrl::rekey_wakeup
 * Please Place Description here.
 * @var wakeup_ctrl::eap_wakeup
 * Please Place Description here.
 * @var wakeup_ctrl::all_data_wakeup
 * Please Place Description here.
 * @var wakeup_ctrl::rsvd0
 * Please Place Description here.
 * @var wakeup_ctrl::rsvd1
 * Please Place Description here.
 * @var wakeup_ctrl::mac_id
 * Please Place Description here.
 */
struct wakeup_ctrl {
	u32 pattern_match_en:1;
	u32 magic_en:1;
	u32 hw_unicast_en:1;
	u32 fw_unicast_en:1;
	u32 deauth_wakeup:1;
	u32 rekey_wakeup:1;
	u32 eap_wakeup:1;
	u32 all_data_wakeup:1;
	u32 rsvd0:1;
	u32 rsvd1:15;
	u32 mac_id:8;
};

/**
 * @struct negative_pattern
 * @brief negative_pattern
 *
 * @var negative_pattern::negative_pattern_en
 * Please Place Description here.
 * @var negative_pattern::rsvd0
 * Please Place Description here.
 * @var negative_pattern::pattern_count
 * Please Place Description here.
 * @var negative_pattern::mac_id
 * Please Place Description here.
 * @var negative_pattern::pattern_content
 * Please Place Description here.
 */
struct negative_pattern {
	u32 negative_pattern_en:1;
	u32 rsvd0:19;
	u32 pattern_count:4;
	u32 mac_id:8;
	u32 pattern_content:32;
};

/**
 * @struct uphy_ctrl
 * @brief uphy_ctrl
 *
 * @var uphy_ctrl::disable_uphy
 * Please Place Description here.
 * @var uphy_ctrl::handshake_mode
 * Please Place Description here.
 * @var uphy_ctrl::rsvd0
 * Please Place Description here.
 * @var uphy_ctrl::rise_hst2dev_dis_uphy
 * Please Place Description here.
 * @var uphy_ctrl::uphy_dis_delay_unit
 * Please Place Description here.
 * @var uphy_ctrl::pdn_as_uphy_dis
 * Please Place Description here.
 * @var uphy_ctrl::pdn_to_enable_uphy
 * Please Place Description here.
 * @var uphy_ctrl::rsvd1
 * Please Place Description here.
 * @var uphy_ctrl::hst2dev_gpio_num
 * Please Place Description here.
 * @var uphy_ctrl::uphy_dis_delay_count
 * Please Place Description here.
 */
struct uphy_ctrl {
	u32 disable_uphy:1;
	u32 handshake_mode:3;
	u32 rsvd0:4;
	u32 rise_hst2dev_dis_uphy:1;
	u32 uphy_dis_delay_unit:1;
	u32 pdn_as_uphy_dis:1;
	u32 pdn_to_enable_uphy:1;
	u32 rsvd1:4;
	u32 hst2dev_gpio_num:8;
	u32 uphy_dis_delay_count:8;
};

/**
 * @struct wowcam_upd
 * @brief wowcam_upd
 *
 * @var wowcam_upd::r_w
 * Please Place Description here.
 * @var wowcam_upd::idx
 * Please Place Description here.
 * @var wowcam_upd::rsvd0
 * Please Place Description here.
 * @var wowcam_upd::wkfm1
 * Please Place Description here.
 * @var wowcam_upd::wkfm2
 * Please Place Description here.
 * @var wowcam_upd::wkfm3
 * Please Place Description here.
 * @var wowcam_upd::wkfm4
 * Please Place Description here.
 * @var wowcam_upd::crc
 * Please Place Description here.
 * @var wowcam_upd::rsvd1
 * Please Place Description here.
 * @var wowcam_upd::negative_pattern_match
 * Please Place Description here.
 * @var wowcam_upd::skip_mac_hdr
 * Please Place Description here.
 * @var wowcam_upd::uc
 * Please Place Description here.
 * @var wowcam_upd::mc
 * Please Place Description here.
 * @var wowcam_upd::bc
 * Please Place Description here.
 * @var wowcam_upd::rsvd2
 * Please Place Description here.
 * @var wowcam_upd::valid
 * Please Place Description here.
 */
struct wowcam_upd {
	u32 r_w: 1;
	u32 idx: 7;
	u32 rsvd0: 24;
	u32 wkfm1: 32;
	u32 wkfm2: 32;
	u32 wkfm3: 32;
	u32 wkfm4: 32;
	u32 crc: 16;
	u32 rsvd1: 6;
	u32 negative_pattern_match: 1;
	u32 skip_mac_hdr: 1;
	u32 uc: 1;
	u32 mc: 1;
	u32 bc: 1;
	u32 rsvd2: 4;
	u32 valid: 1;
};

struct wow_stoptrx_h2creg {
	u32 func:7;
	u32 ack:1;
	u32 total_len:4;
	u32 seq_num:4;
	u32 rsvd:16;
};

/**
 * @addtogroup WakeOnWlan
 * @{
 */
/**
 * @brief mac_cfg_wow_wake
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_wow_wake(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_wow_wake_info *info,
		     struct mac_ax_remotectrl_info_parm_ *content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_disconnect_det
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_disconnect_det(struct mac_ax_adapter *adapter,
			   u8 macid,
			   struct mac_ax_disconnect_det_info *info);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_keep_alive
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_keep_alive(struct mac_ax_adapter *adapter,
		       u8 macid,
		       struct mac_ax_keep_alive_info *info);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief get_wake_reason
 *
 * @param *adapter
 * @param *wowlan_wake_reason
 * @return Please Place Description here.
 * @retval u32
 */

u32 get_wake_reason(struct mac_ax_adapter *adapter,
		    u8 *wowlan_wake_reason);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_gtk_ofld
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_gtk_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_gtk_ofld_info *info,
		     struct mac_ax_gtk_info_parm_ *content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_arp_ofld
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *parp_info_content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_arp_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_arp_ofld_info *info,
		     void  *parp_info_content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_ndp_ofld
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_ndp_ofld(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_ndp_ofld_info *info,
		     struct mac_ax_ndp_info_parm_ *content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_realwow
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_realwow(struct mac_ax_adapter *adapter,
		    u8 macid,
		    struct mac_ax_realwow_info *info,
		    struct mac_ax_realwowv2_info_parm_ *content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_nlo
 *
 * @param *adapter
 * @param macid
 * @param *info
 * @param *content
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_nlo(struct mac_ax_adapter *adapter,
		u8 macid,
		struct mac_ax_nlo_info *info,
		struct mac_ax_nlo_networklist_parm_ *content);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_dev2hst_gpio
 *
 * @param *adapter
 * @param *parm
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_dev2hst_gpio(struct mac_ax_adapter *adapter,
			 struct mac_ax_dev2hst_gpio_info *parm);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_uphy_ctrl
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_uphy_ctrl(struct mac_ax_adapter *adapter,
		      struct mac_ax_uphy_ctrl_info *info);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_wowcam_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_wowcam_upd(struct mac_ax_adapter *adapter,
		       struct mac_ax_wowcam_upd_info *info);
/**
 * @}
 */

u32 mac_get_wow_wake_rsn(struct mac_ax_adapter *adapter, u8 *wake_rsn,
			 u8 *reset);

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_cfg_wow_sleep
 *
 * @param *adapter
 * @param sleep
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_wow_sleep(struct mac_ax_adapter *adapter,
		      u8 sleep);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_get_wow_fw_status
 *
 * @param *adapter
 * @param *status
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_wow_fw_status(struct mac_ax_adapter *adapter,
			  u8 *status, u8 func_en);
/**
 * @}
 */

static u32 read_aoac_c2hreg(struct mac_ax_adapter *adapter,
			    struct mac_ax_aoac_report *aoac_rpt);

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_request_aoac_report
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_request_aoac_report(struct mac_ax_adapter *adapter,
			    u8 rx_ready);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_read_aoac_report
 *
 * @param *adapter
 * @param **rpt_buf
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_aoac_report(struct mac_ax_adapter *adapter,
			 struct mac_ax_aoac_report *rpt_buf, u8 rx_ready);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_check_aoac_report_done
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_aoac_report_done(struct mac_ax_adapter *adapter);
/**
 * @}
 */

u32 mac_wow_stop_trx(struct mac_ax_adapter *adapter);

u32 mac_wow_get_stoptrx_st(struct mac_ax_adapter *adapter);

/**
 * @brief mac_cfg_wow_auto_test
 *
 * @param *adapter
 * @param rxtest
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cfg_wow_auto_test(struct mac_ax_adapter *adapter, u8 rxtest);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_proxyofld
 *
 * @param *adapter
 * @param cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_proxyofld(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxyofld *pcfg);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_proxy_mdns_serv_pktofld
 *
 * @param *adapter
 * @param serv
 * @param pktid
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_proxy_mdns_serv_pktofld(struct mac_ax_adapter *adapter,
				struct rtw_hal_mac_proxy_mdns_service *pserv, u8 *pktid);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_proxy_mdns_txt_pktofld
 *
 * @param *adapter
 * @param txt
 * @param pktid
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_proxy_mdns_txt_pktofld(struct mac_ax_adapter *adapter,
			       struct rtw_hal_mac_proxy_mdns_txt *ptxt, u8 *pktid);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_proxy_mdns
 *
 * @param *adapter
 * @param mdns
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_proxy_mdns(struct mac_ax_adapter *adapter, struct rtw_hal_mac_proxy_mdns *pmdns);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_proxy_ptcl_pattern
 *
 * @param *adapter
 * @param cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_proxy_ptcl_pattern(struct mac_ax_adapter *adapter,
			   struct rtw_hal_mac_proxy_ptcl_pattern *cfg);
/**
 * @}
 */

/**
 * @addtogroup WakeOnWlan
 * @{
 */

/**
 * @brief mac_check_proxy_done
 *
 * @param *adapter
 * @param *fw_ret
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_proxy_done(struct mac_ax_adapter *adapter, u8 *fw_ret);
/**
 * @}
 */

#endif // #define _MAC_AX_WOWLAN_H_
