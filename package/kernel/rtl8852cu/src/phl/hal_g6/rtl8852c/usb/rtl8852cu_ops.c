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
#define _RTL8852CU_OPS_C_
#include "../rtl8852c_hal.h"
#include "rtl8852cu.h"

static void init_int_default_value_8852cu(struct hal_info_t *hal, enum rtw_hal_int_set_opt opt)
{
	struct rtw_hal_com_t *hal_com = hal->hal_com;

	hal_com->intr.halt_c2h_int.val_default = (u32)(
		B_AX_HALT_C2H_INT_EN |
		0);

	hal_com->intr.watchdog_timer_int.val_default = (u32)(
		B_AX_WDT_PTFM_INT_EN |
		0);

	hal_com->intr.halt_c2h_int.val_mask = hal_com->intr.halt_c2h_int.val_default;
	hal_com->intr.watchdog_timer_int.val_mask = hal_com->intr.watchdog_timer_int.val_default;
}

static void init_default_value_8852cu(struct hal_info_t *hal)
{
	init_default_value_8852c(hal);

	init_int_default_value_8852cu(hal, INT_SET_OPT_HAL_INIT);
}

void hal_set_ops_8852cu(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal)
{
	struct hal_ops_t *ops = hal_get_ops(hal);

	hal_set_ops_8852c(phl_com, hal);

	ops->init_hal_spec = init_hal_spec_8852cu;
	ops->hal_get_efuse = hal_get_efuse_8852cu;
	ops->hal_init = hal_init_8852cu;
	ops->hal_deinit = hal_deinit_8852cu;
	ops->hal_start = hal_start_8852cu;
	ops->hal_stop = hal_stop_8852cu;
#ifdef CONFIG_WOWLAN
	ops->hal_wow_init = hal_wow_init_8852cu;
	ops->hal_wow_deinit = hal_wow_deinit_8852cu;
#endif /* CONFIG_WOWLAN */
	ops->hal_mp_init = hal_mp_init_8852cu;
	ops->hal_mp_deinit = hal_mp_deinit_8852cu;
	ops->hal_mp_path_chk = hal_mp_path_chk_8852cu;
	ops->hal_hci_configure = hal_hci_cfg_8852cu;
	ops->init_default_value = init_default_value_8852cu;
	ops->init_int_default_value = init_int_default_value_8852cu;
	ops->recognize_interrupt = hal_recognize_int_8852cu;
	ops->interrupt_handler = hal_int_hdler_8852cu;
}



