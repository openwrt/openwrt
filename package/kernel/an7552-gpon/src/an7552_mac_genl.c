// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha AN7552 xPON MAC - generic netlink interface ("gpon").
 *
 * Everything the kernel keeps is dynamic (lost on deactivation or reboot):
 * GEM ports, T-CONTs, upstream classification, QoS, counters and ONU
 * control.  Persistent configuration and the OMCI MIB belong to the
 * userspace daemon, which pushes them through these commands.
 *
 * Multicast groups:
 *   "omci"  - downstream OMCI frames (always)
 *   "event" - state/alarm changes, PLOAM the kernel did not handle
 */
#include <linux/soc/airoha/airoha_xpon.h>
#include <net/genetlink.h>

#include "an7552_mac.h"
#include "uapi/gpon_genl.h"


static struct an7552_mac *genl_mac;
/*
 * True while the family is registered and behind it is a live MAC.  Distinct
 * from genl_mac, which stays set until genl_unregister_family() has drained the
 * requests already in flight: a doit handler that read genl_mac before it went
 * NULL is entitled to finish against a MAC that has not been freed yet, whereas
 * everything that calls in from the MAC has to stop at once.
 */
static bool genl_live;
static struct genl_family an7552_mac_family;

static const struct nla_policy an7552_mac_policy[GPON_ATTR_MAX + 1] = {
	[GPON_ATTR_OMCI]		= { .type = NLA_BINARY, .len = 2048 },
	[GPON_ATTR_SN]		= NLA_POLICY_EXACT_LEN(GPON_SN_LEN),
	[GPON_ATTR_PASSWORD]	= NLA_POLICY_EXACT_LEN(GPON_PASSWD_LEN),
	[GPON_ATTR_GEM]		= { .type = NLA_U16 },
	[GPON_ATTR_ALLOC_ID]	= NLA_POLICY_MAX(NLA_U16, 0xfff),
	[GPON_ATTR_MCAST]	= { .type = NLA_FLAG },
	[GPON_ATTR_RULE]		= NLA_POLICY_MAX(NLA_U8,
							 GPON_MAX_US_RULES - 1),
	[GPON_ATTR_VID]		= NLA_POLICY_MAX(NLA_U16,
							 GPON_US_VID_ANY),
	[GPON_ATTR_PCP_MASK]	= { .type = NLA_U8 },
	[GPON_ATTR_PCP_MAP]	= NLA_POLICY_EXACT_LEN(8),
	[GPON_ATTR_SCHED_MODE]	= NLA_POLICY_MAX(NLA_U8, 7),
	[GPON_ATTR_WEIGHTS]	= NLA_POLICY_EXACT_LEN(8 * sizeof(u16)),
	[GPON_ATTR_RATE]		= { .type = NLA_U32 },
	[GPON_ATTR_BUCKET]	= { .type = NLA_U32 },
	[GPON_ATTR_CLEAR]	= { .type = NLA_FLAG },
	[GPON_ATTR_EMERGENCY]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_SILENCE]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_MIB_RESET]	= { .type = NLA_FLAG },
	[GPON_ATTR_MIB_SYNCED]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_UP_OMCI]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_UP_DATA]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_SD]		= { .type = NLA_U8 },
	[GPON_ATTR_SF]		= { .type = NLA_U8 },
	[GPON_ATTR_TO1]		= { .type = NLA_U32 },
	[GPON_ATTR_TO2]		= { .type = NLA_U32 },
	[GPON_ATTR_MODE]		= { .type = NLA_U8 },
	[GPON_ATTR_ADMIN_DOWN]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_PLOAM]	= NLA_POLICY_EXACT_LEN(GPON_PLOAM_LEN),
	[GPON_ATTR_STATE]	= NLA_POLICY_RANGE(NLA_U8, 1, 7),
	[GPON_ATTR_ONU_ID]	= { .type = NLA_U8 },
	[GPON_ATTR_EQD]		= { .type = NLA_U32 },
	[GPON_ATTR_OMCC]		= { .type = NLA_U16 },
	[GPON_ATTR_ENCRYPT]	= NLA_POLICY_MAX(NLA_U8, 1),
	[GPON_ATTR_CFG_DONE]	= { .type = NLA_FLAG },
	[GPON_ATTR_TOD_SF]	= { .type = NLA_U32 },
	[GPON_ATTR_TOD_SEC]	= { .type = NLA_U32 },
	[GPON_ATTR_TOD_NSEC]	= { .type = NLA_U32 },
	[GPON_ATTR_DSCP_MAP]	= NLA_POLICY_EXACT_LEN(GPON_DSCP_NUM),
	[GPON_ATTR_DSCP_EN]	= NLA_POLICY_MAX(NLA_U8, 1),
	/* measurements: replies only, never read from a request */
	[GPON_ATTR_SPF_CNT]	= { .type = NLA_U32 },
	[GPON_ATTR_ACTIVATE]	= { .type = NLA_FLAG },
	[GPON_ATTR_DEACTIVATE]	= { .type = NLA_FLAG },
	[GPON_ATTR_ANI_VID]	= NLA_POLICY_RANGE(NLA_U16, 1, 4095),
	[GPON_ATTR_PCP]		= NLA_POLICY_MAX(NLA_U8, GPON_PCP_KEEP),
	[GPON_ATTR_MCAST_RX]	= NLA_POLICY_MAX(NLA_U8, 1),
};

static u16 attr_u16(struct genl_info *info, int a)
{
	return nla_get_u16(info->attrs[a]);
}

static bool has(struct genl_info *info, int a)
{
	return !!info->attrs[a];
}

/*
 * The answer of every command that replies: a sk_buff that already carries
 * the generic netlink header, ready for the attributes.  NULL when there is
 * no memory or no room for the header, which the callers report as -ENOMEM
 * (that is all nl_reply() can tell apart) or as -EMSGSIZE, as they did before.
 */
static struct sk_buff *nl_reply(struct genl_info *info, u8 cmd, void **hdr)
{
	struct sk_buff *msg = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);

	if (!msg)
		return NULL;
	*hdr = genlmsg_put_reply(msg, info, &an7552_mac_family, 0, cmd);
	if (!*hdr) {
		nlmsg_free(msg);
		return NULL;
	}

	return msg;
}

/* ---- OMCI / identity ---- */

static int nl_omci(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *a = info->attrs[GPON_ATTR_OMCI];
	int ret;

	if (!a)
		return -EINVAL;

	ret = an7552_mac_omci_tx(genl_mac, nla_data(a), nla_len(a));
	an7552_mac_dbg(genl_mac, AN7552_DBG_MAC, "nl_omci len %d: %d\n",
		       nla_len(a), ret);

	return ret;
}

/* ---- GEM / T-CONT / classification / QoS ---- */

static int nl_gem_add(struct sk_buff *skb, struct genl_info *info)
{
	if (!has(info, GPON_ATTR_GEM) ||
	    (!has(info, GPON_ATTR_ALLOC_ID) &&
	     !has(info, GPON_ATTR_MCAST)))
		return -EINVAL;

	return an7552_mac_gem_add(genl_mac,
				   attr_u16(info, GPON_ATTR_GEM),
				   has(info, GPON_ATTR_ALLOC_ID) ?
				   attr_u16(info, GPON_ATTR_ALLOC_ID) :
				   GPON_UNASSIGN_ALLOC_ID,
				   has(info, GPON_ATTR_MCAST));
}

static int nl_gem_flush(struct sk_buff *skb, struct genl_info *info)
{
	return an7552_mac_gem_flush(genl_mac);
}

/*
 * The commands that are one required attribute and one call: the handler is
 * the table row, so none of them is written out.
 */
struct nl_one {
	u8	cmd;			/* GPON_CMD_* */
	u16	attr;			/* required attribute */
	u8	get;			/* NLA_U8 or NLA_U16 */
	int (*fn)(struct an7552_mac *, u16);
};

static int nl_one_doit(struct genl_info *info, const struct nl_one *o)
{
	struct nlattr *a = info->attrs[o->attr];
	u16 v;

	if (!a)
		return -EINVAL;
	v = o->get == NLA_U8 ? nla_get_u8(a) : nla_get_u16(a);

	return o->fn(genl_mac, v);
}

static const struct nl_one nl_one_ops[] = {
	{ GPON_CMD_GEM_DEL, GPON_ATTR_GEM, NLA_U16,
	  an7552_mac_gem_del },
	{ GPON_CMD_SET_US_GEM, GPON_ATTR_GEM, NLA_U16,
	  an7552_mac_us_default },
	{ GPON_CMD_TCONT_ADD, GPON_ATTR_ALLOC_ID, NLA_U16,
	  an7552_mac_tcont_add },
	{ GPON_CMD_TCONT_DEL, GPON_ATTR_ALLOC_ID, NLA_U16,
	  an7552_mac_tcont_del },
	{ GPON_CMD_US_RULE_DEL, GPON_ATTR_RULE, NLA_U8,
	  an7552_mac_us_rule_del },
	{ GPON_CMD_VID_MAP_DEL, GPON_ATTR_RULE, NLA_U8,
	  an7552_mac_vid_map_del },
	{ GPON_CMD_MCAST_RX, GPON_ATTR_MCAST_RX, NLA_U8,
	  an7552_mac_mcast_rx },
};

/* the generic netlink handler, dispatched on the command it answers */
static int nl_one_table(struct sk_buff *skb, struct genl_info *info)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(nl_one_ops); i++)
		if (nl_one_ops[i].cmd == info->genlhdr->cmd)
			return nl_one_doit(info, &nl_one_ops[i]);

	return -EINVAL;
}

static int nl_us_rule_set(struct sk_buff *skb, struct genl_info *info)
{
	if (!has(info, GPON_ATTR_RULE) ||
	    !has(info, GPON_ATTR_GEM))
		return -EINVAL;

	return an7552_mac_us_rule_set(genl_mac,
		nla_get_u8(info->attrs[GPON_ATTR_RULE]),
		has(info, GPON_ATTR_VID) ?
		attr_u16(info, GPON_ATTR_VID) : GPON_US_VID_ANY,
		has(info, GPON_ATTR_PCP_MASK) ?
		nla_get_u8(info->attrs[GPON_ATTR_PCP_MASK]) : 0,
		attr_u16(info, GPON_ATTR_GEM));
}

static int nl_vid_map_set(struct sk_buff *skb, struct genl_info *info)
{
	if (!has(info, GPON_ATTR_RULE) || !has(info, GPON_ATTR_VID) ||
	    !has(info, GPON_ATTR_ANI_VID))
		return -EINVAL;

	return an7552_mac_vid_map_set(genl_mac,
		nla_get_u8(info->attrs[GPON_ATTR_RULE]),
		attr_u16(info, GPON_ATTR_VID),
		has(info, GPON_ATTR_PCP_MASK) ?
		nla_get_u8(info->attrs[GPON_ATTR_PCP_MASK]) : 0,
		attr_u16(info, GPON_ATTR_ANI_VID),
		has(info, GPON_ATTR_PCP) ?
		nla_get_u8(info->attrs[GPON_ATTR_PCP]) : GPON_PCP_KEEP);
}

static int nl_pcp_queue(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *a = info->attrs[GPON_ATTR_PCP_MAP];

	if (!a)
		return -EINVAL;

	return an7552_mac_pcp_queue(genl_mac, nla_data(a));
}

static int nl_dscp_map(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *map = info->attrs[GPON_ATTR_DSCP_MAP];
	struct nlattr *en = info->attrs[GPON_ATTR_DSCP_EN];

	if (!map && !en)
		return -EINVAL;

	return an7552_mac_dscp_map(genl_mac, map ? nla_data(map) : NULL,
				    en ? nla_get_u8(en) : -1);
}

static int nl_qos_sched(struct sk_buff *skb, struct genl_info *info)
{
	u16 w[8] = { };

	if (!has(info, GPON_ATTR_ALLOC_ID) ||
	    !has(info, GPON_ATTR_SCHED_MODE))
		return -EINVAL;
	if (has(info, GPON_ATTR_WEIGHTS))
		nla_memcpy(w, info->attrs[GPON_ATTR_WEIGHTS], sizeof(w));

	return an7552_mac_tcont_sched(genl_mac,
		attr_u16(info, GPON_ATTR_ALLOC_ID),
		nla_get_u8(info->attrs[GPON_ATTR_SCHED_MODE]), w);
}

static int nl_qos_rate(struct sk_buff *skb, struct genl_info *info)
{
	if (!has(info, GPON_ATTR_ALLOC_ID) ||
	    !has(info, GPON_ATTR_RATE))
		return -EINVAL;

	return an7552_mac_tcont_rate(genl_mac,
		attr_u16(info, GPON_ATTR_ALLOC_ID),
		nla_get_u32(info->attrs[GPON_ATTR_RATE]),
		has(info, GPON_ATTR_BUCKET) ?
		nla_get_u32(info->attrs[GPON_ATTR_BUCKET]) : 0);
}

/* ---- state / counters / tables ---- */

static int fill_state(struct an7552_mac *x, struct sk_buff *msg)
{
	if (nla_put_u8(msg, GPON_ATTR_STATE, x->state) ||
	    nla_put_u8(msg, GPON_ATTR_ONU_ID, x->onu_id) ||
	    nla_put_u16(msg, GPON_ATTR_OMCC, x->omcc) ||
	    nla_put_u32(msg, GPON_ATTR_ALARMS, x->alarms) ||
	    nla_put_u32(msg, GPON_ATTR_EQD, x->eqd) ||
	    nla_put_u8(msg, GPON_ATTR_MODE, x->mode) ||
	    nla_put_u8(msg, GPON_ATTR_EMERGENCY, x->emergency) ||
	    nla_put_u8(msg, GPON_ATTR_SILENCE, x->silence) ||
	    nla_put_u8(msg, GPON_ATTR_MIB_SYNCED, x->mib_synced) ||
	    nla_put_u8(msg, GPON_ATTR_ADMIN_DOWN, x->admin_down) ||
	    nla_put_u8(msg, GPON_ATTR_SD, x->sd_thld) ||
	    nla_put_u8(msg, GPON_ATTR_SF, x->sf_thld) ||
	    nla_put_u32(msg, GPON_ATTR_TO1, x->to1_ms) ||
	    nla_put_u32(msg, GPON_ATTR_TO2, x->to2_ms) ||
	    /*
	     * The superframe counter runs whether the MAC is running or not,
	     * so it is read outside x->lock like every other bare register
	     * read (mac_rr()).
	     */
	    nla_put_u32(msg, GPON_ATTR_SPF_CNT,
			an7552_mac_get_superframe(x)) ||
	    nla_put_u8(msg, GPON_ATTR_TRAFFIC_UP,
		       an7552_mac_traffic_up(x)))
		return -EMSGSIZE;

	return 0;
}

static int nl_get_state(struct sk_buff *skb, struct genl_info *info)
{
	struct an7552_mac *x = genl_mac;
	struct sk_buff *msg;
	void *hdr;
	int ret;

	msg = nl_reply(info, GPON_CMD_GET_STATE, &hdr);
	if (!msg)
		return -ENOMEM;
	mutex_lock(&x->lock);
	ret = fill_state(x, msg);
	mutex_unlock(&x->lock);
	if (ret) {
		nlmsg_free(msg);
		return ret;
	}
	genlmsg_end(msg, hdr);

	return genlmsg_reply(msg, info);
}

static int put_cnt(struct sk_buff *msg, int type, u64 val)
{
	return nla_put_u64_64bit(msg, type, val, GPON_CNT_PAD);
}

static int fill_global_counters(struct an7552_mac *x, struct sk_buff *msg)
{
	struct an7552_phy_stats ps = { };
	static const struct {
		int type;
		u32 reg;
	} dbg[] = {
		{ GPON_CNT_RX_GEM, DBG_RX_GEM_CNT },
		{ GPON_CNT_RX_CRC_ERR, DBG_RX_CRC_ERR_CNT },
		{ GPON_CNT_RX_GTC, DBG_RX_GTC_CNT },
		{ GPON_CNT_TX_GEM, DBG_TX_GEM_CNT },
		{ GPON_CNT_TX_BURST, DBG_TX_BST_CNT },
		{ GPON_CNT_HEC_ONE, DBG_GEM_HEC_ONE_ERR_CNT },
		{ GPON_CNT_HEC_TWO, DBG_GEM_HEC_TWO_ERR_CNT },
		{ GPON_CNT_HEC_UC, DBG_GEM_HEC_UC_ERR_CNT },
	};
	int i;

	/*
	 * get_stats() reads the BIP counter to clear: keep what it took for
	 * the BER interval, which would otherwise lose it (every poll of the
	 * counters, e.g. the OMCI PM engine, would eat SD/SF and REI input).
	 */
	an7552_phy_get_stats(x->phy, &ps);
	x->bip_pending += ps.bip_errors;

	if (put_cnt(msg, GPON_CNT_PLOAM_RX, x->stats.ploam_rx) ||
	    put_cnt(msg, GPON_CNT_PLOAM_TX, x->stats.ploam_tx) ||
	    put_cnt(msg, GPON_CNT_PLOAM_DROP, x->stats.ploam_rx_drop) ||
	    put_cnt(msg, GPON_CNT_OMCI_RX, x->stats.omci_rx) ||
	    put_cnt(msg, GPON_CNT_OMCI_TX, x->stats.omci_tx) ||
	    put_cnt(msg, GPON_CNT_ACTIVATIONS, x->activations) ||
	    put_cnt(msg, GPON_CNT_BIP, x->bip_total + x->bip_pending) ||
	    put_cnt(msg, GPON_CNT_FEC_CORR_BYTES, ps.fec_corrected_bytes) ||
	    put_cnt(msg, GPON_CNT_FEC_CORR_CW, ps.fec_corrected_codewords) ||
	    put_cnt(msg, GPON_CNT_FEC_UNCORR_CW,
		    ps.fec_uncorrectable_codewords) ||
	    put_cnt(msg, GPON_CNT_FEC_TOTAL_CW, ps.fec_total_codewords) ||
	    put_cnt(msg, GPON_CNT_FEC_SECONDS, ps.fec_seconds) ||
	    put_cnt(msg, GPON_CNT_LOF, ps.lof_count) ||
	    put_cnt(msg, GPON_CNT_RX_DROP_GEM, x->stats.rx_drop_gem) ||
	    put_cnt(msg, GPON_CNT_ROGUE, x->stats.rogue))
		return -EMSGSIZE;

	for (i = 0; i < ARRAY_SIZE(dbg); i++)
		if (put_cnt(msg, dbg[i].type, mac_rr(x, dbg[i].reg)))
			return -EMSGSIZE;

	return 0;
}

static int nl_get_counters(struct sk_buff *skb, struct genl_info *info)
{
	struct an7552_mac *x = genl_mac;
	u64 cnt[GPON_GEM_CNT_MAX];
	struct nlattr *nest;
	struct sk_buff *msg;
	void *hdr;
	int ret = 0;

	/* reading is for everyone, clearing the PM base is not */
	if (has(info, GPON_ATTR_CLEAR) &&
	    !netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	if (has(info, GPON_ATTR_GEM)) {
		u16 gem = attr_u16(info, GPON_ATTR_GEM);

		if (gem >= GPON_MAX_GEM_ID)
			return -EINVAL;
		ret = an7552_mac_gem_counters(x, gem,
					       has(info, GPON_ATTR_CLEAR),
					       cnt);
	} else if (has(info, GPON_ATTR_ALLOC_ID)) {
		ret = an7552_mac_tcont_counters(x,
				attr_u16(info, GPON_ATTR_ALLOC_ID),
				has(info, GPON_ATTR_CLEAR), cnt);
	}
	if (ret)
		return ret;

	msg = nl_reply(info, GPON_CMD_GET_COUNTERS, &hdr);
	if (!msg)
		return -ENOMEM;

	if (has(info, GPON_ATTR_GEM) &&
	    nla_put_u16(msg, GPON_ATTR_GEM,
			attr_u16(info, GPON_ATTR_GEM)))
		goto nospace;
	if (!has(info, GPON_ATTR_GEM) &&
	    has(info, GPON_ATTR_ALLOC_ID) &&
	    nla_put_u16(msg, GPON_ATTR_ALLOC_ID,
			attr_u16(info, GPON_ATTR_ALLOC_ID)))
		goto nospace;
	nest = nla_nest_start(msg, GPON_ATTR_CNT);
	if (!nest)
		goto nospace;
	if (has(info, GPON_ATTR_GEM) ||
	    has(info, GPON_ATTR_ALLOC_ID)) {
		if (put_cnt(msg, GPON_CNT_RX_FRAMES,
			    cnt[GPON_GEM_CNT_RX_FRAMES]) ||
		    put_cnt(msg, GPON_CNT_RX_BYTES,
			    cnt[GPON_GEM_CNT_RX_BYTES]) ||
		    put_cnt(msg, GPON_CNT_TX_FRAMES,
			    cnt[GPON_GEM_CNT_TX_FRAMES]) ||
		    put_cnt(msg, GPON_CNT_TX_BYTES,
			    cnt[GPON_GEM_CNT_TX_BYTES]))
			goto nospace;
	} else {
		mutex_lock(&x->lock);
		ret = fill_global_counters(x, msg);
		mutex_unlock(&x->lock);
		if (ret)
			goto nospace;
	}
	nla_nest_end(msg, nest);
	genlmsg_end(msg, hdr);

	return genlmsg_reply(msg, info);

nospace:
	nlmsg_free(msg);
	return -EMSGSIZE;
}

static int nl_clear_counters(struct sk_buff *skb, struct genl_info *info)
{
	an7552_mac_clear_counters(genl_mac);

	return 0;
}

/* one message per valid T-CONT and GEM port */
static int nl_get_tables(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct an7552_mac *x = genl_mac;
	long pos = cb->args[0];
	void *hdr;

	mutex_lock(&x->lock);
	for (; pos < GPON_MAX_TCONT + GPON_MAX_GEM_ID; pos++) {
		bool tcont = pos < GPON_MAX_TCONT;
		long i = tcont ? pos : pos - GPON_MAX_TCONT;
		struct an7552_gpon_gem *g = tcont ? NULL : &x->gem[i];

		if (tcont ? !(x->tcont_valid & BIT(i)) : !g->valid)
			continue;

		hdr = genlmsg_put(skb, NETLINK_CB(cb->skb).portid,
				  cb->nlh->nlmsg_seq, &an7552_mac_family,
				  NLM_F_MULTI, GPON_CMD_GET_TABLES);
		if (!hdr)
			break;
		if (tcont ?
		    (nla_put_u8(skb, GPON_ATTR_CHANNEL, i) ||
		     nla_put_u16(skb, GPON_ATTR_ALLOC_ID,
				 x->tcont_alloc[i])) :
		    (nla_put_u16(skb, GPON_ATTR_GEM, i) ||
		     nla_put_u16(skb, GPON_ATTR_ALLOC_ID, g->alloc_id) ||
		     nla_put_u8(skb, GPON_ATTR_CHANNEL, g->channel) ||
		     nla_put_u8(skb, GPON_ATTR_ENCRYPT, g->encrypt) ||
		     (g->channel == GPON_MULTICAST_CHANNEL &&
		      nla_put_flag(skb, GPON_ATTR_MCAST)) ||
		     (i == x->omcc &&
		      nla_put_u16(skb, GPON_ATTR_OMCC, i)))) {
			genlmsg_cancel(skb, hdr);
			break;
		}
		genlmsg_end(skb, hdr);
	}
	mutex_unlock(&x->lock);
	cb->args[0] = pos;

	return skb->len;
}

/* ---- ONU control ---- */

static int nl_onu_ctrl(struct sk_buff *skb, struct genl_info *info)
{
	struct an7552_mac *x = genl_mac;
	struct nlattr **a = info->attrs;
	int ret = 0;

	if (a[GPON_ATTR_MODE])
		ret = an7552_mac_set_mode(x,
					   nla_get_u8(a[GPON_ATTR_MODE]));
	if (!ret && a[GPON_ATTR_MIB_RESET])
		ret = an7552_mac_mib_reset(x);
	if (!ret && a[GPON_ATTR_EMERGENCY])
		ret = an7552_mac_set_emergency(x,
				nla_get_u8(a[GPON_ATTR_EMERGENCY]));
	if (!ret && a[GPON_ATTR_SILENCE])
		ret = an7552_mac_set_silence(x,
				nla_get_u8(a[GPON_ATTR_SILENCE]));
	if (!ret && (a[GPON_ATTR_SD] || a[GPON_ATTR_SF]))
		ret = an7552_mac_set_sd_sf(x,
			a[GPON_ATTR_SD] ?
			nla_get_u8(a[GPON_ATTR_SD]) : 0,
			a[GPON_ATTR_SF] ?
			nla_get_u8(a[GPON_ATTR_SF]) : 0);
	if (!ret && (a[GPON_ATTR_TO1] || a[GPON_ATTR_TO2]))
		ret = an7552_mac_set_timers(x,
			a[GPON_ATTR_TO1] ?
			nla_get_u32(a[GPON_ATTR_TO1]) : 0,
			a[GPON_ATTR_TO2] ?
			nla_get_u32(a[GPON_ATTR_TO2]) : 0);
	if (!ret && a[GPON_ATTR_CFG_DONE])
		ret = an7552_mac_cfg_done(x);
	if (!ret && a[GPON_ATTR_ADMIN_DOWN])
		ret = an7552_mac_set_admin(x,
				nla_get_u8(a[GPON_ATTR_ADMIN_DOWN]));
	/*
	 * Identity, then the gate, and in that order on purpose: storing an
	 * identity un-arms, so a daemon that sent { SN, ACTIVATE } in one
	 * message would close the gate it just opened had this been the other
	 * way round.  One message is one atomic ask, which is the whole
	 * reason these attributes moved here from CMD_SET_IDENTITY.  A
	 * DEACTIVATE in the same message is applied last, so a message that
	 * carries both ends with the gate closed.
	 */
	if (!ret && a[GPON_ATTR_SN])
		ret = an7552_mac_set_identity(x, nla_data(a[GPON_ATTR_SN]),
					      false);
	if (!ret && a[GPON_ATTR_PASSWORD])
		ret = an7552_mac_set_identity(x,
					      nla_data(a[GPON_ATTR_PASSWORD]),
					      true);
	if (!ret && a[GPON_ATTR_ACTIVATE])
		ret = an7552_mac_activate(x);
	/* an explicit "do not announce" wins over a gate that is already open */
	if (!ret && a[GPON_ATTR_DEACTIVATE])
		ret = an7552_mac_deactivate(x);
	if (ret)
		return ret;

	/* xmcs_set_mib_flag() / xmcs_set_up_traffic(): software flags */
	if (a[GPON_ATTR_MIB_SYNCED])
		WRITE_ONCE(x->mib_synced,
			   nla_get_u8(a[GPON_ATTR_MIB_SYNCED]));
	if (a[GPON_ATTR_UP_OMCI])
		WRITE_ONCE(x->up_omci, nla_get_u8(a[GPON_ATTR_UP_OMCI]));
	if (a[GPON_ATTR_UP_DATA])
		WRITE_ONCE(x->up_data, nla_get_u8(a[GPON_ATTR_UP_DATA]));

	return 0;
}

/* OLT-G Time of day information (G.988 9.12.2), applied by the MAC */
static int nl_tod(struct sk_buff *skb, struct genl_info *info)
{
	struct an7552_mac *x = genl_mac;
	struct nlattr **a = info->attrs;
	struct sk_buff *msg;
	u32 sec, nsec;
	void *hdr;
	int ret;

	if (a[GPON_ATTR_TOD_SF]) {
		if (!a[GPON_ATTR_TOD_SEC] || !a[GPON_ATTR_TOD_NSEC])
			return -EINVAL;
		ret = an7552_mac_set_tod(x,
				nla_get_u32(a[GPON_ATTR_TOD_SF]),
				nla_get_u32(a[GPON_ATTR_TOD_SEC]),
				nla_get_u32(a[GPON_ATTR_TOD_NSEC]));
		if (ret)
			return ret;
	}

	msg = nl_reply(info, GPON_CMD_TOD, &hdr);
	if (!msg)
		return -ENOMEM;
	an7552_mac_get_tod(x, &sec, &nsec);
	if (nla_put_u32(msg, GPON_ATTR_TOD_SEC, sec) ||
	    nla_put_u32(msg, GPON_ATTR_TOD_NSEC, nsec)) {
		nlmsg_free(msg);
		return -EMSGSIZE;
	}
	genlmsg_end(msg, hdr);

	return genlmsg_reply(msg, info);
}

#define OP(_cmd, _doit, _flags) \
	{ .cmd = (_cmd), .doit = (_doit), .flags = (_flags) }

static const struct genl_small_ops an7552_mac_ops[] = {
	OP(GPON_CMD_OMCI, nl_omci, GENL_ADMIN_PERM),
	OP(GPON_CMD_GEM_ADD, nl_gem_add, GENL_ADMIN_PERM),
	OP(GPON_CMD_GEM_DEL, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_SET_US_GEM, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_GET_STATE, nl_get_state, 0),
	OP(GPON_CMD_GEM_FLUSH, nl_gem_flush, GENL_ADMIN_PERM),
	OP(GPON_CMD_TCONT_ADD, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_TCONT_DEL, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_US_RULE_SET, nl_us_rule_set, GENL_ADMIN_PERM),
	OP(GPON_CMD_US_RULE_DEL, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_PCP_QUEUE, nl_pcp_queue, GENL_ADMIN_PERM),
	OP(GPON_CMD_QOS_SCHED, nl_qos_sched, GENL_ADMIN_PERM),
	OP(GPON_CMD_QOS_RATE, nl_qos_rate, GENL_ADMIN_PERM),
	OP(GPON_CMD_GET_COUNTERS, nl_get_counters, 0),
	OP(GPON_CMD_CLEAR_COUNTERS, nl_clear_counters, GENL_ADMIN_PERM),
	OP(GPON_CMD_ONU_CTRL, nl_onu_ctrl, GENL_ADMIN_PERM),
	{
		.cmd	= GPON_CMD_GET_TABLES,
		.dumpit	= nl_get_tables,
	},
	OP(GPON_CMD_TOD, nl_tod, GENL_ADMIN_PERM),
	OP(GPON_CMD_DSCP_MAP, nl_dscp_map, GENL_ADMIN_PERM),
	OP(GPON_CMD_VID_MAP_SET, nl_vid_map_set, GENL_ADMIN_PERM),
	OP(GPON_CMD_VID_MAP_DEL, nl_one_table, GENL_ADMIN_PERM),
	OP(GPON_CMD_MCAST_RX, nl_one_table, GENL_ADMIN_PERM),
};

static const struct genl_multicast_group an7552_mac_mcgrps[] = {
	[GPON_MCGRP_OMCI]	= { .name = GPON_MCGRP_OMCI_NAME, },
	[GPON_MCGRP_EVENT]	= { .name = GPON_MCGRP_EVENT_NAME, },
};

static struct genl_family an7552_mac_family __ro_after_init = {
	.name		= GPON_GENL_NAME,
	.version	= GPON_GENL_VERSION,
	.maxattr	= GPON_ATTR_MAX,
	.policy		= an7552_mac_policy,
	.module		= THIS_MODULE,
	.small_ops	= an7552_mac_ops,
	.n_small_ops	= ARRAY_SIZE(an7552_mac_ops),
	.resv_start_op	= __GPON_CMD_MAX,
	.mcgrps		= an7552_mac_mcgrps,
	.n_mcgrps	= ARRAY_SIZE(an7552_mac_mcgrps),
};

/* ---- notifications ---- */

static void mcast_data(int cmd, int grp, int attr, const void *data,
		       size_t len, gfp_t gfp)
{
	struct sk_buff *msg;
	void *hdr;
	int ret;

	/* the OMCI and PLOAM callers run from the NAPI and the PLOAM IRQ, which
	 * outlive the family at teardown */
	if (!smp_load_acquire(&genl_live))
		return;

	msg = genlmsg_new(nla_total_size(len), gfp);
	if (!msg)
		return;

	hdr = genlmsg_put(msg, 0, 0, &an7552_mac_family, 0, cmd);
	if (!hdr || nla_put(msg, attr, len, data)) {
		nlmsg_free(msg);
		return;
	}
	genlmsg_end(msg, hdr);
	ret = genlmsg_multicast(&an7552_mac_family, msg, 0, grp, gfp);
	an7552_mac_dbg(genl_mac, AN7552_DBG_MAC, "mcast cmd %d grp %d len %zu: %d\n",
		       cmd, grp, len, ret);
}

/* Called from the airoha-eth NAPI context; consumes @skb. */
void an7552_mac_genl_omci_rx(struct an7552_mac *x, struct sk_buff *skb)
{
	mcast_data(GPON_CMD_OMCI, GPON_MCGRP_OMCI,
		   GPON_ATTR_OMCI, skb->data, skb->len, GFP_ATOMIC);
	dev_kfree_skb_any(skb);
}

/* ToD at the first 1PPS after an update (threaded IRQ) */
void an7552_mac_genl_tod(struct an7552_mac *x, u32 sec, u32 nsec)
{
	struct sk_buff *msg;
	void *hdr;

	if (!smp_load_acquire(&genl_live))
		return;

	msg = genlmsg_new(2 * nla_total_size(sizeof(u32)), GFP_KERNEL);
	if (!msg)
		return;
	hdr = genlmsg_put(msg, 0, 0, &an7552_mac_family, 0,
			  GPON_CMD_TOD);
	if (!hdr || nla_put_u32(msg, GPON_ATTR_TOD_SEC, sec) ||
	    nla_put_u32(msg, GPON_ATTR_TOD_NSEC, nsec)) {
		nlmsg_free(msg);
		return;
	}
	genlmsg_end(msg, hdr);
	genlmsg_multicast(&an7552_mac_family, msg, 0, GPON_MCGRP_EVENT,
			  GFP_KERNEL);
}

/* mode 0: a downstream PLOAM the kernel did not act on */
void an7552_mac_genl_ploam_unhandled(struct an7552_mac *x, const u8 *msg)
{
	mcast_data(GPON_CMD_PLOAM, GPON_MCGRP_EVENT,
		   GPON_ATTR_PLOAM, msg, GPON_PLOAM_LEN, GFP_KERNEL);
}

/* State/alarm change event; called with x->lock held. */
void an7552_mac_genl_state(struct an7552_mac *x)
{
	struct sk_buff *msg;
	void *hdr;

	if (!smp_load_acquire(&genl_live))
		return;

	msg = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return;

	hdr = genlmsg_put(msg, 0, 0, &an7552_mac_family, 0,
			  GPON_CMD_GET_STATE);
	if (!hdr || fill_state(x, msg)) {
		nlmsg_free(msg);
		return;
	}
	genlmsg_end(msg, hdr);
	genlmsg_multicast(&an7552_mac_family, msg, 0, GPON_MCGRP_EVENT,
			  GFP_KERNEL);
}

int an7552_mac_genl_init(struct an7552_mac *x)
{
	int ret;

	if (READ_ONCE(genl_mac))
		return -EBUSY;

	/*
	 * The MAC is in place before the family exists: registering sends
	 * CTRL_CMD_NEWFAMILY at once, and a daemon that reacts to it with a
	 * request must not find genl_mac NULL in the doit handler.
	 */
	WRITE_ONCE(genl_mac, x);
	ret = genl_register_family(&an7552_mac_family);
	if (ret) {
		WRITE_ONCE(genl_mac, NULL);
		return ret;
	}
	/* only announce events once the family is there */
	smp_store_release(&genl_live, true);

	return 0;
}

void an7552_mac_genl_exit(void)
{
	/*
	 * Order matters, and both halves are load-bearing:
	 *
	 *  - genl_live goes down first, so that anything called in from the MAC
	 *    (a PHY event, a PLOAM, a dying gasp) stops at its own entry rather
	 *    than reaching a family that is on its way out.
	 *  - genl_unregister_family() waits for the requests that are already in
	 *    flight - genl_rcv_msg() takes the family lock - so a doit handler
	 *    that has already read genl_mac runs to the end against a MAC that is
	 *    still alive.  Clearing genl_mac first, as this used to, left the
	 *    window in between: a doit handler could read NULL and dereference it,
	 *    and a multicast could land on a half-torn-down family.
	 *  - genl_mac goes to NULL last, and it is what the multicast paths test,
	 *    so they are already silent by the time it reads NULL.
	 */
	smp_store_release(&genl_live, false);
	genl_unregister_family(&an7552_mac_family);
	WRITE_ONCE(genl_mac, NULL);
}
