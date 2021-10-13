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
#ifndef _HPPE_PORTVLAN_H_
#define _HPPE_PORTVLAN_H_

#include "hppe_portvlan_reg.h"

#define XLT_RULE_TBL_MAX_ENTRY	64
#define XLT_ACTION_TBL_MAX_ENTRY	64
#define PORT_PARSING_REG_MAX_ENTRY	8
#define EG_VLAN_XLT_RULE_MAX_ENTRY	64
#define PORT_EG_DEF_VID_MAX_ENTRY	8
#define PORT_EG_VLAN_MAX_ENTRY	8
#define EG_VLAN_XLT_ACTION_MAX_ENTRY	64


sw_error_t
hppe_port_parsing_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_parsing_reg_u *value);

sw_error_t
hppe_port_parsing_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_parsing_reg_u *value);

sw_error_t
hppe_edma_vlan_tpid_reg_get(
		a_uint32_t dev_id,
		union edma_vlan_tpid_reg_u *value);

sw_error_t
hppe_edma_vlan_tpid_reg_set(
		a_uint32_t dev_id,
		union edma_vlan_tpid_reg_u *value);

sw_error_t
hppe_vlan_tpid_reg_get(
		a_uint32_t dev_id,
		union vlan_tpid_reg_u *value);

sw_error_t
hppe_vlan_tpid_reg_set(
		a_uint32_t dev_id,
		union vlan_tpid_reg_u *value);

sw_error_t
hppe_port_parsing_reg_port_role_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_parsing_reg_port_role_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vlan_tpid_reg_stag_tpid_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_vlan_tpid_reg_stag_tpid_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_vlan_tpid_reg_ctag_tpid_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_vlan_tpid_reg_ctag_tpid_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_bridge_config_get(
		a_uint32_t dev_id,
		union bridge_config_u *value);

sw_error_t
hppe_bridge_config_set(
		a_uint32_t dev_id,
		union bridge_config_u *value);

sw_error_t
hppe_port_def_vid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_vid_u *value);

sw_error_t
hppe_port_def_vid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_vid_u *value);

sw_error_t
hppe_port_def_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_pcp_u *value);

sw_error_t
hppe_port_def_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_def_pcp_u *value);

sw_error_t
hppe_port_vlan_config_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_vlan_config_u *value);

sw_error_t
hppe_port_vlan_config_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		union port_vlan_config_u *value);

sw_error_t
hppe_iv_dbg_addr_get(
		a_uint32_t dev_id,
		union iv_dbg_addr_u *value);

sw_error_t
hppe_iv_dbg_addr_set(
		a_uint32_t dev_id,
		union iv_dbg_addr_u *value);

sw_error_t
hppe_iv_dbg_data_get(
		a_uint32_t dev_id,
		union iv_dbg_data_u *value);

sw_error_t
hppe_iv_dbg_data_set(
		a_uint32_t dev_id,
		union iv_dbg_data_u *value);

sw_error_t
hppe_eco_reserve_get(
		a_uint32_t dev_id,
		union eco_reserve_u *value);

sw_error_t
hppe_eco_reserve_set(
		a_uint32_t dev_id,
		union eco_reserve_u *value);

sw_error_t
hppe_xlt_rule_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_rule_tbl_u *value);

sw_error_t
hppe_xlt_rule_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_rule_tbl_u *value);

sw_error_t
hppe_xlt_action_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_action_tbl_u *value);

sw_error_t
hppe_xlt_action_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union xlt_action_tbl_u *value);

sw_error_t
hppe_bridge_config_bridge_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_bridge_config_bridge_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_port_def_vid_port_def_cvid_en_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_vid_port_def_cvid_en_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_vid_port_def_svid_en_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_vid_port_def_svid_en_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_vid_port_def_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_vid_port_def_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_vid_port_def_svid_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_vid_port_def_svid_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_pcp_port_def_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_pcp_port_def_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_pcp_port_def_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_pcp_port_def_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_pcp_port_def_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_pcp_port_def_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_def_pcp_port_def_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_def_pcp_port_def_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_in_dei_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_in_dei_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_in_pcp_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_in_pcp_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_untag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_untag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_in_vlan_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_in_vlan_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_pri_tag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_pri_tag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_vlan_xlt_miss_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_vlan_xlt_miss_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_port_vlan_config_port_tag_fltr_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int *value);

sw_error_t
hppe_port_vlan_config_port_tag_fltr_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t port_id,
		unsigned int value);

sw_error_t
hppe_iv_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_iv_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_iv_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_iv_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eco_reserve_eco_res_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eco_reserve_eco_res_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_frm_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_frm_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_prot_value_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_prot_value_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_frm_type_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_frm_type_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_prot_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_prot_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_ckey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_rule_tbl_skey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_rule_tbl_skey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_dei_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_dei_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cvid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_counter_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_counter_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_vid_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_vid_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_counter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_counter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_svid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_svid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_vsi_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_vsi_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cpcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_pcp_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_pcp_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_xlt_action_tbl_xlt_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_rule_u *value);

sw_error_t
hppe_eg_vlan_xlt_rule_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_rule_u *value);

sw_error_t
hppe_eg_vsi_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_tag_u *value);

sw_error_t
hppe_eg_vsi_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_tag_u *value);

sw_error_t
hppe_port_eg_def_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_def_vid_u *value);

sw_error_t
hppe_port_eg_def_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_def_vid_u *value);

sw_error_t
hppe_port_eg_vlan_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_vlan_u *value);

sw_error_t
hppe_port_eg_vlan_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_eg_vlan_u *value);

sw_error_t
hppe_eg_vlan_tpid_get(
		a_uint32_t dev_id,
		union eg_vlan_tpid_u *value);

sw_error_t
hppe_eg_vlan_tpid_set(
		a_uint32_t dev_id,
		union eg_vlan_tpid_u *value);

sw_error_t
hppe_eg_bridge_config_get(
		a_uint32_t dev_id,
		union eg_bridge_config_u *value);

sw_error_t
hppe_eg_bridge_config_set(
		a_uint32_t dev_id,
		union eg_bridge_config_u *value);

sw_error_t
hppe_eg_vlan_xlt_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_action_u *value);

sw_error_t
hppe_eg_vlan_xlt_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vlan_xlt_action_u *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_ckey_pcp_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_vid_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_incl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_skey_dei_incl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_rule_vsi_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vsi_tag_tagged_mode_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vsi_tag_tagged_mode_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_def_vid_port_def_svid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_def_vid_port_def_svid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_def_vid_port_def_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_def_vid_port_def_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_def_vid_port_def_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_tx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_tx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_ctag_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_ctag_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_port_eg_pcp_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_port_eg_pcp_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_vsi_tag_mode_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_vsi_tag_mode_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_stag_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_port_eg_vlan_stag_mode_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_port_eg_dei_prop_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_port_eg_dei_prop_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_eg_vlan_port_vlan_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_eg_vlan_port_vlan_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_tpid_ctpid_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eg_vlan_tpid_ctpid_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_vlan_tpid_stpid_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eg_vlan_tpid_stpid_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_bridge_config_bridge_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eg_bridge_config_bridge_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_bridge_config_pkt_l2_edit_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value);
sw_error_t
hppe_eg_bridge_config_pkt_l2_edit_en_set(
		a_uint32_t dev_id,
		a_uint32_t value);
sw_error_t
hppe_eg_bridge_config_queue_cnt_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eg_bridge_config_queue_cnt_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_vlan_xlt_action_dei_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_dei_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cvid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_counter_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_counter_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_vid_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_vid_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_counter_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_counter_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cpcp_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_pcp_swap_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_pcp_swap_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vlan_xlt_action_xlt_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_tx_counter_tbl_u *value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_dev_tx_counter_tbl_u *value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vlan_dev_tx_counter_tbl_tx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

