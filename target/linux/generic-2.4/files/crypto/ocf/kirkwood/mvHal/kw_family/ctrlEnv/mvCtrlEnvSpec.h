/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvCtrlEnvSpech
#define __INCmvCtrlEnvSpech

#include "mvDeviceId.h"
#include "mvSysHwConfig.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MV_ARM_SOC
#define SOC_NAME_PREFIX			"MV88F"


/* units base and port numbers */
#ifdef MV_ASMLANGUAGE
#define XOR_UNIT_BASE(unit)           	0x60800
#else
#define MV_XOR_REG_BASE			0x60000
#define XOR_UNIT_BASE(unit)           	((unit)? 0x60900:0x60800)
#endif

#define TDM_REG_BASE			0xD0000
#define USB_REG_BASE(dev)       	0x50000
#define AUDIO_REG_BASE  		0xA0000
#define SATA_REG_BASE           	0x80000
#define MV_CESA_REG_BASE                0x3D000
#define MV_CESA_TDMA_REG_BASE		0x30000
#define MV_SDIO_REG_BASE		0x90000
#define MV_ETH_REG_BASE(port)      	(((port) == 0) ? 0x72000 : 0x76000)
#define MV_UART_CHAN_BASE(chanNum)	(0x12000 + (chanNum * 0x100))
#define DRAM_BASE			0x0
#define CNTMR_BASE                  	0x20300
#define TWSI_SLAVE_BASE(chanNum)    	0x11000
#define PEX_IF_BASE(pexIf)      	0x40000
#define MPP_REG_BASE			0x10000
#define TSU_GLOBAL_REG_BASE             0xB4000
#define MAX_AHB_TO_MBUS_REG_BASE	0x20000

#define INTER_REGS_SIZE	 	 	_1M
/* This define describes the TWSI interrupt bit and location */
#define TWSI_CPU_MAIN_INT_CAUSE_REG		0x20200
#define TWSI0_CPU_MAIN_INT_BIT			(1<<29)
#define TWSI_SPEED				100000

#define MV_GPP_MAX_GROUP    		2
#define MV_CNTMR_MAX_COUNTER 		2
#define MV_UART_MAX_CHAN		2
#define MV_XOR_MAX_UNIT         	2
#define MV_XOR_MAX_CHAN         	4 /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT       	2 /* channels for units */
#define MV_SATA_MAX_CHAN	 	2

#define MV_6281_MPP_MAX_MODULE    	2
#define MV_6192_MPP_MAX_MODULE    	1
#define MV_6190_MPP_MAX_MODULE    	1
#define MV_6180_MPP_MAX_MODULE    	2
#define MV_6281_MPP_MAX_GROUP    	7
#define MV_6192_MPP_MAX_GROUP    	4
#define MV_6190_MPP_MAX_GROUP    	4
#define MV_6180_MPP_MAX_GROUP    	3

#define MV_DRAM_MAX_CS      		4

/* This define describes the maximum number of supported PCI\PCIX Interfaces*/
#define MV_PCI_MAX_IF		0
#define MV_PCI_START_IF		0

/* This define describes the maximum number of supported PEX Interfaces 	*/
#define MV_INCLUDE_PEX0
#define MV_DISABLE_PEX_DEVICE_BAR 
#define MV_PEX_MAX_IF		1
#define MV_PEX_START_IF		MV_PCI_MAX_IF

/* This define describes the maximum number of supported PCI Interfaces 	*/
#define MV_PCI_IF_MAX_IF   	(MV_PEX_MAX_IF+MV_PCI_MAX_IF)

#define MV_ETH_MAX_PORTS		2
#define MV_6281_ETH_MAX_PORTS	   	2
#define MV_6192_ETH_MAX_PORTS	   	2
#define MV_6190_ETH_MAX_PORTS	   	1
#define MV_6180_ETH_MAX_PORTS	   	1

#define MV_IDMA_MAX_CHAN    		0

#define MV_USB_MAX_PORTS		1

#define MV_USB_VERSION              1


#define MV_6281_NAND			1
#define MV_6192_NAND			1
#define MV_6190_NAND			1
#define MV_6180_NAND			0

#define MV_6281_SDIO			1
#define MV_6192_SDIO			1
#define MV_6190_SDIO			1
#define MV_6180_SDIO			1

#define MV_6281_TS			1
#define MV_6192_TS			1
#define MV_6190_TS			0
#define MV_6180_TS			0

#define MV_6281_AUDIO			1
#define MV_6192_AUDIO			1
#define MV_6190_AUDIO			0
#define MV_6180_AUDIO			1

#define MV_6281_TDM			1
#define MV_6192_TDM			1
#define MV_6190_TDM			0
#define MV_6180_TDM			0

#define MV_DEVICE_MAX_CS      		4

/* Others */
#define PEX_HOST_BUS_NUM(pciIf)		(pciIf)
#define PEX_HOST_DEV_NUM(pciIf)		0

#define PCI_IO(pciIf)		(PEX0_IO)
#define PCI_MEM(pciIf, memNum)  (PEX0_MEM0)
/* CESA version #2: One channel, 2KB SRAM, TDMA */
#if defined(MV_CESA_CHAIN_MODE_SUPPORT)
	#define MV_CESA_VERSION		 	3
#else
#define MV_CESA_VERSION		 	2
#endif
#define MV_CESA_SRAM_SIZE               2*1024
/* This define describes the maximum number of supported Ethernet ports 	*/
#define MV_ETH_VERSION 			4
#define MV_ETH_MAX_RXQ              	8
#define MV_ETH_MAX_TXQ              	8
#define MV_ETH_PORT_SGMII          	{ MV_FALSE, MV_FALSE }
/* This define describes the the support of USB 	*/
#define MV_USB_VERSION  		1

#define MV_INCLUDE_SDRAM_CS0
#define MV_INCLUDE_SDRAM_CS1
#define MV_INCLUDE_SDRAM_CS2
#define MV_INCLUDE_SDRAM_CS3

#define MV_INCLUDE_DEVICE_CS0
#define MV_INCLUDE_DEVICE_CS1
#define MV_INCLUDE_DEVICE_CS2
#define MV_INCLUDE_DEVICE_CS3

#define MPP_GROUP_1_TYPE {\
	{0, 0, 0}, /* Reserved for AUTO */ \
	{0x22220000, 0x22222222, 0x2222}, /* TDM */ \
	{0x44440000, 0x00044444, 0x0000}, /* AUDIO */ \
	{0x33330000, 0x33003333, 0x0033}, /* RGMII */ \
	{0x33330000, 0x03333333, 0x0033}, /* GMII */ \
	{0x11110000, 0x11111111, 0x0001}, /* TS */ \
	{0x33330000, 0x33333333, 0x3333}  /* MII */ \
}

#define MPP_GROUP_2_TYPE {\
	{0, 0, 0}, /* Reserved for AUTO */ \
	{0x22220000, 0x22222222, 0x22}, /* TDM */ \
	{0x44440000, 0x00044444, 0x0}, /* AUDIO */ \
	{0, 0, 0}, /* N_A */ \
	{0, 0, 0}, /* N_A */ \
	{0x11110000, 0x11111111, 0x01}  /* TS */ \
}

#ifndef MV_ASMLANGUAGE

/* This enumerator defines the Marvell Units ID      */ 
typedef enum _mvUnitId
{
    DRAM_UNIT_ID,
    PEX_UNIT_ID,
    ETH_GIG_UNIT_ID,
    USB_UNIT_ID,
    IDMA_UNIT_ID,
    XOR_UNIT_ID,
    SATA_UNIT_ID,
    TDM_UNIT_ID,
    UART_UNIT_ID,
    CESA_UNIT_ID,
    SPI_UNIT_ID,
    AUDIO_UNIT_ID,
    SDIO_UNIT_ID,
    TS_UNIT_ID,
    MAX_UNITS_ID

}MV_UNIT_ID;

#endif

#endif /* __INCmvCtrlEnvSpech */
