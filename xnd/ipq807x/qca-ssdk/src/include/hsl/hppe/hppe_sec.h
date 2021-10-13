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
#ifndef _HPPE_SEC_H_
#define _HPPE_SEC_H_

#define L3_EXCEPTION_CMD_MAX_ENTRY	72
#define L3_EXP_L3_ONLY_CTRL_MAX_ENTRY	72
#define L3_EXP_L2_ONLY_CTRL_MAX_ENTRY	72
#define L3_EXP_L2_FLOW_CTRL_MAX_ENTRY	72
#define L3_EXP_L3_FLOW_CTRL_MAX_ENTRY	72
#define L3_EXP_MULTICAST_CTRL_MAX_ENTRY	72

sw_error_t
hppe_l3_exception_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exception_cmd_u *value);

sw_error_t
hppe_l3_exception_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exception_cmd_u *value);

sw_error_t
hppe_l3_exp_l3_only_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_only_ctrl_u *value);

sw_error_t
hppe_l3_exp_l3_only_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_only_ctrl_u *value);

sw_error_t
hppe_l3_exp_l2_only_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_only_ctrl_u *value);

sw_error_t
hppe_l3_exp_l2_only_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_only_ctrl_u *value);

sw_error_t
hppe_l3_exp_l2_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_flow_ctrl_u *value);

sw_error_t
hppe_l3_exp_l2_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l2_flow_ctrl_u *value);

sw_error_t
hppe_l3_exp_l3_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_flow_ctrl_u *value);

sw_error_t
hppe_l3_exp_l3_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_l3_flow_ctrl_u *value);

sw_error_t
hppe_l3_exp_multicast_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_multicast_ctrl_u *value);

sw_error_t
hppe_l3_exp_multicast_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l3_exp_multicast_ctrl_u *value);

sw_error_t
hppe_l3_exception_cmd_l3_excep_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exception_cmd_l3_excep_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exception_cmd_de_acce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exception_cmd_de_acce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exp_l3_only_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exp_l3_only_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exp_l2_only_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exp_l2_only_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exp_l2_flow_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exp_l2_flow_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exp_l3_flow_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exp_l3_flow_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exp_multicast_ctrl_excep_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l3_exp_multicast_ctrl_excep_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_get(
		a_uint32_t dev_id,
		union l3_exception_parsing_ctrl_reg_u *value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_set(
		a_uint32_t dev_id,
		union l3_exception_parsing_ctrl_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_0_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_0_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_1_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_1_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_2_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_2_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_get(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_3_reg_u *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_set(
		a_uint32_t dev_id,
		union l4_exception_parsing_ctrl_3_reg_u *value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_hop_limit_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_hop_limit_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_ttl_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l3_exception_parsing_ctrl_reg_small_ttl_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags0_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_0_reg_tcp_flags1_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags2_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_1_reg_tcp_flags3_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags4_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_2_reg_tcp_flags5_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags6_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_mask_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l4_exception_parsing_ctrl_3_reg_tcp_flags7_mask_set(
		a_uint32_t dev_id,
		unsigned int value);

#endif
