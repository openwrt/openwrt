/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef __RTW_PHL_CMD__
#define __RTW_PHL_CMD__
u32 rtw_enqueue_phl_cmd(struct cmd_obj *pcmd);
#ifdef CONFIG_TDLS
enum rtw_phl_status rtw_send_tdls_sync_msg(_adapter *padapter);
#endif
#if defined (CONFIG_CMD_GENERAL) && defined (CONFIG_PCIE_TRX_MIT)
u8 rtw_pcie_trx_mit_cmd(_adapter *padapter, u32 tx_timer, u8 tx_counter,
			u32 rx_timer, u8 rx_counter, u8 fixed_mit);
#endif
#endif /* __RTW_PHL_CMD__ */

