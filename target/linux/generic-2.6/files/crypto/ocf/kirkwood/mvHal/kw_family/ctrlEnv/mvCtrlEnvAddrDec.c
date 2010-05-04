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

/*******************************************************************************
* mvCtrlEnvAddrDec.h - Marvell controller address decode library
*
* DESCRIPTION:
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
#include "ctrlEnv/sys/mvAhbToMbusRegs.h"
#include "ddr2/mvDramIfRegs.h"
#include "pex/mvPexRegs.h"

#define MV_DEBUG

/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif
	
/* Default Attributes array */
MV_TARGET_ATTRIB	mvTargetDefaultsArray[] = TARGETS_DEF_ARRAY;
extern MV_TARGET 	*sampleAtResetTargetArray;
/* Dram\AHBToMbus\PEX share regsiter */

#define CTRL_DEC_BASE_OFFS		16
#define CTRL_DEC_BASE_MASK		(0xffff << CTRL_DEC_BASE_OFFS)
#define CTRL_DEC_BASE_ALIGNMENT	0x10000

#define CTRL_DEC_SIZE_OFFS		16
#define CTRL_DEC_SIZE_MASK		(0xffff << CTRL_DEC_SIZE_OFFS)
#define CTRL_DEC_SIZE_ALIGNMENT	0x10000

#define CTRL_DEC_WIN_EN			BIT0



/*******************************************************************************
* mvCtrlAddrDecToReg - Get address decode register format values
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/
MV_STATUS mvCtrlAddrDecToReg(MV_ADDR_WIN *pAddrDecWin, MV_DEC_REGS *pAddrDecRegs)
{

	MV_U32 baseToReg=0 , sizeToReg=0;
    
	/* BaseLow[31:16] => base register [31:16]		*/
	baseToReg = pAddrDecWin->baseLow & CTRL_DEC_BASE_MASK;

	/* Write to address decode Base Address Register                  */
	pAddrDecRegs->baseReg &= ~CTRL_DEC_BASE_MASK;
	pAddrDecRegs->baseReg |= baseToReg;

	/* Get size register value according to window size						*/
	sizeToReg = ctrlSizeToReg(pAddrDecWin->size, CTRL_DEC_SIZE_ALIGNMENT);
	
	/* Size parameter validity check.                                   */
	if (-1 == sizeToReg)
	{
		return MV_BAD_PARAM;
	}

	/* set size */
	pAddrDecRegs->sizeReg &= ~CTRL_DEC_SIZE_MASK;
	pAddrDecRegs->sizeReg |= (sizeToReg << CTRL_DEC_SIZE_OFFS);
	

	return MV_OK;

}

/*******************************************************************************
* mvCtrlRegToAddrDec - Extract address decode struct from registers.
*
* DESCRIPTION:
*       This function extract address decode struct from address decode 
*       registers given as parameters.
*
* INPUT:
*       pAddrDecRegs - Address decode register struct.
*
* OUTPUT:
*       pAddrDecWin - Target window data structure.
*
* RETURN:
*		MV_BAD_PARAM if address decode registers data is invalid.
*
*******************************************************************************/
MV_STATUS mvCtrlRegToAddrDec(MV_DEC_REGS *pAddrDecRegs, MV_ADDR_WIN *pAddrDecWin)
{
	MV_U32 sizeRegVal;
	
	sizeRegVal = (pAddrDecRegs->sizeReg & CTRL_DEC_SIZE_MASK) >> 
					CTRL_DEC_SIZE_OFFS;

	pAddrDecWin->size = ctrlRegToSize(sizeRegVal, CTRL_DEC_SIZE_ALIGNMENT);


	/* Extract base address						*/
	/* Base register [31:16] ==> baseLow[31:16] 		*/
	pAddrDecWin->baseLow = pAddrDecRegs->baseReg & CTRL_DEC_BASE_MASK;

	pAddrDecWin->baseHigh =  0;

	return MV_OK;
    
}

/*******************************************************************************
* mvCtrlAttribGet - 
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/

MV_STATUS mvCtrlAttribGet(MV_TARGET target,
						  MV_TARGET_ATTRIB *targetAttrib)
{

	targetAttrib->attrib = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].attrib;
	targetAttrib->targetId = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId;

	return MV_OK;

}

/*******************************************************************************
* mvCtrlGetAttrib - 
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/
MV_TARGET mvCtrlTargetGet(MV_TARGET_ATTRIB *targetAttrib)
{
	MV_TARGET target;
	MV_TARGET x;
	for (target = SDRAM_CS0; target < MAX_TARGETS ; target ++)
	{
		x = MV_CHANGE_BOOT_CS(target);
		if ((mvTargetDefaultsArray[x].attrib == targetAttrib->attrib) &&
			(mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId == targetAttrib->targetId))
		{
			/* found it */
			break;
		}
	}

	return target;
}

MV_STATUS mvCtrlAddrDecToParams(MV_DEC_WIN *pAddrDecWin, 
                                MV_DEC_WIN_PARAMS *pWinParam)
{
	MV_U32 baseToReg=0, sizeToReg=0;
    
	/* BaseLow[31:16] => base register [31:16]		*/
	baseToReg = pAddrDecWin->addrWin.baseLow & CTRL_DEC_BASE_MASK;

	/* Write to address decode Base Address Register                  */
	pWinParam->baseAddr &= ~CTRL_DEC_BASE_MASK;
	pWinParam->baseAddr |= baseToReg;

	/* Get size register value according to window size						*/
	sizeToReg = ctrlSizeToReg(pAddrDecWin->addrWin.size, CTRL_DEC_SIZE_ALIGNMENT);
	
	/* Size parameter validity check.                                   */
	if (-1 == sizeToReg)
	{
        mvOsPrintf("mvCtrlAddrDecToParams: ERR. ctrlSizeToReg failed.\n");
		return MV_BAD_PARAM;
	}
    pWinParam->size = sizeToReg;

    pWinParam->attrib   = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(pAddrDecWin->target)].attrib;
    pWinParam->targetId = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(pAddrDecWin->target)].targetId;

    return MV_OK;
}

MV_STATUS mvCtrlParamsToAddrDec(MV_DEC_WIN_PARAMS *pWinParam, 
                                MV_DEC_WIN *pAddrDecWin)
{
    MV_TARGET_ATTRIB    targetAttrib;

    pAddrDecWin->addrWin.baseLow = pWinParam->baseAddr;
	
	/* Upper 32bit address base is supported under PCI High Address remap */
	pAddrDecWin->addrWin.baseHigh = 0;	

	/* Prepare sizeReg to ctrlRegToSize function */
    pAddrDecWin->addrWin.size = ctrlRegToSize(pWinParam->size, CTRL_DEC_SIZE_ALIGNMENT);

	if (-1 == pAddrDecWin->addrWin.size)
	{
		DB(mvOsPrintf("mvCtrlParamsToAddrDec: ERR. ctrlRegToSize failed.\n"));
		return MV_BAD_PARAM;
	}
    targetAttrib.targetId = pWinParam->targetId;
    targetAttrib.attrib = pWinParam->attrib;

    pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

    return MV_OK;
}



