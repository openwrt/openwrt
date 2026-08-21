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
#include <linux/unaligned.h>
#include <net/dst_metadata.h>
#include <net/genetlink.h>

#include "rtl-otto.h"

static LIST_HEAD(rtl931x_stack_list);
static DEFINE_MUTEX(rtl931x_stack_lock);

static struct genl_family rtl931x_stack_family;

#define RTL931X_TALK_MAGIC		0x4f53544b /* "OSTK" */
#define RTL931X_TALK_VERSION		5
#define RTL931X_TALK_PROBE_MESSAGE_LEN	37
#define RTL931X_TALK_MAX_MESSAGE_LEN	112
#define RTL931X_TALK_RX_QUEUE_LEN	64
#define RTL931X_TALK_TIMEOUT_MS		500
#define RTL931X_TALK_RPC_TIMEOUT_MS	200
#define RTL931X_TALK_RPC_ATTEMPTS	3
#define RTL931X_TALK_TARGET_ONE_HOP	0xff
#define RTL931X_TALK_F_ROUTED		BIT(0)
#define RTL931X_TALK_F_MASK		RTL931X_TALK_F_ROUTED

enum rtl931x_talk_type {
	RTL931X_TALK_TYPE_PING = 1,
	RTL931X_TALK_TYPE_PONG,
	RTL931X_TALK_TYPE_RPC_REQUEST,
	RTL931X_TALK_TYPE_RPC_REPLY,
};

enum rtl931x_talk_rpc_opcode {
	RTL931X_TALK_RPC_GET_SWITCH_INFO = 1,
	RTL931X_TALK_RPC_GET_PORT_STATE,
	RTL931X_TALK_RPC_SET_DELEGATED,
	RTL931X_TALK_RPC_SET_BRIDGE_PORT,
	RTL931X_TALK_RPC_SET_PORT_VLAN,
};

struct rtl931x_stack_fabric_vlan {
	struct list_head list;
	u16 vid;
	u16 refs;
	u8 saved_fid;
	u8 saved_profile_id;
	bool saved_member;
	bool saved_untagged;
	bool saved_hash_mc_fid;
	bool saved_hash_uc_fid;
	bool initialized_metadata;
};

struct rtl931x_stack_port_vlan {
	struct list_head list;
	u16 vid;
	u16 flags;
	u8 port;
	bool saved_member;
	bool saved_untagged;
};

enum rtl931x_talk_rpc_result {
	RTL931X_TALK_RPC_OK,
	RTL931X_TALK_RPC_INVALID,
	RTL931X_TALK_RPC_UNSUPPORTED,
	RTL931X_TALK_RPC_STALE_SESSION,
	RTL931X_TALK_RPC_STALE_GENERATION,
	RTL931X_TALK_RPC_NO_DEVICE,
	RTL931X_TALK_RPC_IO,
	RTL931X_TALK_RPC_DENIED,
	RTL931X_TALK_RPC_BUSY,
	__RTL931X_TALK_RPC_RESULT_MAX,
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

struct rtl931x_talk_rpc_header {
	__be64 receiver_boot_nonce;
	__be32 generation;
	__be16 opcode;
	__be16 body_len;
	__be16 result;
	u8 master_device;
	u8 reserved;
} __packed;

struct rtl931x_talk_rpc_wire_body {
	struct rtl931x_talk_rpc_header header;
	u8 body[RTL931X_STACK_RPC_MAX_BODY_LEN];
} __packed;

struct rtl931x_talk_rpc_switch_info {
	__be64 user_port_mask;
	__be64 admin_up_mask;
	__be64 carrier_mask;
	__be64 delegated_port_mask;
	__be32 capabilities;
	__be16 max_body_len;
	u8 port_count;
	u8 cpu_port;
	u8 stack_port;
	u8 protocol_version;
	u8 reserved[10];
} __packed;

struct rtl931x_talk_rpc_port_request {
	u8 port;
	u8 reserved[3];
} __packed;

struct rtl931x_talk_rpc_port_state {
	u8 port;
	u8 flags;
	__be16 reserved;
	__be32 mtu;
	u8 mac[ETH_ALEN];
	u8 padding[2];
} __packed;

struct rtl931x_talk_rpc_delegated_request {
	__be32 sequence;
	u8 enabled;
	u8 reserved[3];
} __packed;

struct rtl931x_talk_rpc_bridge_port_request {
	__be32 sequence;
	u8 port;
	u8 present;
	u8 reserved[2];
} __packed;

struct rtl931x_talk_rpc_port_vlan_request {
	__be32 sequence;
	__be16 vid;
	__be16 flags;
	u8 port;
	u8 present;
	u8 reserved[2];
} __packed;

static_assert(sizeof(struct rtl931x_talk_header) == 28);
static_assert(sizeof(struct rtl931x_talk_probe) == 9);
static_assert(RTL931X_TALK_PROBE_MESSAGE_LEN ==
	      sizeof(struct rtl931x_talk_header) +
	      sizeof(struct rtl931x_talk_probe));
static_assert(sizeof(struct rtl931x_talk_probe_message) ==
	      ETH_ZLEN - ETH_HLEN);
static_assert(sizeof(struct rtl931x_talk_rpc_header) == 20);
static_assert(sizeof(struct rtl931x_talk_rpc_wire_body) ==
	      RTL931X_TALK_MAX_MESSAGE_LEN -
	      sizeof(struct rtl931x_talk_header));
static_assert(sizeof(struct rtl931x_talk_rpc_switch_info) == 52);
static_assert(sizeof(struct rtl931x_talk_rpc_port_request) == 4);
static_assert(sizeof(struct rtl931x_talk_rpc_port_state) == 16);
static_assert(sizeof(struct rtl931x_talk_rpc_delegated_request) == 8);
static_assert(sizeof(struct rtl931x_talk_rpc_bridge_port_request) == 8);
static_assert(sizeof(struct rtl931x_talk_rpc_port_vlan_request) == 12);

static const u8 rtl931x_talk_dest[ETH_ALEN] = {
	0x02, 0x00, 0x00, 0x93, 0x10, 0x01,
};

struct rtl931x_stack_target {
	struct rtl838x_switch_priv *priv;
	struct net_device *dev;
	int port;
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

int
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

void
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

static bool rtl931x_stack_talk_peer_get(struct rtl931x_stack_context *stack,
					u64 *boot_nonce)
{
	bool valid;

	spin_lock_bh(&stack->talk_reply_lock);
	valid = stack->talk_peer_valid;
	if (valid && boot_nonce)
		*boot_nonce = stack->talk_peer_boot_nonce;
	spin_unlock_bh(&stack->talk_reply_lock);

	return valid;
}

static void rtl931x_stack_talk_peer_clear(struct rtl931x_stack_context *stack)
{
	spin_lock_bh(&stack->talk_reply_lock);
	stack->talk_peer_valid = false;
	stack->talk_peer_boot_nonce = 0;
	spin_unlock_bh(&stack->talk_reply_lock);
	stack->peer_mutation_sequence = 0;
	stack->peer_mutation_uncertain = false;
	stack->peer_mutation_opcode = 0;
	stack->peer_mutation_len = 0;
	memset(stack->peer_mutation_body, 0,
	       sizeof(stack->peer_mutation_body));
	stack->last_mutation_sequence = 0;
	stack->last_mutation_opcode = 0;
	stack->last_mutation_len = 0;
	stack->last_mutation_result = 0;
	memset(stack->last_mutation_body, 0,
	       sizeof(stack->last_mutation_body));
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
rtl931x_stack_talk_header_validate(struct rtl931x_stack_context *stack,
				   struct sk_buff *skb,
				   struct rtl931x_talk_header *header,
				   struct rtl931x_talk_rx_metadata *metadata)
{
	struct rtl_otto_tagger_data *tagger_data =
		stack->priv->ds->tagger_data;
	const struct ethhdr *eth = eth_hdr(skb);
	u8 padding[RTL931X_TALK_MAX_MESSAGE_LEN -
		   sizeof(struct rtl931x_talk_header)];
	size_t padding_len;
	bool active;
	u16 flags;

	if (READ_ONCE(stack->state) == RTL931X_STACK_STATE_ERROR ||
	    skb->dev != stack->talk_conduit ||
	    skb->len < sizeof(*header) ||
	    skb->len > RTL931X_TALK_MAX_MESSAGE_LEN ||
	    eth->h_proto != htons(ETH_P_802_EX1) ||
	    !ether_addr_equal(eth->h_dest, rtl931x_talk_dest) ||
	    !is_valid_ether_addr(eth->h_source) ||
	    skb_copy_bits(skb, 0, header, sizeof(*header)))
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
	if (be32_to_cpu(header->magic) != RTL931X_TALK_MAGIC ||
	    header->version != RTL931X_TALK_VERSION ||
	    header->message_len < sizeof(*header) ||
	    header->message_len > skb->len ||
	    skb->len != max_t(size_t, ETH_ZLEN - ETH_HLEN,
			     header->message_len) ||
	    !be64_to_cpu(header->transaction) ||
	    !be64_to_cpu(header->boot_nonce) ||
	    (flags & ~RTL931X_TALK_F_MASK) ||
	    be64_to_cpu(header->boot_nonce) == stack->talk_boot_nonce ||
	    header->sender_device >= RTL931X_STACK_MAX_DEVICES ||
	    (header->target_device != RTL931X_TALK_TARGET_ONE_HOP &&
	     header->target_device >= RTL931X_STACK_MAX_DEVICES) ||
	    header->sender_stack_port != metadata->ingress_port ||
	    ((flags & RTL931X_TALK_F_ROUTED) &&
	     (header->target_device == RTL931X_TALK_TARGET_ONE_HOP ||
	      header->sender_device != metadata->source_device)) ||
	    (!(flags & RTL931X_TALK_F_ROUTED) &&
	     header->target_device != RTL931X_TALK_TARGET_ONE_HOP))
		return false;

	padding_len = skb->len - header->message_len;
	if (padding_len &&
	    (skb_copy_bits(skb, header->message_len, padding, padding_len) ||
	     memchr_inv(padding, 0, padding_len)))
		return false;

	return true;
}

static bool
rtl931x_stack_talk_probe_validate(struct sk_buff *skb,
				  const struct rtl931x_talk_header *header,
				  struct rtl931x_talk_probe *probe)
{
	bool claims_master, is_master;
	u32 status;
	u16 flags;

	if (skb->len != sizeof(struct rtl931x_talk_probe_message) ||
	    (header->type != RTL931X_TALK_TYPE_PING &&
	     header->type != RTL931X_TALK_TYPE_PONG) ||
	    header->message_len != RTL931X_TALK_PROBE_MESSAGE_LEN ||
	    skb_copy_bits(skb, sizeof(*header), probe, sizeof(*probe)))
		return false;

	flags = be16_to_cpu(header->flags);
	status = be32_to_cpu(probe->status);
	claims_master = status & RTL931X_STACK_TALK_S_MASTER;
	is_master = probe->master_device == header->sender_device;

	return (probe->master_device == RTL931X_TALK_TARGET_ONE_HOP ||
		probe->master_device < RTL931X_STACK_MAX_DEVICES) &&
	       !(status & ~RTL931X_STACK_TALK_S_MASK) &&
	       !((status & RTL931X_STACK_TALK_S_ID_VALID) &&
		 probe->master_device == RTL931X_TALK_TARGET_ONE_HOP) &&
	       !(!(status & RTL931X_STACK_TALK_S_ID_VALID) &&
		 probe->master_device != RTL931X_TALK_TARGET_ONE_HOP) &&
	       claims_master == is_master &&
	       !((status & (RTL931X_STACK_TALK_S_CONFIGURED |
			   RTL931X_STACK_TALK_S_ROUTE_READY)) &&
		 !(status & RTL931X_STACK_TALK_S_ID_VALID)) &&
	       !((flags & RTL931X_TALK_F_ROUTED) &&
		 !(status & RTL931X_STACK_TALK_S_ID_VALID));
}

static bool
rtl931x_stack_talk_rpc_session_validate(struct rtl931x_stack_context *stack,
					const struct rtl931x_talk_header *header,
					const struct rtl931x_talk_rpc_header *rpc,
					const struct rtl931x_talk_rx_metadata *metadata)
{
	u64 peer_boot_nonce;
	u32 device_generation;
	int device;

	device = rtl931x_stack_cpu_device_get(stack->priv, &device_generation);
	return device >= 0 &&
	       device == READ_ONCE(stack->member_id) &&
	       device_generation == metadata->cpu_device_generation &&
	       rtl931x_stack_talk_peer_get(stack, &peer_boot_nonce) &&
	       READ_ONCE(stack->enabled) &&
	       READ_ONCE(stack->state) == RTL931X_STACK_STATE_CONFIGURED &&
	       READ_ONCE(stack->generation_valid) &&
	       metadata->ingress_port == READ_ONCE(stack->port) &&
	       header->sender_device == READ_ONCE(stack->peer_id) &&
	       header->target_device == READ_ONCE(stack->member_id) &&
	       be64_to_cpu(header->boot_nonce) == peer_boot_nonce &&
	       be64_to_cpu(rpc->receiver_boot_nonce) == stack->talk_boot_nonce &&
	       be32_to_cpu(rpc->generation) == READ_ONCE(stack->generation) &&
	       rpc->master_device == READ_ONCE(stack->master_id);
}

static bool
rtl931x_stack_talk_rpc_validate(struct rtl931x_stack_context *stack,
				struct sk_buff *skb,
				const struct rtl931x_talk_header *header,
				const struct rtl931x_talk_rx_metadata *metadata,
				struct rtl931x_talk_rpc_header *rpc,
				void *body)
{
	u16 body_len, result;

	if (header->type != RTL931X_TALK_TYPE_RPC_REQUEST &&
	    header->type != RTL931X_TALK_TYPE_RPC_REPLY)
		return false;
	if (skb_copy_bits(skb, sizeof(*header), rpc, sizeof(*rpc)))
		return false;

	body_len = be16_to_cpu(rpc->body_len);
	result = be16_to_cpu(rpc->result);
	if (body_len > RTL931X_STACK_RPC_MAX_BODY_LEN ||
	    header->message_len != sizeof(*header) + sizeof(*rpc) + body_len ||
	    be16_to_cpu(header->flags) != RTL931X_TALK_F_ROUTED ||
	    !be16_to_cpu(rpc->opcode) || rpc->reserved ||
	    result >= __RTL931X_TALK_RPC_RESULT_MAX ||
	    (header->type == RTL931X_TALK_TYPE_RPC_REQUEST &&
	     result != RTL931X_TALK_RPC_OK) ||
	    (header->type == RTL931X_TALK_TYPE_RPC_REPLY &&
	     result != RTL931X_TALK_RPC_OK && body_len) ||
	    !rtl931x_stack_talk_rpc_session_validate(stack, header, rpc,
						       metadata))
		return false;

	if (body_len && body &&
	    skb_copy_bits(skb, sizeof(*header) + sizeof(*rpc), body, body_len))
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
rtl931x_stack_talk_xmit(struct rtl931x_stack_context *stack, u8 type,
			unsigned int mode, u8 target, u8 stack_port,
			u64 transaction, const void *body, size_t body_len,
			u32 expected_device_generation)
{
	struct rtl_otto_tagger_data *tagger_data =
		stack->priv->ds->tagger_data;
	enum rtl838x_eth_device_talk_mode tx_mode;
	struct rtl931x_talk_header *header;
	struct net_device *conduit = stack->talk_conduit;
	unsigned int frame_len, message_len;
	struct sk_buff *skb;
	struct ethhdr *eth;
	u8 device;
	int err;

	if (!conduit || !tagger_data)
		return -EOPNOTSUPP;
	if (!body && body_len)
		return -EINVAL;
	if (body_len > RTL931X_TALK_MAX_MESSAGE_LEN - sizeof(*header))
		return -EMSGSIZE;

	read_lock_bh(&tagger_data->cpu_device_lock);
	if (tagger_data->cpu_device_changing ||
	    tagger_data->cpu_device_generation !=
		expected_device_generation) {
		read_unlock_bh(&tagger_data->cpu_device_lock);
		return -ESTALE;
	}
	device = tagger_data->cpu_device;
	read_unlock_bh(&tagger_data->cpu_device_lock);

	message_len = sizeof(*header) + body_len;
	frame_len = max_t(unsigned int, ETH_ZLEN, ETH_HLEN + message_len);
	skb = netdev_alloc_skb(conduit, frame_len);
	if (!skb)
		return -ENOMEM;

	skb_reset_mac_header(skb);
	eth = skb_put_zero(skb, frame_len);
	ether_addr_copy(eth->h_dest, rtl931x_talk_dest);
	ether_addr_copy(eth->h_source, conduit->dev_addr);
	eth->h_proto = htons(ETH_P_802_EX1);

	header = (struct rtl931x_talk_header *)(eth + 1);
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
	header->message_len = message_len;
	if (body_len)
		memcpy(header + 1, body, body_len);
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

static int
rtl931x_stack_talk_send(struct rtl931x_stack_context *stack, u8 type,
			unsigned int mode, u8 target, u8 stack_port,
			u64 transaction, u32 expected_device_generation)
{
	struct rtl931x_talk_probe probe = {};
	u32 device_generation;
	u8 device, master;
	u32 generation, status;
	int err;

	err = rtl931x_stack_cpu_device_get(stack->priv, &device_generation);
	if (err < 0)
		return err;
	if (device_generation != expected_device_generation)
		return -ESTALE;
	device = err;
	generation = READ_ONCE(stack->generation_valid) ?
		     READ_ONCE(stack->generation) : 0;
	status = rtl931x_stack_talk_status(stack, device, stack_port);
	master = status & RTL931X_STACK_TALK_S_ID_VALID ?
		 READ_ONCE(stack->master_id) : RTL931X_TALK_TARGET_ONE_HOP;

	probe.status = cpu_to_be32(status);
	probe.generation = cpu_to_be32(generation);
	probe.master_device = master;

	return rtl931x_stack_talk_xmit(stack, type, mode, target, stack_port,
				       transaction, &probe, sizeof(probe),
				       expected_device_generation);
}

static int
rtl931x_stack_talk_rpc_xmit(struct rtl931x_stack_context *stack, u8 type,
			    u8 target, u8 stack_port, u64 transaction,
			    u16 opcode, u16 result, u64 receiver_boot_nonce,
			    u32 generation, u8 master, const void *body,
			    size_t body_len, u32 expected_device_generation)
{
	struct rtl931x_talk_rpc_wire_body wire = {};

	if (!body && body_len)
		return -EINVAL;
	if (body_len > RTL931X_STACK_RPC_MAX_BODY_LEN)
		return -EMSGSIZE;

	wire.header.receiver_boot_nonce = cpu_to_be64(receiver_boot_nonce);
	wire.header.generation = cpu_to_be32(generation);
	wire.header.opcode = cpu_to_be16(opcode);
	wire.header.body_len = cpu_to_be16(body_len);
	wire.header.result = cpu_to_be16(result);
	wire.header.master_device = master;
	if (body_len)
		memcpy(wire.body, body, body_len);

	return rtl931x_stack_talk_xmit(stack, type,
				       RTL931X_STACK_TALK_MODE_UNICAST,
				       target, stack_port, transaction, &wire,
				       sizeof(wire.header) + body_len,
				       expected_device_generation);
}

static u64
rtl931x_stack_talk_user_port_mask(struct rtl931x_stack_context *stack)
{
	struct dsa_port *dp;
	u64 mask = 0;

	dsa_switch_for_each_user_port(dp, stack->priv->ds) {
		if (dp->index != READ_ONCE(stack->port))
			mask |= BIT_ULL(dp->index);
	}

	return mask;
}

static int
rtl931x_stack_set_local_delegated_ports(struct rtl931x_stack_context *stack,
					bool delegated,
					struct netlink_ext_ack *extack)
{
	u64 old_mask = stack->delegated_port_mask;
	u64 target_mask = delegated ?
		rtl931x_stack_talk_user_port_mask(stack) : 0;
	u64 changed_mask = old_mask ^ target_mask;
	u64 applied_mask = 0;
	struct dsa_port *dp;
	bool desired;
	int port, err, rollback_err = 0;

	ASSERT_RTNL();

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		u64 bit = BIT_ULL(port);

		if (!(changed_mask & bit))
			continue;
		dp = dsa_to_port(stack->priv->ds, port);
		if (!dp || !dsa_port_is_user(dp)) {
			err = -ENODEV;
			goto rollback;
		}

		desired = target_mask & bit;
		err = dsa_port_set_delegated(dp, desired, extack);
		if (err) {
			netdev_err(dp->user,
				   "failed to %s stack port ownership: %pe\n",
				   desired ? "delegate" : "restore",
				   ERR_PTR(err));
			goto rollback;
		}
		if (desired)
			stack->delegated_port_mask |= bit;
		else
			stack->delegated_port_mask &= ~bit;
		applied_mask |= bit;
	}

	return 0;

rollback:
	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		u64 bit = BIT_ULL(port);
		int restore_err;

		if (!(applied_mask & bit))
			continue;
		dp = dsa_to_port(stack->priv->ds, port);
		desired = old_mask & bit;
		restore_err = dsa_port_set_delegated(dp, desired, NULL);
		if (restore_err) {
			dev_err(stack->priv->dev,
				"failed to roll back delegation for port %d: %pe\n",
				port, ERR_PTR(restore_err));
			rollback_err = restore_err;
			continue;
		}
		if (desired)
			stack->delegated_port_mask |= bit;
		else
			stack->delegated_port_mask &= ~bit;
	}

	return rollback_err ? -EIO : err;
}

static int
rtl931x_stack_delegation_preflight(struct rtl931x_stack_context *stack)
{
	u64 user_port_mask = rtl931x_stack_talk_user_port_mask(stack);
	struct dsa_port *dp;

	ASSERT_RTNL();

	dsa_switch_for_each_user_port(dp, stack->priv->ds) {
		bool tracked;

		if (!(user_port_mask & BIT_ULL(dp->index)))
			continue;
		if (!dp->user)
			return -ENODEV;
		tracked = stack->delegated_port_mask & BIT_ULL(dp->index);
		if (tracked != READ_ONCE(dp->delegated)) {
			netdev_err(dp->user,
				   "stack delegation state disagrees with DSA\n");
			return -EUCLEAN;
		}
		if (tracked)
			continue;
		if (!netif_running(dp->user) ||
		    !netif_device_present(dp->user)) {
			netdev_err(dp->user,
				   "interface must be administratively up for stack delegation\n");
			return -ENETDOWN;
		}
		if (dp->bridge || dp->lag || dp->hsr_dev ||
		    netdev_has_any_upper_dev(dp->user)) {
			netdev_err(dp->user,
				   "remove bridge, LAG, HSR and other uppers before stack delegation\n");
			return -EPERM;
		}
	}

	return 0;
}

static struct rtl931x_stack_host_fdb *
rtl931x_stack_host_find(struct rtl931x_stack_context *stack,
			const unsigned char *addr)
{
	int i;

	for (i = 0; i < stack->delegated_host_count; i++)
		if (ether_addr_equal(stack->delegated_hosts[i].addr, addr))
			return &stack->delegated_hosts[i];

	return NULL;
}

static bool rtl931x_stack_hosts_match_users(struct rtl931x_stack_context *stack)
{
	u64 user_port_mask = rtl931x_stack_talk_user_port_mask(stack);
	bool seen[RTL931X_STACK_MAX_PORTS] = {};
	struct rtl931x_stack_host_fdb *host;
	struct dsa_port *dp;
	int unique = 0;

	dsa_switch_for_each_user_port(dp, stack->priv->ds) {
		int index;

		if (!(user_port_mask & BIT_ULL(dp->index)))
			continue;
		if (!dp->user || !is_valid_ether_addr(dp->user->dev_addr))
			return false;
		host = rtl931x_stack_host_find(stack, dp->user->dev_addr);
		if (!host)
			return false;
		index = host - stack->delegated_hosts;
		if (!seen[index]) {
			seen[index] = true;
			unique++;
		}
	}

	return unique == stack->delegated_host_count;
}

static int rtl931x_stack_hosts_move(struct rtl931x_stack_context *stack,
				    u8 device)
{
	u8 local = READ_ONCE(stack->member_id);
	u8 master = READ_ONCE(stack->master_id);
	u8 other = device == local ? master : local;
	int i, err;

	for (i = 0; i < stack->delegated_host_count; i++) {
		struct rtl931x_stack_host_fdb *host =
			&stack->delegated_hosts[i];

		err = rtl931x_stack_host_fdb_set_device(stack->priv, host->addr,
							other, device);
		if (err) {
			dev_err(stack->priv->dev,
				"failed to move stack host FDB %pM to device %u: %pe\n",
				host->addr, device, ERR_PTR(err));
			return err;
		}
	}

	return 0;
}

static int rtl931x_stack_hosts_cleanup(struct rtl931x_stack_context *stack)
{
	u8 local = READ_ONCE(stack->member_id);
	int first_err = 0;
	int remaining = 0;
	int i, err;

	for (i = 0; i < stack->delegated_host_count; i++) {
		struct rtl931x_stack_host_fdb *host =
			&stack->delegated_hosts[i];

		if (!host->created)
			continue;
		err = rtl931x_stack_host_fdb_remove(stack->priv, host->addr, local);
		if (err) {
			dev_err(stack->priv->dev,
				"failed to remove stack host FDB %pM: %pe\n",
				host->addr, ERR_PTR(err));
			if (!first_err)
				first_err = err;
			if (remaining != i)
				stack->delegated_hosts[remaining] = *host;
			remaining++;
			continue;
		}
	}
	if (first_err) {
		memset(&stack->delegated_hosts[remaining], 0,
		       sizeof(stack->delegated_hosts) -
		       remaining * sizeof(stack->delegated_hosts[0]));
		stack->delegated_host_count = remaining;
		return first_err;
	}

	memset(stack->delegated_hosts, 0, sizeof(stack->delegated_hosts));
	stack->delegated_host_count = 0;
	return 0;
}

static int rtl931x_stack_hosts_prepare(struct rtl931x_stack_context *stack)
{
	u64 user_port_mask = rtl931x_stack_talk_user_port_mask(stack);
	struct rtl838x_switch_priv *priv = stack->priv;
	u8 local = READ_ONCE(stack->member_id);
	struct rtl931x_stack_host_fdb *host;
	struct dsa_port *dp;
	bool created;
	int err;

	if (stack->delegated_host_count)
		return rtl931x_stack_hosts_match_users(stack) ? 0 : -EUCLEAN;

	dsa_switch_for_each_user_port(dp, stack->priv->ds) {
		if (!(user_port_mask & BIT_ULL(dp->index)))
			continue;
		if (!dp->user || !is_valid_ether_addr(dp->user->dev_addr)) {
			err = -ENODEV;
			goto rollback;
		}
		if (rtl931x_stack_host_find(stack, dp->user->dev_addr))
			continue;
		if (stack->delegated_host_count >= RTL931X_STACK_MAX_PORTS) {
			err = -E2BIG;
			goto rollback;
		}

		host = &stack->delegated_hosts[stack->delegated_host_count++];
		ether_addr_copy(host->addr, dp->user->dev_addr);
		created = false;
		err = rtl931x_stack_host_fdb_prepare(priv, host->addr, local, &created);
		host->created = created;
		if (err) {
			dev_err(priv->dev,
				"failed to prepare stack host FDB %pM: %pe\n",
				host->addr, ERR_PTR(err));
			if (!created) {
				memset(host, 0, sizeof(*host));
				stack->delegated_host_count--;
			}
			goto rollback;
		}
	}

	return stack->delegated_host_count ? 0 : -ENODEV;

rollback:
	if (rtl931x_stack_hosts_cleanup(stack))
		return -EIO;
	return err;
}

static int
rtl931x_stack_set_local_delegated(struct rtl931x_stack_context *stack,
				  bool delegated,
				  struct netlink_ext_ack *extack)
{
	u64 user_port_mask = rtl931x_stack_talk_user_port_mask(stack);
	u8 local = READ_ONCE(stack->member_id);
	u8 master = READ_ONCE(stack->master_id);
	u8 target;
	int rollback_err;
	int err;

	ASSERT_RTNL();
	if (delegated) {
		if (stack->delegated_port_mask &&
		    stack->delegated_port_mask != user_port_mask)
			return -EUCLEAN;
		if (!stack->delegated_port_mask &&
		    stack->delegated_host_count) {
			err = rtl931x_stack_hosts_cleanup(stack);
			if (err)
				return err;
		}
		err = rtl931x_stack_delegation_preflight(stack);
		if (err)
			return err;
		err = rtl931x_stack_hosts_prepare(stack);
		if (err)
			return err;
		target = stack->delegated_port_mask ? master : local;
		err = rtl931x_stack_hosts_move(stack, target);
		if (err)
			return err;

		err = rtl931x_stack_set_local_delegated_ports(stack, true,
							      extack);
		if (err) {
			if (rtl931x_stack_hosts_move(stack, local))
				return -EIO;
			if (!stack->delegated_port_mask &&
			    rtl931x_stack_hosts_cleanup(stack))
				return -EIO;
			return err;
		}

		err = rtl931x_stack_hosts_move(stack, master);
		if (!err)
			return 0;

		rollback_err = rtl931x_stack_hosts_move(stack, local);
		if (!rollback_err)
			rollback_err =
				rtl931x_stack_set_local_delegated_ports(stack, false,
									NULL);
		if (!rollback_err || !stack->delegated_port_mask) {
			if (rtl931x_stack_hosts_cleanup(stack))
				return -EIO;
			return err;
		}
		if (stack->delegated_port_mask == user_port_mask &&
		    !rtl931x_stack_hosts_move(stack, master))
			return err;
		return -EIO;
	}

	if (!stack->delegated_port_mask && !stack->delegated_host_count)
		return 0;
	if (!stack->delegated_port_mask)
		return rtl931x_stack_hosts_cleanup(stack);
	if (!stack->delegated_host_count)
		return -EUCLEAN;

	err = rtl931x_stack_hosts_move(stack, local);
	if (err) {
		if (rtl931x_stack_hosts_move(stack, master))
			return -EIO;
		return err;
	}
	err = rtl931x_stack_set_local_delegated_ports(stack, false, extack);
	if (!err)
		return rtl931x_stack_hosts_cleanup(stack);
	if (stack->delegated_port_mask != user_port_mask)
		return -EIO;

	rollback_err = rtl931x_stack_hosts_move(stack, master);
	return rollback_err ? -EIO : err;
}

static int
rtl931x_stack_undelegate_local_ports(struct rtl931x_stack_context *stack,
				     bool force_down)
{
	u64 delegated_mask = stack->delegated_port_mask;
	struct dsa_port *dp;
	int port;
	int err;

	ASSERT_RTNL();
	rtl931x_stack_bridge_cleanup(stack->priv);
	if (!delegated_mask && !stack->delegated_host_count)
		return 0;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		u64 bit = BIT_ULL(port);

		if (!(delegated_mask & bit))
			continue;
		dp = dsa_to_port(stack->priv->ds, port);
		if (!dp || !dsa_port_is_user(dp))
			return -ENODEV;
		if (force_down && netif_running(dp->user))
			dev_close(dp->user);
	}

	err = rtl931x_stack_set_local_delegated(stack, false, NULL);
	if (err)
		dev_err(stack->priv->dev,
			"failed to restore delegated ports: %pe\n", ERR_PTR(err));

	return err;
}

static struct rtl931x_stack_fabric_vlan *
rtl931x_stack_fabric_vlan_find(struct list_head *head, u16 vid)
{
	struct rtl931x_stack_fabric_vlan *vlan;

	list_for_each_entry(vlan, head, list)
		if (vlan->vid == vid)
			return vlan;

	return NULL;
}

static struct rtl931x_stack_port_vlan *
rtl931x_stack_port_vlan_find(struct rtl931x_stack_context *stack, u8 port,
			     u16 vid)
{
	struct rtl931x_stack_port_vlan *vlan;

	list_for_each_entry(vlan, &stack->peer_port_vlans, list)
		if (vlan->port == port && vlan->vid == vid)
			return vlan;

	return NULL;
}

static void rtl931x_stack_vlan_write(struct rtl838x_switch_priv *priv, u16 vid,
				     struct rtldsa_vlan_info *info)
{
	info->untagged_ports &= info->member_ports;
	priv->r->vlan_set_untagged(vid, info->untagged_ports);
	priv->r->vlan_set_tagged(vid, info);
}

static int
rtl931x_stack_fabric_vlan_get(struct rtl931x_stack_context *stack,
			      struct list_head *head, u16 vid)
{
	struct rtl931x_stack_fabric_vlan *vlan;
	struct rtldsa_vlan_info info;
	u64 bit = BIT_ULL(stack->port);

	vlan = rtl931x_stack_fabric_vlan_find(head, vid);
	if (vlan) {
		if (vlan->refs == U16_MAX)
			return -EOVERFLOW;
		vlan->refs++;
		return 0;
	}

	vlan = kzalloc(sizeof(*vlan), GFP_KERNEL);
	if (!vlan)
		return -ENOMEM;

	stack->priv->r->vlan_tables_read(vid, &info);
	vlan->vid = vid;
	vlan->refs = 1;
	vlan->saved_member = !!(info.member_ports & bit);
	vlan->saved_untagged = !!(info.untagged_ports & bit);
	if (!info.member_ports) {
		vlan->saved_fid = info.fid;
		vlan->saved_profile_id = info.profile_id;
		vlan->saved_hash_mc_fid = info.hash_mc_fid;
		vlan->saved_hash_uc_fid = info.hash_uc_fid;
		vlan->initialized_metadata = true;
		info.fid = 0;
		info.hash_mc_fid = false;
		info.hash_uc_fid = false;
		info.profile_id = 0;
	}
	info.member_ports |= bit;
	info.untagged_ports &= ~bit;
	rtl931x_stack_vlan_write(stack->priv, vid, &info);
	list_add_tail(&vlan->list, head);

	return 0;
}

static void
rtl931x_stack_fabric_vlan_put(struct rtl931x_stack_context *stack,
			      struct list_head *head, u16 vid)
{
	struct rtl931x_stack_fabric_vlan *vlan;
	struct rtldsa_vlan_info info;
	u64 bit = BIT_ULL(stack->port);

	vlan = rtl931x_stack_fabric_vlan_find(head, vid);
	if (!vlan || WARN_ON_ONCE(!vlan->refs))
		return;
	if (--vlan->refs)
		return;

	stack->priv->r->vlan_tables_read(vid, &info);
	if (vlan->saved_member)
		info.member_ports |= bit;
	else
		info.member_ports &= ~bit;
	if (vlan->saved_untagged)
		info.untagged_ports |= bit;
	else
		info.untagged_ports &= ~bit;
	if (vlan->initialized_metadata && !info.member_ports) {
		info.fid = vlan->saved_fid;
		info.profile_id = vlan->saved_profile_id;
		info.hash_mc_fid = vlan->saved_hash_mc_fid;
		info.hash_uc_fid = vlan->saved_hash_uc_fid;
	}
	rtl931x_stack_vlan_write(stack->priv, vid, &info);
	list_del(&vlan->list);
	kfree(vlan);
}

static int rtl931x_stack_bridge_port_hw(struct rtl931x_stack_context *stack,
					u8 port, bool present, bool fabric)
{
	u64 bit = BIT_ULL(port);
	int err;

	if (present) {
		if (!(stack->bridge_saved_port_mask & bit)) {
			rtl931x_stack_bridge_port_save(port,
					&stack->bridge_saved[port]);
			stack->bridge_saved_pvid[port] =
				stack->priv->ports[port].pvid;
			stack->bridge_saved_port_mask |= bit;
		}
		rtl931x_stack_bridge_port_apply(port, fabric, stack->port,
						 stack->priv->r->cpu_port);
		stack->priv->ports[port].pvid = 0;
		if (!fabric && stack->priv->r->fast_age) {
			err = stack->priv->r->fast_age(stack->priv, port, -1);
			if (err) {
				rtl931x_stack_bridge_port_restore(port,
						&stack->bridge_saved[port]);
				stack->priv->ports[port].pvid =
					stack->bridge_saved_pvid[port];
				memset(&stack->bridge_saved[port], 0,
				       sizeof(stack->bridge_saved[port]));
				stack->bridge_saved_pvid[port] = 0;
				stack->bridge_saved_port_mask &= ~bit;
				return err;
			}
		}
		return 0;
	}

	if (!(stack->bridge_saved_port_mask & bit))
		return 0;
	rtl931x_stack_bridge_port_restore(port, &stack->bridge_saved[port]);
	stack->priv->ports[port].pvid = stack->bridge_saved_pvid[port];
	memset(&stack->bridge_saved[port], 0,
	       sizeof(stack->bridge_saved[port]));
	stack->bridge_saved_pvid[port] = 0;
	stack->bridge_saved_port_mask &= ~bit;

	return 0;
}

static int rtl931x_stack_bridge_fabric_get(struct rtl931x_stack_context *stack)
{
	int err;

	if (stack->bridge_fabric_users == U8_MAX)
		return -EOVERFLOW;
	if (!stack->bridge_fabric_users) {
		err = rtl931x_stack_bridge_port_hw(stack, stack->port, true, true);
		if (err)
			return err;
	}
	stack->bridge_fabric_users++;

	return 0;
}

static void rtl931x_stack_bridge_fabric_put(struct rtl931x_stack_context *stack)
{
	if (WARN_ON_ONCE(!stack->bridge_fabric_users))
		return;
	if (!--stack->bridge_fabric_users)
		WARN_ON_ONCE(rtl931x_stack_bridge_port_hw(stack, stack->port,
							 false, true));
}

static int
rtl931x_stack_peer_port_vlan_set_local(struct rtl931x_stack_context *stack,
				       u8 port, u16 vid, u16 flags,
				       bool present)
{
	struct rtl931x_stack_port_vlan *vlan;
	struct rtldsa_vlan_info info;
	u64 bit = BIT_ULL(port);
	int err;

	vlan = rtl931x_stack_port_vlan_find(stack, port, vid);
	if (!present) {
		if (!vlan)
			return 0;
		stack->priv->r->vlan_tables_read(vid, &info);
		if (vlan->saved_member)
			info.member_ports |= bit;
		else
			info.member_ports &= ~bit;
		if (vlan->saved_untagged)
			info.untagged_ports |= bit;
		else
			info.untagged_ports &= ~bit;
		rtl931x_stack_vlan_write(stack->priv, vid, &info);
		if (stack->priv->ports[port].pvid == vid) {
			stack->priv->r->vlan_port_pvid_set(port,
							 PBVLAN_TYPE_INNER, 0);
			stack->priv->r->vlan_port_pvid_set(port,
							 PBVLAN_TYPE_OUTER, 0);
			stack->priv->ports[port].pvid = 0;
		}
		rtl931x_stack_fabric_vlan_put(stack,
					       &stack->peer_fabric_vlans, vid);
		list_del(&vlan->list);
		kfree(vlan);
		return 0;
	}

	if (!vlan) {
		vlan = kzalloc(sizeof(*vlan), GFP_KERNEL);
		if (!vlan)
			return -ENOMEM;
		stack->priv->r->vlan_tables_read(vid, &info);
		vlan->port = port;
		vlan->vid = vid;
		vlan->saved_member = !!(info.member_ports & bit);
		vlan->saved_untagged = !!(info.untagged_ports & bit);
		err = rtl931x_stack_fabric_vlan_get(stack,
						     &stack->peer_fabric_vlans,
						     vid);
		if (err) {
			kfree(vlan);
			return err;
		}
		list_add_tail(&vlan->list, &stack->peer_port_vlans);
	}

	stack->priv->r->vlan_tables_read(vid, &info);
	if (!info.member_ports) {
		info.fid = 0;
		info.hash_mc_fid = false;
		info.hash_uc_fid = false;
		info.profile_id = 0;
	}
	info.member_ports |= bit | BIT_ULL(stack->port);
	if (flags & RTL931X_STACK_VLAN_F_UNTAGGED)
		info.untagged_ports |= bit;
	else
		info.untagged_ports &= ~bit;
	info.untagged_ports &= ~BIT_ULL(stack->port);
	rtl931x_stack_vlan_write(stack->priv, vid, &info);
	if (flags & RTL931X_STACK_VLAN_F_PVID) {
		stack->priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_INNER, vid);
		stack->priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_OUTER, vid);
		stack->priv->ports[port].pvid = vid;
	} else if (stack->priv->ports[port].pvid == vid) {
		stack->priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_INNER, 0);
		stack->priv->r->vlan_port_pvid_set(port, PBVLAN_TYPE_OUTER, 0);
		stack->priv->ports[port].pvid = 0;
	}
	vlan->flags = flags;

	return 0;
}

static void
rtl931x_stack_peer_port_vlans_clear(struct rtl931x_stack_context *stack,
				    u8 port)
{
	struct rtl931x_stack_port_vlan *vlan, *tmp;

	list_for_each_entry_safe(vlan, tmp, &stack->peer_port_vlans, list)
		if (vlan->port == port)
			rtl931x_stack_peer_port_vlan_set_local(stack, port,
							      vlan->vid, 0,
							      false);
}

static int rtl931x_stack_peer_bridge_port_set_local(
					struct rtl931x_stack_context *stack,
					u8 port, bool present)
{
	u64 bit = BIT_ULL(port);
	int err;

	if (present) {
		if (stack->peer_bridge_port_mask & bit)
			return 0;
		err = rtl931x_stack_bridge_fabric_get(stack);
		if (err)
			return err;
		err = rtl931x_stack_bridge_port_hw(stack, port, true, false);
		if (err) {
			rtl931x_stack_bridge_fabric_put(stack);
			return err;
		}
		stack->peer_bridge_port_mask |= bit;
		return 0;
	}

	if (!(stack->peer_bridge_port_mask & bit))
		return 0;
	rtl931x_stack_peer_port_vlans_clear(stack, port);
	WARN_ON_ONCE(rtl931x_stack_bridge_port_hw(stack, port, false, false));
	stack->peer_bridge_port_mask &= ~bit;
	rtl931x_stack_bridge_fabric_put(stack);

	return 0;
}

int rtl931x_stack_local_bridge_port(struct rtl838x_switch_priv *priv,
				    bool present)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	int err = 0;

	ASSERT_RTNL();
	mutex_lock(&priv->reg_mutex);
	if (present)
		err = rtl931x_stack_bridge_fabric_get(stack);
	else
		rtl931x_stack_bridge_fabric_put(stack);
	mutex_unlock(&priv->reg_mutex);

	return err;
}

int rtl931x_stack_local_fabric_vlan(struct rtl838x_switch_priv *priv, u16 vid,
				    bool present)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	int err = 0;

	ASSERT_RTNL();
	if (!vid || vid >= MAX_VLANS)
		return -EINVAL;

	mutex_lock(&priv->reg_mutex);
	if (present)
		err = rtl931x_stack_fabric_vlan_get(stack,
						     &stack->local_fabric_vlans,
						     vid);
	else
		rtl931x_stack_fabric_vlan_put(stack,
					       &stack->local_fabric_vlans, vid);
	mutex_unlock(&priv->reg_mutex);

	return err;
}

void rtl931x_stack_local_bridge_replay(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl931x_stack_fabric_vlan *vlan;
	struct rtl838x_vlan_info info;
	u64 bit = BIT_ULL(stack->port);

	ASSERT_RTNL();
	mutex_lock(&priv->reg_mutex);
	if (stack->bridge_fabric_users) {
		rtl931x_stack_bridge_port_apply(stack->port, true, stack->port,
					 stack->priv->r->cpu_port);
		stack->priv->ports[stack->port].pvid = 0;
	}
	list_for_each_entry(vlan, &stack->local_fabric_vlans, list) {
		priv->r->vlan_tables_read(vlan->vid, &info);
		info.member_ports |= bit;
		info.untagged_ports &= ~bit;
		rtl931x_stack_vlan_write(priv, vlan->vid, &info);
	}
	mutex_unlock(&priv->reg_mutex);
}

void rtl931x_stack_bridge_cleanup(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl931x_stack_fabric_vlan *fabric, *fabric_tmp;
	struct rtl931x_stack_port_vlan *vlan, *vlan_tmp;
	int port;

	ASSERT_RTNL();
	mutex_lock(&priv->reg_mutex);
	list_for_each_entry_safe(vlan, vlan_tmp, &stack->peer_port_vlans, list)
		rtl931x_stack_peer_port_vlan_set_local(stack, vlan->port,
							  vlan->vid, 0, false);
	list_for_each_entry_safe(fabric, fabric_tmp,
				 &stack->local_fabric_vlans, list) {
		fabric->refs = 1;
		rtl931x_stack_fabric_vlan_put(stack,
					       &stack->local_fabric_vlans,
					       fabric->vid);
	}
	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++)
		if (stack->peer_bridge_port_mask & BIT_ULL(port))
			rtl931x_stack_peer_bridge_port_set_local(stack, port,
							    false);
	while (stack->bridge_fabric_users)
		rtl931x_stack_bridge_fabric_put(stack);
	stack->peer_bridge_port_mask = 0;
	WARN_ON_ONCE(!list_empty(&stack->peer_fabric_vlans));
	mutex_unlock(&priv->reg_mutex);
}

struct rtl931x_talk_rpc_request_context {
	const struct rtl931x_talk_header *header;
	const struct rtl931x_talk_rpc_header *rpc;
	const struct rtl931x_talk_rx_metadata *metadata;
	const void *body;
};

static u16
rtl931x_stack_talk_rpc_get_switch_info(struct rtl931x_stack_context *stack,
			const struct rtl931x_talk_rpc_request_context *request,
			void *reply)
{
	struct rtl931x_talk_rpc_switch_info *info = reply;
	u64 user_port_mask;
	u64 admin_up_mask = 0;
	u64 carrier_mask = 0;
	u64 delegated_port_mask = 0;
	u32 capabilities;
	struct dsa_port *dp;
	u16 result = RTL931X_TALK_RPC_OK;

	if (!rtnl_trylock())
		return RTL931X_TALK_RPC_BUSY;
	user_port_mask = rtl931x_stack_talk_user_port_mask(stack);

	dsa_switch_for_each_user_port(dp, stack->priv->ds) {
		if (!(user_port_mask & BIT_ULL(dp->index)))
			continue;
		if (!dp->user) {
			result = RTL931X_TALK_RPC_NO_DEVICE;
			goto out_unlock;
		}
		if (netif_running(dp->user))
			admin_up_mask |= BIT_ULL(dp->index);
		if (netif_carrier_ok(dp->user))
			carrier_mask |= BIT_ULL(dp->index);
		if (READ_ONCE(dp->delegated))
			delegated_port_mask |= BIT_ULL(dp->index);
	}
	if (!user_port_mask) {
		result = RTL931X_TALK_RPC_NO_DEVICE;
		goto out_unlock;
	}

	info->user_port_mask = cpu_to_be64(user_port_mask);
	info->admin_up_mask = cpu_to_be64(admin_up_mask);
	info->carrier_mask = cpu_to_be64(carrier_mask);
	info->delegated_port_mask = cpu_to_be64(delegated_port_mask);
	capabilities =
		RTL931X_STACK_PEER_CAP_GET_SWITCH |
		RTL931X_STACK_PEER_CAP_GET_PORT_STATE |
		RTL931X_STACK_PEER_CAP_SET_DELEGATED;
	if (!READ_ONCE(stack->flags))
		capabilities |= RTL931X_STACK_PEER_CAP_BRIDGE_VLAN;
	info->capabilities = cpu_to_be32(capabilities);
	info->max_body_len = cpu_to_be16(RTL931X_STACK_RPC_MAX_BODY_LEN);
	info->port_count = hweight64(user_port_mask);
	info->cpu_port = stack->priv->r->cpu_port;
	info->stack_port = READ_ONCE(stack->port);
	info->protocol_version = RTL931X_TALK_VERSION;

out_unlock:
	rtnl_unlock();
	return result;
}

static u16
rtl931x_stack_talk_rpc_get_port_state(struct rtl931x_stack_context *stack,
			const struct rtl931x_talk_rpc_request_context *request,
			void *reply)
{
	const struct rtl931x_talk_rpc_port_request *port_request = request->body;
	struct rtl931x_talk_rpc_port_state *state = reply;
	const struct dsa_port *dp;
	u8 port = port_request->port;

	if (memchr_inv(port_request->reserved, 0,
		       sizeof(port_request->reserved)) ||
	    port >= stack->priv->ds->num_ports ||
	    port == READ_ONCE(stack->port))
		return RTL931X_TALK_RPC_INVALID;
	if (!rtnl_trylock())
		return RTL931X_TALK_RPC_BUSY;

	dp = stack->priv->ports[port].dp;
	if (!dp || !dsa_is_user_port(stack->priv->ds, port) || !dp->user) {
		rtnl_unlock();
		return RTL931X_TALK_RPC_NO_DEVICE;
	}

	state->port = port;
	if (netif_running(dp->user))
		state->flags |= RTL931X_STACK_PEER_PORT_F_ADMIN_UP;
	if (netif_carrier_ok(dp->user))
		state->flags |= RTL931X_STACK_PEER_PORT_F_CARRIER;
	state->mtu = cpu_to_be32(READ_ONCE(dp->user->mtu));
	ether_addr_copy(state->mac, dp->user->dev_addr);
	if (!is_valid_ether_addr(state->mac)) {
		rtnl_unlock();
		return RTL931X_TALK_RPC_NO_DEVICE;
	}
	rtnl_unlock();

	return RTL931X_TALK_RPC_OK;
}

static u16 rtl931x_stack_talk_rpc_errno(int err)
{
	switch (err) {
	case 0:
		return RTL931X_TALK_RPC_OK;
	case -EINVAL:
		return RTL931X_TALK_RPC_INVALID;
	case -EOPNOTSUPP:
		return RTL931X_TALK_RPC_UNSUPPORTED;
	case -EPERM:
		return RTL931X_TALK_RPC_DENIED;
	case -ENETDOWN:
	case -EBUSY:
		return RTL931X_TALK_RPC_BUSY;
	case -ENODEV:
		return RTL931X_TALK_RPC_NO_DEVICE;
	default:
		return RTL931X_TALK_RPC_IO;
	}
}

static u16
rtl931x_stack_talk_mutation_begin(struct rtl931x_stack_context *stack,
				  const struct rtl931x_talk_rpc_request_context *request,
				  size_t request_len, bool *replay)
{
	u32 sequence = get_unaligned_be32(request->body);
	u16 opcode = be16_to_cpu(request->rpc->opcode);

	*replay = false;
	if (sequence == stack->last_mutation_sequence) {
		if (!stack->last_mutation_sequence ||
		    opcode != stack->last_mutation_opcode ||
		    request_len != stack->last_mutation_len ||
		    memcmp(request->body, stack->last_mutation_body,
			   request_len))
			return RTL931X_TALK_RPC_STALE_GENERATION;
		*replay = true;
		return stack->last_mutation_result;
	}
	if (!sequence || sequence != stack->last_mutation_sequence + 1)
		return RTL931X_TALK_RPC_STALE_GENERATION;

	return RTL931X_TALK_RPC_OK;
}

static void
rtl931x_stack_talk_mutation_commit(struct rtl931x_stack_context *stack,
				   const struct rtl931x_talk_rpc_request_context *request,
				   size_t request_len, u16 result)
{
	stack->last_mutation_sequence = get_unaligned_be32(request->body);
	stack->last_mutation_opcode = be16_to_cpu(request->rpc->opcode);
	stack->last_mutation_len = request_len;
	stack->last_mutation_result = result;
	memcpy(stack->last_mutation_body, request->body, request_len);
}

static u16
rtl931x_stack_talk_rpc_set_delegated(struct rtl931x_stack_context *stack,
				     const struct rtl931x_talk_rpc_request_context *request,
				     void *reply)
{
	const struct rtl931x_talk_rpc_delegated_request *delegated =
		request->body;
	bool replay;
	u16 result;
	int err;

	if (delegated->enabled > 1 ||
	    memchr_inv(delegated->reserved, 0, sizeof(delegated->reserved)))
		return RTL931X_TALK_RPC_INVALID;
	if (!rtnl_trylock())
		return RTL931X_TALK_RPC_BUSY;

	if (!rtl931x_stack_talk_rpc_session_validate(stack,
						     request->header,
						     request->rpc,
						     request->metadata)) {
		result = RTL931X_TALK_RPC_STALE_SESSION;
	} else if (request->header->sender_device !=
		   READ_ONCE(stack->master_id)) {
		result = RTL931X_TALK_RPC_DENIED;
	} else {
		bool enabled = delegated->enabled;

		result = rtl931x_stack_talk_mutation_begin(stack, request,
							  sizeof(*delegated),
							  &replay);
		if (result || replay)
			goto out_unlock;
		/* SET_DELEGATED is the authoritative bridge-shadow reset. */
		rtl931x_stack_bridge_cleanup(stack->priv);
		err = rtl931x_stack_set_local_delegated(stack, enabled, NULL);
		result = rtl931x_stack_talk_rpc_errno(err);
		if (result == RTL931X_TALK_RPC_OK)
			rtl931x_stack_talk_mutation_commit(stack, request,
							   sizeof(*delegated),
							   result);
	}

out_unlock:
	rtnl_unlock();
	return result;
}

static u16
rtl931x_stack_talk_rpc_set_bridge_port(struct rtl931x_stack_context *stack,
			const struct rtl931x_talk_rpc_request_context *request,
			void *reply)
{
	const struct rtl931x_talk_rpc_bridge_port_request *bridge = request->body;
	const struct dsa_port *dp;
	bool replay;
	u16 result;
	int err;

	if (bridge->present > 1 ||
	    memchr_inv(bridge->reserved, 0, sizeof(bridge->reserved)) ||
	    bridge->port >= stack->priv->r->cpu_port ||
	    bridge->port == READ_ONCE(stack->port))
		return RTL931X_TALK_RPC_INVALID;
	if (!rtnl_trylock())
		return RTL931X_TALK_RPC_BUSY;
	if (!rtl931x_stack_talk_rpc_session_validate(stack, request->header,
						     request->rpc,
						     request->metadata)) {
		result = RTL931X_TALK_RPC_STALE_SESSION;
		goto out_unlock;
	}
	if (bridge->present && READ_ONCE(stack->flags)) {
		result = RTL931X_TALK_RPC_UNSUPPORTED;
		goto out_unlock;
	}
	dp = stack->priv->ports[bridge->port].dp;
	if (!dp || !dsa_is_user_port(stack->priv->ds, bridge->port) || !dp->user) {
		result = RTL931X_TALK_RPC_NO_DEVICE;
		goto out_unlock;
	}
	if (bridge->present && !READ_ONCE(dp->delegated)) {
		result = RTL931X_TALK_RPC_DENIED;
		goto out_unlock;
	}
	result = rtl931x_stack_talk_mutation_begin(stack, request,
						  sizeof(*bridge), &replay);
	if (result || replay)
		goto out_unlock;
	mutex_lock(&stack->priv->reg_mutex);
	err = rtl931x_stack_peer_bridge_port_set_local(stack, bridge->port,
							      bridge->present);
	mutex_unlock(&stack->priv->reg_mutex);
	result = rtl931x_stack_talk_rpc_errno(err);
	if (result == RTL931X_TALK_RPC_OK)
		rtl931x_stack_talk_mutation_commit(stack, request,
						   sizeof(*bridge), result);

out_unlock:
	rtnl_unlock();
	return result;
}

static u16
rtl931x_stack_talk_rpc_set_port_vlan(struct rtl931x_stack_context *stack,
			const struct rtl931x_talk_rpc_request_context *request,
			void *reply)
{
	const struct rtl931x_talk_rpc_port_vlan_request *vlan = request->body;
	u16 flags = be16_to_cpu(vlan->flags);
	u16 vid = be16_to_cpu(vlan->vid);
	const struct dsa_port *dp;
	bool replay;
	u16 result;
	int err;

	if (vlan->present > 1 || flags & ~RTL931X_STACK_VLAN_F_MASK ||
	    (!vlan->present && flags) ||
	    memchr_inv(vlan->reserved, 0, sizeof(vlan->reserved)) ||
	    !vid || vid >= MAX_VLANS ||
	    vlan->port >= stack->priv->r->cpu_port ||
	    vlan->port == READ_ONCE(stack->port))
		return RTL931X_TALK_RPC_INVALID;
	if (!rtnl_trylock())
		return RTL931X_TALK_RPC_BUSY;
	if (!rtl931x_stack_talk_rpc_session_validate(stack, request->header,
						     request->rpc,
						     request->metadata)) {
		result = RTL931X_TALK_RPC_STALE_SESSION;
		goto out_unlock;
	}
	if (vlan->present && READ_ONCE(stack->flags)) {
		result = RTL931X_TALK_RPC_UNSUPPORTED;
		goto out_unlock;
	}
	dp = stack->priv->ports[vlan->port].dp;
	if (!dp || !dsa_is_user_port(stack->priv->ds, vlan->port) || !dp->user) {
		result = RTL931X_TALK_RPC_NO_DEVICE;
		goto out_unlock;
	}
	if (vlan->present && (!READ_ONCE(dp->delegated) ||
			       !(stack->peer_bridge_port_mask &
				 BIT_ULL(vlan->port)))) {
		result = RTL931X_TALK_RPC_DENIED;
		goto out_unlock;
	}
	result = rtl931x_stack_talk_mutation_begin(stack, request,
						  sizeof(*vlan), &replay);
	if (result || replay)
		goto out_unlock;
	mutex_lock(&stack->priv->reg_mutex);
	err = rtl931x_stack_peer_port_vlan_set_local(stack, vlan->port, vid,
							    flags,
							    vlan->present);
	mutex_unlock(&stack->priv->reg_mutex);
	result = rtl931x_stack_talk_rpc_errno(err);
	if (result == RTL931X_TALK_RPC_OK)
		rtl931x_stack_talk_mutation_commit(stack, request,
						   sizeof(*vlan), result);

out_unlock:
	rtnl_unlock();
	return result;
}

struct rtl931x_talk_rpc_operation {
	u16 opcode;
	u16 request_len;
	u16 reply_len;
	bool master_only;
	u16 (*handler)(struct rtl931x_stack_context *stack,
		       const struct rtl931x_talk_rpc_request_context *request,
		       void *reply);
};

static const struct rtl931x_talk_rpc_operation rtl931x_talk_rpc_operations[] = {
	{
		.opcode = RTL931X_TALK_RPC_GET_SWITCH_INFO,
		.request_len = 0,
		.reply_len = sizeof(struct rtl931x_talk_rpc_switch_info),
		.handler = rtl931x_stack_talk_rpc_get_switch_info,
	},
	{
		.opcode = RTL931X_TALK_RPC_GET_PORT_STATE,
		.request_len = sizeof(struct rtl931x_talk_rpc_port_request),
		.reply_len = sizeof(struct rtl931x_talk_rpc_port_state),
		.handler = rtl931x_stack_talk_rpc_get_port_state,
	},
	{
		.opcode = RTL931X_TALK_RPC_SET_DELEGATED,
		.request_len = sizeof(struct rtl931x_talk_rpc_delegated_request),
		.reply_len = 0,
		.master_only = true,
		.handler = rtl931x_stack_talk_rpc_set_delegated,
	},
	{
		.opcode = RTL931X_TALK_RPC_SET_BRIDGE_PORT,
		.request_len = sizeof(struct rtl931x_talk_rpc_bridge_port_request),
		.reply_len = 0,
		.master_only = true,
		.handler = rtl931x_stack_talk_rpc_set_bridge_port,
	},
	{
		.opcode = RTL931X_TALK_RPC_SET_PORT_VLAN,
		.request_len = sizeof(struct rtl931x_talk_rpc_port_vlan_request),
		.reply_len = 0,
		.master_only = true,
		.handler = rtl931x_stack_talk_rpc_set_port_vlan,
	},
};

static const struct rtl931x_talk_rpc_operation *
rtl931x_stack_talk_rpc_operation(u16 opcode)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(rtl931x_talk_rpc_operations); i++)
		if (rtl931x_talk_rpc_operations[i].opcode == opcode)
			return &rtl931x_talk_rpc_operations[i];

	return NULL;
}

static void
rtl931x_stack_talk_rpc_complete(struct rtl931x_stack_context *stack,
				const struct rtl931x_talk_header *header,
				const struct rtl931x_talk_rpc_header *rpc,
				const struct rtl931x_talk_rx_metadata *metadata,
				const void *body)
{
	u16 body_len = be16_to_cpu(rpc->body_len);

	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending && stack->talk_pending_rpc &&
	    be64_to_cpu(header->transaction) ==
		stack->talk_pending_transaction &&
	    be16_to_cpu(rpc->opcode) == stack->talk_pending_opcode &&
	    metadata->ingress_port == stack->talk_pending_port &&
	    header->sender_device == stack->talk_pending_peer &&
	    header->target_device == stack->talk_pending_local) {
		stack->talk_rpc_result = be16_to_cpu(rpc->result);
		stack->talk_rpc_reply_len = body_len;
		if (body_len)
			memcpy(stack->talk_rpc_reply, body, body_len);
		stack->talk_pending = false;
		complete(&stack->talk_reply_completion);
	}
	spin_unlock_bh(&stack->talk_reply_lock);
}

static void
rtl931x_stack_talk_rpc_serve(struct rtl931x_stack_context *stack,
			     const struct rtl931x_talk_header *header,
			     const struct rtl931x_talk_rpc_header *rpc,
			     const struct rtl931x_talk_rx_metadata *metadata,
			     const void *body)
{
	const struct rtl931x_talk_rpc_operation *operation;
	const struct rtl931x_talk_rpc_request_context request = {
		.header = header,
		.rpc = rpc,
		.metadata = metadata,
		.body = body,
	};
	u8 reply[RTL931X_STACK_RPC_MAX_BODY_LEN] = {};
	u16 opcode = be16_to_cpu(rpc->opcode);
	u16 body_len = be16_to_cpu(rpc->body_len);
	u16 reply_len = 0;
	u16 result;

	operation = rtl931x_stack_talk_rpc_operation(opcode);
	if (!operation) {
		result = RTL931X_TALK_RPC_UNSUPPORTED;
	} else if (body_len != operation->request_len) {
		result = RTL931X_TALK_RPC_INVALID;
	} else if (operation->master_only &&
		   header->sender_device != READ_ONCE(stack->master_id)) {
		result = RTL931X_TALK_RPC_DENIED;
	} else {
		result = operation->handler(stack, &request, reply);
		if (result == RTL931X_TALK_RPC_OK)
			reply_len = operation->reply_len;
	}

	rtl931x_stack_talk_rpc_xmit(stack, RTL931X_TALK_TYPE_RPC_REPLY,
				    header->sender_device,
				    metadata->ingress_port,
				    be64_to_cpu(header->transaction), opcode,
				    result, be64_to_cpu(header->boot_nonce),
				    be32_to_cpu(rpc->generation),
				    rpc->master_device, reply, reply_len,
				    metadata->cpu_device_generation);
}

static int rtl931x_stack_talk_rcv(struct sk_buff *skb, struct net_device *dev,
				  struct packet_type *pt,
				  struct net_device *orig_dev)
{
	struct rtl931x_stack_context *stack =
		container_of(pt, struct rtl931x_stack_context, talk_packet_type);
	struct rtl931x_talk_rx_metadata metadata;
	struct rtl931x_talk_rpc_header rpc;
	struct rtl931x_talk_header header;
	struct rtl931x_talk_probe probe;
	bool queued = false;

	if (!rtl931x_stack_talk_header_validate(stack, skb, &header,
						&metadata))
		goto drop;
	if (header.type == RTL931X_TALK_TYPE_PING ||
	    header.type == RTL931X_TALK_TYPE_PONG) {
		if (!rtl931x_stack_talk_probe_validate(skb, &header, &probe))
			goto drop;
	} else if (!rtl931x_stack_talk_rpc_validate(stack, skb, &header,
						       &metadata, &rpc, NULL)) {
		goto drop;
	}
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
			    const struct rtl931x_talk_header *header,
			    const struct rtl931x_talk_probe *probe,
			    const struct rtl931x_talk_rx_metadata *metadata)
{
	u64 now = ktime_get_ns();
	u16 flags = be16_to_cpu(header->flags);

	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending && !stack->talk_pending_rpc &&
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
	u8 rpc_body[RTL931X_STACK_RPC_MAX_BODY_LEN];
	struct rtl931x_talk_rpc_header rpc;
	struct rtl931x_talk_header header;
	struct rtl931x_talk_probe probe;
	struct sk_buff *skb;
	int device;
	u16 flags;

	while ((skb = skb_dequeue(&stack->talk_rx_queue))) {
		if (!rtl931x_stack_talk_header_validate(stack, skb, &header,
							&metadata) ||
		    metadata.cpu_device_generation !=
			RTL931X_TALK_SKB_CB(skb)->cpu_device_generation)
			goto next;

		if (header.type == RTL931X_TALK_TYPE_RPC_REQUEST ||
		    header.type == RTL931X_TALK_TYPE_RPC_REPLY) {
			if (!rtl931x_stack_talk_rpc_validate(stack, skb, &header,
							       &metadata, &rpc,
							       rpc_body))
				goto next;
			if (header.type == RTL931X_TALK_TYPE_RPC_REPLY)
				rtl931x_stack_talk_rpc_complete(stack, &header,
							 &rpc, &metadata,
							 rpc_body);
			else
				rtl931x_stack_talk_rpc_serve(stack, &header, &rpc,
						      &metadata, rpc_body);
			goto next;
		}

		if (!rtl931x_stack_talk_probe_validate(skb, &header, &probe))
			goto next;

		flags = be16_to_cpu(header.flags);
		if (header.type == RTL931X_TALK_TYPE_PONG) {
			rtl931x_stack_talk_complete(stack, &header, &probe,
						    &metadata);
			goto next;
		}

		device = rtl931x_stack_cpu_device_get(stack->priv, NULL);
		if (device < 0)
			goto next;

		if (flags & RTL931X_TALK_F_ROUTED) {
			if (header.target_device != device)
				goto next;
			rtl931x_stack_talk_send(stack, RTL931X_TALK_TYPE_PONG,
						RTL931X_STACK_TALK_MODE_UNICAST,
						header.sender_device,
						metadata.ingress_port,
						be64_to_cpu(header.transaction),
						metadata.cpu_device_generation);
		} else {
			if (header.target_device != RTL931X_TALK_TARGET_ONE_HOP)
				goto next;
			rtl931x_stack_talk_send(stack, RTL931X_TALK_TYPE_PONG,
						RTL931X_STACK_TALK_MODE_ONE_HOP,
						metadata.ingress_port,
						metadata.ingress_port,
						be64_to_cpu(header.transaction),
						metadata.cpu_device_generation);
		}

next:
		kfree_skb(skb);
	}
}

static int rtl931x_stack_talk_rpc_result_errno(u16 result)
{
	switch (result) {
	case RTL931X_TALK_RPC_OK:
		return 0;
	case RTL931X_TALK_RPC_INVALID:
		return -EINVAL;
	case RTL931X_TALK_RPC_UNSUPPORTED:
		return -EOPNOTSUPP;
	case RTL931X_TALK_RPC_STALE_SESSION:
	case RTL931X_TALK_RPC_STALE_GENERATION:
		return -ESTALE;
	case RTL931X_TALK_RPC_NO_DEVICE:
		return -ENODEV;
	case RTL931X_TALK_RPC_IO:
		/* A received IO reply proves that this sequence was not committed. */
		return -EREMOTEIO;
	case RTL931X_TALK_RPC_DENIED:
		return -EPERM;
	case RTL931X_TALK_RPC_BUSY:
		return -EBUSY;
	default:
		return -EPROTO;
	}
}

static int
rtl931x_stack_talk_rpc_call(struct rtl931x_stack_context *stack, u16 opcode,
			    const void *request, size_t request_len,
			    void *reply, size_t *reply_len)
{
	const struct rtl931x_talk_rpc_operation *operation;
	unsigned int attempt;
	u64 transaction, peer_boot_nonce;
	u32 device_generation, generation;
	u16 result, received_len;
	long timeout;
	u8 local, peer, master, port;
	bool completed;
	int err;

	ASSERT_RTNL();

	operation = rtl931x_stack_talk_rpc_operation(opcode);
	if (!operation || request_len != operation->request_len ||
	    !reply_len || *reply_len < operation->reply_len)
		return -EINVAL;
	if (!reply && operation->reply_len)
		return -EINVAL;
	if (!stack->talk_conduit || !READ_ONCE(stack->enabled) ||
	    READ_ONCE(stack->state) != RTL931X_STACK_STATE_CONFIGURED ||
	    !READ_ONCE(stack->generation_valid) ||
	    !rtl931x_stack_talk_peer_get(stack, &peer_boot_nonce))
		return -ENOTCONN;

	err = rtl931x_stack_cpu_device_get(stack->priv, &device_generation);
	if (err < 0)
		return err;
	local = err;
	peer = READ_ONCE(stack->peer_id);
	master = READ_ONCE(stack->master_id);
	port = READ_ONCE(stack->port);
	generation = READ_ONCE(stack->generation);
	if (local != READ_ONCE(stack->member_id))
		return -ESTALE;

	err = mutex_lock_interruptible(&stack->talk_request_lock);
	if (err)
		return err;

	do {
		transaction = get_random_u64();
	} while (!transaction);
	reinit_completion(&stack->talk_reply_completion);

	spin_lock_bh(&stack->talk_reply_lock);
	stack->talk_pending = true;
	stack->talk_pending_rpc = true;
	stack->talk_pending_transaction = transaction;
	stack->talk_pending_opcode = opcode;
	stack->talk_pending_mode = RTL931X_STACK_TALK_MODE_UNICAST;
	stack->talk_pending_port = port;
	stack->talk_pending_peer = peer;
	stack->talk_pending_local = local;
	stack->talk_rpc_reply_len = 0;
	stack->talk_rpc_result = RTL931X_TALK_RPC_IO;
	spin_unlock_bh(&stack->talk_reply_lock);

	err = -ETIMEDOUT;
	for (attempt = 0; attempt < RTL931X_TALK_RPC_ATTEMPTS; attempt++) {
		err = rtl931x_stack_talk_rpc_xmit(stack,
					 RTL931X_TALK_TYPE_RPC_REQUEST,
					 peer, port, transaction, opcode,
					 RTL931X_TALK_RPC_OK,
					 peer_boot_nonce, generation, master,
					 request, request_len,
					 device_generation);
		if (err)
			break;

		timeout = msecs_to_jiffies(RTL931X_TALK_RPC_TIMEOUT_MS);
		timeout = wait_for_completion_interruptible_timeout(
			&stack->talk_reply_completion, timeout);
		if (timeout < 0) {
			err = timeout;
			break;
		}

		spin_lock_bh(&stack->talk_reply_lock);
		completed = !stack->talk_pending;
		spin_unlock_bh(&stack->talk_reply_lock);
		if (completed) {
			err = 0;
			break;
		}
		err = -ETIMEDOUT;
	}

	spin_lock_bh(&stack->talk_reply_lock);
	if (stack->talk_pending &&
	    stack->talk_pending_transaction == transaction) {
		stack->talk_pending = false;
	} else {
		err = 0;
		result = stack->talk_rpc_result;
		received_len = stack->talk_rpc_reply_len;
		if (result == RTL931X_TALK_RPC_OK &&
		    received_len == operation->reply_len) {
			if (received_len)
				memcpy(reply, stack->talk_rpc_reply, received_len);
			*reply_len = received_len;
		} else if (result == RTL931X_TALK_RPC_OK) {
			err = -EPROTO;
		} else {
			err = rtl931x_stack_talk_rpc_result_errno(result);
		}
	}
	stack->talk_pending_rpc = false;
	spin_unlock_bh(&stack->talk_reply_lock);

	mutex_unlock(&stack->talk_request_lock);
	return err;
}

static bool rtl931x_stack_mutation_uncertain(int err)
{
	return err == -EIO || err == -ETIMEDOUT || err == -ERESTARTSYS;
}

static int
rtl931x_stack_peer_mutation(struct rtl838x_switch_priv *priv, u16 opcode,
			    const void *request, size_t request_len)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	u8 input[RTL931X_STACK_RPC_MAX_BODY_LEN];
	u8 wire[RTL931X_STACK_RPC_MAX_BODY_LEN];
	size_t reply_len = 0;
	bool same = false;
	u32 sequence;
	int err;

	ASSERT_RTNL();
	if (request_len < sizeof(__be32) || request_len > sizeof(wire))
		return -EINVAL;
	memcpy(input, request, request_len);

	if (stack->peer_mutation_uncertain) {
		same = opcode == stack->peer_mutation_opcode &&
		       request_len == stack->peer_mutation_len &&
		       !memcmp(input + sizeof(__be32),
			       stack->peer_mutation_body + sizeof(__be32),
			       request_len - sizeof(__be32));
		memcpy(wire, stack->peer_mutation_body,
		       stack->peer_mutation_len);
		sequence = get_unaligned_be32(wire);
		err = rtl931x_stack_talk_rpc_call(stack,
						  stack->peer_mutation_opcode,
						  wire,
						  stack->peer_mutation_len,
						  NULL, &reply_len);
		if (err && err != -EREMOTEIO)
			return err;
		if (!err)
			stack->peer_mutation_sequence = sequence;
		stack->peer_mutation_uncertain = false;
		stack->peer_mutation_opcode = 0;
		stack->peer_mutation_len = 0;
		memset(stack->peer_mutation_body, 0,
		       sizeof(stack->peer_mutation_body));
		if (same)
			return err;
	}

	if (stack->peer_mutation_sequence == U32_MAX)
		return -EOVERFLOW;
	memcpy(wire, input, request_len);
	sequence = stack->peer_mutation_sequence + 1;
	put_unaligned_be32(sequence, wire);

	err = rtl931x_stack_talk_rpc_call(stack, opcode, wire, request_len,
						  NULL, &reply_len);
	if (!err) {
		stack->peer_mutation_sequence = sequence;
		stack->peer_mutation_uncertain = false;
		stack->peer_mutation_opcode = 0;
		stack->peer_mutation_len = 0;
		memset(stack->peer_mutation_body, 0,
		       sizeof(stack->peer_mutation_body));
	} else if (rtl931x_stack_mutation_uncertain(err)) {
		stack->peer_mutation_uncertain = true;
		stack->peer_mutation_opcode = opcode;
		stack->peer_mutation_len = request_len;
		memcpy(stack->peer_mutation_body, wire, request_len);
	}

	return err;
}

int rtl931x_stack_peer_set_delegated(struct rtl838x_switch_priv *priv,
				     bool delegated)
{
	struct rtl931x_talk_rpc_delegated_request request = {
		.enabled = delegated,
	};
	struct rtl931x_stack_context *stack = &priv->stack;

	ASSERT_RTNL();

	if (!READ_ONCE(stack->registered))
		return -ENODEV;
	if (READ_ONCE(stack->member_id) != READ_ONCE(stack->master_id))
		return -EPERM;

	return rtl931x_stack_peer_mutation(priv,
					   RTL931X_TALK_RPC_SET_DELEGATED,
					   &request, sizeof(request));
}

int rtl931x_stack_peer_set_bridge_port(struct rtl838x_switch_priv *priv,
				       u8 port, bool present)
{
	struct rtl931x_talk_rpc_bridge_port_request request = {
		.port = port,
		.present = present,
	};

	ASSERT_RTNL();
	if (port >= RTL931X_STACK_MAX_PORTS || port == priv->stack.port)
		return -EINVAL;
	return rtl931x_stack_peer_mutation(priv,
					   RTL931X_TALK_RPC_SET_BRIDGE_PORT,
					   &request, sizeof(request));
}

int rtl931x_stack_peer_set_port_vlan(struct rtl838x_switch_priv *priv, u8 port,
				     u16 vid, u16 flags, bool present)
{
	struct rtl931x_talk_rpc_port_vlan_request request = {
		.vid = cpu_to_be16(vid),
		.flags = cpu_to_be16(flags),
		.port = port,
		.present = present,
	};

	ASSERT_RTNL();
	if (port >= RTL931X_STACK_MAX_PORTS || port == priv->stack.port ||
	    !vid || vid >= MAX_VLANS || flags & ~RTL931X_STACK_VLAN_F_MASK ||
	    (!present && flags))
		return -EINVAL;
	return rtl931x_stack_peer_mutation(priv,
					   RTL931X_TALK_RPC_SET_PORT_VLAN,
					   &request, sizeof(request));
}

int rtl931x_stack_peer_get_switch_info(struct rtl838x_switch_priv *priv,
				       struct rtl931x_stack_peer_switch_info *info,
				       struct netlink_ext_ack *extack)
{
	struct rtl931x_talk_rpc_switch_info wire;
	struct rtl931x_stack_context *stack = &priv->stack;
	size_t reply_len = sizeof(wire);
	u64 valid_port_mask;
	u64 user_port_mask;
	u64 admin_up_mask;
	u64 carrier_mask;
	u64 delegated_port_mask;
	u32 capabilities;
	u16 max_body_len;
	int err;

	ASSERT_RTNL();

	err = rtl931x_stack_talk_rpc_call(stack,
					  RTL931X_TALK_RPC_GET_SWITCH_INFO,
					  NULL, 0, &wire, &reply_len);
	if (err) {
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to read peer switch information");
		return err;
	}

	valid_port_mask = GENMASK_ULL(RTL931X_STACK_MAX_PORTS - 1, 0);
	user_port_mask = be64_to_cpu(wire.user_port_mask);
	admin_up_mask = be64_to_cpu(wire.admin_up_mask);
	carrier_mask = be64_to_cpu(wire.carrier_mask);
	delegated_port_mask = be64_to_cpu(wire.delegated_port_mask);
	capabilities = be32_to_cpu(wire.capabilities);
	max_body_len = be16_to_cpu(wire.max_body_len);
	if (memchr_inv(wire.reserved, 0, sizeof(wire.reserved)) ||
	    wire.protocol_version != RTL931X_TALK_VERSION ||
	    wire.stack_port >= RTL931X_STACK_MAX_PORTS ||
	    wire.stack_port != stack->port ||
	    wire.cpu_port >= 64 || wire.cpu_port != priv->r->cpu_port ||
	    user_port_mask & ~valid_port_mask ||
	    admin_up_mask & ~user_port_mask ||
	    carrier_mask & ~user_port_mask ||
	    delegated_port_mask & ~user_port_mask ||
	    user_port_mask & BIT_ULL(wire.cpu_port) ||
	    user_port_mask & BIT_ULL(wire.stack_port) ||
	    wire.port_count != hweight64(user_port_mask) ||
	    capabilities & ~RTL931X_STACK_PEER_CAP_MASK ||
	    !(capabilities & RTL931X_STACK_PEER_CAP_GET_SWITCH) ||
	    ((capabilities & RTL931X_STACK_PEER_CAP_SET_DELEGATED) &&
	     max_body_len < sizeof(struct rtl931x_talk_rpc_delegated_request)) ||
	    ((capabilities & RTL931X_STACK_PEER_CAP_BRIDGE_VLAN) &&
	     (!(capabilities & RTL931X_STACK_PEER_CAP_SET_DELEGATED) ||
	      !(capabilities & RTL931X_STACK_PEER_CAP_GET_PORT_STATE) ||
	      max_body_len < sizeof(struct rtl931x_talk_rpc_port_vlan_request))) ||
	    ((capabilities & RTL931X_STACK_PEER_CAP_GET_PORT_STATE) &&
	     max_body_len < sizeof(struct rtl931x_talk_rpc_port_state)) ||
	    max_body_len < sizeof(struct rtl931x_talk_rpc_port_request) ||
	    max_body_len > RTL931X_STACK_RPC_MAX_BODY_LEN) {
		NL_SET_ERR_MSG_MOD(extack,
				   "peer returned invalid switch information");
		return -EPROTO;
	}

	info->user_port_mask = user_port_mask;
	info->admin_up_mask = admin_up_mask;
	info->carrier_mask = carrier_mask;
	info->delegated_port_mask = delegated_port_mask;
	info->capabilities = capabilities;
	info->max_body_len = max_body_len;
	info->port_count = wire.port_count;
	info->cpu_port = wire.cpu_port;
	info->stack_port = wire.stack_port;
	info->protocol_version = wire.protocol_version;

	return 0;
}

int rtl931x_stack_peer_get_port_info(struct rtl838x_switch_priv *priv, u8 port,
				     struct rtl931x_stack_peer_port_info *info,
				     struct netlink_ext_ack *extack)
{
	struct rtl931x_talk_rpc_port_request request = {
		.port = port,
	};
	struct rtl931x_talk_rpc_port_state state;
	size_t reply_len = sizeof(state);
	u32 mtu;
	int err;

	ASSERT_RTNL();
	if (!info)
		return -EINVAL;

	err = rtl931x_stack_talk_rpc_call(&priv->stack,
					  RTL931X_TALK_RPC_GET_PORT_STATE,
					  &request, sizeof(request), &state,
					  &reply_len);
	if (err) {
		NL_SET_ERR_MSG_MOD(extack, "failed to read peer port state");
		return err;
	}

	mtu = be32_to_cpu(state.mtu);
	if (state.port != port || state.reserved ||
	    memchr_inv(state.padding, 0, sizeof(state.padding)) ||
	    state.flags & ~(RTL931X_STACK_PEER_PORT_F_ADMIN_UP |
			    RTL931X_STACK_PEER_PORT_F_CARRIER) ||
	    !mtu || !is_valid_ether_addr(state.mac)) {
		NL_SET_ERR_MSG_MOD(extack, "peer returned invalid port state");
		return -EPROTO;
	}

	info->mtu = mtu;
	info->flags = state.flags;
	ether_addr_copy(info->mac, state.mac);

	return 0;
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
	[RTL931X_STACK_ATTR_REMOTE_PORT_MASK] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_PORT_COUNT] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_CPU_PORT] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_CAPABILITIES] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_PORT] =
		NLA_POLICY_MAX(NLA_U8, RTL931X_STACK_MAX_PORTS - 1),
	[RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_PORT_MTU] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK] = { .type = NLA_REJECT },
	[RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK] = {
		.type = NLA_REJECT,
	},
	[RTL931X_STACK_ATTR_REMOTE_PORT_MAC] = { .type = NLA_REJECT },
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
		rtl931x_stack_talk_peer_clear(stack);
		err = rtl931x_stack_undelegate_local_ports(stack, false);
		if (err)
			goto out_put;
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
	if (enabled)
		rtl931x_stack_talk_peer_clear(stack);
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
		rtl931x_stack_talk_peer_clear(stack);
		err = 0;
	}
	mutex_unlock(&target.priv->reg_mutex);
	if (!enabled)
		err = rtl931x_stack_undelegate_local_ports(stack, false);

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
	if (mode == RTL931X_STACK_TALK_MODE_ONE_HOP)
		rtl931x_stack_talk_peer_clear(stack);

	do {
		transaction = get_random_u64();
	} while (!transaction);
	started_ns = ktime_get_ns();
	reinit_completion(&stack->talk_reply_completion);

	spin_lock_bh(&stack->talk_reply_lock);
	stack->talk_pending = true;
	stack->talk_pending_rpc = false;
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
			stack->talk_peer_boot_nonce = reply.boot_nonce;
			stack->talk_peer_valid = true;
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

static int
rtl931x_stack_get_rpc_target(struct genl_info *info,
			     struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack;
	int err;

	err = rtl931x_stack_get_target(info, target);
	if (err)
		return err;

	stack = &target->priv->stack;
	if (!stack->enabled ||
	    stack->state != RTL931X_STACK_STATE_CONFIGURED ||
	    target->port != stack->port) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not a configured stack port");
		return -ENOTCONN;
	}
	if (!rtl931x_stack_talk_peer_get(stack, NULL)) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "stack peer has no verified Device Talk session");
		return -EHOSTUNREACH;
	}
	if (!netif_carrier_ok(target->dev)) {
		NL_SET_ERR_MSG_MOD(info->extack, "stack link is down");
		return -ENOLINK;
	}

	return 0;
}

static int
rtl931x_stack_put_rpc_session(struct sk_buff *skb,
			      struct rtl931x_stack_target *target)
{
	struct rtl931x_stack_context *stack = &target->priv->stack;
	u64 peer_boot_nonce;

	if (!rtl931x_stack_talk_peer_get(stack, &peer_boot_nonce))
		return -ENOTCONN;

	if (nla_put_u16(skb, RTL931X_STACK_ATTR_API_VERSION,
			RTL931X_STACK_GENL_VERSION) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_IFINDEX,
			target->dev->ifindex) ||
	    nla_put_u64_64bit(skb, RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE,
			      peer_boot_nonce, RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_REMOTE_MEMBER_ID,
		       stack->peer_id) ||
	    nla_put_u8(skb, RTL931X_STACK_ATTR_REMOTE_MASTER_ID,
		       stack->master_id) ||
	    nla_put_u32(skb, RTL931X_STACK_ATTR_REMOTE_GENERATION,
			stack->generation))
		return -EMSGSIZE;

	return 0;
}

static int rtl931x_stack_get_peer_switch(struct sk_buff *skb,
					 struct genl_info *info)
{
	struct rtl931x_stack_peer_switch_info peer_info;
	struct rtl931x_stack_target target = {};
	struct sk_buff *reply_skb;
	void *hdr;
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_rpc_target(info, &target);
	if (err)
		goto out_put;

	err = rtl931x_stack_peer_get_switch_info(target.priv, &peer_info,
						 info->extack);
	if (err)
		goto out_put;

	reply_skb = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!reply_skb) {
		err = -ENOMEM;
		goto out_put;
	}
	hdr = genlmsg_put_reply(reply_skb, info, &rtl931x_stack_family, 0,
				info->genlhdr->cmd);
	if (!hdr)
		goto nla_put_failure;
	err = rtl931x_stack_put_rpc_session(reply_skb, &target);
	if (err ||
	    nla_put_u64_64bit(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT_MASK,
			      peer_info.user_port_mask,
			      RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u64_64bit(reply_skb,
			      RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK,
			      peer_info.admin_up_mask,
			      RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u64_64bit(reply_skb,
			      RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK,
			      peer_info.carrier_mask,
			      RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u64_64bit(reply_skb,
			      RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK,
			      peer_info.delegated_port_mask,
			      RTL931X_STACK_ATTR_PAD) ||
	    nla_put_u8(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT_COUNT,
		       peer_info.port_count) ||
	    nla_put_u8(reply_skb, RTL931X_STACK_ATTR_REMOTE_CPU_PORT,
		       peer_info.cpu_port) ||
	    nla_put_u8(reply_skb, RTL931X_STACK_ATTR_REMOTE_STACK_PORT,
		       peer_info.stack_port) ||
	    nla_put_u32(reply_skb, RTL931X_STACK_ATTR_REMOTE_CAPABILITIES,
			peer_info.capabilities) ||
	    nla_put_u16(reply_skb, RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN,
			peer_info.max_body_len))
		goto nla_cancel;

	genlmsg_end(reply_skb, hdr);
	err = genlmsg_reply(reply_skb, info);
	goto out_put;

nla_cancel:
	genlmsg_cancel(reply_skb, hdr);
nla_put_failure:
	nlmsg_free(reply_skb);
	err = -EMSGSIZE;
out_put:
	if (target.dev)
		dev_put(target.dev);
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static int rtl931x_stack_get_peer_port(struct sk_buff *skb,
				       struct genl_info *info)
{
	struct rtl931x_stack_peer_port_info state;
	struct rtl931x_stack_target target = {};
	struct nlattr *port_attr;
	struct sk_buff *reply_skb;
	u8 port;
	void *hdr;
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;

	port_attr = info->attrs[RTL931X_STACK_ATTR_REMOTE_PORT];
	if (!port_attr) {
		NL_SET_ERR_MSG_MOD(info->extack, "remote port is required");
		return -EINVAL;
	}
	port = nla_get_u8(port_attr);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_rpc_target(info, &target);
	if (err)
		goto out_put;

	err = rtl931x_stack_peer_get_port_info(target.priv, port, &state,
					       info->extack);
	if (err)
		goto out_put;

	reply_skb = genlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!reply_skb) {
		err = -ENOMEM;
		goto out_put;
	}
	hdr = genlmsg_put_reply(reply_skb, info, &rtl931x_stack_family, 0,
				info->genlhdr->cmd);
	if (!hdr)
		goto nla_put_failure;
	err = rtl931x_stack_put_rpc_session(reply_skb, &target);
	if (err ||
	    nla_put_u8(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT,
		       port) ||
	    nla_put_u32(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS,
			state.flags) ||
	    nla_put_u32(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT_MTU,
			state.mtu) ||
	    nla_put(reply_skb, RTL931X_STACK_ATTR_REMOTE_PORT_MAC,
		    ETH_ALEN, state.mac))
		goto nla_cancel;

	genlmsg_end(reply_skb, hdr);
	err = genlmsg_reply(reply_skb, info);
	goto out_put;

nla_cancel:
	genlmsg_cancel(reply_skb, hdr);
nla_put_failure:
	nlmsg_free(reply_skb);
	err = -EMSGSIZE;
out_put:
	if (target.dev)
		dev_put(target.dev);
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
	return err;
}

static int rtl931x_stack_set_peer_port_netdevs(struct sk_buff *skb,
					       struct genl_info *info)
{
	struct rtl931x_stack_target target = {};
	struct rtl931x_stack_context *stack;
	struct nlattr **attrs = info->attrs;
	bool enabled;
	int err;

	err = rtl931x_stack_check_version(info);
	if (err)
		return err;
	if (!attrs[RTL931X_STACK_ATTR_ENABLED]) {
		NL_SET_ERR_MSG_MOD(info->extack, "enabled is required");
		return -EINVAL;
	}
	enabled = nla_get_u8(attrs[RTL931X_STACK_ATTR_ENABLED]);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	err = rtl931x_stack_get_target(info, &target);
	if (err)
		goto out_unlock;

	stack = &target.priv->stack;
	if (!stack->enabled || !stack->saved_valid ||
	    stack->state != RTL931X_STACK_STATE_CONFIGURED) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "peer ports require an active stack");
		err = -ENOTCONN;
		goto out_put;
	}
	if (target.port != stack->port) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "interface is not the active stack port");
		err = -EINVAL;
		goto out_put;
	}
	if (stack->member_id != stack->master_id) {
		NL_SET_ERR_MSG_MOD(info->extack,
				   "only the configured stack leader owns peer ports");
		err = -EPERM;
		goto out_put;
	}

	err = rtl931x_stack_reps_set(target.priv, enabled, info->extack);

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

		if (stack->enabled &&
		    (member != stack->member_id || peer != stack->peer_id ||
		     master != stack->master_id)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "disable stacking before changing member roles");
			err = -EOPNOTSUPP;
			goto out_put;
		}
		if (flags != stack->flags &&
		    (stack->reps || stack->delegated_port_mask ||
		     stack->delegated_host_count)) {
			NL_SET_ERR_MSG_MOD(info->extack,
					   "restore peer port ownership before changing stack flags");
			err = -EBUSY;
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
		if (!stack->enabled &&
		    !rtl931x_stack_talk_peer_get(stack, NULL)) {
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
			rtl931x_stack_talk_peer_clear(stack);
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

		if (!enabled)
			rtl931x_stack_talk_peer_clear(stack);
		if (!enabled) {
			err = rtl931x_stack_undelegate_local_ports(stack, false);
			if (err)
				goto out_put;
		}
		err = rtl931x_stack_put_reply(info, &target);
		goto out_put;
	}
	if (!enabled && stack->reps) {
		err = rtl931x_stack_reps_set(target.priv, false, info->extack);
		if (err)
			goto out_put;
	}
	if (!enabled && (stack->delegated_port_mask ||
			 stack->delegated_host_count)) {
		err = rtl931x_stack_undelegate_local_ports(stack, false);
		if (err)
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
	if ((!err && !enabled) ||
	    stack->state == RTL931X_STACK_STATE_DISABLED ||
	    stack->state == RTL931X_STACK_STATE_ERROR)
		rtl931x_stack_talk_peer_clear(stack);
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
	{
		.cmd = RTL931X_STACK_CMD_GET_PEER_SWITCH,
		.doit = rtl931x_stack_get_peer_switch,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = RTL931X_STACK_CMD_GET_PEER_PORT,
		.doit = rtl931x_stack_get_peer_port,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS,
		.doit = rtl931x_stack_set_peer_port_netdevs,
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

static void rtl931x_stack_reps_recovery_work(struct work_struct *work)
{
	struct rtl931x_stack_context *stack = container_of(to_delayed_work(work),
		struct rtl931x_stack_context, reps_recovery_work);
	unsigned long delay;
	u8 attempt;
	int err;

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	if (!stack->registered || !stack->reps_desired ||
	    !stack->reps_recovery_pending || !stack->enabled ||
	    !stack->saved_valid ||
	    stack->state != RTL931X_STACK_STATE_CONFIGURED ||
	    stack->member_id != stack->master_id || !stack->reps ||
	    !stack->fabric_link_up)
		goto out_unlock;

	err = rtl931x_stack_reps_set(stack->priv, true, NULL);
	if (err) {
		dev_err_ratelimited(stack->priv->dev,
			"failed to recover peer ports after stack link-up: %pe\n",
			ERR_PTR(err));
		if ((err == -ENOLINK || err == -EBUSY || err == -EIO ||
		     err == -EREMOTEIO || err == -ETIMEDOUT ||
		     err == -ERESTARTSYS) && stack->reps_recovery_pending &&
		    stack->fabric_link_up) {
			attempt = READ_ONCE(stack->reps_recovery_attempts);
			if (attempt < 8) {
				WRITE_ONCE(stack->reps_recovery_attempts,
					   attempt + 1);
				delay = msecs_to_jiffies(min_t(unsigned int,
						100U << attempt, 5000U));
				mod_delayed_work(system_wq,
						 &stack->reps_recovery_work,
						 delay);
			}
		}
	}

out_unlock:
	mutex_unlock(&rtl931x_stack_lock);
	rtnl_unlock();
}

void rtl931x_stack_register(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct dsa_port *dp;

	INIT_LIST_HEAD(&stack->list);
	INIT_LIST_HEAD(&stack->peer_port_vlans);
	INIT_LIST_HEAD(&stack->peer_fabric_vlans);
	INIT_LIST_HEAD(&stack->local_fabric_vlans);
	stack->state = RTL931X_STACK_STATE_DISABLED;
	stack->priv = priv;
	mutex_init(&stack->talk_request_lock);
	spin_lock_init(&stack->talk_reply_lock);
	init_completion(&stack->talk_reply_completion);
	stack->talk_peer_boot_nonce = 0;
	stack->talk_peer_valid = false;
	stack->talk_pending = false;
	stack->talk_pending_rpc = false;
	stack->delegated_port_mask = 0;
	stack->peer_bridge_port_mask = 0;
	stack->bridge_saved_port_mask = 0;
	stack->delegated_host_count = 0;
	stack->bridge_fabric_users = 0;
	stack->reps = NULL;
	stack->fabric_link_up = false;
	stack->reps_desired = false;
	stack->reps_recovery_pending = false;
	atomic_set(&stack->fabric_link_epoch, 0);
	skb_queue_head_init(&stack->talk_rx_queue);
	INIT_WORK(&stack->talk_rx_work, rtl931x_stack_talk_work);
	INIT_DELAYED_WORK(&stack->reps_recovery_work,
			  rtl931x_stack_reps_recovery_work);
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

	disable_delayed_work_sync(&stack->reps_recovery_work);

	rtnl_lock();
	mutex_lock(&rtl931x_stack_lock);
	if (!stack->registered)
		goto out_unlock;
	rtl931x_stack_reps_unregister(priv);
	err = rtl931x_stack_undelegate_local_ports(stack, true);
	if (err)
		dev_err(priv->dev,
			"failed to restore follower port ownership during removal\n");

	if (stack->saved_valid && priv->ports[stack->port].dp)
		stack_port = priv->ports[stack->port].dp->user;
	else if (stack->talk_armed &&
		 priv->ports[stack->talk_armed_port].dp)
		stack_port = priv->ports[stack->talk_armed_port].dp->user;
	if (stack_port && netif_running(stack_port))
		dev_close(stack_port);
	rtl931x_stack_talk_peer_clear(stack);

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
	int err;

	err = rtl931x_stack_reps_init();
	if (err)
		return err;
	err = genl_register_family(&rtl931x_stack_family);
	if (err)
		rtl931x_stack_reps_exit();

	return err;
}

void rtl931x_stack_exit(void)
{
	WARN_ON_ONCE(!list_empty(&rtl931x_stack_list));
	genl_unregister_family(&rtl931x_stack_family);
	rtl931x_stack_reps_exit();
}
