/*
 * Custom OID/ioctl definitions for
 * Broadcom 802.11abg Networking Device Driver
 *
 * Definitions subject to change without notice.
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _wlioctl_h_
#define	_wlioctl_h_

#include <typedefs.h>
#include <proto/ethernet.h>
#include <proto/802.11.h>

#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#define	PACKED
#endif

#define WLC_ESSID_MAX_SIZE	32

#define WL_NUMRATES		255	/* max # of rates in a rateset */

typedef struct wl_rateset {
	uint32	count;			/* # rates in this set */
	uint8	rates[WL_NUMRATES];	/* rates in 500kbps units w/hi bit set if basic */
} wl_rateset_t;

#define WL_CHANSPEC_CHAN_MASK	0x0fff
#define WL_CHANSPEC_BAND_MASK	0xf000
#define WL_CHANSPEC_BAND_SHIFT	12
#define WL_CHANSPEC_BAND_A	0x1000
#define WL_CHANSPEC_BAND_B	0x2000

/*
 * Per-bss information structure.
 */

#define	WL_LEGACY_BSS_INFO_VERSION	106	/* an older supported version of wl_bss_info struct */
#define	WL_BSS_INFO_VERSION		107	/* current version of wl_bss_info struct */

typedef struct wl_bss_info106 {
	uint		version;	/* version field */
	struct ether_addr BSSID;
	uint8		SSID_len;
	uint8		SSID[32];
	uint8		Privacy;	/* 0=No WEP, 1=Use WEP */
	int16		RSSI;		/* receive signal strength (in dBm) */
	uint16		beacon_period;	/* units are Kusec */
	uint16		atim_window;	/* units are Kusec */
	uint8		channel;	/* Channel no. */
	int8		infra;		/* 0=IBSS, 1=infrastructure, 2=unknown */
	struct {
		uint	count;		/* # rates in this set */
		uint8	rates[12];	/* rates in 500kbps units w/hi bit set if basic */
	} rateset;			/* supported rates */
        uint8           dtim_period;    /* DTIM period */
	int8		phy_noise;	/* noise right after tx (in dBm) */
	uint16		capability;	/* Capability information */
	struct dot11_bcn_prb *prb;	/* probe response frame (ioctl na) */
	uint16		prb_len;	/* probe response frame length (ioctl na) */
	struct {
		uint8 supported;	/* wpa supported */
		uint8 multicast;	/* multicast cipher */
		uint8 ucount;		/* count of unicast ciphers */
		uint8 unicast[4];	/* unicast ciphers */
		uint8 acount;		/* count of auth modes */
		uint8 auth[4];		/* Authentication modes */
	} wpa;
} wl_bss_info106_t;

typedef struct wl_bss_info {
	uint32		version;	/* version field */
	uint32		length;		/* byte length of data in this record, starting at version and including IEs */
	struct ether_addr BSSID;
	uint16		beacon_period;	/* units are Kusec */
	uint16		capability;	/* Capability information */
	uint8		SSID_len;
	uint8		SSID[32];
	struct {
		uint	count;		/* # rates in this set */
		uint8	rates[16];	/* rates in 500kbps units w/hi bit set if basic */
	} rateset;			/* supported rates */
	uint8		channel;	/* Channel no. */
	uint16		atim_window;	/* units are Kusec */
        uint8           dtim_period;    /* DTIM period */
	int16		RSSI;		/* receive signal strength (in dBm) */
	int8		phy_noise;	/* noise (in dBm) */
	uint32		ie_length;	/* byte length of Information Elements */
	/* variable length Information Elements */
} wl_bss_info_t;

typedef struct wlc_ssid {
	uint32		SSID_len;
	uchar		SSID[32];
} wlc_ssid_t;

typedef struct wl_scan_params {
	wlc_ssid_t ssid;	/* default is {0, ""} */
	struct ether_addr bssid;/* default is bcast */
	int8 bss_type;		/* default is any, DOT11_BSSTYPE_ANY/INFRASTRUCTURE/INDEPENDENT */
	int8 scan_type;		/* -1 use default, DOT11_SCANTYPE_ACTIVE/PASSIVE */
	int32 nprobes;		/* -1 use default, number of probes per channel */
	int32 active_time;	/* -1 use default, dwell time per channel for active scanning */
	int32 passive_time;	/* -1 use default, dwell time per channel for passive scanning */
	int32 home_time;	/* -1 use default, dwell time for the home channel between channel scans */
	int32 channel_num;	/* 0 use default (all available channels), count of channels in channel_list */
	uint16 channel_list[1];	/* list of chanspecs */
} wl_scan_params_t;
/* size of wl_scan_params not including variable length array */
#define WL_SCAN_PARAMS_FIXED_SIZE 64

typedef struct wl_scan_results {
	uint32 buflen;
	uint32 version;
	uint32 count;
	wl_bss_info_t bss_info[1];
} wl_scan_results_t;
/* size of wl_scan_results not including variable length array */
#define WL_SCAN_RESULTS_FIXED_SIZE 12

/* uint32 list */
typedef struct wl_uint32_list {
	/* in - # of elements, out - # of entries */
	uint32 count;
	/* variable length uint32 list */
	uint32 element[1];
} wl_uint32_list_t;

#define WLC_CNTRY_BUF_SZ        4       /* Country string is 3 bytes + NULL */

typedef struct wl_channels_in_country {
	uint32 buflen;
	uint32 band;
	char country_abbrev[WLC_CNTRY_BUF_SZ];
	uint32 count;
	uint32 channel[1];
} wl_channels_in_country_t;

typedef struct wl_country_list {
	uint32 buflen;
	uint32 band_set;
	uint32 band;
	uint32 count;
	char country_abbrev[1];
} wl_country_list_t;

#define WL_RM_TYPE_BASIC	1
#define WL_RM_TYPE_CCA		2
#define WL_RM_TYPE_RPI		3

#define WL_RM_FLAG_PARALLEL	(1<<0)

#define WL_RM_FLAG_LATE		(1<<1)
#define WL_RM_FLAG_INCAPABLE	(1<<2)
#define WL_RM_FLAG_REFUSED	(1<<3)

typedef struct wl_rm_req_elt {
	int8	type;
	int8	flags;
	uint16	chanspec;
	uint32	token;		/* token for this measurement */
	uint32	tsf_h;		/* TSF high 32-bits of Measurement start time */
	uint32	tsf_l;		/* TSF low 32-bits */
	uint32	dur;		/* TUs */
} wl_rm_req_elt_t;

typedef struct wl_rm_req {
	uint32	token;		/* overall measurement set token */
	uint32	count;		/* number of measurement reqests */
	wl_rm_req_elt_t	req[1];	/* variable length block of requests */
} wl_rm_req_t;
#define WL_RM_REQ_FIXED_LEN	8

typedef struct wl_rm_rep_elt {
	int8	type;
	int8	flags;
	uint16	chanspec;
	uint32	token;		/* token for this measurement */
	uint32	tsf_h;		/* TSF high 32-bits of Measurement start time */
	uint32	tsf_l;		/* TSF low 32-bits */
	uint32	dur;		/* TUs */
	uint32	len;		/* byte length of data block */
	uint8	data[1];	/* variable length data block */
} wl_rm_rep_elt_t;
#define WL_RM_REP_ELT_FIXED_LEN	24	/* length excluding data block */

#define WL_RPI_REP_BIN_NUM 8
typedef struct wl_rm_rpi_rep {
	uint8	rpi[WL_RPI_REP_BIN_NUM];
	int8	rpi_max[WL_RPI_REP_BIN_NUM];
} wl_rm_rpi_rep_t;

typedef struct wl_rm_rep {
	uint32	token;		/* overall measurement set token */
	uint32	len;		/* length of measurement report block */
	wl_rm_rep_elt_t	rep[1];	/* variable length block of reports */
} wl_rm_rep_t;
#define WL_RM_REP_FIXED_LEN	8


#define WLC_MAX_KEY_SIZE	32	/* max size of any key */
#define WLC_MAX_IV_SIZE		16	/* max size of any IV */
#define WLC_EXT_IV_FLAG		(1<<5)	/* flag to indicate IV is > 4 bytes */
#define WLC_MAX_DEFAULT_KEYS	4	/* # of default WEP keys */
#define WLC_MAX_KEYS		54	/* Max # of WEP keys */
#define WLC_WEP1_KEY_SIZE	5	/* max size of any WEP key */
#define WLC_WEP1_KEY_HEX_SIZE	10	/* size of WEP key in hex. */
#define WLC_WEP128_KEY_SIZE	13	/* max size of any WEP key */
#define WLC_WEP128_KEY_HEX_SIZE	26	/* size of WEP key in hex. */
#define WLC_TKIP_MIC_SIZE	8	/* size of TKIP MIC */
#define WLC_TKIP_EOM_SIZE	7	/* max size of TKIP EOM */
#define WLC_TKIP_EOM_FLAG	0x5a	/* TKIP EOM flag byte */
#define WLC_TKIP_KEY_SIZE	32	/* size of any TKIP key */
#define WLC_TKIP_MIC_AUTH_TX	16	/* offset to Authenticator MIC TX key */
#define WLC_TKIP_MIC_AUTH_RX	24	/* offset to Authenticator MIC RX key */
#define WLC_TKIP_MIC_SUP_RX	16	/* offset to Supplicant MIC RX key */
#define WLC_TKIP_MIC_SUP_TX	24	/* offset to Supplicant MIC TX key */
#define WLC_TKIP_P1_KEY_SIZE	10	/* size of TKHash Phase1 output, in bytes */
#define WLC_TKIP_P2_KEY_SIZE	16	/* size of TKHash Phase2 output */
#define WLC_AES_KEY_SIZE	16	/* size of AES key */


typedef enum sup_auth_status {
	WLC_SUP_DISCONNECTED = 0,
	WLC_SUP_CONNECTING,
	WLC_SUP_IDREQUIRED,
	WLC_SUP_AUTHENTICATING,
	WLC_SUP_AUTHENTICATED,
	WLC_SUP_KEYXCHANGE,
	WLC_SUP_KEYED
} sup_auth_status_t;

/* Enumerate crypto algorithms */
#define	CRYPTO_ALGO_OFF			0
#define	CRYPTO_ALGO_WEP1		1
#define	CRYPTO_ALGO_TKIP		2
#define	CRYPTO_ALGO_WEP128		3
#define CRYPTO_ALGO_AES_CCM		4
#define CRYPTO_ALGO_AES_OCB_MSDU	5
#define CRYPTO_ALGO_AES_OCB_MPDU	6
#define CRYPTO_ALGO_NALG		7

#define WSEC_GEN_MIC_ERROR	0x0001
#define WSEC_GEN_REPLAY		0x0002

#define WL_SOFT_KEY	(1 << 0)	/* Indicates this key is using soft encrypt */
#define WL_PRIMARY_KEY	(1 << 1)	/* Indicates this key is the primary (ie tx) key */
#define WL_KF_RES_4	(1 << 4)	/* Reserved for backward compat */
#define WL_KF_RES_5	(1 << 5)	/* Reserved for backward compat */


typedef struct wlc_tkip_info {
	uint16		phase1[WLC_TKIP_P1_KEY_SIZE/sizeof(uint16)];	/* tkhash phase1 result */
	uint8		phase2[WLC_TKIP_P2_KEY_SIZE];	/* tkhash phase2 result */
	uint32		micl;
	uint32		micr;
} tkip_info_t;

typedef struct _wsec_iv {
	uint32		hi;	/* upper 32 bits of IV */
	uint16		lo;	/* lower 16 bits of IV */
} wsec_iv_t;

typedef struct wsec_key {
	uint32		index;		/* key index */
	uint32		len;		/* key length */
	uint8		data[WLC_MAX_KEY_SIZE];	/* key data */
	tkip_info_t	tkip_tx;	/* tkip transmit state */
	tkip_info_t	tkip_rx;	/* tkip receive state */
	uint32		algo;		/* CRYPTO_ALGO_AES_CCM, CRYPTO_ALGO_WEP128, etc */
	uint32		flags;		/* misc flags */
	uint32 		algo_hw;	/* cache for hw register*/
	uint32 		aes_mode;	/* cache for hw register*/
	int		iv_len;		/* IV length */		
	int		iv_initialized;	/* has IV been initialized already? */		
	int		icv_len;	/* ICV length */
	wsec_iv_t	rxiv;		/* Rx IV */
	wsec_iv_t	txiv;		/* Tx IV */
	struct ether_addr ea;		/* per station */
} wsec_key_t;



typedef struct wl_wsec_key {
	uint32		index;		/* key index */
	uint32		len;		/* key length */
	uint8		data[DOT11_MAX_KEY_SIZE];	/* key data */
	uint32		pad_1[18];
	uint32		algo;		/* CRYPTO_ALGO_AES_CCM, CRYPTO_ALGO_WEP128, etc */
	uint32		flags;		/* misc flags */
	uint32 		pad_2[2];
	int		pad_3;
	int		iv_initialized;	/* has IV been initialized already? */		
	int		pad_4;
	/* Rx IV */
	struct {
		uint32	hi;		/* upper 32 bits of IV */
		uint16	lo;		/* lower 16 bits of IV */
	} rxiv;
	uint32		pad_5[2];
	struct ether_addr ea;		/* per station */
} wl_wsec_key_t;

/* For use with wlc_wep_key.flags */
#define WSEC_PRIMARY_KEY	(1 << 1)	/* Indicates this key is the primary (ie tx) key */
#define WSEC_TKIP_ERROR		(1 << 2)	/* Provoke deliberate error */

#define WSEC_MIN_PSK_LEN	8
#define WSEC_MAX_PSK_LEN	64

/* Flag for key material needing passhash'ing */
#define WSEC_PASSPHRASE		(1<<0)

/* recepticle for WLC_SET_WSEC_PMK parameter */
typedef struct {
	ushort	key_len;		/* octets in key material */
	ushort	flags;			/* key handling qualification */
	uint8	key[WSEC_MAX_PSK_LEN];	/* PMK material */
} wsec_pmk_t;

/* wireless security bitvec */
#define WEP_ENABLED		1
#define TKIP_ENABLED		2
#define AES_ENABLED		4
#define WSEC_SWFLAG		8

#define WSEC_SW(wsec)		((wsec) & WSEC_SWFLAG)
#define WSEC_HW(wsec)		(!WSEC_SW(wsec))
#define WSEC_WEP_ENABLED(wsec)	((wsec) & WEP_ENABLED)
#define WSEC_TKIP_ENABLED(wsec)	((wsec) & TKIP_ENABLED)
#define WSEC_AES_ENABLED(wsec)	((wsec) & AES_ENABLED)
#define WSEC_ENABLED(wsec)	((wsec) & (WEP_ENABLED | TKIP_ENABLED | AES_ENABLED))


/* wireless authentication bit vector */
#define WPA_ENABLED	1
#define PSK_ENABLED	2

#define WAUTH_WPA_ENABLED(wauth)	((wauth) & WPA_ENABLED)
#define WAUTH_PSK_ENABLED(wauth)	((wauth) & PSK_ENABLED)
#define WAUTH_ENABLED(wauth)		((wauth) & (WPA_ENABLED | PSK_ENABLED))

/* group/mcast cipher */
#define WPA_MCAST_CIPHER(wsec)	(((wsec) & TKIP_ENABLED) ? WPA_CIPHER_TKIP : \
				((wsec) & AES_ENABLED) ? WPA_CIPHER_AES_CCM : \
				WPA_CIPHER_NONE)


typedef struct wl_led_info {
	uint32		index;		/* led index */
	uint32		behavior;
	bool		activehi;
} wl_led_info_t;

/*
 * definitions for driver messages passed from WL to NAS.
 */
/* Use this to recognize wpa and 802.1x driver messages. */
static const uint8 wl_wpa_snap_template[] =
	{ 0xaa, 0xaa, 0x03, 0x00, 0x90, 0x4c };

#define WL_WPA_MSG_IFNAME_MAX	16

/* WPA driver message */
typedef struct wl_wpa_header {
	struct ether_header eth;
	struct dot11_llc_snap_header snap;
	uint8 version;
	uint8 type;
	/* version 2 additions */
	char ifname[WL_WPA_MSG_IFNAME_MAX];
	/* version specific data */
	/* uint8 data[1]; */
} wl_wpa_header_t PACKED;

/*
 * definitions for 802.2 messages passed from WL to NAS.
 */
/* This seems not to be defined outside the kernel on linux. */
#ifndef ETH_P_802_2
#define ETH_P_802_2		4
#endif

#define WL_WPA_HEADER_LEN	(ETHER_HDR_LEN + DOT11_LLC_SNAP_HDR_LEN + 2 + WL_WPA_MSG_IFNAME_MAX)

/* WPA driver message ethertype - private between wlc and nas */
#define WL_WPA_ETHER_TYPE	0x9999

/* WPA driver message current version */
#define WL_WPA_MSG_VERSION	2

/* Type field values for the 802.2 driver messages for WPA. */
#define WLC_ASSOC_MSG		1
#define WLC_DISASSOC_MSG	2
#define WLC_PTK_MIC_MSG		3
#define WLC_GTK_MIC_MSG		4

/* Use this to recognize 802.2 driver messages. */
static const uint8 wpa_snap_template[] =
	{ 0xaa, 0xaa, 0x03, 0x00, 0x90, 0x4c };


/* 802.1x driver message */
typedef struct wl_eapol_header {
	struct ether_header eth;
	struct dot11_llc_snap_header snap;
	uint8 version;
	uint8 reserved;
	char ifname[WL_WPA_MSG_IFNAME_MAX];
	/* version specific data */
	/* uint8 802_1x_msg[1]; */
} wl_eapol_header_t PACKED;

#define WL_EAPOL_HEADER_LEN	(ETHER_HDR_LEN + DOT11_LLC_SNAP_HDR_LEN + 2 + WL_WPA_MSG_IFNAME_MAX)

/* 802.1x driver message ethertype - private between wlc and nas */
#define WL_EAPOL_ETHER_TYPE	0x999A

/* 802.1x driver message current version */
#define WL_EAPOL_MSG_VERSION	1

/* srom read/write struct passed through ioctl */
typedef struct {
	uint   byteoff;		/* byte offset */
	uint   nbytes;		/* number of bytes */
	uint16 buf[1];
} srom_rw_t;

/* R_REG and W_REG struct passed through ioctl */
typedef struct {
	uint32	byteoff;	/* byte offset of the field in d11regs_t */
	uint32	val;		/* read/write value of the field */
	uint32	size;		/* sizeof the field */
} rw_reg_t;

/* Structure used by GET/SET_ATTEN ioctls */
typedef struct {
	uint16	auto_ctrl;	/* 1: Automatic control, 0: overriden */
	uint16	bb;		/* Baseband attenuation */
	uint16	radio;		/* Radio attenuation */
	uint16	txctl1;		/* Radio TX_CTL1 value */
} atten_t;

/* Used to get specific STA parameters */ 
typedef struct {
	uint32	val;
	struct ether_addr ea;
} scb_val_t;

/* callback registration data types */

typedef struct _mac_event_params {
	uint msg;
	struct ether_addr *addr;
	uint result;
	uint status; 
	uint auth_type;
} mac_event_params_t;

typedef struct _mic_error_params {
	struct ether_addr *ea;
	bool group;
	bool flush_txq;
} mic_error_params_t;

typedef enum _wl_callback {
	WL_MAC_EVENT_CALLBACK = 0,
	WL_LINK_UP_CALLBACK,
	WL_LINK_DOWN_CALLBACK,
	WL_MIC_ERROR_CALLBACK,
	WL_LAST_CALLBACK
} wl_callback_t;

typedef struct _callback {
	void (*fn)(void *, void *);
	void *context;
} callback_t;

typedef struct _scan_callback {
	void (*fn)(void *);
	void *context;
} scan_callback_t;

/* used to register an arbitrary callback via the IOCTL interface */
typedef struct _set_callback {
	int index;
	callback_t callback;
} set_callback_t;



/* Event data type */
typedef struct {
	uint msg;			/* Message (see below) */
	struct ether_addr *addr;	/* Station address (if applicable) */
	uint status;			/* Status code (see below) */
	uint reason;			/* Reason code (if applicable) */
	uint auth_type;			/* WLC_E_AUTH */
	bool link;			/* WLC_E_LINK */
	bool group;			/* WLC_E_MIC_ERROR */
	bool flush_txq;			/* WLC_E_MIC_ERROR */
} wlc_event_t;

typedef struct {
	uint16		ver;	/* version of this struct */
	uint16		cap;	/* sta's advertized capabilities */
	uint32 		flags;	/* flags defined below */
	uint32		idle;	/* time since data pkt rx'd from sta */
	struct ether_addr	ea;	/* Station address */
	wl_rateset_t	rateset;	/* rateset in use */
} sta_info_t;

#define WL_STA_INFO_LEN	300
#define WL_STA_VER	1

/* flags fields */
#define WL_STA_BRCM	0x01
#define WL_STA_WME	0x02
#define WL_STA_ABCAP	0x04
#define WL_STA_AUTHE	0x08 
#define WL_STA_ASSOC	0x10 
#define WL_STA_AUTHO	0x20 

/* Event messages */
#define WLC_E_SET_SSID		1
#define WLC_E_JOIN		2
#define WLC_E_START		3
#define WLC_E_AUTH		4
#define WLC_E_AUTH_IND		5
#define WLC_E_DEAUTH		6
#define WLC_E_DEAUTH_IND	7
#define WLC_E_ASSOC		8
#define WLC_E_ASSOC_IND		9
#define WLC_E_REASSOC		10
#define WLC_E_REASSOC_IND	11
#define WLC_E_DISASSOC		12
#define WLC_E_DISASSOC_IND	13
#define WLC_E_QUIET_START	14	/* 802.11h Quiet period started */
#define WLC_E_QUIET_END		15	/* 802.11h Quiet period ended */
#define WLC_E_GOT_BEACONS	16	
#define WLC_E_LINK		17	/* Link indication */
#define WLC_E_MIC_ERROR		18	/* TKIP MIC error occurred */
#define WLC_E_NDIS_LINK		19	/* NDIS style link indication */
#define WLC_E_ROAM		20
#define WLC_E_LAST		21

/* Event status codes */
#define WLC_E_STATUS_SUCCESS		0
#define WLC_E_STATUS_FAIL		1
#define WLC_E_STATUS_TIMEOUT		2
#define WLC_E_STATUS_NO_NETWORKS	3
#define WLC_E_STATUS_ABORT		4

typedef struct wlc_event_cb {
	uint msg;				/* Event message or 0 for all */
	void (*fn)(void *, wlc_event_t *);	/* Callback function */
	void *context;				/* Passed to callback function */
	struct wlc_event_cb *next;		/* Next in the chain */
} wlc_event_cb_t;

/*
 * Country locale determines which channels are available to us.
 */
typedef enum _wlc_locale {
	WLC_WW = 0,	/* Worldwide */
	WLC_THA,	/* Thailand */
	WLC_ISR,	/* Israel */
	WLC_JDN,	/* Jordan */
	WLC_PRC,	/* China */
	WLC_JPN,	/* Japan */
	WLC_FCC,	/* USA */
	WLC_EUR,	/* Europe */
	WLC_USL,	/* US Low Band only */
	WLC_JPH,	/* Japan High Band only */
	WLC_ALL,	/* All the channels in this band */
	WLC_11D,	/* Represents locale recieved by 11d beacons */
	WLC_LAST_LOCALE,
	WLC_UNDEFINED_LOCALE = 0xf
} wlc_locale_t;

/* channel encoding */
typedef struct channel_info {
	int hw_channel;
	int target_channel;
	int scan_channel;
} channel_info_t;

/* For ioctls that take a list of MAC addresses */
struct maclist {
	uint count;			/* number of MAC addresses */
	struct ether_addr ea[1];	/* variable length array of MAC addresses */
};

/* get pkt count struct passed through ioctl */
typedef struct get_pktcnt {
	uint rx_good_pkt;
	uint rx_bad_pkt;
	uint tx_good_pkt;
	uint tx_bad_pkt;
} get_pktcnt_t;

/* Linux network driver ioctl encoding */
typedef struct wl_ioctl {
	uint cmd;	/* common ioctl definition */
	void *buf;	/* pointer to user buffer */
	uint len;	/* length of user buffer */
	bool set;	/* get or set request (optional) */
	uint used;	/* bytes read or written (optional) */
	uint needed;	/* bytes needed (optional) */
} wl_ioctl_t;

/* 
 * Structure for passing hardware and software 
 * revision info up from the driver. 
 */
typedef struct wlc_rev_info {
	uint		vendorid;	/* PCI vendor id */
	uint		deviceid;	/* device id of chip */
	uint		radiorev;	/* radio revision */
	uint		chiprev;	/* chip revision */
	uint		corerev;	/* core revision */
	uint		boardid;	/* board identifier (usu. PCI sub-device id) */
	uint		boardvendor;	/* board vendor (usu. PCI sub-vendor id) */
	uint		boardrev;	/* board revision */
	uint		driverrev;	/* driver version */
	uint		ucoderev;	/* microcode version */
	uint		bus;		/* bus type */
	uint        chipnum;    /* chip number */
} wlc_rev_info_t;

/* check this magic number */
#define WLC_IOCTL_MAGIC		0x14e46c77

/* bump this number if you change the ioctl interface */
#define WLC_IOCTL_VERSION	1

/* maximum length buffer required */
#define WLC_IOCTL_MAXLEN	8192

/* maximum length buffer required */
#define WLC_IOCTL_SMLEN         256

/* common ioctl definitions */
#define WLC_GET_MAGIC				0
#define WLC_GET_VERSION				1
#define WLC_UP					2
#define WLC_DOWN				3
#define WLC_GET_LOOP				4
#define WLC_SET_LOOP				5
#define WLC_DUMP				6
#define WLC_GET_MSGLEVEL			7
#define WLC_SET_MSGLEVEL			8
#define WLC_GET_PROMISC				9
#define WLC_SET_PROMISC				10
#define WLC_OBSOLETE				11 
#define WLC_GET_RATE				12
#define WLC_SET_RATE				13
#define WLC_GET_INSTANCE			14
#define WLC_GET_FRAG				15
#define WLC_SET_FRAG				16
#define WLC_GET_RTS				17
#define WLC_SET_RTS				18
#define WLC_GET_INFRA				19
#define WLC_SET_INFRA				20
#define WLC_GET_AUTH				21
#define WLC_SET_AUTH				22
#define WLC_GET_BSSID				23
#define WLC_SET_BSSID				24
#define WLC_GET_SSID				25
#define WLC_SET_SSID				26
#define WLC_RESTART				27
#define WLC_DUMP_SCB				28 
#define WLC_GET_CHANNEL				29
#define WLC_SET_CHANNEL				30
#define WLC_GET_SRL				31
#define WLC_SET_SRL				32
#define WLC_GET_LRL				33
#define WLC_SET_LRL				34
#define WLC_GET_PLCPHDR				35
#define WLC_SET_PLCPHDR				36
#define WLC_GET_RADIO				37
#define WLC_SET_RADIO				38
#define WLC_GET_PHYTYPE				39
#define WLC_DUMP_RATE				40
#define WLC_SET_RATE_PARAMS			41
#define WLC_GET_WEP				42
#define WLC_SET_WEP				43
#define WLC_GET_KEY				44
#define WLC_SET_KEY				45
#define WLC_GET_REGULATORY			46
#define WLC_SET_REGULATORY			47
#define WLC_GET_PASSIVE				48	/* added by nbd */
#define WLC_SET_PASSIVE				49	/* added by nbd */
#define WLC_SCAN				50
#define WLC_SCAN_RESULTS			51
#define WLC_DISASSOC				52
#define WLC_REASSOC				53
#define WLC_GET_ROAM_TRIGGER			54
#define WLC_SET_ROAM_TRIGGER			55
#define WLC_GET_ROAM_DELTA			56
#define WLC_SET_ROAM_DELTA			57
#define WLC_GET_ROAM_SCAN_PERIOD		58
#define WLC_SET_ROAM_SCAN_PERIOD		59
#define WLC_EVM					60
#define WLC_GET_TXANT				61
#define WLC_SET_TXANT				62
#define WLC_GET_ANTDIV				63
#define WLC_SET_ANTDIV				64
#define WLC_GET_TXPWR				65
#define WLC_SET_TXPWR				66
#define WLC_GET_CLOSED				67
#define WLC_SET_CLOSED				68
#define WLC_GET_MACLIST				69
#define WLC_SET_MACLIST				70
#define WLC_GET_RATESET				71
#define WLC_SET_RATESET				72
#define WLC_GET_LOCALE				73
#define WLC_SET_LOCALE				74
#define WLC_GET_BCNPRD				75
#define WLC_SET_BCNPRD				76
#define WLC_GET_DTIMPRD				77
#define WLC_SET_DTIMPRD				78
#define WLC_GET_SROM				79
#define WLC_SET_SROM				80
#define WLC_GET_WEP_RESTRICT			81
#define WLC_SET_WEP_RESTRICT			82
#define WLC_GET_COUNTRY				83
#define WLC_SET_COUNTRY				84
#define WLC_GET_PM				85
#define WLC_SET_PM				86
#define WLC_GET_WAKE				87
#define WLC_SET_WAKE				88
#define	WLC_GET_D11CNTS				89
#define WLC_GET_FORCELINK			90	/* ndis only */
#define WLC_SET_FORCELINK			91	/* ndis only */
#define WLC_FREQ_ACCURACY			92
#define WLC_CARRIER_SUPPRESS			93
#define WLC_GET_PHYREG				94
#define WLC_SET_PHYREG				95
#define WLC_GET_RADIOREG			96
#define WLC_SET_RADIOREG			97
#define WLC_GET_REVINFO				98
#define WLC_GET_UCANTDIV			99
#define WLC_SET_UCANTDIV			100
#define WLC_R_REG				101
#define WLC_W_REG				102
#define WLC_DIAG_LOOPBACK			103
#define WLC_RESET_D11CNTS			104
#define WLC_GET_MACMODE				105
#define WLC_SET_MACMODE				106
#define WLC_GET_MONITOR				107     /* added by nbd */
#define WLC_SET_MONITOR				108     /* added by nbd */
#define WLC_GET_GMODE				109
#define WLC_SET_GMODE				110
#define WLC_GET_LEGACY_ERP			111
#define WLC_SET_LEGACY_ERP			112
#define WLC_GET_RX_ANT				113
#define WLC_GET_CURR_RATESET			114	/* current rateset */
#define WLC_GET_SCANSUPPRESS			115
#define WLC_SET_SCANSUPPRESS			116
#define WLC_GET_AP				117
#define WLC_SET_AP				118
#define WLC_GET_EAP_RESTRICT			119
#define WLC_SET_EAP_RESTRICT			120
#define WLC_SCB_AUTHORIZE			121
#define WLC_SCB_DEAUTHORIZE			122
#define WLC_GET_WDSLIST				123
#define WLC_SET_WDSLIST				124
#define WLC_GET_ATIM				125
#define WLC_SET_ATIM				126
#define WLC_GET_RSSI				127
#define WLC_GET_PHYANTDIV			128
#define WLC_SET_PHYANTDIV			129
#define WLC_AP_RX_ONLY				130
#define WLC_GET_TX_PATH_PWR			131
#define WLC_SET_TX_PATH_PWR			132
#define WLC_GET_WSEC				133
#define WLC_SET_WSEC				134
#define WLC_GET_PHY_NOISE			135
#define WLC_GET_BSS_INFO			136
#define WLC_GET_PKTCNTS				137
#define WLC_GET_LAZYWDS				138
#define WLC_SET_LAZYWDS				139
#define WLC_GET_BANDLIST			140
#define WLC_GET_BAND				141
#define WLC_SET_BAND				142
#define WLC_SCB_DEAUTHENTICATE			143
#define WLC_GET_SHORTSLOT			144
#define WLC_GET_SHORTSLOT_OVERRIDE		145
#define WLC_SET_SHORTSLOT_OVERRIDE		146
#define WLC_GET_SHORTSLOT_RESTRICT		147
#define WLC_SET_SHORTSLOT_RESTRICT		148
#define WLC_GET_GMODE_PROTECTION		149
#define WLC_GET_GMODE_PROTECTION_OVERRIDE	150
#define WLC_SET_GMODE_PROTECTION_OVERRIDE	151
#define WLC_UPGRADE				152
#define WLC_GET_MRATE				153
#define WLC_SET_MRATE				154
#define WLC_GET_IGNORE_BCNS			155
#define WLC_SET_IGNORE_BCNS			156
#define WLC_GET_SCB_TIMEOUT			157
#define WLC_SET_SCB_TIMEOUT			158
#define WLC_GET_ASSOCLIST			159
#define WLC_GET_CLK				160
#define WLC_SET_CLK				161
#define WLC_GET_UP				162
#define WLC_OUT					163
#define WLC_GET_WPA_AUTH			164
#define WLC_SET_WPA_AUTH			165
#define WLC_GET_UCFLAGS				166
#define WLC_SET_UCFLAGS				167
#define WLC_GET_PWRIDX				168
#define WLC_SET_PWRIDX				169
#define WLC_GET_TSSI				170
#define WLC_GET_SUP_RATESET_OVERRIDE		171
#define WLC_SET_SUP_RATESET_OVERRIDE		172
#define WLC_SET_FAST_TIMER			173
#define WLC_GET_FAST_TIMER			174
#define WLC_SET_SLOW_TIMER			175
#define WLC_GET_SLOW_TIMER			176
#define WLC_DUMP_PHYREGS			177
#define WLC_GET_GMODE_PROTECTION_CONTROL	178
#define WLC_SET_GMODE_PROTECTION_CONTROL	179
#define WLC_GET_PHYLIST				180
#define WLC_ENCRYPT_STRENGTH			181	/* ndis only */
#define WLC_DECRYPT_STATUS			182	/* ndis only */
#define WLC_GET_KEY_SEQ				183
#define WLC_GET_SCAN_CHANNEL_TIME		184
#define WLC_SET_SCAN_CHANNEL_TIME		185
#define WLC_GET_SCAN_UNASSOC_TIME		186
#define WLC_SET_SCAN_UNASSOC_TIME		187
#define WLC_GET_SCAN_HOME_TIME			188
#define WLC_SET_SCAN_HOME_TIME			189
#define WLC_GET_SCAN_PASSES			190
#define WLC_SET_SCAN_PASSES			191
#define WLC_GET_PRB_RESP_TIMEOUT		192
#define WLC_SET_PRB_RESP_TIMEOUT		193
#define WLC_GET_ATTEN				194
#define WLC_SET_ATTEN				195
#define WLC_GET_SHMEM				196	/* diag */
#define WLC_SET_SHMEM				197	/* diag */
#define WLC_GET_GMODE_PROTECTION_CTS		198
#define WLC_SET_GMODE_PROTECTION_CTS		199
#define WLC_SET_TKIP_MIC_FLAG			200
#define WLC_SCB_DEAUTHENTICATE_FOR_REASON	201
#define WLC_TKIP_COUNTERMEASURES		202
#define WLC_GET_PIOMODE				203
#define WLC_SET_PIOMODE				204
#define WLC_SET_LED				209
#define WLC_GET_LED				210
#define WLC_GET_INTERFERENCE_MODE		211
#define WLC_SET_INTERFERENCE_MODE		212
#define WLC_GET_CHANNEL_QA			213
#define WLC_START_CHANNEL_QA			214
#define WLC_GET_CHANNEL_SEL			215
#define WLC_START_CHANNEL_SEL			216
#define WLC_GET_VALID_CHANNELS			217
#define WLC_GET_FAKEFRAG			218
#define WLC_SET_FAKEFRAG			219
#define WLC_GET_PWROUT_PERCENTAGE		220
#define WLC_SET_PWROUT_PERCENTAGE		221
#define WLC_SET_BAD_FRAME_PREEMPT		222
#define WLC_GET_BAD_FRAME_PREEMPT		223
#define WLC_SET_LEAP_LIST			224
#define WLC_GET_LEAP_LIST			225
#define WLC_GET_CWMIN				226
#define WLC_SET_CWMIN				227
#define WLC_GET_CWMAX				228
#define WLC_SET_CWMAX				229
#define WLC_GET_WET				230
#define WLC_SET_WET				231
#define WLC_GET_KEY_PRIMARY			235
#define WLC_SET_KEY_PRIMARY			236
#define WLC_SCAN_WITH_CALLBACK			240
#define WLC_WDS_GET_REMOTE_HWADDR		246	/* currently handled in wl_linux.c/wl_vx.c */
#define WLC_SET_CS_SCAN_TIMER			248
#define WLC_GET_CS_SCAN_TIMER			249
#define WLC_CURRENT_PWR				256
#define WLC_GET_CHANNELS_IN_COUNTRY		260
#define WLC_GET_COUNTRY_LIST			261
#define WLC_GET_VAR				262	/* get value of named variable */
#define WLC_SET_VAR				263	/* set named variable to value */
#define WLC_NVRAM_GET				264
#define WLC_NVRAM_SET				265
#define WLC_SET_WSEC_PMK			268
#define WLC_GET_AUTH_MODE			269
#define WLC_SET_AUTH_MODE			270
#define WLC_LAST				273	/* do not change - use get_var/set_var */

/*
 * Minor kludge alert:
 * Duplicate a few definitions that irelay requires from epiioctl.h here
 * so caller doesn't have to include this file and epiioctl.h .
 * If this grows any more, it would be time to move these irelay-specific
 * definitions out of the epiioctl.h and into a separate driver common file.
 */
#ifndef EPICTRL_COOKIE
#define EPICTRL_COOKIE		0xABADCEDE
#endif

/* vx wlc ioctl's offset */
#define CMN_IOCTL_OFF 0x180

/*
 * custom OID support
 *
 * 0xFF - implementation specific OID
 * 0xE4 - first byte of Broadcom PCI vendor ID
 * 0x14 - second byte of Broadcom PCI vendor ID
 * 0xXX - the custom OID number
 */

/* begin 0x1f values beyond the start of the ET driver range. */
#define WL_OID_BASE		0xFFE41420

/* NDIS overrides */
#define OID_WL_GETINSTANCE	(WL_OID_BASE + WLC_GET_INSTANCE)
#define OID_WL_GET_FORCELINK	(WL_OID_BASE + WLC_GET_FORCELINK)
#define OID_WL_SET_FORCELINK	(WL_OID_BASE + WLC_SET_FORCELINK)
#define	OID_WL_ENCRYPT_STRENGTH	(WL_OID_BASE + WLC_ENCRYPT_STRENGTH)
#define OID_WL_DECRYPT_STATUS	(WL_OID_BASE + WLC_DECRYPT_STATUS)

#define WL_DECRYPT_STATUS_SUCCESS	1
#define WL_DECRYPT_STATUS_FAILURE	2
#define WL_DECRYPT_STATUS_UNKNOWN	3

/* allows user-mode app to poll the status of USB image upgrade */
#define WLC_UPGRADE_SUCCESS			0
#define WLC_UPGRADE_PENDING			1

/* Bit masks for radio disabled status - returned by WL_GET_RADIO */
#define WL_RADIO_SW_DISABLE	(1<<0)
#define WL_RADIO_HW_DISABLE	(1<<1)

/* Override bit for WLC_SET_TXPWR.  if set, ignore other level limits */
#define WL_TXPWR_OVERRIDE	(1<<31)


/* Bus types */
#define WL_SB_BUS	0	/* Silicon Backplane */
#define WL_PCI_BUS	1	/* PCI target */
#define WL_PCMCIA_BUS	2	/* PCMCIA target */

/* band types */
#define	WLC_BAND_AUTO		0	/* auto-select */
#define	WLC_BAND_A		1	/* "a" band (5   Ghz) */
#define	WLC_BAND_B		2	/* "b" band (2.4 Ghz) */

/* MAC list modes */
#define WLC_MACMODE_DISABLED	0	/* MAC list disabled */
#define WLC_MACMODE_DENY	1	/* Deny specified (i.e. allow unspecified) */
#define WLC_MACMODE_ALLOW	2	/* Allow specified (i.e. deny unspecified) */	


/* 54g modes (basic bits may still be overridden) */
#define GMODE_LEGACY_B		0	/* Rateset: 1b, 2b, 5.5, 11 */
					/* Preamble: Long */
					/* Shortslot: Off */
#define GMODE_AUTO		1	/* Rateset: 1b, 2b, 5.5b, 11b, 18, 24, 36, 54 */
					/* Extended Rateset: 6, 9, 12, 48 */
					/* Preamble: Long */
					/* Shortslot: Auto */
#define GMODE_ONLY		2	/* Rateset: 1b, 2b, 5.5b, 11b, 18, 24b, 36, 54 */
					/* Extended Rateset: 6b, 9, 12b, 48 */
					/* Preamble: Short required */
					/* Shortslot: Auto */
#define GMODE_B_DEFERRED	3	/* Rateset: 1b, 2b, 5.5b, 11b, 18, 24, 36, 54 */
					/* Extended Rateset: 6, 9, 12, 48 */
					/* Preamble: Long */
					/* Shortslot: On */
#define GMODE_PERFORMANCE	4	/* Rateset: 1b, 2b, 5.5b, 6b, 9, 11b, 12b, 18, 24b, 36, 48, 54 */
					/* Preamble: Short required */
					/* Shortslot: On and required */
#define GMODE_LRS		5	/* Rateset: 1b, 2b, 5.5b, 11b */
					/* Extended Rateset: 6, 9, 12, 18, 24, 36, 48, 54 */
					/* Preamble: Long */
					/* Shortslot: Auto */
#define GMODE_MAX		6


/* values for PLCPHdr_override */
#define WLC_PLCP_AUTO	-1
#define WLC_PLCP_SHORT	0
#define WLC_PLCP_LONG	1

/* values for g_protection_override */
#define WLC_G_PROTECTION_AUTO	-1
#define WLC_G_PROTECTION_OFF	0
#define WLC_G_PROTECTION_ON	1

/* values for g_protection_control */
#define WLC_G_PROTECTION_CTL_OFF	0
#define WLC_G_PROTECTION_CTL_LOCAL	1
#define WLC_G_PROTECTION_CTL_OVERLAP	2

/* Values for PM */
#define PM_OFF	0
#define PM_MAX	1
#define PM_FAST 2

/* interference mitigation options */
#define	INTERFERE_NONE	0	/* off */
#define	NON_WLAN        1	/* foreign/non 802.11 interference, no auto detect */
#define	WLAN_MANUAL     2	/* ACI: no auto detection */
#define	WLAN_AUTO       3	/* ACI: auto - detact */

/* Message levels */
#define WL_ERROR_VAL		0x0001
#define WL_TRACE_VAL		0x0002
#define WL_PRHDRS_VAL		0x0004
#define WL_PRPKT_VAL		0x0008
#define WL_INFORM_VAL		0x0010
#define WL_TMP_VAL		0x0020
#define WL_OID_VAL		0x0040
#define WL_RATE_VAL		0x0080
#define WL_ASSOC_VAL		0x0100
#define WL_PRUSR_VAL		0x0200
#define WL_PS_VAL		0x0400
#define WL_TXPWR_VAL		0x0800
#define WL_GMODE_VAL		0x1000
#define WL_DUAL_VAL		0x2000
#define WL_WSEC_VAL		0x4000
#define WL_WSEC_DUMP_VAL	0x8000
#define WL_LOG_VAL		0x10000
#define WL_NRSSI_VAL		0x20000
#define WL_LOFT_VAL		0x40000
#define WL_REGULATORY_VAL	0x80000
#define WL_ACI_VAL		0x100000


/* 802.11h enforcement levels */
#define SPECT_MNGMT_OFF         0   /* 11h disabled */
#define SPECT_MNGMT_LOOSE       1   /* Allow scan lists to contain non-11h AP */
				    /* when 11h is enabled */
#define SPECT_MNGMT_STRICT      2   /* Prine out non-11h APs from scan list */



/* max # of leds supported by GPIO (gpio pin# == led index#) */
#define	WL_LED_NUMGPIO		16	/* gpio 0-15 */

/* led per-pin behaviors */
#define	WL_LED_OFF		0		/* always off */
#define	WL_LED_ON		1		/* always on */
#define	WL_LED_ACTIVITY		2		/* activity */
#define	WL_LED_RADIO		3		/* radio enabled */
#define	WL_LED_ARADIO		4		/* 5  Ghz radio enabled */
#define	WL_LED_BRADIO		5		/* 2.4Ghz radio enabled */
#define	WL_LED_BGMODE		6		/* on if gmode, off if bmode */
#define	WL_LED_WI1		7		
#define	WL_LED_WI2		8		
#define	WL_LED_WI3		9		
#define	WL_LED_ASSOC		10		/* associated state indicator */
#define	WL_LED_INACTIVE		11		/* null behavior (clears default behavior) */
#define	WL_LED_NUMBEHAVIOR	12

/* led behavior numeric value format */
#define	WL_LED_BEH_MASK		0x7f		/* behavior mask */
#define	WL_LED_AL_MASK		0x80		/* activelow (polarity) bit */


/* maximum channels */
#define WL_NUMCHANNELS	64	/* max # of channels in the band */

/* rate check */
#define WL_RATE_OFDM(r)		(((r) & 0x7f) == 12 || ((r) & 0x7f) == 18 || \
				 ((r) & 0x7f) == 24 || ((r) & 0x7f) == 36 || \
				 ((r) & 0x7f) == 48 || ((r) & 0x7f) == 72 || \
				 ((r) & 0x7f) == 96 || ((r) & 0x7f) == 108)

/* WDS link local endpoint WPA role */
#define WL_WDS_WPA_ROLE_AUTH	0	/* authenticator */
#define WL_WDS_WPA_ROLE_SUP	1	/* supplicant */
#define WL_WDS_WPA_ROLE_AUTO	255	/* auto, based on mac addr value */

/* afterburner_override */
#define	ABO_AUTO		-1	/* auto - no override */
#define	ABO_OFF			0	/* force afterburner off */
#define	ABO_ON			1	/* force afterburner on */

#define GMODE_AFTERBURNER 6

#undef PACKED

#endif /* _wlioctl_h_ */
