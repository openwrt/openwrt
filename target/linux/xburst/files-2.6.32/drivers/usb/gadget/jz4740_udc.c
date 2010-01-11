/*
 * linux/drivers/usb/gadget/jz4740_udc.c
 *
 * Ingenic JZ4740 on-chip high speed USB device controller
 *
 * Copyright (C) 2006 - 2008 Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * This device has ep0, two bulk-in/interrupt-in endpoints, and one bulk-out endpoint.
 *
 *  - Endpoint numbering is fixed: ep0, ep1in-int, ep2in-bulk, ep1out-bulk.
 *  - DMA works with bulk-in (channel 1) and bulk-out (channel 2) endpoints.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/usb.h>
#include <linux/usb/gadget.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/mach-jz4740/jz4740.h>
#include <asm/mach-jz4740/clock.h>

#include "jz4740_udc.h"

#define JZ_REG_UDC_FADDR	0x00 /* Function Address 8-bit */
#define JZ_REG_UDC_POWER	0x01 /* Power Managemetn 8-bit */
#define JZ_REG_UDC_INTRIN	0x02 /* Interrupt IN 16-bit */
#define JZ_REG_UDC_INTROUT	0x04 /* Interrupt OUT 16-bit */
#define JZ_REG_UDC_INTRINE	0x06 /* Intr IN enable 16-bit */
#define JZ_REG_UDC_INTROUTE	0x08 /* Intr OUT enable 16-bit */
#define JZ_REG_UDC_INTRUSB	0x0a /* Interrupt USB 8-bit */
	#define JZ_REG_UDC_INTRUSBE	0x0b /* Interrupt USB Enable 8-bit */
#define JZ_REG_UDC_FRAME	0x0c /* Frame number 16-bit */
#define JZ_REG_UDC_INDEX	0x0e /* Index register 8-bit */
#define JZ_REG_UDC_TESTMODE	0x0f /* USB test mode 8-bit */

#define JZ_REG_UDC_CSR0		0x12 /* EP0 CSR 8-bit */
#define JZ_REG_UDC_INMAXP	0x10 /* EP1-2 IN Max Pkt Size 16-bit */
#define JZ_REG_UDC_INCSR	0x12 /* EP1-2 IN CSR LSB 8/16bit */
#define JZ_REG_UDC_INCSRH	0x13 /* EP1-2 IN CSR MSB 8-bit */
#define JZ_REG_UDC_OUTMAXP	0x14 /* EP1 OUT Max Pkt Size 16-bit */
#define JZ_REG_UDC_OUTCSR	0x16 /* EP1 OUT CSR LSB 8/16bit */
#define JZ_REG_UDC_OUTCSRH	0x17 /* EP1 OUT CSR MSB 8-bit */
#define JZ_REG_UDC_OUTCOUNT	0x18 /* bytes in EP0/1 OUT FIFO 16-bit */

#define JZ_REG_UDC_EP_FIFO(x)	(4 * (x) + 0x20)

#define JZ_REG_UDC_EPINFO	0x78 /* Endpoint information */
#define JZ_REG_UDC_RAMINFO	0x79 /* RAM information */

#define JZ_REG_UDC_INTR		0x200 /* DMA pending interrupts */
#define JZ_REG_UDC_CNTL1	0x204 /* DMA channel 1 control */
#define JZ_REG_UDC_ADDR1	0x208 /* DMA channel 1 AHB memory addr */
#define JZ_REG_UDC_COUNT1	0x20c /* DMA channel 1 byte count */
#define JZ_REG_UDC_CNTL2	0x214 /* DMA channel 2 control */
#define JZ_REG_UDC_ADDR2	0x218 /* DMA channel 2 AHB memory addr */
#define JZ_REG_UDC_COUNT2	0x21c /* DMA channel 2 byte count */

/* Power register bit masks */
#define USB_POWER_SUSPENDM	0x01
#define USB_POWER_RESUME	0x04
#define USB_POWER_HSMODE	0x10
#define USB_POWER_HSENAB	0x20
#define USB_POWER_SOFTCONN	0x40

/* Interrupt register bit masks */
#define USB_INTR_SUSPEND	0x01
#define USB_INTR_RESUME		0x02
#define USB_INTR_RESET		0x04

#define USB_INTR_EP0		0x0001
#define USB_INTR_INEP1		0x0002
#define USB_INTR_INEP2		0x0004
#define USB_INTR_OUTEP1		0x0002

/* CSR0 bit masks */
#define USB_CSR0_OUTPKTRDY	0x01
#define USB_CSR0_INPKTRDY	0x02
#define USB_CSR0_SENTSTALL	0x04
#define USB_CSR0_DATAEND	0x08
#define USB_CSR0_SETUPEND	0x10
#define USB_CSR0_SENDSTALL	0x20
#define USB_CSR0_SVDOUTPKTRDY	0x40
#define USB_CSR0_SVDSETUPEND	0x80

/* Endpoint CSR register bits */
#define USB_INCSRH_AUTOSET	0x80
#define USB_INCSRH_ISO		0x40
#define USB_INCSRH_MODE		0x20
#define USB_INCSRH_DMAREQENAB	0x10
#define USB_INCSRH_DMAREQMODE	0x04
#define USB_INCSR_CDT		0x40
#define USB_INCSR_SENTSTALL	0x20
#define USB_INCSR_SENDSTALL	0x10
#define USB_INCSR_FF		0x08
#define USB_INCSR_UNDERRUN	0x04
#define USB_INCSR_FFNOTEMPT	0x02
#define USB_INCSR_INPKTRDY	0x01
#define USB_OUTCSRH_AUTOCLR	0x80
#define USB_OUTCSRH_ISO		0x40
#define USB_OUTCSRH_DMAREQENAB	0x20
#define USB_OUTCSRH_DNYT	0x10
#define USB_OUTCSRH_DMAREQMODE	0x08
#define USB_OUTCSR_CDT		0x80
#define USB_OUTCSR_SENTSTALL	0x40
#define USB_OUTCSR_SENDSTALL	0x20
#define USB_OUTCSR_FF		0x10
#define USB_OUTCSR_DATAERR	0x08
#define USB_OUTCSR_OVERRUN	0x04
#define USB_OUTCSR_FFFULL	0x02
#define USB_OUTCSR_OUTPKTRDY	0x01

/* Testmode register bits */
#define USB_TEST_SE0NAK		0x01
#define USB_TEST_J		0x02
#define USB_TEST_K		0x04
#define USB_TEST_PACKET		0x08

/* DMA control bits */
#define USB_CNTL_ENA		0x01
#define USB_CNTL_DIR_IN		0x02
#define USB_CNTL_MODE_1		0x04
#define USB_CNTL_INTR_EN	0x08
#define USB_CNTL_EP(n)		((n) << 4)
#define USB_CNTL_BURST_0	(0 << 9)
#define USB_CNTL_BURST_4	(1 << 9)
#define USB_CNTL_BURST_8	(2 << 9)
#define USB_CNTL_BURST_16	(3 << 9)


#ifndef DEBUG
# define DEBUG(fmt,args...) do {} while(0)
#endif
#ifndef DEBUG_EP0
# define NO_STATES
# define DEBUG_EP0(fmt,args...) do {} while(0)
#endif
#ifndef DEBUG_SETUP
# define DEBUG_SETUP(fmt,args...) do {} while(0)
#endif

static unsigned int udc_debug = 0; /* 0: normal mode, 1: test udc cable type mode */

module_param(udc_debug, int, 0);
MODULE_PARM_DESC(udc_debug, "test udc cable or power type");

static unsigned int use_dma = 0;   /* 1: use DMA, 0: use PIO */

module_param(use_dma, int, 0);
MODULE_PARM_DESC(use_dma, "DMA mode enable flag");

struct jz4740_udc *the_controller;

/*
 * Local declarations.
 */
static void jz4740_ep0_kick(struct jz4740_udc *dev, struct jz4740_ep *ep);
static void jz4740_handle_ep0(struct jz4740_udc *dev, uint32_t intr);

static void done(struct jz4740_ep *ep, struct jz4740_request *req,
		 int status);
static void pio_irq_enable(struct jz4740_ep *ep);
static void pio_irq_disable(struct jz4740_ep *ep);
static void stop_activity(struct jz4740_udc *dev,
			  struct usb_gadget_driver *driver);
static void nuke(struct jz4740_ep *ep, int status);
static void flush(struct jz4740_ep *ep);
static void udc_set_address(struct jz4740_udc *dev, unsigned char address);

/*-------------------------------------------------------------------------*/

/* inline functions of register read/write/set/clear  */

static inline uint8_t usb_readb(struct jz4740_udc *udc, size_t reg)
{
	return readb(udc->base + reg);
}

static inline uint16_t usb_readw(struct jz4740_udc *udc, size_t reg)
{
	return readw(udc->base + reg);
}

static inline uint32_t usb_readl(struct jz4740_udc *udc, size_t reg)
{
	return readl(udc->base + reg);
}

static inline void usb_writeb(struct jz4740_udc *udc, size_t reg, uint8_t val)
{
	writeb(val, udc->base + reg);
}

static inline void usb_writew(struct jz4740_udc *udc, size_t reg, uint16_t val)
{
	writew(val, udc->base + reg);
}

static inline void usb_writel(struct jz4740_udc *udc, size_t reg, uint32_t val)
{
	writel(val, udc->base + reg);
}

static inline void usb_setb(struct jz4740_udc *udc, size_t reg, uint8_t mask)
{
	usb_writeb(udc, reg, usb_readb(udc, reg) | mask);
}

static inline void usb_setw(struct jz4740_udc *udc, size_t reg, uint8_t mask)
{
	usb_writew(udc, reg, usb_readw(udc, reg) | mask);
}

static inline void usb_setl(struct jz4740_udc *udc, size_t reg, uint32_t mask)
{
	usb_writel(udc, reg, usb_readl(udc, reg) | mask);
}

static inline void usb_clearb(struct jz4740_udc *udc, size_t reg, uint8_t mask)
{
	usb_writeb(udc, reg, usb_readb(udc, reg) & ~mask);
}

static inline void usb_clearw(struct jz4740_udc *udc, size_t reg, uint16_t mask)
{
	usb_writew(udc, reg, usb_readw(udc, reg) & ~mask);
}

static inline void usb_clearl(struct jz4740_udc *udc, size_t reg, uint32_t mask)
{
	usb_writel(udc, reg, usb_readl(udc, reg) & ~mask);
}

/*-------------------------------------------------------------------------*/

static inline void jz_udc_set_index(struct jz4740_udc *udc, uint8_t index)
{
	usb_writeb(udc, JZ_REG_UDC_INDEX, index);
}

static inline void jz_udc_select_ep(struct jz4740_ep *ep)
{
	jz_udc_set_index(ep->dev, ep_index(ep));
}

static inline int write_packet(struct jz4740_ep *ep,
				   struct jz4740_request *req, int max)
{
	uint8_t *buf;
	int length, nlong, nbyte;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	buf = req->req.buf + req->req.actual;
	prefetch(buf);

	length = req->req.length - req->req.actual;
	length = min(length, max);
	req->req.actual += length;

	DEBUG("Write %d (max %d), fifo %x\n", length, max, ep->fifo);

	nlong = length >> 2;
	nbyte = length & 0x3;
	while (nlong--) {
		usb_writel(ep->dev, ep->fifo, *((uint32_t *)buf));
		buf += 4;
	}
	while (nbyte--) {
		usb_writeb(ep->dev, ep->fifo, *buf++);
	}

	return length;
}

static inline int read_packet(struct jz4740_ep *ep,
				  struct jz4740_request *req, int count)
{
	uint8_t *buf;
	int length, nlong, nbyte;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	buf = req->req.buf + req->req.actual;
	prefetchw(buf);

	length = req->req.length - req->req.actual;
	length = min(length, count);
	req->req.actual += length;

	DEBUG("Read %d, fifo %x\n", length, ep->fifo);

	nlong = length >> 2;
	nbyte = length & 0x3;
	while (nlong--) {
		*((uint32_t *)buf) = usb_readl(ep->dev, ep->fifo);
		buf += 4;
	}
	while (nbyte--) {
		*buf++ = usb_readb(ep->dev, ep->fifo);
	}

	return length;
}

/*-------------------------------------------------------------------------*/

/*
 * 	udc_disable - disable USB device controller
 */
static void udc_disable(struct jz4740_udc *dev)
{
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	udc_set_address(dev, 0);

	/* Disable interrupts */
	usb_writew(dev, JZ_REG_UDC_INTRINE, 0);
	usb_writew(dev, JZ_REG_UDC_INTROUTE, 0);
	usb_writeb(dev, JZ_REG_UDC_INTRUSBE, 0);

	/* Disable DMA */
	usb_writel(dev, JZ_REG_UDC_CNTL1, 0);
	usb_writel(dev, JZ_REG_UDC_CNTL2, 0);

	/* Disconnect from usb */
	usb_clearb(dev, JZ_REG_UDC_POWER, USB_POWER_SOFTCONN);

	/* Disable the USB PHY */
#ifdef CONFIG_SOC_JZ4740
	REG_CPM_SCR &= ~CPM_SCR_USBPHY_ENABLE;
#elif defined(CONFIG_SOC_JZ4750) || defined(CONFIG_SOC_JZ4750D)
	REG_CPM_OPCR &= ~CPM_OPCR_UDCPHY_ENABLE;
#endif

	dev->ep0state = WAIT_FOR_SETUP;
	dev->gadget.speed = USB_SPEED_UNKNOWN;

	return;
}

/*
 * 	udc_reinit - initialize software state
 */
static void udc_reinit(struct jz4740_udc *dev)
{
	int i;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	/* device/ep0 records init */
	INIT_LIST_HEAD(&dev->gadget.ep_list);
	INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);
	dev->ep0state = WAIT_FOR_SETUP;

	for (i = 0; i < UDC_MAX_ENDPOINTS; i++) {
		struct jz4740_ep *ep = &dev->ep[i];

		if (i != 0)
			list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);

		INIT_LIST_HEAD(&ep->queue);
		ep->desc = 0;
		ep->stopped = 0;
		ep->pio_irqs = 0;
	}
}

/* until it's enabled, this UDC should be completely invisible
 * to any USB host.
 */
static void udc_enable(struct jz4740_udc *dev)
{
	int i;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	/* UDC state is incorrect - Added by River */
	if (dev->state != UDC_STATE_ENABLE) {
		return;
	}

	dev->gadget.speed = USB_SPEED_UNKNOWN;

	/* Flush FIFO for each */
	for (i = 0; i < UDC_MAX_ENDPOINTS; i++) {
		struct jz4740_ep *ep = &dev->ep[i];

		jz_udc_set_index(dev, ep_index(ep));
		flush(ep);
	}

	/* Set this bit to allow the UDC entering low-power mode when
	 * there are no actions on the USB bus.
	 * UDC still works during this bit was set.
	 */
	__cpm_stop_udc();

	/* Enable the USB PHY */
#ifdef CONFIG_SOC_JZ4740
	REG_CPM_SCR |= CPM_SCR_USBPHY_ENABLE;
#elif defined(CONFIG_SOC_JZ4750) || defined(CONFIG_SOC_JZ4750D)
	REG_CPM_OPCR |= CPM_OPCR_UDCPHY_ENABLE;
#endif

	/* Disable interrupts */
/*	usb_writew(dev, JZ_REG_UDC_INTRINE, 0);
	usb_writew(dev, JZ_REG_UDC_INTROUTE, 0);
	usb_writeb(dev, JZ_REG_UDC_INTRUSBE, 0);*/

	/* Enable interrupts */
	usb_setw(dev, JZ_REG_UDC_INTRINE, USB_INTR_EP0);
	usb_setb(dev, JZ_REG_UDC_INTRUSBE, USB_INTR_RESET);
	/* Don't enable rest of the interrupts */
	/* usb_setw(dev, JZ_REG_UDC_INTRINE, USB_INTR_INEP1 | USB_INTR_INEP2);
	   usb_setw(dev, JZ_REG_UDC_INTROUTE, USB_INTR_OUTEP1); */

	/* Enable SUSPEND */
	/* usb_setb(dev, JZ_REG_UDC_POWER, USB_POWER_SUSPENDM); */

	/* Enable HS Mode */
	usb_setb(dev, JZ_REG_UDC_POWER, USB_POWER_HSENAB);

	/* Let host detect UDC:
	 * Software must write a 1 to the PMR:USB_POWER_SOFTCONN bit to turn this
	 * transistor on and pull the USBDP pin HIGH.
	 */
	usb_setb(dev, JZ_REG_UDC_POWER, USB_POWER_SOFTCONN);

	return;
}

/*-------------------------------------------------------------------------*/

/* keeping it simple:
 * - one bus driver, initted first;
 * - one function driver, initted second
 */

/*
 * Register entry point for the peripheral controller driver.
 */

int usb_gadget_register_driver(struct usb_gadget_driver *driver)
{
	struct jz4740_udc *dev = the_controller;
	int retval;

	if (!driver || !driver->bind) {
		return -EINVAL;
	}

	if (!dev) {
		return -ENODEV;
	}

	if (dev->driver) {
		return -EBUSY;
	}

	/* hook up the driver */
	dev->driver = driver;
	dev->gadget.dev.driver = &driver->driver;

	retval = driver->bind(&dev->gadget);
	if (retval) {
		DEBUG("%s: bind to driver %s --> error %d\n", dev->gadget.name,
		            driver->driver.name, retval);
		dev->driver = 0;
		return retval;
	}

	/* then enable host detection and ep0; and we're ready
	 * for set_configuration as well as eventual disconnect.
	 */
	udc_enable(dev);

	DEBUG("%s: registered gadget driver '%s'\n", dev->gadget.name,
	      driver->driver.name);

	return 0;
}

EXPORT_SYMBOL(usb_gadget_register_driver);

static void stop_activity(struct jz4740_udc *dev,
			  struct usb_gadget_driver *driver)
{
	int i;

	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	/* don't disconnect drivers more than once */
	if (dev->gadget.speed == USB_SPEED_UNKNOWN)
		driver = 0;
	dev->gadget.speed = USB_SPEED_UNKNOWN;

	/* prevent new request submissions, kill any outstanding requests  */
	for (i = 0; i < UDC_MAX_ENDPOINTS; i++) {
		struct jz4740_ep *ep = &dev->ep[i];

		ep->stopped = 1;

		jz_udc_set_index(dev, ep_index(ep));
		nuke(ep, -ESHUTDOWN);
	}

	/* report disconnect; the driver is already quiesced */
	if (driver) {
		spin_unlock(&dev->lock);
		driver->disconnect(&dev->gadget);
		spin_lock(&dev->lock);
	}

	/* re-init driver-visible data structures */
	udc_reinit(dev);
}


/*
 * Unregister entry point for the peripheral controller driver.
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
	struct jz4740_udc *dev = the_controller;
	unsigned long flags;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	if (!dev)
		return -ENODEV;
	if (!driver || driver != dev->driver)
		return -EINVAL;
	if (!driver->unbind)
		return -EBUSY;

	spin_lock_irqsave(&dev->lock, flags);
	dev->driver = 0;
	stop_activity(dev, driver);
	spin_unlock_irqrestore(&dev->lock, flags);

	driver->unbind(&dev->gadget);

	udc_disable(dev);

	DEBUG("unregistered driver '%s'\n", driver->driver.name);

	return 0;
}

EXPORT_SYMBOL(usb_gadget_unregister_driver);

/*-------------------------------------------------------------------------*/

/*
 * Starting DMA using mode 1
 */
static void kick_dma(struct jz4740_ep *ep, struct jz4740_request *req)
{
	struct jz4740_udc *dev = ep->dev;
	uint32_t count = req->req.length;
	uint32_t physaddr = virt_to_phys((void *)req->req.buf);

	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	jz_udc_select_ep(ep);

	if (ep_is_in(ep)) { /* Bulk-IN transfer using DMA channel 1 */
		ep->reg_addr = JZ_REG_UDC_ADDR1;

		dma_cache_wback_inv((unsigned long)req->req.buf, count);

		pio_irq_enable(ep);

		usb_writeb(dev, JZ_REG_UDC_INCSRH,
			   USB_INCSRH_DMAREQENAB | USB_INCSRH_AUTOSET | USB_INCSRH_DMAREQMODE);

		usb_writel(dev, JZ_REG_UDC_ADDR1, physaddr);
		usb_writel(dev, JZ_REG_UDC_COUNT1, count);
		usb_writel(dev, JZ_REG_UDC_CNTL1, USB_CNTL_ENA | USB_CNTL_DIR_IN | USB_CNTL_MODE_1 |
			   USB_CNTL_INTR_EN | USB_CNTL_BURST_16 | USB_CNTL_EP(ep_index(ep)));
	}
	else { /* Bulk-OUT transfer using DMA channel 2 */
		ep->reg_addr = JZ_REG_UDC_ADDR2;

		dma_cache_wback_inv((unsigned long)req->req.buf, count);

		pio_irq_enable(ep);

		usb_setb(dev, JZ_REG_UDC_OUTCSRH,
			 USB_OUTCSRH_DMAREQENAB | USB_OUTCSRH_AUTOCLR | USB_OUTCSRH_DMAREQMODE);

		usb_writel(dev, JZ_REG_UDC_ADDR2, physaddr);
		usb_writel(dev, JZ_REG_UDC_COUNT2, count);
		usb_writel(dev, JZ_REG_UDC_CNTL2, USB_CNTL_ENA | USB_CNTL_MODE_1 |
			   USB_CNTL_INTR_EN | USB_CNTL_BURST_16 | USB_CNTL_EP(ep_index(ep)));
	}
}

/*-------------------------------------------------------------------------*/

/** Write request to FIFO (max write == maxp size)
 *  Return:  0 = still running, 1 = completed, negative = errno
 *  NOTE: INDEX register must be set for EP
 */
static int write_fifo(struct jz4740_ep *ep, struct jz4740_request *req)
{
	struct jz4740_udc *dev = ep->dev;
	uint32_t max, csr;
	uint32_t physaddr = virt_to_phys((void *)req->req.buf);

	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
	max = le16_to_cpu(ep->desc->wMaxPacketSize);

	if (use_dma) {
		uint32_t dma_count;

		/* DMA interrupt generated due to the last packet loaded into the FIFO */

		dma_count = usb_readl(dev, ep->reg_addr) - physaddr;
		req->req.actual += dma_count;

		if (dma_count % max) {
			/* If the last packet is less than MAXP, set INPKTRDY manually */
			usb_setb(dev, ep->csr, USB_INCSR_INPKTRDY);
		}

		done(ep, req, 0);
		if (list_empty(&ep->queue)) {
			pio_irq_disable(ep);
			return 1;
		}
		else {
			/* advance the request queue */
			req = list_entry(ep->queue.next, struct jz4740_request, queue);
			kick_dma(ep, req);
			return 0;
		}
	}

	/*
	 * PIO mode handling starts here ...
	 */

	csr = usb_readb(dev, ep->csr);

	if (!(csr & USB_INCSR_FFNOTEMPT)) {
		unsigned count;
		int is_last, is_short;

		count = write_packet(ep, req, max);
		usb_setb(dev, ep->csr, USB_INCSR_INPKTRDY);

		/* last packet is usually short (or a zlp) */
		if (unlikely(count != max))
			is_last = is_short = 1;
		else {
			if (likely(req->req.length != req->req.actual)
			    || req->req.zero)
				is_last = 0;
			else
				is_last = 1;
			/* interrupt/iso maxpacket may not fill the fifo */
			is_short = unlikely(max < ep_maxpacket(ep));
		}

		DEBUG("%s: wrote %s %d bytes%s%s %d left %p\n", __FUNCTION__,
		      ep->ep.name, count,
		      is_last ? "/L" : "", is_short ? "/S" : "",
		      req->req.length - req->req.actual, req);

		/* requests complete when all IN data is in the FIFO */
		if (is_last) {
			done(ep, req, 0);
			if (list_empty(&ep->queue)) {
				pio_irq_disable(ep);
			}
			return 1;
		}
	} else {
		DEBUG("Hmm.. %d ep FIFO is not empty!\n", ep_index(ep));
	}

	return 0;
}

/** Read to request from FIFO (max read == bytes in fifo)
 *  Return:  0 = still running, 1 = completed, negative = errno
 *  NOTE: INDEX register must be set for EP
 */
static int read_fifo(struct jz4740_ep *ep, struct jz4740_request *req)
{
	struct jz4740_udc *dev = ep->dev;
	uint32_t csr;
	unsigned count, is_short;
	uint32_t physaddr = virt_to_phys((void *)req->req.buf);

	if (use_dma) {
		uint32_t dma_count;

		/* DMA interrupt generated due to a packet less than MAXP loaded into the FIFO */

		dma_count = usb_readl(dev, ep->reg_addr) - physaddr;
		req->req.actual += dma_count;

		/* Disable interrupt and DMA */
		pio_irq_disable(ep);
		usb_writel(dev, JZ_REG_UDC_CNTL2, 0);

		/* Read all bytes from this packet */
		count = usb_readw(dev, JZ_REG_UDC_OUTCOUNT);
		count = read_packet(ep, req, count);

		if (count) {
			/* If the last packet is greater than zero, clear OUTPKTRDY manually */
			usb_clearb(dev, ep->csr, USB_OUTCSR_OUTPKTRDY);
		}
		done(ep, req, 0);

		if (!list_empty(&ep->queue)) {
			/* advance the request queue */
			req = list_entry(ep->queue.next, struct jz4740_request, queue);
			kick_dma(ep, req);
		}

		return 1;
	}

	/*
	 * PIO mode handling starts here ...
	 */

	/* make sure there's a packet in the FIFO. */
	csr = usb_readb(dev, ep->csr);
	if (!(csr & USB_OUTCSR_OUTPKTRDY)) {
		DEBUG("%s: Packet NOT ready!\n", __FUNCTION__);
		return -EINVAL;
	}

	/* read all bytes from this packet */
	count = usb_readw(dev, JZ_REG_UDC_OUTCOUNT);

	is_short = (count < ep->ep.maxpacket);

	count = read_packet(ep, req, count);

	DEBUG("read %s %02x, %d bytes%s req %p %d/%d\n",
	      ep->ep.name, csr, count,
	      is_short ? "/S" : "", req, req->req.actual, req->req.length);

	/* Clear OutPktRdy */
	usb_clearb(dev, ep->csr, USB_OUTCSR_OUTPKTRDY);

	/* completion */
	if (is_short || req->req.actual == req->req.length) {
		done(ep, req, 0);

		if (list_empty(&ep->queue))
			pio_irq_disable(ep);
		return 1;
	}

	/* finished that packet.  the next one may be waiting... */
	return 0;
}

/*
 *	done - retire a request; caller blocked irqs
 *  INDEX register is preserved to keep same
 */
static void done(struct jz4740_ep *ep, struct jz4740_request *req, int status)
{
	unsigned int stopped = ep->stopped;
	unsigned long flags;
	uint32_t index;

	DEBUG("%s, %p\n", __FUNCTION__, ep);
	list_del_init(&req->queue);

	if (likely(req->req.status == -EINPROGRESS))
		req->req.status = status;
	else
		status = req->req.status;

	if (status && status != -ESHUTDOWN)
		DEBUG("complete %s req %p stat %d len %u/%u\n",
		      ep->ep.name, &req->req, status,
		      req->req.actual, req->req.length);

	/* don't modify queue heads during completion callback */
	ep->stopped = 1;
	/* Read current index (completion may modify it) */
	spin_lock_irqsave(&ep->dev->lock, flags);
	index = usb_readb(ep->dev, JZ_REG_UDC_INDEX);

	req->req.complete(&ep->ep, &req->req);

	/* Restore index */
	jz_udc_set_index(ep->dev, index);
	spin_unlock_irqrestore(&ep->dev->lock, flags);
	ep->stopped = stopped;
}

/** Enable EP interrupt */
static void pio_irq_enable(struct jz4740_ep *ep)
{
	uint8_t index = ep_index(ep);
	struct jz4740_udc *dev = ep->dev;
	DEBUG("%s: EP%d %s\n", __FUNCTION__, ep_index(ep), ep_is_in(ep) ? "IN": "OUT");

	if (ep_is_in(ep)) {
		switch (index) {
		case 1:
		case 2:
			usb_setw(dev, JZ_REG_UDC_INTRINE, BIT(index));
			dev->in_mask |= BIT(index);
			break;
		default:
			DEBUG("Unknown endpoint: %d\n", index);
			break;
		}
	}
	else {
		switch (index) {
		case 1:
			usb_setw(dev, JZ_REG_UDC_INTROUTE, BIT(index));
			dev->out_mask |= BIT(index);
			break;
		default:
			DEBUG("Unknown endpoint: %d\n", index);
			break;
		}
	}
}

/** Disable EP interrupt */
static void pio_irq_disable(struct jz4740_ep *ep)
{
	uint8_t index = ep_index(ep);
	struct jz4740_udc *dev = ep->dev;

	DEBUG("%s: EP%d %s\n", __FUNCTION__, ep_index(ep), ep_is_in(ep) ? "IN": "OUT");

	if (ep_is_in(ep)) {
		switch (ep_index(ep)) {
		case 1:
		case 2:
			usb_clearw(ep->dev, JZ_REG_UDC_INTRINE, BIT(index));
			dev->in_mask &= ~BIT(index);
			break;
		default:
			DEBUG("Unknown endpoint: %d\n", index);
			break;
		}
	}
	else {
		switch (ep_index(ep)) {
		case 1:
			usb_clearw(ep->dev, JZ_REG_UDC_INTROUTE, BIT(index));
			dev->out_mask &= ~BIT(index);
			break;
		default:
			DEBUG("Unknown endpoint: %d\n", index);
			break;
	    }
	}
}

/*
 * 	nuke - dequeue ALL requests
 */
static void nuke(struct jz4740_ep *ep, int status)
{
	struct jz4740_request *req;

	DEBUG("%s, %p\n", __FUNCTION__, ep);

	/* Flush FIFO */
	flush(ep);

	/* called with irqs blocked */
	while (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct jz4740_request, queue);
		done(ep, req, status);
	}

	/* Disable IRQ if EP is enabled (has descriptor) */
	if (ep->desc)
		pio_irq_disable(ep);
}

/** Flush EP FIFO
 * NOTE: INDEX register must be set before this call
 */
static void flush(struct jz4740_ep *ep)
{
	DEBUG("%s: %s\n", __FUNCTION__, ep->ep.name);

	switch (ep->type) {
	case ep_bulk_in:
	case ep_interrupt:
		usb_setb(ep->dev, ep->csr, USB_INCSR_FF);
		break;
	case ep_bulk_out:
		usb_setb(ep->dev, ep->csr, USB_OUTCSR_FF);
		break;
	case ep_control:
		break;
	}
}

/**
 * jz4740_in_epn - handle IN interrupt
 */
static void jz4740_in_epn(struct jz4740_udc *dev, uint32_t ep_idx, uint32_t intr)
{
	uint32_t csr;
	struct jz4740_ep *ep = &dev->ep[ep_idx + 1];
	struct jz4740_request *req;
	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	jz_udc_set_index(dev, ep_index(ep));

	csr = usb_readb(dev, ep->csr);
	DEBUG("%s: %d, csr %x\n", __FUNCTION__, ep_idx, csr);

	if (csr & USB_INCSR_SENTSTALL) {
		DEBUG("USB_INCSR_SENTSTALL\n");
		usb_clearb(dev, ep->csr, USB_INCSR_SENTSTALL);
		return;
	}

	if (!ep->desc) {
		DEBUG("%s: NO EP DESC\n", __FUNCTION__);
		return;
	}

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct jz4740_request, queue);

	DEBUG("req: %p\n", req);

	if (!req)
		return;

	write_fifo(ep, req);
}

/*
 * Bulk OUT (recv)
 */
static void jz4740_out_epn(struct jz4740_udc *dev, uint32_t ep_idx, uint32_t intr)
{
	struct jz4740_ep *ep = &dev->ep[ep_idx];
	struct jz4740_request *req;

	DEBUG("%s: %d\n", __FUNCTION__, ep_idx);

	jz_udc_set_index(dev, ep_index(ep));
	if (ep->desc) {
		uint32_t csr;

		if (use_dma) {
			/* DMA starts here ... */
			if (list_empty(&ep->queue))
				req = 0;
			else
				req = list_entry(ep->queue.next, struct jz4740_request, queue);

			if (req)
				read_fifo(ep, req);
			return;
		}

		/*
		 * PIO mode starts here ...
		 */

		while ((csr = usb_readb(dev, ep->csr)) &
		       (USB_OUTCSR_OUTPKTRDY | USB_OUTCSR_SENTSTALL)) {
			DEBUG("%s: %x\n", __FUNCTION__, csr);

			if (csr & USB_OUTCSR_SENTSTALL) {
				DEBUG("%s: stall sent, flush fifo\n",
				      __FUNCTION__);
				/* usb_set(USB_OUT_CSR1_FIFO_FLUSH, ep->csr1); */
				flush(ep);
			} else if (csr & USB_OUTCSR_OUTPKTRDY) {
				if (list_empty(&ep->queue))
					req = 0;
				else
					req =
						list_entry(ep->queue.next,
							   struct jz4740_request,
							   queue);

				if (!req) {
					DEBUG("%s: NULL REQ %d\n",
					      __FUNCTION__, ep_idx);
					break;
				} else {
					read_fifo(ep, req);
				}
			}
		}
	} else {
		/* Throw packet away.. */
		DEBUG("%s: ep %p ep_indx %d No descriptor?!?\n", __FUNCTION__, ep, ep_idx);
		flush(ep);
	}
}

/** Halt specific EP
 *  Return 0 if success
 *  NOTE: Sets INDEX register to EP !
 */
static int jz4740_set_halt(struct usb_ep *_ep, int value)
{
	struct jz4740_udc *dev;
	struct jz4740_ep *ep;
	unsigned long flags;

	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (unlikely(!_ep || (!ep->desc && ep->type != ep_control))) {
		DEBUG("%s, bad ep\n", __FUNCTION__);
		return -EINVAL;
	}

	dev = ep->dev;

	spin_lock_irqsave(&dev->lock, flags);

	jz_udc_select_ep(ep);

	DEBUG("%s, ep %d, val %d\n", __FUNCTION__, ep_index(ep), value);

	if (ep_index(ep) == 0) {
		/* EP0 */
		usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SENDSTALL);
	} else if (ep_is_in(ep)) {
		uint32_t csr = usb_readb(dev, ep->csr);
		if (value && ((csr & USB_INCSR_FFNOTEMPT)
			      || !list_empty(&ep->queue))) {
			/*
			 * Attempts to halt IN endpoints will fail (returning -EAGAIN)
			 * if any transfer requests are still queued, or if the controller
			 * FIFO still holds bytes that the host hasnt collected.
			 */
			spin_unlock_irqrestore(&dev->lock, flags);
			DEBUG
			    ("Attempt to halt IN endpoint failed (returning -EAGAIN) %d %d\n",
			     (csr & USB_INCSR_FFNOTEMPT),
			     !list_empty(&ep->queue));
			return -EAGAIN;
		}
		flush(ep);
		if (value) {
			usb_setb(dev, ep->csr, USB_INCSR_SENDSTALL);
		}
		else {
			usb_clearb(dev, ep->csr, USB_INCSR_SENDSTALL);
			usb_setb(dev, ep->csr, USB_INCSR_CDT);
		}
	} else {

		flush(ep);
		if (value) {
			usb_setb(dev, ep->csr, USB_OUTCSR_SENDSTALL);
		}
		else {
			usb_clearb(dev, ep->csr, USB_OUTCSR_SENDSTALL);
			usb_setb(dev, ep->csr, USB_OUTCSR_CDT);
		}
	}

	if (value) {
		ep->stopped = 1;
	} else {
		ep->stopped = 0;
	}

	spin_unlock_irqrestore(&dev->lock, flags);

	DEBUG("%s %s halted\n", _ep->name, value == 0 ? "NOT" : "IS");

	return 0;
}


static int jz4740_ep_enable(struct usb_ep *_ep,
			    const struct usb_endpoint_descriptor *desc)
{
	struct jz4740_ep *ep;
	struct jz4740_udc *dev;
	unsigned long flags;
	uint32_t max, csrh = 0;

	DEBUG("%s: trying to enable %s\n", __FUNCTION__, _ep->name);

	if (!_ep || !desc)
		return -EINVAL;

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (ep->desc || ep->type == ep_control
	    || desc->bDescriptorType != USB_DT_ENDPOINT
	    || ep->bEndpointAddress != desc->bEndpointAddress) {
		DEBUG("%s, bad ep or descriptor\n", __FUNCTION__);
		return -EINVAL;
	}

	/* xfer types must match, except that interrupt ~= bulk */
	if (ep->bmAttributes != desc->bmAttributes
	    && ep->bmAttributes != USB_ENDPOINT_XFER_BULK
	    && desc->bmAttributes != USB_ENDPOINT_XFER_INT) {
		DEBUG("%s, %s type mismatch\n", __FUNCTION__, _ep->name);
		return -EINVAL;
	}

	dev = ep->dev;
	if (!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN) {
		DEBUG("%s, bogus device state\n", __FUNCTION__);
		return -ESHUTDOWN;
	}

	max = le16_to_cpu(desc->wMaxPacketSize);

	spin_lock_irqsave(&ep->dev->lock, flags);

	/* Configure the endpoint */
	jz_udc_set_index(dev, desc->bEndpointAddress & 0x0F);
	if (ep_is_in(ep)) {
		usb_writew(dev, JZ_REG_UDC_INMAXP, max);
		switch (desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
		case USB_ENDPOINT_XFER_BULK:
		case USB_ENDPOINT_XFER_INT:
			csrh &= ~USB_INCSRH_ISO;
			break;
		case USB_ENDPOINT_XFER_ISOC:
			csrh |= USB_INCSRH_ISO;
			break;
		}
		usb_writeb(dev, JZ_REG_UDC_INCSRH, csrh);
	}
	else {
		usb_writew(dev, JZ_REG_UDC_OUTMAXP, max);
		switch (desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
		case USB_ENDPOINT_XFER_BULK:
			 csrh &= ~USB_OUTCSRH_ISO;
			break;
		case USB_ENDPOINT_XFER_INT:
			csrh &= ~USB_OUTCSRH_ISO;
			csrh |= USB_OUTCSRH_DNYT;
			break;
		case USB_ENDPOINT_XFER_ISOC:
			csrh |= USB_OUTCSRH_ISO;
			break;
		}
		usb_writeb(dev, JZ_REG_UDC_OUTCSRH, csrh);
	}


	ep->stopped = 0;
	ep->desc = desc;
	ep->pio_irqs = 0;
	ep->ep.maxpacket = max;

	spin_unlock_irqrestore(&ep->dev->lock, flags);

	/* Reset halt state (does flush) */
	jz4740_set_halt(_ep, 0);

	DEBUG("%s: enabled %s\n", __FUNCTION__, _ep->name);

	return 0;
}

/** Disable EP
 *  NOTE: Sets INDEX register
 */
static int jz4740_ep_disable(struct usb_ep *_ep)
{
	struct jz4740_ep *ep;
	unsigned long flags;

	DEBUG("%s, %p\n", __FUNCTION__, _ep);

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (!_ep || !ep->desc) {
		DEBUG("%s, %s not enabled\n", __FUNCTION__,
		      _ep ? ep->ep.name : NULL);
		return -EINVAL;
	}

	spin_lock_irqsave(&ep->dev->lock, flags);

	jz_udc_select_ep(ep);

	/* Nuke all pending requests (does flush) */
	nuke(ep, -ESHUTDOWN);

	/* Disable ep IRQ */
	pio_irq_disable(ep);

	ep->desc = 0;
	ep->stopped = 1;

	spin_unlock_irqrestore(&ep->dev->lock, flags);

	DEBUG("%s: disabled %s\n", __FUNCTION__, _ep->name);
	return 0;
}

static struct usb_request *jz4740_alloc_request(struct usb_ep *ep, gfp_t gfp_flags)
{
	struct jz4740_request *req;

	DEBUG("%s, %p\n", __FUNCTION__, ep);

	req = kzalloc(sizeof(*req), gfp_flags);
	if (!req)
		return 0;

	INIT_LIST_HEAD(&req->queue);

	return &req->req;
}

static void jz4740_free_request(struct usb_ep *ep, struct usb_request *_req)
{
	struct jz4740_request *req;

	DEBUG("%s, %p\n", __FUNCTION__, ep);

	req = container_of(_req, struct jz4740_request, req);
	WARN_ON(!list_empty(&req->queue));
	kfree(req);
}

/*--------------------------------------------------------------------*/

/** Queue one request
 *  Kickstart transfer if needed
 *  NOTE: Sets INDEX register
 */
static int jz4740_queue(struct usb_ep *_ep, struct usb_request *_req,
			gfp_t gfp_flags)
{
	struct jz4740_request *req;
	struct jz4740_ep *ep;
	struct jz4740_udc *dev;
	unsigned long flags;

	DEBUG("%s, %p\n", __FUNCTION__, _ep);

	req = container_of(_req, struct jz4740_request, req);
	if (unlikely
	    (!_req || !_req->complete || !_req->buf
	     || !list_empty(&req->queue))) {
		DEBUG("%s, bad params\n", __FUNCTION__);
		return -EINVAL;
	}

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (unlikely(!_ep || (!ep->desc && ep->type != ep_control))) {
		DEBUG("%s, bad ep\n", __FUNCTION__);
		return -EINVAL;
	}

	dev = ep->dev;
	if (unlikely(!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN)) {
		DEBUG("%s, bogus device state %p\n", __FUNCTION__, dev->driver);
		return -ESHUTDOWN;
	}

	DEBUG("%s queue req %p, len %d buf %p\n", _ep->name, _req, _req->length,
	      _req->buf);

	spin_lock_irqsave(&dev->lock, flags);

	_req->status = -EINPROGRESS;
	_req->actual = 0;

	/* kickstart this i/o queue? */
	DEBUG("Add to %d Q %d %d\n", ep_index(ep), list_empty(&ep->queue),
	      ep->stopped);
	if (list_empty(&ep->queue) && likely(!ep->stopped)) {
		uint32_t csr;

		if (unlikely(ep_index(ep) == 0)) {
			/* EP0 */
			list_add_tail(&req->queue, &ep->queue);
			jz4740_ep0_kick(dev, ep);
			req = 0;
		} else if (use_dma) {
			/* DMA */
			kick_dma(ep, req);
		}
		/* PIO */
		else if (ep_is_in(ep)) {
			/* EP1 & EP2 */
			jz_udc_set_index(dev, ep_index(ep));
			csr = usb_readb(dev, ep->csr);
			pio_irq_enable(ep);
			if (!(csr & USB_INCSR_FFNOTEMPT)) {
				if (write_fifo(ep, req) == 1)
					req = 0;
			}
		} else {
			/* EP1 */
			jz_udc_set_index(dev, ep_index(ep));
			csr = usb_readb(dev, ep->csr);
			pio_irq_enable(ep);
			if (csr & USB_OUTCSR_OUTPKTRDY) {
				if (read_fifo(ep, req) == 1)
					req = 0;
			}
		}
	}

	/* pio or dma irq handler advances the queue. */
	if (likely(req != 0))
		list_add_tail(&req->queue, &ep->queue);

	spin_unlock_irqrestore(&dev->lock, flags);

	return 0;
}

/* dequeue JUST ONE request */
static int jz4740_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct jz4740_ep *ep;
	struct jz4740_request *req;
	unsigned long flags;

	DEBUG("%s, %p\n", __FUNCTION__, _ep);

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (!_ep || ep->type == ep_control)
		return -EINVAL;

	spin_lock_irqsave(&ep->dev->lock, flags);

	/* make sure it's actually queued on this endpoint */
	list_for_each_entry(req, &ep->queue, queue) {
		if (&req->req == _req)
			break;
	}
	if (&req->req != _req) {
		spin_unlock_irqrestore(&ep->dev->lock, flags);
		return -EINVAL;
	}
	done(ep, req, -ECONNRESET);

	spin_unlock_irqrestore(&ep->dev->lock, flags);
	return 0;
}

/** Return bytes in EP FIFO
 *  NOTE: Sets INDEX register to EP
 */
static int jz4740_fifo_status(struct usb_ep *_ep)
{
	uint32_t csr;
	int count = 0;
	struct jz4740_ep *ep;
	unsigned long flags;

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (!_ep) {
		DEBUG("%s, bad ep\n", __FUNCTION__);
		return -ENODEV;
	}

	DEBUG("%s, %d\n", __FUNCTION__, ep_index(ep));

	/* LPD can't report unclaimed bytes from IN fifos */
	if (ep_is_in(ep))
		return -EOPNOTSUPP;

	spin_lock_irqsave(&ep->dev->lock, flags);
	jz_udc_set_index(ep->dev, ep_index(ep));

	csr = usb_readb(ep->dev, ep->csr);
	if (ep->dev->gadget.speed != USB_SPEED_UNKNOWN ||
	    csr & 0x1) {
		count = usb_readw(ep->dev, JZ_REG_UDC_OUTCOUNT);
	}

	spin_unlock_irqrestore(&ep->dev->lock, flags);

	return count;
}

/** Flush EP FIFO
 *  NOTE: Sets INDEX register to EP
 */
static void jz4740_fifo_flush(struct usb_ep *_ep)
{
	struct jz4740_ep *ep;
	unsigned long flags;

	DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	ep = container_of(_ep, struct jz4740_ep, ep);
	if (unlikely(!_ep || (!ep->desc && ep->type == ep_control))) {
		DEBUG("%s, bad ep\n", __FUNCTION__);
		return;
	}

	spin_lock_irqsave(&ep->dev->lock, flags);

	jz_udc_set_index(ep->dev, ep_index(ep));
	flush(ep);

	spin_unlock_irqrestore(&ep->dev->lock, flags);
}

/****************************************************************/
/* End Point 0 related functions                                */
/****************************************************************/

/* return:  0 = still running, 1 = completed, negative = errno */
static int write_fifo_ep0(struct jz4740_ep *ep, struct jz4740_request *req)
{
	uint32_t max;
	unsigned count;
	int is_last;

    DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
	max = ep_maxpacket(ep);

	count = write_packet(ep, req, max);

	/* last packet is usually short (or a zlp) */
	if (unlikely(count != max))
		is_last = 1;
	else {
		if (likely(req->req.length != req->req.actual) || req->req.zero)
			is_last = 0;
		else
			is_last = 1;
	}

	DEBUG_EP0("%s: wrote %s %d bytes%s %d left %p\n", __FUNCTION__,
		  ep->ep.name, count,
		  is_last ? "/L" : "", req->req.length - req->req.actual, req);

	/* requests complete when all IN data is in the FIFO */
	if (is_last) {
		done(ep, req, 0);
		return 1;
	}

	return 0;
}

static inline int jz4740_fifo_read(struct jz4740_ep *ep,
				       unsigned char *cp, int max)
{
	int bytes;
	int count = usb_readw(ep->dev, JZ_REG_UDC_OUTCOUNT);

	if (count > max)
		count = max;
	bytes = count;
	while (count--)
		*cp++ = usb_readb(ep->dev, ep->fifo);

	return bytes;
}

static inline void jz4740_fifo_write(struct jz4740_ep *ep,
					 unsigned char *cp, int count)
{
	DEBUG("fifo_write: %d %d\n", ep_index(ep), count);
	while (count--)
		usb_writeb(ep->dev, ep->fifo, *cp++);
}

static int read_fifo_ep0(struct jz4740_ep *ep, struct jz4740_request *req)
{
	struct jz4740_udc *dev = ep->dev;
	uint32_t csr;
	uint8_t *buf;
	unsigned bufferspace, count, is_short;

	DEBUG_EP0("%s\n", __FUNCTION__);

	csr = usb_readb(dev, JZ_REG_UDC_CSR0);
	if (!(csr & USB_CSR0_OUTPKTRDY))
		return 0;

	buf = req->req.buf + req->req.actual;
	prefetchw(buf);
	bufferspace = req->req.length - req->req.actual;

	/* read all bytes from this packet */
	if (likely(csr & USB_CSR0_OUTPKTRDY)) {
		count = usb_readw(dev, JZ_REG_UDC_OUTCOUNT);
		req->req.actual += min(count, bufferspace);
	} else			/* zlp */
		count = 0;

	is_short = (count < ep->ep.maxpacket);
	DEBUG_EP0("read %s %02x, %d bytes%s req %p %d/%d\n",
		  ep->ep.name, csr, count,
		  is_short ? "/S" : "", req, req->req.actual, req->req.length);

	while (likely(count-- != 0)) {
		uint8_t byte = (uint8_t)usb_readl(dev, ep->fifo);

		if (unlikely(bufferspace == 0)) {
			/* this happens when the driver's buffer
			 * is smaller than what the host sent.
			 * discard the extra data.
			 */
			if (req->req.status != -EOVERFLOW)
				DEBUG_EP0("%s overflow %d\n", ep->ep.name,
					  count);
			req->req.status = -EOVERFLOW;
		} else {
			*buf++ = byte;
			bufferspace--;
		}
	}

	/* completion */
	if (is_short || req->req.actual == req->req.length) {
		done(ep, req, 0);
		return 1;
	}

	/* finished that packet.  the next one may be waiting... */
	return 0;
}

/**
 * udc_set_address - set the USB address for this device
 * @address:
 *
 * Called from control endpoint function after it decodes a set address setup packet.
 */
static void udc_set_address(struct jz4740_udc *dev, unsigned char address)
{
	DEBUG_EP0("%s: %d\n", __FUNCTION__, address);

	dev->usb_address = address;
	usb_writeb(dev, JZ_REG_UDC_FADDR, address);
}

/*
 * DATA_STATE_RECV (USB_CSR0_OUTPKTRDY)
 *      - if error
 *              set USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND | USB_CSR0_SENDSTALL bits
 *      - else
 *              set USB_CSR0_SVDOUTPKTRDY bit
 				if last set USB_CSR0_DATAEND bit
 */
static void jz4740_ep0_out(struct jz4740_udc *dev, uint32_t csr, int kickstart)
{
	struct jz4740_request *req;
	struct jz4740_ep *ep = &dev->ep[0];
	int ret;

	DEBUG_EP0("%s: %x\n", __FUNCTION__, csr);

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct jz4740_request, queue);

	if (req) {
		if (req->req.length == 0) {
			DEBUG_EP0("ZERO LENGTH OUT!\n");
			usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));
			dev->ep0state = WAIT_FOR_SETUP;
			return;
		} else if (kickstart) {
			usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY));
			return;
		}
		ret = read_fifo_ep0(ep, req);
		if (ret) {
			/* Done! */
			DEBUG_EP0("%s: finished, waiting for status\n",
				  __FUNCTION__);
			usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));
			dev->ep0state = WAIT_FOR_SETUP;
		} else {
			/* Not done yet.. */
			DEBUG_EP0("%s: not finished\n", __FUNCTION__);
			usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
		}
	} else {
		DEBUG_EP0("NO REQ??!\n");
	}
}

/*
 * DATA_STATE_XMIT
 */
static int jz4740_ep0_in(struct jz4740_udc *dev, uint32_t csr)
{
	struct jz4740_request *req;
	struct jz4740_ep *ep = &dev->ep[0];
	int ret, need_zlp = 0;

	DEBUG_EP0("%s: %x\n", __FUNCTION__, csr);

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct jz4740_request, queue);

	if (!req) {
		DEBUG_EP0("%s: NULL REQ\n", __FUNCTION__);
		return 0;
	}

	if (req->req.length == 0) {
		usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_INPKTRDY | USB_CSR0_DATAEND));
		dev->ep0state = WAIT_FOR_SETUP;
		return 1;
	}

	if (req->req.length - req->req.actual == EP0_MAXPACKETSIZE) {
		/* Next write will end with the packet size, */
		/* so we need zero-length-packet */
		need_zlp = 1;
	}

	ret = write_fifo_ep0(ep, req);

	if (ret == 1 && !need_zlp) {
		/* Last packet */
		DEBUG_EP0("%s: finished, waiting for status\n", __FUNCTION__);

		usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_INPKTRDY | USB_CSR0_DATAEND));
		dev->ep0state = WAIT_FOR_SETUP;
	} else {
		DEBUG_EP0("%s: not finished\n", __FUNCTION__);
		usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_INPKTRDY);
	}

	if (need_zlp) {
		DEBUG_EP0("%s: Need ZLP!\n", __FUNCTION__);
		usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_INPKTRDY);
		dev->ep0state = DATA_STATE_NEED_ZLP;
	}

	return 1;
}

static int jz4740_handle_get_status(struct jz4740_udc *dev,
				    struct usb_ctrlrequest *ctrl)
{
	struct jz4740_ep *ep0 = &dev->ep[0];
	struct jz4740_ep *qep;
	int reqtype = (ctrl->bRequestType & USB_RECIP_MASK);
	uint16_t val = 0;

    DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

	if (reqtype == USB_RECIP_INTERFACE) {
		/* This is not supported.
		 * And according to the USB spec, this one does nothing..
		 * Just return 0
		 */
		DEBUG_SETUP("GET_STATUS: USB_RECIP_INTERFACE\n");
	} else if (reqtype == USB_RECIP_DEVICE) {
		DEBUG_SETUP("GET_STATUS: USB_RECIP_DEVICE\n");
		val |= (1 << 0);	/* Self powered */
		/*val |= (1<<1); *//* Remote wakeup */
	} else if (reqtype == USB_RECIP_ENDPOINT) {
		int ep_num = (ctrl->wIndex & ~USB_DIR_IN);

		DEBUG_SETUP
			("GET_STATUS: USB_RECIP_ENDPOINT (%d), ctrl->wLength = %d\n",
			 ep_num, ctrl->wLength);

		if (ctrl->wLength > 2 || ep_num > 3)
			return -EOPNOTSUPP;

		qep = &dev->ep[ep_num];
		if (ep_is_in(qep) != ((ctrl->wIndex & USB_DIR_IN) ? 1 : 0)
		    && ep_index(qep) != 0) {
			return -EOPNOTSUPP;
		}

		jz_udc_set_index(dev, ep_index(qep));

		/* Return status on next IN token */
		switch (qep->type) {
		case ep_control:
			val =
			    (usb_readb(dev, qep->csr) & USB_CSR0_SENDSTALL) ==
			    USB_CSR0_SENDSTALL;
			break;
		case ep_bulk_in:
		case ep_interrupt:
			val =
			    (usb_readb(dev, qep->csr) & USB_INCSR_SENDSTALL) ==
			    USB_INCSR_SENDSTALL;
			break;
		case ep_bulk_out:
			val =
			    (usb_readb(dev, qep->csr) & USB_OUTCSR_SENDSTALL) ==
			    USB_OUTCSR_SENDSTALL;
			break;
		}

		/* Back to EP0 index */
		jz_udc_set_index(dev, 0);

		DEBUG_SETUP("GET_STATUS, ep: %d (%x), val = %d\n", ep_num,
			    ctrl->wIndex, val);
	} else {
		DEBUG_SETUP("Unknown REQ TYPE: %d\n", reqtype);
		return -EOPNOTSUPP;
	}

	/* Clear "out packet ready" */
	usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
	/* Put status to FIFO */
	jz4740_fifo_write(ep0, (uint8_t *)&val, sizeof(val));
	/* Issue "In packet ready" */
	usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_INPKTRDY | USB_CSR0_DATAEND));

	return 0;
}

/*
 * WAIT_FOR_SETUP (OUTPKTRDY)
 *      - read data packet from EP0 FIFO
 *      - decode command
 *      - if error
 *              set USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND | USB_CSR0_SENDSTALL bits
 *      - else
 *              set USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND bits
 */
static void jz4740_ep0_setup(struct jz4740_udc *dev, uint32_t csr)
{
	struct jz4740_ep *ep = &dev->ep[0];
	struct usb_ctrlrequest ctrl;
	int i;

	DEBUG_SETUP("%s: %x\n", __FUNCTION__, csr);

	/* Nuke all previous transfers */
	nuke(ep, -EPROTO);

	/* read control req from fifo (8 bytes) */
	jz4740_fifo_read(ep, (unsigned char *)&ctrl, 8);

	DEBUG_SETUP("SETUP %02x.%02x v%04x i%04x l%04x\n",
		    ctrl.bRequestType, ctrl.bRequest,
		    ctrl.wValue, ctrl.wIndex, ctrl.wLength);

	/* Set direction of EP0 */
	if (likely(ctrl.bRequestType & USB_DIR_IN)) {
		ep->bEndpointAddress |= USB_DIR_IN;
	} else {
		ep->bEndpointAddress &= ~USB_DIR_IN;
	}

	/* Handle some SETUP packets ourselves */
	switch (ctrl.bRequest) {
	case USB_REQ_SET_ADDRESS:
		if (ctrl.bRequestType != (USB_TYPE_STANDARD | USB_RECIP_DEVICE))
			break;

		DEBUG_SETUP("USB_REQ_SET_ADDRESS (%d)\n", ctrl.wValue);
		udc_set_address(dev, ctrl.wValue);
		usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));
		return;

	case USB_REQ_SET_CONFIGURATION:
		if (ctrl.bRequestType != (USB_TYPE_STANDARD | USB_RECIP_DEVICE))
			break;

		DEBUG_SETUP("USB_REQ_SET_CONFIGURATION (%d)\n", ctrl.wValue);
/*		usb_setb(JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));*/

		/* Enable RESUME and SUSPEND interrupts */
		usb_setb(dev, JZ_REG_UDC_INTRUSBE, (USB_INTR_RESUME | USB_INTR_SUSPEND));
		break;

	case USB_REQ_SET_INTERFACE:
		if (ctrl.bRequestType != (USB_TYPE_STANDARD | USB_RECIP_DEVICE))
			break;

		DEBUG_SETUP("USB_REQ_SET_INTERFACE (%d)\n", ctrl.wValue);
/*		usb_setb(JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));*/
		break;

	case USB_REQ_GET_STATUS:
		if (jz4740_handle_get_status(dev, &ctrl) == 0)
			return;

	case USB_REQ_CLEAR_FEATURE:
	case USB_REQ_SET_FEATURE:
		if (ctrl.bRequestType == USB_RECIP_ENDPOINT) {
			struct jz4740_ep *qep;
			int ep_num = (ctrl.wIndex & 0x0f);

			/* Support only HALT feature */
			if (ctrl.wValue != 0 || ctrl.wLength != 0
			    || ep_num > 3 || ep_num < 1)
				break;

			qep = &dev->ep[ep_num];
			spin_unlock(&dev->lock);
			if (ctrl.bRequest == USB_REQ_SET_FEATURE) {
				DEBUG_SETUP("SET_FEATURE (%d)\n",
					    ep_num);
				jz4740_set_halt(&qep->ep, 1);
			} else {
				DEBUG_SETUP("CLR_FEATURE (%d)\n",
					    ep_num);
				jz4740_set_halt(&qep->ep, 0);
			}
			spin_lock(&dev->lock);

			jz_udc_set_index(dev, 0);

			/* Reply with a ZLP on next IN token */
			usb_setb(dev, JZ_REG_UDC_CSR0,
				 (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND));
			return;
		}
		break;

	default:
		break;
	}

	/* gadget drivers see class/vendor specific requests,
	 * {SET,GET}_{INTERFACE,DESCRIPTOR,CONFIGURATION},
	 * and more.
	 */
	if (dev->driver) {
		/* device-2-host (IN) or no data setup command, process immediately */
		spin_unlock(&dev->lock);

		i = dev->driver->setup(&dev->gadget, &ctrl);
		spin_lock(&dev->lock);

		if (unlikely(i < 0)) {
			/* setup processing failed, force stall */
			DEBUG_SETUP
			    ("  --> ERROR: gadget setup FAILED (stalling), setup returned %d\n",
			     i);
			jz_udc_set_index(dev, 0);
			usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND | USB_CSR0_SENDSTALL));

			/* ep->stopped = 1; */
			dev->ep0state = WAIT_FOR_SETUP;
		}
		else {
			DEBUG_SETUP("gadget driver setup ok (%d)\n", ctrl.wLength);
/*			if (!ctrl.wLength) {
				usb_setb(JZ_REG_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
			}*/
		}
	}
}

/*
 * DATA_STATE_NEED_ZLP
 */
static void jz4740_ep0_in_zlp(struct jz4740_udc *dev, uint32_t csr)
{
	DEBUG_EP0("%s: %x\n", __FUNCTION__, csr);

	usb_setb(dev, JZ_REG_UDC_CSR0, (USB_CSR0_INPKTRDY | USB_CSR0_DATAEND));
	dev->ep0state = WAIT_FOR_SETUP;
}

/*
 * handle ep0 interrupt
 */
static void jz4740_handle_ep0(struct jz4740_udc *dev, uint32_t intr)
{
	struct jz4740_ep *ep = &dev->ep[0];
	uint32_t csr;

    DEBUG("%s:%s[%d]\n", __FILE__, __func__, __LINE__);
	/* Set index 0 */
	jz_udc_set_index(dev, 0);
	csr = usb_readb(dev, JZ_REG_UDC_CSR0);

	DEBUG_EP0("%s: csr = %x  state = \n", __FUNCTION__, csr);//, state_names[dev->ep0state]);

	/*
	 * if SENT_STALL is set
	 *      - clear the SENT_STALL bit
	 */
	if (csr & USB_CSR0_SENTSTALL) {
		DEBUG_EP0("%s: USB_CSR0_SENTSTALL is set: %x\n", __FUNCTION__, csr);
		usb_clearb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SENDSTALL | USB_CSR0_SENTSTALL);
		nuke(ep, -ECONNABORTED);
		dev->ep0state = WAIT_FOR_SETUP;
		return;
	}

	/*
	 * if a transfer is in progress && INPKTRDY and OUTPKTRDY are clear
	 *      - fill EP0 FIFO
	 *      - if last packet
	 *      -       set IN_PKT_RDY | DATA_END
	 *      - else
	 *              set IN_PKT_RDY
	 */
	if (!(csr & (USB_CSR0_INPKTRDY | USB_CSR0_OUTPKTRDY))) {
		DEBUG_EP0("%s: INPKTRDY and OUTPKTRDY are clear\n",
			  __FUNCTION__);

		switch (dev->ep0state) {
		case DATA_STATE_XMIT:
			DEBUG_EP0("continue with DATA_STATE_XMIT\n");
			jz4740_ep0_in(dev, csr);
			return;
		case DATA_STATE_NEED_ZLP:
			DEBUG_EP0("continue with DATA_STATE_NEED_ZLP\n");
			jz4740_ep0_in_zlp(dev, csr);
			return;
		default:
			/* Stall? */
//			DEBUG_EP0("Odd state!! state = %s\n",
//				  state_names[dev->ep0state]);
			dev->ep0state = WAIT_FOR_SETUP;
			/* nuke(ep, 0); */
			/* usb_setb(ep->csr, USB_CSR0_SENDSTALL); */
//			break;
			return;
		}
	}

	/*
	 * if SETUPEND is set
	 *      - abort the last transfer
	 *      - set SERVICED_SETUP_END_BIT
	 */
	if (csr & USB_CSR0_SETUPEND) {
		DEBUG_EP0("%s: USB_CSR0_SETUPEND is set: %x\n", __FUNCTION__, csr);

		usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SVDSETUPEND);
		nuke(ep, 0);
		dev->ep0state = WAIT_FOR_SETUP;
	}

	/*
	 * if USB_CSR0_OUTPKTRDY is set
	 *      - read data packet from EP0 FIFO
	 *      - decode command
	 *      - if error
	 *              set SVDOUTPKTRDY | DATAEND | SENDSTALL bits
	 *      - else
	 *              set SVDOUTPKTRDY | DATAEND bits
	 */
	if (csr & USB_CSR0_OUTPKTRDY) {

		DEBUG_EP0("%s: EP0_OUT_PKT_RDY is set: %x\n", __FUNCTION__,
			  csr);

		switch (dev->ep0state) {
		case WAIT_FOR_SETUP:
			DEBUG_EP0("WAIT_FOR_SETUP\n");
			jz4740_ep0_setup(dev, csr);
			break;

		case DATA_STATE_RECV:
			DEBUG_EP0("DATA_STATE_RECV\n");
			jz4740_ep0_out(dev, csr, 0);
			break;

		default:
			/* send stall? */
			DEBUG_EP0("strange state!! 2. send stall? state = %d\n",
				  dev->ep0state);
			break;
		}
	}
}

static void jz4740_ep0_kick(struct jz4740_udc *dev, struct jz4740_ep *ep)
{
	uint32_t csr;

	jz_udc_set_index(dev, 0);

	DEBUG_EP0("%s: %x\n", __FUNCTION__, csr);

	/* Clear "out packet ready" */

	if (ep_is_in(ep)) {
		usb_setb(dev, JZ_REG_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
		csr = usb_readb(dev, JZ_REG_UDC_CSR0);
		dev->ep0state = DATA_STATE_XMIT;
		jz4740_ep0_in(dev, csr);
	} else {
		csr = usb_readb(dev, JZ_REG_UDC_CSR0);
		dev->ep0state = DATA_STATE_RECV;
		jz4740_ep0_out(dev, csr, 1);
	}
}

/** Handle USB RESET interrupt
 */
static void jz4740_reset_irq(struct jz4740_udc *dev)
{
	dev->gadget.speed = (usb_readb(dev, JZ_REG_UDC_POWER) & USB_POWER_HSMODE) ?
		USB_SPEED_HIGH : USB_SPEED_FULL;

	DEBUG_SETUP("%s: address = %d, speed = %s\n", __FUNCTION__, dev->usb_address,
		    (dev->gadget.speed == USB_SPEED_HIGH) ? "HIGH":"FULL" );
}

/*
 *	jz4740 usb device interrupt handler.
 */
static irqreturn_t jz4740_udc_irq(int irq, void *_dev)
{
	struct jz4740_udc *dev = _dev;
	uint8_t index;

	uint32_t intr_usb = usb_readb(dev, JZ_REG_UDC_INTRUSB) & 0x7; /* mask SOF */
	uint32_t intr_in  = usb_readw(dev, JZ_REG_UDC_INTRIN);
	uint32_t intr_out = usb_readw(dev, JZ_REG_UDC_INTROUT);
	uint32_t intr_dma = usb_readb(dev, JZ_REG_UDC_INTR);

	if (!intr_usb && !intr_in && !intr_out && !intr_dma)
		return IRQ_HANDLED;


	DEBUG("intr_out=%x intr_in=%x intr_usb=%x\n",
	      intr_out, intr_in, intr_usb);

	spin_lock(&dev->lock);
	index = usb_readb(dev, JZ_REG_UDC_INDEX);

	/* Check for resume from suspend mode */
	if ((intr_usb & USB_INTR_RESUME) &&
	    (usb_readb(dev, JZ_REG_UDC_INTRUSBE) & USB_INTR_RESUME)) {
		DEBUG("USB resume\n");
		dev->driver->resume(&dev->gadget); /* We have suspend(), so we must have resume() too. */
	}

	/* Check for system interrupts */
	if (intr_usb & USB_INTR_RESET) {
		DEBUG("USB reset\n");
		jz4740_reset_irq(dev);
	}

	/* Check for endpoint 0 interrupt */
	if (intr_in & USB_INTR_EP0) {
		DEBUG("USB_INTR_EP0 (control)\n");
		jz4740_handle_ep0(dev, intr_in);
	}

	/* Check for Bulk-IN DMA interrupt */
	if (intr_dma & 0x1) {
		int ep_num;
		struct jz4740_ep *ep;
		ep_num = (usb_readl(dev, JZ_REG_UDC_CNTL1) >> 4) & 0xf;
		ep = &dev->ep[ep_num + 1];
		jz_udc_set_index(dev, ep_num);
		usb_setb(dev, ep->csr, USB_INCSR_INPKTRDY);
/*		jz4740_in_epn(dev, ep_num, intr_in);*/
	}

	/* Check for Bulk-OUT DMA interrupt */
	if (intr_dma & 0x2) {
		int ep_num;
		ep_num = (usb_readl(dev, JZ_REG_UDC_CNTL2) >> 4) & 0xf;
		jz4740_out_epn(dev, ep_num, intr_out);
	}

	/* Check for each configured endpoint interrupt */
	if (intr_in & USB_INTR_INEP1) {
		DEBUG("USB_INTR_INEP1\n");
		jz4740_in_epn(dev, 1, intr_in);
	}

	if (intr_in & USB_INTR_INEP2) {
		DEBUG("USB_INTR_INEP2\n");
		jz4740_in_epn(dev, 2, intr_in);
	}

	if (intr_out & USB_INTR_OUTEP1) {
		DEBUG("USB_INTR_OUTEP1\n");
		jz4740_out_epn(dev, 1, intr_out);
	}

	/* Check for suspend mode */
	if ((intr_usb & USB_INTR_SUSPEND) &&
	    (usb_readb(dev, JZ_REG_UDC_INTRUSBE) & USB_INTR_SUSPEND)) {
		DEBUG("USB suspend\n");
		dev->driver->suspend(&dev->gadget);
		/* Host unloaded from us, can do something, such as flushing
		 the NAND block cache etc. */
	}

    jz_udc_set_index(dev, index);

	spin_unlock(&dev->lock);

	return IRQ_HANDLED;
}



/*-------------------------------------------------------------------------*/

/* Common functions - Added by River */
static struct jz4740_udc udc_dev;

static inline struct jz4740_udc *gadget_to_udc(struct usb_gadget *gadget)
{
	return container_of(gadget, struct jz4740_udc, gadget);
}
/* End added */

static int jz4740_udc_get_frame(struct usb_gadget *_gadget)
{
	DEBUG("%s, %p\n", __FUNCTION__, _gadget);
	return usb_readw(gadget_to_udc(_gadget), JZ_REG_UDC_FRAME);
}

static int jz4740_udc_wakeup(struct usb_gadget *_gadget)
{
	/* host may not have enabled remote wakeup */
	/*if ((UDCCS0 & UDCCS0_DRWF) == 0)
	   return -EHOSTUNREACH;
	   udc_set_mask_UDCCR(UDCCR_RSM); */
	return -ENOTSUPP;
}

static int jz4740_udc_pullup(struct usb_gadget *_gadget, int on)
{
	struct jz4740_udc *udc = gadget_to_udc(_gadget);

	unsigned long flags;

	local_irq_save(flags);

	if (on) {
		udc->state = UDC_STATE_ENABLE;
		udc_enable(udc);
	} else {
		udc->state = UDC_STATE_DISABLE;
		udc_disable(udc);
	}

	local_irq_restore(flags);

	return 0;
}

static const struct usb_gadget_ops jz4740_udc_ops = {
	.get_frame = jz4740_udc_get_frame,
	.wakeup = jz4740_udc_wakeup,
	.pullup = jz4740_udc_pullup,
	/* current versions must always be self-powered */
};

static struct usb_ep_ops jz4740_ep_ops = {
	.enable		= jz4740_ep_enable,
	.disable	= jz4740_ep_disable,

	.alloc_request	= jz4740_alloc_request,
	.free_request	= jz4740_free_request,

	.queue		= jz4740_queue,
	.dequeue	= jz4740_dequeue,

	.set_halt	= jz4740_set_halt,
	.fifo_status	= jz4740_fifo_status,
	.fifo_flush	= jz4740_fifo_flush,
};


/*-------------------------------------------------------------------------*/

static struct jz4740_udc udc_dev = {
	.usb_address = 0,
	.gadget = {
		.ops = &jz4740_udc_ops,
		.ep0 = &udc_dev.ep[0].ep,
		.name = "jz-udc",
		.dev = {
			.init_name = "gadget",
		},
	},

	/* control endpoint */
	.ep[0] = {
		.ep = {
			.name = "ep0",
			.ops = &jz4740_ep_ops,
			.maxpacket = EP0_MAXPACKETSIZE,
		},
		.dev = &udc_dev,

		.bEndpointAddress = 0,
		.bmAttributes = 0,

		.type = ep_control,
		.fifo = JZ_REG_UDC_EP_FIFO(0),
		.csr = JZ_REG_UDC_CSR0,
	},

	/* bulk out endpoint */
	.ep[1] = {
		.ep = {
			.name = "ep1out-bulk",
			.ops = &jz4740_ep_ops,
			.maxpacket = EPBULK_MAXPACKETSIZE,
		},
		.dev = &udc_dev,

		.bEndpointAddress = 1,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,

		.type = ep_bulk_out,
		.fifo = JZ_REG_UDC_EP_FIFO(1),
		.csr = JZ_REG_UDC_OUTCSR,
	},

	/* bulk in endpoint */
	.ep[2] = {
		.ep = {
			.name = "ep1in-bulk",
			.ops = &jz4740_ep_ops,
			.maxpacket = EPBULK_MAXPACKETSIZE,
		},
		.dev = &udc_dev,

		.bEndpointAddress = 1 | USB_DIR_IN,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,

		.type = ep_bulk_in,
		.fifo = JZ_REG_UDC_EP_FIFO(1),
		.csr = JZ_REG_UDC_INCSR,
	},

	/* interrupt in endpoint */
	.ep[3] = {
		.ep = {
			.name = "ep2in-int",
			.ops = &jz4740_ep_ops,
			.maxpacket = EPINTR_MAXPACKETSIZE,
		},
		.dev = &udc_dev,

		.bEndpointAddress = 2 | USB_DIR_IN,
		.bmAttributes = USB_ENDPOINT_XFER_INT,

		.type = ep_interrupt,
		.fifo = JZ_REG_UDC_EP_FIFO(2),
		.csr = JZ_REG_UDC_INCSR,
	},
};

static void gadget_release(struct device *_dev)
{
}


static int jz4740_udc_probe(struct platform_device *pdev)
{
	struct jz4740_udc *dev = &udc_dev;
	int ret;

	spin_lock_init(&dev->lock);
	the_controller = dev;

	dev->dev = &pdev->dev;
	dev_set_name(&dev->gadget.dev, "gadget");
	dev->gadget.dev.parent = &pdev->dev;
	dev->gadget.dev.dma_mask = pdev->dev.dma_mask;
	dev->gadget.dev.release = gadget_release;

	ret = device_register(&dev->gadget.dev);
        if (ret)
		return ret;

	platform_set_drvdata(pdev, dev);

	dev->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!dev->mem) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get mmio memory resource\n");
		goto err_device_unregister;
	}

	dev->mem = request_mem_region(dev->mem->start, resource_size(dev->mem), pdev->name);

	if (!dev->mem) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		goto err_device_unregister;
	}

	dev->base = ioremap(dev->mem->start, resource_size(dev->mem));

	if (!dev->base) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to ioremap mmio memory\n");
		goto err_release_mem_region;
	}

	dev->irq = platform_get_irq(pdev, 0);

	ret = request_irq(dev->irq, jz4740_udc_irq, IRQF_DISABLED,
                            pdev->name, dev);
        if (ret) {
                dev_err(&pdev->dev, "Failed to request irq: %d\n", ret);
                goto err_iounmap;
	}

	udc_disable(dev);
	udc_reinit(dev);

	return 0;

err_iounmap:
	iounmap(dev->base);
err_release_mem_region:
	release_mem_region(dev->mem->start, resource_size(dev->mem));
err_device_unregister:
	device_unregister(&dev->gadget.dev);
	platform_set_drvdata(pdev, NULL);

	the_controller = 0;

        return ret;
}

static int jz4740_udc_remove(struct platform_device *pdev)
{
	struct jz4740_udc *dev = platform_get_drvdata(pdev);

	if (dev->driver)
		return -EBUSY;

	udc_disable(dev);
#ifdef	UDC_PROC_FILE
	remove_proc_entry(proc_node_name, NULL);
#endif

	free_irq(dev->irq, dev);
	iounmap(dev->base);
	release_mem_region(dev->mem->start, resource_size(dev->mem));

	platform_set_drvdata(pdev, NULL);
	device_unregister(&dev->gadget.dev);
	the_controller = NULL;

	return 0;
}

static struct platform_driver udc_driver = {
	.probe		= jz4740_udc_probe,
	.remove		= jz4740_udc_remove,
	.driver		= {
		.name	= "jz-udc",
		.owner	= THIS_MODULE,
	},
};

/*-------------------------------------------------------------------------*/

static int __init udc_init (void)
{
	return platform_driver_register(&udc_driver);
}

static void __exit udc_exit (void)
{
	platform_driver_unregister(&udc_driver);
}

module_init(udc_init);
module_exit(udc_exit);

MODULE_DESCRIPTION("JZ4740 USB Device Controller");
MODULE_AUTHOR("Wei Jianli <jlwei@ingenic.cn>");
MODULE_LICENSE("GPL");
