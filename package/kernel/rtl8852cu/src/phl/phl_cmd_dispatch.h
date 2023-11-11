/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __PHL_CMD_DISP_ENG_H_
#define __PHL_CMD_DISP_ENG_H_
#define SHARE_THREAD_MODE (0x1)
#define SOLO_THREAD_MODE (0x2)
/**
 * phl_cmd_dispatch_engine,
 * @phl_info: for general reference usage.
 * @dispatcher: array of cmd dispatcher,
 *	each dispatch is design to address coexist issue for each PHY instance.
 * @phy_num: current PHY instance number
 * @token_mgnt_thread: token mgnt thread
 * @token_chk: semaphore to wake up token mgnt thread
 * @thread_mode: indicate current thread operation mode.
 * 		 SHARE_THREAD_MODE means all dispatcher share an background thread
 * 		 SOLO_THREAD_MODE means each disaptcher has a dedicated background thread
 * @msg_q_sema: (opt) only used in SHARE_THREAD_MODE, used to wake up shared background thread
 * @share_thread: (opt) only used in SHARE_THREAD_MODE, context of shared background thread
 */
struct phl_cmd_dispatch_engine {
	struct phl_info_t *phl_info;
	void **dispatcher; //an array of struct cmd_dispatcher
	u8 phy_num;
	u8 thread_mode;

#ifdef CONFIG_CMD_DISP_SOLO_MODE
	_os_sema dispr_ctrl_sema; /* keep msg from different dispr sequentially forward to ctrl*/
#else
	_os_sema msg_q_sema;
	_os_thread share_thread;
#endif
};


/* functions called in phl layer*/
enum rtw_phl_status phl_disp_eng_init(struct phl_info_t *phl, u8 phy_num);
enum rtw_phl_status phl_disp_eng_deinit(struct phl_info_t *phl);
enum rtw_phl_status phl_disp_eng_start(struct phl_info_t *phl);
enum rtw_phl_status phl_disp_eng_stop(struct phl_info_t *phl);

/* bk module would be initialized in phl_disp_eng_register_module call
 * as for de-initialization, user can use phl_disp_eng_deregister_module as pair-wise operation
 * or phl_disp_eng_deinit would deinit all bk modules per dispatcher
*/
enum rtw_phl_status phl_disp_eng_register_module(struct phl_info_t *phl,
						 u8 band_idx,
						 enum phl_module_id id,
						 struct phl_bk_module_ops *ops);

enum rtw_phl_status phl_disp_eng_deregister_module(struct phl_info_t *phl,
						   u8 band_idx,
						   enum phl_module_id id);
enum rtw_phl_status phl_dispr_get_idx(void *dispr, u8 *idx);
u8 phl_disp_eng_is_dispr_busy(struct phl_info_t *phl, u8 band_idx);
u8 phl_disp_eng_is_fg_empty(struct phl_info_t *phl, u8 band_idx);
enum rtw_phl_status phl_disp_eng_set_cur_cmd_info(struct phl_info_t *phl, u8 band_idx,
					       struct phl_module_op_info *op_info);
enum rtw_phl_status phl_disp_eng_query_cur_cmd_info(struct phl_info_t *phl, u8 band_idx,
						 struct phl_module_op_info *op_info);

void rtw_phl_dump_mdl(struct phl_msg *msg, const char *caller);

enum rtw_phl_status phl_disp_eng_set_bk_module_info(struct phl_info_t *phl, u8 band_idx,
						enum phl_module_id id, struct phl_module_op_info *op_info);
enum rtw_phl_status phl_disp_eng_query_bk_module_info(struct phl_info_t *phl, u8 band_idx,
							enum phl_module_id id, struct phl_module_op_info *op_info);
enum rtw_phl_status phl_disp_eng_set_src_info(struct phl_info_t *phl, struct phl_msg *msg,
						struct phl_module_op_info *op_info);
enum rtw_phl_status phl_disp_eng_query_src_info(struct phl_info_t *phl, struct phl_msg *msg,
						struct phl_module_op_info *op_info);
enum rtw_phl_status phl_disp_eng_send_msg(struct phl_info_t *phl, struct phl_msg *msg,
						struct phl_msg_attribute *attr, u32 *msg_hdl);
enum rtw_phl_status phl_disp_eng_cancel_msg(struct phl_info_t *phl, u8 band_idx, u32 *msg_hdl);
enum rtw_phl_status phl_disp_eng_clr_pending_msg(struct phl_info_t *phl, u8 band_idx);

enum rtw_phl_status phl_disp_eng_add_token_req(struct phl_info_t *phl, u8 band_idx,
					    struct phl_cmd_token_req *req, u32 *req_hdl);
enum rtw_phl_status phl_disp_eng_cancel_token_req(struct phl_info_t *phl, u8 band_idx, u32 *req_hdl);
enum rtw_phl_status phl_disp_eng_free_token(struct phl_info_t *phl, u8 band_idx, u32 *req_hdl);
enum rtw_phl_status phl_disp_eng_clearance_acquire(struct phl_info_t *phl, u8 band_idx);
enum rtw_phl_status phl_disp_eng_clearance_release(struct phl_info_t *phl, u8 band_idx);
enum rtw_phl_status phl_disp_eng_exclusive_ready(struct phl_info_t *phl, u8 band_idx);
enum rtw_phl_status phl_disp_eng_notify_dev_io_status(struct phl_info_t *phl,
                                                      u8 band_idx,
                                                      enum phl_module_id mdl_id,
                                                      bool allow_io);
void phl_disp_eng_notify_shall_stop(struct phl_info_t *phl);

enum rtw_phl_status phl_disp_eng_set_msg_disp_seq(struct phl_info_t *phl,
							struct phl_msg_attribute *attr,
							struct msg_self_def_seq *seq);
u8 phl_disp_query_mdl_id(struct phl_info_t *phl, void *bk_mdl);
#ifdef CONFIG_CMD_DISP
/* following functions are only used inside phl_cmd_dispatch_eng.c */
enum rtw_phl_status dispr_init(struct phl_info_t *phl, void **dispr, u8 idx);
enum rtw_phl_status dispr_deinit(struct phl_info_t *phl, void *dispr);
enum rtw_phl_status dispr_start(void *dispr);
bool is_dispr_started(void *dispr);
enum rtw_phl_status dispr_stop(void *dispr);
enum rtw_phl_status dispr_register_module(void *dispr,
					  enum phl_module_id id,
					  struct phl_bk_module_ops *ops);
enum rtw_phl_status dispr_deregister_module(void *dispr,
					    enum phl_module_id id);
enum rtw_phl_status dispr_module_init(void *dispr);
enum rtw_phl_status dispr_module_deinit(void *dispr);
enum rtw_phl_status dispr_module_start(void *dispr);
enum rtw_phl_status dispr_module_stop(void *dispr);
enum rtw_phl_status dispr_get_cur_cmd_req(void *dispr, void **handle);
enum rtw_phl_status dispr_set_cur_cmd_info(void *dispr,
					       struct phl_module_op_info *op_info);
enum rtw_phl_status dispr_query_cur_cmd_info(void *dispr,
						 struct phl_module_op_info *op_info);

enum rtw_phl_status dispr_get_bk_module_handle(void *dispr,
						   enum phl_module_id id,
						   void **handle);
enum rtw_phl_status dispr_set_bk_module_info(void *dispr, void *handle,
						 struct phl_module_op_info *op_info);
enum rtw_phl_status dispr_query_bk_module_info(void *dispr, void *handle,
						   struct phl_module_op_info *op_info);
enum rtw_phl_status dispr_set_src_info(void *dispr, struct phl_msg *msg,
					   struct phl_module_op_info *op_info);
enum rtw_phl_status dispr_query_src_info(void *dispr, struct phl_msg *msg,
					     struct phl_module_op_info *op_info);
enum rtw_phl_status dispr_send_msg(void *dispr, struct phl_msg *msg,
				       struct phl_msg_attribute *attr, u32 *msg_hdl);
enum rtw_phl_status dispr_cancel_msg(void *dispr, u32 *msg_hdl);
enum rtw_phl_status dispr_clr_pending_msg(void *dispr);

enum rtw_phl_status dispr_add_token_req(void *dispr,
					    struct phl_cmd_token_req *req, u32 *req_hdl);
enum rtw_phl_status dispr_cancel_token_req(void *dispr, u32 *req_hdl);
enum rtw_phl_status dispr_free_token(void *dispr, u32 *req_hdl);

void dispr_clearance_acquire(void *dispr);
void dispr_clearance_release(void *dispr);
void dispr_exclusive_ready(void *dispr, bool renew_req);

enum rtw_phl_status dispr_notify_dev_io_status(void *dispr, enum phl_module_id mdl_id, bool allow_io);
void dispr_notify_shall_stop(void *dispr);
u8 dispr_is_fg_empty(void *dispr);

#if !defined(CONFIG_CMD_DISP_SOLO_MODE)
void dispr_share_thread_loop_hdl(void *dispr);
void dispr_share_thread_leave_hdl(void *dispr);
void dispr_share_thread_stop_prior_hdl(void *dispr);
void dispr_share_thread_stop_post_hdl(void *dispr);
#endif
enum rtw_phl_status dispr_set_dispatch_seq(void *dispr, struct phl_msg_attribute *attr,
							struct msg_self_def_seq* seq);

/* following functions are called inside phl_cmd_dispatcher.c */
#define IS_DISPR_CTRL(_mdl_id) ((_mdl_id) < PHL_BK_MDL_ROLE_START)

#define disp_eng_is_solo_thread_mode(_phl) \
	((_phl)->disp_eng.thread_mode == SOLO_THREAD_MODE)
void disp_eng_notify_share_thread(struct phl_info_t *phl, void *dispr);
void dispr_ctrl_hook_ops(void *dispr, struct phl_bk_module_ops *ops);
u8 disp_query_mdl_id(struct phl_info_t *phl, void *bk_mdl);
#ifdef CONFIG_CMD_DISP_SOLO_MODE
void dispr_ctrl_sema_down(struct phl_info_t *phl);
void dispr_ctrl_sema_up(struct phl_info_t *phl);
#endif
#endif
#endif	/* __PHL_PHY_H_ */
