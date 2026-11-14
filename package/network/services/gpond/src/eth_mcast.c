// SPDX-License-Identifier: GPL-2.0
/*
 * Multicast operations profile (ME 309) / multicast subscriber config info
 * (ME 310) -> Linux.  The ME side (value checks, tables, the ME 311
 * monitor) is omci_mcast.c, the attribute list with the SDK references is
 * at its top.
 *
 * Which profile a port gets: ME 310 of the bridge port (ME type 0, instance
 * id = the bridge port config data), else the one of the 802.1p mapper
 * (ME type 1) or of the ANI bridge port (ME type 0) of the same bridge,
 * which the SDK applies to every LAN port (getLanPortByMeType(),
 * omci_me_multicast_func.c:311-409).  The bridge-wide settings (snooping,
 * robustness, query timers) come from the first profile found.
 *
 * Mapping, everything through netifd / procd / UCI (gpond writes only the
 * sections that say "comment gpond ..."):
 *   function 0, 1, 2   bridge igmp_snooping 1 with multicast_querier 0
 *                      (bridge.c:1233-1236: igmp_snooping alone turns the
 *                      querier on): the vendor stack answers a leave with a
 *                      group specific query to the port and sends no
 *                      periodic general queries (xpon_igmp_core.c:1030-1075,
 *                      1315, 1867), its work_mode is stored and read nowhere
 *                      (xpon_igmp_itf.c:343-364); on a leave the
 *                      kernel bridge removes the group at once with fast
 *                      leave, shortens the group timer to last member
 *                      count * interval when it hears no querier, and
 *                      leaves the group to the OLT's querier otherwise
 *                      (br_multicast.c:3655-3745 br_multicast_leave_group()).
 *                      Function 2 with a router side behind VEIP / IP host
 *                      and onu.mcast_downlink: an omcproxy section
 *                      (/etc/config/omcproxy: uplink, list downlink,
 *                      omcproxy.init:15-44, reload trigger :128); without them it stays snooping
 *                      (what the vendor does for every function).
 *   immediate leave    per UNI port multicast_fast_leave (device.c:501)
 *   robustness         bridge robustness (bridge.c:1242): startup and last
 *                      member query count, membership interval
 *   query interval     bridge query_interval, seconds -> centiseconds
 *   max response time  bridge query_response_interval, 1/10 s -> cs
 *   last member query  bridge last_member_interval, 1/10 s -> cs
 *   (units: the SDK defaults 125 / 100 / 10, omci_me_multicast.h:124-130,
 *   and the vendor kernel: seconds for the interval, xpon_igmp_core.c:1430,
 *   the Max Resp Code of the query in 1/10 s, :1212; G.988 is not
 *   available: unverified)
 *   upstream IGMP tag control 1..3, TCI VID != 0
 *                      the VLAN device pon0.<vid> (egress P-bit of the TCI;
 *                      control 3 keeps the frame priority) becomes the
 *                      permanent multicast router port (multicast_router 2)
 *                      of the bridge: the untagged IGMP of the UNI side is
 *                      forwarded to it only and leaves tagged; the UNI
 *                      ports are never router ports (multicast_router 0).
 *                      tc on the UNI ingress keeps the IGMP away from the
 *                      VLAN rows (pass) and removes the tag of a tagged
 *                      message for control 2 / 3 (vlan pop)
 *   downstream IGMP and TCI control 1 (strip)
 *                      the VLAN device pon0.<vid> of the TCI (or of the ACL
 *                      rows when the TCI VID is 0) is an ANI port: it
 *                      removes the tag of the multicast frames
 *   downstream IGMP and TCI control 2 / 3 / 4 (add / replace TCI / replace
 *                      VID, xpon_igmp_core.c:738-806 cases 2, 3, 4)
 *                      a tc filter on the ingress of the raw ANI device
 *                      (pon0, before the bridge and before the ME 171
 *                      rules of eth_vlan.c) for every frame with an IPv4
 *                      multicast destination MAC: "vlan push" with the TCI
 *                      of the attribute (2, on an untagged frame - the
 *                      vendor pushes a tag onto every frame it forwards) or
 *                      "vlan modify id [priority]" (3, the VID and the
 *                      priority, 4, the VID alone; a tc action has no DEI,
 *                      act_vlan.c:54-74).  A VLAN device of its own
 *                      rewrites the tag again on the way to the bridge, so
 *                      with one only the untagged multicast is affected
 *   upstream IGMP rate tc police pkts_rate on the ingress of the UNI,
 *                      IPv4 IGMP and the MLD reports that go to 224.0.0.22:
 *                      the IGMP module applies the rate limit of a UNI
 *                      before it looks at the protocol
 *                      (xpon_uni_incoming_handle(),
 *                      xpon_igmp_core.c:3450-3457), so an MLD report counts
 *                      too.  Continue when conform (the ME 171 rows after
 *                      it still apply)
 *   max simultaneous groups (ME 310) per port mcast_max_groups (kernel
 *                      br_netlink.c:1052-1056).  netifd has no option for
 *                      it: "mport" sections of /etc/config/gpon, applied by
 *                      the procd service xpon-mcast with
 *                      "bridge link set dev <port> mcast_max_groups N"
 *                      (iproute2 bridge/link.c:428)
 *   dynamic access control (ME 309 attribute 7)
 *                      per port IGMP report filter of the procd service
 *                      xpon-mcast, a bridge family nft table "xpon_mc"
 *                      (hook prerouting, before the snooping): with at least
 *                      one row an IGMP v1/v2 report is dropped unless its
 *                      group is in a row range, a v3 report unless it has one
 *                      group record and that group is in a range (more records
 *                      cannot be walked by nft), as the vendor does for the
 *                      groups (xpon_igmp_core.c:1463-1500, 1700-1725).  The
 *                      row source, VLAN and GEM are not checked, nor MLD.
 *                      A range whose imputed bandwidth does not fit the max
 *                      multicast bandwidth of the subscriber (attribute 4)
 *                      with bandwidth enforcement (attribute 5) on is left
 *                      out, which is how the vendor refuses the join
 *                      (xpon_igmp_core.c:4229-4306)
 *   static access control (ME 309 attribute 8)
 *                      a row that names one group becomes a permanent entry
 *                      of the bridge MDB of that port ("bridge mdb add dev
 *                      <bridge> port <port> grp <group> permanent", the
 *                      service xpon-mcast): the group reaches the port
 *                      without a report, as the vendor forwards a flow of a
 *                      static row whatever the snooping table says
 *                      (xpon_igmp_core.c:2032-2045, 2266-2291).  The report
 *                      of a host for such a group is dropped in the same nft
 *                      table: the kernel would keep a second MDB entry for
 *                      it and deliver the flow twice (br_forward.c:297-353),
 *                      the vendor sends one copy (xpon_igmp_core.c:2287-2291).
 *                      A row with a range of groups stays stored only, the
 *                      MDB holds groups, not ranges.  A static match is
 *                      never counted against the max bandwidth
 *                      (xpon_igmp_core.c:4161-4226)
 * Not expressible, stored and reported only (eth_mcast_report()): the static
 * access control rows with more than one group, the unauthorized join
 * behaviour, the service package and preview group tables, the querier IP,
 * the IGMP version, the downstream controls 5..7, upstream tag control values
 * above 3, the ME 311 IPv6 active group table and the ME 309 lost groups
 * table (no producer in the vendor either).
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uci.h>

#include "gpond.h"
#include "eth.h"
#include "omci_mcast.h"

#define MC_MAX_ACL_VIDS		4

/* ---------------------------------------------------------------------
 * profile resolution
 * ------------------------------------------------------------------- */

static bool tp_ani(uint8_t type)
{
	return type == TP_8021P_MAPPER || type == TP_GEM_IWTP ||
	       type == TP_MCAST_GEM_IWTP;
}

/* ME 310 of a bridge port; @id the bridge (ME 45) it is in */
static struct omci_inst *port_subscriber(uint16_t bridge_id,
					 struct omci_inst *bp)
{
	struct omci_inst *sub = mc_subscriber(MC_TYPE_BRIDGE_PORT, bp->id), *o;

	if (sub)
		return sub;
	/* every LAN port: the ANI port's own ME 310, or its mapper's */
	list_for_each_entry(o, &omci_mib, list) {
		uint8_t type;

		if (o->def->class_id != 47 || mib_get_uint(o, 1) != bridge_id)
			continue;
		type = mib_get_uint(o, 3);
		if (!tp_ani(type))
			continue;
		sub = mc_subscriber(MC_TYPE_BRIDGE_PORT, o->id);
		if (!sub && type == TP_8021P_MAPPER)
			sub = mc_subscriber(MC_TYPE_MAPPER, mib_get_uint(o, 4));
		if (sub)
			return sub;
	}

	return NULL;
}

/* the Linux UNI devices a multicast ME (type, id) covers */
int eth_mcast_ports(uint8_t me_type, uint16_t id, const char **dev, int max)
{
	struct omci_inst *bp, *o;
	uint16_t bridge = 0;
	bool found = false;
	int n = 0, i;

	list_for_each_entry(bp, &omci_mib, list) {
		if (bp->def->class_id != 47)
			continue;
		if (me_type == MC_TYPE_BRIDGE_PORT && bp->id == id) {
			found = true;
			bridge = mib_get_uint(bp, 1);
			break;
		}
		if (me_type == MC_TYPE_MAPPER &&
		    mib_get_uint(bp, 3) == TP_8021P_MAPPER &&
		    mib_get_uint(bp, 4) == id) {
			found = true;
			bridge = mib_get_uint(bp, 1);
			break;
		}
	}
	if (!found)
		return 0;
	/* a UNI is itself; the ANI ports mean every UNI of their bridge; a
	 * router side (VEIP, IP host) has no MDB to look at */
	if (me_type == MC_TYPE_BRIDGE_PORT) {
		uint8_t type = mib_get_uint(bp, 3);

		if (type == TP_PPTP_ETH_UNI) {
			const char *d = eth_uni_dev(mib_get_uint(bp, 4));

			if (d && max > 0)
				dev[n++] = d;
			return n;
		}
		if (!tp_ani(type))
			return 0;
	}
	list_for_each_entry(o, &omci_mib, list) {
		const char *d;

		if (o->def->class_id != 47 || mib_get_uint(o, 1) != bridge ||
		    mib_get_uint(o, 3) != TP_PPTP_ETH_UNI)
			continue;
		d = eth_uni_dev(mib_get_uint(o, 4));
		for (i = 0; d && i < n; i++)
			if (!strcmp(dev[i], d))
				d = NULL;
		if (d && n < max)
			dev[n++] = d;
	}

	return n;
}

/* ---------------------------------------------------------------------
 * model
 * ------------------------------------------------------------------- */

static const struct eth_vlan *vlan_find(const struct eth_model *m,
					const char *name)
{
	int i;

	for (i = 0; i < m->nvlan; i++)
		if (!strcmp(m->vlan[i].name, name))
			return &m->vlan[i];

	return NULL;
}

/*
 * May a join of a dynamic ACL row with the bandwidth @used so far on the port
 * be honoured?  The IGMP module sums the imputed bandwidth of the dynamic rows
 * the port has joined and drops the join that passes the limit, and only when
 * the subscriber enforces it (xpon_ds_mcast_rate_limit_handle(),
 * xpon_igmp_core.c:4229-4306); a row without an imputed bandwidth is never
 * limited (:4229, 4298) and the static table counts nothing (:4161-4226).
 */
static bool bw_fits(struct omci_inst *sub, uint32_t used, const struct mc_acl *a)
{
	uint32_t maxbw;

	if (!sub || !mib_get_uint(sub, MSC_BW_ENFORCEMENT))
		return true;
	maxbw = mc_max_bw(sub);

	return !maxbw || !a->bandwidth || used + a->bandwidth <= maxbw;
}

/*
 * The groups an IGMP report of a port may name:
 *   dynamic access control table (ME 309 attribute 7): only these ranges, an
 *   empty table lets every group through
 *   (upstream_dynamic_acl_ctr_igmpv2(), xpon_igmp_core.c:1463-1500).  A row
 *   whose imputed bandwidth does not fit what is left of the max multicast
 *   bandwidth of the subscriber (ME 310 attribute 4) with bandwidth
 *   enforcement (attribute 5) on is left out: the vendor counts the
 *   bandwidth of the dynamic rows only and drops the join that would pass
 *   the limit (xpon_ds_mcast_rate_limit_handle(),
 *   xpon_igmp_core.c:4229-4306), a row without an imputed bandwidth is
 *   never limited (:4229, 4298).  The rows are admitted in table order,
 *   which is the order the vendor meets the joins in.
 *   static access control table (ME 309 attribute 8): the rows that name
 *   one group.  The service xpon-mcast keeps them in the bridge MDB
 *   (permanent) and drops their reports in the nft table, so the group
 *   reaches the port without a report; they are in this list as well so
 *   that a report is not refused by the range check when the nft rule of
 *   the service could not be installed.
 * A table that does not fit stays unenforced.  Rows with an empty range are
 * skipped, equal ranges once.
 */
static void acl_collect(struct omci_inst *prof, struct omci_inst *sub,
			struct eth_port *p)
{
	static const uint8_t attr[2] = { MOP_DYN_ACL, MOP_STATIC_ACL };
	uint32_t used = 0;
	int t;

	p->nacl = 0;
	for (t = 0; t < 2; t++) {
		const uint8_t *tab = mib_val(prof, attr[t]);
		unsigned int o, k;

		for (o = 0; tab && o + MC_ACL_LEN <= prof->len[attr[t]];
		     o += MC_ACL_LEN) {
			struct mc_acl a;

			mc_acl_decode(tab + o, &a);
			if (memcmp(a.start, a.end, 4) > 0)
				continue;
			/* a range of groups is not in the MDB */
			if (t && !mc_acl_single(&a))
				continue;
			for (k = 0; k < p->nacl; k++)
				if (!memcmp(p->acl[k].start, a.start, 4) &&
				    !memcmp(p->acl[k].end, a.end, 4))
					break;
			if (k < p->nacl)
				continue;
			/* the static table counts no bandwidth */
			if (!t) {
				if (!bw_fits(sub, used, &a))
					continue;
				used += a.bandwidth;
			}
			if (p->nacl == ETH_MAX_ACL) {
				p->nacl = 0;
				WARN("eth: multicast ACL of %s has more than %d ranges: not enforced",
				     p->dev, ETH_MAX_ACL);
				return;
			}
			memcpy(p->acl[p->nacl].start, a.start, 4);
			memcpy(p->acl[p->nacl].end, a.end, 4);
			p->nacl++;
		}
	}
}

/* the options of a bridge port that is a VLAN device live in its own section */
static void port_opts(struct eth_model *m, struct eth_port *p, bool fast_leave,
		      uint8_t mrouter, uint16_t max_groups)
{
	struct eth_vlan *v = (struct eth_vlan *)vlan_find(m, p->dev);

	if (fast_leave) {
		p->fast_leave = true;
		if (v)
			v->fast_leave = true;
	}
	if (mrouter) {
		p->mrouter = mrouter;
		if (v)
			v->mrouter = mrouter;
	}
	if (max_groups)
		p->max_groups = max_groups;
}

/* the ports of @b that carry the traffic of the UNI device @uni */
static bool uni_port_of(const struct eth_model *m, const struct eth_port *p,
			const char *uni)
{
	const struct eth_vlan *v;

	if (!strcmp(p->dev, uni))
		return true;
	v = vlan_find(m, p->dev);

	return v && !strcmp(v->parent, uni);
}

static uint32_t cs(uint32_t v, uint32_t unit, uint32_t max)
{
	return v > max ? max * unit : v * unit;
}

/* bridge-wide parameters of one profile */
static void bridge_params(struct eth_bridge *b, struct omci_inst *prof)
{
	uint32_t v;

	b->igmp = true;
	v = mib_get_uint(prof, MOP_ROBUSTNESS);
	if (v)
		b->robustness = v > 255 ? 255 : v;
	/* 0 (and what the vendor stack refuses) keeps the netifd default */
	v = mib_get_uint(prof, MOP_QUERY_INTERVAL);
	if (v)
		b->query_interval = cs(v, 100, 86400);
	v = mib_get_uint(prof, MOP_QUERY_RESPONSE);
	if (v)
		b->query_response = cs(v, 10, 86400 * 10);
	v = mib_get_uint(prof, MOP_LAST_MEMBER);
	if (v)
		b->last_member = cs(v, 10, 86400 * 10);
}

/* VIDs the multicast frames of a profile come with: ACL rows */
static int acl_vids(struct omci_inst *prof, uint16_t *vid, int max)
{
	static const uint8_t attr[2] = { MOP_DYN_ACL, MOP_STATIC_ACL };
	int n = 0, t, i;

	for (t = 0; t < 2; t++) {
		const uint8_t *tab = prof->val[attr[t]];
		unsigned int o;

		for (o = 0; tab && o + MC_ACL_LEN <= prof->len[attr[t]];
		     o += MC_ACL_LEN) {
			struct mc_acl a;
			uint16_t v;

			mc_acl_decode(tab + o, &a);
			v = a.vid & 0xfff;
			for (i = 0; i < n && vid[i] != v; i++)
				;
			if (v && v < VID_ANY - 1 && i == n && n < max)
				vid[n++] = v;
		}
	}

	return n;
}

/* rows of one access control table, and the ones that name a single group */
static int acl_rows(struct omci_inst *prof, uint8_t idx, int *single)
{
	const uint8_t *tab = mib_val(prof, idx);
	unsigned int o;
	int n = 0;

	if (single)
		*single = 0;
	for (o = 0; tab && o + MC_ACL_LEN <= prof->len[idx]; o += MC_ACL_LEN) {
		struct mc_acl a;

		mc_acl_decode(tab + o, &a);
		if (memcmp(a.start, a.end, 4) > 0)
			continue;
		n++;
		if (single && mc_acl_single(&a))
			(*single)++;
	}

	return n;
}

/* an ANI port that is a VLAN device of pon0 */
static const char *ani_leaf(struct eth_model *m, struct eth_bridge *b,
			    uint16_t vid, int egress)
{
	const char *d;

	d = eth_vlan_dev(m, GPON_NETDEV, TPID_8021Q, vid, egress, ING_DONTCARE);
	if (!d && egress != EGR_DONTCARE) {
		d = eth_vlan_dev(m, GPON_NETDEV, TPID_8021Q, vid, EGR_DONTCARE,
				 ING_DONTCARE);
		if (d)
			INFO("eth: multicast VLAN %u: its priority differs from "
			     "the VLAN device already there", vid);
	}
	if (!d || !eth_add_port(b, d))
		return NULL;
	eth_bridge_ani(b, d, vid);

	return d;
}

/* the VID the upstream IGMP is tagged with (tag control 1..3), 0: none */
static uint16_t up_vid(struct omci_inst *prof)
{
	uint8_t tag = mib_get_uint(prof, MOP_UP_TAG);
	uint16_t vid = mib_get_uint(prof, MOP_UP_TCI) & 0xfff;

	if (tag < MC_UPTAG_ADD_TCI || tag > MC_UPTAG_REPLACE_VID ||
	    !vid || vid >= VID_ANY - 1)
		return 0;

	return vid;
}

/* settings of one UNI from its profile + subscriber */
static void uni_apply(struct eth_model *m, struct eth_bridge *b,
		      const char *uni, struct omci_inst *sub,
		      struct omci_inst *prof)
{
	uint8_t tag = mib_get_uint(prof, MOP_UP_TAG);
	uint16_t tci = mib_get_uint(prof, MOP_UP_TCI);
	const uint8_t *ds = mib_val(prof, MOP_DOWN_TCI);
	bool fast = mib_get_uint(prof, MOP_FAST_LEAVE);
	uint16_t maxg = mc_max_groups(sub);
	int i;

	for (i = 0; i < b->nport; i++)
		if (uni_port_of(m, &b->port[i], uni)) {
			port_opts(m, &b->port[i], fast, MROUTER_NEVER, maxg);
			acl_collect(prof, sub, &b->port[i]);
		}

	/* upstream IGMP: the tagged leaf is the only router port */
	if (up_vid(prof)) {
		const char *d = ani_leaf(m, b, tci & 0xfff,
					 tag == MC_UPTAG_REPLACE_VID ? EGR_IDENT :
					 tci >> 13);
		struct eth_port *p = d ? eth_add_port(b, d) : NULL;

		if (p)
			port_opts(m, p, false, MROUTER_ALWAYS, 0);
	}

	/* downstream multicast: the tag of the multicast VLAN is stripped */
	if (ds && ds[0] == MC_DOWNTAG_STRIP) {
		uint16_t vid[MC_MAX_ACL_VIDS];
		int n = 0;

		vid[0] = ((ds[1] << 8) | ds[2]) & 0xfff;
		if (vid[0] && vid[0] < VID_ANY - 1)
			n = 1;
		else
			n = acl_vids(prof, vid, MC_MAX_ACL_VIDS);
		if (!n)
			n = mc_service_vids(sub, vid, MC_MAX_ACL_VIDS);
		for (i = 0; i < n; i++)
			ani_leaf(m, b, vid[i], EGR_DONTCARE);
	}
}

static int bridge_unis(uint16_t bridge_id, struct mc_uni *u, int max)
{
	struct omci_inst *bp;
	int n = 0;

	list_for_each_entry(bp, &omci_mib, list) {
		const char *d;

		if (bp->def->class_id != 47 || mib_get_uint(bp, 1) != bridge_id ||
		    mib_get_uint(bp, 3) != TP_PPTP_ETH_UNI)
			continue;
		d = eth_uni_dev(mib_get_uint(bp, 4));
		if (d && n < max) {
			u[n].bp = bp;
			u[n++].dev = d;
		}
	}

	return n;
}

/* the multicast destination MAC of IPv4 (01:00:5e) */
#define MC_MAC_MATCH	"dst_mac 01:00:5e/24"

/*
 * The tag the downstream multicast leaves the ANI side with, control 2 / 3 /
 * 4 of ME 309 attribute 16 (xpon_downstream_vlan_handle(),
 * xpon_igmp_core.c:738-806): 2 adds the TCI, 3 replaces the VID and the
 * priority, 4 the VID alone.  The DEI (bit 12) is not in a tc action, the
 * frame is matched untagged for 2 (a tag is pushed) and as multicast for 3
 * and 4 ("vlan modify" does nothing to an untagged frame,
 * act_vlan.c:54-58).
 */
static bool down_tci_rule(struct eth_model *m, struct omci_inst *prof)
{
	const uint8_t *ds = mib_val(prof, MOP_DOWN_TCI);
	uint16_t tci = (ds[1] << 8) | ds[2];
	char act[64];

	if (!(tci & 0xfff) || (tci & 0xfff) >= VID_ANY - 1) {
		INFO("eth: multicast: downstream TCI 0x%04x of profile 0x%04x: "
		     "no VLAN, not applied", tci, prof->id);
		return false;
	}

	switch (ds[0]) {
	case MC_DOWNTAG_ADD_TCI:
		snprintf(act, sizeof(act), "vlan push id %u priority %u protocol 802.1Q",
			 tci & 0xfff, tci >> 13 & 7);
		return eth_tc_add(m, GPON_NETDEV, false, "all",
				  "num_of_vlans 0 " MC_MAC_MATCH, act) ? false : true;
	case MC_DOWNTAG_REPLACE_TCI:
		snprintf(act, sizeof(act), "vlan modify id %u priority %u protocol 802.1Q",
			 tci & 0xfff, tci >> 13 & 7);
		break;
	default:
		snprintf(act, sizeof(act), "vlan modify id %u protocol 802.1Q",
			 tci & 0xfff);
		break;
	}

	return eth_tc_add(m, GPON_NETDEV, false, "802.1Q", MC_MAC_MATCH,
			  act) ? false : true;
}

/*
 * The IGMP of the UNI: the ingress of the UNI device, before the VLAN rows
 * of the port (the filters are added in order).
 *   rate     conform continues with the next filter, so ME 171 still applies;
 *            the IGMP module applies the rate limit of a UNI before it looks
 *            at the protocol (xpon_pass_rate_limit(),
 *            xpon_igmp_core.c:3400-3447, called by xpon_uni_incoming_handle()
 *            at 3450-3457), so an MLD message counts too: the reports go to
 *            224.0.0.22 (types 131, 132 and 143 - the ones
 *            xpon_igmp_core.c:2155-2158 walks), a query (type 130, sent by a
 *            router) does not.
 *   tagging  with tag control 1..3 the IGMP is not for the VLAN rows (the
 *            vendor does it in the multicast module, xpon_igmp_itf.c:
 *            226-245): it passes as it is, a tag the host put on it is
 *            removed first by control 2 / 3, the VLAN device of the
 *            multicast VLAN adds the new one (uni_apply()).  The priority
 *            of a tagged message is not kept by control 3.
 * The downstream multicast TCI control 2 / 3 / 4: the ingress of pon0, the
 * raw ANI device every multicast frame of the bridge passes, before the
 * bridge and before the ME 171 rules that eth_vlan.c adds later.
 */
void eth_mcast_pre(struct eth_model *m, struct omci_inst *br)
{
	struct mc_uni u[ETH_MAX_PORTS];
	int n = bridge_unis(br->id, u, ETH_MAX_PORTS), i;
	bool down = false;

	for (i = 0; i < n; i++) {
		struct omci_inst *prof = mc_profile(port_subscriber(br->id, u[i].bp));
		const uint8_t *ds = prof ? mib_val(prof, MOP_DOWN_TCI) : NULL;
		uint32_t rate = prof ? mib_get_uint(prof, MOP_UP_RATE) : 0;
		char act[96];

		if (!prof)
			continue;
		if (rate) {
			if (rate > 1000000)
				rate = 1000000;
			snprintf(act, sizeof(act), "police pkts_rate %u "
				 "pkts_burst %u conform-exceed drop/continue",
				 rate, rate);
			eth_tc_add(m, u[i].dev, false, "ip", "ip_proto 2", act);
			eth_tc_add(m, u[i].dev, false, "802.1Q",
				   "vlan_ethtype ip ip_proto 2", act);
			eth_tc_add(m, u[i].dev, false, "ipv6",
				   "ip_proto icmpv6 dst_ip ff02::16", act);
			eth_tc_add(m, u[i].dev, false, "802.1Q",
				   "vlan_ethtype ipv6 ip_proto icmpv6 dst_ip ff02::16",
				   act);
		}
		if (up_vid(prof)) {
			eth_tc_add(m, u[i].dev, false, "ip", "ip_proto 2", "pass");
			eth_tc_add(m, u[i].dev, false, "802.1Q",
				   "vlan_ethtype ip ip_proto 2",
				   mib_get_uint(prof, MOP_UP_TAG) == MC_UPTAG_ADD_TCI ?
				   "pass" : "vlan pop");
		}
		/* the first profile that wants a downstream tag of its own */
		if (!down && ds && ds[0] >= MC_DOWNTAG_ADD_TCI &&
		    ds[0] <= MC_DOWNTAG_REPLACE_VID) {
			down = down_tci_rule(m, prof);
		}
	}
}

/* the L3 interface of a router side port that faces the multicast VLAN */
static void proxy_mark(struct eth_model *m, uint16_t cls, uint16_t me,
		       uint16_t mvid)
{
	struct eth_l3 *pick = NULL;
	char suffix[16];
	int i;

	snprintf(suffix, sizeof(suffix), ".%u", mvid);
	for (i = 0; i < m->nl3; i++) {
		struct eth_l3 *l = &m->l3[i];
		size_t dl = strlen(l->dev), sl = strlen(suffix);

		if (l->class_id != cls || l->me != me)
			continue;
		if (!pick)
			pick = l;
		if (mvid && dl >= sl && !strcmp(l->dev + dl - sl, suffix)) {
			pick = l;
			break;
		}
	}
	if (pick)
		pick->mc_proxy = true;
}

void eth_mcast_bridge(struct eth_model *m, struct eth_bridge *b)
{
	struct mc_uni u[ETH_MAX_PORTS];
	struct omci_inst *bp, *first = NULL;
	int n = bridge_unis(b->id, u, ETH_MAX_PORTS), i;

	/* the UNI sides: their subscriber, the bridge-wide settings */
	for (i = 0; i < n; i++) {
		struct omci_inst *sub = port_subscriber(b->id, u[i].bp);
		struct omci_inst *prof = mc_profile(sub);

		if (!prof)
			continue;
		if (!first) {
			first = prof;
			bridge_params(b, prof);
		}
		uni_apply(m, b, u[i].dev, sub, prof);
	}

	list_for_each_entry(bp, &omci_mib, list) {
		uint8_t type;
		struct omci_inst *prof;

		if (bp->def->class_id != 47 || mib_get_uint(bp, 1) != b->id)
			continue;
		type = mib_get_uint(bp, 3);
		prof = mc_profile(mc_subscriber(MC_TYPE_BRIDGE_PORT, bp->id));
		if (!prof)
			continue;
		/* the router side: omcproxy, only without a UNI to share the bridge */
		if ((type == TP_VEIP || type == TP_IP_HOST) && cfg.mcast_downlink[0] &&
		    mib_get_uint(prof, MOP_FUNCTION) == MC_FUNC_PROXY && !n) {
			const uint8_t *ds = mib_val(prof, MOP_DOWN_TCI);
			uint16_t vid[MC_MAX_ACL_VIDS] = { 0 };

			if (ds && (((ds[1] << 8) | ds[2]) & 0xfff))
				vid[0] = ((ds[1] << 8) | ds[2]) & 0xfff;
			else
				acl_vids(prof, vid, MC_MAX_ACL_VIDS);
			proxy_mark(m, type == TP_VEIP ? CLASS_VEIP : CLASS_IP_HOST,
				   mib_get_uint(bp, 4), vid[0]);
			snprintf(m->mc_downlink, sizeof(m->mc_downlink), "%s",
				 cfg.mcast_downlink);
		}
	}
}

/* ---------------------------------------------------------------------
 * UCI: /etc/config/omcproxy, /etc/config/gpon
 * ------------------------------------------------------------------- */

static bool proxy_in(const struct eth_model *m, const char *name)
{
	int i;

	for (i = 0; i < m->nl3; i++)
		if (m->l3[i].mc_proxy && !strcmp(m->l3[i].name, name))
			return true;

	return false;
}

static int proxy_count(const struct eth_model *m)
{
	int i, n = 0;

	for (i = 0; i < m->nl3; i++)
		n += m->l3[i].mc_proxy;

	return n;
}

bool eth_mcast_proxy_changed(const struct eth_model *a, const struct eth_model *b)
{
	int i;

	if (proxy_count(a) != proxy_count(b) ||
	    (proxy_count(b) && strcmp(a->mc_downlink, b->mc_downlink)))
		return true;
	for (i = 0; i < b->nl3; i++)
		if (b->l3[i].mc_proxy && !proxy_in(a, b->l3[i].name))
			return true;

	return false;
}

/* is there an omcproxy to configure; logs once when there is none */
bool eth_mcast_proxy_present(const struct eth_model *m)
{
	static bool warned;

	if (!access(UCI_CONFDIR "/" OMCP_PKG, F_OK))
		return true;
	if (proxy_count(m) && !warned) {
		warned = true;
		WARN("eth: no /etc/config/omcproxy: the IGMP proxy function is "
		     "not applied");
	}

	return false;
}

/* section names in /etc/config/omcproxy: gpon_mc_<l3 name> */
static int proxy_section(struct uci_context *ctx, const struct eth_model *m,
			 const struct eth_l3 *l)
{
	char sn[48], tmp[sizeof(m->mc_downlink)], *tok, *save;
	int ret;

	snprintf(sn, sizeof(sn), "gpon_mc_%s", l->name);
	ret = persist_set(ctx, OMCP_PKG, sn, NULL, "proxy");
	ret = ret ? ret : persist_comment(ctx, OMCP_PKG, sn,
					  "gpond me %u 0x%04x multicast proxy",
					  l->class_id, l->me);
	ret = ret ? ret : persist_set(ctx, OMCP_PKG, sn, "uplink", l->name);
	snprintf(tmp, sizeof(tmp), "%s", m->mc_downlink);
	for (tok = strtok_r(tmp, " ", &save); tok && !ret;
	     tok = strtok_r(NULL, " ", &save))
		ret = persist_add_list(ctx, OMCP_PKG, sn, "downlink", tok);

	return ret;
}

/* what omcproxy does is the owned "proxy" sections; the rest is not ours */
int eth_mcast_proxy_write(const struct eth_model *m, bool *changed)
{
	struct uci_package *pkg;
	struct uci_context *ctx;
	struct uci_element *e, *tmp;
	char *before, *after;
	bool ch;
	int i, ret = 0;

	ctx = persist_open(OMCP_PKG, &pkg);
	if (!ctx)
		return -EIO;
	before = persist_snapshot(ctx, pkg);
	uci_foreach_element_safe(&pkg->sections, tmp, e) {
		struct uci_section *s = uci_to_section(e);

		if (persist_ours(ctx, s))
			persist_set(ctx, OMCP_PKG, s->e.name, NULL, NULL);
	}
	for (i = 0; i < m->nl3 && !ret; i++)
		if (m->l3[i].mc_proxy)
			ret = proxy_section(ctx, m, &m->l3[i]);

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

/*
 * The groups the static access control rows (ME 309 attribute 8) of the
 * profile name: the procd service xpon-mcast keeps them in the bridge MDB of
 * the port as permanent entries, so the multicast of a static row reaches the
 * port without a report (acl_collect()).
 */
#define MC_MAX_SGRP	16

static int sgroup_list(struct omci_inst *prof, uint8_t grp[MC_MAX_SGRP][4])
{
	const uint8_t *tab = mib_val(prof, MOP_STATIC_ACL);
	unsigned int o;
	int n = 0;

	for (o = 0; tab && n < MC_MAX_SGRP &&
	     o + MC_ACL_LEN <= prof->len[MOP_STATIC_ACL]; o += MC_ACL_LEN) {
		struct mc_acl a;

		mc_acl_decode(tab + o, &a);
		if (mc_acl_single(&a))
			memcpy(grp[n++], a.start, 4);
	}

	return n;
}

/* per port limits, ACL and static groups for the service xpon-mcast:
 * "mport" sections */
int eth_mcast_gpon_uci(struct uci_context *ctx, const struct eth_model *m)
{
	char sn[64], val[32], r[40];
	int i, j, ret = 0;

	for (i = 0; i < m->nbr && !ret; i++) {
		struct mc_uni u[ETH_MAX_PORTS];
		int n = bridge_unis(m->br[i].id, u, ETH_MAX_PORTS);

		for (j = 0; j < m->br[i].nport && !ret; j++) {
			const struct eth_port *p = &m->br[i].port[j];
			uint8_t sgrp[MC_MAX_SGRP][4];
			struct omci_inst *prof = NULL;
			int ns, k;
			size_t l;

			/* the profile of the UNI this port carries */
			for (k = 0; k < n; k++)
				if (uni_port_of(m, p, u[k].dev))
					prof = mc_profile(port_subscriber(m->br[i].id,
									    u[k].bp));
			ns = prof ? sgroup_list(prof, sgrp) : 0;
			if (!p->max_groups && !p->nacl && !ns)
				continue;
			snprintf(sn, sizeof(sn), "mp_%s", p->dev);
			for (l = 0; sn[l]; l++)
				if (sn[l] == '.' || sn[l] == '-')
					sn[l] = '_';
			ret = persist_set(ctx, GPOND_PKG, sn, NULL, "mport");
			ret = ret ? ret : persist_comment(ctx, GPOND_PKG, sn,
							  "gpond me 45 0x%04x port %s",
							  m->br[i].id, p->dev);
			ret = ret ? ret : persist_set(ctx, GPOND_PKG, sn, "device",
						      p->dev);
			if (p->max_groups) {
				snprintf(val, sizeof(val), "%u", p->max_groups);
				ret = ret ? ret : persist_set(ctx, GPOND_PKG, sn,
							      "max_groups", val);
			}
			/* the bridge the static groups go into */
			if (ns) {
				snprintf(val, sizeof(val), "%s", m->br[i].name);
				ret = ret ? ret : persist_set(ctx, GPOND_PKG, sn,
							      "bridge", val);
			}
			for (k = 0; k < p->nacl && !ret; k++) {
				const uint8_t *s = p->acl[k].start, *e = p->acl[k].end;

				snprintf(r, sizeof(r), "%u.%u.%u.%u-%u.%u.%u.%u",
					 s[0], s[1], s[2], s[3],
					 e[0], e[1], e[2], e[3]);
				ret = persist_add_list(ctx, GPOND_PKG, sn, "acl", r);
			}
			for (k = 0; k < ns && !ret; k++) {
				snprintf(r, sizeof(r), "%u.%u.%u.%u",
					 sgrp[k][0], sgrp[k][1], sgrp[k][2],
					 sgrp[k][3]);
				ret = persist_add_list(ctx, GPOND_PKG, sn, "sgrp", r);
			}
		}
	}

	return ret;
}

/* ---------------------------------------------------------------------
 * what is stored only
 * ------------------------------------------------------------------- */

#define NOTE_MAX	16
#define NOTE_LEN	96

static int rows(struct omci_inst *in, uint8_t idx, unsigned int len)
{
	return in->len[idx] / len;
}

/* the dynamic rows with an imputed bandwidth that do not fit the limit */
static int bw_dropped(struct omci_inst *prof, struct omci_inst *sub)
{
	const uint8_t *tab = mib_val(prof, MOP_DYN_ACL);
	uint32_t used = 0;
	unsigned int o;
	int n = 0;

	for (o = 0; tab && o + MC_ACL_LEN <= prof->len[MOP_DYN_ACL];
	     o += MC_ACL_LEN) {
		struct mc_acl a;

		mc_acl_decode(tab + o, &a);
		if (memcmp(a.start, a.end, 4) > 0 || !a.bandwidth)
			continue;
		if (bw_fits(sub, used, &a))
			used += a.bandwidth;
		else
			n++;
	}

	return n;
}

/*
 * Every setting of a subscriber and its profile that Linux does not carry
 * out, as a short text.
 */
static int notes(struct omci_inst *sub, struct omci_inst *prof,
		 const struct eth_model *m, char n[NOTE_MAX][NOTE_LEN])
{
	int c = 0, v, i, single = 0;
	uint16_t tci;

#define NOTE(...)	do { if (c < NOTE_MAX) snprintf(n[c++], NOTE_LEN, __VA_ARGS__); } while (0)
	v = mib_get_uint(prof, MOP_FUNCTION);
	if (v == MC_FUNC_PROXY) {
		for (i = 0; i < m->nl3 && !m->l3[i].mc_proxy; i++)
			;
		if (i == m->nl3)
			NOTE("function proxy: snooping only (needs a UNI-less VEIP/IP host and mcast_downlink)");
	}
	NOTE("igmp version %u: not applied (the IGMP module of the board enables every version)",
	     (unsigned)mib_get_uint(prof, MOP_VERSION));
	if (mib_get_uint(prof, MOP_QUERIER_IP))
		NOTE("querier ip: not applied (the bridge querier is off, the OLT queries)");
	v = mib_get_uint(prof, MOP_UP_TAG);
	tci = mib_get_uint(prof, MOP_UP_TCI);
	if (v > MC_UPTAG_REPLACE_VID)
		NOTE("upstream tag control %d: stored only", v);
	else if (v && !(tci & 0xfff))
		NOTE("upstream tag control %d with VID 0: not applied", v);
	if (mib_get_uint(prof, MOP_UP_RATE))
		NOTE("upstream igmp rate: policed on the IGMP and the MLD reports, not on the other multicast");
	v = rows(prof, MOP_DYN_ACL, MC_ACL_LEN);
	if (v)
		NOTE("dynamic access control: %d row(s), IGMP v1/v2 reports and single record v3 reports checked by group; source, VLAN, GEM, MLD not checked", v);
	v = acl_rows(prof, MOP_STATIC_ACL, &single);
	if (single)
		NOTE("static access control: %d group(s) added to the bridge MDB as permanent members (a report of a host for one is dropped); VLAN, source and GEM not checked", single);
	if (v > single)
		NOTE("static access control: %d row(s) with a range of groups not enforced", v - single);
	if (mib_get_uint(prof, MOP_UNAUTHORIZED))
		NOTE("unauthorized join request: not applied (nothing in the IGMP module reads it)");
	v = rows(prof, MOP_LOST_GROUPS, MC_LOST_ROW_LEN);
	if (v)
		NOTE("lost groups list table: %d row(s) stored, nothing produces it", v);
	v = mib_val(prof, MOP_DOWN_TCI) ? mib_val(prof, MOP_DOWN_TCI)[0] : 0;
	tci = mib_val(prof, MOP_DOWN_TCI) ?
		((mib_val(prof, MOP_DOWN_TCI)[1] << 8) | mib_val(prof, MOP_DOWN_TCI)[2]) : 0;
	if (v > MC_DOWNTAG_REPLACE_VID)
		NOTE("downstream tag control %d: not applied (the IGMP module takes 0..4)", v);
	else if (v >= MC_DOWNTAG_ADD_TCI && !(tci & 0xfff))
		NOTE("downstream tag control %d with VID 0: not applied", v);
	else if (v >= MC_DOWNTAG_ADD_TCI)
		NOTE("downstream tag control %d: a tc rule on pon0, a VLAN device of its own rewrites the tag again", v);
	v = rows(sub, MSC_SERVICE_PACKAGE, 20);
	if (v > 1)
		NOTE("service package table: %d row(s), first active profile applied", v);
	v = rows(sub, MSC_PREVIEW_GROUPS, 22);
	if (v)
		NOTE("preview groups table: %d row(s) stored only", v);
	v = mc_max_bw(sub);
	if (v && mib_get_uint(sub, MSC_BW_ENFORCEMENT)) {
		i = bw_dropped(prof, sub);
		NOTE("max multicast bandwidth %u: %d dynamic range(s) with a bandwidth do not fit, a report for one is refused",
		     (unsigned)v, i);
	} else if (v) {
		NOTE("max multicast bandwidth %u: accounted by the monitor only",
		     (unsigned)v);
	} else if (mib_get_uint(sub, MSC_BW_ENFORCEMENT)) {
		NOTE("bandwidth enforcement without a limit: nothing to enforce");
	}
#undef NOTE

	return c;
}

void eth_mcast_report(const struct eth_model *m)
{
	struct omci_inst *sub;
	char n[NOTE_MAX][NOTE_LEN];

	list_for_each_entry(sub, &omci_mib, list) {
		struct omci_inst *prof;
		int c, i;

		if (sub->def->class_id != CLASS_MC_SUBSCRIBER)
			continue;
		prof = mc_profile(sub);
		if (!prof)
			continue;
		c = notes(sub, prof, m, n);
		for (i = 0; i < c; i++)
			INFO("eth: multicast subscriber 0x%04x profile 0x%04x: %s",
			     sub->id, prof->id, n[i]);
	}
}

void eth_mcast_dump(struct blob_buf *b)
{
	const struct eth_model *m = eth_current();
	struct omci_inst *sub;
	char n[NOTE_MAX][NOTE_LEN];
	void *a, *t, *l;
	int i;

	a = blobmsg_open_array(b, "multicast");
	list_for_each_entry(sub, &omci_mib, list) {
		struct omci_inst *prof;
		int c, k;

		if (sub->def->class_id != CLASS_MC_SUBSCRIBER)
			continue;
		prof = mc_profile(sub);
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "me", sub->id);
		blobmsg_add_u32(b, "type", mib_get_uint(sub, MSC_ME_TYPE));
		blobmsg_add_u32(b, "profile", mc_profile_id(sub));
		blobmsg_add_u8(b, "profile_found", !!prof);
		l = blobmsg_open_array(b, "not_applied");
		c = prof ? notes(sub, prof, m, n) : 0;
		for (k = 0; k < c; k++)
			blobmsg_add_string(b, NULL, n[k]);
		blobmsg_close_array(b, l);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);

	a = blobmsg_open_array(b, "mcast_proxy");
	for (i = 0; i < m->nl3; i++)
		if (m->l3[i].mc_proxy)
			blobmsg_add_string(b, NULL, m->l3[i].name);
	blobmsg_close_array(b, a);
	mcast_mon_dump(b);
}
