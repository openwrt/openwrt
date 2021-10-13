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
#include "hppe_portvlan_reg.h"
#include "hppe_portvlan.h"

sw_error_t
hppe_port_parsing_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_parsing_reg_u *value)
{
	if (index >= PORT_PARSING_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + PORT_PARSING_REG_ADDRESS + \
				index * PORT_PARSING_REG_INC,
				&value->val);
}

sw_error_t
hppe_port_parsing_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_parsing_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + PORT_PARSING_REG_ADDRESS + \
				index * PORT_PARSING_REG_INC,
				value->val);
}

sw_error_t
hppe_edma_vlan_tpid_reg_get(
		a_uint32_t dev_id,
		union edma_vlan_tpid_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				EDMA_CSR_BASE_ADDR + EDMA_VLAN_TPID_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_edma_vlan_tpid_reg_set(
		a_uint32_t dev_id,
		union edma_vlan_tpid_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				EDMA_CSR_BASE_ADDR + EDMA_VLAN_TPID_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_vlan_tpid_reg_get(
		a_uint32_t dev_id,
		union vlan_tpid_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + VLAN_TPID_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_vlan_tpid_reg_set(
		a_uint32_t dev_id,
		union vlan_tpid_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + VLAN_TPID_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_parsing_reg_port_role_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_parsing_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_parsing_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_role;
	return ret;
}

sw_error_t
hppe_port_parsing_reg_port_role_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_parsing_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_parsing_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_role = value;
	ret = hppe_port_parsing_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_tpid_reg_stag_tpid_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union vlan_tpid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_tpid_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.stag_tpid;
	return ret;
}

sw_error_t
hppe_vlan_tpid_reg_stag_tpid_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union vlan_tpid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_tpid_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stag_tpid = value;
	ret = hppe_vlan_tpid_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_tpid_reg_ctag_tpid_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union vlan_tpid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_tpid_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.ctag_tpid;
	return ret;
}

sw_error_t
hppe_vlan_tpid_reg_ctag_tpid_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union vlan_tpid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_tpid_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctag_tpid = value;
	ret = hppe_vlan_tpid_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_bridge_config_get(
		a_uint32_t dev_id,
		union bridge_config_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + BRIDGE_CONFIG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_bridge_config_set(
		a_uint32_t dev_id,
		union bridge_config_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + BRIDGE_CONFIG_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_def_vid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_vid_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_DEF_VID_ADDRESS +
				port_id * PORT_DEF_VID_INC,
				&value->val);
}

sw_error_t
hppe_port_def_vid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_vid_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_DEF_VID_ADDRESS +
				port_id * PORT_DEF_VID_INC,
				value->val);
}

sw_error_t
hppe_port_def_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_pcp_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_DEF_PCP_ADDRESS +
				port_id * PORT_DEF_PCP_INC,
				&value->val);
}

sw_error_t
hppe_port_def_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_pcp_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_DEF_PCP_ADDRESS +
				port_id * PORT_DEF_PCP_INC,
				value->val);
}

sw_error_t
hppe_port_vlan_config_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_vlan_config_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_VLAN_CONFIG_ADDRESS +
				port_id * PORT_VLAN_CONFIG_INC,
				&value->val);
}

sw_error_t
hppe_port_vlan_config_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_vlan_config_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + PORT_VLAN_CONFIG_ADDRESS +
				port_id * PORT_VLAN_CONFIG_INC,
				value->val);
}

#ifndef IN_PORTVLAN_MINI
sw_error_t
hppe_iv_dbg_addr_get(
		a_uint32_t dev_id,
		union iv_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + IV_DBG_ADDR_ADDRESS,
				&value->val);
}

sw_error_t
hppe_iv_dbg_addr_set(
		a_uint32_t dev_id,
		union iv_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + IV_DBG_ADDR_ADDRESS,
				value->val);
}

sw_error_t
hppe_iv_dbg_data_get(
		a_uint32_t dev_id,
		union iv_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + IV_DBG_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_iv_dbg_data_set(
		a_uint32_t dev_id,
		union iv_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_eco_reserve_get(
		a_uint32_t dev_id,
		union eco_reserve_u *value)
{
	return hppe_reg_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + ECO_RESERVE_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eco_reserve_set(
		a_uint32_t dev_id,
		union eco_reserve_u *value)
{
	return hppe_reg_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + ECO_RESERVE_ADDRESS,
				value->val);
}
#endif

sw_error_t
hppe_xlt_rule_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_rule_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + XLT_RULE_TBL_ADDRESS + \
				index * XLT_RULE_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_xlt_rule_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_rule_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + XLT_RULE_TBL_ADDRESS + \
				index * XLT_RULE_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_xlt_action_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_action_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + XLT_ACTION_TBL_ADDRESS + \
				index * XLT_ACTION_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_xlt_action_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_action_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_VLAN_BASE_ADDR + XLT_ACTION_TBL_ADDRESS + \
				index * XLT_ACTION_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_bridge_config_bridge_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bridge_config_get(dev_id, &reg_val);
	*value = reg_val.bf.bridge_type;
	return ret;
}

sw_error_t
hppe_bridge_config_bridge_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_bridge_config_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bridge_type = value;
	ret = hppe_bridge_config_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_cvid_en_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_cvid_en;
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_cvid_en_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cvid_en = value;
	ret = hppe_port_def_vid_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_svid_en_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_svid_en;
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_svid_en_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_svid_en = value;
	ret = hppe_port_def_vid_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_cvid;
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cvid = value;
	ret = hppe_port_def_vid_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_svid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_svid;
	return ret;
}

sw_error_t
hppe_port_def_vid_port_def_svid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_vid_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_svid = value;
	ret = hppe_port_def_vid_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_sdei;
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_sdei = value;
	ret = hppe_port_def_pcp_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_spcp;
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_spcp = value;
	ret = hppe_port_def_pcp_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_cdei;
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cdei = value;
	ret = hppe_port_def_pcp_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_def_cpcp;
	return ret;
}

sw_error_t
hppe_port_def_pcp_port_def_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_def_pcp_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_def_pcp_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cpcp = value;
	ret = hppe_port_def_pcp_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_dei_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_in_dei_prop_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_dei_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_in_dei_prop_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_pcp_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_in_pcp_prop_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_pcp_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_in_pcp_prop_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_untag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_untag_fltr_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_untag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_untag_fltr_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_vlan_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_in_vlan_fltr_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_in_vlan_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_in_vlan_fltr_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_pri_tag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_pri_tag_fltr_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_pri_tag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_pri_tag_fltr_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_vlan_xlt_miss_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_vlan_xlt_miss_fwd_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_vlan_xlt_miss_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_vlan_xlt_miss_fwd_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_tag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t *value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	*value = reg_val.bf.port_tag_fltr_cmd;
	return ret;
}

sw_error_t
hppe_port_vlan_config_port_tag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		a_uint32_t value)
{
	union port_vlan_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_vlan_config_get(dev_id, port_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_tag_fltr_cmd = value;
	ret = hppe_port_vlan_config_set(dev_id, port_id, &reg_val);
	return ret;
}

#ifndef IN_PORTVLAN_MINI
sw_error_t
hppe_iv_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union iv_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_iv_dbg_addr_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_addr;
	return ret;
}

sw_error_t
hppe_iv_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union iv_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_iv_dbg_addr_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_addr = value;
	ret = hppe_iv_dbg_addr_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_iv_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union iv_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_iv_dbg_data_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data;
	return ret;
}

sw_error_t
hppe_iv_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_eco_reserve_eco_res_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eco_reserve_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_get(dev_id, &reg_val);
	*value = reg_val.bf.eco_res;
	return ret;
}

sw_error_t
hppe_eco_reserve_eco_res_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eco_reserve_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eco_res = value;
	ret = hppe_eco_reserve_set(dev_id, &reg_val);
	return ret;
}
#endif

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_vid;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_vid = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_frm_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.frm_type;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_frm_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.frm_type = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_prot_value_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.prot_value_1 << 7 | \
		reg_val.bf.prot_value_0;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_prot_value_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.prot_value_1 = value >> 7;
	reg_val.bf.prot_value_0 = value & (((a_uint64_t)1<<7)-1);
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_frm_type_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.frm_type_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_frm_type_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.frm_type_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_dei;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_dei = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_dei;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_dei = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_pcp;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_pcp = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_pcp;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_pcp = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_vid_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_vid_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_dei_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_dei_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_bitmap;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_bitmap = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_prot_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.prot_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_prot_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.prot_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_pcp_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_pcp_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_fmt_1 << 1 | \
		reg_val.bf.ckey_fmt_0;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_fmt_1 = value >> 1;
	reg_val.bf.ckey_fmt_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_vid;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_vid = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_fmt;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_fmt = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_pcp_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_pcp_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_dei_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_dei_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_vid_incl;
	return ret;
}

sw_error_t
hppe_xlt_rule_tbl_skey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_rule_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_rule_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_vid_incl = value;
	ret = hppe_xlt_rule_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_dei_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dei_swap_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_dei_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dei_swap_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cvid;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cvid = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cpcp;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cpcp = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_spcp_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_spcp_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_sdei_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_sdei_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cvid_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cvid_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_spcp_1 << 1 | \
		reg_val.bf.xlt_spcp_0;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_spcp_1 = value >> 1;
	reg_val.bf.xlt_spcp_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_counter_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.counter_id;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_counter_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter_id = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vid_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vid_swap_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vid_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vid_swap_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_sdei;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_sdei = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_counter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.counter_en;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_counter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter_en = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_svid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_svid_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_svid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_svid_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_svid;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_svid = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vsi_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_vsi_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cpcp_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cpcp_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cdei_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cdei_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_pcp_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_swap_cmd;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_pcp_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_swap_cmd = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cdei;
	return ret;
}

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union xlt_action_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_xlt_action_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cdei = value;
	ret = hppe_xlt_action_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_rule_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_XLT_RULE_ADDRESS + \
				index * EG_VLAN_XLT_RULE_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_vlan_xlt_rule_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_rule_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_XLT_RULE_ADDRESS + \
				index * EG_VLAN_XLT_RULE_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_vsi_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_tag_u *value)
{
	if (index >= EG_VSI_TAG_NUM)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VSI_TAG_ADDRESS + \
				index * EG_VSI_TAG_INC,
				&value->val);
}

sw_error_t
hppe_eg_vsi_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_tag_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VSI_TAG_ADDRESS + \
				index * EG_VSI_TAG_INC,
				value->val);
}

sw_error_t
hppe_port_eg_def_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_def_vid_u *value)
{
	if (index >= PORT_EG_DEF_VID_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_EG_DEF_VID_ADDRESS + \
				index * PORT_EG_DEF_VID_INC,
				&value->val);
}

sw_error_t
hppe_port_eg_def_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_def_vid_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_EG_DEF_VID_ADDRESS + \
				index * PORT_EG_DEF_VID_INC,
				value->val);
}

sw_error_t
hppe_port_eg_vlan_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_vlan_u *value)
{
	if (index >= PORT_EG_VLAN_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_EG_VLAN_ADDRESS + \
				index * PORT_EG_VLAN_INC,
				&value->val);
}

sw_error_t
hppe_port_eg_vlan_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_vlan_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_EG_VLAN_ADDRESS + \
				index * PORT_EG_VLAN_INC,
				value->val);
}

sw_error_t
hppe_eg_vlan_tpid_get(
		a_uint32_t dev_id,
		union eg_vlan_tpid_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_TPID_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eg_vlan_tpid_set(
		a_uint32_t dev_id,
		union eg_vlan_tpid_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_TPID_ADDRESS,
				value->val);
}

sw_error_t
hppe_eg_bridge_config_get(
		a_uint32_t dev_id,
		union eg_bridge_config_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_BRIDGE_CONFIG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eg_bridge_config_set(
		a_uint32_t dev_id,
		union eg_bridge_config_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_BRIDGE_CONFIG_ADDRESS,
				value->val);
}

sw_error_t
hppe_eg_vlan_xlt_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_action_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_XLT_ACTION_ADDRESS + \
				index * EG_VLAN_XLT_ACTION_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_vlan_xlt_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_action_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VLAN_XLT_ACTION_ADDRESS + \
				index * EG_VLAN_XLT_ACTION_INC,
				value->val,
				2);
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_vid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_vid = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_dei;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_dei = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_dei;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_dei = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_pcp;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_pcp = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_pcp;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_pcp = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_vid_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_vid_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_dei_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_dei_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_bitmap;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_bitmap = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_pcp_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_pcp_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_fmt;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_fmt = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_vid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_vid = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_fmt;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_fmt = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ckey_pcp_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ckey_pcp_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_vid_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_vid_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.skey_dei_incl;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.skey_dei_incl = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi_valid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_rule_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_rule_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi_valid = value;
	ret = hppe_eg_vlan_xlt_rule_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vsi_tag_tagged_mode_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vsi_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tagged_mode_port_bitmap;
	return ret;
}

sw_error_t
hppe_eg_vsi_tag_tagged_mode_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vsi_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tagged_mode_port_bitmap = value;
	ret = hppe_eg_vsi_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_svid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_def_svid_en;
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_svid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_svid_en = value;
	ret = hppe_port_eg_def_vid_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_def_svid;
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_svid = value;
	ret = hppe_port_eg_def_vid_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_def_cvid_en;
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cvid_en = value;
	ret = hppe_port_eg_def_vid_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_def_cvid;
	return ret;
}

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_def_vid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_def_vid_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_def_cvid = value;
	ret = hppe_port_eg_def_vid_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_tx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_counting_en;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_tx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_counting_en = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_ctag_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_eg_vlan_ctag_mode;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_ctag_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_eg_vlan_ctag_mode = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_pcp_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_eg_pcp_prop_cmd;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_pcp_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_eg_pcp_prop_cmd = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_vsi_tag_mode_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi_tag_mode_en;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_vsi_tag_mode_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi_tag_mode_en = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_stag_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_eg_vlan_stag_mode;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_stag_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_eg_vlan_stag_mode = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_dei_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_eg_dei_prop_cmd;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_eg_dei_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_eg_dei_prop_cmd = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_vlan_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_vlan_type;
	return ret;
}

sw_error_t
hppe_port_eg_vlan_port_vlan_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_eg_vlan_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_eg_vlan_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_vlan_type = value;
	ret = hppe_port_eg_vlan_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_tpid_ctpid_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_vlan_tpid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_tpid_get(dev_id, &reg_val);
	*value = reg_val.bf.ctpid;
	return ret;
}

sw_error_t
hppe_eg_vlan_tpid_ctpid_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_vlan_tpid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_tpid_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ctpid = value;
	ret = hppe_eg_vlan_tpid_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_tpid_stpid_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_vlan_tpid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_tpid_get(dev_id, &reg_val);
	*value = reg_val.bf.stpid;
	return ret;
}

sw_error_t
hppe_eg_vlan_tpid_stpid_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_vlan_tpid_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_tpid_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.stpid = value;
	ret = hppe_eg_vlan_tpid_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_bridge_config_bridge_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	*value = reg_val.bf.bridge_type;
	return ret;
}

sw_error_t
hppe_eg_bridge_config_bridge_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bridge_type = value;
	ret = hppe_eg_bridge_config_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_bridge_config_pkt_l2_edit_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	*value = reg_val.bf.pkt_l2_edit_en;
	return ret;
}

sw_error_t
hppe_eg_bridge_config_pkt_l2_edit_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_l2_edit_en = value;
	ret = hppe_eg_bridge_config_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_bridge_config_queue_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	*value = reg_val.bf.queue_cnt_en;
	return ret;
}

sw_error_t
hppe_eg_bridge_config_queue_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eg_bridge_config_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_bridge_config_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.queue_cnt_en = value;
	ret = hppe_eg_bridge_config_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_dei_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dei_swap_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_dei_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dei_swap_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cvid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cvid = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cpcp;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cpcp = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_spcp_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_spcp_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_sdei_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_sdei_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cvid_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cvid_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_spcp_1 << 1 | \
		reg_val.bf.xlt_spcp_0;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_spcp_1 = value >> 1;
	reg_val.bf.xlt_spcp_0 = value & (((a_uint64_t)1<<1)-1);
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_counter_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.counter_id;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_counter_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter_id = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_vid_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vid_swap_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_vid_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vid_swap_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_sdei;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_sdei = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_counter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.counter_en;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_counter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter_en = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_svid_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_svid_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_svid;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_svid = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cpcp_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cpcp_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cdei_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cdei_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_pcp_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_swap_cmd;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_pcp_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_swap_cmd = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.xlt_cdei;
	return ret;
}

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vlan_xlt_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vlan_xlt_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xlt_cdei = value;
	ret = hppe_eg_vlan_xlt_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_tx_counter_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + VLAN_DEV_TX_COUNTER_TBL_ADDRESS + \
				index * VLAN_DEV_TX_COUNTER_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_tx_counter_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + VLAN_DEV_TX_COUNTER_TBL_ADDRESS + \
				index * VLAN_DEV_TX_COUNTER_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union vlan_dev_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_tx_counter_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_byte_cnt_1 << 32 | \
		reg_val.bf.tx_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union vlan_dev_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_tx_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_byte_cnt_1 = value >> 32;
	reg_val.bf.tx_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_vlan_dev_tx_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vlan_dev_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_tx_counter_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pkt_cnt;
	return ret;
}

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vlan_dev_tx_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_dev_tx_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_pkt_cnt = value;
	ret = hppe_vlan_dev_tx_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}

