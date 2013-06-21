/*****************************************************************************
 **   FILE NAME       : ifxusb_plat.h
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the Platform Specific constants, interfaces
 **                     (functions and macros).
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
  \defgroup IFXUSB_PLATEFORM_DEFINITION Platform Specific constants, interfaces (functions and macros).
  \ingroup IFXUSB_DRIVER_V3
  \brief Maintain plateform specific definitions and macros in this file.
         Each plateform has its own definition zone.
 */

/*!
  \defgroup IFXUSB_PLATEFORM_MEM_ADDR Definition of memory address and size and default parameters
  \ingroup IFXUSB_PLATEFORM_DEFINITION
 */

/*!
  \defgroup IFXUSB_DBG_ROUTINE Routines for debug message
  \ingroup IFXUSB_PLATEFORM_DEFINITION
 */


/*! \file ifxusb_plat.h
    \ingroup IFXUSB_DRIVER_V3
    \brief This file contains the Platform Specific constants, interfaces (functions and macros).
*/

#if !defined(__IFXUSB_PLAT_H__)
#define __IFXUSB_PLAT_H__


#include <linux/types.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <asm/io.h>


#define IFXUSB_IOMEM_SIZE   0x00001000
#define IFXUSB_FIFOMEM_SIZE 0x00010000
#define IFXUSB_FIFODBG_SIZE 0x00020000



/*!
  \addtogroup IFXUSB_PLATEFORM_MEM_ADDR
 */
/*@{*/
#if defined(__UEIP__)
	#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#define IFXUSB_IRQ          62
		#define IFXUSB_IOMEM_BASE   0x1e101000
		#define IFXUSB_FIFOMEM_BASE 0x1e120000
		#define IFXUSB_FIFODBG_BASE 0x1e140000
		#define IFXUSB_OC_IRQ       159

		#ifndef DANUBE_RCU_BASE_ADDR
			#define DANUBE_RCU_BASE_ADDR            (0xBF203000)
		#endif

		#ifndef DANUBE_CGU
			#define DANUBE_CGU                      (0xBF103000)
		#endif
		#ifndef DANUBE_CGU_IFCCR
			#define DANUBE_CGU_IFCCR                ((volatile unsigned long *)(DANUBE_CGU+ 0x0018))
		#endif
		#ifndef DANUBE_PMU
			#define DANUBE_PMU                      (KSEG1+0x1F102000)
		#endif
		#ifndef DANUBE_PMU_PWDCR
			#define DANUBE_PMU_PWDCR                ((volatile unsigned long *)(DANUBE_PMU+0x001C))
		#endif

		#ifndef DANUBE_GPIO_P0_OUT
			#define DANUBE_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define DANUBE_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define DANUBE_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define DANUBE_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define DANUBE_GPIO_P0_OD                       (0xBF103000+0x24)
			#define DANUBE_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define DANUBE_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define DANUBE_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define DANUBE_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define DANUBE_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define DANUBE_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define DANUBE_GPIO_P1_OD                       (0xBF103000+0x54)
			#define DANUBE_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define DANUBE_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define DANUBE_RCU_USBCFG  ((volatile unsigned long *)(DANUBE_RCU_BASE_ADDR + 0x18))
		#define DANUBE_RCU_RESET   ((volatile unsigned long *)(DANUBE_RCU_BASE_ADDR + 0x10))
		#define DANUBE_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define DANUBE_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define DANUBE_USBCFG_SLV_END_BIT  9	// 0:little_end, 1:big_end

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4
		#define default_param_turn_around_time_fs 4
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        640
			#define default_param_nperio_tx_fifo_size 640
			#define default_param_perio_tx_fifo_size  768
		#endif //__IS_HOST__

		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
//				#define default_param_rx_fifo_size          1024
//				#define default_param_nperio_tx_fifo_size   1016
//				#define default_param_perio_tx_fifo_size_01 8
				#define default_param_rx_fifo_size          1008
				#define default_param_nperio_tx_fifo_size   1008
				#define default_param_perio_tx_fifo_size_01 32
			#else
				#define default_param_rx_fifo_size          1024
				#define default_param_nperio_tx_fifo_size   1024
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_AMAZON_SE__)
		//#include <asm/amazon_se/amazon_se.h>
		//#include <asm/amazon_se/irq.h>

		#define IFXUSB_IRQ          39
		#define IFXUSB_IOMEM_BASE   0x1e101000
		#define IFXUSB_FIFOMEM_BASE 0x1e120000
		#define IFXUSB_FIFODBG_BASE 0x1e140000
		#define IFXUSB_OC_IRQ       20

		#ifndef AMAZON_SE_RCU_BASE_ADDR
			#define AMAZON_SE_RCU_BASE_ADDR            (0xBF203000)
		#endif
		#define AMAZON_SE_RCU_USBCFG  ((volatile unsigned long *)(AMAZON_SE_RCU_BASE_ADDR + 0x18))
		#define AMAZON_SE_RCU_RESET   ((volatile unsigned long *)(AMAZON_SE_RCU_BASE_ADDR + 0x10))
		#define AMAZON_SE_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AMAZON_SE_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AMAZON_SE_USBCFG_SLV_END_BIT  9 	// 0:little_end, 1:big_end

		#ifndef AMAZON_SE_GPIO_P0_OUT
			#define AMAZON_SE_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AMAZON_SE_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AMAZON_SE_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AMAZON_SE_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AMAZON_SE_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AMAZON_SE_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AMAZON_SE_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AMAZON_SE_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AMAZON_SE_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AMAZON_SE_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AMAZON_SE_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AMAZON_SE_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AMAZON_SE_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AMAZON_SE_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#ifndef AMAZON_SE_CGU
			#define AMAZON_SE_CGU                      (0xBF103000)
		#endif
		#ifndef AMAZON_SE_CGU_IFCCR
			#define AMAZON_SE_CGU_IFCCR                ((volatile unsigned long *)(AMAZON_SE_CGU+ 0x0018))
		#endif
		#ifndef AMAZON_SE_PMU
			#define AMAZON_SE_PMU                      (KSEG1+0x1F102000)
		#endif
		#ifndef AMAZON_SE_PMU_PWDCR
			#define AMAZON_SE_PMU_PWDCR                ((volatile unsigned long *)(AMAZON_SE_PMU+0x001C))
		#endif

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4 //(NoChange)
		#define default_param_turn_around_time_fs 4 //(NoChange)
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
//				#define default_param_rx_fifo_size          256
//				#define default_param_nperio_tx_fifo_size   248
//				#define default_param_perio_tx_fifo_size_01 8
				#define default_param_rx_fifo_size          240
				#define default_param_nperio_tx_fifo_size   240
				#define default_param_perio_tx_fifo_size_01 32
			#else
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   256
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_AR9__)
		#define IFXUSB1_IRQ 62
		#define IFXUSB1_IOMEM_BASE   0x1E101000
		#define IFXUSB1_FIFOMEM_BASE 0x1E120000
		#define IFXUSB1_FIFODBG_BASE 0x1E140000

		#define IFXUSB2_IRQ 91
		#define IFXUSB2_IOMEM_BASE   0x1E106000
		#define IFXUSB2_FIFOMEM_BASE 0x1E1E0000
		#define IFXUSB2_FIFODBG_BASE 0x1E1C0000

		#define IFXUSB_OC_IRQ 68

		#ifndef AR9_RCU_BASE_ADDR
			#define AR9_RCU_BASE_ADDR                (0xBF203000)
		#endif

		#ifndef AR9_CGU
			#define AR9_CGU                          (0xBF103000)
		#endif
		#ifndef AR9_CGU_IFCCR
			#define AR9_CGU_IFCCR                        ((volatile unsigned long *)(AR9_CGU+ 0x0018))
		#endif

		#ifndef AR9_PMU
			#define AR9_PMU                              (KSEG1+0x1F102000)
		#endif
		#ifndef AR9_PMU_PWDCR
			#define AR9_PMU_PWDCR                        ((volatile unsigned long *)(AR9_PMU+0x001C))
		#endif

		#ifndef AR9_GPIO_P0_OUT
			#define AR9_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AR9_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AR9_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AR9_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AR9_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AR9_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AR9_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AR9_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AR9_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AR9_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AR9_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AR9_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AR9_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AR9_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define AR9_RCU_USB1CFG  ((volatile unsigned long *)(AR9_RCU_BASE_ADDR + 0x18))
		#define AR9_RCU_USB2CFG  ((volatile unsigned long *)(AR9_RCU_BASE_ADDR + 0x34))
		#define AR9_RCU_USBRESET ((volatile unsigned long *)(AR9_RCU_BASE_ADDR + 0x10))
		#define AR9_USBCFG_ARB          7	//
		#define AR9_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AR9_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AR9_USBCFG_SLV_END_BIT  17	// 0:little_end, 1:big_end

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4 //(NoChange)
		#define default_param_turn_around_time_fs 4 //(NoChange)
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
//				#define default_param_rx_fifo_size          256
//				#define default_param_nperio_tx_fifo_size   248
//				#define default_param_perio_tx_fifo_size_01 8
				#define default_param_rx_fifo_size          240
				#define default_param_nperio_tx_fifo_size   240
				#define default_param_perio_tx_fifo_size_01 32
			#else
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   256
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_VR9__)
		#define IFXUSB1_IRQ 62
		#define IFXUSB1_IOMEM_BASE   0x1E101000
		#define IFXUSB1_FIFOMEM_BASE 0x1E120000
		#define IFXUSB1_FIFODBG_BASE 0x1E140000

		#define IFXUSB2_IRQ 91
		#define IFXUSB2_IOMEM_BASE   0x1E106000
		#define IFXUSB2_FIFOMEM_BASE 0x1E1E0000
		#define IFXUSB2_FIFODBG_BASE 0x1E1C0000
		#define IFXUSB_OC_IRQ 60
		
		#ifndef IFX_MPS
			#define IFX_MPS								(KSEG1+0x1F107000)
		#endif
		#ifndef IFX_MPS_CHIPID
			#define IFX_MPS_CHIPID						((volatile unsigned long *)(IFX_MPS + 0x0344))
		#endif

		#ifndef VR9_RCU_BASE_ADDR
			#define VR9_RCU_BASE_ADDR            (0xBF203000)
		#endif

		#ifndef VR9_CGU
			#define VR9_CGU                          (0xBF103000)
		#endif
		#ifndef VR9_CGU_IFCCR
			#define VR9_CGU_IFCCR                        ((volatile unsigned long *)(VR9_CGU+ 0x0018))
		#endif

		#ifndef VR9_PMU
			#define VR9_PMU                              (KSEG1+0x1F102000)
		#endif
		#ifndef VR9_PMU_PWDCR
			#define VR9_PMU_PWDCR                        ((volatile unsigned long *)(VR9_PMU+0x001C))
		#endif

		#ifndef VR9_GPIO_P0_OUT
			#define VR9_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define VR9_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define VR9_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define VR9_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define VR9_GPIO_P0_OD                       (0xBF103000+0x24)
			#define VR9_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define VR9_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define VR9_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define VR9_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define VR9_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define VR9_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define VR9_GPIO_P1_OD                       (0xBF103000+0x54)
			#define VR9_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define VR9_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define VR9_RCU_USB1CFG   ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x18))
		#define VR9_RCU_USB2CFG   ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x34))
		#define VR9_RCU_USB_ANA_CFG1A  ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x38))
		#define VR9_RCU_USB_ANA_CFG1B  ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x3C))
		#define VR9_RCU_USBRESET  ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x10))
		#define VR9_RCU_USBRESET2 ((volatile unsigned long *)(VR9_RCU_BASE_ADDR + 0x48))
		#define VR9_USBCFG_ARB          7	//
		#define VR9_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define VR9_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define VR9_USBCFG_SLV_END_BIT  9	// 0:little_end, 1:big_end

//		#define default_param_dma_burst_size 4      //(ALL)
		//WA for AHB
		#define default_param_dma_burst_size   0      //(ALL)
		#define default_param_dma_burst_size_n 4      //(ALL)

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size -1  //(Max, hwcfg)
		#define default_param_max_packet_count  -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width    16

		#define default_param_turn_around_time_hs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_turn_around_time_fs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
#if 0
			#define default_param_rx_fifo_size    256
			#define default_param_tx_fifo_size_00 -1
			#define default_param_tx_fifo_size_01 -1
			#define default_param_tx_fifo_size_02 -1
#else
			#define default_param_rx_fifo_size    256
			#define default_param_tx_fifo_size_00 32
			#define default_param_tx_fifo_size_01 200
			#define default_param_tx_fifo_size_02 8
#endif
			#define default_param_tx_fifo_size_03 -1
			#define default_param_tx_fifo_size_04 -1
			#define default_param_tx_fifo_size_05 -1
			#define default_param_tx_fifo_size_06 -1
			#define default_param_tx_fifo_size_07 -1
			#define default_param_tx_fifo_size_08 -1
			#define default_param_tx_fifo_size_09 -1
			#define default_param_tx_fifo_size_10 -1
			#define default_param_tx_fifo_size_11 -1
			#define default_param_tx_fifo_size_12 -1
			#define default_param_tx_fifo_size_13 -1
			#define default_param_tx_fifo_size_14 -1
			#define default_param_tx_fifo_size_15 -1
			#define default_param_dma_unalgned_tx -1
			#define default_param_dma_unalgned_rx -1
			#define default_param_thr_ctl         -1
			#define default_param_tx_thr_length   -1
			#define default_param_rx_thr_length   -1
		#endif //__IS_DEVICE__

	#elif defined(__IS_AR10__)
		#define IFXUSB1_IRQ 54
		#define IFXUSB1_IOMEM_BASE   0x1E101000
		#define IFXUSB1_FIFOMEM_BASE 0x1E120000
		#define IFXUSB1_FIFODBG_BASE 0x1E140000
		#define IFXUSB1_OC_IRQ 60

		#define IFXUSB2_IRQ 83
		#define IFXUSB2_IOMEM_BASE   0x1E106000
		#define IFXUSB2_FIFOMEM_BASE 0x1E1E0000
		#define IFXUSB2_FIFODBG_BASE 0x1E1C0000
		#define IFXUSB2_OC_IRQ 56

		#ifndef AR10_RCU_BASE_ADDR
			#define AR10_RCU_BASE_ADDR            (0xBF203000)
		#endif
		#ifndef AR10_CGU
			#define AR10_CGU                      (0xBF103000)
		#endif

		#ifndef AR10_CGU_IFCCR
			#define AR10_CGU_IFCCR                      ((volatile unsigned long *)(AR10_CGU+ 0x0018))
		#endif
		#ifndef AR10_PMU
			#define AR10_PMU                              (KSEG1+0x1F102000)
		#endif
		#ifndef AR10_PMU_PWDCR
			#define AR10_PMU_PWDCR                        ((volatile unsigned long *)(AR10_PMU+0x0044))
		#endif

		#ifndef AR10_GPIO_P0_OUT
			#define AR10_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AR10_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AR10_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AR10_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AR10_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AR10_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AR10_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AR10_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AR10_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AR10_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AR10_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AR10_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AR10_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AR10_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define AR10_RCU_USB1CFG   ((volatile unsigned long *)(AR10_RCU_BASE_ADDR + 0x18))
		#define AR10_RCU_USB2CFG   ((volatile unsigned long *)(AR10_RCU_BASE_ADDR + 0x34))
		#define AR10_RCU_USB_ANA_CFG1A  ((volatile unsigned long *)(AR10_RCU_BASE_ADDR + 0x38))
		#define AR10_RCU_USB_ANA_CFG1B  ((volatile unsigned long *)(AR10_RCU_BASE_ADDR + 0x3C))

		#define AR10_RCU_USBRESET  ((volatile unsigned long *)(AR10_RCU_BASE_ADDR + 0x10))

		#define AR10_USBCFG_ARB          7	//
		#define AR10_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AR10_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AR10_USBCFG_SLV_END_BIT  9	// 0:little_end, 1:big_end

//		#define default_param_dma_burst_size 4      //(ALL)
		//WA for AHB
		#define default_param_dma_burst_size 0      //(ALL)

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size -1  //(Max, hwcfg)
		#define default_param_max_packet_count  -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width    16

		#define default_param_turn_around_time_hs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_turn_around_time_fs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
#if 0
			#define default_param_rx_fifo_size    256
			#define default_param_tx_fifo_size_00 -1
			#define default_param_tx_fifo_size_01 -1
			#define default_param_tx_fifo_size_02 -1
#else
			#define default_param_rx_fifo_size    256
			#define default_param_tx_fifo_size_00 32
			#define default_param_tx_fifo_size_01 200
			#define default_param_tx_fifo_size_02 8
#endif
			#define default_param_tx_fifo_size_03 -1
			#define default_param_tx_fifo_size_04 -1
			#define default_param_tx_fifo_size_05 -1
			#define default_param_tx_fifo_size_06 -1
			#define default_param_tx_fifo_size_07 -1
			#define default_param_tx_fifo_size_08 -1
			#define default_param_tx_fifo_size_09 -1
			#define default_param_tx_fifo_size_10 -1
			#define default_param_tx_fifo_size_11 -1
			#define default_param_tx_fifo_size_12 -1
			#define default_param_tx_fifo_size_13 -1
			#define default_param_tx_fifo_size_14 -1
			#define default_param_tx_fifo_size_15 -1
			#define default_param_dma_unalgned_tx -1
			#define default_param_dma_unalgned_rx -1
			#define default_param_thr_ctl         -1
			#define default_param_tx_thr_length   -1
			#define default_param_rx_thr_length   -1
		#endif //__IS_DEVICE__
	#else // __IS_AR10__
		#error "Please choose one platform!!"
	#endif // __IS_VR9__

#else //UEIP
	#if defined(__IS_TWINPASS__) || defined(__IS_DANUBE__)
		#define IFXUSB_IRQ          54
		#define IFXUSB_IOMEM_BASE   0x1e101000
		#define IFXUSB_FIFOMEM_BASE 0x1e120000
		#define IFXUSB_FIFODBG_BASE 0x1e140000
		#define IFXUSB_OC_IRQ       151


		#ifndef DANUBE_RCU_BASE_ADDR
			#define DANUBE_RCU_BASE_ADDR            (0xBF203000)
		#endif

		#ifndef DANUBE_CGU
			#define DANUBE_CGU                      (0xBF103000)
		#endif
		#ifndef DANUBE_CGU_IFCCR
			#define DANUBE_CGU_IFCCR                ((volatile unsigned long *)(DANUBE_CGU+ 0x0018))
		#endif
		#ifndef DANUBE_PMU
			#define DANUBE_PMU                      (KSEG1+0x1F102000)
		#endif
		#ifndef DANUBE_PMU_PWDCR
			#define DANUBE_PMU_PWDCR                ((volatile unsigned long *)(DANUBE_PMU+0x001C))
		#endif

		#ifndef DANUBE_GPIO_P0_OUT
			#define DANUBE_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define DANUBE_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define DANUBE_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define DANUBE_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define DANUBE_GPIO_P0_OD                       (0xBF103000+0x24)
			#define DANUBE_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define DANUBE_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define DANUBE_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define DANUBE_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define DANUBE_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define DANUBE_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define DANUBE_GPIO_P1_OD                       (0xBF103000+0x54)
			#define DANUBE_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define DANUBE_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif


		#define DANUBE_RCU_USBCFG  ((volatile unsigned long *)(DANUBE_RCU_BASE_ADDR + 0x18))
		#define DANUBE_RCU_RESET   ((volatile unsigned long *)(DANUBE_RCU_BASE_ADDR + 0x10))
		#define DANUBE_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define DANUBE_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define DANUBE_USBCFG_SLV_END_BIT  9	// 0:little_end, 1:big_end

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4 //(NoChange)
		#define default_param_turn_around_time_fs 4 //(NoChange)
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)
		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        640
			#define default_param_nperio_tx_fifo_size 640
			#define default_param_perio_tx_fifo_size  768
		#endif //__IS_HOST__

		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
				#define default_param_rx_fifo_size          1024
				#define default_param_nperio_tx_fifo_size   1016
				#define default_param_perio_tx_fifo_size_01 8
			#else
				#define default_param_rx_fifo_size          1024
				#define default_param_nperio_tx_fifo_size   1024
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_AMAZON_SE__)
		#include <asm/amazon_se/amazon_se.h>
		//#include <asm/amazon_se/irq.h>

		#define IFXUSB_IRQ          31
		#define IFXUSB_IOMEM_BASE   0x1e101000
		#define IFXUSB_FIFOMEM_BASE 0x1e120000
		#define IFXUSB_FIFODBG_BASE 0x1e140000
		#define IFXUSB_OC_IRQ       20

		#define AMAZON_SE_RCU_USBCFG  ((volatile unsigned long *)(AMAZON_SE_RCU_BASE_ADDR + 0x18))
		#define AMAZON_SE_RCU_RESET   ((volatile unsigned long *)(AMAZON_SE_RCU_BASE_ADDR + 0x10))
		#define AMAZON_SE_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AMAZON_SE_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AMAZON_SE_USBCFG_SLV_END_BIT  9 	// 0:little_end, 1:big_end

		#ifndef AMAZON_SE_GPIO_P0_OUT
			#define AMAZON_SE_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AMAZON_SE_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AMAZON_SE_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AMAZON_SE_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AMAZON_SE_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AMAZON_SE_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AMAZON_SE_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AMAZON_SE_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AMAZON_SE_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AMAZON_SE_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AMAZON_SE_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AMAZON_SE_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AMAZON_SE_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AMAZON_SE_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif


		#ifndef AMAZON_SE_CGU
			#define AMAZON_SE_CGU                      (0xBF103000)
		#endif
		#ifndef AMAZON_SE_CGU_IFCCR
			#define AMAZON_SE_CGU_IFCCR                ((volatile unsigned long *)(AMAZON_SE_CGU+ 0x0018))
		#endif
		#ifndef AMAZON_SE_PMU
			#define AMAZON_SE_PMU                      (KSEG1+0x1F102000)
		#endif
		#ifndef AMAZON_SE_PMU_PWDCR
			#define AMAZON_SE_PMU_PWDCR                ((volatile unsigned long *)(AMAZON_SE_PMU+0x001C))
		#endif

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4 //(NoChange)
		#define default_param_turn_around_time_fs 4 //(NoChange)
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   248
				#define default_param_perio_tx_fifo_size_01 8
			#else
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   256
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_AR9__)
		#define IFXUSB1_IRQ 54
		#define IFXUSB1_IOMEM_BASE   0x1E101000
		#define IFXUSB1_FIFOMEM_BASE 0x1E120000
		#define IFXUSB1_FIFODBG_BASE 0x1E140000

		#define IFXUSB2_IRQ 83
		#define IFXUSB2_IOMEM_BASE   0x1E106000
		#define IFXUSB2_FIFOMEM_BASE 0x1E1E0000
		#define IFXUSB2_FIFODBG_BASE 0x1E1C0000

		#define IFXUSB_OC_IRQ 60

		#ifndef AMAZON_S_RCU_BASE_ADDR
			#define AMAZON_S_RCU_BASE_ADDR                (0xBF203000)
		#endif

		#ifndef AMAZON_S_CGU
			#define AMAZON_S_CGU                          (0xBF103000)
		#endif
		#ifndef AMAZON_S_CGU_IFCCR
			#define AMAZON_S_CGU_IFCCR                        ((volatile unsigned long *)(AMAZON_S_CGU+ 0x0018))
		#endif

		#ifndef AMAZON_S_PMU
			#define AMAZON_S_PMU                              (KSEG1+0x1F102000)
		#endif
		#ifndef AMAZON_S_PMU_PWDCR
			#define AMAZON_S_PMU_PWDCR                        ((volatile unsigned long *)(AMAZON_S_PMU+0x001C))
		#endif

		#ifndef AMAZON_S_GPIO_P0_OUT
			#define AMAZON_S_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AMAZON_S_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AMAZON_S_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AMAZON_S_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AMAZON_S_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AMAZON_S_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AMAZON_S_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AMAZON_S_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AMAZON_S_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AMAZON_S_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AMAZON_S_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AMAZON_S_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AMAZON_S_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AMAZON_S_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define AMAZON_S_RCU_USB1CFG  ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x18))
		#define AMAZON_S_RCU_USB2CFG  ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x34))
		#define AMAZON_S_RCU_USBRESET ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x10))
		#define AMAZON_S_USBCFG_ARB          7	//
		#define AMAZON_S_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AMAZON_S_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AMAZON_S_USBCFG_SLV_END_BIT  17	// 0:little_end, 1:big_end

		#define default_param_dma_burst_size      4

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size   -1  //(Max, hwcfg)
		#define default_param_max_packet_count    -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width      16

		#define default_param_turn_around_time_hs 4 //(NoChange)
		#define default_param_turn_around_time_fs 4 //(NoChange)
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
			#ifdef __DED_INTR__
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   248
				#define default_param_perio_tx_fifo_size_01 8
			#else
				#define default_param_rx_fifo_size          256
				#define default_param_nperio_tx_fifo_size   256
				#define default_param_perio_tx_fifo_size_01 0
			#endif
			#define default_param_perio_tx_fifo_size_02 0
			#define default_param_perio_tx_fifo_size_03 0
			#define default_param_perio_tx_fifo_size_04 0
			#define default_param_perio_tx_fifo_size_05 0
			#define default_param_perio_tx_fifo_size_06 0
			#define default_param_perio_tx_fifo_size_07 0
			#define default_param_perio_tx_fifo_size_08 0
			#define default_param_perio_tx_fifo_size_09 0
			#define default_param_perio_tx_fifo_size_10 0
			#define default_param_perio_tx_fifo_size_11 0
			#define default_param_perio_tx_fifo_size_12 0
			#define default_param_perio_tx_fifo_size_13 0
			#define default_param_perio_tx_fifo_size_14 0
			#define default_param_perio_tx_fifo_size_15 0
		#endif //__IS_DEVICE__

	#elif defined(__IS_VR9__)
		#define IFXUSB1_IRQ 54
		#define IFXUSB1_IOMEM_BASE   0x1E101000
		#define IFXUSB1_FIFOMEM_BASE 0x1E120000
		#define IFXUSB1_FIFODBG_BASE 0x1E140000

		#define IFXUSB2_IRQ 83
		#define IFXUSB2_IOMEM_BASE   0x1E106000
		#define IFXUSB2_FIFOMEM_BASE 0x1E1E0000
		#define IFXUSB2_FIFODBG_BASE 0x1E1C0000
		#define IFXUSB_OC_IRQ 68

		#ifndef AMAZON_S_RCU_BASE_ADDR
			#define AMAZON_S_RCU_BASE_ADDR            (0xBF203000)
		#endif

		#ifndef AMAZON_S_CGU
			#define AMAZON_S_CGU                          (0xBF103000)
		#endif
		#ifndef AMAZON_S_CGU_IFCCR
			#define AMAZON_S_CGU_IFCCR                        ((volatile unsigned long *)(AMAZON_S_CGU+ 0x0018))
		#endif

		#ifndef AMAZON_S_PMU
			#define AMAZON_S_PMU                              (KSEG1+0x1F102000)
		#endif
		#ifndef AMAZON_S_PMU_PWDCR
			#define AMAZON_S_PMU_PWDCR                        ((volatile unsigned long *)(AMAZON_S_PMU+0x001C))
		#endif

		#ifndef AMAZON_S_GPIO_P0_OUT
			#define AMAZON_S_GPIO_P0_OUT                      (0xBF103000+0x10)
			#define AMAZON_S_GPIO_P0_DIR                      (0xBF103000+0x18)
			#define AMAZON_S_GPIO_P0_ALTSEL0                  (0xBF103000+0x1C)
			#define AMAZON_S_GPIO_P0_ALTSEL1                  (0xBF103000+0x20)
			#define AMAZON_S_GPIO_P0_OD                       (0xBF103000+0x24)
			#define AMAZON_S_GPIO_P0_PUDSEL                   (0xBF103000+0x2C)
			#define AMAZON_S_GPIO_P0_PUDEN                    (0xBF103000+0x30)
			#define AMAZON_S_GPIO_P1_OUT                      (0xBF103000+0x40)
			#define AMAZON_S_GPIO_P1_DIR                      (0xBF103000+0x48)
			#define AMAZON_S_GPIO_P1_ALTSEL0                  (0xBF103000+0x4C)
			#define AMAZON_S_GPIO_P1_ALTSEL1                  (0xBF103000+0x50)
			#define AMAZON_S_GPIO_P1_OD                       (0xBF103000+0x54)
			#define AMAZON_S_GPIO_P1_PUDSEL                   (0xBF103000+0x5C)
			#define AMAZON_S_GPIO_P1_PUDEN                    (0xBF103000+0x60)
		#endif

		#define AMAZON_S_RCU_USB1CFG  ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x18))
		#define AMAZON_S_RCU_USB2CFG  ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x34))
		#define AMAZON_S_RCU_USBRESET ((volatile unsigned long *)(AMAZON_S_RCU_BASE_ADDR + 0x10))
		#define AMAZON_S_USBCFG_ARB          7	//
		#define AMAZON_S_USBCFG_HDSEL_BIT    11	// 0:host, 1:device
		#define AMAZON_S_USBCFG_HOST_END_BIT 10	// 0:little_end, 1:big_end
		#define AMAZON_S_USBCFG_SLV_END_BIT  17	// 0:little_end, 1:big_end

		#define default_param_dma_burst_size 4      //(ALL)

		#define default_param_speed               IFXUSB_PARAM_SPEED_HIGH

		#define default_param_max_transfer_size -1  //(Max, hwcfg)
		#define default_param_max_packet_count  -1  //(Max, hwcfg)
		#define default_param_phy_utmi_width    16

		#define default_param_turn_around_time_hs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_turn_around_time_fs 6 //(NoChange) snpsid >= 0x4f54260a
		#define default_param_timeout_cal_hs      -1 //(NoChange)
		#define default_param_timeout_cal_fs      -1 //(NoChange)

		#define default_param_data_fifo_size      -1 //(Max, hwcfg)

		#ifdef __IS_HOST__
			#define default_param_host_channels       -1 //(Max, hwcfg)
			#define default_param_rx_fifo_size        240
			#define default_param_nperio_tx_fifo_size 240
			#define default_param_perio_tx_fifo_size  32
		#endif //__IS_HOST__
		#ifdef __IS_DEVICE__
				#define default_param_rx_fifo_size          256
			#define default_param_tx_fifo_size_00 -1
			#define default_param_tx_fifo_size_01 -1
			#define default_param_tx_fifo_size_02 -1
			#define default_param_tx_fifo_size_03 -1
			#define default_param_tx_fifo_size_04 -1
			#define default_param_tx_fifo_size_05 -1
			#define default_param_tx_fifo_size_06 -1
			#define default_param_tx_fifo_size_07 -1
			#define default_param_tx_fifo_size_08 -1
			#define default_param_tx_fifo_size_09 -1
			#define default_param_tx_fifo_size_10 -1
			#define default_param_tx_fifo_size_11 -1
			#define default_param_tx_fifo_size_12 -1
			#define default_param_tx_fifo_size_13 -1
			#define default_param_tx_fifo_size_14 -1
			#define default_param_tx_fifo_size_15 -1
			#define default_param_dma_unalgned_tx -1
			#define default_param_dma_unalgned_rx -1
			#define default_param_thr_ctl         -1
			#define default_param_tx_thr_length   -1
			#define default_param_rx_thr_length   -1
		#endif //__IS_DEVICE__
	#else // __IS_VR9__
		#error "Please choose one platform!!"
	#endif // __IS_VR9__
#endif //UEIP

/*@}*//*IFXUSB_PLATEFORM_MEM_ADDR*/

/////////////////////////////////////////////////////////////////////////

#ifdef __IS_HOST__
	#if defined(CONFIG_USB_HOST_IFX_FORCE_USB11) || defined(__FORCE_USB11__)
		#undef  default_param_speed
		#define default_param_speed               IFXUSB_PARAM_SPEED_FULL
	#endif
#endif
#ifdef __IS_DEVICE__
	#if !defined(CONFIG_USB_GADGET_DUALSPEED) || defined(__FORCE_USB11__)
		#undef  default_param_speed
		#define default_param_speed               IFXUSB_PARAM_SPEED_FULL
	#endif
#endif

/////////////////////////////////////////////////////////////////////////

static __inline__ void UDELAY( const uint32_t _usecs )
{
	udelay( _usecs );
}

static __inline__ void MDELAY( const uint32_t _msecs )
{
	mdelay( _msecs );
}

static __inline__ void SPIN_LOCK( spinlock_t *_lock )
{
	spin_lock(_lock);
}

static __inline__ void SPIN_UNLOCK( spinlock_t *_lock )
{
	spin_unlock(_lock);
}

#define SPIN_LOCK_IRQSAVE( _l, _f )  \
	{ \
	spin_lock_irqsave(_l,_f); \
	}

#define SPIN_UNLOCK_IRQRESTORE( _l,_f ) \
	{ \
	spin_unlock_irqrestore(_l,_f); \
	}

/////////////////////////////////////////////////////////////////////////
/*!
  \addtogroup IFXUSB_DBG_ROUTINE
 */
/*@{*/
#ifdef __IS_HOST__
	extern uint32_t h_dbg_lvl;
#endif

#ifdef __IS_DEVICE__
	extern uint32_t d_dbg_lvl;
#endif

/*! \brief When debug level has the DBG_CIL bit set, display CIL Debug messages. */
#define DBG_CIL		(0x2)
/*! \brief When debug level has the DBG_CILV bit set, display CIL Verbose debug messages */
#define DBG_CILV	(0x20)
/*! \brief When debug level has the DBG_PCD bit set, display PCD (Device) debug messages */
#define DBG_PCD		(0x4)
/*! \brief When debug level has the DBG_PCDV set, display PCD (Device) Verbose debug messages */
#define DBG_PCDV	(0x40)
/*! \brief When debug level has the DBG_HCD bit set, display Host debug messages */
#define DBG_HCD		(0x8)
/*! \brief When debug level has the DBG_HCDV bit set, display Verbose Host debug messages */
#define DBG_HCDV	(0x80)
/*! \brief When debug level has the DBG_HCD_URB bit set, display enqueued URBs in host mode. */
#define DBG_HCD_URB	(0x800)
/*! \brief When debug level has any bit set, display debug messages */
#define DBG_ANY		(0xFF)
/*! \brief All debug messages off */
#define DBG_OFF		0

#define DBG_ENTRY	(0x8000)

#define IFXUSB "IFXUSB: "

/*!
   \fn    inline uint32_t SET_DEBUG_LEVEL( const uint32_t _new )
   \brief Set the Debug Level variable.
   \param _new 32-bit mask of debug level.
   \return previous debug level
 */
static inline uint32_t SET_DEBUG_LEVEL( const uint32_t _new )
{
	#ifdef __IS_HOST__
		uint32_t old = h_dbg_lvl;
		h_dbg_lvl = _new;
	#endif

	#ifdef __IS_DEVICE__
		uint32_t old = d_dbg_lvl;
		d_dbg_lvl = _new;
	#endif
	return old;
}

#ifdef __DEBUG__
	#ifdef __IS_HOST__
		# define IFX_DEBUGPL(lvl, x...) do{ if ((lvl)&h_dbg_lvl)printk( KERN_DEBUG IFXUSB x ); }while(0)
		# define CHK_DEBUG_LEVEL(level) ((level) & h_dbg_lvl)
	#endif

	#ifdef __IS_DEVICE__
		# define IFX_DEBUGPL(lvl, x...) do{ if ((lvl)&d_dbg_lvl)printk( KERN_DEBUG IFXUSB x ); }while(0)
		# define CHK_DEBUG_LEVEL(level) ((level) & d_dbg_lvl)
	#endif

	# define IFX_DEBUGP(x...)	IFX_DEBUGPL(DBG_ANY, x )
#else
	# define IFX_DEBUGPL(lvl, x...) do{}while(0)
	# define IFX_DEBUGP(x...)
	# define CHK_DEBUG_LEVEL(level) (0)
#endif //__DEBUG__

/* Print an Error message. */
#define IFX_ERROR(x...) printk( KERN_ERR IFXUSB x )
/* Print a Warning message. */
#define IFX_WARN(x...) printk( KERN_WARNING IFXUSB x )
/* Print a notice (normal but significant message). */
#define IFX_NOTICE(x...) printk( KERN_NOTICE IFXUSB x )
/*  Basic message printing. */
#define IFX_PRINT(x...) printk( KERN_INFO IFXUSB x )

/*@}*//*IFXUSB_DBG_ROUTINE*/


#endif //__IFXUSB_PLAT_H__

