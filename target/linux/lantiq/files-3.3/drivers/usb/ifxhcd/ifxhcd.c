/*****************************************************************************
 **   FILE NAME       : ifxhcd.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the structures, constants, and interfaces for
 **                     the Host Contoller Driver (HCD).
 **
 **                     The Host Controller Driver (HCD) is responsible for translating requests
 **                     from the USB Driver into the appropriate actions on the IFXUSB controller.
 **                     It isolates the USBD from the specifics of the controller by providing an
 **                     API to the USBD.
 *****************************************************************************/

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

#ifdef CONFIG_AVM_POWERMETER
#include <linux/avm_power.h>
#endif /*--- #ifdef CONFIG_AVM_POWERMETER ---*/

#ifdef __DEBUG__
	static void dump_urb_info(struct urb *_urb, char* _fn_name);
	static void dump_channel_info(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh);
#endif


/*!
 \brief Sets the final status of an URB and returns it to the device driver. Any
  required cleanup of the URB is performed.
 */
void ifxhcd_complete_urb(ifxhcd_hcd_t *_ifxhcd, ifxhcd_urbd_t *_urbd,  int _status)
{
	struct urb *urb=NULL;
	unsigned long flags = 0;

	/*== AVM/BC 20101111 Function called with Lock ==*/
	//SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

	if (!list_empty(&_urbd->urbd_list_entry))
		list_del_init (&_urbd->urbd_list_entry);

	if(!_urbd->urb)
	{
		IFX_ERROR("%s: invalid urb\n",__func__);
		/*== AVM/BC 20101111 Function called with Lock ==*/
		//SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
		return;
	}

	urb=_urbd->urb;

	#ifdef __DEBUG__
		if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
		{
			IFX_PRINT("%s: _urbd %p, urb %p, device %d, ep %d %s/%s, status=%d\n",
				  __func__, _urbd,_urbd->urb, usb_pipedevice(_urbd->urb->pipe),
				  usb_pipeendpoint(_urbd->urb->pipe),
				  usb_pipein(_urbd->urb->pipe) ? "IN" : "OUT",
				  (_urbd->is_in) ? "IN" : "OUT",
				   _status);
			if (_urbd->epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			{
				int i;
				for (i = 0; i < _urbd->urb->number_of_packets; i++)
					IFX_PRINT("  ISO Desc %d status: %d\n", i, _urbd->urb->iso_frame_desc[i].status);
			}
		}
	#endif

	if (!_urbd->epqh)
		IFX_ERROR("%s: invalid epqd\n",__func__);

	#if   defined(__UNALIGNED_BUFFER_ADJ__)
		else if(_urbd->is_active)
		{
			if( _urbd->epqh->aligned_checked   &&
			    _urbd->epqh->using_aligned_buf &&
			    _urbd->xfer_buff &&
			    _urbd->is_in )
				memcpy(_urbd->xfer_buff,_urbd->epqh->aligned_buf,_urbd->xfer_len);
			_urbd->epqh->using_aligned_buf=0;
			_urbd->epqh->using_aligned_setup=0;
			_urbd->epqh->aligned_checked=0;
		}
	#endif

	urb->status = _status;
	urb->hcpriv=NULL;
	kfree(_urbd);

	usb_hcd_unlink_urb_from_ep(ifxhcd_to_syshcd(_ifxhcd), urb);
	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);

//    usb_hcd_giveback_urb(ifxhcd_to_syshcd(_ifxhcd), urb);
    usb_hcd_giveback_urb(ifxhcd_to_syshcd(_ifxhcd), urb, _status);

    /*== AVM/BC 20100630 - 2.6.28 needs HCD link/unlink URBs ==*/
	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);
}

/*== AVM/BC 20101111 URB Complete deferred
 * Must be called with Spinlock
 */

/*!
 \brief Inserts an urbd structur in the completion list. The urbd will be
  later completed by select_eps_sub
 */
void defer_ifxhcd_complete_urb(ifxhcd_hcd_t *_ifxhcd, ifxhcd_urbd_t *_urbd,  int _status)
{

	_urbd->status = _status;

	//Unlink Urbd from epqh / Insert it into the complete list
	list_move_tail(&_urbd->urbd_list_entry, &_ifxhcd->urbd_complete_list);

}

/*!
 \brief Processes all the URBs in a single EPQHs. Completes them with
        status and frees the URBD.
 */
//static
void kill_all_urbs_in_epqh(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh, int _status)
{
	struct list_head *urbd_item;
	ifxhcd_urbd_t    *urbd;

	if(!_epqh)
		return;

	for (urbd_item  =  _epqh->urbd_list.next;
	     urbd_item != &_epqh->urbd_list;
	     urbd_item  =  _epqh->urbd_list.next)
	{
		urbd = list_entry(urbd_item, ifxhcd_urbd_t, urbd_list_entry);
		ifxhcd_complete_urb(_ifxhcd, urbd, _status);
	}
}


/*!
 \brief Free all EPS in one Processes all the URBs in a single list of EPQHs. Completes them with
        -ETIMEDOUT and frees the URBD.
 */
//static
void epqh_list_free(ifxhcd_hcd_t *_ifxhcd, struct list_head *_epqh_list)
{
		struct list_head *item;
		ifxhcd_epqh_t    *epqh;

		if (!_epqh_list)
			return;
		if (_epqh_list->next == NULL) /* The list hasn't been initialized yet. */
			return;

	/* Ensure there are no URBDs or URBs left. */
	for (item = _epqh_list->next; item != _epqh_list; item = _epqh_list->next)
	{
		epqh = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
		kill_all_urbs_in_epqh(_ifxhcd, epqh, -ETIMEDOUT);
		ifxhcd_epqh_free(epqh);
	}
}



//static
void epqh_list_free_all(ifxhcd_hcd_t *_ifxhcd)
{
	unsigned long flags;

	/*== AVM/BC 20101111 - 2.6.28 Needs Spinlock ==*/
	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

	epqh_list_free(_ifxhcd, &_ifxhcd->epqh_np_active   );
	epqh_list_free(_ifxhcd, &_ifxhcd->epqh_np_ready    );
	epqh_list_free(_ifxhcd, &_ifxhcd->epqh_intr_active );
	epqh_list_free(_ifxhcd, &_ifxhcd->epqh_intr_ready  );
	#ifdef __EN_ISOC__
		epqh_list_free(_ifxhcd, &_ifxhcd->epqh_isoc_active );
		epqh_list_free(_ifxhcd, &_ifxhcd->epqh_isoc_ready  );
	#endif
	epqh_list_free(_ifxhcd, &_ifxhcd->epqh_stdby       );

	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);

}


/*!
   \brief This function is called to handle the disconnection of host port.
 */
int32_t ifxhcd_disconnect(ifxhcd_hcd_t *_ifxhcd)
{
	IFX_DEBUGPL(DBG_HCDV, "%s(%p)\n", __func__, _ifxhcd);

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
			if (list_empty(&channel->hc_list_entry))
			{
				hc_regs = _ifxhcd->core_if.hc_regs[i];
				hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
				if (hcchar.b.chen)
				{
					/* Halt the channel. */
					hcchar.b.chdis = 1;
					ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
				}
				list_add_tail(&channel->hc_list_entry, &_ifxhcd->free_hc_list);
				ifxhcd_hc_cleanup(&_ifxhcd->core_if, channel);
			}
		}
	}
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
	ifxusb_free_buf(ifxhcd->status_buf);
	return;
}
#ifdef __USE_TIMER_4_SOF__
static enum hrtimer_restart ifxhcd_timer_func(struct hrtimer *timer) {
	ifxhcd_hcd_t 	*ifxhcd = container_of(timer, ifxhcd_hcd_t, hr_timer);
	
	ifxhcd_handle_intr(ifxhcd);

    return HRTIMER_NORESTART;
}
#endif

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

	spin_lock_init(&_ifxhcd->lock);
#ifdef __USE_TIMER_4_SOF__
	hrtimer_init(&_ifxhcd->hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	_ifxhcd->hr_timer.function = ifxhcd_timer_func;
#endif
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

	/* Allocate memory for and initialize the base HCD and  */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	syshcd = usb_create_hcd(&_ifxhcd->hc_driver, _ifxhcd->dev, _ifxhcd->core_if.core_name);
#else
	syshcd = usb_create_hcd(&_ifxhcd->hc_driver, _ifxhcd->dev, _ifxhcd->dev->bus_id);
#endif

	if (syshcd == NULL)
	{
		retval = -ENOMEM;
		goto error1;
	}
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	syshcd->has_tt = 1;
#endif

	syshcd->rsrc_start = (unsigned long)_ifxhcd->core_if.core_global_regs;
	syshcd->regs       = (void *)_ifxhcd->core_if.core_global_regs;
	syshcd->self.otg_port = 0;

	//*((unsigned long *)(&(syshcd->hcd_priv)))=(unsigned long)_ifxhcd;
	//*((unsigned long *)(&(syshcd->hcd_priv[0])))=(unsigned long)_ifxhcd;
	syshcd->hcd_priv[0]=(unsigned long)_ifxhcd;
	_ifxhcd->syshcd=syshcd;

	INIT_LIST_HEAD(&_ifxhcd->epqh_np_active   );
	INIT_LIST_HEAD(&_ifxhcd->epqh_np_ready    );
	INIT_LIST_HEAD(&_ifxhcd->epqh_intr_active );
	INIT_LIST_HEAD(&_ifxhcd->epqh_intr_ready  );
	#ifdef __EN_ISOC__
		INIT_LIST_HEAD(&_ifxhcd->epqh_isoc_active );
		INIT_LIST_HEAD(&_ifxhcd->epqh_isoc_ready  );
	#endif
	INIT_LIST_HEAD(&_ifxhcd->epqh_stdby       );
	INIT_LIST_HEAD(&_ifxhcd->urbd_complete_list);

	/*
	 * Create a host channel descriptor for each host channel implemented
	 * in the controller. Initialize the channel descriptor array.
	 */
	INIT_LIST_HEAD(&_ifxhcd->free_hc_list);
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
//	retval = usb_add_hcd(syshcd, _ifxhcd->core_if.irq, SA_INTERRUPT|SA_SHIRQ);
	retval = usb_add_hcd(syshcd, _ifxhcd->core_if.irq, IRQF_DISABLED | IRQF_SHARED );
	if (retval < 0)
		goto error2;

	/*
	 * Allocate space for storing data on status transactions. Normally no
	 * data is sent, but this space acts as a bit bucket. This must be
	 * done after usb_add_hcd since that function allocates the DMA buffer
	 * pool.
	 */
	_ifxhcd->status_buf = ifxusb_alloc_buf(IFXHCD_STATUS_BUF_SIZE, 1);

	if (_ifxhcd->status_buf)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
		IFX_DEBUGPL(DBG_HCD, "IFX USB HCD Initialized, bus=%s, usbbus=%d\n", _ifxhcd->core_if.core_name, syshcd->self.busnum);
#else
		IFX_DEBUGPL(DBG_HCD, "IFX USB HCD Initialized, bus=%s, usbbus=%d\n", _ifxhcd->dev->bus_id, syshcd->self.busnum);
#endif
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

/* == AVM/WK 20100709 - Fix: Order changed, disable IRQs not before remove_hcd == */

	usb_remove_hcd(syshcd);

	/* Turn off all interrupts */
	ifxusb_wreg (&_ifxhcd->core_if.core_global_regs->gintmsk, 0);
	ifxusb_mreg (&_ifxhcd->core_if.core_global_regs->gahbcfg, 1, 0);

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
		struct list_head 	*item;
		item = ifxhcd->free_hc_list.next;
		while (item != &ifxhcd->free_hc_list)
		{
			list_del(item);
			item = ifxhcd->free_hc_list.next;
		}
	}
	{
		int num_channels = ifxhcd->core_if.params.host_channels;
		int i;
		for (i = 0; i < num_channels; i++)
		{
			ifxhcd_hc_t      *channel;
			channel = &ifxhcd->ifxhc[i];
			list_add_tail(&channel->hc_list_entry, &ifxhcd->free_hc_list);
			ifxhcd_hc_cleanup(&ifxhcd->core_if, channel);
		}
	}
	/* Initialize the USB core for host mode operation. */

	ifxusb_host_enable_interrupts(core_if);
	ifxusb_enable_global_interrupts(core_if);
	ifxusb_phy_power_on (core_if);

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
void ifxhcd_stop(struct usb_hcd *_syshcd)
{
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd(_syshcd);
	hprt0_data_t  hprt0 = { .d32=0 };

	IFX_DEBUGPL(DBG_HCD, "IFX USB HCD STOP\n");

	/* Turn off all interrupts. */
	ifxusb_disable_global_interrupts(&ifxhcd->core_if );
	ifxusb_host_disable_interrupts(&ifxhcd->core_if );
#ifdef __USE_TIMER_4_SOF__
	hrtimer_cancel(&ifxhcd->hr_timer);
#endif
	/*
	 * The root hub should be disconnected before this function is called.
	 * The disconnect will clear the URBD lists (via ..._hcd_urb_dequeue)
	 * and the EPQH lists (via ..._hcd_endpoint_disable).
	 */

	/* Turn off the vbus power */
	IFX_PRINT("PortPower off\n");

	ifxusb_vbus_off(&ifxhcd->core_if );

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
                        /*--- struct usb_host_endpoint *_sysep, Parameter im 2.6.28 entfallen ---*/
                        struct urb               *_urb,
                        gfp_t                     _mem_flags)
{
	int retval = 0;
	ifxhcd_hcd_t *ifxhcd = syshcd_to_ifxhcd (_syshcd);
	struct usb_host_endpoint *_sysep = ifxhcd_urb_to_endpoint(_urb);
	ifxhcd_epqh_t *epqh;

	#ifdef __DEBUG__
		if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
			dump_urb_info(_urb, "ifxusb_hcd_urb_enqueue");
	#endif //__DEBUG__

	if (!ifxhcd->flags.b.port_connect_status)  /* No longer connected. */
		return -ENODEV;

	#ifndef __EN_ISOC__
		if(usb_pipetype(_urb->pipe) == PIPE_ISOCHRONOUS)
		{
			IFX_ERROR("ISOC transfer not supported!!!\n");
			return -ENODEV;
		}
	#endif

	retval=ifxhcd_urbd_create (ifxhcd,_urb);

	if (retval)
	{
		IFX_ERROR("IFXUSB HCD URB Enqueue failed creating URBD\n");
		return retval;
	}
	epqh = (ifxhcd_epqh_t *) _sysep->hcpriv;
	ifxhcd_epqh_ready(ifxhcd, epqh);

	select_eps(ifxhcd);
	//enable_sof(ifxhcd);
	{
		gint_data_t gintsts;
		gintsts.d32=0;
		gintsts.b.sofintr = 1;
		ifxusb_mreg(&ifxhcd->core_if.core_global_regs->gintmsk, 0,gintsts.d32);
	}

	return retval;
}

/*!
   \brief Aborts/cancels a USB transfer request. Always returns 0 to indicate
  success.
 */
int ifxhcd_urb_dequeue( struct usb_hcd *_syshcd,
                        struct urb *_urb, int status /* Parameter neu in 2.6.28 */)
{
	unsigned long flags;
	ifxhcd_hcd_t  *ifxhcd;
	ifxhcd_urbd_t *urbd;
	ifxhcd_epqh_t *epqh;
	int is_active=0;
	int rc;

	struct usb_host_endpoint *_sysep;

	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD URB Dequeue\n");

	#ifndef __EN_ISOC__
		if(usb_pipetype(_urb->pipe) == PIPE_ISOCHRONOUS)
			return 0;
	#endif

	_sysep = ifxhcd_urb_to_endpoint(_urb);

	ifxhcd = syshcd_to_ifxhcd(_syshcd);

	SPIN_LOCK_IRQSAVE(&ifxhcd->lock, flags);

	/*== AVM/BC 20100630 - 2.6.28 needs HCD link/unlink URBs ==*/
	rc = usb_hcd_check_unlink_urb(_syshcd, _urb, status);
	if (rc) {
		SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
		return rc;
	}

	urbd = (ifxhcd_urbd_t *) _urb->hcpriv;

	if(_sysep)
		epqh = (ifxhcd_epqh_t *) _sysep->hcpriv;
	else
		epqh = (ifxhcd_epqh_t *) urbd->epqh;

	if(epqh!=urbd->epqh)
		IFX_ERROR("%s inconsistant epqh %p %p\n",__func__,epqh,urbd->epqh);

	#ifdef __DEBUG__
		if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB))
		{
			dump_urb_info(_urb, "ifxhcd_urb_dequeue");
			if (epqh->is_active)
				dump_channel_info(ifxhcd, epqh);
		}
	#endif //__DEBUG__

	if(!epqh->hc)
		epqh->is_active=0;
	else if (!ifxhcd->flags.b.port_connect_status)
			epqh->is_active=0;
	else if (epqh->is_active && urbd->is_active)
	{
		/*== AVM/WK 20100709 - halt channel only if really started ==*/
		//if (epqh->hc->xfer_started && !epqh->hc->wait_for_sof) {
		/*== AVM/WK 20101112 - halt channel if started ==*/
		if (epqh->hc->xfer_started) {
			/*
			 * If still connected (i.e. in host mode), halt the
			 * channel so it can be used for other transfers. If
			 * no longer connected, the host registers can't be
			 * written to halt the channel since the core is in
			 * device mode.
			 */
			/* == 20110803 AVM/WK FIX propagate status == */
			if (_urb->status == -EINPROGRESS) {
				_urb->status = status;
			}
			ifxhcd_hc_halt(&ifxhcd->core_if, epqh->hc, HC_XFER_URB_DEQUEUE);
			epqh->hc = NULL;
			is_active=1;
		}
	}

	if(is_active)
	{
		SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
	}
	else
	{
		list_del_init(&urbd->urbd_list_entry);
		kfree (urbd);

		/*== AVM/BC 20100630 - 2.6.28 needs HCD link/unlink URBs ==*/
		usb_hcd_unlink_urb_from_ep(_syshcd, _urb);

		SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
		_urb->hcpriv = NULL;
//		usb_hcd_giveback_urb(_syshcd, _urb);
		usb_hcd_giveback_urb(_syshcd, _urb, status /* neu in 2.6.28 */);
		select_eps(ifxhcd);
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
	ifxhcd_epqh_t *epqh;
	ifxhcd_hcd_t  *ifxhcd = syshcd_to_ifxhcd(_syshcd);
	unsigned long flags;

	int retry = 0;

	IFX_DEBUGPL(DBG_HCD, "IFXUSB HCD EP DISABLE: _bEndpointAddress=0x%02x, "
	    "endpoint=%d\n", _sysep->desc.bEndpointAddress,
		    ifxhcd_ep_addr_to_endpoint(_sysep->desc.bEndpointAddress));

	SPIN_LOCK_IRQSAVE(&ifxhcd->lock, flags);
	if((uint32_t)_sysep>=0x80000000 && (uint32_t)_sysep->hcpriv>=(uint32_t)0x80000000)
	{
		epqh = (ifxhcd_epqh_t *)(_sysep->hcpriv);
		if (epqh && epqh->sysep==_sysep)
		{

#if 1  /*== AVM/BC 20101111 CHG Option active: Kill URBs when disabling EP  ==*/
			while (!list_empty(&epqh->urbd_list))
			{
				if (retry++ > 250)
				{
					IFX_WARN("IFXUSB HCD EP DISABLE:"
						 " URBD List for this endpoint is not empty\n");
					break;
				}
				kill_all_urbs_in_epqh(ifxhcd, epqh, -ETIMEDOUT);
			}
#else
			while (!list_empty(&epqh->urbd_list))
			{
				/** Check that the QTD list is really empty */
				if (retry++ > 250)
				{
					IFX_WARN("IFXUSB HCD EP DISABLE:"
						 " URBD List for this endpoint is not empty\n");
					break;
				}
				SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
				schedule_timeout_uninterruptible(1);
				SPIN_LOCK_IRQSAVE(&ifxhcd->lock, flags);
			}
#endif

			ifxhcd_epqh_free(epqh);
			_sysep->hcpriv = NULL;
		}
	}
	SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
}


/*!
   \brief Handles host mode interrupts for the IFXUSB controller. Returns IRQ_NONE if
 * there was no interrupt to handle. Returns IRQ_HANDLED if there was a valid
 * interrupt.
 *
 * This function is called by the USB core when an interrupt occurs
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
   \brief Handles host mode Over Current Interrupt
 */
irqreturn_t ifxhcd_oc_irq(int _irq , void *_dev)
{
	ifxhcd_hcd_t *ifxhcd = _dev;
	int32_t retval=1;

	ifxhcd->flags.b.port_over_current_change = 1;
	ifxusb_vbus_off(&ifxhcd->core_if);
	IFX_DEBUGP("OC INTERRUPT # %d\n",ifxhcd->core_if.core_no);

	//mask_and_ack_ifx_irq (_irq);
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
//			desc->bitmap[0] = 0;
//			desc->bitmap[1] = 0xff;
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

#       	ifdef CONFIG_AVM_POWERMETER
			{
				/* first port only, but 2 Hosts */
				static unsigned char ucOldPower1 = 255;
				static unsigned char ucOldPower2 = 255;

				unsigned char ucNewPower = 0;
				struct usb_device *childdev = _syshcd->self.root_hub->children[0];

				if (childdev != NULL) {
					ucNewPower = (childdev->actconfig != NULL)
									? childdev->actconfig->desc.bMaxPower
									: 50;/* default: 50 means 100 mA*/
				}
				if (_syshcd->self.busnum == 1) {
					if (ucOldPower1 != ucNewPower) {
						ucOldPower1 = ucNewPower;
						printk (KERN_INFO "IFXHCD#1: AVM Powermeter changed to %u mA\n", ucNewPower*2);
						PowerManagmentRessourceInfo(powerdevice_usb_host, ucNewPower*2);
					}
				} else {
					if (ucOldPower2 != ucNewPower) {
						ucOldPower2 = ucNewPower;
						printk (KERN_INFO "IFXHCD#2: AVM Powermeter changed to %u mA\n", ucNewPower*2);
						PowerManagmentRessourceInfo(powerdevice_usb_host2, ucNewPower*2);
					}
				}
			}
#	        endif  /*--- #ifdef CONFIG_AVM_POWERMETER ---*/

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
				port_status |= (1 << USB_PORT_FEAT_ENABLE);
			if (hprt0.b.prtsusp)
				port_status |= (1 << USB_PORT_FEAT_SUSPEND);
			if (hprt0.b.prtovrcurract)
				port_status |= (1 << USB_PORT_FEAT_OVER_CURRENT);
			if (hprt0.b.prtrst)
				port_status |= (1 << USB_PORT_FEAT_RESET);
			if (hprt0.b.prtpwr)
				port_status |= (1 << USB_PORT_FEAT_POWER);
/*			if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
				port_status |= (1 << USB_PORT_FEAT_HIGHSPEED);
			else if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
				port_status |= (1 << USB_PORT_FEAT_LOWSPEED);*/
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
 \brief Assigns transactions from a URBD to a free host channel and initializes the
 host channel to perform the transactions. The host channel is removed from
 the free list.
 \param _ifxhcd The HCD state structure.
 \param _epqh Transactions from the first URBD for this EPQH are selected and assigned to a free host channel.
 */
static int assign_and_init_hc(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh)
{
	ifxhcd_hc_t   *ifxhc;
	ifxhcd_urbd_t *urbd;
	struct urb    *urb;

	IFX_DEBUGPL(DBG_HCDV, "%s(%p,%p)\n", __func__, _ifxhcd, _epqh);

	if(list_empty(&_epqh->urbd_list))
		return 0;

	ifxhc = list_entry(_ifxhcd->free_hc_list.next, ifxhcd_hc_t, hc_list_entry);
	/* Remove the host channel from the free list. */
	list_del_init(&ifxhc->hc_list_entry);

	urbd = list_entry(_epqh->urbd_list.next, ifxhcd_urbd_t, urbd_list_entry);
	urb  = urbd->urb;

	_epqh->hc   = ifxhc;
	_epqh->urbd = urbd;
	ifxhc->epqh = _epqh;

	urbd->is_active=1;

	/*
	 * Use usb_pipedevice to determine device address. This address is
	 * 0 before the SET_ADDRESS command and the correct address afterward.
	 */
	ifxhc->dev_addr = usb_pipedevice(urb->pipe);
	ifxhc->ep_num   = usb_pipeendpoint(urb->pipe);

	ifxhc->xfer_started   = 0;

	if      (urb->dev->speed == USB_SPEED_LOW)  ifxhc->speed = IFXUSB_EP_SPEED_LOW;
	else if (urb->dev->speed == USB_SPEED_FULL) ifxhc->speed = IFXUSB_EP_SPEED_FULL;
	else                                        ifxhc->speed = IFXUSB_EP_SPEED_HIGH;

	ifxhc->mps         = _epqh->mps;
	ifxhc->halt_status = HC_XFER_NO_HALT_STATUS;

	ifxhc->ep_type = _epqh->ep_type;

	if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
	{
		ifxhc->control_phase=IFXHCD_CONTROL_SETUP;
		ifxhc->is_in          = 0;
		ifxhc->data_pid_start = IFXUSB_HC_PID_SETUP;
		ifxhc->xfer_buff      = urbd->setup_buff;
		ifxhc->xfer_len       = 8;
		ifxhc->xfer_count     = 0;
		ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
	}
	else
	{
		ifxhc->is_in          = urbd->is_in;
		ifxhc->xfer_buff      = urbd->xfer_buff;
		ifxhc->xfer_len       = urbd->xfer_len;
		ifxhc->xfer_count     = 0;
		/* == AVM/WK 20100710 Fix - Use toggle of usbcore ==*/
		//ifxhc->data_pid_start = _epqh->data_toggle;
		ifxhc->data_pid_start = usb_gettoggle (urb->dev, usb_pipeendpoint(urb->pipe), usb_pipeout (urb->pipe))
								? IFXUSB_HC_PID_DATA1
								: IFXUSB_HC_PID_DATA0;
		if(ifxhc->is_in)
			ifxhc->short_rw       =0;
		else
			ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;

		#ifdef __EN_ISOC__
			if(_epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
			{
				struct usb_iso_packet_descriptor *frame_desc;
				frame_desc = &urb->iso_frame_desc[urbd->isoc_frame_index];
				ifxhc->xfer_buff += frame_desc->offset + urbd->isoc_split_offset;
				ifxhc->xfer_len   = frame_desc->length - urbd->isoc_split_offset;
				if (ifxhc->isoc_xact_pos == IFXUSB_HCSPLIT_XACTPOS_ALL)
				{
					if (ifxhc->xfer_len <= 188)
						ifxhc->isoc_xact_pos = IFXUSB_HCSPLIT_XACTPOS_ALL;
					else
						ifxhc->isoc_xact_pos = IFXUSB_HCSPLIT_XACTPOS_BEGIN;
				}
			}
		#endif
	}

	ifxhc->do_ping=0;
	if (_ifxhcd->core_if.snpsid < 0x4f54271a && ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		ifxhc->do_ping=1;


	/* Set the split attributes */
	ifxhc->split = 0;
	if (_epqh->need_split) {
		ifxhc->split = 1;
		ifxhc->hub_addr       = urb->dev->tt->hub->devnum;
		ifxhc->port_addr      = urb->dev->ttport;
	}

	//ifxhc->uint16_t pkt_count_limit

	{
		hcint_data_t      hc_intr_mask;
		uint8_t           hc_num = ifxhc->hc_num;
		ifxusb_hc_regs_t *hc_regs = _ifxhcd->core_if.hc_regs[hc_num];

		/* Clear old interrupt conditions for this host channel. */
		hc_intr_mask.d32 = 0xFFFFFFFF;
		hc_intr_mask.b.reserved = 0;
		ifxusb_wreg(&hc_regs->hcint, hc_intr_mask.d32);

		/* Enable channel interrupts required for this transfer. */
		hc_intr_mask.d32 = 0;
		hc_intr_mask.b.chhltd = 1;
		hc_intr_mask.b.ahberr = 1;

		ifxusb_wreg(&hc_regs->hcintmsk, hc_intr_mask.d32);

		/* Enable the top level host channel interrupt. */
		{
			uint32_t          intr_enable;
			intr_enable = (1 << hc_num);
			ifxusb_mreg(&_ifxhcd->core_if.host_global_regs->haintmsk, 0, intr_enable);
		}

		/* Make sure host channel interrupts are enabled. */
		{
			gint_data_t       gintmsk ={.d32 = 0};
			gintmsk.b.hcintr = 1;
			ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk, 0, gintmsk.d32);
		}

		/*
		 * Program the HCCHARn register with the endpoint characteristics for
		 * the current transfer.
		 */
		{
			hcchar_data_t     hcchar;

			hcchar.d32 = 0;
			hcchar.b.devaddr   =  ifxhc->dev_addr;
			hcchar.b.epnum     =  ifxhc->ep_num;
			hcchar.b.lspddev   = (ifxhc->speed == IFXUSB_EP_SPEED_LOW);
			hcchar.b.eptype    =  ifxhc->ep_type;
			hcchar.b.mps       =  ifxhc->mps;
			ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);

			IFX_DEBUGPL(DBG_HCDV, "%s: Channel %d\n", __func__, ifxhc->hc_num);
			IFX_DEBUGPL(DBG_HCDV, "  Dev Addr: %d\n"    , hcchar.b.devaddr);
			IFX_DEBUGPL(DBG_HCDV, "  Ep Num: %d\n"      , hcchar.b.epnum);
			IFX_DEBUGPL(DBG_HCDV, "  Is Low Speed: %d\n", hcchar.b.lspddev);
			IFX_DEBUGPL(DBG_HCDV, "  Ep Type: %d\n"     , hcchar.b.eptype);
			IFX_DEBUGPL(DBG_HCDV, "  Max Pkt: %d\n"     , hcchar.b.mps);
			IFX_DEBUGPL(DBG_HCDV, "  Multi Cnt: %d\n"   , hcchar.b.multicnt);
		}
		/* Program the HCSPLIT register for SPLITs */
		{
			hcsplt_data_t     hcsplt;

			hcsplt.d32 = 0;
			if (ifxhc->split)
			{
				IFX_DEBUGPL(DBG_HCDV, "Programming HC %d with split --> %s\n", ifxhc->hc_num,
					   (ifxhc->split==2) ? "CSPLIT" : "SSPLIT");
				hcsplt.b.spltena  = 1;
				hcsplt.b.compsplt = (ifxhc->split==2);
				#ifdef __EN_ISOC__
					if(_epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
						hcsplt.b.xactpos  = ifxhc->isoc_xact_pos;
					else
				#endif
					hcsplt.b.xactpos  = IFXUSB_HCSPLIT_XACTPOS_ALL;
				hcsplt.b.hubaddr  = ifxhc->hub_addr;
				hcsplt.b.prtaddr  = ifxhc->port_addr;
				IFX_DEBUGPL(DBG_HCDV, "   comp split %d\n" , hcsplt.b.compsplt);
				IFX_DEBUGPL(DBG_HCDV, "   xact pos %d\n"   , hcsplt.b.xactpos);
				IFX_DEBUGPL(DBG_HCDV, "   hub addr %d\n"   , hcsplt.b.hubaddr);
				IFX_DEBUGPL(DBG_HCDV, "   port addr %d\n"  , hcsplt.b.prtaddr);
				IFX_DEBUGPL(DBG_HCDV, "   is_in %d\n"      , ifxhc->is_in);
				IFX_DEBUGPL(DBG_HCDV, "   Max Pkt: %d\n"   , ifxhc->mps);
				IFX_DEBUGPL(DBG_HCDV, "   xferlen: %d\n"   , ifxhc->xfer_len);
			}
			ifxusb_wreg(&hc_regs->hcsplt, hcsplt.d32);
		}
	}

	ifxhc->nak_retry_r=ifxhc->nak_retry=0;
	ifxhc->nak_countdown_r=ifxhc->nak_countdown=0;

	if (ifxhc->split)
	{
		if(ifxhc->is_in)
		{
		}
		else
		{
		}
	}
	else if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
	{
		if(ifxhc->is_in)
		{
		}
		else
		{
		}
	}
	else if(_epqh->ep_type==IFXUSB_EP_TYPE_BULK)
	{
		if(ifxhc->is_in)
		{
//			ifxhc->nak_retry_r=ifxhc->nak_retry=nak_retry_max;
//			ifxhc->nak_countdown_r=ifxhc->nak_countdown=nak_countdown_max;
		}
		else
		{
		}
	}
	else if(_epqh->ep_type==IFXUSB_EP_TYPE_INTR)
	{
		if(ifxhc->is_in)
		{
		}
		else
		{
		}
	}
	else if(_epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
	{
		if(ifxhc->is_in)
		{
		}
		else
		{
		}
	}

	return 1;
}

/*!
 \brief This function selects transactions from the HCD transfer schedule and
  assigns them to available host channels. It is called from HCD interrupt
  handler functions.
 */
static void select_eps_sub(ifxhcd_hcd_t *_ifxhcd)
{
	struct list_head *epqh_ptr;
	struct list_head *urbd_ptr;
	ifxhcd_epqh_t    *epqh;
	ifxhcd_urbd_t    *urbd;
	int               ret_val=0;

	/*== AVM/BC 20101111 Function called with Lock ==*/

//	#ifdef __DEBUG__
//		IFX_DEBUGPL(DBG_HCD, "  ifxhcd_select_ep\n");
//	#endif

	/* Process entries in the periodic ready list. */
	#ifdef __EN_ISOC__
		epqh_ptr       = _ifxhcd->epqh_isoc_ready.next;
		while (epqh_ptr != &_ifxhcd->epqh_isoc_ready && !list_empty(&_ifxhcd->free_hc_list))
		{
			epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, epqh_list_entry);
			epqh_ptr = epqh_ptr->next;
			if(epqh->period_do)
			{
				if(assign_and_init_hc(_ifxhcd, epqh))
				{
					IFX_DEBUGPL(DBG_HCD, "  select_eps ISOC\n");
					list_move_tail(&epqh->epqh_list_entry, &_ifxhcd->epqh_isoc_active);
					epqh->is_active=1;
					ret_val=1;
					epqh->period_do=0;
				}
			}
		}
	#endif

	epqh_ptr       = _ifxhcd->epqh_intr_ready.next;
	while (epqh_ptr != &_ifxhcd->epqh_intr_ready && !list_empty(&_ifxhcd->free_hc_list))
	{
		epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, epqh_list_entry);
		epqh_ptr = epqh_ptr->next;
		if(epqh->period_do)
		{
			if(assign_and_init_hc(_ifxhcd, epqh))
			{
				IFX_DEBUGPL(DBG_HCD, "  select_eps INTR\n");
				list_move_tail(&epqh->epqh_list_entry, &_ifxhcd->epqh_intr_active);
				epqh->is_active=1;
				ret_val=1;
				epqh->period_do=0;
			}
		}
	}

	epqh_ptr       = _ifxhcd->epqh_np_ready.next;
	while (epqh_ptr != &_ifxhcd->epqh_np_ready && !list_empty(&_ifxhcd->free_hc_list))  // may need to preserve at lease one for period
	{
		epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, epqh_list_entry);
		epqh_ptr = epqh_ptr->next;
		if(assign_and_init_hc(_ifxhcd, epqh))
		{
			IFX_DEBUGPL(DBG_HCD, "  select_eps CTRL/BULK\n");
			list_move_tail(&epqh->epqh_list_entry, &_ifxhcd->epqh_np_active);
			epqh->is_active=1;
			ret_val=1;
		}
	}
	if(ret_val)
		/*== AVM/BC 20101111 Function called with Lock ==*/
		process_channels_sub(_ifxhcd);

	/* AVM/BC 20101111 Urbds completion loop */
	while (!list_empty(&_ifxhcd->urbd_complete_list))
	{
		urbd_ptr = _ifxhcd->urbd_complete_list.next;
		list_del_init(urbd_ptr);

		urbd = list_entry(urbd_ptr, ifxhcd_urbd_t, urbd_list_entry);

		ifxhcd_complete_urb(_ifxhcd, urbd, urbd->status);

	}

}

static void select_eps_func(unsigned long data)
{
	unsigned long flags;

	ifxhcd_hcd_t *ifxhcd;
	ifxhcd=((ifxhcd_hcd_t *)data);

	/* AVM/BC 20101111 select_eps_in_use flag removed */

	SPIN_LOCK_IRQSAVE(&ifxhcd->lock, flags);

	/*if(ifxhcd->select_eps_in_use){
		SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
		return;
	}
	ifxhcd->select_eps_in_use=1;
	*/

	select_eps_sub(ifxhcd);

	//ifxhcd->select_eps_in_use=0;

	SPIN_UNLOCK_IRQRESTORE(&ifxhcd->lock, flags);
}

void select_eps(ifxhcd_hcd_t *_ifxhcd)
{
	if(in_irq())
	{
		if(!_ifxhcd->select_eps.func)
		{
			_ifxhcd->select_eps.next = NULL;
			_ifxhcd->select_eps.state = 0;
			atomic_set( &_ifxhcd->select_eps.count, 0);
			_ifxhcd->select_eps.func = select_eps_func;
			_ifxhcd->select_eps.data = (unsigned long)_ifxhcd;
		}
		tasklet_schedule(&_ifxhcd->select_eps);
	}
	else
	{
		unsigned long flags;

		/* AVM/BC 20101111 select_eps_in_use flag removed */

		SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

		/*if(_ifxhcd->select_eps_in_use){
			printk ("select_eps non_irq: busy\n");
			SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
			return;
		}
		_ifxhcd->select_eps_in_use=1;
		*/

		select_eps_sub(_ifxhcd);

		//_ifxhcd->select_eps_in_use=0;

		SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
	}
}

/*!
 \brief
 */
static void process_unaligned( ifxhcd_epqh_t *_epqh)
{
	#if   defined(__UNALIGNED_BUFFER_ADJ__)
		if(!_epqh->aligned_checked)
		{
			uint32_t xfer_len;
			xfer_len=_epqh->urbd->xfer_len;
			if(_epqh->urbd->is_in && xfer_len<_epqh->mps)
				xfer_len = _epqh->mps;
			_epqh->using_aligned_buf=0;

			if(xfer_len > 0 && ((unsigned long)_epqh->urbd->xfer_buff) & 3)
			{
				if(   _epqh->aligned_buf
				   && _epqh->aligned_buf_len > 0
				   && _epqh->aligned_buf_len < xfer_len
				  )
				{
					ifxusb_free_buf(_epqh->aligned_buf);
					_epqh->aligned_buf=NULL;
					_epqh->aligned_buf_len=0;
				}
				if(! _epqh->aligned_buf || ! _epqh->aligned_buf_len)
				{
					_epqh->aligned_buf = ifxusb_alloc_buf(xfer_len, _epqh->urbd->is_in);
					if(_epqh->aligned_buf)
						_epqh->aligned_buf_len = xfer_len;
				}
				if(_epqh->aligned_buf)
				{
					if(!_epqh->urbd->is_in)
						memcpy(_epqh->aligned_buf, _epqh->urbd->xfer_buff, xfer_len);
					_epqh->using_aligned_buf=1;
					_epqh->hc->xfer_buff = _epqh->aligned_buf;
				}
				else
					IFX_WARN("%s():%d\n",__func__,__LINE__);
			}
			if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
			{
				_epqh->using_aligned_setup=0;
				if(((unsigned long)_epqh->urbd->setup_buff) & 3)
				{
					if(! _epqh->aligned_setup)
						_epqh->aligned_setup = ifxusb_alloc_buf(8,0);
					if(_epqh->aligned_setup)
					{
						memcpy(_epqh->aligned_setup, _epqh->urbd->setup_buff, 8);
						_epqh->using_aligned_setup=1;
					}
					else
						IFX_WARN("%s():%d\n",__func__,__LINE__);
					_epqh->hc->xfer_buff = _epqh->aligned_setup;
				}
			}
		}
	#elif defined(__UNALIGNED_BUFFER_CHK__)
		if(!_epqh->aligned_checked)
		{
			if(_epqh->urbd->is_in)
			{
				if(_epqh->urbd->xfer_len==0)
					IFX_WARN("%s():%d IN xfer while length is zero \n",__func__,__LINE__);
				else{
					if(_epqh->urbd->xfer_len < _epqh->mps)
						IFX_WARN("%s():%d IN xfer while length < mps \n",__func__,__LINE__);

					if(((unsigned long)_epqh->urbd->xfer_buff) & 3)
						IFX_WARN("%s():%d IN xfer Buffer UNALIGNED\n",__func__,__LINE__);
				}
			}
			else
			{
				if(_epqh->urbd->xfer_len > 0 && (((unsigned long)_epqh->urbd->xfer_buff) & 3) )
					IFX_WARN("%s():%d OUT xfer Buffer UNALIGNED\n",__func__,__LINE__);
			}

			if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
			{
				if(((unsigned long)_epqh->urbd->setup_buff) & 3)
					IFX_WARN("%s():%d SETUP xfer Buffer UNALIGNED\n",__func__,__LINE__);
			}
		}
	#endif
	_epqh->aligned_checked=1;
}


/*!
 \brief
 */
void process_channels_sub(ifxhcd_hcd_t *_ifxhcd)
{
	ifxhcd_epqh_t	 *epqh;
	struct list_head *epqh_item;
	struct ifxhcd_hc *hc;

	#ifdef __EN_ISOC__
		if (!list_empty(&_ifxhcd->epqh_isoc_active))
		{
			for (epqh_item  =  _ifxhcd->epqh_isoc_active.next;
			     epqh_item != &_ifxhcd->epqh_isoc_active;
			     )
			{
				epqh = list_entry(epqh_item, ifxhcd_epqh_t, epqh_list_entry);
				epqh_item  =  epqh_item->next;
				hc=epqh->hc;
				if(hc && !hc->xfer_started && epqh->period_do)
				{
					if(hc->split==0
					    || hc->split==1
					   )
					{
						//epqh->ping_state = 0;
						process_unaligned(epqh);
						hc->wait_for_sof=epqh->wait_for_sof;
						epqh->wait_for_sof=0;
						ifxhcd_hc_start(&_ifxhcd->core_if, hc);
						epqh->period_do=0;
						{
							gint_data_t gintsts = {.d32 = 0};
							gintsts.b.sofintr = 1;
							ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk,0, gintsts.d32);
						}
					}
				}
			}
		}
	#endif

	if (!list_empty(&_ifxhcd->epqh_intr_active))
	{
		for (epqh_item  =  _ifxhcd->epqh_intr_active.next;
		     epqh_item != &_ifxhcd->epqh_intr_active;
		     )
		{
			epqh = list_entry(epqh_item, ifxhcd_epqh_t, epqh_list_entry);
			epqh_item  =  epqh_item->next;
			hc=epqh->hc;
			if(hc && !hc->xfer_started && epqh->period_do)
			{
				if(hc->split==0
				    || hc->split==1
				   )
				{
					//epqh->ping_state = 0;
					process_unaligned(epqh);
					hc->wait_for_sof=epqh->wait_for_sof;
					epqh->wait_for_sof=0;
					ifxhcd_hc_start(&_ifxhcd->core_if, hc);
					epqh->period_do=0;
#ifdef __USE_TIMER_4_SOF__
					/* AVM/WK change: let hc_start decide, if irq is needed */
#else
					{
						gint_data_t gintsts = {.d32 = 0};
						gintsts.b.sofintr = 1;
						ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk,0, gintsts.d32);
					}
#endif
				}
			}

		}
	}

	if (!list_empty(&_ifxhcd->epqh_np_active))
	{
		for (epqh_item  =  _ifxhcd->epqh_np_active.next;
		     epqh_item != &_ifxhcd->epqh_np_active;
		     )
		{
			epqh = list_entry(epqh_item, ifxhcd_epqh_t, epqh_list_entry);
			epqh_item  =  epqh_item->next;
			hc=epqh->hc;
			if(hc)
			{
				if(!hc->xfer_started)
				{
					if(hc->split==0
					    || hc->split==1
					  //|| hc->split_counter == 0
					   )
					{
						//epqh->ping_state = 0;
						process_unaligned(epqh);
						hc->wait_for_sof=epqh->wait_for_sof;
						epqh->wait_for_sof=0;
						ifxhcd_hc_start(&_ifxhcd->core_if, hc);
					}
				}
			}
		}
	}
}

void process_channels(ifxhcd_hcd_t *_ifxhcd)
{
	unsigned long flags;

	/* AVM/WK Fix: use spin_lock instead busy flag
	**/
	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

	//if(_ifxhcd->process_channels_in_use)
	//	return;
	//_ifxhcd->process_channels_in_use=1;

	process_channels_sub(_ifxhcd);
	//_ifxhcd->process_channels_in_use=0;
	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
}


#ifdef __HC_XFER_TIMEOUT__
	static void hc_xfer_timeout(unsigned long _ptr)
	{
		hc_xfer_info_t *xfer_info = (hc_xfer_info_t *)_ptr;
		int hc_num = xfer_info->hc->hc_num;
		IFX_WARN("%s: timeout on channel %d\n", __func__, hc_num);
		IFX_WARN("  start_hcchar_val 0x%08x\n", xfer_info->hc->start_hcchar_val);
	}
#endif

void ifxhcd_hc_dumb_rx(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc,uint8_t   *dump_buf)
{
	ifxusb_hc_regs_t *hc_regs = _core_if->hc_regs[_ifxhc->hc_num];
	hctsiz_data_t hctsiz= { .d32=0 };
	hcchar_data_t hcchar;


	_ifxhc->xfer_len = _ifxhc->mps;
	hctsiz.b.xfersize = _ifxhc->mps;
	hctsiz.b.pktcnt   = 0;
	hctsiz.b.pid      = _ifxhc->data_pid_start;
	ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);

	ifxusb_wreg(&hc_regs->hcdma, (uint32_t)(CPHYSADDR( ((uint32_t)(dump_buf)))));

	{
		hcint_data_t hcint= { .d32=0 };
//		hcint.b.nak =1;
//		hcint.b.nyet=1;
//		hcint.b.ack =1;
		hcint.d32 =0xFFFFFFFF;
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);
	}

	/* Set host channel enable after all other setup is complete. */
	hcchar.b.chen  = 1;
	hcchar.b.chdis = 0;
	hcchar.b.epdir = 1;
	IFX_DEBUGPL(DBG_HCDV, "  HCCHART: 0x%08x\n", hcchar.d32);
	ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
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
void ifxhcd_hc_start(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc)
{
	hctsiz_data_t hctsiz= { .d32=0 };
	hcchar_data_t hcchar;
	uint32_t max_hc_xfer_size = _core_if->params.max_transfer_size;
	uint16_t max_hc_pkt_count = _core_if->params.max_packet_count;
	ifxusb_hc_regs_t *hc_regs = _core_if->hc_regs[_ifxhc->hc_num];
	hfnum_data_t hfnum;

	hctsiz.b.dopng = 0;
//	if(_ifxhc->do_ping && !_ifxhc->is_in) hctsiz.b.dopng = 1;

	_ifxhc->nak_countdown=_ifxhc->nak_countdown_r;

	/* AVM/BC 20101111 Workaround: Always PING if HI-Speed Out and xfer_len > 0 */
	if(/*_ifxhc->do_ping &&*/
		(!_ifxhc->is_in) &&
		(_ifxhc->speed == IFXUSB_EP_SPEED_HIGH) &&
		((_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK) || ((_ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL) && (_ifxhc->control_phase != IFXHCD_CONTROL_SETUP))) &&
		_ifxhc->xfer_len
		)
		hctsiz.b.dopng = 1;

	_ifxhc->xfer_started = 1;

	if(_ifxhc->epqh->pkt_count_limit > 0 && _ifxhc->epqh->pkt_count_limit < max_hc_pkt_count )
	{
		max_hc_pkt_count=_ifxhc->epqh->pkt_count_limit;
		if(max_hc_pkt_count * _ifxhc->mps <  max_hc_xfer_size)
			max_hc_xfer_size = max_hc_pkt_count * _ifxhc->mps;
	}
	if (_ifxhc->split > 0)
	{
		{
			gint_data_t gintsts = {.d32 = 0};
			gintsts.b.sofintr = 1;
			ifxusb_mreg(&_core_if->core_global_regs->gintmsk,0, gintsts.d32);
		}

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
			/*== AVM/BC WK 20110421 ZERO PACKET Workaround: Is not an error ==*/
			//if(_ifxhc->short_rw==0)
			//	printk(KERN_INFO "%s() line %d: ZLP write without short_rw set!\n",__func__,__LINE__);
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
				/*== AVM/BC WK 20110421 ZERO PACKET Workaround / check if short_rw is needed ==*/
				if(_ifxhc->start_pkt_count * _ifxhc->mps != _ifxhc->xfer_len )
					_ifxhc->short_rw = 0;
			}
		}
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

	hctsiz.b.xfersize = _ifxhc->xfer_len;
	hctsiz.b.pktcnt   = _ifxhc->start_pkt_count;
	hctsiz.b.pid      = _ifxhc->data_pid_start;

	ifxusb_wreg(&hc_regs->hctsiz, hctsiz.d32);


	IFX_DEBUGPL(DBG_HCDV, "%s: Channel %d\n", __func__, _ifxhc->hc_num);
	IFX_DEBUGPL(DBG_HCDV, "  Xfer Size: %d\n", hctsiz.b.xfersize);
	IFX_DEBUGPL(DBG_HCDV, "  Num Pkts: %d\n" , hctsiz.b.pktcnt);
	IFX_DEBUGPL(DBG_HCDV, "  Start PID: %d\n", hctsiz.b.pid);
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

		#ifdef __EN_ISOC__
			if (_ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
				hcsplt.b.xactpos = _ifxhc->isoc_xact_pos;
			else
		#endif
		hcsplt.b.xactpos = IFXUSB_HCSPLIT_XACTPOS_ALL;// if not ISO
		ifxusb_wreg(&hc_regs->hcsplt, hcsplt.d32);
		IFX_DEBUGPL(DBG_HCDV, "  SPLIT: XACT_POS:0x%08x\n", hcsplt.d32);
	}

	hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
//	hcchar.b.multicnt = _ifxhc->multi_count;
	hcchar.b.multicnt = 1;

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

	IFX_DEBUGPL(DBG_HCDV, "  HCCHART: 0x%08x\n", _ifxhc->hcchar);

	/* == 20110901 AVM/WK Fix: Clear IRQ flags in any case ==*/
	{
		hcint_data_t hcint= { .d32=0 };
		hcint.d32 =0xFFFFFFFF;
		ifxusb_wreg(&hc_regs->hcint, hcint.d32);
	}

	if(_ifxhc->wait_for_sof==0)
	{
		hcint_data_t hcint;

		hcint.d32=ifxusb_rreg(&hc_regs->hcintmsk);

		hcint.b.nak =0;
		hcint.b.ack =0;
		/* == 20110901 AVM/WK Fix: We don't need NOT YET IRQ ==*/
		hcint.b.nyet=0;
		if(_ifxhc->nak_countdown_r)
			hcint.b.nak =1;
		ifxusb_wreg(&hc_regs->hcintmsk, hcint.d32);

		/* AVM WK / BC 20100827
		 * MOVED. Oddframe updated inmediatly before write HCChar Register.
		 */
		if (_ifxhc->ep_type == IFXUSB_EP_TYPE_INTR || _ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
		{
			hfnum.d32 = ifxusb_rreg(&_core_if->host_global_regs->hfnum);
			/* 1 if _next_ frame is odd, 0 if it's even */
			hcchar.b.oddfrm = (hfnum.b.frnum & 0x1) ? 0 : 1;
			_ifxhc->hcchar=hcchar.d32;
		}

		ifxusb_wreg(&hc_regs->hcchar, _ifxhc->hcchar);
#ifdef __USE_TIMER_4_SOF__
	} else {
		//activate SOF IRQ
		gint_data_t gintsts = {.d32 = 0};
		gintsts.b.sofintr = 1;
		ifxusb_mreg(&_core_if->core_global_regs->gintmsk,0, gintsts.d32);
#endif
	}

	#ifdef __HC_XFER_TIMEOUT__
		/* Start a timer for this transfer. */
		init_timer(&_ifxhc->hc_xfer_timer);
		_ifxhc->hc_xfer_timer.function = hc_xfer_timeout;
		_ifxhc->hc_xfer_timer.core_if = _core_if;
		_ifxhc->hc_xfer_timer.hc = _ifxhc;
		_ifxhc->hc_xfer_timer.data = (unsigned long)(&_ifxhc->hc_xfer_info);
		_ifxhc->hc_xfer_timer.expires = jiffies + (HZ*10);
		add_timer(&_ifxhc->hc_xfer_timer);
	#endif
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
void ifxhcd_hc_halt(ifxusb_core_if_t *_core_if,
                    ifxhcd_hc_t *_ifxhc,
                    ifxhcd_halt_status_e _halt_status)
{
	hcchar_data_t   hcchar;
	ifxusb_hc_regs_t           *hc_regs;

	hc_regs          = _core_if->hc_regs[_ifxhc->hc_num];

	WARN_ON(_halt_status == HC_XFER_NO_HALT_STATUS);

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
			return;
		}
	}

	if (_ifxhc->halting)
	{
		/*
		 * A halt has already been issued for this channel. This might
		 * happen when a transfer is aborted by a higher level in
		 * the stack.
		 */
		#ifdef __DEBUG__
			IFX_PRINT("*** %s: Channel %d, _hc->halting already set ***\n",
				  __func__, _ifxhc->hc_num);
		#endif
		//ifxusb_dump_global_registers(_core_if); */
		//ifxusb_dump_host_registers(_core_if); */
		return;
	}
	hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
	/* == AVM/WK 20100709 halt channel only if enabled ==*/
	if (hcchar.b.chen) {
		_ifxhc->halting = 1;
		hcchar.b.chdis = 1;

		ifxusb_wreg(&hc_regs->hcchar, hcchar.d32);
		_ifxhc->halt_status = _halt_status;
	}

	IFX_DEBUGPL(DBG_HCDV, "%s: Channel %d\n" , __func__, _ifxhc->hc_num);
	IFX_DEBUGPL(DBG_HCDV, "  hcchar: 0x%08x\n"   , hcchar.d32);
	IFX_DEBUGPL(DBG_HCDV, "  halting: %d\n" , _ifxhc->halting);
	IFX_DEBUGPL(DBG_HCDV, "  halt_status: %d\n"  , _ifxhc->halt_status);

	return;
}

/*!
   \brief Clears a host channel.
 */
void ifxhcd_hc_cleanup(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc)
{
	ifxusb_hc_regs_t *hc_regs;

	_ifxhc->xfer_started = 0;
	/*
	 * Clear channel interrupt enables and any unhandled channel interrupt
	 * conditions.
	 */
	hc_regs = _core_if->hc_regs[_ifxhc->hc_num];
	ifxusb_wreg(&hc_regs->hcintmsk, 0);
	ifxusb_wreg(&hc_regs->hcint, 0xFFFFFFFF);

	#ifdef __HC_XFER_TIMEOUT__
		del_timer(&_ifxhc->hc_xfer_timer);
	#endif
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
			IFX_PRINT("    xfer_started: %d\n"   , hc->xfer_started);
			IFX_PRINT("    halt_status: %d\n"    , hc->halt_status);
			IFX_PRINT("    xfer_buff: %p\n"      , hc->xfer_buff);
			IFX_PRINT("    xfer_len: %d\n"       , hc->xfer_len);
			IFX_PRINT("    epqh: %p\n"           , hc->epqh);
			IFX_PRINT("  NP Active:\n");
			list_for_each(item, &_ifxhcd->epqh_np_active)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
				IFX_PRINT("    %p\n", epqh_item);
			}
			IFX_PRINT("  NP Ready:\n");
			list_for_each(item, &_ifxhcd->epqh_np_ready)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
				IFX_PRINT("    %p\n", epqh_item);
			}
			IFX_PRINT("  INTR Active:\n");
			list_for_each(item, &_ifxhcd->epqh_intr_active)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
				IFX_PRINT("    %p\n", epqh_item);
			}
			IFX_PRINT("  INTR Ready:\n");
			list_for_each(item, &_ifxhcd->epqh_intr_ready)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
				IFX_PRINT("    %p\n", epqh_item);
			}
			#ifdef __EN_ISOC__
				IFX_PRINT("  ISOC Active:\n");
				list_for_each(item, &_ifxhcd->epqh_isoc_active)
				{
					epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
					IFX_PRINT("    %p\n", epqh_item);
				}
				IFX_PRINT("  ISOC Ready:\n");
				list_for_each(item, &_ifxhcd->epqh_isoc_ready)
				{
					epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
					IFX_PRINT("    %p\n", epqh_item);
				}
			#endif
			IFX_PRINT("  Standby:\n");
			list_for_each(item, &_ifxhcd->epqh_stdby)
			{
				epqh_item = list_entry(item, ifxhcd_epqh_t, epqh_list_entry);
				IFX_PRINT("    %p\n", epqh_item);
			}
		}
	}
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
			IFX_PRINT("    xfer_started: %d\n"   , hc->xfer_started);
			IFX_PRINT("    xfer_buff: %p\n"      , hc->xfer_buff);
			IFX_PRINT("    xfer_len: %d\n"       , hc->xfer_len);
			IFX_PRINT("    xfer_count: %d\n"     , hc->xfer_count);
			IFX_PRINT("    halting: %d\n"   , hc->halting);
			IFX_PRINT("    halt_status: %d\n"    , hc->halt_status);
			IFX_PRINT("    split: %d\n"          , hc->split);
			IFX_PRINT("    hub_addr: %d\n"       , hc->hub_addr);
			IFX_PRINT("    port_addr: %d\n"      , hc->port_addr);
			#ifdef __EN_ISOC__
				IFX_PRINT("    isoc_xact_pos: %d\n"       , hc->isoc_xact_pos);
			#endif
			IFX_PRINT("    epqh: %p\n"           , hc->epqh);
			IFX_PRINT("    short_rw: %d\n"       , hc->short_rw);
			IFX_PRINT("    do_ping: %d\n"        , hc->do_ping);
			IFX_PRINT("    control_phase: %d\n"  , hc->control_phase);
			IFX_PRINT("    pkt_count_limit: %d\n", hc->epqh->pkt_count_limit);
			IFX_PRINT("    start_pkt_count: %d\n"       , hc->start_pkt_count);
		}
		IFX_PRINT("************************************************************\n");
		IFX_PRINT("\n");
	}
#endif //__ENABLE_DUMP__

