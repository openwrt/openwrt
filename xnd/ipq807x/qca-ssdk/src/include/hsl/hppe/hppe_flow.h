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
#ifndef _HPPE_FLOW_H_
#define _HPPE_FLOW_H_

#define FLOW_CTRL1_MAX_ENTRY	3
#define IN_FLOW_3TUPLE_TBL_MAX_ENTRY	4096
#define IN_FLOW_IPV6_3TUPLE_TBL_MAX_ENTRY	2048
#define IN_FLOW_IPV6_5TUPLE_TBL_MAX_ENTRY	2048
#define IN_FLOW_TBL_MAX_ENTRY	4096
#define EG_FLOW_TREE_MAP_TBL_MAX_ENTRY	4096
#define IN_FLOW_CNT_TBL_MAX_ENTRY	4096

sw_error_t
hppe_in_flow_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_cnt_tbl_u *value);

sw_error_t
hppe_in_flow_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_cnt_tbl_u *value);

sw_error_t
hppe_flow_ctrl0_get(
		a_uint32_t dev_id,
		union flow_ctrl0_u *value);

sw_error_t
hppe_flow_ctrl0_set(
		a_uint32_t dev_id,
		union flow_ctrl0_u *value);

sw_error_t
hppe_flow_ctrl1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_ctrl1_u *value);

sw_error_t
hppe_flow_ctrl1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_ctrl1_u *value);


sw_error_t
hppe_in_flow_tbl_op_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_u *value);

sw_error_t
hppe_in_flow_tbl_op_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_u *value);

sw_error_t
hppe_in_flow_host_tbl_op_get(
		a_uint32_t dev_id,
		union in_flow_host_tbl_op_u *value);

sw_error_t
hppe_in_flow_host_tbl_op_set(
		a_uint32_t dev_id,
		union in_flow_host_tbl_op_u *value);

sw_error_t
hppe_in_flow_tbl_op_data0_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data0_u *value);

sw_error_t
hppe_in_flow_tbl_op_data0_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data0_u *value);

sw_error_t
hppe_in_flow_tbl_op_data1_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data1_u *value);

sw_error_t
hppe_in_flow_tbl_op_data1_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data1_u *value);

sw_error_t
hppe_in_flow_tbl_op_data2_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data2_u *value);

sw_error_t
hppe_in_flow_tbl_op_data2_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data2_u *value);

sw_error_t
hppe_in_flow_tbl_op_data3_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data3_u *value);

sw_error_t
hppe_in_flow_tbl_op_data3_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data3_u *value);

sw_error_t
hppe_in_flow_tbl_op_data4_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data4_u *value);

sw_error_t
hppe_in_flow_tbl_op_data4_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data4_u *value);

sw_error_t
hppe_in_flow_tbl_op_data5_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data5_u *value);

sw_error_t
hppe_in_flow_tbl_op_data5_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data5_u *value);

sw_error_t
hppe_in_flow_tbl_op_data6_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data6_u *value);

sw_error_t
hppe_in_flow_tbl_op_data6_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data6_u *value);

sw_error_t
hppe_in_flow_tbl_op_data7_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data7_u *value);

sw_error_t
hppe_in_flow_tbl_op_data7_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data7_u *value);

sw_error_t
hppe_in_flow_tbl_op_data8_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data8_u *value);

sw_error_t
hppe_in_flow_tbl_op_data8_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_op_data0_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data0_u *value);

sw_error_t
hppe_flow_host_tbl_op_data0_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data0_u *value);

sw_error_t
hppe_flow_host_tbl_op_data1_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data1_u *value);

sw_error_t
hppe_flow_host_tbl_op_data1_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data1_u *value);

sw_error_t
hppe_flow_host_tbl_op_data2_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data2_u *value);

sw_error_t
hppe_flow_host_tbl_op_data2_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data2_u *value);

sw_error_t
hppe_flow_host_tbl_op_data3_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data3_u *value);

sw_error_t
hppe_flow_host_tbl_op_data3_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data3_u *value);

sw_error_t
hppe_flow_host_tbl_op_data4_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data4_u *value);

sw_error_t
hppe_flow_host_tbl_op_data4_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data4_u *value);

sw_error_t
hppe_flow_host_tbl_op_data5_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data5_u *value);

sw_error_t
hppe_flow_host_tbl_op_data5_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data5_u *value);

sw_error_t
hppe_flow_host_tbl_op_data6_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data6_u *value);

sw_error_t
hppe_flow_host_tbl_op_data6_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data6_u *value);

sw_error_t
hppe_flow_host_tbl_op_data7_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data7_u *value);

sw_error_t
hppe_flow_host_tbl_op_data7_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data7_u *value);

sw_error_t
hppe_flow_host_tbl_op_data8_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_op_data8_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_op_data9_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data9_u *value);

sw_error_t
hppe_flow_host_tbl_op_data9_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_data9_u *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_get(
		a_uint32_t dev_id,
		union in_flow_tbl_op_rslt_u *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_set(
		a_uint32_t dev_id,
		union in_flow_tbl_op_rslt_u *value);

sw_error_t
hppe_flow_host_tbl_op_rslt_get(
		a_uint32_t dev_id,
		union flow_host_tbl_op_rslt_u *value);

sw_error_t
hppe_flow_host_tbl_op_rslt_set(
		a_uint32_t dev_id,
		union flow_host_tbl_op_rslt_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_u *value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_get(
		a_uint32_t dev_id,
		union in_flow_host_tbl_rd_op_u *value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_set(
		a_uint32_t dev_id,
		union in_flow_host_tbl_rd_op_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data0_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data0_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data0_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data0_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data1_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data1_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data1_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data1_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data2_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data2_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data2_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data2_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data3_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data3_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data3_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data3_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data4_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data4_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data4_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data4_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data5_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data5_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data5_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data5_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data6_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data6_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data6_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data6_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data7_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data7_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data7_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data7_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data8_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data8_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data8_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data0_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data0_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data0_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data0_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data1_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data1_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data1_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data1_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data2_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data2_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data2_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data2_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data3_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data3_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data3_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data3_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data4_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data4_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data4_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data4_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data5_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data5_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data5_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data5_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data6_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data6_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data6_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data6_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data7_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data7_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data7_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data7_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data8_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data8_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data9_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data9_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data9_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_data9_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_rslt_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_flow_host_tbl_rd_op_rslt_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data0_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data0_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data0_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data0_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data1_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data1_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data1_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data1_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data2_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data2_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data2_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data2_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data3_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data3_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data3_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data3_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data4_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data4_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data4_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data4_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data5_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data5_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data5_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data5_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data6_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data6_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data6_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data6_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data7_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data7_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data7_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data7_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data8_get(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data8_u *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data8_set(
		a_uint32_t dev_id,
		union in_flow_tbl_rd_rslt_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data0_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data0_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data0_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data0_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data1_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data1_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data1_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data1_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data2_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data2_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data2_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data2_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data3_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data3_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data3_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data3_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data4_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data4_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data4_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data4_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data5_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data5_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data5_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data5_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data6_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data6_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data6_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data6_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data7_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data7_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data7_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data7_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data8_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data8_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data8_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data9_get(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data9_u *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data9_set(
		a_uint32_t dev_id,
		union flow_host_tbl_rd_rslt_data9_u *value);

sw_error_t
hppe_in_flow_3tuple_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_3tuple_tbl_u *value);

sw_error_t
hppe_in_flow_3tuple_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_3tuple_tbl_u *value);

sw_error_t
hppe_in_flow_ipv6_3tuple_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_ipv6_3tuple_tbl_u *value);

sw_error_t
hppe_in_flow_ipv6_3tuple_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_ipv6_3tuple_tbl_u *value);

sw_error_t
hppe_in_flow_ipv6_5tuple_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_ipv6_5tuple_tbl_u *value);

sw_error_t
hppe_in_flow_ipv6_5tuple_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_ipv6_5tuple_tbl_u *value);

sw_error_t
hppe_in_flow_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_tbl_u *value);

sw_error_t
hppe_in_flow_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_flow_tbl_u *value);

sw_error_t
hppe_eg_flow_tree_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_flow_tree_map_tbl_u *value);

sw_error_t
hppe_eg_flow_tree_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_flow_tree_map_tbl_u *value);

sw_error_t
hppe_flow_ctrl0_flow_hash_mode_0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_ctrl0_flow_hash_mode_0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_ctrl0_flow_age_timer_unit_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_ctrl0_flow_age_timer_unit_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_ctrl0_flow_hash_mode_1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_ctrl0_flow_hash_mode_1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_ctrl0_flow_age_timer_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_ctrl0_flow_age_timer_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_ctrl0_flow_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_ctrl0_flow_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_frag_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_frag_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_key_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_key_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_key_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_key_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_frag_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_frag_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_miss_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_miss_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_key_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_key_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_tcp_special_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_tcp_special_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_tcp_special_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_tcp_special_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_frag_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_frag_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_tcp_special_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_tcp_special_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_miss_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_miss_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_frag_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_frag_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_tcp_special_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl1_tcp_special_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_key_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_key_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_miss_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_miss_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_key_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_key_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_tcp_special_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl0_tcp_special_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_frag_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl2_frag_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_miss_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl3_miss_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_miss_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_ctrl1_flow_ctl4_miss_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_flow_tbl_op_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_op_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_op_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_op_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_op_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_op_host_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_op_host_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_op_result_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_op_result_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_busy_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_busy_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_host_tbl_op_host_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_host_tbl_op_host_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_host_tbl_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_host_tbl_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_data9_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_data9_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_rslt_flow_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_flow_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_op_rslt_host_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_op_rslt_host_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_host_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_host_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_result_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_op_result_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_busy_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_busy_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_host_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_host_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_host_tbl_rd_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_data9_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_data9_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_flow_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_flow_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_op_rslt_host_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_op_rslt_host_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_flow_tbl_rd_rslt_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data3_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data3_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data4_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data4_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data5_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data5_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data6_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data6_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data7_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data7_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data8_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data8_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data9_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flow_host_tbl_rd_rslt_data9_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_flow_tree_map_tbl_tree_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_flow_tree_map_tbl_tree_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_flow_cnt_tbl_hit_byte_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_in_flow_cnt_tbl_hit_byte_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_in_flow_cnt_tbl_hit_pkt_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_flow_cnt_tbl_hit_pkt_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_ipv4_5tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);

sw_error_t
hppe_flow_ipv4_3tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_5tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_3tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);

sw_error_t
hppe_flow_flush_common(a_uint32_t dev_id);

sw_error_t
hppe_flow_ipv4_5tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);

sw_error_t
hppe_flow_ipv4_3tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_5tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_3tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv4_5tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);

sw_error_t
hppe_flow_ipv4_3tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_5tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);

sw_error_t
hppe_flow_ipv6_3tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);


#include "hppe_ip_reg.h"

sw_error_t
hppe_flow_host_get_common(
		a_uint32_t dev_id,
		a_uint32_t op_mode,
		a_uint32_t *index,
		a_uint32_t *data,
		a_uint32_t num);


sw_error_t
hppe_flow_host_flush_common(a_uint32_t dev_id);


sw_error_t
hppe_flow_host_op_both_common(
		a_uint32_t dev_id,
		a_uint32_t op_type,
		a_uint32_t op_mode,
		a_uint32_t *index);


sw_error_t
hppe_flow_entry_host_op_ipv4_5tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv4_3tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_5tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_3tuple_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv4_5tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv4_3tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_5tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_3tuple_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv4_5tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv4_3tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_5tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_5tuple_tbl_u *entry);


sw_error_t
hppe_flow_entry_host_op_ipv6_3tuple_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union in_flow_ipv6_3tuple_tbl_u *entry);


sw_error_t
hppe_flow_host_data_op_common(
		a_uint32_t dev_id,
		a_uint32_t op_type,
		a_uint32_t op_mode,
		a_uint32_t *index);


sw_error_t
hppe_flow_host_ipv4_data_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry);


sw_error_t
hppe_flow_host_ipv6_data_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry);


sw_error_t
hppe_flow_host_ipv4_data_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry);


sw_error_t
hppe_flow_host_ipv6_data_get(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry);


sw_error_t
hppe_flow_host_ipv4_data_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry);


sw_error_t
hppe_flow_host_ipv6_data_del(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry);

sw_error_t
hppe_flow_host_ipv4_data_rd_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_tbl_u *entry);

sw_error_t
hppe_flow_host_ipv6_data_rd_add(
		a_uint32_t dev_id, a_uint32_t op_mode,
		a_uint32_t *index, union host_ipv6_tbl_u *entry);

#endif
