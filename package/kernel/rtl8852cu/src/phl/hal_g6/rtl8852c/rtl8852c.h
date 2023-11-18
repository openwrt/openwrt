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
#ifndef _RTL8852C_H_
#define _RTL8852C_H_

/*to communicate with upper-hal*/
#ifdef CONFIG_PCI_HCI
void hal_set_ops_8852ce(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);
u32 hal_hook_trx_ops_8852ce(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info);
#endif

#ifdef CONFIG_USB_HCI
void hal_set_ops_8852cu(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);
u32 hal_hook_trx_ops_8852cu(struct hal_info_t *hal_info);

#endif

#ifdef CONFIG_SDIO_HCI
void hal_set_ops_8852cs(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);
u32 hal_hook_trx_ops_8852cs(struct hal_info_t *hal_info);

#endif

#endif /* _RTL8852C_H_ */
