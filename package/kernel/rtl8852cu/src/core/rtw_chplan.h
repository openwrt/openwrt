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
#ifndef __RTW_CHPLAN_H__
#define __RTW_CHPLAN_H__

#define RTW_CHPLAN_IOCTL_NULL		0xFFFE /* special value by ioctl: null(empty) chplan */
#define RTW_CHPLAN_IOCTL_UNSPECIFIED	0xFFFF /* special value by ioctl: no change (keep original) */

#define RTW_CHPLAN_NULL		0x0A
#define RTW_CHPLAN_WORLDWIDE	0x7F
#define RTW_CHPLAN_UNSPECIFIED	0xFF
#define RTW_CHPLAN_6G_NULL		0x00
#define RTW_CHPLAN_6G_WORLDWIDE		0x7F
#define RTW_CHPLAN_6G_UNSPECIFIED	0xFF

u8 rtw_chplan_get_default_regd_2g(u8 id);
#if CONFIG_IEEE80211_BAND_5GHZ
u8 rtw_chplan_get_default_regd_5g(u8 id);
#endif
bool rtw_is_channel_plan_valid(u8 id);

#if CONFIG_IEEE80211_BAND_6GHZ
u8 rtw_chplan_get_default_regd_6g(u8 id);
bool rtw_is_channel_plan_6g_valid(u8 id);
#endif

void rtw_rfctl_addl_ch_disable_conf_init(struct rf_ctl_t *rfctl, struct registry_priv *regsty);

u8 rtw_chplan_is_bchbw_valid(u8 id, u8 id_6g, enum band_type band, u8 ch, u8 bw, u8 offset
	, bool allow_primary_passive, bool allow_passive, struct rf_ctl_t *rfctl);

enum regd_src_t {
	REGD_SRC_RTK_PRIV = 0, /* Regulatory settings from Realtek framework (Realtek defined or customized) */
	REGD_SRC_OS = 1, /* Regulatory settings from OS */
	REGD_SRC_NUM,
};

#define regd_src_is_valid(src) ((src) < REGD_SRC_NUM)

extern const char *_regd_src_str[];
#define regd_src_str(src) ((src) >= REGD_SRC_NUM ? _regd_src_str[REGD_SRC_NUM] : _regd_src_str[src])

void rtw_rfctl_chset_apply_regulatory(struct dvobj_priv *dvobj, bool req_lock);

bool rtw_rfctl_reg_allow_beacon_hint(struct rf_ctl_t *rfctl);
bool rtw_chinfo_allow_beacon_hint(struct _RT_CHANNEL_INFO *chinfo);
u8 rtw_process_beacon_hint(struct rf_ctl_t *rfctl, WLAN_BSSID_EX *bss);
void rtw_beacon_hint_ch_change_notifier(struct rf_ctl_t *rfctl);

#define ALPHA2_FMT "%c%c"
#define ALPHA2_ARG(a2) ((is_alpha(a2[0]) || is_decimal(a2[0])) ? a2[0] : '-'), ((is_alpha(a2[1]) || is_decimal(a2[1])) ? a2[1] : '-')

#define WORLDWIDE_ALPHA2	"00"
#define UNSPEC_ALPHA2		"99"
#define INTERSECTED_ALPHA2	"98"

#define IS_ALPHA2_WORLDWIDE(_alpha2)	(strncmp(_alpha2, WORLDWIDE_ALPHA2, 2) == 0)
#define IS_ALPHA2_UNSPEC(_alpha2)	(strncmp(_alpha2, UNSPEC_ALPHA2, 2) == 0)
#define IS_ALPHA2_INTERSECTED(_alpha2)	(strncmp(_alpha2, INTERSECTED_ALPHA2, 2) == 0)
#define SET_UNSPEC_ALPHA2(_alpha2)		do { _rtw_memcpy(_alpha2, UNSPEC_ALPHA2, 2); } while (0)
#define SET_INTERSECTEDC_ALPHA2(_alpha2)	do { _rtw_memcpy(_alpha2, INTERSECTED_ALPHA2, 2); } while (0)

enum rtw_regd_inr {
	RTW_REGD_SET_BY_INIT = 0,
	RTW_REGD_SET_BY_USER = 1,
	RTW_REGD_SET_BY_COUNTRY_IE = 2,

	/* below is not used for REGD_SRC_RTK_PRIV */
	RTW_REGD_SET_BY_DRIVER = 3,
	RTW_REGD_SET_BY_CORE = 4,

	RTW_REGD_SET_BY_NUM,
};

extern const char *const _regd_inr_str[];
#define regd_inr_str(inr) (((inr) >= RTW_REGD_SET_BY_NUM) ? _regd_inr_str[RTW_REGD_SET_BY_NUM] : _regd_inr_str[(inr)])

enum rtw_regd {
	RTW_REGD_NA = 0,
	RTW_REGD_FCC = 1,
	RTW_REGD_MKK = 2,
	RTW_REGD_ETSI = 3,
	RTW_REGD_IC = 4,
	RTW_REGD_KCC = 5,
	RTW_REGD_NCC = 6,
	RTW_REGD_ACMA = 7,
	RTW_REGD_CHILE = 8,
	RTW_REGD_MEX = 9,
	RTW_REGD_WW,
	RTW_REGD_NUM,
};

extern const char *const _regd_str[];
#define regd_str(regd) (((regd) >= RTW_REGD_NUM) ? _regd_str[RTW_REGD_NA] : _regd_str[(regd)])

enum rtw_edcca_mode_t {
	RTW_EDCCA_NORM	= 0, /* normal */
	RTW_EDCCA_CS	= 1, /* carrier sense */
	RTW_EDCCA_ADAPT	= 2, /* adaptivity */

	RTW_EDCCA_MODE_NUM,
	RTW_EDCCA_DEF		= RTW_EDCCA_MODE_NUM, /* default (ref to domain code), used at country chplan map's override field */
};

extern const char *const _rtw_edcca_mode_str[];
#define rtw_edcca_mode_str(mode) (((mode) >= RTW_EDCCA_MODE_NUM) ? _rtw_edcca_mode_str[RTW_EDCCA_NORM] : _rtw_edcca_mode_str[(mode)])

enum rtw_dfs_regd {
	RTW_DFS_REGD_NONE	= 0,
	RTW_DFS_REGD_FCC	= 1,
	RTW_DFS_REGD_MKK	= 2,
	RTW_DFS_REGD_ETSI	= 3,
	RTW_DFS_REGD_KCC	= 4,
	RTW_DFS_REGD_NUM,
	RTW_DFS_REGD_AUTO	= 0xFF, /* follow channel plan */
};

#define RTW_DFS_REGD_IS_UNKNOWN(regd) ((regd) == RTW_DFS_REGD_NONE || (regd) >= RTW_DFS_REGD_NUM)

extern const char *const _rtw_dfs_regd_str[];
#define rtw_dfs_regd_str(region) (((region) >= RTW_DFS_REGD_NUM) ? _rtw_dfs_regd_str[RTW_DFS_REGD_NONE] : _rtw_dfs_regd_str[(region)])

typedef enum _REGULATION_TXPWR_LMT {
	TXPWR_LMT_NONE = 0, /* no limit */
	TXPWR_LMT_FCC = 1,
	TXPWR_LMT_MKK = 2,
	TXPWR_LMT_ETSI = 3,
	TXPWR_LMT_IC = 4,
	TXPWR_LMT_KCC = 5,
	TXPWR_LMT_NCC = 6,
	TXPWR_LMT_ACMA = 7,
	TXPWR_LMT_CHILE = 8,
	TXPWR_LMT_UKRAINE = 9,
	TXPWR_LMT_MEXICO = 10,
	TXPWR_LMT_CN = 11,
	TXPWR_LMT_QATAR = 12,
	TXPWR_LMT_UK = 13,
	TXPWR_LMT_WW, /* smallest of all available limit, keep last */

	TXPWR_LMT_NUM,
	TXPWR_LMT_DEF = TXPWR_LMT_NUM, /* default (ref to domain code), used at country chplan map's override field */
} REGULATION_TXPWR_LMT;

extern const char *const _txpwr_lmt_str[];
#define txpwr_lmt_str(regd) (((regd) >= TXPWR_LMT_NUM) ? _txpwr_lmt_str[TXPWR_LMT_NUM] : _txpwr_lmt_str[(regd)])

extern const REGULATION_TXPWR_LMT _txpwr_lmt_alternate[];
#define txpwr_lmt_alternate(ori) (((ori) > TXPWR_LMT_NUM) ? _txpwr_lmt_alternate[TXPWR_LMT_WW] : _txpwr_lmt_alternate[(ori)])

#define TXPWR_LMT_ALTERNATE_DEFINED(txpwr_lmt) (txpwr_lmt_alternate(txpwr_lmt) != txpwr_lmt)

extern const enum rtw_edcca_mode_t _rtw_regd_to_edcca_mode[RTW_REGD_NUM];
#define rtw_regd_to_edcca_mode(regd) (((regd) >= RTW_REGD_NUM) ? RTW_EDCCA_NORM : _rtw_regd_to_edcca_mode[(regd)])

extern const REGULATION_TXPWR_LMT _rtw_regd_to_txpwr_lmt[];
#define rtw_regd_to_txpwr_lmt(regd) (((regd) >= RTW_REGD_NUM) ? TXPWR_LMT_WW : _rtw_regd_to_txpwr_lmt[(regd)])

#define REGD_INR_BMP_STR_LEN (33)
char *rtw_get_regd_inr_bmp_str(char *buf, u8 bmp);

#define EDCCA_MODE_OF_BANDS_STR_LEN (((6 + 3 + 1) * BAND_MAX) + 1)
char *rtw_get_edcca_mode_of_bands_str(char *buf, u8 mode_of_band[]);
void rtw_edcca_mode_update(struct dvobj_priv *dvobj, bool req_lock);
u8 rtw_get_edcca_mode(struct dvobj_priv *dvobj, enum band_type band);

#if CONFIG_TXPWR_LIMIT
#define TXPWR_NAME_OF_BANDS_STR_LEN (((1 + 7 + 1) * BAND_MAX) + 1)
char *rtw_get_txpwr_lmt_name_of_bands_str(char *buf, const char *name_of_band[], u8 unknown_bmp);
void rtw_txpwr_update_cur_lmt_regs(struct dvobj_priv *dvobj, bool req_lock);
#endif

#define CHPLAN_PROTO_EN_A	BIT0
#define CHPLAN_PROTO_EN_AC	BIT1
#define CHPLAN_PROTO_EN_AX	BIT2
#define CHPLAN_PROTO_EN_ALL	0xFF

struct country_chplan {
	char alpha2[2];

	u8 domain_code;
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 domain_code_6g;
#endif

	/* will override edcca mode get by domain code (/6g) */
	u8 edcca_mode_2g_override:2;
#if CONFIG_IEEE80211_BAND_5GHZ
	u8 edcca_mode_5g_override:2;
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 edcca_mode_6g_override:2;
#endif

	/* will override txpwr_lmt get by domain code (/6g) */
	u8 txpwr_lmt_override;

#if defined(CONFIG_80211AX_HE) || defined(CONFIG_80211AC_VHT) || CONFIG_IEEE80211_BAND_5GHZ
	u8 proto_en;
#endif
};

#if CONFIG_IEEE80211_BAND_5GHZ
#define COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_5G_OVERRIDE(_val) , .edcca_mode_5g_override = (_val)
#else
#define COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_5G_OVERRIDE(_val)
#endif

#if CONFIG_IEEE80211_BAND_6GHZ
#define COUNTRY_CHPLAN_ASSIGN_CHPLAN_6G(_val) , .domain_code_6g = (_val)
#define COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_6G_OVERRIDE(_val) , .edcca_mode_6g_override = (_val)
#else
#define COUNTRY_CHPLAN_ASSIGN_CHPLAN_6G(_val)
#define COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_6G_OVERRIDE(_val)
#endif

#if defined(CONFIG_80211AX_HE) || defined(CONFIG_80211AC_VHT) || CONFIG_IEEE80211_BAND_5GHZ
#define COUNTRY_CHPLAN_ASSIGN_PROTO_EN(_val) , .proto_en = (_val)
#else
#define COUNTRY_CHPLAN_ASSIGN_PROTO_EN(_val)
#endif

#define COUNTRY_CHPLAN_ENT(_alpha2, _chplan, _chplan_6g, _txpwr_lmt_override, _en_11ax, _en_11ac, _en_11a) \
	{.alpha2 = (_alpha2), .domain_code = (_chplan) \
		COUNTRY_CHPLAN_ASSIGN_CHPLAN_6G(_chplan_6g) \
		, .edcca_mode_2g_override = RTW_EDCCA_DEF \
		COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_5G_OVERRIDE(RTW_EDCCA_DEF) \
		COUNTRY_CHPLAN_ASSIGN_EDCCA_MODE_6G_OVERRIDE(RTW_EDCCA_DEF) \
		, .txpwr_lmt_override = TXPWR_LMT_##_txpwr_lmt_override \
		COUNTRY_CHPLAN_ASSIGN_PROTO_EN((_en_11ax ? CHPLAN_PROTO_EN_AX : 0) | (_en_11ac ? CHPLAN_PROTO_EN_AC : 0) | (_en_11a ? CHPLAN_PROTO_EN_A : 0)) \
	}

#if CONFIG_IEEE80211_BAND_5GHZ
#define COUNTRY_CHPLAN_EN_11A(_ent) (((_ent)->proto_en & CHPLAN_PROTO_EN_A) ? true : false)
#else
#define COUNTRY_CHPLAN_EN_11A(_ent) false
#endif

#ifdef CONFIG_80211AC_VHT
#define COUNTRY_CHPLAN_EN_11AC(_ent) (((_ent)->proto_en & CHPLAN_PROTO_EN_AC) ? true : false)
#else
#define COUNTRY_CHPLAN_EN_11AC(_ent) false
#endif

#ifdef CONFIG_80211AX_HE
#define COUNTRY_CHPLAN_EN_11AX(_ent) (((_ent)->proto_en & CHPLAN_PROTO_EN_AX) ? true : false)
#else
#define COUNTRY_CHPLAN_EN_11AX(_ent) false
#endif

void rtw_get_chplan_worldwide(struct country_chplan *ent);
bool rtw_get_chplan_from_country(const char *country_code, struct country_chplan *ent);

void rtw_chplan_ioctl_input_mapping(u16 *chplan, u16 *chplan_6g);
bool rtw_chplan_ids_is_world_wide(u8 chplan, u8 chplan_6g);

u8 rtw_country_chplan_is_bchbw_valid(struct country_chplan *ent, enum band_type band, u8 ch, u8 bw, u8 offset
	, bool allow_primary_passive, bool allow_passive, struct rf_ctl_t *rfctl);

enum country_ie_slave_en_mode {
	CISEM_DISABLE	= 0, /* disable */
	CISEM_ENABLE	= 1, /* enable */
	CISEM_ENABLE_WW	= 2, /* enable when INIT/USER set world wide mode */
	CISEM_NUM,
};

#define CIS_EN_MODE_IS_VALID(mode) ((mode) < CISEM_NUM)

enum country_ie_slave_flags {
	CISF_INTERSECT	= BIT0, /* if not set, choose effected one from received IEs */
	CISF_ENV_BSS	= BIT1, /* if not set, consider IEs of associated BSSs only */
};

enum country_ie_slave_status {
	COUNTRY_IE_SLAVE_NOCOUNTRY	= 0,
	COUNTRY_IE_SLAVE_UNKNOWN	= 1,
	COUNTRY_IE_SLAVE_OPCH_NOEXIST	= 2,
	COUNTRY_IE_SLAVE_APPLICABLE	= 3,

	COUNTRY_IE_SLAVE_STATUS_NUM,
};

struct country_ie_slave_record {
	char alpha2[2]; /* country code get from connected AP of STA ifaces, "\x00\x00" is not set */
	enum band_type band;
	u8 opch;
	enum country_ie_slave_status status;
	struct country_chplan chplan;
};

#ifdef CONFIG_80211D
extern const char *const _cis_status_str[];
#define cis_status_str(s) (((s) >= COUNTRY_IE_SLAVE_STATUS_NUM) ? _cis_status_str[COUNTRY_IE_SLAVE_STATUS_NUM] : _cis_status_str[(s)])

void dump_country_ie_slave_records(void *sel, struct rf_ctl_t *rfctl, bool skip_noset);

enum country_ie_slave_status rtw_get_cisr_from_recv_country_ie(struct rf_ctl_t *rfctl
	, enum band_type band, u8 opch, const u8 *country_ie
	, struct country_ie_slave_record *cisr, const char *caller_msg);
#endif

void dump_country_chplan(void *sel, const struct country_chplan *ent, bool regd_info);
void dump_country_chplan_map(void *sel, bool regd_info);
void dump_country_list(void *sel);
void dump_chplan_id_list(void *sel);
void dump_chplan_country_list(void *sel);
#if CONFIG_IEEE80211_BAND_6GHZ
void dump_chplan_6g_id_list(void *sel);
void dump_chplan_6g_country_list(void *sel);
#endif
#ifdef CONFIG_RTW_DEBUG
void dump_chplan_test(void *sel);
#endif
void dump_chplan_ver(void *sel);

struct regd_req_t {
	_list list;
	int ref_cnt; /* used by RTK_PRIV's COUNTRY_IE req */
	bool applied;

	enum regd_src_t src;
	enum rtw_regd_inr inr;

	struct country_chplan chplan;
};

void rtw_regd_req_list_init(struct rf_ctl_t *rfctl, struct registry_priv *regsty);
void rtw_regd_req_list_free(struct rf_ctl_t *rfctl);

void rtw_regd_watchdog_hdl(struct dvobj_priv *dvobj);

struct _ADAPTER_LINK;
enum channel_width alink_adjust_linking_bw_by_regd(struct _ADAPTER_LINK *alink
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset);
enum channel_width adapter_adjust_linking_bw_by_regd(_adapter *adapter
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset);

void rtw_rfctl_decide_init_chplan(struct rf_ctl_t *rfctl,
	const char *hw_alpha2, u8 hw_chplan, u8 hw_chplan_6g, u8 hw_force_chplan);

bool rtw_rfctl_is_disable_sw_channel_plan(struct dvobj_priv *dvobj);

enum chplan_confs_type {
	CHPLAN_CONFS_DIS_CH_FLAGS,
	CHPLAN_CONFS_EXCL_CHS,
	CHPLAN_CONFS_EXCL_CHS_6G,
	CHPLAN_CONFS_INIT_REGD_ALWAYS_APPLY,
	CHPLAN_CONFS_USER_REGD_ALWAYS_APPLY,
	CHPLAN_CONFS_BCN_HINT_VALID_MS,
	CHPLAN_CONFS_CIS_EN_MODE,
	CHPLAN_CONFS_CIS_FLAGS,
	CHPLAN_CONFS_CIS_EN_ROLE,
	CHPLAN_CONFS_CIS_EN_IFBMP,
	CHPLAN_CONFS_CIS_SCAN_INT_MS,
	CHPLAN_CONFS_NUM,
};

struct chplan_confs {
	u16 set_types; /* bitmap of chplan_confs_type */
	u8 dis_ch_flags;
	u8 excl_chs[MAX_CHANNEL_NUM_2G_5G];
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 excl_chs_6g[MAX_CHANNEL_NUM_6G];
#endif
	bool init_regd_always_apply;
	bool user_regd_always_apply;
	u32 bcn_hint_valid_ms;
#ifdef CONFIG_80211D
	u8 cis_en_mode;
	u8 cis_flags;
	u8 cis_en_role;
	u8 cis_en_ifbmp;
	u32 cis_scan_int_ms;
#endif
};

struct SetChannelPlan_param {
	enum regd_src_t regd_src;
	enum rtw_regd_inr inr;
	struct country_chplan country_ent;
	bool has_country;
	u8 channel_plan;
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 channel_plan_6g;
#endif

#ifdef CONFIG_80211D
	/* used for regd_src == RTK_PRIV and inr == COUNTRY_IE */
	struct country_ie_slave_record cisr;
	u8 cisr_alink_id;
	bool has_cisr;
#endif

	struct chplan_confs confs;

#ifdef PLATFORM_LINUX
	bool rtnl_lock_needed;
#endif
};

u8 rtw_set_chplan_hdl(_adapter *adapter, u8 *pbuf);

u8 rtw_set_chplan_cmd(_adapter *adapter, int flags, u8 chplan, u8 chplan_6g, enum rtw_regd_inr inr);
u8 rtw_set_country_cmd(_adapter *adapter, int flags, const char *country_code, enum rtw_regd_inr inr);
#ifdef CONFIG_80211D
u8 rtw_alink_apply_recv_country_ie_cmd(struct _ADAPTER_LINK *alink, int flags, enum band_type band,u8 opch, const u8 *country_ie);
u8 rtw_apply_recv_country_ie_cmd(_adapter *adapter, int flags, enum band_type band,u8 opch, const u8 *country_ie);
u8 rtw_apply_scan_network_country_ie_cmd(_adapter *adapter, int flags);
#endif
#ifdef CONFIG_REGD_SRC_FROM_OS
u8 rtw_sync_os_regd_cmd(_adapter *adapter, int flags, const char *country_code, u8 dfs_region, enum rtw_regd_inr inr);
#endif
u8 rtw_set_chplan_confs_cmd(_adapter *adapter, int flags, struct chplan_confs *confs);
#ifdef CONFIG_PROC_DEBUG
u16 rtw_parse_chplan_confs_cmd_str(struct chplan_confs *confs, char *str);
#endif

struct get_chplan_resp {
	enum regd_src_t regd_src;
	u8 regd_inr_bmp;
	char alpha2[2];
	u8 channel_plan;
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 chplan_6g;
#endif

#if CONFIG_TXPWR_LIMIT
	/* point to content of txpwr_lmt_names of specific band (after content of chset) */
	const char *txpwr_lmt_names[BAND_MAX];
	int txpwr_lmt_names_len[BAND_MAX];
	int txpwr_lmt_names_len_total; /* ease for free operation */
#endif
	u8 edcca_mode_2g;
#if CONFIG_IEEE80211_BAND_5GHZ
	u8 edcca_mode_5g;
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 edcca_mode_6g;
#endif
#ifdef CONFIG_DFS_MASTER
	u8 dfs_domain;
#endif
	u8 proto_en;

	struct chplan_confs confs;

	u8 chs_len;
	RT_CHANNEL_INFO chs[0];
};

struct get_channel_plan_param {
	struct get_chplan_resp **chplan;
};

u8 rtw_get_chplan_hdl(_adapter *adapter, u8 *pbuf);

u8 rtw_get_chplan_cmd(_adapter *adapter, int flags, struct get_chplan_resp **chplan);
void rtw_free_get_chplan_resp(struct get_chplan_resp *chplan);

#ifdef CONFIG_80211D
void rtw_alink_joinbss_update_regulatory(struct _ADAPTER_LINK *alink, const WLAN_BSSID_EX *network);
void rtw_alink_leavebss_update_regulatory(struct _ADAPTER_LINK * alink);
void rtw_alink_csa_update_regulatory(struct _ADAPTER_LINK *alink, enum band_type req_band, u8 req_ch);
void alink_process_country_ie(struct _ADAPTER_LINK *alink, u8 *ies, uint ies_len);

void rtw_joinbss_update_regulatory(_adapter *adapter, const WLAN_BSSID_EX *network);
void rtw_leavebss_update_regulatory(_adapter *adapter);
void rtw_csa_update_regulatory(_adapter *adapter, enum band_type req_band, u8 req_ch);
void process_country_ie(_adapter *adapter, u8 *ies, uint ies_len);

bool rtw_update_scanned_network_cisr(struct rf_ctl_t *rfctl, struct wlan_network *network);

void rtw_rfctl_cis_init(struct rf_ctl_t *rfctl, struct registry_priv *regsty);
void rtw_rfctl_cis_deinit(struct rf_ctl_t *rfctl);
#endif

#ifdef CONFIG_PROC_DEBUG
void dump_cur_chplan_confs(void *sel, struct rf_ctl_t *rfctl);
void dump_cur_country(void *sel, struct rf_ctl_t *rfctl);
void dump_cur_chplan(void *sel, struct rf_ctl_t *rfctl);
#endif

#define CHPLAN_VER_STR_BUF_LEN 16

struct rtw_regdb_ops {
	u8 (*get_default_regd_2g)(u8 id);
#if CONFIG_IEEE80211_BAND_5GHZ
	u8 (*get_default_regd_5g)(u8 id);
#endif
	bool (*is_domain_code_valid)(u8 id);
	bool (*domain_get_ch)(u8 id, u32 ch, u8 *flags);
#if CONFIG_IEEE80211_BAND_6GHZ
	u8 (*get_default_regd_6g)(u8 id);
	bool (*is_domain_code_6g_valid)(u8 id);
	bool (*domain_6g_get_ch)(u8 id, u32 ch, u8 *flags);
#endif

	bool (*get_chplan_from_alpha2)(const char *alpha2, struct country_chplan *ent);

#ifdef CONFIG_RTW_DEBUG
	void (*dump_chplan_test)(void *sel);
#endif
	void (*get_ver_str)(char *buf, size_t buf_len);
};

#endif /* __RTW_CHPLAN_H__ */
