// SPDX-License-Identifier: GPL-2.0-only

#include <errno.h>
#include <limits.h>
#include <net/if.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <linux/if_ether.h>
#include <linux/rtl931x_stack.h>

#include <netlink/attr.h>
#include <netlink/errno.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif
#ifndef NETLINK_EXT_ACK
#define NETLINK_EXT_ACK 11
#endif
#ifndef NLM_F_CAPPED
#define NLM_F_CAPPED 0x100
#endif
#ifndef NLM_F_ACK_TLVS
#define NLM_F_ACK_TLVS 0x200
#endif
#ifndef NLMSGERR_ATTR_MSG
#define NLMSGERR_ATTR_MSG 1
#endif
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

struct stack_nl {
	struct nl_sock *sock;
	int family;
};

struct stack_request {
	int cmd;
	unsigned int ifindex;
	unsigned int generation;
	unsigned int flags;
	unsigned char enabled;
	unsigned char member;
	unsigned char peer;
	unsigned char master;
	unsigned char remote_port;
};

struct request_context {
	int cmd;
	int family;
	int kernel_error;
	int parse_error;
	bool done;
	bool expect_reply;
	bool got_ack;
	bool got_reply;
	char extack[256];
};

static const struct nla_policy stack_policy[RTL931X_STACK_ATTR_MAX + 1] = {
	[RTL931X_STACK_ATTR_API_VERSION] = { .type = NLA_U16 },
	[RTL931X_STACK_ATTR_IFINDEX] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_ENABLED] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_MEMBER_ID] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_ID] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_MASTER_ID] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_FLAGS] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_GENERATION] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_STATE] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_LINK_UP] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_TALK_MODE] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_TALK_TRANSACTION] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_MASTER_ID] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_STACK_PORT] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_STATUS] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_REMOTE_GENERATION] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_ROUND_TRIP_US] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_REMOTE_PORT_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_PORT_COUNT] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_CPU_PORT] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_CAPABILITIES] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN] = { .type = NLA_U16 },
	[RTL931X_STACK_ATTR_REMOTE_PORT] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_REMOTE_PORT_MTU] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_REMOTE_PORT_MAC] = {
		.type = NLA_BINARY,
		.minlen = ETH_ALEN,
		.maxlen = ETH_ALEN,
	},
};

static const char *stack_state_name(unsigned char state)
{
	static const char * const names[] = {
		[RTL931X_STACK_STATE_DISABLED] = "disabled",
		[RTL931X_STACK_STATE_CONFIGURED] = "configured",
		[RTL931X_STACK_STATE_ERROR] = "error",
		[RTL931X_STACK_STATE_ARMED] = "armed",
		[RTL931X_STACK_STATE_PEER_VERIFIED] = "peer-verified",
	};

	if (state >= ARRAY_SIZE(names) || !names[state])
		return "unknown";
	return names[state];
}

static const char *talk_mode_name(unsigned char mode)
{
	switch (mode) {
	case RTL931X_STACK_TALK_MODE_ONE_HOP:
		return "one-hop";
	case RTL931X_STACK_TALK_MODE_UNICAST:
		return "unicast";
	default:
		return "unknown";
	}
}

static void print_bool(const char *name, bool value)
{
	printf("%s=%s\n", name, value ? "true" : "false");
}

static void print_stack_flags(unsigned int flags)
{
	printf("flags=0x%08x\n", flags);
	print_bool("flag_auto_learn", flags & RTL931X_STACK_F_AUTO_LEARN);
	print_bool("flag_drop_my_dev", flags & RTL931X_STACK_F_DROP_MY_DEV);
}

static void print_talk_status(unsigned int status)
{
	printf("remote_status=0x%08x\n", status);
	print_bool("remote_id_valid", status & RTL931X_STACK_TALK_S_ID_VALID);
	print_bool("remote_is_master", status & RTL931X_STACK_TALK_S_MASTER);
	print_bool("remote_configured", status & RTL931X_STACK_TALK_S_CONFIGURED);
	print_bool("remote_route_ready", status & RTL931X_STACK_TALK_S_ROUTE_READY);
	print_bool("remote_link_up", status & RTL931X_STACK_TALK_S_LINK_UP);
}

static void print_capabilities(unsigned int capabilities)
{
	printf("remote_capabilities=0x%08x\n", capabilities);
	print_bool("remote_cap_get_switch",
		   capabilities & RTL931X_STACK_PEER_CAP_GET_SWITCH);
	print_bool("remote_cap_get_port_state",
		   capabilities & RTL931X_STACK_PEER_CAP_GET_PORT_STATE);
	print_bool("remote_cap_set_delegated",
		   capabilities & RTL931X_STACK_PEER_CAP_SET_DELEGATED);
	print_bool("remote_cap_bridge_vlan",
		   capabilities & RTL931X_STACK_PEER_CAP_BRIDGE_VLAN);
}

static int require_attrs(struct nlattr **attrs, const int *required,
			 size_t count)
{
	size_t i;

	for (i = 0; i < count; i++)
		if (!attrs[required[i]])
			return -EPROTO;
	return 0;
}

static int print_status_reply(struct nlattr **attrs)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_API_VERSION,
		RTL931X_STACK_ATTR_IFINDEX,
		RTL931X_STACK_ATTR_ENABLED,
		RTL931X_STACK_ATTR_MEMBER_ID,
		RTL931X_STACK_ATTR_PEER_ID,
		RTL931X_STACK_ATTR_MASTER_ID,
		RTL931X_STACK_ATTR_FLAGS,
		RTL931X_STACK_ATTR_GENERATION,
		RTL931X_STACK_ATTR_STATE,
		RTL931X_STACK_ATTR_LINK_UP,
	};
	unsigned char state;
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	state = nla_get_u8(attrs[RTL931X_STACK_ATTR_STATE]);

	printf("ifindex=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]));
	print_bool("enabled", nla_get_u8(attrs[RTL931X_STACK_ATTR_ENABLED]));
	printf("member_id=%u\n", nla_get_u8(attrs[RTL931X_STACK_ATTR_MEMBER_ID]));
	printf("peer_id=%u\n", nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_ID]));
	printf("master_id=%u\n", nla_get_u8(attrs[RTL931X_STACK_ATTR_MASTER_ID]));
	printf("generation=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_GENERATION]));
	printf("state=%s\n", stack_state_name(state));
	printf("state_id=%u\n", state);
	print_bool("link_up", nla_get_u8(attrs[RTL931X_STACK_ATTR_LINK_UP]));
	print_stack_flags(nla_get_u32(attrs[RTL931X_STACK_ATTR_FLAGS]));

	return 0;
}

static int print_probe_reply(struct nlattr **attrs)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_API_VERSION,
		RTL931X_STACK_ATTR_IFINDEX,
		RTL931X_STACK_ATTR_TALK_MODE,
		RTL931X_STACK_ATTR_TALK_TRANSACTION,
		RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE,
		RTL931X_STACK_ATTR_REMOTE_MEMBER_ID,
		RTL931X_STACK_ATTR_REMOTE_MASTER_ID,
		RTL931X_STACK_ATTR_REMOTE_STACK_PORT,
		RTL931X_STACK_ATTR_REMOTE_STATUS,
		RTL931X_STACK_ATTR_REMOTE_GENERATION,
		RTL931X_STACK_ATTR_ROUND_TRIP_US,
	};
	unsigned long long transaction;
	unsigned long long boot_nonce;
	unsigned char mode;
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	mode = nla_get_u8(attrs[RTL931X_STACK_ATTR_TALK_MODE]);
	transaction = nla_get_u64(attrs[RTL931X_STACK_ATTR_TALK_TRANSACTION]);
	boot_nonce = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE]);

	printf("ifindex=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]));
	printf("talk_mode=%s\n", talk_mode_name(mode));
	printf("talk_mode_id=%u\n", mode);
	printf("transaction=0x%016llx\n", transaction);
	printf("remote_boot_nonce=0x%016llx\n", boot_nonce);
	printf("remote_member_id=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID]));
	printf("remote_master_id=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MASTER_ID]));
	printf("remote_stack_port=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_STACK_PORT]));
	printf("remote_generation=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_GENERATION]));
	printf("round_trip_us=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_ROUND_TRIP_US]));
	print_talk_status(nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_STATUS]));

	return 0;
}

static int print_rpc_session(struct nlattr **attrs)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_API_VERSION,
		RTL931X_STACK_ATTR_IFINDEX,
		RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE,
		RTL931X_STACK_ATTR_REMOTE_MEMBER_ID,
		RTL931X_STACK_ATTR_REMOTE_MASTER_ID,
		RTL931X_STACK_ATTR_REMOTE_GENERATION,
	};
	unsigned long long boot_nonce;
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	boot_nonce = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE]);

	printf("ifindex=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]));
	printf("remote_boot_nonce=0x%016llx\n", boot_nonce);
	printf("remote_member_id=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID]));
	printf("remote_master_id=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MASTER_ID]));
	printf("remote_generation=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_GENERATION]));

	return 0;
}

static int print_peer_switch_reply(struct nlattr **attrs)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_REMOTE_PORT_MASK,
		RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK,
		RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK,
		RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK,
		RTL931X_STACK_ATTR_REMOTE_PORT_COUNT,
		RTL931X_STACK_ATTR_REMOTE_CPU_PORT,
		RTL931X_STACK_ATTR_REMOTE_STACK_PORT,
		RTL931X_STACK_ATTR_REMOTE_CAPABILITIES,
		RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN,
	};
	unsigned long long port_mask;
	unsigned long long admin_up_mask;
	unsigned long long carrier_mask;
	unsigned long long delegated_mask;
	unsigned int capabilities;
	int err;

	err = print_rpc_session(attrs);
	if (err)
		return err;
	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	port_mask = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MASK]);
	admin_up_mask = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK]);
	carrier_mask = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK]);
	delegated_mask = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK]);
	capabilities = nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_CAPABILITIES]);

	printf("remote_port_mask=0x%016llx\n", port_mask);
	printf("remote_admin_up_mask=0x%016llx\n", admin_up_mask);
	printf("remote_carrier_mask=0x%016llx\n", carrier_mask);
	printf("remote_delegated_port_mask=0x%016llx\n", delegated_mask);
	printf("remote_port_count=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_COUNT]));
	printf("remote_cpu_port=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_CPU_PORT]));
	printf("remote_stack_port=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_STACK_PORT]));
	printf("remote_max_body_len=%u\n",
	       nla_get_u16(attrs[RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN]));
	print_capabilities(capabilities);

	return 0;
}

static int print_peer_port_reply(struct nlattr **attrs)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_REMOTE_PORT,
		RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS,
		RTL931X_STACK_ATTR_REMOTE_PORT_MTU,
		RTL931X_STACK_ATTR_REMOTE_PORT_MAC,
	};
	const unsigned char *mac;
	unsigned int flags;
	int err;

	err = print_rpc_session(attrs);
	if (err)
		return err;
	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	flags = nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS]);

	printf("remote_port=%u\n",
	       nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_PORT]));
	printf("remote_port_flags=0x%08x\n", flags);
	print_bool("remote_port_admin_up",
		   flags & RTL931X_STACK_PEER_PORT_F_ADMIN_UP);
	print_bool("remote_port_carrier",
		   flags & RTL931X_STACK_PEER_PORT_F_CARRIER);
	printf("remote_port_mtu=%u\n",
	       nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MTU]));
	mac = nla_data(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MAC]);
	printf("remote_port_mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
	       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return 0;
}

static void capture_extack(struct request_context *context,
			   struct nlmsgerr *error)
{
	struct nlmsghdr *header = (void *)((char *)error - NLMSG_HDRLEN);
	struct nlattr *attr;
	size_t payload_len;
	size_t offset;
	int remaining;

	if (!(header->nlmsg_flags & NLM_F_ACK_TLVS) ||
	    header->nlmsg_len < NLMSG_HDRLEN + sizeof(*error))
		return;

	payload_len = header->nlmsg_len - NLMSG_HDRLEN;
	offset = NLMSG_ALIGN(sizeof(*error));
	if (!(header->nlmsg_flags & NLM_F_CAPPED)) {
		if (error->msg.nlmsg_len < NLMSG_HDRLEN)
			return;
		offset += NLMSG_ALIGN(error->msg.nlmsg_len - NLMSG_HDRLEN);
	}
	if (offset > payload_len)
		return;

	attr = (void *)((char *)error + offset);
	remaining = payload_len - offset;
	while (nla_ok(attr, remaining)) {
		if (nla_type(attr) == NLMSGERR_ATTR_MSG && nla_len(attr) > 0) {
			size_t len = strnlen(nla_data(attr), nla_len(attr));

			if (len >= sizeof(context->extack))
				len = sizeof(context->extack) - 1;
			memcpy(context->extack, nla_data(attr), len);
			context->extack[len] = '\0';
			return;
		}
		attr = nla_next(attr, &remaining);
	}
}

static int error_handler(struct sockaddr_nl *address, struct nlmsgerr *error,
			 void *arg)
{
	struct request_context *context = arg;

	(void)address;
	context->kernel_error = error->error;
	capture_extack(context, error);
	context->done = true;
	return NL_SKIP;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	struct request_context *context = arg;

	(void)msg;
	context->got_ack = true;
	context->done = !context->expect_reply || context->got_reply;
	return context->done ? NL_STOP : NL_OK;
}

static int valid_handler(struct nl_msg *msg, void *arg)
{
	struct request_context *context = arg;
	struct nlmsghdr *header = nlmsg_hdr(msg);
	struct genlmsghdr *genl = nlmsg_data(header);
	struct nlattr *attrs[RTL931X_STACK_ATTR_MAX + 1];
	int err;

	if (header->nlmsg_type != context->family ||
	    !genlmsg_valid_hdr(header, 0)) {
		err = -EPROTO;
		goto out;
	}
	if (genl->cmd != context->cmd) {
		err = -EPROTO;
		goto out;
	}
	err = genlmsg_parse(header, 0, attrs, RTL931X_STACK_ATTR_MAX,
			    stack_policy);
	if (err < 0)
		goto out;
	if (!attrs[RTL931X_STACK_ATTR_API_VERSION] ||
	    nla_get_u16(attrs[RTL931X_STACK_ATTR_API_VERSION]) !=
		RTL931X_STACK_GENL_VERSION) {
		err = -EPROTONOSUPPORT;
		goto out;
	}

	switch (context->cmd) {
	case RTL931X_STACK_CMD_GET:
	case RTL931X_STACK_CMD_SET_TWO_MEMBER:
	case RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT:
		err = print_status_reply(attrs);
		break;
	case RTL931X_STACK_CMD_PROBE_PEER:
		err = print_probe_reply(attrs);
		break;
	case RTL931X_STACK_CMD_GET_PEER_SWITCH:
		err = print_peer_switch_reply(attrs);
		break;
	case RTL931X_STACK_CMD_GET_PEER_PORT:
		err = print_peer_port_reply(attrs);
		break;
	default:
		err = -EPROTO;
		break;
	}

out:
	context->parse_error = err;
	context->got_reply = true;
	context->done = context->got_ack;
	return context->done ? NL_STOP : NL_OK;
}

static bool command_expects_reply(int cmd)
{
	return cmd != RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS;
}

static int add_request_attrs(struct nl_msg *msg,
			     const struct stack_request *request)
{
	if (nla_put_u16(msg, RTL931X_STACK_ATTR_API_VERSION,
			RTL931X_STACK_GENL_VERSION) ||
	    nla_put_u32(msg, RTL931X_STACK_ATTR_IFINDEX, request->ifindex))
		return -NLE_NOMEM;

	switch (request->cmd) {
	case RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT:
	case RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS:
		if (nla_put_u8(msg, RTL931X_STACK_ATTR_ENABLED,
			       request->enabled))
			return -NLE_NOMEM;
		break;
	case RTL931X_STACK_CMD_SET_TWO_MEMBER:
		if (nla_put_u8(msg, RTL931X_STACK_ATTR_ENABLED,
			       request->enabled) ||
		    nla_put_u32(msg, RTL931X_STACK_ATTR_GENERATION,
				request->generation))
			return -NLE_NOMEM;
		if (!request->enabled)
			break;
		if (nla_put_u8(msg, RTL931X_STACK_ATTR_MEMBER_ID,
			       request->member) ||
		    nla_put_u8(msg, RTL931X_STACK_ATTR_PEER_ID,
			       request->peer) ||
		    nla_put_u8(msg, RTL931X_STACK_ATTR_MASTER_ID,
			       request->master) ||
		    nla_put_u32(msg, RTL931X_STACK_ATTR_FLAGS, request->flags))
			return -NLE_NOMEM;
		break;
	case RTL931X_STACK_CMD_GET_PEER_PORT:
		if (nla_put_u8(msg, RTL931X_STACK_ATTR_REMOTE_PORT,
			       request->remote_port))
			return -NLE_NOMEM;
		break;
	default:
		break;
	}

	return 0;
}

static int stack_request(struct stack_nl *stack,
			 const struct stack_request *request)
{
	struct request_context context = {
		.cmd = request->cmd,
		.family = stack->family,
		.expect_reply = command_expects_reply(request->cmd),
	};
	struct nl_cb *callbacks;
	struct nl_msg *msg;
	int err;

	msg = nlmsg_alloc();
	if (!msg)
		return -NLE_NOMEM;
	if (!genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, stack->family, 0, 0,
			 request->cmd, RTL931X_STACK_GENL_VERSION)) {
		err = -NLE_NOMEM;
		goto out_msg;
	}
	err = add_request_attrs(msg, request);
	if (err)
		goto out_msg;

	callbacks = nl_cb_alloc(NL_CB_CUSTOM);
	if (!callbacks) {
		err = -NLE_NOMEM;
		goto out_msg;
	}
	nl_cb_err(callbacks, NL_CB_CUSTOM, error_handler, &context);
	nl_cb_set(callbacks, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &context);
	nl_cb_set(callbacks, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, &context);

	err = nl_send_auto_complete(stack->sock, msg);
	if (err < 0)
		goto out_callbacks;
	while (!context.done) {
		err = nl_recvmsgs(stack->sock, callbacks);
		if (err < 0)
			goto out_callbacks;
	}

	if (context.kernel_error) {
		fprintf(stderr, "kernel rejected request: %s (%d)",
			strerror(-context.kernel_error), -context.kernel_error);
		if (context.extack[0])
			fprintf(stderr, ": %s", context.extack);
		fputc('\n', stderr);
		err = -NLE_FAILURE;
	} else if (context.parse_error) {
		fprintf(stderr, "invalid kernel reply: %s\n",
			strerror(-context.parse_error));
		err = -NLE_PROTO_MISMATCH;
	} else if (context.expect_reply && !context.got_reply) {
		fprintf(stderr, "kernel acknowledged request without a reply\n");
		err = -NLE_MSG_TRUNC;
	} else {
		err = 0;
	}

out_callbacks:
	nl_cb_put(callbacks);
out_msg:
	nlmsg_free(msg);
	return err;
}

static int stack_nl_open(struct stack_nl *stack)
{
	int enable = 1;
	int err;

	memset(stack, 0, sizeof(*stack));
	stack->sock = nl_socket_alloc();
	if (!stack->sock)
		return -NLE_NOMEM;
	err = genl_connect(stack->sock);
	if (err < 0)
		goto out_error;
	if (setsockopt(nl_socket_get_fd(stack->sock), SOL_NETLINK,
		       NETLINK_EXT_ACK, &enable, sizeof(enable)) < 0 &&
	    errno != ENOPROTOOPT) {
		fprintf(stderr, "warning: cannot enable netlink extack: %s\n",
			strerror(errno));
	}
	stack->family = genl_ctrl_resolve(stack->sock, RTL931X_STACK_GENL_NAME);
	if (stack->family < 0) {
		err = stack->family;
		goto out_error;
	}

	return 0;

out_error:
	nl_socket_free(stack->sock);
	stack->sock = NULL;
	return err;
}

static void stack_nl_close(struct stack_nl *stack)
{
	if (stack->sock)
		nl_socket_free(stack->sock);
}

static int parse_number(const char *text, unsigned long long maximum,
			unsigned long long *value)
{
	char *end;
	unsigned long long parsed;

	errno = 0;
	parsed = strtoull(text, &end, 0);
	if (errno || !text[0] || *end || parsed > maximum)
		return -EINVAL;
	*value = parsed;
	return 0;
}

static int parse_ifindex(const char *name, unsigned int *ifindex)
{
	*ifindex = if_nametoindex(name);
	if (!*ifindex) {
		fprintf(stderr, "unknown interface '%s': %s\n", name,
			strerror(errno));
		return -ENODEV;
	}
	return 0;
}

static int parse_enabled(const char *text, unsigned char *enabled)
{
	if (!strcmp(text, "enable")) {
		*enabled = 1;
		return 0;
	}
	if (!strcmp(text, "disable")) {
		*enabled = 0;
		return 0;
	}
	return -EINVAL;
}

static void usage(FILE *stream)
{
	fprintf(stream,
		"Usage:\n"
		"  rtl931x-stack status IFACE\n"
		"  rtl931x-stack get IFACE\n"
		"  rtl931x-stack talk-arm IFACE\n"
		"  rtl931x-stack talk-disarm IFACE\n"
		"  rtl931x-stack probe IFACE\n"
		"  rtl931x-stack enable IFACE MEMBER PEER MASTER GENERATION FLAGS\n"
		"  rtl931x-stack disable IFACE GENERATION\n"
		"  rtl931x-stack peer-switch IFACE\n"
		"  rtl931x-stack peer-port IFACE PORT\n"
		"  rtl931x-stack peer-netdevs IFACE enable|disable\n"
		"\n"
		"IDs are 0..15, ports are 0..55, and numbers accept decimal or 0x.\n"
		"FLAGS: bit 0 = auto-learn, bit 1 = drop-my-device (valid mask 0x3).\n");
}

static int parse_command(int argc, char **argv, struct stack_request *request)
{
	unsigned long long value;
	const char *command;
	int err;

	if (argc < 3)
		return -EINVAL;
	command = argv[1];
	err = parse_ifindex(argv[2], &request->ifindex);
	if (err)
		return err;

	if ((!strcmp(command, "status") || !strcmp(command, "get")) &&
	    argc == 3) {
		request->cmd = RTL931X_STACK_CMD_GET;
	} else if (!strcmp(command, "talk-arm") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT;
		request->enabled = 1;
	} else if (!strcmp(command, "talk-disarm") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT;
		request->enabled = 0;
	} else if (!strcmp(command, "probe") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_PROBE_PEER;
	} else if (!strcmp(command, "enable") && argc == 8) {
		request->cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER;
		request->enabled = 1;
		if (parse_number(argv[3], 15, &value))
			return -EINVAL;
		request->member = value;
		if (parse_number(argv[4], 15, &value))
			return -EINVAL;
		request->peer = value;
		if (parse_number(argv[5], 15, &value))
			return -EINVAL;
		request->master = value;
		if (parse_number(argv[6], UINT_MAX, &value))
			return -EINVAL;
		request->generation = value;
		if (parse_number(argv[7], RTL931X_STACK_F_MASK, &value))
			return -EINVAL;
		request->flags = value;
	} else if (!strcmp(command, "disable") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER;
		if (parse_number(argv[3], UINT_MAX, &value))
			return -EINVAL;
		request->generation = value;
	} else if (!strcmp(command, "peer-switch") && argc == 3) {
		request->cmd = RTL931X_STACK_CMD_GET_PEER_SWITCH;
	} else if (!strcmp(command, "peer-port") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_GET_PEER_PORT;
		if (parse_number(argv[3], 55, &value))
			return -EINVAL;
		request->remote_port = value;
	} else if (!strcmp(command, "peer-netdevs") && argc == 4) {
		request->cmd = RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS;
		if (parse_enabled(argv[3], &request->enabled))
			return -EINVAL;
	} else {
		return -EINVAL;
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct stack_request request = {};
	struct stack_nl stack;
	int err;

	err = parse_command(argc, argv, &request);
	if (err) {
		usage(stderr);
		return 2;
	}
	err = stack_nl_open(&stack);
	if (err) {
		fprintf(stderr, "cannot connect to %s: %s\n",
			RTL931X_STACK_GENL_NAME, nl_geterror(err));
		return 1;
	}
	err = stack_request(&stack, &request);
	stack_nl_close(&stack);
	if (err) {
		if (err != -NLE_FAILURE && err != -NLE_PROTO_MISMATCH &&
		    err != -NLE_MSG_TRUNC)
			fprintf(stderr, "netlink request failed: %s\n",
				nl_geterror(err));
		return 1;
	}

	return 0;
}
