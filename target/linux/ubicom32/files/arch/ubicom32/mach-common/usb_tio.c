/*
 * arch/ubicom32/mach-common/usb_tio.c
 *  Linux side Ubicom USB TIO driver
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
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <asm/devtree.h>
#include "usb_tio.h"

#ifdef CONFIG_SMP
static DEFINE_SPINLOCK(tio_lock);
#define USB_TIO_LOCK(lock, flag) spin_lock_irqsave(lock, flag)
#define USB_TIO_UNLOCK(lock, flag) spin_unlock_irqrestore(lock, flag)
#define USB_TIO_LOCK_ISLOCKED(lock) spin_try_lock(lock)
#else
#define USB_TIO_LOCK(lock, flag) local_irq_save(flag)
#define USB_TIO_UNLOCK(lock, flag) local_irq_restore(flag)
#endif

spinlock_t usb_tio_lock;

/*
 * usb_tio_set_hrt_interrupt()
 */
static inline void usb_tio_set_hrt_interrupt(void)
{
	ubicom32_set_interrupt(usb_node->dn.sendirq);
}

static inline void usb_tio_wait_hrt(void)
{
	while (unlikely(usb_node->pdesc));
}

#if defined(USB_TIO_DEBUG)
static void usb_tio_request_verify_magic(volatile struct usb_tio_request *req)
{
	BUG_ON(req->magic != USB_TIO_REQUEST_MAGIC2);
}

static void usb_tio_request_clear_magic(volatile struct usb_tio_request *req)
{
	req->magic = 0;
}
#endif

static void usb_tio_request_set_magic(volatile struct usb_tio_request *req)
{
	req->magic = USB_TIO_REQUEST_MAGIC1;
}

/*
 * usb_tio_commit_request()
 */
static inline void usb_tio_commit_request(volatile struct usb_tio_request *request)
{
	wmb();
	usb_node->pdesc = request;

	/*
	 * next thing to do is alway checking if (usb_node->pdesc == NULL)
	 * to see if the request is done, so add a mb() here
	 */
	mb();
	usb_tio_set_hrt_interrupt();
}

/*
 * usb_tio_read_u16()
 *	Synchronously read 16 bits.
 */
u8_t usb_tio_read_u16(u32_t address, u16_t *data)
{
	volatile struct usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	/*
	 * Fill in the request.
	 */
	tio_req->address = address;
	tio_req->cmd = USB_TIO_READ16_SYNC;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);
	usb_tio_commit_request(tio_req);

	/*
	 * Wait for the result to show up.
	 */
	usb_tio_wait_hrt();
	USB_TIO_REQUEST_VERIFY_MAGIC(tio_req);
	*data = (u16_t)tio_req->data;
	USB_TIO_REQUEST_CLEAR_MAGIC(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_read_u8()
 *	Synchronously read 16 bits.
 */
u8_t usb_tio_read_u8(u32_t address, u8_t *data)
{
	volatile struct usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	/*
	 * Fill in the request.
	 */
	tio_req->address = address;
	tio_req->cmd = USB_TIO_READ8_SYNC;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);

	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);

	/*
	 * Wait for the result to show up.
	 */
	usb_tio_wait_hrt();
	USB_TIO_REQUEST_VERIFY_MAGIC(tio_req);
	*data = (u8_t)tio_req->data;
	USB_TIO_REQUEST_CLEAR_MAGIC(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_write_u16()
 *	Asynchronously  write 16 bits.
 */
u8_t usb_tio_write_u16(u32_t address, u16_t data)
{
	volatile struct usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	/*
	 * Wait for any previous write or pending read to complete.
	 */
	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	tio_req->address = address;
	tio_req->data = data;
	tio_req->cmd = USB_TIO_WRITE16_ASYNC;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);

	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_write_u8()
 *	Asynchronously  write 8 bits.
 */
u8_t usb_tio_write_u8(u32_t address, u8_t data)
{
	volatile struct usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	/*
	 * Wait for any previous write or pending read to complete.
	 */
	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	tio_req->address = address;
	tio_req->data = data;
	tio_req->cmd = USB_TIO_WRITE8_ASYNC;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);

	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_read_fifo()
 *	Synchronously read FIFO.
 */
u8_t usb_tio_read_fifo(u32_t address, u32_t buffer, u32_t bytes)
{
	volatile struct usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	/*
	 * Fill in the request.
	 */
	tio_req->address = address;
	tio_req->cmd = USB_TIO_READ_FIFO_SYNC;
	tio_req->buffer = buffer;
	tio_req->transfer_length = bytes;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);

	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);

	 /*
	 * Wait for the result to show up.
	 */
	usb_tio_wait_hrt();
	USB_TIO_REQUEST_VERIFY_MAGIC(tio_req);
	USB_TIO_REQUEST_CLEAR_MAGIC(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_write_fifo()
 *	Synchronously  write 32 bits.
 */
u8_t usb_tio_write_fifo(u32_t address, u32_t buffer, u32_t bytes)
{
	volatile struct	usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	tio_req->address = address;
	tio_req->buffer = buffer;
	tio_req->cmd = USB_TIO_WRITE_FIFO_SYNC;
	tio_req->transfer_length = bytes;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);
	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);

	/*
	 * Wait for the result to show up.
	 */
	usb_tio_wait_hrt();
	USB_TIO_REQUEST_VERIFY_MAGIC(tio_req);
	USB_TIO_REQUEST_CLEAR_MAGIC(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_write_fifo_async()
 *	Asynchronously write 32 bits.
 */
u8_t usb_tio_write_fifo_async(u32_t address, u32_t buffer, u32_t bytes)
{
	volatile struct	usb_tio_request *tio_req = &usb_node->request;
	unsigned long flag;

	USB_TIO_LOCK(&tio_lock, flag);
	usb_tio_wait_hrt();

	tio_req->address = address;

	/*
	 * Is it necessary to make a local copy of the buffer? Any chance the URB is aborted before TIO finished the FIFO write?
	 */
	tio_req->buffer = buffer;
	tio_req->cmd = USB_TIO_WRITE_FIFO_SYNC;
	tio_req->transfer_length = bytes;
	USB_TIO_REQUEST_SET_MAGIC(tio_req);
	/*
	 * commit the request
	 */
	usb_tio_commit_request(tio_req);
	USB_TIO_UNLOCK(&tio_lock, flag);
	return USB_TIO_OK;
}

/*
 * usb_tio_read_int_status()
 * 	read and clear the interrupt status registers
 */
void usb_tio_read_int_status(u8_t *int_usb, u16_t *int_tx, u16_t *int_rx)
{

	/*
	 * clear the interrupt must be syncronized with the TIO thread to prevent the racing condiiton
	 * that TIO thread try to set it at same time
	 */
	asm volatile (
	"1:	bset (%0), (%0), #0	\n\t" \
	"	jmpne.f 1b		\n\t" \
		:
		: "a" (&usb_node->usb_vp_control)
		: "memory", "cc"
	);

	*int_usb = usb_node->usb_vp_hw_int_usb;
	*int_tx  = cpu_to_le16(usb_node->usb_vp_hw_int_tx);
	*int_rx  = cpu_to_le16(usb_node->usb_vp_hw_int_rx);

	//printk(KERN_INFO "int read %x, %x, %x\n", *int_usb, *int_tx, *int_rx);

	/*
	 * The interrupt status register is read-clean, so clear it now
	 */
	usb_node->usb_vp_hw_int_usb = 0;
	usb_node->usb_vp_hw_int_tx = 0;
	usb_node->usb_vp_hw_int_rx = 0;

	/*
	 * release the lock bit
	 */
	usb_node->usb_vp_control &= 0xfffe;
}
