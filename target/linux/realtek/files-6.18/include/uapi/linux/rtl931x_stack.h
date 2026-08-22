/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_RTL931X_STACK_H
#define _UAPI_LINUX_RTL931X_STACK_H

#define RTL931X_STACK_GENL_NAME		"rtl931x_stack"
#define RTL931X_STACK_GENL_VERSION	5

/*
 * Version 5 supports the fixed two-member, one-link topology only. Both
 * members must reserve the same physical port number for the fabric link.
 * Peer port addresses are reported by GET_PEER_PORT, not GET_PEER_SWITCH.
 * GET reports local delegation and leader peer-netdevice health so an early
 * boot manager can distinguish configured from safe-to-release-to-userspace.
 */

enum rtl931x_stack_command {
	RTL931X_STACK_CMD_UNSPEC,
	RTL931X_STACK_CMD_GET,
	/* Configure the fixed two-member, one-link bring-up topology. */
	RTL931X_STACK_CMD_SET_TWO_MEMBER,
	/* Provisionally reserve a physical port for CPU Device Talk. */
	RTL931X_STACK_CMD_SET_DEVICE_TALK_PORT,
	/* Probe the directly connected peer over CPU Device Talk. */
	RTL931X_STACK_CMD_PROBE_PEER,
	/* Read the configured peer's switch inventory. */
	RTL931X_STACK_CMD_GET_PEER_SWITCH,
	/* Read one configured peer user port's state. */
	RTL931X_STACK_CMD_GET_PEER_PORT,
	/* Create or remove the leader-owned peer user-port netdevices. */
	RTL931X_STACK_CMD_SET_PEER_PORT_NETDEVS,

	__RTL931X_STACK_CMD_MAX,
};

#define RTL931X_STACK_CMD_MAX	(__RTL931X_STACK_CMD_MAX - 1)

enum rtl931x_stack_attribute {
	RTL931X_STACK_ATTR_UNSPEC,
	RTL931X_STACK_ATTR_API_VERSION,	/* u16 */
	RTL931X_STACK_ATTR_IFINDEX,	/* u32 */
	RTL931X_STACK_ATTR_ENABLED,	/* u8 */
	RTL931X_STACK_ATTR_MEMBER_ID,	/* u8 */
	RTL931X_STACK_ATTR_PEER_ID,	/* u8 */
	RTL931X_STACK_ATTR_MASTER_ID,	/* u8 */
	RTL931X_STACK_ATTR_FLAGS,	/* u32 */
	RTL931X_STACK_ATTR_GENERATION,	/* u32, monotonic and non-wrapping */
	RTL931X_STACK_ATTR_STATE,	/* u8, reply only */
	RTL931X_STACK_ATTR_LINK_UP,	/* u8, reply only */
	RTL931X_STACK_ATTR_TALK_MODE,	/* u8, reply only */
	RTL931X_STACK_ATTR_TALK_TRANSACTION, /* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_BOOT_NONCE, /* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_MEMBER_ID, /* u8, reply only */
	RTL931X_STACK_ATTR_REMOTE_MASTER_ID, /* u8, reply only */
	RTL931X_STACK_ATTR_REMOTE_STACK_PORT, /* u8, reply only */
	RTL931X_STACK_ATTR_REMOTE_STATUS,	/* u32, reply only */
	RTL931X_STACK_ATTR_REMOTE_GENERATION, /* u32, reply only */
	RTL931X_STACK_ATTR_ROUND_TRIP_US,	/* u32, reply only */
	RTL931X_STACK_ATTR_PAD,
	RTL931X_STACK_ATTR_REMOTE_PORT_MASK,	/* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_PORT_COUNT,	/* u8, reply only */
	RTL931X_STACK_ATTR_REMOTE_CPU_PORT,	/* u8, reply only */
	RTL931X_STACK_ATTR_REMOTE_CAPABILITIES,	/* u32, reply only */
	RTL931X_STACK_ATTR_REMOTE_MAX_BODY_LEN,	/* u16, reply only */
	RTL931X_STACK_ATTR_REMOTE_PORT,	/* u8, request and reply */
	RTL931X_STACK_ATTR_REMOTE_PORT_FLAGS,	/* u32, reply only */
	RTL931X_STACK_ATTR_REMOTE_PORT_MTU,	/* u32, reply only */
	RTL931X_STACK_ATTR_REMOTE_ADMIN_UP_MASK,	/* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_CARRIER_MASK,	/* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_DELEGATED_PORT_MASK, /* u64, reply only */
	RTL931X_STACK_ATTR_REMOTE_PORT_MAC,	/* binary ETH_ALEN, peer-port reply */
	RTL931X_STACK_ATTR_LOCAL_PORT_MASK,	/* u64, GET reply only */
	RTL931X_STACK_ATTR_LOCAL_DELEGATED_PORT_MASK, /* u64, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_DESIRED,	/* u8, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_ACTIVE,	/* u8, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_PUBLISHED, /* u8, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_FENCED,	/* u8, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_RECOVERING, /* u8, GET reply only */
	RTL931X_STACK_ATTR_PEER_NETDEVS_LAST_ERROR, /* u32 errno, GET reply only */

	__RTL931X_STACK_ATTR_MAX,
};

#define RTL931X_STACK_ATTR_MAX	(__RTL931X_STACK_ATTR_MAX - 1)

enum rtl931x_stack_state {
	RTL931X_STACK_STATE_DISABLED,
	RTL931X_STACK_STATE_CONFIGURED,
	RTL931X_STACK_STATE_ERROR,
	/* The selected port is provisionally a stack port for Device Talk. */
	RTL931X_STACK_STATE_ARMED,
	/* A bidirectional one-hop Device Talk probe completed. */
	RTL931X_STACK_STATE_PEER_VERIFIED,
};

enum rtl931x_stack_flags {
	RTL931X_STACK_F_AUTO_LEARN	= 1U << 0,
	RTL931X_STACK_F_DROP_MY_DEV	= 1U << 1,
};

#define RTL931X_STACK_F_MASK	(RTL931X_STACK_F_AUTO_LEARN | \
				 RTL931X_STACK_F_DROP_MY_DEV)

enum rtl931x_stack_talk_mode {
	RTL931X_STACK_TALK_MODE_ONE_HOP,
	RTL931X_STACK_TALK_MODE_UNICAST,
};

enum rtl931x_stack_talk_status {
	RTL931X_STACK_TALK_S_ID_VALID		= 1U << 0,
	RTL931X_STACK_TALK_S_MASTER		= 1U << 1,
	RTL931X_STACK_TALK_S_CONFIGURED		= 1U << 2,
	RTL931X_STACK_TALK_S_ROUTE_READY	= 1U << 3,
	RTL931X_STACK_TALK_S_LINK_UP		= 1U << 4,
};

#define RTL931X_STACK_TALK_S_MASK	(RTL931X_STACK_TALK_S_ID_VALID | \
					 RTL931X_STACK_TALK_S_MASTER | \
					 RTL931X_STACK_TALK_S_CONFIGURED | \
					 RTL931X_STACK_TALK_S_ROUTE_READY | \
					 RTL931X_STACK_TALK_S_LINK_UP)

enum rtl931x_stack_peer_capability {
	RTL931X_STACK_PEER_CAP_GET_SWITCH	= 1U << 0,
	RTL931X_STACK_PEER_CAP_GET_PORT_STATE	= 1U << 1,
	RTL931X_STACK_PEER_CAP_SET_DELEGATED	= 1U << 2,
	RTL931X_STACK_PEER_CAP_BRIDGE_VLAN	= 1U << 3,
	RTL931X_STACK_PEER_CAP_PORT_STATUS_EVENT	= 1U << 4,
};

#define RTL931X_STACK_PEER_CAP_MASK \
	(RTL931X_STACK_PEER_CAP_GET_SWITCH | \
	 RTL931X_STACK_PEER_CAP_GET_PORT_STATE | \
	 RTL931X_STACK_PEER_CAP_SET_DELEGATED | \
	 RTL931X_STACK_PEER_CAP_BRIDGE_VLAN | \
	 RTL931X_STACK_PEER_CAP_PORT_STATUS_EVENT)

enum rtl931x_stack_peer_port_flags {
	RTL931X_STACK_PEER_PORT_F_ADMIN_UP	= 1U << 0,
	RTL931X_STACK_PEER_PORT_F_CARRIER	= 1U << 1,
};

#endif /* _UAPI_LINUX_RTL931X_STACK_H */
