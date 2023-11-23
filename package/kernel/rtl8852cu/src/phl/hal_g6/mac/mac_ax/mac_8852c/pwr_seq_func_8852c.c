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
 * 8852C V008
 ******************************************************************************/

#include "../pwr.h"
#include "../pwr_seq_func.h"
#if MAC_AX_8852C_SUPPORT

#define MAC_AX_HCI_SEL_SDIO_UART 0
#define MAC_AX_HCI_SEL_MULTI_USB 1
#define MAC_AX_HCI_SEL_PCIE_UART 2
#define MAC_AX_HCI_SEL_PCIE_USB 3
#define MAC_AX_HCI_SEL_MULTI_SDIO 4

u32 mac_pwr_on_sdio_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, hci_sel;
	u32 ret;
	u8 val8;

	/* 0x4086[0] = 0 == 0x4084[16] = 0 */
	val32 = MAC_REG_R32(R_AX_SDIO_BUS_CTRL_V1);
	MAC_REG_W32(R_AX_SDIO_BUS_CTRL_V1, val32 & ~B_AX_HCI_SUS_REQ);

	/* polling 0x4086[1] = 1 */
	ret = pwr_poll_u32(adapter, R_AX_SDIO_BUS_CTRL_V1, B_AX_HCI_RESUME_RDY,
			   B_AX_HCI_RESUME_RDY);
	if (ret)
		return ret;

	val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
	hci_sel = GET_FIELD(val32, B_AX_PAD_HCI_SEL_V2);
	if (hci_sel == MAC_AX_HCI_SEL_MULTI_SDIO) {
		/* 0x218[16] = 1 */
		val32 = MAC_REG_R32(R_AX_LDO_AON_CTRL0);
		MAC_REG_W32(R_AX_LDO_AON_CTRL0, val32 | B_AX_PD_REGU_L);
	}

	/* 0x04[12:11] = 2'b00 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~(B_AX_AFSM_WLSUS_EN |
			B_AX_AFSM_PCIE_SUS_EN));

	/* 0x04[18] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_DIS_WLBT_PDNSUSEN_SOPC);

	/* 0x90[1] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_DIS_WLBT_LPSEN_LOPC);

	/* 0x04[15] = 0 */
	//val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	//MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APDM_HPDN);

	/* 0x04[10] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APFM_SWLPS);

	/* 0x200[15:13] = 3'b111*/
	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x7, B_AX_OCP_L1);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* polling 0x04[17] = 1*/
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_RDY_SYSPWR,
			   B_AX_RDY_SYSPWR);
	if (ret)
		return ret;

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x04[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFN_ONMAC);

	/* polling 0x04[8] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFN_ONMAC, 0);
	if (ret)
		return ret;

	/* reset platform twice : 0x88[0] = 1->0->1->0 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	/* 0x88[0] = 1 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	/* CMAC1 Power off */
	/* 0x80[30] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 & ~B_AX_CMAC1_FEN);

	/* 0x80[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_CMAC12PP);

	/* 0x24[4:0] = 0 */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1,
		    val32 & ~B_AX_R_SYM_WLCMAC1_P4_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P3_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P2_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P1_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_PC_EN);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x24[6:4] = 3'b001 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_2, 0x10, 0x70);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x26[3:0] = 4'b0000 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_4, 0, 0x0F);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0xCC[2] = 1 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 | B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* 0x00[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 | B_AX_ISO_EB2CORE);

	/* 0x00[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(15)));

	PLTFM_DELAY_MS(1);

	/* 0x00[14] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(14)));

	/* 0xCC[2] = 0 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 & ~B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	adapter->sm.pwr = MAC_AX_PWR_ON;
	adapter->sm.plat = MAC_AX_PLAT_ON;
	adapter->sm.io_st = MAC_AX_IO_ST_NORM;

	/*enable dmac , 0x8400*/
	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN,
		    val32 | B_AX_MAC_FUNC_EN |
			B_AX_DMAC_FUNC_EN |
			B_AX_MPDU_PROC_EN |
			B_AX_WD_RLS_EN |
			B_AX_DLE_WDE_EN |
			B_AX_TXPKT_CTRL_EN |
			B_AX_STA_SCH_EN |
			B_AX_DLE_PLE_EN |
			B_AX_PKT_BUF_EN |
			B_AX_DMAC_TBL_EN |
			B_AX_PKT_IN_EN |
			B_AX_DLE_CPUIO_EN |
			B_AX_DISPATCHER_EN |
			B_AX_BBRPT_EN |
			B_AX_MAC_SEC_EN |
			B_AX_DMACREG_GCKEN);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	/*enable cmac , 0xC000*/
	val32 = MAC_REG_R32(R_AX_CMAC_FUNC_EN);
	MAC_REG_W32(R_AX_CMAC_FUNC_EN,
		    val32 | B_AX_CMAC_EN |
			B_AX_CMAC_TXEN |
			B_AX_CMAC_RXEN |
			B_AX_FORCE_CMACREG_GCKEN |
			B_AX_PHYINTF_EN |
			B_AX_CMAC_DMA_EN |
			B_AX_PTCLTOP_EN |
			B_AX_SCHEDULER_EN |
			B_AX_TMAC_EN |
			B_AX_RMAC_EN);

	adapter->sm.cmac0_func = MAC_AX_FUNC_ON;

	/* 0x2DC[27:24] = 4'b0001 */
	val32 = MAC_REG_R32(R_AX_LED1_FUNC_SEL);
	SET_CLR_WORD(val32, 0x1, B_AX_PINMUX_EESK_FUNC_SEL_V1);
	MAC_REG_W32(R_AX_LED1_FUNC_SEL, val32);

	return MACSUCCESS;
}

u32 mac_pwr_on_usb_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u8 val8;

	/* 0x218[16] = 1 */
	val32 = MAC_REG_R32(R_AX_LDO_AON_CTRL0);
	MAC_REG_W32(R_AX_LDO_AON_CTRL0, val32 | B_AX_PD_REGU_L);

	/* 0x04[12:11] = 2'b00 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~(B_AX_AFSM_WLSUS_EN |
			B_AX_AFSM_PCIE_SUS_EN));

	/* 0x04[18] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_DIS_WLBT_PDNSUSEN_SOPC);

	/* 0x90[1] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_DIS_WLBT_LPSEN_LOPC);

	/* 0x04[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APDM_HPDN);

	/* 0x04[10] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APFM_SWLPS);

	/* 0x200[15:13] = 3'b111*/
	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x7, B_AX_OCP_L1);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* polling 0x04[17] = 1*/
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_RDY_SYSPWR,
			   B_AX_RDY_SYSPWR);
	if (ret)
		return ret;

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x04[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFN_ONMAC);

	/* polling 0x04[8] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFN_ONMAC, 0);
	if (ret)
		return ret;

	/* reset platform twice : 0x88[0] = 1->0->1->0 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	/* 0x88[0] = 1 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	/* CMAC1 Power off */
	/* 0x80[30] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 & ~B_AX_CMAC1_FEN);

	/* 0x80[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_CMAC12PP);

	/* 0x24[4:0] = 0 */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1,
		    val32 & ~B_AX_R_SYM_WLCMAC1_P4_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P3_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P2_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P1_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_PC_EN);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x24[6:4] = 3'b001 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_2, 0x10, 0x70);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x26[3:0] = 4'b0000 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_4, 0, 0x0F);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0xCC[2] = 1 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 | B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* 0x00[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 | B_AX_ISO_EB2CORE);

	/* 0x00[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(15)));

	PLTFM_DELAY_MS(1);

	/* 0x00[14] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(14)));

	/* 0xCC[2] = 0 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 & ~B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	adapter->sm.pwr = MAC_AX_PWR_ON;
	adapter->sm.plat = MAC_AX_PLAT_ON;
	adapter->sm.io_st = MAC_AX_IO_ST_NORM;

	/*enable dmac , 0x8400*/
	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN,
		    val32 | B_AX_MAC_FUNC_EN |
			B_AX_DMAC_FUNC_EN |
			B_AX_MPDU_PROC_EN |
			B_AX_WD_RLS_EN |
			B_AX_DLE_WDE_EN |
			B_AX_TXPKT_CTRL_EN |
			B_AX_STA_SCH_EN |
			B_AX_DLE_PLE_EN |
			B_AX_PKT_BUF_EN |
			B_AX_DMAC_TBL_EN |
			B_AX_PKT_IN_EN |
			B_AX_DLE_CPUIO_EN |
			B_AX_DISPATCHER_EN |
			B_AX_BBRPT_EN |
			B_AX_MAC_SEC_EN |
			B_AX_DMACREG_GCKEN);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	/*enable cmac , 0xC000*/
	val32 = MAC_REG_R32(R_AX_CMAC_FUNC_EN);
	MAC_REG_W32(R_AX_CMAC_FUNC_EN,
		    val32 | B_AX_CMAC_EN |
			B_AX_CMAC_TXEN |
			B_AX_CMAC_RXEN |
			B_AX_FORCE_CMACREG_GCKEN |
			B_AX_PHYINTF_EN |
			B_AX_CMAC_DMA_EN |
			B_AX_PTCLTOP_EN |
			B_AX_SCHEDULER_EN |
			B_AX_TMAC_EN |
			B_AX_RMAC_EN);

	adapter->sm.cmac0_func = MAC_AX_FUNC_ON;

	/* 0x2DC[27:24] = 4'b0001 */
	val32 = MAC_REG_R32(R_AX_LED1_FUNC_SEL);
	SET_CLR_WORD(val32, 0x1, B_AX_PINMUX_EESK_FUNC_SEL_V1);
	MAC_REG_W32(R_AX_LED1_FUNC_SEL, val32);

	return MACSUCCESS;
}

u32 mac_pwr_on_ap_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u8 val8;

	/* 0x218[16] = 1 */
	val32 = MAC_REG_R32(R_AX_LDO_AON_CTRL0);
	MAC_REG_W32(R_AX_LDO_AON_CTRL0, val32 | B_AX_PD_REGU_L);

	/* 0x04[12:11] = 2'b00 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~(B_AX_AFSM_WLSUS_EN |
			B_AX_AFSM_PCIE_SUS_EN));

	/* 0x04[18] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_DIS_WLBT_PDNSUSEN_SOPC);

	/* 0x90[1] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_DIS_WLBT_LPSEN_LOPC);

	/* 0x04[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APDM_HPDN);

	/* 0x04[10] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APFM_SWLPS);

	/* 0x200[15:13] = 3'b111*/
	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x7, B_AX_OCP_L1);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* polling 0x04[17] = 1*/
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_RDY_SYSPWR,
			   B_AX_RDY_SYSPWR);
	if (ret)
		return ret;

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x04[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFN_ONMAC);

	/* polling 0x04[8] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFN_ONMAC, 0);
	if (ret)
		return ret;

	/* reset platform twice : 0x88[0] = 1->0->1->0 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	/* 0x88[0] = 1 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	/* 0x70[12] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_SDIO_CTRL);
	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, val32 & ~B_AX_PCIE_CALIB_EN);

	/* CMAC1 Power off */
	/* 0x80[30] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 & ~B_AX_CMAC1_FEN);

	/* 0x80[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_CMAC12PP);

	/* 0x24[4:0] = 0 */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1,
		    val32 & ~B_AX_R_SYM_WLCMAC1_P4_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P3_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P2_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P1_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_PC_EN);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x24[6:4] = 3'b001 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_2, 0x10, 0x70);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x26[3:0] = 4'b0000 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_4, 0, 0x0F);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0xCC[2] = 1 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 | B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* 0x00[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 | B_AX_ISO_EB2CORE);

	/* 0x00[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(15)));

	PLTFM_DELAY_MS(1);

	/* 0x00[14] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(14)));

	/* 0xCC[2] = 0 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 & ~B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* enable GPIO16 GPIO17 GPIO18 internal weakly pull low */
	val32 = MAC_REG_R32(R_AX_GPIO0_15_EECS_EESK_LED1_PULL_LOW_EN);
	MAC_REG_W32(R_AX_GPIO0_15_EECS_EESK_LED1_PULL_LOW_EN,
		    val32 | B_AX_EECS_PULL_LOW_EN |
			B_AX_EESK_PULL_LOW_EN |
			B_AX_LED1_PULL_LOW_EN);

	adapter->sm.pwr = MAC_AX_PWR_ON;
	adapter->sm.plat = MAC_AX_PLAT_ON;
	adapter->sm.io_st = MAC_AX_IO_ST_NORM;

	/*enable dmac , 0x8400*/
	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN,
		    val32 | B_AX_MAC_FUNC_EN |
			B_AX_DMAC_FUNC_EN |
			B_AX_MPDU_PROC_EN |
			B_AX_WD_RLS_EN |
			B_AX_DLE_WDE_EN |
			B_AX_TXPKT_CTRL_EN |
			B_AX_STA_SCH_EN |
			B_AX_DLE_PLE_EN |
			B_AX_PKT_BUF_EN |
			B_AX_DMAC_TBL_EN |
			B_AX_PKT_IN_EN |
			B_AX_DLE_CPUIO_EN |
			B_AX_DISPATCHER_EN |
			B_AX_BBRPT_EN |
			B_AX_MAC_SEC_EN |
			B_AX_DMACREG_GCKEN);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	/*enable cmac , 0xC000*/
	val32 = MAC_REG_R32(R_AX_CMAC_FUNC_EN);
	MAC_REG_W32(R_AX_CMAC_FUNC_EN,
		    val32 | B_AX_CMAC_EN |
			B_AX_CMAC_TXEN |
			B_AX_CMAC_RXEN |
			B_AX_FORCE_CMACREG_GCKEN |
			B_AX_PHYINTF_EN |
			B_AX_CMAC_DMA_EN |
			B_AX_PTCLTOP_EN |
			B_AX_SCHEDULER_EN |
			B_AX_TMAC_EN |
			B_AX_RMAC_EN);

	adapter->sm.cmac0_func = MAC_AX_FUNC_ON;

	/* 0x2DC[27:24] = 4'b0001 */
	val32 = MAC_REG_R32(R_AX_LED1_FUNC_SEL);
	SET_CLR_WORD(val32, 0x1, B_AX_PINMUX_EESK_FUNC_SEL_V1);
	MAC_REG_W32(R_AX_LED1_FUNC_SEL, val32);

	return MACSUCCESS;
}

u32 mac_pwr_on_nic_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, hci_sel;
	u32 ret;
	u8 val8;

	val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
	hci_sel = GET_FIELD(val32, B_AX_PAD_HCI_SEL_V2);
	if (hci_sel == MAC_AX_HCI_SEL_PCIE_USB) {
		/* 0x218[16] = 1 */
		val32 = MAC_REG_R32(R_AX_LDO_AON_CTRL0);
		MAC_REG_W32(R_AX_LDO_AON_CTRL0, val32 | B_AX_PD_REGU_L);
	}

	/* 0x04[12:11] = 2'b00 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~(B_AX_AFSM_WLSUS_EN |
			B_AX_AFSM_PCIE_SUS_EN));

	/* 0x04[18] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_DIS_WLBT_PDNSUSEN_SOPC);

	/* 0x90[1] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_DIS_WLBT_LPSEN_LOPC);

	/* 0x04[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APDM_HPDN);

	/* 0x04[10] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_APFM_SWLPS);

	/* 0x200[15:13] = 3'b111*/
	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x7, B_AX_OCP_L1);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* polling 0x04[17] = 1*/
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_RDY_SYSPWR,
			   B_AX_RDY_SYSPWR);
	if (ret)
		return ret;

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x04[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFN_ONMAC);

	/* polling 0x04[8] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFN_ONMAC, 0);
	if (ret)
		return ret;

	/* reset platform twice : 0x88[0] = 1->0->1->0 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 & ~B_AX_PLATFORM_EN);

	/* 0x88[0] = 1 */
	val8 = MAC_REG_R8(R_AX_PLATFORM_ENABLE);
	MAC_REG_W8(R_AX_PLATFORM_ENABLE, val8 | B_AX_PLATFORM_EN);

	/* 0x70[12] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_SDIO_CTRL);
	MAC_REG_W32(R_AX_SYS_SDIO_CTRL, val32 & ~B_AX_PCIE_CALIB_EN);

	/* CMAC1 Power off */
	/* 0x80[30] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 & ~B_AX_CMAC1_FEN);

	/* 0x80[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_CMAC12PP);

	/* 0x24[4:0] = 0 */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1,
		    val32 & ~B_AX_R_SYM_WLCMAC1_P4_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P3_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P2_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_P1_PC_EN &
		    ~B_AX_R_SYM_WLCMAC1_PC_EN);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x24[6:4] = 3'b001 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_2, 0x10, 0x70);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x26[3:0] = 4'b0000 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_XTAL_XMD_4, 0, 0x0F);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0xCC[2] = 1 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 | B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* 0x00[8] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 | B_AX_ISO_EB2CORE);

	/* 0x00[15] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(15)));

	PLTFM_DELAY_MS(1);

	/* 0x00[14] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL, val32 & ~(BIT(14)));

	/* 0xCC[2] = 0 */
	val32 = MAC_REG_R32(R_AX_PMC_DBG_CTRL2);
	MAC_REG_W32(R_AX_PMC_DBG_CTRL2, val32 & ~B_AX_SYSON_DIS_PMCR_AX_WRMSK);

	/* enable GPIO16 GPIO17 GPIO18 internal weakly pull low */
	val32 = MAC_REG_R32(R_AX_GPIO0_15_EECS_EESK_LED1_PULL_LOW_EN);
	MAC_REG_W32(R_AX_GPIO0_15_EECS_EESK_LED1_PULL_LOW_EN,
		    val32 | B_AX_EECS_PULL_LOW_EN |
			B_AX_EESK_PULL_LOW_EN |
			B_AX_LED1_PULL_LOW_EN);

	adapter->sm.pwr = MAC_AX_PWR_ON;
	adapter->sm.plat = MAC_AX_PLAT_ON;
	adapter->sm.io_st = MAC_AX_IO_ST_NORM;

	/*enable dmac , 0x8400*/
	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN,
		    val32 | B_AX_MAC_FUNC_EN |
			B_AX_DMAC_FUNC_EN |
			B_AX_MPDU_PROC_EN |
			B_AX_WD_RLS_EN |
			B_AX_DLE_WDE_EN |
			B_AX_TXPKT_CTRL_EN |
			B_AX_STA_SCH_EN |
			B_AX_DLE_PLE_EN |
			B_AX_PKT_BUF_EN |
			B_AX_DMAC_TBL_EN |
			B_AX_PKT_IN_EN |
			B_AX_DLE_CPUIO_EN |
			B_AX_DISPATCHER_EN |
			B_AX_BBRPT_EN |
			B_AX_MAC_SEC_EN |
			B_AX_DMACREG_GCKEN);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	/*enable cmac , 0xC000*/
	val32 = MAC_REG_R32(R_AX_CMAC_FUNC_EN);
	MAC_REG_W32(R_AX_CMAC_FUNC_EN,
		    val32 | B_AX_CMAC_EN |
			B_AX_CMAC_TXEN |
			B_AX_CMAC_RXEN |
			B_AX_FORCE_CMACREG_GCKEN |
			B_AX_PHYINTF_EN |
			B_AX_CMAC_DMA_EN |
			B_AX_PTCLTOP_EN |
			B_AX_SCHEDULER_EN |
			B_AX_TMAC_EN |
			B_AX_RMAC_EN);

	adapter->sm.cmac0_func = MAC_AX_FUNC_ON;

	/* 0x2DC[27:24] = 4'b0001 */
	val32 = MAC_REG_R32(R_AX_LED1_FUNC_SEL);
	SET_CLR_WORD(val32, 0x1, B_AX_PINMUX_EESK_FUNC_SEL_V1);
	MAC_REG_W32(R_AX_LED1_FUNC_SEL, val32);

	return MACSUCCESS;
}

u32 mac_pwr_off_sdio_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u8 val8;
	u32 ret;

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 and 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x2F0[17] = 0 */
	val32 = MAC_REG_R32(R_AX_WLRF_CTRL);
	MAC_REG_W32(R_AX_WLRF_CTRL, val32 & ~B_AX_AFC_AFEDIG);

	/* 0x02[1:0] = 0 */
	val8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
	MAC_REG_W8(R_AX_SYS_FUNC_EN,
		   val8 & ~B_AX_FEN_BB_GLB_RSTN &
		   ~B_AX_FEN_BBRSTB);

	/* 0x82[1:0] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    val32 & ~B_AX_R_SYM_FEN_WLBBGLB_1 &
		    ~B_AX_R_SYM_FEN_WLBBFUN_1);

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[9] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_OFFMAC);

	/* polling 0x04[9] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFM_OFFMAC, 0);
	if (ret)
		return ret;

	/* 0x04[28][30] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_SOP_EDSWR &
			~B_AX_SOP_EASWR);

	/* 0x04[22] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_XTAL_OFF_A_DIE);

	//0x10[10]=1,  0x200[18:17] = 2'b11
	val32 = MAC_REG_R32(R_AX_SYS_SWR_CTRL1);
	MAC_REG_W32(R_AX_SYS_SWR_CTRL1, val32 | B_AX_SYM_CTRL_SPS_PWMFREQ);

	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x3, B_AX_REG_ZCDC_H);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* 0x04[12:11] = 2'b01 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, (val32 | B_AX_AFSM_WLSUS_EN) &
			~B_AX_AFSM_PCIE_SUS_EN);

	/* 0x4086[0] = 1 */
	val32 = MAC_REG_R32(R_AX_SDIO_BUS_CTRL_V1);
	MAC_REG_W32(R_AX_SDIO_BUS_CTRL_V1, val32 | B_AX_HCI_SUS_REQ);

	/* polling 0x4086[1] = 0 */
	ret = pwr_poll_u32(adapter,
			   R_AX_SDIO_BUS_CTRL_V1, B_AX_HCI_RESUME_RDY, 0);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_pwr_off_usb_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u8 val8;
	u32 ret;

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 and 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x2F0[17] = 0 */
	val32 = MAC_REG_R32(R_AX_WLRF_CTRL);
	MAC_REG_W32(R_AX_WLRF_CTRL, val32 & ~B_AX_AFC_AFEDIG);

	/* 0x02[1:0] = 0 */
	val8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
	MAC_REG_W8(R_AX_SYS_FUNC_EN,
		   val8 & ~B_AX_FEN_BB_GLB_RSTN & ~B_AX_FEN_BBRSTB);

	/* 0x82[1:0] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    val32 & ~B_AX_R_SYM_FEN_WLBBGLB_1 &
		    ~B_AX_R_SYM_FEN_WLBBFUN_1);

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[9] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_OFFMAC);

	/* polling 0x04[9] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFM_OFFMAC, 0);
	if (ret)
		return ret;

	/* 0x04[28] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 & ~B_AX_SOP_EDSWR);

	/* 0x04[22] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_XTAL_OFF_A_DIE);

	//0x10[10]=1,  0x200[18:17] = 2'b11
	val32 = MAC_REG_R32(R_AX_SYS_SWR_CTRL1);
	MAC_REG_W32(R_AX_SYS_SWR_CTRL1, val32 | B_AX_SYM_CTRL_SPS_PWMFREQ);

	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x3, B_AX_REG_ZCDC_H);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* 0x04[12:11] = 2'b01 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, (val32 | B_AX_AFSM_WLSUS_EN) &
			~B_AX_AFSM_PCIE_SUS_EN);

	return MACSUCCESS;
}

u32 mac_pwr_off_ap_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u8 val8;
	u32 ret;

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 and 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x2F0[17] = 0 */
	val32 = MAC_REG_R32(R_AX_WLRF_CTRL);
	MAC_REG_W32(R_AX_WLRF_CTRL, val32 & ~B_AX_AFC_AFEDIG);

	/* 0x02[1:0] = 0 */
	val8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
	MAC_REG_W8(R_AX_SYS_FUNC_EN,
		   val8 & ~B_AX_FEN_BB_GLB_RSTN & ~B_AX_FEN_BBRSTB);

	/* 0x82[1:0] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    val32 & ~B_AX_R_SYM_FEN_WLBBGLB_1 &
		    ~B_AX_R_SYM_FEN_WLBBFUN_1);

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[9] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_OFFMAC);

	/* polling 0x04[9] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFM_OFFMAC, 0);
	if (ret)
		return ret;

	/* 0x91[0] = 0 == 0x90[8]=0 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 & ~B_AX_LPSOP_DSWR);

	/* 0x04[10] = 1 */
	//val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	//MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_SWLPS);

	return MACSUCCESS;
}

u32 mac_pwr_off_nic_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u8 val8;
	u32 ret;

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 and 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[16] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_EN_WLON);

	/* 0x2F0[17] = 0 */
	val32 = MAC_REG_R32(R_AX_WLRF_CTRL);
	MAC_REG_W32(R_AX_WLRF_CTRL, val32 & ~B_AX_AFC_AFEDIG);

	/* 0x02[1:0] = 0 */
	val8 = MAC_REG_R8(R_AX_SYS_FUNC_EN);
	MAC_REG_W8(R_AX_SYS_FUNC_EN,
		   val8 & ~B_AX_FEN_BB_GLB_RSTN & ~B_AX_FEN_BBRSTB);

	/* 0x82[1:0] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    val32 & ~B_AX_R_SYM_FEN_WLBBGLB_1 &
		    ~B_AX_R_SYM_FEN_WLBBFUN_1);

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL,
		    val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x04[9] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_OFFMAC);

	/* polling 0x04[9] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_SYS_PW_CTRL, B_AX_APFM_OFFMAC, 0);
	if (ret)
		return ret;

	/* 0x91[0] = 0 == 0x90[8]=0 */
	//val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	//MAC_REG_W32(R_AX_WLLPS_CTRL, val32 & ~B_AX_LPSOP_DSWR);

	/* 0x90[31:0] = 0x00_01_A0_B0 */
	MAC_REG_W32(R_AX_WLLPS_CTRL, 0x0001A0B0);

	/* 0x04[22] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_XTAL_OFF_A_DIE);

	//0x10[10]=1,  0x200[18:17] = 2'b11
	val32 = MAC_REG_R32(R_AX_SYS_SWR_CTRL1);
	MAC_REG_W32(R_AX_SYS_SWR_CTRL1, val32 | B_AX_SYM_CTRL_SPS_PWMFREQ);

	val32 = MAC_REG_R32(R_AX_SPS_DIG_ON_CTRL0);
	val32 = SET_CLR_WORD(val32, 0x3, B_AX_REG_ZCDC_H);
	MAC_REG_W32(R_AX_SPS_DIG_ON_CTRL0, val32);

	/* 0x04[10] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_PW_CTRL);
	MAC_REG_W32(R_AX_SYS_PW_CTRL, val32 | B_AX_APFM_SWLPS);

	return MACSUCCESS;
}

#if MAC_AX_FEATURE_HV
u32 mac_enter_lps_sdio_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;

	/* 0x7800[0] = 1  */
	val32 = MAC_REG_R32(R_AX_FWD1IMR_V1);
	MAC_REG_W32(R_AX_FWD1IMR_V1, val32 | BIT(0));

	/* 0x82[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM22PP);
	/* 0x25[2] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM2_PC_EN);

	/* 0x82[5] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM32PP);
	/* 0x25[3] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM3_PC_EN);

	/* 0x82[6] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM42PP);
	/* 0x25[4] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM4_PC_EN);

	/* 0x83[2] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM32PP_V1);
	/* 0x25[5] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM1_PC_EN);

	/* 0x83[3] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM42PP);
	/* 0x25[6] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM2_PC_EN);

	/* 0x83[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM52PP);
	/* 0x25[7] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM3_PC_EN);

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x90[31:0] = 0x04_80_A1_B0 */
	MAC_REG_W32(R_AX_WLLPS_CTRL, 0x0480A1B0);

	/* 0x90[0] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_WL_LPS_EN);

	/* polling 0x90[0] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_WLLPS_CTRL, B_AX_WL_LPS_EN, 0);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_enter_lps_usb_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;

	/* 0x7800[0] = 1  */
	val32 = MAC_REG_R32(R_AX_FWD1IMR_V1);
	MAC_REG_W32(R_AX_FWD1IMR_V1, val32 | BIT(0));

	/* 0x82[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM22PP);
	/* 0x25[2] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM2_PC_EN);

	/* 0x82[5] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM32PP);
	/* 0x25[3] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM3_PC_EN);

	/* 0x82[6] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM42PP);
	/* 0x25[4] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM4_PC_EN);

	/* 0x83[2] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM32PP_V1);
	/* 0x25[5] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM1_PC_EN);

	/* 0x83[3] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM42PP);
	/* 0x25[6] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM2_PC_EN);

	/* 0x83[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM52PP);
	/* 0x25[7] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM3_PC_EN);

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x90[31:0] = 0x04_81_A1_B0 */
	MAC_REG_W32(R_AX_WLLPS_CTRL, 0x0481A1B0);

	/* 0x90[0] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_WL_LPS_EN);

	/* polling 0x90[0] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_WLLPS_CTRL, B_AX_WL_LPS_EN, 0);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_enter_lps_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;

	/* 0x7800[0] = 1  */
	val32 = MAC_REG_R32(R_AX_FWD1IMR_V1);
	MAC_REG_W32(R_AX_FWD1IMR_V1, val32 | BIT(0));

	/* 0x82[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM22PP);
	/* 0x25[2] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM2_PC_EN);

	/* 0x82[5] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM32PP);
	/* 0x25[3] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM3_PC_EN);

	/* 0x82[6] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_IMEM42PP);
	/* 0x25[4] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_IMEM4_PC_EN);

	/* 0x83[2] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM32PP_V1);
	/* 0x25[5] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM1_PC_EN);

	/* 0x83[3] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM42PP);
	/* 0x25[6] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM2_PC_EN);

	/* 0x83[4] = 1  */
	val32 = MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND);
	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, val32 | B_AX_R_SYM_ISO_DMEM52PP);
	/* 0x25[7] = 0  */
	val32 = MAC_REG_R32(R_AX_AFE_CTRL1);
	MAC_REG_W32(R_AX_AFE_CTRL1, val32 & ~B_AX_DMEM3_PC_EN);

	/* XTAL_SI 0x90[0] = 1 then 0x90[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0x02, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x10, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[3] = 0 then 0x90[2] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 0 */
	/* XTAL_SI 0x90[5] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[6] = 0 */
	/* XTAL_SI 0x90[6] = 0 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 & ~B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x90[31:0] = 0x04_81_A1_B0 */
	MAC_REG_W32(R_AX_WLLPS_CTRL, 0x0481A1B0);

	/* 0x90[0] = 1 */
	val32 = MAC_REG_R32(R_AX_WLLPS_CTRL);
	MAC_REG_W32(R_AX_WLLPS_CTRL, val32 | B_AX_WL_LPS_EN);

	/* polling 0x90[0] = 0 */
	ret = pwr_poll_u32(adapter, R_AX_WLLPS_CTRL, B_AX_WL_LPS_EN, 0);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_leave_lps_sdio_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u32 cnt = 320;

	/* 0x4083[7] = 1 == 0x4080[31] = 1 */
	val32 = MAC_REG_R32(R_AX_SDIO_HRPWM1_V1);
	MAC_REG_W32(R_AX_SDIO_HRPWM1_V1, val32 | BIT(31));

	/* polling 0x1E5[7] = 1 == 0x1E4[15] = 1 */
	ret = pwr_poll_u32(adapter, R_AX_RPWM,
			   B_AX_RPWM_TOGGLE, B_AX_RPWM_TOGGLE);
	if (ret)
		return ret;

	/* delay 0x10 ms */
	while (--cnt)
		PLTFM_DELAY_US(50);

	/* 0x4083[7] = 0 == 0x4080[31] = 0 */
	val32 = MAC_REG_R32(R_AX_SDIO_HRPWM1_V1);
	MAC_REG_W32(R_AX_SDIO_HRPWM1_V1, val32 & ~(BIT(31)));

	/* 0x7804[0] = 1 */
	val32 = MAC_REG_R32(R_AX_FWD1ISR_V1);
	MAC_REG_W32(R_AX_FWD1ISR_V1, val32 | B_AX_FS_RPWM_INT);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	return MACSUCCESS;
}

u32 mac_leave_lps_usb_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u32 cnt = 320;

	/* 0x5203[7] = 1 == 0x5200[31] = 1 */
	val32 = MAC_REG_R32(R_AX_USB_D2F_F2D_INFO_V1);
	MAC_REG_W32(R_AX_USB_D2F_F2D_INFO_V1, val32 | BIT(31));

	/* polling 0x1E5[7] = 1 == 0x1E4[15] = 1 */
	ret = pwr_poll_u32(adapter, R_AX_RPWM,
			   B_AX_RPWM_TOGGLE, B_AX_RPWM_TOGGLE);
	if (ret)
		return ret;

	/* delay 0x10 ms */
	while (--cnt)
		PLTFM_DELAY_US(50);

	/* 0x5203[7] = 0 == 0x5200[31] = 0 */
	val32 = MAC_REG_R32(R_AX_USB_D2F_F2D_INFO_V1);
	MAC_REG_W32(R_AX_USB_D2F_F2D_INFO_V1, val32 & ~(BIT(31)));

	/* 0x7804[0] = 1 */
	val32 = MAC_REG_R32(R_AX_FWD1ISR_V1);
	MAC_REG_W32(R_AX_FWD1ISR_V1, val32 | B_AX_FS_RPWM_INT);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	return MACSUCCESS;
}

u32 mac_leave_lps_pcie_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u32 cnt = 320;

	/* 0x30C1[7] = 1 */
	val32 = MAC_REG_R32(R_AX_PCIE_HRPWM_V1);
	MAC_REG_W32(R_AX_PCIE_HRPWM_V1, val32 | BIT(15));

	/* polling 0x1E5[7] = 1 == 0x1E4[15] = 1 */
	ret = pwr_poll_u32(adapter, R_AX_RPWM,
			   B_AX_RPWM_TOGGLE, B_AX_RPWM_TOGGLE);
	if (ret)
		return ret;

	/* delay 0x10 ms */
	while (--cnt)
		PLTFM_DELAY_US(50);

	/* 0x30C1[7] = 0 */
	val32 = MAC_REG_R32(R_AX_PCIE_HRPWM_V1);
	MAC_REG_W32(R_AX_PCIE_HRPWM_V1, val32 & ~(BIT(15)));

	/* 0x7804[0] = 1 */
	val32 = MAC_REG_R32(R_AX_FWD1ISR_V1);
	MAC_REG_W32(R_AX_FWD1ISR_V1, val32 | B_AX_FS_RPWM_INT);

	/* 0x18[6] = 1 */
	/* XTAL_SI 0x90[6] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_PTA_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x40, 0x40);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* 0x18[5] = 1 */
	/* XTAL_SI 0x90[5] = 1 */
	val32 = MAC_REG_R32(R_AX_SYS_ADIE_PAD_PWR_CTRL);
	MAC_REG_W32(R_AX_SYS_ADIE_PAD_PWR_CTRL, val32 | B_AX_SYM_PADPDN_WL_RFC_1P3);

	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x20, 0x20);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[2] = 1 then 0x90[3] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x04, 0x04);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x08, 0x08);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[4] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x10);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x80[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S0, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x81[0] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WL_RFC_S1, 0x01, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0xA1[1] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_SRAM_CTRL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[7] = 1 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0x80, 0x80);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	/* XTAL_SI 0x90[1] = 0 then 0x90[0] = 0 */
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x02);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}
	ret = mac_write_xtal_si(adapter, XTAL_SI_ANAPAR_WL, 0, 0x01);
	if (ret) {
		PLTFM_MSG_ERR("Write XTAL_SI fail!\n");
		return ret;
	}

	return MACSUCCESS;
}

#endif

#endif /* #if MAC_AX_8852C_SUPPORT */
