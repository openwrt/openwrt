/*
 * Header bits derived from MadWifi source:
 *   Copyright (c) 2001 Atsushi Onoe
 *   Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 *   All rights reserved.
 *
 * Distributed under the terms of the GPLv2 license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MADWIFI_H
#define _MADWIFI_H

/* ieee80211.h */
#define	IEEE80211_ADDR_LEN		6
#define	IEEE80211_RATE_VAL		0x7f


/* ieee80211_crypto.h */
#define	IEEE80211_KEYBUF_SIZE		16
#define	IEEE80211_MICBUF_SIZE		16
#define IEEE80211_TID_SIZE			17

#define	IEEE80211_CIPHER_WEP		0
#define	IEEE80211_CIPHER_TKIP		1
#define	IEEE80211_CIPHER_AES_OCB	2
#define	IEEE80211_CIPHER_AES_CCM	3
#define	IEEE80211_CIPHER_CKIP		5
#define	IEEE80211_CIPHER_NONE		6
#define	IEEE80211_CIPHER_MAX		(IEEE80211_CIPHER_NONE + 1)


/* ieee80211_ioctl.h */
#define	IEEE80211_KEY_DEFAULT		0x80
#define	IEEE80211_CHAN_MAX			255
#define	IEEE80211_CHAN_BYTES		32
#define	IEEE80211_RATE_MAXSIZE		15

#define	IEEE80211_IOCTL_GETKEY		(SIOCDEVPRIVATE+3)
#define	IEEE80211_IOCTL_STA_STATS	(SIOCDEVPRIVATE+5)
#define	IEEE80211_IOCTL_STA_INFO	(SIOCDEVPRIVATE+6)

#define	IEEE80211_IOCTL_GETPARAM	(SIOCIWFIRSTPRIV+1)
#define	IEEE80211_IOCTL_GETMODE		(SIOCIWFIRSTPRIV+3)
#define	IEEE80211_IOCTL_GETCHANLIST	(SIOCIWFIRSTPRIV+7)
#define	IEEE80211_IOCTL_GETCHANINFO	(SIOCIWFIRSTPRIV+13)

#define	SIOC80211IFCREATE			(SIOCDEVPRIVATE+7)
#define	SIOC80211IFDESTROY	 		(SIOCDEVPRIVATE+8)

#define	IEEE80211_CLONE_BSSID	0x0001	/* allocate unique mac/bssid */
#define	IEEE80211_NO_STABEACONS	0x0002	/* Do not setup the station beacon timers */

struct ieee80211_clone_params {
	char icp_name[IFNAMSIZ];		/* device name */
	u_int16_t icp_opmode;			/* operating mode */
	u_int16_t icp_flags;			/* see below */
};

enum ieee80211_opmode {
	IEEE80211_M_STA		= 1,	/* infrastructure station */
	IEEE80211_M_IBSS 	= 0,	/* IBSS (adhoc) station */
	IEEE80211_M_AHDEMO	= 3,	/* Old lucent compatible adhoc demo */
	IEEE80211_M_HOSTAP	= 6,	/* Software Access Point */
	IEEE80211_M_MONITOR	= 8,	/* Monitor mode */
	IEEE80211_M_WDS		= 2,	/* WDS link */
};

enum {
	IEEE80211_PARAM_AUTHMODE		= 3,	/* authentication mode */
	IEEE80211_PARAM_MCASTCIPHER		= 5,	/* multicast/default cipher */
	IEEE80211_PARAM_MCASTKEYLEN		= 6,	/* multicast key length */
	IEEE80211_PARAM_UCASTCIPHERS	= 7,	/* unicast cipher suites */
	IEEE80211_PARAM_WPA				= 10,	/* WPA mode (0,1,2) */
};

/*
 * Authentication mode.
 */
enum ieee80211_authmode {
	IEEE80211_AUTH_NONE	= 0,
	IEEE80211_AUTH_OPEN	= 1,	/* open */
	IEEE80211_AUTH_SHARED	= 2,	/* shared-key */
	IEEE80211_AUTH_8021X	= 3,	/* 802.1x */
	IEEE80211_AUTH_AUTO	= 4,	/* auto-select/accept */
	/* NB: these are used only for ioctls */
	IEEE80211_AUTH_WPA	= 5,	/* WPA/RSN w/ 802.1x/PSK */
};

struct ieee80211_channel {
	u_int16_t ic_freq;	/* setting in MHz */
	u_int16_t ic_flags;	/* see below */
	u_int8_t ic_ieee;	/* IEEE channel number */
	int8_t ic_maxregpower;	/* maximum regulatory tx power in dBm */
	int8_t ic_maxpower;	/* maximum tx power in dBm */
	int8_t ic_minpower;	/* minimum tx power in dBm */
	u_int8_t ic_scanflags;
	u_int8_t ic_idletime; /* phy idle time in % */
};

struct ieee80211req_key {
	u_int8_t ik_type;		/* key/cipher type */
	u_int8_t ik_pad;
	u_int16_t ik_keyix;	/* key index */
	u_int8_t ik_keylen;		/* key length in bytes */
	u_int8_t ik_flags;
	u_int8_t ik_macaddr[IEEE80211_ADDR_LEN];
	u_int64_t ik_keyrsc;		/* key receive sequence counter */
	u_int64_t ik_keytsc;		/* key transmit sequence counter */
	u_int8_t ik_keydata[IEEE80211_KEYBUF_SIZE+IEEE80211_MICBUF_SIZE];
};

struct ieee80211req_chanlist {
	u_int8_t ic_channels[IEEE80211_CHAN_BYTES];
};

struct ieee80211req_chaninfo {
	u_int ic_nchans;
	struct ieee80211_channel ic_chans[IEEE80211_CHAN_MAX];
};

struct ieee80211req_sta_info {
	u_int16_t isi_len;		/* length (mult of 4) */
	u_int16_t isi_freq;		/* MHz */
	u_int16_t isi_flags;		/* channel flags */
	u_int16_t isi_state;		/* state flags */
	u_int8_t isi_authmode;		/* authentication algorithm */
	u_int8_t isi_rssi;
	int8_t isi_noise;
	u_int16_t isi_capinfo;		/* capabilities */
	u_int8_t isi_athflags;		/* Atheros capabilities */
	u_int8_t isi_erp;		/* ERP element */
	u_int8_t isi_macaddr[IEEE80211_ADDR_LEN];
	u_int8_t isi_nrates;		/* negotiated rates */
	u_int8_t isi_rates[IEEE80211_RATE_MAXSIZE];
	u_int8_t isi_txrate;		/* index to isi_rates[] */
	u_int16_t isi_ie_len;		/* IE length */
	u_int16_t isi_associd;		/* assoc response */
	u_int16_t isi_txpower;		/* current tx power */
	u_int16_t isi_vlan;		/* vlan tag */
	u_int16_t isi_txseqs[17];	/* seq to be transmitted */
	u_int16_t isi_rxseqs[17];	/* seq previous for qos frames*/
	u_int16_t isi_inact;		/* inactivity timer */
	u_int8_t isi_uapsd;		/* UAPSD queues */
	u_int8_t isi_opmode;		/* sta operating mode */
};

#endif
