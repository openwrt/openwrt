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

#ifndef _MAC_AX_FLASH_H_
#define _MAC_AX_FLASH_H_

#include "../type.h"
#include "../mac_def.h"
#include "../mac_ax.h"
#include "fwcmd.h"
#include "trx_desc.h"
#include "trxcfg.h"
#include "dle.h"

#define MAX_LENGTH_ENUM 7
#define FLASH_H2C_SIZE 1984
#define LEN_FLASH_C2H_HDR_ADDR 4
#define LEN_FLASH_C2H_HDR_LENGTH 4
#define LEN_FLASH_H2C_HDR 8
#define LEN_FLASH_C2H_HDR 8
#define MAX_READ_SIZE 1984

u32 mac_flash_erase(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u32 timeout);
u32 mac_flash_read(struct mac_ax_adapter *adapter,
		   u32 addr,
		   u32 length,
		   u8 *buffer,
		   u32 timeout);
u32 mac_flash_write(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u8 *buffer,
		    u32 timeout);
u32 c2h_sys_flash_pkt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		      struct rtw_c2h_info *info);

#endif
