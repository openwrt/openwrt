/*
 * Copyright (c) 2016-2017, 2021, The Linux Foundation. All rights reserved.
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
#include "hppe_policer_reg.h"
#include "hppe_policer.h"

sw_error_t
hppe_meter_cmpst_length_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union meter_cmpst_length_reg_u *value)
{
	if (index >= METER_CMPST_LENGTH_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + METER_CMPST_LENGTH_REG_ADDRESS + \
				index * METER_CMPST_LENGTH_REG_INC,
				&value->val);
}

sw_error_t
hppe_meter_cmpst_length_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union meter_cmpst_length_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + METER_CMPST_LENGTH_REG_ADDRESS + \
				index * METER_CMPST_LENGTH_REG_INC,
				value->val);
}

sw_error_t
hppe_pc_drop_bypass_reg_set(
		a_uint32_t dev_id,
		union pc_drop_bypass_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_DROP_BYPASS_REG_ADDRESS,
				value->val);
}

#ifndef IN_POLICER_MINI
sw_error_t
hppe_pc_drop_bypass_reg_get(
		a_uint32_t dev_id,
		union pc_drop_bypass_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_DROP_BYPASS_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_pc_spare_reg_get(
		a_uint32_t dev_id,
		union pc_spare_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_SPARE_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_pc_spare_reg_set(
		a_uint32_t dev_id,
		union pc_spare_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_SPARE_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_time_slot_reg_get(
		a_uint32_t dev_id,
		union time_slot_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + TIME_SLOT_REG_ADDRESS,
				&value->val);
}
#endif

sw_error_t
hppe_time_slot_reg_set(
		a_uint32_t dev_id,
		union time_slot_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + TIME_SLOT_REG_ADDRESS,
				value->val);
}

#ifndef IN_POLICER_MINI
sw_error_t
hppe_pc_dbg_addr_reg_get(
		a_uint32_t dev_id,
		union pc_dbg_addr_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_DBG_ADDR_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_pc_dbg_addr_reg_set(
		a_uint32_t dev_id,
		union pc_dbg_addr_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_DBG_ADDR_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_pc_dbg_data_reg_get(
		a_uint32_t dev_id,
		union pc_dbg_data_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_DBG_DATA_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_pc_dbg_data_reg_set(
		a_uint32_t dev_id,
		union pc_dbg_data_reg_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CFG_TBL_ADDRESS + \
				index * IN_ACL_METER_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CFG_TBL_ADDRESS + \
				index * IN_ACL_METER_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_crdt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CRDT_TBL_ADDRESS + \
				index * IN_ACL_METER_CRDT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_crdt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CRDT_TBL_ADDRESS + \
				index * IN_ACL_METER_CRDT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_port_meter_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_cfg_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CFG_TBL_ADDRESS + \
				index * IN_PORT_METER_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_in_port_meter_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_cfg_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CFG_TBL_ADDRESS + \
				index * IN_PORT_METER_CFG_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_in_port_meter_crdt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_crdt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CRDT_TBL_ADDRESS + \
				index * IN_PORT_METER_CRDT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_port_meter_crdt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_crdt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CRDT_TBL_ADDRESS + \
				index * IN_PORT_METER_CRDT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_port_meter_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CNT_TBL_ADDRESS + \
				index * IN_PORT_METER_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_in_port_meter_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_port_meter_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_PORT_METER_CNT_TBL_ADDRESS + \
				index * IN_PORT_METER_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CNT_TBL_ADDRESS + \
				index * IN_ACL_METER_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_acl_meter_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + IN_ACL_METER_CNT_TBL_ADDRESS + \
				index * IN_ACL_METER_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_pc_global_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pc_global_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_GLOBAL_CNT_TBL_ADDRESS + \
				index * PC_GLOBAL_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_pc_global_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pc_global_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PC_GLOBAL_CNT_TBL_ADDRESS + \
				index * PC_GLOBAL_CNT_TBL_INC,
				value->val,
				3);
}
#endif

sw_error_t
hppe_drop_cpu_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cpu_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + DROP_CPU_CNT_TBL_ADDRESS + \
				index * DROP_CPU_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_drop_cpu_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cpu_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + DROP_CPU_CNT_TBL_ADDRESS + \
				index * DROP_CPU_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PORT_TX_DROP_CNT_TBL_ADDRESS + \
				index * PORT_TX_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PORT_TX_DROP_CNT_TBL_ADDRESS + \
				index * PORT_TX_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VP_TX_DROP_CNT_TBL_ADDRESS + \
				index * VP_TX_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_drop_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VP_TX_DROP_CNT_TBL_ADDRESS + \
				index * VP_TX_DROP_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vlan_dev_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VLAN_DEV_CNT_TBL_ADDRESS + \
				index * VLAN_DEV_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vlan_dev_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VLAN_DEV_CNT_TBL_ADDRESS + \
				index * VLAN_DEV_CNT_TBL_INC,
				value->val,
				3);
}

#ifndef IN_POLICER_MINI
sw_error_t
hppe_meter_cmpst_length_reg_cmpst_length_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union meter_cmpst_length_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_meter_cmpst_length_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cmpst_length;
	return ret;
}

sw_error_t
hppe_meter_cmpst_length_reg_cmpst_length_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union meter_cmpst_length_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_meter_cmpst_length_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmpst_length = value;
	ret = hppe_meter_cmpst_length_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_drop_bypass_reg_drop_bypass_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union pc_drop_bypass_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_drop_bypass_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.drop_bypass_en;
	return ret;
}

sw_error_t
hppe_pc_drop_bypass_reg_drop_bypass_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union pc_drop_bypass_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_drop_bypass_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drop_bypass_en = value;
	ret = hppe_pc_drop_bypass_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_spare_reg_spare_reg_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union pc_spare_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_spare_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_reg;
	return ret;
}

sw_error_t
hppe_pc_spare_reg_spare_reg_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union pc_spare_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_spare_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_reg = value;
	ret = hppe_pc_spare_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_time_slot_reg_time_slot_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union time_slot_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_time_slot_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.time_slot;
	return ret;
}

sw_error_t
hppe_time_slot_reg_time_slot_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union time_slot_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_time_slot_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.time_slot = value;
	ret = hppe_time_slot_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_dbg_addr_reg_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union pc_dbg_addr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_dbg_addr_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_addr;
	return ret;
}

sw_error_t
hppe_pc_dbg_addr_reg_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union pc_dbg_addr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_dbg_addr_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_addr = value;
	ret = hppe_pc_dbg_addr_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_dbg_data_reg_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union pc_dbg_data_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_dbg_data_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data;
	return ret;
}

sw_error_t
hppe_pc_dbg_data_reg_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_mode;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_mode = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_color_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.color_mode;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_color_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.color_mode = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_pcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_pcp_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_pcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_pcp_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_pri;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_pri = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_pri_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_pri_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_pri_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_pri_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_dp;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_dp = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_dp;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_dp = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_dei;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_dei = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_pri_1 << 1 | \
		reg_val.bf.violate_pri_0;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_pri_1 = value >> 1;
	reg_val.bf.violate_pri_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_cbs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cbs;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_cbs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cbs = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_dei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_dei_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_dei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_dei_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_pcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_pcp_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_pcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_pcp_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_cir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cir_1 << 8 | \
		reg_val.bf.cir_0;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_cir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cir_1 = value >> 8;
	reg_val.bf.cir_0 = value & (((a_uint64_t)1<<8)-1);
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_dei;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_dei = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_pri_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_pri_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_pri_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_pri_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_unit;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_unit = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_en;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_meter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_en = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_dp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_dp_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_chg_dp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_dp_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_eir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eir_1 << 6 | \
		reg_val.bf.eir_0;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_eir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eir_1 = value >> 6;
	reg_val.bf.eir_0 = value & (((a_uint64_t)1<<6)-1);
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_dp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_dp_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_dp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_dp_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_dei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_dei_cmd;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_chg_dei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_dei_cmd = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_pcp;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_exceed_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_pcp = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_pcp;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_violate_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_pcp = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_token_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.token_unit;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_token_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.token_unit = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_coupling_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.coupling_flag;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_coupling_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.coupling_flag = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_ebs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ebs;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cfg_tbl_ebs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ebs = value;
	ret = hppe_in_acl_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_c_crdt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_crdt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_crdt;
	return ret;
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_c_crdt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_crdt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_crdt = value;
	ret = hppe_in_acl_meter_crdt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_e_crdt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_crdt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_crdt;
	return ret;
}

sw_error_t
hppe_in_acl_meter_crdt_tbl_e_crdt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_crdt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_crdt = value;
	ret = hppe_in_acl_meter_crdt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_mode;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_mode = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_color_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.color_mode;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_color_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.color_mode = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_pcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_pcp_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_pcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_pcp_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_pri;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_pri = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_pri_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_pri_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_pri_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_pri_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_dp;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_dp = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_dp;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_dp = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_dei;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_dei = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_pri;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_pri = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_cbs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cbs;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_cbs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cbs = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_dei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_dei_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_dei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_dei_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_pcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_pcp_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_pcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_pcp_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_cir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cir_1 << 3 | \
		reg_val.bf.cir_0;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_cir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cir_1 = value >> 3;
	reg_val.bf.cir_0 = value & (((a_uint64_t)1<<3)-1);
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_dei;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_dei = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_pri_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_pri_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_pri_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_pri_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_unit;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_unit = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_en;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_en = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_dp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_chg_dp_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_chg_dp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_chg_dp_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_eir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eir_1 << 1 | \
		reg_val.bf.eir_0;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_eir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eir_1 = value >> 1;
	reg_val.bf.eir_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_dp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_dp_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_dp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_dp_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_dei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_chg_dei_cmd;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_chg_dei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_chg_dei_cmd = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.exceed_pcp;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_exceed_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.exceed_pcp = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.violate_pcp;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_violate_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.violate_pcp = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_token_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.token_unit;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_token_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.token_unit = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_coupling_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.coupling_flag;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_coupling_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.coupling_flag = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.meter_flag;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_meter_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.meter_flag = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_ebs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ebs;
	return ret;
}

sw_error_t
hppe_in_port_meter_cfg_tbl_ebs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ebs = value;
	ret = hppe_in_port_meter_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_crdt_tbl_c_crdt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_crdt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_crdt;
	return ret;
}

sw_error_t
hppe_in_port_meter_crdt_tbl_c_crdt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_crdt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_crdt = value;
	ret = hppe_in_port_meter_crdt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_crdt_tbl_e_crdt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_crdt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_crdt;
	return ret;
}

sw_error_t
hppe_in_port_meter_crdt_tbl_e_crdt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_crdt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_crdt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_crdt = value;
	ret = hppe_in_port_meter_crdt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cnt_tbl_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union in_port_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.byte_cnt_1 << 32 | \
		reg_val.bf.byte_cnt_0;
	return ret;
}

sw_error_t
hppe_in_port_meter_cnt_tbl_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union in_port_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byte_cnt_1 = value >> 32;
	reg_val.bf.byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_in_port_meter_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_port_meter_cnt_tbl_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_port_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkt_cnt;
	return ret;
}

sw_error_t
hppe_in_port_meter_cnt_tbl_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_port_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_port_meter_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_cnt = value;
	ret = hppe_in_port_meter_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union in_acl_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.byte_cnt_1 << 32 | \
		reg_val.bf.byte_cnt_0;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union in_acl_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byte_cnt_1 = value >> 32;
	reg_val.bf.byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_in_acl_meter_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_acl_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkt_cnt;
	return ret;
}

sw_error_t
hppe_in_acl_meter_cnt_tbl_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_acl_meter_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_acl_meter_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_cnt = value;
	ret = hppe_in_acl_meter_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_global_cnt_tbl_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union pc_global_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_global_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.byte_cnt_1 << 32 | \
		reg_val.bf.byte_cnt_0;
	return ret;
}

sw_error_t
hppe_pc_global_cnt_tbl_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union pc_global_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_global_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byte_cnt_1 = value >> 32;
	reg_val.bf.byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_pc_global_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pc_global_cnt_tbl_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pc_global_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_global_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkt_cnt;
	return ret;
}

sw_error_t
hppe_pc_global_cnt_tbl_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pc_global_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pc_global_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_cnt = value;
	ret = hppe_pc_global_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_drop_cpu_cnt_tbl_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union drop_cpu_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cpu_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.byte_cnt_1 << 32 | \
		reg_val.bf.byte_cnt_0;
	return ret;
}

sw_error_t
hppe_drop_cpu_cnt_tbl_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union drop_cpu_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cpu_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byte_cnt_1 = value >> 32;
	reg_val.bf.byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_drop_cpu_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_drop_cpu_cnt_tbl_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union drop_cpu_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cpu_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkt_cnt;
	return ret;
}

sw_error_t
hppe_drop_cpu_cnt_tbl_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union drop_cpu_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cpu_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_cnt = value;
	ret = hppe_drop_cpu_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_tx_drop_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union port_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_drop_byte_cnt_1 << 32 | \
		reg_val.bf.tx_drop_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_tx_drop_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union port_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_drop_byte_cnt_1 = value >> 32;
	reg_val.bf.tx_drop_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_port_tx_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_tx_drop_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_drop_pkt_cnt;
	return ret;
}

sw_error_t
hppe_port_tx_drop_cnt_tbl_tx_drop_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_drop_pkt_cnt = value;
	ret = hppe_port_tx_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_tx_drop_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union vp_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_drop_byte_cnt_1 << 32 | \
		reg_val.bf.tx_drop_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_tx_drop_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union vp_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_drop_byte_cnt_1 = value >> 32;
	reg_val.bf.tx_drop_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_vp_tx_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_tx_drop_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vp_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_drop_pkt_cnt;
	return ret;
}

sw_error_t
hppe_vp_tx_drop_cnt_tbl_tx_drop_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vp_tx_drop_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_drop_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_drop_pkt_cnt = value;
	ret = hppe_vp_tx_drop_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_dev_cnt_tbl_rx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union vlan_dev_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.rx_byte_cnt_1 << 32 | \
		reg_val.bf.rx_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_vlan_dev_cnt_tbl_rx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union vlan_dev_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_byte_cnt_1 = value >> 32;
	reg_val.bf.rx_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_vlan_dev_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_dev_cnt_tbl_rx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vlan_dev_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pkt_cnt;
	return ret;
}

sw_error_t
hppe_vlan_dev_cnt_tbl_rx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vlan_dev_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_pkt_cnt = value;
	ret = hppe_vlan_dev_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif
