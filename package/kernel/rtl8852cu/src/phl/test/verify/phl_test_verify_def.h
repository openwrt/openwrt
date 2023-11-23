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
#ifndef _PHL_TEST_VERIFY_DEF_H_
#define _PHL_TEST_VERIFY_DEF_H_

#ifdef CONFIG_PHL_TEST_VERIFY
enum verify_cmd_status {
	VERIFY_STATUS_NOT_INIT = 0,
	VERIFY_STATUS_INIT = 1,
	VERIFY_STATUS_WAIT_CMD = 2,
	VERIFY_STATUS_CMD_EVENT = 3,
	VERIFY_STATUS_RUN_CMD = 4,
};

/*
 *	Command structure definition.
 *	Fixed part would be feature/cmd/cmd_ok for command and report parsing.
 *	Data members might have input or output usage.
 */

struct verify_cmd_hdr {
	u8 feature;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
};

struct s_handler {
	enum rtw_phl_status(*callback)(void *priv);
};

struct verify_context {
	u8 status;
	u8 cur_phy; // need ??
	_os_sema cmd_sema;
	void *buf;
	u32 buf_len;
	void *rpt;
	u32 rpt_len;
	struct test_obj_ctrl_interface test_ctrl;
	u8 is_test_end;
	struct phl_info_t *phl;
	struct rtw_phl_com_t *phl_com;
	void *hal;
	struct s_handler *handler;
	u32 max_para;
};
#endif /* CONFIG_PHL_TEST_VERIFY */
#endif /* _PHL_TEST_VERIFY_DEF_H_ */
