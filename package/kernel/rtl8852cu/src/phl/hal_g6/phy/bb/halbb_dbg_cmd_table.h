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
#ifndef _HALBB_DBG_CMD_TABLE_H_
#define _HALBB_DBG_CMD_TABLE_H_
/*@--------------------------[Define] ---------------------------------------*/


/*@--------------------------[Enum]------------------------------------------*/
enum HALBB_CMD_ID {
	HALBB_HELP,
	HALBB_DEMO,
	HALBB_REG_RW,
	HALBB_CMN_DBG,
	HALBB_RA,
	HALBB_PROFILE,
	HALBB_TRACE,
	HALBB_MP_DBG,
	HALBB_SUPPORT_ABILITY,
	HALBB_TX_PW,
	HALBB_IC_API,
	HALBB_SPUR_SUPP,
	HALBB_LA_MODE,
	HALBB_DUMP_REG,
	HALBB_AUTO_DBG,
	HALBB_DD_DBG,
	HALBB_SHOW_RXRATE,
	HALBB_NBI_EN,
	HALBB_CSI_MASK_EN,
	HALBB_DFS_DBG,
	HALBB_DIG,
	HALBB_NHM,
	HALBB_CLM,
	HALBB_IFS_CLM,
	HALBB_FAHM,
	HALBB_EDCCA_CLM,
	HALBB_EDCCA,
	HALBB_ENV_MNTR,
	HALBB_BB_INFO,
	HALBB_H2C,
	HALBB_STASISTICS,
	HALBB_PSD,
	HALBB_DBG_PORT,
	HALBB_CFO_TRK,
	HALBB_UL_TB,
	HALBB_ADAPTIVITY_DBG,
	HALBB_STA_INFO,
	HALBB_PAUSE_FUNC,
	HALBB_PER_TONE_EVM,
	HALBB_DYN_TXPWR,
	HALBB_PHY_STATUS,
	HALBB_DCC,
	HALBB_PMAC_TX,
	HALBB_FW_DBG,
	HALBB_CH_INFO,
	HALBB_RUA_TBL,
	HALBB_TD_CFG,
	HALBB_FD_CFG,
	HALBB_DBCC,
	HALBB_ANT_DIV,
	HALBB_DCR_DBG,
	HALBB_RX_GAIN_TABLE,
	HALBB_RX_OP1DB_TABLE,
	HALBB_HW_SETTING,
	HALBB_PATH_DIV,
	HALBB_DTP,
	HALBB_TX_INFO,
	HALBB_DYN_1R_CCA,
	HALBB_CMN_INFO,
	HALBB_SNIFFER_MODE,
	HALBB_DTR_DBG,
	HALBB_FW_OFLD,
	HALBB_MATH
};

/*@--------------------------[Structure]-------------------------------------*/
 
struct halbb_cmd_info {
	char name[16];
	u8 id;
};

static const struct halbb_cmd_info halbb_cmd_i[] = {
	{"-h", HALBB_HELP}, /*@do not move this element to other position*/
	{"demo", HALBB_DEMO}, /*@do not move this element to other position*/
	{"cr", HALBB_REG_RW},
	{"cmn", HALBB_CMN_DBG},
	{"ra", HALBB_RA},
	{"profile", HALBB_PROFILE},
	{"dbg", HALBB_TRACE},
	{"mp_dbg", HALBB_MP_DBG},
	{"ability", HALBB_SUPPORT_ABILITY},
	{"tx_pw", HALBB_TX_PW},
	{"ic_api", HALBB_IC_API},
	{"spur", HALBB_SPUR_SUPP},
	{"lamode", HALBB_LA_MODE},
	{"psd", HALBB_PSD},
	{"dumpreg", HALBB_DUMP_REG},
	//{"auto_dbg", HALBB_AUTO_DBG},
	{"dd_dbg", HALBB_DD_DBG},
	{"cfo_trk", HALBB_CFO_TRK},
	{"ul_tb", HALBB_UL_TB},
	{"rxrate", HALBB_SHOW_RXRATE},
	//{"nbi", HALBB_NBI_EN},
	//{"csi_mask", HALBB_CSI_MASK_EN},
	{"dfs", HALBB_DFS_DBG},
	{"dig", HALBB_DIG},
	{"nhm", HALBB_NHM},
	{"clm", HALBB_CLM},
	{"ifs_clm", HALBB_IFS_CLM},
	{"fahm", HALBB_FAHM},
	{"edcca_clm", HALBB_EDCCA_CLM},
	{"edcca", HALBB_EDCCA},	
	{"env_mntr", HALBB_ENV_MNTR},
	//{"bbinfo", HALBB_BB_INFO},
	//{"h2c", HALBB_H2C},
	{"stat", HALBB_STASISTICS},
	{"dbgport", HALBB_DBG_PORT},
	{"sta_info", HALBB_STA_INFO},
	{"pause", HALBB_PAUSE_FUNC},
	{"physts", HALBB_PHY_STATUS},
	{"pmac_tx", HALBB_PMAC_TX},
	{"fw_dbg", HALBB_FW_DBG},
	{"ch_info", HALBB_CH_INFO},
	{"td", HALBB_TD_CFG},
	{"fd", HALBB_FD_CFG},
	{"dbcc", HALBB_DBCC},
	{"rua", HALBB_RUA_TBL},
	{"ant_div", HALBB_ANT_DIV},
	{"dcr", HALBB_DCR_DBG},
	{"gain_table", HALBB_RX_GAIN_TABLE},
	{"dtr", HALBB_DTR_DBG},
	{"op1db_table", HALBB_RX_OP1DB_TABLE},
	{"hw_set", HALBB_HW_SETTING},
	{"path_div", HALBB_PATH_DIV},
	{"dtp", HALBB_DTP},
	{"txinfo", HALBB_TX_INFO},
	{"dyn_1r_cca", HALBB_DYN_1R_CCA},
	{"cmn_info", HALBB_CMN_INFO},
	{"snif", HALBB_SNIFFER_MODE},
	{"fw_ofld", HALBB_FW_OFLD},
	{"math", HALBB_MATH}
	};

/*@--------------------------[Prptotype]-------------------------------------*/

#endif
