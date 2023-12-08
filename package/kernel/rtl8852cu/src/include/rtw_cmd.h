/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef __RTW_CMD_H_
#define __RTW_CMD_H_


#define C2H_MEM_SZ (16*1024)


#define MAX_CMDSZ	1536
#define MAX_RSPSZ	512
#define MAX_EVTSZ	1024

#define CMDBUFF_ALIGN_SZ 512

struct cmd_obj {
	_adapter *padapter;
	/*
	* band_idx, could be used when padapter is not up and operated on specific hwband
	* >= HW_BAND_MAX: not specified (get hwband by adapter/alink)
	*/
	u8 band_idx;

	u16	cmdcode;
	u32 cmd_token;
	u8	res;
	u8	*parmbuf;
	u32	cmdsz;
	u8	*rsp;
	u32	rspsz;
	struct submit_ctx *sctx;
	u8 no_io;
	/* _sema 	cmd_sem; */
	_list	list;
};

#define CMD_OBJ_SET_HWBAND(cmdobj, hwband) do { (cmdobj)->band_idx = hwband; } while (0)

/* cmd flags */
enum {
	RTW_CMDF_DIRECTLY = BIT0,
	RTW_CMDF_WAIT_ACK = BIT1,
};

struct cmd_priv {
	u8	cmd_seq;
	u8	*cmd_buf;	/* shall be non-paged, and 4 bytes aligned */
	u8	*cmd_allocated_buf;
	u8	*rsp_buf;	/* shall be non-paged, and 4 bytes aligned */
	u8	*rsp_allocated_buf;
	u32	cmd_issued_cnt;

	struct dvobj_priv *dvobj;
	_mutex sctx_mutex;
	ATOMIC_T event_seq;
	u32 evt_done_cnt;

	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	_queue	cmd_queue;
	_sema	cmd_queue_sema;
	_sema	start_cmdthread_sema;
	ATOMIC_T cmdthd_running;
	#endif
};

struct back_op_param {
	unsigned int off_ch_dur;
	unsigned int off_ch_ext_dur; /* extend when MGNT_TX */
	unsigned int on_ch_dur;
};

#define init_h2fwcmd_w_parm_no_rsp(pcmd, pparm, code) \
	do {\
		_rtw_init_listhead(&pcmd->list);\
		CMD_OBJ_SET_HWBAND(pcmd, HW_BAND_MAX);\
		pcmd->cmdcode = code;\
		pcmd->parmbuf = (u8 *)(pparm);\
		pcmd->cmdsz = sizeof (*pparm);\
		pcmd->rsp = NULL;\
		pcmd->rspsz = 0;\
	} while (0)

#define init_h2fwcmd_w_parm_no_parm_rsp(pcmd, code) \
	do {\
		_rtw_init_listhead(&pcmd->list);\
		CMD_OBJ_SET_HWBAND(pcmd, HW_BAND_MAX);\
		pcmd->cmdcode = code;\
		pcmd->parmbuf = NULL;\
		pcmd->cmdsz = 0;\
		pcmd->rsp = NULL;\
		pcmd->rspsz = 0;\
	} while (0)

struct P2P_PS_Offload_t {
	u8 Offload_En:1;
	u8 role:1; /* 1: Owner, 0: Client */
	u8 CTWindow_En:1;
	u8 NoA0_En:1;
	u8 NoA1_En:1;
	u8 AllStaSleep:1; /* Only valid in Owner */
	u8 discovery:1;
	u8 rsvd:1;
#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
	u8 p2p_macid:7;
	u8 disable_close_rf:1; /*1: not close RF but just pause p2p_macid when NoA duration*/
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */
};

struct P2P_PS_CTWPeriod_t {
	u8 CTWPeriod;	/* TU */
};

#ifdef CONFIG_P2P_WOWLAN

struct P2P_WoWlan_Offload_t {
	u8 Disconnect_Wkup_Drv:1;
	u8 role:2;
	u8 Wps_Config[2];
};

#endif /* CONFIG_P2P_WOWLAN */

extern u32 rtw_enqueue_cmd(struct cmd_priv *pcmdpriv, struct cmd_obj *obj);

extern void rtw_free_cmd_obj(struct cmd_obj *pcmd);

#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
void rtw_stop_cmd_thread(_adapter *adapter);
thread_return rtw_cmd_thread(thread_context context);
#endif

u32 rtw_init_cmd_priv(struct dvobj_priv *dvobj);
void rtw_free_cmd_priv(struct dvobj_priv *dvobj);

#ifdef CONFIG_IOCTL_CFG80211
u8 rtw_mgnt_tx_cmd(_adapter *adapter, u8 tx_band, u8 tx_ch, u8 no_cck, const u8 *buf, size_t len, int wait_ack, u8 flags);
struct mgnt_tx_parm {
	u8 tx_band;
	u8 tx_ch;
	u8 no_cck;
	const u8 *buf;
	size_t len;
	int wait_ack;
};
#endif

enum rtw_drvextra_cmd_id {
	NONE_WK_CID, /*MCC_CMD_WK_CID*/
	STA_MSTATUS_RPT_WK_CID,
	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	DYNAMIC_CHK_WK_CID,
	#endif
	DM_CTRL_WK_CID,
	PBC_POLLING_WK_CID,
	LPS_CTRL_WK_CID,
	ANT_SELECT_WK_CID,
	P2P_PS_WK_CID,
	CHECK_HIQ_WK_CID,/* for softap mode, check hi queue if empty */
	C2H_WK_CID,
	RESET_SECURITYPRIV, /* add for CONFIG_IEEE80211W, none 11w also can use */
	FREE_ASSOC_RESOURCES, /* add for CONFIG_IEEE80211W, none 11w also can use */
	DM_IN_LPS_WK_CID,
	LPS_CHANGE_DTIM_CID,
	DFS_RADAR_DETECT_WK_CID,
	DFS_RADAR_DETECT_EN_DEC_WK_CID,
	SESSION_TRACKER_WK_CID,
	TEST_H2C_CID,
	MP_CMD_WK_CID,
	CUSTOMER_STR_WK_CID,
	MGNT_TX_WK_CID,
	REQ_PER_CMD_WK_CID,
	SSMPS_WK_CID,
#ifdef CONFIG_CTRL_TXSS_BY_TP
	TXSS_WK_CID,
#endif
	AC_PARM_CMD_WK_CID,
#ifdef CONFIG_AP_MODE
	STOP_AP_WK_CID,
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	TBTX_CONTROL_TX_WK_CID,
#endif
	TSF_SYNC_DONE_WK_CID,
#ifdef PRIVATE_R
	FIND_REMOTE_WK_CID,
#ifdef CONFIG_P2P
	HIDE_SSID_WK_CID,
#endif
#endif
	MAX_WK_CID
};

enum LPS_CTRL_TYPE {
	LPS_CTRL_SCAN = 0,
	LPS_CTRL_JOINBSS = 1,
	LPS_CTRL_CONNECT = 2,
	LPS_CTRL_DISCONNECT = 3,
	LPS_CTRL_SPECIAL_PACKET = 4,
	LPS_CTRL_LEAVE = 5,
	LPS_CTRL_TRAFFIC_BUSY = 6,
	LPS_CTRL_TX_TRAFFIC_LEAVE = 7,
	LPS_CTRL_RX_TRAFFIC_LEAVE = 8,
	LPS_CTRL_ENTER = 9,
	LPS_CTRL_LEAVE_CFG80211_PWRMGMT = 10,
	LPS_CTRL_LEAVE_SET_OPTION = 11,
};

enum STAKEY_TYPE {
	GROUP_KEY		= 0,
	UNICAST_KEY		= 1,
	TDLS_KEY		= 2,
};

enum RFINTFS {
	SWSI,
	HWSI,
	HWPI,
};

/*
Caller Mode: Infra, Ad-Hoc

Notes: To join the specified bss

Command Event Mode

*/
struct joinbss_parm {
	WLAN_BSSID_EX network;
};

/*
Caller Mode: Infra, Ad-HoC(C)

Notes: To disconnect the current associated BSS

Command Mode

*/
struct disconnect_parm {
	u32 deauth_timeout_ms;
};

/*
Caller Mode: AP, Ad-HoC(M)

Notes: To create a BSS

Command Mode
*/
struct createbss_parm {
	bool adhoc;

	/* used by AP/Mesh mode now */
	u8 ifbmp;
	u8 excl_ifbmp;
	s8 req_band;
	s16 req_ch;
	s8 req_bw;
	s8 req_offset;
	u8 ifbmp_ch_changed;
	u8 ch_to_set;
	u8 offset_to_set;
	u8 bw_to_set;
	u8 do_rfk;
	u8 is_change_chbw;
	#ifdef CONFIG_DBCC_SUPPORT
	u8 is_trigger_dbcc;
	#endif
};


struct	setopmode_parm {
	u8	mode;
	u8	rsvd[3];
};


/*
Caller Mode: Any

Notes: To set the auth type of RTL8711. open/shared/802.1x

Command Mode

*/
struct setauth_parm {
	u8 mode;  /* 0: legacy open, 1: legacy shared 2: 802.1x */
	u8 _1x;   /* 0: PSK, 1: TLS */
	u8 rsvd[2];
};

/*
Caller Mode: Infra

a. algorithm: wep40, wep104, tkip & aes
b. keytype: grp key/unicast key
c. key contents

when shared key ==> keyid is the camid
when 802.1x ==> keyid [0:1] ==> grp key
when 802.1x ==> keyid > 2 ==> unicast key

*/
struct setkey_parm {
	u8	algorithm;	/* encryption algorithm, could be none, wep40, TKIP, CCMP, wep104 */
	u8	keyid;
	u8	set_tx;		/* 1: main tx key for wep. 0: other key. */
	u8	key[32];	/* this could be 40 or 104 */
};

/*
When in AP or Ad-Hoc mode, this is used to
allocate an sw/hw entry for a newly associated sta.

Command

when shared key ==> algorithm/keyid

*/
struct set_stakey_parm {
	u8 addr[ETH_ALEN];
	u8 algorithm;
	u8 keyid;
	u8 key[32];
	u8 gk;
};

struct set_stakey_rsp {
	u8	addr[ETH_ALEN];
	u8	keyid;
	u8	rsvd;
};

struct Tx_Beacon_param {
	WLAN_BSSID_EX network;
};

/*
	Notes: This command is used for H2C/C2H loopback testing

	mac[0] == 0
	==> CMD mode, return H2C_SUCCESS.
	The following condition must be ture under CMD mode
		mac[1] == mac[4], mac[2] == mac[3], mac[0]=mac[5]= 0;
		s0 == 0x1234, s1 == 0xabcd, w0 == 0x78563412, w1 == 0x5aa5def7;
		s2 == (b1 << 8 | b0);

	mac[0] == 1
	==> CMD_RSP mode, return H2C_SUCCESS_RSP

	The rsp layout shall be:
	rsp:			parm:
		mac[0]  =   mac[5];
		mac[1]  =   mac[4];
		mac[2]  =   mac[3];
		mac[3]  =   mac[2];
		mac[4]  =   mac[1];
		mac[5]  =   mac[0];
		s0		=   s1;
		s1		=   swap16(s0);
		w0		=	swap32(w1);
		b0		=	b1
		s2		=	s0 + s1
		b1		=	b0
		w1		=	w0

	mac[0] ==	2
	==> CMD_EVENT mode, return	H2C_SUCCESS
	The event layout shall be:
	event:			parm:
		mac[0]  =   mac[5];
		mac[1]  =   mac[4];
		mac[2]  =   event's sequence number, starting from 1 to parm's marc[3]
		mac[3]  =   mac[2];
		mac[4]  =   mac[1];
		mac[5]  =   mac[0];
		s0		=   swap16(s0) - event.mac[2];
		s1		=   s1 + event.mac[2];
		w0		=	swap32(w0);
		b0		=	b1
		s2		=	s0 + event.mac[2]
		b1		=	b0
		w1		=	swap32(w1) - event.mac[2];

		parm->mac[3] is the total event counts that host requested.


	event will be the same with the cmd's param.

*/

/* CMD param Formart for driver extra cmd handler */
struct drvextra_cmd_parm {
	struct _ADAPTER_LINK *adapter_link;
	int ec_id; /* extra cmd id */
	int type; /* Can use this field as the type id or command size */
	int size; /* buffer size */
	unsigned char *pbuf;
};

/*------------------- Below are used for RF/BB tunning ---------------------*/
struct addBaReq_parm {
	unsigned int tid;
	u8	addr[ETH_ALEN];
};

struct addBaRsp_parm {
	unsigned int tid;
	unsigned int start_seq;
	u8 addr[ETH_ALEN];
	u8 status;
	u8 size;
	struct ADDBA_request preq;
};

struct set_ch_parm {
	enum band_type band;
	u8 ch;
	u8 bw;
	u8 ch_offset;
	u8 do_rfk;
	u8 link_idx;
};

struct LedBlink_param {
	void *pLed;
};

struct TDLSoption_param {
	u8 addr[ETH_ALEN];
	u8 option;
};

struct RunInThread_param {
	void (*func)(void *);
	void *context;
};


#define GEN_CMD_CODE(cmd)	cmd ## _CMD_


/*

Result:
0x00: success
0x01: sucess, and check Response.
0x02: cmd ignored due to duplicated sequcne number
0x03: cmd dropped due to invalid cmd code
0x04: reserved.

*/

#define H2C_RSP_OFFSET			512

#define H2C_SUCCESS			0x00
#define H2C_SUCCESS_RSP			0x01
#define H2C_DUPLICATED			0x02
#define H2C_DROPPED			0x03
#define H2C_PARAMETERS_ERROR		0x04
#define H2C_REJECTED			0x05
#define H2C_CMD_OVERFLOW		0x06
#define H2C_RESERVED			0x07
#define H2C_ENQ_HEAD			0x08
#define H2C_ENQ_HEAD_FAIL		0x09
#define H2C_CMD_FAIL			0x0A
#define H2C_MEMORY				0x0B

u8 rtw_create_ibss_cmd(_adapter *adapter, int flags);
u8 rtw_startbss_cmd(_adapter *adapter, int flags);

#define REQ_BAND_NONE		-1
#define REQ_CH_NONE		-1
#define REQ_CH_INT_INFO	-2
#define REQ_BW_NONE		-1
#define REQ_BW_ORI		-2
#define REQ_OFFSET_NONE	-1

u8 rtw_change_bss_bchbw_cmd(_adapter *adapter, int flags
	, u8 ifbmp, u8 excl_ifbmp, s8 req_band, s16 req_ch, s8 req_bw, s8 req_offset);

struct sta_info;
extern u8 rtw_setstakey_cmd(_adapter  *padapter, struct sta_info *sta, u8 key_type, bool enqueue);
extern u8 rtw_clearstakey_cmd(_adapter *padapter, struct sta_info *sta, u8 enqueue);

extern u8 rtw_joinbss_cmd(_adapter  *padapter, struct wlan_network *pnetwork);
u8 rtw_disassoc_cmd(_adapter *padapter, u32 deauth_timeout_ms, int flags);
#ifdef CONFIG_AP_MODE
u8 rtw_stop_ap_cmd(_adapter *adapter, u8 flags);
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 rtw_tx_control_cmd(_adapter *adapter);
#endif
extern u8 rtw_setopmode_cmd(_adapter  *padapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype, u8 flags);

extern u8 rtw_addbareq_cmd(_adapter *padapter, u8 tid, u8 *addr);
extern u8 rtw_addbarsp_cmd(_adapter *padapter, u8 *addr, u16 tid,
			   struct ADDBA_request *paddba_req, u8 status, u8 size,
			   u16 start_seq);
extern u8 rtw_delba_cmd(struct _ADAPTER *a, u8 *addr, u16 tid);
/* add for CONFIG_IEEE80211W, none 11w also can use */
extern u8 rtw_reset_securitypriv_cmd(_adapter *padapter);
extern u8 rtw_free_assoc_resources_cmd(_adapter *padapter, u8 lock_scanned_queue, int flags);
#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
extern u8 rtw_dynamic_chk_wk_cmd(_adapter *adapter);
#endif
#ifdef PRIVATE_R
extern u8 rtw_find_remote_wk_cmd(_adapter *adapter);
#ifdef CONFIG_P2P
extern u8 rtw_hide_ssid_wk_cmd(_adapter *adapter);
#endif
#endif

#ifdef CONFIG_ANTENNA_DIVERSITY
extern  u8 rtw_antenna_select_cmd(_adapter *padapter, u8 antenna, u8 enqueue);
#endif

#ifdef CONFIG_AP_MODE
u8 rtw_chk_hi_queue_cmd(_adapter *padapter);
#endif /* CONFIG_AP_MODE */

u8 rtw_test_h2c_cmd(_adapter *adapter, u8 *buf, u8 len);
#if defined(RTW_PHL_DBG_CMD)
void core_cmd_phl_handler(_adapter *adapter, char *extra);
void core_add_record(_adapter *adapter, u8 type, void *p);
void phl_add_record(void *d, u8 type, void *p, u32 size);
#endif

u8 rtw_set_chbw_cmd(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		enum band_type band, u8 ch, u8 bw, u8 ch_offset, u8 flags);

#ifdef CONFIG_RTW_LED_HANDLED_BY_CMD_THREAD
u8 rtw_led_blink_cmd(_adapter *padapter, void *pLed);
#endif
extern u8 rtw_tdls_cmd(_adapter *padapter, u8 *addr, u8 option);

u8 rtw_mp_cmd(_adapter *adapter, u8 mp_cmd_id, u8 flags);

#ifdef CONFIG_RTW_CUSTOMER_STR
u8 rtw_customer_str_req_cmd(_adapter *adapter);
u8 rtw_customer_str_write_cmd(_adapter *adapter, const u8 *cstr);
#endif

u8 rtw_c2h_packet_wk_cmd(_adapter *adapter, u8 *c2h_evt, u16 length);

u8 rtw_run_in_thread_cmd(_adapter *adapter, void (*func)(void *), void *context);
u8 rtw_run_in_thread_cmd_wait(_adapter *adapter, void (*func)(void *), void *context, s32 timeout_ms);

struct ssmps_cmd_parm {
	struct sta_info *sta;
	u8 smps;
};
u8 rtw_ssmps_wk_cmd(_adapter *adapter, struct sta_info *sta, u8 smps, u8 enqueue);

u8 session_tracker_chk_cmd(_adapter *adapter, struct sta_info *sta);
u8 session_tracker_add_cmd(_adapter *adapter, struct sta_info *sta, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port);
u8 session_tracker_del_cmd(_adapter *adapter, struct sta_info *sta, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port);

u8 set_txq_params_cmd(_adapter *adapter, struct _ADAPTER_LINK *padapter_link, u32 ac_parm, u8 ac_type);

#if defined(CONFIG_RTW_MESH) && defined(RTW_PER_CMD_SUPPORT_FW)
u8 rtw_req_per_cmd(_adapter * adapter);
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 rtw_tbtx_chk_cmd(_adapter *adapter);
u8 rtw_tbtx_token_dispatch_cmd(_adapter *adapter);
#endif
#ifdef CONFIG_CTRL_TXSS_BY_TP
struct txss_cmd_parm {
	struct sta_info *sta;
	bool tx_1ss;
};

void rtw_ctrl_txss_update(_adapter *adapter, struct sta_info *sta);
u8 rtw_ctrl_txss(_adapter *adapter, struct sta_info *sta, bool tx_1ss);
void rtw_ctrl_tx_ss_by_tp(_adapter *adapter, u8 from_timer);

#ifdef DBG_CTRL_TXSS
void dbg_ctrl_txss(_adapter *adapter, bool tx_1ss);
#endif
#endif

u8 rtw_drvextra_cmd_hdl(_adapter *padapter, unsigned char *pbuf);

extern void rtw_disassoc_cmd_callback(_adapter  *padapter, struct cmd_obj *pcmd);
void rtw_create_ibss_post_hdl(_adapter *padapter, int status);
extern void rtw_readtssi_cmdrsp_callback(_adapter	*padapter,  struct cmd_obj *pcmd);

extern void rtw_setstaKey_cmdrsp_callback(_adapter  *padapter,  struct cmd_obj *pcmd);
extern void rtw_getrttbl_cmdrsp_callback(_adapter  *padapter,  struct cmd_obj *pcmd);
void rtw_run_cmd(_adapter *padapter, struct cmd_obj *pcmd, bool discard);
u32 rtw_get_turbo_edca(_adapter *padapter, u8 aifs, u8 ecwmin, u8 ecwmax, u8 txop);

enum rtw_cmd_id {
	CMD_JOINBSS,
	CMD_DISCONNECT,
	CMD_CREATE_BSS,
	CMD_SET_OPMODE,
#ifdef CONFIG_FSM
	CMD_SITE_SURVEY,
#endif
	CMD_SET_AUTH,
#ifndef CONFIG_CMD_DISP
	CMD_SET_KEY,
	CMD_SET_STAKEY,
#endif
	CMD_ADD_BAREQ,
	CMD_SET_CHANNEL,
	CMD_TX_BEACON,
	CMD_SET_MLME_EVT,
	CMD_SET_DRV_EXTRA,
	CMD_SET_CHANPLAN,
	CMD_LEDBLINK,
	CMD_TDLS,
	CMD_CHK_BMCSLEEPQ,
	CMD_RUN_INTHREAD,
	CMD_ADD_BARSP,
	CMD_RM_POST_EVENT,
	CMD_SET_MESH_PLINK_STATE,
	CMD_DELBA,
	CMD_GET_CHANPLAN,
	CMD_ID_MAX
};

#define CMD_FMT "cmd=%d,%d,%d"
#define CMD_ARG(cmd) \
	(cmd)->cmdcode, \
	(cmd)->cmdcode == CMD_SET_DRV_EXTRA ? ((struct drvextra_cmd_parm *)(cmd)->parmbuf)->ec_id : ((cmd)->cmdcode == CMD_SET_MLME_EVT ? ((struct rtw_evt_header *)(cmd)->parmbuf)->id : 0), \
	(cmd)->cmdcode == CMD_SET_DRV_EXTRA ? ((struct drvextra_cmd_parm *)(cmd)->parmbuf)->type : 0

#ifdef CONFIG_CMD_GENERAL
void rtw_dynamic_chk_wk_sw_hdl(_adapter *padapter);
void rtw_dynamic_chk_wk_hw_hdl(_adapter *padapter);
#else
void rtw_dynamic_chk_wk_hdl(_adapter *padapter);
#endif
#endif /* _CMD_H_ */
