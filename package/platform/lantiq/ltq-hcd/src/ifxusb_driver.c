/*****************************************************************************
 **   FILE NAME       : ifxusb_driver.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The provides the initialization and cleanup entry
 **                     points for the IFX USB driver. This module can be
 **                     dynamically loaded with insmod command or built-in
 **                     with kernel. When loaded or executed the ifxusb_driver_init
 **                     function is called. When the module is removed (using rmmod),
 **                     the ifxusb_driver_cleanup function is called.
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
#include <linux/of_platform.h>
#include <linux/of_gpio.h>

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/stat.h>  /* permission constants */

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
	const char ifxusb_hcd_driver_name[]    = "ifxusb_hcd";
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
		ifxhcd_hcd_t *oc_int_id=NULL;
		#ifdef __IS_DUAL__
			ifxhcd_hcd_t *oc_int_id_1=NULL;
			ifxhcd_hcd_t *oc_int_id_2=NULL;
		#endif
	#endif
#endif

#ifdef __IS_DEVICE__
	#include "ifxpcd.h"

	#define    USB_DRIVER_DESC		"IFX USB PCD driver"
	const char ifxusb_pcd_driver_name[] = "ifxusb_pcd";
	ifxpcd_pcd_t ifxusb_pcd;
	const char ifxusb_pcd_name[]        = "ifxusb_pcd";
#endif

/* Global Debug Level Mask. */
#ifdef __IS_HOST__
	uint32_t h_dbg_lvl = 0xff;
#endif

#ifdef __IS_DEVICE__
	uint32_t d_dbg_lvl = 0x00;
#endif

#ifdef __IS_HOST__
ifxusb_params_t ifxusb_module_params_h;
#else
ifxusb_params_t ifxusb_module_params_d;
#endif

static void parse_parms(void);


#if defined(__IS_TWINPASS__)
#warning "Compiled as TWINPASS"
#elif defined(__IS_DANUBE__)
#warning "Compiled as DANUBE"
#elif defined(__IS_AMAZON_SE__)
#warning "Compiled as AMAZON_SE"
#elif defined(__IS_AR9__)
#warning "Compiled as AR9"
#elif defined(__IS_VR9__)
#warning "Compiled as VR9"
#elif defined(__IS_AR10__)
#warning "Compiled as AR10"
#else
#error "No Platform defined"
#endif


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

		ifxusb_power_on_h (&_hcd->core_if);
		mdelay(50);
		ifxusb_phy_power_on_h  (&_hcd->core_if); // Test
		mdelay(50);
		ifxusb_hard_reset_h(&_hcd->core_if);
		retval =ifxusb_core_if_init_h(&_hcd->core_if,
		                             _irq,
		                             _iobase,
		                             _fifomem,
		                             _fifodbg);
		if(retval)
			return retval;

		ifxusb_host_core_init(&_hcd->core_if,&ifxusb_module_params_h);

		ifxusb_disable_global_interrupts_h( &_hcd->core_if);

		/* The driver is now initialized and need to be registered into Linux USB sub-system */

		retval = ifxhcd_init(_hcd); // hook the hcd into usb ss

		if (retval != 0)
		{
			IFX_ERROR("_hcd_init failed\n");
			return retval;
		}

		//ifxusb_enable_global_interrupts_h( _hcd->core_if ); // this should be done at hcd_start , including hcd_interrupt
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
		ifxusb_power_on_d  (&_pcd->core_if);
		mdelay(50);
		ifxusb_phy_power_on_d  (&_pcd->core_if); // Test
		mdelay(50);
		ifxusb_hard_reset_d(&_pcd->core_if);
		retval =ifxusb_core_if_init_d(&_pcd->core_if,
		                             _irq,
		                             _iobase,
		                             _fifomem,
		                             _fifodbg);
		if(retval)
			return retval;

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		ifxusb_dev_core_init(&_pcd->core_if,&ifxusb_module_params_d);

		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		ifxusb_disable_global_interrupts_d( &_pcd->core_if);

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
		//ifxusb_enable_global_interrupts_d( _pcd->core_if );  // this should be done at gadget bind or start
		return 0;
	}
#endif //__IS_DEVICE__

/*!
   \brief This function is called when a driver is unregistered. This happens when
  the rmmod command is executed. The device may or may not be electrically
  present. If it is present, the driver stops device processing. Any resources
  used on behalf of this device are freed.
*/
static int ifxusb_driver_remove(struct platform_device *_pdev)
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#ifdef __IS_HOST__
		#if defined(__IS_DUAL__)
			ifxhcd_remove(&ifxusb_hcd_1);
			ifxusb_core_if_remove_h(&ifxusb_hcd_1.core_if );
			ifxhcd_remove(&ifxusb_hcd_2);
			ifxusb_core_if_remove_h(&ifxusb_hcd_2.core_if );
		#else
			ifxhcd_remove(&ifxusb_hcd);
			ifxusb_core_if_remove_h(&ifxusb_hcd.core_if );
		#endif
	#endif
	#ifdef __IS_DEVICE__
		ifxpcd_remove();
		ifxusb_core_if_remove_d(&ifxusb_pcd.core_if );
	#endif
	/* Remove the device attributes */
	#ifdef __IS_HOST__
		ifxusb_attr_remove_h(&_pdev->dev);
	#else
		ifxusb_attr_remove_d(&_pdev->dev);
	#endif
	return 0;
}

/*!
   \brief This function is called by module management in 2.6 kernel or by ifxusb_driver_init with 2.4 kernel
  It is to probe and setup IFXUSB core(s).
*/
static int ifxusb_driver_probe(struct platform_device *_pdev)
{
	int retval = 0;
	struct device_node *np;
	int gpio_count;
	u32 port_mask = 0x1;

#ifdef __IS_DANUBE__
        np = of_find_compatible_node(NULL, NULL, "lantiq,ifxhcd-danube");
#elif defined __IS_AMAZON_SE__
        np = of_find_compatible_node(NULL, NULL, "lantiq,ifxhcd-ase");
#elif defined __IS_AR9__
        np = of_find_compatible_node(NULL, NULL, "lantiq,ifxhcd-arx100");
#elif defined __IS_VR9__
        np = of_find_compatible_node(NULL, NULL, "lantiq,ifxhcd-xrx200");
#endif
	if (!np) {
		dev_err(&_pdev->dev, "failed to find hcd device node\n");
		return -ENODEV;
	}
	of_property_read_u32(np, "lantiq,portmask", &port_mask);
	// Parsing and store the parameters
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	parse_parms();

	#ifdef __IS_HOST__
		#if defined(__DO_OC_INT__)
			if(!oc_int_id)
			{
				#if   defined(__IS_DUAL__)
					oc_int_id=&ifxusb_hcd_1;
					oc_int_id_1=&ifxusb_hcd_1;
					oc_int_id_2=&ifxusb_hcd_2;
				#else
					oc_int_id=&ifxusb_hcd;
				#endif
			}
		#endif

		#if   defined(__IS_DUAL__)
			memset(&ifxusb_hcd_1, 0, sizeof(ifxhcd_hcd_t));
			memset(&ifxusb_hcd_2, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd_1.core_if.core_no=0;
			ifxusb_hcd_2.core_if.core_no=1;
			ifxusb_hcd_1.core_if.core_name=(char *)ifxusb_hcd_name_1;
			ifxusb_hcd_2.core_if.core_name=(char *)ifxusb_hcd_name_2;

			ifxusb_hcd_1.dev=&_pdev->dev;
			ifxusb_hcd_2.dev=&_pdev->dev;

			if (port_mask & 0x1) {
				retval = ifxusb_driver_probe_h(&ifxusb_hcd_1,
			                               IFXUSB1_IRQ,
			                               IFXUSB1_IOMEM_BASE,
			                               IFXUSB1_FIFOMEM_BASE,
			                               IFXUSB1_FIFODBG_BASE
			                               );
				if(retval)
					goto ifxusb_driver_probe_fail;
			}

			if (port_mask & 0x2) {
				retval = ifxusb_driver_probe_h(&ifxusb_hcd_2,
			                               IFXUSB2_IRQ,
			                               IFXUSB2_IOMEM_BASE,
			                               IFXUSB2_FIFOMEM_BASE,
			                               IFXUSB2_FIFODBG_BASE
			                              );
				if(retval)
					goto ifxusb_driver_probe_fail;
			}
		#elif defined(__IS_FIRST__)
			memset(&ifxusb_hcd, 0, sizeof(ifxhcd_hcd_t));

			ifxusb_hcd.core_if.core_no=0;
			ifxusb_hcd.core_if.core_name=(char *)ifxusb_hcd_name;

			ifxusb_hcd.dev=&_pdev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFXUSB1_IRQ,
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

			ifxusb_hcd.dev=&_pdev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFXUSB2_IRQ,
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

			ifxusb_hcd.dev=&_pdev->dev;

			retval = ifxusb_driver_probe_h(&ifxusb_hcd,
			                               IFXUSB_IRQ,
			                               IFXUSB_IOMEM_BASE,
			                               IFXUSB_FIFOMEM_BASE,
			                               IFXUSB_FIFODBG_BASE
			                              );
			if(retval)
				goto ifxusb_driver_probe_fail;
		#endif
	#endif

	#ifdef __IS_DEVICE__
		memset(&ifxusb_pcd, 0, sizeof(ifxpcd_pcd_t));
		ifxusb_pcd.core_if.core_name=(char *)&ifxusb_pcd_name[0];

		ifxusb_pcd.dev=&_pdev->dev;

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

	#ifdef __IS_HOST__
		ifxusb_attr_create_h(&_pdev->dev);
	#else
		ifxusb_attr_create_d(&_pdev->dev);
	#endif

	gpio_count = of_gpio_count(np);
	while (gpio_count) {
		enum of_gpio_flags flags;
		int gpio = of_get_gpio_flags(np, --gpio_count, &flags);
		if (gpio_request(gpio, "usb"))
			continue;
		dev_info(&_pdev->dev, "requested GPIO %d\n", gpio);
		gpio_direction_output(gpio, (flags & OF_GPIO_ACTIVE_LOW) ? (0) : (1));
	}


	return 0;

ifxusb_driver_probe_fail:
	ifxusb_driver_remove(_pdev);
	return retval;
}

static struct resource ifxusb_device_resources[] =
{
	#if defined(__IS_DUAL__)
		[0] = {	.start  = IFXUSB1_IRQ,
				.flags  = IORESOURCE_IRQ,
		},
		[1] = {	.start  = IFXUSB1_IOMEM_BASE,
				.end    = IFXUSB1_IOMEM_BASE   + IFXUSB_IOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[2] = {	.start  = IFXUSB2_IRQ,
				.flags  = IORESOURCE_IRQ,
		},
		[3] = {	.start  = IFXUSB2_IOMEM_BASE,
				.end    = IFXUSB2_IOMEM_BASE + IFXUSB_IOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[4] = {	.start  = IFXUSB1_FIFOMEM_BASE,
				.end    = IFXUSB1_FIFOMEM_BASE + IFXUSB_FIFOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[5] = {	.start  = IFXUSB2_FIFOMEM_BASE,
				.end    = IFXUSB2_FIFOMEM_BASE + IFXUSB_FIFOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[6] = {	.start  = IFXUSB1_FIFODBG_BASE,
				.end    = IFXUSB1_FIFODBG_BASE + IFXUSB_FIFODBG_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[7] = {	.start  = IFXUSB2_FIFODBG_BASE,
				.end    = IFXUSB2_FIFODBG_BASE + IFXUSB_FIFODBG_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
	#elif defined(__IS_FIRST__)
		[0] = {	.start = IFXUSB1_IRQ,
				.flags	= IORESOURCE_IRQ,
		},
		[1] = {	.start  = IFXUSB1_IOMEM_BASE,
				.end    = IFXUSB1_IOMEM_BASE + IFXUSB_IOMEM_SIZE-1,
				.flags	= IORESOURCE_MEM,
		},
		[2] = {	.start  = IFXUSB1_FIFOMEM_BASE,
				.end    = IFXUSB1_FIFOMEM_BASE + IFXUSB_FIFOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[3] = {	.start  = IFXUSB1_FIFODBG_BASE,
				.end    = IFXUSB1_FIFODBG_BASE + IFXUSB_FIFODBG_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
	#elif defined(__IS_SECOND__)
		[0] = {	.start = IFXUSB2_IRQ,
				.flags	= IORESOURCE_IRQ,
		},
		[1] = {	.start  = IFXUSB2_IOMEM_BASE,
				.end    = IFXUSB2_IOMEM_BASE + IFXUSB_IOMEM_SIZE-1,
				.flags	= IORESOURCE_MEM,
		},
		[2] = {	.start  = IFXUSB2_FIFOMEM_BASE,
				.end    = IFXUSB2_FIFOMEM_BASE + IFXUSB_FIFOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[3] = { .start  = IFXUSB2_FIFODBG_BASE,
				.end    = IFXUSB2_FIFODBG_BASE + IFXUSB_FIFODBG_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
	#else
		[0] = {	.start = IFXUSB_IRQ,
				.flags	= IORESOURCE_IRQ,
		},
		[1] = {	.start  = IFXUSB_IOMEM_BASE,
				.end    = IFXUSB_IOMEM_BASE   + IFXUSB_IOMEM_SIZE-1,
				.flags	= IORESOURCE_MEM,
		},
		[2] = {	.start  = IFXUSB_FIFOMEM_BASE,
				.end    = IFXUSB_FIFOMEM_BASE+IFXUSB_FIFOMEM_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
		[3] = {	.start  = IFXUSB_FIFODBG_BASE,
				.end    = IFXUSB_FIFODBG_BASE+IFXUSB_FIFODBG_SIZE-1,
				.flags  = IORESOURCE_MEM,
		},
	#endif //__IS_DUAL__
};

static u64 ifxusb_dmamask = (u32)0x1fffffff;

static void ifxusb_device_release(struct device * dev)
{
	IFX_PRINT("IFX USB platform_dev release\n");
	dev->parent = NULL;
}

static struct platform_device ifxusb_device =
{
	.id			= -1,
	.dev =
	{
		.release       = ifxusb_device_release,
		.dma_mask      = &ifxusb_dmamask,
	},
	.resource		= ifxusb_device_resources,
	.num_resources		= ARRAY_SIZE(ifxusb_device_resources),
};


/*!
   \brief This function is called when the ifxusb_driver is installed with the insmod command.
*/
static struct platform_driver ifxusb_driver = {
	.probe		= ifxusb_driver_probe,
	.remove		= ifxusb_driver_remove,
	.driver ={
		.owner = THIS_MODULE,
		#ifdef __IS_HOST__
			.name = ifxusb_hcd_driver_name,
		#else
			.name = ifxusb_pcd_driver_name,
		#endif
	},
};

#ifdef __IS_HOST__
	int __init ifxusb_hcd_driver_init(void)
#else
	int __init ifxusb_pcd_driver_init(void)
#endif
{
	int retval = 0;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#if defined(__IS_HOST__)
		IFX_PRINT("%s: version %s\n", ifxusb_hcd_driver_name, IFXUSB_VERSION);
	#else
		IFX_PRINT("%s: version %s\n", ifxusb_pcd_driver_name, IFXUSB_VERSION);
	#endif

	#if 0
		#if   defined(__IS_TWINPASS__)
			IFX_PRINT("   OPTION: __IS_TWINPASS__\n");
		#elif defined(__IS_DANUBE__)
			IFX_PRINT("   OPTION: __IS_DANUBE__\n");
		#elif defined(__IS_AMAZON_SE__)
			IFX_PRINT("   OPTION: __IS_AMAZON_SE__\n");
		#elif defined(__IS_AR9__)
			IFX_PRINT("   OPTION: __IS_AR9__\n");
		#elif defined(__IS_VR9__)
			IFX_PRINT("   OPTION: __IS_VR9__\n");
		#elif defined(__IS_AR10__)
			IFX_PRINT("   OPTION: __IS_AR10__\n");
		#else
			IFX_PRINT("   OPTION: NO PLATFORM DEFINED\n");
		#endif

		#ifdef __UEIP__
			IFX_PRINT("   OPTION: __UEIP__\n");
		#endif

		#ifdef __PHY_LONG_PREEMP__
			IFX_PRINT("   OPTION: __PHY_LONG_PREEMP__\n");
		#endif
		#ifdef __FORCE_USB11__
			IFX_PRINT("   OPTION: __FORCE_USB11__\n");
		#endif
		#ifdef __UNALIGNED_BUF_ADJ__
			IFX_PRINT("   OPTION: __UNALIGNED_BUF_ADJ__\n");
		#endif
		#ifdef __UNALIGNED_BUF_CHK__
			IFX_PRINT("   OPTION: __UNALIGNED_BUF_CHK__\n");
		#endif
		#ifdef __UNALIGNED_BUF_BURST__
			IFX_PRINT("   OPTION: __UNALIGNED_BUF_BURST__\n");
		#endif
		#ifdef __DEBUG__
			IFX_PRINT("   OPTION: __DEBUG__\n");
		#endif
		#ifdef __ENABLE_DUMP__
			IFX_PRINT("   OPTION: __ENABLE_DUMP__\n");
		#endif

		#ifdef __IS_HOST__
			IFX_PRINT("   OPTION: __IS_HOST__\n");
			#ifdef __IS_DUAL__
				IFX_PRINT("           __IS_DUAL__\n");
			#endif
			#ifdef __IS_FIRST__
				IFX_PRINT("           __IS_FIRST__\n");
			#endif
			#ifdef __IS_SECOND__
				IFX_PRINT("           __IS_SECOND__\n");
			#endif
			#ifdef __WITH_HS_ELECT_TST__
				IFX_PRINT("           __WITH_HS_ELECT_TST__\n");
			#endif
			#ifdef __EN_ISOC__
				IFX_PRINT("           __EN_ISOC__\n");
			#endif
			#ifdef __EN_ISOC_SPLIT__
				IFX_PRINT("           __EN_ISOC_SPLIT__\n");
			#endif
			#ifdef __EPQD_DESTROY_TIMEOUT__
				IFX_PRINT("           __EPQD_DESTROY_TIMEOUT__\n");
			#endif
			#ifdef __DYN_SOF_INTR__
				IFX_PRINT("           __DYN_SOF_INTR__\n");
			#endif
		#else
			IFX_PRINT("   OPTION: __IS_DEVICE__\n");
			#ifdef __DED_INTR__
				IFX_PRINT("           __DED_INTR__\n");
			#endif
			#ifdef __DED_FIFO__
				IFX_PRINT("           __DED_FIFO__\n");
			#endif
			#ifdef __DESC_DMA__
				IFX_PRINT("           __DESC_DMA__\n");
			#endif
			#ifdef __IS_FIRST__
				IFX_PRINT("           __IS_FIRST__\n");
			#endif
			#ifdef __IS_SECOND__
				IFX_PRINT("           __IS_SECOND__\n");
			#endif
			#ifdef __GADGET_TASKLET_TX__
				IFX_PRINT("           __GADGET_TASKLET_TX__\n");
			#endif
			#ifdef __GADGET_TASKLET_RX__
				IFX_PRINT("           __GADGET_TASKLET_RX__\n");
			#endif
			#ifdef __GADGET_TASKLET_HIGH__
				IFX_PRINT("           __GADGET_TASKLET_HIGH__\n");
			#endif
			#ifdef __DO_PCD_UNLOCK__
				IFX_PRINT("           __DO_PCD_UNLOCK__\n");
			#endif
			#ifdef __GADGET_LED__
				IFX_PRINT("           __GADGET_LED__\n");
			#endif
			
			#ifdef __ECM_NO_INTR__
				IFX_PRINT("           __ECM_NO_INTR__\n");
			#endif
			#ifdef __NOSWAPINCTRL__
				IFX_PRINT("           __NOSWAPINCTRL__\n");
			#endif
			#ifdef __MAC_ECM_FIX__
				IFX_PRINT("           __MAC_ECM_FIX__\n");
			#endif
			#ifdef __RETAIN_BUF_TX__
				IFX_PRINT("           __RETAIN_BUF_TX__\n");
			#endif
			#ifdef __RETAIN_BUF_RX__
				IFX_PRINT("           __RETAIN_BUF_RX__\n");
			#endif
			#ifdef __QUICKNAK__
				IFX_PRINT("           __QUICKNAK__\n");
			#endif
		#endif
	#endif

	retval = platform_driver_register(&ifxusb_driver);

	if (retval < 0) {
		IFX_ERROR("%s retval=%d\n", __func__, retval);
		return retval;
	}

	#ifdef __IS_HOST__
		ifxusb_device.name = ifxusb_hcd_driver_name;
	#else
		ifxusb_device.name = ifxusb_pcd_driver_name;
	#endif

	if (ifxusb_device.dev.parent)
		retval = -EBUSY;
	else
		retval = platform_device_register(&ifxusb_device);

	if (retval < 0)
	{
		IFX_ERROR("%s retval=%d\n", __func__, retval);
		platform_driver_unregister(&ifxusb_driver);
		return retval;
	}
	return retval;
}

#ifdef __IS_HOST__
	module_init(ifxusb_hcd_driver_init);
#else
	module_init(ifxusb_pcd_driver_init);
#endif

/*!
   \brief This function is called when the driver is removed from the kernel
  with the rmmod command. The driver unregisters itself with its bus
  driver.
*/
#ifdef __IS_HOST__
	void __exit ifxusb_hcd_driver_cleanup(void)
	{
		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		platform_device_unregister(&ifxusb_device);
		platform_driver_unregister(&ifxusb_driver);
		IFX_PRINT("%s module removed\n", ifxusb_hcd_driver_name);
	}
	module_exit(ifxusb_hcd_driver_cleanup);
#else
	void __exit ifxusb_pcd_driver_cleanup(void)
	{
		IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
		platform_device_unregister(&ifxusb_device);
		platform_driver_unregister(&ifxusb_driver);
		IFX_PRINT("%s module removed\n", ifxusb_pcd_driver_name);
	}
	module_exit(ifxusb_pcd_driver_cleanup);
#endif
MODULE_DESCRIPTION(USB_DRIVER_DESC);
MODULE_AUTHOR("Lantiq");
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

	ifxusb_params_t *params;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#ifdef __IS_HOST__
		h_dbg_lvl=dbg_lvl;
		params=&ifxusb_module_params_h;
	#endif
	#ifdef __IS_DEVICE__
		d_dbg_lvl=dbg_lvl;
		params=&ifxusb_module_params_d;
	#endif

	switch(dma_burst_size)
	{
		case 0:
		case 1:
		case 4:
		case 8:
		case 16:
			params->dma_burst_size=dma_burst_size;
			break;
		default:
			#if defined(__IS_VR9__)
			{
				unsigned int chipid;
				unsigned int partnum;
				chipid=*((volatile uint32_t *)IFX_MPS_CHIPID);
				partnum=(chipid&0x0FFFF000)>>12;
				switch(partnum)
				{
					case 0x000B: //VRX288_A2x
					case 0x000E: //VRX282_A2x
					case 0x000C: //VRX268_A2x
					case 0x000D: //GRX288_A2x
						params->dma_burst_size=default_param_dma_burst_size_n;
						break;
					default:
						params->dma_burst_size=default_param_dma_burst_size;
				}
				printk(KERN_INFO "Chip Version :%04x BurstSize=%d\n",partnum,params->dma_burst_size);
			}
			#else
				params->dma_burst_size=default_param_dma_burst_size;
			#endif
	}

	if(speed==0 || speed==1)
		params->speed=speed;
	else
		params->speed=default_param_speed;

	if(max_transfer_size>=2048 && max_transfer_size<=65535)
		params->max_transfer_size=max_transfer_size;
	else
		params->max_transfer_size=default_param_max_transfer_size;

	if(max_packet_count>=15 && max_packet_count<=511)
		params->max_packet_count=max_packet_count;
	else
		params->max_packet_count=default_param_max_packet_count;

	switch(phy_utmi_width)
	{
		case 8:
		case 16:
			params->phy_utmi_width=phy_utmi_width;
			break;
		default:
			params->phy_utmi_width=default_param_phy_utmi_width;
	}

	if(turn_around_time_hs>=0 && turn_around_time_hs<=7)
		params->turn_around_time_hs=turn_around_time_hs;
	else
		params->turn_around_time_hs=default_param_turn_around_time_hs;

	if(turn_around_time_fs>=0 && turn_around_time_fs<=7)
		params->turn_around_time_fs=turn_around_time_fs;
	else
		params->turn_around_time_fs=default_param_turn_around_time_fs;

	if(timeout_cal_hs>=0 && timeout_cal_hs<=7)
		params->timeout_cal_hs=timeout_cal_hs;
	else
		params->timeout_cal_hs=default_param_timeout_cal_hs;

	if(timeout_cal_fs>=0 && timeout_cal_fs<=7)
		params->timeout_cal_fs=timeout_cal_fs;
	else
		params->timeout_cal_fs=default_param_timeout_cal_fs;

	if(data_fifo_size>=32 && data_fifo_size<=32768)
		params->data_fifo_size=data_fifo_size;
	else
		params->data_fifo_size=default_param_data_fifo_size;

	#ifdef __IS_HOST__
		if(host_channels>=1 && host_channels<=16)
			params->host_channels=host_channels;
		else
			params->host_channels=default_param_host_channels;

		if(rx_fifo_size>=16 && rx_fifo_size<=32768)
			params->rx_fifo_size=rx_fifo_size;
		else
			params->rx_fifo_size=default_param_rx_fifo_size;

		if(nperio_tx_fifo_size>=16 && nperio_tx_fifo_size<=32768)
			params->nperio_tx_fifo_size=nperio_tx_fifo_size;
		else
			params->nperio_tx_fifo_size=default_param_nperio_tx_fifo_size;

		if(perio_tx_fifo_size>=16 && perio_tx_fifo_size<=32768)
			params->perio_tx_fifo_size=perio_tx_fifo_size;
		else
			params->perio_tx_fifo_size=default_param_perio_tx_fifo_size;
	#endif //__IS_HOST__

	#ifdef __IS_DEVICE__
		if(rx_fifo_size>=16 && rx_fifo_size<=32768)
			params->rx_fifo_size=rx_fifo_size;
		else
			params->rx_fifo_size=default_param_rx_fifo_size;
		#ifdef __DED_FIFO__
			if(tx_fifo_size_00>=16 && tx_fifo_size_00<=32768)
				params->tx_fifo_size[ 0]=tx_fifo_size_00;
			else
				params->tx_fifo_size[ 0]=default_param_tx_fifo_size_00;
			if(tx_fifo_size_01>=0 && tx_fifo_size_01<=32768)
				params->tx_fifo_size[ 1]=tx_fifo_size_01;
			else
				params->tx_fifo_size[ 1]=default_param_tx_fifo_size_01;
			if(tx_fifo_size_02>=0 && tx_fifo_size_02<=32768)
				params->tx_fifo_size[ 2]=tx_fifo_size_02;
			else
				params->tx_fifo_size[ 2]=default_param_tx_fifo_size_02;
			if(tx_fifo_size_03>=0 && tx_fifo_size_03<=32768)
				params->tx_fifo_size[ 3]=tx_fifo_size_03;
			else
				params->tx_fifo_size[ 3]=default_param_tx_fifo_size_03;
			if(tx_fifo_size_04>=0 && tx_fifo_size_04<=32768)
				params->tx_fifo_size[ 4]=tx_fifo_size_04;
			else
				params->tx_fifo_size[ 4]=default_param_tx_fifo_size_04;
			if(tx_fifo_size_05>=0 && tx_fifo_size_05<=32768)
				params->tx_fifo_size[ 5]=tx_fifo_size_05;
			else
				params->tx_fifo_size[ 5]=default_param_tx_fifo_size_05;
			if(tx_fifo_size_06>=0 && tx_fifo_size_06<=32768)
				params->tx_fifo_size[ 6]=tx_fifo_size_06;
			else
				params->tx_fifo_size[ 6]=default_param_tx_fifo_size_06;
			if(tx_fifo_size_07>=0 && tx_fifo_size_07<=32768)
				params->tx_fifo_size[ 7]=tx_fifo_size_07;
			else
				params->tx_fifo_size[ 7]=default_param_tx_fifo_size_07;
			if(tx_fifo_size_08>=0 && tx_fifo_size_08<=32768)
				params->tx_fifo_size[ 8]=tx_fifo_size_08;
			else
				params->tx_fifo_size[ 8]=default_param_tx_fifo_size_08;
			if(tx_fifo_size_09>=0 && tx_fifo_size_09<=32768)
				params->tx_fifo_size[ 9]=tx_fifo_size_09;
			else
				params->tx_fifo_size[ 9]=default_param_tx_fifo_size_09;
			if(tx_fifo_size_10>=0 && tx_fifo_size_10<=32768)
				params->tx_fifo_size[10]=tx_fifo_size_10;
			else
				params->tx_fifo_size[10]=default_param_tx_fifo_size_10;
			if(tx_fifo_size_11>=0 && tx_fifo_size_11<=32768)
				params->tx_fifo_size[11]=tx_fifo_size_11;
			else
				params->tx_fifo_size[11]=default_param_tx_fifo_size_11;
			if(tx_fifo_size_12>=0 && tx_fifo_size_12<=32768)
				params->tx_fifo_size[12]=tx_fifo_size_12;
			else
				params->tx_fifo_size[12]=default_param_tx_fifo_size_12;
			if(tx_fifo_size_13>=0 && tx_fifo_size_13<=32768)
				params->tx_fifo_size[13]=tx_fifo_size_13;
			else
				params->tx_fifo_size[13]=default_param_tx_fifo_size_13;
			if(tx_fifo_size_14>=0 && tx_fifo_size_14<=32768)
				params->tx_fifo_size[14]=tx_fifo_size_14;
			else
				params->tx_fifo_size[14]=default_param_tx_fifo_size_14;
			if(tx_fifo_size_15>=0 && tx_fifo_size_15<=32768)
				params->tx_fifo_size[15]=tx_fifo_size_15;
			else
				params->tx_fifo_size[15]=default_param_tx_fifo_size_15;
			if(thr_ctl==0 || thr_ctl==1)
				params->thr_ctl=thr_ctl;
			else
				params->thr_ctl=default_param_thr_ctl;
			if(tx_thr_length>=16 && tx_thr_length<=511)
				params->tx_thr_length=tx_thr_length;
			else
				params->tx_thr_length=default_param_tx_thr_length;
			if(rx_thr_length>=16 && rx_thr_length<=511)
				params->rx_thr_length=rx_thr_length;
			else
				params->rx_thr_length=default_param_rx_thr_length;
		#else  //__DED_FIFO__
			if(nperio_tx_fifo_size>=16 && nperio_tx_fifo_size<=32768)
				params->tx_fifo_size[ 0]=nperio_tx_fifo_size;
			else
				params->tx_fifo_size[ 0]=default_param_nperio_tx_fifo_size;
			if(perio_tx_fifo_size_01>=0 && perio_tx_fifo_size_01<=32768)
				params->tx_fifo_size[ 1]=perio_tx_fifo_size_01;
			else
				params->tx_fifo_size[ 1]=default_param_perio_tx_fifo_size_01;
			if(perio_tx_fifo_size_02>=0 && perio_tx_fifo_size_02<=32768)
				params->tx_fifo_size[ 2]=perio_tx_fifo_size_02;
			else
				params->tx_fifo_size[ 2]=default_param_perio_tx_fifo_size_02;
			if(perio_tx_fifo_size_03>=0 && perio_tx_fifo_size_03<=32768)
				params->tx_fifo_size[ 3]=perio_tx_fifo_size_03;
			else
				params->tx_fifo_size[ 3]=default_param_perio_tx_fifo_size_03;
			if(perio_tx_fifo_size_04>=0 && perio_tx_fifo_size_04<=32768)
				params->tx_fifo_size[ 4]=perio_tx_fifo_size_04;
			else
				params->tx_fifo_size[ 4]=default_param_perio_tx_fifo_size_04;
			if(perio_tx_fifo_size_05>=0 && perio_tx_fifo_size_05<=32768)
				params->tx_fifo_size[ 5]=perio_tx_fifo_size_05;
			else
				params->tx_fifo_size[ 5]=default_param_perio_tx_fifo_size_05;
			if(perio_tx_fifo_size_06>=0 && perio_tx_fifo_size_06<=32768)
				params->tx_fifo_size[ 6]=perio_tx_fifo_size_06;
			else
				params->tx_fifo_size[ 6]=default_param_perio_tx_fifo_size_06;
			if(perio_tx_fifo_size_07>=0 && perio_tx_fifo_size_07<=32768)
				params->tx_fifo_size[ 7]=perio_tx_fifo_size_07;
			else
				params->tx_fifo_size[ 7]=default_param_perio_tx_fifo_size_07;
			if(perio_tx_fifo_size_08>=0 && perio_tx_fifo_size_08<=32768)
				params->tx_fifo_size[ 8]=perio_tx_fifo_size_08;
			else
				params->tx_fifo_size[ 8]=default_param_perio_tx_fifo_size_08;
			if(perio_tx_fifo_size_09>=0 && perio_tx_fifo_size_09<=32768)
				params->tx_fifo_size[ 9]=perio_tx_fifo_size_09;
			else
				params->tx_fifo_size[ 9]=default_param_perio_tx_fifo_size_09;
			if(perio_tx_fifo_size_10>=0 && perio_tx_fifo_size_10<=32768)
				params->tx_fifo_size[10]=perio_tx_fifo_size_10;
			else
				params->tx_fifo_size[10]=default_param_perio_tx_fifo_size_10;
			if(perio_tx_fifo_size_11>=0 && perio_tx_fifo_size_11<=32768)
				params->tx_fifo_size[11]=perio_tx_fifo_size_11;
			else
				params->tx_fifo_size[11]=default_param_perio_tx_fifo_size_11;
			if(perio_tx_fifo_size_12>=0 && perio_tx_fifo_size_12<=32768)
				params->tx_fifo_size[12]=perio_tx_fifo_size_12;
			else
				params->tx_fifo_size[12]=default_param_perio_tx_fifo_size_12;
			if(perio_tx_fifo_size_13>=0 && perio_tx_fifo_size_13<=32768)
				params->tx_fifo_size[13]=perio_tx_fifo_size_13;
			else
				params->tx_fifo_size[13]=default_param_perio_tx_fifo_size_13;
			if(perio_tx_fifo_size_14>=0 && perio_tx_fifo_size_14<=32768)
				params->tx_fifo_size[14]=perio_tx_fifo_size_14;
			else
				params->tx_fifo_size[14]=default_param_perio_tx_fifo_size_14;
			if(perio_tx_fifo_size_15>=0 && perio_tx_fifo_size_15<=32768)
				params->tx_fifo_size[15]=perio_tx_fifo_size_15;
			else
				params->tx_fifo_size[15]=default_param_perio_tx_fifo_size_15;
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

