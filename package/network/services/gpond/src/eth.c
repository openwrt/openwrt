// SPDX-License-Identifier: GPL-2.0
/*
 * Ethernet side of OMCI: MAC bridges, VLANs, L3 hosts, multicast, policing.
 *
 * The OMCI MIB is turned into a model of Linux devices that only covers
 * pon0 and what is derived from it, plus the UNI devices named in
 * onu.uni_dev:
 *   - 802.1Q/802.1ad devices on pon0 and on a UNI device (eth_vlan.c),
 *   - one bridge per MAC bridge service profile (ME 45), named "br-pon"
 *     when there is one and "br-pon-<me id>" when there are more, with the
 *     ports of its bridge port config data (ME 47); a bridge with fewer
 *     than two ports is no bridge (the port is the device the interfaces
 *     use), and a bridge whose only UNI side port is a VEIP or an IP host
 *     is not a bridge at all: the router side is the ANI-side leaf device
 *     itself,
 *
 * Nothing outside the devices gpond owns is written: pon0 and the VLAN
 * devices on it, the bridges above, and a UNI device only when onu.uni_dev
 * names one of those (a "pon*" or "br-pon*" device).  A LAN bridge of the
 * admin is never taken over - a second "config device" section for the same
 * netdev would drop the options of the first one (netifd bridge_reload()
 * reads "ports" from the section that comes last).
 *   - netifd interfaces of the IP host config data (ME 134) and of the
 *     VEIP (ME 329), HGU mode (eth_l3.c),
 *   - tc flower filters for what VLAN devices cannot do (ME 171 rows with
 *     P-bit/ethertype filters, DSCP -> P-bit), installed by the procd
 *     service xpon-vlan from the "vrule" sections of /etc/config/gpon,
 *   - ping / traceroute response of an IP host (ME 134 IP options bits 1
 *     and 2): not written anywhere, only reported (eth_l3.c) - the
 *     firewall of the system decides whether the host answers,
 *   - multicast for IPTV (eth_mcast.c): IGMP snooping and its timers,
 *     fast leave, the multicast VLANs, the IGMP rate of the UNIs and the
 *     group limit of the ports from the multicast operations profile
 *     (ME 309) and subscriber config info (ME 310); omcproxy for the proxy
 *     function (/etc/config/omcproxy), the group limit by the procd service
 *     xpon-mcast from the "mport" sections of /etc/config/gpon,
 *   - policing of pon0 ports from the traffic descriptors (ME 280) the
 *     bridge port points to (a meter: tc police on clsact ingress/egress),
 *   - the upstream classification of the MAC (genl US_RULE_SET: VID and
 *     P-bit mask -> GEM port) from the 802.1p mapper (ME 130) or GEM
 *     interworking TP of the ANI port, per VID the ANI side sends, the
 *     P-bit -> queue map (PCP_QUEUE) and the DSCP -> P-bit map for the
 *     untagged IP frames of a mapper with the unmarked frame option 0
 *     (DSCP_MAP; the MAC has one map: the first untagged leaf's wins).
 *     The DSCP P-bit of an ME 171 treatment (priority 10) is written into
 *     the tag by tc (eth_vlan.c) and read by the MAC like any tag.
 *
 * VLAN handling (extended VLAN tagging operation, ME 171; VLAN tagging
 * filter data, ME 84) is eth_vlan.c.  The MAC sees the frame as the
 * VLAN devices and tc left it: its VID/PCP match uses the outer tag, so
 * the rules are computed from the outer VID of every ANI-side leaf, which
 * keeps them consistent with the P-bits the treatment wrote.
 *
 * PPTP Ethernet UNI (ME 11) instance 0x101 + n -> the netifd device
 * section of onu.uni_dev[n] (netifd device.c dev_attrs[]):
 *   admin state 1 (lock)      option enabled 0: netifd takes the device as
 *                             not present (device_refresh_present()), it
 *                             leaves its bridge and goes down
 *   auto detection config     option autoneg 1, always, plus option speed
 *                             and/or duplex.  The SDK switch API keeps
 *                             autonegotiation on and only restricts what is
 *                             advertised (mtkswitch_api_krl.c
 *                             MIIMediaOption[]), as netifd does with speed /
 *                             duplex and autoneg 1 (system-linux.c
 *                             system_set_ethtool_settings()); autoneg 1
 *                             without speed/duplex advertises every
 *                             supported mode again
 *   max frame size            option mtu = max frame size - 18 (Ethernet
 *                             header and FCS, 1518 <-> 1500), only when it
 *                             differs from the ONU default 1518; netifd
 *                             restores the original MTU when the option
 *                             goes (device_apply_live_settings())
 *   Ethernet loopback config  no netifd option: stored and reported only
 *   pause time                option pause (nonzero enables PAUSE frames)
 *   pppoe filter              tc ingress: PPPoE only when 1, drop the rest
 *   DTE/DCE ind,              stored and reported only
 *   bridged or IP ind
 * Alarms and AVCs of a locked UNI are not reported (omci_alarm.c).
 *
 * gpond never creates devices or qdiscs itself: the model is written to
 * UCI and the system services apply it - netifd the devices and
 * interfaces ("ubus call network reload"), the procd services xpon-qos
 * (policers), xpon-vlan (tc filters) and xpon-mcast (port group limits),
 * from the "police", "vrule" and "mport" sections of /etc/config/gpon, on a
 * procd config.change event, omcproxy from its own UCI package.  What
 * happens follows main.config_mode:
 *   0 live    nothing to apply without UCI: the model is only logged
 *   1 stage   UCI sections staged (not committed), nothing applied
 *   2 commit  UCI committed, nothing applied
 *   3 apply   committed, netifd reloaded, procd triggered
 * The gpond sections of /etc/config/network carry a "comment gpond ..." and are
 * the only ones it changes or removes.  /etc/config/firewall is not touched
 * at all: it belongs to the system, not to the daemon.  The MAC upstream
 * rules are kernel state, not system configuration: they are programmed in
 * every mode.
 */
#include <errno.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uci.h>

#include "gpond.h"
#include "eth.h"

#ifndef ETH_DEBOUNCE_MS
#define ETH_DEBOUNCE_MS		2000
#endif

/* PPTP Ethernet UNI attributes (SDK omciAttriDescriptListPPTPEthernetUNI) */
#define PPTP_AUTO_DETECT	3
#define PPTP_LOOPBACK		4
#define PPTP_ADMIN		5
#define PPTP_MAX_FRAME		8
#define PPTP_PAUSE		10
#define PPTP_PPPOE_FILTER	14
#define PPTP_ADMIN_LOCK		1
#define PPTP_DEFAULT_FRAME	1518	/* omci_mib.c init_uni() */
#define ETH_FRAME_OVERHEAD	18	/* header 14 + FCS 4 */
#define NETIFD_MIN_MTU		68	/* netifd device_init_settings() */

static struct eth_model cur;		/* last model put in place */
static struct eth_model next;
static struct eth_us_rule us_applied[ETH_MAX_US_RULES];
static struct eth_vid_map vmap_applied[ETH_MAX_VID_MAP];
static bool pcp_queue_done;
static uint8_t pcp_queue_applied[8];
static bool dscp_applied;
static uint8_t dscp_applied_map[ETH_DSCP_NUM];
static struct uloop_timeout sync_tmo;

/* ---------------------------------------------------------------------
 * model
 * ------------------------------------------------------------------- */

const struct eth_model *eth_current(void)
{
	return &cur;
}

/* the netdev of the bridge an ME 45 instance made, NULL when it has none */
const char *eth_bridge_name(uint16_t id)
{
	int i;

	for (i = 0; i < cur.nbr; i++)
		if (cur.br[i].id == id)
			return cur.br[i].name;
	for (i = 0; i < next.nbr; i++)
		if (next.br[i].id == id)
			return next.br[i].name;

	return NULL;
}

/*
 * The devices gpond may write: the ANI netdev and the VLAN devices on it,
 * the bridges it creates itself, and the UNI devices named in onu.uni_dev
 * (and their VLAN devices).  Everything else is the admin's and is never
 * named in a section gpond writes.
 */
static bool dev_is_uni(const char *dev)
{
	int i;

	if (!dev || !dev[0] || !strcmp(dev, "-"))
		return false;
	for (i = 0; i < cfg.n_uni_dev && i < GPON_MAX_UNI; i++) {
		size_t l = strlen(cfg.uni_dev[i]);

		if (!l || !strcmp(cfg.uni_dev[i], "-"))
			continue;
		if (!strcmp(dev, cfg.uni_dev[i]) ||
		    (!strncmp(dev, cfg.uni_dev[i], l) && dev[l] == '.'))
			return true;
	}

	return false;
}

static bool dev_ours(const char *dev)
{
	return dev && (!strncmp(dev, "pon", 3) || !strncmp(dev, "br-pon", 6) ||
		       dev_is_uni(dev));
}

/*
 * The VLAN device of @vid on @parent, created on first use.  Names:
 * "<parent>.<vid>" (802.1Q), "<parent>.s<vid>" (802.1ad).  A device used
 * twice has to be used the same way (P-bit maps); NULL on a conflict, when
 * the model is full or the name is too long.
 */
const char *eth_vlan_dev(struct eth_model *m, const char *parent,
			 uint16_t tpid, uint16_t vid, int egress, int ingress)
{
	char name[IFNAMSIZ + 24];
	struct eth_vlan *v;
	int i;

	if (!parent || vid == 0 || vid >= VID_ANY)
		return NULL;
	snprintf(name, sizeof(name), "%s.%s%u", parent,
		 tpid == TPID_8021AD ? "s" : "", vid);
	if (strlen(name) >= IFNAMSIZ)
		return NULL;

	for (i = 0; i < m->nvlan; i++) {
		v = &m->vlan[i];
		if (strcmp(v->name, name))
			continue;
		if ((egress != EGR_DONTCARE && v->egress != egress) ||
		    (ingress != ING_DONTCARE && v->ingress != ingress))
			return NULL;
		return v->name;
	}
	if (m->nvlan >= ETH_MAX_VLANS)
		return NULL;

	v = &m->vlan[m->nvlan++];
	memcpy(v->name, name, strlen(name) + 1);	/* < IFNAMSIZ */
	snprintf(v->parent, sizeof(v->parent), "%s", parent);
	v->vid = vid;
	v->tpid = tpid;
	v->egress = egress == EGR_DONTCARE ? EGR_NONE : egress;
	v->ingress = ingress == ING_DONTCARE ? ING_NONE : ingress;

	return v->name;
}

struct eth_port *eth_add_port(struct eth_bridge *b, const char *dev)
{
	int i;

	if (!dev_ours(dev) || !strcmp(dev, b->name))
		return NULL;
	for (i = 0; i < b->nport; i++)
		if (!strcmp(b->port[i].dev, dev))
			return &b->port[i];
	if (b->nport >= ETH_MAX_PORTS)
		return NULL;
	snprintf(b->port[b->nport].dev, IFNAMSIZ, "%s", dev);

	return &b->port[b->nport++];
}

void eth_bridge_ani(struct eth_bridge *b, const char *dev, uint16_t vid)
{
	int i;

	for (i = 0; i < b->nani; i++)
		if (!strcmp(b->ani[i].dev, dev))
			return;
	if (b->nani >= ETH_MAX_ANI)
		return;
	snprintf(b->ani[b->nani].dev, IFNAMSIZ, "%s", dev);
	b->ani[b->nani++].vid = vid;
}

/* a tc filter in the order it is added: the precedence of the row */
int eth_tc_add(struct eth_model *m, const char *dev, bool egress,
	       const char *proto, const char *match, const char *action)
{
	struct eth_tc *t;
	size_t ml = strlen(match);
	int i, n = 0;

	while (ml && match[ml - 1] == ' ')
		ml--;
	for (i = 0; i < m->ntc; i++) {
		t = &m->tc[i];
		if (strcmp(t->dev, dev) || t->egress != egress)
			continue;
		/* a second copy would never match: the first one takes it */
		if (!strcmp(t->proto, proto) && strlen(t->match) == ml &&
		    !strncmp(t->match, match, ml) && !strcmp(t->action, action))
			return 0;
		n++;
	}
	if (m->ntc >= ETH_MAX_TC || ETH_TC_PREF_BASE + n > 60000 ||
	    strlen(proto) >= sizeof(t->proto) ||
	    strlen(match) >= sizeof(t->match) ||
	    strlen(action) >= sizeof(t->action))
		return -ENOSPC;

	t = &m->tc[m->ntc++];
	snprintf(t->dev, sizeof(t->dev), "%s", dev);
	t->egress = egress;
	t->pref = ETH_TC_PREF_BASE + n;
	snprintf(t->proto, sizeof(t->proto), "%s", proto);
	snprintf(t->match, sizeof(t->match), "%s", match);
	snprintf(t->action, sizeof(t->action), "%s", action);
	for (i = strlen(t->match); i && t->match[i - 1] == ' '; i--)
		t->match[i - 1] = 0;

	return 0;
}

static const char *uni_dev(uint16_t pptp_id)
{
	static char warned[IFNAMSIZ];
	int idx = (pptp_id & 0xff) - 1;	/* slot 1, port 1.. (omci_mib.c) */
	const char *dev;

	if (idx < 0 || idx >= cfg.n_uni_dev || !cfg.uni_dev[idx][0] ||
	    !strcmp(cfg.uni_dev[idx], "-"))
		return NULL;
	dev = cfg.uni_dev[idx];
	/* a LAN bridge of the admin is not ours to enslave or to describe in a
	 * second device section */
	if (!strcmp(dev, "br-lan") || !strcmp(dev, "lo") ||
	    !strcmp(dev, GPON_NETDEV)) {
		if (strcmp(warned, dev)) {
			snprintf(warned, sizeof(warned), "%s", dev);
			WARN("eth: onu.uni_dev %s cannot be used as a UNI "
			     "port, the UNI stays unconnected", warned);
		}
		return NULL;
	}

	return dev;
}

/* the Linux device behind a PPTP Ethernet UNI (omci_pm.c counters) */
const char *eth_uni_dev(uint16_t pptp_id)
{
	return uni_dev(pptp_id);
}

/* the ME 171 / ME 78 instance associated with a bridge port / UNI / L3 ME */
static struct omci_inst *find_ext_vlan(uint16_t port_id, uint8_t tp_type,
				       uint16_t tp_ptr)
{
	struct omci_inst *in;
	int pass;

	/* ME 171 wins when a port has both; ME 78 is the fallback */
	for (pass = 0; pass < 2; pass++) {
		list_for_each_entry(in, &omci_mib, list) {
			uint8_t at;
			uint16_t ptr;

			if (pass == 0) {
				if (in->def->class_id != CLASS_EXT_VLAN)
					continue;
				at = mib_get_uint(in, 1);
				ptr = mib_get_uint(in, 7);
				if ((at == ASSOC_BRIDGE_PORT && ptr == port_id) ||
				    (at == ASSOC_PPTP_ETH_UNI &&
				     tp_type == TP_PPTP_ETH_UNI && ptr == tp_ptr) ||
				    (at == ASSOC_IP_HOST &&
				     tp_type == TP_IP_HOST && ptr == tp_ptr) ||
				    (at == ASSOC_VEIP && tp_type == TP_VEIP &&
				     ptr == tp_ptr))
					return in;
				continue;
			}
			if (in->def->class_id != CLASS_VLAN_TAG_OP)
				continue;
			at = mib_get_uint(in, 4);
			ptr = mib_get_uint(in, 5);
			/*
			 * G.988 9.3.12: 0 PPTP Ethernet UNI with the ME id
			 * (or an IP host with the same id), 1 IP host,
			 * 3 MAC bridge port, 10 PPTP Ethernet UNI with the
			 * pointer, 11 VEIP.  The SDK's getVlanRulePortId-
			 * ByInst() takes a LAN-side bridge port only, so 3
			 * is the bridge port of a UNI (tp_ptr is the ME 47
			 * id there, not the TP).
			 */
			switch (at) {
			case VT_ASSOC_UNI_IMPLICIT:
				if (in->id == tp_ptr &&
				    (tp_type == TP_PPTP_ETH_UNI ||
				     tp_type == TP_IP_HOST))
					return in;
				break;
			case VT_ASSOC_IP_HOST:
				if (tp_type == TP_IP_HOST && ptr == tp_ptr)
					return in;
				break;
			case VT_ASSOC_BRIDGE_PORT:
				if (tp_type == TP_PPTP_ETH_UNI && ptr == port_id)
					return in;
				break;
			case VT_ASSOC_UNI:
				if (tp_type == TP_PPTP_ETH_UNI && ptr == tp_ptr)
					return in;
				break;
			case VT_ASSOC_VEIP:
				if (tp_type == TP_VEIP && ptr == tp_ptr)
					return in;
				break;
			}
		}
	}

	return NULL;
}

/* an ME 78 whose association is the bridge port (type 3) */
static bool me78_bridge(uint16_t port_id)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == CLASS_VLAN_TAG_OP &&
		    mib_get_uint(in, 4) == VT_ASSOC_BRIDGE_PORT &&
		    mib_get_uint(in, 5) == port_id)
			return true;

	return false;
}

/* G.988 9.3.1 gives the bridge timers in 256ths of a second; the SDK
 * (setMBSPMaxAgeValue() and the like) also takes plain seconds, shifting
 * only when the high byte is set */
static uint16_t stp_time(uint32_t v)
{
	return v > 0xff ? v >> 8 : v;
}

static void police_add(struct eth_model *m, const char *dev, bool ingress,
		       uint16_t td_ptr)
{
	struct omci_inst *td;
	struct eth_police *p;
	uint32_t rate, burst;

	if (!dev || strncmp(dev, GPON_NETDEV, strlen(GPON_NETDEV)) ||
	    td_ptr == 0 || td_ptr == 0xffff || m->npol >= ETH_MAX_POLICE)
		return;
	td = mib_find(280, td_ptr);
	if (!td)
		return;
	/*
	 * The OMCI meter has two rates: what is over the CIR is yellow, what
	 * is over the PIR is red.  Only the red frames are dropped (the
	 * yellow ones when the link is congested, which the policer cannot
	 * know), so the limit is the PIR with its bucket, the CIR with its
	 * bucket where there is no PIR.  Colour mode, marking and meter type
	 * (attributes 5..8) are not applied: tc police has no colours.
	 */
	rate = mib_get_uint(td, 2);			/* PIR, bytes/s */
	burst = rate ? mib_get_uint(td, 4) : 0;		/* PBS */
	if (!rate) {
		rate = mib_get_uint(td, 1);		/* CIR */
		burst = mib_get_uint(td, 3);		/* CBS */
	} else if (!burst) {
		burst = mib_get_uint(td, 3);
	}
	if (!rate)
		return;

	p = &m->pol[m->npol++];
	snprintf(p->dev, sizeof(p->dev), "%s", dev);
	p->ingress = ingress;
	p->rate = rate;
	p->burst = burst ? burst : 1500 * 10;
}

/*
 * Auto detection configuration code points (SDK AutoDetectionCfg[]) and
 * what they advertise (mtkswitch_api_krl.c MIIMediaOption[] / the
 * macMT7530SetAutoDetection() comment): speed 0 any, duplex -1 any.
 */
static const struct eth_auto_detect auto_detect[] = {
	{ 0x00, 0, -1 },	{ 0x01, 10, 1 },	{ 0x02, 100, 1 },
	{ 0x03, 1000, 1 },	{ 0x04, 0, 1 },		{ 0x05, 2500, 1 },
	{ 0x06, 5000, 1 },	{ 0x10, 10, -1 },	{ 0x11, 10, 0 },
	{ 0x12, 100, 0 },	{ 0x13, 1000, 0 },	{ 0x14, 0, 0 },
	{ 0x20, 1000, -1 },	{ 0x30, 100, -1 },	{ 0x40, 10000, -1 },
};

#define PPPOE_FILTER_RULES	15	/* 2 + 2 * 2 + 2 * 4 + 1 */

/*
 * ME 11 attribute 14 "PPPoE filter": value 1 discards everything but PPPoE
 * (G.988 9.5.1).  tc flower has no "not PPPoE" match, so accept the PPPoE
 * frames first and drop the rest with a "protocol all" rule: a matching
 * pass action stops the classification, so the drop rule is never reached
 * by them.  The ethertype follows up to two VLAN tags, which the
 * vlan_ethtype / cvlan_ethtype keys see.  These rules stand in front of
 * the port's tc VLAN rows: the ordinary tagging is done by the VLAN
 * devices, which still see the frames, but a row that needs tc does not.
 */
static void pppoe_filter(struct eth_model *m, const char *dev)
{
	static const char *const outer[] = { "802.1Q", "802.1ad" };
	static const char *const inner[] = { "0x8100", "0x88a8" };
	int n0 = m->ntc;
	char match[64];
	unsigned int o, i;

	eth_tc_add(m, dev, false, "0x8863", "", "pass");
	eth_tc_add(m, dev, false, "0x8864", "", "pass");
	for (o = 0; o < 2; o++) {
		eth_tc_add(m, dev, false, outer[o], "vlan_ethtype 0x8863",
			   "pass");
		eth_tc_add(m, dev, false, outer[o], "vlan_ethtype 0x8864",
			   "pass");
		for (i = 0; i < 2; i++) {
			snprintf(match, sizeof(match),
				 "vlan_ethtype %s cvlan_ethtype 0x8863",
				 inner[i]);
			eth_tc_add(m, dev, false, outer[o], match, "pass");
			snprintf(match, sizeof(match),
				 "vlan_ethtype %s cvlan_ethtype 0x8864",
				 inner[i]);
			eth_tc_add(m, dev, false, outer[o], match, "pass");
		}
	}
	eth_tc_add(m, dev, false, "all", "", "drop");
	if (m->ntc - n0 != PPPOE_FILTER_RULES) {
		WARN("eth: %s: PPPoE filter: %d of %d tc rules added",
		     dev, m->ntc - n0, PPPOE_FILTER_RULES);
		m->ntc = n0;
	}
}

static void uni_settings(struct eth_model *m)
{
	struct omci_inst *in;
	struct eth_uni *u;
	uint32_t mfs;
	uint8_t ad;
	size_t i;
	int n;

	for (n = 0; n < cfg.n_uni_dev && n < GPON_MAX_UNI; n++) {
		const char *dev = uni_dev(0x100 + n + 1);

		in = mib_find(11, 0x100 + n + 1);	/* omci_mib.c init_uni() */
		if (!in || !dev)
			continue;
		u = &m->uni[m->nuni++];
		snprintf(u->dev, sizeof(u->dev), "%s", dev);
		u->me = in->id;
		u->disabled = mib_get_uint(in, PPTP_ADMIN) == PPTP_ADMIN_LOCK;
		u->duplex = -1;
		ad = mib_get_uint(in, PPTP_AUTO_DETECT);
		for (i = 0; i < ARRAY_SIZE(auto_detect); i++) {
			if (auto_detect[i].code != ad)
				continue;
			u->speed = auto_detect[i].speed;
			u->duplex = auto_detect[i].duplex;
			break;
		}
		mfs = mib_get_uint(in, PPTP_MAX_FRAME);
		if (mfs != PPTP_DEFAULT_FRAME &&
		    mfs >= NETIFD_MIN_MTU + ETH_FRAME_OVERHEAD)
			u->mtu = mfs - ETH_FRAME_OVERHEAD;
		u->loopback = mib_get_uint(in, PPTP_LOOPBACK);
		/* pause time (10): the OLT wrote it, nonzero = PAUSE frames */
		if (in->olt_mask & omci_mask(PPTP_PAUSE)) {
			u->pause_set = true;
			u->pause = mib_get_uint(in, PPTP_PAUSE) != 0;
		}
		u->pppoe_filter = mib_get_uint(in, PPTP_PPPOE_FILTER) != 0;
		if (u->pppoe_filter)
			pppoe_filter(m, u->dev);
	}
}

/* ---- ANI side ports of a bridge ---- */

static void ani_port_fill(struct ani_port *ap, struct omci_inst *bp,
			  uint8_t type)
{
	uint16_t ptr = mib_get_uint(bp, 4);
	struct omci_inst *mp;
	int p;

	memset(ap, 0, sizeof(*ap));
	ap->id = bp->id;
	ap->td_out = mib_get_uint(bp, 11);
	ap->td_in = mib_get_uint(bp, 12);
	for (p = 0; p < 8; p++)
		ap->gem[p] = -1;
	xv_filter_get(mib_find(CLASS_VLAN_FILTER, bp->id), &ap->flt);

	switch (type) {
	case TP_8021P_MAPPER:
		mp = mib_find(CLASS_MAPPER, ptr);
		if (!mp)
			break;
		for (p = 0; p < 8; p++) {
			uint16_t t = mib_get_uint(mp, 2 + p);

			ap->gem[p] = t == PTR_NULL ? -1 : bind_iwtp_gem(t);
		}
		ap->unmarked = mib_get_uint(mp, 10);
		ap->def_pbit = mib_get_uint(mp, 12) & 7;
		/* unmarked frame option 0: the P-bit of an untagged IP frame
		 * from the DSCP map (attribute 11, 24 bytes), the MAC reads
		 * it (SDK setdot1pUnmarkOptionValue(), omci_me_vlan_func.c:
		 * 2830-2935; options omci_me_vlan.h:183-184); an all zero map
		 * is P-bit 0 for every frame, nothing to read */
		if (!ap->unmarked && mp->len[11] >= 24) {
			xv_dscp_decode(mp->val[11], ap->dscp_map);
			for (p = 0; p < ETH_DSCP_NUM && !ap->dscp_map[p]; p++)
				;
			ap->dscp = p < ETH_DSCP_NUM;
		}
		break;
	case TP_GEM_IWTP:
		for (p = 0; p < 8; p++)
			ap->gem[p] = bind_iwtp_gem(ptr);
		break;
	default:
		break;			/* multicast: downstream only */
	}
}

/* the VID rewrite of a router row, once per UNI VID and P-bit mask */
void eth_vid_map_add(struct eth_model *m, uint16_t uni_vid, uint8_t pcp_mask,
		     uint16_t ani_vid, uint8_t pcp)
{
	int i;

	for (i = 0; i < ETH_MAX_VID_MAP; i++) {
		struct eth_vid_map *v = &m->vmap[i];

		if (!v->valid)
			break;
		if (v->uni_vid == uni_vid && v->pcp_mask == pcp_mask)
			return;		/* the first row takes the frames */
	}
	if (i >= ETH_MAX_VID_MAP) {
		WARN("eth: more than %d router VID rewrites, the rest stays "
		     "out of the PPE", ETH_MAX_VID_MAP);
		return;
	}
	/* field by field: us_sync() compares the entries with memcmp() */
	m->vmap[i].valid = true;
	m->vmap[i].uni_vid = uni_vid;
	m->vmap[i].pcp_mask = pcp_mask;
	m->vmap[i].ani_vid = ani_vid;
	m->vmap[i].pcp = pcp;
}

/* MAC upstream rules of one bridge: every ANI leaf, per VID */
static void us_add(struct eth_model *m, uint16_t vid, uint8_t mask,
		   uint16_t gem)
{
	int i;

	for (i = 0; i < ETH_MAX_US_RULES; i++) {
		struct eth_us_rule *r = &m->us[i];

		if (!r->valid)
			break;
		if (r->vid == vid && r->pcp_mask == mask) {
			if (r->gem != gem)
				WARN("eth: US rule VID %u mask 0x%02x: GEM %u and "
				     "%u", vid, mask, r->gem, gem);
			return;
		}
	}
	if (i >= ETH_MAX_US_RULES) {
		WARN("eth: more than %d upstream rules", ETH_MAX_US_RULES);
		return;
	}
	m->us[i].valid = true;
	m->us[i].vid = vid;
	m->us[i].pcp_mask = mask;
	m->us[i].gem = gem;
}

/*
 * The queue of the priority queue a GEM port points to (ME 268 attribute 4
 * "traffic management pointer for upstream", ME 277 related port: its low 16
 * bits are the priority, omci_mib.c init_ani()); -1: it names none.
 */
static int gem_queue(uint16_t gem)
{
	struct omci_inst *ctp, *pq;

	list_for_each_entry(ctp, &omci_mib, list) {
		if (ctp->def->class_id != 268 || mib_get_uint(ctp, 1) != gem ||
		    mib_get_uint(ctp, 3) == GEM_DIR_ANI_TO_UNI)
			continue;
		pq = mib_find(277, mib_get_uint(ctp, 4));

		return pq ? 7 - (int)(mib_get_uint(pq, 6) & 7) : -1;
	}

	return -1;
}

static bool ani_has_us(const struct ani_port *ap)
{
	int p;

	for (p = 0; p < 8; p++)
		if (ap->gem[p] >= 0)
			return true;

	return false;
}

/* the MAC has one P-bit -> queue map: the first leaf to name a queue wins */
static void queue_use(struct eth_model *m, int pbit, int q)
{
	if (m->pcp_queue_set & (1 << pbit)) {
		if (m->pcp_queue[pbit] != q)
			INFO("eth: P-bit %d: queues %u and %d, the MAC has one "
			     "map: the first is kept", pbit, m->pcp_queue[pbit], q);
		return;
	}
	m->pcp_queue[pbit] = q;
	m->pcp_queue_set |= 1 << pbit;
}

/* the MAC has one DSCP map: the first untagged leaf's one wins */
static void dscp_use(struct eth_model *m, const struct ani_port *ap)
{
	if (!m->dscp_on) {
		m->dscp_on = true;
		memcpy(m->dscp_map, ap->dscp_map, sizeof(m->dscp_map));
	} else if (memcmp(m->dscp_map, ap->dscp_map, sizeof(m->dscp_map))) {
		INFO("eth: ME130 0x%04x: a different DSCP map for untagged "
		     "frames, the MAC has one: not used", ap->id);
	}
}

/* the rules of one outer VID (0: untagged): the first ANI port that passes it */
static void us_vid(struct eth_model *m, uint16_t vid,
		   const struct ani_port *ap, int nap)
{
	const struct ani_port *own = NULL;
	int gem[8], done[8] = { 0 };
	int k, p;

	for (k = 0; k < nap && !own; k++)
		if (ani_has_us(&ap[k]) && xv_filter_pass(&ap[k].flt, vid))
			own = &ap[k];
	if (!own)
		return;
	memcpy(gem, own->gem, sizeof(gem));
	/* untagged frames: P-bit 0, the default P-bit option, or the
	 * P-bit of the DSCP (the MAC derives it, one map for all) */
	if (!vid && own->unmarked == 1)
		gem[0] = gem[own->def_pbit];
	if (!vid && own->dscp)
		dscp_use(m, own);
	for (p = 0; p < 8; p++) {
		int q = gem[p] < 0 ? -1 : gem_queue(gem[p]);

		if (q >= 0)
			queue_use(m, p, q);
	}

	for (p = 0; p < 8; p++) {
		uint8_t mask = 0;

		if (done[p] || gem[p] < 0)
			continue;
		for (k = p; k < 8; k++)
			if (gem[k] == gem[p]) {
				mask |= 1 << k;
				done[k] = 1;
			}
		us_add(m, vid ? vid : VID_ANY, mask == 0xff ? 0 : mask, gem[p]);
	}
}

static void us_bridge(struct eth_model *m, const struct eth_bridge *b,
		      const struct ani_port *ap, int nap)
{
	int j;

	for (j = 0; j < b->nani; j++)
		us_vid(m, b->ani[j].vid, ap, nap);
}

/*
 * A UNI without onu.uni_dev: the router's VLANs are tc rewrites on the pon0
 * egress (run_rows(), router rows) and leave no ANI device behind, while the
 * MAC classifies on the rewritten outer VID.  The rules follow the VLAN
 * tagging filter (ME 84) of each upstream ANI port instead.
 */
static void us_ani(struct eth_model *m, const struct ani_port *ap, int nap)
{
	int i, k;

	for (k = 0; k < nap; k++) {
		if (!ani_has_us(&ap[k]))
			continue;
		if (ap[k].flt.active && !ap[k].flt.pbit) {
			for (i = 0; i < ap[k].flt.n; i++)
				us_vid(m, ap[k].flt.vid[i], ap, nap);
		} else if (ap[k].flt.untagged_ok) {
			us_vid(m, 0, ap, nap);
		}
	}
}

/* rules of a specific VID first: the MAC takes the first match */
static void us_sort(struct eth_model *m)
{
	struct eth_us_rule t[ETH_MAX_US_RULES] = { 0 };
	int i, n = 0;

	for (i = 0; i < ETH_MAX_US_RULES; i++)
		if (m->us[i].valid && m->us[i].vid != VID_ANY)
			t[n++] = m->us[i];
	for (i = 0; i < ETH_MAX_US_RULES; i++)
		if (m->us[i].valid && m->us[i].vid == VID_ANY)
			t[n++] = m->us[i];
	memcpy(m->us, t, sizeof(t));
}

/* ---- L3 ports ---- */

static void l3_port(struct eth_model *m, struct eth_bridge *b,
		    struct omci_inst *bp, uint8_t type, uint16_t ptr,
		    const struct xv_filter *flt, bool has_uni,
		    struct l3seen *seen)
{
	uint16_t cls = type == TP_VEIP ? CLASS_VEIP : CLASS_IP_HOST;
	struct omci_inst *ev = find_ext_vlan(bp->id, type, ptr);
	struct xv_leaf leaf[XV_MAX_LEAF];
	int n, i;

	if (!mib_find(cls, ptr)) {
		INFO("eth: bridge port 0x%04x: no ME %u 0x%04x", bp->id, cls, ptr);
		return;
	}
	n = xv_l3_port(m, ev, flt, leaf, XV_MAX_LEAF, has_uni);
	if (seen->n < ETH_MAX_L3 * 2) {
		seen->class_id[seen->n] = cls;
		seen->me[seen->n++] = ptr;
	}
	for (i = 0; i < n; i++) {
		if (has_uni)
			eth_add_port(b, leaf[i].dev);
		eth_bridge_ani(b, leaf[i].dev, leaf[i].vid);
	}
	if (has_uni) {
		eth_l3_add(m, cls, ptr, b->name);
		return;
	}
	for (i = 0; i < n; i++)
		if (strcmp(leaf[i].dev, GPON_NETDEV))
			eth_l3_add(m, cls, ptr, leaf[i].dev);
}

static bool l3_seen(const struct l3seen *s, uint16_t cls, uint16_t me)
{
	int i;

	for (i = 0; i < s->n; i++)
		if (s->class_id[i] == cls && s->me[i] == me)
			return true;

	return false;
}

/*
 * ME 84 with a P-bit investigation (the SDK's action (h), user priority):
 * tc drops of the P-bits that do not pass, for the leaves of the ANI port
 * that owns them (the first ANI port that lets the VID through, like the
 * upstream rules): a VLAN leaf on pon0, the raw leaf on the UNI devices.
 */
static void pbit_filters(struct eth_model *m, const struct eth_bridge *b,
			 const struct ani_port *ap, int nap,
			 const char *const *unis, int nuni)
{
	int i, j, k;

	for (i = 0; i < nap; i++) {
		if (!ap[i].flt.pbit || !ani_has_us(&ap[i]))
			continue;
		for (j = 0; j < b->nani; j++) {
			const struct eth_ani *a = &b->ani[j];

			if (!a->vid) {
				for (k = 0; k < nuni; k++)
					xv_pbit_raw(m, unis[k], &ap[i].flt);
				continue;
			}
			for (k = 0; k < nap; k++)
				if (ani_has_us(&ap[k]) &&
				    xv_filter_pass(&ap[k].flt, a->vid))
					break;
			if (k == i)
				xv_pbit_leaf(m, a->dev, &ap[i].flt);
		}
	}
}

static void build_bridge(struct eth_model *m, struct omci_inst *in,
			 struct l3seen *seen, bool single)
{
	struct eth_bridge *b = &m->br[m->nbr];
	struct ani_port ap[ETH_MAX_ANI];
	struct xv_filter merged;
	bool has_uni = false, has_l3 = false, all_active, any_pbit = false;
	bool uni_nodev = false;
	struct omci_inst *bp;
	const char *unis[ETH_MAX_PORTS];
	int l3_start = m->nl3;
	int nap = 0, nuni = 0, i, j, k;

	memset(b, 0, sizeof(*b));
	b->id = in->id;
	/* ME 45 MAC bridge service profile */
	b->stp = mib_get_uint(in, 1) != 0;
	b->learning = mib_get_uint(in, 2) != 0;
	b->unknown_discard = mib_get_uint(in, 8) != 0;
	b->priority = mib_get_uint(in, 4);
	b->max_age = stp_time(mib_get_uint(in, 5));
	b->hello_time = stp_time(mib_get_uint(in, 6));
	b->forward_delay = stp_time(mib_get_uint(in, 7));
	b->ageing_time = mib_get_uint(in, 10);
	/* before any IP host / VEIP port below asks for the device its
	 * interface sits on; build() renames the single surviving bridge */
	if (single)
		snprintf(b->name, sizeof(b->name), "br-pon");
	else
		snprintf(b->name, sizeof(b->name), "br-pon-%04x", in->id);
	/* the IGMP rate filters stand in front of the VLAN rows of the UNIs */
	eth_mcast_pre(m, in);

	/* the ANI side first: its VLAN filter limits what the others add */
	list_for_each_entry(bp, &omci_mib, list) {
		uint8_t type;

		if (bp->def->class_id != 47 || mib_get_uint(bp, 1) != in->id)
			continue;
		type = mib_get_uint(bp, 3);
		switch (type) {
		case TP_8021P_MAPPER:
		case TP_GEM_IWTP:
		case TP_MCAST_GEM_IWTP:
			if (nap < ETH_MAX_ANI)
				ani_port_fill(&ap[nap++], bp, type);
			break;
		case TP_PPTP_ETH_UNI:
			if (uni_dev(mib_get_uint(bp, 4)))
				has_uni = true;
			break;
		case TP_IP_HOST:
		case TP_VEIP:
			has_l3 = true;
			break;
		}
	}
	memset(&merged, 0, sizeof(merged));
	all_active = nap > 0;
	for (i = 0; i < nap; i++) {
		if (!ap[i].flt.active) {
			all_active = false;
			continue;
		}
		merged.untagged_ok |= ap[i].flt.untagged_ok;
		/* every P-bit passes where this port has no P-bit filter */
		merged.pbit_mask |= ap[i].flt.pbit ? ap[i].flt.pbit_mask : 0xff;
		any_pbit |= ap[i].flt.pbit;
		for (j = 0; j < ap[i].flt.n && merged.n < 12; j++) {
			for (k = 0; k < merged.n; k++)
				if (merged.vid[k] == ap[i].flt.vid[j])
					break;
			if (k == merged.n)
				merged.vid[merged.n++] = ap[i].flt.vid[j];
		}
	}
	merged.active = all_active;
	merged.pbit = all_active && any_pbit;
	if (!all_active)
		merged.untagged_ok = true;

	list_for_each_entry(bp, &omci_mib, list) {
		uint8_t type;
		uint16_t ptr;
		const char *uni = NULL;
		struct omci_inst *ev;

		if (bp->def->class_id != 47 || mib_get_uint(bp, 1) != in->id)
			continue;
		type = mib_get_uint(bp, 3);
		ptr = mib_get_uint(bp, 4);

		switch (type) {
		case TP_PPTP_ETH_UNI:
			uni = uni_dev(ptr);
			ev = find_ext_vlan(bp->id, type, ptr);
			xv_uni_port(m, b, uni, ev, &merged);
			if (uni && nuni < ETH_MAX_PORTS)
				unis[nuni++] = uni;
			else if (!uni) {
				INFO("eth: UNI 0x%04x has no onu.uni_dev, "
				     "ANI VLANs created without a bridge", ptr);
				uni_nodev = true;
			}
			break;
		case TP_IP_HOST:
		case TP_VEIP:
			l3_port(m, b, bp, type, ptr, &merged, has_uni, seen);
			break;
		case TP_8021P_MAPPER:
		case TP_GEM_IWTP:
		case TP_MCAST_GEM_IWTP:
			if (find_ext_vlan(bp->id, type, ptr) ||
			    me78_bridge(bp->id))
				INFO("eth: ME171/78 on the ANI port 0x%04x is "
				     "not supported", bp->id);
			break;
		default:
			INFO("eth: bridge port 0x%04x TP type %u skipped",
			     bp->id, type);
			break;
		}
	}

	/* an ANI port with nothing else on the bridge: what it lets through */
	if (!b->nani && nap && !has_l3)
		xv_transparent_port(m, b, NULL, &merged);

	/* IGMP snooping, the multicast VLANs, per port limits (ME 309/310) */
	eth_mcast_bridge(m, b);

	for (i = 0; i < nap; i++)
		for (j = 0; j < b->nani; j++) {
			if (!xv_filter_pass(&ap[i].flt, b->ani[j].vid))
				continue;
			police_add(m, b->ani[j].dev, true, ap[i].td_in);
			police_add(m, b->ani[j].dev, false, ap[i].td_out);
		}
	pbit_filters(m, b, ap, nap, unis, nuni);
	us_bridge(m, b, ap, nap);
	if (uni_nodev && !has_uni)
		us_ani(m, ap, nap);

	/* a bridge is only built when it connects a physical UNI (onu.uni_dev)
	 * with at least one other port; without a physical UNI the ANI-side
	 * VLAN devices on pon0 stand on their own for the router */
	if (has_uni && b->nport >= 2) {
		m->nbr++;
		return;
	}
	/* the IP host / VEIP interface of this bridge was added on b->name:
	 * there is no such device now, it sits on the only port instead */
	for (i = l3_start; i < m->nl3; i++) {
		const char *dev = b->nport ? b->port[0].dev : GPON_NETDEV;

		if (strcmp(m->l3[i].dev, b->name))
			continue;
		INFO("eth: bridge 0x%04x has %d port(s): no bridge, %s uses "
		     "%s", b->id, b->nport, m->l3[i].name, dev);
		snprintf(m->l3[i].dev, sizeof(m->l3[i].dev), "%s", dev);
	}
}

static void build(struct eth_model *m)
{
	struct omci_inst *in, *ev;
	struct l3seen seen = { .n = 0 };

	int pb, i, n45 = 0;

	memset(m, 0, sizeof(*m));
	for (pb = 0; pb < 8; pb++)
		m->pcp_queue[pb] = 7 - pb;
	uni_settings(m);

	/* the name depends on how many bridges the OLT provisioned: one is
	 * br-pon, more carry their instance ("br-pon-0101") so that a name
	 * stays the same when another bridge appears */
	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == 45 && n45 < ETH_MAX_BRIDGES)
			n45++;

	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == 45 && m->nbr < ETH_MAX_BRIDGES)
			build_bridge(m, in, &seen, n45 == 1);

	/* profiles whose bridge had fewer than two ports are gone: what is
	 * left alone is the one and only, so it gets the plain name */
	if (m->nbr == 1 && strcmp(m->br[0].name, "br-pon")) {
		char old[IFNAMSIZ];

		snprintf(old, sizeof(old), "%s", m->br[0].name);
		INFO("eth: bridge 0x%04x is the only one, named br-pon",
		     m->br[0].id);
		snprintf(m->br[0].name, sizeof(m->br[0].name), "br-pon");
		for (i = 0; i < m->nl3; i++)
			if (!strcmp(m->l3[i].dev, old))
				snprintf(m->l3[i].dev, sizeof(m->l3[i].dev),
					 "br-pon");
	}

	/* IP host / VEIP with an extended VLAN tagging operation (ME 171) or a
	 * VLAN tag operation (ME 78) of their own and no bridge port */
	list_for_each_entry(ev, &omci_mib, list) {
		struct xv_leaf leaf[XV_MAX_LEAF];
		struct xv_filter none = { .untagged_ok = true };
		struct omci_inst *xev;
		uint8_t at, tp;
		uint16_t cls, ptr;
		int n, i;

		if (ev->def->class_id == CLASS_EXT_VLAN) {
			at = mib_get_uint(ev, 1);
			ptr = mib_get_uint(ev, 7);
			if (at != ASSOC_IP_HOST && at != ASSOC_VEIP)
				continue;
		} else if (ev->def->class_id == CLASS_VLAN_TAG_OP) {
			uint8_t vt = mib_get_uint(ev, 4);

			ptr = mib_get_uint(ev, 5);
			/*
			 * G.988 9.3.12: 1 IP host, 11 VEIP, 0 implicit
			 * (a PPTP Ethernet UNI, or an IP host with the same
			 * id).  An implicit UNI is handled with the UNI
			 * port of the bridge, not here.
			 */
			if (vt == VT_ASSOC_VEIP) {
				at = ASSOC_VEIP;
			} else if (vt == VT_ASSOC_IP_HOST ||
				   (vt == VT_ASSOC_UNI_IMPLICIT &&
				    !mib_find(11, ev->id))) {
				if (vt == VT_ASSOC_UNI_IMPLICIT)
					ptr = ev->id;
				at = ASSOC_IP_HOST;
			} else {
				continue;
			}
		} else {
			continue;
		}
		cls = at == ASSOC_VEIP ? CLASS_VEIP : CLASS_IP_HOST;
		if (l3_seen(&seen, cls, ptr) || !mib_find(cls, ptr))
			continue;
		if (seen.n < ETH_MAX_L3 * 2) {
			seen.class_id[seen.n] = cls;
			seen.me[seen.n++] = ptr;
		}
		/* ME 171 wins when both name the same port */
		tp = at == ASSOC_VEIP ? TP_VEIP : TP_IP_HOST;
		xev = find_ext_vlan(0, tp, ptr);
		if (!xev)
			continue;
		n = xv_l3_port(m, xev, &none, leaf, XV_MAX_LEAF, false);
		for (i = 0; i < n; i++)
			if (strcmp(leaf[i].dev, GPON_NETDEV))
				eth_l3_add(m, cls, ptr, leaf[i].dev);
	}
	eth_cwmp_build(m);
	us_sort(m);
}

static bool model_equal(const struct eth_model *a, const struct eth_model *b)
{
	return !memcmp(a, b, sizeof(*a));
}

/* ---------------------------------------------------------------------
 * MAC upstream classification (genl)
 * ------------------------------------------------------------------- */

/*
 * P-bit -> queue: the priority queue of upstream ME 277 with related port
 * priority p is queue 7 - p (omci_mib.c init_ani(), omci_bind.c
 * qos_tcont()).  A frame goes to the queue its GEM port points to
 * (gem_queue()), and to queue 7 - P-bit where the GEM port names none.
 */
static void us_sync(const struct eth_model *m)
{
	int i;

	if (!pcp_queue_done ||
	    memcmp(m->pcp_queue, pcp_queue_applied, sizeof(pcp_queue_applied))) {
		if (!kx_pcp_queue(m->pcp_queue)) {
			memcpy(pcp_queue_applied, m->pcp_queue,
			       sizeof(pcp_queue_applied));
			pcp_queue_done = true;
		}
	}
	/* P-bit of untagged IP frames from the DSCP */
	if (m->dscp_on != dscp_applied ||
	    (m->dscp_on && memcmp(m->dscp_map, dscp_applied_map,
				  sizeof(dscp_applied_map)))) {
		if (!kx_dscp_map(m->dscp_on ? m->dscp_map : NULL, m->dscp_on)) {
			dscp_applied = m->dscp_on;
			memcpy(dscp_applied_map, m->dscp_map,
			       sizeof(dscp_applied_map));
		}
	}
	for (i = 0; i < ETH_MAX_US_RULES; i++) {
		const struct eth_us_rule *w = &m->us[i];
		struct eth_us_rule *a = &us_applied[i];

		if (w->valid) {
			if (a->valid && !memcmp(a, w, sizeof(*a)))
				continue;
			if (kx_us_rule(i, w->vid, w->pcp_mask, w->gem)) {
				a->valid = false;
				continue;
			}
			*a = *w;
		} else if (a->valid) {
			kx_us_rule_del(i);
			a->valid = false;
		}
	}
	for (i = 0; i < ETH_MAX_VID_MAP; i++) {
		const struct eth_vid_map *w = &m->vmap[i];
		struct eth_vid_map *a = &vmap_applied[i];

		if (w->valid) {
			if (a->valid && !memcmp(a, w, sizeof(*a)))
				continue;
			if (kx_vid_map(i, w->uni_vid, w->pcp_mask, w->ani_vid,
				       w->pcp)) {
				a->valid = false;
				continue;
			}
			*a = *w;
		} else if (a->valid) {
			kx_vid_map_del(i);
			a->valid = false;
		}
	}
}

/* MIB reset: the kernel dropped everything */
void eth_reset(void)
{
	memset(us_applied, 0, sizeof(us_applied));
	memset(vmap_applied, 0, sizeof(vmap_applied));
	pcp_queue_done = false;
	dscp_applied = false;
	eth_l3_mib_reset();
}

/* ---------------------------------------------------------------------
 * UCI: /etc/config/network
 * ------------------------------------------------------------------- */

static void sect_name(char *out, size_t len, const char *prefix,
		      const char *dev)
{
	size_t i;

	snprintf(out, len, "gpon_%s_%s", prefix, dev);
	for (i = 0; out[i]; i++)
		if (out[i] == '.' || out[i] == '-')
			out[i] = '_';
}

/* an option of the gpond device section of @dev (UNI, bridge port) */
static int dev_set(struct uci_context *ctx, struct uci_package *pkg,
		   const char *dev, const char *cmt, const char *opt,
		   const char *val)
{
	struct uci_element *e;
	const char *sect = NULL;
	char sn[64];
	int ret;

	/* if the admin already has a config device for @dev (e.g. lan3 with
	 * option macaddr), update that section instead of adding a second one */
	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		const char *n;

		if (strcmp(s->type, "device") || persist_ours(ctx, s))
			continue;
		n = uci_lookup_option_string(ctx, s, "name");
		if (n && !strcmp(n, dev)) {
			sect = s->e.name;
			break;
		}
	}
	if (sect)
		return persist_set(ctx, "network", sect, opt, val);

	sect_name(sn, sizeof(sn), "p", dev);
	ret = persist_set(ctx, "network", sn, NULL, "device");
	ret = ret ? ret : persist_set(ctx, "network", sn, "name", dev);
	/* what the section is for: the ME of a UNI, the port of a bridge */
	if (cmt)
		ret = ret ? ret : persist_set(ctx, "network", sn, "comment", cmt);
	else
		ret = ret ? ret : persist_comment(ctx, "network", sn,
						  "gpond port %s", dev);

	return ret ? ret : persist_set(ctx, "network", sn, opt, val);
}

static int uci_write_uni(struct uci_context *ctx, struct uci_package *pkg,
			 const struct eth_uni *u)
{
	char val[16], cmt[32];
	int ret;

	snprintf(cmt, sizeof(cmt), "gpond me 11 0x%04x", u->me);
	ret = dev_set(ctx, pkg, u->dev, cmt, "autoneg", "1");
	if (!ret && u->disabled)
		ret = dev_set(ctx, pkg, u->dev, cmt, "enabled", "0");
	if (!ret && u->speed) {
		snprintf(val, sizeof(val), "%u", u->speed);
		ret = dev_set(ctx, pkg, u->dev, cmt, "speed", val);
	}
	if (!ret && u->duplex >= 0)
		ret = dev_set(ctx, pkg, u->dev, cmt, "duplex",
			      u->duplex ? "1" : "0");
	if (!ret && u->mtu) {
		snprintf(val, sizeof(val), "%u", u->mtu);
		ret = dev_set(ctx, pkg, u->dev, cmt, "mtu", val);
	}
	if (!ret && u->pause_set)
		ret = dev_set(ctx, pkg, u->dev, cmt, "pause",
			      u->pause ? "1" : "0");

	return ret;
}

/* netifd multicast_router value of a MROUTER_* code */
static const char *mrouter_str(uint8_t mrouter)
{
	return mrouter == MROUTER_ALWAYS ? "2" : "0";
}

static bool port_is_vlan(const struct eth_model *m, const char *dev)
{
	int i;

	for (i = 0; i < m->nvlan; i++)
		if (!strcmp(m->vlan[i].name, dev))
			return true;

	return false;
}

/* detach a claimed UNI device (e.g. lan3) from any admin bridge (br-lan) */
static void detach_from_admin_bridges(struct uci_context *ctx,
				      struct uci_package *pkg, const char *dev)
{
	struct uci_element *e;

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		const char *type;

		if (strcmp(s->type, "device") || persist_ours(ctx, s))
			continue;
		type = uci_lookup_option_string(ctx, s, "type");
		if (!type || strcmp(type, "bridge"))
			continue;
		persist_del_list(ctx, "network", s->e.name, "ports", dev);
	}
}

static int uci_write_vlan(struct uci_context *ctx, const struct eth_vlan *v)
{
	char sn[64], val[24];
	int j, ret = 0;

#define SET(_o, _v)	ret = ret ? ret : persist_set(ctx, "network", sn, _o, _v)
	sect_name(sn, sizeof(sn), "v", v->name);
	SET(NULL, "device");
	SET("comment", "gpond vlan");
	SET("type", v->tpid == TPID_8021AD ? "8021ad" : "8021q");
	SET("name", v->name);
	SET("ifname", v->parent);
	snprintf(val, sizeof(val), "%u", v->vid);
	SET("vid", val);
#undef SET
	if (!ret && v->fast_leave)
		ret = persist_set(ctx, "network", sn, "multicast_fast_leave", "1");
	if (!ret && v->mrouter)
		ret = persist_set(ctx, "network", sn, "multicast_router",
				  mrouter_str(v->mrouter));
	for (j = 0; j < 8 && v->ingress == ING_IDENT && !ret; j++) {
		snprintf(val, sizeof(val), "%d:%d", j, j);
		ret = persist_add_list(ctx, "network", sn,
				       "ingress_qos_mapping", val);
	}
	for (j = 0; j < 8 && v->egress != EGR_NONE && !ret; j++) {
		snprintf(val, sizeof(val), "%d:%d", j,
			 v->egress == EGR_IDENT ? j : v->egress);
		ret = persist_add_list(ctx, "network", sn,
				       "egress_qos_mapping", val);
	}

	return ret;
}

static int uci_write(const struct eth_model *m, bool *changed)
{
	struct uci_package *pkg;
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	char *before, *after;
	char sn[64], val[32];
	bool ch;
	int i, j, ret = 0;

	ctx = persist_open("network", &pkg);
	if (!ctx)
		return -EIO;
	before = persist_snapshot(ctx, pkg);

	/* drop what gpond wrote before */
	uci_foreach_element_safe(&pkg->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		if (persist_ours(ctx, s))
			persist_set(ctx, "network", s->e.name, NULL, NULL);
	}

	for (i = 0; i < m->nvlan && !ret; i++)
		ret = uci_write_vlan(ctx, &m->vlan[i]);

#define SET(_o, _v)	ret = ret ? ret : persist_set(ctx, "network", sn, _o, _v)
	for (i = 0; i < m->nbr && !ret; i++) {
		const struct eth_bridge *b = &m->br[i];

		sect_name(sn, sizeof(sn), "b", b->name);
		SET(NULL, "device");
		snprintf(val, sizeof(val), "gpond me 45 0x%04x", b->id);
		SET("comment", val);
		SET("type", "bridge");
		SET("name", b->name);
		SET("bridge_empty", "1");
		SET("igmp_snooping", b->igmp ? "1" : "0");
		/* ME 45: spanning tree and the bridge timers (seconds) */
		SET("stp", b->stp ? "1" : "0");
		if (b->priority) {
			snprintf(val, sizeof(val), "%u", b->priority);
			SET("priority", val);
		}
		if (b->max_age) {
			snprintf(val, sizeof(val), "%u", b->max_age);
			SET("max_age", val);
		}
		if (b->hello_time) {
			snprintf(val, sizeof(val), "%u", b->hello_time);
			SET("hello_time", val);
		}
		if (b->forward_delay) {
			snprintf(val, sizeof(val), "%u", b->forward_delay);
			SET("forward_delay", val);
		}
		if (b->ageing_time) {
			snprintf(val, sizeof(val), "%u", b->ageing_time);
			SET("ageing_time", val);
		}
		/* netifd turns the querier on with igmp_snooping, bridge.c:1233 */
		if (b->igmp)
			SET("multicast_querier", "0");
		if (b->robustness) {
			snprintf(val, sizeof(val), "%d", b->robustness);
			SET("robustness", val);
		}
		if (b->query_interval) {
			snprintf(val, sizeof(val), "%u", b->query_interval);
			SET("query_interval", val);
		}
		if (b->query_response) {
			snprintf(val, sizeof(val), "%u", b->query_response);
			SET("query_response_interval", val);
		}
		if (b->last_member) {
			snprintf(val, sizeof(val), "%u", b->last_member);
			SET("last_member_interval", val);
		}
		for (j = 0; j < b->nport && !ret; j++)
			ret = persist_add_list(ctx, "network", sn, "ports",
					       b->port[j].dev);

		/* per port options; a VLAN device carries its own in its section */
		for (j = 0; j < b->nport && !ret; j++) {
			const struct eth_port *p = &b->port[j];

			/* netifd's vlandev has no learning / unicast_flood
			 * options: the ME 45 flags reach the raw ports only */
			if (port_is_vlan(m, p->dev))
				continue;
			if (dev_is_uni(p->dev))
				detach_from_admin_bridges(ctx, pkg, p->dev);
			if (!b->learning)
				ret = dev_set(ctx, pkg, p->dev, NULL,
					      "learning", "0");
			if (!ret && b->unknown_discard)
				ret = dev_set(ctx, pkg, p->dev, NULL,
					      "unicast_flood", "0");
			if (!ret && p->fast_leave)
				ret = dev_set(ctx, pkg, p->dev, NULL,
					      "multicast_fast_leave", "1");
			if (!ret && p->mrouter)
				ret = dev_set(ctx, pkg, p->dev, NULL,
					      "multicast_router",
					      mrouter_str(p->mrouter));
		}
	}
#undef SET

	for (i = 0; i < m->nuni && !ret; i++)
		ret = uci_write_uni(ctx, pkg, &m->uni[i]);
	for (i = 0; i < m->nl3 && !ret; i++)
		ret = eth_l3_uci(ctx, pkg, &m->l3[i]);

	after = persist_snapshot(ctx, pkg);
	ch = !before || !after || strcmp(before, after);
	free(before);
	free(after);
	if (changed)
		*changed = ch;

	if (ret || !ch) {
		persist_close(ctx, pkg, false);
		return ret;
	}

	return persist_close(ctx, pkg, true);
}

/* policing and tc filters are not netifd's: /etc/config/gpon */
static int uci_write_gpon(const struct eth_model *m, bool *changed)
{
	struct uci_package *pkg;
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	char *before, *after;
	char sn[64], val[32], cmt[96];
	bool ch;
	int i, ret = 0;

	ctx = persist_open(GPOND_PKG, &pkg);
	if (!ctx)
		return -EIO;
	before = persist_snapshot(ctx, pkg);

	uci_foreach_element_safe(&pkg->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		/* police, vrule, mport: written below from the model */
		if (persist_ours(ctx, s))
			persist_set(ctx, GPOND_PKG, s->e.name, NULL, NULL);
	}

#define XSET(_o, _v)	ret = ret ? ret : persist_set(ctx, GPOND_PKG, sn, _o, _v)
	for (i = 0; i < m->npol && !ret; i++) {
		const struct eth_police *p = &m->pol[i];

		snprintf(val, sizeof(val), "%s", p->ingress ? "in" : "out");
		sect_name(sn, sizeof(sn), val, p->dev);
		XSET(NULL, "police");
		snprintf(cmt, sizeof(cmt), "gpond police %s %s", p->dev,
			 p->ingress ? "in" : "out");
		XSET("comment", cmt);
		XSET("device", p->dev);
		XSET("direction", val);
		snprintf(val, sizeof(val), "%u", p->rate);
		XSET("rate", val);
		snprintf(val, sizeof(val), "%u", p->burst);
		XSET("burst", val);
	}
	for (i = 0; i < m->ntc && !ret; i++) {
		const struct eth_tc *t = &m->tc[i];

		snprintf(sn, sizeof(sn), "vr%d", i);
		XSET(NULL, "vrule");
		snprintf(cmt, sizeof(cmt), "gpond vlan rule %s", t->dev);
		XSET("comment", cmt);
		XSET("device", t->dev);
		XSET("direction", t->egress ? "egress" : "ingress");
		snprintf(val, sizeof(val), "%u", t->pref);
		XSET("pref", val);
		XSET("proto", t->proto);
		XSET("match", t->match);
		XSET("action", t->action);
	}

#undef XSET
	ret = ret ? ret : eth_mcast_gpon_uci(ctx, m);

	after = persist_snapshot(ctx, pkg);
	ch = !before || !after || strcmp(before, after);
	free(before);
	free(after);
	if (changed)
		*changed = ch;

	if (ret || !ch) {
		persist_close(ctx, pkg, false);
		return ret;
	}

	return persist_close(ctx, pkg, true);
}

/* ---------------------------------------------------------------------
 * sync
 * ------------------------------------------------------------------- */

/*
 * global.own_config_only: what OMCI would have put into the other
 * packages is only logged, once per change of the model, so the
 * administrator can set it up by hand.
 */
static void own_config_note(const struct eth_model *m, bool mcast, bool cwmp)
{
	int i;

	INFO("eth: own_config_only: /etc/config/network left alone "
	     "(%d bridge(s), %d VLAN device(s), %d IP interface(s), "
	     "%d UNI port(s) asked for)", m->nbr, m->nvlan, m->nl3, m->nuni);
	for (i = 0; i < m->nvlan; i++)
		INFO("eth:   VLAN device %s on %s, VID %u", m->vlan[i].name,
		     m->vlan[i].parent, m->vlan[i].vid);
	for (i = 0; i < m->nuni; i++)
		INFO("eth:   UNI port %s", m->uni[i].dev);
	if (mcast)
		INFO("eth: own_config_only: /etc/config/omcproxy left alone");
	if (cwmp && cfg.setup_cwmp)
		INFO("eth: own_config_only: /etc/config/easycwmp left alone");
}

static void sync_cb(struct uloop_timeout *t)
{
	int i, nus = 0;
	bool mcp, cwp, nch = false, xch = false, mch = false, cch = false;

	build(&next);
	us_sync(&next);
	if (model_equal(&next, &cur)) {
		/* a MIB reset and the same provisioning again */
		eth_l3_applied(&cur);
		return;
	}

	for (i = 0; i < ETH_MAX_US_RULES; i++)
		nus += next.us[i].valid;
	INFO("eth: %d bridge(s), %d VLAN device(s), %d tc rule(s), %d L3 "
	     "interface(s), %d policer(s), %d upstream rule(s), %d UNI(s)",
	     next.nbr, next.nvlan, next.ntc, next.nl3, next.npol, nus,
	     next.nuni);
	for (i = 0; i < next.nuni; i++)
		if (next.uni[i].loopback &&
		    (i >= cur.nuni ||
		     cur.uni[i].loopback != next.uni[i].loopback))
			WARN("eth: UNI %s: loopback %u not supported (no netifd "
			     "option), only reported", next.uni[i].dev,
			     next.uni[i].loopback);

	eth_mcast_report(&next);
	mcp = eth_mcast_proxy_changed(&cur, &next) &&
	      eth_mcast_proxy_present(&next);
	cwp = memcmp(&cur.cwmp, &next.cwmp, sizeof(cur.cwmp)) != 0;
	if (cfg.own_config_only)
		own_config_note(&next, mcp, cwp);
	if ((!cfg.own_config_only && uci_write(&next, &nch)) ||
	    uci_write_gpon(&next, &xch)) {
		ERR("eth: cannot write UCI");
	} else {
		/* a package that did not change is not reloaded */
		if (nch && ubus_network_reload())
			ERR("eth: network reload failed");
		if (xch && ubus_config_change(GPOND_PKG))
			ERR("eth: cannot notify procd (%s)", GPOND_PKG);
		if (mcp && !cfg.own_config_only) {
			if (eth_mcast_proxy_write(&next, &mch))
				ERR("eth: cannot write the omcproxy UCI");
			else if (mch && ubus_config_change(OMCP_PKG))
				ERR("eth: cannot notify procd (omcproxy)");
		}
		if (cwp && !cfg.own_config_only) {
			if (!cfg.setup_cwmp) {
				if (next.cwmp.present)
					INFO("eth: TR-069 (ME 340) ignored "
					     "(setup_cwmp=0)");
			} else if (eth_cwmp_write(&next, &cch)) {
				ERR("eth: cannot write the easycwmp UCI");
			} else if (cch && ubus_config_change(CWMP_PKG)) {
				ERR("eth: cannot notify procd (easycwmp)");
			}
		}
	}

	cur = next;
	eth_l3_applied(&cur);
	if (mibstore_dirty())
		mibstore_submit();
}

/* removes the sections of @pkgname that are ours; how many, or -errno */
static int drop_ours(const char *pkgname)
{
	struct uci_package *pkg;
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	int n = 0, ret;

	ctx = persist_open(pkgname, &pkg);
	if (!ctx)
		return -EIO;

	uci_foreach_element_safe(&pkg->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		if (persist_ours(ctx, s) &&
		    !persist_set(ctx, pkgname, s->e.name, NULL, NULL))
			n++;
	}

	ret = persist_close(ctx, pkg, n > 0);

	return ret ? ret : n;
}

/*
 * factory: every section of /etc/config that says it came from gpond goes,
 * whatever the package.  What changed is told to its service afterwards.
 */
int eth_factory(void)
{
	struct uci_context *ctx = uci_alloc_context();
	char **names = NULL, **p;
	int ret = 0;

	if (!ctx)
		return -ENOMEM;
	if (uci_list_configs(ctx, &names) || !names) {
		uci_free_context(ctx);
		return -EIO;
	}

	for (p = names; *p; p++) {
		int n;

		/* own_config_only: the other packages were never written */
		if (cfg.own_config_only && strcmp(*p, GPOND_PKG))
			continue;
		n = drop_ours(*p);

		if (n < 0) {
			ERR("factory: cannot clean /etc/config/%s", *p);
			ret = n;
			continue;
		}
		if (!n)
			continue;
		INFO("factory: %d section(s) removed from /etc/config/%s", n, *p);
		if (!strcmp(*p, "network")) {
			if (ubus_network_reload())
				ERR("factory: network reload failed");
		} else if (ubus_config_change(*p)) {
			ERR("factory: cannot notify procd (%s)", *p);
		}
	}
	free(names);
	uci_free_context(ctx);

	return ret;
}

/* the MIB changed: rebuild once it has been quiet for a moment */
void eth_changed(void)
{
	if (gpond_stopping)
		return;
	uloop_timeout_set(&sync_tmo, ETH_DEBOUNCE_MS);
}

void eth_init(void)
{
	sync_tmo.cb = sync_cb;
	eth_l3_init();
}

void eth_dump(struct blob_buf *b)
{
	void *a, *t, *p;
	int i, j;

	a = blobmsg_open_array(b, "bridges");
	for (i = 0; i < cur.nbr; i++) {
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "name", cur.br[i].name);
		blobmsg_add_u32(b, "me", cur.br[i].id);
		blobmsg_add_u8(b, "igmp_snooping", cur.br[i].igmp);
		p = blobmsg_open_array(b, "ports");
		for (j = 0; j < cur.br[i].nport; j++)
			blobmsg_add_string(b, NULL, cur.br[i].port[j].dev);
		blobmsg_close_array(b, p);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);

	a = blobmsg_open_array(b, "vlans");
	for (i = 0; i < cur.nvlan; i++)
		blobmsg_add_string(b, NULL, cur.vlan[i].name);
	blobmsg_close_array(b, a);

	a = blobmsg_open_array(b, "tc");
	for (i = 0; i < cur.ntc; i++) {
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "device", cur.tc[i].dev);
		blobmsg_add_string(b, "direction",
				   cur.tc[i].egress ? "egress" : "ingress");
		blobmsg_add_u32(b, "pref", cur.tc[i].pref);
		blobmsg_add_string(b, "proto", cur.tc[i].proto);
		blobmsg_add_string(b, "match", cur.tc[i].match);
		blobmsg_add_string(b, "action", cur.tc[i].action);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);

	blobmsg_add_u8(b, "dscp_map", cur.dscp_on);
	a = blobmsg_open_array(b, "upstream");
	for (i = 0; i < ETH_MAX_US_RULES; i++) {
		if (!cur.us[i].valid)
			continue;
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "vid", cur.us[i].vid);
		blobmsg_add_u32(b, "pcp_mask", cur.us[i].pcp_mask);
		blobmsg_add_u32(b, "gem", cur.us[i].gem);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);

	eth_l3_dump(b);
	eth_mcast_dump(b);

	a = blobmsg_open_array(b, "uni");
	for (i = 0; i < cur.nuni; i++) {
		const struct eth_uni *u = &cur.uni[i];

		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "device", u->dev);
		blobmsg_add_u32(b, "me", u->me);
		blobmsg_add_u8(b, "enabled", !u->disabled);
		if (u->speed)
			blobmsg_add_u32(b, "speed", u->speed);
		if (u->duplex >= 0)
			blobmsg_add_string(b, "duplex", u->duplex ? "full" : "half");
		if (u->mtu)
			blobmsg_add_u32(b, "mtu", u->mtu);
		if (u->loopback)
			blobmsg_add_u32(b, "loopback", u->loopback);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);

	a = blobmsg_open_array(b, "police");
	for (i = 0; i < cur.npol; i++) {
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_string(b, "device", cur.pol[i].dev);
		blobmsg_add_string(b, "direction",
				   cur.pol[i].ingress ? "in" : "out");
		blobmsg_add_u32(b, "rate", cur.pol[i].rate);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}
