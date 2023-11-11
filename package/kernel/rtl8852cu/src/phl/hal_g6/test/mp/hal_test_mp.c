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
#define _HAL_TEST_MP_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"
#include "hal_test_mp_api.h"


#ifdef CONFIG_HAL_TEST_MP

void rtw_hal_mp_cfg(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	if (true == phl_is_mp_mode(phl_com))
		hal_ops->hal_mp_init(phl_com, hal_info);
	else
		hal_ops->hal_mp_deinit(phl_com, hal_info);
}

#endif /* CONFIG_HAL_TEST_MP */