/*
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "adpt.h"

#define OP_TYPE_ADD	0x0
#define OP_TYPE_DEL	0x1
#define OP_TYPE_GET	0x2
#define OP_TYPE_FLUSH	0x4
#define OP_TYPE_AGE	0x5

#define OP_MODE_HASH	0x0
#define OP_MODE_INDEX	0x1

#define OP_CMD_ID_SIZE	0xf

#define OP_FIFO_CNT_SIZE	0x8

#define ARL_FIRST_ENTRY		0x0
#define ARL_NEXT_ENTRY		0x1
#define ARL_EXTENDFIRST_ENTRY	0x2
#define ARL_EXTENDNEXT_ENTRY	0x3


static aos_lock_t hppe_fdb_lock;

/*
 * Remove port type.
 * Current fal_port_t format:
 * 1) highest 8 bits is defined as port type.
 * 2) lowest 24 bits for port id value.
 * The range of physical port id is 0-7, the range of trunk id is 32-33, and the range of virtual port is 64-255.
 * Port type: 0 is physical port, 1 is trunk port, 2 is virtual port.
 */
sw_error_t
_remove_port_type(fal_port_t * port_id)
{
	*port_id = FAL_PORT_ID_VALUE(*port_id);
	return SW_OK;
}

/*
 * Add port type.
 * Current fal_port_t format:
 * 1) highest 8 bits is defined as port type.
 * 2) lowest 24 bits for port id value.
 * The range of physical port id is 0-7, the range of trunk id is 32-33, and the range of virtual port is 64-255.
 * Port type: 0 is physical port, 1 is trunk port, 2 is virtual port.
 */
sw_error_t
_add_port_type(a_bool_t bitmap, fal_port_t * port_id)
{
	if (bitmap == A_TRUE)
		return SW_OK;

	if (*port_id == 32 || *port_id == 33)
		*port_id = FAL_PORT_ID(FAL_PORT_TYPE_TRUNK, *port_id);
	else if (*port_id >= 64)
		*port_id = FAL_PORT_ID(FAL_PORT_TYPE_VPORT, *port_id);
	return SW_OK;
}

/*
 * set values to register FDB_TBL_OP
 */
sw_error_t
_adpt_hppe_fdb_tbl_op_reg_set(a_uint32_t dev_id, a_uint32_t cmd_id, a_uint32_t op_type)
{
	sw_error_t rv = SW_OK;
	union fdb_tbl_op_u reg_val_op;
	a_uint32_t op_mode = OP_MODE_HASH;
	a_uint32_t entry_index = 0x0;

	reg_val_op.bf.cmd_id = cmd_id;
	reg_val_op.bf.byp_rslt_en = 0x0;
	reg_val_op.bf.op_type = op_type;
	reg_val_op.bf.hash_block_bitmap = 0x3;
	reg_val_op.bf.op_mode = op_mode;
	reg_val_op.bf.entry_index = entry_index;
	rv = hppe_fdb_tbl_op_set(dev_id, &reg_val_op);

	return rv;
}

/*
 * get results from register FDB_TBL_OP_RSLT
 */
sw_error_t
_adpt_hppe_fdb_tbl_op_rslt_reg_get(a_uint32_t dev_id, a_uint32_t cmd_id)
{
	sw_error_t rv = SW_OK;
	union fdb_tbl_op_rslt_u reg_val_op_rslt;

	rv = hppe_fdb_tbl_op_rslt_get(dev_id, &reg_val_op_rslt);

	if (rv != SW_OK || reg_val_op_rslt.bf.cmd_id != cmd_id || reg_val_op_rslt.bf.valid_cnt > OP_FIFO_CNT_SIZE)
		return SW_FAIL;

	return SW_OK;
}

/*
 * set values to register FDB_TBL_RD_OP
 */
sw_error_t
_adpt_hppe_fdb_tbl_rd_op_reg_set(a_uint32_t dev_id, a_uint32_t cmd_id, a_uint32_t op_mode, a_uint32_t entry_index)
{
	sw_error_t rv = SW_OK;
	union fdb_tbl_rd_op_u reg_val_rd_op;
	a_uint32_t op_type = OP_TYPE_GET;

	reg_val_rd_op.bf.cmd_id = cmd_id;
	reg_val_rd_op.bf.byp_rslt_en = 0x0;
	reg_val_rd_op.bf.op_type = op_type;
	reg_val_rd_op.bf.hash_block_bitmap = 0x3;
	reg_val_rd_op.bf.op_mode = op_mode;
	reg_val_rd_op.bf.entry_index = entry_index;
	rv = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val_rd_op);

	return rv;
}

/*
 * get results from register FDB_TBL_RD_OP_RSLT
 */
sw_error_t
_adpt_hppe_fdb_tbl_rd_op_rslt_reg_get(a_uint32_t dev_id, a_uint32_t cmd_id, a_uint32_t *entry_index)
{
	sw_error_t rv = SW_OK;
	union fdb_tbl_rd_op_rslt_u reg_val_rd_op_rslt;

	rv = hppe_fdb_tbl_rd_op_rslt_get(dev_id, &reg_val_rd_op_rslt);

	if (rv != SW_OK || reg_val_rd_op_rslt.bf.cmd_id != cmd_id ||
		reg_val_rd_op_rslt.bf.valid_cnt > OP_FIFO_CNT_SIZE)
	{
		return SW_FAIL;
	}

	*entry_index = reg_val_rd_op_rslt.bf.entry_index;

	return SW_OK;
}

/*
 * set values to register FDB_TBL_OP_DATA0/FDB_TBL_OP_DATA1/FDB_TBL_OP_DATA2
 */
sw_error_t
_adpt_hppe_fdb_tbl_op_data_reg_set(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t i, port_value, dst_type = 0x0;
	a_uint32_t reg_value[3] = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	for (i = 2; i < 6; i++)
	{
		reg_value[0] = (reg_value[0] << 8) + entry->addr.uc[i];
	}
	rv = hppe_fdb_tbl_op_data0_data_set(dev_id, reg_value[0]);
	if (rv != SW_OK)
		return rv;

	for (i = 0; i < 2; i++)
	{
		reg_value[1] = (reg_value[1] << 8) + entry->addr.uc[i];
	}
	reg_value[1] += (0x1 << (FDB_TBL_ENTRY_VALID_OFFSET - 32)) + (0x1 << (FDB_TBL_LOOKUP_VALID_OFFSET -32)) +
			(entry->fid << (FDB_TBL_VSI_OFFSET - 32));
	if (entry->portmap_en == A_TRUE)
	{
		port_value = entry->port.map;
		dst_type = 0x3;
	}
	else
	{
		port_value = entry->port.id;
		dst_type = 0x2;
	}
	reg_value[1] += ((port_value & 0x1ff) << (FDB_TBL_DST_INFO_OFFSET - 32));
	rv = hppe_fdb_tbl_op_data1_data_set(dev_id, reg_value[1]);
	if (rv != SW_OK)
		return rv;

	reg_value[2] = ((port_value >> 0x9) & 0x7) + ((dst_type & 0x3) << 3) +
			((entry->sacmd & 0x3) << (FDB_TBL_SA_CMD_OFFSET - 64)) +
			((entry->dacmd & 0x3) << (FDB_TBL_DA_CMD_OFFSET - 64));
	if (entry->static_en == A_TRUE)
		reg_value[2] += (0x3 << (FDB_TBL_HIT_AGE_OFFSET - 64));
	else
		reg_value[2] += (0x2 << (FDB_TBL_HIT_AGE_OFFSET - 64));
	rv = hppe_fdb_tbl_op_data2_data_set(dev_id, reg_value[2]);
	if (rv != SW_OK)
		return rv;

	return SW_OK;
}

/*
 * set values to register FDB_TBL_RD_OP_DATA0/FDB_TBL_RD_OP_DATA1/FDB_TBL_RD_OP_DATA2
 */
sw_error_t
_adpt_hppe_fdb_tbl_rd_op_data_reg_set(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	sw_error_t rv = SW_OK;
	a_uint32_t i, reg_value[3] = {0};

	for (i = 2; i < 6; i++)
	{
		reg_value[0] = (reg_value[0] << 8) + entry->addr.uc[i];
	}
	rv = hppe_fdb_tbl_rd_op_data0_data_set(dev_id, reg_value[0]);
	if (rv != SW_OK)
		return rv;

	for (i = 0; i < 2; i++)
	{
		reg_value[1] = (reg_value[1] << 8) + entry->addr.uc[i];
	}
	reg_value[1] += (entry->fid << (FDB_TBL_VSI_OFFSET - 32));
	rv = hppe_fdb_tbl_rd_op_data1_data_set(dev_id, reg_value[1]);
	if (rv != SW_OK)
		return rv;

	rv = hppe_fdb_tbl_rd_op_data2_data_set(dev_id, reg_value[2]);
	if (rv != SW_OK)
		return rv;

	return SW_OK;
}

/*
 * get values from register FDB_TBL_RD_OP_RSLT_DATA0/FDB_TBL_RD_OP_RSLT_DATA1/FDB_TBL_RD_OP_RSLT_DATA2
 */
sw_error_t
_adpt_hppe_fdb_tbl_rd_op_rslt_data_reg_get(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	a_uint32_t rslt_data[3];
	a_uint32_t entry_valid, lookup_valid;
	a_uint32_t i, dst_info_encode;

	hppe_fdb_tbl_rd_op_rslt_data0_data_get(dev_id, &rslt_data[0]);
	hppe_fdb_tbl_rd_op_rslt_data1_data_get(dev_id, &rslt_data[1]);
	hppe_fdb_tbl_rd_op_rslt_data2_data_get(dev_id, &rslt_data[2]);

	entry_valid = (rslt_data[1] >> (FDB_TBL_ENTRY_VALID_OFFSET - 32)) & 0x1;
	lookup_valid = (rslt_data[1] >> (FDB_TBL_LOOKUP_VALID_OFFSET - 32)) & 0x1;
	dst_info_encode = (rslt_data[2] >> (FDB_TBL_DST_INFO_OFFSET + 12 -64)) & 0x3;

	if (entry_valid == 0x0 || dst_info_encode == 0x0)
	{
		return SW_NOT_FOUND;
	}
	else
	{
		entry->entry_valid = A_TRUE;
		if (lookup_valid == 0x1)
			entry->lookup_valid = A_TRUE;
		else
			entry->lookup_valid = A_FALSE;
		entry->fid = (rslt_data[1] >> (FDB_TBL_VSI_OFFSET - 32)) & 0x1f;
		entry->sacmd = (rslt_data[2] >> (FDB_TBL_SA_CMD_OFFSET - 64)) & 0x3;
		entry->dacmd = (rslt_data[2] >> (FDB_TBL_DA_CMD_OFFSET - 64)) & 0x3;
		if (((rslt_data[2] >> (FDB_TBL_HIT_AGE_OFFSET - 64)) & 0x3) == 0x3)
			entry->static_en = A_TRUE;
		else
			entry->static_en = A_FALSE;
		if (dst_info_encode == 0x2)
		{
			entry->portmap_en = A_FALSE;
			entry->port.id = ((rslt_data[2] & 0x7) << 9) + ((rslt_data[1] >> (FDB_TBL_DST_INFO_OFFSET - 32)) & 0x1ff);
		}
		else
		{
			entry->portmap_en = A_TRUE;
			entry->port.map = ((rslt_data[2] & 0x7) << 9) + ((rslt_data[1] >> (FDB_TBL_DST_INFO_OFFSET - 32)) & 0x1ff);
		}
		for (i = 2; i < 6; i++)
			entry->addr.uc[i] = (rslt_data[0] >> ((5 - i) << 3)) & 0xff;
		for (i = 0; i < 2; i++)
			entry->addr.uc[i] = (rslt_data[1] >> ((1 - i) << 3)) & 0xff;
	}

	return SW_OK;
}

sw_error_t
_get_fdb_table_entryindex_by_entry(a_uint32_t dev_id, fal_fdb_entry_t * entry,
		a_uint32_t *entry_index, a_uint32_t cmd_id)
{
	sw_error_t rv = SW_OK;
	a_uint32_t init_entry_index = 0;
	fal_fdb_entry_t temp_entry;

	aos_lock_bh(&hppe_fdb_lock);
	rv = _adpt_hppe_fdb_tbl_rd_op_data_reg_set(dev_id, entry);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_rd_op_reg_set(dev_id, cmd_id, OP_MODE_HASH, init_entry_index);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_rd_op_rslt_data_reg_get(dev_id, &temp_entry);

	rv = _adpt_hppe_fdb_tbl_rd_op_rslt_reg_get(dev_id, cmd_id, entry_index);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	aos_unlock_bh(&hppe_fdb_lock);

	if (*entry_index == 0)
	{
		if (!(temp_entry.addr.uc[0] == entry->addr.uc[0] && temp_entry.addr.uc[1] == entry->addr.uc[1] &&
			temp_entry.addr.uc[2] == entry->addr.uc[2] && temp_entry.addr.uc[3] == entry->addr.uc[3] &&
			temp_entry.addr.uc[4] == entry->addr.uc[4] && temp_entry.addr.uc[5] == entry->addr.uc[5] &&
			temp_entry.fid == entry->fid))
			return SW_NOT_FOUND;
	}

	return SW_OK;
}

sw_error_t
_get_fdb_table_entry_by_entryindex(a_uint32_t dev_id, fal_fdb_entry_t * entry,
		a_uint32_t entry_index, a_uint32_t cmd_id)
{
	sw_error_t rv = SW_OK, rv1 = SW_OK;
	fal_fdb_entry_t init_entry;
	a_uint32_t rslt_entry_index = 0;

	aos_mem_zero(&init_entry, sizeof (fal_fdb_entry_t));

	aos_lock_bh(&hppe_fdb_lock);
	rv = _adpt_hppe_fdb_tbl_rd_op_data_reg_set(dev_id, &init_entry);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_rd_op_reg_set(dev_id, cmd_id, OP_MODE_INDEX, entry_index);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_rd_op_rslt_data_reg_get(dev_id, entry);
	if (rv != SW_OK && rv != SW_NOT_FOUND)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}
	else
		rv1 = rv;

	rv = _adpt_hppe_fdb_tbl_rd_op_rslt_reg_get(dev_id, cmd_id, &rslt_entry_index);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	aos_unlock_bh(&hppe_fdb_lock);

	return rv1;
}

sw_error_t
_modify_fdb_table_entry(a_uint32_t dev_id, fal_fdb_entry_t * entry, a_uint32_t op_type,
	a_uint32_t cmd_id)
{
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t temp_entry;

	aos_lock_bh(&hppe_fdb_lock);
	rv = _adpt_hppe_fdb_tbl_op_data_reg_set(dev_id, entry);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_op_reg_set(dev_id, cmd_id, op_type);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	rv = _adpt_hppe_fdb_tbl_rd_op_rslt_data_reg_get(dev_id, &temp_entry);

	rv = _adpt_hppe_fdb_tbl_op_rslt_reg_get(dev_id, cmd_id);
	if (rv != SW_OK)
	{
		aos_unlock_bh(&hppe_fdb_lock);
		return rv;
	}

	aos_unlock_bh(&hppe_fdb_lock);

	return SW_OK;
}

sw_error_t
_adpt_hppe_fdb_extend_first_next(a_uint32_t dev_id, fal_fdb_entry_t * entry, fal_fdb_op_t * option, a_uint32_t hwop)
{
	sw_error_t rv;
	a_uint32_t cmd_id = 0x0;
	a_uint32_t entry_index = 0x0;
	fal_fdb_entry_t ori_entry;

	aos_mem_zero(&ori_entry, sizeof (fal_fdb_entry_t));

	if (hwop == ARL_EXTENDFIRST_ENTRY || hwop == ARL_EXTENDNEXT_ENTRY)
	{
		ori_entry.portmap_en = entry->portmap_en;
		ori_entry.port.id = entry->port.id;
		ori_entry.fid = entry->fid;
	}

	if (hwop == ARL_FIRST_ENTRY || hwop == ARL_EXTENDFIRST_ENTRY)
		aos_mem_zero(entry, sizeof (fal_fdb_entry_t));

	rv = _get_fdb_table_entryindex_by_entry(dev_id, entry, &entry_index, cmd_id);
	if (rv != SW_OK && rv != SW_NOT_FOUND)
		return rv;

	if (rv != SW_NOT_FOUND)
		entry_index += 1;

	for (; entry_index < FDB_TBL_NUM; entry_index++)
	{
		cmd_id = entry_index % OP_CMD_ID_SIZE;
		rv = _get_fdb_table_entry_by_entryindex(dev_id, entry, entry_index, cmd_id);
		if (rv == SW_NOT_FOUND)
			continue;
		else if (rv == SW_OK)
		{
			if (hwop == ARL_EXTENDFIRST_ENTRY || hwop == ARL_EXTENDNEXT_ENTRY)
			{
				if (option->fid_en == A_TRUE && ori_entry.fid != entry->fid)
					continue;
				if (option->port_en == A_TRUE && !(ori_entry.portmap_en == entry->portmap_en &&
					ori_entry.port.id == entry->port.id))
					continue;
			}
			break;
		}
		else
			return rv;
	}

	if (entry_index == FDB_TBL_NUM)
		return SW_NO_MORE;
	return SW_OK;
}

void _fdb_copy(fal_fdb_entry_t *new, const fal_fdb_entry_t *old)
{
	a_uint32_t i;

	aos_mem_zero(new, sizeof (fal_fdb_entry_t));

	for (i = 0; i < 6; i++)
		new->addr.uc[i] = old->addr.uc[i];

	new->fid = old->fid;
	new->dacmd = old->dacmd;
	new->sacmd = old->sacmd;
	new->port.id = old->port.id;
	new->portmap_en = old->portmap_en;
	new->is_multicast = old->is_multicast;
	new->static_en = old->static_en;

}

sw_error_t
adpt_hppe_fdb_first(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	fal_fdb_op_t option;
	sw_error_t rv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	aos_mem_zero(&option, sizeof (fal_fdb_op_t));

	_remove_port_type(&entry->port.id);
	rv = _adpt_hppe_fdb_extend_first_next(dev_id, entry, &option, ARL_FIRST_ENTRY);
	_add_port_type(entry->portmap_en, &entry->port.id);

	return rv;
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_next(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	fal_fdb_op_t option;
	sw_error_t rv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	aos_mem_zero(&option, sizeof (fal_fdb_op_t));

	_remove_port_type(&entry->port.id);
	rv = _adpt_hppe_fdb_extend_first_next(dev_id, entry, &option, ARL_NEXT_ENTRY);
	_add_port_type(entry->portmap_en, &entry->port.id);

	return rv;
}
#endif
sw_error_t
adpt_hppe_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry_temp;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	_fdb_copy(&entry_temp, entry);

	_remove_port_type(&entry_temp.port.id);
	rv = _modify_fdb_table_entry(dev_id, &entry_temp, OP_TYPE_ADD, 0x0);
	_add_port_type(entry_temp.portmap_en, &entry_temp.port.id);

	return rv;
}

sw_error_t
adpt_hppe_fdb_del_by_mac(a_uint32_t dev_id, const fal_fdb_entry_t *entry)
{
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry_temp;
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	_fdb_copy(&entry_temp, entry);
	_remove_port_type(&entry_temp.port.id);
	rv = _modify_fdb_table_entry(dev_id, &entry_temp, OP_TYPE_DEL, 0x0);
	_add_port_type(entry_temp.portmap_en, &entry_temp.port.id);

	return rv;
}

sw_error_t
adpt_hppe_fdb_del_by_port(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag)
{
	a_uint32_t entry_index, id, cmd_id = 0;
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry;

	ADPT_DEV_ID_CHECK(dev_id);

	_remove_port_type(&port_id);

	for (entry_index = 0; entry_index < FDB_TBL_NUM; entry_index++)
	{
		cmd_id = entry_index % OP_CMD_ID_SIZE;
		aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));
		rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);

		if (rv == SW_NOT_FOUND)
			continue;
		else if (rv == SW_OK)
		{
			if (entry.portmap_en == A_TRUE)
			{
				if (((entry.port.map >> port_id) & 0x1) == 1)
				{
					if ((!flag && entry.static_en == A_FALSE) || (flag & FAL_FDB_DEL_STATIC))
					{
						id = entry.port.map & (~(0x1 << port_id));
						if (id == 0)
						{
							rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
							if (rv != SW_OK)
								return rv;
						}
						else
						{
							entry.port.map &= (~(0x1 << port_id));
							rv = adpt_hppe_fdb_add(dev_id, &entry);
							if (rv != SW_OK)
								return rv;
						}
					}
				}
			}
			else
			{
				if (entry.port.id == port_id &&
					((!flag && entry.static_en == A_FALSE) || (flag & FAL_FDB_DEL_STATIC)))
				{
					rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
					if (rv != SW_OK)
						return rv;
				}
			}
		}
		else
			return rv;
	}

	return SW_OK;
}

sw_error_t
adpt_hppe_fdb_del_all(a_uint32_t dev_id, a_uint32_t flag)
{
	sw_error_t rv;
	fal_fdb_entry_t entry;
	a_uint32_t entry_index = 0, cmd_id;

	ADPT_DEV_ID_CHECK(dev_id);

	if (FAL_FDB_DEL_STATIC & flag)
	{
		aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));
		return _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_FLUSH, 0x0);
	}
	else
	{
		for (entry_index = 0; entry_index < FDB_TBL_NUM; entry_index++)
		{
			cmd_id = entry_index % OP_CMD_ID_SIZE;
			aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));
			rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);
			if (rv != SW_OK && rv != SW_NOT_FOUND)
				return rv;

			if (entry.static_en == A_FALSE)
			{
				rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
				if (rv != SW_OK)
					return rv;
			}
		}
	}

	return SW_OK;
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_transfer(a_uint32_t dev_id, fal_port_t old_port, fal_port_t new_port,
                     a_uint32_t fid, fal_fdb_op_t * option)
{
	a_uint32_t entry_index, cmd_id = 0;
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(option);

	if (option->port_en == A_TRUE)
		return SW_NOT_SUPPORTED;

	_remove_port_type(&old_port);
	_remove_port_type(&new_port);

	for (entry_index = 0; entry_index < FDB_TBL_NUM; entry_index++)
	{
		cmd_id = entry_index % OP_CMD_ID_SIZE;
		aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));
		rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);

		if (rv == SW_NOT_FOUND)
			continue;
		else if (rv == SW_OK)
		{
			if (option->fid_en == A_TRUE && entry.fid != fid)
				continue;
			if (entry.portmap_en == A_TRUE)
			{
				if (((entry.port.map >> old_port) & 0x1) == 1)
				{
					entry.port.map &= (~(0x1 << old_port));
					entry.port.map |= (0x1 << new_port);
					rv = adpt_hppe_fdb_add(dev_id, &entry);
					if (rv != SW_OK)
						return rv;
				}
			}
			else
			{
				if (entry.port.id == old_port)
				{
					entry.port.id = new_port;
					rv = adpt_hppe_fdb_add(dev_id, &entry);
					if (rv != SW_OK)
						return rv;
				}
			}
		}
		else
			return rv;
	}

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_find(a_uint32_t dev_id, fal_fdb_entry_t * entry)
{
	sw_error_t rv;
	a_uint32_t cmd_id = 0x0;
	a_uint32_t entry_index = 0x0;

	_remove_port_type(&entry->port.id);
	rv = _get_fdb_table_entryindex_by_entry(dev_id, entry, &entry_index, cmd_id);
	if (rv != SW_OK)
		return rv;

	cmd_id = entry_index % OP_CMD_ID_SIZE;

	rv = _get_fdb_table_entry_by_entryindex(dev_id, entry, entry_index, cmd_id);
	_add_port_type(entry->portmap_en, &entry->port.id);

	return rv;
}

sw_error_t
adpt_hppe_fdb_iterate(a_uint32_t dev_id, a_uint32_t * iterator, fal_fdb_entry_t * entry)
{
	sw_error_t rv;
	a_uint32_t cmd_id = 0x0;
	a_uint32_t entry_index = 0x0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(iterator);
	ADPT_NULL_POINT_CHECK(entry);

	_remove_port_type(&entry->port.id);
	for (entry_index = *iterator; entry_index < FDB_TBL_NUM; entry_index++)
	{
		cmd_id = entry_index % OP_CMD_ID_SIZE;
		rv = _get_fdb_table_entry_by_entryindex(dev_id, entry, entry_index, cmd_id);
		if (rv == SW_NOT_FOUND)
			continue;
		else if (rv == SW_OK)
			break;
		else
			return rv;
	}

	if (entry_index == FDB_TBL_NUM)
		return SW_NO_MORE;

	_add_port_type(entry->portmap_en, &entry->port.id);
	*iterator = entry_index + 1;
	return SW_OK;
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_age_time_set(a_uint32_t dev_id, a_uint32_t * time)
{
	union age_timer_u age_timer = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	if (*time < 1 || *time > 1048575)
		return SW_BAD_PARAM;

	age_timer.bf.age_val = *time;

	return hppe_age_timer_set(dev_id, &age_timer);
}

sw_error_t
adpt_hppe_fdb_age_time_get(a_uint32_t dev_id, a_uint32_t * time)
{
	sw_error_t rv = SW_OK;
	union age_timer_u age_timer = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(time);

	rv = hppe_age_timer_get(dev_id, &age_timer);

	if( rv != SW_OK )
		return rv;

	*time = age_timer.bf.age_val;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_extend_first(a_uint32_t dev_id, fal_fdb_op_t * option,
                         fal_fdb_entry_t * entry)
{
	sw_error_t rv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(option);
	ADPT_NULL_POINT_CHECK(entry);

	_remove_port_type(&entry->port.id);
	rv = _adpt_hppe_fdb_extend_first_next(dev_id, entry, option, ARL_EXTENDFIRST_ENTRY);
	_add_port_type(entry->portmap_en, &entry->port.id);

	return rv;
}

sw_error_t
adpt_hppe_fdb_extend_next(a_uint32_t dev_id, fal_fdb_op_t * option,
                        fal_fdb_entry_t * entry)
{
	sw_error_t rv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(option);
	ADPT_NULL_POINT_CHECK(entry);

	_remove_port_type(&entry->port.id);
	rv = _adpt_hppe_fdb_extend_first_next(dev_id, entry, option, ARL_EXTENDNEXT_ENTRY);
	_add_port_type(entry->portmap_en, &entry->port.id);

	return rv;
}

sw_error_t
adpt_hppe_fdb_learn_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);

	if( rv != SW_OK )
		return rv;

	l2_global_conf.bf.lrn_en = enable;

	return hppe_l2_global_conf_set(dev_id, &l2_global_conf);
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_learn_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);


	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);

	if( rv != SW_OK )
		return rv;

	*enable = l2_global_conf.bf.lrn_en;

	return SW_OK;
}

sw_error_t
adpt_hppe_fdb_port_maclimit_ctrl_set(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
	sw_error_t rv = SW_OK;
	union port_lrn_limit_ctrl_u port_lrn_limit_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_lrn_limit_ctrl_get(dev_id, port_id, &port_lrn_limit_ctrl);

	if( rv != SW_OK )
		return rv;

	port_lrn_limit_ctrl.bf.lrn_lmt_en = maclimit_ctrl->enable;
	port_lrn_limit_ctrl.bf.lrn_lmt_cnt = maclimit_ctrl->limit_num;
	port_lrn_limit_ctrl.bf.lrn_lmt_exceed_fwd = maclimit_ctrl->action;

	return hppe_port_lrn_limit_ctrl_set(dev_id, port_id, &port_lrn_limit_ctrl);
}

sw_error_t
adpt_hppe_fdb_port_maclimit_ctrl_get(a_uint32_t dev_id, fal_port_t port_id, fal_maclimit_ctrl_t * maclimit_ctrl)
{
	sw_error_t rv = SW_OK;
	union port_lrn_limit_ctrl_u port_lrn_limit_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(maclimit_ctrl);

	rv = hppe_port_lrn_limit_ctrl_get(dev_id, port_id, &port_lrn_limit_ctrl);

	if( rv != SW_OK )
		return rv;

	maclimit_ctrl->enable = port_lrn_limit_ctrl.bf.lrn_lmt_en;
	maclimit_ctrl->limit_num = port_lrn_limit_ctrl.bf.lrn_lmt_cnt;
	maclimit_ctrl->action = port_lrn_limit_ctrl.bf.lrn_lmt_exceed_fwd;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_fdb_learn_limit_set(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable, a_uint32_t cnt)
{
	sw_error_t rv = SW_OK;
	fal_maclimit_ctrl_t maclimit_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);

	rv = adpt_hppe_fdb_port_maclimit_ctrl_get(dev_id, port_id, &maclimit_ctrl);
	if( rv != SW_OK )
		return rv;

	maclimit_ctrl.enable = enable;
	maclimit_ctrl.limit_num = cnt;

	return adpt_hppe_fdb_port_maclimit_ctrl_set(dev_id, port_id, &maclimit_ctrl);
}

sw_error_t
adpt_hppe_port_fdb_learn_limit_get(a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t * enable, a_uint32_t * cnt)
{
	sw_error_t rv = SW_OK;
	fal_maclimit_ctrl_t maclimit_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
	ADPT_NULL_POINT_CHECK(cnt);

	rv = adpt_hppe_fdb_port_maclimit_ctrl_get(dev_id, port_id, &maclimit_ctrl);
	if( rv != SW_OK )
		return rv;

	*enable = maclimit_ctrl.enable;
	*cnt = maclimit_ctrl.limit_num;

	return SW_OK;
}

sw_error_t
adpt_hppe_fdb_port_add(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry;
	int i, cmd_id = 0x0, entry_index = 0x0, id;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(addr);

	_remove_port_type(&port_id);

	aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));

	entry.fid = fid;
	for (i = 0; i < 6; i++)
		entry.addr.uc[i] = addr->uc[i];

	rv = _get_fdb_table_entryindex_by_entry(dev_id, &entry, &entry_index, cmd_id);
	if (rv != SW_OK)
		return rv;

	cmd_id = entry_index % OP_CMD_ID_SIZE;
	rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);
	if (rv != SW_OK)
		return rv;

	if (entry.portmap_en == A_TRUE)
	{
		if (port_id >= 12)
			return SW_FAIL;
		entry.port.map |= (0x1 << port_id);
	}
	else
	{
		if (port_id >= 12 || entry.port.id >= 12)
			return SW_FAIL;
		entry.portmap_en = A_TRUE;
		id = entry.port.id;
		entry.port.map = 0;
		entry.port.map |= (0x1 << id);
		entry.port.map |= (0x1 << port_id);
	}

	return adpt_hppe_fdb_add(dev_id, &entry);
}

sw_error_t
adpt_hppe_fdb_port_del(a_uint32_t dev_id, a_uint32_t fid, fal_mac_addr_t * addr, fal_port_t port_id)
{
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry;
	int i, cmd_id = 0x0, entry_index = 0x0, id;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(addr);

	_remove_port_type(&port_id);

	aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));

	entry.fid = fid;
	for (i = 0; i < 6; i++)
		entry.addr.uc[i] = addr->uc[i];

	rv = _get_fdb_table_entryindex_by_entry(dev_id, &entry, &entry_index, cmd_id);
	if (rv != SW_OK)
		return rv;

	cmd_id = entry_index % OP_CMD_ID_SIZE;
	rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);
	if (rv != SW_OK)
		return rv;

	if (entry.portmap_en == A_TRUE)
	{
		if (((entry.port.map >> port_id) & 0x1) == 1)
		{
			id = entry.port.map & (~(0x1 << port_id));
			if (id == 0)
			{
				rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
			}
			else
			{
				entry.port.map &= (~(0x1 << port_id));
				rv = adpt_hppe_fdb_add(dev_id, &entry);
			}
		}
	}
	else
	{
		if (entry.port.id == port_id)
			rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
	}

	return rv;
}
#endif
sw_error_t
adpt_hppe_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	port_bridge_ctrl.bf.new_addr_lrn_en = enable;
	port_bridge_ctrl.bf.station_move_lrn_en = enable;

	return hppe_port_bridge_ctrl_set(dev_id, port_id, &port_bridge_ctrl);
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_port_learn_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	*enable = port_bridge_ctrl.bf.new_addr_lrn_en;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_port_newaddr_lrn_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl;

	memset(&port_bridge_ctrl, 0, sizeof(port_bridge_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	port_bridge_ctrl.bf.new_addr_lrn_en = enable;
	port_bridge_ctrl.bf.new_addr_fwd_cmd = cmd;

	return hppe_port_bridge_ctrl_set(dev_id, port_id, &port_bridge_ctrl);
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_port_newaddr_lrn_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl;

	memset(&port_bridge_ctrl, 0, sizeof(port_bridge_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
	ADPT_NULL_POINT_CHECK(cmd);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	*enable = port_bridge_ctrl.bf.new_addr_lrn_en;
	*cmd = port_bridge_ctrl.bf.new_addr_fwd_cmd;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_port_stamove_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable, fal_fwd_cmd_t cmd)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl;

	memset(&port_bridge_ctrl, 0, sizeof(port_bridge_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	port_bridge_ctrl.bf.station_move_lrn_en = enable;
	port_bridge_ctrl.bf.station_move_fwd_cmd = cmd;

	return hppe_port_bridge_ctrl_set(dev_id, port_id, &port_bridge_ctrl);
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_port_stamove_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable, fal_fwd_cmd_t *cmd)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl;

	memset(&port_bridge_ctrl, 0, sizeof(port_bridge_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
	ADPT_NULL_POINT_CHECK(cmd);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	*enable = port_bridge_ctrl.bf.station_move_lrn_en;
	*cmd = port_bridge_ctrl.bf.station_move_fwd_cmd;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_fdb_learn_counter_get(a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t * cnt)
{
	sw_error_t rv = SW_OK;
	union port_lrn_limit_counter_u port_lrn_limit_counter = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cnt);

	rv = hppe_port_lrn_limit_counter_get(dev_id, port_id, &port_lrn_limit_counter);

	if( rv != SW_OK )
		return rv;

	*cnt = port_lrn_limit_counter.bf.lrn_cnt;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_fdb_learn_exceed_cmd_set(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t cmd)
{
	sw_error_t rv = SW_OK;
	fal_maclimit_ctrl_t maclimit_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);

	rv = adpt_hppe_fdb_port_maclimit_ctrl_get(dev_id, port_id, &maclimit_ctrl);
	if( rv != SW_OK )
		return rv;

	maclimit_ctrl.action = cmd;

	return adpt_hppe_fdb_port_maclimit_ctrl_set(dev_id, port_id, &maclimit_ctrl);
}

sw_error_t
adpt_hppe_port_fdb_learn_exceed_cmd_get(a_uint32_t dev_id, fal_port_t port_id,
                                      fal_fwd_cmd_t * cmd)
{
	sw_error_t rv = SW_OK;
	fal_maclimit_ctrl_t maclimit_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cmd);

	rv = adpt_hppe_fdb_port_maclimit_ctrl_get(dev_id, port_id, &maclimit_ctrl);
	if( rv != SW_OK )
		return rv;

	*cmd = maclimit_ctrl.action;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_age_ctrl_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);

	if( rv != SW_OK )
		return rv;

	l2_global_conf.bf.age_en = enable;

	return hppe_l2_global_conf_set(dev_id, &l2_global_conf);
}
#ifndef IN_FDB_MINI
sw_error_t
adpt_hppe_fdb_age_ctrl_get(a_uint32_t dev_id, a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	union l2_global_conf_u l2_global_conf = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_l2_global_conf_get(dev_id, &l2_global_conf);

	if( rv != SW_OK )
		return rv;

	*enable = l2_global_conf.bf.age_en;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_fdb_del_by_fid(a_uint32_t dev_id, a_uint16_t fid, a_uint32_t flag)
{
	a_uint32_t entry_index, cmd_id = 0;
	sw_error_t rv = SW_OK;
	fal_fdb_entry_t entry;

	ADPT_DEV_ID_CHECK(dev_id);

	for (entry_index = 0; entry_index < FDB_TBL_NUM; entry_index++)
	{
		cmd_id = entry_index % OP_CMD_ID_SIZE;
		aos_mem_zero(&entry, sizeof (fal_fdb_entry_t));
		rv = _get_fdb_table_entry_by_entryindex(dev_id, &entry, entry_index, cmd_id);

		if (rv == SW_NOT_FOUND)
		{
			continue;
		}
		else if (rv == SW_OK)
		{
			if (entry.fid == fid &&
				((!flag && entry.static_en == A_FALSE) || (flag & FAL_FDB_DEL_STATIC)))
			{
				rv = _modify_fdb_table_entry(dev_id, &entry, OP_TYPE_DEL, 0x0);
				if (rv != SW_OK)
				{
					return rv;
				}
			}
		}
		else
		{
			return rv;
		}
	}

	return SW_OK;
}

void adpt_hppe_fdb_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_fdb_func_bitmap[0] = ((1 << FUNC_FDB_ENTRY_ADD) |
						(1 << FUNC_FDB_ENTRY_FLUSH) |
						(1 << FUNC_FDB_ENTRY_DEL_BYPORT) |
						(1 << FUNC_FDB_ENTRY_DEL_BYMAC) |
						(1 << FUNC_FDB_ENTRY_GETFIRST) |
						(1 << FUNC_FDB_ENTRY_GETNEXT) |
						(1 << FUNC_FDB_ENTRY_SEARCH) |
						(1 << FUNC_FDB_PORT_LEARN_SET) |
						(1 << FUNC_FDB_PORT_LEARN_GET) |
						(1 << FUNC_FDB_PORT_LEARNING_CTRL_SET) |
						(1 << FUNC_FDB_PORT_LEARNING_CTRL_GET) |
						(1 << FUNC_FDB_PORT_STAMOVE_CTRL_SET) |
						(1 << FUNC_FDB_PORT_STAMOVE_CTRL_GET) |
						(1 << FUNC_FDB_AGING_CTRL_SET) |
						(1 << FUNC_FDB_AGING_CTRL_GET) |
						(1 << FUNC_FDB_LEARNING_CTRL_SET) |
						(1 << FUNC_FDB_LEARNING_CTRL_GET) |
						(1 << FUNC_FDB_AGING_TIME_SET) |
						(1 << FUNC_FDB_AGING_TIME_GET) |
						(1 << FUNC_FDB_ENTRY_GETNEXT_BYINDEX) |
						(1 << FUNC_FDB_ENTRY_EXTEND_GETNEXT) |
						(1 << FUNC_FDB_ENTRY_EXTEND_GETFIRST) |
						(1 << FUNC_FDB_ENTRY_UPDATE_BYPORT) |
						(1 << FUNC_PORT_FDB_LEARN_LIMIT_SET) |
						(1 << FUNC_PORT_FDB_LEARN_LIMIT_GET) |
						(1 << FUNC_PORT_FDB_LEARN_EXCEED_CMD_SET) |
						(1 << FUNC_PORT_FDB_LEARN_EXCEED_CMD_GET) |
						(1 << FUNC_FDB_PORT_LEARNED_MAC_COUNTER_GET) |
						(1 << FUNC_FDB_PORT_ADD) |
						(1 << FUNC_FDB_PORT_DEL) |
						(1 << FUNC_FDB_PORT_MACLIMIT_CTRL_SET) |
						(1 << FUNC_FDB_PORT_MACLIMIT_CTRL_GET));
	p_adpt_api->adpt_fdb_func_bitmap[1] = ((1 << (FUNC_FDB_DEL_BY_FID % 32)));

	return;
}

static void adpt_hppe_fdb_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_fdb_first = NULL;
	p_adpt_api->adpt_fdb_next = NULL;
	p_adpt_api->adpt_fdb_add = NULL;
	p_adpt_api->adpt_fdb_del_by_port = NULL;
	p_adpt_api->adpt_fdb_del_by_mac = NULL;
	p_adpt_api->adpt_fdb_del_all = NULL;
	p_adpt_api->adpt_fdb_transfer = NULL;
	p_adpt_api->adpt_fdb_find = NULL;
	p_adpt_api->adpt_fdb_iterate = NULL;
	p_adpt_api->adpt_fdb_age_time_set = NULL;
	p_adpt_api->adpt_fdb_age_time_get = NULL;
	p_adpt_api->adpt_fdb_extend_first = NULL;
	p_adpt_api->adpt_fdb_extend_next = NULL;
	p_adpt_api->adpt_fdb_learn_ctrl_set = NULL;
	p_adpt_api->adpt_fdb_learn_ctrl_get = NULL;
	p_adpt_api->adpt_port_fdb_learn_limit_set = NULL;
	p_adpt_api->adpt_port_fdb_learn_limit_get = NULL;
	p_adpt_api->adpt_fdb_port_add = NULL;
	p_adpt_api->adpt_fdb_port_del = NULL;
	p_adpt_api->adpt_fdb_port_learn_set = NULL;
	p_adpt_api->adpt_fdb_port_learn_get = NULL;
	p_adpt_api->adpt_fdb_port_newaddr_lrn_set = NULL;
	p_adpt_api->adpt_fdb_port_newaddr_lrn_get = NULL;
	p_adpt_api->adpt_fdb_port_stamove_set = NULL;
	p_adpt_api->adpt_fdb_port_stamove_get = NULL;
	p_adpt_api->adpt_port_fdb_learn_counter_get = NULL;
	p_adpt_api->adpt_port_fdb_learn_exceed_cmd_set = NULL;
	p_adpt_api->adpt_port_fdb_learn_exceed_cmd_get = NULL;
	p_adpt_api->adpt_fdb_age_ctrl_set = NULL;
	p_adpt_api->adpt_fdb_age_ctrl_get = NULL;
	p_adpt_api->adpt_fdb_port_maclimit_ctrl_set = NULL;
	p_adpt_api->adpt_fdb_port_maclimit_ctrl_get = NULL;
	p_adpt_api->adpt_fdb_del_by_fid = NULL;

	return;
}

sw_error_t adpt_hppe_fdb_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_fdb_func_unregister(dev_id, p_adpt_api);

	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_GETFIRST))
		p_adpt_api->adpt_fdb_first = adpt_hppe_fdb_first;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_GETNEXT))
		p_adpt_api->adpt_fdb_next = adpt_hppe_fdb_next;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_ADD))
		p_adpt_api->adpt_fdb_add = adpt_hppe_fdb_add;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_DEL_BYPORT))
		p_adpt_api->adpt_fdb_del_by_port = adpt_hppe_fdb_del_by_port;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_DEL_BYMAC))
		p_adpt_api->adpt_fdb_del_by_mac = adpt_hppe_fdb_del_by_mac;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_FLUSH))
		p_adpt_api->adpt_fdb_del_all = adpt_hppe_fdb_del_all;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_UPDATE_BYPORT))
		p_adpt_api->adpt_fdb_transfer = adpt_hppe_fdb_transfer;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_SEARCH))
		p_adpt_api->adpt_fdb_find = adpt_hppe_fdb_find;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_GETNEXT_BYINDEX))
		p_adpt_api->adpt_fdb_iterate = adpt_hppe_fdb_iterate;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_AGING_TIME_SET))
		p_adpt_api->adpt_fdb_age_time_set = adpt_hppe_fdb_age_time_set;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_AGING_TIME_GET))
		p_adpt_api->adpt_fdb_age_time_get = adpt_hppe_fdb_age_time_get;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_EXTEND_GETFIRST))
		p_adpt_api->adpt_fdb_extend_first = adpt_hppe_fdb_extend_first;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_ENTRY_EXTEND_GETNEXT))
		p_adpt_api->adpt_fdb_extend_next = adpt_hppe_fdb_extend_next;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_LEARNING_CTRL_SET))
		p_adpt_api->adpt_fdb_learn_ctrl_set = adpt_hppe_fdb_learn_ctrl_set;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_LEARNING_CTRL_GET))
		p_adpt_api->adpt_fdb_learn_ctrl_get = adpt_hppe_fdb_learn_ctrl_get;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_PORT_FDB_LEARN_LIMIT_SET))
		p_adpt_api->adpt_port_fdb_learn_limit_set = adpt_hppe_port_fdb_learn_limit_set;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_PORT_FDB_LEARN_LIMIT_GET))
		p_adpt_api->adpt_port_fdb_learn_limit_get = adpt_hppe_port_fdb_learn_limit_get;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_ADD))
		p_adpt_api->adpt_fdb_port_add = adpt_hppe_fdb_port_add;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_DEL))
		p_adpt_api->adpt_fdb_port_del = adpt_hppe_fdb_port_del;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_LEARN_SET))
		p_adpt_api->adpt_fdb_port_learn_set = adpt_hppe_fdb_port_learn_set;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_LEARN_GET))
		p_adpt_api->adpt_fdb_port_learn_get = adpt_hppe_fdb_port_learn_get;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_LEARNING_CTRL_SET))
		p_adpt_api->adpt_fdb_port_newaddr_lrn_set = adpt_hppe_fdb_port_newaddr_lrn_set;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_LEARNING_CTRL_GET))
		p_adpt_api->adpt_fdb_port_newaddr_lrn_get = adpt_hppe_fdb_port_newaddr_lrn_get;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_STAMOVE_CTRL_SET))
		p_adpt_api->adpt_fdb_port_stamove_set = adpt_hppe_fdb_port_stamove_set;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_STAMOVE_CTRL_GET))
		p_adpt_api->adpt_fdb_port_stamove_get = adpt_hppe_fdb_port_stamove_get;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_LEARNED_MAC_COUNTER_GET))
		p_adpt_api->adpt_port_fdb_learn_counter_get = adpt_hppe_port_fdb_learn_counter_get;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_PORT_FDB_LEARN_EXCEED_CMD_SET))
		p_adpt_api->adpt_port_fdb_learn_exceed_cmd_set = adpt_hppe_port_fdb_learn_exceed_cmd_set;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_PORT_FDB_LEARN_EXCEED_CMD_GET))
		p_adpt_api->adpt_port_fdb_learn_exceed_cmd_get = adpt_hppe_port_fdb_learn_exceed_cmd_get;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_AGING_CTRL_SET))
		p_adpt_api->adpt_fdb_age_ctrl_set = adpt_hppe_fdb_age_ctrl_set;
#ifndef IN_FDB_MINI
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_AGING_CTRL_GET))
		p_adpt_api->adpt_fdb_age_ctrl_get = adpt_hppe_fdb_age_ctrl_get;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_MACLIMIT_CTRL_SET))
		p_adpt_api->adpt_fdb_port_maclimit_ctrl_set = adpt_hppe_fdb_port_maclimit_ctrl_set;
	if (p_adpt_api->adpt_fdb_func_bitmap[0] & (1 << FUNC_FDB_PORT_MACLIMIT_CTRL_GET))
		p_adpt_api->adpt_fdb_port_maclimit_ctrl_get = adpt_hppe_fdb_port_maclimit_ctrl_get;
#endif
	if (p_adpt_api->adpt_fdb_func_bitmap[1] & (1 << (FUNC_FDB_DEL_BY_FID % 32)))
		p_adpt_api->adpt_fdb_del_by_fid = adpt_hppe_fdb_del_by_fid;

	aos_lock_init(&hppe_fdb_lock);

	return SW_OK;
}

/**
 * @}
 */

