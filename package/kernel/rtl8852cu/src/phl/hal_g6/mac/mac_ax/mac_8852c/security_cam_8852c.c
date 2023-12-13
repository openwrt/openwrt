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
#include "security_cam_8852c.h"

#define SEC_CAM_SIZE_8852C	0x14
#define HW_SUPPORT_ENC_TYPE_NUM 0x0A
#define OUTRANGE_KEY_INDEX	0xFF
#define NO_MACID_ROLE		0xFF
#define VALID			0x01
#define INVALID			0x00

struct addr_sec_only_info {
	u32 dword0;
	u32 dword1;
	u32 dword2;
};

enum SEC_CAM_KEY_TYPE {
	SEC_CAM_KEY_TYPE_UNI = 0,
	SEC_CAM_KEY_TYPE_GROUP = 1,
	SEC_CAM_KEY_TYPE_BIP = 2,
	SEC_CAM_KEY_TYPE_DEFAULT = 3
};

enum ADDR_CAM_SEC_MODE {
	ADDR_CAM_SEC_MODE_ZERO = 0,
	ADDR_CAM_SEC_MODE_ONE  = 1,
	ADDR_CAM_SEC_MODE_TWO  = 2,
	ADDR_CAM_SEC_MODE_THREE = 3,
};

enum DCTL_TBL_KEYID_MODE {
	DCTL_TBL_KEYID_ZERO_MODE = 0,
	DCTL_TBL_KEYID_ONE_MODE  = 1,
	DCTL_TBL_KEYID_TWO_MODE  = 2,
	DCTL_TBL_KEYID_THREE_MODE = 3,
	DCTL_TBL_KEYID_FOUR_MODE = 4,
	DCTL_TBL_KEYID_FIVE_MODE = 5,
	DCTL_TBL_KEYID_SIX_MODE = 6,
	DCTL_TBL_FLUSH_MODE = 7,
	DCTL_TBL_NONE_UPD_MODE = 8,
};

enum HW_SUPPORT_ENC_TYPE {
	HW_SUPPORT_ENC_TYPE_NONE = 0x0,
	HW_SUPPORT_ENC_TYPE_WEP40  = 0x1,
	HW_SUPPORT_ENC_TYPE_WEP104  = 0x2,
	HW_SUPPORT_ENC_TYPE_TKIP = 0x3,
	HW_SUPPORT_ENC_TYPE_WAPI = 0x4,
	HW_SUPPORT_ENC_TYPE_GCMSMS4 = 0x5,
	HW_SUPPORT_ENC_TYPE_CCMP128 = 0x6,
	HW_SUPPORT_ENC_TYPE_CCMP256 = 0x7,
	HW_SUPPORT_ENC_TYPE_GCMP128 = 0x8,
	HW_SUPPORT_ENC_TYPE_GCMP256 = 0x9,
	HW_SUPPORT_ENC_TYPE_BIP128 = 0xA,
};

u32 sec_info_tbl_init_8852c(struct mac_ax_adapter *adapter)
{
	u8 i = 0, keyidx = 0;
	struct sec_cam_table_t **sec_cam_table =
		&adapter->hw_info->sec_cam_table;
	struct dctl_sec_info_t **dctl_sec_info =
		&adapter->hw_info->dctl_sec_info;
	struct dctl_secinfo_entry_t *dctl_sec_info_entry = NULL;

	/*First time access sec cam , initial sec cam table INVALID  */
	if ((*sec_cam_table) == NULL) {
		(*sec_cam_table) = (struct sec_cam_table_t *)PLTFM_MALLOC
			(sizeof(struct sec_cam_table_t));

		if (!(*sec_cam_table))
			return MACNOBUF;

		for (i = 0; i < SEC_CAM_ENTRY_NUM; i++) {
			(*sec_cam_table)->sec_cam_entry[i] =
			(struct sec_cam_entry_t *)PLTFM_MALLOC
			(sizeof(struct sec_cam_entry_t));

			if (!(*sec_cam_table)->sec_cam_entry[i])
				return MACNOBUF;

			(*sec_cam_table)->sec_cam_entry[i]->sec_cam_info =
			(struct mac_ax_sec_cam_info *)PLTFM_MALLOC
			(sizeof(struct mac_ax_sec_cam_info));

			if (!(*sec_cam_table)->sec_cam_entry[i]->sec_cam_info)
				return MACNOBUF;

			/* initial value*/
			(*sec_cam_table)->sec_cam_entry[i]->valid = INVALID;
			(*sec_cam_table)->sec_cam_entry[i]->mac_id = 0;
			(*sec_cam_table)->sec_cam_entry[i]->key_id = 0;
			(*sec_cam_table)->sec_cam_entry[i]->key_type = 0;
		}
		(*sec_cam_table)->next_cam_storage_idx = 0;
	}

	if ((*dctl_sec_info) == NULL) {
		(*dctl_sec_info) = (struct dctl_sec_info_t *)PLTFM_MALLOC
				   (sizeof(struct dctl_sec_info_t));

		if (!(*dctl_sec_info))
			return MACNOBUF;

		for (i = 0; i < MACID_ENTRY_NUM; i++) {
			(*dctl_sec_info)->dctl_secinfo_entry[i] =
			(struct dctl_secinfo_entry_t *)PLTFM_MALLOC
			(sizeof(struct dctl_secinfo_entry_t));

			if (!(*dctl_sec_info)->dctl_secinfo_entry[i])
				return MACNOBUF;

			dctl_sec_info_entry =
				(*dctl_sec_info)->dctl_secinfo_entry[i];

			/*initial value*/
			dctl_sec_info_entry->sec_ent_valid = INVALID;
			dctl_sec_info_entry->sec_keyid = 0;
			dctl_sec_info_entry->aes_iv_h = 0;
			dctl_sec_info_entry->aes_iv_l = 0;
			for (keyidx = 0; keyidx < KEYNUM_PER_MACID; keyidx++) {
				dctl_sec_info_entry->sec_ent_keyid[keyidx] = 0;
				dctl_sec_info_entry->sec_ent[keyidx] = 0;
			}
		}
	}
	return MACSUCCESS;
}

u32 free_sec_info_tbl_8852c(struct mac_ax_adapter *adapter)
{
	u8 i;
	struct sec_cam_table_t *sec_cam_table = adapter->hw_info->sec_cam_table;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	if (!sec_cam_table && !dctl_sec_info)
		return MACSUCCESS;

	if (sec_cam_table) {
		for (i = 0; i < SEC_CAM_ENTRY_NUM; i++) {
			PLTFM_FREE(sec_cam_table->sec_cam_entry[i]->sec_cam_info,
				   sizeof(struct mac_ax_sec_cam_info));
		}
		for (i = 0; i < SEC_CAM_ENTRY_NUM; i++) {
			PLTFM_FREE(sec_cam_table->sec_cam_entry[i],
				   sizeof(struct sec_cam_entry_t));
		}

		PLTFM_FREE(sec_cam_table, sizeof(struct sec_cam_table_t));

		adapter->hw_info->sec_cam_table = NULL;
	}

	if (dctl_sec_info) {
		for (i = 0; i < MACID_ENTRY_NUM; i++) {
			PLTFM_FREE(dctl_sec_info->dctl_secinfo_entry[i],
				   sizeof(struct dctl_secinfo_entry_t));
		}

		PLTFM_FREE(dctl_sec_info, sizeof(struct dctl_sec_info_t));

		adapter->hw_info->dctl_sec_info = NULL;
	}

	return MACSUCCESS;
}

u32 upt_dctl_secinfo_entry_8852c(struct mac_ax_adapter *adapter,
				 u8 macid, u8 keyidx, u8 sec_iv_upd)
{
	u32 ret = 0;
	struct mac_ax_dctl_info info = {0}, mask = {0};
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	if (!dctl_sec_info)
		return MACNOKEYINDEX;

	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[macid];

	/* Put the keyid into the field */
	info.sec_ent_valid = dctl_secinfo_entry->sec_ent_valid;
	mask.sec_ent_valid = KEY_VALID_MSK;
	info.sec_key_id = dctl_secinfo_entry->sec_keyid;
	mask.sec_key_id = KEYID_MSK;
	info.wapi = dctl_secinfo_entry->wapi_ctrl;
	mask.wapi = VALID;

	switch (keyidx) {
	case DCTL_TBL_KEYID_ZERO_MODE:
		/**/
		info.sec_ent0_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent0 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent0_keyid = KEYID_MSK;
		mask.sec_ent0 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_ONE_MODE:
		info.sec_ent1_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent1 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent1_keyid = KEYID_MSK;
		mask.sec_ent1 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_TWO_MODE:
		info.sec_ent2_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent2 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent2_keyid = KEYID_MSK;
		mask.sec_ent2 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_THREE_MODE:
		info.sec_ent3_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent3 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent3_keyid = KEYID_MSK;
		mask.sec_ent3 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_FOUR_MODE:
		info.sec_ent4_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent4 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent4_keyid = KEYID_MSK;
		mask.sec_ent4 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_FIVE_MODE:
		info.sec_ent5_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent5 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent5_keyid = KEYID_MSK;
		mask.sec_ent5 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_KEYID_SIX_MODE:
		info.sec_ent6_keyid = dctl_secinfo_entry->sec_ent_keyid[keyidx];
		info.sec_ent6 = dctl_secinfo_entry->sec_ent[keyidx];
		mask.sec_ent6_keyid = KEYID_MSK;
		mask.sec_ent6 = KEY_CAM_IDX_MSK;
		break;
	case DCTL_TBL_FLUSH_MODE:
		mask.sec_ent0_keyid = KEYID_MSK;
		mask.sec_ent1_keyid = KEYID_MSK;
		mask.sec_ent2_keyid = KEYID_MSK;
		mask.sec_ent3_keyid = KEYID_MSK;
		mask.sec_ent4_keyid = KEYID_MSK;
		mask.sec_ent5_keyid = KEYID_MSK;
		mask.sec_ent6_keyid = KEYID_MSK;
		mask.sec_ent0 = KEY_CAM_IDX_MSK;
		mask.sec_ent1 = KEY_CAM_IDX_MSK;
		mask.sec_ent2 = KEY_CAM_IDX_MSK;
		mask.sec_ent3 = KEY_CAM_IDX_MSK;
		mask.sec_ent4 = KEY_CAM_IDX_MSK;
		mask.sec_ent5 = KEY_CAM_IDX_MSK;
		mask.sec_ent6 = KEY_CAM_IDX_MSK;
		break;
	default:
		break;
	}

	if (sec_iv_upd == SEC_IV_UPD_TYPE_WRITE) {
		info.aes_iv_h = dctl_secinfo_entry->aes_iv_h;
		mask.aes_iv_h = AES_IV_H_MSK;
		info.aes_iv_l = dctl_secinfo_entry->aes_iv_l;
		mask.aes_iv_l = AES_IV_L_MSK;
	}

	ret = mac_upd_dctl_info_8852c(adapter, &info, &mask, macid, 1);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 fill_sec_cam_info_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_sec_cam_info *s_info,
			    struct fwcmd_seccam_info *sec_info,
			    u8 clear)
{
	if (clear == INVALID) {
		sec_info->dword0 =
		cpu_to_le32(SET_WORD(s_info->sec_cam_idx, FWCMD_H2C_SECCAM_INFO_IDX) |
			    SET_WORD(s_info->offset, FWCMD_H2C_SECCAM_INFO_OFFSET) |
			    SET_WORD(s_info->len, FWCMD_H2C_SECCAM_INFO_LEN));

		sec_info->dword1 =
		cpu_to_le32(SET_WORD(s_info->type, FWCMD_H2C_SECCAM_INFO_TYPE) |
			    ((s_info->ext_key) ? FWCMD_H2C_SECCAM_INFO_EXT_KEY : 0) |
			    ((s_info->spp_mode) ? FWCMD_H2C_SECCAM_INFO_SPP_MODE : 0));
		sec_info->dword2 = s_info->key[0];
		sec_info->dword3 = s_info->key[1];
		sec_info->dword4 = s_info->key[2];
		sec_info->dword5 = s_info->key[3];
	} else {
		sec_info->dword0 =
		cpu_to_le32(SET_WORD(s_info->sec_cam_idx, FWCMD_H2C_SECCAM_INFO_IDX) |
			    SET_WORD(s_info->offset, FWCMD_H2C_SECCAM_INFO_OFFSET) |
			    SET_WORD(s_info->len, FWCMD_H2C_SECCAM_INFO_LEN));

		sec_info->dword1 = 0x0;
		sec_info->dword2 = 0x0;
		sec_info->dword3 = 0x0;
		sec_info->dword4 = 0x0;
		sec_info->dword5 = 0x0;
	}
	return MACSUCCESS;
}

u32 mac_upd_sec_infotbl_8852c(struct mac_ax_adapter *adapter,
			      struct fwcmd_seccam_info *info)
{
	u32 ret = 0, s_info_tbl[6], cam_address = 0;
	u8 *buf, i;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_seccam_info *tbl;
	struct mac_ax_sec_cam_info *s_info;

	/*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_seccam_info));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	tbl = (struct fwcmd_seccam_info *)buf;

	tbl->dword0 = info->dword0;
	tbl->dword1 = info->dword1;
	tbl->dword2 = info->dword2;
	tbl->dword3 = info->dword3;
	tbl->dword4 = info->dword4;
	tbl->dword5 = info->dword5;

	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = h2c_pkt_set_hdr(adapter, h2cb,
				      FWCMD_TYPE_H2C,
				      FWCMD_H2C_CAT_MAC,
				      FWCMD_H2C_CL_SEC_CAM,
				      FWCMD_H2C_FUNC_SECCAM_INFO,
				      0,
				      1);

		if (ret != MACSUCCESS)
			goto fail;

		ret = h2c_agg_enqueue(adapter, h2cb);
		if (!ret)
			return MACSUCCESS;

		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret != MACSUCCESS)
			goto fail;

		#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
		#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
		#endif
		if (ret != MACSUCCESS)
			goto fail;

		h2cb_free(adapter, h2cb);
		return MACSUCCESS;
fail:
		h2cb_free(adapter, h2cb);
	} else {
		/* Indirect Access */
		s_info = (struct mac_ax_sec_cam_info *)info;
		s_info_tbl[0] = info->dword0;
		s_info_tbl[1] = info->dword1;
		s_info_tbl[2] = info->dword2;
		s_info_tbl[3] = info->dword3;
		s_info_tbl[4] = info->dword4;
		s_info_tbl[5] = info->dword5;

		/*  Indirect write security cam */
		PLTFM_MSG_WARN("%s ind access start\n", __func__);
		cam_address = s_info->sec_cam_idx * SEC_CAM_ENTRY_SIZE;
		for (i = 0; i < 5; i++)
			mac_sram_dbg_write(adapter, cam_address + (i * 4),
					   cpu_to_le32(s_info_tbl[i + 1]),
					   SEC_CAM_SEL);
		PLTFM_MSG_WARN("%s ind access end\n", __func__);

		h2cb_free(adapter, h2cb);
		return MACSUCCESS;
	}

	return ret;
}

u8 check_key_index_8852c(u8 addr_cam_sec_mode, u8 key_type, u8 keyidx)
{
	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (keyidx <= 6)
				return MACSUCCESS;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_ONE:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (keyidx <= 6)
				return MACSUCCESS;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_TWO:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (keyidx <= 1)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_GROUP:
			if (keyidx >= 2 && keyidx <= 4)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_BIP:
			if (keyidx >= 5 && keyidx <= 6)
				return MACSUCCESS;
			break;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_THREE:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (keyidx <= 1)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_GROUP:
			if (keyidx >= 2 && keyidx <= 5)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_BIP:
			if (keyidx == 6)
				return MACSUCCESS;
			break;
		default:
			break;
		}
		break;
	}
	return 1;
}

u8 decide_key_index_8852c(u8 addr_cam_sec_mode, u8 key_type)
{
	u8 keyidx = 0;

	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			keyidx = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			return MACWNGKEYTYPE;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			return MACWNGKEYTYPE;
		break;
	case ADDR_CAM_SEC_MODE_ONE:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			keyidx = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			return MACWNGKEYTYPE;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			return MACWNGKEYTYPE;
		break;
	case ADDR_CAM_SEC_MODE_TWO:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			keyidx = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			keyidx = 2;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			keyidx = 5;
		break;
	case ADDR_CAM_SEC_MODE_THREE:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			keyidx = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			keyidx = 2;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			keyidx = 6;
		break;
	default:
		break;
	}
	return keyidx;
}

u8 decide_sec_cam_index_8852c(struct mac_ax_adapter *adapter, u8 *sec_cam_idx)
{
	u8 sec_idx = 0, i = 0;
	/* call by pointer */
	struct sec_cam_table_t **sec_cam_table =
		&adapter->hw_info->sec_cam_table;

	/*First time access sec cam , initial sec cam table INVALID  */
	if ((*sec_cam_table) == NULL) {
		(*sec_cam_table) = (struct sec_cam_table_t *)PLTFM_MALLOC
			(sizeof(struct sec_cam_table_t));

		if (!(*sec_cam_table))
			return MACNOBUF;

		for (i = 0; i < SEC_CAM_ENTRY_NUM; i++) {
			(*sec_cam_table)->sec_cam_entry[i] =
			(struct sec_cam_entry_t *)PLTFM_MALLOC
			(sizeof(struct sec_cam_entry_t));

			if (!(*sec_cam_table)->sec_cam_entry[i])
				return MACNOBUF;

			(*sec_cam_table)->sec_cam_entry[i]->sec_cam_info =
			(struct mac_ax_sec_cam_info *)PLTFM_MALLOC
			(sizeof(struct mac_ax_sec_cam_info));

			if (!(*sec_cam_table)->sec_cam_entry[i]->sec_cam_info)
				return MACNOBUF;

			/* initial value*/
			(*sec_cam_table)->sec_cam_entry[i]->valid = INVALID;
			(*sec_cam_table)->sec_cam_entry[i]->mac_id = 0;
			(*sec_cam_table)->sec_cam_entry[i]->key_id = 0;
			(*sec_cam_table)->sec_cam_entry[i]->key_type = 0;
		}
		(*sec_cam_table)->next_cam_storage_idx = 0;
	}

	/*If table has been initialize, assgin the sec cam storge idx */
	sec_idx = (*sec_cam_table)->next_cam_storage_idx;
	for (i = 0; i < SEC_CAM_ENTRY_NUM; i++) {
		if ((*sec_cam_table)->sec_cam_entry[sec_idx]->valid ==
		    INVALID) {
			(*sec_cam_table)->next_cam_storage_idx =
			(sec_idx + 1) % SEC_CAM_ENTRY_NUM;
			*sec_cam_idx = sec_idx;
			return MACSUCCESS;
		}
		sec_idx++;
		sec_idx %= SEC_CAM_ENTRY_NUM;
	}

	return MACSECCAMFL;
}

u32 delete_key_from_dmac_tbl_8852c(struct mac_ax_adapter *adapter,
				   u8 mac_id, u8 key_type,
				   u8 key_id, u8 *sec_cam_idx)
{
	u32 ret = 0;
	u8 key_valid[7] = {0}, hit_flag = 0;
	u8 keyidx = 0, key_valid_byte = 0, sec_ent_mode = 0, i = 0;
	struct mac_role_tbl *role = NULL;
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info =	adapter->hw_info->dctl_sec_info;

	/*check dmac tbl is valid*/
	if (!dctl_sec_info)
		return MACNOKEYINDEX;

	/*check mac_id role is valid*/
	role = mac_role_srch(adapter, mac_id);
	if (!role)
		return MACNOROLE;

	/*get some decide key index info*/
	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];
	key_valid_byte = dctl_secinfo_entry->sec_ent_valid;
	sec_ent_mode = role->info.a_info.sec_ent_mode;

	keyidx = decide_key_index_8852c(sec_ent_mode, key_type);

	if (keyidx == MACWNGKEYTYPE)
		return MACWNGKEYTYPE;

	for (i = 0; i < KEYNUM_PER_MACID; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
	}

	/* find the key real storage idx */
	for (i = keyidx; i < KEYNUM_PER_MACID; i++) {
		if (dctl_secinfo_entry->sec_ent_keyid[i] == key_id &&
		    key_valid[i] == VALID) {
			keyidx = i;
			hit_flag = 1;
			break;
		}
	}

	if (hit_flag == 0)
		return MACKEYNOTEXT;

	*sec_cam_idx = dctl_secinfo_entry->sec_ent[keyidx];

	/* dmac tbl key info copy in halmac */
	dctl_secinfo_entry->sec_ent_valid &= ~(BIT(keyidx));
	dctl_secinfo_entry->sec_ent_keyid[keyidx] = 0;
	dctl_secinfo_entry->sec_ent[keyidx] = 0;

	if (key_type == SEC_CAM_KEY_TYPE_UNI)
		if (dctl_secinfo_entry->sec_keyid == key_id)
			dctl_secinfo_entry->sec_keyid = 0;

	ret = upt_dctl_secinfo_entry_8852c(adapter, mac_id, keyidx,
					   SEC_IV_UPD_TYPE_NONE);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 insert_key_to_dmac_tbl_8852c(struct mac_ax_adapter *adapter,
				 u8 mac_id, u8 key_type,
				 u8 key_id, u8 sec_cam_idx)
{
	u32 ret = 0;
	u8 key_valid[7] = {0}, hit_flag = 0;
	u8 keyidx = 0, key_valid_byte = 0, sec_ent_mode = 0, i = 0;
	struct mac_role_tbl *role = NULL;
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	/*check dmac tbl is valid*/
	if (!dctl_sec_info)
		return MACNOKEYINDEX;

	/*check mac_id role is valid*/
	role = mac_role_srch(adapter, mac_id);
	if (!role)
		return MACNOROLE;

	/*get some decide key index info*/
	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];
	key_valid_byte = dctl_secinfo_entry->sec_ent_valid;
	sec_ent_mode = role->info.a_info.sec_ent_mode;

	keyidx = decide_key_index_8852c(sec_ent_mode, key_type);

	if (keyidx == MACWNGKEYTYPE)
		return MACWNGKEYTYPE;

	for (i = 0; i < KEYNUM_PER_MACID; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
	}

	/* find the key real storage idx */
	for (i = keyidx; i < 7; i++) {
		if (key_valid[i] != VALID) {
			keyidx = i;
			hit_flag = 1;
			break;
		}
	}
	if (hit_flag == 0)
		return MACADDRCAMKEYFL;

	if (check_key_index_8852c(sec_ent_mode, key_type, keyidx)) {
		PLTFM_MSG_TRACE("check addr key index full\n");
		return MACADDRCAMKEYFL;
	}

	dctl_secinfo_entry->sec_ent_valid |= BIT(keyidx);
	dctl_secinfo_entry->sec_ent_keyid[keyidx] = key_id;
	dctl_secinfo_entry->sec_ent[keyidx] = sec_cam_idx;

	if (key_type == SEC_CAM_KEY_TYPE_UNI)
		dctl_secinfo_entry->sec_keyid = key_id;

	ret = upt_dctl_secinfo_entry_8852c(adapter, mac_id, keyidx,
					   SEC_IV_UPD_TYPE_NONE);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 m_security_cam_hal_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_sec_cam_info *sec_cam_info,
			     u8 mac_id, u8 key_id, u8 key_type,
			     u8 sec_cam_idx, u8 clear)
{
	u8 i = 0;
	struct sec_cam_entry_t *sec_cam_entry = NULL;
	struct sec_cam_table_t *sec_cam_table =	adapter->hw_info->sec_cam_table;

	if (!sec_cam_table)
		return MACSUCCESS;

	if (sec_cam_idx >= SEC_CAM_ENTRY_NUM)
		return MACSUCCESS;

	sec_cam_entry =
		adapter->hw_info->sec_cam_table->sec_cam_entry[sec_cam_idx];
	if (clear == 1) {
		sec_cam_entry->valid = INVALID;
		sec_cam_entry->mac_id = 0;
		sec_cam_entry->key_id = 0;
		sec_cam_entry->key_type = 0;
		sec_cam_entry->sec_cam_info->type = 0;
		sec_cam_entry->sec_cam_info->ext_key = 0;
		sec_cam_entry->sec_cam_info->spp_mode = 0;
		for (i = 0; i < 3; i++)
			sec_cam_entry->sec_cam_info->key[i] = 0;
	} else {
		if (!sec_cam_info) {
			sec_cam_entry->valid = VALID;
			sec_cam_entry->mac_id = mac_id;
			sec_cam_entry->key_id = key_id;
			sec_cam_entry->key_type = key_type;
			sec_cam_entry->sec_cam_info->type = 0x0;
			sec_cam_entry->sec_cam_info->ext_key = 0x0;
			sec_cam_entry->sec_cam_info->spp_mode = 0x0;
			for (i = 0; i < 4; i++)
				sec_cam_entry->sec_cam_info->key[i] = 0x0;
		} else {
			sec_cam_entry->valid = VALID;
			sec_cam_entry->mac_id = mac_id;
			sec_cam_entry->key_id = key_id;
			sec_cam_entry->key_type = key_type;
			sec_cam_entry->sec_cam_info->type = sec_cam_info->type;
			sec_cam_entry->sec_cam_info->ext_key =
				sec_cam_info->ext_key;
			sec_cam_entry->sec_cam_info->spp_mode =
				sec_cam_info->spp_mode;
			for (i = 0; i < 4; i++)
				sec_cam_entry->sec_cam_info->key[i] =
				sec_cam_info->key[i];
		}
	}

	return MACSUCCESS;
}

u32 disconnect_flush_key_8852c(struct mac_ax_adapter *adapter,
			       struct mac_role_tbl *role)
{
	u32 ret = 0;
	u8 i, macid, key_type, keyidx, key_valid_byte;
	u8 key_valid[7] = {0}, sec_cam_index[7] = {0}, sec_ent_keyid[7] = {0};
	struct mac_ax_sec_cam_info *sec_cam_info = NULL;
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	if (!dctl_sec_info)
		return MACNOKEYINDEX;

	macid = role->macid;

	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[macid];
	key_valid_byte = dctl_secinfo_entry->sec_ent_valid;

	/* Get key valid byte */
	for (i = 0; i < KEYNUM_PER_MACID; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
		sec_cam_index[i] = dctl_secinfo_entry->sec_ent[i];
		sec_ent_keyid[i] = dctl_secinfo_entry->sec_ent_keyid[i];
	}

	key_type = SEC_CAM_KEY_TYPE_DEFAULT;
	for (keyidx = 0; keyidx < KEYNUM_PER_MACID; keyidx++) {
		if (key_valid[keyidx] == VALID) {
			m_security_cam_hal_8852c(adapter, sec_cam_info,
						 macid,
						 sec_ent_keyid[keyidx],
						 key_type,
						 sec_cam_index[keyidx],
						 1);
		}
	}

	dctl_secinfo_entry->sec_ent_valid = 0;
	dctl_secinfo_entry->sec_keyid = 0;
	dctl_secinfo_entry->wapi_ctrl = 0;

	ret = upt_dctl_secinfo_entry_8852c(adapter, macid, DCTL_TBL_FLUSH_MODE,
					   SEC_IV_UPD_TYPE_NONE);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 mac_sta_del_key_8852c(struct mac_ax_adapter *adapter,
			  u8 mac_id,
			  u8 key_id,
			  u8 key_type)
{
	struct mac_ax_sec_cam_info sec_cam_info;
	u8 sec_cam_idx = 0;
	u32 sec_table[6] = {0}, ret = 0;

	ret = delete_key_from_dmac_tbl_8852c(adapter, mac_id, key_type,
					     key_id, &sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	sec_cam_info.sec_cam_idx = sec_cam_idx;
	sec_cam_info.offset = 0x00;
	sec_cam_info.len = SEC_CAM_SIZE_8852C;

	fill_sec_cam_info_8852c(adapter, &sec_cam_info,
				(struct fwcmd_seccam_info *)sec_table, 1);

	ret = (u8)mac_upd_sec_infotbl_8852c(adapter,
				      (struct fwcmd_seccam_info *)sec_table);
	if (ret != MACSUCCESS)
		return ret;

	m_security_cam_hal_8852c(adapter, NULL,
				 mac_id, key_id, key_type, sec_cam_idx, 1);

	return MACSUCCESS;
}

u32 mac_sta_add_key_8852c(struct mac_ax_adapter *adapter,
			  struct mac_ax_sec_cam_info *sec_cam_info,
			  u8 mac_id,
			  u8 key_id,
			  u8 key_type)
{
	u8 sec_cam_idx = 0;
	u32 sec_table[6] = {0}, ret = 0;
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	if (sec_cam_info->type > HW_SUPPORT_ENC_TYPE_NUM)
		return MACWNGKEYTYPE;

	if (!dctl_sec_info)
		return MACNOKEYINDEX;
	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];

	ret = decide_sec_cam_index_8852c(adapter, &sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	sec_cam_info->sec_cam_idx = sec_cam_idx;
	sec_cam_info->offset = 0x00;
	sec_cam_info->len = SEC_CAM_SIZE_8852C;

	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];
	if (sec_cam_info->type == HW_SUPPORT_ENC_TYPE_WAPI ||
	    sec_cam_info->type == HW_SUPPORT_ENC_TYPE_GCMSMS4)
		dctl_secinfo_entry->wapi_ctrl = 1;
	else
		dctl_secinfo_entry->wapi_ctrl = 0;

	ret = insert_key_to_dmac_tbl_8852c(adapter, mac_id, key_type, key_id,
					   sec_cam_info->sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	fill_sec_cam_info_8852c(adapter, sec_cam_info,
				(struct fwcmd_seccam_info *)sec_table, 0);

	ret = (u8)mac_upd_sec_infotbl_8852c(adapter,
					    (struct fwcmd_seccam_info *)sec_table);
	if (ret != MACSUCCESS)
		return ret;
	m_security_cam_hal_8852c(adapter, sec_cam_info,
				 mac_id, key_id, key_type,
				 sec_cam_idx, 0);

	return MACSUCCESS;
}

u32 mac_sta_search_key_idx_8852c(struct mac_ax_adapter *adapter,
				 u8 mac_id, u8 key_id, u8 key_type)
{
	u32 sec_cam_idx = 0;
	struct sec_cam_entry_t *s_entry = NULL;
	struct sec_cam_table_t *sec_cam_table =
		adapter->hw_info->sec_cam_table;

	if (!sec_cam_table)
		return MACNOKEYINDEX;

	/*Search SEC CAM Table */
	for (sec_cam_idx = 0; sec_cam_idx < SEC_CAM_ENTRY_NUM; sec_cam_idx++) {
		/* Only search valid entry */
		if (sec_cam_table->sec_cam_entry[sec_cam_idx]->valid == VALID) {
			s_entry = sec_cam_table->sec_cam_entry[sec_cam_idx];

			if (s_entry->mac_id != mac_id)
				continue;
			if (s_entry->key_id != key_id)
				continue;
			if (s_entry->key_type != key_type)
				continue;

			return sec_cam_idx;
		}
	}
	return MACNOKEYINDEX;
}

u32 mac_sta_hw_security_support_8852c(struct mac_ax_adapter *adapter,
				      u8 hw_security_support_type, u8 enable)
{
	u32 val32 = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (enable) {
	case 1:
		val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
		switch (hw_security_support_type) {
		case SEC_TX_ENC:
			val32 |= B_AX_SEC_TX_ENC;
			break;
		case SEC_RX_ENC:
			val32 |= B_AX_SEC_RX_DEC;
			break;
		case SEC_BC_ENC:
			val32 |= B_AX_BC_DEC;
			break;
		case SEC_MC_ENC:
			val32 |= B_AX_MC_DEC;
			break;
		case SEC_UC_MGNT_ENC:
			val32 |= B_AX_UC_MGNT_DEC;
			break;
		case SEC_BMC_MGNT_ENC:
			val32 |= B_AX_BMC_MGNT_DEC;
			break;
		default:
			break;
		}
		MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);
		break;
	case 0:
		val32 = MAC_REG_R32(R_AX_SEC_ENG_CTRL);
		switch (hw_security_support_type) {
		case SEC_TX_ENC:
			val32 &= ~B_AX_SEC_TX_ENC;
			break;
		case SEC_RX_ENC:
			val32 &= ~B_AX_SEC_RX_DEC;
			break;
		case SEC_BC_ENC:
			val32 &= ~B_AX_BC_DEC;
			break;
		case SEC_MC_ENC:
			val32 &= ~B_AX_MC_DEC;
			break;
		case SEC_UC_MGNT_ENC:
			val32 &= ~B_AX_UC_MGNT_DEC;
			break;
		case SEC_BMC_MGNT_ENC:
			val32 &= ~B_AX_BMC_MGNT_DEC;
			break;
		default:
			break;
		}

		MAC_REG_W32(R_AX_SEC_ENG_CTRL, val32);
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u8 check_key_type_8852c(u8 addr_cam_sec_mode, u8 keyidx)
{
	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		if (keyidx <= 6)
			return SEC_CAM_KEY_TYPE_UNI;
	break;

	case ADDR_CAM_SEC_MODE_ONE:
		if (keyidx <= 6)
			return SEC_CAM_KEY_TYPE_UNI;
	break;

	case ADDR_CAM_SEC_MODE_TWO:
		if (keyidx <= 1)
			return SEC_CAM_KEY_TYPE_UNI;

		else if (keyidx >= 2 && keyidx <= 4)
			return SEC_CAM_KEY_TYPE_GROUP;

		else if (keyidx >= 5 && keyidx <= 6)
			return SEC_CAM_KEY_TYPE_BIP;

		else
			break;
	break;

	case ADDR_CAM_SEC_MODE_THREE:
		if (keyidx <= 1)
			return SEC_CAM_KEY_TYPE_UNI;

		else if (keyidx >= 2 && keyidx <= 5)
			return SEC_CAM_KEY_TYPE_GROUP;

		else if (keyidx == 6)
			return SEC_CAM_KEY_TYPE_BIP;

		else
			break;
	break;
	}

	return MACKEYNOTEXT;
}

u32 refresh_security_cam_info_8852c(struct mac_ax_adapter *adapter,
				    struct mac_ax_sec_iv_info *sec_iv_info)
{
	u32 dtbl_address = 0;
	u8 i = 0;
	u8 key_valid[7] = {0}, key_cam_index[7] = {0}, sec_ent_keyid[7] = {0};
	u8 mac_id = 0, key_id_sh = 0, key_cam_idx_sh = 0;
	u8 sec_iv_sh = 0;
	u8 key_valid_byte = 0, key_valid_byte_ori = 0, key_type = 0;
	u8 keyidx = 0, sec_ent_mode = 0, sec_cam_idx = 0;
	u32 dword[10] = {0};
	u8 dmac_tbl_size = DCTRL_TBL_SIZE_8852C;

	struct sec_cam_entry_t *s_entry = NULL;
	struct sec_cam_table_t *sec_cam_table = adapter->hw_info->sec_cam_table;

	if (!sec_cam_table)
		return MACSUCCESS;

	mac_id = sec_iv_info->macid;

	/*read HW key in dmac tbl*/
	PLTFM_MSG_ERR("%s ind access macid %d start\n", __func__, mac_id);
	dtbl_address = mac_id * dmac_tbl_size;
	for (i = 0; i < 8; i++)
		dword[i] = mac_sram_dbg_read(adapter, dtbl_address + (i * 4),
					     DMAC_TBL_SEL);
	PLTFM_MSG_ERR("%s ind access macid %d end\n", __func__, mac_id);

	/* parse sec info from read dmac tbl */
	sec_iv_sh = 0;
	for (i = 0; i < 2; i++) {
		sec_iv_sh = (i + 2) * 8;
		sec_iv_info->ptktxiv[i] =
			(dword[0] >> sec_iv_sh) &
			DMAC_TBL_IV_BYTE_MSK;
	}

	sec_iv_sh = 0;
	for (i = 2; i < 6; i++) {
		sec_iv_sh = (i - 2) * 8;
		sec_iv_info->ptktxiv[i] =
			(dword[1] >> sec_iv_sh) &
			DMAC_TBL_IV_BYTE_MSK;
	}

	sec_ent_mode = (dword[4] >> SEC_MODE_SH) & SEC_MODE_MSK;

	key_id_sh = SEC_ENT0_KEYID_SH;
	for (i = 0; i < 7; i++) {
		sec_ent_keyid[i] = (dword[4] >> key_id_sh) & KEYID_MSK;
		key_id_sh += 2;
	}

	key_valid_byte = dword[5] & KEY_VALID_MSK;
	key_valid_byte_ori = key_valid_byte;

	for (i = 0; i < 7; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
	}

	key_cam_idx_sh = 0;
	for (i = 0; i < 3; i++) {
		key_cam_idx_sh = (i + 1) * 8;
		key_cam_index[i] = (dword[5] >> key_cam_idx_sh) & KEY_CAM_IDX_MSK;
	}

	key_cam_idx_sh = 0;
	for (i = 3; i < 7; i++) {
		key_cam_idx_sh = (i - 3) * 8;
		key_cam_index[i] = (dword[6] >> key_cam_idx_sh) & KEY_CAM_IDX_MSK;
	}

	// clear halmac table
	for (sec_cam_idx = 0; sec_cam_idx < SEC_CAM_ENTRY_NUM; sec_cam_idx++) {
		// Only search valid entry
		if (sec_cam_table->sec_cam_entry[sec_cam_idx]->valid == VALID) {
			s_entry = sec_cam_table->sec_cam_entry[sec_cam_idx];

			if (s_entry->mac_id == mac_id) {
				m_security_cam_hal_8852c(adapter, NULL,
							 mac_id, DEFAULT_KEYID,
							 DEFAULT_KEYTYPE,
							 sec_cam_idx, 1);
			}
		}
	}

	// insert halmac table
	for (keyidx = 0; keyidx < 7; keyidx++) {
		if (key_valid[keyidx] == VALID) {
			key_type = check_key_type_8852c(sec_ent_mode, keyidx);
			m_security_cam_hal_8852c(adapter, NULL,
						 mac_id, sec_ent_keyid[keyidx],
						 key_type,
						 key_cam_index[keyidx], 0);
		}
	}

	return MACSUCCESS;
}

u32 mac_wowlan_secinfo_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_sec_iv_info *sec_iv_info)
{
	u8 mac_id = 0;
	u32 ret = 0;
	struct mac_role_tbl *role = NULL;
	struct dctl_secinfo_entry_t *dctl_secinfo_entry = NULL;
	struct dctl_sec_info_t *dctl_sec_info = adapter->hw_info->dctl_sec_info;

	/*check dmac tbl is valid*/
	if (!dctl_sec_info)
		return MACNOKEYINDEX;

	if (!sec_iv_info)
		return MACNOBUF;

	mac_id = sec_iv_info->macid;
	/*check mac_id role is valid*/
	role = mac_role_srch(adapter, mac_id);
	if (!role)
		return MACNOROLE;

	/*get some decide key index info*/
	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];

	switch (sec_iv_info->opcode) {
	case SEC_IV_UPD_TYPE_WRITE:
		dctl_secinfo_entry->aes_iv_l =
		cpu_to_le16(SET_WORD(sec_iv_info->ptktxiv[0], DCTRL_TBL_SEC_IV0) |
			    SET_WORD(sec_iv_info->ptktxiv[1], DCTRL_TBL_SEC_IV1));

		dctl_secinfo_entry->aes_iv_h =
		cpu_to_le32(SET_WORD(sec_iv_info->ptktxiv[4], DCTRL_TBL_SEC_IV2) |
			    SET_WORD(sec_iv_info->ptktxiv[5], DCTRL_TBL_SEC_IV3) |
			    SET_WORD(sec_iv_info->ptktxiv[6], DCTRL_TBL_SEC_IV4) |
			    SET_WORD(sec_iv_info->ptktxiv[7], DCTRL_TBL_SEC_IV5));

		ret = upt_dctl_secinfo_entry_8852c(adapter, mac_id,
						   DCTL_TBL_NONE_UPD_MODE,
						   SEC_IV_UPD_TYPE_WRITE);

		if (ret != MACSUCCESS)
			return ret;

		break;

	case SEC_IV_UPD_TYPE_READ:
		ret = refresh_security_cam_info_8852c(adapter, sec_iv_info);
		break;

	default:
		break;
	}

	return MACSUCCESS;
}

