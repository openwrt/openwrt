/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#ifndef _PHL_DEF_H_
#define _PHL_DEF_H_

enum phl_packet_type {
	PACKET_BEACON,
	PACKET_PROBE_REQUEST_ML,
	PACKET_PROBE_RESPONSE,
	PACKET_PROBE_RESPONSE_ML,
	PACKET_ASSOC_REQUEST,
	PACKET_ASSOC_RESPONSE,
	PACKET_AUTH,
	PACKET_MAX
};

/*HW_BAND0 - CMAC0 + PHY0 + S0*/
/*HW_BAND1 - CMAC1 + PHY1 + S1*/
enum phl_band_idx {
	HW_BAND_0,
	HW_BAND_1,
	HW_BAND_MAX
};

#define RTW_RLINK_MAX (HW_BAND_MAX)
#define RTW_ONE_LINK (1)
#define RTW_RLINK_PRIMARY (0)

/*
 * Refer to 802.11 spec (BE D1.3).
 * The description of Link ID subfield reserves 15 for if the reported AP is
 * not part of an AP MLD, or if the reporting AP does not have that information.
 * Do not misuse with role->rlink_num or mld->sta_num.
 */
#define MAX_MLD_LINK_NUM 15

enum rtw_device_type {
	DEV_TYPE_INACTIVE,
	DEV_TYPE_LEGACY,
	DEV_TYPE_MLD
};

/*wifi_role->hw_port*/
enum phl_hw_port {
	HW_PORT0,
	HW_PORT1,
	HW_PORT2,
	HW_PORT3,
	HW_PORT4,
	HW_PORT_MAX,
};

enum phl_hwamsdu_max_len {
	HWAMSDU_MAX_LEN_2048,
	HWAMSDU_MAX_LEN_3824,
	HWAMSDU_MAX_LEN_7920,
	HWAMSDU_MAX_LEN_11384,
	HWAMSDU_MAX_LEN_4024,
	HWAMSDU_MAX_LEN_5120,
	HWAMSDU_MAX_LEN_6656,
	HWAMSDU_MAX_LEN_10240,
};

#define WMM_AC_TID_NUM 8
#define RTW_MAX_TID_NUM 16
#define RTW_MAX_AC_QUEUE_NUM 4

#ifdef CONFIG_PCI_HCI
#define RTW_MAX_WP_RPT_AC_NUM(a) rtw_hal_query_txch_num(a)
#else
#define RTW_MAX_WP_RPT_AC_NUM(a) RTW_MAX_AC_QUEUE_NUM
#endif

enum phl_ac_queue {
	PHL_BE_QUEUE_SEL		= 0,
	PHL_BK_QUEUE_SEL		= 1,
	PHL_VI_QUEUE_SEL		= 2,
	PHL_VO_QUEUE_SEL		= 3,
	PHL_AC_QUEUE_TOTAL
};

enum phl_stat_info_query {
	STAT_INFO_FA_ALL,
	STAT_INFO_CCA_ALL,
};

/* Refer to B_AX_RX_CNT_IDX in R_AX_RX_DBG_CNT_SEL */
enum phl_rxcnt_idx {
	RXCNT_FULLDRP_PKT = 33,
};

enum phl_lifetime_queue {
	PHL_LIFETIME_MGQ,
	PHL_LIFETIME_ACQ,
	PHL_LIFETIME_MAX
};

#ifdef CONFIG_VW_REFINE
#define _CMD_RESET_VW_CNT 0
#define _CMD_DUMP_VW_CNT  1
#endif

#define PHL_MACID_MAX_ARRAY_NUM 8 /* 8x32=256 */
#define PHL_MACID_MAX_NUM (PHL_MACID_MAX_ARRAY_NUM * 32)

/**
 * struct rtw_chan_def - channel defination
 * @chan: the (control/primary) channel
 * @center_ch: the center channel
 * @bw: channel bandwidth
 * @center_freq1: center frequency of first segment
 * @center_freq2: center frequency of second segment
 *	(only with 80+80 MHz)
 */

struct rtw_chan_def {
	enum band_type band; /* protocol -2.4G,5G,6G*/
	u8 chan; /*primary channel*/
	enum channel_width bw;
	enum chan_offset offset;
	u8 center_ch;
	u16 hw_value;
	u32 center_freq1;
	u32 center_freq2;
};

struct chg_opch_param {
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_chan_def new_chdef;
	struct rtw_chan_def ori_chdef;
	enum rtw_phl_status cmd_start_sts;
	void (*chg_opch_done)(void *priv,
	                      u8 ridx,
	                      struct rtw_wifi_role_link_t *rlink,
	                      enum rtw_phl_status status);
};

struct rtw_amsdu_tx_param {
	u8 macid;
	u8 enable;
	enum phl_hwamsdu_max_len amsdu_max_len;
	u8 qos_field_h;
	u8 qos_field_h_en;
	u8 mhdr_len;
	u8 vlan_tag_valid;

	/* can add more data in this structure */
};

/**
 * struct rtw_chan_ctx - channel context
 * @list:
 * @chan_ctx_lock:
 * @chan_def:
 */
struct rtw_chan_ctx {
	_os_list list;
	struct rtw_chan_def chan_def;
	u8 role_map; /*used role_idx*/
	bool dfs_enabled;
};


#ifdef CONFIG_PCI_HCI
struct rtw_pci_info {
	u8 dummy;
};
#endif


#ifdef CONFIG_USB_HCI
struct rtw_usb_info {
	enum rtw_usb_speed usb_speed; /* USB 1.1, 2.0 or 3.0 */
	u16 usb_bulkout_size;
	u8 outep_num;
	u8 inep_num;
};

enum phl_usb_rx_agg_mode {
	PHL_RX_AGG_DISABLE,
	PHL_RX_AGG_DEFAULT,
	PHL_RX_AGG_SMALL_PKT,
	PHL_RX_AGG_USER_DEFINE,
};
/*
 * refers to _usb.h
 * #define SWITCHMODE           0x2
 * #define FORCEUSB3MODE        0x1
 * #define FORCEUSB2MODE        0x0
*/
enum rtw_usb_sw_ability {
	RTW_USB2_ONLY = 0,
	RTW_USB3_ONLY,
	RTW_USB_SUPPORT_SWITCH,
	RTW_USB_SUPPORT_MAX
};
#endif

#ifdef CONFIG_SDIO_HCI
struct rtw_sdio_info {
	unsigned int clock;
	unsigned int timing;
	u8 sd3_bus_mode;
	u16 block_sz;
	u16 io_align_sz;
	u16 tx_align_sz;
	bool tx_512_by_byte_mode;	/* Send 512 bytes by cmd53 byte or */
					/* block mode. */
};
#endif

struct rtw_ic_info {
	enum rtl_ic_id ic_id;
	enum rtw_hci_type hci_type;
	#ifdef CONFIG_SDIO_HCI
	struct rtw_sdio_info sdio_info;
	#endif

	#ifdef CONFIG_USB_HCI
	struct rtw_usb_info usb_info;
	#endif

	#ifdef CONFIG_PCI_HCI
	struct rtw_pci_info pci_info;
	#endif
};

enum rtw_proc_cmd_type {
	RTW_PROC_CMD_UNKNOW,
	RTW_PROC_CMD_BB,	/* 1 */
	RTW_PROC_CMD_RF,	/* 2 */
	RTW_PROC_CMD_MAC,	/* 3 */
	RTW_PROC_CMD_PHL,	/* 4 */
	RTW_PROC_CMD_CORE,	/* 5 */
	RTW_PROC_CMD_BTC,	/* 6 */
	RTW_PROC_CMD_EFUSE,	/* 7 */
	RTW_PROC_CMD_MAX
};

enum rtw_arg_type {
	RTW_ARG_TYPE_UNKNOW,
	RTW_ARG_TYPE_BUF,	/* 1 */
	RTW_ARG_TYPE_ARRAY,	/* 2 */
	RTW_ARG_TYPE_MAX
};

#define		MAX_ARGC	20
#define		MAX_ARGV	16


struct rtw_proc_cmd {
	enum rtw_arg_type in_type;
	u32 in_cnt_len;
	union {
		char *buf;
		char vector[MAX_ARGC][MAX_ARGV];
	}in;
};

enum rtw_para_src {
	RTW_PARA_SRC_INTNAL, /* 0 */
	RTW_PARA_SRC_EXTNAL, /* 1 */
	RTW_PARA_SRC_EXTNAL_BUF, /* 2 */
	RTW_PARA_SRC_CUSTOM, /* 3 */
	RTW_PARA_SRC_MAX
};

struct rtw_para_info_t {
	enum rtw_para_src para_src;
	char para_path[256];
	char *hal_phy_folder;
	char postfix[33];

	u8 *ext_para_file_buf;
	u32 ext_para_file_buf_len;
	u32 para_data_len;
	u32 *para_data;
};

#define regd_name_max_size 32

#define RTW_PHL_EXT_REG_MATCH_NONE	0
#define RTW_PHL_EXT_REG_MATCH_DOMAIN	BIT0
#define RTW_PHL_EXT_REG_MATCH_COUNTRY	BIT1

struct rtw_para_pwrlmt_info_t {
	enum rtw_para_src para_src;
	char para_path[256];
	char *hal_phy_folder;
	char postfix[33];

	u8 *ext_para_file_buf;
	u32 ext_para_file_buf_len;
	u32 para_data_len;
	u32 *para_data;

	char ext_regd_name[regd_name_max_size][10];
	u16 ext_regd_arridx;
	u16 ext_reg_map_num;
	u8 *ext_reg_codemap;
};

#define RTW_PHL_HANDLER_STATUS_INITIALIZED BIT0
#define RTW_PHL_HANDLER_STATUS_SET BIT1
#define RTW_PHL_HANDLER_STATUS_RELEASED BIT2

#define RTW_PHL_HANDLER_PRIO_HIGH 0
#define RTW_PHL_HANDLER_PRIO_NORMAL 1
#define RTW_PHL_HANDLER_PRIO_LOW 2

#define RTW_PHL_HANDLER_CB_NAME_LEN 32

enum rtw_phl_evt {
	RTW_PHL_EVT_RX = BIT0,
	RTW_PHL_EVT_TX_RECYCLE = BIT1,

	RTW_PHL_EVT_MAX = BIT31
};

enum rtw_phl_config_int {
	RTW_PHL_STOP_RX_INT,
	RTW_PHL_RESUME_RX_INT,
	RTW_PHL_SER_HANDSHAKE_MODE,
	RTW_PHL_EN_HCI_INT,
	RTW_PHL_DIS_HCI_INT,
	RTW_PHL_CLR_HCI_INT,
	RTW_PHL_CONFIG_INT_MAX
};

#ifdef CONFIG_RTW_OS_HANDLER_EXT
enum rtw_phl_handler_id {
	RTW_PHL_UNKNOWN_HANDLER, /* Unset, unknown */
	RTW_PHL_TX_HANDLER, /* PHL TX */
	RTW_PHL_RX_HANDLER, /* PHL RX */
	RTW_PHL_EVT_HANDLER, /* Core RX */
	#ifdef CONFIG_PHL_TEST_SUITE
        RTW_PHL_TEST_HANDLER, /* MP Test */
	#endif
};
#endif /* CONFIG_RTW_OS_HANDLER_EXT */

/**
 * phl_handler - scheduled by core layer or phl itself
 * and the properties is assigned by different hanlder type
 * @status: handler current status defined by RTW_PHL_HANDLER_STATUS_XXX
 * @type: define different properties of handler - tasklet, thread, workitem
 * @handle: store different type of handler structure
 * @callback: handler callback function
 * @context: context used in handler callback function
 */
struct rtw_phl_handler {
	char status;
	char type;
	void *drv_priv;
	struct _os_handler os_handler;
	void (*callback)(void *context);
	char cb_name[RTW_PHL_HANDLER_CB_NAME_LEN];
	void *context;
	#ifdef CONFIG_RTW_OS_HANDLER_EXT
	enum rtw_phl_handler_id	id;
	#endif /* CONFIG_RTW_OS_HANDLER_EXT */
};

struct rtw_xmit_req;
struct rtw_aoac_report;
struct tx_local_buf;
struct rtw_phl_evt_ops {
	enum rtw_phl_status (*rx_process)(void *drv_priv);
	enum rtw_phl_status (*tx_recycle)(void *drv_priv, struct rtw_xmit_req *txreq);
	enum rtw_phl_status (*tx_test_recycle)(void *phl, struct rtw_xmit_req *txreq);
	bool (*set_rf_state)(void *drv_priv, enum rtw_rf_state state_to_set);
	void (*wow_handle_sec_info_update)(void *drv_priv, struct rtw_aoac_report *aoac_info, u8 aoac_report_get_ok, u8 phase);
	void (*indicate_wake_rsn)(void *drv_priv, u8 rsn);
#ifdef CONFIG_SYNC_INTERRUPT
	void (*interrupt_restore)(void *drv_priv, u8 rx);
	void (*set_interrupt_caps)(void *drv_priv, u8 en);
#endif /* CONFIG_SYNC_INTERRUPT */
	void (*ap_ps_sta_ps_change)(void *drv_priv, u8 role_id, u8 *sta_mac,
	                            int power_save);
	bool (*issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps);

#ifdef CONFIG_VW_REFINE
	enum rtw_phl_status (*tx_dev_map)(void *phl, struct rtw_xmit_req *txreq);
#endif
	void (*os_query_local_buf)(void *priv, struct tx_local_buf *buf);
	void (*os_return_local_buf)(void *priv, struct tx_local_buf *buf);

	void (*tx_power_tbl_loaded)(void *drv_priv, bool target_loaded, bool limit_loaded);
};

/*
 * PHL CMD support direct execution, no-wait: synchronization, wait:asynchronization
 * PHL_CMD_CMD_DIRECTLY: call PHL API including I/O operation directly
 * PHL_CMD_NO_WARIT: send phl cmd msg to cmd dispatcher and do not wait for completion
 * PHL_CMD_WAIT: send phl cmd msg to cmd dispatcher and wait for completion
 */
enum phl_cmd_type {
	PHL_CMD_DIRECTLY,
	PHL_CMD_NO_WAIT,
	PHL_CMD_WAIT,
	PHL_CMD_MAX,
};

enum role_type {
	PHL_RTYPE_NONE,
	PHL_RTYPE_STATION,
	PHL_RTYPE_AP,
	PHL_RTYPE_VAP,
	PHL_RTYPE_ADHOC,
	PHL_RTYPE_ADHOC_MASTER,
	PHL_RTYPE_MESH,
	PHL_RTYPE_MONITOR,
	PHL_RTYPE_P2P_DEVICE,
	PHL_RTYPE_P2P_GC,
	PHL_RTYPE_P2P_GO,
	PHL_RTYPE_TDLS,
	PHL_RTYPE_NAN,
	PHL_MLME_MAX
};

enum role_state {
	PHL_ROLE_MSTS_STA_CONN_END,/*CORE*/
	PHL_ROLE_MSTS_STA_DIS_CONN,/*CORE*/
	PHL_ROLE_MSTS_AP_START,/*CORE*/
	PHL_ROLE_MSTS_AP_STOP,/*CORE*/
	PHL_ROLE_STATE_UNKNOWN,
};

enum link_state {
	/* config period */
	PHL_EN_LINK_START,
	PHL_EN_LINK_DONE,
	PHL_DIS_LINK_START,
	PHL_DIS_LINK_DONE,
	PHL_EN_DBCC_START,
	PHL_EN_DBCC_DONE,
	PHL_DIS_DBCC_START,
	PHL_DIS_DBCC_DONE,
	/* state */
	PHL_LINK_STARTED,
	PHL_LINK_STOPPED,
	PHL_ClIENT_JOINING, /* For AP Role, SUT be going to join to our AP Role.*/
	PHL_ClIENT_LEFT, /* For AP Role, SUT join to our AP Role*/
	PHL_LINK_UP_NOA,
	PHL_LINK_CHG_CH,
	PHL_LINK_UNKNOWN
};


enum mlme_state {
	MLME_NO_LINK,
	MLME_LINKING,
	MLME_LINKED
};
enum wr_chg_id {
	WR_CHG_TYPE,
	WR_CHG_MADDR,
	WR_CHG_AP_PARAM,
	WR_CHG_EDCA_PARAM,
	WR_CHG_MU_EDCA_PARAM,
	WR_CHG_MU_EDCA_CFG,
	WR_CHG_BSS_COLOR,
	WR_CHG_RTS_TH,
	WR_CHG_DFS_HE_TB_CFG,
	WR_CHG_TRX_PATH,
	WR_CHG_STBC_CFG,
	WR_CHG_MAX,
};

enum rlink_status{
	RLINK_STATUS_PS_ANN = BIT0,
	RLINK_STATUS_BCN_STOP = BIT1,
	RLINK_STATUS_TSF_SYNC = BIT2,
	RLINK_STATUS_MAX = BIT7
};

enum wr_status{
	WR_STATUS_SUSPEND = BIT0,
	WR_STATUS_HW_ALLOC_FAIL = BIT1,
	WR_STATUS_MAX = BIT7
};

enum rtw_cfg_type { /* sync with pcfg_type */
	CFG_TBTT_AGG,
	CFG_TBTT_SHIFT,
	CFG_HIQ_WIN,
	CFG_HIQ_DTIM,
	CFG_HIQ_MAX,
	CFG_BCN_INTERVAL,	/* Beacon Interval */
	CFG_BSS_CLR,
	CFG_BCN_DRP_ALL
};

struct rtw_ap_param {
	u32 cfg_id;
	u32 value;
};

struct rtw_edca_param {
	/* Access Category, 0:BE, 1:BK, 2:VI, 3:VO */
	u8 ac;
	/*
	 * EDCA parameter
	 * |31...16|15...12|11...8|7...0|
	 * |   TXOP|  CWMAX| CWMIN| AIFS|
	 */
	u32 param;
};

struct rtw_mu_edca_param {
	u8 ac;
	u8 aifsn;
	u8 cw;
	u8 timer;
};

struct rtw_trx_path_param {
	enum rf_path tx;
	enum rf_path rx;
	u8 tx_nss;
	u8 rx_nss;
};

#define MAX_STORE_BCN_NUM 20
enum conf_lvl {
	CONF_LVL_NONE = 0,
	CONF_LVL_LOW,
	CONF_LVL_MID,
	CONF_LVL_HIGH
};

enum h_type {
	H_UPPER = 0,
	H_CNT,
	H_MAX
};

struct rtw_bcn_offset {
	u16 offset; /*TU*/
	enum conf_lvl conf_lvl; /*confidence level*/
	u16 cr_tbtt_shift; /* CR current setting */
};

#define H_MAX_TYPE H_MAX
#define BIN_WIDTH 5
#define MAX_NUM_BIN 6
struct rtw_bcn_histogram {
	u16 num;
	u16 h_array[H_MAX_TYPE][MAX_NUM_BIN];
};

struct rtw_bcn_dist {
	bool calc_fail;
	struct rtw_bcn_histogram hist_i;
	u16 min;
	u16 max;
	u16 outlier_num;
	u16 outlier_l;
	u16 outlier_h;
};

struct rtw_bcn_tracking_cfg {
	u16 macid;
	u16 bcn_timeout;
};

#define MAX_SHORT_INFO_BCN_NUM 40
#define BCN_SHORT_TYPE SHORT_BCN_MAX

enum bcn_short_type {
	SHORT_BCN_MOD = 0,
	SHORT_BCN_RSSI,
	SHORT_BCN_MAX
};

struct rtw_bcn_short_i {
	u16 min;
	u16 max;
	u8 idx;
	u8 num;
	u16 info[BCN_SHORT_TYPE][MAX_SHORT_INFO_BCN_NUM];
};

/*
 * Store rx bcn tsf info
 * @num: the store noumber of "info" array
 * @idx: store current index of "info" array
 * @num_per_watchdog: counter the rx bcn each watchdog.
 * @info: store array. info[0]: store tsf, info[1]: store mod(TU), info[2]: store hw rx time, info[3]: current sym time
 * @offset_i: Bcn offset info. Dont't access directionly this variable for application.
                   You can get offset_i info from phl_get_sta_bcn_offset_info.
 */
struct rtw_rx_bcn_info {
	u16 pkt_len;
	enum rtw_data_rate rate;
	u8 last_num_per_watchdog;
	u8 num_per_watchdog;
	/* short info of bcn, store simple information */
	struct rtw_bcn_short_i bcn_s_i;
	/* long bcn info start, store full information */
	u8 idx;
	u8 num;
	u64 info[4][MAX_STORE_BCN_NUM];
	/* long bcn info end */
	struct rtw_bcn_offset offset_i;
	struct rtw_bcn_dist bcn_dist_i; /* beacon distribution info */
	struct rtw_bcn_tracking_cfg cfg; /* Suggest setting*/
};

struct rtw_bcn_pkt_info {
	struct rtw_phl_stainfo_t *sta;
	u64 tsf;
	u64 hw_tsf;
	u8 rssi;
	u16 pkt_len;
	enum rtw_data_rate rate;
};

struct rtw_rts_threshold {
	u16 rts_time_th;
	u16 rts_len_th;
};

enum phl_module_id{
	/* 0 ~ 128 PHL background module starts from here*/
	/* 1,2,3 cmd controller section */
	PHL_BK_MDL_START = 0,
	PHL_MDL_PHY_MGNT = 1,
	PHL_MDL_TX = 2,
	PHL_MDL_RX = 3,

	/* above enum is fixed, add new module id from here*/
	/* 10 ~ 40 protocol, wifi role section*/
	PHL_BK_MDL_ROLE_START = 10,
	PHL_MDL_MRC = 10, /* Multi-Role Controller intead of STA/P2P role /NAN/AP*/
	PHL_MDL_SOUND = 11,

	PHL_BK_MDL_ROLE_END = 40,

	/* 41 ~ 70 mandatory background module section*/
	PHL_BK_MDL_MDRY_START = 41,
	PHL_MDL_POWER_MGNT = 41,
	PHL_MDL_SER = 42,

	PHL_BK_MDL_MDRY_END = 70,

	/* 70 ~ 127 optional background module section*/
	PHL_BK_MDL_OPT_START = 71,
	PHL_MDL_MR_COEX = 71, /* Multi-Role Coex */
	PHL_FUNC_MDL_TEST_MODULE = 72,
	PHL_MDL_BTC = 73,
	PHL_MDL_CUSTOM = 74,
	PHL_MDL_WOW = 75,
	PHL_MDL_LED = 76,
	PHL_MDL_GENERAL = 77,
	PHL_MDL_REGU = 78,
	PHL_BK_MDL_OPT_END = 127,

	/* Fixed BK MDL Max Value*/
	PHL_BK_MDL_END = 128,

	/* 129 ~ 256 PHL foreground module starts from here*/
	PHL_FG_MDL_START = 129,
	PHL_FG_MDL_SCAN = 130,
	PHL_FG_MDL_CONNECT = 131,
	PHL_FG_MDL_DISCONNECT = 132,
	PHL_FG_MDL_AP_START = 133,
	PHL_FG_MDL_AP_STOP = 134,
	PHL_FG_MDL_ECSA = 135,
	PHL_FG_MDL_AP_ADD_DEL_STA = 136,
	PHL_FG_MDL_END = 254,

	/* Fixed MDL Max Value*/
	PHL_MDL_ID_MAX = 255
};

/* General phl event id shall share this common enum definition
 * if definition of private events for a specific module is required,
 * please be sure to start its enum from PRIVATE_EVT_START(0x8000)
 */
enum phl_msg_evt_id {
	MSG_EVT_NONE = 0,
	MSG_EVT_PHY_ON = 1,
	MSG_EVT_PHY_IDLE = 2,
	MSG_EVT_SCAN_START = 3,
	MSG_EVT_SCAN_END = 4,
	MSG_EVT_CONNECT_START = 5,
	MSG_EVT_CONNECT_LINKED = 6,
	MSG_EVT_CONNECT_END = 7,
	MSG_EVT_SER_L1 = 8,
	MSG_EVT_SER_L2 = 9,
	MSG_EVT_FWDL_OK = 10,
	MSG_EVT_FWDL_FAIL = 11,
	MSG_EVT_HAL_INIT_OK = 12,
	MSG_EVT_HAL_INIT_FAIL = 13,
	MSG_EVT_MP_CMD_DONE = 14,
	/* wow */
	MSG_EVT_WOW_ENTER = 15,
	MSG_EVT_WOW_LEAVE = 16,
	MSG_EVT_WOW_WAKE_RSN = 17,
	MSG_EVT_BCN_RESEND = 18,
	MSG_EVT_DUMP_PLE_BUFFER = 19,
	MSG_EVT_MP_RX_PHYSTS = 20,
	MSG_EVT_ROLE_NTFY = 21,
	MSG_EVT_RX_PSTS = 22,
	MSG_EVT_SWCH_START = 23,
	MSG_EVT_SWCH_DONE = 24,
	MSG_EVT_DISCONNECT_PREPARE = 25,
	MSG_EVT_DISCONNECT = 26,
	MSG_EVT_DISCONNECT_END = 27,
	MSG_EVT_TSF_SYNC_DONE = 28,
	MSG_EVT_TX_RESUME = 29,
	MSG_EVT_AP_START_PREPARE = 30,
	MSG_EVT_AP_START = 31,
	MSG_EVT_AP_START_END = 32,
	MSG_EVT_AP_STOP_PREPARE = 33,
	MSG_EVT_AP_STOP = 34,
	MSG_EVT_AP_STOP_END = 35,
	MSG_EVT_PCIE_TRX_MIT = 36,
	MSG_EVT_BTC_TMR = 37,
	MSG_EVT_BTC_FWEVNT = 38,
	MSG_EVT_BTC_REQ_BT_SLOT = 39,
	/* ser*/
	MSG_EVT_SER_L0_RESET = 41, /* L0 notify only */
	MSG_EVT_SER_M1_PAUSE_TRX = 42,
	MSG_EVT_SER_IO_TIMER_EXPIRE = 43,
	MSG_EVT_SER_FW_TIMER_EXPIRE = 44,
	MSG_EVT_SER_M3_DO_RECOV = 45,
	MSG_EVT_SER_M5_READY = 46,
	MSG_EVT_SER_M9_L2_RESET = 47,
	MSG_EVT_SER_EVENT_CHK = 48,
	MSG_EVT_SER_POLLING_CHK = 49,
	MSG_EVT_ECSA_START = 50,
	MSG_EVT_ECSA_UPDATE_FIRST_BCN_DONE = 51,
	MSG_EVT_ECSA_COUNT_DOWN = 52,
	MSG_EVT_ECSA_SWITCH_START = 53,
	MSG_EVT_ECSA_SWITCH_DONE = 54,
	MSG_EVT_ECSA_CHECK_TX_RESUME = 55,
	MSG_EVT_ECSA_DONE = 56,
	MSG_EVT_LISTEN_STATE_EXPIRE = 57,
	/* beamform */
	MSG_EVT_SET_VHT_GID = 58,
	MSG_EVT_HW_WATCHDOG = 59,
	MSG_EVT_DEV_CANNOT_IO = 60,
	MSG_EVT_DEV_RESUME_IO = 61,
	MSG_EVT_FORCE_USB_SW = 62,
	MSG_EVT_GET_USB_SPEED = 63,
	MSG_EVT_GET_USB_SW_ABILITY = 64,
	MSG_EVT_CFG_AMPDU = 65,
	MSG_EVT_DFS_PAUSE_TX = 66,
	MSG_EVT_ROLE_RECOVER = 67,
	MSG_EVT_ROLE_SUSPEND = 68,
	MSG_EVT_HAL_SET_L2_LEAVE = 69,
	MSG_EVT_NOTIFY_HAL = 70,
	MSG_EVT_ISSUE_BCN = 71,
	MSG_EVT_FREE_BCN = 72,
	MSG_EVT_STOP_BCN = 73,
	MSG_EVT_SEC_KEY = 74,
	MSG_EVT_ROLE_START = 75,
	MSG_EVT_ROLE_CHANGE = 76,
	MSG_EVT_ROLE_STOP = 77,
	MSG_EVT_STA_INFO_CTRL = 78,
	MSG_EVT_STA_MEDIA_STATUS_UPT = 79,
	MSG_EVT_CFG_CHINFO = 80,
	MSG_EVT_STA_CHG_STAINFO = 81,
	MSG_EVT_HW_TRX_RST_RESUME = 82,
	MSG_EVT_HW_TRX_PAUSE = 83,
	MSG_EVT_SW_TX_RESUME = 84,
	MSG_EVT_SW_RX_RESUME = 85,
	MSG_EVT_SW_TX_PAUSE = 86,
	MSG_EVT_SW_RX_PAUSE = 87,
	MSG_EVT_SW_TX_RESET = 88,
	MSG_EVT_SW_RX_RESET = 89,
	MSG_EVT_TRX_SW_PAUSE = 90,
	MSG_EVT_TRX_SW_RESUME = 91,
	MSG_EVT_TRX_PAUSE_W_RST = 92,
	MSG_EVT_TRX_RESUME_W_RST = 93,
	/* Regulation*/
	MSG_EVT_REGU_SET_DOMAIN = 94,
	MSG_EVT_RF_ON = 95,
	MSG_EVT_RF_OFF = 96,
	MSG_EVT_WPS_PRESSED = 97,
	MSG_EVT_WPS_RELEASED = 98,
	MSG_EVT_SURPRISE_REMOVE = 99,
	MSG_EVT_DATA_PATH_START = 100,
	MSG_EVT_DATA_PATH_STOP = 101,
	MSG_EVT_TRX_PWR_REQ = 102,
	/* tdls */
	MSG_EVT_TDLS_SYNC = 103,
	/* beamformee */
	MSG_EVT_SET_BFEE_AID = 104,

	/* ccx */
	MSG_EVT_CCX_REPORT_TX_OK = 105,
	MSG_EVT_CCX_REPORT_TX_FAIL = 106,

	/* bfer sounding */
	MSG_EVT_BFER_SOUND = 107,

	/* MRC DBCC */
	MSG_EVT_DBCC_PROTOCOL_HDL = 108,/*TODO-useless delete it*/
	MSG_EVT_DBCC_ENABLE = 109,
	MSG_EVT_DBCC_DISABLE = 110,

	/* PS */
	MSG_EVT_PS_CAP_CHG = 111,
	MSG_EVT_PS_PERIOD_CHK = 112,
	MSG_EVT_PS_DBG_CMD = 113,
	MSG_EVT_PS_LPS_ENTER = 114,
	MSG_EVT_PS_LPS_LEAVE = 115,

	MSG_EVT_ASSOCIATED_TSF_ERROR = 116,
	/* Change operating ch def(ch / bw) */
	MSG_EVT_CHG_OP_CH_DEF_START = 117,
	MSG_EVT_CHG_OP_CH_DEF_END = 118,

	MSG_EVT_GET_CUR_TSF = 119,

	/* MACID pause and drop */
	MSG_EVT_SET_MACID_PAUSE = 120,
	MSG_EVT_SET_MACID_PKT_DROP = 121,

	/* beacon */
	MSG_EVT_BCN_CNT_RPT = 122,
	MSG_EVT_CSA_COUNTDOWN_ZERO = 123,

	/* gtimer */
	MSG_EVT_GT3_SETUP = 124,

	/* hw seq */
	MSG_EVT_HW_SEQ_SETUP = 125,

	/* TWT */
	MSG_EVT_TWT_STA_ACCEPT = 126,
	MSG_EVT_TWT_STA_TEARDOWN = 127,
	MSG_EVT_TWT_GET_TWT = 128,
	MSG_EVT_TWT_WAIT_ANNOUNCE = 129,
	/* TWT end */

	/* Lifetime */
	MSG_EVT_LIFETIME_SETUP = 130,

	MSG_EVT_MDL_CHECK_STOP = 131,

	/* TPU Power Offset */
	MSG_EVT_POWER_OFFSET_SETUP = 132,

	/* RF state by GPIO */
	MSG_EVT_HW_RF_CHG = 133,
	/* update tpe info */
	MSG_EVT_TPE_INFO_UPDATE = 134,
	/* Beacon early report */
	MSG_EVT_BCN_EARLY_REPORT = 135,

	/* DFS radar detecting*/
	MSG_EVT_DFS_RD_IS_DETECTING = 136,
	MSG_EVT_DFS_RD_SETUP = 137,

	/* UL fixinfo */
	MSG_EVT_SET_UL_FIXINFO = 138,

	MSG_EVT_FPGA_CMD_DONE = 139,
	MSG_EVT_TX_PKT_NTFY = 140,
	MSG_EVT_SET_STA_SEC_IV = 141,
	MSG_EVT_SER_L2_RESET_DONE = 142,

	/* HW AMSDU */
	MSG_EVT_CFG_AMSDU_TX = 143,
	MSG_EVT_SW_WATCHDOG = 144,

	/* hw sequence */
	MSG_EVT_HWSEQ_GET_HW_SEQUENCE = 145,

	/* Rx debug count */
	MSG_EVT_RX_DBG_CNT_GET = 146,
	MSG_EVT_RX_DBG_CNT_GET_BY_IDX = 147,
	MSG_EVT_RX_DBG_CNT_RESET = 148,

	/* USB rx agg paramters config */
	MSG_EVT_USB_RX_AGG_CFG = 149,

	/* tx power */
	MSG_EVT_TXPWR_SETUP = 150,
	MSG_EVT_GET_TX_PWR_DBM = 151,
	MSG_EVT_UPDT_EXT_TXPWR_LMT = 152,

	/* HW CTS2Self setting */
	MSG_EVT_HW_CTS2SELF = 153,

	/* dbg */
	MSG_EVT_DBG_SIP_REG_DUMP = 200,
	MSG_EVT_DBG_FULL_REG_DUMP = 201,
	MSG_EVT_DBG_L2_DIAGNOSE = 202,
	MSG_EVT_DBG_RX_DUMP = 203,
	MSG_EVT_DBG_TX_DUMP = 204,
	/* dbg end */
	/* p2pps */
	MSG_EVT_TSF32_TOG = 210,
	MSG_EVT_NOA_DISABLE = 211,
	MSG_EVT_NOA_UP = 212,
	/* p2pps end */
	/* MCC */
	MSG_EVT_MCC_START = 220,
	MSG_EVT_MCC_STOP = 221,
	/* MCC end */
	MSG_EVT_PKT_EVT_NTFY = 230,
	MSG_EVT_CSI_TX_RESULT = 231,
	MSG_EVT_BCN_TSF_REPORT = 232,

	/* sub module IO */
	MSG_EVT_NOTIFY_BB = 300,
	MSG_EVT_NOTIFY_RF = 301,
	MSG_EVT_NOTIFY_MAC = 302,
	/* sub module IO end*/
	/* scan ofld */
	MSG_EVT_SCANOFLD = 400,
	MSG_EVT_SCANOFLD_START = 401,
	MSG_EVT_SCANOFLD_END = 402,
	MSG_EVT_SCANOFLD_SWITCH = 403,
	MSG_EVT_SCANOFLD_CHKPT_TIMER = 404,

	/* Tx rpt */
	MSG_EVT_USR_TX_RPT = 410,

	/* Add EVT-ID for linux core cmd temporality */
	MSG_EVT_LINUX_CMD_WRK = 888,
	MSG_EVT_LINUX_CMD_WRK_TRI_PS = 889,
	/* LED */
	MSG_EVT_LED_TICK = 5000,
	MSG_EVT_LED_MANUAL_CTRL = 5001,
	MSG_EVT_LED_EVT_START = 5002,
	MSG_EVT_LED_EVT_END = 5050,
	MSG_EVT_MAX = 0x7fff
};

struct rtw_scanofld_rsp {
	u8 pri_ch;
	u8 notify_reason;
	u8 status;
	u8 actual_period;
	u8 band;
	u8 hw_band;
};

enum phl_msg_recver_layer {
	MSG_RECV_PHL = 0,
	MSG_RECV_CORE = 1,
	MSG_RECV_MAX
};

enum phl_msg_indicator {
	MSG_INDC_PRE_PHASE = BIT0,
	MSG_INDC_FAIL = BIT1,
	MSG_INDC_CANCEL = BIT2,
	MSG_INDC_CANNOT_IO = BIT3,
	MSG_INDC_CLR_SNDR = BIT4
};

enum phl_msg_opt {
	MSG_OPT_SKIP_NOTIFY_OPT_MDL = BIT0,
	MSG_OPT_BLIST_PRESENT = BIT1,
	MSG_OPT_CLR_SNDR_MSG_IF_PENDING = BIT2,
	MSG_OPT_SEND_IN_ABORT = BIT3,
	MSG_OPT_PENDING_DURING_CANNOT_IO = BIT4,
};


/* all module share this common enum definition */
enum phy_bk_module_opcode {
	BK_MODL_OP_NONE = 0,
	BK_MODL_OP_INPUT_CMD,
	BK_MODL_OP_CHK_NEW_MSG,
	BK_MODL_OP_STATE,
	BK_MODL_OP_CUS_FEATURE_SET,
	BK_MODL_OP_CUS_FEATURE_QUERY,
	BK_MODL_OP_MAX
};

/* Foreground cmd token opcode */
enum phy_fg_cmd_req_opcode {
	FG_REQ_OP_NONE = 0,
	FG_REQ_OP_GET_ROLE,
	FG_REQ_OP_GET_ROLE_LINK,
	FG_REQ_OP_GET_MDL_ID,

	FG_REQ_OP_NOTIFY_BCN_RCV,
#ifdef RTW_WKARD_CMD_SCAN_EXTEND_ACTION_FRAME_TX
	FG_REQ_OP_NOTIFY_ACTION_FRAME_TX,
#endif
	FG_REQ_OP_MAX
};

enum fg_cmd_opt {
	FG_CMD_OPT_EXCLUSIVE = BIT0,
};

/* priority of phl background
module which would be considered when dispatching phl msg*/
enum phl_bk_module_priority {
	PHL_MDL_PRI_ROLE = 0,
	PHL_MDL_PRI_OPTIONAL,
	PHL_MDL_PRI_MANDATORY,
	PHL_MDL_PRI_MAX
};

enum phl_data_ctl_cmd {
	PHL_DATA_CTL_HW_TRX_RST_RESUME = 1,
	PHL_DATA_CTL_HW_TRX_PAUSE = 2,
	PHL_DATA_CTL_SW_TX_RESUME = 3,
	PHL_DATA_CTL_SW_RX_RESUME = 4,
	PHL_DATA_CTL_SW_TX_PAUSE = 5,
	PHL_DATA_CTL_SW_RX_PAUSE = 6,
	PHL_DATA_CTL_SW_TX_RESET = 7,
	PHL_DATA_CTL_SW_RX_RESET = 8,
	PHL_DATA_CTL_TRX_SW_PAUSE = 9,
	PHL_DATA_CTL_TRX_SW_RESUME = 10,
	PHL_DATA_CTL_TRX_PAUSE_W_RST = 11,
	PHL_DATA_CTL_TRX_RESUME_W_RST = 12,
	PHL_DATA_CTL_MAX = 0xFF
};

/**
 * phl_msg - define a general msg format for PHL/CORE layer module to handle
 * one can easily extend additional mgnt info by encapsulating inside a file
 * refer to
 *		struct phl_msg_ex 		in phl_msg_hub.c
 *		struct phl_dispr_msg_ex		in phl_cmd_dispatcher.c
 *
 * @msg_id: indicate msg source & msg type
 *	    BYTE 3: RSVD
 *	    BYTE 2: PHL Module ID,  refer to enum phl_module_id
 *	    BYTE 0-1: event id, refer to enum phl_msg_evt_id
 * @inbuf: input buffer that sent along with msg
 * @inlen: input buffer length
 * @outbuf: output buffer that returned after all phl modules have recved msg.
 * @outlen: output buffer length
 * @band_idx: index of Band(PHY) which associate to this msg

 * @rsvd: feature reserved, passing object pointer.
 *        For example,
 *        - cmd_scan : [0]: wifi_role.
 *        - CANNOT_IO error: [0]: mdl handle.
 */
union _ext_info {
	void *ptr;
	long value;
};

struct phl_msg{
	u32 msg_id;
	enum phl_band_idx band_idx;
	u8* inbuf;
	u8* outbuf;
	u32 inlen;
	u32 outlen;
	union _ext_info rsvd[4];
};

struct msg_notify_map {
	u8* id_arr;
	u8 len;
};
struct msg_dispatch_seq {
	struct msg_notify_map map[PHL_MDL_PRI_MAX];
};
struct msg_self_def_seq {
	struct msg_dispatch_seq pre_prot_phase;
	struct msg_dispatch_seq post_prot_phase;
};
struct msg_completion_routine {
	void* priv;
	void (*completion)(void* priv, struct phl_msg* msg);
};
/**
 * phl_msg_attribute: used in phl_disp_eng_send_msg
 * @opt: refers to enum phl_msg_opt.
 * @notify: input id array (refer to enum phl_module_id)
 * 	    for indicating additional dependency
 * @completion: completion routine
 */
struct phl_msg_attribute {
	u8 opt;
	struct msg_notify_map notify;
	struct msg_completion_routine completion;
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	void *dispr_attr;
#endif
};

/**
 * phl_module_op_info - set by core layer or phl itself,
 * op code process is an synchronous process.
 * which would be handled directly by module handler
 * @op_code: refer to enum phy_module_opcode
 * @inbuf: input buffer that sent along with msg
 * @inlen: input buffer length
 * @outbuf: output buffer that returned after all phy modules have recved msg.
 * @outlen: output buffer length
 */
struct phl_module_op_info{
	u32 op_code;
	u8* inbuf;
	u8* outbuf;
	u32 inlen;
	u32 outlen;
};

/**
 * phl_cmd_token_req - request foramt for applying token of a specific cmd
 * dispatcher.
 * cmd token request is regarded as foreground module, thus,
 * need to contend for cmd token.
 * Normally, these req would be linked to a specific wifi role
 * and acquiring RF resource for a specific task.
 *
 * @module_id: starting from PHL_FG_MDL_START
 * @opt: refers to enum fg_cmd_opt.
 * @priv: private context from requestor
 * @role: designated role info associated with current request.
 * -----------------------------------------
 * regarding on "return code" for following ops, refer to enum phl_mdl_ret_code
 * -----------------------------------------
 * @acquired: notify requestor when cmd token has acquired for this cmd and
              cannot have any I/O operation.
 * @abort: notify requestor when cmd has been canceled
           after calling rtw_phl_phy_cancel_token_req and
           cannot have any I/O operation.
 * @msg_hdlr: notify requestor about incoming msg.
 * @set_info: notify requestor to handle specific op code.
 * @query_info: notify requestor to handle specific op code.
 */

struct phl_cmd_token_req{
	u8 module_id;
	u8 opt;
	void* priv;
	void* role;
	enum phl_mdl_ret_code (*acquired)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*abort)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*msg_hdlr)(void* dispr, void* priv,
							struct phl_msg* msg);
	enum phl_mdl_ret_code (*set_info)(void* dispr, void* priv,
					struct phl_module_op_info* info);
	enum phl_mdl_ret_code (*query_info)(void* dispr, void* priv,
				struct phl_module_op_info* info);
};

/**
 * phl_module_ops - standard interface for interacting with a cmd dispatcher.
 * -----------------------------------------
 * regarding on "return code" for following ops, refer to enum phl_mdl_ret_code
 * -----------------------------------------
 * @init: notify module for initialization.
 * @deinit: notify module for de-initialization.
 * @start: notify module to start.
 * @stop: notify module to stop.
 * @msg_hdlr: notify module about incoming msg.
 * @set_info: notify module to handle specific op code.
 * @query_info: notify module to handle specific op code.
 */
struct phl_bk_module_ops {
	enum phl_mdl_ret_code (*init)(void* phl_info, void* dispr, void** priv);
	void (*deinit)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*start)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*stop)(void* dispr, void* priv);
	enum phl_mdl_ret_code (*msg_hdlr)(void* dispr, void* priv,
							struct phl_msg* msg);
	enum phl_mdl_ret_code (*set_info)(void* dispr, void* priv,
			struct phl_module_op_info* info);
	enum phl_mdl_ret_code (*query_info)(void* dispr, void* priv,
			struct phl_module_op_info* info);
};

/**
 * phl_data_ctl_t - datapath control parameters for dispatcher controller
 * @cmd: data path control command
 * @id: module id which request data path control
 */
struct phl_data_ctl_t {
	enum phl_data_ctl_cmd cmd;
	enum phl_module_id id;
};

#define MSG_MDL_ID_FIELD(_msg_id) (((_msg_id) >> 16) & 0xFF)
#define MSG_EVT_ID_FIELD(_msg_id) ((_msg_id) & 0xFFFF)
#define MSG_INDC_FIELD(_msg_id) (((_msg_id) >> 24) & 0xFF)
#define IS_PRIVATE_MSG(_msg_id) ((_msg_id) & PRIVATE_EVT_START)
#define IS_MSG_FAIL(_msg_id) ((_msg_id) & ( MSG_INDC_FAIL <<  24))
#define IS_MSG_IN_PRE_PHASE(_msg_id) ((_msg_id) & ( MSG_INDC_PRE_PHASE <<  24))
#define IS_MSG_CANCEL(_msg_id) ((_msg_id) & ( MSG_INDC_CANCEL <<  24))
#define IS_MSG_CANNOT_IO(_msg_id) ((_msg_id) & ( MSG_INDC_CANNOT_IO <<  24))
#define IS_MSG_CLR_SNDR(_msg_id) ((_msg_id) & ( MSG_INDC_CLR_SNDR <<  24))

#define SET_MSG_MDL_ID_FIELD(_msg_id, _id) \
	((_msg_id) = (((_msg_id) & 0xFF00FFFF) | ((u32)(_id) << 16)))
#define SET_MSG_EVT_ID_FIELD(_msg_id, _id) \
	((_msg_id) = (((_msg_id) & 0xFFFF0000) | (_id)))
#define SET_MSG_INDC_FIELD(_msg_id, _indc) \
	((_msg_id) = (((_msg_id) & ~((u32)(_indc) << 24))|((u32)(_indc) << 24)))
#define CLEAR_MSG_INDC_FIELD(_msg_id, _indc) ((_msg_id) &= ~((_indc) << 24))

#if !defined(RTW_MAX_FW_SIZE)
#define RTW_MAX_FW_SIZE 0x100000
#endif

enum rtw_fw_src {
	RTW_FW_SRC_INTNAL, /* 0 */
	RTW_FW_SRC_EXTNAL, /* 1 */
	RTW_FW_SRC_MAX
};

enum rtw_fw_rsn {
	RTW_FW_RSN_INIT, /* 0 */
	RTW_FW_RSN_SPIC, /* 1 */
	RTW_FW_RSN_LPS, /* 2 */
	RTW_FW_RSN_MCC, /* 3 */
	RTW_FW_RSN_WOW, /* 4 */
	RTW_FW_RSN_MAX
};

struct rtw_fw_cap_t {
	enum rtw_fw_src fw_src;
	u32 offload_cap;
	u8 dlram_en;
	u8 dlrom_en;
};

#define INVALID_WIFI_ROLE_IDX MAX_WIFI_ROLE_NUMBER
#define UNSPECIFIED_ROLE_ID 0xFF
#define MAX_SECCAM_NUM_PER_ENTRY 7

#define RX_BUF_SIZE 11460 /* Rx buffer size (without RXBD info length) to 8 byte alignment  by DD suggestion  */

/* Role Link hw TX CAP */
struct role_link_cap_t {
	enum wlan_mode wmode;
	enum channel_width bw;
	u8 rty_lmt;		/* retry limit for DATA frame, 0xFF: invalid */
	u8 rty_lmt_rts;		/* retry limit for RTS frame, 0xFF: invalid */

	u8 tx_num_ampdu;
	u8 tx_amsdu_in_ampdu; /*from SW & HW*/
	u8 tx_ampdu_len_exp; /*from  SW & HW*/
	u8 tx_htc;
	u8 tx_sgi;

	u8 tx_ht_ldpc:1;
	u8 tx_vht_ldpc:1;
	u8 tx_he_ldpc:1;
};

struct role_sw_cap_t {
	u16 bf_cap; /* use define : HW_CAP_BFER_XX_XX */
	u16 stbc_cap;/* use define: HW_CAP_STBC_XX */
};

/*
Protocol - RX CAP from 80211 PKT,
driver TX related function need to
reference __rx__ of rtw_phl_stainfo_t->asoc_cap
*/
struct protocol_cap_t {
	/* MAC related */
	u16 bcn_interval;	/* beacon interval */
	u16 num_ampdu;
	u8 ampdu_len_exp; /* rx ampdu cap */
	u8 ampdu_density:3;	/* rx ampdu cap */
	u8 amsdu_in_ampdu:1; /* rx ampdu cap */
	u8 max_amsdu_len:2; /* 0: 4k, 1: 8k, 2: 11k */
	u8 htc_rx:1;

	u8 sm_ps:2;
	u8 trig_padding:2;
	u8 twt:6;
	u8 all_ack:1;
	u8 a_ctrl:4;
	u8 ops:1;
	u8 ht_vht_trig_rx:1;
	u8 bsscolor;
	u16 rts_th:10;

	u8 short_slot:1;	/* Short Slot Time */

	u8 preamble:1;		/* Preamble, 0: long, 1: short */
	u8 sgi_20:1;		/* HT Short GI for 20 MHz */
	u8 sgi_40:1;		/* HT Short GI for 40 MHz */
	u8 sgi_80:1;		/* VHT Short GI for 80 MHz */
	u8 sgi_160:1;		/* VHT Short GI for 160/80+80 MHz */
	struct rtw_edca_param edca[4]; 	/* Access Category, 0:BE, 1:BK, 2:VI, 3:VO */
	u8 mu_qos_info;
	struct rtw_mu_edca_param mu_edca[4];

	u16 tid2link_ul[WMM_AC_TID_NUM]; /* EHT: Tid-to-Link, uplink */
	u16 tid2link_dl[WMM_AC_TID_NUM]; /* EHT: Tid-to-Link, downlink */

	/* BB related */
	u8 ht_ldpc:1;
	u8 vht_ldpc:1;
	u8 he_ldpc:1;
	u8 he_su_bfmr:1;
	u8 he_su_bfme:1;
	u8 he_mu_bfmr:1;
	u8 he_mu_bfme:1;
	u8 bfme_sts:3;
	u8 num_snd_dim:3;

	u8 ht_su_bfmr:1;
	u8 ht_su_bfme:1;
	u8 vht_su_bfmr:1;
	u8 vht_su_bfme:1;
	u8 vht_mu_bfmr:1;
	u8 vht_mu_bfme:1;
	u8 ht_vht_ng:2;
	u8 ht_vht_cb:2;
	/*
	 * supported_rates: Supported data rate of CCK/OFDM.
	 * The rate definition follow Wi-Fi spec, unit is 500kb/s,
	 * and the MSB(bit 7) represent basic rate.
	 * ex. CCK 2Mbps not basic rate is encoded as 0x04,
	 *     and OFDM 6M basic rate is encoded as 0x8c.
	 * Suppose rates come from Supported Rates and Extended Supported
	 * Rates IE.
	 * Value 0 means it is end of array, and no more valid data rate follow.
	 */
	u8 supported_rates[12];
	u8 ht_rx_mcs[4];
	u8 ht_tx_mcs[4];
	u8 ht_basic_mcs[4];	/* Basic rate of HT */
	u8 vht_rx_mcs[2];
	u8 vht_tx_mcs[2];
	u8 vht_basic_mcs[2];	/* Basic rate of VHT */
	u8 he_rx_mcs[6];/*80,160,80+80*/
	u8 he_tx_mcs[6];/*80,160,80+80*/
	u8 he_basic_mcs[2];	/* Basic rate of HE */
	u8 stbc_ht_rx:2;
	u8 stbc_vht_rx:3;
	u8 stbc_he_rx:1;
	u8 stbc_ht_tx:1;
	u8 stbc_vht_tx:1;
	u8 stbc_he_tx:1;
	u8 ltf_gi;
	u8 doppler_tx:1;
	u8 doppler_rx:1;
	u8 dcm_max_const_tx:2;
	u8 dcm_max_nss_tx:1;
	u8 dcm_max_const_rx:2;
	u8 dcm_max_nss_rx:1;
	u8 partial_bw_su_in_mu:1;
	u8 bfme_sts_greater_80mhz:3;
	u8 num_snd_dim_greater_80mhz:3;
	u8 stbc_tx_greater_80mhz:1;
	u8 stbc_rx_greater_80mhz:1;
	u8 ng_16_su_fb:1;
	u8 ng_16_mu_fb:1;
	u8 cb_sz_su_fb:1;
	u8 cb_sz_mu_fb:1;
	u8 trig_su_bfm_fb:1;
	u8 trig_mu_bfm_fb:1;
	u8 trig_cqi_fb:1;
	u8 partial_bw_su_er:1;
	u8 pkt_padding:2;
	u8 ppe_thr[8][4];
	u8 pwr_bst_factor:1;
	u8 max_nc:3;
	u8 dcm_max_ru:2;
	u8 long_sigb_symbol:1;
	u8 non_trig_cqi_fb:1;
	u8 tx_1024q_ru:1;
	u8 rx_1024q_ru:1;
	u8 fbw_su_using_mu_cmprs_sigb:1;
	u8 fbw_su_using_mu_non_cmprs_sigb:1;
	u8 er_su:1;
	u8 tb_pe:3;
	u16 txop_du_rts_th;
	u8 he_rx_ndp_4x32:1;

	/* RF related */
	u8 nss_tx:3;
	u8 nss_rx:3;

	/*backup section*/
	u8 stbc_ht_rx_bk:2;
	u8 stbc_vht_rx_bk:3;
	u8 stbc_he_rx_bk:1;
	u8 nss_rx_bk:3;
	/*
	u8 stbc_ht_tx_bk:1;
	u8 stbc_vht_tx_bk:1;
	u8 stbc_he_tx_bk:1;
	u8 nss_tx_bk:3;
	*/
	u16 num_ampdu_bk;
	u8 cap_option;

	u8 ppe_th_present:1;
	u8 er_su_gi_ltf; /* same define as u8 ltf_gi; */
};

#define EXT_CAP_LIMIT_2G_RX_STBC	BIT0


#define LOAD_MAC_REG_FILE				BIT0
#define LOAD_BB_PHY_REG_FILE			BIT1
#define LOAD_BB_PHY_REG_MP_FILE			BIT2
#define LOAD_RF_RADIO_FILE				BIT3
#define LOAD_RF_TXPWR_BY_RATE			BIT4
#define LOAD_RF_TXPWR_TRACK_FILE		BIT5
#define LOAD_RF_TXPWR_LMT_FILE			BIT6
#define LOAD_RF_TXPWR_LMT_RU_FILE		BIT7
#define LOAD_BB_PHY_REG_GAIN_FILE		BIT8
#define LOAD_RF_TXPWR_LMT_6G_FILE		BIT9
#define LOAD_RF_TXPWR_LMT_RU_6G_FILE		BIT10

#define PHL_UNDEFINED_SW_CAP 0xFF

struct rtw_pcie_ltr_lat_ctrl {
	enum rtw_pcie_bus_func_cap_t ctrl;
	u32 val;
};

enum rtw_pcie_ltr_state {
	RTW_PCIE_LTR_SW_ACT = 1,
	RTW_PCIE_LTR_SW_IDLE = 2
};

struct bus_sw_cap_t {
#ifdef CONFIG_PCI_HCI
	enum rtw_pcie_bus_func_cap_t l0s_ctrl;
	enum rtw_pcie_bus_func_cap_t l1_ctrl;
	enum rtw_pcie_bus_func_cap_t l1ss_ctrl;
	enum rtw_pcie_bus_func_cap_t wake_ctrl;
	enum rtw_pcie_bus_func_cap_t crq_ctrl;
	u16 txbd_num;
	u16 wd_num;
	u16 rxbd_num;
	u16 rpbd_num;
	u32 rxbuf_num;
	u32 rpbuf_num;
	u32 rxbuf_size;
	u32 rpbuf_size;
	u8 clkdly_ctrl;
	u8 l0sdly_ctrl;
	u8 l1dly_ctrl;
	struct rtw_pcie_ltr_lat_ctrl ltr_act;
	struct rtw_pcie_ltr_lat_ctrl ltr_idle;
	u8 ltr_init_state;
	u16 ltr_sw_ctrl_thre; /* [15:8] tx [7:0] rx */
	u8 ltr_sw_ctrl;
	u8 ltr_hw_ctrl;
#elif defined (CONFIG_USB_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 tx_h2c_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
	u32 in_token_num;
#elif defined(CONFIG_SDIO_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
#else
	u8 temp_for_struct_empty; /* for undefined interface */
#endif
};
struct bus_cap_t {
#ifdef CONFIG_PCI_HCI
	enum rtw_pcie_bus_func_cap_t l0s_ctrl;
	enum rtw_pcie_bus_func_cap_t l1_ctrl;
	enum rtw_pcie_bus_func_cap_t l1ss_ctrl;
	enum rtw_pcie_bus_func_cap_t wake_ctrl;
	enum rtw_pcie_bus_func_cap_t crq_ctrl;
	u16 txbd_num;
	u16 rxbd_num;
	u16 rpbd_num;
	u32 rxbuf_num;
	u32 rpbuf_num;
	u32 rxbuf_size;
	u32 rpbuf_size;
	u8 clkdly_ctrl;
	u8 l0sdly_ctrl;
	u8 l1dly_ctrl;
	struct rtw_pcie_ltr_lat_ctrl ltr_act;
	struct rtw_pcie_ltr_lat_ctrl ltr_idle;
	u8 ltr_init_state;
	u8 ltr_sw_ctrl;
	u8 ltr_hw_ctrl;
#elif defined (CONFIG_USB_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 tx_h2c_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
	u32 in_token_num;
#elif defined(CONFIG_SDIO_HCI)
	u32 tx_buf_size;
	u32 tx_buf_num;
	u32 tx_mgnt_buf_size;
	u32 tx_mgnt_buf_num;
	u32 rx_buf_size;
	u32 rx_buf_num;
#else
	u8 temp_for_struct_empty; /* for undefined interface */
#endif
};

#ifdef CONFIG_PHL_TWT

#define DELETE_ALL 0xFF
#define IGNORE_CFG_ID 0xFF
#define IGNORE_MACID 0xFF

enum rtw_phl_twt_sup_cap {
	RTW_PHL_TWT_REQ_SUP = BIT(0), /* REQUESTER */
	RTW_PHL_TWT_RSP_SUP = BIT(1)/* RESPONDER */
};

enum rtw_phl_nego_type {
	RTW_PHL_INDIV_TWT = 0, /*individual TWT*/
	RTW_PHL_WAKE_TBTT_INR = 1, /*wake TBTT and wake interval*/
	RTW_PHL_BCAST_TWT = 2, /*Broadcast TWT*/
	RTW_PHL_MANAGE_BCAST_TWT = 3 /*Manage memberships in broadcast TWT schedules*/
};

enum rtw_phl_wake_dur_unit{ /*wake duration unit*/
	RTW_PHL_WAKE_256US = 0,
	RTW_PHL_WAKE_1TU = 1
};

enum rtw_phl_setup_cmd{
	RTW_PHL_REQUEST_TWT = 0,
	RTW_PHL_SUGGEST_TWT = 1,
	RTW_PHL_DEMAND_TWT = 2,
	RTW_PHL_TWT_GROUPING = 3,
	RTW_PHL_ACCEPT_TWT = 4,
	RTW_PHL_ALTERNATE_TWT = 5,
	RTW_PHL_DICTATE_TWT = 6,
	RTW_PHL_REJECT_TWT = 7
};

enum rtw_phl_flow_type{
	RTW_PHL_ANNOUNCED_TWT = 0,
	RTW_PHL_UNANNOUNCED_TWT = 1
};

enum rtw_phl_twt_sta_action {
	TWT_STA_NONE = 0,
	TWT_STA_ADD_MACID = 1,
	TWT_STA_DEL_MACID = 2,
	TWT_STA_TETMINATW_SP = 3,
	TWT_STA_SUSPEND_TWT = 4,
	TWT_STA_RESUME_TWT = 5
};

enum rtw_phl_twt_cfg_action {
	TWT_CFG_ADD = 0,
	TWT_CFG_DELETE = 1,
	TWT_CFG_MODIFY = 2
};

enum phl_wait_annc_type {
	RTW_PHL_TWT_WAIT_ANNC_DISABLE = 0,
	RTW_PHL_TWT_WAIT_ANNC_ENABLE = 1
};

struct rtw_phl_twt_flow_type01 {
	u8 twt_flow_id;
	u8 teardown_all;
};

struct rtw_phl_twt_flow_type2 {
	u8 reserved;
};

struct rtw_phl_twt_flow_type3 {
	u8 bcast_twt_id;
	u8 teardown_all;
};

struct rtw_phl_twt_flow_field{
	enum rtw_phl_nego_type nego_type;
	union {
		struct rtw_phl_twt_flow_type01 twt_flow01;
		struct rtw_phl_twt_flow_type2 twt_flow2;
		struct rtw_phl_twt_flow_type3 twt_flow3;
	} info;
};

/*phl_twt_setup_info Start*/

/*Broadcast TWT Parameter Set field*/
struct rtw_phl_bcast_twt_para_set{
	u8 reserved; /*todo*/
};

/*Individual TWT Parameter Set field*/
struct rtw_phl_twt_group_asgmt{
	u8 reserved; /*todo*/
};

struct rtw_phl_req_type_indiv{
	enum rtw_phl_setup_cmd twt_setup_cmd; /*twt setup command*/
	enum rtw_phl_flow_type flow_type;
	u8 twt_request;
	u8 trigger;
	u8 implicit;
	u8 twt_flow_id;
	u8 twt_wake_int_exp;/*twt wake interval exponent*/
	u8 twt_protection;
};

struct rtw_phl_indiv_twt_para_set{
	struct rtw_phl_req_type_indiv req_type;
	struct rtw_phl_twt_group_asgmt twt_group_asgmt; /* twt group assignment*/
	u32 target_wake_t_h; /* if contain twt_group_assignment then don't contain target_wake_time*/
	u32 target_wake_t_l;
	u16 twt_wake_int_mantissa; /*twt wake interval mantissa*/
	u8 nom_min_twt_wake_dur; /*nominal minimum twt wake duration*/
	u8 twt_channel;
};

struct rtw_phl_twt_control{
	enum rtw_phl_nego_type nego_type; /*negotiation type*/
	enum rtw_phl_wake_dur_unit wake_dur_unit; /*wake duration unit*/
	u8 ndp_paging_indic; /*ndp paging indicator*/
	u8 responder_pm_mode;
	u8 twt_info_frame_disable; /*twt information frame disable*/
};
struct rtw_phl_twt_element{
/* element info*/
	/*control filed*/
	struct rtw_phl_twt_control twt_ctrl;
	/*twt para info*/
	union {
		struct rtw_phl_indiv_twt_para_set i_twt_para_set;
		struct rtw_phl_bcast_twt_para_set b_twt_para_set;
	} info;
};

struct rtw_phl_twt_setup_info{
	struct rtw_phl_twt_element twt_element;
	//struct rtw_phl_stainfo_t *phl_sta; //sta entry
	u8 dialog_token;
};
/*phl_twt_setup_info End*/


/*phl_twt_info Start*/
struct rtw_twt_sta_info{
	_os_list list;
	struct rtw_phl_stainfo_t *phl_sta; /*sta entry*/
	u8 id; /*twt_flow_identifier or broadcast_twt_id*/
};

struct rtw_phl_twt_info{
	enum rtw_phl_wake_dur_unit wake_dur_unit;
	enum rtw_phl_nego_type nego_type;
	enum rtw_phl_flow_type flow_type;
	u8 twt_id; /*config id*/
	u8 bcast_twt_id; /*ignore in individual TWT*/
	u8 twt_action;
	u8 responder_pm_mode;
	u8 trigger;
	u8 implicit_lastbcast; /*implicit or lastbroadcast*/
	u8 twt_protection;
	u8 twt_wake_int_exp;
	u8 nom_min_twt_wake_dur;
	u16 twt_wake_int_mantissa;
	u32 target_wake_time_h;
	u32 target_wake_time_l;
};

struct rtw_phl_twt_sta_accept_i {
	struct rtw_phl_stainfo_t *sta;
	struct rtw_phl_twt_setup_info setup_info;
	void (*accept_done)(void *priv, struct rtw_phl_stainfo_t *sta,
			    struct rtw_phl_twt_setup_info *setup_info,
			    enum rtw_phl_status sts);
};

struct rtw_phl_twt_sta_teardown_i{
	struct rtw_phl_stainfo_t *sta;
	struct rtw_phl_twt_flow_field twt_flow;
	void (*teardown_done)(void *priv, struct rtw_phl_stainfo_t *sta,
			      enum rtw_phl_status sts);
};

/*
 * @wrole: specific wrole
 * @id: reference id of twt configuration
 * @offset: unit: ms. An amount of time that you will start TWT from now
 * @tsf_h: high 4-byte value of target wake time
 * @tsf_l: low 4-byte value of target wake time
 */
struct rtw_phl_twt_get_twt_i{
	struct rtw_wifi_role_t *wrole;
	u8 id;
	u16 offset;
	u32 *tsf_h;
	u32 *tsf_l;
};

#define RTW_PHL_TWT_WAIT_ANNO_STA_NUM 3
struct rtw_phl_twt_wait_anno_rpt {
	enum phl_wait_annc_type wait_case;
	u8 macid[RTW_PHL_TWT_WAIT_ANNO_STA_NUM];
};
#endif /* CONFIG_PHL_TWT */


enum rtw_lps_listen_bcn_mode {
	RTW_LPS_RLBM_MIN         = 0,
	RTW_LPS_RLBM_MAX         = 1,
	RTW_LPS_RLBM_USERDEFINE  = 2,
	RTW_LPS_LISTEN_BCN_MAX,
};

enum rtw_lps_smart_ps_mode {
	RTW_LPS_LEGACY_PWR1      = 0,
	RTW_LPS_TRX_PWR0         = 1,
	RTW_LPS_SMART_PS_MAX,
};

struct  rtw_wow_cap_t {
	u8 magic_sup;
	u8 pattern_sup;
	u8 ping_pattern_wake_sup;
	u8 arp_ofld_sup;
	u8 ns_oflod_sup;
	u8 gtk_ofld_sup;
	u8 nlo_sup;
};

struct rtw_sec_cap {
	u8 hw_form_hdr:1;
	u8 hw_tx_search_key:1;
	u8 hw_sec_iv:1;
};

/**
 * enum phl_ps_leave_fail_act decide the action when leave ps fail
 * BIT 0 : reject all subsequent power request
 * BIT 1 : trigger L2 reset
 */
enum phl_ps_leave_fail_act {
	PS_LEAVE_FAIL_ACT_REJ_PWR = BIT0,
	PS_LEAVE_FAIL_ACT_L2 = BIT1,
	PS_LEAVE_FAIL_SER_L1_RECOVERY = BIT2
};
#define PS_LEAVE_FAIL_ACT_NONE 0

enum phl_ps_operation_mode {
	PS_OP_MODE_DISABLED = 0,
	PS_OP_MODE_FORCE_ENABLED = 1,
	PS_OP_MODE_AUTO = 2
};

enum phl_ps_pwr_lvl {
	PS_PWR_LVL_PWROFF = 0, /* hal deinit */
	PS_PWR_LVL_PWR_GATED = 1, /* FW control*/
	PS_PWR_LVL_CLK_GATED = 2, /* FW control*/
	PS_PWR_LVL_RF_OFF = 3, /* FW control*/
	PS_PWR_LVL_PWRON = 4, /* hal init */
	PS_PWR_LVL_MAX
};

/**
 * enum phl_stop_rson record the reason to stop power saving
 * BIT 0 : by core initialization setting
 * BIT 1 : by debug flow setting
 * BIT 2 : by battery change
 */
enum phl_ps_rt_rson {
	PS_RT_DEBUG = BIT0,
	PS_RT_CORE_INIT = BIT1,
	PS_RT_BATTERY_CHG = BIT2,
};
#define PS_RT_RSON_NONE 0

#define PS_CAP_PWRON BIT0
#define PS_CAP_RF_OFF BIT1
#define PS_CAP_CLK_GATED BIT2
#define PS_CAP_PWR_GATED BIT3
#define PS_CAP_PWR_OFF BIT4

enum phl_ps_defer_rson {
	PS_DEFER_PING_PKT = BIT0,
	PS_DEFER_DHCP_PKT = BIT1,
};
#define PS_DEFER_RSON_NONE 0

struct rtw_ps_defer_para {
	/* see phl_ps_defer_rson */
	u8 defer_rson;

	/* Tx ping defer time(ms) */
	u32 lps_ping_defer_time;
	/* Reserved for dhcp*/
	u32 lps_dhcp_defer_time;
};

enum rtw_lps_adv_cap {
	RTW_LPS_ADV_NONE = 0,
	RTW_LPS_ADV_PVB_W_RX = BIT0,
	RTW_LPS_ADV_MAX,
};

/**
 * ips_en/lps_en
 * refs. structure "phl_ps_operation_mode"
 * 0: disable -> disable all ps mechanism
 * 1: force enable -> ignore all other condition, force enter ps
 * 2: auto -> will be affected by runtime capability set by core
 *
 * ips_cap/ips_wow_cap/lps_cap/lps_wow_cap are bit defined
 * corresponding bit is set if specific power level is supported
 * BIT0: Power on
 * BIT1: Rf off
 * BIT2: Clock gating
 * BIT3: Power gating
 * BIT4: Power off
 */
struct rtw_ps_cap_t {
	/* rf state */
	enum rtw_rf_state init_rf_state;
	u8 init_rt_stop_rson;
	u8 leave_fail_act; /* action when leave ps fail */
	/* ips */
	u8 ips_en;
	u8 ips_cap;
	u8 ips_wow_en;
	u8 ips_wow_cap;
	/* lps */
	bool bcn_tracking;
	u8 lps_en;
	u8 lps_cap;
	u8 lps_awake_interval;
	enum rtw_lps_listen_bcn_mode lps_listen_bcn_mode;
	enum rtw_lps_smart_ps_mode lps_smart_ps_mode;
	u8 lps_bcnnohit_en;
	u8 lps_rssi_enter_threshold;
	u8 lps_rssi_leave_threshold;
	u8 lps_rssi_diff_threshold;

	struct rtw_ps_defer_para defer_para;
	enum rtw_lps_adv_cap lps_adv_cap;

	bool lps_pause_tx;
	/* wow lps */
	u8 lps_wow_en;
	u8 lps_wow_cap;
	u8 lps_wow_awake_interval;
	enum rtw_lps_listen_bcn_mode lps_wow_listen_bcn_mode;
	enum rtw_lps_smart_ps_mode lps_wow_smart_ps_mode;
	u8 lps_wow_bcnnohit_en;
};

struct rtw_edcca_cap_t {
	u8 edcca_adap_th_2g;
	u8 edcca_adap_th_5g;
	u8 edcca_cbp_th_6g;
	u8 edcca_carrier_sense_th;
};

enum rtw_rsc_cfg {
	RTW_RSC_CFG_PRI20 = 0,
	RTW_RSC_CFG_FOLLOW_RX = 1,
	RTW_RSC_CFG_DUPLICATE = 2,
};

struct rtw_rsc_mode {
	u8 rsc_2g : 2;
	u8 rsc_5g : 2;
	u8 rsc_6g : 2;
	u8 rsvd : 2;
};
struct phy_sw_cap_t {
	struct rtw_para_info_t mac_reg_info;
	struct rtw_para_info_t bb_phy_reg_info;
	struct rtw_para_info_t bb_phy_reg_mp_info;
	struct rtw_para_info_t bb_phy_reg_gain_info;

	struct rtw_para_info_t rf_radio_a_info;
	struct rtw_para_info_t rf_radio_b_info;
	struct rtw_para_info_t rf_txpwr_byrate_info;
	struct rtw_para_info_t rf_txpwrtrack_info;

	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_info;
	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_ru_info;

	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_6g_info;
	struct rtw_para_pwrlmt_info_t rf_txpwrlmt_ru_6g_info;

	u8 proto_sup;
	u8 band_sup;
	u8 bw_sup;
	u8 txss;
	u8 rxss;
	u8 tx_path_num;
	u8 rx_path_num;
	u16 hw_rts_time_th;
	u16 hw_rts_len_th;
	struct rtw_rsc_mode rsc_mode;
	bool bfreed_para;
	u32 txagg_num;
};

/* final capability of phy */
struct phy_cap_t {
	u8 proto_sup;
	u8 band_sup;
	u8 bw_sup;
	u8 txss;
	u8 rxss;
	u8 tx_path_num;
	u8 rx_path_num;
	u16 hw_rts_time_th;
	u16 hw_rts_len_th;
	u32 txagg_num;
};

/* final capability of device */
struct dev_cap_t {
	u64 hw_sup_flags;/*hw's feature support flags*/
#ifdef RTW_WKARD_LAMODE
	bool la_mode;
#endif
	u8 pkg_type;
	u8 rfe_type;
	u8 bypass_rfe_chk;
	u8 xcap;
	struct rtw_fw_cap_t fw_cap;
#ifdef CONFIG_MCC_SUPPORT
	bool mcc_sup;
#endif
#ifdef CONFIG_DBCC_SUPPORT
	bool dbcc_sup;
	bool mcmb_skip_dbcc;/*core prefer op mode in mcmb case*/
	bool mcmb_itf_ctrl;/*platform can handle interference by adjust antenna isolation, tx power or LNA or not*/

#ifdef CONFIG_DBCC_FORCE
	/* force band one
	    the format is bit map.
	    Ex:
	    dbcc_force_rmap = BIT(0)
	    We assign 0 of role ID to band one
	    dbcc_force_rmap = BIT(0) | BIT(1)
	     We assign 0 and 1 of role ID to band one
	*/
	u8 dbcc_force_rmap;
	/*HW_PHY_0 or HW_PHY_1*/
	u8 dbcc_force_cck_phyidx;
#endif
#endif
#ifdef CONFIG_PHL_TWT
	u8 twt_sup;
#endif /* CONFIG_PHL_TWT */

	struct rtw_wow_cap_t wow_cap;
	struct rtw_ps_cap_t ps_cap;
	struct rtw_sec_cap sec_cap;
	u8 hw_hdr_conv;
	u8 hw_amsdu;
	u8 domain;
	u8 domain_6g;
	u8 btc_mode;
	u8 ap_ps;           /* support for AP mode PS in PHL */
	u8 pwrbyrate_off;
	u8 pwrlmt_type;
	u8 rf_board_opt;
	u8 sta_ulru; /* support UL OFDAM for STA mode (reply trigger frame) */
#ifdef RTW_WKARD_BB_DISABLE_STA_2G40M_ULOFDMA
	u8 sta_ulru_2g40mhz; /* when "sta_ulru" is enabled, support UL OFDAM on 2.4G 40MHz ? */
#endif
	u8 tx_mu_ru;
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	bool bfree_para_info;	/* keep load file para info buf,default 0*/
#endif
	u8 hw_stype_cap;
	u8 wl_func_cap;
	struct rtw_edcca_cap_t edcca_cap;
	u32 rfk_cap;
	u8 sgi_160_sup;
	u8 rpq_agg_num; /* 0: no adjust, use mac default size: 121 */
	bool quota_turbo;
	u8 band_sup;
	u8 bw_sup;

	u16 macid_num;
	u8 max_link_num;
	u16 max_mld_num; /* support maximum MLO device */
	u8 drv_info_sup;
	u8 bfee_rx_ndp_sts; /* BFee receive maximum NDP STS number : 0 = 1sts, 1=2sts, ... */
	u32 uuid;
	/*CHANNEL_WIDTH_10 or CHANNEL_WIDTH_5 or 0(non 5/10M setting)*/
	u8 nb_config;
#ifdef CONFIG_PHL_THERMAL_PROTECT
	/* TX duty could be 0~100, 100 means no TX duty control */
	u8 min_tx_duty;
	u8 thermal_threshold;
#endif
};

#ifdef RTW_PHL_BCN //phl def

#define BCN_ID_MAX		(0xFF)
#define MAX_BCN_SIZE	1000

enum bcn_offload_flags{
	BCN_HW_SEQ = 0,
	BCN_HW_TIM,

	BCN_HW_MAX = 32,
};

struct rtw_bcn_info_cmn {
	u8 role_idx;
	u8 lidx;
	u8 bcn_id;
	u8 bcn_added;

	u8 bssid[6];
	u32 bcn_interval;

	u8 bcn_buf[MAX_BCN_SIZE];
	u32 bcn_length;
	u32 bcn_rate;

	u32 bcn_dtim;
	u32 ie_offset_tim;

#ifdef CONFIG_PHL_DFS_SWITCH_CH_WITH_CSA
	u32 offset_csa_cntdown;
#endif

	u32 bcn_offload;
};

struct rtw_bcn_info_hw {
	u8 band;
	u8 port;
	u8 mbssid;
	u8 mac_id;
	u32 hiq_win;
	u8 bss_color;
};

#ifdef CONFIG_RTW_DEBUG_BCN_TX

#ifdef CONFIG_BCN_DBG_LVL
	#define BCN_DBG_LVL CONFIG_BCN_DBG_LVL
#else
	#define BCN_DBG_LVL 0
#endif /* CONFIG_BCN_DBG_LVL */

enum bcn_dbg_lvl {
	BCN_DBG_LVL_ERROR = 0, /* Only print bcn rpt when all tx fail */
	BCN_DBG_LVL_WARN, /* Only print bcn rpt if tx fail rate more than 50% */
	BCN_DBG_LVL_TRACE /* Print bcn rpt every time */
};

struct rtw_bcn_stats {
	u32 bcn_ok;
	u32 bcn_fail;

    /* bcn_stats records */
	u32 ok_cnt;
	u32 cca_cnt;
	u32 edcca_cnt;
	u32 nav_cnt;
	u32 txon_cnt;
	u32 mac_cnt;
	u32 others_cnt;
	u32 lock_cnt;
	u32 cmp_cnt;
	u32 invalid_cnt;
	u32 srchend_cnt;
	u32 dbg_bcn_tx_cnt;
};
#endif

struct rtw_bcn_entry {
	_os_list list;
	struct rtw_bcn_info_cmn	*bcn_cmn;	//fill by core
	struct rtw_bcn_info_hw 	bcn_hw;	//fill by phl //?? void mapping ?? for 8852, 8834 ...blabla
#ifdef CONFIG_RTW_DEBUG_BCN_TX
	struct rtw_bcn_stats bcn_stats;
#endif
};
#endif

struct rtw_phl_com_t;

struct phl_msg_receiver {
		void* priv;
		void (*incoming_evt_notify)(void* priv, struct phl_msg *msg);
};

#ifdef CONFIG_PHL_P2PPS

#define MAX_NOA_DESC 5
#define NOAID_NONE 0xFF

enum p2pps_trig_tag {
	P2PPS_TRIG_GO = 0,
	P2PPS_TRIG_GC = 1,
	P2PPS_TRIG_GC_255 = 2,
	P2PPS_TRIG_MCC = 3,
	P2PPS_TRIG_2G_SCC_1AP_1STA_BT = 4,
	P2PPS_TRIG_MAX = MAX_NOA_DESC
};

struct rtw_phl_noa_desc {
	u8 enable; /*false=disable, true=enable*/
	struct rtw_wifi_role_t *w_role;
	struct rtw_wifi_role_link_t *rlink;
	enum p2pps_trig_tag tag;
	u32 start_t_h;
	u32 start_t_l;
	u32 interval;
	u32 duration;
	u8 count;
	u8 noa_id; /*filed by phl noa module*/
};

struct rtw_phl_opps_desc {
	u16 ctw;
	u8 all_slep;
};

struct rtw_phl_tsf32_tog_rpt{
	u8 band;
	u8 port;
	u8 valid;
	u16 early;
	u16 status;
	u32 tsf_l;
	u32 tsf_h;
};

struct rtw_phl_p2pps_ops {
	void *priv; /* ops private, define by core layer*/
	void (*tsf32_tog_update_noa)(void *priv, struct rtw_wifi_role_t *w_role,
					struct rtw_phl_tsf32_tog_rpt *rpt);
	void (*tsf32_tog_update_single_noa)(void *priv,
					struct rtw_wifi_role_t *w_role,
					struct rtw_phl_noa_desc *desc);
};
#endif

enum rtw_rxfltr_stype_ctrl {
	RXFLTR_STYPE_CTRL_RSVD_0000 = 0,
	RXFLTR_STYPE_CTRL_RSVD_0001,
	RXFLTR_STYPE_TRIGGER,
	RXFLTR_STYPE_TACK,
	RXFLTR_STYPE_BFRP,
	RXFLTR_STYPE_VHT_HE_NDPA,
	RXFLTR_STYPE_CFE,
	RXFLTR_STYPE_CW,
	RXFLTR_STYPE_BAR,
	RXFLTR_STYPE_BA,
	RXFLTR_STYPE_PS_POLL,
	RXFLTR_STYPE_RTS,
	RXFLTR_STYPE_CTS,
	RXFLTR_STYPE_ACK,
	RXFLTR_STYPE_CF_END,
	RXFLTR_STYPE_CF_END_ACK,
	RXFLTR_STYPE_CTRL_MAX
};

enum rtw_rxfltr_stype_mgnt {
	RXFLTR_STYPE_ASSOC_REQ = 0,
	RXFLTR_STYPE_ASSOC_RSP,
	RXFLTR_STYPE_REASSOC_REQ,
	RXFLTR_STYPE_REASSOC_RSP,
	RXFLTR_STYPE_PROBE_REQ,
	RXFLTR_STYPE_PROBE_RSP,
	RXFLTR_STYPE_TIMING_ADV,
	RXFLTR_STYPE_MGNT_RSVD_0111,
	RXFLTR_STYPE_BEACON,
	RXFLTR_STYPE_ATIM,
	RXFLTR_STYPE_DIS_ASSOC,
	RXFLTR_STYPE_AUTH,
	RXFLTR_STYPE_DEAUTH,
	RXFLTR_STYPE_ACTION,
	RXFLTR_STYPE_ACTION_NO_ACK,
	RXFLTR_STYPE_MGNT_RSVD_1111,
	RXFLTR_STYPE_MGNT_MAX
};

enum rtw_rxfltr_stype_data {
	RXFLTR_STYPE_DATA_DATA = 0,
	RXFLTR_STYPE_DATA_CF_ACK,
	RXFLTR_STYPE_DATA_CF_POLL,
	RXFLTR_STYPE_DATA_CF_ACK_POLL,
	RXFLTR_STYPE_NULL_DATA,
	RXFLTR_STYPE_CF_ACK,
	RXFLTR_STYPE_CF_POLL,
	RXFLTR_STYPE_CF_ACK_POLL,
	RXFLTR_STYPE_QOS_DATA,
	RXFLTR_STYPE_QOS_DATA_CF_ACK,
	RXFLTR_STYPE_QOS_DATA_CF_POLL,
	RXFLTR_STYPE_QOS_DATA_CF_ACK_POLL,
	RXFLTR_STYPE_QOS_NULL_DATA,
	RXFLTR_STYPE_DATA_RSVD_1101,
	RXFLTR_STYPE_QOS_CF_POLL,
	RXFLTR_STYPE_QOS_CF_ACK_POLL,
	RXFLTR_STYPE_DATA_MAX
};

enum rtw_rxfltr_target {
	RXFLTR_TARGET_DROP = 0,
	RXFLTR_TARGET_TO_HOST,
	RXFLTR_TARGET_TO_WLCPU,
	RXFLTR_TARGET_MAX
};

enum rtw_rxfltr_type_mode {
	RX_FLTR_TYPE_MODE_HAL_INIT = 0,
	RX_FLTR_TYPE_MODE_ROLE_INIT,
	RX_FLTR_TYPE_MODE_STA_CONN,
	RX_FLTR_TYPE_MODE_STA_DIS_CONN,
	RX_FLTR_TYPE_MODE_STA_WOW_INIT_PRE,
	RX_FLTR_TYPE_MODE_STA_WOW_INIT_POST,
	RX_FLTR_TYPE_MODE_AP_CONN,
	RX_FLTR_TYPE_MODE_AP_DIS_CONN,
#ifdef CONFIG_PHL_CHANNEL_INFO
	RX_FLTR_TYPE_MODE_CHAN_INFO_EN,
	RX_FLTR_TYPE_MODE_CHAN_INFO_DIS,
#endif
	RX_FLTR_TYPE_MODE_MAX
};

struct rxfltr_stype_cap_to_set {
	bool set;
	enum rtw_rxfltr_target target;
};

struct rxfltr_cap_to_set_ctrl {
	struct rxfltr_stype_cap_to_set stype[RXFLTR_STYPE_CTRL_MAX];
};

struct rxfltr_cap_to_set_mgnt {
	struct rxfltr_stype_cap_to_set stype[RXFLTR_STYPE_MGNT_MAX];
};

struct rxfltr_cap_to_set_data {
	struct rxfltr_stype_cap_to_set stype[RXFLTR_STYPE_DATA_MAX];
};

struct rxfltr_type_cap_to_set {
	struct rxfltr_cap_to_set_ctrl rxfltr_ctrl;
	struct rxfltr_cap_to_set_mgnt rxfltr_mgnt;
	struct rxfltr_cap_to_set_data rxfltr_data;
};

struct rxfltr_stype_cap {
	enum rtw_rxfltr_target target;
};

struct rxfltr_cap_ctrl {
	struct rxfltr_stype_cap stype[RXFLTR_STYPE_CTRL_MAX];
};

struct rxfltr_cap_mgnt {
	struct rxfltr_stype_cap stype[RXFLTR_STYPE_MGNT_MAX];
};

struct rxfltr_cap_data {
	struct rxfltr_stype_cap stype[RXFLTR_STYPE_DATA_MAX];
};

struct rxfltr_type_cap {
	struct rxfltr_cap_ctrl rxfltr_ctrl;
	struct rxfltr_cap_mgnt rxfltr_mgnt;
	struct rxfltr_cap_data rxfltr_data;
};

struct rtw_assoc_status {
	u8 link_id;
	u16 status; /* status code */
	u8 used;
};

struct rtw_phl_mld_t {
	_os_list list;
	struct rtw_wifi_role_t *wrole; /*point to wrole*/
	u8 mac_addr[MAC_ALEN];         /*MLD or non-MLD mac address*/
	enum rtw_device_type type;
	u8 sta_num;
	/* idx 0 is the primary STA; others are secondary STAs */
	struct rtw_phl_stainfo_t *phl_sta[RTW_RLINK_MAX];
	/* status codes of each link id for assoc rsp, handled by core */
	struct rtw_assoc_status assoc_status[RTW_RLINK_MAX];
};

struct rtw_wifi_role_link_t {
	struct rtw_wifi_role_t *wrole; /*point to wrole*/
	u8 id;                         /*rlink idx in wrole*/
	u8 mac_addr[MAC_ALEN];         /*link mac address*/

	enum mlme_state mstate;
	#ifdef RTW_WKARD_HW_WMM_ALLOCATE
	_os_atomic hw_wmm0_ref_cnt;
	#endif
	u8 hw_wmm;                     /*HW EDCA - wmm0 or wmm1*/
	u8 hw_band;                    /*MAC Band0 or Band1*/
	                               /*Currently, hw_band == id*/
								   /*Can be changed in the future*/
	u8 hw_port;                    /*MAC HW Port*/
	enum rlink_status status;

	/*
	 * final capability of role from intersection of
	 * sw role cap, final phy cap and final dev cap
	 */
	struct role_link_cap_t cap;

	/*
	 * final protocol capability of role from intersection of
	 * sw role cap, sw protocol cap and hw protocol cap
	 */
	struct protocol_cap_t protocol_cap;

	struct rxfltr_type_cap rxfltr_type_cap;

	/*#ifdef CONFIG_AP*/
#ifdef RTW_PHL_BCN
	/* TODO: ieee mbssid case & multi-bcn (in one iface) case */
	struct rtw_bcn_info_cmn bcn_cmn;
	u8 hw_mbssid;
	/* BSS Parameters Change Count */
	u8 bss_params_chg_cnt;
#endif
	u8 dtim_period;
	u8 mbid_num;
	u32 hiq_win;
	/*#endif CONFIG_AP*/

	struct rtw_chan_def chandef;
	struct rtw_chan_ctx *chanctx;  /*point to chanctx*/

	struct phl_queue assoc_sta_queue;
};

struct rtw_wifi_role_t {
	struct rtw_phl_com_t *phl_com; /*point to phl_com*/
	enum role_type type;           /*will mapping to net type*/
	enum role_type target_type;
	#ifdef RTW_WKARD_PHL_NTFY_MEDIA_STS
	bool is_gc;
	#endif
	bool active;
	u8 id;                         /*recode role_idx in phl_com*/
	u8 mac_addr[MAC_ALEN];
	enum wr_status status;
	enum mlme_state mstate;

#ifdef CONFIG_PHL_TWT
	struct rtw_phl_twt_setup_info twt_setup_info;
#endif /* CONFIG_PHL_TWT */

#ifdef CONFIG_PHL_P2PPS
	struct rtw_phl_noa_desc noa_desc[MAX_NOA_DESC];
#endif

	/* Track back to counter part in core layer */
	void *core_data;
#ifdef RTW_WKARD_BFEE_SET_AID
	u16 last_set_aid;
#endif

	struct phl_queue assoc_mld_queue;
	u8 rlink_num;
	struct rtw_wifi_role_link_t rlink[RTW_RLINK_MAX];
};

#define TXTP_CALC_DIFF_MS 1000
#define RXTP_CALC_DIFF_MS 1000

#define	TX_ULTRA_LOW_TP_THRES_KBPS 100
#define	RX_ULTRA_LOW_TP_THRES_KBPS 100
#define	TX_LOW_TP_THRES_MBPS 2
#define	RX_LOW_TP_THRES_MBPS 2
#define	TX_MID_TP_THRES_MBPS  10
#define	RX_MID_TP_THRES_MBPS  10
#define	TX_HIGH_TP_THRES_MBPS  50
#define	RX_HIGH_TP_THRES_MBPS  50


enum rtw_tfc_lvl {
	RTW_TFC_IDLE = 0,
	RTW_TFC_ULTRA_LOW = 1,
	RTW_TFC_LOW = 2,
	RTW_TFC_MID = 3,
	RTW_TFC_HIGH = 4,
	RTW_TFC_LVL_MAX = 0xFF
};

enum rtw_tfc_sts {
	TRAFFIC_CHANGED = BIT0,
	TRAFFIC_INCREASE = BIT1,
	TRAFFIC_DECREASE = BIT2,
	TRAFFIC_STS_MAX = BIT7
};

struct rtw_traffic_t {
	enum rtw_tfc_lvl lvl;
	enum rtw_tfc_sts sts;
};

struct rtw_stats_tp {
	u64 last_calc_bits;
	u32 last_calc_time_ms;
};
/*statistic*/
struct rtw_stats {
	u64 tx_byte_uni;/*unicast tx byte*/
	u64 rx_byte_uni;/*unicast rx byte*/
	u64 tx_byte_total;
	u64 rx_byte_total;
	u32 tx_tp_kbits;
	u32 rx_tp_kbits;
	u16 tx_moving_average_tp; /* tx average MBps*/
	u16 rx_moving_average_tp; /* rx average MBps*/
	u32 last_tx_time_ms;
	u32 last_rx_time_ms;
	u32 txreq_num;
	u32 rx_rate;
	u32 rx_rate_nmr[RTW_DATA_RATE_HE_NSS4_MCS11 +1];
	u64 ser_event[9]; /* RTW_PHL_SER_MAX */
	struct rtw_stats_tp txtp;
	struct rtw_stats_tp rxtp;
	struct rtw_traffic_t tx_traffic;
	struct rtw_traffic_t rx_traffic;
	u32 rx_tf_cnt; /* rx trigger frame number (accumulated, only reset in disconnect) */
	u32 pre_rx_tf_cnt; /* last record rx trigger frame number from BB */
	/* ltr */
	u32 ltr_last_trigger_time;
	u32 ltr_sw_act_tri_cnt;
	u32 ltr_sw_idle_tri_cnt;
	u8 ltr_cur_state;
};
enum sta_chg_id {
	STA_CHG_BW,
	STA_CHG_NSS,
	STA_CHG_RAMASK,
	STA_CHG_VCS,
	STA_CHG_SEC_MODE,
	STA_CHG_MBSSID,
	STA_CHG_RA_GILTF,
	STA_CHG_MAX
};

enum phl_upd_mode {
	PHL_UPD_ROLE_CREATE,
	PHL_UPD_ROLE_REMOVE,
	PHL_UPD_ROLE_TYPE_CHANGE,
	PHL_UPD_ROLE_INFO_CHANGE,
	PHL_UPD_STA_INFO_CHANGE,
	PHL_UPD_STA_CON_DISCONN,
	PHL_UPD_ROLE_FW_RESTORE,
	PHL_UPD_ROLE_BAND_SW,
	PHL_UPD_ROLE_MAX
};

#ifdef CONFIG_PHL_TXSC
#define PHL_TXSC_ENTRY_NUM 8
#define MAX_WD_SIZE	128

struct phl_txsc_entry {
	u8 txsc_wd_cached;
	u8 txsc_wd_cache[MAX_WD_SIZE];
	u8 txsc_wd_len;
	u8 txsc_wd_seq_offset;
	u32 txsc_cache_hit;
};
#endif

struct rtw_hal_stainfo_t;
struct rtw_phl_stainfo_t {
	_os_list list;
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_mld_t *mld;
	u8 link_id;
	bool active;
	u16 aid;
	u16 macid;
	u32 hw_seq;            /* set hw starting seq in ac que */
	u16 set_rts_init_rate; /* set the val as type rtw_data_rate + 1 */
	u8 mac_addr[MAC_ALEN];
	u8 tid_ul_map;         /* bitmask */
	u8 tid_dl_map;         /* bitmask */

	struct rtw_chan_def chandef;
	struct rtw_stats stats;
	enum wlan_mode wmode;

	/*mlme protocol or MAC related CAP*/
	u8 bcn_hit_cond;
	u8 hit_rule;
	u8 tf_trs;
	u8 tgt_ind;
	u8 frm_tgt_ind;
	u8 addr_sel;
	u8 addr_msk;

	/* rx agg */
	struct phl_tid_ampdu_rx *tid_rx[RTW_MAX_TID_NUM]; /* TID_MAX_NUM */
	_os_lock tid_rx_lock;               /* guarding @tid_rx */
	_os_event comp_sync;     /* reorder timer completion event */
	_os_timer reorder_timer; /* reorder timer for all @tid_rx of the
	                          * stainfo */
	/* TODO: add missing part */

	/*mlme protocol or PHY related CAP*/
	struct protocol_cap_t asoc_cap;
	enum rtw_protect_mode protect;

	/*security related*/
	u8 wapi;
	u8 sec_mode;
	u64 sec_iv;

	u8 cts2self;
	u8 rts_en;
	u8 hw_rts_en;
	u8 rts_cca_mode;
	u8 rts_sel;

	/*
	 * STA powersave, those could be implemented as bit flags but there's no
	 * corresponding atomic bit operations available on Windows.
	 */
	_os_atomic ps_sta;      /* the sta is in PS mode or not */

	struct rtw_hal_stainfo_t *hal_sta;

#ifdef CONFIG_PHL_TXSC
	struct phl_txsc_entry phl_txsc[PHL_TXSC_ENTRY_NUM];
#endif
	struct rtw_rx_bcn_info bcn_i;
	void *core_data; /* Track back to counter part in core layer */
	u8 wol_magic;
#ifdef CONFIG_PHL_HANDLE_SER_L2
	bool l2recover_connect_bk;
	u8 l2recover_assoc_rssi_bk;
	struct protocol_cap_t l2recover_asoc_cap_bk;
#endif
};




#define WL_FUNC_P2P		BIT0
#define WL_FUNC_MIRACAST	BIT1
#define WL_FUNC_TDLS		BIT2
#define WL_FUNC_FTM		BIT3
#define WL_FUNC_BIT_NUM	4


/* HW MAC capability*/
#define HW_SUP_DBCC			BIT0
#define HW_SUP_AMSDU			BIT1
#define HW_SUP_TCP_TX_CHKSUM		BIT2
#define HW_SUP_TCP_RX_CHKSUM		BIT3
#define HW_SUP_TXPKT_CONVR		BIT4
#define HW_SUP_RXPKT_CONVR		BIT5
#define HW_SUP_MULTI_BSSID		BIT6
#define HW_SUP_OFDMA			BIT7
#define HW_SUP_CHAN_INFO		BIT8
#define HW_SUP_TSSI			BIT9
#define HW_SUP_TANK_K			BIT10

/*BUS Section CAP */
#define HW_SUP_PCIE_PLFH		BIT20	/*payload from host*/
#define HW_SUP_USB_MULTI_FUN		BIT21
#define HW_SUP_SDIO_MULTI_FUN		BIT22

/* Beamform CAP */
#define HW_CAP_BF_NON_SUPPORT 0
#define HW_CAP_BFEE_HT_SU BIT(0)
#define HW_CAP_BFER_HT_SU BIT(1)
#define HW_CAP_BFEE_VHT_SU BIT(2)
#define HW_CAP_BFER_VHT_SU BIT(3)
#define HW_CAP_BFEE_VHT_MU BIT(4)
#define HW_CAP_BFER_VHT_MU BIT(5)
#define HW_CAP_BFEE_HE_SU BIT(6)
#define HW_CAP_BFER_HE_SU BIT(7)
#define HW_CAP_BFEE_HE_MU BIT(8)
#define HW_CAP_BFER_HE_MU BIT(9)
#define HW_CAP_HE_NON_TB_CQI BIT(10)
#define HW_CAP_HE_TB_CQI BIT(11)

#define RTW_HW_CAP_ULRU_AUTO 0
#define RTW_HW_CAP_ULRU_DISABLE 1
#define RTW_HW_CAP_ULRU_ENABLE 2

/* general define for some of dev sw/hw cap */
#define RTW_DEV_CAP_AUTO 0
#define RTW_DEV_CAP_ENABLE 1
#define RTW_DEV_CAP_DISABLE 2

/* STBC CAP */
#define HW_CAP_STBC_HT_TX BIT(0)
#define HW_CAP_STBC_VHT_TX BIT(1)
#define HW_CAP_STBC_HE_TX BIT(2)
#define HW_CAP_STBC_HE_TX_GT_80M BIT(3)
#define HW_CAP_STBC_HT_RX BIT(4)
#define HW_CAP_STBC_VHT_RX BIT(5)
#define HW_CAP_STBC_HE_RX BIT(6)
#define HW_CAP_STBC_HE_RX_GT_80M BIT(7)

#define RTW_SEC_CAP_HW_FORM_HDR BIT0
#define RTW_SEC_CAP_HW_SEARCH_KEY BIT1
#define RTW_SEC_CAP_HW_SEC_IV BIT2

struct hal_spec_t {
	char *ic_name;
	u16 macid_num;

	u8 sec_cam_ent_num;
	u8 sec_cap;
	u8 wow_cap;

	u8 rfpath_num_2g:4;	/* used for tx power index path */
	u8 rfpath_num_5g:4;	/* used for tx power index path */
	u8 rf_reg_path_num;
	u8 max_tx_cnt;

	u8 port_num;
	u8 wmm_num;
	u8 proto_cap;	/* value of PROTO_CAP_XXX */
	u8 wl_func;	/* value of WL_FUNC_XXX */

	/********* xmit ************/


	/********* recv ************/
	u8 rx_bd_info_sz;

	u16 rx_tag[2];
	#ifdef CONFIG_USB_HCI
	u8 max_bulkin_num;
	u8 max_bulkout_num;
	#endif
	#ifdef CONFIG_PCI_HCI
	u16 txbd_multi_tag;
	u8 txbd_upd_lmt;
	#ifdef RTW_WKARD_BUSCAP_IN_HALSPEC
	u8 phyaddr_num;
	#endif
	u16 addr_info_len_lmt;
	#endif
	u8 cts2_thres_en;
	u16 cts2_thres;
	/********* beamformer ************/
	u8 max_csi_buf_su_nr;
	u8 max_csi_buf_mu_nr;
	u8 max_bf_ent_nr;
	u8 max_su_sta_nr;
	u8 max_mu_sta_nr;

	bool ser_cfg_int;
	bool lps_cfg_int;

	u8 max_std_entry_num;
	u8 max_tmp_entry_num;
};

#define phl_get_hci_type(_phlcom) (_phlcom->hci_type)
#define phl_get_ic_spec(_phlcom) (&_phlcom->hal_spec)
#define phl_get_fw_buf(_phlcom) (_phlcom->fw_info.ram_buff)
#define phl_get_fw_size(_phlcom) (_phlcom->fw_info.ram_size)

enum rtw_drv_mode {
	RTW_DRV_MODE_NORMAL = 0,
	RTW_DRV_MODE_EQC = 1,
	RTW_DRV_MODE_HIGH_THERMAL = 2,

	/* 11~20 for MP submodule section*/
	RTW_DRV_MODE_MP_SMDL_START = 11,
	RTW_DRV_MODE_MP = 11,
	RTW_DRV_MODE_HOMOLOGATION = 12,
	RTW_DRV_MODE_MP_TMAC = 13,
	RTW_DRV_MODE_MP_SMDL_END = 20,

	/* 21~30 for FPGA submodule section*/
	RTW_DRV_MODE_FPGA_SMDL_START = 21,
	RTW_DRV_MODE_FPGA_SMDL_TEST = 21,
	RTW_DRV_MODE_FPGA_SMDL_LOOPBACK = 22,
	RTW_DRV_MODE_FPGA_SMDL_END = 30,

	/* 31~60 for VERIFY submodule section*/
	RTW_DRV_MODE_VERIFY_SMDL_START = 31,
	RTW_DRV_MODE_VERIFY_SMDL_END = 60,

	/* 61~80 for TOOL submodule section*/
	RTW_DRV_MODE_TOOL_SMDL_START = 61,
	RTW_DRV_MODE_TOOL_SMDL_END = 80,

	/* Fixed Max Value*/
	RTW_DRV_MODE_MAX = 255
};

struct rtw_evt_info_t {
	_os_lock evt_lock;
	enum rtw_phl_evt evt_bitmap;
};

// WiFi FW
struct rtw_fw_info_t {
	u8 fw_en;
	u8 fw_src;
	u8 fw_type;
	u8 dlram_en;
	u8 dlrom_en;
	u8 *rom_buff;
	u32 rom_addr;
	u32 rom_size;
	char rom_path[256];
	u8 *ram_buff;
	u32 ram_size;
	char ram_path[256];
	u8 *buf;
	u32 buf_size;
	u8 *wow_buf;
	u32 wow_buf_size;
	u8 *sym_buf;
	u32 sym_buf_size;
};

enum rtw_fw_status {
	RTW_FW_STATUS_OK,
	RTW_FW_STATUS_NOFW,
	RTW_FW_STATUS_ASSERT,
	RTW_FW_STATUS_EXCEP,
	RTW_FW_STATUS_RXI300,
	RTW_FW_STATUS_HANG
};

#ifdef CONFIG_PHL_DFS
enum dfs_regd_t {
	DFS_REGD_UNKNOWN	= 0,
	DFS_REGD_FCC	= 1,
	DFS_REGD_JAP	= 2,
	DFS_REGD_ETSI	= 3,
	DFS_REGD_KCC	= 4,
	DFS_REGD_NUM,
};

#define PHL_DFS_REGD_IS_UNKNOWN(regd) ((regd) == DFS_REGD_UNKNOWN || (regd) >= DFS_REGD_NUM)

struct rtw_dfs_t {
	enum dfs_regd_t region_domain;
	bool enable; /* set by core layer to enable/disable radar detection */

	/*
	* set by core to specify detect range
	* sp_detect_range_hi = 0 means no specified range, whole range allowed
	* by HAL will enable radar detection
	*/
	u32 sp_detect_range_hi;
	u32 sp_detect_range_lo;

	bool radar_detect_enabled; /* if radar detection is enabled */
	bool cac_tx_paused; /* if tx paused by CAC */
	bool pending_domain_change; /* if there is domain change under process */

	bool is_radar_detectd; /* if radar is detected */
};
#endif

struct txpwr_regu_info_t {
	bool force; /* false: control by regulation status, true: control by following values */
	u8 *regu_2g;
	u8 regu_2g_len;
	u8 *regu_5g;
	u8 regu_5g_len;
	u8 *regu_6g;
	u8 regu_6g_len;
};

struct txpwr_ctl_param {
	enum phl_band_idx band_idx;

	/*
	* set true to write tx power setting to HW even if no configuration change
	*/
	bool force_write_txpwr;

	/*
	* set true NOT to write tx power setting to HW even if configuration change
	*/
	bool force_sw_config;

	/*
	* set true to config tx power regulation info
	*/
	bool pr_info_config;
	struct txpwr_regu_info_t pr_info;

	/*
	* tx power constraint in unit of mB (0.01dB)
	* < 0: not set (keep original)
	*/
	s32 constraint_mb;
};

#define txpwr_ctl_param_init(param) \
	do { \
		(param)->band_idx = HW_BAND_MAX; \
		(param)->force_write_txpwr = false; \
		(param)->force_sw_config = false; \
		(param)->pr_info_config = false; \
		(param)->constraint_mb = -1; \
	} while (0)



#ifdef CONFIG_MCC_SUPPORT
#define BT_SEG_NUM 2
#define SLOT_NUM 4
#define MIN_TDMRA_SLOT_NUM 2
enum rtw_phl_mcc_dbg_type {
	MCC_DBG_NONE = 0,
	MCC_DBG_STATE,
	MCC_DBG_OP_MODE,
	MCC_DBG_COEX_MODE,
	MCC_DBG_BT_INFO,
	MCC_DBG_EN_INFO
};

enum rtw_phl_mcc_state {
	MCC_NONE = 0,
	MCC_CFG_EN_INFO,
	MCC_TRIGGER_FW_EN,
	MCC_FW_EN_FAIL,
	MCC_RUNING,
	MCC_TRIGGER_FW_DIS,
	MCC_FW_DIS_FAIL,
	MCC_STOP
};

enum rtw_phl_mcc_coex_mode {
	RTW_PHL_MCC_COEX_MODE_NONE = 0,
	RTW_PHL_MCC_COEX_MODE_BT_MASTER,
	RTW_PHL_MCC_COEX_MODE_WIFI_MASTER,
	RTW_PHL_MCC_COEX_MODE_BT_WIFI_BALANCE
};

enum rtw_phl_tdmra_wmode {
	RTW_PHL_TDMRA_WMODE_NONE = 0,
	RTW_PHL_TDMRA_AP_CLIENT_WMODE,
	RTW_PHL_TDMRA_2CLIENTS_WMODE,
	RTW_PHL_TDMRA_AP_WMODE,
	RTW_PHL_TDMRA_UNKNOWN_WMODE
};

enum rtw_phl_mcc_rpt {
	RTW_MCC_RPT_OFF = 0,
	RTW_MCC_RPT_FAIL_ONLY,
	RTW_MCC_RPT_ALL
};

enum rtw_phl_mcc_dur_lim_tag {
	RTW_MCC_DUR_LIM_NONE = 0,
	RTW_MCC_DUR_LIM_NOA
};

/*Export to core layer. Phl get switch ch setting of role from core layer*/
#define NONSPECIFIC_SETTING 0xff
struct rtw_phl_mcc_setting_info {
	struct rtw_wifi_role_t *wrole;
	u8 role_map;/*the wifi role map in operating mcc */
	u8 tx_null_early;
	u16 dur; /*core specific duration in a period of 100 ms */
	bool en_fw_mcc_log;
	u8 fw_mcc_log_lv;/* fw mcc log level */
};


/*Export to core layer. Core get NOA info to update p2p beacon*/
struct rtw_phl_mcc_noa {
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	u32 start_t_h;
	u32 start_t_l;
	u16 dur;
	u16 interval;
	u8 cnt;
};

struct rtw_phl_mcc_ops {
	void *priv; /* ops private, define by core layer*/
	int (*mcc_update_noa)(void *priv, struct rtw_phl_mcc_noa *param);
	int (*mcc_get_setting)(void *priv, struct rtw_phl_mcc_setting_info *param);
};


struct rtw_phl_mcc_dbg_slot_info {
	bool bt_role;
	u16 dur;
	u16 ch;
	u16 macid;
};

struct rtw_phl_mcc_dbg_hal_info {
	u8 slot_num;
	struct rtw_phl_mcc_dbg_slot_info dbg_slot_i[SLOT_NUM];
	bool btc_in_group;
};

struct rtw_phl_mcc_macid_bitmap {
	u32 *bitmap;
	u8 len;
};

struct rtw_phl_mcc_sync_tsf_info {
	u8 sync_en;
	u16 source;
	u16 target;
	u16 offset;
};

struct rtw_phl_mcc_dur_lim_info {
	bool enable;
	enum rtw_phl_mcc_dur_lim_tag tag;
	u16 max_tob;
	u16 max_toa;
	u16 max_dur;
};

struct rtw_phl_mcc_dur_info {
	u16 dur;
	struct rtw_phl_mcc_dur_lim_info dur_limit;
};

struct rtw_phl_mcc_policy_info {
	u8 c2h_rpt;
	u8 tx_null_early;
	u8 dis_tx_null;
	u8 in_curr_ch;
	u8 dis_sw_retry;
	u8 sw_retry_count;
	struct rtw_phl_mcc_dur_info dur_info;
	u8 rfk_chk;
	u8 protect_bcn;
	u8 courtesy_en;
	u8 courtesy_num;
	u8 courtesy_target;
};

struct rtw_phl_mcc_role {
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_mcc_macid_bitmap used_macid;
	struct rtw_chan_def *chandef;
	struct rtw_phl_mcc_policy_info policy;
	u16 macid;
	u16 bcn_intvl;
	bool bt_role;
	u8 group;
};

/*
 * @c_en: Enable courtesy function
 * @c_num: the time slot of src_role replace by tgt_role
 */
struct rtw_phl_mcc_courtesy {
	bool c_en;
	bool c_num;
	struct rtw_phl_mcc_role *src_role;
	struct rtw_phl_mcc_role *tgt_role;
};

/*
 * @slot: duration, unit: TU
 * @bt_role: True: bt role, False: Wifi role
 * @mrole: mcc role info for Wifi Role
 */
struct rtw_phl_mcc_slot_info {
	u16 slot;
	bool bt_role;
	struct rtw_phl_mcc_role *mrole;
};

/*
 * @slot_num: total slot num(Wifi+BT)
 * @bt_slot_num: total BT slot num
 * |      Dur1      |     Dur2      |
 * 	bcn              bcn
 * |tob_r | toa_r|tob_a | toa_a|
 */
struct rtw_phl_mcc_pattern {
	u8 slot_num;
	u8 bt_slot_num;
	struct rtw_phl_mcc_role *role_ref;
	struct rtw_phl_mcc_role *role_ano;
	s16 tob_r;
	s16 toa_r;
	s16 tob_a;
	s16 toa_a;
	u16 bcns_offset;

	u16 calc_fail;
	/**
	* |tob_r|toa_r|
	* -----------<d_r_d_a_spacing>-----------
	*                                               |tob_a|toa_a|
	**/
	u16 d_r_d_a_spacing_max;
	struct rtw_phl_mcc_courtesy courtesy_i;
	/*record slot order for X wifi slot + Y bt slot*/
	struct rtw_phl_mcc_slot_info slot_order[SLOT_NUM];
};

/*
 * Enable info for mcc
 * @ref_role_idx: the index of reference role
 * @mrole_map: use mcc role num
 * @mrole_num: use mcc role num
 * @group: assigned by halmac mcc, the group resource of fw feture, phl layer ignore it
 *	fw mcc can handle differenec slot pattern, and the group is the id of slot pattern.
 * @tsf_high, tsf_low: Start TSF
 * @tsf_high_l, tsf_low_l: Limitation of Start TSF
 * @dbg_hal_i: Debug info for hal mcc
 */
struct rtw_phl_mcc_en_info {
	struct rtw_phl_mcc_role mcc_role[MCC_ROLE_NUM];
	struct rtw_phl_mcc_sync_tsf_info sync_tsf_info;
	struct rtw_phl_mcc_pattern m_pattern;
	u8 ref_role_idx;
	u8 mrole_map;
	u8 mrole_num;
	u8 group;
	u16 mcc_intvl;
	u32 tsf_high;
	u32 tsf_low;
	u32 tsf_high_l;
	u32 tsf_low_l;
	struct rtw_phl_mcc_dbg_hal_info dbg_hal_i;
};

/*
 * Bt info
 * @bt_dur: bt slot
 * @bt_seg: segment bt slot
 * @bt_seg_num: segment num
 * @add_bt_role: if add_bt_role = true, we need to add bt slot to fw
 */
struct rtw_phl_mcc_bt_info {
	u16 bt_dur;
	u16 bt_seg[BT_SEG_NUM];
	u8 bt_seg_num;
	bool add_bt_role;
};

enum mr_coex_trigger {
	MR_COEX_TRIG_BY_BT,
	MR_COEX_TRIG_BY_LINKING,
	MR_COEX_TRIG_BY_DIS_LINKING,
	MR_COEX_TRIG_BY_CHG_SLOT,
	MR_COEX_TRIG_BY_SCAN,
	MR_COEX_TRIG_BY_ECSA,
	MR_COEX_TRIG_BY_CHG_OP_CHDEF,
	MR_COEX_TRIG_BY_EN_DBCC,
	MR_COEX_TRIG_BY_DIS_DBCC,
	MR_COEX_TRIG_BY_ASSOC_TSF_EFFOR,
	MR_COEX_TRIG_MAX,
};

#endif /* CONFIG_MCC_SUPPORT */

/*multi-roles control components*/
enum mr_op_mode {
	MR_OP_NONE,
	MR_OP_SWR,/*Single Linked WRole*/
	MR_OP_SCC,
	MR_OP_MCC,
	MR_OP_DBCC,
	MR_OP_MAX,
};

enum cc_band_type {
	CC_BAND_24G	= 0,
	CC_BAND_5G_B1	= 1,
	CC_BAND_5G_B2	= 2,
	CC_BAND_5G_B3	= 3,
	CC_BAND_5G_B4	= 4,
	CC_BAND_6G_U5	= 5,
	CC_BAND_6G_U6	= 6,
	CC_BAND_6G_U7	= 7,
	CC_BAND_6G_U8	= 8,
	CC_BAND_MAX,
	CC_BAND_NON = CC_BAND_MAX,
};

struct rtw_mr_chctx_info {
	bool is_db;
	u8 cur_band_idx;
	u8 role_num;
	u8 chctx_num;
	u16 chctx_band_map;/*enum cc_band_type*/
	enum mr_op_mode sugg_opmode;
};

enum mr_op_type {
	MR_OP_TYPE_NONE,
	MR_OP_TYPE_STATION_ONLY,
	MR_OP_TYPE_AP_ONLY,
	MR_OP_TYPE_STATION_AP,
	MR_OP_TYPE_MAX,
};

struct mr_info {
	u8 sta_num;
	u8 ld_sta_num;
	u8 lg_sta_num;		/* WIFI_STATION_STATE && WIFI_UNDER_LINKING */

	u8 ap_num;
	u8 ld_ap_num;	/*&& asoc_sta_count > 2*/
	u8 monitor_num;

	u8 p2p_device_num;
	u8 p2p_gc_num;
	u8 p2p_go_num;

#ifdef CONFIG_PHL_TDLS
	u8 ld_tdls_num;	/* phl_role->type == PHL_RTYPE_TDLS */
#endif

#if 0
#ifdef CONFIG_AP_MODE
	u8 starting_ap_num;	/*WIFI_FW_AP_STATE*/
#endif
	u8 adhoc_num;		/* (WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) && WIFI_ASOC_STATE */
	u8 ld_adhoc_num;	/* (WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) && WIFI_ASOC_STATE && asoc_sta_count > 2 */
#ifdef CONFIG_RTW_MESH
	u8 mesh_num;		/* WIFI_MESH_STATE &&  WIFI_ASOC_STATE */
	u8 ld_mesh_num;		/* WIFI_MESH_STATE &&  WIFI_ASOC_STATE && asoc_sta_count > 2 */
#endif
#endif
};

#ifdef CONFIG_MR_COEX_SUPPORT

enum rtw_phl_mr_coex_chk_inprocess_type {
	RTW_MR_COEX_CHK_INPROGRESS = 0,
	RTW_MR_COEX_CHK_INPROGRESS_TDMRA,
	RTW_MR_COEX_CHK_INPROGRESS_TDMRA_SINGLE_CH,
	RTW_MR_COEX_CHK_INPROGRESS_TDMRA_MULTI_CH,
	RTW_MR_COEX_CHK_MAX
};


enum mr_coex_mode {
	MR_COEX_MODE_NONE = 0,
	MR_COEX_MODE_2GSCC_1AP_1STA_BTC = 1,
	MR_COEX_MODE_TDMRA = 2
};

struct mr_coex_info {
	enum mr_coex_mode coex_mode;
	enum mr_coex_trigger coex_trig;
	bool prohibit;
	u8 proh_rsn; /* Prohibited Reason */
};
#endif /* CONFIG_MR_COEX_SUPPORT */

/*export to core layer*/
struct mr_query_info {
	struct mr_info cur_info;
	enum mr_op_mode op_mode;
	enum mr_op_type op_type;
};

struct hw_band_ctl_t {
	_os_lock lock;
	u8 id;
	u8 port_map; /*used port_idx*/
	u8 role_map; /*used role_idx*/
#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	u8 mbssid_cnt; /* used M-BSSID number */
	u16 mbssid_map; /* used M-BSSID ID */
#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
	u8 wmm_map;
	struct mr_info cur_info;
	enum mr_op_mode op_mode;
	enum mr_op_type op_type;
	enum phl_hw_port tsf_sync_port;
	u16 chctx_band_map;/*enum cc_band_type- BIT MAP*/
	_os_atomic ccb_24g_rc;
	_os_atomic ccb_5gb1_rc;
	_os_atomic ccb_5gb2_rc;
	_os_atomic ccb_5gb3_rc;
	_os_atomic ccb_5gb4_rc;
	_os_atomic ccb_6gu5_rc;
	_os_atomic ccb_6gu6_rc;
	_os_atomic ccb_6gu7_rc;
	_os_atomic ccb_6gu8_rc;

	struct phl_queue chan_ctx_queue;/*struct rtw_chan_ctx*/
#ifdef CONFIG_MR_COEX_SUPPORT
	struct mr_coex_info mr_coex_i;
#endif /* CONFIG_MR_COEX_SUPPORT */
#ifdef CONFIG_MCC_SUPPORT
	void *mcc_info; /*struct phl_mcc_info*/
#endif
};

#define MAX_BAND_NUM 2
struct rtw_hal_com_t;

#ifdef CONFIG_PHL_P2PPS
struct rtw_phl_noa {
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	enum p2pps_trig_tag tag;
	u32 start_t_h;
	u32 start_t_l;
	u16 dur;
	u8 cnt;
	u16 interval;
};
#endif

#ifdef CONFIG_DBCC_SUPPORT
struct dbcc_proto_msg {
	struct rtw_wifi_role_t *wr;
	bool dbcc_en;
};
#endif

struct rtw_phl_mr_ops {
	void *priv; /* ops private, define by core layer*/
#ifdef CONFIG_PHL_P2PPS
	int (*phl_mr_update_noa)(void *priv, struct rtw_phl_noa *param);
#endif
#ifdef CONFIG_MCC_SUPPORT
	struct rtw_phl_mcc_ops *mcc_ops;
#endif
#ifdef CONFIG_DBCC_SUPPORT
	u8 (*dbcc_protocol_hdl)(void *drv, enum phl_band_idx band_idx,
				  struct dbcc_proto_msg *dbcc_proto);
#endif
};

struct mr_ctl_t {
	struct rtw_hal_com_t *hal_com;
	_os_lock lock;
	struct hw_band_ctl_t band_ctrl[MAX_BAND_NUM];
	struct phl_bk_module_ops bk_ops;
	u8 role_map;
	bool is_db;/*hw dual band*/
	struct rtw_phl_mr_ops mr_ops;
#ifdef CONFIG_DBCC_SUPPORT
	u8 trigger_dbcc_cfg;
#endif
#ifdef CONFIG_MCC_SUPPORT
	u8 init_mcc;
	void *com_mcc;/*struct phl_com_mcc_info*/
#endif
	u8 cck_phyidx;
};

enum rtw_rssi_type {
	RTW_RSSI_DATA_ACAM,
	RTW_RSSI_DATA_ACAM_A1M,
	RTW_RSSI_DATA_OTHER,
	RTW_RSSI_CTRL_ACAM,
	RTW_RSSI_CTRL_ACAM_A1M,
	RTW_RSSI_CTRL_OTHER,
	RTW_RSSI_MGNT_ACAM,
	RTW_RSSI_MGNT_ACAM_A1M,
	RTW_RSSI_MGNT_OTHER,
	RTW_RSSI_UNKNOWN,
	RTW_RSSI_TYPE_MAX
};
#define PHL_RSSI_MA_H 4 /*RSSI_MA_H*/
#define PHL_MAX_RSSI 110
#define PHL_RSSI_MAVG_NUM 16
#define UPDATE_MA_RSSI(_RSSI, _TYPE, _VAL) \
	do { \
		u8 oldest_rssi = 0; \
		if(_RSSI->ma_rssi_ele_idx[_TYPE] < PHL_RSSI_MAVG_NUM) { \
			oldest_rssi = _RSSI->ma_rssi_ele[_TYPE][\
					_RSSI->ma_rssi_ele_idx[_TYPE]]; \
			_RSSI->ma_rssi_ele[_TYPE][_RSSI->ma_rssi_ele_idx[_TYPE]] = \
				((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		} else { \
			_RSSI->ma_rssi_ele_idx[_TYPE] = 0; \
			oldest_rssi = _RSSI->ma_rssi_ele[_TYPE][0]; \
			_RSSI->ma_rssi_ele[_TYPE][0] = \
				((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		} \
		_RSSI->ma_rssi_ele_sum[_TYPE] -= oldest_rssi;\
		_RSSI->ma_rssi_ele_sum[_TYPE] += \
			((_VAL > PHL_MAX_RSSI) ? PHL_MAX_RSSI : _VAL ); \
		_RSSI->ma_rssi_ele_idx[_TYPE]++; \
		if(_RSSI->ma_rssi_ele_cnt[_TYPE] < PHL_RSSI_MAVG_NUM) \
			_RSSI->ma_rssi_ele_cnt[_TYPE]++; \
		_RSSI->ma_rssi[_TYPE] = (u8)(_RSSI->ma_rssi_ele_sum[_TYPE] / \
			_RSSI->ma_rssi_ele_cnt[_TYPE]);\
	} while (0)

#define PHL_TRANS_2_RSSI(X) (X >> 1)

struct rtw_phl_rssi_stat {
	_os_lock lock;
	u8 ma_rssi_ele_idx[RTW_RSSI_TYPE_MAX];
	u8 ma_rssi_ele_cnt[RTW_RSSI_TYPE_MAX]; /* maximum : PHL_RSSI_MAVG_NUM */
	u8 ma_rssi_ele[RTW_RSSI_TYPE_MAX][PHL_RSSI_MAVG_NUM]; /* rssi element for moving average */
	u32 ma_rssi_ele_sum[RTW_RSSI_TYPE_MAX];
	u8 ma_rssi[RTW_RSSI_TYPE_MAX]; /* moving average : 0~PHL_MAX_RSSI (dBm = rssi - PHL_MAX_RSSI) */
};

#define PHL_MAX_PPDU_CNT 8
#define PHL_MAX_PPDU_STA_NUM 4
struct rtw_phl_ppdu_sts_sta_ent {
	u8 vld;
	/*u8 rssi;*/
	u16 macid;
};

struct rtw_phl_ppdu_phy_info {
	/* from ppdu_sts, valid, if is_valid = true */
	bool is_valid;
	bool is_pkt_with_data; /* 0: FA, 1: not FA */
	u8 rssi; /*signal power : 0 - PHL_MAX_RSSI, rssi dbm = PHL_MAX_RSSI - value*/
	u8 rssi_path[RTW_PHL_MAX_RF_PATH];/*PATH A, PATH B ... PATH D*/
	u8 tx_bf;
	u8 frame_type; /* type + subtype */
	u8 snr_fd_avg;
	u8 snr_fd[4];
	u8 snr_td_avg;
	u8 snr_td[4];

	/* from drvinfo, valid if is_from_drvinfo = ture */
	bool is_drvinfo_vld;
	u8 signal_strength;

	/* valid if one of (is_drvinfo_vld) or (is_valid) is true */
	/* if both is_from_drvinfo and is_valid are true, this value will come from drv_info */
	u8 ch_idx; /* center channel , not primary channel */
	u8 rxsc;
#ifdef CONFIG_PHL_CHANNEL_INFO
	u8 n_rx;
	u8 n_sts;
	u8 evm_1_sts;
	u8 evm_2_sts;
	u8 avg_idle_noise_pwr;
	bool is_ch_info_len_valid;
	u16 ch_info_len;
	u8 *ch_info_addr;
#endif
};

struct rtw_phl_ppdu_sts_ent {
	/* from normal data */
	u8 frame_type;
	u8 addr_cam_vld;
	u8 crc32;
	u8 ppdu_type;
	u16 rx_rate;
	u8 src_mac_addr[MAC_ADDRESS_LENGTH];

	/* from ppdu status */
	bool valid;
	bool phl_done;
	u8 usr_num;
	u32 freerun_cnt;
	struct rtw_phl_ppdu_phy_info phy_info;
	struct rtw_phl_ppdu_sts_sta_ent sta[PHL_MAX_PPDU_STA_NUM];
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	/* for ppdu status per pkt */
	struct phl_queue frames;
#endif
};
struct rtw_phl_ppdu_sts_info {
	struct rtw_phl_ppdu_sts_ent sts_ent[HW_BAND_MAX][PHL_MAX_PPDU_CNT];
	u8 cur_rx_ppdu_cnt[HW_BAND_MAX];
	bool en_ppdu_sts[HW_BAND_MAX];
	bool latest_rx_is_psts[HW_BAND_MAX];
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	bool en_psts_per_pkt;
	bool psts_ampdu;
#define RTW_PHL_PSTS_FLTR_MGNT BIT(RTW_FRAME_TYPE_MGNT)
#define RTW_PHL_PSTS_FLTR_CTRL BIT(RTW_FRAME_TYPE_CTRL)
#define RTW_PHL_PSTS_FLTR_DATA BIT(RTW_FRAME_TYPE_DATA)
#define RTW_PHL_PSTS_FLTR_EXT_RSVD BIT(RTW_FRAME_TYPE_EXT_RSVD)
	u8 ppdu_sts_filter;
	u8 en_fake_psts;
	u8 cur_ppdu_cnt[HW_BAND_MAX];
#endif
};

struct rtw_phl_gid_pos_tbl {
#define RTW_VHT_GID_MGNT_FRAME_GID_SZ 8
#define RTW_VHT_GID_MGNT_FRAME_POS_SZ 16
	u8 gid_vld[RTW_VHT_GID_MGNT_FRAME_GID_SZ]; /* from 0 - 63 */
	u8 pos[RTW_VHT_GID_MGNT_FRAME_POS_SZ]; /* 0 - 63, per 2 bit*/
};


struct rtw_iot_t {
	u32 id;
	u32 iot_id[MAX_WIFI_ROLE_NUMBER];
};

#ifdef CONFIG_PHL_THERMAL_PROTECT
enum phl_thermal_protect_action{
	PHL_THERMAL_PROTECT_ACTION_NONE = 0,
	PHL_THERMAL_PROTECT_ACTION_LEVEL1 = 1,
	PHL_THERMAL_PROTECT_ACTION_LEVEL2 = 2,
	PHL_THERMAL_PROTECT_ACTION_LEVEL3 = 3,
	PHL_THERMAL_PROTECT_ACTION_LEVEL4 = 4,
	PHL_THERMAL_PROTECT_ACTION_LEVEL5 = 5,
	PHL_THERMAL_PROTECT_ACTION_LEVEL_MAX = PHL_THERMAL_PROTECT_ACTION_LEVEL5,
};
#endif

enum tpe_pwr_intpn { /* tpe power interpretaion */
	PWR_INTPN_UNDEFINED = 0,
	PWR_INTPN_EIRP = 1, /* EIRP, unit=dBm */
	PWR_INTPN_EIRP_PSD = 2 /* EIRP PSD, unit=dBm/MHz */
};

#define MAX_TPE_TX_PWR_CNT 8
struct rtw_r_tpe_ele_t { /* reduced tpe element */
	/* pwr_intpn = PWR_INTPN_EIRP(1)
	* valid_pwr_cnt = 1, 2, 3, 4, otherwise invalid
	* max_tx_pwr[0] maximum transmit power for 20 MHz
	* max_tx_pwr[1] maximum transmit power for 40 MHz
	* max_tx_pwr[2] maximum transmit power for 80 MHz
	* max_tx_pwr[3] maximum transmit power for 160/80+80 MHz
	*/

	/* pwr_intpn = PWR_INTPN_EIRP_PSD(2)
	* valid_pwr_cnt = 0, 1, 2, 4, 8, otherwise invalid
	* valid_pwr_cnt = 0 is a special case
	*                 use one max_tx_pwr to represent any bw's PSD
	* maximum transmit power spectral density (PSD)
	* max_tx_pwr[0] maximum transmit PSD 1
	* max_tx_pwr[1] maximum transmit PSD 2
	* ...
	* max_tx_pwr[7] maximum transmit PSD 8
	*/
	enum tpe_pwr_intpn pwr_intpn;
	u8 max_tx_pwr[MAX_TPE_TX_PWR_CNT];
	u8 valid_pwr_cnt;
};

enum regu_info_var { /* regulatory info */
	REGU_INFO_UNDEFINED = 0,
	REGU_INFO_INDOOR_AP = 1,
	REGU_INFO_STANDARD_PWR_AP = 2
};

#define MAX_TPE_ELE_CNT 4
struct rtw_tpe_info_t { /* tpe info */
	struct rtw_chan_def rx_chdef;
	enum regu_info_var ap_type;
	struct rtw_r_tpe_ele_t r_tpe[MAX_TPE_ELE_CNT];
	u8 valid_tpe_cnt;
};

#ifdef PHL_RXSC_AMPDU
struct rtw_rxsc_cache_entry {
	u16 cached_rx_macid;
	u32 cached_rx_tid;
	u32 cached_rx_ppdu_cnt;
	u32 cached_rx_seq;

	struct rtw_phl_stainfo_t *cached_sta;
	struct phl_tid_ampdu_rx *cached_r;
	_os_lock rxsc_lock;	/* guarding @cached_r */
};
#endif

#ifdef DEBUG_PHL_TX
/* for tx path debug and counter */
struct phl_tx_stats {
	u8 flag_print_wdpage_once;
	u32 phl_txsc_apply_cnt;
	u32 phl_txsc_copy_cnt;
	u32 wp_tg_out_of_resource;
	u32 wp_tg_force_reuse;
	u32 phl_txreq_sta_leave_drop;
};
#endif /* DEBUG_PHL_TX */

struct rtw_phl_evt_ops;
struct rtw_phl_com_t {
	struct rtw_wifi_role_t wifi_roles[MAX_WIFI_ROLE_NUMBER];
	struct mr_ctl_t mr_ctrl; /*multi wifi_role control module*/
	struct rtw_phl_evt_ops evt_ops;
	enum rtw_hci_type hci_type;
	enum rtw_drv_mode drv_mode;/*normal or mp mode*/
	enum rtw_dev_state dev_state;

	struct hal_spec_t hal_spec;

	struct role_sw_cap_t role_sw_cap; /* SW control capability of role for any purpose */
	struct protocol_cap_t proto_sw_cap[2]; /* SW control wifi protocol capability for any purpose */
	struct phy_sw_cap_t phy_sw_cap[2]; /* SW control phy capability for any purpose */
	struct phy_cap_t phy_cap[2]; /* final capability of phy (intersection of sw/hw) */

	struct dev_cap_t dev_sw_cap;
	struct dev_cap_t dev_cap; /* final capability of device (intersection of sw/hw) */

	struct bus_sw_cap_t bus_sw_cap; /* SW controlled bus capability */

	struct rtw_fw_info_t fw_info;
	struct rtw_evt_info_t evt_info;
	struct rtw_stats phl_stats;
	#ifdef CONFIG_PHL_DFS
	struct rtw_dfs_t dfs_info;
	#endif
	struct txpwr_regu_info_t txpwr_regu_info;
	struct rtw_iot_t id;
	/* Flags to control/check RX packets */
	bool append_fcs;
	bool accept_icv_err;

	u8 rf_type; /*enum rf_type , is RF_PATH - GET_HAL_RFPATH*/
	u8 rf_path_num; /*GET_HAL_RFPATH_NUM*/
	u8 regulation;  /*regulation*/
	u8 edcca_mode;

	struct rtw_tpe_info_t tpe_info; /* Transmit Power Envelope Information */
#ifdef CONFIG_PHL_CHANNEL_INFO
	struct rx_chan_info_pool *chan_info_pool;
	struct chan_info_t *chan_info; /* Handle channel info packet */
	struct rtw_chinfo_cur_parm *cur_parm;
#endif /* CONFIG_PHL_CHANNEL_INFO */
	void *p2pps_info;

	struct rtw_phl_ppdu_sts_info ppdu_sts_info;
	struct rtw_phl_rssi_stat rssi_stat;

#ifdef CONFIG_RTW_MIRROR_DUMP
	u32 mirror_cfg;
	u8 mirror_txch;
#endif
#ifdef CONFIG_PHL_THERMAL_PROTECT
	enum phl_thermal_protect_action thermal_protect_action;
#endif
	void *test_mgnt;

	void *phl_priv; /* pointer to phl_info */
	void *drv_priv;
#ifdef RTW_WKARD_BFEE_SET_AID
	u8 is_in_lps;
#endif

#ifdef PHL_RXSC_AMPDU
	struct rtw_rxsc_cache_entry rxsc_entry;
#endif

#ifdef DEBUG_PHL_TX
	struct phl_tx_stats tx_stats;
#endif
#ifdef DBG_DUMP_TX_COUNTER
	bool dbg_dump_tx;
	u8 dbg_dump_tx_bidx;
#endif
};

struct phl_sec_param_h {
	u8 keyid;
	u8 enc_type;
	u8 key_type;
	u8 key_len;
	u8 spp;
};

#define PHL_MAX_AGG_WSIZE 32

struct mp_usr_sw_tx_gen_in {
	u32 data_rate : 9;
	u32 mcs : 6;
	u32 mpdu_len : 14;
	u32 n_mpdu : 9;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd0 : 1;
	u32 aid : 12;
	u32 scrambler_seed : 8; // rand (1~255)
	u32 random_init_seed : 8; // rand (1~255)
	u32 rsvd1 : 4;
	u32 apep : 22;
	u32 ru_alloc : 8;
	u32 rsvd2 : 2;
	u32 nss : 4;
	u32 txbf : 1;
	u32 pwr_boost_db : 5;
	u32 rsvd3 : 22;
};


struct mp_sw_tx_param_t {
	u32 dbw : 2; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
	u32 source_gen_mode : 2;
	u32 locked_clk : 1;
	u32 dyn_bw : 1;
	u32 ndp_en : 1;
	u32 long_preamble_en : 1; //bmode
	u32 stbc : 1;
	u32 gi : 2; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 vht_txop_not_allowed : 1;
	u32 tb_disam : 1;
	u32 doppler : 2;
	u32 he_ltf_type : 2;//0:1x,1:2x,2:4x

	u32 ht_l_len : 12;
	u32 preamble_puncture : 1;
	u32 he_mcs_sigb : 3;//0~5
	u32 he_dcm_sigb : 1;
	u32 he_sigb_compress_en : 1;
	u32 max_tx_time_0p4us : 14;


	u32 ul_flag : 1;
	u32 tb_ldpc_extra : 1;
	u32 bss_color : 6;
	u32 sr : 4;
	u32 beamchange_en : 1;
	u32 he_er_u106ru_en : 1;
	u32 ul_srp1 : 4;
	u32 ul_srp2 : 4;
	u32 ul_srp3 : 4;
	u32 ul_srp4 : 4;
	u32 mode : 2;

	u32 group_id : 6;
	u32 ppdu_type : 4;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop : 7;
	u32 tb_strt_sts : 3;
	u32 tb_pre_fec_padding_factor : 2;
	u32 cbw : 2;
	u32 txsc : 4;
	u32 tb_mumimo_mode_en : 1;
	u32 rsvd1 : 3;

	u8 nominal_t_pe : 2; // def = 2
	u8 ness : 2; // def = 0
	u8 rsvd2 : 4;

	u8 n_user;
	u16 tb_rsvd : 9;//def = 0
	u16 rsvd3 : 7;

	struct mp_usr_sw_tx_gen_in usr[4];
};

struct mp_usr_plcp_gen_in {
	u32 mcs : 6;
	u32 mpdu_len : 14;
	u32 n_mpdu : 9;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd0 : 1;
	u32 aid : 12;
	u32 scrambler_seed : 8; // rand (1~255)
	u32 random_init_seed : 8; // rand (1~255)
	u32 rsvd1 : 4;
	u32 apep : 22;
	u32 ru_alloc : 8;
	u32 rsvd2 : 2;
	u32 nss : 4;
	u32 txbf : 1;
	u32 pwr_boost_db : 5;
	u32 rsvd3 : 22;
};

enum pkt_ofld_type {
	PKT_TYPE_PROBE_RSP = 0,
	PKT_TYPE_PS_POLL = 1,
	PKT_TYPE_NULL_DATA = 2,
	PKT_TYPE_QOS_NULL = 3,
	PKT_TYPE_CTS2SELF = 4,
	PKT_TYPE_ARP_RSP = 5,
	PKT_TYPE_NDP = 6,
	PKT_TYPE_EAPOL_KEY = 7,
	PKT_TYPE_SA_QUERY = 8,
	PKT_TYPE_REALWOW_KAPKT = 9, /* RealWoW Keep Alive Packet */
	PKT_TYPE_REALWOW_ACK = 10, /* RealWoW Ack Patten */
	PKT_TYPE_REALWOW_WP = 11, /* RealWoW Wakeup Patten */
	PKT_TYPE_PROBE_REQ = 12,
	PKT_OFLD_TYPE_MAX,
};

#ifdef CONFIG_PHL_SCANOFLD
struct scan_ofld_ch_info {
	enum band_type band;
	u8 center_chan;
	u8 chan; /* primary channel */
	u8 bw;
	u8 period;
	bool tx_pkt; /* 1:probe request will be sent */
	bool tx_data_pause; /* 1:no data will be sent during fw scanning */
	u8 probe_req_id;
	bool tx_null; /* null 0 when entering, null 1 when leaving */
	u8 enter_notify;
	u8 chkpt_time; /* 5 ms unit, c2h event every chkpt_time*/
};

enum SCAN_OFLD_OP {
	SCAN_OFLD_OP_STOP,
	SCAN_OFLD_OP_START,
	SCAN_OFLD_OP_SET,
	SCAN_OFLD_OP_RPT
};

enum SCAN_OFLD_MD {
	/* scan once */
	SCAN_OFLD_MD_ONCE,
	/**
	 * normal period repeatd
	 * e.g., period = 2s
	 * scan - 2s - scan - 2s -....
	 */

	SCAN_OFLD_MD_PD,
	/**
	 * slow period repeat
	 * e.g., period = 2s, cycle = 2, slow period = 3s
	 * scan - 2s - scan - 2s - scan - 3s - scan - 3s
	 */
	SCAN_OFLD_MD_PD_SLOW,
	/**
	 * seamless
	 * scan - scan - scan - scan - scan  -....
	 */
	SCAN_OFLD_MD_SEEMLESS,
};

struct scan_ofld_info {
	enum SCAN_OFLD_OP operation;
	enum SCAN_OFLD_MD mode;
	/* destinated tsf to start scanning, set 0 for both to scan immediately*/
	u32 tsf_low;
	u32 tsf_high;
	u32 period; /* SCAN_OFLD_MD_PD & SCAN_OFLD_MD_PD_SLOW */
	u8 cycle; /* SCAN_OFLD_MD_PD & SCAN_OFLD_MD_PD_SLOW*/
	u32 slow_period; /* SCAN_OFLD_MD_PD_SLOW */
	u8 end_notify;
	bool tgt_mode;
	struct rtw_chan_def tgt_chandef;
};
#endif

struct mp_plcp_param_t {
	u32 dbw : 2; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80
	u32 source_gen_mode : 2;
	u32 locked_clk : 1;
	u32 dyn_bw : 1;
	u32 ndp_en : 1;
	u32 long_preamble_en : 1; //bmode
	u32 stbc : 1;
	u32 gi : 2; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 vht_txop_not_allowed : 1;
	u32 tb_disam : 1;
	u32 doppler : 2;
	u32 he_ltf_type : 2;//0:1x,1:2x,2:4x

	u32 ht_l_len : 12;
	u32 preamble_puncture : 1;
	u32 he_mcs_sigb : 3;//0~5
	u32 he_dcm_sigb : 1;
	u32 he_sigb_compress_en : 1;
	u32 max_tx_time_0p4us : 14;


	u32 ul_flag : 1;
	u32 tb_ldpc_extra : 1;
	u32 bss_color : 6;
	u32 sr : 4;
	u32 beamchange_en : 1;
	u32 he_er_u106ru_en : 1;
	u32 ul_srp1 : 4;
	u32 ul_srp2 : 4;
	u32 ul_srp3 : 4;
	u32 ul_srp4 : 4;
	u32 mode : 2;

	u32 group_id : 6;
	u32 ppdu_type : 4;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop : 7;
	u32 tb_strt_sts : 3;
	u32 tb_pre_fec_padding_factor : 2;
	u32 cbw : 2;
	u32 txsc : 4;
	u32 tb_mumimo_mode_en : 1;
	u32 rsvd1 : 3;

	u8 nominal_t_pe : 2; // def = 2
	u8 ness : 2; // def = 0
	u8 rsvd2 : 4;

	u8 n_user;
	u16 tb_rsvd : 9;//def = 0
	u16 rsvd3 : 7;

	struct mp_usr_plcp_gen_in usr[4];
};



#define MP_MAC_AX_MAX_RU_NUM	4

struct mp_mac_ax_tf_depend_user_para {
	u8 pref_AC: 2;
	u8 rsvd: 6;
};

struct mp_mac_ax_tf_user_para {
	u16 aid12: 12;
	u16 ul_mcs: 4;
	u8 macid;
	u8 ru_pos;

	u8 ul_fec_code: 1;
	u8 ul_dcm: 1;
	u8 ss_alloc: 6;
	u8 ul_tgt_rssi: 7;
	u8 rsvd: 1;
	u16 rsvd2;
};


struct mp_mac_ax_tf_pkt_para {
	u8 ul_bw: 2;
	u8 gi_ltf: 2;
	u8 num_he_ltf: 3;
	u8 ul_stbc: 1;
	u8 doppler: 1;
	u8 ap_tx_power: 6;
	u8 rsvd0: 1;
	u8 user_num: 3;
	u8 pktnum: 3;
	u8 rsvd1: 2;
	u8 pri20_bitmap;

	struct mp_mac_ax_tf_user_para user[MP_MAC_AX_MAX_RU_NUM];
	struct mp_mac_ax_tf_depend_user_para dep_user[MP_MAC_AX_MAX_RU_NUM];
};

struct mp_mac_ax_tf_wd_para {
	u16 datarate: 9;
	u16 mulport_id: 3;
	u16 pwr_ofset: 3;
	u16 rsvd: 1;
};

struct mp_mac_ax_f2p_test_para {
	struct mp_mac_ax_tf_pkt_para tf_pkt;
	struct mp_mac_ax_tf_wd_para tf_wd;
	u8 mode: 2;
	u8 frexch_type: 6;
	u8 sigb_len;
};

struct mp_mac_ax_f2p_wd {
	/* dword 0 */
	u32 cmd_qsel:6;
	u32 rsvd0:2;
	u32 rsvd1:2;
	u32 ls:1;
	u32 fs:1;
	u32 total_number:4;
	u32 seq:8;
	u32 length:8;
	/* dword 1 */
	u32 rsvd2;
};

struct mp_mac_ax_f2p_tx_cmd {
	/* dword 0 */
	u32 cmd_type:8;
	u32 cmd_sub_type:8;
	u32 dl_user_num:5;
	u32 bw:2;
	u32 tx_power:9;
	/* dword 1 */
	u32 fw_define:16;
	u32 ss_sel_mode:2;
	u32 next_qsel:6;
	u32 twt_group:4;
	u32 dis_chk_slp:1;
	u32 ru_mu_2_su:1;
	u32 dl_t_pe:2;
	/* dword 2 */
	u32 sigb_ch1_len:8;
	u32 sigb_ch2_len:8;
	u32 sigb_sym_num:6;
	u32 sigb_ch2_ofs:5;
	u32 dis_htp_ack:1;
	u32 tx_time_ref:2;
	u32 pri_user_idx:2;
	/* dword 3 */
	u32 ampdu_max_txtime:14;
	u32 rsvd0:2;
	u32 group_id:6;
	u32 rsvd1:2;
	u32 rsvd2:4;
	u32 twt_chk_en:1;
	u32 twt_port_id:3;
	/* dword 4 */
	u32 twt_start_time:32;
	/* dword 5 */
	u32 twt_end_time:32;
	/* dword 6 */
	u32 apep_len:12;
	u32 tri_pad:2;
	u32 ul_t_pe:2;
	u32 rf_gain_idx:10;
	u32 fixed_gain_en:1;
	u32 ul_gi_ltf:3;
	u32 ul_doppler:1;
	u32 ul_stbc:1;
	/* dword 7 */
	u32 ul_mid_per:1;
	u32 ul_cqi_rrp_tri:1;
	u32 rsvd3:6;
	u32 rsvd4:8;
	u32 sigb_dcm:1;
	u32 sigb_comp:1;
	u32 doppler:1;
	u32 stbc:1;
	u32 mid_per:1;
	u32 gi_ltf_size:3;
	u32 sigb_mcs:3;
	u32 rsvd5:5;
	/* dword 8 */
	u32 macid_u0:8;
	u32 ac_type_u0:2;
	u32 mu_sta_pos_u0:2;
	u32 dl_rate_idx_u0:9;
	u32 dl_dcm_en_u0:1;
	u32 rsvd6:2;
	u32 ru_alo_idx_u0:8;
	/* dword 9 */
	u32 pwr_boost_u0:5;
	u32 agg_bmp_alo_u0:3;
	u32 ampdu_max_txnum_u0:8;
	u32 user_define_u0:8;
	u32 user_define_ext_u0:8;
	/* dword 10 */
	u32 ul_addr_idx_u0:8;
	u32 ul_dcm_u0:1;
	u32 ul_fec_cod_u0:1;
	u32 ul_ru_rate_u0:7;
	u32 rsvd8:7;
	u32 ul_ru_alo_idx_u0:8;
	/* dword 11 */
	u32 rsvd9:32;
	/* dword 12 */
	u32 macid_u1:8;
	u32 ac_type_u1:2;
	u32 mu_sta_pos_u1:2;
	u32 dl_rate_idx_u1:9;
	u32 dl_dcm_en_u1:1;
	u32 rsvd10:2;
	u32 ru_alo_idx_u1:8;
	/* dword 13 */
	u32 pwr_boost_u1:5;
	u32 agg_bmp_alo_u1:3;
	u32 ampdu_max_txnum_u1:8;
	u32 user_define_u1:8;
	u32 user_define_ext_u1:8;
	/* dword 14 */
	u32 ul_addr_idx_u1:8;
	u32 ul_dcm_u1:1;
	u32 ul_fec_cod_u1:1;
	u32 ul_ru_rate_u1:7;
	u32 rsvd12:7;
	u32 ul_ru_alo_idx_u1:8;
	/* dword 15 */
	u32 rsvd13:32;
	/* dword 16 */
	u32 macid_u2:8;
	u32 ac_type_u2:2;
	u32 mu_sta_pos_u2:2;
	u32 dl_rate_idx_u2:9;
	u32 dl_dcm_en_u2:1;
	u32 rsvd14:2;
	u32 ru_alo_idx_u2:8;
	/* dword 17 */
	u32 pwr_boost_u2:5;
	u32 agg_bmp_alo_u2:3;
	u32 ampdu_max_txnum_u2:8;
	u32 user_define_u2:8;
	u32 user_define_ext_u2:8;
	/* dword 18 */
	u32 ul_addr_idx_u2:8;
	u32 ul_dcm_u2:1;
	u32 ul_fec_cod_u2:1;
	u32 ul_ru_rate_u2:7;
	u32 rsvd16:7;
	u32 ul_ru_alo_idx_u2:8;
	/* dword 19 */
	u32 rsvd17:32;
	/* dword 20 */
	u32 macid_u3:8;
	u32 ac_type_u3:2;
	u32 mu_sta_pos_u3:2;
	u32 dl_rate_idx_u3:9;
	u32 dl_dcm_en_u3:1;
	u32 rsvd18:2;
	u32 ru_alo_idx_u3:8;
	/* dword 21 */
	u32 pwr_boost_u3:5;
	u32 agg_bmp_alo_u3:3;
	u32 ampdu_max_txnum_u3:8;
	u32 user_define_u3:8;
	u32 user_define_ext_u3:8;
	/* dword 22 */
	u32 ul_addr_idx_u3:8;
	u32 ul_dcm_u3:1;
	u32 ul_fec_cod_u3:1;
	u32 ul_ru_rate_u3:7;
	u32 rsvd20:7;
	u32 ul_ru_alo_idx_u3:8;
	/* dword 23 */
	u32 rsvd21:32;
	/* dword 24 */
	u32 pkt_id_0:12;
	u32 rsvd22:3;
	u32 valid_0:1;
	u32 ul_user_num_0:4;
	u32 rsvd23:12;
	/* dword 25 */
	u32 pkt_id_1:12;
	u32 rsvd24:3;
	u32 valid_1:1;
	u32 ul_user_num_1:4;
	u32 rsvd25:12;
	/* dword 26 */
	u32 pkt_id_2:12;
	u32 rsvd26:3;
	u32 valid_2:1;
	u32 ul_user_num_2:4;
	u32 rsvd27:12;
	/* dword 27 */
	u32 pkt_id_3:12;
	u32 rsvd28:3;
	u32 valid_3:1;
	u32 ul_user_num_3:4;
	u32 rsvd29:12;
	/* dword 28 */
	u32 pkt_id_4:12;
	u32 rsvd30:3;
	u32 valid_4:1;
	u32 ul_user_num_4:4;
	u32 rsvd31:12;
	/* dword 29 */
	u32 pkt_id_5:12;
	u32 rsvd32:3;
	u32 valid_5:1;
	u32 ul_user_num_5:4;
	u32 rsvd33:12;
};

u8 mp_start(void *priv);


struct rtw_role_cmd {
	struct rtw_wifi_role_t *wrole;
	enum link_state lstate;
};

struct role_ntfy_info {
	u8 role_id;
	u16 macid;
	enum role_state rstate;
};

struct battery_chg_ntfy_info {
	bool ips_allow;
	bool lps_allow;
};

struct ps_ntfy_info {
	bool sync;
	void *ctx;
	void (*cb)(void *phl, void *hdl, void *ctx, enum rtw_phl_status stat);
};

struct set_rf_ntfy_info {
	enum rtw_rf_state state_to_set;
	_os_event done;
};


/**
 * rtw_phl_rainfo - structure use to query RA information
 * from hal layer to core/phl layer
 * @rate: current rate selected by RA, define by general definition enum rtw_data_rate
 * @bw: current BW, define by general definition enum channel_width
 * @gi_ltf: current gi_ltf, define by general definition enum rtw_gi_ltf
 * @is_actrl: report by RA,0: don't append a-ctrl field; 1: could append a-ctrl field
 */
struct rtw_phl_rainfo {
	enum rtw_data_rate rate;
	enum channel_width bw;
	enum rtw_gi_ltf gi_ltf;
	bool is_actrl;
};

struct rtw_pcie_trx_mit_info_t {
	u32 tx_timer;
	u8 tx_counter;
	u32 rx_timer;
	u8 rx_counter;
	u8 fixed_mitigation; /*no watchdog dynamic setting*/
	u8 rx_mit_counter_high;
	u32 rx_mit_timer_high;
	void *priv;
};

struct rtw_env_report {
	bool rpt_status; /*1 means CCX_SUCCESS,0 means fail*/
	u8 clm_ratio;
	u8 nhm_ratio;
	u8 nhm_cca_ratio; /* = clm_ratio */
	u8 nhm_idle_ratio;
	u16 nhm_tx_cnt;
	u16 nhm_cca_cnt;
	u16 nhm_idle_cnt;
	u8 nhm_tx_ratio;
	u8 nhm_pwr;
};

struct rtw_tx_pkt_rpt {
	u16 macid;
	u8 seq_no;
};

#ifdef CONFIG_PHL_DFS_SWITCH_CH_WITH_CSA
struct rtw_csa_cntdown_rpt {
	u8 port:7;	/* BIT0 - BIT6 => {port0_mbssid0, port0_mbssid1, ..., port0_mbssid15, port1, port2, port3, port4} */
	u8 band:1;	/* BIT7 => band (0 or 1) */
};
#endif

struct rtw_phl_port_tsf{
	u32 tsf_l;
	u32 tsf_h;
	u8 hwband;
	u8 port;
};

enum rtw_phl_ser_lv1_recv_step {
	RTW_PHL_SER_LV1_RCVY_STEP_1 = 0,
	RTW_PHL_SER_LV1_SER_RCVY_STEP_2,

	/* keep last */
	RTW_PHL_SER_LV1_RCVY_STEP_LAST,
	RTW_PHL_SER_LV1_RCVY_STEP_MAX = RTW_PHL_SER_LV1_RCVY_STEP_LAST,
	RTW_PHL_SER_LV1_RCVY_STEP_INVALID = RTW_PHL_SER_LV1_RCVY_STEP_LAST,
};

/*
  struct rtw_phl_ml_ie_info:
   @ pkt_type: packet type
   @ rlink: the link that builds a ML element
   @ mld: the MLD that @rlink is affiliated with
   @ critical_update: is a critical update or not while updating beacon
   @ mld_id_present: to include MLD ID subfield in Common Info field or not
   @ mld_id: MLD ID subfield in Common Info field
   @ opt: optional subelements pointer
   @ opt_len: length of @opt
*/
struct rtw_phl_ml_ie_info {
	enum phl_packet_type pkt_type;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_mld_t *mld;

	/* Critical Update for Basic ML */
	u8 critical_update;

	/* ML Control of Probe Request ML */
	u8 mld_id_present;
	u8 mld_id;

	/* optional subelements pointer */
	u8 *opt;
	u8 opt_len;
};

/*
  struct rtw_phl_per_sta_profile_info:
   @ pkt_type: packet type
   @ rlink: the link that is included in a Per-STA Profile
   @ mld: the MLD that @rlink is affiliated with
   @ link_id: to specify the link ID in STA Control field
              (request direction)
   @ complete_profile: to indicate the Per-STA Profile is complete or not
   @ sta_profile: STA Profile field pointer from core layer
   @ sta_profile_len: length of @sta_profile
*/
struct rtw_phl_per_sta_profile_info {
	enum phl_packet_type pkt_type;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_mld_t *mld;

	/* STA Control */
	u8 link_id;
	u8 complete_profile;

	/* STA Profile */
	u8 *sta_profile;
	u8 sta_profile_len;
};

enum phl_pkt_evt_type {
	PKT_EVT_DHCP,
	PKT_EVT_ARP,
	PKT_EVT_TX_PING_REQ,
	PKT_EVT_RX_PING_RSP,
	PKT_EVT_EAPOL_START,
	PKT_EVT_EAPOL_END,
	PKT_EVT_MAX
};

struct rtw_phl_hw_cts2self_cfg {
	u8 band_sel;
	u8 enable;
	u8 non_sec_threshold;
	u8 sec_threshold;
};


#endif /*_PHL_DEF_H_*/
