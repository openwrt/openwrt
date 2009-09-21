/*-
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2004 Sam Leffler, Errno Consulting
 * Copyright (c) 2006 Atheros Communications, Inc.
 *
 * Wireless Network driver for Atheros AR6001
 * All rights reserved.
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
 *
 */
#ifndef _NET80211_IEEE80211_H_
#define _NET80211_IEEE80211_H_

#include "athstartpack.h"

/*
 * 802.11 protocol definitions.
 */

#define IEEE80211_ADDR_LEN  6       /* size of 802.11 address */
/* is 802.11 address multicast/broadcast? */
#define IEEE80211_IS_MULTICAST(_a)  (*(_a) & 0x01)
#define IEEE80211_ADDR_EQ(addr1, addr2)     \
    (A_MEMCMP(addr1, addr2, IEEE80211_ADDR_LEN) == 0)

#define IEEE80211_KEYBUF_SIZE 16
#define IEEE80211_MICBUF_SIZE (8+8)  /* space for both tx and rx */

/*
 * NB: these values are ordered carefully; there are lots of
 * of implications in any reordering.  In particular beware
 * that 4 is not used to avoid conflicting with IEEE80211_F_PRIVACY.
 */
#define IEEE80211_CIPHER_WEP            0
#define IEEE80211_CIPHER_TKIP           1
#define IEEE80211_CIPHER_AES_OCB        2
#define IEEE80211_CIPHER_AES_CCM        3
#define IEEE80211_CIPHER_CKIP           5
#define IEEE80211_CIPHER_CCKM_KRK       6
#define IEEE80211_CIPHER_NONE           7       /* pseudo value */

#define IEEE80211_CIPHER_MAX            (IEEE80211_CIPHER_NONE+1)

#define IEEE80211_IS_VALID_WEP_CIPHER_LEN(len) \
        (((len) == 5) || ((len) == 13) || ((len) == 16))



/*
 * generic definitions for IEEE 802.11 frames
 */
PREPACK struct ieee80211_frame {
    A_UINT8    i_fc[2];
    A_UINT8    i_dur[2];
    A_UINT8    i_addr1[IEEE80211_ADDR_LEN];
    A_UINT8    i_addr2[IEEE80211_ADDR_LEN];
    A_UINT8    i_addr3[IEEE80211_ADDR_LEN];
    A_UINT8    i_seq[2];
    /* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
    /* see below */
} POSTPACK;

#define IEEE80211_FC0_VERSION_MASK          0x03
#define IEEE80211_FC0_VERSION_SHIFT         0
#define IEEE80211_FC0_VERSION_0             0x00
#define IEEE80211_FC0_TYPE_MASK             0x0c
#define IEEE80211_FC0_TYPE_SHIFT            2
#define IEEE80211_FC0_TYPE_MGT              0x00
#define IEEE80211_FC0_TYPE_CTL              0x04
#define IEEE80211_FC0_TYPE_DATA             0x08

#define IEEE80211_FC0_SUBTYPE_MASK          0xf0
#define IEEE80211_FC0_SUBTYPE_SHIFT         4
/* for TYPE_MGT */
#define IEEE80211_FC0_SUBTYPE_ASSOC_REQ     0x00
#define IEEE80211_FC0_SUBTYPE_ASSOC_RESP    0x10
#define IEEE80211_FC0_SUBTYPE_REASSOC_REQ   0x20
#define IEEE80211_FC0_SUBTYPE_REASSOC_RESP  0x30
#define IEEE80211_FC0_SUBTYPE_PROBE_REQ     0x40
#define IEEE80211_FC0_SUBTYPE_PROBE_RESP    0x50
#define IEEE80211_FC0_SUBTYPE_BEACON        0x80
#define IEEE80211_FC0_SUBTYPE_ATIM          0x90
#define IEEE80211_FC0_SUBTYPE_DISASSOC      0xa0
#define IEEE80211_FC0_SUBTYPE_AUTH          0xb0
#define IEEE80211_FC0_SUBTYPE_DEAUTH        0xc0
/* for TYPE_CTL */
#define IEEE80211_FC0_SUBTYPE_PS_POLL       0xa0
#define IEEE80211_FC0_SUBTYPE_RTS           0xb0
#define IEEE80211_FC0_SUBTYPE_CTS           0xc0
#define IEEE80211_FC0_SUBTYPE_ACK           0xd0
#define IEEE80211_FC0_SUBTYPE_CF_END        0xe0
#define IEEE80211_FC0_SUBTYPE_CF_END_ACK    0xf0
/* for TYPE_DATA (bit combination) */
#define IEEE80211_FC0_SUBTYPE_DATA          0x00
#define IEEE80211_FC0_SUBTYPE_CF_ACK        0x10
#define IEEE80211_FC0_SUBTYPE_CF_POLL       0x20
#define IEEE80211_FC0_SUBTYPE_CF_ACPL       0x30
#define IEEE80211_FC0_SUBTYPE_NODATA        0x40
#define IEEE80211_FC0_SUBTYPE_CFACK         0x50
#define IEEE80211_FC0_SUBTYPE_CFPOLL        0x60
#define IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK 0x70
#define IEEE80211_FC0_SUBTYPE_QOS           0x80
#define IEEE80211_FC0_SUBTYPE_QOS_NULL      0xc0

#define IEEE80211_FC1_DIR_MASK              0x03
#define IEEE80211_FC1_DIR_NODS              0x00    /* STA->STA */
#define IEEE80211_FC1_DIR_TODS              0x01    /* STA->AP  */
#define IEEE80211_FC1_DIR_FROMDS            0x02    /* AP ->STA */
#define IEEE80211_FC1_DIR_DSTODS            0x03    /* AP ->AP  */

#define IEEE80211_FC1_MORE_FRAG             0x04
#define IEEE80211_FC1_RETRY                 0x08
#define IEEE80211_FC1_PWR_MGT               0x10
#define IEEE80211_FC1_MORE_DATA             0x20
#define IEEE80211_FC1_WEP                   0x40
#define IEEE80211_FC1_ORDER                 0x80

#define IEEE80211_SEQ_FRAG_MASK             0x000f
#define IEEE80211_SEQ_FRAG_SHIFT            0
#define IEEE80211_SEQ_SEQ_MASK              0xfff0
#define IEEE80211_SEQ_SEQ_SHIFT             4

#define IEEE80211_NWID_LEN                  32

/*
 * 802.11 rate set.
 */
#define IEEE80211_RATE_SIZE     8       /* 802.11 standard */
#define IEEE80211_RATE_MAXSIZE  15      /* max rates we'll handle */

#define WMM_NUM_AC                  4   /* 4 AC categories */

#define WMM_PARAM_ACI_M         0x60    /* Mask for ACI field */
#define WMM_PARAM_ACI_S         5   /* Shift for ACI field */
#define WMM_PARAM_ACM_M         0x10    /* Mask for ACM bit */
#define WMM_PARAM_ACM_S         4       /* Shift for ACM bit */
#define WMM_PARAM_AIFSN_M       0x0f    /* Mask for aifsn field */
#define WMM_PARAM_LOGCWMIN_M    0x0f    /* Mask for CwMin field (in log) */
#define WMM_PARAM_LOGCWMAX_M    0xf0    /* Mask for CwMax field (in log) */
#define WMM_PARAM_LOGCWMAX_S    4   /* Shift for CwMax field */

#define WMM_AC_TO_TID(_ac) (       \
    ((_ac) == WMM_AC_VO) ? 6 : \
    ((_ac) == WMM_AC_VI) ? 5 : \
    ((_ac) == WMM_AC_BK) ? 1 : \
    0)

#define TID_TO_WMM_AC(_tid) (      \
    ((_tid) < 1) ? WMM_AC_BE : \
    ((_tid) < 3) ? WMM_AC_BK : \
    ((_tid) < 6) ? WMM_AC_VI : \
    WMM_AC_VO)
/*
 * Management information element payloads.
 */

enum {
    IEEE80211_ELEMID_SSID       = 0,
    IEEE80211_ELEMID_RATES      = 1,
    IEEE80211_ELEMID_FHPARMS    = 2,
    IEEE80211_ELEMID_DSPARMS    = 3,
    IEEE80211_ELEMID_CFPARMS    = 4,
    IEEE80211_ELEMID_TIM        = 5,
    IEEE80211_ELEMID_IBSSPARMS  = 6,
    IEEE80211_ELEMID_COUNTRY    = 7,
    IEEE80211_ELEMID_CHALLENGE  = 16,
    /* 17-31 reserved for challenge text extension */
    IEEE80211_ELEMID_PWRCNSTR   = 32,
    IEEE80211_ELEMID_PWRCAP     = 33,
    IEEE80211_ELEMID_TPCREQ     = 34,
    IEEE80211_ELEMID_TPCREP     = 35,
    IEEE80211_ELEMID_SUPPCHAN   = 36,
    IEEE80211_ELEMID_CHANSWITCH = 37,
    IEEE80211_ELEMID_MEASREQ    = 38,
    IEEE80211_ELEMID_MEASREP    = 39,
    IEEE80211_ELEMID_QUIET      = 40,
    IEEE80211_ELEMID_IBSSDFS    = 41,
    IEEE80211_ELEMID_ERP        = 42,
    IEEE80211_ELEMID_RSN        = 48,
    IEEE80211_ELEMID_XRATES     = 50,
    IEEE80211_ELEMID_TPC        = 150,
    IEEE80211_ELEMID_CCKM       = 156,
    IEEE80211_ELEMID_VENDOR     = 221,  /* vendor private */
};

#define ATH_OUI             0x7f0300        /* Atheros OUI */
#define ATH_OUI_TYPE        0x01
#define ATH_OUI_SUBTYPE     0x01
#define ATH_OUI_VERSION     0x00

#define WPA_OUI             0xf25000
#define WPA_OUI_TYPE        0x01
#define WPA_VERSION         1          /* current supported version */

#define WPA_CSE_NULL        0x00
#define WPA_CSE_WEP40       0x01
#define WPA_CSE_TKIP        0x02
#define WPA_CSE_CCMP        0x04
#define WPA_CSE_WEP104      0x05

#define WPA_ASE_NONE        0x00
#define WPA_ASE_8021X_UNSPEC    0x01
#define WPA_ASE_8021X_PSK   0x02

#define RSN_OUI         0xac0f00
#define RSN_VERSION     1       /* current supported version */

#define RSN_CSE_NULL        0x00
#define RSN_CSE_WEP40       0x01
#define RSN_CSE_TKIP        0x02
#define RSN_CSE_WRAP        0x03
#define RSN_CSE_CCMP        0x04
#define RSN_CSE_WEP104      0x05

#define RSN_ASE_NONE        0x00
#define RSN_ASE_8021X_UNSPEC    0x01
#define RSN_ASE_8021X_PSK   0x02

#define RSN_CAP_PREAUTH     0x01

#define WMM_OUI         0xf25000
#define WMM_OUI_TYPE        0x02
#define WMM_INFO_OUI_SUBTYPE    0x00
#define WMM_PARAM_OUI_SUBTYPE   0x01
#define WMM_VERSION     1

/* WMM stream classes */
#define WMM_NUM_AC  4
#define WMM_AC_BE   0       /* best effort */
#define WMM_AC_BK   1       /* background */
#define WMM_AC_VI   2       /* video */
#define WMM_AC_VO   3       /* voice */

/* TSPEC related */
#define ACTION_CATEGORY_CODE_TSPEC                 17
#define ACTION_CODE_TSPEC_ADDTS                    0
#define ACTION_CODE_TSPEC_ADDTS_RESP               1
#define ACTION_CODE_TSPEC_DELTS                    2

typedef enum {
    TSPEC_STATUS_CODE_ADMISSION_ACCEPTED = 0,
    TSPEC_STATUS_CODE_ADDTS_INVALID_PARAMS = 0x1,
    TSPEC_STATUS_CODE_ADDTS_REQUEST_REFUSED = 0x3,
    TSPEC_STATUS_CODE_UNSPECIFIED_QOS_RELATED_FAILURE = 0xC8,
    TSPEC_STATUS_CODE_REQUESTED_REFUSED_POLICY_CONFIGURATION = 0xC9,
    TSPEC_STATUS_CODE_INSUFFCIENT_BANDWIDTH = 0xCA,
    TSPEC_STATUS_CODE_INVALID_PARAMS = 0xCB,
    TSPEC_STATUS_CODE_DELTS_SENT    = 0x30,
    TSPEC_STATUS_CODE_DELTS_RECV    = 0x31,
} TSPEC_STATUS_CODE;

/*
 * WMM/802.11e Tspec Element
 */
typedef PREPACK struct wmm_tspec_ie_t {
    A_UINT8     elementId;
    A_UINT8     len;
    A_UINT8     oui[3];
    A_UINT8     ouiType;
    A_UINT8     ouiSubType;
    A_UINT8     version;
    A_UINT16    tsInfo_info;
    A_UINT8     tsInfo_reserved;
    A_UINT16    nominalMSDU;
    A_UINT16    maxMSDU;
    A_UINT32    minServiceInt;
    A_UINT32    maxServiceInt;
    A_UINT32    inactivityInt;
    A_UINT32    suspensionInt;
    A_UINT32    serviceStartTime;
    A_UINT32    minDataRate;
    A_UINT32    meanDataRate;
    A_UINT32    peakDataRate;
    A_UINT32    maxBurstSize;
    A_UINT32    delayBound;
    A_UINT32    minPhyRate;
    A_UINT16    sba;
    A_UINT16    mediumTime;
} POSTPACK WMM_TSPEC_IE;


/*
 * BEACON management packets
 *
 *  octet timestamp[8]
 *  octet beacon interval[2]
 *  octet capability information[2]
 *  information element
 *      octet elemid
 *      octet length
 *      octet information[length]
 */

#define IEEE80211_BEACON_INTERVAL(beacon) \
    ((beacon)[8] | ((beacon)[9] << 8))
#define IEEE80211_BEACON_CAPABILITY(beacon) \
    ((beacon)[10] | ((beacon)[11] << 8))

#define IEEE80211_CAPINFO_ESS               0x0001
#define IEEE80211_CAPINFO_IBSS              0x0002
#define IEEE80211_CAPINFO_CF_POLLABLE       0x0004
#define IEEE80211_CAPINFO_CF_POLLREQ        0x0008
#define IEEE80211_CAPINFO_PRIVACY           0x0010
#define IEEE80211_CAPINFO_SHORT_PREAMBLE    0x0020
#define IEEE80211_CAPINFO_PBCC              0x0040
#define IEEE80211_CAPINFO_CHNL_AGILITY      0x0080
/* bits 8-9 are reserved */
#define IEEE80211_CAPINFO_SHORT_SLOTTIME    0x0400
#define IEEE80211_CAPINFO_APSD              0x0800
/* bit 12 is reserved */
#define IEEE80211_CAPINFO_DSSSOFDM          0x2000
/* bits 14-15 are reserved */

/*
 * Authentication Modes
 */

enum ieee80211_authmode {
    IEEE80211_AUTH_NONE     = 0,
    IEEE80211_AUTH_OPEN     = 1,
    IEEE80211_AUTH_SHARED   = 2,
    IEEE80211_AUTH_8021X    = 3,
    IEEE80211_AUTH_AUTO     = 4,   /* auto-select/accept */
    /* NB: these are used only for ioctls */
    IEEE80211_AUTH_WPA      = 5,  /* WPA/RSN  w/ 802.1x */
    IEEE80211_AUTH_WPA_PSK  = 6,  /* WPA/RSN  w/ PSK */
    IEEE80211_AUTH_WPA_CCKM = 7,  /* WPA/RSN IE  w/ CCKM */
};

#include "athendpack.h"

#endif /* _NET80211_IEEE80211_H_ */
