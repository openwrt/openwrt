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
#ifndef _PHL_IE_H_
#define _PHL_IE_H_

#define MAX_ELE_SZ                   768

#define MAX_ELE_LEN 255

/* Element ID */
enum wlan_eid {
	EID_SSID = 0,
	EID_SUPP_RATES = 1,
	EID_FH_PARAMS = 2, /* reserved now */
	EID_DS_PARAMS = 3,
	EID_CF_PARAMS = 4,
	EID_TIM = 5,
	EID_IBSS_PARAMS = 6,
	EID_COUNTRY = 7,
	/* 8, 9 reserved */
	EID_REQUEST = 10,
	EID_QBSS_LOAD = 11,
	EID_EDCA_PARAM_SET = 12,
	EID_TSPEC = 13,
	EID_TCLAS = 14,
	EID_SCHEDULE = 15,
	EID_CHALLENGE = 16,
	/* 17-31 reserved for challenge text extension */
	EID_PWR_CONSTRAINT = 32,
	EID_PWR_CAPABILITY = 33,
	EID_TPC_REQUEST = 34,
	EID_TPC_REPORT = 35,
	EID_SUPPORTED_CHANNELS = 36,
	EID_CHANNEL_SWITCH = 37,
	EID_MEASURE_REQUEST = 38,
	EID_MEASURE_REPORT = 39,
	EID_QUIET = 40,
	EID_IBSS_DFS = 41,
	EID_ERP_INFO = 42,
	EID_TS_DELAY = 43,
	EID_TCLAS_PROCESSING = 44,
	EID_HT_CAPABILITY = 45,
	EID_QOS_CAPA = 46,
	/* 47 reserved for Broadcom */
	EID_RSN = 48,
	EID_802_15_COEX = 49,
	EID_EXT_SUPP_RATES = 50,
	EID_AP_CHAN_REPORT = 51,
	EID_NEIGHBOR_REPORT = 52,
	EID_RCPI = 53,
	EID_MOBILITY_DOMAIN = 54,
	EID_FAST_BSS_TRANSITION = 55,
	EID_TIMEOUT_INTERVAL = 56,
	EID_RIC_DATA = 57,
	EID_DSE_REGISTERED_LOCATION = 58,
	EID_SUPPORTED_REGULATORY_CLASSES = 59,
	EID_EXT_CHANSWITCH_ANN = 60,
	EID_HT_OPERATION = 61,
	EID_SECONDARY_CHANNEL_OFFSET = 62,
	EID_BSS_AVG_ACCESS_DELAY = 63,
	EID_ANTENNA_INFO = 64,
	EID_RSNI = 65,
	EID_MEASUREMENT_PILOT_TX_INFO = 66,
	EID_BSS_AVAILABLE_CAPACITY = 67,
	EID_BSS_AC_ACCESS_DELAY = 68,
	EID_TIME_ADVERTISEMENT = 69,
	EID_RRM_ENABLED_CAPABILITIES = 70,
	EID_MULTIPLE_BSSID = 71,
	EID_BSS_COEX_2040 = 72,
	EID_BSS_INTOLERANT_CHL_REPORT = 73,
	EID_OVERLAP_BSS_SCAN_PARAM = 74,
	EID_RIC_DESCRIPTOR = 75,
	EID_MMIE = 76,
	EID_ASSOC_COMEBACK_TIME = 77,
	EID_EVENT_REQUEST = 78,
	EID_EVENT_REPORT = 79,
	EID_DIAGNOSTIC_REQUEST = 80,
	EID_DIAGNOSTIC_REPORT = 81,
	EID_LOCATION_PARAMS = 82,
	EID_NON_TX_BSSID_CAP =  83,
	EID_SSID_LIST = 84,
	EID_MULTI_BSSID_IDX = 85,
	EID_FMS_DESCRIPTOR = 86,
	EID_FMS_REQUEST = 87,
	EID_FMS_RESPONSE = 88,
	EID_QOS_TRAFFIC_CAPA = 89,
	EID_BSS_MAX_IDLE_PERIOD = 90,
	EID_TSF_REQUEST = 91,
	EID_TSF_RESPOSNE = 92,
	EID_WNM_SLEEP_MODE = 93,
	EID_TIM_BCAST_REQ = 94,
	EID_TIM_BCAST_RESP = 95,
	EID_COLL_IF_REPORT = 96,
	EID_CHANNEL_USAGE = 97,
	EID_TIME_ZONE = 98,
	EID_DMS_REQUEST = 99,
	EID_DMS_RESPONSE = 100,
	EID_LINK_ID = 101,
	EID_WAKEUP_SCHEDUL = 102,
	/* 103 reserved */
	EID_CHAN_SWITCH_TIMING = 104,
	EID_PTI_CONTROL = 105,
	EID_PU_BUFFER_STATUS = 106,
	EID_INTERWORKING = 107,
	EID_ADVERTISEMENT_PROTOCOL = 108,
	EID_EXPEDITED_BW_REQ = 109,
	EID_QOS_MAP_SET = 110,
	EID_ROAMING_CONSORTIUM = 111,
	EID_EMERGENCY_ALERT = 112,
	EID_MESH_CONFIG = 113,
	EID_MESH_ID = 114,
	EID_LINK_METRIC_REPORT = 115,
	EID_CONGESTION_NOTIFICATION = 116,
	EID_PEER_MGMT = 117,
	EID_CHAN_SWITCH_PARAM = 118,
	EID_MESH_AWAKE_WINDOW = 119,
	EID_BEACON_TIMING = 120,
	EID_MCCAOP_SETUP_REQ = 121,
	EID_MCCAOP_SETUP_RESP = 122,
	EID_MCCAOP_ADVERT = 123,
	EID_MCCAOP_TEARDOWN = 124,
	EID_GANN = 125,
	EID_RANN = 126,
	EID_EXT_CAPABILITY = 127,
	/* 128, 129 reserved for Agere */
	EID_PREQ = 130,
	EID_PREP = 131,
	EID_PERR = 132,
	/* 133-136 reserved for Cisco */
	EID_PXU = 137,
	EID_PXUC = 138,
	EID_AUTH_MESH_PEER_EXCH = 139,
	EID_MIC = 140,
	EID_DESTINATION_URI = 141,
	EID_UAPSD_COEX = 142,
	EID_WAKEUP_SCHEDULE = 143,
	EID_EXT_SCHEDULE = 144,
	EID_STA_AVAILABILITY = 145,
	EID_DMG_TSPEC = 146,
	EID_DMG_AT = 147,
	EID_DMG_CAP = 148,
	/* 149 reserved for Cisco */
	EID_CISCO_VENDOR_SPECIFIC = 150,
	EID_DMG_OPERATION = 151,
	EID_DMG_BSS_PARAM_CHANGE = 152,
	EID_DMG_BEAM_REFINEMENT = 153,
	EID_CHANNEL_MEASURE_FEEDBACK = 154,
	/* 155-156 reserved for Cisco */
	EID_AWAKE_WINDOW = 157,
	EID_MULTI_BAND = 158,
	EID_ADDBA_EXT = 159,
	EID_NEXT_PCP_LIST = 160,
	EID_PCP_HANDOVER = 161,
	EID_DMG_LINK_MARGIN = 162,
	EID_SWITCHING_STREAM = 163,
	EID_SESSION_TRANSITION = 164,
	EID_DYN_TONE_PAIRING_REPORT = 165,
	EID_CLUSTER_REPORT = 166,
	EID_RELAY_CAP = 167,
	EID_RELAY_XFER_PARAM_SET = 168,
	EID_BEAM_LINK_MAINT = 169,
	EID_MULTIPLE_MAC_ADDR = 170,
	EID_U_PID = 171,
	EID_DMG_LINK_ADAPT_ACK = 172,
	/* 173 reserved for Symbol */
	EID_MCCAOP_ADV_OVERVIEW = 174,
	EID_QUIET_PERIOD_REQ = 175,
	/* 176 reserved for Symbol */
	EID_QUIET_PERIOD_RESP = 177,
	/* 178-179 reserved for Symbol */
	/* 180 reserved for ISO/IEC 20011 */
	EID_EPAC_POLICY = 182,
	EID_CLISTER_TIME_OFF = 183,
	EID_INTER_AC_PRIO = 184,
	EID_SCS_DESCRIPTOR = 185,
	EID_QLOAD_REPORT = 186,
	EID_HCCA_TXOP_UPDATE_COUNT = 187,
	EID_HL_STREAM_ID = 188,
	EID_GCR_GROUP_ADDR = 189,
	EID_ANTENNA_SECTOR_ID_PATTERN = 190,
	EID_VHT_CAPABILITY = 191,
	EID_VHT_OPERATION = 192,
	EID_EXTENDED_BSS_LOAD = 193,
	EID_WIDE_BW_CHANNEL_SWITCH = 194,
	EID_TX_POWER_ENVELOPE = 195,
	EID_CHANNEL_SWITCH_WRAPPER = 196,
	EID_AID = 197,
	EID_QUIET_CHANNEL = 198,
	EID_OPMODE_NOTIF = 199,
	EID_REDUCED_NEIGHBOR_REPORT = 201,
	EID_AID_REQUEST = 210,
	EID_AID_RESPONSE = 211,
	EID_S1G_BCN_COMPAT = 213,
	EID_S1G_SHORT_BCN_INTERVAL = 214,
	EID_S1G_TWT = 216,
	EID_S1G_CAPABILITIES = 217,
	EID_VENDOR_SPECIFIC = 221,
	EID_QOS_PARAMETER = 222,
	EID_S1G_OPERATION = 232,
	EID_CAG_NUMBER = 237,
	EID_AP_CSN = 239,
	EID_FILS_INDICATION = 240,
	EID_DILS = 241,
	EID_FRAGMENT = 242,
	EID_RSN_EXTENSION = 244,
	EID_EXTENSION = 255
};

/* Element ID Extension */
enum wlan_eid_ext {
	EID_EXT_ASSOC_DELAY_INFO = 1,
	EID_EXT_FILS_REQ_PARAMS = 2,
	EID_EXT_FILS_KEY_CONFIRM = 3,
	EID_EXT_FILS_SESSION = 4,
	EID_EXT_FILS_HLP_CONTAINER = 5,
	EID_EXT_FILS_IP_ADDR_ASSIGN = 6,
	EID_EXT_KEY_DELIVERY = 7,
	EID_EXT_FILS_WRAPPED_DATA = 8,
	EID_EXT_FILS_PUBLIC_KEY = 12,
	EID_EXT_FILS_NONCE = 13,
	EID_EXT_FUTURE_CHAN_GUIDANCE = 14,
	EID_EXT_CDMG_EXTEND_SCHEDULE = 19,
	EID_EXT_SSW_REPORT = 20,
	EID_EXT_SPSH_REPORT = 25,
	EID_EXT_HE_CAPABILITY = 35,
	EID_EXT_HE_OPERATION = 36,
	EID_EXT_UORA = 37,
	EID_EXT_MU_EDCA_PARAM_SET = 38,
	EID_EXT_SPATIAL_REUSE_PARAM_SET = 39,
	EID_EXT_GAS_EXTENSION = 40,
	EID_EXT_NDP_FEEDBACK_REPORT_PARAM_SET = 41,
	EID_EXT_BSS_COLOR_CHG_ANN = 42,
	EID_EXT_QUIET_TIME_PERIOD_SETUP = 43,
	EID_EXT_ESS_REPORT = 45,
	EID_EXT_OPS = 46,
	EID_EXT_HE_BSS_LOAD = 47,
	EID_EXT_MAX_CHANNEL_SWITCH_TIME = 52,
	EID_EXT_MULTIPLE_BSSID_CONFIGURATION = 55,
	EID_EXT_NON_INHERITANCE = 56,
	EID_EXT_KNOWN_BSSID = 57,
	EID_EXT_SHORT_SSID_LIST = 58,
	EID_EXT_HE_6GHZ_CAPABILITY = 59,
	EID_EXT_UL_MU_POWER_CAPABILITY = 60,
	EID_EXT_EHT_OPERATION = 106,
	EID_EXT_MULTI_LINK = 107,
	EID_EXT_EHT_CAPABILITY = 108,
	EID_EXT_TID_TO_LINK_MAPPING = 109,
	EID_EXT_MULTI_LINK_TRAFFIC = 110,
};

/* Subelement ID for ML */
#define WLAN_SUBEID_PER_STA_PROFILE  0

/*****************************************************************************
 * Multi-Link (ML) Element
 *****************************************************************************/

#define STA_CTRL_LEN 2
#define MAX_STA_INFO_LEN 13

enum ml_ele_type {
	BASIC_ML = 0,
	PROBE_REQUEST_ML = 1,
	RESERVED
};

struct mld_cap {
	u16 max_num_sl:4;
	u16 srs_support:1;
	u16 tid_to_link_nego_support:2;
	u16 freq_sep_for_str:5;
	u16 aar_support:1;
	u16 rsvd:3;
};

struct basic_ml {
	bool link_id_info_present;
	bool bss_param_chg_cnt_present;
	bool msd_info_present;
	bool eml_cap_present;
	bool mld_cap_present;
	u8 *mld_address;
	u8 link_id;
	u8 bss_param_chg_cnt;
	/* TODO: msd info */
	/* TODO: eml cap */
	struct mld_cap mld_cap;
};

struct probe_req_ml {
	bool mld_id_present;
	u8 mld_id;
};

struct rtw_phl_per_sta_profile_element {
	/* sta ctrl */
	u8 link_id;
	bool complete_profile;
	bool mac_addr_present;
	bool bcn_interval_present;
	bool dtim_info_present;
	bool nstr_link_pair_present;
	u8 nstr_bitmap_size;
	/* sta info */
	u8 mac_addr[MAC_ALEN];
	u16 bcn_interval;
	u8 dtim_cnt;
	u8 dtim_period;
	u16 nstr_indication_bitmap;
	/* sta profile */
	u8 *sta_profile;
	u8 sta_profile_len;
	u8 *sta_profile_frag;
	u8 sta_profile_frag_len;
};

struct rtw_phl_ml_element {
	enum ml_ele_type type;
	union {
		struct basic_ml basic_ml;
		struct probe_req_ml probe_req_ml;
	} common_info;
	struct rtw_phl_per_sta_profile_element profile[MAX_MLD_LINK_NUM];
	u8 profile_num;
};

/*****************************************************************************
 * Reduced Neighbor Report Element
 *****************************************************************************/
#define MAX_TBTT_INFO_NUM 3
#define MAX_NEIGHBOR_AP_NUM 3

struct rtw_phl_tbtt_info {
	u8 offset;
	u8 bssid[MAC_ALEN];
	u8 short_ssid[4];
	struct bss_param_t {
		u8 oct_recomm:1,
		   same_ssid:1,
		   multi_bssid:1,
		   transmitted_bssid:1,
		   mem_24G_5G_colated_ap:1,
		   unsolicited_probe_resp_act:1,
		   colated_ap:1,
		   resv:1;
	} bss_param;
	u8 max_tx_pwr;
	struct mld_param_t {
		u8 mld_id;
		u8 link_id;
		u8 bss_params_chg_cnt;
	} mld_param;
};

struct tbtt_info_header {
	u8 type;
	u8 cnt;
	u8 len;
	bool filtered_nb_ap;
	/* self-defined variable*/
	u8 is_legal;
	u8 bssid_is_present:1,
	   short_ssid_is_present:1,
	   bss_param_is_present:1,
	   max_tx_pwr_is_present:1,
	   mld_param_is_present:1;
};

struct rtw_phl_neighbor_ap {
	struct tbtt_info_header tbtt_info_hdr;
	u8 op_class;
	u8 ch;
	struct rtw_phl_tbtt_info tbtt_infos[MAX_TBTT_INFO_NUM];
	/* self-defined variable*/
	struct rtw_chan_def chan_def;
};

struct rtw_phl_rnb_rpt_element {
	struct rtw_phl_neighbor_ap nb_aps[MAX_NEIGHBOR_AP_NUM];
	u8 nb_ap_num;
};

/******************************************************************************
 * WLAN Element APIs
 *****************************************************************************/
/*
  rtw_phl_build_ml_ie(): API for building ML element
  @ info: information that needs to build a ML element
  @ pbuf: buffer for the element
  @ return: length of the element
*/
u8 rtw_phl_build_ml_ie(struct rtw_phl_com_t *phl_com,
                       struct rtw_phl_ml_ie_info *info,
                       u8 *pbuf);

/*
  rtw_phl_build_per_sta_profile(): API for building Per-STA Profile
  @ info: the information that needs to build a Per-STA Profile
  @ pbuf: the buffer for the Per-STA Profile
  @ return: the length of the element
*/
u8 rtw_phl_build_per_sta_profile(struct rtw_phl_com_t *phl_com,
                                 struct rtw_phl_per_sta_profile_info *info,
                                 u8 *pbuf);

/*
  rtw_phl_parse_tid2link(): Parse Tid-To-Link Mapping element
  @ ele_pos: start of tid-to-link mapping element
  @ ele_len: length of tid-to-link mapping element
*/
void rtw_phl_parse_tid2link(struct rtw_phl_stainfo_t *sta,
                            u8 *ele_start,
                            u16 ele_len);

void rtw_phl_tid2link_not_present(struct rtw_phl_stainfo_t *sta, u8 nego);

u8 rtw_phl_build_tid2link(struct rtw_wifi_role_link_t *rlink,
                          u8 *ele_start);

void
rtw_phl_parse_ml_ie(struct rtw_phl_com_t *phl_com,
                    u8 *ele_pos,
                    u16 ele_len,
                    struct rtw_phl_ml_element *ml_ele
);

u16
rtw_phl_get_ie(u8 *ie_start,
               u16 ies_len,
               u8 target_id,
               u8 target_ext_id,
               u8 **out_ele
);

bool
rtw_phl_is_ie_fragmentable(u32 eid,
                           u32 eid_ext
);

u8 rtw_phl_build_reduced_nb_rpt(struct rtw_wifi_role_t *wrole,
                                struct rtw_wifi_role_link_t *rlink,
                                u8 *pbuf);

/*
  rtw_phl_parse_reduced_nb_rpt(): Parse Reduced Neighbor Report element
  @ ele_start: start of reduced neighbor report element
  @ ele_len: length of reduced neighbor report element
  @ rnr: the reduced neighbor report structure
*/
void
rtw_phl_parse_reduced_nb_rpt(struct rtw_phl_com_t *phl_com,
                             u8 *ele_start,
                             u16 ele_len,
                             struct rtw_phl_rnb_rpt_element *reduced_nb_rpt);

/************************************************************
 * SET ML Control field: _pStart = start of ML Control field
 ************************************************************/
/* Basic ML */
#define SET_ML_ELE_ML_CTRL_TYPE(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 3, _val)
#define SET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 4, 1, _val)
#define SET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 5, 1, _val)
#define SET_ML_ELE_ML_CTRL_MEDIUM_SYNC_DELAY_INFO_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 6, 1, _val)
#define SET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 7, 1, _val)
#define SET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 8, 1, _val)
/* Probe Request ML */
#define SET_ML_ELE_ML_CTRL_MLD_ID_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 4, 1, _val)

/************************************************************
 GET ML Control field: _pStart = start of ML Control field
 ************************************************************/
/* Basic ML */
#define GET_ML_ELE_ML_CTRL_TYPE(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 3)
#define GET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 4, 1)
#define GET_ML_ELE_ML_CTRL_BSS_PARAMS_CHG_CNT_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 5, 1)
#define GET_ML_ELE_ML_CTRL_MEDIUM_SYNC_DELAY_INFO_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 6, 1)
#define GET_ML_ELE_ML_CTRL_EML_CAP_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 7, 1)
#define GET_ML_ELE_ML_CTRL_MLD_CAP_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 8, 1)
/* Probe Request ML */
#define GET_ML_ELE_ML_CTRL_MLD_ID_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 4, 1)

/************************************************************
 SET Common Info field: _pStart = start of each subfield
 ************************************************************/
/* Basic ML */
#define SET_ML_ELE_COMMON_INFO_LINK_ID(_pStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pStart, 0, 4, _val)
#define SET_ML_ELE_COMMON_INFO_BSS_PARAMS_CHG_CNT(_pStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pStart, 0, 8, _val)
/* Probe Request ML */
#define SET_ML_ELE_COMMON_INFO_MLD_ID(_pStart, _val) \
	SET_BITS_TO_LE_1BYTE(_pStart, 0, 8, _val)

/************************************************************
 GET MLD Capabilities subfield: _pStart = start of MLD Capabilities subfield
 ************************************************************/
#define GET_MLD_CAP_MAX_NUM_OF_SL(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 4)
#define GET_MLD_CAP_SRS_SUPPORT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 4, 1)
#define GET_MLD_CAP_TID_TO_LINK_NEGO_SUPPORT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 5, 2)
#define GET_MLD_CAP_FREQ_SEP_FOR_STR(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 7, 5)
#define GET_MLD_CAP_AAR_SUPPORT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 12, 1)

/************************************************************
 SET MLD Capabilities subfield: _pStart = start of MLD Capabilities subfield
 ************************************************************/
#define SET_MLD_CAP_MAX_NUM_OF_SL(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 4, _val)
#define SET_MLD_CAP_SRS_SUPPORT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 4, 1, _val)
#define SET_MLD_CAP_TID_TO_LINK_NEGO_SUPPORT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 5, 2, _val)
#define SET_MLD_CAP_FREQ_SEP_FOR_STR(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 7, 5, _val)
#define SET_MLD_CAP_AAR_SUPPORT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 12, 1, _val)

/************************************************************
 SET STA Control: _pStart = start of STA Control field
 ************************************************************/
#define SET_STA_CTRL_LINK_ID(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 4, _val)
#define SET_STA_CTRL_COMPLETE_PROFILE(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 4, 1, _val)
#define SET_STA_CTRL_MAC_ADDR_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 5, 1, _val)
#define SET_STA_CTRL_BEACON_INTEREVAL_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 6, 1, _val)
#define SET_STA_CTRL_DTIM_INFO_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 7, 1, _val)
#define SET_STA_CTRL_NSTR_LINK_PAIR_PRESENT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 8, 1, _val)
#define SET_STA_CTRL_NSTR_BITMAP_SIZE(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 9, 1, _val)

/************************************************************
 GET STA Control: _pStart = start of STA Control field
 ************************************************************/
#define GET_STA_CTRL_LINK_ID(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 4)
#define GET_STA_CTRL_COMPLETE_PROFILE(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 4, 1)
#define GET_STA_CTRL_MAC_ADDR_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 5, 1)
#define GET_STA_CTRL_BEACON_INTEREVAL_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 6, 1)
#define GET_STA_CTRL_DTIM_INFO_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 7, 1)
#define GET_STA_CTRL_NSTR_LINK_PAIR_PRESENT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 8, 1)
#define GET_STA_CTRL_NSTR_BITMAP_SIZE(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 9, 1)



/************************************************************
 SET TID-To-Link Mapping element: _pStart = start of tid2link control field
 ************************************************************/
#define SET_TID2LINK_CTRL_DIRECT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 2, _val)
#define SET_TID2LINK_CTRL_DEFAULT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 2, 1, _val)
#define SET_TID2LINK_CTRL_INDIC(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 8, 8, _val)

/* SET Link Mapping of TID n field: _pStart = start of Link Mapping of TID field */
#define SET_LINK_MAPPING_TID(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 16, _val)

/************************************************************
 GET TID-To-Link Mapping element: _pStart = start of tid2link control field
 ************************************************************/
#define GET_TID2LINK_CTRL_DIRECT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 2)
#define GET_TID2LINK_CTRL_DEFAULT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 2, 1)
#define GET_TID2LINK_CTRL_INDIC(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 8, 8)

/* GET Link Mapping of TID n field: _pStart = start of Link Mapping of TID field */
#define GET_LINK_MAPPING_TID(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 16)


/************************************************************
 Reduced Neighbor Report
 SET TBTT Info Header subfield: _pStart = start of TBTT Info Header subfield
 ************************************************************/
#define SET_TBTT_INFO_FIELD_TYPE(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 0, 2, _val)
#define SET_FILTED_NB_AP(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 2, 1, _val)
#define SET_TBTT_INFO_CNT(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 4, 4, _val)
#define SET_TBTT_INFO_LEN(_pStart, _val) \
	SET_BITS_TO_LE_2BYTE(_pStart, 8, 8, _val)

/************************************************************
 Reduced Neighbor Report
 SET MLD Parameters: _pStart = start of MLD Parameters subfield
 ************************************************************/
#define SET_MLD_PARAMS_MLD_ID(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE(_pStart, 0, 8, _val)
#define SET_MLD_PARAMS_LINK_ID(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE(_pStart, 8, 4, _val)
#define SET_MLD_PARAMS_BSS_PARAMS_CHG_CNT(_pStart, _val) \
	SET_BITS_TO_LE_4BYTE(_pStart, 12, 8, _val)

/************************************************************
 Reduced Neighbor Report
 GET TBTT Info Header subfield: _pStart = start of TBTT Info Header subfield
 ************************************************************/
#define GET_TBTT_INFO_FIELD_TYPE(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 0, 2)
#define GET_FILTED_NB_AP(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 2, 1)
#define GET_TBTT_INFO_CNT(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 4, 4)
#define GET_TBTT_INFO_LEN(_pStart) \
	LE_BITS_TO_2BYTE(_pStart, 8, 8)

/************************************************************
 Reduced Neighbor Report
 GET TBTT Info BSS Parameters subfield: _pStart = start of TBTT BSS Parameters subfield
 ************************************************************/
#define GET_BSS_PARAMS_OCT_RECOMM(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 0, 1)
#define GET_BSS_PARAMS_SAME_SSID(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 1, 1)
#define GET_BSS_PARAMS_MULTI_BSSID(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 2, 1)
#define GET_BSS_PARAMS_TRANSMITTED_BSSID(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 3, 1)
#define GET_BSS_PARAMS_MEM_24G_5G_COLOCATED_AP(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 4, 1)
#define GET_BSS_PARAMS_UNSOLICITED_PROBE_RESP_ACTIVE(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 5, 1)
#define GET_BSS_PARAMS_COLOACTED_AP(_pEleStart) \
	LE_BITS_TO_1BYTE(_pEleStart, 6, 1)

/************************************************************
 Reduced Neighbor Report
 GET MLD Parameters: _pStart = start of MLD Parameters subfield
 ************************************************************/
#define GET_MLD_PARAMS_MLD_ID(_pStart) \
	LE_BITS_TO_4BYTE(_pStart, 0, 8)
#define GET_MLD_PARAMS_LINK_ID(_pStart) \
	LE_BITS_TO_4BYTE(_pStart, 8, 4)
#define GET_MLD_PARAMS_BSS_PARAMS_CHG_CNT(_pStart) \
	LE_BITS_TO_4BYTE(_pStart, 12, 8)

#endif  /*_PHL_STA_H_*/