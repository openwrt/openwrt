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


#ifndef __INCmvCpuIfRegsh
#define __INCmvCpuIfRegsh

/****************************************/
/* ARM Control and Status Registers Map */
/****************************************/

#define CPU_CONFIG_REG				0x20100
#define CPU_CTRL_STAT_REG			0x20104
#define CPU_RSTOUTN_MASK_REG			0x20108
#define CPU_SYS_SOFT_RST_REG			0x2010C
#define CPU_AHB_MBUS_CAUSE_INT_REG		0x20110
#define CPU_AHB_MBUS_MASK_INT_REG		0x20114
#define CPU_FTDLL_CONFIG_REG			0x20120
#define CPU_L2_CONFIG_REG			0x20128



/* ARM Configuration register */
/* CPU_CONFIG_REG (CCR) */


/* Reset vector location */
#define CCR_VEC_INIT_LOC_OFFS			1
#define CCR_VEC_INIT_LOC_MASK			BIT1
/* reset at 0x00000000 */
#define CCR_VEC_INIT_LOC_0000			(0 << CCR_VEC_INIT_LOC_OFFS)
/* reset at 0xFFFF0000 */
#define CCR_VEC_INIT_LOC_FF00			(1 << CCR_VEC_INIT_LOC_OFFS)


#define CCR_AHB_ERROR_PROP_OFFS			2
#define CCR_AHB_ERROR_PROP_MASK			BIT2
/* Erros are not propogated to AHB */
#define CCR_AHB_ERROR_PROP_NO_INDICATE		(0 << CCR_AHB_ERROR_PROP_OFFS)
/* Erros are propogated to AHB */
#define CCR_AHB_ERROR_PROP_INDICATE		(1 << CCR_AHB_ERROR_PROP_OFFS)


#define CCR_ENDIAN_INIT_OFFS			3
#define CCR_ENDIAN_INIT_MASK			BIT3
#define CCR_ENDIAN_INIT_LITTLE			(0 << CCR_ENDIAN_INIT_OFFS)
#define CCR_ENDIAN_INIT_BIG			(1 << CCR_ENDIAN_INIT_OFFS)


#define CCR_INCR_EN_OFFS			4
#define CCR_INCR_EN_MASK			BIT4
#define CCR_INCR_EN				BIT4


#define CCR_NCB_BLOCKING_OFFS			5			
#define CCR_NCB_BLOCKING_MASK			(1 << CCR_NCB_BLOCKING_OFFS)
#define CCR_NCB_BLOCKING_NON			(0 << CCR_NCB_BLOCKING_OFFS)
#define CCR_NCB_BLOCKING_EN			(1 << CCR_NCB_BLOCKING_OFFS)

#define CCR_CPU_2_MBUSL_TICK_DRV_OFFS		8
#define CCR_CPU_2_MBUSL_TICK_DRV_MASK		(0xF << CCR_CPU_2_MBUSL_TICK_DRV_OFFS)
#define CCR_CPU_2_MBUSL_TICK_SMPL_OFFS		12
#define CCR_CPU_2_MBUSL_TICK_SMPL_MASK		(0xF << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS)
#define CCR_ICACH_PREF_BUF_ENABLE		BIT16
#define CCR_DCACH_PREF_BUF_ENABLE		BIT17

/* Ratio options for CPU to DDR for 6281/6192/6190 */
#define CPU_2_DDR_CLK_1x3	    4
#define CPU_2_DDR_CLK_1x4	    6

/* Ratio options for CPU to DDR for 6281 only */
#define CPU_2_DDR_CLK_2x9	    7
#define CPU_2_DDR_CLK_1x5	    8
#define CPU_2_DDR_CLK_1x6	    9

/* Ratio options for CPU to DDR for 6180 only */
#define CPU_2_DDR_CLK_1x3_1	    0x5
#define CPU_2_DDR_CLK_1x4_1	    0x6

/* Default values for CPU to Mbus-L DDR Interface Tick Driver and 	*/
/* CPU to Mbus-L Tick Sample fields in CPU config register		*/

#define TICK_DRV_1x1	0
#define TICK_DRV_1x2	0
#define TICK_DRV_1x3	1
#define TICK_DRV_1x4	2
#define TICK_SMPL_1x1	0
#define TICK_SMPL_1x2	1
#define TICK_SMPL_1x3	0
#define TICK_SMPL_1x4	0

#define CPU_2_MBUSL_DDR_CLK_1x2						\
		 ((TICK_DRV_1x2  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x2 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))
#define CPU_2_MBUSL_DDR_CLK_1x3						\
		 ((TICK_DRV_1x3  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x3 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))
#define CPU_2_MBUSL_DDR_CLK_1x4						\
		 ((TICK_DRV_1x4  << CCR_CPU_2_MBUSL_TICK_DRV_OFFS) | 	\
		  (TICK_SMPL_1x4 << CCR_CPU_2_MBUSL_TICK_SMPL_OFFS))

/* ARM Control and Status register */
/* CPU_CTRL_STAT_REG (CCSR) */


/*
This is used to block PCI express\PCI from access Socrates/Feroceon GP
while ARM boot is still in progress
*/

#define CCSR_PCI_ACCESS_OFFS			0
#define CCSR_PCI_ACCESS_MASK			BIT0
#define CCSR_PCI_ACCESS_ENABLE			(0 << CCSR_PCI_ACCESS_OFFS)
#define CCSR_PCI_ACCESS_DISBALE			(1 << CCSR_PCI_ACCESS_OFFS)

#define CCSR_ARM_RESET				BIT1
#define CCSR_SELF_INT				BIT2
#define CCSR_BIG_ENDIAN				BIT15


/* RSTOUTn Mask Register */
/* CPU_RSTOUTN_MASK_REG (CRMR) */

#define CRMR_PEX_RST_OUT_OFFS			0
#define CRMR_PEX_RST_OUT_MASK			BIT0
#define CRMR_PEX_RST_OUT_ENABLE			(1 << CRMR_PEX_RST_OUT_OFFS)
#define CRMR_PEX_RST_OUT_DISABLE		(0 << CRMR_PEX_RST_OUT_OFFS)

#define CRMR_WD_RST_OUT_OFFS			1
#define CRMR_WD_RST_OUT_MASK			BIT1
#define CRMR_WD_RST_OUT_ENABLE			(1 << CRMR_WD_RST_OUT_OFFS)
#define CRMR_WD_RST_OUT_DISBALE			(0 << CRMR_WD_RST_OUT_OFFS)			

#define CRMR_SOFT_RST_OUT_OFFS			2
#define CRMR_SOFT_RST_OUT_MASK			BIT2
#define CRMR_SOFT_RST_OUT_ENABLE		(1 << CRMR_SOFT_RST_OUT_OFFS)
#define CRMR_SOFT_RST_OUT_DISBALE		(0 << CRMR_SOFT_RST_OUT_OFFS)

/* System Software Reset Register */
/* CPU_SYS_SOFT_RST_REG (CSSRR) */

#define CSSRR_SYSTEM_SOFT_RST			BIT0

/* AHB to Mbus Bridge Interrupt Cause Register*/
/* CPU_AHB_MBUS_CAUSE_INT_REG (CAMCIR) */

#define CAMCIR_ARM_SELF_INT			BIT0
#define CAMCIR_ARM_TIMER0_INT_REQ		BIT1
#define CAMCIR_ARM_TIMER1_INT_REQ		BIT2
#define CAMCIR_ARM_WD_TIMER_INT_REQ		BIT3


/* AHB to Mbus Bridge Interrupt Mask Register*/
/* CPU_AHB_MBUS_MASK_INT_REG (CAMMIR) */

#define CAMCIR_ARM_SELF_INT_OFFS		0
#define CAMCIR_ARM_SELF_INT_MASK		BIT0
#define CAMCIR_ARM_SELF_INT_EN			(1 << CAMCIR_ARM_SELF_INT_OFFS)
#define CAMCIR_ARM_SELF_INT_DIS			(0 << CAMCIR_ARM_SELF_INT_OFFS)


#define CAMCIR_ARM_TIMER0_INT_REQ_OFFS		1
#define CAMCIR_ARM_TIMER0_INT_REQ_MASK		BIT1
#define CAMCIR_ARM_TIMER0_INT_REQ_EN		(1 << CAMCIR_ARM_TIMER0_INT_REQ_OFFS) 
#define CAMCIR_ARM_TIMER0_INT_REQ_DIS		(0 << CAMCIR_ARM_TIMER0_INT_REQ_OFFS)

#define CAMCIR_ARM_TIMER1_INT_REQ_OFFS		2
#define CAMCIR_ARM_TIMER1_INT_REQ_MASK		BIT2
#define CAMCIR_ARM_TIMER1_INT_REQ_EN		(1 << CAMCIR_ARM_TIMER1_INT_REQ_OFFS) 
#define CAMCIR_ARM_TIMER1_INT_REQ_DIS		(0 << CAMCIR_ARM_TIMER1_INT_REQ_OFFS) 

#define CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS 	3
#define CAMCIR_ARM_WD_TIMER_INT_REQ_MASK 	BIT3
#define CAMCIR_ARM_WD_TIMER_INT_REQ_EN	 	(1 << CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS) 
#define CAMCIR_ARM_WD_TIMER_INT_REQ_DIS	 	(0 << CAMCIR_ARM_WD_TIMER_INT_REQ_OFFS) 

/* CPU FTDLL Config register (CFCR) fields */
#define CFCR_FTDLL_ICACHE_TAG_OFFS		0
#define CFCR_FTDLL_ICACHE_TAG_MASK		(0x7F << CFCR_FTDLL_ICACHE_TAG_OFFS)
#define CFCR_FTDLL_DCACHE_TAG_OFFS		8
#define CFCR_FTDLL_DCACHE_TAG_MASK		(0x7F << CFCR_FTDLL_DCACHE_TAG_OFFS)
#define CFCR_FTDLL_OVERWRITE_ENABLE		(1 << 15)
/* For Orion 2 D2 only */
#define CFCR_MRVL_CPU_ID_OFFS			16
#define CFCR_MRVL_CPU_ID_MASK			(0x1 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_ARM_CPU_ID				(0x0 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_MRVL_CPU_ID			(0x1 << CFCR_MRVL_CPU_ID_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_OFFS		7
#define CFCR_VFP_SUB_ARC_NUM_MASK		(0x1 << CFCR_VFP_SUB_ARC_NUM_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_1			(0x0 << CFCR_VFP_SUB_ARC_NUM_OFFS)
#define CFCR_VFP_SUB_ARC_NUM_2			(0x1 << CFCR_VFP_SUB_ARC_NUM_OFFS)

/* CPU_L2_CONFIG_REG fields */
#ifdef MV_CPU_LE
#define CL2CR_L2_ECC_EN_OFFS			2
#define CL2CR_L2_WT_MODE_OFFS			4
#else
#define CL2CR_L2_ECC_EN_OFFS			26
#define CL2CR_L2_WT_MODE_OFFS			28
#endif

#define CL2CR_L2_ECC_EN_MASK			(1 << CL2CR_L2_ECC_EN_OFFS)
#define CL2CR_L2_WT_MODE_MASK			(1 << CL2CR_L2_WT_MODE_OFFS)

/*******************************************/
/* Main Interrupt Controller Registers Map */
/*******************************************/

#define CPU_MAIN_INT_CAUSE_REG			0x20200
#define CPU_MAIN_IRQ_MASK_REG			0x20204
#define CPU_MAIN_FIQ_MASK_REG			0x20208
#define CPU_ENPOINT_MASK_REG			0x2020C
#define CPU_MAIN_INT_CAUSE_HIGH_REG		0x20210
#define CPU_MAIN_IRQ_MASK_HIGH_REG		0x20214
#define CPU_MAIN_FIQ_MASK_HIGH_REG		0x20218
#define CPU_ENPOINT_MASK_HIGH_REG		0x2021C


/*******************************************/
/* ARM Doorbell Registers Map		   */
/*******************************************/

#define CPU_HOST_TO_ARM_DRBL_REG		0x20400
#define CPU_HOST_TO_ARM_MASK_REG		0x20404
#define CPU_ARM_TO_HOST_DRBL_REG		0x20408
#define CPU_ARM_TO_HOST_MASK_REG		0x2040C



/* CPU control register map */
/* Set bits means value is about to change according to new value */
#define CPU_CONFIG_DEFAULT_MASK         	(CCR_VEC_INIT_LOC_MASK  | CCR_AHB_ERROR_PROP_MASK)      

#define CPU_CONFIG_DEFAULT                      (CCR_VEC_INIT_LOC_FF00)
                 
/* CPU Control and status defaults */
#define CPU_CTRL_STAT_DEFAULT_MASK              (CCSR_PCI_ACCESS_MASK)
                                                                        

#define CPU_CTRL_STAT_DEFAULT                   (CCSR_PCI_ACCESS_ENABLE)

#endif /* __INCmvCpuIfRegsh */

