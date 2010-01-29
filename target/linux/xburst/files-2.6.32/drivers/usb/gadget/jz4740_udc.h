/*
 * linux/drivers/usb/gadget/jz4740_udc.h
 *
 * Ingenic JZ4740 on-chip high speed USB device controller
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __USB_GADGET_JZ4740_H__
#define __USB_GADGET_JZ4740_H__

/*-------------------------------------------------------------------------*/

// Max packet size
#define EP0_MAXPACKETSIZE  	64
#define EPBULK_MAXPACKETSIZE  	512
#define EPINTR_MAXPACKETSIZE  	64

#define UDC_MAX_ENDPOINTS       4

/*-------------------------------------------------------------------------*/

typedef enum ep_type {
	ep_control, ep_bulk_in, ep_bulk_out, ep_interrupt
} ep_type_t;

struct jz4740_ep {
	struct usb_ep ep;
	struct jz4740_udc *dev;

	const struct usb_endpoint_descriptor *desc;
	unsigned long pio_irqs;

	uint8_t stopped;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;

	ep_type_t type;
	size_t fifo;
	u32 csr;

	uint32_t reg_addr;
	struct list_head queue;
};

struct jz4740_request {
	struct usb_request req;
	struct list_head queue;
};

enum ep0state {
	WAIT_FOR_SETUP,		/* between STATUS ack and SETUP report */
	DATA_STATE_XMIT, 	/* data tx stage */
	DATA_STATE_NEED_ZLP,	/* data tx zlp stage */
	WAIT_FOR_OUT_STATUS,	/* status stages */
	DATA_STATE_RECV,	/* data rx stage */
};

/* For function binding with UDC Disable - Added by River */
typedef enum {
	UDC_STATE_ENABLE = 0,
	UDC_STATE_DISABLE,
}udc_state_t;

struct jz4740_udc {
	struct usb_gadget gadget;
	struct usb_gadget_driver *driver;
	struct device *dev;
	spinlock_t lock;

	enum ep0state ep0state;
	struct jz4740_ep ep[UDC_MAX_ENDPOINTS];

	unsigned char usb_address;
	
	udc_state_t state;

	struct resource *mem;
	void __iomem *base;
	int irq;
	uint32_t in_mask;
	uint32_t out_mask;

	struct clk *clk;
};

extern struct jz4740_udc *the_controller;

#define ep_is_in(EP) 		(((EP)->bEndpointAddress&USB_DIR_IN)==USB_DIR_IN)
#define ep_maxpacket(EP) 	((EP)->ep.maxpacket)
#define ep_index(EP) 		((EP)->bEndpointAddress&0xF)

#endif /* __USB_GADGET_JZ4740_H__ */
