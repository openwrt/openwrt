/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
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
#include "hppe_global_reg.h"
#include "cppe_portctrl_reg.h"
#include "cppe_portctrl.h"

sw_error_t
cppe_mru_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cppe_mru_mtu_ctrl_tbl_u *value)
{
	return hppe_reg_tbl_get(
			dev_id,
			IPE_L2_BASE_ADDR + CPPE_MRU_MTU_CTRL_TBL_ADDRESS + \
			index * CPPE_MRU_MTU_CTRL_TBL_INC,
			value->val,
			2);
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cppe_mru_mtu_ctrl_tbl_u *value)
{
	return hppe_reg_tbl_set(
			dev_id,
			IPE_L2_BASE_ADDR + CPPE_MRU_MTU_CTRL_TBL_ADDRESS + \
			index * CPPE_MRU_MTU_CTRL_TBL_INC,
			value->val,
			2);
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_force_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_res_prec_force;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_force_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_res_prec_force = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_qos_group_id;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_qos_group_id = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_force_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_res_prec_force;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_force_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_res_prec_force = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_post_acl_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.post_acl_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_post_acl_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.post_acl_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_preheader_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.preheader_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_preheader_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.preheader_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pre_acl_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pre_acl_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_pre_acl_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pre_acl_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_flow_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.flow_res_prec;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_flow_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_res_prec = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_qos_group_id;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_qos_group_id = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_src_profile_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.src_profile;
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_src_profile_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_profile = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
	{
		return ret;
	}
	reg_val.bf.source_filtering_bypass = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);

	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.source_filtering_bypass;

	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
	{
		return ret;
	}
	reg_val.bf.source_filtering_mode = value;
	ret = cppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);

	return ret;
}

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union cppe_mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.source_filtering_mode;

	return ret;
}

sw_error_t
cppe_port_phy_status_1_get(
		a_uint32_t dev_id,
		union cppe_port_phy_status_1_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_PHY_STATUS_1_ADDRESS,
				&value->val);
}

sw_error_t
cppe_port5_pcs1_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union cppe_port_phy_status_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = cppe_port_phy_status_1_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_pcs1_phy_status;
	return ret;
}
