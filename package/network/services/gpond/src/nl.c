// SPDX-License-Identifier: GPL-2.0
/*
 * Generic netlink client for the "gpon" family.
 *
 * Two sockets: one for synchronous requests, one subscribed to the
 * "omci" and "event" multicast groups and driven by uloop.
 */
#include <errno.h>
#include <string.h>

#include <unl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/errno.h>

#include "gpond.h"

static struct unl req;
static struct unl evt;
static struct nl_cb *evt_cb;
static struct uloop_fd evt_fd;

static int parse(struct nl_msg *msg, struct nlattr **tb, int *cmd)
{
	struct genlmsghdr *gh = nlmsg_data(nlmsg_hdr(msg));

	*cmd = gh->cmd;

	return nla_parse(tb, GPON_ATTR_MAX, genlmsg_attrdata(gh, 0),
			 genlmsg_attrlen(gh, 0), NULL);
}

static int reply_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[GPON_ATTR_MAX + 1];
	struct reply_ctx *ctx = arg;
	int cmd;

	if (!parse(msg, tb, &cmd) && ctx->cb)
		ctx->cb(tb, ctx->arg);

	return NL_SKIP;
}

struct nl_msg *nl_msg(int cmd, bool dump)
{
	return unl_genl_msg(&req, cmd, dump);
}

int nl_request(struct nl_msg *msg, nl_reply_cb cb, void *arg)
{
	struct reply_ctx ctx = { cb, arg };

	if (!msg)
		return -ENOMEM;

	return unl_request(&req, msg, reply_handler, &ctx);
}

int nl_put_u8(struct nl_msg *m, int attr, uint8_t v)
{
	if (!m)
		return -ENOMEM;

	return nla_put_u8(m, attr, v);
}

int nl_put_u16(struct nl_msg *m, int attr, uint16_t v)
{
	if (!m)
		return -ENOMEM;

	return nla_put_u16(m, attr, v);
}

int nl_put_u32(struct nl_msg *m, int attr, uint32_t v)
{
	if (!m)
		return -ENOMEM;

	return nla_put_u32(m, attr, v);
}

int nl_put_flag(struct nl_msg *m, int attr)
{
	if (!m)
		return -ENOMEM;

	return nla_put_flag(m, attr);
}

int nl_put(struct nl_msg *m, int attr, const void *data, int len)
{
	if (!m)
		return -ENOMEM;

	return nla_put(m, attr, len, data);
}

int nl_cmd_u8(int cmd, int attr, uint8_t v)
{
	struct nl_msg *m = nl_msg(cmd, false);

	if (!m)
		return -ENOMEM;
	nla_put_u8(m, attr, v);

	return nl_request(m, NULL, NULL);
}

/* ---- kernel wrappers ---- */

static int log_ret(const char *what, int ret)
{
	if (ret)
		WARN("%s: %s", what, strerror(-ret));

	return ret;
}

int kx_omci_tx(const void *data, size_t len)
{
	struct nl_msg *m = nl_msg(GPON_CMD_OMCI, false);

	gpond_hexdump("OMCI tx", data, len);
	nl_put(m, GPON_ATTR_OMCI, data, len);

	return log_ret("OMCI tx", nl_request(m, NULL, NULL));
}

int kx_gem_add(uint16_t gem, uint16_t alloc_id, bool mcast)
{
	struct nl_msg *m = nl_msg(GPON_CMD_GEM_ADD, false);

	nl_put_u16(m, GPON_ATTR_GEM, gem);
	if (mcast)
		nl_put_flag(m, GPON_ATTR_MCAST);
	else
		nl_put_u16(m, GPON_ATTR_ALLOC_ID, alloc_id);

	return log_ret("GEM add", nl_request(m, NULL, NULL));
}

/* GPON_CMD_MCAST_RX: the multicast GEM ports in the hardware table or not */
int kx_mcast_rx(bool on)
{
	struct nl_msg *m = nl_msg(GPON_CMD_MCAST_RX, false);

	nl_put_u8(m, GPON_ATTR_MCAST_RX, on);

	return nl_request(m, NULL, NULL);
}

int kx_gem_del(uint16_t gem)
{
	struct nl_msg *m = nl_msg(GPON_CMD_GEM_DEL, false);

	nl_put_u16(m, GPON_ATTR_GEM, gem);

	return log_ret("GEM del", nl_request(m, NULL, NULL));
}

int kx_gem_flush(void)
{
	return log_ret("GEM flush",
		       nl_request(nl_msg(GPON_CMD_GEM_FLUSH, false),
				  NULL, NULL));
}

static void gem_encrypt_cb(struct nlattr **tb, void *arg)
{
	struct gem_encrypt_ctx *c = arg;

	if (!tb[GPON_ATTR_GEM] || !tb[GPON_ATTR_ENCRYPT])
		return;
	if (nla_get_u16(tb[GPON_ATTR_GEM]) != c->gem)
		return;
	c->encrypt = nla_get_u8(tb[GPON_ATTR_ENCRYPT]);
	c->found = true;
}

/*
 * The encryption state of a GEM port: the kernel table dump carries it
 * (GPON_ATTR_ENCRYPT of the GPON_CMD_GET_TABLES entries).  -ENOENT: the
 * port is not in the table (yet).
 */
int kx_gem_encrypt(uint16_t gem, uint8_t *encrypt)
{
	struct gem_encrypt_ctx c = { .gem = gem };
	int ret;

	ret = nl_request(nl_msg(GPON_CMD_GET_TABLES, true), gem_encrypt_cb,
			 &c);
	if (ret)
		return ret;
	if (!c.found)
		return -ENOENT;
	*encrypt = c.encrypt;

	return 0;
}

static int alloc_cmd(int cmd, uint16_t alloc_id)
{
	struct nl_msg *m = nl_msg(cmd, false);

	nl_put_u16(m, GPON_ATTR_ALLOC_ID, alloc_id);

	return nl_request(m, NULL, NULL);
}

int kx_tcont_add(uint16_t alloc_id)
{
	return log_ret("T-CONT add", alloc_cmd(GPON_CMD_TCONT_ADD,
					       alloc_id));
}

int kx_tcont_del(uint16_t alloc_id)
{
	return log_ret("T-CONT del", alloc_cmd(GPON_CMD_TCONT_DEL,
					       alloc_id));
}

int kx_us_gem(uint16_t gem)
{
	struct nl_msg *m = nl_msg(GPON_CMD_SET_US_GEM, false);

	nl_put_u16(m, GPON_ATTR_GEM, gem);

	return log_ret("US GEM", nl_request(m, NULL, NULL));
}

int kx_us_rule(uint8_t rule, uint16_t vid, uint8_t pcp_mask, uint16_t gem)
{
	struct nl_msg *m = nl_msg(GPON_CMD_US_RULE_SET, false);

	nl_put_u8(m, GPON_ATTR_RULE, rule);
	nl_put_u16(m, GPON_ATTR_VID, vid);
	nl_put_u8(m, GPON_ATTR_PCP_MASK, pcp_mask);
	nl_put_u16(m, GPON_ATTR_GEM, gem);

	return log_ret("US rule", nl_request(m, NULL, NULL));
}

int kx_us_rule_del(uint8_t rule)
{
	return nl_cmd_u8(GPON_CMD_US_RULE_DEL, GPON_ATTR_RULE,
			 rule);
}

int kx_vid_map(uint8_t rule, uint16_t uni_vid, uint8_t pcp_mask,
	       uint16_t ani_vid, uint8_t pcp)
{
	struct nl_msg *m = nl_msg(GPON_CMD_VID_MAP_SET, false);

	nl_put_u8(m, GPON_ATTR_RULE, rule);
	nl_put_u16(m, GPON_ATTR_VID, uni_vid);
	nl_put_u8(m, GPON_ATTR_PCP_MASK, pcp_mask);
	nl_put_u16(m, GPON_ATTR_ANI_VID, ani_vid);
	nl_put_u8(m, GPON_ATTR_PCP, pcp);

	return log_ret("VID map", nl_request(m, NULL, NULL));
}

int kx_vid_map_del(uint8_t rule)
{
	return nl_cmd_u8(GPON_CMD_VID_MAP_DEL, GPON_ATTR_RULE, rule);
}

int kx_pcp_queue(const uint8_t map[8])
{
	struct nl_msg *m = nl_msg(GPON_CMD_PCP_QUEUE, false);

	nl_put(m, GPON_ATTR_PCP_MAP, map, 8);

	return log_ret("PCP queue", nl_request(m, NULL, NULL));
}

int kx_dscp_map(const uint8_t map[64], bool enable)
{
	struct nl_msg *m = nl_msg(GPON_CMD_DSCP_MAP, false);

	if (map)
		nl_put(m, GPON_ATTR_DSCP_MAP, map, 64);
	nl_put_u8(m, GPON_ATTR_DSCP_EN, enable);

	return log_ret("DSCP map", nl_request(m, NULL, NULL));
}

int kx_qos_rate(uint16_t alloc_id, uint32_t kbps, uint32_t bucket)
{
	struct nl_msg *m = nl_msg(GPON_CMD_QOS_RATE, false);

	nl_put_u16(m, GPON_ATTR_ALLOC_ID, alloc_id);
	nl_put_u32(m, GPON_ATTR_RATE, kbps);
	if (bucket)
		nl_put_u32(m, GPON_ATTR_BUCKET, bucket);

	return log_ret("QoS rate", nl_request(m, NULL, NULL));
}

int kx_onu_flag(int attr, uint8_t v)
{
	return log_ret("ONU ctrl", nl_cmd_u8(GPON_CMD_ONU_CTRL, attr, v));
}

int kx_onu_mib_reset(void)
{
	struct nl_msg *m = nl_msg(GPON_CMD_ONU_CTRL, false);

	nl_put_flag(m, GPON_ATTR_MIB_RESET);

	return log_ret("MIB reset", nl_request(m, NULL, NULL));
}

int kx_cfg_done(void)
{
	struct nl_msg *m = nl_msg(GPON_CMD_ONU_CTRL, false);

	nl_put_flag(m, GPON_ATTR_CFG_DONE);

	return log_ret("CFG done", nl_request(m, NULL, NULL));
}

static void cnt_cb(struct nlattr **tb, void *arg)
{
	uint64_t *cnt = arg;
	struct nlattr *pos;
	int rem;

	if (!tb[GPON_ATTR_CNT])
		return;
	nla_for_each_nested(pos, tb[GPON_ATTR_CNT], rem) {
		int t = nla_type(pos);

		if (t > 0 && t < __GPON_CNT_MAX)
			cnt[t] = nla_get_u64(pos);
	}
}

/* the global counters, GPON_CNT_* (totals, nothing is cleared) */
int kx_counters_global(uint64_t cnt[__GPON_CNT_MAX])
{
	memset(cnt, 0, __GPON_CNT_MAX * sizeof(*cnt));

	return nl_request(nl_msg(GPON_CMD_GET_COUNTERS, false), cnt_cb,
			  cnt);
}

/* the counters of a GEM port: rx frames, rx bytes, tx frames, tx bytes */
int kx_counters_gem(uint16_t gem, uint64_t out[4])
{
	uint64_t cnt[__GPON_CNT_MAX];
	struct nl_msg *m = nl_msg(GPON_CMD_GET_COUNTERS, false);
	int ret;

	memset(cnt, 0, sizeof(cnt));
	nl_put_u16(m, GPON_ATTR_GEM, gem);
	ret = nl_request(m, cnt_cb, cnt);
	out[0] = cnt[GPON_CNT_RX_FRAMES];
	out[1] = cnt[GPON_CNT_RX_BYTES];
	out[2] = cnt[GPON_CNT_TX_FRAMES];
	out[3] = cnt[GPON_CNT_TX_BYTES];

	return ret;
}

int kx_tod(uint32_t sf, uint32_t sec, uint32_t nsec)
{
	struct nl_msg *m = nl_msg(GPON_CMD_TOD, false);

	nl_put_u32(m, GPON_ATTR_TOD_SF, sf);
	nl_put_u32(m, GPON_ATTR_TOD_SEC, sec);
	nl_put_u32(m, GPON_ATTR_TOD_NSEC, nsec);

	return log_ret("ToD", nl_request(m, NULL, NULL));
}

/* ---- notifications ---- */

static int evt_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[GPON_ATTR_MAX + 1];
	int cmd;

	if (!parse(msg, tb, &cmd)) {
		DBG("event: cmd %d", cmd);
		event_handle(cmd, tb);
	} else {
		WARN("event: unparsable message");
	}

	return NL_OK;
}

/*
 * nl_socket_disable_seq_check() only changes the socket's own callback set;
 * evt_cb is a separate one, and without a SEQ_CHECK there libnl insists on
 * the sequence of a request, which a multicast (seq 0) never has.
 */
static int evt_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static void evt_read(struct uloop_fd *u, unsigned int events)
{
	int ret;

	/*
	 * One read per wake-up: libnl-tiny returns 0 both for a handled batch
	 * and for an empty non-blocking socket, so a "drain until error" loop
	 * would never end.  uloop is level triggered and calls again while
	 * data is queued.
	 */
	ret = nl_recvmsgs(evt.sock, evt_cb);
	if (ret < 0 && ret != -NLE_AGAIN)
		WARN("event socket: %s", nl_geterror(ret));
}

int nl_init(void)
{
	static const char * const groups[] = {
		GPON_MCGRP_OMCI_NAME,
		GPON_MCGRP_EVENT_NAME,
	};
	size_t i;

	if (unl_genl_init(&req, GPON_GENL_NAME)) {
		ERR("generic netlink family %s not found (module loaded?)",
		    GPON_GENL_NAME);
		return -ENOENT;
	}
	if (unl_genl_init(&evt, GPON_GENL_NAME)) {
		unl_free(&req);
		return -ENOENT;
	}

	/*
	 * Commands are dispatched by their number, so a daemon built against a
	 * different uapi than the module does not get an error - it gets
	 * whatever command that number has become, run for real.  Version 10
	 * to 11 removed CMD_SET_IDENTITY from the middle of the enum and
	 * shifted every number after it, which is the case this catches.
	 *
	 * unl_genl_init() has just looked the family up, so the module's
	 * answer is already here.  A module that is not loaded at all was
	 * caught above, with a different message; this one means it is there
	 * and speaks something else.
	 */
	if (req.family->gf_version != GPON_GENL_VERSION) {
		ERR("gpond speaks %s v%d, the module is v%d - mismatched "
		    "packages, refusing to start",
		    GPON_GENL_NAME, GPON_GENL_VERSION,
		    (int)req.family->gf_version);
		unl_free(&evt);
		unl_free(&req);
		return -EPROTO;
	}

	for (i = 0; i < sizeof(groups) / sizeof(groups[0]); i++) {
		if (unl_genl_subscribe(&evt, groups[i]))
			WARN("cannot join multicast group %s", groups[i]);
		else
			INFO("joined multicast group %s", groups[i]);
	}

	nl_socket_disable_seq_check(evt.sock);
	nl_socket_set_nonblocking(evt.sock);
	nl_socket_set_buffer_size(evt.sock, 1 << 20, 0);
	evt_cb = nl_cb_alloc(NL_CB_CUSTOM);
	nl_cb_set(evt_cb, NL_CB_VALID, NL_CB_CUSTOM, evt_handler, NULL);
	nl_cb_set(evt_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, evt_seq_check, NULL);

	evt_fd.fd = nl_socket_get_fd(evt.sock);
	evt_fd.cb = evt_read;
	uloop_fd_add(&evt_fd, ULOOP_READ);

	return 0;
}

void nl_done(void)
{
	uloop_fd_delete(&evt_fd);
	if (evt_cb)
		nl_cb_put(evt_cb);
	unl_free(&evt);
	unl_free(&req);
}
