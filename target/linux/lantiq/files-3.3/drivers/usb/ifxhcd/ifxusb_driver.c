/*****************************************************************************
 **   FILE NAME       : ifxusb_driver.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The provides the initialization and cleanup entry
 **                     points for the IFX USB driver. This module can be
 **                     dynamically loaded with insmod command or built-in
 **                     with kernel. When loaded or executed the ifxusb_driver_init
 **                     function is called. When the module is removed (using rmmod),
 **                     the ifxusb_driver_cleanup function is called.
 *****************************************************************************/

/*!
 \file ifxusb_driver.c
 \brief This file contains the loading/unloading interface to the Linux driver.
*/

#include <linux/version.h>
#include "ifxusb_version.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/device.h>
#include <linux/platform_device.h>

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/stat.h>  /* permission constants */
#include <linux/gpio.h>
#include <lantiq_soc.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	#include <linux/irq.h>
#endif

#include <asm/io.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
	#include <asm/irq.h>
#endif

#include "ifxusb_plat.h"

#include "ifxusb_cif.h"

#ifdef __IS_HOST__
	#include "ifxhcd.h"

	#define    USB_DRIVER_DESC		"IFX USB HCD driver"
	const char ifxusb_driver_name[]    = "ifxusb_hcd";

	#ifdef __IS_DUAL__
		ifxhcd_hcd_t ifxusb_hcd_1;
		ifxhcd_hcd_t ifxusb_hcd_2;
		const char ifxusb_hcd_name_1[] = "ifxusb_hcd_1";
		const char ifxusb_hcd_name_2[] = "ifxusb_hcd_2";
	#else
		ifxhcd_hcd_t ifxusb_hcd;
		const char ifxusb_hcd_name[]   = "ifxusb_hcd";
	#endif

	#if defined(__DO_OC_INT__)
		static unsigned int  oc_int_installed=0;
		static ifxhcd_hcd_t *oc_int_id=NULL;
	#endif
#endif

#ifdef __IS_DEVICE__
	#include "ifxpcd.h"

	#define    USB_DRIVER_DESC		"IFX USB PCD driver"
	const char ifxusb_driver_name[] = "ifxusb_pcd";

	ifxpcd_pcd_t ifxusb_pcd;
	const char ifxusb_pcd_name[]    = "ifxusb_pcd";
#endif

/* Global Debug Level Mask. */
#ifdef __IS_HOST__
	uint32_t h_dbg_lvl = 0x00;
#endif

#ifdef __IS_DEVICE__
	uint32_t d_dbg_lvl = 0x00;
#endif

ifxusb_params_t ifxusb_module_params;

static void parse_parms(void);


#include <lantiq_irq.h>
#define IFX_USB0_IR                     (INT_NUM_IM1_IRL0 + 22)
#define IFX_USB1_IR                     (INT_NUM_IM2_IRL0 + 19)

/*!
   \brief This function is called when a driver is unregistered. This happens when
  the rmmod command is executed. The device may or may not be electrically
  present. If it is present, the driver stops device processing. Any resources
  used on behalf of this device are freed.
*/
static int ifxusb_driver_remove(struct platform_device *_dev)
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#ifdef __IS_HOST__
		#if defined(__DO_OC_INT__)
			#if defined(__DO_OC_INT_ENABLE__)
				ifxusb_oc_int_off();
			#endif

			if(oc_int_installed && oc_int_id)
				free_irq((unsigned int)IFXUSB_OC_IRQ, oc_int_id );
			oc_int_installed=0;
			oc_int_id=NULL;
		#endif

		#if defined(__IS_DUAL__)
			ifxhcd_remove(&ifxusb_hcd_1);
			ifxusb_core_if_remove(&ifxusb_hcd_1.core_if );
			ifxhcd_remove(&ifxusb_hcd_2);
			ifxusb_core_if_remove(&ifxusb_hcd_2.core_if );
		#else
			ifxhcd_remove(&ifxusb_hcd);
			ifxusb_core_if_remove(&ifxusb_hcd.core_if );
		#endif
	#endif

	#ifdef __IS_DEVICE__
		ifxpcd_remove();
		ifxusb_core_if_remove(&ifxusb_pcd.core_if );
	#endif

	/* Remove the device attributes */

	ifxusb_attr_remove(&_dev->dev);

	return 0;
}


/* Function to setup the structures to control one usb core running as host*/
#ifdef __IS_HOST__
/*!
   \brief inlined by ifxusb_driver_probe(), handling host mode probing. Run at each host core.
*/
	static inline int ifxusb_driver_probe_h(ifxhcd_hcd_t *_hcd,
	                                        int           _irq,
	                                        uint32_t      _iobase,
	                                        uint32_t      _fifomem,
	                                        uint32_t      _fifodbg
	                                        )
	{
		int retval = 0;

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

#ifdef __DEV_NEW__
		ifxusb_power_off  (&_hcd->core_if);
		ifxusb_phy_power_off  (&_hcd->core_if); // Test
		mdelay(500);
#endif //__DEV_NEW__
		ifxusb_power_on  (&_hcd->core_if);
		mdelay(50);
		ifxusb_phy_power_on  (&_hcd->core_if); // Test
		mdelay(50);
		ifxusb_hard_reset(&_hcd->core_if);
		retval =ifxusb_core_if_init(&_hcd->core_if,
		                             _irq,
		                             _iobase,
		                             _fifomem,
		                             _fifodbg);
		if(retval)
			return retval;

		ifxusb_host_core_init(&_hcd->core_if,&ifxusb_module_params);

		ifxusb_disable_global_interrupts( &_hcd->core_if);

		/* The driver is now initialized and need to be registered into Linux USB sub-system */

		retval = ifxhcd_init(_hcd); // hook the hcd into usb ss

		if (retval != 0)
		{
			IFX_ERROR("_hcd_init failed\n");
			return retval;
		}

		//ifxusb_enable_global_interrupts( _hcd->core_if ); // this should be done at hcd_start , including hcd_interrupt
		return 0;
	}
#endif //__IS_HOST__

#ifdef __IS_DEVICE__
/*!
  \brief inlined by ifxusb_driver_probe(), handling device mode probing.
*/
	static inline int ifxusb_driver_probe_d(ifxpcd_pcd_t *_pcd,
	                                        int           _irq,
	                                        uint32_t      _iobase,
	                                        uint32_t      _fifomem,
	                                        uint32_t      _fifodbg
	                                        )
	{
		int retval = 0;

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
#ifdef __DEV_NEW__
		ifxusb_power_off  (&_pcd->core_if);
		ifxusb_phy_power_off (&_pcd->core_if); // Test
		mdelay(500);
#endif // __DEV_NEW__
		ifxusb_power_on  (&_pcd->core_if);
		mdelay(50);
		ifxusb_phy_power_on  (&_pcd->core_if); // Test
		mdelay(50);
		ifxusb_hard_reset(&_pcd->core_if);
		retval =ifxusb_core_if_init(&_pcd->core_if,
		                             _irq,
		                             _iobase,
		                             _fifomem,
		                             _fifodbg);
		if(retval)
			return retval;

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		ifxusb_dev_core_init(&_pcd->core_if,&ifxusb_module_params);

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		ifxusb_disable_global_interrupts( &_pcd->core_if);

		/* The driver is now initialized and need to be registered into
		   Linux USB Gadget sub-system
		 */
		retval = ifxpcd_init();
		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

		if (retval != 0)
		{
			IFX_ERROR("_pcd_init failed\n");
			return retval;
		}
		//ifxusb_enable_global_interrupts( _pcd->core_if );  // this should be done at gadget bind or start
		return 0;
	}
#endif //__IS_DEVICE__



/*!
   \brief This function is called by module management in 2.6 kernel or by ifxusb_driver_init with 2.4 kernel
  It is to probe and setup IFXUSB core(s).
*/
static int ifxusb_driver_probe(struct platform_device *_dev)
{
	int retval = 0;
	int *pins = _dev->dev.platform_data;
	if (ltq_is_vr9()) {
		gpio_request(6, "id1");
		gpio_request(9, "id2");
		gpio_direction_input(6);
		gpio_direction_input(9);
	}
	if (pins) {
		if (pins[0]) {
			gpio_request(pins[0], "vbus1");
			gpio_direction_output(pins[0], 1);
		}
		if (pins[1] && ltq_is_vr9()) {
			gpio_request(pins[1], "vbus2");
			gpio_direction_output(pins[1], 1);
		}
	}
	// Parsing and store the parameters
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	parse_parms();

	#ifdef __IS_HOST__
		#if   defined(__IS_DUAL__)
			memset(&ifxusb_hcd_1, 0, sizeof(ifxhcd_hcd_t));
			memset(&ifxusb_hcd_2, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd_1.core_if.core_no=0;
			ifxusb_hcd_2.core_if.core_no=1;
			ifxusb_hcd_1.core_if.core_name=(char *)ifxusb_hcd_name_1;
			ifxusb_hcd_2.core_if.core_name=(char *)ifxusb_hcd_name_2;

			ifxusb_hcd_1.dev=&_dev->dev;
			ifxusb_hcd_2.dev=&_dev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd_1,
			                               IFX_USB0_IR,
			                               IFXUSB1_IOMEM_BASE,
			                               IFXUSB1_FIFOMEM_BASE,
			                               IFXUSB1_FIFODBG_BASE
			                               );
			if(retval)
				goto ifxusb_driver_probe_fail;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd_2,
			                               IFX_USB1_IR,
			                               IFXUSB2_IOMEM_BASE,
			                               IFXUSB2_FIFOMEM_BASE,
			                               IFXUSB2_FIFODBG_BASE
			                              );
			if(retval)
				goto ifxusb_driver_probe_fail;

		#elif defined(__IS_FIRST__)
			memset(&ifxusb_hcd, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd.core_if.core_no=0;
			ifxusb_hcd.core_if.core_name=(char *)ifxusb_hcd_name;

			ifxusb_hcd.dev=&_dev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFX_USB0_IR,
			                               IFXUSB1_IOMEM_BASE,
			                               IFXUSB1_FIFOMEM_BASE,
			                               IFXUSB1_FIFODBG_BASE
			                              );
			if(retval)
				goto ifxusb_driver_probe_fail;

		#elif defined(__IS_SECOND__)
			memset(&ifxusb_hcd, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd.core_if.core_no=1;
			ifxusb_hcd.core_if.core_name=(char *)ifxusb_hcd_name;

			ifxusb_hcd.dev=&_dev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFX_USB1_IR,
			                               IFXUSB2_IOMEM_BASE,
			                               IFXUSB2_FIFOMEM_BASE,
			                               IFXUSB2_FIFODBG_BASE
			                              );
			if(retval)
				goto ifxusb_driver_probe_fail;

		#else
			memset(&ifxusb_hcd, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd.core_if.core_no=0;
			ifxusb_hcd.core_if.core_name=(char *)ifxusb_hcd_name;

			ifxusb_hcd.dev=&_dev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFXUSB_IRQ,
			                               IFXUSB_IOMEM_BASE,
			                               IFXUSB_FIFOMEM_BASE,
			                               IFXUSB_FIFODBG_BASE
			                              );
			if(retval)
				goto ifxusb_driver_probe_fail;
		#endif

		#if defined(__DO_OC_INT__)
			IFXUSB_DEBUGPL( DBG_CIL, "registering (overcurrent) handler for irq%d\n", IFXUSB_OC_IRQ);
			#if   defined(__IS_DUAL__)
				request_irq((unsigned int)IFXUSB_OC_IRQ, &ifx_hcd_oc_irq,
//				  SA_INTERRUPT|SA_SHIRQ, "ifxusb_oc", (void *)&ifxusb_hcd_1);
				  IRQF_DISABLED | IRQF_SHARED, "ifxusb_oc", (void *)&ifxusb_hcd_1);
				oc_int_id=&ifxusb_hcd_1;
			#else
				request_irq((unsigned int)IFXUSB_OC_IRQ, &ifx_hcd_oc_irq,
//				  SA_INTERRUPT|SA_SHIRQ, "ifxusb_oc", (void *)&ifxusb_hcd);
				  IRQF_DISABLED | IRQF_SHARED, "ifxusb_oc", (void *)&ifxusb_hcd);
				oc_int_id=&ifxusb_hcd;
			#endif
			oc_int_installed=1;

			#if defined(__DO_OC_INT_ENABLE__)
				ifxusb_oc_int_on();
			#endif
		#endif

	#endif

	#ifdef __IS_DEVICE__
		memset(&ifxusb_pcd, 0, sizeof(ifxpcd_pcd_t));
		ifxusb_pcd.core_if.core_name=(char *)&ifxusb_pcd_name[0];

		ifxusb_pcd.dev=&_dev->dev;

		#if   defined(__IS_FIRST__)
			ifxusb_pcd.core_if.core_no=0;
			retval = ifxusb_driver_probe_d(&ifxusb_pcd,
			                               IFXUSB1_IRQ,
			                               IFXUSB1_IOMEM_BASE,
			                               IFXUSB1_FIFOMEM_BASE,
			                               IFXUSB1_FIFODBG_BASE
			                              );
		#elif defined(__IS_SECOND__)
			ifxusb_pcd.core_if.core_no=1;
			retval = ifxusb_driver_probe_d(&ifxusb_pcd,
			                               IFXUSB2_IRQ,
			                               IFXUSB2_IOMEM_BASE,
			                               IFXUSB2_FIFOMEM_BASE,
			                               IFXUSB2_FIFODBG_BASE
			                              );
		#else
			ifxusb_pcd.core_if.core_no=0;
			retval = ifxusb_driver_probe_d(&ifxusb_pcd,
			                               IFXUSB_IRQ,
			                               IFXUSB_IOMEM_BASE,
			                               IFXUSB_FIFOMEM_BASE,
			                               IFXUSB_FIFODBG_BASE
			                              );
		#endif
		if(retval)
			goto ifxusb_driver_probe_fail;
	#endif

	ifxusb_attr_create(&_dev->dev);

	return 0;

ifxusb_driver_probe_fail:
	ifxusb_driver_remove(_dev);
	return retval;
}



/*!
   \brief This function is called when the ifxusb_driver is installed with the insmod command.
*/


static struct platform_driver ifxusb_driver = {
	.driver = {
		.name		= ifxusb_driver_name,
		.owner = THIS_MODULE,
	},
	.probe		= ifxusb_driver_probe,
	.remove		= ifxusb_driver_remove,
};

int __init ifxusb_driver_init(void)
{
	int retval = 0;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	IFX_PRINT("%s: version %s\n", ifxusb_driver_name, IFXUSB_VERSION);

	retval = platform_driver_register(&ifxusb_driver);

	if (retval < 0) {
		IFX_ERROR("%s retval=%d\n", __func__, retval);
		return retval;
	}
	return retval;
}

#if 0 // 2.4
	int __init ifxusb_driver_init(void)
	{
		int retval = 0;
		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		IFX_PRINT("%s: version %s\n", ifxusb_driver_name, IFXUSB_VERSION);
		retval = ifxusb_driver_probe();

		if (retval < 0) {
			IFX_ERROR("%s retval=%d\n", __func__, retval);
			return retval;
		}

		return retval;
	}
#endif

module_init(ifxusb_driver_init);


/*!
   \brief This function is called when the driver is removed from the kernel
  with the rmmod command. The driver unregisters itself with its bus
  driver.
*/

void __exit ifxusb_driver_cleanup(void)
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

	platform_driver_unregister(&ifxusb_driver);

	IFX_PRINT("%s module removed\n", ifxusb_driver_name);
}
#if 0
	void __exit ifxusb_driver_cleanup(void)
	{
		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		ifxusb_driver_remove();
		IFX_PRINT("%s module removed\n", ifxusb_driver_name);
	}
#endif
module_exit(ifxusb_driver_cleanup);



MODULE_DESCRIPTION(USB_DRIVER_DESC);
MODULE_AUTHOR("Infineon");
MODULE_LICENSE("GPL");



// Parameters set when loaded
//static long  dbg_lvl =0xFFFFFFFF;
static long  dbg_lvl =0;
static short dma_burst_size =-1;
static short speed =-1;
static long  data_fifo_size =-1;
#ifdef __IS_DEVICE__
	static long   rx_fifo_size =-1;
	#ifdef __DED_FIFO__
		static long  tx_fifo_size_00 =-1;
		static long  tx_fifo_size_01 =-1;
		static long  tx_fifo_size_02 =-1;
		static long  tx_fifo_size_03 =-1;
		static long  tx_fifo_size_04 =-1;
		static long  tx_fifo_size_05 =-1;
		static long  tx_fifo_size_06 =-1;
		static long  tx_fifo_size_07 =-1;
		static long  tx_fifo_size_08 =-1;
		static long  tx_fifo_size_09 =-1;
		static long  tx_fifo_size_10 =-1;
		static long  tx_fifo_size_11 =-1;
		static long  tx_fifo_size_12 =-1;
		static long  tx_fifo_size_13 =-1;
		static long  tx_fifo_size_14 =-1;
		static long  tx_fifo_size_15 =-1;
		static short thr_ctl=-1;
		static long  tx_thr_length =-1;
		static long  rx_thr_length =-1;
	#else
		static long   nperio_tx_fifo_size =-1;
		static long   perio_tx_fifo_size_01 =-1;
		static long   perio_tx_fifo_size_02 =-1;
		static long   perio_tx_fifo_size_03 =-1;
		static long   perio_tx_fifo_size_04 =-1;
		static long   perio_tx_fifo_size_05 =-1;
		static long   perio_tx_fifo_size_06 =-1;
		static long   perio_tx_fifo_size_07 =-1;
		static long   perio_tx_fifo_size_08 =-1;
		static long   perio_tx_fifo_size_09 =-1;
		static long   perio_tx_fifo_size_10 =-1;
		static long   perio_tx_fifo_size_11 =-1;
		static long   perio_tx_fifo_size_12 =-1;
		static long   perio_tx_fifo_size_13 =-1;
		static long   perio_tx_fifo_size_14 =-1;
		static long   perio_tx_fifo_size_15 =-1;
	#endif
	static short   dev_endpoints =-1;
#endif

#ifdef __IS_HOST__
	static long   rx_fifo_size =-1;
	static long   nperio_tx_fifo_size =-1;
	static long   perio_tx_fifo_size =-1;
	static short  host_channels =-1;
#endif

static long   max_transfer_size =-1;
static long   max_packet_count =-1;
static long   phy_utmi_width =-1;
static long   turn_around_time_hs =-1;
static long   turn_around_time_fs =-1;
static long   timeout_cal_hs =-1;
static long   timeout_cal_fs =-1;

/*!
   \brief Parsing the parameters taken when module load
*/
static void parse_parms(void)
{

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#ifdef __IS_HOST__
		h_dbg_lvl=dbg_lvl;
	#endif
	#ifdef __IS_DEVICE__
		d_dbg_lvl=dbg_lvl;
	#endif

	switch(dma_burst_size)
	{
		case 0:
		case 1:
		case 4:
		case 8:
		case 16:
			ifxusb_module_params.dma_burst_size=dma_burst_size;
			break;
		default:
			ifxusb_module_params.dma_burst_size=default_param_dma_burst_size;
	}

	if(speed==0 || speed==1)
		ifxusb_module_params.speed=speed;
	else
		ifxusb_module_params.speed=default_param_speed;

	if(max_transfer_size>=2048 && max_transfer_size<=65535)
		ifxusb_module_params.max_transfer_size=max_transfer_size;
	else
		ifxusb_module_params.max_transfer_size=default_param_max_transfer_size;

	if(max_packet_count>=15 && max_packet_count<=511)
		ifxusb_module_params.max_packet_count=max_packet_count;
	else
		ifxusb_module_params.max_packet_count=default_param_max_packet_count;

	switch(phy_utmi_width)
	{
		case 8:
		case 16:
			ifxusb_module_params.phy_utmi_width=phy_utmi_width;
			break;
		default:
			ifxusb_module_params.phy_utmi_width=default_param_phy_utmi_width;
	}

	if(turn_around_time_hs>=0 && turn_around_time_hs<=7)
		ifxusb_module_params.turn_around_time_hs=turn_around_time_hs;
	else
		ifxusb_module_params.turn_around_time_hs=default_param_turn_around_time_hs;

	if(turn_around_time_fs>=0 && turn_around_time_fs<=7)
		ifxusb_module_params.turn_around_time_fs=turn_around_time_fs;
	else
		ifxusb_module_params.turn_around_time_fs=default_param_turn_around_time_fs;

	if(timeout_cal_hs>=0 && timeout_cal_hs<=7)
		ifxusb_module_params.timeout_cal_hs=timeout_cal_hs;
	else
		ifxusb_module_params.timeout_cal_hs=default_param_timeout_cal_hs;

	if(timeout_cal_fs>=0 && timeout_cal_fs<=7)
		ifxusb_module_params.timeout_cal_fs=timeout_cal_fs;
	else
		ifxusb_module_params.timeout_cal_fs=default_param_timeout_cal_fs;

	if(data_fifo_size>=32 && data_fifo_size<=32768)
		ifxusb_module_params.data_fifo_size=data_fifo_size;
	else
		ifxusb_module_params.data_fifo_size=default_param_data_fifo_size;

	#ifdef __IS_HOST__
		if(host_channels>=1 && host_channels<=16)
			ifxusb_module_params.host_channels=host_channels;
		else
			ifxusb_module_params.host_channels=default_param_host_channels;

		if(rx_fifo_size>=16 && rx_fifo_size<=32768)
			ifxusb_module_params.rx_fifo_size=rx_fifo_size;
		else
			ifxusb_module_params.rx_fifo_size=default_param_rx_fifo_size;

		if(nperio_tx_fifo_size>=16 && nperio_tx_fifo_size<=32768)
			ifxusb_module_params.nperio_tx_fifo_size=nperio_tx_fifo_size;
		else
			ifxusb_module_params.nperio_tx_fifo_size=default_param_nperio_tx_fifo_size;

		if(perio_tx_fifo_size>=16 && perio_tx_fifo_size<=32768)
			ifxusb_module_params.perio_tx_fifo_size=perio_tx_fifo_size;
		else
			ifxusb_module_params.perio_tx_fifo_size=default_param_perio_tx_fifo_size;
	#endif //__IS_HOST__

	#ifdef __IS_DEVICE__
		if(rx_fifo_size>=16 && rx_fifo_size<=32768)
			ifxusb_module_params.rx_fifo_size=rx_fifo_size;
		else
			ifxusb_module_params.rx_fifo_size=default_param_rx_fifo_size;
		#ifdef __DED_FIFO__
			if(tx_fifo_size_00>=16 && tx_fifo_size_00<=32768)
				ifxusb_module_params.tx_fifo_size[ 0]=tx_fifo_size_00;
			else
				ifxusb_module_params.tx_fifo_size[ 0]=default_param_tx_fifo_size_00;
			if(tx_fifo_size_01>=0 && tx_fifo_size_01<=32768)
				ifxusb_module_params.tx_fifo_size[ 1]=tx_fifo_size_01;
			else
				ifxusb_module_params.tx_fifo_size[ 1]=default_param_tx_fifo_size_01;
			if(tx_fifo_size_02>=0 && tx_fifo_size_02<=32768)
				ifxusb_module_params.tx_fifo_size[ 2]=tx_fifo_size_02;
			else
				ifxusb_module_params.tx_fifo_size[ 2]=default_param_tx_fifo_size_02;
			if(tx_fifo_size_03>=0 && tx_fifo_size_03<=32768)
				ifxusb_module_params.tx_fifo_size[ 3]=tx_fifo_size_03;
			else
				ifxusb_module_params.tx_fifo_size[ 3]=default_param_tx_fifo_size_03;
			if(tx_fifo_size_04>=0 && tx_fifo_size_04<=32768)
				ifxusb_module_params.tx_fifo_size[ 4]=tx_fifo_size_04;
			else
				ifxusb_module_params.tx_fifo_size[ 4]=default_param_tx_fifo_size_04;
			if(tx_fifo_size_05>=0 && tx_fifo_size_05<=32768)
				ifxusb_module_params.tx_fifo_size[ 5]=tx_fifo_size_05;
			else
				ifxusb_module_params.tx_fifo_size[ 5]=default_param_tx_fifo_size_05;
			if(tx_fifo_size_06>=0 && tx_fifo_size_06<=32768)
				ifxusb_module_params.tx_fifo_size[ 6]=tx_fifo_size_06;
			else
				ifxusb_module_params.tx_fifo_size[ 6]=default_param_tx_fifo_size_06;
			if(tx_fifo_size_07>=0 && tx_fifo_size_07<=32768)
				ifxusb_module_params.tx_fifo_size[ 7]=tx_fifo_size_07;
			else
				ifxusb_module_params.tx_fifo_size[ 7]=default_param_tx_fifo_size_07;
			if(tx_fifo_size_08>=0 && tx_fifo_size_08<=32768)
				ifxusb_module_params.tx_fifo_size[ 8]=tx_fifo_size_08;
			else
				ifxusb_module_params.tx_fifo_size[ 8]=default_param_tx_fifo_size_08;
			if(tx_fifo_size_09>=0 && tx_fifo_size_09<=32768)
				ifxusb_module_params.tx_fifo_size[ 9]=tx_fifo_size_09;
			else
				ifxusb_module_params.tx_fifo_size[ 9]=default_param_tx_fifo_size_09;
			if(tx_fifo_size_10>=0 && tx_fifo_size_10<=32768)
				ifxusb_module_params.tx_fifo_size[10]=tx_fifo_size_10;
			else
				ifxusb_module_params.tx_fifo_size[10]=default_param_tx_fifo_size_10;
			if(tx_fifo_size_11>=0 && tx_fifo_size_11<=32768)
				ifxusb_module_params.tx_fifo_size[11]=tx_fifo_size_11;
			else
				ifxusb_module_params.tx_fifo_size[11]=default_param_tx_fifo_size_11;
			if(tx_fifo_size_12>=0 && tx_fifo_size_12<=32768)
				ifxusb_module_params.tx_fifo_size[12]=tx_fifo_size_12;
			else
				ifxusb_module_params.tx_fifo_size[12]=default_param_tx_fifo_size_12;
			if(tx_fifo_size_13>=0 && tx_fifo_size_13<=32768)
				ifxusb_module_params.tx_fifo_size[13]=tx_fifo_size_13;
			else
				ifxusb_module_params.tx_fifo_size[13]=default_param_tx_fifo_size_13;
			if(tx_fifo_size_14>=0 && tx_fifo_size_14<=32768)
				ifxusb_module_params.tx_fifo_size[14]=tx_fifo_size_14;
			else
				ifxusb_module_params.tx_fifo_size[14]=default_param_tx_fifo_size_14;
			if(tx_fifo_size_15>=0 && tx_fifo_size_15<=32768)
				ifxusb_module_params.tx_fifo_size[15]=tx_fifo_size_15;
			else
				ifxusb_module_params.tx_fifo_size[15]=default_param_tx_fifo_size_15;
			if(thr_ctl==0 || thr_ctl==1)
				ifxusb_module_params.thr_ctl=thr_ctl;
			else
				ifxusb_module_params.thr_ctl=default_param_thr_ctl;
			if(tx_thr_length>=16 && tx_thr_length<=511)
				ifxusb_module_params.tx_thr_length=tx_thr_length;
			else
				ifxusb_module_params.tx_thr_length=default_param_tx_thr_length;
			if(rx_thr_length>=16 && rx_thr_length<=511)
				ifxusb_module_params.rx_thr_length=rx_thr_length;
			else
				ifxusb_module_params.rx_thr_length=default_param_rx_thr_length;
		#else  //__DED_FIFO__
			if(nperio_tx_fifo_size>=16 && nperio_tx_fifo_size<=32768)
				ifxusb_module_params.tx_fifo_size[ 0]=nperio_tx_fifo_size;
			else
				ifxusb_module_params.tx_fifo_size[ 0]=default_param_nperio_tx_fifo_size;
			if(perio_tx_fifo_size_01>=0 && perio_tx_fifo_size_01<=32768)
				ifxusb_module_params.tx_fifo_size[ 1]=perio_tx_fifo_size_01;
			else
				ifxusb_module_params.tx_fifo_size[ 1]=default_param_perio_tx_fifo_size_01;
			if(perio_tx_fifo_size_02>=0 && perio_tx_fifo_size_02<=32768)
				ifxusb_module_params.tx_fifo_size[ 2]=perio_tx_fifo_size_02;
			else
				ifxusb_module_params.tx_fifo_size[ 2]=default_param_perio_tx_fifo_size_02;
			if(perio_tx_fifo_size_03>=0 && perio_tx_fifo_size_03<=32768)
				ifxusb_module_params.tx_fifo_size[ 3]=perio_tx_fifo_size_03;
			else
				ifxusb_module_params.tx_fifo_size[ 3]=default_param_perio_tx_fifo_size_03;
			if(perio_tx_fifo_size_04>=0 && perio_tx_fifo_size_04<=32768)
				ifxusb_module_params.tx_fifo_size[ 4]=perio_tx_fifo_size_04;
			else
				ifxusb_module_params.tx_fifo_size[ 4]=default_param_perio_tx_fifo_size_04;
			if(perio_tx_fifo_size_05>=0 && perio_tx_fifo_size_05<=32768)
				ifxusb_module_params.tx_fifo_size[ 5]=perio_tx_fifo_size_05;
			else
				ifxusb_module_params.tx_fifo_size[ 5]=default_param_perio_tx_fifo_size_05;
			if(perio_tx_fifo_size_06>=0 && perio_tx_fifo_size_06<=32768)
				ifxusb_module_params.tx_fifo_size[ 6]=perio_tx_fifo_size_06;
			else
				ifxusb_module_params.tx_fifo_size[ 6]=default_param_perio_tx_fifo_size_06;
			if(perio_tx_fifo_size_07>=0 && perio_tx_fifo_size_07<=32768)
				ifxusb_module_params.tx_fifo_size[ 7]=perio_tx_fifo_size_07;
			else
				ifxusb_module_params.tx_fifo_size[ 7]=default_param_perio_tx_fifo_size_07;
			if(perio_tx_fifo_size_08>=0 && perio_tx_fifo_size_08<=32768)
				ifxusb_module_params.tx_fifo_size[ 8]=perio_tx_fifo_size_08;
			else
				ifxusb_module_params.tx_fifo_size[ 8]=default_param_perio_tx_fifo_size_08;
			if(perio_tx_fifo_size_09>=0 && perio_tx_fifo_size_09<=32768)
				ifxusb_module_params.tx_fifo_size[ 9]=perio_tx_fifo_size_09;
			else
				ifxusb_module_params.tx_fifo_size[ 9]=default_param_perio_tx_fifo_size_09;
			if(perio_tx_fifo_size_10>=0 && perio_tx_fifo_size_10<=32768)
				ifxusb_module_params.tx_fifo_size[10]=perio_tx_fifo_size_10;
			else
				ifxusb_module_params.tx_fifo_size[10]=default_param_perio_tx_fifo_size_10;
			if(perio_tx_fifo_size_11>=0 && perio_tx_fifo_size_11<=32768)
				ifxusb_module_params.tx_fifo_size[11]=perio_tx_fifo_size_11;
			else
				ifxusb_module_params.tx_fifo_size[11]=default_param_perio_tx_fifo_size_11;
			if(perio_tx_fifo_size_12>=0 && perio_tx_fifo_size_12<=32768)
				ifxusb_module_params.tx_fifo_size[12]=perio_tx_fifo_size_12;
			else
				ifxusb_module_params.tx_fifo_size[12]=default_param_perio_tx_fifo_size_12;
			if(perio_tx_fifo_size_13>=0 && perio_tx_fifo_size_13<=32768)
				ifxusb_module_params.tx_fifo_size[13]=perio_tx_fifo_size_13;
			else
				ifxusb_module_params.tx_fifo_size[13]=default_param_perio_tx_fifo_size_13;
			if(perio_tx_fifo_size_14>=0 && perio_tx_fifo_size_14<=32768)
				ifxusb_module_params.tx_fifo_size[14]=perio_tx_fifo_size_14;
			else
				ifxusb_module_params.tx_fifo_size[14]=default_param_perio_tx_fifo_size_14;
			if(perio_tx_fifo_size_15>=0 && perio_tx_fifo_size_15<=32768)
				ifxusb_module_params.tx_fifo_size[15]=perio_tx_fifo_size_15;
			else
				ifxusb_module_params.tx_fifo_size[15]=default_param_perio_tx_fifo_size_15;
		#endif //__DED_FIFO__
	#endif //__IS_DEVICE__
}







module_param(dbg_lvl, long, 0444);
MODULE_PARM_DESC(dbg_lvl, "Debug level.");

module_param(dma_burst_size, short, 0444);
MODULE_PARM_DESC(dma_burst_size, "DMA Burst Size 0, 1, 4, 8, 16");

module_param(speed, short, 0444);
MODULE_PARM_DESC(speed, "Speed 0=High Speed 1=Full Speed");

module_param(data_fifo_size, long, 0444);
MODULE_PARM_DESC(data_fifo_size, "Total number of words in the data FIFO memory 32-32768");

#ifdef __IS_DEVICE__
	module_param(rx_fifo_size, long, 0444);
	MODULE_PARM_DESC(rx_fifo_size, "Number of words in the Rx FIFO 16-32768");

	#ifdef __DED_FIFO__
		module_param(tx_fifo_size_00, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_00, "Number of words in the Tx FIFO #00 16-32768");
		module_param(tx_fifo_size_01, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_01, "Number of words in the Tx FIFO #01  0-32768");
		module_param(tx_fifo_size_02, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_02, "Number of words in the Tx FIFO #02  0-32768");
		module_param(tx_fifo_size_03, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_03, "Number of words in the Tx FIFO #03  0-32768");
		module_param(tx_fifo_size_04, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_04, "Number of words in the Tx FIFO #04  0-32768");
		module_param(tx_fifo_size_05, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_05, "Number of words in the Tx FIFO #05  0-32768");
		module_param(tx_fifo_size_06, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_06, "Number of words in the Tx FIFO #06  0-32768");
		module_param(tx_fifo_size_07, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_07, "Number of words in the Tx FIFO #07  0-32768");
		module_param(tx_fifo_size_08, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_08, "Number of words in the Tx FIFO #08  0-32768");
		module_param(tx_fifo_size_09, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_09, "Number of words in the Tx FIFO #09  0-32768");
		module_param(tx_fifo_size_10, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_10, "Number of words in the Tx FIFO #10  0-32768");
		module_param(tx_fifo_size_11, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_11, "Number of words in the Tx FIFO #11  0-32768");
		module_param(tx_fifo_size_12, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_12, "Number of words in the Tx FIFO #12  0-32768");
		module_param(tx_fifo_size_13, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_13, "Number of words in the Tx FIFO #13  0-32768");
		module_param(tx_fifo_size_14, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_14, "Number of words in the Tx FIFO #14  0-32768");
		module_param(tx_fifo_size_15, long, 0444);
		MODULE_PARM_DESC(tx_fifo_size_15, "Number of words in the Tx FIFO #15  0-32768");

		module_param(thr_ctl, short, 0444);
		MODULE_PARM_DESC(thr_ctl, "0=Without 1=With Theshold Ctrl");

		module_param(tx_thr_length, long, 0444);
		MODULE_PARM_DESC(tx_thr_length, "TX Threshold length");

		module_param(rx_thr_length, long, 0444);
		MODULE_PARM_DESC(rx_thr_length, "RX Threshold length");

	#else
		module_param(nperio_tx_fifo_size, long, 0444);
		MODULE_PARM_DESC(nperio_tx_fifo_size, "Number of words in the non-periodic Tx FIFO 16-32768");

		module_param(perio_tx_fifo_size_01, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_01, "Number of words in the periodic Tx FIFO #01  0-32768");
		module_param(perio_tx_fifo_size_02, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_02, "Number of words in the periodic Tx FIFO #02  0-32768");
		module_param(perio_tx_fifo_size_03, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_03, "Number of words in the periodic Tx FIFO #03  0-32768");
		module_param(perio_tx_fifo_size_04, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_04, "Number of words in the periodic Tx FIFO #04  0-32768");
		module_param(perio_tx_fifo_size_05, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_05, "Number of words in the periodic Tx FIFO #05  0-32768");
		module_param(perio_tx_fifo_size_06, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_06, "Number of words in the periodic Tx FIFO #06  0-32768");
		module_param(perio_tx_fifo_size_07, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_07, "Number of words in the periodic Tx FIFO #07  0-32768");
		module_param(perio_tx_fifo_size_08, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_08, "Number of words in the periodic Tx FIFO #08  0-32768");
		module_param(perio_tx_fifo_size_09, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_09, "Number of words in the periodic Tx FIFO #09  0-32768");
		module_param(perio_tx_fifo_size_10, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_10, "Number of words in the periodic Tx FIFO #10  0-32768");
		module_param(perio_tx_fifo_size_11, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_11, "Number of words in the periodic Tx FIFO #11  0-32768");
		module_param(perio_tx_fifo_size_12, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_12, "Number of words in the periodic Tx FIFO #12  0-32768");
		module_param(perio_tx_fifo_size_13, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_13, "Number of words in the periodic Tx FIFO #13  0-32768");
		module_param(perio_tx_fifo_size_14, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_14, "Number of words in the periodic Tx FIFO #14  0-32768");
		module_param(perio_tx_fifo_size_15, long, 0444);
		MODULE_PARM_DESC(perio_tx_fifo_size_15, "Number of words in the periodic Tx FIFO #15  0-32768");
	#endif//__DED_FIFO__
	module_param(dev_endpoints, short, 0444);
	MODULE_PARM_DESC(dev_endpoints, "The number of endpoints in addition to EP0 available for device mode 1-15");
#endif

#ifdef __IS_HOST__
	module_param(rx_fifo_size, long, 0444);
	MODULE_PARM_DESC(rx_fifo_size, "Number of words in the Rx FIFO 16-32768");

	module_param(nperio_tx_fifo_size, long, 0444);
	MODULE_PARM_DESC(nperio_tx_fifo_size, "Number of words in the non-periodic Tx FIFO 16-32768");

	module_param(perio_tx_fifo_size, long, 0444);
	MODULE_PARM_DESC(perio_tx_fifo_size, "Number of words in the host periodic Tx FIFO 16-32768");

	module_param(host_channels, short, 0444);
	MODULE_PARM_DESC(host_channels, "The number of host channel registers to use 1-16");
#endif

module_param(max_transfer_size, long, 0444);
MODULE_PARM_DESC(max_transfer_size, "The maximum transfer size supported in bytes 2047-65535");

module_param(max_packet_count, long, 0444);
MODULE_PARM_DESC(max_packet_count, "The maximum number of packets in a transfer 15-511");

module_param(phy_utmi_width, long, 0444);
MODULE_PARM_DESC(phy_utmi_width, "Specifies the UTMI+ Data Width 8 or 16 bits");

module_param(turn_around_time_hs, long, 0444);
MODULE_PARM_DESC(turn_around_time_hs, "Turn-Around time for HS");

module_param(turn_around_time_fs, long, 0444);
MODULE_PARM_DESC(turn_around_time_fs, "Turn-Around time for FS");

module_param(timeout_cal_hs, long, 0444);
MODULE_PARM_DESC(timeout_cal_hs, "Timeout Cal for HS");

module_param(timeout_cal_fs, long, 0444);
MODULE_PARM_DESC(timeout_cal_fs, "Timeout Cal for FS");


