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
#include "security_cam.h"

#define ADDR_CAM_SECINFO_OFFSET 0x1C
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

u32 sec_info_tbl_init(struct mac_ax_adapter *adapter)
{
	u8 i = 0;
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
	return MACSUCCESS;
}

u32 free_sec_info_tbl(struct mac_ax_adapter *adapter)
{
	u8 i;
	struct sec_cam_table_t *sec_cam_table = adapter->hw_info->sec_cam_table;

	if (!sec_cam_table)
		return MACSUCCESS;

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
	return MACSUCCESS;
}

u32 fill_sec_cam_info(struct mac_ax_adapter *adapter,
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

u32 fill_addr_cam_sec_only(struct mac_ax_adapter *adapter,
			   struct mac_ax_role_info *info,
			   struct addr_sec_only_info *addr_sec_info)
{
	struct mac_ax_addr_cam_info a_info;

	a_info = info->a_info;

	addr_sec_info->dword0 =
	cpu_to_le32((a_info.aid12 & 0xfff) |
		    ((a_info.wol_pattern) ? FWCMD_H2C_ADDRCAM_INFO_WOL_PATTERN
		     : 0) |
		    ((a_info.wol_uc) ? FWCMD_H2C_ADDRCAM_INFO_WOL_UC : 0) |
		    ((a_info.wol_magic) ?
		     FWCMD_H2C_ADDRCAM_INFO_WOL_MAGIC : 0) |
		    ((a_info.wapi) ? FWCMD_H2C_ADDRCAM_INFO_WAPI : 0) |
		    SET_WORD(a_info.sec_ent_mode,
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT_MODE) |
		    SET_WORD(a_info.sec_ent_keyid[0],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT0_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[1],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT1_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[2],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT2_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[3],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT3_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[4],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT4_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[5],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT5_KEYID) |
		    SET_WORD(a_info.sec_ent_keyid[6],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT6_KEYID));

	addr_sec_info->dword1 =
	  cpu_to_le32(SET_WORD(a_info.sec_ent_valid,
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT_VALID) |
		      SET_WORD(a_info.sec_ent[0],
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT0) |
		      SET_WORD(a_info.sec_ent[1],
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT1) |
		      SET_WORD(a_info.sec_ent[2],
			       FWCMD_H2C_ADDRCAM_INFO_SEC_ENT2));

	addr_sec_info->dword2 =
	cpu_to_le32(SET_WORD(a_info.sec_ent[3],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT3) |
		    SET_WORD(a_info.sec_ent[4],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT4) |
		    SET_WORD(a_info.sec_ent[5],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT5) |
		    SET_WORD(a_info.sec_ent[6],
			     FWCMD_H2C_ADDRCAM_INFO_SEC_ENT6));

	return MACSUCCESS;
}

u32 mac_upd_sec_infotbl(struct mac_ax_adapter *adapter,
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

		// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
		// The H2C shall be sent by mac_h2c_agg_tx.
		ret = h2c_agg_enqueue(adapter, h2cb);
		if (ret == MACSUCCESS)
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

		return MACSUCCESS;
	}

	return ret;
}

u8 check_key_index(u8 addr_cam_sec_mode, u8 key_type, u8 key_index)
{
	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (key_index <= 6)
				return MACSUCCESS;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_ONE:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (key_index <= 6)
				return MACSUCCESS;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_TWO:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (key_index <= 1)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_GROUP:
			if (key_index >= 2 && key_index <= 4)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_BIP:
			if (key_index >= 5 && key_index <= 6)
				return MACSUCCESS;
			break;
		default:
			break;
		}
		break;
	case ADDR_CAM_SEC_MODE_THREE:
		switch (key_type) {
		case SEC_CAM_KEY_TYPE_UNI:
			if (key_index <= 1)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_GROUP:
			if (key_index >= 2 && key_index <= 5)
				return MACSUCCESS;
			break;
		case SEC_CAM_KEY_TYPE_BIP:
			if (key_index == 6)
				return MACSUCCESS;
			break;
		default:
			break;
		}
		break;
	}
	return 1;
}

u8 decide_key_index(u8 addr_cam_sec_mode, u8 key_type)
{
	u8 key_index = 0;

	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			key_index = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			return MACWNGKEYTYPE;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			return MACWNGKEYTYPE;
		break;
	case ADDR_CAM_SEC_MODE_ONE:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			key_index = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			return MACWNGKEYTYPE;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			return MACWNGKEYTYPE;
		break;
	case ADDR_CAM_SEC_MODE_TWO:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			key_index = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			key_index = 2;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			key_index = 5;
		break;
	case ADDR_CAM_SEC_MODE_THREE:
		if (key_type == SEC_CAM_KEY_TYPE_UNI)
			key_index = 0;
		else if (key_type == SEC_CAM_KEY_TYPE_GROUP)
			key_index = 2;
		else if (key_type == SEC_CAM_KEY_TYPE_BIP)
			key_index = 6;
		break;
	default:
		break;
	}
	return key_index;
}

u8 decide_sec_cam_index(struct mac_ax_adapter *adapter, u8 *sec_cam_idx)
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

u8 delete_key_from_addr_cam(struct mac_ax_adapter *adapter,
			    struct mac_role_tbl *role, u8 key_type,
			    u8 key_id, u8 *sec_cam_idx)
{
	u32 ret = 0;
	u8 key_index = 0, key_valid_byte = 0, i = 0;
	u8 key_valid[7] = {0}, hit_flag = 0;

	key_valid_byte = role->info.a_info.sec_ent_valid;

	key_index = decide_key_index(role->info.a_info.sec_ent_mode, key_type);

	if (key_index == MACWNGKEYTYPE)
		return MACWNGKEYTYPE;

	for (i = 0; i < 7; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
	}

	/* find the key real storage idx */
	for (i = key_index; i < 7; i++) {
		if (role->info.a_info.sec_ent_keyid[i] == key_id &&
		    key_valid[i] == VALID) {
			key_index = i;
			hit_flag = 1;
			break;
		}
	}

	if (hit_flag == 0)
		return MACKEYNOTEXT;

	if (check_key_index(role->info.a_info.sec_ent_mode,
			    key_type, key_index)) {
		PLTFM_MSG_TRACE("check addr key index full\n");
		return MACADDRCAMKEYFL;
	}

	/*get the key cam index*/
	*sec_cam_idx = role->info.a_info.sec_ent[key_index];

	role->info.a_info.sec_ent_keyid[key_index] = 0;
	role->info.a_info.sec_ent_valid &= ~(BIT(key_index));
	role->info.a_info.sec_ent[key_index] = 0;

	/*  update addr cam */
	ret = mac_upd_addr_cam(adapter, &role->info, CHG);
	if (ret == MACBSSIDCAMFL) {
		PLTFM_MSG_ERR("[ERR]: BSSID CAM full\n");
		return MACBSSIDCAMFL;
	}

	return MACSUCCESS;
}

u8 insert_key_to_addr_cam(struct mac_ax_adapter *adapter,
			  struct mac_role_tbl *role, u8 key_type,
			  u8 key_id, u8 sec_cam_idx)
{
	u32 ret = 0;
	u8 key_index = 0, key_valid_byte = 0, i = 0;
	u8 key_valid[7] = {0}, hit_flag = 0;

	key_valid_byte = role->info.a_info.sec_ent_valid;
	key_index = decide_key_index(role->info.a_info.sec_ent_mode, key_type);

	if (key_index == MACWNGKEYTYPE)
		return MACWNGKEYTYPE;

	for (i = 0; i < 7; i++) {
		key_valid[i] = key_valid_byte % 2;
		key_valid_byte = key_valid_byte >> 1;
	}

	/* find the key real storage idx */
	for (i = key_index; i < 7; i++) {
		if (key_valid[i] != VALID) {
			key_index = i;
			hit_flag = 1;
			break;
		}
	}
	if (hit_flag == 0)
		return MACADDRCAMKEYFL;

	if (check_key_index(role->info.a_info.sec_ent_mode,
			    key_type, key_index)) {
		PLTFM_MSG_TRACE("check addr key index full\n");
		return MACADDRCAMKEYFL;
	}

	role->info.a_info.sec_ent_keyid[key_index] = key_id;
	role->info.a_info.sec_ent_valid |= BIT(key_index);
	role->info.a_info.sec_ent[key_index] = sec_cam_idx;

	ret = mac_upd_addr_cam(adapter, &role->info, CHG);
	if (ret == MACBSSIDCAMFL) {
		PLTFM_MSG_ERR("[ERR]: BSSID CAM full\n");
		return MACBSSIDCAMFL;
	}

	return MACSUCCESS;
}

u32 m_security_cam_hal(struct mac_ax_adapter *adapter,
		       struct mac_ax_sec_cam_info *sec_cam_info,
		       u8 mac_id, u8 key_id, u8 key_type,
		       u8 sec_cam_idx, u8 clear)
{
	u8 i = 0;
	struct sec_cam_entry_t *sec_cam_entry = NULL;
	struct sec_cam_table_t *sec_cam_table =
		adapter->hw_info->sec_cam_table;

	if (!sec_cam_table)
		return MACSUCCESS;

	/* patch for read wrong value from HW*/
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

u32 disconnect_flush_key(struct mac_ax_adapter *adapter,
			 struct mac_role_tbl *role)
{
	struct mac_ax_sec_cam_info *sec_cam_info = NULL;
	u8 i, sec_ent_valid, mac_id, key_type, key_index;
	u8 key_valid[7] = {0}, sec_cam_index[7] = {0}, sec_ent_keyid[7] = {0};

	sec_ent_valid = role->info.a_info.sec_ent_valid;
	mac_id = role->info.a_info.sec_ent_valid;

	for (i = 0; i < 7; i++) {
		key_valid[i] = sec_ent_valid % 2;
		sec_ent_valid = sec_ent_valid >> 1;
		sec_cam_index[i] = role->info.a_info.sec_ent[i];
		sec_ent_keyid[i] = role->info.a_info.sec_ent_keyid[i];
	}

	key_type = SEC_CAM_KEY_TYPE_DEFAULT;
	for (key_index = 0; key_index < 7; key_index++) {
		if (key_valid[key_index] == VALID) {
			m_security_cam_hal(adapter, sec_cam_info,
					   mac_id, sec_ent_keyid[key_index],
					   key_type, sec_cam_index[key_index],
					   1);
		}
	}
	return MACSUCCESS;
}

u32 mac_sta_del_key(struct mac_ax_adapter *adapter,
		    u8 mac_id,
		    u8 key_id,
		    u8 key_type)
{
	struct mac_ax_sec_cam_info sec_cam_info;
	struct mac_role_tbl *role = NULL;
	u8 sec_cam_idx = 0;
	u32 sec_table[6] = {0}, ret = 0;

	role = mac_role_srch(adapter, mac_id);

	if (!role)
		return MACNOROLE;

	ret = delete_key_from_addr_cam(adapter, role, key_type,
				       key_id, &sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	sec_cam_info.sec_cam_idx = sec_cam_idx;
	sec_cam_info.offset = 0x00;
	sec_cam_info.len = SEC_CAM_ENTRY_SIZE;

	fill_sec_cam_info(adapter, &sec_cam_info,
			  (struct fwcmd_seccam_info *)sec_table, 1);

	ret = (u8)mac_upd_sec_infotbl(adapter,
				      (struct fwcmd_seccam_info *)sec_table);
	if (ret != MACSUCCESS)
		return ret;

	m_security_cam_hal(adapter, NULL,
			   mac_id, key_id, key_type,
			   sec_cam_idx, 1);

	return MACSUCCESS;
}

u32 mac_sta_add_key(struct mac_ax_adapter *adapter,
		    struct mac_ax_sec_cam_info *sec_cam_info,
		    u8 mac_id,
		    u8 key_id,
		    u8 key_type)
{
	u8 sec_cam_idx = 0;
	u32 sec_table[6] = {0}, ret = 0;
	struct mac_role_tbl *role = NULL;

	if (sec_cam_info->type > HW_SUPPORT_ENC_TYPE_NUM)
		return MACWNGKEYTYPE;

	role = mac_role_srch(adapter, mac_id);
	if (!role)
		return MACNOROLE;

	ret = decide_sec_cam_index(adapter, &sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	sec_cam_info->sec_cam_idx = sec_cam_idx;
	sec_cam_info->offset = 0x00;
	sec_cam_info->len = SEC_CAM_ENTRY_SIZE;
	ret = insert_key_to_addr_cam(adapter, role, key_type, key_id,
				     sec_cam_info->sec_cam_idx);
	if (ret != MACSUCCESS)
		return ret;

	fill_sec_cam_info(adapter, sec_cam_info,
			  (struct fwcmd_seccam_info *)sec_table, 0);

	ret = (u8)mac_upd_sec_infotbl(adapter,
				      (struct fwcmd_seccam_info *)sec_table);
	if (ret != MACSUCCESS)
		return ret;
	m_security_cam_hal(adapter, sec_cam_info,
			   mac_id, key_id, key_type,
			   sec_cam_idx, 0);

	return MACSUCCESS;
}

u32 mac_sta_search_key_idx(struct mac_ax_adapter *adapter,
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

u32 mac_sta_hw_security_support(struct mac_ax_adapter *adapter,
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

u8 check_key_type(u8 addr_cam_sec_mode, u8 key_index)
{
	switch (addr_cam_sec_mode) {
	case ADDR_CAM_SEC_MODE_ZERO:
		if (key_index <= 6)
			return SEC_CAM_KEY_TYPE_UNI;
	break;

	case ADDR_CAM_SEC_MODE_ONE:
		if (key_index <= 6)
			return SEC_CAM_KEY_TYPE_UNI;
	break;

	case ADDR_CAM_SEC_MODE_TWO:
		if (key_index <= 1)
			return SEC_CAM_KEY_TYPE_UNI;

		else if (key_index >= 2 && key_index <= 4)
			return SEC_CAM_KEY_TYPE_GROUP;

		else if (key_index >= 5 && key_index <= 6)
			return SEC_CAM_KEY_TYPE_BIP;

		else
			break;
	break;

	case ADDR_CAM_SEC_MODE_THREE:
		if (key_index <= 1)
			return SEC_CAM_KEY_TYPE_UNI;

		else if (key_index >= 2 && key_index <= 5)
			return SEC_CAM_KEY_TYPE_GROUP;

		else if (key_index == 6)
			return SEC_CAM_KEY_TYPE_BIP;

		else
			break;
	break;
	}

	return MACKEYNOTEXT;
}

u32 refresh_security_cam_info(struct mac_ax_adapter *adapter,
			      u8 mac_id)
{
	u32 addr_idx = 0, cam_address = 0;
	u32 i = 0;
	u8 key_valid[7] = {0}, key_cam_index[7] = {0}, sec_ent_keyid[7] = {0};
	u8 macid = 0, hit_flag = VALID, key_id_sh = 0, key_cam_idx_sh = 0;
	u8 key_valid_byte = 0, key_valid_byte_ori = 0, key_type = 0;
	u8 key_index = 0, sec_ent_mode = 0, sec_cam_idx = 0;
	u32 dword[10] = {0};
	u8 addr_cam_size = get_addr_cam_size(adapter);

	struct sec_cam_entry_t *s_entry = NULL;
	struct sec_cam_table_t *sec_cam_table = adapter->hw_info->sec_cam_table;
	struct mac_role_tbl *role = NULL;

	/*read HW key in address cam */
	for (addr_idx = 0; addr_idx < 0x80; addr_idx++) {
		hit_flag = INVALID;

		PLTFM_MSG_WARN("%s ind access macid %d start\n", __func__, mac_id);
		cam_address = addr_idx * addr_cam_size;
		for (i = 0; i < 10; i++) {
			dword[i] = mac_sram_dbg_read(adapter, cam_address + (i * 4),
						     ADDR_CAM_SEL);
			PLTFM_MSG_WARN("CAMADDR<%x>=%x\n", cam_address, dword[i]);
		}
		PLTFM_MSG_WARN("%s ind access macid %d end\n", __func__, mac_id);

		if ((dword[0] & ADDRCAM_VALID) == VALID) {
			macid = (dword[6] & ADDRCAM_MACID_MSK);

			if (macid == mac_id) {
				sec_ent_mode = (dword[7] >> ADDRCAM_SEC_MODE_SH)
						& ADDRCAM_SECMODE_MSK;

				key_id_sh = ADDRCAM_SEC_ENT0_KEYID_SH;
				for (i = 0; i < 7; i++) {
					sec_ent_keyid[i] =
						(dword[7] >> key_id_sh) &
						ADDRCAM_KEYID_MSK;
					key_id_sh += 2;
				}

				key_valid_byte =
					dword[8] & ADDRCAM_KEY_VALID_MSK;
				key_valid_byte_ori = key_valid_byte;

				for (i = 0; i < 7; i++) {
					key_valid[i] = key_valid_byte % 2;
					key_valid_byte = key_valid_byte >> 1;
				}

				key_cam_idx_sh = 0;
				for (i = 0; i < 3; i++) {
					key_cam_idx_sh = (i + 1) * 8;
					key_cam_index[i] =
						(dword[8] >> key_cam_idx_sh) &
						ADDRCAM_KEY_CAM_IDX_MSK;
				}

				key_cam_idx_sh = 0;
				for (i = 3; i < 7; i++) {
					key_cam_idx_sh = (i - 3) * 8;
					key_cam_index[i] =
						(dword[9] >> key_cam_idx_sh) &
						ADDRCAM_KEY_CAM_IDX_MSK;
				}

				hit_flag = VALID;
				break;
			}
		}
	}

	if (hit_flag == INVALID) {
		PLTFM_MSG_ALWAYS("MACID : %d not exist\n", mac_id);
		return MACNOROLE;
	}

	// clear halmac table
	for (sec_cam_idx = 0; sec_cam_idx < SEC_CAM_ENTRY_NUM; sec_cam_idx++) {
		// Only search valid entry
		if (sec_cam_table->sec_cam_entry[sec_cam_idx]->valid == VALID) {
			s_entry = sec_cam_table->sec_cam_entry[sec_cam_idx];

			if (s_entry->mac_id == mac_id) {
				m_security_cam_hal(adapter, NULL,
						   mac_id, DEFAULT_KEYID,
						   DEFAULT_KEYTYPE,
						   sec_cam_idx, 1);
			}
		}
	}

	// insert halmac table
	for (key_index = 0; key_index < 7; key_index++) {
		if (key_valid[key_index] == VALID) {
			key_type = check_key_type(sec_ent_mode, key_index);
			m_security_cam_hal(adapter, NULL,
					   mac_id, sec_ent_keyid[key_index],
					   key_type,
					   key_cam_index[key_index], 0);
		}
	}

	// write back to address cam sec part
	role = mac_role_srch(adapter, mac_id);
	if (!role) {
		PLTFM_MSG_ALWAYS("%s mac_role_srch fail\n", __func__);
		return MACNOROLE;
	}

	role->info.a_info.sec_ent_valid = key_valid_byte_ori;
	for (i = 0; i < 7; i++) {
		role->info.a_info.sec_ent_keyid[i] = sec_ent_keyid[i];
		role->info.a_info.sec_ent[i] = key_cam_index[i];
	}

	return MACSUCCESS;
}

u32 mac_wowlan_secinfo(struct mac_ax_adapter *adapter,
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

	mac_id = sec_iv_info->macid;
	/*check mac_id role is valid*/
	role = mac_role_srch(adapter, mac_id);
	if (!role)
		return MACNOROLE;

	/*get some decide key index info*/
	dctl_secinfo_entry = dctl_sec_info->dctl_secinfo_entry[mac_id];

	switch (sec_iv_info->opcode) {
	case SEC_IV_UPD_TYPE_WRITE:
		break;
	case SEC_IV_UPD_TYPE_READ:
		ret = refresh_security_cam_info(adapter, mac_id);
		break;
	default:

		break;
	}

	return MACSUCCESS;
}