// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha AN7552/AN7563 xPON - driver entry, interrupts and datapath
 *
 * One platform device, two halves: the GPON MAC (an7552_mac.h, the rest of
 * this module) and the PON SerDes PHY (an7552_phy.h, brought up by
 * an7552_phy_probe()).  This file owns the DT node and the probe order:
 * PHY windows and line first, then the GPON window, then the WAN GDM the
 * datapath runs on.
 *
 * The MAC has no DMA of its own.  It rides on the WAN GDM of airoha-eth
 * ("ethernet" phandle) through the airoha_xpon_* client API: upstream
 * frames are queued with the GEM port and T-CONT channel in QDMA message
 * word 0, downstream frames come back with the source GEM port and the
 * OMCI (OAM) flag.  Data GEM ports are bridged to the "pon%d" netdev, OMCI
 * frames to userspace over generic netlink.
 */
#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/rtnetlink.h>
#include <linux/soc/airoha/airoha_xpon.h>
#include <linux/unaligned.h>
#include <net/sch_generic.h>
#include <net/tcx.h>

#include "an7552_mac.h"

/* NP SCU DMTC: dying gasp status, write 1 to clear (ecnt_scu.c:72) */

/* DSCP in the first 16 bits of the IP header (both 6 bits, 0..63) */

/*
 * traffic_status_refresh(): GPON traffic is up in O5.  Changes go
 * to the PHY (PON_SET 0x1b), which feeds the optical front end.
 */
static void an7552_mac_traffic(struct an7552_mac *x)
{
	bool up = READ_ONCE(x->state) == GPON_O5;
	int ret;

	if (up == READ_ONCE(x->traffic_up))
		return;
	WRITE_ONCE(x->traffic_up, up);
	ret = an7552_phy_set_traffic(x->phy, up);

	/*
	 * -EBUSY here is the front end saying it cannot take the mode with its
	 * transmitter running, which means the two sides are running in
	 * different modes and every burst is going out timed for the wrong one.
	 * The mode itself was handed over long before O5 and should have taken,
	 * so this is worth saying rather than returning it to a caller that
	 * cannot do anything about it.
	 */
	if (ret)
		dev_warn(x->dev, "traffic %s, optical front end: %d\n",
			 up ? "up" : "down", ret);
}

void an7552_mac_carrier(struct an7552_mac *x)
{
	struct net_device *ndev = READ_ONCE(x->ndev);
	enum an7552_gpon_state state;
	bool up;

	/* pon0 is gone (front end away) or has never been registered */
	if (!ndev)
		return;

	/*
	 * O6 is Type B protection: the light was lost in O5, the ONU-ID, the
	 * T-CONTs and the GEM ports are kept, and a Popup of the protecting OLT
	 * brings the ONU back (gpon_loss_of_light()).  The carrier stays for
	 * that time: the link goes down when TO2 runs out and the ONU leaves
	 * O6 for O1, and not before.  What is sent meanwhile is dropped by
	 * an7552_mac_xmit(), which sends in O5 only, so nothing queues up for
	 * the moment the datapath is back, and netifd never sees a link that
	 * returns within TO2 - no DHCP or PPPoE session is torn down for it.
	 * The front end is told about the traffic separately, below, and stays
	 * O5 only as in the vendor's traffic_status_refresh().
	 */
	state = READ_ONCE(x->state);
	up = (state == GPON_O5 || state == GPON_O6) &&
	     READ_ONCE(x->us_gem) != GPON_UNASSIGN_GEM_ID;

	an7552_mac_traffic(x);

	if (up && !netif_carrier_ok(ndev)) {
		netif_carrier_on(ndev);
	} else if (!up && netif_carrier_ok(ndev)) {
		netif_carrier_off(ndev);
		/* the PPE would go on sending into a stopped T-CONT */
		an7552_mac_ppe_flush(x);
	}
}

/* ---- datapath ---- */

/*
 * gwan_prepare_tx_message(): OMCI frames go out on the OMCC GEM port with
 * the OAM flag set, channel 0; a trailing OMCI CRC/MIC is removed first
 * (remove_omci_crc_if_exist()).
 */
int an7552_mac_omci_tx(struct an7552_mac *x, const void *data, size_t len)
{
	struct airoha_xpon_tx_info info = {
		.channel = 0,
		.queue = GPON_OMCI_TX_QUEUE,
		.oam = true,
	};
	const u8 *hdr = data;
	struct sk_buff *skb;
	u16 omcc = READ_ONCE(x->omcc);
	struct net_device *ndev = READ_ONCE(x->ndev);

	an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx request len %zu omcc 0x%x up_omci %d: %*ph\n",
		       len, omcc, READ_ONCE(x->up_omci), (int)min_t(size_t, len, 64), data);

	/* the front end is away: pon0 has been given back */
	if (!ndev) {
		an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx refused: no ndev\n");
		return -ENODEV;
	}
	if (omcc == GPON_UNASSIGN_GEM_ID) {
		an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx refused: no OMCC\n");
		return -ENOTCONN;
	}
	if (!READ_ONCE(x->up_omci)) {
		an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx refused: up_omci off\n");
		return -EPERM;
	}
	if (len < 10) {
		an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx refused: short frame\n");
		return -EINVAL;
	}

	if (hdr[3] == OMCI_BASIC_MSG_DEV_ID) {
		if (len == OMCI_BASIC_MSG_FIX_LEN)
			len -= OMCI_CRC_LEN;
		else if (len != OMCI_BASIC_MSG_FIX_LEN - OMCI_CRC_LEN)
			return -EINVAL;
	} else if (hdr[3] == OMCI_EXTENDED_MSG_DEV_ID) {
		/* G.988 extended header: contents length at bytes 8-9 */
		size_t msg_len = get_unaligned_be16(&hdr[8]) + 10;

		if (len > msg_len)
			len = msg_len;
	}

	skb = netdev_alloc_skb(ndev, len);
	if (!skb)
		return -ENOMEM;
	skb_put_data(skb, data, len);

	unsigned int skb_len = skb->len;
	int ret;

	info.gem_port = omcc;
	x->stats.omci_tx++;

	ret = airoha_xpon_tx(skb, &info);
	an7552_mac_dbg(x, AN7552_DBG_MAC, "omci tx gem 0x%x len %u: airoha_xpon_tx %d\n",
		       omcc, skb_len, ret);

	/* -EBUSY: the ring is full and the skb was left to us; userspace
	 * retries, so drop it here rather than leaking it. */
	if (ret == -EBUSY)
		dev_kfree_skb_any(skb);

	return ret;
}

static void an7552_mac_rx(void *priv, struct sk_buff *skb,
			   const struct airoha_xpon_rx_info *info)
{
	struct an7552_mac *x = priv;
	struct net_device *ndev = x->ndev;
	struct an7552_gpon_gem *g;

	an7552_mac_dbg(x, AN7552_DBG_MAC, "rx oam %d gem 0x%x len %u crc_err %d\n",
		       info->oam, info->gem_port, skb->len, info->crc_err);

	if (info->oam) {
		/*
		 * An extended message (up to 1980 bytes) outgrows the first RX
		 * buffer (about 1.6 KB of the 2 KB page half) and arrives as
		 * page fragments; the genl copy reads skb->data.
		 */
		if (skb_is_nonlinear(skb) && skb_linearize(skb)) {
			ndev->stats.rx_dropped++;
			dev_kfree_skb_any(skb);
			return;
		}
		x->stats.omci_rx++;
		an7552_mac_genl_omci_rx(x, skb);
		return;
	}

	if (info->gem_port >= GPON_MAX_GEM_ID ||
	    !READ_ONCE(x->gem[info->gem_port].valid) || info->crc_err) {
		x->stats.rx_drop_gem++;
		ndev->stats.rx_dropped++;
		dev_kfree_skb_any(skb);
		return;
	}

	g = &x->gem[info->gem_port];
	atomic64_inc(&g->rx_packets);
	atomic64_add(skb->len, &g->rx_bytes);
	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += skb->len;

	/*
	 * The GDM checked the checksums (through the VLAN/PPPoE headers it
	 * parses): the stack need not.  A bad or unchecked one stays for the
	 * stack to verify and drop.
	 */
	if (info->csum_ok && (ndev->features & NETIF_F_RXCSUM)) {
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		x->stats.rx_csum_ok++;
	} else if (info->csum_bad) {
		x->stats.rx_csum_bad++;
	} else {
		x->stats.rx_csum_none++;
	}

	skb->dev = ndev;
	skb->protocol = eth_type_trans(skb, ndev);
	/* the airoha RX NAPI flushes the GRO list at the end of its poll */
	if (info->napi)
		napi_gro_receive(info->napi, skb);
	else
		netif_receive_skb(skb);
}

/*
 * The PCP of a frame without a VLAN tag from its DSCP (the 802.1p mapper
 * "unmarked frame option" 0): IPv4 type of service, IPv6 traffic class,
 * both without the 2 ECN bits.  skb->data is the Ethernet header here;
 * only the first two bytes of the IP header are read, through
 * skb_header_pointer() (the frame may not be linear).  Other ethertypes
 * and truncated frames: 0.
 */
static u8 an7552_mac_dscp_pcp(struct an7552_mac *x, struct sk_buff *skb)
{
	u8 buf[2], *ip;
	u16 w;

	if (skb->protocol != htons(ETH_P_IP) &&
	    skb->protocol != htons(ETH_P_IPV6))
		return 0;

	ip = skb_header_pointer(skb, ETH_HLEN, sizeof(buf), buf);
	if (!ip)
		return 0;

	/* the first 16 bits: IPv4 version/IHL + TOS, IPv6 version + traffic
	 * class + 4 flow label bits */
	w = get_unaligned_be16(ip);

	return READ_ONCE(x->dscp_pcp[skb->protocol == htons(ETH_P_IP) ?
				     FIELD_GET(IPV4_TOS_DSCP, w) :
				     FIELD_GET(IPV6_TCLASS_DSCP, w)]);
}

/*
 * Upstream classification: the first rule matching the outer VLAN id and
 * PCP picks the GEM port, else the default one; the PCP picks the queue
 * of the T-CONT channel.  Untagged IP frames take the PCP from the DSCP
 * map when it is enabled (CMD_DSCP_MAP).  @dei is the DEI bit of the outer
 * tag (the sysfs "dei_mark" attribute decides whether it reaches the TX
 * descriptor).
 */
static u16 an7552_mac_us_gem(struct an7552_mac *x, u16 vid, u8 pcp)
{
	int i;

	for (i = 0; i < GPON_MAX_US_RULES; i++) {
		struct an7552_us_rule *r = &x->us_rule[i];

		if (!smp_load_acquire(&r->valid))
			continue;
		if (r->vid != GPON_US_VID_ANY && r->vid != vid)
			continue;
		if (r->pcp_mask && !(r->pcp_mask & BIT(pcp)))
			continue;
		return r->gem;
	}

	return READ_ONCE(x->us_gem);
}

static u16 an7552_mac_classify(struct an7552_mac *x, struct sk_buff *skb,
				u8 *pcp, bool *dei)
{
	u16 vid = GPON_US_VID_ANY, tci;

	*pcp = 0;
	*dei = false;
	if (skb_vlan_tag_present(skb)) {
		tci = skb_vlan_tag_get(skb);
	} else if (eth_type_vlan(skb->protocol) &&
		   skb_headlen(skb) >= VLAN_ETH_HLEN) {
		tci = ntohs(((struct vlan_ethhdr *)skb->data)->h_vlan_TCI);
	} else {
		tci = 0xffff;
	}
	if (tci != 0xffff) {
		vid = tci & VLAN_VID_MASK;
		*pcp = tci >> VLAN_PRIO_SHIFT;
		*dei = tci & VLAN_CFI_MASK;
	} else if (smp_load_acquire(&x->dscp_en)) {
		*pcp = an7552_mac_dscp_pcp(x, skb);
	}

	return an7552_mac_us_gem(x, vid, *pcp);
}

/*
 * Any tc filter or tcx program on the egress of @dev: the CPU path of a
 * frame may change it there (gpond's VLAN rewrite), the PPE path never
 * runs it.  An empty clsact qdisc does not count.
 */
static bool an7552_mac_tc_egress(struct net_device *dev)
{
	struct bpf_mprog_entry *entry;
	struct mini_Qdisc *miniq;
	bool tc = false;

	rcu_read_lock();
	entry = rcu_dereference(dev->tcx_egress);
	if (entry) {
		miniq = rcu_dereference(tcx_entry(entry)->miniq);
		tc = bpf_mprog_total(entry) ||
		     (miniq && READ_ONCE(miniq->filter_list));
	}
	rcu_read_unlock();

	return tc;
}

/*
 * airoha_xpon_ops.ppe_upstream: an upstream flow the PPE would forward to
 * pon0 gets what an7552_mac_xmit() would give its frames once pon0's tc
 * rules rewrote them: the VID and P-bit on the fibre (vid_map, the tc
 * rewrite of gpond), the GEM port of the upstream rules, its T-CONT and
 * the queue of the P-bit.  Untagged flows take the P-bit from the DSCP as
 * an7552_mac_classify() does.  Anything that would not be sent - no O5,
 * no valid GEM, a router VID the tc rules do not translate - stays on the
 * CPU.  With tc on the egress of pon0 only what the VID map describes is
 * taken: a tagged VID needs an entry, an untagged flow is left to the CPU,
 * whatever gpond's rules do (they could change the frame where the PPE
 * cannot follow).  Lockless like the xmit path; every change of what is
 * read here unbinds the flows again (an7552_mac_ppe_flush()); a tc change
 * does not, so tc is to be in place before the switch is turned on.
 */
static int an7552_mac_ppe_upstream(void *priv, struct airoha_xpon_ppe_us *us)
{
	struct an7552_mac *x = priv;
	u16 vid = GPON_US_VID_ANY, gem;
	bool mapped, tc;
	u8 pcp, ch;
	int i;

	if (!READ_ONCE(x->ppe_upstream) || !READ_ONCE(x->up_data) ||
	    READ_ONCE(x->state) != GPON_O5)
		return -EOPNOTSUPP;

	tc = an7552_mac_tc_egress(x->ndev);
	mapped = tc;

	if (us->vid) {
		vid = us->vid;
		pcp = us->pcp;
		for (i = 0; i < GPON_MAX_VID_MAP; i++) {
			struct an7552_vid_map *m = &x->vid_map[i];

			if (!smp_load_acquire(&m->valid))
				continue;
			mapped = true;	/* stays set unless this VID matches */
			if (m->uni_vid != us->vid ||
			    (m->pcp_mask && !(m->pcp_mask & BIT(us->pcp))))
				continue;
			vid = m->ani_vid;
			if (m->pcp != GPON_PCP_KEEP)
				pcp = m->pcp;
			mapped = false;
			break;
		}
		/*
		 * tc on pon0, or entries for other VIDs, and none for this
		 * one: tc may rewrite or drop these frames
		 */
		if (mapped)
			return -ENOENT;
	} else {
		if (tc)
			return -ENOENT;
		/*
		 * Untagged on the raw pon0 leaves untagged on the CPU path as
		 * well: gpond rewrites only tagged router frames with tc
		 * (plan_tc(), eth_vlan.c), untagged ones go through VLAN
		 * devices.  Bridged flows, where tc on a UNI could tag them,
		 * are not offered here (airoha_ppe.c refuses PPE_PKT_TYPE_BRIDGE).
		 */
		pcp = smp_load_acquire(&x->dscp_en) ?
		      READ_ONCE(x->dscp_pcp[us->dscp]) : 0;
	}

	gem = an7552_mac_us_gem(x, vid, pcp);
	if (gem >= GPON_MAX_GEM_ID || !READ_ONCE(x->gem[gem].valid))
		return -ENOENT;
	ch = READ_ONCE(x->gem[gem].channel);
	if (ch >= GPON_MAX_TCONT)
		return -ENOENT;

	us->vid = us->vid ? vid : 0;
	us->pcp = pcp;
	us->gem_port = gem;
	us->channel = ch;
	us->queue = READ_ONCE(x->pcp_queue[pcp]);

	return 0;
}

static netdev_tx_t an7552_mac_xmit(struct sk_buff *skb,
				    struct net_device *ndev)
{
	struct an7552_mac *x = ndev_to_xpon(ndev);
	struct airoha_xpon_tx_info info = { };
	unsigned int len;
	bool dei;
	u8 pcp, ch;
	u16 gem;
	int ret;

	/*
	 * Nothing below pads: a short frame would leave in a GEM frame as it
	 * is, and the OLT side drops runts (a 36-byte PADI on VLAN 1501 got no
	 * answer).  The vendor pads every data frame to ETH_ZLEN, the VLAN
	 * tag counted (xpon_1g pwan/xpon_netif.c:617-626).  Frees the skb on
	 * failure.
	 */
	if (eth_skb_pad(skb)) {
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	len = skb->len;

	gem = an7552_mac_classify(x, skb, &pcp, &dei);
	ch = gem < GPON_MAX_GEM_ID ? READ_ONCE(x->gem[gem].channel) : 0xff;
	if (!READ_ONCE(x->up_data) || gem >= GPON_MAX_GEM_ID ||
	    !READ_ONCE(x->gem[gem].valid) || ch >= GPON_MAX_TCONT ||
	    READ_ONCE(x->state) != GPON_O5) {
		ndev->stats.tx_dropped++;
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	info.gem_port = gem;
	info.channel = ch;
	info.queue = READ_ONCE(x->pcp_queue[pcp]);
	info.dei = dei && READ_ONCE(x->dei_mark);
	ret = airoha_xpon_tx(skb, &info);
	if (ret == -EBUSY) {
		/*
		 * Ring full: the queues are stopped and the skb is still
		 * ours to hand back.  NETDEV_TX_BUSY requeues it; the qdisc
		 * will be woken by airoha_xpon_wake_txqs() on completion.
		 * Anything else means the frame was consumed and dropped.
		 */
		return NETDEV_TX_BUSY;
	}
	if (ret) {
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	atomic64_inc(&x->gem[gem].tx_packets);
	atomic64_add(len, &x->gem[gem].tx_bytes);
	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += len;

	return NETDEV_TX_OK;
}

static int an7552_mac_open(struct net_device *ndev)
{
	struct an7552_mac *x = ndev_to_xpon(ndev);

	/*
	 * Two ways of having no optical path, and both stop here:
	 *
	 *  - the DOWN bit, which an7552_mac_down() sets when a front end
	 *    unregisters.  The interface stays registered across that
	 *    (unregistering a netdev cannot be undone), so this is where that
	 *    bit is enforced.
	 *  - no front end has registered yet, which is the state this MAC probes
	 *    in, because the registry is told about it last and the EN7571 driver
	 *    is loaded after this one.  The DOWN bit does not cover this case:
	 *    it starts clear and only ever gets set by a teardown, so a MAC that
	 *    has never seen a front end would otherwise start, run the
	 *    activation machine and sit in O1 with a transmitter it has no
	 *    regulator to switch on.  That also made priv->trans_power_on stale
	 *    and forced a SerDes reset when the front end finally arrived.
	 *
	 * Refusing here rather than at probe is the point: this core probes and
	 * comes up with no optical front end at all, so that pon0 exists and
	 * userspace has something to report the absence with.  Registering does
	 * not wait for light; starting does.
	 */
	if (an7552_mac_is_down(x) || !READ_ONCE(x->phy->laser_set_traffic))
		return -ENODEV;

	netif_carrier_off(ndev);
	netif_start_queue(ndev);
	an7552_mac_start(x);

	return 0;
}

static int an7552_mac_stop_netdev(struct net_device *ndev)
{
	struct an7552_mac *x = ndev_to_xpon(ndev);

	netif_stop_queue(ndev);
	an7552_mac_stop(x);
	netif_carrier_off(ndev);

	return 0;
}

/* the PPE egress MTU of GDM2 follows pon0 */
static int an7552_mac_change_mtu(struct net_device *ndev, int mtu)
{
	if (ndev->mtu == mtu)
		return 0;

	WRITE_ONCE(ndev->mtu, mtu);
	airoha_xpon_mtu_changed(ndev);

	return 0;
}

static const struct net_device_ops an7552_mac_netdev_ops = {
	.ndo_open		= an7552_mac_open,
	.ndo_stop		= an7552_mac_stop_netdev,
	.ndo_start_xmit		= an7552_mac_xmit,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_setup_tc		= airoha_xpon_setup_tc,
	.ndo_change_mtu		= an7552_mac_change_mtu,
};

/* ---- interrupts and PHY events ---- */

/*
 * xpon_dying_gasp_interrupt(): GPON queues the Dying_Gasp PLOAMs
 * right away.
 */
static irqreturn_t an7552_mac_dg_irq(int irq, void *data)
{
	struct an7552_mac *x = data;

	regmap_set_bits(x->scu, SCU_DMTC, SCU_DYING_GASP_STATUS);
	if (READ_ONCE(x->started))
		an7552_mac_dying_gasp(x);

	return IRQ_HANDLED;
}

static void an7552_mac_phy_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(w, struct an7552_mac, phy_work);
	unsigned long ev = xchg(&x->phy_events, 0);
	int i;

	for_each_set_bit(i, &ev, BITS_PER_LONG) {
		an7552_mac_dbg(x, AN7552_DBG_ACT, "phy event %d (%s) in O%d\n", i,
			 i == AN7552_EVENT_LOS ? "LOS" :
			 i == AN7552_EVENT_LOF ? "LOF" :
			 i == AN7552_EVENT_PHY_RDY ? "PHY ready" : "other",
			 x->state);
		an7552_mac_handle_phy_event(x, i);
	}
}

/* Called from the PHY event work, with no PHY lock held. */
void an7552_mac_phy_event(struct an7552_mac *x, enum an7552_xpon_event ev)
{
	/* xpon_phy_los_illegal_handler(): an illegal event is a LOS */
	if (ev == AN7552_EVENT_ILLG)
		ev = AN7552_EVENT_LOS;
	an7552_mac_dbg(x, AN7552_DBG_ACT, "phy event %d\n", ev);
	/* LOS/LOF/ready matter to the activation; the rest is informational */
	if (ev != AN7552_EVENT_LOS && ev != AN7552_EVENT_LOF &&
	    ev != AN7552_EVENT_PHY_RDY)
		return;

	set_bit(ev, &x->phy_events);
	queue_work(x->wq, &x->phy_work);
}

static ssize_t gpon_sn_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	u8 sn[GPON_SN_LEN];
	int i, len = 0;

	an7552_mac_get_identity(x, sn, false);
	for (i = 0; i < GPON_SN_LEN; i++)
		len += sysfs_emit_at(buf, len, "%02x", sn[i]);
	len += sysfs_emit_at(buf, len, "\n");

	return len;
}

static ssize_t gpon_sn_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));

	if (an7552_mac_set_identity_text(x, buf, false))
		return -EINVAL;

	return count;
}
static DEVICE_ATTR_RW(gpon_sn);

static ssize_t gpon_pwd_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));

	if (an7552_mac_set_identity_text(x, buf, true))
		return -EINVAL;

	return count;
}
static DEVICE_ATTR_WO(gpon_pwd);

static ssize_t activate_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	u64 val;
	int ret;

	ret = kstrtoull(buf, 0, &val);
	if (ret)
		return ret;
	if (!val)
		return count;

	return an7552_mac_activate(x) ? -EIO : count;
}
static DEVICE_ATTR_WO(activate);

static ssize_t state_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	int len;

	guard(mutex)(&x->lock);
	len = sysfs_emit(buf, "O%d onu_id=0x%02x omcc=0x%04x eqd=0x%08x alarms=0x%08x\n",
			 x->state, x->onu_id, x->omcc, x->eqd, x->alarms);

	return len;
}
static DEVICE_ATTR_RO(state);

static ssize_t ppe_downstream_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%u\n", airoha_xpon_get_ppe_downstream());
}

static ssize_t ppe_downstream_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	u64 val;
	int ret;

	ret = kstrtoull(buf, 0, &val);
	if (ret)
		return ret;
	airoha_xpon_set_ppe_downstream(!!val);

	return count;
}
static DEVICE_ATTR_RW(ppe_downstream);

/*
 * Upstream flows of pon0 in the PPE (an7552_mac_ppe_upstream()); off by
 * default, like downstream, until tested against an OLT.  Off unbinds the
 * flows already in the PPE.
 */
static ssize_t ppe_upstream_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));

	return sysfs_emit(buf, "%u\n", READ_ONCE(x->ppe_upstream));
}

static ssize_t ppe_upstream_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	bool val;
	int ret;

	ret = kstrtobool(buf, &val);
	if (ret)
		return ret;
	WRITE_ONCE(x->ppe_upstream, val);
	if (!val)
		an7552_mac_ppe_flush(x);

	return count;
}
static DEVICE_ATTR_RW(ppe_upstream);

/* ---- QoS (WAN QDMA congestion control: DEI drop/mark and WRED) ---- */

/*
 * The knobs are one number each, so sysfs carries them better than debugfs
 * and they stay available when CONFIG_DEBUG_FS is off.  The net_device has
 * a single attribute-group slot of its own (netstat and the phys port take
 * the next ones), so they live flat in "gpon".
 */
enum an7552_qos_kind {
	AN7552_QOS_DEI_MARK,
	AN7552_QOS_DEI_DROP,
	AN7552_QOS_WRED,
	AN7552_QOS_WRED_THR,
	AN7552_QOS_WRED_PROB,
};

struct an7552_qos_attr {
	struct device_attribute dev_attr;
	enum an7552_qos_kind kind;
	enum airoha_xpon_pkt_class cls;
	unsigned int idx;
};

static ssize_t an7552_qos_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	const struct an7552_qos_attr *qa =
		container_of(attr, struct an7552_qos_attr, dev_attr);
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	struct airoha_xpon_wred wred;
	int ret;

	if (qa->kind == AN7552_QOS_DEI_MARK)
		return sysfs_emit(buf, "%d\n", READ_ONCE(x->dei_mark));

	ret = airoha_xpon_get_wred(&wred);
	if (ret)
		return ret;

	switch (qa->kind) {
	case AN7552_QOS_DEI_DROP:
		return sysfs_emit(buf, "%d\n", wred.dei_drop);
	case AN7552_QOS_WRED:
		return sysfs_emit(buf, "%d\n", wred.wred);
	case AN7552_QOS_WRED_THR:
		return sysfs_emit(buf, "%u\n",
				  (unsigned int)wred.thr[qa->cls][qa->idx]);
	default:
		return sysfs_emit(buf, "%u\n",
				  (unsigned int)wred.prob[qa->cls][qa->idx]);
	}
}

static ssize_t an7552_qos_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	const struct an7552_qos_attr *qa =
		container_of(attr, struct an7552_qos_attr, dev_attr);
	struct an7552_mac *x = ndev_to_xpon(to_net_dev(dev));
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	switch (qa->kind) {
	case AN7552_QOS_DEI_MARK:
		if (val > 1)
			return -ERANGE;
		WRITE_ONCE(x->dei_mark, val);
		return count;
	case AN7552_QOS_DEI_DROP:
		if (val > 1)
			return -ERANGE;
		ret = airoha_xpon_set_dei_drop(val);
		break;
	case AN7552_QOS_WRED:
		if (val > 1)
			return -ERANGE;
		ret = airoha_xpon_set_wred(val);
		break;
	case AN7552_QOS_WRED_THR:
		if (val > AIROHA_XPON_WRED_THR_MAX)
			return -ERANGE;
		ret = airoha_xpon_set_wred_thr(qa->cls, qa->idx, val);
		break;
	case AN7552_QOS_WRED_PROB:
		if (val > 100)
			return -ERANGE;
		ret = airoha_xpon_set_wred_prob(qa->cls, qa->idx, val);
		break;
	default:
		return -EINVAL;
	}

	return ret ? ret : count;
}

#define AN7552_QOS_ATTR(_name, _kind, _cls, _idx)			\
	static struct an7552_qos_attr an7552_qos_##_name = {		\
		.dev_attr = __ATTR(_name, 0600, an7552_qos_show,	\
				   an7552_qos_store),			\
		.kind	= _kind,						\
		.cls	= _cls,							\
		.idx	= _idx,							\
	}

AN7552_QOS_ATTR(dei_mark, AN7552_QOS_DEI_MARK, AIROHA_XPON_PKT_NORMAL, 0);
AN7552_QOS_ATTR(dei_drop, AN7552_QOS_DEI_DROP, AIROHA_XPON_PKT_NORMAL, 0);
AN7552_QOS_ATTR(wred, AN7552_QOS_WRED, AIROHA_XPON_PKT_NORMAL, 0);
AN7552_QOS_ATTR(wred_thr_n0, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_NORMAL, 0);
AN7552_QOS_ATTR(wred_thr_n1, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_NORMAL, 1);
AN7552_QOS_ATTR(wred_thr_n2, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_NORMAL, 2);
AN7552_QOS_ATTR(wred_thr_n3, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_NORMAL, 3);
AN7552_QOS_ATTR(wred_thr_n4, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_NORMAL, 4);
AN7552_QOS_ATTR(wred_thr_d0, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_DEI, 0);
AN7552_QOS_ATTR(wred_thr_d1, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_DEI, 1);
AN7552_QOS_ATTR(wred_thr_d2, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_DEI, 2);
AN7552_QOS_ATTR(wred_thr_d3, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_DEI, 3);
AN7552_QOS_ATTR(wred_thr_d4, AN7552_QOS_WRED_THR, AIROHA_XPON_PKT_DEI, 4);
AN7552_QOS_ATTR(wred_prob_n0, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_NORMAL, 0);
AN7552_QOS_ATTR(wred_prob_n1, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_NORMAL, 1);
AN7552_QOS_ATTR(wred_prob_n2, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_NORMAL, 2);
AN7552_QOS_ATTR(wred_prob_n3, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_NORMAL, 3);
AN7552_QOS_ATTR(wred_prob_d0, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_DEI, 0);
AN7552_QOS_ATTR(wred_prob_d1, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_DEI, 1);
AN7552_QOS_ATTR(wred_prob_d2, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_DEI, 2);
AN7552_QOS_ATTR(wred_prob_d3, AN7552_QOS_WRED_PROB, AIROHA_XPON_PKT_DEI, 3);

static struct attribute *an7552_gpon_attrs[] = {
	&dev_attr_gpon_sn.attr,
	&dev_attr_gpon_pwd.attr,
	&dev_attr_activate.attr,
	&dev_attr_state.attr,
	&dev_attr_ppe_downstream.attr,
	&dev_attr_ppe_upstream.attr,
	&an7552_qos_dei_mark.dev_attr.attr,
	&an7552_qos_dei_drop.dev_attr.attr,
	&an7552_qos_wred.dev_attr.attr,
	&an7552_qos_wred_thr_n0.dev_attr.attr,
	&an7552_qos_wred_thr_n1.dev_attr.attr,
	&an7552_qos_wred_thr_n2.dev_attr.attr,
	&an7552_qos_wred_thr_n3.dev_attr.attr,
	&an7552_qos_wred_thr_n4.dev_attr.attr,
	&an7552_qos_wred_thr_d0.dev_attr.attr,
	&an7552_qos_wred_thr_d1.dev_attr.attr,
	&an7552_qos_wred_thr_d2.dev_attr.attr,
	&an7552_qos_wred_thr_d3.dev_attr.attr,
	&an7552_qos_wred_thr_d4.dev_attr.attr,
	&an7552_qos_wred_prob_n0.dev_attr.attr,
	&an7552_qos_wred_prob_n1.dev_attr.attr,
	&an7552_qos_wred_prob_n2.dev_attr.attr,
	&an7552_qos_wred_prob_n3.dev_attr.attr,
	&an7552_qos_wred_prob_d0.dev_attr.attr,
	&an7552_qos_wred_prob_d1.dev_attr.attr,
	&an7552_qos_wred_prob_d2.dev_attr.attr,
	&an7552_qos_wred_prob_d3.dev_attr.attr,
	NULL,
};

#undef AN7552_QOS_ATTR

static const struct attribute_group an7552_gpon_group = {
	.name	= "gpon",
	.attrs	= an7552_gpon_attrs,
};

/* ---- probe ---- */

static int an7552_mac_get_wan(struct an7552_mac *x)
{
	struct device_node *np;

	np = of_parse_phandle(x->dev->of_node, "ethernet", 0);
	if (!np)
		return dev_err_probe(x->dev, -EINVAL, "missing ethernet\n");

	x->wan_dev = of_find_net_device_by_node(np);
	of_node_put(np);

	return x->wan_dev ? 0 : -EPROBE_DEFER;
}

static const struct airoha_xpon_ops an7552_mac_ops = {
	.rx = an7552_mac_rx,
	.ppe_upstream = an7552_mac_ppe_upstream,
};

/*
 * Default GPON serial number from the "gpon-serial-number" nvmem cell: 16 ASCII
 * hex digits (vendor id + vendor specific part, the stock flash keeps
 * "54504C47159A77B8").  gpond overrides it with gpon.sn from /etc/config/gpon.
 * There is no password source in the flash and none is set by default.
 */
static int an7552_mac_read_sn(struct an7552_mac *x)
{
	struct nvmem_cell *cell;
	u8 sn[GPON_SN_LEN];
	size_t len;
	char *buf;
	int ret = 0;

	cell = devm_nvmem_cell_get(x->dev, "gpon-serial-number");
	if (IS_ERR(cell)) {
		ret = PTR_ERR(cell);
		return ret == -ENOENT ? 0 : ret;
	}

	buf = nvmem_cell_read(cell, &len);
	if (IS_ERR(buf))
		return PTR_ERR(buf);

	if (len == 2 * GPON_SN_LEN && !hex2bin(sn, buf, GPON_SN_LEN))
		memcpy(x->sn, sn, GPON_SN_LEN);
	else
		dev_warn(x->dev, "gpon-serial-number is not %d hex digits\n",
			 2 * GPON_SN_LEN);
	kfree(buf);
	return ret;
}

static int an7552_gpon_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct airoha_xpon_ops ops = an7552_mac_ops;
	struct an7552_phy *phy;
	struct net_device *ndev;
	struct an7552_mac *x;
	int ret;

	x = devm_kzalloc(dev, sizeof(*x), GFP_KERNEL);
	if (!x)
		return -ENOMEM;
	phy = devm_kzalloc(dev, sizeof(*phy), GFP_KERNEL);
	if (!phy)
		return -ENOMEM;
	x->dev = dev;
	x->phy = phy;
	/* the PHY half notifies the MAC directly, see an7552_phy_irq.c */
	phy->dev = dev;
	phy->mac = x;
	mutex_init(&x->lock);
	mutex_init(&x->mode_lock);
	mutex_init(&x->state_lock);
	spin_lock_init(&x->reg_lock);
	INIT_WORK(&x->phy_work, an7552_mac_phy_work);

	ret = an7552_mac_read_sn(x);
	if (ret)
		return dev_err_probe(dev, ret, "gpon-serial-number\n");

	x->gpon = devm_platform_ioremap_resource_byname(pdev, "gpon");
	if (IS_ERR(x->gpon))
		return PTR_ERR(x->gpon);

	x->scu = syscon_regmap_lookup_by_phandle(dev->of_node, "airoha,scu");
	if (IS_ERR(x->scu))
		return dev_err_probe(dev, PTR_ERR(x->scu), "airoha,scu\n");

	x->mac_rst = devm_reset_control_get_exclusive(dev, "mac");
	if (IS_ERR(x->mac_rst))
		return dev_err_probe(dev, PTR_ERR(x->mac_rst), "mac reset\n");

	/* the SerDes half: windows, SCUs, phy reset line, phy interrupt */
	ret = an7552_phy_probe(pdev, phy);
	if (ret)
		return dev_err_probe(dev, ret, "PHY\n");

	x->mode = an7552_phy_get_mode(phy);
	/* GPON only: the PHY refuses every other airoha,pon-mode */
	if (x->mode != AN7552_XPON_GPON && x->mode != AN7552_XPON_GPON_SYM)
		return dev_err_probe(dev, -EOPNOTSUPP,
				     "PON mode %d is not GPON\n", x->mode);

	ret = an7552_mac_get_wan(x);
	if (ret)
		goto phy_remove;

	x->irq = platform_get_irq_byname(pdev, "mac");
	if (x->irq < 0) {
		ret = x->irq;
		goto put_wan;
	}
	x->dg_irq = platform_get_irq_byname(pdev, "dying-gasp");
	if (x->dg_irq < 0) {
		ret = x->dg_irq;
		goto put_wan;
	}

	x->wq = alloc_ordered_workqueue("%s", 0, dev_name(dev));
	if (!x->wq) {
		ret = -ENOMEM;
		goto put_wan;
	}

	ret = an7552_mac_init(x);
	if (ret)
		goto destroy_wq;

	/* queue 0 plus one per T-CONT channel for HTB offload leaves */
	ndev = alloc_etherdev_mqs(sizeof(struct an7552_mac *),
				  1 + GPON_MAX_TCONT, 1);
	if (!ndev) {
		ret = -ENOMEM;
		goto destroy_wq;
	}
	SET_NETDEV_DEV(ndev, dev);
	strscpy(ndev->name, "pon%d", sizeof(ndev->name));
	ndev->netdev_ops = &an7552_mac_netdev_ops;
	ndev->max_mtu = GPON_MAX_MTU;
	/*
	 * airoha_xpon_tx() takes fragments and sets the QDMA checksum and
	 * TSO bits; VLAN devices on pon0 (pon0.35) get them too, the tag
	 * stays in the frame data.  Downstream frames get the GDM's L3/L4
	 * checksum result (NETIF_F_RXCSUM).
	 */
	ndev->hw_features |= NETIF_F_HW_TC | NETIF_F_RXCSUM | AN7552_TX_FEATURES;
	ndev->features |= NETIF_F_HW_TC | NETIF_F_RXCSUM | AN7552_TX_FEATURES;
	ndev->vlan_features |= AN7552_TX_FEATURES;
	netif_set_real_num_tx_queues(ndev, 1);
	*(struct an7552_mac **)netdev_priv(ndev) = x;
	x->ndev = ndev;
	x->ppe_upstream = true;
	if (of_get_ethdev_address(dev->of_node, ndev))
		eth_hw_addr_random(ndev);

	ops.priv = x;
	/* data frames are charged to pon0's TX queues (BQL) */
	ops.tx_dev = ndev;
	ops.ppe_downstream = true;
	ret = airoha_xpon_register(x->wan_dev, &ops);
	if (ret)
		goto free_ndev;

	/* xpon_isr() -> ECNT_API_XPON_INT_NOTIFY: one MAC interrupt line */
	ret = devm_request_threaded_irq(dev, x->irq, NULL,
					an7552_mac_irq_thread, IRQF_ONESHOT,
					dev_name(dev), x);
	if (ret)
		goto unreg_client;
	ret = devm_request_irq(dev, x->dg_irq, an7552_mac_dg_irq, 0,
			       "xpon-dying-gasp", x);
	if (ret)
		goto free_mac_irq;

	ret = an7552_mac_genl_init(x);
	if (ret)
		goto free_dg_irq;

	ret = an7552_led_init(x);
	if (ret)
		goto genl_exit;

	ndev->sysfs_groups[0] = &an7552_gpon_group;
	ret = register_netdev(ndev);
	if (ret)
		goto led_exit;

	an7552_mac_debugfs_init(x);
	platform_set_drvdata(pdev, x);

	/*
	 * Announce the core to the optical front end registry, last, once every
	 * step an7552_laser_attached() depends on is in place: the mode it
	 * reports, the netdev it finds not NULL, the WAN client it restarts if
	 * this MAC had been taken down.  A front end that registered before this
	 * probe is picked up by this very call.
	 *
	 * This core probes and comes up either way.  There is no optical path yet
	 * until a front end shows up, so nothing here may wait for one - pon0
	 * existing is what lets userspace see that there is no front end at all,
	 * and ndo_open() refuses to start the MAC while the DOWN bit says there
	 * is nowhere for its bursts to go.
	 */
	an7552_laser_core_ready(x);

	dev_info(dev, "%s mode, WAN %s\n",
		 x->mode == AN7552_XPON_GPON_SYM ? "GPON-SYM" : "GPON",
		 netdev_name(x->wan_dev));

	return 0;

led_exit:
	an7552_led_exit(x);
genl_exit:
	an7552_mac_genl_exit();
	/*
	 * The interrupts go before the workqueue and the PHY, as in
	 * an7552_mac_shutdown(): devres would free them only after this
	 * function returns, and one arriving in between queues work onto a
	 * destroyed workqueue.
	 */
free_dg_irq:
	devm_free_irq(dev, x->dg_irq, x);
free_mac_irq:
	devm_free_irq(dev, x->irq, x);
unreg_client:
	airoha_xpon_unregister(x->wan_dev);
free_ndev:
	/*
	 * Nothing has to be taken back out of the optical front end registry
	 * here: an7552_laser_core_ready() is called further down, past every one
	 * of these labels, so on the way out of a failed probe this MAC was never
	 * announced and no xpon_alarm() can be aimed at it.
	 */
	x->ndev = NULL;
	free_netdev(ndev);
destroy_wq:
	destroy_workqueue(x->wq);
put_wan:
	dev_put(x->wan_dev);
phy_remove:
	an7552_phy_remove(phy);
	return ret;
}

/*
 * Take the MAC out of service - and nothing more, so that it can be put back.
 *
 * This is what the optical front end going away does: there is no optical path
 * and nothing this MAC could be asked to do that would work, so it stops rather
 * than sitting there taking commands.  pon0 leaves userspace's view but stays
 * registered and allocated, which is the only form of "gone" that can be
 * undone - see the detach/attach pair below.  An7552_mac_open() refuses while
 * the DOWN bit is set as well, so bringing the link up by hand does not start a
 * MAC with no optical path behind it.
 *
 * Only the steps that have a clean inverse are here.  The window maps, the
 * reset control and the interrupts are devm, and the workqueue cannot go while
 * an interrupt can still queue onto it - so none of that is touched, and
 * an7552_mac_up() puts back exactly this list and nothing else.
 *
 * Order matters four times over:
 *
 *  - the dying gasp goes first, while the link is still in O5 and the PLOAM
 *    channel is there to carry it; an7552_mac_dying_gasp() sends nothing in any
 *    other state, and after an7552_mac_stop() below the ONU is back in O1.
 *  - airoha_xpon_unregister() drains the NAPI context of the WAN GDM and
 *    stops the QDMA, so it has to run before the interface is quiesced:
 *    an7552_mac_rx() must not be left reaching a netdev that is going down.
 *  - the PHY is stopped by an7552_mac_stop(), and the next gpon_dev_reset()
 *    starts it again, so it is not stopped twice here.
 *
 * [lock] x->state_lock, claimed with test_and_set_bit(AN7552_MAC_DOWN) so that
 * a second caller cannot start a teardown that is already running.  x->ndev is
 * NULL once an7552_mac_shutdown() has handed it back, which is what stops a
 * caller arriving after that.
 */
static void an7552_mac_teardown(struct an7552_mac *x)
{
	an7552_mac_dying_gasp(x);

	/* the interface stops before it goes, so userspace is not left sending */
	netif_stop_queue(x->ndev);
	an7552_mac_stop(x);
	netif_carrier_off(x->ndev);

	/*
	 * What "leaves the kernel" now means.  netif_device_detach() clears
	 * __LINK_STATE_PRESENT, so netif_device_present() is false, ip link on it
	 * answers ENODEV and nothing can bring it up - while the allocation and
	 * the registration stay exactly where they are.  unregister_netdev() is
	 * what cannot be undone: it leaves reg_state at NETREG_UNREGISTERED and
	 * register_netdev() on the same struct is a BUG_ON
	 * (net/core/dev.c, register_netdevice()).  detach/attach is the pair
	 * meant for this, and it is what DSA uses for a port that disappears and
	 * comes back.
	 *
	 * An7552_mac_open() refuses while the DOWN bit is set as well, so the
	 * guard does not rest on the detach alone.
	 */
	rtnl_lock();
	netif_device_detach(x->ndev);
	rtnl_unlock();

	an7552_led_exit(x);
	an7552_mac_genl_exit();
	airoha_xpon_unregister(x->wan_dev);
}

void an7552_mac_down(struct an7552_mac *x)
{
	if (!READ_ONCE(x->ndev))
		return;			/* shutdown already took it */

	guard(mutex)(&x->state_lock);
	if (test_and_set_bit(AN7552_MAC_DOWN, &x->flags))
		return;			/* already down, or one is running */

	an7552_mac_teardown(x);
}

/*
 * Put it back, the exact inverse of an7552_mac_teardown() above and in the same
 * order - the WAN client has to be registered before the netdev it charges
 * frames to, and userspace cannot drive pon0 before it is enabled again.  The
 * traffic classifier is not here: its rules are the software table
 * x->us_rule, which the teardown does not touch, and ndo_setup_tc replaces it
 * when tc is configured again.
 *
 * The MAC is left as it was before it went down: not started, state in O1, the
 * interface down.  Bringing it up is still userspace's to do, through the same
 * ifup that would have done it the first time - nothing here guesses at a mode
 * or starts a burst, because the optical front end that just registered is the
 * side that knows whether it can be started at all.
 *
 * [lock] x->state_lock, same bit as an7552_mac_down() and taken the other way
 * round: the DOWN bit set means a teardown has to be undone, and it is cleared
 * only once everything is back, so a concurrent an7552_mac_down() cannot pull
 * the MAC out from under this.
 */
int an7552_mac_up(struct an7552_mac *x)
{
	struct airoha_xpon_ops ops = an7552_mac_ops;
	int ret;

	if (!READ_ONCE(x->ndev))
		return -ENODEV;

	guard(mutex)(&x->state_lock);
	if (!test_and_set_bit(AN7552_MAC_DOWN, &x->flags))
		return 0;		/* already up */

	ops.priv = x;
	/* data frames are charged to pon0's TX queues (BQL) */
	ops.tx_dev = x->ndev;
	ops.ppe_downstream = true;

	ret = airoha_xpon_register(x->wan_dev, &ops);
	if (ret)
		goto err_down;

	ret = an7552_mac_genl_init(x);
	if (ret)
		goto unreg_client;

	ret = an7552_led_init(x);
	if (ret)
		goto genl_exit;

	/*
	 * The inverse of the detach in an7552_mac_teardown(): the interface is
	 * present again.  It is deliberately not brought up - netif_device_attach()
	 * only wakes the queues of a device that is already IFF_UP - so starting
	 * the MAC stays userspace's to do, as it was when the netdev was first
	 * registered.
	 */
	rtnl_lock();
	netif_device_attach(x->ndev);
	rtnl_unlock();

	clear_bit(AN7552_MAC_DOWN, &x->flags);

	return 0;

genl_exit:
	an7552_mac_genl_exit();
unreg_client:
	airoha_xpon_unregister(x->wan_dev);
err_down:
	return ret;
}


/*
 * Give the MAC up for good, as the platform device going away does.
 *
 * Whatever it is doing now comes down first, so that a front end that has
 * already unregistered does not leave work behind.  Then the rest, which is the
 * part with no inverse: the interrupts, the workqueue and the netdev
 * allocation all go, in that order - an interrupt can still queue onto the
 * workqueue, so it cannot outlive it.
 *
 * The optical front end registry is let go first and not at all if the front
 * end is already gone: xpon_alarm() and xpon_burst_ctrl() reach this MAC
 * through a pointer the registry keeps, and one that outlives the platform
 * device would be a use-after-free the moment the next DDMI tick fired.
 */
static void an7552_mac_shutdown(struct an7552_mac *x)
{
	struct net_device *ndev = READ_ONCE(x->ndev);
	struct net_device *wan;

	/* a half-finished probe has neither netdev nor a registry entry */
	if (!ndev)
		return;

	an7552_laser_core_gone(x);

	guard(mutex)(&x->state_lock);
	if (!test_and_set_bit(AN7552_MAC_DOWN, &x->flags))
		an7552_mac_teardown(x);

	/*
	 * The transmitter reference goes with the core, after the teardown:
	 * its dying gasp still needs the transmitter.  It is a reference on
	 * the front end's module, and holding it past this point is what keeps
	 * en7571 loaded after its own platform device has gone; the front end
	 * takes it again when a core appears.  The put switches the line off
	 * first, so nothing is left transmitting once the driver is gone.
	 */
	an7552_tx_reg_put(x->phy);

	/*
	 * The gate closes for good here and nowhere else.  It stays as it is
	 * across an7552_mac_down()/an7552_mac_up() on purpose - a front end
	 * that goes away and comes back should re-announce without being asked
	 * a second time - but this is a new device, so the next one starts
	 * shut and waits for whoever is going to open it.
	 *
	 * The debugfs directory goes with it: it is created once in the probe
	 * and lives as long as the platform device, so its control files are
	 * still there to be read and written while no front end is attached.
	 * That is why an7552_mac_debugfs_init() is not in an7552_mac_up()
	 * and an7552_mac_debugfs_exit() is not in an7552_mac_teardown() either.
	 */
	x->armed = false;
	an7552_mac_debugfs_exit(x);

	/* nothing left that queues phy_work */
	an7552_phy_remove(x->phy);
	cancel_work_sync(&x->phy_work);
	/* before the workqueue goes: an IRQ can still queue onto it */
	devm_free_irq(x->dev, x->irq, x);
	devm_free_irq(x->dev, x->dg_irq, x);
	destroy_workqueue(x->wq);

	/*
	 * Now that nothing can queue onto the workqueue and the interrupts are
	 * gone, the interface can at last leave the kernel - this is the one
	 * place unregister_netdev() belongs, because it is the one place the
	 * allocation is not needed again.  x->ndev goes to NULL with it, which is
	 * what an7552_mac_carrier(), an7552_mac_omci_tx() and an7552_mac_down()
	 * above test against.
	 */
	unregister_netdev(ndev);
	free_netdev(ndev);
	x->ndev = NULL;

	wan = x->wan_dev;
	x->wan_dev = NULL;
	dev_put(wan);

	/* and this is now the end of it: a front end may not register again */
	set_bit(AN7552_MAC_GONE, &x->flags);
}

/*
 * The front end is what the MAC depends on, so its going away ends the MAC -
 * see an7552_mac_down(), which is the whole of what is left to do here.
 */
static void an7552_gpon_remove(struct platform_device *pdev)
{
	an7552_mac_shutdown(platform_get_drvdata(pdev));
}

static const struct of_device_id an7552_gpon_of_match[] = {
	{ .compatible = "airoha,an7552-gpon" },
	{ }
};
MODULE_DEVICE_TABLE(of, an7552_gpon_of_match);

static struct platform_driver an7552_gpon_driver = {
	.probe	= an7552_gpon_probe,
	.remove	= an7552_gpon_remove,
	.driver	= {
		.name		= AN7552_DRV_NAME,
		.of_match_table	= an7552_gpon_of_match,
	},
};

/* module parameter "debug": what to print to the kernel log */
unsigned int an7552_phy_debug;
module_param_named(debug, an7552_phy_debug, uint, 0644);
MODULE_PARM_DESC(debug,
		 "kernel log: bit 0 PHY events and recover work, "
		 "bit 1 PHY register dump at every event, "
		 "bit 2 MAC activation (O1..O7 states, PLOAM both ways)");

module_platform_driver(an7552_gpon_driver);

MODULE_DESCRIPTION("Airoha AN7552/AN7563 xPON MAC and SerDes PHY");
MODULE_LICENSE("GPL");
