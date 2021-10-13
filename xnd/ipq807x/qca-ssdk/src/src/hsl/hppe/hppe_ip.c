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
#include "hppe_ip_reg.h"
#include "hppe_ip.h"

#ifndef IN_IP_MINI
static a_uint32_t host_cmd_id = 0;
sw_error_t
hppe_rt_interface_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rt_interface_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + RT_INTERFACE_CNT_TBL_ADDRESS + \
				index * RT_INTERFACE_CNT_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_rt_interface_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rt_interface_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + RT_INTERFACE_CNT_TBL_ADDRESS + \
				index * RT_INTERFACE_CNT_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_my_mac_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union my_mac_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + MY_MAC_TBL_ADDRESS + \
				index * MY_MAC_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_my_mac_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union my_mac_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + MY_MAC_TBL_ADDRESS + \
				index * MY_MAC_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l3_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vsi_u *value)
{
	if (index >= L3_VSI_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VSI_ADDRESS + \
				index * L3_VSI_INC,
				&value->val);
}

sw_error_t
hppe_l3_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vsi_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VSI_ADDRESS + \
				index * L3_VSI_INC,
				value->val);
}

sw_error_t
hppe_l3_vsi_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vsi_ext_u *value)
{
	if (index >= L3_VSI_EXT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VSI_EXT_ADDRESS + \
				index * L3_VSI_EXT_INC,
				&value->val);
}

sw_error_t
hppe_l3_vsi_ext_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vsi_ext_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VSI_EXT_ADDRESS + \
				index * L3_VSI_EXT_INC,
				value->val);
}

sw_error_t
hppe_network_route_ip_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_ip_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_IP_ADDRESS + \
				index * NETWORK_ROUTE_IP_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_pub_ip_addr_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_pub_ip_addr_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + IN_PUB_IP_ADDR_TBL_ADDRESS + \
				index * IN_PUB_IP_ADDR_TBL_INC,
				value->val);
}


sw_error_t
hppe_in_pub_ip_addr_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_pub_ip_addr_tbl_u *value)
{
	if (index >= IN_PUB_IP_ADDR_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + IN_PUB_IP_ADDR_TBL_ADDRESS + \
				index * IN_PUB_IP_ADDR_TBL_INC,
				&value->val);
}

sw_error_t
hppe_network_route_ip_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_ip_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_IP_ADDRESS + \
				index * NETWORK_ROUTE_IP_INC,
				value->val,
				2);
}

sw_error_t
hppe_network_route_ip_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_ip_ext_u *value)
{
	if (index >= NETWORK_ROUTE_IP_EXT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_IP_EXT_ADDRESS + \
				index * NETWORK_ROUTE_IP_EXT_INC,
				&value->val);
}

sw_error_t
hppe_network_route_ip_ext_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_ip_ext_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_IP_EXT_ADDRESS + \
				index * NETWORK_ROUTE_IP_EXT_INC,
				value->val);
}

sw_error_t
hppe_network_route_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_action_u *value)
{
	if (index >= NETWORK_ROUTE_ACTION_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_ACTION_ADDRESS + \
				index * NETWORK_ROUTE_ACTION_INC,
				&value->val);
}

sw_error_t
hppe_network_route_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union network_route_action_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + NETWORK_ROUTE_ACTION_ADDRESS + \
				index * NETWORK_ROUTE_ACTION_INC,
				value->val);
}
#endif
#if ((!defined IN_IP_MINI) || (!defined IN_FLOW_MINI))
sw_error_t
hppe_l3_route_ctrl_get(
		a_uint32_t dev_id,
		union l3_route_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_ROUTE_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_route_ctrl_set(
		a_uint32_t dev_id,
		union l3_route_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_ROUTE_CTRL_ADDRESS,
				value->val);
}

sw_error_t
hppe_l3_route_ctrl_ext_get(
		a_uint32_t dev_id,
		union l3_route_ctrl_ext_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_ROUTE_CTRL_EXT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_route_ctrl_ext_set(
		a_uint32_t dev_id,
		union l3_route_ctrl_ext_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_ROUTE_CTRL_EXT_ADDRESS,
				value->val);
}
#endif
#ifndef IN_IP_MINI
sw_error_t
hppe_host_tbl_op_get(
		a_uint32_t dev_id,
		union host_tbl_op_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_set(
		a_uint32_t dev_id,
		union host_tbl_op_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_ADDRESS,
				value->val);
}
#endif
#if ((!defined IN_IP_MINI) || (!defined IN_FLOW_MINI))
sw_error_t
hppe_host_tbl_op_data0_get(
		a_uint32_t dev_id,
		union host_tbl_op_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data0_set(
		a_uint32_t dev_id,
		union host_tbl_op_data0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA0_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data1_get(
		a_uint32_t dev_id,
		union host_tbl_op_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data1_set(
		a_uint32_t dev_id,
		union host_tbl_op_data1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA1_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data2_get(
		a_uint32_t dev_id,
		union host_tbl_op_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data2_set(
		a_uint32_t dev_id,
		union host_tbl_op_data2_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA2_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data3_get(
		a_uint32_t dev_id,
		union host_tbl_op_data3_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA3_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data3_set(
		a_uint32_t dev_id,
		union host_tbl_op_data3_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA3_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data4_get(
		a_uint32_t dev_id,
		union host_tbl_op_data4_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA4_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data4_set(
		a_uint32_t dev_id,
		union host_tbl_op_data4_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA4_ADDRESS,
				value->val);
}
#endif
#ifndef IN_IP_MINI
sw_error_t
hppe_host_tbl_op_data5_get(
		a_uint32_t dev_id,
		union host_tbl_op_data5_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA5_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data5_set(
		a_uint32_t dev_id,
		union host_tbl_op_data5_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA5_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data6_get(
		a_uint32_t dev_id,
		union host_tbl_op_data6_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA6_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data6_set(
		a_uint32_t dev_id,
		union host_tbl_op_data6_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA6_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data7_get(
		a_uint32_t dev_id,
		union host_tbl_op_data7_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA7_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data7_set(
		a_uint32_t dev_id,
		union host_tbl_op_data7_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA7_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data8_get(
		a_uint32_t dev_id,
		union host_tbl_op_data8_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA8_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data8_set(
		a_uint32_t dev_id,
		union host_tbl_op_data8_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA8_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_data9_get(
		a_uint32_t dev_id,
		union host_tbl_op_data9_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA9_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_data9_set(
		a_uint32_t dev_id,
		union host_tbl_op_data9_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_DATA9_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_op_rslt_get(
		a_uint32_t dev_id,
		union host_tbl_op_rslt_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_OP_RSLT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_op_rslt_set(
		a_uint32_t dev_id,
		union host_tbl_op_rslt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_op_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_ADDRESS,
				value->val);
}
#endif
#if ((!defined IN_IP_MINI) || (!defined IN_FLOW_MINI))
sw_error_t
hppe_host_tbl_rd_op_data0_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data0_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA0_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data1_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data1_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA1_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data2_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data2_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data2_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA2_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data3_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data3_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA3_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data3_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data3_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA3_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data4_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data4_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA4_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data4_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data4_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA4_ADDRESS,
				value->val);
}
#endif
#ifndef IN_IP_MINI
sw_error_t
hppe_host_tbl_rd_op_data5_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data5_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA5_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data5_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data5_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA5_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data6_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data6_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA6_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data6_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data6_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA6_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data7_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data7_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA7_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data7_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data7_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA7_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data8_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data8_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA8_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data8_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data8_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA8_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data9_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data9_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA9_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_data9_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_data9_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_DATA9_ADDRESS,
				value->val);
}

sw_error_t
hppe_host_tbl_rd_op_rslt_get(
		a_uint32_t dev_id,
		union host_tbl_rd_op_rslt_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_OP_RSLT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_op_rslt_set(
		a_uint32_t dev_id,
		union host_tbl_rd_op_rslt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data0_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data0_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data0_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data0_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data1_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data1_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data1_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data1_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data2_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data2_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data2_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data2_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data3_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data3_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA3_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data3_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data3_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data4_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data4_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA4_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data4_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data4_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data5_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data5_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA5_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data5_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data5_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data6_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data6_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA6_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data6_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data6_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data7_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data7_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA7_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data7_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data7_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data8_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data8_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA8_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data8_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data8_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data9_get(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data9_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA9_ADDRESS,
				&value->val);
}

sw_error_t
hppe_host_tbl_rd_rslt_data9_set(
		a_uint32_t dev_id,
		union host_tbl_rd_rslt_data9_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l3_dbg_cmd_get(
		a_uint32_t dev_id,
		union l3_dbg_cmd_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_DBG_CMD_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_dbg_cmd_set(
		a_uint32_t dev_id,
		union l3_dbg_cmd_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_DBG_CMD_ADDRESS,
				value->val);
}

sw_error_t
hppe_l3_dbg_wr_data_get(
		a_uint32_t dev_id,
		union l3_dbg_wr_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_DBG_WR_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_dbg_wr_data_set(
		a_uint32_t dev_id,
		union l3_dbg_wr_data_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_DBG_WR_DATA_ADDRESS,
				value->val);
}

sw_error_t
hppe_l3_dbg_rd_data_get(
		a_uint32_t dev_id,
		union l3_dbg_rd_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_DBG_RD_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_l3_dbg_rd_data_set(
		a_uint32_t dev_id,
		union l3_dbg_rd_data_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_l3_vp_port_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vp_port_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VP_PORT_TBL_ADDRESS + \
				index * L3_VP_PORT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_l3_vp_port_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_vp_port_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + L3_VP_PORT_TBL_ADDRESS + \
				index * L3_VP_PORT_TBL_INC,
				value->val,
				3);
}
#if ((!defined IN_IP_MINI) || (defined IN_PPPOE))
sw_error_t
hppe_in_l3_if_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l3_if_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + IN_L3_IF_TBL_ADDRESS + \
				index * IN_L3_IF_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_in_l3_if_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l3_if_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + IN_L3_IF_TBL_ADDRESS + \
				index * IN_L3_IF_TBL_INC,
				value->val,
				2);
}
#endif
#ifndef IN_IP_MINI
sw_error_t
hppe_host_ipv6_mcast_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv6_mcast_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV6_MCAST_TBL_ADDRESS + \
				index * HOST_IPV6_MCAST_TBL_INC,
				value->val,
				10);
}

sw_error_t
hppe_host_ipv6_mcast_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv6_mcast_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV6_MCAST_TBL_ADDRESS + \
				index * HOST_IPV6_MCAST_TBL_INC,
				value->val,
				10);
}

sw_error_t
hppe_host_ipv4_mcast_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv4_mcast_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV4_MCAST_TBL_ADDRESS + \
				index * HOST_IPV4_MCAST_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_host_ipv4_mcast_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv4_mcast_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV4_MCAST_TBL_ADDRESS + \
				index * HOST_IPV4_MCAST_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_host_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_ADDRESS + \
				index * HOST_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_host_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_ADDRESS + \
				index * HOST_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_host_ipv6_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv6_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV6_TBL_ADDRESS + \
				index * HOST_IPV6_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_host_ipv6_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union host_ipv6_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_IPV6_TBL_ADDRESS + \
				index * HOST_IPV6_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_in_nexthop_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_nexthop_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + IN_NEXTHOP_TBL_ADDRESS + \
				index * IN_NEXTHOP_TBL_INC,
				value->val,
				4);
}

sw_error_t
hppe_in_nexthop_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_nexthop_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L3_BASE_ADDR + IN_NEXTHOP_TBL_ADDRESS + \
				index * IN_NEXTHOP_TBL_INC,
				value->val,
				4);
}
#endif
#if ((!defined IN_IP_MINI) || (defined IN_PPPOE))
sw_error_t
hppe_eg_l3_if_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_l3_if_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_L3_IF_TBL_ADDRESS + \
				index * EG_L3_IF_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_eg_l3_if_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_l3_if_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_L3_IF_TBL_ADDRESS + \
				index * EG_L3_IF_TBL_INC,
				value->val,
				3);
}
#endif
#ifndef IN_IP_MINI
sw_error_t
hppe_my_mac_tbl_mac_da_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union my_mac_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_my_mac_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mac_da_1 << 32 | \
		reg_val.bf.mac_da_0;
	return ret;
}

sw_error_t
hppe_my_mac_tbl_mac_da_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union my_mac_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_my_mac_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da_1 = value >> 32;
	reg_val.bf.mac_da_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_my_mac_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_my_mac_tbl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union my_mac_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_my_mac_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_my_mac_tbl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union my_mac_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_my_mac_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_my_mac_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l3_if_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l3_if_index;
	return ret;
}

sw_error_t
hppe_l3_vsi_l3_if_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_if_index = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv6_mc_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l2_ipv6_mc_mode;
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv6_mc_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ipv6_mc_mode = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l3_if_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l3_if_valid;
	return ret;
}

sw_error_t
hppe_l3_vsi_l3_if_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_if_valid = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv6_mc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l2_ipv6_mc_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv6_mc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ipv6_mc_en = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv4_mc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l2_ipv4_mc_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv4_mc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ipv4_mc_en = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv4_mc_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l2_ipv4_mc_mode;
	return ret;
}

sw_error_t
hppe_l3_vsi_l2_ipv4_mc_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ipv4_mc_mode = value;
	ret = hppe_l3_vsi_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_svlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_cvlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_vio_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_port_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_svlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_cvlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_svlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_svlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_vio_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_port_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_src_unk_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_port_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_src_unk_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_vio_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_port_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_vio_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_src_unk_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_arp_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_cvlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ip_nd_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_src_unk_cmd = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv4_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vsi_ext_ipv6_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vsi_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vsi_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_cvlan_en = value;
	ret = hppe_l3_vsi_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_ip_ip_addr_mask_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_ip_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_addr_mask;
	return ret;
}

sw_error_t
hppe_network_route_ip_ip_addr_mask_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_ip_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr_mask = value;
	ret = hppe_network_route_ip_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_ip_ip_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_ip_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
hppe_network_route_ip_ip_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_ip_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = hppe_network_route_ip_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_ip_ext_entry_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_ip_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_type;
	return ret;
}

sw_error_t
hppe_network_route_ip_ext_entry_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_ip_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.entry_type = value;
	ret = hppe_network_route_ip_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_ip_ext_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_ip_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_network_route_ip_ext_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_ip_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_ip_ext_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_network_route_ip_ext_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_action_lan_wan_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lan_wan;
	return ret;
}

sw_error_t
hppe_network_route_action_lan_wan_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lan_wan = value;
	ret = hppe_network_route_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_action_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fwd_cmd;
	return ret;
}

sw_error_t
hppe_network_route_action_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fwd_cmd = value;
	ret = hppe_network_route_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_network_route_action_dst_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dst_info;
	return ret;
}

sw_error_t
hppe_network_route_action_dst_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union network_route_action_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_network_route_action_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dst_info = value;
	ret = hppe_network_route_action_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_src_if_check_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_src_if_check_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_src_if_check_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_src_if_check_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_pppoe_multicast_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.pppoe_multicast_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_pppoe_multicast_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pppoe_multicast_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_fail_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mtu_fail;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_fail_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mtu_fail = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_src_if_check_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_src_if_check_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_src_if_check_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_src_if_check_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_icmp_rdt_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.icmp_rdt_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_icmp_rdt_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.icmp_rdt_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_de_acce_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_de_acce_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_de_acce_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_de_acce_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_fail_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mtu_fail_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_fail_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mtu_fail_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_sync_mismatch_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_sync_mismatch_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_sync_mismatch_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_sync_mismatch_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_service_code_loop_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_service_code_loop_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_service_code_loop_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_service_code_loop_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_sync_mismatch_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_sync_mismatch_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_sync_mismatch_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_sync_mismatch_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_df_fail_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mtu_df_fail;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_df_fail_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mtu_df_fail = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_service_code_loop_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_service_code_loop;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_flow_service_code_loop_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_service_code_loop = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mru_check_fail_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mru_check_fail_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mru_check_fail_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mru_check_fail_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_prefix_bc_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_prefix_bc_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_prefix_bc_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_prefix_bc_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_df_fail_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mtu_df_fail_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mtu_df_fail_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mtu_df_fail_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_pppoe_multicast_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.pppoe_multicast_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_pppoe_multicast_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pppoe_multicast_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mru_check_fail_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_mru_check_fail;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_mru_check_fail_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_mru_check_fail = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_icmp_rdt_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.icmp_rdt_cmd;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_icmp_rdt_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.icmp_rdt_cmd = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_prefix_bc_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_prefix_bc_de_acce;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ip_prefix_bc_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_prefix_bc_de_acce = value;
	ret = hppe_l3_route_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_flow_service_code_loop_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	*value = reg_val.bf.flow_service_code_loop_en;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_flow_service_code_loop_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_service_code_loop_en = value;
	ret = hppe_l3_route_ctrl_ext_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_host_hash_mode_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	*value = reg_val.bf.host_hash_mode_0;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_host_hash_mode_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.host_hash_mode_0 = value;
	ret = hppe_l3_route_ctrl_ext_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_host_hash_mode_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	*value = reg_val.bf.host_hash_mode_1;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_host_hash_mode_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.host_hash_mode_1 = value;
	ret = hppe_l3_route_ctrl_ext_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_ip_route_mismatch_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	*value = reg_val.bf.ip_route_mismatch;
	return ret;
}

sw_error_t
hppe_l3_route_ctrl_ext_ip_route_mismatch_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_route_ctrl_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_route_ctrl_ext_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_route_mismatch = value;
	ret = hppe_l3_route_ctrl_ext_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_host_tbl_op_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.entry_index = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_host_tbl_op_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmd_id = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.byp_rslt_en;
	return ret;
}

sw_error_t
hppe_host_tbl_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byp_rslt_en = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_mode;
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_mode = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_type;
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_type = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_result_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_result;
	return ret;
}

sw_error_t
hppe_host_tbl_op_op_result_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_result = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_busy_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.busy;
	return ret;
}

sw_error_t
hppe_host_tbl_op_busy_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.busy = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	*value = reg_val.bf.hash_block_bitmap;
	return ret;
}

sw_error_t
hppe_host_tbl_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_block_bitmap = value;
	ret = hppe_host_tbl_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data2_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data2_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data3_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data3_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data3_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data3_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data3_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data4_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data4_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data4_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data4_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data4_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data5_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data5_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data5_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data5_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data5_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data6_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data6_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data6_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data6_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data6_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data7_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data7_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data7_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data7_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data7_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data8_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data8_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data8_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data8_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data8_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data8_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data8_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_data9_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_data9_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data9_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_op_data9_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_op_data9_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_data9_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_op_data9_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.op_rslt;
	return ret;
}

sw_error_t
hppe_host_tbl_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.valid_cnt;
	return ret;
}

sw_error_t
hppe_host_tbl_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_host_tbl_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_host_tbl_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_op_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.entry_index = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cmd_id = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.byp_rslt_en;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byp_rslt_en = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_mode;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_mode = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_type;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_type = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_result_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.op_result;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_op_result_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.op_result = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_busy_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.busy;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_busy_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.busy = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	*value = reg_val.bf.hash_block_bitmap;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hash_block_bitmap = value;
	ret = hppe_host_tbl_rd_op_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data2_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data2_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data3_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data3_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data3_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data3_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data3_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data4_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data4_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data4_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data4_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data4_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data5_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data5_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data5_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data5_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data5_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data6_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data6_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data6_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data6_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data6_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data7_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data7_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data7_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data7_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data7_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data8_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data8_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data8_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data8_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data8_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data8_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data8_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data9_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_data9_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data9_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_data9_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union host_tbl_rd_op_data9_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_data9_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_host_tbl_rd_op_data9_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.op_rslt;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.valid_cnt;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.entry_index;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_op_rslt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_op_rslt_get(dev_id, &reg_val);
	*value = reg_val.bf.cmd_id;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data0_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data0_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data0_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data1_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data1_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data1_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data2_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data2_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data2_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data3_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data3_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data3_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data4_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data4_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data4_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data4_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data5_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data5_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data5_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data5_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data6_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data6_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data6_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data6_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data7_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data7_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data7_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data7_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data8_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data8_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data8_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data8_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_host_tbl_rd_rslt_data9_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union host_tbl_rd_rslt_data9_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_host_tbl_rd_rslt_data9_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_host_tbl_rd_rslt_data9_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l3_dbg_cmd_type_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_dbg_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_cmd_get(dev_id, &reg_val);
	*value = reg_val.bf.type;
	return ret;
}

sw_error_t
hppe_l3_dbg_cmd_type_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_dbg_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_cmd_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.type = value;
	ret = hppe_l3_dbg_cmd_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_dbg_cmd_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_dbg_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_cmd_get(dev_id, &reg_val);
	*value = reg_val.bf.addr;
	return ret;
}

sw_error_t
hppe_l3_dbg_cmd_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_dbg_cmd_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_cmd_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.addr = value;
	ret = hppe_l3_dbg_cmd_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_dbg_wr_data_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_dbg_wr_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_wr_data_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_l3_dbg_wr_data_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union l3_dbg_wr_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_wr_data_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.data = value;
	ret = hppe_l3_dbg_wr_data_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_dbg_rd_data_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union l3_dbg_rd_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_dbg_rd_data_get(dev_id, &reg_val);
	*value = reg_val.bf.data;
	return ret;
}

sw_error_t
hppe_l3_dbg_rd_data_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_svlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_cvlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_l3_if_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l3_if_valid;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_l3_if_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_if_valid = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_vio_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_port_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_l3_if_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.l3_if_index;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_l3_if_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_if_index = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_svlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_cvlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_svlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_mac_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_valid;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_mac_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_valid = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_svlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_svlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_svlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_svlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_mac_da_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mac_da_1 << 16 | \
		reg_val.bf.mac_da_0;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_mac_da_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da_1 = value >> 16;
	reg_val.bf.mac_da_0 = value & (((a_uint64_t)1<<16)-1);
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_vio_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_sg_port_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_src_unk_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_port_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_src_unk_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_port_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_port_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_port_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_port_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_vio_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_vio_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_vio_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_vio_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_vio_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_src_unk_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_arp_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_arp_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_arp_sg_cvlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_vsi_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vsi_valid;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_vsi_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vsi_valid = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_src_unk_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_nd_src_unk_cmd;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ip_nd_src_unk_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_nd_src_unk_cmd = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_sg_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv4_sg_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_sg_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_cvlan_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_sg_cvlan_en;
	return ret;
}

sw_error_t
hppe_l3_vp_port_tbl_ipv6_sg_cvlan_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l3_vp_port_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l3_vp_port_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_sg_cvlan_en = value;
	ret = hppe_l3_vp_port_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_dec_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ttl_dec_bypass;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_dec_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ttl_dec_bypass = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_exceed_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ttl_exceed_cmd;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_exceed_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ttl_exceed_cmd = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mru_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mru;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mru_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mru = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ipv4_uc_route_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv4_uc_route_en;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ipv4_uc_route_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_uc_route_en = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ipv6_uc_route_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipv6_uc_route_en;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ipv6_uc_route_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_uc_route_en = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_exceed_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ttl_exceed_de_acce;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_ttl_exceed_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ttl_exceed_de_acce = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_icmp_trigger_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.icmp_trigger_en;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_icmp_trigger_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.icmp_trigger_en = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mac_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_bitmap;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mac_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_bitmap = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_pppoe_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pppoe_en;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_pppoe_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pppoe_en = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mtu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mtu;
	return ret;
}

sw_error_t
hppe_in_l3_if_tbl_mtu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mtu = value;
	ret = hppe_in_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_pub_addr_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_pub_addr_index;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_pub_addr_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_pub_addr_index = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.cvid;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.cvid = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_post_l3_if_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.post_l3_if;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_post_l3_if_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.post_l3_if = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_mac_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf1.mac_addr_1 << 16 | \
		reg_val.bf1.mac_addr_0;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_mac_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.mac_addr_1 = value >> 16;
	reg_val.bf1.mac_addr_0 = value & (((a_uint64_t)1<<16)-1);
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf0.port;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf0.port = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_to_me_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_to_me;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_to_me_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_to_me = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_addr_dnat_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ip_addr_dnat;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ip_addr_dnat_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ip_addr_dnat = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.type;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.type = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_stag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.stag_fmt;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_stag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.stag_fmt = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.vsi;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.vsi = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.svid;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.svid = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ctag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf1.ctag_fmt;
	return ret;
}

sw_error_t
hppe_in_nexthop_tbl_ctag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_nexthop_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_nexthop_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf1.ctag_fmt = value;
	ret = hppe_in_nexthop_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_session_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.session_id;
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_session_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.session_id = value;
	ret = hppe_eg_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_mac_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.mac_addr_1 << 32 | \
		reg_val.bf.mac_addr_0;
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_mac_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_1 = value >> 32;
	reg_val.bf.mac_addr_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_eg_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_pppoe_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pppoe_en;
	return ret;
}

sw_error_t
hppe_eg_l3_if_tbl_pppoe_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_l3_if_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_l3_if_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pppoe_en = value;
	ret = hppe_eg_l3_if_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_in_pub_ip_addr_tbl_ip_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union in_pub_ip_addr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_pub_ip_addr_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
hppe_in_pub_ip_addr_tbl_ip_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union in_pub_ip_addr_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_in_pub_ip_addr_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = hppe_in_pub_ip_addr_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.byte_cnt_1 << 32 | \
		reg_val.bf.byte_cnt_0;
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.byte_cnt_1 = value >> 32;
	reg_val.bf.byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_rt_interface_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkt_cnt;
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_cnt = value;
	ret = hppe_rt_interface_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_drop_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.drop_byte_cnt_1 << 24 | \
		reg_val.bf.drop_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_drop_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drop_byte_cnt_1 = value >> 24;
	reg_val.bf.drop_byte_cnt_0 = value & (((a_uint64_t)1<<24)-1);
	ret = hppe_rt_interface_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_drop_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drop_pkt_cnt_1 << 24 | \
		reg_val.bf.drop_pkt_cnt_0;
	return ret;
}

sw_error_t
hppe_rt_interface_cnt_tbl_drop_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rt_interface_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rt_interface_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drop_pkt_cnt_1 = value >> 24;
	reg_val.bf.drop_pkt_cnt_0 = value & (((a_uint64_t)1<<24)-1);
	ret = hppe_rt_interface_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}


sw_error_t
hppe_host_op_common(
		a_uint32_t dev_id,
		a_uint32_t op_type,
		a_uint32_t op_mode,
		a_uint32_t *index)
{
	union host_tbl_op_u op;
	union host_tbl_op_rslt_u result;
	a_uint32_t i = 0x100;
	sw_error_t rv;

	op.bf.cmd_id = host_cmd_id;
	host_cmd_id++;
	op.bf.byp_rslt_en = 0;
	op.bf.op_type = op_type;
	op.bf.hash_block_bitmap = 3;
	op.bf.op_mode = op_mode;
	op.bf.entry_index = *index;

	rv = hppe_host_tbl_op_set(dev_id, &op);
	if (SW_OK != rv)
		return rv;
	rv = hppe_host_tbl_op_rslt_get(dev_id, &result);
	if (SW_OK != rv)
		return rv;
	while (!result.bf.valid_cnt && --i) {
		hppe_host_tbl_op_rslt_get(dev_id, &result);
	}
	if (i == 0)
		return SW_BUSY;
	if (result.bf.op_rslt == 0) {
		*index = result.bf.entry_index;
		return SW_OK;
	}
	else
		return SW_FAIL;
	
}

sw_error_t
hppe_host_ipv4_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry)
{
	hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
	hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
	hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
	return hppe_host_op_common(dev_id, 0, op_mode, index);
}

sw_error_t
hppe_host_ipv6_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry)
{
	hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
	hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
	hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
	hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
	hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
	return hppe_host_op_common(dev_id, 0, op_mode, index);
}

sw_error_t
hppe_host_ipv4_mcast_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv4_mcast_tbl_u *entry)
{
	hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
	hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
	hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
	hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
	hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
	return hppe_host_op_common(dev_id, 0, op_mode, index);
}

sw_error_t
hppe_host_ipv6_mcast_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_mcast_tbl_u *entry)
{
	hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
	hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
	hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
	hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
	hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
	hppe_host_tbl_op_data5_set(dev_id, (union host_tbl_op_data5_u *)(&entry->val[5]));
	hppe_host_tbl_op_data6_set(dev_id, (union host_tbl_op_data6_u *)(&entry->val[6]));
	hppe_host_tbl_op_data7_set(dev_id, (union host_tbl_op_data7_u *)(&entry->val[7]));
	hppe_host_tbl_op_data8_set(dev_id, (union host_tbl_op_data8_u *)(&entry->val[8]));
	hppe_host_tbl_op_data9_set(dev_id, (union host_tbl_op_data9_u *)(&entry->val[9]));
	return hppe_host_op_common(dev_id, 0, op_mode, index);
}

sw_error_t
hppe_host_ipv4_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
	}
	return hppe_host_op_common(dev_id, 1, op_mode, index);

}

sw_error_t
hppe_host_ipv6_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
	}
	return hppe_host_op_common(dev_id, 1, op_mode, index);
}

sw_error_t
hppe_host_ipv4_mcast_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv4_mcast_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
	}
	return hppe_host_op_common(dev_id, 1, op_mode, index);
}

sw_error_t
hppe_host_ipv6_mcast_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_mcast_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_op_data0_set(dev_id, (union host_tbl_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_op_data1_set(dev_id, (union host_tbl_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_op_data2_set(dev_id, (union host_tbl_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_op_data3_set(dev_id, (union host_tbl_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_op_data4_set(dev_id, (union host_tbl_op_data4_u *)(&entry->val[4]));
		hppe_host_tbl_op_data5_set(dev_id, (union host_tbl_op_data5_u *)(&entry->val[5]));
		hppe_host_tbl_op_data6_set(dev_id, (union host_tbl_op_data6_u *)(&entry->val[6]));
		hppe_host_tbl_op_data7_set(dev_id, (union host_tbl_op_data7_u *)(&entry->val[7]));
		hppe_host_tbl_op_data8_set(dev_id, (union host_tbl_op_data8_u *)(&entry->val[8]));
		hppe_host_tbl_op_data9_set(dev_id, (union host_tbl_op_data9_u *)(&entry->val[9]));
	}
	return hppe_host_op_common(dev_id, 1, op_mode, index);
}

sw_error_t
hppe_host_get_common(
		a_uint32_t dev_id,
		a_uint32_t op_mode,
		a_uint32_t *index,
		a_uint32_t *data,
		a_uint32_t num)
{
	union host_tbl_rd_op_u op;
	union host_tbl_rd_op_rslt_u result;
	a_uint32_t i = 0x100;
	sw_error_t rv;

	op.bf.cmd_id = host_cmd_id;
	host_cmd_id++;
	op.bf.byp_rslt_en = 0;
	op.bf.op_type = 2;
	op.bf.hash_block_bitmap = 3;
	op.bf.op_mode = op_mode;
	op.bf.entry_index = *index;

	rv = hppe_host_tbl_rd_op_set(dev_id, &op);
	if (SW_OK != rv)
		return rv;
	rv = hppe_host_tbl_rd_op_rslt_get(dev_id, &result);
	if (SW_OK != rv)
		return rv;
	while (!result.bf.valid_cnt && --i) {
		hppe_host_tbl_rd_op_rslt_get(dev_id, &result);
	}
	if (i == 0)
		return SW_BUSY;
	if (result.bf.op_rslt == 0) {
		hppe_reg_tbl_get(
				dev_id,
				IPE_L3_BASE_ADDR + HOST_TBL_RD_RSLT_DATA0_ADDRESS,
				data, num);
		*index = result.bf.entry_index;
		return SW_OK;
	}
	else
		return SW_FAIL;
	
}


sw_error_t
hppe_host_ipv4_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_rd_op_data0_set(dev_id, (union host_tbl_rd_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_rd_op_data1_set(dev_id, (union host_tbl_rd_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_rd_op_data2_set(dev_id, (union host_tbl_rd_op_data2_u *)(&entry->val[2]));
	}
	return hppe_host_get_common(dev_id, op_mode, index,
					(a_uint32_t *)entry, 3);

}

sw_error_t
hppe_host_ipv6_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_rd_op_data0_set(dev_id, (union host_tbl_rd_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_rd_op_data1_set(dev_id, (union host_tbl_rd_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_rd_op_data2_set(dev_id, (union host_tbl_rd_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_rd_op_data3_set(dev_id, (union host_tbl_rd_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_rd_op_data4_set(dev_id, (union host_tbl_rd_op_data4_u *)(&entry->val[4]));
	}
	return hppe_host_get_common(dev_id, op_mode, index,
					(a_uint32_t *)entry, 5);
}

sw_error_t
hppe_host_ipv4_mcast_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv4_mcast_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_rd_op_data0_set(dev_id, (union host_tbl_rd_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_rd_op_data1_set(dev_id, (union host_tbl_rd_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_rd_op_data2_set(dev_id, (union host_tbl_rd_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_rd_op_data3_set(dev_id, (union host_tbl_rd_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_rd_op_data4_set(dev_id, (union host_tbl_rd_op_data4_u *)(&entry->val[4]));
	}
	return hppe_host_get_common(dev_id, op_mode, index,
					(a_uint32_t *)entry, 5);
}

sw_error_t
hppe_host_ipv6_mcast_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_mcast_tbl_u *entry)
{
	if (op_mode == 0) {
		hppe_host_tbl_rd_op_data0_set(dev_id, (union host_tbl_rd_op_data0_u *)(&entry->val[0]));
		hppe_host_tbl_rd_op_data1_set(dev_id, (union host_tbl_rd_op_data1_u *)(&entry->val[1]));
		hppe_host_tbl_rd_op_data2_set(dev_id, (union host_tbl_rd_op_data2_u *)(&entry->val[2]));
		hppe_host_tbl_rd_op_data3_set(dev_id, (union host_tbl_rd_op_data3_u *)(&entry->val[3]));
		hppe_host_tbl_rd_op_data4_set(dev_id, (union host_tbl_rd_op_data4_u *)(&entry->val[4]));
		hppe_host_tbl_rd_op_data5_set(dev_id, (union host_tbl_rd_op_data5_u *)(&entry->val[5]));
		hppe_host_tbl_rd_op_data6_set(dev_id, (union host_tbl_rd_op_data6_u *)(&entry->val[6]));
		hppe_host_tbl_rd_op_data7_set(dev_id, (union host_tbl_rd_op_data7_u *)(&entry->val[7]));
		hppe_host_tbl_rd_op_data8_set(dev_id, (union host_tbl_rd_op_data8_u *)(&entry->val[8]));
		hppe_host_tbl_rd_op_data9_set(dev_id, (union host_tbl_rd_op_data9_u *)(&entry->val[9]));
	}
	return hppe_host_get_common(dev_id, op_mode, index,
					(a_uint32_t *)entry, 10);
}

sw_error_t
hppe_host_flush_common(a_uint32_t dev_id)
{
	union host_tbl_op_u op;
	union host_tbl_op_rslt_u result;
	a_uint32_t i = 0x100 * 50;
	sw_error_t rv;

	op.bf.cmd_id = host_cmd_id;
	host_cmd_id++;
	op.bf.byp_rslt_en = 0;
	op.bf.op_type = 3;
	op.bf.hash_block_bitmap = 3;
	op.bf.op_mode = 0;

	rv = hppe_host_tbl_op_set(dev_id, &op);
	if (SW_OK != rv)
		return rv;
	rv = hppe_host_tbl_op_rslt_get(dev_id, &result);
	if (SW_OK != rv)
		return rv;
	while (!result.bf.valid_cnt && --i) {
		hppe_host_tbl_op_rslt_get(dev_id, &result);
	}
	if (i == 0)
		return SW_BUSY;
	if (result.bf.op_rslt == 0)
		return SW_OK;
	else
		return SW_FAIL;
	
	
}
#endif
