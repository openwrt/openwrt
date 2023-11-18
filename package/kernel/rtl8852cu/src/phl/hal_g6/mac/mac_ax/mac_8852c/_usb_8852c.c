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
#include "_usb_8852c.h"
#include "../../mac_ax.h"
#if MAC_AX_8852C_SUPPORT

#if MAC_AX_USB_SUPPORT

u8 reg_read8_usb_8852c(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R8(addr);
}

void reg_write8_usb_8852c(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	PLTFM_REG_W8(addr, val);
}

u16 reg_read16_usb_8852c(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R16(addr);
}

void reg_write16_usb_8852c(struct mac_ax_adapter *adapter, u32 addr, u16 val)
{
	PLTFM_REG_W16(addr, val);
}

u32 reg_read32_usb_8852c(struct mac_ax_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R32(addr);
}

void reg_write32_usb_8852c(struct mac_ax_adapter *adapter, u32 addr, u32 val)
{
	PLTFM_REG_W32(addr, val);
}

u8 get_bulkout_id_8852c(struct mac_ax_adapter *adapter, u8 ch_dma, u8 mode)
{
	u8 bulkout_id = 0;

	if (mode == 0 && adapter->usb_info.ep5 && adapter->usb_info.ep6 &&
	    adapter->usb_info.ep12) {
		switch (ch_dma) {
		case MAC_AX_DMA_ACH0:
			bulkout_id = BULKOUTID3;
			break;
		case MAC_AX_DMA_ACH2:
			bulkout_id = BULKOUTID5;
			break;
		case MAC_AX_DMA_ACH4:
			bulkout_id = BULKOUTID4;
			break;
		case MAC_AX_DMA_ACH6:
			bulkout_id = BULKOUTID6;
			break;
		case MAC_AX_DMA_B0MG:
		case MAC_AX_DMA_B0HI:
			bulkout_id = BULKOUTID0;
			break;
		case MAC_AX_DMA_B1MG:
		case MAC_AX_DMA_B1HI:
			bulkout_id = BULKOUTID1;
			break;
		case MAC_AX_DMA_H2C:
			bulkout_id = BULKOUTID2;
			break;
		default:
			return USBEPMAPERR;
		}
	} else if ((mode == 1) && adapter->usb_info.ep5 &&
			adapter->usb_info.ep6 && adapter->usb_info.ep12) {
		switch (ch_dma) {
		case MAC_AX_DMA_ACH0:
			bulkout_id = BULKOUTID2;
			break;
		case MAC_AX_DMA_ACH2:
			bulkout_id = BULKOUTID4;
			break;
		case MAC_AX_DMA_ACH4:
			bulkout_id = BULKOUTID3;
			break;
		case MAC_AX_DMA_ACH6:
			bulkout_id = BULKOUTID5;
			break;
		case MAC_AX_DMA_B0MG:
		case MAC_AX_DMA_B0HI:
		case MAC_AX_DMA_B1MG:
		case MAC_AX_DMA_B1HI:
			bulkout_id = BULKOUTID0;
			break;
		case MAC_AX_DMA_H2C:
			bulkout_id = BULKOUTID2;
			break;
		default:
			bulkout_id = USBEPMAPERR;
		}
	} else {
		bulkout_id = USBEPMAPERR;
	}
	return bulkout_id;
}

u32 usb_pre_init_8852c(struct mac_ax_adapter *adapter, void *param)
{
	u32 val32 = 0;

	val32 = PLTFM_REG_R32(R_AX_USB_HOST_REQUEST_2_V1) | B_AX_R_USBIO_MODE_V1;
	PLTFM_REG_W32(R_AX_USB_HOST_REQUEST_2_V1, val32);
	// fix USB IO hang suggest by chihhanli@realtek.com
	val32 = PLTFM_REG_R32(R_AX_USB_WLAN0_1_V1) & ~B_AX_USBRX_RST_V1 & ~B_AX_USBTX_RST_V1;
	PLTFM_REG_W32(R_AX_USB_WLAN0_1_V1, val32);

	val32 = PLTFM_REG_R32(R_AX_HCI_FUNC_EN_V1);
	val32 &= ~B_AX_HCI_RXDMA_EN;
	val32 &= ~B_AX_HCI_TXDMA_EN;
	PLTFM_REG_W32(R_AX_HCI_FUNC_EN_V1, val32);
	val32 |= B_AX_HCI_RXDMA_EN;
	val32 |= B_AX_HCI_TXDMA_EN;
	PLTFM_REG_W32(R_AX_HCI_FUNC_EN_V1, val32);
	// fix USB TRX hang suggest by chihhanli@realtek.com

	val32 = PLTFM_REG_R32(R_AX_USB_ENDPOINT_3_V1);
	if ((val32 & B_AX_BULKOUT0_V1) == B_AX_BULKOUT0_V1)
		adapter->usb_info.ep5 = ENABLE;
	if ((val32 & B_AX_BULKOUT1_V1) == B_AX_BULKOUT1_V1)
		adapter->usb_info.ep6 = ENABLE;
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3_V1) >> B_AX_AC_BULKOUT_V1_SH) &
		B_AX_AC_BULKOUT_V1_MSK) == 1)
		adapter->usb_info.ep10 = ENABLE;
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3_V1) >> B_AX_AC_BULKOUT_V1_SH) &
		B_AX_AC_BULKOUT_V1_MSK) == 2) {
		adapter->usb_info.ep10 = ENABLE;
		adapter->usb_info.ep11 = ENABLE;
	}
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3_V1) >> B_AX_AC_BULKOUT_V1_SH) &
		B_AX_AC_BULKOUT_V1_MSK) == 3) {
		adapter->usb_info.ep10 = ENABLE;
		adapter->usb_info.ep11 = ENABLE;
		adapter->usb_info.ep12 = ENABLE;
	}
	return MACSUCCESS;
}

u32 usb_init_8852c(struct mac_ax_adapter *adapter, void *param)
{
	u32 val32;
	u8 val8;

	adapter->usb_info.max_bulkout_wd_num = (u8)GET_FIELD
		(PLTFM_REG_R32(R_AX_CH_PAGE_CTRL_V1), B_AX_PREC_PAGE_CH011);
	adapter->usb_info.max_dma_txagg_msk = AX_TXD_DMA_TXAGG_NUM_V1_MSK;

	val32 = PLTFM_REG_R32(R_AX_USB3_MAC_NPI_CONFIG_INTF_0_V1);
	val32 &= ~B_AX_SSPHY_LFPS_FILTER_V1;
	PLTFM_REG_W32(R_AX_USB3_MAC_NPI_CONFIG_INTF_0_V1, val32);

	val32 = get_usb_mode(adapter);
	if (val32 == MAC_AX_USB3)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB3_BULKSIZE);
	else if (val32 == MAC_AX_USB2)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB2_BULKSIZE);
	else if (val32 == MAC_AX_USB11)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB11_BULKSIZE);
	else
		return MACHWNOSUP;

	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP5, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP6, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP7, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP9, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP10, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP11, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0_V1);
	val8 = SET_CLR_WORD(val8, EP12, B_AX_EP_IDX_V1);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0_V1, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2_V1 + 1, NUMP);
	return MACSUCCESS;
}

u32 usb_deinit_8852c(struct mac_ax_adapter *adapter, void *param)
{
	return MACSUCCESS;
}

u32 read_usb2phy_para_8852c(struct mac_ax_adapter *adapter, u16 offset)
{
	u32 value32 = 0;
	u8 rdata = 0;

	value32 = SET_CLR_WORD(value32, offset - phyoffset,
			       B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_REG_EN;
	value32 |= B_AX_USB_REG_STATUS;
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);
	while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
		;
	rdata = GET_FIELD(PLTFM_REG_R32(R_AX_USB_SIE_INTF),
			  B_AX_USB_SIE_INTF_RD);
	//DD-Yingli suggest that shall clear it if read operation is done.
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, 0);
	return rdata;
}

u32 write_usb2phy_para_8852c(struct mac_ax_adapter *adapter, u16 offset, u8 val)
{
	u32 value32 = 0;

	value32 = SET_CLR_WORD(value32, val, B_AX_USB_SIE_INTF_WD);
	value32 = SET_CLR_WORD(value32, offset, B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_WRITE_EN;
	value32 |= B_AX_USB_REG_EN;
	value32 |= B_AX_USB_REG_STATUS;
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);
	while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
		;
	//DD-Yingli suggest that shall clear it if write operation is done.
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, 0);
	return MACSUCCESS;
}

u32 static polling_usb_sie_ready(struct mac_ax_adapter *adapter)
{
#define MAC_AX_POLL_SIE_CNT 1000
#define MAC_AX_POLL_SIE_WAIT_US 50
	u32 cnt = MAC_AX_POLL_SIE_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	while (cnt--) {
		if (!(MAC_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN))
			return MACSUCCESS;
		PLTFM_DELAY_US(MAC_AX_POLL_SIE_WAIT_US);
	}

	PLTFM_MSG_ERR("%s: polling SIE timeout\n", __func__);

	return MACPOLLTO;
}

u32 read_usb3phy_para_8852c(struct mac_ax_adapter *adapter, u16 offset, u8 b_sel)
{
	u32 value32 = 0, ret;
	u16 rdata = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	value32 = SET_CLR_WORD(value32, offset + USBPHYOFFSET,
			       B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_REG_EN;
	if (b_sel)
		value32 |= B_AX_USB_PHY_BYTE_SEL;

	MAC_REG_W32(R_AX_USB_SIE_INTF, value32);
	ret = polling_usb_sie_ready(adapter);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("%s: polling timeout\n");
	rdata = GET_FIELD(PLTFM_REG_R32(R_AX_USB_SIE_INTF),
			  B_AX_USB_SIE_INTF_RD);

	return rdata;
}

u32 write_usb3phy_para_8852c(struct mac_ax_adapter *adapter,
			     u16 offset, u8 b_sel, u8 val)
{
	u32 value32 = 0, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	value32 = SET_CLR_WORD(value32, val, B_AX_USB_SIE_INTF_WD);
	value32 = SET_CLR_WORD(value32, offset + USBPHYOFFSET,
			       B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_WRITE_EN;
	value32 |= B_AX_USB_REG_EN;
	if (b_sel)
		value32 |= B_AX_USB_PHY_BYTE_SEL;
	MAC_REG_W32(R_AX_USB_SIE_INTF, value32);

	ret = polling_usb_sie_ready(adapter);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("%s: polling timeout\n");

	return ret;
}

u32 u2u3_switch_8852c(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;

	PLTFM_REG_W8(R_AX_PAD_CTRL2 + 1, USB_SWITCH_DELAY);
	ret = get_usb_mode(adapter);
	if (ret == MAC_AX_USB2)
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, U2SWITCHU3);
	else if (ret == MAC_AX_USB3)
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, U3SWITCHU2);
	else
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, U2SWITCHU3);

	return MACSUCCESS;
}

u32 get_usb_support_ability_8852c(struct mac_ax_adapter *adapter)
{
	u32 u2force = 0;
	u32 u3force = 0;

	u2force = PLTFM_REG_R32(R_AX_USB_HOST_REQUEST_2_V1) &
		  B_AX_R_FORCE_U3MAC_HS_MODE_V1;
	u3force = PLTFM_REG_R32(R_AX_PAD_CTRL2) &
		  B_AX_USB3_USB2_TRANSITION;

	if (u2force == B_AX_R_FORCE_U3MAC_HS_MODE_V1)
		return FORCEUSB2MODE;
	else if (u3force == B_AX_USB3_USB2_TRANSITION)
		return SWITCHMODE;
	else
		return FORCEUSB3MODE;
}

u32 usb_tx_agg_cfg_8852c(struct mac_ax_adapter *adapter,
			 struct mac_ax_usb_tx_agg_cfg *agg)
{
	u32 dw1 = ((struct wd_body_usb_8852c *)agg->pkt)->dword1;

	((struct wd_body_usb_8852c *)agg->pkt)->dword1 =
		SET_CLR_WORD(dw1, agg->agg_num, AX_TXD_DMA_TXAGG_NUM_V1);
	return MACSUCCESS;
}

u32 usb_rx_agg_cfg_8852c(struct mac_ax_adapter *adapter,
			 struct mac_ax_rx_agg_cfg *cfg)
{
	u8 size;
	u8 timeout;
	u8 agg_en;
	u8 agg_mode;
	u8 pkt_num;
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	/* size unit was 4k in 8852A|B|51B */
#define COMPAT_RX_AGG_UNIT 4
	if (cfg->mode == MAC_AX_RX_AGG_MODE_DMA) {
		agg_en = ENABLE;
		agg_mode = ENABLE;
	} else if (cfg->mode == MAC_AX_RX_AGG_MODE_USB) {
		agg_en = ENABLE;
		agg_mode = DISABLE;
	} else {
		agg_en = DISABLE;
		agg_mode = DISABLE;
	}

	if (cfg->thold.drv_define == 0) {
		/* unit: 1k */
		size = RXAGGSIZE * COMPAT_RX_AGG_UNIT;
		timeout = RXAGGTO;
		pkt_num = 0;
	} else {
		size = cfg->thold.size;
		timeout = cfg->thold.timeout;
		pkt_num = cfg->thold.pkt_num;
	}

	val32 = MAC_REG_R32(R_AX_RXAGG_0_V1);
	MAC_REG_W32(R_AX_RXAGG_0_V1, (agg_en ? B_AX_RXAGG_EN : 0) |
		    (agg_mode ? B_AX_RXAGG_DMA_STORE : 0) |
		    (val32 & B_AX_RXAGG_SW_EN) |
		    SET_WORD(pkt_num == 0 ? B_AX_RXAGG_PKTNUM_TH_MSK : pkt_num,
			     B_AX_RXAGG_PKTNUM_TH) |
		    SET_WORD(timeout, B_AX_RXAGG_TIMEOUT_TH) |
		    SET_WORD(size, B_AX_RXAGG_LEN_TH));

	/* hw function not used, try to disable it */
	val32 = MAC_REG_R32(R_AX_RXAGG_1_V1);
	MAC_REG_W32(R_AX_RXAGG_1_V1, SET_WORD(0, B_AX_RXAGG_LEN_TH_HIGH_V1) |
		    SET_WORD(0, B_AX_RXAGG_SML_PKT_SIZE_V1) |
		    SET_WORD(B_AX_RXAGG_SML_PKTNUM_TH_MSK, B_AX_RXAGG_SML_PKTNUM_TH));
#undef COMPAT_RX_AGG_UNIT

	return MACSUCCESS;
}

u32 usb_pwr_switch_8852c(void *vadapter, u8 pre_switch, u8 on)
{
	return MACSUCCESS;
}

u32 set_usb_wowlan_8852c(struct mac_ax_adapter *adapter,
			 enum mac_ax_wow_ctrl w_c)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (w_c == MAC_AX_WOW_ENTER) {
		PLTFM_REG_W32(R_AX_USB2_LPM_0_V1, PLTFM_REG_R32(R_AX_USB2_LPM_0_V1) |
			      B_AX_USB_SUS_WAKEUP_EN_V1);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_WLOCK_1C_BIT6);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_R_DIS_PRST);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_WLOCK_1C_BIT6);
	} else if (w_c == MAC_AX_WOW_LEAVE) {
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_WLOCK_1C_BIT6);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_R_DIS_PRST);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_WLOCK_1C_BIT6);
	} else {
		PLTFM_MSG_ERR("[ERR] Invalid WoWLAN input.\n");
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

u32 usb_get_txagg_num_8852c(struct mac_ax_adapter *adapter, u8 band)
{
	u32 quotanum = band ? adapter->dle_info.c1_tx_min : adapter->dle_info.c0_tx_min;

	return quotanum * PLE_PAGE_SIZE / (PINGPONG * (SINGLE_MSDU_SIZE + SEC_FCS_SIZE));
}

u32 usb_get_rx_state_8852c(struct mac_ax_adapter *adapter, u32 *val)
{
	u8 rxdma_cnt, ep_cnt, curr_rxdma_cnt, curr_ep_cnt;
	*val = 0;
	rxdma_cnt = GET_FIELD(PLTFM_REG_R32(R_AX_USB_DEBUG_1_V1), B_AX_RXDMA_DMA_COUNTER_V1);
	ep_cnt = GET_FIELD(PLTFM_REG_R32(R_AX_USB_DEBUG_1_V1), B_AX_RXDMA_ENDPOINT_COUNTER_V1);
	PLTFM_DELAY_MS(RX_POLLING_PERIOD);
	curr_rxdma_cnt = GET_FIELD(PLTFM_REG_R32(R_AX_USB_DEBUG_1_V1), B_AX_RXDMA_DMA_COUNTER_V1);
	curr_ep_cnt = GET_FIELD(PLTFM_REG_R32(R_AX_USB_DEBUG_1_V1), B_AX_RXDMA_ENDPOINT_COUNTER_V1);
	if (curr_rxdma_cnt == rxdma_cnt)
		return MACRXDMAHANG;
	else if (curr_ep_cnt == ep_cnt)
		return MACUSBRXHANG;
	else
		return MACSUCCESS;
}

u32 usb_ep_cfg_8852c(struct mac_ax_adapter *adapter, struct mac_ax_usb_ep *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ep_cfg, cnt, ep_sts;

	if (!cfg)
		return MACNPTR;
	ep_cfg = MAC_REG_R32(R_AX_USB_ENDPOINT_3_V1);
	ep_sts = ep_cfg;
	if (cfg->ep4) {
		ep_cfg |= B_AX_EP4_RX_PAUSE_V1;
		ep_sts |= (B_AX_EP4_RX_PAUSE_V1 | B_AX_EP4_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP4_RX_PAUSE_V1;
		ep_sts &= (~B_AX_EP4_RX_PAUSE_V1 & ~B_AX_EP4_PAUSE_STATE_V1);
	}
	if (cfg->ep5) {
		ep_cfg |= B_AX_EP5_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP5_TX_PAUSE_V1 | B_AX_EP5_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP5_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP5_TX_PAUSE_V1 & ~B_AX_EP5_PAUSE_STATE_V1);
	}
	if (cfg->ep6) {
		ep_cfg |= B_AX_EP6_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP6_TX_PAUSE_V1 | B_AX_EP6_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP6_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP6_TX_PAUSE_V1 & ~B_AX_EP6_PAUSE_STATE_V1);
	}
	if (cfg->ep7) {
		ep_cfg |= B_AX_EP7_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP7_TX_PAUSE_V1 | B_AX_EP7_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP7_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP7_TX_PAUSE_V1 & ~B_AX_EP7_PAUSE_STATE_V1);
	}
	if (cfg->ep8) {
		ep_cfg |= B_AX_EP8_RX_PAUSE_V1;
		ep_sts |= (B_AX_EP8_RX_PAUSE_V1 | B_AX_EP8_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP8_RX_PAUSE_V1;
		ep_sts &= (~B_AX_EP8_RX_PAUSE_V1 & ~B_AX_EP8_PAUSE_STATE_V1);
	}
	if (cfg->ep9) {
		ep_cfg |= B_AX_EP9_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP9_TX_PAUSE_V1 | B_AX_EP9_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP9_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP9_TX_PAUSE_V1 & ~B_AX_EP9_PAUSE_STATE_V1);
	}
	if (cfg->ep10) {
		ep_cfg |= B_AX_EP10_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP10_TX_PAUSE_V1 | B_AX_EP10_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP10_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP10_TX_PAUSE_V1 & ~B_AX_EP10_PAUSE_STATE_V1);
	}
	if (cfg->ep11) {
		ep_cfg |= B_AX_EP11_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP11_TX_PAUSE_V1 | B_AX_EP11_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP11_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP11_TX_PAUSE_V1 & ~B_AX_EP11_PAUSE_STATE_V1);
	}
	if (cfg->ep12) {
		ep_cfg |= B_AX_EP12_TX_PAUSE_V1;
		ep_sts |= (B_AX_EP12_TX_PAUSE_V1 | B_AX_EP12_PAUSE_STATE_V1);
	} else {
		ep_cfg &= ~B_AX_EP12_TX_PAUSE_V1;
		ep_sts &= (~B_AX_EP12_TX_PAUSE_V1 & ~B_AX_EP12_PAUSE_STATE_V1);
	}
	MAC_REG_W32(R_AX_USB_ENDPOINT_3_V1, ep_cfg);

	cnt = 2000;
	while (cnt--) {
		if (ep_sts == PLTFM_REG_R32(R_AX_USB_ENDPOINT_3_V1))
			break;
		PLTFM_DELAY_US(1);
	}
	if (cnt == 0)
		return MACUSBPAUSEERR;

	return MACSUCCESS;
}
#endif /* #if MAC_AX_USB_SUPPORT */
#endif /* #if MAC_AX_8852C_SUPPORT */
