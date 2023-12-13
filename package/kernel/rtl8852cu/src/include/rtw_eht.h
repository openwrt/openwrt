/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#ifndef _RTW_EHT_H_
#define _RTW_EHT_H_

struct mlo_priv {
	u8	mlo_option;
	u8	tid2link_nego;
	/* ToDo */
};

struct eht_priv {
	u8	eht_option;
	/* ToDo */
};

#define EHT_ML_CONTROL_LEN 2

/* Min length of common info in basic ML
** Common info length field: 1 */
#define EHT_BASIC_COMMON_INFO_MIN_LEN 1 + ETH_ALEN

/* Min length of a legal basic ML element
** Element ID + Element length fields: 2
** Element ID extension field: 1 */
#define EHT_ML_BASIC_MIN_LEN 2 + 1 + EHT_ML_CONTROL_LEN + EHT_BASIC_COMMON_INFO_MIN_LEN

/* Min length of a legal probe req ML element
** Element ID + Element length fields: 2
** Element ID extension field: 1
** Common info length field: 1 */
#define EHT_ML_PROBEREQ_MIN_LEN 2 + 1 + EHT_ML_CONTROL_LEN + 1

#define EHT_ML_STA_CONTROL_LEN 2

void rtw_set_assoc_status(_adapter *padapter, struct rtw_phl_mld_t *pmld, u8 link_id, u16 status);

/*
  rtw_update_ies_with_inheritance(): API for core to update the current IEs with the inherited IEs
  @ cur_ies: current IEs
  @ cur_ies_len: length of the current IEs
  @ inh_ies: the inherited IEs
  @ inh_ies_len: length of the inherited IEs
*/
void rtw_update_ies_with_inheritance(u8 *cur_ies, u32 *cur_ies_len, u8 *inh_ies, u8 inh_ies_len);

u32 rtw_parse_ml_ie(_adapter *padapter, u8 *ies, u32 ies_len,
		struct rtw_phl_ml_element *ml_ele);

/*
  rtw_eht_ml_ies_handler(): API for core to parse ML related element in Assoc Rsp
  @ ies: start point of IEs in Assoc Rsp
  @ ies_len: length of the IEs in Assoc Rsp
*/
void rtw_eht_ml_ies_handler(struct _ADAPTER *padapter, struct _ADAPTER_LINK *padapter_link,
				u8 *ies, u32 ies_len);

/*
  rtw_build_ml_ie(): API for core to build ML element
  @ pkt_type: packet type, e.g., PACKET_AUTH, PACKET_ASSOC_REQUEST, etc
  @ primary_ies: IEs built in the primary link
  @ primary_ies_len: length of the IEs in the primary link
*/
u8 *rtw_build_ml_ies(_adapter *padapter, struct sta_info *psta, u8 *pframe, struct pkt_attrib *pattrib,
			enum phl_packet_type pkt_type, u8 *primary_ies, u32 primary_ies_len, u8 reserved);

u32 rtw_restructure_ml_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *in_ie, u8 *out_ie, uint in_len, uint *pout_len);

#endif /* _RTW_EHT_H_ */
