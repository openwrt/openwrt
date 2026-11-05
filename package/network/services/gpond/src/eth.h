/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Ethernet side of OMCI - the model of Linux devices, bridges, tc rules,
 * L3 interfaces and MAC upstream rules shared by eth.c (MIB walk, UCI,
 * sync), eth_vlan.c (ME 171 / ME 84 translation) and eth_l3.c (IP host
 * config data, VEIP).
 */
#ifndef _GPOND_ETH_H
#define _GPOND_ETH_H

#include <net/if.h>
#include <stdbool.h>
#include <stdint.h>

#include <gpon_genl.h>

#include "gpond.h"
#include "omci.h"

struct uci_context;
struct uci_package;

#define ETH_MAX_BRIDGES		16
#define ETH_MAX_PORTS		24
#define ETH_MAX_ANI		12
#define ETH_MAX_VLANS		64
#define ETH_MAX_POLICE		16
#define ETH_MAX_TC		384
#define ETH_MAX_L3		8
/* kernel GPON_MAX_US_RULES (an7552_mac.h) */
#define ETH_MAX_US_RULES	32
/* the kernel's GPON_MAX_VID_MAP */
#define ETH_MAX_VID_MAP		16
#define ETH_PCP_KEEP		8
#define ETH_TC_PREF_BASE	100	/* xpon-qos police filters use pref 10 */

#define VID_ANY			4096
#define TPID_8021Q		0x8100
#define TPID_8021AD		0x88a8
#define PTR_NULL		0xffff

/* ME 47 TP types (SDK omci_me_mac_bridge.h:89-94) */
#define TP_PPTP_ETH_UNI		1
#define TP_8021P_MAPPER		3
#define TP_IP_HOST		4
#define TP_GEM_IWTP		5
#define TP_MCAST_GEM_IWTP	6
#define TP_VEIP			11

/* ME 171 association types (SDK omci_me_vlan.h:256-261) */
#define ASSOC_BRIDGE_PORT	0
#define ASSOC_PPTP_ETH_UNI	2
#define ASSOC_IP_HOST		3
#define ASSOC_VEIP		10

#define CLASS_VLAN_FILTER	84
#define CLASS_MAPPER		130
#define CLASS_IP_HOST		134
#define CLASS_TCP_UDP		136
#define CLASS_NET_ADDR		137
#define CLASS_AUTH_METHOD	148
#define CLASS_LARGE_STR		157
#define CLASS_EXT_VLAN		171
#define CLASS_VLAN_TAG_OP	78
#define CLASS_VEIP		329
#define CLASS_TR069		340

/* ME 78 association types (G.988 9.3.12) */
#define VT_ASSOC_UNI_IMPLICIT	0	/* ME id = PPTP Ethernet UNI / IP host */
#define VT_ASSOC_IP_HOST	1
#define VT_ASSOC_BRIDGE_PORT	3
#define VT_ASSOC_UNI		10	/* explicit PPTP Ethernet UNI */
#define VT_ASSOC_VEIP		11

/* VEIP: SDK omci_me.h:54,93, VEIP_SLOT << 8 | VEIP_INST_ID */
#define VEIP_ME_ID		0x0a01

/* egress P-bit handling of an 802.1Q/ad device */
#define EGR_DONTCARE		(-2)	/* lookup only */
#define EGR_NONE		(-1)	/* no mapping, tag P-bit 0 */
#define EGR_IDENT		8	/* skb->priority is the P-bit */
/* ingress */
#define ING_DONTCARE		(-1)
#define ING_NONE		0
#define ING_IDENT		1	/* P-bit of the tag is skb->priority */

/*
 * An 802.1Q / 802.1ad device netifd creates (vlandev.c: device types
 * "8021q" and "8021ad", options ifname, vid, ingress_qos_mapping,
 * egress_qos_mapping).  The P-bit goes through skb->priority: the ingress
 * mapping of the device a frame is received on sets it, the egress mapping
 * of the device it leaves through writes it to the tag.
 */
struct eth_vlan {
	char name[IFNAMSIZ];
	char parent[IFNAMSIZ];
	uint16_t vid;
	uint16_t tpid;
	int8_t egress;			/* EGR_NONE, 0..7 fixed, EGR_IDENT */
	int8_t ingress;			/* ING_NONE / ING_IDENT */
	bool dscp;			/* DSCP -> priority rules installed */
	/* multicast options of a bridge port that is this device (eth_mcast.c) */
	bool fast_leave;
	uint8_t mrouter;		/* MROUTER_* */
};

/*
 * netifd multicast_router (device.c:493-497) is the code minus one:
 * 0 never, 1 learn (the kernel default), 2 always
 */
#define MROUTER_UNSET		0
#define MROUTER_NEVER		1
#define MROUTER_ALWAYS		3

#define ETH_MAX_ACL		32

/* IPv4 group range of a dynamic access control row */
struct eth_acl {
	uint8_t start[4], end[4];
};

struct eth_port {
	char dev[IFNAMSIZ];
	bool fast_leave;
	uint8_t mrouter;		/* MROUTER_*, plain devices only */
	uint16_t max_groups;		/* mcast_max_groups, 0: no limit */
	/* dynamic ACL: the groups an IGMP report may name; none: any */
	struct eth_acl acl[ETH_MAX_ACL];
	uint8_t nacl;
};

/* an ANI-side leaf device of a bridge, with the VID of its outer tag */
struct eth_ani {
	char dev[IFNAMSIZ];
	uint16_t vid;			/* 0: no tag */
};

/* one Linux bridge: an OMCI MAC bridge service profile (ME 45) */
struct eth_bridge {
	uint16_t id;
	char name[IFNAMSIZ];
	bool igmp;
	int robustness;
	/* netifd bridge options, centiseconds, 0: netifd default */
	uint32_t query_interval, query_response, last_member;
	/* ME 45 MAC bridge service profile -> netifd bridge / port options */
	bool stp;			/* spanning tree ind */
	bool learning;			/* learning ind */
	bool unknown_discard;		/* unknown MAC address discard */
	uint16_t priority;		/* 0: netifd default */
	uint16_t max_age, hello_time, forward_delay;	/* seconds, 0: default */
	uint32_t ageing_time;		/* seconds, 0: default */
	struct eth_port port[ETH_MAX_PORTS];
	int nport;
	struct eth_ani ani[ETH_MAX_ANI];
	int nani;
};

struct eth_police {
	char dev[IFNAMSIZ];
	bool ingress;
	uint32_t rate;			/* bytes/s */
	uint32_t burst;			/* bytes */
};

/* port settings of a UNI device (PPTP Ethernet UNI) */
struct eth_uni {
	char dev[IFNAMSIZ];
	uint16_t me;
	bool disabled;			/* admin state lock */
	uint16_t speed;			/* Mbit/s, 0: any */
	int8_t duplex;			/* 1 full, 0 half, -1 any */
	uint16_t mtu;			/* 0: not set */
	bool pause;			/* pause time, nonzero: PAUSE on */
	bool pause_set;			/* the OLT wrote attribute 10 */
	bool pppoe_filter;		/* attr 14: only PPPoE ingress */
	uint8_t loopback;		/* not applied */
};

/*
 * A tc filter the xpon-vlan service installs ("vrule" sections of
 * /etc/config/gpon):
 *   tc filter replace dev <dev> <ingress|egress> pref <pref>
 *      protocol <proto> flower <match> skip_hw action <action>
 */
struct eth_tc {
	char dev[IFNAMSIZ];
	bool egress;
	uint16_t pref;
	char proto[8];
	char match[128];
	char action[192];
};

/* a netifd interface of an IP host (ME 134) or behind a VEIP (ME 329) */
struct eth_l3 {
	char name[20];			/* xpon_iph<id> / xpon_wan<n> */
	char dev[IFNAMSIZ];
	uint16_t class_id, me;
	char proto[16];
	bool disabled;
	bool ipv6;			/* + <name>6 with proto dhcpv6 */
	bool ping, trace;		/* ME 134 IP options bit 1 / bit 2 */
	bool mc_proxy;			/* uplink of an omcproxy (ME 309 function 2) */
	uint8_t addr[4], mask[4], gw[4], dns[2][4];
};

struct eth_us_rule {
	bool valid;
	uint16_t vid;			/* VID_ANY */
	uint8_t pcp_mask;		/* 0: any */
	uint16_t gem;
};

/*
 * The VID rewrite of a router row (tc on pon0 egress), handed to the
 * kernel for the upstream flows the PPE forwards (CMD_VID_MAP_SET): the
 * PPE never runs the tc rules.
 */
struct eth_vid_map {
	bool valid;
	uint16_t uni_vid;
	uint8_t pcp_mask;		/* 0: any */
	uint16_t ani_vid;
	uint8_t pcp;			/* ETH_PCP_KEEP: unchanged */
};

struct eth_cwmp {
	bool present;
	bool enabled;			/* ME 340 attr 1 == 0 (unlocked) */
	uint16_t me;
	uint16_t vid;			/* Associated tag VID, 0: none */
	uint8_t pbit;			/* Associated tag P-bit */
	uint16_t port;			/* ME 136 TCP/UDP port, 0: none */
	uint8_t auth_scheme;		/* ME 148 attr 1 */
	char iface[20];			/* netifd L3 interface name */
	char url[376];			/* ME 157 up to 15 * 25 bytes */
	char username[51];		/* ME 148 Username 1 + Username 2 */
	char password[26];		/* ME 148 Password */
	char realm[26];			/* ME 148 Realm */
};

#define ETH_DSCP_NUM		64

struct eth_model {
	struct eth_bridge br[ETH_MAX_BRIDGES];
	int nbr;
	struct eth_uni uni[GPON_MAX_UNI];
	int nuni;
	struct eth_vlan vlan[ETH_MAX_VLANS];
	int nvlan;
	struct eth_police pol[ETH_MAX_POLICE];
	int npol;
	struct eth_tc tc[ETH_MAX_TC];
	int ntc;
	struct eth_l3 l3[ETH_MAX_L3];
	int nl3;
	struct eth_cwmp cwmp;
	struct eth_us_rule us[ETH_MAX_US_RULES];
	struct eth_vid_map vmap[ETH_MAX_VID_MAP];
	/*
	 * The queue of its T-CONT channel a frame of a P-bit goes to (CMD_PCP_
	 * QUEUE, one map for the MAC): the priority queue the GEM port of that
	 * P-bit points to (ME 268 attribute 4, ME 277 related port: priority p
	 * is queue 7 - p), 7 - P-bit where no queue is named.
	 */
	uint8_t pcp_queue[8];
	uint8_t pcp_queue_set;		/* the P-bits a priority queue decided */
	/* P-bit of untagged IP frames per DSCP (ME 130 unmarked frame
	 * option 0): the MAC reads the DSCP (CMD_DSCP_MAP) */
	bool dscp_on;
	uint8_t dscp_map[ETH_DSCP_NUM];
	/* omcproxy downlinks (onu.mcast_downlink) of the proxies in l3[] */
	char mc_downlink[64];
};

/* ---- eth.c: model helpers ---- */
struct eth_auto_detect {
	uint8_t code;
	uint16_t speed;
	int8_t duplex;
};

struct l3seen {
	uint16_t class_id[ETH_MAX_L3 * 2], me[ETH_MAX_L3 * 2];
	int n;
};

const char *eth_vlan_dev(struct eth_model *m, const char *parent,
			 uint16_t tpid, uint16_t vid, int egress, int ingress);
/* only a pon* or br-pon* device, and never the bridge itself */
struct eth_port *eth_add_port(struct eth_bridge *b, const char *dev);
int eth_tc_add(struct eth_model *m, const char *dev, bool egress,
	       const char *proto, const char *match, const char *action);
void eth_vid_map_add(struct eth_model *m, uint16_t uni_vid, uint8_t pcp_mask,
		     uint16_t ani_vid, uint8_t pcp);
void eth_bridge_ani(struct eth_bridge *b, const char *dev, uint16_t vid);
const struct eth_model *eth_current(void);

/* ---- eth_vlan.c ---- */

/* ME 84 (VLAN tagging filter data) of the ANI side of a bridge */
struct xv_filter {
	bool active;			/* restricts tagged frames */
	bool untagged_ok;
	bool pbit;			/* by P-bit (any VID), not by VID */
	uint8_t pbit_mask;		/* the P-bits that pass, bit per value */
	uint16_t vid[12];
	int n;
};

struct ani_port {
	uint16_t id, td_in, td_out;
	struct xv_filter flt;
	int gem[8];			/* GEM port per P-bit, -1 none */
	uint8_t unmarked, def_pbit;
	bool dscp;			/* untagged frames: P-bit from the DSCP */
	uint8_t dscp_map[ETH_DSCP_NUM];
};

/* a leaf device the ANI side of a port ends in */
struct xv_leaf {
	char dev[IFNAMSIZ];
	uint16_t vid;			/* outer tag, 0: none */
};
#define XV_MAX_LEAF		16

/* decoded row of the received frame VLAN tagging operation table */
struct xv_row {
	/* filter, [0] outer, [1] inner tag */
	uint8_t f_prio[2];	/* 0..7, 8 any, 14 default row, 15 no tag */
	uint16_t f_vid[2];	/* 0..4094, 4096 any */
	uint8_t f_tpid[2];	/* 0 any, 4 0x8100, 5 input TPID, 6/7 +DEI 0/1 */
	uint8_t ethertype;	/* 0 any, 1 IPoE, 2 PPPoE, 3 ARP, 4 IPv6 */
	/* treatment, [0] outer, [1] inner tag to add */
	uint8_t remove;		/* tags to remove 0..2, 3 discard */
	uint8_t t_prio[2];	/* 0..7, 8/9 copy inner/outer, 10 DSCP, 15 none */
	uint16_t t_vid[2];	/* 0..4094, 4096/4097 copy inner/outer */
	uint8_t t_tpid[2];	/* 0..4, 6, 7 */
	uint8_t ntags;		/* tags of the frames the row filters */
	bool def;		/* default row (filter priority 14) */
	bool del;		/* delete entry: last 8 bytes all ones */
};

#define XV_ROW_LEN		16
#define XV_MAX_ROWS		32	/* SDK OMCI_MAX_PORT_VLAN_RULE_NUM */

struct tag {
	int vid;			/* -1: any */
	int prio;			/* -1: any */
	uint16_t tpid;
};

struct addt {
	uint16_t tpid;
	int vid, vid_src;		/* vid >= 0 fixed, else copy of f[vid_src] */
	int prio, prio_src;		/* prio >= 0 fixed, else copy / dscp */
	bool dscp;
};

struct plan {
	int ntags, remove, nadd;
	bool block, dei;
	bool any_tpid;			/* outer filter TPID 0: 802.1Q and 802.1ad */
	struct tag f[2];		/* received tags, outermost first */
	struct addt a[2];		/* tags added, outermost first */
};

struct ctx {
	struct eth_model *m;
	const char *uni;		/* UNI device, NULL: the router side */
	uint16_t tpid_in, tpid_out;
	bool inverse;			/* downstream mode 0 */
	const uint8_t *dscp;		/* ME 171 attribute 8 */
	bool force_tc;
	bool guard;			/* tagged frames pass before untagged rules */
};

struct dev_res {
	const char *uni;		/* leaf of the UNI side */
	const char *ani;		/* leaf of the ANI side */
	uint16_t vid;			/* outer tag of the ANI side, 0 none */
	bool dscp;
};

struct xv_add_tag {
	uint16_t tpid;
	int vid, eg;
};

struct vrow {
	struct xv_row v;
	int key;
};

/* 24 bytes DSCP -> P-bit table of ME 171 / ME 130 -> P-bit per DSCP */
void xv_dscp_decode(const uint8_t *map24, uint8_t pbit[ETH_DSCP_NUM]);
/* may row @row be stored in ME 171 @in: NULL, or why not */
const char *xv_row_check(struct omci_inst *in, const uint8_t *row);
void xv_filter_get(struct omci_inst *f84, struct xv_filter *f);
/* may the OLT set ME 84 forward operation @op */
bool xv_filter_op_ok(uint8_t op);
bool xv_filter_pass(const struct xv_filter *f, uint16_t vid);
/* UNI side port @uni of bridge @b (ME 171 @ev, may be NULL); the
 * ANI leaves are added to the bridge */
void xv_uni_port(struct eth_model *m, struct eth_bridge *b, const char *uni,
		 struct omci_inst *ev, const struct xv_filter *f);
/* L3 side port (IP host, VEIP): the ANI-side leaves, count returned;
 * @bridged: the router shares a bridge with a UNI */
int xv_l3_port(struct eth_model *m, struct omci_inst *ev,
	       const struct xv_filter *f, struct xv_leaf *leaf, int max,
	       bool bridged);
/* no ME 171: the ANI side as ME 84 / transparent says */
void xv_transparent_port(struct eth_model *m, struct eth_bridge *b,
			 const char *uni, const struct xv_filter *f);
/* ME 84 P-bit investigation (f->pbit): tc rules for an ANI leaf device
 * with a VLAN tag (pon0 egress/ingress) and for the raw ANI leaf (the
 * ingress of the UNI devices) */
void xv_pbit_leaf(struct eth_model *m, const char *leaf, const struct xv_filter *f);
void xv_pbit_raw(struct eth_model *m, const char *uni, const struct xv_filter *f);

/* ---- eth_mcast.c: multicast operations profile (ME 309) / subscriber (ME 310) ---- */
/* UNI ports of a bridge: (port ME, device) */
struct mc_uni {
	struct omci_inst *bp;
	const char *dev;
};
void eth_mcast_pre(struct eth_model *m, struct omci_inst *br);
void eth_mcast_bridge(struct eth_model *m, struct eth_bridge *b);
/* omcproxy and bridge port limits: /etc/config/omcproxy, /etc/config/gpon */
bool eth_mcast_proxy_changed(const struct eth_model *a, const struct eth_model *b);
bool eth_mcast_proxy_present(const struct eth_model *m);
int eth_mcast_proxy_write(const struct eth_model *m, bool *changed);
int eth_mcast_gpon_uci(struct uci_context *ctx, const struct eth_model *m);
void eth_mcast_report(const struct eth_model *m);
void eth_mcast_dump(struct blob_buf *b);
/* the UNI Linux devices a multicast subscriber ME (310/311) covers */
int eth_mcast_ports(uint8_t me_type, uint16_t id, const char **dev, int max);
#define OMCP_PKG		"omcproxy"

#define CWMP_PKG		"easycwmp"

/* ---- eth_l3.c ---- */
struct eth_l3 *eth_l3_add(struct eth_model *m, uint16_t class_id,
			  uint16_t me, const char *dev);
int eth_l3_uci(struct uci_context *ctx, struct uci_package *pkg,
	       const struct eth_l3 *l);
void eth_cwmp_build(struct eth_model *m);
int eth_cwmp_write(const struct eth_model *m, bool *changed);
void eth_l3_init(void);
void eth_l3_applied(const struct eth_model *m);
void eth_l3_mib_reset(void);
bool eth_l3_check(struct omci_inst *in, uint8_t idx, const uint8_t *v);
/*
 * Ping / traceroute response of the IP hosts (ME 134 IP options bits 1/2)
 * is only reported, never written: /etc/config/firewall belongs to the
 * system, see eth_l3.c.  No FW_PKG here on purpose.
 */
void eth_l3_dump(struct blob_buf *b);

#endif /* _GPOND_ETH_H */
