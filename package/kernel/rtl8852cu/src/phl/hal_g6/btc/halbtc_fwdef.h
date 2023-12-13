#ifndef __INC_BTC_FW_DEF_H__
#define __INC_BTC_FW_DEF_H__

#pragma pack(push)
#pragma pack(1)
/*
 * shared FW Definition
 */

#define BTC_AISO_SUPPORT
//#define BTC_FDDT_TRAIN_SUPPORT /* this setup must sync with fw btc  */
//#define BTC_FW_STEP_DBG

#define CXMREG_MAX 30
#define FCXDEF_STEP 50 /* MUST fw_step size*/
#define BTC_CYCLE_SLOT_MAX 48 /* must be even number, non-zero */

#ifdef BTC_AISO_SUPPORT
enum btc_aiso_method {
	/* tx busy raw */
	BTC_AISO_M0, /* m0: busy avg */
	BTC_AISO_M1, /* m1: busy max */
	BTC_AISO_M2, /* m2: busy remove max and min */
	BTC_AISO_M3, /* m3: busy remove max */

	/* tx busy filter */
	BTC_AISO_M4, /* m4:filter m1 */

	/* all data */
	BTC_AISO_M5, /* m5: hold max value */
	BTC_AISO_M6, /* m6: filter m5 */

	/* avg */
	BTC_AISO_M01_AVG, /* avg (m0 + m1) */
	BTC_AISO_M12_AVG, /* avg (m1 + m2) */
	BTC_AISO_M13_AVG, /* avg (m1 + m3) */

	/* avg */
	BTC_AISO_M46_AVG, /* avg (m4 + m6) */

	BTC_AISO_M_MAX,
	BTC_AISO_M_ALL = 0xFF,
};

struct btc_aiso_val {
	u16 psd_rec_cnt;

	s8 psd_max[10];
	s8 psd_min[10];

	s8 txbusy_psd_max[10];
	s8 txbusy_psd_min[10];
	s8 txbusy_psd_avg[10];
	u8 txbusy_psd_cnt[10];
	s16 txbusy_psd_sum[10];

	s8 txidle_psd_max[10];
	s8 txidle_psd_min[10];
	s8 txidle_psd_avg[10];
	u8 txidle_psd_cnt[10];
	s16 txidle_psd_sum[10];

	s8 wl_air_psd_avg;   /* Avg(Tx-power) in dBm per MHz */
	s32 wl_air_psd_sum;  /* Sum(Tx-power) in dBm per MHz */

	u8 last_1st_half[6];
	u8 last_2nd_half[6];

	s16 aiso_md[BTC_AISO_M_MAX];
	s8 rx_psd[BTC_AISO_M_MAX];
};

struct btc_bt_psd_dm {
	struct btc_aiso_val aiso_val;
	u8 aiso_data_ok;
	u8 aiso_db_cnt;
	u8 aiso_cmd_cnt;
	u8 aiso_db[16];
	u8 aiso_sort_db[16];
	u8 aiso_sort_avg;
	u8 raw_info[8];

	u8 aiso_method;
	u8 aiso_method_final;
	u8 rec_start;
	u8 wl_tx_ss;
	u8 wl_ch_last;
	u16 wl_tx_rate;

	bool en;
	bool rec_time_out;
};
#endif

enum btc_bt_sta_counter {
	BTC_BCNT_RFK_REQ = 0,
	BTC_BCNT_HI_TX = 1,
	BTC_BCNT_HI_RX = 2,
	BTC_BCNT_LO_TX = 3,
	BTC_BCNT_LO_RX = 4,
	BTC_BCNT_POLLUTED = 5,
	BTC_BCNT_STA_MAX
};

struct btc_rpt_ctrl_info {
	u32 cx_ver; /* match which driver's coex version */
	u32 fw_ver;
	u32 en; /* report map */

	u16 cnt; /* fw report counter */
	u16 cnt_c2h; /* fw send c2h counter  */
	u16 cnt_h2c; /* fw recv h2c counter */
	u16 len_c2h; /* The total length of the last C2H  */

	u16 cnt_aoac_rf_on;  /* rf-on counter for aoac switch notify */
	u16 cnt_aoac_rf_off; /* rf-off counter for aoac switch notify */
};

struct btc_rpt_ctrl_a2dp_empty {
	u32 cnt_empty; /* a2dp empty count */
	u32 cnt_flowctrl; /* a2dp empty flow control counter */
	u32 cnt_tx;
	u32 cnt_ack;
	u32 cnt_nack;
};

struct btc_rpt_ctrl_bt_mailbox {
	u32 cnt_send_ok; /* fw send mailbox ok counter */
	u32 cnt_send_fail; /* fw send mailbox fail counter */
	u32 cnt_recv; /* fw recv mailbox counter */
	struct btc_rpt_ctrl_a2dp_empty a2dp;
};

#define FCX_BTCRPT_VER 5
struct fbtc_rpt_ctrl {
	u8 fver;
	u8 rsvd;
	u16 rsvd1;

	u8 gnt_val[HW_PHY_MAX][4];
	u16 bt_cnt[BTC_BCNT_STA_MAX];

	struct btc_rpt_ctrl_info rpt_info;
	struct btc_rpt_ctrl_bt_mailbox bt_mbx_info;
};

/*
 * ============== TDMA related ==============
 */
enum fbtc_tdma_template {
	CXTD_OFF = 0x0,
	CXTD_OFF_B2,
	CXTD_OFF_EXT,
	CXTD_FIX,
	CXTD_PFIX,
	CXTD_AUTO,
	CXTD_PAUTO,
	CXTD_AUTO2,
	CXTD_PAUTO2,
	CXTD_MAX
};

enum fbtc_tdma_type {
	CXTDMA_OFF = 0x0, /* tdma off */
	CXTDMA_FIX = 0x1, /* fixed slot */
	CXTDMA_AUTO = 0x2, /* auto slot */
	CXTDMA_AUTO2 = 0x3, /* extended auto slot */
	CXTDMA_MAX
};

enum fbtc_tdma_rx_flow_ctrl {
	CXFLC_OFF = 0x0,  /* rx flow off */
	CXFLC_NULLP = 0x1, /* Null/Null-P */
	CXFLC_QOSNULL = 0x2, /* QOS Null/Null-P */
	CXFLC_CTS = 0x3,  /* CTS to Self control */
	CXFLC_MAX
};

enum fbtc_tdma_wlan_tx_pause {
	CXTPS_OFF = 0x0,  /* no wl tx pause*/
	CXTPS_ON = 0x1,
	CXTPS_MAX
};

/* define if ext-ctrl-slot allowed while TDMA off */
enum fbtc_ext_ctrl_type {
	CXECTL_OFF = 0x0, /* tdma off */
	CXECTL_B2 = 0x1, /* allow B2 (beacon-early) */
	CXECTL_EXT = 0x2,
	CXECTL_MAX
};

union fbtc_rxflct {
	u8 val;
	u8 type: 3;
	u8 tgln_n: 5;
};

/* define if ext-ctrl-slot allowed while TDMA off */
enum fbtc_option_ctrl_def {
	CXOPCTL_INST_EXEC = BIT(0), /* tdma instant execute */
	CXOPCTL_FDDT_ENABLE = BIT(1),/* info wl fw to enable fdd-train*/
	CXOPCTL_FDDT_RENEW = BIT(2), /* info wl fw to update fddt_info.train */
	CXOPCTL_FDDT_DEBUG = BIT(3)  /* info wl fw to enter debug mode */
};

#define FCX_TDMA_VER 3
struct fbtc_tdma {
	u8 type; /* refer to fbtc_tdma_type*/
	u8 rxflctrl; /* refer to fbtc_tdma_rx_flow_ctrl */
	u8 txflctrl; /* If WL stop Tx while enter BT-slot */
	u8 rsvd;
	u8 leak_n; /* every leak_n cycle do leak detection */
	u8 ext_ctrl; /* refer to fbtc_ext_ctrl_type*/
	/* send rxflctrl to which role
	 * enum role_type, default: 0 for single-role
	 * if multi-role: [7:4] second-role, [3:0] fisrt-role
	 */
	u8 rxflctrl_role;
	u8 option_ctrl; /*bit0: execute immediately, no tdma cycle waitting */
};

struct fbtc_1tdma {
	u8 fver;
	u8 rsvd;
	u16 rsvd1;
	struct fbtc_tdma tdma;
};

/*
 * ============== SLOT related ==============
 */
 enum { /* slot */
	CXST_OFF = 0x0,
	CXST_B2W = 0x1,
	CXST_W1 = 0x2,
	CXST_W2 = 0x3,
	CXST_W2B = 0x4,
	CXST_B1 = 0x5,
	CXST_B2 = 0x6,
	CXST_B3 = 0x7,
	CXST_B4 = 0x8,
	CXST_LK = 0x9,
	CXST_BLK = 0xa,
	CXST_E2G = 0xb, /* for ext-control-slot 2G*/
	CXST_E5G = 0xc, /* for ext-control-slot 5G*/
	CXST_EBT = 0xd, /* for ext-control-slot BT*/
	CXST_ENULL = 0xe, /* for ext-control-slot Null*/
	CXST_WLK = 0xf, /* for WL link slot */
	CXST_W1FDD = 0x10,
	CXST_B1FDD = 0x11,
	CXST_MAX = 0x12 /* The max slot must be even*/
};

enum {
	CXEVNT_TDMA_ENTRY = 0x0,
	CXEVNT_WL_TMR,
	CXEVNT_B1_TMR,
	CXEVNT_B2_TMR,
	CXEVNT_B3_TMR,
	CXEVNT_B4_TMR,
	CXEVNT_W2B_TMR,
	CXEVNT_B2W_TMR,
	CXEVNT_BCN_EARLY,
	CXEVNT_A2DP_EMPTY,
	CXEVNT_LK_END,
	CXEVNT_RX_ISR,
	CXEVNT_RX_FC0,
	CXEVNT_RX_FC1,
	CXEVNT_BT_RELINK,
	CXEVNT_BT_RETRY,
	CXEVNT_E2G,
	CXEVNT_E5G,
	CXEVNT_EBT,
	CXEVNT_ENULL,
	CXEVNT_DRV_WLK,
	CXEVNT_BCN_OK,
	CXEVNT_BT_CHANGE,
	CXEVNT_EBT_EXTEND,
	CXEVNT_E2G_NULL1,
	CXEVNT_B1FDD_TMR,
	CXEVNT_MAX
};

enum {
	CXBCN_ALL = 0x0,
	CXBCN_ALL_OK,
	CXBCN_BT_SLOT,
	CXBCN_BT_OK,
	CXBCN_MAX
};

/* Slot isolation Definition
 * Same definition as WL RX Definition
 */
enum {
	SLOT_MIX = 0x0, /* accept BT Lower-Pri Tx/Rx request 0x778 = 1 */
	SLOT_ISO = 0x1, /* no accept BT Lower-Pri Tx/Rx request 0x778 = d*/
	CXSTYPE_MAX
};

enum { /* TIME */
	CXT_BT = 0x0,
	CXT_WL = 0x1,
	CXT_MAX
};

enum { /* TIME-A2DP */
	CXT_FLCTRL_OFF = 0x0,
	CXT_FLCTRL_ON = 0x1,
	CXT_FLCTRL_MAX
};

enum { /* STEP TYPE */
	CXSTEP_NONE = 0x0,
	CXSTEP_EVNT = 0x1,
	CXSTEP_SLOT = 0x2,
	CXSTEP_MAX
};

enum {
	CXNULL_STATE_0 = 0,
	CXNULL_STATE_1 = 1,
	CXNULL_STATE_MAX = 2
};

enum {
	CXNULL_FAIL = 0,
	CXNULL_OK = 1,
	CXNULL_LATE = 2,
	CXNULL_RETRY = 3,
	CXNULL_TX = 4,
	CXNULL_MAX = 5
};

struct fbtc_set_drvinfo {
	u8 type;
	u8 len;
	u8 buf[1];
};

#define FCX_GPIODBG_VER 1
#define BTC_DBG_MAX1  32
struct fbtc_gpio_dbg {
	u8 fver;
	u8 rsvd;
	u16 rsvd2;
	u32 en_map; /* which debug signal (see btc_wl_gpio_debug) is enable */
	u32 pre_state; /* the debug signal is 1 or 0  */
	u8 gpio_map[BTC_DBG_MAX1]; /* debug signals to GPIO-Position mapping */
};

#define FCX_MREG_VER 1
struct fbtc_mreg_val {
	u8 fver;
	u8 reg_num;
	u16 rsvd;
	u32 mreg_val[CXMREG_MAX];
};

struct fbtc_mreg {
	u16 type;
	u16 bytes;
	u32 offset;
};

#define FCX_SLOT_VER 1
struct fbtc_slot {
	u16 dur; /* slot duration */
	u32 cxtbl;
	u16 cxtype;
};

struct fbtc_1slot {
	u8 fver;
	u8 sid; /* slot id */
	struct fbtc_slot slot;
};

struct fbtc_slots {
	u8 fver;
	u8 tbl_num;
	u16 rsvd;
	u32 update_map;
	struct fbtc_slot slot[CXST_MAX];
};

#define FCX_STEP_VER 3
struct fbtc_step {
	u8 type;
	u8 val;
	u16 difft;
};

struct fbtc_steps {
	u8 fver;
	u8 en;
	u16 rsvd;
	u32 cnt;
#ifdef BTC_FW_STEP_DBG
	struct fbtc_step step[FCXDEF_STEP];
#endif
};

#define FCX_CYSTA_VER 4

struct fbtc_sta_time_info {
	u16 tavg[CXT_MAX]; /* avg wl/bt cycle time */
	u16 tmax[CXT_MAX]; /* max wl/bt cycle time */
	u16 tmaxdiff[CXT_MAX]; /* max wl-wl bt-bt cycle diff time */
};

struct fbtc_cycle_a2dptrx_info {
	u8 empty_cnt;
	u8 retry_cnt;
	u8 tx_rate;
	u8 tx_cnt;
	u8 ack_cnt;
	u8 nack_cnt;
	u8 no_empty_cnt;
	u8 rsvd;
};

struct fbtc_cycle_fddt_info {
	u16 phase_cycle: 10;
	u16 train_step: 6;
	u16 tp;

	s8 tx_power; /* absolute Tx power (dBm), 0xff-> no BTC control */
	s8 bt_tx_power; /* decrease Tx power (dB) */
	s8 bt_rx_gain;  /* LNA constrain level */
	u8 no_empty_cnt;

	u8 rssi; /* [7:4] -> bt_rssi_level, [3:0]-> wl_rssi_level */
	u8 cn; /* condition_num */
	u8 train_status; /* [7:4]-> state, [3:0]-> phase */
	u8 train_result; /* response: break,pass..  */
};

struct fbtc_fddt_cell_status {
	s8 wl_tx_pwr;
	s8 bt_tx_pwr;
	s8 bt_rx_gain;
	u8 state: 4; /* train state */
	u8 phase: 4;  /* train phase */
};

struct fbtc_sta_a2dpept_info {
	u16 cnt; /* a2dp empty cnt */
	u16 cnt_timeout; /* a2dp empty timeout cnt*/
	u16 tavg; /* avg a2dp empty time */
	u16 tmax; /* max a2dp empty time */
};

struct fbtc_sta_leak_info {
	u32 cnt_rximr; /* the rximr occur at leak slot  */
	u16 tavg; /* avg leak-slot time */
	u16 tamx; /* max leak-slot time */
};

struct fbtc_cysta { /* statistics for cycles */
	u8 fver;
	u8 rsvd;
	u8 collision_cnt; /* counter for event/timer occur at the same time */
	u8 except_cnt;

	u16 skip_cnt;
	u16 cycles; /* total cycle number */

	u16 slot_step_time[BTC_CYCLE_SLOT_MAX]; /* record the wl/bt slot time */
	u16 slot_cnt[CXST_MAX]; /* slot count */
	u16 bcn_cnt[CXBCN_MAX];

	struct fbtc_sta_time_info cycle_time;
	struct fbtc_sta_leak_info leak_slot;
	struct fbtc_sta_a2dpept_info a2dp_ept;
	struct fbtc_cycle_a2dptrx_info a2dp_trx[BTC_CYCLE_SLOT_MAX];

	struct fbtc_cycle_fddt_info fddt_trx[BTC_CYCLE_SLOT_MAX];
	struct fbtc_fddt_cell_status fddt_cells [2][5][5];

	u32 except_map;
};

#define FCX_NULLSTA_VER 2
struct fbtc_cynullsta { /* cycle null statistics */
	u8 fver;
	u8 rsvd;
	u16 rsvd2;
	u32 tmax[CXNULL_STATE_MAX]; /* max_t for 0:null0/1:null1 */
	u32 tavg[CXNULL_STATE_MAX]; /* avg_t for 0:null0/1:null1 */
	/* result for null , 0:fail, 1:ok, 2:late, 3:retry */
	u32 result[CXNULL_STATE_MAX][CXNULL_MAX];
};

#define FCX_BTVER_VER 1
struct fbtc_btver {
	u8 fver;
	u8 rsvd;
	u16 rsvd2;
	u32 coex_ver; /*bit[15:8]->shared, bit[7:0]->non-shared */
	u32 fw_ver;
	u32 feature;
};

#define FCX_BTSCAN_VER 1
struct fbtc_btscan {
	u8 fver;
	u8 rsvd;
	u16 rsvd2;
	u8 scan[6];
};

#define FCX_BTAFH_VER 2
struct fbtc_btafh {
	u8 fver;
	u8 rsvd;
	u8 rsvd2;
	u8 map_type;
	u8 afh_l[4]; /*bit0:2402, bit1: 2403.... bit31:2433 */
	u8 afh_m[4]; /*bit0:2434, bit1: 2435.... bit31:2465 */
	u8 afh_h[4]; /*bit0:2466, bit1:2467......bit14:2480 */
	u8 afh_le_a[4];
	u8 afh_le_b[4];
};

enum {
	BTC_RPT_BT_AFH_SEQ_LEGACY = 0x10,
	BTC_RPT_BT_AFH_SEQ_LE = 0x20
};

#define FCX_BTDEVINFO_VER 1
struct fbtc_btdevinfo {
	u8 fver;
	u8 rsvd;
	u16 vendor_id;
	u32 dev_name; /* only 24 bits valid */
	u32 flush_time;
};

/*
 * End of FW Definition
 */
#pragma pack(pop)

#endif	/* __INC_BTC_FW_DEF_H__ */
