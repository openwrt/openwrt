/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
/*-------------------------------------------------------------------------------

	For type defines and data structure defines

--------------------------------------------------------------------------------*/
#ifndef __DRV_TYPES_H__
#define __DRV_TYPES_H__

#include <drv_conf.h>
#include <basic_types.h>
#include <osdep_service.h>
#include <rtw_byteorder.h>
#include <wlan_bssdef.h>
#include <wifi.h>
#include "../phl/rtw_general_def.h"
#include <ieee80211.h>
#ifdef CONFIG_ARP_KEEP_ALIVE_GW
	#include <net/neighbour.h>
	#include <net/arp.h>
#endif

typedef struct _ADAPTER _adapter;
struct _ADAPTER_LINK;
/* connection interface of drv and hal */

#include <rtw_debug.h>
#include <rtw_rf.h>
#include "../core/rtw_chset.h"
#include "../core/rtw_chplan.h"

#include "../phl/phl_headers_core.h"

#ifdef CONFIG_80211N_HT
	#include <rtw_ht.h>
#endif

#ifdef CONFIG_80211AC_VHT
	#include <rtw_vht.h>
#endif


#include <rtw_security.h>
#include <rtw_xmit.h>
#include <xmit_osdep.h>
#include <rtw_recv.h>
#include <rtw_rm.h>

#ifdef CONFIG_80211AX_HE
	#include <rtw_he.h>
#endif

#ifdef CONFIG_BEAMFORMING
	#include <rtw_beamforming.h>
#endif

#include <recv_osdep.h>
#include <rtw_sreset.h>

/*CONFIG_PHL_ARCH*/
#include "rtw_cmd.h"
#include "rtw_phl_cmd.h"
#include "phl_api_tmp.h"
#include "rtw_phl.h"

/*GEORGIA_TODO_FIXIT*/
#include "_hal_rate.h"
#include "_hal_api_tmp.h"

#include "platform_ops.h"
#include "rtw_scan.h"
#ifdef CONFIG_RTW_80211R
#include <rtw_ft.h>
#endif
#if defined(CONFIG_RTW_WNM) || defined(CONFIG_RTW_80211K)
#include <rtw_wnm.h>
#endif
#ifdef CONFIG_RTW_MBO
#include <rtw_mbo.h>
#endif

#ifdef CONFIG_80211BE_EHT
	#include <rtw_eht.h>
#endif

#include <rtw_qos.h>
#include <rtw_wow.h>
#include <rtw_pwrctrl.h>
#include <rtw_mlme.h>
#include <mlme_osdep.h>
#include <rtw_io.h>

#ifdef CONFIG_RTW_CORE_RXSC
#include <rtw_recv_shortcut.h>
#endif
#ifdef CONFIG_CORE_TXSC
#include <rtw_xmit_shortcut.h>
#endif

#include <rtw_ioctl.h>
#include <rtw_ioctl_set.h>
#include <rtw_ioctl_query.h>
#include "rtw_cfg.h"
#include <osdep_intf.h>
#include <sta_info.h>
#include <rtw_event.h>
#include <rtw_mlme_ext.h>
#include "../core/rtw_dfs.h"
#include "../core/rtw_txpwr.h"
#include <rtw_sec_cam.h>
#include <rtw_mi.h>
#include <rtw_ap.h>
#include <rtw_csa.h>
#ifdef CONFIG_RTW_WDS
#include "../core/wds/rtw_wds.h"
#endif
#ifdef CONFIG_RTW_MESH
#include "../core/mesh/rtw_mesh.h"
#endif
#ifdef CONFIG_WIFI_MONITOR
#include "../core/monitor/rtw_radiotap.h"
#endif

#include <rtw_version.h>

#include <rtw_p2p.h>

#ifdef CONFIG_TDLS
	#include <rtw_tdls.h>
#endif /* CONFIG_TDLS */

#ifdef CONFIG_WAPI_SUPPORT
	#include <rtw_wapi.h>
#endif /* CONFIG_WAPI_SUPPORT */

#ifdef CONFIG_MP_INCLUDED
	#include <rtw_mp.h>
	#include <rtw_efuse.h>
#endif /* CONFIG_MP_INCLUDED */

#ifdef CONFIG_BR_EXT
	#include <rtw_br_ext.h>
#endif /* CONFIG_BR_EXT */

#ifdef CONFIG_FPGA_INCLUDED
	#include <rtw_fpga.h>
#endif /* CONFIG_FPGA_INCLUDED */

#include <ip.h>
#include <if_ether.h>
#include <ethernet.h>
#include <circ_buf.h>

#include <rtw_android.h>

#include <rtw_btc.h>

#define SPEC_DEV_ID_NONE BIT(0)
#define SPEC_DEV_ID_DISABLE_HT BIT(1)
#define SPEC_DEV_ID_ENABLE_PS BIT(2)
#define SPEC_DEV_ID_RF_CONFIG_1T1R BIT(3)
#define SPEC_DEV_ID_RF_CONFIG_2T2R BIT(4)
#define SPEC_DEV_ID_ASSIGN_IFNAME BIT(5)

#if defined(RTW_PHL_TX) || defined(RTW_PHL_RX)
//#define PHLRX_LOG(fmt, args...) printk("phl-rx [%s][%d]"fmt, __FUNCTION__,__LINE__, ## args)
#define PHLRX_LOG		printk("phl-rx [%s][%d] \n", __FUNCTION__, __LINE__);
#define PHLRX_ENTER		printk("phl-rx [%s][%d] ++\n", __FUNCTION__, __LINE__);
#define PHLRX_EXIT		printk("phl-rx [%s][%d] --\n", __FUNCTION__, __LINE__);
#endif

struct specific_device_id {

	u32		flags;

	u16		idVendor;
	u16		idProduct;

};

struct registry_priv {
	u8	chip_version;
	u8	rfintfs;
	u8	lbkmode;
	u8	hci;
	NDIS_802_11_SSID	ssid;
	u8	network_mode;	/* infra, ad-hoc, auto */
	u8	band;
	u8	channel;/* ad-hoc support requirement */
	u8	wireless_mode;/* A, B, G, auto */
	u8	band_type;
	enum rtw_phl_scan_type	scan_mode;/*scan methods - active, passive */
	u8	radio_enable;
	u8	preamble;/* long, short, auto */
	u8	vrtl_carrier_sense;/* Enable, Disable, Auto */
	u8	vcs_type;/* RTS/CTS, CTS-to-self */
	u16	rts_thresh;
	u8	hw_rts_en;
	u16  frag_thresh;
	u8	adhoc_tx_pwr;
	u8	soft_ap;
	u8	ips_mode;
	u8	lps_mode;
	u8	smart_ps;
	u8   usb_rxagg_mode;
	u8	dynamic_agg_enable;
	u8	long_retry_lmt;
	u8	short_retry_lmt;
	u16	busy_thresh;
	u16	max_bss_cnt;
	u8	ack_policy;
	u8	mp_mode;
#if defined(CONFIG_MP_INCLUDED) && defined(CONFIG_RTW_CUSTOMER_STR)
	u8 mp_customer_str;
#endif
	u8  mp_dm;
	u8	software_encrypt;
	u8	software_decrypt;
#ifdef CONFIG_TX_EARLY_MODE
	u8   early_mode;
#endif
#ifdef CONFIG_NARROWBAND_SUPPORTING
	u8	rtw_nb_config;
#endif
	u8	acm_method;
	/* WMM */
	u8	wmm_enable;

	WLAN_BSSID_EX    dev_network;

	u8 tx_bw_mode;
#ifdef CONFIG_AP_MODE
	u8 bmc_tx_rate;
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	u8 ap_src_b2u_flags;
	u8 ap_fwd_b2u_flags;
	#endif
#endif

#ifdef CONFIG_RTW_MESH
	#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
	u8 msrc_b2u_flags;
	u8 mfwd_b2u_flags;
	#endif
#endif

#ifdef CONFIG_80211N_HT
	u8	ht_enable;
	/* 0: 20 MHz, 1: 40 MHz, 2: 80 MHz, 3: 160MHz */
	/* 2.4G use bit 0 ~ 3, 5G use bit 4 ~ 7,  6G use bit 8 ~ 11*/
	/* 0x321 means enable 2.4G 40MHz & 5G 80MHz & 6G 160MHz*/
	u16 bw_mode;
	u8	ampdu_enable;/* for tx */
	u8	rx_ampdu_amsdu;/* Rx A-MPDU Supports A-MSDU is permitted */
	u8	tx_ampdu_amsdu;/* Tx A-MPDU Supports A-MSDU is permitted */
	u8	tx_ampdu_num;
	u8	tx_quick_addba_req;
	u8 rx_ampdu_sz_limit_by_nss_bw[4][4]; /* 1~4SS, BW20~BW160 */
	/* Short GI support Bit Map */
	/* BIT0 - 20MHz, 1: support, 0: non-support */
	/* BIT1 - 40MHz, 1: support, 0: non-support */
	/* BIT2 - 80MHz, 1: support, 0: non-support */
	/* BIT3 - 160MHz, 1: support, 0: non-support */
	u8	short_gi;
	/*
	  * BIT0: Enable VHT LDPC Rx, BIT1: Enable VHT LDPC Tx
	  * BIT2: Enable HE LDPC Rx, BIT3: Enable HE LDPC Tx
	  * BIT4: Enable HT LDPC Rx, BIT5: Enable HT LDPC Tx
	  */
	u8	ldpc_cap;
	/*
	 * BIT0: Enable VHT STBC Rx, BIT1: Enable VHT STBC Tx
	 * BIT4: Enable HT STBC Rx, BIT5: Enable HT STBC Tx
	 * BIT8: Enable HE STBC Rx, BIT9: Enable HE STBC Rx(greater than 80M)
	 * BIT10: Enable HE STBC Tx, BIT11: Enable HE STBC Tx(greater than 80M)
	 */
	u16	stbc_cap;
	#if defined(CONFIG_RTW_TX_NPATH_EN)
	u8	tx_npath;
	#endif
	#if defined(CONFIG_RTW_PATH_DIV)
	u8 path_div;
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
	u16	beamform_cap;
	u8	dyn_txbf;
	u8	beamformer_rf_num;
	u8	beamformee_rf_num;
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
	u8	vht_enable; /* 0:disable, 1:enable, 2:auto */
	u8	vht_24g_enable; /* 0:disable, 1:enable */
	u8	ampdu_factor;
	u8 vht_rx_mcs_map[2];
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
	u8	he_enable; /* 0:disable, 1:enable, 2:auto */
#endif

	u8	lowrate_two_xmit;

	u8	low_power ;

	u8	wifi_spec;/* !turbo_mode */

	u8 rf_path; /*rf_config*/
	u8 tx_nss;
	u8 rx_nss;

#ifdef CONFIG_REGD_SRC_FROM_OS
	enum regd_src_t regd_src;
#endif
	bool init_regd_always_apply;
	bool user_regd_always_apply;
	char alpha2[2];
	u8	channel_plan;
	u8	excl_chs[MAX_CHANNEL_NUM_2G_5G];
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 channel_plan_6g;
	u8 excl_chs_6g[MAX_CHANNEL_NUM_6G];
#endif
	u8 dis_ch_flags;
	u32 bcn_hint_valid_ms;

	u8	full_ch_in_p2p_handshake; /* 0: reply only softap channel, 1: reply full channel list*/

#ifdef CONFIG_BTC
	u8	btcoex;
	u8	bt_iso;
	u8	bt_sco;
	u8	bt_ampdu;
	u8	ant_num;
	u8	single_ant_path;
#endif
	BOOLEAN	bAcceptAddbaReq;

	u8	antdiv_cfg;
	u8	antdiv_type;
	u8	drv_ant_band_switch;

	u8	switch_usb_mode;

	u8	hw_wps_pbc;/* 0:disable,1:enable */

#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
	char	adaptor_info_caching_file_path[PATH_LENGTH_MAX];
#endif

#ifdef CONFIG_LAYER2_ROAMING
	u8	max_roaming_times; /* the max number driver will try to roaming */
#endif

#ifdef CONFIG_80211D
	u8 country_ie_slave_en_mode;
	u8 country_ie_slave_flags;
	u8 country_ie_slave_en_role;
	u8 country_ie_slave_en_ifbmp;
	u32 country_ie_slave_scan_int_ms;
#endif

	u8 ifname[16];
	u8 if2name[16];
#if defined(CONFIG_PLATFORM_ANDROID) && (CONFIG_IFACE_NUMBER > 2)
	u8 if3name[16];
#endif
	/* for pll reference clock selction */
	u8 pll_ref_clk_sel;

	u8 target_tx_pwr_valid;
	s8 target_tx_pwr_2g[RF_PATH_MAX][RATE_SECTION_NUM];
#if CONFIG_IEEE80211_BAND_5GHZ
	s8 target_tx_pwr_5g[RF_PATH_MAX][RATE_SECTION_NUM - 1];
#endif

	s8	TxBBSwing_2G;
	s8	TxBBSwing_5G;
	u8	AmplifierType_2G;
	u8	AmplifierType_5G;
	u8	bEn_RFE;
	u8	RFE_Type;
	u8	PowerTracking_Type;
	u8	GLNA_Type;
	u8	RegPwrTrimEnable;

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	u8	load_phy_file;
	u8	RegDecryptCustomFile;
#endif
#if defined(CONFIG_CONCURRENT_MODE) && !RTW_P2P_GROUP_INTERFACE
#ifdef CONFIG_P2P
	u8 sel_p2p_iface;
#endif
#endif

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
	u8 ignore_go_in_scan;
	u8 ignore_low_rssi_in_scan;
#endif
	u32 vo_edca;

	u8 qos_opt_enable;

	u8 hiq_filter;
	u8 adaptivity_en;
	u8 adaptivity_mode;
	s8 adaptivity_th_l2h_ini;
	s8 adaptivity_th_edcca_hl_diff;
	u8 adaptivity_idle_probability;

	u8 boffefusemask;
	BOOLEAN bFileMaskEfuse;
	BOOLEAN bBTFileMaskEfuse;
#ifdef CONFIG_RTW_ACS
	u8 acs_auto_scan;
	u8 acs_mode;
#endif

	u32	reg_rxgain_offset_2g;
	u32	reg_rxgain_offset_5gl;
	u32	reg_rxgain_offset_5gm;
	u32	reg_rxgain_offset_5gh;

#ifdef CONFIG_DFS_MASTER
	u8 dfs_region_domain;
#endif

#ifdef CONFIG_RTW_NAPI
	u8 en_napi;
#ifdef CONFIG_RTW_NAPI_DYNAMIC
	u32 napi_threshold;	/* unit: Mbps */
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
#ifdef CONFIG_RTW_GRO
	u8 en_gro;
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

#ifdef CONFIG_WOWLAN
	u8 wowlan_enable;
	u8 wakeup_event;
	u8 suspend_type;
#endif

	u8 check_hw_status;
	u8 wowlan_sta_mix_mode;

#ifdef CONFIG_PCI_HCI
	u32 pci_aspm_config;
	u32 pci_dynamic_aspm_linkctrl;
#endif

	u8 iqk_fw_offload;
	u8 ch_switch_offload;

#ifdef CONFIG_TDLS
	u8 en_tdls;
#endif

#ifdef CONFIG_FW_OFFLOAD_PARAM_INIT
	u8 fw_param_init;
#endif

#ifdef DBG_LA_MODE
	u8 la_mode_en;
#endif
	u32 phydm_ability;
	u32 halrf_ability;
#ifdef CONFIG_TDMADIG
	u8 tdmadig_en;
	u8 tdmadig_mode;
	u8 tdmadig_dynamic;
#endif/*CONFIG_TDMADIG*/
	u8 en_dyn_rrsr;
	u32 set_rrsr_value;
#ifdef CONFIG_RTW_MESH
	u8 peer_alive_based_preq;
#endif

#ifdef RTW_BUSY_DENY_SCAN
	/*
	 * scan_interval_thr means scan interval threshold which is used to
	 * judge if user is in scan page or not.
	 * If scan interval < scan_interval_thr we guess user is in scan page,
	 * and driver won't deny any scan request at that time.
	 * Its default value comes from compiler flag
	 * BUSY_TRAFFIC_SCAN_DENY_PERIOD, and unit is ms.
	 */
	u32 scan_interval_thr;
#endif
	u16 scan_pch_ex_time;
	u8 deny_legacy;
#ifdef CONFIG_RTW_MULTI_AP
	u8 unassoc_sta_mode_of_stype[UNASOC_STA_SRC_NUM];
	u16 max_unassoc_sta_cnt;
#endif

#ifdef CONFIG_IOCTL_CFG80211
	u16 roch_min_home_dur; /* min duration for op channel */
	u16 roch_max_away_dur; /* max acceptable away duration for remain on channel */
	u16 roch_extend_dur; /* minimum duration to stay in roch when mgnt tx */
#endif

#if defined(PRIVATE_R) && defined(CONFIG_P2P)
	unsigned long go_hidden_ssid_mode;
	ATOMIC_T set_hide_ssid_timer;
#endif
	u8 amsdu_mode;

	u8 p2p_go_skip_keep_alive;

#if CONFIG_DFS
#ifdef CONFIG_ECSA_PHL
	u8 en_ecsa;
#endif
#endif
};

/* For registry parameters */
#define RGTRY_OFT(field) ((u32)FIELD_OFFSET(struct registry_priv, field))
#define RGTRY_SZ(field)   sizeof(((struct registry_priv *) 0)->field)

#define WOWLAN_IS_STA_MIX_MODE(_Adapter)	(_Adapter->registrypriv.wowlan_sta_mix_mode)
#define BSSID_OFT(field) ((u32)FIELD_OFFSET(WLAN_BSSID_EX, field))
#define BSSID_SZ(field)   sizeof(((PWLAN_BSSID_EX) 0)->field)

#define BW_MODE_2G(bw_mode) ((bw_mode) & 0x0F)
#define BW_MODE_5G(bw_mode) (((bw_mode) >> 4) & 0x0F)
#define BW_MODE_6G(bw_mode) (((bw_mode) >> 8) & 0x0F)
#ifdef CONFIG_80211N_HT
#define REGSTY_BW_2G(regsty) BW_MODE_2G((regsty)->bw_mode)
#define REGSTY_BW_5G(regsty) BW_MODE_5G((regsty)->bw_mode)
#define REGSTY_BW_6G(regsty) BW_MODE_6G((regsty)->bw_mode)
#else
#define REGSTY_BW_2G(regsty) CHANNEL_WIDTH_20
#define REGSTY_BW_5G(regsty) CHANNEL_WIDTH_20
#define REGSTY_BW_6G(regsty) CHANNEL_WIDTH_20
#endif
#define REGSTY_IS_BW_2G_SUPPORT(regsty, bw) (REGSTY_BW_2G((regsty)) >= (bw))
#define REGSTY_IS_BW_5G_SUPPORT(regsty, bw) (REGSTY_BW_5G((regsty)) >= (bw))
#define REGSTY_IS_BW_6G_SUPPORT(regsty, bw) (REGSTY_BW_6G((regsty)) >= (bw))

#ifdef CONFIG_80211AC_VHT
#define REGSTY_IS_11AC_ENABLE(regsty) ((regsty)->vht_enable != 0)
#define REGSTY_IS_11AC_AUTO(regsty) ((regsty)->vht_enable == 2)
#define REGSTY_IS_11AC_24G_ENABLE(regsty) ((regsty)->vht_24g_enable != 0)
#else
#define REGSTY_IS_11AC_ENABLE(regsty) 0
#define REGSTY_IS_11AC_AUTO(regsty) 0
#define REGSTY_IS_11AC_24G_ENABLE(regsty) 0
#endif

#define REGSTY_IS_11AX_ENABLE(regsty) ((regsty)->he_enable != 0)
#define REGSTY_IS_11AX_AUTO(regsty) ((regsty)->he_enable == 2)

#ifdef CONFIG_REGD_SRC_FROM_OS
#define REGSTY_REGD_SRC_FROM_OS(regsty) ((regsty)->regd_src == REGD_SRC_OS)
#else
#define REGSTY_REGD_SRC_FROM_OS(regsty) 0
#endif

#ifdef CONFIG_SDIO_HCI
	#include <drv_types_sdio.h>
#endif
#ifdef CONFIG_GSPI_HCI
	#include <drv_types_gspi.h>
#endif
#ifdef CONFIG_PCI_HCI
	#include <drv_types_pci.h>
#endif
#ifdef CONFIG_USB_HCI
	#include <drv_types_usb.h>
#endif

#include <rtw_trx.h>

#ifdef CONFIG_CONCURRENT_MODE
	#define is_primary_adapter(adapter) (adapter->adapter_type == PRIMARY_ADAPTER)
	#define is_vir_adapter(adapter) (adapter->adapter_type == VIRTUAL_ADAPTER)
#else
	#define is_primary_adapter(adapter) (1)
	#define is_vir_adapter(adapter) (0)
#endif
#define GET_PRIMARY_ADAPTER(padapter) (((_adapter *)padapter)->dvobj->padapters[IFACE_ID0])
#define GET_IFACE_NUMS(padapter) (((_adapter *)padapter)->dvobj->iface_nums)
#define GET_ADAPTER(padapter, iface_id) (((_adapter *)padapter)->dvobj->padapters[iface_id])
#define GET_PRIMARY_LINK(padapter) (((_adapter *)padapter)->adapter_link[RTW_RLINK_PRIMARY])
#define GET_LINK(padapter, link_idx) (((_adapter *)padapter)->adapter_link[link_idx])


#ifdef RTW_PHL_TX

#if 1
#define	PHLTX_ENTER //printk("eric-tx [%s][%d] ++\n", __FUNCTION__, __LINE__)
#define	PHLTX_LOG 	//printk("eric-tx [%s][%d]\n", __FUNCTION__, __LINE__)
#define	PHLTX_EXIT 	//printk("eric-tx [%s][%d] --\n", __FUNCTION__, __LINE__)
#define	PHLTX_ERR 	//printk("PHLTX_ERR [%s][%d]\n", __FUNCTION__, __LINE__)
#else
#define	PHLTX_ENTER printk("eric-tx [%s][%d] ++\n", __FUNCTION__, __LINE__)
#define	PHLTX_LOG 	printk("eric-tx [%s][%d]\n", __FUNCTION__, __LINE__)
#define	PHLTX_EXIT 	printk("eric-tx [%s][%d] --\n", __FUNCTION__, __LINE__)

#define	PHLTX_ERR 	printk("PHLTX_ERR [%s][%d]\n", __FUNCTION__, __LINE__)
#endif


#define SZ_TXREQ 	(sizeof(struct rtw_xmit_req))
#define SZ_HEAD_BUF	100
#define SZ_TAIL_BUF	30

#define NUM_PKT_LIST_PER_TXREQ	8
#define SZ_PKT_LIST (sizeof(struct rtw_pkt_buf_list))


#define SZ_TX_RING 		(SZ_TXREQ+SZ_HEAD_BUF+SZ_TAIL_BUF+(SZ_PKT_LIST*NUM_PKT_LIST_PER_TXREQ))
#define SZ_MGT_RING		(SZ_TXREQ + SZ_PKT_LIST)/* MGT_TXREQ_QMGT */

#ifndef MAX_TX_RING_NUM
#define MAX_TX_RING_NUM 	4096
#endif /*MAX_TX_RING_NUM*/
#endif


enum _IFACE_ID {
	IFACE_ID0, /*PRIMARY_ADAPTER*/
	IFACE_ID1,
	IFACE_ID2,
	IFACE_ID3,
	IFACE_ID4,
	IFACE_ID5,
	IFACE_ID6,
	IFACE_ID7,
	IFACE_ID_MAX,
};

#define VIF_START_ID	1

#ifdef CONFIG_DBG_COUNTER
struct rx_logs {
	u32 intf_rx;
	u32 intf_rx_err_recvframe;
	u32 intf_rx_err_skb;
	u32 intf_rx_report;
	u32 core_rx;
	u32 core_rx_pre;
	u32 core_rx_pre_ver_err;
	u32 core_rx_pre_mgmt;
	u32 core_rx_pre_mgmt_err_80211w;
	u32 core_rx_pre_mgmt_err;
	u32 core_rx_pre_ctrl;
	u32 core_rx_pre_ctrl_err;
	u32 core_rx_pre_data;
	u32 core_rx_pre_data_wapi_seq_err;
	u32 core_rx_pre_data_wapi_key_err;
	u32 core_rx_pre_data_handled;
	u32 core_rx_pre_data_err;
	u32 core_rx_pre_data_unknown;
	u32 core_rx_pre_unknown;
	u32 core_rx_enqueue;
	u32 core_rx_dequeue;
	u32 core_rx_post;
	u32 core_rx_post_decrypt;
	u32 core_rx_post_decrypt_wep;
	u32 core_rx_post_decrypt_tkip;
	u32 core_rx_post_decrypt_aes;
	u32 core_rx_post_decrypt_wapi;
	u32 core_rx_post_decrypt_gcmp;
	u32 core_rx_post_decrypt_hw;
	u32 core_rx_post_decrypt_unknown;
	u32 core_rx_post_decrypt_err;
	u32 core_rx_post_defrag_err;
	u32 core_rx_post_portctrl_err;
	u32 core_rx_post_indicate;
	u32 core_rx_post_indicate_in_oder;
	u32 core_rx_post_indicate_reoder;
	u32 core_rx_post_indicate_err;
	u32 os_indicate;
	u32 os_indicate_ap_mcast;
	u32 os_indicate_ap_forward;
	u32 os_indicate_ap_self;
	u32 os_indicate_err;
	u32 os_netif_ok;
	u32 os_netif_err;
};

struct tx_logs {
	u32 os_tx;
	u32 os_tx_err_up;
	u32 os_tx_err_xmit;
	u32 os_tx_m2u;
	u32 os_tx_m2u_ignore_fw_linked;
	u32 os_tx_m2u_ignore_self;
	u32 os_tx_m2u_entry;
	u32 os_tx_m2u_entry_err_xmit;
	u32 os_tx_m2u_entry_err_skb;
	u32 os_tx_m2u_stop;
	u32 core_tx;
	u32 core_tx_err_pxmitframe;
	u32 core_tx_err_brtx;
	u32 core_tx_upd_attrib;
	u32 core_tx_upd_attrib_adhoc;
	u32 core_tx_upd_attrib_sta;
	u32 core_tx_upd_attrib_ap;
	u32 core_tx_upd_attrib_unknown;
	u32 core_tx_upd_attrib_dhcp;
	u32 core_tx_upd_attrib_icmp;
	u32 core_tx_upd_attrib_active;
	u32 core_tx_upd_attrib_err_ucast_sta;
	u32 core_tx_upd_attrib_err_ucast_ap_link;
	u32 core_tx_upd_attrib_err_sta;
	u32 core_tx_upd_attrib_err_link;
	u32 core_tx_upd_attrib_err_sec;
	u32 core_tx_ap_enqueue_warn_fwstate;
	u32 core_tx_ap_enqueue_warn_sta;
	u32 core_tx_ap_enqueue_warn_nosta;
	u32 core_tx_ap_enqueue_warn_link;
	u32 core_tx_ap_enqueue_warn_trigger;
	u32 core_tx_ap_enqueue_mcast;
	u32 core_tx_ap_enqueue_ucast;
	u32 core_tx_ap_enqueue;
	u32 intf_tx;
	u32 intf_tx_pending_ac;
	u32 intf_tx_pending_fw_under_survey;
	u32 intf_tx_pending_fw_under_linking;
	u32 intf_tx_pending_xmitbuf;
	u32 intf_tx_enqueue;
	u32 core_tx_enqueue;
	u32 core_tx_enqueue_class;
	u32 core_tx_enqueue_class_err_sta;
	u32 core_tx_enqueue_class_err_nosta;
	u32 core_tx_enqueue_class_err_fwlink;
	u32 intf_tx_direct;
	u32 intf_tx_direct_err_coalesce;
	u32 intf_tx_dequeue;
	u32 intf_tx_dequeue_err_coalesce;
	u32 intf_tx_dump_xframe;
	u32 intf_tx_dump_xframe_err_txdesc;
	u32 intf_tx_dump_xframe_err_port;
};

struct int_logs {
	u32 all;
	u32 err;
	u32 tbdok;
	u32 tbder;
	u32 bcnderr;
	u32 bcndma;
	u32 bcndma_e;
	u32 rx;
	u32 rx_rdu;
	u32 rx_fovw;
	u32 txfovw;
	u32 mgntok;
	u32 highdok;
	u32 bkdok;
	u32 bedok;
	u32 vidok;
	u32 vodok;
};

#endif /* CONFIG_DBG_COUNTER */

#ifdef RTW_DETECT_HANG
struct fw_hang_info {
	u8 dbg_is_fw_hang;
	u8 dbg_is_fw_gone;
};

struct rxff_hang_info {
	u8 dbg_is_rxff_hang;
	u8 rx_ff_hang_cnt;
	/* increase when HCI Rx */
	u16 rx_cnt;
	/* record in rtw_is_rxff_hang() */
	u16 last_rx_cnt;
};

struct hang_info {
	u8 is_stop;
	u32 enter_cnt;
	struct rxff_hang_info dbg_rxff_hang_info;
	struct fw_hang_info dbg_fw_hang_info;
};
#endif /* RTW_DETECT_HANG */

struct debug_priv {
	u32 dbg_sdio_free_irq_error_cnt;
	u32 dbg_sdio_alloc_irq_error_cnt;
	u32 dbg_sdio_free_irq_cnt;
	u32 dbg_sdio_alloc_irq_cnt;
	u32 dbg_sdio_deinit_error_cnt;
	u32 dbg_sdio_init_error_cnt;
	u32 dbg_suspend_error_cnt;
	u32 dbg_suspend_cnt;
	u32 dbg_resume_cnt;
	u32 dbg_resume_error_cnt;
	u32 dbg_deinit_fail_cnt;
	u32 dbg_carddisable_cnt;
	u32 dbg_carddisable_error_cnt;
	u32 dbg_ps_insuspend_cnt;
	u32 dbg_dev_unload_inIPS_cnt;
	u32 dbg_wow_leave_ps_fail_cnt;
	u32 dbg_scan_pwr_state_cnt;
	u32 dbg_downloadfw_pwr_state_cnt;
	u32 dbg_fw_read_ps_state_fail_cnt;
	u32 dbg_leave_ips_fail_cnt;
	u32 dbg_leave_lps_fail_cnt;
	u32 dbg_h2c_leave32k_fail_cnt;
	u32 dbg_diswow_dload_fw_fail_cnt;
	u32 dbg_enwow_dload_fw_fail_cnt;
	u32 dbg_ips_drvopen_fail_cnt;
	u32 dbg_poll_fail_cnt;
	u32 dbg_rpwm_toogle_cnt;
	u32 dbg_rpwm_timeout_fail_cnt;
	u32 dbg_sreset_cnt;
	u32 dbg_fw_mem_dl_error_cnt;
	u64 dbg_rx_fifo_last_overflow;
	u64 dbg_rx_fifo_curr_overflow;
	u64 dbg_rx_fifo_diff_overflow;
#ifdef RTW_DETECT_HANG
	struct hang_info dbg_hang_info;
#endif
};

struct rtw_traffic_statistics {
	/* tx statistics */
	u64	tx_bytes;
	u64	tx_pkts;
	u64	tx_drop;
	u64	cur_tx_bytes;
	u64	last_tx_bytes;
	u32	cur_tx_tp; /* Tx throughput in Mbps. */

	/* rx statistics */
	u64	rx_bytes;
	u64	rx_pkts;
	u64	rx_drop;
	u64	cur_rx_bytes;
	u64	last_rx_bytes;
	u32	cur_rx_tp; /* Rx throughput in Mbps. */
};

#define SEC_CAP_CHK_EXTRA_SEC	BIT1 /* 256 bit */

#define KEY_FMT "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
#define KEY_ARG(x) ((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3], ((u8 *)(x))[4], ((u8 *)(x))[5], \
	((u8 *)(x))[6], ((u8 *)(x))[7], ((u8 *)(x))[8], ((u8 *)(x))[9], ((u8 *)(x))[10], ((u8 *)(x))[11], \
	((u8 *)(x))[12], ((u8 *)(x))[13], ((u8 *)(x))[14], ((u8 *)(x))[15]



/* used for rf_ctl_t.rate_bmp_cck_ofdm */
#define RATE_BMP_CCK		0x000F
#define RATE_BMP_OFDM		0xFFF0
#define RATE_BMP_HAS_CCK(_bmp_cck_ofdm)		(_bmp_cck_ofdm & RATE_BMP_CCK)
#define RATE_BMP_HAS_OFDM(_bmp_cck_ofdm)	(_bmp_cck_ofdm & RATE_BMP_OFDM)
#define RATE_BMP_GET_CCK(_bmp_cck_ofdm)		(_bmp_cck_ofdm & RATE_BMP_CCK)
#define RATE_BMP_GET_OFDM(_bmp_cck_ofdm)	((_bmp_cck_ofdm & RATE_BMP_OFDM) >> 4)

/* used for rf_ctl_t.rate_bmp_ht_by_bw */
#define RATE_BMP_HT_1SS		0x000000FF
#define RATE_BMP_HT_2SS		0x0000FF00
#define RATE_BMP_HT_3SS		0x00FF0000
#define RATE_BMP_HT_4SS		0xFF000000
#define RATE_BMP_HAS_HT_1SS(_bmp_ht)		(_bmp_ht & RATE_BMP_HT_1SS)
#define RATE_BMP_HAS_HT_2SS(_bmp_ht)		(_bmp_ht & RATE_BMP_HT_2SS)
#define RATE_BMP_HAS_HT_3SS(_bmp_ht)		(_bmp_ht & RATE_BMP_HT_3SS)
#define RATE_BMP_HAS_HT_4SS(_bmp_ht)		(_bmp_ht & RATE_BMP_HT_4SS)
#define RATE_BMP_GET_HT_1SS(_bmp_ht)		(_bmp_ht & RATE_BMP_HT_1SS)
#define RATE_BMP_GET_HT_2SS(_bmp_ht)		((_bmp_ht & RATE_BMP_HT_2SS) >> 8)
#define RATE_BMP_GET_HT_3SS(_bmp_ht)		((_bmp_ht & RATE_BMP_HT_3SS) >> 16)
#define RATE_BMP_GET_HT_4SS(_bmp_ht)		((_bmp_ht & RATE_BMP_HT_4SS) >> 24)

/* used for rf_ctl_t.rate_bmp_vht_by_bw */
#define RATE_BMP_VHT_1SS	0x00000003FF
#define RATE_BMP_VHT_2SS	0x00000FFC00
#define RATE_BMP_VHT_3SS	0x003FF00000
#define RATE_BMP_VHT_4SS	0xFFC0000000
#define RATE_BMP_HAS_VHT_1SS(_bmp_vht)		(_bmp_vht & RATE_BMP_VHT_1SS)
#define RATE_BMP_HAS_VHT_2SS(_bmp_vht)		(_bmp_vht & RATE_BMP_VHT_2SS)
#define RATE_BMP_HAS_VHT_3SS(_bmp_vht)		(_bmp_vht & RATE_BMP_VHT_3SS)
#define RATE_BMP_HAS_VHT_4SS(_bmp_vht)		(_bmp_vht & RATE_BMP_VHT_4SS)
#define RATE_BMP_GET_VHT_1SS(_bmp_vht)		((u16)(_bmp_vht & RATE_BMP_VHT_1SS))
#define RATE_BMP_GET_VHT_2SS(_bmp_vht)		((u16)((_bmp_vht & RATE_BMP_VHT_2SS) >> 10))
#define RATE_BMP_GET_VHT_3SS(_bmp_vht)		((u16)((_bmp_vht & RATE_BMP_VHT_3SS) >> 20))
#define RATE_BMP_GET_VHT_4SS(_bmp_vht)		((u16)((_bmp_vht & RATE_BMP_VHT_4SS) >> 30))

#define TXPWR_LMT_REF_VHT_FROM_HT	BIT0
#define TXPWR_LMT_REF_HT_FROM_VHT	BIT1

#define TXPWR_LMT_HAS_CCK_1T	BIT0
#define TXPWR_LMT_HAS_CCK_2T	BIT1
#define TXPWR_LMT_HAS_CCK_3T	BIT2
#define TXPWR_LMT_HAS_CCK_4T	BIT3
#define TXPWR_LMT_HAS_OFDM_1T	BIT4
#define TXPWR_LMT_HAS_OFDM_2T	BIT5
#define TXPWR_LMT_HAS_OFDM_3T	BIT6
#define TXPWR_LMT_HAS_OFDM_4T	BIT7

#define OFFCHS_NONE			0
#define OFFCHS_LEAVING_OP	1
#define OFFCHS_LEAVE_OP		2
#define OFFCHS_BACKING_OP	3

#define COUNTRY_IE_SLAVE_EN_ROLE_STA	BIT0 /* pure STA mode */
#define COUNTRY_IE_SLAVE_EN_ROLE_GC		BIT1 /* P2P group client */

struct rf_ctl_t {
	bool disable_sw_chplan;

	enum regd_src_t regd_src;
	u8 regd_inr_bmp;
	char alpha2[2];
	u8 domain_code;
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 domain_code_6g;
#endif

#if defined(CONFIG_80211AX_HE) || defined(CONFIG_80211AC_VHT) || CONFIG_IEEE80211_BAND_5GHZ
	u8 proto_en;
#endif
	u8 dis_ch_flags;
	u8 excl_chs[MAX_CHANNEL_NUM_2G_5G];
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 excl_chs_6g[MAX_CHANNEL_NUM_6G];
#endif

	_mutex regd_req_mutex;
	_list regd_req_list;
	u8 regd_req_num;
	bool init_regd_always_apply;
	bool user_regd_always_apply;
	struct regd_req_t init_req;
	struct regd_req_t *user_req;

	u32 bcn_hint_valid_ms; /* the length of time beacon hint continue */

#ifdef CONFIG_80211D
	enum country_ie_slave_en_mode cis_en_mode;
	u8 cis_flags; /* bitmap of enum country_ie_slave_flags */

	u8 cis_en_role; /* per link cis enable role, see COUNTRY_IE_SLAVE_EN_ROLE_XXX, used when CISF_ENV_BSS is not set */
	u8 cis_en_ifbmp; /* per link cis enable iface bitmap, used when CISF_ENV_BSS is not set */
	u32 cis_scan_int_ms; /* 0 means no env BSS scan triggerred by driver self, used when CISF_ENV_BSS is set */

	bool cis_enabled; /* enable status */

	/* per link cis status, used when CISF_ENV_BSS is not set */
	u8 *recv_country_ie[CONFIG_IFACE_NUMBER][RTW_RLINK_MAX];
	u32 recv_country_ie_len[CONFIG_IFACE_NUMBER][RTW_RLINK_MAX];
	struct country_ie_slave_record cisr[CONFIG_IFACE_NUMBER][RTW_RLINK_MAX];

	/* effected one, used when CISF_INTERSECT is not set */
	struct country_ie_slave_record *effected_cisr;
	struct country_ie_slave_record effected_cisr_cont; /*  valid when effected_cisr != NULL */
#endif

	struct rtw_chset chset;
	struct op_class_pref_t **spt_op_class_ch;
	u8 cap_spt_op_class_num;
	u8 reg_spt_op_class_num;
	u8 cur_spt_op_class_num;
	struct p2p_channels channel_list;

	u8 op_class;
	u8 op_ch;
	s16 op_txpwr_max; /* mBm */
	u8 if_op_class[CONFIG_IFACE_NUMBER];
	u8 if_op_ch[CONFIG_IFACE_NUMBER];

	_mutex offch_mutex;
	u8 offch_state;

	/* used for debug or by tx power limit */
	u16 rate_bmp_cck_ofdm;		/* 20MHz */
	u32 rate_bmp_ht_by_bw[2];	/* 20MHz, 40MHz. 4SS supported */
	u64 rate_bmp_vht_by_bw[4];	/* 20MHz, 40MHz, 80MHz, 160MHz. 4SS supported */

#if CONFIG_TXPWR_LIMIT
	u8 highest_ht_rate_bw_bmp;
	u8 highest_vht_rate_bw_bmp;
#endif
	u8 tpc_mode;
	u16 tpc_manual_constraint; /* mB */

	bool ch_sel_within_same_band;

	u8 edcca_mode_2g;
#if CONFIG_IEEE80211_BAND_5GHZ
	u8 edcca_mode_5g;
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 edcca_mode_6g;
#endif
	enum band_type last_edcca_mode_op_band;

#if CONFIG_DFS
#ifdef CONFIG_DFS_MASTER
	enum rtw_dfs_regd dfs_region_domain;
	_timer radar_detect_timer;
	bool radar_detect_by_others[HW_BAND_MAX];
	u8 radar_detect_enabled;

	bool radar_detected;

	enum phl_band_idx radar_detect_hwband;
	/* 5G band is implicit */
	u8 radar_detect_cch[HW_BAND_MAX];
	u8 radar_detect_bw[HW_BAND_MAX];
	u32 radar_detect_freq_hi[HW_BAND_MAX];
	u32 radar_detect_freq_lo[HW_BAND_MAX];

	bool non_ocp_finished;
	systime cac_start_time;
	systime cac_end_time;
	u8 cac_force_stop;

#if CONFIG_DFS_SLAVE_WITH_RADAR_DETECT
	u8 dfs_slave_with_rd;
#endif
	u8 dfs_ch_sel_e_flags;
	u8 dfs_ch_sel_d_flags;

	u8 dbg_dfs_fake_radar_detect_cnt;
	u8 dbg_dfs_radar_detect_trigger_non;
	u8 dbg_dfs_choose_dfs_ch_first;
#endif /* CONFIG_DFS_MASTER */
#endif /* CONFIG_DFS */
#ifdef CONFIG_RTW_MBO
	struct npref_ch_rtp ch_rtp;
#endif
};

#define RFCTL_REG_WORLDWIDE(rfctl)	(IS_ALPHA2_WORLDWIDE(rfctl->alpha2))
#define RFCTL_REG_ALPHA2_UNSPEC(rfctl)	(IS_ALPHA2_UNSPEC(rfctl->alpha2)) /* ex: only domain code is specified */
#define RFCTL_REG_INTERSECTED(rfctl)	(IS_ALPHA2_INTERSECTED(rfctl->alpha2))

#if CONFIG_IEEE80211_BAND_5GHZ
#define RFCTL_REG_EN_11A(rfctl) (((rfctl)->proto_en & CHPLAN_PROTO_EN_A) ? true : false)
#else
#define RFCTL_REG_EN_11A(rfctl) false
#endif

#ifdef CONFIG_80211AC_VHT
#define RFCTL_REG_EN_11AC(rfctl) (((rfctl)->proto_en & CHPLAN_PROTO_EN_AC) ? true : false)
#else
#define RFCTL_REG_EN_11AC(rfctl) false
#endif

#ifdef CONFIG_80211AX_HE
#define RFCTL_REG_EN_11AX(rfctl) (((rfctl)->proto_en & CHPLAN_PROTO_EN_AX) ? true : false)
#else
#define RFCTL_REG_EN_11AX(rfctl) false
#endif

#ifdef CONFIG_DFS_MASTER
#define IS_CAC_STOPPED(rfctl) ((rfctl)->cac_end_time == RTW_CAC_STOPPED)
#define IS_CH_WAITING(rfctl) (!IS_CAC_STOPPED(rfctl) && rtw_time_after((rfctl)->cac_end_time, rtw_get_current_time()))
#define IS_UNDER_CAC(rfctl) (IS_CH_WAITING(rfctl) && rtw_time_after_eq(rtw_get_current_time(), (rfctl)->cac_start_time))
#define IS_RADAR_DETECTED(rfctl) ((rfctl)->radar_detected)
#else
#define IS_CAC_STOPPED(rfctl) 1
#define IS_CH_WAITING(rfctl) 0
#define IS_UNDER_CAC(rfctl) 0
#define IS_RADAR_DETECTED(rfctl) 0
#endif /* CONFIG_DFS_MASTER */

#if CONFIG_DFS_SLAVE_WITH_RADAR_DETECT
#define IS_DFS_SLAVE_WITH_RD(rfctl) ((rfctl)->dfs_slave_with_rd)
#else
#define IS_DFS_SLAVE_WITH_RD(rfctl) 0
#endif

#define HWBAND_FMT "HB%d"
#define HWBAND_ARG(band_idx) (band_idx)
#define FUNC_HWBAND_FMT "%s("HWBAND_FMT")"
#define FUNC_HWBAND_ARG(band_idx) __func__, HWBAND_ARG(band_idx)

#ifdef CONFIG_USB_HCI

struct trx_urb_buf_q {
	_queue free_urb_buf_queue;
	u8 *alloc_urb_buf;
	u8 *urb_buf;
	uint free_urb_buf_cnt;
};

struct data_urb {
	_list	list;
	struct urb *urb;
	u8 bulk_id;
	u8 minlen;
};

#endif

struct trx_data_buf_q {
	_queue free_data_buf_queue;
	u8 *alloc_data_buf;
	u8 *data_buf;
	uint free_data_buf_cnt;
};


struct lite_data_buf {
	_list	list;
	struct dvobj_priv *dvobj;
	u16 buf_tag;
	u8 *pbuf;
	u8 *phl_buf_ptr; /*point to phl rtw_usb_buf from phl*/
#ifdef CONFIG_USB_HCI
	struct data_urb *dataurb;
#endif
	struct submit_ctx *sctx;

};

#ifdef CONFIG_DRV_FAKE_AP
struct fake_ap {
	struct sk_buff_head rxq;	/* RX queue */
	_workitem work;
	struct rtw_timer_list bcn_timer;
};
#endif /* CONFIG_DRV_FAKE_AP */

/*device object*/
struct dvobj_priv {
	/*-------- below is common data --------*/
	ATOMIC_T bSurpriseRemoved;
	ATOMIC_T bDriverStopped;
	ATOMIC_T hw_start;
	s32	processing_dev_remove;

	_mutex hw_init_mutex;
	_mutex ioctrl_mutex;
	_mutex setch_mutex;
	_mutex setbw_mutex;

	_adapter *padapters[CONFIG_IFACE_NUMBER];/*IFACE_ID_MAX*/
	u8 virtual_iface_num;/*from registary*/
	u8 iface_nums; /* total number of ifaces used runtime */
	struct mi_state iface_state[MAX_BAND_NUM];

	enum rtl_ic_id ic_id;
	enum rtw_hci_type interface_type;/*USB,SDIO,SPI,PCI*/

	/*CONFIG_PHL_ARCH*/
	void *phl;
	struct rtw_phl_com_t *phl_com;
	#ifdef DBG_PHL_MEM_ALLOC
	ATOMIC_T phl_mem;
	#endif

	struct rf_ctl_t rf_ctl;
	/* move to phl */
	/* struct macid_ctl_t macid_ctl; *//*shared HW resource*/
	struct cam_ctl_t cam_ctl;/*sec-cam shared HW resource*/
	struct sec_cam_ent cam_cache[SEC_CAM_ENT_NUM_SW_LIMIT];


	/****** Band info may be x 2*********/
	/* saved channel info when call set_channel_bw */
	systime on_oper_ch_time;

	u32 fa_cnt_acc[HW_BAND_MAX];

	/****** hal dep info*********/


	ATOMIC_T continual_io_error;
	ATOMIC_T disable_func;

	u8 xmit_block;
	_lock xmit_block_lock;

	struct pwrctrl_priv pwrctl_priv;
	struct cmd_priv	cmdpriv;
	struct recv_priv recvpriv;
#ifdef CONFIG_WOWLAN
	struct wow_priv wowlan_priv;
#endif /* CONFIG_WOWLAN */

	struct rtw_traffic_statistics	traffic_stat;

	#ifdef PLATFORM_LINUX
	_thread_hdl_ rtnl_lock_holder;

	#if defined(CONFIG_IOCTL_CFG80211)
	struct wiphy *wiphy;
	#endif
	#endif /* PLATFORM_LINUX */

	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	_timer dynamic_chk_timer; /* dynamic/periodic check timer */
	#endif

	#ifdef CONFIG_RTW_NAPI_DYNAMIC
	u8 en_napi_dynamic;
	#endif /* CONFIG_RTW_NAPI_DYNAMIC */

	#ifdef CONFIG_RTW_WIFI_HAL
	u32 nodfs;
	#endif

	/*-------- below is for PCIE/USB/SDIO INTERFACE --------*/
	#ifdef CONFIG_SDIO_HCI
	struct sdio_data sdio_data;
	#endif
	#ifdef CONFIG_GSPI_HCI
	GSPI_DATA gspi_data;
	#endif
	#ifdef CONFIG_PCI_HCI
	PCI_DATA pci_data;
	#endif
	#ifdef CONFIG_USB_HCI
	USB_DATA usb_data;
	#endif

	struct rtw_intf_ops *intf_ops;

	struct trx_data_buf_q  litexmitbuf_q;
	struct trx_data_buf_q  litexmit_extbuf_q;
	struct trx_data_buf_q  literecvbuf_q;

	/*-------- below is for USB INTERFACE --------*/
	#ifdef CONFIG_USB_HCI
	u8	Queue2Pipe[HW_QUEUE_ENTRY];/* for out pipe mapping */
	struct trx_urb_buf_q xmit_urb_q;
	struct trx_urb_buf_q recv_urb_q;
	#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	struct trx_data_buf_q  intin_buf_q;
	struct trx_urb_buf_q intin_urb_q;
	ATOMIC_T rx_pending_cnt;/* urb counts for sumit to host  */
	#endif
	#endif/* CONFIG_USB_HCI */

	/*-------- below is for PCIE INTERFACE --------*/
	#ifdef CONFIG_PCI_HCI


	#endif/* CONFIG_PCI_HCI */

	/* also for RTK T/P Testing Mode */
	u8 scan_deny;


	#ifdef CONFIG_RTW_CUSTOMER_STR
	_mutex customer_str_mutex;
	struct submit_ctx *customer_str_sctx;
	u8 customer_str[RTW_CUSTOMER_STR_LEN];
	#endif

	struct debug_priv drv_dbg;

#ifdef CONFIG_DRV_FAKE_AP
	struct fake_ap fakeap;
#endif /* CONFIG_DRV_FAKE_AP */

	/* WPAS maintain from w1.fi */
#define RTW_WPAS_W1FI		0x00
	/* WPAS maintain from android */
#define RTW_WPAS_ANDROID	0x01
	u8 wpas_type;
};

#define HWBAND_STA_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_STA_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_STA_LD_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_STA_LD_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_STA_LG_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_STA_LG_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_TDLS_LD_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_TDLS_LD_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_AP_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_AP_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_AP_STARTING_NUM(_dvobj, _band_idx)	((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_AP_STARTING_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_AP_LD_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_AP_LD_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_ADHOC_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_ADHOC_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_ADHOC_LD_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_ADHOC_LD_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_MESH_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_MESH_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_MESH_LD_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_MESH_LD_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_P2P_DV_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_P2P_DV_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_P2P_GC_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_P2P_GC_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_P2P_GO_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_P2P_GO_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_SCAN_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_SCAN_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_WPS_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_WPS_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_ROCH_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_ROCH_NUM(&((_dvobj)->iface_state[_band_idx])))
#define HWBAND_MGMT_TX_NUM(_dvobj, _band_idx)		((_band_idx) >= HW_BAND_MAX ? 0 : MSTATE_MGMT_TX_NUM(&((_dvobj)->iface_state[_band_idx])))

#define dvobj_to_pwrctl(dvobj) (&(dvobj->pwrctl_priv))
#ifdef CONFIG_WOWLAN
#define dvobj_to_wowlan(dvobj) (&(dvobj->wowlan_priv))
#endif /* CONFIG_WOWLAN */
#define pwrctl_to_dvobj(pwrctl) container_of(pwrctl, struct dvobj_priv, pwrctl_priv)
#define dvobj_to_macidctl(dvobj) (&(dvobj->macid_ctl))
#define dvobj_to_sec_camctl(dvobj) (&(dvobj->cam_ctl))
#define dvobj_to_regsty(dvobj) (&(dvobj->padapters[IFACE_ID0]->registrypriv))
#if defined(CONFIG_IOCTL_CFG80211)
#define dvobj_to_wiphy(dvobj) ((dvobj)->wiphy)
#endif
#define dvobj_to_rfctl(dvobj) (&(dvobj->rf_ctl))
#define rfctl_to_dvobj(rfctl) container_of((rfctl), struct dvobj_priv, rf_ctl)
#define dvobj_to_chset(dvobj) (&dvobj_to_rfctl(dvobj)->chset)

#ifdef CONFIG_PCI_HCI
static inline PCI_DATA *dvobj_to_pci(struct dvobj_priv *dvobj)
{
	return &dvobj->pci_data;
}
#endif
#ifdef CONFIG_USB_HCI
static inline USB_DATA *dvobj_to_usb(struct dvobj_priv *dvobj)
{
	return &dvobj->usb_data;
}
#endif
#ifdef CONFIG_SDIO_HCI
#define dvobj_to_sdio(d)		(&(d)->sdio_data)
#define rtw_is_sdio30(d)		(!(!dvobj_to_sdio(d)->sd3_bus_mode))
#define rtw_sdio_get_clock(d)		(dvobj_to_sdio(d)->clock)		/* unit: HZ */
#define rtw_sdio_get_block_size(d)	(dvobj_to_sdio(d)->block_transfer_len)	/* unit: byte */
#define rtw_sdio_set_irq_thd(d, id)	do {dvobj_to_sdio(d)->sys_sdio_irq_thd = id;} while (0)
#ifdef PLATFORM_LINUX
#define dvobj_to_sdio_func(d)		(dvobj_to_sdio(d)->func)
#endif /* PLATFORM_LINUX */
#endif /* CONFIG_SDIO_HCI */
#ifdef CONFIG_GSPI_HCI
static inline GSPI_DATA *dvobj_to_gspi(struct dvobj_priv *dvobj)
{
	return &dvobj->gspi_data;
}
#endif


#ifdef PLATFORM_LINUX
static inline struct device *dvobj_to_dev(struct dvobj_priv *dvobj)
{
	/* todo: get interface type from dvobj and the return the dev accordingly */
#ifdef RTW_DVOBJ_CHIP_HW_TYPE
#endif

#ifdef CONFIG_USB_HCI
	return &dvobj->usb_data.pusbintf->dev;
#endif
#ifdef CONFIG_SDIO_HCI
	return &dvobj->sdio_data.func->dev;
#endif
#ifdef CONFIG_GSPI_HCI
	return &dvobj->gspi_data.func->dev;
#endif
#ifdef CONFIG_PCI_HCI
	return &dvobj->pci_data.ppcidev->dev;
#endif
}
#endif

_adapter *dvobj_get_unregisterd_adapter(struct dvobj_priv *dvobj);
_adapter *dvobj_get_adapter_by_addr(struct dvobj_priv *dvobj, u8 *addr);
#define dvobj_get_primary_adapter(dvobj)	((dvobj)->padapters[IFACE_ID0])


enum _ADAPTER_TYPE {
	PRIMARY_ADAPTER,
	VIRTUAL_ADAPTER,
	MAX_ADAPTER = 0xFF,
};

#ifdef CONFIG_RTW_NAPI
enum _NAPI_STATE {
	NAPI_DISABLE = 0,
	NAPI_ENABLE = 1,
};
#endif

#if 0 /*#ifdef CONFIG_MAC_LOOPBACK_DRIVER*/
typedef struct loopbackdata {
	_sema	sema;
	_thread_hdl_ lbkthread;
	u8 bstop;
	u32 cnt;
	u16 size;
	u16 txsize;
	u8 txbuf[0x8000];
	u16 rxsize;
	u8 rxbuf[0x8000];
	u8 msg[100];

} LOOPBACKDATA, *PLOOPBACKDATA;
#endif

#define ADAPTER_TX_BW_2G(adapter) BW_MODE_2G((adapter)->driver_tx_bw_mode)
#define ADAPTER_TX_BW_5G(adapter) BW_MODE_5G((adapter)->driver_tx_bw_mode)
#define ADAPTER_TX_BW_6G(adapter) BW_MODE_6G((adapter)->driver_tx_bw_mode)

#ifdef RTW_PHL_DBG_CMD
#define	CORE_LOG_NUM 	(100)
#define MAX_FRAG		(4)
#define INV_TXFORCE_VAL	(0xFFFF)

/*
enum _CORE_REC_DUMP {
	REC_DUMP_NO = 0,
	REC_DUMP_ALL,
	REC_DUMP_TX,
	REC_DUMP_RX,
};
*/

enum _CORE_REC_TYPE {
	REC_TX_MGMT = 0,
	REC_TX_DATA,
	REC_TX_PHL,
	REC_TX_PHL_RCC,

	REC_RX_PHL,
	REC_RX_PHL_RCC,
	REC_RX_MGMT,
	REC_RX_DATA,
	REC_RX_DATA_RETRY,
};

struct core_record {
	u32 type;
	u32 totalSz;

	u32 wl_seq;
	u32 wl_type;
	u32 wl_subtype;

	u8 	fragNum;
	u32 fragLen[MAX_FRAG];
	void* virtAddr[MAX_FRAG];
	void* phyAddrL[MAX_FRAG];
	void* phyAddrH[MAX_FRAG];
};

struct core_logs {
	u32 txCnt_all;
	u32 txCnt_data;
	u32 txCnt_mgmt;
	u32 txCnt_phl;
	u32 txSize_phl;
	u32 txCnt_recycle;
	u32 txSize_recycle;
	struct core_record drvTx[CORE_LOG_NUM];
	struct core_record phlTx[CORE_LOG_NUM];
	struct core_record txRcycle[CORE_LOG_NUM];

	u32 rxCnt_phl;
	u32 rxSize_phl;
	u32 rxCnt_recycle;
	u32 rxSize_recycle;
	u32 rxCnt_data;
	u32 rxCnt_data_retry;
	u32 rxCnt_mgmt;
	u32 rxCnt_all;
	struct core_record drvRx[CORE_LOG_NUM];
	struct core_record phlRx[CORE_LOG_NUM];
	struct core_record rxRcycle[CORE_LOG_NUM];
#ifdef CONFIG_RTW_CORE_RXSC
	u32 rxCnt_data_orig;
	u32 rxCnt_data_shortcut;
#endif
};

#define MAX_TXBD_SIZE	40
#define MAX_TXWD_SIZE	128
#define MAX_RXWD_SIZE	32


enum _PHL_REC_TYPE {
	REC_TXBD = 0,
	REC_TXWD,
	REC_RXWD,
	REC_WP_RCC,
	REC_RX_MAP,
	REC_RX_UNMAP,
	REC_RX_AMPDU,
};

struct record_txbd {
	u32 bd_len;
	u8	bd_buf[MAX_TXBD_SIZE];
};

struct record_txwd {
	u32 wp_seq;
	u32 wd_len;
	u8	wd_buf[MAX_TXWD_SIZE];
};

struct record_rxwd {
	u32 wd_len;
	u8	wd_buf[MAX_RXWD_SIZE];
};

struct record_pci {
	u32 map_len;
	void *virtAddr;
	void* phyAddrL;
	void* phyAddrH;
};

struct record_wp_rcc {
	u32 wp_seq;
};

struct phl_logs {
	u32 txCnt_bd;
	u32 txCnt_wd;
	u32 txCnt_recycle;

	struct record_txbd txBd[CORE_LOG_NUM];
	struct record_txwd txWd[CORE_LOG_NUM];
	struct record_wp_rcc wpRecycle[CORE_LOG_NUM];

	u32 rxCnt_map;
	u32 rxSize_map;
	u32 rxCnt_unmap;
	u32 rxSize_unmap;
	struct record_pci rxPciMap[CORE_LOG_NUM];
	struct record_pci rxPciUnmap[CORE_LOG_NUM];

	u32 rxCnt_wd;
	struct record_rxwd rxWd[CORE_LOG_NUM];

	u32 rxCnt_ampdu;
	u32	rxAmpdu[CORE_LOG_NUM];
};

#endif

enum _DIS_TURBO_EDCA {
	EN_TURBO = 0,
	DIS_TURBO,
	DIS_TURBO_USE_MANUAL,
};

struct _ADAPTER_LINK {
	_adapter *adapter;
	u8 mac_addr[ETH_ALEN]; /* link address */
	struct rtw_wifi_role_link_t *wrlink;

	struct link_mlme_priv mlmepriv;
	struct link_mlme_ext_priv mlmeextpriv;
	struct link_security_priv securitypriv;
};

#define ALINK_GET_HWBAND(alink) ((alink)->wrlink->hw_band)
#define ALINK_GET_BAND(alink) ((alink)->mlmeextpriv.chandef.band)
#define ALINK_GET_CH(alink) ((alink)->mlmeextpriv.chandef.chan)
#define ALINK_GET_BW(alink) ((alink)->mlmeextpriv.chandef.bw)
#define ALINK_GET_OFFSET(alink) ((alink)->mlmeextpriv.chandef.offset)

struct _ADAPTER {
	int	pid[3];/*process id from UI, 0:wpa_supplicant, 1:hostapd, 2:dhcpcd*/

	/*extend to support multi interface*/
	u8 iface_id;
	u8 isprimary; /* is primary adapter or not */
	/* notes:
	**	if isprimary is true, the adapter_type value is 0, iface_id is IFACE_ID0 for PRIMARY_ADAPTER
	**	if isprimary is false, the adapter_type value is 1, iface_id is IFACE_ID1 for VIRTUAL_ADAPTER
	**	refer to iface_id if iface_nums>2 and isprimary is false and the adapter_type value is 0xff.*/
	u8 adapter_type;/*be used in  Multi-interface to recognize whether is PRIMARY_ADAPTER  or not(PRIMARY_ADAPTER/VIRTUAL_ADAPTER) .*/

	u8 mac_addr[ETH_ALEN];
	/*CONFIG_PHL_ARCH*/
	struct rtw_wifi_role_t *phl_role;
	ATOMIC_T need_tsf_sync_done;

#ifdef CONFIG_HWSIM
	int bup_hwsim;
#endif

	u8 netif_up;

	u8 registered;
	u8 ndev_unregistering;

	struct _ADAPTER_LINK *adapter_link[RTW_RLINK_MAX];
	u8 adapter_link_num;

	struct dvobj_priv *dvobj;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	struct xmit_priv xmitpriv;
	struct recv_info recvinfo;/*rssi*/
	struct sta_priv	stapriv;
	struct security_priv securitypriv;
	_lock   security_key_mutex; /* add for CONFIG_IEEE80211W, none 11w also can use */
	struct registry_priv	registrypriv;

	#ifdef CONFIG_RTW_80211K
	struct rm_priv	rmpriv;
	#endif

	#ifdef CONFIG_MP_INCLUDED
	struct mp_priv	mppriv;
	#endif

	#ifdef CONFIG_AP_MODE
	struct hostapd_priv	*phostapdpriv;
	u8 bmc_tx_rate;
	#ifdef CONFIG_AP_CMD_DISPR
	struct ap_cmd_dispr_priv *apcmd_dipsr_priv;
	u32 ap_stop_cmd_token;
	enum phl_band_idx ap_start_cmd_bidx;
	enum phl_band_idx ap_stop_cmd_bidx;
	u8 ap_start_cmd_state;
	u8 ap_stop_cmd_state;
	struct phl_cmd_token_req add_del_sta_req;
	_lock ap_add_del_sta_lock;
#define ADD_DEL_STA_ST_IDLE		1
#define ADD_DEL_STA_ST_REQUESTING	2
#define ADD_DEL_STA_ST_ACQUIRED		3
	u32 ap_add_del_sta_cmd_token;
	u8 ap_add_del_sta_cmd_state;
	#endif
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	u8 b2u_flags_ap_src;
	u8 b2u_flags_ap_fwd;
	#endif
	#endif/*CONFIG_AP_MODE*/

	u32	setband;
	ATOMIC_T bandskip;

	#ifdef CONFIG_P2P
	struct wifidirect_info	wdinfo;
	#endif /* CONFIG_P2P */

	#ifdef CONFIG_TDLS
	struct tdls_info	tdlsinfo;
	#endif /* CONFIG_TDLS */

	#ifdef CONFIG_WFD
	struct wifi_display_info wfd_info;
	#endif /* CONFIG_WFD */

	#ifdef CONFIG_RTW_NAPI
	struct	napi_struct napi;
	u8	napi_state;
	#endif

	#ifdef CONFIG_GPIO_API
	u8	pre_gpio_pin;
	struct gpio_int_priv {
		u8 interrupt_mode;
		u8 interrupt_enable_mask;
		void (*callback[8])(u8 level);
	} gpiointpriv;
	#endif

	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	_thread_hdl_ cmdThread;
	#endif

	#if 0 /*def CONFIG_XMIT_THREAD_MODE*/
	_thread_hdl_ xmitThread;
	#endif
	#ifdef CONFIG_RECV_THREAD_MODE
	_thread_hdl_ recvThread;
	#endif

	#ifdef PLATFORM_LINUX
	#ifdef CONFIG_IOCTL_CFG80211
	struct cfg80211_roch_info cfg80211_rochinfo;
	#endif /* CONFIG_IOCTL_CFG80211 */

	_nic_hdl pnetdev;
	char old_ifname[IFNAMSIZ];

	/* used by rtw_rereg_nd_name related function */
	struct rereg_nd_name_data {
		_nic_hdl old_pnetdev;
		char old_ifname[IFNAMSIZ];
		u8 old_ips_mode;
		u8 old_bRegUseLed;
	} rereg_nd_name_priv;

	struct net_device_stats stats;
	struct iw_statistics iwstats;
	struct proc_dir_entry *dir_dev;/* for proc directory */
	struct proc_dir_entry *dir_odm;

	#ifdef CONFIG_IOCTL_CFG80211
	struct wireless_dev *rtw_wdev;
	struct rtw_wdev_priv wdev_data;

	#endif /* CONFIG_IOCTL_CFG80211 */

	#endif /* PLATFORM_LINUX */

	#ifdef CONFIG_TX_AMSDU
	u8 tx_amsdu;
	u16 tx_amsdu_rate;
	#endif

	#ifdef CONFIG_RTW_WDS
	bool use_wds; /* for STA, AP mode */

	/* for STA mode */
	struct rtw_wds_gptr_table *wds_gpt_records;
	ATOMIC_T wds_gpt_record_num;

	/* for AP mode */
	#ifdef CONFIG_AP_MODE
	struct rtw_wds_table *wds_paths;
	ATOMIC_T wds_path_num;
	#endif
	#endif /* CONFIG_RTW_WDS */

	#ifdef CONFIG_RTW_MULTI_AP
	u8 multi_ap;
	u8 ch_util_threshold;
	#endif

	#ifdef CONFIG_RTW_MESH
	struct rtw_mesh_cfg mesh_cfg;
	struct rtw_mesh_info mesh_info;
	_timer mesh_path_timer;
	_timer mesh_path_root_timer;
	_timer mesh_atlm_param_req_timer; /* airtime link metrics param request timer */
	_workitem mesh_work;
	unsigned long wrkq_flags;
	#endif /* CONFIG_RTW_MESH */

	#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	ATOMIC_T tbtx_tx_pause;
	ATOMIC_T tbtx_remove_tx_pause;
	u8 	tbtx_capability;
	u32	tbtx_duration;
	#endif /* CONFIG_RTW_TOKEN_BASED_XMIT */

	#ifdef CONFIG_WAPI_SUPPORT
	u8	WapiSupport;
	RT_WAPI_T wapiInfo;
	#endif

	#ifdef CONFIG_BR_EXT
	_lock				br_ext_lock;
	/* unsigned int			macclone_completed; */
	struct nat25_network_db_entry	*nethash[NAT25_HASH_SIZE];
	int				pppoe_connection_in_progress;
	unsigned char			pppoe_addr[MACADDRLEN];
	unsigned char			scdb_mac[MACADDRLEN];
	unsigned char			scdb_ip[4];
	struct nat25_network_db_entry	*scdb_entry;
	unsigned char			br_mac[MACADDRLEN];
	unsigned char			br_ip[4];

	struct br_ext_info			ethBrExtInfo;
	#endif /* CONFIG_BR_EXT */

	#if 0 /*#ifdef CONFIG_MAC_LOOPBACK_DRIVER*/
	PLOOPBACKDATA ploopback;
	#endif

	#ifdef PLATFORM_FREEBSD
	_nic_hdl pifp;
	_lock glock;
	#endif /* PLATFORM_FREEBSD */

	/* for debug purpose */
#define NO_FIX_RATE		0xFFFF
#define GET_FIX_RATE(v)		((v) & 0x0FFF)
#define GET_FIX_RATE_SGI(v)	(((v) & 0x7000) >> 12)
	u16 fix_rate;
#define NO_FIX_BW		0xFF
	u8 fix_bw;
	u8 data_fb; /* data rate fallback, valid only when fix_rate is not 0xffff */
	u8 power_offset;
	u8 driver_tx_bw_mode;
	u8 rsvd_page_offset;
	u8 rsvd_page_num;
	u8 ch_clm_ratio;
	u8 ch_nhm_ratio;
	#ifdef CONFIG_SUPPORT_FIFO_DUMP
	u8 fifo_sel;
	u32 fifo_addr;
	u32 fifo_size;
	#endif

	u8 bLinkInfoDump;
	/*	Added by Albert 2012/10/26 */
	/*	The driver will show up the desired channel number when this flag is 1. */
	u8 bNotifyChannelChange;
	u8 bsta_tp_dump;
	#ifdef CONFIG_P2P
	/*	Added by Albert 2012/12/06 */
	/*	The driver will show the current P2P status when the upper application reads it. */
	u8 bShowGetP2PState;
	#endif
	u8 driver_vcs_en; /* Enable=1, Disable=0 driver control vrtl_carrier_sense for tx */
	u8 driver_vcs_type;/* force 0:disable VCS, 1:RTS-CTS, 2:CTS-to-self when vcs_en=1. */
	u8 driver_ampdu_spacing;/* driver control AMPDU Density for peer sta's rx */
	u8 driver_rx_ampdu_factor;/* 0xff: disable drv ctrl, 0:8k, 1:16k, 2:32k, 3:64k; */
	u8 driver_rx_ampdu_spacing;  /* driver control Rx AMPDU Density */
	u8 fix_rx_ampdu_accept;
	u8 fix_rx_ampdu_size; /* 0~127, TODO:consider each sta and each TID */

	#ifdef DBG_RX_COUNTER_DUMP
	u8 dump_rx_cnt_mode;/*BIT0:drv,BIT1:mac,BIT2:phy*/
	u32 drv_rx_cnt_ok;
	u32 drv_rx_cnt_crcerror;
	u32 drv_rx_cnt_drop;
	#endif

	#ifdef CONFIG_DBG_COUNTER
	struct rx_logs rx_logs;
	struct tx_logs tx_logs;
	struct int_logs int_logs;
	#endif

#ifdef RTW_PHL_DBG_CMD

	struct core_logs core_logs;
	struct phl_logs phl_logs;

	u32 txForce_enable;
	u32 txForce_rate;
	u32 txForce_agg;
	u32 txForce_aggnum;
	u32 txForce_gi;

	u32 sniffer_enable;
	u8	record_enable;
#endif
#ifdef RTW_PHL_TX
	u8 *pxmit_txreq_buf;
	_queue	free_txreq_queue;
	u32 free_txreq_cnt;
	u32 txreq_full_cnt;

	u32 tx_ring_idx;
	u8 *tx_pool_ring [MAX_TX_RING_NUM];
#endif
#ifdef CONFIG_RTW_CORE_RXSC
	u8 enable_rxsc;
#endif
#ifdef DBG_CONFIG_CMD_DISP
	enum phl_cmd_type cmd_type;
	u32 cmd_timeout;
#endif
	u8 dis_turboedca;/*	1: disable turboedca,
					2. disable turboedca,and setting EDCA parameter based on the input parameter*/
	u32 edca_param_mode;
	u32 last_edca;
	u8 edca_reset;/*1:be reset 0:not reset*/
#ifdef CONFIG_STA_CMD_DISPR
	_lock connect_st_lock;
	u8 connect_state;
#define CONNECT_ST_NOT_READY	0
#define CONNECT_ST_IDLE		1
#define CONNECT_ST_REQUESTING	2
#define CONNECT_ST_ACQUIRED	3
	bool connect_abort;
	struct phl_cmd_token_req connect_req;
	u32 connect_token;
	enum phl_band_idx connect_bidx;/*connect band idx*/

	_lock disconnect_lock;
	struct phl_cmd_token_req disconnect_req;
	u32 disconnect_token;
	enum phl_band_idx disconnect_bidx;/*disconnect band idx*/
	struct cmd_obj *discon_cmd;
#endif /* CONFIG_STA_CMD_DISPR */

#ifdef CONFIG_ECSA_PHL
	struct core_ecsa_info ecsa_info;
#endif
};

#define adapter_to_dvobj(adapter) ((adapter)->dvobj)
#define adapter_to_regsty(adapter) dvobj_to_regsty(adapter_to_dvobj((adapter)))
#define adapter_to_pwrctl(adapter) dvobj_to_pwrctl(adapter_to_dvobj((adapter)))
#ifdef CONFIG_WOWLAN
#define adapter_to_wowlan(adapter) dvobj_to_wowlan(adapter_to_dvobj((adapter)))
#endif /* CONFIG_WOWLAN */

#define adapter_wdev_data(adapter) (&((adapter)->wdev_data))
#define adapter_to_wiphy(adapter) dvobj_to_wiphy(adapter_to_dvobj(adapter))

#define adapter_to_rfctl(adapter) dvobj_to_rfctl(adapter_to_dvobj((adapter)))
#define adapter_to_macidctl(adapter) dvobj_to_macidctl(adapter_to_dvobj((adapter)))

#ifdef CONFIG_RTW_WDS
#define adapter_use_wds(adapter) (adapter->use_wds)
#define adapter_set_use_wds(adapter, en) do { \
		(adapter)->use_wds = (en) ? 1 : 0; \
		RTW_INFO(FUNC_ADPT_FMT" set use_wds=%d\n", FUNC_ADPT_ARG(adapter), (adapter)->use_wds); \
	} while (0)
#else
#define adapter_use_wds(adapter) 0
#endif

#define adapter_mac_addr(adapter) (adapter->mac_addr)
#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
#define adapter_pno_mac_addr(adapter) \
	((adapter_wdev_data(adapter))->pno_mac_addr)
#endif

#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
#define adapter_pno_mac_addr(adapter) \
	((adapter_wdev_data(adapter))->pno_mac_addr)
#endif

#define adapter_to_chset(adapter) (&adapter_to_rfctl((adapter))->chset)

#define mlme_to_adapter(mlme) container_of((mlme), _adapter, mlmepriv)
#define tdls_info_to_adapter(tdls) container_of((tdls), _adapter, tdlsinfo)

#define rtw_get_chip_id(adapter) (((_adapter *)adapter)->dvobj->chip_id)
#define rtw_get_intf_type(adapter) (((_adapter *)adapter)->dvobj->interface_type)

#define rtw_get_mi_nums(adapter) (((_adapter *)adapter)->dvobj->iface_nums)

static inline void dev_set_surprise_removed(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->bSurpriseRemoved, _TRUE);
	if (dvobj->phl)
		rtw_phl_dev_terminate_ntf(dvobj->phl);
}
static inline void dev_clr_surprise_removed(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->bSurpriseRemoved, _FALSE);
	if (dvobj->phl_com)
		CLEAR_STATUS_FLAG(dvobj->phl_com->dev_state, RTW_DEV_SURPRISE_REMOVAL);
}
static inline void dev_set_drv_stopped(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->bDriverStopped, _TRUE);
}
static inline void dev_clr_drv_stopped(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->bDriverStopped, _FALSE);
}
static inline void dev_set_hw_start(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->hw_start, _TRUE);
}
static inline void dev_clr_hw_start(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->hw_start, _FALSE);
}

#define dev_is_surprise_removed(dvobj)	(ATOMIC_READ(&dvobj->bSurpriseRemoved) == _TRUE)
#define dev_is_drv_stopped(dvobj)		(ATOMIC_READ(&dvobj->bDriverStopped) == _TRUE)
#define dev_is_hw_start(dvobj)		(ATOMIC_READ(&dvobj->hw_start) == _TRUE)
/*
 * Function disabled.
 *   */
#define DF_TX_BIT		BIT0			/*rtw_usb_write_port_cancel*/
#define DF_RX_BIT		BIT1			/*rtw_usb_read_port_cancel*/
#define DF_IO_BIT		BIT2

/* #define RTW_DISABLE_FUNC(padapter, func) (ATOMIC_ADD(&dvobj->disable_func, (func))) */
/* #define RTW_ENABLE_FUNC(padapter, func) (ATOMIC_SUB(&dvobj->disable_func, (func))) */
__inline static void RTW_DISABLE_FUNC(struct dvobj_priv *dvobj, int func_bit)
{
	int df = ATOMIC_READ(&dvobj->disable_func);
	df |= func_bit;
	ATOMIC_SET(&dvobj->disable_func, df);
}

__inline static void RTW_ENABLE_FUNC(struct dvobj_priv *dvobj, int func_bit)
{
	int df = ATOMIC_READ(&dvobj->disable_func);
	df &= ~(func_bit);
	ATOMIC_SET(&dvobj->disable_func, df);
}

#define RTW_CANNOT_RUN(dvobj) \
	(dev_is_surprise_removed(dvobj) || \
	dev_is_drv_stopped(dvobj))

#define RTW_IS_FUNC_DISABLED(dvobj, func_bit) \
	(ATOMIC_READ(&dvobj->disable_func) & (func_bit))

#define RTW_CANNOT_IO(dvobj) \
	(dev_is_surprise_removed(dvobj) || \
	 RTW_IS_FUNC_DISABLED((dvobj), DF_IO_BIT))

#define RTW_CANNOT_RX(dvobj) \
	(RTW_CANNOT_RUN(dvobj) || \
	 RTW_IS_FUNC_DISABLED((dvobj), DF_RX_BIT))

#define RTW_CANNOT_TX(dvobj) \
	(RTW_CANNOT_RUN(dvobj) || \
	 RTW_IS_FUNC_DISABLED((dvobj), DF_TX_BIT))


/* HCI Related header file */
#ifdef CONFIG_USB_HCI
	#include <usb_ops.h>
#endif

#ifdef CONFIG_SDIO_HCI
	#include <sdio_ops.h>
#endif

#ifdef CONFIG_GSPI_HCI
	#include <gspi_ops.h>
#endif

#ifdef CONFIG_PCI_HCI
	#include <pci_ops.h>
#endif
#include <rtw_trx_ops.h>


#endif /* __DRV_TYPES_H__ */
