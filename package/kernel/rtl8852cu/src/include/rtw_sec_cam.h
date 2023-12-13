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
#ifndef __RTW_SEC_CAM_H__
#define __RTW_SEC_CAM_H__

#define SEC_STATUS_STA_PK_GK_CONFLICT_DIS_BMC_SEARCH	BIT0

struct sec_cam_bmp {
	u32 m0;
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 32)
	u32 m1;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 64)
	u32 m2;
#endif
#if (SEC_CAM_ENT_NUM_SW_LIMIT > 96)
	u32 m3;
#endif
};

struct cam_ctl_t {
	_lock lock;

	u8 sec_cap;
	u32 flags;

	u8 num;
	struct sec_cam_bmp used;

	_mutex sec_cam_access_mutex;
};

struct sec_cam_ent {
	u16 ctrl;
	u8 mac[ETH_ALEN];
	u8 key[16];
};

bool _rtw_camctl_chk_cap(_adapter *adapter, u8 cap);
void _rtw_camctl_set_flags(_adapter *adapter, u32 flags);
void rtw_camctl_set_flags(_adapter *adapter, u32 flags);
void _rtw_camctl_clr_flags(_adapter *adapter, u32 flags);
void rtw_camctl_clr_flags(_adapter *adapter, u32 flags);
bool _rtw_camctl_chk_flags(_adapter *adapter, u32 flags);

void dump_sec_cam_map(void *sel, struct sec_cam_bmp *map, u8 max_num);
void rtw_sec_cam_map_set(struct sec_cam_bmp *map, u8 id);
void rtw_sec_cam_map_clr(struct sec_cam_bmp *map, u8 id);
void rtw_sec_cam_map_clr_all(struct sec_cam_bmp *map);

bool rtw_sec_camid_is_used(struct cam_ctl_t *cam_ctl, u8 id);

bool _rtw_camid_is_gk(_adapter *adapter, u8 cam_id);
bool rtw_camid_is_gk(_adapter *adapter, u8 cam_id);
s16 rtw_camid_search(_adapter *adapter, u8 *addr, s16 kid, s8 gk);
s16 rtw_camid_alloc(_adapter *adapter, struct sta_info *sta, u8 kid, u8 gk, bool ext_sec, bool *used);
void rtw_camid_free(_adapter *adapter, u8 cam_id);
u8 rtw_get_sec_camid(_adapter *adapter, u8 max_bk_key_num, u8 *sec_key_id);

void _clear_cam_entry(_adapter *padapter, u8 entry);
void write_cam_from_cache(_adapter *adapter, u8 id);
void rtw_sec_cam_swap(_adapter *adapter, u8 cam_id_a, u8 cam_id_b);
void rtw_clean_dk_section(_adapter *adapter);
void rtw_clean_hw_dk_cam(_adapter *adapter);

/* modify both HW and cache */
void write_cam(_adapter *padapter, u8 id, u16 ctrl, u8 *mac, u8 *key);
void clear_cam_entry(_adapter *padapter, u8 id);

/* modify cache only */
void write_cam_cache(_adapter *adapter, u8 id, u16 ctrl, u8 *mac, u8 *key);
void clear_cam_cache(_adapter *adapter, u8 id);

void invalidate_cam_all(_adapter *padapter);

void flush_all_cam_entry(_adapter *padapter, enum phl_cmd_type cmd_type, u32 cmd_timeout);

#if defined(DBG_CONFIG_ERROR_RESET) && defined(CONFIG_CONCURRENT_MODE)
void rtw_iface_bcmc_sec_cam_map_restore(_adapter *adapter);
#endif

#ifdef  CONFIG_DBG_AX_CAM

#define INDIRECT_ACCESS_ADDR	0xC04
#define INDIRECT_ACCESS_VALUE	0x40000
#define SEC_CAM_BASE_ADDR	0x18814000
#define BSSID_CAM_BASE_ADDR	0x18853000
#define ADDR_CAM_BASE_ADDR	0x18850000

enum CAM_KEY_TYPE {
	KEY_TYPE_UNI = 0,
	KEY_TYPE_GROUP = 1,
	KEY_TYPE_BIP = 2,
	KEY_TYPE_NONE = 3
};

/*Address cam field info DW0 - DW9*/

/* DWORD 0 ; Offset 00h */
#define GET_AX_ADDR_CAM_VALID(__pCAM)		LE_BITS_TO_4BYTE(__pCAM, 0, 1)
#define GET_AX_ADDR_CAM_NET_TYPE(__pCAM)	LE_BITS_TO_4BYTE(__pCAM, 1, 2)
#define GET_AX_ADDR_CAM_BCN_HD_COND(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 3, 2)
#define GET_AX_ADDR_CAM_HIT_RULE(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 5, 2)
#define GET_AX_ADDR_CAM_BB_SEL(__pCAM)		LE_BITS_TO_4BYTE(__pCAM, 7, 1)
#define GET_AX_ADDR_CAM_ADDR_MASK(__pCAM)	LE_BITS_TO_4BYTE(__pCAM, 8, 6)
#define GET_AX_ADDR_CAM_MASK_SEL(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 14, 2)
#define GET_AX_ADDR_CAM_SMA_HASH(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 16, 8)
#define GET_AX_ADDR_CAM_TMA_HASH(__pCAM)	LE_BITS_TO_4BYTE(__pCAM, 24, 8)

/* DWORD 1 ; Offset 04h */
#define GET_AX_ADDR_CAM_BSSID_CAM_IDX(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+4, 0, 6)
/* DWORD 2 ; Offset 08h */
#define GET_AX_ADDR_CAM_SMA_0(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+8, 0, 8)
#define GET_AX_ADDR_CAM_SMA_1(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+8, 8, 8)
#define GET_AX_ADDR_CAM_SMA_2(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+8, 16, 8)
#define GET_AX_ADDR_CAM_SMA_3(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+8, 24, 8)
/* DWORD 3 ; Offset 0Ch */
#define GET_AX_ADDR_CAM_SMA_4(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+12, 8, 6)
#define GET_AX_ADDR_CAM_SMA_5(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+12, 14, 2)
#define GET_AX_ADDR_CAM_TMA_0(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+12, 16, 8)
#define GET_AX_ADDR_CAM_TMA_1(__pCAM)		LE_BITS_TO_4BYTE(__pCAM+12, 24, 8)
/* DWORD 4 ; Offset 10h */
#define GET_AX_ADDR_CAM_TMA_2(__pCAM)		LE_BITS_TO_4BYTE(__pCAM+16, 0, 8)
#define GET_AX_ADDR_CAM_TMA_3(__pCAM)		LE_BITS_TO_4BYTE(__pCAM+16, 8, 8)
#define GET_AX_ADDR_CAM_TMA_4(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+16, 16, 8)
#define GET_AX_ADDR_CAM_TMA_5(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+16, 24, 8)
/* DWORD 5 ; Offset 14h : RSVD ALL*/
/* DWORD 6 ; Offset 18h */

#define GET_AX_ADDR_CAM_MACID(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+24, 0, 8)
#define GET_AX_ADDR_CAM_PORT_INT(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+24, 8, 3)
#define GET_AX_ADDR_CAM_TSF_SYNC(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+24, 11, 3)
#define GET_AX_ADDR_CAM_TF(__pCAM)		LE_BITS_TO_4BYTE(__pCAM+24, 14, 2)
#define GET_AX_ADDR_CAM_LSIG_TXOP(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+24, 16, 2)
#define GET_AX_ADDR_CAM_CTRLCNT_IDX(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+24, 18, 4)
#define GET_AX_ADDR_CAM_CTRLCNT_VALID(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+24, 20, 1)
#define GET_AX_ADDR_CAM_TARGET_IND(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+24, 24, 3)
#define GET_AX_ADDR_CAM_FRM_TARGET_IND(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+24, 27, 3)

/* DWORD 7 ; Offset 1Ch */
#define GET_AX_ADDR_CAM_AID12_0(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+28, 0, 8)
#define GET_AX_ADDR_CAM_AID12_1(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+28, 8, 4)
#define GET_AX_ADDR_CAM_WOL_PATTERN(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+28, 12, 1)
#define GET_AX_ADDR_CAM_WOL_UC(__pCAM)		LE_BITS_TO_4BYTE(__pCAM+28, 13, 1)
#define GET_AX_ADDR_CAM_WOL_MAGIC(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+28, 14, 1)
#define GET_AX_ADDR_CAM_WAPI(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM+28, 15, 1)
#define GET_AX_ADDR_CAM_SEC_ENT_MODE(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 16, 2)
#define GET_AX_ADDR_CAM_SEC_ENT0_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 18, 2)
#define GET_AX_ADDR_CAM_SEC_ENT1_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 20, 2)
#define GET_AX_ADDR_CAM_SEC_ENT2_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 22, 2)
#define GET_AX_ADDR_CAM_SEC_ENT3_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 24, 2)
#define GET_AX_ADDR_CAM_SEC_ENT4_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 26, 2)
#define GET_AX_ADDR_CAM_SEC_ENT5_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 28, 2)
#define GET_AX_ADDR_CAM_SEC_ENT6_KEYID(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+28, 30, 2)
/* DWORD 8 ; Offset 20h */
#define GET_AX_ADDR_CAM_SEC_ENT_VALID(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+32, 0, 8)
#define GET_AX_ADDR_CAM_SEC_ENT0(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+32, 8, 8)
#define GET_AX_ADDR_CAM_SEC_ENT1(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+32, 16, 8)
#define GET_AX_ADDR_CAM_SEC_ENT2(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+32, 24, 8)
/* DWORD 9 ; Offset 24h */
#define GET_AX_ADDR_CAM_SEC_ENT3(__pCAM)	LE_BITS_TO_4BYTE(__pCAM+36, 0, 8)
#define GET_AX_ADDR_CAM_SEC_ENT4(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+36, 8, 8)
#define GET_AX_ADDR_CAM_SEC_ENT5(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+36, 16, 8)
#define GET_AX_ADDR_CAM_SEC_ENT6(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM+36, 24, 8)

/* Security cam */
/*DWORD 0 ; offset 00h*/
#define GET_AX_SEC_CAM_TYPE(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM, 0, 4)
#define GET_AX_SEC_CAM_EXT_KEY(__pCAM)		LE_BITS_TO_4BYTE(__pCAM, 4, 1)
#define GET_AX_SEC_SPP_MODE_(__pCAM) 		LE_BITS_TO_4BYTE(__pCAM, 5, 1)

/*BSSID cam*/
/*DWORD 0 ; offset 00h*/
#define GET_AX_BSSID_CAM_VALID(__pCAM)		LE_BITS_TO_4BYTE(__pCAM, 0, 1)
#define GET_AX_BSSID_CAM_BB_SEL(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 1, 1)
#define GET_AX_BSSID_CAM_COLOR_(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 8, 6)
#define GET_AX_BSSID_CAM_BSSID_0(__pCAM)	LE_BITS_TO_4BYTE(__pCAM, 16, 8)
#define GET_AX_BSSID_CAM_BSSID_1(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 24, 8)
/*DWORD 1 ; offset 04h*/
#define GET_AX_BSSID_CAM_BSSID_2(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 0, 8)
#define GET_AX_BSSID_CAM_BSSID_3(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 8, 8)
#define GET_AX_BSSID_CAM_BSSID_4(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 16, 8)
#define GET_AX_BSSID_CAM_BSSID_5(__pCAM) 	LE_BITS_TO_4BYTE(__pCAM, 24, 8)

int get_ax_sec_cam(void* sel, struct _ADAPTER *a);
int get_ax_address_cam(void* sel, struct _ADAPTER *a);
int get_ax_valid_key(void* sel, struct _ADAPTER *a);

#endif /* CONFIG_DBG_AX_CAM */

#endif /* __RTW_SEC_CAM_H__ */
