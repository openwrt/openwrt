#ifndef __INC_BTC_FW_H__
#define __INC_BTC_FW_H__

#include "halbtc_fwdef.h"

#pragma pack(push)
#pragma pack(1)

struct btc_t;

struct btf_tlv {
	u8 type;
	u8 len;
	u8 val[1];
};

enum btf_reg_type {
	REG_MAC = 0x0,
	REG_BB = 0x1,
	REG_RF = 0x2,
	REG_BT_RF = 0x3,
	REG_BT_MODEM = 0x4,
	REG_BT_BLUEWIZE = 0x5,
	REG_BT_VENDOR = 0x6,
	REG_BT_LE = 0x7,
	REG_MAX_TYPE
};

/*
 * h2c_class : 0x10~0x17: BTC
 * Please see : struct rtw_g6_h2c_hdr
 */
enum bt_h2c_class {
	BTFC_SET = 0x10, /* 0x10~0x17 for BTC */
	BTFC_GET = 0x11,
	BTFC_FW_EVENT = 0x12,
	BTFC_MAX
};

/* =======================================
 * BTFC_SET class 0x10 ->function
 */
enum btf_set {
	SET_REPORT_EN = 0x0,
	SET_SLOT_TABLE, /* Slot table  */
	SET_MREG_TABLE, /* moniter register  */
	SET_CX_POLICY,
	SET_GPIO_DBG,
	SET_DRV_INFO,
	SET_DRV_EVENT,
	SET_BT_WREG_ADDR,
	SET_BT_WREG_VAL,
	SET_BT_RREG_ADDR,
	SET_BT_WL_CH_INFO,
	SET_BT_INFO_REPORT,
	SET_BT_IGNORE_WLAN_ACT,
	SET_BT_TX_PWR,
	SET_BT_LNA_CONSTRAIN,
	SET_BT_QUERY_DEV_LIST,
	SET_BT_QUERY_DEV_INFO,
	SET_BT_GOLDEN_RX_RANGE,
	SET_BT_PSD_REPORT,
	SET_H2C_TEST,
	SET_MAX1
};

/* SET_REPORT_EN function 0x0 -> TLV sub-function */
enum btf_set_report_en {
	RPT_EN_TDMA = BIT0,
	RPT_EN_CYCLE = BIT1,
	RPT_EN_MREG = BIT2,
	RPT_EN_BT_VER_INFO = BIT3, /* FW, Coex, Feature*/
	RPT_EN_BT_SCAN_INFO = BIT4,  /* BT scan parameter */
	RPT_EN_BT_DEVICE_INFO = BIT5, /* vendor id, device name, flush-val*/
	RPT_EN_BT_AFH_MAP = BIT6, /* AFH map H/M/L */
	RPT_EN_BT_AFH_MAP_LE = BIT7, /* AFH map L/M */
	RPT_EN_FW_STEP_INFO = BIT8,
	RPT_EN_ALL = 0x1ff
};

/* SET_SLOT_TABLE function 0x1 -> sub-function */
struct btf_set_report {
	u8 fver;
	u32 enable;
	u32 para;
};

struct btf_set_slot_table {
	u8 fver;
	u8 tbl_num;
	u8 buf[1];
};

/* SET_MREG_TABLE function 0x2 -> sub-function */
struct btf_set_mon_reg {
	u8 fver;
	u8 reg_num;
	u8 buf[1];
};

/* SET_CX_POLICY function 0x3 -> TLV sub-function */
enum btf_set_cx_policy {
	CXPOLICY_TDMA = 0x0,
	CXPOLICY_SLOT = 0x1,
	CXPOLICY_TYPE = 0x2,
	CXPOLICY_MAX
};

/* SET_GPIO_DBG function 0x4 -> TLV sub-function */
enum {
	CXDGPIO_EN_MAP = 0x0,
	CXDGPIO_MUX_MAP = 0x1,
	CXDGPIO_MAX
};

/* SET_DRV_INFO function 0x5 -> TLV sub-function */
enum {
	CXDRVINFO_INIT = 0, /* wl_only, dbcc_en...*/
	CXDRVINFO_ROLE, /* Role */
	CXDRVINFO_DBCC, /* DBCC */
	CXDRVINFO_SMAP, /* status map */
	CXDRVINFO_RFK,  /* wl rfk info */
	CXDRVINFO_RUN,  /* wl run reason */
	CXDRVINFO_CTRL, /* ctrl info */
	CXDRVINFO_SCAN, /* scan info */
	CXDRVINFO_TRX,  /* WL traffic to WL fw */
	CXDRVINFO_TXPWR, /* Set WL tx pwr in WL fw */
	CXDRVINFO_FDDT, /* FDD train info  */
	CXDRVINFO_MAX
};

/* SET_DRV_EVENT function 0x6 -> TLV sub-function */
enum {
	CXDRVEVNT_1 = 0x0,
	CXDRVEVNT_2 = 0x1,
	CXDRVEVNT_MAX
};

/* =======================================
 * BTFC_GET class 0x11 ->function
 */
enum btf_get {
	GET_BT_REG,
	GET_MAX
};

/* =======================================
 * BTFC_FW_EVENT class 0x12 ->function
 */
enum btf_fw_event {
	BTF_EVNT_RPT = 0,
	BTF_EVNT_BT_INFO = 1,
	BTF_EVNT_BT_SCBD = 2,
	BTF_EVNT_BT_REG = 3,
	BTF_EVNT_CX_RUNINFO = 4,
	BTF_EVNT_BT_PSD = 5,
	BTF_EVNT_BT_DEV_INFO = 6,
	BTF_EVNT_BUF_OVERFLOW,
	BTF_EVNT_C2H_LOOPBACK,
	BTF_EVNT_MAX
};

struct btf_event {
	u8 event;
	u8 reserved;
	u16 length;
	u8 buf[1];
};

/* BTF_EVNT_RPT function 0x0 -> TLV type */
enum btf_fw_event_report {
	BTC_RPT_TYPE_CTRL = 0x0,
	BTC_RPT_TYPE_TDMA,
	BTC_RPT_TYPE_SLOT,
	BTC_RPT_TYPE_CYSTA,
	BTC_RPT_TYPE_STEP,
	BTC_RPT_TYPE_NULLSTA,
	BTC_RPT_TYPE_MREG,
	BTC_RPT_TYPE_GPIO_DBG,
	BTC_RPT_TYPE_BT_VER,
	BTC_RPT_TYPE_BT_SCAN,
	BTC_RPT_TYPE_BT_AFH,
	BTC_RPT_TYPE_BT_DEVICE,
	BTC_RPT_TYPE_TEST,
	BTC_RPT_TYPE_MAX = 31
};

struct btc_rpt_cmn_info {
	u32 rx_cnt;
	u32 rx_len;
	u32 req_len; /* expected rsp len */
	u8 req_fver; /* expected rsp fver */
	u8 rsp_fver; /* fver from fw */
	u8 valid;
};

#pragma pack(pop)

struct btc_report_ctrl_state {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_rpt_ctrl finfo; /* info from fw */
};

struct btc_fbtc_tdma {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_1tdma finfo; /* info from fw */
};

struct btc_fbtc_slots {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_slots finfo; /* info from fw */
};

struct btc_fbtc_cysta {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_cysta finfo; /* info from fw */
};

struct btc_fbtc_step {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_steps finfo; /* info from fw */
};

struct btc_fbtc_nullsta {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_cynullsta finfo; /* info from fw */
};

struct btc_fbtc_mreg {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_mreg_val finfo; /* info from fw */
};

struct btc_fbtc_gpio_dbg {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_gpio_dbg finfo; /* info from fw */
};

struct btc_fbtc_btver {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_btver finfo; /* info from fw */
};

struct btc_fbtc_btscan {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_btscan finfo; /* info from fw */
};

struct btc_fbtc_btafh {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_btafh finfo; /* info from fw */
};

struct btc_fbtc_btdev {
	struct btc_rpt_cmn_info cinfo; /* common info, by driver */
	struct fbtc_btdevinfo finfo; /* info from fw */
};

enum {
	BTFRE_INVALID_INPUT = 0x0, /* invalid input parameters */
	BTFRE_UNDEF_TYPE,
	BTFRE_EXCEPTION,
	BTFRE_MAX
};

struct btf_fwinfo {
	u32 cnt_c2h; /* total c2h cnt */
	u32 cnt_h2c; /* total h2c cnt */
	u32 cnt_h2c_fail; /* total h2c fail cnt */
	u32 event[BTF_EVNT_MAX]; /* event statistics */

	u32 err[BTFRE_MAX]; /* counters for statistics */
	u32 len_mismch; /* req/rsp length mis match */
	u32 fver_mismch; /* req/rsp fver mis match */
	u32 rpt_en_map;

	struct btc_report_ctrl_state rpt_ctrl;
	struct btc_fbtc_tdma rpt_fbtc_tdma;
	struct btc_fbtc_slots rpt_fbtc_slots;
	struct btc_fbtc_cysta rpt_fbtc_cysta;
	struct btc_fbtc_step rpt_fbtc_step;
	struct btc_fbtc_nullsta rpt_fbtc_nullsta;
	struct btc_fbtc_mreg rpt_fbtc_mregval;
	struct btc_fbtc_gpio_dbg rpt_fbtc_gpio_dbg;
	struct btc_fbtc_btver rpt_fbtc_btver;
	struct btc_fbtc_btscan rpt_fbtc_btscan;
	struct btc_fbtc_btafh rpt_fbtc_btafh;
	struct btc_fbtc_btdev rpt_fbtc_btdev;
};

/*
 * extern functions
 */

void _chk_btc_err(struct btc_t *btc, u8 type, u32 cnt);
void hal_btc_fw_event(struct btc_t *btc, u8 evt_id, void *data, u32 len);
void hal_btc_fw_en_rpt(struct btc_t *btc, u32 rpt_map, u32 rpt_state);
void hal_btc_fw_set_slots(struct btc_t *btc, u8 num, struct fbtc_slot *s);
void hal_btc_fw_set_monreg(struct btc_t *btc);
bool hal_btc_fw_set_1tdma(struct btc_t *btc,  u16 len, u8 *buf);
bool hal_btc_fw_set_1slot(struct btc_t *btc,  u16 len, u8 *buf);
bool hal_btc_fw_set_policy(struct btc_t *btc, bool force_exec, u16 policy_type,
			   const char* action);
void hal_btc_fw_set_gpio_dbg(struct btc_t *btc, u8 type, u32 val);
void hal_btc_fw_set_drv_info(struct btc_t *btc, u8 type);
void hal_btc_fw_set_bt(struct btc_t *btc, u8 type, u16 len, u8* buf);
void hal_btc_fw_set_drv_event(struct btc_t *btc, u8 type);
void hal_btc_notify_ps_tdma(struct btc_t *btc, bool tdma_start);

#endif	/* __INC_BTC_FW_H__ */
