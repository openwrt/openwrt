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
#ifndef __HALRF_DBG_H__
#define __HALRF_DBG_H__

/*@--------------------------[Define] ---------------------------------------*/
#define HALRF_WATCHDOG_PERIOD	2 /*second*/



#define RFDBG_TRACE_EN
#ifdef RFDBG_TRACE_EN
	#define RF_DBG(rf, comp, fmt, ...)     \
		do {\
			if(rf->dbg_component & comp)\
				_os_dbgdump("[RF]" fmt, ##__VA_ARGS__);\
		} while (0)
		
	#define RF_TRACE(fmt, ...)     \
		do {\
			_os_dbgdump("[RF]" fmt, ##__VA_ARGS__);\
		} while (0)
		
	#define RF_WARNING(fmt, ...)     \
		do {\
			_os_dbgdump("[WARNING][RF]" fmt, ##__VA_ARGS__);\
		} while (0)

	#define RF_DBG_VAST(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
		do {\
			_os_dbgdump(fmt, ##__VA_ARGS__);\
		} while (0)

	#define	RF_DBG_CNSL(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
		do {									\
			u32 *used_len_tmp = &(used_len);				\
			if (*used_len_tmp < max_buff_len)				\
				*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
		} while (0)
#else
	#define RF_DBG
	#define RF_TRACE
	#define RF_WARNING
	#define RF_DBG_CNSL		/*Print on Consol,CLI */
	#define RF_DBG_VAST		/*Print to Comport, Debug View*/
#endif	


/*@--------------------------[Enum]------------------------------------------*/
 
/*@--------------------------[Structure]-------------------------------------*/
 
/*@--------------------------[Prptotype]-------------------------------------*/
struct rf_info;
void halrf_dbg_setting_init(struct rf_info *rf);
void halrf_iqk_log(struct rf_info *rf);
void halrf_lck_log(struct rf_info *rf);
void halrf_dump_rfk_reg(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_support_ability(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_dbg_trace(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_dpk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_dpk_track_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_rx_dck_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_dack_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_tssi_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_iqk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
void halrf_iqk_bypass_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
void halrf_iqk_klog_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
void halrf_pwr_table_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_rfk_check_reg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_test_cmd(struct rf_info *rf, char input[][16], u32 *_used,  char *output, u32 *_out_len);
void halrf_scanf(char *in, enum rf_scanf_type type, u32 *out);
void halrf_txgapk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_dump_rf_reg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_hwtx_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_kfree_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_chl_rfk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_op5k_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halrf_rfk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used, char *output, u32 *_out_len);

#endif
