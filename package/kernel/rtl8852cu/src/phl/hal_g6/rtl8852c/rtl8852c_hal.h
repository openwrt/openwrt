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
#ifndef _RTL8852C_HAL_H_
#define _RTL8852C_HAL_H_
#include "../hal_headers.h"

/*usage under rtl8852c folder*/
#include "rtl8852c_spec.h"
#include "hal_trx_8852c.h"

#ifdef CONFIG_PCI_HCI
#include "pci/rtl8852ce_hal.h"
#endif

#ifdef CONFIG_USB_HCI
#include "usb/rtl8852cu_hal.h"
#endif

#ifdef CONFIG_SDIO_HCI
#include "sdio/rtl8852cs_hal.h"
#endif

enum hw_stype_8852c {
	EFUSE_HW_STYPE_NONE_8852C = 0,
	EFUSE_HW_STYPE_VF1_CG_8852C = 0xe,
	EFUSE_HW_STYPE_GENERAL_8852C = 0xf
};


/* rtl8852c_halinit.c */
void init_hal_spec_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);
enum rtw_hal_status hal_cfg_fw_8852c(struct rtw_phl_com_t *phl_com,
				     struct hal_info_t *hal,
				     char *ic_name,
				     enum rtw_fw_type fw_type);

enum rf_path hal_get_path_from_ant_num_8852c(u8 antnum);

/* rtl8852c_ops.c */
void hal_set_ops_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);
/*void hal_set_trx_ops_8852c(struct hal_info_t *hal);*/

void init_default_value_8852c(struct hal_info_t *hal);
enum rtw_hal_status hal_get_efuse_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal,
					struct hal_init_info_t *init_info);
enum rtw_hal_status hal_start_8852c(struct rtw_phl_com_t *phl_com,
				   struct hal_info_t *hal,
				   struct hal_init_info_t *init_info);
enum rtw_hal_status hal_stop_8852c(struct rtw_phl_com_t *phl_com,
				     struct hal_info_t *hal);

#ifdef CONFIG_WOWLAN
enum rtw_hal_status
hal_wow_init_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info, struct rtw_phl_stainfo_t *sta,
					struct hal_init_info_t *init_info);
enum rtw_hal_status
hal_wow_deinit_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info, struct rtw_phl_stainfo_t *sta,
					struct hal_init_info_t *init_info);
#endif /* CONFIG_WOWLAN */

#ifdef RTW_PHL_BCN
enum rtw_hal_status hal_config_beacon_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry);
enum rtw_hal_status hal_update_beacon_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry);
#endif

enum rtw_hal_status
hal_mp_init_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info,
					struct hal_init_info_t *init_info);
enum rtw_hal_status
hal_mp_deinit_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info,
					struct hal_init_info_t *init_info);


#endif /* _RTL8852C_HAL_H_ */
