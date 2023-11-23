/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef __PHL_PKT_OFLD_H__
#define __PHL_PKT_OFLD_H__

#define TYPE_DATA_FRAME 0x08
#define TYPE_ACTION_FRAME 0xD0
#define TYPE_NULL_FRAME 0x48
#define TYPE_PROBE_REQ_FRAME 0x40

#define HDR_OFFSET_FRAME_CONTROL 0
#define HDR_OFFSET_DURATION 2
#define HDR_OFFSET_ADDRESS1 4
#define HDR_OFFSET_ADDRESS2 10
#define HDR_OFFSET_ADDRESS3 16
#define HDR_OFFSET_SEQUENCE 22
#define HDR_OFFSET_ADDRESS4 24

#define SET_80211_PKT_HDR_FRAME_CONTROL(_hdr, _val)	\
	WriteLE2Byte(_hdr, _val)
#define SET_80211_PKT_HDR_TYPE_AND_SUBTYPE(_hdr, _val)	\
	WriteLE1Byte(_hdr, _val)
#define SET_80211_PKT_HDR_PROTOCOL_VERSION(_hdr, _val)	\
	SET_BITS_TO_LE_2BYTE(_hdr, 0, 2, _val)
#define SET_80211_PKT_HDR_TYPE(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 2, 2, _val)
#define SET_80211_PKT_HDR_SUBTYPE(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 4, 4, _val)
#define SET_80211_PKT_HDR_TO_DS(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 8, 1, _val)
#define SET_80211_PKT_HDR_FROM_DS(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 9, 1, _val)
#define SET_80211_PKT_HDR_MORE_FRAG(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 10, 1, _val)
#define SET_80211_PKT_HDR_RETRY(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 11, 1, _val)
#define SET_80211_PKT_HDR_PWR_MGNT(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 12, 1, _val)
#define SET_80211_PKT_HDR_MORE_DATA(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 13, 1, _val)
#define SET_80211_PKT_HDR_PROTECT(_hdr, _val)		\
	SET_BITS_TO_LE_2BYTE(_hdr, 14, 1, _val)

#define SET_80211_PKT_HDR_DURATION(_hdr, _val)	\
	WriteLE2Byte((u8 *)(_hdr)+HDR_OFFSET_DURATION, _val)
#define SET_80211_PKT_HDR_ADDRESS1(_h, _hdr, _val)	\
	_os_mem_cpy(_h, _hdr+HDR_OFFSET_ADDRESS1, _val, MAC_ALEN)
#define SET_80211_PKT_HDR_ADDRESS2(_h, _hdr, _val) \
	_os_mem_cpy(_h, _hdr+HDR_OFFSET_ADDRESS2, _val, MAC_ALEN)
#define SET_80211_PKT_HDR_ADDRESS3(_h, _hdr, _val) \
	_os_mem_cpy(_h, _hdr+HDR_OFFSET_ADDRESS3, _val, MAC_ALEN)
#define SET_80211_PKT_HDR_FRAGMENT_SEQUENCE(_hdr, _val) \
	WriteLE2Byte((u8 *)(_hdr)+HDR_OFFSET_SEQUENCE, _val)


#define NOT_USED 0xFF
#define NULL_PACKET_LEN 24
#define MAC_HDR_LEN 24
#define FCS_LEN 4

#define MAX_MSDU_LEN 2304

/* 11w SA-Query */
#define SAQ_ACTION_LEN 4

#define SAQ_OFFSET_ACTION 1

#define ACT_TYPE_SA_QUERY 8

#define SET_ACTION_FRAME_CATEGORY(_ptr, _val) WriteLE1Byte(_ptr, _val)
#define SET_SAQ_ACTION_FIELD(_ptr, _val) WriteLE1Byte(_ptr+SAQ_OFFSET_ACTION, _val)


#define LLC_LEN 8

#define LLC_OFFSET_HDR 0
#define LLC_OFFSET_OUI 3
#define LLC_OFFSET_PROTO_ID 6

#define SET_LLC_HDR(_h, _ptr, _val) _os_mem_cpy(d, _ptr+LLC_OFFSET_HDR, _val, 3);
#define SET_LLC_OUI(_h, _ptr, _val) _os_mem_cpy(d, _ptr+LLC_OFFSET_OUI, _val, 3);
#define SET_LLC_PROTO_ID(_h, _ptr, _val) _os_mem_cpy(d, _ptr+LLC_OFFSET_PROTO_ID, _val, 2);

#define AUTH_1X_HDR_LEN 4

#define AUTH_1X_OFFSET_PROTO_VER 0
#define AUTH_1X_OFFSET_PKT_TYPE 1
#define AUTH_1X_OFFSET_LENGTH 2

#define LIB1X_EAPOL_VER	1 /* 00000001B */

#define LIB1X_TYPE_EAPOL_EAPPKT 0 /* 0000 0000B */
#define LIB1X_TYPE_EAPOL_START 1 /* 0000 0001B */
#define LIB1X_TYPE_EAPOL_LOGOFF 2 /* 0000 0010B */
#define LIB1X_TYPE_EAPOL_KEY 3 /* 0000 0011B */
#define LIB1X_TYPE_EAPOL_ENCASFALERT 4 /* 0000 0100B */

#define SET_AUTH_1X_PROTO_VER(_ptr, _val) WriteLE1Byte(_ptr+AUTH_1X_OFFSET_PROTO_VER, _val)
#define SET_AUTH_1X_PKT_TYPE(_ptr, _val) WriteLE1Byte(_ptr+AUTH_1X_OFFSET_PKT_TYPE, _val)
#define SET_AUTH_1X_LENGTH(_ptr, _val) WriteBE2Byte(_ptr+AUTH_1X_OFFSET_LENGTH, _val)

#define EAPOLMSG_HDR_LEN 95

#define EAPOLKEY_OFFSET_KEYDESC_TYPE 0
#define EAPOLKEY_OFFSET_KEY_INFO 1
#define EAPOLKEY_OFFSET_REPLAY_CNT 5

#define EAPOLKEY_KEYDESC_TYPE_RSN 2
#define EAPOLKEY_KEYDESC_VER_1 1
#define EAPOLKEY_KEYDESC_VER_2 2
#define EAPOLKEY_KEYDESC_VER_3 3

#define EAPOLKEY_KEYMIC BIT(8)
#define EAPOLKEY_SECURE BIT(9)

#define SET_EAPOLKEY_KEYDESC_TYPE(_ptr, _val) WriteLE1Byte(_ptr+EAPOLKEY_OFFSET_KEYDESC_TYPE, _val)
#define SET_EAPOLKEY_KEY_INFO(_ptr, _val) WriteBE2Byte(_ptr+EAPOLKEY_OFFSET_KEY_INFO, _val)
#define SET_EAPOLKEY_REPLAY_CNT(_h, _ptr, _val)	_os_mem_cpy(_h, _ptr+EAPOLKEY_OFFSET_REPLAY_CNT, _val, 8);

struct pkt_ofld_obj {
	_os_mutex mux;
	struct phl_info_t *phl_info;

	struct list_head entry_q;
	u32 entry_cnt;

	u32 cur_seq;
};

struct pkt_ofld_req {
	struct list_head list;
	u32 token;
	char *req_name;
	u32 req_name_len;
};

struct rtw_pkt_ofld_probe_req_info {
	u8 a2[MAC_ADDRESS_LENGTH];
	void (* construct_pbreq)(void *priv, u8 *pkt_buf, u16 *len);
};

struct rtw_pkt_ofld_null_info {
	u8 a1[MAC_ADDRESS_LENGTH];
	u8 a2[MAC_ADDRESS_LENGTH];
	u8 a3[MAC_ADDRESS_LENGTH];
};


struct rtw_pkt_ofld_arp_rsp_info {
	u8 a1[MAC_ADDRESS_LENGTH];
	u8 a2[MAC_ADDRESS_LENGTH];
	u8 a3[MAC_ADDRESS_LENGTH];
	u8 host_ipv4_addr[IPV4_ADDRESS_LENGTH];
	u8 remote_mac_addr[MAC_ADDRESS_LENGTH];
	u8 remote_ipv4_addr[IPV4_ADDRESS_LENGTH];
	u8 protect_bit;
	u8 sec_hdr_len;
};

struct rtw_pkt_ofld_na_info {
	u8 a1[MAC_ADDRESS_LENGTH];
	u8 a2[MAC_ADDRESS_LENGTH];
	u8 a3[MAC_ADDRESS_LENGTH];
	u8 protect_bit;
	u8 sec_hdr_len;
};

struct rtw_pkt_ofld_eapol_key_info {
	u8 a1[MAC_ADDRESS_LENGTH];
	u8 a2[MAC_ADDRESS_LENGTH];
	u8 a3[MAC_ADDRESS_LENGTH];
	u8 protect_bit;
	u8 sec_hdr_len;
	u8 key_desc_ver;
	u8 replay_cnt[8];
};

struct rtw_pkt_ofld_sa_query_info {
	u8 a1[MAC_ADDRESS_LENGTH];
	u8 a2[MAC_ADDRESS_LENGTH];
	u8 a3[MAC_ADDRESS_LENGTH];
	u8 protect_bit;
	u8 sec_hdr_len;
};

struct rtw_pkt_ofld_realwow_kapkt_info {
	u8 keep_alive_pkt_ptrn[MAX_REALWOW_KCP_SIZE];
	u16 keep_alive_pkt_size;
};

struct rtw_pkt_ofld_realwow_ack_info {
	u8 ack_ptrn[MAX_REALWOW_PAYLOAD];
	u16 ack_ptrn_size;
};

struct rtw_pkt_ofld_realwow_wp_info {
	u8 wakeup_ptrn[MAX_REALWOW_PAYLOAD];
	u16 wakeup_ptrn_size;
	u8 wakeupsecnum; /* ? */
};

/* init api */
enum rtw_phl_status phl_pkt_ofld_init(struct phl_info_t *phl_info);
void phl_pkt_ofld_deinit(struct phl_info_t *phl_info);
void phl_pkt_ofld_del_all_entry_req(struct phl_info_t *phl_info);

enum rtw_phl_status phl_pkt_ofld_add_entry(struct phl_info_t *phl_info, u16 macid);
enum rtw_phl_status phl_pkt_ofld_del_entry(struct phl_info_t *phl_info, u16 macid);

enum rtw_phl_status rtw_phl_pkt_ofld_request(struct phl_info_t *phl_info,
						u16 macid, u8 type,
						u32 *token, void *buf, const char *req_name);

enum rtw_phl_status rtw_phl_pkt_ofld_cancel(struct phl_info_t *phl_info,
					u16 macid, u8 type, u32 *token);

void phl_pkt_ofld_show_info(struct phl_info_t *phl_info);
u8 phl_pkt_ofld_get_id(struct phl_info_t *phl_info, u16 macid, u8 type);
const char *phl_get_pkt_ofld_str(enum pkt_ofld_type type);

#endif /* __PHL_PKT_OFLD_H__ */

