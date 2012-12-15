/*****************************************************************************
 **   FILE NAME       : ifxhcd_queue.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the functions to manage Queue Heads and Queue
 **                     Transfer Descriptors.
 **   FUNCTIONS       :
 **   COMPILER        : gcc
 **   REFERENCE       : Synopsys DWC-OTG Driver 2.7
 **   COPYRIGHT       :  Copyright (c) 2010
 **                      LANTIQ DEUTSCHLAND GMBH,
 **                      Am Campeon 3, 85579 Neubiberg, Germany
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 **  Version Control Section  **
 **   $Author$
 **   $Date$
 **   $Revisions$
 **   $Log$       Revision history
 *****************************************************************************/

/*
 * This file contains code fragments from Synopsys HS OTG Linux Software Driver.
 * For this code the following notice is applicable:
 *
 * ==========================================================================
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/*!
 \file ifxhcd_queue.c
 \ingroup IFXUSB_DRIVER_V3
  \brief This file contains the functions to manage Queue Heads and Queue
  Transfer Descriptors.
*/
#include <linux/version.h>
#include "ifxusb_version.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/string.h>

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"
#include "ifxhcd.h"

#ifdef __EPQD_DESTROY_TIMEOUT__
	#define epqh_self_destroy_timeout 300
	static void eqph_destroy_func(unsigned long _ptr)
	{
		ifxhcd_epqh_t *epqh=(ifxhcd_epqh_t *)_ptr;
		if(epqh)
		{
			if(epqh->sysep)
			{
				epqh->sysep->hcpriv=NULL;
			}
			ifxhcd_epqh_free (epqh);
		}
	}
#endif

/*!
  \brief This function allocates and initializes a EPQH.

  \param _ifxhcd The HCD state structure for the USB Host controller.
  \param[in] _urb Holds the information about the device/endpoint that we need
  to initialize the EPQH.

  \return Returns pointer to the newly allocated EPQH, or NULL on error.
 */
static ifxhcd_epqh_t *ifxhcd_epqh_create (ifxhcd_hcd_t *_ifxhcd, struct urb *_urb)
{
	ifxhcd_epqh_t *epqh;

	hprt0_data_t   hprt0;
	struct usb_host_endpoint *sysep = ifxhcd_urb_to_endpoint(_urb);

	/* Allocate memory */
//	epqh=(ifxhcd_epqh_t *) kmalloc (sizeof(ifxhcd_epqh_t), GFP_KERNEL);
	epqh=(ifxhcd_epqh_t *) kmalloc (sizeof(ifxhcd_epqh_t), GFP_ATOMIC);

	if(epqh == NULL)
		return NULL;

	memset (epqh, 0, sizeof (ifxhcd_epqh_t));

	epqh->sysep=sysep;

	epqh->devno=_urb->dev->devnum;

	epqh->ifxhcd=_ifxhcd;
	epqh->phase=EPQH_IDLE;

	/* Initialize EPQH */
	switch (usb_pipetype(_urb->pipe))
	{
		case PIPE_CONTROL    : epqh->ep_type = IFXUSB_EP_TYPE_CTRL; break;
		case PIPE_BULK       : epqh->ep_type = IFXUSB_EP_TYPE_BULK; break;
		case PIPE_ISOCHRONOUS: epqh->ep_type = IFXUSB_EP_TYPE_ISOC; break;
		case PIPE_INTERRUPT  : epqh->ep_type = IFXUSB_EP_TYPE_INTR; break;
	}

	usb_settoggle(_urb->dev, usb_pipeendpoint (_urb->pipe), !usb_pipein(_urb->pipe), IFXUSB_HC_PID_DATA0);
	epqh->mps = usb_maxpacket(_urb->dev, _urb->pipe, !(usb_pipein(_urb->pipe)));

	INIT_LIST_HEAD(&epqh->urbd_list);
	#ifdef __STRICT_ORDER__
		INIT_LIST_HEAD(&epqh->release_list);
	#endif
	INIT_LIST_HEAD(&epqh->ql);
	INIT_LIST_HEAD(&epqh->ql_all);
	INIT_URBD_LIST(epqh);

	epqh->hc = NULL;

	/* FS/LS Enpoint on HS Hub
	 * NOT virtual root hub */
	epqh->need_split = 0;
	hprt0.d32 = ifxusb_read_hprt0 (&_ifxhcd->core_if);
	if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED &&
	    ((_urb->dev->speed == USB_SPEED_LOW) ||
	     (_urb->dev->speed == USB_SPEED_FULL)) &&
	     (_urb->dev->tt) && (_urb->dev->tt->hub) && (_urb->dev->tt->hub->devnum != 1))
	{
		IFX_DEBUGPL(DBG_HCD, "QH init: EP %d: TT found at hub addr %d, for port %d\n",
		       usb_pipeendpoint(_urb->pipe), _urb->dev->tt->hub->devnum,
		       _urb->dev->ttport);
		epqh->need_split = 1;
	}

	if (epqh->ep_type == IFXUSB_EP_TYPE_INTR ||
	    epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
	{
		/* Compute scheduling parameters once and save them. */
		epqh->interval    = _urb->interval;
		if(epqh->need_split)
			epqh->interval *= 8;
	}

	#ifdef __EN_ISOC__
		if (epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			_ifxhcd->isoc_ep_count++;
	#endif

	epqh->period_counter=0;

	#ifdef __EPQD_DESTROY_TIMEOUT__
		/* Start a timer for this transfer. */
		init_timer(&epqh->destroy_timer);
		epqh->destroy_timer.function = eqph_destroy_func;
		epqh->destroy_timer.data = (unsigned long)(epqh);
	#endif

	#ifdef __DEBUG__
		IFX_DEBUGPL(DBG_HCD , "IFXUSB HCD EPQH Initialized\n");
		IFX_DEBUGPL(DBG_HCDV, "IFXUSB HCD EPQH  - epqh = %p\n", epqh);
		IFX_DEBUGPL(DBG_HCDV, "IFXUSB HCD EPQH  - Device Address = %d EP %d, %s\n",
			    _urb->dev->devnum,
			    usb_pipeendpoint(_urb->pipe),
			    usb_pipein(_urb->pipe) == USB_DIR_IN ? "IN" : "OUT");
		IFX_DEBUGPL(DBG_HCDV, "IFXUSB HCD EPQH  - Speed = %s\n",
			    ({ char *speed; switch (_urb->dev->speed) {
			    case USB_SPEED_LOW: speed  = "low" ; break;
			    case USB_SPEED_FULL: speed = "full"; break;
			    case USB_SPEED_HIGH: speed = "high"; break;
			    default: speed = "?";	break;
			    }; speed;}));
		IFX_DEBUGPL(DBG_HCDV, "IFXUSB HCD EPQH  - Type = %s\n",
			({
				char *type; switch (epqh->ep_type)
				{
				    case IFXUSB_EP_TYPE_ISOC: type = "isochronous"; break;
				    case IFXUSB_EP_TYPE_INTR: type = "interrupt"  ; break;
				    case IFXUSB_EP_TYPE_CTRL: type = "control"    ; break;
				    case IFXUSB_EP_TYPE_BULK: type = "bulk"       ; break;
				    default: type = "?";	break;
				};
				type;
			}));
		if (epqh->ep_type == IFXUSB_EP_TYPE_INTR)
			IFX_DEBUGPL(DBG_HCDV, "IFXUSB HCD EPQH - interval = %d\n", epqh->interval);
	#endif

	LOCK_EPQH_LIST_ALL(_ifxhcd);
	list_add_tail(&epqh->ql_all, &_ifxhcd->epqh_list_all);
	UNLOCK_EPQH_LIST_ALL(_ifxhcd);

	LOCK_EPQH_LIST(_ifxhcd);
	switch (epqh->ep_type)
	{
		case IFXUSB_EP_TYPE_CTRL:
		case IFXUSB_EP_TYPE_BULK:

			list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_np);
			break;
		case IFXUSB_EP_TYPE_INTR:
			list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_intr);
			break;
		#ifdef __EN_ISOC__
			case IFXUSB_EP_TYPE_ISOC:
				list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_isoc);

				break;
		#endif
	}
	UNLOCK_EPQH_LIST(_ifxhcd);
	return epqh;
}






/*!
  \brief Free the EPQH.  EPQH should already be removed from a list.
  URBD list should already be empty if called from URB Dequeue.

  \param[in] _epqh The EPQH to free.
 */
void ifxhcd_epqh_free (ifxhcd_epqh_t *_epqh)
{
	unsigned long     flags;
	if(!_epqh)
		return;

	if(_epqh->sysep) _epqh->sysep->hcpriv=NULL;
	_epqh->sysep=NULL;

	local_irq_save (flags);
	if (!list_empty(&_epqh->urbd_list))
		IFX_WARN("%s() invalid epqh state\n",__func__);
	else
	{
		LOCK_EPQH_LIST_ALL(_epqh->ifxhcd);
		if (!list_empty(&_epqh->ql_all))
			list_del_init (&_epqh->ql_all);
		UNLOCK_EPQH_LIST_ALL(_epqh->ifxhcd);

		LOCK_EPQH_LIST(_epqh->ifxhcd);
		if (!list_empty(&_epqh->ql))
			list_del_init (&_epqh->ql);
		UNLOCK_EPQH_LIST(_epqh->ifxhcd);

		#ifdef __EPQD_DESTROY_TIMEOUT__
			del_timer(&_epqh->destroy_timer);
		#endif
		kfree (_epqh);
	}
	local_irq_restore (flags);
}


void ifxhcd_epqh_idle(ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	local_irq_save(flags);
	LOCK_URBD_LIST(_epqh);
	if (list_empty(&_epqh->urbd_list))
	{
		if(_epqh->ep_type == IFXUSB_EP_TYPE_ISOC || _epqh->ep_type == IFXUSB_EP_TYPE_INTR)
			_epqh->phase=EPQH_STDBY;
		else
		{
			_epqh->phase=EPQH_IDLE;
			#ifdef __EPQD_DESTROY_TIMEOUT__
				del_timer(&_epqh->destroy_timer);
				_epqh->destroy_timer.expires = jiffies + (HZ*epqh_self_destroy_timeout);
				add_timer(&_epqh->destroy_timer );
			#endif
		}
	}
	else
	{
		_epqh->phase=EPQH_READY;
		#ifdef __EPQD_DESTROY_TIMEOUT__
			del_timer(&_epqh->destroy_timer);
		#endif
	}
	UNLOCK_URBD_LIST(_epqh);
	local_irq_restore(flags);
}


void ifxhcd_epqh_idle_periodic(ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	if(_epqh->ep_type != IFXUSB_EP_TYPE_ISOC && _epqh->ep_type != IFXUSB_EP_TYPE_INTR && _epqh->phase!=EPQH_STDBY)
		return;

	local_irq_save(flags);
	LOCK_URBD_LIST(_epqh);
	if (!list_empty(&_epqh->urbd_list))
		IFX_WARN("%s() invalid epqh state(not empty)\n",__func__);

	_epqh->phase=EPQH_IDLE;

	#ifdef __EPQD_DESTROY_TIMEOUT__
		del_timer(&_epqh->destroy_timer);
		_epqh->destroy_timer.expires = jiffies + (HZ*epqh_self_destroy_timeout);
		add_timer(&_epqh->destroy_timer );
	#endif

	#ifdef __EN_ISOC__
		if (_epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			_epqh->ifxhcd->isoc_ep_count--;
	#endif
	UNLOCK_URBD_LIST(_epqh);
	local_irq_restore(flags);
}


ifxhcd_epqh_t *ifxhcd_urbd_create (ifxhcd_hcd_t *_ifxhcd,struct urb *_urb)
{
	ifxhcd_urbd_t            *urbd;
	struct usb_host_endpoint *sysep;
	ifxhcd_epqh_t            *epqh=NULL;
	unsigned long             flags;

	local_irq_save(flags);

	sysep = ifxhcd_urb_to_endpoint(_urb);

	LOCK_EPQH_LIST_ALL(_ifxhcd);
	epqh = sysep_to_epqh(_ifxhcd, sysep);

	if (!epqh)
	{
		sysep->hcpriv = NULL;
		epqh = ifxhcd_epqh_create (_ifxhcd, _urb);
	}
	UNLOCK_EPQH_LIST_ALL(_ifxhcd);

	if (!epqh)
	{
		IFX_ERROR("EPQH Error alloc\n");
		local_irq_restore (flags);
		return (ifxhcd_epqh_t *)NULL;
	}
	if(epqh->phase==EPQH_DISABLING)
	{
		IFX_ERROR("EPQH Error alloc while disabling\n");
		local_irq_restore (flags);
		return (ifxhcd_epqh_t *)NULL;
	}
	sysep->hcpriv = epqh;

	if(_urb->hcpriv)
	{
		IFX_WARN("%s() Previous urb->hcpriv exist %p\n",__func__,_urb->hcpriv);
	#if 1
		local_irq_restore (flags);
		return (ifxhcd_epqh_t *)NULL;
	#else
		urbd = _urb->hcpriv;
		if(urbd->epqh!=epqh)
			IFX_WARN("%s() Previous urb->hcpriv exist %p and epqh not the same %p %p\n",__func__,_urb->hcpriv,urbd->epqh,epqh);
	#endif
	}
	else
	{
		urbd =  (ifxhcd_urbd_t *) kmalloc (sizeof(ifxhcd_urbd_t), GFP_ATOMIC);
		if (!urbd)
		{
			local_irq_restore (flags);
			return (ifxhcd_epqh_t *)NULL;
		}
		memset (urbd, 0, sizeof (ifxhcd_urbd_t));
		INIT_LIST_HEAD(&urbd->ql);
	}

	_urb->hcpriv = urbd;
	urbd->urb   = _urb;
	urbd->epqh  =  epqh;
	urbd->status= -EINPROGRESS;

	urbd->is_in=usb_pipein(_urb->pipe) ? 1 : 0;
#define URB_NO_SETUP_DMA_MAP         0
	#ifdef __EN_ISOC__
	if(epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
	{
		if(_urb->transfer_flags && URB_NO_TRANSFER_DMA_MAP)
			urbd->xfer_buff = (uint8_t *) (KSEG1ADDR((uint32_t *)_urb->transfer_dma));
		else
			urbd->xfer_buff = (uint8_t *) _urb->transfer_buffer;
	}
	else
	#endif
	{
		urbd->xfer_len=_urb->transfer_buffer_length;
		if(urbd->xfer_len>0)
		{
			if(_urb->transfer_flags && URB_NO_TRANSFER_DMA_MAP)
				urbd->xfer_buff = (uint8_t *) (KSEG1ADDR((uint32_t *)_urb->transfer_dma));
			else
				urbd->xfer_buff = (uint8_t *) _urb->transfer_buffer;
		}
	}

	#if 1 // cache write-back, so DMA engine can get correct content. Precaution
		if(urbd->xfer_len)
			dma_cache_wback_inv((unsigned long)urbd->xfer_buff, urbd->xfer_len);
	#endif

	if(epqh->ep_type == IFXUSB_EP_TYPE_CTRL)
	{
		if(_urb->transfer_flags && URB_NO_SETUP_DMA_MAP)
			urbd->setup_buff = (uint8_t *) (KSEG1ADDR((uint32_t *)_urb->setup_dma));
		else
			urbd->setup_buff = (uint8_t *) _urb->setup_packet;
		#if 1 // cache write-back, so DMA engine can get correct content. Precaution
			dma_cache_wback_inv((unsigned long)urbd->setup_buff, 16);
		#endif
	}

	LOCK_URBD_LIST(epqh);
	if (!list_empty(&urbd->ql))
		list_del_init(&urbd->ql);
	list_add_tail(&urbd->ql, &epqh->urbd_list);
	epqh->urbd_count++;
	UNLOCK_URBD_LIST(epqh);

	local_irq_restore (flags);
	return epqh;
}



ifxhcd_epqh_t * sysep_to_epqh(ifxhcd_hcd_t *_ifxhcd, struct usb_host_endpoint *_sysep)
{
	ifxhcd_epqh_t *epqh;

	LOCK_EPQH_LIST_ALL(_ifxhcd);
	list_for_each_entry( epqh, &_ifxhcd->epqh_list_all, ql_all)
	{
		if(epqh->sysep==_sysep)
		{
			UNLOCK_EPQH_LIST_ALL(_ifxhcd);
			return epqh;
		}
	}
	UNLOCK_EPQH_LIST_ALL(_ifxhcd);
	return NULL;
}

