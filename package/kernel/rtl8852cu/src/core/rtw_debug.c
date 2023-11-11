/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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
#define _RTW_DEBUG_C_

#include <drv_types.h>
#include "_hal_rate.h"

#ifdef CONFIG_RTW_DEBUG
const char *rtw_log_level_str[] = {
	"_DRV_NONE_ = 0",
	"_DRV_ALWAYS_ = 1",
	"_DRV_ERR_ = 2",
	"_DRV_WARNING_ = 3",
	"_DRV_INFO_ = 4",
	"_DRV_DEBUG_ = 5",
	"_DRV_MAX_ = 6",
};
#endif

#ifdef CONFIG_DEBUG_RTL871X
	u64 GlobalDebugComponents = 0;
#endif /* CONFIG_DEBUG_RTL871X */

#include <rtw_version.h>

#ifdef CONFIG_TDLS
	#define TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE	41
#endif

void dump_drv_version(void *sel)
{
	RTW_PRINT_SEL(sel, "%s %s\n", DRV_NAME, DRIVERVERSION);
	RTW_PRINT_SEL(sel, "build time: %s %s\n", __DATE__, __TIME__);
}

void dump_drv_cfg(void *sel)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
	char *kernel_version = utsname()->release;

	RTW_PRINT_SEL(sel, "\nKernel Version: %s\n", kernel_version);
#endif

#ifdef CONFIG_RTW_ANDROID
	RTW_PRINT_SEL(sel, "Android Driver: %d\n", CONFIG_RTW_ANDROID);
#else
	RTW_PRINT_SEL(sel, "Linux Driver: \n");
#endif /* CONFIG_RTW_ANDROID */

	RTW_PRINT_SEL(sel, "Driver Version: %s\n", DRIVERVERSION);
	RTW_PRINT_SEL(sel, "------------------------------------------------\n");

#ifdef CONFIG_IOCTL_CFG80211
	RTW_PRINT_SEL(sel, "CFG80211\n");
#ifdef RTW_USE_CFG80211_STA_EVENT
	RTW_PRINT_SEL(sel, "RTW_USE_CFG80211_STA_EVENT\n");
#endif
	#ifdef CONFIG_RADIO_WORK
	RTW_PRINT_SEL(sel, "CONFIG_RADIO_WORK\n");
	#endif
#else
	RTW_PRINT_SEL(sel, "WEXT\n");
#endif

	RTW_PRINT_SEL(sel, "DBG:%d\n", DBG);
#ifdef CONFIG_RTW_DEBUG
	RTW_PRINT_SEL(sel, "CONFIG_RTW_DEBUG\n");
#endif

#ifdef CONFIG_CONCURRENT_MODE
	RTW_PRINT_SEL(sel, "CONFIG_CONCURRENT_MODE\n");

	#ifdef CONFIG_RTW_STATIC_NDEV_NUM
	RTW_PRINT_SEL(sel, "CONFIG_RTW_STATIC_NDEV_NUM = %d\n", CONFIG_RTW_STATIC_NDEV_NUM);
	#endif
	#if (RTW_P2P_GROUP_INTERFACE == 1)
	RTW_PRINT_SEL(sel, "RTW_P2P_GROUP_INTERFACE\n");
	#endif
	#ifdef RTW_DEDICATED_P2P_DEVICE
	RTW_PRINT_SEL(sel, "RTW_DEDICATED_P2P_DEVICE\n");
	#endif
	#if defined(CONFIG_P2P) && defined(CONFIG_SEL_P2P_IFACE)
	RTW_PRINT_SEL(sel, "CONFIG_SEL_P2P_IFACE = %d\n", CONFIG_SEL_P2P_IFACE);
	#endif
#endif

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	RTW_PRINT_SEL(sel, "LOAD_PHY_PARA_FROM_FILE - REALTEK_CONFIG_PATH=%s\n", REALTEK_CONFIG_PATH);
	#if defined(CONFIG_MULTIDRV) || defined(REALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER)
	RTW_PRINT_SEL(sel, "LOAD_PHY_PARA_FROM_FILE - REALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER\n");
	#endif

/* configurations about TX power */
#ifdef CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY
	RTW_PRINT_SEL(sel, "CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY\n");
#endif
#ifdef CONFIG_CALIBRATE_TX_POWER_TO_MAX
	RTW_PRINT_SEL(sel, "CONFIG_CALIBRATE_TX_POWER_TO_MAX\n");
#endif
#endif
	RTW_PRINT_SEL(sel, "RTW_DEF_MODULE_REGULATORY_CERT=0x%02x\n", RTW_DEF_MODULE_REGULATORY_CERT);

	RTW_PRINT_SEL(sel, "CONFIG_TXPWR_BY_RATE=%d\n", CONFIG_TXPWR_BY_RATE);
	RTW_PRINT_SEL(sel, "CONFIG_TXPWR_BY_RATE_EN=%d\n", CONFIG_TXPWR_BY_RATE_EN);
	RTW_PRINT_SEL(sel, "CONFIG_TXPWR_LIMIT=%d\n", CONFIG_TXPWR_LIMIT);
	RTW_PRINT_SEL(sel, "CONFIG_TXPWR_LIMIT_EN=%d\n", CONFIG_TXPWR_LIMIT_EN);


#ifdef CONFIG_DISABLE_ODM
	RTW_PRINT_SEL(sel, "CONFIG_DISABLE_ODM\n");
#endif

#ifdef CONFIG_MINIMAL_MEMORY_USAGE
	RTW_PRINT_SEL(sel, "CONFIG_MINIMAL_MEMORY_USAGE\n");
#endif

	RTW_PRINT_SEL(sel, "CONFIG_RTW_ADAPTIVITY_EN = %d\n", CONFIG_RTW_ADAPTIVITY_EN);
	RTW_PRINT_SEL(sel, "CONFIG_RTW_ADAPTIVITY_MODE = %d\n", CONFIG_RTW_ADAPTIVITY_MODE);

#ifdef CONFIG_WOWLAN
	RTW_PRINT_SEL(sel, "CONFIG_WOWLAN - ");

#ifdef CONFIG_GPIO_WAKEUP
	RTW_PRINT_SEL(sel, "CONFIG_GPIO_WAKEUP - WAKEUP_GPIO_IDX:%d\n", WAKEUP_GPIO_IDX);
#endif
#endif

#ifdef CONFIG_TDLS
	RTW_PRINT_SEL(sel, "CONFIG_TDLS\n");
#endif

#ifdef CONFIG_RTW_80211R
	RTW_PRINT_SEL(sel, "CONFIG_RTW_80211R\n");
#endif

#ifdef CONFIG_RTW_NETIF_SG
	RTW_PRINT_SEL(sel, "CONFIG_RTW_NETIF_SG\n");
#endif

#ifdef CONFIG_RTW_WIFI_HAL
	RTW_PRINT_SEL(sel, "CONFIG_RTW_WIFI_HAL\n");
#endif

#ifdef RTW_BUSY_DENY_SCAN
	RTW_PRINT_SEL(sel, "RTW_BUSY_DENY_SCAN\n");
	RTW_PRINT_SEL(sel, "BUSY_TRAFFIC_SCAN_DENY_PERIOD = %u ms\n", \
		      BUSY_TRAFFIC_SCAN_DENY_PERIOD);
#endif


#ifdef CONFIG_USB_HCI
#ifdef CONFIG_SUPPORT_USB_INT
	RTW_PRINT_SEL(sel, "CONFIG_SUPPORT_USB_INT\n");
#endif
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	RTW_PRINT_SEL(sel, "CONFIG_USB_INTERRUPT_IN_PIPE\n");
#endif
#ifdef CONFIG_USB_TX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_USB_TX_AGGREGATION\n");
#endif
#ifdef CONFIG_USB_RX_AGGREGATION
	RTW_PRINT_SEL(sel, "CONFIG_USB_RX_AGGREGATION\n");
#endif
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_TX
	RTW_PRINT_SEL(sel, "CONFIG_USE_USB_BUFFER_ALLOC_TX\n");
#endif
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_RX
	RTW_PRINT_SEL(sel, "CONFIG_USE_USB_BUFFER_ALLOC_RX\n");
#endif

#ifdef CONFIG_FIX_NR_BULKIN_BUFFER
	RTW_PRINT_SEL(sel, "CONFIG_FIX_NR_BULKIN_BUFFER\n");
#endif
#endif /*CONFIG_USB_HCI*/

#ifdef CONFIG_SDIO_HCI
#ifdef DBG_SDIO
	RTW_PRINT_SEL(sel, "DBG_SDIO = %d\n", DBG_SDIO);
#endif

#ifdef RTW_WKARD_SDIO_TX_USE_YIELD
	RTW_PRINT_SEL(sel, "RTW_WKARD_SDIO_TX_USE_YIELD\n");
#endif
#endif /*CONFIG_SDIO_HCI*/

#ifdef CONFIG_PCI_HCI
#endif

	RTW_PRINT_SEL(sel, "CONFIG_IFACE_NUMBER = %d\n", CONFIG_IFACE_NUMBER);


#ifdef CONFIG_PCI_TX_POLLING
	RTW_PRINT_SEL(sel, "CONFIG_PCI_TX_POLLING\n");
#endif
	RTW_PRINT_SEL(sel, "CONFIG_RTW_UP_MAPPING_RULE = %s\n", (CONFIG_RTW_UP_MAPPING_RULE == 1) ? "dscp" : "tos");

	/*GEORGIA_TODO_TRX - need get trx buff accroding to IC spec*/
	RTW_PRINT_SEL(sel, "\n=== XMIT-INFO ===\n");
	RTW_PRINT_SEL(sel, "NR_XMITFRAME = %d\n", NR_XMITFRAME);
	RTW_PRINT_SEL(sel, "MAX_TX_PHL_RING_ENTRY_NUM = %d\n", MAX_PHL_TX_RING_ENTRY_NUM);
#ifdef CONFIG_QUOTA_TURBO_ENABLE
	RTW_PRINT_SEL(sel, "CONFIG_QUOTA_TURBO_ENABLE\n");
#endif
#ifdef CONFIG_USB_HCI
	RTW_PRINT_SEL(sel, "MAX_XMITBUF_SZ = %d\n", MAX_XMITBUF_SZ);
	RTW_PRINT_SEL(sel, "NR_XMITBUFF = %d\n", NR_XMITBUFF);
	RTW_PRINT_SEL(sel, "MAX_MGNT_XMITBUF_SZ = %d\n", MAX_MGNT_XMITBUF_SZ);
	RTW_PRINT_SEL(sel, "NR_MGNT_XMITBUFF = %d\n", NR_MGNT_XMITBUFF);
#endif

	RTW_PRINT_SEL(sel, "\n=== RECV-INFO ===\n");
	RTW_PRINT_SEL(sel, "NR_RECVFRAME = %d\n", NR_RECVFRAME);
	RTW_PRINT_SEL(sel, "MAX_RX_PHL_RING_ENTRY_NUM = %d\n", MAX_PHL_RX_RING_ENTRY_NUM);
#ifdef CONFIG_USB_HCI
	RTW_PRINT_SEL(sel, "MAX_RECVBUF_SZ = %d\n", MAX_RECVBUF_SZ);
	RTW_PRINT_SEL(sel, "NR_RECVBUFF = %d\n", NR_RECVBUFF);
	RTW_PRINT_SEL(sel, "NR_RECV_URB = %d\n", NR_RECV_URB);
#endif
}

void dump_log_level(void *sel)
{
#ifdef CONFIG_RTW_DEBUG
	int i;

	RTW_PRINT_SEL(sel, "drv_log_level:%d\n", rtw_drv_log_level);
	for (i = 0; i <= _DRV_MAX_; i++) {
		if (rtw_log_level_str[i])
			RTW_PRINT_SEL(sel, "%c %s = %d\n",
				(rtw_drv_log_level == i) ? '+' : ' ', rtw_log_level_str[i], i);
	}
#else
	RTW_PRINT_SEL(sel, "CONFIG_RTW_DEBUG is disabled\n");
#endif
}


void rtw_sink_rtp_seq_dbg(_adapter *adapter, u8 *ehdr_pos)
{
	struct recv_info *precvinfo = &(adapter->recvinfo);
	if (precvinfo->sink_udpport > 0) {
		if (*((u16 *)(ehdr_pos + 0x24)) == cpu_to_be16(precvinfo->sink_udpport)) {
			precvinfo->pre_rtp_rxseq = precvinfo->cur_rtp_rxseq;
			precvinfo->cur_rtp_rxseq = be16_to_cpu(*((u16 *)(ehdr_pos + 0x2C)));
			if (precvinfo->pre_rtp_rxseq + 1 != precvinfo->cur_rtp_rxseq) {
				if(precvinfo->pre_rtp_rxseq == 65535 ) {
					if( precvinfo->cur_rtp_rxseq != 0) {
						RTW_INFO("%s : RTP Seq num from %d to %d\n", __FUNCTION__, precvinfo->pre_rtp_rxseq, precvinfo->cur_rtp_rxseq);
					}
				} else {
					RTW_INFO("%s : RTP Seq num from %d to %d\n", __FUNCTION__, precvinfo->pre_rtp_rxseq, precvinfo->cur_rtp_rxseq);
				}
			}	
		}
	}
}

void sta_rx_reorder_ctl_dump(void *sel, struct sta_info *sta)
{
	struct recv_reorder_ctrl *reorder_ctl;
	int i;

	for (i = 0; i < 16; i++) {
		reorder_ctl = &sta->recvreorder_ctrl[i];
		if (reorder_ctl->ampdu_size != RX_AMPDU_SIZE_INVALID || reorder_ctl->indicate_seq != 0xFFFF) {
			RTW_PRINT_SEL(sel, "tid=%d, enable=%d, ampdu_size=%u, indicate_seq=%u\n"
				, i, reorder_ctl->enable, reorder_ctl->ampdu_size, reorder_ctl->indicate_seq
				     );
		}
	}
}

void dump_tx_rate_bmp(void *sel, struct dvobj_priv *dvobj)
{
	_adapter *adapter = dvobj_get_primary_adapter(dvobj);
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	u8 bw;

	RTW_PRINT_SEL(sel, "%-6s", "bw");
	if (rtw_hw_chk_proto_cap(dvobj, PROTO_CAP_11AC))
		_RTW_PRINT_SEL(sel, " %-15s", "vht");

	_RTW_PRINT_SEL(sel, " %-11s %-4s %-3s\n", "ht", "ofdm", "cck");

	for (bw = CHANNEL_WIDTH_20; bw <= CHANNEL_WIDTH_160; bw++) {
		if (!rtw_hw_is_bw_support(adapter_to_dvobj(adapter), bw))
			continue;

		RTW_PRINT_SEL(sel, "%6s", ch_width_str(bw));
		if (rtw_hw_chk_proto_cap(dvobj, PROTO_CAP_11AC)) {
			_RTW_PRINT_SEL(sel, " %03x %03x %03x %03x"
				, RATE_BMP_GET_VHT_4SS(rfctl->rate_bmp_vht_by_bw[bw])
				, RATE_BMP_GET_VHT_3SS(rfctl->rate_bmp_vht_by_bw[bw])
				, RATE_BMP_GET_VHT_2SS(rfctl->rate_bmp_vht_by_bw[bw])
				, RATE_BMP_GET_VHT_1SS(rfctl->rate_bmp_vht_by_bw[bw])
			);
		}

		_RTW_PRINT_SEL(sel, " %02x %02x %02x %02x"
			, bw <= CHANNEL_WIDTH_40 ? RATE_BMP_GET_HT_4SS(rfctl->rate_bmp_ht_by_bw[bw]) : 0
			, bw <= CHANNEL_WIDTH_40 ? RATE_BMP_GET_HT_3SS(rfctl->rate_bmp_ht_by_bw[bw]) : 0
			, bw <= CHANNEL_WIDTH_40 ? RATE_BMP_GET_HT_2SS(rfctl->rate_bmp_ht_by_bw[bw]) : 0
			, bw <= CHANNEL_WIDTH_40 ? RATE_BMP_GET_HT_1SS(rfctl->rate_bmp_ht_by_bw[bw]) : 0
		);

		_RTW_PRINT_SEL(sel, "  %03x   %01x\n"
			, bw <= CHANNEL_WIDTH_20 ? RATE_BMP_GET_OFDM(rfctl->rate_bmp_cck_ofdm) : 0
			, bw <= CHANNEL_WIDTH_20 ? RATE_BMP_GET_CCK(rfctl->rate_bmp_cck_ofdm) : 0
		);
	}
}

void dump_adapters_status(void *sel, struct dvobj_priv *dvobj)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	int i;
	_adapter *iface;
	u8 u_ch, u_bw, u_offset;
	struct _ADAPTER_LINK *iface_link;

	dump_mi_status(sel, dvobj);

	RTW_PRINT_SEL(sel, "dev status:%s%s\n\n"
		, dev_is_surprise_removed(dvobj) ? " SR" : ""
		, dev_is_drv_stopped(dvobj) ? " DS" : ""
	);

#ifdef CONFIG_P2P
#define P2P_INFO_TITLE_FMT	" %-3s %-4s"
#define P2P_INFO_TITLE_ARG	, "lch", "p2ps"
#ifdef CONFIG_IOCTL_CFG80211
#define P2P_INFO_VALUE_FMT	" %3u %c"
#define P2P_INFO_VALUE_ARG	, iface->wdinfo.listen_channel, iface->wdev_data.p2p_enabled ? 'e' : ' '
#else
#define P2P_INFO_VALUE_FMT	" %3u"
#define P2P_INFO_VALUE_ARG	, iface->wdinfo.listen_channel
#endif
#define P2P_INFO_DASH		"---------"
#else
#define P2P_INFO_TITLE_FMT	""
#define P2P_INFO_TITLE_ARG
#define P2P_INFO_VALUE_FMT	""
#define P2P_INFO_VALUE_ARG
#define P2P_INFO_DASH
#endif

#define INFO_FMT	""
#define INFO_ARG
#define INFO_CNT_FMT	""
#define INFO_CNT_ARG


	RTW_PRINT_SEL(sel, "%-2s %-15s %c %-3s %-17s %-4s %-7s %-5s"
		P2P_INFO_TITLE_FMT
		" %s"INFO_FMT"\n"
		, "id", "ifname", ' ', "netif_up", "macaddr", "port", "ch", "class"
		P2P_INFO_TITLE_ARG
		, "status"INFO_ARG);

	RTW_PRINT_SEL(sel, "---------------------------------------------------------------"
		P2P_INFO_DASH
		"-------\n");

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			iface_link = GET_PRIMARY_LINK(iface);
			RTW_PRINT_SEL(sel, "%2d %-15s %c %3u "MAC_FMT" %4hhu %3u,%u,%u %5u"
				P2P_INFO_VALUE_FMT
				" "MLME_STATE_FMT" " INFO_CNT_FMT"\n"
				, i, iface->registered ? ADPT_ARG(iface) : NULL
				, iface->registered ? 'R' : ' '
				, iface->netif_up
				, MAC_ARG(adapter_mac_addr(iface))
				, (iface_link->wrlink) ? iface_link->wrlink->hw_port : HW_PORT_MAX
				, iface_link->mlmeextpriv.chandef.chan
				, iface_link->mlmeextpriv.chandef.bw
				, iface_link->mlmeextpriv.chandef.offset
				, rtw_get_op_class_by_chbw(iface_link->mlmeextpriv.chandef.chan
					, iface_link->mlmeextpriv.chandef.bw
					, iface_link->mlmeextpriv.chandef.offset)
				P2P_INFO_VALUE_ARG
				, MLME_STATE_ARG(iface)
				INFO_CNT_ARG
			);
		}
	}

	RTW_PRINT_SEL(sel, "---------------------------------------------------------------"
		P2P_INFO_DASH
		"-------\n");
#if 0
	if (rtw_mi_get_ch_setting_union(dvobj_get_primary_adapter(dvobj), &u_ch, &u_bw, &u_offset))
		RTW_PRINT_SEL(sel, "%55s %3u,%u,%u %5u\n"
			, "union:"
			, u_ch, u_bw, u_offset, rtw_get_op_class_by_chbw(u_ch, u_bw, u_offset));

	RTW_PRINT_SEL(sel, "%55s %3u,%u,%u offch_state:%d\n"
		, "oper:"
		, dvobj->chandef.chan
		, dvobj->chandef.bw
		, dvobj->chandef.offset
		, rfctl->offch_state
	);
#endif

	#ifdef CONFIG_DFS_MASTER
	dump_radar_detect_status(sel, rfctl, "                                          radar_detect:");
	#endif
}

#define SEC_CAM_ENT_ID_TITLE_FMT "%-2s"
#define SEC_CAM_ENT_ID_TITLE_ARG "id"
#define SEC_CAM_ENT_ID_VALUE_FMT "%2u"
#define SEC_CAM_ENT_ID_VALUE_ARG(id) (id)

#define SEC_CAM_ENT_TITLE_FMT "%-6s %-17s %-32s %-3s %-8s %-2s %-2s %-5s"
#define SEC_CAM_ENT_TITLE_ARG "ctrl", "addr", "key", "kid", "type", "MK", "GK", "valid"
#define SEC_CAM_ENT_VALUE_FMT "0x%04x "MAC_FMT" "KEY_FMT" %3u %-8s %2u %2u %5u"
#define SEC_CAM_ENT_VALUE_ARG(ent) \
	(ent)->ctrl \
	, MAC_ARG((ent)->mac) \
	, KEY_ARG((ent)->key) \
	, ((ent)->ctrl) & 0x03 \
	, (((ent)->ctrl) & 0x200) ? \
	security_type_str((((ent)->ctrl) >> 2 & 0x7) | _SEC_TYPE_256_) : \
	security_type_str(((ent)->ctrl) >> 2 & 0x7) \
	, (((ent)->ctrl) >> 5) & 0x01 \
	, (((ent)->ctrl) >> 6) & 0x01 \
	, (((ent)->ctrl) >> 15) & 0x01

void dump_sec_cam_ent(void *sel, struct sec_cam_ent *ent, int id)
{
	if (id >= 0) {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_ID_VALUE_FMT " " SEC_CAM_ENT_VALUE_FMT"\n"
			, SEC_CAM_ENT_ID_VALUE_ARG(id), SEC_CAM_ENT_VALUE_ARG(ent));
	} else
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_VALUE_FMT"\n", SEC_CAM_ENT_VALUE_ARG(ent));
}

void dump_sec_cam_ent_title(void *sel, u8 has_id)
{
	if (has_id) {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_ID_TITLE_FMT " " SEC_CAM_ENT_TITLE_FMT"\n"
			, SEC_CAM_ENT_ID_TITLE_ARG, SEC_CAM_ENT_TITLE_ARG);
	} else
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_TITLE_FMT"\n", SEC_CAM_ENT_TITLE_ARG);
}

void dump_sec_cam(void *sel, _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	struct sec_cam_ent ent;
	int i;

	RTW_PRINT_SEL(sel, "HW sec cam:\n");
	dump_sec_cam_ent_title(sel, 1);
	for (i = 0; i < cam_ctl->num; i++) {
		rtw_hal_sec_read_cam_ent(adapter, i, (u8 *)(&ent.ctrl), ent.mac, ent.key);
		dump_sec_cam_ent(sel , &ent, i);
	}
}

void dump_sec_cam_cache(void *sel, _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;

	RTW_PRINT_SEL(sel, "SW sec cam cache:\n");
	dump_sec_cam_ent_title(sel, 1);
	for (i = 0; i < cam_ctl->num; i++) {
		if (dvobj->cam_cache[i].ctrl != 0)
			dump_sec_cam_ent(sel, &dvobj->cam_cache[i], i);
	}

}

static u8 del_rx_ampdu_test_no_tx_fail = 0;

bool rtw_del_rx_ampdu_test_trigger_no_tx_fail(void)
{
	if (del_rx_ampdu_test_no_tx_fail) {
		RTW_PRINT("del_rx_ampdu test case: trigger no_tx_fail\n");
		del_rx_ampdu_test_no_tx_fail--;
		return _TRUE;
	}
	return _FALSE;
}

static u32 g_wait_hiq_empty_ms = 0;

u32 rtw_get_wait_hiq_empty_ms(void)
{
	return g_wait_hiq_empty_ms;
}

static systime sta_linking_test_start_time = 0;
static u32 sta_linking_test_wait_ms = 0;
static u8 sta_linking_test_force_fail = 0;

void rtw_sta_linking_test_set_start(void)
{
	sta_linking_test_start_time = rtw_get_current_time();
}

bool rtw_sta_linking_test_wait_done(void)
{
	return rtw_get_passing_time_ms(sta_linking_test_start_time) >= sta_linking_test_wait_ms;
}

bool rtw_sta_linking_test_force_fail(void)
{
	return sta_linking_test_force_fail;
}

#ifdef CONFIG_AP_MODE
static u16 ap_linking_test_force_auth_fail = 0;
static u16 ap_linking_test_force_asoc_fail = 0;

u16 rtw_ap_linking_test_force_auth_fail(void)
{
	return ap_linking_test_force_auth_fail;
}

u16 rtw_ap_linking_test_force_asoc_fail(void)
{
	return ap_linking_test_force_asoc_fail;
}
#endif

int proc_get_defs_param(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlme = &adapter->mlmepriv;

	RTW_PRINT_SEL(m, "%s %15s\n", "lmt_sta", "lmt_time");
	RTW_PRINT_SEL(m, "%-15u %-15u\n"
		, mlme->defs_lmt_sta
		, mlme->defs_lmt_time
	);

	return 0;
}

ssize_t proc_set_defs_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlme = &adapter->mlmepriv;

	char tmp[32];
	u32 defs_lmt_sta;
	u32 defs_lmt_time;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%u %u", &defs_lmt_sta, &defs_lmt_time);

		if (num >= 1)
			mlme->defs_lmt_sta = defs_lmt_sta;
		if (num >= 2)
			mlme->defs_lmt_time = defs_lmt_time;
	}

	return count;

}

ssize_t proc_set_write_reg(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char tmp[32];
	u32 addr, val, len;

	if (count < 3) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x %x %x", &addr, &val, &len);

		if (num !=  3) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}

		switch (len) {
		case 1:
			rtw_phl_write8(dvobj->phl, addr, (u8)val);
			break;
		case 2:
			rtw_phl_write16(dvobj->phl, addr, (u16)val);
			break;
		case 4:
			rtw_phl_write32(dvobj->phl, addr, val);
			break;
		default:
			RTW_INFO("error write length=%d", len);
			break;
		}

	}
	return count;

}

static u32 proc_get_read_addr = 0xeeeeeeee;
static u32 proc_get_read_len = 0x4;

int proc_get_read_reg(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (proc_get_read_addr == 0xeeeeeeee) {
		RTW_PRINT_SEL(m, "address not initialized\n");
		return 0;
	}

	switch (proc_get_read_len) {
	case 1:
		RTW_PRINT_SEL(m, "reg_read8(0x%x)=0x%x\n", proc_get_read_addr,
			rtw_phl_read8(dvobj->phl, proc_get_read_addr));
		break;
	case 2:
		RTW_PRINT_SEL(m, "reg_read16(0x%x)=0x%x\n", proc_get_read_addr,
			rtw_phl_read16(dvobj->phl, proc_get_read_addr));
		break;
	case 4:
		RTW_PRINT_SEL(m, "reg_read32(0x%x)=0x%x\n", proc_get_read_addr,
			rtw_phl_read32(dvobj->phl, proc_get_read_addr));
		break;
	default:
		RTW_PRINT_SEL(m, "error read length=%d\n", proc_get_read_len);
		break;
	}

	return 0;
}

ssize_t proc_set_read_reg(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[16];
	u32 addr, len;

	if (count < 2) {
		RTW_INFO("argument size is less than 2\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x %x", &addr, &len);

		if (num !=  2) {
			RTW_INFO("invalid read_reg parameter!\n");
			return count;
		}

		proc_get_read_addr = addr;

		proc_get_read_len = len;
	}

	return count;

}

ssize_t proc_set_mac_dbg_status_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char tmp[32] = {0};
	u32 val[2] = {0};
	u32 en;
	u8 en_u8;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x", &en);

		if (num !=  1) {
			RTW_INFO("invalid mac_dbg_status_dump parameter!\n");
			return count;
		}
		en_u8 = (u8)en;
		rtw_phl_mac_dbg_status_dump(dvobj->phl, val, &en_u8);
		if (en & BIT0)
			RTW_INFO_DUMP("ss_dbgpkg: ", (const u8 *)val, 8);
	}
	return count;

}

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
int proc_get_ignore_go_and_low_rssi_in_scan(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	RTW_PRINT_SEL(m, "ignore_go_in_scan=%d, ignore_low_rssi_in_scan=%d\n",
		pregpriv->ignore_go_in_scan, pregpriv->ignore_low_rssi_in_scan);
	return 0;
}

ssize_t proc_set_ignore_go_and_low_rssi_in_scan(struct file *file,
	const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	char tmp[32] = {0};
	int enable = 0;
	int num = 0, low_rssi=0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num	= sscanf(tmp, "%d %d", &enable, &low_rssi);
		RTW_INFO("num=%d enable=%d low_rssi=%d\n", num, enable, low_rssi);

		if (num != 2) {
			RTW_INFO("argument number is wrong\n");
			return -EFAULT;
		}

		pregpriv->ignore_go_in_scan = enable;
		pregpriv->ignore_low_rssi_in_scan = low_rssi;
	}
	return count;
}
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/

#ifdef CONFIG_PROC_DEBUG
int proc_get_rx_stat(struct seq_file *m, void *v)
{
	_list	*plist, *phead;
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_info *psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct sta_priv		*pstapriv = &(adapter->stapriv);
	u32 i, j;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			pstats = &psta->sta_stats;

			if (pstats == NULL)
				continue;
			if ((_rtw_memcmp(psta->phl_sta->mac_addr, bc_addr, ETH_ALEN) !=  _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, null_addr, ETH_ALEN) != _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, psta->padapter_link->mac_addr, ETH_ALEN) != _TRUE)) {
				RTW_PRINT_SEL(m, "MAC :\t\t"MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
				RTW_PRINT_SEL(m, "data_rx_cnt :\t%llu\n", sta_rx_data_uc_pkts(psta) - pstats->last_rx_data_uc_pkts);
				pstats->last_rx_data_uc_pkts = sta_rx_data_uc_pkts(psta);
				RTW_PRINT_SEL(m, "duplicate_cnt :\t%u\n", pstats->duplicate_cnt);
				pstats->duplicate_cnt = 0;
				RTW_PRINT_SEL(m, "rx_per_rate_cnt :\n");

				for (j = 0; j < 0x60; j++) {
					RTW_PRINT_SEL(m, "%08u  ", pstats->rxratecnt[j]);
					pstats->rxratecnt[j] = 0;
					if ((j%8) == 7)
						RTW_PRINT_SEL(m, "\n");
				}
				RTW_PRINT_SEL(m, "\n");
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	return 0;
}

int proc_get_sta_tx_stat(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv	*stapriv = &(adapter->stapriv);
	struct sta_priv	*stapriv_primary = &(GET_PRIMARY_ADAPTER(adapter))->stapriv;
	struct sta_info *sta;
	struct stainfo_stats *stats;
	u8 mac[ETH_ALEN] = {0};
	int ret = 0;

	_rtw_memcpy(mac, stapriv_primary->c2h_sta_mac, ETH_ALEN);
	sta = rtw_get_stainfo(stapriv, mac);
	if (sta) {
		ret = rtw_get_sta_tx_stat(adapter, sta);
		stats = &sta->sta_stats;
		RTW_PRINT_SEL(m, "MAC: " MAC_FMT " fail: %u retry: %u\n",
			      MAC_ARG(mac), stats->tx_fail_cnt_sum, stats->tx_retry_cnt_sum);
	} else
		ret = -ENODATA;
	return ret;
}

ssize_t proc_set_sta_tx_stat(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv	*stapriv_primary = &(GET_PRIMARY_ADAPTER(adapter))->stapriv;
	char tmp[18] = {0};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (sscanf(tmp, MAC_SFMT, MAC_SARG(stapriv_primary->c2h_sta_mac)) != 6) {
			_rtw_memset(stapriv_primary->c2h_sta_mac, 0, 6);
			RTW_PRINT(FUNC_ADPT_FMT" Invalid format\n",
				  FUNC_ADPT_ARG(adapter));
		}

	}

	return count;
}

int proc_get_tx_stat(struct seq_file *m, void *v)
{
	_list	*plist, *phead;
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_info *psta = NULL;
	u8 sta_mac[NUM_STA][ETH_ALEN] = {{0}};
	uint mac_id[NUM_STA];
	struct stainfo_stats	*pstats = NULL;
	struct sta_priv	*pstapriv = &(adapter->stapriv);
	struct sta_priv	*pstapriv_primary = &(GET_PRIMARY_ADAPTER(adapter))->stapriv;
	u32 i, macid_rec_idx = 0;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if ((_rtw_memcmp(psta->phl_sta->mac_addr, bc_addr, ETH_ALEN) !=  _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, null_addr, ETH_ALEN) != _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, psta->padapter_link->mac_addr, ETH_ALEN) != _TRUE)) {
				_rtw_memcpy(&sta_mac[macid_rec_idx][0], psta->phl_sta->mac_addr, ETH_ALEN);
				mac_id[macid_rec_idx] = psta->phl_sta->macid;
				macid_rec_idx++;
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < macid_rec_idx; i++) {
		psta = rtw_get_stainfo(pstapriv, &sta_mac[i][0]);
		if(psta) {
			rtw_get_sta_tx_stat(adapter, psta);
			pstats = &psta->sta_stats;
#if defined(PRIVATE_R) || defined(CONFIG_RTW_MULTI_AP)
			RTW_PRINT_SEL(m, "MAC: " MAC_FMT " sent: %u fail: %u retry: %u\n",
			MAC_ARG(&sta_mac[i][0]), pstats->tx_ok_cnt, pstats->tx_fail_cnt, pstats->tx_retry_cnt);
#else
			RTW_PRINT_SEL(m, "data_sent_cnt :\t%u\n", pstats->tx_ok_cnt + pstats->tx_fail_cnt);
			RTW_PRINT_SEL(m, "success_cnt :\t%u\n", pstats->tx_ok_cnt);
			RTW_PRINT_SEL(m, "failure_cnt :\t%u\n", pstats->tx_fail_cnt);
			RTW_PRINT_SEL(m, "retry_cnt :\t%u\n\n", pstats->tx_retry_cnt);
#endif

		} else
			RTW_PRINT_SEL(m, "STA is gone\n");
	}
	return 0;
}

int proc_get_fwstate(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	RTW_PRINT_SEL(m, "fwstate=0x%x\n", get_fwstate(pmlmepriv));

	return 0;
}

int proc_get_sec_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct security_priv *sec = &padapter->securitypriv;

	RTW_PRINT_SEL(m, "auth_alg=0x%x, enc_alg=0x%x, auth_type=0x%x, enc_type=0x%x\n",
		sec->dot11AuthAlgrthm, sec->dot11PrivacyAlgrthm,
		sec->ndisauthtype, sec->ndisencryptstatus);

	RTW_PRINT_SEL(m, "hw_decrypted=%d\n", sec->hw_decrypted);
	RTW_PRINT_SEL(m, "wpa_psk=");
	if(sec->wpa_psk & BIT(0))
		RTW_PRINT_SEL(m, "WPA ");
	if(sec->wpa_psk & BIT(1))
		RTW_PRINT_SEL(m, "WPA2 ");
	RTW_PRINT_SEL(m, "\n");
	RTW_PRINT_SEL(m, "wpa2_group_cipher=%d\n", sec->wpa2_group_cipher);
	RTW_PRINT_SEL(m, "wpa2_pairwise_cipher=%d\n", sec->wpa2_pairwise_cipher);
	RTW_PRINT_SEL(m, "wpa_group_cipher=%d\n", sec->wpa_group_cipher);
	RTW_PRINT_SEL(m, "wpa_pairwise_cipher=%d\n", sec->wpa_pairwise_cipher);
	RTW_PRINT_SEL(m, "80211W_enable=%d\n", sec->mfp_opt);
	RTW_PRINT_SEL(m, "akm_suite_bitmap=0x%x\n", sec->akmp);

#ifdef DBG_SW_SEC_CNT
	RTW_PRINT_SEL(m, "==sw counters(bc, mc, uc)==\n");
	RTW_PRINT_SEL(m, "wep_sw_enc_cnt=%llu, %llu, %llu\n"
		, sec->wep_sw_enc_cnt_bc , sec->wep_sw_enc_cnt_mc, sec->wep_sw_enc_cnt_uc);
	RTW_PRINT_SEL(m, "wep_sw_dec_cnt=%llu, %llu, %llu\n"
		, sec->wep_sw_dec_cnt_bc , sec->wep_sw_dec_cnt_mc, sec->wep_sw_dec_cnt_uc);

	RTW_PRINT_SEL(m, "tkip_sw_enc_cnt=%llu, %llu, %llu\n"
		, sec->tkip_sw_enc_cnt_bc , sec->tkip_sw_enc_cnt_mc, sec->tkip_sw_enc_cnt_uc);
	RTW_PRINT_SEL(m, "tkip_sw_dec_cnt=%llu, %llu, %llu\n"
		, sec->tkip_sw_dec_cnt_bc , sec->tkip_sw_dec_cnt_mc, sec->tkip_sw_dec_cnt_uc);

	RTW_PRINT_SEL(m, "aes_sw_enc_cnt=%llu, %llu, %llu\n"
		, sec->aes_sw_enc_cnt_bc , sec->aes_sw_enc_cnt_mc, sec->aes_sw_enc_cnt_uc);
	RTW_PRINT_SEL(m, "aes_sw_dec_cnt=%llu, %llu, %llu\n"
		, sec->aes_sw_dec_cnt_bc , sec->aes_sw_dec_cnt_mc, sec->aes_sw_dec_cnt_uc);

	RTW_PRINT_SEL(m, "gcmp_sw_enc_cnt=%llu, %llu, %llu\n"
		, sec->gcmp_sw_enc_cnt_bc , sec->gcmp_sw_enc_cnt_mc, sec->gcmp_sw_enc_cnt_uc);
	RTW_PRINT_SEL(m, "gcmp_sw_dec_cnt=%llu, %llu, %llu\n"
		, sec->gcmp_sw_dec_cnt_bc , sec->gcmp_sw_dec_cnt_mc, sec->gcmp_sw_dec_cnt_uc);
#endif /* DBG_SW_SEC_CNT */

	return 0;
}

int proc_get_mlmext_state(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_PRINT_SEL(m, "pmlmeinfo->state=0x%x\n", pmlmeinfo->state);

	return 0;
}

#ifdef CONFIG_LAYER2_ROAMING
int proc_get_roam_flags(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "0x%02x\n", rtw_roam_flags(adapter));

	return 0;
}

ssize_t proc_set_roam_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &flags);

		if (num == 1)
			rtw_assign_roam_flags(adapter, flags);
	}

	return count;

}

int proc_get_roam_param(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlme = &adapter->mlmepriv;

	RTW_PRINT_SEL(m, "%12s %15s %26s %16s\n", "rssi_diff_th", "scanr_exp_ms", "scan_interval(unit:2 sec)", "rssi_threshold");
	RTW_PRINT_SEL(m, "%-15u %-13u %-27u %-11u\n"
		, mlme->roam_rssi_diff_th
		, mlme->roam_scanr_exp_ms
		, mlme->roam_scan_int
		, mlme->roam_rssi_threshold
	);

	return 0;
}

ssize_t proc_set_roam_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlme = &adapter->mlmepriv;

	char tmp[32];
	u8 rssi_diff_th;
	u32 scanr_exp_ms;
	u32 scan_int;
	u8 rssi_threshold;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %u %u %hhu", &rssi_diff_th, &scanr_exp_ms, &scan_int, &rssi_threshold);

		if (num >= 1)
			mlme->roam_rssi_diff_th = rssi_diff_th;
		if (num >= 2)
			mlme->roam_scanr_exp_ms = scanr_exp_ms;
		if (num >= 3)
			mlme->roam_scan_int = scan_int;
		if (num >= 4)
			mlme->roam_rssi_threshold = rssi_threshold;
	}

	return count;

}

ssize_t proc_set_roam_tgt_addr(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32];
	u8 addr[ETH_ALEN];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", addr, addr + 1, addr + 2, addr + 3, addr + 4, addr + 5);
		if (num == 6)
			_rtw_memcpy(adapter->mlmepriv.roam_tgt_addr, addr, ETH_ALEN);

		RTW_INFO("set roam_tgt_addr to "MAC_FMT"\n", MAC_ARG(adapter->mlmepriv.roam_tgt_addr));
	}

	return count;
}
#endif /* CONFIG_LAYER2_ROAMING */


int proc_get_qos_option(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);

	RTW_PRINT_SEL(m, "qos_option=%d\n", pmlmepriv->qospriv.qos_option);

	return 0;
}

int proc_get_ht_option(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);

#ifdef CONFIG_80211N_HT
	RTW_PRINT_SEL(m, "ht_option=%d\n", pmlmepriv->htpriv.ht_option);
#endif /* CONFIG_80211N_HT */

	return 0;
}

int proc_get_rf_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;

	RTW_PRINT_SEL(m, "cur_ch=%d, cur_bw=%d, cur_ch_offet=%d\n",
		pmlmeext->chandef.chan, pmlmeext->chandef.bw, pmlmeext->chandef.offset);

	RTW_PRINT_SEL(m, "oper_ch=%d, oper_bw=%d, oper_ch_offet=%d\n",
		rtw_get_oper_ch(padapter, padapter_link), rtw_get_oper_bw(padapter, padapter_link),
				rtw_get_oper_choffset(padapter, padapter_link));

	return 0;
}

int proc_get_scan_param(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct ss_res *ss = &mlmeext->sitesurvey_res;

#define SCAN_PARAM_TITLE_FMT "%10s"
#define SCAN_PARAM_VALUE_FMT "%-10u"
#define SCAN_PARAM_TITLE_ARG , "scan_ch_ms"
#define SCAN_PARAM_VALUE_ARG , ss->scan_ch_ms
#ifdef CONFIG_80211N_HT
#define SCAN_PARAM_TITLE_FMT_HT " %15s %13s"
#define SCAN_PARAM_VALUE_FMT_HT " %-15u %-13u"
#define SCAN_PARAM_TITLE_ARG_HT , "rx_ampdu_accept", "rx_ampdu_size"
#define SCAN_PARAM_VALUE_ARG_HT , ss->rx_ampdu_accept, ss->rx_ampdu_size
#else
#define SCAN_PARAM_TITLE_FMT_HT ""
#define SCAN_PARAM_VALUE_FMT_HT ""
#define SCAN_PARAM_TITLE_ARG_HT
#define SCAN_PARAM_VALUE_ARG_HT
#endif
#ifdef CONFIG_SCAN_BACKOP
#define SCAN_PARAM_TITLE_FMT_BACKOP " %9s %12s"
#define SCAN_PARAM_VALUE_FMT_BACKOP " %-9u %-12u"
#define SCAN_PARAM_TITLE_ARG_BACKOP , "backop_ms", "scan_cnt_max"
#define SCAN_PARAM_VALUE_ARG_BACKOP , ss->backop_ms, ss->scan_cnt_max
#else
#define SCAN_PARAM_TITLE_FMT_BACKOP ""
#define SCAN_PARAM_VALUE_FMT_BACKOP ""
#define SCAN_PARAM_TITLE_ARG_BACKOP
#define SCAN_PARAM_VALUE_ARG_BACKOP
#endif

	RTW_PRINT_SEL(m,
		SCAN_PARAM_TITLE_FMT
		SCAN_PARAM_TITLE_FMT_HT
		SCAN_PARAM_TITLE_FMT_BACKOP
		"\n"
		SCAN_PARAM_TITLE_ARG
		SCAN_PARAM_TITLE_ARG_HT
		SCAN_PARAM_TITLE_ARG_BACKOP
	);

	RTW_PRINT_SEL(m,
		SCAN_PARAM_VALUE_FMT
		SCAN_PARAM_VALUE_FMT_HT
		SCAN_PARAM_VALUE_FMT_BACKOP
		"\n"
		SCAN_PARAM_VALUE_ARG
		SCAN_PARAM_VALUE_ARG_HT
		SCAN_PARAM_VALUE_ARG_BACKOP
	);

	return 0;
}

ssize_t proc_set_scan_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct ss_res *ss = &mlmeext->sitesurvey_res;

	char tmp[32] = {0};

	u16 scan_ch_ms;
#define SCAN_PARAM_INPUT_FMT "%hu"
#define SCAN_PARAM_INPUT_ARG , &scan_ch_ms
#ifdef CONFIG_80211N_HT
	u8 rx_ampdu_accept;
	u8 rx_ampdu_size;
#define SCAN_PARAM_INPUT_FMT_HT " %hhu %hhu"
#define SCAN_PARAM_INPUT_ARG_HT , &rx_ampdu_accept, &rx_ampdu_size
#else
#define SCAN_PARAM_INPUT_FMT_HT ""
#define SCAN_PARAM_INPUT_ARG_HT
#endif
#ifdef CONFIG_SCAN_BACKOP
	u16 backop_ms;
	u8 scan_cnt_max;
#define SCAN_PARAM_INPUT_FMT_BACKOP " %hu %hhu"
#define SCAN_PARAM_INPUT_ARG_BACKOP , &backop_ms, &scan_cnt_max
#else
#define SCAN_PARAM_INPUT_FMT_BACKOP ""
#define SCAN_PARAM_INPUT_ARG_BACKOP
#endif

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp,
			SCAN_PARAM_INPUT_FMT
			SCAN_PARAM_INPUT_FMT_HT
			SCAN_PARAM_INPUT_FMT_BACKOP
			SCAN_PARAM_INPUT_ARG
			SCAN_PARAM_INPUT_ARG_HT
			SCAN_PARAM_INPUT_ARG_BACKOP
		);

		if (num-- > 0)
			ss->scan_ch_ms = scan_ch_ms;
#ifdef CONFIG_80211N_HT
		if (num-- > 0)
			ss->rx_ampdu_accept = rx_ampdu_accept;
		if (num-- > 0)
			ss->rx_ampdu_size = rx_ampdu_size;
#endif
#ifdef CONFIG_SCAN_BACKOP
		if (num-- > 0)
			ss->backop_ms = backop_ms;
		if (num-- > 0)
			ss->scan_cnt_max = scan_cnt_max;
#endif
	}

	return count;
}
ssize_t proc_set_scan_abort(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u32 timeout = 0;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = 0;

		num = sscanf(tmp, "%d", &timeout);
		if (num != 1) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}

		rtw_scan_abort(padapter, timeout);
	}

	return count;
}

int proc_get_scan_abort(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);


	return 0;
}

int proc_get_survey_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	dump_scanned_queue(m, adapter);

	return 0;
}

ssize_t proc_set_survey_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 _status = _FALSE;
	u8 ssc_chk;
	char tmp[32] = {0};
	char cmd[8] = {0};
	bool acs = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%s", cmd);

		if (num < 1)
			return count;

		if (strcmp("acs", cmd) == 0)
			acs = 1;
	}

#if 1
	ssc_chk = rtw_sitesurvey_condition_check(padapter, _FALSE);
	if (ssc_chk != SS_ALLOW)
		goto exit;

	if (!rtw_is_adapter_up(padapter)) {
		RTW_INFO("scan abort!! adapter cannot use\n");
		goto exit;
	}
#else
#ifdef CONFIG_MP_INCLUDED
	if (rtw_mp_mode_check(padapter)) {
		RTW_INFO("MP mode block Scan request\n");
		goto exit;
	}
#endif
	if (rtw_is_scan_deny(padapter)) {
		RTW_INFO(FUNC_ADPT_FMT  ": scan deny\n", FUNC_ADPT_ARG(padapter));
		goto exit;
	}

	if (!rtw_is_adapter_up(padapter)) {
		RTW_INFO("scan abort!! adapter cannot use\n");
		goto exit;
	}

	if (rtw_mi_busy_traffic_check(padapter)) {
		RTW_INFO("scan abort!! BusyTraffic == _TRUE\n");
		goto exit;
	}

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) && check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		RTW_INFO("scan abort!! AP mode process WPS\n");
		goto exit;
	}
	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING) == _TRUE) {
		RTW_INFO("scan abort!! fwstate=0x%x\n", pmlmepriv->fw_state);
		goto exit;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter,
		       WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING | WIFI_UNDER_WPS)) {
		RTW_INFO("scan abort!! buddy_fwstate check failed\n");
		goto exit;
	}
#endif
#endif

	if (acs) {
		#ifdef CONFIG_RTW_ACS
		_status = rtw_set_acs_sitesurvey(padapter);
		#endif
	} else
		_status = rtw_sitesurvey_cmd(padapter, NULL);

exit:
	return count;
}
#ifdef PRIVATE_R
int proc_get_infra_ap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct sta_info *psta;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct ht_priv_infra_ap *phtpriv = &pmlmepriv->htpriv_infra_ap;
#ifdef CONFIG_80211AC_VHT
	struct vht_priv_infra_ap *pvhtpriv = &pmlmepriv->vhtpriv_infra_ap;
#endif
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct wlan_network *cur_network = &(pmlmepriv->cur_network);
	struct sta_priv *pstapriv = &padapter->stapriv;

	if (MLME_IS_STA(padapter)) {
		psta = rtw_get_stainfo(pstapriv, cur_network->network.MacAddress);
		if (psta) {
			unsigned int i, j;
			unsigned int Rx_ss = 0, Tx_ss = 0;

			RTW_PRINT_SEL(m, "SSID=%s\n", pmlmeinfo->network.Ssid.Ssid);
			RTW_PRINT_SEL(m, "sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
			RTW_PRINT_SEL(m, "Supported rate=");
			for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
				if (pmlmeinfo->SupportedRates_infra_ap[i] == 0)
					break;
				RTW_PRINT_SEL(m, " 0x%x", pmlmeinfo->SupportedRates_infra_ap[i]);
			}
			RTW_PRINT_SEL(m, "\n");
#ifdef CONFIG_80211N_HT
			if (pmlmeinfo->ht_vht_received & BIT(0)) {
				RTW_PRINT_SEL(m, "Supported MCS set=");
				for (i = 0; i < 16 ; i++)
					RTW_PRINT_SEL(m, " 0x%02x",  phtpriv->MCS_set_infra_ap[i]);
				RTW_PRINT_SEL(m, "\n");
				RTW_PRINT_SEL(m, "highest supported data rate=0x%x\n", phtpriv->rx_highest_data_rate_infra_ap);
				RTW_PRINT_SEL(m, "HT_supported_channel_width_set=0x%x\n", phtpriv->channel_width_infra_ap);
				RTW_PRINT_SEL(m, "sgi_20m=%d, sgi_40m=%d\n", phtpriv->sgi_20m_infra_ap, phtpriv->sgi_40m_infra_ap);
				RTW_PRINT_SEL(m, "ldpc_cap=0x%x, stbc_cap=0x%x\n", phtpriv->ldpc_cap_infra_ap, phtpriv->stbc_cap_infra_ap);
				RTW_PRINT_SEL(m, "HT_number_of_stream=%d\n", phtpriv->Rx_ss_infra_ap);
			}
#endif

#ifdef CONFIG_80211AC_VHT
			if (pmlmeinfo->ht_vht_received & BIT(1)) {
				RTW_PRINT_SEL(m, "VHT_supported_channel_width_set=0x%x\n", pvhtpriv->channel_width_infra_ap);
				RTW_PRINT_SEL(m, "vht_ldpc_cap=0x%x, vht_stbc_cap=0x%x, vht_beamform_cap=0x%x\n", pvhtpriv->ldpc_cap_infra_ap, pvhtpriv->stbc_cap_infra_ap, pvhtpriv->beamform_cap_infra_ap);
				RTW_PRINT_SEL(m, "Rx_vht_mcs_map=0x%x, Tx_vht_mcs_map=0x%x\n", *(u16 *)pvhtpriv->vht_mcs_map_infra_ap, *(u16 *)pvhtpriv->vht_mcs_map_tx_infra_ap);
				RTW_PRINT_SEL(m, "VHT_number_of_stream=%d\n", pvhtpriv->number_of_streams_infra_ap);
			}
#endif
		} else
			RTW_PRINT_SEL(m, "can't get sta's macaddr, cur_network's macaddr:" MAC_FMT "\n", MAC_ARG(cur_network->network.MacAddress));
	} else
		RTW_PRINT_SEL(m, "this only applies to STA mode\n");
	return 0;
}

#endif /* PRIVATE_R */

static int wireless_mode_to_str(u32 mode, char *str)
{
	str[0]='\0';
	if (mode & WLAN_MD_11A)
		sprintf(str+strlen(str),"%s","A/");
	if (mode & WLAN_MD_11B)
		sprintf(str+strlen(str),"%s","B/");
	if (mode & WLAN_MD_11G)
		sprintf(str+strlen(str),"%s","G/");
	#ifdef CONFIG_80211N_HT
	if (mode & WLAN_MD_11N)
		sprintf(str+strlen(str),"%s","N/");
	#endif
	#ifdef CONFIG_80211AC_VHT
	if (mode & WLAN_MD_11AC)
		sprintf(str+strlen(str),"%s","AC/");
	#endif
	#ifdef CONFIG_80211AX_HE
	if (mode & WLAN_MD_11AX)
		sprintf(str+strlen(str),"%s","AX/");
	#endif
	if (strlen(str)>1)
		str[strlen(str)-1]='\0';

	return strlen(str);
}

int proc_get_ap_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct sta_info *psta;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct wlan_network *cur_network = &(pmlmepriv->cur_network);
	struct mlme_ext_info	*pmlmeinfo = &(padapter->mlmeextpriv.mlmext_info);
	struct sta_priv *pstapriv = &padapter->stapriv;
	char wl_mode[16];

	/* ap vendor */
	char vendor[VENDOR_NAME_LEN] = {0};
	get_assoc_AP_Vendor(vendor, pmlmeinfo->assoc_AP_vendor);
	RTW_PRINT_SEL(m,"AP Vendor %s\n", vendor);

	psta = rtw_get_stainfo(pstapriv, cur_network->network.MacAddress);
	if (psta) {
		wireless_mode_to_str(psta->phl_sta->wmode, wl_mode);
		RTW_PRINT_SEL(m, "SSID=%s\n", cur_network->network.Ssid.Ssid);
		RTW_PRINT_SEL(m, "sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
		RTW_PRINT_SEL(m, "cur_channel=%d, cur_bwmode=%d(%s), cur_ch_offset=%d\n", pmlmeext->chandef.chan, pmlmeext->chandef.bw, ch_width_str(pmlmeext->chandef.bw), pmlmeext->chandef.offset);
		RTW_PRINT_SEL(m, "wireless_mode=0x%x(%s), rtsen=%d, cts2slef=%d hw_rts_en=%d\n",
				psta->phl_sta->wmode, wl_mode, psta->rtsen, psta->cts2self, psta->hw_rts_en);
		/* ToDo: need API to query hal_sta->ra_info.rate_id */
		/* RTW_PRINT_SEL(m, "state=0x%x, aid=%d, macid=%d, raid=%d\n",
			psta->state, psta->phl_sta->aid, psta->phl_sta->macid, psta->phl_sta->ra_info.rate_id); */
		RTW_PRINT_SEL(m, "state=0x%x, aid=%d, macid=%d\n",
			psta->state, psta->phl_sta->aid, psta->phl_sta->macid);
#ifdef CONFIG_80211N_HT
		RTW_PRINT_SEL(m, "qos_en=%d, ht_en=%d, init_rate=%d\n", psta->qos_option, psta->htpriv.ht_option, psta->init_rate);
		RTW_PRINT_SEL(m, "bwmode=%d, ch_offset=%d, sgi_20m=%d,sgi_40m=%d\n"
			, psta->phl_sta->chandef.bw, psta->htpriv.ch_offset, psta->htpriv.sgi_20m, psta->htpriv.sgi_40m);
		RTW_PRINT_SEL(m, "ampdu_enable = %d\n", psta->ampdu_priv.ampdu_enable);
		RTW_PRINT_SEL(m, "agg_enable_bitmap=%x, candidate_tid_bitmap=%x\n", psta->ampdu_priv.agg_enable_bitmap, psta->ampdu_priv.candidate_tid_bitmap);
		RTW_PRINT_SEL(m, "ldpc_cap=0x%x, stbc_cap=0x%x, beamform_cap=0x%x\n", psta->htpriv.ldpc_cap, psta->htpriv.stbc_cap, psta->htpriv.beamform_cap);
#endif /* CONFIG_80211N_HT */
		if (MLME_IS_AP(padapter))
			RTW_PRINT_SEL(m, " VHT or HE IE is configured by upper layer : %s\n", pmlmepriv->upper_layer_setting ? "True" : "False");
#ifdef CONFIG_80211AC_VHT
		RTW_PRINT_SEL(m, "vht_en=%u, vht_sgi_80m=%u\n", psta->vhtpriv.vht_option, psta->vhtpriv.sgi_80m);
		RTW_PRINT_SEL(m, "vht_ldpc_cap=0x%x, vht_stbc_cap=0x%x, vht_beamform_cap=0x%x\n", psta->vhtpriv.ldpc_cap, psta->vhtpriv.stbc_cap, psta->vhtpriv.beamform_cap);
		RTW_PRINT_SEL(m, "vht_mcs_map=0x%x, vht_highest_rate=0x%x, vht_ampdu_len=%d\n", *(u16 *)psta->vhtpriv.vht_mcs_map, psta->vhtpriv.vht_highest_rate, psta->vhtpriv.ampdu_len);
		if (psta->vhtpriv.vht_option) {
			RTW_MAP_DUMP_SEL_ALWAYS(m, "vht_cap=", psta->vhtpriv.vht_cap, 32);
		} else {
			RTW_PRINT_SEL(m, "vht_cap=N/A\n");
		}
#endif
#ifdef CONFIG_80211AX_HE
		RTW_PRINT_SEL(m, "he_en=%d\n", psta->hepriv.he_option);
		if (psta->hepriv.he_option) {
			RTW_MAP_DUMP_SEL_ALWAYS(m, "he_cap=", psta->hepriv.he_cap, HE_CAP_ELE_MAX_LEN);
		} else {
			RTW_PRINT_SEL(m, "he_cap=N/A\n");
		}
#endif
#ifdef CONFIG_RECV_REORDERING_CTRL
		sta_rx_reorder_ctl_dump(m, psta);
#endif
	} else
		RTW_PRINT_SEL(m, "can't get sta's macaddr, cur_network's macaddr:" MAC_FMT "\n", MAC_ARG(cur_network->network.MacAddress));

	return 0;
}

ssize_t proc_reset_trx_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct recv_info  *precvinfo = &padapter->recvinfo;
	char cmd[32] = {0};
	u8 cnt = 0;

	if (count > sizeof(cmd)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(cmd, buffer, count)) {
		int num = sscanf(cmd, "%hhx", &cnt);

		if (num == 1 && cnt == 0) {
			precvinfo->dbg_rx_ampdu_drop_count = 0;
			precvinfo->dbg_rx_ampdu_forced_indicate_count = 0;
			precvinfo->dbg_rx_ampdu_loss_count = 0;
			precvinfo->dbg_rx_dup_mgt_frame_drop_count = 0;
			precvinfo->dbg_rx_ampdu_window_shift_cnt = 0;
			precvinfo->dbg_rx_conflic_mac_addr_cnt = 0;
			precvinfo->dbg_rx_drop_count = 0;
		}
	}

	return count;
}

int proc_get_trx_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	int i;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct recv_info *precvinfo = &padapter->recvinfo;
	struct recv_priv  *precvpriv = &adapter_to_dvobj(padapter)->recvpriv;
	struct hw_xmit *phwxmit;
	struct trx_data_buf_q  *litexmitbuf_q = &dvobj->litexmitbuf_q;
	struct trx_data_buf_q  *literecvbuf_q = &dvobj->literecvbuf_q;
#ifdef CONFIG_USB_HCI
	struct trx_urb_buf_q *xmit_urb_q = &dvobj->xmit_urb_q;
	struct trx_urb_buf_q *recv_urb_q = &dvobj->recv_urb_q;
#endif
	u16 vo_params[4]={0}, vi_params[4]={0}, be_params[4]={0}, bk_params[4]={0};

	rtw_hal_read_edca(padapter, vo_params, vi_params, be_params, bk_params);

	RTW_PRINT_SEL(m, "wmm_edca_vo, aifs = %u us, cw_min = %u, cw_max = %u, txop_limit = %u us\n", vo_params[0], vo_params[1], vo_params[2], vo_params[3]);
	RTW_PRINT_SEL(m, "wmm_edca_vi, aifs = %u us, cw_min = %u, cw_max = %u, txop_limit = %u us\n", vi_params[0], vi_params[1], vi_params[2], vi_params[3]);
	RTW_PRINT_SEL(m, "wmm_edca_be, aifs = %u us, cw_min = %u, cw_max = %u, txop_limit = %u us\n", be_params[0], be_params[1], be_params[2], be_params[3]);
	RTW_PRINT_SEL(m, "wmm_edca_bk, aifs = %u us, cw_min = %u, cw_max = %u, txop_limit = %u us\n", bk_params[0], bk_params[1], bk_params[2], bk_params[3]);

	dump_os_queue(m, padapter);

	#if 0 /*CONFIG_CORE_XMITBUF*/
	RTW_PRINT_SEL(m, "free_xmitbuf_cnt=%d\n"
		, pxmitpriv->free_xmitbuf_cnt);
	RTW_PRINT_SEL(m, "free_ext_xmitbuf_cnt=%d\n"
		, pxmitpriv->free_xmit_extbuf_cnt);
	#endif

	RTW_PRINT_SEL(m, "free_xmitframe_cnt=%d\n"
		, pxmitpriv->free_xmitframe_cnt);
	RTW_PRINT_SEL(m, "free_xframe_ext_cnt=%d\n"
		, pxmitpriv->free_xframe_ext_cnt);
	RTW_PRINT_SEL(m, "free_recvframe_cnt=%d\n"
		      , precvpriv->free_recvframe_cnt);

	RTW_PRINT_SEL(m, "free_litexmitbuf_cnt=%d\n"
		, litexmitbuf_q->free_data_buf_cnt);
	RTW_PRINT_SEL(m, "free_recvbuf_cnt=%d\n"
		, literecvbuf_q->free_data_buf_cnt);

	for (i = 0; i < 4; i++) {
		phwxmit = pxmitpriv->hwxmits + i;
		RTW_PRINT_SEL(m, "%d, hwq.accnt=%d\n", i, phwxmit->accnt);
	}

	rtw_hal_get_hwreg(padapter, HW_VAR_DUMP_MAC_TXFIFO, (u8 *)m);

#ifdef CONFIG_USB_HCI
	RTW_PRINT_SEL(m, "tx_urb_cnt=%d\n", xmit_urb_q->free_urb_buf_cnt);
	RTW_PRINT_SEL(m, "rx_urb_cnt=%d\n", recv_urb_q->free_urb_buf_cnt);
	RTW_PRINT_SEL(m, "rx_urb_pending_cn=%d\n", ATOMIC_READ(&(dvobj->rx_pending_cnt)));
#endif

	/* Folowing are RX info */
	RTW_PRINT_SEL(m, "RX: Count of Packets dropped by Driver: %llu\n", (unsigned long long)precvinfo->dbg_rx_drop_count);
	/* Counts of packets whose seq_num is less than preorder_ctrl->indicate_seq, Ex delay, retransmission, redundant packets and so on */
	RTW_PRINT_SEL(m, "Rx: Counts of Packets Whose Seq_Num Less Than Reorder Control Seq_Num: %llu\n", (unsigned long long)precvinfo->dbg_rx_ampdu_drop_count);
	/* How many times the Rx Reorder Timer is triggered. */
	RTW_PRINT_SEL(m, "Rx: Reorder Time-out Trigger Counts: %llu\n", (unsigned long long)precvinfo->dbg_rx_ampdu_forced_indicate_count);
	/* Total counts of packets loss */
	RTW_PRINT_SEL(m, "Rx: Packet Loss Counts: %llu\n", (unsigned long long)precvinfo->dbg_rx_ampdu_loss_count);
	RTW_PRINT_SEL(m, "Rx: Duplicate Management Frame Drop Count: %llu\n", (unsigned long long)precvinfo->dbg_rx_dup_mgt_frame_drop_count);
	RTW_PRINT_SEL(m, "Rx: AMPDU BA window shift Count: %llu\n", (unsigned long long)precvinfo->dbg_rx_ampdu_window_shift_cnt);
	/*The same mac addr counts*/
	RTW_PRINT_SEL(m, "Rx: Conflict MAC Address Frames Count: %llu\n", (unsigned long long)precvinfo->dbg_rx_conflic_mac_addr_cnt);
	return 0;
}

static const char *rtw_data_rate_str(enum rtw_data_rate rate)
{
	if (rate >= RTW_DATA_RATE_CCK1 && rate <= RTW_DATA_RATE_CCK11) {
		switch (rate) {
#define CASE_CCK_RATE(cck)	case RTW_DATA_RATE_CCK ## cck: return "CCK_" # cck "M"
		CASE_CCK_RATE(1);
		CASE_CCK_RATE(2);
		CASE_CCK_RATE(5_5);
		CASE_CCK_RATE(11);
#undef CASE_CCK_RATE
		default:
			return "CCK_UNKNOWN";
		}
	} else if (rate >= RTW_DATA_RATE_OFDM6 && rate <= RTW_DATA_RATE_OFDM54) {
		switch (rate) {
#define CASE_OFDM_RATE(ofdm)	case RTW_DATA_RATE_OFDM ## ofdm: return "OFDM_" # ofdm "M"
		CASE_OFDM_RATE(6);
		CASE_OFDM_RATE(9);
		CASE_OFDM_RATE(12);
		CASE_OFDM_RATE(18);
		CASE_OFDM_RATE(24);
		CASE_OFDM_RATE(36);
		CASE_OFDM_RATE(48);
		CASE_OFDM_RATE(54);
#undef CASE_OFDM_RATE
		default:
			return "OFDM_UNKNOWN";
		}
	} else if (rate >= RTW_DATA_RATE_MCS0 && rate <= RTW_DATA_RATE_MCS31) {
		switch (rate) {
#define CASE_HT_RATE(mcs)	case RTW_DATA_RATE_MCS ## mcs: return "MCS_" # mcs
		CASE_HT_RATE(0);
		CASE_HT_RATE(1);
		CASE_HT_RATE(2);
		CASE_HT_RATE(3);
		CASE_HT_RATE(4);
		CASE_HT_RATE(5);
		CASE_HT_RATE(6);
		CASE_HT_RATE(7);
		CASE_HT_RATE(8);
		CASE_HT_RATE(9);
		CASE_HT_RATE(10);
		CASE_HT_RATE(11);
		CASE_HT_RATE(12);
		CASE_HT_RATE(13);
		CASE_HT_RATE(14);
		CASE_HT_RATE(15);
		CASE_HT_RATE(16);
		CASE_HT_RATE(17);
		CASE_HT_RATE(18);
		CASE_HT_RATE(19);
		CASE_HT_RATE(20);
		CASE_HT_RATE(21);
		CASE_HT_RATE(22);
		CASE_HT_RATE(23);
		CASE_HT_RATE(24);
		CASE_HT_RATE(25);
		CASE_HT_RATE(26);
		CASE_HT_RATE(27);
		CASE_HT_RATE(28);
		CASE_HT_RATE(29);
		CASE_HT_RATE(30);
		CASE_HT_RATE(31);
#undef CASE_HT_RATE
		default:
			return "HT_UNKNOWN";
		}
	} else if (rate >= RTW_DATA_RATE_VHT_NSS1_MCS0 && rate <= RTW_DATA_RATE_VHT_NSS4_MCS9) {
		switch (rate) {
#define CASE_VHT_RATE(ss, mcs)	case RTW_DATA_RATE_VHT_NSS ## ss ##_MCS ##mcs: return "VHT_SS" #ss "MCS" #mcs
		CASE_VHT_RATE(1, 0);
		CASE_VHT_RATE(1, 1);
		CASE_VHT_RATE(1, 2);
		CASE_VHT_RATE(1, 3);
		CASE_VHT_RATE(1, 4);
		CASE_VHT_RATE(1, 5);
		CASE_VHT_RATE(1, 6);
		CASE_VHT_RATE(1, 7);
		CASE_VHT_RATE(1, 8);
		CASE_VHT_RATE(1, 9);
		CASE_VHT_RATE(2, 0);
		CASE_VHT_RATE(2, 1);
		CASE_VHT_RATE(2, 2);
		CASE_VHT_RATE(2, 3);
		CASE_VHT_RATE(2, 4);
		CASE_VHT_RATE(2, 5);
		CASE_VHT_RATE(2, 6);
		CASE_VHT_RATE(2, 7);
		CASE_VHT_RATE(2, 8);
		CASE_VHT_RATE(2, 9);
		CASE_VHT_RATE(3, 0);
		CASE_VHT_RATE(3, 1);
		CASE_VHT_RATE(3, 2);
		CASE_VHT_RATE(3, 3);
		CASE_VHT_RATE(3, 4);
		CASE_VHT_RATE(3, 5);
		CASE_VHT_RATE(3, 6);
		CASE_VHT_RATE(3, 7);
		CASE_VHT_RATE(3, 8);
		CASE_VHT_RATE(3, 9);
		CASE_VHT_RATE(4, 0);
		CASE_VHT_RATE(4, 1);
		CASE_VHT_RATE(4, 2);
		CASE_VHT_RATE(4, 3);
		CASE_VHT_RATE(4, 4);
		CASE_VHT_RATE(4, 5);
		CASE_VHT_RATE(4, 6);
		CASE_VHT_RATE(4, 7);
		CASE_VHT_RATE(4, 8);
		CASE_VHT_RATE(4, 9);
#undef CASE_VHT_RATE
		default:
			return "VHT_UNKNOWN";
		}
	} else if (rate >= RTW_DATA_RATE_HE_NSS1_MCS0 && rate <= RTW_DATA_RATE_HE_NSS4_MCS11) {
		switch (rate) {
#define CASE_HE_RATE(ss, mcs)	case RTW_DATA_RATE_HE_NSS ## ss ##_MCS ##mcs: return "HE_SS" #ss "MCS" #mcs
		CASE_HE_RATE(1, 0);
		CASE_HE_RATE(1, 1);
		CASE_HE_RATE(1, 2);
		CASE_HE_RATE(1, 3);
		CASE_HE_RATE(1, 4);
		CASE_HE_RATE(1, 5);
		CASE_HE_RATE(1, 6);
		CASE_HE_RATE(1, 7);
		CASE_HE_RATE(1, 8);
		CASE_HE_RATE(1, 9);
		CASE_HE_RATE(1, 10);
		CASE_HE_RATE(1, 11);
		CASE_HE_RATE(2, 0);
		CASE_HE_RATE(2, 1);
		CASE_HE_RATE(2, 2);
		CASE_HE_RATE(2, 3);
		CASE_HE_RATE(2, 4);
		CASE_HE_RATE(2, 5);
		CASE_HE_RATE(2, 6);
		CASE_HE_RATE(2, 7);
		CASE_HE_RATE(2, 8);
		CASE_HE_RATE(2, 9);
		CASE_HE_RATE(2, 10);
		CASE_HE_RATE(2, 11);
		CASE_HE_RATE(3, 0);
		CASE_HE_RATE(3, 1);
		CASE_HE_RATE(3, 2);
		CASE_HE_RATE(3, 3);
		CASE_HE_RATE(3, 4);
		CASE_HE_RATE(3, 5);
		CASE_HE_RATE(3, 6);
		CASE_HE_RATE(3, 7);
		CASE_HE_RATE(3, 8);
		CASE_HE_RATE(3, 9);
		CASE_HE_RATE(3, 10);
		CASE_HE_RATE(3, 11);
		CASE_HE_RATE(4, 0);
		CASE_HE_RATE(4, 1);
		CASE_HE_RATE(4, 2);
		CASE_HE_RATE(4, 3);
		CASE_HE_RATE(4, 4);
		CASE_HE_RATE(4, 5);
		CASE_HE_RATE(4, 6);
		CASE_HE_RATE(4, 7);
		CASE_HE_RATE(4, 8);
		CASE_HE_RATE(4, 9);
		CASE_HE_RATE(4, 10);
		CASE_HE_RATE(4, 11);
#undef CASE_HE_RATE
		default:
			return "HE_UNKNOWN";
		}
	}

	return "ALL_UNKNOWN";
}

int proc_get_rate_ctl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	u16 data_rate = 0;
	u8 sgi = 0, data_fb = 0;

	if (adapter->fix_rate != NO_FIX_RATE) {
		data_rate = GET_FIX_RATE(adapter->fix_rate);
		sgi = GET_FIX_RATE_SGI(adapter->fix_rate);
		data_fb = adapter->data_fb ? 1 : 0;
		RTW_PRINT_SEL(m, "FIXED %s%s%s\n"
			, rtw_data_rate_str(data_rate)
			, data_rate >= RTW_DATA_RATE_MCS0 ? (sgi ? " SGI" : " LGI") : ""
			, data_fb ? " FB" : ""
		);
		RTW_PRINT_SEL(m, "0x%02x %u\n", adapter->fix_rate, adapter->data_fb);
	} else
		RTW_PRINT_SEL(m, "RA\n");

	return 0;
}

ssize_t proc_set_rate_ctl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u16 fix_rate = NO_FIX_RATE;
	u8 data_fb = 0;


	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hx %hhu", &fix_rate, &data_fb);

		if (num >= 1) {
			u16 fix_rate_ori = adapter->fix_rate;

			adapter->fix_rate = fix_rate;
			#if 0 /*GEORGIA_TODO_FIXIT*/
			if (fix_rate == 0xFF)
				hal_data->ForcedDataRate = 0;
			else
				hal_data->ForcedDataRate = hwrate_to_mrate(fix_rate & 0x7F);
			#endif
			if (adapter->fix_bw != NO_FIX_BW && fix_rate_ori != fix_rate)
				rtw_run_in_thread_cmd(adapter, ((void *)(rtw_update_tx_rate_bmp)), adapter_to_dvobj(adapter));
		}
		if (num >= 2)
			adapter->data_fb = data_fb ? 1 : 0;
	}

	return count;
}

#ifdef CONFIG_AP_MODE
int proc_get_bmc_tx_rate(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (!MLME_IS_AP(adapter) && !MLME_IS_MESH(adapter)) {
		RTW_PRINT_SEL(m, "[ERROR] Not in SoftAP/Mesh mode !!\n");
		return 0;
	}

	RTW_PRINT_SEL(m, " BMC Tx rate - %s\n", MGN_RATE_STR(adapter->bmc_tx_rate));
	return 0;
}

ssize_t proc_set_bmc_tx_rate(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 bmc_tx_rate;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &bmc_tx_rate);

		if (num >= 1)
			/*adapter->bmc_tx_rate = hwrate_to_mrate(bmc_tx_rate);*/
			adapter->bmc_tx_rate = bmc_tx_rate;
	}

	return count;
}
#endif /*CONFIG_AP_MODE*/


int proc_get_tx_power_offset(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "Tx power offset - %u\n", adapter->power_offset);
	return 0;
}

ssize_t proc_set_tx_power_offset(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 power_offset = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu", &power_offset);

		if (num >= 1) {
			if (power_offset > 5)
				power_offset = 0;

			adapter->power_offset = power_offset;
		}
	}

	return count;
}

int proc_get_bw_ctl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	u8 data_bw = 0;

	if (adapter->fix_bw != NO_FIX_BW) {
		data_bw = adapter->fix_bw;
		RTW_PRINT_SEL(m, "FIXED %s\n", ch_width_str(data_bw));
	} else
		RTW_PRINT_SEL(m, "Auto\n");

	return 0;
}

ssize_t proc_set_bw_ctl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 fix_bw;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhu", &fix_bw);

		if (num >= 1) {
			u8 fix_bw_ori = adapter->fix_bw;

			adapter->fix_bw = fix_bw;

			if (adapter->fix_rate != NO_FIX_RATE && fix_bw_ori != fix_bw)
				rtw_run_in_thread_cmd(adapter, ((void *)(rtw_update_tx_rate_bmp)), adapter_to_dvobj(adapter));
		}
	}

	return count;
}

#ifdef DBG_RX_COUNTER_DUMP
int proc_get_rx_cnt_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	int i;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "BIT0- Dump RX counters of DRV\n");
	RTW_PRINT_SEL(m, "BIT1- Dump RX counters of MAC\n");
	RTW_PRINT_SEL(m, "BIT2- Dump RX counters of PHY\n");
	RTW_PRINT_SEL(m, "BIT3- Dump TRX data frame of DRV\n");
	RTW_PRINT_SEL(m, "dump_rx_cnt_mode = 0x%02x\n", adapter->dump_rx_cnt_mode);

	return 0;
}
ssize_t proc_set_rx_cnt_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 dump_rx_cnt_mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &dump_rx_cnt_mode);

		if (num == 1) {
			rtw_dump_phy_rxcnts_preprocess(adapter, dump_rx_cnt_mode);
			adapter->dump_rx_cnt_mode = dump_rx_cnt_mode;
		}
	}

	return count;
}
#endif


ssize_t proc_set_del_rx_ampdu_test_case(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count))
		sscanf(tmp, "%hhu", &del_rx_ampdu_test_no_tx_fail);

	return count;
}

ssize_t proc_set_wait_hiq_empty(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count))
		sscanf(tmp, "%u", &g_wait_hiq_empty_ms);

	return count;
}

ssize_t proc_set_sta_linking_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u32 wait_ms = 0;
		u8 force_fail = 0;
		int num = sscanf(tmp, "%u %hhu", &wait_ms, &force_fail);

		if (num >= 1)
			sta_linking_test_wait_ms = wait_ms;
		if (num >= 2)
			sta_linking_test_force_fail = force_fail;
	}

	return count;
}

#ifdef CONFIG_AP_MODE
ssize_t proc_set_ap_linking_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u16 force_auth_fail = 0;
		u16 force_asoc_fail = 0;
		int num = sscanf(tmp, "%hu %hu", &force_auth_fail, &force_asoc_fail);

		if (num >= 1)
			ap_linking_test_force_auth_fail = force_auth_fail;
		if (num >= 2)
			ap_linking_test_force_asoc_fail = force_asoc_fail;
	}

	return count;
}
#endif /* CONFIG_AP_MODE */

int proc_get_ps_dbg_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;

	RTW_PRINT_SEL(m, "dbg_sdio_alloc_irq_cnt=%d\n", pdbgpriv->dbg_sdio_alloc_irq_cnt);
	RTW_PRINT_SEL(m, "dbg_sdio_free_irq_cnt=%d\n", pdbgpriv->dbg_sdio_free_irq_cnt);
	RTW_PRINT_SEL(m, "dbg_sdio_alloc_irq_error_cnt=%d\n", pdbgpriv->dbg_sdio_alloc_irq_error_cnt);
	RTW_PRINT_SEL(m, "dbg_sdio_free_irq_error_cnt=%d\n", pdbgpriv->dbg_sdio_free_irq_error_cnt);
	RTW_PRINT_SEL(m, "dbg_sdio_init_error_cnt=%d\n", pdbgpriv->dbg_sdio_init_error_cnt);
	RTW_PRINT_SEL(m, "dbg_sdio_deinit_error_cnt=%d\n", pdbgpriv->dbg_sdio_deinit_error_cnt);
	RTW_PRINT_SEL(m, "dbg_suspend_error_cnt=%d\n", pdbgpriv->dbg_suspend_error_cnt);
	RTW_PRINT_SEL(m, "dbg_suspend_cnt=%d\n", pdbgpriv->dbg_suspend_cnt);
	RTW_PRINT_SEL(m, "dbg_resume_cnt=%d\n", pdbgpriv->dbg_resume_cnt);
	RTW_PRINT_SEL(m, "dbg_resume_error_cnt=%d\n", pdbgpriv->dbg_resume_error_cnt);
	RTW_PRINT_SEL(m, "dbg_deinit_fail_cnt=%d\n", pdbgpriv->dbg_deinit_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_carddisable_cnt=%d\n", pdbgpriv->dbg_carddisable_cnt);
	RTW_PRINT_SEL(m, "dbg_ps_insuspend_cnt=%d\n", pdbgpriv->dbg_ps_insuspend_cnt);
	RTW_PRINT_SEL(m, "dbg_dev_unload_inIPS_cnt=%d\n", pdbgpriv->dbg_dev_unload_inIPS_cnt);
	RTW_PRINT_SEL(m, "dbg_scan_pwr_state_cnt=%d\n", pdbgpriv->dbg_scan_pwr_state_cnt);
	RTW_PRINT_SEL(m, "dbg_downloadfw_pwr_state_cnt=%d\n", pdbgpriv->dbg_downloadfw_pwr_state_cnt);
	RTW_PRINT_SEL(m, "dbg_carddisable_error_cnt=%d\n", pdbgpriv->dbg_carddisable_error_cnt);
	RTW_PRINT_SEL(m, "dbg_fw_read_ps_state_fail_cnt=%d\n", pdbgpriv->dbg_fw_read_ps_state_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_leave_ips_fail_cnt=%d\n", pdbgpriv->dbg_leave_ips_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_leave_lps_fail_cnt=%d\n", pdbgpriv->dbg_leave_lps_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_h2c_leave32k_fail_cnt=%d\n", pdbgpriv->dbg_h2c_leave32k_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_diswow_dload_fw_fail_cnt=%d\n", pdbgpriv->dbg_diswow_dload_fw_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_enwow_dload_fw_fail_cnt=%d\n", pdbgpriv->dbg_enwow_dload_fw_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_ips_drvopen_fail_cnt=%d\n", pdbgpriv->dbg_ips_drvopen_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_poll_fail_cnt=%d\n", pdbgpriv->dbg_poll_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_rpwm_toogle_cnt=%d\n", pdbgpriv->dbg_rpwm_toogle_cnt);
	RTW_PRINT_SEL(m, "dbg_rpwm_timeout_fail_cnt=%d\n", pdbgpriv->dbg_rpwm_timeout_fail_cnt);
	RTW_PRINT_SEL(m, "dbg_sreset_cnt=%d\n", pdbgpriv->dbg_sreset_cnt);
	RTW_PRINT_SEL(m, "dbg_fw_mem_dl_error_cnt=%d\n", pdbgpriv->dbg_fw_mem_dl_error_cnt);

	return 0;
}
ssize_t proc_set_ps_dbg_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter->dvobj;
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;
	char tmp[32];
	u8 ps_dbg_cmd_id;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &ps_dbg_cmd_id);

		if (num == 1 && ps_dbg_cmd_id == 1) /*Clean all*/
			_rtw_memset(pdbgpriv, 0, sizeof(struct debug_priv));

	}

	return count;
}


#ifdef CONFIG_DBG_COUNTER

int proc_get_rx_logs(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rx_logs *rx_logs = &padapter->rx_logs;

	RTW_PRINT_SEL(m,
		      "intf_rx=%d\n"
		      "intf_rx_err_recvframe=%d\n"
		      "intf_rx_err_skb=%d\n"
		      "intf_rx_report=%d\n"
		      "core_rx=%d\n"
		      "core_rx_pre=%d\n"
		      "core_rx_pre_ver_err=%d\n"
		      "core_rx_pre_mgmt=%d\n"
		      "core_rx_pre_mgmt_err_80211w=%d\n"
		      "core_rx_pre_mgmt_err=%d\n"
		      "core_rx_pre_ctrl=%d\n"
		      "core_rx_pre_ctrl_err=%d\n"
		      "core_rx_pre_data=%d\n"
		      "core_rx_pre_data_wapi_seq_err=%d\n"
		      "core_rx_pre_data_wapi_key_err=%d\n"
		      "core_rx_pre_data_handled=%d\n"
		      "core_rx_pre_data_err=%d\n"
		      "core_rx_pre_data_unknown=%d\n"
		      "core_rx_pre_unknown=%d\n"
		      "core_rx_enqueue=%d\n"
		      "core_rx_dequeue=%d\n"
		      "core_rx_post=%d\n"
		      "core_rx_post_decrypt=%d\n"
		      "core_rx_post_decrypt_wep=%d\n"
		      "core_rx_post_decrypt_tkip=%d\n"
		      "core_rx_post_decrypt_aes=%d\n"
		      "core_rx_post_decrypt_wapi=%d\n"
		      "core_rx_post_decrypt_hw=%d\n"
		      "core_rx_post_decrypt_unknown=%d\n"
		      "core_rx_post_decrypt_err=%d\n"
		      "core_rx_post_defrag_err=%d\n"
		      "core_rx_post_portctrl_err=%d\n"
		      "core_rx_post_indicate=%d\n"
		      "core_rx_post_indicate_in_oder=%d\n"
		      "core_rx_post_indicate_reoder=%d\n"
		      "core_rx_post_indicate_err=%d\n"
		      "os_indicate=%d\n"
		      "os_indicate_ap_mcast=%d\n"
		      "os_indicate_ap_forward=%d\n"
		      "os_indicate_ap_self=%d\n"
		      "os_indicate_err=%d\n"
		      "os_netif_ok=%d\n"
		      "os_netif_err=%d\n",
		      rx_logs->intf_rx,
		      rx_logs->intf_rx_err_recvframe,
		      rx_logs->intf_rx_err_skb,
		      rx_logs->intf_rx_report,
		      rx_logs->core_rx,
		      rx_logs->core_rx_pre,
		      rx_logs->core_rx_pre_ver_err,
		      rx_logs->core_rx_pre_mgmt,
		      rx_logs->core_rx_pre_mgmt_err_80211w,
		      rx_logs->core_rx_pre_mgmt_err,
		      rx_logs->core_rx_pre_ctrl,
		      rx_logs->core_rx_pre_ctrl_err,
		      rx_logs->core_rx_pre_data,
		      rx_logs->core_rx_pre_data_wapi_seq_err,
		      rx_logs->core_rx_pre_data_wapi_key_err,
		      rx_logs->core_rx_pre_data_handled,
		      rx_logs->core_rx_pre_data_err,
		      rx_logs->core_rx_pre_data_unknown,
		      rx_logs->core_rx_pre_unknown,
		      rx_logs->core_rx_enqueue,
		      rx_logs->core_rx_dequeue,
		      rx_logs->core_rx_post,
		      rx_logs->core_rx_post_decrypt,
		      rx_logs->core_rx_post_decrypt_wep,
		      rx_logs->core_rx_post_decrypt_tkip,
		      rx_logs->core_rx_post_decrypt_aes,
		      rx_logs->core_rx_post_decrypt_wapi,
		      rx_logs->core_rx_post_decrypt_hw,
		      rx_logs->core_rx_post_decrypt_unknown,
		      rx_logs->core_rx_post_decrypt_err,
		      rx_logs->core_rx_post_defrag_err,
		      rx_logs->core_rx_post_portctrl_err,
		      rx_logs->core_rx_post_indicate,
		      rx_logs->core_rx_post_indicate_in_oder,
		      rx_logs->core_rx_post_indicate_reoder,
		      rx_logs->core_rx_post_indicate_err,
		      rx_logs->os_indicate,
		      rx_logs->os_indicate_ap_mcast,
		      rx_logs->os_indicate_ap_forward,
		      rx_logs->os_indicate_ap_self,
		      rx_logs->os_indicate_err,
		      rx_logs->os_netif_ok,
		      rx_logs->os_netif_err
		     );

	return 0;
}

int proc_get_tx_logs(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tx_logs *tx_logs = &padapter->tx_logs;

	RTW_PRINT_SEL(m,
		      "os_tx=%d\n"
		      "os_tx_err_up=%d\n"
		      "os_tx_err_xmit=%d\n"
		      "os_tx_m2u=%d\n"
		      "os_tx_m2u_ignore_fw_linked=%d\n"
		      "os_tx_m2u_ignore_self=%d\n"
		      "os_tx_m2u_entry=%d\n"
		      "os_tx_m2u_entry_err_xmit=%d\n"
		      "os_tx_m2u_entry_err_skb=%d\n"
		      "os_tx_m2u_stop=%d\n"
		      "core_tx=%d\n"
		      "core_tx_err_pxmitframe=%d\n"
		      "core_tx_err_brtx=%d\n"
		      "core_tx_upd_attrib=%d\n"
		      "core_tx_upd_attrib_adhoc=%d\n"
		      "core_tx_upd_attrib_sta=%d\n"
		      "core_tx_upd_attrib_ap=%d\n"
		      "core_tx_upd_attrib_unknown=%d\n"
		      "core_tx_upd_attrib_dhcp=%d\n"
		      "core_tx_upd_attrib_icmp=%d\n"
		      "core_tx_upd_attrib_active=%d\n"
		      "core_tx_upd_attrib_err_ucast_sta=%d\n"
		      "core_tx_upd_attrib_err_ucast_ap_link=%d\n"
		      "core_tx_upd_attrib_err_sta=%d\n"
		      "core_tx_upd_attrib_err_link=%d\n"
		      "core_tx_upd_attrib_err_sec=%d\n"
		      "core_tx_ap_enqueue_warn_fwstate=%d\n"
		      "core_tx_ap_enqueue_warn_sta=%d\n"
		      "core_tx_ap_enqueue_warn_nosta=%d\n"
		      "core_tx_ap_enqueue_warn_link=%d\n"
		      "core_tx_ap_enqueue_warn_trigger=%d\n"
		      "core_tx_ap_enqueue_mcast=%d\n"
		      "core_tx_ap_enqueue_ucast=%d\n"
		      "core_tx_ap_enqueue=%d\n"
		      "intf_tx=%d\n"
		      "intf_tx_pending_ac=%d\n"
		      "intf_tx_pending_fw_under_survey=%d\n"
		      "intf_tx_pending_fw_under_linking=%d\n"
		      "intf_tx_pending_xmitbuf=%d\n"
		      "intf_tx_enqueue=%d\n"
		      "core_tx_enqueue=%d\n"
		      "core_tx_enqueue_class=%d\n"
		      "core_tx_enqueue_class_err_sta=%d\n"
		      "core_tx_enqueue_class_err_nosta=%d\n"
		      "core_tx_enqueue_class_err_fwlink=%d\n"
		      "intf_tx_direct=%d\n"
		      "intf_tx_direct_err_coalesce=%d\n"
		      "intf_tx_dequeue=%d\n"
		      "intf_tx_dequeue_err_coalesce=%d\n"
		      "intf_tx_dump_xframe=%d\n"
		      "intf_tx_dump_xframe_err_txdesc=%d\n"
		      "intf_tx_dump_xframe_err_port=%d\n",
		      tx_logs->os_tx,
		      tx_logs->os_tx_err_up,
		      tx_logs->os_tx_err_xmit,
		      tx_logs->os_tx_m2u,
		      tx_logs->os_tx_m2u_ignore_fw_linked,
		      tx_logs->os_tx_m2u_ignore_self,
		      tx_logs->os_tx_m2u_entry,
		      tx_logs->os_tx_m2u_entry_err_xmit,
		      tx_logs->os_tx_m2u_entry_err_skb,
		      tx_logs->os_tx_m2u_stop,
		      tx_logs->core_tx,
		      tx_logs->core_tx_err_pxmitframe,
		      tx_logs->core_tx_err_brtx,
		      tx_logs->core_tx_upd_attrib,
		      tx_logs->core_tx_upd_attrib_adhoc,
		      tx_logs->core_tx_upd_attrib_sta,
		      tx_logs->core_tx_upd_attrib_ap,
		      tx_logs->core_tx_upd_attrib_unknown,
		      tx_logs->core_tx_upd_attrib_dhcp,
		      tx_logs->core_tx_upd_attrib_icmp,
		      tx_logs->core_tx_upd_attrib_active,
		      tx_logs->core_tx_upd_attrib_err_ucast_sta,
		      tx_logs->core_tx_upd_attrib_err_ucast_ap_link,
		      tx_logs->core_tx_upd_attrib_err_sta,
		      tx_logs->core_tx_upd_attrib_err_link,
		      tx_logs->core_tx_upd_attrib_err_sec,
		      tx_logs->core_tx_ap_enqueue_warn_fwstate,
		      tx_logs->core_tx_ap_enqueue_warn_sta,
		      tx_logs->core_tx_ap_enqueue_warn_nosta,
		      tx_logs->core_tx_ap_enqueue_warn_link,
		      tx_logs->core_tx_ap_enqueue_warn_trigger,
		      tx_logs->core_tx_ap_enqueue_mcast,
		      tx_logs->core_tx_ap_enqueue_ucast,
		      tx_logs->core_tx_ap_enqueue,
		      tx_logs->intf_tx,
		      tx_logs->intf_tx_pending_ac,
		      tx_logs->intf_tx_pending_fw_under_survey,
		      tx_logs->intf_tx_pending_fw_under_linking,
		      tx_logs->intf_tx_pending_xmitbuf,
		      tx_logs->intf_tx_enqueue,
		      tx_logs->core_tx_enqueue,
		      tx_logs->core_tx_enqueue_class,
		      tx_logs->core_tx_enqueue_class_err_sta,
		      tx_logs->core_tx_enqueue_class_err_nosta,
		      tx_logs->core_tx_enqueue_class_err_fwlink,
		      tx_logs->intf_tx_direct,
		      tx_logs->intf_tx_direct_err_coalesce,
		      tx_logs->intf_tx_dequeue,
		      tx_logs->intf_tx_dequeue_err_coalesce,
		      tx_logs->intf_tx_dump_xframe,
		      tx_logs->intf_tx_dump_xframe_err_txdesc,
		      tx_logs->intf_tx_dump_xframe_err_port
		     );

	return 0;
}

int proc_get_int_logs(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m,
		      "all=%d\n"
		      "err=%d\n"
		      "tbdok=%d\n"
		      "tbder=%d\n"
		      "bcnderr=%d\n"
		      "bcndma=%d\n"
		      "bcndma_e=%d\n"
		      "rx=%d\n"
		      "rx_rdu=%d\n"
		      "rx_fovw=%d\n"
		      "txfovw=%d\n"
		      "mgntok=%d\n"
		      "highdok=%d\n"
		      "bkdok=%d\n"
		      "bedok=%d\n"
		      "vidok=%d\n"
		      "vodok=%d\n",
		      padapter->int_logs.all,
		      padapter->int_logs.err,
		      padapter->int_logs.tbdok,
		      padapter->int_logs.tbder,
		      padapter->int_logs.bcnderr,
		      padapter->int_logs.bcndma,
		      padapter->int_logs.bcndma_e,
		      padapter->int_logs.rx,
		      padapter->int_logs.rx_rdu,
		      padapter->int_logs.rx_fovw,
		      padapter->int_logs.txfovw,
		      padapter->int_logs.mgntok,
		      padapter->int_logs.highdok,
		      padapter->int_logs.bkdok,
		      padapter->int_logs.bedok,
		      padapter->int_logs.vidok,
		      padapter->int_logs.vodok
		     );

	return 0;
}

#endif /* CONFIG_DBG_COUNTER */

int proc_get_hw_status(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);

	if (regsty->check_hw_status == 0)
		RTW_PRINT_SEL(m, "RX FIFO full count: not check in watch dog\n");
	else if (pdbgpriv->dbg_rx_fifo_last_overflow == 1
	    && pdbgpriv->dbg_rx_fifo_curr_overflow == 1
	    && pdbgpriv->dbg_rx_fifo_diff_overflow == 1
	   )
		RTW_PRINT_SEL(m, "RX FIFO full count: no implementation\n");
	else {
		RTW_PRINT_SEL(m, "RX FIFO full count: last_time=%llu, current_time=%llu, differential=%llu\n"
			, pdbgpriv->dbg_rx_fifo_last_overflow, pdbgpriv->dbg_rx_fifo_curr_overflow, pdbgpriv->dbg_rx_fifo_diff_overflow);
	}

	return 0;
}

ssize_t proc_set_hw_status(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = padapter->dvobj;
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);
	char tmp[32];
	u32 enable;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &enable);

		if (num == 1 && regsty && enable <= 1) {
			regsty->check_hw_status = enable;
			RTW_INFO("check_hw_status=%d\n", regsty->check_hw_status);
		}
	}

	return count;
}

int proc_get_trx_info_debug(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	/*============  tx info ============	*/
	rtw_hal_get_def_var(padapter, padapter_link, HW_DEF_RA_INFO_DUMP, m);

	/*============  rx info ============	*/
	/*rtw_hal_set_phydm_var(padapter, HAL_PHYDM_RX_INFO_DUMP, m, _FALSE);*/

	return 0;
}

int proc_get_rx_signal(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
#ifdef CONFIG_SNR_RPT
	_list	*plist, *phead;
	struct sta_info *psta = NULL;
	u8 sta_mac[NUM_STA][ETH_ALEN] = {{0}};
	uint mac_id[NUM_STA];
	struct stainfo_stats	*pstats = NULL;
	struct sta_priv	*pstapriv = &(padapter->stapriv);
	u32 i, j, macid_rec_idx = 0;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if ((_rtw_memcmp(psta->phl_sta->mac_addr, bc_addr, 6) !=  _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, null_addr, 6) != _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, psta->padapter_link->mac_addr, 6) != _TRUE)) {
				_rtw_memcpy(&sta_mac[macid_rec_idx][0], psta->phl_sta->mac_addr, ETH_ALEN);
				mac_id[macid_rec_idx] = psta->phl_sta->macid;
				macid_rec_idx++;
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

	for (i = 0; i < macid_rec_idx; i++) {
		psta = rtw_get_stainfo(pstapriv, &sta_mac[i][0]);
		if(psta) {
			/*ToDo, base on the real number of rf path to show the information*/
			RTW_PRINT_SEL(m, "STA:"MAC_FMT" SNR:{%d, %d, %d, %d}\n", MAC_ARG(psta->phl_sta->mac_addr),
				psta->snr_fd_avg[0], psta->snr_fd_avg[1], psta->snr_fd_avg[2], psta->snr_fd_avg[3]);
		} else {
			RTW_INFO("STA is gone\n");
		}
	}
#endif /* CONFIG_SNR_RPT */

	RTW_PRINT_SEL(m, "rssi:%d\n", padapter->recvinfo.rssi);
#if 0//def CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1) {
		struct dm_struct *odm = adapter_to_phydm(padapter);
		if (padapter->mppriv.antenna_rx == ANTENNA_A)
			RTW_PRINT_SEL(m, "Antenna: A\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_B)
			RTW_PRINT_SEL(m, "Antenna: B\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_C)
			RTW_PRINT_SEL(m, "Antenna: C\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_D)
			RTW_PRINT_SEL(m, "Antenna: D\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_AB)
			RTW_PRINT_SEL(m, "Antenna: AB\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_BC)
			RTW_PRINT_SEL(m, "Antenna: BC\n");
		else if (padapter->mppriv.antenna_rx == ANTENNA_CD)
			RTW_PRINT_SEL(m, "Antenna: CD\n");
		else
			RTW_PRINT_SEL(m, "Antenna: __\n");

		RTW_PRINT_SEL(m, "rx_rate = %s\n", HDATA_RATE(odm->rx_rate));
		return 0;
	} else 
#endif
	{
		/* RTW_PRINT_SEL(m, "rxpwdb:%d\n", padapter->recvinfo.rxpwdb); */
		RTW_PRINT_SEL(m, "signal_strength:%u\n", padapter->recvinfo.signal_strength);
		RTW_PRINT_SEL(m, "signal_qual:%u\n", padapter->recvinfo.signal_qual);
	}

	return 0;
}

ssize_t proc_set_rx_signal(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 is_signal_dbg, signal_strength;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%u %u", &is_signal_dbg, &signal_strength);

		if (num < 1)
			return count;

		is_signal_dbg = is_signal_dbg == 0 ? 0 : 1;

		if (is_signal_dbg && num < 2)
			return count;

		signal_strength = signal_strength > PHL_MAX_RSSI ? PHL_MAX_RSSI : signal_strength;

		padapter->recvinfo.is_signal_dbg = is_signal_dbg;
		padapter->recvinfo.signal_strength_dbg = signal_strength;

		if (is_signal_dbg)
			RTW_INFO("set %s %u\n", "DBG_SIGNAL_STRENGTH", signal_strength);
		else
			RTW_INFO("set %s\n", "HW_SIGNAL_STRENGTH");

	}

	return count;

}

int proc_get_mac_rptbuf(struct seq_file *m, void *v)
{
	return 0;
}

#ifdef CONFIG_80211N_HT

int proc_get_ht_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "%d\n", pregpriv->ht_enable);

	return 0;
}

ssize_t proc_set_ht_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if ( num == 1 && pregpriv && mode < 2) {
			pregpriv->ht_enable = mode;
			RTW_INFO("ht_enable=%d\n", pregpriv->ht_enable);
		}
	}

	return count;

}

int proc_get_bw_mode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%02x\n", pregpriv->bw_mode);

	return 0;
}

ssize_t proc_set_bw_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;
	u8 bw_2g;
	u8 bw_5g;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x ", &mode);
		bw_5g = mode >> 4;
		bw_2g = mode & 0x0f;

		if (num == 1 && pregpriv && bw_2g <= 4 && bw_5g <= 4) {
			pregpriv->bw_mode = mode;
			printk("bw_mode=0x%x\n", mode);
		}
	}

	return count;

}

int proc_get_ampdu_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "%d\n", pregpriv->ampdu_enable);

	return 0;
}

ssize_t proc_set_ampdu_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if (num == 1 && pregpriv && mode < 2) {
			pregpriv->ampdu_enable = mode;
			printk("ampdu_enable=%d\n", mode);
		}

	}

	return count;

}


void dump_regsty_rx_ampdu_size_limit(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = adapter_to_regsty(adapter);
	int i;

	RTW_PRINT_SEL(sel, "%-3s %-3s %-3s %-3s %-4s\n"
		, "", "20M", "40M", "80M", "160M");
	for (i = 0; i < 4; i++)
		RTW_PRINT_SEL(sel, "%dSS %3u %3u %3u %4u\n", i + 1
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][0]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][1]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][2]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][3]);
}

int proc_get_rx_ampdu(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	_RTW_PRINT_SEL(m, "accept: ");
	if (padapter->fix_rx_ampdu_accept == RX_AMPDU_ACCEPT_INVALID)
		RTW_PRINT_SEL(m, "%u%s\n", rtw_rx_ampdu_is_accept(padapter), "(auto)");
	else
		RTW_PRINT_SEL(m, "%u%s\n", padapter->fix_rx_ampdu_accept, "(fixed)");

	_RTW_PRINT_SEL(m, "size: ");
	if (padapter->fix_rx_ampdu_size == RX_AMPDU_SIZE_INVALID) {
		RTW_PRINT_SEL(m, "%u%s\n", rtw_rx_ampdu_size(padapter), "(auto) with conditional limit:");
		dump_regsty_rx_ampdu_size_limit(m, padapter);
	} else
		RTW_PRINT_SEL(m, "%u%s\n", padapter->fix_rx_ampdu_size, "(fixed)");
	RTW_PRINT_SEL(m, "\n");

	RTW_PRINT_SEL(m, "%19s %17s\n", "fix_rx_ampdu_accept", "fix_rx_ampdu_size");

	_RTW_PRINT_SEL(m, "%-19d %-17u\n"
		, padapter->fix_rx_ampdu_accept
		, padapter->fix_rx_ampdu_size);

	return 0;
}

ssize_t proc_set_rx_ampdu(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 accept;
	u8 size;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hhu", &accept, &size);

		if (num >= 1)
			rtw_rx_ampdu_set_accept(padapter, accept, RX_AMPDU_DRV_FIXED);
		if (num >= 2)
			rtw_rx_ampdu_set_size(padapter, size, RX_AMPDU_DRV_FIXED);

		rtw_rx_ampdu_apply(padapter);
	}

	return count;
}

int proc_get_rx_ampdu_factor(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	if (padapter)
		RTW_PRINT_SEL(m, "rx ampdu factor = %x\n", padapter->driver_rx_ampdu_factor);

	return 0;
}

ssize_t proc_set_rx_ampdu_factor(struct file *file, const char __user *buffer
				 , size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 factor;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &factor);

		if (padapter && (num == 1)) {
			RTW_INFO("padapter->driver_rx_ampdu_factor = %x\n", factor);

			if (factor  > 0x03)
				padapter->driver_rx_ampdu_factor = 0xFF;
			else
				padapter->driver_rx_ampdu_factor = factor;
		}
	}

	return count;
}

int proc_get_tx_ampdu_num(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	int i;

	if (padapter) {
		for (i = HW_BAND_0 ; i < HW_BAND_MAX ; i++) {
			RTW_PRINT_SEL(m, "===== HW band index %d =====\n", i);
			RTW_PRINT_SEL(m, "[phy_cap] tx ampdu num = %s",
						  phl_com->phy_cap[i].txagg_num ? "":"not yet set");
			if (phl_com->phy_cap[i].txagg_num)
				RTW_PRINT_SEL(m, "%d\n", phl_com->phy_cap[i].txagg_num);
			else
				RTW_PRINT_SEL(m, "\n");

			RTW_PRINT_SEL(m, "[phy_sw_cap] tx ampdu num = %s",
						  phl_com->phy_sw_cap[i].txagg_num ? "":"default by HW");
			if (phl_com->phy_sw_cap[i].txagg_num)
				RTW_PRINT_SEL(m, "%d\n", phl_com->phy_sw_cap[i].txagg_num);
			else
				RTW_PRINT_SEL(m, "\n");

			if (i >= HW_BAND_0) {
#ifdef CONFIG_DBCC_SUPPORT
				if (phl_com->dev_cap.dbcc_sup == true)
					continue;
#endif
				break;
			}
		}
	}

	return 0;
}

ssize_t proc_set_tx_ampdu_num(struct file *file, const char __user *buffer
				 , size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	char tmp[32];
	u8 hw_band_idx;
	u32 tx_ampdu_num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %u", &hw_band_idx, &tx_ampdu_num);

		if (padapter && (num == 2)) {
			if (hw_band_idx < HW_BAND_MAX && hw_band_idx >= HW_BAND_0) {
				phl_com->phy_cap[hw_band_idx].txagg_num = tx_ampdu_num;
				RTW_INFO("[HW Band %d] set phy_cap tx ampdu num = %u\n",
					 hw_band_idx, tx_ampdu_num);
                        } else {
				RTW_INFO("The input of HW Band index (%u) is invalid !\n",
                                         hw_band_idx);
			}
		}
	}

	return count;
}

int proc_get_rx_ampdu_density(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	if (padapter)
		RTW_PRINT_SEL(m, "rx ampdu densityg = %x\n", padapter->driver_rx_ampdu_spacing);

	return 0;
}

ssize_t proc_set_rx_ampdu_density(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 density;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &density);

		if (padapter && (num == 1)) {
			RTW_INFO("padapter->driver_rx_ampdu_spacing = %x\n", density);

			if (density > 0x07)
				padapter->driver_rx_ampdu_spacing = 0xFF;
			else
				padapter->driver_rx_ampdu_spacing = density;
		}
	}

	return count;
}

int proc_get_tx_ampdu_density(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);


	if (padapter)
		RTW_PRINT_SEL(m, "tx ampdu density = %x\n", padapter->driver_ampdu_spacing);

	return 0;
}

ssize_t proc_set_tx_ampdu_density(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 density;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &density);

		if (padapter && (num == 1)) {
			RTW_INFO("padapter->driver_ampdu_spacing = %x\n", density);

			if (density > 0x07)
				padapter->driver_ampdu_spacing = 0xFF;
			else
				padapter->driver_ampdu_spacing = density;
		}
	}

	return count;
}

int proc_get_tx_quick_addba_req(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (padapter)
		RTW_PRINT_SEL(m, "tx_quick_addba_req = %x\n", pregpriv->tx_quick_addba_req);

	return 0;
}

ssize_t proc_set_tx_quick_addba_req(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 enable;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &enable);

		if (padapter && (num == 1)) {
			pregpriv->tx_quick_addba_req = enable;
			RTW_INFO("tx_quick_addba_req = %d\n", pregpriv->tx_quick_addba_req);
		}
	}

	return count;
}
#ifdef CONFIG_TX_AMSDU
int proc_get_tx_amsdu(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	int i;

	if (padapter)
	{
		RTW_PRINT_SEL(m, "tx amsdu = %d\n", padapter->tx_amsdu);
		RTW_PRINT_SEL(m, "amsdu set timer conut = %u\n", pxmitpriv->amsdu_debug_set_timer);
		RTW_PRINT_SEL(m, "amsdu time out count = %u\n", pxmitpriv->amsdu_debug_timeout);
		for (i = 0; i < (AMSDU_DEBUG_MAX_COUNT - 1); i++) {
			RTW_PRINT_SEL(m, "amsdu coalesce %d count = %u\n",
					i + 1, pxmitpriv->amsdu_debug_coalesce[i]);
		}
		RTW_PRINT_SEL(m, "amsdu coalesce >%d count = %u\n",
				i, pxmitpriv->amsdu_debug_coalesce[i]);
		RTW_PRINT_SEL(m, "amsdu tasklet count = %u\n", pxmitpriv->amsdu_debug_tasklet);
		RTW_PRINT_SEL(m, "amsdu enqueue count = %u\n", pxmitpriv->amsdu_debug_enqueue);
		RTW_PRINT_SEL(m, "amsdu dequeue count = %u\n", pxmitpriv->amsdu_debug_dequeue);
	}

	return 0;
}

ssize_t proc_set_tx_amsdu(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	char tmp[32];
	u32 i, amsdu;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &amsdu);

		if (padapter && (num == 1)) {
			RTW_INFO("padapter->tx_amsdu = %x\n", amsdu);

			if (amsdu == 255) {
				pxmitpriv->amsdu_debug_set_timer = 0;
				pxmitpriv->amsdu_debug_timeout = 0;
				pxmitpriv->amsdu_debug_tasklet = 0;
				pxmitpriv->amsdu_debug_enqueue = 0;
				pxmitpriv->amsdu_debug_dequeue = 0;
				for (i = 0; i < AMSDU_DEBUG_MAX_COUNT; i++)
					pxmitpriv->amsdu_debug_coalesce[i] = 0;
			} else {
				padapter->tx_amsdu = amsdu;
			}
		}
	}

	return count;
}

int proc_get_tx_amsdu_rate(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	if (padapter)
		RTW_PRINT_SEL(m, "tx amsdu rate = %d Mbps\n", padapter->tx_amsdu_rate);

	return 0;
}

ssize_t proc_set_tx_amsdu_rate(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 amsdu_rate;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &amsdu_rate);

		if (padapter && (num == 1)) {
			RTW_INFO("padapter->tx_amsdu_rate = %x\n", amsdu_rate);
			padapter->tx_amsdu_rate = amsdu_rate;
		}
	}

	return count;
}
#endif /* CONFIG_TX_AMSDU */
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
int proc_get_vht_24g_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _ADAPTER *a = (struct _ADAPTER *)rtw_netdev_priv(dev);
	struct registry_priv *regpriv = &a->registrypriv;


	if (regpriv)
		RTW_PRINT_SEL(m, "%d\n", regpriv->vht_24g_enable);

	return 0;
}

ssize_t proc_set_vht_24g_enable(struct file *file, const char __user *buffer,
				size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	struct _ADAPTER *a = (struct _ADAPTER *)rtw_netdev_priv(dev);
	struct registry_priv *regpriv = &a->registrypriv;
	char tmp[32];
	u32 mode;


	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if ((num == 1) && regpriv && (mode < 2)) {
			regpriv->vht_24g_enable = mode;
			RTW_INFO("vht_24g_enable = %d\n", regpriv->vht_24g_enable);
		}
	}

	return count;
}
#endif /* CONFIG_80211AC_VHT */

ssize_t proc_set_dyn_rrsr(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	char tmp[32] = {0};
	u32 num = 0, enable = 0, rrsr_val = 0; /* gpio_mode:0 input  1:output; */

	if (count < 2)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num	= sscanf(tmp, "%d 0x%x", &enable, &rrsr_val);
		RTW_INFO("num=%u enable=%d rrsr_val=0x%x\n", num, enable, rrsr_val);
		pregpriv->en_dyn_rrsr = enable;
		pregpriv->set_rrsr_value = rrsr_val;
		/*rtw_phydm_dyn_rrsr_en(padapter, enable);*/
		/*rtw_phydm_set_rrsr(padapter, rrsr_val, _TRUE);*/

	}
	return count;

}
int proc_get_dyn_rrsr(struct seq_file *m, void *v) {

	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	u32 init_rrsr =0xFFFFFFFF;

	if (padapter) 
		RTW_PRINT_SEL(m, "en_dyn_rrsr = %d fixed_rrsr_value =0x%x %s\n"
			, pregpriv->en_dyn_rrsr
			, pregpriv->set_rrsr_value 
			, (pregpriv->set_rrsr_value == init_rrsr)?"(default)":"(fixed)"
		);

	return 0;
}

#ifdef CONFIG_80211N_HT
void rtw_dump_dft_phy_cap(void *sel, _adapter *adapter)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_priv *pmlmepriv = &adapter_link->mlmepriv;
	struct ht_priv	*phtpriv = &pmlmepriv->htpriv;
	#ifdef CONFIG_80211AC_VHT
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;
	#endif

	#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT STBC Tx : %s\n", (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT STBC Rx : %s\n", (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX)) ? "V" : "X");
	#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT STBC Tx : %s\n", (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT STBC Rx : %s\n\n", (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_RX)) ? "V" : "X");

	#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT LDPC Tx : %s\n", (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT LDPC Rx : %s\n", (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX)) ? "V" : "X");
	#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT LDPC Tx : %s\n", (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT LDPC Rx : %s\n\n", (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_RX)) ? "V" : "X");

	#ifdef CONFIG_BEAMFORMING
	#ifdef CONFIG_80211AX_HE
        RTW_PRINT_SEL(sel, "[DFT CAP] HE MU Bfer : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_HE_MU_MIMO_AP_ENABLE)) ? "V" : "X");
        RTW_PRINT_SEL(sel, "[DFT CAP] HE MU Bfee : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_HE_MU_MIMO_STA_ENABLE)) ? "V" : "X");
        RTW_PRINT_SEL(sel, "[DFT CAP] HE SU Bfer : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_HE_BEAMFORMER_ENABLE)) ? "V" : "X");
        RTW_PRINT_SEL(sel, "[DFT CAP] HE SU Bfee : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_HE_BEAMFORMEE_ENABLE)) ? "V" : "X");
        #endif
	#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT MU Bfer : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT MU Bfee : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT SU Bfer : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT SU Bfee : %s\n", (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)) ? "V" : "X");
	#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT Bfer : %s\n", (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE))  ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT Bfee : %s\n", (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE)) ? "V" : "X");
	#endif
}

void rtw_get_dft_phy_cap(void *sel, _adapter *adapter)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	RTW_PRINT_SEL(sel, "\n ======== PHY CAP protocol ========\n");
	rtw_ht_use_default_setting(adapter, adapter_link);
	#ifdef CONFIG_80211AC_VHT
	rtw_vht_get_real_setting(adapter, adapter_link);
	#endif
	#ifdef CONFIG_80211N_HT
	rtw_dump_dft_phy_cap(sel, adapter);
	#endif
}

void rtw_dump_drv_phy_cap(void *sel, _adapter *adapter)
{
	struct registry_priv	*pregistry_priv = &adapter->registrypriv;

	RTW_PRINT_SEL(sel, "\n ======== DRV's configuration ========\n");
	#if 0
	RTW_PRINT_SEL(sel, "[DRV CAP] TRx Capability : 0x%08x\n", phy_spec->trx_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Stream Num Index : %d\n", (phy_spec->trx_cap >> 24) & 0xFF); /*Tx Stream Num Index [31:24]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] Rx Stream Num Index : %d\n", (phy_spec->trx_cap >> 16) & 0xFF); /*Rx Stream Num Index [23:16]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Path Num Index : %d\n", (phy_spec->trx_cap >> 8) & 0xFF);/*Tx Path Num Index	[15:8]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] Rx Path Num Index : %d\n", (phy_spec->trx_cap & 0xFF));/*Rx Path Num Index	[7:0]*/
	#endif
	#ifdef CONFIG_80211N_HT
	RTW_PRINT_SEL(sel, "[DRV CAP] STBC Capability : 0x%04x\n", pregistry_priv->stbc_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] HT STBC Tx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT5)) ? "V" : "X"); /*BIT5: Enable HT STBC Tx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HT STBC Rx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT4)) ? "V" : "X"); /*BIT4: Enable HT STBC Rx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT STBC Tx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT1)) ? "V" : "X"); /*BIT1: Enable VHT STBC Tx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT STBC Rx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT0)) ? "V" : "X"); /*BIT0: Enable VHT STBC Rx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HE STBC Tx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT10)) ? "V" : "X"); /*BIT10: Enable HE STBC Tx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HE STBC Rx : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT8)) ? "V" : "X"); /*BIT8: Enable HE STBC Rx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HE STBC Tx(>80M) : %s\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT11)) ? "V" : "X"); /*BIT11: Enable HE STBC Tx(>80M)*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HE STBC Rx(>80M) : %s\n\n", (TEST_FLAG(pregistry_priv->stbc_cap, BIT9)) ? "V" : "X"); /*BIT9: Enable HE STBC Rx(>80M)*/


	RTW_PRINT_SEL(sel, "[DRV CAP] LDPC Capability : 0x%02x\n", pregistry_priv->ldpc_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT LDPC Tx : %s\n", (TEST_FLAG(pregistry_priv->ldpc_cap, BIT1)) ? "V" : "X"); /*BIT1: Enable VHT LDPC Tx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT LDPC Rx : %s\n", (TEST_FLAG(pregistry_priv->ldpc_cap, BIT0)) ? "V" : "X"); /*BIT0: Enable VHT LDPC Rx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HT LDPC Tx : %s\n", (TEST_FLAG(pregistry_priv->ldpc_cap, BIT5)) ? "V" : "X"); /*BIT5: Enable HT LDPC Tx*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HT LDPC Rx : %s\n\n", (TEST_FLAG(pregistry_priv->ldpc_cap, BIT4)) ? "V" : "X"); /*BIT4: Enable HT LDPC Rx*/
	#endif /* CONFIG_80211N_HT */
	#ifdef CONFIG_BEAMFORMING
	#if 0
	RTW_PRINT_SEL(sel, "[DRV CAP] TxBF parameter : 0x%08x\n", phy_spec->txbf_param);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT Sounding Dim : %d\n", (phy_spec->txbf_param >> 24) & 0xFF); /*VHT Sounding Dim [31:24]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT Steering Ant : %d\n", (phy_spec->txbf_param >> 16) & 0xFF); /*VHT Steering Ant [23:16]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Sounding Dim : %d\n", (phy_spec->txbf_param >> 8) & 0xFF); /*HT Sounding Dim [15:8]*/
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Steering Ant : %d\n", phy_spec->txbf_param & 0xFF); /*HT Steering Ant [7:0]*/
	#endif

	/*
	 * BIT0: Enable VHT SU Beamformer
	 * BIT1: Enable VHT SU Beamformee
	 * BIT2: Enable VHT MU Beamformer, depend on VHT SU Beamformer
	 * BIT3: Enable VHT MU Beamformee, depend on VHT SU Beamformee
	 * BIT4: Enable HT Beamformer
	 * BIT5: Enable HT Beamformee
	 * BIT6: Enable HE SU Beamformer
	 * BIT7: Enable HE SU Beamformee
	 * BIT8: Enable HE MU Beamformer
	 * BIT9: Enable HE MU Beamformee
	 */
	RTW_PRINT_SEL(sel, "[DRV CAP] TxBF Capability : 0x%04x\n", pregistry_priv->beamform_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT MU Bfer : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT2)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT MU Bfee : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT3)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT SU Bfer : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT0)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT SU Bfee : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT1)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Bfer : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT4))  ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Bfee : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT5)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HE SU Bfer : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT6)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HE SU Bfee : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT7)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HE MU Bfer : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT8)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HE MU Bfee : %s\n", (TEST_FLAG(pregistry_priv->beamform_cap, BIT9)) ? "V" : "X");

	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Bfer rf_num : %d\n", pregistry_priv->beamformer_rf_num);
	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Bfee rf_num : %d\n", pregistry_priv->beamformee_rf_num);
	#endif
}

void rtw_dump_macaddr(void *sel, _adapter *adapter)
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u8 mac_addr[ETH_ALEN];

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			RTW_PRINT_SEL(sel, ADPT_FMT"- mac_addr ="MAC_FMT"\n",
				ADPT_ARG(iface), MAC_ARG(adapter_mac_addr(iface)));
		}
	}
}

int proc_get_stbc_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%04x\n", pregpriv->stbc_cap);

	return 0;
}

ssize_t proc_set_stbc_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if (num == 1 && pregpriv) {
			pregpriv->stbc_cap = mode;
			RTW_INFO("stbc_cap = 0x%02x\n", mode);
		}
	}

	return count;
}

int proc_get_ldpc_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%02x\n", pregpriv->ldpc_cap);

	return 0;
}

ssize_t proc_set_ldpc_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if (num == 1 && pregpriv) {
			pregpriv->ldpc_cap = mode;
			RTW_INFO("ldpc_cap = 0x%02x\n", mode);
		}
	}

	return count;
}
#ifdef CONFIG_BEAMFORMING
int proc_get_txbf_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%04x\n", pregpriv->beamform_cap);

	return 0;
}

ssize_t proc_set_txbf_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &mode);

		if (num == 1 && pregpriv) {
			pregpriv->beamform_cap = mode;
			RTW_INFO("beamform_cap = 0x%04x\n", mode);
		}
	}

	return count;
}
#endif
#endif /* CONFIG_80211N_HT */

/*int proc_get_rssi_disp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	return 0;
}
*/

/*ssize_t proc_set_rssi_disp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 enable=0;

	if (count < 1)
	{
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x", &enable);

		if (num !=  1) {
			RTW_INFO("invalid set_rssi_disp parameter!\n");
			return count;
		}

		if(enable)
		{
			RTW_INFO("Linked info Function Enable\n");
			padapter->bLinkInfoDump = enable ;
		}
		else
		{
			RTW_INFO("Linked info Function Disable\n");
			padapter->bLinkInfoDump = 0 ;
		}

	}

	return count;

}

*/
#ifdef CONFIG_AP_MODE

void dump_phl_tring_status(struct seq_file *m, _adapter *padapter, struct sta_info *psta)
{
	int i = 0;
	u16 tring_len = 0;

	RTW_PRINT_SEL(m, "PHL_tring_len=");
	for (i = 0; i < MAX_PHL_RING_CAT_NUM; i++) {
		tring_len = rtw_phl_tring_rsc(padapter->dvobj->phl, psta->phl_sta->macid, i);
		RTW_PRINT_SEL(m, "%d ", tring_len);
	}
	RTW_PRINT_SEL(m, "\n");
}

int proc_get_sta_active_time(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_list *phead, *plist;
	struct sta_info *psta;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	u32 i, macid_rec_idx = 0;
	u8 sta_mac[NUM_STA][ETH_ALEN]={{0}};

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

	while((rtw_end_of_queue_search(phead, plist) == _FALSE)){
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);
		_rtw_memcpy(&sta_mac[macid_rec_idx][0], psta->phl_sta->mac_addr, ETH_ALEN);
		macid_rec_idx++;
	}
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	for(i = 0; i < macid_rec_idx; i++){
		psta = rtw_get_stainfo(pstapriv, &sta_mac[i][0]);
		if(psta){
			_rtw_spinlock_bh(&pstapriv->active_time_lock);
			RTW_PRINT_SEL(m, "==============================\n");
			RTW_PRINT_SEL(m, "sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
			RTW_PRINT_SEL(m, "start active time: %u\n", psta->start_active_time);
			RTW_PRINT_SEL(m, "latest active time: %u\n", psta->latest_active_time);
			RTW_PRINT_SEL(m, "==============================\n");
			_rtw_spinunlock_bh(&pstapriv->active_time_lock);
		}
	}
	return 0;
}

int proc_get_all_sta_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct sta_info *psta;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	int i;
	_list	*plist, *phead;

	RTW_MAP_DUMP_SEL_ALWAYS(m, "sta_dz_bitmap=", pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
	RTW_MAP_DUMP_SEL_ALWAYS(m, "tim_bitmap=", pstapriv->tim_bitmap, pstapriv->aid_bmp_len);

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

			plist = get_next(plist);

			/* if(extra_arg == psta->phl_sta->aid) */
			{
				RTW_PRINT_SEL(m, "==============================\n");
				RTW_PRINT_SEL(m, "sta's macaddr:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
				RTW_PRINT_SEL(m, "ieee8021x_blocked=%d\n", psta->ieee8021x_blocked);
				RTW_PRINT_SEL(m, "rtsen=%d, cts2slef=%d, hw_rts_en=%d\n",
						psta->rtsen, psta->cts2self, psta->hw_rts_en);
				/* ToDo: need API to query hal_sta->ra_info.rate_id */
				/* RTW_PRINT_SEL(m, "state=0x%x, aid=%d, macid=%d, raid=%d\n",
					psta->state, psta->phl_sta->aid, psta->phl_sta->macid, psta->phl_sta->hal_sta->ra_info.rate_id); */
				RTW_PRINT_SEL(m, "state=0x%x, aid=%d, macid=%d\n",
					psta->state, psta->phl_sta->aid, psta->phl_sta->macid);
#ifdef CONFIG_RTS_FULL_BW
				if(psta->vendor_8812)
					RTW_PRINT_SEL(m,"Vendor Realtek 8812\n");
#endif/*CONFIG_RTS_FULL_BW*/
#ifdef CONFIG_80211N_HT
				RTW_PRINT_SEL(m, "qos_en=%d, ht_en=%d, vht_en=%d, he_en=%d, init_rate=%d, bwmode=%d\n",
					psta->qos_option, psta->htpriv.ht_option, psta->vhtpriv.vht_option, psta->hepriv.he_option,
					psta->init_rate, psta->phl_sta->chandef.bw);

				RTW_PRINT_SEL(m, "ampdu_enable = %d\n", psta->ampdu_priv.ampdu_enable);
				RTW_PRINT_SEL(m, "tx_amsdu_enable = %d\n", psta->ampdu_priv.tx_amsdu_enable);
				RTW_PRINT_SEL(m, "agg_enable_bitmap=%x, candidate_tid_bitmap=%x\n", psta->ampdu_priv.agg_enable_bitmap, psta->ampdu_priv.candidate_tid_bitmap);

				if (psta->htpriv.ht_option) {
					RTW_PRINT_SEL(m, "ht_ch_offset=%d, ht_is_8K_amsdu=%d\n",
						psta->htpriv.ch_offset,
						(psta->htpriv.ht_cap.cap_info & IEEE80211_HT_CAP_MAX_AMSDU) ? 1 : 0);

					RTW_PRINT_SEL(m, "ht_sgi_20/40m=(%d, %d)\n",
						psta->htpriv.sgi_20m, psta->htpriv.sgi_40m);

					RTW_PRINT_SEL(m, "ht_max_bitrate_20/40m=(%u, %u)\n",
						rtw_ht_mcs_rate(0, psta->htpriv.sgi_20m, psta->htpriv.ht_cap.supp_mcs_set) / 10,
						rtw_ht_mcs_rate(1, psta->htpriv.sgi_40m, psta->htpriv.ht_cap.supp_mcs_set) / 10);
				}
#ifdef CONFIG_80211AC_VHT
				if (psta->vhtpriv.vht_option) {
					RTW_PRINT_SEL(m, "vht_sgi_80/160m=(%d, %d)\n",
						psta->vhtpriv.sgi_80m, psta->vhtpriv.sgi_160m);

					RTW_PRINT_SEL(m, "vht_max_bitrate_20/40/80/160m=(%u, %u, %u, %u)\n",
						(rtw_vht_mcs_to_data_rate(0, psta->htpriv.sgi_20m, psta->vhtpriv.vht_highest_rate) + 1) >> 1,
						(rtw_vht_mcs_to_data_rate(1, psta->htpriv.sgi_40m, psta->vhtpriv.vht_highest_rate) + 1) >> 1,
						(rtw_vht_mcs_to_data_rate(2, psta->vhtpriv.sgi_80m, psta->vhtpriv.vht_highest_rate) + 1) >> 1,
						(rtw_vht_mcs_to_data_rate(3, psta->vhtpriv.sgi_160m, psta->vhtpriv.vht_highest_rate) + 1) >> 1);

					RTW_PRINT_SEL(m, "vht_ldpc_cap=0x%x, vht_stbc_cap=0x%x, vht_beamform_cap=0x%x\n", psta->vhtpriv.ldpc_cap, psta->vhtpriv.stbc_cap, psta->vhtpriv.beamform_cap);
					RTW_PRINT_SEL(m, "vht_mcs_map=0x%x, vht_highest_rate=0x%x, vht_ampdu_len=%d\n", *(u16 *)psta->vhtpriv.vht_mcs_map, psta->vhtpriv.vht_highest_rate, psta->vhtpriv.ampdu_len);
					RTW_MAP_DUMP_SEL_ALWAYS(m, "vht_cap=", psta->vhtpriv.vht_cap, 32);
				}
#ifdef CONFIG_80211AX_HE
				if (psta->hepriv.he_option) {
					/* NOTE: ltf_gi is unused in rtw_he_mcs_to_data_rate currently */
					RTW_PRINT_SEL(m, "he_max_bitrate_20/40/80/160m=(%u, %u, %u, %u)\n",
						(rtw_he_mcs_to_data_rate(0, psta->phl_sta->asoc_cap.ltf_gi, psta->hepriv.he_highest_rate) + 1) >> 1,
						(rtw_he_mcs_to_data_rate(1, psta->phl_sta->asoc_cap.ltf_gi, psta->hepriv.he_highest_rate) + 1) >> 1,
						(rtw_he_mcs_to_data_rate(2, psta->phl_sta->asoc_cap.ltf_gi, psta->hepriv.he_highest_rate) + 1) >> 1,
						(rtw_he_mcs_to_data_rate(3, psta->phl_sta->asoc_cap.ltf_gi, psta->hepriv.he_highest_rate) + 1) >> 1);

					RTW_MAP_DUMP_SEL_ALWAYS(m, "he_cap=", psta->hepriv.he_cap, HE_CAP_ELE_MAX_LEN);
				}
#endif
#endif /* CONFIG_80211N_VHT */
#endif /* CONFIG_80211N_HT */
				RTW_PRINT_SEL(m, "tx_nss=%d\n", rtw_get_sta_tx_nss(padapter, psta));
				RTW_PRINT_SEL(m, "rx_nss=%d\n", rtw_get_sta_rx_nss(padapter, psta));
				RTW_PRINT_SEL(m, "sleepq_len=%d\n", psta->sleepq_len);
				RTW_PRINT_SEL(m, "sta_xmitpriv.vo_q_qcnt=%d\n", psta->sta_xmitpriv.vo_q.qcnt);
				RTW_PRINT_SEL(m, "sta_xmitpriv.vi_q_qcnt=%d\n", psta->sta_xmitpriv.vi_q.qcnt);
				RTW_PRINT_SEL(m, "sta_xmitpriv.be_q_qcnt=%d\n", psta->sta_xmitpriv.be_q.qcnt);
				RTW_PRINT_SEL(m, "sta_xmitpriv.bk_q_qcnt=%d\n", psta->sta_xmitpriv.bk_q.qcnt);

				RTW_PRINT_SEL(m, "capability=0x%x\n", psta->capability);
				RTW_PRINT_SEL(m, "flags=0x%x\n", psta->flags);
				RTW_PRINT_SEL(m, "isPMF=%d\n", (psta->flags & WLAN_STA_MFP)?1:0);
				RTW_PRINT_SEL(m, "wpa_psk=0x%x\n", psta->wpa_psk);
				RTW_PRINT_SEL(m, "wpa2_group_cipher=0x%x\n", psta->wpa2_group_cipher);
				RTW_PRINT_SEL(m, "wpa2_pairwise_cipher=0x%x\n", psta->wpa2_pairwise_cipher);
				RTW_PRINT_SEL(m, "qos_info=0x%x\n", psta->qos_info);
				RTW_PRINT_SEL(m, "dot118021XPrivacy=0x%x\n", psta->dot118021XPrivacy);
				RTW_PRINT_SEL(m, "AuthAlgrthm=0x%x\n", psta->authalg);
#ifdef CONFIG_RECV_REORDERING_CTRL
				sta_rx_reorder_ctl_dump(m, psta);
#endif
#ifdef CONFIG_TDLS
				RTW_PRINT_SEL(m, "tdls_sta_state=0x%08x\n", psta->tdls_sta_state);
				RTW_PRINT_SEL(m, "PeerKey_Lifetime=%d\n", psta->TDLS_PeerKey_Lifetime);
#endif /* CONFIG_TDLS */
				if(psta->bssratelen >= 0 && psta->bssratelen <= 16)
					RTW_MAP_DUMP_SEL_ALWAYS(m, "bssrateset=", psta->bssrateset, psta->bssratelen);
				RTW_PRINT_SEL(m, "rx_data_uc_pkts=%llu\n", sta_rx_data_uc_pkts(psta));
				RTW_PRINT_SEL(m, "rx_data_mc_pkts=%llu\n", psta->sta_stats.rx_data_mc_pkts);
				RTW_PRINT_SEL(m, "rx_data_bc_pkts=%llu\n", psta->sta_stats.rx_data_bc_pkts);
				RTW_PRINT_SEL(m, "rx_uc_bytes=%llu\n", sta_rx_uc_bytes(psta));
				RTW_PRINT_SEL(m, "rx_mc_bytes=%llu\n", psta->sta_stats.rx_mc_bytes);
				RTW_PRINT_SEL(m, "rx_bc_bytes=%llu\n", psta->sta_stats.rx_bc_bytes);
				if (psta->sta_stats.rx_tp_kbits >> 10)
					RTW_PRINT_SEL(m, "rx_tp =%d (Mbps)\n", psta->sta_stats.rx_tp_kbits >> 10);
				else
					RTW_PRINT_SEL(m, "rx_tp =%d (Kbps)\n", psta->sta_stats.rx_tp_kbits);

				RTW_PRINT_SEL(m, "tx_data_pkts=%llu\n", psta->sta_stats.tx_pkts);
				RTW_PRINT_SEL(m, "tx_bytes=%llu\n", psta->sta_stats.tx_bytes);
				if (psta->sta_stats.tx_tp_kbits >> 10)
					RTW_PRINT_SEL(m, "tx_tp =%d (Mbps)\n", psta->sta_stats.tx_tp_kbits >> 10);
				else
					RTW_PRINT_SEL(m, "tx_tp =%d (Kbps)\n", psta->sta_stats.tx_tp_kbits);
#ifdef CONFIG_RTW_80211K
				RTW_PRINT_SEL(m, "rm_en_cap="RM_CAP_FMT"\n", RM_CAP_ARG(psta->rm_en_cap));
#endif
				dump_st_ctl(m, &psta->st_ctl);

				if (STA_OP_WFD_MODE(psta))
					RTW_PRINT_SEL(m, "op_wfd_mode:0x%02x\n", STA_OP_WFD_MODE(psta));

				RTW_PRINT_SEL(m, "tx_bitrate_100kbps=%u\n",
						rtw_desc_rate_to_bitrate(psta->phl_sta->rlink->chandef.bw,
								       rtw_get_current_tx_rate(padapter, psta),
								       rtw_get_current_tx_sgi(padapter, psta)));
				RTW_PRINT_SEL(m, "rx_bitrate_100kbps=%u\n",
						rtw_desc_rate_to_bitrate(psta->phl_sta->rlink->chandef.bw,
					    psta->curr_rx_rate, psta->curr_rx_gi_ltf));

				RTW_PRINT_SEL(m, "rssi=%d\n", rtw_phl_get_sta_rssi(psta->phl_sta));

				dump_phl_tring_status(m, padapter, psta);
				RTW_PRINT_SEL(m, "==============================\n");
			}

		}

	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

	return 0;
}

#endif

#ifdef CONFIG_RTKM
int proc_get_rtkm_info(struct seq_file *m, void *v)
{
	rtkm_dump_mstatus(m);
	return 0;
}

ssize_t proc_set_rtkm_info(struct file *file, const char __user *buffer,
			   size_t count, loff_t *pos, void *data)
{
	char tmp[32] = { 0 };
	unsigned int mask = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%x", &mask);

		if (num == 1)
			rtkm_set_trace(mask);
	}

	return count;
}
#endif /* CONFIG_RTKM */

#ifdef DBG_MEMORY_LEAK
#include <asm/atomic.h>
extern ATOMIC_T _malloc_cnt;
extern ATOMIC_T _malloc_size;

int proc_get_malloc_cnt(struct seq_file *m, void *v)
{
	RTW_PRINT_SEL(m, "_malloc_cnt=%d\n", atomic_read(&_malloc_cnt));
	RTW_PRINT_SEL(m, "_malloc_size=%d\n", atomic_read(&_malloc_size));

	return 0;
}
#endif /* DBG_MEMORY_LEAK */

#ifdef CONFIG_FIND_BEST_CHANNEL
int proc_get_best_channel(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	u32 i, best_channel_24G = 1, best_channel_5G = 36, index_24G = 0, index_5G = 0;

	for (i = 0; i < chset->chs_len; i++) {
		if (chset->chs[i].flags & RTW_CHF_DIS)
			continue;
		if (chset->chs[i].ChannelNum == 1)
			index_24G = i;
		if (chset->chs[i].ChannelNum == 36)
			index_5G = i;
	}

	for (i = 0; i < chset->chs_len; i++) {
		if (chset->chs[i].flags & RTW_CHF_DIS)
			continue;
		/* 2.4G */
		if (chset->chs[i].ChannelNum == 6) {
			if (chset->chs[i].rx_count < chset->chs[index_24G].rx_count) {
				index_24G = i;
				best_channel_24G = chset->chs[i].ChannelNum;
			}
		}

		/* 5G */
		if (chset->chs[i].ChannelNum >= 36
		    && chset->chs[i].ChannelNum < 140) {
			/* Find primary channel */
			if (((chset->chs[i].ChannelNum - 36) % 8 == 0)
			    && (chset->chs[i].rx_count < chset->chs[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = chset->chs[i].ChannelNum;
			}
		}

		if (chset->chs[i].ChannelNum >= 149
		    && chset->chs[i].ChannelNum < 165) {
			/* find primary channel */
			if (((chset->chs[i].ChannelNum - 149) % 8 == 0)
			    && (chset->chs[i].rx_count < chset->chs[index_5G].rx_count)) {
				index_5G = i;
				best_channel_5G = chset->chs[i].ChannelNum;
			}
		}
#if 1 /* debug */
		RTW_PRINT_SEL(m, "The rx cnt of channel %3d = %d\n",
			chset->chs[i].ChannelNum, chset->chs[i].rx_count);
#endif
	}

	RTW_PRINT_SEL(m, "best_channel_5G = %d\n", best_channel_5G);
	RTW_PRINT_SEL(m, "best_channel_24G = %d\n", best_channel_24G);

	return 0;
}

ssize_t proc_set_best_channel(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int i;
		for (i = 0; i < chset->chs_len; i++)
			chset->chs[i].rx_count = 0;

		RTW_INFO("set %s\n", "Clean Best Channel Count");
	}

	return count;
}
#endif /* CONFIG_FIND_BEST_CHANNEL */

#if defined(DBG_CONFIG_ERROR_DETECT)
int proc_get_sreset(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *psdpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(psdpriv);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	if (psrtpriv->dbg_sreset_ctrl == _TRUE) {
		RTW_PRINT_SEL(m, "self_dect_tx_cnt:%llu\n", psrtpriv->self_dect_tx_cnt);
		RTW_PRINT_SEL(m, "self_dect_rx_cnt:%llu\n", psrtpriv->self_dect_rx_cnt);
		RTW_PRINT_SEL(m, "self_dect_fw_cnt:%llu\n", psrtpriv->self_dect_fw_cnt);
		RTW_PRINT_SEL(m, "tx_dma_status_cnt:%llu\n", psrtpriv->tx_dma_status_cnt);
		RTW_PRINT_SEL(m, "rx_dma_status_cnt:%llu\n", psrtpriv->rx_dma_status_cnt);
		RTW_PRINT_SEL(m, "self_dect_case:%d\n", psrtpriv->self_dect_case);
		RTW_PRINT_SEL(m, "dbg_sreset_cnt:%d\n", pdbgpriv->dbg_sreset_cnt);
	}
	return 0;
}

ssize_t proc_set_sreset(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(adapter_to_dvobj(padapter));
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	char tmp[32];
	s32 trigger_point;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d", &trigger_point);

		if (num < 1)
			return count;

		if (trigger_point == SRESET_TGP_NULL)
			rtw_hal_sreset_reset(padapter);
		else if (trigger_point == SRESET_TGP_INFO)
			psrtpriv->dbg_sreset_ctrl = _TRUE;
		else
			sreset_set_trigger_point(padapter, trigger_point);
	}

	return count;

}
#endif /* DBG_CONFIG_ERROR_DETECT */

#ifdef CONFIG_PCI_HCI

ssize_t proc_set_pci_bridge_conf_space(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv       *pdvobjpriv = adapter_to_dvobj(padapter);
	PPCI_DATA pci_data = dvobj_to_pci(pdvobjpriv);
	struct pci_dev  *pdev = pci_data->ppcidev;
	struct pci_dev  *bridge_pdev = pdev->bus->self;

	char tmp[32] = { 0 };
	int num;

	u32 reg = 0, value = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num = sscanf(tmp, "%x %x", &reg, &value);
		if (num != 2) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}

		if (reg >= 0x1000) {
			RTW_INFO("invalid register!\n");
			return count;
		}

		if (value > 0xFF) {
			RTW_INFO("invalid value! Only one byte\n");
			return count;
		}

		RTW_INFO(FUNC_ADPT_FMT ": register 0x%x value 0x%x\n",
			FUNC_ADPT_ARG(padapter), reg, value);

		pci_write_config_byte(bridge_pdev, reg, value);
	}
	return count;
}


int proc_get_pci_bridge_conf_space(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	struct dvobj_priv       *pdvobjpriv = adapter_to_dvobj(padapter);
	PPCI_DATA pci_data = dvobj_to_pci(pdvobjpriv);
	struct pci_dev  *pdev = pci_data->ppcidev;
	struct pci_dev  *bridge_pdev = pdev->bus->self;

	u32 tmp[4] = { 0 };
	u32 i, j;

	RTW_PRINT_SEL(m, "\n*****  PCI Host Device Configuration Space*****\n\n");

	for (i = 0; i < 0x1000; i += 0x10) {
		for (j = 0 ; j < 4 ; j++)
			pci_read_config_dword(bridge_pdev, i + j * 4, tmp+j);

		RTW_PRINT_SEL(m, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			i, tmp[0] & 0xFF, (tmp[0] >> 8) & 0xFF, (tmp[0] >> 16) & 0xFF, (tmp[0] >> 24) & 0xFF,
			tmp[1] & 0xFF, (tmp[1] >> 8) & 0xFF, (tmp[1] >> 16) & 0xFF, (tmp[1] >> 24) & 0xFF,
			tmp[2] & 0xFF, (tmp[2] >> 8) & 0xFF, (tmp[2] >> 16) & 0xFF, (tmp[2] >> 24) & 0xFF,
			tmp[3] & 0xFF, (tmp[3] >> 8) & 0xFF, (tmp[3] >> 16) & 0xFF, (tmp[3] >> 24) & 0xFF);
	}
	return 0;
}


ssize_t proc_set_pci_conf_space(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv       *pdvobjpriv = adapter_to_dvobj(padapter);
	PPCI_DATA pci_data = dvobj_to_pci(pdvobjpriv);
	struct pci_dev  *pdev = pci_data->ppcidev;

	char tmp[32] = { 0 };
	int num;

	u32 reg = 0, value = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num = sscanf(tmp, "%x %x", &reg, &value);

		if (num != 2) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}


		if (reg >= 0x1000) {
			RTW_INFO("invalid register!\n");
			return count;
		}

		if (value > 0xFF) {
			RTW_INFO("invalid value! Only one byte\n");
			return count;
		}

		RTW_INFO(FUNC_ADPT_FMT ": register 0x%x value 0x%x\n",
			FUNC_ADPT_ARG(padapter), reg, value);

		pci_write_config_byte(pdev, reg, value);


	}
	return count;
}


int proc_get_pci_conf_space(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	struct dvobj_priv       *pdvobjpriv = adapter_to_dvobj(padapter);
	PPCI_DATA pci_data = dvobj_to_pci(pdvobjpriv);
	struct pci_dev *pdev = pci_data->ppcidev;
	struct pci_dev *bridge_pdev = pdev->bus->self;

	u32 tmp[4] = { 0 };
	u32 i, j;

	RTW_PRINT_SEL(m, "\n*****  PCI Device Configuration Space *****\n\n");

	for (i = 0; i < 0x1000; i += 0x10) {
		for (j = 0 ; j < 4 ; j++)
			pci_read_config_dword(pdev, i + j * 4, tmp+j);

		RTW_PRINT_SEL(m, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			i, tmp[0] & 0xFF, (tmp[0] >> 8) & 0xFF, (tmp[0] >> 16) & 0xFF, (tmp[0] >> 24) & 0xFF,
			tmp[1] & 0xFF, (tmp[1] >> 8) & 0xFF, (tmp[1] >> 16) & 0xFF, (tmp[1] >> 24) & 0xFF,
			tmp[2] & 0xFF, (tmp[2] >> 8) & 0xFF, (tmp[2] >> 16) & 0xFF, (tmp[2] >> 24) & 0xFF,
			tmp[3] & 0xFF, (tmp[3] >> 8) & 0xFF, (tmp[3] >> 16) & 0xFF, (tmp[3] >> 24) & 0xFF);
	}

	return 0;
}


int proc_get_pci_aspm(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	PPCI_DATA pci_data = dvobj_to_pci(pdvobjpriv);
	struct pci_priv	*pcipriv = &(pci_data->pcipriv);
	u8 tmp8 = 0;
	u16 tmp16 = 0;
	u32 tmp32 = 0;
	u8 l1_idle = 0;
	u32 l1ss_cap_ptr = 0;


	RTW_PRINT_SEL(m, "***** ASPM Capability *****\n");

	pci_read_config_dword(pci_data->ppcidev, pcipriv->pciehdr_offset + PCI_EXP_LNKCAP, &tmp32);

	RTW_PRINT_SEL(m, "CLK REQ:	%s\n", (tmp32&PCI_EXP_LNKCAP_CLKPM) ? "Enable" : "Disable");
	RTW_PRINT_SEL(m, "ASPM L0s:	%s\n", (tmp32&BIT10) ? "Enable" : "Disable");
	RTW_PRINT_SEL(m, "ASPM L1:	%s\n", (tmp32&BIT11) ? "Enable" : "Disable");

	l1ss_cap_ptr = pci_find_ext_capability(pci_data->ppcidev, PCI_EXT_CAP_ID_L1SS);
	pci_read_config_byte(pci_data->ppcidev, l1ss_cap_ptr + PCI_L1SS_CAP, &tmp8);
	RTW_PRINT_SEL(m, "ASPM L1OFF:	%s\n", (tmp8 & (PCI_L1SS_CAP_PCIPM_L1_2 |
				 PCI_L1SS_CAP_PCIPM_L1_1 | PCI_L1SS_CAP_ASPM_L1_2 |
				 PCI_L1SS_CAP_ASPM_L1_1 | PCI_L1SS_CAP_L1_PM_SS)) ?
				 "Enable" : "Disable");

	RTW_PRINT_SEL(m, "***** ASPM CTRL Reg *****\n");

	pci_read_config_word(pci_data->ppcidev, pcipriv->pciehdr_offset + PCI_EXP_LNKCTL, &tmp16);

	RTW_PRINT_SEL(m, "CLK REQ:	%s\n", (tmp16&PCI_EXP_LNKCTL_CLKREQ_EN) ? "Enable" : "Disable");
	RTW_PRINT_SEL(m, "ASPM L0s:	%s\n", (tmp16&BIT0) ? "Enable" : "Disable");
	RTW_PRINT_SEL(m, "ASPM L1:	%s\n", (tmp16&BIT1) ? "Enable" : "Disable");

	pci_read_config_byte(pci_data->ppcidev, l1ss_cap_ptr + PCI_L1SS_CTL1, &tmp8);
	RTW_PRINT_SEL(m, "ASPM L1OFF:	%s\n", (tmp8 & (PCI_L1SS_CTL1_L1SS_MASK)) ? "Enable" : "Disable");

	RTW_PRINT_SEL(m, "***** ASPM Backdoor *****\n");

	pci_read_config_byte(pci_data->ppcidev, 0x719, &tmp8);
	RTW_PRINT_SEL(m, "CLK REQ:	%s\n", (tmp8 & BIT4) ? "Enable" : "Disable");

	pci_read_config_byte(pci_data->ppcidev, 0x70f, &tmp8);
	l1_idle = tmp8 & 0x38;
	RTW_PRINT_SEL(m, "ASPM L0s:	%s\n", (tmp8 & BIT7) ? "Enable" : "Disable");

	pci_read_config_byte(pci_data->ppcidev, 0x719, &tmp8);
	RTW_PRINT_SEL(m, "ASPM L1:	%s\n", (tmp8 & BIT3) ? "Enable" : "Disable");

	pci_read_config_byte(pci_data->ppcidev, 0x718, &tmp8);
	RTW_PRINT_SEL(m, "ASPM L1OFF:	%s\n", (tmp8 & BIT5) ? "Enable" : "Disable");

	RTW_PRINT_SEL(m, "********* MISC **********\n");
	RTW_PRINT_SEL(m, "ASPM L1 Idel Time: 0x%x\n", l1_idle>>3);
	RTW_PRINT_SEL(m, "*************************\n");

#ifdef CONFIG_PCI_DYNAMIC_ASPM
	RTW_PRINT_SEL(m, "Dynamic ASPM mode: %d (%s)\n", pcipriv->aspm_mode,
		      pcipriv->aspm_mode == ASPM_MODE_PERF ? "Perf" :
		      pcipriv->aspm_mode == ASPM_MODE_PS ? "PS" : "Und");
#endif

	return 0;
}

int proc_get_rx_ring(struct seq_file *m, void *v)
{

	return 0;
}

int proc_get_tx_ring(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	int i, j, k;
	unsigned long sp_flags;

	_rtw_spinlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);
	for (i = 0; i < PCI_MAX_TX_QUEUE_COUNT; i++) {
		struct rtw_tx_ring *tx_ring = &pxmitpriv->tx_ring[i];

		RTW_PRINT_SEL(m, "tx ring[%d] (%p)\n", i, tx_ring);
		RTW_PRINT_SEL(m, "  dma: 0x%08x\n", (int) tx_ring->dma);
		RTW_PRINT_SEL(m, "  idx: %d\n", tx_ring->idx);
		RTW_PRINT_SEL(m, "  entries: %d\n", tx_ring->entries);
		/*		RTW_PRINT_SEL(m, "  queue: %d\n", tx_ring->queue); */
		RTW_PRINT_SEL(m, "  qlen: %d\n", tx_ring->qlen);

		for (j = 0; j < pxmitpriv->txringcount[i]; j++) {
#ifdef CONFIG_TRX_BD_ARCH
			struct tx_buf_desc *entry = &tx_ring->buf_desc[j];
			RTW_PRINT_SEL(m, "  buf_desc[%03d]: %p\n", j, entry);
#else
			struct tx_desc *entry = &tx_ring->desc[j];
			RTW_PRINT_SEL(m, "  desc[%03d]: %p\n", j, entry);
#endif

			for (k = 0; k < sizeof(*entry) / 4; k++) {
				if ((k % 4) == 0)
					RTW_PRINT_SEL(m, "  0x%03x", k);

				RTW_PRINT_SEL(m, " 0x%08x ", ((int *) entry)[k]);

				if ((k % 4) == 3)
					RTW_PRINT_SEL(m, "\n");
			}
		}
	}
	_rtw_spinunlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);

	return 0;
}

#ifdef DBG_TXBD_DESC_DUMP
int proc_get_tx_ring_ext(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *) rtw_netdev_priv(dev);
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct rtw_tx_desc_backup *pbuf;
	int i, j, k, idx;
	unsigned long sp_flags;

	RTW_PRINT_SEL(m, "<<<< tx ring ext dump settings >>>>\n");
	RTW_PRINT_SEL(m, " - backup frame num: %d\n", TX_BAK_FRMAE_CNT);
	RTW_PRINT_SEL(m, " - backup max. desc size: %d bytes\n", TX_BAK_DESC_LEN);
	RTW_PRINT_SEL(m, " - backup data size: %d bytes\n\n", TX_BAK_DATA_LEN);

	if (!pxmitpriv->dump_txbd_desc) {
		RTW_PRINT_SEL(m, "Dump function is disabled.\n");
		return 0;
	}

	_rtw_spinlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);
	for (i = 0; i < HW_QUEUE_ENTRY; i++) {
		struct rtw_tx_ring *tx_ring = &pxmitpriv->tx_ring[i];

		idx = rtw_get_tx_desc_backup(padapter, i, &pbuf);

		RTW_PRINT_SEL(m, "Tx ring[%d]", i);
		switch (i) {
		case 0:
			RTW_PRINT_SEL(m, " (VO)\n");
			break;
		case 1:
			RTW_PRINT_SEL(m, " (VI)\n");
			break;
		case 2:
			RTW_PRINT_SEL(m, " (BE)\n");
			break;
		case 3:
			RTW_PRINT_SEL(m, " (BK)\n");
			break;
		case 4:
			RTW_PRINT_SEL(m, " (BCN)\n");
			break;
		case 5:
			RTW_PRINT_SEL(m, " (MGT)\n");
			break;
		case 6:
			RTW_PRINT_SEL(m, " (HIGH)\n");
			break;
		case 7:
			RTW_PRINT_SEL(m, " (TXCMD)\n");
			break;
		default:
			RTW_PRINT_SEL(m, " (?)\n");
			break;
		}

		RTW_PRINT_SEL(m, "  Entries: %d\n", TX_BAK_FRMAE_CNT);
		RTW_PRINT_SEL(m, "  Last idx: %d\n", idx);

		for (j = 0; j < TX_BAK_FRMAE_CNT; j++) {
			RTW_PRINT_SEL(m, "  desc[%03d]:\n", j);

			for (k = 0; k < (pbuf->tx_desc_size) / 4; k++) {
				if ((k % 4) == 0)
					RTW_PRINT_SEL(m, "  0x%03x", k);

				RTW_PRINT_SEL(m, " 0x%08x ", ((int *)pbuf->tx_bak_desc)[k]);

				if ((k % 4) == 3)
					RTW_PRINT_SEL(m, "\n");
			}

#if 1 /* data dump */
			if (pbuf->tx_desc_size) {
				RTW_PRINT_SEL(m, "  data[%03d]:\n", j);

				for (k = 0; k < (TX_BAK_DATA_LEN) / 4; k++) {
					if ((k % 4) == 0)
						RTW_PRINT_SEL(m, "  0x%03x", k);

					RTW_PRINT_SEL(m, " 0x%08x ", ((int *)pbuf->tx_bak_data_hdr)[k]);

					if ((k % 4) == 3)
						RTW_PRINT_SEL(m, "\n");
				}
				RTW_PRINT_SEL(m, "\n");
			}
#endif

			RTW_PRINT_SEL(m, "  R/W pointer: %d/%d\n", pbuf->tx_bak_rp, pbuf->tx_bak_wp);

			pbuf = pbuf + 1;
		}
		RTW_PRINT_SEL(m, "\n");
	}
	_rtw_spinunlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);

	return 0;
}

ssize_t proc_set_tx_ring_ext(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	char tmp[32];
	u32 reset = 0;
	u32 dump = 0;
	unsigned long sp_flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%u %u", &dump, &reset);

		if (num != 2) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}

		_rtw_spinlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);
		pxmitpriv->dump_txbd_desc = (BOOLEAN) dump;

		if (reset == 1)
			rtw_tx_desc_backup_reset();

		_rtw_spinunlock_irq(&dvobj_to_pci(pdvobjpriv)->irq_th_lock, &sp_flags);

	}

	return count;
}

#endif

#endif

#ifdef CONFIG_WOWLAN
int proc_get_wow_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *registry_pair = &padapter->registrypriv;

	RTW_PRINT_SEL(m, "wow - %s\n", (registry_pair->wowlan_enable)? "enable" : "disable");
	return 0;
}

ssize_t proc_set_wow_enable(struct file *file, const char __user *buffer,
			    size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *registry_pair = &padapter->registrypriv;
	char tmp[8];
	int num = 0;
	int mode = 0;

	if (count < 1) 
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) 
		num = sscanf(tmp, "%d", &mode);
	else 
		return -EFAULT;

	if (num != 1) {
		RTW_ERR("%s: %s - invalid parameter!\n", __func__, tmp);
		return -EINVAL;
	}

	if (mode == 1) {
		RTW_PRINT("%s: wowlan - enable\n", __func__);
	} else if (mode == 0) {
		RTW_PRINT("%s: wowlan - disable\n", __func__);
	} else {
		RTW_ERR("%s: %s - invalid parameter!, mode=%d\n",
			__func__, tmp, mode);
		return -EINVAL;
	}

	registry_pair->wowlan_enable = mode;

	return count;
}

int proc_get_pattern_info(struct seq_file *m, void *v)
{
// TODO(jw.hou): Get pattern info from PHL.
#if 0
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	u8 val8;
	char str_1[128];
	char *p_str;
	int i = 0 , j = 0, k = 0;
	int len = 0, max_len = 0, total = 0;

	p_str = str_1;
	max_len = sizeof(str_1);

	total = pwrpriv->wowlan_pattern_idx;

	rtw_set_default_pattern(padapter);

	/*show pattern*/
	RTW_PRINT_SEL(m, "\n======[Pattern Info.]======\n");
	RTW_PRINT_SEL(m, "pattern number: %d\n", total);
	RTW_PRINT_SEL(m, "support default patterns: %c\n",
		      (pwrpriv->default_patterns_en) ? 'Y' : 'N');

	for (k = 0; k < total ; k++) {
		RTW_PRINT_SEL(m, "\npattern idx: %d\n", k);
		RTW_PRINT_SEL(m, "pattern content:\n");

		p_str = str_1;
		max_len = sizeof(str_1);
		for (i = 0 ; i < MAX_WKFM_PATTERN_SIZE / 8 ; i++) {
			_rtw_memset(p_str, 0, max_len);
			len = 0;
			for (j = 0 ; j < 8 ; j++) {
				val8 = pwrpriv->patterns[k].content[i * 8 + j];
				len += snprintf(p_str + len, max_len - len,
						"%02x ", val8);
			}
			RTW_PRINT_SEL(m, "%s\n", p_str);
		}
		RTW_PRINT_SEL(m, "\npattern mask:\n");
		for (i = 0 ; i < MAX_WKFM_SIZE / 8 ; i++) {
			_rtw_memset(p_str, 0, max_len);
			len = 0;
			for (j = 0 ; j < 8 ; j++) {
				val8 = pwrpriv->patterns[k].mask[i * 8 + j];
				len += snprintf(p_str + len, max_len - len,
						"%02x ", val8);
			}
			RTW_PRINT_SEL(m, "%s\n", p_str);
		}

		RTW_PRINT_SEL(m, "\npriv_pattern_len:\n");
		RTW_PRINT_SEL(m, "pattern_len: %d\n", pwrpriv->patterns[k].len);
		RTW_PRINT_SEL(m, "*****************\n");
	}
#endif

	return 0;
}

ssize_t proc_set_pattern_info(struct file *file, const char __user *buffer,
			      size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv  *registry_par = &adapter->registrypriv;
	struct rtw_wowcam_upd_info wowcam_info = {0};
	u8 tmp[MAX_WKFM_PATTERN_STR_LEN + 1] = {0};

	if (!(registry_par->wakeup_event & BIT(3))) {
		RTW_INFO("%s: customized pattern disabled, wakeup_event: %#2x\n",
			 __func__, registry_par->wakeup_event);
		return -EOPNOTSUPP;
	}

	if (count < 1)
		return -EINVAL;

	if (count >= sizeof(tmp)) {
		RTW_ERR("%s: pattern string is too long, count=%zu\n",
			__func__, count);
		return -EINVAL;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (strncmp(tmp, "clean", 5) == 0) {
			rtw_wow_pattern_clean(adapter, RTW_CUSTOMIZED_PATTERN);
		} else {
			if (rtw_wowlan_parser_pattern_cmd(tmp,
							  wowcam_info.ptrn,
							  &wowcam_info.ptrn_len,
							  wowcam_info.mask)) {
				if (_FAIL == rtw_wow_pattern_set(adapter,
								 &wowcam_info,
								 RTW_CUSTOMIZED_PATTERN))
					return -EFAULT;
			} else {
				return -EINVAL;
			}
		}
	} else {
		rtw_warn_on(1);
		return -EFAULT;
	}

	return count;
}

int proc_get_wakeup_event(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv  *registry_par = &padapter->registrypriv;

	RTW_PRINT_SEL(m, "wakeup event: %#02x\n", registry_par->wakeup_event);
	return 0;
}

ssize_t proc_set_wakeup_event(struct file *file, const char __user *buffer,
			      size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct registry_priv  *registry_par = &padapter->registrypriv;
	u32 wakeup_event = 0;

	u8 tmp[8] = {0};
	int num = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count))
		num = sscanf(tmp, "%u", &wakeup_event);
	else
		return -EFAULT;

	if (num == 1 && wakeup_event <= 0x1f) {
		registry_par->wakeup_event = wakeup_event;

		if (!(wakeup_event & BIT(1)))
			rtw_wow_pattern_clean(padapter, RTW_DEFAULT_PATTERN);

		if (!(wakeup_event & BIT(3)))
			rtw_wow_pattern_clean(padapter, RTW_CUSTOMIZED_PATTERN);

		RTW_INFO("%s: wakeup_event: %#2x\n",
			 __func__, registry_par->wakeup_event);
	} else {
		return -EINVAL;
	}

	return count;
}

int proc_get_wakeup_reason(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);

	RTW_PRINT_SEL(m, "last wake reason: %#02x\n", wowpriv->wow_wake_reason);

	return 0;
}

int proc_get_wake_indication(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
#ifdef RTW_DETECT_HANG
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobjpriv->drv_dbg;
	struct hang_info *phang_info = &pdbgpriv->dbg_hang_info;
	struct fw_hang_info *pfw_hang_info = &phang_info->dbg_fw_hang_info;
	struct rxff_hang_info *prxff_hang_info = &phang_info->dbg_rxff_hang_info;
#endif /* RTW_DETECT_HANG */
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

#ifdef RTW_DETECT_HANG
	if (pfw_hang_info->dbg_is_fw_hang ||
	    pfw_hang_info->dbg_is_fw_gone ||
	    prxff_hang_info->dbg_is_rxff_hang)
		RTW_PRINT_SEL(m, "wake indication: -1\n");
	else
#endif /* RTW_DETECT_HANG */
		RTW_PRINT_SEL(m, "wake indication: %#02x\n", !pwrpriv->bInSuspend);

	return 0;
}

#ifdef CONFIG_GPIO_WAKEUP
int proc_get_wowlan_gpio_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct rtw_wow_gpio_info *wow_gpio = &wowpriv->wow_gpio;
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;

	RTW_PRINT_SEL(m, "wakeup_gpio_idx: %d\n", WAKEUP_GPIO_IDX);
	RTW_PRINT_SEL(m, "high_active: %d\n", d2h_gpio_info->gpio_active);

	return 0;
}

ssize_t proc_set_wowlan_gpio_info(struct file *file, const char __user *buffer,
				  size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	void *phl = GET_PHL_INFO(dvobj);
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct rtw_wow_gpio_info *wow_gpio = &wowpriv->wow_gpio;
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;
	char tmp[32] = {0};
	int num = 0;
	u32 is_high_active = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num = sscanf(tmp, "%u", &is_high_active);

		if (num != 1) {
			RTW_INFO("Invalid format\n");
			return count;
		}

		wow_gpio->dev2hst_high = is_high_active == 0 ? 1 : 0;
		d2h_gpio_info->gpio_active = is_high_active;

		status = rtw_phl_cfg_wow_sw_gpio_ctrl(phl, wow_gpio);

		RTW_INFO("set %s %d\n", "gpio_high_active",
			 d2h_gpio_info->gpio_active);
		RTW_INFO("%s: set GPIO_%d %d as default. status=%d\n",
			 __func__, WAKEUP_GPIO_IDX, wow_gpio->dev2hst_high, status);
	}

	return count;
}

ssize_t proc_set_wow_gpio_duration(struct file *file, const char __user *buffer,
				size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct rtw_wow_gpio_info *wow_gpio = &wowpriv->wow_gpio;
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;
	u32 wow_cmn_gpio_dur;
	u32 wow_custom_gpio_dur;
	u8 tmp[8] = {0};
	int ret = 0, num = 0;
	u8 index = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count))
		num = sscanf(tmp, "%u %u", &wow_cmn_gpio_dur, &wow_custom_gpio_dur);
	else
		return -EFAULT;

	if (num == 2 && wow_cmn_gpio_dur <= 127 && wow_custom_gpio_dur <= 127) {
		d2h_gpio_info->gpio_pulse_dura = wow_cmn_gpio_dur;
		d2h_gpio_info->gpio_pulse_period = wow_cmn_gpio_dur * 2;
		d2h_gpio_info->rsn_a_pulse_duration = wow_custom_gpio_dur;
		d2h_gpio_info->rsn_a_pulse_period = wow_custom_gpio_dur * 2;

		RTW_INFO("%s: wow_cmn_gpio_dur: %u, wow_custom_gpio_dur: %u\n",
			 __func__, d2h_gpio_info->gpio_pulse_dura,
			 d2h_gpio_info->rsn_a_pulse_duration);
	} else {
		return -EINVAL;
	}

	return count;
}

int proc_get_wow_gpio_duration(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wow_priv *wowpriv = adapter_to_wowlan(padapter);
	struct rtw_wow_gpio_info *wow_gpio = &wowpriv->wow_gpio;
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;

	RTW_PRINT_SEL(m, "wow_cmn_gpio_dur = %u, wow_custom_gpio_dur = %u\n"
		"wow_cmn_gpio_period = %u, wow_custom_gpio_period = %u\n",
		d2h_gpio_info->gpio_pulse_dura,
		d2h_gpio_info->rsn_a_pulse_duration,
		d2h_gpio_info->gpio_pulse_period,
		d2h_gpio_info->rsn_a_pulse_period);
	return 0;
}
#endif /* CONFIG_GPIO_WAKEUP */
#endif /*CONFIG_WOWLAN*/

#ifdef CONFIG_P2P_WOWLAN
int proc_get_p2p_wowlan_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	struct p2p_wowlan_info	 peerinfo = pwdinfo->p2p_wow_info;
	if (_TRUE == peerinfo.is_trigger) {
		RTW_PRINT_SEL(m, "is_trigger: TRUE\n");
		switch (peerinfo.wowlan_recv_frame_type) {
		case P2P_WOWLAN_RECV_NEGO_REQ:
			RTW_PRINT_SEL(m, "Frame Type: Nego Request\n");
			break;
		case P2P_WOWLAN_RECV_INVITE_REQ:
			RTW_PRINT_SEL(m, "Frame Type: Invitation Request\n");
			break;
		case P2P_WOWLAN_RECV_PROVISION_REQ:
			RTW_PRINT_SEL(m, "Frame Type: Provision Request\n");
			break;
		default:
			break;
		}
		RTW_PRINT_SEL(m, "Peer Addr: "MAC_FMT"\n", MAC_ARG(peerinfo.wowlan_peer_addr));
		RTW_PRINT_SEL(m, "Peer WPS Config: %x\n", peerinfo.wowlan_peer_wpsconfig);
		RTW_PRINT_SEL(m, "Persistent Group: %d\n", peerinfo.wowlan_peer_is_persistent);
		RTW_PRINT_SEL(m, "Intivation Type: %d\n", peerinfo.wowlan_peer_invitation_type);
	} else
		RTW_PRINT_SEL(m, "is_trigger: False\n");
	return 0;
}
#endif /* CONFIG_P2P_WOWLAN */

#ifdef CONFIG_POWER_SAVE
enum ps_mode {
	PS_MODE_NONE,
	PS_MODE_LPS,
	PS_MODE_IPS
};

ssize_t proc_set_ps_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	struct _ADAPTER *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct rtw_phl_com_t *phl_com = dvobj->phl_com;
	struct rtw_ps_cap_t *ps_cap_p = &phl_com->dev_cap.ps_cap;
	struct registry_priv  *registry_par = &adapter->registrypriv;
	char tmp[32];
	int num = 0, ps_mode = 0, ps_cap = 0;
	u8 lps_cap = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%d %d", &ps_mode, &ps_cap);

	if (num > 2) {
		RTW_ERR("%s: invalid parameter!\n", __FUNCTION__);
		goto exit;
	}

	if (ps_mode == 1) {
#ifdef CONFIG_RTW_LPS
		switch (ps_cap) {
			case 0:
				if (ps_cap_p->lps_en == PS_OP_MODE_DISABLED)
					goto exit;
				break;
			case 1:
				lps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF;
				break;
			case 2:
				lps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED;
				break;
			case 3:
				lps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
				break;
			default:
				goto exit;
		}

		if (ps_cap == 0) {
			rtw_phl_dbg_ps_op_mode(GET_PHL_INFO(dvobj), HW_BAND_0, PS_MODE_LPS, PS_OP_MODE_DISABLED);
		} else {
			if (ps_cap_p->lps_cap != lps_cap) {
				if (ps_cap_p->lps_en != registry_par->lps_mode)
					rtw_phl_dbg_ps_op_mode(GET_PHL_INFO(dvobj), HW_BAND_0, PS_MODE_LPS, registry_par->lps_mode);
				rtw_phl_dbg_ps_cap(GET_PHL_INFO(dvobj), HW_BAND_0, PS_MODE_LPS, lps_cap);
			}
		}
#endif /* CONFIG_RTW_LPS */
	} else if (ps_mode == 2) {
#ifdef CONFIG_RTW_IPS
		if (ps_cap_p->ips_en != ps_cap) {
			if (ps_cap != 0)
				ps_cap = registry_par->ips_mode;

			rtw_phl_dbg_ps_op_mode(GET_PHL_INFO(dvobj), HW_BAND_0, PS_MODE_IPS, ps_cap);
		}
#endif /* #ifdef CONFIG_RTW_IPS */
	}
	else {
		RTW_ERR("%s: invalid parameter, mode = %d!\n", __FUNCTION__, ps_mode);
	}
exit:
	return count;
}
#endif /* CONFIG_POWER_SAVE */

#ifdef CONFIG_TDLS
int proc_get_tdls_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "TDLS is %s !\n", (rtw_is_tdls_enabled(padapter) == _TRUE) ? "enabled" : "disabled");

	return 0;
}

ssize_t proc_set_tdls_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 en_tdls = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &en_tdls);

		if (num == 1 && pregpriv) {
			if (en_tdls > 0)
				rtw_enable_tdls_func(padapter);
			else
				rtw_disable_tdls_func(padapter, _TRUE);
		}
	}

	return count;
}

static int proc_tdls_display_tdls_function_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;
	u8 SpaceBtwnItemAndValueTmp = 0;
	BOOLEAN FirstMatchFound = _FALSE;
	int j = 0;

	RTW_PRINT_SEL(m, "============[TDLS Function Info]============\n");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Enable", (rtw_is_tdls_enabled(padapter) == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Driver Setup", (ptdlsinfo->driver_setup == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Prohibited", (ptdlsinfo->ap_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Channel Switch Prohibited", (ptdlsinfo->ch_switch_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Link Established", (ptdlsinfo->link_established == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %d/%d\n", SpaceBtwnItemAndValue, "TDLS STA Num (Linked/Allowed)", ptdlsinfo->sta_cnt, MAX_ALLOWED_TDLS_STA_NUM);
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Allowed STA Num Reached", (ptdlsinfo->sta_maximum == _TRUE) ? "_TRUE" : "_FALSE");

#ifdef CONFIG_TDLS_CH_SW
	RTW_PRINT_SEL(m, "%-*s =", SpaceBtwnItemAndValue, "TDLS CH SW State");
	if (ptdlsinfo->chsw_info.ch_sw_state == TDLS_STATE_NONE)
		RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_STATE_NONE");
	else {
		for (j = 0; j < 32; j++) {
			if (ptdlsinfo->chsw_info.ch_sw_state & BIT(j)) {
				if (FirstMatchFound ==  _FALSE) {
					SpaceBtwnItemAndValueTmp = 1;
					FirstMatchFound = _TRUE;
				} else
					SpaceBtwnItemAndValueTmp = SpaceBtwnItemAndValue + 3;
				switch (BIT(j)) {
				case TDLS_INITIATOR_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_INITIATOR_STATE");
					break;
				case TDLS_RESPONDER_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_RESPONDER_STATE");
					break;
				case TDLS_LINKED_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_LINKED_STATE");
					break;
				case TDLS_WAIT_PTR_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_WAIT_PTR_STATE");
					break;
				case TDLS_ALIVE_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_ALIVE_STATE");
					break;
				case TDLS_CH_SWITCH_ON_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_CH_SWITCH_ON_STATE");
					break;
				case TDLS_PEER_AT_OFF_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_PEER_AT_OFF_STATE");
					break;
				case TDLS_CH_SW_INITIATOR_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_CH_SW_INITIATOR_STATE");
					break;
				case TDLS_WAIT_CH_RSP_STATE:
					RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValue, " ", "TDLS_WAIT_CH_RSP_STATE");
					break;
				default:
					RTW_PRINT_SEL(m, "%-*sBIT(%d)\n", SpaceBtwnItemAndValueTmp, " ", j);
					break;
				}
			}
		}
	}

	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS CH SW On", (ATOMIC_READ(&ptdlsinfo->chsw_info.chsw_on) == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "TDLS CH SW Off-Channel Num", ptdlsinfo->chsw_info.off_ch_num);
	RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "TDLS CH SW Channel Offset", ptdlsinfo->chsw_info.ch_offset);
	RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "TDLS CH SW Current Time", ptdlsinfo->chsw_info.cur_time);
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS CH SW Delay Switch Back", (ptdlsinfo->chsw_info.delay_switch_back == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "TDLS CH SW Dump Back", ptdlsinfo->chsw_info.dump_stack);
#endif

	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Device Discovered", (ptdlsinfo->dev_discovered == _TRUE) ? "_TRUE" : "_FALSE");

	return 0;
}

static int proc_tdls_display_network_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct wlan_network *cur_network = &(pmlmepriv->cur_network);
	int i = 0;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;
	char wl_mode[16];

	/* Display the linked AP/GO info */
	RTW_PRINT_SEL(m, "============[Associated AP/GO Info]============\n");

	if ((padapter->mlmepriv.fw_state & WIFI_STATION_STATE) && (padapter->mlmepriv.fw_state & WIFI_ASOC_STATE)) {
		wireless_mode_to_str(pmlmeext->cur_wireless_mode, wl_mode);
		RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "BSSID", cur_network->network.Ssid.Ssid);
		RTW_PRINT_SEL(m, "%-*s = "MAC_FMT"\n", SpaceBtwnItemAndValue, "Mac Address", MAC_ARG(cur_network->network.MacAddress));
		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Wireless Mode");
		RTW_PRINT_SEL(m, "0x%x(%s)\n", pmlmeext->cur_wireless_mode, wl_mode);

		RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Privacy");
		switch (padapter->securitypriv.dot11PrivacyAlgrthm) {
		case _NO_PRIVACY_:
			RTW_PRINT_SEL(m, "%s\n", "NO PRIVACY");
			break;
		case _WEP40_:
			RTW_PRINT_SEL(m, "%s\n", "WEP 40");
			break;
		case _TKIP_:
			RTW_PRINT_SEL(m, "%s\n", "TKIP");
			break;
		case _TKIP_WTMIC_:
			RTW_PRINT_SEL(m, "%s\n", "TKIP WTMIC");
			break;
		case _AES_:
			RTW_PRINT_SEL(m, "%s\n", "AES");
			break;
		case _WEP104_:
			RTW_PRINT_SEL(m, "%s\n", "WEP 104");
			break;
#if 0 /* no this setting */
		case _WEP_WPA_MIXED_:
			RTW_PRINT_SEL(m, "%s\n", "WEP/WPA Mixed");
			break;
#endif
		case _SMS4_:
			RTW_PRINT_SEL(m, "%s\n", "SMS4");
			break;
#ifdef CONFIG_IEEE80211W
		case _BIP_CMAC_128_:
			RTW_PRINT_SEL(m, "%s\n", "BIP");
			break;
#endif /* CONFIG_IEEE80211W */
		}

		RTW_PRINT_SEL(m, "%-*s = %d\n", SpaceBtwnItemAndValue, "Channel", pmlmeext->chandef.chan);
		RTW_PRINT_SEL(m, "%-*s = %u\n", SpaceBtwnItemAndValue, "Channel Offset", pmlmeext->chandef.offset);
		RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "Bandwidth Mode", ch_width_str(pmlmeext->chandef.bw));

	} else
		RTW_PRINT_SEL(m, "No association with AP/GO exists!\n");

	return 0;
}

static int proc_tdls_display_tdls_sta_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	struct sta_info *psta;
	int i = 0, j = 0;
	_list	*plist, *phead;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;
	u8 SpaceBtwnItemAndValueTmp = 0;
	u8 NumOfTdlsStaToShow = 0;
	BOOLEAN FirstMatchFound = _FALSE;
	char wl_mode[16];

	/* Search for TDLS sta info to display */
	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if (psta->tdls_sta_state != TDLS_STATE_NONE) {
				/* We got one TDLS sta info to show */
				RTW_PRINT_SEL(m, "============[TDLS Peer STA Info: STA %d]============\n", ++NumOfTdlsStaToShow);
				RTW_PRINT_SEL(m, "%-*s = "MAC_FMT"\n", SpaceBtwnItemAndValue, "Mac Address", MAC_ARG(psta->phl_sta->mac_addr));
				RTW_PRINT_SEL(m, "%-*s =", SpaceBtwnItemAndValue, "TDLS STA State");
				SpaceBtwnItemAndValueTmp = 0;
				FirstMatchFound = _FALSE;
				for (j = 0; j < 32; j++) {
					if (psta->tdls_sta_state & BIT(j)) {
						if (FirstMatchFound ==  _FALSE) {
							SpaceBtwnItemAndValueTmp = 1;
							FirstMatchFound = _TRUE;
						} else
							SpaceBtwnItemAndValueTmp = SpaceBtwnItemAndValue + 3;
						switch (BIT(j)) {
						case TDLS_INITIATOR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_INITIATOR_STATE");
							break;
						case TDLS_RESPONDER_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_RESPONDER_STATE");
							break;
						case TDLS_LINKED_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_LINKED_STATE");
							break;
						case TDLS_WAIT_PTR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_WAIT_PTR_STATE");
							break;
						case TDLS_ALIVE_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_ALIVE_STATE");
							break;
						case TDLS_CH_SWITCH_ON_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_CH_SWITCH_ON_STATE");
							break;
						case TDLS_PEER_AT_OFF_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_PEER_AT_OFF_STATE");
							break;
						case TDLS_CH_SW_INITIATOR_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValueTmp, " ", "TDLS_CH_SW_INITIATOR_STATE");
							break;
						case TDLS_WAIT_CH_RSP_STATE:
							RTW_PRINT_SEL(m, "%-*s%s\n", SpaceBtwnItemAndValue, " ", "TDLS_WAIT_CH_RSP_STATE");
							break;
						default:
							RTW_PRINT_SEL(m, "%-*sBIT(%d)\n", SpaceBtwnItemAndValueTmp, " ", j);
							break;
						}
					}
				}

				wireless_mode_to_str(psta->phl_sta->wmode, wl_mode);
				RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Wireless Mode");
				RTW_PRINT_SEL(m, "0x%x(%s)\n", psta->phl_sta->wmode, wl_mode);

				RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "Bandwidth Mode", ch_width_str(psta->phl_sta->chandef.bw));

				RTW_PRINT_SEL(m, "%-*s = ", SpaceBtwnItemAndValue, "Privacy");
				switch (psta->dot118021XPrivacy) {
				case _NO_PRIVACY_:
					RTW_PRINT_SEL(m, "%s\n", "NO PRIVACY");
					break;
				case _WEP40_:
					RTW_PRINT_SEL(m, "%s\n", "WEP 40");
					break;
				case _TKIP_:
					RTW_PRINT_SEL(m, "%s\n", "TKIP");
					break;
				case _TKIP_WTMIC_:
					RTW_PRINT_SEL(m, "%s\n", "TKIP WTMIC");
					break;
				case _AES_:
					RTW_PRINT_SEL(m, "%s\n", "AES");
					break;
				case _WEP104_:
					RTW_PRINT_SEL(m, "%s\n", "WEP 104");
					break;
#if 0 /* no this setting */
				case _WEP_WPA_MIXED_:
					RTW_PRINT_SEL(m, "%s\n", "WEP/WPA Mixed");
					break;
#endif
				case _SMS4_:
					RTW_PRINT_SEL(m, "%s\n", "SMS4");
					break;
#ifdef CONFIG_IEEE80211W
				case _BIP_CMAC_128_:
					RTW_PRINT_SEL(m, "%s\n", "BIP");
					break;
#endif /* CONFIG_IEEE80211W */
				}

				RTW_PRINT_SEL(m, "%-*s = %d sec/%d sec\n", SpaceBtwnItemAndValue, "TPK Lifetime (Current/Expire)", psta->TPK_count, psta->TDLS_PeerKey_Lifetime);
				RTW_PRINT_SEL(m, "%-*s = %llu\n", SpaceBtwnItemAndValue, "Tx Packets Over Direct Link", psta->sta_stats.tx_pkts);
				RTW_PRINT_SEL(m, "%-*s = %llu\n", SpaceBtwnItemAndValue, "Rx Packets Over Direct Link", psta->sta_stats.rx_data_pkts);
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	if (NumOfTdlsStaToShow == 0) {
		RTW_PRINT_SEL(m, "============[TDLS Peer STA Info]============\n");
		RTW_PRINT_SEL(m, "No TDLS direct link exists!\n");
	}

	return 0;
}

int proc_get_tdls_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct wlan_network *cur_network = &(pmlmepriv->dev_cur_network);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	struct sta_info *psta;
	int i = 0, j = 0;
	_list	*plist, *phead;
	u8 SpaceBtwnItemAndValue = 41;
	u8 SpaceBtwnItemAndValueTmp = 0;
	u8 NumOfTdlsStaToShow = 0;
	BOOLEAN FirstMatchFound = _FALSE;

	if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_TDLS) == _FALSE) {
		RTW_PRINT_SEL(m, "No tdls info can be shown since hal doesn't support tdls\n");
		return 0;
	}

	proc_tdls_display_tdls_function_info(m);
	proc_tdls_display_network_info(m);
	proc_tdls_display_tdls_sta_info(m);

	return 0;
}
#endif

int proc_get_monitor(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLDL: [currentlty primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (MLME_IS_MONITOR(padapter)) {
		RTW_PRINT_SEL(m, "Monitor mode : Enable\n");
		RTW_PRINT_SEL(m, "Device type  : %u\n", dev->type);

		RTW_PRINT_SEL(m, "ch=%d, ch_offset=%d, bw=%d\n",
			rtw_get_oper_ch(padapter, padapter_link),
			rtw_get_oper_choffset(padapter, padapter_link),
			rtw_get_oper_bw(padapter, padapter_link));
	} else
		RTW_PRINT_SEL(m, "Monitor mode : Disable\n");

	return 0;
}

ssize_t proc_set_monitor(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u16 target_type;
	u8 target_ch, target_offset, target_bw;
	/* ToDo CONFIG_RTW_MLDL: [currentlty primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (count < 3) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = 0;

		num = sscanf(tmp, "type %hu", &target_type);
		if ((num == 1) &&
			((target_type != ARPHRD_IEEE80211) &&
			(target_type != ARPHRD_IEEE80211_RADIOTAP))) {
			dev->type = ARPHRD_IEEE80211_RADIOTAP;
			return count;
		}

		num = sscanf(tmp, "%hhu %hhu %hhu", &target_ch, &target_offset, &target_bw);
		if (num != 3) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}

		padapter_link->mlmeextpriv.chandef.chan = target_ch;
		set_channel_bwmode(padapter, padapter_link,
				target_ch, target_offset, target_bw, _TRUE);
	}

	return count;
}
#ifdef DBG_XMIT_BLOCK
int proc_get_xmit_block(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	dump_xmit_block(m, padapter);

	return 0;
}

ssize_t proc_set_xmit_block(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 xb_mode, xb_reason;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx %hhx", &xb_mode, &xb_reason);

		if (num != 2) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}

		if (xb_mode == 0)/*set*/
			rtw_set_xmit_block(padapter, xb_reason);
		else if (xb_mode == 1)/*clear*/
			rtw_clr_xmit_block(padapter, xb_reason);
		else
			RTW_INFO("invalid parameter!\n");
	}

	return count;
}
#endif

int proc_get_efuse_map(struct seq_file *m, void *v)
{
#if 0
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	HAL_DATA_TYPE *pHalData = GET_PHL_COM(adapter_to_dvobj(padapter));
	struct pwrctrl_priv *pwrctrlpriv  = adapter_to_pwrctl(padapter);
	PEFUSE_HAL pEfuseHal = &pHalData->EfuseHal;
	struct efuse_info *efuse = adapter_to_efuse(padapter);
	int i, j;
	u16 mapLen;

	EFUSE_GetEfuseDefinition(padapter, EFUSE_WIFI, TYPE_EFUSE_MAP_LEN, (void *)&mapLen, _FALSE);
	if (mapLen > EFUSE_MAX_MAP_LEN)
		mapLen = EFUSE_MAX_MAP_LEN;

	if (efuse->file_status == EFUSE_FILE_LOADED) {
		RTW_PRINT_SEL(m, "File eFuse Map loaded! file path:%s\nDriver eFuse Map From File\n", EFUSE_MAP_PATH);
		if (efuse->is_autoload_fail)
			RTW_PRINT_SEL(m, "File Autoload fail!!!\n");
	} else if (efuse->file_status ==  EFUSE_FILE_FAILED) {
		RTW_PRINT_SEL(m, "Open File eFuse Map Fail ! file path:%s\nDriver eFuse Map From Default\n", EFUSE_MAP_PATH);
		if (efuse->is_autoload_fail)
			RTW_PRINT_SEL(m, "HW Autoload fail!!!\n");
	} else {
		RTW_PRINT_SEL(m, "Driver eFuse Map From HW\n");
		if (efuse->is_autoload_fail)
			RTW_PRINT_SEL(m, "HW Autoload fail!!!\n");
	}
	for (i = 0; i < mapLen; i += 16) {
		RTW_PRINT_SEL(m, "0x%02x\t", i);
		for (j = 0; j < 8; j++)
			RTW_PRINT_SEL(m, "%02X ", efuse->data[i + j]);
		RTW_PRINT_SEL(m, "\t");
		for (; j < 16; j++)
			RTW_PRINT_SEL(m, "%02X ", efuse->data[i + j]);
		RTW_PRINT_SEL(m, "\n");
	}

	if (rtw_efuse_map_read(padapter, 0, mapLen, pEfuseHal->fakeEfuseInitMap) == _FAIL) {
		RTW_PRINT_SEL(m, "WARN - Read Realmap Failed\n");
		return 0;
	}

	RTW_PRINT_SEL(m, "\n");
	RTW_PRINT_SEL(m, "HW eFuse Map\n");
	for (i = 0; i < mapLen; i += 16) {
		RTW_PRINT_SEL(m, "0x%02x\t", i);
		for (j = 0; j < 8; j++)
			RTW_PRINT_SEL(m, "%02X ", pEfuseHal->fakeEfuseInitMap[i + j]);
		RTW_PRINT_SEL(m, "\t");
		for (; j < 16; j++)
			RTW_PRINT_SEL(m, "%02X ", pEfuseHal->fakeEfuseInitMap[i + j]);
		RTW_PRINT_SEL(m, "\n");
	}

#endif
	return 0;
}

ssize_t proc_set_efuse_map(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
#if 0
	char tmp[256] = {0};
	u32 addr, cnts;
	u8 efuse_data;

	int jj, kk;

	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrctrlpriv  = adapter_to_pwrctl(padapter);

	if (count < 3) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%x %d %x", &addr, &cnts, &efuse_data);

		if (num != 3) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}
	}
	if (rtw_efuse_map_write(padapter, addr, cnts, &efuse_data) == _FAIL)
		RTW_INFO("WARN - rtw_efuse_map_write error!!\n");
#endif
	return count;
}

#ifdef CONFIG_IEEE80211W
ssize_t proc_set_tx_sa_query(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* macid_ctl move to phl */
	/* struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj); */
	struct sta_info *psta;
	_list	*plist, *phead;
	char tmp[16];
	u8	mac_addr[NUM_STA][ETH_ALEN];
	u32 key_type;
	u8 index;
	void *phl = GET_PHL_INFO(dvobj);
	u16 macid_num = rtw_phl_get_macid_max_num(phl);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {

		int num = sscanf(tmp, "%x", &key_type);

		if (num !=  1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return count;
		}
		RTW_INFO("0: set sa query request , key_type=%d\n", key_type);
	}

	if (MLME_IS_STA(padapter)
	    && (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) && SEC_IS_BIP_KEY_INSTALLED(&padapter_link->securitypriv) == _TRUE) {
		RTW_INFO("STA:"MAC_FMT"\n", MAC_ARG(get_my_bssid(&(pmlmeinfo->network))));
		/* TX unicast sa_query to AP */
		issue_action_SA_Query(padapter, padapter_link,
				get_my_bssid(&(pmlmeinfo->network)), 0, 0, (u8)key_type);
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE && SEC_IS_BIP_KEY_INSTALLED(&padapter_link->securitypriv) == _TRUE) {
		/* TX unicast sa_query to every client STA */
		_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
		for (index = 0; index < NUM_STA; index++) {
			psta = NULL;

			phead = &(pstapriv->sta_hash[index]);
			plist = get_next(phead);

			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
				plist = get_next(plist);
				_rtw_memcpy(&mac_addr[psta->phl_sta->macid][0], psta->phl_sta->mac_addr, ETH_ALEN);
			}
		}
		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

		for (index = 0; index < macid_num && index < NUM_STA; index++) {
			if (rtw_phl_macid_is_used(phl, index) && !rtw_phl_macid_is_bmc(phl, index)) {
				if (!_rtw_memcmp(get_my_bssid(&(pmlmeinfo->network)), &mac_addr[index][0], ETH_ALEN)
				    && !IS_MCAST(&mac_addr[index][0])) {
					issue_action_SA_Query(padapter, padapter_link,
						&mac_addr[index][0], 0, 0, (u8)key_type);
					RTW_INFO("STA[%u]:"MAC_FMT"\n", index , MAC_ARG(&mac_addr[index][0]));
				}
			}
		}
	}

	return count;
}

int proc_get_tx_sa_query(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_deauth(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* macid_ctl move to phl */
	/* struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj); */
	struct sta_info *psta;
	_list	*plist, *phead;
	char tmp[16];
	u8	mac_addr[NUM_STA][ETH_ALEN];
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u32 key_type = 0;
	u8 index;
	void *phl = GET_PHL_INFO(dvobj);
	u16 macid_num = rtw_phl_get_macid_max_num(phl);


	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {

		int num = sscanf(tmp, "%x", &key_type);

		if (num !=  1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return count;
		}
		RTW_INFO("key_type=%d\n", key_type);
	}
	if (key_type > 4)
		return count;

	if (MLME_IS_STA(padapter)
	    && (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) {
		if (key_type == 3) /* key_type 3 only for AP mode */
			return count;
		/* TX unicast deauth to AP */
		issue_deauth_11w(padapter, get_my_bssid(&(pmlmeinfo->network)), 0, (u8)key_type);
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE) {
		u8 updated = _FALSE;

		if (key_type == 3)
			issue_deauth_11w(padapter, bc_addr, 0, IEEE80211W_RIGHT_KEY);

		/* TX unicast deauth to every client STA */
		_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
		for (index = 0; index < NUM_STA; index++) {
			psta = NULL;

			phead = &(pstapriv->sta_hash[index]);
			plist = get_next(phead);

			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
				plist = get_next(plist);
				_rtw_memcpy(&mac_addr[psta->phl_sta->macid][0], psta->phl_sta->mac_addr, ETH_ALEN);
			}
		}
		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

		for (index = 0; index < macid_num && index < NUM_STA; index++) {
			if (rtw_phl_macid_is_used(phl, index) && !rtw_phl_macid_is_bmc(phl, index)) {
				if (!_rtw_memcmp(get_my_bssid(&(pmlmeinfo->network)), &mac_addr[index][0], ETH_ALEN)) {
					if (key_type != 3)
						issue_deauth_11w(padapter, &mac_addr[index][0], 0, (u8)key_type);

					psta = rtw_get_stainfo(pstapriv, &mac_addr[index][0]);
					if (psta && key_type != IEEE80211W_WRONG_KEY && key_type != IEEE80211W_NO_KEY) {
						_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
						if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
							rtw_list_delete(&psta->asoc_list);
							pstapriv->asoc_list_cnt--;
							#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
							if (psta->tbtx_enable)
								pstapriv->tbtx_asoc_list_cnt--;
							#endif
							updated |= ap_free_sta(padapter, psta, _FALSE, WLAN_REASON_PREV_AUTH_NOT_VALID, _TRUE, _FALSE);

						}
						_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);
					}

					RTW_INFO("STA[%u]:"MAC_FMT"\n", index , MAC_ARG(&mac_addr[index][0]));
				}
			}
		}

		associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
	}

	return count;
}

int proc_get_tx_deauth(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_auth(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* macid_ctl move to phl */
	/*struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj); */
	struct sta_info *psta;
	_list	*plist, *phead;
	char tmp[16];
	u8	mac_addr[NUM_STA][ETH_ALEN];
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u32 tx_auth;
	u8 index;


	if (count > 2) {
		RTW_INFO("argument size is more than 2\n");
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {

		int num = sscanf(tmp, "%x", &tx_auth);

		if (num !=  1) {
			RTW_INFO("invalid read_reg parameter!\n");
			return count;
		}
		RTW_INFO("1: setnd auth, 2: send assoc request. tx_auth=%d\n", tx_auth);
	}

	if (MLME_IS_STA(padapter)
	    && (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) {
		if (tx_auth == 1) {
			/* TX unicast auth to AP */
			issue_auth(padapter, NULL, 0);
		} else if (tx_auth == 2) {
			/* TX unicast auth to AP */
			issue_assocreq(padapter);
		}
	}

	return count;
}

int proc_get_tx_auth(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}
#endif /* CONFIG_IEEE80211W */

int proc_get_ack_timeout(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 ack_timeout_val = 0;
	u8 ack_timeout_val_cck = 0;

	rtw_hal_get_hwreg(padapter, HW_VAR_ACKTO, &ack_timeout_val);
	rtw_hal_get_hwreg(padapter, HW_VAR_ACKTO_CCK, &ack_timeout_val_cck);

	RTW_PRINT_SEL(m, "Current CCK packet ACK Timeout = %d us (0x%x).\n", ack_timeout_val_cck, ack_timeout_val_cck);
	RTW_PRINT_SEL(m, "Current non-CCK packet ACK Timeout = %d us (0x%x).\n", ack_timeout_val, ack_timeout_val);
	return 0;
}

ssize_t proc_set_ack_timeout(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 tmp_v1 = 0, tmp_v2 = 0;
	u8 ack_to_ms = 0, ack_to_cck_ms = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u", &tmp_v1, &tmp_v2);

		if (num < 1) {
			RTW_INFO(FUNC_ADPT_FMT ": input parameters < 1\n", FUNC_ADPT_ARG(padapter));
			return -EINVAL;
		}

		if (tmp_v1) {
			ack_to_ms = (u8)tmp_v1;
			rtw_hal_set_hwreg(padapter, HW_VAR_ACKTO, &ack_to_ms);
		}

		if (tmp_v2) {
			ack_to_cck_ms = (u8)tmp_v2;
			rtw_hal_set_hwreg(padapter, HW_VAR_ACKTO_CCK, &ack_to_cck_ms);
		}
		/* This register sets the Ack time out value after Tx unicast packet. It is in units of us. */
		/* This register sets the Ack time out value after Tx unicast CCK packet. It is in units of us. */
		RTW_INFO("Set CCK packet ACK Timeout to %d us.\n", ack_to_cck_ms);
		RTW_INFO("Set non-CCK packet ACK Timeout to %d us.\n", ack_to_ms);
	}

	return count;
}

int proc_get_fw_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	void *phl = GET_PHL_INFO(adapter_to_dvobj(adapter));
	char fw_ver[20] = {0};
	enum rtw_fw_status fw_sts;

	fw_sts = rtw_phl_get_fw_status(phl);

	if (fw_sts == RTW_FW_STATUS_NOFW) {
		RTW_PRINT_SEL(m, "FW not ready\n");
	} else {
		rtw_phl_get_fw_ver(GET_PHL_INFO(adapter_to_dvobj(adapter)), fw_ver, 20);
		RTW_PRINT_SEL(m, "FW VER %s\n", fw_ver);
	}

	return 0;
}

ssize_t proc_set_fw_offload(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{

#if 0 /*GEORGIA_TODO_FIXIT*/
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	_adapter *pri_adapter = GET_PRIMARY_ADAPTER(adapter);
	HAL_DATA_TYPE *hal = GET_PHL_COM(adapter_to_dvobj(adapter));
	char tmp[32];
	u32 iqk_offload_enable = 0, ch_switch_offload_enable = 0;

	if (buffer == NULL) {
		RTW_INFO("input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO("input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO("input length is too large\n");
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d %d", &iqk_offload_enable, &ch_switch_offload_enable);

		if (num < 2) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}

		if (hal->RegIQKFWOffload != iqk_offload_enable) {
			hal->RegIQKFWOffload = iqk_offload_enable;
			rtw_run_in_thread_cmd(pri_adapter, ((void *)(rtw_hal_update_iqk_fw_offload_cap)), pri_adapter);
		}

		if (hal->ch_switch_offload != ch_switch_offload_enable)
			hal->ch_switch_offload = ch_switch_offload_enable;
	}

	return count;
#endif
	return 0;
}

int proc_get_fw_offload(struct seq_file *m, void *v)
{
#if 0 /*GEORGIA_TODO_FIXIT*/
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	HAL_DATA_TYPE *hal = GET_PHL_COM(adapter_to_dvobj(adapter));


	RTW_PRINT_SEL(m, "IQK FW offload:%s\n", hal->RegIQKFWOffload?"enable":"disable");
	RTW_PRINT_SEL(m, "Channel switch FW offload:%s\n", hal->ch_switch_offload?"enable":"disable");
#endif
	return 0;
}

#ifdef CONFIG_CTRL_TXSS_BY_TP
ssize_t proc_set_txss_tp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	char tmp[32];
	u32 enable = 0;
	u32 txss_tx_tp = 0;
	int txss_chk_cnt = 0;

	if (buffer == NULL) {
		RTW_INFO("input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO("input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO("input length is too large\n");
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u %d",
			&enable, &txss_tx_tp, &txss_chk_cnt);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}
		pmlmeext->txss_ctrl_en = enable;

		if (txss_tx_tp)
			pmlmeext->txss_tp_th = txss_tx_tp;
		if (txss_chk_cnt)
			pmlmeext->txss_tp_chk_cnt = txss_chk_cnt;

		RTW_INFO("%s txss_ctl_en :%s , txss_tp_th:%d, tp_chk_cnt:%d\n",
			__func__, pmlmeext->txss_tp_th ? "Y" : "N",
			pmlmeext->txss_tp_th, pmlmeext->txss_tp_chk_cnt);

	}

	return count;
}

int proc_get_txss_tp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	RTW_PRINT_SEL(m, "TXSS  Control - %s\n", pmlmeext->txss_ctrl_en ? "enable" : "disable");
	RTW_PRINT_SEL(m, "TXSS  Tx TP TH - %d\n", pmlmeext->txss_tp_th);
	RTW_PRINT_SEL(m, "TXSS  check cnt - %d\n", pmlmeext->txss_tp_chk_cnt);

	return 0;
}
#ifdef DBG_CTRL_TXSS
ssize_t proc_set_txss_ctrl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	char tmp[32];
	u32 tx_1ss = 0;

	if (buffer == NULL) {
		RTW_INFO("input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO("input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO("input length is too large\n");
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u",	&tx_1ss);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}

		pmlmeext->txss_ctrl_en = _FALSE;

		dbg_ctrl_txss(adapter, tx_1ss);

		RTW_INFO("%s set tx to  1ss :%s\n", __func__, tx_1ss ? "Y" : "N");
	}

	return count;
}

int proc_get_txss_ctrl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	RTW_PRINT_SEL(m, "TXSS  1ss - %s\n", pmlmeext->txss_1ss ? "Y" : "N");

	return 0;
}
#endif
#endif

#ifdef CONFIG_DBG_RF_CAL
int proc_get_iqk_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	return 0;
}

ssize_t proc_set_iqk(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 recovery, clear, segment;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d %d %d", &recovery, &clear, &segment);

		if (num != 3) {
			RTW_INFO("Invalid format\n");
			return count;
		}

		rtw_hal_iqk_test(padapter, recovery, clear, segment);
	}

	return count;

}

int proc_get_lck_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	return 0;
}

ssize_t proc_set_lck(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u32 trigger;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d", &trigger);

		if (num != 1) {
			RTW_INFO("Invalid format\n");
			return count;
		}

		rtw_hal_lck_test(padapter);
	}

	return count;
}
#endif /* CONFIG_DBG_RF_CAL */

#ifdef CONFIG_SUPPORT_STATIC_SMPS
ssize_t proc_set_smps(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	char tmp[32];
	u32 enable = 0;
	u32 smps_en, smps_tx_tp = 0, smps_rx_tp = 0;
	u32 smps_test = 0, smps_test_en = 0;

	if (buffer == NULL) {
		RTW_INFO("input buffer is NULL!\n");
		return -EFAULT;
	}

	if (count < 1) {
		RTW_INFO("input length is 0!\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		RTW_INFO("input length is too large\n");
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%u %u %u %u %u", &smps_en, &smps_tx_tp, &smps_rx_tp,
			&smps_test, &smps_test_en);

		if (num < 1) {
			RTW_INFO("input parameters < 1\n");
			return -EINVAL;
		}

		pmlmeext->ssmps_en = smps_en;
		if (smps_tx_tp) {
			pmlmeext->ssmps_tx_tp_th= smps_tx_tp;
			pmlmeext->ssmps_rx_tp_th= smps_tx_tp;
		}
		if (smps_rx_tp)
			pmlmeext->ssmps_rx_tp_th = smps_rx_tp;

		#ifdef DBG_STATIC_SMPS
		if (num > 3) {
			pmlmeext->ssmps_test = smps_test;
			pmlmeext->ssmps_test_en = smps_test_en;
		}
		#endif
		RTW_INFO("SM PS : %s tx_tp_th:%d, rx_tp_th:%d\n",
			(smps_en) ? "Enable" : "Disable",
			pmlmeext->ssmps_tx_tp_th,
			pmlmeext->ssmps_rx_tp_th);
		#ifdef DBG_STATIC_SMPS
		RTW_INFO("SM PS : %s ssmps_test_en:%d\n",
			(smps_test) ? "Enable" : "Disable",
			pmlmeext->ssmps_test_en);
		#endif
	}

	return count;
}

int proc_get_smps(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	RTW_PRINT_SEL(m, "Static SMPS %s\n", pmlmeext->ssmps_en ? "enable" : "disable");
	RTW_PRINT_SEL(m, "Tx TP TH %d\n", pmlmeext->ssmps_tx_tp_th);
	RTW_PRINT_SEL(m, "Rx TP TH %d\n", pmlmeext->ssmps_rx_tp_th);
	#ifdef DBG_STATIC_SMPS
	RTW_PRINT_SEL(m, "test %d, test_en:%d\n", pmlmeext->ssmps_test, pmlmeext->ssmps_test_en);
	#endif
	return 0;
}
#endif /*CONFIG_SUPPORT_STATIC_SMPS*/

#ifdef RTW_DETECT_HANG
ssize_t proc_set_hang_info(struct file *file, const char __user *buffer,
			   size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobjpriv->drv_dbg;
	struct hang_info *phang_info = &pdbgpriv->dbg_hang_info;
	char tmp[32] = {0};
	int level = 0;
	int stop = 0;
	int num = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num = sscanf(tmp, "%d %d", &level, &stop);
		if (num == 1) {
			RTW_INFO("level configuration is not available now\n");
		} else if (num == 2) {
			phang_info->is_stop = stop;
			RTW_INFO("level=%d, stop=%d\n", level,
				 phang_info->is_stop);
		} else {
			RTW_INFO("argument number is wrong\n");
			return -EFAULT;
		}
	}

	return count;
}

int proc_get_hang_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobjpriv->drv_dbg;
	struct hang_info *phang_info = &pdbgpriv->dbg_hang_info;
	struct fw_hang_info *pfw_hang_info = &phang_info->dbg_fw_hang_info;
	struct rxff_hang_info *prxff_hang_info = &phang_info->dbg_rxff_hang_info;
	u8 is_hang = _FALSE;
	u8 fw_gone = _FALSE;

	if (phang_info->is_stop) {
		RTW_PRINT_SEL(m, "This function is disabled\n");
		goto exit;
	}

	if (dev_is_surprise_removed(pdvobjpriv))
		is_hang = _TRUE;

	if (pfw_hang_info->dbg_is_fw_gone &&
	    rtw_hw_get_init_completed(pdvobjpriv) &&
	    !padapter->registrypriv.mp_mode)
		fw_gone = _TRUE;

	if (pfw_hang_info->dbg_is_fw_hang || is_hang || fw_gone ||
	    prxff_hang_info->dbg_is_rxff_hang) {
		RTW_PRINT_SEL(m, "1\n");

		if (pfw_hang_info->dbg_is_fw_hang)
			RTW_PRINT_SEL(m, "FW hang\n");
		if (prxff_hang_info->dbg_is_rxff_hang)
			RTW_PRINT_SEL(m, "RXFF hang\n");
		if (is_hang)
			RTW_PRINT_SEL(m, "surprise remove\n");
		if (fw_gone)
			RTW_PRINT_SEL(m, "FW gone\n");
	} else {
		RTW_PRINT_SEL(m, "0\n");
	}

	RTW_PRINT_SEL(m, "Enter cnt = %d\n", phang_info->enter_cnt);
exit:
	return 0;
}
#endif /* RTW_DETECT_HANG */

int proc_get_disconnect_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	char *disconnect_reason;
	char *illegal_beacon_reason;

	if (pmlmeinfo) {
		switch (pmlmeinfo->disconnect_code) {
		case DISCONNECTION_NOT_YET_OCCUR:
			disconnect_reason = "Disconnection has not yet occurred.";
			break;
		case DISCONNECTION_BY_SYSTEM_DUE_TO_HIGH_LAYER_COMMAND:
			disconnect_reason = "System requests to disconnect by command.";
			break;
		case DISCONNECTION_BY_SYSTEM_DUE_TO_NET_DEVICE_DOWN:
			disconnect_reason = "System makes net_device down to cause disconnection.";
			break;
		case DISCONNECTION_BY_SYSTEM_DUE_TO_SYSTEM_IN_SUSPEND:
			disconnect_reason = "System enters suspend state to cause disconnection.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_CONNECTION_EXIST:
			disconnect_reason = "The connection is exist so it triggers disconnection when joinbss.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_EACH_IFACE_CHBW_NOT_SYNC:
			disconnect_reason = "Multiple interfaces do not synchronize channel and bandwidth when joinbss.";
			break;
		/*
		case DISCONNECTION_BY_DRIVER_DUE_TO_DFS_DETECTION:
			disconnect_reason = "DFS Slave mechanism detects radar to cause disconnection.";
			break;
		*/
		case DISCONNECTION_BY_DRIVER_DUE_TO_IOCTL_DBG_PORT:
			disconnect_reason = "Using ioctl dbg port command to accomplish disconnection.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED:
			disconnect_reason = "AP's beacon content was changed to cause disconnection.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_KEEPALIVE_TIMEOUT:
			disconnect_reason = "Maybe AP disappears to trigger station keepalive timeout and connection break.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_LAYER2_ROAMING_TERMINATE:
			disconnect_reason = "Layer2 roaming terminated to trigger disconnection.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_JOINBSS_TIMEOUT:
			disconnect_reason = "Station joinbss process is timeout to trigger disconnection.";
			break;
		case DISCONNECTION_BY_FW_DUE_TO_FW_DECISION_IN_WOW_RESUME:
			disconnect_reason = "FW decides to disconnect in WOW resume flow.";
			break;
		case DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC_IN_WOW_RESUME:
			disconnect_reason = "Receiving Disassociation frame from AP to cause disconnection in WOW resume flow.";
			break;
		case DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH_IN_WOW_RESUME:
			disconnect_reason = "Receiving Deauth frame from AP to cause disconnection in WOW resume flow.";
			break;
		case DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH:
			disconnect_reason = "Receiving Deauth frame from AP to cause disconnection.";
			break;
		case DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC:
			disconnect_reason = "Receiving Disassociation frame from AP to cause disconnection.";
			break;
		/*
		case DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_CSA_NON_DFS:
			disconnect_reason = "Disconnection due to AP switched to unsupported channel.";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_CSA_DFS:
			disconnect_reason = "Disconnection due to AP switched to unsupported DFS channel.";
			break;
		*/
		case DISCONNECTION_BY_DRIVER_DUE_TO_RECEIVE_INVALID_CSA:
			disconnect_reason = "Disconnection due to received invalid CSA IE";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_JOIN_WRONG_CHANNEL:
			disconnect_reason = "Disconnection due to joined wrong channel";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_FT:
			disconnect_reason = "Disconnection due to FT";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_ROAMING:
			disconnect_reason = "Disconnection due to roaming";
			break;
		case DISCONNECTION_BY_DRIVER_DUE_TO_SA_QUERY_TIMEOUT:
			disconnect_reason = "Disconnection due to SA Query timeout";
			break;
		default:
			disconnect_reason = "Unspecified";
			break;
		}

		RTW_PRINT_SEL(m, "occurred disconnection time = %d\n", pmlmeinfo->disconnect_occurred_time);
		RTW_PRINT_SEL(m, "disconnect code = %d\n", pmlmeinfo->disconnect_code);
		RTW_PRINT_SEL(m, "last disconnect reason: %s\n", disconnect_reason);
		RTW_PRINT_SEL(m, "beacon code: %d\n", pmlmeinfo->illegal_beacon_code);
		if (pmlmeinfo->disconnect_code == DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED) {
			if(pmlmeinfo->illegal_beacon_code & SSID_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The SSID of beacon is changed.\n");
			if(pmlmeinfo->illegal_beacon_code & SSID_LENGTH_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The SSID length of beacon is changed.\n");
			if(pmlmeinfo->illegal_beacon_code & BEACON_CHANNEL_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The Beacon channel of beacon is changed.\n");
			if(pmlmeinfo->illegal_beacon_code & ENCRYPT_PROTOCOL_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The Encrypt protocol of beacon is changed.\n");
			if(pmlmeinfo->illegal_beacon_code & PAIRWISE_CIPHER_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The Pairwise cipheris of beacon changed.\n");
			if(pmlmeinfo->illegal_beacon_code & GROUP_CIPHER_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The Group cipher of beacon is changed.\n");
			if(pmlmeinfo->illegal_beacon_code & IS_8021X_CHANGED)
				RTW_PRINT_SEL(m, "illegal beacon reason: The 802.1x of beacon is changed.\n");
		} else {
			RTW_PRINT_SEL(m, "illegal beacon reason: NA.\n");
		}

		RTW_PRINT_SEL(m, "Wi-Fi reason code: %d\n",
			      pmlmeinfo->disconnect_code ? pmlmeinfo->wifi_reason_code : 0);
	}

	return 0;
}

ssize_t proc_set_disconnect_info(struct file *file, const char __user *buffer,
				 size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	char tmp[32];
	u8 clear;

	if (!pmlmeinfo)
		return -EPERM;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhd", &clear);
		if (num == 1 && clear == 0) {
			pmlmeinfo->disconnect_occurred_time = 0;
			pmlmeinfo->disconnect_code = DISCONNECTION_NOT_YET_OCCUR;
			pmlmeinfo->illegal_beacon_code = 0;
		}
	}

	return count;
}

int proc_get_chan(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct _ADAPTER_LINK *padapter_link;
	u8 lidx;

	for (lidx = 0; lidx < padapter->phl_role->rlink_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		RTW_PRINT_SEL(m, "ch=%d, ch_offset=%d, bw=%d\n",
				rtw_get_oper_ch(padapter, padapter_link),
				rtw_get_oper_choffset(padapter, padapter_link),
				rtw_get_oper_bw(padapter, padapter_link));

		RTW_PRINT_SEL(m, "PHL rlink %d ch=%d\n", lidx,
			rtw_phl_get_cur_ch(padapter->phl_role,
							padapter_link->wrlink));
	}
	return 0;
}

ssize_t proc_set_chan(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	u8 target_ch, target_offset, target_bw;
#ifdef DBG_CONFIG_CMD_DISP
	u8 cmd_type = 0xFF;
	u8 cmd_to = 0;
#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (count < 3) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = 0;
#ifdef DBG_CONFIG_CMD_DISP
		num = sscanf(tmp, "%hhu %hhu %hhu %hhu %hhu",
			&target_ch, &target_offset, &target_bw,
			&cmd_type, &cmd_to);
		if (num != 5) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}

		padapter->cmd_type = cmd_type;
		padapter->cmd_timeout = cmd_to;
#else
		num = sscanf(tmp, "%hhu %hhu %hhu",
			&target_ch, &target_offset, &target_bw);
		if (num != 3) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}
#endif
		set_channel_bwmode(padapter, padapter_link,
				target_ch, target_offset, target_bw, _FALSE);
	}

	return count;
}

int proc_get_mr_test(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	return 0;
}

ssize_t proc_set_mr_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 mode, param, param_1, param_2;
	bool show_caller;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (count < 2) {
		RTW_INFO("argument size is less than 3\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = 0;

		num = sscanf(tmp, "%hhu %hhu %hhu %hhu", &mode, &param, &param_1, &param_2);
		if (num < 2) {
			RTW_INFO("invalid write_reg parameter!\n");
			return count;
		}
#ifdef PHL_MR_PROC_CMD
		if (mode == 1) {
			show_caller = (param) ? _TRUE : _FALSE;
			rtw_phl_mr_dump_info(dvobj->phl, show_caller);
		}
		else if (mode == 2) {
			show_caller = (param) ? _TRUE : _FALSE;
			rtw_phl_mr_dump_band_ctl(dvobj->phl, show_caller);
		}
		else if (mode == 3) {
			if (param == 1)
				rtw_phl_mr_dump_mac_addr(dvobj->phl, padapter->phl_role);
			else if (param == 2)
				rtw_phl_mr_buddy_dump_mac_addr(dvobj->phl, padapter->phl_role);
			else if (param == 3)
				rtw_mi_dump_mac_addr(padapter);
		}
		else if (mode == 4) {
			show_caller = (param) ? _TRUE : _FALSE;
			rtw_phl_sta_dump_info(dvobj->phl, show_caller, padapter->phl_role, param_1);
		}
		else if (mode == 5 || mode == 6 || mode == 7 || mode == 8 || mode == 9) {
			u8 test_mode = 0;/*0-chk, 1-add, 2-del*/
			struct rtw_chan_def new_chdef = {0};
			struct rtw_mr_chctx_info mr_cc_info = {0};
			bool rst;

			if (mode == 5)
				test_mode = 0;
			else if (mode == 6)
				test_mode = 1;
			else if (mode == 7)
				test_mode = 2;
			else if (mode == 8)
				test_mode = 3;
			else if (mode == 9)
				test_mode = 4;
			new_chdef.chan = param;
			new_chdef.bw = param_1;
			new_chdef.offset = param_2;
			new_chdef.band = rtw_get_band_type(new_chdef.chan);

			rst = rtw_phl_chanctx_test(dvobj->phl, padapter->phl_role, padapter_link->wrlink,
						   test_mode, &new_chdef, &mr_cc_info);
			RTW_INFO("%s s_ch:%d,s_bw:%d,s_offset:%d\n",
				__func__, param, param_1, param_2);
		}
		else if (mode == 10) {
			u8 new_port = param;
			u8 org_port = padapter_link->wrlink->hw_port;

			rtw_phl_wifi_role_realloc_port(dvobj->phl, padapter->phl_role, padapter_link->wrlink, new_port);
			RTW_INFO("%s realloc port from %d to %d\n",
					__func__, org_port, new_port);
		}
		else if (mode == 11) {
			rtw_phl_get_addr_cam(dvobj->phl, param, NULL, 0);
		}
		else if (mode == 12) {
			struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
			struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
			struct sta_priv *pstapriv = &padapter->stapriv;
			struct sta_info *psta;

			psta = rtw_get_stainfo(pstapriv, get_link_bssid(pmlmepriv));
			if (psta) {
				if (param == 1)
					rtw_ssmps_enter(padapter, psta);
				else if (param == 0)
					rtw_ssmps_leave(padapter, psta);
			}
		}

#endif
	}

	return count;
}

int proc_get_deny_legacy(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "deny_legacy       = 0x%x\n", pregpriv->deny_legacy);

	if (pmlmeext)
		RTW_PRINT_SEL(m, "cur_wireless_mode = 0x%x\n", pmlmeext->cur_wireless_mode);

	return 0;
}

ssize_t proc_set_deny_legacy(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	u8 legacy;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu ", &legacy);

		if (num == 1 && pregpriv) {
			pregpriv->deny_legacy = legacy;
			RTW_INFO("deny_legacy=%d\n", legacy);
		}
	}

	return count;
}

#ifdef CONFIG_80211AX_HE
int proc_get_tx_ul_mu_disable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct rtw_he_actrl_om *cur_om_info = &(pmlmepriv->hepriv.om_info);

	RTW_PRINT_SEL(m, "OM ul_mu_disable = %s\n", cur_om_info->om_actrl_ele.ul_mu_disable ? "True" : "False");
	return 0;
}

ssize_t proc_set_tx_ul_mu_disable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_he_actrl_om om_info = {0};
	u8 om_mask = 0;
	char tmp[32];
	u8 ul_mu_disable;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu ", &ul_mu_disable);

		if (num == 1) {
			om_mask = om_mask | OM_UL_MU_DIS;
			/*om_info.actrl_om_normal_tx = _TRUE;*/
			/*om_info.actrl_om_normal_tx_cnt = 8;*/
			if (ul_mu_disable == _TRUE)
				om_info.om_actrl_ele.ul_mu_disable = _TRUE;
			else
				om_info.om_actrl_ele.ul_mu_disable = _FALSE;

#if 0
			/*om others control setting*/
			om_mask = om_mask | OM_RX_NSS;
			om_info.om_actrl_ele.rx_nss = 1;
			om_mask = om_mask | OM_CH_BW;
			om_info.om_actrl_ele.channel_width= 2;
			om_mask = om_mask | OM_TX_NSTS;
			om_info.om_actrl_ele.tx_nsts= 2;
			om_mask = om_mask | OM_ER_SU_DIS;
			om_info.om_actrl_ele.er_su_disable = _TRUE;
			om_mask = om_mask | OM_DL_MU_RR;
			om_info.om_actrl_ele.dl_mu_mimo_rr= _TRUE;
			om_mask = om_mask | OM_UL_MU_DATA_DIS;
			om_info.om_actrl_ele.ul_mu_data_disable= _TRUE;
#endif
			rtw_he_set_om_info(padapter, padapter_link, om_mask, &om_info);
			issue_qos_nulldata(padapter, padapter_link,
					NULL, 0, 0, 3, 10, _TRUE);
		}
	}

	return count;
}
#endif

#endif /* CONFIG_PROC_DEBUG */
#define RTW_BUFDUMP_BSIZE		16
#if 1
inline void RTW_BUF_DUMP_SEL(uint _loglevel, void *sel, u8 *_titlestring,
					bool _idx_show, const u8 *_hexdata, int _hexdatalen)
{
#ifdef CONFIG_RTW_DEBUG
	int __i;
	u8 *ptr = (u8 *)_hexdata;

	if (_loglevel <= rtw_drv_log_level) {
		if (_titlestring) {
			if (sel == RTW_DBGDUMP)
				RTW_PRINT("");
			_RTW_PRINT_SEL(sel, "%s", _titlestring);
			if (_hexdatalen >= RTW_BUFDUMP_BSIZE)
				_RTW_PRINT_SEL(sel, "\n");
		}

		for (__i = 0; __i < _hexdatalen; __i++) {
			if (((__i % RTW_BUFDUMP_BSIZE) == 0) && (_hexdatalen >= RTW_BUFDUMP_BSIZE)) {
				if (sel == RTW_DBGDUMP)
					RTW_PRINT("");
				if (_idx_show)
					_RTW_PRINT_SEL(sel, "0x%03X: ", __i);
			}
			_RTW_PRINT_SEL(sel, "%02X%s", ptr[__i], (((__i + 1) % 4) == 0) ? "  " : " ");
			if ((__i + 1 < _hexdatalen) && ((__i + 1) % RTW_BUFDUMP_BSIZE) == 0)
				_RTW_PRINT_SEL(sel, "\n");
		}
		_RTW_PRINT_SEL(sel, "\n");
	}
#endif
}

inline void RTW_BUF_DUMP_SEL_ALWAYS(void *sel, u8 *_titlestring,
					bool _idx_show, const u8 *_hexdata, int _hexdatalen)
{
	int __i;
	u8 *ptr = (u8 *)_hexdata;

	if (_titlestring) {
		if (sel == RTW_DBGDUMP)
			RTW_PRINT("");
		_RTW_PRINT_SEL(sel, "%s", _titlestring);
		if (_hexdatalen >= RTW_BUFDUMP_BSIZE)
			_RTW_PRINT_SEL(sel, "\n");
	}

	for (__i = 0; __i < _hexdatalen; __i++) {
		if (((__i % RTW_BUFDUMP_BSIZE) == 0) && (_hexdatalen >= RTW_BUFDUMP_BSIZE)) {
			if (sel == RTW_DBGDUMP)
				RTW_PRINT("");
			if (_idx_show)
				_RTW_PRINT_SEL(sel, "0x%03X: ", __i);
		}
		_RTW_PRINT_SEL(sel, "%02X%s", ptr[__i], (((__i + 1) % 4) == 0) ? "  " : " ");
		if ((__i + 1 < _hexdatalen) && ((__i + 1) % RTW_BUFDUMP_BSIZE) == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}
	_RTW_PRINT_SEL(sel, "\n");
}
#else
inline void _RTW_STR_DUMP_SEL(void *sel, char *str_out)
{
	if (sel == RTW_DBGDUMP)
		_dbgdump("%s\n", str_out);
	#if defined(_seqdump)
	else
		_seqdump(sel, "%s\n", str_out);
	#endif /*_seqdump*/
}
inline void RTW_BUF_DUMP_SEL(uint _loglevel, void *sel, u8 *_titlestring,
					bool _idx_show, u8 *_hexdata, int _hexdatalen)
{
	int __i, len;
	int __j, idx;
	int block_num, remain_byte;
	char str_out[128] = {'\0'};
	char str_val[32] = {'\0'};
	char *p = NULL;
	u8 *ptr = (u8 *)_hexdata;

	if (_loglevel <= rtw_drv_log_level) {
		/*dump title*/
		p = &str_out[0];
		if (_titlestring) {
			if (sel == RTW_DBGDUMP) {
				len = snprintf(str_val, sizeof(str_val), "%s", DRIVER_PREFIX);
				strncpy(p, str_val, len);
				p += len;
			}
			len = snprintf(str_val, sizeof(str_val), "%s", _titlestring);
			strncpy(p, str_val, len);
			p += len;
		}
		if (p != &str_out[0]) {
			_RTW_STR_DUMP_SEL(sel, str_out);
			_rtw_memset(&str_out, '\0', sizeof(str_out));
		}

		/*dump buffer*/
		block_num = _hexdatalen / RTW_BUFDUMP_BSIZE;
		remain_byte = _hexdatalen % RTW_BUFDUMP_BSIZE;
		for (__i = 0; __i < block_num; __i++) {
			p = &str_out[0];
			if (sel == RTW_DBGDUMP) {
				len = snprintf(str_val, sizeof(str_val), "%s", DRIVER_PREFIX);
				strncpy(p, str_val, len);
				p += len;
			}
			if (_idx_show) {
				len = snprintf(str_val, sizeof(str_val), "0x%03X: ", __i * RTW_BUFDUMP_BSIZE);
				strncpy(p, str_val, len);
				p += len;
			}
			for (__j =0; __j < RTW_BUFDUMP_BSIZE; __j++) {
				idx = __i * RTW_BUFDUMP_BSIZE + __j;
				len = snprintf(str_val, sizeof(str_val), "%02X%s", ptr[idx], (((__j + 1) % 4) == 0) ? "  " : " ");
				strncpy(p, str_val, len);
				p += len;
			}
			_RTW_STR_DUMP_SEL(sel, str_out);
			_rtw_memset(&str_out, '\0', sizeof(str_out));
		}

		p = &str_out[0];
		if ((sel == RTW_DBGDUMP) && remain_byte) {
			len = snprintf(str_val, sizeof(str_val), "%s", DRIVER_PREFIX);
			strncpy(p, str_val, len);
			p += len;
		}
		if (_idx_show && remain_byte) {
			len = snprintf(str_val, sizeof(str_val), "0x%03X: ", block_num * RTW_BUFDUMP_BSIZE);
			strncpy(p, str_val, len);
			p += len;
		}
		for (__i = 0; __i < remain_byte; __i++) {
			idx = block_num * RTW_BUFDUMP_BSIZE + __i;
			len = snprintf(str_val, sizeof(str_val), "%02X%s", ptr[idx], (((__i + 1) % 4) == 0) ? "  " : " ");
			strncpy(p, str_val, len);
			p += len;
		}
		_RTW_STR_DUMP_SEL(sel, str_out);
	}
}

#endif
