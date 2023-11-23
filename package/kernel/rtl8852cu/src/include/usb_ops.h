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
#ifndef __USB_OPS_H_
#define __USB_OPS_H_


#define REALTEK_USB_VENQT_READ		0xC0
#define REALTEK_USB_VENQT_WRITE	0x40
#define REALTEK_USB_VENQT_CMD_REQ	0x05
#define REALTEK_USB_VENQT_CMD_IDX	0x00
#define REALTEK_USB_BULK_IN_EP_IDX	0
#define REALTEK_USB_IN_INT_EP_IDX	1

enum {
	VENDOR_WRITE = 0x00,
	VENDOR_READ = 0x01,
};
#define ALIGNMENT_UNIT				16
#define MAX_VENDOR_REQ_CMD_SIZE	254		/* 8188cu SIE Support */
#define MAX_USB_IO_CTL_SIZE		(MAX_VENDOR_REQ_CMD_SIZE + ALIGNMENT_UNIT)

#ifdef PLATFORM_LINUX
#include <usb_ops_linux.h>
#endif /* PLATFORM_LINUX */

#define IS_FULL_SPEED_USB(_dvobj)	(dvobj_to_usb(_dvobj)->usb_speed == RTW_USB_SPEED_FULL)
#define IS_HIGH_SPEED_USB(_dvobj)	(dvobj_to_usb(_dvobj)->usb_speed == RTW_USB_SPEED_HIGH)
#define IS_SUPER_SPEED_USB(_dvobj)	(dvobj_to_usb(_dvobj)->usb_speed == RTW_USB_SPEED_SUPER)
#define IS_SUPER_PLUS_SPEED_USB(_dvobj)	(dvobj_to_usb(_dvobj)->usb_speed == RTW_USB_SPEED_SUPER_10G)

static inline u16 rtw_usb_bulkout_size(struct dvobj_priv *dvobj)
{
	if (IS_SUPER_SPEED_USB(dvobj))
		return USB_SUPER_SPEED_BULK_SIZE;
	else if (IS_HIGH_SPEED_USB(dvobj))
		return USB_HIGH_SPEED_BULK_SIZE;
	else
		return USB_FULL_SPEED_BULK_SIZE;
}

static inline u8 rtw_usb_bulkout_size_boundary(struct dvobj_priv *dvobj, int buf_len)
{
	u8 rst = _TRUE;

	if (IS_SUPER_SPEED_USB(dvobj))
		rst = (0 == (buf_len) % USB_SUPER_SPEED_BULK_SIZE) ? _TRUE : _FALSE;
	else if (IS_HIGH_SPEED_USB(dvobj))
		rst = (0 == (buf_len) % USB_HIGH_SPEED_BULK_SIZE) ? _TRUE : _FALSE;
	else
		rst = (0 == (buf_len) % USB_FULL_SPEED_BULK_SIZE) ? _TRUE : _FALSE;
	return rst;
}


#endif /* __USB_OPS_H_ */
