/*
 * Copyright(c) 2018 Realtek Corporation. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 */

#ifndef __RTW_HWSIM_INTF_H_
#define __RTW_HWSIM_INTF_H_

#include <linux/skbuff.h>

#include <drv_types.h>

int rtw_hwsim_medium_tx(struct _ADAPTER *adapter, const void *tx_ctx,
                        u8 *buf, size_t buflen);

void rtw_hwsim_medium_pre_netif_rx(struct sk_buff *skb);
	

#endif	/* __RTW_HWSIM_INTF_H__ */
