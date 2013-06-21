/*****************************************************************************
 **   FILE NAME       : ifxusb_cif_d.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The Core Interface provides basic services for accessing and
 **                     managing the IFX USB hardware. These services are used by the
 **                     Peripheral Controller Driver only.
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
 \file ifxusb_cif_d.c
 \ingroup IFXUSB_DRIVER_V3
 \brief This file contains the interface to the IFX USB Core.
*/

#include <linux/version.h>
#include "ifxusb_version.h"


#include <asm/byteorder.h>
#include <asm/unaligned.h>

#ifdef __DEBUG__
	#include <linux/jiffies.h>
#endif

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"

#include "ifxpcd.h"



/*!
 \brief Initializes the DevSpd field of the DCFG register depending on the PHY type
 and the enumeration speed of the device.
 \param _core_if        Pointer of core_if structure
 */
void ifxusb_dev_init_spd(ifxusb_core_if_t *_core_if)
{
	uint32_t    val;
	dcfg_data_t dcfg;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	if (_core_if->params.speed == IFXUSB_PARAM_SPEED_FULL)
		/* High speed PHY running at full speed */
		val = 0x1;
	else
		/* High speed PHY running at high speed and full speed*/
		val = 0x0;

	IFX_DEBUGPL(DBG_CIL, "Initializing DCFG.DevSpd to 0x%1x\n", val);
	dcfg.d32 = ifxusb_rreg(&_core_if->dev_global_regs->dcfg);
	dcfg.b.devspd = val;
	ifxusb_wreg(&_core_if->dev_global_regs->dcfg, dcfg.d32);
}


/*!
 \brief This function enables the Device mode interrupts.
 \param _core_if        Pointer of core_if structure
 */
void ifxusb_dev_enable_interrupts(ifxusb_core_if_t *_core_if)
{
	gint_data_t intr_mask ={ .d32 = 0};
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	IFX_DEBUGPL(DBG_CIL, "%s()\n", __func__);

	/* Clear any pending OTG Interrupts */
	ifxusb_wreg( &global_regs->gotgint, 0xFFFFFFFF);

	/* Clear any pending interrupts */
	ifxusb_wreg( &global_regs->gintsts, 0xFFFFFFFF);

	/* Enable the interrupts in the GINTMSK.*/
	intr_mask.b.modemismatch = 1;
	intr_mask.b.conidstschng = 1;
	intr_mask.b.wkupintr = 1;
	intr_mask.b.disconnect = 1;
	intr_mask.b.usbsuspend = 1;

	intr_mask.b.usbreset = 1;
	intr_mask.b.enumdone = 1;
	intr_mask.b.inepintr = 1;
	intr_mask.b.outepintr = 1;
	intr_mask.b.erlysuspend = 1;
	#ifndef __DED_FIFO__
		#ifndef __DED_INTR__
			intr_mask.b.epmismatch = 1;
		#endif
	#endif

	ifxusb_mreg( &global_regs->gintmsk, intr_mask.d32, intr_mask.d32);
	IFX_DEBUGPL(DBG_CIL, "%s() gintmsk=%0x\n", __func__, ifxusb_rreg( &global_regs->gintmsk));
}

/*!
 \brief Gets the current USB frame number. This is the frame number from the last SOF packet.
 \param _core_if        Pointer of core_if structure
 */
uint32_t ifxusb_dev_get_frame_number(ifxusb_core_if_t *_core_if)
{
	dsts_data_t dsts;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	dsts.d32 = ifxusb_rreg(&_core_if->dev_global_regs->dsts);
	/* read current frame/microfreme number from DSTS register */
	return dsts.b.soffn;
}


/*!
 \brief  Set the EP STALL.
 */
void ifxusb_dev_ep_set_stall(ifxusb_core_if_t *_core_if, uint8_t _epno, uint8_t _is_in)
{
	depctl_data_t depctl;
	volatile uint32_t *depctl_addr;

	IFX_DEBUGPL(DBG_PCD, "%s ep%d-%s\n", __func__, _epno, (_is_in?"IN":"OUT"));

	depctl_addr = (_is_in)? (&(_core_if->in_ep_regs [_epno]->diepctl)):
	                        (&(_core_if->out_ep_regs[_epno]->doepctl));
	depctl.d32 = ifxusb_rreg(depctl_addr);
	depctl.b.stall = 1;

	if (_is_in && depctl.b.epena)
		depctl.b.epdis = 1;

	ifxusb_wreg(depctl_addr, depctl.d32);
	IFX_DEBUGPL(DBG_PCD,"DEPCTL=%0x\n",ifxusb_rreg(depctl_addr));
	return;
}

/*!
\brief  Clear the EP STALL.
 */
void ifxusb_dev_ep_clear_stall(ifxusb_core_if_t *_core_if, uint8_t _epno, uint8_t _ep_type, uint8_t _is_in)
{
	depctl_data_t depctl;
	volatile uint32_t *depctl_addr;

	IFX_DEBUGPL(DBG_PCD, "%s ep%d-%s\n", __func__, _epno, (_is_in?"IN":"OUT"));

	depctl_addr = (_is_in)? (&(_core_if->in_ep_regs [_epno]->diepctl)):
	                        (&(_core_if->out_ep_regs[_epno]->doepctl));

	depctl.d32 = ifxusb_rreg(depctl_addr);
	/* clear the stall bits */
	depctl.b.stall = 0;

	/*
	 * USB Spec 9.4.5: For endpoints using data toggle, regardless
	 * of whether an endpoint has the Halt feature set, a
	 * ClearFeature(ENDPOINT_HALT) request always results in the
	 * data toggle being reinitialized to DATA0.
	 */
	if (_ep_type == IFXUSB_EP_TYPE_INTR || _ep_type == IFXUSB_EP_TYPE_BULK)
		depctl.b.setd0pid = 1; /* DATA0 */

	ifxusb_wreg(depctl_addr, depctl.d32);
	IFX_DEBUGPL(DBG_PCD,"DEPCTL=%0x\n",ifxusb_rreg(depctl_addr));
	return;
}

/*!
   \brief This function initializes the IFXUSB controller registers for Device mode.
 This function flushes the Tx and Rx FIFOs and it flushes any entries in the
 request queues.
   \param _core_if        Pointer of core_if structure
   \param _params         parameters to be set
 */
void ifxusb_dev_core_init(ifxusb_core_if_t *_core_if, ifxusb_params_t  *_params)
{
	ifxusb_core_global_regs_t *global_regs =  _core_if->core_global_regs;

	gusbcfg_data_t usbcfg   ={.d32 = 0};
	gahbcfg_data_t ahbcfg   ={.d32 = 0};
	dcfg_data_t    dcfg     ={.d32 = 0};
	grstctl_t      resetctl ={.d32 = 0};
	gotgctl_data_t gotgctl  ={.d32 = 0};

	uint32_t dir;
	int i;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	IFX_DEBUGPL(DBG_CILV, "%s(%p)\n",__func__,_core_if);

	/* Copy Params */
	_core_if->params.dma_burst_size      =  _params->dma_burst_size;
	_core_if->params.speed               =  _params->speed;
	if(_params->max_transfer_size < 2048 || _params->max_transfer_size > ((1 << (_core_if->hwcfg3.b.xfer_size_cntr_width + 11)) - 1) )
		_core_if->params.max_transfer_size = ((1 << (_core_if->hwcfg3.b.xfer_size_cntr_width + 11)) - 1);
	else
		_core_if->params.max_transfer_size = _params->max_transfer_size;

	if(_params->max_packet_count < 16 || _params->max_packet_count > ((1 << (_core_if->hwcfg3.b.packet_size_cntr_width + 4)) - 1) )
		_core_if->params.max_packet_count= ((1 << (_core_if->hwcfg3.b.packet_size_cntr_width + 4)) - 1);
	else
		_core_if->params.max_packet_count=  _params->max_packet_count;
	_core_if->params.phy_utmi_width      =  _params->phy_utmi_width;
	_core_if->params.turn_around_time_hs =  _params->turn_around_time_hs;
	_core_if->params.turn_around_time_fs =  _params->turn_around_time_fs;
	_core_if->params.timeout_cal_hs      =  _params->timeout_cal_hs;
	_core_if->params.timeout_cal_fs      =  _params->timeout_cal_fs;

	#ifdef __DED_FIFO__
		_core_if->params.thr_ctl         =  _params->thr_ctl;
		_core_if->params.tx_thr_length   =  _params->tx_thr_length;
		_core_if->params.rx_thr_length   =  _params->rx_thr_length;
	#endif

	/* Reset the Controller */
	do
	{
		while(ifxusb_core_soft_reset_d( _core_if ))
			ifxusb_hard_reset_d(_core_if);
	} while (ifxusb_is_host_mode(_core_if));

	usbcfg.d32 = ifxusb_rreg(&global_regs->gusbcfg);

	usbcfg.b.ForceDevMode = 1;
	usbcfg.b.ForceHstMode = 0;

	usbcfg.b.term_sel_dl_pulse = 0;
	ifxusb_wreg (&global_regs->gusbcfg, usbcfg.d32);

	/* This programming sequence needs to happen in FS mode before any other
	 * programming occurs */
	/* High speed PHY. */
	if (!_core_if->phy_init_done)
	{
		_core_if->phy_init_done = 1;
		/* HS PHY parameters.  These parameters are preserved
		 * during soft reset so only program the first time.  Do
		 * a soft reset immediately after setting phyif.  */
		usbcfg.b.ulpi_utmi_sel = 0; //UTMI+
		usbcfg.b.phyif = ( _core_if->params.phy_utmi_width == 16)?1:0;
		ifxusb_wreg( &global_regs->gusbcfg, usbcfg.d32);
		/* Reset after setting the PHY parameters */
		ifxusb_core_soft_reset_d( _core_if );
	}

	/* Program the GAHBCFG Register.*/
	switch (_core_if->params.dma_burst_size)
	{
		case 0 :
			ahbcfg.b.hburstlen = IFXUSB_GAHBCFG_INT_DMA_BURST_SINGLE;
			break;
		case 1 :
			ahbcfg.b.hburstlen = IFXUSB_GAHBCFG_INT_DMA_BURST_INCR;
			break;
		case 4 :
			ahbcfg.b.hburstlen = IFXUSB_GAHBCFG_INT_DMA_BURST_INCR4;
			break;
		case 8 :
			ahbcfg.b.hburstlen = IFXUSB_GAHBCFG_INT_DMA_BURST_INCR8;
			break;
		case 16:
			ahbcfg.b.hburstlen = IFXUSB_GAHBCFG_INT_DMA_BURST_INCR16;
			break;
	}
	#if defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__)
		_core_if->unaligned_mask=3;
		#if defined(__UNALIGNED_BUF_BURST__)
			switch (_core_if->params.dma_burst_size)
			{
				case 4 :
					_core_if->unaligned_mask=15;
					break;
				case 8 :
					_core_if->unaligned_mask=31;
					break;
				case 16:
					_core_if->unaligned_mask=63;
					break;
				case 0 :
				case 1 :
					break;
			}
		#endif //defined(__UNALIGNED_BUF_BURST__)
	#endif //defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__)
	ahbcfg.b.dmaenable = 1;
	ifxusb_wreg(&global_regs->gahbcfg, ahbcfg.d32);

	/* Program the GUSBCFG register. */
	usbcfg.d32 = ifxusb_rreg( &global_regs->gusbcfg );
	usbcfg.b.hnpcap = 0;
	usbcfg.b.srpcap = 0;
	ifxusb_wreg( &global_regs->gusbcfg, usbcfg.d32);

	{
		dctl_data_t dctl = {.d32=0};
		dctl.d32=ifxusb_rreg(&_core_if->dev_global_regs->dctl);
		dctl.b.sftdiscon=1;
		ifxusb_wreg(&_core_if->dev_global_regs->dctl,dctl.d32);
	}

	/* Restart the Phy Clock */
	ifxusb_wreg(_core_if->pcgcctl, 0);

	/* Device configuration register */
	ifxusb_dev_init_spd(_core_if);
	dcfg.d32 = ifxusb_rreg( &_core_if->dev_global_regs->dcfg);
	dcfg.b.perfrint = IFXUSB_DCFG_FRAME_INTERVAL_80;
	#if defined(__DED_FIFO__)
		#if defined(__DESC_DMA__)
			dcfg.b.descdma = 1;
		#else
			dcfg.b.descdma = 0;
		#endif
	#endif

	ifxusb_wreg( &_core_if->dev_global_regs->dcfg, dcfg.d32 );

	/* Configure data FIFO sizes */
	_core_if->params.data_fifo_size = _core_if->hwcfg3.b.dfifo_depth;
	_core_if->params.rx_fifo_size   = ifxusb_rreg(&global_regs->grxfsiz);
	IFX_DEBUGPL(DBG_CIL, "Initial: FIFO Size=0x%06X\n"   , _core_if->params.data_fifo_size);
	IFX_DEBUGPL(DBG_CIL, "         Rx FIFO Size=0x%06X\n", _core_if->params.rx_fifo_size);

	_core_if->params.tx_fifo_size[0]= ifxusb_rreg(&global_regs->gnptxfsiz) >> 16;

	#ifdef __DED_FIFO__
		for (i=1; i <= _core_if->hwcfg4.b.num_in_eps; i++)
			_core_if->params.tx_fifo_size[i] =
				ifxusb_rreg(&global_regs->dptxfsiz_dieptxf[i-1]) >> 16;
	#else
		for (i=0; i < _core_if->hwcfg4.b.num_dev_perio_in_ep; i++)
			_core_if->params.tx_fifo_size[i+1] =
				ifxusb_rreg(&global_regs->dptxfsiz_dieptxf[i]) >> 16;
	#endif

	#ifdef __DEBUG__
		#ifdef __DED_FIFO__
			for (i=0; i <= _core_if->hwcfg4.b.num_in_eps; i++)
				IFX_DEBUGPL(DBG_CIL, "         Tx[%02d] FIFO Size=0x%06X\n",i, _core_if->params.tx_fifo_size[i]);
		#else
			IFX_DEBUGPL(DBG_CIL, "         NPTx FIFO Size=0x%06X\n", _core_if->params.tx_fifo_size[0]);
			for (i=0; i < _core_if->hwcfg4.b.num_dev_perio_in_ep; i++)
				IFX_DEBUGPL(DBG_CIL, "         PTx[%02d] FIFO Size=0x%06X\n",i, _core_if->params.tx_fifo_size[i+1]);
		#endif
	#endif

	{
		fifosize_data_t txfifosize;
		if(_params->data_fifo_size >=0 && _params->data_fifo_size < _core_if->params.data_fifo_size)
			_core_if->params.data_fifo_size = _params->data_fifo_size;


		if(_params->rx_fifo_size >=0 && _params->rx_fifo_size < _core_if->params.rx_fifo_size)
			_core_if->params.rx_fifo_size = _params->rx_fifo_size;
		if(_core_if->params.data_fifo_size < _core_if->params.rx_fifo_size)
			_core_if->params.rx_fifo_size = _core_if->params.data_fifo_size;
		ifxusb_wreg( &global_regs->grxfsiz, _core_if->params.rx_fifo_size);

		for (i=0; i < MAX_EPS_CHANNELS; i++)
			if(_params->tx_fifo_size[i] >=0 && _params->tx_fifo_size[i] < _core_if->params.tx_fifo_size[i])
				_core_if->params.tx_fifo_size[i] = _params->tx_fifo_size[i];

		txfifosize.b.startaddr = _core_if->params.rx_fifo_size;
		#ifdef __DED_FIFO__
			if(txfifosize.b.startaddr + _core_if->params.tx_fifo_size[0] > _core_if->params.data_fifo_size)
				_core_if->params.tx_fifo_size[0]= _core_if->params.data_fifo_size - txfifosize.b.startaddr;
			txfifosize.b.depth=_core_if->params.tx_fifo_size[0];
			ifxusb_wreg( &global_regs->gnptxfsiz, txfifosize.d32);
			txfifosize.b.startaddr += _core_if->params.tx_fifo_size[0];
			for (i=1; i <= _core_if->hwcfg4.b.num_in_eps; i++)
			{
				if(txfifosize.b.startaddr + _core_if->params.tx_fifo_size[i] > _core_if->params.data_fifo_size)
					_core_if->params.tx_fifo_size[i]= _core_if->params.data_fifo_size - txfifosize.b.startaddr;
				txfifosize.b.depth=_core_if->params.tx_fifo_size[i];
				ifxusb_wreg( &global_regs->dptxfsiz_dieptxf[i-1], txfifosize.d32);
				txfifosize.b.startaddr += _core_if->params.tx_fifo_size[i];
			}
		#else
			if(txfifosize.b.startaddr + _core_if->params.tx_fifo_size[0] > _core_if->params.data_fifo_size)
				_core_if->params.tx_fifo_size[0]= _core_if->params.data_fifo_size - txfifosize.b.startaddr;
			txfifosize.b.depth=_core_if->params.tx_fifo_size[0];
			ifxusb_wreg( &global_regs->gnptxfsiz, txfifosize.d32);
			txfifosize.b.startaddr += _core_if->params.tx_fifo_size[0];
			for (i=0; i < _core_if->hwcfg4.b.num_dev_perio_in_ep; i++)
			{
				if(txfifosize.b.startaddr + _core_if->params.tx_fifo_size[i+1] > _core_if->params.data_fifo_size)
					_core_if->params.tx_fifo_size[i+1]= _core_if->params.data_fifo_size - txfifosize.b.startaddr;
				//txfifosize.b.depth=_core_if->params.tx_fifo_size[i+1];
				ifxusb_wreg( &global_regs->dptxfsiz_dieptxf[i], txfifosize.d32);
				txfifosize.b.startaddr += _core_if->params.tx_fifo_size[i+1];
			}
		#endif
	}

	#ifdef __DEBUG__
	{
		fifosize_data_t fifosize;
		IFX_DEBUGPL(DBG_CIL, "Result : FIFO Size=0x%06X\n"   , _core_if->params.data_fifo_size);

		IFX_DEBUGPL(DBG_CIL, "         Rx FIFO =0x%06X Sz=0x%06X\n", 0,ifxusb_rreg(&global_regs->grxfsiz));
		#ifdef __DED_FIFO__
			fifosize.d32=ifxusb_rreg(&global_regs->gnptxfsiz);
			IFX_DEBUGPL(DBG_CIL, "         Tx[00] FIFO =0x%06X Sz=0x%06X\n", fifosize.b.startaddr,fifosize.b.depth);
			for (i=1; i <= _core_if->hwcfg4.b.num_in_eps; i++)
			{
				fifosize.d32=ifxusb_rreg(&global_regs->dptxfsiz_dieptxf[i-1]);
				IFX_DEBUGPL(DBG_CIL, "         Tx[%02d] FIFO 0x%06X Sz=0x%06X\n",i, fifosize.b.startaddr,fifosize.b.depth);
			}
		#else
			fifosize.d32=ifxusb_rreg(&global_regs->gnptxfsiz);
			IFX_DEBUGPL(DBG_CIL, "         NPTx FIFO =0x%06X Sz=0x%06X\n", fifosize.b.startaddr,fifosize.b.depth);
			for (i=0; i < _core_if->hwcfg4.b.num_dev_perio_in_ep; i++)
			{
				fifosize.d32=ifxusb_rreg(&global_regs->dptxfsiz_dieptxf[i]);
				IFX_DEBUGPL(DBG_CIL, "         PTx[%02d] FIFO 0x%06X Sz=0x%06X\n",i, fifosize.b.startaddr,fifosize.b.depth);
			}
		#endif
	}
	#endif

	/* Clear Host Set HNP Enable in the OTG Control Register */
	gotgctl.b.hstsethnpen = 1;
	ifxusb_mreg( &global_regs->gotgctl, gotgctl.d32, 0);

	/* Flush the FIFOs */
	ifxusb_flush_tx_fifo_d(_core_if, 0x10);  /* all Tx FIFOs */
	ifxusb_flush_rx_fifo_d(_core_if);

	/* Flush the Learning Queue. */
	resetctl.b.intknqflsh = 1;
	ifxusb_wreg( &global_regs->grstctl, resetctl.d32);

	/* Clear all pending Device Interrupts */
	ifxusb_wreg( &_core_if->dev_global_regs->diepmsk , 0 );
	ifxusb_wreg( &_core_if->dev_global_regs->doepmsk , 0 );
	ifxusb_wreg( &_core_if->dev_global_regs->daint   , 0xFFFFFFFF );
	ifxusb_wreg( &_core_if->dev_global_regs->daintmsk, 0 );

	dir=_core_if->hwcfg1.d32;
	for (i=0; i <= _core_if->hwcfg2.b.num_dev_ep ; i++,dir>>=2)
	{
		depctl_data_t depctl;
		if((dir&0x03)==0 || (dir&0x03) ==1)
		{
			depctl.d32 = ifxusb_rreg(&_core_if->in_ep_regs[i]->diepctl);
			if (depctl.b.epena)
			{
				depctl.d32 = 0;
				depctl.b.epdis = 1;
				depctl.b.snak = 1;
			}
			else
				depctl.d32 = 0;
			ifxusb_wreg( &_core_if->in_ep_regs[i]->diepctl, depctl.d32);
			#ifndef __DESC_DMA__
				ifxusb_wreg( &_core_if->in_ep_regs[i]->dieptsiz, 0);
			#endif
			ifxusb_wreg( &_core_if->in_ep_regs[i]->diepdma, 0);
			ifxusb_wreg( &_core_if->in_ep_regs[i]->diepint, 0xFF);
		}

		if((dir&0x03)==0 || (dir&0x03) ==2)
		{
			depctl.d32 = ifxusb_rreg(&_core_if->out_ep_regs[i]->doepctl);
			if (depctl.b.epena)
			{
				depctl.d32 = 0;
				depctl.b.epdis = 1;
				depctl.b.snak = 1;
			}
			else
				depctl.d32 = 0;
			ifxusb_wreg( &_core_if->out_ep_regs[i]->doepctl, depctl.d32);
			#ifndef __DESC_DMA__
				ifxusb_wreg( &_core_if->out_ep_regs[i]->doeptsiz, 0);
			#endif
			ifxusb_wreg( &_core_if->out_ep_regs[i]->doepdma, 0);
			ifxusb_wreg( &_core_if->out_ep_regs[i]->doepint, 0xFF);
		}
	}
}

