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

#include "power_saving.h"
#include "coex.h"
#include "mac_priv.h"

#define RPWM_SEQ_NUM_MAX                3
#define CPWM_SEQ_NUM_MAX                3

//RPWM bit definition
#define PS_RPWM_TOGGLE			BIT(15)
#define PS_RPWM_ACK             BIT(14)
#define PS_RPWM_SEQ_NUM_SH      12
#define PS_RPWM_SEQ_NUM_MSK     0x3
#define PS_RPWM_NOTIFY_WAKE     BIT(8)
#define PS_RPWM_STATE_SH        0
#define PS_RPWM_STATE_MSK       0x7

//CPWM bit definition
#define PS_CPWM_TOGGLE			BIT(15)
#define PS_CPWM_ACK             BIT(14)
#define PS_CPWM_SEQ_NUM_SH      12
#define PS_CPWM_SEQ_NUM_MSK     0x3
#define PS_CPWM_RSP_SEQ_NUM_SH  8
#define PS_CPWM_RSP_SEQ_NUM_MSK 0x3
#define PS_CPWM_STATE_SH        0
#define PS_CPWM_STATE_MSK       0x7

//(workaround) CPWM register is in OFF area
//LPS debug message bit definition
#define B_PS_LDM_32K_EN         BIT(31)
#define B_PS_LDM_32K_EN_SH      31

static u32 lps_status[4] = {0};
static u32 ips_status[4] = {0};
static u8 rpwm_seq_num = RPWM_SEQ_NUM_MAX;
static u8 cpwm_seq_num = CPWM_SEQ_NUM_MAX;

static u32 send_h2c_lps_parm(struct mac_ax_adapter *adapter,
			     struct lps_parm *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_lps_parm *fwcmd_lps;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_lps_parm));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_lps = (struct fwcmd_lps_parm *)buf;
	fwcmd_lps->dword0 =
	cpu_to_le32(SET_WORD(parm->macid, FWCMD_H2C_LPS_PARM_MACID) |
		SET_WORD(parm->psmode, FWCMD_H2C_LPS_PARM_PSMODE) |
		SET_WORD(parm->rlbm, FWCMD_H2C_LPS_PARM_RLBM) |
		SET_WORD(parm->smartps, FWCMD_H2C_LPS_PARM_SMARTPS) |
		SET_WORD(parm->awakeinterval,
			 FWCMD_H2C_LPS_PARM_AWAKEINTERVAL));

	fwcmd_lps->dword1 =
	cpu_to_le32((parm->vouapsd ? FWCMD_H2C_LPS_PARM_VOUAPSD : 0) |
		(parm->viuapsd ? FWCMD_H2C_LPS_PARM_VIUAPSD : 0) |
		(parm->beuapsd ? FWCMD_H2C_LPS_PARM_BEUAPSD : 0) |
		(parm->bkuapsd ? FWCMD_H2C_LPS_PARM_BKUAPSD : 0) |
		SET_WORD(parm->lastrpwm, FWCMD_H2C_LPS_PARM_LASTRPWM) |
		(parm->bcnnohit_en ? FWCMD_H2C_LPS_PARM_BCNNOHIT_EN : 0) |
		(parm->nulltype ? FWCMD_H2C_LPS_PARM_NULLTYPE : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_LPS_PARM,
			      0,
			      1);
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

static void send_rpwm(struct mac_ax_adapter *adapter,
		      struct ps_rpwm_parm *parm)
{
	u16 rpwm_value = 0;
	u8 toggle = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	rpwm_value = MAC_REG_R16(R_AX_RPWM);
	if (0 == (rpwm_value & PS_RPWM_TOGGLE))
		toggle = 1;

	if (parm->notify_wake) {
		rpwm_value |= PS_RPWM_NOTIFY_WAKE;
	} else {
		if (rpwm_seq_num == RPWM_SEQ_NUM_MAX)
			rpwm_seq_num = 0;
		else
			rpwm_seq_num += 1;

		rpwm_value = (SET_WORD(parm->req_pwr_state, PS_RPWM_STATE) |
			SET_WORD(rpwm_seq_num, PS_RPWM_SEQ_NUM));

		if (parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_ACTIVE ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFON ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFON ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFOFF ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFOFF)
			rpwm_value |= PS_RPWM_ACK;

		if (parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_PWR_GATED ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_HIOE_PWR_GATED) {
			adapter->mac_pwr_info.pwr_in_lps = 1;
		}
	}

	if (toggle == 1)
		rpwm_value |= PS_RPWM_TOGGLE;
	else
		rpwm_value &= ~PS_RPWM_TOGGLE;

	switch (adapter->hw_info->intf) {
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			MAC_REG_W16(R_AX_USB_D2F_F2D_INFO + 2, rpwm_value);
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			MAC_REG_W16(R_AX_USB_D2F_F2D_INFO_V1 + 2, rpwm_value);
#endif
		break;
#endif //MAC_AX_USB_SUPPORT

#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			MAC_REG_W16(R_AX_SDIO_HRPWM1 + 2, rpwm_value);
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			MAC_REG_W16(R_AX_SDIO_HRPWM1_V1 + 2, rpwm_value);
#endif

		if (parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_PWR_GATED ||
		    parm->req_pwr_state == MAC_AX_RPWM_REQ_PWR_STATE_HIOE_PWR_GATED)
			adapter->sdio_info.tx_seq = 1;
		break;
#endif //MAC_AX_SDIO_SUPPORT

#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			MAC_REG_W16(R_AX_PCIE_HRPWM, rpwm_value);
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			MAC_REG_W16(R_AX_PCIE_HRPWM_V1, rpwm_value);
#endif

		break;
#endif //MAC_AX_PCIE_SUPPORT
	default:
		PLTFM_MSG_ERR("%s: invalid interface = %d!!\n",
			      __func__, adapter->hw_info->intf);

		break;
	}

	PLTFM_DELAY_US(RPWM_DELAY_FOR_32K_TICK);

	PLTFM_MSG_TRACE("Send RPWM. rpwm_val=0x%x\n", rpwm_value);
}

static u32 leave_lps(struct mac_ax_adapter *adapter, u8 macid)
{
	struct lps_parm h2c_lps_parm;
	u32 ret;

	PLTFM_MEMSET(&h2c_lps_parm, 0, sizeof(struct lps_parm));

	h2c_lps_parm.macid = macid;
	h2c_lps_parm.psmode = MAC_AX_PS_MODE_ACTIVE;
	h2c_lps_parm.lastrpwm = LAST_RPWM_ACTIVE;

	ret = send_h2c_lps_parm(adapter, &h2c_lps_parm);

	if (ret)
		return ret;

	lps_status[(macid >> 5)] &= ~BIT(macid & 0x1F);

	return MACSUCCESS;
}

static u32 enter_lps(struct mac_ax_adapter *adapter,
		     u8 macid,
		     struct mac_ax_lps_info *lps_info)
{
	struct lps_parm h2c_lps_parm;
	u32 ret = 0;

	if (!lps_info) {
		PLTFM_MSG_ERR("[ERR]:LPS info is null\n");
		return MACNOITEM;
	}

	if (lps_status[(macid >> 5)] & BIT(macid & 0x1F))
		ret = leave_lps(adapter, macid);

	if (ret)
		return ret;

	PLTFM_MEMSET(&h2c_lps_parm, 0, sizeof(struct lps_parm));

	h2c_lps_parm.macid = macid;
	h2c_lps_parm.psmode = MAC_AX_PS_MODE_LEGACY;

	if (lps_info->listen_bcn_mode > MAC_AX_RLBM_USERDEFINE)
		lps_info->listen_bcn_mode = MAC_AX_RLBM_MIN;

	if (lps_info->listen_bcn_mode == MAC_AX_RLBM_USERDEFINE) {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_USERDEFINE;
		h2c_lps_parm.awakeinterval = lps_info->awake_interval;
		if (h2c_lps_parm.awakeinterval == 0)
			h2c_lps_parm.awakeinterval = 1;
	} else if (lps_info->listen_bcn_mode == MAC_AX_RLBM_MAX) {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_MAX;
		h2c_lps_parm.awakeinterval = 1;
	} else {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_MIN;
		h2c_lps_parm.awakeinterval = 1;
	}

	h2c_lps_parm.smartps = lps_info->smart_ps_mode;
	h2c_lps_parm.lastrpwm = LAST_RPWM_PS;
	h2c_lps_parm.bcnnohit_en = lps_info->bcnnohit_en;
	h2c_lps_parm.nulltype = lps_info->nulltype;

	ret = send_h2c_lps_parm(adapter, &h2c_lps_parm);

	if (ret)
		return ret;

	lps_status[(macid >> 5)] |= BIT(macid & 0x1F);

	return MACSUCCESS;
}

static u32 enter_wmmlps(struct mac_ax_adapter *adapter,
			u8 macid,
			struct mac_ax_lps_info *lps_info)
{
	struct lps_parm h2c_lps_parm;
	u32 ret = 0;

	if (!lps_info) {
		PLTFM_MSG_ERR("[ERR]:LPS info is null\n");
		return MACNOITEM;
	}

	if (lps_status[(macid >> 5)] & BIT(macid & 0x1F))
		ret = leave_lps(adapter, macid);

	if (ret)
		return ret;

	PLTFM_MEMSET(&h2c_lps_parm, 0, sizeof(struct lps_parm));

	h2c_lps_parm.macid = macid;
	h2c_lps_parm.psmode = MAC_AX_PS_MODE_WMMPS;

	if (lps_info->listen_bcn_mode > MAC_AX_RLBM_USERDEFINE)
		lps_info->listen_bcn_mode = MAC_AX_RLBM_MIN;

	if (lps_info->listen_bcn_mode == MAC_AX_RLBM_USERDEFINE) {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_USERDEFINE;
		h2c_lps_parm.awakeinterval = lps_info->awake_interval;
		if (h2c_lps_parm.awakeinterval == 0)
			h2c_lps_parm.awakeinterval = 1;
	} else if (lps_info->listen_bcn_mode == MAC_AX_RLBM_MAX) {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_MAX;
		h2c_lps_parm.awakeinterval = 1;
	} else {
		h2c_lps_parm.rlbm = MAC_AX_RLBM_MIN;
		h2c_lps_parm.awakeinterval = 1;
	}

	h2c_lps_parm.lastrpwm = LAST_RPWM_PS;
	h2c_lps_parm.bcnnohit_en = lps_info->bcnnohit_en;
	h2c_lps_parm.vouapsd = lps_info->vouapsd_en;
	h2c_lps_parm.viuapsd = lps_info->viuapsd_en;
	h2c_lps_parm.beuapsd = lps_info->beuapsd_en;
	h2c_lps_parm.bkuapsd = lps_info->bkuapsd_en;

	ret = send_h2c_lps_parm(adapter, &h2c_lps_parm);

	if (ret)
		return ret;

	lps_status[(macid >> 5)] |= BIT(macid & 0x1F);

	return MACSUCCESS;
}

static u32 set_req_pwr_state(struct mac_ax_adapter *adapter,
			     enum mac_ax_rpwm_req_pwr_state req_pwr_state)
{
	struct ps_rpwm_parm parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct ps_rpwm_parm));

	if (req_pwr_state >= MAC_AX_RPWM_REQ_PWR_STATE_MAX) {
		PLTFM_MSG_ERR("%s: invalid pwr state:%d\n",
			      __func__, req_pwr_state);
		return MACNOITEM;
	}

	parm.req_pwr_state = req_pwr_state;
	parm.notify_wake = 0;
	send_rpwm(adapter, &parm);

	return MACSUCCESS;
}

static u32 _chk_cpwm_seq_num(u8 seq_num)
{
	u32 ret;

	if (cpwm_seq_num == CPWM_SEQ_NUM_MAX) {
		if (seq_num == 0) {
			cpwm_seq_num = seq_num;
			ret = MACSUCCESS;
		} else {
			ret = MACCPWMSEQERR;
		}
	} else {
		if (seq_num == (cpwm_seq_num + 1)) {
			cpwm_seq_num = seq_num;
			ret = MACSUCCESS;
		} else {
			ret = MACCPWMSEQERR;
		}
	}

	return ret;
}

static u32 chk_cur_pwr_state(struct mac_ax_adapter *adapter,
			     enum mac_ax_rpwm_req_pwr_state req_pwr_state)
{
	u16 cpwm = 0;
	u32 rpwm_32k;
	u32 req_32k;
#if MAC_AX_PCIE_SUPPORT
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;
#endif
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (req_pwr_state >= MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED)
		req_32k = 1;
	else
		req_32k = 0;

	//(workaround) CPWM register is in OFF area
	//Use LDM to check if FW receives RPWM
	rpwm_32k = (MAC_REG_R32(R_AX_LDM) & B_PS_LDM_32K_EN) >> B_PS_LDM_32K_EN_SH;
	if (req_32k != rpwm_32k)
		return MACCPWMPWRSTATERR;

	//There is no CPWM if 32K state
	if (req_32k)
		return MACSUCCESS;

#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		cpwm = MAC_REG_R16(R_AX_CPWM);
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		switch (adapter->hw_info->intf) {
#if MAC_AX_USB_SUPPORT
		case MAC_AX_INTF_USB:
			cpwm = MAC_REG_R16(R_AX_USB_D2F_F2D_INFO_V1);
			break;
#endif // MAC_AX_USB_SUPPORT

#if MAC_AX_SDIO_SUPPORT
		case MAC_AX_INTF_SDIO:
			return MACCPWMINTFERR;
			break;
#endif // MAC_AX_SDIO_SUPPORT

#if MAC_AX_PCIE_SUPPORT
		case MAC_AX_INTF_PCIE:
			cpwm = MAC_REG_R16(R_AX_PCIE_CRPWM);
			break;
#endif // MAC_AX_PCIE_SUPPORT
		default:
			PLTFM_MSG_ERR("%s: invalid interface = %d!!\n",
				      __func__, adapter->hw_info->intf);
			return MACCPWMINTFERR;
			break;
		}
	}
#endif

	PLTFM_MSG_TRACE("Read CPWM=0x%x\n", cpwm);
	if (rpwm_seq_num != GET_FIELD(cpwm, PS_CPWM_RSP_SEQ_NUM)) {
		PLTFM_MSG_TRACE("RPWM seq mismatch!!: expect val:%d, Rx val:%d\n",
				rpwm_seq_num, GET_FIELD(cpwm, PS_CPWM_RSP_SEQ_NUM));
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			return MACCPWMSEQERR;
		}
#endif
	}

	if (_chk_cpwm_seq_num(GET_FIELD(cpwm, PS_CPWM_SEQ_NUM)) == MACCPWMSEQERR) {
		PLTFM_MSG_TRACE("CPWM seq mismatch!!: expect val:%d, Rx val:%d\n",
				cpwm_seq_num, GET_FIELD(cpwm, PS_CPWM_SEQ_NUM));
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			return MACCPWMSEQERR;
		}
#endif
	}

	if (req_pwr_state != GET_FIELD(cpwm, PS_CPWM_STATE))
		return MACCPWMSTATERR;

	if (adapter->mac_pwr_info.pwr_in_lps) {
		adapter->mac_pwr_info.pwr_in_lps = 0;
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
#if MAC_AX_PCIE_SUPPORT
			if (adapter->hw_info->intf == MAC_AX_INTF_PCIE) {
				ret = p_ops->sync_trx_bd_idx(adapter);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("sync trx bd fail: %d\n",
						      ret);
					return ret;
				}
			}
#endif
		}
#endif
	}

	return MACSUCCESS;
}

u32 mac_cfg_lps(struct mac_ax_adapter *adapter, u8 macid,
		enum mac_ax_ps_mode ps_mode, struct mac_ax_lps_info *lps_info)
{
	u32 ret = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (ps_mode) {
	case MAC_AX_PS_MODE_ACTIVE:
		ret = leave_lps(adapter, macid);
		/* patch form BT BG/LDO issue */
		MAC_REG_W8(R_AX_SCOREBOARD + 3, MAC_AX_NOTIFY_TP_MAJOR);
		break;
	case MAC_AX_PS_MODE_LEGACY:
		ret = enter_lps(adapter, macid, lps_info);
		/* patch form BT BG/LDO issue */
		MAC_REG_W8(R_AX_SCOREBOARD + 3, MAC_AX_NOTIFY_PWR_MAJOR);
		break;
	case MAC_AX_PS_MODE_WMMPS:
		ret = enter_wmmlps(adapter, macid, lps_info);
		/* patch form BT BG/LDO issue */
		MAC_REG_W8(R_AX_SCOREBOARD + 3, MAC_AX_NOTIFY_PWR_MAJOR);
		break;
	default:
		return MACNOITEM;
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_ps_pwr_state(struct mac_ax_adapter *adapter,
		     enum mac_ax_pwr_state_action action,
		     enum mac_ax_rpwm_req_pwr_state req_pwr_state)
{
	u32 ret = MACSUCCESS;

	switch (action) {
	case MAC_AX_PWR_STATE_ACT_REQ:
		ret = set_req_pwr_state(adapter, req_pwr_state);
		break;
	case MAC_AX_PWR_STATE_ACT_CHK:
		ret = chk_cur_pwr_state(adapter, req_pwr_state);
		break;
	default:
		ret = MACNOITEM;
	}

	return ret;
}

u32 mac_chk_leave_lps(struct mac_ax_adapter *adapter, u8 macid)
{
	u8 band = 0;
	u8 port = 0;
	u32 chk_msk = 0;
	struct mac_role_tbl *role;
	u16 pwrbit_set_reg[2] = {R_AX_PPWRBIT_SETTING, R_AX_PPWRBIT_SETTING_C1};
	u32 pwr_mgt_en_bit = 0xE;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_pause = 0;
	u32 reg_sleep = 0;
	u8 macid_grp = macid >> MACID_GRP_SH;
	u8 macid_sh = macid & MACID_GRP_MASK;

	role = mac_role_srch(adapter, macid);

	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", macid);
		return MACNOITEM;
	}

	band = role->info.a_info.bb_sel;
	port = role->info.a_info.port_int;

	chk_msk = pwr_mgt_en_bit << (PORT_SH * port);
	switch (macid_grp) {
	case MACID_GRP_0:
		reg_sleep = R_AX_MACID_SLEEP_0;
		reg_pause = R_AX_SS_MACID_PAUSE_0;
		break;
	case MACID_GRP_1:
		reg_sleep = R_AX_MACID_SLEEP_1;
		reg_pause = R_AX_SS_MACID_PAUSE_1;
		break;
	case MACID_GRP_2:
		reg_sleep = R_AX_MACID_SLEEP_2;
		reg_pause = R_AX_SS_MACID_PAUSE_2;
		break;
	case MACID_GRP_3:
		reg_sleep = R_AX_MACID_SLEEP_3;
		reg_pause = R_AX_SS_MACID_PAUSE_3;
		break;
	default:
		return MACPSSTATFAIL;
	}

	// Bypass Tx pause check during STOP SER period
	if (adapter->sm.ser_ctrl_st != MAC_AX_SER_CTRL_STOP)
		if (MAC_REG_R32(reg_pause) & BIT(macid_sh))
			return MACPSSTATFAIL;

	if ((MAC_REG_R32(reg_sleep) & BIT(macid_sh)))
		return MACPSSTATFAIL;

	if ((MAC_REG_R32(pwrbit_set_reg[band]) & chk_msk))
		return MACPSSTATPWRBITFAIL;

	return MACSUCCESS;
}

u8 _is_in_lps(struct mac_ax_adapter *adapter)
{
	u8 i;

	for (i = 0; i < 4; i++) {
		if (lps_status[i] != 0)
			return 1;
	}

	return 0;
}

void reset_lps_seq_num(struct mac_ax_adapter *adapter)
{
	rpwm_seq_num = RPWM_SEQ_NUM_MAX;
	cpwm_seq_num = CPWM_SEQ_NUM_MAX;
}

static u32 send_h2c_ips_cfg(struct mac_ax_adapter *adapter,
			    struct ips_cfg *cfg)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_ips_cfg *fwcmd_ips;
	u32 ret;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ips_cfg));
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	fwcmd_ips = (struct fwcmd_ips_cfg *)buf;
	fwcmd_ips->dword0 =
	cpu_to_le32(SET_WORD(cfg->macid, FWCMD_H2C_IPS_CFG_MACID) |
		(cfg->enable ? FWCMD_H2C_IPS_CFG_ENABLE : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_IPS_CFG,
			      0,
			      1);
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
}

static u32 leave_ips(struct mac_ax_adapter *adapter, u8 macid)
{
	struct ips_cfg h2c_ips_cfg;
	u32 ret;

	PLTFM_MEMSET(&h2c_ips_cfg, 0, sizeof(struct ips_cfg));

	h2c_ips_cfg.macid = macid;
	h2c_ips_cfg.enable = 0;

	ret = send_h2c_ips_cfg(adapter, &h2c_ips_cfg);
	if (ret != MACSUCCESS)
		return ret;

	ips_status[(macid >> 5)] &= ~BIT(macid & 0x1F);

	return MACSUCCESS;
}

static u32 enter_ips(struct mac_ax_adapter *adapter,
		     u8 macid)
{
	struct ips_cfg h2c_ips_cfg;
	u32 ret;

	if (ips_status[(macid >> 5)] & BIT(macid & 0x1F)) {
		PLTFM_MSG_ERR("[ERR]:IPS info is null\n");
		ret = leave_ips(adapter, macid);
		if (ret != MACSUCCESS)
			return ret;
	}

	PLTFM_MEMSET(&h2c_ips_cfg, 0, sizeof(struct ips_cfg));

	h2c_ips_cfg.macid = macid;
	h2c_ips_cfg.enable = 1;

	ret = send_h2c_ips_cfg(adapter, &h2c_ips_cfg);
	if (ret != MACSUCCESS)
		return ret;

	ips_status[(macid >> 5)] |= BIT(macid & 0x1F);

	return MACSUCCESS;
}

u32 mac_cfg_ips(struct mac_ax_adapter *adapter, u8 macid,
		u8 enable)
{
	u32 ret;

	if (enable == 1)
		ret = enter_ips(adapter, macid);
	else
		ret = leave_ips(adapter, macid);

	return ret;
}

u32 mac_chk_leave_ips(struct mac_ax_adapter *adapter, u8 macid)
{
	u8 band = 0;
	u8 port = 0;
	u32 chk_msk = 0;
	struct mac_role_tbl *role;
	u32 pwr_mgt_en_bit = 0xE;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg_pause = 0;
	u32 reg_sleep = 0;
	u8 macid_grp = macid >> MACID_GRP_SH;
	u8 macid_sh = macid & MACID_GRP_MASK;

	role = mac_role_srch(adapter, macid);

	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", macid);
		return MACNOITEM;
	}

	band = role->info.a_info.bb_sel;
	port = role->info.a_info.port_int;

	chk_msk = pwr_mgt_en_bit << (PORT_SH * port);
	switch (macid_grp) {
	case MACID_GRP_0:
		reg_sleep = R_AX_MACID_SLEEP_0;
		reg_pause = R_AX_SS_MACID_PAUSE_0;
		break;
	case MACID_GRP_1:
		reg_sleep = R_AX_MACID_SLEEP_1;
		reg_pause = R_AX_SS_MACID_PAUSE_1;
		break;
	case MACID_GRP_2:
		reg_sleep = R_AX_MACID_SLEEP_2;
		reg_pause = R_AX_SS_MACID_PAUSE_2;
		break;
	case MACID_GRP_3:
		reg_sleep = R_AX_MACID_SLEEP_3;
		reg_pause = R_AX_SS_MACID_PAUSE_3;
		break;
	default:
		return MACPSSTATFAIL;
	}

	// Bypass Tx pause check during STOP SER period
	if (adapter->sm.ser_ctrl_st != MAC_AX_SER_CTRL_STOP)
		if (MAC_REG_R32(reg_pause) & BIT(macid_sh))
			return MACPSSTATFAIL;

	if (MAC_REG_R32(reg_sleep) & BIT(macid_sh))
		return MACPSSTATFAIL;

	return MACSUCCESS;
}

u8 _is_in_ips(struct mac_ax_adapter *adapter)
{
	u8 i;

	for (i = 0; i < 4; i++) {
		if (ips_status[i] != 0)
			return 1;
	}

	return 0;
}

u32 mac_ps_notify_wake(struct mac_ax_adapter *adapter)
{
	struct ps_rpwm_parm parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct ps_rpwm_parm));

	if (adapter->mac_pwr_info.pwr_in_lps == 0) {
		PLTFM_MSG_ERR("%s: Not in power saving!\n", __func__);
		return MACPWRSTAT;
	}

	parm.notify_wake = 1;
	send_rpwm(adapter, &parm);

	return MACSUCCESS;
}

u32 mac_cfg_ps_advance_parm(struct mac_ax_adapter *adapter,
			    struct mac_ax_ps_adv_parm *parm)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_ps_advance_parm *fwcmd_parm;
	u32 ret;

	if (!parm)
		return MACBADDR;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	PLTFM_MSG_ALWAYS("%s: MACID(%d), TRXTimeOutTimeSet(%d), TRXTimeOutTimeVal(%d)!\n",
			 __func__, parm->macid, parm->trxtimeouttimeset, parm->trxtimeouttimeval);
	PLTFM_MSG_ALWAYS("%s: EnSmartPsDtimRx(%d)!\n", __func__, parm->ensmartpsdtimrx);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ps_advance_parm));
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	fwcmd_parm = (struct fwcmd_ps_advance_parm *)buf;
	fwcmd_parm->dword0 =
	cpu_to_le32(SET_WORD(parm->macid, FWCMD_H2C_PS_ADVANCE_PARM_MACID) |
		    SET_WORD(parm->trxtimeouttimeset, FWCMD_H2C_PS_ADVANCE_PARM_TRXTIMEOUTTIMESET) |
		    (parm->ensmartpsdtimrx ? FWCMD_H2C_PS_ADVANCE_PARM_ENSMARTPSDTIMRX : 0));
	fwcmd_parm->dword1 =
	cpu_to_le32(SET_WORD(parm->trxtimeouttimeval, FWCMD_H2C_PS_ADVANCE_PARM_TRXTIMEOUTTIMEVAL));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_PS_ADVANCE_PARM,
			      0,
			      1);
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
}

static u32 send_h2c_pw_cfg(struct mac_ax_adapter *adapter,
			   struct periodic_wake_cfg *parm)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_periodic_wake *fwcmd_pw;
	u32 ret = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_periodic_wake));

	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_pw = (struct fwcmd_periodic_wake *)buf;
	fwcmd_pw->dword0 =
	cpu_to_le32(SET_WORD(parm->macid, FWCMD_H2C_PERIODIC_WAKE_MACID) |
		(parm->enable ? FWCMD_H2C_PERIODIC_WAKE_ENABLE : 0) |
		(parm->band ? FWCMD_H2C_PERIODIC_WAKE_BAND : 0) |
		SET_WORD(parm->port, FWCMD_H2C_PERIODIC_WAKE_PORT));
	fwcmd_pw->dword1 = cpu_to_le32(parm->wake_period);
	fwcmd_pw->dword2 = cpu_to_le32(parm->wake_duration);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_PERIODIC_WAKE,
			      0,
			      1);
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

u32 mac_periodic_wake_cfg(struct mac_ax_adapter *adapter,
			  struct mac_ax_periodic_wake_info pw_info)
{
	u32 ret = MACSUCCESS;
	struct periodic_wake_cfg parm;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MEMSET(&parm, 0, sizeof(struct periodic_wake_cfg));
	parm.macid = pw_info.macid;
	parm.enable = pw_info.enable;
	parm.band = pw_info.band;
	parm.port = pw_info.port;
	parm.wake_period = pw_info.wake_period;
	parm.wake_duration = pw_info.wake_duration;

	PLTFM_MSG_ALWAYS("%s: macid(%d), enable(%d), band(%d), port(%d)\n",
			 __func__, parm.macid, parm.enable, parm.band, parm.port);
	PLTFM_MSG_ALWAYS("%s: wake_period(%d), wake_duration(%d)\n",
			 __func__, parm.wake_period, parm.wake_duration);

	ret = send_h2c_pw_cfg(adapter, &parm);
	if (ret)
		return ret;

	return ret;
}

u32 send_h2c_req_pwr_state(struct mac_ax_adapter *adapter,
			   struct req_pwr_state_cfg *parm)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_bb_rf_pwr_st *fwcmd_req_pwr_st;
	u32 ret = 0;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_bb_rf_pwr_st));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_req_pwr_st = (struct fwcmd_bb_rf_pwr_st *)buf;
	fwcmd_req_pwr_st->dword0 =
	cpu_to_le32(SET_WORD(parm->req_pwr_state, FWCMD_H2C_BB_RF_PWR_ST_BB_RF_PWR_REQ_ST));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_BB_RF_PWR_ST,
			      0,
			      1);
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

u32 set_pwr_st_cfg(struct mac_ax_adapter *adapter,
		   struct req_pwr_state_cfg *parm)
{
	u32 ret = MACSUCCESS;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (parm->req_pwr_state) {
	case REQ_PWR_ST_ADC_OFF:
		MAC_REG_W8(R_AX_ANAPAR_POW_MAC, (MAC_REG_R8(R_AX_ANAPAR_POW_MAC) &
			   ~(B_AX_POW_PC_LDO_PORT0 | B_AX_POW_PC_LDO_PORT1)));
		break;
	case REQ_PWR_ST_BB_OFF:
		MAC_REG_W8(R_AX_SYS_FUNC_EN, (MAC_REG_R8(R_AX_SYS_FUNC_EN) &
			   ~(B_AX_FEN_BBRSTB | B_AX_FEN_BB_GLB_RSTN)));
		MAC_REG_W8(R_AX_SYS_ISO_CTRL_EXTEND, (MAC_REG_R8(R_AX_SYS_ISO_CTRL_EXTEND) |
			   B_AX_R_SYM_ISO_BB2PP));
		MAC_REG_W16(R_AX_AFE_LDO_CTRL, (MAC_REG_R16(R_AX_AFE_LDO_CTRL) &
			    ~(B_AX_R_SYM_WLBBOFF_PC_EN | B_AX_R_SYM_WLBBOFF_P1_PC_EN |
			    B_AX_R_SYM_WLBBOFF_P2_PC_EN | B_AX_R_SYM_WLBBOFF_P3_PC_EN |
			    B_AX_R_SYM_WLBBOFF_P4_PC_EN | B_AX_R_SYM_WLBBOFF1_P1_PC_EN |
			    B_AX_R_SYM_WLBBOFF1_P2_PC_EN | B_AX_R_SYM_WLBBOFF1_P3_PC_EN |
			    B_AX_R_SYM_WLBBOFF1_P4_PC_EN)));
		break;
	case REQ_PWR_ST_CPU_OFF:
		MAC_REG_W8(R_AX_PLATFORM_ENABLE, (MAC_REG_R8(R_AX_PLATFORM_ENABLE) &
			   ~B_AX_WCPU_EN));
		MAC_REG_W16(R_AX_SYS_CLK_CTRL, (MAC_REG_R16(R_AX_SYS_CLK_CTRL) &
			    ~(B_AX_CPU_CLK_EN | B_AX_CPU_IDMEM_CLK_EN)));
		MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, (MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) |
			    B_AX_R_SYM_ISO_IMEM02PP | B_AX_R_SYM_ISO_IMEM12PP |
			    B_AX_R_SYM_ISO_IMEM22PP | B_AX_R_SYM_ISO_IMEM32PP |
			    B_AX_R_SYM_ISO_IMEM42PP | B_AX_R_SYM_ISO_DMEM12PP_V1 |
			    B_AX_R_SYM_ISO_DMEM22PP_V1 | B_AX_R_SYM_ISO_DMEM32PP_V1 |
			    B_AX_R_SYM_ISO_DMEM42PP | B_AX_R_SYM_ISO_DMEM52PP |
			    B_AX_R_SYM_ISO_DMEM62PP));
		MAC_REG_W32(R_AX_AFE_CTRL1, (MAC_REG_R32(R_AX_AFE_CTRL1) &
			    ~(B_AX_IMEM0_PC_EN | B_AX_IMEM1_PC_EN | B_AX_IMEM2_PC_EN |
			    B_AX_IMEM3_PC_EN | B_AX_IMEM4_PC_EN | B_AX_DMEM1_PC_EN |
			    B_AX_DMEM2_PC_EN | B_AX_DMEM3_PC_EN | B_AX_DMEM4_PC_EN |
			    B_AX_DMEM5_PC_EN | B_AX_DMEM6_PC_EN)));
		break;
	case REQ_PWR_ST_MAC_OFF:
		MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND, (MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) &
			    ~B_AX_R_SYM_FEN_WLMACOFF));
		MAC_REG_W16(R_AX_SYS_CLK_CTRL, (MAC_REG_R16(R_AX_SYS_CLK_CTRL) & ~B_AX_MAC_CLK_EN));
		MAC_REG_W8(R_AX_PMC_DBG_CTRL2, (MAC_REG_R8(R_AX_PMC_DBG_CTRL2) |
			   B_AX_SYSON_DIS_PMCR_AX_WRMSK));
		MAC_REG_W8(R_AX_SYS_ISO_CTRL, (MAC_REG_R8(R_AX_SYS_ISO_CTRL) | B_AX_ISO_WLPON2PP));
		MAC_REG_W8(R_AX_SYS_ISO_CTRL, (MAC_REG_R8(R_AX_SYS_ISO_CTRL) | B_AX_ISO_WD2PP));
		MAC_REG_W32(R_AX_AFE_LDO_CTRL, (MAC_REG_R32(R_AX_AFE_LDO_CTRL) &
			    ~(B_AX_R_SYM_WLPOFF_PC_EN | B_AX_R_SYM_WLPOFF_P1_PC_EN |
			    B_AX_R_SYM_WLPOFF_P2_PC_EN | B_AX_R_SYM_WLPOFF_P3_PC_EN |
			    B_AX_R_SYM_WLPOFF_P4_PC_EN)));
		MAC_REG_W32(R_AX_AFE_LDO_CTRL, (MAC_REG_R32(R_AX_AFE_LDO_CTRL) &
			    ~(B_AX_R_SYM_WLPON_PC_EN | B_AX_R_SYM_WLPON_P1_PC_EN |
			    B_AX_R_SYM_WLPON_P2_PC_EN | B_AX_R_SYM_WLPON_P3_PC_EN)));
		break;
	case REQ_PWR_ST_PLL_OFF:
		MAC_REG_W8(R_AX_ANAPAR_POW_MAC, (MAC_REG_R8(R_AX_ANAPAR_POW_MAC) &
			   ~(B_AX_POW_POWER_CUT_POW_LDO | B_AX_POW_PLL_V1)));
		break;
	case REQ_PWR_ST_SWRD_OFF:
		MAC_REG_W8(R_AX_ANAPARLDO_POW_MAC, (MAC_REG_R8(R_AX_ANAPARLDO_POW_MAC) &
			   ~B_AX_POW_SW_SPSDIG));
		break;
	case REQ_PWR_ST_XTAL_OFF:
		MAC_REG_W32(R_AX_WLAN_XTAL_SI_CTRL, REQ_PWR_ST_XTAL_OFF_VAL);
		break;
	case REQ_PWR_ST_ADIE_OFF:
		MAC_REG_W32(R_AX_SYM_ANAPAR_XTAL_MODE_DECODER,
			    (MAC_REG_R32(R_AX_SYM_ANAPAR_XTAL_MODE_DECODER) |
			    B_AX_ADIE_CTRL_BY_SW));
		MAC_REG_W32(R_AX_SYM_ANAPAR_XTAL_MODE_DECODER,
			    (MAC_REG_R32(R_AX_SYM_ANAPAR_XTAL_MODE_DECODER) & ~B_AX_ADIE_EN));
		break;
	case REQ_PWR_ST_BYPASS_DATA_ON:
		MAC_REG_W32(R_AX_RCR, (MAC_REG_R32(R_AX_RCR) | B_AX_STOP_RX_IN));
		if ((MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) & B_AX_CMAC1_FEN) == B_AX_CMAC1_FEN)
			MAC_REG_W32(R_AX_RCR_C1, (MAC_REG_R32(R_AX_RCR_C1) | B_AX_STOP_RX_IN));
		break;
	default:
		return MACNOITEM;
	}

	return ret;
}

u32 mac_req_pwr_state_cfg(struct mac_ax_adapter *adapter,
			  enum mac_req_pwr_st req_pwr_st)
{
	u32 ret = MACSUCCESS;
	struct req_pwr_state_cfg parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct req_pwr_state_cfg));
	parm.req_pwr_state = req_pwr_st;

	switch (req_pwr_st) {
	case REQ_PWR_ST_OPEN_RF:
		ret = send_h2c_req_pwr_state(adapter, &parm);
		break;
	case REQ_PWR_ST_CLOSE_RF:
		ret = send_h2c_req_pwr_state(adapter, &parm);
		break;
	case REQ_PWR_ST_ADC_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_BB_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_CPU_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_MAC_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_PLL_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_SWRD_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_XTAL_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_ADIE_OFF:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	case REQ_PWR_ST_BYPASS_DATA_ON:
		ret = set_pwr_st_cfg(adapter, &parm);
		break;
	default:
		return MACNOITEM;
	}

	return ret;
}

u32 send_h2c_req_pwr_lvl(struct mac_ax_adapter *adapter,
			 struct req_pwr_lvl_cfg *parm)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_ps_power_level *fwcmd_req_pwr_lvl;
	u32 ret = 0;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ps_power_level));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_req_pwr_lvl = (struct fwcmd_ps_power_level *)buf;
	fwcmd_req_pwr_lvl->dword0 =
	cpu_to_le32(SET_WORD(parm->macid, FWCMD_H2C_PS_POWER_LEVEL_MACID) |
		    SET_WORD(parm->bcn_to_val, FWCMD_H2C_PS_POWER_LEVEL_BCN_TO_VAL) |
		    SET_WORD(parm->ps_lvl, FWCMD_H2C_PS_POWER_LEVEL_PS_LVL) |
		    SET_WORD(parm->trx_lvl, FWCMD_H2C_PS_POWER_LEVEL_TRX_LVL));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_PS_POWER_LEVEL,
			      0,
			      1);
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

u32 mac_req_pwr_lvl_cfg(struct mac_ax_adapter *adapter,
			struct mac_ax_req_pwr_lvl_info *pwr_lvl_info)
{
	u32 ret = MACSUCCESS;
	struct req_pwr_lvl_cfg parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct req_pwr_lvl_cfg));

	parm.macid = pwr_lvl_info->macid;

	if (pwr_lvl_info->ps_lvl >= REQ_PS_LVL_MAX ||
	    pwr_lvl_info->trx_lvl >= REQ_TRX_LVL_MAX ||
	    pwr_lvl_info->bcn_to_lvl >= REQ_BCN_TO_LVL_MAX)
		return MACNOITEM;

	if (pwr_lvl_info->bcn_to_val == REQ_BCN_TO_VAL_NONVALID)
		pwr_lvl_info->bcn_to_val = REQ_BCN_TO_VAL_MIN << pwr_lvl_info->bcn_to_lvl;
	else if (pwr_lvl_info->bcn_to_val < REQ_BCN_TO_VAL_MIN)
		pwr_lvl_info->bcn_to_val = REQ_BCN_TO_VAL_MIN;
	else if (pwr_lvl_info->bcn_to_val > REQ_BCN_TO_VAL_MAX)
		pwr_lvl_info->bcn_to_val = REQ_BCN_TO_VAL_MAX;

	parm.bcn_to_val = pwr_lvl_info->bcn_to_val;
	parm.ps_lvl = pwr_lvl_info->ps_lvl;
	parm.trx_lvl = pwr_lvl_info->trx_lvl;

	ret = send_h2c_req_pwr_lvl(adapter, &parm);

	return ret;
}

u32 send_h2c_lps_option_cfg(struct mac_ax_adapter *adapter,
			    struct lps_option_cfg *parm)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_lps_option_cfg *fwcmd_lps_option;
	u32 ret = 0;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_lps_option_cfg));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	fwcmd_lps_option = (struct fwcmd_lps_option_cfg *)buf;
	fwcmd_lps_option->dword0 =
	cpu_to_le32(parm->req_lps_option ? FWCMD_H2C_LPS_OPTION_CFG_REQ_XTAL_OPTION : 0);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_LPS_OPTION_CFG,
			      0,
			      1);
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

u32 mac_lps_option_cfg(struct mac_ax_adapter *adapter,
		       struct mac_lps_option *lps_opt)
{
	u32 ret = MACSUCCESS;
	struct lps_option_cfg parm;

	PLTFM_MEMSET(&parm, 0, sizeof(struct lps_option_cfg));

#if MAC_AX_USB_SUPPORT
	if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
		parm.req_lps_option = lps_opt->req_xtal_option;
		ret = send_h2c_lps_option_cfg(adapter, &parm);
	}
#endif //MAC_AX_USB_SUPPORT
	return ret;
}
