/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#ifndef _CPPE_QOS_H_
#define _CPPE_QOS_H_

#define QOS_MAPPING_TBL_MAX_ENTRY	2592
#define QOS_MAPPING_FLOW_TBL_MAX_ENTRY	2048
#define QOS_MAPPING_DSCP_TBL_MAX_ENTRY	256
#define QOS_MAPPING_PCP_TBL_MAX_ENTRY	16
#define QOS_MAPPING_TBL_MAX_GROUP	2


sw_error_t
cppe_qos_mapping_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union qos_mapping_tbl_u *value);

sw_error_t
cppe_qos_mapping_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union qos_mapping_tbl_u *value);

sw_error_t
cppe_qos_mapping_tbl_int_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dei_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dei_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_dscp_tc_mask_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_dscp_tc_mask_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dscp_tc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dscp_tc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_pri_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_pri_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_qos_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_qos_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_pcp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_pcp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_qos_mapping_tbl_int_dscp_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_qos_mapping_tbl_int_dscp_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif