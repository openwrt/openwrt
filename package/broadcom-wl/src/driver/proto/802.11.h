/*
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * Fundamental types and constants relating to 802.11
 *
 * $Id$
 */

#ifndef _802_11_H_
#define _802_11_H_

#ifndef _TYPEDEFS_H_
#include <typedefs.h>
#endif

#ifndef _NET_ETHERNET_H_
#include <proto/ethernet.h>
#endif

#include <proto/wpa.h>

/* enable structure packing */
#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#pragma pack(1)
#define	PACKED
#endif

#define DOT11_TU_TO_US			1024	/* 802.11 Time Unit is 1024 microseconds */

/* Generic 802.11 frame constants */
#define DOT11_A3_HDR_LEN		24	/* d11 header length with A3 */
#define DOT11_A4_HDR_LEN		30	/* d11 header length with A4 */
#define DOT11_MAC_HDR_LEN		DOT11_A3_HDR_LEN	/* MAC header length */
#define DOT11_FCS_LEN			4	/* d11 FCS length */
#define DOT11_ICV_LEN			4	/* d11 ICV length */
#define DOT11_ICV_AES_LEN		8	/* d11 ICV/AES length */
#define DOT11_QOS_LEN			2	/* d11 QoS length */
#define DOT11_HTC_LEN			4	/* d11 HT Control field length */

#define DOT11_KEY_INDEX_SHIFT		6	/* d11 key index shift */
#define DOT11_IV_LEN			4	/* d11 IV length */
#define DOT11_IV_TKIP_LEN		8	/* d11 IV TKIP length */
#define DOT11_IV_AES_OCB_LEN		4	/* d11 IV/AES/OCB length */
#define DOT11_IV_AES_CCM_LEN		8	/* d11 IV/AES/CCM length */
#define DOT11_IV_MAX_LEN		8	/* maximum iv len for any encryption */

/* Includes MIC */
#define DOT11_MAX_MPDU_BODY_LEN		2304	/* max MPDU body length */
/* A4 header + QoS + CCMP + PDU + ICV + FCS = 2352 */
#define DOT11_MAX_MPDU_LEN		(DOT11_A4_HDR_LEN + \
					 DOT11_QOS_LEN + \
					 DOT11_IV_AES_CCM_LEN + \
					 DOT11_MAX_MPDU_BODY_LEN + \
					 DOT11_ICV_LEN + \
					 DOT11_FCS_LEN)	/* d11 max MPDU length */

#define DOT11_MAX_SSID_LEN		32	/* d11 max ssid length */

/* dot11RTSThreshold */
#define DOT11_DEFAULT_RTS_LEN		2347	/* d11 default RTS length */
#define DOT11_MAX_RTS_LEN		2347	/* d11 max RTS length */

/* dot11FragmentationThreshold */
#define DOT11_MIN_FRAG_LEN		256	/* d11 min fragmentation length */
#define DOT11_MAX_FRAG_LEN		2346	/* Max frag is also limited by aMPDUMaxLength
						* of the attached PHY
						*/
#define DOT11_DEFAULT_FRAG_LEN		2346	/* d11 default fragmentation length */

/* dot11BeaconPeriod */
#define DOT11_MIN_BEACON_PERIOD		1	/* d11 min beacon period */
#define DOT11_MAX_BEACON_PERIOD		0xFFFF	/* d11 max beacon period */

/* dot11DTIMPeriod */
#define DOT11_MIN_DTIM_PERIOD		1	/* d11 min DTIM period */
#define DOT11_MAX_DTIM_PERIOD		0xFF	/* d11 max DTIM period */

/* 802.2 LLC/SNAP header used by 802.11 per 802.1H */
#define DOT11_LLC_SNAP_HDR_LEN		8	/* d11 LLC/SNAP header length */
#define DOT11_OUI_LEN			3	/* d11 OUI length */
struct dot11_llc_snap_header {
	uint8	dsap;				/* always 0xAA */
	uint8	ssap;				/* always 0xAA */
	uint8	ctl;				/* always 0x03 */
	uint8	oui[DOT11_OUI_LEN];		/* RFC1042: 0x00 0x00 0x00
						 * Bridge-Tunnel: 0x00 0x00 0xF8
						 */
	uint16	type;				/* ethertype */
} PACKED;

/* RFC1042 header used by 802.11 per 802.1H */
#define RFC1042_HDR_LEN	(ETHER_HDR_LEN + DOT11_LLC_SNAP_HDR_LEN)	/* RCF1042 header length */

/* Generic 802.11 MAC header */
/*
 * N.B.: This struct reflects the full 4 address 802.11 MAC header.
 *		 The fields are defined such that the shorter 1, 2, and 3
 *		 address headers just use the first k fields.
 */
struct dot11_header {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	a1;		/* address 1 */
	struct ether_addr	a2;		/* address 2 */
	struct ether_addr	a3;		/* address 3 */
	uint16			seq;		/* sequence control */
	struct ether_addr	a4;		/* address 4 */
} PACKED;

/* Control frames */

struct dot11_rts_frame {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	ra;		/* receiver address */
	struct ether_addr	ta;		/* transmitter address */
} PACKED;
#define	DOT11_RTS_LEN		16		/* d11 RTS frame length */

struct dot11_cts_frame {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	ra;		/* receiver address */
} PACKED;
#define	DOT11_CTS_LEN		10		/* d11 CTS frame length */

struct dot11_ack_frame {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	ra;		/* receiver address */
} PACKED;
#define	DOT11_ACK_LEN		10		/* d11 ACK frame length */

struct dot11_ps_poll_frame {
	uint16			fc;		/* frame control */
	uint16			durid;		/* AID */
	struct ether_addr	bssid;		/* receiver address, STA in AP */
	struct ether_addr	ta;		/* transmitter address */
} PACKED;
#define	DOT11_PS_POLL_LEN	16		/* d11 PS poll frame length */

struct dot11_cf_end_frame {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	ra;		/* receiver address */
	struct ether_addr	bssid;		/* transmitter address, STA in AP */
} PACKED;
#define	DOT11_CS_END_LEN	16		/* d11 CF-END frame length */

/* BA/BAR Control parameters */
#define DOT11_BA_CTL_POLICY_NORMAL	0x0000	/* normal ack */
#define DOT11_BA_CTL_POLICY_NOACK	0x0001	/* no ack */
#define DOT11_BA_CTL_POLICY_MASK	0x0001	/* ack policy mask */

#define DOT11_BA_CTL_MTID		0x0002	/* multi tid BA */
#define DOT11_BA_CTL_COMPRESSED		0x0004	/* compressed bitmap */

#define DOT11_BA_CTL_NUMMSDU_MASK	0x0FC0	/* num msdu in bitmap mask */
#define DOT11_BA_CTL_NUMMSDU_SHIFT	6	/* num msdu in bitmap shift */

#define DOT11_BA_CTL_TID_MASK		0xF000	/* tid mask */
#define DOT11_BA_CTL_TID_SHIFT		12	/* tid shift */

/* control frame header (BA/BAR) */
struct dot11_ctl_header {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	ra;		/* receiver address */
	struct ether_addr	ta;		/* transmitter address */
} PACKED;
#define DOT11_CTL_HDR_LEN	16		/* control frame hdr len */

/* BAR frame payload */
struct dot11_bar {
	uint16			bar_control;	/* BAR Control */
	uint16			seqnum;		/* Starting Sequence control */
} PACKED;
#define DOT11_BAR_LEN		4		/* BAR frame payload length */

#define DOT11_BA_BITMAP_LEN	128		/* bitmap length */
#define DOT11_BA_CMP_BITMAP_LEN	8		/* compressed bitmap length */
/* BA frame payload */
struct dot11_ba {
	uint16			ba_control;	/* BA Control */
	uint16			seqnum;		/* Starting Sequence control */
	uint8			bitmap[DOT11_BA_BITMAP_LEN];	/* Block Ack Bitmap */
} PACKED;
#define DOT11_BA_LEN		4		/* BA frame payload len (wo bitmap) */

/* Management frame header */
struct dot11_management_header {
	uint16			fc;		/* frame control */
	uint16			durid;		/* duration/ID */
	struct ether_addr	da;		/* receiver address */
	struct ether_addr	sa;		/* transmitter address */
	struct ether_addr	bssid;		/* BSS ID */
	uint16			seq;		/* sequence control */
} PACKED;
#define	DOT11_MGMT_HDR_LEN	24		/* d11 management header length */

/* Management frame payloads */

struct dot11_bcn_prb {
	uint32			timestamp[2];
	uint16			beacon_interval;
	uint16			capability;
} PACKED;
#define	DOT11_BCN_PRB_LEN	12		/* 802.11 beacon/probe frame fixed length */

struct dot11_auth {
	uint16			alg;		/* algorithm */
	uint16			seq;		/* sequence control */
	uint16			status;		/* status code */
} PACKED;
#define DOT11_AUTH_FIXED_LEN	6		/* length of auth frame without challenge info
						 * elt
						 */

struct dot11_assoc_req {
	uint16			capability;	/* capability information */
	uint16			listen;		/* listen interval */
} PACKED;
#define DOT11_ASSOC_REQ_FIXED_LEN	4	/* length of assoc frame without info elts */

struct dot11_reassoc_req {
	uint16			capability;	/* capability information */
	uint16			listen;		/* listen interval */
	struct ether_addr	ap;		/* Current AP address */
} PACKED;
#define DOT11_REASSOC_REQ_FIXED_LEN	10	/* length of assoc frame without info elts */

struct dot11_assoc_resp {
	uint16			capability;	/* capability information */
	uint16			status;		/* status code */
	uint16			aid;		/* association ID */
} PACKED;

struct dot11_action_measure {
	uint8	category;
	uint8	action;
	uint8	token;
	uint8	data[1];
} PACKED;
#define DOT11_ACTION_MEASURE_LEN	3	/* d11 action measurement header length */

struct dot11_action_ht_ch_width {
	uint8	category;
	uint8	action;
	uint8	ch_width;
} PACKED;

struct dot11_action_ht_mimops {
	uint8	category;
	uint8	action;
	uint8	control;
} PACKED;

#define SM_PWRSAVE_ENABLE	1
#define SM_PWRSAVE_MODE		2

struct dot11_action_ht_info_xchg {
	uint8	category;
	uint8	action;
	uint8	info;
} PACKED;

#define	DOT11_HT_INFO_XCHG_INFO_REQ		0x01
#define	DOT11_HT_INFO_XCHG_40MHZ_INTOLERANT	0x02
#define	DOT11_HT_INFO_XCHG_STA_CHAN_WIDTH	0x04


/* ************* 802.11h related definitions. ************* */
struct dot11_power_cnst {
	uint8 id;
	uint8 len;
	uint8 power;
} PACKED;
typedef struct dot11_power_cnst dot11_power_cnst_t;

struct dot11_power_cap {
	uint8 min;
	uint8 max;
} PACKED;
typedef struct dot11_power_cap dot11_power_cap_t;

struct dot11_tpc_rep {
	uint8 id;
	uint8 len;
	uint8 tx_pwr;
	uint8 margin;
} PACKED;
typedef struct dot11_tpc_rep dot11_tpc_rep_t;
#define DOT11_MNG_IE_TPC_REPORT_LEN	2 	/* length of IE data, not including 2 byte header */

struct dot11_supp_channels {
	uint8 id;
	uint8 len;
	uint8 first_channel;
	uint8 num_channels;
} PACKED;
typedef struct dot11_supp_channels dot11_supp_channels_t;

/* Extension Channel Offset IE: 802.11n-D1.0 spec. added sideband
 * offset for 40MHz operation.  The possible 3 values are:
 * 1 = above control channel
 * 3 = below control channel
 * 0 = no extension channel
 */
struct dot11_extch {
	uint8	id;		/* IE ID, 62, DOT11_MNG_EXT_CHANNEL_OFFSET */
	uint8	len;		/* IE length */
	uint8	extch;
} PACKED;
typedef struct dot11_extch dot11_extch_ie_t;

struct dot11_brcm_extch {
	uint8	id;		/* IE ID, 221, DOT11_MNG_PROPR_ID */
	uint8	len;		/* IE length */
	uint8	oui[3];		/* Proprietary OUI, BRCM_OUI */
	uint8	type;           /* type inidicates what follows */
	uint8	extch;
} PACKED;
typedef struct dot11_brcm_extch dot11_brcm_extch_ie_t;

#define BRCM_EXTCH_IE_LEN	5
#define BRCM_EXTCH_IE_TYPE	53	/* 802.11n ID not yet assigned */
#define DOT11_EXTCH_IE_LEN	1
#define DOT11_EXT_CH_MASK	0x03	/* extension channel mask */
#define DOT11_EXT_CH_UPPER	0x01	/* ext. ch. on upper sb */
#define DOT11_EXT_CH_LOWER	0x03	/* ext. ch. on lower sb */
#define DOT11_EXT_CH_NONE	0x00	/* no extension ch.  */

struct dot11_action_frmhdr {
	uint8	category;
	uint8	action;
	uint8	data[1];
} PACKED;

/* CSA IE data structure */
struct dot11_channel_switch {
	uint8 id;	/* id DOT11_MNG_CHANNEL_SWITCH_ID */
	uint8 len;	/* length of IE */
	uint8 mode;	/* mode 0 or 1 */
	uint8 channel;	/* channel switch to */
	uint8 count;	/* number of beacons before switching */
} PACKED;
typedef struct dot11_channel_switch dot11_chan_switch_ie_t;

#define DOT11_SWITCH_IE_LEN	3	/* length of IE data, not including 2 byte header */
/* CSA mode - 802.11h-2003 $7.3.2.20 */
#define DOT11_CSA_MODE_ADVISORY		0	/* no DOT11_CSA_MODE_NO_TX restriction imposed */
#define DOT11_CSA_MODE_NO_TX		1	/* no transmission upon receiving CSA frame. */

struct dot11_action_switch_channel {
	uint8	category;
	uint8	action;
	dot11_chan_switch_ie_t chan_switch_ie;	/* for switch IE */
	dot11_brcm_extch_ie_t extch_ie;		/* extension channel offset */
} PACKED;

/* 11n Extended Channel Switch IE data structure */
struct dot11_ext_csa {
	uint8 id;	/* id DOT11_MNG_EXT_CHANNEL_SWITCH_ID */
	uint8 len;	/* length of IE */
	uint8 mode;	/* mode 0 or 1 */
	uint8 reg;	/* regulatory class */
	uint8 channel;	/* channel switch to */
	uint8 count;	/* number of beacons before switching */
} PACKED;
typedef struct dot11_ext_csa dot11_ext_csa_ie_t;
#define DOT11_EXT_CSA_IE_LEN	4	/* length of extended channel switch IE body */

struct dot11_action_ext_csa {
	uint8	category;
	uint8	action;
	dot11_ext_csa_ie_t chan_switch_ie;	/* for switch IE */
} PACKED;

/* 802.11h Measurement Request/Report IEs */
/* Measurement Type field */
#define DOT11_MEASURE_TYPE_BASIC 	0	/* d11 measurement basic type */
#define DOT11_MEASURE_TYPE_CCA 		1	/* d11 measurement CCA type */
#define DOT11_MEASURE_TYPE_RPI		2	/* d11 measurement PRI type */

/* Measurement Request Modes */
#define DOT11_MEASURE_MODE_ENABLE 	(1<<1)	/* d11 measurement enable */
#define DOT11_MEASURE_MODE_REQUEST	(1<<2)	/* d11 measurement request */
#define DOT11_MEASURE_MODE_REPORT 	(1<<3)	/* d11 measurement report */
/* Measurement Report Modes */
#define DOT11_MEASURE_MODE_LATE 	(1<<0)	/* d11 measurement late */
#define DOT11_MEASURE_MODE_INCAPABLE	(1<<1)	/* d11 measurement incapable */
#define DOT11_MEASURE_MODE_REFUSED	(1<<2)	/* d11 measurement refuse */
/* Basic Measurement Map bits */
#define DOT11_MEASURE_BASIC_MAP_BSS	((uint8)(1<<0))	/* d11 measurement basic map BSS */
#define DOT11_MEASURE_BASIC_MAP_OFDM	((uint8)(1<<1))	/* d11 measurement map OFDM */
#define DOT11_MEASURE_BASIC_MAP_UKNOWN	((uint8)(1<<2))	/* d11 measurement map unknown */
#define DOT11_MEASURE_BASIC_MAP_RADAR	((uint8)(1<<3))	/* d11 measurement map radar */
#define DOT11_MEASURE_BASIC_MAP_UNMEAS	((uint8)(1<<4))	/* d11 measurement map unmeasuremnt */

struct dot11_meas_req {
	uint8 id;
	uint8 len;
	uint8 token;
	uint8 mode;
	uint8 type;
	uint8 channel;
	uint8 start_time[8];
	uint16 duration;
} PACKED;
typedef struct dot11_meas_req dot11_meas_req_t;
#define DOT11_MNG_IE_MREQ_LEN 14	/* d11 measurement request IE length */
/* length of Measure Request IE data not including variable len */
#define DOT11_MNG_IE_MREQ_FIXED_LEN 3	/* d11 measurement request IE fixed length */

struct dot11_meas_rep {
	uint8 id;
	uint8 len;
	uint8 token;
	uint8 mode;
	uint8 type;
	union
	{
		struct {
			uint8 channel;
			uint8 start_time[8];
			uint16 duration;
			uint8 map;
		} PACKED basic;
		uint8 data[1];
	} PACKED rep;
} PACKED;
typedef struct dot11_meas_rep dot11_meas_rep_t;

/* length of Measure Report IE data not including variable len */
#define DOT11_MNG_IE_MREP_FIXED_LEN	3	/* d11 measurement response IE fixed length */

struct dot11_meas_rep_basic {
	uint8 channel;
	uint8 start_time[8];
	uint16 duration;
	uint8 map;
} PACKED;
typedef struct dot11_meas_rep_basic dot11_meas_rep_basic_t;
#define DOT11_MEASURE_BASIC_REP_LEN	12	/* d11 measurement basic report length */

struct dot11_quiet {
	uint8 id;
	uint8 len;
	uint8 count;	/* TBTTs until beacon interval in quiet starts */
	uint8 period;	/* Beacon intervals between periodic quiet periods ? */
	uint16 duration;	/* Length of quiet period, in TU's */
	uint16 offset;	/* TU's offset from TBTT in Count field */
} PACKED;
typedef struct dot11_quiet dot11_quiet_t;

struct chan_map_tuple {
	uint8 channel;
	uint8 map;
} PACKED;
typedef struct chan_map_tuple chan_map_tuple_t;

struct dot11_ibss_dfs {
	uint8 id;
	uint8 len;
	uint8 eaddr[ETHER_ADDR_LEN];
	uint8 interval;
	chan_map_tuple_t map[1];
} PACKED;
typedef struct dot11_ibss_dfs dot11_ibss_dfs_t;

/* WME Elements */
#define WME_OUI			"\x00\x50\xf2"	/* WME OUI */
#define WME_VER			1	/* WME version */
#define WME_TYPE		2	/* WME type */
#define WME_SUBTYPE_IE		0	/* Information Element */
#define WME_SUBTYPE_PARAM_IE	1	/* Parameter Element */
#define WME_SUBTYPE_TSPEC	2	/* Traffic Specification */

/* WME Access Category Indices (ACIs) */
#define AC_BE			0	/* Best Effort */
#define AC_BK			1	/* Background */
#define AC_VI			2	/* Video */
#define AC_VO			3	/* Voice */
#define AC_COUNT		4	/* number of ACs */

typedef uint8 ac_bitmap_t;	/* AC bitmap of (1 << AC_xx) */

#define AC_BITMAP_NONE		0x0	/* No ACs */
#define AC_BITMAP_ALL		0xf	/* All ACs */
#define AC_BITMAP_TST(ab, ac)	(((ab) & (1 << (ac))) != 0)

/* WME Information Element (IE) */
struct wme_ie {
	uint8 oui[3];
	uint8 type;
	uint8 subtype;
	uint8 version;
	uint8 qosinfo;
} PACKED;
typedef struct wme_ie wme_ie_t;
#define WME_IE_LEN 7	/* WME IE length */

struct edcf_acparam {
	uint8	ACI;
	uint8	ECW;
	uint16  TXOP;		/* stored in network order (ls octet first) */
} PACKED;
typedef struct edcf_acparam edcf_acparam_t;

/* WME Parameter Element (PE) */
struct wme_param_ie {
	uint8 oui[3];
	uint8 type;
	uint8 subtype;
	uint8 version;
	uint8 qosinfo;
	uint8 rsvd;
	edcf_acparam_t acparam[AC_COUNT];
} PACKED;
typedef struct wme_param_ie wme_param_ie_t;
#define WME_PARAM_IE_LEN            24          /* WME Parameter IE length */

/* QoS Info field for IE as sent from AP */
#define WME_QI_AP_APSD_MASK         0x80        /* U-APSD Supported mask */
#define WME_QI_AP_APSD_SHIFT        7           /* U-APSD Supported shift */
#define WME_QI_AP_COUNT_MASK        0x0f        /* Parameter set count mask */
#define WME_QI_AP_COUNT_SHIFT       0           /* Parameter set count shift */

/* QoS Info field for IE as sent from STA */
#define WME_QI_STA_MAXSPLEN_MASK    0x60        /* Max Service Period Length mask */
#define WME_QI_STA_MAXSPLEN_SHIFT   5           /* Max Service Period Length shift */
#define WME_QI_STA_APSD_ALL_MASK    0xf         /* APSD all AC bits mask */
#define WME_QI_STA_APSD_ALL_SHIFT   0           /* APSD all AC bits shift */
#define WME_QI_STA_APSD_BE_MASK     0x8         /* APSD AC_BE mask */
#define WME_QI_STA_APSD_BE_SHIFT    3           /* APSD AC_BE shift */
#define WME_QI_STA_APSD_BK_MASK     0x4         /* APSD AC_BK mask */
#define WME_QI_STA_APSD_BK_SHIFT    2           /* APSD AC_BK shift */
#define WME_QI_STA_APSD_VI_MASK     0x2         /* APSD AC_VI mask */
#define WME_QI_STA_APSD_VI_SHIFT    1           /* APSD AC_VI shift */
#define WME_QI_STA_APSD_VO_MASK     0x1         /* APSD AC_VO mask */
#define WME_QI_STA_APSD_VO_SHIFT    0           /* APSD AC_VO shift */

/* ACI */
#define EDCF_AIFSN_MIN               1           /* AIFSN minimum value */
#define EDCF_AIFSN_MAX               15          /* AIFSN maximum value */
#define EDCF_AIFSN_MASK              0x0f        /* AIFSN mask */
#define EDCF_ACM_MASK                0x10        /* ACM mask */
#define EDCF_ACI_MASK                0x60        /* ACI mask */
#define EDCF_ACI_SHIFT               5           /* ACI shift */

/* ECW */
#define EDCF_ECW_MIN                 0           /* cwmin/cwmax exponent minimum value */
#define EDCF_ECW_MAX                 15          /* cwmin/cwmax exponent maximum value */
#define EDCF_ECW2CW(exp)             ((1 << (exp)) - 1)
#define EDCF_ECWMIN_MASK             0x0f        /* cwmin exponent form mask */
#define EDCF_ECWMAX_MASK             0xf0        /* cwmax exponent form mask */
#define EDCF_ECWMAX_SHIFT            4           /* cwmax exponent form shift */

/* TXOP */
#define EDCF_TXOP_MIN                0           /* TXOP minimum value */
#define EDCF_TXOP_MAX                65535       /* TXOP maximum value */
#define EDCF_TXOP2USEC(txop)         ((txop) << 5)

/* Default EDCF parameters that AP advertises for STA to use; WMM draft Table 12 */
#define EDCF_AC_BE_ACI_STA           0x03	/* STA ACI value for best effort AC */
#define EDCF_AC_BE_ECW_STA           0xA4	/* STA ECW value for best effort AC */
#define EDCF_AC_BE_TXOP_STA          0x0000	/* STA TXOP value for best effort AC */
#define EDCF_AC_BK_ACI_STA           0x27	/* STA ACI value for background AC */
#define EDCF_AC_BK_ECW_STA           0xA4	/* STA ECW value for background AC */
#define EDCF_AC_BK_TXOP_STA          0x0000	/* STA TXOP value for background AC */
#define EDCF_AC_VI_ACI_STA           0x42	/* STA ACI value for video AC */
#define EDCF_AC_VI_ECW_STA           0x43	/* STA ECW value for video AC */
#define EDCF_AC_VI_TXOP_STA          0x005e	/* STA TXOP value for video AC */
#define EDCF_AC_VO_ACI_STA           0x62	/* STA ACI value for audio AC */
#define EDCF_AC_VO_ECW_STA           0x32	/* STA ECW value for audio AC */
#define EDCF_AC_VO_TXOP_STA          0x002f	/* STA TXOP value for audio AC */

/* Default EDCF parameters that AP uses; WMM draft Table 14 */
#define EDCF_AC_BE_ACI_AP            0x03	/* AP ACI value for best effort AC */
#define EDCF_AC_BE_ECW_AP            0x64	/* AP ECW value for best effort AC */
#define EDCF_AC_BE_TXOP_AP           0x0000	/* AP TXOP value for best effort AC */
#define EDCF_AC_BK_ACI_AP            0x27	/* AP ACI value for background AC */
#define EDCF_AC_BK_ECW_AP            0xA4	/* AP ECW value for background AC */
#define EDCF_AC_BK_TXOP_AP           0x0000	/* AP TXOP value for background AC */
#define EDCF_AC_VI_ACI_AP            0x41	/* AP ACI value for video AC */
#define EDCF_AC_VI_ECW_AP            0x43	/* AP ECW value for video AC */
#define EDCF_AC_VI_TXOP_AP           0x005e	/* AP TXOP value for video AC */
#define EDCF_AC_VO_ACI_AP            0x61	/* AP ACI value for audio AC */
#define EDCF_AC_VO_ECW_AP            0x32	/* AP ECW value for audio AC */
#define EDCF_AC_VO_TXOP_AP           0x002f	/* AP TXOP value for audio AC */

struct dot11_qbss_load_ie {
	uint8 id; 			/* 11, DOT11_MNG_QBSS_LOAD_ID */
	uint8 length;
	uint16 station_count; 		/* total number of STAs associated */
	uint8 channel_utilization;	/* % of time, normalized to 255, QAP sensed medium busy */
	uint16 aac; 			/* available admission capacity */
} PACKED;
typedef struct dot11_qbss_load_ie dot11_qbss_load_ie_t;

/* nom_msdu_size */
#define FIXED_MSDU_SIZE 0x8000		/* MSDU size is fixed */
#define MSDU_SIZE_MASK	0x7fff		/* (Nominal or fixed) MSDU size */

/* surplus_bandwidth */
/* Represented as 3 bits of integer, binary point, 13 bits fraction */
#define	INTEGER_SHIFT	13	/* integer shift */
#define FRACTION_MASK	0x1FFF	/* fraction mask */

/* Management Notification Frame */
struct dot11_management_notification {
	uint8 category;			/* DOT11_ACTION_NOTIFICATION */
	uint8 action;
	uint8 token;
	uint8 status;
	uint8 data[1];			/* Elements */
} PACKED;
#define DOT11_MGMT_NOTIFICATION_LEN 4	/* Fixed length */

/* WME Action Codes */
#define WME_ADDTS_REQUEST	0	/* WME ADDTS request */
#define WME_ADDTS_RESPONSE	1	/* WME ADDTS response */
#define WME_DELTS_REQUEST	2	/* WME DELTS request */

/* WME Setup Response Status Codes */
#define WME_ADMISSION_ACCEPTED	0	/* WME admission accepted */
#define WME_INVALID_PARAMETERS	1	/* WME invalide parameters */
#define WME_ADMISSION_REFUSED	3	/* WME admission refused */

/* Macro to take a pointer to a beacon or probe response
 * body and return the char* pointer to the SSID info element
 */
#define BCN_PRB_SSID(body) ((char*)(body) + DOT11_BCN_PRB_LEN)

/* Authentication frame payload constants */
#define DOT11_OPEN_SYSTEM	0	/* d11 open authentication */
#define DOT11_SHARED_KEY	1	/* d11 shared authentication */
#define DOT11_CHALLENGE_LEN	128	/* d11 challenge text length */

/* Frame control macros */
#define FC_PVER_MASK		0x3	/* PVER mask */
#define FC_PVER_SHIFT		0	/* PVER shift */
#define FC_TYPE_MASK		0xC	/* type mask */
#define FC_TYPE_SHIFT		2	/* type shift */
#define FC_SUBTYPE_MASK		0xF0	/* subtype mask */
#define FC_SUBTYPE_SHIFT	4	/* subtype shift */
#define FC_TODS			0x100	/* to DS */
#define FC_TODS_SHIFT		8	/* to DS shift */
#define FC_FROMDS		0x200	/* from DS */
#define FC_FROMDS_SHIFT		9	/* from DS shift */
#define FC_MOREFRAG		0x400	/* more frag. */
#define FC_MOREFRAG_SHIFT	10	/* more frag. shift */
#define FC_RETRY		0x800	/* retry */
#define FC_RETRY_SHIFT		11	/* retry shift */
#define FC_PM			0x1000	/* PM */
#define FC_PM_SHIFT		12	/* PM shift */
#define FC_MOREDATA		0x2000	/* more data */
#define FC_MOREDATA_SHIFT	13	/* more data shift */
#define FC_WEP			0x4000	/* WEP */
#define FC_WEP_SHIFT		14	/* WEP shift */
#define FC_ORDER		0x8000	/* order */
#define FC_ORDER_SHIFT		15	/* order shift */

/* sequence control macros */
#define SEQNUM_SHIFT		4	/* seq. number shift */
#define SEQNUM_MAX		0x1000	/* max seqnum + 1 */
#define FRAGNUM_MASK		0xF	/* frag. number mask */

/* Frame Control type/subtype defs */

/* FC Types */
#define FC_TYPE_MNG		0	/* management type */
#define FC_TYPE_CTL		1	/* control type */
#define FC_TYPE_DATA		2	/* data type */

/* Management Subtypes */
#define FC_SUBTYPE_ASSOC_REQ		0	/* assoc. request */
#define FC_SUBTYPE_ASSOC_RESP		1	/* assoc. response */
#define FC_SUBTYPE_REASSOC_REQ		2	/* reassoc. request */
#define FC_SUBTYPE_REASSOC_RESP		3	/* reassoc. response */
#define FC_SUBTYPE_PROBE_REQ		4	/* probe request */
#define FC_SUBTYPE_PROBE_RESP		5	/* probe response */
#define FC_SUBTYPE_BEACON		8	/* beacon */
#define FC_SUBTYPE_ATIM			9	/* ATIM */
#define FC_SUBTYPE_DISASSOC		10	/* disassoc. */
#define FC_SUBTYPE_AUTH			11	/* authentication */
#define FC_SUBTYPE_DEAUTH		12	/* de-authentication */
#define FC_SUBTYPE_ACTION		13	/* action */
#define FC_SUBTYPE_ACTION_NOACK		14	/* action no-ack */

/* Control Subtypes */
#define FC_SUBTYPE_CTL_WRAPPER		7	/* Control Wrapper */
#define FC_SUBTYPE_BLOCKACK_REQ		8	/* Block Ack Req */
#define FC_SUBTYPE_BLOCKACK		9	/* Block Ack */
#define FC_SUBTYPE_PS_POLL		10	/* PS poll */
#define FC_SUBTYPE_RTS			11	/* RTS */
#define FC_SUBTYPE_CTS			12	/* CTS */
#define FC_SUBTYPE_ACK			13	/* ACK */
#define FC_SUBTYPE_CF_END		14	/* CF-END */
#define FC_SUBTYPE_CF_END_ACK		15	/* CF-END ACK */

/* Data Subtypes */
#define FC_SUBTYPE_DATA			0	/* Data */
#define FC_SUBTYPE_DATA_CF_ACK		1	/* Data + CF-ACK */
#define FC_SUBTYPE_DATA_CF_POLL		2	/* Data + CF-Poll */
#define FC_SUBTYPE_DATA_CF_ACK_POLL	3	/* Data + CF-Ack + CF-Poll */
#define FC_SUBTYPE_NULL			4	/* Null */
#define FC_SUBTYPE_CF_ACK		5	/* CF-Ack */
#define FC_SUBTYPE_CF_POLL		6	/* CF-Poll */
#define FC_SUBTYPE_CF_ACK_POLL		7	/* CF-Ack + CF-Poll */
#define FC_SUBTYPE_QOS_DATA		8	/* QoS Data */
#define FC_SUBTYPE_QOS_DATA_CF_ACK	9	/* QoS Data + CF-Ack */
#define FC_SUBTYPE_QOS_DATA_CF_POLL	10	/* QoS Data + CF-Poll */
#define FC_SUBTYPE_QOS_DATA_CF_ACK_POLL	11	/* QoS Data + CF-Ack + CF-Poll */
#define FC_SUBTYPE_QOS_NULL		12	/* QoS Null */
#define FC_SUBTYPE_QOS_CF_POLL		14	/* QoS CF-Poll */
#define FC_SUBTYPE_QOS_CF_ACK_POLL	15	/* QoS CF-Ack + CF-Poll */

/* Data Subtype Groups */
#define FC_SUBTYPE_ANY_QOS(s)		(((s) & 8) != 0)
#define FC_SUBTYPE_ANY_NULL(s)		(((s) & 4) != 0)
#define FC_SUBTYPE_ANY_CF_POLL(s)	(((s) & 2) != 0)
#define FC_SUBTYPE_ANY_CF_ACK(s)	(((s) & 1) != 0)

/* Type/Subtype Combos */
#define FC_KIND_MASK		(FC_TYPE_MASK | FC_SUBTYPE_MASK)	/* FC kind mask */

#define FC_KIND(t, s)	(((t) << FC_TYPE_SHIFT) | ((s) << FC_SUBTYPE_SHIFT))	/* FC kind */

#define FC_SUBTYPE(fc)	(((fc) & FC_SUBTYPE_MASK) >> FC_SUBTYPE_SHIFT)	/* Subtype from FC */
#define FC_TYPE(fc)	(((fc) & FC_TYPE_MASK) >> FC_TYPE_SHIFT)	/* Type from FC */

#define FC_ASSOC_REQ	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_ASSOC_REQ)	/* assoc. request */
#define FC_ASSOC_RESP	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_ASSOC_RESP)	/* assoc. response */
#define FC_REASSOC_REQ	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_REASSOC_REQ)	/* reassoc. request */
#define FC_REASSOC_RESP	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_REASSOC_RESP)	/* reassoc. response */
#define FC_PROBE_REQ	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_PROBE_REQ)	/* probe request */
#define FC_PROBE_RESP	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_PROBE_RESP)	/* probe response */
#define FC_BEACON	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_BEACON)		/* beacon */
#define FC_DISASSOC	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_DISASSOC)	/* disassoc */
#define FC_AUTH		FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_AUTH)		/* authentication */
#define FC_DEAUTH	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_DEAUTH)		/* deauthentication */
#define FC_ACTION	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_ACTION)		/* action */
#define FC_ACTION_NOACK	FC_KIND(FC_TYPE_MNG, FC_SUBTYPE_ACTION_NOACK)	/* action no-ack */

#define FC_CTL_WRAPPER	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_CTL_WRAPPER)	/* Control Wrapper */
#define FC_BLOCKACK_REQ	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_BLOCKACK_REQ)	/* Block Ack Req */
#define FC_BLOCKACK	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_BLOCKACK)	/* Block Ack */
#define FC_PS_POLL	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_PS_POLL)	/* PS poll */
#define FC_RTS		FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_RTS)		/* RTS */
#define FC_CTS		FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_CTS)		/* CTS */
#define FC_ACK		FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_ACK)		/* ACK */
#define FC_CF_END	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_CF_END)		/* CF-END */
#define FC_CF_END_ACK	FC_KIND(FC_TYPE_CTL, FC_SUBTYPE_CF_END_ACK)	/* CF-END ACK */

#define FC_DATA		FC_KIND(FC_TYPE_DATA, FC_SUBTYPE_DATA)		/* data */
#define FC_NULL_DATA	FC_KIND(FC_TYPE_DATA, FC_SUBTYPE_NULL)		/* null data */
#define FC_DATA_CF_ACK	FC_KIND(FC_TYPE_DATA, FC_SUBTYPE_DATA_CF_ACK)	/* data CF ACK */
#define FC_QOS_DATA	FC_KIND(FC_TYPE_DATA, FC_SUBTYPE_QOS_DATA)	/* QoS data */
#define FC_QOS_NULL	FC_KIND(FC_TYPE_DATA, FC_SUBTYPE_QOS_NULL)	/* QoS null */

/* QoS Control Field */

/* 802.1D Priority */
#define QOS_PRIO_SHIFT		0	/* QoS priority shift */
#define QOS_PRIO_MASK		0x0007	/* QoS priority mask */
#define QOS_PRIO(qos)		(((qos) & QOS_PRIO_MASK) >> QOS_PRIO_SHIFT)	/* QoS priority */

/* Traffic Identifier */
#define QOS_TID_SHIFT		0	/* QoS TID shift */
#define QOS_TID_MASK		0x000f	/* QoS TID mask */
#define QOS_TID(qos)		(((qos) & QOS_TID_MASK) >> QOS_TID_SHIFT)	/* QoS TID */

/* End of Service Period (U-APSD) */
#define QOS_EOSP_SHIFT		4	/* QoS End of Service Period shift */
#define QOS_EOSP_MASK		0x0010	/* QoS End of Service Period mask */
#define QOS_EOSP(qos)		(((qos) & QOS_EOSP_MASK) >> QOS_EOSP_SHIFT)	/* Qos EOSP */

/* Ack Policy */
#define QOS_ACK_NORMAL_ACK	0	/* Normal Ack */
#define QOS_ACK_NO_ACK		1	/* No Ack (eg mcast) */
#define QOS_ACK_NO_EXP_ACK	2	/* No Explicit Ack */
#define QOS_ACK_BLOCK_ACK	3	/* Block Ack */
#define QOS_ACK_SHIFT		5	/* QoS ACK shift */
#define QOS_ACK_MASK		0x0060	/* QoS ACK mask */
#define QOS_ACK(qos)		(((qos) & QOS_ACK_MASK) >> QOS_ACK_SHIFT)	/* QoS ACK */

/* A-MSDU flag */
#define QOS_AMSDU_SHIFT		7	/* AMSDU shift */
#define QOS_AMSDU_MASK		0x0080	/* AMSDU mask */

/* Management Frames */

/* Management Frame Constants */

/* Fixed fields */
#define DOT11_MNG_AUTH_ALGO_LEN		2	/* d11 management auth. algo. length */
#define DOT11_MNG_AUTH_SEQ_LEN		2	/* d11 management auth. seq. length */
#define DOT11_MNG_BEACON_INT_LEN	2	/* d11 management beacon interval length */
#define DOT11_MNG_CAP_LEN		2	/* d11 management cap. length */
#define DOT11_MNG_AP_ADDR_LEN		6	/* d11 management AP address length */
#define DOT11_MNG_LISTEN_INT_LEN	2	/* d11 management listen interval length */
#define DOT11_MNG_REASON_LEN		2	/* d11 management reason length */
#define DOT11_MNG_AID_LEN		2	/* d11 management AID length */
#define DOT11_MNG_STATUS_LEN		2	/* d11 management status length */
#define DOT11_MNG_TIMESTAMP_LEN		8	/* d11 management timestamp length */

/* DUR/ID field in assoc resp is 0xc000 | AID */
#define DOT11_AID_MASK			0x3fff	/* d11 AID mask */

/* Reason Codes */
#define DOT11_RC_RESERVED		0	/* d11 RC reserved */
#define DOT11_RC_UNSPECIFIED		1	/* Unspecified reason */
#define DOT11_RC_AUTH_INVAL		2	/* Previous authentication no longer valid */
#define DOT11_RC_DEAUTH_LEAVING		3	/* Deauthenticated because sending station
						 * is leaving (or has left) IBSS or ESS
						 */
#define DOT11_RC_INACTIVITY		4	/* Disassociated due to inactivity */
#define DOT11_RC_BUSY			5	/* Disassociated because AP is unable to handle
						 * all currently associated stations
						 */
#define DOT11_RC_INVAL_CLASS_2		6	/* Class 2 frame received from
						 * nonauthenticated station
						 */
#define DOT11_RC_INVAL_CLASS_3		7	/* Class 3 frame received from
						 *  nonassociated station
						 */
#define DOT11_RC_DISASSOC_LEAVING	8	/* Disassociated because sending station is
						 * leaving (or has left) BSS
						 */
#define DOT11_RC_NOT_AUTH		9	/* Station requesting (re)association is not
						 * authenticated with responding station
						 */
#define DOT11_RC_BAD_PC			10	/* Unacceptable power capability element */
#define DOT11_RC_BAD_CHANNELS		11	/* Unacceptable supported channels element */
/* 12 is unused */

/* 32-39 are QSTA specific reasons added in 11e */
#define DOT11_RC_UNSPECIFIED_QOS	32	/* unspecified QoS-related reason */
#define DOT11_RC_INSUFFCIENT_BW		33	/* QAP lacks sufficient bandwidth */
#define DOT11_RC_EXCESSIVE_FRAMES	34	/* excessive number of frames need ack */
#define DOT11_RC_TX_OUTSIDE_TXOP	35	/* transmitting outside the limits of txop */
#define DOT11_RC_LEAVING_QBSS		36	/* QSTA is leaving the QBSS (or restting) */
#define DOT11_RC_BAD_MECHANISM		37	/* does not want to use the mechanism */
#define DOT11_RC_SETUP_NEEDED		38	/* mechanism needs a setup */
#define DOT11_RC_TIMEOUT		39	/* timeout */

#define DOT11_RC_MAX			23	/* Reason codes > 23 are reserved */

/* Status Codes */
#define DOT11_SC_SUCCESS		0	/* Successful */
#define DOT11_SC_FAILURE		1	/* Unspecified failure */
#define DOT11_SC_CAP_MISMATCH		10	/* Cannot support all requested
						 * capabilities in the Capability
						 * Information field
						 */
#define DOT11_SC_REASSOC_FAIL		11	/* Reassociation denied due to inability
						 * to confirm that association exists
						 */
#define DOT11_SC_ASSOC_FAIL		12	/* Association denied due to reason
						 * outside the scope of this standard
						 */
#define DOT11_SC_AUTH_MISMATCH		13	/* Responding station does not support
						 * the specified authentication
						 * algorithm
						 */
#define DOT11_SC_AUTH_SEQ		14	/* Received an Authentication frame
						 * with authentication transaction
						 * sequence number out of expected
						 * sequence
						 */
#define DOT11_SC_AUTH_CHALLENGE_FAIL	15	/* Authentication rejected because of
						 * challenge failure
						 */
#define DOT11_SC_AUTH_TIMEOUT		16	/* Authentication rejected due to timeout
						 * waiting for next frame in sequence
						 */
#define DOT11_SC_ASSOC_BUSY_FAIL	17	/* Association denied because AP is
						 * unable to handle additional
						 * associated stations
						 */
#define DOT11_SC_ASSOC_RATE_MISMATCH	18	/* Association denied due to requesting
						 * station not supporting all of the
						 * data rates in the BSSBasicRateSet
						 * parameter
						 */
#define DOT11_SC_ASSOC_SHORT_REQUIRED	19	/* Association denied due to requesting
						 * station not supporting the Short
						 * Preamble option
						 */
#define DOT11_SC_ASSOC_PBCC_REQUIRED	20	/* Association denied due to requesting
						 * station not supporting the PBCC
						 * Modulation option
						 */
#define DOT11_SC_ASSOC_AGILITY_REQUIRED	21	/* Association denied due to requesting
						 * station not supporting the Channel
						 * Agility option
						 */
#define DOT11_SC_ASSOC_SPECTRUM_REQUIRED	22	/* Association denied because Spectrum
							 * Management capability is required.
							 */
#define DOT11_SC_ASSOC_BAD_POWER_CAP	23	/* Association denied because the info
						 * in the Power Cap element is
						 * unacceptable.
						 */
#define DOT11_SC_ASSOC_BAD_SUP_CHANNELS	24	/* Association denied because the info
						 * in the Supported Channel element is
						 * unacceptable
						 */
#define DOT11_SC_ASSOC_SHORTSLOT_REQUIRED	25	/* Association denied due to requesting
							 * station not supporting the Short Slot
							 * Time option
							 */
#define DOT11_SC_ASSOC_ERPBCC_REQUIRED	26	/* Association denied due to requesting
						 * station not supporting the ER-PBCC
						 * Modulation option
						 */
#define DOT11_SC_ASSOC_DSSOFDM_REQUIRED	27	/* Association denied due to requesting
						 * station not supporting the DSS-OFDM
						 * option
						 */

#define	DOT11_SC_DECLINED		37	/* request declined */
#define	DOT11_SC_INVALID_PARAMS		38	/* One or more params have invalid values */

/* Info Elts, length of INFORMATION portion of Info Elts */
#define DOT11_MNG_DS_PARAM_LEN			1	/* d11 management DS parameter length */
#define DOT11_MNG_IBSS_PARAM_LEN		2	/* d11 management IBSS parameter length */

/* TIM Info element has 3 bytes fixed info in INFORMATION field,
 * followed by 1 to 251 bytes of Partial Virtual Bitmap
 */
#define DOT11_MNG_TIM_FIXED_LEN			3	/* d11 management TIM fixed length */
#define DOT11_MNG_TIM_DTIM_COUNT		0	/* d11 management DTIM count */
#define DOT11_MNG_TIM_DTIM_PERIOD		1	/* d11 management DTIM period */
#define DOT11_MNG_TIM_BITMAP_CTL		2	/* d11 management TIM BITMAP control  */
#define DOT11_MNG_TIM_PVB			3	/* d11 management TIM PVB */

/* TLV defines */
#define TLV_TAG_OFF		0	/* tag offset */
#define TLV_LEN_OFF		1	/* length offset */
#define TLV_HDR_LEN		2	/* header length */
#define TLV_BODY_OFF		2	/* body offset */

/* Management Frame Information Element IDs */
#define DOT11_MNG_SSID_ID			0	/* d11 management SSID id */
#define DOT11_MNG_RATES_ID			1	/* d11 management rates id */
#define DOT11_MNG_FH_PARMS_ID			2	/* d11 management FH parameter id */
#define DOT11_MNG_DS_PARMS_ID			3	/* d11 management DS parameter id */
#define DOT11_MNG_CF_PARMS_ID			4	/* d11 management CF parameter id */
#define DOT11_MNG_TIM_ID			5	/* d11 management TIM id */
#define DOT11_MNG_IBSS_PARMS_ID			6	/* d11 management IBSS parameter id */
#define DOT11_MNG_COUNTRY_ID			7	/* d11 management country id */
#define DOT11_MNG_HOPPING_PARMS_ID		8	/* d11 management hopping parameter id */
#define DOT11_MNG_HOPPING_TABLE_ID		9	/* d11 management hopping table id */
#define DOT11_MNG_REQUEST_ID			10	/* d11 management request id */
#define DOT11_MNG_QBSS_LOAD_ID 			11	/* d11 management QBSS Load id */
#define DOT11_MNG_CHALLENGE_ID			16	/* d11 management chanllenge id */
#define DOT11_MNG_PWR_CONSTRAINT_ID		32	/* 11H PowerConstraint */
#define DOT11_MNG_PWR_CAP_ID			33	/* 11H PowerCapability */
#define DOT11_MNG_TPC_REQUEST_ID 		34	/* 11H TPC Request */
#define DOT11_MNG_TPC_REPORT_ID			35	/* 11H TPC Report */
#define DOT11_MNG_SUPP_CHANNELS_ID		36	/* 11H Supported Channels */
#define DOT11_MNG_CHANNEL_SWITCH_ID		37	/* 11H ChannelSwitch Announcement */
#define DOT11_MNG_MEASURE_REQUEST_ID		38	/* 11H MeasurementRequest */
#define DOT11_MNG_MEASURE_REPORT_ID		39	/* 11H MeasurementReport */
#define DOT11_MNG_QUIET_ID			40	/* 11H Quiet */
#define DOT11_MNG_IBSS_DFS_ID			41	/* 11H IBSS_DFS */
#define DOT11_MNG_ERP_ID			42	/* d11 management ERP id */
#define DOT11_MNG_TS_DELAY_ID			43	/* d11 management TS Delay id */
#define	DOT11_MNG_HT_CAP			45	/* d11 mgmt HT cap id */
#define DOT11_MNG_NONERP_ID			47	/* d11 management NON-ERP id */
#define DOT11_MNG_RSN_ID			48	/* d11 management RSN id */
#define DOT11_MNG_EXT_RATES_ID			50	/* d11 management ext. rates id */
#define DOT11_MNG_EXT_CSA_ID			60	/* d11 Extended CSA */
#define	DOT11_MNG_HT_ADD			61	/* d11 mgmt additional HT info */
#define	DOT11_MNG_EXT_CHANNEL_OFFSET		62	/* d11 mgmt ext channel offset */
#define	DOT11_MNG_EXT_CAP			127	/* d11 mgmt ext capability */
#define DOT11_MNG_WPA_ID			221	/* d11 management WPA id */
#define DOT11_MNG_PROPR_ID			221	/* d11 management proprietary id */

/* Rate element Basic flag and rate mask */
#define DOT11_RATE_BASIC			0x80	/* flag for a Basic Rate */
#define DOT11_RATE_MASK				0x7F	/* mask for numeric part of rate */

/* ERP info element bit values */
#define DOT11_MNG_ERP_LEN			1	/* ERP is currently 1 byte long */
#define DOT11_MNG_NONERP_PRESENT		0x01	/* NonERP (802.11b) STAs are present
							 *in the BSS
							 */
#define DOT11_MNG_USE_PROTECTION		0x02	/* Use protection mechanisms for
							 *ERP-OFDM frames
							 */
#define DOT11_MNG_BARKER_PREAMBLE		0x04	/* Short Preambles: 0 == allowed,
							 * 1 == not allowed
							 */
/* TS Delay element offset & size */
#define DOT11_MGN_TS_DELAY_LEN		4	/* length of TS DELAY IE */
#define TS_DELAY_FIELD_SIZE			4	/* TS DELAY field size */

/* Capability Information Field */
#define DOT11_CAP_ESS				0x0001	/* d11 cap. ESS */
#define DOT11_CAP_IBSS				0x0002	/* d11 cap. IBSS */
#define DOT11_CAP_POLLABLE			0x0004	/* d11 cap. pollable */
#define DOT11_CAP_POLL_RQ			0x0008	/* d11 cap. poll request */
#define DOT11_CAP_PRIVACY			0x0010	/* d11 cap. privacy */
#define DOT11_CAP_SHORT				0x0020	/* d11 cap. short */
#define DOT11_CAP_PBCC				0x0040	/* d11 cap. PBCC */
#define DOT11_CAP_AGILITY			0x0080	/* d11 cap. agility */
#define DOT11_CAP_SPECTRUM			0x0100	/* d11 cap. spectrum */
#define DOT11_CAP_SHORTSLOT			0x0400	/* d11 cap. shortslot */
#define DOT11_CAP_CCK_OFDM			0x2000	/* d11 cap. CCK/OFDM */

/* Extended Capability Information Field */
#define DOT11_EXT_CAP_HT_IE_SUPPORT	0x01	/* support for info xchg action frame */

/* Action Frame Constants */
#define DOT11_ACTION_HDR_LEN		2	/* action frame header length */
#define DOT11_ACTION_CAT_ERR_MASK	0x80	/* d11 action category error mask */
#define DOT11_ACTION_CAT_MASK		0x7F	/* d11 action category mask */
#define DOT11_ACTION_CAT_SPECT_MNG	0x00	/* d11 action category spectrum management */
#define DOT11_ACTION_CAT_BLOCKACK	0x03	/* d11 action category block ack */
#define DOT11_ACTION_CAT_HT		0x07	/* d11 action category for HT */
#define DOT11_ACTION_NOTIFICATION	0x11	/* 17 */

#define DOT11_ACTION_ID_M_REQ		0	/* d11 action measurement request */
#define DOT11_ACTION_ID_M_REP		1	/* d11 action measurement response */
#define DOT11_ACTION_ID_TPC_REQ		2	/* d11 action TPC request */
#define DOT11_ACTION_ID_TPC_REP		3	/* d11 action TPC response */
#define DOT11_ACTION_ID_CHANNEL_SWITCH	4	/* d11 action channel switch */
#define DOT11_ACTION_ID_EXT_CSA		5	/* d11 extened CSA for 11n */

/* HT action ids */
#define DOT11_ACTION_ID_HT_CH_WIDTH	0	/* notify channel width action id */
#define DOT11_ACTION_ID_HT_MIMO_PS	1	/* mimo ps action id */
#define DOT11_ACTION_ID_HT_INFO_XCHG	8	/* HT Information Exchange action id */

/* Block Ack action types */
#define DOT11_BA_ACTION_ADDBA_REQ	0	/* ADDBA Req action frame type */
#define DOT11_BA_ACTION_ADDBA_RESP	1	/* ADDBA Resp action frame type */
#define DOT11_BA_ACTION_DELBA		2	/* DELBA action frame type */

/* ADDBA action parameters */
#define DOT11_ADDBA_PARAM_AMSDU_SUP	0x0001	/* AMSDU supported under BA */
#define DOT11_ADDBA_PARAM_POLICY_MASK	0x0002	/* policy mask(ack vs delayed) */
#define DOT11_ADDBA_PARAM_POLICY_SHIFT	1	/* policy shift */
#define DOT11_ADDBA_PARAM_TID_MASK	0x003c	/* tid mask */
#define DOT11_ADDBA_PARAM_TID_SHIFT	2	/* tid shift */
#define DOT11_ADDBA_PARAM_BSIZE_MASK	0xffc0	/* buffer size mask */
#define DOT11_ADDBA_PARAM_BSIZE_SHIFT	6	/* buffer size shift */

#define DOT11_ADDBA_POLICY_DELAYED	0	/* delayed BA policy */
#define DOT11_ADDBA_POLICY_IMMEDIATE	1	/* immediate BA policy */

struct dot11_addba_req {
	uint8 category;				/* category of action frame (3) */
	uint8 action;				/* action: addba req */
	uint8 token;				/* identifier */
	uint16 addba_param_set;			/* parameter set */
	uint16 timeout;				/* timeout in seconds */
	uint16 start_seqnum;			/* starting sequence number */
}PACKED;
typedef struct dot11_addba_req dot11_addba_req_t;
#define DOT11_ADDBA_REQ_LEN		9	/* length of addba req frame */

struct dot11_addba_resp {
	uint8 category;				/* category of action frame (3) */
	uint8 action;				/* action: addba resp */
	uint8 token;				/* identifier */
	uint16 status;				/* status of add request */
	uint16 addba_param_set;			/* negotiated parameter set */
	uint16 timeout;				/* negotiated timeout in seconds */
}PACKED;
typedef struct dot11_addba_resp dot11_addba_resp_t;
#define DOT11_ADDBA_RESP_LEN		9	/* length of addba resp frame */

/* DELBA action parameters */
#define DOT11_DELBA_PARAM_INIT_MASK	0x0800	/* initiator mask */
#define DOT11_DELBA_PARAM_INIT_SHIFT	11	/* initiator shift */
#define DOT11_DELBA_PARAM_TID_MASK	0xf000	/* tid mask */
#define DOT11_DELBA_PARAM_TID_SHIFT	12	/* tid shift */

struct dot11_delba {
	uint8 category;				/* category of action frame (3) */
	uint8 action;				/* action: addba req */
	uint16 delba_param_set;			/* paarmeter set */
	uint16 reason;				/* reason for dellba */
}PACKED;
typedef struct dot11_delba dot11_delba_t;
#define DOT11_DELBA_LEN			6	/* length of delba frame */

/* MLME Enumerations */
#define DOT11_BSSTYPE_INFRASTRUCTURE		0	/* d11 infrastructure */
#define DOT11_BSSTYPE_INDEPENDENT		1	/* d11 independent */
#define DOT11_BSSTYPE_ANY			2	/* d11 any BSS type */
#define DOT11_SCANTYPE_ACTIVE			0	/* d11 scan active */
#define DOT11_SCANTYPE_PASSIVE			1	/* d11 scan passive */

/* 802.11 BRCM "Compromise" Pre N constants */
#define PREN_PREAMBLE		24	/* green field preamble time */
#define PREN_MM_EXT		8	/* extra mixed mode preamble time */
#define PREN_PREAMBLE_EXT	4	/* extra preamble (multiply by unique_streams-1) */

/* 802.11 N PHY constants */
#define NPHY_RIFS_TIME		2	/* NPHY RIFS time */

/* 802.11 A PHY constants */
#define APHY_SLOT_TIME		9	/* APHY slot time */
#define APHY_SIFS_TIME		16	/* APHY SIFS time */
#define APHY_DIFS_TIME		(APHY_SIFS_TIME + (2 * APHY_SLOT_TIME))	/* APHY DIFS time */
#define APHY_PREAMBLE_TIME	16	/* APHY preamble time */
#define APHY_SIGNAL_TIME	4	/* APHY signal time */
#define APHY_SYMBOL_TIME	4	/* APHY symbol time */
#define APHY_SERVICE_NBITS	16	/* APHY service nbits */
#define APHY_TAIL_NBITS		6	/* APHY tail nbits */
#define	APHY_CWMIN		15	/* APHY cwmin */

/* 802.11 B PHY constants */
#define BPHY_SLOT_TIME		20	/* BPHY slot time */
#define BPHY_SIFS_TIME		10	/* BPHY SIFS time */
#define BPHY_DIFS_TIME		50	/* BPHY DIFS time */
#define BPHY_PLCP_TIME		192	/* BPHY PLCP time */
#define BPHY_PLCP_SHORT_TIME	96	/* BPHY PLCP short time */
#define	BPHY_CWMIN		31	/* BPHY cwmin */

/* 802.11 G constants */
#define DOT11_OFDM_SIGNAL_EXTENSION	6	/* d11 OFDM signal extension */

#define PHY_CWMAX		1023	/* PHY cwmax */

#define	DOT11_MAXNUMFRAGS	16	/* max # fragments per MSDU */

/* dot11Counters Table - 802.11 spec., Annex D */
typedef struct d11cnt {
	uint32		txfrag;		/* dot11TransmittedFragmentCount */
	uint32		txmulti;	/* dot11MulticastTransmittedFrameCount */
	uint32		txfail;		/* dot11FailedCount */
	uint32		txretry;	/* dot11RetryCount */
	uint32		txretrie;	/* dot11MultipleRetryCount */
	uint32		rxdup;		/* dot11FrameduplicateCount */
	uint32		txrts;		/* dot11RTSSuccessCount */
	uint32		txnocts;	/* dot11RTSFailureCount */
	uint32		txnoack;	/* dot11ACKFailureCount */
	uint32		rxfrag;		/* dot11ReceivedFragmentCount */
	uint32		rxmulti;	/* dot11MulticastReceivedFrameCount */
	uint32		rxcrc;		/* dot11FCSErrorCount */
	uint32		txfrmsnt;	/* dot11TransmittedFrameCount */
	uint32		rxundec;	/* dot11WEPUndecryptableCount */
} d11cnt_t;

/* BRCM OUI */
#define BRCM_OUI		"\x00\x10\x18"	/* Broadcom OUI */

/* OUI for BRCM proprietary IE */
#define BRCM_PROP_OUI		"\x00\x90\x4C"	/* Broadcom proprietary OUI */

/* BRCM info element */
struct brcm_ie {
	uint8	id;		/* IE ID, 221, DOT11_MNG_PROPR_ID */
	uint8	len;		/* IE length */
	uint8	oui[3];		/* Proprietary OUI, BRCM_OUI */
	uint8	ver;		/* type/ver of this IE */
	uint8	assoc;		/* # of assoc STAs */
	uint8	flags;		/* misc flags */
	uint8	flags1;		/* misc flags */
	uint16	amsdu_mtu_pref;	/* preferred A-MSDU MTU */
} PACKED;
typedef	struct brcm_ie brcm_ie_t;
#define BRCM_IE_LEN		11	/* BRCM IE length */
#define BRCM_IE_VER		2	/* BRCM IE version */
#define BRCM_IE_LEGACY_AES_VER	1	/* BRCM IE legacy AES version */

/* brcm_ie flags */
#define	BRF_ABCAP		0x1	/* afterburner capable */
#define	BRF_ABRQRD		0x2	/* afterburner requested */
#define	BRF_LZWDS		0x4	/* lazy wds enabled */
#define	BRF_BLOCKACK		0x8	/* BlockACK capable */
#define BRF_ABCOUNTER_MASK	0xf0	/* afterburner wds "state" counter */
#define BRF_ABCOUNTER_SHIFT	4	/* offset of afterburner wds "state" counter */

/* brcm_ie flags1 */
#define	BRF1_AMSDU		0x1	/* A-MSDU capable */
#define	BRF1_DPT		0x2	/* DPT capable */

#define AB_WDS_TIMEOUT_MAX	15	/* afterburner wds Max count indicating not
					 * locally capable
					 */
#define AB_WDS_TIMEOUT_MIN	1	/* afterburner wds, use zero count as indicating
					 * "downrev"
					 */

/* EWC definitions */
#define MCSSET_LEN	16	/* 16-bits per 8-bit set to give 128-bits bitmap of MCS Index */
#define MAX_MCS_NUM	(128)	/* max mcs number = 128 */

struct ewc_cap_ie {
	uint16	cap;
	uint8	params;
	uint8	supp_mcs[MCSSET_LEN];
	uint16	ext_htcap;
	uint32	txbf_cap;
	uint8	as_cap;
} PACKED;
typedef struct ewc_cap_ie ewc_cap_ie_t;

/* CAP IE: EWC 1.0 spec. simply stole a 802.11 IE, we use our prop. IE until this is resolved */
/* the capability IE is primarily used to convey this nodes abilities */
struct ewc_prop_cap_ie {
	uint8	id;		/* IE ID, 221, DOT11_MNG_PROPR_ID */
	uint8	len;		/* IE length */
	uint8	oui[3];		/* Proprietary OUI, BRCM_PROP_OUI */
	uint8	type;           /* type inidicates what follows */
	ewc_cap_ie_t cap_ie;
} PACKED;
typedef struct ewc_prop_cap_ie ewc_prop_cap_ie_t;
#define EWC_PROP_IE_OVERHEAD	4	/* overhead bytes for prop oui ie */
#define EWC_CAP_IE_LEN	26	
#define EWC_CAP_IE_TYPE	51      

#define EWC_CAP_LDPC_CODING	0x0001	/* Support for rx of LDPC coded pkts */
#define EWC_CAP_40MHZ		0x0002  /* FALSE:20Mhz, TRUE:20/40MHZ supported */
#define EWC_CAP_MIMO_PS_MASK	0x000C  /* Mimo PS mask */
#define EWC_CAP_MIMO_PS_SHIFT	0x0002	/* Mimo PS shift */
#define EWC_CAP_MIMO_PS_OFF	0x0003	/* Mimo PS, no restriction */
#define EWC_CAP_MIMO_PS_RTS	0x0001	/* Mimo PS, send RTS/CTS around MIMO frames */
#define EWC_CAP_MIMO_PS_ON	0x0000	/* Mimo PS, MIMO disallowed */
#define EWC_CAP_GF		0x0010	/* Greenfield preamble support */
#define EWC_CAP_SHORT_GI_20	0x0020	/* 20MHZ short guard interval support */
#define EWC_CAP_SHORT_GI_40	0x0040	/* 40Mhz short guard interval support */
#define EWC_CAP_TX_STBC		0x0080	/* Tx STBC support */
#define EWC_CAP_RX_STBC_MASK	0x0300	/* Rx STBC mask */
#define EWC_CAP_RX_STBC_SHIFT	8	/* Rx STBC shift */
#define EWC_CAP_DELAYED_BA	0x0400	/* delayed BA support */
#define EWC_CAP_MAX_AMSDU	0x0800	/* Max AMSDU size in bytes , 0=3839, 1=7935 */
#define EWC_CAP_DSSS_CCK	0x1000	/* DSSS/CCK supported by the BSS */
#define EWC_CAP_PSMP		0x2000	/* Power Save Multi Poll support */
#define EWC_CAP_40MHZ_INTOLERANT 0x4000	/* 40MHz Intolerant */
#define EWC_CAP_LSIG_TXOP	0x8000	/* L-SIG TXOP protection support */

#define EWC_CAP_RX_STBC_NO		0x0	/* no rx STBC support */
#define EWC_CAP_RX_STBC_ONE_STREAM	0x1	/* rx STBC support of 1 spatial stream */
#define EWC_CAP_RX_STBC_TWO_STREAM	0x2	/* rx STBC support of 1-2 spatial streams */
#define EWC_CAP_RX_STBC_THREE_STREAM	0x3	/* rx STBC support of 1-3 spatial streams */

#define EWC_MAX_AMSDU		7935	/* max amsdu size (bytes) per the EWC spec */
#define EWC_MIN_AMSDU		3835	/* min amsdu size (bytes) per the EWC spec */

#define EWC_PARAMS_RX_FACTOR_MASK	0x03	/* ampdu rcv factor mask */
#define EWC_PARAMS_DENSITY_MASK		0x1C	/* ampdu density mask */
#define EWC_PARAMS_DENSITY_SHIFT	2	/* ampdu density shift */

/* EWC/AMPDU specific define */
#define AMPDU_MAX_MPDU_DENSITY	7	/* max mpdu density; in 1/8 usec units */
#define AMPDU_MAX_RX_FACTOR	3	/* max rcv ampdu len (64kb) */
#define AMPDU_RX_FACTOR_BASE	8*1024	/* ampdu factor base for rx len */
#define AMPDU_DELIMITER_LEN	4	/* length of ampdu delimiter */

struct ewc_add_ie {
	uint8	ctl_ch;			/* control channel number */
	uint8	byte1;			/* ext ch,rec. ch. width, RIFS support */
	uint16	opmode;			/* operation mode */
	uint16	misc_bits;		/* misc bits */
	uint8	basic_mcs[MCSSET_LEN];  /* required MCS set */
} PACKED;
typedef struct ewc_add_ie ewc_add_ie_t;

/* ADD IE: EWC 1.0 spec. simply stole a 802.11 IE, we use our prop. IE until this is resolved */
/* the additional IE is primarily used to convey the current BSS configuration */
struct ewc_prop_add_ie {
	uint8	id;		/* IE ID, 221, DOT11_MNG_PROPR_ID */
	uint8	len;		/* IE length */
	uint8	oui[3];		/* Proprietary OUI, BRCM_PROP_OUI */
	uint8	type;		/* indicates what follows */
	ewc_add_ie_t add_ie;
} PACKED;
typedef struct ewc_prop_add_ie ewc_prop_add_ie_t;

#define EWC_ADD_IE_LEN	22	
#define EWC_ADD_IE_TYPE	52	

/* byte1 defn's */
#define EWC_BW_ANY		0x04	/* set, STA can use 20 or 40MHz */
#define EWC_RIFS_PERMITTED     	0x08	/* RIFS allowed */

/* opmode defn's */
#define EWC_OPMODE_MASK	        0x0003	/* protection mode mask */
#define EWC_OPMODE_SHIFT	0	/* protection mode shift */
#define EWC_OPMODE_PURE		0x0000	/* protection mode PURE */
#define EWC_OPMODE_OPTIONAL	0x0001	/* protection mode optional */
#define EWC_OPMODE_HT20IN40	0x0002	/* protection mode 20MHz HT in 40MHz BSS */
#define EWC_OPMODE_MIXED	0x0003	/* protection mode Mixed Mode */
#define EWC_OPMODE_NONGF	0x0004	/* protection mode non-GF */
#define DOT11N_TXBURST		0x0008	/* Tx burst limit */
#define DOT11N_OBSS_NONHT	0x0010	/* OBSS Non-HT STA present */

/* misc_bites defn's */
#define EWC_BASIC_STBC_MCS	0x007f	/* basic STBC MCS */
#define EWC_DUAL_STBC_PROT	0x0080	/* Dual STBC Protection */
#define EWC_SECOND_BCN		0x0100	/* Secondary beacon support */
#define EWC_LSIG_TXOP		0x0200	/* L-SIG TXOP Protection full support */
#define EWC_PCO_ACTIVE		0x0400	/* PCO active */
#define EWC_PCO_PHASE		0x0800	/* PCO phase */

/* Tx Burst Limits */
#define DOT11N_2G_TXBURST_LIMIT	6160	/* 2G band Tx burst limit per 802.11n Draft 1.10 (usec) */
#define DOT11N_5G_TXBURST_LIMIT	3080	/* 5G band Tx burst limit per 802.11n Draft 1.10 (usec) */

/* Macros for opmode */
#define GET_EWC_OPMODE(add_ie)		((ltoh16_ua(&add_ie->opmode) & EWC_OPMODE_MASK) \
					>> EWC_OPMODE_SHIFT)
#define EWC_MIXEDMODE_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & EWC_OPMODE_MASK) \
					== EWC_OPMODE_MIXED)	/* mixed mode present */
#define EWC_HT20_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & EWC_OPMODE_MASK) \
					== EWC_OPMODE_HT20IN40)	/* 20MHz HT present */
#define EWC_OPTIONAL_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & EWC_OPMODE_MASK) \
					== EWC_OPMODE_OPTIONAL)	/* Optional protection present */
#define EWC_USE_PROTECTION(add_ie)	(EWC_HT20_PRESENT((add_ie)) || \
					EWC_MIXEDMODE_PRESENT((add_ie))) /* use protection */
#define EWC_NONGF_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & EWC_OPMODE_NONGF) \
					== EWC_OPMODE_NONGF)	/* non-GF present */
#define DOT11N_TXBURST_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & DOT11N_TXBURST) \
					== DOT11N_TXBURST)	/* Tx Burst present */
#define DOT11N_OBSS_NONHT_PRESENT(add_ie)	((ltoh16_ua(&add_ie->opmode) & DOT11N_OBSS_NONHT) \
					== DOT11N_OBSS_NONHT)	/* OBSS Non-HT present */

/* Vendor IE structure */
struct vndr_ie {
	uchar id;
	uchar len;
	uchar oui [3];
	uchar data [1]; 	/* Variable size data */
} PACKED;
typedef struct vndr_ie vndr_ie_t;

#define VNDR_IE_HDR_LEN		2	/* id + len field */
#define VNDR_IE_MIN_LEN		3	/* size of the oui field */
#define VNDR_IE_MAX_LEN		256	/* verdor IE max length */

/* WPA definitions */
#define WPA_VERSION		1	/* WPA version */
#define WPA_OUI			"\x00\x50\xF2"	/* WPA OUI */

#define WPA2_VERSION		1	/* WPA2 version */
#define WPA2_VERSION_LEN	2	/* WAP2 version length */
#define WPA2_OUI		"\x00\x0F\xAC"	/* WPA2 OUI */

#define WPA_OUI_LEN	3	/* WPA OUI length */

/* RSN authenticated key managment suite */
#define RSN_AKM_NONE		0	/* None (IBSS) */
#define RSN_AKM_UNSPECIFIED	1	/* Over 802.1x */
#define RSN_AKM_PSK		2	/* Pre-shared Key */

/* Key related defines */
#define DOT11_MAX_DEFAULT_KEYS	4	/* number of default keys */
#define DOT11_MAX_KEY_SIZE	32	/* max size of any key */
#define DOT11_MAX_IV_SIZE	16	/* max size of any IV */
#define DOT11_EXT_IV_FLAG	(1<<5)	/* flag to indicate IV is > 4 bytes */

#define WEP1_KEY_SIZE		5	/* max size of any WEP key */
#define WEP1_KEY_HEX_SIZE	10	/* size of WEP key in hex. */
#define WEP128_KEY_SIZE		13	/* max size of any WEP key */
#define WEP128_KEY_HEX_SIZE	26	/* size of WEP key in hex. */
#define TKIP_MIC_SIZE		8	/* size of TKIP MIC */
#define TKIP_EOM_SIZE		7	/* max size of TKIP EOM */
#define TKIP_EOM_FLAG		0x5a	/* TKIP EOM flag byte */
#define TKIP_KEY_SIZE		32	/* size of any TKIP key */
#define TKIP_MIC_AUTH_TX	16	/* offset to Authenticator MIC TX key */
#define TKIP_MIC_AUTH_RX	24	/* offset to Authenticator MIC RX key */
#define TKIP_MIC_SUP_RX		TKIP_MIC_AUTH_TX	/* offset to Supplicant MIC RX key */
#define TKIP_MIC_SUP_TX		TKIP_MIC_AUTH_RX	/* offset to Supplicant MIC TX key */
#define AES_KEY_SIZE		16	/* size of AES key */

#undef PACKED
#if !defined(__GNUC__)
#pragma pack()
#endif

#endif /* _802_11_H_ */
