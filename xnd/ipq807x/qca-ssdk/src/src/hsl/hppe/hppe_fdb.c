/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#include "hsl.h"
#include "hppe_reg_access.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#ifndef IN_FDB_MINI
sw_error_t
hppe_l2_dbg_addr_get(
		a_uint32_t dev_id,
		union l2_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBG_ADDR_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_dbg_addr_set(
		a_uint32_t dev_id,
		union l2_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBG_ADDR_ADDRESS,
				value->val);
}

sw_error_t
hppe_l2_dbg_data_get(
		a_uint32_t dev_id,
		union l2_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBG_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_dbg_data_set(
		a_uint32_t dev_id,
		union l2_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_ADDRESS,
				&value->val);
}
#endif
sw_error_t
hppe_fdb_tbl_op_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_ADDRESS,
				value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_ADDRESS,
				&value->val);
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_op_rslt_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_rslt_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_RSLT_ADDRESS,
				&value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_op_rslt_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_rslt_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_RSLT_ADDRESS,
				&value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_age_timer_get(
		a_uint32_t dev_id,
		union age_timer_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + AGE_TIMER_ADDRESS,
				&value->val);
}

sw_error_t
hppe_age_timer_set(
		a_uint32_t dev_id,
		union age_timer_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + AGE_TIMER_ADDRESS,
				value->val);
}
#endif
sw_error_t
hppe_l2_global_conf_get(
		a_uint32_t dev_id,
		union l2_global_conf_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_GLOBAL_CONF_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_global_conf_set(
		a_uint32_t dev_id,
		union l2_global_conf_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + L2_GLOBAL_CONF_ADDRESS,
				value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_l2_dbgcnt_cmd_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_cmd_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBGCNT_CMD_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_dbgcnt_cmd_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_cmd_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBGCNT_CMD_ADDRESS,
				value->val);
}

sw_error_t
hppe_l2_dbgcnt_rdata_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_rdata_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBGCNT_RDATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_dbgcnt_rdata_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_rdata_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l2_dbgcnt_wdata_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_wdata_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBGCNT_WDATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l2_dbgcnt_wdata_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_wdata_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + L2_DBGCNT_WDATA_ADDRESS,
				value->val);
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_RSLT_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data0_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_RSLT_DATA1_ADDRESS,
				&value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data1_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_RSLT_DATA2_ADDRESS,
				&value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data2_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_fdb_tbl_op_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_op_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA0_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_op_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_op_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA1_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_op_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_op_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data2_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_OP_DATA2_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA0_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA1_ADDRESS,
				value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_fdb_tbl_rd_op_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data2_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_RD_OP_DATA2_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_bridge_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_bridge_ctrl_u *value)
{
	if (index >= PORT_BRIDGE_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_BRIDGE_CTRL_ADDRESS + \
				index * PORT_BRIDGE_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_port_bridge_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_bridge_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_BRIDGE_CTRL_ADDRESS + \
				index * PORT_BRIDGE_CTRL_INC,
				value->val);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_port_lrn_limit_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_ctrl_u *value)
{
	if (index >= PORT_LRN_LIMIT_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_LRN_LIMIT_CTRL_ADDRESS + \
				index * PORT_LRN_LIMIT_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_port_lrn_limit_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_LRN_LIMIT_CTRL_ADDRESS + \
				index * PORT_LRN_LIMIT_CTRL_INC,
				value->val);
}

sw_error_t
hppe_port_lrn_limit_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_counter_u *value)
{
	if (index >= PORT_LRN_LIMIT_COUNTER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_LRN_LIMIT_COUNTER_ADDRESS + \
				index * PORT_LRN_LIMIT_COUNTER_INC,
				&value->val);
}

sw_error_t
hppe_port_lrn_limit_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_counter_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_rfdb_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfdb_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + RFDB_TBL_ADDRESS + \
				index * RFDB_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_rfdb_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfdb_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + RFDB_TBL_ADDRESS + \
				index * RFDB_TBL_INC,
				value->val,
				2);
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union fdb_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_ADDRESS + \
				index * FDB_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_fdb_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union fdb_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + FDB_TBL_ADDRESS + \
				index * FDB_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_l2_dbg_addr_l2_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbg_addr_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_dbg_addr;
	return ret;
}

sw_error_t
hppe_l2_dbg_addr_l2_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbg_addr_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_dbg_addr = value;
	ret = hppe_l2_dbg_addr_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_dbg_data_l2_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbg_data_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_dbg_data;
	return ret;
}

sw_error_t
hppe_l2_dbg_data_l2_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_op_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_mode;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_op_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_mode = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_op_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_type;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_op_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_type = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.entry_index = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmd_id = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.hash_block_bitmap;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_block_bitmap = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.byp_rslt_en;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byp_rslt_en = value;
	ret = hppe_fdb_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_op_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_mode;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_op_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_mode = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_op_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_type;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_op_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_type = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.entry_index = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmd_id = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.hash_block_bitmap;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_block_bitmap = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.byp_rslt_en;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byp_rslt_en = value;
	ret = hppe_fdb_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.op_rslt;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.valid_cnt;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_fdb_tbl_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.op_rslt;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.valid_cnt;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_fdb_tbl_rd_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_age_timer_age_val_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union age_timer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_age_timer_get(dev_id, &reg_val);
	*value = reg_val.bf.age_val;
	return ret;
}

sw_error_t
hppe_age_timer_age_val_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union age_timer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_age_timer_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.age_val = value;
	ret = hppe_age_timer_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_full_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.fdb_hash_full_fwd_cmd;
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_full_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fdb_hash_full_fwd_cmd = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_failover_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.failover_en;
	return ret;
}

sw_error_t
hppe_l2_global_conf_failover_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.failover_en = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_lrn_ctrl_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.lrn_ctrl_mode;
	return ret;
}

sw_error_t
hppe_l2_global_conf_lrn_ctrl_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lrn_ctrl_mode = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_age_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.age_en;
	return ret;
}

sw_error_t
hppe_l2_global_conf_age_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.age_en = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.fdb_hash_mode_1;
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fdb_hash_mode_1 = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.lrn_en;
	return ret;
}

sw_error_t
hppe_l2_global_conf_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lrn_en = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.fdb_hash_mode_0;
	return ret;
}

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fdb_hash_mode_0 = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_age_ctrl_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.age_ctrl_mode;
	return ret;
}

sw_error_t
hppe_l2_global_conf_age_ctrl_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.age_ctrl_mode = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_global_conf_service_code_loop_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	*value = reg_val.bf.service_code_loop;
	return ret;
}

sw_error_t
hppe_l2_global_conf_service_code_loop_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_global_conf_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_global_conf_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.service_code_loop = value;
	ret = hppe_l2_global_conf_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_cmd_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbgcnt_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_cmd_get(dev_id, &reg_val);
	*value = reg_val.bf.type;
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_cmd_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_dbgcnt_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_cmd_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.type = value;
	ret = hppe_l2_dbgcnt_cmd_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_cmd_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbgcnt_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_cmd_get(dev_id, &reg_val);
	*value = reg_val.bf.addr;
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_cmd_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_dbgcnt_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_cmd_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.addr = value;
	ret = hppe_l2_dbgcnt_cmd_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_rdata_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbgcnt_rdata_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_rdata_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_rdata_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l2_dbgcnt_wdata_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l2_dbgcnt_wdata_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_wdata_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_l2_dbgcnt_wdata_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l2_dbgcnt_wdata_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l2_dbgcnt_wdata_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_l2_dbgcnt_wdata_set(dev_id, &reg_val);
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_rslt_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_rslt_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_fdb_tbl_op_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_op_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_op_data0_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_op_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_op_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_op_data1_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_op_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_op_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_op_data2_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_op_data2_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_rd_op_data0_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_rd_op_data1_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_fdb_tbl_rd_op_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union fdb_tbl_rd_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}
#endif
sw_error_t
hppe_fdb_tbl_rd_op_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union fdb_tbl_rd_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_fdb_tbl_rd_op_data2_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_fdb_tbl_rd_op_data2_set(dev_id, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_port_bridge_ctrl_txmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmac_en;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_txmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.txmac_en = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_port_isolation_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_isolation_bitmap;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_port_isolation_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_isolation_bitmap = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_station_move_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.station_move_lrn_en;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_station_move_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.station_move_lrn_en = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_new_addr_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.new_addr_fwd_cmd;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_new_addr_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.new_addr_fwd_cmd = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_promisc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.promisc_en;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_promisc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.promisc_en = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_new_addr_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.new_addr_lrn_en;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_new_addr_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.new_addr_lrn_en = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_station_move_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.station_move_fwd_cmd;
	return ret;
}

sw_error_t
hppe_port_bridge_ctrl_station_move_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_bridge_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_bridge_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.station_move_fwd_cmd = value;
	ret = hppe_port_bridge_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lrn_lmt_en;
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lrn_lmt_en = value;
	ret = hppe_port_lrn_limit_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lrn_lmt_cnt;
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lrn_lmt_cnt = value;
	ret = hppe_port_lrn_limit_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_exceed_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lrn_lmt_exceed_fwd;
	return ret;
}

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_exceed_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_lrn_limit_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lrn_lmt_exceed_fwd = value;
	ret = hppe_port_lrn_limit_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_lrn_limit_counter_lrn_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_lrn_limit_counter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_lrn_limit_counter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lrn_cnt;
	return ret;
}

sw_error_t
hppe_port_lrn_limit_counter_lrn_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_rfdb_tbl_mac_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union rfdb_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfdb_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mac_addr_1 << 32 | \
		reg_val.bf.mac_addr_0;
	return ret;
}

sw_error_t
hppe_rfdb_tbl_mac_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union rfdb_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfdb_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_1 = value >> 32;
	reg_val.bf.mac_addr_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_rfdb_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_FDB_MINI
sw_error_t
hppe_rfdb_tbl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rfdb_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfdb_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}
#endif
sw_error_t
hppe_rfdb_tbl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rfdb_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfdb_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_rfdb_tbl_set(dev_id, index, &reg_val);
	return ret;
}

