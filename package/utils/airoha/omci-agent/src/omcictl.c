// SPDX-License-Identifier: GPL-2.0-only
#define _GNU_SOURCE

#include <errno.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/omci.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef NLA_ALIGNTO
#define NLA_ALIGNTO 4
#endif
#ifndef NLA_ALIGN
#define NLA_ALIGN(len) (((len) + NLA_ALIGNTO - 1) & ~(NLA_ALIGNTO - 1))
#endif
#ifndef NLA_HDRLEN
#define NLA_HDRLEN ((int)NLA_ALIGN(sizeof(struct nlattr)))
#endif

#define NL_BUF_SIZE 8192
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static volatile sig_atomic_t stop_requested;
static bool json_output;

struct nl_ctx {
	int fd;
	uint16_t family_id;
	uint32_t portid;
	uint32_t seq;
};

struct nl_request {
	char buf[NL_BUF_SIZE];
	struct nlmsghdr *nlh;
};

struct parsed_olt_g {
	char vendor_id[OMCI_OLT_VENDOR_ID_LEN + 1];
	char equipment_id[OMCI_OLT_EQUIPMENT_ID_LEN + 1];
	char version[OMCI_OLT_VERSION_LEN + 1];
	bool present;
};

struct parsed_vlan_filter_entry {
	uint16_t tci;
	uint16_t vid;
	uint8_t pbit;
	uint8_t dei;
};

struct parsed_vlan_filter {
	struct parsed_vlan_filter_entry entries[OMCI_VLAN_FILTER_MAX_ENTRIES];
	uint8_t forward_operation;
	uint8_t num_entries;
	bool present;
};

struct parsed_ext_vlan_rule {
	uint8_t raw[OMCI_EXT_VLAN_RULE_LEN];
	uint16_t filter_outer_vid;
	uint16_t filter_inner_vid;
	uint16_t treat_outer_vid;
	uint16_t treat_inner_vid;
	uint8_t filter_outer_pbit;
	uint8_t filter_outer_tpid_dei;
	uint8_t filter_inner_pbit;
	uint8_t filter_inner_tpid_dei;
	uint8_t filter_ethertype;
	uint8_t tags_to_remove;
	uint8_t treat_outer_pbit;
	uint8_t treat_outer_tpid_dei;
	uint8_t treat_inner_pbit;
	uint8_t treat_inner_tpid_dei;
	bool delete;
};

struct parsed_ext_vlan {
	struct parsed_ext_vlan_rule rules[OMCI_EXT_VLAN_MAX_RULES];
	uint8_t dscp_to_pbit[24];
	uint16_t input_tpid;
	uint16_t output_tpid;
	uint16_t associated_me;
	uint16_t max_table_size;
	uint8_t association_type;
	uint8_t downstream_mode;
	uint8_t rule_count;
	bool present;
};

struct parsed_class {
	uint16_t class_id;
	uint8_t category;
	uint8_t support;
	uint32_t flags;
	uint32_t next_index;
	char name[128];
};

struct parsed_mib {
	uint16_t class_id;
	uint16_t entity_id;
	uint16_t mask;
	uint8_t origin;
	uint8_t data[OMCI_MAX_ATTR_DATA];
	size_t data_len;
	uint32_t next_index;
	char name[128];
	struct parsed_olt_g olt_g;
	struct parsed_vlan_filter vlan_filter;
	struct parsed_ext_vlan ext_vlan;
};

struct config_name {
	const char *name;
	uint16_t key;
	bool scalar;
};

static const struct config_name config_names[] = {
	{ "serial", OMCI_CONFIG_SERIAL_NUMBER, false },
	{ "vendor-id", OMCI_CONFIG_VENDOR_ID, false },
	{ "version", OMCI_CONFIG_VERSION, false },
	{ "equipment-id", OMCI_CONFIG_EQUIPMENT_ID, false },
	{ "password", OMCI_CONFIG_PASSWORD, false },
	{ "traffic-management-option", OMCI_CONFIG_TRAFFIC_MGMT_OPTION, true },
	{ "onu-type", OMCI_CONFIG_ONU_TYPE, true },
	{ "uni-count", OMCI_CONFIG_UNI_COUNT, true },
	{ "agent-enabled", OMCI_CONFIG_AGENT_ENABLED, true },
	{ "agent-permissive", OMCI_CONFIG_AGENT_PERMISSIVE, true },
	{ "agent-fake-omci", OMCI_CONFIG_AGENT_FAKE_OMCI, true },
	{ "olt-profile", OMCI_CONFIG_OLT_PROFILE, true },
	{ "olt-profile-force", OMCI_CONFIG_OLT_PROFILE_FORCE, true },
	{ "agent-dying-gasp", OMCI_CONFIG_AGENT_DYING_GASP, true },
};

struct olt_quirk_name {
	uint32_t bit;
	const char *name;
};

static const struct olt_quirk_name olt_quirk_names[] = {
	{ OMCI_OLT_QUIRK_ALLOW_SET_CREATE, "allow-set-create" },
	{ OMCI_OLT_QUIRK_FAKE_UNSUPPORTED_SUCCESS, "fake-unsupported-success" },
	{ OMCI_OLT_QUIRK_IGNORE_UNSUPPORTED_UNI, "ignore-unsupported-uni" },
	{ OMCI_OLT_QUIRK_FULL_UNI_ENTITY_ID, "full-uni-entity-id" },
	{ OMCI_OLT_QUIRK_SANITIZE_VERSION, "sanitize-version" },
	{ OMCI_OLT_QUIRK_ZTE_VLAN_TAG_MODE, "zte-vlan-tag-mode" },
	{ OMCI_OLT_QUIRK_VENDOR_SPECIFIC_MES, "vendor-specific-mes" },
	{ OMCI_OLT_QUIRK_DASAN_MULTICAST_ANI, "dasan-multicast-ani" },
};

static void handle_signal(int signo)
{
	(void)signo;
	stop_requested = 1;
}

static bool attr_ok(const struct nlattr *attr, int remaining)
{
	return remaining >= (int)sizeof(*attr) &&
	       attr->nla_len >= sizeof(*attr) &&
	       attr->nla_len <= remaining;
}

static struct nlattr *attr_next(const struct nlattr *attr, int *remaining)
{
	int step = NLA_ALIGN(attr->nla_len);

	*remaining -= step;
	return (struct nlattr *)((char *)attr + step);
}

static void *attr_data(const struct nlattr *attr)
{
	return (char *)attr + NLA_HDRLEN;
}

static int attr_payload_len(const struct nlattr *attr)
{
	return attr->nla_len - NLA_HDRLEN;
}

static uint64_t get_u64(const struct nlattr *attr)
{
	uint64_t value = 0;

	if (attr && attr_payload_len(attr) >= (int)sizeof(value))
		memcpy(&value, attr_data(attr), sizeof(value));
	return value;
}

static uint32_t get_u32(const struct nlattr *attr)
{
	uint32_t value = 0;

	if (attr && attr_payload_len(attr) >= (int)sizeof(value))
		memcpy(&value, attr_data(attr), sizeof(value));
	return value;
}

static int32_t get_s32(const struct nlattr *attr)
{
	int32_t value = 0;

	if (attr && attr_payload_len(attr) >= (int)sizeof(value))
		memcpy(&value, attr_data(attr), sizeof(value));
	return value;
}

static uint16_t get_u16(const struct nlattr *attr)
{
	uint16_t value = 0;

	if (attr && attr_payload_len(attr) >= (int)sizeof(value))
		memcpy(&value, attr_data(attr), sizeof(value));
	return value;
}

static uint8_t get_u8(const struct nlattr *attr)
{
	uint8_t value = 0;

	if (attr && attr_payload_len(attr) >= (int)sizeof(value))
		memcpy(&value, attr_data(attr), sizeof(value));
	return value;
}

static void parse_attrs(struct nlmsghdr *nlh,
			struct nlattr *attrs[OMCI_ATTR_MAX + 1])
{
	struct genlmsghdr *genl = NLMSG_DATA(nlh);
	int remaining = nlh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;
	struct nlattr *attr = (struct nlattr *)((char *)genl + GENL_HDRLEN);

	memset(attrs, 0, sizeof(struct nlattr *) * (OMCI_ATTR_MAX + 1));
	while (attr_ok(attr, remaining)) {
		uint16_t type = attr->nla_type & NLA_TYPE_MASK;

		if (type <= OMCI_ATTR_MAX)
			attrs[type] = attr;
		attr = attr_next(attr, &remaining);
	}
}

static void parse_nested_attrs(const struct nlattr *nested,
			       struct nlattr **attrs, size_t max_attr)
{
	int remaining = attr_payload_len(nested);
	struct nlattr *attr = attr_data(nested);

	memset(attrs, 0, sizeof(*attrs) * (max_attr + 1));
	while (attr_ok(attr, remaining)) {
		uint16_t type = attr->nla_type & NLA_TYPE_MASK;

		if (type <= max_attr)
			attrs[type] = attr;
		attr = attr_next(attr, &remaining);
	}
}

static void copy_attr_string(char *dest, size_t dest_len,
			     const struct nlattr *attr)
{
	size_t len;

	if (!attr || !dest_len)
		return;
	len = (size_t)attr_payload_len(attr);
	if (len >= dest_len)
		len = dest_len - 1;
	memcpy(dest, attr_data(attr), len);
	dest[len] = '\0';
}

static void parse_olt_g(struct parsed_olt_g *olt,
			const struct nlattr *nested)
{
	struct nlattr *a[OMCI_OLT_G_ATTR_MAX + 1];

	parse_nested_attrs(nested, a, OMCI_OLT_G_ATTR_MAX);
	copy_attr_string(olt->vendor_id, sizeof(olt->vendor_id),
			 a[OMCI_OLT_G_ATTR_VENDOR_ID]);
	copy_attr_string(olt->equipment_id, sizeof(olt->equipment_id),
			 a[OMCI_OLT_G_ATTR_EQUIPMENT_ID]);
	copy_attr_string(olt->version, sizeof(olt->version),
			 a[OMCI_OLT_G_ATTR_VERSION]);
	olt->present = true;
}

static void parse_vlan_filter_entries(struct parsed_vlan_filter *filter,
				      const struct nlattr *entries)
{
	int remaining = attr_payload_len(entries);
	struct nlattr *item = attr_data(entries);

	while (attr_ok(item, remaining)) {
		struct nlattr *a[OMCI_VLAN_FILTER_ENTRY_ATTR_MAX + 1];
		uint8_t index;

		parse_nested_attrs(item, a, OMCI_VLAN_FILTER_ENTRY_ATTR_MAX);
		index = get_u8(a[OMCI_VLAN_FILTER_ENTRY_ATTR_INDEX]);
		if (index < OMCI_VLAN_FILTER_MAX_ENTRIES) {
			filter->entries[index].tci =
				get_u16(a[OMCI_VLAN_FILTER_ENTRY_ATTR_TCI]);
			filter->entries[index].pbit =
				get_u8(a[OMCI_VLAN_FILTER_ENTRY_ATTR_PBIT]);
			filter->entries[index].dei =
				get_u8(a[OMCI_VLAN_FILTER_ENTRY_ATTR_DEI]);
			filter->entries[index].vid =
				get_u16(a[OMCI_VLAN_FILTER_ENTRY_ATTR_VID]);
		}
		item = attr_next(item, &remaining);
	}
}

static void parse_vlan_filter(struct parsed_vlan_filter *filter,
			      const struct nlattr *nested)
{
	struct nlattr *a[OMCI_VLAN_FILTER_ATTR_MAX + 1];

	parse_nested_attrs(nested, a, OMCI_VLAN_FILTER_ATTR_MAX);
	filter->forward_operation =
		get_u8(a[OMCI_VLAN_FILTER_ATTR_FORWARD_OPERATION]);
	filter->num_entries =
		get_u8(a[OMCI_VLAN_FILTER_ATTR_NUMBER_OF_ENTRIES]);
	if (filter->num_entries > OMCI_VLAN_FILTER_MAX_ENTRIES)
		filter->num_entries = OMCI_VLAN_FILTER_MAX_ENTRIES;
	if (a[OMCI_VLAN_FILTER_ATTR_ENTRIES])
		parse_vlan_filter_entries(filter,
					  a[OMCI_VLAN_FILTER_ATTR_ENTRIES]);
	filter->present = true;
}

static void parse_ext_vlan_rules(struct parsed_ext_vlan *vlan,
				 const struct nlattr *rules)
{
	int remaining = attr_payload_len(rules);
	struct nlattr *item = attr_data(rules);

	while (attr_ok(item, remaining)) {
		struct nlattr *a[OMCI_EXT_VLAN_RULE_ATTR_MAX + 1];
		struct nlattr *raw;
		uint8_t index;

		parse_nested_attrs(item, a, OMCI_EXT_VLAN_RULE_ATTR_MAX);
		index = get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_INDEX]);
		if (index >= OMCI_EXT_VLAN_MAX_RULES)
			goto next;
		raw = a[OMCI_EXT_VLAN_RULE_ATTR_RAW];
		if (raw) {
			size_t len = (size_t)attr_payload_len(raw);

			if (len > sizeof(vlan->rules[index].raw))
				len = sizeof(vlan->rules[index].raw);
			memcpy(vlan->rules[index].raw, attr_data(raw), len);
		}
		vlan->rules[index].delete =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_DELETE]);
		vlan->rules[index].filter_outer_pbit =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_PBIT]);
		vlan->rules[index].filter_outer_vid =
			get_u16(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_VID]);
		vlan->rules[index].filter_outer_tpid_dei =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_OUTER_TPID_DEI]);
		vlan->rules[index].filter_inner_pbit =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_PBIT]);
		vlan->rules[index].filter_inner_vid =
			get_u16(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_VID]);
		vlan->rules[index].filter_inner_tpid_dei =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_INNER_TPID_DEI]);
		vlan->rules[index].filter_ethertype =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_FILTER_ETHERTYPE]);
		vlan->rules[index].tags_to_remove =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_TAGS_TO_REMOVE]);
		vlan->rules[index].treat_outer_pbit =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_PBIT]);
		vlan->rules[index].treat_outer_vid =
			get_u16(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_VID]);
		vlan->rules[index].treat_outer_tpid_dei =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_OUTER_TPID_DEI]);
		vlan->rules[index].treat_inner_pbit =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_PBIT]);
		vlan->rules[index].treat_inner_vid =
			get_u16(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_VID]);
		vlan->rules[index].treat_inner_tpid_dei =
			get_u8(a[OMCI_EXT_VLAN_RULE_ATTR_TREAT_INNER_TPID_DEI]);
		if (index >= vlan->rule_count)
			vlan->rule_count = index + 1;
next:
		item = attr_next(item, &remaining);
	}
}

static void parse_ext_vlan(struct parsed_ext_vlan *vlan,
			   const struct nlattr *nested)
{
	struct nlattr *a[OMCI_EXT_VLAN_ATTR_MAX + 1];
	struct nlattr *map;

	parse_nested_attrs(nested, a, OMCI_EXT_VLAN_ATTR_MAX);
	vlan->association_type =
		get_u8(a[OMCI_EXT_VLAN_ATTR_ASSOCIATION_TYPE]);
	vlan->max_table_size =
		get_u16(a[OMCI_EXT_VLAN_ATTR_MAX_TABLE_SIZE]);
	vlan->input_tpid = get_u16(a[OMCI_EXT_VLAN_ATTR_INPUT_TPID]);
	vlan->output_tpid = get_u16(a[OMCI_EXT_VLAN_ATTR_OUTPUT_TPID]);
	vlan->downstream_mode =
		get_u8(a[OMCI_EXT_VLAN_ATTR_DOWNSTREAM_MODE]);
	vlan->associated_me = get_u16(a[OMCI_EXT_VLAN_ATTR_ASSOCIATED_ME]);
	map = a[OMCI_EXT_VLAN_ATTR_DSCP_TO_PBIT];
	if (map) {
		size_t len = (size_t)attr_payload_len(map);

		if (len > sizeof(vlan->dscp_to_pbit))
			len = sizeof(vlan->dscp_to_pbit);
		memcpy(vlan->dscp_to_pbit, attr_data(map), len);
	}
	if (a[OMCI_EXT_VLAN_ATTR_RULES])
		parse_ext_vlan_rules(vlan, a[OMCI_EXT_VLAN_ATTR_RULES]);
	vlan->present = true;
}

static int add_attr(struct nlmsghdr *nlh, size_t capacity, uint16_t type,
		    const void *data, size_t len)
{
	size_t offset = NLMSG_ALIGN(nlh->nlmsg_len);
	size_t total = NLA_ALIGN(NLA_HDRLEN + len);
	struct nlattr *attr;

	if (offset + total > capacity)
		return -EMSGSIZE;
	attr = (struct nlattr *)((char *)nlh + offset);
	attr->nla_type = type;
	attr->nla_len = NLA_HDRLEN + len;
	memcpy(attr_data(attr), data, len);
	memset((char *)attr + attr->nla_len, 0, total - attr->nla_len);
	nlh->nlmsg_len = offset + total;
	return 0;
}

static int add_u8(struct nl_request *req, uint16_t type, uint8_t value)
{
	return add_attr(req->nlh, sizeof(req->buf), type, &value, sizeof(value));
}

static int add_u16(struct nl_request *req, uint16_t type, uint16_t value)
{
	return add_attr(req->nlh, sizeof(req->buf), type, &value, sizeof(value));
}

static int add_u32(struct nl_request *req, uint16_t type, uint32_t value)
{
	return add_attr(req->nlh, sizeof(req->buf), type, &value, sizeof(value));
}

static int nl_open(struct nl_ctx *ctx)
{
	struct sockaddr_nl addr = { .nl_family = AF_NETLINK };
	socklen_t addr_len = sizeof(addr);

	memset(ctx, 0, sizeof(*ctx));
	ctx->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_GENERIC);
	if (ctx->fd < 0)
		return -errno;
	if (bind(ctx->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		goto error;
	if (getsockname(ctx->fd, (struct sockaddr *)&addr, &addr_len) < 0)
		goto error;
	ctx->portid = addr.nl_pid;
	return 0;

error: {
	int ret = -errno;

	close(ctx->fd);
	ctx->fd = -1;
	return ret;
}
}

static int nl_send(struct nl_ctx *ctx, struct nlmsghdr *nlh)
{
	struct sockaddr_nl addr = { .nl_family = AF_NETLINK };
	struct iovec iov = { .iov_base = nlh, .iov_len = nlh->nlmsg_len };
	struct msghdr msg = {
		.msg_name = &addr,
		.msg_namelen = sizeof(addr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};

	if (sendmsg(ctx->fd, &msg, 0) < 0)
		return -errno;
	return 0;
}

static int recv_for_seq(struct nl_ctx *ctx, uint32_t seq, bool want_payload,
			int (*payload_cb)(struct nlmsghdr *, void *), void *arg)
{
	char buf[NL_BUF_SIZE];

	for (;;) {
		ssize_t len = recv(ctx->fd, buf, sizeof(buf), 0);
		struct nlmsghdr *nlh;

		if (len < 0)
			return errno == EINTR ? -EINTR : -errno;
		for (nlh = (struct nlmsghdr *)buf;
		     NLMSG_OK(nlh, (unsigned int)len); nlh = NLMSG_NEXT(nlh, len)) {
			if (nlh->nlmsg_seq != seq)
				continue;
			if (nlh->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = NLMSG_DATA(nlh);

				return err->error;
			}
			if (nlh->nlmsg_type == NLMSG_DONE)
				return 0;
			if (want_payload && payload_cb)
				return payload_cb(nlh, arg);
		}
	}
}

static void request_init(struct nl_ctx *ctx, struct nl_request *req,
			 uint8_t command, bool want_payload)
{
	struct genlmsghdr *genl;

	memset(req, 0, sizeof(*req));
	req->nlh = (struct nlmsghdr *)req->buf;
	req->nlh->nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	req->nlh->nlmsg_type = ctx->family_id;
	req->nlh->nlmsg_flags = NLM_F_REQUEST | (want_payload ? 0 : NLM_F_ACK);
	req->nlh->nlmsg_seq = ++ctx->seq;
	req->nlh->nlmsg_pid = ctx->portid;
	genl = NLMSG_DATA(req->nlh);
	genl->cmd = command;
	genl->version = OMCI_GENL_VERSION;
}

static int request_exec(struct nl_ctx *ctx, struct nl_request *req,
			bool want_payload,
			int (*payload_cb)(struct nlmsghdr *, void *), void *arg)
{
	int ret = nl_send(ctx, req->nlh);

	if (ret)
		return ret;
	return recv_for_seq(ctx, req->nlh->nlmsg_seq, want_payload,
			    payload_cb, arg);
}

static int resolve_family_reply(struct nlmsghdr *nlh, void *arg)
{
	struct genlmsghdr *genl = NLMSG_DATA(nlh);
	int remaining = nlh->nlmsg_len - NLMSG_HDRLEN - GENL_HDRLEN;
	struct nlattr *attr = (struct nlattr *)((char *)genl + GENL_HDRLEN);
	uint16_t *family_id = arg;

	while (attr_ok(attr, remaining)) {
		if (attr->nla_type == CTRL_ATTR_FAMILY_ID &&
		    attr_payload_len(attr) >= (int)sizeof(*family_id)) {
			memcpy(family_id, attr_data(attr), sizeof(*family_id));
			return 0;
		}
		attr = attr_next(attr, &remaining);
	}
	return -ENOENT;
}

static int resolve_family(struct nl_ctx *ctx)
{
	char buf[512] = {};
	struct nlmsghdr *nlh = (struct nlmsghdr *)buf;
	struct genlmsghdr *genl;
	const char name[] = OMCI_GENL_NAME;
	uint32_t seq = ++ctx->seq;
	int ret;

	nlh->nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	nlh->nlmsg_type = GENL_ID_CTRL;
	nlh->nlmsg_flags = NLM_F_REQUEST;
	nlh->nlmsg_seq = seq;
	nlh->nlmsg_pid = ctx->portid;
	genl = NLMSG_DATA(nlh);
	genl->cmd = CTRL_CMD_GETFAMILY;
	genl->version = 1;
	ret = add_attr(nlh, sizeof(buf), CTRL_ATTR_FAMILY_NAME, name,
		       sizeof(name));
	if (ret)
		return ret;
	ret = nl_send(ctx, nlh);
	if (ret)
		return ret;
	return recv_for_seq(ctx, seq, true, resolve_family_reply,
			    &ctx->family_id);
}

static void print_json_string(const void *data, size_t len)
{
	const unsigned char *text = data;
	size_t i;

	putchar('"');
	for (i = 0; i < len; i++) {
		unsigned char ch = text[i];

		if (ch == '"' || ch == '\\')
			printf("\\%c", ch);
		else if (ch >= 0x20 && ch < 0x7f)
			putchar(ch);
		else
			printf("\\u%04x", ch);
	}
	putchar('"');
}

static void print_hex_inline(const void *data, size_t len)
{
	const uint8_t *bytes = data;
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", bytes[i]);
}

static size_t format_gpon_serial(const void *data, size_t len, char *text,
				 size_t capacity)
{
	static const char hex[] = "0123456789ABCDEF";
	const uint8_t *serial = data;
	bool vendor_printable = true;
	size_t output_len;
	size_t pos = 0;
	size_t i;

	if (len != 8)
		return 0;

	for (i = 0; i < 4; i++) {
		if (serial[i] < 0x20 || serial[i] >= 0x7f) {
			vendor_printable = false;
			break;
		}
	}

	output_len = vendor_printable ? 12 : 16;
	if (capacity <= output_len)
		return 0;

	if (vendor_printable) {
		memcpy(text, serial, 4);
		pos = 4;
	} else {
		for (i = 0; i < 4; i++) {
			text[pos++] = hex[serial[i] >> 4];
			text[pos++] = hex[serial[i] & 0x0f];
		}
	}

	for (i = 4; i < 8; i++) {
		text[pos++] = hex[serial[i] >> 4];
		text[pos++] = hex[serial[i] & 0x0f];
	}
	text[pos] = '\0';

	return pos;
}

static int hex_value(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return -1;
}

static int parse_hex(const char *text, uint8_t *data, size_t capacity,
		     size_t *length)
{
	int high = -1;
	size_t used = 0;

	for (; *text; text++) {
		int value = hex_value(*text);

		if (value < 0) {
			if (*text == ':' || *text == '-' || *text == ' ' ||
			    *text == '\t')
				continue;
			return -EINVAL;
		}
		if (high < 0) {
			high = value;
			continue;
		}
		if (used == capacity)
			return -EMSGSIZE;
		data[used++] = (uint8_t)((high << 4) | value);
		high = -1;
	}
	if (high >= 0)
		return -EINVAL;
	*length = used;
	return 0;
}

static int parse_u32(const char *text, uint32_t *value)
{
	char *end;
	unsigned long parsed;

	errno = 0;
	parsed = strtoul(text, &end, 0);
	if (errno || !*text || *end || parsed > UINT32_MAX)
		return -EINVAL;
	*value = (uint32_t)parsed;
	return 0;
}

static const struct config_name *find_config(const char *name)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(config_names); i++)
		if (!strcmp(name, config_names[i].name))
			return &config_names[i];
	return NULL;
}

static const char *olt_profile_name(uint8_t profile)
{
	switch (profile) {
	case OMCI_OLT_PROFILE_GENERIC:
		return "generic";
	case OMCI_OLT_PROFILE_AUTO:
		return "auto";
	case OMCI_OLT_PROFILE_NOKIA_ALCL:
		return "nokia-alcl";
	case OMCI_OLT_PROFILE_DASAN:
		return "dasan";
	case OMCI_OLT_PROFILE_HUAWEI:
		return "huawei";
	case OMCI_OLT_PROFILE_FIBERHOME:
		return "fiberhome";
	case OMCI_OLT_PROFILE_ZTE:
		return "zte";
	default:
		return "unspecified";
	}
}

static int parse_olt_profile(const char *text, bool force, uint8_t *profile)
{
	uint32_t parsed;
	uint8_t value;

	if (!strcmp(text, "unspecified") || !strcmp(text, "none"))
		value = OMCI_OLT_PROFILE_UNSPEC;
	else if (!strcmp(text, "generic"))
		value = OMCI_OLT_PROFILE_GENERIC;
	else if (!strcmp(text, "auto"))
		value = OMCI_OLT_PROFILE_AUTO;
	else if (!strcmp(text, "nokia-alcl") || !strcmp(text, "nokia") ||
		 !strcmp(text, "alcl"))
		value = OMCI_OLT_PROFILE_NOKIA_ALCL;
	else if (!strcmp(text, "dasan"))
		value = OMCI_OLT_PROFILE_DASAN;
	else if (!strcmp(text, "huawei"))
		value = OMCI_OLT_PROFILE_HUAWEI;
	else if (!strcmp(text, "fiberhome"))
		value = OMCI_OLT_PROFILE_FIBERHOME;
	else if (!strcmp(text, "zte"))
		value = OMCI_OLT_PROFILE_ZTE;
	else {
		if (parse_u32(text, &parsed) || parsed > UINT8_MAX)
			return -EINVAL;
		value = (uint8_t)parsed;
	}

	if (force) {
		if (value == OMCI_OLT_PROFILE_AUTO || value > OMCI_OLT_PROFILE_ZTE)
			return -EINVAL;
	} else if (value < OMCI_OLT_PROFILE_GENERIC ||
		   value > OMCI_OLT_PROFILE_ZTE) {
		return -EINVAL;
	}

	*profile = value;
	return 0;
}

static void print_olt_quirks(uint32_t quirks)
{
	size_t i;
	bool first = true;

	for (i = 0; i < ARRAY_SIZE(olt_quirk_names); i++) {
		if (!(quirks & olt_quirk_names[i].bit))
			continue;
		if (!first)
			fputs(", ", stdout);
		fputs(olt_quirk_names[i].name, stdout);
		first = false;
	}
	if (first)
		fputs("none", stdout);
}

static const char *fec_status_name(uint8_t status)
{
	switch (status) {
	case OMCI_FEC_STATUS_DOWN:
		return "down";
	case OMCI_FEC_STATUS_UP:
		return "up";
	default:
		return "unknown";
	}
}

static void print_scaled_u32(const char *name, uint32_t value,
			     uint32_t scale, unsigned int digits,
			     const char *unit)
{
	printf("%s: %u.%0*u %s\n", name, value / scale, digits,
	       value % scale, unit);
}

static void print_temperature_mc(int32_t value)
{
	int64_t magnitude = value;

	if (magnitude < 0) {
		putchar('-');
		magnitude = -magnitude;
	}
	printf("%lld.%03lld C\n",
	       (long long)(magnitude / 1000),
	       (long long)(magnitude % 1000));
}

static int status_reply(struct nlmsghdr *nlh, void *arg)
{
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	uint32_t profile_quirks;
	uint32_t flags;
	uint8_t profile_configured;
	uint8_t profile_effective;
	uint8_t profile_forced;

	(void)arg;
	parse_attrs(nlh, a);
	flags = get_u32(a[OMCI_ATTR_FLAGS]);
	profile_configured = get_u8(a[OMCI_ATTR_OLT_PROFILE_CONFIGURED]);
	profile_effective = get_u8(a[OMCI_ATTR_OLT_PROFILE_EFFECTIVE]);
	profile_forced = get_u8(a[OMCI_ATTR_OLT_PROFILE_FORCED]);
	profile_quirks = get_u32(a[OMCI_ATTR_OLT_PROFILE_QUIRKS]);
	if (json_output) {
		printf("{\"device\":%u,\"ifindex\":%u,\"state\":%u,"
		       "\"onu_id\":%u,\"gem_port\":%u,\"channel_up\":%s,"
		       "\"agent_enabled\":%s,\"agent_operational\":%s,"
		       "\"permissive\":%s,\"fake_omci\":%s,"
		       "\"dying_gasp\":%s,\"olt_profile_configured\":%u,"
		       "\"olt_profile_effective\":%u,\"olt_profile_forced\":%u,"
		       "\"olt_profile_quirks\":%u,\"mib_sync\":%u,"
		       "\"mib_objects\":%u,"
		       "\"rx_packets\":%llu,\"rx_bytes\":%llu,"
		       "\"rx_dropped\":%llu,\"tx_packets\":%llu,"
		       "\"tx_bytes\":%llu,\"tx_errors\":%llu,"
		       "\"responses\":%llu,\"duplicates\":%llu,"
		       "\"unsupported\":%llu,\"fake_responses\":%llu,"
		       "\"telemetry_valid\":%u,"
		       "\"fec_downstream\":",
		       get_u32(a[OMCI_ATTR_DEV_ID]),
		       get_u32(a[OMCI_ATTR_IFINDEX]),
		       get_u8(a[OMCI_ATTR_STATE]),
		       get_u16(a[OMCI_ATTR_ONU_ID]),
		       get_u16(a[OMCI_ATTR_GEM_PORT_ID]),
		       flags & OMCI_F_CHANNEL_UP ? "true" : "false",
		       get_u8(a[OMCI_ATTR_AGENT_ENABLED]) ? "true" : "false",
		       get_u8(a[OMCI_ATTR_AGENT_OPERATIONAL]) ? "true" : "false",
		       get_u8(a[OMCI_ATTR_AGENT_PERMISSIVE]) ? "true" : "false",
		       get_u8(a[OMCI_ATTR_AGENT_FAKE_OMCI]) ? "true" : "false",
		       get_u8(a[OMCI_ATTR_AGENT_DYING_GASP]) ? "true" : "false",
		       profile_configured, profile_effective, profile_forced,
		       profile_quirks,
		       get_u16(a[OMCI_ATTR_MIB_SYNC]),
		       get_u32(a[OMCI_ATTR_MIB_OBJECTS]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_RX_PACKETS]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_RX_BYTES]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_RX_DROPPED]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_TX_PACKETS]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_TX_BYTES]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_TX_ERRORS]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_RESPONSES]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_DUPLICATES]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_UNSUPPORTED]),
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_FAKE_RESPONSES]),
		       get_u32(a[OMCI_ATTR_TELEMETRY_VALID]));
		if (a[OMCI_ATTR_FEC_DOWNSTREAM])
			printf("%u", get_u8(a[OMCI_ATTR_FEC_DOWNSTREAM]));
		else
			fputs("null", stdout);
		fputs(",\"fec_upstream\":", stdout);
		if (a[OMCI_ATTR_FEC_UPSTREAM])
			printf("%u", get_u8(a[OMCI_ATTR_FEC_UPSTREAM]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_temperature_mc\":", stdout);
		if (a[OMCI_ATTR_BOSA_TEMPERATURE_MC])
			printf("%d", get_s32(a[OMCI_ATTR_BOSA_TEMPERATURE_MC]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_voltage_uv\":", stdout);
		if (a[OMCI_ATTR_BOSA_VOLTAGE_UV])
			printf("%u", get_u32(a[OMCI_ATTR_BOSA_VOLTAGE_UV]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_bias_ua\":", stdout);
		if (a[OMCI_ATTR_BOSA_BIAS_UA])
			printf("%u", get_u32(a[OMCI_ATTR_BOSA_BIAS_UA]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_tx_power_nw\":", stdout);
		if (a[OMCI_ATTR_BOSA_TX_POWER_NW])
			printf("%u", get_u32(a[OMCI_ATTR_BOSA_TX_POWER_NW]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_rx_power_nw\":", stdout);
		if (a[OMCI_ATTR_BOSA_RX_POWER_NW])
			printf("%u", get_u32(a[OMCI_ATTR_BOSA_RX_POWER_NW]));
		else
			fputs("null", stdout);
		fputs(",\"bosa_alarms\":", stdout);
		if (a[OMCI_ATTR_BOSA_ALARMS])
			printf("%u", get_u32(a[OMCI_ATTR_BOSA_ALARMS]));
		else
			fputs("null", stdout);
		puts("}");
	} else {
		printf("device: %u\n", get_u32(a[OMCI_ATTR_DEV_ID]));
		printf("ifindex: %u\n", get_u32(a[OMCI_ATTR_IFINDEX]));
		printf("state: O%u\n", get_u8(a[OMCI_ATTR_STATE]));
		printf("onu-id: %u\n", get_u16(a[OMCI_ATTR_ONU_ID]));
		printf("gem-port: %u\n", get_u16(a[OMCI_ATTR_GEM_PORT_ID]));
		printf("channel: %s\n", flags & OMCI_F_CHANNEL_UP ? "up" : "down");
		printf("agent: %s (%s)\n",
		       get_u8(a[OMCI_ATTR_AGENT_ENABLED]) ? "enabled" : "disabled",
		       get_u8(a[OMCI_ATTR_AGENT_PERMISSIVE]) ? "permissive" : "strict");
		printf("agent-operational: %s\n",
		       get_u8(a[OMCI_ATTR_AGENT_OPERATIONAL]) ? "yes" : "no");
		printf("fake-omci: %s\n",
		       get_u8(a[OMCI_ATTR_AGENT_FAKE_OMCI]) ? "enabled" : "disabled");
		printf("dying-gasp: %s\n",
		       get_u8(a[OMCI_ATTR_AGENT_DYING_GASP]) ? "enabled" : "disabled");
		printf("olt-profile-configured: %s (%u)\n",
		       olt_profile_name(profile_configured), profile_configured);
		printf("olt-profile-effective: %s (%u)\n",
		       olt_profile_name(profile_effective), profile_effective);
		printf("olt-profile-forced: %s (%u)\n",
		       olt_profile_name(profile_forced), profile_forced);
		printf("olt-profile-quirks: 0x%08x [", profile_quirks);
		print_olt_quirks(profile_quirks);
		puts("]");
		printf("mib-sync: %u\n", get_u16(a[OMCI_ATTR_MIB_SYNC]));
		printf("mib-objects: %u\n", get_u32(a[OMCI_ATTR_MIB_OBJECTS]));
		printf("rx-packets: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_RX_PACKETS]));
		printf("rx-dropped: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_RX_DROPPED]));
		printf("tx-packets: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_TX_PACKETS]));
		printf("tx-errors: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_TX_ERRORS]));
		printf("agent-responses: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_RESPONSES]));
		printf("agent-duplicates: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_DUPLICATES]));
		printf("agent-unsupported: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_UNSUPPORTED]));
		printf("agent-fake-responses: %llu\n",
		       (unsigned long long)get_u64(a[OMCI_ATTR_AGENT_FAKE_RESPONSES]));
		printf("fec-downstream: %s\n",
		       a[OMCI_ATTR_FEC_DOWNSTREAM] ?
		       fec_status_name(get_u8(a[OMCI_ATTR_FEC_DOWNSTREAM])) :
		       "unavailable");
		printf("fec-upstream: %s\n",
		       a[OMCI_ATTR_FEC_UPSTREAM] ?
		       fec_status_name(get_u8(a[OMCI_ATTR_FEC_UPSTREAM])) :
		       "unavailable");
		fputs("bosa-temperature: ", stdout);
		if (a[OMCI_ATTR_BOSA_TEMPERATURE_MC])
			print_temperature_mc(
				get_s32(a[OMCI_ATTR_BOSA_TEMPERATURE_MC]));
		else
			puts("unavailable");
		if (a[OMCI_ATTR_BOSA_VOLTAGE_UV])
			print_scaled_u32("bosa-supply-voltage",
				get_u32(a[OMCI_ATTR_BOSA_VOLTAGE_UV]),
				1000000, 6, "V");
		else
			puts("bosa-supply-voltage: unavailable");
		if (a[OMCI_ATTR_BOSA_BIAS_UA])
			print_scaled_u32("bosa-tx-bias",
				get_u32(a[OMCI_ATTR_BOSA_BIAS_UA]),
				1000, 3, "mA");
		else
			puts("bosa-tx-bias: unavailable");
		if (a[OMCI_ATTR_BOSA_TX_POWER_NW])
			print_scaled_u32("bosa-tx-power",
				get_u32(a[OMCI_ATTR_BOSA_TX_POWER_NW]),
				1000, 3, "uW");
		else
			puts("bosa-tx-power: unavailable");
		if (a[OMCI_ATTR_BOSA_RX_POWER_NW])
			print_scaled_u32("bosa-rx-power",
				get_u32(a[OMCI_ATTR_BOSA_RX_POWER_NW]),
				1000, 3, "uW");
		else
			puts("bosa-rx-power: unavailable");
		if (a[OMCI_ATTR_BOSA_ALARMS])
			printf("bosa-alarms: %#x\n",
			       get_u32(a[OMCI_ATTR_BOSA_ALARMS]));
		else
			puts("bosa-alarms: unavailable");
	}
	return 0;
}

static int command_status(struct nl_ctx *ctx, uint32_t dev_id)
{
	struct nl_request req;

	request_init(ctx, &req, OMCI_CMD_GET, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	return request_exec(ctx, &req, true, status_reply, NULL);
}

struct config_reply_ctx {
	const struct config_name *config;
	bool print_name;
	bool include_source;
};

static const char *config_source_name(uint8_t source)
{
	switch (source) {
	case OMCI_CONFIG_SOURCE_DEFAULT:
		return "default";
	case OMCI_CONFIG_SOURCE_DEVICE_TREE:
		return "device-tree";
	case OMCI_CONFIG_SOURCE_NVMEM:
		return "nvmem";
	case OMCI_CONFIG_SOURCE_DRIVER:
		return "driver";
	case OMCI_CONFIG_SOURCE_NETLINK:
		return "netlink";
	default:
		return "unspecified";
	}
}

static void print_config_json_value(const struct config_name *config,
				    const struct nlattr *value)
{
	char serial[17];
	size_t len;

	if (config->scalar) {
		printf("%u", *(uint8_t *)attr_data(value));
		return;
	}

	if (config->key == OMCI_CONFIG_SERIAL_NUMBER) {
		len = format_gpon_serial(attr_data(value),
					 attr_payload_len(value), serial,
					 sizeof(serial));
		if (len) {
			print_json_string(serial, len);
			return;
		}
	}

	print_json_string(attr_data(value), attr_payload_len(value));
}

static void print_config_text_value(const struct config_name *config,
				    const struct nlattr *value)
{
	char serial[17];
	size_t len;

	if (config->scalar) {
		printf("%u", *(uint8_t *)attr_data(value));
		return;
	}

	if (config->key == OMCI_CONFIG_SERIAL_NUMBER) {
		len = format_gpon_serial(attr_data(value),
					 attr_payload_len(value), serial,
					 sizeof(serial));
		if (len) {
			fwrite(serial, 1, len, stdout);
			return;
		}
	}

	fwrite(attr_data(value), 1, attr_payload_len(value), stdout);
}

static int config_reply(struct nlmsghdr *nlh, void *arg)
{
	struct config_reply_ctx *ctx = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	struct nlattr *value;
	uint8_t source;

	parse_attrs(nlh, a);
	value = a[OMCI_ATTR_CONFIG_VALUE];
	if (!value)
		return -EPROTO;
	source = get_u8(a[OMCI_ATTR_CONFIG_SOURCE]);
	if (json_output) {
		if (ctx->print_name) {
			print_json_string(ctx->config->name,
					  strlen(ctx->config->name));
			putchar(':');
		}
		if (ctx->include_source) {
			fputs("{\"value\":", stdout);
			print_config_json_value(ctx->config, value);
			printf(",\"source\":%u,\"source_name\":", source);
			print_json_string(config_source_name(source),
					  strlen(config_source_name(source)));
			putchar('}');
		} else {
			print_config_json_value(ctx->config, value);
		}
	} else {
		if (ctx->print_name)
			printf("%s: ", ctx->config->name);
		print_config_text_value(ctx->config, value);
		if (ctx->include_source)
			printf(" [source=%s]", config_source_name(source));
		putchar('\n');
	}
	return 0;
}

static int command_config_get(struct nl_ctx *ctx, uint32_t dev_id,
			      const struct config_name *config, bool print_name,
			      bool include_source)
{
	struct config_reply_ctx reply = { config, print_name, include_source };
	struct nl_request req;

	request_init(ctx, &req, OMCI_CMD_CONFIG_GET, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CONFIG_KEY, config->key);
	return request_exec(ctx, &req, true, config_reply, &reply);
}

static int command_config_set(struct nl_ctx *ctx, uint32_t dev_id,
			      const struct config_name *config, const char *text)
{
	struct nl_request req;
	uint8_t scalar;
	const void *value = text;
	size_t len = strlen(text);
	uint32_t parsed;

	if (config->scalar) {
		if (config->key == OMCI_CONFIG_OLT_PROFILE ||
		    config->key == OMCI_CONFIG_OLT_PROFILE_FORCE) {
			if (parse_olt_profile(text,
				config->key == OMCI_CONFIG_OLT_PROFILE_FORCE,
				&scalar))
				return -EINVAL;
		} else {
			if (parse_u32(text, &parsed) || parsed > UINT8_MAX)
				return -EINVAL;
			scalar = (uint8_t)parsed;
		}
		value = &scalar;
		len = sizeof(scalar);
	}
	request_init(ctx, &req, OMCI_CMD_CONFIG_SET, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CONFIG_KEY, config->key);
	if (add_attr(req.nlh, sizeof(req.buf), OMCI_ATTR_CONFIG_VALUE, value, len))
		return -EMSGSIZE;
	return request_exec(ctx, &req, false, NULL, NULL);
}

static int command_config_list(struct nl_ctx *ctx, uint32_t dev_id)
{
	size_t i;
	int ret;

	if (json_output)
		putchar('{');
	for (i = 0; i < ARRAY_SIZE(config_names); i++) {
		if (json_output && i)
			putchar(',');
		ret = command_config_get(ctx, dev_id, &config_names[i], true, false);
		if (ret)
			return ret;
	}
	if (json_output)
		puts("}");
	return 0;
}

static int command_config_details(struct nl_ctx *ctx, uint32_t dev_id)
{
	size_t i;
	int ret;

	if (json_output)
		putchar('{');
	for (i = 0; i < ARRAY_SIZE(config_names); i++) {
		if (json_output && i)
			putchar(',');
		ret = command_config_get(ctx, dev_id, &config_names[i], true,
					 true);
		if (ret)
			return ret;
	}
	if (json_output)
		puts("}");
	return 0;
}

static const char *class_category_name(uint8_t category)
{
	switch (category) {
	case OMCI_CLASS_CATEGORY_EQUIPMENT:
		return "equipment";
	case OMCI_CLASS_CATEGORY_ANI:
		return "ani";
	case OMCI_CLASS_CATEGORY_UNI:
		return "uni";
	case OMCI_CLASS_CATEGORY_LAYER2:
		return "layer2";
	case OMCI_CLASS_CATEGORY_LAYER3:
		return "layer3";
	case OMCI_CLASS_CATEGORY_MULTICAST:
		return "multicast";
	case OMCI_CLASS_CATEGORY_VOICE:
		return "voice";
	case OMCI_CLASS_CATEGORY_SECURITY:
		return "security";
	case OMCI_CLASS_CATEGORY_MANAGEMENT:
		return "management";
	case OMCI_CLASS_CATEGORY_PERFORMANCE:
		return "performance";
	case OMCI_CLASS_CATEGORY_XDSL:
		return "xdsl";
	case OMCI_CLASS_CATEGORY_LEGACY:
		return "legacy";
	case OMCI_CLASS_CATEGORY_VENDOR:
		return "vendor";
	case OMCI_CLASS_CATEGORY_RESERVED:
		return "reserved";
	case OMCI_CLASS_CATEGORY_OTHER:
		return "other";
	default:
		return "unspecified";
	}
}

static const char *class_support_name(uint8_t support)
{
	switch (support) {
	case OMCI_CLASS_SUPPORT_CATALOG:
		return "catalog";
	case OMCI_CLASS_SUPPORT_SHADOW:
		return "shadow";
	case OMCI_CLASS_SUPPORT_PARSED:
		return "parsed";
	case OMCI_CLASS_SUPPORT_PROVISIONED:
		return "provisioned";
	case OMCI_CLASS_SUPPORT_NATIVE:
		return "native";
	default:
		return "unknown";
	}
}

static int class_reply(struct nlmsghdr *nlh, void *arg)
{
	struct parsed_class *class = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];

	parse_attrs(nlh, a);
	class->class_id = get_u16(a[OMCI_ATTR_CLASS_ID]);
	class->category = get_u8(a[OMCI_ATTR_CLASS_CATEGORY]);
	class->support = get_u8(a[OMCI_ATTR_CLASS_SUPPORT]);
	class->flags = get_u32(a[OMCI_ATTR_CLASS_FLAGS]);
	class->next_index = get_u32(a[OMCI_ATTR_INDEX]);
	copy_attr_string(class->name, sizeof(class->name), a[OMCI_ATTR_NAME]);

	return 0;
}

static void print_class(const struct parsed_class *class)
{
	if (json_output) {
		printf("{\"class_id\":%u,\"name\":", class->class_id);
		print_json_string(class->name, strlen(class->name));
		printf(",\"category\":%u,\"category_name\":",
		       class->category);
		print_json_string(class_category_name(class->category),
				  strlen(class_category_name(class->category)));
		printf(",\"support\":%u,\"support_name\":",
		       class->support);
		print_json_string(class_support_name(class->support),
				  strlen(class_support_name(class->support)));
		printf(",\"flags\":%u}", class->flags);
	} else {
		printf("class=%u name=%s category=%s support=%s flags=0x%08x\n",
		       class->class_id, class->name,
		       class_category_name(class->category),
		       class_support_name(class->support), class->flags);
	}
}

static int command_class_get(struct nl_ctx *ctx, uint32_t dev_id,
			     uint16_t class_id, struct parsed_class *class)
{
	struct nl_request req;

	memset(class, 0, sizeof(*class));
	request_init(ctx, &req, OMCI_CMD_CLASS_GET, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	return request_exec(ctx, &req, true, class_reply, class);
}

static int command_class_next(struct nl_ctx *ctx, uint32_t dev_id,
			      uint32_t index, struct parsed_class *class)
{
	struct nl_request req;

	memset(class, 0, sizeof(*class));
	request_init(ctx, &req, OMCI_CMD_CLASS_NEXT, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u32(&req, OMCI_ATTR_INDEX, index);
	return request_exec(ctx, &req, true, class_reply, class);
}

static int command_class_list(struct nl_ctx *ctx, uint32_t dev_id)
{
	struct parsed_class class;
	uint32_t index = 0;
	bool first = true;
	int ret;

	if (json_output)
		putchar('[');
	for (;;) {
		ret = command_class_next(ctx, dev_id, index, &class);
		if (ret == -ENOENT)
			break;
		if (ret)
			return ret;
		if (json_output && !first)
			putchar(',');
		print_class(&class);
		first = false;
		if (class.next_index <= index)
			return -EPROTO;
		index = class.next_index;
	}
	if (json_output)
		puts("]");
	return 0;
}

static int mib_reply(struct nlmsghdr *nlh, void *arg)
{
	struct parsed_mib *mib = arg;
	struct nlattr *a[OMCI_ATTR_MAX + 1];
	struct nlattr *data;
	struct nlattr *name;

	parse_attrs(nlh, a);
	mib->class_id = get_u16(a[OMCI_ATTR_CLASS_ID]);
	mib->entity_id = get_u16(a[OMCI_ATTR_ENTITY_ID]);
	mib->mask = get_u16(a[OMCI_ATTR_ATTR_MASK]);
	mib->origin = get_u8(a[OMCI_ATTR_ORIGIN]);
	mib->next_index = get_u32(a[OMCI_ATTR_INDEX]);
	data = a[OMCI_ATTR_ATTR_DATA];
	if (data) {
		mib->data_len = (size_t)attr_payload_len(data);
		if (mib->data_len > sizeof(mib->data))
			mib->data_len = sizeof(mib->data);
		memcpy(mib->data, attr_data(data), mib->data_len);
	}
	if (a[OMCI_ATTR_OLT_G])
		parse_olt_g(&mib->olt_g, a[OMCI_ATTR_OLT_G]);
	if (a[OMCI_ATTR_VLAN_TAGGING_FILTER])
		parse_vlan_filter(&mib->vlan_filter,
				  a[OMCI_ATTR_VLAN_TAGGING_FILTER]);
	if (a[OMCI_ATTR_EXTENDED_VLAN])
		parse_ext_vlan(&mib->ext_vlan, a[OMCI_ATTR_EXTENDED_VLAN]);
	name = a[OMCI_ATTR_NAME];
	if (name) {
		size_t len = (size_t)attr_payload_len(name);

		if (len >= sizeof(mib->name))
			len = sizeof(mib->name) - 1;
		memcpy(mib->name, attr_data(name), len);
		mib->name[len] = '\0';
	}
	return 0;
}

static void print_olt_g_json(const struct parsed_olt_g *olt)
{
	printf(",\"olt_g\":{\"vendor_id\":");
	print_json_string(olt->vendor_id, strlen(olt->vendor_id));
	printf(",\"equipment_id\":");
	print_json_string(olt->equipment_id, strlen(olt->equipment_id));
	printf(",\"version\":");
	print_json_string(olt->version, strlen(olt->version));
	putchar('}');
}

static void print_vlan_filter_json(const struct parsed_vlan_filter *filter)
{
	unsigned int i;

	printf(",\"vlan_filter\":{\"forward_operation\":%u,"
	       "\"number_of_entries\":%u,\"entries\":[",
	       filter->forward_operation, filter->num_entries);
	for (i = 0; i < filter->num_entries; i++) {
		const struct parsed_vlan_filter_entry *entry = &filter->entries[i];

		if (i)
			putchar(',');
		printf("{\"index\":%u,\"tci\":%u,\"pbit\":%u,"
		       "\"dei\":%u,\"vid\":%u}",
		       i, entry->tci, entry->pbit, entry->dei, entry->vid);
	}
	printf("]}");
}

static void print_ext_vlan_json(const struct parsed_ext_vlan *vlan)
{
	unsigned int i;

	printf(",\"extended_vlan\":{\"association_type\":%u,"
	       "\"max_table_size\":%u,\"input_tpid\":%u,"
	       "\"output_tpid\":%u,\"downstream_mode\":%u,"
	       "\"associated_me\":%u,\"dscp_to_pbit\":\"",
	       vlan->association_type, vlan->max_table_size,
	       vlan->input_tpid, vlan->output_tpid, vlan->downstream_mode,
	       vlan->associated_me);
	print_hex_inline(vlan->dscp_to_pbit, sizeof(vlan->dscp_to_pbit));
	printf("\",\"rules\":[");
	for (i = 0; i < vlan->rule_count; i++) {
		const struct parsed_ext_vlan_rule *rule = &vlan->rules[i];

		if (i)
			putchar(',');
		printf("{\"index\":%u,\"raw\":\"", i);
		print_hex_inline(rule->raw, sizeof(rule->raw));
		printf("\",\"delete\":%s,\"filter_outer_pbit\":%u,"
		       "\"filter_outer_vid\":%u,"
		       "\"filter_outer_tpid_dei\":%u,"
		       "\"filter_inner_pbit\":%u,"
		       "\"filter_inner_vid\":%u,"
		       "\"filter_inner_tpid_dei\":%u,"
		       "\"filter_ethertype\":%u,\"tags_to_remove\":%u,"
		       "\"treat_outer_pbit\":%u,\"treat_outer_vid\":%u,"
		       "\"treat_outer_tpid_dei\":%u,"
		       "\"treat_inner_pbit\":%u,\"treat_inner_vid\":%u,"
		       "\"treat_inner_tpid_dei\":%u}",
		       rule->delete ? "true" : "false",
		       rule->filter_outer_pbit, rule->filter_outer_vid,
		       rule->filter_outer_tpid_dei, rule->filter_inner_pbit,
		       rule->filter_inner_vid, rule->filter_inner_tpid_dei,
		       rule->filter_ethertype, rule->tags_to_remove,
		       rule->treat_outer_pbit, rule->treat_outer_vid,
		       rule->treat_outer_tpid_dei, rule->treat_inner_pbit,
		       rule->treat_inner_vid, rule->treat_inner_tpid_dei);
	}
	printf("]}");
}

static void print_olt_g_text(const struct parsed_olt_g *olt)
{
	printf("  olt-g: vendor=%s equipment=%s version=%s\n",
	       olt->vendor_id[0] ? olt->vendor_id : "-",
	       olt->equipment_id[0] ? olt->equipment_id : "-",
	       olt->version[0] ? olt->version : "-");
}

static void print_vlan_filter_text(const struct parsed_vlan_filter *filter)
{
	unsigned int i;

	printf("  vlan-filter: forward=%u entries=%u\n",
	       filter->forward_operation, filter->num_entries);
	for (i = 0; i < filter->num_entries; i++) {
		const struct parsed_vlan_filter_entry *entry = &filter->entries[i];

		printf("    [%u] tci=0x%04x pbit=%u dei=%u vid=%u\n",
		       i, entry->tci, entry->pbit, entry->dei, entry->vid);
	}
}

static void print_ext_vlan_text(const struct parsed_ext_vlan *vlan)
{
	unsigned int i;

	printf("  extended-vlan: association=%u pointer=0x%04x input-tpid=0x%04x "
	       "output-tpid=0x%04x downstream=%u max=%u rules=%u\n",
	       vlan->association_type, vlan->associated_me, vlan->input_tpid,
	       vlan->output_tpid, vlan->downstream_mode, vlan->max_table_size,
	       vlan->rule_count);
	for (i = 0; i < vlan->rule_count; i++) {
		const struct parsed_ext_vlan_rule *rule = &vlan->rules[i];

		printf("    [%u] raw=", i);
		print_hex_inline(rule->raw, sizeof(rule->raw));
		printf(" filter={outer:%u/%u/%u inner:%u/%u/%u eth:%u} "
		       "treatment={remove:%u outer:%u/%u/%u inner:%u/%u/%u}\n",
		       rule->filter_outer_pbit, rule->filter_outer_vid,
		       rule->filter_outer_tpid_dei, rule->filter_inner_pbit,
		       rule->filter_inner_vid, rule->filter_inner_tpid_dei,
		       rule->filter_ethertype, rule->tags_to_remove,
		       rule->treat_outer_pbit, rule->treat_outer_vid,
		       rule->treat_outer_tpid_dei, rule->treat_inner_pbit,
		       rule->treat_inner_vid, rule->treat_inner_tpid_dei);
	}
}

static void print_mib(const struct parsed_mib *mib)
{
	if (json_output) {
		printf("{\"class_id\":%u,\"entity_id\":%u,\"name\":",
		       mib->class_id, mib->entity_id);
		print_json_string(mib->name, strlen(mib->name));
		printf(",\"mask\":%u,\"origin\":%u,\"data\":\"",
		       mib->mask, mib->origin);
		print_hex_inline(mib->data, mib->data_len);
		putchar('"');
		if (mib->olt_g.present)
			print_olt_g_json(&mib->olt_g);
		if (mib->vlan_filter.present)
			print_vlan_filter_json(&mib->vlan_filter);
		if (mib->ext_vlan.present)
			print_ext_vlan_json(&mib->ext_vlan);
		putchar('}');
	} else {
		printf("class=%u entity=%u mask=0x%04x origin=%u name=%s data=",
		       mib->class_id, mib->entity_id, mib->mask, mib->origin,
		       mib->name);
		print_hex_inline(mib->data, mib->data_len);
		putchar('\n');
		if (mib->olt_g.present)
			print_olt_g_text(&mib->olt_g);
		if (mib->vlan_filter.present)
			print_vlan_filter_text(&mib->vlan_filter);
		if (mib->ext_vlan.present)
			print_ext_vlan_text(&mib->ext_vlan);
	}
}

static int command_mib_get(struct nl_ctx *ctx, uint32_t dev_id,
			   uint16_t class_id, uint16_t entity_id,
			   struct parsed_mib *mib)
{
	struct nl_request req;

	memset(mib, 0, sizeof(*mib));
	request_init(ctx, &req, OMCI_CMD_MIB_GET, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	return request_exec(ctx, &req, true, mib_reply, mib);
}

static int command_mib_next(struct nl_ctx *ctx, uint32_t dev_id,
			    uint32_t index, struct parsed_mib *mib)
{
	struct nl_request req;

	memset(mib, 0, sizeof(*mib));
	request_init(ctx, &req, OMCI_CMD_MIB_NEXT, true);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u32(&req, OMCI_ATTR_INDEX, index);
	return request_exec(ctx, &req, true, mib_reply, mib);
}

static int command_mib_list(struct nl_ctx *ctx, uint32_t dev_id)
{
	struct parsed_mib mib;
	uint32_t index = 0;
	bool first = true;
	int ret;

	if (json_output)
		putchar('[');
	for (;;) {
		ret = command_mib_next(ctx, dev_id, index, &mib);
		if (ret == -ENOENT)
			break;
		if (ret)
			return ret;
		if (json_output && !first)
			putchar(',');
		print_mib(&mib);
		first = false;
		if (mib.next_index <= index)
			return -EPROTO;
		index = mib.next_index;
	}
	if (json_output)
		puts("]");
	return 0;
}

static int command_mib_set(struct nl_ctx *ctx, uint32_t dev_id,
			   uint16_t class_id, uint16_t entity_id,
			   uint16_t mask, const char *hex)
{
	uint8_t data[OMCI_MAX_ATTR_DATA] = {};
	struct nl_request req;
	size_t len;
	int ret;

	ret = parse_hex(hex, data, sizeof(data), &len);
	if (ret)
		return ret;
	request_init(ctx, &req, OMCI_CMD_MIB_SET, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	add_u16(&req, OMCI_ATTR_ATTR_MASK, mask);
	add_attr(req.nlh, sizeof(req.buf), OMCI_ATTR_ATTR_DATA, data, len);
	return request_exec(ctx, &req, false, NULL, NULL);
}

static int command_mib_delete(struct nl_ctx *ctx, uint32_t dev_id,
			      uint16_t class_id, uint16_t entity_id)
{
	struct nl_request req;

	request_init(ctx, &req, OMCI_CMD_MIB_DELETE, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u16(&req, OMCI_ATTR_CLASS_ID, class_id);
	add_u16(&req, OMCI_ATTR_ENTITY_ID, entity_id);
	return request_exec(ctx, &req, false, NULL, NULL);
}

static int command_mib_reset(struct nl_ctx *ctx, uint32_t dev_id)
{
	struct nl_request req;

	request_init(ctx, &req, OMCI_CMD_MIB_RESET, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	return request_exec(ctx, &req, false, NULL, NULL);
}

static int command_agent(struct nl_ctx *ctx, uint32_t dev_id,
			 const char *mode)
{
	struct nl_request req;
	uint16_t attr;
	uint8_t value;

	if (!strcmp(mode, "enable")) {
		attr = OMCI_ATTR_AGENT_ENABLED;
		value = 1;
	} else if (!strcmp(mode, "disable")) {
		attr = OMCI_ATTR_AGENT_ENABLED;
		value = 0;
	} else if (!strcmp(mode, "permissive")) {
		attr = OMCI_ATTR_AGENT_PERMISSIVE;
		value = 1;
	} else if (!strcmp(mode, "strict")) {
		attr = OMCI_ATTR_AGENT_PERMISSIVE;
		value = 0;
	} else if (!strcmp(mode, "fake-enable")) {
		attr = OMCI_ATTR_AGENT_FAKE_OMCI;
		value = 1;
	} else if (!strcmp(mode, "fake-disable")) {
		attr = OMCI_ATTR_AGENT_FAKE_OMCI;
		value = 0;
	} else if (!strcmp(mode, "dying-gasp-enable")) {
		attr = OMCI_ATTR_AGENT_DYING_GASP;
		value = 1;
	} else if (!strcmp(mode, "dying-gasp-disable")) {
		attr = OMCI_ATTR_AGENT_DYING_GASP;
		value = 0;
	} else {
		return -EINVAL;
	}
	request_init(ctx, &req, OMCI_CMD_AGENT_SET, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_u8(&req, attr, value);
	return request_exec(ctx, &req, false, NULL, NULL);
}

static int bind_observer(struct nl_ctx *ctx, uint32_t dev_id, bool bind)
{
	struct nl_request req;

	request_init(ctx, &req, bind ? OMCI_CMD_BIND : OMCI_CMD_UNBIND, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	return request_exec(ctx, &req, false, NULL, NULL);
}

static const char *event_name(uint8_t event)
{
	switch (event) {
	case OMCI_EVENT_CHANNEL_UP: return "channel-up";
	case OMCI_EVENT_CHANNEL_DOWN: return "channel-down";
	case OMCI_EVENT_STATE_CHANGE: return "state-change";
	case OMCI_EVENT_MIB_CHANGE: return "mib-change";
	case OMCI_EVENT_CONFIG_CHANGE: return "config-change";
	case OMCI_EVENT_UNSUPPORTED: return "unsupported";
	case OMCI_EVENT_PROFILE_CHANGE: return "profile-change";
	case OMCI_EVENT_OPERATIONAL_CHANGE: return "operational-change";
	case OMCI_EVENT_DYING_GASP: return "dying-gasp";
	default: return "unknown";
	}
}

static void process_async(struct nlmsghdr *nlh)
{
	struct genlmsghdr *genl;
	struct nlattr *a[OMCI_ATTR_MAX + 1];

	if (nlh->nlmsg_type == NLMSG_ERROR)
		return;
	genl = NLMSG_DATA(nlh);
	parse_attrs(nlh, a);
	if (genl->cmd == OMCI_CMD_RX) {
		struct nlattr *pdu = a[OMCI_ATTR_PDU];

		if (json_output) {
			printf("{\"type\":\"rx\",\"sequence\":%llu,\"onu_id\":%u,"
			       "\"gem_port\":%u,\"flags\":%u,\"pdu\":\"",
			       (unsigned long long)get_u64(a[OMCI_ATTR_SEQUENCE]),
			       get_u16(a[OMCI_ATTR_ONU_ID]),
			       get_u16(a[OMCI_ATTR_GEM_PORT_ID]),
			       get_u32(a[OMCI_ATTR_FLAGS]));
			if (pdu)
				print_hex_inline(attr_data(pdu), attr_payload_len(pdu));
			puts("\"}");
		} else {
			printf("rx seq=%llu onu=%u gem=%u flags=0x%x pdu=",
			       (unsigned long long)get_u64(a[OMCI_ATTR_SEQUENCE]),
			       get_u16(a[OMCI_ATTR_ONU_ID]),
			       get_u16(a[OMCI_ATTR_GEM_PORT_ID]),
			       get_u32(a[OMCI_ATTR_FLAGS]));
			if (pdu)
				print_hex_inline(attr_data(pdu), attr_payload_len(pdu));
			putchar('\n');
		}
	} else if (genl->cmd == OMCI_CMD_EVENT) {
		if (json_output)
			printf("{\"type\":\"event\",\"event\":\"%s\","
			       "\"state\":%u,\"onu_id\":%u,\"gem_port\":%u}\n",
			       event_name(get_u8(a[OMCI_ATTR_EVENT])),
			       get_u8(a[OMCI_ATTR_STATE]),
			       get_u16(a[OMCI_ATTR_ONU_ID]),
			       get_u16(a[OMCI_ATTR_GEM_PORT_ID]));
		else
			printf("event %s state=O%u onu=%u gem=%u\n",
			       event_name(get_u8(a[OMCI_ATTR_EVENT])),
			       get_u8(a[OMCI_ATTR_STATE]),
			       get_u16(a[OMCI_ATTR_ONU_ID]),
			       get_u16(a[OMCI_ATTR_GEM_PORT_ID]));
	}
	fflush(stdout);
}

static int command_monitor(struct nl_ctx *ctx, uint32_t dev_id)
{
	char buf[NL_BUF_SIZE];
	int ret = bind_observer(ctx, dev_id, true);

	if (ret)
		return ret;
	while (!stop_requested) {
		ssize_t len = recv(ctx->fd, buf, sizeof(buf), 0);
		struct nlmsghdr *nlh;

		if (len < 0) {
			if (errno == EINTR)
				continue;
			ret = -errno;
			break;
		}
		for (nlh = (struct nlmsghdr *)buf;
		     NLMSG_OK(nlh, (unsigned int)len); nlh = NLMSG_NEXT(nlh, len))
			process_async(nlh);
	}
	bind_observer(ctx, dev_id, false);
	return ret;
}

static int command_raw_tx(struct nl_ctx *ctx, uint32_t dev_id, const char *hex)
{
	uint8_t pdu[OMCI_MAX_PDU_LEN];
	struct nl_request req;
	size_t len;
	int ret;

	ret = parse_hex(hex, pdu, sizeof(pdu), &len);
	if (ret)
		return ret;
	ret = bind_observer(ctx, dev_id, true);
	if (ret)
		return ret;
	request_init(ctx, &req, OMCI_CMD_TX, false);
	add_u32(&req, OMCI_ATTR_DEV_ID, dev_id);
	add_attr(req.nlh, sizeof(req.buf), OMCI_ATTR_PDU, pdu, len);
	ret = request_exec(ctx, &req, false, NULL, NULL);
	bind_observer(ctx, dev_id, false);
	return ret;
}

static void usage(const char *program)
{
	fprintf(stderr,
		"usage: %s [--json] [--device ID] COMMAND\n"
		"  status\n"
		"  agent enable|disable|permissive|strict|fake-enable|fake-disable|dying-gasp-enable|dying-gasp-disable\n"
		"  config list\n"
		"  config details\n"
		"  config get KEY\n"
		"  config inspect KEY\n"
		"  config set KEY VALUE\n"
		"  class list\n"
		"  class show CLASS\n"
		"  mib list\n"
		"  mib show CLASS ENTITY\n"
		"  mib set CLASS ENTITY MASK HEX-DATA\n"
		"  mib delete CLASS ENTITY\n"
		"  mib reset\n"
		"  monitor\n"
		"  raw-tx HEX-PDU\n", program);
}

int main(int argc, char **argv)
{
	struct nl_ctx ctx;
	uint32_t dev_id = 0;
	int index = 1;
	int ret = -EINVAL;

	while (index < argc && argv[index][0] == '-') {
		if (!strcmp(argv[index], "--json") || !strcmp(argv[index], "-j")) {
			json_output = true;
			index++;
		} else if (!strcmp(argv[index], "--device") ||
			   !strcmp(argv[index], "-d")) {
			if (++index >= argc || parse_u32(argv[index], &dev_id)) {
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			index++;
		} else {
			break;
		}
	}
	if (index >= argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	ret = nl_open(&ctx);
	if (ret)
		goto out;
	ret = resolve_family(&ctx);
	if (ret)
		goto close_socket;
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	if (!strcmp(argv[index], "status") && index + 1 == argc) {
		ret = command_status(&ctx, dev_id);
	} else if (!strcmp(argv[index], "agent") && index + 2 == argc) {
		ret = command_agent(&ctx, dev_id, argv[index + 1]);
	} else if (!strcmp(argv[index], "config") && index + 2 <= argc) {
		const char *sub = argv[index + 1];

		if (!strcmp(sub, "list") && index + 2 == argc) {
			ret = command_config_list(&ctx, dev_id);
		} else if (!strcmp(sub, "details") && index + 2 == argc) {
			ret = command_config_details(&ctx, dev_id);
		} else if (!strcmp(sub, "get") && index + 3 == argc) {
			const struct config_name *config = find_config(argv[index + 2]);

			ret = config ? command_config_get(&ctx, dev_id, config, false,
						  false) : -EINVAL;
		} else if (!strcmp(sub, "inspect") && index + 3 == argc) {
			const struct config_name *config = find_config(argv[index + 2]);

			ret = config ? command_config_get(&ctx, dev_id, config, false,
						  true) : -EINVAL;
		} else if (!strcmp(sub, "set") && index + 4 == argc) {
			const struct config_name *config = find_config(argv[index + 2]);

			ret = config ? command_config_set(&ctx, dev_id, config,
							  argv[index + 3]) : -EINVAL;
		}
	} else if (!strcmp(argv[index], "class") && index + 2 <= argc) {
		const char *sub = argv[index + 1];
		uint32_t class_id;
		struct parsed_class class;

		if (!strcmp(sub, "list") && index + 2 == argc) {
			ret = command_class_list(&ctx, dev_id);
		} else if (!strcmp(sub, "show") && index + 3 == argc &&
			   !parse_u32(argv[index + 2], &class_id) &&
			   class_id <= UINT16_MAX) {
			ret = command_class_get(&ctx, dev_id, (uint16_t)class_id,
					    &class);
			if (!ret) {
				print_class(&class);
				if (json_output)
					putchar('\n');
			}
		}
	} else if (!strcmp(argv[index], "mib") && index + 2 <= argc) {
		const char *sub = argv[index + 1];
		uint32_t class_id;
		uint32_t entity_id;
		uint32_t mask;
		struct parsed_mib mib;

		if (!strcmp(sub, "list") && index + 2 == argc) {
			ret = command_mib_list(&ctx, dev_id);
		} else if (!strcmp(sub, "reset") && index + 2 == argc) {
			ret = command_mib_reset(&ctx, dev_id);
		} else if (!strcmp(sub, "show") && index + 4 == argc &&
			   !parse_u32(argv[index + 2], &class_id) &&
			   !parse_u32(argv[index + 3], &entity_id) &&
			   class_id <= UINT16_MAX && entity_id <= UINT16_MAX) {
			ret = command_mib_get(&ctx, dev_id, (uint16_t)class_id,
					      (uint16_t)entity_id, &mib);
			if (!ret) {
				print_mib(&mib);
				if (json_output)
					putchar('\n');
			}
		} else if (!strcmp(sub, "delete") && index + 4 == argc &&
			   !parse_u32(argv[index + 2], &class_id) &&
			   !parse_u32(argv[index + 3], &entity_id) &&
			   class_id <= UINT16_MAX && entity_id <= UINT16_MAX) {
			ret = command_mib_delete(&ctx, dev_id, (uint16_t)class_id,
						 (uint16_t)entity_id);
		} else if (!strcmp(sub, "set") && index + 6 == argc &&
			   !parse_u32(argv[index + 2], &class_id) &&
			   !parse_u32(argv[index + 3], &entity_id) &&
			   !parse_u32(argv[index + 4], &mask) &&
			   class_id <= UINT16_MAX && entity_id <= UINT16_MAX &&
			   mask <= UINT16_MAX) {
			ret = command_mib_set(&ctx, dev_id, (uint16_t)class_id,
					      (uint16_t)entity_id, (uint16_t)mask,
					      argv[index + 5]);
		}
	} else if (!strcmp(argv[index], "monitor") && index + 1 == argc) {
		ret = command_monitor(&ctx, dev_id);
	} else if (!strcmp(argv[index], "raw-tx") && index + 2 == argc) {
		ret = command_raw_tx(&ctx, dev_id, argv[index + 1]);
	}

close_socket:
	close(ctx.fd);
out:
	if (ret) {
		if (ret == -EINVAL)
			usage(argv[0]);
		fprintf(stderr, "omci-netlink: %s\n", strerror(-ret));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
