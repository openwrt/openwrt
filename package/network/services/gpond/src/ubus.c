// SPDX-License-Identifier: GPL-2.0
/*
 * ubus object "gpon".
 *
 *   status                     kernel state, OMCI summary (with the
 *                              active OMCI alarms)
 *   counters [gem|alloc_id] [clear]
 *                              alloc_id: sum of the GEM ports of a T-CONT
 *   tables                     GEM/T-CONT tables
 *   config                     current configuration (without password)
 *   submit                     keep what the OLT built: the MIB to
 *                              /usr/lib/gpon/gpon.json, the UCI sections
 *                              derived from it committed
 *   factory                    remove /usr/lib/gpon/gpon.json and every
 *                              section of /etc/config that gpond wrote,
 *                              default MIB, then the PON link goes down and
 *                              the daemon stops
 *   reload                     re-read /etc/config/gpon and apply
 *   restart                    apply and restart the PON link
 *   mib                        OMCI MIB dump (with active alarms, ARC)
 *   pm                         OMCI PM: interval, per instance the running
 *                              and the last interval counters, thresholds,
 *                              raised TCAs
 *   alarms                     active OMCI alarms, alarm sources (kernel
 *                              alarm bitmap, EN7571 hwmon, UNI links)
 *   eth                        bridges, VLAN devices, tc rules, L3 interfaces,
 *                              upstream rules, policers built from the MIB
 *   inject                     {"frame":"hex"} or {"replay":true}
 *
 * Notifications: "state" on the object and the "gpon.state" event
 * ("traffic_up" once the configuration is done and a data GEM exists),
 * "tod" and "gpon.tod" when the MAC took a new time of day over.
 */
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <libubus.h>
#include <netlink/attr.h>
#include <netlink/msg.h>

#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"

static struct ubus_context *ctx;

/*
 * blobmsg_get_u32() makes -fanalyzer report a NULL dereference inside the
 * libubox header for any call (even a guarded one); same value, read here
 */
static uint32_t attr_u32(struct blob_attr *a)
{
	const void *d = a ? blobmsg_data(a) : NULL;
	uint32_t v = 0;

	if (d)
		memcpy(&v, d, sizeof(v));

	return ntohl(v);
}
static struct blob_buf b;

/* ---- kernel attributes -> blobmsg ---- */

static const struct kattr_desc kattrs[] = {
	{ GPON_ATTR_MODE, "mode", K_U8 },
	{ GPON_ATTR_STATE, "state", K_U8 },
	{ GPON_ATTR_ONU_ID, "onu_id", K_U8 },
	{ GPON_ATTR_OMCC, "omcc", K_U16 },
	{ GPON_ATTR_ALARMS, "alarms", K_U32 },
	{ GPON_ATTR_EQD, "eqd", K_U32 },
	{ GPON_ATTR_EMERGENCY, "emergency", K_U8 },
	{ GPON_ATTR_SILENCE, "silence", K_U8 },
	{ GPON_ATTR_MIB_SYNCED, "mib_synced", K_U8 },
	{ GPON_ATTR_ADMIN_DOWN, "admin_down", K_U8 },
	{ GPON_ATTR_SD, "sd", K_U8 },
	{ GPON_ATTR_SF, "sf", K_U8 },
	{ GPON_ATTR_TO1, "to1", K_U32 },
	{ GPON_ATTR_TO2, "to2", K_U32 },
	{ GPON_ATTR_GEM, "gem", K_U16 },
	{ GPON_ATTR_ALLOC_ID, "alloc_id", K_U16 },
	{ GPON_ATTR_CHANNEL, "channel", K_U8 },
	{ GPON_ATTR_ENCRYPT, "encrypt", K_U8 },
	{ GPON_ATTR_TOD_SEC, "tod_sec", K_U32 },
	{ GPON_ATTR_TOD_NSEC, "tod_nsec", K_U32 },
	{ GPON_ATTR_TRAFFIC_UP, "traffic_up", K_U8 },
	{ GPON_ATTR_SPF_CNT, "spf_cnt", K_U32 },
};

static const char * const cnt_name[] = {
	[GPON_CNT_RX_FRAMES] = "rx_frames",
	[GPON_CNT_RX_BYTES] = "rx_bytes",
	[GPON_CNT_TX_FRAMES] = "tx_frames",
	[GPON_CNT_TX_BYTES] = "tx_bytes",
	[GPON_CNT_PLOAM_RX] = "ploam_rx",
	[GPON_CNT_PLOAM_TX] = "ploam_tx",
	[GPON_CNT_PLOAM_DROP] = "ploam_drop",
	[GPON_CNT_OMCI_RX] = "omci_rx",
	[GPON_CNT_OMCI_TX] = "omci_tx",
	[GPON_CNT_ACTIVATIONS] = "activations",
	[GPON_CNT_BIP] = "bip",
	[GPON_CNT_RX_GEM] = "rx_gem",
	[GPON_CNT_RX_CRC_ERR] = "rx_crc_err",
	[GPON_CNT_RX_GTC] = "rx_gtc",
	[GPON_CNT_TX_GEM] = "tx_gem",
	[GPON_CNT_TX_BURST] = "tx_burst",
	[GPON_CNT_HEC_ONE] = "hec_one",
	[GPON_CNT_HEC_TWO] = "hec_two",
	[GPON_CNT_HEC_UC] = "hec_uncorrectable",
	[GPON_CNT_FEC_CORR_BYTES] = "fec_corrected_bytes",
	[GPON_CNT_FEC_CORR_CW] = "fec_corrected_cw",
	[GPON_CNT_FEC_UNCORR_CW] = "fec_uncorrectable_cw",
	[GPON_CNT_FEC_TOTAL_CW] = "fec_total_cw",
	[GPON_CNT_FEC_SECONDS] = "fec_seconds",
	[GPON_CNT_LOF] = "lof",
	[GPON_CNT_RX_DROP_GEM] = "rx_drop_gem",
};

static void put_attrs(struct blob_buf *bb, struct nlattr **tb)
{
	size_t i;

	for (i = 0; i < sizeof(kattrs) / sizeof(kattrs[0]); i++) {
		struct nlattr *a = tb[kattrs[i].attr];

		if (!a)
			continue;
		switch (kattrs[i].type) {
		case K_U8:
			blobmsg_add_u32(bb, kattrs[i].name, nla_get_u8(a));
			break;
		case K_U16:
			blobmsg_add_u32(bb, kattrs[i].name, nla_get_u16(a));
			break;
		case K_U32:
			blobmsg_add_u32(bb, kattrs[i].name, nla_get_u32(a));
			break;
		case K_U64:
			blobmsg_add_u64(bb, kattrs[i].name, nla_get_u64(a));
			break;
		case K_BIN:
			break;
		}
	}
	if (tb[GPON_ATTR_MCAST])
		blobmsg_add_u8(bb, "multicast", 1);
	if (tb[GPON_ATTR_MODE])
		blobmsg_add_string(bb, "mode_name", config_mode_name(
				   nla_get_u8(tb[GPON_ATTR_MODE])));
}

static void put_counters(struct blob_buf *bb, struct nlattr *nest)
{
	struct nlattr *pos;
	int rem;

	nla_for_each_nested(pos, nest, rem) {
		int t = nla_type(pos);

		if (t <= 0 || (size_t)t >= sizeof(cnt_name) / sizeof(cnt_name[0]) ||
		    !cnt_name[t])
			continue;
		blobmsg_add_u64(bb, cnt_name[t], nla_get_u64(pos));
	}
}

/* ---- methods ---- */

static void status_cb(struct nlattr **tb, void *arg)
{
	put_attrs(&b, tb);
}

static int m_status(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	void *t;
	int ret;

	blob_buf_init(&b, 0);
	t = blobmsg_open_table(&b, "kernel");
	ret = nl_request(nl_msg(GPON_CMD_GET_STATE, false), status_cb,
			 NULL);
	blobmsg_close_table(&b, t);
	if (ret)
		blobmsg_add_string(&b, "error", strerror(-ret));
	blobmsg_add_string(&b, "mode", config_mode_name(cfg.mode));
	t = blobmsg_open_table(&b, "omci");
	omci_stats(&b);
	blobmsg_close_table(&b, t);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static const struct blobmsg_policy cnt_policy[__CNT_MAX] = {
	[CNT_GEM] = { "gem", BLOBMSG_TYPE_INT32 },
	[CNT_ALLOC] = { "alloc_id", BLOBMSG_TYPE_INT32 },
	[CNT_CLEAR] = { "clear", BLOBMSG_TYPE_BOOL },
};

static void counters_cb(struct nlattr **tb, void *arg)
{
	put_attrs(&b, tb);
	if (tb[GPON_ATTR_CNT])
		put_counters(&b, tb[GPON_ATTR_CNT]);
}

static int m_counters(struct ubus_context *c, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__CNT_MAX];
	struct nl_msg *m;
	int ret;

	blobmsg_parse(cnt_policy, __CNT_MAX, tb, blob_data(msg), blob_len(msg));
	/* the kernel takes a 16 bit GEM port / Alloc-ID: more would wrap */
	if ((tb[CNT_GEM] && attr_u32(tb[CNT_GEM]) > 0xffff) ||
	    (tb[CNT_ALLOC] && attr_u32(tb[CNT_ALLOC]) > 0xffff))
		return UBUS_STATUS_INVALID_ARGUMENT;
	m = nl_msg(GPON_CMD_GET_COUNTERS, false);
	if (!m)
		return UBUS_STATUS_UNKNOWN_ERROR;
	if (tb[CNT_GEM])
		nl_put_u16(m, GPON_ATTR_GEM, attr_u32(tb[CNT_GEM]));
	else if (tb[CNT_ALLOC])
		nl_put_u16(m, GPON_ATTR_ALLOC_ID,
			   attr_u32(tb[CNT_ALLOC]));
	if (tb[CNT_CLEAR] && blobmsg_get_bool(tb[CNT_CLEAR]))
		nl_put_flag(m, GPON_ATTR_CLEAR);

	blob_buf_init(&b, 0);
	ret = nl_request(m, counters_cb, NULL);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;
	ubus_send_reply(c, req, b.head);

	return 0;
}

static void tables_cb(struct nlattr **tb, void *arg)
{
	void *t = blobmsg_open_table(&b, NULL);

	put_attrs(&b, tb);
	blobmsg_close_table(&b, t);
}

static int m_tables(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	void *a;

	blob_buf_init(&b, 0);
	a = blobmsg_open_array(&b, "entries");
	nl_request(nl_msg(GPON_CMD_GET_TABLES, true), tables_cb, NULL);
	blobmsg_close_array(&b, a);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static int m_config(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	config_dump(&cfg, &b);
	ubus_send_reply(c, req, b.head);

	return 0;
}

/*
 * After factory the ONU has nothing to run on: the PON link goes down (netifd,
 * after the network reload that factory itself started) and the daemon is
 * stopped.  procd respawns a daemon that merely exits, so the init script is
 * asked to stop it.
 */
static struct uloop_process stop_proc;

static void stop_done(struct uloop_process *p, int ret)
{
}

static void factory_end(struct uloop_timeout *t)
{
	pid_t pid;

	INFO("factory: PON link down, gpond stops");
	link_set(false);

	pid = fork();
	if (pid == 0) {
		int fd;

		/* the init script gets a plain process: no ignored SIGPIPE, none
		 * of the sockets and the epoll fd of the daemon */
		signal(SIGPIPE, SIG_DFL);
		for (fd = 3; fd < 256; fd++)
			close(fd);
		execl("/etc/init.d/gpond", "gpond", "stop", (char *)NULL);
		_exit(127);
	}
	if (pid < 0) {
		ERR("factory: cannot stop the daemon: %s", strerror(errno));
		return;
	}
	stop_proc.pid = pid;
	stop_proc.cb = stop_done;
	uloop_process_add(&stop_proc);
}

static struct uloop_timeout factory_tmo = { .cb = factory_end };

/* back to the state before any OLT: no /usr/lib/gpon/gpon.json, every
 * section gpond wrote gone, the PON link down and the daemon stopped */
static int m_factory(struct ubus_context *c, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	int ret = 0;

	/* from here nothing reloads or stages again: the daemon is on its way
	 * out, and eth_factory() notifies the services that would send it a
	 * SIGHUP */
	gpond_stopping = true;
	omci_factory();
	if (eth_factory())
		ret = UBUS_STATUS_UNKNOWN_ERROR;
	if (ret) {
		/* the admin looks at the log first, the daemon keeps running */
		gpond_stopping = false;
		return ret;
	}
	/* the network reload of eth_factory() is asynchronous: the link goes
	 * down after it has had time to settle */
	uloop_timeout_set(&factory_tmo, 5000);

	return 0;
}

static int m_reload(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	if (gpond_stopping)
		return UBUS_STATUS_NOT_SUPPORTED;
	gpond_reload();

	return 0;
}

static int m_restart(struct ubus_context *c, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	return apply_config(true) ? UBUS_STATUS_UNKNOWN_ERROR : 0;
}

static int m_eth(struct ubus_context *c, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	eth_dump(&b);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static int m_mib(struct ubus_context *c, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	omci_dump(&b);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static int m_alarms(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	void *t;

	blob_buf_init(&b, 0);
	omci_alarm_dump(&b);
	t = blobmsg_open_table(&b, "sources");
	monitor_dump(&b);
	blobmsg_close_table(&b, t);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static int m_pm(struct ubus_context *c, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	omci_pm_dump(&b);
	ubus_send_reply(c, req, b.head);

	return 0;
}

static const struct blobmsg_policy inject_policy[__INJ_MAX] = {
	[INJ_FRAME] = { "frame", BLOBMSG_TYPE_STRING },
	[INJ_REPLAY] = { "replay", BLOBMSG_TYPE_BOOL },
};

static int m_inject(struct ubus_context *c, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	struct blob_attr *tb[__INJ_MAX];
	uint8_t raw[2048];
	const char *hex;
	size_t slen;

	blobmsg_parse(inject_policy, __INJ_MAX, tb, blob_data(msg),
		      blob_len(msg));
	if (tb[INJ_REPLAY] && blobmsg_get_bool(tb[INJ_REPLAY]))
		return mibstore_replay() ? UBUS_STATUS_NOT_FOUND : 0;
	if (!tb[INJ_FRAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	hex = blobmsg_get_string(tb[INJ_FRAME]);
	slen = strlen(hex);
	if (slen < 16 || slen % 2 || slen / 2 > sizeof(raw))
		return UBUS_STATUS_INVALID_ARGUMENT;
	if (hex_decode(hex, raw, slen / 2))
		return UBUS_STATUS_INVALID_ARGUMENT;
	raw[2] &= ~0x40;		/* clear AR so no response goes on PON */
	omci_rx(raw, slen / 2);

	return 0;
}

static const struct ubus_method gpon_methods[] = {
	UBUS_METHOD_NOARG("status", m_status),
	UBUS_METHOD("counters", m_counters, cnt_policy),
	UBUS_METHOD_NOARG("tables", m_tables),
	UBUS_METHOD_NOARG("config", m_config),
	UBUS_METHOD_NOARG("factory", m_factory),
	UBUS_METHOD_NOARG("reload", m_reload),
	UBUS_METHOD_NOARG("restart", m_restart),
	UBUS_METHOD_NOARG("mib", m_mib),
	UBUS_METHOD_NOARG("alarms", m_alarms),
	UBUS_METHOD_NOARG("pm", m_pm),
	UBUS_METHOD_NOARG("eth", m_eth),
	UBUS_METHOD("inject", m_inject, inject_policy),
};

static struct ubus_object_type gpon_type =
	UBUS_OBJECT_TYPE("gpon", gpon_methods);

static struct ubus_object gpon_obj = {
	.name = "gpon",
	.type = &gpon_type,
	.methods = gpon_methods,
	.n_methods = sizeof(gpon_methods) / sizeof(gpon_methods[0]),
};

/* ---- outgoing ---- */

void ubus_notify_state(struct nlattr **tb)
{
	if (!ctx)
		return;

	blob_buf_init(&b, 0);
	put_attrs(&b, tb);
	ubus_notify(ctx, &gpon_obj, "state", b.head, -1);
	ubus_send_event(ctx, "gpon.state", b.head);
}

void ubus_notify_tod(uint32_t sec, uint32_t nsec)
{
	if (!ctx)
		return;

	INFO("time of day taken over: %u.%09u", sec, nsec);
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "tod_sec", sec);
	blobmsg_add_u32(&b, "tod_nsec", nsec);
	ubus_notify(ctx, &gpon_obj, "tod", b.head, -1);
	ubus_send_event(ctx, "gpon.tod", b.head);
}

int ubus_network_reload(void)
{
	uint32_t id;

	if (!ctx)
		return -ENOTCONN;
	if (ubus_lookup_id(ctx, "network", &id))
		return -ENOENT;

	blob_buf_init(&b, 0);

	return ubus_invoke(ctx, id, "reload", b.head, NULL, NULL, 10000) ?
	       -EIO : 0;
}

/*
 * Tell procd that a package changed, like reload_config does: services
 * with a reload trigger on it (xpon-qos) are reloaded.
 */
int ubus_config_change(const char *pkg)
{
	uint32_t id;
	void *t;

	if (!ctx)
		return -ENOTCONN;
	if (ubus_lookup_id(ctx, "service", &id))
		return -ENOENT;

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "type", "config.change");
	t = blobmsg_open_table(&b, "data");
	blobmsg_add_string(&b, "package", pkg);
	blobmsg_close_table(&b, t);

	return ubus_invoke(ctx, id, "event", b.head, NULL, NULL, 3000) ?
	       -EIO : 0;
}

/* procd: ubus call system reboot */
int ubus_system_reboot(void)
{
	uint32_t id;

	if (!ctx)
		return -ENOTCONN;
	if (ubus_lookup_id(ctx, "system", &id))
		return -ENOENT;

	blob_buf_init(&b, 0);

	return ubus_invoke(ctx, id, "reboot", b.head, NULL, NULL, 3000) ?
	       -EIO : 0;
}

static const struct blobmsg_policy dev_status_policy[__DS_MAX] = {
	[DS_PRESENT] = { "present", BLOBMSG_TYPE_BOOL },
	[DS_CARRIER] = { "carrier", BLOBMSG_TYPE_BOOL },
	[DS_SPEED] = { "speed", BLOBMSG_TYPE_STRING },
	[DS_MAC] = { "macaddr", BLOBMSG_TYPE_STRING },
};

static void dev_status_cb(struct ubus_request *req, int type,
			  struct blob_attr *msg)
{
	struct blob_attr *tb[__DS_MAX];
	struct gpon_link *l = req->priv;
	const char *sp;
	char *end;

	blobmsg_parse(dev_status_policy, __DS_MAX, tb, blob_data(msg),
		      blob_len(msg));
	l->present = tb[DS_PRESENT] && blobmsg_get_bool(tb[DS_PRESENT]);
	l->carrier = l->present && tb[DS_CARRIER] &&
		     blobmsg_get_bool(tb[DS_CARRIER]);
	/* netifd system_if_dump_info(): "<Mbit/s>F" or "<Mbit/s>H" */
	if (tb[DS_SPEED]) {
		sp = blobmsg_get_string(tb[DS_SPEED]);
		l->speed = strtol(sp, &end, 10);
		l->full_duplex = *end != 'H';
	}
	/* device.c: "macaddr": "aa:bb:cc:dd:ee:ff" */
	if (tb[DS_MAC]) {
		unsigned int m[6];
		int i;

		if (sscanf(blobmsg_get_string(tb[DS_MAC]),
			   "%02x:%02x:%02x:%02x:%02x:%02x", &m[0], &m[1], &m[2],
			   &m[3], &m[4], &m[5]) == 6) {
			for (i = 0; i < 6; i++)
				l->mac[i] = m[i];
			l->mac_valid = true;
		}
	}
}

/*
 * netifd: ubus call network.device status {"name": dev}.  A device netifd
 * does not know reads as not present; -errno when netifd cannot be asked.
 */
int ubus_device_status(const char *dev, struct gpon_link *l)
{
	uint32_t id;
	int ret;

	memset(l, 0, sizeof(*l));
	if (!ctx)
		return -ENOTCONN;
	if (ubus_lookup_id(ctx, "network.device", &id))
		return -ENOENT;

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "name", dev);
	ret = ubus_invoke(ctx, id, "status", b.head, dev_status_cb, l, 1000);
	/* netifd_dev_status(): unknown device */
	if (ret == UBUS_STATUS_INVALID_ARGUMENT)
		return 0;

	return ret ? -EIO : 0;
}

static const struct blobmsg_policy iface_status_policy[__IS_MAX] = {
	[IS_UP] = { "up", BLOBMSG_TYPE_BOOL },
	[IS_PENDING] = { "pending", BLOBMSG_TYPE_BOOL },
	[IS_ADDR] = { "ipv4-address", BLOBMSG_TYPE_ARRAY },
	[IS_ROUTE] = { "route", BLOBMSG_TYPE_ARRAY },
	[IS_DNS] = { "dns-server", BLOBMSG_TYPE_ARRAY },
	[IS_SEARCH] = { "dns-search", BLOBMSG_TYPE_ARRAY },
};

/* prefix length -> dotted mask */
static void prefix_mask(unsigned int len, uint8_t *m)
{
	uint32_t v = len ? 0xffffffffu << (32 - (len > 32 ? 32 : len)) : 0;

	m[0] = v >> 24;
	m[1] = v >> 16;
	m[2] = v >> 8;
	m[3] = v;
}

static void iface_status_cb(struct ubus_request *req, int type,
			    struct blob_attr *msg)
{
	struct blob_attr *tb[__IS_MAX], *cur, *f[2];
	struct gpon_l3_status *s = req->priv;
	static const struct blobmsg_policy addr_p[2] = {
		{ "address", BLOBMSG_TYPE_STRING },
		{ "mask", BLOBMSG_TYPE_INT32 },
	};
	static const struct blobmsg_policy route_p[3] = {
		{ "target", BLOBMSG_TYPE_STRING },
		{ "mask", BLOBMSG_TYPE_INT32 },
		{ "nexthop", BLOBMSG_TYPE_STRING },
	};
	size_t rem;
	int n = 0;

	blobmsg_parse(iface_status_policy, __IS_MAX, tb, blob_data(msg),
		      blob_len(msg));
	s->valid = true;
	s->up = tb[IS_UP] && blobmsg_get_bool(tb[IS_UP]);
	s->pending = tb[IS_PENDING] && blobmsg_get_bool(tb[IS_PENDING]);

	/* ubus.c netifd_dump_status(): the interface address list */
	if (tb[IS_ADDR])
		blobmsg_for_each_attr(cur, tb[IS_ADDR], rem) {
			blobmsg_parse(addr_p, 2, f, blobmsg_data(cur),
				      blobmsg_data_len(cur));
			if (f[0] && f[1] &&
			    inet_pton(AF_INET, blobmsg_get_string(f[0]), s->addr) == 1) {
				prefix_mask(attr_u32(f[1]), s->mask);
				break;
			}
		}
	if (tb[IS_ROUTE])
		blobmsg_for_each_attr(cur, tb[IS_ROUTE], rem) {
			struct blob_attr *r[3];
			uint8_t t[4];

			blobmsg_parse(route_p, 3, r, blobmsg_data(cur),
				      blobmsg_data_len(cur));
			/* the default route: 0.0.0.0/0 and a next hop */
			if (r[0] && r[1] && r[2] && !attr_u32(r[1]) &&
			    inet_pton(AF_INET, blobmsg_get_string(r[0]), t) == 1 &&
			    !(t[0] | t[1] | t[2] | t[3]) &&
			    inet_pton(AF_INET, blobmsg_get_string(r[2]), s->gw) == 1)
				break;
		}
	if (tb[IS_DNS])
		blobmsg_for_each_attr(cur, tb[IS_DNS], rem) {
			if (blobmsg_type(cur) != BLOBMSG_TYPE_STRING)
				continue;
			if (n < 2 && inet_pton(AF_INET, blobmsg_get_string(cur),
					       s->dns[n]) == 1)
				n++;
		}
	if (tb[IS_SEARCH])
		blobmsg_for_each_attr(cur, tb[IS_SEARCH], rem) {
			if (blobmsg_type(cur) != BLOBMSG_TYPE_STRING)
				continue;
			snprintf(s->domain, sizeof(s->domain), "%s",
				 blobmsg_get_string(cur));
			break;
		}
}

/*
 * netifd: ubus call network.interface.<iface> status.  Not valid when the
 * interface is unknown (nothing configured yet) or netifd is not there.
 */
int ubus_iface_status(const char *iface, struct gpon_l3_status *s)
{
	char path[64];
	uint32_t id;

	memset(s, 0, sizeof(*s));
	if (!ctx)
		return -ENOTCONN;
	snprintf(path, sizeof(path), "network.interface.%s", iface);
	if (ubus_lookup_id(ctx, path, &id))
		return -ENOENT;

	blob_buf_init(&b, 0);

	return ubus_invoke(ctx, id, "status", b.head, iface_status_cb, s,
			   1000) ? -EIO : 0;
}

/* netifd: ubus call network.interface.<iface> up|down */
int ubus_iface_call(const char *iface, const char *method)
{
	char path[64];
	uint32_t id;

	if (!ctx)
		return -ENOTCONN;
	snprintf(path, sizeof(path), "network.interface.%s", iface);
	if (ubus_lookup_id(ctx, path, &id))
		return -ENOENT;

	blob_buf_init(&b, 0);

	return ubus_invoke(ctx, id, method, b.head, NULL, NULL, 3000) ? -EIO : 0;
}

int ubus_init(void)
{
	ctx = ubus_connect(NULL);
	if (!ctx) {
		ERR("cannot connect to ubus");
		return -EIO;
	}
	ubus_add_uloop(ctx);

	return ubus_add_object(ctx, &gpon_obj) ? -EIO : 0;
}

void ubus_done(void)
{
	if (!ctx)
		return;
	ubus_remove_object(ctx, &gpon_obj);
	ubus_free(ctx);
	ctx = NULL;
}
