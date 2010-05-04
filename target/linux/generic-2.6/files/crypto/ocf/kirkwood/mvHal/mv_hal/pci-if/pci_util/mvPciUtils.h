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

#ifndef __INCmvPciUtilsh
#define __INCmvPciUtilsh

/* 
This module only support scanning of Header type 00h of pci devices 
There is no suppotr for Header type 01h of pci devices  ( PCI bridges )
*/

/* includes */
#include "mvSysHwConfig.h"
#include "pci-if/mvPciIf.h"
#include "pci/mvPciRegs.h"



/* PCI base address low bar mask */
#define PCI_ERROR_CODE                      0xffffffff

#define PCI_BRIDGE_CLASS					0x6
#define P2P_BRIDGE_SUB_CLASS_CODE			0x4


#define P2P_BUSSES_NUM						0x18
#define P2P_IO_BASE_LIMIT_SEC_STATUS		0x1C
#define P2P_MEM_BASE_LIMIT					0x20
#define P2P_PREF_MEM_BASE_LIMIT				0x24
#define P2P_PREF_BASE_UPPER_32				0x28
#define P2P_PREF_LIMIT_UPPER_32				0x2C
#define P2P_IO_BASE_LIMIT_UPPER_16			0x30
#define P2P_EXP_ROM							0x38

/* P2P_BUSSES_NUM  (PBM) */

#define PBM_PRIME_BUS_NUM_OFFS				0
#define PBM_PRIME_BUS_NUM_MASK				(0xff << PBM_PRIME_BUS_NUM_OFFS)

#define PBM_SEC_BUS_NUM_OFFS				8
#define PBM_SEC_BUS_NUM_MASK				(0xff << PBM_SEC_BUS_NUM_OFFS)

#define PBM_SUB_BUS_NUM_OFFS				16
#define PBM_SUB_BUS_NUM_MASK				(0xff << PBM_SUB_BUS_NUM_OFFS)

#define PBM_SEC_LAT_TMR_OFFS				24
#define PBM_SEC_LAT_TMR_MASK				(0xff << PBM_SEC_LAT_TMR_OFFS)

/* P2P_IO_BASE_LIMIT_SEC_STATUS (PIBLSS) */

#define PIBLSS_IO_BASE_OFFS					0
#define PIBLSS_IO_BASE_MASK					(0xff << PIBLSS_IO_BASE_OFFS)

#define PIBLSS_ADD_CAP_OFFS					0
#define PIBLSS_ADD_CAP_MASK 				(0x3 << PIBLSS_ADD_CAP_OFFS)
#define PIBLSS_ADD_CAP_16BIT 				(0x0 << PIBLSS_ADD_CAP_OFFS)
#define PIBLSS_ADD_CAP_32BIT 				(0x1 << PIBLSS_ADD_CAP_OFFS)

#define PIBLSS_LOW_ADDR_OFFS				0
#define PIBLSS_LOW_ADDR_MASK				(0xFFF << PIBLSS_LOW_ADDR_OFFS)

#define PIBLSS_HIGH_ADDR_OFFS				12
#define PIBLSS_HIGH_ADDR_MASK				(0xF << PIBLSS_HIGH_ADDR_OFFS)

#define PIBLSS_IO_LIMIT_OFFS				8
#define PIBLSS_IO_LIMIT_MASK				(0xff << PIBLSS_IO_LIMIT_OFFS)

#define PIBLSS_SEC_STATUS_OFFS				16
#define PIBLSS_SEC_STATUS_MASK				(0xffff << PIBLSS_SEC_STATUS_OFFS)


/* P2P_MEM_BASE_LIMIT (PMBL)*/

#define PMBL_MEM_BASE_OFFS					0
#define PMBL_MEM_BASE_MASK					(0xffff << PMBL_MEM_BASE_OFFS)

#define PMBL_MEM_LIMIT_OFFS					16
#define PMBL_MEM_LIMIT_MASK					(0xffff << PMBL_MEM_LIMIT_OFFS)


#define PMBL_LOW_ADDR_OFFS					0
#define PMBL_LOW_ADDR_MASK					(0xFFFFF << PMBL_LOW_ADDR_OFFS)

#define PMBL_HIGH_ADDR_OFFS					20
#define PMBL_HIGH_ADDR_MASK					(0xFFF << PMBL_HIGH_ADDR_OFFS)


/* P2P_PREF_MEM_BASE_LIMIT (PRMBL) */

#define PRMBL_PREF_MEM_BASE_OFFS			0
#define PRMBL_PREF_MEM_BASE_MASK			(0xffff << PRMBL_PREF_MEM_BASE_OFFS)

#define PRMBL_PREF_MEM_LIMIT_OFFS			16
#define PRMBL_PREF_MEM_LIMIT_MASK			(0xffff<<PRMBL_PREF_MEM_LIMIT_OFFS)

#define PRMBL_LOW_ADDR_OFFS					0
#define PRMBL_LOW_ADDR_MASK					(0xFFFFF << PRMBL_LOW_ADDR_OFFS)

#define PRMBL_HIGH_ADDR_OFFS				20
#define PRMBL_HIGH_ADDR_MASK				(0xFFF << PRMBL_HIGH_ADDR_OFFS)

#define PRMBL_ADD_CAP_OFFS					0
#define PRMBL_ADD_CAP_MASK					(0xf << PRMBL_ADD_CAP_OFFS)
#define PRMBL_ADD_CAP_32BIT					(0x0 << PRMBL_ADD_CAP_OFFS)
#define PRMBL_ADD_CAP_64BIT					(0x1 << PRMBL_ADD_CAP_OFFS)

/* P2P_IO_BASE_LIMIT_UPPER_16 (PIBLU) */

#define PRBU_IO_UPP_BASE_OFFS				0
#define PRBU_IO_UPP_BASE_MASK				(0xffff << PRBU_IO_UPP_BASE_OFFS)

#define PRBU_IO_UPP_LIMIT_OFFS				16
#define PRBU_IO_UPP_LIMIT_MASK				(0xffff << PRBU_IO_UPP_LIMIT_OFFS)


/* typedefs */

typedef enum _mvPciBarMapping
{
    PCI_MEMORY_BAR, 
    PCI_IO_BAR, 
    PCI_NO_MAPPING
}MV_PCI_BAR_MAPPING;

typedef enum _mvPciBarType
{
    PCI_32BIT_BAR, 
    PCI_64BIT_BAR
}MV_PCI_BAR_TYPE;

typedef enum _mvPciIntPin
{
    MV_PCI_INTA = 1,
    MV_PCI_INTB = 2,
    MV_PCI_INTC = 3,
    MV_PCI_INTD = 4
}MV_PCI_INT_PIN;

typedef enum _mvPciHeader
{
    MV_PCI_STANDARD,
    MV_PCI_PCI2PCI_BRIDGE

}MV_PCI_HEADER;


/* BAR structure */
typedef struct _pciBar
{
    MV_U32 barOffset;
    MV_U32 barBaseLow;
    MV_U32 barBaseHigh;
    MV_U32 barSizeLow;
    MV_U32 barSizeHigh;
    /* The 'barBaseAddr' is a 64-bit variable
       that will contain the TOTAL base address
       value achived by combining both the 'barBaseLow'
       and the 'barBaseHigh' parameters as follows:

       BIT: 63          31         0
            |           |         |
            barBaseHigh barBaseLow */
    MV_U64 barBaseAddr;
    /* The 'barSize' is a 64-bit variable
       that will contain the TOTAL size achived
       by combining both the 'barSizeLow' and
       the 'barSizeHigh' parameters as follows:

       BIT: 63          31         0
            |           |         |
            barSizeHigh barSizeLow

       NOTE: The total size described above
             is AFTER the size calculation as
             described in PCI spec rev2.2 */
    MV_U64 barSize;
    MV_BOOL            isPrefetchable;
    MV_PCI_BAR_TYPE       barType;
    MV_PCI_BAR_MAPPING    barMapping;


} PCI_BAR;

/* Device information structure */
typedef struct _mvPciDevice
{
    /* Device specific information */
	MV_U32			busNumber; 	/* Pci agent bus number */
    MV_U32			deviceNum;	/* Pci agent device number */
    MV_U32			function;	/* Pci agent function number */

	MV_U32			venID;		/* Pci agent Vendor ID */
    MV_U32			deviceID;	/* Pci agent Device ID */

    MV_BOOL			isFastB2BCapable;	/* Capability of Fast Back to Back 
										   transactions */
	MV_BOOL			isCapListSupport;	/* Support of Capability list */
	MV_BOOL			is66MHZCapable;		/* 66MHZ support */

    MV_U32			baseClassCode;		/* Pci agent base Class Code */
    MV_U32			subClassCode;		/* Pci agent sub Class Code */
    MV_U32			progIf;				/* Pci agent Programing interface */
	MV_U32			revisionID;

    PCI_BAR			pciBar[6]; 			/* Pci agent bar list */

	MV_U32			p2pPrimBusNum;		/* P2P Primary Bus number*/
	MV_U32			p2pSecBusNum;		/* P2P Secondary Bus Number*/
	MV_U32			p2pSubBusNum;		/* P2P Subordinate bus Number */
	MV_U32			p2pSecLatencyTimer;	/* P2P Econdary Latency Timer*/
	MV_U32			p2pIObase;			/* P2P IO Base */
	MV_U32			p2pIOLimit;			/* P2P IO Linit */
	MV_BOOL			bIO32;	
	MV_U32			p2pSecStatus;		/* P2P Secondary Status */
	MV_U32			p2pMemBase;			/* P2P Memory Space */
	MV_U32			p2pMemLimit;		/* P2P Memory Limit*/
	MV_U32			p2pPrefMemBase;		/* P2P Prefetchable Mem Base*/
	MV_U32			p2pPrefMemLimit;	/* P2P Prefetchable Memory Limit*/
	MV_BOOL			bPrefMem64;	
	MV_U32			p2pPrefBaseUpper32Bits;/* P2P Prefetchable upper 32 bits*/
	MV_U32			p2pPrefLimitUpper32Bits;/* P2P prefetchable limit upper 32*/


	MV_U32			pciCacheLine;		/* Pci agent cache line */
	MV_U32			pciLatencyTimer;	/* Pci agent Latency timer  */
    MV_PCI_HEADER	pciHeader;			/* Pci agent header type*/
    MV_BOOL			isMultiFunction;	/* Multi function support */
	MV_BOOL			isBISTCapable;		/* Self test capable */

	MV_U32			subSysID;			/* Sub System ID */
	MV_U32			subSysVenID;		/* Sub System Vendor ID */

	MV_BOOL			isExpRom;			/* Expantion Rom support */
	MV_U32			expRomAddr;			/* Expantion Rom pointer */

	MV_U32			capListPointer;		/* Capability list pointer */

	MV_U32			irqLine;		/* IRQ line  */
	MV_PCI_INT_PIN	intPin;			/* Interrupt pin */
	MV_U32			minGrant;		/* Minimum grant*/
	MV_U32			maxLatency;		/* Maximum latency*/

	MV_U32 			funtionsNum;	/* pci agent total functions number */

	MV_U32 			barsNum;
    MV_U8           type[60];		/* class name of the pci agent */


} MV_PCI_DEVICE;

/* PCI gloabl functions */
MV_STATUS mvPciClassNameGet(MV_U32 classCode, MV_8 *pType);


/* Performs a full scan on both PCIs and returns all possible details on the
   agents found on the bus. */
MV_STATUS mvPciScan(MV_U32 pciIf,
					MV_PCI_DEVICE *pPciAgents,
					MV_U32 *pPciAgentsNum);


#endif /* #ifndef __INCmvPciUtilsh */
