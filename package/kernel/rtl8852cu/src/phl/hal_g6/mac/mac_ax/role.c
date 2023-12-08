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

#include "role.h"
#include "mac_priv.h"

static u32 _add_role(struct mac_ax_adapter *adapter,
		     struct mac_ax_role_info *info);

static u32 _change_role(struct mac_ax_adapter *adapter,
			struct mac_ax_role_info *info);

static u32 _remove_role(struct mac_ax_adapter *adapter, u8 macid);

static u32 _remove_role_soft(struct mac_ax_adapter *adapter, u8 macid);

static u32 _remove_role_by_band(struct mac_ax_adapter *adapter, u8 band, u8 sw);

static struct mac_role_tbl *_role_srch(struct mac_ax_adapter *adapter, u8 macid);

static u32 _role_sync(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info);

static inline void __role_queue_head_init(struct mac_ax_adapter *adapter,
					  struct mac_role_tbl_head *list)
{
	list->prev = (struct mac_role_tbl *)list;
	list->next = (struct mac_role_tbl *)list;
	list->qlen = 0;
}

static inline void __role_insert(struct mac_ax_adapter *adapter,
				 struct mac_role_tbl *new_role,
				 struct mac_role_tbl *prev,
				 struct mac_role_tbl *next,
				 struct mac_role_tbl_head *list)
{
	new_role->next = next;
	new_role->prev = prev;
	next->prev  = new_role;
	prev->next = new_role;
	list->qlen++;
}

static inline void __role_queue_before(struct mac_ax_adapter *adapter,
				       struct mac_role_tbl_head *list,
				       struct mac_role_tbl *next,
				       struct mac_role_tbl *new_role)
{
	__role_insert(adapter, new_role, next->prev, next, list);
}

static inline void __role_unlink(struct mac_ax_adapter *adapter,
				 struct mac_role_tbl *role,
				 struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *next, *prev;

	list->qlen--;
	next = role->next;
	prev = role->prev;
	role->prev = NULL;
	role->next = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct mac_role_tbl *__role_peek(struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *role = list->next;

	if (role == (struct mac_role_tbl *)list)
		role = NULL;

	return role;
}

static struct mac_role_tbl *role_alloc(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl *role;

	role = (struct mac_role_tbl *)PLTFM_MALLOC(sizeof(struct mac_role_tbl));
	if (!role)
		return NULL;

	PLTFM_MEMSET(role, 0, sizeof(struct mac_role_tbl));

	return role;
}

static inline u32 role_queue_len(struct mac_role_tbl_head *list)
{
	return list->qlen;
}

static inline void role_queue_head_init(struct mac_ax_adapter *adapter,
					struct mac_role_tbl_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__role_queue_head_init(adapter, list);
}

static inline void role_enqueue(struct mac_ax_adapter *adapter,
				struct mac_role_tbl_head *list,
				struct mac_role_tbl *new_role)
{
	__role_queue_before(adapter,
			    list, (struct mac_role_tbl *)list, new_role);
}

static inline struct mac_role_tbl *role_dequeue(struct mac_ax_adapter *adapter,
						struct mac_role_tbl_head *list)
{
	struct mac_role_tbl *role = NULL;

	role = __role_peek(list);
	if (role)
		__role_unlink(adapter, role, list);

	return role;
}

static struct mac_role_tbl *role_get(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *role_pool;

	if (adapter->role_tbl)
		role_pool = adapter->role_tbl->role_tbl_pool;
	else
		return NULL;

	if (!role_pool->qlen)
		role = NULL;
	else
		role = role_dequeue(adapter, role_pool);

	return role;
}

static void role_return(struct mac_ax_adapter *adapter,
			struct mac_role_tbl *role)
{
	struct mac_role_tbl_head *role_head = adapter->role_tbl;
	struct mac_role_tbl_head *role_pool = NULL;

	if (role_head)
		role_pool = adapter->role_tbl->role_tbl_pool;
	else
		return;

	__role_unlink(adapter, role, role_head);
	role_enqueue(adapter, role_pool, role);
}

static u32 role_free(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *role_head;
	struct mac_role_tbl_head *role_pool = NULL;
	u32 ret = MACSUCCESS;

	role_head = adapter->role_tbl;
	if (role_head) {
		role_pool = role_head->role_tbl_pool;
		while (role_queue_len(role_head) > 0) {
			role = role_dequeue(adapter, role_head);
			if (role) {
				PLTFM_FREE(role, sizeof(struct mac_role_tbl));
			} else {
				ret = MACMEMRO;
				break;
			}
		}
	}
	if (role_pool) {
		while (role_queue_len(role_pool) > 0) {
			role = role_dequeue(adapter, role_pool);
			if (role) {
				PLTFM_FREE(role, sizeof(struct mac_role_tbl));
			} else {
				ret = MACMEMRO;
				break;
			}
		}
	}

	return ret;
}

u32 role_info_valid(struct mac_ax_adapter *adapter,
		    struct mac_ax_role_info *info,
		    enum mac_ax_role_opmode op)
{
	// Check info is NULL or not
	if (!info) {
		PLTFM_MSG_ERR("role info is null\n");
		return MACNPTR;
	}

	if (op == ADD) {
		// check add role opmode
		if (info->opmode != MAC_AX_ROLE_DISCONN) {
			PLTFM_MSG_ERR("add_role, opmode should equals to:\n");
			PLTFM_MSG_ERR("MAC_AX_ROLE_DISCONN\n");
			return MACFUNCINPUT;
		}
		// check add role updmode
		if (info->upd_mode != MAC_AX_ROLE_CREATE) {
			PLTFM_MSG_ERR("add_role, updmode should equals to:\n");
			PLTFM_MSG_ERR("MAC_AX_ROLE_CREATE\n");
			return MACFUNCINPUT;
		}
		// Check macid exist or not
		if (_role_srch(adapter, info->macid)) {
			PLTFM_MSG_ERR("existing macid: %d\n", info->macid);
			return MACSAMACID;
		}
	} else if (op == CHG) {
		if (info->upd_mode == MAC_AX_ROLE_CREATE) {
			PLTFM_MSG_ERR("change_role, updmode should not:\n");
			PLTFM_MSG_ERR("equals to MAC_AX_ROLE_CREATE\n");
			return MACFUNCINPUT;
		}
		if (!_role_srch(adapter, info->macid)) {
			PLTFM_MSG_ERR("macid not exist: %d\n", info->macid);
			return MACNOITEM;
		}
	}

	// If net_type is IFRA, self_role should only be CLIENT
	if (info->net_type == MAC_AX_NET_TYPE_INFRA &&
	    (info->self_role == MAC_AX_SELF_ROLE_AP ||
	     info->self_role == MAC_AX_SELF_ROLE_AP_CLIENT)) {
		PLTFM_MSG_ERR("net_type is INFRA, self_role can only be:\n");
		PLTFM_MSG_ERR("CLIENT\n");
			return MACFUNCINPUT;
	}

	// If net_type is AP, self_role should be AP or AP_CLIENT
	if (info->net_type == MAC_AX_NET_TYPE_AP &&
	    info->self_role == MAC_AX_SELF_ROLE_CLIENT) {
		PLTFM_MSG_ERR("net_type is AP, self_role should be AP:\n");
		PLTFM_MSG_ERR("or AP_CLIENT\n");
			return MACFUNCINPUT;
	}

	// If self_role is AP, the SMA and TMA should equals to BSSID
	if (info->self_role == MAC_AX_SELF_ROLE_AP &&
	    info->net_type == MAC_AX_NET_TYPE_AP) {
		if (PLTFM_MEMCMP(info->self_mac, info->bssid, 6) ||
		    PLTFM_MEMCMP(info->target_mac, info->bssid, 6)) {
			PLTFM_MSG_ERR("self_role is AP, the SMA and TMA:\n");
			PLTFM_MSG_ERR("should equal to BSSID\n");
			return MACFUNCINPUT;
		}
	}

	// If is_mulitcast_entry = 1, group bit should equals to 1
	if (!(info->is_mul_ent ^
	       ((info->net_type != MAC_AX_NET_TYPE_AP &&
		 ((info->self_mac[0] & BIT0) == 0)) ||
		(info->net_type == MAC_AX_NET_TYPE_AP &&
		 ((info->bssid[0] & BIT0) == 0))))) {
		PLTFM_MSG_ERR("is_mul_ent = 1, group bit should = 1\n");
		return MACFUNCINPUT;
	}

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		if (info->wmm >= 2) {
			PLTFM_MSG_ERR("invalid wmm %d\n", info->wmm);
			return MACFUNCINPUT;
		}
		if (info->band == MAC_AX_BAND_1) {
			PLTFM_MSG_ERR("invalid band %d\n", info->band);
			return MACFUNCINPUT;
		}
	}

	if (info->band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("invalid band %d\n", info->band);
		return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

u32 role_tbl_init(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl *role = NULL;
	struct mac_role_tbl_head *role_head;
	struct mac_role_tbl_head *role_pool;
	u16 a_idx;
	u32 ret = 0;

	adapter->role_tbl = NULL;
	adapter->role_tbl = (struct mac_role_tbl_head *)
		PLTFM_MALLOC(sizeof(struct mac_role_tbl_head));
	if (!adapter->role_tbl) {
		PLTFM_MSG_ERR("%s memory alloc failed\n", __func__);
		return MACBUFALLOC;
	}

	adapter->role_tbl->role_tbl_pool = NULL;
	adapter->role_tbl->role_tbl_pool = (struct mac_role_tbl_head *)
		PLTFM_MALLOC(sizeof(struct mac_role_tbl_head));
	if (!adapter->role_tbl->role_tbl_pool) {
		PLTFM_MSG_ERR("%s memory alloc failed\n", __func__);
		return MACBUFALLOC;
	}

	role_head = adapter->role_tbl;
	role_pool = adapter->role_tbl->role_tbl_pool;

	role_queue_head_init(adapter, role_head);
	role_queue_head_init(adapter, role_pool);

	for (a_idx = 0; a_idx < adapter->hw_info->macid_num; a_idx++) {
		role = role_alloc(adapter);
		if (role) {
			PLTFM_MUTEX_LOCK(&role_pool->lock);
			role_enqueue(adapter, role_pool, role);
			PLTFM_MUTEX_UNLOCK(&role_pool->lock);
		} else {
			PLTFM_MSG_ERR("%s memory alloc failed\n", __func__);
			ret = MACBUFALLOC;
			break;
		}
	}

	if (a_idx < adapter->hw_info->macid_num)
		role_tbl_exit(adapter);
	else
		ret = MACSUCCESS;

	return ret;
}

u32 role_tbl_exit(struct mac_ax_adapter *adapter)
{
	struct mac_role_tbl_head *role_head = NULL;
	struct mac_role_tbl_head *role_pool = NULL;
	u32 ret = MACSUCCESS;

	role_head = adapter->role_tbl;

	if (!role_head) {
		PLTFM_MSG_ERR("%s role head list is null\n", __func__);
		return MACMEMRO;
	}

	role_pool = adapter->role_tbl->role_tbl_pool;

	if (!role_pool) {
		PLTFM_MSG_ERR("%s role pool list is null\n", __func__);
		return MACMEMRO;
	}

	PLTFM_MUTEX_LOCK(&role_head->lock);
	PLTFM_MUTEX_LOCK(&role_pool->lock);
	ret = role_free(adapter);
	PLTFM_MUTEX_UNLOCK(&role_pool->lock);
	PLTFM_MUTEX_UNLOCK(&role_head->lock);

	PLTFM_MUTEX_DEINIT(&role_head->lock);
	PLTFM_MUTEX_DEINIT(&role_pool->lock);

	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("%s memory free failed\n", __func__);

	if (adapter->role_tbl) {
		if (adapter->role_tbl->role_tbl_pool) {
			PLTFM_FREE(adapter->role_tbl->role_tbl_pool,
				   sizeof(struct mac_role_tbl_head));
			adapter->role_tbl->role_tbl_pool = NULL;
		}
		PLTFM_FREE(adapter->role_tbl,
			   sizeof(struct mac_role_tbl_head));
		adapter->role_tbl = NULL;
	}
	return ret;
}

u32 role_info_init(struct mac_ax_adapter *adapter,
		   struct mac_ax_role_info *info)
{
	u8 i;

	// address cam info
	info->a_info.len = get_addr_cam_size(adapter);
	info->a_info.offset = 0;
	info->a_info.mask_sel = info->mask_sel;
	info->a_info.addr_mask = info->addr_mask;
	info->a_info.hit_rule = info->hit_rule;
	info->a_info.bcn_hit_cond = info->bcn_hit_cond;
	info->a_info.is_mul_ent = info->is_mul_ent;
	info->a_info.port_int = info->port;
	info->a_info.tsf_sync = info->tsf_sync;
	info->a_info.lsig_txop = info->lsig_txop;
	info->a_info.tgt_ind = info->tgt_ind;
	info->a_info.frm_tgt_ind = info->frm_tgt_ind;
	info->a_info.wol_pattern = info->wol_pattern;
	info->a_info.wol_uc = info->wol_uc;
	info->a_info.wol_magic = info->wol_magic;
	info->a_info.bb_sel = info->band;
	info->a_info.tf_trs = info->trigger;
	info->a_info.aid12 = info->aid & 0xfff;
	info->a_info.macid = info->macid;
	info->a_info.wapi = info->wapi;
	info->a_info.sec_ent_mode = info->sec_ent_mode;
	info->a_info.net_type = info->net_type;

	// bssid cam info
	info->b_info.len = BSSID_CAM_ENT_SIZE;
	info->b_info.offset = 0;
	info->b_info.valid = 1;
	info->b_info.bss_color = info->bss_color;
	info->b_info.bb_sel = info->band;

	for (i = 0; i < 6; i++) {
		info->a_info.sma[i] = info->self_mac[i];
		info->a_info.tma[i] = info->target_mac[i];
		info->b_info.bssid[i] = info->bssid[i];
	}

	return MACSUCCESS;
}

u32 sec_info_init(struct mac_ax_adapter *adapter,
		  struct mac_ax_role_info *info)
{
	u8 i;

	info->a_info.sec_ent_valid = 0;
	for (i = 0; i < 7; i++) {
		info->a_info.sec_ent_keyid[i] = 0;
		info->a_info.sec_ent[i] = 0;
	}

	return MACSUCCESS;
}

u32 sec_info_deinit(struct mac_ax_adapter *adapter,
		    struct mac_ax_role_info *info,
		    struct mac_role_tbl *role)
{
	u32 i, ret;
	struct sec_cam_table_t *sec_cam_table = adapter->hw_info->sec_cam_table;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (!sec_cam_table)
		return MACSUCCESS;

	ret = p_ops->disconnect_flush_key(adapter, role);
	if (ret != MACSUCCESS)
		return ret;

	/* deinit info */
	info->a_info.sec_ent_mode = 0;
	info->a_info.sec_ent_valid = 0;
	for (i = 0; i < 7; i++) {
		info->a_info.sec_ent_keyid[i] = 0;
		info->a_info.sec_ent[i] = 0;
	}

	/* deinit role */
	role->info.a_info.sec_ent_mode = 0;
	role->info.a_info.sec_ent_valid = 0;
	for (i = 0; i < 7; i++) {
		role->info.a_info.sec_ent_keyid[i] = 0;
		role->info.a_info.sec_ent[i] = 0;
	}

	return MACSUCCESS;
}

u32 role_init(struct mac_ax_adapter *adapter,
	      struct mac_role_tbl *role,
	      struct mac_ax_role_info *info)
{
	struct mac_ax_macid_pause_cfg pause;
	u32 ret;

	role_info_init(adapter, info);

	pause.macid = info->macid;
	pause.pause = 0;
	ret = set_macid_pause(adapter, &pause);
	if (ret != MACSUCCESS)
		return ret;

	/* The definition of wmm is different between MAC & drivers
	 * MAC HW use wmm 0~3 to indicate
	 * phy0-wmm0, phy0-wmm1, phy1-wmm0, phy1-wmm1 correspondingly.
	 * Drivers use wmm 0/1 & band 0/1
	 * Notice that wmm mapping may be changed in DBCC feature,
	 * so we use dbcc_role to indicate DBCC feature
	 */
	role->macid = info->macid;
	role->wmm = (info->band ? MAC_AX_ACTUAL_WMM_BAND : 0) |
		    (info->wmm ? MAC_AX_ACTUAL_WMM_DRV_WMM : 0);

	if (info->dbcc_role) {
		ret = dbcc_wmm_add_macid(adapter, info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("dbcc_wmm_add_macid %d\n", ret);
			return ret;
		}
	}

	role->info = *info;

	return MACSUCCESS;
}

u32 set_role_bss_clr(struct mac_ax_adapter *adapter,
		     struct mac_ax_role_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 bss_clr = 0;

	switch (info->port) {
	case MAC_AX_PORT_0:
		bss_clr = MAC_REG_R32((info->band == MAC_AX_BAND_0) ?
				       R_AX_PTCL_BSS_COLOR_0 :
				       R_AX_PTCL_BSS_COLOR_0_C1);
		bss_clr = SET_CLR_WORD(bss_clr, info->bss_color,
				       B_AX_BSS_COLOB_AX_PORT_0);
		MAC_REG_W32((info->band == MAC_AX_BAND_0) ?
			     R_AX_PTCL_BSS_COLOR_0 :
			     R_AX_PTCL_BSS_COLOR_0_C1, bss_clr);
		break;
	case MAC_AX_PORT_1:
		bss_clr = MAC_REG_R32((info->band == MAC_AX_BAND_0) ?
				       R_AX_PTCL_BSS_COLOR_0 :
				       R_AX_PTCL_BSS_COLOR_0_C1);
		bss_clr = SET_CLR_WORD(bss_clr, info->bss_color,
				       B_AX_BSS_COLOB_AX_PORT_1);
		MAC_REG_W32((info->band == MAC_AX_BAND_0) ?
			     R_AX_PTCL_BSS_COLOR_0 :
			     R_AX_PTCL_BSS_COLOR_0_C1, bss_clr);
		break;
	case MAC_AX_PORT_2:
		bss_clr = MAC_REG_R32((info->band == MAC_AX_BAND_0) ?
				       R_AX_PTCL_BSS_COLOR_0 :
				       R_AX_PTCL_BSS_COLOR_0_C1);
		bss_clr = SET_CLR_WORD(bss_clr, info->bss_color,
				       B_AX_BSS_COLOB_AX_PORT_2);
		MAC_REG_W32((info->band == MAC_AX_BAND_0) ?
			     R_AX_PTCL_BSS_COLOR_0 :
			     R_AX_PTCL_BSS_COLOR_0_C1, bss_clr);
		break;
	case MAC_AX_PORT_3:
		bss_clr = MAC_REG_R32((info->band == MAC_AX_BAND_0) ?
				       R_AX_PTCL_BSS_COLOR_0 :
				       R_AX_PTCL_BSS_COLOR_0_C1);
		bss_clr = SET_CLR_WORD(bss_clr, info->bss_color,
				       B_AX_BSS_COLOB_AX_PORT_3);
		MAC_REG_W32((info->band == MAC_AX_BAND_0) ?
			     R_AX_PTCL_BSS_COLOR_0 :
			     R_AX_PTCL_BSS_COLOR_0_C1, bss_clr);
		break;
	case MAC_AX_PORT_4:
		bss_clr = MAC_REG_R32((info->band == MAC_AX_BAND_0) ?
				       R_AX_PTCL_BSS_COLOR_1 :
				       R_AX_PTCL_BSS_COLOR_1_C1);
		bss_clr = SET_CLR_WORD(bss_clr, info->bss_color,
				       B_AX_BSS_COLOB_AX_PORT_4);
		MAC_REG_W32((info->band == MAC_AX_BAND_0) ?
			     R_AX_PTCL_BSS_COLOR_1 :
			     R_AX_PTCL_BSS_COLOR_1_C1, bss_clr);
		break;
	default:
		break;
	}
	return 0;
}

static u32 _add_role(struct mac_ax_adapter *adapter,
		     struct mac_ax_role_info *info)
{
	struct mac_role_tbl *role = NULL;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;
	u32 ret = MACSUCCESS;
	u32 cmac_en;

	cmac_en = check_mac_en(adapter, info->band, MAC_AX_CMAC_SEL);
	if (cmac_en != MACSUCCESS)
		PLTFM_MSG_WARN("%s CMAC%d not enable\n", __func__, info->band);

	role = role_get(adapter);
	if (!role) {
		PLTFM_MSG_ERR("role get failed\n");
		return MACBUFALLOC;
	}

	ret = role_info_valid(adapter, info, ADD);
	if (ret != MACSUCCESS)
		goto role_add_fail;

	info->a_info.valid = 1;
	if (role_queue_len(list_head) > adapter->hw_info->macid_num) {
		PLTFM_MSG_ERR("role list full\n");
		ret = MACMACIDFL;
		goto role_add_fail;
	}

	if (sec_info_init(adapter, info)) {
		PLTFM_MSG_ERR("sec info change failed\n");
		return MACROLEINITFL;
	}

	if (role_init(adapter, role, info)) {
		PLTFM_MSG_ERR("role init failed\n");
		ret = MACROLEINITFL;
		goto role_add_fail;
	}

	// Do not call mac_fw_role_maintain if is_mulitcast_entry = 1.
	// mac_fw_role_maintain will trigger FW to create FW Role.
	if (!info->is_mul_ent) {
		ret = mac_fw_role_maintain(adapter, info);
		if (ret != MACSUCCESS) {
			if (ret == MACFWNONRDY) {
				PLTFM_MSG_WARN("skip fw role maintain\n");
			} else {
				PLTFM_MSG_ERR("mac_fw_role_maintain failed:%d\n", ret);
				goto role_add_fail;
			}
		}
	}

	if (info->self_role == MAC_AX_SELF_ROLE_AP) {
		ret = mac_h2c_join_info(adapter, info);
		if (ret != MACSUCCESS) {
			if (ret == MACFWNONRDY) {
				PLTFM_MSG_WARN("skip join info\n");
			} else {
				PLTFM_MSG_ERR("mac_h2c_join_info: %d\n", ret);
				return ret;
			}
		}
	}

	ret = mac_upd_addr_cam(adapter, &role->info, ADD);

	if (ret == MACADDRCAMFL) {
		PLTFM_MSG_ERR("ADDRESS CAM full\n");
		ret = MACADDRCAMFL;
		goto role_add_fail;
	} else if (ret == MACBSSIDCAMFL) {
		PLTFM_MSG_ERR("BSSID CAM full\n");
		ret = MACBSSIDCAMFL;
		goto role_add_fail;
	}

	role_enqueue(adapter, list_head, role);

	if (cmac_en)
		set_role_bss_clr(adapter, info);

	return MACSUCCESS;

role_add_fail:
	role_enqueue(adapter, list_head->role_tbl_pool, role);
	if (role->info.dbcc_role) {
		ret = dbcc_wmm_rm_macid(adapter, &role->info);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("add role fail dbcc wmm rm macid %d\n", ret);
	}
	return ret;
}

static u32 _change_role(struct mac_ax_adapter *adapter,
			struct mac_ax_role_info *info)
{
	struct mac_role_tbl *role;
	u32 ret, cmac_en;

	cmac_en = check_mac_en(adapter, info->band, MAC_AX_CMAC_SEL);
	if (cmac_en != MACSUCCESS)
		PLTFM_MSG_WARN("%s CMAC%d not enable\n", __func__, info->band);

	ret = role_info_valid(adapter, info, CHG);
	if (ret != MACSUCCESS)
		return ret;

	role = _role_srch(adapter, info->macid);
	if (!role) {
		PLTFM_MSG_ERR("role search failed\n");
		return MACNOITEM;
	}

	if (info->upd_mode == MAC_AX_ROLE_BAND_SW) {
		if (!role->info.dbcc_role) {
			PLTFM_MSG_ERR("role band sw runs only for dbcc role\n");
			return MACFUNCINPUT;
		}

		ret = mac_dbcc_move_wmm(adapter, info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("mac_dbcc_move_wmm %d/%d/%d\n",
				      info->band, info->macid, ret);
			return ret;
		}
	}
	info->a_info = role->info.a_info;
	info->b_info = role->info.b_info;
	info->s_info = role->info.s_info;
	info->c_info = role->info.c_info;

	info->dbcc_role = role->info.dbcc_role;

	if (role_info_init(adapter, info)) {
		PLTFM_MSG_ERR("role change failed\n");
		return MACROLEINITFL;
	}

	role->macid = info->macid;
	role->wmm = (info->band ? MAC_AX_ACTUAL_WMM_BAND : 0) |
		    (info->wmm ? MAC_AX_ACTUAL_WMM_DRV_WMM : 0);
	role->info = *info;

	if (info->upd_mode == MAC_AX_ROLE_TYPE_CHANGE ||
	    info->upd_mode == MAC_AX_ROLE_REMOVE ||
	    info->upd_mode == MAC_AX_ROLE_FW_RESTORE) {
		// Do not call mac_fw_role_maintain if is_mulitcast_entry = 1.
		if (!info->is_mul_ent &&
		    info->upd_mode != MAC_AX_ROLE_TYPE_CHANGE) {
			ret = mac_fw_role_maintain(adapter, info);
			if (ret != MACSUCCESS) {
				if (ret == MACFWNONRDY) {
					PLTFM_MSG_WARN("skip fw role maintain\n");
				} else {
					PLTFM_MSG_ERR("mac_fw_role_maintain :%d\n",
						      ret);
					return ret;
				}
			}
		}
		if ((info->upd_mode == MAC_AX_ROLE_TYPE_CHANGE ||
		     info->upd_mode == MAC_AX_ROLE_FW_RESTORE) &&
		    info->self_role == MAC_AX_SELF_ROLE_AP) {
			ret = mac_h2c_join_info(adapter, info);
			if (ret != MACSUCCESS) {
				if (ret == MACFWNONRDY) {
					PLTFM_MSG_WARN("skip join info\n");
				} else {
					PLTFM_MSG_ERR("mac_h2c_join_info: %d\n",
						      ret);
					return ret;
				}
			}
		}
	} else if (info->upd_mode == MAC_AX_ROLE_CON_DISCONN) {
		if (info->opmode == MAC_AX_ROLE_DISCONN) {
			ret = sec_info_deinit(adapter, info, role);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("sec info change failed\n");
				return ret;
			}
		}
		ret = mac_h2c_join_info(adapter, info);
		if (ret != MACSUCCESS) {
			if (ret == MACFWNONRDY) {
				PLTFM_MSG_WARN("skip join info\n");
			} else {
				PLTFM_MSG_ERR("mac_h2c_join_info: %d\n", ret);
				return ret;
			}
		}
	} else if (info->upd_mode == MAC_AX_ROLE_INFO_CHANGE ||
		   info->upd_mode == MAC_AX_ROLE_BAND_SW) {
		if (info->self_role == MAC_AX_SELF_ROLE_CLIENT) {
		} else {
			ret = mac_h2c_join_info(adapter, info);
			if (ret != MACSUCCESS) {
				if (ret == MACFWNONRDY) {
					PLTFM_MSG_WARN("skip join info\n");
				} else {
					PLTFM_MSG_ERR("mac_h2c_join_info: %d\n", ret);
					return ret;
				}
			}
		}
	} else {
		PLTFM_MSG_ERR("role info upd_mode invalid\n");
		return ret;
	}

	ret = mac_upd_addr_cam(adapter, &role->info, CHG);
	if (ret == MACBSSIDCAMFL) {
		PLTFM_MSG_ERR("BSSID CAM full\n");
		return MACBSSIDCAMFL;
	}

	if (cmac_en)
		set_role_bss_clr(adapter, info);

	return ret;
}

static u32 _remove_role(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_role_tbl *role;
	u32 ret;

	role = _role_srch(adapter, macid);
	if (!role) {
		PLTFM_MSG_ERR("no role for macid %d\n", macid);
		return MACNOROLE;
	}

	role->info.a_info.valid = 0;
	role->info.upd_mode = MAC_AX_ROLE_REMOVE;
	role->info.opmode = MAC_AX_ROLE_DISCONN;

	if (!mac_role_srch_by_bssid(adapter, role->info.a_info.bssid_cam_idx))
		role->info.b_info.valid = 0;

	ret = _change_role(adapter, &role->info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: %d\n", __func__, ret);
		return ret;
	}

	if (role->info.dbcc_role) {
		role->info.dbcc_role = 0;
		ret = dbcc_wmm_rm_macid(adapter, &role->info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("dbcc_wmm_rm_macid %d\n", ret);
			return ret;
		}
	}

	role_return(adapter, role);

	return MACSUCCESS;
}

static u32 _remove_role_soft(struct mac_ax_adapter *adapter, u8 macid)
{
	/* This function should be called only before poweroff,
	 * because it will unlink the role without update any info to FW
	 */
	struct mac_role_tbl *role;

	role = _role_srch(adapter, macid);
	if (!role) {
		PLTFM_MSG_ERR("no role for macid %d\n", macid);
		return MACNOROLE;
	}

	role_return(adapter, role);

	return MACSUCCESS;
}

static u32 _remove_role_by_band(struct mac_ax_adapter *adapter, u8 band, u8 sw)
{
	struct mac_role_tbl *role, *curr_role;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;
	u32 ret = MACSUCCESS;
	u32 (*_rm_role)(struct mac_ax_adapter *adapter, u8 macid);

	if (sw)
		_rm_role = _remove_role_soft;
	else
		_rm_role = _remove_role;

	role = list_head->next;
	while (role->next != list_head->next) {
		curr_role = role;
		role = role->next;
		if (band == curr_role->info.band) {
			ret = _rm_role(adapter, curr_role->macid);
			if (ret && ret != MACNOROLE)
				return ret;
		}
	}

	return MACSUCCESS;
}

static struct mac_role_tbl *_role_srch(struct mac_ax_adapter *adapter,
				       u8 macid)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	if (!list_head)
		return NULL;

	role = list_head->next;
	for (; role->next != list_head->next; role = role->next) {
		if (macid == role->macid)
			return role;
	}

	return NULL;
}

static u32 _role_sync(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info)
{
	struct mac_role_tbl *role;
	u32 ret = MACSUCCESS;

	role = _role_srch(adapter, info->macid);
	if (!role) {
		PLTFM_MSG_ERR("role search failed\n");
		return MACNOITEM;
	}

	ret = mac_fw_role_maintain(adapter, info);
	if (ret) {
		if (ret == MACFWNONRDY)
			PLTFM_MSG_WARN("skip fw role sync since fw not ready\n");
		else
			PLTFM_MSG_ERR("mac_fw_role_maintain failed:%d\n", ret);
		return ret;
	}
	if (info->self_role == MAC_AX_SELF_ROLE_AP) {
		ret = mac_h2c_join_info(adapter, info);
		if (ret) {
			if (ret == MACFWNONRDY)
				PLTFM_MSG_WARN("skip join info\n");
			else
				PLTFM_MSG_ERR("mac_h2c_join_info: %d\n", ret);
		}
	}
	return ret;
}

u32 mac_add_role(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info)
{
	u32 ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _add_role(adapter, info);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

u32 mac_change_role(struct mac_ax_adapter *adapter,
		    struct mac_ax_role_info *info)
{
	u32 ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _change_role(adapter, info);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

u32 mac_remove_role(struct mac_ax_adapter *adapter, u8 macid)
{
	u32 ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _remove_role(adapter, macid);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

u32 mac_remove_role_by_band(struct mac_ax_adapter *adapter, u8 band, u8 sw)
{
	u32 ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _remove_role_by_band(adapter, band, sw);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

struct mac_role_tbl *mac_role_srch(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_role_tbl *ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _role_srch(adapter, macid);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

u32 mac_role_sync(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info)
{
	u32 ret;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	ret = _role_sync(adapter, info);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return ret;
}

struct mac_role_tbl *role_srch_no_lock(struct mac_ax_adapter *adapter, u8 macid)
{
	return _role_srch(adapter, macid);
}

struct mac_role_tbl *mac_role_srch_by_addr_cam(struct mac_ax_adapter *adapter,
					       u16 addr_cam_idx)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	if (!list_head)
		return NULL;

	role = list_head->next;
	for (; role->next != list_head->next; role = role->next) {
		if (addr_cam_idx == role->info.a_info.addr_cam_idx &&
		    role->info.a_info.valid)
			return role;
	}

	return NULL;
}

struct mac_role_tbl *mac_role_srch_by_bssid(struct mac_ax_adapter *adapter,
					    u8 bssid_cam_idx)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;

	if (!list_head)
		return NULL;

	role = list_head->next;
	for (; role->next != list_head->next; role = role->next) {
		if (bssid_cam_idx == role->info.b_info.bssid_cam_idx &&
		    role->info.a_info.valid)
			return role;
	}

	return NULL;
}

u32 mac_get_macaddr(struct mac_ax_adapter *adapter,
		    struct mac_ax_macaddr *macaddr,
		    u8 role_idx)
{
	struct mac_role_tbl *role;
	struct mac_role_tbl_head *list_head = adapter->role_tbl;
	struct mac_ax_macaddr *macaddr_list;
	u8 i;
	u8 m_list_idx;
	u32 ret;

	if (role_idx >= adapter->hw_info->macid_num) {
		PLTFM_MSG_ERR("role_idx: %d\n", role_idx);
		return MACFUNCINPUT;
	}
	if (!macaddr) {
		PLTFM_MSG_ERR("mac_ax_macaddr is NULL\n");
		return MACFUNCINPUT;
	}

	macaddr_list = (struct mac_ax_macaddr *)
			PLTFM_MALLOC(sizeof(struct mac_ax_macaddr) *
			adapter->hw_info->macid_num);
	role = list_head->next;
	for (m_list_idx = 0; role->next != list_head->next; role = role->next) {
		if (!role->info.a_info.valid)
			continue;
		for (i = 0; i < m_list_idx; i++) {
			if (!PLTFM_MEMCMP(macaddr_list + i,
					  role->info.a_info.sma,
					  sizeof(struct mac_ax_macaddr))) {
				break;
			}
		}
		if (i < m_list_idx)
			continue;
		PLTFM_MEMCPY(macaddr_list + m_list_idx,
			     role->info.a_info.sma,
			     sizeof(struct mac_ax_macaddr));
		if (m_list_idx == role_idx) {
			PLTFM_MEMCPY(macaddr,
				     role->info.a_info.sma,
				     sizeof(struct mac_ax_macaddr));
			ret = MACSUCCESS;
			goto fin;
		}
		m_list_idx++;
	}
	ret = MACNOITEM;
fin:
	PLTFM_FREE(macaddr_list, sizeof(struct mac_ax_macaddr) *
				 adapter->hw_info->macid_num);
	return ret;
}

u32 mac_set_slot_time(struct mac_ax_adapter *adapter, enum mac_ax_slot_time st)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = MACSUCCESS;

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		switch (st) {
		case MAC_AX_SLOT_TIME_BAND0_9US:
			ret = MAC_REG_W_OFLD(R_AX_SLOTTIME_CFG,
					     B_AX_SLOT_TIME_MSK <<
					     B_AX_SLOT_TIME_SH, 9, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: write offload fail;"
					      "offset: %u, ret: %u\n",
					      __func__, R_AX_SLOTTIME_CFG, ret);
				return ret;
			}
			break;
		case MAC_AX_SLOT_TIME_BAND0_20US:
			ret = MAC_REG_W_OFLD(R_AX_SLOTTIME_CFG,
					     B_AX_SLOT_TIME_MSK <<
					     B_AX_SLOT_TIME_SH, 20, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: write offload fail;"
					      "offset: %u, ret: %u\n",
					      __func__, R_AX_SLOTTIME_CFG, ret);
				return ret;
			}
			break;
		case MAC_AX_SLOT_TIME_BAND1_9US:
			ret = MAC_REG_W_OFLD(R_AX_SLOTTIME_CFG_C1,
					     B_AX_SLOT_TIME_MSK <<
					     B_AX_SLOT_TIME_SH, 9, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: write offload fail;"
					      "offset: %u, ret: %u\n", __func__,
					      R_AX_SLOTTIME_CFG_C1, ret);
				return ret;
			}
			break;
		case MAC_AX_SLOT_TIME_BAND1_20US:
			ret = MAC_REG_W_OFLD(R_AX_SLOTTIME_CFG_C1,
					     B_AX_SLOT_TIME_MSK <<
					     B_AX_SLOT_TIME_SH, 20, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s: write offload fail;"
					      "offset: %u, ret: %u\n", __func__,
					      R_AX_SLOTTIME_CFG_C1, ret);
				return ret;
			}
			break;
		}

		return ret;
	}
#endif
	switch (st) {
	case MAC_AX_SLOT_TIME_BAND0_9US:
		MAC_REG_W8(R_AX_SLOTTIME_CFG, 9);
		break;
	case MAC_AX_SLOT_TIME_BAND0_20US:
		MAC_REG_W8(R_AX_SLOTTIME_CFG, 20);
		break;
	case MAC_AX_SLOT_TIME_BAND1_9US:
		MAC_REG_W8(R_AX_SLOTTIME_CFG_C1, 9);
		break;
	case MAC_AX_SLOT_TIME_BAND1_20US:
		MAC_REG_W8(R_AX_SLOTTIME_CFG_C1, 20);
		break;
	default:
		ret = MACFUNCINPUT;
		break;
	}

	return ret;
}

static u32 mac_h2c_join_info(struct mac_ax_adapter *adapter,
			     struct mac_ax_role_info *info)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_joininfo *fwcmd_tbl;
	struct mac_ax_sta_init_info sta;
	u32 ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	// sta info
	sta.macid = info->macid;
	sta.opmode = info->opmode;
	sta.band = info->band;
	sta.wmm = (info->band ? MAC_AX_ACTUAL_WMM_BAND : 0) |
			(info->wmm ? MAC_AX_ACTUAL_WMM_DRV_WMM : 0);
	sta.trigger = info->trigger;
	sta.is_hesta = info->is_hesta;
	sta.dl_bw = info->dl_bw;
	sta.tf_mac_padding = info->tf_mac_padding;
	sta.dl_t_pe = info->dl_t_pe;
	sta.port_id = info->port;
	sta.net_type = info->net_type;
	sta.wifi_role = info->wifi_role;
	sta.self_role = info->self_role;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_joininfo));
	if (!buf) {
		ret = MACNOBUF;
		goto join_info_fail;
	}

	fwcmd_tbl = (struct fwcmd_joininfo *)buf;
	fwcmd_tbl->dword0 =
	cpu_to_le32(SET_WORD(sta.macid, FWCMD_H2C_JOININFO_MACID) |
		    (sta.opmode ? FWCMD_H2C_JOININFO_OPMODE : 0) |
		    (sta.band ? FWCMD_H2C_JOININFO_BAND : 0) |
		    SET_WORD(sta.wmm, FWCMD_H2C_JOININFO_WMM) |
		    (sta.trigger ? FWCMD_H2C_JOININFO_TRIGGER : 0) |
		    (sta.is_hesta ? FWCMD_H2C_JOININFO_ISHESTA : 0) |
		    SET_WORD(sta.dl_bw, FWCMD_H2C_JOININFO_DL_BW) |
		    SET_WORD(sta.tf_mac_padding,
			     FWCMD_H2C_JOININFO_TF_MAC_PADDING) |
		    SET_WORD(sta.dl_t_pe, FWCMD_H2C_JOININFO_DL_T_PE) |
		    SET_WORD(sta.port_id, FWCMD_H2C_JOININFO_PORT_ID) |
		    SET_WORD(sta.net_type, FWCMD_H2C_JOININFO_NET_TYPE) |
		    SET_WORD(sta.wifi_role, FWCMD_H2C_JOININFO_WIFI_ROLE) |
		    SET_WORD(sta.self_role, FWCMD_H2C_JOININFO_SELF_ROLE));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_JOININFO,
			      0,
			      1);

	if (ret != MACSUCCESS)
		goto join_info_fail;

	// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
	// The H2C shall be sent by mac_h2c_agg_tx.
	ret = h2c_agg_enqueue(adapter, h2cb);
	if (ret == MACSUCCESS)
		return MACSUCCESS;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto join_info_fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto join_info_fail;

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;

join_info_fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

static u32 mac_fw_role_maintain(struct mac_ax_adapter *adapter,
				struct mac_ax_role_info *info)
{
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_fwrole_maintain *fwcmd_tbl;
	struct mac_ax_fwrole_maintain fwrole_main;
	u32 ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	fwrole_main.macid = info->macid;
	fwrole_main.self_role = info->self_role;
	fwrole_main.upd_mode = info->upd_mode;
	fwrole_main.wifi_role = info->wifi_role;
	fwrole_main.band = info->band;
	fwrole_main.port = info->port;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_fwrole_maintain));
	if (!buf) {
		ret = MACNOBUF;
		goto role_maintain_fail;
	}

	fwcmd_tbl = (struct fwcmd_fwrole_maintain *)buf;
	fwcmd_tbl->dword0 =
	cpu_to_le32(SET_WORD(fwrole_main.macid,
			     FWCMD_H2C_FWROLE_MAINTAIN_MACID) |
		    SET_WORD(fwrole_main.self_role,
			     FWCMD_H2C_FWROLE_MAINTAIN_SELF_ROLE) |
		    SET_WORD(fwrole_main.upd_mode,
			     FWCMD_H2C_FWROLE_MAINTAIN_UPD_MODE) |
		    SET_WORD(fwrole_main.wifi_role,
			     FWCMD_H2C_FWROLE_MAINTAIN_WIFI_ROLE) |
		    SET_WORD(fwrole_main.band,
			     FWCMD_H2C_FWROLE_MAINTAIN_BAND) |
		    SET_WORD(fwrole_main.port,
			     FWCMD_H2C_FWROLE_MAINTAIN_PORT));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_FWROLE_MAINTAIN,
			      0,
			      1);

	if (ret != MACSUCCESS)
		goto role_maintain_fail;

	// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
	// The H2C shall be sent by mac_h2c_agg_tx.
	ret = h2c_agg_enqueue(adapter, h2cb);
	if (ret == MACSUCCESS)
		return MACSUCCESS;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto role_maintain_fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS)
		goto role_maintain_fail;

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;

role_maintain_fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

