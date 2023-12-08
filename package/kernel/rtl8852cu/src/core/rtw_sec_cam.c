/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
 *****************************************************************************/
#define _RTW_SEC_CAM_C_

#include <drv_types.h>
#include <rtw_sec_cam.h>

void invalidate_cam_all(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	u8 val8 = 0;

	rtw_hal_set_hwreg(padapter, HW_VAR_CAM_INVALID_ALL, &val8);

	_rtw_spinlock_bh(&cam_ctl->lock);
	rtw_sec_cam_map_clr_all(&cam_ctl->used);
	_rtw_memset(dvobj->cam_cache, 0, sizeof(struct sec_cam_ent) * SEC_CAM_ENT_NUM_SW_LIMIT);
	_rtw_spinunlock_bh(&cam_ctl->lock);
}

void _clear_cam_entry(_adapter *padapter, u8 entry)
{
	unsigned char null_sta[6] = {0};
	unsigned char null_key[32] = {0};

	rtw_hal_sec_write_cam_ent(padapter, entry, 0, null_sta, null_key);
}

inline void _write_cam(_adapter *adapter, u8 id, u16 ctrl, u8 *mac, u8 *key)
{
#ifdef CONFIG_WRITE_CACHE_ONLY
	write_cam_cache(adapter, id , ctrl, mac, key);
#else
	rtw_hal_sec_write_cam_ent(adapter, id, ctrl, mac, key);
	write_cam_cache(adapter, id , ctrl, mac, key);
#endif
}

inline void write_cam(_adapter *adapter, u8 id, u16 ctrl, u8 *mac, u8 *key)
{
	if (ctrl & BIT(9)) {
		_write_cam(adapter, id, ctrl, mac, key);
		_write_cam(adapter, (id + 1), ctrl | BIT(5), mac, (key + 16));
		RTW_INFO_DUMP("key-0: ", key, 16);
		RTW_INFO_DUMP("key-1: ", (key + 16), 16);
	} else
		_write_cam(adapter, id, ctrl, mac, key);
}

inline void clear_cam_entry(_adapter *adapter, u8 id)
{
	_clear_cam_entry(adapter, id);
	clear_cam_cache(adapter, id);
}

inline void write_cam_from_cache(_adapter *adapter, u8 id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	struct sec_cam_ent cache;

	_rtw_spinlock_bh(&cam_ctl->lock);
	_rtw_memcpy(&cache, &dvobj->cam_cache[id], sizeof(struct sec_cam_ent));
	_rtw_spinunlock_bh(&cam_ctl->lock);

	rtw_hal_sec_write_cam_ent(adapter, id, cache.ctrl, cache.mac, cache.key);
}
void write_cam_cache(_adapter *adapter, u8 id, u16 ctrl, u8 *mac, u8 *key)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);

	dvobj->cam_cache[id].ctrl = ctrl;
	_rtw_memcpy(dvobj->cam_cache[id].mac, mac, ETH_ALEN);
	_rtw_memcpy(dvobj->cam_cache[id].key, key, 16);

	_rtw_spinunlock_bh(&cam_ctl->lock);
}

void clear_cam_cache(_adapter *adapter, u8 id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);

	_rtw_memset(&(dvobj->cam_cache[id]), 0, sizeof(struct sec_cam_ent));

	_rtw_spinunlock_bh(&cam_ctl->lock);
}

inline bool _rtw_camctl_chk_cap(_adapter *adapter, u8 cap)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	if (cam_ctl->sec_cap & cap)
		return _TRUE;
	return _FALSE;
}

inline void _rtw_camctl_set_flags(_adapter *adapter, u32 flags)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	cam_ctl->flags |= flags;
}

inline void rtw_camctl_set_flags(_adapter *adapter, u32 flags)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);
	_rtw_camctl_set_flags(adapter, flags);
	_rtw_spinunlock_bh(&cam_ctl->lock);
}

inline void _rtw_camctl_clr_flags(_adapter *adapter, u32 flags)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	cam_ctl->flags &= ~flags;
}

inline void rtw_camctl_clr_flags(_adapter *adapter, u32 flags)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);
	_rtw_camctl_clr_flags(adapter, flags);
	_rtw_spinunlock_bh(&cam_ctl->lock);
}

inline bool _rtw_camctl_chk_flags(_adapter *adapter, u32 flags)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	if (cam_ctl->flags & flags)
		return _TRUE;
	return _FALSE;
}

void dump_sec_cam_map(void *sel, struct sec_cam_bmp *map, u8 max_num)
{
	RTW_PRINT_SEL(sel, "0x%08x\n", map->m0);
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	if (max_num && max_num > 32)
		RTW_PRINT_SEL(sel, "0x%08x\n", map->m1);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	if (max_num && max_num > 64)
		RTW_PRINT_SEL(sel, "0x%08x\n", map->m2);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	if (max_num && max_num > 96)
		RTW_PRINT_SEL(sel, "0x%08x\n", map->m3);
#endif
}

inline bool rtw_sec_camid_is_set(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		return map->m0 & BIT(id);
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	else if (id < 64)
		return map->m1 & BIT(id - 32);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	else if (id < 96)
		return map->m2 & BIT(id - 64);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	else if (id < 128)
		return map->m3 & BIT(id - 96);
#endif
	else
		rtw_warn_on(1);

	return 0;
}

inline void rtw_sec_cam_map_set(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 |= BIT(id);
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	else if (id < 64)
		map->m1 |= BIT(id - 32);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	else if (id < 96)
		map->m2 |= BIT(id - 64);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	else if (id < 128)
		map->m3 |= BIT(id - 96);
#endif
	else
		rtw_warn_on(1);
}

inline void rtw_sec_cam_map_clr(struct sec_cam_bmp *map, u8 id)
{
	if (id < 32)
		map->m0 &= ~BIT(id);
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	else if (id < 64)
		map->m1 &= ~BIT(id - 32);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	else if (id < 96)
		map->m2 &= ~BIT(id - 64);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	else if (id < 128)
		map->m3 &= ~BIT(id - 96);
#endif
	else
		rtw_warn_on(1);
}

inline void rtw_sec_cam_map_clr_all(struct sec_cam_bmp *map)
{
	map->m0 = 0;
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	map->m1 = 0;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	map->m2 = 0;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	map->m3 = 0;
#endif
}

inline bool rtw_sec_camid_is_drv_forbid(struct cam_ctl_t *cam_ctl, u8 id)
{
	struct sec_cam_bmp forbid_map;

	forbid_map.m0 = 0x00000ff0;
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	forbid_map.m1 = 0x00000000;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	forbid_map.m2 = 0x00000000;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	forbid_map.m3 = 0x00000000;
#endif

	if (id < 32)
		return forbid_map.m0 & BIT(id);
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	else if (id < 64)
		return forbid_map.m1 & BIT(id - 32);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	else if (id < 96)
		return forbid_map.m2 & BIT(id - 64);
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	else if (id < 128)
		return forbid_map.m3 & BIT(id - 96);
#endif
	else
		rtw_warn_on(1);

	return 1;
}

bool _rtw_sec_camid_is_used(struct cam_ctl_t *cam_ctl, u8 id)
{
	bool ret = _FALSE;

	if (id >= cam_ctl->num) {
		rtw_warn_on(1);
		goto exit;
	}

#if 0 /* for testing */
	if (rtw_sec_camid_is_drv_forbid(cam_ctl, id)) {
		ret = _TRUE;
		goto exit;
	}
#endif

	ret = rtw_sec_camid_is_set(&cam_ctl->used, id);

exit:
	return ret;
}

inline bool rtw_sec_camid_is_used(struct cam_ctl_t *cam_ctl, u8 id)
{
	bool ret;

	_rtw_spinlock_bh(&cam_ctl->lock);
	ret = _rtw_sec_camid_is_used(cam_ctl, id);
	_rtw_spinunlock_bh(&cam_ctl->lock);

	return ret;
}
u8 rtw_get_sec_camid(_adapter *adapter, u8 max_bk_key_num, u8 *sec_key_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;

	u8 sec_cam_num = 0;

	_rtw_spinlock_bh(&cam_ctl->lock);
	for (i = 0; i < cam_ctl->num; i++) {
		if (_rtw_sec_camid_is_used(cam_ctl, i)) {
			sec_key_id[sec_cam_num++] = i;
			if (sec_cam_num == max_bk_key_num)
				break;
		}
	}
	_rtw_spinunlock_bh(&cam_ctl->lock);

	return sec_cam_num;
}

inline bool _rtw_camid_is_gk(_adapter *adapter, u8 cam_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	bool ret = _FALSE;

	if (cam_id >= cam_ctl->num) {
		rtw_warn_on(1);
		goto exit;
	}

	if (_rtw_sec_camid_is_used(cam_ctl, cam_id) == _FALSE)
		goto exit;

	ret = (dvobj->cam_cache[cam_id].ctrl & BIT6) ? _TRUE : _FALSE;

exit:
	return ret;
}

inline bool rtw_camid_is_gk(_adapter *adapter, u8 cam_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	bool ret;

	_rtw_spinlock_bh(&cam_ctl->lock);
	ret = _rtw_camid_is_gk(adapter, cam_id);
	_rtw_spinunlock_bh(&cam_ctl->lock);

	return ret;
}

bool cam_cache_chk(_adapter *adapter, u8 id, u8 *addr, s16 kid, s8 gk)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	bool ret = _FALSE;

	if (addr && _rtw_memcmp(dvobj->cam_cache[id].mac, addr, ETH_ALEN) == _FALSE)
		goto exit;
	if (kid >= 0 && kid != (dvobj->cam_cache[id].ctrl & 0x03))
		goto exit;
	if (gk != -1 && (gk ? _TRUE : _FALSE) != _rtw_camid_is_gk(adapter, id))
		goto exit;

	ret = _TRUE;

exit:
	return ret;
}

s16 _rtw_camid_search(_adapter *adapter, u8 *addr, s16 kid, s8 gk)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;
	s16 cam_id = -1;

	for (i = 0; i < cam_ctl->num; i++) {
		if (cam_cache_chk(adapter, i, addr, kid, gk)) {
			cam_id = i;
			break;
		}
	}

	if (0) {
		if (addr)
			RTW_INFO(FUNC_ADPT_FMT" addr:"MAC_FMT" kid:%d, gk:%d, return cam_id:%d\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(addr), kid, gk, cam_id);
		else
			RTW_INFO(FUNC_ADPT_FMT" addr:%p kid:%d, gk:%d, return cam_id:%d\n"
				, FUNC_ADPT_ARG(adapter), addr, kid, gk, cam_id);
	}

	return cam_id;
}

s16 rtw_camid_search(_adapter *adapter, u8 *addr, s16 kid, s8 gk)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	s16 cam_id = -1;

	_rtw_spinlock_bh(&cam_ctl->lock);
	cam_id = _rtw_camid_search(adapter, addr, kid, gk);
	_rtw_spinunlock_bh(&cam_ctl->lock);

	return cam_id;
}

s16 rtw_get_camid(_adapter *adapter, u8 *addr, s16 kid, u8 gk, bool ext_sec)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;
#if 0 /* for testing */
	static u8 start_id = 0;
#else
	u8 start_id = 0;
#endif
	s16 cam_id = -1;

	if (addr == NULL) {
		RTW_PRINT(FUNC_ADPT_FMT" mac_address is NULL\n"
			  , FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		goto _exit;
	}

	/* find cam entry which has the same addr, kid (, gk bit) */
	if (_rtw_camctl_chk_cap(adapter, SEC_CAP_CHK_BMC) == _TRUE)
		i = _rtw_camid_search(adapter, addr, kid, gk);
	else
		i = _rtw_camid_search(adapter, addr, kid, -1);

	if (i >= 0) {
		cam_id = i;
		goto _exit;
	}

	for (i = 0; i < cam_ctl->num; i++) {
		/* bypass default key which is allocated statically */
#if 0//ndef CONFIG_CONCURRENT_MODE
		if (((i + start_id) % cam_ctl->num) < 4)
			continue;
#endif
		if (_rtw_sec_camid_is_used(cam_ctl, ((i + start_id) % cam_ctl->num)) == _FALSE) {
			if (ext_sec) {
				/* look out continue slot */
				if (((i + 1) < cam_ctl->num) &&
					(_rtw_sec_camid_is_used(cam_ctl, (((i + 1) + start_id) % cam_ctl->num)) == _FALSE))
			break;
				else
					continue;
			} else
				break;
		}
	}

	if (i == cam_ctl->num) {
		RTW_PRINT(FUNC_ADPT_FMT" %s key with "MAC_FMT" id:%u no room\n"
			, FUNC_ADPT_ARG(adapter), gk ? "group" : "pairwise", MAC_ARG(addr), kid);
		rtw_warn_on(1);
		goto _exit;
	}

	cam_id = ((i + start_id) % cam_ctl->num);
	start_id = ((i + start_id + 1) % cam_ctl->num);

_exit:
	return cam_id;
}

s16 rtw_camid_alloc(_adapter *adapter, struct sta_info *sta, u8 kid, u8 gk, bool ext_sec, bool *used)
{
	struct mlme_ext_info *mlmeinfo = &adapter->mlmeextpriv.mlmext_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	s16 cam_id = -1;

	*used = _FALSE;

	_rtw_spinlock_bh(&cam_ctl->lock);

	if ((((mlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) || ((mlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE))
	    && !sta) {
		/*
		* 1. non-STA mode WEP key
		* 2. group TX key
		*/
#if 0//ndef CONFIG_CONCURRENT_MODE
		/* static alloction to default key by key ID when concurrent is not defined */
		if (kid > 3) {
			RTW_PRINT(FUNC_ADPT_FMT" group key with invalid key id:%u\n"
				  , FUNC_ADPT_ARG(adapter), kid);
			rtw_warn_on(1);
			goto bitmap_handle;
		}
		cam_id = kid;
#else
		u8 *addr = adapter_mac_addr(adapter);

		cam_id = rtw_get_camid(adapter, addr, kid, gk, ext_sec);
		if (1)
			RTW_PRINT(FUNC_ADPT_FMT" group key with "MAC_FMT" assigned cam_id:%u\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(addr), cam_id);
#endif
	} else {
		/*
		* 1. STA mode WEP key
		* 2. STA mode group RX key
		* 3. sta key (pairwise, group RX)
		*/
		u8 *addr = sta ? sta->phl_sta->mac_addr : NULL;

		if (!sta) {
			if (!(mlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)) {
				/* bypass STA mode group key setting before connected(ex:WEP) because bssid is not ready */
				goto bitmap_handle;
			}
			addr = get_bssid(&adapter->mlmepriv);/*A2*/
		}
		cam_id = rtw_get_camid(adapter, addr, kid, gk, ext_sec);
	}


bitmap_handle:
	if (cam_id >= 0) {
		*used = _rtw_sec_camid_is_used(cam_ctl, cam_id);
		rtw_sec_cam_map_set(&cam_ctl->used, cam_id);
		if (ext_sec)
			rtw_sec_cam_map_set(&cam_ctl->used, cam_id + 1);
	}

	_rtw_spinunlock_bh(&cam_ctl->lock);

	return cam_id;
}

void rtw_camid_set(_adapter *adapter, u8 cam_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);

	if (cam_id < cam_ctl->num)
		rtw_sec_cam_map_set(&cam_ctl->used, cam_id);

	_rtw_spinunlock_bh(&cam_ctl->lock);
}

void rtw_camid_free(_adapter *adapter, u8 cam_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	_rtw_spinlock_bh(&cam_ctl->lock);

	if (cam_id < cam_ctl->num)
		rtw_sec_cam_map_clr(&cam_ctl->used, cam_id);

	_rtw_spinunlock_bh(&cam_ctl->lock);
}

/*Must pause TX/RX before use this API*/
inline void rtw_sec_cam_swap(_adapter *adapter, u8 cam_id_a, u8 cam_id_b)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	struct sec_cam_ent cache_a, cache_b;
	bool cam_a_used, cam_b_used;

	if (1)
		RTW_INFO(ADPT_FMT" - sec_cam %d,%d swap\n", ADPT_ARG(adapter), cam_id_a, cam_id_b);

	if (cam_id_a == cam_id_b)
		return;

	/*setp-1. backup org cam_info*/
	_rtw_spinlock_bh(&cam_ctl->lock);

	cam_a_used = _rtw_sec_camid_is_used(cam_ctl, cam_id_a);
	cam_b_used = _rtw_sec_camid_is_used(cam_ctl, cam_id_b);

	if (cam_a_used)
		_rtw_memcpy(&cache_a, &dvobj->cam_cache[cam_id_a], sizeof(struct sec_cam_ent));

	if (cam_b_used)
		_rtw_memcpy(&cache_b, &dvobj->cam_cache[cam_id_b], sizeof(struct sec_cam_ent));

	_rtw_spinunlock_bh(&cam_ctl->lock);

	/*setp-2. clean cam_info*/
	if (cam_a_used) {
		rtw_camid_free(adapter, cam_id_a);
		clear_cam_entry(adapter, cam_id_a);
	}
	if (cam_b_used) {
		rtw_camid_free(adapter, cam_id_b);
		clear_cam_entry(adapter, cam_id_b);
	}

	/*setp-3. set cam_info*/
	if (cam_a_used) {
		write_cam(adapter, cam_id_b, cache_a.ctrl, cache_a.mac, cache_a.key);
		rtw_camid_set(adapter, cam_id_b);
	}

	if (cam_b_used) {
		write_cam(adapter, cam_id_a, cache_b.ctrl, cache_b.mac, cache_b.key);
		rtw_camid_set(adapter, cam_id_a);
	}
}

s16 rtw_get_empty_cam_entry(_adapter *adapter, u8 start_camid)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	int i;
	s16 cam_id = -1;

	_rtw_spinlock_bh(&cam_ctl->lock);
	for (i = start_camid; i < cam_ctl->num; i++) {
		if (_FALSE == _rtw_sec_camid_is_used(cam_ctl, i)) {
			cam_id = i;
			break;
		}
	}
	_rtw_spinunlock_bh(&cam_ctl->lock);

	return cam_id;
}
void rtw_clean_dk_section(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = dvobj_to_sec_camctl(dvobj);
	s16 ept_cam_id;
	int i;

	for (i = 0; i < 4; i++) {
		if (rtw_sec_camid_is_used(cam_ctl, i)) {
			ept_cam_id = rtw_get_empty_cam_entry(adapter, 4);
			if (ept_cam_id > 0)
				rtw_sec_cam_swap(adapter, i, ept_cam_id);
		}
	}
}
void rtw_clean_hw_dk_cam(_adapter *adapter)
{
	int i;

	for (i = 0; i < 4; i++)
		rtw_hal_sec_clr_cam_ent(adapter, i);
		/*_clear_cam_entry(adapter, i);*/
}

void flush_all_cam_entry(struct _ADAPTER *a, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct sta_priv	*stapriv = &a->stapriv;
	u8 *mac;
	struct sta_info	*sta;
	struct _ADAPTER_LINK *alink;
	u8 lidx;


	if (MLME_IS_STA(a)) {
		for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
			alink = GET_LINK(a, lidx);
			mac = alink->mlmeextpriv.mlmext_info.network.MacAddress;
			sta = rtw_get_stainfo(stapriv, mac);
			if (sta) {
				if (sta->state & WIFI_AP_STATE)
					/*clear cam when ap free per sta_info*/
					RTW_INFO("%s: sta->state(0x%x) is AP, "
					 	"do nothing\n",
						 __func__, sta->state);
				else
					rtw_hw_del_all_key(a, sta, cmd_type, cmd_timeout);
			} else {
				RTW_WARN("%s: cann't find sta for %pM\n", __func__, mac);
				rtw_warn_on(1);
			}
		}
	} else if (MLME_IS_AP(a) || MLME_IS_MESH(a)) {
		for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
			alink = GET_LINK(a, lidx);
			mac = alink->mac_addr;
			sta = rtw_get_stainfo(stapriv, mac);
			if (sta)
				rtw_hw_del_all_key(a, sta, cmd_type, cmd_timeout);
		}
	}
}

#if defined(DBG_CONFIG_ERROR_RESET) && defined(CONFIG_CONCURRENT_MODE)
void rtw_iface_bcmc_sec_cam_map_restore(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = dvobj_to_sec_camctl(dvobj);
	int cam_id = -1;

	cam_id = rtw_phl_wrole_bcmc_id_get(GET_PHL_INFO(dvobj), adapter->phl_role);
	if (cam_id != INVALID_SEC_MAC_CAM_ID)
		rtw_sec_cam_map_set(&cam_ctl->used, cam_id);
}
#endif

#ifdef  CONFIG_DBG_AX_CAM
/*
1. Mode 0 : no key
2. Mode 1 : unicast	0 - 6
3. Mode 2 : unicast	0 - 1	Group: 2 - 4	BIP: 5 - 6
4. Mode 3 : unicast	0 - 1	Group: 2 - 5	BIP: 6
*/
static enum CAM_KEY_TYPE search_keytype_by_mode(u8 mode, u8 entry_num)
{
	enum CAM_KEY_TYPE key_type = KEY_TYPE_NONE;

	switch (mode) {
	case 0:
		key_type = KEY_TYPE_NONE;
		break;
	case 1:
		key_type = KEY_TYPE_UNI;
		break;
	case 2:
		if (entry_num < 2)
			key_type = KEY_TYPE_UNI;
		else if (entry_num < 5)
			key_type = KEY_TYPE_GROUP;
		else if (entry_num < 7)
			key_type = KEY_TYPE_BIP;
		else
			key_type = KEY_TYPE_NONE;
		break;
	case 3:
		if (entry_num < 2)
			key_type = KEY_TYPE_UNI;
		else if (entry_num < 6)
			key_type = KEY_TYPE_GROUP;
		else if (entry_num < 7)
			key_type = KEY_TYPE_BIP;
		else
			key_type = KEY_TYPE_NONE;
		break;
	default:
		key_type = KEY_TYPE_NONE;
		break;
	}
	return key_type;
}

static const char* enc_algo_to_string[] = {
	"RTW_ENC_NONE",
	"RTW_ENC_WEP40",
	"RTW_ENC_WEP104",
	"RTW_ENC_TKIP",
	"RTW_ENC_WAPI",
	"RTW_ENC_GCMSMS4",
	"RTW_ENC_CCMP",
	"RTW_ENC_CCMP256",
	"RTW_ENC_GCMP",
	"RTW_ENC_GCMP256",
	"RTW_ENC_BIP_CCMP128",
	"RTW_ENC_MAX"
};

static const char* type_to_string[] = {
	"unicast",
	"multicast",
	"BIP",
	"NONE"
};

static const char* nettype_to_string[] = {
	"NoLink",
	"Ad-hoc",
	"Infra",
	"AP"
};

static u8 get_keyid(u8 num, u8* addr_cam)
{
	u8 keyid = 0;

	switch (num) {
	case 0:
		keyid = GET_AX_ADDR_CAM_SEC_ENT0_KEYID(addr_cam);
		break;
	case 1:
		keyid = GET_AX_ADDR_CAM_SEC_ENT1_KEYID(addr_cam);
		break;
	case 2:
		keyid = GET_AX_ADDR_CAM_SEC_ENT2_KEYID(addr_cam);
		break;
	case 3:
		keyid = GET_AX_ADDR_CAM_SEC_ENT3_KEYID(addr_cam);
		break;
	case 4:
		keyid = GET_AX_ADDR_CAM_SEC_ENT4_KEYID(addr_cam);
		break;
	case 5:
		keyid = GET_AX_ADDR_CAM_SEC_ENT5_KEYID(addr_cam);
		break;
	case 6:
		keyid = GET_AX_ADDR_CAM_SEC_ENT6_KEYID(addr_cam);
		break;
	default:
		keyid = 0;
		break;
	}
	return keyid;
}

static u8 get_sec_entry(u8 num, u8* addr_cam)
{
	u8 entry = 0;

	switch (num) {
	case 0:
		entry = GET_AX_ADDR_CAM_SEC_ENT0(addr_cam);
		break;
	case 1:
		entry = GET_AX_ADDR_CAM_SEC_ENT1(addr_cam);
		break;
	case 2:
		entry = GET_AX_ADDR_CAM_SEC_ENT2(addr_cam);
		break;
	case 3:
		entry = GET_AX_ADDR_CAM_SEC_ENT3(addr_cam);
		break;
	case 4:
		entry = GET_AX_ADDR_CAM_SEC_ENT4(addr_cam);
		break;
	case 5:
		entry = GET_AX_ADDR_CAM_SEC_ENT5(addr_cam);
		break;
	case 6:
		entry = GET_AX_ADDR_CAM_SEC_ENT6(addr_cam);
		break;
	default:
		entry = 0;
		break;
	}
	return entry;
}

static void dump_cam_info(void* sel, u8 *buf, u32 start, u32 end)
{
	int i, j = 1;

	for (i = start; i < end; i += 1) {
		if (j % 4 == 1)
			RTW_PRINT_SEL(sel,"0x%04x", i);
		RTW_PRINT_SEL(sel," 0x%08x ", *(u32 *)(buf+ (i*4)));
		if ((j++) % 4 == 0)
			RTW_PRINT_SEL(sel,"\n");
	}
}

static void search_sec_cam_by_entry(struct _ADAPTER *a, u8 num, u8* target_entry)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	void *phl;
	u32 sec_tmp_buf = 0;
	int i;

	phl = GET_PHL_INFO(dvobj);
	rtw_phl_write32(phl, INDIRECT_ACCESS_ADDR, (SEC_CAM_BASE_ADDR + num*32));

	for (i = 0; i < 5; i++) {
		sec_tmp_buf = rtw_phl_read32(phl,INDIRECT_ACCESS_VALUE + i*4);
		_rtw_memcpy((target_entry + i*4), &sec_tmp_buf, 4);
	}
}

static void search_bssid_cam_by_entry(struct _ADAPTER *a, u8 num, u8* target_entry)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	void *phl;
	u32 BSSID_tmp_buf = 0;
	int i;

	phl = GET_PHL_INFO(dvobj);
	rtw_phl_write32(phl, INDIRECT_ACCESS_ADDR, (BSSID_CAM_BASE_ADDR+ num*8));

	for (i = 0 ; i < 2; i++) {
		BSSID_tmp_buf = rtw_phl_read32(phl,INDIRECT_ACCESS_VALUE + i*4);
		_rtw_memcpy((target_entry + i*4), &BSSID_tmp_buf, 4);
	}
}

static u8 varify_256_len(u8 enc)
{
	u8 hit = _FALSE;

	if (enc == 4|| enc == 7|| enc == 9)
		hit = _TRUE;
	return  hit;
}

int get_ax_address_cam(void* sel, struct _ADAPTER *a)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	void *phl;
	/*addr cam: 128 entries*/
	int loop_num = 128;
	/*addr cam: 11 DW but the last one all rsvd*/
	u32 addr_tmp_buf = 0;
	u8  addr_map[40] = {0};
	int i, j;

	phl = GET_PHL_INFO(dvobj);

	for (i = 0; i < loop_num; i++) {
		_rtw_memset(addr_map,0,sizeof(addr_map));
		rtw_phl_write32(phl, INDIRECT_ACCESS_ADDR, (ADDR_CAM_BASE_ADDR + i*64));

		for (j = 0; j < 10; j++) {
			addr_tmp_buf = rtw_phl_read32(phl,INDIRECT_ACCESS_VALUE + j*4);
			_rtw_memcpy(&addr_map[j*4], &addr_tmp_buf, 4);
		}
		RTW_PRINT_SEL(sel, "======= ADDR CAM (%d)DUMP  =======\n", i);
		dump_cam_info(sel,addr_map,0,10);
		RTW_PRINT_SEL(sel, "\n");
	}
	return 0;
}

int get_ax_sec_cam(void* sel, struct _ADAPTER *a)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	void *phl;
	/*security cam: 128 entries*/
	int loop_num = 128;
	/*security cam: 5 DW*/
	u32 sec_tmp_buf = 0;
	u8  sec_map[20] = {0};
	int i, j;

	phl = GET_PHL_INFO(dvobj);

	for (i = 0; i < loop_num; i++) {
		_rtw_memset(sec_map,0,sizeof(sec_map));
		rtw_phl_write32(phl, INDIRECT_ACCESS_ADDR, (SEC_CAM_BASE_ADDR + i*32));

		for (j = 0; j < 5; j++) {
			sec_tmp_buf = rtw_phl_read32(phl,INDIRECT_ACCESS_VALUE + j*4);
			_rtw_memcpy(&sec_map[j*4], &sec_tmp_buf, 4);
		}

		RTW_PRINT_SEL(sel, "======= sec CAM (%d)DUMP  =======\n", i);
		dump_cam_info(sel, sec_map, 0, 5);
		RTW_PRINT_SEL(sel, "\n");
	}
	return 0;
}

static void dump_valid_key(void* sel, u8* addr_map, u8* sec_map, u8* bssid_map\
		, u8 sec_entry, u8 key_type, u8 keyid)
{
	u8 SMA[ETH_ALEN] = {0}, TMA[ETH_ALEN] = {0};
	u8 macid = 0, nettype = 0;

	macid = GET_AX_ADDR_CAM_MACID(addr_map);
	nettype = GET_AX_ADDR_CAM_NET_TYPE(addr_map);

	_rtw_memcpy(SMA, &addr_map[8], ETH_ALEN);
	_rtw_memcpy(TMA,&addr_map[14], ETH_ALEN);
	RTW_PRINT_SEL(sel, "%-5u %s "MAC_FMT"  " MAC_FMT" " MAC_FMT\
		" %-5u %-5u %s %-7u %-3u %s "KEY_FMT"\n", macid\
		, nettype_to_string[nettype], MAC_ARG(SMA), MAC_ARG(TMA)\
		, MAC_ARG(&bssid_map[2]), keyid, sec_entry\
		, enc_algo_to_string[GET_AX_SEC_CAM_TYPE(sec_map)] \
		, GET_AX_SEC_CAM_EXT_KEY(sec_map), GET_AX_SEC_SPP_MODE_(sec_map)\
		, type_to_string[key_type], KEY_ARG(&sec_map[4]));
}

static void prepare_to_dump_valid_key(void* sel, struct _ADAPTER *a, u8* addr_map, u8 ent)
{
	u8 sec_map[20] = {0};
	u8 keyid = 0, sec_entry = 0, key_type = KEY_TYPE_NONE;
	u8 bssid_entry;
	u8 bssid_map[8] = {0};
	u8 enc_algor = 0;

	key_type = search_keytype_by_mode(GET_AX_ADDR_CAM_SEC_ENT_MODE(addr_map) ,ent);
	sec_entry = get_sec_entry(ent, addr_map);
	bssid_entry = GET_AX_ADDR_CAM_BSSID_CAM_IDX(addr_map);
	keyid = get_keyid(ent,addr_map);
	search_sec_cam_by_entry(a, sec_entry, sec_map);
	search_bssid_cam_by_entry(a, bssid_entry, bssid_map);
	enc_algor = GET_AX_SEC_CAM_TYPE(sec_map);

	if (varify_256_len(enc_algor) == _TRUE) {
		dump_valid_key(sel, addr_map, sec_map, bssid_map, sec_entry, key_type, keyid);
		search_sec_cam_by_entry(a, sec_entry + 1, sec_map);
		dump_valid_key(sel, addr_map, sec_map, bssid_map, sec_entry + 1, key_type, keyid);
	} else {
		dump_valid_key(sel, addr_map, sec_map, bssid_map, sec_entry, key_type, keyid);
	}
}

int get_ax_valid_key(void* sel, struct _ADAPTER *a)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	void *phl;
	/*addr cam: 128 entries*/
	int loop_num = 128;
	/*addr cam: 11 DW but the last one all rsvd*/
	u32 addr_tmp_buf = 0;
	u8 addr_map[40]= {0};
	u8 sec_ent_valid = 0;
	u8 macid = 0;
	int i, j;

	phl = GET_PHL_INFO(dvobj);

	RTW_PRINT_SEL(sel, "Dump valid KEY\n");
	RTW_PRINT_SEL(sel, "%-5s %-7s %-17s %-17s %-17s %-5s %-5s %-8s %-3s"\
		"  %-3s  %s  %s\n", "macid", "NETTYPE", "SMA", "TMA", "BSSID", "keyid"\
		, "sec_entry", "enc_algor", "ext_key", "ssp", "key_type", "key");

	for (i = 0; i < loop_num; i++) {
		_rtw_memset(addr_map,0,sizeof(addr_map));
		rtw_phl_write32(phl, INDIRECT_ACCESS_ADDR, (ADDR_CAM_BASE_ADDR + i*64));

		for (j = 0; j < 10; j++) {
			addr_tmp_buf = rtw_phl_read32(phl,INDIRECT_ACCESS_VALUE + j*4);
			_rtw_memcpy(&addr_map[j*4], &addr_tmp_buf, 4);
		}
		sec_ent_valid = GET_AX_ADDR_CAM_SEC_ENT_VALID(addr_map);

		for (j = 0 ;j< 7; j++) {
			if (sec_ent_valid & BIT(j))
				prepare_to_dump_valid_key(sel, a, addr_map, j);
		}
	}
	return 0;
}
#endif /* CONFIG_DBG_AX_CAM */

