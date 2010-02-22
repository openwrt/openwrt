/*
 * arch/ubicom32/mach-common/usb_tio.h
 *   Definitions for usb_tio.c
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <asm/devtree.h>
#include <asm/ip5000.h>

#ifndef _USB_TIO_H
#define _USB_TIO_H

#undef  USB_TIO_DEBUG

#define USB_TIO_REQUEST_MAGIC1	0x2307
#define USB_TIO_REQUEST_MAGIC2	0x0789
#if defined(USB_TIO_DEBUG)
#define USB_TIO_REQUEST_VERIFY_MAGIC(req)	usb_tio_request_verify_magic(req)
#define USB_TIO_REQUEST_SET_MAGIC(req)          usb_tio_request_set_magic(req)
#define USB_TIO_REQUEST_CLEAR_MAGIC(req)	usb_tio_request_clear_magic(req)
#else
#define USB_TIO_REQUEST_VERIFY_MAGIC(req)
#define USB_TIO_REQUEST_SET_MAGIC(req)          usb_tio_request_set_magic(req)
#define USB_TIO_REQUEST_CLEAR_MAGIC(req)
#endif

enum USB_TIO_status {
	USB_TIO_OK,
	USB_TIO_ERROR,
	USB_TIO_ERROR_COMMIT,
};

enum USB_TIO_cmds {
	USB_TIO_READ16_SYNC,
	USB_TIO_READ8_SYNC,
	USB_TIO_READ_FIFO_SYNC,

	USB_TIO_WRITE16_ASYNC,
	USB_TIO_WRITE8_ASYNC,
	USB_TIO_WRITE_FIFO_ASYNC,

	USB_TIO_WRITE16_SYNC,
	USB_TIO_WRITE8_SYNC,
	USB_TIO_WRITE_FIFO_SYNC,

};

enum USB_TIO_state {
	USB_TIO_NORMAL,
	USB_TIO_DMA_SETUP,
};

struct usb_tio_request {
	volatile u32_t address;
	union {
		volatile u32_t data;
		volatile u32_t buffer;
	};
	volatile u16_t cmd;
	const volatile u16_t status;
	volatile u32_t transfer_length;
	volatile u32_t thread_mask;
	volatile u16_t magic;
};

struct usbtio_node {
	struct devtree_node dn;
	volatile struct usb_tio_request * volatile pdesc;
	struct usb_tio_request	request;
	volatile u32_t usb_vp_config;
	volatile u32_t usb_vp_control;
	const volatile u32_t usb_vp_status;
	volatile u16_t usb_vp_hw_int_tx;
	volatile u16_t usb_vp_hw_int_rx;
	volatile u8_t  usb_vp_hw_int_usb;
	volatile u8_t usb_vp_hw_int_mask_usb;
        volatile u16_t usb_vp_hw_int_mask_tx;
        volatile u16_t usb_vp_hw_int_mask_rx;

};

extern struct usbtio_node *usb_node;
extern void ubi32_usb_init(void);
#endif
