/* SPDX-License-Identifier: GPL-2.0 */
/*
 * OMCI multicast MEs: multicast operations profile (ME 309), multicast
 * subscriber config info (ME 310) and multicast subscriber monitor
 * (ME 311).  omci_mcast.c is the ME side (Set checks, ACL tables, the
 * monitor over the kernel bridge MDB), eth_mcast.c maps the MIB to Linux.
 */
#ifndef _GPOND_OMCI_MCAST_H
#define _GPOND_OMCI_MCAST_H

#include <net/if.h>
#include <stdbool.h>
#include <stdint.h>

#include "omci.h"

#define CLASS_MC_PROFILE	309
#define CLASS_MC_SUBSCRIBER	310
#define CLASS_MC_MONITOR	311

/* ME 309 attributes (SDK omci_me_multicast.h:76-91) */
#define MOP_VERSION		1
#define MOP_FUNCTION		2
#define MOP_FAST_LEAVE		3
#define MOP_UP_TCI		4
#define MOP_UP_TAG		5
#define MOP_UP_RATE		6
#define MOP_DYN_ACL		7
#define MOP_STATIC_ACL		8
#define MOP_LOST_GROUPS		9
#define MOP_ROBUSTNESS		10
#define MOP_QUERIER_IP		11
#define MOP_QUERY_INTERVAL	12
#define MOP_QUERY_RESPONSE	13
#define MOP_LAST_MEMBER		14
#define MOP_UNAUTHORIZED	15
#define MOP_DOWN_TCI		16

/* IGMP version (omci_me_multicast.h:96-100) */
#define MC_VERSION_V2		2
#define MC_VERSION_V3		3
#define MC_VERSION_MLD1		16
#define MC_VERSION_MLD2		17

/* IGMP function (omci_me_multicast.h:101-105) */
#define MC_FUNC_SNOOPING	0
#define MC_FUNC_SPR		1
#define MC_FUNC_PROXY		2

/* upstream IGMP tag control (omci_me_multicast.h:110-115) */
#define MC_UPTAG_TRANSPARENT	0
#define MC_UPTAG_ADD_TCI	1
#define MC_UPTAG_REPLACE_TCI	2
#define MC_UPTAG_REPLACE_VID	3

/* downstream IGMP and multicast TCI control (omci_me_multicast.h:135-143) */
#define MC_DOWNTAG_TRANSPARENT	0
#define MC_DOWNTAG_STRIP	1
#define MC_DOWNTAG_ADD_TCI	2
#define MC_DOWNTAG_REPLACE_TCI	3
#define MC_DOWNTAG_REPLACE_VID	4
/* 5..7 (add / replace TCI / replace VID using the VID of the UNI) are
 * accepted by the Set (omci_me_multicast_func.c:2701-2755) but refused by
 * the IGMP module (xpon_igmp_itf.c:453-467) */

/* ME 310 attributes (omci_me_multicast.c nodetable, omciAttriDescriptListMulticasSubscribertCfgInfo) */
#define MSC_ME_TYPE		1
#define MSC_PROFILE		2
#define MSC_MAX_GROUPS		3
#define MSC_MAX_BANDWIDTH	4
#define MSC_BW_ENFORCEMENT	5
#define MSC_SERVICE_PACKAGE	6
#define MSC_PREVIEW_GROUPS	7

/* ME 310 / 311 ME type (omci_me_multicast.h:158-159) */
#define MC_TYPE_BRIDGE_PORT	0
#define MC_TYPE_MAPPER		1

/* ME 311 attributes */
#define MSM_ME_TYPE		1
#define MSM_CUR_BANDWIDTH	2
#define MSM_JOINS		3
#define MSM_BW_EXCEEDED		4
#define MSM_IPV4_GROUPS		5
#define MSM_IPV6_GROUPS		6

/* table rows: ACL (attribute 7/8) 24 bytes, monitor IPv4 row 24 bytes */
#define MC_ACL_LEN		24
#define MC_GROUP_ROW_LEN	24
/* ME 309 lost groups (attribute 9) 10 bytes, ME 311 IPv6 group row 58 */
#define MC_LOST_ROW_LEN		10
#define MC_IPV6_ROW_LEN		58

/* decoded dynamic / static access control row (omci_me_multicast_func.c:2222-2243) */
struct mc_acl {
	uint16_t ctrl;			/* set control 15:14, part 13:11, row id 9:0 */
	uint16_t gem;			/* GEM port of the group */
	uint16_t vid;			/* VLAN carrying the group, 0: untagged */
	uint8_t src[4];			/* 0.0.0.0: any source */
	uint8_t start[4], end[4];	/* destination range */
	uint32_t bandwidth;		/* imputed bandwidth, bytes/s */
};

void mc_acl_decode(const uint8_t *row, struct mc_acl *a);
/* group @grp / source @src matches the row (VLAN is not part of the MDB) */
bool mc_acl_match(const struct mc_acl *a, const uint8_t grp[4],
		  const uint8_t src[4]);
/* the row names one group (start address = end address) */
bool mc_acl_single(const struct mc_acl *a);

/* the profile (ME 309) a subscriber (ME 310) points to */
struct omci_inst *mc_profile(struct omci_inst *sub);
uint16_t mc_profile_id(struct omci_inst *sub);
uint16_t mc_max_groups(struct omci_inst *sub);
uint32_t mc_max_bw(struct omci_inst *sub);
int mc_service_vids(struct omci_inst *sub, uint16_t *vid, int max);
/* ME 310 of a bridge port / mapper (ME type 0 / 1), NULL: none */
struct omci_inst *mc_subscriber(uint8_t me_type, uint16_t id);

/* bind hooks (omci_bind.c) */
bool mcast_check(struct omci_inst *in, uint8_t idx, const uint8_t *v,
		 bool create);
void mcast_set(struct omci_inst *in, uint16_t mask);
void mcast_create(struct omci_inst *in);
void mcast_delete(struct omci_inst *in);
void mcast_mib_reset(void);
bool mcast_is_me(uint16_t class_id);

/* ---- ME 311 monitor over the kernel bridge MDB ---- */

/* one port group of a bridge, as the kernel reports it */
struct mc_mdb {
	char br[IFNAMSIZ], port[IFNAMSIZ];
	uint8_t family;			/* 4 or 6 */
	uint8_t group[16], source[16];	/* IPv4: first 4 bytes */
};
#define MC_MDB_MAX		256

/* RTM_GETMDB dump; tests replace it.  Returns the entries or -errno */
extern int (*mcast_mdb_read)(struct mc_mdb *e, int max);
void mcast_mon_refresh(void);
void mcast_mon_dump(struct blob_buf *b);

/* ---- state of omci_mcast.c ---- */

/* the rows of a table of ME 309 / 310 as the OLT wrote them */
struct shadow {
	struct list_head list;
	uint16_t class_id, id;
	uint8_t idx;
	uint8_t *data;
	uint32_t len;
};

#define MC_MAX_INST		32

/* one group the bridge MDB reported, and its first sight */
struct mon_act {
	bool used, seen, fresh;
	struct mc_mdb e;
	uint32_t first;			/* monotonic seconds of the first sight */
};

/* the ME 311 counters of one instance */
struct mon_cnt {
	bool used;
	uint16_t id;
	uint32_t joins, exceeded;
};

/* RTM_GETMDB dump of one bridge */
struct mdb_ctx {
	struct mc_mdb *e;
	int n, max;
	uint32_t br_ifindex;
	char br[IFNAMSIZ];
};

#endif /* _GPOND_OMCI_MCAST_H */
