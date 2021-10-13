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
#include "hppe_vsi_reg.h"
#include "hppe_vsi.h"

sw_error_t
hppe_vsi_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vsi_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + VSI_TBL_ADDRESS + \
				index * VSI_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_vsi_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vsi_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + VSI_TBL_ADDRESS + \
				index * VSI_TBL_INC,
				value->val,
				2);
}

#ifndef IN_VSI_MINI
sw_error_t
hppe_vsi_tbl_umc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.umc_bitmap;
	return ret;
}

sw_error_t
hppe_vsi_tbl_umc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.umc_bitmap = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_station_move_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.station_move_lrn_en;
	return ret;
}

sw_error_t
hppe_vsi_tbl_station_move_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.station_move_lrn_en = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_new_addr_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.new_addr_fwd_cmd;
	return ret;
}

sw_error_t
hppe_vsi_tbl_new_addr_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.new_addr_fwd_cmd = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_uuc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.uuc_bitmap;
	return ret;
}

sw_error_t
hppe_vsi_tbl_uuc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uuc_bitmap = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_member_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.member_port_bitmap;
	return ret;
}

sw_error_t
hppe_vsi_tbl_member_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.member_port_bitmap = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_new_addr_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.new_addr_lrn_en;
	return ret;
}

sw_error_t
hppe_vsi_tbl_new_addr_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.new_addr_lrn_en = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_bc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bc_bitmap;
	return ret;
}

sw_error_t
hppe_vsi_tbl_bc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bc_bitmap = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vsi_tbl_station_move_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.station_move_fwd_cmd;
	return ret;
}

sw_error_t
hppe_vsi_tbl_station_move_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vsi_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vsi_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.station_move_fwd_cmd = value;
	ret = hppe_vsi_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif

sw_error_t
hppe_vlan_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VLAN_CNT_TBL_ADDRESS + \
				index * VLAN_CNT_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_vlan_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + VLAN_CNT_TBL_ADDRESS + \
				index * VLAN_CNT_TBL_INC,
				value->val,
				3);
}

#ifndef IN_VSI_MINI
sw_error_t
hppe_vlan_cnt_tbl_rx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union vlan_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.rx_byte_cnt_1 << 32 | \
		reg_val.bf.rx_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_vlan_cnt_tbl_rx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union vlan_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_byte_cnt_1 = value >> 32;
	reg_val.bf.rx_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_vlan_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vlan_cnt_tbl_rx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vlan_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pkt_cnt;
	return ret;
}

sw_error_t
hppe_vlan_cnt_tbl_rx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vlan_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vlan_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_pkt_cnt = value;
	ret = hppe_vlan_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif


sw_error_t
hppe_eg_vsi_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_counter_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VSI_COUNTER_TBL_ADDRESS + \
				index * EG_VSI_COUNTER_TBL_INC,
				value->val,
				3);
}

sw_error_t
hppe_eg_vsi_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_counter_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EG_VSI_COUNTER_TBL_ADDRESS + \
				index * EG_VSI_COUNTER_TBL_INC,
				value->val,
				3);
}

#ifndef IN_VSI_MINI
sw_error_t
hppe_eg_vsi_counter_tbl_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union eg_vsi_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_counter_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_bytes_1 << 32 | \
		reg_val.bf.tx_bytes_0;
	return ret;
}

sw_error_t
hppe_eg_vsi_counter_tbl_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union eg_vsi_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_bytes_1 = value >> 32;
	reg_val.bf.tx_bytes_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_eg_vsi_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_eg_vsi_counter_tbl_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union eg_vsi_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_counter_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_packets;
	return ret;
}

sw_error_t
hppe_eg_vsi_counter_tbl_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union eg_vsi_counter_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eg_vsi_counter_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_packets = value;
	ret = hppe_eg_vsi_counter_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif

sw_error_t
hppe_pre_l2_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pre_l2_cnt_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PRE_L2_CNT_TBL_ADDRESS + \
				index * PRE_L2_CNT_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_pre_l2_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pre_l2_cnt_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				INGRESS_POLICER_BASE_ADDR + PRE_L2_CNT_TBL_ADDRESS + \
				index * PRE_L2_CNT_TBL_INC,
				value->val,
				5);
}
#ifndef IN_VSI_MINI
sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.rx_drop_byte_cnt_1 << 24 | \
		reg_val.bf.rx_drop_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_drop_byte_cnt_1 = value >> 24;
	reg_val.bf.rx_drop_byte_cnt_0 = value & (((a_uint64_t)1<<24)-1);
	ret = hppe_pre_l2_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.rx_byte_cnt_1 << 32 | \
		reg_val.bf.rx_byte_cnt_0;
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_byte_cnt_1 = value >> 32;
	reg_val.bf.rx_byte_cnt_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_pre_l2_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pkt_cnt;
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_pkt_cnt = value;
	ret = hppe_pre_l2_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_drop_pkt_cnt_1 << 24 | \
		reg_val.bf.rx_drop_pkt_cnt_0;
	return ret;
}

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pre_l2_cnt_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pre_l2_cnt_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_drop_pkt_cnt_1 = value >> 24;
	reg_val.bf.rx_drop_pkt_cnt_0 = value & (((a_uint64_t)1<<24)-1);
	ret = hppe_pre_l2_cnt_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif
