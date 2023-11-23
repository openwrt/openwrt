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
#ifndef __PHL_PHY_MSG_FWD_H_
#define __PHL_PHY_MSG_FWD_H_

enum rtw_phl_status phl_msg_hub_init(struct phl_info_t *phl);
enum rtw_phl_status phl_msg_hub_deinit(struct phl_info_t *phl);
enum rtw_phl_status phl_msg_hub_start(struct phl_info_t *phl);
enum rtw_phl_status phl_msg_hub_stop(struct phl_info_t *phl);
enum rtw_phl_status phl_msg_hub_send(struct phl_info_t *phl,
				     struct phl_msg_attribute *attr,
				     struct phl_msg *msg);
enum rtw_phl_status
phl_msg_hub_register_recver(void *phl, struct phl_msg_receiver *ctx,
			    enum phl_msg_recver_layer layer);
enum rtw_phl_status
phl_msg_hub_update_recver_mask(void *phl, enum phl_msg_recver_layer layer,
			       u8 *mdl_id, u8 len, u8 clr);
enum rtw_phl_status
phl_msg_hub_deregister_recver(void *phl, enum phl_msg_recver_layer layer);

void phl_msg_hub_phy_mgnt_evt_hdlr(struct phl_info_t *phl, u16 evt_id);
void phl_msg_hub_rx_evt_hdlr(struct phl_info_t *phl, u16 evt_id, u8 *buf,
			     u32 len);
#endif
