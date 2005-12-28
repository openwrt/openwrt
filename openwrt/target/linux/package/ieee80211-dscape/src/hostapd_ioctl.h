/*
 * Host AP (software wireless LAN access point) user space daemon for
 * Host AP kernel driver
 * Copyright 2002-2003, Jouni Malinen <jkmaline@cc.hut.fi>
 * Copyright 2002-2004, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef HOSTAPD_IOCTL_H
#define HOSTAPD_IOCTL_H

#ifndef __KERNEL__
#include "ieee80211_shared.h"
#endif /* __KERNEL__ */

#define PRISM2_IOCTL_PRISM2_PARAM (SIOCIWFIRSTPRIV + 0)
#define PRISM2_IOCTL_GET_PRISM2_PARAM (SIOCIWFIRSTPRIV + 1)
#define PRISM2_IOCTL_HOSTAPD (SIOCIWFIRSTPRIV + 3)
#define PRISM2_IOCTL_TEST_PARAM (SIOCIWFIRSTPRIV + 4)

/* PRISM2_IOCTL_PRISM2_PARAM ioctl() subtypes: */
enum {
	PRISM2_PARAM_PTYPE = 1,
	PRISM2_PARAM_TXRATECTRL = 2,
	PRISM2_PARAM_BEACON_INT = 3,
	PRISM2_PARAM_PSEUDO_IBSS = 4,
	PRISM2_PARAM_ALC = 5,
	PRISM2_PARAM_TXPOWER = 6,
	PRISM2_PARAM_DUMP = 7,
	PRISM2_PARAM_OTHER_AP_POLICY = 8,
	PRISM2_PARAM_AP_MAX_INACTIVITY = 9,
	PRISM2_PARAM_AP_BRIDGE_PACKETS = 10,
	PRISM2_PARAM_DTIM_PERIOD = 11,
	PRISM2_PARAM_AP_NULLFUNC_ACK = 12,
	PRISM2_PARAM_MAX_WDS = 13,
	PRISM2_PARAM_AP_AUTOM_AP_WDS = 14,
	PRISM2_PARAM_AP_AUTH_ALGS = 15,
	PRISM2_PARAM_MONITOR_ALLOW_FCSERR = 16,
	PRISM2_PARAM_HOST_ENCRYPT = 17,
	PRISM2_PARAM_HOST_DECRYPT = 18,
	PRISM2_PARAM_BUS_MASTER_THRESHOLD_RX = 19,
	PRISM2_PARAM_BUS_MASTER_THRESHOLD_TX = 20,
	PRISM2_PARAM_HOST_ROAMING = 21,
	PRISM2_PARAM_BCRX_STA_KEY = 22,
	PRISM2_PARAM_IEEE_802_1X = 23,
	PRISM2_PARAM_ANTSEL_TX = 24,
	PRISM2_PARAM_ANTSEL_RX = 25,
	PRISM2_PARAM_MONITOR_TYPE = 26,
	PRISM2_PARAM_WDS_TYPE = 27,
	PRISM2_PARAM_HOSTSCAN = 28,
	PRISM2_PARAM_AP_SCAN = 29,

	/* Instant802 additions */
	PRISM2_PARAM_CTS_PROTECT_ERP_FRAMES = 1001,
	PRISM2_PARAM_DROP_UNENCRYPTED = 1002,
	PRISM2_PARAM_PREAMBLE = 1003,
	PRISM2_PARAM_RATE_LIMIT = 1004,
	PRISM2_PARAM_RATE_LIMIT_BURST = 1005,
	PRISM2_PARAM_SHORT_SLOT_TIME = 1006,
	PRISM2_PARAM_TEST_MODE = 1007,
	PRISM2_PARAM_NEXT_MODE = 1008,
	PRISM2_PARAM_CLEAR_KEYS = 1009,
	PRISM2_PARAM_ADM_STATUS = 1010,
	PRISM2_PARAM_ANTENNA_SEL = 1011,
	PRISM2_PARAM_CALIB_INT = 1012,
        PRISM2_PARAM_ANTENNA_MODE = 1013,
	PRISM2_PARAM_PRIVACY_INVOKED = 1014,
	PRISM2_PARAM_BROADCAST_SSID = 1015,
        PRISM2_PARAM_STAT_TIME = 1016,
	PRISM2_PARAM_STA_ANTENNA_SEL = 1017,
	PRISM2_PARAM_FORCE_UNICAST_RATE = 1018,
	PRISM2_PARAM_RATE_CTRL_NUM_UP = 1019,
	PRISM2_PARAM_RATE_CTRL_NUM_DOWN = 1020,
	PRISM2_PARAM_MAX_RATECTRL_RATE = 1021,
	PRISM2_PARAM_TX_POWER_REDUCTION = 1022,
	PRISM2_PARAM_EAPOL = 1023,
	PRISM2_PARAM_KEY_TX_RX_THRESHOLD = 1024,
	PRISM2_PARAM_KEY_INDEX = 1025,
	PRISM2_PARAM_DEFAULT_WEP_ONLY = 1026,
	PRISM2_PARAM_WIFI_WME_NOACK_TEST = 1033,
	PRISM2_PARAM_ALLOW_BROADCAST_ALWAYS = 1034,
	PRISM2_PARAM_SCAN_FLAGS = 1035,
	PRISM2_PARAM_HW_MODES = 1036,
	PRISM2_PARAM_CREATE_IBSS = 1037,
	PRISM2_PARAM_WMM_ENABLED = 1038,
	PRISM2_PARAM_MIXED_CELL = 1039,
	PRISM2_PARAM_KEY_MGMT = 1040,
	PRISM2_PARAM_RADAR_DETECT = 1043,
	PRISM2_PARAM_SPECTRUM_MGMT = 1044,
	/* NOTE: Please try to coordinate with other active development
	 * branches before allocating new param numbers so that each new param
	 * will be unique within all branches and the allocated number will not
	 * need to be changed when merging new features. Existing numbers in
	 * the mainline (or main devel branch) must not be changed when merging
	 * in new features. */
};

/* PRISM2_IOCTL_HOSTAPD ioctl() cmd: */
enum {
	PRISM2_HOSTAPD_FLUSH = 1,
	PRISM2_HOSTAPD_ADD_STA = 2,
	PRISM2_HOSTAPD_REMOVE_STA = 3,
	PRISM2_HOSTAPD_GET_INFO_STA = 4,
	/* REMOVED: PRISM2_HOSTAPD_RESET_TXEXC_STA = 5, */
	PRISM2_SET_ENCRYPTION = 6,
	PRISM2_GET_ENCRYPTION = 7,
	PRISM2_HOSTAPD_SET_FLAGS_STA = 8,
	PRISM2_HOSTAPD_GET_RID = 9,
	PRISM2_HOSTAPD_SET_RID = 10,
	PRISM2_HOSTAPD_SET_ASSOC_AP_ADDR = 11,
	PRISM2_HOSTAPD_MLME = 13,

	/* Instant802 additions */
	PRISM2_HOSTAPD_SET_BEACON = 1001,
        PRISM2_HOSTAPD_GET_HW_FEATURES = 1002,
        PRISM2_HOSTAPD_SCAN = 1003,
	PRISM2_HOSTAPD_WPA_TRIGGER = 1004,
	PRISM2_HOSTAPD_SET_RATE_SETS = 1005,
        PRISM2_HOSTAPD_ADD_IF = 1006,
        PRISM2_HOSTAPD_REMOVE_IF = 1007,
        PRISM2_HOSTAPD_GET_DOT11COUNTERSTABLE = 1008,
        PRISM2_HOSTAPD_GET_LOAD_STATS = 1009,
        PRISM2_HOSTAPD_SET_STA_VLAN = 1010,
	PRISM2_HOSTAPD_SET_GENERIC_INFO_ELEM = 1011,
	PRISM2_HOSTAPD_SET_CHANNEL_FLAG = 1012,
	PRISM2_HOSTAPD_SET_REGULATORY_DOMAIN = 1013,
	PRISM2_HOSTAPD_SET_TX_QUEUE_PARAMS = 1014,
	PRISM2_HOSTAPD_SET_BSS = 1015,
	PRISM2_HOSTAPD_GET_TX_STATS = 1016,
	PRISM2_HOSTAPD_UPDATE_IF = 1017,
	PRISM2_HOSTAPD_SCAN_REQ = 1019,
	PRISM2_STA_GET_STATE = 1020,
	PRISM2_HOSTAPD_FLUSH_IFS = 1021,
	PRISM2_HOSTAPD_SET_RADAR_PARAMS = 1023,
	PRISM2_HOSTAPD_SET_QUIET_PARAMS = 1024,
	PRISM2_HOSTAPD_GET_TX_POWER = 1025,
	/* NOTE: Please try to coordinate with other active development
	 * branches before allocating new param numbers so that each new param
	 * will be unique within all branches and the allocated number will not
	 * need to be changed when merging new features. Existing numbers in
	 * the mainline (or main devel branch) must not be changed when merging
	 * in new features. */
};

	/* these definitions mirror the ieee80211_i.h
	 * IEEE80211_DISABLED, ... IEEE80211_ASSOCIATED enumeration */
enum {
	PRISM2_PARAM_STA_DISABLED,
	PRISM2_PARAM_STA_AUTHENTICATE,
	PRISM2_PARAM_STA_ASSOCIATE,
	PRISM2_PARAM_STA_ASSOCIATED,
};

#define PRISM2_HOSTAPD_MAX_BUF_SIZE 2048
#define HOSTAP_CRYPT_ALG_NAME_LEN 16

/* Use this to make sure that structure elements are correctly aligned
 * for access as other types. Most commonly, this affects the placeholder
 * types used for data at the end of a structure in this union.
 */
#ifdef __GNUC__
#undef	ALIGNED
#define ALIGNED __attribute__ ((aligned))
#else
/* Check if it has been defined elsewhere */
#ifndef ALIGNED
#error "Must define ALIGNED to generate aligned structure elements"
#endif
#endif

struct prism2_hostapd_param {
	u32 cmd;
	u8 sta_addr[ETH_ALEN];
	u8 pad[2];
	union {
		struct {
			u16 aid;
			u16 capability;
			u8 supp_rates[32];
			/* atheros_super_ag and enc_flags are only used with
			 * IEEE80211_ATHEROS_SUPER_AG 
			 */
			u8 atheros_super_ag;
			u8 atheros_xr_mode;
			u8 wds_flags;
#define IEEE80211_STA_DYNAMIC_ENC BIT(0)
			u8 enc_flags;
		} add_sta;
		struct {
			u32 inactive_msec;
			u32 rx_packets;
			u32 tx_packets;
			u32 rx_bytes;
			u32 tx_bytes;
			u32 current_tx_rate; /* in 100 kbps */
                        u32 channel_use;
                        u32 flags;
			u32 num_ps_buf_frames;
			u32 tx_retry_failed;
			u32 tx_retry_count;
			u32 last_rssi;
			u32 last_ack_rssi;
		} get_info_sta;
		struct {
			u8 alg[HOSTAP_CRYPT_ALG_NAME_LEN];
			u32 flags;
			u32 err;
			u8 idx;
#define HOSTAP_SEQ_COUNTER_SIZE 8
			u8 seq_counter[HOSTAP_SEQ_COUNTER_SIZE];
			u16 key_len;
			u8 key[0] ALIGNED;
		} crypt;
		struct {
			u32 flags_and;
			u32 flags_or;
		} set_flags_sta;
		struct {
			u16 rid;
			u16 len;
			u8 data[0] ALIGNED;
		} rid;
		struct {
			u16 head_len;
			u16 tail_len;
			u8 data[0] ALIGNED; /* head_len + tail_len bytes */
		} beacon;
		struct {
			u16 num_modes;
			u16 flags;
			u8 data[0] ALIGNED; /* num_modes * feature data */
                } hw_features;
                struct {
                        u8  now;
                        s8  our_mode_only;
                        s16 last_rx;
                        u16 channel;
                        s16 interval; /* seconds */
                        s32 listen;   /* microseconds */
                } scan;
		struct {
#define WPA_TRIGGER_FAIL_TX_MIC BIT(0)
#define WPA_TRIGGER_FAIL_TX_ICV BIT(1)
#define WPA_TRIGGER_FAIL_RX_MIC BIT(2)
#define WPA_TRIGGER_FAIL_RX_ICV BIT(3)
#define WPA_TRIGGER_TX_REPLAY BIT(4)
#define WPA_TRIGGER_TX_REPLAY_FRAG BIT(5)
#define WPA_TRIGGER_TX_SKIP_SEQ BIT(6)
			u32 trigger;
		} wpa_trigger;
		struct {
			u16 mode; /* MODE_* */
			u16 num_supported_rates;
			u16 num_basic_rates;
			u8 data[0] ALIGNED; /* num_supported_rates * u16 +
				             * num_basic_rates * u16 */
                } set_rate_sets;
                struct {
			u8 type; /* WDS, VLAN, etc */
			u8 name[IFNAMSIZ];
                        u8 data[0] ALIGNED;
                } if_info;
                struct dot11_counters {
                        u32 dot11TransmittedFragmentCount;
                        u32 dot11MulticastTransmittedFrameCount;
                        u32 dot11FailedCount;
                        u32 dot11ReceivedFragmentCount;
                        u32 dot11MulticastReceivedFrameCount;
                        u32 dot11FCSErrorCount;
                        u32 dot11TransmittedFrameCount;
                        u32 dot11WEPUndecryptableCount;
			u32 dot11ACKFailureCount;
			u32 dot11RTSFailureCount;
			u32 dot11RTSSuccessCount;
                } dot11CountersTable;
		struct {
#define LOAD_STATS_CLEAR BIT(1)
			u32 flags;
			u32 channel_use;
                } get_load_stats;
                struct {
                        char vlan_name[IFNAMSIZ];
			int vlan_id;
                } set_sta_vlan;
		struct {
			u8 len;
			u8 data[0] ALIGNED;
		} set_generic_info_elem;
		struct {
			u16 mode; /* MODE_* */
			u16 chan;
			u32 flag;
                        u8  power_level; /* regulatory limit in dBm */
                        u8  antenna_max;
		} set_channel_flag;
                struct {
                        u32 rd;
                } set_regulatory_domain;
		struct {
			u32 queue;
			s32 aifs;
			u32 cw_min;
			u32 cw_max;
			u32 burst_time; /* maximum burst time in 0.1 ms, i.e.,
					 * 10 = 1 ms */
		} tx_queue_params;
		struct {
			u32 bss_count;
			u8 bssid_mask[ETH_ALEN];
		} set_bss;
		struct ieee80211_tx_stats {
			struct {
				unsigned int len; /* num packets in queue */
				unsigned int limit; /* queue len (soft) limit
						     */
				unsigned int count; /* total num frames sent */
			} data[4];
		} get_tx_stats;
		struct {
			u8 ssid_len;
			u8 ssid[0] ALIGNED;
		} scan_req;
		struct {
			u32 state;
		} sta_get_state;
		struct {
#define MLME_STA_DEAUTH 0
#define MLME_STA_DISASSOC 1
			u16 cmd;
			u16 reason_code;
		} mlme;
		struct {
			unsigned int value;
		/* 	TODO
			int pulse_width;
			int num_pulse;
			int period;
		*/
		}radar;
		struct {
			unsigned int period;
			unsigned int offset;
			unsigned int duration;
		}quiet;
		struct {
			unsigned int tx_power_min;
			unsigned int tx_power_max;
		}tx_power;
		struct {
			u8 dummy[80]; /* Make sizeof() this struct large enough
				       * with some compiler versions. */
		} dummy;
	} u;
};


#ifndef IEEE80211_TX_QUEUE_NUMS
#define IEEE80211_TX_QUEUE_NUMS
/* TODO: these need to be synchronized with ieee80211.h; make a shared header
 * file that can be included into low-level drivers, 80211.o, and hostapd */
/* tx_queue_params - queue */
enum {
	IEEE80211_TX_QUEUE_DATA0 = 0, /* used for EDCA AC_VO data */
	IEEE80211_TX_QUEUE_DATA1 = 1, /* used for EDCA AC_VI data */
	IEEE80211_TX_QUEUE_DATA2 = 2, /* used for EDCA AC_BE data */
	IEEE80211_TX_QUEUE_DATA3 = 3, /* used for EDCA AC_BK data */
	IEEE80211_TX_QUEUE_DATA4 = 4,
	IEEE80211_TX_QUEUE_AFTER_BEACON = 6,
	IEEE80211_TX_QUEUE_BEACON = 7
};
#endif /* IEEE80211_TX_QUEUE_NUMS */


#define HOSTAP_CRYPT_FLAG_SET_TX_KEY BIT(0)
#define HOSTAP_CRYPT_FLAG_PERMANENT BIT(1)

#define HOSTAP_CRYPT_ERR_UNKNOWN_ALG 2
#define HOSTAP_CRYPT_ERR_UNKNOWN_ADDR 3
#define HOSTAP_CRYPT_ERR_CRYPT_INIT_FAILED 4
#define HOSTAP_CRYPT_ERR_KEY_SET_FAILED 5
#define HOSTAP_CRYPT_ERR_TX_KEY_SET_FAILED 6
#define HOSTAP_CRYPT_ERR_CARD_CONF_FAILED 7

#define HOSTAP_HW_FLAG_NULLFUNC_OK BIT(0)

enum {
	IEEE80211_KEY_MGMT_NONE = 0,
	IEEE80211_KEY_MGMT_IEEE8021X = 1,
	IEEE80211_KEY_MGMT_WPA_PSK = 2,
	IEEE80211_KEY_MGMT_WPA_EAP = 3,
};


/* Data structures used for get_hw_features ioctl */
struct hostapd_ioctl_hw_modes_hdr {
	int mode;
	int num_channels;
	int num_rates;
};

struct ieee80211_channel_data {
	short chan; /* channel number (IEEE 802.11) */
	short freq; /* frequency in MHz */
	int flag; /* flag for hostapd use (IEEE80211_CHAN_*) */
};

struct ieee80211_rate_data {
	int rate; /* rate in 100 kbps */
	int flags; /* IEEE80211_RATE_ flags */
};


/* ADD_IF, REMOVE_IF, and UPDATE_IF 'type' argument */
enum {
	HOSTAP_IF_WDS = 1, HOSTAP_IF_VLAN = 2, HOSTAP_IF_BSS = 3,
	HOSTAP_IF_STA = 4
};

struct hostapd_if_wds {
        u8 remote_addr[ETH_ALEN];
};

struct hostapd_if_vlan {
        u8 id;
};

struct hostapd_if_bss {
        u8 bssid[ETH_ALEN];
};

struct hostapd_if_sta {
};

#endif /* HOSTAPD_IOCTL_H */
