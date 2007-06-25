#ifndef BCM43xx_XMIT_H_
#define BCM43xx_XMIT_H_

#include "bcm43xx_main.h"


#define _bcm43xx_declare_plcp_hdr(size) \
	struct bcm43xx_plcp_hdr##size {		\
		union {				\
			__le32 data;		\
			__u8 raw[size];		\
		} __attribute__((__packed__));	\
	} __attribute__((__packed__))

/* struct bcm43xx_plcp_hdr4 */
_bcm43xx_declare_plcp_hdr(4);
/* struct bcm43xx_plcp_hdr6 */
_bcm43xx_declare_plcp_hdr(6);

#undef _bcm43xx_declare_plcp_hdr


/* TX header for v4 firmware */
struct bcm43xx_txhdr_fw4 {
	__le32 mac_ctl;				/* MAC TX control */
	__le16 mac_frame_ctl;			/* Copy of the FrameControl field */
	__le16 tx_fes_time_norm;		/* TX FES Time Normal */
	__le16 phy_ctl;				/* PHY TX control */
	__le16 phy_ctl_0;			/* Unused */
	__le16 phy_ctl_1;			/* Unused */
	__le16 phy_ctl_rts_0;			/* Unused */
	__le16 phy_ctl_rts_1;			/* Unused */
	__u8 phy_rate;				/* PHY rate */
	__u8 phy_rate_rts;			/* PHY rate for RTS/CTS */
	__u8 extra_ft;				/* Extra Frame Types */
	__u8 chan_radio_code;			/* Channel Radio Code */
	__u8 iv[16];				/* Encryption IV */
	__u8 tx_receiver[6];			/* TX Frame Receiver address */
	__le16 tx_fes_time_fb;			/* TX FES Time Fallback */
	struct bcm43xx_plcp_hdr6 rts_plcp_fb;	/* RTS fallback PLCP */
	__le16 rts_dur_fb;			/* RTS fallback duration */
	struct bcm43xx_plcp_hdr6 plcp_fb;	/* Fallback PLCP */
	__le16 dur_fb;				/* Fallback duration */
	__le16 mm_dur_time;			/* Unused */
	__le16 mm_dur_time_fb;			/* Unused */
	__le32 time_stamp;			/* Timestamp */
	PAD_BYTES(2);
	__le16 cookie;				/* TX frame cookie */
	__le16 tx_status;			/* TX status */
	struct bcm43xx_plcp_hdr6 rts_plcp;	/* RTS PLCP */
	__u8 rts_frame[16];			/* The RTS frame (if used) */
	PAD_BYTES(2);
	struct bcm43xx_plcp_hdr6 plcp;		/* Main PLCP */
} __attribute__((__packed__));

/* MAC TX control */
#define BCM43xx_TX4_MAC_KEYIDX		0x0FF00000 /* Security key index */
#define BCM43xx_TX4_MAC_KEYIDX_SHIFT	20
#define BCM43xx_TX4_MAC_KEYALG		0x00070000 /* Security key algorithm */
#define BCM43xx_TX4_MAC_KEYALG_SHIFT	16
#define BCM43xx_TX4_MAC_LIFETIME	0x00001000
#define BCM43xx_TX4_MAC_FRAMEBURST	0x00000800
#define BCM43xx_TX4_MAC_SENDCTS		0x00000400
#define BCM43xx_TX4_MAC_AMPDU		0x00000300
#define BCM43xx_TX4_MAC_AMPDU_SHIFT	8
#define BCM43xx_TX4_MAC_5GHZ		0x00000080
#define BCM43xx_TX4_MAC_IGNPMQ		0x00000020
#define BCM43xx_TX4_MAC_HWSEQ		0x00000010 /* Use Hardware Sequence Number */
#define BCM43xx_TX4_MAC_STMSDU		0x00000008 /* Start MSDU */
#define BCM43xx_TX4_MAC_SENDRTS		0x00000004
#define BCM43xx_TX4_MAC_LONGFRAME	0x00000002
#define BCM43xx_TX4_MAC_ACK		0x00000001

/* Extra Frame Types */
#define BCM43xx_TX4_EFT_FBOFDM		0x0001 /* Data frame fallback rate type */
#define BCM43xx_TX4_EFT_RTSOFDM		0x0004 /* RTS/CTS rate type */
#define BCM43xx_TX4_EFT_RTSFBOFDM	0x0010 /* RTS/CTS fallback rate type */

/* PHY TX control word */
#define BCM43xx_TX4_PHY_OFDM		0x0001 /* Data frame rate type */
#define BCM43xx_TX4_PHY_SHORTPRMBL	0x0010 /* Use short preamble */
#define BCM43xx_TX4_PHY_ANT		0x03C0 /* Antenna selection */
#define  BCM43xx_TX4_PHY_ANT0		0x0000 /* Use antenna 0 */
#define  BCM43xx_TX4_PHY_ANT1		0x0100 /* Use antenna 1 */
#define  BCM43xx_TX4_PHY_ANTLAST	0x0300 /* Use last used antenna */



void bcm43xx_generate_txhdr(struct bcm43xx_wldev *dev,
			    u8 *txhdr,
			    const unsigned char *fragment_data,
			    unsigned int fragment_len,
			    const struct ieee80211_tx_control *txctl,
			    u16 cookie);


/* Transmit Status */
struct bcm43xx_txstatus {
	u16 cookie;		/* The cookie from the txhdr */
	u16 seq;		/* Sequence number */
	u8 phy_stat;		/* PHY TX status */
	u8 frame_count;		/* Frame transmit count */
	u8 rts_count;		/* RTS transmit count */
	u8 supp_reason;		/* Suppression reason */
	/* flags */
	u8 pm_indicated;	/* PM mode indicated to AP */
	u8 intermediate;	/* Intermediate status notification (not final) */
	u8 for_ampdu;		/* Status is for an AMPDU (afterburner) */
	u8 acked;		/* Wireless ACK received */
};

/* txstatus supp_reason values */
enum {
	BCM43xx_TXST_SUPP_NONE,		/* Not suppressed */
	BCM43xx_TXST_SUPP_PMQ,		/* Suppressed due to PMQ entry */
	BCM43xx_TXST_SUPP_FLUSH,	/* Suppressed due to flush request */
	BCM43xx_TXST_SUPP_PREV,		/* Previous fragment failed */
	BCM43xx_TXST_SUPP_CHAN,		/* Channel mismatch */
	BCM43xx_TXST_SUPP_LIFE,		/* Lifetime expired */
	BCM43xx_TXST_SUPP_UNDER,	/* Buffer underflow */
	BCM43xx_TXST_SUPP_ABNACK,	/* Afterburner NACK */
};

/* Transmit Status as received through DMA/PIO on old chips */
struct bcm43xx_hwtxstatus {
	PAD_BYTES(4);
	__le16 cookie;
	u8 flags;
	u8 count;
	PAD_BYTES(2);
	__le16 seq;
	u8 phy_stat;
	PAD_BYTES(1);
} __attribute__((__packed__));


/* Receive header for v4 firmware. */
struct bcm43xx_rxhdr_fw4 {
	__le16 frame_len;	/* Frame length */
	PAD_BYTES(2);
	__le16 phy_status0;	/* PHY RX Status 0 */
	__u8 jssi;		/* PHY RX Status 1: JSSI */
	__u8 sig_qual;		/* PHY RX Status 1: Signal Quality */
	__le16 phy_status2;	/* PHY RX Status 2 */
	__le16 phy_status3;	/* PHY RX Status 3 */
	__le32 mac_status;	/* MAC RX status */
	__le16 mac_time;
	__le16 channel;
} __attribute__((__packed__));


/* PHY RX Status 0 */
#define BCM43xx_RX_PHYST0_GAINCTL	0x4000 /* Gain Control */
#define BCM43xx_RX_PHYST0_PLCPHCF	0x0200
#define BCM43xx_RX_PHYST0_PLCPFV	0x0100
#define BCM43xx_RX_PHYST0_SHORTPRMBL	0x0080 /* Received with Short Preamble */
#define BCM43xx_RX_PHYST0_LCRS		0x0040
#define BCM43xx_RX_PHYST0_ANT		0x0020 /* Antenna */
#define BCM43xx_RX_PHYST0_UNSRATE	0x0010
#define BCM43xx_RX_PHYST0_CLIP		0x000C
#define BCM43xx_RX_PHYST0_CLIP_SHIFT	2
#define BCM43xx_RX_PHYST0_FTYPE		0x0003 /* Frame type */
#define  BCM43xx_RX_PHYST0_CCK		0x0000 /* Frame type: CCK */
#define  BCM43xx_RX_PHYST0_OFDM		0x0001 /* Frame type: OFDM */
#define  BCM43xx_RX_PHYST0_PRE_N	0x0002 /* Pre-standard N-PHY frame */
#define  BCM43xx_RX_PHYST0_STD_N	0x0003 /* Standard N-PHY frame */

/* PHY RX Status 2 */
#define BCM43xx_RX_PHYST2_LNAG		0xC000 /* LNA Gain */
#define BCM43xx_RX_PHYST2_LNAG_SHIFT	14
#define BCM43xx_RX_PHYST2_PNAG		0x3C00 /* PNA Gain */
#define BCM43xx_RX_PHYST2_PNAG_SHIFT	10
#define BCM43xx_RX_PHYST2_FOFF		0x03FF /* F offset */

/* PHY RX Status 3 */
#define BCM43xx_RX_PHYST3_DIGG		0x1800 /* DIG Gain */
#define BCM43xx_RX_PHYST3_DIGG_SHIFT	11
#define BCM43xx_RX_PHYST3_TRSTATE	0x0400 /* TR state */

/* MAC RX Status */
#define BCM43xx_RX_MAC_BEACONSENT	0x00008000 /* Beacon send flag */
#define BCM43xx_RX_MAC_KEYIDX		0x000007E0 /* Key index */
#define BCM43xx_RX_MAC_KEYIDX_SHIFT	5
#define BCM43xx_RX_MAC_DECERR		0x00000010 /* Decrypt error */
#define BCM43xx_RX_MAC_DEC		0x00000008 /* Decryption attempted */
#define BCM43xx_RX_MAC_PADDING		0x00000004 /* Pad bytes present */
#define BCM43xx_RX_MAC_RESP		0x00000002 /* Response frame transmitted */
#define BCM43xx_RX_MAC_FCSERR		0x00000001 /* FCS error */

/* RX channel */
#define BCM43xx_RX_CHAN_GAIN		0xFC00 /* Gain */
#define BCM43xx_RX_CHAN_GAIN_SHIFT	10
#define BCM43xx_RX_CHAN_ID		0x03FC /* Channel ID */
#define BCM43xx_RX_CHAN_ID_SHIFT	2
#define BCM43xx_RX_CHAN_PHYTYPE		0x0003 /* PHY type */



u8 bcm43xx_plcp_get_ratecode_cck(const u8 bitrate);
u8 bcm43xx_plcp_get_ratecode_ofdm(const u8 bitrate);

void bcm43xx_generate_plcp_hdr(struct bcm43xx_plcp_hdr4 *plcp,
			       const u16 octets, const u8 bitrate);

void bcm43xx_rx(struct bcm43xx_wldev *dev,
		struct sk_buff *skb,
		const void *_rxhdr);

void bcm43xx_handle_txstatus(struct bcm43xx_wldev *dev,
			     const struct bcm43xx_txstatus *status);

void bcm43xx_handle_hwtxstatus(struct bcm43xx_wldev *dev,
			       const struct bcm43xx_hwtxstatus *hw);

void bcm43xx_tx_suspend(struct bcm43xx_wldev *dev);
void bcm43xx_tx_resume(struct bcm43xx_wldev *dev);


#define BCM43xx_NR_QOSPARMS		22
enum {
	BCM43xx_QOSPARM_TXOP = 0,
	BCM43xx_QOSPARM_CWMIN,
	BCM43xx_QOSPARM_CWMAX,
	BCM43xx_QOSPARM_CWCUR,
	BCM43xx_QOSPARM_AIFS,
	BCM43xx_QOSPARM_BSLOTS,
	BCM43xx_QOSPARM_REGGAP,
	BCM43xx_QOSPARM_STATUS,
};
void bcm43xx_qos_init(struct bcm43xx_wldev *dev);


/* Helper functions for converting the key-table index from "firmware-format"
 * to "raw-format" and back. The firmware API changed for this at some revision.
 * We need to account for that here. */
static inline
int bcm43xx_new_kidx_api(struct bcm43xx_wldev *dev)
{
	/* FIXME: Not sure the change was at rev 351 */
	return (dev->fw.rev >= 351);
}
static inline
u8 bcm43xx_kidx_to_fw(struct bcm43xx_wldev *dev, u8 raw_kidx)
{
	u8 firmware_kidx;
	if (bcm43xx_new_kidx_api(dev)) {
		firmware_kidx = raw_kidx;
	} else {
		if (raw_kidx >= 4) /* Is per STA key? */
			firmware_kidx = raw_kidx - 4;
		else
			firmware_kidx = raw_kidx; /* TX default key */
	}
	return firmware_kidx;
}
static inline
u8 bcm43xx_kidx_to_raw(struct bcm43xx_wldev *dev, u8 firmware_kidx)
{
	u8 raw_kidx;
	if (bcm43xx_new_kidx_api(dev))
		raw_kidx = firmware_kidx;
	else
		raw_kidx = firmware_kidx + 4; /* RX default keys or per STA keys */
	return raw_kidx;
}

#endif /* BCM43xx_XMIT_H_ */
