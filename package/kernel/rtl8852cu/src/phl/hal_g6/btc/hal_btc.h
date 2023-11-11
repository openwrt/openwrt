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
#ifndef _HAL_BTC_H_
#define _HAL_BTC_H_

#include "halbtc_fw.h"
#include "halbtc_dbg_cmd.h"

#ifdef CONFIG_RTL8852A
#define BTC_8852A_SUPPORT
#endif

#ifdef CONFIG_RTL8852B
#define BTC_8852B_SUPPORT
#endif

#ifdef CONFIG_RTL8852BP
#define BTC_8852B_SUPPORT
#define BTC_8852BP_SUPPORT
#endif

#ifdef CONFIG_RTL8851B
#define BTC_8852B_SUPPORT
#define BTC_8851B_SUPPORT
#endif

#ifdef CONFIG_RTL8852C
#define BTC_8852C_SUPPORT
#endif

#ifdef CONFIG_FW_IO_OFLD_SUPPORT
#define BTC_CONFIG_FW_IO_OFLD_SUPPORT
#endif

#define BTC_GPIO_DBG_EN 1
#define BTC_PHY_MAX 2
#define BTC_WL_TX_PWR_CTRL_OFLD
#define BTC_NON_SHARED_ANT_FREERUN 0
#define BTC_WL_MAX_ROLE_NUMBER 6  /* Must = MAX_WIFI_ROLE_NUMBER */

#define BTC_FDDT_INFO_MAP_DEF 0x1f

#define BTC_WL_TP_LOW_THRES 10

#define BTC_SCBD_REWRITE_DELAY 1000
#define BTC_MSG_MAXLEN 200
#define BTC_POLICY_MAXLEN 512
#define BTC_HUBMSG_MAXLEN 512
#define BTC_RPT_PERIOD 2000
#define BTC_RSN_MAXLEN 32
#define BTC_ACT_MAXLEN 32
#define BTC_DELAYED_PERIODIC_TIME 2000 /* ms, delayed to start periodic timer */
#define BTC_PERIODIC_TIME 100 /* 100ms, must be non zero and < 2000 */
#define BTC_WRFK_MAXT 300 /* WL RFK 300ms */
#define BTC_BRFK_MAXT 800 /* BT RFK 800ms */
#define BTC_SPECPKT_MAXT 4000 /* special pkt unit: 4000ms */
#define BTC_A2DP_RESUME_MAXT 16000 /* for A2DP resume, 16sec = 16000ms */
#define BTC_RPT_HDR_SIZE 3
#define BTC_BUSY2IDLE_THRES 10000 /* wait time for WL busy-to-idle, ms */
#define BTC_WL_DEF_TX_PWR 15
/* reserved 5 rf_trx_para level for non-freerun case */
#define BTC_LEVEL_ID_OFFSET 5
/* Define MAC-Related Reg Addr and Bitmap
 * These definitions should be removed if MACAPI support.
 */
#define BTC_DM_MAXSTEP 30

#define BTC_LEAK_AP_TH 10
#define BTC_CYSTA_CHK_PERIOD 100

#define BTC_SCC_CYCLE 100
#define BTC_NULL1_EBT_EARLY_T 10

#define R_BTC_CFG 0xDA00
#define B_BTC_DIS_BTC_CLK_G BIT(2)
#define B_BTC_WL_SRC BIT(0)

struct btc_t;

#define NM_EXEC false
#define FC_EXEC true

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define bMASKDW 0xffffffff
#define bMASKHW 0xffff0000
#define bMASKLW 0x0000ffff
#define bMASKB0 0x000000ff
#define bMASKB1 0x0000ff00
#define bMASKB2 0x00ff0000
#define bMASKB3 0xff000000
#define bMASKRF 0x000fffff

#define BTC_WL_RSSI_THMAX 4
#define BTC_BT_RSSI_THMAX 4

#define BTC_WL_RSSI_MAX_BTG 70 /* for BTG co-rx Hi-RSSI thres */

#define BTC_TDMA_WLROLE_MAX 3
#define BTC_TDMA_BTHID_MAX 2
#define BTC_FREERUN_ANTISO_MIN 30
#define BTC_FDDTRAIN_ANTISO_MIN 10
#define BTC_FDDTRAIN_ANTISO_MAX 40
#define BTC_BT_RX_NORMAL_LVL 7

#define BTC_BT_INFO_LEN 6
#define BTC_B1_MAX 250  /* unit:ms, used for Max A2DP retry adjust */

#define BTC_COEX_RTK_MODE 0
#define BTC_COEX_CSR_MODE 1
#define BTC_COEX_INNER 0
#define BTC_COEX_OUTPUT 1
#define BTC_COEX_INPUT 2

#define BTC_BLINK_NOCONNECT 0
#define BTC_WL_NONE 0
#define BTC_CHK_HANG_MAX 3
#define BTC_CHK_WLSLOT_DRIFT_MAX 15
#define BTC_CHK_BTSLOT_DRIFT_MAX 15

#define BTC_RFK_PATH_MAP 0xf
#define BTC_RFK_PHY_MAP 0x30
#define BTC_RFK_BAND_MAP 0xc0

#define BTC_GNT_SET_SKIP 0xff

/* mailbox 0x45 BT device info related */
#define BTC_BT_INFO_LINK_MAX 4
#define BTC_BT_INFO_HID_LIST 0x0
#define BTC_BT_INFO_HID_VENDOR_INFO 0x1
#define BTC_BT_INFO_HID_DEVICE_NAME 0x2
#define BTC_BT_INFO_HID_BASIC_INFO 0x3

#define BTC_BT_INFO_A2DP_LIST 0x10
#define BTC_BT_INFO_A2DP_BASIC_INFO 0x11

#define BTC_BT_INFO_PAN_LIST 0x20
#define BTC_BT_INFO_PAN_BASIC_INFO 0x21

#define _write_cx_reg(btc, offset, val) \
	rtw_hal_mac_coex_reg_write(btc->hal, offset, val)

#define _read_wl_rf_reg(btc, path, addr, mask) \
	rtw_hal_read_rf_reg(btc->hal, path, addr, mask);
#define _write_wl_rf_reg(btc, path, addr, mask, data) \
	rtw_hal_write_rf_reg(btc->hal, path, addr, mask, data)

#define _update_poicy hal_btc_fw_set_policy

#define run_rsn(r) \
	(hal_mem_cmp(btc->hal, btc->dm.run_reason, r, _os_strlen((u8*)r))? 0:1)

#define _rsn_cpy(dst, src, len) hal_mem_cpy(btc->hal, dst, src, len);

#define _act_cpy(dst, src, len) hal_mem_cpy(btc->hal, dst, src, len);

#define _tdma_cmp(dst, src) \
	hal_mem_cmp(btc->hal, dst, src, sizeof(struct fbtc_tdma))
#define _tdma_cpy(dst, src) \
	hal_mem_cpy(btc->hal, dst, src, sizeof(struct fbtc_tdma))
#define _tdma_set(tp,rxflc,txflc,wtg,lek,ext,flc_role) \
	do { \
		btc->dm.tdma.type = tp;\
		btc->dm.tdma.rxflctrl = rxflc; \
		btc->dm.tdma.txflctrl = txflc; \
		btc->dm.tdma.rsvd = wtg; \
		btc->dm.tdma.leak_n = lek; \
		btc->dm.tdma.ext_ctrl = ext; \
		btc->dm.tdma.rxflctrl_role = flc_role; \
	} while (0)
#define _tdma_set_rxflctrl(rxflc) btc->dm.tdma.rxflctrl = rxflc
#define _tdma_set_txflctrl(txflc) btc->dm.tdma.txflctrl = txflc
#define _tdma_set_flctrl_role(role) btc->dm.tdma.rxflctrl_role = role
#define _tdma_set_lek(lek) btc->dm.tdma.leak_n = lek

#define _tdma_set_instant() btc->dm.tdma.option_ctrl |= CXOPCTL_INST_EXEC
#define _tdma_set_fddt_en() btc->dm.tdma.option_ctrl |= CXOPCTL_FDDT_ENABLE
#define _tdma_set_fddt_renew() btc->dm.tdma.option_ctrl |= CXOPCTL_FDDT_RENEW
#define _tdma_set_fddt_dbg() btc->dm.tdma.option_ctrl |= CXOPCTL_FDDT_DEBUG

#define _slot_cmp(dst, src) \
	hal_mem_cmp(btc->hal, dst, src, sizeof(struct fbtc_slot))
#define _slot_cpy(dst, src) \
	hal_mem_cpy(btc->hal, dst, src, sizeof(struct fbtc_slot))
#define _slots_cpy(dst, src) \
	hal_mem_cpy(btc->hal, dst, src, CXST_MAX*sizeof(struct fbtc_slot))
#define _slot_set(sid,dura,tbl,type) \
	do { \
		btc->dm.slot[sid].dur = dura;\
		btc->dm.slot[sid].cxtbl = tbl; \
		btc->dm.slot[sid].cxtype = type; \
	} while (0)
#define _slot_set_dur(sid,dura) btc->dm.slot[sid].dur = dura
#define _slot_set_tbl(sid,tbl) btc->dm.slot[sid].cxtbl = tbl
#define _slot_set_type(sid,type) btc->dm.slot[sid].cxtype = type

#define _fddt_cmp(dst, src) \
	hal_mem_cmp(btc->hal, dst, src, sizeof(struct btc_fddt_train_info))
#define _fddt_cpy(dst, src) \
	hal_mem_cpy(btc->hal, dst, src, sizeof(struct btc_fddt_train_info))

#ifdef BTC_AISO_SUPPORT
#define BT_PSD_RPT_TYPE_QUERY	0x0 /* report when WLAN query */
#define BT_PSD_RPT_TYPE_CAL	0x1 /* when PSD scan idx~idx+4, idx+5~idx+9 */
#define BT_PSD_RPT_TYPE_NOISE	0x2 /* report all channel */

#define BT_PSD_SRC_RAW		0x0
#define BT_PSD_SRC_POST_BB	0x1
#define BT_PSD_SRC_POST_BB_FW	0x2

#define BT_PSD_WAIT_CNT		200

#define BT_PSD_TX_BUSY_CNT_MIN	3    /* data num per frequency point*/
#define BT_PSD_VALID_CH_MIN	4    /* 1~5, 3/4 is more suitbale */
#define BT_PSD_PRINT_PERIOD	10
#endif

enum btc_wl_gpio_debug {
	BTC_DBG_GNT_BT = 0,
	BTC_DBG_GNT_WL = 1,
	/* The following signals should 0-1 tiggle by each function-call */
	BTC_DBG_BCN_EARLY = 2,
	BTC_DBG_WL_NULL0 = 3,
	BTC_DBG_WL_NULL1 = 4,
	BTC_DBG_WL_RXISR = 5,
	BTC_DBG_TDMA_ENTRY = 6,
	BTC_DBG_A2DP_EMPTY = 7,
	BTC_DBG_BT_RETRY = 8,
	/* The following signals should 0-1 tiggle by state L/H */
	BTC_DBG_BT_RELINK = 9,
	BTC_DBG_SLOT_WL = 10,
	BTC_DBG_SLOT_BT = 11,
	BTC_DBG_WL_RFK = 12,
	BTC_DBG_BT_RFK = 13,
	/* The following signals should appear only 1 "Hi" at same time */
	BTC_DBG_SLOT_B2W = 14,
	BTC_DBG_SLOT_W1 = 15,
	BTC_DBG_SLOT_W2 = 16,
	BTC_DBG_SLOT_W2B = 17,
	BTC_DBG_SLOT_B1 = 18,
	BTC_DBG_SLOT_B2 = 19,
	BTC_DBG_SLOT_B3 = 20,
	BTC_DBG_SLOT_B4 = 21,
	BTC_DBG_SLOT_LK = 22,
	BTC_DBG_SLOT_E2G = 23,
	BTC_DBG_SLOT_E5G = 24,
	BTC_DBG_SLOT_EBT = 25,
	BTC_DBG_SLOT_ENULL = 26,
	BTC_DBG_SLOT_WLK = 27,
	BTC_DBG_SLOT_W1FDD = 28,
	BTC_DBG_SLOT_B1FDD = 29,
	BTC_DBG_BT_CHANGE = 30
};

enum btc_cx_run_info {
	BTC_CXR_WSCBD = 0,
	BTC_CXR_RESULT,
	BTC_CXR_MAX
};

enum btc_bt_rfk_state {
	BTC_BRFK_STOP = 0,
	BTC_BRFK_START = 1
};

enum btc_wl_rfk_state {
	BTC_WRFK_STOP = 0,
	BTC_WRFK_START = 1,
	BTC_WRFK_ONESHOT_START = 2,
	BTC_WRFK_ONESHOT_STOP = 3
};

enum btc_wl_rfk_result {
	BTC_WRFK_REJECT = 0,
	BTC_WRFK_ALLOW = 1
};

enum {
	BTC_LPS_OFF = 0,
	BTC_LPS_RF_OFF = 1,
	BTC_LPS_RF_ON = 2
};

enum {
	BTC_CTRL_BY_BT = 0,
	BTC_CTRL_BY_WL
};

enum {
	BTC_3CX_NONE = 0,
	BTC_3CX_LTE = 1,
	BTC_3CX_ZB = 2,
	BTC_3CX_MAX
};

enum {
	BTC_PRI_MASK_RX_RESP = 0,
	BTC_PRI_MASK_TX_RESP,
	BTC_PRI_MASK_BEACON,
	BTC_PRI_MASK_RX_CCK,
	BTC_PRI_MASK_TX_CCK,
	BTC_PRI_MASK_TX_TRIG,
	BTC_PRI_MASK_MAX
};

enum {
	BTC_INIT_NORMAL,
	BTC_INIT_WLONLY,
	BTC_INIT_WLOFF
};

enum {
	BTC_BRANCH_MAIN = 0,
	BTC_BRANCH_HP,
	BTC_BRANCH_LENOVO,
	BTC_BRANCH_HUAWEI,
	BTC_BRANCH_MAX
};

enum {
	BTC_STR_ROLE = 0,
	BTC_STR_MROLE,
	BTC_STR_REG,
	BTC_STR_SLOT,
	BTC_STR_TDMA,
	BTC_STR_TRACE,
	BTC_STR_BRANCH,
	BTC_STR_RXFLCTRL,
	BTC_STR_WLLINK,
	BTC_STR_ANTPATH,
	BTC_STR_GDBG,
	BTC_STR_CHIPID,
	BTC_STR_EVENT,
	BTC_STR_WLMODE,
	BTC_STR_WLBW,
	BTC_STR_RFTYPE,
	BTC_STR_POLICY,
	BTC_STR_MSTATE,
	BTC_STR_RATE,
	BTC_STR_POLUT,
	BTC_STR_BAND,
	BTC_STR_CXSTATE,
	BTC_STR_FDDT_TYPE,
	BTC_STR_FDDT_STATE,
	BTC_STR_MAX
};

enum {
	BTC_WL = 0,
	BTC_BT,
	BTC_ZB,
	BTC_LTE,
	BTC_MAX
};

enum {
	BTC_BT_SS_GROUP = 0x0,
	BTC_BT_TX_GROUP = 0x2,
	BTC_BT_RX_GROUP = 0x3,
	BTC_BT_MAX_GROUP,
};

enum btc_rssi_st {
	BTC_RSSI_ST_LOW = 0x0,
	BTC_RSSI_ST_HIGH,
	BTC_RSSI_ST_STAY_LOW,
	BTC_RSSI_ST_STAY_HIGH,
	BTC_RSSI_ST_MAX
};

enum {
	BTC_FDDT_DISABLE,
	BTC_FDDT_ENABLE,
};

enum btc_fddt_type {
	BTC_FDDT_TYPE_STOP,
	BTC_FDDT_TYPE_AUTO,
	BTC_FDDT_TYPE_FIX_TDD,
	BTC_FDDT_TYPE_FIX_FULL_FDD,
	BTC_FDDT_MAX
};

#define	BTC_RSSI_HIGH(_rssi_) \
	((_rssi_ == BTC_RSSI_ST_HIGH || _rssi_ == BTC_RSSI_ST_STAY_HIGH)? 1:0)
#define	BTC_RSSI_LOW(_rssi_) \
	((_rssi_ == BTC_RSSI_ST_LOW || _rssi_ == BTC_RSSI_ST_STAY_LOW)? 1:0)
#define	BTC_RSSI_CHANGE(_rssi_) \
	((_rssi_ == BTC_RSSI_ST_LOW || _rssi_ == BTC_RSSI_ST_HIGH)? 1:0)

enum btc_coex_info_map_en {
	BTC_COEX_INFO_CX = BIT(0),
	BTC_COEX_INFO_WL = BIT(1),
	BTC_COEX_INFO_BT = BIT(2),
	BTC_COEX_INFO_DM = BIT(3),
	BTC_COEX_INFO_MREG = BIT(4),
	BTC_COEX_INFO_SUMMARY = BIT(5),
	BTC_COEX_INFO_ALL = 0xff
};

/* list all-chip scoreboard definition, remap at chip file */
enum btc_w2b_scoreboard {
	BTC_WSCB_ACTIVE	= BIT(0),
	BTC_WSCB_ON = BIT(1),
	BTC_WSCB_SCAN = BIT(2),
	BTC_WSCB_UNDERTEST = BIT(3),
	BTC_WSCB_RXGAIN = BIT(4), /* set BT LNA constrain for free-run */
	BTC_WSCB_WLBUSY = BIT(7),
	BTC_WSCB_EXTFEM = BIT(8),
	BTC_WSCB_TDMA = BIT(9),
	BTC_WSCB_FIX2M = BIT(10),
	BTC_WSCB_WLRFK = BIT(11),
	BTC_WSCB_RXSCAN_PRI = BIT(12), /* set BT Rx-Scan PTA pri */
	BTC_WSCB_BT_HILNA = BIT(13), /* request BT use Hi-LNA table for BTG */
	BTC_WSCB_BTLOG = BIT(14),  /* open BT log */
	BTC_WSCB_ALL = 0xffffff /* driver only use bit0~23 */
};

enum btc_b2w_scoreboard {
	BTC_BSCB_ACT = BIT(0),
	BTC_BSCB_ON = BIT(1),
	BTC_BSCB_WHQL = BIT(2),
	BTC_BSCB_BT_S1 = BIT(3),  /* 1-> BT at S1, 0-> BT at S0 */
	BTC_BSCB_A2DP_ACT = BIT(4),
	BTC_BSCB_RFK_RUN = BIT(5),
	BTC_BSCB_RFK_REQ = BIT(6),
	BTC_BSCB_LPS = BIT(7),
	BTC_BSCB_BT_LNAB0 = BIT(8),
	BTC_BSCB_BT_LNAB1 = BIT(10),
	BTC_BSCB_WLRFK = BIT(11),
	BTC_BSCB_BT_HILNA = BIT(13), /* reply if BT use Hi-LNA table for BTG */
	BTC_BSCB_BT_CONNECT = BIT(16), /* If any BT connected */
	BTC_BSCB_PATCH_CODE = BIT(30),  /* BT use 1: patch code 2:ROM code */
	BTC_BSCB_ALL = 0x7fffffff
};

enum btc_bt_link_status {
	BTC_BLINK_CONNECT = BIT(0),
	BTC_BLINK_BLE_CONNECT = BIT(1),
	BTC_BLINK_ACL_BUSY = BIT(2),
	BTC_BLINK_SCO_BUSY = BIT(3),
	BTC_BLINK_MESH_BUSY = BIT(4),
	BTC_BLINK_INQ_PAGE = BIT(5)
};

enum {
	BTC_DCNT_RUN = 0x0,
	BTC_DCNT_CX_RUNINFO,
	BTC_DCNT_RPT,
	BTC_DCNT_RPT_HANG,
	BTC_DCNT_CYCLE,
	BTC_DCNT_CYCLE_HANG,
	BTC_DCNT_W1,
	BTC_DCNT_W1_HANG,
	BTC_DCNT_B1,
	BTC_DCNT_B1_HANG,
	BTC_DCNT_TDMA_NONSYNC,
	BTC_DCNT_SLOT_NONSYNC,
	BTC_DCNT_BTCNT_HANG,
	BTC_DCNT_WL_SLOT_DRIFT,
	BTC_DCNT_WL_STA_LAST,
	BTC_DCNT_BT_SLOT_DRIFT,
	BTC_DCNT_FDDT_TRIG,
	BTC_DCNT_MAX
};

enum {
	BTC_NCNT_POWER_ON = 0x0,
	BTC_NCNT_POWER_OFF,
	BTC_NCNT_INIT_COEX,
	BTC_NCNT_SCAN_START,
	BTC_NCNT_SCAN_FINISH,
	BTC_NCNT_SPECIAL_PACKET,
	BTC_NCNT_SWITCH_BAND,
	BTC_NCNT_RFK_TIMEOUT,
	BTC_NCNT_SHOW_COEX_INFO,
	BTC_NCNT_ROLE_INFO,
	BTC_NCNT_CONTROL,
	BTC_NCNT_RADIO_STATE,
	BTC_NCNT_CUSTOMERIZE,
	BTC_NCNT_WL_RFK,
	BTC_NCNT_WL_STA,
	BTC_NCNT_FWINFO,
	BTC_NCNT_TIMER,
	BTC_NCNT_MAX
};

enum btc_wl_state_cnt {
	BTC_WCNT_SCANAP = 0x0,
	BTC_WCNT_DHCP,
	BTC_WCNT_EAPOL,
	BTC_WCNT_ARP,
	BTC_WCNT_SCBDUPDATE,
	BTC_WCNT_RFK_REQ,
	BTC_WCNT_RFK_GO,
	BTC_WCNT_RFK_REJECT,
	BTC_WCNT_RFK_TIMEOUT,
	BTC_WCNT_CH_UPDATE,
	BTC_WCNT_DBCC_ALL_2G,
	BTC_WCNT_DBCC_CHG,
	BTC_WCNT_MAX
};

enum btc_bt_state_cnt {
	BTC_BCNT_RETRY = 0x0,
	BTC_BCNT_REINIT,
	BTC_BCNT_REENABLE,
	BTC_BCNT_SCBDREAD,
	BTC_BCNT_RELINK,
	BTC_BCNT_IGNOWL,
	BTC_BCNT_INQPAG,
	BTC_BCNT_INQ,
	BTC_BCNT_PAGE,
	BTC_BCNT_ROLESW,
	BTC_BCNT_AFH,
	BTC_BCNT_INFOUPDATE,
	BTC_BCNT_INFOSAME,
	BTC_BCNT_SCBDUPDATE,
	BTC_BCNT_HIPRI_TX,
	BTC_BCNT_HIPRI_RX,
	BTC_BCNT_LOPRI_TX,
	BTC_BCNT_LOPRI_RX,
	BTC_BCNT_POLUT,
	BTC_BCNT_RATECHG,
	BTC_BCNT_MAX
};

enum btc_wl_link_mode {
	BTC_WLINK_NOLINK = 0x0,
	BTC_WLINK_2G_STA,
	BTC_WLINK_2G_AP,
	BTC_WLINK_2G_GO,
	BTC_WLINK_2G_GC,
	BTC_WLINK_2G_SCC,
	BTC_WLINK_2G_MCC,
	BTC_WLINK_25G_MCC,
	BTC_WLINK_25G_DBCC,
	BTC_WLINK_5G,
	BTC_WLINK_2G_NAN,
	BTC_WLINK_OTHER,
	BTC_WLINK_MAX
};

enum btc_wl_mrole_type {
	BTC_WLMROLE_NONE = 0x0,
	BTC_WLMROLE_STA_GC,
	BTC_WLMROLE_STA_GC_NOA,
	BTC_WLMROLE_STA_GO,
	BTC_WLMROLE_STA_GO_NOA,
	BTC_WLMROLE_STA_STA,
	BTC_WLMROLE_MAX
};

enum {
	BTC_BTINFO_L0 = 0,
	BTC_BTINFO_L1,
	BTC_BTINFO_L2,
	BTC_BTINFO_L3,
	BTC_BTINFO_H0,
	BTC_BTINFO_H1,
	BTC_BTINFO_H2,
	BTC_BTINFO_H3,
	BTC_BTINFO_MAX
};

struct btc_btinfo_lb2 {
	u8 connect: 1;
	u8 sco_busy: 1;
	u8 inq_pag: 1;
	u8 acl_busy: 1;
	u8 hfp: 1;
	u8 hid: 1;
	u8 a2dp: 1;
	u8 pan: 1;
};

struct btc_btinfo_lb3 {
	u8 retry: 4;
	u8 cqddr: 1;
	u8 inq: 1;
	u8 mesh_busy: 1;
	u8 pag: 1;
};

struct btc_btinfo_hb0 {
	u8 rssi;
};

struct btc_btinfo_hb1 {
	u8 ble_connect: 1;
	u8 reinit: 1;
	u8 relink: 1;
	u8 igno_wl: 1;
	u8 voice: 1;
	u8 ble_scan: 1;
	u8 role_sw: 1;
	u8 multi_link: 1;
};

struct btc_btinfo_hb2 {
	u8 pan_active: 1;
	u8 afh_update: 1;
	u8 a2dp_active: 1;
	u8 slave: 1;
	u8 hid_slot: 2;
	u8 hid_cnt: 2;
};

struct btc_btinfo_hb3 {
	u8 a2dp_bitpool: 6;
	u8 tx_3M: 1;
	u8 a2dp_sink: 1;
};

union btc_btinfo {
	u8 val;
	struct btc_btinfo_lb2 lb2;
	struct btc_btinfo_lb3 lb3;
	struct btc_btinfo_hb0 hb0;
	struct btc_btinfo_hb1 hb1;
	struct btc_btinfo_hb2 hb2;
	struct btc_btinfo_hb3 hb3;
};

enum btc_rinfo_lo_b2 {
	BTC_RINFO_INQPAG = BIT(2),
};

enum btc_bt_hfp_type {
	BTC_HFP_SCO = 0,
	BTC_HFP_ESCO = 1,
	BTC_HFP_ESCO_2SLOT = 2,
};

enum btc_bt_hid_type {
	BTC_HID_218 = BIT(0),
	BTC_HID_418 = BIT(1),
	BTC_HID_BLE = BIT(2),
	BTC_HID_RCU = BIT(3),
	BTC_HID_RCU_VOICE = BIT(4),
	BTC_HID_OTHER_LEGACY = BIT(5)
};

enum btc_bt_a2dp_type {
	BTC_A2DP_LEGACY = 0,
	BTC_A2DP_TWS_SNIFF = 1, /* Airpod */
	BTC_A2DP_TWS_RELAY = 2, /* RTL8763B */
};

enum btc_bt_PAN_type {
	BTC_BT_PAN_ONLY = 0,
	BTC_BT_OPP_ONLY = 1
};

enum btc_bt_scan_type {
	BTC_SCAN_INQ	= 0,
	BTC_SCAN_PAGE,
	BTC_SCAN_BLE,
	BTC_SCAN_INIT,
	BTC_SCAN_TV,
	BTC_SCAN_ADV,
	BTC_SCAN_MAX1
};

enum btc_bt_mailbox_id {
	BTC_BTINFO_REPLY = 0x23,
	BTC_BTINFO_AUTO = 0x27
};

enum {
	BTC_BT_HFP = BIT(0),
	BTC_BT_HID = BIT(1),
	BTC_BT_A2DP = BIT(2),
	BTC_BT_PAN = BIT(3),
	BTC_PROFILE_MAX = 4
};

enum {
	BTC_BT_IQK_OK = 0,
	BTC_BT_IQK_START = 1,
	BTC_BT_IQK_STOP = 2,
	BTC_BT_IQK_MAX
};

enum {
	BTC_ANT_SHARED = 0,
	BTC_ANT_DEDICATED,
	BTC_ANTTYPE_MAX
};

enum {
	BTC_BT_ALONE = 0,
	BTC_BT_BTG
};

enum {
	BTC_SWITCH_INTERNAL = 0,
	BTC_SWITCH_EXTERNAL
};

enum {
	BTC_RESET_CX = BIT(0),
	BTC_RESET_DM = BIT(1),
	BTC_RESET_CTRL = BIT(2),
	BTC_RESET_CXDM = BIT(0) | BIT(1),
	BTC_RESET_BTINFO = BIT(3),
	BTC_RESET_MDINFO = BIT(4),
	BTC_RESET_BT_PSD_DM = BIT(5),
	BTC_RESET_ALL = 0xff
};

enum btc_gnt_state {
	BTC_GNT_HW	= 0,
	BTC_GNT_SW_LO,
	BTC_GNT_SW_HI,
	BTC_GNT_MAX
};

enum btc_wl_max_tx_time {
	BTC_MAX_TX_TIME_L1 = 500,
	BTC_MAX_TX_TIME_L2 = 1000,
	BTC_MAX_TX_TIME_L3 = 2000,
	BTC_MAX_TX_TIME_DEF = 5280
};

enum btc_wl_max_tx_retry {
	BTC_MAX_TX_RETRY_L1 = 7,
	BTC_MAX_TX_RETRY_L2 = 15,
	BTC_MAX_TX_RETRY_DEF = 31
};

enum btc_nofddt_rsn {
	BTC_NFRSN_SUPPORT,
	BTC_NFRSN_FORCE_STOP,
	BTC_NFRSN_DEDICATED_ANT,
	BTC_NFRSN_ANT_ISO_LOW,
	BTC_NFRSN_ANT_ISO_HI,
	BTC_NFRSN_WL_2GSTA,     /* if 2G STA only */
	BTC_NFRSN_WL_BUSY,      /* if idle/LPS */
	BTC_NFRSN_WL_NOSCAN,
	BTC_NFRSN_WL_NORFK,
	BTC_NFRSN_WB_RSSI,      /* if WL & BT RSSI to low */
	BTC_NFRSN_BT_PROFILE,   /* if NOT A2DP or A2DP + HID/HFP */
	BTC_NFRSN_BT_A2DP_BUSY, /* if A2DP idle */
	BTC_NFRSN_BT_NOINQ,
	BTC_NFRSN_COND_NUM,
	BTC_NFRSN_NHM,
	BTC_NFRSN_RETRY_PERIOD,/* if bt retry-period not reach */
	BTC_NFRSN_MAX
};

enum btc_fddt_state {
	BTC_FDDT_STATE_STOP,
	BTC_FDDT_STATE_RUN,
	BTC_FDDT_STATE_PENDING,
	BTC_FDDT_STATE_DEBUG,
	BTC_FDDT_STATE_MAX
};

enum btc_fddt_result {
	BTC_FDDT_RESULT_UNFINISH,
	BTC_FDDT_RESULT_OK,
	BTC_FDDT_RESULT_FAIL,
	BTC_FDDT_RESULT_MAX
};

struct btc_wl_tx_limit_para {
	u16 en;
	u32 tx_time;	/* unit: us */
	u16 tx_retry;
};

struct btc_rf_trx_para {
	u32 wl_tx_power; /* absolute Tx power (dBm), 1's complement -5->0x85 */
	u32 wl_rx_gain;  /* rx gain table index (TBD.) */
	u32 bt_tx_power; /* decrease Tx power (dB) */
	u32 bt_rx_gain;  /* LNA constrain level */
};

struct btc_gnt_ctrl {
	u8 gnt_bt_sw_en;
	u8 gnt_bt;
	u8 gnt_wl_sw_en;
	u8 gnt_wl;
};

struct btc_fw_dm {
	u32 freerun: 1;
	u32 wl_ps_ctrl: 2; /* 0: off, 1: force on, 2:forec off */
	u32 wl_mimo_ps: 1;
	u32 leak_ap: 1;
	u32 igno_bt: 1;
	u32 noisy_level: 3;
	u32 set_ant_path: 16;
	u32 rsvd: 7;
};

struct btc_cxr_result {
    struct btc_fw_dm dm;
    struct btc_rf_trx_para rf_trx_para;
    u32 cx_state_map;
    u32 policy_type;
    u32 run_cnt;

    char run_reason[BTC_RSN_MAXLEN];
    char run_action[BTC_ACT_MAXLEN];
};

struct btc_bt_link_hid {
	u8 handle_id;
	u8 vendor_id;
	u8 interval;
	u8 window;
	bool type;
	bool role;
	bool sniff;
};

struct btc_bt_link_a2dp {
	u8 handle_id;
	u8 vendor_id;
	bool type;
	bool role;
	bool sniff;
};

struct btc_bt_link_pan {
	u8 handle_id;
	u8 vendor_id;
	bool type;
	bool role;
	bool sniff;
	bool direct;
};

struct btc_bt_link_hfp {
	u8 handle_id;
	u8 vendor_id;
	bool type;
	bool role;
	bool sniff;
};

struct btc_bt_link_noprofile {
	u8 handle_id;
	u8 vendor_id;
	bool type;
	bool role;
	bool sniff;
};

struct btc_bt_hfp_desc {
	u32 exist: 1;
	u32 type: 2; /* refer to btc_bt_hfp_type */
	u32 rsvd: 29;

	u8 hfp_cnt;
	u8 hfp_list[BTC_BT_INFO_LINK_MAX];
	struct btc_bt_link_hfp hfps[BTC_BT_INFO_LINK_MAX];
};

struct btc_bt_hid_desc {
	u32 exist: 1;
	u32 slot_info: 2;
	u32 pair_cnt: 2;
	u32 type: 8; /* refer to btc_bt_hid_type */
	u32 rsvd: 19;

	u8 hid_cnt;
	u8 hid_list[BTC_BT_INFO_LINK_MAX];
	struct btc_bt_link_hid hids[BTC_BT_INFO_LINK_MAX];
};

struct btc_bt_a2dp_desc {
	u8 exist: 1;
	u8 exist_last: 1;
	u8 play_latency: 1;
	u8 type: 3; /* refer to btc_bt_a2dp_type */
	u8 active: 1;
	u8 sink: 1;

	u8 bitpool;
	u16 vendor_id;
	u32 device_name;
	u32 flush_time;

	u8 a2dp_cnt;
	u8 a2dp_list[BTC_BT_INFO_LINK_MAX];
	struct btc_bt_link_a2dp a2dps[BTC_BT_INFO_LINK_MAX];
};

struct btc_bt_pan_desc {
	u32 exist: 1;
	u32 type: 1; /* refer to btc_bt_pan_type */
	u32 active: 1;
	u32 rsvd: 29;

	u8 pan_cnt;
	u8 pan_list[BTC_BT_INFO_LINK_MAX];
	struct btc_bt_link_pan pans[BTC_BT_INFO_LINK_MAX];
};

struct btc_bt_noprofile_desc {
	u8 no_profile_cnt;
	u8 no_profile[BTC_BT_INFO_LINK_MAX];
	struct btc_bt_link_noprofile noprofiles[BTC_BT_INFO_LINK_MAX];
};

struct btc_wl_nhm {
	s8 instant_wl_nhm_dbm;
	s8 instant_wl_nhm_per_mhz;
	u16 valid_record_times;
	s8 record_pwr[16];
	u8 record_ratio[16];
	s8 pwr; /* dbm_per_MHz  */
	u8 ratio;
	u8 current_status;
	u8 refresh;
	bool start_flag;
	u8 last_ccx_rpt_stamp;
};

struct btc_chip_ops {
	void (*set_rfe)(struct btc_t *btc);
	void (*init_cfg)(struct btc_t *btc);
	void (*wl_pri)(struct btc_t *btc, u8 mask, bool state);
	void (*wl_tx_power)(struct btc_t *btc, u32 level);
	void (*wl_rx_gain)(struct btc_t *btc, u32 level);
	void (*wl_s1_standby)(struct btc_t *btc, u32 state);
	void (*wl_req_mac)(struct btc_t *btc, u8 mac_id);
	void (*update_bt_cnt)(struct btc_t *btc);
	u8 (*bt_rssi)(struct btc_t *btc, u8 val);
};

struct btc_chip {
	u8 chip_id;
	u32 para_ver;  /* chip parameter version */
	u8 btcx_desired; /* bt fw desired coex version */
	u32 wlcx_desired; /* wl fw desired coex version */
	u8 scbd; /* scoreboard version, 0: not support*/
	u8 mailbox; /* mailbox version, 0: not support */
	u8 pta_mode;
	u8 pta_direction;
	u8 afh_guard_ch;
	const u8 *wl_rssi_thres; /* wl rssi thre level */
	const u8 *bt_rssi_thres; /* bt rssi thre level */
	u8 rssi_tol; /* rssi tolerance */
	struct btc_chip_ops *ops;
	u8 mon_reg_num;
	struct fbtc_mreg *mon_reg;
	u8 rf_para_ulink_num;
	struct btc_rf_trx_para *rf_para_ulink;
	u8 rf_para_dlink_num;
	struct btc_rf_trx_para *rf_para_dlink;

};

struct btc_bt_scan_info {
	u16 win;
	u16 intvl;
	u32 enable: 1;
	u32 interlace: 1;
	u32 rsvd: 30;
};

struct btc_bt_rfk_info {
	u32 run: 1;
	u32 req: 1;
	u32 timeout: 1;
	u32 rsvd: 29;
};

union btc_bt_rfk_info_map {
	u32 val;
	struct btc_bt_rfk_info map;
};

struct btc_bt_ver_info {
	u32 fw_coex; /* match with which coex_ver */
	u32 fw;
};
struct btc_bool_sta_chg { /* status change varible */
	u32 now: 1;
	u32 last: 1;
	u32 remain: 1;
	u32 srvd: 29;
};

struct btc_u8_sta_chg { /* status change varible */
	u8 now;
	u8 last;
	u8 remain;
	u8 rsvd;
};

struct btc_bit_remap {
	u32 index;
	u32 bpos;
};

struct btc_ant_info {
	u8 type;  /* shared, dedicated */
	u8 num;
	u8 isolation;

	u8 single_pos: 1;/* Single antenna at S0 or S1 */
	u8 diversity: 1;
};

struct btc_traffic {
	enum rtw_tfc_lvl tx_lvl;
	enum rtw_tfc_sts tx_sts;
	enum rtw_tfc_lvl rx_lvl;
	enum rtw_tfc_sts rx_sts;
	u16 tx_rate;
	u16 rx_rate;
	u8 tx_1ss_limit;

	u64 tx_byte;
	u64 rx_byte;
	u32 tx_time;
	u32 rx_time;

	u32 tx_tp;
	u32 rx_tp;
};

struct btc_wl_scan_info {
	u8 band[HW_PHY_MAX];
	u8 phy_map;
	u8 rsvd;
};

struct btc_wl_dbcc_info {
	u8 op_band[HW_PHY_MAX]; /* op band in each phy */
	u8 scan_band[HW_PHY_MAX]; /* scan band in  each phy */
	u8 real_band[HW_PHY_MAX];
	u16 role[HW_PHY_MAX]; /* role in each phy */
};

struct btc_wl_active_role { /* struct size must be n*4 bytes */
	u8 connected: 1;
	u8 pid: 3;
	u8 phy: 1;
	u8 noa: 1;
	u8 band: 2; /* enum band_type RF band: 2.4G/5G/6G */

	u8 client_ps: 1;
	u8 bw: 7; /* enum channel_width */

	u8 role; /*enum role_type */
	u8 ch;

	u16 tx_lvl;
	u16 rx_lvl;
	u16 tx_rate;
	u16 rx_rate;

	u32 noa_duration; /* ms */
};

struct btc_wl_role_info_bpos {
	u16 none: 1;
	u16 station: 1;
	u16 ap: 1;
	u16 vap: 1;
	u16 adhoc: 1;
	u16 adhoc_master: 1;
	u16 mesh: 1;
	u16 moniter: 1;
	u16 p2p_device: 1;
	u16 p2p_gc: 1;
	u16 p2p_go: 1;
	u16 nan: 1;
};

union btc_wl_role_info_map {
	u16 val;
	struct btc_wl_role_info_bpos role;
};

struct btc_wl_scc_ctrl {
	u8 null_role1;
	u8 null_role2;
	u8 ebt_null; /* if tx null at EBT slot */
	u8 rsvd;
};

struct btc_wl_role_info { /* struct size must be n*4 bytes */
	u8 connect_cnt;
	u8 link_mode;
	union btc_wl_role_info_map role_map;
	struct btc_wl_active_role active_role[BTC_WL_MAX_ROLE_NUMBER];
	u32 mrole_type; /* btc_wl_mrole_type */
	u32 mrole_noa_duration; /* ms */

	u32 dbcc_en: 1;
	u32 dbcc_chg: 1;
	u32 dbcc_2g_phy: 2; /* which phy operate in 2G, HW_PHY_0 or HW_PHY_1 */
	u32 link_mode_chg: 1;
	u32 p2p_2g: 1;
	u32 rsvd: 26;
};

struct btc_statistic {
	u8 rssi; /* 0%~110% (dBm = rssi -110) */
	struct btc_traffic traffic;
};

struct btc_wl_stat_info {
	u8 pid;
	struct btc_statistic stat;
};

struct btc_wl_link_info {
	struct btc_statistic stat;
	enum rtw_traffic_dir dir;
	struct rtw_chan_def chdef;

	u8 mode; /* wl mode: 11b, 11g, 11n... */
	u8 ch;
	u8 bw;
	u8 band; /* enum  RF band: 2.4G/5G/6G */
	u8 role; /*enum role_type */
	u8 pid; /* MAC HW Port id: 0~4 */
	u8 phy; /* PHY-0 or PHY-1*/
	u8 dtim_period;

	u8 busy;
	u8 rssi_state[BTC_WL_RSSI_THMAX];
	u8 mac_addr[6];
	u8 tx_retry;

	u16 mac_id; /* 0~63 */

	u32 bcn_period;
	u32 noa_duration; /* us */
	u32 busy_t; /* busy start time */
	u32 tx_time; /* the original max_tx_time */
	u32 client_cnt; /* connected-client cnt for p2p-GO/soft-AP mode */
	u32 rx_rate_drop_cnt;

	u32 active: 1;
	u32 noa: 1; /* Todo: for P2P */
	u32 client_ps: 1; /* Todo: for soft-AP */
	u32 connected: 2;
};

struct btc_wl_rfk_info {
	u32 state: 2;
	u32 path_map: 4;
	u32 phy_map: 2;
	u32 band: 2; /*0:2g, 1:5g, 2:6g  */
	u32 type: 8;
	u32 rsvd: 14;

	u32 start_time;
	u32 proc_time;
};

struct btc_wl_ver_info {
	u32 fw_coex; /* match with which coex_ver */
	u32 fw;
	u32 mac;
	u32 bb;
	u32 rf;
};

struct btc_wl_afh_info {
	u8 en;
	u8 ch;
	u8 bw;
	u8 rsvd;
};

struct btc_ops {
	void (*fw_cmd)(struct btc_t *btc, u8 h2c_class, u8 h2c_func,
		      u8 *param, u16 len);
	void (*ntfy_power_on)(struct btc_t *btc);
	void (*ntfy_power_off)(struct btc_t *btc);
	void (*ntfy_init_coex)(struct btc_t *btc, u8 mode);
	void (*ntfy_scan_start)(struct btc_t *btc, u8 band_idx, u8 band);
	void (*ntfy_scan_finish)(struct btc_t *btc, u8 band_idx);
	void (*ntfy_switch_band)(struct btc_t *btc, u8 band_idx, u8 band);
	void (*ntfy_specific_packet)(struct btc_t *btc, u8 pkt_type);
	void (*ntfy_role_info)(struct btc_t *btc, u8 rid,
			      struct btc_wl_link_info *info,
			      enum link_state reason);
	void (*ntfy_radio_state)(struct btc_t *btc, u8 rf_state);
	void (*ntfy_customerize)(struct btc_t *btc, u8 type, u16 len, u8 *buf);
	u8  (*ntfy_wl_rfk)(struct btc_t *btc, u8 phy, u8 type, u8 state);
	void (*ntfy_wl_sta)(struct btc_t *btc, struct rtw_stats *phl_stats,
			   u8 ntfy_num, struct rtw_phl_stainfo_t *sta[],
			   u8 reason);
	void (*ntfy_fwinfo)(struct btc_t *btc, u8 *buf, u32 len, u8 cls,
			   u8 func);
	void (*ntfy_timer)(struct btc_t *btc, u16 tmr_id);
};

struct btc_bt_smap {
	u32 connect: 1;
	u32 ble_connect: 1;
	u32 acl_busy: 1;
	u32 sco_busy: 1;
	u32 mesh_busy: 1;
	u32 inq_pag: 1;
};

union btc_bt_state_map {
	u32 val;
	struct btc_bt_smap map;
};

struct btc_bt_link_info {
	struct btc_u8_sta_chg profile_cnt;
	struct btc_bool_sta_chg multi_link;
	struct btc_bool_sta_chg relink;
	struct btc_bt_hfp_desc hfp_desc;
	struct btc_bt_hid_desc hid_desc;
	struct btc_bt_a2dp_desc a2dp_desc;
	struct btc_bt_pan_desc pan_desc;
	struct btc_bt_noprofile_desc noprofile_desc;
	union btc_bt_state_map status;

	u8 sut_pwr_level[BTC_PROFILE_MAX];
	u8 golden_rx_shift[BTC_PROFILE_MAX];
	u8 rssi_state[BTC_BT_RSSI_THMAX];
	u8 afh_map[12];
	u8 afh_map_le[5];

	u32 role_sw: 1;
	u32 slave_role: 1;
	u32 afh_update: 1;
	u32 cqddr: 1;
	u32 rssi: 8;
	u32 tx_3M: 1;
	u32 rsvd: 19;
};

struct btc_3rdcx_info {
	u8 type;   /* 0: none, 1:zigbee, 2:LTE  */
	u8 hw_coex;
	u16 rsvd;
};

struct btc_rf_para {
	u32 tx_pwr_freerun;
	u32 rx_gain_freerun;
	u32 tx_pwr_perpkt;
	u32 rx_gain_perpkt;
};

struct btc_bt_info {
	struct btc_bt_link_info link_info;
	struct btc_bt_scan_info scan_info[BTC_SCAN_MAX1];
	struct btc_bt_ver_info ver_info;
	struct btc_bool_sta_chg enable;
	struct btc_bool_sta_chg inq_pag;
	struct btc_rf_para rf_para; /* for record */
	union btc_bt_rfk_info_map rfk_info;

	u8 raw_info[BTC_BTINFO_MAX]; /* raw bt info from mailbox */
	u8 rssi_level;

	u32 scbd; /* scoreboard value */
	u32 feature;

	u32 mbx_avl: 1; /* mailbox available */
	u32 whql_test: 1;
	u32 igno_wl: 1;
	u32 reinit: 1;
	u32 ble_scan_en: 1;
	u32 btg_type: 1; /* BT located at BT only or BTG */
	u32 inq: 1;
	u32 pag: 1;

	u32 run_patch_code: 1;
	u32 hi_lna_rx: 1;
	u32 scan_rx_low_pri: 1;
	u32 lna_constrain: 3;

	u32 rsvd: 18;
};

struct btc_bt_mb_devinfo {
	u8 cmd;
	u8 len;
	u8 sub_id;
	u8 data[5];
};

struct btc_wl_smap {
	u32 busy: 1;
	u32 scan: 1;
	u32 connecting: 1;
	u32 roaming: 1;
	u32 dbccing: 1;
	u32 _4way: 1;
	u32 rf_off: 1;
	u32 lps: 2;  /* 1: LPS-Protocol + WL_power, 2:LPS_Protocol   */
	u32 ips: 1;
	u32 init_ok: 1;
	u32 traffic_dir : 2;
	u32 rf_off_pre: 1;
	u32 lps_pre: 2;
};

union btc_wl_state_map {
	u32 val;
	struct btc_wl_smap map;
};

struct btc_dm_emap {
	u32 init: 1;
	u32 pta_owner: 1;
	u32 wl_rfk_timeout: 1;
	u32 bt_rfk_timeout: 1;

	u32 wl_fw_hang: 1;
	u32 offload_mismatch: 1;
	u32 cycle_hang: 1;
	u32 w1_hang: 1;

	u32 b1_hang: 1;
	u32 tdma_no_sync: 1;
	u32 wl_slot_drift: 1;
	u32 bt_slot_drift: 1; /* for external slot control */

	u32 role_num_mismatch: 1;
};

union btc_dm_error_map {
	u32 val;
	struct btc_dm_emap map;
};

struct btc_wl_info {
	struct btc_wl_link_info link_info[BTC_WL_MAX_ROLE_NUMBER];
	struct btc_wl_rfk_info  rfk_info;
	struct btc_wl_ver_info  ver_info;
	struct btc_wl_afh_info afh_info;
	struct btc_wl_role_info role_info;
	struct btc_wl_scan_info scan_info;
	struct btc_wl_dbcc_info dbcc_info;
	struct btc_bool_sta_chg cck_lock;
	struct btc_rf_para rf_para; /* for record */
	union btc_wl_state_map status;
	struct btc_wl_nhm nhm;

	u8 iot_peer;
	u8 rssi_level;/* the overall-role rssi-level 0~4, "low_rssi->hi_level"*/
	u8 bssid[6];
	u8 pta_req_mac;
	u8 bt_polut_type[2]; /* BT polluted WL-Tx type for phy0/1  */
	u8 cn_report;

	u8 scbd_change: 1;
	u8 pta_reg_mac_chg: 1;

	u32 scbd;
};

struct btc_module {
	struct btc_ant_info ant;
	u8 rfe_type;
	u8 kt_ver;

	u8 bt_solo: 1;
	u8 bt_pos: 1; /* wl-end view: get from efuse, must compare bt.btg_type*/
	u8 switch_type: 1; /* WL/BT switch type: 0: internal, 1: external */

	u8 kt_ver_adie;
};

struct btc_init_info {
	struct btc_module module;
	u8 wl_guard_ch;

	u8 wl_only: 1;
	u8 wl_init_ok: 1;
	u8 dbcc_en: 1;
	u8 cx_other: 1;
	u8 bt_only: 1;

	u16 rsvd;
};

/* record the last 20 reason/action */
struct btc_dm_step {
	char step[BTC_DM_MAXSTEP][BTC_RSN_MAXLEN];
	u32 cnt;
};

struct btc_fddt_bt_stat {
	struct btc_rpt_ctrl_a2dp_empty a2dp_last;
	u32 retry_last;
};

struct btc_fddt_cell {
	s8 wl_pwr_min;
	s8 wl_pwr_max;
	s8 bt_pwr_dec_max;
	s8 bt_rx_gain;
};

struct btc_fddt_fail_check { /* for cell stay in training */
	u8 check_map;         /* check pass condition if bit-map = 1 */

	u8 bt_no_empty_cnt;   /* 0-fail if no bt-empty >= th in train_cycle */
	u8 wl_tp_ratio;       /* 1-fail if wl tp rise ratio < th */
	u8 wl_kpibtr_ratio;   /* 2-fail if phase_now_tp < phase_last_tp * kpibtr_ratio*/
};

struct btc_fddt_break_check { /* for cell stay in training or train-ok */
	u8 check_map;            /* check break condition if bit-map = 1 */

	u8 bt_no_empty_cnt;  /* 0-break if no empty count >= th  */
	u8 wl_tp_ratio;      /* 1-break if wl tp ratio < th (%)*/
	u8 wl_tp_low_bound;  /* 2-break if wl tp (in Mbps) < th */
	u8 cn;               /* 3-break if (cn >= cn_limit) >= th cycle */
	u8 cell_chg;         /* 4-break if non-matched-RSSI >= th cycle */
	s8 nhm_limit;        /* 5-break if nhm >= th --> ill-condition*/

	u8 cn_limit;         /* if condition number >= th --> ill-condition  */
};

struct btc_fddt_time_ctrl {
	/* 1 TDD cycle = w1 + b1, FDD 1cycle = w1fdd-slot + b1fdd-slot */
	u8 m_cycle; /* KPI Moving-Average-Cycle: 1~32 cycles */
	u8 w_cycle; /* Start to calcul WKPI after this if train-phase change */
	u8 k_cycle; /* Total kpi-estimate cycles for each training-step */

	u8 rsvd;
};

struct btc_fddt_train_info {
	struct btc_fddt_time_ctrl t_ctrl;
	struct btc_fddt_break_check b_chk;
	struct btc_fddt_fail_check f_chk;
	struct btc_fddt_cell cell_ul[5][5];
	struct btc_fddt_cell cell_dl[5][5];
};

struct btc_fddt_info {
	u8 type;         /* refer to enum btc_fddt_type */
	u8 result;       /* fw send fdd-training status by c2h  */
	u8 state;        /* refer to enum btc_fddt_state */

	u8 wl_iot[6];    /* wl bssid  */
	u16 bt_iot;      /* bt vendor-id */

	u32 nrsn_map;    /* the reason map for no-run fdd-traing */
	struct btc_fddt_bt_stat bt_stat;  /* bt statistics */
	struct btc_fddt_train_info train;
	struct btc_fddt_train_info train_now;
};

struct btc_fddt_trx_info {
	u8 tx_lvl;
	u8 rx_lvl;
	u8 wl_rssi;
	u8 bt_rssi;

	s8 tx_power; /* absolute Tx power (dBm), 0xff-> no BTC control */
	s8 rx_gain;  /* rx gain table index (TBD.) */
	s8 bt_tx_power; /* decrease Tx power (dB) */
	s8 bt_rx_gain;  /* LNA constrain level */

	u8 cn; /* condition_num */
	s8 nhm;
	u8 bt_profile;
	u8 rsvd2;

	u16 tx_rate;
	u16 rx_rate;

	u32 tx_tp;
	u32 rx_tp;
};

/* dynamic coex mechanism  */
struct btc_dm {
	struct fbtc_slot slot[CXST_MAX];
	struct fbtc_slot slot_now[CXST_MAX];
	struct fbtc_tdma tdma;
	struct fbtc_tdma tdma_now;
	struct btc_gnt_ctrl gnt_set[HW_PHY_MAX];
	struct btc_gnt_ctrl gnt_val[HW_PHY_MAX];
	struct btc_init_info init_info; /* pass to wl_fw if offload */
	struct btc_rf_trx_para rf_trx_para;
	struct btc_wl_tx_limit_para wl_tx_limit;
	struct btc_wl_scc_ctrl wl_scc;
	struct btc_dm_step dm_step;

	struct btc_fddt_info fddt_info;
	struct btc_fddt_trx_info fddt_trx_info;

	union btc_dm_error_map error;

	char run_reason[BTC_RSN_MAXLEN];
	char run_action[BTC_ACT_MAXLEN];

	u16 slot_dur[CXST_MAX]; /* for user-define slot duration */

	u32 set_ant_path;
	u32 cnt_dm[BTC_DCNT_MAX];
	u32 cnt_notify[BTC_NCNT_MAX];

	u32 wl_only: 1; /* drv->Fw if offload  */
	u32 wl_fw_cx_offload: 1; /* BTC_CX_FW_OFFLOAD from FW code  */
	u32 freerun: 1;
	u32 fddt_train: 1;
	u32 wl_ps_ctrl: 2; /* 0: off, 1: force on, 2:forec off */
	u32 leak_ap: 1;
	u32 noisy_level: 3;
	u32 coex_info_map: 8;
	u32 bt_only: 1; /* drv->Fw if offload  */
	u32 wl_btg_rx: 2;  /* if wl rx shared with bt */
	u32 trx_para_level: 8;
	u32 wl_stb_chg: 1; /* if change s1 WL standby mode table = Rx  */
	u32 pta_owner: 1; /* 0x73[2] BTC_CTRL_BY_WL, BTC_CTRL_BT*/
	u32 tdma_instant_excute: 1;
};

/* the wl/bt/zb cx instance */
struct btc_cx {
	struct btc_wl_info wl;
	struct btc_bt_info bt;
	struct btc_3rdcx_info other;
	struct btc_rf_trx_para rf_para;
	u32 state_map; /* wl/bt combined state map  */
	u32 cnt_bt[BTC_BCNT_MAX];
	u32 cnt_wl[BTC_WCNT_MAX];
};

struct btc_ctrl {
	u32 manual: 1;
	u32 igno_bt: 1;
	u32 always_freerun: 1;
	u32 rsvd: 29;
};

struct btc_dbg {
	/* cmd "rb" */
	bool rb_done;
	u32 rb_val;
};

struct btc_tmr {
	void *btc;
	u16 id;
	_os_timer tmr;
};

struct btc_t {
	struct rtw_phl_com_t *phl;
	struct rtw_hal_com_t *hal;

	struct btc_ops *ops;
	const struct btc_chip *chip;

	struct btc_cx cx;
	struct btc_dm dm;
	struct btc_ctrl ctrl;
	struct btc_module mdinfo;
	struct btf_fwinfo fwinfo;
	struct btc_dbg dbg;
	struct btc_bt_psd_dm bt_psd_dm;

	/* btc timers */
	bool tmr_init;
	bool tmr_stop;
	struct btc_tmr timer[BTC_TIMER_MAX];
	/* a delayed timer to start periodic timer (wait phl started) */
	_os_timer delay_tmr;

	char mbuf[BTC_MSG_MAXLEN]; /* msg buffer */
	size_t mlen; /* max msg len */

	u8 policy[BTC_POLICY_MAXLEN]; /* coex policy buffer */
	u16 policy_len;
	u16 policy_type;

	u8 hubmsg[BTC_HUBMSG_MAXLEN]; /* phl hub msg */
	u16 hubmsg_len;
	u32 hubmsg_cnt;
	u32 bt_req_len[HW_PHY_MAX]; /* request bt-slot in WL SCC/MCC +BT coex */
	u8 bt_req_stbc; /*STBC flag*/
	bool bt_req_en;
};

static void _send_fw_cmd(struct btc_t *btc, u8 h2c_class, u8 h2c_func,
			 u8 *param, u16 len);
static void _ntfy_power_on(struct btc_t *btc);
static void _ntfy_power_off(struct btc_t *btc);
static void _ntfy_init_coex(struct btc_t *btc, u8 mode);
static void _ntfy_scan_start(struct btc_t *btc, u8 phy_idx, u8 band);
static void _ntfy_scan_finish(struct btc_t *btc, u8 pyh_idx);
static void _ntfy_switch_band(struct btc_t *btc, u8 phy_idx, u8 band);
static void _ntfy_specific_packet(struct btc_t *btc, u8 pkt_type);
static void _ntfy_role_info(struct btc_t *btc, u8 rid,
			    struct btc_wl_link_info *info,
			    enum link_state reason);
static void _ntfy_radio_state(struct btc_t *btc, u8 rf_state);
static void _ntfy_customerize(struct btc_t *btc, u8 type, u16 len, u8 *buf);
static u8 _ntfy_wl_rfk(struct btc_t *btc, u8 phy_path, u8 type, u8 state);
static void _ntfy_wl_sta(struct btc_t *btc, struct rtw_stats *phl_stats,
			u8 ntfy_num, struct rtw_phl_stainfo_t *sta[],
			u8 reason);
static void _ntfy_fwinfo(struct btc_t *btc, u8 *buf, u32 len, u8 cls, u8 func);
static void _ntfy_timer(struct btc_t *btc, u16 tmr_id);

#ifdef BTC_8852A_SUPPORT
extern const struct btc_chip chip_8852a;
#endif
#ifdef BTC_8852B_SUPPORT
extern const struct btc_chip chip_8852b;
#endif
#ifdef BTC_8852C_SUPPORT
extern const struct btc_chip chip_8852c;
#endif
extern const u32 coex_ver;

void _update_bt_scbd(struct btc_t *btc, bool only_update);
bool hal_btc_init(struct btc_t *btc);
void hal_btc_deinit(struct btc_t *btc);
u32 _read_cx_reg(struct btc_t *btc, u32 offset);
u32 _read_cx_ctrl(struct btc_t *btc);
void _write_bt_reg(struct btc_t *btc, u8 reg_type, u16 addr, u32 val);
void _read_bt_reg(struct btc_t *btc, u8 reg_type, u16 addr);
u32 _read_scbd(struct btc_t *btc);
void _write_scbd(struct btc_t *btc, u32 val, bool state);
void _run_coex(struct btc_t *btc, const char *reason);
void _set_init_info(struct btc_t *btc);
void _update_dm_step(struct btc_t *btc, const char *strin);
void hal_btc_send_event(struct btc_t *btc, enum phl_band_idx hw_band, u8 *buf,
			u32 len, u16 ev_id);
u8 _get_wl_role_idx(struct btc_t *btc, u8 role);
void _update_btc_state_map(struct btc_t *btc);
void _set_fddt_cell_by_antiso(struct btc_t *btc);

#ifdef BTC_CONFIG_FW_IO_OFLD_SUPPORT
void _btc_io_ofld (struct btc_t *btc, u32 srcpath, u32 addr, u32 mask,
		   u32 value, bool trigger);
#endif

#endif /*_HAL_BTC_H_*/
