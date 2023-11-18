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
#ifndef _PHL_STATUS_H_
#define _PHL_STATUS_H_

enum rtw_phl_status {
	RTW_PHL_STATUS_SUCCESS, /* 0 */
	RTW_PHL_STATUS_FAILURE, /* 1 */
	RTW_PHL_STATUS_RESOURCE, /* 2 */
	RTW_PHL_STATUS_HAL_INIT_FAILURE, /* 3 */
	RTW_PHL_STATUS_PENDING, /* 4 */
	RTW_PHL_STATUS_FRAME_DROP, /* 5 */
	RTW_PHL_STATUS_INVALID_PARAM, /* 6 */
	RTW_PHL_STATUS_CMD_TIMEOUT, /* 7 */
	RTW_PHL_STATUS_CMD_ERROR, /* 8 */
	RTW_PHL_STATUS_CMD_DROP, /* 9 */
	RTW_PHL_STATUS_CMD_CANNOT_IO, /* 10 */
	RTW_PHL_STATUS_CMD_SUCCESS, /* 11 */
	RTW_PHL_STATUS_UNEXPECTED_ERROR, /* 12 */
	RTW_PHL_STATUS_CANNOT_IO, /* 13 */
	RTW_PHL_STATUS_SH_TASK, /* 14 */
};

#define is_cmd_failure(psts)	((psts == RTW_PHL_STATUS_CMD_TIMEOUT) || \
				 (psts == RTW_PHL_STATUS_CMD_ERROR) || \
				 (psts == RTW_PHL_STATUS_CMD_DROP) || \
				 (psts == RTW_PHL_STATUS_CMD_CANNOT_IO))

enum phl_mdl_ret_code {
	MDL_RET_SUCCESS = 0,
	MDL_RET_FAIL,
	MDL_RET_IGNORE,
	MDL_RET_PENDING,
	MDL_RET_CANNOT_IO,
};

#endif /*_PHL_STATUS_H_*/

