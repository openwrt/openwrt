/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_pcd.h $
 * $Revision: #36 $
 * $Date: 2008/09/26 $
 * $Change: 1103515 $
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
#ifndef DWC_HOST_ONLY
#if !defined(__DWC_PCD_H__)
#define __DWC_PCD_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include <linux/interrupt.h>
#include <linux/dma-mapping.h>

struct dwc_otg_device;

#include "otg_cil.h"

/**
 * @file
 *
 * This file contains the structures, constants, and interfaces for
 * the Perpherial Contoller Driver (PCD).
 *
 * The Peripheral Controller Driver (PCD) for Linux will implement the
 * Gadget API, so that the existing Gadget drivers can be used.	 For
 * the Mass Storage Function driver the File-backed USB Storage Gadget
 * (FBS) driver will be used.  The FBS driver supports the
 * Control-Bulk (CB), Control-Bulk-Interrupt (CBI), and Bulk-Only
 * transports.
 *
 */

/** Invalid DMA Address */
#define DMA_ADDR_INVALID	(~(dma_addr_t)0)
/** Maxpacket size for EP0 */
#define MAX_EP0_SIZE	64
/** Maxpacket size for any EP */
#define MAX_PACKET_SIZE 1024

/** Max Transfer size for any EP */
#define MAX_TRANSFER_SIZE 65535

/** Max DMA Descriptor count for any EP */
#define MAX_DMA_DESC_CNT 64

/**
 * Get the pointer to the core_if from the pcd pointer.
 */
#define GET_CORE_IF( _pcd ) (_pcd->otg_dev->core_if)

/**
 * States of EP0.
 */
typedef enum ep0_state
{
	EP0_DISCONNECT,		/* no host */
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_OUT_DATA_PHASE,
	EP0_IN_STATUS_PHASE,
	EP0_OUT_STATUS_PHASE,
	EP0_STALL,
} ep0state_e;

/** Fordward declaration.*/
struct dwc_otg_pcd;

/** DWC_otg iso request structure.
 *
 */
typedef struct usb_iso_request  dwc_otg_pcd_iso_request_t;

/**	  PCD EP structure.
 * This structure describes an EP, there is an array of EPs in the PCD
 * structure.
 */
typedef struct dwc_otg_pcd_ep
{
	/** USB EP data */
	struct usb_ep		ep;
	/** USB EP Descriptor */
	const struct usb_endpoint_descriptor	*desc;

	/** queue of dwc_otg_pcd_requests. */
	struct list_head	queue;
	unsigned stopped : 1;
	unsigned disabling : 1;
	unsigned dma : 1;
	unsigned queue_sof : 1;

#ifdef DWC_EN_ISOC
	/** DWC_otg Isochronous Transfer */
	struct usb_iso_request* iso_req;
#endif //DWC_EN_ISOC

	/** DWC_otg ep data. */
	dwc_ep_t dwc_ep;

	/** Pointer to PCD */
	struct dwc_otg_pcd *pcd;
}dwc_otg_pcd_ep_t;



/** DWC_otg PCD Structure.
 * This structure encapsulates the data for the dwc_otg PCD.
 */
typedef struct dwc_otg_pcd
{
	/** USB gadget */
	struct usb_gadget gadget;
	/** USB gadget driver pointer*/
	struct usb_gadget_driver *driver;
	/** The DWC otg device pointer. */
	struct dwc_otg_device *otg_dev;

	/** State of EP0 */
	ep0state_e	ep0state;
	/** EP0 Request is pending */
	unsigned	ep0_pending : 1;
	/** Indicates when SET CONFIGURATION Request is in process */
	unsigned	request_config : 1;
	/** The state of the Remote Wakeup Enable. */
	unsigned	remote_wakeup_enable : 1;
	/** The state of the B-Device HNP Enable. */
	unsigned	b_hnp_enable : 1;
	/** The state of A-Device HNP Support. */
	unsigned	a_hnp_support : 1;
	/** The state of the A-Device Alt HNP support. */
	unsigned	a_alt_hnp_support : 1;
	/** Count of pending Requests */
	unsigned	request_pending;

		/** SETUP packet for EP0
	 * This structure is allocated as a DMA buffer on PCD initialization
	 * with enough space for up to 3 setup packets.
	 */
	union
	{
			struct usb_ctrlrequest	req;
			uint32_t	d32[2];
	} *setup_pkt;

	dma_addr_t setup_pkt_dma_handle;

	/** 2-byte dma buffer used to return status from GET_STATUS */
	uint16_t *status_buf;
	dma_addr_t status_buf_dma_handle;

	/** EP0 */
	dwc_otg_pcd_ep_t ep0;

	/** Array of IN EPs. */
	dwc_otg_pcd_ep_t in_ep[ MAX_EPS_CHANNELS - 1];
	/** Array of OUT EPs. */
	dwc_otg_pcd_ep_t out_ep[ MAX_EPS_CHANNELS - 1];
	/** number of valid EPs in the above array. */
//	  unsigned	num_eps : 4;
	spinlock_t	lock;
	/** Timer for SRP.	If it expires before SRP is successful
	 * clear the SRP. */
	struct timer_list srp_timer;

	/** Tasklet to defer starting of TEST mode transmissions until
	 *	Status Phase has been completed.
	 */
	struct tasklet_struct test_mode_tasklet;

	/** Tasklet to delay starting of xfer in DMA mode */
	struct tasklet_struct *start_xfer_tasklet;

	/** The test mode to enter when the tasklet is executed. */
	unsigned test_mode;

} dwc_otg_pcd_t;


/** DWC_otg request structure.
 * This structure is a list of requests.
 */
typedef struct
{
	struct usb_request	req; /**< USB Request. */
	struct list_head	queue;	/**< queue of these requests. */
} dwc_otg_pcd_request_t;


extern int dwc_otg_pcd_init(struct platform_device *pdev);

//extern void dwc_otg_pcd_remove( struct dwc_otg_device *_otg_dev );
extern void dwc_otg_pcd_remove( struct platform_device *pdev );
extern int32_t dwc_otg_pcd_handle_intr( dwc_otg_pcd_t *pcd );
extern void dwc_otg_pcd_start_srp_timer(dwc_otg_pcd_t *pcd );

extern void dwc_otg_pcd_initiate_srp(dwc_otg_pcd_t *pcd);
extern void dwc_otg_pcd_remote_wakeup(dwc_otg_pcd_t *pcd, int set);

extern void dwc_otg_iso_buffer_done(dwc_otg_pcd_ep_t *ep, dwc_otg_pcd_iso_request_t *req);
extern void dwc_otg_request_done(dwc_otg_pcd_ep_t *_ep, dwc_otg_pcd_request_t *req,
				int status);
extern void dwc_otg_request_nuke(dwc_otg_pcd_ep_t *_ep);
extern void dwc_otg_pcd_update_otg(dwc_otg_pcd_t *_pcd,
					const unsigned reset);
#ifndef VERBOSE
#define VERIFY_PCD_DMA_ADDR(_addr_) 	BUG_ON(((_addr_)==DMA_ADDR_INVALID)||\
				       ((_addr_)==0)||\
				       ((_addr_)&0x3))
#else
#define VERIFY_PCD_DMA_ADDR(_addr_) 	{\
						if(((_addr_)==DMA_ADDR_INVALID)||\
						       ((_addr_)==0)||\
						       ((_addr_)&0x3)) {\
							printk("%s: Invalid DMA address "#_addr_"(%.8x)\n",__func__,_addr_);\
							BUG();\
						}\
					}
#endif


static inline void ep_check_and_patch_dma_addr(dwc_otg_pcd_ep_t *ep){
//void ep_check_and_patch_dma_addr(dwc_otg_pcd_ep_t *ep){
	dwc_ep_t *dwc_ep=&ep->dwc_ep;

DWC_DEBUGPL(DBG_PCDV,"%s: dwc_ep xfer_buf=%.8x, total_len=%d, dma_addr=%.8x\n",__func__,(u32)dwc_ep->xfer_buff,(dwc_ep->total_len),dwc_ep->dma_addr);
	if (/*(core_if->dma_enable)&&*/(dwc_ep->dma_addr==DMA_ADDR_INVALID)) {
		if((((u32)dwc_ep->xfer_buff)&0x3)==0){
			dwc_ep->dma_addr=dma_map_single(NULL,(void *)(dwc_ep->start_xfer_buff),(dwc_ep->total_len), DMA_TO_DEVICE);
DWC_DEBUGPL(DBG_PCDV,"    got dma_addr=%.8x\n",dwc_ep->dma_addr);
		}else{
DWC_DEBUGPL(DBG_PCDV,"    buf not aligned, use aligned_buf instead. xfer_buf=%.8x, total_len=%d, aligned_buf_size=%d\n",(u32)dwc_ep->xfer_buff,(dwc_ep->total_len),dwc_ep->aligned_buf_size);
			if(dwc_ep->aligned_buf_size<dwc_ep->total_len){
				if(dwc_ep->aligned_buf){
//printk("    free buff dwc_ep aligned_buf_size=%d, aligned_buf(%.8x), aligned_dma_addr(%.8x));\n",dwc_ep->aligned_buf_size,dwc_ep->aligned_buf,dwc_ep->aligned_dma_addr);
					//dma_free_coherent(NULL,dwc_ep->aligned_buf_size,dwc_ep->aligned_buf,dwc_ep->aligned_dma_addr);
					kfree(dwc_ep->aligned_buf);
				}
				dwc_ep->aligned_buf_size=((1<<20)>(dwc_ep->total_len<<1))?(dwc_ep->total_len<<1):(1<<20);
				//dwc_ep->aligned_buf = dma_alloc_coherent (NULL, dwc_ep->aligned_buf_size, &dwc_ep->aligned_dma_addr, GFP_KERNEL|GFP_DMA);
				dwc_ep->aligned_buf=kmalloc(dwc_ep->aligned_buf_size,GFP_KERNEL|GFP_DMA|GFP_ATOMIC);
				dwc_ep->aligned_dma_addr=dma_map_single(NULL,(void *)(dwc_ep->aligned_buf),(dwc_ep->aligned_buf_size),DMA_FROM_DEVICE);
				if(!dwc_ep->aligned_buf){
					DWC_ERROR("Cannot alloc required buffer!!\n");
					BUG();
				}
DWC_DEBUGPL(DBG_PCDV,"    dwc_ep allocated aligned buf=%.8x, dma_addr=%.8x, size=%d(0x%x)\n", (u32)dwc_ep->aligned_buf, dwc_ep->aligned_dma_addr, dwc_ep->aligned_buf_size, dwc_ep->aligned_buf_size);
			}
			dwc_ep->dma_addr=dwc_ep->aligned_dma_addr;
			if(dwc_ep->is_in) {
				memcpy(dwc_ep->aligned_buf,dwc_ep->xfer_buff,dwc_ep->total_len);
				dma_sync_single_for_device(NULL,dwc_ep->dma_addr,dwc_ep->total_len,DMA_TO_DEVICE);
			}
		}
	}
}

#endif
#endif /* DWC_HOST_ONLY */
