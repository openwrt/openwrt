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
#include "_sdio_8852c.h"
#include "../mac_priv.h"

#if MAC_AX_8852C_SUPPORT
#if MAC_AX_SDIO_SUPPORT
u32 get_sdio_rx_req_len_8852c(struct mac_ax_adapter *adapter, u32 *rx_req_len)
{
	u8 pwr_state;
	u32 val32;

	pwr_state = pwr_state_chk_sdio(adapter);

	if (pwr_state == SDIO_PWR_OFF && adapter->mac_pwr_info.pwr_in_lps != 1) {
		*rx_req_len = 0;
		PLTFM_MSG_ERR("[ERR]get_sdio_rx_req_len when power off\n");
		return MACPWRSTAT;
	}

	val32 = PLTFM_SDIO_CMD53_R32(R_AX_SDIO_RX_REQ_LEN_V1);
	*rx_req_len = GET_FIELD(val32, B_AX_RX_REQ_LEN);

	return MACSUCCESS;
}

u32 sdio_pre_init_8852c(struct mac_ax_adapter *adapter, void *param)
{
	u16 val16;
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = MAC_REG_R32(R_AX_HCI_OPT_CTRL);
	MAC_REG_W32(R_AX_HCI_OPT_CTRL, val32 | B_AX_SDIO_DATA_PAD_SMT);

	val32 = MAC_REG_R32(R_AX_SDIO_TX_CTRL_V1) & ~(B_AX_CMD53_TX_FORMAT);
	MAC_REG_W32(R_AX_SDIO_TX_CTRL_V1, val32 | B_AX_RXINT_READ_MASK_DIS);
	adapter->sdio_info.tx_mode = MAC_AX_SDIO_TX_MODE_AGG;
	adapter->sdio_info.tx_seq = 1;

	val16 = MAC_REG_R16(R_AX_SDIO_BUS_CTRL_V1);
	MAC_REG_W16(R_AX_SDIO_BUS_CTRL_V1, val16 | B_AX_EN_RPT_TXCRC);

	return MACSUCCESS;
}

u32 tx_mode_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			   enum mac_ax_sdio_tx_mode mode)
{
	u16 val16;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val16 = MAC_REG_R16(R_AX_SDIO_TX_CTRL_V1);

	if (mode == MAC_AX_SDIO_TX_MODE_AGG) {
		MAC_REG_W16(R_AX_SDIO_TX_CTRL_V1, val16 & ~(B_AX_CMD53_TX_FORMAT));
	} else if (mode == MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK ||
		   mode == MAC_AX_SDIO_TX_MODE_DUMMY_AUTO) {
		if ((val16 & B_AX_CMD53_W_MIX) == 0) {
			MAC_REG_W16(R_AX_SDIO_TX_CTRL_V1,
				    val16 | B_AX_CMD53_TX_FORMAT);
		} else {
			if ((val16 & B_AX_CMD53_TX_FORMAT) == 0)
				return MACSDIOMIXMODE;
			else
				return MACSDIOSEQERR;
		}
	} else {
		PLTFM_MSG_ERR("[ERR]sdio tx mode = %d\n", mode);
		return MACNOITEM;
	}

	adapter->sdio_info.tx_mode = mode;

	return MACSUCCESS;
}

u32 get_int_latency_sdio_8852c(struct mac_ax_adapter *adapter)
{
	u32 free_cnt, free_cnt2;
	u32 int_start;
	u32 int_latency = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	int_start = MAC_REG_R32(R_AX_SDIO_MONITOR_V1);
	free_cnt = MAC_REG_R32(R_AX_FREERUN_CNT_LOW);
	free_cnt2 = MAC_REG_R32(R_AX_FREERUN_CNT_LOW);
	int_latency = free_cnt - int_start - (free_cnt2 - free_cnt);

	return int_latency;
}

u32 get_clk_cnt_sdio_8852c(struct mac_ax_adapter *adapter, u32 *cnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_SDIO_MONITOR_2_V1);
	if (GET_FIELD(val32, B_AX_SDIO_CLK_MONITOR) == 0)
		*cnt = GET_FIELD(val32, B_AX_SDIO_CLK_CNT);
	else
		return MACNOITEM;

	return MACSUCCESS;
}

u32 set_wt_cfg_sdio_8852c(struct mac_ax_adapter *adapter, u8 en)
{
	u32 reg = R_AX_SDIO_MONITOR_2_V1 + 2;

	if (en)
		PLTFM_SDIO_CMD52_W8(reg, BIT(7));
	else
		PLTFM_SDIO_CMD52_W8(reg, 0);

	return MACSUCCESS;
}

u32 set_clk_mon_sdio_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_sdio_clk_mon_cfg *cfg)
{
	u32 reg = R_AX_SDIO_MONITOR_2_V1 + 2;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	p_ops->set_wt_cfg_sdio(adapter, 0);

	switch (cfg->mon) {
	case MAC_AX_SDIO_CLK_MON_SHORT:
		PLTFM_SDIO_CMD52_W8(reg, BIT(5));
		break;
	case MAC_AX_SDIO_CLK_MON_LONG:
		PLTFM_SDIO_CMD52_W8(reg, BIT(6));
		break;
	case MAC_AX_SDIO_CLK_MON_USER_DEFINE:
		MAC_REG_W32(R_AX_SDIO_MONITOR_2_V1,
			    cfg->cycle & B_AX_SDIO_CLK_CNT_MSK);
		PLTFM_SDIO_CMD52_W8(reg, BIT(5) | BIT(6));
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u32 leave_suspend_sdio_8852c(struct mac_ax_adapter *adapter)
{
	u8 val8;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val8 = MAC_REG_R8(R_AX_SDIO_BUS_CTRL_V1 + 2);
	MAC_REG_W8(R_AX_SDIO_BUS_CTRL_V1 + 2, val8 & ~(BIT(0)));

	cnt = 10000;
	while (!(MAC_REG_R8(R_AX_SDIO_BUS_CTRL_V1 + 2) & BIT(1))) {
		cnt--;
		if (cnt == 0)
			return MACLSUS;
	}

	return MACSUCCESS;
}

u32 sdio_pwr_switch_8852c(void *vadapter, u8 pre_switch, u8 on)
{
	struct mac_ax_adapter *adapter = (struct mac_ax_adapter *)vadapter;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (pre_switch == PWR_PRE_SWITCH) {
		adapter->sdio_info.rpwm_bak = MAC_REG_R32(R_AX_SDIO_HIMR_V1);
		MAC_REG_W32(R_AX_SDIO_HIMR_V1, 0);
		adapter->mac_pwr_info.pwr_seq_proc = 1;
	} else if (pre_switch == PWR_POST_SWITCH) {
		if (on)
			adapter->sdio_info.tx_seq = 1;
		adapter->mac_pwr_info.pwr_seq_proc = 0;
	} else if (pre_switch == PWR_END_SWITCH) {
		MAC_REG_W32(R_AX_SDIO_HIMR_V1, adapter->sdio_info.rpwm_bak);
	}

	return MACSUCCESS;
}

u8 r_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	return p_ops->_r_indir_cmd52_sdio(adapter, adr);
}

u8 _r_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 tmp;
	u32 cnt;

	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1, (u8)adr);
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 1, (u8)(adr >> 8));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 2, (u8)(adr >> 16));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 3,
			    (u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_CTRL_V1, (u8)B_AX_INDIRECT_REG_R);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_ADDR_V1 + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD52 read\n");

	return PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA_V1);
}

u32 _r_indir_cmd53_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 val[12] = {0};
	u32 cnt, i;
	union {
		__le32 dword;
		u8 byte[4];
	} value32 = { 0x00000000 };

	for (i = 0; i < 3; i++)
		*(val + i) = (u8)(adr >> (i << 3));
	*(val + 3) = (u8)((adr | B_AX_INDIRECT_RDY) >> 24);
	*(val + 8) = (u8)(B_AX_INDIRECT_REG_R);
	PLTFM_SDIO_CMD53_WN(R_AX_SDIO_INDIRECT_ADDR_V1, sizeof(val), val);

	cnt = SDIO_WAIT_CNT;
	do {
		PLTFM_SDIO_CMD53_RN(R_AX_SDIO_INDIRECT_ADDR_V1 + 3, 8, val);
		cnt--;
	} while (((val[0] & BIT(7)) == 0) && (cnt > 0));

	if (((val[0] & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD53 read\n");

	value32.byte[0] = val[1];
	value32.byte[1] = val[2];
	value32.byte[2] = val[3];
	value32.byte[3] = val[4];

	return le32_to_cpu(value32.dword);
}

u32 r16_indir_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 pwr_state;
	union {
		__le32 dword;
		u8 byte[4];
	} val32 = { 0x00000000 };
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	pwr_state = pwr_state_chk_sdio(adapter);

	if (pwr_state == SDIO_PWR_OFF) {
		val32.byte[0] = p_ops->r_indir_cmd52_sdio(adapter, adr);
		val32.byte[1] =
			PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA_V1 + 1);
		return  le32_to_cpu(val32.dword);
	}

	return r_indir_cmd53_sdio(adapter, adr);
}

u32 r32_indir_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr)
{
	u8 pwr_state;
	union {
		__le32 dword;
		u8 byte[4];
	} val32 = { 0x00000000 };
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	pwr_state = pwr_state_chk_sdio(adapter);

	if (pwr_state == SDIO_PWR_OFF) {
		val32.byte[0] = p_ops->r_indir_cmd52_sdio(adapter, adr);
		val32.byte[1] =
			PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA_V1 + 1);
		val32.byte[2] =
			PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA_V1 + 2);
		val32.byte[3] =
			PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA_V1 + 3);
		return le32_to_cpu(val32.dword);
	}

	return r_indir_cmd53_sdio(adapter, adr);
}

void w_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			      enum sdio_io_size size)
{
	u8 tmp;
	u32 cnt;

	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1, (u8)adr);
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 1, (u8)(adr >> 8));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 2, (u8)(adr >> 16));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR_V1 + 3,
			    (u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	switch (size) {
	case SDIO_IO_BYTE:
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1, (u8)val);
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_CTRL_V1,
				    (u8)(B_AX_INDIRECT_REG_W));
		break;
	case SDIO_IO_WORD:
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1, (u8)val);
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1 + 1,
				    (u8)(val >> 8));
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_CTRL_V1,
				    (u8)(B_AX_INDIRECT_REG_W | 0x1));
		break;
	case SDIO_IO_DWORD:
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1, (u8)val);
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1 + 1,
				    (u8)(val >> 8));
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1 + 2,
				    (u8)(val >> 16));
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_DATA_V1 + 3,
				    (u8)(val >> 24));
		PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_CTRL_V1,
				    (u8)(B_AX_INDIRECT_REG_W | 0x2));
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_ADDR_V1 + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD52 write\n");
}

void w_indir_cmd53_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			      enum sdio_io_size size)
{
	u8 value[12] = {0};
	u8 tmp;
	u32 cnt, i;

	switch (size) {
	case SDIO_IO_BYTE:
		*(value + 8) = (u8)B_AX_INDIRECT_REG_W;
		break;
	case SDIO_IO_WORD:
		*(value + 8) = (u8)(B_AX_INDIRECT_REG_W | 0x1);
		break;
	case SDIO_IO_DWORD:
		*(value + 8) = (u8)(B_AX_INDIRECT_REG_W | 0x2);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid IO size\n");
		break;
	}

	for (i = 0; i < 3; i++)
		*(value + i) = (u8)(adr >> (i << 3));
	*(value + 3) = (u8)((adr | B_AX_INDIRECT_RDY) >> 24);
	for (i = 0; i < 4; i++)
		*(value + i + 4) = (u8)(val >> (i << 3));
	PLTFM_SDIO_CMD53_WN(R_AX_SDIO_INDIRECT_ADDR_V1, sizeof(value), value);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_ADDR_V1 + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD53 read\n");
}

void ud_fs_8852c(struct mac_ax_adapter *adapter)
{
	u8 val[28] = {0};
	u32 fs0, fs1, fs2, fs3, fs4, fs5, fs6;
	struct mac_ax_hfc_ch_info *ch_info = adapter->hfc_param->ch_info;
	struct mac_ax_hfc_pub_info *pub_info = adapter->hfc_param->pub_info;

	PLTFM_SDIO_CMD53_RN(R_AX_SDIO_TXPG_WP_V1, sizeof(val), val);

	fs0 = le32_to_cpu(*(u32 *)(val));
	fs1 = le32_to_cpu(*(u32 *)(val + 4));
	fs2 = le32_to_cpu(*(u32 *)(val + 8));
	fs3 = le32_to_cpu(*(u32 *)(val + 12));
	fs4 = le32_to_cpu(*(u32 *)(val + 16));
	fs5 = le32_to_cpu(*(u32 *)(val + 20));
	fs6 = le32_to_cpu(*(u32 *)(val + 24));

	ch_info[MAC_AX_DMA_H2C].aval = GET_FIELD(fs0, B_AX_SDIO_ACH12_AVAL_PG);
	pub_info->wp_aval = GET_FIELD(fs0, B_AX_SDIO_WP_AVAL_PG);
	ch_info[MAC_AX_DMA_ACH0].used = GET_FIELD(fs1, B_AX_SDIO_ACH0_USE_PG);
	ch_info[MAC_AX_DMA_ACH1].used = GET_FIELD(fs1, B_AX_SDIO_ACH1_USE_PG);
	ch_info[MAC_AX_DMA_ACH2].used = GET_FIELD(fs2, B_AX_SDIO_ACH2_USE_PG);
	ch_info[MAC_AX_DMA_ACH3].used = GET_FIELD(fs2, B_AX_SDIO_ACH3_USE_PG);
	ch_info[MAC_AX_DMA_ACH4].used = GET_FIELD(fs3, B_AX_SDIO_ACH4_USE_PG);
	ch_info[MAC_AX_DMA_ACH5].used = GET_FIELD(fs3, B_AX_SDIO_ACH5_USE_PG);
	ch_info[MAC_AX_DMA_ACH6].used = GET_FIELD(fs4, B_AX_SDIO_ACH6_USE_PG);
	ch_info[MAC_AX_DMA_ACH7].used = GET_FIELD(fs4, B_AX_SDIO_ACH7_USE_PG);
	ch_info[MAC_AX_DMA_B0MG].used = GET_FIELD(fs5, B_AX_SDIO_ACH8_USE_PG);
	ch_info[MAC_AX_DMA_B0HI].used = GET_FIELD(fs5, B_AX_SDIO_ACH9_USE_PG);
	ch_info[MAC_AX_DMA_B1MG].used = GET_FIELD(fs6, B_AX_SDIO_ACH10_USE_PG);
	ch_info[MAC_AX_DMA_B1HI].used = GET_FIELD(fs6, B_AX_SDIO_ACH11_USE_PG);
}

void rx_agg_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rx_agg_cfg *cfg)
{
	u8 size;
	u8 timeout;
	u8 agg_en;
	u8 pkt_num;
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (cfg->mode == MAC_AX_RX_AGG_MODE_DMA)
		agg_en = 1;
	else
		agg_en = 0;

	if (cfg->thold.drv_define == 0) {
		size = 0xFF;
		timeout = 0x01;
		pkt_num = 0;
	} else {
		size = cfg->thold.size;
		timeout = cfg->thold.timeout;
		pkt_num = cfg->thold.pkt_num;
	}

	val32 = MAC_REG_R32(R_AX_RXAGG_0_V1);
	MAC_REG_W32(R_AX_RXAGG_0_V1, (agg_en ? B_AX_RXAGG_EN : 0) |
		    B_AX_RXAGG_DMA_STORE | (val32 & B_AX_RXAGG_SW_EN) |
		    SET_WORD(pkt_num, B_AX_RXAGG_PKTNUM_TH) |
		    SET_WORD(timeout, B_AX_RXAGG_TIMEOUT_TH) |
		    SET_WORD(size, B_AX_RXAGG_LEN_TH));
}

void aval_page_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			      struct mac_ax_aval_page_cfg *cfg)
{
	u32 val32;
	struct mac_sdio_ch_thr ch_thr;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (cfg->ch_dma) {
	case 0:
		ch_thr.thr = R_AX_ACH0_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH0_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH0_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH0_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH0_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH0_THR_WD_MSK;
		break;
	case 1:
		ch_thr.thr = R_AX_ACH1_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH1_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH1_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH1_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH1_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH1_THR_WD_MSK;
		break;
	case 2:
		ch_thr.thr = R_AX_ACH2_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH2_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH2_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH2_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH2_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH2_THR_WD_MSK;
		break;
	case 3:
		ch_thr.thr = R_AX_ACH3_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH3_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH3_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH3_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH3_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH3_THR_WD_MSK;
		break;
	case 4:
		ch_thr.thr = R_AX_ACH4_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH4_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH4_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH4_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH4_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH4_THR_WD_MSK;
		break;
	case 5:
		ch_thr.thr = R_AX_ACH5_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH5_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH5_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH5_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH5_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH5_THR_WD_MSK;
		break;
	case 6:
		ch_thr.thr = R_AX_ACH6_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH6_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH6_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH6_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH6_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH6_THR_WD_MSK;
		break;
	case 7:
		ch_thr.thr = R_AX_ACH7_THR_V1;
		ch_thr.intrpt_en = B_AX_ACH7_INTRPT_EN;
		ch_thr.wp_sh = B_AX_ACH7_THR_WP_SH;
		ch_thr.wp_msk = B_AX_ACH7_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_ACH7_THR_WD_SH;
		ch_thr.wd_msk = B_AX_ACH7_THR_WD_MSK;
		break;
	case 8:
		ch_thr.thr = R_AX_CH8_THR_V1;
		ch_thr.intrpt_en = B_AX_CH8_INTRPT_EN;
		ch_thr.wp_sh = B_AX_CH8_THR_WP_SH;
		ch_thr.wp_msk = B_AX_CH8_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_CH8_THR_WD_SH;
		ch_thr.wd_msk = B_AX_CH8_THR_WD_MSK;
		break;
	case 9:
		ch_thr.thr = R_AX_CH9_THR_V1;
		ch_thr.intrpt_en = B_AX_CH9_INTRPT_EN;
		ch_thr.wp_sh = B_AX_CH9_THR_WP_SH;
		ch_thr.wp_msk = B_AX_CH9_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_CH9_THR_WD_SH;
		ch_thr.wd_msk = B_AX_CH9_THR_WD_MSK;
		break;
	case 10:
		ch_thr.thr = R_AX_CH10_THR_V1;
		ch_thr.intrpt_en = B_AX_CH10_INTRPT_EN;
		ch_thr.wp_sh = B_AX_CH10_THR_WP_SH;
		ch_thr.wp_msk = B_AX_CH10_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_CH10_THR_WD_SH;
		ch_thr.wd_msk = B_AX_CH10_THR_WD_MSK;
		break;
	case 11:
		ch_thr.thr = R_AX_CH11_THR_V1;
		ch_thr.intrpt_en = B_AX_CH11_INTRPT_EN;
		ch_thr.wp_sh = B_AX_CH11_THR_WP_SH;
		ch_thr.wp_msk = B_AX_CH11_THR_WP_MSK;
		ch_thr.wd_sh = B_AX_CH11_THR_WD_SH;
		ch_thr.wd_msk = B_AX_CH11_THR_WD_MSK;
		break;
	default:
		PLTFM_MSG_ERR("[ERR]invalid channel number\n");
		return;
	}

	val32 = ((cfg->thold_wd & ch_thr.wd_msk) << ch_thr.wd_sh) |
		((cfg->thold_wp & ch_thr.wp_msk) << ch_thr.wp_sh);
	if (cfg->en)
		val32 |= ch_thr.intrpt_en;
	else
		val32 &= ~ch_thr.intrpt_en;
	MAC_REG_W32(ch_thr.thr, val32);
}

#endif /*MAC_AX_SDIO_SUPPORT*/
#endif /* MAC_AX_8852C_SUPPORT */
