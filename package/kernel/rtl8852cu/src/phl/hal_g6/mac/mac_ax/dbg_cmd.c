/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/
#include "dbg_cmd.h"

const char *MallocIDName[eMallocMAX] = {
	"OS",
	"Common",
	"Task",
	"AXIDMA",
	"SPIC",
	"Timer",
	"UART",
	"H2C",
	"C2H",
	"DBG",
	"Role",
	"PS",
	"WoWLAN",
	"Sec",
	"Ofld",
	"TX",
	"RX",
	"Report",
	"PHYDM",
	"RF",
	"BTCoex",
	"SCSI",
	"Free"
};

const char *MallocTypeName[eMallocTypeMAX] = {
	"N/A",
	"Fixed",
	"Dynamic"
};

const char *ISRName[ISRStatistic_MAX] = {
	/* First layer */
	// WDTISRROM8852A
	"WDT",
	// CMACISRROM8852A
	"CMAC0",
	"CMAC1",
	"CMAC2",
	"CMAC3",
	"CMAC4",
	"CMAC5",
	"CMAC6",
	"CMAC7",
	"CMAC8",
	"CMAC9",
	"CMAC10",
	// DMACISRROM8852A
	"DMAC0",
	"DMAC1",
	"DMAC2",
	// SysISRROM8852A
	"SYS0",
	// DataHDLISRROM8852A
	"IPSec",
	"AXIDMA",
	"IDDMA",
	// PlatISRROM8852A
	"MACErr",
	"UART",
	"RXI300",
	/* Second layer */
	"DMAC0_GT0",
	"DMAC0_GT1",
	"DMAC0_GT2_3",
	"DMAC0_H2C",
	"SYS0_HALT_H2C",
	"SYS0_GPIO",
	"MACErr_DMAC",
	"MACErr_CMAC0",
	"MACErr_CMAC1"
};

// gerrit test 5
static const struct mac_hal_cmd_info mac_hal_cmd_i[] = {
	{"-h", MAC_HAL_HELP, cmd_mac_help},
	{"dd_dbg", MAC_MAC_DD_DBG, cmd_mac_dbg_dump},/*@do not move this element to other position*/
	{"reg_dump", MAC_MAC_REG_DUMP, cmd_mac_reg_dump},
	{"error_dump", MAC_MAC_ERROR_DUMP, cmd_mac_error_dump},
	{"dbg_tx", MAC_MAC_DBG_TX, cmd_mac_dbg_tx_dump},
	{"dbg_rx", MAC_MAC_DBG_RX, cmd_mac_dbg_rx_dump},
	{"dbg_cmac", MAC_MAC_DBG_CMAC, cmd_mac_dbg_rx_dump},
	{"dbg_dmac", MAC_MAC_DBG_DMAC, cmd_mac_dbg_rx_dump},
	{"dbg_bcn", MAC_MAC_DBG_BCN, cmd_mac_dbg_bcn},
	{"ser_cnt", MAC_MAC_SER_CNT_DUMP, cmd_mac_ser_cnt_dump},
	{"get_ser_log_lvl", MAC_MAC_GET_SER_LVL, cmd_mac_ser_level_dump},
	{"fw_dbg", MAC_MAC_FW_DBG, cmd_mac_fw_dump},
	{"help", MAC_HAL_HELP, cmd_mac_help},
	{"fw_log", MAC_MAC_FW_LOG, cmd_mac_fw_log_cfg},
	{"fw_curtcb", MAC_MAC_FW_CURTCB, cmd_mac_fw_curtcb},
	{"fw_info", MAC_MAC_FW_INFO, cmd_mac_fw_status_parser},
	{"dl_sym", MAC_MAC_DL_SYM, cmd_mac_dl_sym},
	{"qc_start", MAC_MAC_QC_START, cmd_mac_qc_start},
	{"qc_end", MAC_MAC_QC_END, cmd_mac_qc_end},
	{"req_pwr_st", MAC_MAC_REQ_PWR_ST, cmd_mac_req_pwr_st},
	{"req_pwr_lvl", MAC_MAC_REQ_PWR_LVL, cmd_mac_req_pwr_lvl},
	{"chsw", MAC_MAC_CHSW, cmd_mac_fw_chsw},
	{"chsw_ret", MAC_MAC_CHSW_RET, cmd_mac_fw_chsw_ret},
	{"bcn_stats", MAC_MAC_QC_START, cmd_mac_bcn_stats}
	/*@do not move this element to other position*/
};

static const struct mac_hal_cmd_info mac_fw_status_cmd_i[] = {
	{"task", FW_STATUS_TASKINFO},
	{"flash", FW_STATUS_FLASHINFO},
	{"heap", FW_STATUS_HEAPINFO},
	{"mem_fast", FW_STATUS_MEMINFO_FAST},
	{"mem_slow", FW_STATUS_MEMINFO_SLOW},
	{"ps", FW_STATUS_PSINFO},
	{"h2c_c2h", FW_STATUS_H2C_C2HINFO},
	{"isr", FW_STATUS_ISRINFO},
	{"chsw", FW_STATUS_CHSW_TIMING}
};

static const char * const type_names[] = {
	"level",
	"output",
	"comp",
	"comp_ext",
	NULL
};

static struct fw_status_proc_class fw_status_proc_sys[] = {
	{FW_STATUS_TASKINFO, fw_status_taskinfo_handler},
	{FW_STATUS_FLASHINFO, fw_status_flashinfo_handler},
	{FW_STATUS_HEAPINFO, fw_status_heapinfo_handler},
	{FW_STATUS_MEMINFO_FAST, fw_status_meminfo_fast_handler},
	{FW_STATUS_MEMINFO_SLOW, fw_status_meminfo_slow_handler},
	{FW_STATUS_PSINFO, fw_status_psinfo_handler},
	{FW_STATUS_H2C_C2HINFO, fw_status_h2c_c2hinfo_handler},
	{FW_STATUS_ISRINFO, fw_status_isrinfo_handler},
	{FW_STATUS_CHSW_TIMING, fw_status_chsw_handler},
	{FW_STATUS_MAX, NULL},
};

u8 qc_cmd_id;
u32 fheap_start, sheap_start;

u32 cmd_mac_help(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		 char *output, u32 out_len, u32 *used)
{
	u32 hal_cmd_ary_size = sizeof(mac_hal_cmd_i) / sizeof(struct mac_hal_cmd_info);
	u32 i;

	//PLTFM_MSG_TRACE("HAL cmd ==>\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used, "HAL cmd ==>\n");
	for (i = 0; i < hal_cmd_ary_size - 1; i++) {
		//PLTFM_MSG_TRACE("	 %-5d: %s\n",
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used, "	 %-5d: %s\n",
			    i, mac_hal_cmd_i[i + 1].name);
	}
	return MACSUCCESS;
}

u32 cmd_mac_dbg_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used)
{
	//struct hal_info_t *hal = hal_com->hal_priv;
	struct mac_ax_adapter *mac = adapter;
	//struct mac_ax_ops *ops = mac->ops;
	struct mac_ax_dbgpkg ss_dbg = {0};
	struct mac_ax_dbgpkg_en dbg_msk = {0};

	ss_dbg.ss_dbg_0 = 0;
	ss_dbg.ss_dbg_1 = 0;
	dbg_msk.ss_dbg = 1;
	dbg_msk.dle_dbg = 1;
	dbg_msk.dmac_dbg = 1;
	dbg_msk.cmac_dbg = 1;
	dbg_msk.mac_dbg_port = 1;

	mac_dbg_status_dump(mac, &ss_dbg, &dbg_msk);

	//PLTFM_MSG_TRACE("rtw_hal_mac_dbg_dump(): ss_dbg.ss_dbg_0 = 0x%08X,
	//		      ss_dbg.ss_dbg_1 = 0x%08X\n",
	//		      ss_dbg.ss_dbg_0, ss_dbg.ss_dbg_1);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "rtw_hal_mac_dbg_dump(): ss_dbg.ss_dbg_0 = 0x%08X,ss_dbg.ss_dbg_1 = 0x%08X\n",
		    ss_dbg.ss_dbg_0, ss_dbg.ss_dbg_1);

	return MACSUCCESS;
}

u32 cmd_mac_reg_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used)
{
	u32 ret = MACSUCCESS, val = 0;

	// input argument start from input[1], input[0] is mac_hal_cmd_info.name
	if (input_num < 2) {
		//PLTFM_MSG_TRACE("invalid argument\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "invalid argument\n");
		return MACFWSTATUSFAIL;
	}
	//PLTFM_SSCANF(input[1], "%d", &val);
	val = PLTFM_STRTOUL(input[1], 10);

	//PLTFM_MSG_TRACE("%s: sel:%d\n", __func__, val);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "%s: sel:%d\n", __func__, val);
	ret = mac_reg_dump(adapter, (enum mac_ax_reg_sel)val);
	return ret;
}

u32 cmd_mac_error_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		       char *output, u32 out_len, u32 *used)
{
	u32 ret = MACSUCCESS;

	return ret;
}

void cmd_mac_get_version(struct mac_ax_adapter *adapter, char *ver_str, u16 len)
{
	PLTFM_SNPRINTF(ver_str, len, "V%u.%u.%u.%u",
		       MAC_AX_MAJOR_VER, MAC_AX_PROTOTYPE_VER,
		       MAC_AX_SUB_VER, MAC_AX_SUB_INDEX);
}

void cmd_mac_get_fw_ver(struct mac_ax_adapter *adapter, char *ver_str, u16 len)
{
	PLTFM_SNPRINTF(ver_str, len, "V%u.%u.%u.%u",
		       adapter->fw_info.major_ver, adapter->fw_info.minor_ver,
		       adapter->fw_info.sub_ver, adapter->fw_info.sub_idx);
}

u32 cmd_mac_fw_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used)
{
	//struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 i;
	char mac_ver[20], fw_ver[20];

	cmd_mac_get_version(adapter, mac_ver, sizeof(mac_ver));
	//PLTFM_MSG_TRACE("HALMAC version %s\n", mac_ver);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "HALMAC version %s\n", mac_ver);

	cmd_mac_get_fw_ver(adapter, fw_ver, sizeof(fw_ver));
	//PLTFM_MSG_TRACE("FW version %s\n", fw_ver);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "FW version %s\n", fw_ver);

	/* dump dbg reg */
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x01f0[31:0] = 0x%08x\n", MAC_REG_R32(0x01f0));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x01f4[31:0] = 0x%08x\n", MAC_REG_R32(0x01f4));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x01f8[31:0] = 0x%08x\n", MAC_REG_R32(0x01f8));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x016c[31:0] = 0x%08x\n", MAC_REG_R32(0x016c));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x01fc[31:0] = 0x%08x\n", MAC_REG_R32(0x01fc));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "0x8424[31:0] = 0x%08x\n", MAC_REG_R32(0x8424));

	/* dump fw pc */
	MAC_REG_W32(0x58, 0xf200f2);
	MAC_REG_W8(0xf6, 0x1);
	for (i = 0; i < 15; i++) {
		//PLTFM_MSG_TRACE("0x00c0[31:0] = 0x%08x\n", MAC_REG_R32(0xc0));
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "0x00c0[31:0] = 0x%08x\n", MAC_REG_R32(0xc0));
		PLTFM_DELAY_MS(1);
	}
	return MACSUCCESS;
}

static void cmd_mac_fw_log_set(struct mac_ax_fw_log *fl_cfg, u8 type, u32 value)
{
	switch (type) {
	case FWDGB_CFG_TYPE_LEVEL:
		fl_cfg->level = value;
		break;

	case FWDGB_CFG_TYPE_OUTPUT:
		fl_cfg->output |= value;
		break;

	case FWDGB_CFG_TYPE_COMP:
		fl_cfg->comp |= value;
		break;

	case FWDGB_CFG_TYPE_COMP_EXT:
		fl_cfg->comp_ext |= value;
		break;

	default:
		break;
	}
}

static void cmd_mac_fw_log_clr(struct mac_ax_fw_log *fl_cfg, u8 type, u32 value)
{
	switch (type) {
	case FWDGB_CFG_TYPE_LEVEL:
		break;
	case FWDGB_CFG_TYPE_OUTPUT:
		fl_cfg->output &= (~value);
		break;
	case FWDGB_CFG_TYPE_COMP:
		fl_cfg->comp &= (~value);
		break;
	case FWDGB_CFG_TYPE_COMP_EXT:
		fl_cfg->comp_ext &= (~value);
		break;
	default:
		break;
	}
}

u32 cmd_mac_dbg_tx_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
			char *output, u32 out_len, u32 *used)
{
	u32 ret;

	PLTFM_MSG_TRACE("%s\n", __func__);
	ret = mac_tx_flow_dbg(adapter);

	return ret;
}

u32 cmd_mac_dbg_rx_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
			char *output, u32 out_len, u32 *used)
{
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	u32 ret;

	PLTFM_MSG_TRACE("%s\n", __func__);

	ret = mops->event_notify(adapter, MSG_EVT_DBG_RX_DUMP, HW_BAND_0);

	return ret;
}

u32 cmd_mac_dbg_dmac(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used)
{
	//struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	PLTFM_MSG_TRACE("%s\n", __func__);

	if (adapter->sm.dmac_func == MAC_AX_FUNC_ON) {
		//ret = p_ops->dmac_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("dmac dbg dump\n");
	}

	return ret;
}

u32 cmd_mac_dbg_cmac(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used)
{
	//struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	PLTFM_MSG_TRACE("%s\n", __func__);

	if (adapter->sm.cmac0_func == MAC_AX_FUNC_ON) {
		//ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n", MAC_AX_BAND_0, ret);
	}

	if (adapter->sm.cmac1_func == MAC_AX_FUNC_ON) {
		//ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n", MAC_AX_BAND_1, ret);
	}

	return ret;
}

u32 cmd_mac_dbg_bcn(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 i, band, en_port_num = 0;
	u32 reg, address;
	char mac_ver[20], fw_ver[20];

	cmd_mac_get_version(adapter, mac_ver, sizeof(mac_ver));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "HALMAC version %s\n", mac_ver);
	cmd_mac_get_fw_ver(adapter, fw_ver, sizeof(fw_ver));
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "FW version %s\n", fw_ver);

	if (input_num == 2) {
		band = PLTFM_STRCMP(input[1], "1") == 0 ? 1 : 0;
		PLTFM_MSG_ERR("Select band: %d\n", band);

		/* TRX Setting */
		/* Check MACID Sleep */
		address = (band == MAC_AX_BAND_0) ?
			  R_AX_MACID_SLEEP_0 : R_AX_MACID_SLEEP_0_C1;
		reg = MAC_REG_R32(address);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check MACID Sleep:\t0x%08x[31:0] = 0x%08x\n",
			    address, reg);
		if ((reg & 0x1) != 0)
			PLTFM_MSG_ERR("[ERR] 0x%08x[0] should not be 1\n", address);

		/* Check MACID Pause */
		reg = MAC_REG_R32(R_AX_SS_MACID_PAUSE_0);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check MACID Pause:\t0x%08x[31:0] = 0x%08x\n",
			    R_AX_SS_MACID_PAUSE_0, reg);
		if ((reg & 0x1) != 0)
			PLTFM_MSG_ERR("[ERR] 0x%08x[0] should not be 1\n", R_AX_SS_MACID_PAUSE_0);

		/* Check TXEN */
		address = (band == MAC_AX_BAND_0) ? R_AX_CTN_TXEN : R_AX_CTN_TXEN_C1;
		reg = MAC_REG_R32(address);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check Contention EN Flag:\t0x%08x[31:0] = 0x%08x\n",
			    address, reg);
		if (reg == 0x3fff)
			PLTFM_MSG_ERR("[ERR] 0x%08x[0] should be 0x3fff\n", address);

		/* Check BCN Queue */
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check PTCL TXON Status:\n");
		for (i = 0; i < 5; i++) {
			address = (band == MAC_AX_BAND_0) ?
			R_AX_PTCL_TX_CTN_SEL : R_AX_PTCL_TX_CTN_SEL_C1;
			reg = MAC_REG_R32(address);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "\t0x%08x[31:0] = 0x%08x\n", address, reg);
			if (reg == 0x12)
				break;
			PLTFM_DELAY_MS(100);
		}
		if ((reg & 0x1f) != 0xc)
			PLTFM_MSG_ERR("[ERR] Beacon queue should not be empty\n");

		address = (band == MAC_AX_BAND_0) ?
			  R_AX_MBSSID_CTRL : R_AX_MBSSID_CTRL_C1;
		reg = MAC_REG_R32(address);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check MBSSID EN:\t0x%08x[31:0] = 0x%08x\n", address, reg);
		PLTFM_MSG_ERR("Number of enable MBSSID: %d\n", (reg >> 16) & 0xf);

		address = (band == MAC_AX_BAND_0) ? R_AX_MBSSID_DROP_0 : R_AX_MBSSID_DROP_0_C1;
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "Check MBSSID Drop:\t0x%08x[31:0] = 0x%08x\n",
			    address, MAC_REG_R32(address));

		/* Check BCN Setting */
		for (i = 0; i < MAC_AX_PORT_NUM; i++) {
			/* Check BCN ISR Flag */
			address = ((band == MAC_AX_BAND_0) ? R_AX_FWC01ISR : R_AX_FWC01ISR_C1)
				  + i * FWCISR_OFFSET;
			reg = MAC_REG_R32(address);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Check Port%d Beacon ISR Flag:\t0x%08x[31:0] = 0x%08x\n",
				    i, address, reg);
			if (reg != 0x0)
				PLTFM_MSG_ERR("[ERR] Please clear port%d beacon ISR flag "\
					      "and read again\n", i);

			/* Check TXBCNEN */
			address = ((band == MAC_AX_BAND_0) ?
				   R_AX_PORT_CFG_P0 : R_AX_PORT_CFG_P0_C1)
				   + i * PORT_CFG_OFFSET;
			reg = MAC_REG_R32(address);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Check Port%d Beacon Setting:\t0x%08x[31:0] = 0x%08x\n",
				    i, address, reg);
			en_port_num += (((reg >> 12) & 0x1) == 0) ? 0 : 1;

			/* Check BCN Space */
			address = ((band == MAC_AX_BAND_0) ?
				   R_AX_BCN_SPACE_CFG_P0 : R_AX_BCN_SPACE_CFG_P0_C1)
				   + i * PORT_CFG_OFFSET;
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Check Port%d Beacon Space:\t0x%08x[31:0] = 0x%08x\n",
				    i, address, MAC_REG_R32(address));

			/* Check BCN Error Flag */
			address = ((band == MAC_AX_BAND_0) ?
				   R_AX_BCN_ERR_FLAG_P0 : R_AX_BCN_ERR_FLAG_P0_C1)
				   + i * PORT_CFG_OFFSET;
			reg = MAC_REG_R32(address);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Check Port%d Beacon Error Flag:\t0x%08x[31:0] = 0x%08x\n",
				    i, address, reg);
			if ((reg & 0xffff) != 0x0000)
				PLTFM_MSG_ERR("[ERR] 0x%08x[15:0] should be all 0\n", address);

			/* Check BCN Error Counter */
			address = ((band == MAC_AX_BAND_0) ?
				   R_AX_BCN_ERR_CNT_P0 : R_AX_BCN_ERR_CNT_P0_C1)
				   + i * PORT_CFG_OFFSET;
			reg = MAC_REG_R32(address);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Check Port%d Beacon Error Counter:\t0x%08x[31:0] = 0x%08x\n",
				    i, address, reg);
			if (reg != 0x0)
				PLTFM_MSG_ERR("[ERR] 0x%08x[31:0] should be all 0\n", address);
		}
		PLTFM_MSG_ERR("Number of enable port: %d\n", en_port_num);
	} else {
		PLTFM_MSG_ERR("Command example: \"cmd mac dbg_bcn <band number>\"\n");
	}
	return MACSUCCESS;
}

u32 cmd_mac_ser_cnt_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
			 char *output, u32 out_len, u32 *used)
{
	u32 ret;

	PLTFM_MSG_TRACE("%s\n", __func__);
	ret = mac_dump_ser_cnt(adapter, (struct mac_ser_status *)output);

	return MACSUCCESS;
}

u32 cmd_mac_ser_level_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV],
			   u32 input_num, char *output, u32 out_len, u32 *used)
{
	PLTFM_MSG_TRACE("%s\n", __func__);

	PLTFM_MSG_ALWAYS("dbgprt_dump: %d\n", adapter->ser_info.dbg_lvl.dbgprt_dump);
	PLTFM_MSG_ALWAYS("share_mem_dump: %d\n", adapter->ser_info.dbg_lvl.share_mem_dump);
	PLTFM_MSG_ALWAYS("cmac_dbg_dump: %d\n", adapter->ser_info.dbg_lvl.cmac_dbg_dump);
	PLTFM_MSG_ALWAYS("dmac_dbg_dump: %d\n", adapter->ser_info.dbg_lvl.dmac_dbg_dump);
	PLTFM_MSG_ALWAYS("txflow_dbg_dump: %d\n", adapter->ser_info.dbg_lvl.txflow_dbg_dump);
	PLTFM_MSG_ALWAYS("mac_reg_dump: %d\n", adapter->ser_info.dbg_lvl.mac_reg_dump);

	return MACSUCCESS;
}

u32 cmd_mac_fw_log_cfg_set(struct mac_ax_adapter *adapter,
			   struct mac_ax_fw_log *log_cfg, char *output, u32 out_len, u32 *used)
{
	//struct hal_info_t *hal = hal_com->hal_priv;
	struct mac_ax_adapter *mac = adapter;
	//struct mac_ax_fw_log log_cfg = {0};
	u32 status;

	if (!mac)
		return MACFWSTATUSFAIL;

	//log_cfg.level = fl_cfg->level;
	//log_cfg.output = fl_cfg->output;
	//log_cfg.comp = fl_cfg->comp;
	//log_cfg.comp_ext = fl_cfg->comp_ext;
	/*
	 *PLTFM_MSG_TRACE("Configuring firmware log level %d, output 0x%08x, "
	 *	  "comp 0x%08x, comp ext 0x%08x.\n",
	 *		log_cfg->level,
	 *		log_cfg->output,
	 *		log_cfg->comp,
	 *		log_cfg->comp_ext);
	 */
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Config firmware log level %d,output 0x%08x,comp 0x%08x,comp ext 0x%08x.\n",
		    log_cfg->level,
		    log_cfg->output,
		    log_cfg->comp,
		    log_cfg->comp_ext);
	if (log_cfg->output & MAC_AX_FL_LV_UART)	{
		//PLTFM_MSG_TRACE("%s: Enabling UART...\n", __func__);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "%s: Enabling UART...\n", __func__);
		mac->ops->pinmux_set_func(mac, MAC_AX_GPIO_UART_TX_GPIO5);
		mac->ops->sel_uart_tx_pin(mac, MAC_AX_UART_TX_GPIO5);
		mac->ops->pinmux_set_func(mac, MAC_AX_GPIO_UART_RX_GPIO6);
		mac->ops->sel_uart_rx_pin(mac, MAC_AX_UART_RX_GPIO6);
	}

	status = mac->ops->fw_log_cfg(mac, log_cfg);
	if (status != MACSUCCESS) {
		//PLTFM_MSG_TRACE("%s fault, status = %d.\n", __func__, status);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "%s fault, status = %d.\n", __func__, status);
		//return status;
	}
	return status;
}

u32 cmd_mac_fw_log_cfg(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		       char *output, u32 out_len, u32 *used)
{
	//struct rtw_hal_com_t *hal_com = (struct rtw_hal_com_t *)adapter->drv_adapter;
	//struct rtw_hal_fw_log_cfg *fl_cfg = &hal_com->fw_log_cfg;
	/* TYPE VALUE(HEX) 1(SET)|2(CLEAR) */
	struct mac_ax_fw_log *plog_cfg = &adapter->log_cfg;
	u8 type = 0;
	u32 op = FWDGB_CFG_OP_INFO, value = 0;

	if (input_num == 4) {
		//PLTFM_MSG_TRACE("%s,%s,%s\n", input[1], input[2], input[3]);
		while (type_names[type]) {
			if (PLTFM_STRCMP(input[1], type_names[type]) == 0)
				break;
			type++;
		}
		if (!type_names[type]) {
			//PLTFM_MSG_TRACE("Invalid type \"%s\"\n", input[1]);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "Invalid type \"%s\"\n", input[1]);
			return MACFWSTATUSFAIL;
		}
		//PLTFM_SSCANF(input[2], "%x", &value);
		value = PLTFM_STRTOUL(input[2], 16);
		//PLTFM_SSCANF(input[3], "%d", &op);
		op = PLTFM_STRTOUL(input[3], 10);
		//PLTFM_MSG_TRACE("value = 0x%x, op = %d\n", value, op);
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "value = 0x%x, op = %d\n", value, op);
		if (op == 1)
			op = FWDGB_CFG_OP_SET;
		else if (op == 2)
			op = FWDGB_CFG_OP_CLR;
		else
			op = FWDGB_CFG_OP_INFO;
	} else {
		//PLTFM_MSG_TRACE("invalid argument\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "invalid argument\n");
		return MACFWSTATUSFAIL;
	}

	switch (op) {
	case FWDGB_CFG_OP_SET:
		cmd_mac_fw_log_set(plog_cfg, type, value);
		break;
	case FWDGB_CFG_OP_CLR:
		cmd_mac_fw_log_clr(plog_cfg, type, value);
		break;
	case FWDGB_CFG_OP_INFO:
	//default:
		//_hal_fw_log_info(&log_cfg);
		//PLTFM_MSG_TRACE("fw_log invalid op\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "fw_log invalid op\n");
		return MACFWSTATUSFAIL;
	}

	return cmd_mac_fw_log_cfg_set(adapter, plog_cfg, output, out_len, used);
}

u32 cmd_mac_fw_curtcb(struct mac_ax_adapter *adapter,
		      char input[][MAC_MAX_ARGV], u32 input_num,
		      char *output, u32 out_len, u32 *used)
{
	//struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 curtcb, index, val32;
	char task_name[FW_MAX_TASK_NAME_LEN];

	curtcb = mac_sram_dbg_read(adapter, FW_CURTCB_AX, WCPU_DATA_SEL);

	val32 = mac_sram_dbg_read(adapter, (curtcb & FW_TCB_ADDR_MASK_AX) +
				  FW_CURTCB_SP_START_OFFSET, WCPU_DATA_SEL);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "[FW]Start of the stack = 0x%08x\n", val32);

	for (index = 0; index < 16 ; index = index + 4) {
		val32 = mac_sram_dbg_read(adapter, (curtcb & FW_TCB_ADDR_MASK_AX) +
					FW_CURTCB_TASK_NAME_OFFSET + index, WCPU_DATA_SEL);

		PLTFM_MEMCPY((u8 *)&task_name[index], (u8 *)&val32, 4);
	}

	task_name[FW_MAX_TASK_NAME_LEN - 1] = '\0';

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "[FW]Current task name = %s\n", task_name);
	return MACSUCCESS;
}

u32 cmd_mac_fw_status_parser(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
			     u32 input_num, char *output, u32 out_len, u32 *used)
{
	u32 i, cmd_strlen, *para_start_addr, para_val;
	char *fw_status_cmd;
	u16 id = FWSTATUS_OPCODE_MASK;
	struct mac_ax_fwstatus_payload data;
	u32 hal_cmd_ary_size = sizeof(mac_fw_status_cmd_i) / sizeof(struct mac_hal_cmd_info);

	if (input_num < OPCODE_HDR_LEN) {
		//PLTFM_MSG_TRACE("fw status invalid op code\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nfw status invalid op code\n");
		return MACFWSTATUSFAIL;
	}
	if (input_num > MAX_FWSTATSUS_PKT_LEN) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nfw status argument should be less than 10\n");
		return MACFWSTATUSFAIL;
	}
	 /* Parsing Cmd ID */
	fw_status_cmd = input[1];
	cmd_strlen = PLTFM_STRLEN(fw_status_cmd);
	for (i = 0; i < cmd_strlen; i++) {
		if (*fw_status_cmd == '\n')
			*fw_status_cmd = '\0';
		fw_status_cmd++;
	}
	for (i = 0; i < hal_cmd_ary_size; i++) {
		//PLTFM_MSG_TRACE("[FW STATUS]input string : [%s], input_num = %d\n",
		//		input[1], input_num);
		//PLTFM_MSG_TRACE("mac_fw_status_cmd_i[i].name : [%s]",mac_fw_status_cmd_i[i].name);
		//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		//	"[FW STATUS]input string : [%s], input_num = %d\n",
		//	input[1], input_num);
		//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		//	"mac_fw_status_cmd_i[i].name : [%s]", mac_fw_status_cmd_i[i].name);
		if (PLTFM_STRCMP(mac_fw_status_cmd_i[i].name, input[1]) == 0) {
			id = mac_fw_status_cmd_i[i].id;
			//PLTFM_MSG_TRACE("enter fw status dbg %s\n", mac_fw_status_cmd_i[i].name);
			//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			//	"enter fw status dbg %s\n", mac_fw_status_cmd_i[i].name);
			break;
		}
	}
	if (i == hal_cmd_ary_size) {
		//PLTFM_MSG_TRACE("FW STATUS command not found!\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "FW STATUS command not found!\n");
		return MACFWSTATUSFAIL;
	}
	// gen h2c
	data.dword0 = (u32)id;
	data.dword1 = (u32)input_num - OPCODE_HDR_LEN;
	if (input_num > OPCODE_HDR_LEN) {
		para_start_addr = &data.dword2;
		for (i = OPCODE_HDR_LEN; i < input_num; i++) {
			para_val = PLTFM_STRTOUL(input[i], 16);
			*para_start_addr = para_val;
			para_start_addr++;
		}
		/*
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "input parameter num = %d\n", input_num);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 0 = 0x%x\n", data.dword0);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 1 = 0x%x\n", data.dword1);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 2 = 0x%x\n", data.dword2);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 3 = 0x%x\n", data.dword3);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 4 = 0x%x\n", data.dword4);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 5 = 0x%x\n", data.dword5);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 6 = 0x%x\n", data.dword6);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 7 = 0x%x\n", data.dword7);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 8 = 0x%x\n", data.dword8);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data 9 = 0x%x\n", data.dword9);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data10 = 0x%x\n", data.dword10);
		 *MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		 *	    "data11 = 0x%x\n", data.dword11);
		 */
	}
	if (mac_fw_status_cmd(adapter, &data)) {
		//PLTFM_MSG_TRACE("FW STATUS H2C Fail!\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "FW STATUS H2C Fail!\n");
		return MACFWSTATUSFAIL;
	}
	return MACSUCCESS;
}

u32 cmd_mac_dl_sym(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
		   u32 input_num, char *output, u32 out_len, u32 *used)
{
#if MAC_AX_FEATURE_DBGDEC
	u32 val, ret;
	struct mac_ax_adapter *mac = adapter;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 *symbol_ptr = NULL;
	u32 file_size = 0;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "down symbol.bin\n");
	// input argument start from input[1], input[0] is mac_hal_cmd_info.name
	if (input_num < 2) {
		//PLTFM_MSG_TRACE("invalid argument\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "invalid argument\n");
		return MACFWSTATUSFAIL;
	}
	//PLTFM_SSCANF(input[1], "%d", &val);
	val = PLTFM_STRTOUL(input[1], 10);

	if (val == 1) {
		// download file
		// PLTFM_DL file, assign symbol_ptr and file_size
		switch (hw_info->chip_id) {
#if MAC_AX_8852A_SUPPORT
		case MAC_AX_CHIP_ID_8852A:
			ret = PLTFM_LD_FW_SYMBOL("hal8852a_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8852B_SUPPORT
		case MAC_AX_CHIP_ID_8852B:
			ret = PLTFM_LD_FW_SYMBOL("hal8852b_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8852C_SUPPORT
		case MAC_AX_CHIP_ID_8852C:
			ret = PLTFM_LD_FW_SYMBOL("hal8852c_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8192XB_SUPPORT
		case MAC_AX_CHIP_ID_8192XB:
			ret = PLTFM_LD_FW_SYMBOL("hal8192xb_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8851B_SUPPORT
		case MAC_AX_CHIP_ID_8851B:
			ret = PLTFM_LD_FW_SYMBOL("hal8851b_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8851E_SUPPORT
		case MAC_AX_CHIP_ID_8851E:
			ret = PLTFM_LD_FW_SYMBOL("hal8851e_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
#if MAC_AX_8852D_SUPPORT
		case MAC_AX_CHIP_ID_8852D:
			ret = PLTFM_LD_FW_SYMBOL("hal8852d_msg_symbol.bin", &symbol_ptr,
						 &file_size);
			break;
#endif
		default:
			return MACFWSTATUSFAIL;
			break;
		}

		if (ret != MACSUCCESS) {
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "reading fw failed !!!\n");
			return MACFWSTATUSFAIL;
		}

		ret = mac->ops->fw_log_set_array(adapter, (void *)symbol_ptr, file_size);
		if (ret != MACSUCCESS) {
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "load symbol failed\n");
			return MACFWSTATUSFAIL;
		}
	} else if (val == 0) {
		//release file
		ret = mac->ops->fw_log_unset_array(adapter);
		if (ret != MACSUCCESS)
			return MACFWSTATUSFAIL;
	}
	return MACSUCCESS;
#endif
}

u32 cmd_mac_qc_start(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32[4];
	struct mac_ax_fwstatus_payload data;
	u8 val8;

	/* dump fw status reg */
	val32[0] = MAC_REG_R32(R_AX_HALT_C2H);
	val32[1] = MAC_REG_R32(R_AX_UDM0);
	val32[2] = MAC_REG_R32(R_AX_UDM1);
	val32[3] = MAC_REG_R32(R_AX_UDM2);

	/* initial fw status register*/
	MAC_REG_W32(R_AX_HALT_C2H, 0);
	MAC_REG_W8(R_AX_UDM0 + 1, 0);
	MAC_REG_W32(R_AX_UDM1, 0);
	MAC_REG_W32(R_AX_UDM2, 0);

#if 0
	PLTFM_MSG_TRACE("\nQC Start:");
	PLTFM_MSG_TRACE("\n0x016c[31:0] = 0x%08x", val32[0]);
	PLTFM_MSG_TRACE("\n0x01f0[31:0] = 0x%08x", val32[1]);
	PLTFM_MSG_TRACE("\n0x01f4[31:0] = 0x%08x", val32[2]);
	PLTFM_MSG_TRACE("\n0x01f8[31:0] = 0x%08x", val32[3]);
#endif

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x016c[31:0] = 0x%08x", val32[0]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f0[31:0] = 0x%08x", val32[1]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f4[31:0] = 0x%08x", val32[2]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f8[31:0] = 0x%08x", val32[3]);

	val8 = (u8)((val32[1] & 0x0000FF00) >> 8);

	/* Fail: reg0x16c >=0x1000 && reg0x16c != 0x1002. Fail: 0<reg0x1f1<0x80 */
	if ((val32[0] != 0x1002 && val32[0] >= 0x1000) || (val8 < 0x80 && val8 > 0)) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n\n==> [Register Check]:FAIL!!\n\n");
	} else {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n\n==> [Register Check]:PASS!!\n\n");
	}

	/* dump heapinfo */
	data.dword0 = (u32)FW_STATUS_HEAPINFO;
	data.dword1 = 0;

	qc_cmd_id = 1;

	if (mac_fw_status_cmd(adapter, &data)) {
		//PLTFM_MSG_TRACE("FW STATUS H2C Fail!\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nFW STATUS H2C Fail!\n");
		return MACFWSTATUSFAIL;
	}

	return MACSUCCESS;
}

u32 cmd_mac_qc_end(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		   char *output, u32 out_len, u32 *used)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32[4];
	struct mac_ax_fwstatus_payload data;
	u8 val8;

	/* dump fw status reg */
	val32[0] = MAC_REG_R32(R_AX_HALT_C2H);
	val32[1] = MAC_REG_R32(R_AX_UDM0);
	val32[2] = MAC_REG_R32(R_AX_UDM1);
	val32[3] = MAC_REG_R32(R_AX_UDM2);

#if 0
	PLTFM_MSG_TRACE("\nQC End:");
	PLTFM_MSG_TRACE("\n0x016c[31:0] = 0x%08x", val32[0]);
	PLTFM_MSG_TRACE("\n0x01f0[31:0] = 0x%08x", val32[1]);
	PLTFM_MSG_TRACE("\n0x01f4[31:0] = 0x%08x", val32[2]);
	PLTFM_MSG_TRACE("\n0x01f8[31:0] = 0x%08x", val32[3]);
#endif

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x016c[31:0] = 0x%08x", val32[0]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f0[31:0] = 0x%08x", val32[1]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f4[31:0] = 0x%08x", val32[2]);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n0x01f8[31:0] = 0x%08x", val32[3]);

	val8 = (u8)((val32[1] & 0x0000FF00) >> 8);

	/* Fail: reg0x16c >=0x1000 && reg0x16c != 0x1002. Fail: 0<reg0x1f1<0x80 */
	if ((val32[0] != 0x1002 && val32[0] >= 0x1000) || (val8 < 0x80 && val8 > 0)) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n\n==> [Register Check]:FAIL!!\n\n");
	} else {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n\n==> [Register Check]:PASS!!\n\n");
	}

	/* dump heapinfo */
	data.dword0 = (u32)FW_STATUS_HEAPINFO;
	data.dword1 = 0;

	qc_cmd_id = 2;

	if (mac_fw_status_cmd(adapter, &data)) {
		//PLTFM_MSG_TRACE("FW STATUS H2C Fail!\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nFW STATUS H2C Fail!\n");
		return MACFWSTATUSFAIL;
	}

	return MACSUCCESS;
}

u32 cmd_mac_req_pwr_st(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		       char *output, u32 out_len, u32 *used)
{
	u32 req_pwr_st = 0, ret = MACSUCCESS;

	if (input_num < 2) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\ninvalid argument\n");
		return MACFWSTATUSFAIL;
	}

	req_pwr_st = PLTFM_STRTOUL(input[1], 10);

	switch (req_pwr_st) {
	case REQ_PWR_ST_OPEN_RF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: OPEN_RF\n", __func__);
		break;
	case REQ_PWR_ST_CLOSE_RF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: CLOSE_RF\n", __func__);
		break;
	case REQ_PWR_ST_ADC_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: ADC_OFF\n", __func__);
		break;
	case REQ_PWR_ST_BB_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: BB_OFF\n", __func__);
		break;
	case REQ_PWR_ST_CPU_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: CPU_OFF\n", __func__);
		break;
	case REQ_PWR_ST_MAC_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: MAC_OFF\n", __func__);
		break;
	case REQ_PWR_ST_PLL_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: PLL_OFF\n", __func__);
		break;
	case REQ_PWR_ST_SWRD_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: SWRD_OFF\n", __func__);
		break;
	case REQ_PWR_ST_XTAL_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: XTAL_OFF\n", __func__);
		break;
	case REQ_PWR_ST_ADIE_OFF:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: ADIE_OFF\n", __func__);
		break;
	case REQ_PWR_ST_BYPASS_DATA_ON:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: BYPASS_DATA_ON\n", __func__);
		break;
	default:
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\n%s: nonvalid cmd\n", __func__);
		break;
	}

	ret = mac_req_pwr_state_cfg(adapter, (enum mac_req_pwr_st)req_pwr_st);

	return ret;
}

u32 cmd_mac_req_pwr_lvl(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
			char *output, u32 out_len, u32 *used)
{
	u32 ret = MACSUCCESS;
	struct mac_ax_req_pwr_lvl_info parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct mac_ax_req_pwr_lvl_info));

	if (input_num < 5) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\ninvalid argument\n");
		return MACFWSTATUSFAIL;
	}

	parm.macid = PLTFM_STRTOUL(input[1], 10);
	parm.bcn_to_val = PLTFM_STRTOUL(input[2], 10);
	parm.ps_lvl = PLTFM_STRTOUL(input[3], 10);
	parm.trx_lvl = PLTFM_STRTOUL(input[4], 10);

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n%s: macid:%d req_bcn_to_val:%d req_ps_lvl:%d req_trx_lvl:%d\n",
		    __func__, parm.macid, parm.bcn_to_val, parm.ps_lvl, parm.trx_lvl);

	ret = mac_req_pwr_lvl_cfg(adapter, &parm);

	return ret;
}

u32 cmd_mac_fw_chsw(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used)
{
	// struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ch_switch_parm parm = {0};
	u8 channel;

	if (input_num < 2) {
		//PLTFM_MSG_TRACE("invalid argument\n");
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    " invalid argument\n");
		return MACFWSTATUSFAIL;
	}

	channel = (u8)PLTFM_STRTOUL(input[2], 10);

	if (PLTFM_STRCMP(input[1], "24G") == 0)
		parm.ch_band = BAND_ON_24G;
	else if (PLTFM_STRCMP(input[1], "5G") == 0)
		parm.ch_band = BAND_ON_5G;
	else if (PLTFM_STRCMP(input[1], "6G") == 0)
		parm.ch_band = BAND_ON_6G;
	else
		return MACFWSTATUSFAIL;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n channel = %d / ch_band = %d\n", channel, parm.ch_band);

	parm.pri_ch = channel;
	parm.central_ch = channel;

	mac_ch_switch_ofld(adapter, parm);
	return MACSUCCESS;
}

u32 cmd_mac_fw_chsw_ret(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
			char *output, u32 out_len, u32 *used)
{
	// struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ch_switch_rpt rpt = {0};

	mac_get_ch_switch_rpt(adapter, &rpt);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "\n Channel switch result (%d)\n", rpt.result);

	return MACSUCCESS;
}

u32 cmd_mac_bcn_stats(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		      char *output, u32 out_len, u32 *used)
{
	static u32 bcn_early_cnt;
	static u32 bcn_rx_cnt;
	u32 bcn_early_tmp;
	u32 bcn_rx_tmp;
	u32 dbg_addr = mac_get_ple_dbg_addr(adapter);

	if (PLTFM_STRCMP(input[1], "start") == 0) {
		bcn_early_cnt = mac_sram_dbg_read(adapter,
						  dbg_addr + R_COMMON_BCNEARLY, SHARED_BUF_SEL);
		bcn_rx_cnt = mac_sram_dbg_read(adapter,
					       dbg_addr + R_COMMON_BCN_CNT, SHARED_BUF_SEL);

		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nStart. Current bcn_early_cnt = %d, bcn_rx_cnt = %d\n",
			    bcn_early_cnt, bcn_rx_cnt);
	} else if (PLTFM_STRCMP(input[1], "end") == 0) {
		bcn_early_tmp = mac_sram_dbg_read(adapter,
						  dbg_addr + R_COMMON_BCNEARLY, SHARED_BUF_SEL);
		bcn_early_tmp -= bcn_early_cnt;
		bcn_rx_tmp = mac_sram_dbg_read(adapter,
					       dbg_addr + R_COMMON_BCN_CNT, SHARED_BUF_SEL);
		bcn_rx_tmp -= bcn_rx_cnt;

		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			    "\nEnd. Diff bcn_early_cnt = %d, bcn_rx_cnt = %d, ratio = %d\n",
			    bcn_early_tmp, bcn_rx_tmp, bcn_rx_tmp * 100 / bcn_early_tmp);
	} else {
		return MACFWSTATUSFAIL;
	}

	return MACSUCCESS;
}

s32 mac_halmac_cmd(struct mac_ax_adapter *adapter, char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	u32 token_len = 0;
	char argv[MAC_MAX_ARGC][MAC_MAX_ARGV];

	if (output) {
		adapter->fw_dbgcmd.buf = output;
		adapter->fw_dbgcmd.out_len = out_len;
		adapter->fw_dbgcmd.used = 0;
	} else {
		PLTFM_MSG_TRACE("%s invalid argument\n", __func__);
		return MACFWSTATUSFAIL;
	}
	do {
		token = PLTFM_STRSEP(&input, ", ");
		if (token) {
			token_len = PLTFM_STRLEN(token);
			if (token_len <= MAC_MAX_ARGV) {
				if (token_len > 0) {
					if (token[token_len - 1] == '\n') {
						--token_len;
						token[token_len] = 0;
					}
				}
				PLTFM_STRCPY(argv[argc], token);
			}
			argc++;
		} else {
			break;
		}
	} while (argc < MAC_MAX_ARGC);

	mac_halmac_cmd_parser(adapter, argv, argc, output, out_len);

	return MACSUCCESS;
}

void mac_halmac_cmd_parser(struct mac_ax_adapter *adapter,
			   char input[][MAC_MAX_ARGV], u32 input_num,
			   char *output, u32 out_len)
{
	u32 hal_cmd_ary_size = sizeof(mac_hal_cmd_i) / sizeof(struct mac_hal_cmd_info);
	u32 i = 0;
	u32 *used;

	if (output) {
		adapter->fw_dbgcmd.buf = output;
		adapter->fw_dbgcmd.out_len = out_len;
		adapter->fw_dbgcmd.used = 0;
	} else {
		PLTFM_MSG_TRACE("%s invalid argument\n", __func__);
		return;
	}

	used = &adapter->fw_dbgcmd.used;
	//struct mac_ax_fwstatus_payload data;

	if (hal_cmd_ary_size == 0)
		return;

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < hal_cmd_ary_size; i++) {
			//PLTFM_MSG_TRACE("input string : %s\n, input_num = %d",
			//		input[0], input_num);
			if (PLTFM_STRCMP(mac_hal_cmd_i[i].name, input[0]) == 0) {
				//PLTFM_MSG_TRACE("enter hal dbg %s\n", mac_hal_cmd_i[i].name);
				//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				//"enter hal dbg %s\n", mac_hal_cmd_i[i].name);
				if (mac_hal_cmd_i[i].handler(adapter, input, input_num,
							     output, out_len, used)) {
					//PLTFM_MSG_TRACE("%s command process error\n",
					MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
						    mac_hal_cmd_i[i].name);
				}
				PLTFM_DELAY_MS(200);
				break;
			}
		}
		if (i == hal_cmd_ary_size) {
			//PLTFM_MSG_TRACE("HAL command not found!\n");
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "HAL command not found!\n");
			return;
		}
	}
	PLTFM_MSG_TRACE("-----------------------%s function return\n", __func__);
}

u32 c2h_fw_status(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		  struct rtw_c2h_info *info)
{
	//struct c2h_proc_func *proc = c2h_proc_sys_palt_autotest;
	//u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	//		struct rtw_c2h_info *info) = NULL;
	u32 hdr0, ret = MACSUCCESS;
	u32 func, pkt_cnt, i;
	u8 *content;
	struct fw_status_pkt pkt_info;
	struct fw_status_proc_class *proc = fw_status_proc_sys;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	//u32 remain_len = out_len - *used;

	PLTFM_MSG_TRACE("[--------------------]%s\n", __func__);
	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	//set info
	info->c2h_cat = GET_FIELD(hdr0, C2H_HDR_CAT);
	info->c2h_class = GET_FIELD(hdr0, C2H_HDR_CLASS);
	info->c2h_func = GET_FIELD(hdr0, C2H_HDR_FUNC);
	//info->done_ack = 0;
	//info->rec_ack = 0;
	info->content = buf  + FWCMD_HDR_LEN;
	//info->h2c_return = info->c2h_data[1];

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	if (func == FWCMD_C2H_FUNC_FW_STATUS) {
		pkt_cnt = le32_to_cpu(*((u32 *)info->content));
		//PLTFM_MSG_TRACE("[------------------pkt_cnt--] %d\n", pkt_cnt);
		content = info->content + LEN_PKTCNT;
		for (i = 0; i < pkt_cnt; i++) {
			proc = fw_status_proc_sys;
			pkt_info.op_code = le32_to_cpu(*((u32 *)(content)));
			//PLTFM_MSG_TRACE("[------------------op_code--] %d\n", pkt_info.op_code);
			content += LEN_OPCODE;
			pkt_info.length = le32_to_cpu(*((u32 *)(content)));
			//PLTFM_MSG_TRACE("[------------------length--] %d\n", pkt_info.length);
			content += LEN_LENGTH;
			pkt_info.data = content;
			content += pkt_info.length;
			for (; proc->id != FW_STATUS_MAX; proc++) {
				if (GET_FIELD_OPCODE(pkt_info.op_code) == proc->id) {
					MAC_DBG_MSG(out_len, *used, output + *used,
						    out_len - *used, "\n");
					ret = proc->handler(adapter, pkt_info.data,
					pkt_info.length);
				}
			}
		}
	}
	return ret;
}

u32 fw_status_taskinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	u32 ret = MACSUCCESS;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Task\t\tState\tPrio\tStack start\tMin Stack(DW)\tNum\tCnt\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "-------------------------------------------\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "%s\n", (const char *)buf);

	//PLTFM_MSG_TRACE("Task\t\tState\tPrio\tStack start\tMin Stack(DW)\tNum\n");
	//PLTFM_MSG_TRACE("-------------------------------------------\n");
	//PLTFM_MSG_TRACE("%s", (const char *)buf);
	return ret;
}

u32 fw_status_flashinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct flash_info flashinfo;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	//PLTFM_MSG_TRACE("[--------------------]%s\n", __func__);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "[--------------------]%s\n", __func__);
	PLTFM_MEMCPY(&flashinfo, buf, sizeof(struct flash_info));
	//PLTFM_MSG_TRACE("b1InitDone : %d\n", flashinfo.b1initdone);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "b1InitDone : %d\n", flashinfo.b1initdone);
	//PLTFM_MSG_TRACE("u1FlashType : %d\n", flashinfo.u1flashtype);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "u1FlashType : %d\n", flashinfo.u1flashtype);
	//PLTFM_MSG_TRACE("u4FlashSize : %d\n", flashinfo.u4flashsize);
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "u4FlashSize : %d\n", flashinfo.u4flashsize);

	return MACSUCCESS;
}

u32 fw_status_heapinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	u32 ret = MACSUCCESS;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;
	char *p;
	char str[160] = {0};
	char *pstr = str;
	u8 i = 0;
	u32 fastheap_free = 0, slowheap_free = 0;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Index\t\tStart\t\tTotal(B)\tFree(B)\tMin Free(B)\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "-------------------------------------------------------------------\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "%s", (const char *)buf);

	//below is related to qc_start, qc_end cmd
	if (qc_cmd_id > 0) {
		PLTFM_MEMCPY(str, buf, len);
		p = PLTFM_STRSEP(&pstr, "\t");
		while (p) {
			//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
			//    "%s, i=%d\n", (const char *)p, i);

			if (i == 4)
				fastheap_free = PLTFM_STRTOUL(p, 10);
			else if (i == 9)
				slowheap_free = PLTFM_STRTOUL(p, 10);

			i++;
			p = PLTFM_STRSEP(&pstr, "\t");
		}

		//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		//                "fh=%d\n", fastheap_free);
		//MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		//                "sh=%d\n", slowheap_free);

		if (qc_cmd_id == 1) {
			fheap_start = fastheap_free;
			sheap_start = slowheap_free;
		} else if (qc_cmd_id == 2) {
			u32 fh_diff = fastheap_free - fheap_start;
			u32 sh_diff = slowheap_free - sheap_start;

			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "[Difference] Fast heap : %d\n", fh_diff);
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "[Difference] Slow heap : %d\n", sh_diff);

			if (fheap_start == 0 || sheap_start == 0) {
				MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
					    "\n==> [Heap check]:Wrong cmd sequence!!\n");
			} else {
				if (fh_diff <= FWQC_FAST_HEAP_TH && sh_diff <= FWQC_SLOW_HEAP_TH) {
					MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
						    "\n==> [Heap check]:PASS!!\n");
				} else {
					MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
						    "\n==> [Heap check]:FAIL!!\n");
				}
			}
			fheap_start = 0;
			sheap_start = 0;
		}
		qc_cmd_id = 0;
	}

	return ret;
}

u32 fw_status_meminfo_fast_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct mem_info *mem_info;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	mem_info = (struct mem_info *)buf;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Fast Heap:\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Owner ID\tType\t\tTotalSize(B)\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "------------------------------------------------\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "%s\n", (const char *)buf);
	#if 0
	while (mem_info->total_size != 0) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used, "%s\t\t%s\t\t%u\r\n",
			    MallocIDName[mem_info->owner_id],
			    MallocTypeName[mem_info->owner_type],
			    mem_info->total_size);
		mem_info++;
	}
	#endif
	return MACSUCCESS;
}

u32 fw_status_meminfo_slow_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct mem_info *mem_info;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	mem_info = (struct mem_info *)buf;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Slow Heap:\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "Owner ID\tType\t\tTotalSize(B)\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "------------------------------------------------\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "%s\n", (const char *)buf);
	#if 0
	while (mem_info->total_size != 0) {
		MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used, "%s\t\t%s\t\t%u\r\n",
			    MallocIDName[mem_info->owner_id],
			    MallocTypeName[mem_info->owner_type],
			    mem_info->total_size);
		mem_info++;
	}
	#endif
	return MACSUCCESS;
}

u32 fw_status_psinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;

	// Not support now
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "[Err]\n");
	return MACFWSTATUSFAIL;
}

u32 fw_status_h2c_c2hinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;

	// Not support now
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "[Err]\n");
	return MACFWSTATUSFAIL;
}

u32 fw_status_isrinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct isr_info *isr_info;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;
	u32 i;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	isr_info = (struct isr_info *)buf;

	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "ISR Name\tCount\t\tExec Time(historical high)\n");
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    "------------------------------------------------\n");

	for (i = ISRStatistic_WDT; i < ISRStatistic_MAX; i++) {
		if (i >= ISRStatistic_DMAC0_GT0) {
			if (i == ISRStatistic_DMAC0_GT0) {
				MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
					    "------------------------------------------------\n");
			}
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "%s\t%u\t\t%u_%u\n", ISRName[i],
				    isr_info->ISRCnt[i], isr_info->ISRExecTimeMax_hi[i],
				    isr_info->ISRExecTimeMax_lo[i]);
		} else {
			MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
				    "%s\t\t%u\t\t%u_%u\n", ISRName[i],
				    isr_info->ISRCnt[i], isr_info->ISRExecTimeMax_hi[i],
				    isr_info->ISRExecTimeMax_lo[i]);
		}
	}
	return MACSUCCESS;
}

u32 fw_status_chsw_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len)
{
	struct chswofld_timing_info *timing;
	char *output = adapter->fw_dbgcmd.buf;
	u32 *used = &adapter->fw_dbgcmd.used;
	u32 out_len = adapter->fw_dbgcmd.out_len;
	u32 remain_len = out_len - *used;

	if (len > remain_len)
		return MACFWSTATUSFAIL;

	timing = (struct chswofld_timing_info *)buf;
	MAC_DBG_MSG(out_len, *used, output + *used, out_len - *used,
		    " total: %d us\n\t(mac: %d, bb: %d, rf: %d, rf reld: %d)\n",
		    timing->total, timing->mac, timing->bb, timing->rf, timing->rf_reld);
	return MACSUCCESS;
}

#if MAC_AX_FEATURE_DBGDEC
static void fw_log_private_dump(struct mac_ax_adapter *adapter, u32 *buf, u32 msgno,
				u8 para_num, u8 isint, char *str_buf)
{
	// check data integrity
	switch (para_num) {
	case 1:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf));
		break;
	case 2:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)));
		break;
	case 3:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)));
		break;
	case 4:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)));
		break;
	case 5:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)));
		break;
	case 6:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)));
		break;
	case 7:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)));
		break;
	case 8:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)));
		break;
	case 9:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)));
		break;
	case 10:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)));
		break;
	case 11:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)));
		break;
	case 12:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)));
		break;
	case 13:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)));
		break;
	case 14:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)));
		break;
	case 15:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)),
			       le32_to_cpu(*(u32 *)(buf + 14)));
		break;
	case 16:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",
			       msgno,
			       isint,
			       para_num,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)),
			       le32_to_cpu(*(u32 *)(buf + 14)),
			       le32_to_cpu(*(u32 *)(buf + 15)));
		break;
	default:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       "fw_enc(%d,%d,%d)",
			       msgno,
			       isint,
			       para_num);
		break;
	}
	PLTFM_MSG_ALWAYS("C2H log: %s\n", str_buf);
	//PLTFM_FREE(str_buf, H2C_LONG_DATA_LEN);
	//return;
}

static void fw_log_int_dump(struct mac_ax_adapter *adapter, u32 *buf, u32 msg_array_idx,
			    u8 para_num, char *str_buf)
{
	// check data integrity
	switch (para_num) {
	case 1:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf));
		break;
	case 2:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)));

		break;
	case 3:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)));
		break;
	case 4:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)));
		break;
	case 5:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)));
		break;
	case 6:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)));
		break;
	case 7:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)));
		break;
	case 8:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)));
		break;
	case 9:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)));
		break;
	case 10:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)));
		break;
	case 11:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)));
		break;
	case 12:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)));
		break;
	case 13:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)));
		break;
	case 14:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)));
		break;
	case 15:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)),
			       le32_to_cpu(*(u32 *)(buf + 14)));
		break;
	case 16:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       le32_to_cpu(*(u32 *)buf),
			       le32_to_cpu(*(u32 *)(buf + 1)),
			       le32_to_cpu(*(u32 *)(buf + 2)),
			       le32_to_cpu(*(u32 *)(buf + 3)),
			       le32_to_cpu(*(u32 *)(buf + 4)),
			       le32_to_cpu(*(u32 *)(buf + 5)),
			       le32_to_cpu(*(u32 *)(buf + 6)),
			       le32_to_cpu(*(u32 *)(buf + 7)),
			       le32_to_cpu(*(u32 *)(buf + 8)),
			       le32_to_cpu(*(u32 *)(buf + 9)),
			       le32_to_cpu(*(u32 *)(buf + 10)),
			       le32_to_cpu(*(u32 *)(buf + 11)),
			       le32_to_cpu(*(u32 *)(buf + 12)),
			       le32_to_cpu(*(u32 *)(buf + 13)),
			       le32_to_cpu(*(u32 *)(buf + 14)),
			       le32_to_cpu(*(u32 *)(buf + 15)));
		break;
	default:
		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg);
		break;
	}
	PLTFM_MSG_ALWAYS("C2H log: %s\n", str_buf);
	//return;
}

u32 fw_log_scan_array(struct mac_ax_adapter *adapter, u32 msgno)
{
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 i = 0, msg_last, array_size;

	if (adapter->fw_log_array_dl_size) {
		array_size = adapter->fw_log_array_dl_size / sizeof(struct mac_fw_msg);
		for (i = 0; i < array_size ; i++) {
			if (adapter->fw_log_array[i].msgno == msgno)
				return i;
		}
		return 0;
	} else {
		switch (hw_info->chip_id) {
#if MAC_AX_8852A_SUPPORT
		case MAC_AX_CHIP_ID_8852A:
			msg_last = MSG_8852A_LAST;
			break;
#endif
#if MAC_AX_8852B_SUPPORT
		case MAC_AX_CHIP_ID_8852B:
			msg_last = MSG_8852B_LAST;
			break;
#endif
#if MAC_AX_8852C_SUPPORT
		case MAC_AX_CHIP_ID_8852C:
			msg_last = MSG_8852C_LAST;
			break;
#endif
#if MAC_AX_8192XB_SUPPORT
		case MAC_AX_CHIP_ID_8192XB:
			msg_last = MSG_8192XB_LAST;
			break;
#endif
#if MAC_AX_8851B_SUPPORT
		case MAC_AX_CHIP_ID_8851B:
			msg_last = MSG_8851B_LAST;
			break;
#endif
#if MAC_AX_8851E_SUPPORT
		case MAC_AX_CHIP_ID_8851E:
			msg_last = MSG_8851E_LAST;
			break;
#endif
#if MAC_AX_8852D_SUPPORT
		case MAC_AX_CHIP_ID_8852D:
			msg_last = MSG_8852D_LAST;
			break;
#endif
		default:
			PLTFM_MSG_WARN("array not exist\n");
			return 0;
		}
		if (msgno >= msg_last)
			return 0;

		while (adapter->fw_log_array[i].msgno != msg_last) {
			if (adapter->fw_log_array[i].msgno == msgno)
				return i;
			i++;
		}
		return 0;
	}
}

void fw_log_set_array(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hw_info *hw_info = adapter->hw_info;

	switch (hw_info->chip_id) {
#if MAC_AX_8852A_SUPPORT
	case MAC_AX_CHIP_ID_8852A:
		adapter->fw_log_array = fw_log_8852a;
		break;
#endif
#if MAC_AX_8852B_SUPPORT
	case MAC_AX_CHIP_ID_8852B:
		adapter->fw_log_array = fw_log_8852b;
		break;
#endif
#if MAC_AX_8852C_SUPPORT
	case MAC_AX_CHIP_ID_8852C:
		adapter->fw_log_array = fw_log_8852c;
		break;
#endif
#if MAC_AX_8192XB_SUPPORT
	case MAC_AX_CHIP_ID_8192XB:
		adapter->fw_log_array = fw_log_8192xb;
		break;
#endif
#if MAC_AX_8851B_SUPPORT
	case MAC_AX_CHIP_ID_8851B:
		adapter->fw_log_array = fw_log_8851b;
		break;
#endif
#if MAC_AX_8851E_SUPPORT
	case MAC_AX_CHIP_ID_8851E:
		adapter->fw_log_array = fw_log_8851e;
		break;
#endif
#if MAC_AX_8852D_SUPPORT
	case MAC_AX_CHIP_ID_8852D:
		adapter->fw_log_array = fw_log_8852d;
		break;
#endif
	default:
		return;
	}
}

void fw_log_dump(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		 struct rtw_c2h_info *info)
{
	u8 syntax_3, isint, fileno;
	u32 msgno_0, msgno_1, msgno_2, msgno_3;
	u16 lineno_0, lineno_1;
	u8 para_num, paranum_idx, color;
	u32 msgno, msg_array_idx;
	u16 lineno;
	u8 *para;
	char str_buf[STR_BUF_SIZE];

	if (!adapter->fw_log_array)
		fw_log_set_array(adapter);
	if (!adapter->fw_log_array) {
		PLTFM_MSG_ERR("[ERR]unsupported fw_log_array\n");
		return;
	}
	color = (*(buf + FWCMD_HDR_LEN + 2)) >> 1;
	isint = (color & BIT1) >> 1;
	paranum_idx = color & BIT0;
	syntax_3 = *(buf + FWCMD_HDR_LEN + 3);
	msgno_0 = (u32)*(buf + FWCMD_HDR_LEN + 4);
	msgno_1 = (u32)*(buf + FWCMD_HDR_LEN + 5);
	msgno_2 = (u32)*(buf + FWCMD_HDR_LEN + 6);
	msgno_3 = (u32)*(buf + FWCMD_HDR_LEN + 7);
	msgno = (msgno_0 & 0x000000ff) |
		((msgno_1 & 0x000000ff) << 8) |
		((msgno_2 & 0x000000ff) << 16) |
		((msgno_3 & 0x000000ff) << 24);

	fileno = *(buf + FWCMD_HDR_LEN + 8);
	lineno_0 = (u16)*(buf + FWCMD_HDR_LEN + 9);
	lineno_1 = (u16)*(buf + FWCMD_HDR_LEN + 10);
	lineno = (lineno_0 & 0x00ff) | ((lineno_1 & 0x00ff) << 8);
	if (paranum_idx) {
		para_num = *(buf + FWCMD_HDR_LEN + 11);
		para = (buf + FWCMD_HDR_LEN + 12);
	} else {
		para_num = 0;
		para = NULL;
	}
	PLTFM_MEMSET(str_buf, 0, STR_BUF_SIZE);
	#if 0
	str_buf = (char *)PLTFM_MALLOC(H2C_LONG_DATA_LEN);
	if (!str_buf) {
		PLTFM_MSG_WARN("fw_log_int_dump str_buf 0\n");
		return;
	}
	#endif
	msg_array_idx = fw_log_scan_array(adapter, msgno);
	//PLTFM_MSG_WARN("%s, msgno = %d, idx = %d, isint = %d, para_num = %d\n",
	//__FUNCTION__, msgno, msg_array_idx, isint, para_num);
	if (isint == 0 && para_num != 0 && msg_array_idx != 0) {
		if (buf[len - 1] != '\0')
			buf[len - 1] = '\0';

		PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
			       adapter->fw_log_array[msg_array_idx].msg,
			       (char *)(buf + FWCMD_HDR_LEN + 12));
		PLTFM_MSG_ALWAYS("C2H log: %s\n", str_buf);
	} else if (msg_array_idx != 0 && isint == 1) {
		fw_log_int_dump(adapter, (u32 *)para, msg_array_idx, para_num, str_buf);
	} else {
		// print specific message and msgno, parameters
		if (isint == 1) {
			fw_log_private_dump(adapter, (u32 *)para, msgno, para_num, isint, str_buf);
		} else if (para_num != 0) {
			PLTFM_SNPRINTF(str_buf, STR_BUF_SIZE,
				       "fw_enc(%d,%d,%d,%s)",
				       msgno,
				       isint,
				       para_num,
				       (char *)(buf + FWCMD_HDR_LEN + 12));
			PLTFM_MSG_ALWAYS("C2H log: %s\n", str_buf);
		}
	}
	//return;
}

u8 *fw_log_skip_trailing_zero(u8 *s_ptr, u8 *end_ptr)
{
	u8 charac;

	while (s_ptr < end_ptr) {
		charac = *(s_ptr);
		if (charac != 0x00)
			break;
		s_ptr++;
	}
	return s_ptr;
}

u32 fw_log_check_integrity(u8 *symbol_tab, u8 *end_ptr)
{
	u8 charac, i = 0;
	u8 reserved[] = {'R', 'E', 'S', 'E', 'R', 'V', 'E', 'D', '\0'};

	charac = *symbol_tab;
	while (charac != 0x00) {
		if (charac != reserved[i])
			return MACFWLOGINTERR;// return fail
		i++;
		if ((symbol_tab + i) >= end_ptr)
			return MACFWLOGINTERR;// return fail
		charac = *(symbol_tab + i);
	}
	return MACSUCCESS;
}

u32 fw_log_create_array(struct mac_ax_adapter *adapter, struct mac_fw_msg *array_ptr,
			u8 *symbol_tab, u8 *end_ptr, u32 enum_count, u8 *enum_list)
{
	u32 i;
	u8 charac;

	for (i = 0; i < enum_count; i++) {
		array_ptr[i].msgno = le32_to_cpu(*(u32 *)enum_list);
		enum_list += 4;
		// walk through symbol table
		array_ptr[i].msg = (char *)symbol_tab;
		//PLTFM_MSG_WARN("[test str] %s\n", (char *)symbol_tab);
		//point to next msg
		while (symbol_tab < end_ptr) {
			charac = *(symbol_tab);
			if (charac == 0x00)
				break;
			symbol_tab++;
		}
		symbol_tab = fw_log_skip_trailing_zero(symbol_tab, end_ptr);
		if (symbol_tab > end_ptr)
			return MACFWLOGINTERR;
	}
	return MACSUCCESS;
}

u32 mac_fw_log_set_array(struct mac_ax_adapter *adapter, void *symbol_ptr, u32 file_size)
{
	u8 *sym_hdr, *enum_list, *symbol_tab, *end_ptr;
	u32 enum_count, alignment_size, ret;
	struct mac_fw_msg *new_fw_log_array = NULL;

	sym_hdr = (u8 *)symbol_ptr;
	enum_list = (u8 *)(sym_hdr + 8);
	alignment_size = le32_to_cpu(*(u32 *)sym_hdr);
	enum_count = le32_to_cpu(*(u32 *)(sym_hdr + 4));
	symbol_tab = (u8 *)(enum_list + 4 * enum_count);
	end_ptr = sym_hdr + file_size;
	//PLTFM_MSG_WARN("alignment_size = %d, enum_count = %d\n", alignment_size, enum_count);
	// alignment
	symbol_tab = fw_log_skip_trailing_zero(symbol_tab, end_ptr);

	// check integrity
	ret = fw_log_check_integrity(symbol_tab, end_ptr);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]check symbol integrity\n");
		return MACFWLOGINTERR;// return fail
	}

	//create array
	new_fw_log_array = (struct mac_fw_msg *)PLTFM_MALLOC(enum_count *
			    sizeof(struct mac_fw_msg));
	if (!new_fw_log_array) {
		PLTFM_MSG_ERR("[ERR]malloc new_fw_log_array\n");
		return MACBUFALLOC;
	}

	//PLTFM_MSG_WARN("addr of array = 0x%llx\n", (u64)new_fw_log_array);
	ret = fw_log_create_array(adapter, new_fw_log_array, symbol_tab, end_ptr, enum_count,
				  enum_list);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_WARN("unexpected symbol\n");
		PLTFM_FREE((void *)new_fw_log_array, enum_count * sizeof(struct mac_fw_msg));
		return MACFWLOGINTERR;// return fail
	}

	//PLTFM_FREE((void *)new_fw_log_array, enum_count * sizeof(struct mac_fw_msg));
	adapter->fw_log_array = new_fw_log_array;
	adapter->fw_log_array_dl = new_fw_log_array;
	adapter->fw_log_array_dl_size = enum_count * sizeof(struct mac_fw_msg);
	return MACSUCCESS;
}

u32 mac_fw_log_unset_array(struct mac_ax_adapter *adapter)
{
	if (adapter->fw_log_array_dl) {
		PLTFM_FREE(adapter->fw_log_array_dl, adapter->fw_log_array_dl_size);
		adapter->fw_log_array = NULL;
		adapter->fw_log_array_dl = NULL;
		adapter->fw_log_array_dl_size = 0;
	}
	fw_log_set_array(adapter);
	return MACSUCCESS;
}

#endif

