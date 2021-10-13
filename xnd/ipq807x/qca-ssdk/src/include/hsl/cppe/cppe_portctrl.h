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
#ifndef _CPPE_PORTCTRL_H_
#define _CPPE_PORTCTRL_H_
#include "cppe_portctrl_reg.h"

#define CPPE_MRU_MTU_CTRL_TBL_MAX_ENTRY	256

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_force_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_force_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_force_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pcp_res_prec_force_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_post_acl_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_post_acl_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_preheader_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_preheader_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pre_acl_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_pre_acl_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_flow_res_prec_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_flow_res_prec_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_dscp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_src_profile_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_src_profile_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_source_filter_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cppe_mru_mtu_ctrl_tbl_u *value);

sw_error_t
cppe_mru_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union cppe_mru_mtu_ctrl_tbl_u *value);

sw_error_t
cppe_port_phy_status_1_get(
		a_uint32_t dev_id,
		union cppe_port_phy_status_1_u *value);

sw_error_t
cppe_port5_pcs1_phy_status_get(
		a_uint32_t dev_id,
		unsigned int *value);

#endif
