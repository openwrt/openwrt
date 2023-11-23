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

#include "sounding.h"

static u32 _patch_snd_ple_modify(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u16 cr;

	if (!chk_patch_snd_ple_modify(adapter))
		return MACSUCCESS;

	cr = band ? R_AX_BFMEE_RESP_OPTION_C1 : R_AX_BFMEE_RESP_OPTION;
	val32 = MAC_REG_R32(cr);
	val32 = SET_CLR_WORD(val32, PATCH_NDP_RX_STANDBY_TIMER, B_AX_BFMEE_NDP_RX_STANDBY_TIMER);
	MAC_REG_W32(cr, val32);

	return MACSUCCESS;
}

static u32 _patch_snd_fifofull_err(struct mac_ax_adapter *adapter, u8 band)
{
#if MAC_AX_USB_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 cr, val16;

	if (!chk_patch_snd_fifofull_err(adapter))
		return MACSUCCESS;

	if (get_usb_mode(adapter) == MAC_AX_USB2) {
		cr = band ? R_AX_DLK_PROTECT_CTL_C1 : R_AX_DLK_PROTECT_CTL;
		val16 = MAC_REG_R16(cr) | B_AX_RX_DLK_RST_FSM;
		MAC_REG_W16(cr, val16);
	}
#endif
	return MACSUCCESS;
}

u32 mac_get_csi_buffer_index(struct mac_ax_adapter *adapter, u8 band,
			     u8 csi_buffer_id)
{
	u32 val32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;
	if (csi_buffer_id > CSI_MAX_BUFFER_IDX)
		return MACCSIBUFIDERR;

	val32 = MAC_REG_R32((band ? R_AX_BFMER_CSI_BUFF_IDX0_C1 :
			    R_AX_BFMER_CSI_BUFF_IDX0) + CSI_SH * csi_buffer_id);
	return val32;
}

u32 mac_set_csi_buffer_index(struct mac_ax_adapter *adapter, u8 band,
			     u8 macid, u16 csi_buffer_id, u16 buffer_idx)
{
	u32 val32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

#if MAC_AX_FW_REG_OFLD
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;
		if (csi_buffer_id > CSI_MAX_BUFFER_IDX)
			return MACCSIBUFIDERR;

		cr = (band ? R_AX_BFMER_CSI_BUFF_IDX0_C1 :
			  R_AX_BFMER_CSI_BUFF_IDX0) + (CSI_SH * csi_buffer_id);
		val32 = (buffer_idx & B_AX_MER_TXBF_CSI_BUFF_IDX0_MSK)
			 << B_AX_MER_SND_CSI_BUFF_IDX0_SH |
			 (macid & B_AX_MER_CSI_BUFF_MACID_IDX0_MSK);

		ret = MAC_REG_W32_OFLD(cr, val32, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;
	if (csi_buffer_id > CSI_MAX_BUFFER_IDX)
		return MACCSIBUFIDERR;

	val32 = (buffer_idx & B_AX_MER_TXBF_CSI_BUFF_IDX0_MSK)
		 << B_AX_MER_SND_CSI_BUFF_IDX0_SH |
		 (macid & B_AX_MER_CSI_BUFF_MACID_IDX0_MSK);

	MAC_REG_W32((band ? R_AX_BFMER_CSI_BUFF_IDX0_C1 :
		    R_AX_BFMER_CSI_BUFF_IDX0) + CSI_SH * csi_buffer_id, val32);

	return MACSUCCESS;
}

u32 mac_get_snd_sts_index(struct mac_ax_adapter *adapter, u8 band, u8 index)
{
	u32 va32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (index > SOUNDING_STS_MAX_IDX)
		return MACSNDSTSIDERR;

	va32 = MAC_REG_R16((band ? R_AX_BFMER_ASSOCIATED_SU0_C1 :
			   R_AX_BFMER_ASSOCIATED_SU0) + SND_SH * index);
	return va32;
}

u32 mac_set_snd_sts_index(struct mac_ax_adapter *adapter,  u8 band, u8 macid,
			  u8 index)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

#if MAC_AX_FW_REG_OFLD
	u16 cr, val16;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		if (index > SOUNDING_STS_MAX_IDX)
			return MACSNDSTSIDERR;

		cr = (band ? R_AX_BFMER_ASSOCIATED_SU0_C1 :
			   R_AX_BFMER_ASSOCIATED_SU0) + (SND_SH * index);
		val16 = B_AX_MER_SU_BFMEE0_EN | macid;

		ret = MAC_REG_W16_OFLD(cr, val16, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (index > SOUNDING_STS_MAX_IDX)
		return MACSNDSTSIDERR;

	MAC_REG_W16((band ? R_AX_BFMER_ASSOCIATED_SU0_C1 :
		   R_AX_BFMER_ASSOCIATED_SU0) + SND_SH * index,
		   B_AX_MER_SU_BFMEE0_EN | macid);
	return MACSUCCESS;
}

u32 mac_init_snd_mer(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

#if MAC_AX_FW_REG_OFLD
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		cr = band ? R_AX_BFMER_CTRL_0_C1 : R_AX_BFMER_CTRL_0;
		val32 = B_AX_BFMER_NDP_BFEN;
		val32 |= HT_PAYLOAD_OFFSET << B_AX_BFMER_HT_CSI_OFFSET_SH;
		val32 |= VHT_PAYLOAD_OFFSET << B_AX_BFMER_VHT_CSI_OFFSET_SH;
		val32 |= HE_PAYLOAD_OFFSET << B_AX_BFMER_HE_CSI_OFFSET_SH;

		ret = MAC_REG_W32_OFLD(cr, val32, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = B_AX_BFMER_NDP_BFEN;
	val32 |= HT_PAYLOAD_OFFSET << B_AX_BFMER_HT_CSI_OFFSET_SH;
	val32 |= VHT_PAYLOAD_OFFSET << B_AX_BFMER_VHT_CSI_OFFSET_SH;
	val32 |= HE_PAYLOAD_OFFSET << B_AX_BFMER_HE_CSI_OFFSET_SH;
	MAC_REG_W32(band ? R_AX_BFMER_CTRL_0_C1 : R_AX_BFMER_CTRL_0, val32);
	return MACSUCCESS;
}

u32 mac_init_snd_mee(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

#if MAC_AX_FW_REG_OFLD
	u32 mask;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		cr = band ? R_AX_TRXPTCL_RESP_CSI_RRSC_C1 : R_AX_TRXPTCL_RESP_CSI_RRSC;
		val32 = CSI_RRSC_BMAP;

		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = band ? R_AX_BFMEE_RESP_OPTION_C1 : R_AX_BFMEE_RESP_OPTION;
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val32 = (BFRP_RX_STANDBY_TIMER << B_AX_BFMEE_BFRP_RX_STANDBY_TIMER_SH);
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			   is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			val32 = (BFRP_RX_STANDBY_TIMER_V1 << B_AX_BFMEE_BFRP_RX_STANDBY_TIMER_SH);
		} else {
			return MACCHIPID;
		}
		val32 |= (NDP_RX_STANDBY_TIMER << B_AX_BFMEE_NDP_RX_STANDBY_TIMER_SH);
		val32 |= (B_AX_BFMEE_HT_NDPA_EN | B_AX_BFMEE_VHT_NDPA_EN |
			  B_AX_BFMEE_HE_NDPA_EN);
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
			cr = band ? R_AX_TRXPTCL_ERROR_INDICA_MASK_C1 :
				    R_AX_TRXPTCL_ERROR_INDICA_MASK;
			mask = B_AX_RMAC_CSI;
			ret = write_mac_reg_ofld(adapter, cr, mask, 0, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n",
					      __func__, cr);
				return ret;
			}
		}

		cr = band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		val32 = (B_AX_BFMEE_BFPARAM_SEL | B_AX_BFMEE_USE_NSTS |
			  B_AX_BFMEE_CSI_GID_SEL | B_AX_BFMEE_CSI_FORCE_RETE_EN);
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = band ? R_AX_CSIRPT_OPTION_C1 : R_AX_CSIRPT_OPTION;
		mask = B_AX_CSIPRT_VHTSU_AID_EN;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		mask = B_AX_CSIPRT_HESU_AID_EN;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	/*AP mode set tx gid to 63*/
	/*STA mode set tx gid to 0(default)*/
	val32 = MAC_REG_R32(band ? R_AX_BFMER_CTRL_0_C1 : R_AX_BFMER_CTRL_0);
	val32 |= B_AX_BFMER_NDP_BFEN;
	MAC_REG_W32(band ? R_AX_BFMER_CTRL_0_C1 : R_AX_BFMER_CTRL_0, val32);

	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_RRSC_C1 :
		    R_AX_TRXPTCL_RESP_CSI_RRSC, CSI_RRSC_BMAP);

#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		val32 = (BFRP_RX_STANDBY_TIMER << B_AX_BFMEE_BFRP_RX_STANDBY_TIMER_SH);
		val32 |= (NDP_RX_STANDBY_TIMER << B_AX_BFMEE_NDP_RX_STANDBY_TIMER_SH);
		val32 |= (B_AX_BFMEE_HT_NDPA_EN | B_AX_BFMEE_VHT_NDPA_EN |
			  B_AX_BFMEE_HE_NDPA_EN);
		MAC_REG_W32(band ? R_AX_BFMEE_RESP_OPTION_C1 :
				    R_AX_BFMEE_RESP_OPTION, val32);
	}
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		val32 = MAC_REG_R32(band ? R_AX_TRXPTCL_ERROR_INDICA_MASK_C1 :
				    R_AX_TRXPTCL_ERROR_INDICA_MASK);
		val32 &= (~B_AX_RMAC_CSI);
		MAC_REG_W32(band ? R_AX_TRXPTCL_ERROR_INDICA_MASK_C1 :
			    R_AX_TRXPTCL_ERROR_INDICA_MASK, val32);
		val32 = (BFRP_RX_STANDBY_TIMER_V1 << B_AX_BFMEE_CSI_RELEASE_TIMER_SH);
		val32 |= (NDP_RX_STANDBY_TIMER << B_AX_BFMEE_NDP_RX_TIMEOUT_SH);
		val32 |= (B_AX_BFMEE_HT_NDPA_EN | B_AX_BFMEE_VHT_NDPA_EN |
			  B_AX_BFMEE_HE_NDPA_EN);
		MAC_REG_W32(band ? R_AX_BFMEE_RESP_OPTION_C1 :
				    R_AX_BFMEE_RESP_OPTION, val32);
	}
#endif

	val32 = MAC_REG_R32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0);
	val32 |= (B_AX_BFMEE_BFPARAM_SEL | B_AX_BFMEE_USE_NSTS |
		  B_AX_BFMEE_CSI_GID_SEL | B_AX_BFMEE_CSI_FORCE_RETE_EN);
	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
		    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val32);

	val32 = MAC_REG_R32(band ? R_AX_CSIRPT_OPTION_C1 : R_AX_CSIRPT_OPTION);
	val32 |= (B_AX_CSIPRT_VHTSU_AID_EN | B_AX_CSIPRT_HESU_AID_EN);
	MAC_REG_W32(band ? R_AX_CSIRPT_OPTION_C1 : R_AX_CSIRPT_OPTION, val32);
	_patch_snd_ple_modify(adapter, band);
	_patch_snd_fifofull_err(adapter, band);
	return MACSUCCESS;
}

u32 mac_csi_force_rate(struct mac_ax_adapter *adapter, u8 band, u8 ht_rate,
		       u8 vht_rate, u8 he_rate)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

#if MAC_AX_FW_REG_OFLD
	u32 mask;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		cr = band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 : R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		mask = B_AX_BFMEE_CSI_FORCE_RETE_EN;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		mask = B_AX_BFMEE_BFPARAM_SEL;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = band ? R_AX_TRXPTCL_RESP_CSI_RATE_C1 : R_AX_TRXPTCL_RESP_CSI_RATE;
		mask |= B_AX_BFMEE_HT_CSI_RATE_MSK;
		mask |= B_AX_BFMEE_VHT_CSI_RATE_MSK << B_AX_BFMEE_VHT_CSI_RATE_SH;
		mask |= B_AX_BFMEE_HE_CSI_RATE_MSK << B_AX_BFMEE_HE_CSI_RATE_SH;
		val32 = ((u32)ht_rate | ((u32)vht_rate << B_AX_BFMEE_VHT_CSI_RATE_SH) |
			 ((u32)he_rate << B_AX_BFMEE_HE_CSI_RATE_SH));
		ret = write_mac_reg_ofld(adapter, cr, mask, val32, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = MAC_REG_R32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0);
	val32 |= B_AX_BFMEE_CSI_FORCE_RETE_EN | B_AX_BFMEE_BFPARAM_SEL;
	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
		    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val32);

	val32 = ((u32)ht_rate | ((u32)vht_rate << B_AX_BFMEE_VHT_CSI_RATE_SH) |
		 ((u32)he_rate << B_AX_BFMEE_HE_CSI_RATE_SH));
	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_RATE_C1 :
		    R_AX_TRXPTCL_RESP_CSI_RATE, val32);
	return MACSUCCESS;
}

u32 mac_csi_rrsc(struct mac_ax_adapter *adapter, u8 band, u32 rrsc)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

#if MAC_AX_FW_REG_OFLD
	u32 mask;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		cr = band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 : R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		mask = B_AX_BFMEE_CSI_FORCE_RETE_EN;
		ret = write_mac_reg_ofld(adapter, cr, mask, 0, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		mask = B_AX_BFMEE_BFPARAM_SEL;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = band ? R_AX_TRXPTCL_RESP_CSI_RRSC_C1 : R_AX_TRXPTCL_RESP_CSI_RRSC;
		val32 = rrsc;
		ret = MAC_REG_W32_OFLD(cr, val32, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = MAC_REG_R32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0);
	val32 |= B_AX_BFMEE_BFPARAM_SEL;
	val32 &= (~B_AX_BFMEE_CSI_FORCE_RETE_EN);
	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
		    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val32);
	MAC_REG_W32(band ? R_AX_TRXPTCL_RESP_CSI_RRSC_C1 :
		    R_AX_TRXPTCL_RESP_CSI_RRSC, rrsc);
	return MACSUCCESS;
}

u32 mac_set_mu_table(struct mac_ax_adapter *adapter,
		     struct mac_mu_table *mu_table)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
#if MAC_AX_FW_REG_OFLD
		u32 val32, ret;
		u16 cr;

		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			cr = R_AX_SS_MU_CTRL;
			val32 = mu_table->mu_score_tbl_ctrl;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_0;
			val32 = mu_table->mu_score_tbl_0;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_1;
			val32 = mu_table->mu_score_tbl_1;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_2;
			val32 = mu_table->mu_score_tbl_2;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_3;
			val32 = mu_table->mu_score_tbl_3;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_4;
			val32 = mu_table->mu_score_tbl_4;
			ret = MAC_REG_W32_OFLD(cr, val32, 0);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}

			cr = R_AX_SS_MU_TBL_5;
			val32 = mu_table->mu_score_tbl_5;
			ret = MAC_REG_W32_OFLD(cr, val32, 1);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
				return ret;
			}
			return MACSUCCESS;
		}
#endif
		MAC_REG_W32(R_AX_SS_MU_CTRL, mu_table->mu_score_tbl_ctrl);
		MAC_REG_W32(R_AX_SS_MU_TBL_0, mu_table->mu_score_tbl_0);
		MAC_REG_W32(R_AX_SS_MU_TBL_1, mu_table->mu_score_tbl_1);
		MAC_REG_W32(R_AX_SS_MU_TBL_2, mu_table->mu_score_tbl_2);
		MAC_REG_W32(R_AX_SS_MU_TBL_3, mu_table->mu_score_tbl_3);
		MAC_REG_W32(R_AX_SS_MU_TBL_4, mu_table->mu_score_tbl_4);
		MAC_REG_W32(R_AX_SS_MU_TBL_5, mu_table->mu_score_tbl_5);
		return MACSUCCESS;
	}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	//For 8852C and 8192XB,
	//the MU Score Table CR are replaced by SS DL Group Table CR(same with RU)
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		return mac_ops->ss_dl_grp_upd(adapter, mu_table->dlmu_grp_info);
	}
#endif
	return MACNOTSUP;
}

u32 mac_set_csi_para_reg(struct mac_ax_adapter *adapter,
			 struct mac_reg_csi_para *csi_para)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;
	u16 val16;

#if MAC_AX_FW_REG_OFLD
	u32 mask;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		if (chk_patch_snd_ng3_setting(adapter)) {
			if (csi_para->ng == 3)
				return MACHWNOSUP;
		}

		cr = csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
				      R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		mask = B_AX_BFMEE_BFPARAM_SEL;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		if (csi_para->portsel == 0)
			cr = csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
				      R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		else
			cr = csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_1_C1 :
				      R_AX_TRXPTCL_RESP_CSI_CTRL_1;
		val16 = SET_WORD(csi_para->nc, B_AX_BFMEE_CSIINFO0_NC) |
			SET_WORD(csi_para->nr, B_AX_BFMEE_CSIINFO0_NR) |
			SET_WORD(csi_para->ng, B_AX_BFMEE_CSIINFO0_NG) |
			SET_WORD(csi_para->cb, B_AX_BFMEE_CSIINFO0_CB) |
			SET_WORD(csi_para->cs, B_AX_BFMEE_CSIINFO0_CS) |
			(csi_para->ldpc_en ? B_AX_BFMEE_CSIINFO0_LDPC_EN : 0) |
			(csi_para->stbc_en ? B_AX_BFMEE_CSIINFO0_STBC_EN : 0);
		ret = MAC_REG_W16_OFLD(cr, val16, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, (u8)csi_para->band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (chk_patch_snd_ng3_setting(adapter)) {
		if (csi_para->ng == 3)
			return MACHWNOSUP;
	}

	val32 = MAC_REG_R32(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0);
	val32 |= B_AX_BFMEE_BFPARAM_SEL;
	MAC_REG_W32(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
		    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val32);

	val16 = SET_WORD(csi_para->nc, B_AX_BFMEE_CSIINFO0_NC) |
		SET_WORD(csi_para->nr, B_AX_BFMEE_CSIINFO0_NR) |
		SET_WORD(csi_para->ng, B_AX_BFMEE_CSIINFO0_NG) |
		SET_WORD(csi_para->cb, B_AX_BFMEE_CSIINFO0_CB) |
		SET_WORD(csi_para->cs, B_AX_BFMEE_CSIINFO0_CS) |
		(csi_para->ldpc_en ? B_AX_BFMEE_CSIINFO0_LDPC_EN : 0) |
		(csi_para->stbc_en ? B_AX_BFMEE_CSIINFO0_STBC_EN : 0);

	if (csi_para->portsel == 0)
		MAC_REG_W16(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val16);
	else
		MAC_REG_W16(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_1_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_1, val16);
	return MACSUCCESS;
}

u32 mac_set_csi_para_cctl(struct mac_ax_adapter *adapter,
			  struct mac_cctl_csi_para *csi_para)
{
	struct rtw_hal_mac_ax_cctl_info info;
	struct rtw_hal_mac_ax_cctl_info mask;
	struct mac_ax_ops *ax_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, val32;

#if MAC_AX_FW_REG_OFLD
	u32 msk;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, (u8)csi_para->band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		if (chk_patch_snd_ng3_setting(adapter)) {
			if (csi_para->ng == 3)
				return MACHWNOSUP;
		}

		cr = csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
				      R_AX_TRXPTCL_RESP_CSI_CTRL_0;
		msk = B_AX_BFMEE_BFPARAM_SEL;
		ret = write_mac_reg_ofld(adapter, cr, msk, 0, 1);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		info.nc = csi_para->nc;
		info.nr = csi_para->nr;
		info.ng = csi_para->ng;
		info.cb = csi_para->cb;
		info.cs = csi_para->cs;
		//info.csi_txbf_en = csi_para->bf_en; BB HW BUG not support
		info.csi_txbf_en = 0x0;
		info.csi_stbc_en = csi_para->stbc_en;
		info.csi_ldpc_en = csi_para->ldpc_en;
		info.csi_para_en = 1;
		info.csi_fix_rate = csi_para->rate;
		info.csi_gi_ltf = csi_para->gi_ltf;
		info.uldl = csi_para->gid_sel;
		info.csi_bw = csi_para->bw;

		PLTFM_MEMSET(&mask, 0, sizeof(mask));
		mask.nc = 0x7;
		mask.nr = 0x7;
		mask.ng = 0x3;
		mask.cb = 0x3;
		mask.cs = 0x3;
		mask.csi_txbf_en = 0x1;
		mask.csi_stbc_en = 0x1;
		mask.csi_ldpc_en = 0x1;
		mask.csi_para_en = 0x1;
		mask.csi_fix_rate = 0x1FF;
		mask.csi_gi_ltf = 0x7;
		mask.uldl = 0x1;
		mask.csi_bw = 0x3;
		ret = ax_ops->upd_cctl_info(adapter, &info, &mask, csi_para->macid, 1);
		if (ret)
			return MACCCTLWRFAIL;

		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, (u8)csi_para->band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	if (chk_patch_snd_ng3_setting(adapter)) {
		if (csi_para->ng == 3)
			return MACHWNOSUP;
	}

	val32 = MAC_REG_R32(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
			    R_AX_TRXPTCL_RESP_CSI_CTRL_0);
	val32 &= (~B_AX_BFMEE_BFPARAM_SEL);
	MAC_REG_W32(csi_para->band ? R_AX_TRXPTCL_RESP_CSI_CTRL_0_C1 :
		    R_AX_TRXPTCL_RESP_CSI_CTRL_0, val32);

	info.nc = csi_para->nc;
	info.nr = csi_para->nr;
	info.ng = csi_para->ng;
	info.cb = csi_para->cb;
	info.cs = csi_para->cs;
	//info.csi_txbf_en = csi_para->bf_en; BB HW BUG not support
	info.csi_txbf_en = 0x0;
	info.csi_stbc_en = csi_para->stbc_en;
	info.csi_ldpc_en = csi_para->ldpc_en;
	info.csi_para_en = 1;
	info.csi_fix_rate = csi_para->rate;
	info.csi_gi_ltf = csi_para->gi_ltf;
	info.uldl = csi_para->gid_sel;
	info.csi_bw = csi_para->bw;

	PLTFM_MEMSET(&mask, 0, sizeof(mask));
	mask.nc = 0x7;
	mask.nr = 0x7;
	mask.ng = 0x3;
	mask.cb = 0x3;
	mask.cs = 0x3;
	mask.csi_txbf_en = 0x1;
	mask.csi_stbc_en = 0x1;
	mask.csi_ldpc_en = 0x1;
	mask.csi_para_en = 0x1;
	mask.csi_fix_rate = 0x1FF;
	mask.csi_gi_ltf = 0x7;
	mask.uldl = 0x1;
	mask.csi_bw = 0x3;
	ret = ax_ops->upd_cctl_info(adapter, &info, &mask, csi_para->macid, 1);
	if (ret)
		return MACCCTLWRFAIL;

	return MACSUCCESS;
}

u32 mac_hw_snd_pause_release(struct mac_ax_adapter *adapter, u8 band,
			     u8 pr)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_sch_tx_en_cfg sch_cfg;
	u32 ret;

	sch_cfg.band = band;
	if (!pr)
		PLTFM_MEMSET(&sch_cfg.tx_en, 0, SCH_TX_EN_SIZE);
	else
		PLTFM_MEMSET(&sch_cfg.tx_en, 0xFF, SCH_TX_EN_SIZE);
	PLTFM_MEMSET(&sch_cfg.tx_en_mask, 0xFF, SCH_TX_EN_SIZE);
	ret = mac_ops->set_hw_value(adapter, MAC_AX_HW_SET_SCH_TXEN_CFG,
				    (void *)&sch_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("B%d pause%d sch txen cfg %d\n", band, pr, ret);
		return ret;
	}

	return MACSUCCESS;
}

u32 mac_bypass_snd_sts(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val8;

#if MAC_AX_FW_REG_OFLD
	u32 mask, ret;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		cr = R_AX_AGG_BK_0;
		mask = B_AX_DIS_SND_STS_CHECK;
		ret = write_mac_reg_ofld(adapter, cr, mask, 1, 1);

		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	val8 = MAC_REG_R8(R_AX_AGG_BK_0);
	MAC_REG_W8(R_AX_AGG_BK_0, val8 | B_AX_DIS_SND_STS_CHECK);

	return MACSUCCESS;
}

u32 mac_deinit_mee(struct mac_ax_adapter *adapter, u8 band)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, ret;

#if MAC_AX_FW_REG_OFLD
	u32 mask;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
		if (ret != MACSUCCESS)
			return ret;

		cr = band ? R_AX_BFMEE_RESP_OPTION_C1 : R_AX_BFMEE_RESP_OPTION;
		mask = (B_AX_BFMEE_HT_NDPA_EN | B_AX_BFMEE_VHT_NDPA_EN |
			B_AX_BFMEE_HE_NDPA_EN);
		ret = write_mac_reg_ofld(adapter, cr, mask, 0, 1);

		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	val32 = MAC_REG_R32(band ? R_AX_BFMEE_RESP_OPTION_C1 :
			    R_AX_BFMEE_RESP_OPTION);
	val32 &= ~(B_AX_BFMEE_HT_NDPA_EN | B_AX_BFMEE_VHT_NDPA_EN |
		  B_AX_BFMEE_HE_NDPA_EN);
	MAC_REG_W32(band ? R_AX_BFMEE_RESP_OPTION_C1 :
		    R_AX_BFMEE_RESP_OPTION, val32);

	return MACSUCCESS;
}

u32 mac_snd_sup(struct mac_ax_adapter *adapter, struct mac_bf_sup *bf_sup)
{
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		bf_sup->bf_entry_num = 16;
		bf_sup->su_buffer_num = 16;
		bf_sup->mu_buffer_num = 6;
	} else {
		return MACNOTSUP;
	}
	return MACSUCCESS;
}

u32 mac_gidpos(struct mac_ax_adapter *adapter, struct mac_gid_pos *mu_gid)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

#if MAC_AX_FW_REG_OFLD
	u32 ret, val32;
	u16 cr;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		cr = mu_gid->band ? R_AX_GID_POSITION_EN0_C1 : R_AX_GID_POSITION_EN0;
		val32 = mu_gid->gid_tab[0];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = mu_gid->band ? R_AX_GID_POSITION_EN1_C1 : R_AX_GID_POSITION_EN1;
		val32 = mu_gid->gid_tab[1];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = mu_gid->band ? R_AX_GID_POSITION0_C1 : R_AX_GID_POSITION0;
		val32 = mu_gid->user_pos[0];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = mu_gid->band ? R_AX_GID_POSITION1_C1 : R_AX_GID_POSITION1;
		val32 = mu_gid->user_pos[1];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = mu_gid->band ? R_AX_GID_POSITION2_C1 : R_AX_GID_POSITION2;
		val32 = mu_gid->user_pos[2];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}

		cr = mu_gid->band ? R_AX_GID_POSITION3_C1 : R_AX_GID_POSITION3;
		val32 = mu_gid->user_pos[3];
		ret = MAC_REG_W32_OFLD(cr, val32, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s FW_OFLD in %x\n", __func__, cr);
			return ret;
		}
		return MACSUCCESS;
	}
#endif
	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION_EN0_C1 :
		    R_AX_GID_POSITION_EN0, mu_gid->gid_tab[0]);
	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION_EN1_C1 :
		    R_AX_GID_POSITION_EN1, mu_gid->gid_tab[1]);

	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION0_C1 : R_AX_GID_POSITION0,
		    mu_gid->user_pos[0]);
	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION1_C1 : R_AX_GID_POSITION1,
		    mu_gid->user_pos[1]);
	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION2_C1 : R_AX_GID_POSITION2,
		    mu_gid->user_pos[2]);
	MAC_REG_W32(mu_gid->band ? R_AX_GID_POSITION3_C1 : R_AX_GID_POSITION3,
		    mu_gid->user_pos[3]);

	return MACSUCCESS;
}

#if MAC_AX_8852A_SUPPORT
static u32 build_snd_h2c(struct mac_ax_adapter *adapter, struct mac_ax_fwcmd_snd *snd_info)
{
	u32 ret = 0;
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_set_snd_para *h2c;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, MAX_FWCMD_SND_LEN);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMSET(buf, 0, MAX_FWCMD_SND_LEN);
	h2c = (struct fwcmd_set_snd_para *)buf;

	h2c->dword0 =
	cpu_to_le32(SET_WORD(snd_info->frexgtype,
			     FWCMD_H2C_SET_SND_PARA_FREXCH_TYPE) |
		    SET_WORD(snd_info->mode, FWCMD_H2C_SET_SND_PARA_MODE) |
		    SET_WORD(snd_info->bfrp0_user_num,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_USER_NUM) |
		    SET_WORD(snd_info->bfrp1_user_num,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP1_USER_NUM));
	h2c->dword1 =
	cpu_to_le32(SET_WORD(snd_info->macid[0], FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->macid[1], FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->macid[2], FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->macid[3], FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword2 =
	cpu_to_le32(SET_WORD(snd_info->macid[8], FWCMD_H2C_SET_SND_PARA_MACID4) |
		    SET_WORD(snd_info->macid[9], FWCMD_H2C_SET_SND_PARA_MACID5) |
		    SET_WORD(snd_info->macid[10], FWCMD_H2C_SET_SND_PARA_MACID6) |
		    SET_WORD(snd_info->macid[11], FWCMD_H2C_SET_SND_PARA_MACID7));
	h2c->dword3 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_NDPA_FRAME_CTRL) |
		    SET_WORD(snd_info->pndpa.common.duration,
			     FWCMD_H2C_SET_SND_PARA_NDPA_DURATION));
	h2c->dword4 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword5 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword6 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword7 =
	cpu_to_le32((snd_info->pndpa.snd_dialog.he ?
		     FWCMD_H2C_SET_SND_PARA_NDPA_SND_DLG_HE : 0) |
		    SET_WORD(snd_info->pndpa.snd_dialog.dialog,
			     FWCMD_H2C_SET_SND_PARA_NDPA_SND_DLG_DIALOG));
	h2c->dword8 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.ht_para.addr3[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword9 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.ht_para.addr3[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
			SET_WORD(snd_info->pndpa.ht_para.addr3[5],
				 FWCMD_H2C_SET_SND_PARA_MACID1) |
			SET_WORD(snd_info->pndpa.ht_para.seq_control,
				 FWCMD_H2C_SET_SND_PARA_HT_SEQ_CONTROL));
	h2c->dword10 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.vht_para.sta_info[0].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA0_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[0].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA0_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[0].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA0_NC) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[1].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA1_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[1].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA1_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[1].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA1_NC));
	h2c->dword11 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.vht_para.sta_info[2].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA2_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[2].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA2_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[2].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA2_NC) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[3].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA3_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[3].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA3_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[3].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA3_NC));
	h2c->dword12 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[0].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[0].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[0].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword13 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[1].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[1].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA1_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[1].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA1_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_NC));
	h2c->dword14 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[2].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[2].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA2_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[2].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA2_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_NC));
	h2c->dword15 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[3].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[3].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA3_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[3].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA3_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_NC));
	h2c->dword16 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[4].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[4].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA4_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[4].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA4_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_NC));
	h2c->dword17 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[5].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[5].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA5_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[5].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA5_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_NC));
	h2c->dword18 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[6].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[6].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA6_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[6].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA6_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_NC));
	h2c->dword19 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[7].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[7].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA7_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[7].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA7_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_NC));
	h2c->dword20 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[0].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword21 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword22 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword23 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword24 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[1].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword25 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword26 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword27 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword28 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[2].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword29 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword30 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword31 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword32 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].common.tgr_info,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_TRIGGER_INFO) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_len,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_LENGTH) |
		    (snd_info->pbfrp.he_para[0].common.more_tf ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MORE_TF : 0) |
		    (snd_info->pbfrp.he_para[0].common.cs_rqd ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_CS_REQUIRED : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_bw,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_BW) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_GI_LTF) |
		    (snd_info->pbfrp.he_para[0].common.mimo_ltfmode ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MU_MIMO_LTF_MODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.num_heltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_NUM_OF_HE_LTF) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_pktext,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_PKTEXT) |
		    (snd_info->pbfrp.he_para[0].common.ul_stbc ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_STBC : 0) |
		    (snd_info->pbfrp.he_para[0].common.ldpc_extra_sym ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_LDPC_EXTRA_SYMBOL : 0) |
		    (snd_info->pbfrp.he_para[0].common.dplr ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_DOPPLER : 0));
	h2c->dword33 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].common.ap_tx_pwr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_AP_TX_POWER) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_sr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_SPATIAL_REUSE));
	h2c->dword34 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[0].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[0].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[0].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword35 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[0].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword36 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[1].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[1].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[1].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword37 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[1].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword38 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[2].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[2].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[2].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword39 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[2].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword40 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[3].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[3].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[3].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword41 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[3].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword42 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].common.tgr_info,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_TRIGGER_INFO) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_len,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_LENGTH) |
		    (snd_info->pbfrp.he_para[1].common.more_tf ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MORE_TF : 0) |
		    (snd_info->pbfrp.he_para[1].common.cs_rqd ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_CS_REQUIRED : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_bw,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_BW) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_GI_LTF) |
		    (snd_info->pbfrp.he_para[1].common.mimo_ltfmode ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MU_MIMO_LTF_MODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.num_heltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_NUM_OF_HE_LTF) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_pktext,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_PKTEXT) |
		    (snd_info->pbfrp.he_para[1].common.ul_stbc ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_STBC : 0) |
		    (snd_info->pbfrp.he_para[1].common.ldpc_extra_sym ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_LDPC_EXTRA_SYMBOL : 0) |
		    (snd_info->pbfrp.he_para[1].common.dplr ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_DOPPLER : 0));
	h2c->dword43 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].common.ap_tx_pwr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_AP_TX_POWER) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_sr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_SPATIAL_REUSE));
	h2c->dword44 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[0].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[0].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[0].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword45 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[0].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword46 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[1].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[1].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[1].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword47 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[1].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword48 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[2].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[2].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[2].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword49 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[2].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword50 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[3].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[3].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[3].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword51 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[3].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword52 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.vht_para[0].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.vht_para[1].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP1_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.vht_para[2].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP2_FB_REXMIT));
	h2c->dword53 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[0].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword54 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[0].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[0].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[0].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[0].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[0].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[0].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[0].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[0].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[0].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword55 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[0].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[0].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[0].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[0].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[0].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[0].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[0].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[0].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword56 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[1].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword57 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[1].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[1].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[1].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[1].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[1].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[1].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[1].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[1].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[1].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword58 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[1].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[1].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[1].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[1].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[1].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[1].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[1].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[1].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword59 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[2].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword60 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[2].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[2].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[2].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[2].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[2].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[2].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[2].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[2].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[2].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword61 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[2].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[2].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[2].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[2].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[2].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[2].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[2].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[2].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword62 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[3].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword63 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[3].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[3].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[3].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[3].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[3].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[3].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[3].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[3].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[3].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword64 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[3].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[3].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[3].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[3].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[3].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[3].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[3].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[3].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword65 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[4].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword66 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[4].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[4].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[4].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[4].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[4].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[4].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[4].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[4].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[4].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword67 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[4].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[4].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[4].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[4].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[4].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[4].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[4].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[4].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword68 =
	cpu_to_le32(SET_WORD(snd_info->f2p[0].csi_len_bfrp,
			     FWCMD_H2C_SET_SND_PARA_CSI_LEN_BFRP0) |
		    SET_WORD(snd_info->f2p[0].tb_t_pe_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TB_T_PE_BFRP0) |
		    SET_WORD(snd_info->f2p[0].tri_pad_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TRI_PAD_BFRP0) |
		    (snd_info->f2p[0].ul_cqi_rpt_tri_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_UL_CQI_RPT_TRI_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[0].rf_gain_idx_bfrp,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0) |
		    (snd_info->f2p[0].fix_gain_en_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_FIX_GAIN_EN_BFRP0 : 0));
	h2c->dword69 =
	cpu_to_le32(SET_WORD(snd_info->f2p[1].csi_len_bfrp,
			     FWCMD_H2C_SET_SND_PARA_CSI_LEN_BFRP0) |
		    SET_WORD(snd_info->f2p[1].tb_t_pe_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TB_T_PE_BFRP0) |
		    SET_WORD(snd_info->f2p[1].tri_pad_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TRI_PAD_BFRP0) |
		    (snd_info->f2p[1].ul_cqi_rpt_tri_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_UL_CQI_RPT_TRI_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[1].rf_gain_idx_bfrp,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0) |
		    (snd_info->f2p[1].fix_gain_en_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_FIX_GAIN_EN_BFRP0 : 0));
	h2c->dword70 =
	cpu_to_le32(SET_WORD(snd_info->sfp.f2p_type,
			     FWCMD_H2C_SET_SND_PARA_F2P_TYPE) |
		    SET_WORD(snd_info->sfp.f2p_index,
			     FWCMD_H2C_SET_SND_PARA_F2P_INDEX) |
		    SET_WORD(snd_info->sfp.f2p_period,
			     FWCMD_H2C_SET_SND_PARA_F2P_PERIOD));
	h2c->dword71 =
	cpu_to_le32(SET_WORD(snd_info->sfp.f2p_updcnt,
			     FWCMD_H2C_SET_SND_PARA_F2P_UPDCNT) |
		    SET_WORD(snd_info->sfp.cr_idx,
			     FWCMD_H2C_SET_SND_PARA_CR_IDX));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_SOUND,
			      FWCMD_H2C_FUNC_SET_SND_PARA,
			      0,
			      0);
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
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
static u32 build_snd_h2c_v1(struct mac_ax_adapter *adapter, struct mac_ax_fwcmd_snd *snd_info)
{
	u32 ret = 0;
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_set_snd_para_v1 *h2c;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, MAX_FWCMD_SND_LEN);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMSET(buf, 0, MAX_FWCMD_SND_LEN);
	h2c = (struct fwcmd_set_snd_para_v1 *)buf;

	h2c->dword0 =
	cpu_to_le32(SET_WORD(snd_info->frexgtype,
			     FWCMD_H2C_SET_SND_PARA_FREXCH_TYPE) |
		    SET_WORD(snd_info->mode, FWCMD_H2C_SET_SND_PARA_MODE) |
		    SET_WORD(snd_info->bfrp0_user_num,
			     FWCMD_H2C_SET_SND_PARA_V1_HE_BFRP0_USER_NUM) |
		    SET_WORD(snd_info->bfrp1_user_num,
			     FWCMD_H2C_SET_SND_PARA_V1_HE_BFRP1_USER_NUM));
	h2c->dword1 =
	cpu_to_le32(SET_WORD(snd_info->macid[0], FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->macid[1], FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->macid[2], FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->macid[3], FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword2 =
	cpu_to_le32(SET_WORD(snd_info->macid[4], FWCMD_H2C_SET_SND_PARA_MACID4) |
		    SET_WORD(snd_info->macid[5], FWCMD_H2C_SET_SND_PARA_MACID5) |
		    SET_WORD(snd_info->macid[6], FWCMD_H2C_SET_SND_PARA_MACID6) |
		    SET_WORD(snd_info->macid[7], FWCMD_H2C_SET_SND_PARA_MACID7));
	h2c->dword3 =
	cpu_to_le32(SET_WORD(snd_info->macid[8], FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->macid[9], FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->macid[10], FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->macid[11], FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword4 =
	cpu_to_le32(SET_WORD(snd_info->macid[12], FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->macid[13], FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->macid[14], FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->macid[15], FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword5 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_NDPA_FRAME_CTRL) |
		    SET_WORD(snd_info->pndpa.common.duration,
			     FWCMD_H2C_SET_SND_PARA_NDPA_DURATION));
	h2c->dword6 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword7 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword8 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.common.addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.common.addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.common.addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.common.addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword9 =
	cpu_to_le32((snd_info->pndpa.snd_dialog.he ?
		     FWCMD_H2C_SET_SND_PARA_NDPA_SND_DLG_HE : 0) |
		    SET_WORD(snd_info->pndpa.snd_dialog.dialog,
			     FWCMD_H2C_SET_SND_PARA_NDPA_SND_DLG_DIALOG));
	h2c->dword10 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.ht_para.addr3[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pndpa.ht_para.addr3[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword11 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.ht_para.addr3[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
			SET_WORD(snd_info->pndpa.ht_para.addr3[5],
				 FWCMD_H2C_SET_SND_PARA_MACID1) |
			SET_WORD(snd_info->pndpa.ht_para.seq_control,
				 FWCMD_H2C_SET_SND_PARA_HT_SEQ_CONTROL));
	h2c->dword12 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.vht_para.sta_info[0].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA0_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[0].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA0_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[0].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA0_NC) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[1].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA1_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[1].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA1_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[1].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA1_NC));
	h2c->dword13 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.vht_para.sta_info[2].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA2_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[2].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA2_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[2].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA2_NC) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[3].aid,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA3_AID12) |
		    (snd_info->pndpa.vht_para.sta_info[3].fb_type ?
		     FWCMD_H2C_SET_SND_PARA_VHT_STA3_FEEDBACK_TYPE : 0) |
		    SET_WORD(snd_info->pndpa.vht_para.sta_info[3].nc,
			     FWCMD_H2C_SET_SND_PARA_VHT_STA3_NC));
	h2c->dword14 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[0].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[0].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[0].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[0].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword15 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[1].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[1].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA1_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[1].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA1_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[1].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA1_NC));
	h2c->dword16 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[2].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[2].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA2_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[2].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA2_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[2].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA2_NC));
	h2c->dword17 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[3].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[3].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA3_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[3].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA3_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[3].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA3_NC));
	h2c->dword18 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[4].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[4].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA4_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[4].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA4_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[4].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA4_NC));
	h2c->dword19 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[5].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[5].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA5_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[5].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA5_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[5].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA5_NC));
	h2c->dword20 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[6].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[6].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA6_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[6].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA6_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[6].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA6_NC));
	h2c->dword21 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[7].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[7].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA7_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[7].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA7_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[7].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA7_NC));
	h2c->dword22 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[8].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[8].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[8].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[8].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[8].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[8].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword23 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[9].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[9].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[9].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[9].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[9].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[9].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword24 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[10].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[10].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[10].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[10].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[10].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[10].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword25 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[11].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[11].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[11].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[11].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[11].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[11].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword26 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[12].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[12].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[12].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[12].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[12].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[12].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword27 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[13].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[13].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[13].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[13].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[13].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[13].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword28 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[14].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[14].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[14].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[14].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[14].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[14].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword29 =
	cpu_to_le32(SET_WORD(snd_info->pndpa.he_para.sta_info[15].aid,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_AID11) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[15].bw,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_BW) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[15].fb_ng,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_FB_NG) |
		    (snd_info->pndpa.he_para.sta_info[15].disambiguation ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_DISAMBIGUATION : 0) |
		    (snd_info->pndpa.he_para.sta_info[15].cb ?
		     FWCMD_H2C_SET_SND_PARA_HE_STA0_CB : 0) |
		    SET_WORD(snd_info->pndpa.he_para.sta_info[15].nc,
			     FWCMD_H2C_SET_SND_PARA_HE_STA0_NC));
	h2c->dword30 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[0].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword31 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword32 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword33 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[0].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[0].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword34 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[1].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword35 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword36 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword37 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[1].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[1].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword38 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].frame_ctl,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_FRAME_CTL) |
		    SET_WORD(snd_info->pbfrp.hdr[2].duration,
			     FWCMD_H2C_SET_SND_PARA_BFRP0_DURATION));
	h2c->dword39 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr1[0],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[1],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[2],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[3],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword40 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr1[4],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr1[5],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[0],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[1],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword41 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.hdr[2].addr2[2],
			     FWCMD_H2C_SET_SND_PARA_MACID0) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[3],
			     FWCMD_H2C_SET_SND_PARA_MACID1) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[4],
			     FWCMD_H2C_SET_SND_PARA_MACID2) |
		    SET_WORD(snd_info->pbfrp.hdr[2].addr2[5],
			     FWCMD_H2C_SET_SND_PARA_MACID3));
	h2c->dword42 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].common.tgr_info,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_TRIGGER_INFO) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_len,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_LENGTH) |
		    (snd_info->pbfrp.he_para[0].common.more_tf ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MORE_TF : 0) |
		    (snd_info->pbfrp.he_para[0].common.cs_rqd ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_CS_REQUIRED : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_bw,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_BW) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_GI_LTF) |
		    (snd_info->pbfrp.he_para[0].common.mimo_ltfmode ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MU_MIMO_LTF_MODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.num_heltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_NUM_OF_HE_LTF) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_pktext,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_PKTEXT) |
		    (snd_info->pbfrp.he_para[0].common.ul_stbc ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_STBC : 0) |
		    (snd_info->pbfrp.he_para[0].common.ldpc_extra_sym ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_LDPC_EXTRA_SYMBOL : 0) |
		    (snd_info->pbfrp.he_para[0].common.dplr ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_DOPPLER : 0));
	h2c->dword43 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].common.ap_tx_pwr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_AP_TX_POWER) |
		    SET_WORD(snd_info->pbfrp.he_para[0].common.ul_sr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_SPATIAL_REUSE));
	h2c->dword44 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[0].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[0].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[0].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword45 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[0].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[0].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword46 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[1].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[1].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[1].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword47 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[1].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[1].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword48 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[2].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[2].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[2].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword49 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[2].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[2].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword50 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[3].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[3].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[3].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword51 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[3].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[3].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword52 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[4].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[4].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[4].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[4].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[4].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[4].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword53 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[4].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[4].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword54 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[5].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[5].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[5].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[5].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[5].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[5].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword55 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[5].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[5].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword56 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[6].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[6].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[6].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[6].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[6].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[6].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword57 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[6].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[6].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword58 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[7].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[7].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[0].user[7].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[7].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[0].user[7].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[7].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword59 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[0].user[7].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[0].user[7].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword60 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].common.tgr_info,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_TRIGGER_INFO) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_len,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_LENGTH) |
		    (snd_info->pbfrp.he_para[1].common.more_tf ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MORE_TF : 0) |
		    (snd_info->pbfrp.he_para[1].common.cs_rqd ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_CS_REQUIRED : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_bw,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_BW) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_GI_LTF) |
		    (snd_info->pbfrp.he_para[1].common.mimo_ltfmode ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_MU_MIMO_LTF_MODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.num_heltf,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_NUM_OF_HE_LTF) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_pktext,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_PKTEXT) |
		    (snd_info->pbfrp.he_para[1].common.ul_stbc ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_STBC : 0) |
		    (snd_info->pbfrp.he_para[1].common.ldpc_extra_sym ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_LDPC_EXTRA_SYMBOL : 0) |
		    (snd_info->pbfrp.he_para[1].common.dplr ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_DOPPLER : 0));
	h2c->dword61 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].common.ap_tx_pwr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_AP_TX_POWER) |
		    SET_WORD(snd_info->pbfrp.he_para[1].common.ul_sr,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_UL_SPATIAL_REUSE));
	h2c->dword62 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[0].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[0].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[0].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword63 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[0].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[0].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword64 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[1].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[1].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[1].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword65 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[1].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[1].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword66 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[2].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[2].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[2].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword67 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[2].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[2].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword68 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[3].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[3].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[3].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword69 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[3].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[3].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword70 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[4].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[4].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[4].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[4].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[4].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[4].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword71 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[4].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[4].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword72 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[5].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[5].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[5].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[5].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[5].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[5].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword73 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[5].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[5].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword74 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[6].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[6].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[6].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[6].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[6].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[6].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword75 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[6].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[6].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword76 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[7].aid12,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_AID12) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[7].ru_pos,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_RU_POS) |
		    (snd_info->pbfrp.he_para[1].user[7].ul_fec_code ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_FEC_CODE : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[7].ul_mcs,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_MCS) |
		    (snd_info->pbfrp.he_para[1].user[7].ul_dcm ?
		     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_DCM : 0) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[7].ss_alloc,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_SS_ALLOC));
	h2c->dword77 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.he_para[1].user[7].fbseg_rexmit_bmp,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.he_para[1].user[7].ul_tgt_rssi,
			     FWCMD_H2C_SET_SND_PARA_HE_BFRP0_U0_UL_TGT_RSSI));
	h2c->dword78 =
	cpu_to_le32(SET_WORD(snd_info->pbfrp.vht_para[0].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP0_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.vht_para[1].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP1_FB_REXMIT) |
		    SET_WORD(snd_info->pbfrp.vht_para[2].retransmission_bitmap,
			     FWCMD_H2C_SET_SND_PARA_VHT_BFRP2_FB_REXMIT));
	h2c->dword79 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[0].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword80 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[0].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[0].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[0].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[0].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[0].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[0].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[0].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[0].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[0].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword81 =
	cpu_to_le32(SET_WORD(snd_info->wd[0].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[0].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[0].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[0].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[0].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[0].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[0].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[0].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[0].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[0].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword82 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[1].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword83 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[1].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[1].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[1].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[1].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[1].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[1].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[1].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[1].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[1].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword84 =
	cpu_to_le32(SET_WORD(snd_info->wd[1].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[1].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[1].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[1].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[1].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[1].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[1].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[1].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[1].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[1].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword85 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[2].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword86 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[2].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[2].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[2].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[2].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[2].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[2].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[2].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[2].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[2].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword87 =
	cpu_to_le32(SET_WORD(snd_info->wd[2].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[2].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[2].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[2].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[2].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[2].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[2].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[2].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[2].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[2].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword88 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[3].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword89 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[3].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[3].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[3].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[3].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[3].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[3].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[3].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[3].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[3].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword90 =
	cpu_to_le32(SET_WORD(snd_info->wd[3].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[3].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[3].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[3].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[3].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[3].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[3].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[3].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[3].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[3].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword91 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].txpktsize,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPKTSIZE_WD0) |
		    SET_WORD(snd_info->wd[4].ndpa_duration,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_DURATION_WD0));
	h2c->dword92 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].datarate,
			     FWCMD_H2C_SET_SND_PARA_WD_DATARATE_WD0) |
		    SET_WORD(snd_info->wd[4].macid,
			     FWCMD_H2C_SET_SND_PARA_WD_MACID_WD0) |
		    (snd_info->wd[4].force_txop ?
		     FWCMD_H2C_SET_SND_PARA_WD_FORCE_TXOP_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].data_bw,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_WD0) |
		    SET_WORD(snd_info->wd[4].gi_ltf,
			     FWCMD_H2C_SET_SND_PARA_WD_GI_LTF_WD0) |
		    (snd_info->wd[4].data_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_ER_WD0 : 0) |
		    (snd_info->wd[4].data_dcm ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_DCM_WD0 : 0) |
		    (snd_info->wd[4].data_stbc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_STBC_WD0 : 0) |
		    (snd_info->wd[4].data_ldpc ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_LDPC_WD0 : 0) |
		    (snd_info->wd[4].data_bw_er ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_BW_ER_WD0 : 0) |
		    (snd_info->wd[4].multiport_id ?
		     FWCMD_H2C_SET_SND_PARA_WD_MULTIPORT_ID_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].mbssid,
			     FWCMD_H2C_SET_SND_PARA_WD_MBSSID_WD0));
	h2c->dword93 =
	cpu_to_le32(SET_WORD(snd_info->wd[4].signaling_ta_pkt_sc,
			     FWCMD_H2C_SET_SND_PARA_WD_SIGNALING_TA_PKT_SC_WD0) |
		    SET_WORD(snd_info->wd[4].sw_define,
			     FWCMD_H2C_SET_SND_PARA_WD_SW_DEFINE_WD0) |
		    SET_WORD(snd_info->wd[4].txpwr_ofset_type,
			     FWCMD_H2C_SET_SND_PARA_WD_TXPWR_OFSET_TYPE_WD0) |
		    SET_WORD(snd_info->wd[4].lifetime_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_LIFETIME_SEL_WD0) |
		    (snd_info->wd[4].stf_mode ?
		     FWCMD_H2C_SET_SND_PARA_WD_STF_MODE_WD0 : 0) |
		    (snd_info->wd[4].disdatafb ?
		     FWCMD_H2C_SET_SND_PARA_WD_DISDATAFB_WD0 : 0) |
		    (snd_info->wd[4].data_txcnt_lmt_sel ?
		     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_SEL_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].data_txcnt_lmt,
			     FWCMD_H2C_SET_SND_PARA_WD_DATA_TXCNT_LMT_WD0) |
		    (snd_info->wd[4].sifs_tx ?
		     FWCMD_H2C_SET_SND_PARA_WD_SIFS_TX_WD0 : 0) |
		    SET_WORD(snd_info->wd[4].snd_pkt_sel,
			     FWCMD_H2C_SET_SND_PARA_WD_SND_PKT_SEL_WD0) |
		    SET_WORD(snd_info->wd[4].ndpa,
			     FWCMD_H2C_SET_SND_PARA_WD_NDPA_WD0));
	h2c->dword94 =
	cpu_to_le32(SET_WORD(snd_info->f2p[0].csi_len_bfrp,
			     FWCMD_H2C_SET_SND_PARA_CSI_LEN_BFRP0) |
		    SET_WORD(snd_info->f2p[0].tb_t_pe_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TB_T_PE_BFRP0) |
		    SET_WORD(snd_info->f2p[0].tri_pad_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TRI_PAD_BFRP0) |
		    (snd_info->f2p[0].ul_cqi_rpt_tri_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_UL_CQI_RPT_TRI_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[0].rf_gain_idx_bfrp,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0) |
		    (snd_info->f2p[0].fix_gain_en_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_FIX_GAIN_EN_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[0].ul_len_ref,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0));
	h2c->dword95 =
	cpu_to_le32(SET_WORD(snd_info->f2p[1].csi_len_bfrp,
			     FWCMD_H2C_SET_SND_PARA_CSI_LEN_BFRP0) |
		    SET_WORD(snd_info->f2p[1].tb_t_pe_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TB_T_PE_BFRP0) |
		    SET_WORD(snd_info->f2p[1].tri_pad_bfrp,
			     FWCMD_H2C_SET_SND_PARA_TRI_PAD_BFRP0) |
		    (snd_info->f2p[1].ul_cqi_rpt_tri_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_UL_CQI_RPT_TRI_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[1].rf_gain_idx_bfrp,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0) |
		    (snd_info->f2p[1].fix_gain_en_bfrp ?
		     FWCMD_H2C_SET_SND_PARA_FIX_GAIN_EN_BFRP0 : 0) |
		    SET_WORD(snd_info->f2p[1].ul_len_ref,
			     FWCMD_H2C_SET_SND_PARA_RF_GAIN_IDX_BFRP0));
	h2c->dword96 =
	cpu_to_le32(SET_WORD(snd_info->sfp.f2p_type,
			     FWCMD_H2C_SET_SND_PARA_F2P_TYPE) |
		    SET_WORD(snd_info->sfp.f2p_index,
			     FWCMD_H2C_SET_SND_PARA_F2P_INDEX) |
		    SET_WORD(snd_info->sfp.f2p_period,
			     FWCMD_H2C_SET_SND_PARA_F2P_PERIOD));
	h2c->dword97 =
	cpu_to_le32(SET_WORD(snd_info->sfp.f2p_updcnt,
			     FWCMD_H2C_SET_SND_PARA_F2P_UPDCNT) |
		    SET_WORD(snd_info->sfp.cr_idx,
			     FWCMD_H2C_SET_SND_PARA_CR_IDX));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_SOUND,
			      FWCMD_H2C_FUNC_SET_SND_PARA_V1,
			      0,
			      0);
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
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}
#endif

u32 mac_set_snd_para(struct mac_ax_adapter *adapter,
		     struct mac_ax_fwcmd_snd *snd_info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret, val32;
	u8 band = 0;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;
	//set wmm1 to sw mode tx for snd
	val32 = MAC_REG_R32(R_AX_PTCL_COMMON_SETTING_0);
	val32 = val32 & (~B_AX_CMAC_TX_MODE_1);
	MAC_REG_W32(R_AX_PTCL_COMMON_SETTING_0, val32);

	switch (snd_info->frexgtype) {
	case FRAME_EXCHANGE_SND_N_SU:
		snd_info->wd[0].txpktsize = 31;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 0;
		snd_info->wd[0].ndpa = 1;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 0;
		snd_info->wd[1].snd_pkt_sel = 2;
		snd_info->wd[1].ndpa = 1;

		snd_info->wd[2].txpktsize = 0;
		snd_info->wd[2].stf_mode = 0;
		snd_info->wd[2].disdatafb = 0;
		snd_info->wd[2].data_txcnt_lmt_sel = 0;
		snd_info->wd[2].data_txcnt_lmt = 0;
		snd_info->wd[2].sifs_tx = 0;
		snd_info->wd[2].snd_pkt_sel = 0;
		snd_info->wd[2].ndpa = 0;

		snd_info->wd[3].txpktsize = 0;
		snd_info->wd[3].stf_mode = 0;
		snd_info->wd[3].disdatafb = 0;
		snd_info->wd[3].data_txcnt_lmt_sel = 0;
		snd_info->wd[3].data_txcnt_lmt = 0;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 0;
		snd_info->wd[3].ndpa = 0;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;

	case FRAME_EXCHANGE_SND_AC_SU:
		snd_info->wd[0].txpktsize = 19;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 0;
		snd_info->wd[0].ndpa = 2;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 0;
		snd_info->wd[1].snd_pkt_sel = 2;
		snd_info->wd[1].ndpa = 2;

		snd_info->wd[2].txpktsize = 0;
		snd_info->wd[2].stf_mode = 0;
		snd_info->wd[2].disdatafb = 0;
		snd_info->wd[2].data_txcnt_lmt_sel = 0;
		snd_info->wd[2].data_txcnt_lmt = 0;
		snd_info->wd[2].sifs_tx = 0;
		snd_info->wd[2].snd_pkt_sel = 0;
		snd_info->wd[2].ndpa = 0;

		snd_info->wd[3].txpktsize = 0;
		snd_info->wd[3].stf_mode = 0;
		snd_info->wd[3].disdatafb = 0;
		snd_info->wd[3].data_txcnt_lmt_sel = 0;
		snd_info->wd[3].data_txcnt_lmt = 0;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 0;
		snd_info->wd[3].ndpa = 0;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	case FRAME_EXCHANGE_SND_AC_MU_BFRP1:
		snd_info->wd[0].txpktsize = 21;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 1;
		snd_info->wd[0].ndpa = 2;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 1;
		snd_info->wd[1].snd_pkt_sel = 3;
		snd_info->wd[1].ndpa = 2;

		snd_info->wd[2].txpktsize = 17;
		snd_info->wd[2].stf_mode = 1;
		snd_info->wd[2].disdatafb = 1;
		snd_info->wd[2].data_txcnt_lmt_sel = 1;
		snd_info->wd[2].data_txcnt_lmt = 1;
		snd_info->wd[2].sifs_tx = 0;
		snd_info->wd[2].snd_pkt_sel = 5;
		snd_info->wd[2].ndpa = 2;

		snd_info->wd[3].txpktsize = 0;
		snd_info->wd[3].stf_mode = 0;
		snd_info->wd[3].disdatafb = 0;
		snd_info->wd[3].data_txcnt_lmt_sel = 0;
		snd_info->wd[3].data_txcnt_lmt = 0;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 0;
		snd_info->wd[3].ndpa = 0;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	case FRAME_EXCHANGE_SND_AC_MU_BFRP2:
		snd_info->wd[0].txpktsize = 23;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 1;
		snd_info->wd[0].ndpa = 2;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 1;
		snd_info->wd[1].snd_pkt_sel = 3;
		snd_info->wd[1].ndpa = 2;

		snd_info->wd[2].txpktsize = 17;
		snd_info->wd[2].stf_mode = 1;
		snd_info->wd[2].disdatafb = 1;
		snd_info->wd[2].data_txcnt_lmt_sel = 1;
		snd_info->wd[2].data_txcnt_lmt = 1;
		snd_info->wd[2].sifs_tx = 1;
		snd_info->wd[2].snd_pkt_sel = 4;
		snd_info->wd[2].ndpa = 2;

		snd_info->wd[3].txpktsize = 17;
		snd_info->wd[3].stf_mode = 1;
		snd_info->wd[3].disdatafb = 1;
		snd_info->wd[3].data_txcnt_lmt_sel = 1;
		snd_info->wd[3].data_txcnt_lmt = 1;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 5;
		snd_info->wd[3].ndpa = 2;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	case FRAME_EXCHANGE_SND_AC_MU_BFRP3:
		snd_info->wd[0].txpktsize = 25;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 1;
		snd_info->wd[0].ndpa = 2;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 1;
		snd_info->wd[1].snd_pkt_sel = 3;
		snd_info->wd[1].ndpa = 2;

		snd_info->wd[2].txpktsize = 17;
		snd_info->wd[2].stf_mode = 1;
		snd_info->wd[2].disdatafb = 1;
		snd_info->wd[2].data_txcnt_lmt_sel = 1;
		snd_info->wd[2].data_txcnt_lmt = 1;
		snd_info->wd[2].sifs_tx = 1;
		snd_info->wd[2].snd_pkt_sel = 4;
		snd_info->wd[2].ndpa = 2;

		snd_info->wd[3].txpktsize = 17;
		snd_info->wd[3].stf_mode = 1;
		snd_info->wd[3].disdatafb = 1;
		snd_info->wd[3].data_txcnt_lmt_sel = 1;
		snd_info->wd[3].data_txcnt_lmt = 1;
		snd_info->wd[3].sifs_tx = 1;
		snd_info->wd[3].snd_pkt_sel = 4;
		snd_info->wd[3].ndpa = 2;

		snd_info->wd[4].txpktsize = 17;
		snd_info->wd[4].stf_mode = 1;
		snd_info->wd[4].disdatafb = 1;
		snd_info->wd[4].data_txcnt_lmt_sel = 1;
		snd_info->wd[4].data_txcnt_lmt = 1;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 5;
		snd_info->wd[4].ndpa = 2;
		break;
	case FRAME_EXCHANGE_SND_AX_SU:
		snd_info->wd[0].txpktsize = 21;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 0;
		snd_info->wd[0].ndpa = 3;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 0;
		snd_info->wd[1].snd_pkt_sel = 2;
		snd_info->wd[1].ndpa = 3;

		snd_info->wd[2].txpktsize = 0;
		snd_info->wd[2].stf_mode = 0;
		snd_info->wd[2].disdatafb = 0;
		snd_info->wd[2].data_txcnt_lmt_sel = 0;
		snd_info->wd[2].data_txcnt_lmt = 0;
		snd_info->wd[2].sifs_tx = 0;
		snd_info->wd[2].snd_pkt_sel = 0;
		snd_info->wd[2].ndpa = 0;

		snd_info->wd[3].txpktsize = 0;
		snd_info->wd[3].stf_mode = 0;
		snd_info->wd[3].disdatafb = 0;
		snd_info->wd[3].data_txcnt_lmt_sel = 0;
		snd_info->wd[3].data_txcnt_lmt = 0;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 0;
		snd_info->wd[3].ndpa = 0;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	case FRAME_EXCHANGE_SND_AX_MU_BFRP1:
		snd_info->wd[0].txpktsize = 17 + 4 * snd_info->bfrp0_user_num;
		if (snd_info->mode == SND_NDPA_PATCH_STA)
			snd_info->wd[0].txpktsize += 4;
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 1;
		snd_info->wd[0].ndpa = 3;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 1;
		snd_info->wd[1].snd_pkt_sel = 3;
		snd_info->wd[1].ndpa = 3;

		snd_info->wd[2].txpktsize = 24 + 6 * snd_info->bfrp0_user_num;
		snd_info->wd[2].stf_mode = 1;
		snd_info->wd[2].disdatafb = 1;
		snd_info->wd[2].data_txcnt_lmt_sel = 1;
		snd_info->wd[2].data_txcnt_lmt = 1;
		snd_info->wd[2].sifs_tx = 0;
		snd_info->wd[2].snd_pkt_sel = 5;
		snd_info->wd[2].ndpa = 3;

		snd_info->wd[3].txpktsize = 0;
		snd_info->wd[3].stf_mode = 0;
		snd_info->wd[3].disdatafb = 0;
		snd_info->wd[3].data_txcnt_lmt_sel = 0;
		snd_info->wd[3].data_txcnt_lmt = 0;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 0;
		snd_info->wd[3].ndpa = 0;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	case FRAME_EXCHANGE_SND_AX_MU_BFRP2:
		snd_info->wd[0].txpktsize = 17 + 4 * (snd_info->bfrp0_user_num
					    + snd_info->bfrp1_user_num);
		snd_info->wd[0].stf_mode = 1;
		snd_info->wd[0].disdatafb = 1;
		snd_info->wd[0].data_txcnt_lmt_sel = 1;
		snd_info->wd[0].data_txcnt_lmt = 1;
		snd_info->wd[0].sifs_tx = 1;
		snd_info->wd[0].snd_pkt_sel = 1;
		snd_info->wd[0].ndpa = 3;

		snd_info->wd[1].txpktsize = 0;
		snd_info->wd[1].stf_mode = 1;
		snd_info->wd[1].disdatafb = 1;
		snd_info->wd[1].data_txcnt_lmt_sel = 1;
		snd_info->wd[1].data_txcnt_lmt = 1;
		snd_info->wd[1].sifs_tx = 1;
		snd_info->wd[1].snd_pkt_sel = 3;
		snd_info->wd[1].ndpa = 3;

		snd_info->wd[2].txpktsize = 24 + 6 * (snd_info->bfrp0_user_num);
		snd_info->wd[2].stf_mode = 1;
		snd_info->wd[2].disdatafb = 1;
		snd_info->wd[2].data_txcnt_lmt_sel = 1;
		snd_info->wd[2].data_txcnt_lmt = 1;
		snd_info->wd[2].sifs_tx = 1;
		snd_info->wd[2].snd_pkt_sel = 4;
		snd_info->wd[2].ndpa = 3;

		snd_info->wd[3].txpktsize = 24 + 6 * (snd_info->bfrp1_user_num);
		snd_info->wd[3].stf_mode = 1;
		snd_info->wd[3].disdatafb = 1;
		snd_info->wd[3].data_txcnt_lmt_sel = 1;
		snd_info->wd[3].data_txcnt_lmt = 1;
		snd_info->wd[3].sifs_tx = 0;
		snd_info->wd[3].snd_pkt_sel = 5;
		snd_info->wd[3].ndpa = 3;

		snd_info->wd[4].txpktsize = 0;
		snd_info->wd[4].stf_mode = 0;
		snd_info->wd[4].disdatafb = 0;
		snd_info->wd[4].data_txcnt_lmt_sel = 0;
		snd_info->wd[4].data_txcnt_lmt = 0;
		snd_info->wd[4].sifs_tx = 0;
		snd_info->wd[4].snd_pkt_sel = 0;
		snd_info->wd[4].ndpa = 0;
		break;
	default:
		return MACNOTSUP;
	}

	// check input valid
	if (snd_info->sfp.f2p_type > SNDF2P_DEL) {
		PLTFM_MSG_ERR("Unrecognized SND F2P type: %d\n",
			      snd_info->sfp.f2p_type);
		return MACFUNCINPUT;
	}
	if (snd_info->sfp.f2p_index >= MAX_SNDTXCMDINFO_NUM) {
		PLTFM_MSG_ERR("Unrecognized SND F2P index: %d\n",
			      snd_info->sfp.f2p_index);
		return MACFUNCINPUT;
	}
	if (snd_info->sfp.f2p_type == SNDF2P_ADD && snd_info->sfp.f2p_period == 0) {
		PLTFM_MSG_ERR("Unrecognized SND F2P period: %d\n",
			      snd_info->sfp.f2p_period);
		return MACFUNCINPUT;
	}

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

#if MAC_AX_8852A_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		return build_snd_h2c(adapter, snd_info);
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
		return build_snd_h2c_v1(adapter, snd_info);
#endif

	return MACNOTSUP;
}
