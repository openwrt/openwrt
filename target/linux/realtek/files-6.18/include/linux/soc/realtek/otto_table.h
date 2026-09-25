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
 * type value; unless they also carry an offset, that means they are the same
 * rows read with a different layout. They are listed separately because their
 * entry widths differ.
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

	RTL9607C_TBL_L2_MC_DSL,
	RTL9607C_TBL_L2_UC,
	RTL9607C_TBL_L3_MC,
	RTL9607C_TBL_L3_MC_FID,
	RTL9607C_TBL_L3_MC_VID,
	RTL9607C_TBL_VLAN,
	RTL9607C_TBL_ACL_DATA,
	RTL9607C_TBL_ACL_MASK,
	RTL9607C_TBL_ACL_ACTION_TABLE,
	RTL9607C_TBL_CF_MASK_T0,
	RTL9607C_TBL_CF_MASK_T1,
	RTL9607C_TBL_CF_MASK_T2,
	RTL9607C_TBL_CF_RULE_T0,
	RTL9607C_TBL_CF_RULE_T1,
	RTL9607C_TBL_CF_RULE_T2,
	RTL9607C_TBL_CF_ACTION_DS,
	RTL9607C_TBL_CF_ACTION_US,

	RTL9607C_TBL_INTERFACE,
	RTL9607C_TBL_ETHER_TYPE,
	RTL9607C_TBL_CAM_TAG,
	RTL9607C_TBL_FB_EXT_PORT,
	RTL9607C_TBL_WAN_ACCESS_LIMIT,
	RTL9607C_TBL_FLOW_TABLE_PATH1_2,
	RTL9607C_TBL_FLOW_TABLE_PATH3_4,
	RTL9607C_TBL_FLOW_TABLE_PATH5,
	RTL9607C_TBL_FLOW_TABLE_PATH6,
	RTL9607C_TBL_CAM,
	RTL9607C_TBL_MAC_IDX,
	RTL9607C_TBL_FLOW_TABLE_TAG,
	RTL9607C_TBL_TCAM,
	RTL9607C_TBL_TCAM_RAW_TABLE_PATH1_2,
	RTL9607C_TBL_TCAM_RAW_TABLE_PATH3_5,

	OTTO_TBL_END
};

/*
 * enum otto_table_l2_method - look-up method for read queries of L2 table
 * @OTTO_TABLE_L2_METHOD_MAC: look-up by source MAC address and FID (or VID)
 * @OTTO_TABLE_L2_METHOD_ADDR: look-up by entry address
 * @OTTO_TABLE_L2_METHOD_ADDR_NEXT: look-up next entry starting from the
 *   supplied address
 * @OTTO_TABLE_L2_METHOD_ADDR_NEXT_UC: same as ADDR_NEXT but search only
 *   unicast addresses
 * @OTTO_TABLE_L2_METHOD_ADDR_NEXT_MC: same as ADDR_NEXT but search only
 *   multicast addresses
 * @OTTO_TABLE_L2_METHOD_ADDR_NEXT_MC_L3: same as ADDR_NEXT_MC but search only
 *   L2 IP multicast addresses
 * @OTTO_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT: same as ADDR_NEXT_UC but
 *   search only entries with matching source port
 *
 * NOTE: Don't change the enum values.
 */
enum otto_table_l2_method {
	OTTO_TABLE_L2_METHOD_MAC = 0,
	OTTO_TABLE_L2_METHOD_ADDR = 1,
	OTTO_TABLE_L2_METHOD_ADDR_NEXT = 2,
	OTTO_TABLE_L2_METHOD_ADDR_NEXT_UC = 3,
	OTTO_TABLE_L2_METHOD_ADDR_NEXT_MC = 4,
	OTTO_TABLE_L2_METHOD_ADDR_NEXT_MC_L3 = 5,
	/*
	 * OTTO_TABLE_L2_METHOD_ADDR_NEXT_MC_L2L3 = 6,
	 */
	OTTO_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT = 7,
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

#define otto_table_offset_read(id, idx, p, word_offset) \
	otto_table_read_bytes((id), (idx), (p), (word_offset), otto_table_size(p))

#define otto_table_l2_read(id, idx, p, port, method) \
	otto_table_l2_query((id), (idx), (p), false, otto_table_size(p), \
			    (port), (method))
#define otto_table_l2_write(id, idx, p, port, method) \
	otto_table_l2_query((id), (idx), (p), true, otto_table_size(p), \
			    (port), (method))

/* How many rows a table has, so that a caller sweeping one does not have to
 * carry its own copy of the size. Negative errno for an id that does not name
 * a table.
 */
int otto_table_rows(enum otto_table_id id);

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

#define __otto_table_l2_read(handle, idx, p, port, method) \
	__otto_table_l2_query((handle), (idx), (p), false, otto_table_size(p), \
			      (port), (method))
#define __otto_table_l2_write(handle, idx, p, port, method) \
	__otto_table_l2_query((handle), (idx), (p), true, otto_table_size(p), \
			      (port), (method))

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

/*
 * otto_table_l2_query - read from or write to a L2 table
 * @id: target table from enum otto_table_id
 * @idx: table address. For L2 read queries, it is ignored as input for
 *       MAC-based lookup methods and used as input for address-based
 *       lookup methods. On successful L2 queries, it is updated with
 *       the matched entry address.
 * @buf: data buffer used to read from or write to the table. For L2 MAC
 *       lookups, this buffer provides the lookup key and recieves the
 *       matched entry contents on success.
 * @is_write: read or write operation
 * @size: size of @buf in bytes. The caller must ensure that @size matches the
 *        target table's entry size.
 * @port: for L2 read queries using method
 *        %OTTO_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT, restrict the search
 *        to entries associated with this source port. Ignored otherwise.
 * @method: L2 table lookup method, see &enum otto_table_l2_method.
 *          Ignored for non-L2 tables.
 */
int otto_table_l2_query(enum otto_table_id id, int *idx, void *buf,
			bool is_write, size_t size, int port,
			enum otto_table_l2_method method);
int __otto_table_l2_query(int handle, int *idx, void *buf, bool is_write,
			  size_t size, int port,
			  enum otto_table_l2_method method);

#endif /* _OTTO_TABLE_H */
