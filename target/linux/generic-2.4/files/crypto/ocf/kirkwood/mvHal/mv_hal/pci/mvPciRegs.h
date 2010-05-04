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

#ifndef __INCPCIREGSH
#define __INCPCIREGSH


#include "pci-if/mvPciIfRegs.h"
/* defines */
#define MAX_PCI_DEVICES         32
#define MAX_PCI_FUNCS           8
#define MAX_PCI_BUSSES          128

/* enumerators */

/* This enumerator described the possible PCI slave targets.    	   */
/* PCI slave targets are designated memory/IO address spaces that the  */
/* PCI slave targets can access. They are also refered as "targets"    */
/* this enumeratoe order is determined by the content of : 
		PCI_BASE_ADDR_ENABLE_REG 				 					*/


/* registers offsetes defines */



/*************************/
/* PCI control registers */
/*************************/
/* maen : should add new registers */
#define PCI_CMD_REG(pciIf)				 		(0x30c00  + ((pciIf) * 0x80))
#define PCI_MODE_REG(pciIf)				 		(0x30d00  + ((pciIf) * 0x80))
#define PCI_RETRY_REG(pciIf)					(0x30c04  + ((pciIf) * 0x80))
#define PCI_DISCARD_TIMER_REG(pciIf)			(0x30d04  + ((pciIf) * 0x80))
#define PCI_ARBITER_CTRL_REG(pciIf)				(0x31d00 + ((pciIf) * 0x80))
#define PCI_P2P_CONFIG_REG(pciIf)				(0x31d14 + ((pciIf) * 0x80))
#define PCI_ACCESS_CTRL_BASEL_REG(pciIf, targetWin) \
							(0x31e00 + ((pciIf) * 0x80) + ((targetWin) * 0x10))
#define PCI_ACCESS_CTRL_BASEH_REG(pciIf, targetWin) \
							(0x31e04 + ((pciIf) * 0x80) + ((targetWin) * 0x10))
#define PCI_ACCESS_CTRL_SIZE_REG(pciIf, targetWin)	\
							(0x31e08 + ((pciIf) * 0x80) + ((targetWin) * 0x10))
							
#define PCI_DLL_CTRL_REG(pciIf)	   		 		(0x31d20  + ((pciIf) * 0x80))

/* PCI Dll Control (PDC)*/
#define PDC_DLL_EN					BIT0


/* PCI Command Register (PCR) */
#define PCR_MASTER_BYTE_SWAP_EN     BIT0
#define PCR_MASTER_WR_COMBINE_EN    BIT4
#define PCR_MASTER_RD_COMBINE_EN    BIT5
#define PCR_MASTER_WR_TRIG_WHOLE    BIT6
#define PCR_MASTER_RD_TRIG_WHOLE    BIT7
#define PCR_MASTER_MEM_RD_LINE_EN   BIT8
#define PCR_MASTER_MEM_RD_MULT_EN   BIT9
#define PCR_MASTER_WORD_SWAP_EN     BIT10
#define PCR_SLAVE_WORD_SWAP_EN      BIT11
#define PCR_NS_ACCORDING_RCV_TRANS  BIT14
#define PCR_MASTER_PCIX_REQ64N_EN   BIT15
#define PCR_SLAVE_BYTE_SWAP_EN      BIT16
#define PCR_MASTER_DAC_EN           BIT17
#define PCR_MASTER_M64_ALLIGN       BIT18
#define PCR_ERRORS_PROPAGATION_EN   BIT19
#define PCR_SLAVE_SWAP_ENABLE       BIT20
#define PCR_MASTER_SWAP_ENABLE      BIT21
#define PCR_MASTER_INT_SWAP_EN      BIT22
#define PCR_LOOP_BACK_ENABLE        BIT23
#define PCR_SLAVE_INTREG_SWAP_OFFS  24
#define PCR_SLAVE_INTREG_SWAP_MASK  0x3
#define PCR_SLAVE_INTREG_BYTE_SWAP  \
                             (MV_BYTE_SWAP << PCR_SLAVE_INT_REG_SWAP_MASK)
#define PCR_SLAVE_INTREG_NO_SWAP    \
                             (MV_NO_SWAP   << PCR_SLAVE_INT_REG_SWAP_MASK)
#define PCR_SLAVE_INTREG_BYTE_WORD  \
                             (MV_BYTE_WORD_SWAP << PCR_SLAVE_INT_REG_SWAP_MASK)
#define PCR_SLAVE_INTREG_WORD_SWAP  \
                             (MV_WORD_SWAP << PCR_SLAVE_INT_REG_SWAP_MASK)
#define PCR_RESET_REASSERTION_EN    BIT26
#define PCR_PCI_TO_CPU_REG_ORDER_EN BIT28
#define PCR_CPU_TO_PCI_ORDER_EN     BIT29
#define PCR_PCI_TO_CPU_ORDER_EN     BIT30

/* PCI Mode Register (PMR) */
#define PMR_PCI_ID_OFFS 			0  /* PCI Interface ID */
#define PMR_PCI_ID_MASK 			(0x1 << PMR_PCI_ID_OFFS)
#define PMR_PCI_ID_PCI(pciNum) 		((pciNum) << PCI_MODE_PCIID_OFFS)

#define PMR_PCI_64_OFFS				2 	/* 64-bit PCI Interface */
#define PMR_PCI_64_MASK				(0x1 << PMR_PCI_64_OFFS)
#define PMR_PCI_64_64BIT			(0x1 << PMR_PCI_64_OFFS)
#define PMR_PCI_64_32BIT			(0x0 << PMR_PCI_64_OFFS)

#define PMR_PCI_MODE_OFFS			4 	/* PCI interface mode of operation */
#define PMR_PCI_MODE_MASK			(0x3 << PMR_PCI_MODE_OFFS)
#define PMR_PCI_MODE_CONV			(0x0 << PMR_PCI_MODE_OFFS)
#define PMR_PCI_MODE_PCIX_66MHZ		(0x1 << PMR_PCI_MODE_OFFS)
#define PMR_PCI_MODE_PCIX_100MHZ	(0x2 << PMR_PCI_MODE_OFFS)
#define PMR_PCI_MODE_PCIX_133MHZ	(0x3 << PMR_PCI_MODE_OFFS)

#define PMR_EXP_ROM_SUPPORT			BIT8	/* Expansion ROM Active */

#define PMR_PCI_RESET_OFFS			31 /* PCI Interface Reset Indication */
#define PMR_PCI_RESET_MASK			(0x1 << PMR_PCI_RESET_OFFS)
#define PMR_PCI_RESET_PCIXRST		(0x0 << PMR_PCI_RESET_OFFS)


/* PCI Retry Register (PRR) */
#define PRR_RETRY_CNTR_OFFS			16 /* Retry Counter */
#define PRR_RETRY_CNTR_MAX			0xff
#define PRR_RETRY_CNTR_MASK			(PRR_RETRY_CNTR_MAX << PRR_RETRY_CNTR_OFFS)


/* PCI Discard Timer Register (PDTR) */
#define PDTR_TIMER_OFFS				0	/* Timer */
#define PDTR_TIMER_MAX				0xffff
#define PDTR_TIMER_MIN				0x7F
#define PDTR_TIMER_MASK				(PDTR_TIMER_MAX << PDTR_TIMER_OFFS)


/* PCI Arbiter Control Register (PACR) */
#define PACR_BROKEN_DETECT_EN		BIT1	/* Broken Detection Enable */

#define PACR_BROKEN_VAL_OFFS		3	/* Broken Value */
#define PACR_BROKEN_VAL_MASK		(0xf << PACR_BROKEN_VAL_OFFS)
#define PACR_BROKEN_VAL_CONV_MIN	0x2
#define PACR_BROKEN_VAL_PCIX_MIN	0x6

#define PACR_PARK_DIS_OFFS		14	/* Parking Disable */
#define PACR_PARK_DIS_MAX_AGENT	0x3f
#define PACR_PARK_DIS_MASK		(PACR_PARK_DIS_MAX_AGENT<<PACR_PARK_DIS_OFFS)
#define PACR_PARK_DIS(agent)	((1 << (agent)) << PACR_PARK_DIS_OFFS)

#define PACR_ARB_ENABLE				BIT31	/* Enable Internal Arbiter */


/* PCI P2P Configuration Register (PPCR) */
#define PPCR_2ND_BUS_L_OFFS			0	/* 2nd PCI Interface Bus Range Lower */
#define PPCR_2ND_BUS_L_MASK			(0xff << PPCR_2ND_BUS_L_OFFS)

#define PPCR_2ND_BUS_H_OFFS			8	/* 2nd PCI Interface Bus Range Upper */
#define PPCR_2ND_BUS_H_MASK			(0xff << PPCR_2ND_BUS_H_OFFS)

#define PPCR_BUS_NUM_OFFS			16  /* The PCI interface's Bus number */
#define PPCR_BUS_NUM_MASK			(0xff << PPCR_BUS_NUM_OFFS)

#define PPCR_DEV_NUM_OFFS			24  /* The PCI interface’s Device number */
#define PPCR_DEV_NUM_MASK			(0xff << PPCR_DEV_NUM_OFFS)


/* PCI Access Control Base Low Register (PACBLR) */
#define PACBLR_EN					BIT0 /* Access control window enable */

#define PACBLR_ACCPROT				BIT4 /* Access Protect */
#define PACBLR_WRPROT				BIT5 /* Write Protect */

#define PACBLR_PCISWAP_OFFS			6 	 /* PCI slave Data Swap Control */
#define PACBLR_PCISWAP_MASK			(0x3 << PACBLR_PCISWAP_OFFS)
#define PACBLR_PCISWAP_BYTE			(0x0 << PACBLR_PCISWAP_OFFS)
#define PACBLR_PCISWAP_NO_SWAP		(0x1 << PACBLR_PCISWAP_OFFS)
#define PACBLR_PCISWAP_BYTE_WORD	(0x2 << PACBLR_PCISWAP_OFFS)
#define PACBLR_PCISWAP_WORD			(0x3 << PACBLR_PCISWAP_OFFS)

#define PACBLR_RDMBURST_OFFS		8 /* Read Max Burst */
#define PACBLR_RDMBURST_MASK		(0x3 << PACBLR_RDMBURST_OFFS)
#define PACBLR_RDMBURST_32BYTE		(0x0 << PACBLR_RDMBURST_OFFS)
#define PACBLR_RDMBURST_64BYTE		(0x1 << PACBLR_RDMBURST_OFFS)
#define PACBLR_RDMBURST_128BYTE		(0x2 << PACBLR_RDMBURST_OFFS)

#define PACBLR_RDSIZE_OFFS			10 /* Typical PCI read transaction Size. */
#define PACBLR_RDSIZE_MASK			(0x3 << PACBLR_RDSIZE_OFFS)
#define PACBLR_RDSIZE_32BYTE		(0x0 << PACBLR_RDSIZE_OFFS)
#define PACBLR_RDSIZE_64BYTE		(0x1 << PACBLR_RDSIZE_OFFS)
#define PACBLR_RDSIZE_128BYTE		(0x2 << PACBLR_RDSIZE_OFFS)
#define PACBLR_RDSIZE_256BYTE		(0x3 << PACBLR_RDSIZE_OFFS)

#define PACBLR_BASE_L_OFFS			12	/* Corresponds to address bits [31:12] */
#define PACBLR_BASE_L_MASK			(0xfffff << PACBLR_BASE_L_OFFS)
#define PACBLR_BASE_L_ALIGNMENT		(1 << PACBLR_BASE_L_OFFS)
#define PACBLR_BASE_ALIGN_UP(base)  \
                             ((base+PACBLR_BASE_L_ALIGNMENT)&PACBLR_BASE_L_MASK)
#define PACBLR_BASE_ALIGN_DOWN(base)  (base & PACBLR_BASE_L_MASK)


/* PCI Access Control Base High Register (PACBHR) 	*/
#define PACBHR_BASE_H_OFFS			0	/* Corresponds to address bits [63:32] */
#define PACBHR_CTRL_BASE_H_MASK		(0xffffffff << PACBHR_BASE_H_OFFS)

/* PCI Access Control Size Register (PACSR) 		*/
#define PACSR_WRMBURST_OFFS			8 /* Write Max Burst */
#define PACSR_WRMBURST_MASK			(0x3 << PACSR_WRMBURST_OFFS)
#define PACSR_WRMBURST_32BYTE		(0x0 << PACSR_WRMBURST_OFFS)
#define PACSR_WRMBURST_64BYTE		(0x1 << PACSR_WRMBURST_OFFS)
#define PACSR_WRMBURST_128BYTE		(0x2 << PACSR_WRMBURST_OFFS)

#define PACSR_PCI_ORDERING			BIT11 /* PCI Ordering required */

#define PACSR_SIZE_OFFS				12	/* PCI access window size */
#define PACSR_SIZE_MASK				(0xfffff << PACSR_SIZE_OFFS)
#define PACSR_SIZE_ALIGNMENT		(1 << PACSR_SIZE_OFFS)
#define PACSR_SIZE_ALIGN_UP(size)   \
                                   ((size+PACSR_SIZE_ALIGNMENT)&PACSR_SIZE_MASK)
#define PACSR_SIZE_ALIGN_DOWN(size) (size & PACSR_SIZE_MASK)


/***************************************/
/* PCI Configuration Access Registers  */
/***************************************/

#define PCI_CONFIG_ADDR_REG(pciIf)	(0x30C78 - ((pciIf) * 0x80) )
#define PCI_CONFIG_DATA_REG(pciIf)	(0x30C7C - ((pciIf) * 0x80) )
#define PCI_INT_ACK_REG(pciIf)		(0x30C34 + ((pciIf) * 0x80) )

/* PCI Configuration Address Register (PCAR) */
#define PCAR_REG_NUM_OFFS			2
#define PCAR_REG_NUM_MASK			(0x3F << PCAR_REG_NUM_OFFS)
		
#define PCAR_FUNC_NUM_OFFS			8
#define PCAR_FUNC_NUM_MASK			(0x7 << PCAR_FUNC_NUM_OFFS)
		
#define PCAR_DEVICE_NUM_OFFS		11
#define PCAR_DEVICE_NUM_MASK		(0x1F << PCAR_DEVICE_NUM_OFFS)
		
#define PCAR_BUS_NUM_OFFS			16
#define PCAR_BUS_NUM_MASK			(0xFF << PCAR_BUS_NUM_OFFS)
		
#define PCAR_CONFIG_EN				BIT31


/***************************************/
/* PCI Configuration registers */
/***************************************/

/*********************************************/
/* PCI Configuration, Function 0, Registers  */
/*********************************************/

/* Marvell Specific */
#define PCI_SCS0_BASE_ADDR_LOW			   			0x010
#define PCI_SCS0_BASE_ADDR_HIGH			   			0x014
#define PCI_SCS1_BASE_ADDR_LOW		  				0x018
#define PCI_SCS1_BASE_ADDR_HIGH			  			0x01C
#define PCI_INTER_REG_MEM_MAPPED_BASE_ADDR_L 		0x020
#define PCI_INTER_REG_MEM_MAPPED_BASE_ADDR_H		0x024

/* capability list */
#define PCI_POWER_MNG_CAPABILITY		            0x040
#define PCI_POWER_MNG_STATUS_CONTROL		        0x044
#define PCI_VPD_ADDRESS_REG	                        0x048
#define PCI_VPD_DATA_REG	                        0x04c
#define PCI_MSI_MESSAGE_CONTROL						0x050
#define PCI_MSI_MESSAGE_ADDR		                0x054
#define PCI_MSI_MESSAGE_UPPER_ADDR		            0x058
#define PCI_MSI_MESSAGE_DATA		                0x05c
#define PCIX_COMMAND		                        0x060
#define PCIX_STATUS		                            0x064
#define PCI_COMPACT_PCI_HOT_SWAP		            0x068


/*********************************************/
/* PCI Configuration, Function 1, Registers  */
/*********************************************/

#define PCI_SCS2_BASE_ADDR_LOW						0x10
#define PCI_SCS2_BASE_ADDR_HIGH						0x14
#define PCI_SCS3_BASE_ADDR_LOW		 				0x18
#define PCI_SCS3_BASE_ADDR_HIGH						0x1c


/***********************************************/
/*  PCI Configuration, Function 2, Registers   */
/***********************************************/

#define PCI_DEVCS0_BASE_ADDR_LOW					0x10
#define PCI_DEVCS0_BASE_ADDR_HIGH		 			0x14
#define PCI_DEVCS1_BASE_ADDR_LOW		 			0x18
#define PCI_DEVCS1_BASE_ADDR_HIGH		      		0x1c
#define PCI_DEVCS2_BASE_ADDR_LOW		 			0x20
#define PCI_DEVCS2_BASE_ADDR_HIGH		      		0x24

/***********************************************/
/*  PCI Configuration, Function 3, Registers   */
/***********************************************/

#define PCI_BOOTCS_BASE_ADDR_LOW					0x18
#define PCI_BOOTCS_BASE_ADDR_HIGH		      		0x1c

/***********************************************/
/*  PCI Configuration, Function 4, Registers   */
/***********************************************/

#define PCI_P2P_MEM0_BASE_ADDR_LOW				   	0x10
#define PCI_P2P_MEM0_BASE_ADDR_HIGH		 			0x14
#define PCI_P2P_IO_BASE_ADDR		               	0x20
#define PCI_INTER_REGS_IO_MAPPED_BASE_ADDR		   0x24

/* PCIX_STATUS  register fields (PXS) */

#define PXS_FN_OFFS		0	/* Description Number */
#define PXS_FN_MASK		(0x7 << PXS_FN_OFFS)

#define PXS_DN_OFFS		3	/* Device Number */
#define PXS_DN_MASK		(0x1f << PXS_DN_OFFS)

#define PXS_BN_OFFS		8	/* Bus Number */
#define PXS_BN_MASK		(0xff << PXS_BN_OFFS)


/* PCI Error Report Register Map */
#define PCI_SERRN_MASK_REG(pciIf)		(0x30c28  + (pciIf * 0x80))
#define PCI_CAUSE_REG(pciIf)			(0x31d58 + (pciIf * 0x80))
#define PCI_MASK_REG(pciIf)				(0x31d5C + (pciIf * 0x80))
#define PCI_ERROR_ADDR_LOW_REG(pciIf)	(0x31d40 + (pciIf * 0x80))
#define PCI_ERROR_ADDR_HIGH_REG(pciIf)	(0x31d44 + (pciIf * 0x80))
#define PCI_ERROR_ATTRIBUTE_REG(pciIf)	(0x31d48 + (pciIf * 0x80))
#define PCI_ERROR_COMMAND_REG(pciIf)	(0x31d50 + (pciIf * 0x80))

/* PCI Interrupt Cause Register (PICR) */
#define PICR_ERR_SEL_OFFS           27
#define PICR_ERR_SEL_MASK           (0x1f << PICR_ERR_SEL_OFFS)

/* PCI Error Command Register (PECR) */
#define PECR_ERR_CMD_OFFS			0
#define PECR_ERR_CMD_MASK			(0xf << PECR_ERR_CMD_OFFS)
#define PECR_DAC					BIT4


/* defaults */
/* Set bits means value is about to change according to new value */
#define PCI_COMMAND_DEFAULT_MASK                0xffffdff1
#define PCI_COMMAND_DEFAULT                             \
                (PCR_MASTER_WR_TRIG_WHOLE   |   \
         PCR_MASTER_RD_TRIG_WHOLE       |       \
                 PCR_MASTER_MEM_RD_LINE_EN      |       \
         PCR_MASTER_MEM_RD_MULT_EN  |   \
                 PCR_NS_ACCORDING_RCV_TRANS     |       \
                 PCR_MASTER_PCIX_REQ64N_EN      |       \
                 PCR_MASTER_DAC_EN                      |       \
                 PCR_MASTER_M64_ALLIGN          |       \
                 PCR_ERRORS_PROPAGATION_EN)
                 
                 
#define PCI_ARBITER_CTRL_DEFAULT_MASK   0x801fc07a
#define PCI_ARBITER_CTRL_DEFAULT        \
        (PACR_BROKEN_VAL_PCIX_MIN << PACR_BROKEN_VAL_OFFS)


#endif /* #ifndef __INCPCIREGSH */

