/*****************************************************************************
 **   FILE NAME       : ifxhcd_queue.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the functions to manage Queue Heads and Queue
 **                     Transfer Descriptors.
 *****************************************************************************/

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
	#define epqh_self_destroy_timeout 5
	static void eqph_destroy_func(unsigned long _ptr)
	{
		ifxhcd_epqh_t *epqh=(ifxhcd_epqh_t *)_ptr;
		if(epqh)
		{
			ifxhcd_epqh_free (epqh);
		}
	}
#endif

#define SCHEDULE_SLOP 10

/*!
  \brief This function allocates and initializes a EPQH.

  \param _ifxhcd The HCD state structure for the USB Host controller.
  \param[in] _urb Holds the information about the device/endpoint that we need
  to initialize the EPQH.

  \return Returns pointer to the newly allocated EPQH, or NULL on error.
 */
ifxhcd_epqh_t *ifxhcd_epqh_create (ifxhcd_hcd_t *_ifxhcd, struct urb *_urb)
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

	/* Initialize EPQH */
	switch (usb_pipetype(_urb->pipe))
	{
		case PIPE_CONTROL    : epqh->ep_type = IFXUSB_EP_TYPE_CTRL; break;
		case PIPE_BULK       : epqh->ep_type = IFXUSB_EP_TYPE_BULK; break;
		case PIPE_ISOCHRONOUS: epqh->ep_type = IFXUSB_EP_TYPE_ISOC; break;
		case PIPE_INTERRUPT  : epqh->ep_type = IFXUSB_EP_TYPE_INTR; break;
	}

	//epqh->data_toggle = IFXUSB_HC_PID_DATA0;

	epqh->mps = usb_maxpacket(_urb->dev, _urb->pipe, !(usb_pipein(_urb->pipe)));

	hprt0.d32 = ifxusb_read_hprt0 (&_ifxhcd->core_if);

	INIT_LIST_HEAD(&epqh->urbd_list);
	INIT_LIST_HEAD(&epqh->epqh_list_entry);
	epqh->hc = NULL;

	epqh->dump_buf = ifxusb_alloc_buf(epqh->mps, 0);

	/* FS/LS Enpoint on HS Hub
	 * NOT virtual root hub */
	epqh->need_split = 0;
	epqh->pkt_count_limit=0;
	if(epqh->ep_type == IFXUSB_EP_TYPE_BULK && !(usb_pipein(_urb->pipe)) )
		epqh->pkt_count_limit=4;
	if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED &&
	    ((_urb->dev->speed == USB_SPEED_LOW) ||
	     (_urb->dev->speed == USB_SPEED_FULL)) &&
	     (_urb->dev->tt) && (_urb->dev->tt->hub->devnum != 1))
	{
		IFX_DEBUGPL(DBG_HCD, "QH init: EP %d: TT found at hub addr %d, for port %d\n",
		       usb_pipeendpoint(_urb->pipe), _urb->dev->tt->hub->devnum,
		       _urb->dev->ttport);
		epqh->need_split = 1;
		epqh->pkt_count_limit=1;
	}

	if (epqh->ep_type == IFXUSB_EP_TYPE_INTR ||
	    epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
	{
		/* Compute scheduling parameters once and save them. */
		epqh->interval    = _urb->interval;
		if(epqh->need_split)
			epqh->interval *= 8;
	}

	epqh->period_counter=0;
	epqh->is_active=0;

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

	if(_epqh->sysep) _epqh->sysep->hcpriv=NULL;
	_epqh->sysep=NULL;

	if(!_epqh)
		return;

	/* Free each QTD in the QTD list */
	local_irq_save (flags);
	if (!list_empty(&_epqh->urbd_list))
		IFX_WARN("%s() invalid epqh state\n",__func__);

	#if defined(__UNALIGNED_BUFFER_ADJ__)
		if(_epqh->aligned_buf)
			ifxusb_free_buf(_epqh->aligned_buf);
		if(_epqh->aligned_setup)
			ifxusb_free_buf(_epqh->aligned_setup);
	#endif

	if (!list_empty(&_epqh->epqh_list_entry))
		list_del_init(&_epqh->epqh_list_entry);

	#ifdef __EPQD_DESTROY_TIMEOUT__
		del_timer(&_epqh->destroy_timer);
	#endif
	if(_epqh->dump_buf)
		ifxusb_free_buf(_epqh->dump_buf);
	_epqh->dump_buf=0;


	kfree (_epqh);
	local_irq_restore (flags);
}

/*!
  \brief This function adds a EPQH to

  \return 0 if successful, negative error code otherwise.
 */
void ifxhcd_epqh_ready(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	local_irq_save(flags);
	if (list_empty(&_epqh->epqh_list_entry))
	{
		#ifdef __EN_ISOC__
		if     (_epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			list_add_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_isoc_ready);
		else
		#endif
		if(_epqh->ep_type == IFXUSB_EP_TYPE_INTR)
			list_add_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_intr_ready);
		else
			list_add_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_np_ready);
		_epqh->is_active=0;
	}
	else if(!_epqh->is_active)
	{
		#ifdef __EN_ISOC__
		if     (_epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_isoc_ready);
		else
		#endif
		if(_epqh->ep_type == IFXUSB_EP_TYPE_INTR)
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_intr_ready);
		else
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_np_ready);
	}
	#ifdef __EPQD_DESTROY_TIMEOUT__
		del_timer(&_epqh->destroy_timer);
	#endif
	local_irq_restore(flags);
}

void ifxhcd_epqh_active(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	local_irq_save(flags);
	if (list_empty(&_epqh->epqh_list_entry))
		IFX_WARN("%s() invalid epqh state\n",__func__);
	#ifdef __EN_ISOC__
		if     (_epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_isoc_active);
		else
	#endif
	if(_epqh->ep_type == IFXUSB_EP_TYPE_INTR)
		list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_intr_active);
	else
		list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_np_active);
	_epqh->is_active=1;
	#ifdef __EPQD_DESTROY_TIMEOUT__
		del_timer(&_epqh->destroy_timer);
	#endif
	local_irq_restore(flags);
}

void ifxhcd_epqh_idle(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	local_irq_save(flags);

	if (list_empty(&_epqh->urbd_list))
	{
		if(_epqh->ep_type == IFXUSB_EP_TYPE_ISOC || _epqh->ep_type == IFXUSB_EP_TYPE_INTR)
		{
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_stdby);
		}
		else
		{
			list_del_init(&_epqh->epqh_list_entry);
			#ifdef __EPQD_DESTROY_TIMEOUT__
				del_timer(&_epqh->destroy_timer);
				_epqh->destroy_timer.expires = jiffies + (HZ*epqh_self_destroy_timeout);
				add_timer(&_epqh->destroy_timer );
			#endif
		}
	}
	else
	{
		#ifdef __EN_ISOC__
		if     (_epqh->ep_type == IFXUSB_EP_TYPE_ISOC)
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_isoc_ready);
		else
		#endif
		if(_epqh->ep_type == IFXUSB_EP_TYPE_INTR)
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_intr_ready);
		else
			list_move_tail(&_epqh->epqh_list_entry, &_ifxhcd->epqh_np_ready);
	}
	_epqh->is_active=0;
	local_irq_restore(flags);
}


void ifxhcd_epqh_idle_periodic(ifxhcd_epqh_t *_epqh)
{
	unsigned long flags;
	if(_epqh->ep_type != IFXUSB_EP_TYPE_ISOC && _epqh->ep_type != IFXUSB_EP_TYPE_INTR)
		return;

	local_irq_save(flags);

	if (list_empty(&_epqh->epqh_list_entry))
		IFX_WARN("%s() invalid epqh state\n",__func__);
	if (!list_empty(&_epqh->urbd_list))
		IFX_WARN("%s() invalid epqh state(not empty)\n",__func__);

	_epqh->is_active=0;
	list_del_init(&_epqh->epqh_list_entry);
	#ifdef __EPQD_DESTROY_TIMEOUT__
		del_timer(&_epqh->destroy_timer);
		_epqh->destroy_timer.expires = jiffies + (HZ*epqh_self_destroy_timeout);
		add_timer(&_epqh->destroy_timer );
	#endif

	local_irq_restore(flags);
}


int ifxhcd_urbd_create (ifxhcd_hcd_t *_ifxhcd,struct urb *_urb)
{
	ifxhcd_urbd_t            *urbd;
	struct usb_host_endpoint *sysep;
	ifxhcd_epqh_t            *epqh;
	unsigned long             flags;
	/* == AVM/WK 20100714 retval correctly initialized ==*/
	int                       retval = -ENOMEM;

	/*== AVM/BC 20100630 - Spinlock ==*/
	//local_irq_save(flags);
	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

//		urbd =  (ifxhcd_urbd_t *) kmalloc (sizeof(ifxhcd_urbd_t), GFP_KERNEL);
	urbd =  (ifxhcd_urbd_t *) kmalloc (sizeof(ifxhcd_urbd_t), GFP_ATOMIC);
	if (urbd != NULL) /* Initializes a QTD structure.*/
	{
		retval = 0;
		memset (urbd, 0, sizeof (ifxhcd_urbd_t));

		sysep = ifxhcd_urb_to_endpoint(_urb);
		epqh = (ifxhcd_epqh_t *)sysep->hcpriv;
		if (epqh == NULL)
		{
			epqh = ifxhcd_epqh_create (_ifxhcd, _urb);
			if (epqh == NULL)
			{
				retval = -ENOSPC;
				kfree(urbd);
				//local_irq_restore (flags);
				SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
				return retval;
			}
			sysep->hcpriv = epqh;
		}

		INIT_LIST_HEAD(&urbd->urbd_list_entry);

		/*== AVM/BC 20100630 - 2.6.28 needs HCD link/unlink URBs ==*/
		retval = usb_hcd_link_urb_to_ep(ifxhcd_to_syshcd(_ifxhcd), _urb);

		if (unlikely(retval)){
			kfree(urbd);
			kfree(epqh);
			SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
			return retval;
		}

		list_add_tail(&urbd->urbd_list_entry, &epqh->urbd_list);
		urbd->urb = _urb;
		_urb->hcpriv = urbd;

		urbd->epqh=epqh;
		urbd->is_in=usb_pipein(_urb->pipe) ? 1 : 0;;

		urbd->xfer_len=_urb->transfer_buffer_length;
#define URB_NO_SETUP_DMA_MAP 0

		if(urbd->xfer_len>0)
		{
			if(_urb->transfer_flags && URB_NO_TRANSFER_DMA_MAP)
				urbd->xfer_buff = (uint8_t *) (KSEG1ADDR((uint32_t *)_urb->transfer_dma));
			else
				urbd->xfer_buff = (uint8_t *) _urb->transfer_buffer;
		}
		if(epqh->ep_type == IFXUSB_EP_TYPE_CTRL)
		{
			if(_urb->transfer_flags && URB_NO_SETUP_DMA_MAP)
				urbd->setup_buff = (uint8_t *) (KSEG1ADDR((uint32_t *)_urb->setup_dma));
			else
				urbd->setup_buff = (uint8_t *) _urb->setup_packet;
		}
	}
	//local_irq_restore (flags);
	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);
	return retval;
}

