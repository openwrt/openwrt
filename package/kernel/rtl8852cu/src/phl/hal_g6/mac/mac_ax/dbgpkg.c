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

#include "dbgpkg.h"
#include "dbgport_hw.h"
#include "mac_priv.h"

#if MAC_AX_FEATURE_HV
#include "../hv_ax/phy_rpt_hv.h"
#endif

static u8 *fwcmd_lb_data;

const char *tx_cnt_type_g6[] = {"LCCK", "SCCK", "OFDM", "HT", "HT_GF", "VHTSU", "VHTMU",
				"HESU", "HEERSU", "HEMU", "HETB"};

const char *rx_cnt_type[] = {"OFDM_OK", "OFDM_FAIL", "OFDM_FAM",
			     "CCK_OK", "CCK_FAIL", "CCK_FAM",
			     "HT_OK", "HT_FAIL", "HT_PPDU", "HT_FAM",
			     "VHTSU_OK", "VHTSU_FAIL", "VHTSU_PPDU", "VHTSU_FAM",
			     "VHTMU_OK", "VHTMU_FAIL", "VHTMU_PPDU", "VHTMU_FAM",
			     "HESU_OK", "HESU_FAIL", "HESU_PPDU", "HESU_FAM",
			     "HEMU_OK", "HEMU_FAIL", "HEMU_PPDU", "HEMU_FAM",
			     "HETB_OK", "HETB_FAIL", "HETB_PPDU", "HETB_FAM",
			     "INVD", "RECCA", "FULLDRP", "FULLDRP_PKT", "RXDMA",
			     "PKTFLTR_DRP", "CSIPKT_DMA_OK", "CSIPKT_DMA_DROP",
			     "NDP_PPDU", "CONT_FCS",
			     "USER0", "USER1", "USER2", "USER3",
			     "USER4", "USER5", "USER6", "USER7"};

/* base address mapping table of enum mac_ax_mem_sel */
static u32 base_addr_map_tbl[MAC_AX_MEM_MAX] = {
	CPU_LOCAL_BASE_ADDR,
	WCPU_DATA_BASE_ADDR,
	AXIDMA_BASE_ADDR,
	SHARED_BUF_BASE_ADDR,
	DMAC_TBL_BASE_ADDR,
	SHCUT_MACHDR_BASE_ADDR,
	STA_SCHED_BASE_ADDR,
	RXPLD_FLTR_CAM_BASE_ADDR,
	SEC_CAM_BASE_ADDR,
	WOW_CAM_BASE_ADDR,
	CMAC_TBL_BASE_ADDR,
	ADDR_CAM_BASE_ADDR,
	BA_CAM_BASE_ADDR,
	BCN_IE_CAM0_BASE_ADDR,
	BCN_IE_CAM1_BASE_ADDR,
	TXD_FIFO_0_BASE_ADDR,
	TXD_FIFO_1_BASE_ADDR,
};

u32 mac_fwcmd_lb(struct mac_ax_adapter *adapter, u32 len, u8 burst)
{
	u32 i;
	u32 ret;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	enum h2c_buf_class buf_class;
	u8 *buf;

	if (!burst) {
		if (fwcmd_lb_data) {
			PLTFM_MSG_ERR("C2H loopbakc is not received\n");
			return MACWQBUSY;
		}
	}

	if (len <= H2C_CMD_LEN)
		buf_class = H2CB_CLASS_CMD;
	else if (len <= H2C_DATA_LEN)
		buf_class = H2CB_CLASS_DATA;
	else
		buf_class = H2CB_CLASS_LONG_DATA;

	h2cb = h2cb_alloc(adapter, buf_class);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, len - FWCMD_HDR_LEN);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	for (i = 0; i < len - FWCMD_HDR_LEN; i++)
		buf[i] = (u8)(i & 0xFF);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_TEST,
			      FWCMD_H2C_CL_CMD_PATH,
			      FWCMD_H2C_FUNC_H2C_LB,
			      0,
			      0);
	if (ret != MACSUCCESS)
		goto fail;

	if (!burst) {
		fwcmd_lb_data = (u8 *)PLTFM_MALLOC(len);
#if MAC_AX_PHL_H2C
		PLTFM_MEMCPY(fwcmd_lb_data, h2cb->vir_data, len);
#else
		PLTFM_MEMCPY(fwcmd_lb_data, h2cb->data, len);
#endif
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	PLTFM_FREE(fwcmd_lb_data, len);
	h2cb_free(adapter, h2cb);

	return ret;
}

static u32 c2h_sys_cmd_lb(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			  struct rtw_c2h_info *info)
{
	u32 hdr1;
	u32 c2h_len;
	u32 h2c_len;
	u32 ret = MACSUCCESS;

	PLTFM_MSG_TRACE("receive c2h cmd_lb\n");

	if (!fwcmd_lb_data)
		return MACSUCCESS;

	hdr1 = ((struct fwcmd_hdr *)buf)->hdr1;
	hdr1 = le32_to_cpu(hdr1);
	c2h_len = GET_FIELD(hdr1, C2H_HDR_TOTAL_LEN);

	hdr1 = ((struct fwcmd_hdr *)fwcmd_lb_data)->hdr1;
	hdr1 = le32_to_cpu(hdr1);
	h2c_len = GET_FIELD(hdr1, H2C_HDR_TOTAL_LEN);

	if (c2h_len != h2c_len) {
		PLTFM_MSG_ERR("[ERR]fwcmd lb wrong len\n");
		ret = MACCMP;
		goto END;
	}

	if (PLTFM_MEMCMP(buf + FWCMD_HDR_LEN, fwcmd_lb_data + FWCMD_HDR_LEN,
			 c2h_len - FWCMD_HDR_LEN)) {
		PLTFM_MSG_ERR("[ERR]fwcmd lb contents compare fail\n");
		ret = MACCMP;
		goto END;
	}

END:
	PLTFM_FREE(fwcmd_lb_data, len);
	fwcmd_lb_data = NULL;

	return ret;
}

static struct c2h_proc_func c2h_proc_sys_cmd_path[] = {
	{FWCMD_C2H_FUNC_C2H_LB, c2h_sys_cmd_lb},
#if MAC_AX_FEATURE_HV
	{FWCMD_C2H_FUNC_TEST_PHY_RPT, c2h_test_phy_rpt},
#endif
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_sys_cmd_path(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		     struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_sys_cmd_path;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: 0x%X\n", proc->id);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_sys_plat_autotest(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			  struct rtw_c2h_info *info)
{
	//struct c2h_proc_func *proc = c2h_proc_sys_palt_autotest;
	//u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	//	       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	//PLTFM_MSG_TRACE("c2h_sys_plat_autotest\n");
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
	return MACSUCCESS;
}

u32 iecam_aces_cfg(struct mac_ax_adapter *adapter, u8 band, u8 en,
		   struct iecam_cfg_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 camctrl_reg, rbp_reg, ioctrl_reg, val32;
	u16 ioctrl_bit, val16;
	u32 cnt;

	camctrl_reg = band == MAC_AX_BAND_0 ?
		      R_AX_BCN_IECAM_CTRL : R_AX_BCN_IECAM_CTRL_C1;

	rbp_reg = band == MAC_AX_BAND_0 ?
		  R_AX_BCN_PSR_CTRL : R_AX_BCN_PSR_CTRL_C1;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		ioctrl_reg = band == MAC_AX_BAND_0 ?
			     R_AX_RXGCK_CTRL : R_AX_RXGCK_CTRL_C1;
		ioctrl_bit = B_AX_DISGCLK;
	} else {
		ioctrl_reg = camctrl_reg;
		ioctrl_bit = B_AX_BCN_IECAM_IORST;
	}

	if (en == MAC_AX_FUNC_DIS) {
		/* Restore original setting */
		MAC_REG_W16(camctrl_reg, info->camctrl_bkp);
		MAC_REG_W32(rbp_reg, info->rbp_bkp);
		MAC_REG_W16(ioctrl_reg, info->ioctrl_bkp);
		return MACSUCCESS;
	}
	info->camctrl_bkp = MAC_REG_R16(camctrl_reg);
	info->rbp_bkp = MAC_REG_R32(rbp_reg);
	info->ioctrl_bkp = MAC_REG_R16(ioctrl_reg);

	val16 = MAC_REG_R16(camctrl_reg);
	if (val16 & B_AX_BCN_IECAM_EN)
		MAC_REG_W16(camctrl_reg, val16 & ~B_AX_BCN_IECAM_EN);

	val32 = MAC_REG_R32(rbp_reg);
	if (val32 & B_AX_TIM_PARSER_EN)
		MAC_REG_W32(rbp_reg, val32 & ~B_AX_TIM_PARSER_EN);

	val16 = MAC_REG_R16(ioctrl_reg);
	if (!(val16 & ioctrl_bit))
		MAC_REG_W16(ioctrl_reg, val16 | ioctrl_bit);

	cnt = BCN_PSR_WAIT_CNT;
	while (cnt-- && MAC_REG_R16(camctrl_reg) & B_AX_BCN_PSR_BUSY)
		PLTFM_DELAY_US(BCN_PSR_WAIT_US);
	if (!++cnt) {
		PLTFM_MSG_ERR("[ERR]BCN PSR polling timeout\n");
		return MACPOLLTO;
	}

	return MACSUCCESS;
}

static u32 __dump_mac_mem(struct mac_ax_adapter *adapter,
			  enum mac_ax_mem_sel sel, u32 offset, u8 *buf, u32 len,
			  u32 dbg_path)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct iecam_cfg_info iec_info;
	u32 base_addr, strt_pg, residue, i, cnt, val32;
	u32 ret = MACSUCCESS, ret2;
	u8 iecam_cfg_flag, band;

	if (sel >= MAC_AX_MEM_LAST)
		return MACNOITEM;

	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]%s sec mode ind aces\n", __func__);
		return MACIOERRINSEC;
	}

	if (sel == MAC_AX_MEM_BCN_IE_CAM0 || sel == MAC_AX_MEM_BCN_IE_CAM1) {
		iecam_cfg_flag = 1;
		band = sel == MAC_AX_MEM_BCN_IE_CAM0 ?
		       MAC_AX_BAND_0 : MAC_AX_BAND_1;
	} else {
		iecam_cfg_flag = 0;
		band = 0;
	}

	if (iecam_cfg_flag) {
		ret = iecam_aces_cfg(adapter, band, MAC_AX_FUNC_EN, &iec_info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]iecam access en fail %d\n", ret);
			ret2 = iecam_aces_cfg(adapter, band, MAC_AX_FUNC_DIS,
					      &iec_info);
			if (ret2 != MACSUCCESS)
				PLTFM_MSG_ERR("[ERR]iecam access dis fail %d\n",
					      ret2);
			return ret;
		}
	}

	strt_pg = offset / MAC_MEM_DUMP_PAGE_SIZE;
	residue = offset % MAC_MEM_DUMP_PAGE_SIZE;
	base_addr = base_addr_map_tbl[sel] + strt_pg * MAC_MEM_DUMP_PAGE_SIZE;

	cnt = 0;
	while (cnt < len) {
		//PLTFM_MSG_ERR("%s ind access sel %d start\n", __func__, sel);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, base_addr);
		for (i = R_AX_INDIR_ACCESS_ENTRY + residue;
		     i < R_AX_INDIR_ACCESS_ENTRY + MAC_MEM_DUMP_PAGE_SIZE;
		     i += 4) {
			val32 = le32_to_cpu(MAC_REG_R32(i));
			if (dbg_path)
				*(u32 *)(buf + cnt) = val32;
			else
				PLTFM_MSG_ALWAYS("0x%X: 0x%X\n",
						 (base_addr + i -
						  R_AX_INDIR_ACCESS_ENTRY),
						 val32);
			cnt += 4;
			if (cnt >= len)
				break;
		}
		adapter->hw_info->ind_aces_cnt--;
		PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
		//PLTFM_MSG_ERR("%s ind access sel %d end\n", __func__, sel);

		residue = 0;
		base_addr += MAC_MEM_DUMP_PAGE_SIZE;
	}

	if (iecam_cfg_flag) {
		ret = iecam_aces_cfg(adapter, band, MAC_AX_FUNC_DIS, &iec_info);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[ERR]iecam access dis fail %d\n", ret);
	}

	return ret;
}

u32 mac_mem_dump(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel,
		 u32 strt_addr, u8 *buf, u32 len, u32 dbg_path)
{
	u32 ret, size;

	if (len & (4 - 1)) {
		PLTFM_MSG_ERR("[ERR]not 4byte alignment\n");
		return MACBUFSZ;
	}

	size = mac_get_mem_size(adapter, sel);
	if (len > size) {
		PLTFM_MSG_ERR("[ERR]len %d over max mem size %d\n", len, size);
		return MACBUFSZ;
	}

	/* dbg_path: 1: memory ; 0: console */
	if (dbg_path) {
		if (!buf) {
			PLTFM_MSG_ERR("[ERR]null buf for mem dump\n");
			return MACNPTR;
		}
	}

	ret = __dump_mac_mem(adapter, sel, strt_addr, buf, len, dbg_path);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dump mac memory\n");
		return ret;
	}

	return MACSUCCESS;
}

u32 mac_get_mem_size(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel)
{
	u32 size;
	u16 macid_num = adapter->hw_info->macid_num;

	switch (sel) {
	case MAC_AX_MEM_AXIDMA:
		size = AXIDMA_REG_SIZE;
		break;
	case MAC_AX_MEM_SHARED_BUF:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			size = SHARE_BUFFER_SIZE_8852A;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			size = SHARE_BUFFER_SIZE_8852B;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
			size = SHARE_BUFFER_SIZE_8852C;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
			size = SHARE_BUFFER_SIZE_8192XB;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			size = SHARE_BUFFER_SIZE_8851B;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
			size = SHARE_BUFFER_SIZE_8851E;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			size = SHARE_BUFFER_SIZE_8852D;
		else
			size = 0;
		break;
	case MAC_AX_MEM_DMAC_TBL:
		size = DCTL_INFO_SIZE * macid_num;
		break;
	case MAC_AX_MEM_SHCUT_MACHDR:
		size = MACHDR_SIZE * macid_num;
		break;
	case MAC_AX_MEM_STA_SCHED:
		size = STA_SCHED_MEM_SIZE;
		break;
	case MAC_AX_MEM_RXPLD_FLTR_CAM:
		size = RXPLD_FLTR_CAM_MEM_SIZE;
		break;
	case MAC_AX_MEM_SECURITY_CAM:
		size = SECURITY_CAM_MEM_SIZE;
		break;
	case MAC_AX_MEM_WOW_CAM:
		size = WOW_CAM_MEM_SIZE;
		break;
	case MAC_AX_MEM_CMAC_TBL:
		size = CCTL_INFO_SIZE * macid_num;
		break;
	case MAC_AX_MEM_ADDR_CAM:
		size = ADDR_CAM_MEM_SIZE;
		break;
	case MAC_AX_MEM_BA_CAM:
		size = BA_CAM_SIZE << BA_CAM_NUM_SH;
		break;
	case MAC_AX_MEM_BCN_IE_CAM0:
		size = BCN_IE_CAM_SIZE * BCN_IE_CAM_NUM;
		break;
	case MAC_AX_MEM_BCN_IE_CAM1:
		size = BCN_IE_CAM_SIZE * BCN_IE_CAM_NUM;
		break;
	case MAC_AX_MEM_TXD_FIFO_0:
	case MAC_AX_MEM_TXD_FIFO_1:
		size = TXD_FIFO_SIZE;
		break;
	default:
		size = 0;
		break;
	}

	return size;
}

static u32 __dump_reg_range(struct mac_ax_adapter *adapter,
			    u32 srt, u32 end)
{
#define REG_PAGE_SIZE 0x100
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 i, j, page, val32_1, val32_2, val32_3, val32_4;

	for (i = srt; i <= end; i++) {
		page = i << 8;
		for (j = page; j < page + REG_PAGE_SIZE; j += 16) {
			val32_1 = MAC_REG_R32(j);
			val32_2 = MAC_REG_R32(j + 4);
			val32_3 = MAC_REG_R32(j + 8);
			val32_4 = MAC_REG_R32(j + 12);
			PLTFM_MSG_ALWAYS("%08x        %08x %08x %08x %08x\n",
					 j, val32_1, val32_2, val32_3, val32_4);
		}
	}

	return MACSUCCESS;
#undef REG_PAGE_SIZE
}

static u32 __dump_mac_reg(struct mac_ax_adapter *adapter,
			  enum mac_ax_reg_sel sel)
{
	u32 srt, end;

	switch (sel) {
	case MAC_AX_REG_MAC:
		srt = MAC_PAGE_SRT;
		if (adapter->sm.pwr != MAC_AX_PWR_ON) {
			PLTFM_MSG_WARN("[WARN]MAC is not power on %d\n",
				       adapter->sm.pwr);
			end = MAC_PAGE_AON_END;
		} else {
			end = MAC_PAGE_TOP_END;
		}
		PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
		__dump_reg_range(adapter, srt, end);

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			srt = MAC_PAGE_HCI_SRT;
			end = MAC_PAGE_HCI_END;
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
				srt = MAC_PAGE_PCIE_SRT;
				end = MAC_PAGE_PCIE_END;
			} else if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
				srt = MAC_PAGE_USB_SRT;
				end = MAC_PAGE_USB_END;
			} else if (adapter->hw_info->intf == MAC_AX_INTF_SDIO) {
				srt = MAC_PAGE_SDIO_SRT;
				end = MAC_PAGE_SDIO_END;
			} else {
				PLTFM_MSG_ERR("Undefined intf for hci reg range\n");
				return MACINTF;
			}
			PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
			__dump_reg_range(adapter, srt, end);

			srt = MAC_PAGE_HAXI_SRT;
			end = MAC_PAGE_HAXI_END;
		} else {
			PLTFM_MSG_ERR("Undefined chip id for hci reg range\n");
			return MACCHIPID;
		}
		PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
		__dump_reg_range(adapter, srt, end);

		if (check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_DMAC_SEL)
		    == MACSUCCESS) {
			srt = MAC_PAGE_DMAC_SRT;
			end = MAC_PAGE_DMAC_END;
			PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
			__dump_reg_range(adapter, srt, end);
		}

		if (check_mac_en(adapter, MAC_AX_BAND_0, MAC_AX_CMAC_SEL)
		    == MACSUCCESS) {
			srt = MAC_PAGE_CMAC0_SRT;
			end = MAC_PAGE_CMAC0_END;
			PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
			__dump_reg_range(adapter, srt, end);
		}

		if (check_mac_en(adapter, MAC_AX_BAND_1, MAC_AX_CMAC_SEL)
		    == MACSUCCESS) {
			srt = MAC_PAGE_CMAC1_SRT;
			end = MAC_PAGE_END;
			PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
			__dump_reg_range(adapter, srt, end);
		}

		PLTFM_MSG_ALWAYS("dump MAC REG all\n");
		break;
	case MAC_AX_REG_BB:
		srt = BB_PAGE_SRT;
		end = BB_PAGE_END;
		PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
		__dump_reg_range(adapter, srt, end);
		PLTFM_MSG_ALWAYS("dump BB REG all\n");
		break;
	case MAC_AX_REG_IQK:
		srt = IQK_PAGE_SRT;
		end = IQK_PAGE_END;
		PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
		__dump_reg_range(adapter, srt, end);
		PLTFM_MSG_ALWAYS("dump IQK REG all\n");
		break;
	case MAC_AX_REG_RFC:
		srt = RFC_PAGE_SRT;
		end = RFC_PAGE_END;
		PLTFM_MSG_ALWAYS("dump MAC REG pg 0x%X-0x%X\n", srt, end);
		__dump_reg_range(adapter, srt, end);
		PLTFM_MSG_ALWAYS("dump RFC REG all\n");
		break;
	default:
		PLTFM_MSG_ERR("Page sel error (%d)\n", sel);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_reg_dump(struct mac_ax_adapter *adapter, enum mac_ax_reg_sel sel)
{
	u32 ret;

	adapter->sm.l2_st = MAC_AX_L2_DIS;

	PLTFM_MSG_ALWAYS("MAC Registers:\n");
	ret = __dump_mac_reg(adapter, sel);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dump mac memory\n");
		return ret;
	}

	adapter->sm.l2_st = MAC_AX_L2_EN;

	return MACSUCCESS;
}

static void print_dbg_port_sel(struct mac_ax_adapter *adapter, u32 sel)
{
	u32 i;

	switch (sel) {
	case MAC_AX_DBG_PORT_SEL_PTCL_C0:
		PLTFM_MSG_ALWAYS("Dump debug port PTCL C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PTCL_C1:
		PLTFM_MSG_ALWAYS("Dump debug port PTCL C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_SCH_C0:
		PLTFM_MSG_ALWAYS("Dump debug port SCH C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_SCH_C1:
		PLTFM_MSG_ALWAYS("Dump debug port SCH C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TMAC_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TMAC C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TMAC_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TMAC C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_C0:
		PLTFM_MSG_ALWAYS("Dump debug port RMAC C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMAC_C1:
		PLTFM_MSG_ALWAYS("Dump debug port RMAC C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMACST_C0:
		PLTFM_MSG_ALWAYS("Dump debug port RMACST C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_RMACST_C1:
		PLTFM_MSG_ALWAYS("Dump debug port RMACST C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TRXPTCL_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TRXPTCL C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TRXPTCL_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TRXPTCL C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TXINFOL C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TXINFOH C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOL_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TXINFOL C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TX_INFOH_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TXINFOH C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TXTFINFOL C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C0:
		PLTFM_MSG_ALWAYS("Dump debug port TXTFINFOH C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOL_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TXTFINFOL C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXTF_INFOH_C1:
		PLTFM_MSG_ALWAYS("Dump debug port TXTFINFOH C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA0_C0:
		PLTFM_MSG_ALWAYS("Dump debug port CMAC DMA0 C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C0:
		PLTFM_MSG_ALWAYS("Dump debug port CMAC DMA1 C0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA0_C1:
		PLTFM_MSG_ALWAYS("Dump debug port CMAC DMA0 C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_CMAC_DMA1_C1:
		PLTFM_MSG_ALWAYS("Dump debug port CMAC DMA1 C1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_BUFMGN_CTL:
		PLTFM_MSG_ALWAYS("Dump debug port wde bufmgn ctl:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_BUFMGN_ARB:
		PLTFM_MSG_ALWAYS("Dump debug port wde bufmgn arb:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_CTL:
		PLTFM_MSG_ALWAYS("Dump debug port wde quemgn ctl:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_INFO:
		PLTFM_MSG_ALWAYS("Dump debug port wde quemgn info:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_QUEMGN_ARB:
		PLTFM_MSG_ALWAYS("Dump debug port wde quemgn arb:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT0:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT1:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT3:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 3:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT4:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 4:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT6:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 6:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDE_PORT7:
		PLTFM_MSG_ALWAYS("Dump debug port wde port 7:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_BUFMGN_CTL:
		PLTFM_MSG_ALWAYS("Dump debug port ple bufmgn ctl:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_BUFMGN_ARB:
		PLTFM_MSG_ALWAYS("Dump debug port ple bufmgn arb:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_CTL:
		PLTFM_MSG_ALWAYS("Dump debug port ple quemgn ctl:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_INFO:
		PLTFM_MSG_ALWAYS("Dump debug port ple quemgn info:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_QUEMGN_ARB:
		PLTFM_MSG_ALWAYS("Dump debug port ple quemgn arb:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT0:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT1:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 1_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT2:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 2:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT2_1:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 2_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT3:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 3:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT4:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 4:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT5:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 5:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PLE_PORT6:
		PLTFM_MSG_ALWAYS("Dump debug port ple port 6:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_CTL:
		PLTFM_MSG_ALWAYS("Dump debug port wdrls ctl:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_RPTGEN0:
		PLTFM_MSG_ALWAYS("Dump debug port wdrls rptgen0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_RPTGEN1:
		PLTFM_MSG_ALWAYS("Dump debug port wdrls rptgen1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_PLED_CH0:
		PLTFM_MSG_ALWAYS("Dump debug port wdrls pledchn0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_WDRLS_PLED_CH1:
		PLTFM_MSG_ALWAYS("Dump debug port wdrls pledchn1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_FETPKT:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl fetpkt:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_CMDPSR:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 cmdpsr:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_CMACDMAIF:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 cmacdmaif:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_PRELD0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 preld0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_PRELD1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 preld1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT0_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit0_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT0_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit0_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT1_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit1_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT1_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit1_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT2_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit2_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT2_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit2_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT3_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit3_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT3_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit3_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT4_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit4_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT4_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit4_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT5_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit5_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT5_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit5_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT6_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit6_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT6_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit6_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT7_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit7_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B0_UNIT7_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b0 unit7_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_CMDPSR:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b1 cmdpsr:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_CMACDMAIF:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b1 cmacdmaif:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_UNIT0_0:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b1 unit0_0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_TXPKTCTRL_B1_UNIT0_1:
		PLTFM_MSG_ALWAYS("Dump debug port txpktctrl b1 unit0_1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PKTINFO:
		PLTFM_MSG_ALWAYS("Dump debug port pktinfo:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_MPDUINFO_B0:
		PLTFM_MSG_ALWAYS("Dump debug port mpduinfo b0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_MPDUINFO_B1:
		PLTFM_MSG_ALWAYS("Dump debug port mpduinfo b1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PRELD_B0:
		PLTFM_MSG_ALWAYS("Dump debug port preld b0:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PRELD_B1:
		PLTFM_MSG_ALWAYS("Dump debug port preld b1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_TXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE TXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_RXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE RXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_CVT:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE CVT:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC04:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 0-4:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC5:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 5:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC6:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 6:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC7:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 7:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_PNP_IO:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC P-NP-IO:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC814:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 8-14:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC15:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 15:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC16:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 16:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC17:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 17:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PCIE_EMAC18:
		PLTFM_MSG_ALWAYS("Dump debug port PCIE EMAC 18:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_PHY:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 PHY:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 SIE:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_UTMI:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 UTMI:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE_MMU:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 SIE MMU:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_SIE_PCE:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 SIE PCE:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_UTMI_IF:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 UTMI IF:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLTX:
		PLTFM_MSG_ALWAYS("Dump debug port USB WLTX:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLRX:
		PLTFM_MSG_ALWAYS("Dump debug port USB WLRX:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB3:
		PLTFM_MSG_ALWAYS("Dump debug port USB3:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_SETUP:
		PLTFM_MSG_ALWAYS("Dump debug port USB SETUP:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLTX_DMA:
		PLTFM_MSG_ALWAYS("Dump debug port USB WLTXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_WLRX_DMA:
		PLTFM_MSG_ALWAYS("Dump debug port USB WLRXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_AINST:
		PLTFM_MSG_ALWAYS("Dump debug port USB AINST:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_MISC:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 MISC:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB_BTTX:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 BTTX:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_USB2_BT:
		PLTFM_MSG_ALWAYS("Dump debug port USB2 BT:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX1:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX2:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX3:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX4:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX5:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX6:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX7:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX8:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX9:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXA:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXB:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXC:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXD:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXE:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_TXF:
		i = sel - MAC_AX_DBG_PORT_SEL_DSPT_HDT_TX0;
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER HDT TX%x:\n", i);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX0:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX3:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX4:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX5:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX6:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX7:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX8:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX9:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXA:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXB:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_TXC:
		if (sel == MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX0)
			i = 0;
		else
			i = sel + 3 - MAC_AX_DBG_PORT_SEL_DSPT_CDT_TX3;
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER CDT TX%x:\n", i);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX0:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX1:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX2:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX3:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX4:
	case MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX5:
		i = sel - MAC_AX_DBG_PORT_SEL_DSPT_HDT_RX0;
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER HDT RX%x:\n", i);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_0:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_1:
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_2:
		i = sel - MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P0_0;
		PLTFM_MSG_ALWAYS("Dump debug port DISP CDT RX part0 %x:\n",
				 i);
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_CDT_RX_P1:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER CDT RX part1:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_STF_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER STF Control:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_ADDR_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER ADDR Control:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_WDE_INTF:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER WDE Interface:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_PLE_INTF:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER PLE Interface:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_DSPT_FLOW_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port DISPATCHER FLOW Control:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_AXI_TXDMA_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port AXI_TXDMA_CTRL:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_AXI_RXDMA_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port AXI_RXDMA_CTRL:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_AXI_MST_WLAN:
		PLTFM_MSG_ALWAYS("Dump debug port AXI_MST_WLAN:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_AXI_INT_WLAN:
		PLTFM_MSG_ALWAYS("Dump debug port AXI_INT_WLAN::\n");
		break;
	case MAC_AX_DBG_PORT_SEL_AXI_PAGE_FLOW_CTRL:
		PLTFM_MSG_ALWAYS("Dump debug port AXI_PAGE_FLOW_CTRL:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_TXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port PAXIDMA TXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_RXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port PAXIDMA RXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_MST:
		PLTFM_MSG_ALWAYS("Dump debug port PAXIDMA MST:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_PAXI_INT:
		PLTFM_MSG_ALWAYS("Dump debug port PAXIDMA INT:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_TXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port HAXIDMA TXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_RXDMA:
		PLTFM_MSG_ALWAYS("Dump debug port HAXIDMA RXDMA:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_MST:
		PLTFM_MSG_ALWAYS("Dump debug port HAXIDMA MST:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_HAXI_INT:
		PLTFM_MSG_ALWAYS("Dump debug port HAXIDMA INT:\n");
		break;
	case MAC_AX_DBG_PORT_SEL_STA_SCH:
		PLTFM_MSG_ALWAYS("Dump debug port STA scheduler:\n");
		break;
	default:
		PLTFM_MSG_WARN("Dump unknown debug port sel %d:\n", sel);
		break;
	}
}

void print_dbg_port(struct mac_ax_adapter *adapter, struct mac_ax_dbg_port_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 i;
	u8 val8;
	u16 val16;
	u32 val32;

	for (i = info->srt; i <= info->end; i += info->inc_num) {
		switch (info->sel_byte) {
		case MAC_AX_BYTE_SEL_1:
		default:
			val8 = SET_CLR_WOR2(MAC_REG_R8(info->sel_addr), i,
					    info->sel_sh,
					    info->sel_msk);
			MAC_REG_W8(info->sel_addr, val8);
			PLTFM_MSG_ALWAYS("0x%02X: ", val8);
			break;
		case MAC_AX_BYTE_SEL_2:
			val16 = SET_CLR_WOR2(MAC_REG_R16(info->sel_addr), i,
					     info->sel_sh,
					     info->sel_msk);
			MAC_REG_W16(info->sel_addr, val16);
			PLTFM_MSG_ALWAYS("0x%04X: ", val16);
			break;
		case MAC_AX_BYTE_SEL_4:
			val32 = SET_CLR_WOR2(MAC_REG_R32(info->sel_addr), i,
					     info->sel_sh,
					     info->sel_msk);
			MAC_REG_W32(info->sel_addr, val32);
			PLTFM_MSG_ALWAYS("0x%08X: ", val32);
			break;
		}

		PLTFM_DELAY_US(DBG_PORT_DUMP_DLY_US);

		switch (info->rd_byte) {
		case MAC_AX_BYTE_SEL_1:
		default:
			val8 = GET_FIEL2(MAC_REG_R8(info->rd_addr),
					 info->rd_sh, info->rd_msk);
			PLTFM_MSG_ALWAYS("0x%02X\n", val8);
			break;
		case MAC_AX_BYTE_SEL_2:
			val16 = GET_FIEL2(MAC_REG_R16(info->rd_addr),
					  info->rd_sh, info->rd_msk);
			PLTFM_MSG_ALWAYS("0x%04X\n", val16);
			break;
		case MAC_AX_BYTE_SEL_4:
			val32 = GET_FIEL2(MAC_REG_R32(info->rd_addr),
					  info->rd_sh, info->rd_msk);
			PLTFM_MSG_ALWAYS("0x%08X\n", val32);
			break;
		}
	}
}

u32 dbg_port_dump(struct mac_ax_adapter *adapter, u32 sel)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_dbg_port_info *info;
	u32 ret;

	ret = p_ops->dbg_port_sel(adapter, &info, sel);
	if (ret != MACSUCCESS)
		return ret;

	print_dbg_port_sel(adapter, sel);

	PLTFM_MSG_ALWAYS("Sel addr = 0x%X\n", info->sel_addr);
	PLTFM_MSG_ALWAYS("Read addr = 0x%X\n", info->rd_addr);

	print_dbg_port(adapter, info);

	ret = p_ops->dbg_port_sel_rst(adapter, sel);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("[ERR] dbg sel reset %d\n", ret);

	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);

	return ret;
}

static u32 ss_poll_own(struct mac_ax_adapter *adapter, u32 addr)
{
	u32 cnt = 100;
	u32 val32 = SS_POLL_UNEXPECTED;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	while (--cnt) {
		val32 = MAC_REG_R32(addr);
		if ((val32 & B_AX_SS_OWN) == 0)
			break;
		PLTFM_DELAY_US(1);
	}

	return val32;
}

static u32 ss_tx_len_chk(struct mac_ax_adapter *adapter, u16 macid)
{
	u32 sel;
	u32 val32 = 0;
	u32 dw[4];
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	for (sel = 0; sel < 4; sel++) {
		MAC_REG_W32(R_AX_SS_SRAM_CTRL_1,
			    B_AX_SS_OWN |
			    SET_WORD(SS_CTRL1_R_TX_LEN, B_AX_SS_CMD_SEL) |
			    SET_WORD(sel, B_AX_SS_AC) | macid);
		dw[sel] = ss_poll_own(adapter, R_AX_SS_SRAM_CTRL_1);
		if (dw[sel] & (BIT(29) | BIT(30))) {
			val32 |= SS_POLL_STAT_TX_LEN;
			PLTFM_MSG_ALWAYS("[WARN]SS_POLL_STAT_TX_LEN, macid %d, ",
					 macid);
			PLTFM_MSG_ALWAYS("ac %d\n", sel);
			PLTFM_MSG_ALWAYS("R_AX_SS_SRAM_CTRL_1 = 0x%x\n",
					 dw[sel]);
			break;
		}
		if (dw[sel] & B_AX_SS_OWN) {
			val32 |= SS_POLL_OWN_TX_LEN;
			PLTFM_MSG_ALWAYS("[WARN]SS_POLL_OWN_TX_LEN, macid %d, ",
					 macid);
			PLTFM_MSG_ALWAYS("ac %d\n", sel);
			PLTFM_MSG_ALWAYS("R_AX_SS_SRAM_CTRL_1 = 0x%x\n",
					 dw[sel]);
			break;
		}
	}

	if (((dw[0] >> SS_MACID_SH) & SS_TX_LEN_MSK) != 0) {
		val32 |= SS_TX_LEN_BE;
		PLTFM_MSG_ALWAYS("[WARN]SS_TX_LEN_BE, macid %d, ", macid);
		PLTFM_MSG_ALWAYS("len 0x%X\n",
				 (dw[0] >> SS_MACID_SH) & SS_TX_LEN_MSK);
	}
	if (((dw[1] >> SS_MACID_SH) & SS_TX_LEN_MSK) != 0) {
		val32 |= SS_TX_LEN_BK;
		PLTFM_MSG_ALWAYS("[WARN]SS_TX_LEN_BK, macid %d, ", macid);
		PLTFM_MSG_ALWAYS("len 0x%X\n",
				 (dw[1] >> SS_MACID_SH) & SS_TX_LEN_MSK);
	}
	if (((dw[2] >> SS_MACID_SH) & SS_TX_LEN_MSK) != 0) {
		val32 |= SS_TX_LEN_VI;
		PLTFM_MSG_ALWAYS("[WARN]SS_TX_LEN_VI, macid %d, ", macid);
		PLTFM_MSG_ALWAYS("len 0x%X\n",
				 (dw[2] >> SS_MACID_SH) & SS_TX_LEN_MSK);
	}
	if (((dw[3] >> SS_MACID_SH) & SS_TX_LEN_MSK) != 0) {
		val32 |= SS_TX_LEN_VO;
		PLTFM_MSG_ALWAYS("[WARN]SS_TX_LEN_VO, macid %d, ", macid);
		PLTFM_MSG_ALWAYS("len 0x%X\n",
				 (dw[3] >> SS_MACID_SH) & SS_TX_LEN_MSK);
	}

	return val32;
}

static u32 ss_link_chk(struct mac_ax_adapter *adapter,
		       struct ss_link_info *link)
{
	u32 val32 = 0;
	u32 poll;
	u8 wmm, ac;
	u8 link_head;
	u8 link_tail;
	u8 link_len;
	u32 i;
	u32 cmd;
	u8 macid;
	u16 id_empty = adapter->hw_info->sta_empty_flg;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (link->ul) {
		wmm = 0;
		ac = 0;
	} else {
		wmm = link->wmm;
		ac = link->ac;
	}

	MAC_REG_W32(R_AX_SS_LINK_INFO, B_AX_SS_OWN |
		    (link->ul ? B_AX_SS_UL : 0) |
		    SET_WORD(wmm, B_AX_SS_WMM) |
		    SET_WORD(ac, B_AX_SS_AC));
	poll = ss_poll_own(adapter, R_AX_SS_LINK_INFO);
	link_head = GET_FIELD(poll, B_AX_SS_LINK_HEAD);
	link_tail = GET_FIELD(poll, B_AX_SS_LINK_TAIL);
	link_len = GET_FIELD(poll, B_AX_SS_LINK_LEN);

	if (poll & (BIT(29) | BIT(30))) {
		val32 |= SS_POLL_STAT_LINK;
		PLTFM_MSG_ALWAYS("[WARN]SS_POLL_STAT_LINK, ul/wmm/ac %d/%d/%d\n",
				 link->ul, wmm, ac);
		PLTFM_MSG_ALWAYS("R_AX_SS_LINK_INFO = 0x%x\n", poll);
		return val32;
	}
	if (poll & B_AX_SS_OWN) {
		val32 |= SS_POLL_OWN_LINK;
		PLTFM_MSG_ALWAYS("[WARN]SS_POLL_OWN_LINK, ul/wmm/ac %d/%d/%d\n",
				 link->ul, wmm, ac);
		PLTFM_MSG_ALWAYS("R_AX_SS_LINK_INFO = 0x%x\n", poll);
		return val32;
	}

	if (link_head != id_empty || link_tail != id_empty || link_len != 0) {
		if (link->ul)
			val32 |= SS_LINK_UL << ((wmm << 2) + ac);
		else
			val32 |= SS_LINK_WMM0_BE << ((wmm << 2) + ac);
		PLTFM_MSG_ALWAYS("[WARN]SS_LINK_DUMP, ul/wmm/ac %d/%d/%d, ",
				 link->ul, wmm, ac);
		PLTFM_MSG_ALWAYS("head/tail/len 0x%X/0x%X/%d\n",
				 link_head, link_tail, link_len);
		macid = link_head;
		i = 0;
		do {
			PLTFM_MSG_ALWAYS("0x%X, ", macid);
			cmd = B_AX_SS_OWN |
			      SET_WORD(SS_CTRL1_R_NEXT_LINK, B_AX_SS_CMD_SEL) |
			      SET_WORD(ac, B_AX_SS_AC) |
			      (link->ul ? BIT(23) : 0) | macid;
			MAC_REG_W32(R_AX_SS_SRAM_CTRL_1, cmd);
			poll = ss_poll_own(adapter, R_AX_SS_SRAM_CTRL_1);
			if (poll & (BIT(29) | BIT(30))) {
				PLTFM_MSG_ALWAYS("SS_LINK_DUMP_POLL_STAT\n");
				PLTFM_MSG_ALWAYS("R_AX_SS_SRAM_CTRL_1 = 0x%x\n",
						 poll);
				break;
			}
			if (poll & B_AX_SS_OWN) {
				PLTFM_MSG_ALWAYS("SS_LINK_DUMP_POLL_OWN\n");
				PLTFM_MSG_ALWAYS("R_AX_SS_SRAM_CTRL_1 = 0x%x\n",
						 poll);
				break;
			}

			if (GET_FIELD(poll, B_AX_SS_VALUE) == id_empty) {
				if (macid != link_tail) {
					PLTFM_MSG_ALWAYS("[WARN]tail error!!\n");
					break;
				}
				if (i >= link_len) {
					PLTFM_MSG_ALWAYS("[WARN]len error!!\n");
					break;
				}
				break;
			}
			macid = GET_FIELD(poll, B_AX_SS_VALUE);

			i++;
		} while (i < SS_LINK_SIZE);
		PLTFM_MSG_ALWAYS("\nSS_LINK_DUMP, end\n");
	}

	return val32;
}

static void ss_dbgpkg_val_parser(struct mac_ax_adapter *adapter,
				 struct mac_ax_dbgpkg *val)
{
	/* STA Scheduler 0, indirect */
	PLTFM_MSG_ALWAYS("[WARN][STA_SCH] ss_dbg_0 = 0x%X\n", val->ss_dbg_0);
	if (val->ss_dbg_0 & SS_TX_LEN_BE)
		PLTFM_MSG_ERR("[WARN][STA_SCH] some MACID's BE length != 0\n");
	if (val->ss_dbg_0 & SS_TX_LEN_BK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] some MACID's BK length != 0\n");
	if (val->ss_dbg_0 & SS_TX_LEN_VI)
		PLTFM_MSG_ERR("[WARN][STA_SCH] some MACID's VI length != 0\n");
	if (val->ss_dbg_0 & SS_TX_LEN_VO)
		PLTFM_MSG_ERR("[WARN][STA_SCH] some MACID's VO length != 0\n");
	if (val->ss_dbg_0 & SS_LINK_WMM0_BE)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_0 BE not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM0_BK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_0 BK not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM0_VI)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_0 VI not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM0_VO)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_0 VO not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM1_BE)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_1 BE not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM1_BK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_1 BK not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM1_VI)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_1 VI not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM1_VO)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_1 VO not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM2_BE)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_2 BE not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM2_BK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_2 BK not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM2_VI)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_2 VI not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM2_VO)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_2 VO not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM3_BE)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_3 BE not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM3_BK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_3 BK not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM3_VI)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_3 VI not empty\n");
	if (val->ss_dbg_0 & SS_LINK_WMM3_VO)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WMM_3 VO not empty\n");
	if (val->ss_dbg_0 & SS_LINK_UL)
		PLTFM_MSG_ERR("[WARN][STA_SCH] UL not empty\n");
	if (val->ss_dbg_0 & SS_POLL_OWN_TX_LEN)
		PLTFM_MSG_ERR("[WARN][STA_SCH] (length) own bit polling fail\n");
	if (val->ss_dbg_0 & SS_POLL_OWN_LINK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] (link) own bit polling fail\n");
	if (val->ss_dbg_0 & SS_POLL_STAT_TX_LEN)
		PLTFM_MSG_ERR("[WARN][STA_SCH] (length) state machine fail\n");
	if (val->ss_dbg_0 & SS_POLL_STAT_LINK)
		PLTFM_MSG_ERR("[WARN][STA_SCH] (link) state machine fail\n");

	/* STA Scheduler 1, direct */
	PLTFM_MSG_ALWAYS("[WARN][STA_SCH] ss_dbg_1 = 0x%X\n", val->ss_dbg_1);
	if (val->ss_dbg_1 & SS_TX_HW_LEN_UDN)
		PLTFM_MSG_ERR("[WARN][STA_SCH] HW cause length underflow\n");
	if (val->ss_dbg_1 & SS_TX_SW_LEN_UDN)
		PLTFM_MSG_ERR("[WARN][STA_SCH] SW cause length underflow\n");
	if (val->ss_dbg_1 & SS_TX_HW_LEN_OVF)
		PLTFM_MSG_ERR("[WARN][STA_SCH] HW cause length overflow\n");
	if (val->ss_dbg_1 & SS_STAT_FWTX)
		PLTFM_MSG_ERR("[WARN][STA_SCH] SW Tx state machine not idle\n");
	if (val->ss_dbg_1 & SS_STAT_RPTA)
		PLTFM_MSG_ERR("[WARN][STA_SCH] Report state machine not idle\n");
	if (val->ss_dbg_1 & SS_STAT_WDEA)
		PLTFM_MSG_ERR("[WARN][STA_SCH] WDE state machine not idle\n");
	if (val->ss_dbg_1 & SS_STAT_PLEA)
		PLTFM_MSG_ERR("[WARN][STA_SCH] PLE state machine not idle\n");
	if (val->ss_dbg_1 & SS_STAT_ULRU)
		PLTFM_MSG_ERR("[WARN][STA_SCH] UL RU state machine not idle\n");
	if (val->ss_dbg_1 & SS_STAT_DLTX)
		PLTFM_MSG_ERR("[WARN][STA_SCH] HW Tx state machine not idle\n");
}

static void ps_dbg_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u16 reg32 = 0;
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg32 = R_AX_CPWM;
	}
#endif
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		reg32 = R_AX_CPWM_V1;
	}
#endif

	PLTFM_MSG_ALWAYS("R_AX_PLATFORM_ENABLE=0x%x\n",
			 MAC_REG_R32(R_AX_PLATFORM_ENABLE));
	PLTFM_MSG_ALWAYS("R_AX_WLLPS_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_WLLPS_CTRL));
	PLTFM_MSG_ALWAYS("R_AX_WLRESUME_CTRL=0x%x\n",
			 MAC_REG_R32(R_AX_WLRESUME_CTRL));
	PLTFM_MSG_ALWAYS("R_AX_SYS_CFG5=0x%x\n",
			 MAC_REG_R32(R_AX_SYS_CFG5));
	PLTFM_MSG_ALWAYS("R_AX_IC_PWR_STATE=0x%x\n",
			 MAC_REG_R32(R_AX_IC_PWR_STATE));
	switch (intf) {
	case MAC_AX_INTF_USB:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ALWAYS("USB HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_USB_D2F_F2D_INFO + 2));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ALWAYS("USB HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_USB_D2F_F2D_INFO_V1 + 2));
		}
#endif
		break;
	case MAC_AX_INTF_SDIO:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ALWAYS("SDIO HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_SDIO_HRPWM1 + 2));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ALWAYS("SDIO HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_SDIO_HRPWM1_V1 + 2));
		}
#endif
		break;
	case MAC_AX_INTF_PCIE:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ALWAYS("PCIE HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_PCIE_HRPWM));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ALWAYS("PCIE HRPWM=0x%x\n",
					 MAC_REG_R16(R_AX_PCIE_HRPWM_V1));
		}
#endif
		break;
	default:
		PLTFM_MSG_ALWAYS("RPWM error interface=%d\n",
				 intf);
		break;
	}
	PLTFM_MSG_ALWAYS("R_AX_RPWM=0x%x\n",
			 MAC_REG_R16(R_AX_RPWM));
	PLTFM_MSG_ALWAYS("R_AX_LDM=0x%x\n",
			 MAC_REG_R32(R_AX_LDM));

	if (mac_io_chk_access(adapter, reg32) == MACSUCCESS) {
		PLTFM_MSG_ALWAYS("R_AX_CPWM=0x%x\n",
				 MAC_REG_R16(reg32));
	}
}

u32 fw_backtrace_dump(struct mac_ax_adapter *adapter)
{
	u32 addr = 0;
	u32 str_addr = 0;
	u32 size = 0;
	u32 key = 0;
	u32 i;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct fw_backtrace_info info;

	if (adapter->mac_pwr_info.pwr_seq_proc ||
	    adapter->sm.pwr != MAC_AX_PWR_ON ||
	    adapter->mac_pwr_info.pwr_in_lps ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_RECV_DONE ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_PROCESS) {
		PLTFM_MSG_ERR("[ERR]pwr seq proc %d/sm pwr %d/pwr in lps %d\n",
			      adapter->mac_pwr_info.pwr_seq_proc,
			      adapter->sm.pwr,
			      adapter->mac_pwr_info.pwr_in_lps);
		PLTFM_MSG_ERR("[ERR]sm fw rst %d\n", adapter->sm.fw_rst);
		return MACPROCERR;
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		addr = RSVD_PLE_OFST_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		addr = RSVD_PLE_OFST_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		addr = RSVD_PLE_OFST_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		addr = RSVD_PLE_OFST_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		addr = RSVD_PLE_OFST_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		addr = RSVD_PLE_OFST_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		addr = RSVD_PLE_OFST_8852D;
	} else {
		PLTFM_MSG_ERR("[ERR]unknown chip id\n");
		return MACCHIPID;
	}

	// Get FW Backtrace start address and size
	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees!\n");
		PLTFM_MSG_ERR("[ERR]Skip FW Backtrace Check!\n");
	} else {
		addr = base_addr_map_tbl[MAC_AX_MEM_SHARED_BUF] + addr;
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, addr);
		str_addr = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY) & ~0xA0000000;
		size = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + FW_BACKTRACE_SIZE_OFST);
		key = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + FW_BACKTRACE_KEY_OFST);
		adapter->hw_info->ind_aces_cnt--;
		PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
		if (str_addr == 0) {
			PLTFM_MSG_ERR("[FW Backtrace] Invalid address(0x%x)!\n", str_addr);
			return MACBADDR;
		}
		if (size == 0 || size > FW_BACKTRACE_MAX_SIZE
		    || (size % sizeof(struct fw_backtrace_info) != 0)) {
			PLTFM_MSG_ERR("[FW Backtrace] Invalid size(0x%x)!\n", size);
			return MACBUFSZ;
		}
		if (key != FW_BACKTRACE_KEY) {
			PLTFM_MSG_ERR("[FW Backtrace] Invalid key(0x%x)!\n", key);
			return MACNOITEM;
		}
	}

	// Dump FW backtrace
	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees!\n");
		PLTFM_MSG_ERR("[ERR]Skip FW Backtrace Check!\n");
	} else {
		PLTFM_MSG_WARN("%s ind access FW backtrace start\n", __func__);
		PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
		adapter->hw_info->ind_aces_cnt++;
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, str_addr);
		for (i = 0; i < size; i += sizeof(struct fw_backtrace_info)) {
			info.ra = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + i);
			info.sp = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + i + 4);
			PLTFM_MSG_ERR("Next SP:0x%x, Next RA:0x%x\n", info.sp, info.ra);
		}
		adapter->hw_info->ind_aces_cnt--;
		PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
		PLTFM_MSG_WARN("%s ind access FW backtrace end\n", __func__);
	}

	return MACSUCCESS;
}

void pltfm_dbg_dump(struct mac_ax_adapter *adapter)
{
	u32 val32;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ALWAYS("R_AX_SER_DBG_INFO =0x%08x\n",
			 MAC_REG_R32(R_AX_SER_DBG_INFO));

	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_INIT_CFG1, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_INIT_CFG1 = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_DMA_STOP, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_DMA_STOP = 0x%x\n", val32);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_INIT_CFG2, AXIDMA_SEL);
		PLTFM_MSG_ALWAYS("R_PL_AXIDMA_INIT_CFG2 = 0x%x\n", val32);
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		val32 = mac_sram_dbg_read(adapter, R_AX_HAXI_INIT_CFG1, AXIDMA_SEL);
		PLTFM_MSG_ALWAYS("R_AX_HAXI_INIT_CFG1 = 0x%x\n", val32);
	}
#endif
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_INFO, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_INFO = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_BUSY, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_BUSY = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH0_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH0_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH1_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH1_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH2_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH2_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH3_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH3_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH4_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH4_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH5_RXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH5_RXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH0_TXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH0_TXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH1_TXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH1_TXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH2_TXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH2_TXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_CH3_TXBD_IDX, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_CH3_TXBD_IDX = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_DBG_ERR_FLAG, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_DBG_ERR_FLAG = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_FWIMR0, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_FWIMR0 = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_FWIMR1, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_FWIMR1 = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_FWISR0, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_FWISR0 = 0x%x\n", val32);
	val32 = mac_sram_dbg_read(adapter, R_PL_AXIDMA_FWISR1, AXIDMA_SEL);
	PLTFM_MSG_ALWAYS("R_PL_AXIDMA_FWISR1 = 0x%x\n", val32);
}

u32 fw_pc_dbg_dump_ax(struct mac_ax_adapter *adapter)
{
	u32 val32;
	u16 index;
	u8 chip_id;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	chip_id = adapter->hw_info->chip_id;

	PLTFM_MUTEX_LOCK(&adapter->hw_info->dbg_port_lock);
	adapter->hw_info->dbg_port_cnt++;

	MAC_REG_W32(R_AX_DBG_CTRL, 0xf200f2);
	val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
	val32 = SET_CLR_WORD(val32, 0x1, B_AX_SEL_0XC0);
	MAC_REG_W32(R_AX_SYS_STATUS1, val32);

	for (index = 0; index < 15; index++) {
		switch (chip_id) {
#if defined(MAC_8852A_SUPPORT) || defined(MAC_8852B_SUPPORT) || defined(MAC_8851B_SUPPORT)
		case MAC_AX_CHIP_ID_8852A:
		case MAC_AX_CHIP_ID_8852B:
		case MAC_AX_CHIP_ID_8851B:
			val32 = MAC_REG_R32(R_AX_DBG_PORT_SEL);
			break;
#endif
#if (defined(MAC_8852C_SUPPORT) || defined(MAC_8192XB_SUPPORT) || \
defined(MAC_8851E_SUPPORT) || defined(MAC_8852D_SUPPORT))
		case MAC_AX_CHIP_ID_8852C:
		case MAC_AX_CHIP_ID_8192XB:
		case MAC_AX_CHIP_ID_8851E:
		case MAC_AX_CHIP_ID_8852D:
			val32 = MAC_REG_R32(R_AX_WLCPU_PORT_PC);
			break;
#endif
		default:
			val32 = 0xEAEAEAEA;
			PLTFM_MSG_ERR("[ERR]%s :Unknown Chip ID. 0x%X\n", __func__, chip_id);
			break;
		}

		PLTFM_MSG_ERR("[ERR]fw PC = 0x%x\n", val32);
		PLTFM_DELAY_US(10);
	}

	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);
	return MACSUCCESS;
}

u32 fw_st_dbg_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

	if (adapter->mac_pwr_info.pwr_seq_proc ||
	    adapter->sm.pwr != MAC_AX_PWR_ON) {
		PLTFM_MSG_ERR("[ERR]pwr seq proc %d/sm pwr %d\n",
			      adapter->mac_pwr_info.pwr_seq_proc,
			      adapter->sm.pwr);
		return MACPROCERR;
	}

	PLTFM_MSG_ALWAYS("FW status = 0x%x\n", MAC_REG_R32(R_AX_UDM0));
	PLTFM_MSG_ALWAYS("FW BADADDR = 0x%x\n", MAC_REG_R32(R_AX_UDM1));
	PLTFM_MSG_ALWAYS("FW EPC/RA = 0x%x\n", MAC_REG_R32(R_AX_UDM2));
	PLTFM_MSG_ALWAYS("FW MISC = 0x%x\n", MAC_REG_R32(R_AX_UDM3));
	PLTFM_MSG_ALWAYS("R_AX_HALT_C2H = 0x%x\n", MAC_REG_R32(R_AX_HALT_C2H));

	val32 = MAC_REG_R32(R_AX_HALT_C2H);
	if (val32 == MAC_AX_ERR_L1_ERR_DMAC || val32 == MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE)
		PLTFM_MSG_ALWAYS("[WARN] SER L1 is occured, but not recovery done yet\n");

	if (mac_io_chk_access(adapter, R_AX_SER_DBG_INFO) == MACSUCCESS) {
		PLTFM_MSG_ALWAYS("R_AX_SER_DBG_INFO = 0x%x\n", MAC_REG_R32(R_AX_SER_DBG_INFO));
		PLTFM_MSG_ALWAYS("R_AX_DMAC_ERR_ISR = 0x%x\n", MAC_REG_R32(R_AX_DMAC_ERR_ISR));
	}

	/* Dump FW program counter */
	ret = fw_pc_dbg_dump_ax(adapter);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 rsvd_ple_dump(struct mac_ax_adapter *adapter)
{
	u32 ret;
	u32 addr;

	if (adapter->mac_pwr_info.pwr_seq_proc ||
	    adapter->sm.pwr != MAC_AX_PWR_ON ||
	    adapter->mac_pwr_info.pwr_in_lps ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_RECV_DONE ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_PROCESS) {
		PLTFM_MSG_ERR("[ERR]pwr seq proc %d/sm pwr %d/pwr in lps %d\n",
			      adapter->mac_pwr_info.pwr_seq_proc,
			      adapter->sm.pwr,
			      adapter->mac_pwr_info.pwr_in_lps);
		PLTFM_MSG_ERR("[ERR]sm fw rst %d\n", adapter->sm.fw_rst);
		return MACPROCERR;
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		addr = RSVD_PLE_OFST_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		addr = RSVD_PLE_OFST_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		addr = RSVD_PLE_OFST_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		addr = RSVD_PLE_OFST_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		addr = RSVD_PLE_OFST_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		addr = RSVD_PLE_OFST_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		addr = RSVD_PLE_OFST_8852D;
	} else {
		PLTFM_MSG_ERR("[ERR]unknown chip id\n");
		return MACCHIPID;
	}

	ret = mac_mem_dump(adapter, MAC_AX_MEM_SHARED_BUF, addr, 0,
			   FW_RSVD_PLE_SIZE, 0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("Dump fw rsvd ple\n");
		return ret;
	}

	ret = fw_backtrace_dump(adapter);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("fw backtrace dump %d\n", ret);

	return MACSUCCESS;
}

u32 ss_dbgpkg(struct mac_ax_adapter *adapter, struct mac_ax_dbgpkg *val)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_role_tbl *role;
	struct ss_link_info link;
	u32 ret, val32;
	u16 macid_num = adapter->hw_info->macid_num;
	u16 i_u16;
	u8 wmm_num, ul_vld, wmm, ac;

	PLTFM_MSG_ALWAYS("%s...", __func__);

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ALWAYS("[ERR] check dmac en %d\n", ret);
		return ret;
	}

	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	if ((val32 & B_AX_MAC_FUNC_EN) == 0) {
		PLTFM_MSG_ALWAYS("[ERR] B_AX_MAC_FUNC_EN=0");
		return MACSUCCESS;
	}
	if ((val32 & B_AX_DMAC_FUNC_EN) == 0) {
		PLTFM_MSG_ALWAYS("[ERR] B_AX_DMAC_FUNC_EN=0");
		return MACSUCCESS;
	}
	if ((val32 & B_AX_STA_SCH_EN) == 0) {
		PLTFM_MSG_ALWAYS("[ERR] B_AX_STA_SCH_EN=0");
		return MACSUCCESS;
	}
	val32 = MAC_REG_R32(R_AX_DMAC_CLK_EN);
	if ((val32 & B_AX_STA_SCH_CLK_EN) == 0) {
		PLTFM_MSG_ALWAYS("[ERR] B_AX_STA_SCH_CLK_EN=0");
		return MACSUCCESS;
	}
	val32 = MAC_REG_R32(R_AX_SS_CTRL);
	if ((val32 & B_AX_SS_EN) == 0) {
		PLTFM_MSG_ALWAYS("[ERR] B_AX_SS_EN=0");
		return MACSUCCESS;
	}

	switch (adapter->hw_info->chip_id) {
	case MAC_AX_CHIP_ID_8852A:
		wmm_num = SS_WMM_NUM_8852A;
		ul_vld = SS_UL_SUPPORT_8852A;
		break;
	case MAC_AX_CHIP_ID_8852B:
		wmm_num = SS_WMM_NUM_8852B;
		ul_vld = SS_UL_SUPPORT_8852B;
		break;
	case MAC_AX_CHIP_ID_8852C:
		wmm_num = SS_WMM_NUM_8852C;
		ul_vld = SS_UL_SUPPORT_8852C;
		break;
	case MAC_AX_CHIP_ID_8192XB:
		wmm_num = SS_WMM_NUM_8192XB;
		ul_vld = SS_UL_SUPPORT_8192XB;
		break;
	case MAC_AX_CHIP_ID_8851B:
		wmm_num = SS_WMM_NUM_8851B;
		ul_vld = SS_UL_SUPPORT_8851B;
		break;
	case MAC_AX_CHIP_ID_8851E:
		wmm_num = SS_WMM_NUM_8851E;
		ul_vld = SS_UL_SUPPORT_8851E;
		break;
	case MAC_AX_CHIP_ID_8852D:
		wmm_num = SS_WMM_NUM_8852D;
		ul_vld = SS_UL_SUPPORT_8852D;
		break;
	default:
		wmm_num = 0;
		ul_vld = 0;
		break;
	}

	for (i_u16 = 0; i_u16 < macid_num; i_u16++) {
		role = mac_role_srch(adapter, (u8)i_u16);
		if (role) {
			PLTFM_MSG_ALWAYS("[ss_tx_len] macid = %d", i_u16);
			val->ss_dbg_0 |= ss_tx_len_chk(adapter, i_u16);
			if (val->ss_dbg_0 &
			    (SS_POLL_STAT_TX_LEN | SS_POLL_OWN_TX_LEN))
				break;
		}
	}

	link.ul = 0;
	for (wmm = 0; wmm < wmm_num; wmm++) {
		link.wmm = wmm;
		for (ac = 0; ac < 4; ac++) {
			link.ac = ac;
			val->ss_dbg_0 |= ss_link_chk(adapter, &link);
			if (val->ss_dbg_0 &
			    (SS_POLL_STAT_LINK | SS_POLL_OWN_LINK))
				break;
		}
		if (val->ss_dbg_0 & (SS_POLL_STAT_LINK | SS_POLL_OWN_LINK))
			break;
	}

	if (ul_vld) {
		link.ul = 1;
		link.wmm = 0;
		link.ac = 0;
		val->ss_dbg_0 |= ss_link_chk(adapter, &link);
	}

	val->ss_dbg_1 |= p_ops->ss_stat_chk(adapter);
	ss_dbgpkg_val_parser(adapter, val);

	return MACSUCCESS;
}

u32 dle_dbg_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct dle_dfi_freepg_t freepg;
	struct dle_dfi_quota_t quota;
	struct dle_dfi_qempty_t qempty;
	u32 qtaid, grpsel;
	u32 qnum, qtanum, ret, val32;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] check dmac en %d\n", ret);
		return ret;
	}

	PLTFM_MSG_ALWAYS("R_AX_WDE_PKTBUF_CFG=0x%x\n",
			 MAC_REG_R32(R_AX_WDE_PKTBUF_CFG));
	PLTFM_MSG_ALWAYS("R_AX_PLE_PKTBUF_CFG=0x%x\n",
			 MAC_REG_R32(R_AX_PLE_PKTBUF_CFG));

	val32 = MAC_REG_R32(R_AX_WDE_QTA0_CFG);
	PLTFM_MSG_ALWAYS("[WDE][HIF]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q0_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[WDE][HIF]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q0_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_WDE_QTA1_CFG);
	PLTFM_MSG_ALWAYS("[WDE][WLAN_CPU]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q1_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[WDE][WLAN_CPU]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q1_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_WDE_QTA2_CFG);
	PLTFM_MSG_ALWAYS("[WDE][DATA_CPU]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q2_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[WDE][DATA_CPU]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q2_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_WDE_QTA3_CFG);
	PLTFM_MSG_ALWAYS("[WDE][PKTIN]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q3_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[WDE][PKTIN]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q3_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_WDE_QTA4_CFG);
	PLTFM_MSG_ALWAYS("[WDE][CPUIO]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q4_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[WDE][CPUIO]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_WDE_Q4_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA0_CFG);
	PLTFM_MSG_ALWAYS("[PLE][B0_TXPL]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q0_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][B0_TXPL]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q0_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA1_CFG);
	PLTFM_MSG_ALWAYS("[PLE][B1_TXPL]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q1_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][B1_TXPL]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q1_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA2_CFG);
	PLTFM_MSG_ALWAYS("[PLE][C2H]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q2_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][C2H]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q2_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA3_CFG);
	PLTFM_MSG_ALWAYS("[PLE][H2C]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q3_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][H2C]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q3_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA4_CFG);
	PLTFM_MSG_ALWAYS("[PLE][WLAN_CPU]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q4_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][WLAN_CPU]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q4_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA5_CFG);
	PLTFM_MSG_ALWAYS("[PLE][MPDU]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q5_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][MPDU]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q5_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA6_CFG);
	PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q6_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q6_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA7_CFG);
	PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q7_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q7_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA8_CFG);
	PLTFM_MSG_ALWAYS("[PLE][BBRPT]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q8_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][BBRPT]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q8_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA9_CFG);
	PLTFM_MSG_ALWAYS("[PLE][WDRLS]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q9_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][WDRLS]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q9_MAX_SIZE));

	val32 = MAC_REG_R32(R_AX_PLE_QTA10_CFG);
	PLTFM_MSG_ALWAYS("[PLE][CPUIO]min_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q10_MIN_SIZE));
	PLTFM_MSG_ALWAYS("[PLE][CPUIO]max_pgnum=0x%x\n",
			 GET_FIELD(val32, B_AX_PLE_Q10_MAX_SIZE));

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		val32 = MAC_REG_R32(R_AX_PLE_QTA11_CFG);
		PLTFM_MSG_ALWAYS("[PLE][TXRPT]min_pgnum=0x%x\n",
				 GET_FIELD(val32, B_AX_PLE_Q11_MIN_SIZE));
		PLTFM_MSG_ALWAYS("[PLE][TXRPT]max_pgnum=0x%x\n",
				 GET_FIELD(val32, B_AX_PLE_Q11_MAX_SIZE));
	}
#endif

	freepg.dle_type = DLE_CTRL_TYPE_WDE;
	ret = dle_dfi_freepg(adapter, &freepg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] wde dfi freepg %d\n", ret);
		return ret;
	}
	PLTFM_MSG_ALWAYS("[WDE]free_headpg = 0x%x\n", freepg.free_headpg);
	PLTFM_MSG_ALWAYS("[WDE]free_tailpg = 0x%x\n", freepg.free_tailpg);
	PLTFM_MSG_ALWAYS("[WDE]pub_pgnum = 0x%x\n", freepg.pub_pgnum);

	freepg.dle_type = DLE_CTRL_TYPE_PLE;
	ret = dle_dfi_freepg(adapter, &freepg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] ple dfi freepg %d\n", ret);
		return ret;
	}
	PLTFM_MSG_ALWAYS("[PLE]free_headpg = 0x%x\n", freepg.free_headpg);
	PLTFM_MSG_ALWAYS("[PLE]free_tailpg = 0x%x\n", freepg.free_tailpg);
	PLTFM_MSG_ALWAYS("[PLE]pub_pgunm = 0x%x\n", freepg.pub_pgnum);

	quota.dle_type = DLE_CTRL_TYPE_WDE;
	for (qtaid = 0; qtaid < WDE_QTA_NUM; qtaid++) {
		quota.qtaid = qtaid;
		ret = dle_dfi_quota(adapter, &quota);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] wde dfi quota %d\n", ret);
			return ret;
		}

		switch (qtaid) {
		case WDE_QTAID_HOST_IF:
			PLTFM_MSG_ALWAYS("[WDE][HIF]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[WDE][HIF]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case WDE_QTAID_WLAN_CPU:
			PLTFM_MSG_ALWAYS("[WDE][WLAN_CPU]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[WDE][WLAN_CPU]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case WDE_QTAID_DATA_CPU:
			PLTFM_MSG_ALWAYS("[WDE][DATA_CPU]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[WDE][DATA_CPU]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case WDE_QTAID_PKTIN:
			PLTFM_MSG_ALWAYS("[WDE][PKTIN]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[WDE][PKTIN]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case WDE_QTAID_CPUIO:
			PLTFM_MSG_ALWAYS("[WDE][CPUIO]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[WDE][CPUIO]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		}
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		qtanum = PLE_QTA_NUM_8852AB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		qtanum = PLE_QTA_NUM_8852C;
	} else {
		PLTFM_MSG_ERR("[ERR] ple qta num not define\n");
		return MACCHIPID;
	}
	quota.dle_type = DLE_CTRL_TYPE_PLE;
	for (qtaid = 0; qtaid < qtanum; qtaid++) {
		quota.qtaid = qtaid;
		ret = dle_dfi_quota(adapter, &quota);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] dle dfi quota %d\n", ret);
			return ret;
		}

		switch (qtaid) {
		case PLE_QTAID_B0_TXPL:
			PLTFM_MSG_ALWAYS("[PLE][B0_TXPL]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][B0_TXPL]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_B1_TXPL:
			PLTFM_MSG_ALWAYS("[PLE][B1_TXPL]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][B1_TXPL]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_C2H:
			PLTFM_MSG_ALWAYS("[PLE][C2H]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][C2H]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_H2C:
			PLTFM_MSG_ALWAYS("[PLE][H2C]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][H2C]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_WLAN_CPU:
			PLTFM_MSG_ALWAYS("[PLE][WLAN_CPU]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][WLAN_CPU]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_MPDU:
			PLTFM_MSG_ALWAYS("[PLE][MPDU]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][MPDU]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_CMAC0_RX:
			PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][CMAC0_RX]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_CMAC1_RX:
			PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][CMAC1_RX]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_BBRPT:
			PLTFM_MSG_ALWAYS("[PLE][BBRPT]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][BBRPT]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_WDRLS:
			PLTFM_MSG_ALWAYS("[PLE][WDRLS]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][WDRLS]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_CPUIO:
			PLTFM_MSG_ALWAYS("[PLE][CPUIO]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][CPUIO]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		case PLE_QTAID_TXRPT:
			PLTFM_MSG_ALWAYS("[PLE][TXRPT]rsv_pgnum = %d\n",
					 quota.rsv_pgnum);
			PLTFM_MSG_ALWAYS("[PLE][TXRPT]use_pgnum = %d\n",
					 quota.use_pgnum);
			break;
		default:
			PLTFM_MSG_ERR("[ERR] ple invalid qtaid %d\n", qtaid);
			break;
		}
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		qnum = WDE_QEMPTY_NUM_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		qnum = WDE_QEMPTY_NUM_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		qnum = WDE_QEMPTY_NUM_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		qnum = WDE_QEMPTY_NUM_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		qnum = WDE_QEMPTY_NUM_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		qnum = WDE_QEMPTY_NUM_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		qnum = WDE_QEMPTY_NUM_8852D;
	} else {
		PLTFM_MSG_ERR("[ERR] wde qempty num not define\n");
		return MACCHIPID;
	}
	qempty.dle_type = DLE_CTRL_TYPE_WDE;
	for (grpsel = 0; grpsel < qnum; grpsel++) {
		qempty.grpsel = grpsel;
		ret = dle_dfi_qempty(adapter, &qempty);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] wde dfi qempty %d\n", ret);
			return ret;
		}
		PLTFM_MSG_ALWAYS("[WDE][Group_%d]qempty = 0x%x\n",
				 grpsel, qempty.qempty);
	}

	qempty.dle_type = DLE_CTRL_TYPE_PLE;
	for (grpsel = 0; grpsel < PLE_QEMPTY_NUM; grpsel++) {
		qempty.grpsel = grpsel;
		ret = dle_dfi_qempty(adapter, &qempty);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] ple dfi qempty %d\n", ret);
			return ret;
		}
		PLTFM_MSG_ALWAYS("[PLE][Group_%d]qempty = 0x%x\n",
				 grpsel, qempty.qempty);
	}

	return MACSUCCESS;
}

u8 chk_dle_dfi_valid(struct mac_ax_adapter *adapter, u32 dbg_sel)
{
	if ((check_mac_en(adapter, 0, MAC_AX_DMAC_SEL) == MACSUCCESS) &&
	    dbg_sel <= MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QEMPTY)
		return 1;

	return 0;
}

u32 dle_dfi_dump(struct mac_ax_adapter *adapter, u32 sel)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_dle_dfi_info *info;
	struct dle_dfi_ctrl_t ctrl;
	u32 ret, i;

	ret = p_ops->dle_dfi_sel(adapter, &info, &ctrl.target, sel);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] dle dfi sel %d %d\n", sel, ret);
		return ret;
	}

	if (sel <= MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_QEMPTY) {
		ctrl.type = DLE_CTRL_TYPE_WDE;
	} else if (sel >= MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_FREEPG &&
		 sel <= MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QEMPTY) {
		ctrl.type = DLE_CTRL_TYPE_PLE;
	} else {
		PLTFM_MSG_ERR("[ERR]unknown dle dfi sel-2 %d\n", sel);
		return MACFUNCINPUT;
	}

	switch (sel) {
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_FREEPG:
		PLTFM_MSG_ALWAYS("WDE_BUFMGN_FREEPG\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_QUOTA:
		PLTFM_MSG_ALWAYS("WDE_BUFMGN_QUOTA\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_PAGELLT:
		PLTFM_MSG_ALWAYS("WDE_BUFMGN_PAGELLT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_PKTINFO:
		PLTFM_MSG_ALWAYS("WDE_BUFMGN_PKTINFO\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_PREPKT:
		PLTFM_MSG_ALWAYS("WDE_QUEMGN_PREPKT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_NXTPKT:
		PLTFM_MSG_ALWAYS("WDE_QUEMGN_NXTPKT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_QLNKTBL:
		PLTFM_MSG_ALWAYS("WDE_QUEMGN_QLNKTBL\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_QEMPTY:
		PLTFM_MSG_ALWAYS("WDE_QUEMGN_QEMPTY\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_FREEPG:
		PLTFM_MSG_ALWAYS("PLE_BUFMGN_FREEPG\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_QUOTA:
		PLTFM_MSG_ALWAYS("PLE_BUFMGN_QUOTA\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_PAGELLT:
		PLTFM_MSG_ALWAYS("PLE_BUFMGN_PAGELLT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_PKTINFO:
		PLTFM_MSG_ALWAYS("PLE_BUFMGN_PKTINFO\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_PREPKT:
		PLTFM_MSG_ALWAYS("PLE_QUEMGN_PREPKT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_NXTPKT:
		PLTFM_MSG_ALWAYS("PLE_QUEMGN_NXTPKT\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QLNKTBL:
		PLTFM_MSG_ALWAYS("PLE_QUEMGN_QLNKTBL\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QEMPTY:
		PLTFM_MSG_ALWAYS("PLE_QUEMGN_QEMPTY\n");
		break;
	default:
		break;
	}

	for (i = info->srt; i <= info->end; i += info->inc_num) {
		ctrl.addr = i;
		ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]dle dfi ctrl %d\n", ret);
			return ret;
		}
		PLTFM_MSG_ALWAYS("trg %d addr 0x%X: 0x%X\n",
				 ctrl.target, ctrl.addr, ctrl.out_data);
	}

	return MACSUCCESS;
}

u32 tx_cnt_dump(struct mac_ax_adapter *adapter, u8 band, u32 loop_num)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg, val32, i;
	u8 idx;
	u32 prev_cnt[TMAC_TX_CNT_NUM];

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[WARN] check cmac en %d\n", ret);
		return ret;
	}

	PLTFM_MEMSET(prev_cnt, 0, sizeof(prev_cnt));

	reg = band == MAC_AX_BAND_1 ? R_AX_TX_PPDU_CNT_C1 : R_AX_TX_PPDU_CNT;
	for (i = 0; i < loop_num; i++) {
		for (idx = 0; idx < TMAC_TX_CNT_NUM; idx++) {
			MAC_REG_W8(reg, idx);
			PLTFM_DELAY_US(TRX_CNT_READ_DLY_US);
			val32 = GET_FIELD(MAC_REG_R32(reg), B_AX_TX_PPDU_CNT);
			if (i == 0 || val32 != prev_cnt[idx])
				PLTFM_MSG_ALWAYS("B%d TX_PPDU_CNT[%d %s]-%d=0x%x\n",
						 band, idx, tx_cnt_type_g6[idx], i, val32);
			prev_cnt[idx] = val32;
		}
		PLTFM_DELAY_US(TRX_CNT_REPT_DLY_US);
	}

	return MACSUCCESS;
}

u32 rx_cnt_dump(struct mac_ax_adapter *adapter, u8 band, u32 loop_num)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg, val32, i;
	u8 idx;
	u32 prev_cnt[RMAC_RX_CNT_NUM];

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[WARN] check cmac en %d\n", ret);
		return ret;
	}

	PLTFM_MEMSET(prev_cnt, 0, sizeof(prev_cnt));

	reg = band == MAC_AX_BAND_1 ?
		      R_AX_RX_DBG_CNT_SEL_C1 : R_AX_RX_DBG_CNT_SEL;
	for (i = 0; i < loop_num; i++) {
		for (idx = 0; idx < RMAC_RX_CNT_NUM; idx++) {
			MAC_REG_W8(reg, idx);
			PLTFM_DELAY_US(TRX_CNT_READ_DLY_US);
			val32 = GET_FIELD(MAC_REG_R32(reg), B_AX_RX_DBG_CNT);
			if (i == 0 || val32 != prev_cnt[idx])
				PLTFM_MSG_ALWAYS("B%d RX_CNT[%d %s]-%d=0x%x\n",
						 band, idx, rx_cnt_type[idx],
						 i, val32);
			prev_cnt[idx] = val32;
		}
		PLTFM_DELAY_US(TRX_CNT_REPT_DLY_US);
	}

	return MACSUCCESS;
}

static u32 mac_dle_status_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY0=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY0));
	PLTFM_MSG_ALWAYS("R_AX_DLE_EMPTY1=0x%x\n",
			 MAC_REG_R32(R_AX_DLE_EMPTY1));

	return MACSUCCESS;
}

static u32 mac_hci_flow_ctrl_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		PLTFM_MSG_ALWAYS("R_AX_HDP_DBG_INFO_4=0x%x\n",
				 MAC_REG_R32(R_AX_HDP_DBG_INFO_4));
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		PLTFM_MSG_ALWAYS("R_AX_HDP_DBG_INFO_4=0x%x\n",
				 MAC_REG_R32(R_AX_HDP_DBG_INFO_4_V1));
	}
#endif

	return MACSUCCESS;
}

static u32 mac_quota_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	/* quota */
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		PLTFM_MSG_ALWAYS("R_AX_ACH0_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH0_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH1_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH1_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH2_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH2_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH3_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH3_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH4_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH4_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH5_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH5_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH6_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH6_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_ACH7_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_ACH7_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_CH8_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_CH8_PAGE_INFO));
		PLTFM_MSG_ALWAYS("R_AX_CH9_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_CH9_PAGE_INFO));
#if MAC_AX_8852A_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			PLTFM_MSG_ALWAYS("R_AX_CH10_PAGE_INFO=0x%x\n",
					 MAC_REG_R32(R_AX_CH10_PAGE_INFO));
			PLTFM_MSG_ALWAYS("R_AX_CH11_PAGE_INFO=0x%x\n",
					 MAC_REG_R32(R_AX_CH11_PAGE_INFO));
		}
#endif
		PLTFM_MSG_ALWAYS("R_AX_CH12_PAGE_INFO=0x%x\n",
				 MAC_REG_R32(R_AX_CH12_PAGE_INFO));
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		PLTFM_MSG_ALWAYS("R_AX_ACH0_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH0_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH1_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH1_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH2_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH2_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH3_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH3_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH4_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH4_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH5_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH5_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH6_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH6_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_ACH7_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_ACH7_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_CH8_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_CH8_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_CH9_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_CH9_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_CH10_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_CH10_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_CH11_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_CH11_PAGE_INFO_V1));
		PLTFM_MSG_ALWAYS("R_AX_CH12_PAGE_INFO_V1=0x%x\n",
				 MAC_REG_R32(R_AX_CH12_PAGE_INFO_V1));
	}
#endif
	return MACSUCCESS;
}

static u32 mac_tx_status_dump(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 idx = 0, ret = 0, cmac0_en = 0, cmac1_en = 0;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ALWAYS("[WARN] check dmac en %d\n", ret);
		return ret;
	}

	/* Dispatcher */
	PLTFM_MSG_ALWAYS("R_AX_DMAC_MACID_DROP_0=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_MACID_DROP_0));
	PLTFM_MSG_ALWAYS("R_AX_DMAC_MACID_DROP_1=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_MACID_DROP_1));
	PLTFM_MSG_ALWAYS("R_AX_DMAC_MACID_DROP_2=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_MACID_DROP_2));
	PLTFM_MSG_ALWAYS("R_AX_DMAC_MACID_DROP_3=0x%x\n",
			 MAC_REG_R32(R_AX_DMAC_MACID_DROP_3));

#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
	cmac0_en = check_mac_en(adapter, 0, MAC_AX_CMAC_SEL);
	if (cmac0_en == MACSUCCESS) {
		/* MGQ */
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_0=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_0));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_1=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_1));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_2=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_2));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_3=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_3));
		/* HIQ */
		PLTFM_MSG_ALWAYS("R_AX_MBSSID_DROP_0=0x%x\n",
				 MAC_REG_R32(R_AX_MBSSID_DROP_0));
		/* lifetime, PTCL TX mode */
		PLTFM_MSG_ALWAYS("R_AX_PTCL_COMMON_SETTING_0=0x%x\n",
				 MAC_REG_R32(R_AX_PTCL_COMMON_SETTING_0));

		/* MAC response abort counter */
		PLTFM_MSG_ALWAYS("R_AX_RESP_TX_NAV_ABORT_COUNTER=0x%x\n",
				 MAC_REG_R32(R_AX_RESP_TX_NAV_ABORT_COUNTER));
		PLTFM_MSG_ALWAYS("R_AX_RESP_TX_CCA_ABORT_COUNTER=0x%x\n",
				 MAC_REG_R32(R_AX_RESP_TX_CCA_ABORT_COUNTER));
		PLTFM_MSG_ALWAYS("R_AX_TRXPTCL_RESP_TX_ABORT_COUNTER=0x%x\n",
				 MAC_REG_R32(R_AX_TRXPTCL_RESP_TX_ABORT_COUNTER));
	}
	cmac1_en = check_mac_en(adapter, 1, MAC_AX_CMAC_SEL);
	if (cmac1_en == MACSUCCESS) {
		/* MGQ */
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_0_C1=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_0_C1));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_1_C1=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_1_C1));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_2_C1=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_2_C1));
		PLTFM_MSG_ALWAYS("R_AX_CMAC_MACID_DROP_3_C1=0x%x\n",
				 MAC_REG_R32(R_AX_CMAC_MACID_DROP_3_C1));
		/* HIQ */
		PLTFM_MSG_ALWAYS("R_AX_MBSSID_DROP_0_C1=0x%x\n",
				 MAC_REG_R32(R_AX_MBSSID_DROP_0_C1));
		/* lifetime, PTCL TX mode */
		PLTFM_MSG_ALWAYS("R_AX_PTCL_COMMON_SETTING_0_C1=0x%x\n",
				 MAC_REG_R32(R_AX_PTCL_COMMON_SETTING_0_C1));

		/* MAC response abort counter */
		PLTFM_MSG_ALWAYS("R_AX_RESP_TX_NAV_ABORT_COUNTER_C1=0x%x\n",
				 MAC_REG_R32(R_AX_RESP_TX_NAV_ABORT_COUNTER_C1));
		PLTFM_MSG_ALWAYS("R_AX_RESP_TX_CCA_ABORT_COUNTER_C1=0x%x\n",
				 MAC_REG_R32(R_AX_RESP_TX_CCA_ABORT_COUNTER_C1));
		PLTFM_MSG_ALWAYS("R_AX_TRXPTCL_RESP_TX_ABORT_COUNTER_C1=0x%x\n",
				 MAC_REG_R32(R_AX_TRXPTCL_RESP_TX_ABORT_COUNTER_C1));
	}
#endif

	/* Sleep */
	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_0=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_0));
	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_1=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_1));
	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_2=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_2));
	PLTFM_MSG_ALWAYS("R_AX_MACID_SLEEP_3=0x%x\n",
			 MAC_REG_R32(R_AX_MACID_SLEEP_3));

	/* Pause */
	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_0=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_0));
	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_1=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_1));
	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_2=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_2));
	PLTFM_MSG_ALWAYS("R_AX_SS_MACID_PAUSE_3=0x%x\n",
			 MAC_REG_R32(R_AX_SS_MACID_PAUSE_3));

	if (cmac0_en == MACSUCCESS) {
		/* Scheduler Tx_en */
		PLTFM_MSG_ALWAYS("R_AX_CTN_TXEN=0x%x\n",
				 MAC_REG_R32(R_AX_CTN_TXEN));
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ALWAYS("R_AX_CTN_DRV_TXEN=0x%x\n",
					 MAC_REG_R32(R_AX_CTN_DRV_TXEN));
		}
#endif
		/* Loopback mode */
		PLTFM_MSG_ALWAYS("R_AX_MAC_LOOPBACK=0x%x\n",
				 MAC_REG_R32(R_AX_MAC_LOOPBACK));
	}

	if (cmac1_en == MACSUCCESS) {
		/* Scheduler Tx_en */
		PLTFM_MSG_ALWAYS("R_AX_CTN_TXEN_C1=0x%x\n",
				 MAC_REG_R32(R_AX_CTN_TXEN_C1));
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			PLTFM_MSG_ALWAYS("R_AX_CTN_DRV_TXEN_C1=0x%x\n",
					 MAC_REG_R32(R_AX_CTN_DRV_TXEN_C1));
		}
#endif
		/* Loopback mode */
		PLTFM_MSG_ALWAYS("R_AX_MAC_LOOPBACK_C1=0x%x\n",
				 MAC_REG_R32(R_AX_MAC_LOOPBACK_C1));
	}

	/* per MACID WD empty */
	for (idx = 0; idx < 4; idx++) {
		MAC_REG_W32(R_AX_WDE_Q_STATUS_CFG, idx);
		PLTFM_MSG_ALWAYS("R_AX_WDE_Q_STATUS_VAL=0x%x\n",
				 MAC_REG_R32(R_AX_WDE_Q_STATUS_VAL));
	}

	mac_quota_dump(adapter);

	/* Polluted */
	PLTFM_MSG_ALWAYS("R_AX_CCA_CONTROL=0x%x\n",
			 MAC_REG_R32(R_AX_CCA_CONTROL));
	PLTFM_MSG_ALWAYS("R_AX_BT_PLT=0x%x\n",
			 MAC_REG_R32(R_AX_BT_PLT));

	/* FW Tx report counter */
	PLTFM_MSG_ALWAYS("R_AX_UDM0=0x%x\n",
			 MAC_REG_R32(R_AX_UDM0));

	pltfm_dbg_dump(adapter);

	/* FW CMAC error counter */
	if (mac_io_chk_access(adapter, R_AX_SER_DBG_INFO) == MACSUCCESS)
		PLTFM_MSG_ALWAYS("R_AX_SER_DBG_INFO = 0x%x\n",
				 MAC_REG_R32(R_AX_SER_DBG_INFO));
	PLTFM_DELAY_US(MAC_DBG_DUMP_DLY_US);

	return MACSUCCESS;
}

static u32 ptcl_status_dump(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, reg, val32;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[WARN] check cmac en %d\n", ret);
		return ret;
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_PTCL_TX_CTN_SEL_C1 : R_AX_PTCL_TX_CTN_SEL;
		val32 = MAC_REG_R32(reg);
		PLTFM_MSG_ALWAYS("B%d PTCL_TX_CTN_SEL=0x%x\n", band, val32);
#endif
	} else {
		PLTFM_MSG_ERR("[WARN] unknown chip for ptcl_status dump\n");
		return MACCHIPID;
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_PTCL_TX_MACID_0_C1 : R_AX_PTCL_TX_MACID_0;
		val32 = MAC_REG_R32(reg);
		PLTFM_MSG_ALWAYS("B%d PTCL_TX_MACID_0=0x%x\n", band, val32);
#endif
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		   is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		reg = band == MAC_AX_BAND_1 ?
		      R_AX_PTCL_TX_MACID_0_V1_C1 : R_AX_PTCL_TX_MACID_0_V1;
		val32 = MAC_REG_R32(reg);
		PLTFM_MSG_ALWAYS("B%d PTCL_TX_MACID_0=0x%x\n", band, val32);
#endif
	} else {
		PLTFM_MSG_ERR("[WARN] unknown chip for ptcl_status dump\n");
		return MACCHIPID;
	}

	return MACSUCCESS;
}

u32 mac_tx_flow_dbg(struct mac_ax_adapter *adapter)
{
	u32 ret, idx = 0;
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	u32 i, val32;
#endif

	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	for (idx = 0; idx < TX_FLOW_DMP_NUM; idx++) {
		/* commom check */
		ret = mac_tx_status_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]common check %d\n", ret);

		/* dump TMAC tr counter */
		ret = tx_cnt_dump(adapter, MAC_AX_BAND_0,
				  TXFLOW_TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 tx cnt dump err %d\n", ret);

		ret = tx_cnt_dump(adapter, MAC_AX_BAND_1,
				  TXFLOW_TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 tx cnt dump err %d\n", ret);

		ret = p_ops->crit_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("crit dbg dump %d\n", ret);

		/* dump PTCL TX status */
		ret = ptcl_status_dump(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 ptcl tx status dump err %d\n", ret);
		ret = ptcl_status_dump(adapter, MAC_AX_BAND_1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 ptcl tx status dump err %d\n", ret);

		/* PTCL Debug port */
		ret = p_ops->tx_flow_ptcl_dbg_port(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 ptcl dbg dump err %d\n", ret);

		ret = p_ops->tx_flow_ptcl_dbg_port(adapter, MAC_AX_BAND_1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 ptcl dbg dump err %d\n", ret);

		/* check dle status */
		ret = mac_dle_status_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN] dle status err %d\n", ret);

		/* Sch Debug port */
		ret = p_ops->tx_flow_sch_dbg_port(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 sch dbg err %d\n", ret);

		ret = p_ops->tx_flow_sch_dbg_port(adapter, MAC_AX_BAND_1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 sch dbg dump err %d\n", ret);

		/* HCI flow control */
		mac_hci_flow_ctrl_dump(adapter);

		/* zero delimiter */
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			PLTFM_MSG_ALWAYS("R_AX_DEBUG_ZLD_COUNTER_U0_U1=0x%x\n",
					 MAC_REG_R32(R_AX_DEBUG_ZLD_COUNTER_U0_U1));
			PLTFM_MSG_ALWAYS("R_AX_DEBUG_ZLD_COUNTER_U2_U3=0x%x\n",
					 MAC_REG_R32(R_AX_DEBUG_ZLD_COUNTER_U2_U3));
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			for (i = 0; i <= 3; i++) {
				val32 = MAC_REG_R32(R_AX_MACTX_DBG_SEL_CNT);
				val32 = SET_CLR_WORD(val32, i, B_AX_DBG_USER_SEL);
				MAC_REG_W32(R_AX_MACTX_DBG_SEL_CNT, val32);
				PLTFM_MSG_ALWAYS("R_AX_MACTX_ZLD_CNT %d=0x%x\n", i,
						 MAC_REG_R32(R_AX_MACTX_ZLD_CNT));
			}
		}
#endif

		PLTFM_MSG_ALWAYS("R_AX_PREBKF_CFG_0=0x%x\n",
				 MAC_REG_R32(R_AX_PREBKF_CFG_0));
	}

	return MACSUCCESS;
}

void mac_dbg_status_dump(struct mac_ax_adapter *adapter,
			 struct mac_ax_dbgpkg *val, struct mac_ax_dbgpkg_en *en)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret, i;
	u32 ret_dmac = MACSUCCESS;
	u8 cmac_allow;

	adapter->sm.l2_st = MAC_AX_L2_DIS;
	ret = p_ops->crit_dbg_dump(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("crit dbg dump %d\n", ret);
		return;
	}

	ret = fw_st_dbg_dump(adapter);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("fw st dump %d\n", ret);

	if (en->dmac_dbg) {
		ret_dmac = p_ops->dmac_dbg_dump(adapter);
		if (ret_dmac != MACSUCCESS)
			PLTFM_MSG_ERR("dmac dbg dump %d\n", ret_dmac);
	}

	cmac_allow = en->cmac_dbg && ret_dmac == MACSUCCESS ? 1 : 0;
	if (cmac_allow) {
		ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n",
				      MAC_AX_BAND_0, ret);

		ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n",
				      MAC_AX_BAND_1, ret);
	}

	if (en->mac_dbg_port) {
		for (i = MAC_AX_DBG_PORT_SEL_PTCL_C0;
		     i < MAC_AX_DBG_PORT_SEL_LAST; i++) {
			if (p_ops->is_dbg_port_not_valid(adapter, i))
				continue;
			ret = dbg_port_dump(adapter, i);
			if (ret == MACDBGPORTSEL) {
				PLTFM_MSG_WARN("no dbg port sel %d\n", i);
				continue;
			} else if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("dbg port %d dump %d\n", i, ret);
				break;
			}
		}
	}

	if (cmac_allow) {
		/* 2nd dump trx counter after dbg port dump */
		ret = tx_cnt_dump(adapter, MAC_AX_BAND_0, TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 tx cnt dump err %d\n", ret);
		ret = rx_cnt_dump(adapter, MAC_AX_BAND_0, TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B0 rx cnt dump err %d\n", ret);

		ret = tx_cnt_dump(adapter, MAC_AX_BAND_1, TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 tx cnt dump err %d\n", ret);
		ret = rx_cnt_dump(adapter, MAC_AX_BAND_1, TRX_CNT_REPT_CNT);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("[WARN]B1 rx cnt dump err %d\n", ret);
	}

	if (en->dle_dbg && ret_dmac == MACSUCCESS) {
		ret = dle_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("dle dbg dump %d\n", ret);

		for (i = MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_FREEPG;
		     i < MAC_AX_DLE_DFI_SEL_LAST; i++) {
			if (!chk_dle_dfi_valid(adapter, i))
				continue;
			ret = dle_dfi_dump(adapter, i);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("dle dfi %d dump %d\n", i, ret);
				break;
			}
		}
	}

	if (en->plersvd_dbg) {
		ret = rsvd_ple_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("rsvd dle dump %d\n", ret);
	}

	if (en->ss_dbg && ret_dmac == MACSUCCESS) {
		ret = ss_dbgpkg(adapter, val);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ALWAYS("ss dbgpkg %d\n", ret);
	}

	ret = dbgport_hw_dump(adapter, &en->dp_hw_en);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("dbgport hw dump %d\n", ret);

	PLTFM_MSG_ALWAYS("access H2CREG before MAC init: %d\n",
			 adapter->stats.h2c_reg_uninit);
	PLTFM_MSG_ALWAYS("access C2HREG before MAC init: %d\n",
			 adapter->stats.c2h_reg_uninit);
	PLTFM_MSG_ALWAYS("access H2CPKT before MAC init: %d\n",
			 adapter->stats.h2c_pkt_uninit);

	adapter->sm.l2_st = MAC_AX_L2_EN;

	if (en->tx_flow_dbg) {
		ret = mac_tx_flow_dbg(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("tx flow dbg %d\n", ret);
	}
}

u32 mac_sram_dbg_write(struct mac_ax_adapter *adapter, u32 offset,
		       u32 val, enum mac_ax_sram_dbg_sel sel)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_base;

	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees\n");
		return MACIOERRINSEC;
	}

	switch (sel) {
	case CPU_LOCAL_SEL:
		reg_base = CPU_LOCAL_BASE_ADDR + offset;
		break;
	case WCPU_DATA_SEL:
		reg_base = WCPU_DATA_BASE_ADDR + offset;
		break;
	case AXIDMA_SEL:
		reg_base = AXIDMA_BASE_ADDR + offset;
		break;
	case STA_SCHED_SEL:
		reg_base = STA_SCHED_BASE_ADDR + offset;
		break;
	case RXPLD_FLTR_CAM_SEL:
		reg_base = RXPLD_FLTR_CAM_BASE_ADDR + offset;
		break;
	case SEC_CAM_SEL:
		reg_base = SEC_CAM_BASE_ADDR + offset;
		break;
	case WOW_CAM_SEL:
		reg_base = WOW_CAM_BASE_ADDR + offset;
		break;
	case CMAC_TBL_SEL:
		reg_base = CMAC_TBL_BASE_ADDR + offset;
		break;
	case ADDR_CAM_SEL:
		reg_base = ADDR_CAM_BASE_ADDR + offset;
		break;
	case BSSID_CAM_SEL:
		reg_base = BSSID_CAM_BASE_ADDR + offset;
		break;
	case BA_CAM_SEL:
		reg_base = BA_CAM_BASE_ADDR + offset;
		break;
	case BCN_IE_CAM0_SEL:
		reg_base = BCN_IE_CAM0_BASE_ADDR + offset;
		break;
	case SHARED_BUF_SEL:
		reg_base = SHARED_BUF_BASE_ADDR + offset;
		break;
	case DMAC_TBL_SEL:
		reg_base = DMAC_TBL_BASE_ADDR + offset;
		break;
	case SHCUT_MACHDR_SEL:
		reg_base = SHCUT_MACHDR_BASE_ADDR + offset;
		break;
	case BCN_IE_CAM1_SEL:
		reg_base = BCN_IE_CAM1_BASE_ADDR + offset;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] sel %d", sel);
		return MACNOITEM;
	}

	//PLTFM_MSG_TRACE("%s ind access sel %d start\n", __func__, sel);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, reg_base);
	MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY, val);
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	//PLTFM_MSG_TRACE("%s ind access sel %d end\n", __func__, sel);

	return MACSUCCESS;
}

u32 mac_sram_dbg_read(struct mac_ax_adapter *adapter, u32 offset,
		      enum mac_ax_sram_dbg_sel sel)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_base, ret;

	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees\n");
		return MACIOERRINSEC;
	}

	switch (sel) {
	case CPU_LOCAL_SEL:
		reg_base = CPU_LOCAL_BASE_ADDR + offset;
		break;
	case WCPU_DATA_SEL:
		reg_base = WCPU_DATA_BASE_ADDR + offset;
		break;
	case AXIDMA_SEL:
		reg_base = AXIDMA_BASE_ADDR + offset;
		break;
	case STA_SCHED_SEL:
		reg_base = STA_SCHED_BASE_ADDR + offset;
		break;
	case RXPLD_FLTR_CAM_SEL:
		reg_base = RXPLD_FLTR_CAM_BASE_ADDR + offset;
		break;
	case SEC_CAM_SEL:
		reg_base = SEC_CAM_BASE_ADDR + offset;
		break;
	case WOW_CAM_SEL:
		reg_base = WOW_CAM_BASE_ADDR + offset;
		break;
	case CMAC_TBL_SEL:
		reg_base = CMAC_TBL_BASE_ADDR + offset;
		break;
	case ADDR_CAM_SEL:
		reg_base = ADDR_CAM_BASE_ADDR + offset;
		break;
	case BSSID_CAM_SEL:
		reg_base = BSSID_CAM_BASE_ADDR + offset;
		break;
	case BA_CAM_SEL:
		reg_base = BA_CAM_BASE_ADDR + offset;
		break;
	case BCN_IE_CAM0_SEL:
		reg_base = BCN_IE_CAM0_BASE_ADDR + offset;
		break;
	case SHARED_BUF_SEL:
		reg_base = SHARED_BUF_BASE_ADDR + offset;
		break;
	case DMAC_TBL_SEL:
		reg_base = DMAC_TBL_BASE_ADDR + offset;
		break;
	case SHCUT_MACHDR_SEL:
		reg_base = SHCUT_MACHDR_BASE_ADDR + offset;
		break;
	case BCN_IE_CAM1_SEL:
		reg_base = BCN_IE_CAM1_BASE_ADDR + offset;
		break;
	case WD_PAGE_SEL:
		reg_base = WD_PAGE_BASE_ADDR + offset;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] sel %d", sel);
		return MACNOITEM;
	}

	//PLTFM_MSG_TRACE("%s ind access sel %d start\n", __func__, sel);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, reg_base);
	ret = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY);
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	//PLTFM_MSG_TRACE("%s ind access sel %d end\n", __func__, sel);

	return ret;
}

static u32 get_rx_idx(struct mac_ax_adapter *adapter,
		      struct mac_ax_rx_cnt *rxcnt, u8 *idx)
{
	u8 rx_type[MAC_AX_RX_CNT_TYPE_NUM][MAC_AX_RX_PPDU_MAX] = {
		MAC_AX_RXCRC_OK_IDX, MAC_AX_RXCRC_FAIL_IDX,
		 MAC_AX_RXFA_IDX, MAC_AX_RXPPDU_IDX};
	u8 type = rxcnt->type;

	switch (type) {
	case MAC_AX_RX_CRC_OK:
	case MAC_AX_RX_CRC_FAIL:
	case MAC_AX_RX_FA:
	case MAC_AX_RX_PPDU:
		PLTFM_MEMCPY(idx, rx_type[type], MAC_AX_RX_PPDU_MAX);
		break;
	case MAC_AX_RX_IDX:
		PLTFM_MEMSET(idx, MAC_AX_RX_CNT_IDX_MAX, MAC_AX_RX_PPDU_MAX);
		idx[0] = rxcnt->idx;
		break;
	default:
		PLTFM_MSG_ERR("Wrong RX cnt type\n");
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_rx_cnt(struct mac_ax_adapter *adapter,
	       struct mac_ax_rx_cnt *rxcnt)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;
	u32 reg = rxcnt->band ? R_AX_RX_DBG_CNT_SEL_C1 : R_AX_RX_DBG_CNT_SEL;
	u32 clk = rxcnt->band ? R_AX_RXGCK_CTRL_C1 : R_AX_RXGCK_CTRL;
	u8 i;
	u8 idx[MAC_AX_RX_PPDU_MAX] = {MAC_AX_RX_CNT_IDX_MAX};
	u16 *buf = rxcnt->buf;
	u8 val;

	switch (rxcnt->op) {
	case MAC_AX_RXCNT_R:
		if (!buf) {
			PLTFM_MSG_ERR("The rx cnt buffer is NULL\n");
			return MACNPTR;
		}

		ret = get_rx_idx(adapter, rxcnt, idx);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("Get RX index fail\n");
			return ret;
		}

		for (i = 0; i < MAC_AX_RX_PPDU_MAX; i++) {
			if (idx[i] ==  MAC_AX_RX_CNT_IDX_MAX) {
				buf[i] = 0;
				continue;
			}
			MAC_REG_W8(reg, idx[i]);
			buf[i] = MAC_REG_R16(reg + 2);
		}
		break;
	case MAC_AX_RXCNT_RST_ALL:
		val = MAC_REG_R8(clk);
		MAC_REG_W8(clk, val | BIT(0));
		MAC_REG_W8(reg + 1, BIT(0));
		MAC_REG_W8(clk, val);
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_dump_fw_rsvd_ple(struct mac_ax_adapter *adapter, u8 **buf)
{
	u32 ret = MACSUCCESS;
	u32 addr;
	*buf = NULL;

	if (adapter->mac_pwr_info.pwr_seq_proc ||
	    adapter->sm.pwr != MAC_AX_PWR_ON ||
	    adapter->mac_pwr_info.pwr_in_lps ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_RECV ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_RECV_DONE ||
	    adapter->sm.fw_rst == MAC_AX_FW_RESET_PROCESS)
		return MACPWRSTAT;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		addr = RSVD_PLE_OFST_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		addr = RSVD_PLE_OFST_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		addr = RSVD_PLE_OFST_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		addr = RSVD_PLE_OFST_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		addr = RSVD_PLE_OFST_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		addr = RSVD_PLE_OFST_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		addr = RSVD_PLE_OFST_8852D;
	} else {
		PLTFM_MSG_ERR("[ERR]unknown chip id\n");
		return MACCHIPID;
	}

	*buf = (u8 *)PLTFM_MALLOC(FW_RSVD_PLE_SIZE);
	if (!*buf)
		return MACBUFALLOC;

	ret = __dump_mac_mem(adapter, MAC_AX_MEM_SHARED_BUF, addr,
			     *buf, FW_RSVD_PLE_SIZE, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("Dump fw rsvd ple %d\n", ret);
		PLTFM_FREE(*buf, FW_RSVD_PLE_SIZE);
	}

	ret = fw_backtrace_dump(adapter);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("fw backtrace dump %d\n", ret);

	return ret;
}

void mac_dump_ple_dbg_page(struct mac_ax_adapter *adapter, u8 page_num)
{
	u32 ret = MACSUCCESS;
	u32 addr = 0;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		addr = RSVD_PLE_OFST_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		addr = RSVD_PLE_OFST_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		addr = RSVD_PLE_OFST_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		addr = RSVD_PLE_OFST_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		addr = RSVD_PLE_OFST_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		addr = RSVD_PLE_OFST_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		addr = RSVD_PLE_OFST_8852D;
	} else {
		PLTFM_MSG_ERR("unknown chip id\n");
		return;
	}

	addr = addr + RSVD_PLE_OFST_DBG_START + (page_num * FW_RSVD_PLE_DBG_SIZE);
	PLTFM_MSG_ERR("Dump fw ple dbg page %d:\n", page_num);
	ret = __dump_mac_mem(adapter, MAC_AX_MEM_SHARED_BUF, addr,
			     NULL, FW_RSVD_PLE_DBG_SIZE, 0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("Dump fw ple dbg page fail: %d\n", ret);
		return;
	}
}

u32 mac_fw_dbg_dump(struct mac_ax_adapter *adapter, u8 **buf, struct mac_ax_fwdbg_en *en)
{
	u32 ret = MACSUCCESS;

	if (en->status_dbg)
		fw_st_dbg_dump(adapter);

	if (en->rsv_ple_dbg)
		mac_dump_fw_rsvd_ple(adapter, buf);

	if (en->ps_dbg)
		ps_dbg_dump(adapter);

	return ret;
}

u32 mac_event_notify(struct mac_ax_adapter *adapter, enum phl_msg_evt_id id,
		     u8 band)
{
	u32 ret = MACSUCCESS, io_ret = MACSUCCESS;
	struct mac_ax_dbgpkg dbg_val;
	struct mac_ax_dbgpkg_en dbg_en;
	struct mac_ax_io_stat pcie_io_stat;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_drv_stats *status = &adapter->drv_stats;

	switch (id) {
	case MSG_EVT_DBG_SIP_REG_DUMP:
		ret = p_ops->crit_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("crit dbg dump %d\n", ret);

		ret = fw_st_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("fw st dump %d\n", ret);

		ret = p_ops->dmac_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("dmac dbg dump %d\n", ret);

		ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n",
				      MAC_AX_BAND_0, ret);

		break;
	case MSG_EVT_DBG_FULL_REG_DUMP:
		ret = mac_reg_dump(adapter, MAC_AX_REG_MAC);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("fw st dump %d\n", ret);
		break;
	case MSG_EVT_DBG_L2_DIAGNOSE:
		ret = mac_ps_pwr_state(adapter, MAC_AX_PWR_STATE_ACT_REQ,
				       MAC_AX_RPWM_REQ_PWR_STATE_ACTIVE);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("PWR_STATE_ACT_REQ fail\n");

		PLTFM_MEMSET(&dbg_en, 0, sizeof(struct mac_ax_dbgpkg_en));

		if (ret == MACSUCCESS) {
			if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
				io_ret = ops->get_io_stat(adapter,
							  &pcie_io_stat);
				if (io_ret == MACSUCCESS &&
				    pcie_io_stat.to_flag == 0) {
					dbg_en.dle_dbg = 1;
					dbg_en.dmac_dbg = 1;
					dbg_en.cmac_dbg = 1;
					dbg_en.plersvd_dbg = 1;
				}
			} else {
				dbg_en.dle_dbg = 1;
				dbg_en.dmac_dbg = 1;
				dbg_en.cmac_dbg = 1;
				dbg_en.plersvd_dbg = 1;
			}
		}
		mac_dbg_status_dump(adapter, &dbg_val, &dbg_en);
		break;
	case MSG_EVT_DBG_RX_DUMP:
		ret = p_ops->crit_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("crit dbg dump %d\n", ret);

		ret = p_ops->dmac_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("dmac dbg dump %d\n", ret);

		ret = p_ops->cmac_dbg_dump(adapter, MAC_AX_BAND_0);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("cmac%d dbg dump %d\n",
				      MAC_AX_BAND_0, ret);

		ret = dle_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("dle dbg dump %d\n", ret);
		break;
	case MSG_EVT_DBG_TX_DUMP:
		ret = p_ops->tx_dbg_dump(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("tx dbg dump %d\n", ret);
		break;
	case MSG_EVT_DATA_PATH_START:
		status->rx_ok = 1;
		break;
	case MSG_EVT_DATA_PATH_STOP:
		status->rx_ok = 0;
		break;
	case MSG_EVT_SURPRISE_REMOVE:
		status->drv_rm = 1;
		break;
	default:
		return MACNOITEM;
	}

	return ret;
}

u32 mac_fw_dbg_dle_cfg(struct mac_ax_adapter *adapter, bool lock)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_general_pkt *write_ptr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_general_pkt_ids));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	write_ptr = (struct fwcmd_general_pkt *)buf;
	write_ptr->dword0 =
	cpu_to_le32((lock ? FWCMD_H2C_FW_DBGREG_CFG_FW_DBG_LOCK : 0));
	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_INFO,
			      FWCMD_H2C_FUNC_FW_DBGREG_CFG,
			      0, 0);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_get_fw_status(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8 = FWDL_INITIAL_STATE;
	u32 i = 0, fw_pc = 0;
	u32 val32, dbg_ctrl_bk, sys_status_bk;

	val8 = GET_FIELD(MAC_REG_R8(R_AX_WCPU_FW_CTRL), B_AX_WCPU_FWDL_STS);
	if (val8 == FWDL_WCPU_FW_INIT_RDY) {
		/* Dump FW status */
		val32 = READ_DBG_FS_REG();
		if (val32 == FS_L2ERR_IN ||
		    val32 == FS_L2ERR_CPU_IN ||
		    val32 == FS_L2ERR_HCI_IN ||
		    val32 == FS_L2ERR_ELSE_IN) {
			return MACFWRXI300;
		} else if (val32 == FS_ASSERT_IN) {
			return MACFWASSERT;
		} else if (val32 == FS_EXCEP_IN) {
			return MACFWEXCEP;
		}

		/* Dump FW program counter */
		dbg_ctrl_bk = MAC_REG_R32(R_AX_DBG_CTRL);
		sys_status_bk = MAC_REG_R32(R_AX_SYS_STATUS1);
		MAC_REG_W32(R_AX_DBG_CTRL, DBG_SEL_FW_PROG_CNTR);

		val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
		val32 = SET_CLR_WORD(val32, MAC_DBG_SEL, B_AX_SEL_0XC0);
		MAC_REG_W32(R_AX_SYS_STATUS1, val32);
		val8 = 0;
		for (i = 0; i < FW_PROG_CNTR_DMP_CNT; i++) {
			val32 = MAC_REG_R32(R_AX_DBG_PORT_SEL);
			if (fw_pc == val32)
				val8++;
			fw_pc = val32;
			PLTFM_DELAY_US(FW_PROG_CNTR_DMP_DLY_US);
		}
		if (val8 == FW_PROG_CNTR_DMP_CNT) {
			PLTFM_MSG_ALWAYS("FW PC = 0x%x\n", val32);
			return MACFWPCHANG;
		}

		MAC_REG_W32(R_AX_DBG_CTRL, dbg_ctrl_bk);
		MAC_REG_W32(R_AX_SYS_STATUS1, sys_status_bk);
	} else {
		return MACNOFW;
	}
	return MACSUCCESS;
}

u32 mac_get_ple_dbg_addr(struct mac_ax_adapter *adapter)
{
	u32 addr = RSVD_PLE_OFST_DBG_START;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		addr += RSVD_PLE_OFST_8852A;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		addr += RSVD_PLE_OFST_8852B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		addr += RSVD_PLE_OFST_8852C;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		addr += RSVD_PLE_OFST_8192XB;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		addr += RSVD_PLE_OFST_8851B;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		addr += RSVD_PLE_OFST_8851E;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		addr += RSVD_PLE_OFST_8852D;
	} else {
		PLTFM_MSG_ERR("[ERR]unknown chip id\n");
		return MACCHIPID;
	}

	return addr;
}
