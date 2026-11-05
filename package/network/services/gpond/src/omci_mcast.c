// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI multicast MEs: multicast operations profile (ME 309), multicast
 * subscriber config info (ME 310) and multicast subscriber monitor
 * (ME 311).
 *
 * References (SDK = airoha-pon/src/gpon-en757x/xpon/src/omci/omci_me/,
 * stock = the omci binary of the vendor firmware, descriptor tables
 * omciAttriDescriptListMulticast* in its data segment):
 *
 * ME 309 (omci_me_multicast_nodetable.c:115-135, _func.c:1833-2755)
 *   1 IGMP version         2, 3, 16 (MLDv1), 17 (MLDv2), else the Set fails
 *                          (_func.c:1857-1865).  The value never reaches the
 *                          IGMP module: the igmpONUSetVer() call of
 *                          setRealMulticastRuleByProfile() sits in an #if 0
 *                          with the comment that the OLT only asks what the
 *                          ONU supports (_func.c:558-564), and clearing a
 *                          profile sets MLDv2, i.e. every version is on
 *                          (_func.c:1043-1045).  The stock descriptor of
 *                          attribute 1 is the general getter/setter pair, no
 *                          module call either
 *   2 IGMP function        0 snooping, 1 snooping with proxy reporting,
 *                          2 proxy; else the Set fails (_func.c:1895-1912)
 *   3 immediate leave      0 / 1 (_func.c:1938-1951)
 *   4 upstream IGMP TCI    any (_func.c:1971-2003)
 *   5 upstream tag control 0..3 are applied; any other value is only
 *                          stored, the Set succeeds (_func.c:2022-2034)
 *   6 upstream IGMP rate   any (_func.c:2051-2080); the IGMP module counts
 *                          the multicast packets of a UNI before it looks at
 *                          the protocol, so MLD counts too
 *                          (xpon_igmp_core.c:3400-3447, 3450-3457)
 *   7/8 dynamic / static ACL: rows of 24 bytes (stock descriptor len 24,
 *                          SDK _func.c:2177-2470, row layout 2222-2243:
 *                          control 2, GEM 2, VLAN 2, source IPv4 4, range
 *                          start 4, range end 4, imputed bandwidth 4, 2
 *                          unused).  The stock setter (the function the
 *                          descriptors point to, "_setMOPMulticastAclTableValue")
 *                          clears bit 10 of the control word (test bit),
 *                          set control 0 fails, 1 writes (row key: part
 *                          13:11 + row id 9:0), 2 deletes the first row
 *                          with that row id (a missing row is no error), 3
 *                          clears the table.
 *   9 lost groups          rows of 10 bytes (stock descriptor), read only;
 *                          nothing fills it here and nothing filled it in
 *                          the vendor either: the stock descriptor of
 *                          attribute 9 is getGeneralTableValue with no
 *                          setter, i.e. it returns what was stored and
 *                          nothing stores it
 *  10..13 robustness, querier IP, query interval, max response time
 *                          any value (_func.c:2507-2635)
 *  14 last member query    any, R/W but not set-by-create
 *  15 unauthorized join    any, R/W but not set-by-create.  The IGMP module
 *                          keeps it (xpon_igmp_itf.c:550-556) but the only
 *                          reader, xpon_pass_access_control()
 *                          (xpon_igmp_core.c:3334-3372), is called from an
 *                          #if 0 block (xpon_igmp_itf.c:1593-1596): no
 *                          build of the board uses it
 *  16 downstream IGMP and multicast TCI: control 0..7 accepted, any other
 *                          value only at Create (_func.c:2701-2755); the
 *                          module takes 0..4 (xpon_igmp_itf.c:453-467)
 *
 * ME 310 (nodetable.c:162-171, _func.c:2759-3170; stock for the tables)
 *   1 ME type   0 bridge port, 1 802.1p mapper, else the Set fails
 *               (getLanPortByMeType(), _func.c:311-409)
 *   2..5        pointer to ME 309, max simultaneous groups, max multicast
 *               bandwidth, bandwidth enforcement: any value (SDK stores,
 *               _func.c:2899-3170).  The module takes them
 *               (igmpONUSetMaxPlayNum / igmpONUSetMaxBW /
 *               igmpONUSetMaxEnforcement, _func.c:2974-3167) and the limit
 *               decides whether a join is honoured or counted as exceeded
 *               (xpon_igmp_core.c:4229-4306)
 *   6 service package table: rows of 20 bytes, 7 preview groups table: rows
 *               of 22 bytes (stock descriptors).  Stock: attribute 7 uses
 *               the ACL setter above; attribute 6 has its own
 *               ("_setMulticastSubCfgInfoSrvPktTableValue"): row key = row
 *               id 9:0, set control 0 fails, 1 writes, 2 deletes (a missing
 *               row fails), 3 clears.  The row contents are stored only:
 *               the SDK setters of attributes 6 and 7 are the general one
 *               (nodetable.c:169-170), the only preview the IGMP module has
 *               is per dynamic ACL row (xpon_igmp_itf.c:1064-1068), and
 *               OMCI never writes it there.
 *
 * ME 311 (nodetable.c:184-193, _func.c:3175-3366)
 *   1 ME type   as ME 310 (the instance id is the bridge port / mapper)
 *   2 current multicast bandwidth, 3 join messages counter, 4 bandwidth
 *               exceeded counter, 5 IPv4 active group table: rows of 24 bytes
 *               (_func.c:3300-3336: VLAN 2, source 4, group 4, bandwidth 4,
 *               client 4, time 4, 2 reserved), 6 IPv6 active group table:
 *               rows of 58 bytes (stock descriptor).  The SDK has no
 *               producer for attribute 6 either (the getter is the general
 *               one, nodetable.c:191); here the IPv6 groups of the bridge
 *               MDB are reported in the G.988 field order: VLAN 2, source
 *               16, group 16, bandwidth 4, client 16, join time 4.
 *
 * ME creation: none of the three is created by the ONU (the init functions,
 * _func.c:1759-1771, 2759-2770, 3175-3189, only set the action functions).
 *
 * Linux has no producer for the join/leave statistics the vendor kernel
 * (xpon_igmp_core.c:3948-3972) keeps; the monitor reads the bridge MDB
 * (RTM_GETMDB, net/bridge/br_mdb.c br_mdb_dump()) and follows what appears
 * and disappears there.
 */
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unl.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/socket.h>
#include <linux/if_bridge.h>
#include <linux/if_ether.h>
#include <linux/rtnetlink.h>

#include "gpond.h"
#include "omci_mcast.h"
#include "eth.h"

#define MC_ACL_MAX_ROWS		256
#define MC_SERVICE_LEN		20
#define MC_PREVIEW_LEN		22
#define MC_ROWS_PER_PORT	20	/* SDK OMCI_MAX_ACTIVE_MLTICAST_GROUP_NUM */
#define MC_MON_POLL_MS		10000
#define MC_MON_EVENT_MS		300

/* table control word: set control 15:14, row part 13:11, row id 9:0 */
#define CTRL_SET(_c)		(((_c) >> 14) & 3)
#define CTRL_PART(_c)		(((_c) >> 11) & 7)
#define CTRL_ID(_c)		((_c) & 0x3ff)
#define CTRL_TEST_BIT		(1 << 10)

#define SET_WRITE		1
#define SET_DELETE		2
#define SET_CLEAR		3

bool mcast_is_me(uint16_t class_id)
{
	return class_id >= CLASS_MC_PROFILE && class_id <= CLASS_MC_MONITOR;
}

void mc_acl_decode(const uint8_t *row, struct mc_acl *a)
{
	a->ctrl = rd_be16(row);
	a->gem = rd_be16(row + 2);
	a->vid = rd_be16(row + 4);
	memcpy(a->src, row + 6, 4);
	memcpy(a->start, row + 10, 4);
	memcpy(a->end, row + 14, 4);
	a->bandwidth = (uint32_t)row[18] << 24 | row[19] << 16 | row[20] << 8 |
		       row[21];
}

/* a row that names one group: the bridge MDB holds groups, not ranges */
bool mc_acl_single(const struct mc_acl *a)
{
	return !memcmp(a->start, a->end, 4);
}

/* xpon_pass_access_control() / xpon_up_igmp_filter_src(): range + source */
bool mc_acl_match(const struct mc_acl *a, const uint8_t grp[4],
		  const uint8_t src[4])
{
	if (memcmp(grp, a->start, 4) < 0 || memcmp(grp, a->end, 4) > 0)
		return false;
	if (!is_zero4(a->src) && !is_zero4(src) && memcmp(a->src, src, 4))
		return false;

	return true;
}

/* ---------------------------------------------------------------------
 * instances
 * ------------------------------------------------------------------- */

/*
 * Service package table row (G.988 9.3.28 attribute 6, 20 bytes):
 *   0..1   row control
 *   2..3   VID (12 bits)
 *   4..5   max simultaneous groups
 *   6..9   max multicast bandwidth
 *   10..11 multicast operations profile pointer (ME 309)
 *   12..19 reserved
 * An OLT that provisions ME 310 through the service package table clears
 * attribute 2 (MSC_PROFILE = 0) and writes the ME 309 pointer in the row.
 */
static const uint8_t *service_row(struct omci_inst *sub)
{
	const uint8_t *tab;
	unsigned int o;

	if (!sub)
		return NULL;
	tab = sub->val[MSC_SERVICE_PACKAGE];
	for (o = 0; tab && o + MC_SERVICE_LEN <= sub->len[MSC_SERVICE_PACKAGE];
	     o += MC_SERVICE_LEN) {
		uint16_t ptr = rd_be16(tab + o + 10);

		if (ptr && ptr != 0xffff && mib_find(CLASS_MC_PROFILE, ptr))
			return tab + o;
	}

	return tab && sub->len[MSC_SERVICE_PACKAGE] >= MC_SERVICE_LEN ?
	       tab : NULL;
}

uint16_t mc_profile_id(struct omci_inst *sub)
{
	const uint8_t *row;
	uint16_t ptr;

	if (!sub)
		return 0;
	ptr = mib_get_uint(sub, MSC_PROFILE);
	if (ptr && ptr != 0xffff && mib_find(CLASS_MC_PROFILE, ptr))
		return ptr;
	row = service_row(sub);

	return row ? rd_be16(row + 10) : ptr;
}

struct omci_inst *mc_profile(struct omci_inst *sub)
{
	uint16_t ptr = mc_profile_id(sub);

	return sub && ptr && ptr != 0xffff ?
	       mib_find(CLASS_MC_PROFILE, ptr) : NULL;
}

uint16_t mc_max_groups(struct omci_inst *sub)
{
	const uint8_t *row;
	uint16_t v;

	if (!sub)
		return 0;
	v = mib_get_uint(sub, MSC_MAX_GROUPS);
	if (v)
		return v;
	row = service_row(sub);

	return row ? rd_be16(row + 4) : 0;
}

uint32_t mc_max_bw(struct omci_inst *sub)
{
	const uint8_t *row;
	uint32_t v;

	if (!sub)
		return 0;
	v = mib_get_uint(sub, MSC_MAX_BANDWIDTH);
	if (v)
		return v;
	row = service_row(sub);

	return row ? rd_be32(row + 6) : 0;
}

int mc_service_vids(struct omci_inst *sub, uint16_t *vid, int max)
{
	const uint8_t *tab = sub ? sub->val[MSC_SERVICE_PACKAGE] : NULL;
	unsigned int o;
	int n = 0, i;

	for (o = 0; tab && o + MC_SERVICE_LEN <= sub->len[MSC_SERVICE_PACKAGE];
	     o += MC_SERVICE_LEN) {
		uint16_t v = rd_be16(tab + o + 2) & 0xfff;

		for (i = 0; i < n && vid[i] != v; i++)
			;
		if (v && v < 0xfff && i == n && n < max)
			vid[n++] = v;
	}

	return n;
}

struct omci_inst *mc_subscriber(uint8_t me_type, uint16_t id)
{
	struct omci_inst *in = mib_find(CLASS_MC_SUBSCRIBER, id);

	return in && mib_get_uint(in, MSC_ME_TYPE) == me_type ? in : NULL;
}

/* ---------------------------------------------------------------------
 * tables with a set control word
 * ------------------------------------------------------------------- */

/*
 * The rows of a table of ME 309 / 310 as the OLT wrote them.  The generic
 * row handling of omci.c keys a row by the first half of it and knows no set
 * control, so every row is decoded here, applied to this copy, and what
 * omci.c stored is replaced by the copy (mcast_set(), which runs after
 * all the rows of the Set).
 */

static LIST_HEAD(shadows);

static struct shadow *shadow_get(struct omci_inst *in, uint8_t idx, bool add)
{
	struct shadow *s;

	list_for_each_entry(s, &shadows, list)
		if (s->class_id == in->def->class_id && s->id == in->id &&
		    s->idx == idx)
			return s;
	if (!add)
		return NULL;
	s = calloc(1, sizeof(*s));
	if (!s)
		return NULL;
	s->class_id = in->def->class_id;
	s->id = in->id;
	s->idx = idx;
	/* the rows the MIB already has (restored from /usr/lib/gpon/gpon.json, or written
	 * before a replay): the next Set adds to them */
	if (in->val[idx] && in->len[idx]) {
		s->data = malloc(in->len[idx]);
		if (s->data) {
			memcpy(s->data, in->val[idx], in->len[idx]);
			s->len = in->len[idx];
		}
	}
	list_add_tail(&s->list, &shadows);

	return s;
}

static void shadow_free(struct shadow *s)
{
	list_del(&s->list);
	free(s->data);
	free(s);
}

/* index of the stored row with this key, -1: none */
static int row_find(const struct shadow *s, unsigned int rl, uint16_t id,
		    int part)
{
	unsigned int n;

	for (n = 0; s->data && n + rl <= s->len; n += rl) {
		uint16_t c = rd_be16(s->data + n);

		if (CTRL_ID(c) != id)
			continue;
		if (part < 0 || CTRL_PART(c) == part)
			return n / rl;
	}

	return -1;
}

/*
 * @acl: ACL semantics (the stock "_setMOPMulticastAclTableValue"): the test
 * bit is cleared, rows are keyed by part + row id when written, by the row
 * id alone when deleted, a missing row to delete is no error.  Otherwise the
 * service package semantics: key = row id, a missing row to delete fails.
 */
static bool table_check(struct omci_inst *in, uint8_t idx, unsigned int rl,
			const uint8_t *v, bool acl)
{
	uint8_t row[MC_ACL_LEN];
	uint16_t ctrl = rd_be16(v);
	struct shadow *s;
	uint8_t *p;
	int at;

	if (rl > sizeof(row))
		return false;
	memcpy(row, v, rl);
	if (acl) {
		ctrl &= ~CTRL_TEST_BIT;
		row[0] = ctrl >> 8;
	}
	s = shadow_get(in, idx, true);
	if (!s)
		return false;

	switch (CTRL_SET(ctrl)) {
	case SET_CLEAR:
		s->len = 0;
		return true;
	case SET_WRITE:
		at = row_find(s, rl, CTRL_ID(ctrl),
			      acl ? (int)CTRL_PART(ctrl) : -1);
		if (at >= 0) {
			memcpy(s->data + at * rl, row, rl);
			return true;
		}
		if (s->len / rl >= MC_ACL_MAX_ROWS)
			return false;
		p = realloc(s->data, s->len + rl);
		if (!p)
			return false;
		s->data = p;
		memcpy(s->data + s->len, row, rl);
		s->len += rl;
		return true;
	case SET_DELETE:
		at = row_find(s, rl, CTRL_ID(ctrl), -1);
		if (at < 0)
			return acl;
		memmove(s->data + at * rl, s->data + (at + 1) * rl,
			s->len - (at + 1) * rl);
		s->len -= rl;
		return true;
	default:
		return false;			/* set control 0 */
	}
}

/* ---------------------------------------------------------------------
 * Set / Create checks
 * ------------------------------------------------------------------- */

static bool in_range(uint8_t v, uint8_t max)
{
	return v <= max;
}

static bool profile_check(struct omci_inst *in, uint8_t idx, const uint8_t *v,
			  bool create)
{
	switch (idx) {
	case MOP_VERSION:
		return v[0] == MC_VERSION_V2 || v[0] == MC_VERSION_V3 ||
		       v[0] == MC_VERSION_MLD1 || v[0] == MC_VERSION_MLD2;
	case MOP_FUNCTION:
		return in_range(v[0], MC_FUNC_PROXY);
	case MOP_FAST_LEAVE:
		return in_range(v[0], 1);
	case MOP_DYN_ACL:
	case MOP_STATIC_ACL:
		return table_check(in, idx, MC_ACL_LEN, v, true);
	case MOP_DOWN_TCI:
		/* an unknown control is stored at Create, refused by Set */
		return create || in_range(v[0], 7);
	default:
		return true;
	}
}

static bool subscriber_check(struct omci_inst *in, uint8_t idx,
			     const uint8_t *v)
{
	switch (idx) {
	case MSC_ME_TYPE:
		return in_range(v[0], MC_TYPE_MAPPER);
	case MSC_SERVICE_PACKAGE:
		return table_check(in, idx, MC_SERVICE_LEN, v, false);
	case MSC_PREVIEW_GROUPS:
		return table_check(in, idx, MC_PREVIEW_LEN, v, true);
	default:
		return true;
	}
}

bool mcast_check(struct omci_inst *in, uint8_t idx, const uint8_t *v,
		 bool create)
{
	switch (in->def->class_id) {
	case CLASS_MC_PROFILE:
		return profile_check(in, idx, v, create);
	case CLASS_MC_SUBSCRIBER:
		return subscriber_check(in, idx, v);
	default:
		return true;
	}
}

/* ---------------------------------------------------------------------
 * ME 311 monitor
 * ------------------------------------------------------------------- */

static struct mon_act act[MC_MDB_MAX];
static struct mon_cnt cnt[MC_MAX_INST];
static bool mon_on, mon_baseline, mon_warned;
static struct uloop_timeout mon_tmo, mon_evt_tmo;
static struct unl mon_evt;
static struct uloop_fd mon_fd;
static bool mon_nl_open;

static uint32_t now_s(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);

	return t.tv_sec;
}

/* ---- netlink: RTM_GETMDB ---- */

static void mdb_info(struct mdb_ctx *c, const struct nlattr *info)
{
	const struct br_mdb_entry *e;
	unsigned int hdr = NLA_ALIGN(sizeof(*e));
	const struct nlattr *a;
	struct mc_mdb *m;
	int len = nla_len(info), rem;
	bool srclist = false, include = false;
	uint16_t proto;

	if (len < (int)hdr || c->n >= c->max)
		return;
	e = nla_data(info);
	if (e->ifindex == c->br_ifindex)
		return;				/* the bridge itself joined */
	if (e->flags & MDB_FLAGS_BLOCKED)
		return;				/* an excluded source: not forwarded */
	proto = ntohs(e->addr.proto);
	if (proto != ETH_P_IP && proto != ETH_P_IPV6)
		return;

	m = &c->e[c->n];
	memset(m, 0, sizeof(*m));
	if (!if_indextoname(e->ifindex, m->port))
		return;
	memcpy(m->br, c->br, sizeof(m->br));
	m->family = proto == ETH_P_IP ? 4 : 6;
	memcpy(m->group, &e->addr.u, m->family == 4 ? 4 : 16);

	nla_for_each_attr(a, (const struct nlattr *)((const char *)e + hdr),
			  len - hdr, rem) {
		switch (nla_type(a)) {
		case MDBA_MDB_EATTR_SOURCE:
			if (nla_len(a) >= (m->family == 4 ? 4 : 16))
				memcpy(m->source, nla_data(a),
				       m->family == 4 ? 4 : 16);
			break;
		case MDBA_MDB_EATTR_SRC_LIST:
			srclist = true;
			break;
		case MDBA_MDB_EATTR_GROUP_MODE:
			include = nla_len(a) >= 1 && *(uint8_t *)nla_data(a) == 1;
			break;
		}
	}
	/* an INCLUDE group of IGMPv3 is its (S,G) entries */
	if (srclist && include)
		return;
	c->n++;
}

static int mdb_msg(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct mdb_ctx *c = arg;
	struct nlattr *mdb, *ent, *info;
	const struct br_port_msg *bpm;
	int rem, rem2, rem3;

	if ((nlh->nlmsg_type != RTM_GETMDB && nlh->nlmsg_type != RTM_NEWMDB) ||
	    nlmsg_len(nlh) < (int)sizeof(*bpm))
		return NL_SKIP;
	bpm = nlmsg_data(nlh);
	c->br_ifindex = bpm->ifindex;
	if (!if_indextoname(bpm->ifindex, c->br))
		return NL_SKIP;

	nla_for_each_attr(mdb, nlmsg_attrdata(nlh, sizeof(*bpm)),
			  nlmsg_attrlen(nlh, sizeof(*bpm)), rem) {
		if (nla_type(mdb) != MDBA_MDB)
			continue;
		nla_for_each_nested(ent, mdb, rem2) {
			if (nla_type(ent) != MDBA_MDB_ENTRY)
				continue;
			nla_for_each_nested(info, ent, rem3)
				if (nla_type(info) == MDBA_MDB_ENTRY_INFO)
					mdb_info(c, info);
		}
	}

	return NL_SKIP;
}

static int mdb_read_nl(struct mc_mdb *e, int max)
{
	struct mdb_ctx c = { .e = e, .max = max };
	struct br_port_msg bpm = { .family = AF_BRIDGE };
	struct nl_msg *m;
	struct unl u;
	int err;

	if (unl_rtnl_init(&u))
		return -EIO;
	m = unl_rtnl_msg(&u, RTM_GETMDB, true);
	if (!m) {
		unl_free(&u);
		return -ENOMEM;
	}
	nlmsg_append(m, &bpm, sizeof(bpm), NLMSG_ALIGNTO);
	err = unl_request(&u, m, mdb_msg, &c);
	unl_free(&u);

	return err < 0 ? err : c.n;
}

int (*mcast_mdb_read)(struct mc_mdb *e, int max) = mdb_read_nl;

/* RTM_NEWMDB / RTM_DELMDB: something joined or left, look again soon */
static int mdb_event(struct nl_msg *msg, void *arg)
{
	uloop_timeout_set(&mon_evt_tmo, MC_MON_EVENT_MS);

	return NL_SKIP;
}

/* notifications carry no sequence number of ours (as unl_loop()) */
static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static void mon_evt_read(struct uloop_fd *u, unsigned int events)
{
	struct nl_cb *cb = nl_cb_alloc(NL_CB_CUSTOM);

	if (!cb)
		return;
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, mdb_event, NULL);
	/* one datagram per call: epoll is level triggered, a non-blocking
	 * socket without data answers 0 and would never end a loop */
	nl_recvmsgs(mon_evt.sock, cb);
	nl_cb_put(cb);
}

static void mon_nl_open_evt(void)
{
	if (mon_nl_open || mcast_mdb_read != mdb_read_nl)
		return;
	if (unl_rtnl_init(&mon_evt))
		return;
	if (nl_socket_add_membership(mon_evt.sock, RTNLGRP_MDB)) {
		unl_free(&mon_evt);
		return;
	}
	nl_socket_disable_seq_check(mon_evt.sock);
	nl_socket_set_nonblocking(mon_evt.sock);
	mon_fd.fd = nl_socket_get_fd(mon_evt.sock);
	mon_fd.cb = mon_evt_read;
	uloop_fd_add(&mon_fd, ULOOP_READ);
	mon_nl_open = true;
}

static void mon_nl_close_evt(void)
{
	if (!mon_nl_open)
		return;
	uloop_fd_delete(&mon_fd);
	unl_free(&mon_evt);
	mon_nl_open = false;
}

/* ---- the groups of an instance ---- */

static struct mon_cnt *cnt_get(uint16_t id, bool add)
{
	struct mon_cnt *free_slot = NULL;
	int i;

	for (i = 0; i < MC_MAX_INST; i++) {
		if (cnt[i].used && cnt[i].id == id)
			return &cnt[i];
		if (!cnt[i].used && !free_slot)
			free_slot = &cnt[i];
	}
	if (add && free_slot) {
		memset(free_slot, 0, sizeof(*free_slot));
		free_slot->used = true;
		free_slot->id = id;
		return free_slot;
	}

	return NULL;
}

#define MC_MAX_PORTS		ETH_MAX_PORTS

static int inst_ports(struct omci_inst *mon, const char **dev)
{
	return eth_mcast_ports(mib_get_uint(mon, MSM_ME_TYPE), mon->id, dev,
			       MC_MAX_PORTS);
}

static bool in_ports(const char **dev, int n, const char *port)
{
	int i;

	for (i = 0; i < n; i++)
		if (!strcmp(dev[i], port))
			return true;

	return false;
}

/*
 * The ACL row that admits a group: the static table first (no bandwidth is
 * accounted, xpon_igmp_core.c:4160-4183), then the dynamic one whose imputed
 * bandwidth counts (4184-4227).  The MDB carries no VLAN (bridges here do
 * not filter VLANs): only the group and the source are compared.
 */
static bool acl_lookup(struct omci_inst *prof, const uint8_t *grp,
		       const uint8_t *src, uint16_t *vid, uint32_t *bw)
{
	static const uint8_t attr[2] = { MOP_STATIC_ACL, MOP_DYN_ACL };
	int t;

	*vid = 0;
	*bw = 0;
	if (!prof)
		return false;
	for (t = 0; t < 2; t++) {
		const uint8_t *tab = prof->val[attr[t]];
		unsigned int n;

		for (n = 0; tab && n + MC_ACL_LEN <= prof->len[attr[t]];
		     n += MC_ACL_LEN) {
			struct mc_acl a;

			mc_acl_decode(tab + n, &a);
			if (!mc_acl_match(&a, grp, src))
				continue;
			*vid = a.vid & 0xfff;
			*bw = t ? a.bandwidth : 0;
			return true;
		}
	}

	return false;
}

static void put32(uint8_t *p, uint32_t v)
{
	p[0] = v >> 24;
	p[1] = v >> 16;
	p[2] = v >> 8;
	p[3] = v;
}

/* attributes 2..6 of one ME 311 from the active groups */
static void mon_fill(struct omci_inst *mon)
{
	uint8_t me_type = mib_get_uint(mon, MSM_ME_TYPE);
	struct omci_inst *prof = mc_profile(mc_subscriber(me_type, mon->id));
	const char *dev[MC_MAX_PORTS];
	uint8_t *rows, *rows6;
	unsigned int len = 0, len6 = 0, max_rows;
	uint32_t bw_sum = 0, now = now_s();
	struct mon_cnt *c = cnt_get(mon->id, true);
	int i, nd = inst_ports(mon, dev);

	max_rows = MC_ROWS_PER_PORT * (nd ? nd : 1);
	rows = calloc(max_rows, MC_GROUP_ROW_LEN);
	rows6 = calloc(max_rows, MC_IPV6_ROW_LEN);
	if (!rows || !rows6) {
		free(rows);
		free(rows6);
		return;
	}

	for (i = 0; i < MC_MDB_MAX; i++) {
		const struct mon_act *a = &act[i];
		uint16_t vid;
		uint32_t bw;
		uint8_t *r;

		if (!a->used || !in_ports(dev, nd, a->e.port))
			continue;
		if (a->e.family == 6) {
			/* 58 byte row: VLAN 2, source 16, group 16, bandwidth
			 * 4, client 16, join time 4.  The ACL tables are
			 * IPv4 only, so there is no VLAN or bandwidth. */
			if (len6 / MC_IPV6_ROW_LEN >= max_rows)
				continue;
			r = rows6 + len6;
			r[0] = r[1] = 0;
			memcpy(r + 2, a->e.source, 16);
			memcpy(r + 18, a->e.group, 16);
			put32(r + 34, 0);
			/* client address: the bridge does not know the host */
			memset(r + 38, 0, 16);
			put32(r + 54, now - a->first);
			len6 += MC_IPV6_ROW_LEN;
			continue;
		}
		if (a->e.family != 4)
			continue;
		acl_lookup(prof, a->e.group, a->e.source, &vid, &bw);
		bw_sum += bw;
		if (len / MC_GROUP_ROW_LEN >= max_rows)
			continue;
		r = rows + len;
		r[0] = vid >> 8;
		r[1] = vid;
		memcpy(r + 2, a->e.source, 4);
		memcpy(r + 6, a->e.group, 4);
		put32(r + 10, bw);
		/* client address: the bridge does not know the host */
		put32(r + 18, now - a->first);
		len += MC_GROUP_ROW_LEN;
	}

	mib_set_uint(mon, MSM_CUR_BANDWIDTH, bw_sum);
	if (c) {
		mib_set_uint(mon, MSM_JOINS, c->joins);
		mib_set_uint(mon, MSM_BW_EXCEEDED, c->exceeded);
	}
	mib_set_raw(mon, MSM_IPV4_GROUPS, rows, len);
	mib_set_raw(mon, MSM_IPV6_GROUPS, rows6, len6);
	free(rows);
	free(rows6);
}

/* a group appeared on a port the instance covers: the vendor join accounting */
static void mon_join(struct omci_inst *mon, int at)
{
	const struct mon_act *e = &act[at];
	uint8_t me_type = mib_get_uint(mon, MSM_ME_TYPE);
	struct omci_inst *sub = mc_subscriber(me_type, mon->id);
	struct omci_inst *prof = mc_profile(sub);
	const char *dev[MC_MAX_PORTS];
	uint32_t max_bw = mc_max_bw(sub);
	bool enforce = sub && mib_get_uint(sub, MSC_BW_ENFORCEMENT);
	uint32_t cur = 0, bw;
	struct mon_cnt *c = cnt_get(mon->id, true);
	uint16_t vid;
	int i, nd = inst_ports(mon, dev);
	bool exceeded = false;

	if (!c || !in_ports(dev, nd, e->e.port))
		return;

	/* what the instance carries without the groups that are new */
	for (i = 0; i < MC_MDB_MAX; i++) {
		const struct mon_act *a = &act[i];

		/* the groups that joined before this one in the same pass count */
		if (!a->used || (a->fresh && i > at) || a->e.family != 4 ||
		    !in_ports(dev, nd, a->e.port) || i == at)
			continue;
		acl_lookup(prof, a->e.group, a->e.source, &vid, &bw);
		cur += bw;
	}
	if (e->e.family == 4 &&
	    acl_lookup(prof, e->e.group, e->e.source, &vid, &bw) && bw && max_bw)
		exceeded = cur + bw > max_bw;

	/* xpon_igmp_core.c:4228-4262: with enforcement the join is dropped */
	if (exceeded)
		c->exceeded++;
	if (!exceeded || !enforce)
		c->joins++;
}

static bool same_group(const struct mc_mdb *a, const struct mc_mdb *b)
{
	return a->family == b->family && !strcmp(a->port, b->port) &&
	       !strcmp(a->br, b->br) && !memcmp(a->group, b->group, 16) &&
	       !memcmp(a->source, b->source, 16);
}

static void mon_stop(void)
{
	mon_on = false;
	uloop_timeout_cancel(&mon_tmo);
	uloop_timeout_cancel(&mon_evt_tmo);
	mon_nl_close_evt();
}

void mcast_mon_refresh(void)
{
	static struct mc_mdb tmp[MC_MDB_MAX];
	struct omci_inst *in;
	bool any = false;
	int n, i, j;

	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == CLASS_MC_MONITOR)
			any = true;
	if (!any) {
		mon_stop();
		return;
	}

	n = mcast_mdb_read(tmp, MC_MDB_MAX);
	if (n < 0) {
		if (!mon_warned) {
			mon_warned = true;
			WARN("OMCI: multicast monitor: bridge MDB: %s",
			     strerror(-n));
		}
		n = 0;
	}

	for (i = 0; i < MC_MDB_MAX; i++)
		act[i].seen = act[i].fresh = false;
	for (j = 0; j < n; j++) {
		int free_slot = -1;

		for (i = 0; i < MC_MDB_MAX; i++) {
			if (act[i].used && same_group(&act[i].e, &tmp[j]))
				break;
			if (!act[i].used && free_slot < 0)
				free_slot = i;
		}
		if (i < MC_MDB_MAX) {
			act[i].seen = true;
		} else if (free_slot >= 0) {
			act[free_slot].used = act[free_slot].seen = true;
			act[free_slot].fresh = !mon_baseline;
			act[free_slot].e = tmp[j];
			act[free_slot].first = now_s();
		}
	}
	for (i = 0; i < MC_MDB_MAX; i++)
		if (act[i].used && !act[i].seen)
			memset(&act[i], 0, sizeof(act[i]));
	mon_baseline = false;

	list_for_each_entry(in, &omci_mib, list) {
		if (in->def->class_id != CLASS_MC_MONITOR)
			continue;
		for (i = 0; i < MC_MDB_MAX; i++)
			if (act[i].used && act[i].fresh)
				mon_join(in, i);
	}
	list_for_each_entry(in, &omci_mib, list)
		if (in->def->class_id == CLASS_MC_MONITOR)
			mon_fill(in);

	uloop_timeout_set(&mon_tmo, MC_MON_POLL_MS);
}

static void mon_tmo_cb(struct uloop_timeout *t)
{
	mcast_mon_refresh();
}

static void mon_start(void)
{
	if (mon_on)
		return;
	mon_on = true;
	mon_baseline = true;		/* groups that are there are no joins */
	mon_warned = false;
	mon_tmo.cb = mon_tmo_cb;
	mon_evt_tmo.cb = mon_tmo_cb;
	mon_nl_open_evt();
}

void mcast_mon_dump(struct blob_buf *b)
{
	struct omci_inst *in;
	void *a, *t;
	int n = 0, n6 = 0, i;

	for (i = 0; i < MC_MDB_MAX; i++) {
		n += act[i].used;
		if (act[i].used && act[i].e.family == 6)
			n6++;
	}
	blobmsg_add_u8(b, "monitor", mon_on);
	blobmsg_add_u32(b, "active_groups", n);
	/* what the bridge snoops of IPv6 (ME 311 attribute 6) */
	blobmsg_add_u32(b, "active_ipv6_groups", n6);
	a = blobmsg_open_array(b, "subscribers");
	list_for_each_entry(in, &omci_mib, list) {
		struct mon_cnt *c;

		if (in->def->class_id != CLASS_MC_MONITOR)
			continue;
		c = cnt_get(in->id, false);
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "me", in->id);
		blobmsg_add_u32(b, "type", mib_get_uint(in, MSM_ME_TYPE));
		blobmsg_add_u32(b, "bandwidth", mib_get_uint(in, MSM_CUR_BANDWIDTH));
		blobmsg_add_u32(b, "joins", c ? c->joins : 0);
		blobmsg_add_u32(b, "bandwidth_exceeded", c ? c->exceeded : 0);
		blobmsg_add_u32(b, "ipv4_groups",
				in->len[MSM_IPV4_GROUPS] / MC_GROUP_ROW_LEN);
		blobmsg_add_u32(b, "ipv6_groups",
				in->len[MSM_IPV6_GROUPS] / MC_IPV6_ROW_LEN);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}

/* ---------------------------------------------------------------------
 * bind hooks
 * ------------------------------------------------------------------- */

void mcast_set(struct omci_inst *in, uint16_t mask)
{
	struct shadow *s;

	if (in->def->class_id == CLASS_MC_MONITOR) {
		/* another ME type, another set of ports */
		if (mask & omci_mask(MSM_ME_TYPE))
			mcast_mon_refresh();
		return;
	}
	list_for_each_entry(s, &shadows, list)
		if (s->class_id == in->def->class_id && s->id == in->id &&
		    (mask & omci_mask(s->idx)))
			mib_set_raw(in, s->idx, s->data, s->len);
}

void mcast_create(struct omci_inst *in)
{
	if (in->def->class_id != CLASS_MC_MONITOR)
		return;
	mon_start();
	cnt_get(in->id, true);
	mcast_mon_refresh();
}

void mcast_delete(struct omci_inst *in)
{
	struct shadow *s, *tmp;
	struct mon_cnt *c;

	list_for_each_entry_safe(s, tmp, &shadows, list)
		if (s->class_id == in->def->class_id && s->id == in->id)
			shadow_free(s);
	if (in->def->class_id != CLASS_MC_MONITOR)
		return;
	c = cnt_get(in->id, false);
	if (c)
		memset(c, 0, sizeof(*c));
}

/* MIB reset: the OLT starts again */
void mcast_mib_reset(void)
{
	struct shadow *s, *tmp;

	list_for_each_entry_safe(s, tmp, &shadows, list)
		shadow_free(s);
	memset(act, 0, sizeof(act));
	memset(cnt, 0, sizeof(cnt));
	mon_stop();
}
