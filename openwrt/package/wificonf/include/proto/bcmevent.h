/*
 * Broadcom Event  protocol definitions
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 *
 * Dependencies: proto/bcmeth.h
 *
 */

/*
 * Broadcom Ethernet Events protocol defines 
 *
 */

#ifndef _BCMEVENT_H_
#define _BCMEVENT_H_

/* enable structure packing */
#if defined(__GNUC__)
#define	PACKED	__attribute__((packed))
#else
#pragma pack(1)
#define	PACKED
#endif



#define BCM_EVENT_MSG_VERSION	1
#define BCM_MSG_IFNAME_MAX		16


/* flags */
#define WLC_EVENT_MSG_LINK		0x01
#define WLC_EVENT_MSG_FLUSHTXQ	0x02
#define WLC_EVENT_MSG_GROUP		0x04



/* theses fields are stored in network order */
typedef struct  
{
	uint16	version;
	uint16	flags;			/* see flags below */
	uint32	event_type;		/* Message (see below) */
	uint32	status;			/* Status code (see below) */
	uint32	reason;			/* Reason code (if applicable) */
	uint32	auth_type;		/* WLC_E_AUTH */
	uint32	datalen;		/* data buf */
	struct ether_addr	addr;	/* Station address (if applicable) */
	char	ifname[BCM_MSG_IFNAME_MAX]; /* name of the packet incoming interface*/
} PACKED wl_event_msg_t;

/* used by driver msgs */
typedef struct bcm_event {
	struct ether_header eth;
	bcmeth_hdr_t		bcm_hdr; 
	wl_event_msg_t		event;
	/* data portion follows */
} PACKED bcm_event_t;

#define BCM_MSG_LEN	(sizeof(bcm_event_t) - sizeof(bcmeth_hdr_t) - sizeof(struct ether_header))



/* Event messages */
#define WLC_E_SET_SSID			0	/* indicates status of set SSID */
#define WLC_E_JOIN			1	/* differentiates join IBSS from found (WLC_E_START) IBSS */
#define WLC_E_START			2	/* STA founded an IBSS or AP started a BSS */
#define WLC_E_AUTH			3	/* 802.11 AUTH request */
#define WLC_E_AUTH_IND			4	/* 802.11 AUTH indication */
#define WLC_E_DEAUTH			5	/* 802.11 DEAUTH request */
#define WLC_E_DEAUTH_IND		6	/* 802.11 DEAUTH indication */
#define WLC_E_ASSOC			7	/* 802.11 ASSOC request */
#define WLC_E_ASSOC_IND			8	/* 802.11 ASSOC indication */
#define WLC_E_REASSOC			9	/* 802.11 REASSOC request */
#define WLC_E_REASSOC_IND		10	/* 802.11 REASSOC indication */
#define WLC_E_DISASSOC			11	/* 802.11 DISASSOC request */
#define WLC_E_DISASSOC_IND		12	/* 802.11 DISASSOC indication */
#define WLC_E_QUIET_START		13	/* 802.11h Quiet period started */
#define WLC_E_QUIET_END			14	/* 802.11h Quiet period ended */
#define WLC_E_GOT_BEACONS		15	/* internal use: IBSS mode: saw another STA's BEACONS */
#define WLC_E_LINK			16	/* generic link indication */
#define WLC_E_MIC_ERROR			17	/* TKIP MIC error occurred */
#define WLC_E_NDIS_LINK			18	/* NDIS style link indication */
#define WLC_E_ROAM			19	/* roam attempt occurred: indicate status & reason for roam */
#define WLC_E_TXFAIL			20	/* change in dot11FailedCount (txfail) */
#define WLC_E_PMKID_CACHE		21	/* WPA2 pmkid cache indication */
#define WLC_E_RETROGRADE_TSF		22	/* current AP's TSF value went backward */
#define WLC_E_PRUNE				23	/* AP was pruned from join list for reason */
#define WLC_E_AUTOAUTH			24	/* report AutoAuth table entry match for join attempt */
#define WLC_E_EAPOL_MSG			25	/* Event encapsulating an EAPOL message */
#define WLC_E_LAST			26

/* Event status codes */
#define WLC_E_STATUS_SUCCESS		0
#define WLC_E_STATUS_FAIL		1
#define WLC_E_STATUS_TIMEOUT		2
#define WLC_E_STATUS_NO_NETWORKS	3
#define WLC_E_STATUS_ABORT		4
#define WLC_E_STATUS_NO_ACK		5
#define WLC_E_STATUS_UNSOLICITED	6
#define WLC_E_STATUS_ATTEMPT		7

/* roam reason codes */
#define WLC_E_REASON_LOW_RSSI			1
#define WLC_E_REASON_DEAUTH				2
#define WLC_E_REASON_DISASSOC			3
#define WLC_E_REASON_BCNS_LOST			4
#define WLC_E_REASON_FAST_ROAM_FAILED	5

/* prune reason codes */
#define WLC_E_PRUNE_ENCR_MISMATCH		1	/* ecryption mismatch */
#define WLC_E_PRUNE_BCAST_BSSID			2	/* AP uses a broadcast BSSID */
#define WLC_E_PRUNE_MAC_DENY			3	/* STA's MAC addr is in AP's MAC deny list */
#define WLC_E_PRUNE_MAC_NA			4	/* STA's MAC addr is not in AP's MAC allow list */
#define WLC_E_PRUNE_REG_PASSV			5	/* AP not allowed due to regulatory restriction */
#define WLC_E_PRUNE_SPCT_MGMT			6	/* AP does not support STA locale spectrum mgmt */
#define WLC_E_PRUNE_RADAR			7	/* AP is on a radar channel of STA locale */
#define WLC_E_RSN_MISMATCH			8	/* STA does not support AP's RSN */
#define WLC_E_PRUNE_NO_COMMON_RATES		9	/* No rates in common with AP */
#define WLC_E_PRUNE_BASIC_RATES			10	/* STA does not support all basic rates of BSS */
#define WLC_E_PRUNE_CCXFAST_PREVAP		11	/* CCX FAST ROAM: prune previous AP */
#define WLC_E_PRUNE_CIPHER_NA			12	/* BSS's cipher not supported */
#define WLC_E_PRUNE_KNOWN_STA			13	/* AP is already known to us as a STA */

/* WLC_SET_CALLBACK data type */
typedef struct wlc_event_cb {
	void (*fn)(void *, bcm_event_t *);	/* Callback function */
	void *context;				/* Passed to callback function */
} wlc_event_cb_t;




#undef PACKED
#if !defined(__GNUC__)
#pragma pack()
#endif

#endif
