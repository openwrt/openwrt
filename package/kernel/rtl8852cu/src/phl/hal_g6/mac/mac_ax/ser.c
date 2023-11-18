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

#include "ser.h"
#include "mac_priv.h"

#define DBG_SENARIO_SH 28

#define L0_TO_L1_EVENT_SH 28
#define L0_TO_L1_EVENT_MSK 0xF

enum mac_ax_l0_to_l1_event {
	MAC_AX_L0_TO_L1_CHIF_IDLE = 0,
	MAC_AX_L0_TO_L1_CMAC_DMA_IDLE = 1,
	MAC_AX_L0_TO_L1_RLS_PKID = 2,
	MAC_AX_L0_TO_L1_PTCL_IDLE = 3,
	MAC_AX_L0_TO_L1_RX_QTA_LOST = 4,
	MAC_AX_L0_TO_L1_DLE_STAT_HANG = 5,
	MAC_AX_L0_TO_L1_PCIE_STUCK = 6,
	MAC_AX_L0_TO_L1_EVENT_MAX = 15,
};

static void dump_err_status_dispatcher(struct mac_ax_adapter *adapter);
static void dump_err_status_dmac(struct mac_ax_adapter *adapter);
static void dump_err_status_cmac(struct mac_ax_adapter *adapter, u8 band);

u32 mac_dbg_log_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_dbgpkg dbg_val;
	struct mac_ax_dbgpkg_en dbg_en;
	u32 error = 0, dbg_port_dump_en = 0, val32, mem_size;

	error = (enum mac_ax_err_info)MAC_REG_R32(R_AX_HALT_C2H);

	switch (error) {
	case MAC_AX_ERR_L1_ERR_DMAC:
	case MAC_AX_ERR_L0_PROMOTE_TO_L1:
		PLTFM_MEMSET(&dbg_en, 0, sizeof(struct mac_ax_dbgpkg_en));
		if (adapter->ser_info.dbg_lvl.dmac_dbg_dump) {
			dbg_en.dmac_dbg = 1;
			dbg_port_dump_en = 1;
		}
		if (adapter->ser_info.dbg_lvl.cmac_dbg_dump) {
			dbg_en.cmac_dbg = 1;
			dbg_port_dump_en = 1;
		}
		if (adapter->ser_info.dbg_lvl.txflow_dbg_dump) {
			dbg_en.tx_flow_dbg = 1;
			dbg_port_dump_en = 1;
		}
		if (adapter->ser_info.dbg_lvl.dbgprt_dump) {
			dbg_en.mac_dbg_port = 1;
			dbg_en.dle_dbg = 1;
			dbg_port_dump_en = 1;
		}

		if (dbg_port_dump_en) {
			// for debug port log would cause fw wdt
			val32 = MAC_REG_R32(R_AX_HIMR0);
			val32 &= ~B_AX_WDT_TIMEOUT_INT_EN;
			MAC_REG_W32(R_AX_HIMR0, val32);
			mac_dbg_status_dump(adapter, &dbg_val, &dbg_en);
			val32 = MAC_REG_R32(R_AX_HIMR0);
			val32 |= B_AX_WDT_TIMEOUT_INT_EN;
			MAC_REG_W32(R_AX_HIMR0, val32);
		}
		if (adapter->ser_info.dbg_lvl.share_mem_dump) {
			mem_size = mac_get_mem_size(adapter, MAC_AX_MEM_SHARED_BUF);
			val32 = MAC_REG_R32(R_AX_HIMR0);
			val32 &= ~B_AX_WDT_TIMEOUT_INT_EN;
			MAC_REG_W32(R_AX_HIMR0, val32);
			mac_mem_dump(adapter, MAC_AX_MEM_SHARED_BUF, 0x0, NULL, mem_size, 0);
			val32 = MAC_REG_R32(R_AX_HIMR0);
			val32 |= B_AX_WDT_TIMEOUT_INT_EN;
			MAC_REG_W32(R_AX_HIMR0, val32);
		}

		if (adapter->ser_info.dbg_lvl.mac_reg_dump)
			mac_reg_dump(adapter, MAC_AX_REG_MAC);
		break;
	case MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE:
		break;
	case MAC_AX_ERR_L1_RESET_RECOVERY_DONE:
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u32 mac_dbg_log_lvl_adjust(struct mac_ax_adapter *adapter, struct mac_debug_log_lvl *lvl)
{
	adapter->ser_info.dbg_lvl.dbgprt_dump = lvl->dbgprt_dump;
	adapter->ser_info.dbg_lvl.share_mem_dump = lvl->share_mem_dump;
	adapter->ser_info.dbg_lvl.cmac_dbg_dump = lvl->cmac_dbg_dump;
	adapter->ser_info.dbg_lvl.dmac_dbg_dump = lvl->dmac_dbg_dump;
	adapter->ser_info.dbg_lvl.txflow_dbg_dump = lvl->txflow_dbg_dump;
	adapter->ser_info.dbg_lvl.mac_reg_dump = lvl->mac_reg_dump;

	return MACSUCCESS;
}

u32 mac_trigger_cmac_err(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;
	u16 val16;
	u32 ret;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val8 = MAC_REG_R8(R_AX_CMAC_FUNC_EN);
	MAC_REG_W8(R_AX_CMAC_FUNC_EN, val8 & (~B_AX_TMAC_EN));
	PLTFM_DELAY_MS(1);
	MAC_REG_W8(R_AX_CMAC_FUNC_EN, val8);

	val16 = MAC_REG_R16(R_AX_PTCL_IMR0) | B_AX_F2PCMD_EMPTY_ERR_INT_EN;
	MAC_REG_W16(R_AX_PTCL_IMR0, val16);
	MAC_REG_W16(R_AX_PTCL_IMR0, val16 & ~B_AX_F2PCMD_EMPTY_ERR_INT_EN);

	return MACSUCCESS;
}

u32 mac_trigger_cmac1_err(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;
	u16 val16;
	u32 ret;

	ret = check_mac_en(adapter, MAC_AX_BAND_1, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val8 = MAC_REG_R8(R_AX_CMAC_FUNC_EN_C1);
	MAC_REG_W8(R_AX_CMAC_FUNC_EN_C1, val8 & (~B_AX_TMAC_EN));
	PLTFM_DELAY_MS(1);
	MAC_REG_W8(R_AX_CMAC_FUNC_EN_C1, val8);

	val16 = MAC_REG_R16(R_AX_PTCL_IMR0_C1) | B_AX_F2PCMD_EMPTY_ERR_INT_EN;
	MAC_REG_W16(R_AX_PTCL_IMR0_C1, val16);
	MAC_REG_W16(R_AX_PTCL_IMR0_C1, val16 & ~B_AX_F2PCMD_EMPTY_ERR_INT_EN);

	return MACSUCCESS;
}

u32 mac_trigger_dmac_err(struct mac_ax_adapter *adapter)
{
	struct cpuio_buf_req_t buf_req;
	struct cpuio_ctrl_t ctrl_para;
	u32 ret;

	// Use CPUIO to enqueue packet.
	//WD
	buf_req.len = 0x20;
	ret = mac_dle_buf_req_wd(adapter, &buf_req);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]WDE DLE buf req\n");
		return ret;
	}

	// Enqueue two pkt_id, but only has one pkt_id.
	PLTFM_MEMSET((void *)&ctrl_para, 0, sizeof(ctrl_para));
	ctrl_para.cmd_type = CPUIO_OP_CMD_ENQ_TO_HEAD;
	ctrl_para.start_pktid = buf_req.pktid;
	ctrl_para.end_pktid = buf_req.pktid;
	ctrl_para.pkt_num = 1;
	ctrl_para.dst_pid = WDE_DLE_PORT_ID_WDRLS;
	ctrl_para.dst_qid = 4;
	ret = mac_set_cpuio_wd(adapter, &ctrl_para);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]WDE DLE enqueue to head\n");
		return ret;
	}
	return MACSUCCESS;
}

u32 mac_dump_qta_lost(struct mac_ax_adapter *adapter)
{
	struct dle_dfi_qempty_t qempty;
	struct dle_dfi_quota_t quota;
	struct dle_dfi_ctrl_t ctrl;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, val, not_empty, i;

	qempty.dle_type = DLE_CTRL_TYPE_PLE;
	qempty.grpsel = 0;
	ret = dle_dfi_qempty(adapter, &qempty);
	if (ret == MACSUCCESS)
		PLTFM_MSG_ALWAYS("DLE group0 empty: 0x%x\n", qempty.qempty);
	else
		PLTFM_MSG_ERR("%s: query DLE fail\n", __func__);

	for (not_empty = ~qempty.qempty, i = 0;
	     not_empty != 0; not_empty = not_empty >> 1, i++) {
		if (!(not_empty & BIT(0)))
			continue;
		ctrl.type = DLE_CTRL_TYPE_PLE;
		ctrl.target = DLE_DFI_TYPE_QLNKTBL;
		ctrl.addr = (QLNKTBL_ADDR_INFO_SEL_0 ?
			     QLNKTBL_ADDR_INFO_SEL : 0) |
			SET_WORD(i, QLNKTBL_ADDR_TBL_IDX);
		ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
		if (ret == MACSUCCESS)
			PLTFM_MSG_ALWAYS("qidx%d pktcnt = %d\n", i,
					 GET_FIELD(ctrl.out_data,
						   QLNKTBL_DATA_SEL1_PKT_CNT));
		else
			PLTFM_MSG_ERR("%s: query DLE fail\n", __func__);
	}

	/* cmac0 */
	quota.dle_type = DLE_CTRL_TYPE_PLE;
	quota.qtaid = 6;
	ret = dle_dfi_quota(adapter, &quota);
	if (ret == MACSUCCESS)
		PLTFM_MSG_ALWAYS("quota6 rsv/use: 0x%x/0x%x\n",
				 quota.rsv_pgnum, quota.use_pgnum);
	else
		PLTFM_MSG_ERR("%s: query DLE fail\n", __func__);

	val = MAC_REG_R32(R_AX_PLE_QTA6_CFG);
	PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]min_pgnum=0x%x\n",
			 GET_FIELD(val, B_AX_PLE_Q6_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]max_pgnum=0x%x\n",
			 GET_FIELD(val, B_AX_PLE_Q6_MAX_SIZE));

	val = MAC_REG_R32(R_AX_RX_FLTR_OPT);
	PLTFM_MSG_ALWAYS("[CMAC0_RX]B_AX_RX_MPDU_MAX_LEN=0x%x\n",
			 GET_FIELD(val, B_AX_RX_MPDU_MAX_LEN));

	val = MAC_REG_R32(R_AX_RSP_CHK_SIG);
	PLTFM_MSG_ALWAYS("R_AX_RSP_CHK_SIG=0x%x\n",
			 val);

	val = MAC_REG_R32(R_AX_TRXPTCL_RESP_0);
	PLTFM_MSG_ALWAYS("R_AX_TRXPTCL_RESP_0=0x%x\n",
			 val);

	val = MAC_REG_R32(R_AX_CCA_CONTROL);
	PLTFM_MSG_ALWAYS("R_AX_CCA_CONTROL=0x%x\n",
			 val);

	/* cmac1 */
	ret = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL);

	if (ret == MACSUCCESS) {
		quota.dle_type = DLE_CTRL_TYPE_PLE;
		quota.qtaid = 7;
		ret = dle_dfi_quota(adapter, &quota);
		if (ret == MACSUCCESS)
			PLTFM_MSG_ALWAYS("quota7 rsv/use: 0x%x/0x%x\n",
					 quota.rsv_pgnum, quota.use_pgnum);
		else
			PLTFM_MSG_ERR("%s: query DLE fail\n", __func__);

		val = MAC_REG_R32(R_AX_PLE_QTA7_CFG);
		PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]min_pgnum=0x%x\n",
				 GET_FIELD(val, B_AX_PLE_Q7_MIN_SIZE));
		PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]max_pgnum=0x%x\n",
				 GET_FIELD(val, B_AX_PLE_Q7_MAX_SIZE));

		val = MAC_REG_R32(R_AX_RX_FLTR_OPT_C1);
		PLTFM_MSG_ALWAYS("[CMAC1_RX]B_AX_RX_MPDU_MAX_LEN=0x%x\n",
				 GET_FIELD(val, B_AX_RX_MPDU_MAX_LEN));

		val = MAC_REG_R32(R_AX_RSP_CHK_SIG_C1);
		PLTFM_MSG_ALWAYS("R_AX_RSP_CHK_SIG_C1=0x%x\n",
				 val);

		val = MAC_REG_R32(R_AX_TRXPTCL_RESP_0_C1);
		PLTFM_MSG_ALWAYS("R_AX_TRXPTCL_RESP_0_C1=0x%x\n",
				 val);

		val = MAC_REG_R32(R_AX_CCA_CONTROL_C1);
		PLTFM_MSG_ALWAYS("R_AX_CCA_CONTROL_C1=0x%x\n",
				 val);
	}

	val = MAC_REG_R32(R_AX_DLE_EMPTY0);
	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY0=0x%x\n",
			 val);

	val = MAC_REG_R32(R_AX_DLE_EMPTY1);
	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY1=0x%x\n",
			 val);

	dump_err_status_dispatcher(adapter);

	return MACSUCCESS;
}

u32 mac_dump_l0_to_l1(struct mac_ax_adapter *adapter,
		      enum mac_ax_err_info err)
{
	u32 dbg, event;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ALWAYS("%s\n", __func__);

	dbg = MAC_REG_R32(R_AX_SER_DBG_INFO);
	event = GET_FIELD(dbg, L0_TO_L1_EVENT);

	switch (event) {
	case MAC_AX_L0_TO_L1_RX_QTA_LOST:
		PLTFM_MSG_ALWAYS("quota lost!\n");
		mac_dump_qta_lost(adapter);
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u32 mac_dump_err_status(struct mac_ax_adapter *adapter,
			enum mac_ax_err_info err)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ERR("--->\n err=0x%x\n", err);
	if (err == MAC_AX_ERR_L1_ERR_DMAC ||
	    err == MAC_AX_ERR_L0_PROMOTE_TO_L1 ||
	    err == MAC_AX_ERR_L0_ERR_CMAC0 ||
	    err == MAC_AX_ERR_L0_ERR_CMAC1) {
		PLTFM_MSG_ERR("R_AX_SER_DBG_INFO =0x%08x\n",
			      MAC_REG_R32(R_AX_SER_DBG_INFO));

		dump_err_status_dmac(adapter);
		dump_err_status_cmac(adapter, MAC_AX_BAND_0);
		dump_err_status_cmac(adapter, MAC_AX_BAND_1);

		if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
			if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
				PLTFM_MSG_ERR("R_AX_DBG_ERR_FLAG=0x%08x\n",
					      MAC_REG_R32(R_AX_DBG_ERR_FLAG));
				PLTFM_MSG_ERR("R_AX_LBC_WATCHDOG=0x%08x\n",
					      MAC_REG_R32(R_AX_LBC_WATCHDOG));
			}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
			if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
				PLTFM_MSG_ERR("R_AX_DBG_ERR_FLAG=0x%08x\n",
					      MAC_REG_R32(R_AX_DBG_ERR_FLAG_V1));
				PLTFM_MSG_ERR("R_AX_LBC_WATCHDOG=0x%08x\n",
					      MAC_REG_R32(R_AX_LBC_WATCHDOG_V1));
			}
#endif
		}
		if (err == MAC_AX_ERR_L0_PROMOTE_TO_L1)
			mac_dump_l0_to_l1(adapter, err);
	}
	PLTFM_MSG_ERR("<---\n");

	return MACSUCCESS;
}

u32 mac_set_err_status(struct mac_ax_adapter *adapter,
		       enum mac_ax_err_info err)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt = MAC_SET_ERR_DLY_CNT;
	u32 ret = MACSUCCESS;

	if (err > MAC_AX_SET_ERR_MAX) {
		PLTFM_MSG_ERR("Bad set-err-status value\n");
		return MACFUNCINPUT;
	}

	PLTFM_MUTEX_LOCK(&adapter->hw_info->err_set_lock);

	while (--cnt) {
		if (!MAC_REG_R32(R_AX_HALT_H2C_CTRL))
			break;
		PLTFM_DELAY_US(MAC_SET_ERR_DLY_US);
	}
	if (!cnt) {
		PLTFM_MSG_ERR("FW does not receive previous msg\n");
		ret = MACPOLLTO;
		goto end;
	}

	if (err == MAC_AX_ERR_L1_DISABLE_EN)
		adapter->sm.fw_rst = MAC_AX_FW_RESET_RECV_DONE;

	MAC_REG_W32(R_AX_HALT_H2C, err);
	MAC_REG_W32(R_AX_HALT_H2C_CTRL, B_AX_HALT_H2C_TRIGGER);

end:
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->err_set_lock);
	return ret;
}

u32 mac_get_err_status(struct mac_ax_adapter *adapter,
		       enum mac_ax_err_info *err)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt = MAC_SET_ERR_DLY_CNT;
	u32 ret = MACSUCCESS;
	u32 error_scenario = 0;

	PLTFM_MUTEX_LOCK(&adapter->hw_info->err_get_lock);
	adapter->sm.l2_st = MAC_AX_L2_DIS;

	while (--cnt) {
		if (MAC_REG_R32(R_AX_HALT_C2H_CTRL))
			break;
		PLTFM_DELAY_US(MAC_SET_ERR_DLY_US);
	}
	if (!cnt) {
		PLTFM_MSG_ERR("Polling FW err status fail\n");
		ret = MACPOLLTO;
		goto end;
	}

	*err = (enum mac_ax_err_info)MAC_REG_R32(R_AX_HALT_C2H);
	MAC_REG_W32(R_AX_HALT_C2H_CTRL, 0);
	switch (*err) {
	case MAC_AX_ERR_L1_ERR_DMAC:
	case MAC_AX_ERR_L0_PROMOTE_TO_L1:
		adapter->sm.fw_rst = MAC_AX_FW_RESET_RECV;
		break;
	case MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE:
		adapter->sm.fw_rst = MAC_AX_FW_RESET_PROCESS;
		break;
	case MAC_AX_ERR_L1_RESET_RECOVERY_DONE:
		adapter->sm.fw_rst = MAC_AX_FW_RESET_IDLE;
		break;
	default:
		break;
	}

	/* Decode the error status from halc2h */
	error_scenario = (*err) >> DBG_SENARIO_SH;
	if (!(MAC_REG_R32(R_AX_UDM0) & B_AX_UDM0_DBG_MODE_CTRL)) {
		if (error_scenario == CPU_EXCEPTION)
			*err = MAC_AX_ERR_CPU_EXCEPTION;
		else if (error_scenario == ASSERTION)
			*err = MAC_AX_ERR_ASSERTION;
		else if (error_scenario == RXI300_ERROR)
			*err = MAC_AX_ERR_RXI300;
	}

	//3  3. Execute Recode Normal Debug Register
	if (*err == MAC_AX_ERR_L0_ERR_CMAC0 ||
	    *err == MAC_AX_ERR_L0_ERR_CMAC1) {
		pltfm_dbg_dump(adapter);
	}
	fw_st_dbg_dump(adapter);
	mac_dump_err_status(adapter, *err);
	if (*err == MAC_AX_ERR_L0_ERR_CMAC0 ||
	    *err == MAC_AX_ERR_L0_ERR_CMAC1) {
		pltfm_dbg_dump(adapter);
	}

	//3 4. Execute Recode Share memory debug information
	if (MAC_REG_R32(R_AX_UDM0) & B_AX_UDM0_DBG_MODE_CTRL) {
		/* if debug mode =1 , dump share buffer */
		if (error_scenario) {
			*err = (enum mac_ax_err_info)MAC_AX_DUMP_SHAREBUFF_INDICATOR;
			//notify phl to print share buffer
		}
	}

	//3 5.  dump detailed log for further debug usage.
	mac_dbg_log_dump(adapter);
end:
	adapter->sm.l2_st = MAC_AX_L2_EN;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->err_get_lock);
	return ret;
}

u32 mac_lv1_rcvy(struct mac_ax_adapter *adapter, enum mac_ax_lv1_rcvy_step step)
{
	u32 ret = MACSUCCESS;
#if MAC_AX_PCIE_SUPPORT
	u8 val8;
#endif

	switch (step) {
	case MAC_AX_LV1_RCVY_STEP_1:
		if (adapter->sm.fw_rst != MAC_AX_FW_RESET_RECV) {
			PLTFM_MSG_ERR("The rst-flow state is wrong\n");
			return MACPROCERR;
		}
#if MAC_AX_PCIE_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
			val8 = 0;
			ret = lv1rst_stop_dma_pcie(adapter, val8);
			if (ret) {
				PLTFM_MSG_ERR("lv1 rcvy pcie stop dma fail\n");
				return ret;
			}
		}
	} else {
		// 52C PCIE Stop flow made in FW
	}
#endif

#if MAC_AX_USB_SUPPORT
		if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
			ret = usb_flush_mode(adapter, MAC_AX_FUNC_EN);
			if (ret) {
				PLTFM_MSG_ERR("lv1 rcvy USB flush mode fail\n");
				return ret;
			}
			PLTFM_DELAY_MS(30);
		}
#endif
		break;

	case MAC_AX_LV1_RCVY_STEP_2:
		if (adapter->sm.fw_rst != MAC_AX_FW_RESET_PROCESS) {
			PLTFM_MSG_ERR("The rst-flow state is wrong\n");
			return MACPROCERR;
		}
#if MAC_AX_PCIE_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
				val8 = 0;
				ret = lv1rst_start_dma_pcie(adapter, val8);
				if (ret) {
					PLTFM_MSG_ERR("lv1 rcvy pcie start dma fail\n");
					return ret;
				}
			}
		} else {
			// 52C PCIE Stop flow made in FW
		}
#endif

#if MAC_AX_USB_SUPPORT
		if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
			ret = 0;
			ret = usb_flush_mode(adapter, MAC_AX_FUNC_DIS);
			if (ret) {
				PLTFM_MSG_ERR("lv1 rcvy USB norm mode fail\n");
				return ret;
			}
		}
#endif
		break;

	default:
		return MACLV1STEPERR;
	}

	return ret;
}

u32 mac_err_imr_ctrl(struct mac_ax_adapter *adapter, enum mac_ax_func_sw sw)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 v32_dmac, v32_cmac0, v32_cmac1;
	u32 ret = MACSUCCESS;
	u8 is_dbcc;

	v32_dmac = sw != MAC_AX_FUNC_DIS ? DMAC_ERR_IMR_EN : DMAC_ERR_IMR_DIS;
	v32_cmac0 = sw != MAC_AX_FUNC_DIS ? CMAC0_ERR_IMR_EN : CMAC0_ERR_IMR_DIS;
	v32_cmac1 = sw != MAC_AX_FUNC_DIS ? CMAC1_ERR_IMR_EN : CMAC1_ERR_IMR_DIS;
	is_dbcc = is_curr_dbcc(adapter);

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)R_AX_DMAC_ERR_IMR, DMAC_ERR_IMR_MASK,
				     v32_dmac, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("dmac err imr w ofld fail\n");
			return ret;
		}

		ret = MAC_REG_W_OFLD((u16)R_AX_CMAC_ERR_IMR, CMAC0_ERR_IMR_MASK,
				     v32_cmac0, (is_dbcc ? 0 : 1));
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("cmac0 err imr w ofld fail\n");
			return ret;
		}

		if (is_dbcc) {
			ret = MAC_REG_W_OFLD((u16)R_AX_CMAC_ERR_IMR_C1,
					     CMAC1_ERR_IMR_MASK,
					     v32_cmac1, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("cmac1 err imr w ofld fail\n");
				return ret;
			}
		}

		return ret;
	}
#endif

	MAC_REG_W32(R_AX_DMAC_ERR_IMR, v32_dmac);
	MAC_REG_W32(R_AX_CMAC_ERR_IMR, v32_cmac0);

	if (is_dbcc)
		MAC_REG_W32(R_AX_CMAC_ERR_IMR_C1, v32_cmac1);

	return ret;
}

static void dump_err_status_dispatcher(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ERR("R_AX_HOST_DISPATCHER_ERR_IMR=0x%08x\n",
		      MAC_REG_R32(R_AX_HOST_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ERR("R_AX_HOST_DISPATCHER_ERR_ISR=0x%08x\n",
		      MAC_REG_R32(R_AX_HOST_DISPATCHER_ERR_ISR));

	PLTFM_MSG_ERR("R_AX_CPU_DISPATCHER_ERR_IMR=0x%08x\n",
		      MAC_REG_R32(R_AX_CPU_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ERR("R_AX_CPU_DISPATCHER_ERR_ISR=0x%08x\n",
		      MAC_REG_R32(R_AX_CPU_DISPATCHER_ERR_ISR));
	PLTFM_MSG_ERR("R_AX_OTHER_DISPATCHER_ERR_IMR=0x%08x ",
		      MAC_REG_R32(R_AX_OTHER_DISPATCHER_ERR_IMR));
	PLTFM_MSG_ERR("R_AX_OTHER_DISPATCHER_ERR_ISR=0x%08x\n",
		      MAC_REG_R32(R_AX_OTHER_DISPATCHER_ERR_ISR));
}

static void dump_err_status_dmac(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 dmac_err;

	dmac_err = MAC_REG_R32(R_AX_DMAC_ERR_ISR);
	PLTFM_MSG_ERR("R_AX_DMAC_ERR_ISR =0x%08x\n", dmac_err);
	PLTFM_MSG_ALWAYS("R_AX_DMAC_ERR_IMR=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_ERR_IMR));

	PLTFM_MSG_ERR("R_AX_DMAC_FUNC_EN =0x%08x\n",
		      MAC_REG_R32(R_AX_DMAC_FUNC_EN));
	PLTFM_MSG_ERR("R_AX_DMAC_CLK_EN =0x%08x\n",
		      MAC_REG_R32(R_AX_DMAC_CLK_EN));

	if (dmac_err) {
		PLTFM_MSG_ERR("R_AX_WDE_ERR_FLAG_CFG =0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERR_FLAG_CFG_NUM1));
		PLTFM_MSG_ERR("R_AX_PLE_ERR_FLAG_CFG =0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERR_FLAG_CFG_NUM1));
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		PLTFM_MSG_ERR("R_AX_PLE_ERRFLAG_MSG =0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERRFLAG_MSG));
		PLTFM_MSG_ERR("R_AX_PLE_ERRFLAG_MSG 0x9030 Reg dump =0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERRFLAG_MSG));
		PLTFM_MSG_ERR("R_AX_WDE_ERRFLAG_MSG 0x8C30 Reg dump =0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERRFLAG_MSG));
		PLTFM_MSG_ERR("R_AX_PLE_DBGERR_LOCKEN 0x9020 Reg dump =0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_DBGERR_LOCKEN));
		PLTFM_MSG_ERR("R_AX_PLE_DBGERR_STS 0x9024 Reg dump =0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_DBGERR_STS));
#endif
	}

	if (dmac_err & B_AX_WDRLS_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_WDRLS_ERR_IMR =0x%08x\n",
			      MAC_REG_R32(R_AX_WDRLS_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_WDRLS_ERR_ISR =0x%08x\n",
			      MAC_REG_R32(R_AX_WDRLS_ERR_ISR));
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_RPQ_RXBD_IDX =0x%08x\n",
				      MAC_REG_R32(R_AX_RPQ_RXBD_IDX));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_RPQ_RXBD_IDX =0x%08x\n",
				      MAC_REG_R32(R_AX_RPQ_RXBD_IDX_V1));
		}
#endif
	}

	if (dmac_err & B_AX_WSEC_ERR_FLAG) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_SEC_ERR_IMR_ISR =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_DEBUG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D00 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_ENG_CTRL));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D04 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_MPDU_PROC));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D10 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_CAM_ACCESS));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D14 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_CAM_RDATA));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D18 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_CAM_WDATA));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D20 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_TX_DEBUG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D24 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_RX_DEBUG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D28 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_TRX_PKT_CNT));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D2C =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_TRX_BLK_CNT));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		u32 val32 = 0, i = 0;

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_SEC_ERR_IMR =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_ERROR_FLAG_IMR));
			PLTFM_MSG_ERR("R_AX_SEC_ERR_ISR =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_ERROR_FLAG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D00 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_ENG_CTRL));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D04 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_MPDU_PROC));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D10 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_CAM_ACCESS));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D14 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_CAM_RDATA));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D1C =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_DEBUG1));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D20 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_TX_DEBUG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D24 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_RX_DEBUG));
			PLTFM_MSG_ERR("SEC_local_Register 0x9D28 =0x%08x\n",
				      MAC_REG_R32(R_AX_SEC_DEBUG2));

			val32 = MAC_REG_R32(R_AX_DBG_CTRL);
			val32 = SET_CLR_WORD(val32, SEC_DBG_SEL, B_AX_DBG_SEL0);
			val32 = SET_CLR_WORD(val32, SEC_DBG_SEL, B_AX_DBG_SEL1);
			MAC_REG_W32(R_AX_DBG_CTRL, val32);

			val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
			val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
			MAC_REG_W32(R_AX_SYS_STATUS1, val32);
			PLTFM_MSG_ALWAYS("SECURITY ENGINE DEBUG PORT DUMP\n");
			for (i = 0; i < SEC_DBG_PORT_NUM; i++) {
				val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
				val32 = SET_CLR_WORD(val32, i, SEC_DBG_PORT_FIELD);
				MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);
				PLTFM_MSG_ERR("debug sel = %x, 0x9D28 =0x%08x\n",
					      i, MAC_REG_R32(R_AX_SEC_DEBUG2));
			}
		}
#endif
	}

	if (dmac_err & B_AX_MPDU_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_MPDU_TX_ERR_IMR =0x%08x\n",
			      MAC_REG_R32(R_AX_MPDU_TX_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_MPDU_TX_ERR_ISR =0x%08x\n",
			      MAC_REG_R32(R_AX_MPDU_TX_ERR_ISR));
		PLTFM_MSG_ERR("R_AX_MPDU_RX_ERR_IMR =0x%08x\n",
			      MAC_REG_R32(R_AX_MPDU_RX_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_MPDU_RX_ERR_ISR =0x%08x\n",
			      MAC_REG_R32(R_AX_MPDU_RX_ERR_ISR));
	}

	if (dmac_err & B_AX_STA_SCHEDULER_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_STA_SCHEDULER_ERR_IMR =0x%08x\n",
			      MAC_REG_R32(R_AX_STA_SCHEDULER_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_STA_SCHEDULER_ERR_ISR= 0x%08x\n",
			      MAC_REG_R32(R_AX_STA_SCHEDULER_ERR_ISR));
	}

	if (dmac_err & B_AX_WDE_DLE_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_WDE_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_WDE_ERR_ISR=0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERR_ISR));
		PLTFM_MSG_ERR("R_AX_PLE_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_PLE_ERR_FLAG_ISR=0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERR_FLAG_ISR));
	}

	if (dmac_err & B_AX_TXPKTCTRL_ERR_FLAG) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_ERR_IMR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_ERR_IMR_ISR));
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_ERR_IMR_ISR_B1=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_ERR_IMR_ISR_B1));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_B0_ERRFLAG_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_B0_ERRFLAG_IMR));
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_B0_ERRFLAG_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_B0_ERRFLAG_ISR));
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_B1_ERRFLAG_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_B1_ERRFLAG_IMR));
			PLTFM_MSG_ERR("R_AX_TXPKTCTL_B1_ERRFLAG_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_TXPKTCTL_B1_ERRFLAG_ISR));
		}
#endif
	}

	if (dmac_err & B_AX_PLE_DLE_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_WDE_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_WDE_ERR_ISR=0x%08x\n",
			      MAC_REG_R32(R_AX_WDE_ERR_ISR));
		PLTFM_MSG_ERR("R_AX_PLE_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_PLE_ERR_FLAG_ISR=0x%08x\n",
			      MAC_REG_R32(R_AX_PLE_ERR_FLAG_ISR));
		PLTFM_MSG_ERR("R_AX_WD_CPUQ_OP_0=0x%08x\n",
			      MAC_REG_R32(R_AX_WD_CPUQ_OP_0));
		PLTFM_MSG_ERR("R_AX_WD_CPUQ_OP_1=0x%08x\n",
			      MAC_REG_R32(R_AX_WD_CPUQ_OP_1));
		PLTFM_MSG_ERR("R_AX_WD_CPUQ_OP_2=0x%08x\n",
			      MAC_REG_R32(R_AX_WD_CPUQ_OP_2));
		PLTFM_MSG_ERR("R_AX_WD_CPUQ_OP_STATUS=0x%08x\n",
			      MAC_REG_R32(R_AX_WD_CPUQ_OP_STATUS));
		PLTFM_MSG_ERR("R_AX_PL_CPUQ_OP_0=0x%08x\n",
			      MAC_REG_R32(R_AX_PL_CPUQ_OP_0));
		PLTFM_MSG_ERR("R_AX_PL_CPUQ_OP_1=0x%08x\n",
			      MAC_REG_R32(R_AX_PL_CPUQ_OP_1));
		PLTFM_MSG_ERR("R_AX_PL_CPUQ_OP_2=0x%08x\n",
			      MAC_REG_R32(R_AX_PL_CPUQ_OP_2));
		PLTFM_MSG_ERR("R_AX_PL_CPUQ_OP_STATUS=0x%08x\n",
			      MAC_REG_R32(R_AX_PL_CPUQ_OP_STATUS));
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_RXDMA_PKT_INFO_0=0x%08x\n",
				      MAC_REG_R32(R_AX_RXDMA_PKT_INFO_0));
			PLTFM_MSG_ERR("R_AX_RXDMA_PKT_INFO_1=0x%08x\n",
				      MAC_REG_R32(R_AX_RXDMA_PKT_INFO_1));
			PLTFM_MSG_ERR("R_AX_RXDMA_PKT_INFO_2=0x%08x\n",
				      MAC_REG_R32(R_AX_RXDMA_PKT_INFO_2));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_RX_CTRL0=0x%08x\n",
				      MAC_REG_R32(R_AX_RX_CTRL0));
			PLTFM_MSG_ERR("R_AX_RX_CTRL1=0x%08x\n",
				      MAC_REG_R32(R_AX_RX_CTRL1));
			PLTFM_MSG_ERR("R_AX_RX_CTRL2=0x%08x\n",
				      MAC_REG_R32(R_AX_RX_CTRL2));
		}
#endif
		dump_err_status_dispatcher(adapter);
	}

	if (dmac_err & B_AX_PKTIN_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_PKTIN_ERR_IMR =0x%08x\n",
			      MAC_REG_R32(R_AX_PKTIN_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_PKTIN_ERR_ISR =0x%08x\n",
			      MAC_REG_R32(R_AX_PKTIN_ERR_ISR));
		PLTFM_MSG_ERR("R_AX_PKTIN_ERR_IMR =0x%08x ",
			      MAC_REG_R32(R_AX_PKTIN_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_PKTIN_ERR_ISR =0x%08x\n",
			      MAC_REG_R32(R_AX_PKTIN_ERR_ISR));
	}

	if (dmac_err & B_AX_DISPATCH_ERR_FLAG)
		dump_err_status_dispatcher(adapter);

	if (dmac_err & B_AX_DLE_CPUIO_ERR_FLAG) {
		PLTFM_MSG_ERR("R_AX_CPUIO_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(R_AX_CPUIO_ERR_IMR));
		PLTFM_MSG_ERR("R_AX_CPUIO_ERR_ISR=0x%08x\n",
			      MAC_REG_R32(R_AX_CPUIO_ERR_ISR));
	}

	if (dmac_err & B_AX_BBRPT_ERR_FLAG) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_BBRPT_COM_ERR_IMR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_COM_ERR_IMR_ISR));
			PLTFM_MSG_ERR("R_AX_BBRPT_CHINFO_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_ISR));
			PLTFM_MSG_ERR("R_AX_BBRPT_CHINFO_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_IMR));
			PLTFM_MSG_ERR("R_AX_BBRPT_DFS_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_DFS_ERR_IMR));
			PLTFM_MSG_ERR("R_AX_BBRPT_DFS_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_DFS_ERR_ISR));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_BBRPT_COM_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_COM_ERR_IMR));
			PLTFM_MSG_ERR("R_AX_BBRPT_COM_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_COM_ERR_ISR));
			PLTFM_MSG_ERR("R_AX_BBRPT_CHINFO_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_ISR));
			PLTFM_MSG_ERR("R_AX_BBRPT_CHINFO_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_IMR));
			PLTFM_MSG_ERR("R_AX_BBRPT_DFS_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_DFS_ERR_IMR));
			PLTFM_MSG_ERR("R_AX_BBRPT_DFS_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_BBRPT_DFS_ERR_ISR));
		}
#endif
	}

	if (dmac_err & B_AX_HAXIDMA_ERR_FLAG) {
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_HAXIDMA_ERR_IMR=0x%08x\n",
				      MAC_REG_R32(R_AX_HAXI_IDCT_MSK));
			PLTFM_MSG_ERR("R_AX_HAXIDMA_ERR_ISR=0x%08x\n",
				      MAC_REG_R32(R_AX_HAXI_IDCT));
		}
#endif
	}
}

static void dump_err_status_cmac(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cmac_err;
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return;

	PLTFM_MSG_ERR("CMAC Band =0x%02x\n", band);

	cmac_err = MAC_REG_R32(band == MAC_AX_BAND_0 ? R_AX_CMAC_ERR_ISR :
			       R_AX_CMAC_ERR_ISR_C1);
	PLTFM_MSG_ERR("R_AX_CMAC_ERR_ISR =0x%08x\n", cmac_err);

	PLTFM_MSG_ERR("R_AX_CMAC_FUNC_EN =0x%08x\n",
		      MAC_REG_R32(band == MAC_AX_BAND_0 ? R_AX_CMAC_FUNC_EN :
		      R_AX_CMAC_FUNC_EN_C1));
	PLTFM_MSG_ERR("R_AX_CK_EN =0x%08x\n",
		      MAC_REG_R32(band == MAC_AX_BAND_0 ? R_AX_CK_EN :
		      R_AX_CK_EN_C1));

	if (cmac_err & B_AX_SCHEDULE_TOP_ERR_IND) {
		PLTFM_MSG_ERR("R_AX_SCHEDULE_ERR_IMR=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_SCHEDULE_ERR_IMR : R_AX_SCHEDULE_ERR_IMR_C1));
		PLTFM_MSG_ERR("R_AX_SCHEDULE_ERR_ISR=0x%04x\n",
			      MAC_REG_R16(band == MAC_AX_BAND_0 ?
			      R_AX_SCHEDULE_ERR_ISR : R_AX_SCHEDULE_ERR_ISR_C1));
	}

	if (cmac_err & B_AX_PTCL_TOP_ERR_IND) {
		PLTFM_MSG_ERR("R_AX_PTCL_IMR0=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_PTCL_IMR0 : R_AX_PTCL_IMR0_C1));
		PLTFM_MSG_ERR("R_AX_PTCL_ISR0=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_PTCL_ISR0 : R_AX_PTCL_ISR0_C1));
	}

	if (cmac_err & B_AX_DMA_TOP_ERR_IND) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_DLE_CTRL=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_DLE_CTRL : R_AX_DLE_CTRL_C1));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_RX_ERR_FLAG=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_RX_ERR_FLAG : R_AX_RX_ERR_FLAG_C1));
			PLTFM_MSG_ERR("R_AX_RX_ERR_FLAG_IMR=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_RX_ERR_FLAG_IMR : R_AX_RX_ERR_FLAG_IMR_C1));
		}
#endif
	}

	if (cmac_err & B_AX_PHYINTF_ERR_IND) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_IMR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_IMR :
				      R_AX_PHYINFO_ERR_IMR_C1));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_ISR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_ISR :
				      R_AX_PHYINFO_ERR_ISR_C1));
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_IMR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_IMR_V1 :
				      R_AX_PHYINFO_ERR_IMR_V1_C1));
		}
#endif
	}

	if (cmac_err & B_AX_TXPWR_CTRL_ERR_IND) {
		PLTFM_MSG_ERR("R_AX_TXPWR_IMR=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_TXPWR_IMR : R_AX_TXPWR_IMR_C1));
		PLTFM_MSG_ERR("R_AX_TXPWR_ISR=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_TXPWR_ISR : R_AX_TXPWR_ISR_C1));
	}

	if (cmac_err & B_AX_WMAC_RX_ERR_IND) {
		PLTFM_MSG_ERR("R_AX_DBGSEL_TRXPTCL=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_DBGSEL_TRXPTCL : R_AX_DBGSEL_TRXPTCL_C1));
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_IMR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_IMR :
				      R_AX_PHYINFO_ERR_IMR_C1));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_ISR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_ISR :
				      R_AX_PHYINFO_ERR_ISR_C1));
			PLTFM_MSG_ERR("R_AX_PHYINFO_ERR_IMR=0x%04x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_PHYINFO_ERR_IMR_V1 :
				      R_AX_PHYINFO_ERR_IMR_V1_C1));
		}
#endif
	}

	if (cmac_err & B_AX_WMAC_TX_ERR_IND) {
#if (MAC_AX_8852A_SUPPORT) || (MAC_AX_8852B_SUPPORT) || (MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ERR("R_AX_TMAC_ERR_IMR_ISR=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_TMAC_ERR_IMR_ISR :
				      R_AX_TMAC_ERR_IMR_ISR_C1));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ERR("R_AX_TRXPTCL_ERROR_INDICA=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_TRXPTCL_ERROR_INDICA :
				      R_AX_TRXPTCL_ERROR_INDICA_C1));
			PLTFM_MSG_ERR("R_AX_TRXPTCL_ERROR_INDICA_MASK=0x%08x\n",
				      MAC_REG_R32(band == MAC_AX_BAND_0 ?
				      R_AX_TRXPTCL_ERROR_INDICA_MASK :
				      R_AX_TRXPTCL_ERROR_INDICA_MASK_C1));
		}
#endif
		PLTFM_MSG_ERR("R_AX_DBGSEL_TRXPTCL=0x%08x\n",
			      MAC_REG_R32(band == MAC_AX_BAND_0 ?
			      R_AX_DBGSEL_TRXPTCL : R_AX_DBGSEL_TRXPTCL_C1));
	}

	cmac_err = MAC_REG_R32(band == MAC_AX_BAND_1 ? R_AX_CMAC_ERR_IMR_C1 :
			       R_AX_CMAC_ERR_IMR);
	PLTFM_MSG_ALWAYS("B%d R_AX_CMAC_ERR_IMR=0x%x\n", band, cmac_err);
}

u32 mac_ser_ctrl(struct mac_ax_adapter *adapter, enum mac_ax_func_sw sw)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum mac_ax_err_info err_info;
	u32 val32, ret, cnt;

	if (sw == MAC_AX_FUNC_EN) {
		err_info = MAC_AX_ERR_L1_RCVY_START_REQ;
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_SRT;
	} else if (sw == MAC_AX_FUNC_DIS) {
		err_info = MAC_AX_ERR_L1_RCVY_STOP_REQ;
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_STOP;
	} else if (sw == MAC_AX_FUNC_WO_RECVY_DIS) {
		err_info = MAC_AX_ERR_L1_RCVY_STOP_WO_RECVY_REQ;
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_SRT;
	} else if (sw == MAC_AX_FUNC_WO_RECVY_EN) {
		err_info = MAC_AX_ERR_L1_RCVY_START_WO_RECVY_REQ;
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_STOP;
	} else {
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_ERR;
		PLTFM_MSG_ERR("[ERR]SER ctrl input err %d\n", sw);
		return MACFUNCINPUT;
	}

	ret = mac_set_err_status(adapter, err_info);
	if (ret != MACSUCCESS) {
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_ERR;
		PLTFM_MSG_ERR("[ERR]set err for stop ser %d\n", ret);
		return ret;
	}

	cnt = MAC_SET_ERR_DLY_CNT;
	while (cnt) {
		val32 = MAC_REG_R32(R_AX_HALT_H2C_CTRL);
		if (!(val32 & B_AX_HALT_H2C_TRIGGER))
			break;
		PLTFM_DELAY_US(MAC_SET_ERR_DLY_US);
		cnt--;
	}

	if (!cnt) {
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_ERR;
		PLTFM_MSG_ERR("[ERR]FW not handle haltH2C req\n");
		ret = MACPOLLTO;
		return ret;
	}

	if (sw == MAC_AX_FUNC_EN || sw == MAC_AX_FUNC_WO_RECVY_DIS ||
	    sw == MAC_AX_FUNC_WO_RECVY_EN)
		return MACSUCCESS;

	cnt = MAC_SER_STOP_DLY_CNT;
	while (cnt) {
		PLTFM_DELAY_US(MAC_SER_STOP_DLY_US);
		val32 = MAC_REG_R32(R_AX_UDM0);
		val32 = GET_FIELD(val32, FW_ST);
		if (val32 != FW_ST_ERR_IN)
			break;
		cnt--;
	}

	if (!cnt) {
		adapter->sm.ser_ctrl_st = MAC_AX_SER_CTRL_ERR;
		PLTFM_MSG_ERR("[ERR]stop ser polling FW ST timeout\n");
		return MACPOLLTO;
	}

	return ret;
}

u32 mac_chk_err_status(struct mac_ax_adapter *adapter, u8 *ser_status)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt = MAC_SET_ERR_DLY_CNT;
	u32 ret = MACSUCCESS;
	enum mac_ax_err_info err;

	PLTFM_MUTEX_LOCK(&adapter->hw_info->err_get_lock);
	adapter->sm.l2_st = MAC_AX_L2_DIS;

	while (--cnt) {
		if (MAC_REG_R32(R_AX_HALT_C2H_CTRL))
			break;
		PLTFM_DELAY_US(MAC_SET_ERR_DLY_US);
	}
	if (!cnt) {
		PLTFM_MSG_ERR("Polling FW err status fail\n");
		ret = MACPOLLTO;
		goto end;
	}

	err = (enum mac_ax_err_info)MAC_REG_R32(R_AX_HALT_C2H);

	switch (err) {
	case MAC_AX_ERR_L1_ERR_DMAC:
	case MAC_AX_ERR_L0_PROMOTE_TO_L1:
		*ser_status = MAC_AX_L1_TRUE;
		break;
	default:
		*ser_status = MAC_AX_L1_FALSE;
		break;
	}

end:
	adapter->sm.l2_st = MAC_AX_L2_EN;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->err_get_lock);
	return ret;
}

u32 mac_dump_ser_cnt(struct mac_ax_adapter *adapter, struct mac_ser_status *status)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_SER_DBG_INFO);
	status->l0_cnt = GET_FIELD(val32, B_AX_SER_L0_COUNTER);
	status->l1_cnt = GET_FIELD(val32, B_AX_SER_L1_COUNTER);
	status->l0_pro_event = GET_FIELD(val32, B_AX_SER_L0_PROMOTE_L1_EVENT);
	status->rmac_ppdu_hang_cnt = GET_FIELD(val32, B_AX_RMAC_PPDU_HANG_CNT);

	return MACSUCCESS;
}

