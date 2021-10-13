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
#ifndef _HPPE_ACL_H_
#define _HPPE_ACL_H_

#define IPO_RULE_REG_MAX_ENTRY	512
#define IPO_MASK_REG_MAX_ENTRY	512
#define RULE_EXT_1_REG_MAX_ENTRY	64
#define RULE_EXT_2_REG_MAX_ENTRY	64
#define RULE_EXT_4_REG_MAX_ENTRY	64
#define IPO_ACTION_MAX_ENTRY	512
#define IPO_CNT_TBL_MAX_ENTRY	512

sw_error_t
hppe_non_ip_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_get(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_set(
		a_uint32_t dev_id,
		union udf_ctrl_reg_u *value);

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_non_ip_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv4_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf0_ctrl_reg_udf0_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf1_ctrl_reg_udf1_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf2_ctrl_reg_udf2_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_base_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_base_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipv6_udf3_ctrl_reg_udf3_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_rule_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_rule_reg_u *value);

sw_error_t
hppe_ipo_rule_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_rule_reg_u *value);

sw_error_t
hppe_ipo_mask_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_mask_reg_u *value);

sw_error_t
hppe_ipo_mask_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_mask_reg_u *value);

sw_error_t
hppe_rule_ext_1_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_1_reg_u *value);

sw_error_t
hppe_rule_ext_1_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_1_reg_u *value);

sw_error_t
hppe_rule_ext_2_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_2_reg_u *value);

sw_error_t
hppe_rule_ext_2_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_2_reg_u *value);

sw_error_t
hppe_rule_ext_4_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_4_reg_u *value);

sw_error_t
hppe_rule_ext_4_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rule_ext_4_reg_u *value);

sw_error_t
hppe_ipo_dbg_addr_reg_get(
		a_uint32_t dev_id,
		union ipo_dbg_addr_reg_u *value);

sw_error_t
hppe_ipo_dbg_addr_reg_set(
		a_uint32_t dev_id,
		union ipo_dbg_addr_reg_u *value);

sw_error_t
hppe_ipo_dbg_data_reg_get(
		a_uint32_t dev_id,
		union ipo_dbg_data_reg_u *value);

sw_error_t
hppe_ipo_dbg_data_reg_set(
		a_uint32_t dev_id,
		union ipo_dbg_data_reg_u *value);

sw_error_t
hppe_ipo_spare_reg_reg_get(
		a_uint32_t dev_id,
		union ipo_spare_reg_reg_u *value);

sw_error_t
hppe_ipo_spare_reg_reg_set(
		a_uint32_t dev_id,
		union ipo_spare_reg_reg_u *value);

sw_error_t
hppe_ipo_glb_hit_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_hit_counter_reg_u *value);

sw_error_t
hppe_ipo_glb_hit_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_hit_counter_reg_u *value);

sw_error_t
hppe_ipo_glb_miss_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_miss_counter_reg_u *value);

sw_error_t
hppe_ipo_glb_miss_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_miss_counter_reg_u *value);

sw_error_t
hppe_ipo_glb_bypass_counter_reg_get(
		a_uint32_t dev_id,
		union ipo_glb_bypass_counter_reg_u *value);

sw_error_t
hppe_ipo_glb_bypass_counter_reg_set(
		a_uint32_t dev_id,
		union ipo_glb_bypass_counter_reg_u *value);

sw_error_t
hppe_ipo_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_cnt_tbl_u *value);

sw_error_t
hppe_ipo_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_cnt_tbl_u *value);

sw_error_t
hppe_ipo_cnt_tbl_hit_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_ipo_cnt_tbl_hit_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_ipo_cnt_tbl_hit_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_cnt_tbl_hit_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);
sw_error_t
hppe_ipo_rule_reg_src_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_src_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_inverse_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_inverse_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_rule_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_rule_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_src_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_src_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_range_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_range_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_post_routing_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_post_routing_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_fake_mac_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_fake_mac_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_res_chain_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_res_chain_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_rule_reg_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_rule_reg_rule_field_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_ipo_rule_reg_rule_field_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_ipo_mask_reg_maskfield_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_ipo_mask_reg_maskfield_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_rule_ext_1_reg_ext2_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_1_reg_ext2_2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_1_reg_ext2_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_1_reg_ext2_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_1_reg_ext2_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_1_reg_ext2_3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_1_reg_ext2_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_1_reg_ext2_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_2_reg_ext4_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_2_reg_ext4_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_2_reg_ext4_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_2_reg_ext4_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rule_ext_4_reg_ext8_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rule_ext_4_reg_ext8_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_dbg_addr_reg_ipo_dbg_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_dbg_addr_reg_ipo_dbg_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_dbg_data_reg_ipo_dbg_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_dbg_data_reg_ipo_dbg_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_spare_reg_reg_spare_reg_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_spare_reg_reg_spare_reg_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_glb_hit_counter_reg_hit_count_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_glb_hit_counter_reg_hit_count_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_glb_miss_counter_reg_miss_count_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_glb_miss_counter_reg_miss_count_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_glb_bypass_counter_reg_bypass_count_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ipo_glb_bypass_counter_reg_bypass_count_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ipo_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_action_u *value);

sw_error_t
hppe_ipo_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipo_action_u *value);

sw_error_t
hppe_ipo_action_mirror_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_mirror_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_ctag_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_ctag_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_int_dp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_int_dp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_enqueue_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_enqueue_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_stag_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_stag_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_dscp_tc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_dscp_tc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_cpu_code_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_cpu_code_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_stag_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_stag_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_ctag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_ctag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_dest_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_dest_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_dest_info_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_dest_info_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_policer_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_policer_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_ctag_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_ctag_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_metadata_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_metadata_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_enqueue_pri_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_enqueue_pri_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_stag_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_stag_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_ctag_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_ctag_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_policer_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_policer_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_ctag_dei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_ctag_dei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_stag_pcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_stag_pcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_syn_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_syn_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_service_code_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_service_code_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_qid_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_qid_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_cvid_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_cvid_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_svid_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_svid_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_cpu_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_cpu_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_dscp_tc_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_dscp_tc_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_qid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_qid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipo_action_stag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipo_action_stag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);


#endif

