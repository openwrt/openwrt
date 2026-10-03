/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _RTL838X_H
#define _RTL838X_H

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <net/dsa.h>

#include <linux/soc/realtek/otto_table.h>

#include "l2.h"
#include "l3_limits.h"
#include "lag.h"
#include "mirror.h"
#include "pie.h"
#include "stats.h"
#include "stp.h"
#include "vlan.h"

/* special port action controls */
/* values:
 *      0 = FORWARD (default)
 *      1 = DROP
 *      2 = TRAP2CPU
 *      3 = FLOOD IN ALL PORT
 *
 *      Register encoding.
 *      offset = CTRL + (port >> 4) << 2
 *      value/mask = 3 << ((port & 0xF) << 1)
 */

typedef enum {
	BPDU = 0,
	PTP,
	PTP_UDP,
	PTP_ETH2,
	LLDP,
	EAPOL,
	GRATARP,
} rma_ctrl_t;

typedef enum {
	FORWARD = 0,
	DROP,
	TRAP2CPU,
	FLOODALL,
	TRAP2MASTERCPU,
	COPY2CPU,
} action_type_t;

struct rtldsa_93xx_lag_entry {
	u32 trk_port0:6;
	u32 trk_dev0:4;
	u32 trk_port1:6;
	u32 trk_dev1:4;
	u32 trk_port2:6;
	u32 trk_dev2:4;
	u32 trk_port3:6;
	u32 trk_dev3:4;
	u32 trk_port4:6;
	u32 trk_dev4:4;
	u32 trk_port5:6;
	u32 trk_dev5:4;
	u32 trk_port6:6;
	u32 trk_dev6:4;
	u32 trk_port7:6;
	u32 trk_dev7:4;
	u32 sep_kwn_mc_en:1;
	union {
		// for rtl930x
		u32 sep_dlf_bcast_en:1;
		// for rtl931x
		u32 sep_flood_en:1;
	} flood_dlf_bcast;
	u32 ip6_hash_mask_idx:1;
	u32 ip4_hash_mask_idx:1;
	u32 l2_hash_mask_idx:1;
	u32 num_tx_candi:4;
};

struct rtldsa_port {
	bool enable:1;
	bool phy:1;
	bool isolated:1;
	bool rate_police_egress:1;
	bool rate_police_ingress:1;
	unsigned long cached_flags;
	u64 pm;
	u16 pvid;
	bool eee_enabled;
	bool has_pcs;
	int led_set;
	enum rtldsa_flood_type flood_type;
	int leds_on_this_port;
	struct rtldsa_counter_state counters;
	const struct dsa_port *dp;
};

enum l2_entry_type {
	L2_INVALID = 0,
	L2_UNICAST = 1,
	L2_MULTICAST = 2,
	IP4_MULTICAST = 3,
	IP6_MULTICAST = 4,
};

/* What keeps one L2 unicast hash table entry alive. Only the hash table is
 * covered: the CAM is a table of its own with its own numbering, and a next
 * hop never lands there, so nothing shares a CAM entry.
 */
struct rtldsa_l2_uc {
	bool fdb_ref:1;		/* written by an fdb handler */
	u8 l3_refcount:7;	/* routes forwarding through it */
};

struct rtl838x_l2_entry {
	u8 mac[6];
	u16 vid;
	u16 rvid;
	u8 port;
	enum l2_entry_type type;
	bool valid:1;
	bool is_static:1;
	bool is_ip_mc:1;
	bool is_ipv6_mc:1;
	bool block_da:1;
	bool block_sa:1;
	bool suspended:1;
	bool next_hop:1;
	bool is_trunk:1;
	bool nh_vlan_target:1;  /* Only RTL83xx: VLAN used for next hop */
	int age;
	u8 trunk;
	u8 stack_dev;
	u16 mc_portmask_index;
	u32 mc_gip;
	u32 mc_sip;
	u16 mc_mac_index;
	u16 nh_route_id;

	/* The following is only valid on RTL931x */
	bool is_open_flow:1;
	bool is_pe_forward:1;
	bool is_local_forward:1;
	bool is_remote_forward:1;
	bool is_l2_tunnel:1;
	bool hash_msb:1;
	int l2_tunnel_id;
	int l2_tunnel_list_id;
};

enum fwd_rule_action {
	FWD_RULE_ACTION_NONE = 0,
	FWD_RULE_ACTION_FWD = 1,
};

enum pie_phase {
	PHASE_VACL = 0,
	PHASE_IACL = 1,
};

/* Intermediate representation of a  Packet Inspection Engine Rule
 * as suggested by the Kernel's tc flower offload subsystem
 * Field meaning is universal across SoC families, but data content is specific
 * to SoC family (e.g. because of different port ranges)
 */
struct pie_rule {
	int id;
	enum pie_phase phase;	/* Phase in which this template is applied */
	int packet_cntr;	/* ID of a packet counter assigned to this rule */
	int octet_cntr;		/* ID of a byte counter assigned to this rule */
	u32 last_packet_cnt;
	u64 last_octet_cnt;

	/* The following are requirements for the pie template */
	bool is_egress;
	bool is_ipv6;		/* This is a rule with IPv6 fields */

	/* Fixed fields that are always matched against on RTL8380 */
	u8 spmmask_fix;
	u8 spn;			/* Source port number */
	bool stacking_port;	/* Source port is stacking port */
	bool mgnt_vlan;		/* Packet arrived on management VLAN */
	bool dmac_hit_sw;	/* The packet's destination MAC matches one of the device's */
	bool content_too_deep;	/* The content of the packet cannot be parsed: too many layers */
	bool not_first_frag;	/* Not the first IP fragment */
	u8 frame_type_l4;	/* 0: UDP, 1: TCP, 2: ICMP/ICMPv6, 3: IGMP */
	u8 frame_type;		/* 0: ARP, 1: L2 only, 2: IPv4, 3: IPv6 */
	bool otag_fmt;		/* 0: outer tag packet, 1: outer priority tag or untagged */
	bool itag_fmt;		/* 0: inner tag packet, 1: inner priority tag or untagged */
	bool otag_exist;	/* packet with outer tag */
	bool itag_exist;	/* packet with inner tag */
	bool frame_type_l2;	/* 0: Ethernet, 1: LLC_SNAP, 2: LLC_Other, 3: Reserved */
	bool igr_normal_port;	/* Ingress port is not cpu or stacking port */
	u8 tid;			/* The template ID defining the what the templated fields mean */

	/* Masks for the fields that are always matched against on RTL8380 */
	u8 spmmask_fix_m;
	u8 spn_m;
	bool stacking_port_m;
	bool mgnt_vlan_m;
	bool dmac_hit_sw_m;
	bool content_too_deep_m;
	bool not_first_frag_m;
	u8 frame_type_l4_m;
	u8 frame_type_m;
	bool otag_fmt_m;
	bool itag_fmt_m;
	bool otag_exist_m;
	bool itag_exist_m;
	bool frame_type_l2_m;
	bool igr_normal_port_m;
	u8 tid_m;

	/* Logical operations between rules, special rules for rule numbers apply */
	bool valid;
	bool cond_not;		/* Matches when conditions not match */
	bool cond_and1;		/* And this rule 2n with the next rule 2n+1 in same block */
	bool cond_and2;		/* And this rule m in block 2n with rule m in block 2n+1 */
	bool ivalid;

	/* Actions to be performed */
	bool drop;		/* Drop the packet */
	bool fwd_sel;		/* Forward packet: to port, portmask, dest route, next rule, drop */
	bool ovid_sel;		/* So something to outer vlan-id: shift, re-assign */
	bool ivid_sel;		/* Do something to inner vlan-id: shift, re-assign */
	bool flt_sel;		/* Filter the packet when sending to certain ports */
	bool log_sel;		/* Log the packet in one of the LOG-table counters */
	bool rmk_sel;		/* Re-mark the packet, i.e. change the priority-tag */
	bool meter_sel;		/* Meter the packet, i.e. limit rate of this type of packet */
	bool tagst_sel;		/* Change the ergress tag */
	bool mir_sel;		/* Mirror the packet to a Link Aggregation Group */
	bool nopri_sel;		/* Change the normal priority */
	bool cpupri_sel;	/* Change the CPU priority */
	bool otpid_sel;		/* Change Outer Tag Protocol Identifier (802.1q) */
	bool itpid_sel;		/* Change Inner Tag Protocol Identifier (802.1q) */
	bool shaper_sel;	/* Apply traffic shaper */
	bool mpls_sel;		/* MPLS actions */
	bool bypass_sel;	/* Bypass actions */
	bool fwd_sa_lrn;	/* Learn the source address when forwarding */
	bool fwd_mod_to_cpu;	/* Forward the modified VLAN tag format to CPU-port */

	/* Fields used in predefined templates 0-2 on RTL8380 / 90 / 9300 */
	u64 spm;		/* Source Port Matrix */
	u16 otag;		/* Outer VLAN-ID */
	u8 smac[ETH_ALEN];	/* Source MAC address */
	u8 dmac[ETH_ALEN];	/* Destination MAC address */
	u16 ethertype;		/* Ethernet frame type field in ethernet header */
	u16 itag;		/* Inner VLAN-ID */
	u16 field_range_check;
	u32 sip;		/* Source IP */
	struct in6_addr sip6;	/* IPv6 Source IP */
	u32 dip;		/* Destination IP */
	struct in6_addr dip6;	/* IPv6 Destination IP */
	u16 tos_proto;		/* IPv4: TOS + Protocol fields, IPv6: Traffic class + next header */
	u16 sport;		/* TCP/UDP source port */
	u16 dport;		/* TCP/UDP destination port */
	u16 icmp_igmp;
	u16 tcp_info;
	u16 dsap_ssap;		/* Destination / Source Service Access Point bytes (802.3) */

	u64 spm_m;
	u16 otag_m;
	u8 smac_m[ETH_ALEN];
	u8 dmac_m[ETH_ALEN];
	u16 ethertype_m;
	u16 itag_m;
	u16 field_range_check_m;
	u32 sip_m;
	struct in6_addr sip6_m;	/* IPv6 Source IP mask */
	u32 dip_m;
	struct in6_addr dip6_m;	/* IPv6 Destination IP mask */
	u16 tos_proto_m;
	u16 sport_m;
	u16 dport_m;
	u16 icmp_igmp_m;
	u16 tcp_info_m;
	u16 dsap_ssap_m;

	/* Data associated with actions */
	u8 fwd_act;		/* Type of forwarding action */
				/* 0: permit, 1: drop, 2: copy to port id, 4: copy to portmask */
				/* 4: redirect to portid, 5: redirect to portmask */
				/* 6: route, 7: vlan leaky (only 8380) */
	u16 fwd_data;		/* Additional data for forwarding action, e.g. destination port */
	u8 ovid_act;
	u16 ovid_data;		/* Outer VLAN ID */
	u8 ivid_act;
	u16 ivid_data;		/* Inner VLAN ID */
	u16 flt_data;		/* Filtering data */
	u16 log_data;		/* ID of packet or octet counter in LOG table, on RTL93xx */
				/* unnecessary since PIE-Rule-ID == LOG-counter-ID */
	bool log_octets;
	u8 mpls_act;		/* MPLS action type */
	u16 mpls_lib_idx;	/* MPLS action data */

	u16 rmk_data;		/* Data for remarking */
	u16 meter_data;		/* ID of meter for bandwidth control */
	u16 tagst_data;
	u16 mir_data;
	u16 nopri_data;
	u16 cpupri_data;
	u16 otpid_data;
	u16 itpid_data;
	u16 shaper_data;

	/* Bypass actions, ignored on RTL8380 */
	bool bypass_all;	/* Not clear */
	bool bypass_igr_stp;	/* Bypass Ingress STP state */
	bool bypass_ibc_sc;	/* Bypass Ingress Bandwidth Control and Storm Control */
};

struct rtl838x_switch_priv;

struct rtldsa_config {
	const struct dsa_switch_ops *switch_ops;
	const struct phylink_mac_ops *phylink_mac_ops;
	void (*mask_port_reg_be)(u64 clear, u64 set, int reg);
	void (*set_port_reg_be)(u64 set, int reg);
	u64 (*get_port_reg_be)(int reg);
	void (*mask_port_reg_le)(u64 clear, u64 set, int reg);
	void (*set_port_reg_le)(u64 set, int reg);
	u64 (*get_port_reg_le)(int reg);
	int stat_port_rst;
	int stat_rst;
	void (*stat_init)(struct rtl838x_switch_priv *priv);
	int stat_port_std_mib;
	int stat_port_prv_mib;
	const struct rtldsa_mib_desc *mib_desc;
	u64 (*stat_port_table_read)(int port, unsigned int mib_size, unsigned int offset, bool is_pvt);
	void (*stat_counters_lock)(struct rtl838x_switch_priv *priv, int port);
	void (*stat_counters_unlock)(struct rtl838x_switch_priv *priv, int port);

	/**
	 * @stat_update_counters_atomically: When set, the SoC family allows atomically retrieving
	 * of statistic counters using this function.  This function must not require "might_sleep"
	 * code.
	 *
	 * Any SoC family which requires stat_port_table_read must use the table
	 * rtldsa_counters_(un)lock_table helpers. They are using a mutex for locking. The counters
	 * update is therefore not atomic.
	 */
	void (*stat_update_counters_atomically)(struct rtl838x_switch_priv *priv, int port);
	unsigned long stat_counter_poll_interval;
	int (*port_iso_ctrl)(int p);
	void (*traffic_enable)(int source, int dest);
	void (*traffic_disable)(int source, int dest);
	void (*traffic_set)(int source, u64 dest_matrix);
	int l2_ctrl_0;
	int l2_ctrl_1;
	bool high_res_l2_age;
	u32 self_mac_trap_ctrl;
	u32 l2_port_aging_out;
	int l2_tbl_flush_ctrl;
	int isr_glb_src;
	int isr_port_link_sts_chg;
	int imr_port_link_sts_chg;
	int imr_glb;
	int n_counters;
	int n_pie_blocks;
	/* PIE rule ID doubles as the LOG-table counter ID (RTL930x); also
	 * gates ingress cls_flower offload, which relies on that property.
	 */
	bool pie_rule_id_is_log_counter;
	u8 num_lag_ids;
	u8 cpu_port;
	u8 port_ignore;
	u8 l2_bucket_size;
	u16 n_mst;
	u32 fib_entries;
	enum otto_table_id l2_uc_tbl;
	enum otto_table_id l2_cam_tbl;
	int trk_ctrl;
	int trk_hash_ctrl;
	void (*stp_init)(void);
	void (*vlan_tables_read)(u32 vlan, struct rtldsa_vlan_info *info);
	void (*vlan_set_tagged)(u32 vlan, struct rtldsa_vlan_info *info);
	void (*vlan_set_untagged)(u32 vlan, u64 portmask);
	int (*vlan_profile_get)(int index, struct rtldsa_vlan_profile *profile);
	void (*vlan_profile_dump)(struct rtl838x_switch_priv *priv, int index);
	void (*vlan_profile_setup)(int profile);
	void (*vlan_port_pvidmode_set)(int port, enum pbvlan_type type, enum pbvlan_mode mode);
	void (*vlan_port_pvid_set)(int port, enum pbvlan_type type, int pvid);
	void (*vlan_port_keep_tag_set)(int port, bool keep_outer, bool keep_inner);
	int (*fast_age)(struct rtl838x_switch_priv *priv, int port, int vid);
	void (*set_vlan_igr_filter)(int port, enum igr_filter state);
	void (*set_vlan_egr_filter)(int port, enum egr_filter state);
	void (*enable_learning)(int port, bool enable);
	void (*enable_l2_new_sa_fwd)(int port, enum rtldsa_flood_type flood_type);
	void (*enable_flood)(int port, enum rtldsa_flood_type flood_type);
	void (*enable_mcast_flood)(int port, bool enable);
	void (*enable_bcast_flood)(int port, bool enable);
	void (*set_static_move_action)(int port, bool forward);
	int (*stp_get)(struct rtl838x_switch_priv *priv, u16 msti, int port);
	void (*stp_set)(struct rtl838x_switch_priv *priv, u16 msti, int port, int state);
	int mac_link_sts;
	u32 mac_force_mode_mask;
	int  (*mac_force_mode_ctrl)(int port);
	int  (*mac_port_ctrl)(int port);

	/**
	 * @mac_capabilities: supported MAC capabilities
	 */
	unsigned long mac_capabilities;

	/**
	 * @mac_max_len_reg: Return the switch register holding the MAC maximum
	 * accepted L2 frame length of user port @p. Families whose limit is one
	 * register for the whole switch leave this unset and set
	 * @mac_max_len_ctrl instead.
	 */
	int  (*mac_max_len_reg)(int p);

	/**
	 * @mac_max_len_ctrl: Register holding that same limit for every port of
	 * the switch at once, on the families that have no per port register.
	 * Set this or @mac_max_len_reg, never both.
	 */
	int mac_max_len_ctrl;

	/**
	 * @mac_max_len_ctrl_dup: Second register mirroring @mac_max_len_ctrl,
	 * where the family has one. The vendor SDK writes both.
	 */
	int mac_max_len_ctrl_dup;

	/**
	 * @max_frame: Largest L2 frame the family switches, and what turns the
	 * MTU operations on: families leaving it unset keep the ether_setup()
	 * default MTU and refuse changes. Set together with a max-length
	 * register.
	 */
	int max_frame;

	int  (*l2_port_new_salrn)(int port);
	int  (*l2_port_new_sa_fwd)(int port);
	int (*set_ageing_time)(unsigned long msec);
	int (*get_mirror_config)(struct rtldsa_mirror_config *config, int group, int port);
	int (*port_rate_police_add)(struct dsa_switch *ds, int port,
				    const struct flow_action_entry *act, bool ingress);
	int (*port_rate_police_del)(struct dsa_switch *ds, int port, struct flow_cls_offload *cls,
				    bool ingress);
	void (*print_matrix)(void);
	u64 (*read_l2_entry_using_hash)(u32 hash, u32 position, struct rtl838x_l2_entry *e);
	void (*write_l2_entry_using_hash)(u32 hash, u32 pos, struct rtl838x_l2_entry *e);
	u64 (*read_cam)(int idx, struct rtl838x_l2_entry *e);
	void (*write_cam)(int idx, struct rtl838x_l2_entry *e);
	int rma_bpdu_fld_pmask;
	int spcl_trap_eapol_ctrl;
	void (*init_eee)(struct rtl838x_switch_priv *priv, bool enable);
	void (*set_mac_eee)(struct rtl838x_switch_priv *priv, int port, bool enable);
	u64 (*l2_hash_seed)(u64 mac, u32 vid);
	u32 (*l2_hash_key)(struct rtl838x_switch_priv *priv, u64 seed);
	u64 (*read_mcast_pmask)(int idx);
	void (*write_mcast_pmask)(int idx, u64 portmask);
	void (*vlan_fwd_on_inner)(int port, bool is_set);
	void (*pie_init)(struct rtl838x_switch_priv *priv);
	int (*pie_rule_read)(struct rtl838x_switch_priv *priv, int idx, struct  pie_rule *pr);
	int (*pie_rule_write)(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr);
	int (*pie_rule_add)(struct rtl838x_switch_priv *priv, struct pie_rule *rule);
	void (*pie_rule_rm)(struct rtl838x_switch_priv *priv, struct pie_rule *rule);
	void (*l2_learning_setup)(void);
	u32 (*packet_cntr_read)(struct rtl838x_switch_priv *priv, int counter);
	void (*packet_cntr_clear)(struct rtl838x_switch_priv *priv, int counter);
	void (*set_receive_management_action)(int port, rma_ctrl_t type, action_type_t action);
	void (*led_init)(struct rtl838x_switch_priv *priv);
	u32 (*get_egress_rate)(struct rtl838x_switch_priv *priv, int port);
	int (*set_egress_rate)(struct rtl838x_switch_priv *priv, int port, u32 rate);
	void (*qos_init)(struct rtl838x_switch_priv *priv);
	int (*trk_mbr_ctr)(int group);
	void (*lag_switch_init)(struct rtl838x_switch_priv *priv);
	void (*prepare_lag_fdb)(struct rtl838x_l2_entry *e, int lag_group);
	int (*lag_set_port_members)(struct rtl838x_switch_priv *priv, int group, u64 members,
				    struct netdev_lag_upper_info *info);
	int (*lag_setup_algomask)(struct rtl838x_switch_priv *priv, int group,
				  struct netdev_lag_upper_info *info);
	int (*lag_set_distribution_algorithm)(struct rtl838x_switch_priv *priv,
					      int group, int algoidx,
					      u32 algomask);
	void (*lag_set_local_group_id)(int local_group, int global_group, bool valid);
	void (*lag_write_data)(u32 data[], struct rtldsa_93xx_lag_entry *e);
	void (*lag_fill_data)(u32 data[], struct rtldsa_93xx_lag_entry *e);
	void (*lag_set_local_port2group)(int group, int port, bool valid);
	void (*lag_set_port2group)(int group, int port, bool valid);
	int (*lag_table)(void);
	void (*lag_sync_tables)(void);
};

struct rtl838x_switch_priv {
	/* Switch operation */
	struct dsa_switch *ds;
	struct device *dev;
	u16 family_id;
	struct rtldsa_port ports[57];
	struct mutex reg_mutex;		/* Mutex for individual register manipulations */
	struct mutex pie_mutex;		/* Mutex for Packet Inspection Engine */
	int link_state_irq;
	int mirror_group_ports[4];
	const struct rtldsa_config *r;
	struct otto_l3_ctrl *l3_ctrl;
	u64 irq_mask;
	struct dentry *dbgfs_dir;

	/** @lags_port_members: Port (bit) is part of a specific LAG */
	u64 lags_port_members[MAX_LAGS];

	/** @lag_primary: port of a LAG is primary (repesenting) and is added to
	 * the port matrix
	 */
	u32 lag_primary[MAX_LAGS];

	/**
	 * @lag_non_primary: Port (bit) is part of any LAG but not the
	 * first/primary port which needs to be added in the port matrix
	 */
	u64 lag_non_primary;

	/** @lagmembers: Port (bit) is part of any LAG */
	u64 lagmembers;
	struct workqueue_struct *wq;
	bool eee_enabled;
	unsigned long mc_group_bm[MAX_MC_GROUPS >> 5];
	struct rhashtable tc_ht;
	bool tc_initialized;
	struct mutex tc_flow_lock;	/* Serializes tc flower add/del/stats */
	unsigned long pie_use_bm[MAX_PIE_ENTRIES >> 5];
	unsigned long octet_cntr_use_bm[MAX_COUNTERS >> 5];
	unsigned long packet_cntr_use_bm[MAX_COUNTERS >> 4];
	u16 intf_mtus[MAX_INTF_MTUS];
	int intf_mtu_count[MAX_INTF_MTUS];

	struct delayed_work counters_work;

	/**
	 * @counters_lock: Protects the hardware reads happening from MIB
	 * callbacks and the workqueue which reads the data
	 * periodically.
	 */
	struct mutex counters_lock;

	struct rtldsa_l2_uc *l2_uc_map;

	/**
	 * @msts: MSTI to HW MST slot allocations. index 0 is for HW slot 1 because CIST is
	 * not stored in @msts
	 */
	struct rtldsa_mst msts[];
};

struct fdb_update_work {
	struct work_struct work;
	struct net_device *ndev;
	u64 macs[];
};

void rtldsa_port_fast_age(struct dsa_switch *ds, int port);
int rtldsa_packet_cntr_alloc(struct rtl838x_switch_priv *priv);
void rtldsa_packet_cntr_free(struct rtl838x_switch_priv *priv, int idx);
int rtl83xx_port_is_under(const struct net_device *dev, struct rtl838x_switch_priv *priv);
/* Port register accessor functions for the RTL839x and RTL931X SoCs */
void rtl839x_mask_port_reg_be(u64 clear, u64 set, int reg);
u64 rtl839x_get_port_reg_be(int reg);
void rtl839x_set_port_reg_be(u64 set, int reg);
void rtl839x_mask_port_reg_le(u64 clear, u64 set, int reg);
void rtl839x_set_port_reg_le(u64 set, int reg);
u64 rtl839x_get_port_reg_le(int reg);

/* Port register accessor functions for the RTL838x and RTL930X SoCs */
void rtl838x_mask_port_reg(u64 clear, u64 set, int reg);
void rtl838x_set_port_reg(u64 set, int reg);
u64 rtl838x_get_port_reg(int reg);

/* RTL838x-specific */
u32 rtl838x_hash(struct rtl838x_switch_priv *priv, u64 seed);
void rtldsa_838x_print_matrix(void);

/* RTL839x-specific */
u32 rtl839x_hash(struct rtl838x_switch_priv *priv, u64 seed);
void rtl839x_exec_tbl2_cmd(u32 cmd);
void rtldsa_839x_print_matrix(void);

/* RTL930x-specific */
u32 rtl930x_hash(struct rtl838x_switch_priv *priv, u64 seed);
void rtldsa_930x_print_matrix(void);

/* RTL931x-specific */
void rtldsa_931x_print_matrix(void);

/*
 * TODO: The following functions are currently not in use. So compiler will complain if
 * they are static and not made available externally. To preserve them for future use
 * collect them in this section.
 */

void rtl9300_dump_debug(void);

extern const struct dsa_switch_ops rtldsa_83xx_switch_ops;
extern const struct dsa_switch_ops rtldsa_93xx_switch_ops;

extern const struct phylink_mac_ops rtldsa_83xx_phylink_mac_ops;
extern const struct phylink_mac_ops rtldsa_93xx_phylink_mac_ops;

extern const struct rtldsa_config rtldsa_838x_cfg;
extern const struct rtldsa_config rtldsa_839x_cfg;
extern const struct rtldsa_config rtldsa_930x_cfg;
extern const struct rtldsa_config rtldsa_931x_cfg;

/* TODO actually from arch/mips/rtl838x/prom.c */
extern struct rtl83xx_soc_info soc_info;

void rtl838x_dbgfs_init(struct rtl838x_switch_priv *priv);
void rtl930x_dbgfs_init(struct rtl838x_switch_priv *priv);

void rtldsa_93xx_prepare_lag_fdb(struct rtl838x_l2_entry *e, int lag_group);

struct otto_l3_nexthop;
int rtldsa_find_l2_hash_entry(struct rtl838x_switch_priv *priv, u64 seed,
			      bool must_exist, struct rtl838x_l2_entry *e);

/* RTL931x hashes its second block into rows the fib_entries count does not
 * reach, so an index can fall outside the map and is simply not tracked.
 */
static inline struct rtldsa_l2_uc *rtldsa_l2_uc_lookup(struct rtl838x_switch_priv *priv,
						      int idx)
{
	if (idx < 0 || idx >= priv->r->fib_entries)
		return NULL;

	return &priv->l2_uc_map[idx];
}

int rtldsa_l2_nexthop_add(struct rtl838x_switch_priv *priv, struct otto_l3_nexthop *nh,
			  bool require_existing);
int rtldsa_l2_nexthop_del(struct rtl838x_switch_priv *priv, struct otto_l3_nexthop *nh);

#endif /* _RTL838X_H */
