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
#define _RTL8852C_OPS_C_
#include "rtl8852c_hal.h"

static void read_chip_version_8852c(struct rtw_phl_com_t *phl_com,
						struct hal_info_t *hal)
{
	hal_mac_get_hwinfo(hal, &(phl_com->hal_spec));

}

/*******************temp common IO  APIs *******************/
extern u32 hal_read_macreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask);
extern void hal_write_macreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask, u32 data);
extern u32 hal_read_bbreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask);
extern void hal_write_bbreg(struct hal_info_t *hal,
		u32 offset, u32 bit_mask, u32 data);
extern u32 hal_read_rfreg(struct hal_info_t *hal,
		enum rf_path path, u32 offset, u32 bit_mask);
extern void hal_write_rfreg(struct hal_info_t *hal,
		enum rf_path path, u32 offset, u32 bit_mask, u32 data);

void hal_set_ops_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal)
{
	struct hal_ops_t *ops = hal_get_ops(hal);

	/*** initialize section ***/
	ops->read_chip_version = read_chip_version_8852c;
	ops->hal_cfg_fw = hal_cfg_fw_8852c;

	ops->read_macreg = hal_read_macreg;
	ops->write_macreg = hal_write_macreg;
	ops->read_bbreg = hal_read_bbreg;
	ops->write_bbreg = hal_write_bbreg;
	ops->read_rfreg = hal_read_rfreg;
	ops->write_rfreg = hal_write_rfreg;

#ifdef RTW_PHL_BCN
	ops->cfg_bcn = hal_config_beacon_8852c;
	ops->upt_bcn = hal_update_beacon_8852c;
#endif
	ops->get_path_from_ant_num = hal_get_path_from_ant_num_8852c;
}

#if 0
void hal_set_trx_ops_8852c(struct hal_info_t *hal)
{
	struct hal_trx_ops_t *ops = hal_get_trx_ops(hal);

	ops->get_txdesc_len = get_txdesc_len_8852c;
	ops->fill_txdesc_h2c = fill_txdesc_h2c_8852c;
	ops->fill_txdesc_fwdl = fill_txdesc_fwdl_8852c;
	ops->fill_txdesc_pkt = fill_txdesc_pkt_8852c;
}
#endif

