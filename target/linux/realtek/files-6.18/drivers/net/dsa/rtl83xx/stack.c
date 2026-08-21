// SPDX-License-Identifier: GPL-2.0-only

#include <linux/dsa/tag_rtl_otto.h>
#include <linux/etherdevice.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/pcs/pcs-rtl-otto.h>
#include <linux/pcs/pcs.h>
#include <linux/random.h>
#include <linux/rtl838x_eth.h>
#include <linux/rtnetlink.h>
#include <linux/timekeeping.h>
#include <net/dst_metadata.h>
#include <net/genetlink.h>

#include "rtl-otto.h"

static LIST_HEAD(rtl931x_stack_list);
static DEFINE_MUTEX(rtl931x_stack_lock);

static struct genl_family rtl931x_stack_family;

#define RTL931X_TALK_MAGIC		0x4f53544b /* "OSTK" */
#define RTL931X_TALK_VERSION		1
#define RTL931X_TALK_PROBE_MESSAGE_LEN	37
#define RTL931X_TALK_RX_QUEUE_LEN	64
#define RTL931X_TALK_TIMEOUT_MS		500
#define RTL931X_TALK_TARGET_ONE_HOP	0xff
#define RTL931X_TALK_F_ROUTED		BIT(0)
#define RTL931X_TALK_F_MASK		RTL931X_TALK_F_ROUTED

enum rtl931x_talk_type {
	RTL931X_TALK_TYPE_PING = 1,
	RTL931X_TALK_TYPE_PONG,
};

struct rtl931x_talk_header {
	__be32 magic;
	u8 version;
	u8 type;
	__be16 flags;
	__be64 transaction;
	__be64 boot_nonce;
	u8 sender_device;
	u8 target_device;
	u8 sender_stack_port;
	u8 message_len;
} __packed;

struct rtl931x_talk_probe {
	__be32 status;
	__be32 generation;
	u8 master_device;
} __packed;

struct rtl931x_talk_probe_message {
	struct rtl931x_talk_header header;
	struct rtl931x_talk_probe probe;
	u8 padding[9];
} __packed;

static_assert(sizeof(struct rtl931x_talk_header) == 28);
static_assert(sizeof(struct rtl931x_talk_probe) == 9);
static_assert(RTL931X_TALK_PROBE_MESSAGE_LEN ==
	      sizeof(struct rtl931x_talk_header) +
	      sizeof(struct rtl931x_talk_probe));
static_assert(sizeof(struct rtl931x_talk_probe_message) ==
	      ETH_ZLEN - ETH_HLEN);

static const u8 rtl931x_talk_dest[ETH_ALEN] = {
	0x02, 0x00, 0x00, 0x93, 0x10, 0x01,
};

struct rtl931x_stack_target {
	struct rtl838x_switch_priv *priv;
	struct net_device *dev;
	int port;
};

struct rtl931x_stack_conduit {
	struct net_device *dev;
	bool reattach;
};

struct rtl931x_talk_rx_metadata {
	u32 cpu_device_generation;
	u8 ingress_port;
	u8 source_device;
};

struct rtl931x_talk_skb_cb {
	u32 cpu_device_generation;
};

#define RTL931X_TALK_SKB_CB(skb) \
	((struct rtl931x_talk_skb_cb *)((skb)->cb))

static_assert(sizeof(struct rtl931x_talk_skb_cb) <=
	      sizeof_field(struct sk_buff, cb));

static int
rtl931x_stack_quiesce_conduit(struct rtl838x_switch_priv *priv, int port,
			      struct rtl931x_stack_conduit *ctx)
{
	ctx->dev = dsa_port_to_conduit(priv->ports[port].dp);
	ctx->reattach = netif_device_present(ctx->dev);

	/* Prevent a software tag from crossing a hardware identity change. */
	if (ctx->reattach)
		netif_device_detach(ctx->dev);
	if (!netif_running(ctx->dev))
		return 0;

	netif_tx_disable(ctx->dev);
	return rtl838x_eth_tx_drain(ctx->dev);
}

static void
rtl931x_stack_resume_conduit(struct rtl931x_stack_conduit *ctx)
{
	if (ctx->reattach)
		netif_device_attach(ctx->dev);
}

static void
rtl931x_stack_begin_cpu_device_change(struct rtl838x_switch_priv *priv)
{
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;

	if (!tagger_data)
		return;

	write_lock_bh(&tagger_data->cpu_device_lock);
	tagger_data->cpu_device_generation++;
	tagger_data->cpu_device_changing = true;
	write_unlock_bh(&tagger_data->cpu_device_lock);
}

static void
rtl931x_stack_end_cpu_device_change(struct rtl838x_switch_priv *priv)
{
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	u8 cpu_device;

	if (!tagger_data)
		return;

	cpu_device = FIELD_GET(RTL931X_STK_GBL_CTRL_MY_DEV_ID,
			       sw_r32(RTL931X_STK_GBL_CTRL));
	write_lock_bh(&tagger_data->cpu_device_lock);
	tagger_data->cpu_device = cpu_device;
	tagger_data->cpu_device_changing = false;
	write_unlock_bh(&tagger_data->cpu_device_lock);
}

static int rtl931x_stack_cpu_device_get(struct rtl838x_switch_priv *priv,
					u32 *generation)
{
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	int device;

	if (!tagger_data)
		return -ENODEV;

	read_lock_bh(&tagger_data->cpu_device_lock);
	if (tagger_data->cpu_device_changing) {
		device = -EBUSY;
	} else {
		device = tagger_data->cpu_device;
		if (generation)
			*generation = tagger_data->cpu_device_generation;
	}
	read_unlock_bh(&tagger_data->cpu_device_lock);

	return device;
}

static int
rtl931x_stack_talk_metadata(struct sk_buff *skb,
			    struct rtl931x_talk_rx_metadata *metadata)
{
	struct metadata_dst *md_dst = skb_metadata_dst(skb);
	u32 port_id;

	if (!md_dst || md_dst->type != METADATA_HW_PORT_MUX)
		return -EINVAL;

	port_id = md_dst->u.port_info.port_id;
	if (!(port_id & RTL838X_ETH_DEVICE_TALK_METADATA) ||
	    port_id & ~(RTL838X_ETH_DEVICE_TALK_METADATA |
			RTL838X_ETH_DEVICE_TALK_SOURCE_DEVICE |
			RTL838X_ETH_DEVICE_TALK_INGRESS_PORT))
		return -EINVAL;

	metadata->ingress_port =
		FIELD_GET(RTL838X_ETH_DEVICE_TALK_INGRESS_PORT, port_id);
	metadata->source_device =
		FIELD_GET(RTL838X_ETH_DEVICE_TALK_SOURCE_DEVICE, port_id);

	return metadata->ingress_port < RTL931X_STACK_MAX_PORTS ? 0 : -EINVAL;
}

static bool
rtl931x_stack_talk_validate(struct rtl931x_stack_context *stack,
			    struct sk_buff *skb,
			    struct rtl931x_talk_probe_message *message,
			    struct rtl931x_talk_rx_metadata *metadata)
{
	struct rtl_otto_tagger_data *tagger_data =
		stack->priv->ds->tagger_data;
	struct rtl931x_talk_header *header = &message->header;
	struct rtl931x_talk_probe *probe = &message->probe;
	const struct ethhdr *eth = eth_hdr(skb);
	bool active;
	bool claims_master, is_master;
	u32 status;
	u16 flags;

	if (READ_ONCE(stack->state) == RTL931X_STACK_STATE_ERROR ||
	    skb->dev != stack->talk_conduit ||
	    skb->len != sizeof(*message) ||
	    eth->h_proto != htons(ETH_P_802_EX1) ||
	    !ether_addr_equal(eth->h_dest, rtl931x_talk_dest) ||
	    !is_valid_ether_addr(eth->h_source) ||
	    skb_copy_bits(skb, 0, message, sizeof(*message)))
		return false;

	if (rtl931x_stack_talk_metadata(skb, metadata) || !tagger_data)
		return false;

	read_lock_bh(&tagger_data->cpu_device_lock);
	active = !tagger_data->cpu_device_changing &&
		 rtl931x_stack_port_active(stack->priv, metadata->ingress_port);
	if (active)
		metadata->cpu_device_generation =
			tagger_data->cpu_device_generation;
	read_unlock_bh(&tagger_data->cpu_device_lock);
	if (!active)
		return false;

	flags = be16_to_cpu(header->flags);
	status = be32_to_cpu(probe->status);
	claims_master = status & RTL931X_STACK_TALK_S_MASTER;
	is_master = probe->master_device == header->sender_device;
	if (be32_to_cpu(header->magic) != RTL931X_TALK_MAGIC ||
	    header->version != RTL931X_TALK_VERSION ||
	    (header->type != RTL931X_TALK_TYPE_PING &&
	     header->type != RTL931X_TALK_TYPE_PONG) ||
	    header->message_len != RTL931X_TALK_PROBE_MESSAGE_LEN ||
	    !be64_to_cpu(header->transaction) ||
	    !be64_to_cpu(header->boot_nonce) ||
	    (flags & ~RTL931X_TALK_F_MASK) ||
	    be64_to_cpu(header->boot_nonce) == stack->talk_boot_nonce ||
	    header->sender_device >= RTL931X_STACK_MAX_DEVICES ||
	    (probe->master_device != RTL931X_TALK_TARGET_ONE_HOP &&
	     probe->master_device >= RTL931X_STACK_MAX_DEVICES) ||
	    (header->target_device != RTL931X_TALK_TARGET_ONE_HOP &&
	     header->target_device >= RTL931X_STACK_MAX_DEVICES) ||
	    header->sender_stack_port != metadata->ingress_port ||
	    (status & ~RTL931X_STACK_TALK_S_MASK) ||
	    ((status & RTL931X_STACK_TALK_S_ID_VALID) &&
	     probe->master_device == RTL931X_TALK_TARGET_ONE_HOP) ||
	    (!(status & RTL931X_STACK_TALK_S_ID_VALID) &&
	     probe->master_device != RTL931X_TALK_TARGET_ONE_HOP) ||
	    claims_master != is_master ||
	    ((status & (RTL931X_STACK_TALK_S_CONFIGURED |
			RTL931X_STACK_TALK_S_ROUTE_READY)) &&
	     !(status & RTL931X_STACK_TALK_S_ID_VALID)) ||
	    ((flags & RTL931X_TALK_F_ROUTED) &&
	     (!(status & RTL931X_STACK_TALK_S_ID_VALID) ||
	      header->target_device == RTL931X_TALK_TARGET_ONE_HOP ||
	      header->sender_device != metadata->source_device)) ||
	    (!(flags & RTL931X_TALK_F_ROUTED) &&
	     header->target_device != RTL931X_TALK_TARGET_ONE_HOP) ||
	    memchr_inv(message->padding, 0, sizeof(message->padding)))
		return false;

	return true;
}

static u32 rtl931x_stack_carrier_changes(const struct net_device *dev)
{
	return atomic_read(&dev->carrier_up_count) +
	       atomic_read(&dev->carrier_down_count);
}

static u32 rtl931x_stack_talk_status(struct rtl931x_stack_context *stack,
				     u8 device, u8 port)
{
	bool configured = READ_ONCE(stack->enabled) &&
			  READ_ONCE(stack->state) ==
			  RTL931X_STACK_STATE_CONFIGURED;
	const struct dsa_port *dp;
	u32 status = 0;

	if (configured)
		status |= RTL931X_STACK_TALK_S_ID_VALID |
			  RTL931X_STACK_TALK_S_CONFIGURED |
			  RTL931X_STACK_TALK_S_ROUTE_READY;
	if (configured && device == READ_ONCE(stack->master_id))
		status |= RTL931X_STACK_TALK_S_MASTER;

	dp = port < stack->priv->ds->num_ports ?
		stack->priv->ports[port].dp : NULL;
	if (dp && dp->user && netif_carrier_ok(dp->user))
		status |= RTL931X_STACK_TALK_S_LINK_UP;

	return status;
}

static int
rtl931x_stack_talk_send(struct rtl931x_stack_context *stack, u8 type,
			unsigned int mode, u8 target, u8 stack_port,
			u64 transaction, u32 expected_device_generation)
{
	struct rtl_otto_tagger_data *tagger_data =
		stack->priv->ds->tagger_data;
	enum rtl838x_eth_device_talk_mode tx_mode;
	struct rtl931x_talk_probe_message *message;
	struct rtl931x_talk_header *header;
	struct rtl931x_talk_probe *probe;
	struct net_device *conduit = stack->talk_conduit;
	struct sk_buff *skb;
	struct ethhdr *eth;
	u8 device, master;
	u32 generation, status;
	int err;

	if (!conduit || !tagger_data)
		return -EOPNOTSUPP;

	read_lock_bh(&tagger_data->cpu_device_lock);
	if (tagger_data->cpu_device_changing ||
	    tagger_data->cpu_device_generation !=
		expected_device_generation) {
		read_unlock_bh(&tagger_data->cpu_device_lock);
		return -ESTALE;
	}
	device = tagger_data->cpu_device;
	read_unlock_bh(&tagger_data->cpu_device_lock);
	generation = READ_ONCE(stack->generation_valid) ?
		     READ_ONCE(stack->generation) : 0;
	status = rtl931x_stack_talk_status(stack, device, stack_port);
	master = status & RTL931X_STACK_TALK_S_ID_VALID ?
		 READ_ONCE(stack->master_id) : RTL931X_TALK_TARGET_ONE_HOP;

	skb = netdev_alloc_skb(conduit, ETH_ZLEN);
	if (!skb)
		return -ENOMEM;

	skb_reset_mac_header(skb);
	eth = skb_put_zero(skb, ETH_ZLEN);
	ether_addr_copy(eth->h_dest, rtl931x_talk_dest);
	ether_addr_copy(eth->h_source, conduit->dev_addr);
	eth->h_proto = htons(ETH_P_802_EX1);

	message = (struct rtl931x_talk_probe_message *)(eth + 1);
	header = &message->header;
	probe = &message->probe;
	header->magic = cpu_to_be32(RTL931X_TALK_MAGIC);
	header->version = RTL931X_TALK_VERSION;
	header->type = type;
	header->flags = cpu_to_be16(mode == RTL931X_STACK_TALK_MODE_UNICAST ?
				    RTL931X_TALK_F_ROUTED : 0);
	header->transaction = cpu_to_be64(transaction);
	header->boot_nonce = cpu_to_be64(stack->talk_boot_nonce);
	header->sender_device = device;
	header->target_device =
		mode == RTL931X_STACK_TALK_MODE_UNICAST ? target :
		RTL931X_TALK_TARGET_ONE_HOP;
	header->sender_stack_port = stack_port;
	header->message_len = RTL931X_TALK_PROBE_MESSAGE_LEN;
	probe->status = cpu_to_be32(status);
	probe->generation = cpu_to_be32(generation);
	probe->master_device = master;
	skb->protocol = eth->h_proto;
	tx_mode = mode == RTL931X_STACK_TALK_MODE_UNICAST ?
		  RTL838X_ETH_DEVICE_TALK_UNICAST :
		  RTL838X_ETH_DEVICE_TALK_ONE_HOP;

	/*
	 * Serialize the final submission with identity handover. A sender may
	 * have been descheduled while building the frame; once submitted, the
	 * handover path's conduit drain accounts for this packet.
	 */
	read_lock_bh(&tagger_data->cpu_device_lock);
	if (tagger_data->cpu_device_changing ||
	    tagger_data->cpu_device != device ||
	    tagger_data->cpu_device_generation !=
		expected_device_generation) {
		read_unlock_bh(&tagger_data->cpu_device_lock);
		dev_kfree_skb_any(skb);
		return -EBUSY;
	}
	err = rtl838x_eth_device_talk_xmit(conduit, skb, tx_mode, target);
	read_unlock_bh(&tagger_data->cpu_device_lock);

	return err;
}

static int rtl931x_stack_talk_rcv(struct sk_buff *skb, struct net_device *dev,
				  struct packet_type *pt,
				  struct net_device *orig_dev)
{
	struct rtl931x_stack_context *stack =
		container_of(pt, struct rtl931x_stack_context, talk_packet_type);
	struct rtl931x_talk_probe_message message;
	struct rtl931x_talk_rx_metadata metadata;
	bool queued = false;

	if (!rtl931x_stack_talk_validate(stack, skb, &message, &metadata))
		goto drop;
	RTL931X_TALK_SKB_CB(skb)->cpu_device_generation =
		metadata.cpu_device_generation;

	spin_lock(&stack->talk_rx_queue.lock);
	if (stack->talk_rx_queue.qlen < RTL931X_TALK_RX_QUEUE_LEN) {
		__skb_queue_tail(&stack->talk_rx_queue, skb);
		queued = true;
	}
	spin_unlock(&stack->talk_rx_queue.lock);

	if (queued) {
		schedule_work(&stack->talk_rx_work);
		return 0;
	}

drop:
	kfree_skb(skb);
	return 0;
}

static void
rtl931x_stack_talk_complete(struct rtl931x_stack_context *stack,
			    const struct rtl931x_talk_probe_message *message,
			    const struct rtl931x_talk_rx_metadata *metadata)
{
	const struct rtl931x_talk_header *header = &message->header;
	const struct rtl931x_talk_probe *probe = &message->probe;
	u64 now = ktime_get_ns();
	u16 flags = be16_to_cpu(header->flags);

	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending &&
	    be64_to_cpu(header->transaction) ==
		stack->talk_pending_transaction &&
	    metadata->ingress_port == stack->talk_pending_port &&
	    ((stack->talk_pending_mode == RTL931X_STACK_TALK_MODE_ONE_HOP &&
	      !(flags & RTL931X_TALK_F_ROUTED) &&
	      header->target_device == RTL931X_TALK_TARGET_ONE_HOP) ||
	     (stack->talk_pending_mode == RTL931X_STACK_TALK_MODE_UNICAST &&
	      flags & RTL931X_TALK_F_ROUTED &&
	      header->sender_device == stack->talk_pending_peer &&
	      header->target_device == stack->talk_pending_local))) {
		stack->talk_reply.transaction =
			be64_to_cpu(header->transaction);
		stack->talk_reply.boot_nonce = be64_to_cpu(header->boot_nonce);
		stack->talk_reply.status = be32_to_cpu(probe->status);
		stack->talk_reply.generation =
			be32_to_cpu(probe->generation);
		stack->talk_reply.round_trip_us =
			min_t(u64,
			      div_u64(now - stack->talk_pending_started_ns,
				      NSEC_PER_USEC), U32_MAX);
		stack->talk_reply.member_id = header->sender_device;
		stack->talk_reply.master_id = probe->master_device;
		stack->talk_reply.stack_port = header->sender_stack_port;
		stack->talk_reply.mode = stack->talk_pending_mode;
		stack->talk_pending = false;
		complete(&stack->talk_reply_completion);
	}
	spin_unlock_bh(&stack->talk_reply_lock);
}

static void rtl931x_stack_talk_work(struct work_struct *work)
{
	struct rtl931x_stack_context *stack = container_of(work,
		struct rtl931x_stack_context, talk_rx_work);
	struct rtl931x_talk_rx_metadata metadata;
	struct rtl931x_talk_probe_message message;
	struct rtl931x_talk_header *header;
	struct sk_buff *skb;
	int device;
	u16 flags;

	while ((skb = skb_dequeue(&stack->talk_rx_queue))) {
		if (!rtl931x_stack_talk_validate(stack, skb, &message,
						 &metadata) ||
		    metadata.cpu_device_generation !=
			RTL931X_TALK_SKB_CB(skb)->cpu_device_generation)
			goto next;

		header = &message.header;
		flags = be16_to_cpu(header->flags);
		if (header->type == RTL931X_TALK_TYPE_PONG) {
			rtl931x_stack_talk_complete(stack, &message, &metadata);
			goto next;
		}

		device = rtl931x_stack_cpu_device_get(stack->priv, NULL);
		if (device < 0)
			goto next;

		if (flags & RTL931X_TALK_F_ROUTED) {
			if (header->target_device != device)
				goto next;
			rtl931x_stack_talk_send(stack, RTL931X_TALK_TYPE_PONG,
						RTL931X_STACK_TALK_MODE_UNICAST,
						header->sender_device,
						metadata.ingress_port,
						be64_to_cpu(header->transaction),
						metadata.cpu_device_generation);
		} else {
			if (header->target_device != RTL931X_TALK_TARGET_ONE_HOP)
				goto next;
			rtl931x_stack_talk_send(stack, RTL931X_TALK_TYPE_PONG,
						RTL931X_STACK_TALK_MODE_ONE_HOP,
						metadata.ingress_port,
						metadata.ingress_port,
						be64_to_cpu(header->transaction),
						metadata.cpu_device_generation);
		}

next:
		kfree_skb(skb);
	}
}

static const struct nla_policy
rtl931x_stack_policy[RTL931X_STACK_ATTR_MAX + 1] = {
	[RTL931X_STACK_ATTR_UNSPEC] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_API_VERSION] = { .type = NLA_U16 },
	[RTL931X_STACK_ATTR_IFINDEX] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_ENABLED] = NLA_POLICY_MAX(NLA_U8, 1),
	[RTL931X_STACK_ATTR_MEMBER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_PEER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_MASTER_ID] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_DEVICES - 1),
	[RTL931X_STACK_ATTR_FLAGS] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_GENERATION] = { .type = NLA_U32 },
	[RTL931X_STACK_ATTR_STATE] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_LINK_UP] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_TALK_MODE] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_TALK_TRANSACTION] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_MEMBER_ID] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_MASTER_ID] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_STACK_PORT] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_STATUS] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_GENERATION] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_ROUND_TRIP_US] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_PAD] = { .type = NLA_REJECT },
};

static int rtl931x_stack_check_version(struct genl_info *info)
{
	struct nlattr *version = info->attrs[RTL931X_STACK_ATTR_API_VERSION];

	if (!version) {
		NL_SET_ERR_MSG_MOD(info->extack, "API version is required");
		return -EINVAL;
	}

	if (nla_get_u16(version) != RTL931X_STACK_GENL_VERSION) {
		NL_SET_ERR_MSG_MOD(info->extack, "unsupported API version");
		return -EPROTONOSUPPORT;
	}

	return 0;
}

static int rtl931x_stack_get_target(struct genl_info *info,
				    struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack;
	struct nlattr *ifindex;
	int i;

	ASSERT_RTNL();
	lockdep_assert_held(&rtl931x_stack_lock);

	ifindex = info->attrs[RTL931X_STACK_ATTR_IFINDEX];
	if (!ifindex) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack port ifindex is required");
		return -EINVAL;
	}

	target->dev = dev_get_by_index(genl_info_net(info),
				       nla_get_u32(ifindex));
	if (!target->dev) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack port does not exist");
		return -ENODEV;
	}

	list_for_each_entry(stack, &rtl931x_stack_list, list) {
		target->priv = container_of(stack, struct rtl838x_switch_priv,
					    stack);

		for (i = 0; i < target->priv->ds->num_ports; i++) {
			if (!target->priv->ports[i].dp ||
			    target->priv->ports[i].dp->user != target->dev)
				continue;

			target->port = i;
			return 0;
		}
	}

	dev_put(target->dev);
	target->dev = NULL;
	NL_SET_ERR_MSG_MOD(info->extack, "interface is not an RTL931x user port");

	return -ENODEV;
}

static int rtl931x_stack_put_reply(struct genl_info *info,
				   struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack = &target->priv->stack;
	struct sk_buff *skb;
	void *hdr;
	u32 ifindex;

	ifindex = stack->enabled ? stack->ifindex : target->dev->ifindex;
	skb = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	hdr = genlmsg_put_reply(skb, info, &rtl931x_stack_family, 0,
				info->genlhdr->cmd);
	if (!hdr)
		goto nla_put_failure;

	if (nla_put_u16(skb, RTL931X_STACK_ATTR_API_VERSION,
			RTL931X_STACK_GENL_VERSION) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_IFINDEX, ifindex) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_ENABLED, stack->enabled) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_MEMBER_ID, stack->member_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_PEER_ID, stack->peer_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_MASTER_ID, stack->master_id) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_FLAGS, stack->flags) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_GENERATION,
			stack->generation) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_STATE, stack->state) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_LINK_UP,
		       netif_carrier_ok(target->dev))) {
		genlmsg_cancel(skb, hdr);
		goto nla_put_failure;
	}

	genlmsg_end(skb, hdr);
	return genlmsg_reply(skb, info);

nla_put_failure:
	nlmsg_free(skb);
	return -EMSGSIZE;
}

static int rtl931x_stack_get(struct sk_buff *skb, struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	if ((target.priv->stack.enabled &&
	     target.port != target.priv->stack.port) ||
	    (target.priv->stack.talk_armed &&
	     target.port != target.priv->stack.talk_armed_port)) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -ENODEV;
		goto out_put;
	}

	err = rtl931x_stack_put_reply(info, &target);

out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static int rtl931x_stack_set_device_talk_port(struct sk_buff *skb,
					      struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	struct rtl931x_stack_context *stack;
	struct rtl931x_stack_conduit conduit = {};
	struct phylink_pcs *pcs;
	const struct dsa_port *dp;
	bool enabled, reopen;
	int err, open_err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	if (!info->attrs[RTL931X_STACK_ATTR_ENABLED]) {
		NL_SET_ERR_MSG_MOD(info->extack, "enabled is required");
		return -EINVAL;
	}
	enabled = nla_get_u8(info->attrs[RTL931X_STACK_ATTR_ENABLED]);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	stack = &target.priv->stack;
	dp = target.priv->ports[target.port].dp;
	if (!stack->talk_conduit) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "CPU Device Talk is unavailable");
		err = -EOPNOTSUPP;
		goto out_put;
	}
	if (stack->state == RTL931X_STACK_STATE_ERROR) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "stack state requires disable and recovery");
		err = -EUCLEAN;
		goto out_put;
	}
	if (stack->enabled || stack->saved_valid) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "Device Talk port cannot change while stacking is enabled");
		err = -EBUSY;
		goto out_put;
	}
	if (stack->talk_armed && target.port != stack->talk_armed_port) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the armed Device Talk port");
		err = -EINVAL;
		goto out_put;
	}
	if (enabled && netdev_has_any_upper_dev(target.dev)) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "stack port must not have an upper device");
		err = -EBUSY;
		goto out_put;
	}

	if (enabled && stack->talk_armed) {
		err = rtl931x_stack_put_reply(info, &target);
		goto out_put;
	}
	if (!enabled && !stack->talk_armed) {
		if (stack->state != RTL931X_STACK_STATE_DISABLED) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack state requires disable and recovery");
			err = -EUCLEAN;
			goto out_put;
		}
		err = rtl931x_stack_put_reply(info, &target);
		goto out_put;
	}
	if (enabled) {
		if (dp->pl_config.num_possible_pcs != 1) {
			err = -EOPNOTSUPP;
		} else {
			pcs = fwnode_pcs_get(of_fwnode_handle(dp->dn), 0);
			if (IS_ERR(pcs))
				err = PTR_ERR(pcs);
			else
				err = rtl931x_pcs_validate_stack(pcs, target.port);
		}
		if (err) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack port requires one dedicated RTL931x SerDes");
			goto out_put;
		}
	}

	/* Fence stale responders and drain submissions before slot ownership. */
	rtl931x_stack_begin_cpu_device_change(target.priv);
	err = rtl931x_stack_quiesce_conduit(target.priv, target.port, &conduit);
	if (err) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "failed to quiesce CPU conduit");
		rtl931x_stack_end_cpu_device_change(target.priv);
		rtl931x_stack_resume_conduit(&conduit);
		goto out_put;
	}

	reopen = netif_running(target.dev);
	if (reopen)
		dev_close(target.dev);

	mutex_lock(&target.priv->reg_mutex);
	if (enabled) {
		err = rtl931x_stack_device_talk_arm(target.priv, target.port,
						    info->extack);
	} else {
		rtl931x_stack_device_talk_disarm(target.priv);
		err = 0;
	}
	mutex_unlock(&target.priv->reg_mutex);

	if (reopen) {
		open_err = dev_open(target.dev, err ? NULL : info->extack);
		if (err && open_err)
			netdev_err(target.dev,
				   "failed to restore admin-up state: %pe\n",
				   ERR_PTR(open_err));
		if (!err)
			err = open_err;
	}
	rtl931x_stack_end_cpu_device_change(target.priv);
	rtl931x_stack_resume_conduit(&conduit);
	if (!err)
		err = rtl931x_stack_put_reply(info, &target);

out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static int
rtl931x_stack_put_probe_reply(struct genl_info *info,
			      struct rtl931x_stack_target *target,
			      const struct rtl931x_stack_peer_reply *reply)
{
	struct sk_buff *skb;
	void *hdr;

	skb = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	hdr = genlmsg_put_reply(skb, info, &rtl931x_stack_family, 0,
				info->genlhdr->cmd);
	if (!hdr)
		goto nla_put_failure;

	if (nla_put_u16(skb, RTL931X_STACK_ATTR_API_VERSION,
			RTL931X_STACK_GENL_VERSION) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_IFINDEX,
			target->dev->ifindex) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_TALK_MODE, reply->mode) ||
	    nla_put_u64_64bit(skb, RTL931X_STACK_ATTR_TALK_TRANSACTION,
			      reply->transaction, RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u64_64bit(skb, RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE,
			      reply->boot_nonce, RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_REMOTE_MEMBER_ID,
		       reply->member_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_REMOTE_MASTER_ID,
		       reply->master_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_REMOTE_STACK_PORT,
		       reply->stack_port) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_REMOTE_STATUS,
			reply->status) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_REMOTE_GENERATION,
			reply->generation) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_ROUND_TRIP_US,
			reply->round_trip_us)) {
		genlmsg_cancel(skb, hdr);
		goto nla_put_failure;
	}

	genlmsg_end(skb, hdr);
	return genlmsg_reply(skb, info);

nla_put_failure:
	nlmsg_free(skb);
	return -EMSGSIZE;
}

static int rtl931x_stack_probe_peer(struct sk_buff *skb,
				    struct genl_info *info)
{
	struct rtl931x_stack_peer_reply reply;
	struct rtl931x_stack_target target = {};
	struct rtl931x_stack_context *stack;
	u64 transaction, started_ns;
	u32 carrier_epoch, device_generation;
	unsigned int mode;
	long timeout;
	u8 local, peer;
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	stack = &target.priv->stack;
	if (stack->state == RTL931X_STACK_STATE_ERROR) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "stack state requires disable and recovery");
		err = -EUCLEAN;
		goto out_put;
	}
	if (!stack->talk_conduit) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "CPU Device Talk is unavailable");
		err = -EOPNOTSUPP;
		goto out_put;
	}

	if ((stack->enabled && target.port != stack->port) ||
	    (stack->talk_armed && target.port != stack->talk_armed_port)) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -EINVAL;
		goto out_put;
	}
	if (!stack->enabled && !stack->talk_armed) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "arm the Device Talk port before probing");
		err = -EINVAL;
		goto out_put;
	}

	if (!netif_carrier_ok(target.dev)) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack link is down");
		err = -ENOLINK;
		goto out_put;
	}
	carrier_epoch = rtl931x_stack_carrier_changes(target.dev);

	err = rtl931x_stack_cpu_device_get(target.priv, &device_generation);
	if (err < 0) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "stack identity is changing");
		goto out_put;
	}
	local = err;

	if (stack->enabled &&
	    stack->state == RTL931X_STACK_STATE_CONFIGURED) {
		mode = RTL931X_STACK_TALK_MODE_UNICAST;
		peer = stack->peer_id;
	} else {
		mode = RTL931X_STACK_TALK_MODE_ONE_HOP;
		peer = target.port;
	}

	err = mutex_lock_interruptible(&stack->talk_request_lock);
	if (err)
		goto out_put;

	do {
		transaction = get_random_u64();
	} while (!transaction);
	started_ns = ktime_get_ns();
	reinit_completion(&stack->talk_reply_completion);

	spin_lock_bh(&stack->talk_reply_lock);
	stack->talk_pending = true;
	stack->talk_pending_transaction = transaction;
	stack->talk_pending_started_ns = started_ns;
	stack->talk_pending_mode = mode;
	stack->talk_pending_port = target.port;
	stack->talk_pending_peer = mode == RTL931X_STACK_TALK_MODE_UNICAST ?
		peer : RTL931X_TALK_TARGET_ONE_HOP;
	stack->talk_pending_local = local;
	if (mode == RTL931X_STACK_TALK_MODE_ONE_HOP)
		stack->state = RTL931X_STACK_STATE_ARMED;
	spin_unlock_bh(&stack->talk_reply_lock);

	err = rtl931x_stack_talk_send(stack, RTL931X_TALK_TYPE_PING, mode,
				      peer, target.port, transaction,
				      device_generation);
	if (err) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "failed to transmit CPU Device Talk probe");
		goto clear_pending;
	}

	timeout = msecs_to_jiffies(RTL931X_TALK_TIMEOUT_MS);
	timeout = wait_for_completion_interruptible_timeout(&stack->talk_reply_completion,
							    timeout);
	if (timeout < 0) {
		err = timeout;
		goto clear_pending;
	}

	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending) {
		stack->talk_pending = false;
		err = -ETIMEDOUT;
	} else {
		reply = stack->talk_reply;
		if (mode != RTL931X_STACK_TALK_MODE_ONE_HOP) {
			err = 0;
		} else if (!netif_carrier_ok(target.dev)) {
			stack->state = RTL931X_STACK_STATE_ARMED;
			err = -ENOLINK;
		} else if (rtl931x_stack_carrier_changes(target.dev) !=
			   carrier_epoch) {
			stack->state = RTL931X_STACK_STATE_ARMED;
			err = -ESTALE;
		} else {
			stack->talk_verified_carrier_changes = carrier_epoch;
			stack->state = RTL931X_STACK_STATE_PEER_VERIFIED;
			err = 0;
		}
	}
	spin_unlock_bh(&stack->talk_reply_lock);
	if (err) {
		if (err == -ENOLINK)
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack link went down during peer verification");
		else if (err == -ESTALE)
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack link changed during peer verification");
		else
			NL_SET_ERR_MSG_MOD(info->extack,
					   "CPU Device Talk peer did not reply");
		goto out_request_unlock;
	}

	err = rtl931x_stack_put_probe_reply(info, &target, &reply);
	goto out_request_unlock;

clear_pending:
	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending_transaction == transaction)
		stack->talk_pending = false;
	spin_unlock_bh(&stack->talk_reply_lock);
out_request_unlock:
	mutex_unlock(&stack->talk_request_lock);
out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static bool
rtl931x_stack_request_matches(struct rtl931x_stack_context *stack, int port,
			      u8 member, u8 peer, u8 master, u32 flags,
			      bool enabled)
{
	if (!enabled)
		return !stack->enabled && !stack->saved_valid &&
		       stack->state != RTL931X_STACK_STATE_ERROR;

	return stack->enabled && stack->saved_valid &&
	       stack->state == RTL931X_STACK_STATE_CONFIGURED &&
	       stack->port == port && stack->member_id == member &&
	       stack->peer_id == peer && stack->master_id == master &&
	       stack->flags == flags;
}

static int rtl931x_stack_set_two_member(struct sk_buff *skb,
					struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	struct rtl931x_stack_context *stack;
	struct nlattr **attrs = info->attrs;
	u32 flags, generation;
	u8 member, peer, master;
	struct rtl931x_stack_conduit conduit = {};
	bool enabled, restart_port;
	int err, open_err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	if (!attrs[RTL931X_STACK_ATTR_ENABLED] ||
	    !attrs[RTL931X_STACK_ATTR_GENERATION]) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "enabled and generation are required");
		return -EINVAL;
	}

	enabled = nla_get_u8(attrs[RTL931X_STACK_ATTR_ENABLED]);
	generation = nla_get_u32(attrs[RTL931X_STACK_ATTR_GENERATION]);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	stack = &target.priv->stack;
	member = stack->member_id;
	peer = stack->peer_id;
	master = stack->master_id;
	flags = stack->flags;

	if ((stack->enabled && target.port != stack->port) ||
	    (stack->talk_armed && target.port != stack->talk_armed_port)) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -EINVAL;
		goto out_put;
	}

	if (enabled) {
		if (!attrs[RTL931X_STACK_ATTR_MEMBER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_PEER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_MASTER_ID] ||
		    !attrs[RTL931X_STACK_ATTR_FLAGS]) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "member, peer, master and flags are required");
			err = -EINVAL;
			goto out_put;
		}

		member = nla_get_u8(attrs[RTL931X_STACK_ATTR_MEMBER_ID]);
		peer = nla_get_u8(attrs[RTL931X_STACK_ATTR_PEER_ID]);
		master = nla_get_u8(attrs[RTL931X_STACK_ATTR_MASTER_ID]);
		flags = nla_get_u32(attrs[RTL931X_STACK_ATTR_FLAGS]);

		if (member == peer) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "member and peer IDs must differ");
			err = -EINVAL;
			goto out_put;
		}

		if (master != member && master != peer) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "master ID must identify a stack member");
			err = -EINVAL;
			goto out_put;
		}

		if (flags & ~RTL931X_STACK_F_MASK) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "unsupported stacking flags");
			err = -EOPNOTSUPP;
			goto out_put;
		}

		if (netdev_has_any_upper_dev(target.dev)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack port must not have an upper device");
			err = -EBUSY;
			goto out_put;
		}

		if (!stack->enabled &&
		    (!stack->talk_armed ||
		     stack->state != RTL931X_STACK_STATE_PEER_VERIFIED)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "verify the Device Talk peer before enabling stacking");
			err = -EHOSTUNREACH;
			goto out_put;
		}
		if (!stack->enabled && !netif_carrier_ok(target.dev)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack peer verification is stale because the link is down");
			err = -ENOLINK;
			goto out_put;
		}
		if (!stack->enabled &&
		    rtl931x_stack_carrier_changes(target.dev) !=
		    stack->talk_verified_carrier_changes) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "stack link changed since peer verification");
			stack->state = RTL931X_STACK_STATE_ARMED;
			err = -ESTALE;
			goto out_put;
		}

	}

	if (stack->generation_valid && generation < stack->generation) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "configuration generation is stale");
		err = -ESTALE;
		goto out_put;
	}

	if (stack->generation_valid && generation == stack->generation) {
		if (!rtl931x_stack_request_matches(stack, target.port, member,
						   peer, master, flags,
						   enabled)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "configuration generation is already committed");
			err = -EALREADY;
			goto out_put;
		}

		err = rtl931x_stack_put_reply(info, &target);
		goto out_put;
	}
	if (enabled) {
		mutex_lock(&target.priv->reg_mutex);
		if (target.priv->lagmembers) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "remove all LAGs before enabling stacking");
			err = -EBUSY;
		}
		mutex_unlock(&target.priv->reg_mutex);
		if (err)
			goto out_put;
	}

	restart_port = netif_running(target.dev) &&
		(enabled ? !stack->enabled :
		 (stack->enabled || stack->saved_valid));
	if (restart_port)
		dev_close(target.dev);

	rtl931x_stack_begin_cpu_device_change(target.priv);
	err = rtl931x_stack_quiesce_conduit(target.priv, target.port, &conduit);
	if (err) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "failed to quiesce CPU conduit");
		rtl931x_stack_end_cpu_device_change(target.priv);
		rtl931x_stack_resume_conduit(&conduit);
		goto out_reopen;
	}
	mutex_lock(&target.priv->reg_mutex);
	/* Defend against any future LAG path which is not RTNL-serialized. */
	if (enabled && target.priv->lagmembers) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "remove all LAGs before enabling stacking");
		err = -EBUSY;
		goto out_reg_unlock;
	}

	err = rtl931x_stack_configure(target.priv, target.port, member, peer,
				       master, flags, generation, enabled,
				       info->extack);
	if (!err || stack->saved_valid)
		stack->ifindex = stack->enabled ? target.dev->ifindex : 0;

out_reg_unlock:
	rtl931x_stack_end_cpu_device_change(target.priv);
	mutex_unlock(&target.priv->reg_mutex);
	rtl931x_stack_resume_conduit(&conduit);

out_reopen:
	if (restart_port && stack->state != RTL931X_STACK_STATE_ERROR) {
		open_err = dev_open(target.dev, err ? NULL : info->extack);
		if (err && open_err)
			netdev_err(target.dev,
				   "failed to restore admin-up state: %pe\n",
				   ERR_PTR(open_err));
		if (!err)
			err = open_err;
	} else if (restart_port) {
		netdev_err(target.dev,
			   "leaving stack port down after a hardware recovery failure\n");
	}
	if (!err)
		err = rtl931x_stack_put_reply(info, &target);

out_put:
	dev_put(target.dev);
out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static const struct genl_small_ops rtl931x_stack_ops[] = {
	{
		.cmd = RTL931X_STACK_CMD_GET,
		.doit = rtl931x_stack_get,
	},
	{
		.cmd = RTL931X_STACK_CMD_SET_TWO_MEMBER,
		.doit = rtl931x_stack_set_two_member,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT,
		.doit = rtl931x_stack_set_device_talk_port,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = RTL931X_STACK_CMD_PROBE_PEER,
		.doit = rtl931x_stack_probe_peer,
		.flags = GENL_ADMIN_PERM,
	},
};

static struct genl_family rtl931x_stack_family __ro_after_init = {
	.name = RTL931X_STACK_GENL_NAME,
	.version = RTL931X_STACK_GENL_VERSION,
	.maxattr = RTL931X_STACK_ATTR_MAX,
	.resv_start_op = __RTL931X_STACK_CMD_MAX,
	.policy = rtl931x_stack_policy,
	.module = THIS_MODULE,
	.small_ops = rtl931x_stack_ops,
	.n_small_ops = ARRAY_SIZE(rtl931x_stack_ops),
};

void rtl931x_stack_register(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct dsa_port *dp;

	INIT_LIST_HEAD(&stack->list);
	stack->state = RTL931X_STACK_STATE_DISABLED;
	stack->priv = priv;
	mutex_init(&stack->talk_request_lock);
	spin_lock_init(&stack->talk_reply_lock);
	init_completion(&stack->talk_reply_completion);
	skb_queue_head_init(&stack->talk_rx_queue);
	INIT_WORK(&stack->talk_rx_work, rtl931x_stack_talk_work);
	do {
		stack->talk_boot_nonce = get_random_u64();
	} while (!stack->talk_boot_nonce);

	dsa_switch_for_each_user_port(dp, priv->ds) {
		stack->talk_conduit = dsa_port_to_conduit(dp);
		if (stack->talk_conduit)
			break;
	}

	if (stack->talk_conduit) {
		dev_hold(stack->talk_conduit);
		stack->talk_packet_type.type = htons(ETH_P_802_EX1);
		stack->talk_packet_type.func = rtl931x_stack_talk_rcv;
		stack->talk_packet_type.dev = stack->talk_conduit;
		dev_add_pack(&stack->talk_packet_type);
	} else {
		dev_warn(priv->dev, "CPU Device Talk has no DSA conduit\n");
	}

	mutex_lock(&rtl931x_stack_lock);
	list_add_tail(&stack->list, &rtl931x_stack_list);
	stack->registered = true;
	mutex_unlock(&rtl931x_stack_lock);
}

void rtl931x_stack_unregister(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl931x_stack_conduit conduit = {};
	struct net_device *stack_port = NULL;
	int err = 0;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	if (!stack->registered)
		goto out_unlock;

	if (stack->saved_valid && priv->ports[stack->port].dp)
		stack_port = priv->ports[stack->port].dp->user;
	else if (stack->talk_armed &&
		 priv->ports[stack->talk_armed_port].dp)
		stack_port = priv->ports[stack->talk_armed_port].dp->user;
	if (stack_port && netif_running(stack_port))
		dev_close(stack_port);

	if (stack->talk_conduit) {
		dev_remove_pack(&stack->talk_packet_type);
		cancel_work_sync(&stack->talk_rx_work);
		skb_queue_purge(&stack->talk_rx_queue);
	}

	if (stack->saved_valid) {
		rtl931x_stack_begin_cpu_device_change(priv);
		err = rtl931x_stack_quiesce_conduit(priv, stack->port,
						    &conduit);
		if (err)
			dev_err(priv->dev,
				"failed to quiesce CPU conduit\n");
	}

	mutex_lock(&priv->reg_mutex);
	if (stack->saved_valid) {
		if (!err) {
			err = rtl931x_stack_configure(priv, stack->port,
						      stack->member_id,
						      stack->peer_id,
						      stack->master_id,
						      stack->flags,
						      stack->generation,
						      false, NULL);
			if (err)
				dev_err(priv->dev,
					"failed to restore standalone switch state\n");
		}
	} else if (stack->talk_armed) {
		rtl931x_stack_device_talk_disarm(priv);
	}
	mutex_unlock(&priv->reg_mutex);
	if (conduit.dev)
		rtl931x_stack_end_cpu_device_change(priv);
	rtl931x_stack_resume_conduit(&conduit);

	list_del_init(&stack->list);
	stack->registered = false;
	if (stack->talk_conduit) {
		dev_put(stack->talk_conduit);
		stack->talk_conduit = NULL;
	}

out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
}

int rtl931x_stack_init(void)
{
	return genl_register_family(&rtl931x_stack_family);
}

void rtl931x_stack_exit(void)
{
	WARN_ON_ONCE(!list_empty(&rtl931x_stack_list));
	genl_unregister_family(&rtl931x_stack_family);
}
