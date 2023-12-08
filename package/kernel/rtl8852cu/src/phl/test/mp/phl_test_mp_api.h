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
#ifndef _PHL_TEST_MP_API_H_
#define _PHL_TEST_MP_API_H_

#ifdef CONFIG_PHL_TEST_MP
enum rtw_phl_status mp_config(struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_phl_status mp_tx(struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_phl_status mp_rx(struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_phl_status mp_reg(struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_phl_status mp_efuse(struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_phl_status mp_txpwr(struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_phl_status mp_cal(struct mp_context *mp, struct mp_cal_arg *arg);
#endif /* CONFIG_PHL_TEST_MP */

#endif /* _PHL_TEST_MP_API_H_ */
