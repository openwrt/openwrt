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
#ifndef _H2C_AGG_H_
#define _H2C_AGG_H_

#include "../type.h"
#include "fwcmd.h"
#include "../fw_ax/inc_hdr/fwcmd_intf.h"

#define ALIGN_4_BYTE(len) ((len + 0x3) & ~(0x3))
#define H2C_AGG_SUB_HDR_LEN sizeof(u32)
#define H2C_PKT_AGGREGATABLE(len) ((ALIGN_4_BYTE(len) + \
								   WD_BODY_LEN + FWCMD_HDR_LEN + \
								   H2C_AGG_SUB_HDR_LEN) \
								  < H2C_LONG_DATA_LEN)

void mac_h2c_agg_flush(struct mac_ax_adapter *adapter);
u32 mac_h2c_agg_tx(struct mac_ax_adapter *adapter);
void mac_h2c_agg_enable(struct mac_ax_adapter *adapter, u8 enable);
#endif
