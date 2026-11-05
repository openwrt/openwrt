// SPDX-License-Identifier: GPL-2.0
/*
 * Extended VLAN tagging operation configuration data (ME 171), VLAN
 * tagging filter data (ME 84) -> Linux.
 *
 * Table row layout and code points follow the SDK (gpon-en757x/xpon,
 * src/omci/omci_me/omci_me_vlan_func.c, setExtVlanTagRuleByPkt() 973-1428,
 * constants in inc/omci/omci_me/omci_me_vlan.h:243-401):
 *   word 1 (bytes 0..3)   filter outer: priority bits 31..28, VID 27..15,
 *                         TPID/DEI 14..12
 *   word 2 (bytes 4..7)   filter inner: priority, VID, TPID/DEI as above,
 *                         bits 3..0 ethertype filter
 *   word 3 (bytes 8..11)  treatment: tags to remove bits 31..30, outer
 *                         priority 19..16, VID 15..3, TPID/DEI 2..0
 *   word 4 (bytes 12..15) treatment inner: priority 19..16, VID 15..3,
 *                         TPID/DEI 2..0
 * The SDK accepts (anything else makes the Set fail, result 9):
 *   filter priority 0..8 (8 any), 14 (default row), 15 (no tag); VID 0..4094
 *   or 4096 (any); TPID/DEI 0 (any), 4 (0x8100), 5 (input TPID), 6/7 (input
 *   TPID, DEI 0/1); ethertype 0..4 (any, IPoE 0x0800, PPPoE 0x8863/0x8864,
 *   ARP, IPv6); tags to remove 0..3 (3: discard); treatment priority 0..7,
 *   8/9 copy from the inner/outer tag, 10 derive from DSCP, 15 no tag; VID
 *   0..4094, 4096/4097 copy from the inner/outer tag; TPID/DEI 0 copy from
 *   the inner tag, 1 copy from the outer tag, 2/3 output TPID with DEI from
 *   the inner/outer tag, 4 0x8100, 6/7 output TPID with DEI 0/1 (5
 *   reserved).  The tags of the received frame follow from the filter
 *   priorities: outer != 15 two tags, else inner != 15 one tag, else none;
 *   the operation is the combination of "remove" and the tags added
 *   (setExtVlanTagRuleByPkt() 1141-1147, 1330-1405).  A row whose last
 *   eight bytes are all ones deletes the row with that filter.
 *
 * Linux translation, per row:
 *   1. netifd devices: the VLAN devices ("8021q"/"8021ad" device types,
 *      netifd vlandev.c:25-36) of the tags the UNI side strips and the ANI
 *      side adds make the pair <uni>[.vid..] / pon0[.vid..] bridge ports.  A
 *      VLAN device does the inverse operation downstream by itself.  P-bits
 *      go through skb->priority: ingress_qos_mapping of the receiving
 *      device, egress_qos_mapping of the transmitting one (vlandev.c:35-36);
 *      fixed P-bit = every priority mapped to it, copy = identity maps,
 *      kept tags = identity maps.  DSCP -> P-bit (treatment priority 10):
 *      tc flower ip_tos + skbedit priority on the egress of the leaf device,
 *      before its egress map writes the tag.
 *   2. tc flower on the UNI device ingress (installed by the procd service
 *      xpon-vlan): protocol 802.1Q/802.1ad, vlan_id/vlan_prio,
 *      cvlan_id/cvlan_prio, vlan_ethtype/cvlan_ethtype, action vlan
 *      pop/push/modify (kernel act_vlan.c:40-98).  Used for what devices
 *      cannot express: P-bit and ethertype filters, VID 0, removing a tag
 *      of any VID.  Downstream mode 0 (inverse) is installed as the
 *      inverse filter on the ingress of pon0.
 *   3. the router side (VEIP, IP host: no UNI device): the tagged frames the
 *      router sends on pon0 (it has VLAN devices of its own on it) are
 *      tc rows on the egress of pon0 with the same filters and actions,
 *      their inverse the ingress of pon0 (downstream mode 0), as for a
 *      UNI; the untagged frames of the router are the devices, as above.
 *      Not when the router shares a bridge with a UNI (the bridge sends
 *      the UNIs' tagged frames through pon0 as well).
 *   4. discard rows (remove 3, SDK OP_1 "block", 1400): tc drop with the
 *      filter of the row on the UNI ingress, any VID, any ethertype, all
 *      the frames of a tag count (default rows, filter priority 14, the
 *      SDK gives them the low rule priority) included.  The rows are
 *      added to tc in row order: more tags before fewer, specific before
 *      general, default rows last, and the rows of tagged frames before
 *      those of untagged ones.  A discard of every VID makes the whole
 *      port tc, as does a removed tag of any VID: tc sees the frame
 *      before the VLAN devices do, so every row of the port has to be
 *      there to take its frames before the drop.  Untagged frames of any
 *      ethertype are matched by "protocol all", which tagged frames match
 *      too: pass rules for 802.1Q/802.1ad stand in front of them.
 *   Not expressible (the row is refused, result 9): DEI filters/DEI 1
 *   treatment (neither devices nor flower know DEI), TPIDs other than
 *   0x8100/0x88a8, P-bit/VID copies from a tag without a fixed VID, a
 *   DSCP P-bit in a tc row.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpond.h"
#include "eth.h"

/* ME 171 attributes */
#define EV_ASSOC		1
#define EV_TPID_IN		3
#define EV_TPID_OUT		4
#define EV_DOWN			5
#define EV_TABLE		6
#define EV_DSCP			8

/* ME 78 attributes */
#define VT_UP			1
#define VT_TCI			2
#define VT_DOWN			3
#define VT_ASSOC		4
#define VT_PTR			5

/* filter priority code points */
#define PRIO_ANY		8
#define PRIO_DEFAULT		14
#define PRIO_NONE		15

/* treatment code points */
#define T_PRIO_COPY_INNER	8
#define T_PRIO_COPY_OUTER	9
#define T_PRIO_DSCP		10
#define T_VID_COPY_INNER	4096
#define T_VID_COPY_OUTER	4097

/* ---------------------------------------------------------------------
 * row decoding
 * ------------------------------------------------------------------- */

static bool in_set(unsigned int v, const unsigned int *set, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		if (set[i] == v)
			return true;

	return false;
}

#define IN_SET(_v, ...) ({						\
	static const unsigned int _s[] = { __VA_ARGS__ };		\
	in_set((_v), _s, ARRAY_SIZE(_s));				\
})

/* 0 or -1: a row the SDK refuses */
static int xv_decode(const uint8_t *r, struct xv_row *v)
{
	uint32_t w1 = rd_be32(r), w2 = rd_be32(r + 4), w3 = rd_be32(r + 8);
	uint32_t w4 = rd_be32(r + 12);
	int i;

	memset(v, 0, sizeof(*v));
	v->f_prio[0] = w1 >> 28;
	v->f_vid[0] = (w1 >> 15) & 0x1fff;
	v->f_tpid[0] = (w1 >> 12) & 7;
	v->f_prio[1] = w2 >> 28;
	v->f_vid[1] = (w2 >> 15) & 0x1fff;
	v->f_tpid[1] = (w2 >> 12) & 7;
	v->ethertype = w2 & 0xf;

	for (i = 0; i < 2; i++) {
		if (v->f_prio[i] > PRIO_ANY && v->f_prio[i] != PRIO_DEFAULT &&
		    v->f_prio[i] != PRIO_NONE)
			return -1;
		if (v->f_vid[i] > 4094 && v->f_vid[i] != VID_ANY)
			return -1;
		if (!IN_SET(v->f_tpid[i], 0, 4, 5, 6, 7))
			return -1;
	}
	if (v->ethertype > 4)
		return -1;

	v->ntags = v->f_prio[0] != PRIO_NONE ? 2 :
		   v->f_prio[1] != PRIO_NONE ? 1 : 0;
	v->def = v->f_prio[0] == PRIO_DEFAULT || v->f_prio[1] == PRIO_DEFAULT;

	/* "the OLT deletes a table entry by setting all of its last eight
	 * bytes to 0xFF": nothing else is looked at */
	v->del = true;
	for (i = 8; i < XV_ROW_LEN; i++)
		if (r[i] != 0xff)
			v->del = false;
	if (v->del)
		return 0;

	v->remove = w3 >> 30;
	v->t_prio[0] = (w3 >> 16) & 0xf;
	v->t_vid[0] = (w3 >> 3) & 0x1fff;
	v->t_tpid[0] = w3 & 7;
	v->t_prio[1] = (w4 >> 16) & 0xf;
	v->t_vid[1] = (w4 >> 3) & 0x1fff;
	v->t_tpid[1] = w4 & 7;

	for (i = 0; i < 2; i++) {
		if (v->t_prio[i] > T_PRIO_DSCP && v->t_prio[i] != PRIO_NONE)
			return -1;
		if (v->t_vid[i] > 4094 && v->t_vid[i] != T_VID_COPY_INNER &&
		    v->t_vid[i] != T_VID_COPY_OUTER)
			return -1;
		if (!IN_SET(v->t_tpid[i], 0, 1, 2, 3, 4, 6, 7))
			return -1;
	}
	/* combinations of tags to remove and add the SDK has no operation
	 * for (1371-1405) */
	if (v->remove == 1 && v->t_prio[0] == PRIO_NONE &&
	    v->t_prio[1] != PRIO_NONE && v->ntags == 0)
		return -1;
	if (v->remove == 2 && v->t_prio[0] != PRIO_NONE &&
	    v->t_prio[1] == PRIO_NONE)
		return -1;

	return 0;
}

/* ---------------------------------------------------------------------
 * plan: the operation of a row, resolved
 * ------------------------------------------------------------------- */

static const char *tpid_name(uint16_t tpid)
{
	return tpid == TPID_8021AD ? "802.1ad" : "802.1Q";
}

static const char *plan_make(const struct xv_row *v, const struct ctx *c,
			     struct plan *p)
{
	bool outer = v->t_prio[0] != PRIO_NONE;
	bool inner = v->t_prio[1] != PRIO_NONE;
	int i, k;

	memset(p, 0, sizeof(*p));
	p->ntags = v->ntags;
	for (i = 0; i < p->ntags; i++) {
		int s = p->ntags == 2 ? i : 1;

		p->f[i].vid = v->f_vid[s] == VID_ANY ? -1 : v->f_vid[s];
		p->f[i].prio = v->f_prio[s] <= 7 ? v->f_prio[s] : -1;
		p->f[i].tpid = v->f_tpid[s] >= 5 ? c->tpid_in : TPID_8021Q;
		if (v->f_tpid[s] >= 6)
			p->dei = true;
		if (!i && !v->f_tpid[s])
			p->any_tpid = true;
	}

	if (v->remove == 3) {
		p->block = true;
		return NULL;
	}
	p->remove = v->remove;
	if (p->remove > p->ntags)
		return "removes more tags than the frame has";
	if (outer && !inner)
		return "outer tag treatment without an inner one";

	p->nadd = outer ? 2 : inner ? 1 : 0;
	for (k = 0; k < p->nadd; k++) {
		int s = outer ? k : 1;
		struct addt *a = &p->a[k];
		unsigned int vc = v->t_vid[s], pc = v->t_prio[s];

		a->vid_src = a->prio_src = -1;
		a->prio = -1;
		if (vc <= 4094) {
			a->vid = vc;
		} else {
			if (!p->ntags)
				return "VID copy without a received tag";
			a->vid = -1;
			a->vid_src = vc == T_VID_COPY_INNER ? p->ntags - 1 : 0;
		}
		if (pc <= 7) {
			a->prio = pc;
		} else if (pc == T_PRIO_DSCP) {
			a->dscp = true;
		} else {
			if (!p->ntags)
				return "P-bit copy without a received tag";
			a->prio_src = pc == T_PRIO_COPY_INNER ? p->ntags - 1 : 0;
		}
		switch (v->t_tpid[s]) {
		case 0:
			a->tpid = p->ntags ? p->f[p->ntags - 1].tpid : TPID_8021Q;
			break;
		case 1:
			a->tpid = p->ntags ? p->f[0].tpid : TPID_8021Q;
			break;
		case 2:
		case 3:
		case 6:
			a->tpid = c->tpid_out;
			break;
		case 7:
			return "treatment DEI 1";
		default:
			a->tpid = TPID_8021Q;
			break;
		}
	}

	return NULL;
}

/* ---------------------------------------------------------------------
 * netifd devices
 * ------------------------------------------------------------------- */

static const char *plan_device(struct ctx *c, const struct xv_row *v,
			       const struct plan *p, struct dev_res *r)
{
	struct eth_model *m = c->m;
	struct xv_add_tag A[4];
	int nu = 0, na = 0, i, k, copy_src = -1, nvlan0 = m->nvlan;
	bool copies = false, dscp = false, kept;
	const char *prev, *d, *why;
	int ing;

	if (v->ethertype)
		return "ethertype filter";
	if (p->dei)
		return "DEI filter";
	if (!c->uni && p->ntags)
		return "tagged frames from the router";
	for (i = 0; i < p->ntags; i++)
		if (p->f[i].prio >= 0)
			return "P-bit filter";
	/* a VLAN device sees one VID: a wildcard tag filter needs tc */
	for (i = 0; i < p->ntags; i++)
		if (p->f[i].vid < 0)
			return "wildcard VID filter";
	for (i = 0; i < p->ntags; i++) {
		if (!p->f[i].vid)
			break;
		nu++;
	}
	if (p->remove > nu)
		return "removed tag without a fixed VID";

	for (k = 0; k < p->nadd; k++) {
		const struct addt *a = &p->a[k];
		int vid = a->vid, eg;

		if (vid < 0) {
			if (a->vid_src >= nu)
				return "copied VID is not fixed";
			vid = p->f[a->vid_src].vid;
		}
		if (vid == 0)
			return "VID 0";
		if (a->prio >= 0) {
			eg = a->prio ? a->prio : EGR_NONE;
		} else if (a->dscp) {
			eg = EGR_IDENT;
			dscp = true;
		} else {
			if (a->prio_src >= nu)
				return "copied P-bit is of a tag without fixed VID";
			if (copy_src >= 0 && copy_src != a->prio_src)
				return "P-bit copies from two tags";
			copy_src = a->prio_src;
			eg = EGR_IDENT;
			copies = true;
		}
		A[na].tpid = a->tpid;
		A[na].vid = vid;
		A[na++].eg = eg;
	}
	kept = p->remove < nu;
	for (i = p->remove; i < nu; i++) {
		A[na].tpid = p->f[i].tpid;
		A[na].vid = p->f[i].vid;
		A[na++].eg = EGR_IDENT;
	}
	/* one skb->priority: the innermost stripped tag wins */
	if (copies && nu == 2 && copy_src != 1)
		return "P-bit copy from the outer of two tags";
	if (dscp && (copies || kept))
		return "DSCP P-bit next to kept or copied P-bits";

	ing = nu && (copies || kept) ? ING_IDENT : ING_DONTCARE;
	prev = c->uni;
	why = "conflicting VLAN device settings or name too long";
	for (i = 0; c->uni && i < nu; i++) {
		d = eth_vlan_dev(m, prev, p->f[i].tpid, p->f[i].vid,
				 EGR_DONTCARE, ing);
		if (!d)
			goto fail;
		prev = d;
	}
	r->uni = prev;
	prev = GPON_NETDEV;
	for (i = 0; i < na; i++) {
		d = eth_vlan_dev(m, prev, A[i].tpid, A[i].vid, A[i].eg,
				 ING_DONTCARE);
		if (!d)
			goto fail;
		prev = d;
	}
	r->ani = prev;
	r->vid = na ? A[0].vid : 0;
	r->dscp = dscp;

	return NULL;
fail:
	m->nvlan = nvlan0;

	return why;
}

/* SDK convertDSCPMap2Pbit(), 321-349: 8 groups of 3 bytes, 8 entries of
 * 3 bits each, MSB first (ME 171 attribute 8, ME 130 attribute 11) */
void xv_dscp_decode(const uint8_t *map24, uint8_t pbit[ETH_DSCP_NUM])
{
	int d;

	for (d = 0; d < ETH_DSCP_NUM; d++) {
		const uint8_t *g = map24 + (d / 8) * 3;
		uint32_t u24 = g[0] << 16 | g[1] << 8 | g[2];

		pbit[d] = (u24 >> (21 - 3 * (d % 8))) & 7;
	}
}

/* DSCP -> priority on the leaf device */
static void dscp_rules(struct eth_model *m, const char *leaf,
		       const uint8_t *map)
{
	char match[32], action[32];
	struct eth_vlan *vd = NULL;
	uint8_t pbit[ETH_DSCP_NUM];
	int i, d;

	for (i = 0; i < m->nvlan; i++)
		if (!strcmp(m->vlan[i].name, leaf))
			vd = &m->vlan[i];
	if (!vd || vd->dscp || !map)
		return;
	vd->dscp = true;
	xv_dscp_decode(map, pbit);
	for (d = 0; d < ETH_DSCP_NUM; d++) {
		int p = pbit[d];

		if (!p)
			continue;
		snprintf(match, sizeof(match), "ip_tos 0x%02x/0xfc", d << 2);
		snprintf(action, sizeof(action), "skbedit priority %d", p);
		if (eth_tc_add(m, leaf, true, "ip", match, action) ||
		    eth_tc_add(m, leaf, true, "ipv6", match, action))
			return;
	}
}

/* ---------------------------------------------------------------------
 * tc flower
 * ------------------------------------------------------------------- */

static int app(char *buf, size_t n, const char *fmt, ...)
{
	size_t l = strlen(buf);
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsnprintf(buf + l, n - l, fmt, ap);
	va_end(ap);

	return r < 0 || (size_t)r >= n - l ? -1 : 0;
}

static const char *et_name(uint8_t et, int variant)
{
	switch (et) {
	case 1:
		return "ip";
	case 2:
		return variant ? "0x8864" : "0x8863";
	case 3:
		return "arp";
	case 4:
		return "ipv6";
	default:
		return NULL;
	}
}

/* match of the frames the row filters: UNI ingress */
static int tc_up_match(const struct plan *p, const struct xv_row *v, int variant,
		       char *proto, size_t pn, char *match, size_t mn)
{
	const char *et = et_name(v->ethertype, variant);
	int r = 0;

	match[0] = 0;
	if (!p->ntags) {
		snprintf(proto, pn, "%s", et ? et : "all");
		return 0;
	}
	snprintf(proto, pn, "%s", tpid_name(p->f[0].tpid));
	if (p->f[0].vid >= 0)
		r |= app(match, mn, "vlan_id %d ", p->f[0].vid);
	if (p->f[0].prio >= 0)
		r |= app(match, mn, "vlan_prio %d ", p->f[0].prio);
	if (p->ntags == 1) {
		if (et)
			r |= app(match, mn, "vlan_ethtype %s ", et);
		return r;
	}
	r |= app(match, mn, "vlan_ethtype %s ", tpid_name(p->f[1].tpid));
	if (p->f[1].vid >= 0)
		r |= app(match, mn, "cvlan_id %d ", p->f[1].vid);
	if (p->f[1].prio >= 0)
		r |= app(match, mn, "cvlan_prio %d ", p->f[1].prio);
	if (et)
		r |= app(match, mn, "cvlan_ethtype %s ", et);

	return r;
}

static const char *tc_up_actions(const struct plan *p, char *buf, size_t n)
{
	int r = 0, k, i;

	buf[0] = 0;
	if (p->block)
		return app(buf, n, "drop") ? "tc action too long" : NULL;
	if (!p->remove && !p->nadd)
		return app(buf, n, "pass") ? "tc action too long" : NULL;

	for (k = 0; k < p->nadd; k++)
		if (p->a[k].dscp)
			return "DSCP P-bit needs a VLAN device";

	if (p->remove == 1 && p->nadd == 1) {
		/* replace the outermost tag: modify, a missing priority keeps
		 * the P-bit of the tag it replaces (act_vlan.c:71-75) */
		const struct addt *a = &p->a[0];
		int vid = a->vid >= 0 ? a->vid : p->f[a->vid_src].vid;

		if (vid < 0)
			return "copied VID is not fixed";
		r |= app(buf, n, "vlan modify id %d ", vid);
		if (a->prio >= 0) {
			r |= app(buf, n, "priority %d ", a->prio);
		} else if (a->prio_src != 0) {
			if (p->f[a->prio_src].prio < 0)
				return "copied P-bit is not fixed";
			r |= app(buf, n, "priority %d ", p->f[a->prio_src].prio);
		}
		r |= app(buf, n, "protocol %s", tpid_name(a->tpid));
		return r ? "tc action too long" : NULL;
	}

	for (i = 0; i < p->remove; i++)
		r |= app(buf, n, "%svlan pop", i ? " action " : "");
	for (k = p->nadd - 1; k >= 0; k--) {
		const struct addt *a = &p->a[k];
		int vid = a->vid >= 0 ? a->vid : p->f[a->vid_src].vid;
		int prio = a->prio >= 0 ? a->prio : p->f[a->prio_src].prio;

		if (vid < 0)
			return "copied VID is not fixed";
		if (prio < 0)
			return "copied P-bit is not fixed";
		r |= app(buf, n, "%svlan push id %d priority %d protocol %s",
			 buf[0] ? " action " : "", vid, prio, tpid_name(a->tpid));
	}

	return r ? "tc action too long" : NULL;
}

/*
 * Downstream mode 0, the inverse operation, on the ingress of pon0: the
 * frames that carry the tags the row produced get the removed tags back
 * and lose the added ones.
 */
static const char *tc_down(struct ctx *c, const struct plan *p, int variant,
			   const struct xv_row *v)
{
	struct tag R[4];
	char proto[8], match[128] = "", action[192] = "";
	const char *et = et_name(v->ethertype, variant);
	int nr = 0, k, i, r = 0;

	if (p->block || (!p->remove && !p->nadd))
		return NULL;
	if (!p->nadd)
		return "tag removal without a replacement has no inverse";
	for (k = 0; k < p->nadd; k++) {
		const struct addt *a = &p->a[k];

		R[nr].tpid = a->tpid;
		R[nr].prio = -1;
		R[nr++].vid = a->vid >= 0 ? a->vid : p->f[a->vid_src].vid;
	}
	for (i = p->remove; i < p->ntags; i++)
		R[nr++] = p->f[i];
	if (R[0].vid < 0)
		return "result tag without a fixed VID";

	snprintf(proto, sizeof(proto), "%s", tpid_name(R[0].tpid));
	r |= app(match, sizeof(match), "vlan_id %d ", R[0].vid);
	if (nr > 1) {
		r |= app(match, sizeof(match), "vlan_ethtype %s ",
			 tpid_name(R[1].tpid));
		if (R[1].vid >= 0)
			r |= app(match, sizeof(match), "cvlan_id %d ", R[1].vid);
		if (et && nr == 2)
			r |= app(match, sizeof(match), "cvlan_ethtype %s ", et);
	} else if (et) {
		r |= app(match, sizeof(match), "vlan_ethtype %s ", et);
	}

	if (p->remove == 1 && p->nadd == 1) {
		const struct addt *a = &p->a[0];
		int aprio = a->prio >= 0 ? a->prio :
			    a->prio_src >= 0 ? p->f[a->prio_src].prio : -1;

		if (p->f[0].vid < 0)
			return "removed tag without a fixed VID";
		r |= app(action, sizeof(action), "vlan modify id %d ", p->f[0].vid);
		/*
		 * A row that kept the P-bit (copy of the outer tag, or the
		 * same fixed value) has a VID-only inverse that keeps it too:
		 * rows that differ only in the P-bit (35 p0/p5/p6 -> 1501)
		 * then give one rule instead of several where the first one
		 * forces its P-bit on all.  A row that rewrote the P-bit
		 * takes back only the frames with the P-bit it wrote.
		 */
		if (a->prio < 0 && a->prio_src == 0) {
			/* P-bit copied from the replaced tag */
		} else if (aprio >= 0 && aprio == p->f[0].prio) {
			/* same fixed P-bit both ways */
		} else if (p->f[0].prio >= 0) {
			if (aprio >= 0)
				r |= app(match, sizeof(match), "vlan_prio %d ",
					 aprio);
			r |= app(action, sizeof(action), "priority %d ",
				 p->f[0].prio);
		}
		r |= app(action, sizeof(action), "protocol %s", tpid_name(p->f[0].tpid));
	} else {
		for (i = 0; i < p->nadd; i++)
			r |= app(action, sizeof(action), "%svlan pop",
				 i ? " action " : "");
		for (i = p->remove - 1; i >= 0; i--) {
			if (p->f[i].vid < 0)
				return "removed tag without a fixed VID";
			r |= app(action, sizeof(action),
				 "%svlan push id %d priority %d protocol %s",
				 action[0] ? " action " : "", p->f[i].vid,
				 p->f[i].prio >= 0 ? p->f[i].prio : 0,
				 tpid_name(p->f[i].tpid));
		}
	}
	if (r)
		return "tc rule too long";

	return eth_tc_add(c->m, GPON_NETDEV, false, proto, match, action) ?
	       "too many tc rules" : NULL;
}

/*
 * Where the frames of the row come from: the ingress of the UNI device, or,
 * for the router side of a VEIP / IP host (no UNI), what the router sends
 * on pon0, the egress of pon0.
 */
static const char *up_dev(const struct ctx *c, bool *egress)
{
	*egress = !c->uni;

	return c->uni ? c->uni : GPON_NETDEV;
}

/*
 * A tc filter for untagged frames of any ethertype ("protocol all") matches
 * tagged frames too: the tagged ones the device rules take over later have
 * to get through first.  Added once, before the first such rule.
 */
static int up_guard(struct ctx *c, const char *dev, bool egress)
{
	if (c->guard)
		return 0;
	if (eth_tc_add(c->m, dev, egress, "802.1Q", "", "pass") ||
	    eth_tc_add(c->m, dev, egress, "802.1ad", "", "pass"))
		return -ENOSPC;
	c->guard = true;

	return 0;
}

static const char *plan_tc(struct ctx *c, const struct xv_row *v,
			   const struct plan *p)
{
	char proto[8], match[128], action[192];
	const char *why, *dev;
	int variant, nvar = v->ethertype == 2 ? 2 : 1;
	/* TPID code 0 filters any TPID: 802.1Q and 802.1ad both have to match */
	int tp, ntp = p->ntags && p->any_tpid ? 2 : 1;
	int ntc0 = c->m->ntc;
	bool guard0 = c->guard, egress;
	struct plan q;

	dev = up_dev(c, &egress);
	/*
	 * Keep it so: the PPE forwards the router's flows past the tc rules
	 * of pon0, and an7552_mac_ppe_upstream() takes an untagged flow to
	 * the raw pon0 as leaving untagged.  Only the tagged rows have their
	 * rewrite handed to the kernel (router_vid_map()).
	 */
	if (!c->uni && !p->ntags)
		return "untagged frames of the router are handled by devices";
	if (p->dei)
		return "DEI filter";
	why = tc_up_actions(p, action, sizeof(action));
	if (why)
		return why;

	if (!p->ntags && !et_name(v->ethertype, 0) && c->uni &&
	    up_guard(c, dev, egress))
		goto fail;
	for (tp = 0; tp < ntp; tp++) {
		q = *p;
		if (tp)
			q.f[0].tpid = TPID_8021AD;
		for (variant = 0; variant < nvar; variant++) {
			if (tc_up_match(&q, v, variant, proto, sizeof(proto),
					match, sizeof(match)))
				goto fail;
			if (eth_tc_add(c->m, dev, egress, proto, match, action))
				goto fail;
			if (c->inverse && !tp) {
				why = tc_down(c, p, variant, v);
				if (why)
					INFO("eth: ME171 downstream inverse skipped: %s",
					     why);
			}
		}
	}

	return NULL;
fail:
	c->m->ntc = ntc0;
	c->guard = guard0;

	return "too many tc rules";
}

/* ---------------------------------------------------------------------
 * rows of one ME 171
 * ------------------------------------------------------------------- */

/*
 * Most specific first, default rows last (G.988 9.3.13).  The SDK sends a
 * default row (filter priority 14, 1015-1023, 1086-1092) to a port of its
 * own, consulted after the rows of the port (1666-1675, 5725), and has
 * transparent default rules there for 0, 1 and 2 tags until the OLT
 * writes one (setExtVlanOpTBLDefaultRule(), 860-912, 5798-5803): frames
 * no row takes pass, as they do here when no default row discards them.
 * Rows for tagged frames come before the rows for untagged ones: a tc rule
 * for untagged frames of any ethertype also sees tagged ones, which the
 * tagged rows have to have taken first (up_guard()).
 */
static int row_key(const struct xv_row *v)
{
	int spec = 0, i;

	for (i = 0; i < 2; i++)
		spec += (v->f_vid[i] != VID_ANY && v->f_prio[i] != PRIO_NONE) +
			(v->f_prio[i] <= 7) + (v->f_tpid[i] != 0);
	spec += v->ethertype != 0;

	return (v->ntags ? 0 : 10000) + (v->def ? 1000 : 0) +
	       (2 - v->ntags) * 100 + (20 - spec);
}

/*
 * ME 78 VLAN tag operation: one TCI and a mode, translated into the same
 * rows as ME 171 (setVlanTagOpRuleValByTci(), omci_me_vlan_func.c:650-720):
 *   0 transparent, no row
 *   1 untagged: add the tag, tagged: change it (OP_40, remove + add)
 *   2 untagged and tagged: add the tag (OP_21)
 * The treatment slot of a single added tag is the inner one (ntags is 0 or
 * 1, so plan_make() reads slot 1).  TPID code 4 is 0x8100, the output TPID
 * the SDK sets (OMCI_VLAN_TAG_OP_RULE_ADD_TPID_1 with 0x8100); code 7 is
 * the same with DEI 1, which the tc vlan action cannot set.
 */
static int me78_rows(struct omci_inst *ev, struct vrow *rows)
{
	uint8_t mode = mib_get_uint(ev, VT_UP);
	uint16_t tci = mib_get_uint(ev, VT_TCI);
	uint8_t prio = (tci >> 13) & 7;
	uint16_t vid = tci & 0xfff;
	uint8_t tpid = (tci & 0x1000) ? 7 : 4;
	int nr = 0;

	if (mode != 1 && mode != 2)
		return 0;

	/*
	 * Tagged frames first: a tc rule for untagged frames of any
	 * ethertype also sees tagged ones, so up_guard() passes those
	 * through before it, and a pass action ends the classification.
	 * The tagged row has to be there before that point.
	 *
	 * Tagged frames: any VID, any P-bit, any outer TPID (0: 802.1Q and
	 * 802.1ad), one or more tags.  Mode 1 overwrites the outer TCI,
	 * mode 2 prepends a tag (Q-in-Q when the frame is tagged).
	 */
	memset(&rows[nr].v, 0, sizeof(rows[nr].v));
	rows[nr].v.f_prio[0] = PRIO_NONE;
	rows[nr].v.f_prio[1] = PRIO_ANY;
	rows[nr].v.f_vid[1] = VID_ANY;
	rows[nr].v.f_tpid[1] = 0;
	rows[nr].v.ntags = 1;
	rows[nr].v.remove = mode == 1 ? 1 : 0;
	rows[nr].v.t_prio[1] = prio;
	rows[nr].v.t_vid[1] = vid;
	rows[nr].v.t_tpid[1] = tpid;
	rows[nr].key = row_key(&rows[nr].v);
	nr++;

	/* untagged frames: add a tag */
	memset(&rows[nr].v, 0, sizeof(rows[nr].v));
	rows[nr].v.f_prio[0] = rows[nr].v.f_prio[1] = PRIO_NONE;
	rows[nr].v.t_prio[1] = prio;
	rows[nr].v.t_vid[1] = vid;
	rows[nr].v.t_tpid[1] = tpid;
	rows[nr].key = row_key(&rows[nr].v);
	nr++;

	return nr;
}

static int rows_get(struct omci_inst *ev, struct vrow *rows)
{
	const uint8_t *tab = ev ? mib_val(ev, EV_TABLE) : NULL;
	uint32_t n, len = ev ? ev->len[EV_TABLE] : 0;
	int nr = 0;

	if (ev && ev->def->class_id == CLASS_VLAN_TAG_OP)
		return me78_rows(ev, rows);

	for (n = 0; tab && n + XV_ROW_LEN <= len && nr < XV_MAX_ROWS;
	     n += XV_ROW_LEN) {
		struct vrow t;
		int i;

		if (xv_decode(tab + n, &t.v) || t.v.del)
			continue;
		t.key = row_key(&t.v);
		for (i = nr; i > 0 && rows[i - 1].key > t.key; i--)
			rows[i] = rows[i - 1];
		rows[i] = t;
		nr++;
	}

	return nr;
}

static uint16_t tpid_attr(struct omci_inst *ev, uint8_t idx)
{
	uint16_t t = ev ? mib_get_uint(ev, idx) : 0;

	return t == TPID_8021AD ? t : TPID_8021Q;
}

static void ctx_init(struct ctx *c, struct eth_model *m, const char *uni,
		     struct omci_inst *ev)
{
	memset(c, 0, sizeof(*c));
	c->m = m;
	c->uni = uni;
	if (ev && ev->def->class_id == CLASS_VLAN_TAG_OP) {
		/*
		 * ME 78: 0x8100 in and out (setVlanTagRuleByInst(), 1562-
		 * 1566), no inverse (its downstream mode is transparent or
		 * strip, never the inverse of the upstream operation) and no
		 * DSCP map.
		 */
		c->tpid_in = c->tpid_out = TPID_8021Q;
		return;
	}
	c->tpid_in = tpid_attr(ev, EV_TPID_IN);
	c->tpid_out = tpid_attr(ev, EV_TPID_OUT);
	/* SDK 5282-5285: 0 inverse of the upstream operation, 1 transparent */
	c->inverse = !ev || mib_get_uint(ev, EV_DOWN) == 0;
	c->dscp = ev && ev->len[EV_DSCP] >= 24 ? mib_val(ev, EV_DSCP) : NULL;
}

/*
 * Tag-handling a row cannot be expressed by devices and has no pinned
 * outer VID: tc has to take the whole port, because a tc filter on the
 * UNI device sees the frame before the VLAN device of a lower precedence
 * row would.
 */
static bool needs_tc_any(struct ctx *c, const struct vrow *rows, int nr)
{
	int i;

	for (i = 0; i < nr; i++) {
		const struct xv_row *v = &rows[i].v;
		struct plan p;
		struct dev_res r;
		int nvlan0 = c->m->nvlan;
		bool fail;

		if (plan_make(v, c, &p) || !p.ntags)
			continue;
		if (p.block) {
			/* a discard of tagged frames of any VID, or of all the
			 * rest, has to see them before the devices do */
			if (v->def || p.f[0].vid < 0)
				return true;
			continue;
		}
		fail = plan_device(c, v, &p, &r) != NULL;
		c->m->nvlan = nvlan0;
		if (fail && p.f[0].vid < 0)
			return true;
	}

	return false;
}

bool xv_filter_pass(const struct xv_filter *f, uint16_t vid)
{
	int i;

	if (!f)
		return true;
	if (!vid)
		return f->untagged_ok;
	if (!f->active || f->pbit)
		return true;
	for (i = 0; i < f->n; i++)
		if (f->vid[i] == vid)
			return true;

	return false;
}

static void leaf_add(struct xv_leaf *leaf, int *nleaf, int max, const char *dev,
		     uint16_t vid)
{
	int i;

	if (!leaf)
		return;
	for (i = 0; i < *nleaf; i++)
		if (!strcmp(leaf[i].dev, dev))
			return;
	if (*nleaf >= max)
		return;
	snprintf(leaf[*nleaf].dev, IFNAMSIZ, "%s", dev);
	leaf[(*nleaf)++].vid = vid;
}

/* a pair of leaves that are bridge ports / router side leaves */
static void leaves(struct eth_bridge *b, const char *uni_leaf, const char *ani,
		   uint16_t vid, const struct xv_filter *f, struct xv_leaf *leaf,
		   int *nleaf, int max)
{
	if (!xv_filter_pass(f, vid)) {
		DBG("eth: %s filtered out by VLAN tagging filter data", ani);
		return;
	}
	if (b) {
		if (uni_leaf)
			eth_add_port(b, uni_leaf);
		eth_add_port(b, ani);
		eth_bridge_ani(b, ani, vid);
	}
	leaf_add(leaf, nleaf, max, ani, vid);
}

/*
 * The outer VID of the frame after the treatment, 0 untagged, -1 not known
 * (a copy of a tag without a fixed VID): what ME 84 investigates.
 */
static int plan_out_vid(const struct plan *p)
{
	if (p->nadd) {
		const struct addt *a = &p->a[0];

		return a->vid >= 0 ? a->vid : p->f[a->vid_src].vid;
	}

	return p->remove < p->ntags ? p->f[p->remove].vid : 0;
}

/*
 * The VID rewrite of a router row for the PPE (eth_vid_map_add()), which
 * never runs the tc rules: one tag in, the same tag out with another VID
 * (vlan modify) or unchanged.  Rows of another shape give no entry; the
 * kernel then keeps that VID out of the PPE.
 */
static void router_vid_map(struct eth_model *m, const struct plan *p)
{
	const struct addt *a = &p->a[0];
	uint8_t pcp = ETH_PCP_KEEP, mask;
	int vid;

	if (p->ntags != 1 || p->f[0].vid <= 0)
		return;
	mask = p->f[0].prio >= 0 ? 1u << p->f[0].prio : 0;
	if (!p->remove && !p->nadd) {
		eth_vid_map_add(m, p->f[0].vid, mask, p->f[0].vid, pcp);
		return;
	}
	if (p->remove != 1 || p->nadd != 1 || a->dscp)
		return;
	vid = a->vid >= 0 ? a->vid : p->f[a->vid_src].vid;
	if (vid <= 0)
		return;
	/* one tag: a copied P-bit is the one of the tag it replaces */
	if (a->prio >= 0)
		pcp = a->prio;
	eth_vid_map_add(m, p->f[0].vid, mask, vid, pcp);
}

/* the router sends untagged frames only through the leaf devices: drop
 * what a discard row for an ethertype names on each of them (egress) */
static void router_discard(struct eth_model *m, const struct xv_leaf *leaf,
			   int nleaf, const uint8_t *ets, int nets)
{
	int i, k, variant;

	for (k = 0; k < nets; k++)
		for (variant = 0; variant < (ets[k] == 2 ? 2 : 1); variant++)
			for (i = 0; i < nleaf; i++) {
				const char *et = et_name(ets[k], variant);

				/* pon0 itself carries the bridges' frames too */
				if (!strcmp(leaf[i].dev, GPON_NETDEV)) {
					INFO("eth: ME171 discard of ethertype %s for "
					     "the router on %s skipped: shared "
					     "device", et, GPON_NETDEV);
					continue;
				}
				eth_tc_add(m, leaf[i].dev, true, et, "", "drop");
			}
}

/*
 * ME 78 downstream mode 1, "strip a tag" (the SDK maps the attribute value
 * 1 to its downstream mode 2, setVlanTagOpDownModeValue()): the outer tag of
 * the frames that leave the UNI is dropped, whatever its VID.  A UNI is a
 * device of its own, so an egress tc action on it does exactly that; a VLAN
 * device path for the same port would only strip the VID it was built for.
 * The router side (no UNI device) has no per-port device to hook: its
 * downstream leaf is the VLAN device of the untagged row (or of the ME 84
 * filter), which pops the tag by itself.
 */
static void me78_strip(struct eth_model *m, struct omci_inst *ev,
		       const char *uni)
{
	if (!uni || !ev || ev->def->class_id != CLASS_VLAN_TAG_OP ||
	    mib_get_uint(ev, VT_DOWN) != 1)
		return;

	eth_tc_add(m, uni, true, "802.1Q", "", "vlan pop");
	eth_tc_add(m, uni, true, "802.1ad", "", "vlan pop");
}

static void run_rows(struct eth_model *m, struct eth_bridge *b, const char *uni,
		     struct omci_inst *ev, const struct xv_filter *flt,
		     struct xv_leaf *leaf, int *nleaf, int max, bool bridged)
{
	static struct vrow rows[XV_MAX_ROWS];
	uint8_t rdis[4];
	struct ctx c;
	int nr, i, nrdis = 0;

	ctx_init(&c, m, uni, ev);
	nr = rows_get(ev, rows);
	c.force_tc = uni && needs_tc_any(&c, rows, nr);

	for (i = 0; i < nr; i++) {
		const struct xv_row *v = &rows[i].v;
		struct plan p;
		struct dev_res r;
		const char *why = plan_make(v, &c, &p);
		int ovid, k;

		if (why)
			goto skip;
		if (p.block) {
			/*
			 * Discard (treatment "remove 3 tags", SDK 1400 OP_1
			 * block): a tc drop of the frames of the row on the
			 * UNI ingress, in the row order (specific rows first,
			 * default rows last, the rows of tagged frames before
			 * those of untagged ones).  A discard that sees
			 * every VID is why the whole port is tc (force_tc):
			 * the rows before it have taken what they want by
			 * then.  The router: tagged frames it sends on pon0
			 * (tc egress, like the other rows of it), untagged
			 * ones with no ethertype are the lack of a device,
			 * with one a drop on the egress of each leaf.
			 */
			if (!uni && !p.ntags) {
				for (k = 0; k < nrdis; k++)
					if (rdis[k] == v->ethertype)
						break;
				if (v->ethertype && k == nrdis && nrdis < 4)
					rdis[nrdis++] = v->ethertype;
				continue;
			}
			if (!uni && bridged)
				goto bridged_skip;
			why = plan_tc(&c, v, &p);
			if (why)
				goto skip;
			continue;
		}
		if (!uni && p.ntags) {
			/*
			 * Tagged frames the router sends (it has VLAN devices
			 * of its own on pon0): the row is tc on the egress of
			 * pon0, its inverse on the ingress.  Not when the
			 * router shares a bridge with a UNI: the bridge sends
			 * tagged frames of the UNIs through pon0 as well.
			 */
			if (bridged)
				goto bridged_skip;
			ovid = plan_out_vid(&p);
			if (ovid > 0 && !xv_filter_pass(flt, ovid)) {
				DBG("eth: router row %d filtered out by VLAN "
				    "tagging filter data", i);
				continue;
			}
			why = plan_tc(&c, v, &p);
			if (why)
				goto skip;
			router_vid_map(m, &p);
			leaf_add(leaf, nleaf, max, GPON_NETDEV, 0);
			continue;
		}
		if (!c.force_tc) {
			why = plan_device(&c, v, &p, &r);
			if (!why) {
				leaves(b, r.uni, r.ani, r.vid, flt, leaf, nleaf, max);
				if (r.dscp)
					dscp_rules(m, r.ani, c.dscp);
				continue;
			}
		} else {
			why = "tc only";
		}
		/* tc: the leaf is the raw ANI device, the tags are the row's */
		ovid = plan_out_vid(&p);
		if (ovid > 0 && !xv_filter_pass(flt, ovid)) {
			DBG("eth: ME171 row %d filtered out by VLAN tagging "
			    "filter data", i);
			continue;
		}
		{
			const char *tcwhy = plan_tc(&c, v, &p);

			if (!tcwhy) {
				/* the VID was checked above for a tagged result */
				leaves(b, uni, GPON_NETDEV, 0,
				       ovid > 0 ? NULL : flt, leaf, nleaf, max);
				continue;
			}
			if (c.force_tc)
				why = tcwhy;
		}
skip:
		INFO("eth: ME171 0x%04x row %d skipped: %s", ev->id, i, why);
		continue;
bridged_skip:
		INFO("eth: ME171 0x%04x row %d skipped: tagged frames of the "
		     "router in a bridge with a UNI", ev->id, i);
	}
	if (!uni && nrdis && leaf)
		router_discard(m, leaf, *nleaf, rdis, nrdis);
}

void xv_uni_port(struct eth_model *m, struct eth_bridge *b, const char *uni,
		 struct omci_inst *ev, const struct xv_filter *f)
{
	static struct vrow rows[XV_MAX_ROWS];

	/* ME 78 strips downstream even when its upstream mode is 0 */
	me78_strip(m, ev, uni);
	if (!ev || !rows_get(ev, rows)) {
		xv_transparent_port(m, b, uni, f);
		return;
	}
	run_rows(m, b, uni, ev, f, NULL, NULL, 0, false);
}

int xv_l3_port(struct eth_model *m, struct omci_inst *ev,
	       const struct xv_filter *f, struct xv_leaf *leaf, int max,
	       bool bridged)
{
	static struct vrow rows[XV_MAX_ROWS];
	int n = 0, i;

	if (!ev || !rows_get(ev, rows)) {
		/* untagged, or the VIDs the filter lets through */
		if (!f || !f->active || f->pbit) {
			if (!f || f->untagged_ok)
				leaf_add(leaf, &n, max, GPON_NETDEV, 0);
			return n;
		}
		for (i = 0; i < f->n; i++) {
			const char *d = eth_vlan_dev(m, GPON_NETDEV, TPID_8021Q,
						     f->vid[i], EGR_DONTCARE,
						     ING_DONTCARE);

			if (d)
				leaf_add(leaf, &n, max, d, f->vid[i]);
		}
		return n;
	}
	run_rows(m, NULL, NULL, ev, f, leaf, &n, max, bridged);

	return n;
}

/* no ME 171 rows: the VIDs ME 84 lets through, else everything */
void xv_transparent_port(struct eth_model *m, struct eth_bridge *b,
			 const char *uni, const struct xv_filter *f)
{
	int i;

	if (!f || !f->active) {
		leaves(b, uni, GPON_NETDEV, 0, f, NULL, NULL, 0);
		return;
	}
	if (f->pbit) {
		/* every VID passes: the raw ports (xv_pbit_raw() drops the
		 * untagged frames and the P-bits that do not pass) */
		static const struct xv_filter all = { .untagged_ok = true };

		leaves(b, uni, GPON_NETDEV, 0, &all, NULL, NULL, 0);
		return;
	}
	if (f->untagged_ok)
		leaves(b, uni, GPON_NETDEV, 0, f, NULL, NULL, 0);
	for (i = 0; i < f->n; i++) {
		const char *u = uni ? eth_vlan_dev(m, uni, TPID_8021Q,
						   f->vid[i], EGR_DONTCARE,
						   ING_IDENT) : NULL;
		const char *a = eth_vlan_dev(m, GPON_NETDEV, TPID_8021Q, f->vid[i],
					     EGR_IDENT, ING_DONTCARE);

		if ((u || !uni) && a)
			leaves(b, u, a, f->vid[i], f, NULL, NULL, 0);
	}
}

/* ---------------------------------------------------------------------
 * ME 84
 * ------------------------------------------------------------------- */

/*
 * Forward operation codes (setvlanFilterForwardOPValue(), 4515-4555; the
 * file #defines TCSUPPORT_VLAN_FILTER_EX at 3521, so the SDK takes every
 * code of filterFwdOpTbl, 0..0x21, 0 included) and what filterFwdOpTbl
 * (3529-3565) says: tagged action and untagged action.  Implemented:
 *   0x00        bridging, no investigation (the filter is off)
 *   0x03 0x0f 0x1c   action (h) VID investigation, untagged bridged
 *   0x04 0x10 0x1d   action (h) VID investigation, untagged discarded
 *   0x16        action (j) VID investigation, untagged bridged
 *   0x17        action (j) VID investigation, untagged discarded
 *   0x07 0x11 0x1e   action (h) user priority investigation, untagged bridged
 *   0x08 0x12 0x1f   action (h) user priority investigation, untagged discarded
 *   0x18        action (j) user priority investigation, untagged bridged
 *   0x19        action (j) user priority investigation, untagged discarded
 *   0x02 0x15   tagged bridged, untagged discarded
 * The list entries are TCIs (P-bit bits 15..13, VID 11..0: the SDK builds
 * them as (tci >> 13) & 7 and tci & 0xfff, 665-675).  Action (h) bridges
 * the frames that match an entry and discards the other tagged ones, in
 * both directions ("ingress frame match ... then bridge, others is
 * filter", "egress frame ... too", cmd_ci/pon_vlan/ponvlancmd.c:1585-1588);
 * with the P-bit investigation the VID is not looked at.  Action (j) is
 * described there as "egress frame match vid in vlan list, then forward,
 * others is filter" (1591-1593), i.e. the egress direction only; here it is
 * treated like (h), which also filters the other direction (the SDK shows
 * nothing more for it).  Not implemented (refused, omci_bind.c): 0x01
 * (discard tagged: the raw ports cannot tell), action (g) 0x05, 0x06, 0x09,
 * 0x0a, and the TCI investigations 0x0b..0x0e, 0x13, 0x14, 0x1a, 0x1b,
 * 0x20, 0x21, which include the DEI.
 */
static bool filter_op(uint8_t op, struct xv_filter *f)
{
	bool untagged_ok = true, pbit = false, vid = false;

	switch (op) {
	case 0x00:
		break;
	case 0x03: case 0x0f: case 0x16: case 0x1c:
		vid = true;
		break;
	case 0x04: case 0x10: case 0x17: case 0x1d:
		vid = true;
		untagged_ok = false;
		break;
	case 0x07: case 0x11: case 0x18: case 0x1e:
		pbit = true;
		break;
	case 0x08: case 0x12: case 0x19: case 0x1f:
		pbit = true;
		untagged_ok = false;
		break;
	case 0x02: case 0x15:
		/* every tagged frame passes: all P-bits */
		pbit = true;
		untagged_ok = false;
		if (f)
			f->pbit_mask = 0xff;
		break;
	default:
		return false;
	}
	if (f) {
		f->active = vid || pbit;
		f->pbit = pbit;
		f->untagged_ok = untagged_ok;
	}

	return true;
}

/* may the OLT set forward operation @op */
bool xv_filter_op_ok(uint8_t op)
{
	return filter_op(op, NULL);
}

void xv_filter_get(struct omci_inst *f84, struct xv_filter *f)
{
	const uint8_t *list = f84 ? mib_val(f84, 1) : NULL;
	int i, n;

	memset(f, 0, sizeof(*f));
	f->untagged_ok = true;
	if (!f84)
		return;
	if (!filter_op(mib_get_uint(f84, 2), f) || !f->active)
		return;
	/* 0x02, 0x15: no list */
	if (f->pbit && f->pbit_mask == 0xff)
		return;
	n = mib_get_uint(f84, 3);
	for (i = 0; list && i < n && i < 12; i++) {
		uint16_t tci = (list[2 * i] << 8) | list[2 * i + 1];

		if (f->pbit)
			f->pbit_mask |= 1 << (tci >> 13);
		else
			f->vid[f->n++] = tci & 0xfff;
	}
}

/* drop the tagged frames whose P-bit does not pass, on @dev, with @match */
static void pbit_drops(struct eth_model *m, const char *dev, bool egress,
		       uint16_t tpid, const char *vid_match, uint8_t mask)
{
	char match[64];
	int p;

	for (p = 0; p < 8; p++) {
		if (mask & (1 << p))
			continue;
		snprintf(match, sizeof(match), "%svlan_prio %d", vid_match, p);
		eth_tc_add(m, dev, egress, tpid_name(tpid), match, "drop");
	}
}

/*
 * The leaf device (pon0.<vid>, pon0.s<vid>[.<vid>]) is the ANI side of
 * ME 84 with a P-bit investigation: the frames leave through pon0 with the
 * tag of the leaf, so pon0 egress matches the outermost tag (VID and P-bit)
 * of the leaf and drops the P-bits that do not pass, pon0 ingress the same
 * for the frames the OLT sends.
 */
void xv_pbit_leaf(struct eth_model *m, const char *leaf, const struct xv_filter *f)
{
	const struct eth_vlan *outer = NULL;
	char vm[24];
	int i, depth;

	if (!f->pbit || f->pbit_mask == 0xff)
		return;
	for (depth = 0; depth < 3 && leaf; depth++) {
		const struct eth_vlan *v = NULL;

		for (i = 0; i < m->nvlan; i++)
			if (!strcmp(m->vlan[i].name, leaf))
				v = &m->vlan[i];
		if (!v)
			return;
		outer = v;
		if (!strcmp(v->parent, GPON_NETDEV))
			break;
		leaf = v->parent;
	}
	if (!outer || strcmp(outer->parent, GPON_NETDEV))
		return;
	snprintf(vm, sizeof(vm), "vlan_id %u ", outer->vid);
	pbit_drops(m, GPON_NETDEV, true, outer->tpid, vm, f->pbit_mask);
	pbit_drops(m, GPON_NETDEV, false, outer->tpid, vm, f->pbit_mask);
}

/*
 * The raw ANI leaf (pon0 itself, every VID, no VLAN device to put a tc rule
 * on): the same for the ingress of the UNI devices, which send what leaves
 * through pon0 (the P-bit as the UNI sends it: a row that rewrites it is
 * not taken into account).  Discarded untagged frames (0x08, 0x12) are
 * dropped there too, behind the pass rules for tagged frames.
 */
void xv_pbit_raw(struct eth_model *m, const char *uni, const struct xv_filter *f)
{
	if (!f->pbit)
		return;
	if (f->pbit_mask != 0xff) {
		pbit_drops(m, uni, false, TPID_8021Q, "", f->pbit_mask);
		pbit_drops(m, uni, false, TPID_8021AD, "", f->pbit_mask);
	}
	if (!f->untagged_ok &&
	    !eth_tc_add(m, uni, false, "802.1Q", "", "pass") &&
	    !eth_tc_add(m, uni, false, "802.1ad", "", "pass"))
		eth_tc_add(m, uni, false, "all", "", "drop");
}

/* ---------------------------------------------------------------------
 * Set value check
 * ------------------------------------------------------------------- */

static struct eth_model *scratch_model(void)
{
	static struct eth_model s;

	memset(&s, 0, sizeof(s));

	return &s;
}

/* may @row be stored in ME 171 @in (the SDK decode, then Linux) */
const char *xv_row_check(struct omci_inst *in, const uint8_t *row)
{
	uint8_t assoc = mib_get_uint(in, EV_ASSOC);
	bool l3 = assoc == ASSOC_IP_HOST || assoc == ASSOC_VEIP;
	struct xv_row v;
	struct plan p;
	struct ctx c;
	struct dev_res r;
	uint32_t n;
	const char *why;
	int cnt = 0;

	if (xv_decode(row, &v))
		return "value not accepted by the SDK decode";
	/* the SDK keeps 32 rows per port (omci_me_vlan.h:318) */
	for (n = 0; in->val[EV_TABLE] && n + XV_ROW_LEN <= in->len[EV_TABLE];
	     n += XV_ROW_LEN)
		cnt++;
	if (v.del)
		return NULL;
	if (cnt >= XV_MAX_ROWS) {
		/* a row with the same filter replaces one */
		for (n = 0; n + XV_ROW_LEN <= in->len[EV_TABLE]; n += XV_ROW_LEN)
			if (!memcmp(in->val[EV_TABLE] + n, row, XV_ROW_LEN / 2))
				break;
		if (n + XV_ROW_LEN > in->len[EV_TABLE])
			return "VLAN tag operation table full";
	}

	ctx_init(&c, scratch_model(), l3 ? NULL : "uni", in);
	why = plan_make(&v, &c, &p);
	if (why)
		return why;
	if (p.block)
		return p.dei ? "DEI filter" : NULL;
	/* the tagged frames the router sends: tc on pon0, it has to work */
	if (l3 && p.ntags)
		return plan_tc(&c, &v, &p);
	why = plan_device(&c, &v, &p, &r);
	if (!why || l3)
		return why;

	return plan_tc(&c, &v, &p);
}
