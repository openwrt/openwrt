/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_VLAN_H
#define _OTTO_VLAN_H

#include <linux/netlink.h>
#include <linux/types.h>
#include <net/dsa.h>
#include <net/switchdev.h>

struct rtl838x_switch_priv;

/* VLAN definitions shared with family and debug code. */
#define RTL838X_VLAN_CTRL			(0x3a74)
#define RTL838X_VLAN_PORT_EGR_FLTR		(0x3a84)
#define RTL838X_VLAN_PORT_IGR_FLTR		(0x3a7c)
#define RTL838X_VLAN_PROFILE_MAX		7

#define RTL839X_VLAN_CTRL			(0x26d4)
#define RTL839X_VLAN_PORT_EGR_FLTR		(0x27c4)
#define RTL839X_VLAN_PORT_IGR_FLTR		(0x27b4)
#define RTL839X_VLAN_PROFILE_MAX		7

#define RTL930X_VLAN_CTRL			(0x82d4)
#define RTL930X_VLAN_PORT_EGR_FLTR		(0x83c8)
#define RTL930X_VLAN_PORT_IGR_FLTR		(0x83c0)
#define RTL930X_VLAN_PROFILE_MAX		7

#define RTL931X_VLAN_CTRL			(0x94e4)
#define RTL931X_VLAN_PORT_EGR_FLTR		(0x96c4)
#define RTL931X_VLAN_PORT_IGR_FLTR		(0x96b4)
#define RTL931X_VLAN_PROFILE_MAX		15

#define MAX_VLANS				4096

enum igr_filter {
	IGR_FORWARD = 0,
	IGR_DROP = 1,
	IGR_TRAP = 2,
};

enum egr_filter {
	EGR_DISABLE = 0,
	EGR_ENABLE = 1,
};

enum pbvlan_type {
	PBVLAN_TYPE_INNER = 0,
	PBVLAN_TYPE_OUTER,
};

enum pbvlan_mode {
	PBVLAN_MODE_UNTAG_AND_PRITAG = 0,
	PBVLAN_MODE_UNTAG_ONLY,
	PBVLAN_MODE_ALL_PKT,
};

struct rtldsa_vlan_info {
	u64 untagged_ports;
	u64 member_ports;
	u8 profile_id;
	bool hash_mc_fid;
	bool hash_uc_fid;
	u8 fid; /* AKA MSTI */

	/* The following fields are used only by the RTL931X */
	int if_id;		/* Interface (index in L3_EGR_INTF_IDX) */
	u16 multicast_grp_mask;
	int l2_tunnel_list_id;
};

struct rtldsa_vlan_profile {
	union {
		struct {
			u64 l2;
			u64 ip;
			u64 ip6;
		} pmsks;
		struct {
			u16 l2;
			u16 ip;
			u16 ip6;
		} pmsks_idx;
	} unkn_mc_fld;

	int l2_learn;

	u8 pmsk_is_idx:1, routing_ipuc:1, routing_ip6uc:1,
	   routing_ipmc:1, routing_ip6mc:1, bridge_ipmc:1, bridge_ip6mc:1;
};

void rtl838x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info);
void rtl838x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info);
void rtl838x_vlan_set_untagged(u32 vlan, u64 portmask);
void rtl838x_vlan_fwd_on_inner(int port, bool is_set);
int rtldsa_838x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile);
void rtl838x_vlan_profile_setup(int profile);
void rtl838x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner);
void rtl838x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode);
void rtl838x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid);
void rtldsa_838x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx);

void rtl839x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info);
void rtl839x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info);
void rtl839x_vlan_set_untagged(u32 vlan, u64 portmask);
void rtl839x_vlan_fwd_on_inner(int port, bool is_set);
int rtldsa_839x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile);
void rtl839x_vlan_profile_setup(int profile);
void rtldsa_839x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx);
void rtl839x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner);
void rtl839x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode);
void rtl839x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid);

void rtl930x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info);
void rtl930x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info);
int rtldsa_930x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile);
void rtldsa_930x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx);
void rtl930x_vlan_set_untagged(u32 vlan, u64 portmask);
void rtl930x_vlan_fwd_on_inner(int port, bool is_set);
void rtl930x_vlan_profile_setup(int profile);
void rtl930x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner);
void rtl930x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode);
void rtl930x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid);

int rtldsa_931x_vlan_profile_get(int idx, struct rtldsa_vlan_profile *profile);
void rtldsa_931x_vlan_profile_dump(struct rtl838x_switch_priv *priv, int idx);
void rtl931x_vlan_tables_read(u32 vlan, struct rtldsa_vlan_info *info);
void rtl931x_vlan_set_tagged(u32 vlan, struct rtldsa_vlan_info *info);
void rtl931x_vlan_set_untagged(u32 vlan, u64 portmask);
void rtl931x_vlan_fwd_on_inner(int port, bool is_set);
void rtl931x_vlan_profile_setup(int profile);
void rtl931x_vlan_port_keep_tag_set(int port, bool keep_outer, bool keep_inner);
void rtl931x_vlan_port_pvidmode_set(int port, enum pbvlan_type type, enum pbvlan_mode mode);
void rtl931x_vlan_port_pvid_set(int port, enum pbvlan_type type, int pvid);

void rtldsa_vlan_setup(struct rtl838x_switch_priv *priv);
int rtldsa_vlan_filtering(struct dsa_switch *ds, int port, bool vlan_filtering,
			  struct netlink_ext_ack *extack);
int rtldsa_vlan_add(struct dsa_switch *ds, int port, const struct switchdev_obj_port_vlan *vlan,
		    struct netlink_ext_ack *extack);
int rtldsa_vlan_del(struct dsa_switch *ds, int port, const struct switchdev_obj_port_vlan *vlan);
int rtldsa_port_vlan_fast_age(struct dsa_switch *ds, int port, u16 vid);
int rtldsa_vlan_msti_set(struct dsa_switch *ds, struct dsa_bridge bridge,
			 const struct switchdev_vlan_msti *msti);

#endif /* _OTTO_VLAN_H */
