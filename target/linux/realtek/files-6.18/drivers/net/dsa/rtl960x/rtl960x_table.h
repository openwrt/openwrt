/* SPDX-License-Identifier: GPL-2.0-only */
/* Look-up table query interface for the RTL960x switch core */

#ifndef __RTL960X_TABLE_H
#define __RTL960X_TABLE_H

#include <linux/types.h>

#include "rtl960x_dsa.h"

/* Largest entry handled by the engine (L2), in 32-bit words */
#define RTL960X_TABLE_ENTRY_MAX_SIZE		3

/*
 * enum rtl960x_table - switch tables reached through the table-access engine
 * @RTL960X_TABLE_L2: L2 unicast/multicast hash table (2048 + 64 CAM entries)
 * @RTL960X_TABLE_VLAN: VLAN table (4096 entries)
 *
 * NOTE: Don't change the enum values. They are the TBL_TYPE field of the
 * control register.
 */
enum rtl960x_table {
	RTL960X_TABLE_L2 = 0,
	RTL960X_TABLE_VLAN = 1,
};

/*
 * enum rtl960x_table_op - table query operation
 * @RTL960X_TABLE_OP_READ: read an entry from the target table
 * @RTL960X_TABLE_OP_WRITE: write an entry to the target table
 *
 * NOTE: Don't change the enum values. They are the CMD_TYPE field of the
 * control register.
 */
enum rtl960x_table_op {
	RTL960X_TABLE_OP_READ = 0,
	RTL960X_TABLE_OP_WRITE = 1,
};

/*
 * enum rtl960x_table_l2_method - look-up method for L2 table queries
 * @RTL960X_TABLE_L2_METHOD_MAC: look-up by MAC address and FID/VID key
 * @RTL960X_TABLE_L2_METHOD_ADDR: look-up by entry address
 * @RTL960X_TABLE_L2_METHOD_ADDR_NEXT: next valid entry starting from the
 *   supplied address
 * @RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC: same as ADDR_NEXT but unicast only
 * @RTL960X_TABLE_L2_METHOD_ADDR_NEXT_MC: same as ADDR_NEXT but multicast only
 * @RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT: same as ADDR_NEXT_UC but only
 *   entries with a matching source port
 *
 * NOTE: Don't change the enum values. They are the ACCESS_METHOD field of
 * the control register.
 */
enum rtl960x_table_l2_method {
	RTL960X_TABLE_L2_METHOD_MAC = 0,
	RTL960X_TABLE_L2_METHOD_ADDR = 1,
	RTL960X_TABLE_L2_METHOD_ADDR_NEXT = 2,
	RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC = 3,
	RTL960X_TABLE_L2_METHOD_ADDR_NEXT_MC = 4,
	/*
	 * RTL960X_TABLE_L2_METHOD_ADDR_NEXT_MC_L3 = 5,
	 * RTL960X_TABLE_L2_METHOD_ADDR_NEXT_MC_L2L3 = 6,
	 */
	RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT = 7,
};

int rtl960x_table_query(struct rtl960x_dsa *priv, enum rtl960x_table table,
			enum rtl960x_table_op op, u16 *addr,
			enum rtl960x_table_l2_method method, u16 port,
			u32 *data, size_t size);

#endif /* __RTL960X_TABLE_H */
