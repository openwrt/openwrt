/*****************************************************************************
 **   FILE NAME       : ifxhcd.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the structures, constants, and
 **                     interfaces for the Host Contoller Driver (HCD).
 **
 **                     The Host Controller Driver (HCD) is responsible for
 **                     translating requests from the USB Driver into the
 **                     appropriate actions on the IFXUSB controller.
 **                     It isolates the USBD from the specifics of the
 **                     controller by providing an API to the USBD.
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
  \file ifxhcd.c
  \ingroup IFXUSB_DRIVER_V3
  \brief This file contains the implementation of the HCD. In Linux,
   the HCD implements the hc_driver API.
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

#include <linux/dma-mapping.h>


#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"
#include "ifxhcd.h"

#include <asm/irq.h>

#ifdef __DEBUG__
	static void dump_urb_info(struct urb *_urb, char* _fn_name);
#if 0
	static void dump_channel_info(ifxhcd_hcd_t *_ifxhcd,
	                              ifxhcd_epqh_t *_epqh);
#endif
#endif

static void ifxhcd_complete_urb_sub(ifxhcd_urbd_t *_urbd)
{
	ifxhcd_hcd_t *ifxhcd;
	struct urb *urb=NULL;

	if (!list_empty(&_urbd->ql))
	{
		list_del_init(&_urbd->ql);
		_urbd->epqh->urbd_count--;
	}
	else
		IFX_ERROR("%s: urb(%p) not connect to any epqh\n",
		          __func__,_urbd);

	ifxhcd=_urbd->epqh->ifxhcd;
	urb   =_urbd->urb;
	if(!urb)
		IFX_ERROR("%s: invalid urb\n",__func__);
	else if(urb->hcpriv)
	{
		if(urb->hcpriv != _urbd)
			IFX_ERROR("%s: invalid"
			          " urb(%p)->hcpriv(%p) != _urbd(%p)\n",
			          __func__,
			          urb,
			          urb->hcpriv,
			          _urbd);
		#if   defined(__UNALIGNED_BUF_ADJ__)
			if(_urbd->is_in &&
//			   _urbd->using_aligned_buf &&
			   _urbd->aligned_buf)
				memcpy(_urbd->xfer_buff,
				       _urbd->aligned_buf,
				       _urbd->xfer_len);
			if(_urbd->aligned_buf)
				ifxusb_free_buf_h(_urbd->aligned_buf);
		#endif
		urb->hcpriv = NULL;
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
			urb->status=_urbd->status;
			usb_hcd_giveback_urb(ifxhcd_to_syshcd(ifxhcd), urb);
		#else
			usb_hcd_giveback_urb(ifxhcd_to_syshcd(ifxhcd), urb,
			                     _urbd->status);
		#endif
	}
	kfree(_urbd);
}

#ifdef __STRICT_ORDER__
	static void ifxhcd_complete_urb_func(unsigned long data)
	{
		unsigned long             flags;
		ifxhcd_urbd_t *urbd;
		ifxhcd_epqh_t *epqh;
		struct list_head *item;

		int count=10;

		epqh=((ifxhcd_epqh_t *)data);

		while (!list_empty(&epqh->release_list) && count)
		{
			item = epqh->release_list.next;
			urbd = list_entry(item, ifxhcd_urbd_t, ql);
			if (!urbd)
				IFX_ERROR("%s: invalid urbd\n",__func__);
			else if (!urbd->epqh)
				IFX_ERROR("%s: invalid epqd\n",__func__);
			else
			{
				ifxhcd_epqh_t *epqh2;
				epqh2=urbd->epqh;
				local_irq_save(flags);
				LOCK_URBD_LIST(epqh2);
				ifxhcd_complete_urb_sub(urbd);
				UNLOCK_URBD_LIST(epqh2);
				local_irq_restore (flags);
			}
			count--;
		}
		if(!list_empty(&epqh->release_list))
			tasklet_schedule(&epqh->complete_urb_sub);
	}

	/*!
	 \brief Sets the final status of an URB and returns it to the device
	  driver. Any required cleanup of the URB is performed.
	 */
	void ifxhcd_complete_urb(ifxhcd_hcd_t *_ifxhcd,
	                         ifxhcd_urbd_t *_urbd,
	                         int _status)
	{
		unsigned long             flags;

		if(!_urbd)
		{
			IFX_ERROR("%s: invalid urbd\n",__func__);
			return;
		}
		if (!_urbd->epqh)
		{
			IFX_ERROR("%s: invalid epqh\n",__func__);
			return;
		}

		local_irq_save(flags);
		LOCK_URBD_LIST(_urbd->epqh);
		#ifdef __DEBUG__
			if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
			{
				IFX_PRINT("%s: ehqh %p _urbd %p, urb %p,"
				          " device %d, ep %d %s/%s, status=%d\n",
					  __func__,_urbd->epqh,
					  _urbd,_urbd->urb,
					  (_urbd->urb)?usb_pipedevice(_urbd->urb->pipe):-1,
					  (_urbd->urb)?usb_pipeendpoint(_urbd->urb->pipe):-1,
					  (_urbd->urb)?(usb_pipein(_urbd->urb->pipe) ? "IN" : "OUT"):"--",
					  (_urbd->is_in) ? "IN" : "OUT",
					   _status);
				if ((_urbd->urb)&& _urbd->epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
				{
					int i;
					for (i = 0; i < _urbd->urb->number_of_packets; i++)
						IFX_PRINT("  ISO Desc %d status: %d\n", i, _urbd->urb->iso_frame_desc[i].status);
				}
			}
		#endif
		_urbd->status = _status;

		if(_urbd->phase!=URBD_FINISHING)
		{
			if(_urbd->phase!=URBD_DEQUEUEING && _urbd->phase!=URBD_COMPLETING)
				printk(KERN_INFO "Warning: %s() Strange URBD PHASE %d\n",__func__,_urbd->phase);
			if(_urbd->urb)
			{
				if(   _urbd->status == 0
				   && _urbd->phase==URBD_COMPLETING
				   && in_irq())
				{
					list_move_tail(&_urbd->ql,&_urbd->epqh->release_list);
					if(!_urbd->epqh->complete_urb_sub.func)
					{
						_urbd->epqh->complete_urb_sub.next = NULL;
						_urbd->epqh->complete_urb_sub.state = 0;
						atomic_set( &_urbd->epqh->complete_urb_sub.count, 0);
						_urbd->epqh->complete_urb_sub.func = ifxhcd_complete_urb_func;
						_urbd->epqh->complete_urb_sub.data = (unsigned long)_urbd->epqh;
					}
					tasklet_schedule(&_urbd->epqh->complete_urb_sub);
				}
				else
				{
					_urbd->phase=URBD_FINISHING;
					ifxhcd_complete_urb_sub(_urbd);
				}
				UNLOCK_URBD_LIST(_urbd->epqh);
			}
			else
			{
				UNLOCK_URBD_LIST(_urbd->epqh);
				kfree(_urbd);
			}
		}
		else
		{
			printk(KERN_INFO "Warning: %s() Double Completing \n",__func__);
			UNLOCK_URBD_LIST(_urbd->epqh);
		}
		
		local_irq_restore (flags);
	}
#else
	static void ifxhcd_complete_urb_func(unsigned long data)
	{
		unsigned long             flags;
		ifxhcd_urbd_t *urbd;

		urbd=((ifxhcd_urbd_t *)data);

	//	local_irq_save(flags);
		if (!urbd)
			IFX_ERROR("%s: invalid urbd\n",__func__);
		else if (!urbd->epqh)
			IFX_ERROR("%s: invalid epqd\n",__func__);
		else
		{
			local_irq_save(flags);
			LOCK_URBD_LIST(urbd->epqh);
			ifxhcd_complete_urb_sub(urbd);
			UNLOCK_URBD_LIST(urbd->epqh);
			local_irq_restore (flags);
		}
	//	local_irq_restore (flags);
	}


	/*!
	 \brief Sets the final status of an URB and returns it to the device driver. Any
	  required cleanup of the URB is performed.
	 */
	void ifxhcd_complete_urb(ifxhcd_hcd_t *_ifxhcd, ifxhcd_urbd_t *_urbd,  int _status)
	{
		unsigned long             flags;

		if(!_urbd)
		{
			IFX_ERROR("%s: invalid urbd\n",__func__);
			return;
		}
		if (!_urbd->epqh)
		{
			IFX_ERROR("%s: invalid epqh\n",__func__);
			return;
		}

		local_irq_save(flags);
		LOCK_URBD_LIST(_urbd->epqh);
		#ifdef __DEBUG__
			if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
			{
				IFX_PRINT("%s: ehqh %p _urbd %p, urb %p, device %d, ep %d %s/%s, status=%d\n",
					  __func__,_urbd->epqh, _urbd,_urbd->urb,
					  (_urbd->urb)?usb_pipedevice(_urbd->urb->pipe):-1,
					  (_urbd->urb)?usb_pipeendpoint(_urbd->urb->pipe):-1,
					  (_urbd->urb)?(usb_pipein(_urbd->urb->pipe) ? "IN" : "OUT"):"--",
					  (_urbd->is_in) ? "IN" : "OUT",
					   _status);
				if ((_urbd->urb)&& _urbd->epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
				{
					int i;
					for (i = 0; i < _urbd->urb->number_of_packets; i++)
						IFX_PRINT("  ISO Desc %d status: %d\n", i, _urbd->urb->iso_frame_desc[i].status);
				}
			}
		#endif
		_urbd->status = _status;

		if(_urbd->phase!=URBD_FINISHING)
		{
			if(_urbd->phase!=URBD_DEQUEUEING && _urbd->phase!=URBD_COMPLETING)
				printk(KERN_INFO "Warning: %s() Strange URBD PHASE %d\n",__func__,_urbd->phase);
			if(_urbd->urb)
			{
				if(   _urbd->status == 0
				   && _urbd->phase==URBD_COMPLETING
				   && in_irq())
				{
					if(_urbd->complete_urb_sub.func)
						printk(KERN_INFO "Warning: %s() URBD Tasklet is on already\n",__func__);
					_urbd->phase=URBD_FINISHING;
					_urbd->complete_urb_sub.next = NULL;
					_urbd->complete_urb_sub.state = 0;
					atomic_set( &_urbd->complete_urb_sub.count, 0);
					_urbd->complete_urb_sub.func = ifxhcd_complete_urb_func;
					_urbd->complete_urb_sub.data = (unsigned long)_urbd;
					tasklet_schedule(&_urbd->complete_urb_sub);
				}
				else
				{
					_urbd->phase=URBD_FINISHING;
					ifxhcd_complete_urb_sub(_urbd);
				}
			}
			else
				kfree(_urbd);
		}
		else
			printk(KERN_INFO "Warning: %s() Double Completing \n",__func__);
		UNLOCK_URBD_LIST(_urbd->epqh);
		local_irq_restore (flags);
	}
#endif

/*!
 \brief Processes all the URBs in a single EPQHs. Completes them with
        status and frees the URBD.
 */
static
void kill_all_urbs_in_epqh(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh, int _status)
{
	struct list_head *item;
	struct list_head *next;
	ifxhcd_urbd_t    *urbd;

	if(!_epqh)
		return;

	IFX_DEBUGPL(DBG_HCDV, "%s %p\n",__func__,_epqh);
	LOCK_URBD_LIST(_epqh);
	list_for_each(item, &_epqh->urbd_list)
	{
		urbd = list_entry(item, ifxhcd_urbd_t, ql);
		if(   urbd->phase==URBD_IDLE
		   || urbd->phase==URBD_ACTIVE
//		   || urbd->phase==URBD_STARTING
		  )
			urbd->phase=URBD_DEQUEUEING;
	}
	list_for_each_safe(item, next, &_epqh->urbd_list)
	{
		urbd = list_entry(item, ifxhcd_urbd_t, ql);
		if(urbd->phase==URBD_DEQUEUEING)
		{
			urbd->urb->status = _status;
			urbd->phase = URBD_FINISHING;
			ifxhcd_complete_urb_sub(urbd);
		}
		else if(   urbd->phase==URBD_STARTED
		        || urbd->phase==URBD_STARTING
//		        || urbd->phase==URBD_ACTIVE
		       )
		{
			if(ifxhcd_hc_halt(&_ifxhcd->core_if, _epqh->hc, HC_XFER_URB_DEQUEUE))
			{
				urbd->urb->status = _status;
				urbd->phase=URBD_FINISHING;
				ifxhcd_complete_urb_sub(urbd);
			}
		}
		else
			IFX_ERROR("%s: invalid urb phase:%d \n",__func__,urbd->phase);
	}
	UNLOCK_URBD_LIST(_epqh);
	IFX_DEBUGPL(DBG_HCDV, "%s %p finish\n",__func__,_epqh);
}


/*!
 \brief Free all EPS in one Processes all the URBs in a single list of EPQHs. Completes them with
        -ETIMEDOUT and frees the URBD.
 */
static
void epqh_list_free_1(ifxhcd_hcd_t *_ifxhcd, struct list_head *_epqh_list)
{
	ifxhcd_epqh_t    *epqh;
	struct list_head *item;
	if (!_ifxhcd)
		return;
	if (!_epqh_list)
		return;

	IFX_DEBUGPL(DBG_HCDV, "%s %p\n",__func__,_epqh_list);

	item = _epqh_list->next;
	while(item != _epqh_list && item != item->next)
	{
		epqh = list_entry(item, ifxhcd_epqh_t, ql);
		epqh->phase=EPQH_DISABLING;
		item = item->next;
		kill_all_urbs_in_epqh(_ifxhcd, epqh, -ETIMEDOUT);
		#ifdef __STRICT_ORDER__
		if(list_empty(&epqh->urbd_list) && list_empty(&epqh->release_list))
		#else
		if(list_empty(&epqh->urbd_list))
		#endif
			ifxhcd_epqh_free(epqh);
	}
	IFX_DEBUGPL(DBG_HCDV, "%s %p finish\n",__func__,_epqh_list);
	/* Ensure there are no URBDs or URBs left. */
}

static
void epqh_list_free_2(ifxhcd_hcd_t *_ifxhcd, struct list_head *_epqh_list)
{
	ifxhcd_epqh_t    *epqh;
	struct list_head *item;
	struct list_head *next;
	if (!_ifxhcd)
		return;
	if (!_epqh_list)
		return;

	IFX_DEBUGPL(DBG_HCDV, "%s %p\n",__func__,_epqh_list);
	list_for_each_safe(item, next, _epqh_list)
	{
		epqh = list_entry(item, ifxhcd_epqh_t, ql);
		if(item == item->next)
		{
			ifxhcd_epqh_free(epqh);
		}
		else
		{
			uint32_t count=0x80000;
			#ifdef __STRICT_ORDER__
			for(;(!list_empty(&epqh->urbd_list) || !list_empty(&epqh->release_list))&& count> 0; count--) udelay(1);
			#else
			for(;!list_empty(&epqh->urbd_list) && count> 0; count--) udelay(1);
			#endif
			if(!count)
				IFX_ERROR("%s: unable to clear urbd in epqh \n",__func__);
			ifxhcd_epqh_free(epqh);
		}
	}
	IFX_DEBUGPL(DBG_HCDV, "%s %p finish\n",__func__,_epqh_list);
	/* Ensure there are no URBDs or URBs left. */
}

static
void epqh_list_free_all_sub(unsigned long data)
{
	ifxhcd_hcd_t *ifxhcd;

	ifxhcd=(ifxhcd_hcd_t *)data;
	epqh_list_free_1(ifxhcd, &ifxhcd->epqh_list_np  );
	epqh_list_free_1(ifxhcd, &ifxhcd->epqh_list_intr);
	#ifdef __EN_ISOC__
		epqh_list_free_1(ifxhcd, &ifxhcd->epqh_list_isoc);
	#endif

	epqh_list_free_2(ifxhcd, &ifxhcd->epqh_list_np  );
	epqh_list_free_2(ifxhcd, &ifxhcd->epqh_list_intr);
	#ifdef __EN_ISOC__
		epqh_list_free_2(ifxhcd, &ifxhcd->epqh_list_isoc);
	#endif
}

static
void epqh_list_free_all(ifxhcd_hcd_t *_ifxhcd)
{
	_ifxhcd->tasklet_free_epqh_list.next = NULL;
	_ifxhcd->tasklet_free_epqh_list.state = 0;
	atomic_set( &_ifxhcd->tasklet_free_epqh_list.count, 0);
	_ifxhcd->tasklet_free_epqh_list.func=epqh_list_free_all_sub;
	_ifxhcd->tasklet_free_epqh_list.data = (unsigned long)_ifxhcd;
	tasklet_schedule(&_ifxhcd->tasklet_free_epqh_list);
}


/*!
   \brief This function is called to handle the disconnection of host port.
 */
int32_t ifxhcd_disconnect(ifxhcd_hcd_t *_ifxhcd)
{
	IFX_DEBUGPL(DBG_HCDV, "%s(%p)\n", __func__, _ifxhcd);

	_ifxhcd->disconnecting=1;
	/* Set status flags for the hub driver. */
	_ifxhcd->flags.b.port_connect_status_change = 1;
	_ifxhcd->flags.b.port_connect_status = 0;

	/*
	 * Shutdown any transfers in process by clearing the Tx FIFO Empty
	 * interrupt mask and status bits and disabling subsequent host
	 * channel interrupts.
	 */
	 {
		gint_data_t intr = { .d32 = 0 };
		intr.b.nptxfempty = 1;
		intr.b.ptxfempty  = 1;
		intr.b.hcintr     = 1;
		ifxusb_mreg (&_ifxhcd->core_if.core_global_regs->gintmsk, intr.d32, 0);
		ifxusb_mreg (&_ifxhcd->core_if.core_global_regs->gintsts, intr.d32, 0);
	}

	/* Respond with an error status to all URBs in the schedule. */
	epqh_list_free_all(_ifxhcd);

	/* Clean up any host channels that were in use. */
	{
		int               num_channels;
		ifxhcd_hc_t      *channel;
		ifxusb_hc_regs_t *hc_regs;
		hcchar_data_t     hcchar;
		int	              i;

		num_channels = _ifxhcd->core_if.params.host_channels;

		for (i = 0; i < num_channels; i++)
		{
			channel = &_ifxhcd->ifxhc[i];
			hc_regs = _ifxhcd->core_if.hc_regs[i];
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			if (hcchar.b.chen)
				printk(KERN_INFO "Warning: %s() HC still enabled\n",__func__);
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, channel);
		}
	}
	IFX_DEBUGPL(DBG_HCDV, "%s(%p) finish\n", __func__, _ifxhcd);
	return 1;
}


/*!
   \brief Frees secondary storage associated with the ifxhcd_hcd structure contained
          in the struct usb_hcd field.
 */
static void ifxhcd_freeextra(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t 	*ifxhcd = syshcd_to_ifxhcd(_syshcd);

	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD FREE\n");

	/* Free memory for EPQH/URBD lists */
	epqh_list_free_all(ifxhcd);

	/* Free memory for the host channels. */
	ifxusb_free_buf_h(ifxhcd->status_buf);
	return;
}

/*!
   \brief Initializes the HCD. This function allocates memory for and initializes the
  static parts of the usb_hcd and ifxhcd_hcd structures. It also registers the
  USB bus with the core and calls the hc_driver->start() function. It returns
  a negative error on failure.
 */
int ifxhcd_init(ifxhcd_hcd_t *_ifxhcd)
{
	int retval = 0;
	struct usb_hcd *syshcd = NULL;

	IFX_DEBUGPL(DBG_HCD, "IFX USB HCD INIT\n");

	INIT_EPQH_LIST_ALL(_ifxhcd);
	INIT_EPQH_LIST(_ifxhcd);

	init_timer(&_ifxhcd->autoprobe_timer);
	init_timer(&_ifxhcd->host_probe_timer);
	_ifxhcd->probe_sec = 5;
	_ifxhcd->autoprobe_sec = 30;

	_ifxhcd->hc_driver.description      = _ifxhcd->core_if.core_name;
	_ifxhcd->hc_driver.product_desc     = "IFX USB Controller";
	//_ifxhcd->hc_driver.hcd_priv_size    = sizeof(ifxhcd_hcd_t);
	_ifxhcd->hc_driver.hcd_priv_size    = sizeof(unsigned long);
	_ifxhcd->hc_driver.irq              = ifxhcd_irq;
	_ifxhcd->hc_driver.flags            = HCD_MEMORY | HCD_USB2;
	_ifxhcd->hc_driver.start            = ifxhcd_start;
	_ifxhcd->hc_driver.stop             = ifxhcd_stop;
	//_ifxhcd->hc_driver.reset          =
	//_ifxhcd->hc_driver.suspend        =
	//_ifxhcd->hc_driver.resume         =
	_ifxhcd->hc_driver.urb_enqueue      = ifxhcd_urb_enqueue;
	_ifxhcd->hc_driver.urb_dequeue      = ifxhcd_urb_dequeue;
	_ifxhcd->hc_driver.endpoint_disable = ifxhcd_endpoint_disable;
	_ifxhcd->hc_driver.get_frame_number = ifxhcd_get_frame_number;
	_ifxhcd->hc_driver.hub_status_data  = ifxhcd_hub_status_data;
	_ifxhcd->hc_driver.hub_control      = ifxhcd_hub_control;
	//_ifxhcd->hc_driver.hub_suspend    =
	//_ifxhcd->hc_driver.hub_resume     =
	_ifxhcd->pkt_remaining_reload_hs=PKT_REMAINING_RELOAD_HS;
	_ifxhcd->pkt_remaining_reload_fs=PKT_REMAINING_RELOAD_FS;
	_ifxhcd->pkt_remaining_reload_ls=PKT_REMAINING_RELOAD_LS;
	_ifxhcd->pkt_count_limit_bo         =8;
	_ifxhcd->pkt_count_limit_bi         =8;

	/* Allocate memory for and initialize the base HCD and  */
	//syshcd = usb_create_hcd(&_ifxhcd->hc_driver, _ifxhcd->dev, _ifxhcd->dev->bus_id);
	syshcd = usb_create_hcd(&_ifxhcd->hc_driver, _ifxhcd->dev, _ifxhcd->core_if.core_name);

	if (syshcd == NULL)
	{
		retval = -ENOMEM;
		goto error1;
	}

	syshcd->rsrc_start = (unsigned long)_ifxhcd->core_if.core_global_regs;
	syshcd->regs       = (void *)_ifxhcd->core_if.core_global_regs;
	syshcd->self.otg_port = 0;

	//*((unsigned long *)(&(syshcd->hcd_priv)))=(unsigned long)_ifxhcd;
	//*((unsigned long *)(&(syshcd->hcd_priv[0])))=(unsigned long)_ifxhcd;
	syshcd->hcd_priv[0]=(unsigned long)_ifxhcd;
	_ifxhcd->syshcd=syshcd;
	INIT_LIST_HEAD(&_ifxhcd->epqh_list_all   );
	INIT_LIST_HEAD(&_ifxhcd->epqh_list_np    );
	INIT_LIST_HEAD(&_ifxhcd->epqh_list_intr  );
	#ifdef __EN_ISOC__
		INIT_LIST_HEAD(&_ifxhcd->epqh_list_isoc);
	#endif

	/*
	 * Create a host channel descriptor for each host channel implemented
	 * in the controller. Initialize the channel descriptor array.
	 */
	{
		int          num_channels = _ifxhcd->core_if.params.host_channels;
		int i;
		for (i = 0; i < num_channels; i++)
		{
			_ifxhcd->ifxhc[i].hc_num = i;
			IFX_DEBUGPL(DBG_HCDV, "HCD Added channel #%d\n", i);
		}
	}

	/* Set device flags indicating whether the HCD supports DMA. */
	if(_ifxhcd->dev->dma_mask)
		*(_ifxhcd->dev->dma_mask) = ~0;
	_ifxhcd->dev->coherent_dma_mask = ~0;

	/*
	 * Finish generic HCD initialization and start the HCD. This function
	 * allocates the DMA buffer pool, registers the USB bus, requests the
	 * IRQ line, and calls ifxusb_hcd_start method.
	 */
	retval = usb_add_hcd(syshcd, _ifxhcd->core_if.irq, 0
			                                   |IRQF_DISABLED
			                                   |IRQF_SHARED
							   );
	if (retval < 0)
		goto error2;

	/*
	 * Allocate space for storing data on status transactions. Normally no
	 * data is sent, but this space acts as a bit bucket. This must be
	 * done after usb_add_hcd since that function allocates the DMA buffer
	 * pool.
	 */
	_ifxhcd->status_buf = ifxusb_alloc_buf_h(IFXHCD_STATUS_BUF_SIZE, 1);

	if (_ifxhcd->status_buf)
	{
		IFX_DEBUGPL(DBG_HCD, "IFX USB HCD Initialized, bus=%s, usbbus=%d\n", _ifxhcd->core_if.core_name, syshcd->self.busnum);
		return 0;
	}
	IFX_ERROR("%s: status_buf allocation failed\n", __func__);

	/* Error conditions */
	usb_remove_hcd(syshcd);
error2:
	ifxhcd_freeextra(syshcd);
	usb_put_hcd(syshcd);
error1:
	return retval;
}

/*!
   \brief Removes the HCD.
  Frees memory and resources associated with the HCD and deregisters the bus.
 */
void ifxhcd_remove(ifxhcd_hcd_t *_ifxhcd)
{
	struct usb_hcd *syshcd = ifxhcd_to_syshcd(_ifxhcd);

	IFX_DEBUGPL(DBG_HCD, "IFX USB HCD REMOVE\n");

	/* Turn off all interrupts */
	ifxusb_wreg (&_ifxhcd->core_if.core_global_regs->gintmsk, 0);
	ifxusb_mreg (&_ifxhcd->core_if.core_global_regs->gahbcfg, 1, 0);

	usb_remove_hcd(syshcd);
	ifxhcd_freeextra(syshcd);
	usb_put_hcd(syshcd);

	return;
}


/* =========================================================================
 *  Linux HC Driver Functions
 * ========================================================================= */

/*!
   \brief Initializes the IFXUSB controller and its root hub and prepares it for host
 mode operation. Activates the root port. Returns 0 on success and a negative
 error code on failure.
 Called by USB stack.
 */
int ifxhcd_start(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd (_syshcd);
	ifxusb_core_if_t *core_if = &ifxhcd->core_if;
	struct usb_bus *bus;

	IFX_DEBUGPL(DBG_HCD, "IFX USB HCD START\n");

	bus = hcd_to_bus(_syshcd);

	/* Initialize the bus state.  */
	_syshcd->state = HC_STATE_RUNNING;

	/* Initialize and connect root hub if one is not already attached */
	if (bus->root_hub)
	{
		IFX_DEBUGPL(DBG_HCD, "IFX USB HCD Has Root Hub\n");
		/* Inform the HUB driver to resume. */
		usb_hcd_resume_root_hub(_syshcd);
	}

	ifxhcd->flags.d32 = 0;

	/* Put all channels in the free channel list and clean up channel states.*/
	{
		int num_channels = ifxhcd->core_if.params.host_channels;
		int i;
		for (i = 0; i < num_channels; i++)
		{
			ifxhcd_hc_t      *channel;
			channel = &ifxhcd->ifxhc[i];
			ifxhcd_hc_cleanup(&ifxhcd->core_if, channel);
		}
	}
	/* Initialize the USB core for host mode operation. */

	ifxusb_host_enable_interrupts(core_if);
	ifxusb_enable_global_interrupts_h(core_if);
	ifxusb_phy_power_on_h (core_if);

	ifxusb_vbus_init(core_if);

	/* Turn on the vbus power. */
	{
		hprt0_data_t hprt0;
		hprt0.d32 = ifxusb_read_hprt0(core_if);

		IFX_PRINT("Init: Power Port (%d)\n", hprt0.b.prtpwr);
		if (hprt0.b.prtpwr == 0 )
		{
			hprt0.b.prtpwr = 1;
			ifxusb_wreg(core_if->hprt0, hprt0.d32);
			ifxusb_vbus_on(core_if);
		}
	}
	return 0;
}

/*!
   \brief Halts the IFXUSB  host mode operations in a clean manner. USB transfers are
 stopped.
 */
	#if defined(__IS_AR10__)
void		ifxusb_oc_int_free(int port);
	#else
void		ifxusb_oc_int_free(void);
	#endif
 
void ifxhcd_stop(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd(_syshcd);
	hprt0_data_t  hprt0 = { .d32=0 };

	IFX_DEBUGPL(DBG_HCD, "IFX USB HCD STOP\n");

	/* Turn off all interrupts. */
	ifxusb_disable_global_interrupts_h(&ifxhcd->core_if );
	ifxusb_host_disable_interrupts(&ifxhcd->core_if );

	/*
	 * The root hub should be disconnected before this function is called.
	 * The disconnect will clear the URBD lists (via ..._hcd_urb_dequeue)
	 * and the EPQH lists (via ..._hcd_endpoint_disable).
	 */

	/* Turn off the vbus power */
	IFX_PRINT("PortPower off\n");

	ifxusb_vbus_off(&ifxhcd->core_if );
	
	
	#if defined(__IS_AR10__)
		ifxusb_oc_int_free(ifxhcd->core_if.core_no);
	#else
		ifxusb_oc_int_free();
	#endif
	

	ifxusb_vbus_free(&ifxhcd->core_if );
	hprt0.b.prtpwr = 0;
	ifxusb_wreg(ifxhcd->core_if.hprt0, hprt0.d32);
	return;
}

/*!
   \brief Returns the current frame number
 */
int ifxhcd_get_frame_number(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t 	*ifxhcd = syshcd_to_ifxhcd(_syshcd);
	hfnum_data_t hfnum;

	hfnum.d32 = ifxusb_rreg(&ifxhcd->core_if.host_global_regs->hfnum);

	return hfnum.b.frnum;
}

/*!
   \brief Starts processing a USB transfer request specified by a USB Request Block
  (URB). mem_flags indicates the type of memory allocation to use while
  processing this URB.
 */
int ifxhcd_urb_enqueue( struct usb_hcd           *_syshcd,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
                        struct usb_host_endpoint *_sysep,
#endif
                        struct urb               *_urb,
                        gfp_t                     _mem_flags)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd (_syshcd);
	ifxhcd_epqh_t *epqh = NULL;

	#ifdef __DEBUG__
		if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
			dump_urb_info(_urb, "ifxusb_hcd_urb_enqueue");
	#endif //__DEBUG__

	if (!ifxhcd->flags.b.port_connect_status)  /* No longer connected. */
		return -ENODEV;

	#if !defined(__EN_ISOC__)
		if(usb_pipetype(_urb->pipe) == PIPE_ISOCHRONOUS)
		{
			IFX_ERROR("ISOC transfer not supported!!!\n");
			return -ENODEV;
		}
	#endif

	if(_urb->hcpriv)
	{
		IFX_WARN("%s() Previous urb->hcpriv exist %p\n",__func__,_urb->hcpriv);
	#if 1
		return -ENOSPC;
	#endif
	}

	epqh=ifxhcd_urbd_create (ifxhcd,_urb);
	if (!epqh)
	{
		IFX_ERROR("IFXUSB HCD URB Enqueue failed creating URBD\n");
		return -ENOSPC;
	}
	if(epqh->phase==EPQH_DISABLING )
	{
		IFX_ERROR("Enqueue to a DISABLING EP!!!\n");
		return -ENODEV;
	}

	#ifdef __DYN_SOF_INTR__
		ifxhcd->dyn_sof_count = DYN_SOF_COUNT_DEF;
	#endif
	//enable_sof(ifxhcd);
	{
		gint_data_t gintsts;
		gintsts.d32=0;
		gintsts.b.sofintr = 1;
		ifxusb_mreg(&ifxhcd->core_if.core_global_regs->gintmsk, 0,gintsts.d32);
	}

	if(epqh->phase==EPQH_IDLE || epqh->phase==EPQH_STDBY )
	{
		epqh->phase=EPQH_READY;
		#ifdef __EPQD_DESTROY_TIMEOUT__
			del_timer(&epqh->destroy_timer);
		#endif
	}
	select_eps(ifxhcd);
	return 0;
}

/*!
   \brief Aborts/cancels a USB transfer request. Always returns 0 to indicate
  success.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
int ifxhcd_urb_dequeue(struct usb_hcd *_syshcd, struct urb *_urb)
#else
int ifxhcd_urb_dequeue(struct usb_hcd *_syshcd, struct urb *_urb, int status)
#endif
{
	ifxhcd_hcd_t  *ifxhcd;
	struct usb_host_endpoint *sysep;
	ifxhcd_urbd_t *urbd;
	ifxhcd_epqh_t *epqh;

	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD URB Dequeue\n");
	#if !defined(__EN_ISOC__)
		if(usb_pipetype(_urb->pipe) == PIPE_ISOCHRONOUS)
			return 0;
	#endif

	ifxhcd = syshcd_to_ifxhcd(_syshcd);

	urbd = (ifxhcd_urbd_t *) _urb->hcpriv;
	if(!urbd)
	{
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
			_urb->status=-ETIMEDOUT;
			usb_hcd_giveback_urb(_syshcd, _urb);
		#else
//			usb_hcd_giveback_urb(_syshcd, _urb,-ETIMEDOUT);
			usb_hcd_giveback_urb(_syshcd, _urb,status);
		#endif
		return 0;
	}

	sysep = ifxhcd_urb_to_endpoint(_urb);
	if(sysep)
	{
		LOCK_EPQH_LIST_ALL(ifxhcd);
		epqh = sysep_to_epqh(ifxhcd,sysep);
		UNLOCK_EPQH_LIST_ALL(ifxhcd);
		if(epqh!=urbd->epqh)
			IFX_ERROR("%s inconsistant epqh %p %p\n",__func__,epqh,urbd->epqh);
	}
	else
		epqh = (ifxhcd_epqh_t *) urbd->epqh;
	if(!ifxhcd->flags.b.port_connect_status || !epqh)
	{
		urbd->phase=URBD_DEQUEUEING;
		ifxhcd_complete_urb(ifxhcd, urbd, -ENODEV);
	}
	else
	{
		LOCK_URBD_LIST(epqh);
		if(   urbd->phase==URBD_IDLE
		   || urbd->phase==URBD_ACTIVE
//		   || urbd->phase==URBD_STARTING
		   )
		{
			urbd->phase=URBD_DEQUEUEING;
			#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
				ifxhcd_complete_urb(ifxhcd, urbd, -ETIMEDOUT);
			#else
				ifxhcd_complete_urb(ifxhcd, urbd, status);
			#endif
		}
		else if(   urbd->phase==URBD_STARTED
		        || urbd->phase==URBD_STARTING
//		        || urbd->phase==URBD_ACTIVE
		       )
		{
			if(ifxhcd_hc_halt(&ifxhcd->core_if, epqh->hc, HC_XFER_URB_DEQUEUE))
			{
				urbd->phase=URBD_DEQUEUEING;
				#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
					ifxhcd_complete_urb(ifxhcd, urbd, -ETIMEDOUT);
				#else
					ifxhcd_complete_urb(ifxhcd, urbd, status);
				#endif
				ifxhcd_epqh_idle(epqh);
			}
		}
		UNLOCK_URBD_LIST(epqh);
	}
	return 0;
}


/*!
   \brief Frees resources in the IFXUSB controller related to a given endpoint. Also
  clears state in the HCD related to the endpoint. Any URBs for the endpoint
  must already be dequeued.
 */
void ifxhcd_endpoint_disable( struct usb_hcd *_syshcd,
                              struct usb_host_endpoint *_sysep)
{
	ifxhcd_hcd_t  *ifxhcd;
	ifxhcd_epqh_t *epqh;

	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD EP DISABLE: _bEndpointAddress=0x%02x, "
	    "endpoint=%d\n", _sysep->desc.bEndpointAddress,
		    ifxhcd_ep_addr_to_endpoint(_sysep->desc.bEndpointAddress));

	ifxhcd = syshcd_to_ifxhcd(_syshcd);

	LOCK_EPQH_LIST_ALL(ifxhcd);
	epqh = sysep_to_epqh(ifxhcd,_sysep);
	UNLOCK_EPQH_LIST_ALL(ifxhcd);

	if (!epqh)
	{
		return;
	}
	else
	{
		if (epqh->sysep!=_sysep)
		{
			IFX_ERROR("%s inconsistant sysep %p %p %p\n",__func__,epqh,epqh->sysep,_sysep);
			return;
		}

		epqh->phase=EPQH_DISABLING;
		kill_all_urbs_in_epqh(ifxhcd, epqh, -ETIMEDOUT);
		{
			uint32_t count=0x80000;
			for(;!list_empty(&epqh->urbd_list) && count> 0; count--) udelay(1);
			if(!count)
				IFX_ERROR("%s: unable to clear urbd in epqh \n",__func__);
		}
		ifxhcd_epqh_free(epqh);
	}
	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD EP DISABLE: done\n");
}


/*!
  \brief Handles host mode interrupts for the IFXUSB controller. Returns IRQ_NONE if
 there was no interrupt to handle. Returns IRQ_HANDLED if there was a valid
 interrupt.

 This function is called by the USB core when an interrupt occurs
 */
irqreturn_t ifxhcd_irq(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd (_syshcd);
	int32_t retval=0;

	//mask_and_ack_ifx_irq (ifxhcd->core_if.irq);
	retval = ifxhcd_handle_intr(ifxhcd);
	return IRQ_RETVAL(retval);
}



/*!
 \brief Creates Status Change bitmap for the root hub and root port. The bitmap is
  returned in buf. Bit 0 is the status change indicator for the root hub. Bit 1
  is the status change indicator for the single root port. Returns 1 if either
  change indicator is 1, otherwise returns 0.
 */
int ifxhcd_hub_status_data(struct usb_hcd *_syshcd, char *_buf)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd (_syshcd);

	_buf[0] = 0;
	_buf[0] |= (ifxhcd->flags.b.port_connect_status_change ||
	            ifxhcd->flags.b.port_reset_change ||
	            ifxhcd->flags.b.port_enable_change ||
	            ifxhcd->flags.b.port_suspend_change ||
	            ifxhcd->flags.b.port_over_current_change) << 1;

	#ifdef __DEBUG__
		if (_buf[0])
		{
			IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD HUB STATUS DATA:"
				    " Root port status changed\n");
			IFX_DEBUGPL(DBG_HCDV, "  port_connect_status_change: %d\n",
				    ifxhcd->flags.b.port_connect_status_change);
			IFX_DEBUGPL(DBG_HCDV, "  port_reset_change: %d\n",
				    ifxhcd->flags.b.port_reset_change);
			IFX_DEBUGPL(DBG_HCDV, "  port_enable_change: %d\n",
				    ifxhcd->flags.b.port_enable_change);
			IFX_DEBUGPL(DBG_HCDV, "  port_suspend_change: %d\n",
				    ifxhcd->flags.b.port_suspend_change);
			IFX_DEBUGPL(DBG_HCDV, "  port_over_current_change: %d\n",
				    ifxhcd->flags.b.port_over_current_change);
			{
				hprt0_data_t hprt0;
				hprt0.d32 = ifxusb_rreg(ifxhcd->core_if.hprt0);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :%08X\n",hprt0.d32);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :connect: %d/%d\n",hprt0.b.prtconnsts,hprt0.b.prtconndet);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :enable: %d/%d\n",hprt0.b.prtena,hprt0.b.prtenchng);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :OC: %d/%d\n",hprt0.b.prtovrcurract,hprt0.b.prtovrcurrchng);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :rsume/suspend/reset: %d/%d/%d\n",hprt0.b.prtres,hprt0.b.prtsusp,hprt0.b.prtrst);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :port power: %d/\n",hprt0.b.prtpwr);
				IFX_DEBUGPL(DBG_HCDV, "  port reg :speed: %d/\n",hprt0.b.prtspd);
			}
		}
	#endif //__DEBUG__
	return (_buf[0] != 0);
}

#ifdef __WITH_HS_ELECT_TST__
	extern void do_setup(ifxusb_core_if_t *_core_if) ;
	extern void do_in_ack(ifxusb_core_if_t *_core_if);
#endif //__WITH_HS_ELECT_TST__

/*!
 \brief Handles hub class-specific requests.
 */
int ifxhcd_hub_control( struct usb_hcd *_syshcd,
                        u16             _typeReq,
                        u16             _wValue,
                        u16             _wIndex,
                        char           *_buf,
                        u16             _wLength)
{
	int retval = 0;
	ifxhcd_hcd_t              *ifxhcd  = syshcd_to_ifxhcd (_syshcd);
	ifxusb_core_if_t          *core_if = &ifxhcd->core_if;
	struct usb_hub_descriptor *desc;
	hprt0_data_t               hprt0 = {.d32 = 0};

	uint32_t port_status;

	switch (_typeReq)
	{
		case ClearHubFeature:
			IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
			         "ClearHubFeature 0x%x\n", _wValue);
			switch (_wValue)
			{
				case C_HUB_LOCAL_POWER:
				case C_HUB_OVER_CURRENT:
					/* Nothing required here */
					break;
				default:
					retval = -EINVAL;
					IFX_ERROR ("IFXUSB HCD - "
						   "ClearHubFeature request %xh unknown\n", _wValue);
			}
			break;
		case ClearPortFeature:
			if (!_wIndex || _wIndex > 1)
				goto error;

			switch (_wValue)
			{
				case USB_PORT_FEAT_ENABLE:
					IFX_DEBUGPL (DBG_ANY, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_ENABLE\n");
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtena = 1;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					break;
				case USB_PORT_FEAT_SUSPEND:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_SUSPEND\n");
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtres = 1;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					/* Clear Resume bit */
					mdelay (100);
					hprt0.b.prtres = 0;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					break;
				case USB_PORT_FEAT_POWER:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_POWER\n");
					#ifdef __IS_DUAL__
						ifxusb_vbus_off(core_if);
					#else
						ifxusb_vbus_off(core_if);
					#endif
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtpwr = 0;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					break;
				case USB_PORT_FEAT_INDICATOR:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_INDICATOR\n");
					/* Port inidicator not supported */
					break;
				case USB_PORT_FEAT_C_CONNECTION:
					/* Clears drivers internal connect status change
					 * flag */
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_C_CONNECTION\n");
					ifxhcd->flags.b.port_connect_status_change = 0;
					break;
				case USB_PORT_FEAT_C_RESET:
					/* Clears the driver's internal Port Reset Change
					 * flag */
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_C_RESET\n");
					ifxhcd->flags.b.port_reset_change = 0;
					break;
				case USB_PORT_FEAT_C_ENABLE:
					/* Clears the driver's internal Port
					 * Enable/Disable Change flag */
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_C_ENABLE\n");
					ifxhcd->flags.b.port_enable_change = 0;
					break;
				case USB_PORT_FEAT_C_SUSPEND:
					/* Clears the driver's internal Port Suspend
					 * Change flag, which is set when resume signaling on
					 * the host port is complete */
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_C_SUSPEND\n");
					ifxhcd->flags.b.port_suspend_change = 0;
					break;
				case USB_PORT_FEAT_C_OVER_CURRENT:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "ClearPortFeature USB_PORT_FEAT_C_OVER_CURRENT\n");
					ifxhcd->flags.b.port_over_current_change = 0;
					break;
				default:
					retval = -EINVAL;
					IFX_ERROR ("IFXUSB HCD - "
					         "ClearPortFeature request %xh "
					         "unknown or unsupported\n", _wValue);
			}
			break;
		case GetHubDescriptor:
			IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
			         "GetHubDescriptor\n");
			desc = (struct usb_hub_descriptor *)_buf;
			desc->bDescLength = 9;
			desc->bDescriptorType = 0x29;
			desc->bNbrPorts = 1;
			desc->wHubCharacteristics = 0x08;
			desc->bPwrOn2PwrGood = 1;
			desc->bHubContrCurrent = 0;

			desc->u.hs.DeviceRemovable[0] = 0;
		        desc->u.hs.DeviceRemovable[1] = 1;
			/*desc->bitmap[0] = 0;
			desc->bitmap[1] = 0xff;*/
			break;
		case GetHubStatus:
			IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
			         "GetHubStatus\n");
			memset (_buf, 0, 4);
			break;
		case GetPortStatus:
			IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
			         "GetPortStatus\n");
			if (!_wIndex || _wIndex > 1)
				goto error;
			port_status = 0;
			if (ifxhcd->flags.b.port_connect_status_change)
				port_status |= (1 << USB_PORT_FEAT_C_CONNECTION);
			if (ifxhcd->flags.b.port_enable_change)
				port_status |= (1 << USB_PORT_FEAT_C_ENABLE);
			if (ifxhcd->flags.b.port_suspend_change)
				port_status |= (1 << USB_PORT_FEAT_C_SUSPEND);
			if (ifxhcd->flags.b.port_reset_change)
				port_status |= (1 << USB_PORT_FEAT_C_RESET);
			if (ifxhcd->flags.b.port_over_current_change)
			{
				IFX_ERROR("Device Not Supported\n");
				port_status |= (1 << USB_PORT_FEAT_C_OVER_CURRENT);
			}
			if (!ifxhcd->flags.b.port_connect_status)
			{
				/*
				 * The port is disconnected, which means the core is
				 * either in device mode or it soon will be. Just
				 * return 0's for the remainder of the port status
				 * since the port register can't be read if the core
				 * is in device mode.
				 */
				*((u32 *) _buf) = cpu_to_le32(port_status);
				break;
			}

			hprt0.d32 = ifxusb_rreg(core_if->hprt0);
			IFX_DEBUGPL(DBG_HCDV, "  HPRT0: 0x%08x\n", hprt0.d32);
			if (hprt0.b.prtconnsts)
				port_status |= (1 << USB_PORT_FEAT_CONNECTION);
			if (hprt0.b.prtena)
			{
				ifxhcd->disconnecting=0;
				port_status |= (1 << USB_PORT_FEAT_ENABLE);
			}
			if (hprt0.b.prtsusp)
				port_status |= (1 << USB_PORT_FEAT_SUSPEND);
			if (hprt0.b.prtovrcurract)
				port_status |= (1 << USB_PORT_FEAT_OVER_CURRENT);
			if (hprt0.b.prtrst)
				port_status |= (1 << USB_PORT_FEAT_RESET);
			if (hprt0.b.prtpwr)
				port_status |= (1 << USB_PORT_FEAT_POWER);
			if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
				port_status |= USB_PORT_STAT_HIGH_SPEED;
			else if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
				port_status |= USB_PORT_STAT_LOW_SPEED;
			if (hprt0.b.prttstctl)
				port_status |= (1 << USB_PORT_FEAT_TEST);
			/* USB_PORT_FEAT_INDICATOR unsupported always 0 */
			*((u32 *) _buf) = cpu_to_le32(port_status);
			break;
		case SetHubFeature:
			IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
			         "SetHubFeature\n");
			/* No HUB features supported */
			break;
		case SetPortFeature:
			if (_wValue != USB_PORT_FEAT_TEST && (!_wIndex || _wIndex > 1))
				goto error;
			/*
			 * The port is disconnected, which means the core is
			 * either in device mode or it soon will be. Just
			 * return without doing anything since the port
			 * register can't be written if the core is in device
			 * mode.
			 */
			if (!ifxhcd->flags.b.port_connect_status)
				break;
			switch (_wValue)
			{
				case USB_PORT_FEAT_SUSPEND:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "SetPortFeature - USB_PORT_FEAT_SUSPEND\n");
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtsusp = 1;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					//IFX_PRINT( "SUSPEND: HPRT0=%0x\n", hprt0.d32);
					/* Suspend the Phy Clock */
					{
						pcgcctl_data_t pcgcctl = {.d32=0};
						pcgcctl.b.stoppclk = 1;
						ifxusb_wreg(core_if->pcgcctl, pcgcctl.d32);
					}
					break;
				case USB_PORT_FEAT_POWER:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
					     "SetPortFeature - USB_PORT_FEAT_POWER\n");
					ifxusb_vbus_on (core_if);
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtpwr = 1;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					break;
				case USB_PORT_FEAT_RESET:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "SetPortFeature - USB_PORT_FEAT_RESET\n");
					hprt0.d32 = ifxusb_read_hprt0 (core_if);
					hprt0.b.prtrst = 1;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					/* Clear reset bit in 10ms (FS/LS) or 50ms (HS) */
					MDELAY (60);
					hprt0.b.prtrst = 0;
					ifxusb_wreg(core_if->hprt0, hprt0.d32);
					break;
			#ifdef __WITH_HS_ELECT_TST__
				case USB_PORT_FEAT_TEST:
					{
						uint32_t t;
						gint_data_t gintmsk;
						t = (_wIndex >> 8); /* MSB wIndex USB */
						IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
							     "SetPortFeature - USB_PORT_FEAT_TEST %d\n", t);
						warn("USB_PORT_FEAT_TEST %d\n", t);
						if (t < 6)
						{
							hprt0.d32 = ifxusb_read_hprt0 (core_if);
							hprt0.b.prttstctl = t;
							ifxusb_wreg(core_if->hprt0, hprt0.d32);
						}
						else if (t == 6)  /* HS_HOST_PORT_SUSPEND_RESUME */
						{
							/* Save current interrupt mask */
							gintmsk.d32 = ifxusb_rreg(&core_if->core_global_regs->gintmsk);

							/* Disable all interrupts while we muck with
							 * the hardware directly
							 */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, 0);

							/* 15 second delay per the test spec */
							mdelay(15000);

							/* Drive suspend on the root port */
							hprt0.d32 = ifxusb_read_hprt0 (core_if);
							hprt0.b.prtsusp = 1;
							hprt0.b.prtres = 0;
							ifxusb_wreg(core_if->hprt0, hprt0.d32);

							/* 15 second delay per the test spec */
							mdelay(15000);

							/* Drive resume on the root port */
							hprt0.d32 = ifxusb_read_hprt0 (core_if);
							hprt0.b.prtsusp = 0;
							hprt0.b.prtres = 1;
							ifxusb_wreg(core_if->hprt0, hprt0.d32);
							mdelay(100);

							/* Clear the resume bit */
							hprt0.b.prtres = 0;
							ifxusb_wreg(core_if->hprt0, hprt0.d32);

							/* Restore interrupts */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, gintmsk.d32);
						}
						else if (t == 7)  /* SINGLE_STEP_GET_DEVICE_DESCRIPTOR setup */
						{
							/* Save current interrupt mask */
							gintmsk.d32 = ifxusb_rreg(&core_if->core_global_regs->gintmsk);

							/* Disable all interrupts while we muck with
							 * the hardware directly
							 */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, 0);

							/* 15 second delay per the test spec */
							mdelay(15000);

							/* Send the Setup packet */
							do_setup(core_if);

							/* 15 second delay so nothing else happens for awhile */
							mdelay(15000);

							/* Restore interrupts */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, gintmsk.d32);
						}

						else if (t == 8)  /* SINGLE_STEP_GET_DEVICE_DESCRIPTOR execute */
						{
							/* Save current interrupt mask */
							gintmsk.d32 = ifxusb_rreg(&core_if->core_global_regs->gintmsk);

							/* Disable all interrupts while we muck with
							 * the hardware directly
							 */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, 0);

							/* Send the Setup packet */
							do_setup(core_if);

							/* 15 second delay so nothing else happens for awhile */
							mdelay(15000);

							/* Send the In and Ack packets */
							do_in_ack(core_if);

							/* 15 second delay so nothing else happens for awhile */
							mdelay(15000);

							/* Restore interrupts */
							ifxusb_wreg(&core_if->core_global_regs->gintmsk, gintmsk.d32);
						}
					}
					break;
			#endif //__WITH_HS_ELECT_TST__
				case USB_PORT_FEAT_INDICATOR:
					IFX_DEBUGPL (DBG_HCD, "IFXUSB HCD HUB CONTROL - "
						     "SetPortFeature - USB_PORT_FEAT_INDICATOR\n");
					/* Not supported */
					break;
				default:
					retval = -EINVAL;
					IFX_ERROR ("IFXUSB HCD - "
						   "SetPortFeature request %xh "
						   "unknown or unsupported\n", _wValue);
			}
			break;
		default:
		error:
			retval = -EINVAL;
			IFX_WARN ("IFXUSB HCD - "
			          "Unknown hub control request type or invalid typeReq: %xh wIndex: %xh wValue: %xh\n",
			          _typeReq, _wIndex, _wValue);
	}
	return retval;
}




/*!
   \brief This function trigger a data transfer for a host channel and
  starts the transfer.

  For a PING transfer in Slave mode, the Do Ping bit is set in the HCTSIZ
  register along with a packet count of 1 and the channel is enabled. This
  causes a single PING transaction to occur. Other fields in HCTSIZ are
  simply set to 0 since no data transfer occurs in this case.

  For a PING transfer in DMA mode, the HCTSIZ register is initialized with
  all the information required to perform the subsequent data transfer. In
  addition, the Do Ping bit is set in the HCTSIZ register. In this case, the
  controller performs the entire PING protocol, then starts the data
  transfer.
  \param _core_if        Pointer of core_if structure
  \param _ifxhc Information needed to initialize the host channel. The xfer_len
  value may be reduced to accommodate the max widths of the XferSize and
  PktCnt fields in the HCTSIZn register. The multi_count value may be changed
  to reflect the final xfer_len value.
 */
void ifxhcd_hc_start(ifxhcd_hcd_t *_ifxhcd, ifxhcd_hc_t *_ifxhc)
{
	ifxusb_core_if_t *core_if = &_ifxhcd->core_if;
	uint32_t max_hc_xfer_size = core_if->params.max_transfer_size;
	uint16_t max_hc_pkt_count = core_if->params.max_packet_count;
	ifxusb_hc_regs_t *hc_regs = core_if->hc_regs[_ifxhc->hc_num];
	hfnum_data_t hfnum;

	hprt0_data_t hprt0;

	if(_ifxhc->epqh->urbd->phase==URBD_DEQUEUEING)
		return;

	hprt0.d32 = ifxusb_read_hprt0(core_if);

	if(_ifxhcd->pkt_remaining==0)
		return;

#if 0
	if(_ifxhc->phase!=HC_WAITING)
		printk(KERN_INFO "%s() line %d: _ifxhc->phase!=HC_WAITING :%d\n",__func__,__LINE__,_ifxhc->phase);
	if(_ifxhc->epqh->urbd->phase==URBD_IDLE      ) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_IDLE\n",__func__,__LINE__);
//	if(_ifxhc->epqh->urbd->phase==URBD_ACTIVE    ) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_ACTIVE\n",__func__,__LINE__);
	if(_ifxhc->epqh->urbd->phase==URBD_STARTING  ) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_STARTING\n",__func__,__LINE__);
	if(_ifxhc->epqh->urbd->phase==URBD_STARTED   ) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_STARTED\n",__func__,__LINE__);
	if(_ifxhc->epqh->urbd->phase==URBD_COMPLETING) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_COMPLETING\n",__func__,__LINE__);
	if(_ifxhc->epqh->urbd->phase==URBD_DEQUEUEING) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_DEQUEUEING\n",__func__,__LINE__);
	if(_ifxhc->epqh->urbd->phase==URBD_FINISHING ) printk(KERN_INFO "%s() line %d: _ifxhc->epqh->urbd->phase==URBD_FINISHING\n",__func__,__LINE__);
#endif

	if      (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
	{
		if (_ifxhc->split)
		{
			if(max_hc_pkt_count > _ifxhcd->pkt_remaining)
				max_hc_pkt_count = _ifxhcd->pkt_remaining;
		}
		else if(_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)
		{
			if( _ifxhc->is_in && _ifxhcd->pkt_count_limit_bi && max_hc_pkt_count > _ifxhcd->pkt_count_limit_bi)
				max_hc_pkt_count = _ifxhcd->pkt_count_limit_bi;
			if(!_ifxhc->is_in && _ifxhcd->pkt_count_limit_bo && max_hc_pkt_count > _ifxhcd->pkt_count_limit_bo)
				max_hc_pkt_count = _ifxhcd->pkt_count_limit_bo;
			if(max_hc_pkt_count*8 > _ifxhcd->pkt_remaining)
				max_hc_pkt_count = _ifxhcd->pkt_remaining/8;
		}
		else
		{
			if(max_hc_pkt_count > _ifxhcd->pkt_remaining)
				max_hc_pkt_count = _ifxhcd->pkt_remaining;
		}
	}
	else if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
	{
		if(max_hc_pkt_count > _ifxhcd->pkt_remaining)
			max_hc_pkt_count = _ifxhcd->pkt_remaining;
	}
	else
	{
		if(max_hc_pkt_count > _ifxhcd->pkt_remaining)
			max_hc_pkt_count = _ifxhcd->pkt_remaining;
	}

	if(max_hc_pkt_count==0)
		return;

	if(max_hc_pkt_count * _ifxhc->mps <  max_hc_xfer_size)
		max_hc_xfer_size = max_hc_pkt_count * _ifxhc->mps;

	_ifxhc->epqh->urbd->phase=URBD_STARTING;

	if(_ifxhc->is_in || _ifxhc->speed != IFXUSB_EP_SPEED_HIGH || _ifxhc->xfer_len==0)
		_ifxhc->epqh->do_ping=0;
	if(_ifxhc->ep_type == IFXUSB_EP_TYPE_INTR || _ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
		_ifxhc->epqh->do_ping=0;
	if(_ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL && _ifxhc->control_phase != IFXHCD_CONTROL_DATA  )
		_ifxhc->epqh->do_ping=0;

	if (_ifxhc->split > 0)
	{
		_ifxhc->start_pkt_count = 1;
		if(!_ifxhc->is_in && _ifxhc->split>1) // OUT CSPLIT
			_ifxhc->xfer_len = 0;
		if (_ifxhc->xfer_len > _ifxhc->mps)
			_ifxhc->xfer_len = _ifxhc->mps;
		if (_ifxhc->xfer_len > 188)
			_ifxhc->xfer_len = 188;
	}
	else if(_ifxhc->is_in)
	{
		_ifxhc->short_rw = 0;
		if (_ifxhc->xfer_len > 0)
		{
			if (_ifxhc->xfer_len > max_hc_xfer_size)
				_ifxhc->xfer_len = max_hc_xfer_size - _ifxhc->mps + 1;
			_ifxhc->start_pkt_count = (_ifxhc->xfer_len + _ifxhc->mps - 1) / _ifxhc->mps;
			if (_ifxhc->start_pkt_count > max_hc_pkt_count)
				_ifxhc->start_pkt_count = max_hc_pkt_count;
		}
		else /* Need 1 packet for transfer length of 0. */
			_ifxhc->start_pkt_count = 1;
		_ifxhc->xfer_len = _ifxhc->start_pkt_count * _ifxhc->mps;
	}
	else //non-split out
	{
		if (_ifxhc->xfer_len == 0)
		{
			if(_ifxhc->short_rw==0)
				printk(KERN_INFO "Info: %s() line %d: ZLP write without short_rw set! xfer_count:%d/%d \n",__func__,__LINE__,
					_ifxhc->xfer_count,
					_ifxhc->epqh->urbd->xfer_len);
			_ifxhc->start_pkt_count = 1;
		}
		else
		{
			if (_ifxhc->xfer_len > max_hc_xfer_size)
			{
				_ifxhc->start_pkt_count = (max_hc_xfer_size / _ifxhc->mps);
				_ifxhc->xfer_len = _ifxhc->start_pkt_count * _ifxhc->mps;
			}
			else
			{
				_ifxhc->start_pkt_count = (_ifxhc->xfer_len+_ifxhc->mps-1)  / _ifxhc->mps;
//				if(_ifxhc->start_pkt_count * _ifxhc->mps == _ifxhc->xfer_len )
//					_ifxhc->start_pkt_count += _ifxhc->short_rw;
			}
		}
	}

	if      (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
	{
		if (_ifxhc->split)
		{
			if( _ifxhcd->pkt_remaining > _ifxhc->start_pkt_count)
				_ifxhcd->pkt_remaining -= _ifxhc->start_pkt_count;
			else
				_ifxhcd->pkt_remaining  = 0;
		}
		else if(_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)
		{
			if( _ifxhcd->pkt_remaining*8 > _ifxhc->start_pkt_count)
				_ifxhcd->pkt_remaining -= (_ifxhc->start_pkt_count*8);
			else
				_ifxhcd->pkt_remaining  = 0;
		}
		else
		{
			if( _ifxhcd->pkt_remaining > _ifxhc->start_pkt_count)
				_ifxhcd->pkt_remaining -= _ifxhc->start_pkt_count;
			else
				_ifxhcd->pkt_remaining  = 0;
		}
	}
	else if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
	{
		if( _ifxhcd->pkt_remaining > _ifxhc->start_pkt_count)
			_ifxhcd->pkt_remaining -= _ifxhc->start_pkt_count;
		else
			_ifxhcd->pkt_remaining  = 0;
	}
	else
	{
		if( _ifxhcd->pkt_remaining > _ifxhc->start_pkt_count)
			_ifxhcd->pkt_remaining -= _ifxhc->start_pkt_count;
		else
			_ifxhcd->pkt_remaining  = 0;
	}

	#ifdef __EN_ISOC__
		if (_ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
		{
			/* Set up the initial PID for the transfer. */
			#if 1
				_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
			#else
				if (_ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
				{
					if (_ifxhc->is_in)
					{
						if      (_ifxhc->multi_count == 1)
							_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
						else if (_ifxhc->multi_count == 2)
							_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
						else
							_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA2;
					}
					else
					{
						if (_ifxhc->multi_count == 1)
							_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
						else
							_ifxhc->data_pid_start = IFXUSB_HC_PID_MDATA;
					}
				}
				else
					_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
			#endif
		}
	#endif

	IFX_DEBUGPL(DBG_HCDV, "%s: Channel %d\n", __func__, _ifxhc->hc_num);
	{
		hctsiz_data_t hctsiz= { .d32=0 };

		hctsiz.b.dopng = _ifxhc->epqh->do_ping;
		_ifxhc->epqh->do_ping=0;

		if(_ifxhc->is_in || _ifxhc->speed != IFXUSB_EP_SPEED_HIGH || _ifxhc->xfer_len==0)
			hctsiz.b.dopng = 0;
		if(_ifxhc->ep_type == IFXUSB_EP_TYPE_INTR || _ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
			hctsiz.b.dopng = 0;
		if(_ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL && _ifxhc->control_phase != IFXHCD_CONTROL_DATA  )
			hctsiz.b.dopng = 0;

		hctsiz.b.xfersize = _ifxhc->xfer_len;
		hctsiz.b.pktcnt   = _ifxhc->start_pkt_count;
		hctsiz.b.pid      = _ifxhc->data_pid_start;
		ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);

		IFX_DEBUGPL(DBG_HCDV, "  Xfer Size: %d\n", hctsiz.b.xfersize);
		IFX_DEBUGPL(DBG_HCDV, "  Num Pkts: %d\n" , hctsiz.b.pktcnt);
		IFX_DEBUGPL(DBG_HCDV, "  Start PID: %d\n", hctsiz.b.pid);
	}
	IFX_DEBUGPL(DBG_HCDV, "  DMA: 0x%08x\n", (uint32_t)(CPHYSADDR( ((uint32_t)(_ifxhc->xfer_buff))+ _ifxhc->xfer_count )));
	ifxusb_wreg(&hc_regs->hcdma, (uint32_t)(CPHYSADDR( ((uint32_t)(_ifxhc->xfer_buff))+ _ifxhc->xfer_count )));

	/* Start the split */
	if (_ifxhc->split>0)
	{
		hcsplt_data_t hcsplt;
		hcsplt.d32 = ifxusb_rreg (&hc_regs->hcsplt);
		hcsplt.b.spltena = 1;
		if (_ifxhc->split>1)
			hcsplt.b.compsplt = 1;
		else
			hcsplt.b.compsplt = 0;

		#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
			if (_ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
				hcsplt.b.xactpos = _ifxhc->isoc_xact_pos;
			else
		#endif
		hcsplt.b.xactpos = IFXUSB_HCSPLIT_XACTPOS_ALL;// if not ISO
		ifxusb_wreg(&hc_regs->hcsplt, hcsplt.d32);
		IFX_DEBUGPL(DBG_HCDV, "  SPLIT: XACT_POS:0x%08x\n", hcsplt.d32);
	}

	{
		hcchar_data_t hcchar;
		hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
//		hcchar.b.multicnt = _ifxhc->multi_count;
		hcchar.b.multicnt = 1;

		if (_ifxhc->ep_type == IFXUSB_EP_TYPE_INTR || _ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
		{
			hfnum.d32 = ifxusb_rreg(&core_if->host_global_regs->hfnum);
			/* 1 if _next_ frame is odd, 0 if it's even */
			hcchar.b.oddfrm = (hfnum.b.frnum & 0x1) ? 0 : 1;
		}

		#ifdef __DEBUG__
			_ifxhc->start_hcchar_val = hcchar.d32;
			if (hcchar.b.chdis)
				IFX_WARN("%s: chdis set, channel %d, hcchar 0x%08x\n",
					 __func__, _ifxhc->hc_num, hcchar.d32);
		#endif

		/* Set host channel enable after all other setup is complete. */
		hcchar.b.chen  = 1;
		hcchar.b.chdis = 0;
		hcchar.b.epdir =  _ifxhc->is_in;
		_ifxhc->hcchar=hcchar.d32;
	}

	IFX_DEBUGPL(DBG_HCDV, "  HCCHART: 0x%08x\n", _ifxhc->hcchar);

	_ifxhc->phase=HC_STARTING;
}

/*!
   \brief Attempts to halt a host channel. This function should only be called
  to abort a transfer in DMA mode. Under normal circumstances in DMA mode, the
  controller halts the channel when the transfer is complete or a condition
  occurs that requires application intervention.

  In DMA mode, always sets the Channel Enable and Channel Disable bits of the
  HCCHARn register. The controller ensures there is space in the request
  queue before submitting the halt request.

  Some time may elapse before the core flushes any posted requests for this
  host channel and halts. The Channel Halted interrupt handler completes the
  deactivation of the host channel.
 */
int ifxhcd_hc_halt(ifxusb_core_if_t *_core_if,
                    ifxhcd_hc_t *_ifxhc,
                    ifxhcd_halt_status_e _halt_status)
{
	hcchar_data_t   hcchar;
	ifxusb_hc_regs_t           *hc_regs;
	hc_regs          = _core_if->hc_regs[_ifxhc->hc_num];

	WARN_ON(_halt_status == HC_XFER_NO_HALT_STATUS);

	{
		hprt0_data_t hprt0;
		hprt0.d32 = ifxusb_rreg(_core_if->hprt0);
		if(hprt0.b.prtena == 0)
			return -1;
	}

	if (_halt_status == HC_XFER_URB_DEQUEUE ||
	    _halt_status == HC_XFER_AHB_ERR)
	{
		/*
		 * Disable all channel interrupts except Ch Halted. The URBD
		 * and EPQH state associated with this transfer has been cleared
		 * (in the case of URB_DEQUEUE), so the channel needs to be
		 * shut down carefully to prevent crashes.
		 */
		hcint_data_t hcintmsk;
		hcintmsk.d32 = 0;
		hcintmsk.b.chhltd = 1;
		ifxusb_wreg(&hc_regs->hcintmsk, hcintmsk.d32);

		/*
		 * Make sure no other interrupts besides halt are currently
		 * pending. Handling another interrupt could cause a crash due
		 * to the URBD and EPQH state.
		 */
		ifxusb_wreg(&hc_regs->hcint, ~hcintmsk.d32);

		/*
		 * Make sure the halt status is set to URB_DEQUEUE or AHB_ERR
		 * even if the channel was already halted for some other
		 * reason.
		 */
		_ifxhc->halt_status = _halt_status;
	}

	hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
	if (hcchar.b.chen == 0)
	{
		/*
		 * The channel is either already halted or it hasn't
		 * started yet. In DMA mode, the transfer may halt if
		 * it finishes normally or a condition occurs that
		 * requires driver intervention. Don't want to halt
		 * the channel again. In either Slave or DMA mode,
		 * it's possible that the transfer has been assigned
		 * to a channel, but not started yet when an URB is
		 * dequeued. Don't want to halt a channel that hasn't
		 * started yet.
		 */
		_ifxhc->phase=HC_IDLE;
		return -1;
	}

	if (_ifxhc->phase==HC_STOPPING)
	{
		/*
		 * A halt has already been issued for this channel. This might
		 * happen when a transfer is aborted by a higher level in
		 * the stack.
		 */
		#ifdef __DEBUG__
			IFX_PRINT("*** %s: Channel %d, double halt a channel***\n",
				  __func__, _ifxhc->hc_num);
		#endif
		return 0;
	}
	hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
	hcchar.b.chen = 1;
	hcchar.b.chdis = 1;

	ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);

	_ifxhc->halt_status = _halt_status;
	_ifxhc->phase=HC_STOPPING;

	IFX_DEBUGPL(DBG_HCDV, "%s: Channel %d\n" , __func__, _ifxhc->hc_num);
	IFX_DEBUGPL(DBG_HCDV, "  hcchar: 0x%08x\n"   , hcchar.d32);
	IFX_DEBUGPL(DBG_HCDV, "  halt_status: %d\n"  , _ifxhc->halt_status);

	return 0;
}

/*!
   \brief Clears a host channel.
 */
void ifxhcd_hc_cleanup(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc)
{
	ifxusb_hc_regs_t *hc_regs;

	_ifxhc->phase=HC_IDLE;
	_ifxhc->epqh=0;

	/*
	 * Clear channel interrupt enables and any unhandled channel interrupt
	 * conditions.
	 */
	hc_regs = _core_if->hc_regs[_ifxhc->hc_num];
	ifxusb_wreg(&hc_regs->hcintmsk, 0);
	ifxusb_wreg(&hc_regs->hcint, 0xFFFFFFFF);

	#ifdef __DEBUG__
		{
			hcchar_data_t hcchar;
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			if (hcchar.b.chdis)
				IFX_WARN("%s: chdis set, channel %d, hcchar 0x%08x\n", __func__, _ifxhc->hc_num, hcchar.d32);
		}
	#endif
}





#ifdef __DEBUG__
	static void dump_urb_info(struct urb *_urb, char* _fn_name)
	{
		IFX_PRINT("%s, urb %p\n"          , _fn_name, _urb);
		IFX_PRINT("  Device address: %d\n", usb_pipedevice(_urb->pipe));
		IFX_PRINT("  Endpoint: %d, %s\n"  , usb_pipeendpoint(_urb->pipe),
		                                    (usb_pipein(_urb->pipe) ? "IN" : "OUT"));
		IFX_PRINT("  Endpoint type: %s\n",
		    ({	char *pipetype;
		    	switch (usb_pipetype(_urb->pipe)) {
		    		case PIPE_CONTROL:     pipetype = "CONTROL"; break;
		    		case PIPE_BULK:        pipetype = "BULK"; break;
		    		case PIPE_INTERRUPT:   pipetype = "INTERRUPT"; break;
		    		case PIPE_ISOCHRONOUS: pipetype = "ISOCHRONOUS"; break;
		    		default:               pipetype = "UNKNOWN"; break;
		    	};
		    	pipetype;
		    }));
		IFX_PRINT("  Speed: %s\n",
		    ({	char *speed;
		    	switch (_urb->dev->speed) {
		    		case USB_SPEED_HIGH: speed = "HIGH"; break;
		    		case USB_SPEED_FULL: speed = "FULL"; break;
		    		case USB_SPEED_LOW:  speed = "LOW"; break;
		    		default:             speed = "UNKNOWN"; break;
		    	};
		    	speed;
		    }));
		IFX_PRINT("  Max packet size: %d\n",
			  usb_maxpacket(_urb->dev, _urb->pipe, usb_pipeout(_urb->pipe)));
		IFX_PRINT("  Data buffer length: %d\n", _urb->transfer_buffer_length);
		IFX_PRINT("  Transfer buffer: %p, Transfer DMA: %p\n",
			  _urb->transfer_buffer, (void *)_urb->transfer_dma);
		IFX_PRINT("  Setup buffer: %p, Setup DMA: %p\n",
			  _urb->setup_packet, (void *)_urb->setup_dma);
		IFX_PRINT("  Interval: %d\n", _urb->interval);
		if (usb_pipetype(_urb->pipe) == PIPE_ISOCHRONOUS)
		{
			int i;
			for (i = 0; i < _urb->number_of_packets;  i++)
			{
				IFX_PRINT("  ISO Desc %d:\n", i);
				IFX_PRINT("    offset: %d, length %d\n",
				    _urb->iso_frame_desc[i].offset,
				    _urb->iso_frame_desc[i].length);
			}
		}
	}

#if 0
	static void dump_channel_info(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh)
	{
		if (_epqh->hc != NULL)
		{
			ifxhcd_hc_t      *hc = _epqh->hc;
			struct list_head *item;
			ifxhcd_epqh_t      *epqh_item;

			ifxusb_hc_regs_t *hc_regs;

			hcchar_data_t  hcchar;
			hcsplt_data_t  hcsplt;
			hctsiz_data_t  hctsiz;
			uint32_t       hcdma;

			hc_regs = _ifxhcd->core_if.hc_regs[hc->hc_num];
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			hcsplt.d32 = ifxusb_rreg(&hc_regs->hcsplt);
			hctsiz.d32 = ifxusb_rreg(&hc_regs->hctsiz);
			hcdma      = ifxusb_rreg(&hc_regs->hcdma);

			IFX_PRINT("  Assigned to channel %d:\n"       , hc->hc_num);
			IFX_PRINT("    hcchar 0x%08x, hcsplt 0x%08x\n", hcchar.d32, hcsplt.d32);
			IFX_PRINT("    hctsiz 0x%08x, hcdma 0x%08x\n" , hctsiz.d32, hcdma);
			IFX_PRINT("    dev_addr: %d, ep_num: %d, is_in: %d\n",
			   hc->dev_addr, hc->ep_num, hc->is_in);
			IFX_PRINT("    ep_type: %d\n"        , hc->ep_type);
			IFX_PRINT("    max_packet_size: %d\n", hc->mps);
			IFX_PRINT("    data_pid_start: %d\n" , hc->data_pid_start);
			IFX_PRINT("    halt_status: %d\n"    , hc->halt_status);
			IFX_PRINT("    xfer_buff: %p\n"      , hc->xfer_buff);
			IFX_PRINT("    xfer_len: %d\n"       , hc->xfer_len);
			IFX_PRINT("    epqh: %p\n"           , hc->epqh);
			IFX_PRINT("  NP :\n");
			list_for_each(item, &_ifxhcd->epqh_list_np)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, ql);
				IFX_PRINT("    %p\n", epqh_item);
			}
			IFX_PRINT("  INTR :\n");
			list_for_each(item, &_ifxhcd->epqh_list_intr)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, ql);
				IFX_PRINT("    %p\n", epqh_item);
			}
			#ifdef __EN_ISOC__
				IFX_PRINT("  ISOC:\n");
				list_for_each(item, &_ifxhcd->epqh_list_isoc)
				{
					epqh_item = list_entry(item, ifxhcd_epqh_t, ql);
					IFX_PRINT("    %p\n", epqh_item);
				}
			#endif
		}
	}
#endif
#endif //__DEBUG__


/*!
   \brief This function writes a packet into the Tx FIFO associated with the Host
  Channel. For a channel associated with a non-periodic EP, the non-periodic
  Tx FIFO is written. For a channel associated with a periodic EP, the
  periodic Tx FIFO is written. This function should only be called in Slave
  mode.

  Upon return the xfer_buff and xfer_count fields in _hc are incremented by
  then number of bytes written to the Tx FIFO.
 */

#ifdef __ENABLE_DUMP__
	void ifxhcd_dump_state(ifxhcd_hcd_t *_ifxhcd)
	{
		int num_channels;
		int i;
		num_channels = _ifxhcd->core_if.params.host_channels;
		IFX_PRINT("\n");
		IFX_PRINT("************************************************************\n");
		IFX_PRINT("HCD State:\n");
		IFX_PRINT("  Num channels: %d\n", num_channels);
		for (i = 0; i < num_channels; i++) {
			ifxhcd_hc_t *hc = &_ifxhcd->ifxhc[i];
			IFX_PRINT("  Channel %d:\n", hc->hc_num);
			IFX_PRINT("    dev_addr: %d, ep_num: %d, ep_is_in: %d\n",
				  hc->dev_addr, hc->ep_num, hc->is_in);
			IFX_PRINT("    speed: %d\n"          , hc->speed);
			IFX_PRINT("    ep_type: %d\n"        , hc->ep_type);
			IFX_PRINT("    mps: %d\n", hc->mps);
			IFX_PRINT("    data_pid_start: %d\n" , hc->data_pid_start);
			IFX_PRINT("    xfer_buff: %p\n"      , hc->xfer_buff);
			IFX_PRINT("    xfer_len: %d\n"       , hc->xfer_len);
			IFX_PRINT("    xfer_count: %d\n"     , hc->xfer_count);
			IFX_PRINT("    halt_status: %d\n"    , hc->halt_status);
			IFX_PRINT("    split: %d\n"          , hc->split);
			IFX_PRINT("    hub_addr: %d\n"       , hc->hub_addr);
			IFX_PRINT("    port_addr: %d\n"      , hc->port_addr);
			#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
				IFX_PRINT("    isoc_xact_pos: %d\n"       , hc->isoc_xact_pos);
			#endif

			IFX_PRINT("    epqh: %p\n"           , hc->epqh);
			IFX_PRINT("    short_rw: %d\n"       , hc->short_rw);
			IFX_PRINT("    control_phase: %d\n"  , hc->control_phase);
			if(hc->epqh)
			{
				IFX_PRINT("    do_ping: %d\n"        , hc->epqh->do_ping);
			}
			IFX_PRINT("    start_pkt_count: %d\n"       , hc->start_pkt_count);
		}
		IFX_PRINT("************************************************************\n");
		IFX_PRINT("\n");
	}
#endif //__ENABLE_DUMP__

