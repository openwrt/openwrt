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
#ifndef _HAL_TRX_8852CU_H_
#define _HAL_TRX_8852CU_H_

/* Release report size and format*/
#define RX_RP_PACKET_SIZE 4

#define GET_RX_RP_PKT_POLLUTED(rppkt) LE_BITS_TO_4BYTE(rppkt + 0x00, 31, 1)
#define GET_RX_RP_PKT_PCIE_SEQ(rppkt) LE_BITS_TO_4BYTE(rppkt + 0x00, 16, 15)
#define GET_RX_RP_PKT_TX_STS(rppkt) LE_BITS_TO_4BYTE(rppkt + 0x00, 13, 3)
#define GET_RX_RP_PKT_QSEL(rppkt) LE_BITS_TO_4BYTE(rppkt + 0x00, 8, 5)
#define GET_RX_RP_PKT_MAC_ID(rppkt) LE_BITS_TO_4BYTE(rppkt + 0x00, 0, 8)

#endif /*_HAL_TRX_8852CU_H_*/
