/*****************************************************************************
 **   FILE NAME       : ifxusb_cif_h.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The Core Interface provides basic services for accessing and
 **                     managing the IFX USB hardware. These services are used by the
 **                     Host Controller Driver only.
 *****************************************************************************/

/*!
 \file ifxusb_cif_h.c
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
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#if defined(__UEIP__)
//	#include <asm/ifx/ifx_board.h>
#endif

//#include <asm/ifx/ifx_gpio.h>
#if defined(__UEIP__)
//	#include <asm/ifx/ifx_led.h>
#endif

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"

#include "ifxhcd.h"

#if !defined(__UEIP__)
	#undef __USING_LED_AS_GPIO__
#endif


/*!
 \brief This function enables the Host mode interrupts.
 \param _core_if        Pointer of core_if structure
 */
void ifxusb_host_enable_interrupts(ifxusb_core_if_t *_core_if)
{
	gint_data_t intr_mask ={ .d32 = 0};
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;

	IFX_DEBUGPL(DBG_CIL, "%s()\n", __func__);

	/* Clear any pending OTG Interrupts */
	ifxusb_wreg( &global_regs->gotgint, 0xFFFFFFFF);

	/* Clear any pending interrupts */
	ifxusb_wreg( &global_regs->gintsts, 0xFFFFFFFF);

	/* Enable the interrupts in the GINTMSK.*/

	/* Common interrupts */
	intr_mask.b.modemismatch = 1;
	intr_mask.b.conidstschng = 1;
	intr_mask.b.wkupintr = 1;
	intr_mask.b.disconnect = 1;
	intr_mask.b.usbsuspend = 1;

	/* Host interrupts */
	intr_mask.b.sofintr = 1;
	intr_mask.b.portintr = 1;
	intr_mask.b.hcintr = 1;

	ifxusb_mreg( &global_regs->gintmsk, intr_mask.d32, intr_mask.d32);
	IFX_DEBUGPL(DBG_CIL, "%s() gintmsk=%0x\n", __func__, ifxusb_rreg( &global_regs->gintmsk));
}

/*!
 \brief This function disables the Host mode interrupts.
 \param _core_if        Pointer of core_if structure
 */
void ifxusb_host_disable_interrupts(ifxusb_core_if_t *_core_if)
{
	ifxusb_core_global_regs_t *global_regs = _core_if->core_global_regs;

	IFX_DEBUGPL(DBG_CILV, "%s()\n", __func__);

	#if 1
		ifxusb_wreg( &global_regs->gintmsk, 0);
	#else
		/* Common interrupts */
		{
			gint_data_t intr_mask ={.d32 = 0};
			intr_mask.b.modemismatch = 1;
			intr_mask.b.rxstsqlvl = 1;
			intr_mask.b.conidstschng = 1;
			intr_mask.b.wkupintr = 1;
			intr_mask.b.disconnect = 1;
			intr_mask.b.usbsuspend = 1;

			/* Host interrupts */
			intr_mask.b.sofintr = 1;
			intr_mask.b.portintr = 1;
			intr_mask.b.hcintr = 1;
			intr_mask.b.ptxfempty = 1;
			intr_mask.b.nptxfempty = 1;
			ifxusb_mreg(&global_regs->gintmsk, intr_mask.d32, 0);
		}
	#endif
}

/*!
 \brief This function initializes the IFXUSB controller registers for  Host mode.
        This function flushes the Tx and Rx FIFOs and it flushes any entries in the
        request queues.
 \param _core_if        Pointer of core_if structure
 \param _params         parameters to be set
 */
void ifxusb_host_core_init(ifxusb_core_if_t *_core_if, ifxusb_params_t  *_params)
{
	ifxusb_core_global_regs_t *global_regs =  _core_if->core_global_regs;

	gusbcfg_data_t usbcfg   ={.d32 = 0};
	gahbcfg_data_t ahbcfg   ={.d32 = 0};
	gotgctl_data_t gotgctl  ={.d32 = 0};

	int i;

	IFX_DEBUGPL(DBG_CILV, "%s(%p)\n",__func__,_core_if);

	/* Copy Params */

	_core_if->params.dma_burst_size      =  _params->dma_burst_size;
	_core_if->params.speed               =  _params->speed;
	_core_if->params.max_transfer_size   =  _params->max_transfer_size;
	_core_if->params.max_packet_count    =  _params->max_packet_count;
	_core_if->params.phy_utmi_width      =  _params->phy_utmi_width;
	_core_if->params.turn_around_time_hs =  _params->turn_around_time_hs;
	_core_if->params.turn_around_time_fs =  _params->turn_around_time_fs;
	_core_if->params.timeout_cal_hs      =  _params->timeout_cal_hs;
	_core_if->params.timeout_cal_fs      =  _params->timeout_cal_fs;

	/* Reset the Controller */
	do
	{
		while(ifxusb_core_soft_reset( _core_if ))
			ifxusb_hard_reset(_core_if);
	} while (ifxusb_is_device_mode(_core_if));

	usbcfg.d32 = ifxusb_rreg(&global_regs->gusbcfg);
//	usbcfg.b.ulpi_ext_vbus_drv = 1;
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
		ifxusb_core_soft_reset( _core_if );
	}

	usbcfg.d32 = ifxusb_rreg(&global_regs->gusbcfg);
//	usbcfg.b.ulpi_fsls = 0;
//	usbcfg.b.ulpi_clk_sus_m = 0;
	ifxusb_wreg(&global_regs->gusbcfg, usbcfg.d32);

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
	ahbcfg.b.dmaenable = 1;
	ifxusb_wreg(&global_regs->gahbcfg, ahbcfg.d32);

	/* Program the GUSBCFG register. */
	usbcfg.d32 = ifxusb_rreg( &global_regs->gusbcfg );
	usbcfg.b.hnpcap = 0;
	usbcfg.b.srpcap = 0;
	ifxusb_wreg( &global_regs->gusbcfg, usbcfg.d32);

	/* Restart the Phy Clock */
	ifxusb_wreg(_core_if->pcgcctl, 0);

	/* Initialize Host Configuration Register */
	{
		hcfg_data_t 	hcfg;
		hcfg.d32 = ifxusb_rreg(&_core_if->host_global_regs->hcfg);
		hcfg.b.fslspclksel = IFXUSB_HCFG_30_60_MHZ;
		if (_params->speed == IFXUSB_PARAM_SPEED_FULL)
			hcfg.b.fslssupp = 1;
		ifxusb_wreg(&_core_if->host_global_regs->hcfg, hcfg.d32);
	}

	_core_if->params.host_channels=(_core_if->hwcfg2.b.num_host_chan + 1);

	if(_params->host_channels>0 && _params->host_channels < _core_if->params.host_channels)
		_core_if->params.host_channels = _params->host_channels;

	/* Configure data FIFO sizes */
	_core_if->params.data_fifo_size     = _core_if->hwcfg3.b.dfifo_depth;
	_core_if->params.rx_fifo_size       = ifxusb_rreg(&global_regs->grxfsiz);
	_core_if->params.nperio_tx_fifo_size= ifxusb_rreg(&global_regs->gnptxfsiz) >> 16;
	_core_if->params.perio_tx_fifo_size = ifxusb_rreg(&global_regs->hptxfsiz) >> 16;
	IFX_DEBUGPL(DBG_CIL, "Initial: FIFO Size=0x%06X\n"   , _core_if->params.data_fifo_size);
	IFX_DEBUGPL(DBG_CIL, "           Rx FIFO Size=0x%06X\n", _core_if->params.rx_fifo_size);
	IFX_DEBUGPL(DBG_CIL, "         NPTx FIFO Size=0x%06X\n", _core_if->params.nperio_tx_fifo_size);
	IFX_DEBUGPL(DBG_CIL, "          PTx FIFO Size=0x%06X\n", _core_if->params.perio_tx_fifo_size);

	{
		fifosize_data_t txfifosize;
		if(_params->data_fifo_size >=0 && _params->data_fifo_size < _core_if->params.data_fifo_size)
			_core_if->params.data_fifo_size = _params->data_fifo_size;

		if( _params->rx_fifo_size >= 0 && _params->rx_fifo_size < _core_if->params.rx_fifo_size)
			_core_if->params.rx_fifo_size = _params->rx_fifo_size;
		if( _params->nperio_tx_fifo_size >=0 && _params->nperio_tx_fifo_size < _core_if->params.nperio_tx_fifo_size)
			_core_if->params.nperio_tx_fifo_size = _params->nperio_tx_fifo_size;
		if( _params->perio_tx_fifo_size >=0 && _params->perio_tx_fifo_size < _core_if->params.perio_tx_fifo_size)
			_core_if->params.perio_tx_fifo_size = _params->perio_tx_fifo_size;

		if(_core_if->params.data_fifo_size < _core_if->params.rx_fifo_size)
			_core_if->params.rx_fifo_size = _core_if->params.data_fifo_size;
		ifxusb_wreg( &global_regs->grxfsiz, _core_if->params.rx_fifo_size);
		txfifosize.b.startaddr = _core_if->params.rx_fifo_size;

		if(txfifosize.b.startaddr + _core_if->params.nperio_tx_fifo_size > _core_if->params.data_fifo_size)
			_core_if->params.nperio_tx_fifo_size = _core_if->params.data_fifo_size - txfifosize.b.startaddr;
		txfifosize.b.depth=_core_if->params.nperio_tx_fifo_size;
		ifxusb_wreg( &global_regs->gnptxfsiz, txfifosize.d32);
		txfifosize.b.startaddr += _core_if->params.nperio_tx_fifo_size;

		if(txfifosize.b.startaddr + _core_if->params.perio_tx_fifo_size > _core_if->params.data_fifo_size)
			_core_if->params.perio_tx_fifo_size = _core_if->params.data_fifo_size - txfifosize.b.startaddr;
		txfifosize.b.depth=_core_if->params.perio_tx_fifo_size;
		ifxusb_wreg( &global_regs->hptxfsiz, txfifosize.d32);
		txfifosize.b.startaddr += _core_if->params.perio_tx_fifo_size;
	}

	#ifdef __DEBUG__
	{
		fifosize_data_t fifosize;
		IFX_DEBUGPL(DBG_CIL, "Result : FIFO Size=0x%06X\n"   , _core_if->params.data_fifo_size);

		fifosize.d32=ifxusb_rreg(&global_regs->grxfsiz);
		IFX_DEBUGPL(DBG_CIL, "         Rx FIFO =0x%06X 0x%06X\n", fifosize.b.startaddr,fifosize.b.depth);
		fifosize.d32=ifxusb_rreg(&global_regs->gnptxfsiz);
		IFX_DEBUGPL(DBG_CIL, "         NPTx FIFO =0x%06X 0x%06X\n", fifosize.b.startaddr,fifosize.b.depth);
		fifosize.d32=ifxusb_rreg(&global_regs->hptxfsiz);
		IFX_DEBUGPL(DBG_CIL, "          PTx FIFO =0x%06X 0x%06X\n", fifosize.b.startaddr,fifosize.b.depth);
	}
	#endif

	/* Clear Host Set HNP Enable in the OTG Control Register */
	gotgctl.b.hstsethnpen = 1;
	ifxusb_mreg( &global_regs->gotgctl, gotgctl.d32, 0);

	/* Flush the FIFOs */
	ifxusb_flush_tx_fifo(_core_if, 0x10);  /* all Tx FIFOs */
	ifxusb_flush_rx_fifo(_core_if);

	for (i = 0; i < _core_if->hwcfg2.b.num_host_chan + 1; i++)
	{
		hcchar_data_t    hcchar;
		hcchar.d32 = ifxusb_rreg(&_core_if->hc_regs[i]->hcchar);
		hcchar.b.chen  = 0;
		hcchar.b.chdis = 1;
		hcchar.b.epdir = 0;
		ifxusb_wreg(&_core_if->hc_regs[i]->hcchar, hcchar.d32);
	}
	/* Halt all channels to put them into a known state. */
	for (i = 0; i < _core_if->hwcfg2.b.num_host_chan + 1; i++)
	{
		hcchar_data_t    hcchar;
		int count = 0;

		hcchar.d32 = ifxusb_rreg(&_core_if->hc_regs[i]->hcchar);
		hcchar.b.chen  = 1;
		hcchar.b.chdis = 1;
		hcchar.b.epdir = 0;
		ifxusb_wreg(&_core_if->hc_regs[i]->hcchar, hcchar.d32);

		IFX_DEBUGPL(DBG_HCDV, "%s: Halt channel %d\n", __func__, i);
		do{
			hcchar.d32 = ifxusb_rreg(&_core_if->hc_regs[i]->hcchar);
			if (++count > 1000)
			{
				IFX_ERROR("%s: Unable to clear halt on channel %d\n", __func__, i);
				break;
			}
		} while (hcchar.b.chen);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__UEIP__)
	#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
		int ifxusb_vbus_status =-1;
	#endif

	#if defined(IFX_GPIO_USB_VBUS1) || defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
		int ifxusb_vbus1_status =-1;
	#endif

	#if defined(IFX_GPIO_USB_VBUS2) || defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
		int ifxusb_vbus2_status =-1;
	#endif

	#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
		static void *g_usb_vbus_trigger  = NULL;
	#endif
	#if defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
		static void *g_usb_vbus1_trigger = NULL;
	#endif
	#if defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
		static void *g_usb_vbus2_trigger = NULL;
	#endif

	#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
		int ifxusb_vbus_gpio_inited=0;
	#endif

#else //defined(__UEIP__)
	int ifxusb_vbus_gpio_inited=0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ifxusb_vbus_init(ifxusb_core_if_t *_core_if)
{
	#if defined(__UEIP__)
		#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
			if ( !g_usb_vbus_trigger )
			{
				ifx_led_trigger_register("USB_VBUS", &g_usb_vbus_trigger);
				if ( g_usb_vbus_trigger != NULL )
				{
					struct ifx_led_trigger_attrib attrib = {0};
					attrib.delay_on     = 0;
					attrib.delay_off    = 0;
					attrib.timeout      = 0;
					attrib.def_value    = 0;
					attrib.flags        = IFX_LED_TRIGGER_ATTRIB_DELAY_ON | IFX_LED_TRIGGER_ATTRIB_DELAY_OFF | IFX_LED_TRIGGER_ATTRIB_TIMEOUT | IFX_LED_TRIGGER_ATTRIB_DEF_VALUE;
					IFX_DEBUGP("Reg USB power!!\n");
					ifx_led_trigger_set_attrib(g_usb_vbus_trigger, &attrib);
					ifxusb_vbus_status =0;
				}
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
			if(_core_if->core_no==0 && !g_usb_vbus1_trigger )
			{
				ifx_led_trigger_register("USB_VBUS1", &g_usb_vbus1_trigger);
				if ( g_usb_vbus1_trigger != NULL )
				{
					struct ifx_led_trigger_attrib attrib = {0};
					attrib.delay_on     = 0;
					attrib.delay_off    = 0;
					attrib.timeout      = 0;
					attrib.def_value    = 0;
					attrib.flags        = IFX_LED_TRIGGER_ATTRIB_DELAY_ON | IFX_LED_TRIGGER_ATTRIB_DELAY_OFF | IFX_LED_TRIGGER_ATTRIB_TIMEOUT | IFX_LED_TRIGGER_ATTRIB_DEF_VALUE;
					IFX_DEBUGP("Reg USB1 power!!\n");
					ifx_led_trigger_set_attrib(g_usb_vbus1_trigger, &attrib);
					ifxusb_vbus1_status =0;
				}
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
			if(_core_if->core_no==1 && !g_usb_vbus2_trigger )
			{
				ifx_led_trigger_register("USB_VBUS2", &g_usb_vbus2_trigger);
				if ( g_usb_vbus2_trigger != NULL )
				{
					struct ifx_led_trigger_attrib attrib = {0};
					attrib.delay_on     = 0;
					attrib.delay_off    = 0;
					attrib.timeout      = 0;
					attrib.def_value    = 0;
					attrib.flags        = IFX_LED_TRIGGER_ATTRIB_DELAY_ON | IFX_LED_TRIGGER_ATTRIB_DELAY_OFF | IFX_LED_TRIGGER_ATTRIB_TIMEOUT | IFX_LED_TRIGGER_ATTRIB_DEF_VALUE;
					IFX_DEBUGP("Reg USB2 power!!\n");
					ifx_led_trigger_set_attrib(g_usb_vbus2_trigger, &attrib);
					ifxusb_vbus2_status =0;
				}
			}
		#endif

		#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
			/* == 20100712 AVM/WK use gpio_inited as bitmask == */
			if(ifxusb_vbus_gpio_inited == 0)
			{
				if(!ifx_gpio_register(IFX_GPIO_MODULE_USB))
				{
					IFX_DEBUGP("Register USB VBus through GPIO OK!!\n");
					#ifdef IFX_GPIO_USB_VBUS
						ifxusb_vbus_status =0;
					#endif //IFX_GPIO_USB_VBUS
					#ifdef IFX_GPIO_USB_VBUS1
						ifxusb_vbus1_status=0;
					#endif //IFX_GPIO_USB_VBUS1
					#ifdef IFX_GPIO_USB_VBUS2
						ifxusb_vbus2_status=0;
					#endif //IFX_GPIO_USB_VBUS2
					ifxusb_vbus_gpio_inited|= (1<<_core_if->core_no);
				}
				else
					IFX_PRINT("Register USB VBus Failed!!\n");
			} else {
				ifxusb_vbus_gpio_inited|= (1<<_core_if->core_no);
			}
		#endif //defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
	#endif //defined(__UEIP__)
}

void ifxusb_vbus_free(ifxusb_core_if_t *_core_if)
{
	#if defined(__UEIP__)
		#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
			if ( g_usb_vbus_trigger )
			{
			    ifx_led_trigger_deregister(g_usb_vbus_trigger);
			    g_usb_vbus_trigger = NULL;
			    ifxusb_vbus_status =-1;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
			if(_core_if->core_no==0 && g_usb_vbus1_trigger )
			{
			    ifx_led_trigger_deregister(g_usb_vbus1_trigger);
			    g_usb_vbus1_trigger = NULL;
			    ifxusb_vbus1_status =-1;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
			if(_core_if->core_no==1 && g_usb_vbus2_trigger )
			{
			    ifx_led_trigger_deregister(g_usb_vbus2_trigger);
			    g_usb_vbus2_trigger = NULL;
			    ifxusb_vbus2_status =-1;
			}
		#endif

		#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
			/* == 20100712 AVM/WK use gpio_inited as bitmask == */
			if((ifxusb_vbus_gpio_inited & (1<<_core_if->core_no)) == ifxusb_vbus_gpio_inited)
			{
				ifx_gpio_deregister(IFX_GPIO_MODULE_USB);
				#ifdef IFX_GPIO_USB_VBUS
					ifxusb_vbus_status =-1;
				#endif //IFX_GPIO_USB_VBUS
				#ifdef IFX_GPIO_USB_VBUS1
					ifxusb_vbus1_status=-1;
				#endif //IFX_GPIO_USB_VBUS1
				#ifdef IFX_GPIO_USB_VBUS2
					ifxusb_vbus2_status=-1;
				#endif //IFX_GPIO_USB_VBUS2
			}
			ifxusb_vbus_gpio_inited &= ~(1<<_core_if->core_no);
		#endif //defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
	#endif //defined(__UEIP__)
}


/*!
   \brief Turn on the USB 5V VBus Power
   \param _core_if        Pointer of core_if structure
 */
void ifxusb_vbus_on(ifxusb_core_if_t *_core_if)
{
	IFX_DEBUGP("SENDING VBus POWER UP\n");
	#if defined(__UEIP__)
		#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
			if ( g_usb_vbus_trigger && ifxusb_vbus_status==0)
			{
				ifx_led_trigger_activate(g_usb_vbus_trigger);
				IFX_DEBUGP("Enable USB power!!\n");
				ifxusb_vbus_status=1;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
			if(_core_if->core_no==0 && g_usb_vbus1_trigger && ifxusb_vbus1_status==0)
			{
				ifx_led_trigger_activate(g_usb_vbus1_trigger);
				IFX_DEBUGP("Enable USB1 power!!\n");
				ifxusb_vbus1_status=1;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
			if(_core_if->core_no==1 && g_usb_vbus2_trigger && ifxusb_vbus2_status==0)
			{
				ifx_led_trigger_activate(g_usb_vbus2_trigger);
				IFX_DEBUGP("Enable USB2 power!!\n");
				ifxusb_vbus2_status=1;
			}
		#endif

		#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
			if(ifxusb_vbus_gpio_inited)
			{
				#if defined(IFX_GPIO_USB_VBUS)
					if(ifxusb_vbus_status==0)
					{
						ifx_gpio_output_set(IFX_GPIO_USB_VBUS,IFX_GPIO_MODULE_USB);
						ifxusb_vbus_status=1;
					}
				#endif
				#if defined(IFX_GPIO_USB_VBUS1)
					if(_core_if->core_no==0 && ifxusb_vbus1_status==0)
					{
						ifx_gpio_output_set(IFX_GPIO_USB_VBUS1,IFX_GPIO_MODULE_USB);
						ifxusb_vbus1_status=1;
					}
				#endif
				#if defined(IFX_GPIO_USB_VBUS2)
					if(_core_if->core_no==1 && ifxusb_vbus2_status==0)
					{
						ifx_gpio_output_set(IFX_GPIO_USB_VBUS2,IFX_GPIO_MODULE_USB);
						ifxusb_vbus2_status=1;
					}
				#endif
			}
		#endif //defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
	#else
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			ifxusb_vbus_status=1;
			//usb_set_vbus_on();
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			set_bit (4, (volatile unsigned long *)AMAZON_SE_GPIO_P0_OUT);
			ifxusb_vbus_status=1;
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				if (bsp_port_reserve_pin(1, 13, PORT_MODULE_USB) != 0)
				{
					IFX_PRINT("Can't enable USB1 5.5V power!!\n");
					return;
				}
				bsp_port_clear_altsel0(1, 13, PORT_MODULE_USB);
				bsp_port_clear_altsel1(1, 13, PORT_MODULE_USB);
				bsp_port_set_dir_out(1, 13, PORT_MODULE_USB);
				bsp_port_set_pudsel(1, 13, PORT_MODULE_USB);
				bsp_port_set_puden(1, 13, PORT_MODULE_USB);
				bsp_port_set_output(1, 13, PORT_MODULE_USB);
				IFX_DEBUGP("Enable USB1 power!!\n");
				ifxusb_vbus1_status=1;
			}
			else
			{
				if (bsp_port_reserve_pin(3, 4, PORT_MODULE_USB) != 0)
				{
					IFX_PRINT("Can't enable USB2 5.5V power!!\n");
					return;
				}
				bsp_port_clear_altsel0(3, 4, PORT_MODULE_USB);
				bsp_port_clear_altsel1(3, 4, PORT_MODULE_USB);
				bsp_port_set_dir_out(3, 4, PORT_MODULE_USB);
				bsp_port_set_pudsel(3, 4, PORT_MODULE_USB);
				bsp_port_set_puden(3, 4, PORT_MODULE_USB);
				bsp_port_set_output(3, 4, PORT_MODULE_USB);
				IFX_DEBUGP("Enable USB2 power!!\n");
				ifxusb_vbus2_status=1;
			}
		#endif //defined(__IS_AR9__)
		#if defined(__IS_VR9__)
			if(_core_if->core_no==0)
			{
				ifxusb_vbus1_status=1;
			}
			else
			{
				ifxusb_vbus2_status=1;
			}
		#endif //defined(__IS_VR9__)
	#endif //defined(__UEIP__)
}


/*!
   \brief Turn off the USB 5V VBus Power
   \param _core_if        Pointer of core_if structure
 */
void ifxusb_vbus_off(ifxusb_core_if_t *_core_if)
{
	IFX_DEBUGP("SENDING VBus POWER OFF\n");

	#if defined(__UEIP__)
		#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
			if ( g_usb_vbus_trigger && ifxusb_vbus_status==1)
			{
				ifx_led_trigger_deactivate(g_usb_vbus_trigger);
				IFX_DEBUGP("Disable USB power!!\n");
				ifxusb_vbus_status=0;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
			if(_core_if->core_no==0 && g_usb_vbus1_trigger && ifxusb_vbus1_status==1)
			{
				ifx_led_trigger_deactivate(g_usb_vbus1_trigger);
				IFX_DEBUGP("Disable USB1 power!!\n");
				ifxusb_vbus1_status=0;
			}
		#endif
		#if defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
			if(_core_if->core_no==1 && g_usb_vbus2_trigger && ifxusb_vbus2_status==1)
			{
				ifx_led_trigger_deactivate(g_usb_vbus2_trigger);
				IFX_DEBUGP("Disable USB2 power!!\n");
				ifxusb_vbus2_status=0;
			}
		#endif

		#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
			if(ifxusb_vbus_gpio_inited)
			{
				#if defined(IFX_GPIO_USB_VBUS)
					if(ifxusb_vbus_status==1)
					{
						ifx_gpio_output_clear(IFX_GPIO_USB_VBUS,IFX_GPIO_MODULE_USB);
						ifxusb_vbus_status=0;
					}
				#endif
				#if defined(IFX_GPIO_USB_VBUS1)
					if(_core_if->core_no==0 && ifxusb_vbus1_status==1)
					{
						ifx_gpio_output_clear(IFX_GPIO_USB_VBUS1,IFX_GPIO_MODULE_USB);
						ifxusb_vbus1_status=0;
					}
				#endif
				#if defined(IFX_GPIO_USB_VBUS2)
					if(_core_if->core_no==1 && ifxusb_vbus2_status==1)
					{
						ifx_gpio_output_clear(IFX_GPIO_USB_VBUS2,IFX_GPIO_MODULE_USB);
						ifxusb_vbus2_status=0;
					}
				#endif
			}
		#endif //defined(IFX_GPIO_USB_VBUS) || defined(IFX_GPIO_USB_VBUS1) || defined(IFX_GPIO_USB_VBUS2)
	#else
		#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
			ifxusb_vbus_status=0;
			//usb_set_vbus_on();
		#endif //defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#if defined(__IS_AMAZON_SE__)
			clear_bit (4, (volatile unsigned long *)AMAZON_SE_GPIO_P0_OUT);
			ifxusb_vbus_status=0;
		#endif //defined(__IS_AMAZON_SE__)
		#if defined(__IS_AR9__)
			if(_core_if->core_no==0)
			{
				if (bsp_port_reserve_pin(1, 13, PORT_MODULE_USB) != 0) {
					IFX_PRINT("Can't Disable USB1 5.5V power!!\n");
					return;
				}
				bsp_port_clear_altsel0(1, 13, PORT_MODULE_USB);
				bsp_port_clear_altsel1(1, 13, PORT_MODULE_USB);
				bsp_port_set_dir_out(1, 13, PORT_MODULE_USB);
				bsp_port_set_pudsel(1, 13, PORT_MODULE_USB);
				bsp_port_set_puden(1, 13, PORT_MODULE_USB);
				bsp_port_clear_output(1, 13, PORT_MODULE_USB);
				IFX_DEBUGP("Disable USB1 power!!\n");
				ifxusb_vbus1_status=0;
			}
			else
			{
				if (bsp_port_reserve_pin(3, 4, PORT_MODULE_USB) != 0) {
					IFX_PRINT("Can't Disable USB2 5.5V power!!\n");
					return;
				}
				bsp_port_clear_altsel0(3, 4, PORT_MODULE_USB);
				bsp_port_clear_altsel1(3, 4, PORT_MODULE_USB);
				bsp_port_set_dir_out(3, 4, PORT_MODULE_USB);
				bsp_port_set_pudsel(3, 4, PORT_MODULE_USB);
				bsp_port_set_puden(3, 4, PORT_MODULE_USB);
				bsp_port_clear_output(3, 4, PORT_MODULE_USB);
				IFX_DEBUGP("Disable USB2 power!!\n");

				ifxusb_vbus2_status=0;
			}
		#endif //defined(__IS_AR9__)
		#if defined(__IS_VR9__)
			if(_core_if->core_no==0)
			{
				ifxusb_vbus1_status=0;
			}
			else
			{
				ifxusb_vbus2_status=0;
			}
		#endif //defined(__IS_VR9__)
	#endif //defined(__UEIP__)
}



/*!
   \brief Read Current VBus status
   \param _core_if        Pointer of core_if structure
 */
int ifxusb_vbus(ifxusb_core_if_t *_core_if)
{
#if defined(__UEIP__)
	#if defined(IFX_GPIO_USB_VBUS) || defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDLED_USB_VBUS)
		return (ifxusb_vbus_status);
	#endif

	#if defined(IFX_GPIO_USB_VBUS1) || defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDLED_USB_VBUS1)
		if(_core_if->core_no==0)
			return (ifxusb_vbus1_status);
	#endif

	#if defined(IFX_GPIO_USB_VBUS2) || defined(IFX_LEDGPIO_USB_VBUS2) || defined(IFX_LEDLED_USB_VBUS2)
		if(_core_if->core_no==1)
			return (ifxusb_vbus2_status);
	#endif
#else //defined(__UEIP__)
#endif
	return -1;
}

#if defined(__UEIP__)
#else
	#if defined(__IS_TWINPASS__)
		#define ADSL_BASE 0x20000
		#define CRI_BASE          0x31F00
		#define CRI_CCR0          CRI_BASE + 0x00
		#define CRI_CCR1          CRI_BASE + 0x01*4
		#define CRI_CDC0          CRI_BASE + 0x02*4
		#define CRI_CDC1          CRI_BASE + 0x03*4
		#define CRI_RST           CRI_BASE + 0x04*4
		#define CRI_MASK0         CRI_BASE + 0x05*4
		#define CRI_MASK1         CRI_BASE + 0x06*4
		#define CRI_MASK2         CRI_BASE + 0x07*4
		#define CRI_STATUS0       CRI_BASE + 0x08*4
		#define CRI_STATUS1       CRI_BASE + 0x09*4
		#define CRI_STATUS2       CRI_BASE + 0x0A*4
		#define CRI_AMASK0        CRI_BASE + 0x0B*4
		#define CRI_AMASK1        CRI_BASE + 0x0C*4
		#define CRI_UPDCTL        CRI_BASE + 0x0D*4
		#define CRI_MADST         CRI_BASE + 0x0E*4
		// 0x0f is missing
		#define CRI_EVENT0        CRI_BASE + 0x10*4
		#define CRI_EVENT1        CRI_BASE + 0x11*4
		#define CRI_EVENT2        CRI_BASE + 0x12*4

		#define IRI_I_ENABLE    0x32000
		#define STY_SMODE       0x3c004
		#define AFE_TCR_0       0x3c0dc
		#define AFE_ADDR_ADDR   0x3c0e8
		#define AFE_RDATA_ADDR  0x3c0ec
		#define AFE_WDATA_ADDR  0x3c0f0
		#define AFE_CONFIG      0x3c0f4
		#define AFE_SERIAL_CFG  0x3c0fc

		#define DFE_BASE_ADDR         0xBE116000
		//#define DFE_BASE_ADDR         0x9E116000

		#define MEI_FR_ARCINT_C       (DFE_BASE_ADDR + 0x0000001C)
		#define MEI_DBG_WADDR_C       (DFE_BASE_ADDR + 0x00000024)
		#define MEI_DBG_RADDR_C       (DFE_BASE_ADDR + 0x00000028)
		#define MEI_DBG_DATA_C        (DFE_BASE_ADDR + 0x0000002C)
		#define MEI_DBG_DECO_C        (DFE_BASE_ADDR + 0x00000030)
		#define MEI_DBG_MASTER_C      (DFE_BASE_ADDR + 0x0000003C)

		static void WriteARCmem(uint32_t addr, uint32_t data)
		{
			writel(1    ,(volatile uint32_t *)MEI_DBG_MASTER_C);
			writel(1    ,(volatile uint32_t *)MEI_DBG_DECO_C  );
			writel(addr ,(volatile uint32_t *)MEI_DBG_WADDR_C  );
			writel(data ,(volatile uint32_t *)MEI_DBG_DATA_C  );
			while( (ifxusb_rreg((volatile uint32_t *)MEI_FR_ARCINT_C) & 0x20) != 0x20 ){};
			writel(0    ,(volatile uint32_t *)MEI_DBG_MASTER_C);
			IFX_DEBUGP("WriteARCmem %08x %08x\n",addr,data);
		};

		static uint32_t ReadARCmem(uint32_t addr)
		{
			u32 data;
			writel(1    ,(volatile uint32_t *)MEI_DBG_MASTER_C);
			writel(1    ,(volatile uint32_t *)MEI_DBG_DECO_C  );
			writel(addr ,(volatile uint32_t *)MEI_DBG_RADDR_C  );
			while( (ifxusb_rreg((volatile uint32_t *)MEI_FR_ARCINT_C) & 0x20) != 0x20 ){};
			data = ifxusb_rreg((volatile uint32_t *)MEI_DBG_DATA_C  );
			writel(0    ,(volatile uint32_t *)MEI_DBG_MASTER_C);
			IFX_DEBUGP("ReadARCmem %08x %08x\n",addr,data);
		  return data;
		};

		void ifxusb_enable_afe_oc(void)
		{
			/* Start the clock */
			WriteARCmem(CRI_UPDCTL    ,0x00000008);
			WriteARCmem(CRI_CCR0      ,0x00000014);
			WriteARCmem(CRI_CCR1      ,0x00000500);
			WriteARCmem(AFE_CONFIG    ,0x000001c8);
			WriteARCmem(AFE_SERIAL_CFG,0x00000016); // (DANUBE_PCI_CFG_BASE+(1<<addrline))AFE serial interface clock & data latch edge
			WriteARCmem(AFE_TCR_0     ,0x00000002);
			//Take afe out of reset
			WriteARCmem(AFE_CONFIG    ,0x000000c0);
			WriteARCmem(IRI_I_ENABLE  ,0x00000101);
			WriteARCmem(STY_SMODE     ,0x00001980);

			ReadARCmem(CRI_UPDCTL    );
			ReadARCmem(CRI_CCR0      );
			ReadARCmem(CRI_CCR1      );
			ReadARCmem(AFE_CONFIG    );
			ReadARCmem(AFE_SERIAL_CFG); // (DANUBE_PCI_CFG_BASE+(1<<addrline))AFE serial interface clock & data latch edge
			ReadARCmem(AFE_TCR_0     );
			ReadARCmem(AFE_CONFIG    );
			ReadARCmem(IRI_I_ENABLE  );
			ReadARCmem(STY_SMODE     );
		}
	#endif  //defined(__IS_TWINPASS__)
#endif //defined(__UEIP__)


