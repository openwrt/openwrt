// SPDX-License-Identifier: GPL-2.0-only
#ifndef QDMA_DESC_H
#define QDMA_DESC_H

#include <linux/bits.h>
#include <linux/bitfield.h>
#include <linux/types.h>

#define FIELD_SET(current, mask, val)	\
	(((current) & ~(mask)) | FIELD_PREP((mask), (val)))

/** etx:  */
struct etx {
	/**
	 * See accessors:
	 * get_etx_unknown0()
	 * set_etx_unknown0()
	 * get_etx_sp_tag()
	 * set_etx_sp_tag()
	 * is_etx_oam()
	 * set_etx_oam()
	 * get_etx_channel()
	 * set_etx_channel()
	 * get_etx_queue()
	 * set_etx_queue()
	 */
	u32 bitfield_0;

	/**
	 * See accessors:
	 * is_etx_ico()
	 * set_etx_ico()
	 * is_etx_uco()
	 * set_etx_uco()
	 * is_etx_tco()
	 * set_etx_tco()
	 * is_etx_sco()
	 * set_etx_sco()
	 * get_etx_udf_pmap()
	 * set_etx_udf_pmap()
	 * get_etx_fport()
	 * set_etx_fport()
	 * is_etx_vlan_en()
	 * set_etx_vlan_en()
	 * get_etx_vlan_type()
	 * set_etx_vlan_type()
	 * get_etx_vlan_tag()
	 * set_etx_vlan_tag()
	 */
	u32 bitfield_1;

};

/**
 * Bitfield accessors for: etx bitfield_0
 */

#define ETX_UNKNOWN0_MASK				GENMASK(31, 28)
#define ETX_SP_TAG_MASK					GENMASK(27, 12)
#define ETX_OAM						BIT(11)
#define ETX_CHANNEL_MASK				GENMASK(10, 3)
#define ETX_QUEUE_MASK					GENMASK(2, 0)


/** Unused, called "rev" probably short for reserved */
static inline u8 get_etx_unknown0(struct etx *x) {
	return FIELD_GET(ETX_UNKNOWN0_MASK, x->bitfield_0);
}
static inline void set_etx_unknown0(struct etx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ETX_UNKNOWN0_MASK, v);
}

/**
 * MediaTek "Special Tag" format which encapsulates both switch port number and
 * possible VLAN
 */
static inline u16 get_etx_sp_tag(struct etx *x) {
	return FIELD_GET(ETX_SP_TAG_MASK, x->bitfield_0);
}
static inline void set_etx_sp_tag(struct etx *x, u16 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ETX_SP_TAG_MASK, v);
}

/** OAM (management) frame, never used with Ethernet transmissions */
static inline bool is_etx_oam(struct etx *x) {
	return FIELD_GET(ETX_OAM, x->bitfield_0);
}
static inline void set_etx_oam(struct etx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ETX_OAM, v);
}

/** The channel number for QoS prioritization */
static inline u8 get_etx_channel(struct etx *x) {
	return FIELD_GET(ETX_CHANNEL_MASK, x->bitfield_0);
}
static inline void set_etx_channel(struct etx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ETX_CHANNEL_MASK, v);
}

/** The queue number for QoS prioritization */
static inline u8 get_etx_queue(struct etx *x) {
	return FIELD_GET(ETX_QUEUE_MASK, x->bitfield_0);
}
static inline void set_etx_queue(struct etx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ETX_QUEUE_MASK, v);
}

/**
 * Bitfield accessors for: etx bitfield_1
 */

enum etx_fport {
	ETX_FPORT_QDMA0_CPU				= 0,
	ETX_FPORT_GDM1					= 1,
	ETX_FPORT_GDM2					= 2,
	ETX_FPORT_QDMA0_HWF				= 3,
	ETX_FPORT_PPE					= 4,
	ETX_FPORT_QDMA1_CPU				= 5,
	ETX_FPORT_QDMA1_HWF				= 6,
	ETX_FPORT_DROP					= 7,
};
enum etx_vlan_type {
	ETX_VLAN_TYPE_8100				= 0,
	ETX_VLAN_TYPE_9100				= 2,
	ETX_VLAN_TYPE_88A8				= 1,
	ETX_VLAN_TYPE_UNKNOWN				= 3,
};

#define ETX_ICO						BIT(31)
#define ETX_UCO						BIT(30)
#define ETX_TCO						BIT(29)
#define ETX_SCO						BIT(28)
#define ETX_UDF_PMAP_MASK				GENMASK(27, 22)
#define ETX_FPORT_MASK					GENMASK(21, 19)
#define ETX_VLAN_EN					BIT(18)
#define ETX_VLAN_TYPE_MASK				GENMASK(17, 16)
#define ETX_VLAN_TAG_MASK				GENMASK(15, 0)


/** Checksum offload, probably IP */
static inline bool is_etx_ico(struct etx *x) {
	return FIELD_GET(ETX_ICO, x->bitfield_1);
}
static inline void set_etx_ico(struct etx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_ICO, v);
}

/** Checksum offload, probably UDP */
static inline bool is_etx_uco(struct etx *x) {
	return FIELD_GET(ETX_UCO, x->bitfield_1);
}
static inline void set_etx_uco(struct etx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_UCO, v);
}

/** Checksum offload, probably TCP */
static inline bool is_etx_tco(struct etx *x) {
	return FIELD_GET(ETX_TCO, x->bitfield_1);
}
static inline void set_etx_tco(struct etx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_TCO, v);
}

/** Unknown, maybe SCTP checksum offload */
static inline bool is_etx_sco(struct etx *x) {
	return FIELD_GET(ETX_SCO, x->bitfield_1);
}
static inline void set_etx_sco(struct etx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_SCO, v);
}

/** Unknown / unused */
static inline u8 get_etx_udf_pmap(struct etx *x) {
	return FIELD_GET(ETX_UDF_PMAP_MASK, x->bitfield_1);
}
static inline void set_etx_udf_pmap(struct etx *x, u8 v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_UDF_PMAP_MASK, v);
}

/**
 * Where in the Frame Engine to send the packet to QDMA0_CPU / QDMA1_CPU sends
 * to CPU via the relevant QDMA engine. QDMA0_HWF and QDMA1_HWF goes to QDMA
 * hardware forwarding. GDM1 is the LAN, GDM2 is the WAN, and PPE is the Packet
 * Processing Engine.
 */
static inline enum etx_fport get_etx_fport(struct etx *x) {
	return FIELD_GET(ETX_FPORT_MASK, x->bitfield_1);
}
static inline void set_etx_fport(struct etx *x, enum etx_fport v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_FPORT_MASK, v);
}

/** If 1 then add a vlan header to the packet */
static inline bool is_etx_vlan_en(struct etx *x) {
	return FIELD_GET(ETX_VLAN_EN, x->bitfield_1);
}
static inline void set_etx_vlan_en(struct etx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_VLAN_EN, v);
}

/** Which type of vlan to add to the packet header */
static inline enum etx_vlan_type get_etx_vlan_type(struct etx *x) {
	return FIELD_GET(ETX_VLAN_TYPE_MASK, x->bitfield_1);
}
static inline void set_etx_vlan_type(struct etx *x, enum etx_vlan_type v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_VLAN_TYPE_MASK, v);
}

/** The VLAN number, if vlan_en is set */
static inline u16 get_etx_vlan_tag(struct etx *x) {
	return FIELD_GET(ETX_VLAN_TAG_MASK, x->bitfield_1);
}
static inline void set_etx_vlan_tag(struct etx *x, u16 v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ETX_VLAN_TAG_MASK, v);
}

/**
 * qdma_desc_erx
 *
 *     3                     2                   1                   0
 *     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  0 |                            unknown0                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  4 |nknwn|I|P|F|T|L|A| sport |   crsn  |         ppe_entry         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  8 |                           unknown2                          |N|
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 12 |             sp_tag            |              tci              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 16
 *
 * @unknown0 (32 bit): Unknown / unused field, first word in descriptor
 * @bitfield_0 (32 bit):
 *   @unknown1 "nknwn" (bits 31..29): Revision number? (unused)
 *   @ip6 "I" (bit 28): IPv6 packet indicator
 *   @ip4 "P" (bit 27): IPv4 packet indicator
 *   @ip4f "F" (bit 26): IPv4 fragment flag
 *   @tack "T" (bit 25): TCP ACK flag
 *   @l2vld "L" (bit 24): Layer 2 valid flag
 *   @l4f "A" (bit 23): Layer 4 flag (e.g., checksum failure)
 *   @sport (bits 22..19): Where the packet came from, mostly unknown / unused,
 *                         needs testing
 *   @crsn (bits 18..14): Most likely a MediaTek PPE CPU_REASON
 *   @ppe_entry (bits 13..0): PPE (Packet Processing Engine) entry index
 * @bitfield_1 (32 bit):
 *   @unknown2 (bits 31..1): Reserved
 *   @untag "N" (bit 0): VLAN untag flag
 * @bitfield_3 (32 bit):
 *   @sp_tag (bits 31..16): MediaTek "Special Tag" for switch port/VLAN encoding
 *   @tci (bits 15..0): The TCI of any vlan tag that was unpopped beneath the
 *                      MTK "Special Tag"
 */
struct qdma_desc_erx {
	u32 unknown0;
	u32 bitfield_0;
	u32 bitfield_1;
	u32 bitfield_3;
};

/* qdma_desc_erx bitfield_0 */

#define ERX_UNKNOWN1_MASK				GENMASK(31, 29)
#define ERX_IP6						BIT(28)
#define ERX_IP4						BIT(27)
#define ERX_IP4F					BIT(26)
#define ERX_TACK					BIT(25)
#define ERX_L2VLD					BIT(24)
#define ERX_L4F						BIT(23)
#define ERX_SPORT_MASK					GENMASK(22, 19)
#define ERX_CRSN_MASK					GENMASK(18, 14)
#define ERX_PPE_ENTRY_MASK				GENMASK(13, 0)

static inline u8 get_erx_unknown1(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_UNKNOWN1_MASK, x->bitfield_0);
}
static inline void set_erx_unknown1(struct qdma_desc_erx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_UNKNOWN1_MASK, v);
}
static inline bool is_erx_ip6(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_IP6, x->bitfield_0);
}
static inline void set_erx_ip6(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_IP6, v);
}
static inline bool is_erx_ip4(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_IP4, x->bitfield_0);
}
static inline void set_erx_ip4(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_IP4, v);
}
static inline bool is_erx_ip4f(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_IP4F, x->bitfield_0);
}
static inline void set_erx_ip4f(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_IP4F, v);
}
static inline bool is_erx_tack(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_TACK, x->bitfield_0);
}
static inline void set_erx_tack(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_TACK, v);
}
static inline bool is_erx_l2vld(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_L2VLD, x->bitfield_0);
}
static inline void set_erx_l2vld(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_L2VLD, v);
}
static inline bool is_erx_l4f(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_L4F, x->bitfield_0);
}
static inline void set_erx_l4f(struct qdma_desc_erx *x, bool v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_L4F, v);
}
static inline u8 get_erx_sport(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_SPORT_MASK, x->bitfield_0);
}
static inline void set_erx_sport(struct qdma_desc_erx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_SPORT_MASK, v);
}
static inline u8 get_erx_crsn(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_CRSN_MASK, x->bitfield_0);
}
static inline void set_erx_crsn(struct qdma_desc_erx *x, u8 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_CRSN_MASK, v);
}
static inline u16 get_erx_ppe_entry(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_PPE_ENTRY_MASK, x->bitfield_0);
}
static inline void set_erx_ppe_entry(struct qdma_desc_erx *x, u16 v) {
	x->bitfield_0 = FIELD_SET(x->bitfield_0, ERX_PPE_ENTRY_MASK, v);
}

/* qdma_desc_erx bitfield_1 */

#define ERX_UNKNOWN2_MASK				GENMASK(31, 1)
#define ERX_UNTAG					BIT(0)

static inline u32 get_erx_unknown2(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_UNKNOWN2_MASK, x->bitfield_1);
}
static inline void set_erx_unknown2(struct qdma_desc_erx *x, u32 v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ERX_UNKNOWN2_MASK, v);
}
static inline bool is_erx_untag(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_UNTAG, x->bitfield_1);
}
static inline void set_erx_untag(struct qdma_desc_erx *x, bool v) {
	x->bitfield_1 = FIELD_SET(x->bitfield_1, ERX_UNTAG, v);
}

/* qdma_desc_erx bitfield_3 */

#define ERX_SP_TAG_MASK					GENMASK(31, 16)
#define ERX_TCI_MASK					GENMASK(15, 0)

static inline u16 get_erx_sp_tag(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_SP_TAG_MASK, x->bitfield_3);
}
static inline void set_erx_sp_tag(struct qdma_desc_erx *x, u16 v) {
	x->bitfield_3 = FIELD_SET(x->bitfield_3, ERX_SP_TAG_MASK, v);
}
static inline u16 get_erx_tci(struct qdma_desc_erx *x) {
	return FIELD_GET(ERX_TCI_MASK, x->bitfield_3);
}
static inline void set_erx_tci(struct qdma_desc_erx *x, u16 v) {
	x->bitfield_3 = FIELD_SET(x->bitfield_3, ERX_TCI_MASK, v);
}

/** desc: QDMA Packet Descriptor */
struct desc {
	/**
	 * desc_unknown0: Reserved / unused, we still export the symbol so we can show
	 * it in debugging
	 */
	u32 unknown0;

	/** desc_info:  */
	struct desc_info {
		/**
		 * See accessors:
		 * is_desc_info_done()
		 * set_desc_info_done()
		 * is_desc_info_dropped()
		 * set_desc_info_dropped()
		 * is_desc_info_nls()
		 * set_desc_info_nls()
		 * get_desc_info_unknown1()
		 * set_desc_info_unknown1()
		 * get_desc_info_pkt_len()
		 * set_desc_info_pkt_len()
		 */
		u32 word;

	} info;

	/** desc_pkt_addr: Physical (DMA) address of the packet */
	u32 pkt_addr;

	/** desc_next_idx: Index of the next descriptor in the ring, max 4096. */
	u32 next_idx;

	/** desc_msg: This is either Ethernet RX, Ethernet TX, xPON RX, or xPON TX */
	union desc_msg {
		struct qdma_desc_erx erx;

		struct etx etx;

		u32 raw[4];

	} msg;

};

/**
 * Bitfield accessors for: desc_info word
 */

#define DESC_INFO_DONE					BIT(31)
#define DESC_INFO_DROPPED				BIT(30)
#define DESC_INFO_NLS					BIT(29)
#define DESC_INFO_UNKNOWN1_MASK				GENMASK(28, 16)
#define DESC_INFO_PKT_LEN_MASK				GENMASK(15, 0)


/**
 * Descriptor Done flag, this roughly means that the DSCP "belongs to the
 * driver", the hardware will set it when it is done receiving or sending and
 * will check to make sure it's not touching a DSCP that is not meant for it.
 * This is not strictly necessary, you can determine which packets are yours
 * only through ring indexes and the TX Done List, and setting and checking of
 * this flag can be deactivated.
 */
static inline bool is_desc_info_done(struct desc_info *x) {
	return FIELD_GET(DESC_INFO_DONE, x->word);
}
static inline void set_desc_info_done(struct desc_info *x, bool v) {
	x->word = FIELD_SET(x->word, DESC_INFO_DONE, v);
}

/** Packet has been dropped */
static inline bool is_desc_info_dropped(struct desc_info *x) {
	return FIELD_GET(DESC_INFO_DROPPED, x->word);
}
static inline void set_desc_info_dropped(struct desc_info *x, bool v) {
	x->word = FIELD_SET(x->word, DESC_INFO_DROPPED, v);
}

/** Unknown meaning but used on EN761627 and EN7580 */
static inline bool is_desc_info_nls(struct desc_info *x) {
	return FIELD_GET(DESC_INFO_NLS, x->word);
}
static inline void set_desc_info_nls(struct desc_info *x, bool v) {
	x->word = FIELD_SET(x->word, DESC_INFO_NLS, v);
}

/**
 * Reserved / unused, we still export the symbol so we can show it in debugging
 */
static inline u16 get_desc_info_unknown1(struct desc_info *x) {
	return FIELD_GET(DESC_INFO_UNKNOWN1_MASK, x->word);
}
static inline void set_desc_info_unknown1(struct desc_info *x, u16 v) {
	x->word = FIELD_SET(x->word, DESC_INFO_UNKNOWN1_MASK, v);
}

/** Length of the packet in bytes */
static inline u16 get_desc_info_pkt_len(struct desc_info *x) {
	return FIELD_GET(DESC_INFO_PKT_LEN_MASK, x->word);
}
static inline void set_desc_info_pkt_len(struct desc_info *x, u16 v) {
	x->word = FIELD_SET(x->word, DESC_INFO_PKT_LEN_MASK, v);
}

/** fwdesc: QDMA Hardware Forward Packet Descriptor */
struct fwdesc {
	/** fwdesc_pkt_addr: Physical (DMA) address of the packet */
	u32 pkt_addr;

	/** fwdesc_info:  */
	struct fwdesc_info {
		/**
		 * See accessors:
		 * is_fwdesc_info_ctx()
		 * set_fwdesc_info_ctx()
		 * is_fwdesc_info_ctx_ring()
		 * set_fwdesc_info_ctx_ring()
		 * get_fwdesc_info_ctx_idx()
		 * set_fwdesc_info_ctx_idx()
		 * get_fwdesc_info_pkt_len()
		 * set_fwdesc_info_pkt_len()
		 */
		u32 word;

	} info;

	/**
	 * fwdesc_msg: This is either Ethernet RX, Ethernet TX, xPON RX, or xPON TX
	 */
	union fwdesc_msg {
		struct etx etx;

		u32 raw[2];

	} msg;

};

/**
 * Bitfield accessors for: fwdesc_info word
 */

#define FWDESC_INFO_CTX					BIT(31)
#define FWDESC_INFO_CTX_RING				BIT(28)
#define FWDESC_INFO_CTX_IDX_MASK			GENMASK(27, 16)
#define FWDESC_INFO_PKT_LEN_MASK			GENMASK(15, 0)


/** True if there is a context descriptor (i.e. it is send, not a forward) */
static inline bool is_fwdesc_info_ctx(struct fwdesc_info *x) {
	return FIELD_GET(FWDESC_INFO_CTX, x->word);
}
static inline void set_fwdesc_info_ctx(struct fwdesc_info *x, bool v) {
	x->word = FIELD_SET(x->word, FWDESC_INFO_CTX, v);
}

/**
 * If ctx is true then this is the number of the context ring (0 or 1) because
 * there are 2 transmit rings.
 */
static inline bool is_fwdesc_info_ctx_ring(struct fwdesc_info *x) {
	return FIELD_GET(FWDESC_INFO_CTX_RING, x->word);
}
static inline void set_fwdesc_info_ctx_ring(struct fwdesc_info *x, bool v) {
	x->word = FIELD_SET(x->word, FWDESC_INFO_CTX_RING, v);
}

/**
 * If ctx is true then this is the index within the TX ring of the context
 * packet descriptor.
 */
static inline u16 get_fwdesc_info_ctx_idx(struct fwdesc_info *x) {
	return FIELD_GET(FWDESC_INFO_CTX_IDX_MASK, x->word);
}
static inline void set_fwdesc_info_ctx_idx(struct fwdesc_info *x, u16 v) {
	x->word = FIELD_SET(x->word, FWDESC_INFO_CTX_IDX_MASK, v);
}

/** Length of the packet in bytes */
static inline u16 get_fwdesc_info_pkt_len(struct fwdesc_info *x) {
	return FIELD_GET(FWDESC_INFO_PKT_LEN_MASK, x->word);
}
static inline void set_fwdesc_info_pkt_len(struct fwdesc_info *x, u16 v) {
	x->word = FIELD_SET(x->word, FWDESC_INFO_PKT_LEN_MASK, v);
}

#endif /* QDMA_DESC_H */
