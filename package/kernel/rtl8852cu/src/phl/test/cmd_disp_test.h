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
#ifndef _CMD_DISP_TEST_H_
#define _CMD_DISP_TEST_H_

#ifdef CONFIG_PHL_TEST_SUITE
//#define ENABLE_CMD_DISP_TEST
#ifdef ENABLE_CMD_DISP_TEST
void phl_cmd_disp_test_start(void* phl_info, u8 test_case);
#else
#define phl_cmd_disp_test_start(_phl, _case, _mode)
#endif
#endif

#endif /*_TRX_TEST_H_*/
