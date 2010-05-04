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


/* includes */

#include "ddr2/mvDramIf.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "ctrlEnv/sys/mvSysDram.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin);

/*******************************************************************************
* mvDramIfWinSet - Set DRAM interface address decode window
*
* DESCRIPTION: 
*       This function sets DRAM interface address decode window.
*
* INPUT:
*	    target      - System target. Use only SDRAM targets.
*       pAddrDecWin - SDRAM address window structure.
*
* OUTPUT:
*       None
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinSet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg=0,sizeReg=0;
	MV_U32 baseToReg=0 , sizeToReg=0;

    /* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinSet: target %d is not SDRAM\n", target);
		return MV_BAD_PARAM;
	}

    /* Check if the requested window overlaps with current enabled windows	*/
    if (MV_TRUE == sdramIfWinOverlap(target, &pAddrDecWin->addrWin))
	{
        mvOsPrintf("mvDramIfWinSet: ERR. Target %d overlaps\n", target);
		return MV_BAD_PARAM;
	}

	/* check if address is aligned to the size */
	if(MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size))
	{
		mvOsPrintf("mvDramIfWinSet:Error setting DRAM interface window %d."\
				   "\nAddress 0x%08x is unaligned to size 0x%x.\n",
                   target, 
				   pAddrDecWin->addrWin.baseLow,
				   pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	/* read base register*/
	baseReg = MV_REG_READ(SDRAM_BASE_ADDR_REG(0,target));

	/* read size register */
	sizeReg = MV_REG_READ(SDRAM_SIZE_REG(0,target));

	/* BaseLow[31:16] => base register [31:16]		*/
	baseToReg = pAddrDecWin->addrWin.baseLow & SCBAR_BASE_MASK;

	/* Write to address decode Base Address Register                  */
	baseReg &= ~SCBAR_BASE_MASK;
	baseReg |= baseToReg;

	/* Translate the given window size to register format			*/
	sizeToReg = ctrlSizeToReg(pAddrDecWin->addrWin.size, SCSR_SIZE_ALIGNMENT);

	/* Size parameter validity check.                                   */
	if (-1 == sizeToReg)
	{
		mvOsPrintf("mvCtrlAddrDecToReg: ERR. Win %d size invalid.\n",target);
		return MV_BAD_PARAM;
	}

	/* set size */
	sizeReg &= ~SCSR_SIZE_MASK;
	/* Size is located at upper 16 bits */
	sizeReg |= (sizeToReg << SCSR_SIZE_OFFS);

	/* enable/Disable */
	if (MV_TRUE == pAddrDecWin->enable)
	{
		sizeReg |= SCSR_WIN_EN;
	}
	else
	{
		sizeReg &= ~SCSR_WIN_EN;
	}

	/* 3) Write to address decode Base Address Register                   */
	MV_REG_WRITE(SDRAM_BASE_ADDR_REG(0,target), baseReg);

	/* Write to address decode Size Register                        	*/
	MV_REG_WRITE(SDRAM_SIZE_REG(0,target), sizeReg);
	
	return MV_OK;
}
/*******************************************************************************
* mvDramIfWinGet - Get DRAM interface address decode window
*
* DESCRIPTION: 
*       This function gets DRAM interface address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*       pAddrDecWin - SDRAM address window structure.
*
* RETURN:
*       MV_BAD_PARAM if parameters are invalid or window is invalid, MV_OK
*       otherwise.
*******************************************************************************/
MV_STATUS mvDramIfWinGet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin)
{
	MV_U32 baseReg,sizeReg;
	MV_U32 sizeRegVal;
	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinGet: target %d is Illigal\n", target);
		return MV_ERROR;
	}

	/* Read base and size registers */
	sizeReg = MV_REG_READ(SDRAM_SIZE_REG(0,target));
	baseReg = MV_REG_READ(SDRAM_BASE_ADDR_REG(0,target));

	sizeRegVal = (sizeReg & SCSR_SIZE_MASK) >> SCSR_SIZE_OFFS;

	pAddrDecWin->addrWin.size = ctrlRegToSize(sizeRegVal,
							SCSR_SIZE_ALIGNMENT);

    	/* Check if ctrlRegToSize returned OK */
	if (-1 == pAddrDecWin->addrWin.size)
	{
		mvOsPrintf("mvDramIfWinGet: size of target %d is Illigal\n", target);
		return MV_ERROR;
	}

	/* Extract base address						*/
	/* Base register [31:16] ==> baseLow[31:16] 		*/
	pAddrDecWin->addrWin.baseLow = baseReg & SCBAR_BASE_MASK;

	pAddrDecWin->addrWin.baseHigh =  0;


	if (sizeReg & SCSR_WIN_EN)
	{
		pAddrDecWin->enable = MV_TRUE;
	}
	else
	{
		pAddrDecWin->enable = MV_FALSE;			
	}

	return MV_OK;
}
/*******************************************************************************
* mvDramIfWinEnable - Enable/Disable SDRAM address decode window
*
* DESCRIPTION: 
*		This function enable/Disable SDRAM address decode window.
*
* INPUT:
*	    target - System target. Use only SDRAM targets.
*
* OUTPUT:
*		None.
*
* RETURN:
*		MV_ERROR in case function parameter are invalid, MV_OK otherewise.
*
*******************************************************************************/
MV_STATUS mvDramIfWinEnable(MV_TARGET target, MV_BOOL enable)
{
	MV_DRAM_DEC_WIN 	addrDecWin;

	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(target))
	{
		mvOsPrintf("mvDramIfWinEnable: target %d is Illigal\n", target);
		return MV_ERROR;
	}

	if (enable == MV_TRUE) 
	{   /* First check for overlap with other enabled windows				*/
		if (MV_OK != mvDramIfWinGet(target, &addrDecWin))
		{
			mvOsPrintf("mvDramIfWinEnable:ERR. Getting target %d failed.\n", 
                                                                        target);
			return MV_ERROR;
		}
		/* Check for overlapping */
		if (MV_FALSE == sdramIfWinOverlap(target, &(addrDecWin.addrWin)))
		{
			/* No Overlap. Enable address decode winNum window              */
			MV_REG_BIT_SET(SDRAM_SIZE_REG(0,target), SCSR_WIN_EN);
		}
		else
		{   /* Overlap detected	*/
			mvOsPrintf("mvDramIfWinEnable: ERR. Target %d overlap detect\n",
                                                                        target);
			return MV_ERROR;
		}
	}
	else
	{   /* Disable address decode winNum window                             */
		MV_REG_BIT_RESET(SDRAM_SIZE_REG(0, target), SCSR_WIN_EN);
	}

	return MV_OK;
}

/*******************************************************************************
* sdramIfWinOverlap - Check if an address window overlap an SDRAM address window
*
* DESCRIPTION:
*		This function scan each SDRAM address decode window to test if it 
*		overlapps the given address windoow 
*
* INPUT:
*       target      - SDRAM target where the function skips checking.
*       pAddrDecWin - The tested address window for overlapping with 
*					  SDRAM windows.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlaps any enabled address
*       decode map, MV_FALSE otherwise.
*
*******************************************************************************/
static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin)
{
	MV_TARGET	targetNum;
	MV_DRAM_DEC_WIN 	addrDecWin;
	
	for(targetNum = SDRAM_CS0; targetNum < MV_DRAM_MAX_CS ; targetNum++)
	{
		/* don't check our winNum or illegal targets */
		if (targetNum == target)
		{
			continue;
		}
		
		/* Get window parameters 	*/
		if (MV_OK != mvDramIfWinGet(targetNum, &addrDecWin))
		{
			mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
			return MV_ERROR;
		}
	
		/* Do not check disabled windows	*/
		if (MV_FALSE == addrDecWin.enable)
		{
			continue;
		}
	
		if(MV_TRUE == ctrlWinOverlapTest(pAddrWin, &addrDecWin.addrWin))
		{                    
			mvOsPrintf(
			"sdramIfWinOverlap: Required target %d overlap winNum %d\n", 
			target, targetNum);
			return MV_TRUE;           
		}
	}
	
	return MV_FALSE;
}

