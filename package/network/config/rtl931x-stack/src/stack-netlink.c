// SPDX-License-Identifier: GPL-2.0-only

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

#include <linux/rtl931x_stack.h>

#include <netlink/attr.h>
#include <netlink/errno.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>

#include "stack-netlink.h"

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

struct request_context {
	struct rtl931x_stack_nl *nl;
	union rtl931x_stack_reply *reply;
	int cmd;
	int parse_error;
	bool done;
	bool expect_reply;
	bool got_ack;
	bool got_reply;
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
	[RTL931X_STACK_ATTR_LOCAL_PORT_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_LOCAL_DELEGATED_PORT_MASK] = { .type = NLA_U64 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_DESIRED] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_ACTIVE] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_PUBLISHED] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_FENCED] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_RECOVERING] = { .type = NLA_U8 },
	[RTL931X_STACK_ATTR_PEER_NETDEVS_LAST_ERROR] = { .type = NLA_U32 },
};

static int require_attrs(struct nlattr **attrs, const int *required,
			 size_t count)
{
	size_t i;

	for (i = 0; i < count; i++)
		if (!attrs[required[i]])
			return -EPROTO;

	return 0;
}

static int parse_status(struct nlattr **attrs,
			struct rtl931x_stack_status *status)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_IFINDEX,
		RTL931X_STACK_ATTR_ENABLED,
		RTL931X_STACK_ATTR_MEMBER_ID,
		RTL931X_STACK_ATTR_PEER_ID,
		RTL931X_STACK_ATTR_MASTER_ID,
		RTL931X_STACK_ATTR_FLAGS,
		RTL931X_STACK_ATTR_GENERATION,
		RTL931X_STACK_ATTR_STATE,
		RTL931X_STACK_ATTR_LINK_UP,
		RTL931X_STACK_ATTR_LOCAL_PORT_MASK,
		RTL931X_STACK_ATTR_LOCAL_DELEGATED_PORT_MASK,
		RTL931X_STACK_ATTR_PEER_NETDEVS_DESIRED,
		RTL931X_STACK_ATTR_PEER_NETDEVS_ACTIVE,
		RTL931X_STACK_ATTR_PEER_NETDEVS_PUBLISHED,
		RTL931X_STACK_ATTR_PEER_NETDEVS_FENCED,
		RTL931X_STACK_ATTR_PEER_NETDEVS_RECOVERING,
		RTL931X_STACK_ATTR_PEER_NETDEVS_LAST_ERROR,
	};
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;

	status->ifindex = nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]);
	status->enabled = nla_get_u8(attrs[RTL931X_STACK_ATTR_ENABLED]);
	status->member = nla_get_u8(attrs[RTL931X_STACK_ATTR_MEMBER_ID]);
	status->peer = nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_ID]);
	status->master = nla_get_u8(attrs[RTL931X_STACK_ATTR_MASTER_ID]);
	status->flags = nla_get_u32(attrs[RTL931X_STACK_ATTR_FLAGS]);
	status->generation = nla_get_u32(attrs[RTL931X_STACK_ATTR_GENERATION]);
	status->state = nla_get_u8(attrs[RTL931X_STACK_ATTR_STATE]);
	status->link_up = nla_get_u8(attrs[RTL931X_STACK_ATTR_LINK_UP]);
	status->local_port_mask =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_LOCAL_PORT_MASK]);
	status->local_delegated_port_mask =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_LOCAL_DELEGATED_PORT_MASK]);
	status->peer_netdevs_desired =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_DESIRED]);
	status->peer_netdevs_active =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_ACTIVE]);
	status->peer_netdevs_published =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_PUBLISHED]);
	status->peer_netdevs_fenced =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_FENCED]);
	status->peer_netdevs_recovering =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_RECOVERING]);
	status->peer_netdevs_last_error =
		nla_get_u32(attrs[RTL931X_STACK_ATTR_PEER_NETDEVS_LAST_ERROR]);
	if (status->peer_netdevs_desired > 1 ||
	    status->peer_netdevs_active > 1 ||
	    status->peer_netdevs_published > 1 ||
	    status->peer_netdevs_fenced > 1 ||
	    status->peer_netdevs_recovering > 1)
		return -EPROTO;

	return 0;
}

static int parse_probe(struct nlattr **attrs,
		       struct rtl931x_stack_probe *probe)
{
	static const int required[] = {
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
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;

	probe->ifindex = nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]);
	probe->mode = nla_get_u8(attrs[RTL931X_STACK_ATTR_TALK_MODE]);
	probe->transaction =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_TALK_TRANSACTION]);
	probe->boot_nonce =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE]);
	probe->member =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID]);
	probe->master =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MASTER_ID]);
	probe->stack_port =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_STACK_PORT]);
	probe->status = nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_STATUS]);
	probe->generation =
		nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_GENERATION]);
	probe->round_trip_us =
		nla_get_u32(attrs[RTL931X_STACK_ATTR_ROUND_TRIP_US]);

	return 0;
}

static int parse_peer_session(struct nlattr **attrs, uint32_t *ifindex,
			      uint64_t *boot_nonce, uint8_t *member,
			      uint8_t *master, uint32_t *generation)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_IFINDEX,
		RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE,
		RTL931X_STACK_ATTR_REMOTE_MEMBER_ID,
		RTL931X_STACK_ATTR_REMOTE_MASTER_ID,
		RTL931X_STACK_ATTR_REMOTE_GENERATION,
	};
	int err;

	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;

	*ifindex = nla_get_u32(attrs[RTL931X_STACK_ATTR_IFINDEX]);
	*boot_nonce = nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE]);
	*member = nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID]);
	*master = nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_MASTER_ID]);
	*generation =
		nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_GENERATION]);

	return 0;
}

static int parse_peer_switch(struct nlattr **attrs,
			     struct rtl931x_stack_peer_switch *peer)
{
	struct nlattr *delegated_attr;
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
	int err;

	err = parse_peer_session(attrs, &peer->ifindex, &peer->boot_nonce,
				 &peer->member, &peer->master,
				 &peer->generation);
	if (err)
		return err;
	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;
	delegated_attr =
		attrs[RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK];

	peer->port_mask =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MASK]);
	peer->admin_up_mask =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK]);
	peer->carrier_mask =
		nla_get_u64(attrs[RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK]);
	peer->delegated_port_mask = nla_get_u64(delegated_attr);
	peer->port_count =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_COUNT]);
	peer->cpu_port =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_CPU_PORT]);
	peer->stack_port =
		nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_STACK_PORT]);
	peer->capabilities =
		nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_CAPABILITIES]);
	peer->max_body_len =
		nla_get_u16(attrs[RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN]);

	return 0;
}

static int parse_peer_port(struct nlattr **attrs,
			   struct rtl931x_stack_peer_port *port)
{
	static const int required[] = {
		RTL931X_STACK_ATTR_REMOTE_PORT,
		RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS,
		RTL931X_STACK_ATTR_REMOTE_PORT_MTU,
		RTL931X_STACK_ATTR_REMOTE_PORT_MAC,
	};
	int err;

	err = parse_peer_session(attrs, &port->ifindex, &port->boot_nonce,
				 &port->member, &port->master,
				 &port->generation);
	if (err)
		return err;
	err = require_attrs(attrs, required, ARRAY_SIZE(required));
	if (err)
		return err;

	port->port = nla_get_u8(attrs[RTL931X_STACK_ATTR_REMOTE_PORT]);
	port->flags = nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS]);
	port->mtu = nla_get_u32(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MTU]);
	memcpy(port->mac, nla_data(attrs[RTL931X_STACK_ATTR_REMOTE_PORT_MAC]),
	       ETH_ALEN);

	return 0;
}

static void capture_extack(struct rtl931x_stack_nl *nl,
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

			if (len >= sizeof(nl->extack))
				len = sizeof(nl->extack) - 1;
			memcpy(nl->extack, nla_data(attr), len);
			nl->extack[len] = '\0';
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
	context->nl->kernel_error = error->error;
	capture_extack(context->nl, error);
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

	if (header->nlmsg_type != context->nl->family ||
	    !genlmsg_valid_hdr(header, 0) || genl->cmd != context->cmd) {
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
	case RTL931X_STACK_CMD_RECOVER_LOCAL:
		err = parse_status(attrs, &context->reply->status);
		break;
	case RTL931X_STACK_CMD_PROBE_PEER:
		err = parse_probe(attrs, &context->reply->probe);
		break;
	case RTL931X_STACK_CMD_GET_PEER_SWITCH:
		err = parse_peer_switch(attrs, &context->reply->peer_switch);
		break;
	case RTL931X_STACK_CMD_GET_PEER_PORT:
		err = parse_peer_port(attrs, &context->reply->peer_port);
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
			     const struct rtl931x_stack_request *request)
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
	case RTL931X_STACK_CMD_RECOVER_LOCAL:
		if (nla_put_u32(msg, RTL931X_STACK_ATTR_GENERATION,
				request->generation))
			return -NLE_NOMEM;
		break;
	default:
		break;
	}

	return 0;
}

int rtl931x_stack_nl_request(struct rtl931x_stack_nl *nl,
			     const struct rtl931x_stack_request *request,
			     union rtl931x_stack_reply *reply)
{
	struct request_context context = {
		.nl = nl,
		.reply = reply,
		.cmd = request->cmd,
		.expect_reply = command_expects_reply(request->cmd),
	};
	struct nl_cb *callbacks;
	struct nl_msg *msg;
	int err;

	if (!nl || !nl->sock || !request || !reply)
		return -EINVAL;

	nl->kernel_error = 0;
	nl->extack[0] = '\0';
	memset(reply, 0, sizeof(*reply));

	msg = nlmsg_alloc();
	if (!msg)
		return -ENOMEM;
	if (!genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, nl->family, 0, 0,
			 request->cmd, RTL931X_STACK_GENL_VERSION)) {
		err = -ENOMEM;
		goto out_msg;
	}
	err = add_request_attrs(msg, request);
	if (err) {
		err = -ENOMEM;
		goto out_msg;
	}

	callbacks = nl_cb_alloc(NL_CB_CUSTOM);
	if (!callbacks) {
		err = -ENOMEM;
		goto out_msg;
	}
	nl_cb_err(callbacks, NL_CB_CUSTOM, error_handler, &context);
	nl_cb_set(callbacks, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &context);
	nl_cb_set(callbacks, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, &context);

	err = nl_send_auto_complete(nl->sock, msg);
	if (err < 0) {
		err = -EIO;
		goto out_callbacks;
	}
	while (!context.done) {
		err = nl_recvmsgs(nl->sock, callbacks);
		if (err < 0) {
			err = -EIO;
			goto out_callbacks;
		}
	}

	if (nl->kernel_error)
		err = nl->kernel_error;
	else if (context.parse_error)
		err = context.parse_error;
	else if (context.expect_reply && !context.got_reply)
		err = -EPROTO;
	else
		err = 0;

out_callbacks:
	nl_cb_put(callbacks);
out_msg:
	nlmsg_free(msg);
	return err;
}

int rtl931x_stack_nl_open(struct rtl931x_stack_nl *nl)
{
	int enable = 1;
	int err;

	if (!nl)
		return -EINVAL;
	memset(nl, 0, sizeof(*nl));
	nl->sock = nl_socket_alloc();
	if (!nl->sock)
		return -ENOMEM;
	err = genl_connect(nl->sock);
	if (err < 0)
		goto out_error;
	(void)setsockopt(nl_socket_get_fd(nl->sock), SOL_NETLINK,
			 NETLINK_EXT_ACK, &enable, sizeof(enable));
	nl->family = genl_ctrl_resolve(nl->sock, RTL931X_STACK_GENL_NAME);
	if (nl->family < 0) {
		err = -ENOENT;
		goto out_error;
	}

	return 0;

out_error:
	nl_socket_free(nl->sock);
	nl->sock = NULL;
	return err;
}

void rtl931x_stack_nl_close(struct rtl931x_stack_nl *nl)
{
	if (nl && nl->sock)
		nl_socket_free(nl->sock);
	if (nl)
		nl->sock = NULL;
}
