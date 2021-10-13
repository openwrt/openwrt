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
#ifndef _HPPE_FDB_H_
#define _HPPE_FDB_H_

#define PRE_L2_CNT_TBL_MAX_ENTRY	32
#define PORT_BRIDGE_CTRL_MAX_ENTRY	8
#define PORT_LRN_LIMIT_CTRL_MAX_ENTRY	8
#define PORT_LRN_LIMIT_COUNTER_MAX_ENTRY	8
#define RFDB_TBL_MAX_ENTRY	32
#define FDB_TBL_MAX_ENTRY	2048


sw_error_t
hppe_l2_dbg_addr_get(
		a_uint32_t dev_id,
		union l2_dbg_addr_u *value);

sw_error_t
hppe_l2_dbg_addr_set(
		a_uint32_t dev_id,
		union l2_dbg_addr_u *value);

sw_error_t
hppe_l2_dbg_data_get(
		a_uint32_t dev_id,
		union l2_dbg_data_u *value);

sw_error_t
hppe_l2_dbg_data_set(
		a_uint32_t dev_id,
		union l2_dbg_data_u *value);

sw_error_t
hppe_fdb_tbl_op_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_u *value);

sw_error_t
hppe_fdb_tbl_op_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_u *value);

sw_error_t
hppe_fdb_tbl_op_rslt_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_rslt_u *value);

sw_error_t
hppe_fdb_tbl_op_rslt_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_rslt_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_u *value);

sw_error_t
hppe_age_timer_get(
		a_uint32_t dev_id,
		union age_timer_u *value);

sw_error_t
hppe_age_timer_set(
		a_uint32_t dev_id,
		union age_timer_u *value);

sw_error_t
hppe_l2_global_conf_get(
		a_uint32_t dev_id,
		union l2_global_conf_u *value);

sw_error_t
hppe_l2_global_conf_set(
		a_uint32_t dev_id,
		union l2_global_conf_u *value);

sw_error_t
hppe_l2_dbgcnt_cmd_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_cmd_u *value);

sw_error_t
hppe_l2_dbgcnt_cmd_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_cmd_u *value);

sw_error_t
hppe_l2_dbgcnt_rdata_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_rdata_u *value);

sw_error_t
hppe_l2_dbgcnt_rdata_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_rdata_u *value);

sw_error_t
hppe_l2_dbgcnt_wdata_get(
		a_uint32_t dev_id,
		union l2_dbgcnt_wdata_u *value);

sw_error_t
hppe_l2_dbgcnt_wdata_set(
		a_uint32_t dev_id,
		union l2_dbgcnt_wdata_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data0_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data0_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data1_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data1_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data2_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_rslt_data2_u *value);

sw_error_t
hppe_fdb_tbl_op_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data0_u *value);

sw_error_t
hppe_fdb_tbl_op_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data0_u *value);

sw_error_t
hppe_fdb_tbl_op_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data1_u *value);

sw_error_t
hppe_fdb_tbl_op_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data1_u *value);

sw_error_t
hppe_fdb_tbl_op_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_op_data2_u *value);

sw_error_t
hppe_fdb_tbl_op_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_op_data2_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data0_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data0_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data0_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data0_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data1_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data1_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data1_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data1_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data2_get(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data2_u *value);

sw_error_t
hppe_fdb_tbl_rd_op_data2_set(
		a_uint32_t dev_id,
		union fdb_tbl_rd_op_data2_u *value);

sw_error_t
hppe_port_bridge_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_bridge_ctrl_u *value);

sw_error_t
hppe_port_bridge_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_bridge_ctrl_u *value);

sw_error_t
hppe_port_lrn_limit_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_ctrl_u *value);

sw_error_t
hppe_port_lrn_limit_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_ctrl_u *value);

sw_error_t
hppe_port_lrn_limit_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_counter_u *value);

sw_error_t
hppe_port_lrn_limit_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_lrn_limit_counter_u *value);

sw_error_t
hppe_rfdb_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfdb_tbl_u *value);

sw_error_t
hppe_rfdb_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfdb_tbl_u *value);

sw_error_t
hppe_fdb_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union fdb_tbl_u *value);

sw_error_t
hppe_fdb_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union fdb_tbl_u *value);

sw_error_t
hppe_l2_dbg_addr_l2_dbg_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbg_addr_l2_dbg_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_dbg_data_l2_dbg_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbg_data_l2_dbg_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_op_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_op_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_op_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_op_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_op_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_op_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_op_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_op_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_hash_block_bitmap_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_hash_block_bitmap_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_byp_rslt_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_byp_rslt_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_op_rslt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_op_rslt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_valid_cnt_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_valid_cnt_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_entry_index_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_entry_index_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_cmd_id_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_cmd_id_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_age_timer_age_val_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_age_timer_age_val_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_fdb_hash_full_fwd_cmd_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_fdb_hash_full_fwd_cmd_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_failover_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_failover_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_lrn_ctrl_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_lrn_ctrl_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_age_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_age_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_lrn_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_lrn_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_fdb_hash_mode_0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_global_conf_age_ctrl_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_age_ctrl_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

#ifndef IN_FDB_MINI
sw_error_t
hppe_l2_global_conf_service_code_loop_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_global_conf_service_code_loop_set(
		a_uint32_t dev_id,
		unsigned int value);
#endif

sw_error_t
hppe_l2_dbgcnt_cmd_type_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbgcnt_cmd_type_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_dbgcnt_cmd_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbgcnt_cmd_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_dbgcnt_rdata_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbgcnt_rdata_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l2_dbgcnt_wdata_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_l2_dbgcnt_wdata_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_rslt_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_data0_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_data0_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_data1_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_data1_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_fdb_tbl_rd_op_data2_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_fdb_tbl_rd_op_data2_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_port_bridge_ctrl_txmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_txmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_port_isolation_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_port_isolation_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_station_move_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_station_move_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_new_addr_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_new_addr_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_promisc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_promisc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_new_addr_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_new_addr_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_bridge_ctrl_station_move_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_bridge_ctrl_station_move_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_exceed_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_lrn_limit_ctrl_lrn_lmt_exceed_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_lrn_limit_counter_lrn_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_lrn_limit_counter_lrn_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rfdb_tbl_mac_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_rfdb_tbl_mac_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_rfdb_tbl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rfdb_tbl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

