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

#ifndef __INCPCIIFREGSH
#define __INCPCIIFREGSH


/* defines */
#define MAX_PCI_DEVICES         32
#define MAX_PCI_FUNCS           8
#define MAX_PCI_BUSSES          128

/***************************************/
/* PCI Configuration registers */
/***************************************/

/*********************************************/
/* PCI Configuration, Function 0, Registers  */
/*********************************************/


/* Standard registers */
#define PCI_DEVICE_AND_VENDOR_ID					0x000
#define PCI_STATUS_AND_COMMAND						0x004
#define PCI_CLASS_CODE_AND_REVISION_ID			    0x008
#define PCI_BIST_HDR_TYPE_LAT_TMR_CACHE_LINE		0x00C
#define PCI_MEMORY_BAR_BASE_ADDR(barNum)		 	(0x010 + ((barNum) << 2))
#define PCI_SUBSYS_ID_AND_SUBSYS_VENDOR_ID		 	0x02C
#define PCI_EXPANSION_ROM_BASE_ADDR_REG			    0x030
#define PCI_CAPABILTY_LIST_POINTER			        0x034
#define PCI_INTERRUPT_PIN_AND_LINE					0x03C


/* PCI Device and Vendor ID Register (PDVIR) */
#define PDVIR_VEN_ID_OFFS			0 	/* Vendor ID */
#define PDVIR_VEN_ID_MASK			(0xffff << PDVIR_VEN_ID_OFFS)

#define PDVIR_DEV_ID_OFFS			16	/* Device ID */
#define PDVIR_DEV_ID_MASK  			(0xffff << PDVIR_DEV_ID_OFFS)

/* PCI Status and Command Register (PSCR) */
#define PSCR_IO_EN			BIT0 	/* IO Enable 							  */
#define PSCR_MEM_EN			BIT1	/* Memory Enable 						  */
#define PSCR_MASTER_EN		BIT2	/* Master Enable 						  */
#define PSCR_SPECIAL_EN		BIT3	/* Special Cycle Enable 				  */
#define PSCR_MEM_WRI_INV	BIT4	/* Memory Write and Invalidate Enable	  */
#define PSCR_VGA			BIT5	/* VGA Palette Snoops 					  */
#define PSCR_PERR_EN		BIT6	/* Parity Errors Respond Enable 		  */
#define PSCR_ADDR_STEP   	BIT7    /* Address Stepping Enable (Wait Cycle En)*/
#define PSCR_SERR_EN		BIT8	/* Ability to assert SERR# line			  */
#define PSCR_FAST_BTB_EN	BIT9	/* generate fast back-to-back transactions*/
#define PSCR_CAP_LIST		BIT20	/* Capability List Support 				  */
#define PSCR_66MHZ_EN		BIT21   /* 66 MHz Capable 						  */
#define PSCR_UDF_EN			BIT22   /* User definable features 				  */
#define PSCR_TAR_FAST_BB 	BIT23   /* fast back-to-back transactions capable */
#define PSCR_DATA_PERR		BIT24   /* Data Parity reported 				  */

#define PSCR_DEVSEL_TIM_OFFS 	25  /* DEVSEL timing */
#define PSCR_DEVSEL_TIM_MASK 	(0x3 << PSCR_DEVSEL_TIM_OFFS)
#define PSCR_DEVSEL_TIM_FAST	(0x0 << PSCR_DEVSEL_TIM_OFFS)
#define PSCR_DEVSEL_TIM_MED 	(0x1 << PSCR_DEVSEL_TIM_OFFS)
#define PSCR_DEVSEL_TIM_SLOW 	(0x2 << PSCR_DEVSEL_TIM_OFFS)

#define PSCR_SLAVE_TABORT	BIT27	/* Signalled Target Abort 	*/
#define PSCR_MASTER_TABORT	BIT28	/* Recieved Target Abort 	*/
#define PSCR_MABORT			BIT29	/* Recieved Master Abort 	*/
#define PSCR_SYSERR			BIT30	/* Signalled system error 	*/
#define PSCR_DET_PARERR		BIT31	/* Detect Parity Error 		*/

/* 	PCI configuration register offset=0x08 fields 
	(PCI_CLASS_CODE_AND_REVISION_ID)(PCCRI) 				*/

#define PCCRIR_REVID_OFFS		0		/* Revision ID */
#define PCCRIR_REVID_MASK		(0xff << PCCRIR_REVID_OFFS)

#define PCCRIR_FULL_CLASS_OFFS	8		/* Full Class Code */
#define PCCRIR_FULL_CLASS_MASK	(0xffffff << PCCRIR_FULL_CLASS_OFFS)

#define PCCRIR_PROGIF_OFFS		8		/* Prog .I/F*/
#define PCCRIR_PROGIF_MASK		(0xff << PCCRIR_PROGIF_OFFS)

#define PCCRIR_SUB_CLASS_OFFS	16		/* Sub Class*/
#define PCCRIR_SUB_CLASS_MASK	(0xff << PCCRIR_SUB_CLASS_OFFS)

#define PCCRIR_BASE_CLASS_OFFS	24		/* Base Class*/
#define PCCRIR_BASE_CLASS_MASK	(0xff << PCCRIR_BASE_CLASS_OFFS)

/* 	PCI configuration register offset=0x0C fields 
	(PCI_BIST_HEADER_TYPE_LATENCY_TIMER_CACHE_LINE)(PBHTLTCL) 				*/

#define PBHTLTCLR_CACHELINE_OFFS		0	/* Specifies the cache line size */
#define PBHTLTCLR_CACHELINE_MASK		(0xff << PBHTLTCLR_CACHELINE_OFFS)
	
#define PBHTLTCLR_LATTIMER_OFFS			8	/* latency timer */
#define PBHTLTCLR_LATTIMER_MASK			(0xff << PBHTLTCLR_LATTIMER_OFFS)

#define PBHTLTCLR_HEADTYPE_FULL_OFFS	16	/* Full Header Type */
#define PBHTLTCLR_HEADTYPE_FULL_MASK	(0xff << PBHTLTCLR_HEADTYPE_FULL_OFFS)

#define PBHTLTCLR_MULTI_FUNC			BIT23	/* Multi/Single function */

#define PBHTLTCLR_HEADER_OFFS			16		/* Header type */
#define PBHTLTCLR_HEADER_MASK			(0x7f << PBHTLTCLR_HEADER_OFFS)
#define PBHTLTCLR_HEADER_STANDARD		(0x0 << PBHTLTCLR_HEADER_OFFS)
#define PBHTLTCLR_HEADER_PCI2PCI_BRIDGE	(0x1 << PBHTLTCLR_HEADER_OFFS)


#define PBHTLTCLR_BISTCOMP_OFFS		24	/* BIST Completion Code */
#define PBHTLTCLR_BISTCOMP_MASK		(0xf << PBHTLTCLR_BISTCOMP_OFFS)

#define PBHTLTCLR_BISTACT			BIT30	/* BIST Activate bit */
#define PBHTLTCLR_BISTCAP			BIT31	/* BIST Capable Bit */


/* PCI Bar Base Low Register (PBBLR) */
#define PBBLR_IOSPACE			BIT0	/* Memory Space Indicator */

#define PBBLR_TYPE_OFFS			1	   /* BAR Type/Init Val. */ 
#define PBBLR_TYPE_MASK			(0x3 << PBBLR_TYPE_OFFS)
#define PBBLR_TYPE_32BIT_ADDR	(0x0 << PBBLR_TYPE_OFFS)
#define PBBLR_TYPE_64BIT_ADDR	(0x2 << PBBLR_TYPE_OFFS)

#define PBBLR_PREFETCH_EN		BIT3 	/* Prefetch Enable */

				
#define PBBLR_MEM_BASE_OFFS		4	/* Memory Bar Base address. Corresponds to
									address bits [31:4] */
#define PBBLR_MEM_BASE_MASK		(0xfffffff << PBBLR_MEM_BASE_OFFS)

#define PBBLR_IO_BASE_OFFS		2	/* IO Bar Base address. Corresponds to 
										address bits [31:2] */
#define PBBLR_IO_BASE_MASK		(0x3fffffff << PBBLR_IO_BASE_OFFS)


#define PBBLR_BASE_OFFS			12		/* Base address. Address bits [31:12] */
#define PBBLR_BASE_MASK			(0xfffff << PBBLR_BASE_OFFS)
#define PBBLR_BASE_ALIGNMET		(1 << PBBLR_BASE_OFFS)


/* PCI Bar Base High Fegister (PBBHR) */
#define PBBHR_BASE_OFFS			0		/* Base address. Address bits [31:12] */
#define PBBHR_BASE_MASK			(0xffffffff << PBBHR_BASE_OFFS)


/* 	PCI configuration register offset=0x2C fields 
	(PCI_SUBSYSTEM_ID_AND_SUBSYSTEM_VENDOR_ID)(PSISVI) 				*/

#define PSISVIR_VENID_OFFS	0	/* Subsystem Manufacturer Vendor ID Number */
#define PSISVIR_VENID_MASK	(0xffff << PSISVIR_VENID_OFFS)

#define PSISVIR_DEVID_OFFS	16	/* Subsystem Device ID Number */
#define PSISVIR_DEVID_MASK	(0xffff << PSISVIR_DEVID_OFFS)

/* 	PCI configuration register offset=0x30 fields 
	(PCI_EXPANSION_ROM_BASE_ADDR_REG)(PERBA) 				*/

#define PERBAR_EXPROMEN		BIT0	/* Expansion ROM Enable */

#define PERBAR_BASE_OFFS		12		/* Expansion ROM Base Address */
#define PERBAR_BASE_MASK		(0xfffff << PERBAR_BASE_OFFS) 	

/* 	PCI configuration register offset=0x34 fields 
	(PCI_CAPABILTY_LIST_POINTER)(PCLP) 				*/

#define PCLPR_CAPPTR_OFFS	0		/* Capability List Pointer */
#define PCLPR_CAPPTR_MASK	(0xff << PCLPR_CAPPTR_OFFS)

/* 	PCI configuration register offset=0x3C fields 
	(PCI_INTERRUPT_PIN_AND_LINE)(PIPL) 				*/

#define PIPLR_INTLINE_OFFS	0	/* Interrupt line (IRQ) */
#define PIPLR_INTLINE_MASK	(0xff << PIPLR_INTLINE_OFFS)

#define PIPLR_INTPIN_OFFS	8	/* interrupt pin (A,B,C,D) */
#define PIPLR_INTPIN_MASK	(0xff << PIPLR_INTPIN_OFFS)

#define PIPLR_MINGRANT_OFFS	16	/* Minimum Grant on 250 nano seconds units */
#define PIPLR_MINGRANT_MASK	(0xff << PIPLR_MINGRANT_OFFS)

#define PIPLR_MAXLATEN_OFFS	24	/* Maximum latency on 250 nano seconds units */
#define PIPLR_MAXLATEN_MASK	(0xff << PIPLR_MAXLATEN_OFFS)

#endif /* #ifndef __INCPCIIFREGSH */

