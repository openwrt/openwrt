/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _PHL_API_DRV_H_
#define _PHL_API_DRV_H_

void *rtw_phl_get_txbd_buf(struct rtw_phl_com_t *phl_com);
void *rtw_phl_get_rxbd_buf(struct rtw_phl_com_t *phl_com);

/**
 * rtw_phl_query_h2c_pkt - provide h2c buffer for halmac
 * @phl_com: see struct rtw_phl_com_t
 * @type: the type of h2c buf
 *
 * returns struct rtw_h2c_pkt*
 */
struct rtw_h2c_pkt *rtw_phl_query_h2c_pkt(struct rtw_phl_com_t *phl_com,
										  enum rtw_h2c_pkt_type type);

/**
 * rtw_phl_pltfm_tx - h2c platform transmit
 * @phl_com: see struct rtw_phl_com_t
 * @pkt: the h2c pkt
 *
 * returns enum RTW_PHL_STATUS
 */
enum rtw_phl_status rtw_phl_pltfm_tx(struct rtw_phl_com_t *phl_com,
									 struct rtw_h2c_pkt *pkt);

enum rtw_phl_status rtw_phl_msg_hub_hal_send(struct rtw_phl_com_t *phl_com,
						struct phl_msg_attribute* attr, struct phl_msg* msg);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_self(void *phl, struct rtw_wifi_role_link_t *rlink);

struct rtw_phl_mld_t *
rtw_phl_get_mld_self(void *phl, struct rtw_wifi_role_t *wrole);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_by_macid(void *phl, u16 macid);

u16 rtw_phl_get_macid_by_addr(void *phl, u8 *addr);

bool rtw_phl_query_regulation_info(void *phl, struct rtw_regulation_info *info);

u8 rtw_phl_get_center_ch(struct rtw_chan_def *chan_def);

bool rtw_phl_btc_send_cmd(struct rtw_phl_com_t *phl_com,
			enum phl_band_idx hw_band, u8 *buf, u32 len, u16 ev_id);
void rtw_phl_cmd_scan_send_msg(void *phl, u16 evt_id, u8 band_idx, u8 *buf,
			       u32 len);
#ifdef CONFIG_PHL_CHANNEL_INFO
/* Channel info queue operation*/
u32 rtw_phl_get_chaninfo_idle_number(void *drvpriv, struct rtw_phl_com_t *phl_com);
u32 rtw_phl_get_chaninfo_busy_number(void *drvpriv, struct rtw_phl_com_t *phl_com);
struct chan_info_t *rtw_phl_query_idle_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com);

struct chan_info_t *rtw_phl_query_busy_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com);

struct chan_info_t *rtw_phl_query_busy_chaninfo_latest(void *drvpriv, struct rtw_phl_com_t *phl_com);

void rtw_phl_enqueue_idle_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com,
				struct chan_info_t *chan_info_pkt);

struct chan_info_t * rtw_phl_recycle_busy_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com,
				struct chan_info_t *chan_info_pkt);
#endif /* CONFIG_PHL_CHANNEL_INFO */

enum rtw_phl_status rtw_phl_ser_l2_notify(struct rtw_phl_com_t *phl_com);

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_cmd_notify(struct rtw_phl_com_t *phl_com,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);
#endif /* CONFIG_CMD_DISP */

enum rtw_phl_status phl_ps_hal_pwr_req(struct rtw_phl_com_t *phl_com, u8 src, bool pwr_req);

void rtw_phl_init_chdef(struct rtw_phl_com_t *phl_com, struct rtw_chan_def *chdef);

#define rtw_phl_get_rlink(_wrole, _idx) (&(_wrole->rlink[_idx]))

void rtw_phl_enable_interrupt_sync(struct rtw_phl_com_t* phl_com);
void rtw_phl_disable_interrupt_sync(struct rtw_phl_com_t* phl_com);

/* packet offload */
void rtw_phl_pkt_ofld_del_all_entry_req(struct rtw_phl_com_t *phl_com);

enum rtw_phl_status
rtw_phl_pkt_ofld_null_request(struct rtw_phl_com_t* phl_com,
							  struct rtw_phl_stainfo_t *sta, u32 *token);

enum rtw_phl_status rtw_phl_pkt_ofld_reset_entry(struct rtw_phl_com_t* phl_com, u16 macid);

#ifdef CONFIG_PHL_FW_DUMP_EFUSE
void rtw_phl_fw_dump_efuse_precfg(struct rtw_phl_com_t* phl_com);
void rtw_phl_fw_dump_efuse_postcfg(struct rtw_phl_com_t* phl_com);
#endif /* CONFIG_PHL_FW_DUMP_EFUSE */

const char *rtw_phl_get_lstate_str(enum link_state lstate);

#endif /* _PHL_API_DRV_H_ */

