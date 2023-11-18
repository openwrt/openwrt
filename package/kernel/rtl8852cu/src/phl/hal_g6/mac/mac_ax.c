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

#include "mac_ax.h"
#include "mac_ax/mac_priv.h"

#define CHIP_ID_HW_DEF_8852A	0x50
#define CHIP_ID_HW_DEF_8852B	0x51
#define CHIP_ID_HW_DEF_8852C	0x52
#define CHIP_ID_HW_DEF_8192XB	0x53
#define CHIP_ID_HW_DEF_8851B	0x54
#define CHIP_ID_HW_DEF_8851E	0x55
#define CHIP_ID_HW_DEF_8852D	0x56
#define CHIP_ID_HW_DEF_1115E	0x70

#define PID_HW_DEF_8852AS	0xA852
#define PID_HW_DEF_8852BS	0xB852
#define PID_HW_DEF_8852BSA	0xB85B
#define PID_HW_DEF_8851ASA	0x8851
#define PID_HW_DEF_8852BPS	0xA85C
#define PID_HW_DEF_8852CS	0xC852
#define PID_HW_DEF_8192XBS	0x0192
#define PID_HW_DEF_8851BS	0xB851
#define PID_HW_DEF_8851BSM	0xB51A
#define PID_HW_DEF_8851ES	0x851E
#define PID_HW_DEF_8852DS	0xD852
#define PID_HW_DEF_1115ES	0x892A

#define SDIO_FN0_CIS_PID	0x1004

#define SDIO_WAIT_CNT		50
#define MAC_AX_INDIR_CNT	23

#if MAC_AX_SDIO_SUPPORT
static u8 r8_indir_cmd52_sdio(void *drv_adapter,
			      struct mac_ax_pltfm_cb *pltfm_cb, u32 adr);
#endif

#ifndef CONFIG_NEW_HALMAC_INTERFACE
static u8 chk_pltfm_cb(void *drv_adapter, enum mac_ax_intf intf,
		       struct mac_ax_pltfm_cb *pltfm_cb)
{
	if (!pltfm_cb)
		return MACSUCCESS;

	if (!pltfm_cb->msg_print)
		return MACSUCCESS;

#if MAC_AX_SDIO_SUPPORT
	if (!pltfm_cb->sdio_cmd52_r8 || !pltfm_cb->sdio_cmd53_r8 ||
	    !pltfm_cb->sdio_cmd53_r16 || !pltfm_cb->sdio_cmd53_r32 ||
	    !pltfm_cb->sdio_cmd53_rn || !pltfm_cb->sdio_cmd52_w8 ||
	    !pltfm_cb->sdio_cmd53_w8 || !pltfm_cb->sdio_cmd53_w16 ||
	    !pltfm_cb->sdio_cmd53_w32 || !pltfm_cb->sdio_cmd53_wn ||
	    !pltfm_cb->sdio_cmd52_cia_r8) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "[ERR]CB-SDIO\n");
		return MACSUCCESS;
	}
#endif

#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	if (!pltfm_cb->reg_r8 || !pltfm_cb->reg_r16 ||
	    !pltfm_cb->reg_r32 || !pltfm_cb->reg_w8 ||
	    !pltfm_cb->reg_w16 || !pltfm_cb->reg_w32) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "[ERR]CB-USB or PCIE\n");
		return MACSUCCESS;
	}
#endif
	if (!pltfm_cb->rtl_free || !pltfm_cb->rtl_malloc ||
	    !pltfm_cb->rtl_memcpy || !pltfm_cb->rtl_memset ||
	    !pltfm_cb->rtl_delay_us || !pltfm_cb->rtl_delay_ms ||
	    !pltfm_cb->rtl_sleep_us || !pltfm_cb->rtl_sleep_ms ||
	    !pltfm_cb->rtl_mutex_init || !pltfm_cb->rtl_mutex_deinit ||
	    !pltfm_cb->rtl_mutex_lock || !pltfm_cb->rtl_mutex_unlock ||
	    !pltfm_cb->event_notify || !pltfm_cb->get_chip_id) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "[ERR]CB-OS\n");
		return MACSUCCESS;
	}

	return MACPFCB;
}
#endif /*CONFIG_NEW_HALMAC_INTERFACE*/

static u8 chk_pltfm_endian(void)
{
	u32 num = 1;
	u8 *num_ptr = (u8 *)&num;

	if (*num_ptr != PLATFOM_IS_LITTLE_ENDIAN)
		return MACSUCCESS;

	return MACPFCB;
}

static u8 get_analog_info(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	u32 ret;
	u8 xtal_si_val;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		ret = mac_read_xtal_si(adapter, XTAL_SI_CV, &xtal_si_val);
		if (ret)
			PLTFM_MSG_ERR("Read XTAL_SI fail!\n");

		xtal_si_val = (xtal_si_val & acv_mask);
		adapter->hw_info->acv = xtal_si_val;
	}
#endif

	return MACSUCCESS;
}

#ifdef CONFIG_NEW_HALMAC_INTERFACE
#if MAC_AX_SDIO_SUPPORT
static u8 r8_indir_cmd52_sdio(void *drv_adapter,
			      struct mac_ax_pltfm_cb *pltfm_cb, u32 adr)
{
	u8 tmp;
	u32 cnt;

	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR, (u8)adr);
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR + 1, (u8)(adr >> 8));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR + 2, (u8)(adr >> 16));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_ADDR + 3,
			    (u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	PLTFM_SDIO_CMD52_W8(R_AX_SDIO_INDIRECT_CTRL, (u8)B_AX_INDIRECT_REG_R);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_ADDR + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		PLTFM_MSG_ERR("[ERR]sdio indirect CMD52 read\n");

	return PLTFM_SDIO_CMD52_R8(R_AX_SDIO_INDIRECT_DATA);
}

static u8 get_chip_id_hw_def_sdio(void *drv_adapter,
				  struct mac_ax_pltfm_cb *pltfm_cb, u32 adr)
{
	u16 pid;

	pid = PLTFM_SDIO_CMD52_CIA_R8(SDIO_FN0_CIS_PID) |
	      (PLTFM_SDIO_CMD52_CIA_R8(SDIO_FN0_CIS_PID + 1) << 8);

	switch (pid) {
	case PID_HW_DEF_8852AS:
		return CHIP_ID_HW_DEF_8852A;
	case PID_HW_DEF_8852BS:
	case PID_HW_DEF_8852BSA:
	case PID_HW_DEF_8851ASA:
	case PID_HW_DEF_8852BPS:
		return CHIP_ID_HW_DEF_8852B;
	case PID_HW_DEF_8852CS:
		return CHIP_ID_HW_DEF_8852C;
	case PID_HW_DEF_8192XBS:
		return CHIP_ID_HW_DEF_8192XB;
	case PID_HW_DEF_8851BS:
	case PID_HW_DEF_8851BSM:
		return CHIP_ID_HW_DEF_8851B;
	case PID_HW_DEF_8851ES:
		return CHIP_ID_HW_DEF_8851E;
	case PID_HW_DEF_8852DS:
		return CHIP_ID_HW_DEF_8852D;
	case PID_HW_DEF_1115ES:
		return CHIP_ID_HW_DEF_1115E;
	default:
		PLTFM_MSG_WARN("[WARN]read sdio local PID fail\n");
		break;
	}

	return r8_dir_cmd52_sdio(drv_adapter, adr);
}

#endif
static bool chk_get_chip_info(u8 chip_id, u8 cv)
{
	switch (chip_id) {
	case MAC_AX_CHIP_ID_8852A:
		switch (cv) {
		case CAV:
			return PATCH_ENABLE;
		case CBV:
			return PATCH_ENABLE;
		case CCV:
			return PATCH_DISABLE;
		case CDV:
			//fall through
		default:
			return PATCH_DISABLE;
	}
	default:
		return PATCH_DISABLE;
	}
}

static u32 get_chip_info(struct mac_ax_adapter *adapter,
			 struct mac_ax_pltfm_cb *pltfm_cb,
			 enum mac_ax_intf intf, u8 *id, u8 *cv)
{
	u32 cv_temp;
	u8 cur_id;
	u32 ret;

	if (!cv || !id)
		return MACNPTR;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		cur_id = r8_indir_cmd52_sdio(adapter, R_AX_SYS_CHIPINFO);
		*cv = r8_indir_cmd52_sdio(adapter, R_AX_SYS_CFG1 + 1) >> 4;
		break;
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	case MAC_AX_INTF_USB:
	case MAC_AX_INTF_PCIE:
		cur_id = PLTFM_REG_R8(R_AX_SYS_CHIPINFO);
		*cv = PLTFM_REG_R8(R_AX_SYS_CFG1 + 1) >> 4;

		if (chk_get_chip_info(cur_id, *cv) == PATCH_ENABLE) {
			cv_temp = PLTFM_REG_R32(R_AX_GPIO0_7_FUNC_SEL);
			if (cv_temp == 0xdeadbeef)
				*cv = CAV;
			else
				*cv = CBV;
		}
		break;
#endif
	default:
		return MACINTF;
	}

	ret = xlat_chip_id(cur_id, id);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 mac_ax_ops_init_v1(void *phl_adapter, void *drv_adapter,
		       enum rtw_chip_id chip_id,
		       enum rtw_hci_type hci,
		       struct mac_ax_adapter **mac_adapter,
		       struct mac_ax_ops **mac_ops)
{
	u32 ret;
	u8 cv;
	struct mac_ax_adapter *adapter;
	enum mac_ax_intf intf = MAC_AX_INTF_INVALID;

	if (!chk_pltfm_endian())
		return MACPFED;

	ret = 0;

	if (hci == RTW_HCI_PCIE)
		intf = MAC_AX_INTF_PCIE;
	else if (hci == RTW_HCI_USB)
		intf = MAC_AX_INTF_USB;
	else if (hci == RTW_HCI_SDIO)
		intf = MAC_AX_INTF_SDIO;

	ret = get_chip_info(drv_adapter, NULL, intf, &chip_id, &cv);
	if (ret)
		return ret;

	adapter = get_mac_ax_adapter(intf, chip_id, cv, phl_adapter,
				     drv_adapter, NULL);
	if (!adapter) {
		PLTFM_MSG_PRINT("[ERR]Get MAC adapter\n");
		return MACADAPTER;
	}
	PLTFM_MSG_ALWAYS("MAC_AX_MAJOR_VER = %d\n"
			    "MAC_AX_PROTOTYPE_VER = %d\n"
			    "MAC_AX_SUB_VER = %d\n"
			    "MAC_AX_SUB_INDEX = %d\n",
			    MAC_AX_MAJOR_VER, MAC_AX_PROTOTYPE_VER,
			    MAC_AX_SUB_VER, MAC_AX_SUB_INDEX);

	*mac_adapter = adapter;
	*mac_ops = adapter->ops;

	return MACSUCCESS;
}

#else

#if MAC_AX_SDIO_SUPPORT
static u8 r8_indir_cmd52_sdio(void *drv_adapter,
			      struct mac_ax_pltfm_cb *pltfm_cb, u32 adr)
{
	u8 tmp;
	u32 cnt;

	pltfm_cb->sdio_cmd52_w8(drv_adapter, R_AX_SDIO_INDIRECT_ADDR,
				(u8)adr);
	pltfm_cb->sdio_cmd52_w8(drv_adapter, R_AX_SDIO_INDIRECT_ADDR + 1,
				(u8)(adr >> 8));
	pltfm_cb->sdio_cmd52_w8(drv_adapter, R_AX_SDIO_INDIRECT_ADDR + 2,
				(u8)(adr >> 16));
	pltfm_cb->sdio_cmd52_w8(drv_adapter, R_AX_SDIO_INDIRECT_ADDR + 3,
				(u8)((adr | B_AX_INDIRECT_RDY) >> 24));
	pltfm_cb->sdio_cmd52_w8(drv_adapter, R_AX_SDIO_INDIRECT_CTRL,
				(u8)B_AX_INDIRECT_REG_R);

	cnt = SDIO_WAIT_CNT;
	do {
		tmp = pltfm_cb->sdio_cmd52_r8(drv_adapter,
					      R_AX_SDIO_INDIRECT_ADDR + 3);
		cnt--;
	} while (((tmp & BIT(7)) == 0) && (cnt > 0));

	if (((tmp & BIT(7)) == 0) && cnt == 0)
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_,
				    "[ERR]sdio indirect CMD52 read\n");

	return pltfm_cb->sdio_cmd52_r8(drv_adapter, R_AX_SDIO_INDIRECT_DATA);
}

static u8 r8_dir_cmd52_sdio(void *drv_adapter,
			    struct mac_ax_pltfm_cb *pltfm_cb, u32 adr)
{
	/* Original: Use indirect access WLAN reg due to SDIO power off*/
	/* SDIO reg changes after 8852C (0x1000 -> 0x4000) */
	/* -> SDIO indirect access reg is uncertain before get_chip_info */
	/* Fix: CMD52 direct read WLAN reg and repeat several times */
	/* to sync SDIO reg with MAC reg */
	/* MAC reg is in MAC clk domain & SDIO reg is in SDIO clk domain */
	u32 cnt;

	cnt = MAC_AX_INDIR_CNT;
	do {
		pltfm_cb->sdio_cmd52_r8(drv_adapter, adr);
		cnt--;
	} while (cnt);

	return pltfm_cb->sdio_cmd52_r8(drv_adapter, adr);
}

static u8 get_chip_id_hw_def_sdio(void *drv_adapter,
				  struct mac_ax_pltfm_cb *pltfm_cb, u32 adr)
{
	u16 pid;

	pid = pltfm_cb->sdio_cmd52_cia_r8(drv_adapter, SDIO_FN0_CIS_PID) |
	      (pltfm_cb->sdio_cmd52_cia_r8(drv_adapter, SDIO_FN0_CIS_PID + 1) << 8);

	switch (pid) {
	case PID_HW_DEF_8852AS:
		return CHIP_ID_HW_DEF_8852A;
	case PID_HW_DEF_8852BS:
	case PID_HW_DEF_8852BSA:
	case PID_HW_DEF_8851ASA:
	case PID_HW_DEF_8852BPS:
		return CHIP_ID_HW_DEF_8852B;
	case PID_HW_DEF_8852CS:
		return CHIP_ID_HW_DEF_8852C;
	case PID_HW_DEF_8192XBS:
		return CHIP_ID_HW_DEF_8192XB;
	case PID_HW_DEF_8851BS:
	case PID_HW_DEF_8851BSM:
		return CHIP_ID_HW_DEF_8851B;
	case PID_HW_DEF_8851ES:
		return CHIP_ID_HW_DEF_8851E;
	case PID_HW_DEF_8852DS:
		return CHIP_ID_HW_DEF_8852D;
	case PID_HW_DEF_1115ES:
		return CHIP_ID_HW_DEF_1115E;
	default:
		break;
	}

	return r8_dir_cmd52_sdio(drv_adapter, pltfm_cb, adr);
}
#endif
static bool chk_get_chip_info(u8 chip_id, u8 cv)
{
	switch (chip_id) {
	case MAC_AX_CHIP_ID_8852A:
		switch (cv) {
		case CAV:
			return PATCH_ENABLE;
		case CBV:
			return PATCH_ENABLE;
		case CCV:
			return PATCH_DISABLE;
		case CDV:
			//fall through
		default:
			return PATCH_DISABLE;
	}
	default:
		return PATCH_DISABLE;
	}
}

static u32 get_chip_info(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
			 enum mac_ax_intf intf, u8 *id, u8 *cv)
{
	u8 cur_id;
	u32 ret;

	if (!cv || !id)
		return MACNPTR;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		cur_id = r8_indir_cmd52_sdio(drv_adapter, pltfm_cb,
					     R_AX_SYS_CHIPINFO);
		*cv = r8_indir_cmd52_sdio(drv_adapter, pltfm_cb,
					  R_AX_SYS_CFG1 + 1) >> 4;
		if (chk_get_chip_info(cur_id, *cv) == PATCH_ENABLE)
			*cv = CBV;
		break;
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	case MAC_AX_INTF_USB:
	case MAC_AX_INTF_PCIE:
		cur_id = pltfm_cb->reg_r8(drv_adapter, R_AX_SYS_CHIPINFO);
		*cv = pltfm_cb->reg_r8(drv_adapter, R_AX_SYS_CFG1 + 1) >> 4;
		if (chk_get_chip_info(cur_id, *cv) == PATCH_ENABLE)
			*cv = CBV;
		break;
#endif
	default:
		return MACINTF;
	}

	ret = xlat_chip_id(cur_id, id);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 mac_ax_ops_init(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
		    enum mac_ax_intf intf,
		    struct mac_ax_adapter **mac_adapter,
		    struct mac_ax_ops **mac_ops)
{
	u32 ret;
	u8 chip_id = 0;
	u8 cv = 0;
	struct mac_ax_adapter *adapter;
	struct mac_ax_priv_ops *p_ops;

	if (!chk_pltfm_cb(drv_adapter, intf, pltfm_cb))
		return MACPFCB;

	if (!chk_pltfm_endian())
		return MACPFED;

	pltfm_cb->msg_print(drv_adapter, _PHL_ERR_,
			    "MAC_AX_MAJOR_VER = %d\n"
			    "MAC_AX_PROTOTYPE_VER = %d\n"
			    "MAC_AX_SUB_VER = %d\n"
			    "MAC_AX_SUB_INDEX = %d\n",
			    MAC_AX_MAJOR_VER, MAC_AX_PROTOTYPE_VER,
			    MAC_AX_SUB_VER, MAC_AX_SUB_INDEX);

	ret = get_chip_info(drv_adapter, pltfm_cb, intf, &chip_id, &cv);
	if (ret)
		return ret;

	adapter = get_mac_ax_adapter(intf, chip_id, cv, drv_adapter,
				     pltfm_cb);
	if (!adapter) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "[ERR]Get MAC adapter\n");
		return MACADAPTER;
	}

	*mac_adapter = adapter;
	*mac_ops = adapter->ops;

#if MAC_AX_FEATURE_HV
	adapter->hv_ops = get_hv_ax_ops(adapter);
#endif

#if MAC_AX_PHL_H2C

#else
	ret = h2cb_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]h2cb init %d\n", ret);
		return ret;
	}
#endif

	ret = role_tbl_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]role tbl init %d\n", ret);
		return ret;
	}

	p_ops = adapter_to_priv_ops(adapter);
	ret = p_ops->sec_info_tbl_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]sec info tbl init %d\n", ret);
		return ret;
	}

	ret = efuse_tbl_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]efuse tbl init %d\n", ret);
		return ret;
	}

	ret = p2p_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]p2p info init %d\n", ret);
		return ret;
	}

	ret = mport_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mpinfo info init %d\n", ret);
		return ret;
	}

	ret = mix_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mix info init %d\n", ret);
		return ret;
	}

#if MAC_AX_SDIO_SUPPORT
	ret = sdio_tbl_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]sdio tbl init %d\n", ret);
		return ret;
	}
#endif

	ret = hfc_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]hfc info init %d\n", ret);
		return ret;
	}

	ret = twt_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]twt info init %d\n", ret);
		return ret;
	}

	ret = dbcc_info_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dbcc info init %d\n", ret);
		return ret;
	}

	ret = get_analog_info(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get_analog_info %d\n", ret);
		return ret;
	}

	return MACSUCCESS;
}
#endif /*CONFIG_NEW_HALMAC_INTERFACE*/

#if MAC_AX_PHL_H2C
u32 mac_ax_phl_init(void *phl_adapter, struct mac_ax_adapter *mac_adapter)
{
	struct mac_ax_adapter *adapter = mac_adapter;

	adapter->phl_adapter = phl_adapter;

	return MACSUCCESS;
}
#endif
u32 mac_ax_ops_exit(struct mac_ax_adapter *adapter)
{
	u32 ret;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct mac_ax_efuse_ofld_info *efuse_ofld_info = &adapter->efuse_ofld_info;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	struct scan_chinfo_list *scan_list;
	u32 priv_size;
	u8 band_idx;

	ret = h2cb_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]h2c buffer exit %d\n", ret);
		return ret;
	}

	if (ofld_info->buf) {
		PLTFM_FREE(ofld_info->buf, CMD_OFLD_MAX_LEN);
		ofld_info->buf = NULL;
	}

	if (efuse_ofld_info->buf) {
		PLTFM_FREE(efuse_ofld_info->buf, hw_info->dav_log_efuse_size);
		efuse_ofld_info->buf = NULL;
	}

	ret = p_ops->free_sec_info_tbl(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]sec table exit %d\n", ret);
		return ret;
	}

	ret = role_tbl_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]role table exit %d\n", ret);
		return ret;
	}

	ret = efuse_tbl_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]efuse table exit %d\n", ret);
		return ret;
	}

	ret = p2p_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]p2p info exit %d\n", ret);
		return ret;
	}

	ret = mport_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mpinfo info exit %d\n", ret);
		return ret;
	}

	ret = mix_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mix info exit %d\n", ret);
		return ret;
	}

#if MAC_AX_SDIO_SUPPORT
	ret = sdio_tbl_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]efuse table exit %d\n", ret);
		return ret;
	}
#endif

	ret = hfc_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]hfc info exit %d\n", ret);
		return ret;
	}

	ret = twt_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]twt info exit %d\n", ret);
		return ret;
	}

	ret = dbcc_info_exit(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dbcc info exit %d\n", ret);
		return ret;
	}

	if (efuse_param->efuse_map) {
		PLTFM_FREE(efuse_param->efuse_map,
			   adapter->hw_info->efuse_size);
		efuse_param->efuse_map = (u8 *)NULL;
	}

	if (efuse_param->bt_efuse_map) {
		PLTFM_FREE(efuse_param->bt_efuse_map,
			   adapter->hw_info->bt_efuse_size);
		efuse_param->bt_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->log_efuse_map) {
		PLTFM_FREE(efuse_param->log_efuse_map,
			   adapter->hw_info->log_efuse_size);
		efuse_param->log_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->bt_log_efuse_map) {
		PLTFM_FREE(efuse_param->bt_log_efuse_map,
			   adapter->hw_info->bt_log_efuse_size);
		efuse_param->bt_log_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->dav_efuse_map) {
		PLTFM_FREE(efuse_param->dav_efuse_map,
			   adapter->hw_info->dav_efuse_size);
		efuse_param->dav_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->dav_log_efuse_map) {
		PLTFM_FREE(efuse_param->dav_log_efuse_map,
			   adapter->hw_info->dav_log_efuse_size);
		efuse_param->dav_log_efuse_map = (u8 *)NULL;
	}

	if (efuse_param->hidden_rf_map) {
		PLTFM_FREE(efuse_param->hidden_rf_map,
			   adapter->hw_info->hidden_efuse_rf_size);
		efuse_param->hidden_rf_map = (u8 *)NULL;
	}

	for (band_idx = 0; band_idx < MAC_AX_BAND_NUM; band_idx++) {
		scan_list = adapter->scanofld_info.list[band_idx];
		if (scan_list) {
			mac_scanofld_ch_list_clear(adapter, scan_list);
			PLTFM_FREE((u8 *)scan_list,
				   sizeof(struct scan_chinfo_list));
			adapter->scanofld_info.list[band_idx] = NULL;
		}
	}

	PLTFM_FREE(adapter->hw_info, sizeof(struct mac_ax_hw_info));

	priv_size = get_mac_ax_priv_size();
	PLTFM_FREE(adapter, sizeof(struct mac_ax_adapter) + priv_size);
	return MACSUCCESS;
}

u32 is_chip_id(struct mac_ax_adapter *adapter, enum mac_ax_chip_id id)
{
	return (id == adapter->hw_info->chip_id ? 1 : 0);
}

u32 is_cv(struct mac_ax_adapter *adapter, enum rtw_cv cv)
{
	return (cv == adapter->hw_info->cv ? 1 : 0);
}

u32 xlat_chip_id(u8 hw_id, u8 *chip_id)
{
	switch (hw_id) {
	case CHIP_ID_HW_DEF_8852A:
		*chip_id = MAC_AX_CHIP_ID_8852A;
		break;
	case CHIP_ID_HW_DEF_8852B:
		*chip_id = MAC_AX_CHIP_ID_8852B;
		break;
	case CHIP_ID_HW_DEF_8852C:
		*chip_id = MAC_AX_CHIP_ID_8852C;
		break;
	case CHIP_ID_HW_DEF_8192XB:
		*chip_id = MAC_AX_CHIP_ID_8192XB;
		break;
	case CHIP_ID_HW_DEF_8851B:
		*chip_id = MAC_AX_CHIP_ID_8851B;
		break;
	case CHIP_ID_HW_DEF_8851E:
		*chip_id = MAC_AX_CHIP_ID_8851E;
		break;
	case CHIP_ID_HW_DEF_8852D:
		*chip_id = MAC_AX_CHIP_ID_8852D;
		break;
	default:
		*chip_id = MAC_AX_CHIP_ID_INVALID;
		return MACCHIPID;
	}

	return MACSUCCESS;
}

