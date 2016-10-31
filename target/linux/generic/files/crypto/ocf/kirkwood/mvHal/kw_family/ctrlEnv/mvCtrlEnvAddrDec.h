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


#ifndef __INCmvCtrlEnvAddrDech
#define __INCmvCtrlEnvAddrDech

/* includes */
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"


/* defines  */
/* DUnit attributes */
#define ATMWCR_WIN_DUNIT_CS0_OFFS			0
#define ATMWCR_WIN_DUNIT_CS0_MASK			BIT0
#define ATMWCR_WIN_DUNIT_CS0_REQ			(0 << ATMWCR_WIN_DUNIT_CS0_OFFS)

#define ATMWCR_WIN_DUNIT_CS1_OFFS			1
#define ATMWCR_WIN_DUNIT_CS1_MASK			BIT1
#define ATMWCR_WIN_DUNIT_CS1_REQ 			(0 << ATMWCR_WIN_DUNIT_CS1_OFFS)

#define ATMWCR_WIN_DUNIT_CS2_OFFS			2
#define ATMWCR_WIN_DUNIT_CS2_MASK			BIT2
#define ATMWCR_WIN_DUNIT_CS2_REQ 			(0 << ATMWCR_WIN_DUNIT_CS2_OFFS)

#define ATMWCR_WIN_DUNIT_CS3_OFFS			3
#define ATMWCR_WIN_DUNIT_CS3_MASK			BIT3
#define ATMWCR_WIN_DUNIT_CS3_REQ 			(0 << ATMWCR_WIN_DUNIT_CS3_OFFS)

/* RUnit (Device)  attributes */
#define ATMWCR_WIN_RUNIT_DEVCS0_OFFS		0
#define ATMWCR_WIN_RUNIT_DEVCS0_MASK		BIT0
#define ATMWCR_WIN_RUNIT_DEVCS0_REQ			(0 << ATMWCR_WIN_RUNIT_DEVCS0_OFFS)

#define ATMWCR_WIN_RUNIT_DEVCS1_OFFS		1
#define ATMWCR_WIN_RUNIT_DEVCS1_MASK		BIT1
#define ATMWCR_WIN_RUNIT_DEVCS1_REQ 		(0 << ATMWCR_WIN_RUNIT_DEVCS1_OFFS)

#define ATMWCR_WIN_RUNIT_DEVCS2_OFFS		2
#define ATMWCR_WIN_RUNIT_DEVCS2_MASK		BIT2
#define ATMWCR_WIN_RUNIT_DEVCS2_REQ 		(0 << ATMWCR_WIN_RUNIT_DEVCS2_OFFS)

#define ATMWCR_WIN_RUNIT_BOOTCS_OFFS		4
#define ATMWCR_WIN_RUNIT_BOOTCS_MASK		BIT4
#define ATMWCR_WIN_RUNIT_BOOTCS_REQ 		(0 << ATMWCR_WIN_RUNIT_BOOTCS_OFFS)

/* LMaster (PCI)  attributes */
#define ATMWCR_WIN_LUNIT_BYTE_SWP_OFFS		0
#define ATMWCR_WIN_LUNIT_BYTE_SWP_MASK		BIT0
#define ATMWCR_WIN_LUNIT_BYTE_SWP			(0 << ATMWCR_WIN_LUNIT_BYTE_SWP_OFFS)
#define ATMWCR_WIN_LUNIT_BYTE_NO_SWP		(1 << ATMWCR_WIN_LUNIT_BYTE_SWP_OFFS)


#define ATMWCR_WIN_LUNIT_WORD_SWP_OFFS		1
#define ATMWCR_WIN_LUNIT_WORD_SWP_MASK		BIT1
#define ATMWCR_WIN_LUNIT_WORD_SWP			(0 << ATMWCR_WIN_LUNIT_WORD_SWP_OFFS)
#define ATMWCR_WIN_LUNIT_WORD_NO_SWP		(1 << ATMWCR_WIN_LUNIT_WORD_SWP_OFFS)

#define ATMWCR_WIN_LUNIT_NO_SNOOP			BIT2

#define ATMWCR_WIN_LUNIT_TYPE_OFFS			3
#define ATMWCR_WIN_LUNIT_TYPE_MASK			BIT3
#define ATMWCR_WIN_LUNIT_TYPE_IO			(0 << ATMWCR_WIN_LUNIT_TYPE_OFFS)
#define ATMWCR_WIN_LUNIT_TYPE_MEM			(1 << ATMWCR_WIN_LUNIT_TYPE_OFFS)

#define ATMWCR_WIN_LUNIT_FORCE64_OFFS		4
#define ATMWCR_WIN_LUNIT_FORCE64_MASK		BIT4
#define ATMWCR_WIN_LUNIT_FORCE64			(0 << ATMWCR_WIN_LUNIT_FORCE64_OFFS)

#define ATMWCR_WIN_LUNIT_ORDERING_OFFS		6
#define ATMWCR_WIN_LUNIT_ORDERING_MASK		BIT6
#define ATMWCR_WIN_LUNIT_ORDERING			(1 << ATMWCR_WIN_LUNIT_FORCE64_OFFS)

/* PEX Attributes */
#define ATMWCR_WIN_PEX_TYPE_OFFS			3
#define ATMWCR_WIN_PEX_TYPE_MASK			BIT3
#define ATMWCR_WIN_PEX_TYPE_IO				(0 << ATMWCR_WIN_PEX_TYPE_OFFS)
#define ATMWCR_WIN_PEX_TYPE_MEM				(1 << ATMWCR_WIN_PEX_TYPE_OFFS)

/* typedefs */

/* Unsupported attributes for address decode:                               */
/* 2) PCI0/1_REQ64n control                                                 */

typedef struct _mvDecRegs
{
	MV_U32 baseReg;
    MV_U32 baseRegHigh;
    MV_U32 sizeReg;

}MV_DEC_REGS;

typedef struct _mvTargetAttrib
{
	MV_U8			attrib;			/* chip select attributes */
	MV_TARGET_ID 		targetId; 		/* Target Id of this MV_TARGET */

}MV_TARGET_ATTRIB;


/* This structure describes address decode window                           */
typedef struct _mvDecWin 
{
    MV_TARGET       target;         /* Target for addr decode window        */
    MV_ADDR_WIN     addrWin;        /* Address window of target             */
    MV_BOOL     	enable;         /* Window enable/disable                */
}MV_DEC_WIN;

typedef struct _mvDecWinParams
{
    MV_TARGET_ID    targetId;   /* Target ID field */
    MV_U8           attrib;     /* Attribute field */
    MV_U32          baseAddr;   /* Base address in register format */
    MV_U32          size;       /* Size in register format */
}MV_DEC_WIN_PARAMS;


/* mvCtrlEnvAddrDec API list */

MV_STATUS mvCtrlAddrDecToReg(MV_ADDR_WIN *pAddrDecWin,
							 MV_DEC_REGS *pAddrDecRegs);

MV_STATUS mvCtrlRegToAddrDec(MV_DEC_REGS *pAddrDecRegs,
							 MV_ADDR_WIN *pAddrDecWin);

MV_STATUS mvCtrlAttribGet(MV_TARGET target,
						  MV_TARGET_ATTRIB *targetAttrib);

MV_TARGET mvCtrlTargetGet(MV_TARGET_ATTRIB *targetAttrib);


MV_STATUS mvCtrlAddrDecToParams(MV_DEC_WIN *pAddrDecWin, 
                                MV_DEC_WIN_PARAMS *pWinParam);

MV_STATUS mvCtrlParamsToAddrDec(MV_DEC_WIN_PARAMS *pWinParam, 
                                MV_DEC_WIN *pAddrDecWin);




#endif /* __INCmvCtrlEnvAddrDech */
