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
#ifndef __INC_BTC_ACTION_H__
#define __INC_BTC_ACTION_H__

enum {
	BTC_PHY_0 = BIT(0),
	BTC_PHY_1 = BIT(1),
	BTC_PHY_ALL = BIT(0) | BIT(1),
};

enum btc_cx_state_map {
	BTC_WIDLE = 0,
	BTC_WBUSY_BNOSCAN,
	BTC_WBUSY_BSCAN,
	BTC_WSCAN_BNOSCAN,
	BTC_WSCAN_BSCAN,
	BTC_WLINKING,
	BTC_WIDLE_BSCAN
};

enum btc_ant_phase {
	BTC_ANT_WPOWERON = 0,
	BTC_ANT_WINIT,
	BTC_ANT_WONLY,
	BTC_ANT_WOFF,
	BTC_ANT_W2G,
	BTC_ANT_W5G,
	BTC_ANT_W25G,
	BTC_ANT_FREERUN,
	BTC_ANT_FDDTRAIN,
	BTC_ANT_WRFK,
	BTC_ANT_BRFK,
	BTC_ANT_MAX
};

enum {
	BTC_PLT_NONE = 0,
	BTC_PLT_LTE_RX = BIT(0),
	BTC_PLT_GNT_BT_TX = BIT(1),
	BTC_PLT_GNT_BT_RX = BIT(2),
	BTC_PLT_GNT_WL = BIT(3),
	BTC_PLT_BT = BIT(1) | BIT(2),
	BTC_PLT_ALL = 0xf
};

enum btc_cx_poicy_main_type {
	BTC_CXP_OFF = 0,
	BTC_CXP_OFFB,
	BTC_CXP_OFFE,
	BTC_CXP_FIX,
	BTC_CXP_PFIX,
	BTC_CXP_AUTO,
	BTC_CXP_PAUTO,
	BTC_CXP_AUTO2,
	BTC_CXP_PAUTO2,
	BTC_CXP_MANUAL,
	BTC_CXP_USERDEF0,
	BTC_CXP_MAIN_MAX
};

enum {
	BTC_BSLOT_A2DP_HID = 60,
	BTC_BSLOT_A2DP = 50,
	BTC_BSLOT_A2DP_2 = 40,
	BTC_BSLOT_INQ = 30,
	BTC_BSLOT_IDLE = 20,
};

enum btc_cx_poicy_type {
	/* TDMA off + pri: BT > WL */
	BTC_CXP_OFF_BT = (BTC_CXP_OFF << 8) | 0,

	/* TDMA off + pri: WL > BT */
	BTC_CXP_OFF_WL = (BTC_CXP_OFF << 8) | 1,

	/* TDMA off + pri: BT = WL */
	BTC_CXP_OFF_EQ0 = (BTC_CXP_OFF << 8) | 2,

	/* TDMA off + pri: BT = WL > BT_Lo */
	BTC_CXP_OFF_EQ1 = (BTC_CXP_OFF << 8) | 3,

	/* TDMA off + pri: WL = BT, BT_Rx > WL_Lo_Tx */
	BTC_CXP_OFF_EQ2 = (BTC_CXP_OFF << 8) | 4,

	/* TDMA off + pri: WL_Rx = BT, BT_HI > WL_Tx > BT_Lo */
	BTC_CXP_OFF_EQ3 = (BTC_CXP_OFF << 8) | 5,

	/* TDMA off + pri: BT_Hi > WL > BT_Lo */
	BTC_CXP_OFF_BWB0 = (BTC_CXP_OFF << 8) | 6,

	/* TDMA off + pri: BT_Hi_Tx = WL_Hi-Tx > BT_Hi_Rx, BT_Hi > WL > BT_Lo */
	BTC_CXP_OFF_BWB1 = (BTC_CXP_OFF << 8) | 7,

	/* TDMA off + pri: WL_Hi-Tx > BT, BT_Hi > other-WL > BT_Lo */
	BTC_CXP_OFF_BWB2 = (BTC_CXP_OFF << 8) | 8,

	/* TDMA off + pri: WL_Hi-Tx = BT */
	BTC_CXP_OFF_BWB3 = (BTC_CXP_OFF << 8) | 9,

	/* TDMA off+Bcn-Protect + pri: WL_Hi-Tx > BT_Hi_Rx, BT_Hi > WL > BT_Lo*/
	BTC_CXP_OFFB_BWB0 = (BTC_CXP_OFFB << 8) | 0,

	/* TDMA off + Ext-Ctrl + pri: default */
	BTC_CXP_OFFE_2GBWISOB = (BTC_CXP_OFFE << 8) | 0,

	/* TDMA off + Ext-Ctrl + pri: E2G-slot block all BT */
	BTC_CXP_OFFE_2GISOB = (BTC_CXP_OFFE << 8) | 1,

	/* TDMA off + Ext-Ctrl + pri: E2G-slot WL > BT */
	BTC_CXP_OFFE_2GBWMIXB = (BTC_CXP_OFFE << 8) | 2,

	/* TDMA off + Ext-Ctrl + pri: E2G/EBT-slot WL > BT */
	BTC_CXP_OFFE_WL = (BTC_CXP_OFFE << 8) | 3,

	/* TDMA off + Ext-Ctrl + pri: default */
	BTC_CXP_OFFE_2GBWMIXB2 = (BTC_CXP_OFFE << 8) | 4,

	/* TDMA Fix slot-0: W1:B1 = 30:30 */
	BTC_CXP_FIX_TD3030 = (BTC_CXP_FIX << 8) | 0,

	/* TDMA Fix slot-1: W1:B1 = 50:50 */
	BTC_CXP_FIX_TD5050 = (BTC_CXP_FIX << 8) | 1,

	/* TDMA Fix slot-2: W1:B1 = 20:30 */
	BTC_CXP_FIX_TD2030 = (BTC_CXP_FIX << 8) | 2,

	/* TDMA Fix slot-3: W1:B1 = 40:10 */
	BTC_CXP_FIX_TD4010 = (BTC_CXP_FIX << 8) | 3,

	/* TDMA Fix slot-4: W1:B1 = 70:10 */
	BTC_CXP_FIX_TD7010 = (BTC_CXP_FIX << 8) | 4,

	/* TDMA Fix slot-5: W1:B1 = 20:60 */
	BTC_CXP_FIX_TD2060 = (BTC_CXP_FIX << 8) | 5,

	/* TDMA Fix slot-6: W1:B1 = 30:60 */
	BTC_CXP_FIX_TD3060 = (BTC_CXP_FIX << 8) | 6,

	/* TDMA Fix slot-7: W1:B1 = 20:80 */
	BTC_CXP_FIX_TD2080 = (BTC_CXP_FIX << 8) | 7,

	/* TDMA Fix slot-8: W1:B1 = user-define */
	BTC_CXP_FIX_TDW1B1 = (BTC_CXP_FIX << 8) | 8,

	/* TDMA Fix slot-9: W1:B1 = 40:10 */
	BTC_CXP_FIX_TD4010ISO = (BTC_CXP_FIX << 8) | 9,

	/* TDMA Fix slot-10: W1:B1 = 40:10 */
	BTC_CXP_FIX_TD4010ISO_DL = (BTC_CXP_FIX << 8) | 10,

	/* TDMA Fix slot-11: W1:B1 = 40:10 */
	BTC_CXP_FIX_TD4010ISO_UL = (BTC_CXP_FIX << 8) | 11,

	/* PS-TDMA Fix slot-0: W1:B1 = 30:30 */
	BTC_CXP_PFIX_TD3030 = (BTC_CXP_PFIX << 8) | 0,

	/* PS-TDMA Fix slot-1: W1:B1 = 50:50 */
	BTC_CXP_PFIX_TD5050 = (BTC_CXP_PFIX << 8) | 1,

	/* PS-TDMA Fix slot-2: W1:B1 = 20:30 */
	BTC_CXP_PFIX_TD2030 = (BTC_CXP_PFIX << 8) | 2,

	/* PS-TDMA Fix slot-3: W1:B1 = 20:60 */
	BTC_CXP_PFIX_TD2060 = (BTC_CXP_PFIX << 8) | 3,

	/* PS-TDMA Fix slot-4: W1:B1 = 30:70 */
	BTC_CXP_PFIX_TD3070 = (BTC_CXP_PFIX << 8) | 4,

	/* PS-TDMA Fix slot-5: W1:B1 = 20:80 */
	BTC_CXP_PFIX_TD2080 = (BTC_CXP_PFIX << 8) | 5,

	/* PS-TDMA Fix slot-6: W1:B1 = user-define */
	BTC_CXP_PFIX_TDW1B1 = (BTC_CXP_PFIX << 8) | 6,

	/* TDMA Auto slot-0: W1:B1 = 50:BTC_B1_MAX */
	BTC_CXP_AUTO_TD50B1 = (BTC_CXP_AUTO << 8) | 0,

	/* TDMA Auto slot-1: W1:B1 = 60:BTC_B1_MAX */
	BTC_CXP_AUTO_TD60B1 = (BTC_CXP_AUTO << 8) | 1,

	/* TDMA Auto slot-2: W1:B1 = 20:BTC_B1_MAX */
	BTC_CXP_AUTO_TD20B1 = (BTC_CXP_AUTO << 8) | 2,

	/* TDMA Auto slot-3: W1:B1 = user-define */
	BTC_CXP_AUTO_TDW1B1 = (BTC_CXP_AUTO << 8) | 3,

	/* PS-TDMA Auto slot-0: W1:B1 = 50:BTC_B1_MAX */
	BTC_CXP_PAUTO_TD50B1 = (BTC_CXP_PAUTO << 8) | 0,

	/* PS-TDMA Auto slot-1: W1:B1 = 60:BTC_B1_MAX */
	BTC_CXP_PAUTO_TD60B1 = (BTC_CXP_PAUTO << 8) | 1,

	/* PS-TDMA Auto slot-2: W1:B1 = 20:BTC_B1_MAX */
	BTC_CXP_PAUTO_TD20B1 = (BTC_CXP_PAUTO << 8) | 2,

	/* PS-TDMA Auto slot-3: W1:B1 = user-define */
	BTC_CXP_PAUTO_TDW1B1 = (BTC_CXP_PAUTO << 8) | 3,

	/* PS-TDMA Auto slot-5: W1:B1 = user-define for FDD-Train default */
	BTC_CXP_PAUTO_FDDT1 = (BTC_CXP_PAUTO << 8) | 4,

	/* TDMA Auto slot2-0: W1:B4 = 30:50 */
	BTC_CXP_AUTO2_TD3050 = (BTC_CXP_AUTO2 << 8) | 0,

	/* TDMA Auto slot2-1: W1:B4 = 30:70 */
	BTC_CXP_AUTO2_TD3070 = (BTC_CXP_AUTO2 << 8) | 1,

	/* TDMA Auto slot2-2: W1:B4 = 50:50 */
	BTC_CXP_AUTO2_TD5050 = (BTC_CXP_AUTO2 << 8) | 2,

	/* TDMA Auto slot2-3: W1:B4 = 60:60 */
	BTC_CXP_AUTO2_TD6060 = (BTC_CXP_AUTO2 << 8) | 3,

	/* TDMA Auto slot2-4: W1:B4 = 20:80 */
	BTC_CXP_AUTO2_TD2080 = (BTC_CXP_AUTO2 << 8) | 4,

	/* TDMA Auto slot2-5: W1:B4 = user-define */
	BTC_CXP_AUTO2_TDW1B4 = (BTC_CXP_AUTO2 << 8) | 5,

	/* PS-TDMA Auto slot2-0: W1:B4 = 30:50 */
	BTC_CXP_PAUTO2_TD3050 = (BTC_CXP_PAUTO2 << 8) | 0,

	/* PS-TDMA Auto slot2-1: W1:B4 = 30:70 */
	BTC_CXP_PAUTO2_TD3070 = (BTC_CXP_PAUTO2 << 8) | 1,

	/* PS-TDMA Auto slot2-2: W1:B4 = 50:50 */
	BTC_CXP_PAUTO2_TD5050 = (BTC_CXP_PAUTO2 << 8) | 2,

	/* PS-TDMA Auto slot2-3: W1:B4 = 60:60 */
	BTC_CXP_PAUTO2_TD6060 = (BTC_CXP_PAUTO2 << 8) | 3,

	/* PS-TDMA Auto slot2-4: W1:B4 = 20:80 */
	BTC_CXP_PAUTO2_TD2080 = (BTC_CXP_PAUTO2 << 8) | 4,

	/* PS-TDMA Auto slot2-5: W1:B4 = user-define */
	BTC_CXP_PAUTO2_TDW1B4 = (BTC_CXP_PAUTO2 << 8) | 5,

	BTC_CXP_MAX = 0xffff
};

void _action_wl_25g_mcc(struct btc_t *btc);
void _action_wl_2g_ap(struct btc_t *btc);
void _action_wl_2g_go(struct btc_t *btc);
void _action_wl_2g_gc(struct btc_t *btc);
void _action_wl_2g_nan(struct btc_t *btc);
void _action_wl_2g_scc(struct btc_t *btc);
void _action_wl_2g_sta(struct btc_t *btc);
void _action_wl_scan(struct btc_t *btc);
void _action_wl_2g_mcc(struct btc_t *btc);
void _action_wl_5g(struct btc_t *btc);
void _action_wl_other(struct btc_t *btc);
void _action_wl_idle(struct btc_t *btc);
void _action_wl_nc(struct btc_t *btc);
void _action_wl_rfk(struct btc_t *btc);
void _action_wl_init(struct btc_t *btc);
void _action_wl_off(struct btc_t *btc);
void _action_freerun(struct btc_t *btc);
void _action_wl_only(struct btc_t *btc);
void _action_bt_off(struct btc_t *btc);
void _action_bt_rfk(struct btc_t *btc);
void _action_bt_whql(struct btc_t *btc);
void _action_common(struct btc_t *btc);
//extern const u32 cxtbl[];

void _set_bt_ignore_wl_act(struct btc_t *btc, u8 enable);
void _set_wl_tx_power(struct btc_t *btc, u32 level);
void _set_wl_rx_gain(struct btc_t *btc, u32 level);
void _set_bt_tx_power(struct btc_t *btc, u32 level);
void _set_bt_rx_gain(struct btc_t *btc, u32 level);
void _set_gnt(struct btc_t *btc, u8 phy_map, u8 wl_state, u8 bt_state);


extern struct fbtc_tdma t_def[];
extern struct fbtc_slot s_def[];

extern struct btc_fddt_time_ctrl ft_tctrl_def;
extern struct btc_fddt_break_check ft_bchk_def;
extern struct btc_fddt_fail_check ft_fchk_def;
extern struct btc_fddt_cell cell_ul_def[5][5];
extern struct btc_fddt_cell cell_dl_def[5][5];

#endif
