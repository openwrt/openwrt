/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALRF_INTERFACE_H__
#define __HALRF_INTERFACE_H__

#ifdef CONFIG_FW_IO_OFLD_SUPPORT
#define HALRF_CONFIG_FW_IO_OFLD_SUPPORT
#endif

#ifdef CONFIG_FW_DBCC_OFLD_SUPPORT
#define HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
#endif

#ifdef CONFIG_HAL_THERMAL_PROTECT
#define HALRF_THERMAL_PROTECT_SUPPORT
#endif

#define CF_PHL_BB_CTRL_RX_CCA

/*@--------------------------[Define] ---------------------------------------*/
/*[IO Reg]*/
#define RF_OFST 0x10000
#define halrf_btc_ntfy(rf, idx, type, process) rtw_hal_btc_wl_rfk_ntfy((rf)->hal_com, idx, type, process) 
#define halrf_r32(rf, addr) hal_read32((rf)->hal_com, (addr | RF_OFST))
#define halrf_r16(rf, addr) hal_read16((rf)->hal_com, (addr | RF_OFST))
#define halrf_r8(rf, addr) hal_read8((rf)->hal_com, (addr | RF_OFST))
#define halrf_w32(rf, addr, val) hal_write32((rf)->hal_com, (addr | RF_OFST), val)
#define halrf_w16(rf, addr, val) hal_write16((rf)->hal_com, (addr | RF_OFST), val)
#define halrf_w8(rf, addr, val) hal_write8((rf)->hal_com, (addr | RF_OFST), val)
//#define halrf_rrf(rf, path, addr, mask) rtw_hal_read_rf_reg((rf)->hal_com, path, addr, mask)
/*#define halrf_wrf(rf, path, addr, mask, val) rtw_hal_write_rf_reg((rf)->hal_com, path, addr, mask, val)*/
/*#define halrf_wmac32(rf, addr, val) hal_write32((rf)->hal_com, addr, val)*/
//#define halrf_rmac32(rf, addr) hal_read32((rf)->hal_com, addr)
#define halrf_wmac32(rf, addr, val) hal_write32((rf)->hal_com, addr, val)
#define halrf_read_mem(rf, addr, cnt, pmem) hal_read_mem((rf)->hal_com, addr, cnt, pmem)

/*[TX]*/
#define halrf_tx_pause(rf, band_idx, tx_pause, rson) rtw_hal_tx_pause((rf)->hal_com, band_idx, tx_pause, rson)
#define halrf_tx_mode_switch(rf, phy_idx, mode) rtw_hal_bb_tx_mode_switch((rf)->hal_com, phy_idx, mode)
#define halrf_query_regulation_info(rf, info) rtw_hal_query_regulation((rf)->phl_com->phl_priv, info)
#define halrf_hal_bb_backup_info(rf, phy_idx) rtw_hal_bb_backup_info((rf)->hal_com, phy_idx)
#define halrf_hal_bb_restore_info(rf, phy_idx) rtw_hal_bb_restore_info((rf)->hal_com, phy_idx)

/*[Delay]*/
#define halrf_delay_ms(rf, ms) _os_delay_ms(rf->hal_com->drv_priv, ms)
#define halrf_os_delay_us(rf, us) _os_delay_us(rf->hal_com->drv_priv, us)

/*[Memory Access]*/
#define halrf_mem_set(rf, buf, value, size) _os_mem_set(rf->hal_com->drv_priv, (void *)buf, value, size)
#define halrf_mem_cpy(rf, dest, src, size) _os_mem_cpy(rf->hal_com->drv_priv, (void *)dest, (void *)src, size)
#define halrf_mem_cmp(rf, dest, src, size) _os_mem_cmp(rf->hal_com->drv_priv, (void *)dest, (void *)src, size)
#define halrf_mem_alloc(rf, buf_sz) hal_mem_alloc(rf->hal_com, buf_sz)
#define halrf_mem_free(rf, buf, buf_sz) hal_mem_free(rf->hal_com, (void *)buf, buf_sz)


/*[Timer]*/
#define halrf_init_timer(rf, timer, call_back_func, context, sz_id) _os_init_timer(rf->hal_com->drv_priv, timer, call_back_func, context, sz_id)
#define halrf_set_timer(rf, timer, ms_delay) _os_set_timer(rf->hal_com->drv_priv, timer, ms_delay)
#define halrf_cancel_timer(rf, timer) _os_cancel_timer(rf->hal_com->drv_priv, timer)
#define halrf_release_timer(rf, timer) _os_release_timer(rf->hal_com->drv_priv, timer)

/*efuse*/
#ifndef RTW_FLASH_98D
#define halrf_efuse_get_info(rf, info_type, value, size) rtw_hal_efuse_get_info((rf)->hal_com, info_type, (void *)value, size)
#else
#define halrf_efuse_get_info(rf, info_type, value, size) rtw_hal_flash_get_info((rf)->hal_com, info_type, (void *)value, size)
#endif /*RTW_FLASH_98D*/
#define halrf_phy_efuse_get_info(rf, addr, size, value) rtw_hal_mac_read_phy_efuse((rf)->hal_com, addr, size, value)

/*GPIO*/
/*#ifndef RF_8852B_SUPPORT*/
#define halrf_gpio_setting_all(rf, rfe_idx) rtw_hal_bb_gpio_setting_all((rf)->hal_com, rfe_idx)
#define halrf_gpio_setting(rf, gpio_idx, path, inv, src) rtw_hal_bb_gpio_setting((rf)->hal_com, gpio_idx, path, inv, src)
#define halrf_set_gpio_func(rf, func, gpio_cfg) rtw_hal_mac_set_gpio_func((rf)->hal_com, func, gpio_cfg)
/*#endif*/

/*Set power by rate, power limit, power */
#define halrf_mac_write_pwr_limit_rua_reg(rf, band) rtw_hal_mac_write_pwr_limit_rua_reg((rf)->hal_com, band)
#define halrf_mac_write_pwr_limit_reg(rf, band) rtw_hal_mac_write_pwr_limit_reg((rf)->hal_com, band)
#define halrf_mac_write_pwr_by_rate_reg(rf, band) rtw_hal_mac_write_pwr_by_rate_reg((rf)->hal_com, band)
#define halrf_bb_set_tx_pow_ref(rf, phy_idx) rtw_hal_bb_set_tx_pow_ref((rf)->hal_com, phy_idx)
#define halrf_mac_write_pwr_ofst_mode(rf, phy_idx) rtw_hal_mac_write_pwr_ofst_mode((rf)->hal_com, phy_idx)
#define halrf_mac_write_pwr_ofst_bw(rf, phy_idx) rtw_hal_mac_write_pwr_ofst_bw((rf)->hal_com, phy_idx)
#define halrf_mac_write_pwr_limit_en(rf, phy_idx) rtw_hal_mac_write_pwr_limit_en((rf)->hal_com, phy_idx)
#define halrf_bb_set_pow_patten_sharp(rf, channel, is_cck, sharp_id, phy_idx) rtw_hal_bb_set_pow_patten_sharp((rf)->hal_com, channel, is_cck, sharp_id, phy_idx)

#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
/*FW offload*/
#define halrf_mac_add_cmd_ofld(rf, cmd) rtw_hal_mac_add_cmd_ofld((rf)->hal_com, cmd)
#endif

/*BB related*/
#define halrf_bb_ctrl_rx_cca(rf, cca_en, phy_idx) rtw_hal_bb_ctrl_rx_cca((rf)->hal_com, cca_en, phy_idx)
#define halrf_bb_ctrl_cck_en(rf, cck_en, phy_idx) rtw_hal_bb_ctrl_cck_en((rf)->hal_com, cck_en, phy_idx)
#define halrf_bb_query_cck_en(rf, phy_idx) rtw_hal_bb_query_cck_en((rf)->hal_com, phy_idx)

#define halrf_mutex_init(rf, mutex) hal_mutex_init((rf)->hal_com, mutex)
#define halrf_mutex_deinit(rf, mutex) hal_mutex_deinit((rf)->hal_com, mutex)
#define halrf_mutex_lock(rf, mutex) hal_mutex_lock((rf)->hal_com, mutex)
#define halrf_mutex_unlock(rf, mutex) hal_mutex_unlock((rf)->hal_com, mutex)
/*@--------------------------[Enum]------------------------------------------*/


/*@--------------------------[Structure]-------------------------------------*/
 
/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;

u32 halrf_cal_bit_shift(u32 bit_mask);

u32 halrf_get_sys_time(struct rf_info *rf);

void halrf_wmac(struct rf_info *rf, u32 addr, u32 mask, u32 val);

void halrf_wreg(struct rf_info *rf, u32 addr, u32 bit_mask, u32 val);

bool halrf_polling_bb(struct rf_info *rf, u32 addr, u32 mask, u32 val, u32 count);

bool halrf_polling_rf(struct rf_info *rf, u32 path, u32 addr, u32 mask, u32 val, u32 count);

u32 halrf_rreg(struct rf_info *rf, u32 addr, u32 bit_mask);

void halrf_wrf(struct rf_info *rf, enum rf_path path, u32 addr, u32 mask, u32 val);

u32 halrf_rrf(struct rf_info *rf, enum rf_path path, u32 addr, u32 mask);

void halrf_fill_h2c_cmd(struct rf_info *rf, u16 cmdlen, u8 cmdid,
			u8 classid, u32 cmdtype, u32 *pval);
			
void halrf_delay_us(struct rf_info *rf, u32 count);

u32 halrf_rmac32(struct rf_info *rf, u32 addr);
#endif
