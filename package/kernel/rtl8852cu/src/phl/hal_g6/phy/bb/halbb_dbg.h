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
#ifndef __HALBB_DBG_H__
#define __HALBB_DBG_H__

#include "../../hal_headers_le.h"

/*@--------------------------[Define] ---------------------------------------*/
#define HALBB_WATCHDOG_PERIOD	2 /*second*/

#define PHY_HIST_SIZE		12
#define PHY_HIST_TH_SIZE	(PHY_HIST_SIZE - 1)

#define LA_CLK_EN 	0x014 /*Just for dbg, will be removed*/
#define LA_CLK_EN_M 	0x1 /*Just for dbg, will be removed*/

#define FRC_PRINT_LINE 0xffffffff

#ifdef HALBB_DBG_TRACE_SUPPORT
	#ifdef HALBB_DBCC_SUPPORT
		#define BB_DBG(bb, comp, fmt, ...)     \
			do {\
				if(bb->dbg_component & comp) {\
					_os_dbgdump("[BB][%d]" fmt, bb->bb_phy_idx, ##__VA_ARGS__);\
				} \
			} while (0)

		#define BB_TRACE1(bb, fmt, ...)     \
			do {\
				_os_dbgdump("[BB][%d]" fmt, bb->bb_phy_idx, ##__VA_ARGS__);\
			} while (0)
	#else
		#define BB_DBG(bb, comp, fmt, ...)     \
			do {\
				if(bb->dbg_component & comp) {\
					_os_dbgdump("[BB]" fmt, ##__VA_ARGS__);\
				} \
			} while (0)

		#define BB_TRACE1(bb, fmt, ...)     \
			do {\
				_os_dbgdump("[BB]" fmt, ##__VA_ARGS__);\
			} while (0)
	#endif

	#define BB_TRACE(fmt, ...)     \
		do {\
			_os_dbgdump("[BB]" fmt, ##__VA_ARGS__);\
		} while (0)
		
	#define BB_WARNING(fmt, ...)     \
		do {\
			_os_dbgdump("[WARNING][BB]" fmt, ##__VA_ARGS__);\
		} while (0)

	#define	BB_DBG_CNSL2(in_cnsl, max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
		do {								\
			u32 *used_len_tmp = &(used_len);			\
			u32 len_tmp = 0;					\
			if (*used_len_tmp < max_buff_len) {			\
				len_tmp = _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__); \
				if (in_cnsl) {					\
					*used_len_tmp += len_tmp; 		\
				} else {					\
					BB_TRACE("%s\n", buff_addr); 		\
				}						\
			}\
		} while (0)
#else
	#define BB_DBG
	#define BB_TRACE
	#define BB_TRACE1
	#define BB_WARNING
	#define	BB_DBG_CNSL2(in_cnsl, max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
		do {								\
			u32 *used_len_tmp = &(used_len);				\
			if (*used_len_tmp < max_buff_len)				\
				*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
		} while (0)
#endif

#define BB_DBG_VAST(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {\
		_os_dbgdump("[CNSL]" fmt, ##__VA_ARGS__);\
	} while (0)

#define	BB_DBG_CNSL(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {									\
		u32 *used_len_tmp = &(used_len);				\
		if (*used_len_tmp < max_buff_len)				\
			*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
	} while (0)

#define	DBGPORT_PRI_3	3	/*@Debug function (the highest priority)*/
#define	DBGPORT_PRI_2	2	/*@Check hang function & Strong function*/
#define	DBGPORT_PRI_1	1	/*Watch dog function*/
#define	DBGPORT_RELEASE	0	/*@Init value (the lowest priority)*/

/*@--------------------------[Enum]------------------------------------------*/
enum bb_dbg_devider_len_t
{
	BB_DEVIDER_LEN_32 = 0,
	BB_DEVIDER_LEN_16 = 1,
};

enum bb_dbg_port_ip_t
{
	DBGPORT_IP_TD		= 1,
	DBGPORT_IP_RX_INNER	= 2,
	DBGPORT_IP_TX_INNER	= 3,
	DBGPORT_IP_OUTER	= 4,
	DBGPORT_IP_INTF		= 5,
	DBGPORT_IP_CCK		= 6,
	DBGPORT_IP_BF		= 7,
	DBGPORT_IP_RX_OUTER	= 8,
	DBGPORT_IP_RFC0		= 0X1B,
	DBGPORT_IP_RFC1		= 0X1C,
	DBGPORT_IP_RFC2		= 0X1D,
	DBGPORT_IP_RFC3		= 0X1E,
	DBGPORT_IP_TST		= 0X1F,
};

enum bb_frc_phy_dump_reg
{
	FRC_DUMP_PHY0		= 0,
	FRC_DUMP_PHY1		= 1,
	FRC_DUMP_ALL
};

enum bb_basic_dbg_info
{
	BB_BASIC_DBG_01_SYSTEM		= BIT1,
	BB_BASIC_DBG_02_ENVMNTR		= BIT2,
	BB_BASIC_DBG_03_PMAC		= BIT3,
	BB_BASIC_DBG_04_TX		= BIT4,
	BB_BASIC_DBG_05_RX		= BIT5,
	BB_BASIC_DBG_06_RSSI_RATE	= BIT6,
	BB_BASIC_DBG_07_HIST		= BIT7,
	BB_BASIC_DBG_08_RSSI_RATE_MU	= BIT8,
	BB_BASIC_DBG_09_DM_SUMMARY	= BIT9
};

/*@--------------------------[Structure]-------------------------------------*/
struct bb_dbg_cr_info {
	u32 dbgport_ip;
	u32 dbgport_ip_m;
	u32 dbgport_idx;
	u32 dbgport_idx_m;
	u32 dbgport_val;
	u32 dbgport_val_m;
	u32 clk_en;
	u32 clk_en_m;
	u32 dbgport_en;
	u32 dbgport_en_m;
	u32 bb_monitor_sel1;
	u32 bb_monitor_sel1_m;
	u32 bb_monitor1;
	u32 bb_monitor1_m;
	u32 mac_phy_intf_sel_phy1;
	u32 mac_phy_intf_sel_phy1_m;
	u32 mac_phy_txinfo[4];
	u32 mac_phy_txcomct[2];
	u32 mac_phy_txusrct[4][2];
	u32 mac_phy_txtimct;
	u32 mac_phy_lsig;
	u32 mac_phy_siga_0;
	u32 mac_phy_siga_1;
	u32 mac_phy_vht_sigb_0;
	u32 path_0_txpw;
	u32 path_0_txpw_m;
	u32 path_1_txpw;
	u32 path_1_txpw_m;
};

struct bb_tx_info {
	/*From reg*/
	u8 type;
	u8 tx_path_en;
	u8 path_map;
	u8 txcmd_num;
	u8 txsc;
	u8 bw;
	u16 tx_pw; /*tmac*/
	u8 n_usr;
	u8 max_mcs;
	bool stbc;
	u8 gi;
	u8 ltf;
	u8 u_id[4];
	u8 n_sts[4];
	bool fec[4];
	u8 mcs[4];
	bool dcm[4];
	u8 precoding[4];
	u16 n_sym;
	u8 pkt_ext;
	u8 pre_fec;
	u32 l_sig;
	u32 sig_a1;
	u32 sig_a2;
	u32 sig_b;
	u32 txinfo[4];
	u32 txcomct[2];
	u32 txusrct[4][2];
	u32 txtimct;
	u16 txpw_path0; /*bb_path0*/
	u16 txpw_path1; /*bb_path1*/
	/*sw variable*/
	u16 t_data;
	u32 psdu_length;
};

struct bb_ra_dbgreg {
	u32 cmac_tbl_id0;
	u32 cmac_tbl_id1;
	u32 per;
	u32 rdr;
	u32 r4;
	u32 cls;
	u32 rate_up_lmt_cnt;
	u32 per_ma;
	u32 var;
	u32 d_o_n;
	u32 d_o_p;
	u32 rd_th;
	u32 ru_th;
	u32 try_per;
	u32 try_rdr;
	u32 try_r4;
	u32 txrpt_tot;
	u32 ra_timer;
	u32 tot_disra_trying_return;
	u32 r4_return;
	u32 ra_mask_h;
	u32 ra_mask_l;
	u32 highest_rate;
	u32 lowest_rate;
	u32 upd_all_h2c_0;
	u32 upd_all_h2c_1;
	u32 upd_all_h2c_2;
	u32 upd_all_h2c_3;
	u32 dyn_stbc;
	u32 mu_mcs;
	u32 mu_id_lowest_rate;
	u32 mu_rd_ru_th;
	u32 mu_per;
};

struct bb_dbg_info {
	bool	cr_recorder_en;
	bool	cr_init_hook_recorder_en;
	bool	cr_fake_init_hook_en;
	u32	cr_fake_init_hook_val;
	bool	cr_recorder_rf_en; /*HALRF write BB CR*/
	/*CR init debug control*/
	bool	cr_dbg_mode_en;
	u32	cut_curr_dbg;
	u32	rfe_type_curr_dbg;
#ifdef HALBB_TDMA_CR_SUPPORT
	struct halbb_timer_info tdma_cr_timer_i;
	bool		tdma_cr_en;
	u8		tdma_cr_state;
	u32		tdma_cr_idx;
	u32		tdma_cr_mask;
	u32		tdma_cr_val_0;
	u32		tdma_cr_val_1;
	u32		tdma_cr_period_0;
	u32		tdma_cr_period_1;
#endif
	struct bb_tx_info tx_info_i;
	struct bb_ra_dbgreg ra_dbgreg_i;
	struct bb_dbg_cr_info bb_dbg_cr_i;
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_print_devider(struct bb_info *bb, u8 len, bool with_space, u64 comp);
#ifdef HALBB_TDMA_CR_SUPPORT
void halbb_tdma_cr_sel_io_en(struct bb_info *bb);
void halbb_tdma_cr_timer_init(struct bb_info *bb);
void halbb_tdma_cr_sel_main(struct bb_info *bb);
void halbb_tdma_cr_sel_init(struct bb_info *bb);
#endif

void halbb_dbg_comp_init(struct bb_info *bb);
void halbb_bb_dbg_port_clock_en(struct bb_info *bb, u8 enable);
u32 halbb_get_bb_dbg_port_idx(struct bb_info *bb);
void halbb_set_bb_dbg_port(struct bb_info *bb, u32 dbg_port);
void halbb_set_bb_dbg_port_ip(struct bb_info *bb, enum bb_dbg_port_ip_t ip);
void halbb_release_bb_dbg_port(struct bb_info *bb);
bool halbb_bb_dbg_port_racing(struct bb_info *bb, u8 curr_dbg_priority);
u32 halbb_get_bb_dbg_port_val(struct bb_info *bb);
u16 halbb_rx_utility(struct bb_info *bb, u16 avg_phy_rate, u8 rx_max_ss,
		     enum channel_width bw);
u16 halbb_rx_avg_phy_rate(struct bb_info *bb);
void halbb_get_tx_dbg_reg(struct bb_info *bb);
void halbb_basic_dbg_message(struct bb_info *bb);
void halbb_basic_profile_dbg(struct bb_info *bb, u32 *_used, char *output, u32 *_out_len);
void halbb_get_bb_para_pkg_ver(struct bb_info *bb, u32 *date, u32 *release_ver);
void halbb_dump_reg_dbg(struct bb_info *bb, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halbb_dd_dump_dbg(struct bb_info *bb, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halbb_cr_table_dump(struct bb_info *bb, u32 *cr_table, u32 cr_len);
void halbb_cr_hook_fake_init(struct bb_info *bb, u32 *str_table, u32 len);
void halbb_cr_hook_init_dump(struct bb_info *bb, u32 *str_table, u32 len);
void halbb_dump_bb_reg(struct bb_info *bb, u32 *_used, char *output,
		       u32 *_out_len, bool dump_2_buff,
		       enum bb_frc_phy_dump_reg frc_phy_dump);
void halbb_tx_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
void halbb_cmn_dbg(struct bb_info *bb, char input[][16], u32 *_used, char *output, u32 *_out_len);
void halbb_dbg_setting_init(struct bb_info *bb);
void halbb_cr_cfg_dbg_init(struct bb_info *bb);
#endif
