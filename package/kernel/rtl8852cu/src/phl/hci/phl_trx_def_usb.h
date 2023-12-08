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
#ifndef _PHL_TRX_DEF_USB_H_
#define _PHL_TRX_DEF_USB_H_

/* Release report related format */
#define WP_TID_INDIC_RESERVED_BIT BIT(14)
#define WP_RESERVED_SEQ 0xFFF
#define WP_MAX_SEQ_NUMBER WP_RESERVED_SEQ
#define TX_STATUS_TX_DONE 0
#define TX_STATUS_TX_FAIL_REACH_RTY_LMT 1
#define TX_STATUS_TX_FAIL_LIFETIME_DROP 2
#define TX_STATUS_TX_FAIL_MACID_DROP 3
#define TX_STATUS_TX_FAIL_SW_DROP 4
#define TX_STATUS_TX_FAIL_FORCE_DROP_BY_STUCK 5

#define WP_MAX_CNT 4096
#define WP_USED_SEQ 0xFFFF

/* for tx */
struct phl_usb_buf {
	_os_list list;
	u8 *buffer;
	u32 buf_len;
	u8 type;
};


/* for rx */
struct rtw_rx_buf {
	_os_list list;
	u8 *buffer;
	u32 buf_len;
	u32 transfer_len;
	u8 pipe_idx;

	_os_lock lock;	// using phl_queue??
	int pktcnt;		// for usb aggregation
	void *os_priv;
};

#endif	/* _PHL_TRX_DEF_USB_H_ */
