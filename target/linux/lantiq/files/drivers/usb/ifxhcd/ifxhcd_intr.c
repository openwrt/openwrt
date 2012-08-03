/*****************************************************************************
 **   FILE NAME       : ifxhcd_intr.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the implementation of the HCD Interrupt handlers.
 *****************************************************************************/

/*!
 \file ifxhcd_intr.c
 \ingroup IFXUSB_DRIVER_V3
 \brief This file contains the implementation of the HCD Interrupt handlers.
*/


#include <linux/version.h>
#include "ifxusb_version.h"

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"

#include "ifxhcd.h"

/* AVM/WK 20100520*/
#ifdef __EN_ISOC__
#error AVM/WK: CONFIG_USB_HOST_IFX_WITH_ISO currently not supported!
#endif

/* Macro used to clear one channel interrupt */
#define clear_hc_int(_hc_regs_,_intr_) \
	do { \
		hcint_data_t hcint_clear = {.d32 = 0}; \
		hcint_clear.b._intr_ = 1; \
		ifxusb_wreg(&((_hc_regs_)->hcint), hcint_clear.d32); \
	} while (0)

/*
 * Macro used to disable one channel interrupt. Channel interrupts are
 * disabled when the channel is halted or released by the interrupt handler.
 * There is no need to handle further interrupts of that type until the
 * channel is re-assigned. In fact, subsequent handling may cause crashes
 * because the channel structures are cleaned up when the channel is released.
 */
#define disable_hc_int(_hc_regs_,_intr_) \
	do { \
		hcint_data_t hcintmsk = {.d32 = 0}; \
		hcintmsk.b._intr_ = 1; \
		ifxusb_mreg(&((_hc_regs_)->hcintmsk), hcintmsk.d32, 0); \
	} while (0)

#define enable_hc_int(_hc_regs_,_intr_) \
	do { \
		hcint_data_t hcintmsk = {.d32 = 0}; \
		hcintmsk.b._intr_ = 1; \
		ifxusb_mreg(&((_hc_regs_)->hcintmsk),0, hcintmsk.d32); \
	} while (0)

/*
 * Save the starting data toggle for the next transfer. The data toggle is
 * saved in the QH for non-control transfers and it's saved in the QTD for
 * control transfers.
 */
uint8_t read_data_toggle(ifxusb_hc_regs_t *_hc_regs)
{
	hctsiz_data_t hctsiz;
	hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
	return(hctsiz.b.pid);
}


static void release_channel_dump(ifxhcd_hc_t      *ifxhc,
                               struct urb       *urb,
                               ifxhcd_epqh_t    *epqh,
                               ifxhcd_urbd_t    *urbd,
                               ifxhcd_halt_status_e  halt_status)
{
	#ifdef __DEBUG__
		printk(KERN_INFO);
		switch (halt_status)
		{
			case HC_XFER_NO_HALT_STATUS:
				printk("HC_XFER_NO_HALT_STATUS");break;
			case HC_XFER_URB_COMPLETE:
				printk("HC_XFER_URB_COMPLETE");break;
			case HC_XFER_AHB_ERR:
				printk("HC_XFER_AHB_ERR");break;
			case HC_XFER_STALL:
				printk("HC_XFER_STALL");break;
			case HC_XFER_BABBLE_ERR:
				printk("HC_XFER_BABBLE_ERR");break;
			case HC_XFER_XACT_ERR:
				printk("HC_XFER_XACT_ERR");break;
			case HC_XFER_URB_DEQUEUE:
				printk("HC_XFER_URB_DEQUEUE");break;
			case HC_XFER_FRAME_OVERRUN:
				printk("HC_XFER_FRAME_OVERRUN");break;
			case HC_XFER_DATA_TOGGLE_ERR:
				printk("HC_XFER_DATA_TOGGLE_ERR");break;
			case HC_XFER_NAK:
				printk("HC_XFER_NAK");break;
			case HC_XFER_COMPLETE:
				printk("HC_XFER_COMPLETE");break;
			default:
				printk("KNOWN");break;
		}
		if(ifxhc)
			printk("Ch %d %s%s S%d " , ifxhc->hc_num
				,(ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL)?"CTRL-":
				   ((ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)?"BULK-":
				     ((ifxhc->ep_type == IFXUSB_EP_TYPE_INTR)?"INTR-":
				       ((ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)?"ISOC-":"????"
				       )
				     )
				   )
				,(ifxhc->is_in)?"IN":"OUT"
				,(ifxhc->split)
				);
		else
			printk(" [NULL HC] ");
		printk("urb=%p epqh=%p urbd=%p\n",urb,epqh,urbd);

		if(urb)
		{
			printk(KERN_INFO "  Device address: %d\n", usb_pipedevice(urb->pipe));
			printk(KERN_INFO "  Endpoint: %d, %s\n", usb_pipeendpoint(urb->pipe),
				    (usb_pipein(urb->pipe) ? "IN" : "OUT"));
			printk(KERN_INFO "  Endpoint type: %s\n",
				    ({char *pipetype;
				    switch (usb_pipetype(urb->pipe)) {
					    case PIPE_CONTROL: pipetype = "CTRL"; break;
					    case PIPE_BULK: pipetype = "BULK"; break;
					    case PIPE_INTERRUPT: pipetype = "INTR"; break;
					    case PIPE_ISOCHRONOUS: pipetype = "ISOC"; break;
					    default: pipetype = "????"; break;
				    }; pipetype;}));
			printk(KERN_INFO "  Speed: %s\n",
				    ({char *speed;
				    switch (urb->dev->speed) {
					    case USB_SPEED_HIGH: speed = "HS"; break;
					    case USB_SPEED_FULL: speed = "FS"; break;
					    case USB_SPEED_LOW: speed = "LS"; break;
				    	default: speed = "????"; break;
				    }; speed;}));
			printk(KERN_INFO "  Max packet size: %d\n",
				    usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe)));
			printk(KERN_INFO "  Data buffer length: %d\n", urb->transfer_buffer_length);
			printk(KERN_INFO "  Transfer buffer: %p, Transfer DMA: %p\n",
				    urb->transfer_buffer, (void *)urb->transfer_dma);
			printk(KERN_INFO "  Setup buffer: %p, Setup DMA: %p\n",
				    urb->setup_packet, (void *)urb->setup_dma);
			printk(KERN_INFO "  Interval: %d\n", urb->interval);
			switch (urb->status)
			{
				case HC_XFER_NO_HALT_STATUS:
					printk(KERN_INFO "  STATUS:HC_XFER_NO_HALT_STATUS\n");break;
				case HC_XFER_URB_COMPLETE:
					printk(KERN_INFO "  STATUS:HC_XFER_URB_COMPLETE\n");break;
				case HC_XFER_AHB_ERR:
					printk(KERN_INFO "  STATUS:HC_XFER_AHB_ERR\n");break;
				case HC_XFER_STALL:
					printk(KERN_INFO "  STATUS:HC_XFER_STALL\n");break;
				case HC_XFER_BABBLE_ERR:
					printk(KERN_INFO "  STATUS:HC_XFER_BABBLE_ERR\n");break;
				case HC_XFER_XACT_ERR:
					printk(KERN_INFO "  STATUS:HC_XFER_XACT_ERR\n");break;
				case HC_XFER_URB_DEQUEUE:
					printk(KERN_INFO "  STATUS:HC_XFER_URB_DEQUEUE\n");break;
				case HC_XFER_FRAME_OVERRUN:
					printk(KERN_INFO "  STATUS:HC_XFER_FRAME_OVERRUN\n");break;
				case HC_XFER_DATA_TOGGLE_ERR:
					printk(KERN_INFO "  STATUS:HC_XFER_DATA_TOGGLE_ERR\n");break;
				case HC_XFER_COMPLETE:
					printk(KERN_INFO "  STATUS:HC_XFER_COMPLETE\n");break;
				default:
					printk(KERN_INFO "  STATUS:KNOWN\n");break;
			}
		}
	#endif
}


static void release_channel(ifxhcd_hcd_t          *_ifxhcd,
                            ifxhcd_hc_t           *_ifxhc,
                            ifxhcd_halt_status_e  _halt_status)
{
	ifxusb_hc_regs_t *hc_regs = _ifxhcd->core_if.hc_regs[_ifxhc->hc_num];
	struct urb       *urb     = NULL;
	ifxhcd_epqh_t    *epqh    = NULL;
	ifxhcd_urbd_t    *urbd    = NULL;

	IFX_DEBUGPL(DBG_HCDV, "  %s: channel %d, halt_status %d\n",
		    __func__, _ifxhc->hc_num, _halt_status);

	epqh=_ifxhc->epqh;

	if(!epqh)
		IFX_ERROR("%s epqh=null\n",__func__);
	else
	{
		urbd=epqh->urbd;
		if(!urbd)
			IFX_ERROR("%s urbd=null\n",__func__);
		else
		{
			urb=urbd->urb;
			if(!urb)
				IFX_ERROR("%s urb =null\n",__func__);
			else {
				/* == AVM/WK 20100710 Fix - Use toggle of usbcore ==*/
				unsigned toggle = (read_data_toggle(hc_regs) == IFXUSB_HC_PID_DATA0)? 0: 1;
				usb_settoggle (urb->dev, usb_pipeendpoint (urb->pipe), usb_pipeout(urb->pipe), toggle);
			}
		}
		//epqh->data_toggle = read_data_toggle(hc_regs);

	}

	switch (_halt_status)
	{
		case HC_XFER_NO_HALT_STATUS:
			IFX_ERROR("%s: No halt_status, channel %d\n", __func__, _ifxhc->hc_num);
			break;
		case HC_XFER_COMPLETE:
			IFX_ERROR("%s: Inavalid halt_status HC_XFER_COMPLETE, channel %d\n", __func__, _ifxhc->hc_num);
			break;
		case HC_XFER_URB_COMPLETE:
		case HC_XFER_URB_DEQUEUE:
		case HC_XFER_AHB_ERR:
		case HC_XFER_XACT_ERR:
		case HC_XFER_FRAME_OVERRUN:
			if(urbd && urb) {
				/* == 20110803 AVM/WK FIX set status, if still in progress == */
				if (urb->status == -EINPROGRESS) {
					switch (_halt_status) {
					case HC_XFER_URB_COMPLETE:
						urb->status = 0;
						break;
					case HC_XFER_URB_DEQUEUE:
						urb->status = -ECONNRESET;
						break;
					case HC_XFER_AHB_ERR:
					case HC_XFER_XACT_ERR:
					case HC_XFER_FRAME_OVERRUN:
						urb->status = -EPROTO;
						break;
					default:
						break;
					}
				}
				/*== AVM/BC 20101111 Deferred Complete ==*/
				defer_ifxhcd_complete_urb(_ifxhcd, urbd, urb->status);
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			if(epqh)
				ifxhcd_epqh_idle(_ifxhcd, epqh);
			else
			{
				IFX_WARN("WARNING %s():%d epqh=%p\n",__func__,__LINE__,epqh);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}

			list_add_tail(&_ifxhc->hc_list_entry, &_ifxhcd->free_hc_list);
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
			break;
		case HC_XFER_STALL:
			release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			if(urbd)
				/*== AVM/BC 20101111 Deferred Complete ==*/
				defer_ifxhcd_complete_urb(_ifxhcd, urbd, -EPIPE);
			else
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
			if(epqh)
			{
//				epqh->data_toggle = 0;
				ifxhcd_epqh_idle(_ifxhcd, epqh);
			}
			else
				IFX_WARN("WARNING %s():%d epqh=%p\n",__func__,__LINE__,epqh);
			list_add_tail(&_ifxhc->hc_list_entry, &_ifxhcd->free_hc_list);
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
			break;
		case HC_XFER_NAK:
			release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			if(urbd)
			{
				//ifxhcd_complete_urb(_ifxhcd, urbd, -ETIMEDOUT);
				urb->status = 0;
				/*== AVM/BC 20101111 Deferred Complete ==*/
				defer_ifxhcd_complete_urb(_ifxhcd, urbd, urb->status);
			}
			else
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
			if(epqh)
				ifxhcd_epqh_idle(_ifxhcd, epqh);
			else
				IFX_WARN("WARNING %s():%d epqh=%p\n",__func__,__LINE__,epqh);
			list_add_tail(&_ifxhc->hc_list_entry, &_ifxhcd->free_hc_list);
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
			break;
		case HC_XFER_BABBLE_ERR:
		case HC_XFER_DATA_TOGGLE_ERR:
			release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			if(urbd)
				/*== AVM/BC 20101111 Deferred Complete ==*/
				defer_ifxhcd_complete_urb(_ifxhcd, urbd, -EOVERFLOW);
			else
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
			if(epqh)
				ifxhcd_epqh_idle(_ifxhcd, epqh);
			else
				IFX_WARN("WARNING %s():%d epqh=%p\n",__func__,__LINE__,epqh);
			list_add_tail(&_ifxhc->hc_list_entry, &_ifxhcd->free_hc_list);
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
			break;
	}
	select_eps(_ifxhcd);
}

/*
 * Updates the state of the URB after a Transfer Complete interrupt on the
 * host channel. Updates the actual_length field of the URB based on the
 * number of bytes transferred via the host channel. Sets the URB status
 * if the data transfer is finished.
 *
 * @return 1 if the data transfer specified by the URB is completely finished,
 * 0 otherwise.
 */
static int update_urb_state_xfer_comp(ifxhcd_hc_t       *_ifxhc,
                                      ifxusb_hc_regs_t  *_hc_regs,
                                      struct urb        *_urb,
                                      ifxhcd_urbd_t      *_urbd)
{
	int xfer_done  = 0;

	if (_ifxhc->is_in)
	{
		hctsiz_data_t hctsiz;
		hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
		_urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
		if ((hctsiz.b.xfersize != 0) || (_urb->actual_length >= _urb->transfer_buffer_length))
		{
			xfer_done = 1;
			_urb->status = 0;
			/* 20110805 AVM/WK Workaround: catch overflow error here, hardware does not */
			if (_urb->actual_length > _urb->transfer_buffer_length) {
				_urb->status = -EOVERFLOW;
			}
			#if 0
				if (_urb->actual_length < _urb->transfer_buffer_length && _urb->transfer_flags & URB_SHORT_NOT_OK)
				_urb->status = -EREMOTEIO;
			#endif
		}

	}
	else
	{
		if (_ifxhc->split)
			_urb->actual_length +=  _ifxhc->ssplit_out_xfer_count;
		else
			_urb->actual_length +=  _ifxhc->xfer_len;

		if (_urb->actual_length >= _urb->transfer_buffer_length)
		{
			/*== AVM/BC WK 20110421 ZERO PACKET Workaround ==*/
			if ((_ifxhc->short_rw == 1) && ( _ifxhc->xfer_len > 0) && ( _ifxhc->xfer_len % _ifxhc->mps == 0 ))
			{
				_ifxhc->short_rw = 0;
				//Transfer not finished. Another iteration for ZLP.
			}
			else
			{
				xfer_done = 1;
			}
			_urb->status = 0;
		}
	}

	#ifdef __DEBUG__
		{
			hctsiz_data_t 	hctsiz;
			hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
			IFX_DEBUGPL(DBG_HCDV, "IFXUSB: %s: %s, channel %d\n",
				    __func__, (_ifxhc->is_in ? "IN" : "OUT"), _ifxhc->hc_num);
			IFX_DEBUGPL(DBG_HCDV, "  hc->xfer_len %d\n", _ifxhc->xfer_len);
			IFX_DEBUGPL(DBG_HCDV, "  hctsiz.xfersize %d\n", hctsiz.b.xfersize);
			IFX_DEBUGPL(DBG_HCDV, "  urb->transfer_buffer_length %d\n",
				    _urb->transfer_buffer_length);
			IFX_DEBUGPL(DBG_HCDV, "  urb->actual_length %d\n", _urb->actual_length);
		}
	#endif
	return xfer_done;
}

/*== AVM/BC 20101111 Function called with Lock ==*/

void complete_channel(ifxhcd_hcd_t        *_ifxhcd,
                            ifxhcd_hc_t          *_ifxhc,
                            ifxhcd_urbd_t        *_urbd)
{
	ifxusb_hc_regs_t *hc_regs = _ifxhcd->core_if.hc_regs[_ifxhc->hc_num];
	struct urb    *urb  = NULL;
	ifxhcd_epqh_t *epqh = NULL;
	int urb_xfer_done;

	IFX_DEBUGPL(DBG_HCD, "--Complete Channel %d : \n", _ifxhc->hc_num);

	if(!_urbd)
	{
		IFX_ERROR("ERROR %s():%d urbd=%p\n",__func__,__LINE__,_urbd);
		return;
	}

	urb  = _urbd->urb;
	epqh = _urbd->epqh;

	if(!urb || !epqh)
	{
		IFX_ERROR("ERROR %s():%d urb=%p epqh=%p\n",__func__,__LINE__,urb,epqh);
		return;
	}

	_ifxhc->do_ping=0;

	if (_ifxhc->split)
		_ifxhc->split = 1;

	switch (epqh->ep_type)
	{
		case IFXUSB_EP_TYPE_CTRL:
			switch (_ifxhc->control_phase)
			{
				case IFXHCD_CONTROL_SETUP:
					IFX_DEBUGPL(DBG_HCDV, "  Control setup transaction done\n");
					if (_urbd->xfer_len > 0)
					{
						_ifxhc->control_phase = IFXHCD_CONTROL_DATA;
						_ifxhc->is_in         = _urbd->is_in;
						_ifxhc->xfer_len      = _urbd->xfer_len;
						#if   defined(__UNALIGNED_BUFFER_ADJ__)
							if(epqh->using_aligned_buf)
								_ifxhc->xfer_buff      = epqh->aligned_buf;
							else
						#endif
								_ifxhc->xfer_buff      = _urbd->xfer_buff;
					}
					else
					{
						_ifxhc->control_phase = IFXHCD_CONTROL_STATUS;
						_ifxhc->is_in          = 1;
						_ifxhc->xfer_len       = 0;
						_ifxhc->xfer_buff      = _ifxhcd->status_buf;
					}
					if(_ifxhc->is_in)
						_ifxhc->short_rw       =0;
					else
						_ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
					_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
					_ifxhc->xfer_count     = 0;
					_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
					/*== AVM/BC 20101111 Lock not needed ==*/
					process_channels_sub(_ifxhcd);
					break;
				case IFXHCD_CONTROL_DATA:
					urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
					if (urb_xfer_done)
					{
						_ifxhc->control_phase  = IFXHCD_CONTROL_STATUS;
						_ifxhc->is_in          = (_urbd->is_in)?0:1;
						_ifxhc->xfer_len       = 0;
						_ifxhc->xfer_count     = 0;
						_ifxhc->xfer_buff      = _ifxhcd->status_buf;
						_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
						_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
						if(_ifxhc->is_in)
							_ifxhc->short_rw       =0;
						else
							_ifxhc->short_rw       =1;
					}
					else // continue
					{
						_ifxhc->xfer_len       = _urbd->xfer_len - urb->actual_length;
						_ifxhc->xfer_count     = urb->actual_length;
						_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
						_ifxhc->data_pid_start = read_data_toggle(hc_regs);
					}
					/*== AVM/BC 20101111 Lock not needed ==*/
					process_channels_sub(_ifxhcd);
					break;
				case IFXHCD_CONTROL_STATUS:
					if (urb->status == -EINPROGRESS)
						urb->status = 0;
					release_channel(_ifxhcd,_ifxhc,HC_XFER_URB_COMPLETE);
					break;
			}
			break;
		case IFXUSB_EP_TYPE_BULK:
			IFX_DEBUGPL(DBG_HCDV, "  Bulk transfer complete\n");
			urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
			if (urb_xfer_done)
				release_channel(_ifxhcd,_ifxhc,HC_XFER_URB_COMPLETE);
			else
			{
				_ifxhc->xfer_len       = _urbd->xfer_len - urb->actual_length;
				_ifxhc->xfer_count     = urb->actual_length;
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				_ifxhc->data_pid_start = read_data_toggle(hc_regs);
				/*== AVM/BC 20101111 Lock not needed ==*/
				process_channels_sub(_ifxhcd);
			}
			break;
		case IFXUSB_EP_TYPE_INTR:
			urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
			release_channel(_ifxhcd,_ifxhc,HC_XFER_URB_COMPLETE);
			break;
		case IFXUSB_EP_TYPE_ISOC:
//			if (_urbd->isoc_split_pos == IFXUSB_HCSPLIT_XACTPOS_ALL)
//				halt_status = update_isoc_urb_state(_ifxhcd, _ifxhc, hc_regs, _urbd, HC_XFER_COMPLETE);
//			complete_periodic_xfer(_ifxhcd, _ifxhc, hc_regs, _urbd, halt_status);
			urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
			release_channel(_ifxhcd,_ifxhc,HC_XFER_URB_COMPLETE);
			break;
	}
}



void showint(uint32_t val_hcint
            ,uint32_t val_hcintmsk
            ,uint32_t val_hctsiz)
{
#ifdef __DEBUG__
	hcint_data_t  hcint    = {.d32 = val_hcint};
	hcint_data_t  hcintmsk = {.d32 = val_hcintmsk};

	printk(KERN_INFO "   WITH FLAG: Sz:%08x I:%08X/M:%08X %s%s%s%s%s%s%s%s%s%s\n"
		,val_hctsiz,hcint.d32 ,hcintmsk.d32
		,(hcint.b.datatglerr || hcintmsk.b.datatglerr)?
		 (
		   (hcint.b.datatglerr && hcintmsk.b.datatglerr)?"datatglerr[*/*] ":
		    (
		      (hcint.b.datatglerr)?"datatglerr[*/] ":"datatglerr[/*] "
		    )
		 )
		 :""
		,(hcint.b.frmovrun || hcintmsk.b.frmovrun)?
		 (
		   (hcint.b.frmovrun && hcintmsk.b.frmovrun)?"frmovrun[*/*] ":
		    (
		      (hcint.b.frmovrun)?"frmovrun[*/] ":"frmovrun[/*] "
		    )
		 )
		 :""
		,(hcint.b.bblerr || hcintmsk.b.bblerr)?
		 (
		   (hcint.b.bblerr && hcintmsk.b.bblerr)?"bblerr[*/*] ":
		    (
		      (hcint.b.bblerr)?"bblerr[*/] ":"bblerr[/*] "
		    )
		 )
		 :""
		,(hcint.b.xacterr || hcintmsk.b.xacterr)?
		 (
		   (hcint.b.xacterr && hcintmsk.b.xacterr)?"xacterr[*/*] ":
		    (
		      (hcint.b.xacterr)?"xacterr[*/] ":"xacterr[/*] "
		    )
		 )
		 :""
		,(hcint.b.nyet || hcintmsk.b.nyet)?
		 (
		   (hcint.b.nyet && hcintmsk.b.nyet)?"nyet[*/*] ":
		    (
		      (hcint.b.nyet)?"nyet[*/] ":"nyet[/*] "
		    )
		 )
		 :""
		,(hcint.b.nak || hcintmsk.b.nak)?
		 (
		   (hcint.b.nak && hcintmsk.b.nak)?"nak[*/*] ":
		    (
		      (hcint.b.nak)?"nak[*/] ":"nak[/*] "
		    )
		 )
		 :""
		,(hcint.b.ack || hcintmsk.b.ack)?
		 (
		   (hcint.b.ack && hcintmsk.b.ack)?"ack[*/*] ":
		    (
		      (hcint.b.ack)?"ack[*/] ":"ack[/*] "
		    )
		 )
		 :""
		,(hcint.b.stall || hcintmsk.b.stall)?
		 (
		   (hcint.b.stall && hcintmsk.b.stall)?"stall[*/*] ":
		    (
		      (hcint.b.stall)?"stall[*/] ":"stall[/*] "
		    )
		 )
		 :""
		,(hcint.b.ahberr || hcintmsk.b.ahberr)?
		 (
		   (hcint.b.ahberr && hcintmsk.b.ahberr)?"ahberr[*/*] ":
		    (
		      (hcint.b.ahberr)?"ahberr[*/] ":"ahberr[/*] "
		    )
		 )
		 :""
		,(hcint.b.xfercomp || hcintmsk.b.xfercomp)?
		 (
		   (hcint.b.xfercomp && hcintmsk.b.xfercomp)?"xfercomp[*/*] ":
		    (
		      (hcint.b.xfercomp)?"xfercomp[*/] ":"xfercomp[/*] "
		    )
		 )
		 :""
	);
#endif
}


extern void ifxhcd_hc_dumb_rx(ifxusb_core_if_t *_core_if, ifxhcd_hc_t *_ifxhc,uint8_t   *dump_buf);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_rx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                        ifxhcd_hc_t       *_ifxhc,
                                        ifxusb_hc_regs_t  *_hc_regs,
                                        ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);
	_ifxhc->do_ping        = 0;

	if(_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->nak_retry_r)
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			_ifxhc->nak_retry--;
			if(_ifxhc->nak_retry)
			{
				_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count         = _urbd->urb->actual_length;
				_ifxhc->data_pid_start     = read_data_toggle(_hc_regs);
				_ifxhc->wait_for_sof   = 1;
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			else
			{
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
			}
		}
		else
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count         = _urbd->urb->actual_length;
			_ifxhc->data_pid_start     = read_data_toggle(_hc_regs);
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}

	if (hcint.b.xfercomp)
	{
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		// ZLP shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
		// Stall FIFO compensation.
		#if 0
			int sz1,sz2;
			sz2=_ifxhc->start_pkt_count - hctsiz.b.pktcnt;
			sz2*=_ifxhc->mps;
			sz1=_ifxhc->xfer_len - hctsiz.b.xfersize;
			sz2-=sz1;
			if(sz2)
				ifxhcd_hc_dumb_rx(&_ifxhcd->core_if, _ifxhc,_ifxhc->epqh->dump_buf);
		#endif
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if (hcint.b.bblerr)
	{
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;

		// ZLP shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			_ifxhc->wait_for_sof   =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count         = _urbd->urb->actual_length;
			_ifxhc->data_pid_start     = read_data_toggle(_hc_regs);

			/* 20110803 AVM/WK FIX: Reset error count on any handshake */
			if (hcint.b.nak || hcint.b.nyet || hcint.b.ack) {
				_urbd->error_count = 1;
			} else {
				_urbd->error_count++;
			}

			if (_urbd->error_count >= 3)
			{
				_urbd->error_count     =0;
				_ifxhc->wait_for_sof   =0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->wait_for_sof   = 1;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
		#if 1
			if(_ifxhc->data_pid_start == IFXUSB_HC_PID_DATA0)
				_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
			else
				_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		#else
			release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		#endif
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT0 FRMOVRUN [should be Period only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.nyet   )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT0 NYET  [should be Out only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                        ifxhcd_hc_t       *_ifxhc,
                                        ifxusb_hc_regs_t  *_hc_regs,
                                        ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

#ifdef __DEBUG__
static int first=0;
#endif

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

#ifdef __DEBUG__
if(!first&& _ifxhc->ep_type == IFXUSB_EP_TYPE_BULK
   &&(hcint.b.stall || hcint.b.datatglerr || hcint.b.frmovrun || hcint.b.bblerr || hcint.b.xacterr) && !hcint.b.ack)
{
	showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	first=1;
	printk(KERN_INFO "   [%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X] \n"
	,*(_ifxhc->xfer_buff+ 0),*(_ifxhc->xfer_buff+ 1),*(_ifxhc->xfer_buff+ 2),*(_ifxhc->xfer_buff+ 3)
	,*(_ifxhc->xfer_buff+ 4),*(_ifxhc->xfer_buff+ 5),*(_ifxhc->xfer_buff+ 6),*(_ifxhc->xfer_buff+ 7)
	,*(_ifxhc->xfer_buff+ 8),*(_ifxhc->xfer_buff+ 9),*(_ifxhc->xfer_buff+10),*(_ifxhc->xfer_buff+11)
	,*(_ifxhc->xfer_buff+12),*(_ifxhc->xfer_buff+13),*(_ifxhc->xfer_buff+14),*(_ifxhc->xfer_buff+15));

	printk(KERN_INFO "   [_urbd->urb->actual_length:%08X _ifxhc->start_pkt_count:%08X hctsiz.b.pktcnt:%08X ,_urbd->xfer_len:%08x] \n"
	,_urbd->urb->actual_length
	,_ifxhc->start_pkt_count
	,hctsiz.b.pktcnt
	,_urbd->xfer_len);
}
#endif

	if(_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->nak_retry_r)
		{
			_ifxhc->nak_retry--;
			if(_ifxhc->nak_retry)
			{
				if(_ifxhc->xfer_len!=0)
					_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
				_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count     = _urbd->urb->actual_length;
				_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
				_ifxhc->wait_for_sof   = 1;
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			else
			{
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
			}
		}
		else
		{
			if(_ifxhc->xfer_len!=0)
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}

	if (hcint.b.xfercomp)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
		disable_hc_int(_hc_regs,nyet);
		_urbd->error_count     =0;
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && hcint.b.nak)
		{
			// Walkaround: When sending ZLP and receive NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			if(hcint.b.nyet && !out_nak_enh)
				_ifxhc->do_ping        = 1;
			else
				_ifxhc->do_ping        = 0;
			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->wait_for_sof   = 1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		{
			_ifxhc->wait_for_sof   = 0;
			_ifxhc->do_ping        = 0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.stall)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
		disable_hc_int(_hc_regs,nyet);
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		_ifxhc->do_ping        =0;

		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			if(_ifxhc->xfer_len!=0)
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
			_urbd->error_count     =0;
			_ifxhc->wait_for_sof   =0;
			_ifxhc->do_ping        =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			if(_ifxhc->xfer_len!=0)
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->data_pid_start = read_data_toggle(_hc_regs);

			if (hcint.b.nak || hcint.b.nyet || hcint.b.ack)
			{
				_urbd->error_count     =0;
				_ifxhc->wait_for_sof   =1;
				enable_hc_int(_hc_regs,ack);
				enable_hc_int(_hc_regs,nak);
				enable_hc_int(_hc_regs,nyet);
				if(!out_nak_enh)
					_ifxhc->do_ping        =1;
				else
					_ifxhc->do_ping        =0;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			else
			{
				_urbd->error_count ++ ;
				if (_urbd->error_count == 3)
				{
					disable_hc_int(_hc_regs,ack);
					disable_hc_int(_hc_regs,nak);
					disable_hc_int(_hc_regs,nyet);
					_urbd->error_count     =0;
					_ifxhc->wait_for_sof   =0;
					_ifxhc->do_ping        =0;
					release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
				}
				else
				{
					enable_hc_int(_hc_regs,ack);
					enable_hc_int(_hc_regs,nak);
					enable_hc_int(_hc_regs,nyet);
					_ifxhc->wait_for_sof   =1;
					if(!out_nak_enh)
						_ifxhc->do_ping        =1;
					else
						_ifxhc->do_ping        =0;
					ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
				}
			}
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
IFX_WARN("%s() %d Warning CTRLBULK OUT SPLIT0 BABBLE [should be IN only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_ifxhc->do_ping        = 0;
		if(_ifxhc->xfer_len!=0)
			_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.nak || hcint.b.nyet)
	{
		if(!out_nak_enh)
		{
			// ZLP shortcut
			#if 1
			if(hctsiz.b.pktcnt==0)
			{
				_urbd->error_count     =0;
				_ifxhc->wait_for_sof   =0;
				_ifxhc->do_ping        =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else
			#endif
			{
				if(!out_nak_enh)
					_ifxhc->do_ping        =1;
				else
					_ifxhc->do_ping        =0;
				if(_ifxhc->xfer_len!=0)
				{
					_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
					_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
					_ifxhc->xfer_count         = _urbd->urb->actual_length;
				}
				_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
				_ifxhc->wait_for_sof   = 1;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			return 1;
		}
	}
	else if(hcint.b.datatglerr )
	{
IFX_WARN("%s() %d Warning CTRLBULK OUT SPLIT0 DATATGLERR [should be IN only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		_ifxhc->do_ping        =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
IFX_WARN("%s() %d Warning CTRLBULK OUT SPLIT0 FRMOVRUN [should be PERIODIC only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		_ifxhc->do_ping        =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_rx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t       *_ifxhc,
                                    ifxusb_hc_regs_t  *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);
	_ifxhc->do_ping        =0;

	if(_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->nak_retry_r)
		{
			_ifxhc->nak_retry--;
			if(_ifxhc->nak_retry)
			{
				if(_ifxhc->xfer_len!=0)
					_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
				_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count     = _urbd->urb->actual_length;
				_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
				_ifxhc->wait_for_sof   = 1;
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			else
			{
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
			}
		}
		else
		{
			if(_ifxhc->xfer_len!=0)
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}

	if(hcint.b.xfercomp   )
	{
		_urbd->error_count   =0;
		//restart INTR immediately
		#if 1
		if(hctsiz.b.pktcnt>0)
		{
			// TODO Re-initialize Channel (in next b_interval - 1 uF/F)
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		#endif
		{
			_ifxhc->wait_for_sof =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;

		// Don't care shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			// Stall FIFO compensation.
			#if 0
				int sz1,sz2;
				sz2=_ifxhc->start_pkt_count - hctsiz.b.pktcnt;
				sz2*=_ifxhc->mps;
				sz1=_ifxhc->xfer_len - hctsiz.b.xfersize;
				sz2-=sz1;
				if(sz2)
					ifxhcd_hc_dumb_rx(&_ifxhcd->core_if, _ifxhc,_ifxhc->epqh->dump_buf);
			#endif
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}


	else if (hcint.b.bblerr)
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;

		// Don't care shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		}
		return 1;
	}
	else if (hcint.b.nak || hcint.b.datatglerr || hcint.b.frmovrun)
	{
		_urbd->error_count   =0;
		//restart INTR immediately
		#if 1
		if(hctsiz.b.pktcnt>0)
		{
			// TODO Re-initialize Channel (in next b_interval - 1 uF/F)
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		#endif
		{
			_ifxhc->wait_for_sof =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			_ifxhc->wait_for_sof   =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			/* 20110803 AVM/WK FIX: Reset error count on any handshake */
			if (hcint.b.nak || hcint.b.nyet || hcint.b.ack) {
				_urbd->error_count = 1;
			} else {
				_urbd->error_count++;
			}

			if(_urbd->error_count>=3)
			{
				_urbd->error_count     =0;
				_ifxhc->wait_for_sof   =0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
		}
		return 1;
	}
	else if(hcint.b.nyet   )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT0 NYET [should be OUT only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t       *_ifxhc,
                                    ifxusb_hc_regs_t  *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

	if(_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->nak_retry_r)
		{
			_ifxhc->nak_retry--;
			if(_ifxhc->nak_retry)
			{
				if(_ifxhc->xfer_len!=0)
					_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
				_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count     = _urbd->urb->actual_length;
				_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
				_ifxhc->wait_for_sof   = 1;
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			else
			{
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
			}
		}
		else
		{
			if(_ifxhc->xfer_len!=0)
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}

	if(hcint.b.xfercomp   )
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
		disable_hc_int(_hc_regs,nyet);
		_urbd->error_count   =0;
		//restart INTR immediately
		#if 0
		if(hctsiz.b.pktcnt>0)
		{
			// TODO Re-initialize Channel (in next b_interval - 1 uF/F)
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			if(hcint.b.nyet && !out_nak_enh  )
				_ifxhc->do_ping        =1;
			else
				_ifxhc->do_ping        =0;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		#endif
		{
			_ifxhc->wait_for_sof =0;
			_ifxhc->do_ping      =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.stall)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nyet);
		disable_hc_int(_hc_regs,nak);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		_ifxhc->do_ping      =0;

		// Don't care shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			if(_ifxhc->xfer_len!=0)// !_ifxhc->is_in
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if(hcint.b.nak || hcint.b.frmovrun )
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nyet);
		disable_hc_int(_hc_regs,nak);
		_urbd->error_count   =0;
		//restart INTR immediately
		#if 0
		if(hctsiz.b.pktcnt>0)
		{
			// TODO Re-initialize Channel (in next b_interval - 1 uF/F)
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			if(!out_nak_enh  )
				_ifxhc->do_ping        =1;
			else
				_ifxhc->do_ping        =0;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		#endif
		{
			_ifxhc->wait_for_sof =0;
			_ifxhc->do_ping      =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			_ifxhc->wait_for_sof   =0;
			_ifxhc->do_ping        =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			/* 20110803 AVM/WK FIX: Reset error count on any handshake */
			if (hcint.b.nak || hcint.b.nyet || hcint.b.ack) {
				_urbd->error_count = 1;
			} else {
				_urbd->error_count++;
			}

			if(_urbd->error_count>=3)
			{
				_urbd->error_count     =0;
				_ifxhc->wait_for_sof   =0;
				_ifxhc->do_ping        =0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				//_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
				//if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
				_ifxhc->wait_for_sof=1;
				if(!out_nak_enh  )
					_ifxhc->do_ping        =1;
				else
					_ifxhc->do_ping        =0;

				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
IFX_WARN("%s() %d Warning INTR OUT SPLIT0 BABBLEERR  [should be IN only]\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		_ifxhc->do_ping        =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
IFX_WARN("%s() %d Warning INTR OUT SPLIT0 DATATGLERR\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count     =0;
		_ifxhc->wait_for_sof   =0;
		_ifxhc->do_ping        =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_rx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t       *_ifxhc,
                                    ifxusb_hc_regs_t  *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

		if (hcint.b.xfercomp || hcint.b.frmovrun)
		{
			_urbd->error_count=0;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
			_ifxhc->wait_for_sof   = 0;
			if (hcint.b.xfercomp)
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			else
				release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		}
		else if (hcint.b.xacterr || hcint.b.bblerr)
		{
			#ifndef VR9Skip
				if(hctsiz.b.pktcnt==0)
				{
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					int sz1,sz2;
					sz2=_ifxhc->start_pkt_count - hctsiz.b.pktcnt;
					sz2*=_ifxhc->mps;
					sz1=_ifxhc->xfer_len - hctsiz.b.xfersize;
					sz2-=sz1;
					if(sz2)
						ifxhcd_hc_dumb_rx(&_ifxhcd->core_if, _ifxhc,_ifxhc->epqh->dump_buf);
					_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
					_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
					_ifxhc->xfer_count         = _urbd->urb->actual_length;
					_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
					_urbd->error_count++;
					if(_urbd->error_count>=3)
					{
						_urbd->error_count=0;
						_ifxhc->wait_for_sof   = 0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
						release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						_ifxhc->wait_for_sof   = 1;
						enable_hc_int(_hc_regs,ack);
						enable_hc_int(_hc_regs,nak);
						enable_hc_int(_hc_regs,nyet);
						ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
					}
				}
			#endif
		}
		else if(hcint.b.datatglerr )
		{
			warning
		}
		else if(hcint.b.stall      )
		{
			warning
		}
	#else
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t       *_ifxhc,
                                    ifxusb_hc_regs_t  *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;
		int out_nak_enh = 0;

		if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
			out_nak_enh = 1;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

		if (hcint.b.xfercomp)
		{
			_urbd->error_count=0;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
			_ifxhc->wait_for_sof   = 0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
			return 1;
		}
		else if (hcint.b.frmovrun)
		{
			#ifndef VR9Skip
				_urbd->error_count=0;
				disable_hc_int(_hc_regs,ack);
				disable_hc_int(_hc_regs,nak);
				disable_hc_int(_hc_regs,nyet);
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
			#endif
		}
		else if(hcint.b.datatglerr )
		{
			warning
		}
		else if(hcint.b.bblerr     )
		{
			#ifndef VR9Skip
				if(hctsiz.b.pktcnt==0)
				{
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					int sz1,sz2;
					sz2=_ifxhc->start_pkt_count - hctsiz.b.pktcnt;
					sz2*=_ifxhc->mps;
					sz1=_ifxhc->xfer_len - hctsiz.b.xfersize;
					sz2-=sz1;
						if(sz2)
							ifxhcd_hc_dumb_rx(&_ifxhcd->core_if, _ifxhc,_ifxhc->epqh->dump_buf);
						_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
					_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
					_ifxhc->xfer_count         = _urbd->urb->actual_length;
					_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
					_urbd->error_count++;
					if(_urbd->error_count>=3)
					{
						_urbd->error_count=0;
						_ifxhc->wait_for_sof   = 0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
					}
					else
					{
						_ifxhc->wait_for_sof   = 1;
						enable_hc_int(_hc_regs,ack);
						enable_hc_int(_hc_regs,nak);
						enable_hc_int(_hc_regs,nyet);
						ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
					}
				}
			#endif
		}
		else if(hcint.b.xacterr    )
		{
			if(hctsiz.b.pktcnt==0)
			{
				complete_channel(_ifxhcd, _ifxhc, _urbd);
				return 1;
			}
			_urbd->error_count++;
			if(_urbd->error_count>=3)
			{
				_urbd->error_count=0;
				_ifxhc->wait_for_sof   = 0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->wait_for_sof   = 1;
				ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			}
			return 1;
		}
		else if(hcint.b.stall      )
		{
				warning
		}
	#else
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_rx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                      ifxhcd_hc_t       *_ifxhc,
                                      ifxusb_hc_regs_t  *_hc_regs,
                                      ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

	_ifxhc->do_ping        =0;

	if (hcint.b.ack)
	{
		_urbd->error_count=0;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   = 8;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_ifxhc->wait_for_sof   = 1;
		_urbd->error_count     = 0;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->wait_for_sof =1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT1 HC_XFER_DATA_TOGGLE_ERR\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT1 HC_XFER_FRAME_OVERRUN\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.nyet   )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT1 NYET\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	}
	else if(hcint.b.xfercomp   )
	{
IFX_WARN("%s() %d Warning CTRLBULK IN SPLIT1 COMPLETE\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_tx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                      ifxhcd_hc_t       *_ifxhc,
                                      ifxusb_hc_regs_t  *_hc_regs,
                                      ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

#ifdef __DEBUG__
static int first=0;
#endif

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

#ifdef __DEBUG__
	if(!first&& _ifxhc->ep_type == IFXUSB_EP_TYPE_BULK
	   &&(hcint.b.stall || hcint.b.datatglerr || hcint.b.frmovrun || hcint.b.bblerr || hcint.b.xacterr) && !hcint.b.ack)
	{
		showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		first=1;
		printk(KERN_INFO "   [%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X] \n"
		,*(_ifxhc->xfer_buff+ 0),*(_ifxhc->xfer_buff+ 1),*(_ifxhc->xfer_buff+ 2),*(_ifxhc->xfer_buff+ 3)
		,*(_ifxhc->xfer_buff+ 4),*(_ifxhc->xfer_buff+ 5),*(_ifxhc->xfer_buff+ 6),*(_ifxhc->xfer_buff+ 7)
		,*(_ifxhc->xfer_buff+ 8),*(_ifxhc->xfer_buff+ 9),*(_ifxhc->xfer_buff+10),*(_ifxhc->xfer_buff+11)
		,*(_ifxhc->xfer_buff+12),*(_ifxhc->xfer_buff+13),*(_ifxhc->xfer_buff+14),*(_ifxhc->xfer_buff+15));

		printk(KERN_INFO "   [_urbd->urb->actual_length:%08X _ifxhc->start_pkt_count:%08X hctsiz.b.pktcnt:%08X ,_urbd->xfer_len:%08x] \n"
		,_urbd->urb->actual_length
		,_ifxhc->start_pkt_count
		,hctsiz.b.pktcnt
		,_urbd->xfer_len);
	}
#endif

	if     (hcint.b.ack )
	{
		_urbd->error_count=0;
		if (_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK || _ifxhc->control_phase != IFXHCD_CONTROL_SETUP)
			_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   =8;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
IFX_WARN("%s() %d Warning CTRLBULK OUT SPLIT1 NYET\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count=0;
		if (_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK || _ifxhc->control_phase != IFXHCD_CONTROL_SETUP)
			_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   =1;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->wait_for_sof  =1;
		if(!out_nak_enh  )
			_ifxhc->do_ping        =1;
		else
			_ifxhc->do_ping        =0;
		_urbd->error_count    =0;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof  =0;
			_ifxhc->do_ping       =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->wait_for_sof  =1;
			_ifxhc->do_ping       =1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		_ifxhc->do_ping      =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		_ifxhc->do_ping      =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		_ifxhc->do_ping      =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
IFX_WARN("%s() %d Warning CTRLBULK OUT SPLIT1 HC_XFER_FRAME_OVERRUN\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		_ifxhc->do_ping      =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
		printk(KERN_INFO "%s() %d Warning CTRLBULK OUT SPLIT1 COMPLETE\n",__func__,__LINE__);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_rx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t       *_ifxhc,
                                  ifxusb_hc_regs_t  *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

	_ifxhc->do_ping        =0;

	if     (hcint.b.ack )
	{
		/*== AVM/BC 20100701 - Workaround FullSpeed Interrupts with HiSpeed Hub ==*/
		_ifxhc->nyet_count=0;

		_urbd->error_count=0;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		_urbd->error_count=0;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		hcchar_data_t 	hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		_urbd->error_count=hcchar.b.multicnt;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
IFX_WARN( "%s() %d Warning INTR IN SPLIT1 DATATGLERR\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT1 COMPLETE\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_tx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t       *_ifxhc,
                                  ifxusb_hc_regs_t  *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

	if     (hcint.b.ack )
	{
		/*== AVM/BC 20100701 - Workaround FullSpeed Interrupts with HiSpeed Hub ==*/
		_ifxhc->nyet_count=0;

		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
IFX_WARN("%s() %d Warning INTR OUT SPLIT1 NYET\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		_urbd->error_count   =0;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		hcchar_data_t 	hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		_urbd->error_count=hcchar.b.multicnt;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			enable_hc_int(_hc_regs,ack);
			enable_hc_int(_hc_regs,nak);
			enable_hc_int(_hc_regs,nyet);
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT1 DATATGLERR\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT1 BABBLEERR\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT1 STALL\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
IFX_WARN("%s() %d Warning INTR IN SPLIT1 COMPLETE\n",__func__,__LINE__);
showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_rx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t       *_ifxhc,
                                   ifxusb_hc_regs_t  *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
		if     (hcint.b.ack )
		{
			Do Complete Split
		}
		else if(hcint.b.frmovrun   )
		{
			Rewind Buffer Pointers
			Retry Start Split (in next b_interval ¡V 1 uF)
		}
		else if(hcint.b.datatglerr )
		{
			warning
		}
		else if(hcint.b.bblerr     )
		{
			warning
		}
		else if(hcint.b.xacterr    )
		{
			warning
		}
		else if(hcint.b.stall      )
		{
			warning
		}
		else if(hcint.b.nak        )
		{
			warning
		}
		else if(hcint.b.xfercomp   )
		{
			warning
		}
		else if(hcint.b.nyet)
		{
			warning
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_tx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t       *_ifxhc,
                                   ifxusb_hc_regs_t  *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;
		int out_nak_enh = 0;

		if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
			out_nak_enh = 1;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
		if     (hcint.b.ack )
		{
			Do Next Start Split (in next b_interval ¡V 1 uF)
		}
		else if(hcint.b.frmovrun   )
		{
			Do Next Transaction in next frame.
		}
		else if(hcint.b.datatglerr )
		{
			warning
		}
		else if(hcint.b.bblerr     )
		{
			warning
		}
		else if(hcint.b.xacterr    )
		{
			warning
		}
		else if(hcint.b.stall      )
		{
			warning
		}
		else if(hcint.b.nak        )
		{
			warning
		}
		else if(hcint.b.xfercomp   )
		{
			warning
		}
		else if(hcint.b.nyet)
		{
			warning
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_rx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                      ifxhcd_hc_t       *_ifxhc,
                                      ifxusb_hc_regs_t  *_hc_regs,
                                      ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

	_ifxhc->do_ping        = 0;

	if (hcint.b.xfercomp)
	{
		_urbd->error_count   =0;
		_ifxhc->wait_for_sof = 0;
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_urbd->error_count=0;

		_ifxhc->split          = 1;
		_ifxhc->wait_for_sof   = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->wait_for_sof   = 1;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall || hcint.b.bblerr )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		if     (hcint.b.stall)
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		else if(hcint.b.bblerr )
			release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->wait_for_sof   = 1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		if(_ifxhc->data_pid_start == IFXUSB_HC_PID_DATA0)
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
		else
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
		_ifxhc->split=1;
		_ifxhc->wait_for_sof   = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrlbulk_tx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                      ifxhcd_hc_t       *_ifxhc,
                                      ifxusb_hc_regs_t  *_hc_regs,
                                      ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

#if 1
static int first=0;
#endif

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

#if 1
	if(!first&& _ifxhc->ep_type == IFXUSB_EP_TYPE_BULK
	   &&(hcint.b.stall || hcint.b.datatglerr || hcint.b.frmovrun || hcint.b.bblerr || hcint.b.xacterr) && !hcint.b.ack)
	{
		showint( hcint.d32,hcintmsk.d32,hctsiz.d32);
		first=1;
		printk(KERN_INFO "   [%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X] \n"
		,*(_ifxhc->xfer_buff+ 0),*(_ifxhc->xfer_buff+ 1),*(_ifxhc->xfer_buff+ 2),*(_ifxhc->xfer_buff+ 3)
		,*(_ifxhc->xfer_buff+ 4),*(_ifxhc->xfer_buff+ 5),*(_ifxhc->xfer_buff+ 6),*(_ifxhc->xfer_buff+ 7)
		,*(_ifxhc->xfer_buff+ 8),*(_ifxhc->xfer_buff+ 9),*(_ifxhc->xfer_buff+10),*(_ifxhc->xfer_buff+11)
		,*(_ifxhc->xfer_buff+12),*(_ifxhc->xfer_buff+13),*(_ifxhc->xfer_buff+14),*(_ifxhc->xfer_buff+15));

		printk(KERN_INFO "   [_urbd->urb->actual_length:%08X _ifxhc->start_pkt_count:%08X hctsiz.b.pktcnt:%08X ,_urbd->xfer_len:%08x] \n"
		,_urbd->urb->actual_length
		,_ifxhc->start_pkt_count
		,hctsiz.b.pktcnt
		,_urbd->xfer_len);
	}
#endif

	if(hcint.b.xfercomp   )
	{
		_urbd->error_count=0;
		_ifxhc->split=1;
		_ifxhc->do_ping= 0;
		#if 0
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && (hcint.b.nak || hcint.b.nyet))
		{
			// Walkaround: When sending ZLP and receive NYEY or NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->wait_for_sof   = 1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		else
		#endif
		{
			_ifxhc->wait_for_sof   = 0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count=0;

		_ifxhc->split          = 1;
		_ifxhc->wait_for_sof   = 1;
		if(!out_nak_enh  )
			_ifxhc->do_ping        =1;
		else
			_ifxhc->do_ping        =0;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		//Retry Complete Split
		// Issue Retry instantly on next SOF, without gothrough process_channels
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->wait_for_sof   = 1;
		_ifxhc->do_ping        = 0;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			_ifxhc->do_ping        = 0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->wait_for_sof   = 1;
			if(!out_nak_enh  )
				_ifxhc->do_ping        =1;
			else
				_ifxhc->do_ping        =0;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		if(_ifxhc->data_pid_start == IFXUSB_HC_PID_DATA0)
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
		else
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
		_ifxhc->split=1;
		_ifxhc->wait_for_sof   = 1;
		if(!out_nak_enh  )
			_ifxhc->do_ping        =1;
		else
			_ifxhc->do_ping        =0;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_rx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t       *_ifxhc,
                                  ifxusb_hc_regs_t  *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);
	_ifxhc->do_ping        = 0;

	if (hcint.b.xfercomp   )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count=0;
		_ifxhc->split          = 1;
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->wait_for_sof   = 0;

		/*== AVM/BC 20100701 - Workaround FullSpeed Interrupts with HiSpeed Hub ==*/
		_ifxhc->nyet_count++;
		if(_ifxhc->nyet_count > 2) {
			_ifxhc->split = 1;
			_ifxhc->nyet_count = 0;
			_ifxhc->wait_for_sof   = 5;
		}

		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun || hcint.b.bblerr || hcint.b.stall )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		if     (hcint.b.stall)
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		else if(hcint.b.bblerr )
			release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		else if(hcint.b.frmovrun )
			release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		hcchar_data_t 	hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		_urbd->error_count=hcchar.b.multicnt;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		if(_ifxhc->data_pid_start == IFXUSB_HC_PID_DATA0)
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
		else
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
		_ifxhc->split=1;
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_intr_tx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t       *_ifxhc,
                                  ifxusb_hc_regs_t  *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	hcint_data_t  hcint;
	hcint_data_t  hcintmsk;
	hctsiz_data_t hctsiz;
	int out_nak_enh = 0;

	if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
		out_nak_enh = 1;

	hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
	hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
	hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,nyet);

	if(hcint.b.xfercomp   )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->split=1;
		_ifxhc->do_ping        = 0;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count=0;
		_ifxhc->split          = 1;
		_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
		if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
		if(!out_nak_enh  )
			_ifxhc->do_ping        =1;
		else
			_ifxhc->do_ping        =0;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;

		/*== AVM/BC 20100701 - Workaround FullSpeed Interrupts with HiSpeed Hub ==*/
		_ifxhc->nyet_count++;
		if(_ifxhc->nyet_count > 2) {
			_ifxhc->split = 1;
			_ifxhc->nyet_count = 0;
			_ifxhc->wait_for_sof = 5;
		}

		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall || hcint.b.frmovrun)
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;
		if     (hcint.b.stall)
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		else if(hcint.b.frmovrun )
			release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		hcchar_data_t 	hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		_urbd->error_count=hcchar.b.multicnt;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			_ifxhc->do_ping        = 0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->wait_for_sof   = _ifxhc->epqh->interval-1;
			if(!_ifxhc->wait_for_sof) _ifxhc->wait_for_sof=1;
			if(!out_nak_enh  )
				_ifxhc->do_ping        =1;
			else
				_ifxhc->do_ping        =0;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		if(_ifxhc->data_pid_start == IFXUSB_HC_PID_DATA0)
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
		else
			_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA0;
		_ifxhc->split=1;
		if(!out_nak_enh  )
			_ifxhc->do_ping        =1;
		else
			_ifxhc->do_ping        =0;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count=0;
		_ifxhc->wait_for_sof   = 0;
		_ifxhc->do_ping        = 0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_rx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t       *_ifxhc,
                                   ifxusb_hc_regs_t  *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
		if(hcint.b.xfercomp   )
		{
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
			_urbd->error_count=0;
			_ifxhc->wait_for_sof   = 0;
			_ifxhc->split=1;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
			return 1;
		}
		else if(hcint.b.nak        )
		{
			Retry Start Split (in next b_interval ¡V 1 uF)
		}
		else if(hcint.b.nyet)
		{
			//Do Next Complete Split
			// Issue Retry instantly on next SOF, without gothrough process_channels
			_urbd->error_count=0;
			//disable_hc_int(_hc_regs,ack);
			//disable_hc_int(_hc_regs,nak);
			//disable_hc_int(_hc_regs,datatglerr);
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->wait_for_sof   = 1;
			ifxhcd_hc_start(&_ifxhcd->core_if, _ifxhc);
			return 1;
		}
		else if(hcint.b.frmovrun || hcint.b.stall || hcint.b.bblerr)
		{
			_urbd->error_count=0;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nyet);
			disable_hc_int(_hc_regs,nak);
			_ifxhc->wait_for_sof   = 0;

			//if(hctsiz.b.pktcnt==0)
			//{
			//	complete_channel(_ifxhcd, _ifxhc, _urbd);
			//	return 1;
			//}
			//else
			//	_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			if     (hcint.b.stall)
				release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
			else if(hcint.b.frmovrun )
			else if(hcint.b.bblerr )
			return 1;
		}
		else if(hcint.b.xacterr    )
		{
			Rewind Buffer Pointers
			if (HCCHARn.EC = = 3) // ERR response received
			{
				Record ERR error
				Do Next Start Split (in next frame)
			}
			else
			{
				De-allocate Channel
			}
		}
		else if(hcint.b.datatglerr )
		{
			warning
		}
		else if(hcint.b.ack )
		{
			warning
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_tx_csplit(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t       *_ifxhc,
                                   ifxusb_hc_regs_t  *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;
		int out_nak_enh = 0;

		if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
			out_nak_enh = 1;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);
		warning
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int32_t handle_hc_chhltd_intr(ifxhcd_hcd_t      *_ifxhcd,
                                     ifxhcd_hc_t       *_ifxhc,
                                     ifxusb_hc_regs_t  *_hc_regs,
                                     ifxhcd_urbd_t      *_urbd)
{
	IFX_DEBUGPL(DBG_HCD, "--Host Channel %d Interrupt: Channel Halted--\n", _ifxhc->hc_num);

	_ifxhc->halting      = 0;
	_ifxhc->xfer_started = 0;

	if (_ifxhc->halt_status == HC_XFER_URB_DEQUEUE ||
	    _ifxhc->halt_status == HC_XFER_AHB_ERR) {
		/*
		 * Just release the channel. A dequeue can happen on a
		 * transfer timeout. In the case of an AHB Error, the channel
		 * was forced to halt because there's no way to gracefully
		 * recover.
		 */
		release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
		return 1;
	}

	if     (_ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL || _ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)
	{
		if     (_ifxhc->split==0)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrlbulk_rx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrlbulk_tx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==1)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrlbulk_rx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrlbulk_tx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==2)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrlbulk_rx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrlbulk_tx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
	}
	else if(_ifxhc->ep_type == IFXUSB_EP_TYPE_INTR)
	{
		if     (_ifxhc->split==0)
		{
			if(_ifxhc->is_in)
				return (chhltd_intr_rx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_intr_tx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==1)
		{
			if(_ifxhc->is_in)
				return (chhltd_intr_rx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_intr_tx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==2)
		{
			if(_ifxhc->is_in)
				return (chhltd_intr_rx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_intr_tx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
	}
	else if(_ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
	{
		if     (_ifxhc->split==0)
		{
			if(_ifxhc->is_in)
				return (chhltd_isoc_rx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_isoc_tx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==1)
		{
			if(_ifxhc->is_in)
				return (chhltd_isoc_rx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_isoc_tx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==2)
		{
			if(_ifxhc->is_in)
				return (chhltd_isoc_rx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_isoc_tx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
	}
	return 0;
}

/*
 * Handles a host channel AHB error interrupt. This handler is only called in
 * DMA mode.
 */
static void hc_other_intr_dump(ifxhcd_hcd_t      *_ifxhcd,
                               ifxhcd_hc_t       *_ifxhc,
                               ifxusb_hc_regs_t  *_hc_regs,
                               ifxhcd_urbd_t      *_urbd)
{
	#ifdef __DEBUG__
		hcchar_data_t hcchar;
		hcsplt_data_t hcsplt;
		hctsiz_data_t hctsiz;
		uint32_t      hcdma;
		struct urb   *urb = _urbd->urb;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		hcsplt.d32 = ifxusb_rreg(&_hc_regs->hcsplt);
		hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
		hcdma = ifxusb_rreg(&_hc_regs->hcdma);

		IFX_ERROR("Channel %d\n", _ifxhc->hc_num);
		IFX_ERROR("  hcchar 0x%08x, hcsplt 0x%08x\n", hcchar.d32, hcsplt.d32);
		IFX_ERROR("  hctsiz 0x%08x, hcdma 0x%08x\n", hctsiz.d32, hcdma);
		IFX_ERROR("  Device address: %d\n", usb_pipedevice(urb->pipe));
		IFX_ERROR("  Endpoint: %d, %s\n", usb_pipeendpoint(urb->pipe),
			    (usb_pipein(urb->pipe) ? "IN" : "OUT"));
		IFX_ERROR("  Endpoint type: %s\n",
			    ({char *pipetype;
			    switch (usb_pipetype(urb->pipe)) {
				    case PIPE_CONTROL: pipetype = "CTRL"; break;
				    case PIPE_BULK: pipetype = "BULK"; break;
				    case PIPE_INTERRUPT: pipetype = "INTR"; break;
				    case PIPE_ISOCHRONOUS: pipetype = "ISOC"; break;
				    default: pipetype = "????"; break;
			    }; pipetype;}));
		IFX_ERROR("  Speed: %s\n",
			    ({char *speed;
			    switch (urb->dev->speed) {
				    case USB_SPEED_HIGH: speed = "HS"; break;
				    case USB_SPEED_FULL: speed = "FS"; break;
				    case USB_SPEED_LOW: speed = "LS"; break;
			    	default: speed = "????"; break;
			    }; speed;}));
		IFX_ERROR("  Max packet size: %d\n",
			    usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe)));
		IFX_ERROR("  Data buffer length: %d\n", urb->transfer_buffer_length);
		IFX_ERROR("  Transfer buffer: %p, Transfer DMA: %p\n",
			    urb->transfer_buffer, (void *)urb->transfer_dma);
		IFX_ERROR("  Setup buffer: %p, Setup DMA: %p\n",
			    urb->setup_packet, (void *)urb->setup_dma);
		IFX_ERROR("  Interval: %d\n", urb->interval);
	#endif //__DEBUG__
}

/*
 * Handles a host channel ACK interrupt. This interrupt is enabled when
 *  errors occur, and during Start Split transactions.
 */
static int32_t handle_hc_ack_intr(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t      *_ifxhc,
                                  ifxusb_hc_regs_t *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	_urbd->error_count=0;
	if(_ifxhc->nak_countdown_r)
	{
		_ifxhc->nak_retry=_ifxhc->nak_retry_r;
		_ifxhc->nak_countdown=_ifxhc->nak_countdown_r;
	}
	else
		disable_hc_int(_hc_regs,nak);
	disable_hc_int(_hc_regs,ack);
	return 1;
}

/*
 * Handles a host channel ACK interrupt. This interrupt is enabled when
 *  errors occur, and during Start Split transactions.
 */
static int32_t handle_hc_nak_intr(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t      *_ifxhc,
                                  ifxusb_hc_regs_t *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{

	_urbd->error_count=0;

	if(_ifxhc->nak_countdown_r)
	{
		_ifxhc->nak_countdown--;
		if(!_ifxhc->nak_countdown)
		{
			_ifxhc->nak_countdown=_ifxhc->nak_countdown_r;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			ifxhcd_hc_halt(&_ifxhcd->core_if, _ifxhc, HC_XFER_NAK);
		}
		else
			enable_hc_int(_hc_regs,ack);
	}
	else
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
	}
	return 1;
}

/*
 * Handles a host channel AHB error interrupt. This handler is only called in
 * DMA mode.
 */
static int32_t handle_hc_ahberr_intr(ifxhcd_hcd_t      *_ifxhcd,
                                     ifxhcd_hc_t       *_ifxhc,
                                     ifxusb_hc_regs_t  *_hc_regs,
                                     ifxhcd_urbd_t      *_urbd)
{
	IFX_DEBUGPL(DBG_HCD, "--Host Channel %d Interrupt: "
		    "AHB Error--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);

	ifxhcd_hc_halt(&_ifxhcd->core_if, _ifxhc, HC_XFER_AHB_ERR);
	return 1;
}

/*
 * Datatoggle
 */
static int32_t handle_hc_datatglerr_intr(ifxhcd_hcd_t      *_ifxhcd,
                                         ifxhcd_hc_t      *_ifxhc,
                                         ifxusb_hc_regs_t *_hc_regs,
                                         ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "DATATOGGLE Error--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,datatglerr);
	return 1;
}



/*
 * Interrupts which should not been triggered
 */
static int32_t handle_hc_frmovrun_intr(ifxhcd_hcd_t      *_ifxhcd,
                                       ifxhcd_hc_t      *_ifxhc,
                                       ifxusb_hc_regs_t *_hc_regs,
                                       ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "FrameOverRun Error--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,frmovrun);
	return 1;
}

static int32_t handle_hc_bblerr_intr(ifxhcd_hcd_t      *_ifxhcd,
                                     ifxhcd_hc_t      *_ifxhc,
                                     ifxusb_hc_regs_t *_hc_regs,
                                     ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "BBL Error--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,bblerr);
	return 1;
}

static int32_t handle_hc_xacterr_intr(ifxhcd_hcd_t      *_ifxhcd,
                                      ifxhcd_hc_t      *_ifxhc,
                                      ifxusb_hc_regs_t *_hc_regs,
                                      ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "XACT Error--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,xacterr);
	return 1;
}

static int32_t handle_hc_nyet_intr(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t      *_ifxhc,
                                   ifxusb_hc_regs_t *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "NYET--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	_urbd->error_count=0;
	disable_hc_int(_hc_regs,nyet);
	return 1;
}

static int32_t handle_hc_stall_intr(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t      *_ifxhc,
                                    ifxusb_hc_regs_t *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "STALL--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,stall);
	return 1;
}

static int32_t handle_hc_xfercomp_intr(ifxhcd_hcd_t      *_ifxhcd,
                                       ifxhcd_hc_t      *_ifxhc,
                                       ifxusb_hc_regs_t *_hc_regs,
                                       ifxhcd_urbd_t     *_urbd)
{
	IFX_ERROR( "--Host Channel %d Interrupt: "
		    "XFERCOMP--\n", _ifxhc->hc_num);
	hc_other_intr_dump(_ifxhcd,_ifxhc,_hc_regs,_urbd);
	disable_hc_int(_hc_regs,xfercomp);
	return 1;
}



/* This interrupt indicates that the specified host channels has a pending
 * interrupt. There are multiple conditions that can cause each host channel
 * interrupt. This function determines which conditions have occurred for this
 * host channel interrupt and handles them appropriately. */
static int32_t handle_hc_n_intr (ifxhcd_hcd_t *_ifxhcd, uint32_t _num)
{
	uint32_t          hcintval,hcintmsk;
	hcint_data_t      hcint;
	ifxhcd_hc_t      *ifxhc;
	ifxusb_hc_regs_t *hc_regs;
	ifxhcd_urbd_t     *urbd;
	unsigned long     flags;

	int retval = 0;

	IFX_DEBUGPL(DBG_HCDV, "--Host Channel Interrupt--, Channel %d\n", _num);

	/*== AVM/BC 20101111 Lock needed ==*/
	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

	ifxhc = &_ifxhcd->ifxhc[_num];
	hc_regs = _ifxhcd->core_if.hc_regs[_num];

	hcintval  = ifxusb_rreg(&hc_regs->hcint);
	hcintmsk  = ifxusb_rreg(&hc_regs->hcintmsk);
	hcint.d32 = hcintval & hcintmsk;
	IFX_DEBUGPL(DBG_HCDV, "  0x%08x & 0x%08x = 0x%08x\n",
		    hcintval, hcintmsk, hcint.d32);

	urbd = list_entry(ifxhc->epqh->urbd_list.next, ifxhcd_urbd_t, urbd_list_entry);

	if (hcint.b.datatglerr)
		retval |= handle_hc_datatglerr_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.frmovrun)
		retval |= handle_hc_frmovrun_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.bblerr)
		retval |= handle_hc_bblerr_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.xacterr)
		retval |= handle_hc_xacterr_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.nyet)
		retval |= handle_hc_nyet_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.ack)
		retval |= handle_hc_ack_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.nak)
		retval |= handle_hc_nak_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.stall)
		retval |= handle_hc_stall_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	if (hcint.b.ahberr) {
		clear_hc_int(hc_regs, ahberr);
		retval |= handle_hc_ahberr_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	}
	if (hcint.b.chhltd) {
		/* == 20110901 AVM/WK Fix: Flag must not be cleared after restart of channel ==*/
		clear_hc_int(hc_regs, chhltd);
		retval |= handle_hc_chhltd_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	}
	if (hcint.b.xfercomp)
		retval |= handle_hc_xfercomp_intr(_ifxhcd, ifxhc, hc_regs, urbd);

	/* == 20110901 AVM/WK Fix: Never clear possibly new intvals ==*/
	//ifxusb_wreg(&hc_regs->hcint,hcintval);

	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);

	return retval;
}






static uint8_t update_interval_counter(ifxhcd_epqh_t *_epqh,uint32_t _diff)
{
	if(_diff>=_epqh->period_counter)
	{
		_epqh->period_do=1;
		if(_diff>_epqh->interval)
			_epqh->period_counter=1;
		else
			_epqh->period_counter=_epqh->period_counter+_epqh->interval-_diff;
		return 1;
	}
	_epqh->period_counter=_epqh->period_counter-_diff;
	return 0;
}




/*
 * Handles the start-of-frame interrupt in host mode. Non-periodic
 * transactions may be queued to the DWC_otg controller for the current
 * (micro)frame. Periodic transactions may be queued to the controller for the
 * next (micro)frame.
 */
static int32_t handle_sof_intr (ifxhcd_hcd_t *_ifxhcd)
{
	#ifdef __DYN_SOF_INTR__
		uint8_t with_count_down=0;
	#endif
	uint8_t active_on=0;
	uint8_t ready_on=0;
	struct list_head  *epqh_entry;
	ifxhcd_epqh_t     *epqh;
	hfnum_data_t hfnum;
	uint32_t fndiff;

	unsigned long flags;
#ifdef __USE_TIMER_4_SOF__
	uint32_t wait_for_sof = 0x10000;
#endif

	SPIN_LOCK_IRQSAVE(&_ifxhcd->lock, flags);

	{
		int               num_channels;
		ifxusb_hc_regs_t *hc_regs;
		int	              i;
		num_channels = _ifxhcd->core_if.params.host_channels;

// AVM/WK moved block here due to use of SOF timer
		hfnum.d32 = ifxusb_rreg(&_ifxhcd->core_if.host_global_regs->hfnum);
		fndiff = hfnum.b.frnum;
		fndiff+= 0x00004000;
		fndiff-= _ifxhcd->lastframe ;
		fndiff&= 0x00003FFF;
		if(!fndiff) fndiff =1;

		for (i = 0; i < num_channels; i++)
		{
			if(_ifxhcd->ifxhc[i].wait_for_sof && _ifxhcd->ifxhc[i].xfer_started)
			{
#ifdef __USE_TIMER_4_SOF__
				if (_ifxhcd->ifxhc[i].wait_for_sof > fndiff) {
					_ifxhcd->ifxhc[i].wait_for_sof -= fndiff;
				} else {
					_ifxhcd->ifxhc[i].wait_for_sof = 0;
				}
#else
				_ifxhcd->ifxhc[i].wait_for_sof--;
#endif
				if(_ifxhcd->ifxhc[i].wait_for_sof==0)
				{
					hcint_data_t hcint= { .d32=0 };
					hc_regs = _ifxhcd->core_if.hc_regs[i];

					hcint.d32 =0xFFFFFFFF;
					ifxusb_wreg(&hc_regs->hcint, hcint.d32);

					hcint.d32=ifxusb_rreg(&hc_regs->hcintmsk);
					hcint.b.nak =0;
					hcint.b.ack =0;
					/* == 20110901 AVM/WK Fix: We don't need NOT YET IRQ ==*/
					hcint.b.nyet=0;
					_ifxhcd->ifxhc[i].nak_countdown=_ifxhcd->ifxhc[i].nak_countdown_r;
					if(_ifxhcd->ifxhc[i].nak_countdown_r)
						hcint.b.nak =1;
					ifxusb_wreg(&hc_regs->hcintmsk, hcint.d32);

					/* AVM WK / BC 20100827
					 * FIX: Packet was ignored because of wrong Oddframe bit
					 */
					if (_ifxhcd->ifxhc[i].ep_type == IFXUSB_EP_TYPE_INTR || _ifxhcd->ifxhc[i].ep_type == IFXUSB_EP_TYPE_ISOC)
					{
						hcchar_data_t hcchar;
						hcchar.d32 = _ifxhcd->ifxhc[i].hcchar;
						hfnum.d32 = ifxusb_rreg(&_ifxhcd->core_if.host_global_regs->hfnum);
						/* 1 if _next_ frame is odd, 0 if it's even */
						hcchar.b.oddfrm = (hfnum.b.frnum & 0x1) ? 0 : 1;
						_ifxhcd->ifxhc[i].hcchar = hcchar.d32;
					}

					ifxusb_wreg(&hc_regs->hcchar, _ifxhcd->ifxhc[i].hcchar);

				}
			}
			else
				_ifxhcd->ifxhc[i].wait_for_sof=0;

#ifdef __USE_TIMER_4_SOF__
			if (_ifxhcd->ifxhc[i].wait_for_sof && (wait_for_sof > _ifxhcd->ifxhc[i].wait_for_sof)) {
				wait_for_sof = _ifxhcd->ifxhc[i].wait_for_sof;
			}
#endif
		}
	}

	// ISOC Active
	#ifdef __EN_ISOC__
		#error ISOC not supported: missing SOF code
		epqh_entry = _ifxhcd->epqh_isoc_active.next;
		while (epqh_entry != &_ifxhcd->epqh_isoc_active)
		{
			epqh = list_entry(epqh_entry, ifxhcd_epqh_t, epqh_list_entry);
			epqh_entry = epqh_entry->next;
			#ifdef __DYN_SOF_INTR__
				with_count_down=1;
			#endif
			active_on+=update_interval_counter(epqh,fndiff);
		}

		// ISOC Ready
		epqh_entry = _ifxhcd->epqh_isoc_ready.next;
		while (epqh_entry != &_ifxhcd->epqh_isoc_ready)
		{
			epqh = list_entry(epqh_entry, ifxhcd_epqh_t, epqh_list_entry);
			epqh_entry = epqh_entry->next;
			#ifdef __DYN_SOF_INTR__
				with_count_down=1;
			#endif
			ready_on+=update_interval_counter(epqh,fndiff);
		}
	#endif

	// INTR Active
	epqh_entry = _ifxhcd->epqh_intr_active.next;
	while (epqh_entry != &_ifxhcd->epqh_intr_active)
	{
		epqh = list_entry(epqh_entry, ifxhcd_epqh_t, epqh_list_entry);
		epqh_entry = epqh_entry->next;
		#ifdef __DYN_SOF_INTR__
			with_count_down=1;
		#endif
#ifdef __USE_TIMER_4_SOF__
		if (update_interval_counter(epqh,fndiff)) {
			active_on ++;
			wait_for_sof = 1;
		} else {
			if (epqh->period_counter && (wait_for_sof > epqh->period_counter)) {
				wait_for_sof = epqh->period_counter;
			}
		}
#else
		active_on+=update_interval_counter(epqh,fndiff);
#endif
	}

	// INTR Ready
	epqh_entry = _ifxhcd->epqh_intr_ready.next;
	while (epqh_entry != &_ifxhcd->epqh_intr_ready)
	{
		epqh = list_entry(epqh_entry, ifxhcd_epqh_t, epqh_list_entry);
		epqh_entry = epqh_entry->next;
		#ifdef __DYN_SOF_INTR__
			with_count_down=1;
		#endif
#ifdef __USE_TIMER_4_SOF__
		if (update_interval_counter(epqh,fndiff)) {
			ready_on ++;
			wait_for_sof = 1;
		} else {
			if (epqh->period_counter && (wait_for_sof > epqh->period_counter)) {
				wait_for_sof = epqh->period_counter;
			}
		}
#else
		ready_on+=update_interval_counter(epqh,fndiff);
#endif
	}

	// Stdby
	epqh_entry = _ifxhcd->epqh_stdby.next;
	while (epqh_entry != &_ifxhcd->epqh_stdby)
	{
		epqh = list_entry(epqh_entry, ifxhcd_epqh_t, epqh_list_entry);
		epqh_entry = epqh_entry->next;
		if(epqh->period_counter > 0 ) {
#ifdef __USE_TIMER_4_SOF__
			if (epqh->period_counter > fndiff) {
				epqh->period_counter -= fndiff;
			} else {
				epqh->period_counter = 0;
			}
#else
			epqh->period_counter --;
#endif
			#ifdef __DYN_SOF_INTR__
				with_count_down=1;
			#endif
		}
		if(epqh->period_counter == 0) {
			ifxhcd_epqh_idle_periodic(epqh);
		}
#ifdef __USE_TIMER_4_SOF__
		else {
			if (wait_for_sof > epqh->period_counter) {
				wait_for_sof = epqh->period_counter;
			}
		}
#endif
	}
	SPIN_UNLOCK_IRQRESTORE(&_ifxhcd->lock, flags);

	if(ready_on)
		select_eps(_ifxhcd);
	else if(active_on)
		process_channels(_ifxhcd);

	/* Clear interrupt */
	{
		gint_data_t gintsts;
		gintsts.d32=0;
		gintsts.b.sofintr = 1;
		ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);

		#ifdef __DYN_SOF_INTR__
			if(!with_count_down)
				ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk, gintsts.d32,0);
		#endif
#ifdef __USE_TIMER_4_SOF__
		wait_for_sof &= 0xFFFF; // reduce to 16 Bits.

		if(wait_for_sof == 1) {
			// enable SOF
				gint_data_t gintsts;
				gintsts.d32=0;
				gintsts.b.sofintr = 1;
				ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk, 0,gintsts.d32);
		} else {
			// disable SOF
			ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk, gintsts.d32,0);
			if (wait_for_sof > 1) {
				// use timer, not SOF IRQ
				hprt0_data_t   hprt0;
				ktime_t ktime;
				hprt0.d32 = ifxusb_read_hprt0 (&_ifxhcd->core_if);
				if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED) {
					ktime = ktime_set(0, wait_for_sof * 125 * 1000); /*--- wakeup in n*125usec ---*/
				} else {
					ktime = ktime_set(0, wait_for_sof * (1000*1000)); /*--- wakeup in n*1000usec ---*/
				}
				hrtimer_start(&_ifxhcd->hr_timer, ktime, HRTIMER_MODE_REL);
			}
		}
#endif
	}
	_ifxhcd->lastframe=hfnum.b.frnum;
	return 1;
}



/* There are multiple conditions that can cause a port interrupt. This function
 * determines which interrupt conditions have occurred and handles them
 * appropriately. */
static int32_t handle_port_intr (ifxhcd_hcd_t *_ifxhcd)
{
	int retval = 0;
	hprt0_data_t hprt0;
	hprt0_data_t hprt0_modify;

	hprt0.d32        =
	hprt0_modify.d32 = ifxusb_rreg(_ifxhcd->core_if.hprt0);

	/* Clear appropriate bits in HPRT0 to clear the interrupt bit in
	 * GINTSTS */

	hprt0_modify.b.prtena = 0;
	hprt0_modify.b.prtconndet = 0;
	hprt0_modify.b.prtenchng = 0;
	hprt0_modify.b.prtovrcurrchng = 0;

	/* Port Connect Detected
	 * Set flag and clear if detected */
	if (hprt0.b.prtconndet) {
		IFX_DEBUGPL(DBG_HCD, "--Port Interrupt HPRT0=0x%08x "
			    "Port Connect Detected--\n", hprt0.d32);
		_ifxhcd->flags.b.port_connect_status_change = 1;
		_ifxhcd->flags.b.port_connect_status = 1;
		hprt0_modify.b.prtconndet = 1;

		/* The Hub driver asserts a reset when it sees port connect
		 * status change flag */
		retval |= 1;
	}

	/* Port Enable Changed
	 * Clear if detected - Set internal flag if disabled */
	if (hprt0.b.prtenchng) {

		IFX_DEBUGPL(DBG_HCD, "  --Port Interrupt HPRT0=0x%08x "
			    "Port Enable Changed--\n", hprt0.d32);
		hprt0_modify.b.prtenchng = 1;
		if (hprt0.b.prtena == 1)
			/* Port has been enabled set the reset change flag */
			_ifxhcd->flags.b.port_reset_change = 1;
		else
			_ifxhcd->flags.b.port_enable_change = 1;
		retval |= 1;
	}

	/* Overcurrent Change Interrupt */

	if (hprt0.b.prtovrcurrchng) {
		IFX_DEBUGPL(DBG_HCD, "  --Port Interrupt HPRT0=0x%08x "
			    "Port Overcurrent Changed--\n", hprt0.d32);
		_ifxhcd->flags.b.port_over_current_change = 1;
		hprt0_modify.b.prtovrcurrchng = 1;
		retval |= 1;
	}

	/* Clear Port Interrupts */
	ifxusb_wreg(_ifxhcd->core_if.hprt0, hprt0_modify.d32);
	return retval;
}

/*
 * This interrupt indicates that SUSPEND state has been detected on
 * the USB.
 * No Functioning in Host Mode
 */
static int32_t handle_usb_suspend_intr(ifxhcd_hcd_t *_ifxhcd)
{
	gint_data_t gintsts;
	IFX_DEBUGP("USB SUSPEND RECEIVED!\n");
	/* Clear interrupt */
	gintsts.d32 = 0;
	gintsts.b.usbsuspend = 1;
	ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/*
 * This interrupt indicates that the IFXUSB controller has detected a
 * resume or remote wakeup sequence. If the IFXUSB controller is in
 * low power mode, the handler must brings the controller out of low
 * power mode. The controller automatically begins resume
 * signaling. The handler schedules a time to stop resume signaling.
 */
static int32_t handle_wakeup_detected_intr(ifxhcd_hcd_t *_ifxhcd)
{
	gint_data_t gintsts;
	hprt0_data_t hprt0 = {.d32=0};
	pcgcctl_data_t pcgcctl = {.d32=0};
	ifxusb_core_if_t *core_if = &_ifxhcd->core_if;

	IFX_DEBUGPL(DBG_ANY, "++Resume and Remote Wakeup Detected Interrupt++\n");

	/*
	 * Clear the Resume after 70ms. (Need 20 ms minimum. Use 70 ms
	 * so that OPT tests pass with all PHYs).
	 */
	/* Restart the Phy Clock */
	pcgcctl.b.stoppclk = 1;
	ifxusb_mreg(core_if->pcgcctl, pcgcctl.d32, 0);
	UDELAY(10);

	/* Now wait for 70 ms. */
	hprt0.d32 = ifxusb_read_hprt0( core_if );
	IFX_DEBUGPL(DBG_ANY,"Resume: HPRT0=%0x\n", hprt0.d32);
	MDELAY(70);
	hprt0.b.prtres = 0; /* Resume */
	ifxusb_wreg(core_if->hprt0, hprt0.d32);
	IFX_DEBUGPL(DBG_ANY,"Clear Resume: HPRT0=%0x\n", ifxusb_rreg(core_if->hprt0));

	/* Clear interrupt */
	gintsts.d32 = 0;
	gintsts.b.wkupintr = 1;
	ifxusb_wreg(&core_if->core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/*
 * This interrupt indicates that a device is initiating the Session
 * Request Protocol to request the host to turn on bus power so a new
 * session can begin. The handler responds by turning on bus power. If
 * the DWC_otg controller is in low power mode, the handler brings the
 * controller out of low power mode before turning on bus power.
 */
static int32_t handle_session_req_intr(ifxhcd_hcd_t *_ifxhcd)
{
	/* Clear interrupt */
	gint_data_t gintsts = { .d32 = 0 };
	gintsts.b.sessreqintr = 1;
	ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/*
 * This interrupt indicates that a device has been disconnected from
 * the root port.
 */
static int32_t handle_disconnect_intr(ifxhcd_hcd_t *_ifxhcd)
{
	gint_data_t gintsts;

	ifxhcd_disconnect(_ifxhcd);

	gintsts.d32 = 0;
	gintsts.b.disconnect = 1;
	ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/*
 * This function handles the Connector ID Status Change Interrupt.  It
 * reads the OTG Interrupt Register (GOTCTL) to determine whether this
 * is a Device to Host Mode transition or a Host Mode to Device
 * Transition.
 * This only occurs when the cable is connected/removed from the PHY
 * connector.
 */
static int32_t handle_conn_id_status_change_intr(ifxhcd_hcd_t *_ifxhcd)
{
	gint_data_t gintsts;

	IFX_WARN("ID Status Change Interrupt: currently in %s mode\n",
	     ifxusb_mode(&_ifxhcd->core_if) ? "Host" : "Device");

	gintsts.d32 = 0;
	gintsts.b.conidstschng = 1;
	ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

static int32_t handle_otg_intr(ifxhcd_hcd_t *_ifxhcd)
{
	ifxusb_core_global_regs_t *global_regs = _ifxhcd->core_if.core_global_regs;
	gotgint_data_t gotgint;
	gotgint.d32 = ifxusb_rreg( &global_regs->gotgint);
	/* Clear GOTGINT */
	ifxusb_wreg (&global_regs->gotgint, gotgint.d32);
	return 1;
}

/** This function will log a debug message */
static int32_t handle_mode_mismatch_intr(ifxhcd_hcd_t *_ifxhcd)
{
	gint_data_t gintsts;

	IFX_WARN("Mode Mismatch Interrupt: currently in %s mode\n",
	     ifxusb_mode(&_ifxhcd->core_if) ? "Host" : "Device");
	gintsts.d32 = 0;
	gintsts.b.modemismatch = 1;
	ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/** This function handles interrupts for the HCD. */
int32_t ifxhcd_handle_intr (ifxhcd_hcd_t *_ifxhcd)
{
	int retval = 0;

	ifxusb_core_if_t *core_if = &_ifxhcd->core_if;
	/* AVM/BC 20101111 Unnecesary variable removed*/
	//gint_data_t gintsts,gintsts2;
	gint_data_t gintsts;

	/* Check if HOST Mode */
	if (ifxusb_is_device_mode(core_if))
	{
		IFX_ERROR("%s() CRITICAL!  IN DEVICE MODE\n", __func__);
		return 0;
	}

	gintsts.d32 = ifxusb_read_core_intr(core_if);

	if (!gintsts.d32)
		return 0;

	//Common INT
	if (gintsts.b.modemismatch)
	{
		retval |= handle_mode_mismatch_intr(_ifxhcd);
		gintsts.b.modemismatch=0;
	}
	if (gintsts.b.otgintr)
	{
		retval |= handle_otg_intr(_ifxhcd);
		gintsts.b.otgintr=0;
	}
	if (gintsts.b.conidstschng)
	{
		retval |= handle_conn_id_status_change_intr(_ifxhcd);
		gintsts.b.conidstschng=0;
	}
	if (gintsts.b.disconnect)
	{
		retval |= handle_disconnect_intr(_ifxhcd);
		gintsts.b.disconnect=0;
	}
	if (gintsts.b.sessreqintr)
	{
		retval |= handle_session_req_intr(_ifxhcd);
		gintsts.b.sessreqintr=0;
	}
	if (gintsts.b.wkupintr)
	{
		retval |= handle_wakeup_detected_intr(_ifxhcd);
		gintsts.b.wkupintr=0;
	}
	if (gintsts.b.usbsuspend)
	{
		retval |= handle_usb_suspend_intr(_ifxhcd);
		gintsts.b.usbsuspend=0;
	}

	//Host Int
	if (gintsts.b.sofintr)
	{
		retval |= handle_sof_intr (_ifxhcd);
		gintsts.b.sofintr=0;
	}
	if (gintsts.b.portintr)
	{
		retval |= handle_port_intr (_ifxhcd);
		gintsts.b.portintr=0;
	}
	if (gintsts.b.hcintr)
	{
		int i;
		haint_data_t haint;
		haint.d32 = ifxusb_read_host_all_channels_intr(core_if);
		for (i=0; i< core_if->params.host_channels; i++)
			if (haint.b2.chint & (1 << i))
				retval |= handle_hc_n_intr (_ifxhcd, i);
		gintsts.b.hcintr=0;
	}
	return retval;
}
