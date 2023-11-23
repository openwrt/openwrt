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
#ifndef __HALBB_INTERFACE_H__
#define __HALBB_INTERFACE_H__

/*@--------------------------[Define] ---------------------------------------*/
/*[IO Reg]*/

#define HALBB_SET_CR_CMN(bb, cr, val, phy_idx) halbb_set_reg_cmn(bb, cr, cr##_M, val, phy_idx);
#define HALBB_SET_CR(bb, cr, val) halbb_set_reg(bb, cr, cr##_M, val);

#define HALBB_GET_CR_CMN(bb, cr, val, phy_idx) halbb_get_reg_cmn(bb, cr, cr##_M, phy_idx);
#define HALBB_GET_CR(bb, cr) halbb_get_reg(bb, cr, cr##_M);

#if 1
#define halbb_get_32(bb, addr) hal_read32((bb)->hal_com, addr)
//#define halbb_get_16(bb, addr) hal_read16((bb)->hal_com, addr)
//#define halbb_get_8(bb, addr) hal_read8((bb)->hal_com, addr)
#define halbb_set_32(bb, addr, val) hal_write32((bb)->hal_com, addr, val)
//#define halbb_set_16(bb, addr, val) hal_write16((bb)->hal_com, addr, val)
//#define halbb_set_8(bb, addr, val) hal_write8((bb)->hal_com, addr, val)
#endif

#define halbb_read_mem(bb, addr, cnt, pmem) hal_read_mem((bb)->hal_com, addr, cnt, pmem)

/*[Delay]*/
#define halbb_delay_ms(bb, ms) _os_delay_ms(bb->hal_com->drv_priv, ms)
//#define halbb_delay_us(bb, us) _os_delay_us(bb->hal_com->drv_priv, us)

/*[Memory Access]*/
#define halbb_mem_alloc(bb, buf_sz) hal_mem_alloc(bb->hal_com, buf_sz)
#define halbb_mem_free(bb, buf, buf_sz) hal_mem_free(bb->hal_com, (void *)buf, buf_sz)
#define halbb_mem_set(bb, buf, value, size) _os_mem_set(bb->hal_com->drv_priv, (void *)buf, value, size)
#define halbb_mem_cpy(bb, dest, src, size) _os_mem_cpy(bb->hal_com->drv_priv, (void *)dest, (void *)src, size)
#define halbb_mem_cmp(bb, dest, src, size) _os_mem_cmp(bb->hal_com->drv_priv, (void *)dest, (void *)src, size)

/*[Timer]*/
#ifdef HALBB_TIMER_SUPPORT
#define halbb_timer_list	_os_timer
#define halbb_init_timer(bb, timer, call_back_func, context, sz_id) _os_init_timer(bb->hal_com->drv_priv, timer, call_back_func, context, sz_id)
#define halbb_set_timer(bb, timer, ms_delay) _os_set_timer(bb->hal_com->drv_priv, timer, ms_delay)
#define halbb_cancel_timer(bb, timer) _os_cancel_timer(bb->hal_com->drv_priv, timer)
#define halbb_release_timer(bb, timer) _os_release_timer(bb->hal_com->drv_priv, timer)
#else
#define halbb_timer_list	u8
#define halbb_init_timer(bb, timer, call_back_func, context, sz_id)
#define halbb_set_timer(bb, timer, ms_delay)
#define halbb_cancel_timer(bb, timer)
#define halbb_release_timer(bb, timer)
#endif

/*[Efuse]*/
#ifndef RTW_FLASH_98D
#define halbb_efuse_get_info(bb, info_type, value, size) rtw_hal_efuse_get_info(bb->hal_com, info_type, (void *)value, size)
#else
#define halbb_efuse_get_info(bb, info_type, value, size) rtw_hal_flash_get_info(bb->hal_com, info_type, (void *)value, size)
#endif
#define halbb_phy_efuse_get_info(bb, addr, size, value) rtw_hal_mac_read_phy_efuse(bb->hal_com, addr, size, value)

/*[String]*/
#define halbb_snprintf _os_snprintf

/*[PwrTable]*/
#define PWR_TBL_NUM 32
#define NUM_HE_MCS 12
#define NUM_DCM_MCS 4


/*@--------------------------[Enum]------------------------------------------*/
enum bb_timer_cfg_t {
	BB_INIT_TIMER		= 0,
	BB_CANCEL_TIMER		= 1,
	BB_RELEASE_TIMER	= 2,
	BB_SET_TIMER		= 3
};

enum halbb_h2c_ra_cmdid {
	RA_H2C_MACIDCFG		= 0x0,
	RA_H2C_RSSISETTING	= 0x1,
	RA_H2C_GET_TXSTS	= 0x2,
	RA_H2C_RA_ADJUST	= 0x3,
	RA_H2C_ADJUST_RA_MASK		= 0x4,
	RA_H2C_RA_D_O_TIMER	= 0x5,
	RA_H2C_RA_CLS		= 0x6,
	RA_H2C_RA_SHIFT_DARF_TC	= 0x7,
	RA_H2C_MUCFG		= 0x10,
	RA_MAX_H2CCMD
};

enum halbb_h2c_rua_cmdid {
	RUA_H2C_TABLE		= 0x0,
	RUA_H2C_SWGRP		= 0x1,
	RUA_H2C_DL_MACID	= 0x2,
	RUA_H2C_UL_MACID	= 0x3,
	RUA_H2C_CSIINFO		= 0x4,
	RUA_H2C_CQI			= 0x5,
	RUA_H2C_BBINFO		= 0x6,
	RUA_H2C_SEN_TBL		= 0x7,
	RUA_H2C_PWR_TBL		= 0x8,
	RUA_H2C_DBG 		= 0x9,
	RUA_H2C_DBG_W 		= 0x10,	
	RUA_MAX_H2CCMD
};

enum halbb_h2c_classid {
	HALBB_H2C_RUA		= 0x0,
	HALBB_H2C_RA		= 0x1,
	HALBB_H2C_DM		= 0x2,
	HALBB_MAX_H2CCMD
};

enum halbb_h2c_dm_cmdid {
	DM_H2C_FWTRACE		= 0x0,
	DM_H2C_FW_TRIG_TX	= 0x1,
	DM_H2C_FW_H2C_TEST	= 0x2,
	DM_H2C_FW_EHTSIG_SIGB	= 0x3,
	DM_H2C_FW_EDCCA		= 0x4,
	DM_H2C_FW_CMW		= 0x5,
	DM_H2C_FW_MCC		= 0x6,
	DM_H2C_FW_L6M_WA	= 0x7,
	DM_H2C_FW_ENV_MNTR	= 0x8,
	DM_H2C_FW_LPS_INFO	= 0x9,
	DM_MAX_H2CCMD
};

enum halbb_c2h_classid {
	HALBB_C2H_RUA		= 0x0,
	HALBB_C2H_RA		= 0x1,
	HALBB_C2H_DM		= 0x2,
	HALBB_MAX_C2HCMD
};

enum halbb_c2h_ra_cmdid {
	HALBB_C2HRA_STS_RPT		= 0x0,
	HALBB_C2HRA_MU_GPTBL_RPT	= 0x1,
	HALBB_C2HRA_TXSTS		= 0x2,
	HALBB_MAX_C2HRACMD
};

enum halbb_c2h_rua_cmdid {
	HALBB_C2HRUA_DBG		= 0x0,
	HALBB_MAX_C2HRUACMD
};


enum halbb_c2h_dm_cmdid {
	DM_C2H_FW_TEST			= 0x0,
	DM_C2H_FW_TRIG_TX_RPT		= 0x1,
	DM_C2H_SIGB			= 0x2,
	DM_C2H_LOWRT_RTY		= 0x3,
	DM_C2H_MCC_DIG			= 0x4,
	DM_C2H_FW_ENV_MNTR		= 0x5,
	HALBB_MAX_C2HDMCMD
};

enum halbb_event_idx_t {
	/*timer*/
	BB_EVENT_TIMER_DIG		= 0,
	BB_EVENT_TIMER_CFO		= 1,
	BB_EVENT_TIMER_ANTDIV		= 2,
	BB_EVENT_TIMER_TDMA_CR		= 3,
	BB_EVENT_TIMER_LA		= 4,
	BB_EVENT_TIMER_DTR		= 5
};

enum halbb_timer_state_t {
	/*timer*/
	BB_TIMER_IDLE			= 0,
	BB_TIMER_RUN			= 1,
	BB_TIMER_RELEASE		= 0xff,
};

/*@--------------------------[Structure]-------------------------------------*/
struct halbb_timer_info {
	halbb_timer_list timer_list;
	u32	cb_time; /*callback time (ms)*/
	enum halbb_event_idx_t	event_idx;
	enum halbb_timer_state_t timer_state;
};

struct halbb_pwr_by_rate_tbl {
	u8 pwr_by_rate[PWR_TBL_NUM*2];
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;

void halbb_cfg_timers(struct bb_info *bb, enum bb_timer_cfg_t cfg,
		      struct halbb_timer_info *timer);
u32 halbb_get_sys_time(struct bb_info *bb);
u32 halbb_phy0_to_phy1_ofst(struct bb_info *bb, u32 addr, enum phl_phy_idx phy_idx);
void halbb_delay_us(struct bb_info *bb, u32 us);
void halbb_set_cr(struct bb_info *bb, u32 addr, u32 val);
u32 halbb_get_cr(struct bb_info *bb, u32 addr);
void halbb_set_reg_curr_phy(struct bb_info *bb, u32 addr, u32 mask, u32 val);
void halbb_set_reg_phy0_1(struct bb_info *bb, u32 addr, u32 mask, u32 val);
u32 halbb_get_reg_curr_phy(struct bb_info *bb, u32 addr, u32 mask);
u32 halbb_get_reg_phy0_1(struct bb_info *bb, u32 addr, u32 mask, u32 *val_1);
bool halbb_fill_h2c_cmd(struct bb_info *bb, u16 cmdlen, u8 cmdid,
			u8 classid, u32 *pval);
bool halbb_test_h2c_c2h_flow(struct bb_info *bb);

bool halbb_check_fw_ofld(struct bb_info *bb);
bool halbb_fw_set_reg(struct bb_info *bb, u32 addr, u32 mask, u32 val, u8 lst_cmd);
bool halbb_fw_set_reg_cmn(struct bb_info *bb, u32 addr, u32 mask, u32 val, 
			            enum phl_phy_idx phy_idx, u8 lst_cmd);
bool halbb_fw_set_rf_reg(struct bb_info *bb, enum rf_path path,
			      u32 reg_addr, u32 bit_mask, u32 data);
enum rtw_hal_status
halbb_config_cmac_tbl_ax(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i,
			void *cmac_ctrl,
			void *cmac_ctrl_mask);
enum rtw_hal_status
halbb_config_cmac_tbl_be(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i,
			void *cmac_ctrl,
			void *cmac_ctrl_mask);

#endif
