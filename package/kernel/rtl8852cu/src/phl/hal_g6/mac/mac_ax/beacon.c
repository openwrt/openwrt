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

#include "beacon.h"

u8 _byte_rev(u8 in)
{
	u8 data = 0;
	u8 i;

	for (i = 0; i < 8; i++)
		data |= in & BIT(i) ? BIT(7 - i) : 0;
	return data;
}

u8 _crc8_htsig(u8 *mem, u32 len)
{
	u8 crc = 0xFF;
	u8 key = 0x07;
	u8 k;

	if (!mem)
		return 0xFF;

	while (len--) {
		crc ^= _byte_rev(*mem++);
		for (k = 0; k < 8; k++)
			crc = crc & 0x80 ? (crc << 1) ^ key : crc << 1;
	}

	return crc;
}

u32 mac_calc_crc(struct mac_ax_adapter *adapter, struct mac_calc_crc_info *info)
{
	if (!info->buf)
		return MACNPTR;

	if (!info->len)
		return MACBUFSZ;

	info->crc = (u32)_crc8_htsig(info->buf, info->len);

	return MACSUCCESS;
}

u32 mac_bcn_ofld_ctrl(struct mac_ax_adapter *adapter, struct mac_bcn_ofld_info *info)
{
	struct fwcmd_ie_cam *cmd;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	u32 ret, iecam_len, i;
	u8 *buf, *iecam_buf;
	u8 rst = 0, num = 0;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ie_cam) - IECAM_INFO_SIZE);
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	switch (info->ctrl_type) {
	case MAC_BCN_OFLD_UPD_PARAM:
	case MAC_BCN_OFLD_DIS:
		num = 0;
		rst = 0;
		break;
	case MAC_BCN_OFLD_EN:
	case MAC_BCN_OFLD_UPD_CAM:
		if (!info->cam_num) {
			PLTFM_MSG_ERR("%s cam num is 0 when enable/update iecam\n", __func__);
			return MACFUNCINPUT;
		}

		rst = info->rst_iecam == MAC_AX_FUNC_EN ? 1 : 0;
		num = info->cam_num;
	}

	cmd = (struct fwcmd_ie_cam *)buf;
	cmd->dword0 =
		cpu_to_le32((info->band == MAC_AX_BAND_1 ? FWCMD_H2C_IE_CAM_BAND : 0) |
			    SET_WORD(info->port, FWCMD_H2C_IE_CAM_PORT) |
			    (info->ctrl_type == MAC_BCN_OFLD_DIS ?
			     0 : FWCMD_H2C_IE_CAM_CAM_EN) |
			    (info->hit_en == MAC_AX_FUNC_DIS ?
			     0 : FWCMD_H2C_IE_CAM_HIT_FRWD_EN) |
			    SET_WORD(info->hit_sel, FWCMD_H2C_IE_CAM_HIT_FRWD) |
			    (info->miss_en == MAC_AX_FUNC_DIS ?
			     0 : FWCMD_H2C_IE_CAM_MISS_FRWD_EN) |
			    SET_WORD(info->miss_sel, FWCMD_H2C_IE_CAM_MISS_FRWD) |
			    SET_WORD(num, FWCMD_H2C_IE_CAM_UPD_NUM) |
			    (rst ? FWCMD_H2C_IE_CAM_RST : 0));

	if (num) {
		iecam_len = sizeof(struct mac_ie_cam_ent) * num;
		iecam_buf = h2cb_put(h2cb, iecam_len);
		if (!iecam_buf)
			return MACNOBUF;

		for (i = 0; i < num; i++) {
			*(u32 *)iecam_buf = cpu_to_le32(info->cam_list[i].hdr.cam_idx);
			iecam_buf += 4;
			*(u32 *)iecam_buf = cpu_to_le32(info->cam_list[i].u.data.dw0);
			iecam_buf += 4;
			*(u32 *)iecam_buf = cpu_to_le32(info->cam_list[i].u.data.dw1);
			iecam_buf += 4;
		}
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_IE_CAM,
			      FWCMD_H2C_FUNC_IE_CAM, 0, 1);
	if (ret) {
		PLTFM_MSG_ERR("iecam h2c set hdr fail %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("iecam h2c build txd fail %d\n", ret);
		goto fail;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("iecam h2c tx fail %d\n", ret);
		goto fail;
	}

fail:
	h2cb_free(adapter, h2cb);

	if (!ret)
		h2c_end_flow(adapter);

	return ret;
}

