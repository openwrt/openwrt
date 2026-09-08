// SPDX-License-Identifier: GPL-2.0-only
/*
 * Native ucode binding for Linux net/omci Generic Netlink.
 *
 * The module deliberately contains the entire kernel/userspace transport.
 * The rpcd ucode backend imports this .so directly; it does not execute a
 * helper process or shell command.
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <ucode/module.h>

#include "include/linux/omci.h"

#define OMCI_NL_REQ_SIZE 4096
#define OMCI_NL_RX_SIZE  32768
#define OMCI_NL_TIMEOUT_SEC 2

#ifndef NLA_TYPE_MASK
#define NLA_TYPE_MASK ~(NLA_F_NESTED | NLA_F_NET_BYTEORDER)
#endif

struct omci_nl {
	int fd;
	uint16_t family_id;
	uint32_t family_version;
	uint32_t portid;
	uint32_t seq;
};

struct nl_request {
	unsigned char buf[OMCI_NL_REQ_SIZE];
	struct nlmsghdr *nlh;
	struct genlmsghdr *genl;
};

static struct {
	int code;
	char message[192];
} last_error;

static void set_error(int code, const char *message)
{
	last_error.code = code;
	if (!message)
		message = code ? strerror(code < 0 ? -code : code) : "success";
	snprintf(last_error.message, sizeof(last_error.message), "%s", message);
}

static void set_errno_error(int code, const char *where)
{
	char buf[sizeof(last_error.message)];
	int e = code < 0 ? -code : code;

	snprintf(buf, sizeof(buf), "%s: %s", where, strerror(e));
	set_error(-e, buf);
}

static inline size_t nla_align_len(size_t len)
{
	return (len + NLA_ALIGNTO - 1) & ~(NLA_ALIGNTO - 1);
}

static inline void *nla_data_ptr(const struct nlattr *attr)
{
	return (char *)attr + NLA_HDRLEN;
}

static inline int nla_payload_len(const struct nlattr *attr)
{
	return (int)attr->nla_len - NLA_HDRLEN;
}

static bool attr_ok(const struct nlattr *attr, int remaining)
{
	return remaining >= (int)sizeof(*attr) &&
	       attr->nla_len >= sizeof(*attr) &&
	       attr->nla_len <= remaining;
}

static struct nlattr *attr_next(struct nlattr *attr, int *remaining)
{
	int step = (int)nla_align_len(attr->nla_len);

	*remaining -= step;
	return (struct nlattr *)((char *)attr + step);
}

static void parse_attrs(struct nlattr **tb, unsigned int max,
			struct nlattr *attr, int remaining)
{
	memset(tb, 0, sizeof(*tb) * (max + 1));

	while (attr_ok(attr, remaining)) {
		unsigned int type = attr->nla_type & NLA_TYPE_MASK;

		if (type <= max)
			tb[type] = attr;
		attr = attr_next(attr, &remaining);
	}
}

static uint8_t attr_u8(const struct nlattr *attr)
{
	uint8_t v = 0;
	if (attr && nla_payload_len(attr) >= (int)sizeof(v))
		memcpy(&v, nla_data_ptr(attr), sizeof(v));
	return v;
}

static int32_t attr_s32(const struct nlattr *attr)
{
	int32_t v = 0;
	if (attr && nla_payload_len(attr) >= (int)sizeof(v))
		memcpy(&v, nla_data_ptr(attr), sizeof(v));
	return v;
}

static uint16_t attr_u16(const struct nlattr *attr)
{
	uint16_t v = 0;
	if (attr && nla_payload_len(attr) >= (int)sizeof(v))
		memcpy(&v, nla_data_ptr(attr), sizeof(v));
	return v;
}

static uint32_t attr_u32(const struct nlattr *attr)
{
	uint32_t v = 0;
	if (attr && nla_payload_len(attr) >= (int)sizeof(v))
		memcpy(&v, nla_data_ptr(attr), sizeof(v));
	return v;
}

static uint64_t attr_u64(const struct nlattr *attr)
{
	uint64_t v = 0;
	if (attr && nla_payload_len(attr) >= (int)sizeof(v))
		memcpy(&v, nla_data_ptr(attr), sizeof(v));
	return v;
}

static void request_init(struct omci_nl *ctx, struct nl_request *req,
			 uint16_t type, uint8_t cmd, uint8_t version,
			 uint16_t flags)
{
	memset(req, 0, sizeof(*req));
	req->nlh = (struct nlmsghdr *)req->buf;
	req->nlh->nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	req->nlh->nlmsg_type = type;
	req->nlh->nlmsg_flags = NLM_F_REQUEST | flags;
	req->nlh->nlmsg_seq = ++ctx->seq;
	req->nlh->nlmsg_pid = ctx->portid;
	req->genl = (struct genlmsghdr *)NLMSG_DATA(req->nlh);
	req->genl->cmd = cmd;
	req->genl->version = version;
}

static int add_attr(struct nl_request *req, uint16_t type,
		    const void *data, size_t len)
{
	size_t attr_len = NLA_HDRLEN + len;
	size_t aligned = nla_align_len(attr_len);
	struct nlattr *attr;

	if (NLMSG_ALIGN(req->nlh->nlmsg_len) + aligned > sizeof(req->buf))
		return -EMSGSIZE;

	attr = (struct nlattr *)(req->buf + NLMSG_ALIGN(req->nlh->nlmsg_len));
	attr->nla_type = type;
	attr->nla_len = attr_len;
	if (len)
		memcpy(nla_data_ptr(attr), data, len);
	if (aligned > attr_len)
		memset((char *)attr + attr_len, 0, aligned - attr_len);
	req->nlh->nlmsg_len = NLMSG_ALIGN(req->nlh->nlmsg_len) + aligned;
	return 0;
}

static int add_u8(struct nl_request *req, uint16_t type, uint8_t value)
{
	return add_attr(req, type, &value, sizeof(value));
}

static int add_u16(struct nl_request *req, uint16_t type, uint16_t value)
{
	return add_attr(req, type, &value, sizeof(value));
}

static int add_u32(struct nl_request *req, uint16_t type, uint32_t value)
{
	return add_attr(req, type, &value, sizeof(value));
}

static int nl_send_request(struct omci_nl *ctx, struct nl_request *req)
{
	struct sockaddr_nl peer = { .nl_family = AF_NETLINK };
	ssize_t n;

	n = sendto(ctx->fd, req->buf, req->nlh->nlmsg_len, 0,
		   (struct sockaddr *)&peer, sizeof(peer));
	if (n < 0)
		return -errno;
	if ((size_t)n != req->nlh->nlmsg_len)
		return -EIO;
	return 0;
}

typedef int (*reply_cb_t)(struct nlmsghdr *nlh, void *arg);

static int recv_for_seq(struct omci_nl *ctx, uint32_t seq,
			reply_cb_t cb, void *arg)
{
	unsigned char buf[OMCI_NL_RX_SIZE];
	int ret;

	for (;;) {
		ssize_t len = recv(ctx->fd, buf, sizeof(buf), 0);
		struct nlmsghdr *nlh;
		int remaining;

		if (len < 0)
			return -errno;
		if (!len)
			return -ECONNRESET;

		remaining = len;
		for (nlh = (struct nlmsghdr *)buf; NLMSG_OK(nlh, remaining);
		     nlh = NLMSG_NEXT(nlh, remaining)) {
			if (nlh->nlmsg_seq != seq)
				continue;

			if (nlh->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *e;

				if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(*e)))
					return -EBADMSG;
				e = NLMSG_DATA(nlh);
				return e->error;
			}

			if (nlh->nlmsg_type == NLMSG_DONE)
				return 0;

			if (!cb)
				return 0;
			ret = cb(nlh, arg);
			return ret;
		}
	}
}

static int request_exec(struct omci_nl *ctx, struct nl_request *req,
			bool expect_reply, reply_cb_t cb, void *arg)
{
	uint32_t seq = req->nlh->nlmsg_seq;
	int ret;

	if (!expect_reply)
		req->nlh->nlmsg_flags |= NLM_F_ACK;

	ret = nl_send_request(ctx, req);
	if (ret)
		return ret;
	return recv_for_seq(ctx, seq, expect_reply ? cb : NULL, arg);
}

static int resolve_family_reply(struct nlmsghdr *nlh, void *arg)
{
	struct omci_nl *ctx = arg;
	struct genlmsghdr *genl;
	struct nlattr *attr;
	int remaining;

	if (nlh->nlmsg_len < NLMSG_LENGTH(GENL_HDRLEN))
		return -EBADMSG;
	genl = NLMSG_DATA(nlh);
	remaining = nlh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;
	attr = (struct nlattr *)((char *)genl + GENL_HDRLEN);

	while (attr_ok(attr, remaining)) {
		switch (attr->nla_type & NLA_TYPE_MASK) {
		case CTRL_ATTR_FAMILY_ID:
			ctx->family_id = attr_u16(attr);
			break;
		case CTRL_ATTR_VERSION:
			ctx->family_version = attr_u32(attr);
			break;
		default:
			break;
		}
		attr = attr_next(attr, &remaining);
	}
	return ctx->family_id ? 0 : -ENOENT;
}

static int resolve_family(struct omci_nl *ctx)
{
	struct nl_request req;
	int ret;

	request_init(ctx, &req, GENL_ID_CTRL, CTRL_CMD_GETFAMILY, 1, 0);
	ret = add_attr(&req, CTRL_ATTR_FAMILY_NAME, OMCI_GENL_NAME,
		       strlen(OMCI_GENL_NAME) + 1);
	if (ret)
		return ret;
	return request_exec(ctx, &req, true, resolve_family_reply, ctx);
}

static int omci_nl_open(struct omci_nl *ctx)
{
	struct sockaddr_nl local = { .nl_family = AF_NETLINK };
	struct timeval tv = { .tv_sec = OMCI_NL_TIMEOUT_SEC };
	socklen_t addrlen = sizeof(local);
	int ret;

	memset(ctx, 0, sizeof(*ctx));
	ctx->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_GENERIC);
	if (ctx->fd < 0)
		return -errno;
	setsockopt(ctx->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(ctx->fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if (bind(ctx->fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
		ret = -errno;
		close(ctx->fd);
		ctx->fd = -1;
		return ret;
	}
	if (getsockname(ctx->fd, (struct sockaddr *)&local, &addrlen) < 0) {
		ret = -errno;
		close(ctx->fd);
		ctx->fd = -1;
		return ret;
	}
	ctx->portid = local.nl_pid;
	ctx->seq = (uint32_t)getpid();
	ret = resolve_family(ctx);
	if (ret) {
		close(ctx->fd);
		ctx->fd = -1;
		return ret;
	}
	return 0;
}

static void omci_nl_close(struct omci_nl *ctx)
{
	if (ctx->fd >= 0)
		close(ctx->fd);
	ctx->fd = -1;
}

static int omci_req_init(struct omci_nl *ctx, struct nl_request *req,
			 uint8_t cmd, uint32_t device)
{
	request_init(ctx, req, ctx->family_id, cmd, OMCI_GENL_VERSION, 0);
	return add_u32(req, OMCI_ATTR_DEV_ID, device);
}

static int get_reply_attrs(struct nlmsghdr *nlh, struct nlattr **tb,
			   unsigned int max)
{
	struct genlmsghdr *genl;
	int remaining;

	if (nlh->nlmsg_len < NLMSG_LENGTH(GENL_HDRLEN))
		return -EBADMSG;
	genl = NLMSG_DATA(nlh);
	remaining = nlh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;
	parse_attrs(tb, max,
		    (struct nlattr *)((char *)genl + GENL_HDRLEN), remaining);
	return 0;
}

static uc_value_t *hex_value(const void *data, size_t len)
{
	static const char digits[] = "0123456789abcdef";
	const uint8_t *p = data;
	char *out;
	uc_value_t *v;
	size_t i;

	out = malloc(len * 2 + 1);
	if (!out)
		return NULL;
	for (i = 0; i < len; i++) {
		out[i * 2] = digits[p[i] >> 4];
		out[i * 2 + 1] = digits[p[i] & 0xf];
	}
	out[len * 2] = 0;
	v = ucv_string_new_length(out, len * 2);
	free(out);
	return v;
}

static bool bytes_printable(const uint8_t *p, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		if (p[i] < 0x20 || p[i] > 0x7e)
			return false;
	return true;
}

static uc_value_t *config_value_decode(uint16_t key, const void *data, size_t len)
{
	const uint8_t *p = data;
	size_t n;

	if (key >= OMCI_CONFIG_TRAFFIC_MGMT_OPTION &&
	    key <= OMCI_CONFIG_OMCC_VERSION) {
		if (len != 1)
			return hex_value(data, len);
		return ucv_uint64_new(p[0]);
	}

	if (key == OMCI_CONFIG_SERIAL_NUMBER && len == 8) {
		char serial[13];
		if (bytes_printable(p, 4)) {
			snprintf(serial, sizeof(serial), "%c%c%c%c%02X%02X%02X%02X",
				 p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
			return ucv_string_new(serial);
		}
		return hex_value(data, len);
	}

	if (key == OMCI_CONFIG_VENDOR_ID && len == 4) {
		if (bytes_printable(p, len))
			return ucv_string_new_length((const char *)p, len);
		return hex_value(data, len);
	}

	if (key == OMCI_CONFIG_PASSWORD && len == 10) {
		for (n = len; n && !p[n - 1]; n--)
			;
		if (n && bytes_printable(p, n))
			return ucv_string_new_length((const char *)p, n);
		return hex_value(data, len);
	}

	if (key == OMCI_CONFIG_VERSION || key == OMCI_CONFIG_HARDWARE_VERSION ||
	    key == OMCI_CONFIG_SOFTWARE_VERSION_0 ||
	    key == OMCI_CONFIG_SOFTWARE_VERSION_1 ||
	    key == OMCI_CONFIG_EQUIPMENT_ID) {
		for (n = len; n && !p[n - 1]; n--)
			;
		if (bytes_printable(p, n))
			return ucv_string_new_length((const char *)p, n);
		return hex_value(data, len);
	}

	return hex_value(data, len);
}

static void obj_add_u8(uc_value_t *o, const char *name, struct nlattr *a)
{
	if (a)
		ucv_object_add(o, name, ucv_uint64_new(attr_u8(a)));
}

static void obj_add_u16(uc_value_t *o, const char *name, struct nlattr *a)
{
	if (a)
		ucv_object_add(o, name, ucv_uint64_new(attr_u16(a)));
}

static void obj_add_u32(uc_value_t *o, const char *name, struct nlattr *a)
{
	if (a)
		ucv_object_add(o, name, ucv_uint64_new(attr_u32(a)));
}

static void obj_add_s32(uc_value_t *o, const char *name, struct nlattr *a)
{
	if (a)
		ucv_object_add(o, name, ucv_int64_new(attr_s32(a)));
}

static void obj_add_u64(uc_value_t *o, const char *name, struct nlattr *a)
{
	if (a)
		ucv_object_add(o, name, ucv_uint64_new(attr_u64(a)));
}

static void obj_add_string(uc_value_t *o, const char *name, struct nlattr *a)
{
	int len;
	const char *p;
	if (!a)
		return;
	len = nla_payload_len(a);
	p = nla_data_ptr(a);
	if (len > 0 && p[len - 1] == '\0')
		len--;
	ucv_object_add(o, name, ucv_string_new_length(p, len));
}

struct status_reply_arg {
	uc_vm_t *vm;
	struct omci_nl *ctx;
	uc_value_t *value;
};

static int status_reply(struct nlmsghdr *nlh, void *arg)
{
	struct status_reply_arg *r = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	uc_value_t *o;
	uint32_t flags = 0, capabilities = 0;
	int ret;

	ret = get_reply_attrs(nlh, a, OMCI_ATTR_MAX);
	if (ret)
		return ret;
	o = ucv_object_new(r->vm);
	if (!o)
		return -ENOMEM;

	obj_add_u32(o, "device", a[OMCI_ATTR_DEV_ID]);
	obj_add_u32(o, "ifindex", a[OMCI_ATTR_IFINDEX]);
	obj_add_u16(o, "onu_id", a[OMCI_ATTR_ONU_ID]);
	obj_add_u16(o, "gem_port", a[OMCI_ATTR_GEM_PORT_ID]);
	obj_add_u8(o, "state", a[OMCI_ATTR_STATE]);
	obj_add_u32(o, "flags", a[OMCI_ATTR_FLAGS]);
	obj_add_u32(o, "capabilities", a[OMCI_ATTR_CAPABILITIES]);
	obj_add_u32(o, "owner_portid", a[OMCI_ATTR_OWNER_PORTID]);
	obj_add_u64(o, "rx_packets", a[OMCI_ATTR_RX_PACKETS]);
	obj_add_u64(o, "rx_bytes", a[OMCI_ATTR_RX_BYTES]);
	obj_add_u64(o, "rx_dropped", a[OMCI_ATTR_RX_DROPPED]);
	obj_add_u64(o, "tx_packets", a[OMCI_ATTR_TX_PACKETS]);
	obj_add_u64(o, "tx_bytes", a[OMCI_ATTR_TX_BYTES]);
	obj_add_u64(o, "tx_errors", a[OMCI_ATTR_TX_ERRORS]);
	obj_add_u8(o, "agent_enabled", a[OMCI_ATTR_AGENT_ENABLED]);
	obj_add_u8(o, "agent_operational", a[OMCI_ATTR_AGENT_OPERATIONAL]);
	obj_add_u8(o, "permissive", a[OMCI_ATTR_AGENT_PERMISSIVE]);
	obj_add_u8(o, "fake_omci", a[OMCI_ATTR_AGENT_FAKE_OMCI]);
	obj_add_u8(o, "dying_gasp", a[OMCI_ATTR_AGENT_DYING_GASP]);
	obj_add_u8(o, "olt_profile_configured", a[OMCI_ATTR_OLT_PROFILE_CONFIGURED]);
	obj_add_u8(o, "olt_profile_effective", a[OMCI_ATTR_OLT_PROFILE_EFFECTIVE]);
	obj_add_u8(o, "olt_profile_forced", a[OMCI_ATTR_OLT_PROFILE_FORCED]);
	obj_add_u32(o, "olt_profile_quirks", a[OMCI_ATTR_OLT_PROFILE_QUIRKS]);
	obj_add_u16(o, "mib_sync", a[OMCI_ATTR_MIB_SYNC]);
	obj_add_u32(o, "mib_objects", a[OMCI_ATTR_MIB_OBJECTS]);
	obj_add_u64(o, "responses", a[OMCI_ATTR_AGENT_RESPONSES]);
	obj_add_u64(o, "duplicates", a[OMCI_ATTR_AGENT_DUPLICATES]);
	obj_add_u64(o, "unsupported", a[OMCI_ATTR_AGENT_UNSUPPORTED]);
	obj_add_u64(o, "fake_responses", a[OMCI_ATTR_AGENT_FAKE_RESPONSES]);
	obj_add_u8(o, "fec_downstream", a[OMCI_ATTR_FEC_DOWNSTREAM]);
	obj_add_u8(o, "fec_upstream", a[OMCI_ATTR_FEC_UPSTREAM]);
	obj_add_u32(o, "telemetry_valid", a[OMCI_ATTR_TELEMETRY_VALID]);
	obj_add_s32(o, "bosa_temperature_mc", a[OMCI_ATTR_BOSA_TEMPERATURE_MC]);
	obj_add_u32(o, "bosa_voltage_uv", a[OMCI_ATTR_BOSA_VOLTAGE_UV]);
	obj_add_u32(o, "bosa_bias_ua", a[OMCI_ATTR_BOSA_BIAS_UA]);
	obj_add_u32(o, "bosa_tx_power_nw", a[OMCI_ATTR_BOSA_TX_POWER_NW]);
	obj_add_u32(o, "bosa_rx_power_nw", a[OMCI_ATTR_BOSA_RX_POWER_NW]);
	obj_add_u32(o, "bosa_alarms", a[OMCI_ATTR_BOSA_ALARMS]);

	if (a[OMCI_ATTR_FLAGS])
		flags = attr_u32(a[OMCI_ATTR_FLAGS]);
	if (a[OMCI_ATTR_CAPABILITIES])
		capabilities = attr_u32(a[OMCI_ATTR_CAPABILITIES]);
	ucv_object_add(o, "channel_up", ucv_boolean_new(!!(flags & OMCI_F_CHANNEL_UP)));
	ucv_object_add(o, "cap_hw_mic", ucv_boolean_new(!!(capabilities & OMCI_CAP_HW_MIC)));
	ucv_object_add(o, "cap_baseline_agent", ucv_boolean_new(!!(capabilities & OMCI_CAP_BASELINE_AGENT)));
	ucv_object_add(o, "cap_telemetry", ucv_boolean_new(!!(capabilities & OMCI_CAP_TELEMETRY)));
	ucv_object_add(o, "uapi_version", ucv_uint64_new(r->ctx->family_version));
	ucv_object_add(o, "client_uapi_version", ucv_uint64_new(OMCI_GENL_VERSION));
	ucv_object_add(o, "uapi_compatible", ucv_boolean_new(r->ctx->family_version == OMCI_GENL_VERSION));

	r->value = o;
	return 0;
}

struct config_reply_arg {
	uc_vm_t *vm;
	uc_value_t *value;
};

static int config_reply(struct nlmsghdr *nlh, void *arg)
{
	struct config_reply_arg *r = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	uc_value_t *o, *v;
	uint16_t key;
	int ret;

	ret = get_reply_attrs(nlh, a, OMCI_ATTR_MAX);
	if (ret)
		return ret;
	if (!a[OMCI_ATTR_CONFIG_KEY] || !a[OMCI_ATTR_CONFIG_VALUE])
		return -EBADMSG;
	key = attr_u16(a[OMCI_ATTR_CONFIG_KEY]);
	o = ucv_object_new(r->vm);
	if (!o)
		return -ENOMEM;
	v = config_value_decode(key, nla_data_ptr(a[OMCI_ATTR_CONFIG_VALUE]),
				nla_payload_len(a[OMCI_ATTR_CONFIG_VALUE]));
	if (!v) {
		ucv_put(o);
		return -ENOMEM;
	}
	ucv_object_add(o, "key", ucv_uint64_new(key));
	ucv_object_add(o, "value", v);
	obj_add_u8(o, "source", a[OMCI_ATTR_CONFIG_SOURCE]);
	r->value = o;
	return 0;
}

static uc_value_t *parse_olt_g(uc_vm_t *vm, struct nlattr *nested)
{
	struct nlattr *a[OMCI_OLT_G_ATTR_MAX + 1];
	uc_value_t *o;

	if (!nested)
		return NULL;
	parse_attrs(a, OMCI_OLT_G_ATTR_MAX, nla_data_ptr(nested), nla_payload_len(nested));
	o = ucv_object_new(vm);
	obj_add_string(o, "vendor_id", a[OMCI_OLT_G_ATTR_VENDOR_ID]);
	obj_add_string(o, "equipment_id", a[OMCI_OLT_G_ATTR_EQUIPMENT_ID]);
	obj_add_string(o, "version", a[OMCI_OLT_G_ATTR_VERSION]);
	return o;
}

static uc_value_t *parse_vlan_filter(uc_vm_t *vm, struct nlattr *nested)
{
	struct nlattr *a[OMCI_VLAN_FILTER_ATTR_MAX + 1];
	uc_value_t *o, *entries;
	struct nlattr *entry;
	int remaining;

	if (!nested)
		return NULL;
	parse_attrs(a, OMCI_VLAN_FILTER_ATTR_MAX, nla_data_ptr(nested), nla_payload_len(nested));
	o = ucv_object_new(vm);
	obj_add_u8(o, "forward_operation", a[OMCI_VLAN_FILTER_ATTR_FORWARD_OPERATION]);
	obj_add_u8(o, "number_of_entries", a[OMCI_VLAN_FILTER_ATTR_NUMBER_OF_ENTRIES]);
	entries = ucv_array_new(vm);
	if (a[OMCI_VLAN_FILTER_ATTR_ENTRIES]) {
		remaining = nla_payload_len(a[OMCI_VLAN_FILTER_ATTR_ENTRIES]);
		entry = nla_data_ptr(a[OMCI_VLAN_FILTER_ATTR_ENTRIES]);
		while (attr_ok(entry, remaining)) {
			struct nlattr *e[OMCI_VLAN_FILTER_ENTRY_ATTR_MAX + 1];
			uc_value_t *eo = ucv_object_new(vm);
			parse_attrs(e, OMCI_VLAN_FILTER_ENTRY_ATTR_MAX,
				    nla_data_ptr(entry), nla_payload_len(entry));
			obj_add_u8(eo, "index", e[OMCI_VLAN_FILTER_ENTRY_ATTR_INDEX]);
			obj_add_u16(eo, "tci", e[OMCI_VLAN_FILTER_ENTRY_ATTR_TCI]);
			obj_add_u8(eo, "pbit", e[OMCI_VLAN_FILTER_ENTRY_ATTR_PBIT]);
			obj_add_u8(eo, "dei", e[OMCI_VLAN_FILTER_ENTRY_ATTR_DEI]);
			obj_add_u16(eo, "vid", e[OMCI_VLAN_FILTER_ENTRY_ATTR_VID]);
			ucv_array_push(entries, eo);
			entry = attr_next(entry, &remaining);
		}
	}
	ucv_object_add(o, "entries", entries);
	return o;
}

static uc_value_t *parse_ext_vlan_rule(uc_vm_t *vm, struct nlattr *entry)
{
	struct nlattr *a[OMCI_EXT_VLAN_RULE_ATTR_MAX + 1];
	uc_value_t *o = ucv_object_new(vm);

	parse_attrs(a, OMCI_EXT_VLAN_RULE_ATTR_MAX, nla_data_ptr(entry), nla_payload_len(entry));
	obj_add_u8(o, "index", a[OMCI_EXT_VLAN_RULE_ATTR_INDEX]);
	if (a[OMCI_EXT_VLAN_RULE_ATTR_RAW])
		ucv_object_add(o, "raw", hex_value(nla_data_ptr(a[OMCI_EXT_VLAN_RULE_ATTR_RAW]),
						nla_payload_len(a[OMCI_EXT_VLAN_RULE_ATTR_RAW])));
	obj_add_u8(o, "delete", a[OMCI_EXT_VLAN_RULE_ATTR_DELETE]);
	obj_add_u8(o, "filter_outer_pbit", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_PBIT]);
	obj_add_u16(o, "filter_outer_vid", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_VID]);
	obj_add_u8(o, "filter_outer_tpid_dei", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_TPID_DEI]);
	obj_add_u8(o, "filter_inner_pbit", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_PBIT]);
	obj_add_u16(o, "filter_inner_vid", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_VID]);
	obj_add_u8(o, "filter_inner_tpid_dei", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_TPID_DEI]);
	obj_add_u8(o, "filter_ethertype", a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_ETHERTYPE]);
	obj_add_u8(o, "tags_to_remove", a[OMCI_EXT_VLAN_RULE_ATTR_TAGS_TO_REMOVE]);
	obj_add_u8(o, "treat_outer_pbit", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_PBIT]);
	obj_add_u16(o, "treat_outer_vid", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_VID]);
	obj_add_u8(o, "treat_outer_tpid_dei", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_TPID_DEI]);
	obj_add_u8(o, "treat_inner_pbit", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_PBIT]);
	obj_add_u16(o, "treat_inner_vid", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_VID]);
	obj_add_u8(o, "treat_inner_tpid_dei", a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_TPID_DEI]);
	return o;
}

static uc_value_t *parse_extended_vlan(uc_vm_t *vm, struct nlattr *nested)
{
	struct nlattr *a[OMCI_EXT_VLAN_ATTR_MAX + 1];
	uc_value_t *o, *rules;
	struct nlattr *entry;
	int remaining;

	if (!nested)
		return NULL;
	parse_attrs(a, OMCI_EXT_VLAN_ATTR_MAX, nla_data_ptr(nested), nla_payload_len(nested));
	o = ucv_object_new(vm);
	obj_add_u8(o, "association_type", a[OMCI_EXT_VLAN_ATTR_ASSOCIATION_TYPE]);
	obj_add_u16(o, "max_table_size", a[OMCI_EXT_VLAN_ATTR_MAX_TABLE_SIZE]);
	obj_add_u16(o, "input_tpid", a[OMCI_EXT_VLAN_ATTR_INPUT_TPID]);
	obj_add_u16(o, "output_tpid", a[OMCI_EXT_VLAN_ATTR_OUTPUT_TPID]);
	obj_add_u8(o, "downstream_mode", a[OMCI_EXT_VLAN_ATTR_DOWNSTREAM_MODE]);
	obj_add_u16(o, "associated_me", a[OMCI_EXT_VLAN_ATTR_ASSOCIATED_ME]);
	if (a[OMCI_EXT_VLAN_ATTR_DSCP_TO_PBIT])
		ucv_object_add(o, "dscp_to_pbit",
			hex_value(nla_data_ptr(a[OMCI_EXT_VLAN_ATTR_DSCP_TO_PBIT]),
				  nla_payload_len(a[OMCI_EXT_VLAN_ATTR_DSCP_TO_PBIT])));
	rules = ucv_array_new(vm);
	if (a[OMCI_EXT_VLAN_ATTR_RULES]) {
		remaining = nla_payload_len(a[OMCI_EXT_VLAN_ATTR_RULES]);
		entry = nla_data_ptr(a[OMCI_EXT_VLAN_ATTR_RULES]);
		while (attr_ok(entry, remaining)) {
			ucv_array_push(rules, parse_ext_vlan_rule(vm, entry));
			entry = attr_next(entry, &remaining);
		}
	}
	ucv_object_add(o, "rules", rules);
	return o;
}

struct mib_reply_arg {
	uc_vm_t *vm;
	uc_value_t *value;
};

static int mib_reply(struct nlmsghdr *nlh, void *arg)
{
	struct mib_reply_arg *r = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	uc_value_t *o, *nested;
	int ret;

	ret = get_reply_attrs(nlh, a, OMCI_ATTR_MAX);
	if (ret)
		return ret;
	if (!a[OMCI_ATTR_CLASS_ID] || !a[OMCI_ATTR_ENTITY_ID])
		return -EBADMSG;
	o = ucv_object_new(r->vm);
	obj_add_u32(o, "device", a[OMCI_ATTR_DEV_ID]);
	obj_add_u16(o, "class_id", a[OMCI_ATTR_CLASS_ID]);
	obj_add_u16(o, "entity_id", a[OMCI_ATTR_ENTITY_ID]);
	obj_add_u16(o, "attr_mask", a[OMCI_ATTR_ATTR_MASK]);
	obj_add_u8(o, "origin", a[OMCI_ATTR_ORIGIN]);
	obj_add_u32(o, "next_index", a[OMCI_ATTR_INDEX]);
	obj_add_string(o, "name", a[OMCI_ATTR_NAME]);
	if (a[OMCI_ATTR_ATTR_DATA])
		ucv_object_add(o, "data",
			hex_value(nla_data_ptr(a[OMCI_ATTR_ATTR_DATA]),
				  nla_payload_len(a[OMCI_ATTR_ATTR_DATA])));
	nested = parse_olt_g(r->vm, a[OMCI_ATTR_OLT_G]);
	if (nested)
		ucv_object_add(o, "olt_g", nested);
	nested = parse_vlan_filter(r->vm, a[OMCI_ATTR_VLAN_TAGGING_FILTER]);
	if (nested)
		ucv_object_add(o, "vlan_filter", nested);
	nested = parse_extended_vlan(r->vm, a[OMCI_ATTR_EXTENDED_VLAN]);
	if (nested)
		ucv_object_add(o, "extended_vlan", nested);
	r->value = o;
	return 0;
}

struct class_reply_arg {
	uc_vm_t *vm;
	uc_value_t *value;
};

static int class_reply(struct nlmsghdr *nlh, void *arg)
{
	struct class_reply_arg *r = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	uc_value_t *o;
	int ret;

	ret = get_reply_attrs(nlh, a, OMCI_ATTR_MAX);
	if (ret)
		return ret;
	if (!a[OMCI_ATTR_CLASS_ID])
		return -EBADMSG;
	o = ucv_object_new(r->vm);
	obj_add_u16(o, "class_id", a[OMCI_ATTR_CLASS_ID]);
	obj_add_string(o, "name", a[OMCI_ATTR_NAME]);
	obj_add_u8(o, "category", a[OMCI_ATTR_CLASS_CATEGORY]);
	obj_add_u8(o, "support", a[OMCI_ATTR_CLASS_SUPPORT]);
	obj_add_u32(o, "flags", a[OMCI_ATTR_CLASS_FLAGS]);
	obj_add_u32(o, "next_index", a[OMCI_ATTR_INDEX]);
	r->value = o;
	return 0;
}

static bool parse_u32_arg(uc_value_t *v, uint32_t *out)
{
	uint64_t n;

	if (!v)
		return false;
	errno = 0;
	n = ucv_to_unsigned(v);
	if (errno || n > UINT32_MAX)
		return false;
	*out = n;
	return true;
}

static bool parse_u16_arg(uc_value_t *v, uint16_t *out)
{
	uint32_t n;
	if (!parse_u32_arg(v, &n) || n > UINT16_MAX)
		return false;
	*out = n;
	return true;
}

static bool parse_u8_value(uc_value_t *v, uint8_t *out)
{
	uint32_t n;
	if (!parse_u32_arg(v, &n) || n > UINT8_MAX)
		return false;
	*out = n;
	return true;
}

static int hex_decode(const char *s, size_t len, uint8_t *out,
		      size_t out_max, size_t *out_len)
{
	size_t i, n;
	int hi, lo;

	if (len >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
		len -= 2;
	}
	if (len & 1)
		return -EINVAL;
	n = len / 2;
	if (n > out_max)
		return -E2BIG;
	for (i = 0; i < n; i++) {
		char a = s[i * 2], b = s[i * 2 + 1];
		hi = (a >= '0' && a <= '9') ? a - '0' :
		     (a >= 'a' && a <= 'f') ? a - 'a' + 10 :
		     (a >= 'A' && a <= 'F') ? a - 'A' + 10 : -1;
		lo = (b >= '0' && b <= '9') ? b - '0' :
		     (b >= 'a' && b <= 'f') ? b - 'a' + 10 :
		     (b >= 'A' && b <= 'F') ? b - 'A' + 10 : -1;
		if (hi < 0 || lo < 0)
			return -EINVAL;
		out[i] = (hi << 4) | lo;
	}
	*out_len = n;
	return 0;
}

static uc_value_t *uc_error(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *o = ucv_object_new(vm);
	(void)nargs;
	ucv_object_add(o, "code", ucv_int64_new(last_error.code));
	ucv_object_add(o, "errno", ucv_uint64_new(last_error.code < 0 ? -last_error.code : last_error.code));
	ucv_object_add(o, "message", ucv_string_new(last_error.message[0] ? last_error.message : "no error"));
	return o;
}

static uc_value_t *uc_family(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *o;
	int ret;
	(void)nargs;

	ret = omci_nl_open(&ctx);
	if (ret) {
		set_errno_error(ret, "resolve OMCI Generic Netlink family");
		return NULL;
	}
	o = ucv_object_new(vm);
	ucv_object_add(o, "name", ucv_string_new(OMCI_GENL_NAME));
	ucv_object_add(o, "id", ucv_uint64_new(ctx.family_id));
	ucv_object_add(o, "version", ucv_uint64_new(ctx.family_version));
	ucv_object_add(o, "client_version", ucv_uint64_new(OMCI_GENL_VERSION));
	ucv_object_add(o, "compatible", ucv_boolean_new(ctx.family_version == OMCI_GENL_VERSION));
	omci_nl_close(&ctx);
	set_error(0, "success");
	return o;
}

static uc_value_t *uc_status(uc_vm_t *vm, size_t nargs)
{
	struct status_reply_arg r = { .vm = vm };
	struct omci_nl ctx;
	struct nl_request req;
	uint32_t device;
	int ret;

	if (!parse_u32_arg(uc_fn_arg(0), &device)) {
		set_error(-EINVAL, "status(): invalid device id");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	r.ctx = &ctx;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_GET, device);
	if (!ret)
		ret = request_exec(&ctx, &req, true, status_reply, &r);
	omci_nl_close(&ctx);
	if (ret) goto fail;
	set_error(0, "success");
	return r.value;
fail_open:
	set_errno_error(ret, "open OMCI netlink");
	return NULL;
fail:
	set_errno_error(ret, "OMCI status request");
	return NULL;
}

static uc_value_t *uc_agent_set(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uc_value_t *opts = uc_fn_arg(1), *v;
	uint32_t device;
	uint8_t n;
	bool any = false;
	int ret;
	(void)vm;

	if (!parse_u32_arg(uc_fn_arg(0), &device) || ucv_type(opts) != UC_OBJECT) {
		set_error(-EINVAL, "agent_set(): expected device id and options object");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_AGENT_SET, device);
	if (ret) goto out;
#define ADD_OPT(name, attr) do { \
	v = ucv_object_get(opts, name, NULL); \
	if (v) { \
		if (!parse_u8_value(v, &n)) { ret = -EINVAL; goto out; } \
		ret = add_u8(&req, attr, n); if (ret) goto out; any = true; \
	} \
} while (0)
	ADD_OPT("enabled", OMCI_ATTR_AGENT_ENABLED);
	ADD_OPT("permissive", OMCI_ATTR_AGENT_PERMISSIVE);
	ADD_OPT("fake_omci", OMCI_ATTR_AGENT_FAKE_OMCI);
	ADD_OPT("dying_gasp", OMCI_ATTR_AGENT_DYING_GASP);
	ADD_OPT("olt_profile", OMCI_ATTR_OLT_PROFILE_CONFIGURED);
	ADD_OPT("olt_profile_force", OMCI_ATTR_OLT_PROFILE_FORCED);
#undef ADD_OPT
	if (!any) { ret = -EINVAL; goto out; }
	ret = request_exec(&ctx, &req, false, NULL, NULL);
out:
	omci_nl_close(&ctx);
	if (ret) {
		set_errno_error(ret, "OMCI agent set");
		return NULL;
	}
	set_error(0, "success");
	return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink");
	return NULL;
}

static int config_get_one(struct omci_nl *ctx, uc_vm_t *vm,
			  uint32_t device, uint16_t key, uc_value_t **value)
{
	struct config_reply_arg r = { .vm = vm };
	struct nl_request req;
	int ret;

	ret = omci_req_init(ctx, &req, OMCI_CMD_CONFIG_GET, device);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_CONFIG_KEY, key);
	if (!ret) ret = request_exec(ctx, &req, true, config_reply, &r);
	if (!ret) *value = r.value;
	return ret;
}

static uc_value_t *uc_config_get(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *value = NULL;
	uint32_t device;
	uint16_t key;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) ||
	    !parse_u16_arg(uc_fn_arg(1), &key) || !key || key > OMCI_CONFIG_MAX) {
		set_error(-EINVAL, "config_get(): invalid device or key");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = config_get_one(&ctx, vm, device, key, &value);
	omci_nl_close(&ctx);
	if (ret) {
		set_errno_error(ret, "OMCI config get");
		return NULL;
	}
	set_error(0, "success");
	return value;
fail_open:
	set_errno_error(ret, "open OMCI netlink");
	return NULL;
}

static uc_value_t *uc_config_list(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *array, *value;
	uint32_t device;
	uint16_t key;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device)) {
		set_error(-EINVAL, "config_list(): invalid device id");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	array = ucv_array_new(vm);
	for (key = 1; key <= OMCI_CONFIG_MAX; key++) {
		value = NULL;
		ret = config_get_one(&ctx, vm, device, key, &value);
		if (ret)
			break;
		ucv_array_push(array, value);
	}
	omci_nl_close(&ctx);
	if (ret) {
		ucv_put(array);
		set_errno_error(ret, "OMCI config list");
		return NULL;
	}
	set_error(0, "success");
	return array;
fail_open:
	set_errno_error(ret, "open OMCI netlink");
	return NULL;
}

static uc_value_t *uc_config_set(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uc_value_t *value = uc_fn_arg(2);
	uint32_t device, n32;
	uint16_t key;
	uint8_t n8;
	const char *s;
	size_t len;
	int ret;
	(void)vm; (void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) ||
	    !parse_u16_arg(uc_fn_arg(1), &key) || !key || key > OMCI_CONFIG_MAX || !value) {
		set_error(-EINVAL, "config_set(): invalid device, key or value");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_CONFIG_SET, device);
	if (ret) goto out;
	ret = add_u16(&req, OMCI_ATTR_CONFIG_KEY, key);
	if (ret) goto out;

	if (key >= OMCI_CONFIG_TRAFFIC_MGMT_OPTION && key <= OMCI_CONFIG_OMCC_VERSION) {
		if (!parse_u32_arg(value, &n32) || n32 > 255) { ret = -EINVAL; goto out; }
		n8 = n32;
		ret = add_attr(&req, OMCI_ATTR_CONFIG_VALUE, &n8, sizeof(n8));
	} else {
		if (ucv_type(value) != UC_STRING) { ret = -EINVAL; goto out; }
		s = ucv_string_get(value);
		len = ucv_string_length(value);
		if (len > OMCI_MAX_CONFIG_VALUE) { ret = -E2BIG; goto out; }
		ret = add_attr(&req, OMCI_ATTR_CONFIG_VALUE, s, len);
	}
	if (!ret)
		ret = request_exec(&ctx, &req, false, NULL, NULL);
out:
	omci_nl_close(&ctx);
	if (ret) {
		set_errno_error(ret, "OMCI config set");
		return NULL;
	}
	set_error(0, "success");
	return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink");
	return NULL;
}

static int class_get_one(struct omci_nl *ctx, uc_vm_t *vm,
			 uint32_t device, uint8_t cmd, uint32_t index,
			 uint16_t class_id, uc_value_t **value)
{
	struct class_reply_arg r = { .vm = vm };
	struct nl_request req;
	int ret;

	ret = omci_req_init(ctx, &req, cmd, device);
	if (ret) return ret;
	if (cmd == OMCI_CMD_CLASS_GET)
		ret = add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	else
		ret = add_u32(&req, OMCI_ATTR_INDEX, index);
	if (!ret) ret = request_exec(ctx, &req, true, class_reply, &r);
	if (!ret) *value = r.value;
	return ret;
}

static uc_value_t *uc_class_get(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *value = NULL;
	uint32_t device;
	uint16_t class_id;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) || !parse_u16_arg(uc_fn_arg(1), &class_id)) {
		set_error(-EINVAL, "class_get(): invalid device or class id");
		return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = class_get_one(&ctx, vm, device, OMCI_CMD_CLASS_GET, 0, class_id, &value);
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI class get"); return NULL; }
	set_error(0, "success");
	return value;
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static uc_value_t *uc_class_list(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *array, *value, *nv;
	uint32_t device, index = 0, next;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device)) {
		set_error(-EINVAL, "class_list(): invalid device id"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	array = ucv_array_new(vm);
	for (;;) {
		value = NULL;
		ret = class_get_one(&ctx, vm, device, OMCI_CMD_CLASS_NEXT, index, 0, &value);
		if (ret == -ENOENT) { ret = 0; break; }
		if (ret) break;
		nv = ucv_object_get(value, "next_index", NULL);
		next = nv ? (uint32_t)ucv_uint64_get(nv) : 0;
		ucv_array_push(array, value);
		if (!next || next == index) break;
		index = next;
		if (ucv_array_length(array) > 4096) { ret = -EOVERFLOW; break; }
	}
	omci_nl_close(&ctx);
	if (ret) { ucv_put(array); set_errno_error(ret, "OMCI class list"); return NULL; }
	set_error(0, "success"); return array;
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static int mib_get_one(struct omci_nl *ctx, uc_vm_t *vm, uint32_t device,
		       uint8_t cmd, uint32_t index, uint16_t class_id,
		       uint16_t entity_id, uc_value_t **value)
{
	struct mib_reply_arg r = { .vm = vm };
	struct nl_request req;
	int ret;

	ret = omci_req_init(ctx, &req, cmd, device);
	if (ret) return ret;
	if (cmd == OMCI_CMD_MIB_GET) {
		if (!ret) ret = add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
		if (!ret) ret = add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	} else {
		if (!ret) ret = add_u32(&req, OMCI_ATTR_INDEX, index);
	}
	if (!ret) ret = request_exec(ctx, &req, true, mib_reply, &r);
	if (!ret) *value = r.value;
	return ret;
}

static uc_value_t *uc_mib_get(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *value = NULL;
	uint32_t device;
	uint16_t class_id, entity_id;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) ||
	    !parse_u16_arg(uc_fn_arg(1), &class_id) ||
	    !parse_u16_arg(uc_fn_arg(2), &entity_id)) {
		set_error(-EINVAL, "mib_get(): invalid device/class/entity"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = mib_get_one(&ctx, vm, device, OMCI_CMD_MIB_GET, 0, class_id, entity_id, &value);
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI MIB get"); return NULL; }
	set_error(0, "success"); return value;
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static uc_value_t *uc_mib_list(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	uc_value_t *array, *value, *nv;
	uint32_t device, index = 0, next;
	int ret;
	(void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device)) {
		set_error(-EINVAL, "mib_list(): invalid device id"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	array = ucv_array_new(vm);
	for (;;) {
		value = NULL;
		ret = mib_get_one(&ctx, vm, device, OMCI_CMD_MIB_NEXT, index, 0, 0, &value);
		if (ret == -ENOENT) { ret = 0; break; }
		if (ret) break;
		nv = ucv_object_get(value, "next_index", NULL);
		next = nv ? (uint32_t)ucv_uint64_get(nv) : 0;
		ucv_array_push(array, value);
		if (!next || next == index) break;
		index = next;
		if (ucv_array_length(array) > 8192) { ret = -EOVERFLOW; break; }
	}
	omci_nl_close(&ctx);
	if (ret) { ucv_put(array); set_errno_error(ret, "OMCI MIB list"); return NULL; }
	set_error(0, "success"); return array;
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static uc_value_t *uc_mib_set(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uint8_t data[OMCI_MAX_ATTR_DATA];
	uc_value_t *data_v = uc_fn_arg(4);
	uint32_t device;
	uint16_t class_id, entity_id, mask;
	const char *hex;
	size_t data_len;
	int ret;
	(void)vm; (void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) ||
	    !parse_u16_arg(uc_fn_arg(1), &class_id) ||
	    !parse_u16_arg(uc_fn_arg(2), &entity_id) ||
	    !parse_u16_arg(uc_fn_arg(3), &mask) || ucv_type(data_v) != UC_STRING) {
		set_error(-EINVAL, "mib_set(): invalid arguments"); return NULL;
	}
	hex = ucv_string_get(data_v);
	ret = hex_decode(hex, ucv_string_length(data_v), data, sizeof(data), &data_len);
	if (ret) { set_errno_error(ret, "decode MIB data"); return NULL; }
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_MIB_SET, device);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_ATTR_MASK, mask);
	if (!ret && data_len) ret = add_attr(&req, OMCI_ATTR_ATTR_DATA, data, data_len);
	if (!ret) ret = request_exec(&ctx, &req, false, NULL, NULL);
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI MIB set"); return NULL; }
	set_error(0, "success"); return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static uc_value_t *uc_mib_delete(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uint32_t device;
	uint16_t class_id, entity_id;
	int ret;
	(void)vm; (void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) ||
	    !parse_u16_arg(uc_fn_arg(1), &class_id) ||
	    !parse_u16_arg(uc_fn_arg(2), &entity_id)) {
		set_error(-EINVAL, "mib_delete(): invalid arguments"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_MIB_DELETE, device);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	if (!ret) ret = add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	if (!ret) ret = request_exec(&ctx, &req, false, NULL, NULL);
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI MIB delete"); return NULL; }
	set_error(0, "success"); return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static uc_value_t *uc_mib_reset(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uint32_t device;
	int ret;
	(void)vm; (void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device)) {
		set_error(-EINVAL, "mib_reset(): invalid device id"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_MIB_RESET, device);
	if (!ret) ret = request_exec(&ctx, &req, false, NULL, NULL);
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI MIB reset"); return NULL; }
	set_error(0, "success"); return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static int simple_device_request(struct omci_nl *ctx, uint8_t cmd, uint32_t device)
{
	struct nl_request req;
	int ret = omci_req_init(ctx, &req, cmd, device);
	if (!ret) ret = request_exec(ctx, &req, false, NULL, NULL);
	return ret;
}

static uc_value_t *uc_raw_tx(uc_vm_t *vm, size_t nargs)
{
	struct omci_nl ctx;
	struct nl_request req;
	uint8_t pdu[OMCI_MAX_PDU_LEN];
	uc_value_t *pdu_v = uc_fn_arg(1);
	uint32_t device;
	const char *hex;
	size_t pdu_len;
	int ret, unbind_ret;
	(void)vm; (void)nargs;

	if (!parse_u32_arg(uc_fn_arg(0), &device) || ucv_type(pdu_v) != UC_STRING) {
		set_error(-EINVAL, "raw_tx(): invalid device or PDU"); return NULL;
	}
	hex = ucv_string_get(pdu_v);
	ret = hex_decode(hex, ucv_string_length(pdu_v), pdu, sizeof(pdu), &pdu_len);
	if (ret || !pdu_len) {
		set_errno_error(ret ? ret : -EINVAL, "decode OMCI PDU"); return NULL;
	}
	ret = omci_nl_open(&ctx);
	if (ret) goto fail_open;
	ret = simple_device_request(&ctx, OMCI_CMD_BIND, device);
	if (ret) goto out;
	ret = omci_req_init(&ctx, &req, OMCI_CMD_TX, device);
	if (!ret) ret = add_attr(&req, OMCI_ATTR_PDU, pdu, pdu_len);
	if (!ret) ret = request_exec(&ctx, &req, false, NULL, NULL);
	unbind_ret = simple_device_request(&ctx, OMCI_CMD_UNBIND, device);
	if (!ret) ret = unbind_ret;
out:
	omci_nl_close(&ctx);
	if (ret) { set_errno_error(ret, "OMCI diagnostic TX"); return NULL; }
	set_error(0, "success"); return ucv_boolean_new(true);
fail_open:
	set_errno_error(ret, "open OMCI netlink"); return NULL;
}

static const uc_function_list_t functions[] = {
	{ "error", uc_error },
	{ "family", uc_family },
	{ "status", uc_status },
	{ "agent_set", uc_agent_set },
	{ "config_get", uc_config_get },
	{ "config_list", uc_config_list },
	{ "config_set", uc_config_set },
	{ "class_get", uc_class_get },
	{ "class_list", uc_class_list },
	{ "mib_get", uc_mib_get },
	{ "mib_list", uc_mib_list },
	{ "mib_set", uc_mib_set },
	{ "mib_delete", uc_mib_delete },
	{ "mib_reset", uc_mib_reset },
	{ "raw_tx", uc_raw_tx },
};

void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	(void)vm;
	set_error(0, "success");
	uc_function_list_register(scope, functions);
	ucv_object_add(scope, "UAPI_VERSION", ucv_uint64_new(OMCI_GENL_VERSION));
	ucv_object_add(scope, "MAX_ATTR_DATA", ucv_uint64_new(OMCI_MAX_ATTR_DATA));
	ucv_object_add(scope, "MAX_PDU_LEN", ucv_uint64_new(OMCI_MAX_PDU_LEN));
}
