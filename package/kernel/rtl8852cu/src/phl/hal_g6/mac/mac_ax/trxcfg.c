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

#include "trxcfg.h"
#include "mac_priv.h"

u32 check_mac_en(struct mac_ax_adapter *adapter, u8 band,
		 enum mac_ax_hwmod_sel sel)
{
	if (adapter->sm.pwr != MAC_AX_PWR_ON)
		return MACPROCERR;

	if (sel == MAC_AX_DMAC_SEL) {
		if (adapter->sm.dmac_func != MAC_AX_FUNC_ON)
			return MACIOERRDMAC;
	} else if (sel == MAC_AX_CMAC_SEL && band == MAC_AX_BAND_0) {
		if (adapter->sm.cmac0_func != MAC_AX_FUNC_ON)
			return MACIOERRCMAC0;
	} else if (sel == MAC_AX_CMAC_SEL && band == MAC_AX_BAND_1) {
		if (adapter->sm.cmac1_func != MAC_AX_FUNC_ON)
			return MACIOERRCMAC1;
	} else {
		PLTFM_MSG_ERR("[ERR] mac sel: %d\n", sel);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

static u32 scheduler_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32, reg;

		reg = band == MAC_AX_BAND_0 ?
		      R_AX_SCHEDULE_ERR_IMR : R_AX_SCHEDULE_ERR_IMR_C1;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_SORT_NON_IDLE_ERR_INT_EN |
			   B_AX_FSM_TIMEOUT_ERR_INT_EN);
		val32 |= ((B_AX_SORT_NON_IDLE_ERR_INT_EN &
			   SCHEDULER_SORT_NON_IDLE_ERR_SER_EN) |
			  (B_AX_FSM_TIMEOUT_ERR_INT_EN &
			   SCHEDULER_FSM_TIMEOUT_ERR_SER_EN));
		MAC_REG_W32(reg, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 ptcl_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32, reg;

		reg = band == MAC_AX_BAND_0 ?
		      R_AX_PTCL_IMR0 : R_AX_PTCL_IMR0_C1;
		val32 = 0;
		val32 &= ~(B_AX_FSM_TIMEOUT_ERR_INT_EN |
			   B_AX_F2PCMDRPT_FULL_DROP_ERR_INT_EN |
			   B_AX_TXPRT_FULL_DROP_ERR_INT_EN |
			   B_AX_D_PKTID_ERR_INT_EN |
			   B_AX_Q_PKTID_ERR_INT_EN |
			   B_AX_BCNQ_ORDER_ERR_INT_EN |
			   B_AX_TWTSP_QSEL_ERR_INT_EN |
			   B_AX_F2PCMD_EMPTY_ERR_INT_EN |
			   B_AX_TX_RECORD_PKTID_ERR_INT_EN |
			   B_AX_TX_SPF_U3_PKTID_ERR_INT_EN |
			   B_AX_TX_SPF_U2_PKTID_ERR_INT_EN |
			   B_AX_TX_SPF_U1_PKTID_ERR_INT_EN |
			   B_AX_RX_SPF_U0_PKTID_ERR_INT_EN |
			   B_AX_F2PCMD_USER_ALLC_ERR_INT_EN |
			   B_AX_F2PCMD_ASSIGN_PKTID_ERR_INT_EN |
			   B_AX_F2PCMD_RD_PKTID_ERR_INT_EN |
			   B_AX_F2PCMD_PKTID_ERR_INT_EN);

		val32 |= ((B_AX_FSM_TIMEOUT_ERR_INT_EN &
			   PTCL_FSM_TIMEOUT_ERR_SER_EN) |
			  (B_AX_F2PCMDRPT_FULL_DROP_ERR_INT_EN &
			   PTCL_F2PCMDRPT_FULL_DROP_SER_EN) |
			  (B_AX_TXPRT_FULL_DROP_ERR_INT_EN &
			   PTCL_TXRPT_FULL_DROP_SER_EN) |
			  (B_AX_D_PKTID_ERR_INT_EN &
			   PTCL_D_PKTID_ERR_SER_EN) |
			  (B_AX_Q_PKTID_ERR_INT_EN &
			   PTCL_Q_PKTID_ERR_SER_EN) |
			  (B_AX_BCNQ_ORDER_ERR_INT_EN &
			   PTCL_BCNQ_ORDER_ERR_SER_EN) |
			  (B_AX_TWTSP_QSEL_ERR_INT_EN &
			   PTCL_TWTSP_QSEL_ERR_SER_EN) |
			  (B_AX_F2PCMD_EMPTY_ERR_INT_EN &
			   PTCL_F2PCMD_EMPTY_ERR_SER_EN) |
			  (B_AX_TX_RECORD_PKTID_ERR_INT_EN &
			   PTCL_TX_RECORD_PKTID_ERR_SER_EN) |
			  (B_AX_TX_SPF_U3_PKTID_ERR_INT_EN &
			   PTCL_TX_SPF_U3_PKTID_ERR_SER_EN) |
			  (B_AX_TX_SPF_U2_PKTID_ERR_INT_EN &
			   PTCL_TX_SPF_U2_PKTID_ERR_SER_EN) |
			  (B_AX_TX_SPF_U1_PKTID_ERR_INT_EN &
			   PTCL_TX_SPF_U1_PKTID_ERR_SER_EN) |
			  (B_AX_RX_SPF_U0_PKTID_ERR_INT_EN &
			   PTCL_TX_SPF_U0_PKTID_ERR_SER_EN) |
			  (B_AX_F2PCMD_USER_ALLC_ERR_INT_EN &
			   PTCL_F2PCMD_USER_ALLC_ERR_SER_EN) |
			  (B_AX_F2PCMD_ASSIGN_PKTID_ERR_INT_EN &
			   PTCL_F2PCMD_ASSIGN_PKTID_ERR_SER_EN) |
			  (B_AX_F2PCMD_RD_PKTID_ERR_INT_EN &
			   PTCL_F2PCMD_RD_PKTID_ERR_SER_EN) |
			  (B_AX_F2PCMD_PKTID_ERR_INT_EN &
			   PTCL_F2PCMD_PKTID_ERR_SER_EN));
		MAC_REG_W32(reg, val32);
	}
#endif

	return MACSUCCESS;
}

static u32 _patch_cmac_dma_err_fa(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
/*
 * AP  Disable B_AX_STS_FSM_HANG_ERROR_IMR
 * STA Enable  B_AX_STS_FSM_HANG_ERROR_IMR (Wait for "Scan+SER L0")
 */
		if (!chk_patch_cmac_dma_err_fa(adapter))
			return B_AX_RXDATA_FSM_HANG_ERROR_IMR;
	}
#endif
	return 0;
}

static u32 _patch_wmac_timer_src(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 val16;
	u32 reg;

	if (chk_patch_wmac_timer_src(adapter)) {
		reg = band == MAC_AX_BAND_1 ?
			R_AX_DLK_PROTECT_CTL_C1 : R_AX_DLK_PROTECT_CTL;
		val16 = MAC_REG_R16(reg);
		val16 = SET_CLR_WORD(val16, 0x0, B_AX_RX_DLK_CCA_TIME);
		MAC_REG_W16(reg, val16);

		reg = band == MAC_AX_BAND_1 ? R_AX_RCR_C1 : R_AX_RCR;
		val16 = MAC_REG_R16(reg);
		MAC_REG_W16(reg, val16 | BIT12);
	}
	return 0;
}

static u32 _patch_ss2f_path(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	/*52C&92XB has wrong hw default value*/
	if (!chk_patch_ss2f_path(adapter))
		return MACSUCCESS;

	val32 = MAC_REG_R32(R_AX_SS2FINFO_PATH);
	val32 = SET_CLR_WORD(val32, SS2F_PATH_WLCPU, B_AX_SS_DEST_QUEUE);
	MAC_REG_W32(R_AX_SS2FINFO_PATH, val32);

	return MACSUCCESS;
}

bool chk_patch_ss2f_path(struct mac_ax_adapter *adapter)
{
		switch (adapter->hw_info->chip_id) {
		case MAC_AX_CHIP_ID_8192XB:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_ENABLE;
				case CBV:
						return PATCH_ENABLE;
				case CCV:
						return PATCH_ENABLE;
				default:
						return PATCH_ENABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8851B:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_DISABLE;
				case CBV:
						return PATCH_DISABLE;
				default:
						return PATCH_DISABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8852A:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_DISABLE;
				case CBV:
						return PATCH_DISABLE;
				case CCV:
						return PATCH_DISABLE;
				case CDV:
						return PATCH_DISABLE;
				default:
						return PATCH_DISABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8852B:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_DISABLE;
				case CBV:
						return PATCH_DISABLE;
				case CCV:
						return PATCH_DISABLE;
				case CDV:
						return PATCH_DISABLE;
				default:
						return PATCH_DISABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8852C:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_ENABLE;
				case CBV:
						return PATCH_ENABLE;
				default:
						return PATCH_ENABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8851E:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_ENABLE;
				default:
						return PATCH_ENABLE;
				}
				break;
		case MAC_AX_CHIP_ID_8852D:
				switch (adapter->hw_info->cv) {
				case CAV:
						return PATCH_ENABLE;
				default:
						return PATCH_ENABLE;
				}
		default:
				PLTFM_MSG_ALWAYS("Not Support IC version =%x\n",
						 adapter->hw_info->cv);
				return PATCH_DISABLE;
		}
}

static u32 cdma_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32, reg;

		reg = band == MAC_AX_BAND_0 ? R_AX_DLE_CTRL : R_AX_DLE_CTRL_C1;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_RXSTS_FSM_HANG_ERROR_IMR |
			   B_AX_RXDATA_FSM_HANG_ERROR_IMR |
			   B_AX_NO_RESERVE_PAGE_ERR_IMR);
		val32 |= ((B_AX_RXSTS_FSM_HANG_ERROR_IMR &
			   CMAC_DMA_RXSTS_FSM_HANG_SER_EN) |
			  (B_AX_RXDATA_FSM_HANG_ERROR_IMR &
			   CMAC_DMA_RXDATA_FSM_HANG_SER_EN) |
			  (B_AX_NO_RESERVE_PAGE_ERR_IMR &
			   CMAC_DMA_NO_RSVD_PAGE_SER_EN));
		val32 |= _patch_cmac_dma_err_fa(adapter);
		MAC_REG_W32(reg, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 phy_intf_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;
		u32 reg;

		reg = band == MAC_AX_BAND_0 ?
		      R_AX_PHYINFO_ERR_IMR : R_AX_PHYINFO_ERR_IMR_C1;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_PHY_TXON_TIMEOUT_INT_EN |
			   B_AX_CCK_CCA_TIMEOUT_INT_EN |
			   B_AX_OFDM_CCA_TIMEOUT_INT_EN |
			   B_AX_DATA_ON_TIMEOUT_INT_EN |
			   B_AX_STS_ON_TIMEOUT_INT_EN |
			   B_AX_CSI_ON_TIMEOUT_INT_EN);

		val32 |= ((B_AX_PHY_TXON_TIMEOUT_INT_EN &
			   PHYINTF_PHY_TXON_TIMEOUT_ERR_SER_EN) |
			  (B_AX_CCK_CCA_TIMEOUT_INT_EN &
			   PHYINTF_CCK_CCA_TIMEOUT_ERR_SER_EN) |
			  (B_AX_OFDM_CCA_TIMEOUT_INT_EN &
			   PHYINTF_OFDM_CCA_TIMEOUT_ERR_SER_EN) |
			  (B_AX_DATA_ON_TIMEOUT_INT_EN &
			   PHYINTF_DATA_ON_TIMEOUT_ERR_SER_EN) |
			  (B_AX_STS_ON_TIMEOUT_INT_EN &
			   PHYINTF_STS_ON_TIMEOUT_ERR_SER_EN) |
			  (B_AX_CSI_ON_TIMEOUT_INT_EN &
			   PHYINTF_CSI_ON_TIMEOUT_ERR_SER_EN));
		MAC_REG_W32(reg, val32);
	}
#endif

	return MACSUCCESS;
}

static u32 rmac_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32, reg;

		reg = band == MAC_AX_BAND_0 ?
		      R_AX_RMAC_ERR_ISR : R_AX_RMAC_ERR_ISR_C1;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_RMAC_CCA_TO_IDLE_TIMEOUT_INT_EN |
			   B_AX_RMAC_DATA_ON_TO_IDLE_TIMEOUT_INT_EN |
			   B_AX_RMAC_DMA_TIMEOUT_INT_EN |
			   B_AX_RMAC_CCA_TIMEOUT_INT_EN |
			   B_AX_RMAC_DATA_ON_TIMEOUT_INT_EN |
			   B_AX_RMAC_CSI_TIMEOUT_INT_EN |
			   B_AX_RMAC_RX_TIMEOUT_INT_EN |
			   B_AX_RMAC_RX_CSI_TIMEOUT_INT_EN);

		val32 |= ((B_AX_RMAC_CCA_TO_IDLE_TIMEOUT_INT_EN &
			   RMAC_CCA_TO_RX_IDLE_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_DATA_ON_TO_IDLE_TIMEOUT_INT_EN &
			   RMAC_DATA_ON_TO_RX_IDLE_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_DMA_TIMEOUT_INT_EN &
			   RMAC_DMA_WRITE_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_CCA_TIMEOUT_INT_EN &
			   RMAC_CCA_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_DATA_ON_TIMEOUT_INT_EN &
			   RMAC_DATA_ON_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_CSI_TIMEOUT_INT_EN &
			   RMAC_CSI_DATA_ON_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_RX_TIMEOUT_INT_EN &
			   RMAC_RX_FSM_TIMEOUT_ERR_SER_EN) |
			  (B_AX_RMAC_RX_CSI_TIMEOUT_INT_EN &
			   RMAC_CSI_MODE_TIMEOUT_ERR_SER_EN));
		MAC_REG_W32(reg, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 tmac_imr_enable(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32, reg;

		reg = band == MAC_AX_BAND_0 ?
		      R_AX_TMAC_ERR_IMR_ISR : R_AX_TMAC_ERR_IMR_ISR_C1;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_TMAC_MACTX_INT_EN |
			   B_AX_TMAC_TXCTL_INT_EN |
			   B_AX_TMAC_RESP_INT_EN |
			   B_AX_TMAC_TXPLCP_INT_EN);

		val32 |= ((B_AX_TMAC_MACTX_INT_EN &
			   TMAC_MACTX_TIME_ERR_SER_EN) |
			  (B_AX_TMAC_TXCTL_INT_EN &
			   TMAC_TRXPTCL_TXCTL_TIMEOUT_ERR_SER_EN) |
			  (B_AX_TMAC_RESP_INT_EN &
			   TMAC_RESPONSE_TXCTL_TIMEOUT_ERR_SER_EN) |
			  (B_AX_TMAC_TXPLCP_INT_EN &
			   TMAC_TX_PLCP_INFO_ERR_SER_EN));
		MAC_REG_W32(reg, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 wdrls_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_WDRLS_ERR_IMR);
		val32 &= ~(B_AX_WDRLS_CTL_WDPKTID_ISNULL_ERR_INT_EN |
			   B_AX_WDRLS_CTL_PLPKTID_ISNULL_ERR_INT_EN |
			   B_AX_WDRLS_CTL_FRZTO_ERR_INT_EN |
			   B_AX_WDRLS_PLEBREQ_TO_ERR_INT_EN |
			   B_AX_WDRLS_PLEBREQ_PKTID_ISNULL_ERR_INT_EN |
			   B_AX_WDRLS_RPT0_AGGNUM0_ERR_INT_EN |
			   B_AX_WDRLS_RPT0_FRZTO_ERR_INT_EN |
			   B_AX_WDRLS_RPT1_AGGNUM0_ERR_INT_EN |
			   B_AX_WDRLS_RPT1_FRZTO_ERR_INT_EN);

		val32 |= ((B_AX_WDRLS_CTL_WDPKTID_ISNULL_ERR_INT_EN &
			   DMAC_WDRLS_CTL_WDPKTID_ISNULL_ERR_SER_EN) |
			  (B_AX_WDRLS_CTL_PLPKTID_ISNULL_ERR_INT_EN &
			   DMAC_WDRLS_CTL_PLPKTID_ISNULL_ERR_SER_EN) |
			  (B_AX_WDRLS_CTL_FRZTO_ERR_INT_EN &
			   DMAC_WDRLS_CTL_FRZTO_ERR_SER_EN) |
			  (B_AX_WDRLS_PLEBREQ_TO_ERR_INT_EN &
			   DMAC_WDRLS_PLEBREQ_TO_ERR_SER_EN) |
			  (B_AX_WDRLS_PLEBREQ_PKTID_ISNULL_ERR_INT_EN &
			   DMAC_WDRLS_PLEBREQ_PKTID_ISNULL_ERR_SER_EN) |
			  (B_AX_WDRLS_RPT0_AGGNUM0_ERR_INT_EN &
			   DMAC_WDRLS_RPT0_AGGNUM0_ERR_SER_EN) |
			  (B_AX_WDRLS_RPT0_FRZTO_ERR_INT_EN &
			   DMAC_WDRLS_RPT0_FRZTO_ERR_SER_EN) |
			  (B_AX_WDRLS_RPT1_AGGNUM0_ERR_INT_EN &
			   DMAC_WDRLS_RPT1_AGGNUM0_ERR_SER_EN) |
			  (B_AX_WDRLS_RPT1_FRZTO_ERR_INT_EN &
			   DMAC_WDRLS_RPT1_FRZTO_ERR_SER_EN));
		MAC_REG_W32(R_AX_WDRLS_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 wsec_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_SEC_DEBUG);
		val32 &= ~(B_AX_IMR_ERROR);

		val32 |= ((B_AX_IMR_ERROR &
			   DMAC_IMR_ERROR));
		MAC_REG_W32(R_AX_SEC_DEBUG, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 mpdu_trx_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		/* MDPU Processor TX */
		val32 = MAC_REG_R32(R_AX_MPDU_TX_ERR_IMR);
		val32 &= ~(B_AX_TX_GET_ERRPKTID_INT_EN |
			   B_AX_TX_NXT_ERRPKTID_INT_EN |
			   B_AX_TX_MPDU_SIZE_ZERO_INT_EN |
			   B_AX_TX_OFFSET_ERR_INT_EN |
			   B_AX_TX_HDR3_SIZE_ERR_INT_EN);

		val32 |= ((B_AX_TX_GET_ERRPKTID_INT_EN &
			   DMAC_TX_GET_ERRPKTID_SER_EN) |
			  (B_AX_TX_NXT_ERRPKTID_INT_EN &
			   DMAC_TX_NXT_ERRPKTID_SER_EN) |
			  (B_AX_TX_MPDU_SIZE_ZERO_INT_EN &
			   DMAC_TX_MPDU_SIZE_ZERO_SER_EN) |
			  (B_AX_TX_OFFSET_ERR_INT_EN &
			   DMAC_TX_OFFSET_ERR_SER_EN) |
			  (B_AX_TX_HDR3_SIZE_ERR_INT_EN &
			   DMAC_TX_HDR3_SIZE_ERR_SER_EN));
		MAC_REG_W32(R_AX_MPDU_TX_ERR_IMR, val32);

		/* MDPU Processor RX */
		val32 = MAC_REG_R32(R_AX_MPDU_RX_ERR_IMR);
		val32 &= ~(B_AX_GETPKTID_ERR_INT_EN |
			   B_AX_MHDRLEN_ERR_INT_EN |
			   B_AX_RPT_ERR_INT_EN);

		val32 |= ((B_AX_GETPKTID_ERR_INT_EN &
			   DMAC_GETPKTID_ERR_SER_EN) |
			  (B_AX_MHDRLEN_ERR_INT_EN &
			   DMAC_MHDRLEN_ERR_SER_EN) |
			  (B_AX_RPT_ERR_INT_EN &
			   DMAC_RPT_ERR_SER_EN));
		MAC_REG_W32(R_AX_MPDU_RX_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 sta_sch_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		/* MDPU Processor TX */
		val32 = MAC_REG_R32(R_AX_STA_SCHEDULER_ERR_IMR);
		val32 &= ~(B_AX_SEARCH_HANG_TIMEOUT_INT_EN |
			   B_AX_RPT_HANG_TIMEOUT_INT_EN |
			   B_AX_PLE_B_PKTID_ERR_INT_EN);

		val32 |= ((B_AX_SEARCH_HANG_TIMEOUT_INT_EN &
			   DMAC_SEARCH_HANG_TIMEOUT_SER_EN) |
			  (B_AX_RPT_HANG_TIMEOUT_INT_EN &
			   DMAC_RPT_HANG_TIMEOUT_SER_EN) |
			  (B_AX_PLE_B_PKTID_ERR_INT_EN &
			   DMAC_PLE_B_PKTID_ERR_SER_EN));
		MAC_REG_W32(R_AX_STA_SCHEDULER_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 txpktctl_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_TXPKTCTL_ERR_IMR_ISR);
		val32 &= ~(B_AX_TXPKTCTL_USRCTL_REINIT_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_NOINIT_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_RDNRLSCMD_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_RLSBMPLEN_ERR_INT_EN |
			   B_AX_TXPKTCTL_CMDPSR_CMDTYPE_ERR_INT_EN |
			   B_AX_TXPKTCTL_CMDPSR_FRZTO_ERR_INT_EN);

		val32 |= ((B_AX_TXPKTCTL_USRCTL_REINIT_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_REINIT_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_NOINIT_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_NOINIT_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_RDNRLSCMD_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_RDNRLSCMD_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_RLSBMPLEN_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_RLSBMPLEN_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_CMDPSR_CMDTYPE_ERR_INT_EN &
			   DMAC_TXPKTCTL_CMDPSR_CMDTYPE_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_CMDPSR_FRZTO_ERR_INT_EN &
			   DMAC_TXPKTCTL_CMDPSR_FRZTO_ERR_SER_EN));
		MAC_REG_W32(R_AX_TXPKTCTL_ERR_IMR_ISR, val32);

		val32 = MAC_REG_R32(R_AX_TXPKTCTL_ERR_IMR_ISR_B1);
		val32 &= ~(B_AX_TXPKTCTL_USRCTL_REINIT_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_NOINIT_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_RDNRLSCMD_ERR_INT_EN |
			   B_AX_TXPKTCTL_USRCTL_RLSBMPLEN_ERR_INT_EN |
			   B_AX_TXPKTCTL_CMDPSR_CMDTYPE_ERR_INT_EN |
			   B_AX_TXPKTCTL_CMDPSR_FRZTO_ERR_INT_EN);

		val32 |= ((B_AX_TXPKTCTL_USRCTL_REINIT_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_REINIT_B1_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_NOINIT_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_NOINIT_B1_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_RDNRLSCMD_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_RDNRLSCMD_B1_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_USRCTL_RLSBMPLEN_ERR_INT_EN &
			   DMAC_TXPKTCTL_USRCTL_RLSBMPLEN_B1_ERR_SER_EN) |
			  (B_AX_TXPKTCTL_CMDPSR_CMDTYPE_ERR_INT_EN &
			   DMAC_TXPKTCTL_CMDPSR_CMDTYPE_ERR_B1_SER_EN) |
			  (B_AX_TXPKTCTL_CMDPSR_FRZTO_ERR_INT_EN &
			   DMAC_TXPKTCTL_CMDPSR_FRZTO_ERR_B1_SER_EN));
		MAC_REG_W32(R_AX_TXPKTCTL_ERR_IMR_ISR_B1, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 wde_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_WDE_ERR_IMR);
		val32 &= ~(B_AX_WDE_BUFREQ_QTAID_ERR_INT_EN |
			   B_AX_WDE_BUFREQ_UNAVAL_ERR_INT_EN |
			   B_AX_WDE_BUFRTN_INVLD_PKTID_ERR_INT_EN |
			   B_AX_WDE_BUFRTN_SIZE_ERR_INT_EN |
			   B_AX_WDE_BUFREQ_SRCHTAILPG_ERR_INT_EN |
			   B_AX_WDE_GETNPG_STRPG_ERR_INT_EN |
			   B_AX_WDE_GETNPG_PGOFST_ERR_INT_EN |
			   B_AX_WDE_BUFMGN_FRZTO_ERR_INT_EN |
			   B_AX_WDE_QUE_CMDTYPE_ERR_INT_EN |
			   B_AX_WDE_QUE_DSTQUEID_ERR_INT_EN |
			   B_AX_WDE_QUE_SRCQUEID_ERR_INT_EN |
			   B_AX_WDE_ENQ_PKTCNT_OVRF_ERR_INT_EN |
			   B_AX_WDE_ENQ_PKTCNT_NVAL_ERR_INT_EN |
			   B_AX_WDE_PREPKTLLT_AD_ERR_INT_EN |
			   B_AX_WDE_NXTPKTLL_AD_ERR_INT_EN |
			   B_AX_WDE_QUEMGN_FRZTO_ERR_INT_EN |
			   B_AX_WDE_DATCHN_ARBT_ERR_INT_EN |
			   B_AX_WDE_DATCHN_NULLPG_ERR_INT_EN |
			   B_AX_WDE_DATCHN_FRZTO_ERR_INT_EN);

		val32 |= ((B_AX_WDE_BUFREQ_QTAID_ERR_INT_EN &
			   DMAC_WDE_BUFREQ_QTAID_ERR_SER_EN) |
			  (B_AX_WDE_BUFREQ_UNAVAL_ERR_INT_EN &
			   DMAC_WDE_BUFREQ_UNAVAL_ERR_SER_EN) |
			  (B_AX_WDE_BUFRTN_INVLD_PKTID_ERR_INT_EN &
			   DMAC_WDE_BUFRTN_INVLD_PKTID_ERR_SER_EN) |
			  (B_AX_WDE_BUFRTN_SIZE_ERR_INT_EN &
			   DMAC_WDE_BUFRTN_SIZE_ERR_SER_EN) |
			  (B_AX_WDE_BUFREQ_SRCHTAILPG_ERR_INT_EN &
			   DMAC_WDE_BUFREQ_SRCHTAILPG_ERR_SER_EN) |
			  (B_AX_WDE_GETNPG_STRPG_ERR_INT_EN &
			   DMAC_WDE_GETNPG_STRPG_ERR_SER_EN) |
			  (B_AX_WDE_GETNPG_PGOFST_ERR_INT_EN &
			   DMAC_WDE_GETNPG_PGOFST_ERR_SER_EN) |
			  (B_AX_WDE_BUFMGN_FRZTO_ERR_INT_EN &
			   DMAC_WDE_BUFMGN_FRZTO_ERR_SER_EN) |
			  (B_AX_WDE_QUE_CMDTYPE_ERR_INT_EN &
			   DMAC_WDE_QUE_CMDTYPE_ERR_SER_EN) |
			  (B_AX_WDE_QUE_DSTQUEID_ERR_INT_EN &
			   DMAC_WDE_QUE_DSTQUEID_ERR_SER_EN) |
			  (B_AX_WDE_QUE_SRCQUEID_ERR_INT_EN &
			   DMAC_WDE_QUE_SRCQUEID_ERR_SER_EN) |
			  (B_AX_WDE_ENQ_PKTCNT_OVRF_ERR_INT_EN &
			   DMAC_WDE_ENQ_PKTCNT_OVRF_ERR_SER_EN) |
			  (B_AX_WDE_ENQ_PKTCNT_NVAL_ERR_INT_EN &
			   DMAC_WDE_ENQ_PKTCNT_NVAL_ERR_SER_EN) |
			  (B_AX_WDE_PREPKTLLT_AD_ERR_INT_EN &
			   DMAC_WDE_PREPKTLLT_AD_ERR_SER_EN) |
			  (B_AX_WDE_NXTPKTLL_AD_ERR_INT_EN &
			   DMAC_WDE_NXTPKTLL_AD_ERR_SER_EN) |
			  (B_AX_WDE_QUEMGN_FRZTO_ERR_INT_EN &
			   DMAC_WDE_QUEMGN_FRZTO_ERR_SER_EN) |
			  (B_AX_WDE_DATCHN_ARBT_ERR_INT_EN &
			   DMAC_WDE_DATCHN_ARBT_ERR_SER_EN) |
			  (B_AX_WDE_DATCHN_NULLPG_ERR_INT_EN &
			   DMAC_WDE_DATCHN_NULLPG_ERR_SER_EN) |
			  (B_AX_WDE_DATCHN_FRZTO_ERR_INT_EN &
			   DMAC_WDE_DATCHN_FRZTO_ERR_SER_EN));
		MAC_REG_W32(R_AX_WDE_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 ple_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_PLE_ERR_IMR);
		val32 &= ~(B_AX_PLE_BUFREQ_QTAID_ERR_INT_EN |
			   B_AX_PLE_BUFREQ_UNAVAL_ERR_INT_EN |
			   B_AX_PLE_BUFRTN_INVLD_PKTID_ERR_INT_EN |
			   B_AX_PLE_BUFRTN_SIZE_ERR_INT_EN |
			   B_AX_PLE_BUFREQ_SRCHTAILPG_ERR_INT_EN |
			   B_AX_PLE_GETNPG_STRPG_ERR_INT_EN |
			   B_AX_PLE_GETNPG_PGOFST_ERR_INT_EN |
			   B_AX_PLE_BUFMGN_FRZTO_ERR_INT_EN |
			   B_AX_PLE_QUE_CMDTYPE_ERR_INT_EN |
			   B_AX_PLE_QUE_DSTQUEID_ERR_INT_EN |
			   B_AX_PLE_QUE_SRCQUEID_ERR_INT_EN |
			   B_AX_PLE_ENQ_PKTCNT_OVRF_ERR_INT_EN |
			   B_AX_PLE_ENQ_PKTCNT_NVAL_ERR_INT_EN |
			   B_AX_PLE_PREPKTLLT_AD_ERR_INT_EN |
			   B_AX_PLE_NXTPKTLL_AD_ERR_INT_EN |
			   B_AX_PLE_QUEMGN_FRZTO_ERR_INT_EN |
			   B_AX_PLE_DATCHN_ARBT_ERR_INT_EN |
			   B_AX_PLE_DATCHN_NULLPG_ERR_INT_EN |
			   B_AX_PLE_DATCHN_FRZTO_ERR_INT_EN);

		val32 |= ((B_AX_PLE_BUFREQ_QTAID_ERR_INT_EN &
			   DMAC_PLE_BUFREQ_QTAID_ERR_SER_EN) |
			  (B_AX_PLE_BUFREQ_UNAVAL_ERR_INT_EN &
			   DMAC_PLE_BUFREQ_UNAVAL_ERR_SER_EN) |
			  (B_AX_PLE_BUFRTN_INVLD_PKTID_ERR_INT_EN &
			   DMAC_PLE_BUFRTN_INVLD_PKTID_ERR_SER_EN) |
			  (B_AX_PLE_BUFRTN_SIZE_ERR_INT_EN &
			   DMAC_PLE_BUFRTN_SIZE_ERR_SER_EN) |
			  (B_AX_PLE_BUFREQ_SRCHTAILPG_ERR_INT_EN &
			   DMAC_PLE_BUFREQ_SRCHTAILPG_ERR_SER_EN) |
			  (B_AX_PLE_GETNPG_STRPG_ERR_INT_EN &
			   DMAC_PLE_GETNPG_STRPG_ERR_SER_EN) |
			  (B_AX_PLE_GETNPG_PGOFST_ERR_INT_EN &
			   DMAC_PLE_GETNPG_PGOFST_ERR_SER_EN) |
			  (B_AX_PLE_BUFMGN_FRZTO_ERR_INT_EN &
			   DMAC_PLE_BUFMGN_FRZTO_ERR_SER_EN) |
			  (B_AX_PLE_QUE_CMDTYPE_ERR_INT_EN &
			   DMAC_PLE_QUE_CMDTYPE_ERR_SER_EN) |
			  (B_AX_PLE_QUE_DSTQUEID_ERR_INT_EN &
			   DMAC_PLE_QUE_DSTQUEID_ERR_SER_EN) |
			  (B_AX_PLE_QUE_SRCQUEID_ERR_INT_EN &
			   DMAC_PLE_QUE_SRCQUEID_ERR_SER_EN) |
			  (B_AX_PLE_ENQ_PKTCNT_OVRF_ERR_INT_EN &
			   DMAC_PLE_ENQ_PKTCNT_OVRF_ERR_SER_EN) |
			  (B_AX_PLE_ENQ_PKTCNT_NVAL_ERR_INT_EN &
			   DMAC_PLE_ENQ_PKTCNT_NVAL_ERR_SER_EN) |
			  (B_AX_PLE_PREPKTLLT_AD_ERR_INT_EN &
			   DMAC_PLE_PREPKTLLT_AD_ERR_SER_EN) |
			  (B_AX_PLE_NXTPKTLL_AD_ERR_INT_EN &
			   DMAC_PLE_NXTPKTLL_AD_ERR_SER_EN) |
			  (B_AX_PLE_QUEMGN_FRZTO_ERR_INT_EN &
			   DMAC_PLE_QUEMGN_FRZTO_ERR_SER_EN) |
			  (B_AX_PLE_DATCHN_ARBT_ERR_INT_EN &
			   DMAC_PLE_DATCHN_ARBT_ERR_SER_EN) |
			  (B_AX_PLE_DATCHN_NULLPG_ERR_INT_EN &
			   DMAC_PLE_DATCHN_NULLPG_ERR_SER_EN) |
			  (B_AX_PLE_DATCHN_FRZTO_ERR_INT_EN &
			   DMAC_PLE_DATCHN_FRZTO_ERR_SER_EN));
		MAC_REG_W32(R_AX_PLE_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 pktin_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_PKTIN_ERR_IMR);
		val32 &= ~(B_AX_PKTIN_GETPKTID_ERR_INT_EN);

		val32 |= ((B_AX_PKTIN_GETPKTID_ERR_INT_EN &
			   DMAC_PKTIN_GETPKTID_ERR_SER_EN));
		MAC_REG_W32(R_AX_PKTIN_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 dispatcher_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_HOST_DISPATCHER_ERR_IMR);
		val32 &= ~(B_AX_HDT_CHANNEL_DIFF_ERR_INT_EN |
			   B_AX_HDT_CHANNEL_ID_ERR_INT_EN |
			   B_AX_HDT_PKT_FAIL_DBG_INT_EN |
			   B_AX_HDT_PERMU_OVERFLOW_INT_EN |
			   B_AX_HDT_PERMU_UNDERFLOW_INT_EN |
			   B_AX_HDT_PAYLOAD_OVERFLOW_INT_EN |
			   B_AX_HDT_PAYLOAD_UNDERFLOW_INT_EN |
			   B_AX_HDT_OFFSET_UNMATCH_INT_EN |
			   B_AX_HDT_CHANNEL_DMA_ERR_INT_EN |
			   B_AX_HDT_WD_CHK_ERR_INT_EN |
			   B_AX_HDT_PRE_COST_ERR_INT_EN |
			   B_AX_HDT_TXPKTSIZE_ERR_INT_EN |
			   B_AX_HDT_TCP_CHK_ERR_INT_EN |
			   B_AX_HDT_TX_WRITE_OVERFLOW_INT_EN |
			   B_AX_HDT_TX_WRITE_UNDERFLOW_INT_EN |
			   B_AX_HDT_PLD_CMD_OVERLOW_INT_EN |
			   B_AX_HDT_PLD_CMD_UNDERFLOW_INT_EN |
			   B_AX_HDT_FLOW_CTRL_ERR_INT_EN |
			   B_AX_HDT_NULLPKT_ERR_INT_EN |
			   B_AX_HDT_BURST_NUM_ERR_INT_EN |
			   B_AX_HDT_RXAGG_CFG_ERR_INT_EN |
			   B_AX_HDT_SHIFT_EN_ERR_INT_EN |
			   B_AX_HDT_TOTAL_LEN_ERR_INT_EN |
			   B_AX_HDT_DMA_PROCESS_ERR_INT_EN |
			   B_AX_HDT_SHIFT_DMA_CFG_ERR_INT_EN |
			   B_AX_HDT_CHKSUM_FSM_ERR_INT_EN |
			   B_AX_HDT_RX_WRITE_OVERFLOW_INT_EN |
			   B_AX_HDT_RX_WRITE_UNDERFLOW_INT_EN);

		val32 |= ((B_AX_HDT_CHANNEL_DIFF_ERR_INT_EN &
			   DMAC_HDT_CHANNEL_DIFF_ERR_SER_EN) |
			  (B_AX_HDT_CHANNEL_ID_ERR_INT_EN &
			   DMAC_HDT_CHANNEL_ID_ERR_SER_EN) |
			  (B_AX_HDT_PKT_FAIL_DBG_INT_EN &
			   DMAC_HDT_PKT_FAIL_DBG_SER_EN) |
			  (B_AX_HDT_PERMU_OVERFLOW_INT_EN &
			   DMAC_HDT_PERMU_OVERFLOW_SER_EN) |
			  (B_AX_HDT_PERMU_UNDERFLOW_INT_EN &
			   DMAC_HDT_PERMU_UNDERFLOW_SER_EN) |
			  (B_AX_HDT_PAYLOAD_OVERFLOW_INT_EN &
			   DMAC_HDT_PAYLOAD_OVERFLOW_SER_EN) |
			  (B_AX_HDT_PAYLOAD_UNDERFLOW_INT_EN &
			   DMAC_HDT_PAYLOAD_UNDERFLOW_SER_EN) |
			  (B_AX_HDT_OFFSET_UNMATCH_INT_EN &
			   DMAC_HDT_OFFSET_UNMATCH_SER_EN) |
			  (B_AX_HDT_CHANNEL_DMA_ERR_INT_EN &
			   DMAC_HDT_CHANNEL_DMA_ERR_SER_EN) |
			  (B_AX_HDT_WD_CHK_ERR_INT_EN &
			   DMAC_HDT_WD_CHK_ERR_SER_EN) |
			  (B_AX_HDT_PRE_COST_ERR_INT_EN &
			   DMAC_HDT_PRE_COST_ERR_SER_EN) |
			  (B_AX_HDT_TXPKTSIZE_ERR_INT_EN &
			   DMAC_HDT_TXPKTSIZE_ERR_SER_EN) |
			  (B_AX_HDT_TCP_CHK_ERR_INT_EN &
			   DMAC_HDT_TCP_CHK_ERR_SER_EN) |
			  (B_AX_HDT_TX_WRITE_OVERFLOW_INT_EN &
			   DMAC_HDT_TX_WRITE_OVERFLOW_SER_EN) |
			  (B_AX_HDT_TX_WRITE_UNDERFLOW_INT_EN &
			   DMAC_HDT_TX_WRITE_UNDERFLOW_SER_EN) |
			  (B_AX_HDT_PLD_CMD_OVERLOW_INT_EN &
			   DMAC_HDT_PLD_CMD_OVERLOW_SER_EN) |
			  (B_AX_HDT_PLD_CMD_UNDERFLOW_INT_EN &
			   DMAC_HDT_PLD_CMD_UNDERFLOW_SER_EN) |
			  (B_AX_HDT_FLOW_CTRL_ERR_INT_EN &
			   DMAC_HDT_FLOW_CTRL_ERR_SER_EN) |
			  (B_AX_HDT_NULLPKT_ERR_INT_EN &
			   DMAC_HDT_NULLPKT_ERR_SER_EN) |
			  (B_AX_HDT_BURST_NUM_ERR_INT_EN &
			   DMAC_HDT_BURST_NUM_ERR_SER_EN) |
			  (B_AX_HDT_RXAGG_CFG_ERR_INT_EN &
			   DMAC_HDT_RXAGG_CFG_ERR_SER_EN) |
			  (B_AX_HDT_SHIFT_EN_ERR_INT_EN &
			   DMAC_HDT_SHIFT_EN_ERR_SER_EN) |
			  (B_AX_HDT_TOTAL_LEN_ERR_INT_EN &
			   DMAC_HDT_TOTAL_LEN_ERR_SER_EN) |
			  (B_AX_HDT_DMA_PROCESS_ERR_INT_EN &
			   DMAC_HDT_DMA_PROCESS_ERR_SER_EN) |
			  (B_AX_HDT_SHIFT_DMA_CFG_ERR_INT_EN &
			   DMAC_HDT_SHIFT_DMA_CFG_ERR_SER_EN) |
			  (B_AX_HDT_CHKSUM_FSM_ERR_INT_EN &
			   DMAC_HDT_CHKSUM_FSM_ERR_SER_EN) |
			  (B_AX_HDT_RX_WRITE_OVERFLOW_INT_EN &
			   DMAC_HDT_RX_WRITE_OVERFLOW_SER_EN) |
			  (B_AX_HDT_RX_WRITE_UNDERFLOW_INT_EN &
			   DMAC_HDT_RX_WRITE_UNDERFLOW_SER_EN));
		MAC_REG_W32(R_AX_HOST_DISPATCHER_ERR_IMR, val32);

		val32 = MAC_REG_R32(R_AX_CPU_DISPATCHER_ERR_IMR);
		val32 &= ~(B_AX_CPU_CHANNEL_DIFF_ERR_INT_EN |
			   B_AX_CPU_PKT_FAIL_DBG_INT_EN |
			   B_AX_CPU_CHANNEL_ID_ERR_INT_EN |
			   B_AX_CPU_PERMU_OVERFLOW_INT_EN |
			   B_AX_CPU_PERMU_UNDERFLOW_INT_EN |
			   B_AX_CPU_PAYLOAD_OVERFLOW_INT_EN |
			   B_AX_CPU_PAYLOAD_UNDERFLOW_INT_EN |
			   B_AX_CPU_PAYLOAD_CHKSUM_ERR_INT_EN |
			   B_AX_CPU_OFFSET_UNMATCH_INT_EN |
			   B_AX_CPU_CHANNEL_DMA_ERR_INT_EN |
			   B_AX_CPU_WD_CHK_ERR_INT_EN |
			   B_AX_CPU_PRE_COST_ERR_INT_EN |
			   B_AX_CPU_PLD_CMD_OVERLOW_INT_EN |
			   B_AX_CPU_PLD_CMD_UNDERFLOW_INT_EN |
			   B_AX_CPU_F2P_QSEL_ERR_INT_EN |
			   B_AX_CPU_F2P_SEQ_ERR_INT_EN |
			   B_AX_CPU_FLOW_CTRL_ERR_INT_EN |
			   B_AX_CPU_NULLPKT_ERR_INT_EN |
			   B_AX_CPU_BURST_NUM_ERR_INT_EN |
			   B_AX_CPU_RXAGG_CFG_ERR_INT_EN |
			   B_AX_CPU_SHIFT_EN_ERR_INT_EN |
			   B_AX_CPU_TOTAL_LEN_ERR_INT_EN |
			   B_AX_CPU_DMA_PROCESS_ERR_INT_EN |
			   B_AX_CPU_SHIFT_DMA_CFG_ERR_INT_EN |
			   B_AX_CPU_CHKSUM_FSM_ERR_INT_EN |
			   B_AX_CPU_RX_WRITE_OVERFLOW_INT_EN |
			   B_AX_CPU_RX_WRITE_UNDERFLOW_INT_EN);

		val32 |= ((B_AX_CPU_CHANNEL_DIFF_ERR_INT_EN &
			   DMAC_CPU_CHANNEL_DIFF_ERR_SER_EN) |
			  (B_AX_CPU_PKT_FAIL_DBG_INT_EN &
			   DMAC_CPU_PKT_FAIL_DBG_SER_EN) |
			  (B_AX_CPU_CHANNEL_ID_ERR_INT_EN &
			   DMAC_CPU_CHANNEL_ID_ERR_SER_EN) |
			  (B_AX_CPU_PERMU_OVERFLOW_INT_EN &
			   DMAC_CPU_PERMU_OVERFLOW_SER_EN) |
			  (B_AX_CPU_PERMU_UNDERFLOW_INT_EN &
			   DMAC_CPU_PERMU_UNDERFLOW_SER_EN) |
			  (B_AX_CPU_PAYLOAD_OVERFLOW_INT_EN &
			   DMAC_CPU_PAYLOAD_OVERFLOW_SER_EN) |
			  (B_AX_CPU_PAYLOAD_UNDERFLOW_INT_EN &
			   DMAC_CPU_PAYLOAD_UNDERFLOW_SER_EN) |
			  (B_AX_CPU_PAYLOAD_CHKSUM_ERR_INT_EN &
			   DMAC_CPU_PAYLOAD_CHKSUM_ERR_SER_EN) |
			  (B_AX_CPU_OFFSET_UNMATCH_INT_EN &
			   DMAC_CPU_OFFSET_UNMATCH_SER_EN) |
			  (B_AX_CPU_CHANNEL_DMA_ERR_INT_EN &
			   DMAC_CPU_CHANNEL_DMA_ERR_SER_EN) |
			  (B_AX_CPU_WD_CHK_ERR_INT_EN &
			   DMAC_CPU_WD_CHK_ERR_SER_EN) |
			  (B_AX_CPU_PRE_COST_ERR_INT_EN &
			   DMAC_CPU_PRE_COST_ERR_SER_EN) |
			  (B_AX_CPU_PLD_CMD_OVERLOW_INT_EN &
			   DMAC_CPU_PLD_CMD_OVERLOW_SER_EN) |
			  (B_AX_CPU_PLD_CMD_UNDERFLOW_INT_EN &
			   DMAC_CPU_PLD_CMD_UNDERFLOW_SER_EN) |
			  (B_AX_CPU_F2P_QSEL_ERR_INT_EN &
			   DMAC_CPU_F2P_QSEL_ERR_SER_EN) |
			  (B_AX_CPU_F2P_SEQ_ERR_INT_EN &
			   DMAC_CPU_F2P_SEQ_ERR_SER_EN) |
			  (B_AX_CPU_FLOW_CTRL_ERR_INT_EN &
			   DMAC_CPU_FLOW_CTRL_ERR_SER_EN) |
			  (B_AX_CPU_NULLPKT_ERR_INT_EN &
			   DMAC_CPU_NULLPKT_ERR_SER_EN) |
			  (B_AX_CPU_BURST_NUM_ERR_INT_EN &
			   DMAC_CPU_BURST_NUM_ERR_SER_EN) |
			  (B_AX_CPU_RXAGG_CFG_ERR_INT_EN &
			   DMAC_CPU_RXAGG_CFG_ERR_SER_EN) |
			  (B_AX_CPU_SHIFT_EN_ERR_INT_EN &
			   DMAC_CPU_SHIFT_EN_ERR_SER_EN) |
			  (B_AX_CPU_TOTAL_LEN_ERR_INT_EN &
			   DMAC_CPU_TOTAL_LEN_ERR_SER_EN) |
			  (B_AX_CPU_DMA_PROCESS_ERR_INT_EN &
			   DMAC_CPU_DMA_PROCESS_ERR_SER_EN) |
			  (B_AX_CPU_SHIFT_DMA_CFG_ERR_INT_EN &
			   DMAC_CPU_SHIFT_DMA_CFG_ERR_SER_EN) |
			  (B_AX_CPU_CHKSUM_FSM_ERR_INT_EN &
			   DMAC_CPU_CHKSUM_FSM_ERR_SER_EN));
		MAC_REG_W32(R_AX_CPU_DISPATCHER_ERR_IMR, val32);

		val32 = MAC_REG_R32(R_AX_OTHER_DISPATCHER_ERR_IMR);
		val32 &= ~(B_AX_OTHER_STF_WROQT_UNDERFLOW_INT_EN |
			   B_AX_OTHER_STF_WROQT_OVERFLOW_INT_EN |
			   B_AX_OTHER_STF_WRFF_UNDERFLOW_INT_EN |
			   B_AX_OTHER_STF_WRFF_OVERFLOW_INT_EN |
			   B_AX_OTHER_STF_CMD_UNDERFLOW_INT_EN |
			   B_AX_OTHER_STF_CMD_OVERFLOW_INT_EN |
			   B_AX_HOST_ADDR_INFO_LEN_ZERO_ERR_INT_EN |
			   B_AX_CPU_ADDR_INFO_LEN_ZERO_ERR_INT_EN |
			   B_AX_PLE_OUTPUT_ERR_INT_EN |
			   B_AX_PLE_RESP_ERR_INT_EN |
			   B_AX_PLE_BURST_NUM_ERR_INT_EN |
			   B_AX_PLE_NULL_PKT_ERR_INT_EN |
			   B_AX_PLE_FLOW_CTRL_ERR_INT_EN |
			   B_AX_WDE_OUTPUT_ERR_INT_EN |
			   B_AX_WDE_RESP_ERR_INT_EN |
			   B_AX_WDE_BURST_NUM_ERR_INT_EN |
			   B_AX_WDE_NULL_PKT_ERR_INT_EN |
			   B_AX_WDE_FLOW_CTRL_ERR_INT_EN);

		val32 |= ((B_AX_OTHER_STF_WROQT_UNDERFLOW_INT_EN &
			   DMAC_OTHER_STF_WROQT_UNDERFLOW_SER_EN) |
			  (B_AX_OTHER_STF_WROQT_OVERFLOW_INT_EN &
			   DMAC_OTHER_STF_WROQT_OVERFLOW_SER_EN) |
			  (B_AX_OTHER_STF_WRFF_UNDERFLOW_INT_EN &
			   DMAC_OTHER_STF_WRFF_UNDERFLOW_SER_EN) |
			  (B_AX_OTHER_STF_WRFF_OVERFLOW_INT_EN &
			   DMAC_OTHER_STF_WRFF_OVERFLOW_SER_EN) |
			  (B_AX_OTHER_STF_CMD_UNDERFLOW_INT_EN &
			   DMAC_OTHER_STF_CMD_UNDERFLOW_SER_EN) |
			  (B_AX_OTHER_STF_CMD_OVERFLOW_INT_EN &
			   DMAC_OTHER_STF_CMD_OVERFLOW_SER_EN) |
			  (B_AX_HOST_ADDR_INFO_LEN_ZERO_ERR_INT_EN &
			   DMAC_HOST_ADDR_INFO_LEN_ZERO_ERR_SER_EN) |
			  (B_AX_CPU_ADDR_INFO_LEN_ZERO_ERR_INT_EN &
			   DMAC_CPU_ADDR_INFO_LEN_ZERO_ERR_SER_EN) |
			  (B_AX_PLE_OUTPUT_ERR_INT_EN &
			   DMAC_PLE_OUTPUT_ERR_SER_EN) |
			  (B_AX_PLE_RESP_ERR_INT_EN &
			   DMAC_PLE_RESP_ERR_SER_EN) |
			  (B_AX_PLE_BURST_NUM_ERR_INT_EN &
			   DMAC_PLE_BURST_NUM_ERR_SER_EN) |
			  (B_AX_PLE_NULL_PKT_ERR_INT_EN &
			   DMAC_PLE_NULL_PKT_ERR_SER_EN) |
			  (B_AX_PLE_FLOW_CTRL_ERR_INT_EN &
			   DMAC_PLE_FLOW_CTRL_ERR_SER_EN) |
			  (B_AX_WDE_OUTPUT_ERR_INT_EN &
			   DMAC_WDE_OUTPUT_ERR_SER_EN) |
			  (B_AX_WDE_RESP_ERR_INT_EN &
			   DMAC_WDE_RESP_ERR_SER_EN) |
			  (B_AX_WDE_BURST_NUM_ERR_INT_EN &
			   DMAC_WDE_BURST_NUM_ERR_SER_EN) |
			  (B_AX_WDE_NULL_PKT_ERR_INT_EN &
			   DMAC_WDE_NULL_PKT_ERR_SER_EN) |
			  (B_AX_WDE_FLOW_CTRL_ERR_INT_EN &
			   DMAC_WDE_FLOW_CTRL_ERR_SER_EN));
		MAC_REG_W32(R_AX_OTHER_DISPATCHER_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 cpuio_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_CPUIO_ERR_IMR);
		val32 &= ~(B_AX_WDEBUF_OP_ERR_INT_EN |
			   B_AX_WDEQUE_OP_ERR_INT_EN |
			   B_AX_PLEBUF_OP_ERR_INT_EN |
			   B_AX_PLEQUE_OP_ERR_INT_EN);

		val32 |= ((B_AX_WDEBUF_OP_ERR_INT_EN &
			   DMAC_WDEBUF_OP_ERR_SER_EN) |
			  (B_AX_WDEQUE_OP_ERR_INT_EN &
			   DMAC_WDEQUE_OP_ERR_SER_EN) |
			  (B_AX_PLEBUF_OP_ERR_INT_EN &
			   DMAC_PLEBUF_OP_ERR_SER_EN) |
			  (B_AX_PLEQUE_OP_ERR_INT_EN &
			   DMAC_PLEQUE_OP_ERR_SER_EN));
		MAC_REG_W32(R_AX_CPUIO_ERR_IMR, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 bbrpt_imr_enable(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
		u32 val32;

		val32 = MAC_REG_R32(R_AX_BBRPT_COM_ERR_IMR_ISR);
		val32 &= ~(B_AX_BBRPT_COM_NULL_PLPKTID_ERR_INT_EN);

		val32 |= ((B_AX_BBRPT_COM_NULL_PLPKTID_ERR_INT_EN &
			   DMAC_BBRPT_COM_NULL_PLPKTID_ERR_SER_EN));
		MAC_REG_W32(R_AX_BBRPT_COM_ERR_IMR_ISR, val32);

		val32 = MAC_REG_R32(R_AX_BBRPT_CHINFO_ERR_IMR_ISR);
		val32 &= ~(B_AX_BBPRT_CHIF_BB_TO_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_OVF_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_BOVF_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_HDRL_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_LEFT1_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_LEFT2_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_NULL_ERR_INT_EN |
			   B_AX_BBPRT_CHIF_TO_ERR_INT_EN);

		val32 |= ((B_AX_BBPRT_CHIF_BB_TO_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_BB_TO_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_OVF_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_OVF_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_BOVF_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_BOVF_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_HDRL_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_HDRL_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_LEFT1_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_LEFT1_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_LEFT2_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_LEFT2_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_NULL_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_NULL_ERR_SER_EN) |
			  (B_AX_BBPRT_CHIF_TO_ERR_INT_EN &
			   DMAC_BBPRT_CHIF_TO_ERR_SER_EN));
		MAC_REG_W32(R_AX_BBRPT_CHINFO_ERR_IMR_ISR, val32);

		val32 = MAC_REG_R32(R_AX_BBRPT_DFS_ERR_IMR_ISR);
		val32 &= ~(B_AX_BBRPT_DFS_TO_ERR_INT_EN);

		val32 |= ((B_AX_BBRPT_DFS_TO_ERR_INT_EN &
			   DMAC_BBRPT_DFS_TO_ERR_SER_EN));
		MAC_REG_W32(R_AX_BBRPT_DFS_ERR_IMR_ISR, val32);

		val32 = MAC_REG_R32(R_AX_LA_ERRFLAG);
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
#if MAC_AX_8852A_SUPPORT
			val32 &= ~(B_AX_LA_IMR_DATA_LOSS_ERR);

			val32 |= ((B_AX_LA_IMR_DATA_LOSS_ERR &
				   DMAC_LA_IMR_DATA_LOSS_ERR));
#endif
		} else {
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
			val32 &= ~(B_AX_LA_IMR_DATA_LOSS);

			val32 |= ((B_AX_LA_IMR_DATA_LOSS &
				   DMAC_LA_IMR_DATA_LOSS_ERR));
#endif
		}
		MAC_REG_W32(R_AX_LA_ERRFLAG, val32);
	}
#endif
	return MACSUCCESS;
}

static u32 sta_sch_init(struct mac_ax_adapter *adapter,
			struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;
	u32 cnt, ret, val32;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val8 = MAC_REG_R8(R_AX_SS_CTRL);
	val8 |= B_AX_SS_EN;
	MAC_REG_W8(R_AX_SS_CTRL, val8);

	cnt = TRXCFG_WAIT_CNT;
	while (cnt--) {
		if (MAC_REG_R32(R_AX_SS_CTRL) & B_AX_SS_INIT_DONE_1)
			break;
		PLTFM_DELAY_US(TRXCFG_WAIT_US);
	}

	if (!++cnt) {
		PLTFM_MSG_ERR("[ERR]STA scheduler init\n");
		return MACPOLLTO;
	}

	MAC_REG_W32(R_AX_SS_CTRL,
		    MAC_REG_R32(R_AX_SS_CTRL) | B_AX_SS_WARM_INIT_FLG);

	val32 = MAC_REG_R32(R_AX_SS_CTRL);
	if (info->trx_mode == MAC_AX_TRX_SW_MODE)
		val32 |= B_AX_SS_NONEMPTY_SS2FINFO_EN;
	else
		val32 &= ~B_AX_SS_NONEMPTY_SS2FINFO_EN;
	MAC_REG_W32(R_AX_SS_CTRL, val32);

	ret = _patch_ss2f_path(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]_patch_ss2f_path\n");
		return ret;
	}

	if (info->trx_mode == MAC_AX_TRX_SW_MODE) {
		// Set UL SS2FRPT MAX user number
		val32 = 0x0;
		val32 = SET_CLR_WORD(val32, MAX_ULSS2F_SU_STA_NUM, B_AX_SS_MAX_SU_NUM_UL);
		val32 = SET_CLR_WORD(val32, MAX_ULSS2F_TWT_STA_NUM, B_AX_SS_TWT_MAX_SU_NUM_UL);
		val32 = SET_CLR_WORD(val32, MAX_ULSS2F_RU_STA_NUM, B_AX_SS_MAX_RU_NUM_UL);
		MAC_REG_W32(R_AX_SS_UL_RPT_CRTL, val32);
		// Set UL SS2FRPT BSR Threshold
		val32 = SET_CLR_WORD(val32, UL_NORMAL_SS2FWRPT_BSR_THRES, B_AX_SS_BSR_THR_0);
		val32 = SET_CLR_WORD(val32, UL_LATCY_SS2FWRPT_BSR_THRES, B_AX_SS_BSR_THR_1);
		MAC_REG_W32(R_AX_SS_BSR_CTRL, val32);

		val32 = 0x0;
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_RU_STA_NUM, B_AX_SS_MAX_RU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_RU_STA_NUM, B_AX_SS_MAX_RU_NUM_1);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_RU_STA_NUM, B_AX_SS_TWT_MAX_RU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_RU_STA_NUM, B_AX_SS_TWT_MAX_RU_NUM_1);
		MAC_REG_W32(R_AX_SS_DL_RU_RPT_CRTL, val32);

		val32 = 0x0;
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_MU_STA_NUM, B_AX_SS_MAX_MU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_MU_STA_NUM, B_AX_SS_MAX_MU_NUM_1);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_MU_STA_NUM, B_AX_SS_TWT_MAX_MU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_MU_STA_NUM, B_AX_SS_TWT_MAX_MU_NUM_1);
		MAC_REG_W32(R_AX_SS_DL_MU_RPT_CRTL, val32);

		val32 = MAC_REG_R32(R_AX_SS_DL_RPT_CRTL);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_SU_STA_NUM, B_AX_SS_MAX_SU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_SU_STA_NUM, B_AX_SS_MAX_SU_NUM_1);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_SU_STA_NUM, B_AX_SS_TWT_MAX_SU_NUM_0);
		val32 = SET_CLR_WORD(val32, MAX_DLSS2F_SU_STA_NUM, B_AX_SS_TWT_MAX_SU_NUM_1);
		MAC_REG_W32(R_AX_SS_DL_RPT_CRTL, val32);
	}

	return MACSUCCESS;
}

static u32 scheduler_init(struct mac_ax_adapter *adapter, u8 band, struct mac_ax_trx_info *info)
{
	u32 reg, val32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_edca_param edca_para;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_1_C1 : R_AX_PREBKF_CFG_1;
	val32 = MAC_REG_R32(reg);
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		val32 = SET_CLR_WORD(val32, SIFS_MACTXEN_T1_V0,
				     B_AX_SIFS_MACTXEN_T1);
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
		val32 = SET_CLR_WORD(val32, SIFS_MACTXEN_T1_V1,
				     B_AX_SIFS_MACTXEN_T1);
	MAC_REG_W32(reg, val32);

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_SCH_EXT_CTRL_C1 : R_AX_SCH_EXT_CTRL;
		val32 = MAC_REG_R32(reg) | B_AX_PORT_RST_TSF_ADV;
		MAC_REG_W32(reg, val32);
#endif
	}

#if MAC_AX_ASIC_TEMP
	reg = band == MAC_AX_BAND_1 ? R_AX_CCA_CFG_0_C1 : R_AX_CCA_CFG_0;
	val32 = MAC_REG_R32(reg) & ~(B_AX_BTCCA_EN);
	MAC_REG_W32(reg, val32);
#endif

#if MAC_AX_FEATURE_HV
	reg = band == MAC_AX_BAND_1 ? R_AX_CCA_CFG_0_C1 : R_AX_CCA_CFG_0;
	val32 = MAC_REG_R32(reg);
	if (adapter->env == HV_AX_FPGA) {
		if (info->trx_mode == MAC_AX_TRX_LOOPBACK) {
			val32 = val32 & ~(B_AX_CCA_EN) & ~(B_AX_SEC20_EN) &
				~(B_AX_SEC40_EN) & ~(B_AX_SEC80_EN) &
				~(B_AX_EDCCA_EN);
		}
	} else if (adapter->env == HV_AX_ASIC || adapter->env == HV_AX_PXP) {
		val32 = val32;
	} else {
		PLTFM_MSG_ERR("[ERR]Unknown env %d\n", adapter->env);
		return MACNOITEM;
	}

	MAC_REG_W32(reg, val32);
#endif

#ifdef PHL_FEATURE_AP
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_0_C1 : R_AX_PREBKF_CFG_0;
		val32 = MAC_REG_R32(reg);
		val32 = SET_CLR_WORD(val32, SCH_PREBKF_16US, B_AX_PREBKF_TIME);
		MAC_REG_W32(reg, val32);
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		if (!(MAC_REG_R32(R_AX_SEC_ENG_CTRL) & B_AX_TX_PARTIAL_MODE)) {
			reg = band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_0_C1 : R_AX_PREBKF_CFG_0;
			val32 = MAC_REG_R32(reg);
			val32 = SET_CLR_WORD(val32, SCH_PREBKF_16US, B_AX_PREBKF_TIME);
			MAC_REG_W32(reg, val32);
		}
	}
#endif

	if (band == MAC_AX_BAND_0) {
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
			val32 = MAC_REG_R32(R_AX_CCA_CFG_0);
			val32 = SET_CLR_WORD(val32, 0x6a,
					     B_AX_R_SIFS_AGGR_TIME);
			MAC_REG_W32(R_AX_CCA_CFG_0, val32);
#endif
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
#if MAC_AX_8852C_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
			val32 = MAC_REG_R32(R_AX_CCA_CFG_0);
			val32 = SET_CLR_WORD(val32, 0x6a,
					     B_AX_R_SIFS_AGGR_TIME_V1);
			MAC_REG_W32(R_AX_CCA_CFG_0, val32);
#endif
		}
	}
#else /*for NIC mode setting*/
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_0_C1 : R_AX_PREBKF_CFG_0;
		val32 = MAC_REG_R32(reg);
		val32 = SET_CLR_WORD(val32, SCH_PREBKF_24US, B_AX_PREBKF_TIME);
		MAC_REG_W32(reg, val32);
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		if (!(MAC_REG_R32(R_AX_SEC_ENG_CTRL) & B_AX_TX_PARTIAL_MODE)) {
			reg = band == MAC_AX_BAND_1 ? R_AX_PREBKF_CFG_0_C1 : R_AX_PREBKF_CFG_0;
			val32 = MAC_REG_R32(reg);
			val32 = SET_CLR_WORD(val32, SCH_PREBKF_24US, B_AX_PREBKF_TIME);
			MAC_REG_W32(reg, val32);
		}
	}
#endif
#endif
	edca_para.band = band;
	edca_para.path = MAC_AX_CMAC_PATH_SEL_BCN;
	edca_para.ecw_min = 2;
	edca_para.ecw_max = 3;
	edca_para.aifs_us = BCN_IFS_25US;
	ret = set_hw_edca_param(adapter, &edca_para);

	return ret;
}

static u32 mpdu_proc_init(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	MAC_REG_W32(R_AX_ACTION_FWD0, TRXCFG_MPDU_PROC_ACT_FRWD);
	MAC_REG_W32(R_AX_TF_FWD, TRXCFG_MPDU_PROC_TF_FRWD);
	val32 = MAC_REG_R32(R_AX_MPDU_PROC);
	val32 |= (B_AX_APPEND_FCS | B_AX_A_ICV_ERR);
	MAC_REG_W32(R_AX_MPDU_PROC, val32);
	MAC_REG_W32(R_AX_CUT_AMSDU_CTRL, TRXCFG_MPDU_PROC_CUT_CTRL);

	return MACSUCCESS;
}

static u32 sec_eng_init(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32 = 0;
	u32 ret;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
	// init clock
	val32 |= (B_AX_CLK_EN_CGCMP | B_AX_CLK_EN_WAPI | B_AX_CLK_EN_WEP_TKIP);
	// init TX encryption
	val32 |= (B_AX_SEC_TX_ENC | B_AX_SEC_RX_DEC);
	val32 |= (B_AX_MC_DEC | B_AX_BC_DEC);
	val32 |= (B_AX_BMC_MGNT_DEC | B_AX_UC_MGNT_DEC);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	val32 &= ~B_AX_TX_PARTIAL_MODE;
#endif
	MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);

	//init MIC ICV append
	val32 = MAC_REG_R32(R_AX_SEC_MPDU_PROC);
	val32 |= (B_AX_APPEND_ICV | B_AX_APPEND_MIC);

	// option init
	MAC_REG_W32(R_AX_SEC_MPDU_PROC, val32);

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		val32 = MAC_REG_R32(R_AX_SEC_DEBUG1);
		val32 = SET_CLR_WORD(val32, B_AX_TX_TO, B_AX_TX_TIMEOUT_SEL);
		MAC_REG_W32(R_AX_SEC_DEBUG1, val32);
	}
#endif

#ifdef PHL_FEATURE_AP
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = MAC_REG_R32(R_AX_RCR);
			val32 = SET_CLR_WORD(val32, DRVINFO_PATCH_SIZE, B_AX_DRV_INFO_SIZE);
			MAC_REG_W32(R_AX_RCR, val32);
		}
#endif
#endif

	return MACSUCCESS;
}

static u32 tmac_init(struct mac_ax_adapter *adapter, u8 band,
		     struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg, val32, ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ? R_AX_MAC_LOOPBACK_C1 : R_AX_MAC_LOOPBACK;
	val32 = MAC_REG_R32(reg);
	if (info->trx_mode == MAC_AX_TRX_LOOPBACK)
		val32 |= B_AX_MACLBK_EN;
	else
		val32 &= ~B_AX_MACLBK_EN;
	MAC_REG_W32(reg, val32);

#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
	reg = band == MAC_AX_BAND_1 ? R_AX_TCR0_C1 : R_AX_TCR0;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, TCR_UDF_THSD, B_AX_TCR_UDF_THSD);
	MAC_REG_W32(reg, val32);

	reg = band == MAC_AX_BAND_1 ? R_AX_TXD_FIFO_CTRL_C1 : R_AX_TXD_FIFO_CTRL;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, TXDFIFO_HIGH_MCS_THRE, B_AX_TXDFIFO_HIGH_MCS_THRE);
	val32 = SET_CLR_WORD(val32, TXDFIFO_LOW_MCS_THRE, B_AX_TXDFIFO_LOW_MCS_THRE);
	MAC_REG_W32(reg, val32);
#endif
#if MAC_AX_FPGA_TEST
	reg = band == MAC_AX_BAND_1 ? R_AX_MAC_LOOPBACK_C1 : R_AX_MAC_LOOPBACK;
	val32 = MAC_REG_R32(reg);
	if (info->trx_mode == MAC_AX_TRX_LOOPBACK)
		val32 = SET_CLR_WORD(val32, LBK_PLCP_DLY_FPGA,
				     B_AX_MACLBK_PLCP_DLY);
	else
		val32 = SET_CLR_WORD(val32, LBK_PLCP_DLY_DEF,
				     B_AX_MACLBK_PLCP_DLY);
	MAC_REG_W32(reg, val32);
#endif
	return MACSUCCESS;
}

static u32 trxptcl_init(struct mac_ax_adapter *adapter, u8 band,
			struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_rrsr_cfg cfg = {0};
	u32 reg, val32, ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_TRXPTCL_RESP_0_C1 : R_AX_TRXPTCL_RESP_0;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, WMAC_SPEC_SIFS_CCK,
			     B_AX_WMAC_SPEC_SIFS_CCK);
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		val32 = SET_CLR_WORD(val32, WMAC_SPEC_SIFS_OFDM_52A,
				     B_AX_WMAC_SPEC_SIFS_OFDM);
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
		val32 = SET_CLR_WORD(val32, WMAC_SPEC_SIFS_OFDM_52B,
				     B_AX_WMAC_SPEC_SIFS_OFDM);
	else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		val32 = SET_CLR_WORD(val32, WMAC_SPEC_SIFS_OFDM_51B,
				     B_AX_WMAC_SPEC_SIFS_OFDM);
	else
		val32 = SET_CLR_WORD(val32, WMAC_SPEC_SIFS_OFDM_52C,
				     B_AX_WMAC_SPEC_SIFS_OFDM);
	MAC_REG_W32(reg, val32);

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_RXTRIG_TEST_USER_2_C1 : R_AX_RXTRIG_TEST_USER_2;
	val32 = MAC_REG_R32(reg);
	val32 |= B_AX_RXTRIG_FCSCHK_EN;
	MAC_REG_W32(reg, val32);

	/*disable 5.5M CCK rate response for PHY performance consideration*/
	ret = p_ops->get_rrsr_cfg(adapter, &cfg);
	cfg.cck_cfg = WMAC_CCK_EN_1M;
	cfg.rrsr_rate_en |= WMAC_RRSR_RATE_LEGACY_EN;

	/* refer to rx rate and cmac table, choose the smaller as resp initial rate */
	cfg.ref_rate_sel = REF2RXRATEANDCCTBL;
	ret = p_ops->set_rrsr_cfg(adapter, &cfg);

	return MACSUCCESS;
}

static u32 rmac_init(struct mac_ax_adapter *adapter, u8 band,
		     struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct rst_bacam_info rst_info;
	u32 ret;
	u8 val8;
	u16 val16;
	u32 val32, rx_max_len, rx_max_pg, reg;
	u32 rx_min_qta, rx_max_lenb;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (band == MAC_AX_BAND_0) {
		rst_info.type = BACAM_RST_ALL;
		rst_info.ent = 0;
		ret = rst_bacam(adapter, &rst_info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]bacam rst %d\n", ret);
			return ret;
		}
	}

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_RESPBA_CAM_CTRL_C1 : R_AX_RESPBA_CAM_CTRL;
	val8 = MAC_REG_R8(reg) | B_AX_SSN_SEL;
	MAC_REG_W8(reg, val8);

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_DLK_PROTECT_CTL_C1 : R_AX_DLK_PROTECT_CTL;
	val16 = MAC_REG_R16(reg);
	val16 = SET_CLR_WORD(val16, TRXCFG_RMAC_DATA_TO,
			     B_AX_RX_DLK_DATA_TIME);
	val16 = SET_CLR_WORD(val16, TRXCFG_RMAC_CCA_TO,
			     B_AX_RX_DLK_CCA_TIME);
	val16 |= B_AX_RX_DLK_RST_EN;
	MAC_REG_W16(reg, val16);

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		reg = band == MAC_AX_BAND_1 ? R_AX_RCR_C1 : R_AX_RCR;
		val8 = MAC_REG_R8(reg);
		if (band == MAC_AX_BAND_0 &&
		    info->trx_mode == MAC_AX_TRX_SW_MODE)
			val8 = SET_CLR_WORD(val8, 0xF, B_AX_CH_EN);
		else
			val8 = SET_CLR_WORD(val8, 0x1, B_AX_CH_EN);
		MAC_REG_W8(reg, val8);
#endif
	} else {
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT
		reg = band == MAC_AX_BAND_1 ? R_AX_RCR_C1 : R_AX_RCR;
		val8 = MAC_REG_R8(reg);
		if (band == MAC_AX_BAND_0 &&
		    info->trx_mode == MAC_AX_TRX_SW_MODE)
			val8 = (u8)SET_CLR_WORD(val8, 0xF, B_AX_CH_EN_V1);
		else
			val8 = (u8)SET_CLR_WORD(val8, 0x1, B_AX_CH_EN_V1);
		MAC_REG_W8(reg, val8);
#endif
	}
	rx_min_qta = band == MAC_AX_BAND_1 ?
		     adapter->dle_info.c1_rx_qta : adapter->dle_info.c0_rx_qta;
	rx_max_pg = rx_min_qta > PLD_RLS_MAX_PG ? PLD_RLS_MAX_PG : rx_min_qta;
	rx_max_lenb = rx_max_pg * adapter->dle_info.ple_pg_size;
	if (rx_max_lenb < RX_SPEC_MAX_LEN)
		PLTFM_MSG_WARN("B%dRX max len %d lower than spec max %d\n",
			       band, rx_max_lenb, RX_SPEC_MAX_LEN);
	else
		rx_max_lenb = RX_SPEC_MAX_LEN;

	/* rx_max_len shall not be larger than B_AX_RX_MPDU_MAX_LEN_MSK */
	rx_max_len = rx_max_lenb / RX_MAX_LEN_UNIT;

	reg = band == MAC_AX_BAND_1 ? R_AX_RX_FLTR_OPT_C1 : R_AX_RX_FLTR_OPT;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, rx_max_len, B_AX_RX_MPDU_MAX_LEN);
	MAC_REG_W32(reg, val32);

	/* rmac timer src reconfig eco item 96 */
	_patch_wmac_timer_src(adapter, band);

	/* Add drv_info dbg size as dummy (SDIO) */
	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO &&
	    adapter->hw_info->chip_id == MAC_AX_CHIP_ID_8852A) {
		val16 = MAC_REG_R16(R_AX_RCR);
		MAC_REG_W16(R_AX_RCR, val16 |
			    SET_WORD(SDIO_DRV_INFO_SIZE, B_AX_DRV_INFO_SIZE));
	}

	/* NOT ALL vendors calculate VHT SIG-B's CRC */
	reg = band == MAC_AX_BAND_1 ?
	      R_AX_PLCP_HDR_FLTR_C1 : R_AX_PLCP_HDR_FLTR;
	val8 = MAC_REG_R8(reg) & ~B_AX_VHT_SU_SIGB_CRC_CHK;
	MAC_REG_W8(reg, val8);

	return MACSUCCESS;
}

static u32 cmac_com_init(struct mac_ax_adapter *adapter, u8 band,
			 struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, reg;
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_TX_SUB_CARRIER_VALUE_C1 : R_AX_TX_SUB_CARRIER_VALUE;
	val32 = MAC_REG_R32(reg);
	if (info->trx_mode == MAC_AX_TRX_LOOPBACK) {
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_20M_4, B_AX_TXSC_20M);
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_40M_4, B_AX_TXSC_40M);
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_80M_4, B_AX_TXSC_80M);
	} else {
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_20M_0, B_AX_TXSC_20M);
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_40M_0, B_AX_TXSC_40M);
		val32 = SET_CLR_WORD(val32, S_AX_TXSC_80M_0, B_AX_TXSC_80M);
	}
	MAC_REG_W32(reg, val32);

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_PTCL_RRSR1_C1 : R_AX_PTCL_RRSR1;
		val32 = MAC_REG_R32(reg);
		val32 = SET_CLR_WORD(val32, RRSR_OFDM_CCK_EN,
				     B_AX_RRSR_RATE_EN);
		MAC_REG_W32(reg, val32);
	}

	return MACSUCCESS;
}

static void _patch_vht_ampdu_max_len(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 reg;

	if (!chk_patch_vht_ampdu_max_len(adapter))
		return;

	reg = band == MAC_AX_BAND_1 ?
	      R_AX_AGG_LEN_VHT_0_C1 : R_AX_AGG_LEN_VHT_0;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, AMPDU_MAX_LEN_VHT_262K,
			     B_AX_AMPDU_MAX_LEN_VHT);
	MAC_REG_W32(reg, val32);
}

static u32 ptcl_init(struct mac_ax_adapter *adapter, u8 band,
		     struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret;
	u8 val8;
	u32 reg;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
		ret = is_qta_poh(adapter, info->qta_mode, &val8);
		if (ret)
			return ret;
		if (val8) {
			reg = band == MAC_AX_BAND_1 ?
			      R_AX_SIFS_SETTING_C1 : R_AX_SIFS_SETTING;
			val32 = MAC_REG_R32(reg);
			val32 = SET_CLR_WORD(val32, S_AX_CTS2S_TH_1K,
					     B_AX_HW_CTS2SELF_PKT_LEN_TH);
			val32 = SET_CLR_WORD(val32, S_AX_CTS2S_TH_SEC_256B,
					     B_AX_HW_CTS2SELF_PKT_LEN_TH_TWW);
			val32 |= B_AX_HW_CTS2SELF_EN;
			MAC_REG_W32(reg, val32);

			reg = band == MAC_AX_BAND_1 ?
			      R_AX_PTCL_FSM_MON_C1 : R_AX_PTCL_FSM_MON;
			val32 = MAC_REG_R32(reg);
			val32 = SET_CLR_WORD(val32, S_AX_PTCL_TO_2MS,
					     B_AX_PTCL_TX_ARB_TO_THR);
			val32 &= ~B_AX_PTCL_TX_ARB_TO_MODE;
			MAC_REG_W32(reg, val32);
		}
	}

	if (band == MAC_AX_BAND_0) {
		val8 = MAC_REG_R8(R_AX_PTCL_COMMON_SETTING_0);
		if (info->trx_mode == MAC_AX_TRX_SW_MODE) {
			val8 &= ~(B_AX_CMAC_TX_MODE_0 | B_AX_CMAC_TX_MODE_1);
			val8 |= B_AX_PTCL_TRIGGER_SS_EN_0 |
				B_AX_PTCL_TRIGGER_SS_EN_1 |
				B_AX_PTCL_TRIGGER_SS_EN_UL;
		} else {
			val8 |= B_AX_CMAC_TX_MODE_0 | B_AX_CMAC_TX_MODE_1;
			val8 &= ~(B_AX_PTCL_TRIGGER_SS_EN_0 |
				  B_AX_PTCL_TRIGGER_SS_EN_1 |
				  B_AX_PTCL_TRIGGER_SS_EN_UL);
		}
		MAC_REG_W8(R_AX_PTCL_COMMON_SETTING_0, val8);

		val8 = MAC_REG_R8(R_AX_PTCLRPT_FULL_HDL);
		val8 = SET_CLR_WORD(val8, FWD_TO_WLCPU, B_AX_SPE_RPT_PATH);
		MAC_REG_W8(R_AX_PTCLRPT_FULL_HDL, val8);
	} else if (band == MAC_AX_BAND_1) {
		val8 = MAC_REG_R8(R_AX_PTCLRPT_FULL_HDL_C1);
		val8 = SET_CLR_WORD(val8, FWD_TO_WLCPU, B_AX_SPE_RPT_PATH);
		MAC_REG_W8(R_AX_PTCLRPT_FULL_HDL_C1, val8);
	}

	_patch_vht_ampdu_max_len(adapter, band);

	return MACSUCCESS;
}

static u32 cmac_dma_init(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		u32 reg_rx_ctrl = (band == MAC_AX_BAND_1) ?
				 R_AX_RXDMA_CTRL_0_C1 : R_AX_RXDMA_CTRL_0;
		u8 val8 = MAC_REG_R8(reg_rx_ctrl) & ~RX_FULL_MODE;

		MAC_REG_W8(reg_rx_ctrl, val8);
#endif
	} else {
		return MACSUCCESS;
	}

	return MACSUCCESS;
}

static void _patch_dis_resp_chk(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg, val32;
	u32 b_rsp_chk_nav, b_rsp_chk_cca;

	b_rsp_chk_nav = B_AX_RSP_CHK_TXNAV | B_AX_RSP_CHK_INTRA_NAV |
			B_AX_RSP_CHK_BASIC_NAV;

	b_rsp_chk_cca = B_AX_RSP_CHK_SEC_CCA_80 | B_AX_RSP_CHK_SEC_CCA_40 |
			B_AX_RSP_CHK_SEC_CCA_20 | B_AX_RSP_CHK_BTCCA |
			B_AX_RSP_CHK_EDCCA | B_AX_RSP_CHK_CCA;

	if (chk_patch_dis_resp_chk(adapter)) {
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_RSP_CHK_SIG_C1 : R_AX_RSP_CHK_SIG;
		val32 = MAC_REG_R32(reg) & ~b_rsp_chk_nav;
		MAC_REG_W32(reg, val32);

		reg = band == MAC_AX_BAND_1 ?
		      R_AX_TRXPTCL_RESP_0_C1 : R_AX_TRXPTCL_RESP_0;
		val32 = MAC_REG_R32(reg) & ~b_rsp_chk_cca;
		MAC_REG_W32(reg, val32);
	} else {
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_RSP_CHK_SIG_C1 : R_AX_RSP_CHK_SIG;
		val32 = MAC_REG_R32(reg) | b_rsp_chk_nav;
		MAC_REG_W32(reg, val32);

		reg = band == MAC_AX_BAND_1 ?
		      R_AX_TRXPTCL_RESP_0_C1 : R_AX_TRXPTCL_RESP_0;
		val32 = MAC_REG_R32(reg) | b_rsp_chk_cca;
		/*response frame could ignore CCA/EDCCA according to SPEC*/
		val32 &= ~B_AX_WMAC_RESP_ACK_BA_CHK_CCA;
		MAC_REG_W32(reg, val32);
	}
}

static u32 cca_ctrl_init(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, reg;
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	reg = band == MAC_AX_BAND_1 ? R_AX_CCA_CONTROL_C1 : R_AX_CCA_CONTROL;
	val32 = MAC_REG_R32(reg);
	val32 |= (B_AX_TB_CHK_BASIC_NAV | B_AX_TB_CHK_BTCCA |
		  B_AX_TB_CHK_EDCCA | B_AX_TB_CHK_CCA_P20 |
		  B_AX_SIFS_CHK_BTCCA | B_AX_SIFS_CHK_CCA_P20 |
		  B_AX_CTN_CHK_INTRA_NAV |
		  B_AX_CTN_CHK_BASIC_NAV | B_AX_CTN_CHK_BTCCA |
		  B_AX_CTN_CHK_EDCCA | B_AX_CTN_CHK_CCA_S80 |
		  B_AX_CTN_CHK_CCA_S40 | B_AX_CTN_CHK_CCA_S20 |
		  B_AX_CTN_CHK_CCA_P20);

	val32 &= (~B_AX_TB_CHK_TX_NAV & ~B_AX_TB_CHK_CCA_S80 &
		  ~B_AX_TB_CHK_CCA_S40 & ~B_AX_TB_CHK_CCA_S20 &
		  ~B_AX_SIFS_CHK_CCA_S80 & ~B_AX_SIFS_CHK_CCA_S40 &
		  ~B_AX_SIFS_CHK_CCA_S20 & ~B_AX_CTN_CHK_TXNAV &
		  ~B_AX_SIFS_CHK_EDCCA);

	MAC_REG_W32(reg, val32);

	_patch_dis_resp_chk(adapter, band);

	return MACSUCCESS;
}

u32 _patch_rsp_ack(struct mac_ax_adapter *adapter,
		   struct mac_ax_resp_chk_cca *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, reg;
	u32 ret;

	if (chk_patch_rsp_ack(adapter) == PATCH_ENABLE)
		return MACSUCCESS;

	ret = check_mac_en(adapter, cfg->band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (cfg->resp_ack_chk_cca_en) {
		reg = cfg->band == MAC_AX_BAND_1 ? R_AX_TRXPTCL_RESP_0_C1 : R_AX_TRXPTCL_RESP_0;
		val32 = MAC_REG_R32(reg);
		val32 |= B_AX_WMAC_RESP_ACK_BA_CHK_CCA;
		MAC_REG_W32(reg, val32);
	} else {
		reg = cfg->band == MAC_AX_BAND_1 ? R_AX_TRXPTCL_RESP_0_C1 : R_AX_TRXPTCL_RESP_0;
		val32 = MAC_REG_R32(reg);
		val32 &= ~B_AX_WMAC_RESP_ACK_BA_CHK_CCA;
		MAC_REG_W32(reg, val32);
	}

	return MACSUCCESS;
}

u32 mac_sifs_chk_cca_en(struct mac_ax_adapter *adapter, u8 band, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, reg;
	u32 ret;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;
	if (en) {
		reg = band == MAC_AX_BAND_1 ? R_AX_CCA_CONTROL_C1 : R_AX_CCA_CONTROL;
		val32 = MAC_REG_R32(reg);
		val32 |= (B_AX_SIFS_CHK_CCA_S80 | B_AX_SIFS_CHK_CCA_S40 |
			  B_AX_SIFS_CHK_CCA_S20 | B_AX_SIFS_CHK_EDCCA);
		MAC_REG_W32(reg, val32);
	} else {
		reg = band == MAC_AX_BAND_1 ? R_AX_CCA_CONTROL_C1 : R_AX_CCA_CONTROL;
		val32 = MAC_REG_R32(reg);
		val32 &= ~(B_AX_SIFS_CHK_CCA_S80 | B_AX_SIFS_CHK_CCA_S40 |
			   B_AX_SIFS_CHK_CCA_S20 | B_AX_SIFS_CHK_EDCCA);
		MAC_REG_W32(reg, val32);
	}

	return MACSUCCESS;
}

static u32 nav_ctrl_init(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_2nav_info info;
	u32 ret;

	info.plcp_upd_nav_en = 1;
	info.tgr_fram_upd_nav_en = 1;

#ifdef PHL_FEATURE_AP
	info.nav_up = NAV_12MS;
#else
	info.nav_up = NAV_25MS;
#endif

	ret = mac_two_nav_cfg(adapter, &info);

	return MACSUCCESS;
}

u32 dmac_init(struct mac_ax_adapter *adapter, struct mac_ax_trx_info *info,
	      enum mac_ax_band band)
{
	u32 ret = 0;
	struct mac_ax_priv_ops *p_ops;

	ret = dle_init(adapter, info->qta_mode, MAC_AX_QTA_INVALID);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]DLE init %d\n", ret);
		return ret;
	}

	ret = preload_init(adapter, band, info->qta_mode);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]preload init B%d %d\n", band, ret);
		return ret;
	}

	ret = hfc_init(adapter, 1, 1, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]HCI FC init %d\n", ret);
		return ret;
	}

	ret = sta_sch_init(adapter, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]STA SCH init %d\n", ret);
		return ret;
	}

	ret = mpdu_proc_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]MPDU Proc init %d\n", ret);
		return ret;
	}

	ret = sec_eng_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]Security Engine init %d\n", ret);
		return ret;
	}

	p_ops = adapter_to_priv_ops(adapter);
	ret = p_ops->sec_info_tbl_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]sec info tbl init %d\n", ret);
		return ret;
	}

	return ret;
}

u32 cmac_init(struct mac_ax_adapter *adapter, struct mac_ax_trx_info *info,
	      enum mac_ax_band band)
{
	u32 ret;

	ret = scheduler_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d SCH init %d\n", band, ret);
		return ret;
	}

	ret = rst_port_info(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d rst port info %d\n", band, ret);
		return ret;
	}

	ret = addr_cam_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d ADDR_CAM reset %d\n", band, ret);
		return ret;
	}

	ret = rx_fltr_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d RX filter init %d\n", band, ret);
		return ret;
	}

	ret = cca_ctrl_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d CCA CTRL init %d\n", band, ret);
		return ret;
	}

	ret = nav_ctrl_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d NAV CTRL init %d\n", band, ret);
		return ret;
	}

	ret = spatial_reuse_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d Spatial Reuse init %d\n", band, ret);
		return ret;
	}

	ret = tmac_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d TMAC init %d\n", band, ret);
		return ret;
	}

	ret = trxptcl_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d TRXPTCL init %d\n", band, ret);
		return ret;
	}

	ret = rmac_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d RMAC init %d\n", band, ret);
		return ret;
	}

	ret = cmac_com_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d Com init %d\n", band, ret);
		return ret;
	}

	ret = ptcl_init(adapter, band, info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d PTCL init %d\n", band, ret);
		return ret;
	}

	ret = cmac_dma_init(adapter, band);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d DMA init %d\n", band, ret);
		return ret;
	}

	return ret;
}

u32 mac_check_access(struct mac_ax_adapter *adapter, u32 offset)
{
	if (offset >= CMAC1_START_ADDR && offset <= CMAC1_END_ADDR) {
		if (adapter->dle_info.qta_mode == MAC_AX_QTA_SCC)
			return MACHWNOTEN;
		else if (adapter->dle_info.qta_mode == MAC_AX_QTA_SCC_STF)
			return MACHWNOTEN;
		else
			return MACSUCCESS;
	} else {
		return MACSUCCESS;
	}
}

u32 ser_imr_config(struct mac_ax_adapter *adapter, u8 band,
		   enum mac_ax_hwmod_sel sel)
{
	u32 ret;

	ret = check_mac_en(adapter, band, sel);
	if (ret) {
		PLTFM_MSG_ERR("MAC%d band%d is not ready\n", sel, band);
		return ret;
	}

	if (sel == MAC_AX_DMAC_SEL) {
		ret = wdrls_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]wdrls_imr_enable %d\n", ret);
			return ret;
		}

		ret = wsec_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]wsec_imr_enable %d\n", ret);
			return ret;
		}

		ret = mpdu_trx_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]mpdu_trx_imr_enable %d\n", ret);
			return ret;
		}

		ret = sta_sch_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]sta_sch_imr_enable %d\n", ret);
			return ret;
		}

		ret = txpktctl_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]txpktctl_imr_enable %d\n", ret);
			return ret;
		}

		ret = wde_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]wde_imr_enable %d\n", ret);
			return ret;
		}

		ret = ple_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]ple_imr_enable %d\n", ret);
			return ret;
		}

		ret = pktin_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]pktin_imr_enable %d\n", ret);
			return ret;
		}

		ret = dispatcher_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]dispatcher_imr_enable %d\n", ret);
			return ret;
		}

		ret = cpuio_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]cpuio_imr_enable %d\n", ret);
			return ret;
		}

		ret = bbrpt_imr_enable(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]bbrpt_imr_enable %d\n", ret);
			return ret;
		}
	} else if (sel == MAC_AX_CMAC_SEL) {
		ret = scheduler_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]scheduler_imr_enable %d\n", ret);
			return ret;
		}

		ret = ptcl_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]ptcl_imr_enable %d\n", ret);
			return ret;
		}

		ret = cdma_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]cdma_imr_enable %d\n", ret);
			return ret;
		}

		ret = phy_intf_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]phy_intf_imr_enable %d\n", ret);
			return ret;
		}

		ret = rmac_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]rmac_imr_enable %d\n", ret);
			return ret;
		}

		ret = tmac_imr_enable(adapter, band);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]tmac_imr_enable %d\n", ret);
			return ret;
		}
	} else {
		PLTFM_MSG_ERR("illegal sel %d\n", sel);
		return MACNOITEM;
	}
	return MACSUCCESS;
}

u32 mac_enable_imr(struct mac_ax_adapter *adapter, u8 band,
		   enum mac_ax_hwmod_sel sel)
{
	u32 ret;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = p_ops->ser_imr_config(adapter, band, sel);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]ser_imr_config : %d\n", ret);
		return ret;
	}
	return MACSUCCESS;
}

u32 mac_trx_init(struct mac_ax_adapter *adapter, struct mac_ax_trx_info *info)
{
	u32 ret = 0;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
#if MAC_AX_COEX_INIT_EN
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
#endif
	u8 val8;

	/* Check TRX status is idle later. */
	ret = dmac_init(adapter, info, MAC_AX_BAND_0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]DMAC init %d\n", ret);
		return ret;
	}

	ret = cmac_init(adapter, info, MAC_AX_BAND_0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d init %d\n", MAC_AX_BAND_0, ret);
		return ret;
	}

#if MAC_AX_COEX_INIT_EN
	ret = p_ops->coex_mac_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] update coex setting %d\n", ret);
		return ret;
	}
#endif

	ret = is_qta_dbcc(adapter, info->qta_mode, &val8);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] is_qta_dbcc %d\n", ret);
		return ret;
	}
	if (val8) {
		ret = mac_ops->dbcc_enable(adapter, info, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]dbcc_enable init %d\n", ret);
			return ret;
		}
	}

	ret = mac_enable_imr(adapter, MAC_AX_BAND_0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] enable DMAC IMR %d\n", ret);
		return ret;
	}

	ret = mac_enable_imr(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] enable CMAC0 IMR %d\n", ret);
		return ret;
	}

	ret = mac_err_imr_ctrl(adapter, MAC_AX_FUNC_EN);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] enable err IMR %d\n", ret);
		return ret;
	}

	ret = set_host_rpr(adapter, info->rpr_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]set host rpr %d\n", ret);
		return ret;
	}

	ret = set_l2_status(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s %d\n", __func__, ret);
		return ret;
	}

	return MACSUCCESS;
}

u32 mac_tx_mode_sel(struct mac_ax_adapter *adapter,
		    struct mac_ax_mac_tx_mode_sel *mode_sel)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

	ret = check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = MAC_REG_R32(R_AX_SS_DL_RPT_CRTL);
	val32 &= ~(B_AX_SS_TXOP_MODE_0 | B_AX_SS_TXOP_MODE_1 |
		   B_AX_SS_TXOP_MODE_2 | B_AX_SS_TXOP_MODE_3);
	val32 |= (mode_sel->txop_rot_wmm0_en ? B_AX_SS_TXOP_MODE_0 : 0) |
		 (mode_sel->txop_rot_wmm1_en ? B_AX_SS_TXOP_MODE_1 : 0) |
		 (mode_sel->txop_rot_wmm2_en ? B_AX_SS_TXOP_MODE_2 : 0) |
		 (mode_sel->txop_rot_wmm3_en ? B_AX_SS_TXOP_MODE_3 : 0);
	MAC_REG_W32(R_AX_SS_DL_RPT_CRTL, val32);

	return MACSUCCESS;
}

u32 mac_two_nav_cfg(struct mac_ax_adapter *adapter,
		    struct mac_ax_2nav_info *info)

{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32 = 0;

	val32 = MAC_REG_R32(R_AX_WMAC_NAV_CTL);
	val32 |= ((info->plcp_upd_nav_en ? B_AX_WMAC_PLCP_UP_NAV_EN : 0)) |
		 ((info->tgr_fram_upd_nav_en ? B_AX_WMAC_TF_UP_NAV_EN : 0));

	val32 = SET_CLR_WORD(val32, info->nav_up, B_AX_WMAC_NAV_UPPER);

	if (info->nav_up > NAV_UPPER_DEFAULT)
		val32 |= B_AX_WMAC_NAV_UPPER_EN;

	MAC_REG_W32(R_AX_WMAC_NAV_CTL, val32);
	return MACSUCCESS;
}

u32 mac_feat_init(struct mac_ax_adapter *adapter, struct mac_ax_trx_info *info)
{
	u32 ret;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = p_ops->bacam_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC BACAM init %d\n", ret);
		return ret;
	}
	return MACSUCCESS;
}
