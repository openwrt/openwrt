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
#include "rrsr_8852c.h"
#if MAC_AX_8852C_SUPPORT

u32 mac_get_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rrsr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, cfg->band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, cfg->band);
		return val32;
	}

	val32 = MAC_REG_R32((cfg->band == MAC_AX_BAND_1) ?
			    R_AX_TRXPTCL_RRSR_CTL_0_C1 : R_AX_TRXPTCL_RRSR_CTL_0);
	cfg->doppler_en = ((val32 & B_AX_WMAC_RESP_DOPPLEB_AX_EN) != 0);
	cfg->dcm_en = ((val32 & B_AX_WMAC_RESP_DCM_EN) != 0);
	cfg->ref_rate_sel = ((val32 & B_AX_WMAC_RESP_REF_RATE_SEL) != 0);
	cfg->ref_rate =  GET_FIELD(val32, B_AX_WMAC_RESP_REF_RATE);
	cfg->ftm_rrsr_rate_en =  GET_FIELD(val32, B_AX_FTM_RRSR_RATE_EN);

	val32 = MAC_REG_R32((cfg->band == MAC_AX_BAND_1) ?
			    R_AX_PTCL_RRSR1_C1 : R_AX_PTCL_RRSR1);
	cfg->rrsr_rate_en = GET_FIELD(val32, B_AX_RRSR_RATE_EN);
	cfg->cck_cfg = GET_FIELD(val32, B_AX_RRSR_CCK);
	cfg->rsc = GET_FIELD(val32, B_AX_RSC);

	val32 = MAC_REG_R32((cfg->band == MAC_AX_BAND_1) ?
			    R_AX_PTCL_RRSR0_C1 : R_AX_PTCL_RRSR0);
	cfg->ofdm_cfg = GET_FIELD(val32, B_AX_RRSR_OFDM);
	cfg->ht_cfg = GET_FIELD(val32, B_AX_RRSR_HT);
	cfg->vht_cfg = GET_FIELD(val32, B_AX_RRSR_VHT);
	cfg->he_cfg = GET_FIELD(val32, B_AX_RRSR_HE);

	return MACSUCCESS;
}

u32 mac_set_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rrsr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, rrsr_ctl_0, rrsr_ctl_1, rrsr_ctl_2;

	val32 = check_mac_en(adapter, cfg->band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, cfg->band);
		return val32;
	}

	rrsr_ctl_0 = SET_WORD(cfg->ftm_rrsr_rate_en, B_AX_FTM_RRSR_RATE_EN) |
		     (cfg->doppler_en ? B_AX_WMAC_RESP_DOPPLEB_AX_EN : 0) |
		     (cfg->dcm_en ? B_AX_WMAC_RESP_DCM_EN : 0) |
		     (cfg->ref_rate_sel ? B_AX_WMAC_RESP_REF_RATE_SEL : 0) |
		     SET_WORD(cfg->ref_rate, B_AX_WMAC_RESP_REF_RATE);

	rrsr_ctl_1 = SET_WORD(cfg->rrsr_rate_en, B_AX_RRSR_RATE_EN) |
		     SET_WORD(cfg->cck_cfg, B_AX_RRSR_CCK) |
		     SET_WORD(cfg->rsc, B_AX_RSC);

	rrsr_ctl_2 = SET_WORD(cfg->ofdm_cfg, B_AX_RRSR_OFDM) |
		     SET_WORD(cfg->ht_cfg, B_AX_RRSR_HT) |
		     SET_WORD(cfg->vht_cfg, B_AX_RRSR_VHT) |
		     SET_WORD(cfg->he_cfg, B_AX_RRSR_HE);

	// add fw offload later
	if (cfg->band == MAC_AX_BAND_0) {
		MAC_REG_W32(R_AX_TRXPTCL_RRSR_CTL_0, rrsr_ctl_0);
		MAC_REG_W32(R_AX_PTCL_RRSR1, rrsr_ctl_1);
		MAC_REG_W32(R_AX_PTCL_RRSR0, rrsr_ctl_2);
	} else {
		MAC_REG_W32(R_AX_TRXPTCL_RRSR_CTL_0_C1, rrsr_ctl_0);
		MAC_REG_W32(R_AX_PTCL_RRSR1_C1, rrsr_ctl_1);
		MAC_REG_W32(R_AX_PTCL_RRSR0_C1, rrsr_ctl_2);
	}

	return MACSUCCESS;
}

u32 mac_get_cts_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			       struct mac_ax_cts_rrsr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, cfg->band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, cfg->band);
		return val32;
	}

	val32 = MAC_REG_R32((cfg->band == MAC_AX_BAND_1) ?
			    R_AX_TRXPTCL_CTS_RRSR_C1 : R_AX_TRXPTCL_CTS_RRSR);
	cfg->cts_rrsr_rsc = GET_FIELD(val32, B_AX_WMAC_CTS_RRSR_RSC);
	cfg->cts_rrsr_opt = ((val32 & B_AX_WMAC_CTS_RESP_OPT) != 0);
	cfg->cts_rrsr_cck_cfg = GET_FIELD(val32, B_AX_WMAC_CTS_RRSR_CCK);
	cfg->cts_rrsr_ofdm_cfg = GET_FIELD(val32, B_AX_WMAC_CTS_RRSR_OFDM);

	return MACSUCCESS;
}

u32 mac_set_cts_rrsr_cfg_8852c(struct mac_ax_adapter *adapter,
			       struct mac_ax_cts_rrsr_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = check_mac_en(adapter, cfg->band, MAC_AX_CMAC_SEL);
	if (val32 != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s CMAC%d not enable\n", __func__, cfg->band);
		return val32;
	}

	val32 = SET_WORD(cfg->cts_rrsr_rsc, B_AX_WMAC_CTS_RRSR_RSC) |
		(cfg->cts_rrsr_opt ? B_AX_WMAC_CTS_RESP_OPT : 0) |
		SET_WORD(cfg->cts_rrsr_cck_cfg, B_AX_WMAC_CTS_RRSR_CCK) |
		SET_WORD(cfg->cts_rrsr_ofdm_cfg, B_AX_WMAC_CTS_RRSR_OFDM);

	// add fw offload later
	if (cfg->band == MAC_AX_BAND_0)
		MAC_REG_W32(R_AX_TRXPTCL_CTS_RRSR, val32);
	else
		MAC_REG_W32(R_AX_TRXPTCL_CTS_RRSR_C1, val32);

	return MACSUCCESS;
}

#endif /* #if MAC_AX_8852C_SUPPORT */
