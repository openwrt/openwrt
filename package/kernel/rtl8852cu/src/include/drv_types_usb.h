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
#ifndef __DRV_TYPES_USB_H__
#define __DRV_TYPES_USB_H__

/*FOR 8852AU*/
#define MAX_ENDPOINT_NUM 9 /* 2 x Bulk-IN + 7 x Bulk-OUT */
#define MAX_BULKOUT_NUM 7
#define MAX_BULKIN_NUM 2 /*EP4 for Bulk-IN, EP8 for Bulk-IN interrupt or Bulk-INT*/

typedef struct usb_data {
	u8 usb_intf_start;

	u8 usb_speed; /* 1.1, 2.0 or 3.0 */
	u16 usb_bulkout_size;
	u8 nr_endpoint; /*MAX_ENDPOINT_NUM*/

	/* Bulk In , Out Pipe information */
	int RtInPipe[MAX_BULKIN_NUM];
	u8 inpipe_type[MAX_BULKIN_NUM];
	u8 RtNumInPipes;
	int RtOutPipe[MAX_BULKOUT_NUM];
	u8 RtNumOutPipes;

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_mutex usb_vendor_req_mutex;
#endif

#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	u8 *usb_alloc_vendor_req_buf;
	u8 *usb_vendor_req_buf;
#endif

#ifdef PLATFORM_LINUX
	struct usb_interface *pusbintf;
	struct usb_device *pusbdev;
#endif/* PLATFORM_LINUX */
} USB_DATA, *PUSB_DATA;
#endif /*__DRV_TYPES_USB_H__*/
