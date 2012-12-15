/*****************************************************************************
 **   FILE NAME       : ifxusb_cif.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   DESCRIPTION     : The Core Interface provides basic services for accessing and
 **                     managing the IFX USB hardware. These services are used by both the
 **                     Host Controller Driver and the Peripheral Controller Driver.
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
 \file ifxusb_cif.c
 \ingroup IFXUSB_DRIVER_V3
 \brief This file contains the interface to the IFX USB Core.
*/

#include <linux/version.h>
#include "ifxusb_version.h"

#include <asm/byteorder.h>
#include <asm/unaligned.h>

#ifdef __DEBUG__
	#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#endif


#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"


#ifdef __IS_DEVICE__
	#include "ifxpcd.h"
#endif

#ifdef __IS_HOST__
	#include "ifxhcd.h"
#endif

#include <linux/mm.h>

#include <linux/gfp.h>

#include <lantiq_soc.h>

#if defined(__UEIP__)
	#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
		#ifndef USB_CTRL_PMU_SETUP
			#define USB_CTRL_PMU_SETUP(__x) USB0_CTRL_PMU_SETUP(__x)
		#endif
		#ifndef USB_PHY_PMU_SETUP
			#define USB_PHY_PMU_SETUP(__x) USB0_PHY_PMU_SETUP(__x)
		#endif
	#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
#endif // defined(__UEIP__)

/*!
 \brief This function is called to allocate buffer of specified size.
        The allocated buffer is mapped into DMA accessable address.
 \param size Size in BYTE to be allocated
 \param clear 0: don't do clear after buffer allocated, other: do clear to zero
 \return 0/NULL: Fail; uncached pointer of allocated buffer
 */
#ifdef __IS_HOST__
void *ifxusb_alloc_buf_h(size_t size, int clear)
#else
void *ifxusb_alloc_buf_d(size_t size, int clear)
#endif
{
	uint32_t *cached,*uncached;
	uint32_t totalsize,page;

	if(!size)
		return 0;

	size=(size+3)&0xFFFFFFFC;
	totalsize=size + 12;
	page=get_order(totalsize);

	cached = (void *) __get_free_pages(( GFP_ATOMIC | GFP_DMA), page);

	if(!cached)
	{
		IFX_PRINT("%s Allocation Failed size:%d\n",__func__,size);
		return NULL;
	}

	uncached = (uint32_t *)(KSEG1ADDR(cached));
	if(clear)
		memset(uncached, 0, totalsize);

	*(uncached+0)=totalsize;
	*(uncached+1)=page;
	*(uncached+2)=(uint32_t)cached;
	return (void *)(uncached+3);
}


/*!
 \brief This function is called to free allocated buffer.
 \param vaddr the uncached pointer of the buffer
 */
#ifdef __IS_HOST__
void ifxusb_free_buf_h(void *vaddr)
#else
void ifxusb_free_buf_d(void *vaddr)
#endif
{
	uint32_t totalsize,page;
	uint32_t *cached,*uncached;

	if(vaddr != NULL)
	{
		uncached=vaddr;
		uncached-=3;
		totalsize=*(uncached+0);
		page=*(uncached+1);
		cached=(uint32_t *)(*(uncached+2));
		if(totalsize && page==get_order(totalsize) && cached==(uint32_t *)(KSEG0ADDR(uncached)))
		{
			free_pages((unsigned long)cached, page);
			return;
		}
		// the memory is not allocated by ifxusb_alloc_buf. Allowed but must be careful.
		return;
	}
}



/*!
   \brief This function is called to initialize the IFXUSB CSR data
 	 structures.  The register addresses in the device and host
 	 structures are initialized from the base address supplied by the
 	 caller.  The calling function must make the OS calls to get the
 	 base address of the IFXUSB controller registers.

   \param _core_if        Pointer of core_if structure
   \param _irq            irq number
   \param _reg_base_addr  Base address of IFXUSB core registers
   \param _fifo_base_addr Fifo base address
   \param _fifo_dbg_addr  Fifo debug address
   \return 	0: success;
 */
#ifdef __IS_HOST__
int ifxusb_core_if_init_h(ifxusb_core_if_t *_core_if,
#else
int ifxusb_core_if_init_d(ifxusb_core_if_t *_core_if,
#endif
                        int               _irq,
                        uint32_t          _reg_base_addr,
                        uint32_t          _fifo_base_addr,
                        uint32_t          _fifo_dbg_addr)
{
	int retval = 0;
	uint32_t *reg_base  =NULL;
    uint32_t *fifo_base =NULL;
    uint32_t *fifo_dbg  =NULL;

    int i;

	IFX_DEBUGPL(DBG_CILV, "%s(%p,%d,0x%08X,0x%08X,0x%08X)\n", __func__,
	                                             _core_if,
	                                             _irq,
	                                             _reg_base_addr,
	                                             _fifo_base_addr,
	                                             _fifo_dbg_addr);

	if( _core_if == NULL)
	{
		IFX_ERROR("%s() invalid _core_if\n", __func__);
		retval = -ENOMEM;
		goto fail;
	}

	//memset(_core_if, 0, sizeof(ifxusb_core_if_t));

	_core_if->irq=_irq;

	reg_base  =ioremap_nocache(_reg_base_addr , IFXUSB_IOMEM_SIZE  );
	fifo_base =ioremap_nocache(_fifo_base_addr, IFXUSB_FIFOMEM_SIZE);
	fifo_dbg  =ioremap_nocache(_fifo_dbg_addr , IFXUSB_FIFODBG_SIZE);
	if( reg_base == NULL || fifo_base == NULL || fifo_dbg == NULL)
	{
		IFX_ERROR("%s() usb ioremap() failed\n", __func__);
		retval = -ENOMEM;
		goto fail;
	}

	_core_if->core_global_regs = (ifxusb_core_global_regs_t *)reg_base;

	/*
	 * Attempt to ensure this device is really a IFXUSB Controller.
	 * Read and verify the SNPSID register contents. The value should be
	 * 0x45F42XXX
	 */
	{
		int32_t snpsid;
		snpsid = ifxusb_rreg(&_core_if->core_global_regs->gsnpsid);
		if ((snpsid & 0xFFFFF000) != 0x4F542000)
		{
			IFX_ERROR("%s() snpsid error(0x%08x) failed\n", __func__,snpsid);
			retval = -EINVAL;
			goto fail;
		}
		_core_if->snpsid=snpsid;
	}

	#ifdef __IS_HOST__
		_core_if->host_global_regs = (ifxusb_host_global_regs_t *)
		    ((uint32_t)reg_base + IFXUSB_HOST_GLOBAL_REG_OFFSET);
		_core_if->hprt0 = (uint32_t*)((uint32_t)reg_base + IFXUSB_HOST_PORT_REGS_OFFSET);

		for (i=0; i<MAX_EPS_CHANNELS; i++)
		{
			_core_if->hc_regs[i] = (ifxusb_hc_regs_t *)
			    ((uint32_t)reg_base + IFXUSB_HOST_CHAN_REGS_OFFSET +
			    (i * IFXUSB_CHAN_REGS_OFFSET));
			IFX_DEBUGPL(DBG_CILV, "hc_reg[%d]->hcchar=%p\n",
			    i, &_core_if->hc_regs[i]->hcchar);
		}
	#endif //__IS_HOST__

	#ifdef __IS_DEVICE__
		_core_if->dev_global_regs =
		    (ifxusb_device_global_regs_t *)((uint32_t)reg_base + IFXUSB_DEV_GLOBAL_REG_OFFSET);

		for (i=0; i<MAX_EPS_CHANNELS; i++)
		{
			_core_if->in_ep_regs[i] = (ifxusb_dev_in_ep_regs_t *)
			    ((uint32_t)reg_base + IFXUSB_DEV_IN_EP_REG_OFFSET +
			    (i * IFXUSB_EP_REG_OFFSET));
			_core_if->out_ep_regs[i] = (ifxusb_dev_out_ep_regs_t *)
			    ((uint32_t)reg_base + IFXUSB_DEV_OUT_EP_REG_OFFSET +
			    (i * IFXUSB_EP_REG_OFFSET));
			IFX_DEBUGPL(DBG_CILV, "in_ep_regs[%d]->diepctl=%p/%p %p/0x%08X/0x%08X\n",
			    i, &_core_if->in_ep_regs[i]->diepctl, _core_if->in_ep_regs[i],
			    reg_base,IFXUSB_DEV_IN_EP_REG_OFFSET,(i * IFXUSB_EP_REG_OFFSET)
			    );
			IFX_DEBUGPL(DBG_CILV, "out_ep_regs[%d]->doepctl=%p/%p %p/0x%08X/0x%08X\n",
			    i, &_core_if->out_ep_regs[i]->doepctl, _core_if->out_ep_regs[i],
			    reg_base,IFXUSB_DEV_OUT_EP_REG_OFFSET,(i * IFXUSB_EP_REG_OFFSET)
			    );
		}
	#endif //__IS_DEVICE__

	/* Setting the FIFO and other Address. */
	for (i=0; i<MAX_EPS_CHANNELS; i++)
	{
		_core_if->data_fifo[i] = fifo_base + (i * IFXUSB_DATA_FIFO_SIZE);
		IFX_DEBUGPL(DBG_CILV, "data_fifo[%d]=0x%08x\n",
		    i, (unsigned)_core_if->data_fifo[i]);
	}

	_core_if->data_fifo_dbg = fifo_dbg;
	_core_if->pcgcctl = (uint32_t*)(((uint32_t)reg_base) + IFXUSB_PCGCCTL_OFFSET);

	/*
	 * Store the contents of the hardware configuration registers here for
	 * easy access later.
	 */
	_core_if->hwcfg1.d32 = ifxusb_rreg(&_core_if->core_global_regs->ghwcfg1);
	_core_if->hwcfg2.d32 = ifxusb_rreg(&_core_if->core_global_regs->ghwcfg2);
	_core_if->hwcfg3.d32 = ifxusb_rreg(&_core_if->core_global_regs->ghwcfg3);
	_core_if->hwcfg4.d32 = ifxusb_rreg(&_core_if->core_global_regs->ghwcfg4);

	IFX_DEBUGPL(DBG_CILV,"hwcfg1=%08x\n",_core_if->hwcfg1.d32);
	IFX_DEBUGPL(DBG_CILV,"hwcfg2=%08x\n",_core_if->hwcfg2.d32);
	IFX_DEBUGPL(DBG_CILV,"hwcfg3=%08x\n",_core_if->hwcfg3.d32);
	IFX_DEBUGPL(DBG_CILV,"hwcfg4=%08x\n",_core_if->hwcfg4.d32);


	#ifdef __DED_FIFO__
	{
		unsigned int countdown=0xFFFF;
		IFX_PRINT("Waiting for PHY Clock Lock!\n");
		while(--countdown && !( ifxusb_rreg(&_core_if->core_global_regs->grxfsiz) & (1<<9)))
		{
			UDELAY(1);
		}
		if(countdown)
			IFX_PRINT("PHY Clock Locked!\n");
		else
			IFX_PRINT("PHY Clock Not Locked! %08X\n",ifxusb_rreg(&_core_if->core_global_regs->grxfsiz));
	}
	#endif

	/* Create new workqueue and init works */
#if 0
	_core_if->wq_usb = create_singlethread_workqueue(_core_if->core_name);

	if(_core_if->wq_usb == 0)
	{
		IFX_DEBUGPL(DBG_CIL, "Creation of wq_usb failed\n");
		retval = -EINVAL;
		goto fail;
	}

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
		INIT_WORK(&core_if->w_conn_id, w_conn_id_status_change, core_if);
		INIT_WORK(&core_if->w_wkp, w_wakeup_detected, core_if);
	#else
		INIT_WORK(&core_if->w_conn_id, w_conn_id_status_change);
		INIT_DELAYED_WORK(&core_if->w_wkp, w_wakeup_detected);
	#endif
#endif
	return 0;

fail:
	if( reg_base  != NULL) iounmap(reg_base );
	if( fifo_base != NULL) iounmap(fifo_base);
	if( fifo_dbg  != NULL) iounmap(fifo_dbg );
	return retval;
}

/*!
 \brief This function free the mapped address in the IFXUSB CSR data structures.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
void ifxusb_core_if_remove_h(ifxusb_core_if_t *_core_if)
#else
void ifxusb_core_if_remove_d(ifxusb_core_if_t *_core_if)
#endif
{
	/* Disable all interrupts */
	if( _core_if->core_global_regs  != NULL)
	{
		gusbcfg_data_t usbcfg   ={.d32 = 0};
		usbcfg.d32 = ifxusb_rreg( &_core_if->core_global_regs->gusbcfg);
		usbcfg.b.ForceDevMode=0;
		usbcfg.b.ForceHstMode=0;
		ifxusb_wreg( &_core_if->core_global_regs->gusbcfg,usbcfg.d32);
		ifxusb_mreg( &_core_if->core_global_regs->gahbcfg, 1, 0);
		ifxusb_wreg( &_core_if->core_global_regs->gintmsk, 0);
	}

	if( _core_if->core_global_regs  != NULL) iounmap(_core_if->core_global_regs );
	if( _core_if->data_fifo[0]      != NULL) iounmap(_core_if->data_fifo[0]     );
	if( _core_if->data_fifo_dbg     != NULL) iounmap(_core_if->data_fifo_dbg    );

#if 0
	if (_core_if->wq_usb)
		destroy_workqueue(_core_if->wq_usb);
#endif
	memset(_core_if, 0, sizeof(ifxusb_core_if_t));
}




/*!
 \brief This function enbles the controller's Global Interrupt in the AHB Config register.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
void ifxusb_enable_global_interrupts_h( ifxusb_core_if_t *_core_if )
#else
void ifxusb_enable_global_interrupts_d( ifxusb_core_if_t *_core_if )
#endif
{
	gahbcfg_data_t ahbcfg ={ .d32 = 0};
	ahbcfg.b.glblintrmsk = 1; /* Enable interrupts */
	ifxusb_mreg(&_core_if->core_global_regs->gahbcfg, 0, ahbcfg.d32);
}

/*!
 \brief This function disables the controller's Global Interrupt in the AHB Config register.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
void ifxusb_disable_global_interrupts_h( ifxusb_core_if_t *_core_if )
#else
void ifxusb_disable_global_interrupts_d( ifxusb_core_if_t *_core_if )
#endif
{
	gahbcfg_data_t ahbcfg ={ .d32 = 0};
	ahbcfg.b.glblintrmsk = 1; /* Enable interrupts */
	ifxusb_mreg(&_core_if->core_global_regs->gahbcfg, ahbcfg.d32, 0);
}




/*!
 \brief Flush Tx and Rx FIFO.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
void ifxusb_flush_both_fifo_h( ifxusb_core_if_t *_core_if )
#else
void ifxusb_flush_both_fifo_d( ifxusb_core_if_t *_core_if )
#endif
{
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;
	volatile grstctl_t greset ={ .d32 = 0};
	int count = 0;

	IFX_DEBUGPL((DBG_CIL|DBG_PCDV), "%s\n", __func__);
	greset.b.rxfflsh = 1;
	greset.b.txfflsh = 1;
	greset.b.txfnum = 0x10;
	greset.b.intknqflsh=1;
	greset.b.hstfrm=1;
	ifxusb_wreg( &global_regs->grstctl, greset.d32 );

	do
	{
		greset.d32 = ifxusb_rreg( &global_regs->grstctl);
		if (++count > 10000)
		{
			IFX_WARN("%s() HANG! GRSTCTL=%0x\n", __func__, greset.d32);
			break;
		}
	} while (greset.b.rxfflsh == 1 || greset.b.txfflsh == 1);
	/* Wait for 3 PHY Clocks*/
	UDELAY(1);
}

/*!
 \brief Flush a Tx FIFO.
 \param _core_if Pointer of core_if structure
 \param _num Tx FIFO to flush. ( 0x10 for ALL TX FIFO )
 */
#ifdef __IS_HOST__
void ifxusb_flush_tx_fifo_h( ifxusb_core_if_t *_core_if, const int _num )
#else
void ifxusb_flush_tx_fifo_d( ifxusb_core_if_t *_core_if, const int _num )
#endif
{
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;
	volatile grstctl_t greset ={ .d32 = 0};
	int count = 0;

	IFX_DEBUGPL((DBG_CIL|DBG_PCDV), "Flush Tx FIFO %d\n", _num);

	greset.b.intknqflsh=1;
	greset.b.txfflsh = 1;
	greset.b.txfnum = _num;
	ifxusb_wreg( &global_regs->grstctl, greset.d32 );

	do
	{
		greset.d32 = ifxusb_rreg( &global_regs->grstctl);
		if (++count > 10000&&(_num==0 ||_num==0x10))
		{
			IFX_WARN("%s() HANG! GRSTCTL=%0x GNPTXSTS=0x%08x\n",
			    __func__, greset.d32,
			ifxusb_rreg( &global_regs->gnptxsts));
			break;
		}
	} while (greset.b.txfflsh == 1);
	/* Wait for 3 PHY Clocks*/
	UDELAY(1);
}


/*!
 \brief Flush Rx FIFO.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
void ifxusb_flush_rx_fifo_h( ifxusb_core_if_t *_core_if )
#else
void ifxusb_flush_rx_fifo_d( ifxusb_core_if_t *_core_if )
#endif
{
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;
	volatile grstctl_t greset ={ .d32 = 0};
	int count = 0;

	IFX_DEBUGPL((DBG_CIL|DBG_PCDV), "%s\n", __func__);
	greset.b.rxfflsh = 1;
	ifxusb_wreg( &global_regs->grstctl, greset.d32 );

	do
	{
		greset.d32 = ifxusb_rreg( &global_regs->grstctl);
		if (++count > 10000)
		{
			IFX_WARN("%s() HANG! GRSTCTL=%0x\n", __func__, greset.d32);
			break;
		}
	} while (greset.b.rxfflsh == 1);
	/* Wait for 3 PHY Clocks*/
	UDELAY(1);
}


#define SOFT_RESET_DELAY 100 /*!< Delay in msec of detection after soft-reset of usb core */

/*!
 \brief Do a soft reset of the core.  Be careful with this because it
        resets all the internal state machines of the core.
 \param _core_if Pointer of core_if structure
 */
#ifdef __IS_HOST__
int ifxusb_core_soft_reset_h(ifxusb_core_if_t *_core_if)
#else
int ifxusb_core_soft_reset_d(ifxusb_core_if_t *_core_if)
#endif
{
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;
	volatile grstctl_t greset ={ .d32 = 0};
	int count = 0;

	IFX_DEBUGPL(DBG_CILV, "%s\n", __func__);
	/* Wait for AHB master IDLE state. */
	do
	{
		UDELAY(10);
		greset.d32 = ifxusb_rreg( &global_regs->grstctl);
		if (++count > 100000)
		{
			IFX_WARN("%s() HANG! AHB Idle GRSTCTL=%0x %x\n", __func__,
			greset.d32, greset.b.ahbidle);
			break;
		}
	} while (greset.b.ahbidle == 0);

	UDELAY(1);

	/* Core Soft Reset */
	count = 0;
	greset.b.csftrst = 1;
	ifxusb_wreg( &global_regs->grstctl, greset.d32 );

	#ifdef SOFT_RESET_DELAY
		MDELAY(SOFT_RESET_DELAY);
	#endif

	do
	{
		UDELAY(10);
		greset.d32 = ifxusb_rreg( &global_regs->grstctl);
		if (++count > 100000)
		{
			IFX_WARN("%s() HANG! Soft Reset GRSTCTL=%0x\n", __func__, greset.d32);
			return -1;
		}
	} while (greset.b.csftrst == 1);

	#ifdef SOFT_RESET_DELAY
		MDELAY(SOFT_RESET_DELAY);
	#endif

	// This is to reset the PHY of VR9
	#if defined(__IS_VR9__)
		if(_core_if->core_no==0)
		{
			set_bit (4, VR9_RCU_USBRESET2);
			MDELAY(50);
			clear_bit (4, VR9_RCU_USBRESET2);
		}
		else
		{
			set_bit (5, VR9_RCU_USBRESET2);
			MDELAY(50);
			clear_bit (5, VR9_RCU_USBRESET2);
		}
		MDELAY(50);
	#endif //defined(__IS_VR9__)

	IFX_PRINT("USB core #%d soft-reset\n",_core_if->core_no);

	return 0;
}

/*!
 \brief Turn on the USB Core Power
 \param _core_if Pointer of core_if structure
*/
#ifdef __IS_HOST__
void ifxusb_power_on_h (ifxusb_core_if_t *_core_if)
#else
void ifxusb_power_on_d (ifxusb_core_if_t *_core_if)
#endif
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#if defined(__UEIP__)

		// set clock gating
		#if defined(__IS_TWINPASS) || defined(__IS_DANUBE__)
			set_bit (4, (volatile unsigned long *)DANUBE_CGU_IFCCR);
			set_bit (5, (volatile unsigned long *)DANUBE_CGU_IFCCR);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
		//	clear_bit (4, (volatile unsigned long *)AMAZON_SE_CGU_IFCCR);
			clear_bit (5, (volatile unsigned long *)AMAZON_SE_CGU_IFCCR);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			set_bit (0, (volatile unsigned long *)AR9_CGU_IFCCR);
			set_bit (1, (volatile unsigned long *)AR9_CGU_IFCCR);
		#endif //defined(__IS_AR9__)
		#if defined(__IS_VR9__)
//			set_bit (0, (volatile unsigned long *)VR9_CGU_IFCCR);
//			set_bit (1, (volatile unsigned long *)VR9_CGU_IFCCR);
		#endif //defined(__IS_VR9__)
		#if defined(__IS_AR10__)
//			set_bit (0, (volatile unsigned long *)VR9_CGU_IFCCR);
//			set_bit (1, (volatile unsigned long *)VR9_CGU_IFCCR);
		#endif //defined(__IS_AR10__)

		MDELAY(50);
#define PMU_AHBM        BIT(15)
#define PMU_USB0        BIT(6)
#define PMU_USB1        BIT(27)
#define PMU_USB0_P      BIT(0)
#define PMU_USB1_P      BIT(26)
		// set power
		ltq_pmu_enable(PMU_AHBM);
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
			ltq_pmu_enable(PMU_USB0);
			//#if defined(__IS_TWINPASS__)
			//	ifxusb_enable_afe_oc();
			//#endif
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__) || defined(__IS_VR9__)
			if(_core_if->core_no==0)
				ltq_pmu_enable(PMU_USB0);
			else
				ltq_pmu_enable(PMU_USB1);
		#endif //defined(__IS_AR9__) || defined(__IS_VR9__)
		#if defined(__IS_AR10__)
			//if(_core_if->core_no==0)
			//	USB0_CTRL_PMU_SETUP(IFX_PMU_ENABLE);
			//else
			//	USB1_CTRL_PMU_SETUP(IFX_PMU_ENABLE);
		#endif //defined(__IS_AR10__)

		MDELAY(50);

		if(_core_if->pcgcctl)
		{
			pcgcctl_data_t pcgcctl = {.d32=0};
			pcgcctl.b.gatehclk = 1;
			ifxusb_mreg(_core_if->pcgcctl, pcgcctl.d32, 0);
		}


		if(_core_if->core_global_regs)
		{
			// PHY configurations.
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
			#if defined(__IS_VR9__)
				//ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_VR9__)
			#if defined(__IS_AR10__)
				//ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR10__)
		}
	#else //defined(__UEIP__)
		// set clock gating
		#if defined(__IS_TWINPASS) || defined(__IS_DANUBE__)
			set_bit (4, (volatile unsigned long *)DANUBE_CGU_IFCCR);
			set_bit (5, (volatile unsigned long *)DANUBE_CGU_IFCCR);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
		//	clear_bit (4, (volatile unsigned long *)AMAZON_SE_CGU_IFCCR);
			clear_bit (5, (volatile unsigned long *)AMAZON_SE_CGU_IFCCR);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			set_bit (0, (volatile unsigned long *)AMAZON_S_CGU_IFCCR);
			set_bit (1, (volatile unsigned long *)AMAZON_S_CGU_IFCCR);
		#endif //defined(__IS_AR9__)

		MDELAY(50);

		// set power
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			clear_bit (6,  (volatile unsigned long *)DANUBE_PMU_PWDCR);//USB
			clear_bit (9,  (volatile unsigned long *)DANUBE_PMU_PWDCR);//DSL
			clear_bit (15, (volatile unsigned long *)DANUBE_PMU_PWDCR);//AHB
			#if defined(__IS_TWINPASS__)
				ifxusb_enable_afe_oc();
			#endif
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			clear_bit (6,  (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);
			clear_bit (9,  (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);
			clear_bit (15, (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
				clear_bit (6, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//USB
			else
				clear_bit (27, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//USB
			clear_bit (9, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//DSL
			clear_bit (15, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//AHB
		#endif //defined(__IS_AR9__)

		if(_core_if->core_global_regs)
		{
			// PHY configurations.
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
		}

	#endif //defined(__UEIP__)
}

/*!
 \brief Turn off the USB Core Power
 \param _core_if Pointer of core_if structure
*/
#ifdef __IS_HOST__
void ifxusb_power_off_h (ifxusb_core_if_t *_core_if)
#else
void ifxusb_power_off_d (ifxusb_core_if_t *_core_if)
#endif

{
	#ifdef __IS_HOST__
	ifxusb_phy_power_off_h (_core_if);
	#else
	ifxusb_phy_power_off_d (_core_if);
	#endif

	#if defined(__UEIP__)
		//AHBM_PMU_SETUP(IFX_PMU_DISABLE);
		// set power
		if(_core_if->pcgcctl)
		{
			pcgcctl_data_t pcgcctl = {.d32=0};
			pcgcctl.b.gatehclk = 1;
			pcgcctl.b.stoppclk = 1;
			ifxusb_mreg(_core_if->pcgcctl, 0, pcgcctl.d32);
		}
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
			//USB_CTRL_PMU_SETUP(IFX_PMU_DISABLE);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__) || defined(__IS_VR9__)
		/*	if(_core_if->core_no==0)
				USB0_CTRL_PMU_SETUP(IFX_PMU_DISABLE);
			else
				USB1_CTRL_PMU_SETUP(IFX_PMU_DISABLE);*/
		#endif //defined(__IS_AR9__) || defined(__IS_VR9__)
		#if defined(__IS_AR10__)
			//if(_core_if->core_no==0)
			//	USB0_CTRL_PMU_SETUP(IFX_PMU_DISABLE);
			//else
			//	USB1_CTRL_PMU_SETUP(IFX_PMU_DISABLE);
		#endif //defined(__IS_AR10__)
	#else //defined(__UEIP__)
		// set power
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			set_bit (6, (volatile unsigned long *)DANUBE_PMU_PWDCR);//USB
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			set_bit (6, (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);//USB
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
				set_bit (6, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//USB
			else
				set_bit (27, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//USB
		#endif //defined(__IS_AR9__)
	#endif //defined(__UEIP__)
}

/*!
 \brief Turn on the USB PHY Power
 \param _core_if Pointer of core_if structure
*/
#ifdef __IS_HOST__
void ifxusb_phy_power_on_h (ifxusb_core_if_t *_core_if)
#else
void ifxusb_phy_power_on_d (ifxusb_core_if_t *_core_if)
#endif
{
	#if defined(__UEIP__)
		if(_core_if->core_global_regs)
		{
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
			#if ( defined(__IS_VR9__) || defined(__IS_AR10__)) && defined(__PHY_LONG_PREEMP__)
				if(_core_if->core_no==0)
					set_bit (0, VR9_RCU_USB_ANA_CFG1A);
				else
					set_bit (0, VR9_RCU_USB_ANA_CFG1B);
			#endif //( defined(__IS_VR9__) || defined(__IS_AR10__)) && defined(__PHY_LONG_PREEMP__)

			if(_core_if->pcgcctl)
			{
				pcgcctl_data_t pcgcctl = {.d32=0};
				pcgcctl.b.stoppclk = 1;
				ifxusb_mreg(_core_if->pcgcctl, pcgcctl.d32, 0);
			}
		}

		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
			ltq_pmu_enable(PMU_USB0_P);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__) || defined(__IS_VR9__) || defined(__IS_AR10__)
			if(_core_if->core_no==0)
				ltq_pmu_enable(PMU_USB0_P);
			else
				ltq_pmu_enable(PMU_USB1_P);
		#endif //defined(__IS_AR9__) || defined(__IS_VR9__)

		// PHY configurations.
		if(_core_if->core_global_regs)
		{
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
			#if ( defined(__IS_VR9__) || defined(__IS_AR10__)) && defined(__PHY_LONG_PREEMP__)
				if(_core_if->core_no==0)
					set_bit (0, VR9_RCU_USB_ANA_CFG1A);
				else
					set_bit (0, VR9_RCU_USB_ANA_CFG1B);
			#endif //( defined(__IS_VR9__) || defined(__IS_AR10__)) && defined(__PHY_LONG_PREEMP__)
		}
	#else //defined(__UEIP__)
		// PHY configurations.
		if(_core_if->core_global_regs)
		{
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
		}

		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			clear_bit (0,  (volatile unsigned long *)DANUBE_PMU_PWDCR);//PHY
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			clear_bit (0,  (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
				clear_bit (0,  (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//PHY
			else
				clear_bit (26, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//PHY
		#endif //defined(__IS_AR9__)

		// PHY configurations.
		if(_core_if->core_global_regs)
		{
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
		}
	#endif //defined(__UEIP__)
}


/*!
 \brief Turn off the USB PHY Power
 \param _core_if Pointer of core_if structure
*/
#ifdef __IS_HOST__
void ifxusb_phy_power_off_h (ifxusb_core_if_t *_core_if)
#else
void ifxusb_phy_power_off_d (ifxusb_core_if_t *_core_if)
#endif
{
	#if defined(__UEIP__)
		if(_core_if->pcgcctl)
		{
			pcgcctl_data_t pcgcctl = {.d32=0};
			pcgcctl.b.stoppclk = 1;
			ifxusb_mreg(_core_if->pcgcctl, 0, pcgcctl.d32);
		}
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
			//USB_PHY_PMU_SETUP(IFX_PMU_DISABLE);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__) || defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__) || defined(__IS_VR9__) || defined(__IS_AR10__)
/*			if(_core_if->core_no==0)
				USB0_PHY_PMU_SETUP(IFX_PMU_DISABLE);
			else
				USB1_PHY_PMU_SETUP(IFX_PMU_DISABLE);*/
		#endif // defined(__IS_AR9__) || defined(__IS_VR9__)
	#else //defined(__UEIP__)
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			set_bit (0, (volatile unsigned long *)DANUBE_PMU_PWDCR);//PHY
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			set_bit (0, (volatile unsigned long *)AMAZON_SE_PMU_PWDCR);//PHY
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
				set_bit (0, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//PHY
			else
				set_bit (26, (volatile unsigned long *)AMAZON_S_PMU_PWDCR);//PHY
		#endif //defined(__IS_AR9__)
	#endif //defined(__UEIP__)
}


/*!
 \brief Reset on the USB Core RCU
 \param _core_if Pointer of core_if structure
 */
#if defined(__IS_VR9__) || defined(__IS_AR10__)
static int CheckAlready(void)
{
	gusbcfg_data_t usbcfg   ={.d32 = 0};
	usbcfg.d32 = ifxusb_rreg((volatile uint32_t *)0xBE10100C);
	if(usbcfg.b.ForceDevMode)
		return 1;
	if(usbcfg.b.ForceHstMode)
		return 1;
	usbcfg.d32 = ifxusb_rreg((volatile uint32_t *)0xBE10600C);
	if(usbcfg.b.ForceDevMode)
		return 1;
	if(usbcfg.b.ForceHstMode)
		return 1;
	return 0;
}
#endif

#ifdef __IS_HOST__
	void ifxusb_hard_reset_h(ifxusb_core_if_t *_core_if)
#else
	void ifxusb_hard_reset_d(ifxusb_core_if_t *_core_if)
#endif
{
	#if defined(__UEIP__)
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined (__IS_HOST__)
				clear_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			#elif defined (__IS_DEVICE__)
				set_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			#endif
		#endif //defined(__IS_AMAZON_SE__)

		#if defined(__IS_AMAZON_SE__)
			#if defined (__IS_HOST__)
				clear_bit (AMAZON_SE_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			#elif defined (__IS_DEVICE__)
				set_bit (AMAZON_SE_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			#endif
		#endif //defined(__IS_AMAZON_SE__)

		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				#if defined (__IS_HOST__)
					clear_bit (AR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR9_RCU_USB1CFG);
				#elif defined (__IS_DEVICE__)
					set_bit (AR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR9_RCU_USB1CFG);
				#endif
			}
			else
			{
				#if defined (__IS_HOST__)
					clear_bit (AR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR9_RCU_USB2CFG);
				#elif defined (__IS_DEVICE__)
					set_bit (AR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR9_RCU_USB2CFG);
				#endif
			}
		#endif //defined(__IS_AR9__)

		#if defined(__IS_VR9__)
			if(!CheckAlready())
			{
				#if defined (__IS_HOST__)
					#if   defined (__IS_DUAL__)
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
					#elif defined (__IS_FIRST__)
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
						set_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
					#elif defined (__IS_SECOND__)
						set_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
					#endif
				#endif
				#if defined (__IS_DEVICE__)
					#if   defined (__IS_FIRST__)
						set_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
					#elif defined (__IS_SECOND__)
						clear_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
						set_bit (VR9_USBCFG_HDSEL_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
					#endif
				#endif
			}
		#endif //defined(__IS_VR9__)

		#if defined(__IS_AR10__)
			if(!CheckAlready())
			{
				#if defined (__IS_HOST__)
					#if   defined (__IS_DUAL__)
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
					#elif defined (__IS_FIRST__)
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
						set_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
					#elif defined (__IS_SECOND__)
						set_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
					#endif
				#endif
				#if defined (__IS_DEVICE__)
					#if   defined (__IS_FIRST__)
						set_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
					#elif defined (__IS_SECOND__)
						clear_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
						set_bit (AR10_USBCFG_HDSEL_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
					#endif
				#endif
			}
		#endif //defined(__IS_AR10__)

		// set the HC's byte-order to big-endian
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			set_bit   (DANUBE_USBCFG_HOST_END_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			clear_bit (DANUBE_USBCFG_SLV_END_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			set_bit (AMAZON_SE_USBCFG_HOST_END_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			clear_bit (AMAZON_SE_USBCFG_SLV_END_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				set_bit   (AR9_USBCFG_HOST_END_BIT, (volatile unsigned long *)AR9_RCU_USB1CFG);
				clear_bit (AR9_USBCFG_SLV_END_BIT, (volatile unsigned long *)AR9_RCU_USB1CFG);
			}
			else
			{
				set_bit   (AR9_USBCFG_HOST_END_BIT, (volatile unsigned long *)AR9_RCU_USB2CFG);
				clear_bit (AR9_USBCFG_SLV_END_BIT, (volatile unsigned long *)AR9_RCU_USB2CFG);
			}
		#endif //defined(__IS_AR9__)
		#if defined(__IS_VR9__)
			if(_core_if->core_no==0)
			{
				set_bit   (VR9_USBCFG_HOST_END_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
				clear_bit (VR9_USBCFG_SLV_END_BIT, (volatile unsigned long *)VR9_RCU_USB1CFG);
			}
			else
			{
				set_bit   (VR9_USBCFG_HOST_END_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
				clear_bit (VR9_USBCFG_SLV_END_BIT, (volatile unsigned long *)VR9_RCU_USB2CFG);
			}
		#endif //defined(__IS_VR9__)
		#if defined(__IS_AR10__)
			if(_core_if->core_no==0)
			{
				set_bit   (AR10_USBCFG_HOST_END_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
				clear_bit (AR10_USBCFG_SLV_END_BIT, (volatile unsigned long *)AR10_RCU_USB1CFG);
			}
			else
			{
				set_bit   (AR10_USBCFG_HOST_END_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
				clear_bit (AR10_USBCFG_SLV_END_BIT, (volatile unsigned long *)AR10_RCU_USB2CFG);
			}
		#endif //defined(__IS_AR10__)

		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		    set_bit (4, DANUBE_RCU_RESET);
			MDELAY(50);
		    clear_bit (4, DANUBE_RCU_RESET);
			MDELAY(50);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)

		#if defined(__IS_AMAZON_SE__)
		    set_bit (4, AMAZON_SE_RCU_RESET);
			MDELAY(50);
		    clear_bit (4, AMAZON_SE_RCU_RESET);
			MDELAY(50);
		#endif //defined(__IS_AMAZON_SE__)

		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				set_bit (4, AR9_RCU_USBRESET);
				MDELAY(50);
				clear_bit (4, AR9_RCU_USBRESET);
			}
			else
			{
				set_bit (28, AR9_RCU_USBRESET);
				MDELAY(50);
				clear_bit (28, AR9_RCU_USBRESET);
			}
			MDELAY(50);
		#endif //defined(__IS_AR9__)
		#if defined(__IS_VR9__)
			if(!CheckAlready())
			{
				set_bit (4, VR9_RCU_USBRESET);
				MDELAY(50);
				clear_bit (4, VR9_RCU_USBRESET);
				MDELAY(50);
			}
		#endif //defined(__IS_VR9__)
		#if defined(__IS_AR10__)
			if(!CheckAlready())
			{
				set_bit (4, AR10_RCU_USBRESET);
				MDELAY(50);
				clear_bit (4, AR10_RCU_USBRESET);
				MDELAY(50);
			}
		#endif //defined(__IS_AR10__)

		#if defined(__IS_TWINPASS__)
			ifxusb_enable_afe_oc();
		#endif

		if(_core_if->core_global_regs)
		{
			// PHY configurations.
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
			#if defined(__IS_VR9__)
			//	ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_VR9__)
			#if defined(__IS_AR10__)
			//	ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR10__)
		}
	#else //defined(__UEIP__)
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined (__IS_HOST__)
				clear_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			#elif defined (__IS_DEVICE__)
				set_bit (DANUBE_USBCFG_HDSEL_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			#endif
		#endif //defined(__IS_AMAZON_SE__)

		#if defined(__IS_AMAZON_SE__)
			#if defined (__IS_HOST__)
				clear_bit (AMAZON_SE_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			#elif defined (__IS_DEVICE__)
				set_bit (AMAZON_SE_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			#endif
		#endif //defined(__IS_AMAZON_SE__)

		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				#if defined (__IS_HOST__)
					clear_bit (AMAZON_S_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB1CFG);
				#elif defined (__IS_DEVICE__)
					set_bit (AMAZON_S_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB1CFG);
				#endif
			}
			else
			{
				#if defined (__IS_HOST__)
					clear_bit (AMAZON_S_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB2CFG);
				#elif defined (__IS_DEVICE__)
					set_bit (AMAZON_S_USBCFG_HDSEL_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB2CFG);
				#endif
			}
		#endif //defined(__IS_AR9__)

		// set the HC's byte-order to big-endian
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			set_bit   (DANUBE_USBCFG_HOST_END_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
			clear_bit (DANUBE_USBCFG_SLV_END_BIT, (volatile unsigned long *)DANUBE_RCU_USBCFG);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			set_bit (AMAZON_SE_USBCFG_HOST_END_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
			clear_bit (AMAZON_SE_USBCFG_SLV_END_BIT, (volatile unsigned long *)AMAZON_SE_RCU_USBCFG);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				set_bit   (AMAZON_S_USBCFG_HOST_END_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB1CFG);
				clear_bit (AMAZON_S_USBCFG_SLV_END_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB1CFG);
			}
			else
			{
				set_bit   (AMAZON_S_USBCFG_HOST_END_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB2CFG);
				clear_bit (AMAZON_S_USBCFG_SLV_END_BIT, (volatile unsigned long *)AMAZON_S_RCU_USB2CFG);
			}
		#endif //defined(__IS_AR9__)

		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		    set_bit (4, DANUBE_RCU_RESET);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
		    set_bit (4, AMAZON_SE_RCU_RESET);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				set_bit (4, AMAZON_S_RCU_USBRESET);
			}
			else
			{
				set_bit (28, AMAZON_S_RCU_USBRESET);
			}
		#endif //defined(__IS_AR9__)

		MDELAY(50);

		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		    clear_bit (4, DANUBE_RCU_RESET);
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
		    clear_bit (4, AMAZON_SE_RCU_RESET);
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				clear_bit (4, AMAZON_S_RCU_USBRESET);
			}
			else
			{
				clear_bit (28, AMAZON_S_RCU_USBRESET);
			}
		#endif //defined(__IS_AR9__)

		MDELAY(50);

		#if defined(__IS_TWINPASS__)
			ifxusb_enable_afe_oc();
		#endif

		if(_core_if->core_global_regs)
		{
			// PHY configurations.
			#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			#if defined(__IS_AMAZON_SE__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AMAZON_SE__)
			#if defined(__IS_AR9__)
				ifxusb_wreg (&_core_if->core_global_regs->guid,0x14014);
			#endif //defined(__IS_AR9__)
		}
	#endif //defined(__UEIP__)
}

#if defined(__GADGET_LED__) || defined(__HOST_LED__)
	#if defined(__UEIP__)
		static void *g_usb_led_trigger  = NULL;
	#endif

	void ifxusb_led_init(ifxusb_core_if_t *_core_if)
	{
		#if defined(__UEIP__)
			#if defined(IFX_LEDGPIO_USB_LED) || defined(IFX_LEDLED_USB_LED)
				if ( !g_usb_led_trigger )
				{
					ifx_led_trigger_register("usb_link", &g_usb_led_trigger);
					if ( g_usb_led_trigger != NULL )
					{
						struct ifx_led_trigger_attrib attrib = {0};
						attrib.delay_on     = 250;
						attrib.delay_off    = 250;
						attrib.timeout      = 2000;
						attrib.def_value    = 1;
						attrib.flags        = IFX_LED_TRIGGER_ATTRIB_DELAY_ON | IFX_LED_TRIGGER_ATTRIB_DELAY_OFF | IFX_LED_TRIGGER_ATTRIB_TIMEOUT | IFX_LED_TRIGGER_ATTRIB_DEF_VALUE;
						IFX_DEBUGP("Reg USB LED!!\n");
						ifx_led_trigger_set_attrib(g_usb_led_trigger, &attrib);
					}
				}
			#endif
		#endif //defined(__UEIP__)
	}

	void ifxusb_led_free(ifxusb_core_if_t *_core_if)
	{
		#if defined(__UEIP__)
			if ( g_usb_led_trigger )
			{
			    ifx_led_trigger_deregister(g_usb_led_trigger);
			    g_usb_led_trigger = NULL;
			}
		#endif //defined(__UEIP__)
	}

	/*!
	   \brief Turn off the USB 5V VBus Power
	   \param _core_if        Pointer of core_if structure
	 */
	void ifxusb_led(ifxusb_core_if_t *_core_if)
	{
		#if defined(__UEIP__)
			if(g_usb_led_trigger)
				ifx_led_trigger_activate(g_usb_led_trigger);
		#else
		#endif //defined(__UEIP__)
	}
#endif // defined(__GADGET_LED__) || defined(__HOST_LED__)



/*!
 \brief internal routines for debugging
 */
#ifdef __IS_HOST__
void ifxusb_dump_msg_h(const u8 *buf, unsigned int length)
#else
void ifxusb_dump_msg_d(const u8 *buf, unsigned int length)
#endif
{
#ifdef __DEBUG__
	unsigned int	start, num, i;
	char		line[52], *p;

	if (length >= 512)
		return;
	start = 0;
	while (length > 0)
	{
		num = min(length, 16u);
		p = line;
		for (i = 0; i < num; ++i)
		{
			if (i == 8)
				*p++ = ' ';
			sprintf(p, " %02x", buf[i]);
			p += 3;
		}
		*p = 0;
		IFX_PRINT( "%6x: %s\n", start, line);
		buf += num;
		start += num;
		length -= num;
	}
#endif
}

/*!
 \brief internal routines for debugging, reads the SPRAM and prints its content
 */
#ifdef __IS_HOST__
void ifxusb_dump_spram_h(ifxusb_core_if_t *_core_if)
#else
void ifxusb_dump_spram_d(ifxusb_core_if_t *_core_if)
#endif
{
#ifdef __ENABLE_DUMP__
	volatile uint8_t *addr, *start_addr, *end_addr;
	uint32_t size;
	IFX_PRINT("SPRAM Data:\n");
	start_addr = (void*)_core_if->core_global_regs;
	IFX_PRINT("Base Address: 0x%8X\n", (uint32_t)start_addr);

	start_addr = (void*)_core_if->data_fifo_dbg;
	IFX_PRINT("Starting Address: 0x%8X\n", (uint32_t)start_addr);

	size=_core_if->hwcfg3.b.dfifo_depth;
	size<<=2;
	size+=0x200;
	size&=0x0003FFFC;

	end_addr = (void*)_core_if->data_fifo_dbg;
	end_addr += size;

	for(addr = start_addr; addr < end_addr; addr+=16)
	{
		IFX_PRINT("0x%8X:  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X \n", (uint32_t)addr,
			addr[ 0], addr[ 1], addr[ 2], addr[ 3],
			addr[ 4], addr[ 5], addr[ 6], addr[ 7],
			addr[ 8], addr[ 9], addr[10], addr[11],
			addr[12], addr[13], addr[14], addr[15]
			);
	}
	return;
#endif //__ENABLE_DUMP__
}

/*!
 \brief internal routines for debugging, reads the core global registers and prints them
 */
#ifdef __IS_HOST__
void ifxusb_dump_registers_h(ifxusb_core_if_t *_core_if)
#else
void ifxusb_dump_registers_d(ifxusb_core_if_t *_core_if)
#endif
{
#ifdef __ENABLE_DUMP__
	int i;
	volatile uint32_t *addr;
	#ifdef __IS_DEVICE__
		volatile uint32_t *addri,*addro;
	#endif

	IFX_PRINT("Core #%d\n",_core_if->core_no);
	IFX_PRINT("========================================\n");
	IFX_PRINT("Core Global Registers\n");
	addr=&_core_if->core_global_regs->gotgctl;
	IFX_PRINT("  GOTGCTL   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gotgint;
	IFX_PRINT("  GOTGINT   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gahbcfg;
	IFX_PRINT("  GAHBCFG   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gusbcfg;
	IFX_PRINT("  GUSBCFG   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->grstctl;
	IFX_PRINT("  GRSTCTL   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gintsts;
	IFX_PRINT("  GINTSTS   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gintmsk;
	IFX_PRINT("  GINTMSK   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gi2cctl;
	IFX_PRINT("  GI2CCTL   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gpvndctl;
	IFX_PRINT("  GPVNDCTL  @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->ggpio;
	IFX_PRINT("  GGPIO     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->guid;
	IFX_PRINT("  GUID      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->gsnpsid;
	IFX_PRINT("  GSNPSID   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->ghwcfg1;
	IFX_PRINT("  GHWCFG1   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->ghwcfg2;
	IFX_PRINT("  GHWCFG2   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->ghwcfg3;
	IFX_PRINT("  GHWCFG3   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	addr=&_core_if->core_global_regs->ghwcfg4;
	IFX_PRINT("  GHWCFG4   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));

	addr=_core_if->pcgcctl;
	IFX_PRINT("  PCGCCTL   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));

	addr=&_core_if->core_global_regs->grxfsiz;
	IFX_PRINT("  GRXFSIZ   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));

	#ifdef __IS_HOST__
		addr=&_core_if->core_global_regs->gnptxfsiz;
		IFX_PRINT("  GNPTXFSIZ @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->core_global_regs->hptxfsiz;
		IFX_PRINT("  HPTXFSIZ  @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
	#endif //__IS_HOST__

	#ifdef __IS_DEVICE__
		#ifdef __DED_FIFO__
			addr=&_core_if->core_global_regs->gnptxfsiz;
			IFX_PRINT("    GNPTXFSIZ @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			for (i=0; i<= _core_if->hwcfg4.b.num_in_eps; i++)
			{
				addr=&_core_if->core_global_regs->dptxfsiz_dieptxf[i];
				IFX_PRINT("    DPTXFSIZ[%d] @0x%08X : 0x%08X\n",i,(uint32_t)addr,ifxusb_rreg(addr));
			}
		#else
			addr=&_core_if->core_global_regs->gnptxfsiz;
			IFX_PRINT("    TXFSIZ[00] @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			for (i=0; i< _core_if->hwcfg4.b.num_dev_perio_in_ep; i++)
			{
				addr=&_core_if->core_global_regs->dptxfsiz_dieptxf[i];
				IFX_PRINT("    TXFSIZ[%02d] @0x%08X : 0x%08X\n",i+1,(uint32_t)addr,ifxusb_rreg(addr));
			}
		#endif
	#endif //__IS_DEVICE__

	#ifdef __IS_HOST__
		IFX_PRINT("  Host Global Registers\n");
		addr=&_core_if->host_global_regs->hcfg;
		IFX_PRINT("    HCFG      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->host_global_regs->hfir;
		IFX_PRINT("    HFIR      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->host_global_regs->hfnum;
		IFX_PRINT("    HFNUM     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->host_global_regs->hptxsts;
		IFX_PRINT("    HPTXSTS   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->host_global_regs->haint;
		IFX_PRINT("    HAINT     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->host_global_regs->haintmsk;
		IFX_PRINT("    HAINTMSK  @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr= _core_if->hprt0;
		IFX_PRINT("    HPRT0     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));

		for (i=0; i<MAX_EPS_CHANNELS; i++)
		{
			addr=&_core_if->hc_regs[i]->hcchar;
			IFX_PRINT("  Host Channel %d Specific Registers\n", i);
			IFX_PRINT("    HCCHAR    @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			addr=&_core_if->hc_regs[i]->hcsplt;
			IFX_PRINT("    HCSPLT    @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			addr=&_core_if->hc_regs[i]->hcint;
			IFX_PRINT("    HCINT     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			addr=&_core_if->hc_regs[i]->hcintmsk;
			IFX_PRINT("    HCINTMSK  @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			addr=&_core_if->hc_regs[i]->hctsiz;
			IFX_PRINT("    HCTSIZ    @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
			addr=&_core_if->hc_regs[i]->hcdma;
			IFX_PRINT("    HCDMA     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		}
	#endif //__IS_HOST__

	#ifdef __IS_DEVICE__
		IFX_PRINT("  Device Global Registers\n");
		addr=&_core_if->dev_global_regs->dcfg;
		IFX_PRINT("    DCFG      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->dctl;
		IFX_PRINT("    DCTL      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->dsts;
		IFX_PRINT("    DSTS      @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->diepmsk;
		IFX_PRINT("    DIEPMSK   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->doepmsk;
		IFX_PRINT("    DOEPMSK   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->daintmsk;
		IFX_PRINT("    DAINTMSK  @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->daint;
		IFX_PRINT("    DAINT     @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->dvbusdis;
		IFX_PRINT("    DVBUSID   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		addr=&_core_if->dev_global_regs->dvbuspulse;
		IFX_PRINT("    DVBUSPULS @0x%08X : 0x%08X\n", (uint32_t)addr,ifxusb_rreg(addr));

		addr=&_core_if->dev_global_regs->dtknqr1;
		IFX_PRINT("    DTKNQR1   @0x%08X : 0x%08X\n",(uint32_t)addr,ifxusb_rreg(addr));
		if (_core_if->hwcfg2.b.dev_token_q_depth > 6) {
			addr=&_core_if->dev_global_regs->dtknqr2;
			IFX_PRINT("    DTKNQR2   @0x%08X : 0x%08X\n", (uint32_t)addr,ifxusb_rreg(addr));
		}

		if (_core_if->hwcfg2.b.dev_token_q_depth > 14)
		{
			addr=&_core_if->dev_global_regs->dtknqr3_dthrctl;
			IFX_PRINT("    DTKNQR3_DTHRCTL  @0x%08X : 0x%08X\n", (uint32_t)addr, ifxusb_rreg(addr));
		}

		if (_core_if->hwcfg2.b.dev_token_q_depth > 22)
		{
			addr=&_core_if->dev_global_regs->dtknqr4_fifoemptymsk;
			IFX_PRINT("    DTKNQR4  @0x%08X : 0x%08X\n", (uint32_t)addr, ifxusb_rreg(addr));
		}

		//for (i=0; i<= MAX_EPS_CHANNELS; i++)
		//for (i=0; i<= 10; i++)
		for (i=0; i<= 3; i++)
		{
			IFX_PRINT("  Device EP %d Registers\n", i);
			addri=&_core_if->in_ep_regs[i]->diepctl;addro=&_core_if->out_ep_regs[i]->doepctl;
			IFX_PRINT("    DEPCTL    I: 0x%08X O: 0x%08X\n",ifxusb_rreg(addri),ifxusb_rreg(addro));
			                                        addro=&_core_if->out_ep_regs[i]->doepfn;
			IFX_PRINT("    DEPFN     I:            O: 0x%08X\n",ifxusb_rreg(addro));
			addri=&_core_if->in_ep_regs[i]->diepint;addro=&_core_if->out_ep_regs[i]->doepint;
			IFX_PRINT("    DEPINT    I: 0x%08X O: 0x%08X\n",ifxusb_rreg(addri),ifxusb_rreg(addro));
			addri=&_core_if->in_ep_regs[i]->dieptsiz;addro=&_core_if->out_ep_regs[i]->doeptsiz;
			IFX_PRINT("    DETSIZ    I: 0x%08X O: 0x%08X\n",ifxusb_rreg(addri),ifxusb_rreg(addro));
			addri=&_core_if->in_ep_regs[i]->diepdma;addro=&_core_if->out_ep_regs[i]->doepdma;
			IFX_PRINT("    DEPDMA    I: 0x%08X O: 0x%08X\n",ifxusb_rreg(addri),ifxusb_rreg(addro));
			addri=&_core_if->in_ep_regs[i]->dtxfsts;
			IFX_PRINT("    DTXFSTS   I: 0x%08X\n",ifxusb_rreg(addri)                   );
			addri=&_core_if->in_ep_regs[i]->diepdmab;addro=&_core_if->out_ep_regs[i]->doepdmab;
			IFX_PRINT("    DEPDMAB   I: 0x%08X O: 0x%08X\n",ifxusb_rreg(addri),ifxusb_rreg(addro));
		}
	#endif //__IS_DEVICE__
#endif //__ENABLE_DUMP__
}

#ifdef __IS_HOST__
void do_suspend_h(ifxusb_core_if_t *core_if)
{
	ifxusb_vbus_off(core_if);
	mdelay(100);
	ifxusb_power_off_h(core_if);
}
void do_resume_h(ifxusb_core_if_t *core_if)
{
	ifxusb_vbus_on(core_if);
	mdelay(100);
	ifxusb_power_on_h(core_if);
	ifxusb_phy_power_on_h(core_if);
}
#endif
#ifdef __IS_DEVICE__
void do_suspend_d(ifxusb_core_if_t *core_if)
{
	ifxusb_power_off_d(core_if);
}
void do_resume_d(ifxusb_core_if_t *core_if)
{
	dctl_data_t dctl = {.d32=0};

	ifxusb_power_on_d(core_if);
	ifxusb_phy_power_on_d(core_if);
	dctl.d32=ifxusb_rreg(&core_if->dev_global_regs->dctl);
	dctl.b.sftdiscon=1;
	ifxusb_wreg(&core_if->dev_global_regs->dctl,dctl.d32);
	mdelay(50);
	dctl.b.sftdiscon=0;
	ifxusb_wreg(&core_if->dev_global_regs->dctl,dctl.d32);
}
#endif

