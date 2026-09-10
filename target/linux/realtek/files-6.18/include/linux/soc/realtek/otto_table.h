/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_TABLE_H
#define _OTTO_TABLE_H

#include <linux/build_bug.h>
#include <linux/compiler.h>
#include <linux/types.h>

/* An id names a table where the caller writes it; a handle is what
 * otto_table_acquire() gives back and indexes the driver's own data. The two
 * ranges are disjoint so that passing one where the other belongs is caught
 * rather than silently accepted.
 */
#define OTTO_TBL_ID_BASE	1000
#define OTTO_TBL_HANDLE(id)	((id) - OTTO_TBL_ID_BASE)

/* The switch tables reachable through indirect table access, named after
 * themselves rather than after the access register and type value that select
 * them. The names, and the entry width of each table, are Realtek's own, taken
 * from the table lists in the GPL SDK
 * (src/hal/chipdef/<chip>/rtk_<chip>_table_list.c). Several tables share one
 * type value, which means they are the same rows read with a different layout;
 * they are listed separately because their entry widths differ.
 */
enum otto_table_id {
	RTL8380_TBL_L2_UC = OTTO_TBL_ID_BASE,
	RTL8380_TBL_L2_IP_MC,
	RTL8380_TBL_L2_IP_MC_SIP,
	RTL8380_TBL_L2_MC,
	RTL8380_TBL_L2_NEXT_HOP,
	RTL8380_TBL_L2_NEXT_HOP_LEGACY,
	RTL8380_TBL_L2_CAM_UC,
	RTL8380_TBL_L2_CAM_IP_MC,
	RTL8380_TBL_L2_CAM_IP_MC_SIP,
	RTL8380_TBL_L2_CAM_MC,
	RTL8380_TBL_MC_PMSK,

	RTL8380_TBL_VLAN,
	RTL8380_TBL_IACL,
	RTL8380_TBL_MSTI,
	RTL8380_TBL_LOG,

	RTL8380_TBL_UNTAG,
	RTL8380_TBL_ROUTING,

	RTL8390_TBL_L2_UC,
	RTL8390_TBL_L2_IP_MC,
	RTL8390_TBL_L2_IP_MC_SIP,
	RTL8390_TBL_L2_MC,
	RTL8390_TBL_L2_NEXT_HOP,
	RTL8390_TBL_L2_NH_LEGACY,
	RTL8390_TBL_L2_CAM_UC,
	RTL8390_TBL_L2_CAM_IP_MC,
	RTL8390_TBL_L2_CAM_IP_MC_SIP,
	RTL8390_TBL_L2_CAM_MC,
	RTL8390_TBL_MC_PMSK,

	RTL8390_TBL_VLAN,
	RTL8390_TBL_IACL,
	RTL8390_TBL_EACL,
	RTL8390_TBL_LOG,
	RTL8390_TBL_MSTI,

	RTL8390_TBL_UNTAG,
	RTL8390_TBL_ROUTING,

	RTL9300_TBL_L2_UC,
	RTL9300_TBL_L2_MC,
	RTL9300_TBL_L2_CAM_UC,
	RTL9300_TBL_L2_CAM_MC,
	RTL9300_TBL_MC_PORTMASK,

	RTL9300_TBL_VLAN,
	RTL9300_TBL_IACL,
	RTL9300_TBL_VACL,
	RTL9300_TBL_LOG,
	RTL9300_TBL_MSTI,
	RTL9300_TBL_PORT_ISO_CTRL,
	RTL9300_TBL_LAG,
	RTL9300_TBL_SRC_TRK_MAP,

	RTL9300_TBL_L3_ROUTER_MAC,
	RTL9300_TBL_L3_HOST_ROUTE_IPUC,
	RTL9300_TBL_L3_HOST_ROUTE_IP6MC,
	RTL9300_TBL_L3_HOST_ROUTE_IP6UC,
	RTL9300_TBL_L3_HOST_ROUTE_IPMC,
	RTL9300_TBL_L3_PREFIX_ROUTE_IPUC,
	RTL9300_TBL_L3_PREFIX_ROUTE_IP6MC,
	RTL9300_TBL_L3_PREFIX_ROUTE_IP6UC,
	RTL9300_TBL_L3_PREFIX_ROUTE_IPMC,
	RTL9300_TBL_L3_NEXTHOP,
	RTL9300_TBL_L3_EGR_INTF,

	RTL9300_TBL_UNTAG,
	RTL9300_TBL_L3_EGR_INTF_MAC,

	RTL9310_TBL_L2_UC,
	RTL9310_TBL_FT_L2_HASH_FMT0_0,
	RTL9310_TBL_FT_L2_HASH_FMT0_1,
	RTL9310_TBL_FT_L2_HASH_FMT1_0,
	RTL9310_TBL_FT_L2_HASH_FMT1_1,
	RTL9310_TBL_FT_L2_HASH_FMT2_0,
	RTL9310_TBL_FT_L2_HASH_FMT2_1,
	RTL9310_TBL_L2_CB_MC,
	RTL9310_TBL_L2_CB_UC,
	RTL9310_TBL_L2_MC,
	RTL9310_TBL_L2_TNL_MCAST,
	RTL9310_TBL_L2_TNL_UCAST,
	RTL9310_TBL_PE_FWD,
	RTL9310_TBL_WLC_MCAST,
	RTL9310_TBL_WLC_UCAST,
	RTL9310_TBL_L2_CAM_UC,
	RTL9310_TBL_FT_L2_CAM_FMT0_0,
	RTL9310_TBL_FT_L2_CAM_FMT0_1,
	RTL9310_TBL_FT_L2_CAM_FMT1_0,
	RTL9310_TBL_FT_L2_CAM_FMT1_1,
	RTL9310_TBL_FT_L2_CAM_FMT2_0,
	RTL9310_TBL_L2_CAM_CB_MC,
	RTL9310_TBL_L2_CAM_CB_UC,
	RTL9310_TBL_L2_CAM_MC,
	RTL9310_TBL_L2_TNL_MCAST_CAM,
	RTL9310_TBL_L2_TNL_UCAST_CAM,
	RTL9310_TBL_PE_FWD_CAM,
	RTL9310_TBL_WLC_MCAST_CAM,
	RTL9310_TBL_WLC_UCAST_CAM,
	RTL9310_TBL_MC_PMSK,
	RTL9310_TBL_VLAN,
	RTL9310_TBL_MSTI,
	RTL9310_TBL_SRC_TRK_MAP,

	RTL9310_TBL_IACL,
	RTL9310_TBL_EACL,
	RTL9310_TBL_FT_EGR,
	RTL9310_TBL_FT_IGR,
	RTL9310_TBL_VACL,

	RTL9310_TBL_LAG,
	RTL9310_TBL_PORT_ISO_CTRL,

	RTL9310_TBL_VLAN_UNTAG,

	RTL9310_TBL_STAT_PORT_MIB_CNTR,
	RTL9310_TBL_STAT_PORT_PRVTE_CNTR,

	OTTO_TBL_END
};

#define OTTO_TBL_COUNT		OTTO_TBL_HANDLE(OTTO_TBL_END)

/* Size of the object p points at. A bare array is refused: the whole-entry
 * forms take the transfer size from the caller's buffer, which only holds
 * when p addresses the whole object rather than its first element.
 */
#define otto_table_size(p) \
	(sizeof(*(p)) + BUILD_BUG_ON_ZERO(__is_array(p), "pass &array, not the array"))

/* Read or write one whole entry. p addresses the caller's object: a u32 for a
 * one-word table, &array for a wider one. The size comes from the object, so a
 * transfer cannot run past it.
 */
#define otto_table_read(id, idx, p) \
	otto_table_read_bytes((id), (idx), (p), 0, otto_table_size(p))
#define otto_table_write(id, idx, p) \
	otto_table_write_bytes((id), (idx), (p), otto_table_size(p))

/* Hold a table across more than one access. The __ variants expect the caller
 * to hold it.
 */
int otto_table_loaded(void);
int otto_table_acquire(enum otto_table_id id);
void otto_table_release(int handle);

#define __otto_table_read(handle, idx, p) \
	__otto_table_read_bytes((handle), (idx), (p), 0, otto_table_size(p))
#define __otto_table_write(handle, idx, p) \
	__otto_table_write_bytes((handle), (idx), (p), otto_table_size(p))

/* What the macros above expand to. A read takes size bytes starting at word of
 * the entry and has to end inside it; the whole-entry forms start at word zero.
 * A write always moves the whole entry, because the command commits the whole
 * data window, so its size has to equal the entry width. Either mismatch is
 * refused and warned about rather than silently truncated, because no caller
 * checks the return value.
 */
int otto_table_read_bytes(enum otto_table_id id, int idx, void *buf,
			  int word_offset, size_t size);
int otto_table_write_bytes(enum otto_table_id id, int idx, const void *buf, size_t size);

int __otto_table_read_bytes(int handle, int idx, void *buf, int word_offset,
			    size_t size);
int __otto_table_write_bytes(int handle, int idx, const void *buf, size_t size);

#endif /* _OTTO_TABLE_H */
