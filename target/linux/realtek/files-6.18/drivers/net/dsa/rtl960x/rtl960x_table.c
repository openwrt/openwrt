// SPDX-License-Identifier: GPL-2.0-only
/* Look-up table query interface for the RTL960x switch core */

#include <linux/bitfield.h>
#include <linux/regmap.h>

#include "rtl960x_table.h"

/* Table access control register */
#define RTL960X_TABLE_CTRL_REG			0x12000
/* One of rtl960x_table */
#define   RTL960X_TABLE_CTRL_TABLE_MASK		GENMASK(2, 0)
/* One of rtl960x_table_op */
#define   RTL960X_TABLE_CTRL_OP_MASK		GENMASK(4, 3)
/* One of rtl960x_table_l2_method */
#define   RTL960X_TABLE_CTRL_METHOD_MASK	GENMASK(7, 5)
#define   RTL960X_TABLE_CTRL_PORT_MASK		GENMASK(11, 8)
#define   RTL960X_TABLE_CTRL_ADDR_MASK		GENMASK(23, 12)

/* Table status register */
#define RTL960X_TABLE_STATUS_REG		0x12004
/* Matched entry: hash index, or CAM index with ADDR_TYPE set */
#define   RTL960X_TABLE_STATUS_ADDRESS_MASK	GENMASK(10, 0)
#define   RTL960X_TABLE_STATUS_ADDR_TYPE_MASK	BIT(11)
#define   RTL960X_TABLE_STATUS_HIT_STATUS_MASK	BIT(12)
#define   RTL960X_TABLE_STATUS_BUSY_FLAG_MASK	BIT(13)

/* Table read/write data registers, one 32-bit word each */
#define RTL960X_TABLE_WRITE_BASE		0x12008
#define RTL960X_TABLE_WRITE_REG(_x) \
		(RTL960X_TABLE_WRITE_BASE + (_x) * 4)
#define RTL960X_TABLE_READ_BASE			0x1201c
#define RTL960X_TABLE_READ_REG(_x) \
		(RTL960X_TABLE_READ_BASE + (_x) * 4)

static int rtl960x_table_poll_busy(struct rtl960x_dsa *priv)
{
	u32 val;

	return regmap_read_poll_timeout(priv->map, RTL960X_TABLE_STATUS_REG,
			val,
			!FIELD_GET(RTL960X_TABLE_STATUS_BUSY_FLAG_MASK, val),
			1, 10000);
}

/**
 * rtl960x_table_query() - read from or write to a switch table
 * @priv: driver context
 * @table: target table, see &enum rtl960x_table
 * @op: read or write operation, see &enum rtl960x_table_op
 * @addr: table address. For the VLAN table, this selects the entry to
 *        access. For L2 queries, it is ignored as input for the MAC-based
 *        method and for writes (the hash selects the slot), and used as input
 *        for the address-based methods. On successful L2 queries, it is
 *        updated with the matched entry address; the CAM entries follow the
 *        2048 hash entries.
 * @method: L2 table lookup method, see &enum rtl960x_table_l2_method.
 *          Ignored for the VLAN table and for writes.
 * @port: for L2 read queries using method
 *        %RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT, restrict the search to
 *        entries associated with this source port. Ignored otherwise.
 * @data: data buffer used to read from or write to the table. For L2 MAC
 *        lookups, this buffer provides the lookup key and receives the
 *        matched entry contents on success.
 * @size: size of @data in 32-bit words, at most RTL960X_TABLE_ENTRY_MAX_SIZE.
 *
 * The VLAN table is a simple indexed table. The L2 table is a hash table and
 * supports several lookup methods; on a miss, or when a write finds no slot
 * (table full on add), %-ENOENT is returned and @addr is unchanged. Whether
 * %-ENOENT is an error is for the caller to decide.
 *
 * Context: Can sleep. Takes and releases &priv->table_lock.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_table_query(struct rtl960x_dsa *priv, enum rtl960x_table table,
			enum rtl960x_table_op op, u16 *addr,
			enum rtl960x_table_l2_method method, u16 port,
			u32 *data, size_t size)
{
	bool addr_as_input = true;
	bool write_data = false;
	int ret;
	u32 cmd;
	u32 val;
	int i;

	if (size > RTL960X_TABLE_ENTRY_MAX_SIZE)
		return -EINVAL;

	cmd = FIELD_PREP(RTL960X_TABLE_CTRL_TABLE_MASK, table) |
	      FIELD_PREP(RTL960X_TABLE_CTRL_OP_MASK, op);
	if (op == RTL960X_TABLE_OP_READ && table == RTL960X_TABLE_L2) {
		cmd |= FIELD_PREP(RTL960X_TABLE_CTRL_METHOD_MASK, method);
		switch (method) {
		case RTL960X_TABLE_L2_METHOD_MAC:
			/* The key is given in the entry format. */
			write_data = true;
			addr_as_input = false;
			break;
		case RTL960X_TABLE_L2_METHOD_ADDR:
		case RTL960X_TABLE_L2_METHOD_ADDR_NEXT:
		case RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC:
		case RTL960X_TABLE_L2_METHOD_ADDR_NEXT_MC:
			break;
		case RTL960X_TABLE_L2_METHOD_ADDR_NEXT_UC_PORT:
			cmd |= FIELD_PREP(RTL960X_TABLE_CTRL_PORT_MASK, port);
			break;
		default:
			return -EINVAL;
		}
	} else if (op == RTL960X_TABLE_OP_WRITE) {
		write_data = true;
		/* L2 writes are placed by the hash of the key. */
		if (table == RTL960X_TABLE_L2)
			addr_as_input = false;
	}

	if (addr_as_input)
		cmd |= FIELD_PREP(RTL960X_TABLE_CTRL_ADDR_MASK, *addr);

	mutex_lock(&priv->table_lock);

	/* Protect from a busy table access (i.e. previous access timeouts) */
	ret = rtl960x_table_poll_busy(priv);
	if (ret)
		goto out;

	if (write_data) {
		for (i = 0; i < size; i++) {
			ret = regmap_write(priv->map, RTL960X_TABLE_WRITE_REG(i),
					   data[i]);
			if (ret)
				goto out;
		}
	}

	ret = regmap_write(priv->map, RTL960X_TABLE_CTRL_REG, cmd);
	if (ret)
		goto out;

	ret = rtl960x_table_poll_busy(priv);
	if (ret)
		goto out;

	/* For both reads and writes to the L2 table, check status */
	if (table == RTL960X_TABLE_L2) {
		ret = regmap_read(priv->map, RTL960X_TABLE_STATUS_REG, &val);
		if (ret)
			goto out;

		if (!FIELD_GET(RTL960X_TABLE_STATUS_HIT_STATUS_MASK, val)) {
			ret = -ENOENT;
			goto out;
		}

		/* The CAM entries follow the hash entries in the address space. */
		*addr = FIELD_GET(RTL960X_TABLE_STATUS_ADDRESS_MASK, val) |
			FIELD_GET(RTL960X_TABLE_STATUS_ADDR_TYPE_MASK, val) << 11;
	}

	if (op == RTL960X_TABLE_OP_READ) {
		for (i = 0; i < size; i++) {
			ret = regmap_read(priv->map, RTL960X_TABLE_READ_REG(i),
					  &data[i]);
			if (ret)
				goto out;
		}
	}

out:
	mutex_unlock(&priv->table_lock);

	return ret;
}
