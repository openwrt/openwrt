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
#ifndef _PHL_TEST_VERIFY_API_H_
#define _PHL_TEST_VERIFY_API_H_

#ifdef CONFIG_PHL_TEST_VERIFY
/* verify command class */
enum verify_feature {
	VERIFY_FEATURES_NULL = 0,
	VERIFY_FEATURES_DBCC,
	VERIFY_FEATURES_MAX,
};

#define VERIFY_CMD_HDR_SIZE (sizeof(struct verify_cmd_hdr))
#define VERIFY_GET_SUBUF(a) ((u8 *)(a) + VERIFY_CMD_HDR_SIZE)

static inline enum rtw_phl_status func_null(void *priv)
{
	return RTW_PHL_STATUS_SUCCESS;
};
#endif /* CONFIG_PHL_TEST_VERIFY */
#endif /* _PHL_TEST_VERIFY_API_H_ */
