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

#ifndef __INCPEXREGSH
#define __INCPEXREGSH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */
#define MAX_PEX_DEVICES         32
#define MAX_PEX_FUNCS           8
#define MAX_PEX_BUSSES          256



/*********************************************************/
/* PCI Express Configuration Cycles Generation Registers */
/*********************************************************/

#define PEX_CFG_ADDR_REG(pexIf)		((PEX_IF_BASE(pexIf)) + 0x18F8)
#define PEX_CFG_DATA_REG(pexIf)		((PEX_IF_BASE(pexIf)) + 0x18FC)
#define PEX_PHY_ACCESS_REG(pexIf)	((PEX_IF_BASE(pexIf)) + 0x1B00)
/* PCI Express Configuration Address Register */
/* PEX_CFG_ADDR_REG (PXCAR)*/

#define PXCAR_REG_NUM_OFFS			2
#define PXCAR_REG_NUM_MAX			0x3F
#define PXCAR_REG_NUM_MASK			(PXCAR_REG_NUM_MAX << PXCAR_REG_NUM_OFFS)
#define PXCAR_FUNC_NUM_OFFS			8
#define PXCAR_FUNC_NUM_MAX			0x7
#define PXCAR_FUNC_NUM_MASK			(PXCAR_FUNC_NUM_MAX << PXCAR_FUNC_NUM_OFFS)
#define PXCAR_DEVICE_NUM_OFFS		11
#define PXCAR_DEVICE_NUM_MAX		0x1F
#define PXCAR_DEVICE_NUM_MASK		(PXCAR_DEVICE_NUM_MAX << PXCAR_DEVICE_NUM_OFFS)
#define PXCAR_BUS_NUM_OFFS			16
#define PXCAR_BUS_NUM_MAX			0xFF
#define PXCAR_BUS_NUM_MASK			(PXCAR_BUS_NUM_MAX << PXCAR_BUS_NUM_OFFS)
#define PXCAR_EXT_REG_NUM_OFFS		24
#define PXCAR_EXT_REG_NUM_MAX		0xF

/* in pci express register address is now the legacy register address (8 bits)
with the new extended register address (more 4 bits) , below is the mask of
the upper 4 bits of the full register address */

#define PXCAR_REAL_EXT_REG_NUM_OFFS	8
#define PXCAR_EXT_REG_NUM_MASK		(PXCAR_EXT_REG_NUM_MAX << PXCAR_EXT_REG_NUM_OFFS)
#define PXCAR_CONFIG_EN				BIT31

#define PXCAR_REAL_EXT_REG_NUM_OFFS     8
#define PXCAR_REAL_EXT_REG_NUM_MASK     (0xF << PXCAR_REAL_EXT_REG_NUM_OFFS)

/* The traditional PCI spec defined 6-bit field to describe register offset.*/ 
/* The new PCI Express extend the register offset by an extra 4-bits.       */
/* The below macro assign 10-bit register offset into the apprpreate        */
/* fields in the CFG_ADDR_REG                                               */
#define PXCAR_REG_OFFS_SET(regOffs)                         \
 ( (regOff & PXCAR_REG_NUM_MASK) | \
   ( ((regOff & PXCAR_REAL_EXT_REG_NUM_MASK) >> PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS) )

/***********************************/
/* PCI Express Interrupt registers */
/***********************************/
#define PEX_CAUSE_REG(pexIf)		((PEX_IF_BASE(pexIf)) + 0x1900)
#define PEX_MASK_REG(pexIf)		((PEX_IF_BASE(pexIf)) + 0x1910)

#define PXICR_TX_REQ_IN_DLDOWN_ERR  BIT0  /* Transmit request while field   */
                                          /* <DLDown> of the PCI Express    */
/* PCI Express Interrupt Cause */
/* PEX_INT_CAUSE_REG  (PXICR)*/
/* PEX_INT_MASK_REG*/
/*
NOTE:All bits except bits[27:24] are Read/Write Clear only. A cause bit sets
upon an error event occurrence. A write of 0 clears the bit. A write of 1 has
no affect. Bits[24:27} are set and cleared upon reception of interrupt
emulation messages.

Mask bit per cause bit. If a bit is set to 1, the corresponding event is
enabled. Mask does not affect setting of the Interrupt Cause register bits;
it only affects the assertion of the interrupt .*/
                                          

#define PXICR_MDIS_CAUSE			BIT1  /* Attempt to generate PCI transaction 
                                             while master is disabled */
#define PXICR_ERR_WRTO_REG_CAUSE	BIT3  /* Erroneous write attempt to
                                             PCI Express internal register*/
#define PXICR_HIT_DFLT_WIN_ERR		BIT4  /* Hit Default Window Error */
#define PXICR_RX_RAM_PAR_ERR        BIT6  /* Rx RAM Parity Error */
#define PXICR_TX_RAM_PAR_ERR        BIT7  /* Tx RAM Parity Error */
#define PXICR_COR_ERR_DET			BIT8  /* Correctable Error Detected*/
#define PXICR_NF_ERR_DET			BIT9  /* Non-Fatal Error Detected*/
#define PXICR_FERR_DET				BIT10 /* Fatal Error Detected*/
#define PXICR_DSTATE_CHANGE			BIT11 /* Dstate Change Indication*/
#define PXICR_BIST					BIT12 /* PCI-Express BIST activated*/
#define PXICR_FLW_CTRL_PROT     BIT14 /* Flow Control Protocol Error */

#define PXICR_RCV_UR_CA_ERR         BIT15 /* Received UR or CA status. */
#define PXICR_RCV_ERR_FATAL			BIT16 /* Received ERR_FATAL message.*/
#define PXICR_RCV_ERR_NON_FATAL		BIT17 /* Received ERR_NONFATAL message*/
#define PXICR_RCV_ERR_COR			BIT18 /* Received ERR_COR message.*/
#define PXICR_RCV_CRS				BIT19 /* Received CRS completion status*/
#define PXICR_SLV_HOT_RESET			BIT20 /* Received Hot Reset Indication*/
#define PXICR_SLV_DIS_LINK			BIT21 /* Slave Disable Link Indication*/
#define PXICR_SLV_LB				BIT22 /* Slave Loopback Indication*/
#define PXICR_LINK_FAIL				BIT23 /* Link Failure indication.*/
#define PXICR_RCV_INTA				BIT24 /* IntA status.*/
#define PXICR_RCV_INTB				BIT25 /* IntB status.*/
#define PXICR_RCV_INTC				BIT26 /* IntC status.*/
#define PXICR_RCV_INTD				BIT27 /* IntD status.*/
#define PXICR_RCV_PM_PME            BIT28 /* Received PM_PME message. */


/********************************************/
/* PCI Express Control and Status Registers */
/********************************************/
#define PEX_CTRL_REG(pexIf)				((PEX_IF_BASE(pexIf)) + 0x1A00)
#define PEX_STATUS_REG(pexIf)				((PEX_IF_BASE(pexIf)) + 0x1A04)
#define PEX_COMPLT_TMEOUT_REG(pexIf)			((PEX_IF_BASE(pexIf)) + 0x1A10)
#define PEX_PWR_MNG_EXT_REG(pexIf)			((PEX_IF_BASE(pexIf)) + 0x1A18)
#define PEX_FLOW_CTRL_REG(pexIf)			((PEX_IF_BASE(pexIf)) + 0x1A20)
#define PEX_ACK_TMR_4X_REG(pexIf)			((PEX_IF_BASE(pexIf)) + 0x1A30)
#define PEX_ACK_TMR_1X_REG(pexIf)			((PEX_IF_BASE(pexIf)) + 0x1A40)
#define PEX_TL_CTRL_REG(pexIf)				((PEX_IF_BASE(pexIf)) + 0x1AB0)


#define PEX_RAM_PARITY_CTRL_REG(pexIf)  		((PEX_IF_BASE(pexIf)) + 0x1A50)
/* PCI Express Control Register */
/* PEX_CTRL_REG (PXCR) */

#define PXCR_CONF_LINK_OFFS             0
#define PXCR_CONF_LINK_MASK             (1 << PXCR_CONF_LINK_OFFS)
#define PXCR_CONF_LINK_X4               (0 << PXCR_CONF_LINK_OFFS)
#define PXCR_CONF_LINK_X1               (1 << PXCR_CONF_LINK_OFFS)
#define PXCR_DEV_TYPE_CTRL_OFFS			1     /*PCI ExpressDevice Type Control*/
#define PXCR_DEV_TYPE_CTRL_MASK			BIT1
#define PXCR_DEV_TYPE_CTRL_CMPLX		(1 << PXCR_DEV_TYPE_CTRL_OFFS)
#define PXCR_DEV_TYPE_CTRL_POINT		(0 << PXCR_DEV_TYPE_CTRL_OFFS)
#define PXCR_CFG_MAP_TO_MEM_EN			BIT2  /* Configuration Header Mapping 
											   to Memory Space Enable         */

#define PXCR_CFG_MAP_TO_MEM_EN			BIT2 /* Configuration Header Mapping 
											   to Memory Space Enable*/

#define PXCR_RSRV1_OFFS					5
#define PXCR_RSRV1_MASK					(0x7 << PXCR_RSRV1_OFFS)
#define PXCR_RSRV1_VAL					(0x0 << PXCR_RSRV1_OFFS)

#define PXCR_CONF_MAX_OUTSTND_OFFS		8 /*Maximum outstanding NP requests as a master*/
#define PXCR_CONF_MAX_OUTSTND_MASK		(0x3 << PXCR_CONF_MAX_OUTSTND_OFFS)


#define PXCR_CONF_NFTS_OFFS				16 /*number of FTS Ordered-Sets*/
#define PXCR_CONF_NFTS_MASK				(0xff << PXCR_CONF_NFTS_OFFS)

#define PXCR_CONF_MSTR_HOT_RESET		BIT24 /*Master Hot-Reset.*/
#define PXCR_CONF_MSTR_LB				BIT26 /* Master Loopback */
#define PXCR_CONF_MSTR_DIS_SCRMB		BIT27 /* Master Disable Scrambling*/
#define PXCR_CONF_DIRECT_DIS_SCRMB		BIT28 /* Direct Disable Scrambling*/

/* PCI Express Status Register */
/* PEX_STATUS_REG (PXSR) */

#define PXSR_DL_DOWN					BIT0 /* DL_Down indication.*/

#define PXSR_PEX_BUS_NUM_OFFS			8 /* Bus Number Indication */
#define PXSR_PEX_BUS_NUM_MASK			(0xff << PXSR_PEX_BUS_NUM_OFFS)

#define PXSR_PEX_DEV_NUM_OFFS			16 /* Device Number Indication */
#define PXSR_PEX_DEV_NUM_MASK			(0x1f << PXSR_PEX_DEV_NUM_OFFS)

#define PXSR_PEX_SLV_HOT_RESET			BIT24 /* Slave Hot Reset Indication*/
#define PXSR_PEX_SLV_DIS_LINK			BIT25 /* Slave Disable Link Indication*/
#define PXSR_PEX_SLV_LB					BIT26 /* Slave Loopback Indication*/
#define PXSR_PEX_SLV_DIS_SCRMB			BIT27 /* Slave Disable Scrambling Indication*/


/* PCI Express Completion Timeout Register */
/* PEX_COMPLT_TMEOUT_REG (PXCTR)*/

#define PXCTR_CMP_TO_THRSHLD_OFFS		0 /* Completion Timeout Threshold */
#define PXCTR_CMP_TO_THRSHLD_MASK		(0xffff << PXCTR_CMP_TO_THRSHLD_OFFS)

/* PCI Express Power Management Extended Register */
/* PEX_PWR_MNG_EXT_REG (PXPMER) */

#define PXPMER_L1_ASPM_EN_OFFS			1
#define PXPMER_L1_ASPM_EN_MASK			(0x1 << PXPMER_L1_ASPM_EN_OFFS)

/* PCI Express Flow Control Register */
/* PEX_FLOW_CTRL_REG (PXFCR)*/

#define PXFCR_PH_INIT_FC_OFFS			0 /*Posted Headers Flow Control Credit
										    Initial Value.*/
#define PXFCR_PH_INIT_FC_MASK			(0xff << PXFCR_PH_INIT_FC_OFFS)


#define PXFCR_NPH_INIT_FC_OFFS			8 /* Classified Non-Posted Headers
											 Flow Control Credit Initial Value*/
#define PXFCR_NPH_INIT_FC_MASK			(0xff << PXFCR_NPH_INIT_FC_OFFS)

#define PXFCR_CH_INIT_FC_OFFS			16 /* Completion Headers Flow Control
											  Credit Initial Value Infinite*/
										
#define PXFCR_CH_INIT_FC_MASK			(0xff << PXFCR_CH_INIT_FC_OFFS)

#define PXFCR_FC_UPDATE_TO_OFFS			24 /* Flow Control Update Timeout */
#define PXFCR_FC_UPDATE_TO_MASK			(0xff << PXFCR_FC_UPDATE_TO_OFFS)

/* PCI Express Acknowledge Timers (4X) Register */
/* PEX_ACK_TMR_4X_REG (PXAT4R) */
#define PXAT1R_ACK_LAT_TOX4_OFFS		0  /* Ack Latency Timer Timeout Value */
#define PXAT1R_ACK_LAT_TOX4_MASK		(0xffff << PXAT4R_ACK_LAT_TOX1_OFFS)
#define PXAT1R_ACK_RPLY_TOX4_OFFS		16 /* Ack Replay Timer Timeout Value  */
#define PXAT1R_ACK_RPLY_TOX4_MASK		(0xffff << PXAT1R_ACK_RPLY_TOX1_OFFS)

/* PCI Express Acknowledge Timers (1X) Register */
/* PEX_ACK_TMR_1X_REG (PXAT1R) */

#define PXAT1R_ACK_LAT_TOX1_OFFS		0 /* Acknowledge Latency Timer Timeout
										     Value for 1X Link*/
#define PXAT1R_ACK_LAT_TOX1_MASK		(0xffff << PXAT1R_ACK_LAT_TOX1_OFFS)

#define PXAT1R_ACK_RPLY_TOX1_OFFS		16 /* Acknowledge Replay Timer Timeout
											  Value for 1X*/
#define PXAT1R_ACK_RPLY_TOX1_MASK		(0xffff << PXAT1R_ACK_RPLY_TOX1_OFFS)


/* PCI Express TL Control Register */
/* PEX_TL_CTRL_REG (PXTCR) */

#define PXTCR_TX_CMP_BUFF_NO_OFFS		8 /*Number of completion buffers in Tx*/
#define PXTCR_TX_CMP_BUFF_NO_MASK		(0xf << PXTCR_TX_CMP_BUFF_NO_OFFS)

/* PCI Express Debug MAC Control Register */
/* PEX_DEBUG_MAC_CTRL_REG (PXDMCR) */

#define PXDMCR_LINKUP					BIT4



/**********************************************/
/* PCI Express Configuration Header Registers */
/**********************************************/
#define PEX_CFG_DIRECT_ACCESS(pexIf,cfgReg)	((PEX_IF_BASE(pexIf)) + (cfgReg))

#define PEX_DEVICE_AND_VENDOR_ID					0x000
#define PEX_STATUS_AND_COMMAND						0x004
#define PEX_CLASS_CODE_AND_REVISION_ID			    0x008
#define PEX_BIST_HDR_TYPE_LAT_TMR_CACHE_LINE		0x00C
#define PEX_MEMORY_BAR_BASE_ADDR(barNum)		 	(0x010 + ((barNum) << 2))
#define PEX_MV_BAR_BASE(barNum)						(0x010 + (barNum) * 8)
#define PEX_MV_BAR_BASE_HIGH(barNum)				(0x014 + (barNum) * 8)
#define PEX_BAR0_INTER_REG							0x010
#define PEX_BAR0_INTER_REG_HIGH						0x014
#define PEX_BAR1_REG								0x018
#define PEX_BAR1_REG_HIGH							0x01C
#define PEX_BAR2_REG								0x020
#define PEX_BAR2_REG_HIGH							0x024

#define PEX_SUBSYS_ID_AND_SUBSYS_VENDOR_ID			0x02C
#define PEX_EXPANSION_ROM_BASE_ADDR_REG				0x030
#define PEX_CAPABILTY_LIST_POINTER					0x034
#define PEX_INTERRUPT_PIN_AND_LINE					0x03C

/* capability list */
#define PEX_POWER_MNG_CAPABILITY		            0x040
#define PEX_POWER_MNG_STATUS_CONTROL		        0x044

#define PEX_MSI_MESSAGE_CONTROL						0x050
#define PEX_MSI_MESSAGE_ADDR		                0x054
#define PEX_MSI_MESSAGE_HIGH_ADDR		            0x058
#define PEX_MSI_MESSAGE_DATA		                0x05C

#define PEX_CAPABILITY_REG							0x60
#define PEX_DEV_CAPABILITY_REG						0x64
#define PEX_DEV_CTRL_STAT_REG						0x68
#define PEX_LINK_CAPABILITY_REG						0x6C
#define PEX_LINK_CTRL_STAT_REG						0x70

#define PEX_ADV_ERR_RPRT_HDR_TRGT_REG				0x100
#define PEX_UNCORRECT_ERR_STAT_REG					0x104
#define PEX_UNCORRECT_ERR_MASK_REG					0x108
#define PEX_UNCORRECT_ERR_SERVITY_REG				0x10C
#define PEX_CORRECT_ERR_STAT_REG					0x110
#define PEX_CORRECT_ERR_MASK_REG					0x114
#define PEX_ADV_ERR_CAPABILITY_CTRL_REG				0x118
#define PEX_HDR_LOG_FIRST_DWORD_REG					0x11C
#define PEX_HDR_LOG_SECOND_DWORD_REG				0x120
#define PEX_HDR_LOG_THIRD_DWORD_REG					0x124
#define PEX_HDR_LOG_FOURTH_DWORD_REG				0x128



/* PCI Express Device and Vendor ID Register*/
/*PEX_DEVICE_AND_VENDOR_ID (PXDAVI)*/

#define PXDAVI_VEN_ID_OFFS			0 	/* Vendor ID */
#define PXDAVI_VEN_ID_MASK			(0xffff << PXDAVI_VEN_ID_OFFS)

#define PXDAVI_DEV_ID_OFFS			16	/* Device ID */
#define PXDAVI_DEV_ID_MASK  		(0xffff << PXDAVI_DEV_ID_OFFS)


/* PCI Express Command and Status Register*/
/*PEX_STATUS_AND_COMMAND (PXSAC)*/

#define PXSAC_IO_EN			BIT0 	/* IO Enable 							  */
#define PXSAC_MEM_EN		BIT1	/* Memory Enable 						  */
#define PXSAC_MASTER_EN		BIT2	/* Master Enable 						  */
#define PXSAC_PERR_EN		BIT6	/* Parity Errors Respond Enable 		  */
#define PXSAC_SERR_EN		BIT8	/* Ability to assert SERR# line			  */
#define PXSAC_INT_DIS		BIT10   /* Interrupt Disable 					  */
#define PXSAC_INT_STAT		BIT19   /* Interrupt Status 			*/
#define PXSAC_CAP_LIST		BIT20	/* Capability List Support 				  */
#define PXSAC_MAS_DATA_PERR	BIT24   /* Master Data Parity Error				  */
#define PXSAC_SLAVE_TABORT	BIT27	/* Signalled Target Abort 	*/
#define PXSAC_RT_ABORT		BIT28	/* Recieved Target Abort 	*/
#define PXSAC_MABORT			BIT29	/* Recieved Master Abort 	*/
#define PXSAC_SYSERR			BIT30	/* Signalled system error 	*/
#define PXSAC_DET_PARERR		BIT31	/* Detect Parity Error 		*/


/* PCI Express Class Code and Revision ID Register*/
/*PEX_CLASS_CODE_AND_REVISION_ID (PXCCARI)*/       

#define PXCCARI_REVID_OFFS		0		/* Revision ID */
#define PXCCARI_REVID_MASK		(0xff << PXCCARI_REVID_OFFS)

#define PXCCARI_FULL_CLASS_OFFS	8		/* Full Class Code */
#define PXCCARI_FULL_CLASS_MASK	(0xffffff << PXCCARI_FULL_CLASS_OFFS)

#define PXCCARI_PROGIF_OFFS		8		/* Prog .I/F*/
#define PXCCARI_PROGIF_MASK		(0xff << PXCCARI_PROGIF_OFFS)

#define PXCCARI_SUB_CLASS_OFFS	16		/* Sub Class*/
#define PXCCARI_SUB_CLASS_MASK	(0xff << PXCCARI_SUB_CLASS_OFFS)

#define PXCCARI_BASE_CLASS_OFFS	24		/* Base Class*/
#define PXCCARI_BASE_CLASS_MASK	(0xff << PXCCARI_BASE_CLASS_OFFS)


/* PCI Express BIST, Header Type and Cache Line Size Register*/
/*PEX_BIST_HDR_TYPE_LAT_TMR_CACHE_LINE (PXBHTLTCL)*/ 

#define PXBHTLTCL_CACHELINE_OFFS		0	/* Specifies the cache line size */
#define PXBHTLTCL_CACHELINE_MASK		(0xff << PXBHTLTCL_CACHELINE_OFFS)

#define PXBHTLTCL_HEADTYPE_FULL_OFFS	16	/* Full Header Type */
#define PXBHTLTCL_HEADTYPE_FULL_MASK	(0xff << PXBHTLTCL_HEADTYPE_FULL_OFFS)

#define PXBHTLTCL_MULTI_FUNC			BIT23	/* Multi/Single function */

#define PXBHTLTCL_HEADER_OFFS			16		/* Header type */
#define PXBHTLTCL_HEADER_MASK			(0x7f << PXBHTLTCL_HEADER_OFFS)
#define PXBHTLTCL_HEADER_STANDARD		(0x0 << PXBHTLTCL_HEADER_OFFS)
#define PXBHTLTCL_HEADER_PCI2PCI_BRIDGE	(0x1 << PXBHTLTCL_HEADER_OFFS)


#define PXBHTLTCL_BISTCOMP_OFFS		24	/* BIST Completion Code */
#define PXBHTLTCL_BISTCOMP_MASK		(0xf << PXBHTLTCL_BISTCOMP_OFFS)

#define PXBHTLTCL_BISTACT			BIT30	/* BIST Activate bit */
#define PXBHTLTCL_BISTCAP			BIT31	/* BIST Capable Bit */
#define PXBHTLTCL_BISTCAP_OFFS		31	
#define PXBHTLTCL_BISTCAP_MASK		BIT31	
#define PXBHTLTCL_BISTCAP_VAL		0


/* PCI Express Subsystem Device and Vendor ID */
/*PEX_SUBSYS_ID_AND_SUBSYS_VENDOR_ID (PXSIASVI)*/

#define PXSIASVI_VENID_OFFS	0	/* Subsystem Manufacturer Vendor ID Number */
#define PXSIASVI_VENID_MASK	(0xffff << PXSIASVI_VENID_OFFS)

#define PXSIASVI_DEVID_OFFS	16	/* Subsystem Device ID Number */
#define PXSIASVI_DEVID_MASK	(0xffff << PXSIASVI_DEVID_OFFS)


/* PCI Express Capability List Pointer Register*/
/*PEX_CAPABILTY_LIST_POINTER (PXCLP)*/

#define PXCLP_CAPPTR_OFFS	0		/* Capability List Pointer */
#define PXCLP_CAPPTR_MASK	(0xff << PXCLP_CAPPTR_OFFS)

/* PCI Express Interrupt Pin and Line Register */
/*PEX_INTERRUPT_PIN_AND_LINE (PXIPAL)*/

#define PXIPAL_INTLINE_OFFS	0	/* Interrupt line (IRQ) */
#define PXIPAL_INTLINE_MASK	(0xff << PXIPAL_INTLINE_OFFS)

#define PXIPAL_INTPIN_OFFS	8	/* interrupt pin (A,B,C,D) */
#define PXIPAL_INTPIN_MASK	(0xff << PXIPAL_INTPIN_OFFS)


/* PCI Express Power Management Capability Header Register*/
/*PEX_POWER_MNG_CAPABILITY (PXPMC)*/

#define PXPMC_CAP_ID_OFFS		0 /* Capability ID */
#define PXPMC_CAP_ID_MASK		(0xff << PXPMC_CAP_ID_OFFS)

#define PXPMC_NEXT_PTR_OFFS		8 /* Next Item Pointer */
#define PXPMC_NEXT_PTR_MASK		(0xff << PXPMC_NEXT_PTR_OFFS)

#define PXPMC_PMC_VER_OFFS		16 /* PCI Power Management Capability Version*/
#define PXPMC_PMC_VER_MASK		(0x7 << PXPMC_PMC_VER_OFFS)

#define PXPMC_DSI 				BIT21/* Device Specific Initialization */

#define PXPMC_AUX_CUR_OFFS		22 /* Auxiliary Current Requirements */
#define PXPMC_AUX_CUR_MASK		(0x7 << PXPMC_AUX_CUR_OFFS)

#define PXPMC_D1_SUP 			BIT25 /* D1 Power Management support*/

#define PXPMC_D2_SUP 			BIT26 /* D2 Power Management support*/

#define PXPMC_PME_SUP_OFFS		27 /* PM Event generation support*/
#define PXPMC_PME_SUP_MASK		(0x1f << PXPMC_PME_SUP_OFFS)

/* PCI Express Power Management Control and Status Register*/
/*PEX_POWER_MNG_STATUS_CONTROL (PXPMSC)*/

#define PXPMSC_PM_STATE_OFFS	0	/* Power State */
#define PXPMSC_PM_STATE_MASK	(0x3 << PXPMSC_PM_STATE_OFFS)
#define PXPMSC_PM_STATE_D0		(0x0 << PXPMSC_PM_STATE_OFFS)
#define PXPMSC_PM_STATE_D1		(0x1 << PXPMSC_PM_STATE_OFFS)
#define PXPMSC_PM_STATE_D2		(0x2 << PXPMSC_PM_STATE_OFFS)
#define PXPMSC_PM_STATE_D3		(0x3 << PXPMSC_PM_STATE_OFFS)

#define PXPMSC_PME_EN			BIT8/* PM_PME Message Generation Enable */

#define PXPMSC_PM_DATA_SEL_OFFS	9	/* Data Select*/
#define PXPMSC_PM_DATA_SEL_MASK	(0xf << PXPMSC_PM_DATA_SEL_OFFS)

#define PXPMSC_PM_DATA_SCALE_OFFS	13	/* Data Scale */
#define PXPMSC_PM_DATA_SCALE_MASK	(0x3 << PXPMSC_PM_DATA_SCALE_OFFS)

#define PXPMSC_PME_STAT				BIT15/* PME Status */

#define PXPMSC_PM_DATA_OFFS			24		/* State Data */
#define PXPMSC_PM_DATA_MASK			(0xff << PXPMSC_PM_DATA_OFFS)


/* PCI Express MSI Message Control Register*/								 
/*PEX_MSI_MESSAGE_CONTROL (PXMMC)*/

#define PXMMC_CAP_ID_OFFS			0 /* Capability ID */
#define PXMMC_CAP_ID_MASK			(0xff << PXMMC_CAP_ID_OFFS)

#define PXMMC_NEXT_PTR_OFFS			8 /* Next Item Pointer */
#define PXMMC_NEXT_PTR_MASK			(0xff << PXMMC_NEXT_PTR_OFFS)

#define PXMMC_MSI_EN				BIT18 /* MSI Enable */

#define PXMMC_MULTI_CAP_OFFS		17 /* Multiple Message Capable */
#define PXMMC_MULTI_CAP_MASK		(0x7 << PXMMC_MULTI_CAP_OFFS)

#define PXMMC_MULTI_EN_OFFS			20  /* Multiple Messages Enable */
#define PXMMC_MULTI_EN_MASK			(0x7 << PXMMC_MULTI_EN_OFFS)

#define PXMMC_ADDR64				BIT23	/* 64-bit Addressing Capable */


/* PCI Express MSI Message Address Register*/
/*PEX_MSI_MESSAGE_ADDR (PXMMA)*/

#define PXMMA_MSI_ADDR_OFFS			2 /* Message Address  corresponds to 
										Address[31:2] of the MSI MWr TLP*/
#define PXMMA_MSI_ADDR_MASK			(0x3fffffff << PXMMA_MSI_ADDR_OFFS)


/* PCI Express MSI Message Address (High) Register */
/*PEX_MSI_MESSAGE_HIGH_ADDR (PXMMHA)*/

#define PXMMA_MSI_ADDR_H_OFFS		0 /* Message Upper Address corresponds to 
											Address[63:32] of the MSI MWr TLP*/
#define PXMMA_MSI_ADDR_H_MASK		(0xffffffff << PXMMA_MSI_ADDR_H_OFFS )


/* PCI Express MSI Message Data Register*/
/*PEX_MSI_MESSAGE_DATA (PXMMD)*/

#define PXMMD_MSI_DATA_OFFS 		0 /* Message Data */
#define PXMMD_MSI_DATA_MASK 		(0xffff << PXMMD_MSI_DATA_OFFS )


/* PCI Express Capability Register*/								 
/*PEX_CAPABILITY_REG (PXCR)*/

#define PXCR_CAP_ID_OFFS			0	/* Capability ID*/
#define PXCR_CAP_ID_MASK			(0xff << PXCR_CAP_ID_OFFS)

#define PXCR_NEXT_PTR_OFFS			8 /* Next Item Pointer*/
#define PXCR_NEXT_PTR_MASK			(0xff << PXCR_NEXT_PTR_OFFS)

#define PXCR_CAP_VER_OFFS			16 /* Capability Version*/
#define PXCR_CAP_VER_MASK			(0xf << PXCR_CAP_VER_OFFS)

#define PXCR_DEV_TYPE_OFFS			20 /*  Device/Port Type*/
#define PXCR_DEV_TYPE_MASK			(0xf << PXCR_DEV_TYPE_OFFS)

#define PXCR_SLOT_IMP 				BIT24 /* Slot Implemented*/

#define PXCR_INT_MSG_NUM_OFFS		25 /* Interrupt Message Number*/
#define PXCR_INT_MSG_NUM_MASK		(0x1f << PXCR_INT_MSG_NUM_OFFS)


/* PCI Express Device Capabilities Register */
/*PEX_DEV_CAPABILITY_REG (PXDCR)*/

#define PXDCR_MAX_PLD_SIZE_SUP_OFFS			0 /* Maximum Payload Size Supported*/
#define PXDCR_MAX_PLD_SIZE_SUP_MASK			(0x7 << PXDCR_MAX_PLD_SIZE_SUP_OFFS)

#define PXDCR_EP_L0S_ACC_LAT_OFFS			6/* Endpoint L0s Acceptable Latency*/
#define PXDCR_EP_L0S_ACC_LAT_MASK			(0x7 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_64NS_LESS		(0x0 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_64NS_128NS		(0x1 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_128NS_256NS	(0x2 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_256NS_512NS	(0x3 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_512NS_1US		(0x4 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_1US_2US		(0x5 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_2US_4US		(0x6 << PXDCR_EP_L0S_ACC_LAT_OFFS)
#define PXDCR_EP_L0S_ACC_LAT_4US_MORE		(0x7 << PXDCR_EP_L0S_ACC_LAT_OFFS)

#define PXDCR_EP_L1_ACC_LAT_OFFS 			9 /* Endpoint L1 Acceptable Latency*/
#define PXDCR_EP_L1_ACC_LAT_MASK			(0x7 << PXDCR_EP_L1_ACC_LAT_OFFS)
#define PXDCR_EP_L1_ACC_LAT_64NS_LESS       (0x0 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_64NS_128NS      (0x1 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_128NS_256NS     (0x2 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_256NS_512NS     (0x3 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_512NS_1US       (0x4 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_1US_2US         (0x5 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_2US_4US         (0x6 << PXDCR_EP_L1_ACC_LAT_OFFS) 
#define PXDCR_EP_L1_ACC_LAT_4US_MORE        (0x7 << PXDCR_EP_L1_ACC_LAT_OFFS) 


#define PXDCR_ATT_BUT_PRS_OFFS				12 /* Attention Button Present*/
#define PXDCR_ATT_BUT_PRS_MASK				BIT12
#define PXDCR_ATT_BUT_PRS_IMPLEMENTED		BIT12

#define PXDCR_ATT_IND_PRS_OFFS				13 /* Attention Indicator Present*/
#define PXDCR_ATT_IND_PRS_MASK				BIT13
#define PXDCR_ATT_IND_PRS_IMPLEMENTED		BIT13

#define PXDCR_PWR_IND_PRS_OFFS        		14/* Power Indicator Present*/
#define PXDCR_PWR_IND_PRS_MASK       		BIT14
#define PXDCR_PWR_IND_PRS_IMPLEMENTED		BIT14

#define PXDCR_CAP_SPL_VAL_OFFS				18 /*Captured Slot Power Limit 
												 Value*/
#define PXDCR_CAP_SPL_VAL_MASK				(0xff << PXDCR_CAP_SPL_VAL_OFFS)

#define PXDCR_CAP_SP_LSCL_OFFS				26 /* Captured Slot Power Limit
												  Scale */
#define PXDCR_CAP_SP_LSCL_MASK				(0x3 << PXDCR_CAP_SP_LSCL_OFFS)

/* PCI Express Device Control Status Register */
/*PEX_DEV_CTRL_STAT_REG (PXDCSR)*/

#define PXDCSR_COR_ERR_REP_EN		BIT0 /* Correctable Error Reporting Enable*/
#define PXDCSR_NF_ERR_REP_EN		BIT1 /* Non-Fatal Error Reporting Enable*/
#define PXDCSR_F_ERR_REP_EN			BIT2 /* Fatal Error Reporting Enable*/
#define PXDCSR_UR_REP_EN			BIT3 /* Unsupported Request (UR) 
													Reporting Enable*/
#define PXDCSR_EN_RO 				BIT4 /* Enable Relaxed Ordering*/

#define PXDCSR_MAX_PLD_SZ_OFFS		5	 /* Maximum Payload Size*/
#define PXDCSR_MAX_PLD_SZ_MASK		(0x7 << PXDCSR_MAX_PLD_SZ_OFFS)
#define PXDCSR_MAX_PLD_SZ_128B		(0x0 << PXDCSR_MAX_PLD_SZ_OFFS)
#define PXDCSR_EN_NS				BIT11  /* Enable No Snoop*/

#define PXDCSR_MAX_RD_RQ_SZ_OFFS	12 /* Maximum Read Request Size*/
#define PXDCSR_MAX_RD_RQ_SZ_MASK	(0x7 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_128B	(0x0 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_256B	(0x1 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_512B	(0x2 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_1KB		(0x3 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_2KB		(0x4 << PXDCSR_MAX_RD_RQ_SZ_OFFS)
#define PXDCSR_MAX_RD_RQ_SZ_4KB		(0x5 << PXDCSR_MAX_RD_RQ_SZ_OFFS)

#define PXDCSR_COR_ERR_DET 			BIT16 /* Correctable Error Detected*/
#define PXDCSR_NF_ERR_DET 			BIT17 /* Non-Fatal Error Detected.*/
#define PXDCSR_F_ERR_DET 			BIT18 /* Fatal Error Detected.*/
#define PXDCSR_UR_DET				BIT19 /* Unsupported Request Detected */
#define PXDCSR_AUX_PWR_DET 			BIT20 /* Reserved*/

#define PXDCSR_TRANS_PEND_OFFS 			21 /* Transactions Pending*/
#define PXDCSR_TRANS_PEND_MASK 			BIT21
#define PXDCSR_TRANS_PEND_NOT_COMPLETED (0x1 << PXDCSR_TRANS_PEND_OFFS)


/* PCI Express Link Capabilities Register*/
/*PEX_LINK_CAPABILITY_REG (PXLCR)*/

#define PXLCR_MAX_LINK_SPD_OFFS		0 /* Maximum Link Speed*/
#define PXLCR_MAX_LINK_SPD_MASK		(0xf << PXLCR_MAX_LINK_SPD_OFFS)

#define PXLCR_MAX_LNK_WDTH_OFFS 	3 /* Maximum Link Width*/
#define PXLCR_MAX_LNK_WDTH_MASK		(0x3f << PXLCR_MAX_LNK_WDTH_OFFS)

#define PXLCR_ASPM_SUP_OFFS 		10 /* Active State Link PM Support*/
#define PXLCR_ASPM_SUP_MASK			(0x3 << PXLCR_ASPM_SUP_OFFS)

#define PXLCR_L0S_EXT_LAT_OFFS 			12 /* L0s Exit Latency*/
#define PXLCR_L0S_EXT_LAT_MASK			(0x7 << PXLCR_L0S_EXT_LAT_OFFS)
#define PXLCR_L0S_EXT_LAT_64NS_LESS     (0x0 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_64NS_128NS   	(0x1 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_128NS_256NS   (0x2 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_256NS_512NS   (0x3 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_512NS_1US     (0x4 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_1US_2US       (0x5 << PXDCR_EP_L1_ACC_LAT_OFFS)  
#define PXLCR_L0S_EXT_LAT_2US_4US       (0x6 << PXDCR_EP_L1_ACC_LAT_OFFS)  

#define PXLCR_POR_TNUM_OFFS 			24 /* Port Number */
#define PXLCR_POR_TNUM_MASK				(0xff << PXLCR_POR_TNUM_OFFS)

/* PCI Express Link Control Status Register */
/*PEX_LINK_CTRL_STAT_REG (PXLCSR)*/

#define PXLCSR_ASPM_CNT_OFFS			0 /* Active State Link PM Control */
#define PXLCSR_ASPM_CNT_MASK			(0x3 << PXLCSR_ASPM_CNT_OFFS)
#define PXLCSR_ASPM_CNT_DISABLED		(0x0 << PXLCSR_ASPM_CNT_OFFS)
#define PXLCSR_ASPM_CNT_L0S_ENT_SUPP		(0x1 << PXLCSR_ASPM_CNT_OFFS)
#define PXLCSR_ASPM_CNT_L1S_ENT_SUPP		(0x2 << PXLCSR_ASPM_CNT_OFFS)
#define PXLCSR_ASPM_CNT_L0S_L1S_ENT_SUPP	(0x3 << PXLCSR_ASPM_CNT_OFFS)

#define PXLCSR_RCB_OFFS				3 /* Read Completion Boundary */
#define PXLCSR_RCB_MASK				BIT3
#define PXLCSR_RCB_64B				(0 << PXLCSR_RCB_OFFS)
#define PXLCSR_RCB_128B				(1 << PXLCSR_RCB_OFFS)

#define PXLCSR_LNK_DIS 				BIT4 /* Link Disable */
#define PXLCSR_RETRN_LNK 			BIT5 /* Retrain Link */
#define PXLCSR_CMN_CLK_CFG			BIT6 /* Common Clock Configuration */
#define PXLCSR_EXTD_SNC 			BIT7 /* Extended Sync */

#define PXLCSR_LNK_SPD_OFFS 		16 /* Link Speed */
#define PXLCSR_LNK_SPD_MASK			(0xf << PXLCSR_LNK_SPD_OFFS)

#define PXLCSR_NEG_LNK_WDTH_OFFS	20  /* Negotiated Link Width */
#define PXLCSR_NEG_LNK_WDTH_MASK 	(0x3f << PXLCSR_NEG_LNK_WDTH_OFFS)
#define PXLCSR_NEG_LNK_WDTH_X1		(0x1 << PXLCSR_NEG_LNK_WDTH_OFFS)

#define PXLCSR_LNK_TRN 				BIT27 /* Link Training */

#define PXLCSR_SLT_CLK_CFG_OFFS		28 /* Slot Clock Configuration */
#define PXLCSR_SLT_CLK_CFG_MASK		BIT28
#define PXLCSR_SLT_CLK_CFG_INDPNT	(0x0 << PXLCSR_SLT_CLK_CFG_OFFS)
#define PXLCSR_SLT_CLK_CFG_REF		(0x1 << PXLCSR_SLT_CLK_CFG_OFFS)
								
/* PCI Express Advanced Error Report Header Register */								 
/*PEX_ADV_ERR_RPRT_HDR_TRGT_REG (PXAERHTR)*/

/* PCI Express Uncorrectable Error Status Register*/
/*PEX_UNCORRECT_ERR_STAT_REG (PXUESR)*/

/* PCI Express Uncorrectable Error Mask Register */
/*PEX_UNCORRECT_ERR_MASK_REG (PXUEMR)*/

/* PCI Express Uncorrectable Error Severity Register */
/*PEX_UNCORRECT_ERR_SERVITY_REG (PXUESR)*/

/* PCI Express Correctable Error Status Register */
/*PEX_CORRECT_ERR_STAT_REG (PXCESR)*/

/* PCI Express Correctable Error Mask Register */
/*PEX_CORRECT_ERR_MASK_REG (PXCEMR)*/

/* PCI Express Advanced Error Capability and Control Register*/
/*PEX_ADV_ERR_CAPABILITY_CTRL_REG (PXAECCR)*/

/* PCI Express Header Log First DWORD Register*/
/*PEX_HDR_LOG_FIRST_DWORD_REG (PXHLFDR)*/

/* PCI Express Header Log Second DWORD Register*/
/*PEX_HDR_LOG_SECOND_DWORD_REG (PXHLSDR)*/

/* PCI Express Header Log Third DWORD Register*/
/*PEX_HDR_LOG_THIRD_DWORD_REG (PXHLTDR)*/

/* PCI Express Header Log Fourth DWORD Register*/
/*PEX_HDR_LOG_FOURTH_DWORD_REG (PXHLFDR)*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef __INCPEXREGSH */


