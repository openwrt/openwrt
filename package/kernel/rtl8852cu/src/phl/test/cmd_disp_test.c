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
#define _CMD_DISP_TEST_C_
#include "../phl_headers.h"
#include "../phl_api.h"

#ifdef CONFIG_PHL_TEST_SUITE
#ifdef ENABLE_CMD_DISP_TEST

struct test_case_init {
	u8 init_cnt;
	u8 start_cnt;
};
struct test_case_msg {
	u32 msg_cnt[2];
	u8 pending[2];
	u8 forcefail;
};
struct test_case_cmd {
	u32 token_cnt[2];
	u32 cmd_msg[2];
	u8 abort[2];
};
struct test_case_prio_msg {
	u32 msg_idx;
};
struct test_case_cannot_io {
	bool cannot_io;
	u32 last_evt;
};

struct test_case_self_def_seq {
	u32 seq;
	u32 cnt;
};

struct cmd_disp_test_ctx {
	struct test_obj_ctrl_interface intf;
	struct phl_info_t* phl;
	u8 is_test_end;
	u8 test_case;
	u8 is_pass;
	u8 thread_mode;
	char rsn[32];
	struct phl_bk_module_ops ops;
	struct phl_cmd_token_req req;
	struct test_case_init case_init;
	struct test_case_msg case_msg;
	struct test_case_cmd case_cmd;
	struct test_case_prio_msg case_prio_msg;
	struct test_case_cannot_io case_cannot_io;
	struct test_case_self_def_seq case_self_def_seq;
};
struct cmd_disp_test_ctx disp_test_ctx;

enum CMD_DISP_TEST_CASE {
	DISP_TEST_INIT = 1,
	DISP_TEST_SEND_MSG = 2,
	DISP_TEST_CMD_TOKEN_REQ = 3,
	DISP_TEST_CANNOT_IO = 4,
	DISP_TEST_PRIORITIZE_MSG = 5,
	DISP_TEST_SELF_DEF_SEQ = 6
};

char err_rsn[][32] = {
	"init eng fail", /*0*/
	"register mdl fail", /*1*/
	"init mdl fail", /*2*/
	"init mdl count err", /*3*/
	"start eng fail",	/*4*/
	"start mdl count err", /*5*/
	"stop eng fail", /*6*/
	"stop mdl count err", /*7*/
	"deinit mdl count err", /*8*/
	"msg mdl id chk err", /*9*/
	"send msg err", /*10*/
	"msg cnt chk err", /*11*/
	"msg fail hdl err", /*12*/
	"add token err", /*13*/
	"acquire token err", /*14*/
	"free token chk err", /*15*/
	"free token err", /*16*/
	"cmd order err", /*17*/
	"cancel cmd fail", /*18*/
	"pending msg fail", /*19*/
	"hook next req fail", /*20*/
	"msg order err", /*21*/
	"msg status indicator err", /*22*/
	"msg completion not received", /*23*/
	"set dispr attr fail", /*24*/
	"self def seq not match", /*25*/
	"none"};
enum phl_mdl_ret_code test_req_acquired(void* dispr, void* priv)
{
	u8 idx = 0;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	struct phl_msg msg;
	struct phl_msg_attribute attr;
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	if(disp_test_ctx.test_case == DISP_TEST_CMD_TOKEN_REQ) {
		if(phl_dispr_get_idx(dispr, &idx) != RTW_PHL_STATUS_SUCCESS)
			return MDL_RET_FAIL;
		disp_test_ctx.case_cmd.token_cnt[idx]++;
		//wait for 2nd cmd req to send msg
		if(disp_test_ctx.case_cmd.token_cnt[idx] == 2) {
			_os_mem_set(d, &msg, 0, sizeof(struct phl_msg));
			_os_mem_set(d, &attr, 0, sizeof(struct phl_msg_attribute));
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_START + 1);
			SET_MSG_EVT_ID_FIELD(msg.msg_id, 125);
			msg.band_idx = idx;
			status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
			disp_test_ctx.case_cmd.cmd_msg[idx]++;
		}
	}
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_req_abort(void* dispr, void* priv)
{
	u8 idx = 0;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	if(disp_test_ctx.test_case == DISP_TEST_CMD_TOKEN_REQ) {
		if(phl_dispr_get_idx(dispr, &idx) != RTW_PHL_STATUS_SUCCESS)
			return MDL_RET_FAIL;
		disp_test_ctx.case_cmd.abort[idx] = true;
		attr.opt = MSG_OPT_SEND_IN_ABORT;
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, 0x123);
		msg.band_idx = idx;
		phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	}
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_req_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	u8 idx = 0;
	if(disp_test_ctx.test_case == DISP_TEST_CMD_TOKEN_REQ) {
		if(phl_dispr_get_idx(dispr, &idx) != RTW_PHL_STATUS_SUCCESS)
			return MDL_RET_FAIL;
		if(  MSG_EVT_ID_FIELD(msg->msg_id) == 125)
			disp_test_ctx.case_cmd.cmd_msg[idx]++;
	}
	return MDL_RET_SUCCESS;

}
enum phl_mdl_ret_code test_req_set_info(void* dispr, void* priv,
			struct phl_module_op_info* info)
{
	return MDL_RET_SUCCESS;

}
enum phl_mdl_ret_code test_req_query_info(void* dispr, void* priv,
				struct phl_module_op_info* info)
{
	return MDL_RET_SUCCESS;

}
enum phl_mdl_ret_code test_mdl_init(void* phl_info, void* dispr, void** priv)
{
	if(disp_test_ctx.test_case == DISP_TEST_INIT)
		disp_test_ctx.case_init.init_cnt++;
	return MDL_RET_SUCCESS;
}
void test_mdl_deinit(void* dispr, void* priv)
{
	if(disp_test_ctx.test_case == DISP_TEST_INIT)
		disp_test_ctx.case_init.init_cnt--;
	return;
}
enum phl_mdl_ret_code test_mdl_start(void* dispr, void* priv)
{
	if(disp_test_ctx.test_case == DISP_TEST_INIT)
		disp_test_ctx.case_init.start_cnt++;
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_mdl_stop(void* dispr, void* priv)
{
	if(disp_test_ctx.test_case == DISP_TEST_INIT)
		disp_test_ctx.case_init.start_cnt--;
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_mdl_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	u8 idx = 0;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	if(disp_test_ctx.test_case == DISP_TEST_SEND_MSG &&
	   phl_dispr_get_idx(dispr, &idx) == RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.case_msg.msg_cnt[idx]++;
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 4 && disp_test_ctx.case_msg.pending[idx] == false) {
			disp_test_ctx.case_msg.pending[idx] = true;
			return MDL_RET_PENDING; /* reschedule msg*/
		}
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 15) {
			if(!IS_MSG_FAIL(msg->msg_id)) {
				disp_test_ctx.case_msg.forcefail = true;
				return MDL_RET_FAIL; /* force msg fail*/
			}
			else {
				disp_test_ctx.case_msg.forcefail = false;
			}
		}
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 10) {
			_os_sleep_ms(d, 100);
		}
	}
	if(disp_test_ctx.test_case == DISP_TEST_CMD_TOKEN_REQ) {
		if(phl_dispr_get_idx(dispr, &idx) != RTW_PHL_STATUS_SUCCESS)
			return MDL_RET_FAIL;
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 125 &&
		   disp_test_ctx.case_cmd.cmd_msg[idx] != 1)
		   disp_test_ctx.case_cmd.cmd_msg[idx]++;
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 0x123) {
			SET_MSG_EVT_ID_FIELD(msg->msg_id, 0x124);
			return MDL_RET_PENDING;
		}
	}
	if (disp_test_ctx.test_case == DISP_TEST_PRIORITIZE_MSG) {
		if(MSG_EVT_ID_FIELD(msg->msg_id) == 1 && IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			disp_test_ctx.case_prio_msg.msg_idx = 1;
			_os_sleep_ms(d, 100);
		}
		if (disp_test_ctx.case_prio_msg.msg_idx + 1 == MSG_EVT_ID_FIELD(msg->msg_id))
			disp_test_ctx.case_prio_msg.msg_idx = MSG_EVT_ID_FIELD(msg->msg_id);
	}
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_mdl_set_info(void* dispr, void* priv,
				struct phl_module_op_info* info)

{
	return MDL_RET_SUCCESS;
}
enum phl_mdl_ret_code test_mdl_query_info(void* dispr, void* priv,
				struct phl_module_op_info* info)
{
	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code test_btc_mdl_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	if(disp_test_ctx.test_case == DISP_TEST_SELF_DEF_SEQ) {
		disp_test_ctx.case_self_def_seq.cnt++;
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (disp_test_ctx.case_self_def_seq.seq == 1)
				disp_test_ctx.case_self_def_seq.seq = 2;
		}
		else {
			if (disp_test_ctx.case_self_def_seq.seq == 6)
				disp_test_ctx.case_self_def_seq.seq = 7;
		}
	}
	return MDL_RET_SUCCESS;
}
/************************************************************************/

enum phl_mdl_ret_code test_gen_mdl_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	if(disp_test_ctx.test_case == DISP_TEST_SELF_DEF_SEQ) {
		disp_test_ctx.case_self_def_seq.cnt++;
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (disp_test_ctx.case_self_def_seq.seq == 2)
				disp_test_ctx.case_self_def_seq.seq = 3;
		}
		else {
			if (disp_test_ctx.case_self_def_seq.seq == 4)
				disp_test_ctx.case_self_def_seq.seq = 5;
		}
	}
	return MDL_RET_SUCCESS;
}

/************************************************************************/

enum phl_mdl_ret_code test_mcc_mdl_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	if(disp_test_ctx.test_case == DISP_TEST_SELF_DEF_SEQ) {
		disp_test_ctx.case_self_def_seq.cnt++;
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (disp_test_ctx.case_self_def_seq.seq == 3)
				disp_test_ctx.case_self_def_seq.seq = 4;
		}
		else {
			if (disp_test_ctx.case_self_def_seq.seq == 5)
				disp_test_ctx.case_self_def_seq.seq = 6;
		}
	}
	return MDL_RET_SUCCESS;
}

/************************************************************************/

enum phl_mdl_ret_code test_ser_mdl_msg_hdlr(void* dispr, void* priv, struct phl_msg* msg)
{
	void *d = phl_to_drvpriv(disp_test_ctx.phl);

	if(disp_test_ctx.test_case == DISP_TEST_SELF_DEF_SEQ) {
		disp_test_ctx.case_self_def_seq.cnt++;
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (disp_test_ctx.case_self_def_seq.seq == 0)
				disp_test_ctx.case_self_def_seq.seq = 1;
			if (disp_test_ctx.case_self_def_seq.seq == 8)
				_os_delay_ms(d, 100);
		}
		else {
			if (disp_test_ctx.case_self_def_seq.seq == 7)
				disp_test_ctx.case_self_def_seq.seq = 8;
		}
	}
	return MDL_RET_SUCCESS;
}

void disp_init_test( void )
{
	u8 i = 0;
	u8 mdl_cnt = 10;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;
	disp_test_ctx.is_pass = true;
	u32 offset[] = {PHL_BK_MDL_ROLE_START, PHL_BK_MDL_OPT_START, PHL_BK_MDL_MDRY_START};
	u32 idx = 0;

	for(i = 0; i < 5; i++) {
		status = phl_disp_eng_init(disp_test_ctx.phl, 3);
		if(	(!i && status !=  RTW_PHL_STATUS_SUCCESS) ||
			(i && status ==  RTW_PHL_STATUS_SUCCESS)) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
			goto test_err;
		}
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	for( i = 0 ; i < mdl_cnt; i++) {
		idx = i % PHL_MDL_PRI_MAX;
		status = dispr_register_module(disp_eng->dispatcher[2],
						i+offset[idx],
						&(disp_test_ctx.ops));
		if(status !=  RTW_PHL_STATUS_SUCCESS) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
			goto test_err;
		}
	}

	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_err;
	}
	if(disp_test_ctx.case_init.start_cnt != mdl_cnt) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[5], sizeof(err_rsn[5]));
		goto test_err;
	}

	status = phl_disp_eng_stop(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[6], sizeof(err_rsn[6]));
		goto test_err;
	}
	if(disp_test_ctx.case_init.start_cnt) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[7], sizeof(err_rsn[7]));
		goto test_err;
	}

	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	if( status != RTW_PHL_STATUS_SUCCESS)
		disp_test_ctx.is_pass = false;
	if(disp_test_ctx.case_init.init_cnt) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[8], sizeof(err_rsn[8]));
	}
	goto test_end;
test_err:
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
test_end:
	disp_test_ctx.is_test_end = true;
}

void disp_send_msg_test( void )
{
	u32 i = 0;
	struct phl_msg msg;
	struct phl_msg_attribute attr;
	u8 mdl_cnt = 10;
	u8 phy_num = 2;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;
	u8 man_num = 0;
	u8 opt_num = 0;
	u8 role_num = 0;
	u8 total_num = 0;
	u32 msg_hdl = 0;
	u32 offset[] = {PHL_BK_MDL_ROLE_START, PHL_BK_MDL_OPT_START, PHL_BK_MDL_MDRY_START};
	u32 idx = 0;
	u32 tmp = 0;

	disp_test_ctx.is_pass = true;
	status = phl_disp_eng_init(disp_test_ctx.phl, phy_num);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
		goto test_end;
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_end;
	}

	_os_mem_set(d, &msg, 0, sizeof(struct phl_msg));
	_os_mem_set(d, &attr, 0, sizeof(struct phl_msg_attribute));
	/* check recycle flow */
	for( i = 0 ; i < 100; i++) {
		if( !(i % 10 ))
			SET_MSG_MDL_ID_FIELD(msg.msg_id, 10);
		else
			SET_MSG_MDL_ID_FIELD(msg.msg_id, 0);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, i);
		msg.band_idx = 0;
		status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
		if((!(i %10) && status ==  RTW_PHL_STATUS_SUCCESS) ||
		   ((i %10) && status ==  RTW_PHL_STATUS_FAILURE)) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[9], sizeof(err_rsn[9]));
			goto test_end;
		}
		msg.band_idx = 1;
		status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
		if((!(i %10) && status ==  RTW_PHL_STATUS_SUCCESS) ||
		   ((i %10) && status ==  RTW_PHL_STATUS_FAILURE)) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[9], sizeof(err_rsn[9]));
			goto test_end;
		}

	}
	_os_sleep_ms(d, 100);
	/* check dispatch flow */
	for( i = 0 ; i < mdl_cnt; i++) {
		/* num of wifi role module: 4 	(10, 13, 16, 19)
		 * num of optional module: 3 	(72, 75, 78)
		 * num of mandatory module: 3 	(43, 46, 49)
		 * for modules at mandatory & wifi role priority,
		 * they would receive every msg
		 * for modules at optional priority,
		 * they only receive msg when bitmap is set in phl_disp_eng_send_msg
		 */
		man_num = 3;
		opt_num = 3;
		role_num = 4;
		idx = i % PHL_MDL_PRI_MAX;
		status = dispr_register_module(disp_eng->dispatcher[0],
						i+offset[idx],
						&(disp_test_ctx.ops));
		if(status !=  RTW_PHL_STATUS_SUCCESS) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
			goto test_end;
		}
		status = dispr_register_module(disp_eng->dispatcher[1],
						i+offset[idx],
						&(disp_test_ctx.ops));
		if(status !=  RTW_PHL_STATUS_SUCCESS) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
			goto test_end;
		}
	}
	for( i = 0 ; i < mdl_cnt; i++) {
		idx = i % PHL_MDL_PRI_MAX;
		SET_MSG_MDL_ID_FIELD(msg.msg_id, i+offset[idx]);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, i);
		if(i == 3)
			tmp = 72;
		else if( i == 6)
			tmp = 78;
		else
			tmp = 0;
		attr.notify.id_arr = (u8*)&tmp;
		attr.notify.len = 1;
		attr.opt |= MSG_OPT_SKIP_NOTIFY_OPT_MDL;
		msg.band_idx = 0;
		status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
		if(status !=  RTW_PHL_STATUS_SUCCESS) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
			goto test_end;
		}
		msg.band_idx = 1;
		phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	}
	attr.opt = 0;
	attr.notify.id_arr = NULL;
	attr.notify.len = 0;
	_os_sleep_ms(d, 100);
	total_num = 2* (((man_num+role_num) * mdl_cnt) + 3 + 2) + 1;
	if(disp_test_ctx.case_msg.msg_cnt[0] == total_num ||
	   disp_test_ctx.case_msg.msg_cnt[1] == total_num) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[11], sizeof(err_rsn[19]));
		goto test_end;
	}
	phl_disp_eng_clr_pending_msg(disp_test_ctx.phl, 0);
	phl_disp_eng_clr_pending_msg(disp_test_ctx.phl, 1);
	/*
	 * we send 10 msgs above & expect the msg cnt to be as below
	 * total = 2 * ((man_num + role_num) * msg_cnt + 3 + 2) + 1
	 * regarding multiply * 2, since we have pre_phase & post_phase handle
	 * 3: opt module number
	 * 2: when module id = {13, 16}, we indicate one optional module to notify
	 * 1: since module id 43 should be entered twice because of reschedule
	 */
	_os_sleep_ms(d, 100);
	disp_test_ctx.case_msg.pending[0] = false;
	disp_test_ctx.case_msg.pending[1] = false;
	total_num = 2* (((man_num+role_num) * mdl_cnt) + 3 + 2) + 1;
	if(disp_test_ctx.case_msg.msg_cnt[0] != total_num ||
	   disp_test_ctx.case_msg.msg_cnt[1] != total_num) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[11], sizeof(err_rsn[11]));
		goto test_end;
	}
	SET_MSG_MDL_ID_FIELD(msg.msg_id, 78);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 15);
	msg.band_idx = 0;
	phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	_os_sleep_ms(d, 10);
	if(disp_test_ctx.case_msg.forcefail == true) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[11], sizeof(err_rsn[11]));
		goto test_end;
	}
	SET_MSG_MDL_ID_FIELD(msg.msg_id, 78);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 10);
	phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, &msg_hdl);
	_os_sleep_ms(d, 100);
	phl_disp_eng_cancel_msg(disp_test_ctx.phl, 0, &msg_hdl);
test_end:
	status = phl_disp_eng_stop(disp_test_ctx.phl);
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	disp_test_ctx.is_test_end = true;
}

void disp_cmd_token_test( void )
{
	u32 i = 0;
	u32 j = 0;
	u8 phy_num = 2;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;
	u32 req_hdl[2][10] = {0};
	disp_test_ctx.is_pass = true;
	status = phl_disp_eng_init(disp_test_ctx.phl, phy_num);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
		goto test_end;
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_end;
	}
	for( i = 0 ; i < phy_num ; i++) {
		status = dispr_register_module(disp_eng->dispatcher[i],
						PHL_BK_MDL_MDRY_START,
						&(disp_test_ctx.ops));
		if(status !=  RTW_PHL_STATUS_SUCCESS) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
			goto test_end;
		}
		for( j = 0 ; j < 10 ; j++) {
			disp_test_ctx.req.module_id = (u8)(PHL_FG_MDL_START + j);
			status = phl_disp_eng_add_token_req(disp_test_ctx.phl, (u8)i, &(disp_test_ctx.req), &(req_hdl[i][j]));
			_os_sleep_ms(d, 1);
			if( (j == 0 && status !=  RTW_PHL_STATUS_SUCCESS ) ||
			    ( j && j < 8 && status !=  RTW_PHL_STATUS_PENDING) ||
			    (j >= 8 && status !=  RTW_PHL_STATUS_RESOURCE)) {
				PHL_INFO("[DISP_TEST] idx: %d status:%d\n", j, status);
				disp_test_ctx.is_pass = false;
				_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[13], sizeof(err_rsn[13]));
				goto test_end;
			}
		}
	}
	_os_sleep_ms(d, 100);
	if(disp_test_ctx.case_cmd.token_cnt[0] != 1||
	   disp_test_ctx.case_cmd.token_cnt[1] != 1) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[14], sizeof(err_rsn[14]));
		goto test_end;
	 }
	 for(i = 0 ; i < phy_num; i++) {
	 	status = phl_disp_eng_free_token(disp_test_ctx.phl, (u8)i, &(req_hdl[i][1]));
	 	if(status ==  RTW_PHL_STATUS_SUCCESS ) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[15], sizeof(err_rsn[15]));
			goto test_end;
		}
		status = phl_disp_eng_free_token(disp_test_ctx.phl, (u8)i, &(req_hdl[i][0]));
		if(status !=  RTW_PHL_STATUS_SUCCESS ) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[16], sizeof(err_rsn[16]));
			goto test_end;
		}
	 }
	 _os_sleep_ms(d, 100);
	 if(disp_test_ctx.case_cmd.cmd_msg[0] != 2 ||
	    disp_test_ctx.case_cmd.cmd_msg[1] != 2) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[17], sizeof(err_rsn[17]));
		goto test_end;
	}
	for(i = 0 ; i < phy_num; i++) {
		disp_test_ctx.case_cmd.abort[i] = false;
		phl_disp_eng_cancel_token_req(disp_test_ctx.phl, (u8)i, &(req_hdl[i][1]));
		_os_sleep_ms(d, 100);
		if(disp_test_ctx.case_cmd.abort[i] == false ||
		   disp_test_ctx.case_cmd.token_cnt[i] == 3) {
		   	disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[18], sizeof(err_rsn[18]));
			goto test_end;
		  }
		phl_disp_eng_clr_pending_msg(disp_test_ctx.phl, (u8)i);
		_os_sleep_ms(d, 100);
		if( disp_test_ctx.case_cmd.token_cnt[i] != 3) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[20], sizeof(err_rsn[20]));
			goto test_end;
		}
	}
test_end:
	status = phl_disp_eng_stop(disp_test_ctx.phl);
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	disp_test_ctx.is_test_end = true;

}


void disp_prioritize_msg_test( void )
{
	u32 i = 0;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u8 ctrl_msg_cnt = 4;
	u8 msg_cnt = 10;
	u8 ctrl_msg_start = 2;
	u8 msg_start = ctrl_msg_start + ctrl_msg_cnt;
	u8 phy_num = 1;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;

	disp_test_ctx.is_pass = true;
	status = phl_disp_eng_init(disp_test_ctx.phl, phy_num);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
		goto test_end;
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_end;
	}

	/* check dispatch flow */
	status = dispr_register_module(disp_eng->dispatcher[0],
					PHL_MDL_GENERAL,
					&(disp_test_ctx.ops));
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
		goto test_end;
	}
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_GENERAL);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 1);
	msg.band_idx = 0;
	status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
		goto test_end;
	}
	_os_sleep_ms(d, 10);
	for( i = 0 ; i < (u32) (msg_cnt + ctrl_msg_cnt); i++) {
		if (i % 3) {
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_GENERAL);
			SET_MSG_EVT_ID_FIELD(msg.msg_id, msg_start);
			msg_start++;
		}
		else {
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
			SET_MSG_EVT_ID_FIELD(msg.msg_id, ctrl_msg_start);
			ctrl_msg_start++;
		}
		status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	}
	_os_sleep_ms(d, 200);
	PHL_INFO("[DISP_TEST] msg_idx:0x%x\n",disp_test_ctx.case_prio_msg.msg_idx);
	if(disp_test_ctx.case_prio_msg.msg_idx != (u32) (msg_start - 1)) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[21], sizeof(err_rsn[21]));
		goto test_end;
	}

test_end:
	status = phl_disp_eng_stop(disp_test_ctx.phl);
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	disp_test_ctx.is_test_end = true;
}

void _msg_completion(void* priv, struct phl_msg* msg)
{
	void *d = phl_to_drvpriv(disp_test_ctx.phl);

	PHL_INFO("[DISP_TEST] msg_id:0x%x\n", msg->msg_id);
	if (MSG_EVT_ID_FIELD(msg->msg_id) == 10) {
		if ( (!IS_MSG_CANCEL(msg->msg_id) || !IS_MSG_CANNOT_IO(msg->msg_id))) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[22], sizeof(err_rsn[22]));
		}
	}
	else {
		disp_test_ctx.case_cannot_io.last_evt = MSG_EVT_ID_FIELD(msg->msg_id);
		if (disp_test_ctx.case_cannot_io.cannot_io == true) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[19], sizeof(err_rsn[19]));
		}
		else if (IS_MSG_CANCEL(msg->msg_id) || IS_MSG_FAIL(msg->msg_id) || IS_MSG_CANNOT_IO(msg->msg_id)) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[22], sizeof(err_rsn[22]));
		}
	}
}

void disp_cannot_io_test( void )
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u8 phy_num = 1;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;

	disp_test_ctx.is_pass = true;
	status = phl_disp_eng_init(disp_test_ctx.phl, phy_num);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
		goto test_end;
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_end;
	}

	disp_test_ctx.case_cannot_io.cannot_io = true;
	phl_disp_eng_notify_dev_io_status(disp_test_ctx.phl, HW_BAND_MAX, 0, false);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 10);
	msg.band_idx = 0;
	attr.completion.completion = _msg_completion;
	attr.completion.priv = &disp_test_ctx;

	status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
		goto test_end;
	}
	_os_sleep_ms(d, 100);
	if (disp_test_ctx.is_pass == false)
		goto test_end;

	SET_MSG_EVT_ID_FIELD(msg.msg_id, 20);
	attr.opt |= MSG_OPT_PENDING_DURING_CANNOT_IO;
	status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
		goto test_end;
	}
	_os_sleep_ms(d, 100);
	if (disp_test_ctx.is_pass == false)
		goto test_end;

	disp_test_ctx.case_cannot_io.cannot_io = false;
	phl_disp_eng_notify_dev_io_status(disp_test_ctx.phl, HW_BAND_MAX, 0, true);

	_os_sleep_ms(d, 100);
	if (disp_test_ctx.case_cannot_io.last_evt != 20) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[23], sizeof(err_rsn[23]));
	}

test_end:
	status = phl_disp_eng_stop(disp_test_ctx.phl);
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	disp_test_ctx.is_test_end = true;
}
void disp_self_def_msg_test( void )
{
	u32 i = 0;
	struct phl_msg msg;
	struct phl_msg_attribute attr;
	u8 phy_num = 2;
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_dispatch_engine *disp_eng = NULL;
	struct msg_self_def_seq seq = {0};
	u8 pre_proct_seq[] = {PHL_MDL_BTC, PHL_MDL_GENERAL, PHL_MDL_MRC, PHL_MDL_SER, PHL_MDL_POWER_MGNT};
	u8 post_proct_seq[] = {PHL_MDL_GENERAL, PHL_MDL_MRC, PHL_MDL_SER, PHL_MDL_POWER_MGNT};

	disp_test_ctx.is_pass = true;
	status = phl_disp_eng_init(disp_test_ctx.phl, phy_num);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[0], sizeof(err_rsn[0]));
		goto test_end;
	}
	disp_eng = &(disp_test_ctx.phl->disp_eng);
	status = phl_disp_eng_start(disp_test_ctx.phl);
	if(status !=  RTW_PHL_STATUS_SUCCESS ) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[4], sizeof(err_rsn[4]));
		goto test_end;
	}
	disp_test_ctx.ops.msg_hdlr = test_btc_mdl_msg_hdlr;
	if(dispr_register_module(disp_eng->dispatcher[0], PHL_MDL_BTC, &(disp_test_ctx.ops)) !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
		goto test_end;
	}
	disp_test_ctx.ops.msg_hdlr = test_gen_mdl_msg_hdlr;
	if(dispr_register_module(disp_eng->dispatcher[0], PHL_MDL_GENERAL, &(disp_test_ctx.ops)) !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
		goto test_end;
	}
	disp_test_ctx.ops.msg_hdlr = test_mcc_mdl_msg_hdlr;
	if(dispr_register_module(disp_eng->dispatcher[0], PHL_MDL_MRC, &(disp_test_ctx.ops)) !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
		goto test_end;
	}
	disp_test_ctx.ops.msg_hdlr = test_ser_mdl_msg_hdlr;
	if(dispr_register_module(disp_eng->dispatcher[0], PHL_MDL_SER, &(disp_test_ctx.ops)) !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[1], sizeof(err_rsn[1]));
		goto test_end;
	}
	disp_test_ctx.ops.msg_hdlr = test_mdl_msg_hdlr;

	_os_mem_set(d, &msg, 0, sizeof(struct phl_msg));
	_os_mem_set(d, &attr, 0, sizeof(struct phl_msg_attribute));
	seq.pre_prot_phase.map[PHL_MDL_PRI_OPTIONAL].len = sizeof(pre_proct_seq);
	seq.pre_prot_phase.map[PHL_MDL_PRI_OPTIONAL].id_arr = pre_proct_seq;
	seq.pre_prot_phase.map[PHL_MDL_PRI_MANDATORY].len = sizeof(pre_proct_seq);
	seq.pre_prot_phase.map[PHL_MDL_PRI_MANDATORY].id_arr = pre_proct_seq;
	seq.post_prot_phase.map[PHL_MDL_PRI_OPTIONAL].len = sizeof(post_proct_seq);
	seq.post_prot_phase.map[PHL_MDL_PRI_OPTIONAL].id_arr = post_proct_seq;
	seq.post_prot_phase.map[PHL_MDL_PRI_MANDATORY].len = sizeof(post_proct_seq);
	seq.post_prot_phase.map[PHL_MDL_PRI_MANDATORY].id_arr = post_proct_seq;
	for (i = 0 ; i < 3; i++) {
		status = phl_disp_eng_set_msg_disp_seq(disp_test_ctx.phl, &attr, &seq);
		if(status !=  RTW_PHL_STATUS_SUCCESS || attr.dispr_attr == NULL) {
			disp_test_ctx.is_pass = false;
			_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[24], sizeof(err_rsn[24]));
			goto test_end;
		}
	}
	msg.band_idx = 0;
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_SER);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 10);
	status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	if(status !=  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
		goto test_end;
	}
	if (attr.dispr_attr != NULL) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[24], sizeof(err_rsn[24]));
		goto test_end;
	}
	_os_sleep_ms(d, 100);
	PHL_INFO("[DISP_TEST] seq:%d, cnt: %d\n",disp_test_ctx.case_self_def_seq.seq, disp_test_ctx.case_self_def_seq.cnt);
	if(disp_test_ctx.case_self_def_seq.seq != 8 || disp_test_ctx.case_self_def_seq.cnt != 8) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[25], sizeof(err_rsn[25]));
		goto test_end;
	}
	status = phl_disp_eng_set_msg_disp_seq(disp_test_ctx.phl, &attr, &seq);
	if(status !=  RTW_PHL_STATUS_SUCCESS || attr.dispr_attr == NULL) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[24], sizeof(err_rsn[24]));
		goto test_end;
	}
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, 10);
	status = phl_disp_eng_send_msg(disp_test_ctx.phl, &msg, &attr, NULL);
	if(status ==  RTW_PHL_STATUS_SUCCESS) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[10], sizeof(err_rsn[10]));
		goto test_end;
	}
	if (attr.dispr_attr != NULL) {
		disp_test_ctx.is_pass = false;
		_os_mem_cpy(d, disp_test_ctx.rsn, err_rsn[24], sizeof(err_rsn[24]));
		goto test_end;
	}
test_end:
	status = phl_disp_eng_stop(disp_test_ctx.phl);
	status = phl_disp_eng_deinit(disp_test_ctx.phl);
	disp_test_ctx.is_test_end = true;
}


u8 disp_test_bp_handler(void *priv, struct test_bp_info* bp_info)
{
	return true;
}

u8 disp_test_fail_rsn(void *priv,char* rsn, u32 max_len)
{
	void *d = phl_to_drvpriv(disp_test_ctx.phl);
	_os_mem_cpy(d, rsn, disp_test_ctx.rsn, (_os_strlen((u8*)disp_test_ctx.rsn) > max_len)?(max_len):(_os_strlen((u8*)disp_test_ctx.rsn)));
	return true;
}

u8 disp_test_is_test_end(void *priv)
{
	return disp_test_ctx.is_test_end;
}

u8 disp_test_is_test_pass(void *priv)
{
	PHL_INFO("[DISP_TEST] case(%s) pass(%d)\n", disp_test_ctx.test_case,
						disp_test_ctx.is_pass);
	return disp_test_ctx.is_pass;
}

u8 disp_test_start_test(void *priv)
{
	PHL_INFO("[DISP_TEST] case(%s) start\n", disp_test_ctx.test_case);
	switch(disp_test_ctx.test_case) {
		case DISP_TEST_INIT:
			/* verify items:
			 * 1: dispatch engine & dispatcher:
			 *    init, start, stop, deinit
			 * 2: module ops:
			 *    init, start, stop, deinit
			 * 3: double init
			 */
			disp_init_test();
			break;
		case DISP_TEST_SEND_MSG:
			/* verify items:
			 * 1: msg send, cancel, recycle efficiency
			 * 2: msg process flow in background, including
			 *    pre-phase, post phase, reschedule, fail handle
			 */
			disp_send_msg_test();
			break;
		case DISP_TEST_CMD_TOKEN_REQ:
			/* verify items:
			 * 1: cmd send, cancel, recycle
			 * 2: cmd token process flow in token mgnt thread
			 * 3: cmd req ops verification
			 */
			disp_cmd_token_test();
			break;
		case DISP_TEST_CANNOT_IO:
			disp_cannot_io_test();
			break;
		case DISP_TEST_PRIORITIZE_MSG:
			disp_prioritize_msg_test();
			break;
		case DISP_TEST_SELF_DEF_SEQ:
			disp_self_def_msg_test();
			break;
		default:
			disp_test_ctx.is_test_end = true;
			disp_test_ctx.is_pass = false;
			break;
	}
	PHL_INFO("[DISP_TEST] case(%s) pass(%d)\n", disp_test_ctx.test_case,
						disp_test_ctx.is_pass);
	return true;
}

void phl_cmd_disp_test_start(void* phl_info, u8 test_case)
{
	struct phl_info_t *phl = (struct phl_info_t *)phl_info;
	void *d = phl_to_drvpriv(phl);
	struct test_obj_ctrl_interface *intf = NULL;
	_os_mem_set(d, &disp_test_ctx, 0, sizeof(struct cmd_disp_test_ctx));
	disp_test_ctx.phl = phl;
	disp_test_ctx.is_test_end = false;
	disp_test_ctx.is_pass = false;
	disp_test_ctx.test_case = test_case;
	_os_mem_cpy(d, &(disp_test_ctx.rsn), "none", sizeof("none"));
	intf = &(disp_test_ctx.intf);

	/* test obj ops*/
	intf->bp_handler = disp_test_bp_handler;
	intf->get_fail_rsn = disp_test_fail_rsn;
	intf->is_test_end = disp_test_is_test_end;
	intf->is_test_pass = disp_test_is_test_pass;
	intf->start_test = disp_test_start_test;

	/* phl bk module ops*/
	disp_test_ctx.ops.init = test_mdl_init;
	disp_test_ctx.ops.deinit = test_mdl_deinit;
	disp_test_ctx.ops.start = test_mdl_start;
	disp_test_ctx.ops.stop = test_mdl_stop;
	disp_test_ctx.ops.msg_hdlr = test_mdl_msg_hdlr;
	disp_test_ctx.ops.set_info = test_mdl_set_info;
	disp_test_ctx.ops.query_info = test_mdl_query_info;
	/* cmd token req*/
	disp_test_ctx.req.acquired = test_req_acquired;
	disp_test_ctx.req.abort = test_req_abort;
	disp_test_ctx.req.msg_hdlr = test_req_msg_hdlr;
	disp_test_ctx.req.set_info = test_req_set_info;
	disp_test_ctx.req.query_info = test_req_query_info;
	rtw_phl_test_add_new_test_obj(phl->phl_com,
	                              "phl_cmd_disp_test_init",
	                              &disp_test_ctx,
	                              TEST_LVL_LOW,
	                              intf,
	                              10000,
	                              TEST_SUB_MODULE_TRX,
	                              UNIT_TEST_MODE);
	PHL_INFO("%s\n",__FUNCTION__);
}
#endif
#endif /* #ifdef CONFIG_PHL_TEST_SUITE */
