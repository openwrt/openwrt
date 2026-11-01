// SPDX-License-Identifier: GPL-2.0
/*
 * Handler for Realtek 8 byte switch tags
 *
 * NOTE: Currently only supports protocol "4" found in the RTL9607C family of
 * SoCs, or Realtek codenamed ApolloPro hence named tag_rtl8_4ap.
 *
 * Fundamentally identical to tag_rtl8_4 by being 8 byte, 4 protocol cpu tag
 * but the insides are a little different.
 *
 * On switch->CPU the bytes 4-7 look like this:
 *
 *  |-----------------------------------+-----------------------------------|
 *  | (3) |     (3)    /    (5)         |     (2)     |  (1)  |  (1) |  (4) |
 *  | PRI |   EXTSPA   / PON_STREAM_ID  |PON_STREAM_ID|L3ROUTE| ORIG | PORT |
 *  |-----------------------------------+-----------------------------------|
 *  |    (1)   |           (15-bit)     /      (7-bit)                      |
 *  |    FBI   |      FLOW_BASED_HASH   /   DST_PORT_MASK                   |
 *  |-----------------------------------+-----------------------------------|
 *
 * On CPU->Switch however the byte 3 is also different:
 *
 *  |-----------------------------------+-----------------------------------|
 *  |              (8-bit)              |             (8-bit)               |
 *  |          Protocol [0x04]          |         TX_PORTMASK_LOW           |
 *  |-----------------------------------+-----------------------------------|
 *  |     (3-bit)    |(1)|   (1)  | (3) |(1) |(1)|   (1)   |(1) |(2)| (1) |1|
 *  |TX_PORTMASK_HIGH| X | PRI_EN | PRI |KEEP| X |LEARN_DIS|PSEL| X |DIRTX|X|
 *  |-----------------------------------+-----------------------------------|
 *  | (3-bit)|    (2-bit)   |     (4-bit)    |           (7-bit)            |
 *  | EXTSPA |   PPPOE_ACT  |    PPPOE_IDX   |        PON_STREAM_ID         |
 *  |-----------------------------------+-----------------------------------|
 *
 * With the following descriptions for new fields:
 *
 *    field           | description
 *   -----------------+-------------
 *    EXTSPA          | Source Extension Port. Relevant when PORT is CPU (9, 10, 7).
 *    PON_STREAM_ID   | PON Stream ID (GPON) or LLID index (EPON). Relevant when
 *                    | PORT is PON (5)
 *    L3ROUTE         | "This frame is an intended routed frames" as per docs
 *    ORIG            | The received frame by NIC is the original packet's format
 *                    | before switch lookup and switch didn't do any modifications
 *    FBI             | Flow Based I...? Only relevant when REASON is FB (aka < 64)
 *    FLOW_BASED_HASH | Flow Based Hash Index.
 *    DST_PORT_MASK   | Switch lookup destination port mask. bit 0: CPU, bit 1-6 are
 *                    | extension ports 0~5.
 *    PSEL            | PON stream id (GPON) / LLID index(EPON) selection.
 *    DIRTX           | Direct TX, whatever that means? Used to be called L34KEEP.
 *    PPPOE_ACT       | Egress action for PPPoE header whose Ether Type = 0x8864.
 *                    | 0: intact, 1: Add PPPoE header, 2: Remove PPPoE header,
 *                    | 3: remarking PPPoE switch TX mac action.
 *    PPPOE_IDX       | PPPoE index pointed to an entry of the PPPoE table in the
 *                    | switch. Switch TX-MAC look PPPoE table up by this index to
 *                    | get PPPoE session ID.
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/etherdevice.h>

#include "tag.h"

/* Protocols supported:
 *
 * 0x04 = RTL9607C (aka ApolloPro) DSA protocol
 */

#define RTL8_4AP_NAME			"rtl8_4ap"

#define RTL8_4_TAG_LEN			8

#define RTL8_4_PROTOCOL			GENMASK(15, 8)
#define   RTL8_4_PROTOCOL_RTL9607C	0x04
#define RTL8_4_REASON			GENMASK(7, 0)
#define   RTL8_4AP_REASON_FLOOD		202
#define   RTL8_4AP_REASON_LUT		204

#define RTL8_4AP_TX_PORTMASK_LOW	GENMASK(7, 0)
#define RTL8_4AP_TX_PORTMASK_HIGH	GENMASK(15, 13)
#define RTL9607C_TX_PORTMASK_HIGH	GENMASK(10, 8)

#define RTL8_4AP_DIRTX			BIT(1)
#define RTL8_4_LEARN_DIS		BIT(5)
#define RTL8_4_KEEP			BIT(7)

#define RTL8_4_RX_SOURCE_PORT		GENMASK(3, 0)

#define RTL8_4AP_DST_PMASK		GENMASK(6, 0)
#define   RTL8_4AP_DST_CPU_MASK		BIT(0)

static void rtl8_4ap_write_tag(struct sk_buff *skb, struct net_device *dev,
			       void *tag)
{
	u16 pmask = dsa_xmit_port_mask(skb, dev);
	__be16 tag16[RTL8_4_TAG_LEN / 2];
	u16 pmask_low, pmask_high;

	/* Set Realtek EtherType */
	tag16[0] = htons(ETH_P_REALTEK);

	pmask_low = FIELD_GET(RTL8_4AP_TX_PORTMASK_LOW, pmask);
	/* Set Protocol; set TX (CPU->switch) low forwarding port mask */
	tag16[1] = htons(FIELD_PREP(RTL8_4_PROTOCOL, RTL8_4_PROTOCOL_RTL9607C) |
			 FIELD_PREP(RTL8_4AP_TX_PORTMASK_LOW, pmask_low));

	pmask_high = FIELD_GET(RTL9607C_TX_PORTMASK_HIGH, pmask);
	/* set high forwarding port mask, KEEP, LEARN_DIS and DIRTX */
	tag16[2] = htons(FIELD_PREP(RTL8_4AP_TX_PORTMASK_HIGH, pmask_high) |
			 RTL8_4_LEARN_DIS | RTL8_4_KEEP | RTL8_4AP_DIRTX);

	/* Zero EXTSPA, PPPOE_ACT/IDX and PON_STREAM_ID */
	tag16[3] = 0;

	memcpy(tag, tag16, RTL8_4_TAG_LEN);
}

static struct sk_buff *rtl8_4ap_tag_xmit(struct sk_buff *skb,
					 struct net_device *dev)
{
	skb_push(skb, RTL8_4_TAG_LEN);

	dsa_alloc_etype_header(skb, RTL8_4_TAG_LEN);

	rtl8_4ap_write_tag(skb, dev, dsa_etype_header_pos_tx(skb));

	return skb;
}

static int rtl8_4ap_read_tag(struct sk_buff *skb, struct net_device *dev,
			     void *tag)
{
	__be16 tag16[RTL8_4_TAG_LEN / 2];
	u16 etype;
	u8 reason;
	u8 proto;
	u8 port;
	u8 dst_pmask;
	bool is_lut;

	memcpy(tag16, tag, RTL8_4_TAG_LEN);

	/* Parse Realtek EtherType */
	etype = ntohs(tag16[0]);
	if (unlikely(etype != ETH_P_REALTEK)) {
		dev_warn_ratelimited(&dev->dev,
				     "non-realtek ethertype 0x%04x\n", etype);
		return -EPROTO;
	}

	/* Parse Protocol */
	proto = FIELD_GET(RTL8_4_PROTOCOL, ntohs(tag16[1]));
	if (unlikely(proto != RTL8_4_PROTOCOL_RTL9607C)) {
		dev_warn_ratelimited(&dev->dev,
				     "unknown realtek protocol 0x%02x\n", proto);
		return -EPROTO;
	}

	/* Parse REASON */
	reason = FIELD_GET(RTL8_4_REASON, ntohs(tag16[1]));

	/* Parse RX (switch->CPU) */
	port = FIELD_GET(RTL8_4_RX_SOURCE_PORT, ntohs(tag16[2]));
	skb->dev = dsa_conduit_find_user(dev, 0, port);
	if (!skb->dev) {
		dev_warn_ratelimited(&dev->dev,
				     "could not find user for port %d\n", port);
		return -ENOENT;
	}

	/* Parse destination port mask */
	dst_pmask = FIELD_GET(RTL8_4AP_DST_PMASK, ntohs(tag16[3]));

	is_lut = reason == RTL8_4AP_REASON_LUT;
	if (!is_lut && reason != RTL8_4AP_REASON_FLOOD) {
		dev_warn_ratelimited(&dev->dev, "Unknown reason code: %d\n", reason);
	} else if (!is_lut || (dst_pmask & RTL8_4AP_DST_CPU_MASK)) {
		dsa_default_offload_fwd_mark(skb);
	}

	return 0;
}

static struct sk_buff *rtl8_4ap_tag_rcv(struct sk_buff *skb,
					struct net_device *dev)
{
	if (unlikely(!pskb_may_pull(skb, RTL8_4_TAG_LEN)))
		return NULL;

	if (unlikely(rtl8_4ap_read_tag(skb, dev, dsa_etype_header_pos_rx(skb))))
		return NULL;

	/* Remove tag and recalculate checksum */
	skb_pull_rcsum(skb, RTL8_4_TAG_LEN);

	dsa_strip_etype_header(skb, RTL8_4_TAG_LEN);

	return skb;
}

/* Ethertype RTL9607C version */
static const struct dsa_device_ops rtl8_4ap_netdev_ops = {
	.name = RTL8_4AP_NAME,
	.proto = DSA_TAG_PROTO_RTL8_4AP,
	.xmit = rtl8_4ap_tag_xmit,
	.rcv = rtl8_4ap_tag_rcv,
	.needed_headroom = RTL8_4_TAG_LEN,
};

DSA_TAG_DRIVER(rtl8_4ap_netdev_ops);

MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_RTL8_4AP, RTL8_4AP_NAME);

static struct dsa_tag_driver *dsa_tag_drivers[] = {
	&DSA_TAG_DRIVER_NAME(rtl8_4ap_netdev_ops),
};

module_dsa_tag_drivers(dsa_tag_drivers);

MODULE_DESCRIPTION("DSA tag driver for Realtek 8 byte protocol 4 RTL9607C tag");
MODULE_LICENSE("GPL");
