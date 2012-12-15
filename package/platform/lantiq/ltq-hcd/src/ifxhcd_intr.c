/*****************************************************************************
 **   FILE NAME       : ifxhcd_intr.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the implementation of the HCD Interrupt handlers.
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
		#ifdef __NAKSTOP__
			case HC_XFER_NAK:
				printk("HC_XFER_NAK");break;
		#endif
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
			printk(KERN_INFO "  Data buffer length: %d/%d\n",urb->actual_length, urb->transfer_buffer_length);
			printk(KERN_INFO "  Transfer buffer: %p, Transfer DMA: %p\n",
				    urb->transfer_buffer, (void *)urb->transfer_dma);
			printk(KERN_INFO "  Setup buffer: %p, Setup DMA: %p\n",
				    urb->setup_packet, (void *)urb->setup_dma);
			printk(KERN_INFO "  Interval: %d\n", urb->interval);
		}
		if(urbd)
		{
			switch (urbd->status)
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
					printk(KERN_INFO "  STATUS:UNKKNOWN %d\n",urbd->status);break;
			}
		}
	#endif
}

/*!
	 \fn    static void release_channel(ifxhcd_hcd_t          *_ifxhcd,
                            ifxhcd_hc_t           *_ifxhc,
                            ifxhcd_halt_status_e  _halt_status)
	 \brief Release the halted channel.
	 \param _ifxhcd Pointer to the sate of HCD structure
	 \param _ifxhc Pointer to host channel descriptor
	 \param _halt_status Halt satus
	 \return None
	 \ingroup  IFXUSB_HCD
 */

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
	{
		if(_halt_status!=HC_XFER_NO_EPQH)
			IFX_ERROR("%s epqh=null\n",__func__);
	}
	else
	{
		urbd=epqh->urbd;
		if(!urbd)
			IFX_ERROR("%s urbd=null\n",__func__);
		else
		{
			urb=urbd->urb;
			if(!urb)
			{
				if(_halt_status!=HC_XFER_NO_URB)
					IFX_ERROR("%s urb =null\n",__func__);
			}
			else
			{
				if      (read_data_toggle(hc_regs) == IFXUSB_HCTSIZ_DATA0)
					usb_settoggle (urb->dev,usb_pipeendpoint (urb->pipe), (_ifxhc->is_in)?0:1,0);
				else if (read_data_toggle(hc_regs) == IFXUSB_HCTSIZ_DATA1)
					usb_settoggle (urb->dev,usb_pipeendpoint (urb->pipe), (_ifxhc->is_in)?0:1,1);
			}
		}
	}

	switch (_halt_status)
	{
		case HC_XFER_NO_HALT_STATUS:
			IFX_ERROR("%s: No halt_status, channel %d\n", __func__, _ifxhc->hc_num);
//			return;
			break;
		case HC_XFER_COMPLETE:
			IFX_ERROR("%s: Inavalid halt_status HC_XFER_COMPLETE, channel %d\n", __func__, _ifxhc->hc_num);
//			return;
			break;
		case HC_XFER_NO_URB:
			break;
		case HC_XFER_NO_EPQH:
			break;
		case HC_XFER_URB_DEQUEUE:
		case HC_XFER_AHB_ERR:
		case HC_XFER_XACT_ERR:
		case HC_XFER_FRAME_OVERRUN:
			if(urbd && urb)
			{
				urbd->phase=URBD_DEQUEUEING;
				ifxhcd_complete_urb(_ifxhcd, urbd, urbd->status);
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			break;
		case HC_XFER_URB_COMPLETE:
			if(urbd && urb)
			{
				urbd->phase=URBD_COMPLETING;
				ifxhcd_complete_urb(_ifxhcd, urbd, urbd->status);
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			break;
		case HC_XFER_STALL:
			if(urbd)
			{
				urbd->phase=URBD_DEQUEUEING;
				ifxhcd_complete_urb(_ifxhcd, urbd, -EPIPE);
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			if(epqh && urb && urb->dev && urb->pipe)
				usb_settoggle(urb->dev, usb_pipeendpoint (urb->pipe), !usb_pipein(urb->pipe), IFXUSB_HC_PID_DATA0);
			break;
		case HC_XFER_BABBLE_ERR:
		case HC_XFER_DATA_TOGGLE_ERR:
			if(urbd)
			{
				urbd->phase=URBD_DEQUEUEING;
				ifxhcd_complete_urb(_ifxhcd, urbd, -EOVERFLOW);
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			break;
	#ifdef __NAKSTOP__
		case HC_XFER_NAK:
			if (_ifxhc->is_in)
			{
				if(urbd && urb)
				{
					urbd->phase=URBD_COMPLETING;
					ifxhcd_complete_urb(_ifxhcd, urbd, 0);
				}
				else
				{
					IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
					release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
				}
			}
			else
			{
				IFX_WARN("WARNING %s():%d urbd=%p urb=%p\n",__func__,__LINE__,urbd,urb);
				release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
			}
			break;
	#endif
	#if defined(__INTRNAKRETRY__) || defined(__INTRINCRETRY__)
		case HC_XFER_INTR_NAK_RETRY:
			epqh->phase=EPQH_READY;
			urbd->phase=URBD_IDLE;
			ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
			select_eps(_ifxhcd);
			return;
			break;
			
	#endif
	}
	if(epqh)
	{
		ifxhcd_epqh_idle(epqh);
	}
	else if(_halt_status!=HC_XFER_NO_EPQH)
	{
		IFX_WARN("WARNING %s():%d epqh=%p\n",__func__,__LINE__,epqh);
		release_channel_dump(_ifxhc,urb,epqh,urbd,_halt_status);
	}
	ifxhcd_hc_cleanup(&_ifxhcd->core_if, _ifxhc);
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

	#ifdef __EN_ISOC__
	if(_urbd->epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
	{
		struct usb_iso_packet_descriptor *frame_desc;
		frame_desc            = &_urb->iso_frame_desc[_urbd->isoc_frame_index];
		if (_ifxhc->is_in)
		{
			hctsiz_data_t hctsiz;
			hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
			frame_desc->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			if ((hctsiz.b.xfersize != 0) || (frame_desc->actual_length >= _urbd->xfer_len))
			{
				xfer_done = 1;
				frame_desc->status = 0;
				#if 0
					if (frame_desc->actual_length < frame_desc->length && _urb->transfer_flags & URB_SHORT_NOT_OK)
						frame_desc->status = -EREMOTEIO;
				#endif
			}
		}
		else
		{
			if (_ifxhc->split)
				frame_desc->actual_length +=  _ifxhc->ssplit_out_xfer_count;
			else
				frame_desc->actual_length +=  _ifxhc->xfer_len;
			if (frame_desc->actual_length >= _urbd->xfer_len)
			{
				xfer_done = 1;
				frame_desc->status = 0;
			}
		}
	}
	else
	#endif
	if (_ifxhc->is_in)
	{
		hctsiz_data_t hctsiz;
		hctsiz.d32 = ifxusb_rreg(&_hc_regs->hctsiz);
		_urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
#ifdef __INTRINCRETRY__
		if(_urbd->epqh->ep_type==IFXUSB_EP_TYPE_INTR)
		{
			if(_ifxhc->xfer_len != hctsiz.b.xfersize)
			{
				xfer_done = 1;
				_urbd->status = 0;
			}
		}
		else 
#endif
		if ((hctsiz.b.xfersize != 0) || (_urb->actual_length >= _urb->transfer_buffer_length))
		{
			xfer_done = 1;
			_urbd->status = 0;
			if(_urb->transfer_flags & URB_SHORT_NOT_OK)
			{
				if (_urb->actual_length < _urb->transfer_buffer_length)
					_urbd->status = -EREMOTEIO;
			}
		}
	}
	else if(_urb->transfer_buffer_length%_ifxhc->mps) // OUT without ZLP
	{
		if (_ifxhc->split)
			_urb->actual_length +=  _ifxhc->ssplit_out_xfer_count;
		else
			_urb->actual_length +=  _ifxhc->xfer_len;
		if (_urb->actual_length >= _urb->transfer_buffer_length)
		{
			xfer_done = 1;
			_urbd->status = 0;
		}
	}
	else if (_urb->actual_length >= _urb->transfer_buffer_length) //OUT with ZLP
	{
		xfer_done = 1;
		_urbd->status = 0;
	}
	else //OUT without ZLP, unfinished
	{
		if (_ifxhc->split)
			_urb->actual_length +=  _ifxhc->ssplit_out_xfer_count;
		else
			_urb->actual_length +=  _ifxhc->xfer_len;
		if (!_ifxhc->short_rw && _urb->actual_length >= _urb->transfer_buffer_length)
		{
			xfer_done = 1;
			_urbd->status = 0;
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
			#ifdef __EN_ISOC__
			if(_urbd->epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
			{
				IFX_DEBUGPL(DBG_HCDV, "  descritor # %d\n", _urbd->isoc_frame_index);
				IFX_DEBUGPL(DBG_HCDV, "  buffer_length %d\n",
					_urb->iso_frame_desc[_urbd->isoc_frame_index].length);
				IFX_DEBUGPL(DBG_HCDV, "  actual_length %d\n", _urb->iso_frame_desc[_urbd->isoc_frame_index].actual_length);
			}
			else
			#endif
			{
				IFX_DEBUGPL(DBG_HCDV, "  urb->transfer_buffer_length %d\n",
					    _urb->transfer_buffer_length);
				IFX_DEBUGPL(DBG_HCDV, "  urb->actual_length %d\n", _urb->actual_length);
			}
		}
	#endif
	return xfer_done;
}

#ifdef __EN_ISOC__
	static void next_isoc_sub(unsigned long data)
	{
		ifxhcd_urbd_t *urbd;
		ifxhcd_hcd_t *ifxhcd;

		urbd=((ifxhcd_urbd_t *)data);
		ifxhcd=urbd->epqh->ifxhcd;

		if (!urbd->epqh)
			IFX_ERROR("%s: invalid epqd\n",__func__);
		#if   defined(__UNALIGNED_BUF_ADJ__)
		else
		{
			if( urbd->aligned_checked   &&
//			    urbd->using_aligned_buf &&
			    urbd->xfer_buff &&
			    urbd->is_in)
			{
				uint8_t *buf;

				buf=urbd->xfer_buff;
				buf+=urbd->urb->iso_frame_desc[urbd->isoc_frame_index].offset;
				memcpy(buf,urbd->aligned_buf,urbd->urb->iso_frame_desc[urbd->isoc_frame_index].length);
			}
//			urbd->using_aligned_buf=0;
//			urbd->using_aligned_setup=0;
		}
		#endif

		urbd->isoc_frame_index++;
		if(urbd->isoc_frame_index>=urbd->urb->number_of_packets)
			release_channel(ifxhcd,urbd->epqh->hc,HC_XFER_URB_COMPLETE);
		else
			init_hc(urbd->epqh);
	}
#endif

/*!
	 \fn    static void complete_channel(ifxhcd_hcd_t        *_ifxhcd,
                            ifxhcd_hc_t          *_ifxhc,
                            ifxhcd_urbd_t        *_urbd)
	 \brief Complete the transaction on the channel.
	 \param _ifxhcd Pointer to the sate of HCD structure
	 \param _ifxhc Pointer to host channel descriptor
	 \param _urbd Pointer to URB descriptor
	 \return None
	 \ingroup  IFXUSB_HCD
 */
static void complete_channel(ifxhcd_hcd_t        *_ifxhcd,
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

	if(!epqh)
	{
		release_channel(_ifxhcd,_ifxhc,HC_XFER_NO_EPQH);
		return;
	}
	if(!urb || (unsigned long)urb->hcpriv!=(unsigned long)_urbd)
	{
		release_channel(_ifxhcd,_ifxhc,HC_XFER_NO_URB);
		return;
	}

	if (_ifxhc->split)
		_ifxhc->split = 1;

	switch (epqh->ep_type)
	{
		case IFXUSB_EP_TYPE_CTRL:
			switch (_ifxhc->control_phase)
			{
				case IFXHCD_CONTROL_SETUP:
					if (_urbd->xfer_len > 0)
					{
						_ifxhc->control_phase = IFXHCD_CONTROL_DATA;
						IFX_DEBUGPL(DBG_HCDV, "  Control setup transaction done Data Stage now\n");
						_ifxhc->is_in         = _urbd->is_in;
						_ifxhc->xfer_len      = _urbd->xfer_len;
						#if   defined(__UNALIGNED_BUF_ADJ__)
							if(_urbd->aligned_buf)
								_ifxhc->xfer_buff      = _urbd->aligned_buf;
							else
						#endif
								_ifxhc->xfer_buff      = _urbd->xfer_buff;
						#ifdef __NAKSTOP__
						if(!_ifxhc->split)
						{
							#ifdef __INNAKSTOP_CTRL__
							if(_ifxhc->is_in)
								_ifxhc->stop_on=1;
							#endif
							#ifdef __PINGSTOP_CTRL__
							if(!_ifxhc->is_in)
								_ifxhc->stop_on=1;
							#endif
						}
						#endif
					}
					else
					{
						IFX_DEBUGPL(DBG_HCDV, "  Control setup transaction done Status Stage now\n");
						_ifxhc->control_phase = IFXHCD_CONTROL_STATUS;
						_ifxhc->is_in          = 1;
						_ifxhc->xfer_len       = 0;
						_ifxhc->xfer_buff      = _ifxhcd->status_buf;
						#ifdef __NAKSTOP__
							_ifxhc->stop_on=0;
						#endif
					}
					if(_ifxhc->is_in)
						_ifxhc->short_rw       =0;
					else
						_ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
					_ifxhc->data_pid_start = IFXUSB_HC_PID_DATA1;
					_ifxhc->xfer_count     = 0;
					_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
					_ifxhc->phase=HC_WAITING;
					ifxhcd_hc_start(_ifxhcd, _ifxhc);
					break;
				case IFXHCD_CONTROL_DATA:
					urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
					if (urb_xfer_done)
					{
						_ifxhc->control_phase  = IFXHCD_CONTROL_STATUS;
						IFX_DEBUGPL(DBG_HCDV, "  Control data transaction done Status Stage now\n");
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
						#ifdef __NAKSTOP__
							_ifxhc->stop_on=0;
						#endif
					}
					else // continue
					{
						IFX_DEBUGPL(DBG_HCDV, "  Control data transaction continue\n");
						_ifxhc->xfer_len       = _urbd->xfer_len - urb->actual_length;
						_ifxhc->xfer_count     = urb->actual_length;
						_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
						_ifxhc->data_pid_start = read_data_toggle(hc_regs);
					}
					_ifxhc->phase=HC_WAITING;
					ifxhcd_hc_start(_ifxhcd, _ifxhc);
					break;
				case IFXHCD_CONTROL_STATUS:
					IFX_DEBUGPL(DBG_HCDV, "  Control status transaction done\n");
					if (_urbd->status == -EINPROGRESS)
						_urbd->status = 0;
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
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
			break;
		case IFXUSB_EP_TYPE_INTR:
			urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
			
			#ifdef __INTRINCRETRY__
			if(!urb_xfer_done)
			release_channel(_ifxhcd,_ifxhc,HC_XFER_INTR_NAK_RETRY);
			else
			#endif
			release_channel(_ifxhcd,_ifxhc,HC_XFER_URB_COMPLETE);
			break;
		case IFXUSB_EP_TYPE_ISOC:
			#ifdef __EN_ISOC__
				urb_xfer_done = update_urb_state_xfer_comp(_ifxhc, hc_regs, urb, _urbd);
				if (urb_xfer_done)
				{
					#if   defined(__UNALIGNED_BUF_ADJ__)
					if(in_irq())
					{
						if(!epqh->tasklet_next_isoc.func)
						{
							epqh->tasklet_next_isoc.next = NULL;
							epqh->tasklet_next_isoc.state = 0;
							atomic_set( &epqh->tasklet_next_isoc.count, 0);
							epqh->tasklet_next_isoc.func = next_isoc_sub;
							epqh->tasklet_next_isoc.data = (unsigned long)_urbd;
						}
						tasklet_schedule(&epqh->tasklet_next_isoc);
					}
					else
					#endif
					{
						next_isoc_sub((unsigned long)_urbd);
					}
				}
				else
				{
					struct usb_iso_packet_descriptor *frame_desc;
					frame_desc            = &urb->iso_frame_desc[_urbd->isoc_frame_index];
					_ifxhc->xfer_len       = _urbd->xfer_len - frame_desc->actual_length;
					_ifxhc->xfer_count     = frame_desc->actual_length;
					_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
					_ifxhc->data_pid_start = read_data_toggle(hc_regs);
					_ifxhc->phase=HC_WAITING;
					ifxhcd_hc_start(_ifxhcd, _ifxhc);
				}
			#endif
			break;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_rx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
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

	#ifdef __INNAKSTOP_CTRL__
	if (_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			u32 actual_length;
			actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);

			if(_urbd->xfer_len && actual_length >= _urbd->xfer_len)
			{
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else
			{
				_ifxhc->xfer_count        =
				_urbd->urb->actual_length = actual_length;
				_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
				_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
		}
		else
		{
			printk(KERN_INFO "Warning: %s() %d Invalid CTRL Phase:%d\n",__func__,__LINE__,_ifxhc->control_phase);
			release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
		}
		return 1;
	}
	#endif

	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		_urbd->error_count     =0;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count     =0;
		// ZLP shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		#if 0
		if(_ifxhc->control_phase == IFXHCD_CONTROL_STATUS)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
				_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if (hcint.b.bblerr)
	{
		_urbd->error_count     =0;

		// ZLP shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		#if 0
		if(_ifxhc->control_phase == IFXHCD_CONTROL_STATUS)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
				_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
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
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		#if 1
		if(_ifxhc->control_phase == IFXHCD_CONTROL_STATUS)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			#if 1
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);
				if(actual_length >= _urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->error_count++;
					_ifxhc->xfer_count        =
					_urbd->urb->actual_length = actual_length;
					_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					if (_urbd->error_count >= 3)
					{
						_urbd->error_count     =0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						_ifxhc->erron=1;
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				}
			#endif
		}
		else
		{
			_urbd->error_count     =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		#if 0
			#if 1
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);
				if(actual_length>=_urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->urb->actual_length = actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					_urbd->error_count     =0;
					release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
				}
			#endif
		#else
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
				_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		#endif
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
//		if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else
	{
		_urbd->error_count     =0;
		IFX_ERROR("ERROR %s():%d invalid chhlt condition %08X/%08X  %d\n",__func__,__LINE__,hcint.d32,hcintmsk.d32,_ifxhc->halt_status);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		return 1;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
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

	#ifdef __PINGSTOP_CTRL__
	if (_ifxhc->halt_status == HC_XFER_NAK)
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			u32 actual_length;
			actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);

			if(_urbd->xfer_len && actual_length >= _urbd->xfer_len)
			{
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else
			{
				_ifxhc->xfer_count        =
				_urbd->urb->actual_length = actual_length;
				_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
				_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
				_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
		}
		else
		{
			printk(KERN_INFO "Warning: %s() %d Invalid CTRL Phase:%d\n",__func__,__LINE__,_ifxhc->control_phase);
			release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
		}
		return 1;
	}
	#endif


	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		_urbd->error_count     =0;
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && hcint.b.nak)
		{
			// Walkaround: When sending ZLP and receive NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			if(hcint.b.nyet)
				_ifxhc->epqh->do_ping=1;

			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		else
		{
			if(hcint.b.nyet)
				_ifxhc->epqh->do_ping=1;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count     =0;

		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
			{
				_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//				if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
			}
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
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		if(_ifxhc->control_phase == IFXHCD_CONTROL_STATUS)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else if(_ifxhc->control_phase == IFXHCD_CONTROL_SETUP)
		{
			_urbd->error_count     =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			#if 0
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
				if(actual_length>=_urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->error_count++;
					_ifxhc->xfer_count        =
					_urbd->urb->actual_length = actual_length;
					_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					if (_urbd->error_count >= 3)
					{
						_urbd->error_count     =0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						_ifxhc->erron=1;
						_ifxhc->phase=HC_WAITING;
						_ifxhc->epqh->do_ping=1;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				}
			#endif
		}
		else
		{
			_urbd->error_count     =0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		}
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.nak || hcint.b.nyet)
	{
		#ifdef __PINGSTOP_CTRL__
			_urbd->error_count     =0;
			IFX_ERROR("ERROR %s():%d invalid chhlt condition\n",__func__,__LINE__);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		#else
			// ZLP shortcut
			#if 1
			if(hctsiz.b.pktcnt==0)
			{
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else
			#endif
			if(_ifxhc->control_phase == IFXHCD_CONTROL_STATUS)
			{
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else if(_ifxhc->control_phase == IFXHCD_CONTROL_SETUP)
			{
				_urbd->error_count     =0;
				IFX_ERROR("ERROR %s():%d invalid chhlt condition\n",__func__,__LINE__);
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
			{
				#if 0
					_ifxhc->epqh->do_ping=1;
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				#else
					u32 actual_length;
					_ifxhc->epqh->do_ping=1;
					actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
					if(actual_length>=_urbd->xfer_len)
					{
						_urbd->error_count     =0;
						complete_channel(_ifxhcd, _ifxhc, _urbd);
					}
					else
					{
						_ifxhc->xfer_count        =
						_urbd->urb->actual_length = actual_length;
						_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
						_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
						_ifxhc->erron=1;
						_ifxhc->epqh->do_ping=1;
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				#endif
			}
		#endif
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		}
		_urbd->error_count     =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		}
		_urbd->error_count     =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else
	{
		_urbd->error_count     =0;
		IFX_ERROR("ERROR %s():%d invalid chhlt condition %08X/%08X  %d\n",__func__,__LINE__,hcint.d32,hcintmsk.d32,_ifxhc->halt_status);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_rx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
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

	#ifdef __INNAKSTOP_BULK__
	if(_ifxhc->halt_status == HC_XFER_NAK)
	{
		u32 actual_length;
		actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);

		if(
		   (_urbd->xfer_len && actual_length>=_urbd->xfer_len)
		   || hctsiz.b.pktcnt==0
		   || (hctsiz.b.xfersize % _ifxhc->mps)>0
		  )
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		{
			_urbd->urb->actual_length = actual_length;
			_ifxhc->xfer_len          = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count        = _urbd->urb->actual_length;
			_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	#endif

	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		_urbd->error_count     =0;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count     =0;
		// ZLP shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if (hcint.b.bblerr)
	{
		_urbd->error_count     =0;

		// ZLP shortcut
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
	else if (hcint.b.xacterr)
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			#if 0
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);
				if(actual_length >= _urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->error_count++;
					_ifxhc->xfer_count        =
					_urbd->urb->actual_length = actual_length;
					_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					if (_urbd->error_count >= 3)
					{
						_urbd->error_count     =0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						_ifxhc->erron=1;
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				}
			#endif
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		#if 0
			#if 1
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + (_ifxhc->xfer_len - hctsiz.b.xfersize);
				if(actual_length >= _urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->urb->actual_length = actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					_urbd->error_count     =0;
					release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
				}
			#endif
		#else
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
//			if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		#endif
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
//		if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else
	{
		_urbd->error_count     =0;
		IFX_ERROR("ERROR %s():%d invalid chhlt condition %08X/%08X %d sz:%d/%d/%d/%d\n",__func__,__LINE__,hcint.d32,hcintmsk.d32,_ifxhc->halt_status , hctsiz.b.xfersize, _ifxhc->xfer_len-_ifxhc->xfer_len,_ifxhc->xfer_len,_urbd->xfer_len);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
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

	#ifdef __PINGSTOP_BULK__
	if (_ifxhc->halt_status == HC_XFER_NAK)
	{
		u32 actual_length;
		actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);

		if(_urbd->xfer_len && actual_length >= _urbd->xfer_len)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		{
			_ifxhc->xfer_count        =
			_urbd->urb->actual_length = actual_length;
			_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
			_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	#endif

	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		_urbd->error_count     =0;
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && hcint.b.nak)
		{
			// Walkaround: When sending ZLP and receive NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			if(hcint.b.nyet)
				_ifxhc->epqh->do_ping=1;

			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		else
		{
			if(hcint.b.nyet)
				_ifxhc->epqh->do_ping=1;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count     =0;

		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
			if(_urbd->urb->actual_length>_urbd->xfer_len) _urbd->urb->actual_length=_urbd->xfer_len;
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
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			#if 0
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			#else
				u32 actual_length;
				actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
				if(actual_length >= _urbd->xfer_len)
				{
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				}
				else
				{
					_urbd->error_count++;
					_ifxhc->xfer_count        =
					_urbd->urb->actual_length = actual_length;
					_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
					_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
					if (_urbd->error_count >= 3)
					{
						_urbd->error_count     =0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						_ifxhc->erron=1;
						_ifxhc->phase=HC_WAITING;
						_ifxhc->epqh->do_ping=1;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				}
			#endif
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
		if(_urbd->urb->actual_length>_urbd->xfer_len) _urbd->urb->actual_length=_urbd->xfer_len;
		IFX_ERROR("ERROR %s():%d invalid packet babble\n",__func__,__LINE__);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.nak || hcint.b.nyet)
	{
		#ifdef __PINGSTOP_BULK__
			_urbd->error_count     =0;
			IFX_ERROR("ERROR %s():%d invalid chhlt condition\n",__func__,__LINE__);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		#else
			// ZLP shortcut
			#if 1
			if(hctsiz.b.pktcnt==0)
			{
				_urbd->error_count     =0;
				complete_channel(_ifxhcd, _ifxhc, _urbd);
			}
			else
			#endif
			{
				#if 0
					_ifxhc->epqh->do_ping=1;
					_urbd->error_count     =0;
					complete_channel(_ifxhcd, _ifxhc, _urbd);
				#else
					u32 actual_length;
					_ifxhc->epqh->do_ping=1;
					actual_length = _urbd->urb->actual_length + ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
					if(actual_length>=_urbd->xfer_len)
					{
						_urbd->error_count     =0;
						complete_channel(_ifxhcd, _ifxhc, _urbd);
					}
					else
					{
						_ifxhc->xfer_count        =
						_urbd->urb->actual_length = actual_length;
						_ifxhc->xfer_len          = _urbd->xfer_len - actual_length;
						_ifxhc->data_pid_start    = read_data_toggle(_hc_regs);
						_ifxhc->erron=1;
						_ifxhc->epqh->do_ping=1;
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				#endif
			}
		#endif
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//		if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		_urbd->error_count     =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->urb->actual_length += ((_ifxhc->start_pkt_count - hctsiz.b.pktcnt ) * _ifxhc->mps);
//		if( _urbd->urb->actual_length > _ifxhc->xfer_len) _urbd->urb->actual_length = _urbd->xfer_len;
		_urbd->error_count     =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else
	{
		_urbd->error_count     =0;
		IFX_ERROR("ERROR %s():%d invalid chhlt condition %08X/%08X  %d\n",__func__,__LINE__,hcint.d32,hcintmsk.d32,_ifxhc->halt_status);
		release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
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

	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		_urbd->error_count   =0;
		//restart INTR immediately
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.stall)
	{
		_urbd->error_count   =0;

		// Don't care shortcut
		#if 0
		if(hctsiz.b.pktcnt==0)
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		else
		#endif
		{
			_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
			release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		}
		return 1;
	}
	else if (hcint.b.bblerr)
	{
		_urbd->error_count   =0;

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
	else if (hcint.b.datatglerr || hcint.b.frmovrun)
	{
		_urbd->error_count   =0;
		//restart INTR immediately
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			_urbd->error_count++;
			if(_urbd->error_count>=3)
			{
				_urbd->error_count     =0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
		}
		return 1;
	}
	else if(hcint.b.nyet   )
	{
		return 1;
	}
	else if (hcint.b.nak)
	{
		
		#ifdef __INTRNAKRETRY__
		if(hctsiz.b.pktcnt)
		{
			release_channel(_ifxhcd, _ifxhc, HC_XFER_INTR_NAK_RETRY);
			return 1;
		}
		#endif
		_urbd->error_count   =0;
		//restart INTR immediately
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else
	{
		_urbd->error_count   =0;
		//restart INTR immediately
		#if 0
		if(hctsiz.b.pktcnt>0)
		{
			// TODO Re-initialize Channel (in next b_interval - 1 uF/F)
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		else
		#endif
		{
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
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

	if (hcint.b.xfercomp || hcint.d32 == 0x02)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
		disable_hc_int(_hc_regs,nyet);
		_urbd->error_count   =0;
		//restart INTR immediately
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.stall)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nyet);
		disable_hc_int(_hc_regs,nak);
		_urbd->error_count   =0;

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
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		// ZLP shortcut
		#if 1
		if(hctsiz.b.pktcnt==0)
		{
			_urbd->error_count     =0;
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		else
		#endif
		{
			_urbd->error_count++;
			if(_urbd->error_count>=3)
			{
				_urbd->error_count     =0;
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count     =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count     =0;
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
	#ifdef __EN_ISOC__
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

		if (hcint.b.xfercomp || hcint.b.frmovrun || hcint.d32 == 0x02)
		{
			_urbd->error_count=0;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
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
					_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
					_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
					_ifxhc->xfer_count         = _urbd->urb->actual_length;
					_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
					_urbd->error_count++;
					if(_urbd->error_count>=3)
					{
						_urbd->error_count=0;
						if (hcint.b.bblerr)
							release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
						else if (hcint.b.xacterr)
							release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
					}
					else
					{
						enable_hc_int(_hc_regs,ack);
						enable_hc_int(_hc_regs,nak);
						enable_hc_int(_hc_regs,nyet);
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
					}
				}
			#endif
		}
		else if(hcint.b.datatglerr )
		{
			return 1;
		}
		else if(hcint.b.stall      )
		{
			return 1;
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_isoc_tx_nonsplit(ifxhcd_hcd_t      *_ifxhcd,
                                    ifxhcd_hc_t       *_ifxhc,
                                    ifxusb_hc_regs_t  *_hc_regs,
                                    ifxhcd_urbd_t     *_urbd)
{
	#ifdef __EN_ISOC__
		hcint_data_t  hcint;
		hcint_data_t  hcintmsk;
		hctsiz_data_t hctsiz;
		int out_nak_enh = 0;

		if (_ifxhcd->core_if.snpsid >= 0x4f54271a && _ifxhc->speed == IFXUSB_EP_SPEED_HIGH)
			out_nak_enh = 1;

		hcint.d32    = ifxusb_rreg(&_hc_regs->hcint);
		hcintmsk.d32 = ifxusb_rreg(&_hc_regs->hcintmsk);
		hctsiz.d32   = ifxusb_rreg(&_hc_regs->hctsiz);

		if (hcint.b.xfercomp || hcint.d32 == 0x02)
		{
			_urbd->error_count=0;
			disable_hc_int(_hc_regs,ack);
			disable_hc_int(_hc_regs,nak);
			disable_hc_int(_hc_regs,nyet);
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
				release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
			#endif
		}
		else if(hcint.b.datatglerr )
		{
			return 1;
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
					_urbd->urb->actual_length += (_ifxhc->xfer_len - hctsiz.b.xfersize);
					_ifxhc->xfer_len           = _urbd->xfer_len - _urbd->urb->actual_length;
					_ifxhc->xfer_count         = _urbd->urb->actual_length;
					_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
					_urbd->error_count++;
					if(_urbd->error_count>=3)
					{
						_urbd->error_count=0;
						release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
					}
					else
					{
						enable_hc_int(_hc_regs,ack);
						enable_hc_int(_hc_regs,nak);
						enable_hc_int(_hc_regs,nyet);
						_ifxhc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
				release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
			}
			else
			{
				_ifxhc->phase=HC_WAITING;
				ifxhcd_hc_start(_ifxhcd, _ifxhc);
			}
			return 1;
		}
		else if(hcint.b.stall      )
		{
			return 1;
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_rx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
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

	if (hcint.b.ack)
	{
		_urbd->error_count=0;
		_ifxhc->split=2;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_urbd->error_count     = 0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.nyet   )
	{
	}
	else if(hcint.b.xfercomp   )
	{
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_tx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
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
		_urbd->error_count=0;
		if (_ifxhc->control_phase != IFXHCD_CONTROL_SETUP)
			_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		if (_ifxhc->control_phase != IFXHCD_CONTROL_SETUP)
			_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count    =0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
		printk(KERN_INFO "Warning: %s() %d CTRL OUT SPLIT1 COMPLETE\n",__func__,__LINE__);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_rx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
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

	if (hcint.b.ack)
	{
		_urbd->error_count=0;
		_ifxhc->split=2;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_urbd->error_count     = 0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if (hcint.b.xacterr)
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.nyet   )
	{
	}
	else if(hcint.b.xfercomp   )
	{
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_tx_ssplit(ifxhcd_hcd_t      *_ifxhcd,
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
		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start =read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count    =0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
		printk(KERN_INFO "Warning: %s() %d BULK OUT SPLIT1 COMPLETE\n",__func__,__LINE__);
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

	if     (hcint.b.ack)
	{
		_urbd->error_count=0;
		_ifxhc->split=2;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak)
	{
		_urbd->error_count=0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.xacterr)
	{
		hcchar_data_t 	hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		_urbd->error_count=hcchar.b.multicnt;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
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
		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->ssplit_out_xfer_count = _ifxhc->xfer_len;
		_ifxhc->split=2;
		_ifxhc->data_pid_start = read_data_toggle(_hc_regs);
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_urbd->error_count   =0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count   =0;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			enable_hc_int(_hc_regs,ack);
			enable_hc_int(_hc_regs,nak);
			enable_hc_int(_hc_regs,nyet);
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		return 1;
	}
	else if(hcint.b.datatglerr )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_DATA_TOGGLE_ERR);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count   =0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.xfercomp   )
	{
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
			//warning
		}
		else if(hcint.b.bblerr     )
		{
			//warning
		}
		else if(hcint.b.xacterr    )
		{
			//warning
		}
		else if(hcint.b.stall      )
		{
			//warning
		}
		else if(hcint.b.nak        )
		{
			//warning
		}
		else if(hcint.b.xfercomp   )
		{
			//warning
		}
		else if(hcint.b.nyet)
		{
			//warning
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
			//Do Next Start Split (in next b_interval ¡V 1 uF)
		}
		else if(hcint.b.frmovrun   )
		{
			//Do Next Transaction in next frame.
		}
		else if(hcint.b.datatglerr )
		{
			//warning
		}
		else if(hcint.b.bblerr     )
		{
			//warning
		}
		else if(hcint.b.xacterr    )
		{
			//warning
		}
		else if(hcint.b.stall      )
		{
			//warning
		}
		else if(hcint.b.nak        )
		{
			//warning
		}
		else if(hcint.b.xfercomp   )
		{
			//warning
		}
		else if(hcint.b.nyet)
		{
			//warning
		}
	#endif
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_rx_csplit(ifxhcd_hcd_t      *_ifxhcd,
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

	if (hcint.b.xfercomp)
	{
		_urbd->error_count   =0;
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_ifxhc->split          = 1;
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
		}
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall || hcint.b.bblerr )
	{
		_urbd->error_count=0;
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
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
			{
				_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count     = _urbd->urb->actual_length;
			}
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
		}
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_ctrl_tx_csplit(ifxhcd_hcd_t      *_ifxhcd,
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
		_ifxhc->split=1;
		#if 0
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && (hcint.b.nak || hcint.b.nyet))
		{
			// Walkaround: When sending ZLP and receive NYEY or NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		else
		#endif
		{
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->split          = 1;
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
		}
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		//Retry Complete Split
		// Issue Retry instantly on next SOF, without gothrough process_channels
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
			{
				_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
				_ifxhc->xfer_count     = _urbd->urb->actual_length;
			}
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		if(_ifxhc->control_phase == IFXHCD_CONTROL_DATA)
		{
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
		}
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_BABBLE_ERR);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_rx_csplit(ifxhcd_hcd_t      *_ifxhcd,
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

	if (hcint.b.xfercomp)
	{
		_urbd->error_count   =0;
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if (hcint.b.nak)
	{
		_ifxhc->split          = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall || hcint.b.bblerr )
	{
		_urbd->error_count=0;
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
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t chhltd_bulk_tx_csplit(ifxhcd_hcd_t      *_ifxhcd,
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
		_ifxhc->split=1;
		#if 0
		if(_ifxhc->xfer_len==0 && !hcint.b.ack && (hcint.b.nak || hcint.b.nyet))
		{
			// Walkaround: When sending ZLP and receive NYEY or NAK but also issue CMPT intr
			// Solution:   NoSplit: Resend at next SOF
			//             Split  : Resend at next SOF with SSPLIT
			_ifxhc->xfer_len       = 0;
			_ifxhc->xfer_count     = 0;
			_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
		}
		else
		#endif
		{
			complete_channel(_ifxhcd, _ifxhc, _urbd);
		}
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->split          = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		//Retry Complete Split
		// Issue Retry instantly on next SOF, without gothrough process_channels
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall      )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_STALL);
		return 1;
	}
	else if(hcint.b.xacterr    )
	{
		_urbd->error_count++;
		if(_urbd->error_count>=3)
		{
			_urbd->error_count=0;
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->epqh->do_ping=1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun   )
	{
		_urbd->error_count=0;
		release_channel(_ifxhcd, _ifxhc, HC_XFER_FRAME_OVERRUN);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count=0;
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

	if (hcint.b.xfercomp   )
	{
		_urbd->error_count=0;
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->split          = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.frmovrun || hcint.b.bblerr || hcint.b.stall )
	{
		_urbd->error_count=0;
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
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		_ifxhc->split=1;
		complete_channel(_ifxhcd, _ifxhc, _urbd);
		return 1;
	}
	else if(hcint.b.nak        )
	{
		_ifxhc->split          = 1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.nyet)
	{
		_urbd->error_count=0;
		_ifxhc->halt_status    = HC_XFER_NO_HALT_STATUS;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.stall || hcint.b.frmovrun)
	{
		_urbd->error_count=0;
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
			release_channel(_ifxhcd, _ifxhc, HC_XFER_XACT_ERR);
		}
		else
		{
			_ifxhc->split=1;
			_ifxhc->epqh->do_ping=1;
			_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
			_ifxhc->xfer_count     = _urbd->urb->actual_length;
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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
		_ifxhc->epqh->do_ping=1;
		_ifxhc->xfer_len       = _urbd->xfer_len - _urbd->urb->actual_length;
		_ifxhc->xfer_count     = _urbd->urb->actual_length;
		_ifxhc->phase=HC_WAITING;
		ifxhcd_hc_start(_ifxhcd, _ifxhc);
		return 1;
	}
	else if(hcint.b.bblerr     )
	{
		_urbd->error_count=0;
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
			_ifxhc->phase=HC_WAITING;
			ifxhcd_hc_start(_ifxhcd, _ifxhc);
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

/*!
	 \fn    static int32_t handle_hc_chhltd_intr(ifxhcd_hcd_t      *_ifxhcd,
                                     ifxhcd_hc_t       *_ifxhc,
                                     ifxusb_hc_regs_t  *_hc_regs,
                                     ifxhcd_urbd_t      *_urbd)
	 \brief This function handles halted interrupts of host channels.
	 \param  _ifxhcd Pointer to the sate of HCD structure
	 \param  _ifxhc Pointer to host channel descriptor
	 \param  _hc_regs Pointer to host channel registers
	 \param  _urbd Pointer to URB descriptor
	 \return  0 OK
	 \ingroup  IFXUSB_HCD
 */
static
int32_t handle_hc_chhltd_intr(ifxhcd_hcd_t      *_ifxhcd,
                                     ifxhcd_hc_t       *_ifxhc,
                                     ifxusb_hc_regs_t  *_hc_regs,
                                     ifxhcd_urbd_t      *_urbd)
{
	IFX_DEBUGPL(DBG_HCD, "--Host Channel %d Interrupt: Channel Halted--\n", _ifxhc->hc_num);

	_ifxhc->phase = HC_STOPPED;
	if(_ifxhc->epqh)
		if(_ifxhc->epqh->urbd)
			_ifxhc->epqh->urbd->phase=URBD_ACTIVE;

	if (_ifxhc->halt_status == HC_XFER_URB_DEQUEUE ||
	    _ifxhc->halt_status == HC_XFER_AHB_ERR) {
		/*
		 * Just release the channel. A dequeue can happen on a
		 * transfer timeout. In the case of an AHB Error, the channel
		 * was forced to halt because there's no way to gracefully
		 * recover.
		 */
		if(_ifxhc->epqh)
			if(_ifxhc->epqh->urbd)
				_ifxhc->epqh->urbd->phase=URBD_DEQUEUEING;
		release_channel(_ifxhcd, _ifxhc, _ifxhc->halt_status);
		return 1;
	}

	if     (_ifxhc->ep_type == IFXUSB_EP_TYPE_CTRL)
	{
		if     (_ifxhc->split==0)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrl_rx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrl_tx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==1)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrl_rx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrl_tx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==2)
		{
			if(_ifxhc->is_in)
				return (chhltd_ctrl_rx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_ctrl_tx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
	}
	else if(_ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)
	{
		if     (_ifxhc->split==0)
		{
			if(_ifxhc->is_in)
				return (chhltd_bulk_rx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_bulk_tx_nonsplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==1)
		{
			if(_ifxhc->is_in)
				return (chhltd_bulk_rx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_bulk_tx_ssplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
		}
		else if(_ifxhc->split==2)
		{
			if(_ifxhc->is_in)
				return (chhltd_bulk_rx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
			else
				return (chhltd_bulk_tx_csplit(_ifxhcd,_ifxhc,_hc_regs,_urbd));
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
static
int32_t handle_hc_ack_intr(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t      *_ifxhc,
                                  ifxusb_hc_regs_t *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	_urbd->error_count=0;
	_ifxhc->erron = 0;

	disable_hc_int(_hc_regs,nyet);

	#ifdef __NAKSTOP__
	if(!_ifxhc->stop_on)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
	}
	#else
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
	#endif
	return 1;
}

/*
 * Handles a host channel ACK interrupt. This interrupt is enabled when
 *  errors occur, and during Start Split transactions.
 */
static
int32_t handle_hc_nak_intr(ifxhcd_hcd_t      *_ifxhcd,
                                  ifxhcd_hc_t      *_ifxhc,
                                  ifxusb_hc_regs_t *_hc_regs,
                                  ifxhcd_urbd_t     *_urbd)
{
	_urbd->error_count=0;
	_ifxhc->erron=0;
	disable_hc_int(_hc_regs,nyet);
	disable_hc_int(_hc_regs,ack);
	disable_hc_int(_hc_regs,nak);
	#ifdef __NAKSTOP__
	if(_ifxhc->stop_on)
	{
		hcchar_data_t   hcchar;
		hcchar.d32 = ifxusb_rreg(&_hc_regs->hcchar);
		if(hcchar.b.chen)
		{
			hcchar.b.chdis = 1;
			_ifxhc->halt_status = HC_XFER_NAK;
			ifxusb_wreg(&_hc_regs->hcchar, hcchar.d32);
		}
	}
	#endif
	return 1;
}

static
int32_t handle_hc_nyet_intr(ifxhcd_hcd_t      *_ifxhcd,
                                   ifxhcd_hc_t      *_ifxhc,
                                   ifxusb_hc_regs_t *_hc_regs,
                                   ifxhcd_urbd_t     *_urbd)
{
	_urbd->error_count=0;
	_ifxhc->erron = 0;

	disable_hc_int(_hc_regs,nyet);
	#ifdef __NAKSTOP__
	if(!_ifxhc->stop_on)
	{
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
	}
	#else
		disable_hc_int(_hc_regs,ack);
		disable_hc_int(_hc_regs,nak);
	#endif
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

	int retval = 0;

	IFX_DEBUGPL(DBG_HCDV, "--Host Channel Interrupt--, Channel %d\n", _num);

	ifxhc = &_ifxhcd->ifxhc[_num];
	hc_regs = _ifxhcd->core_if.hc_regs[_num];

	hcintval  = ifxusb_rreg(&hc_regs->hcint);
	hcintmsk  = ifxusb_rreg(&hc_regs->hcintmsk);
	hcint.d32 = hcintval & hcintmsk;
	IFX_DEBUGPL(DBG_HCDV, "  0x%08x & 0x%08x = 0x%08x\n",
		    hcintval, hcintmsk, hcint.d32);

	urbd = ifxhc->epqh->urbd;

	if (hcint.b.ahberr)
		retval |= handle_hc_ahberr_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	else if (hcint.b.chhltd)
		retval |= handle_hc_chhltd_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	else
	{
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
		if (hcint.b.xfercomp)
			retval |= handle_hc_xfercomp_intr(_ifxhcd, ifxhc, hc_regs, urbd);
	}

	ifxusb_wreg(&hc_regs->hcint,hcintval);

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

static
void process_unaligned( ifxhcd_epqh_t *_epqh, ifxusb_core_if_t *_core_if)
{
	ifxhcd_urbd_t *urbd;
	urbd  =_epqh->urbd;

	#if   defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__)
	if(!urbd->aligned_checked)
	{
		#if   defined(__UNALIGNED_BUF_ADJ__)
			uint32_t xfer_len;
			xfer_len=urbd->xfer_len;
			if(urbd->is_in && xfer_len<_epqh->mps)
				xfer_len = _epqh->mps;
//			urbd->using_aligned_buf=0;

			if(xfer_len > 0 && ((unsigned long)urbd->xfer_buff) & _core_if->unaligned_mask)
			{
				if(   urbd->aligned_buf
				   && urbd->aligned_buf_len > 0
				   && urbd->aligned_buf_len < xfer_len
				  )
				{
					ifxusb_free_buf_h(urbd->aligned_buf);
					urbd->aligned_buf=NULL;
					urbd->aligned_buf_len=0;
				}
				if(! urbd->aligned_buf || ! urbd->aligned_buf_len)
				{
					urbd->aligned_buf = ifxusb_alloc_buf_h(xfer_len, urbd->is_in);
					if(urbd->aligned_buf)
						urbd->aligned_buf_len = xfer_len;
				}
				if(urbd->aligned_buf)
				{
					if(!urbd->is_in)
						memcpy(urbd->aligned_buf, urbd->xfer_buff, xfer_len);
//					urbd->using_aligned_buf=1;
					_epqh->hc->xfer_buff = urbd->aligned_buf;
				}
				else
					IFX_WARN("%s():%d\n",__func__,__LINE__);
			}
			if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
			{
//				urbd->using_aligned_setup=0;
				if(((unsigned long)urbd->setup_buff) & _core_if->unaligned_mask)
				{
					if(! urbd->aligned_setup)
						urbd->aligned_setup = ifxusb_alloc_buf_h(8,0);
					if(urbd->aligned_setup)
					{
						memcpy(urbd->aligned_setup, urbd->setup_buff, 8);
//						urbd->using_aligned_setup=1;
					}
					else
						IFX_WARN("%s():%d\n",__func__,__LINE__);
					_epqh->hc->xfer_buff = urbd->aligned_setup;
				}
			}
		#elif defined(__UNALIGNED_BUF_CHK__)
			if(_epqh->urbd->is_in)
			{
				if(_epqh->urbd->xfer_len==0)
					IFX_WARN("%s():%d IN xfer while length is zero \n",__func__,__LINE__);
				else{
					if(_epqh->urbd->xfer_len < _epqh->mps)
						IFX_WARN("%s():%d IN xfer while length < mps \n",__func__,__LINE__);
					if(((unsigned long)_epqh->urbd->xfer_buff) & _core_if->unaligned_mask)
						IFX_WARN("%s():%d IN xfer Buffer UNALIGNED\n",__func__,__LINE__);
				}
			}
			else
			{
				if(_epqh->urbd->xfer_len > 0 && (((unsigned long)_epqh->urbd->xfer_buff) & _core_if->unaligned_mask))
					IFX_WARN("%s():%d OUT xfer Buffer UNALIGNED\n",__func__,__LINE__);
			}
			if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
			{
				if(((unsigned long)_epqh->urbd->setup_buff) & _core_if->unaligned_mask)
					IFX_WARN("%s():%d SETUP xfer Buffer UNALIGNED\n",__func__,__LINE__);
			}
		#endif
	}
	urbd->aligned_checked=1;
	#endif
}

/*!
 \brief Assigns transactions from a URBD to a free host channel and initializes the
 host channel to perform the transactions. The host channel is removed from
 the free list.
 \param _ifxhcd The HCD state structure.
 \param _epqh Transactions from the first URBD for this EPQH are selected and assigned to a free host channel.
 */
static
int assign_hc(ifxhcd_hcd_t *_ifxhcd, ifxhcd_epqh_t *_epqh,ifxhcd_urbd_t *_urbd)
{
	ifxhcd_hc_t   *ifxhc;
	struct urb    *urb;

	IFX_DEBUGPL(DBG_HCDV, "%s(%p,%p)\n", __func__, _ifxhcd, _epqh);

	if(_ifxhcd->disconnecting)
	{
		printk(KERN_INFO "Warning: %s() Port is in discoonection\n",__func__);
		return 0;
	}

	if(!_epqh)       return 0;
	if(!_urbd)       return 0;
	if(!_urbd->urb)  return 0;

	{
		int i;
		int num_channels = _ifxhcd->core_if.params.host_channels;
		for(i=0;i<num_channels ; i++)
		{
			hcchar_data_t hcchar;
			ifxusb_hc_regs_t *hc_regs;
			hc_regs = _ifxhcd->core_if.hc_regs[i];
			if(_ifxhcd->ifxhc[i].phase!=HC_IDLE)
			{
				continue;
			}
			hcchar.d32 = ifxusb_rreg(&hc_regs->hcchar);
			if(hcchar.b.chen || hcchar.b.chdis)
			{
				continue;
			}
			break;
		}

		if(i<num_channels)
		{
			ifxhc=&_ifxhcd->ifxhc[i];
			ifxhc->phase=HC_ASSIGNED;
		}
		else
			return 0;
	}

	urb         = _urbd->urb;
	_epqh->hc   = ifxhc;
	_epqh->urbd = _urbd;
	ifxhc->epqh = _epqh;
	/*
	 * Use usb_pipedevice to determine device address. This address is
	 * 0 before the SET_ADDRESS command and the correct address afterward.
	 */
	ifxhc->dev_addr = usb_pipedevice(urb->pipe);
	ifxhc->ep_num   = usb_pipeendpoint(urb->pipe);

	if      (urb->dev->speed == USB_SPEED_LOW)  ifxhc->speed = IFXUSB_EP_SPEED_LOW;
	else if (urb->dev->speed == USB_SPEED_FULL) ifxhc->speed = IFXUSB_EP_SPEED_FULL;
	else                                        ifxhc->speed = IFXUSB_EP_SPEED_HIGH;

	ifxhc->mps         = _epqh->mps;
	ifxhc->halt_status = HC_XFER_NO_HALT_STATUS;
	ifxhc->ep_type = _epqh->ep_type;

	ifxhc->split = 0;
	if (_epqh->need_split)
	{
		ifxhc->split = 1;
		ifxhc->hub_addr       = urb->dev->tt->hub->devnum;
		ifxhc->port_addr      = urb->dev->ttport;
	}
	return 1;
}

/*!
 \brief Assigns transactions from a URBD to a free host channel and initializes the
 host channel to perform the transactions. The host channel is removed from
 the free list.
 \param _ifxhcd The HCD state structure.
 \param _epqh Transactions from the first URBD for this EPQH are selected and assigned to a free host channel.
 */
static
void init_hc(ifxhcd_epqh_t *_epqh)
{
	ifxhcd_hc_t   *ifxhc;
	ifxhcd_urbd_t *urbd;
	struct urb    *urb;
	ifxhcd_hcd_t  *ifxhcd;

	IFX_DEBUGPL(DBG_HCDV, "%s(%p)\n", __func__, _epqh);

	ifxhc =_epqh->hc;
	urbd  =_epqh->urbd;
	ifxhcd=_epqh->ifxhcd;
	urb  = urbd->urb;
	#if   defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__)
	urbd->aligned_checked=0;
	#endif

	ifxhc->halt_status = HC_XFER_NO_HALT_STATUS;

	if(_epqh->ep_type==IFXUSB_EP_TYPE_CTRL)
	{
		ifxhc->control_phase  =IFXHCD_CONTROL_SETUP;
		ifxhc->is_in          = 0;
		ifxhc->data_pid_start = IFXUSB_HC_PID_SETUP;
		ifxhc->xfer_buff      = urbd->setup_buff;
		ifxhc->xfer_len       = 8;
		ifxhc->xfer_count     = 0;
		ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
		ifxhc->sof_delay      = 0;
		_epqh->do_ping=0;
		if(!ifxhc->is_in && ifxhc->split==0)
			_epqh->do_ping=1;
	}
	else if(_epqh->ep_type==IFXUSB_EP_TYPE_ISOC)
	{
		#ifdef __EN_ISOC__
			struct usb_iso_packet_descriptor *frame_desc;
			ifxhc->is_in          = urbd->is_in;
			frame_desc            = &urb->iso_frame_desc[urbd->isoc_frame_index];
			urbd->xfer_len        = ifxhc->xfer_len       = frame_desc->length;
			ifxhc->xfer_buff      = urbd->xfer_buff;
			ifxhc->xfer_buff     += frame_desc->offset;
			ifxhc->xfer_count     = 0;
			ifxhc->sof_delay      = 0;
			if(usb_gettoggle (urb->dev,usb_pipeendpoint (urb->pipe), (ifxhc->is_in)?0:1))
				ifxhc->data_pid_start = IFXUSB_HCTSIZ_DATA1;
			else
				ifxhc->data_pid_start = IFXUSB_HCTSIZ_DATA0;

			if(ifxhc->is_in)
				ifxhc->short_rw       =0;
			else
				ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
			#ifdef __EN_ISOC_SPLIT__
				ifxhc->isoc_xact_pos = IFXUSB_HCSPLIT_XACTPOS_ALL;
			#endif

			_epqh->isoc_frame_index=0;
			_epqh->isoc_now=0;
			_epqh->isoc_start_frame=0;
			if(_urb->transfer_flags && URB_ISO_ASAP)
				_epqh->isoc_now=1;
			else
				_epqh->isoc_start_frame=_urb->start_frame;
			#ifdef __EN_ISOC_SPLIT__
				_epqh->isoc_split_pos   =0;
				_epqh->isoc_split_offset=0;
			#endif
			_epqh->do_ping=0;
		#endif
	}
	else
	{
		ifxhc->is_in          = urbd->is_in;
		ifxhc->xfer_buff      = urbd->xfer_buff;
		ifxhc->xfer_len       = urbd->xfer_len;
		ifxhc->xfer_count     = 0;
		ifxhc->sof_delay      = 0;
//		if(ifxhc->xfer_len==13 &&  ifxhc->is_in && _epqh->ep_type==IFXUSB_EP_TYPE_BULK && ifxhc->split==0)
//			ifxhc->sof_delay      = 8;
		if(usb_gettoggle (urb->dev,usb_pipeendpoint (urb->pipe), (ifxhc->is_in)?0:1))
			ifxhc->data_pid_start = IFXUSB_HCTSIZ_DATA1;
		else
			ifxhc->data_pid_start = IFXUSB_HCTSIZ_DATA0;
		if(ifxhc->is_in)
			ifxhc->short_rw       =0;
		else
			ifxhc->short_rw       =(urb->transfer_flags & URB_ZERO_PACKET)?1:0;
		_epqh->do_ping=0;
		if(!ifxhc->is_in && ifxhc->split==0)
		{
			if(_epqh->ep_type==IFXUSB_EP_TYPE_BULK) _epqh->do_ping=1;
		}
	}

	{
		hcint_data_t      hc_intr_mask;
		uint8_t           hc_num = ifxhc->hc_num;
		ifxusb_hc_regs_t *hc_regs = ifxhcd->core_if.hc_regs[hc_num];

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
			ifxusb_mreg(&ifxhcd->core_if.host_global_regs->haintmsk, 0, intr_enable);
		}

		/* Make sure host channel interrupts are enabled. */
		{
			gint_data_t       gintmsk ={.d32 = 0};
			gintmsk.b.hcintr = 1;
			ifxusb_mreg(&ifxhcd->core_if.core_global_regs->gintmsk, 0, gintmsk.d32);
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
				#if defined(__EN_ISOC__) && defined(__EN_ISOC_SPLIT__)
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
	process_unaligned(_epqh,&ifxhcd->core_if);


	#ifdef __NAKSTOP__
		ifxhc->stop_on=0;
		if (!ifxhc->split && ifxhc->ep_type == IFXUSB_EP_TYPE_BULK)
		{
			#ifdef __INNAKSTOP_BULK__
				if(ifxhc->is_in)
					ifxhc->stop_on=1;
			#endif
			#ifdef __PINGSTOP_BULK__
				if(!ifxhc->is_in)
					ifxhc->stop_on=1;
			#endif
		}
	#endif
}


static
void select_eps_sub(ifxhcd_hcd_t *_ifxhcd)
{
	struct list_head *epqh_ptr;
	ifxhcd_epqh_t    *epqh;
	struct list_head *urbd_ptr;
	unsigned long     flags;
	ifxhcd_urbd_t    *urbd;

	hfnum_data_t hfnum;
	uint32_t fndiff;

	if(_ifxhcd->disconnecting)
	{
//		printk(KERN_INFO "Warning: %s() Port is in discoonection\n",__func__);
		return ;
	}

	local_irq_save(flags);
	LOCK_EPQH_LIST(_ifxhcd);

	hfnum.d32 = ifxusb_rreg(&_ifxhcd->core_if.host_global_regs->hfnum);
	fndiff = hfnum.b.frnum;
	fndiff+= 0x00004000;
	fndiff-= _ifxhcd->lastframe ;
	fndiff&= 0x00003FFF;
	if(!fndiff) fndiff =1;

	#ifdef __EN_ISOC__
		epqh_ptr       = _ifxhcd->epqh_list_isoc.next;
		while (epqh_ptr != &_ifxhcd->epqh_list_isoc)
		{
			epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, ql);
			epqh_ptr = epqh_ptr->next;

			#ifdef __DYN_SOF_INTR__
			if (!list_empty(&epqh->urbd_list))
				_ifxhcd->dyn_sof_count = DYN_SOF_COUNT_DEF;
			#endif

			if(epqh->pause)
				continue;
			if(epqh->phase==EPQH_READY)
			{
				if(update_interval_counter(epqh,fndiff) || epqh->isoc_now)
				{
					LOCK_URBD_LIST(epqh);
					urbd_ptr       = epqh->urbd_list.next;
					while (urbd_ptr != &epqh->urbd_list)
					{
						urbd = list_entry(urbd_ptr, ifxhcd_urbd_t, ql);
						urbd_ptr=urbd_ptr->next;
						if(urbd->phase==URBD_IDLE)
						{
							if(assign_hc(_ifxhcd, epqh,urbd))
							{
								IFX_DEBUGPL(DBG_HCD, "  select_eps ISOC\n");
								#ifdef __EPQD_DESTROY_TIMEOUT__
									del_timer(&epqh->destroy_timer);
								#endif
								epqh->isoc_now=0;
								list_del_init (&epqh->ql);
								list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_isoc);
								init_hc(epqh);
								epqh->phase=EPQH_ACTIVE;
								urbd->phase==URBD_ACTIVE;
								epqh->hc.phase=HC_WAITING;
								ifxhcd_hc_start(_ifxhcd, epqh->hc);
							}
							break;
						}
					}
					UNLOCK_URBD_LIST(epqh);
				}
			}
		}
	#endif //__EN_ISOC__

	epqh_ptr       = _ifxhcd->epqh_list_intr.next;
	while (epqh_ptr != &_ifxhcd->epqh_list_intr)
	{
		epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, ql);
		epqh_ptr = epqh_ptr->next;
		#ifdef __DYN_SOF_INTR__
		if (!list_empty(&epqh->urbd_list))
			_ifxhcd->dyn_sof_count = DYN_SOF_COUNT_DEF;
		#endif
		if(epqh->pause)
			continue;
		if(epqh->phase==EPQH_READY)
		{
			if(update_interval_counter(epqh,fndiff))
			{
				LOCK_URBD_LIST(epqh);
				urbd_ptr       = epqh->urbd_list.next;
				while (urbd_ptr != &epqh->urbd_list)
				{
					urbd = list_entry(urbd_ptr, ifxhcd_urbd_t, ql);
					urbd_ptr=urbd_ptr->next;
					if(urbd->phase==URBD_IDLE)
					{
						if(assign_hc(_ifxhcd, epqh,urbd))
						{
							IFX_DEBUGPL(DBG_HCD, "  select_eps INTR\n");
							#ifdef __EPQD_DESTROY_TIMEOUT__
								del_timer(&epqh->destroy_timer);
							#endif
							list_del_init (&epqh->ql);
							list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_intr);
							init_hc(epqh);
							epqh->phase=EPQH_ACTIVE;
							urbd->phase=URBD_ACTIVE;
							epqh->hc->phase=HC_WAITING;
							ifxhcd_hc_start(_ifxhcd, epqh->hc);
						}
						break;
					}
				}
				UNLOCK_URBD_LIST(epqh);
			}
		}
		else if(epqh->phase==EPQH_STDBY)
		{
			if(epqh->period_counter > 0 )
				epqh->period_counter --;
			if(epqh->period_counter == 0)
				ifxhcd_epqh_idle_periodic(epqh);
			update_interval_counter(epqh,fndiff);
		}
		else
			update_interval_counter(epqh,fndiff);
	}

	epqh_ptr       = _ifxhcd->epqh_list_np.next;
	while (epqh_ptr != &_ifxhcd->epqh_list_np)  // may need to preserve at lease one for period
	{
		epqh = list_entry(epqh_ptr, ifxhcd_epqh_t, ql);
		epqh_ptr = epqh_ptr->next;
		#ifdef __DYN_SOF_INTR__
		if (!list_empty(&epqh->urbd_list))
			_ifxhcd->dyn_sof_count = DYN_SOF_COUNT_DEF;
		#endif
		if(epqh->pause)
			continue;
		if(epqh->phase==EPQH_READY)
		{
			LOCK_URBD_LIST(epqh);
			urbd_ptr       = epqh->urbd_list.next;
			while (urbd_ptr != &epqh->urbd_list)
			{
				urbd = list_entry(urbd_ptr, ifxhcd_urbd_t, ql);
				urbd_ptr=urbd_ptr->next;
				if(urbd->phase==URBD_IDLE)
				{
					if(assign_hc(_ifxhcd, epqh,urbd))
					{
						IFX_DEBUGPL(DBG_HCD, "  select_eps Non-Period\n");
						#ifdef __EPQD_DESTROY_TIMEOUT__
							del_timer(&epqh->destroy_timer);
						#endif
						list_del_init (&epqh->ql);
						list_add_tail(&epqh->ql, &_ifxhcd->epqh_list_np);
						init_hc(epqh);
						epqh->phase=EPQH_ACTIVE;
						urbd->phase=URBD_ACTIVE;
						epqh->hc->phase=HC_WAITING;
						ifxhcd_hc_start(_ifxhcd, epqh->hc);
					}
					break;
				}
			}
			UNLOCK_URBD_LIST(epqh);
		}
	}

	_ifxhcd->lastframe=hfnum.b.frnum;

	UNLOCK_EPQH_LIST(_ifxhcd);
	local_irq_restore(flags);
}

static
void select_eps_func(unsigned long data)
{
	ifxhcd_hcd_t *ifxhcd;
	ifxhcd=((ifxhcd_hcd_t *)data);

	select_eps_sub(ifxhcd);
}

/*!
	 \fn    void select_eps(ifxhcd_hcd_t *_ifxhcd)
	 \brief This function selects transactions from the HCD transfer schedule and assigns them to available host channels.
	 \param _ifxhcd Pointer to the sate of HCD structure
	 \ingroup  IFXUSB_HCD
 */
void select_eps(ifxhcd_hcd_t *_ifxhcd)
{
	if(in_irq())
	{
		if(!_ifxhcd->tasklet_select_eps.func)
		{
			_ifxhcd->tasklet_select_eps.next = NULL;
			_ifxhcd->tasklet_select_eps.state = 0;
			atomic_set( &_ifxhcd->tasklet_select_eps.count, 0);
			_ifxhcd->tasklet_select_eps.func = select_eps_func;
			_ifxhcd->tasklet_select_eps.data = (unsigned long)_ifxhcd;
		}
		tasklet_schedule(&_ifxhcd->tasklet_select_eps);
	}
	else
	{
		select_eps_sub(_ifxhcd);
	}
}

static
void ifxhcd_hc_kickstart(ifxhcd_hcd_t *_ifxhcd)
{
	int               num_channels;
	ifxusb_hc_regs_t *hc_regs;
	int               i;
	ifxhcd_hc_t      *ifxhc;
	num_channels = _ifxhcd->core_if.params.host_channels;

	for (i = 0; i < num_channels; i++)
	{
		ifxhc=&_ifxhcd->ifxhc[i];
		if(ifxhc->phase==HC_STARTING)
		{
			if(ifxhc->sof_delay) ifxhc->sof_delay--;
			if(!ifxhc->sof_delay)
			{
				hcint_data_t hcint;
//				ifxhc->erron=0;
				hc_regs = _ifxhcd->core_if.hc_regs[i];
				hcint.d32 =0xFFFFFFFF;
				ifxusb_wreg(&hc_regs->hcint, hcint.d32);
				hcint.d32 =ifxusb_rreg(&hc_regs->hcintmsk);
				hcint.b.nak =0;
				hcint.b.ack =0;
				hcint.b.nyet=0;
				if(ifxhc->erron)
				{
					hcint.b.ack  =1;
					hcint.b.nak  =1;
					hcint.b.nyet =1;
				}
				#ifdef __NAKSTOP__
				if(ifxhc->stop_on)
				{
					hcint.b.ack  =1;
					hcint.b.nak =1;
				}
				#endif
				ifxusb_wreg(&hc_regs->hcintmsk, hcint.d32);
				ifxusb_wreg(&hc_regs->hcchar, ifxhc->hcchar);
				ifxhc->phase=HC_STARTED;
			}
		}
	}

	for (i = 0; i < num_channels; i++)
	{
		ifxhc=&_ifxhcd->ifxhc[i];
		if(ifxhc->phase==HC_WAITING &&
		    (ifxhc->ep_type == IFXUSB_EP_TYPE_INTR || ifxhc->ep_type == IFXUSB_EP_TYPE_ISOC)
		  )
		{
			ifxhcd_hc_start(_ifxhcd, ifxhc);
		}
	}

	for (i = 0; i < num_channels; i++)
	{
		ifxhc=&_ifxhcd->ifxhc[i];
		if(ifxhc->phase==HC_WAITING)
		{
			ifxhcd_hc_start(_ifxhcd, ifxhc);
		}
	}
}

/*
 * Handles the start-of-frame interrupt in host mode. Non-periodic
 * transactions may be queued to the DWC_otg controller for the current
 * (micro)frame. Periodic transactions may be queued to the controller for the
 * next (micro)frame.
 */
static
int32_t handle_sof_intr (ifxhcd_hcd_t *_ifxhcd)
{
	_ifxhcd->pkt_remaining=_ifxhcd->pkt_remaining_reload;
	ifxhcd_hc_kickstart(_ifxhcd);

	select_eps(_ifxhcd);

	/* Clear interrupt */
	{
		gint_data_t gintsts;
		gintsts.d32=0;
		gintsts.b.sofintr = 1;
		ifxusb_wreg(&_ifxhcd->core_if.core_global_regs->gintsts, gintsts.d32);

		#ifdef __DYN_SOF_INTR__
			if(_ifxhcd->dyn_sof_count)
				_ifxhcd->dyn_sof_count--;
			if(!_ifxhcd->dyn_sof_count)
				ifxusb_mreg(&_ifxhcd->core_if.core_global_regs->gintmsk, gintsts.d32,0);
		#endif
	}
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
		{
			/* Port has been enabled set the reset change flag */
			_ifxhcd->flags.b.port_reset_change = 1;
			if      (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
				_ifxhcd->pkt_remaining_reload=_ifxhcd->pkt_remaining_reload_hs;
			else if (hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
				_ifxhcd->pkt_remaining_reload=_ifxhcd->pkt_remaining_reload_ls;
			else
				_ifxhcd->pkt_remaining_reload=_ifxhcd->pkt_remaining_reload_fs;
		}
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
	gint_data_t gintsts,gintsts2;

	/* Check if HOST Mode */
	if (ifxusb_is_device_mode(core_if))
	{
		IFX_ERROR("%s() CRITICAL!  IN DEVICE MODE\n", __func__);
		return 0;
	}

	gintsts.d32 = ifxusb_read_core_intr(core_if);
	gintsts2.d32 = 0;

	if (!gintsts.d32)
		return 0;

	//Common INT
	if (gintsts.b.modemismatch)
	{
		retval |= handle_mode_mismatch_intr(_ifxhcd);
		gintsts.b.modemismatch=0;
		gintsts2.b.modemismatch=1;
	}
	if (gintsts.b.otgintr)
	{
		retval |= handle_otg_intr(_ifxhcd);
		gintsts.b.otgintr=0;
		gintsts2.b.otgintr=1;
	}
	if (gintsts.b.conidstschng)
	{
		retval |= handle_conn_id_status_change_intr(_ifxhcd);
		gintsts.b.conidstschng=0;
		gintsts2.b.conidstschng=1;
	}
	if (gintsts.b.disconnect)
	{
		retval |= handle_disconnect_intr(_ifxhcd);
		gintsts.b.disconnect=0;
		gintsts2.b.disconnect=1;
	}
	if (gintsts.b.sessreqintr)
	{
		retval |= handle_session_req_intr(_ifxhcd);
		gintsts.b.sessreqintr=0;
		gintsts2.b.sessreqintr=1;
	}
	if (gintsts.b.wkupintr)
	{
		retval |= handle_wakeup_detected_intr(_ifxhcd);
		gintsts.b.wkupintr=0;
		gintsts2.b.wkupintr=1;
	}
	if (gintsts.b.usbsuspend)
	{
		retval |= handle_usb_suspend_intr(_ifxhcd);
		gintsts.b.usbsuspend=0;
		gintsts2.b.usbsuspend=1;
	}

	//Host Int
	if (gintsts.b.sofintr)
	{
		retval |= handle_sof_intr (_ifxhcd);
		gintsts.b.sofintr=0;
		gintsts2.b.sofintr=1;
	}
	if (gintsts.b.portintr)
	{
		retval |= handle_port_intr (_ifxhcd);
		gintsts.b.portintr=0;
		gintsts2.b.portintr=1;
	}
	if (gintsts.b.hcintr)
	{
		int i;
		haint_data_t haint;
		haint.d32 = ifxusb_read_host_all_channels_intr(core_if);
		for (i=0; i<MAX_EPS_CHANNELS && i< core_if->params.host_channels; i++)
			if (haint.b2.chint & (1 << i))
				retval |= handle_hc_n_intr (_ifxhcd, i);
		gintsts.b.hcintr=0;
		gintsts2.b.hcintr=1;
	}
	return retval;
}

