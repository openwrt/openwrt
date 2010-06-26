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
#include "ctrlEnv/sys/mvAhbToMbus.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

#undef MV_DEBUG
/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif

/* typedefs */


/* CPU address remap registers offsets are inconsecutive. This struct 		*/
/* describes address remap register offsets									*/
typedef struct _ahbToMbusRemapRegOffs
{
    MV_U32 lowRegOffs;		/* Low 32-bit remap register offset			*/
    MV_U32 highRegOffs;		/* High 32 bit remap register offset		*/
}AHB_TO_MBUS_REMAP_REG_OFFS;

/* locals   */
static MV_STATUS ahbToMbusRemapRegOffsGet	(MV_U32 winNum,
										AHB_TO_MBUS_REMAP_REG_OFFS *pRemapRegs);

/*******************************************************************************
* mvAhbToMbusInit - Initialize Ahb To Mbus Address Map !
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK laways.
*
*******************************************************************************/
MV_STATUS mvAhbToMbusInit(void)
{
	return MV_OK;

}

/*******************************************************************************
* mvAhbToMbusWinSet - Set CPU-to-peripheral winNum address window
*
* DESCRIPTION:
*       This function sets
*       address window, also known as address decode window.
*       A new address decode window is set for specified winNum address window.
*       If address decode window parameter structure enables the window,
*       the routine will also enable the winNum window, allowing CPU to access
*       the winNum window.
*
* INPUT:
*       winNum      - Windows number.
*       pAddrDecWin - CPU winNum window data structure.
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_OK if CPU winNum window was set correctly, MV_ERROR in case of
*       address window overlapps with other active CPU winNum window or
*		trying to assign 36bit base address while CPU does not support that.
*       The function returns MV_NOT_SUPPORTED, if the winNum is unsupported.
*
*******************************************************************************/
MV_STATUS mvAhbToMbusWinSet(MV_U32 winNum, MV_AHB_TO_MBUS_DEC_WIN *pAddrDecWin)
{
	MV_TARGET_ATTRIB targetAttribs;
	MV_DEC_REGS decRegs;

	/* Parameter checking   */
	if (winNum >= MAX_AHB_TO_MBUS_WINS)
	{
		mvOsPrintf("mvAhbToMbusWinSet: ERR. Invalid winNum %d\n", winNum);
		return MV_NOT_SUPPORTED;
	}


	/* read base register*/
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		decRegs.baseReg = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(winNum));
	}
	else
	{
		decRegs.baseReg = MV_REG_READ(AHB_TO_MBUS_WIN_INTEREG_REG);
	}

	/* check if address is aligned to the size */
	if(MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size))
	{
		mvOsPrintf("mvAhbToMbusWinSet:Error setting AHB to MBUS window %d to "\
				   "target %s.\nAddress 0x%08x is unaligned to size 0x%x.\n",
				   winNum,
				   mvCtrlTargetNameGet(pAddrDecWin->target),
				   pAddrDecWin->addrWin.baseLow,
				   pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	/* read control register*/
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		decRegs.sizeReg = MV_REG_READ(AHB_TO_MBUS_WIN_CTRL_REG(winNum));
	}

	if (MV_OK != mvCtrlAddrDecToReg(&(pAddrDecWin->addrWin),&decRegs))
	{
		mvOsPrintf("mvAhbToMbusWinSet:mvCtrlAddrDecToReg Failed\n");
		return MV_ERROR;
	}

	/* enable\Disable */
	if (MV_TRUE == pAddrDecWin->enable)
	{
		decRegs.sizeReg |= ATMWCR_WIN_ENABLE;
	}
	else
	{
		decRegs.sizeReg &= ~ATMWCR_WIN_ENABLE;
	}

	mvCtrlAttribGet(pAddrDecWin->target,&targetAttribs);

	/* set attributes */
	decRegs.sizeReg &= ~ATMWCR_WIN_ATTR_MASK;
	decRegs.sizeReg |= targetAttribs.attrib << ATMWCR_WIN_ATTR_OFFS;
	/* set target ID */
	decRegs.sizeReg &= ~ATMWCR_WIN_TARGET_MASK;
	decRegs.sizeReg |= targetAttribs.targetId << ATMWCR_WIN_TARGET_OFFS;

#if !defined(MV_RUN_FROM_FLASH)
    /* To be on the safe side we disable the window before writing the  */
    /* new values.                                                      */
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		mvAhbToMbusWinEnable(winNum,MV_FALSE);
	}
#endif

	/* 3) Write to address decode Base Address Register                   */
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		MV_REG_WRITE(AHB_TO_MBUS_WIN_BASE_REG(winNum), decRegs.baseReg);
	}
	else
	{
		MV_REG_WRITE(AHB_TO_MBUS_WIN_INTEREG_REG, decRegs.baseReg);
	}


	/* Internal register space have no size	*/
	/* register. Do not perform size register assigment for those targets 	*/
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		/* Write to address decode Size Register                        	*/
		MV_REG_WRITE(AHB_TO_MBUS_WIN_CTRL_REG(winNum), decRegs.sizeReg);
	}

    return MV_OK;
}

/*******************************************************************************
* mvAhbToMbusWinGet - Get CPU-to-peripheral winNum address window
*
* DESCRIPTION:
*		Get the CPU peripheral winNum address window.
*
* INPUT:
*       winNum - Peripheral winNum enumerator
*
* OUTPUT:
*       pAddrDecWin - CPU winNum window information data structure.
*
* RETURN:
*       MV_OK if winNum exist, MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvAhbToMbusWinGet(MV_U32 winNum, MV_AHB_TO_MBUS_DEC_WIN *pAddrDecWin)
{
	MV_DEC_REGS decRegs;
	MV_TARGET_ATTRIB targetAttrib;


	/* Parameter checking   */
	if (winNum >= MAX_AHB_TO_MBUS_WINS)
	{
		mvOsPrintf("mvAhbToMbusWinGet: ERR. Invalid winNum %d\n", winNum);
		return MV_NOT_SUPPORTED;
	}


	/* Internal register space size have no size register*/
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		decRegs.sizeReg =  MV_REG_READ(AHB_TO_MBUS_WIN_CTRL_REG(winNum));
	}
	else
	{
		decRegs.sizeReg = 0;
	}


	/* Read base and size	*/
	if (winNum != MV_AHB_TO_MBUS_INTREG_WIN)
	{
		decRegs.baseReg = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(winNum));
	}
	else
	{
		decRegs.baseReg = MV_REG_READ(AHB_TO_MBUS_WIN_INTEREG_REG);
	}



	if (MV_OK != mvCtrlRegToAddrDec(&decRegs,&(pAddrDecWin->addrWin)))
	{
		mvOsPrintf("mvAhbToMbusWinGet: mvCtrlRegToAddrDec Failed \n");
		return MV_ERROR;
	}

	if (winNum == MV_AHB_TO_MBUS_INTREG_WIN)
	{
        pAddrDecWin->addrWin.size = INTER_REGS_SIZE;
		pAddrDecWin->target = INTER_REGS;
		pAddrDecWin->enable = MV_TRUE;

		return MV_OK;
	}


	if (decRegs.sizeReg & ATMWCR_WIN_ENABLE)
	{
		pAddrDecWin->enable = MV_TRUE;
	}
	else
	{
		pAddrDecWin->enable = MV_FALSE;

	}



	if (-1 == pAddrDecWin->addrWin.size)
	{
		return MV_ERROR;
	}

	/* attrib and targetId */
	targetAttrib.attrib = (decRegs.sizeReg & ATMWCR_WIN_ATTR_MASK) >>
													ATMWCR_WIN_ATTR_OFFS;
	targetAttrib.targetId = (decRegs.sizeReg & ATMWCR_WIN_TARGET_MASK) >>
													ATMWCR_WIN_TARGET_OFFS;

	pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

	return MV_OK;
}

/*******************************************************************************
* mvAhbToMbusWinTargetGet - Get Window number associated with target
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
MV_U32	  mvAhbToMbusWinTargetGet(MV_TARGET target)
{
	MV_AHB_TO_MBUS_DEC_WIN decWin;
	MV_U32 winNum;

	/* Check parameters */
	if (target >= MAX_TARGETS)
	{
		mvOsPrintf("mvAhbToMbusWinTargetGet: target %d is Illigal\n", target);
		return 0xffffffff;
	}

	if (INTER_REGS == target)
	{
		return MV_AHB_TO_MBUS_INTREG_WIN;
	}

	for (winNum = 0; winNum < MAX_AHB_TO_MBUS_WINS ; winNum++)
	{
		if (winNum == MV_AHB_TO_MBUS_INTREG_WIN)
			continue;

		if (mvAhbToMbusWinGet(winNum,&decWin) != MV_OK)
		{
			mvOsPrintf("mvAhbToMbusWinTargetGet: mvAhbToMbusWinGet fail\n");
			return 0xffffffff;

		}

		if (decWin.enable == MV_TRUE)
		{
			if (decWin.target == target)
			{
				return winNum;
			}

		}

	}

	return 0xFFFFFFFF;


}

/*******************************************************************************
* mvAhbToMbusWinAvailGet - Get First Available window number.
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
MV_U32    mvAhbToMbusWinAvailGet(MV_VOID)
{
        MV_AHB_TO_MBUS_DEC_WIN decWin;
        MV_U32 winNum;

        for (winNum = 0; winNum < MAX_AHB_TO_MBUS_WINS ; winNum++)
        {
                if (winNum == MV_AHB_TO_MBUS_INTREG_WIN)
                        continue;

                if (mvAhbToMbusWinGet(winNum,&decWin) != MV_OK)
                {
                        mvOsPrintf("mvAhbToMbusWinTargetGet: mvAhbToMbusWinGet fail\n");
                        return 0xffffffff;

                }

                if (decWin.enable == MV_FALSE)
                {
			return winNum;
                }

        }

        return 0xFFFFFFFF;
}


/*******************************************************************************
* mvAhbToMbusWinEnable - Enable/disable a CPU address decode window
*
* DESCRIPTION:
*       This function enable/disable a CPU address decode window.
*       if parameter 'enable' == MV_TRUE the routine will enable the
*       window, thus enabling CPU accesses (before enabling the window it is
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - Peripheral winNum enumerator.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if protection window number was wrong, or the window
*       overlapps other winNum window.
*
*******************************************************************************/
MV_STATUS mvAhbToMbusWinEnable(MV_U32 winNum, MV_BOOL enable)
{

	/* Parameter checking   */
	if (winNum >= MAX_AHB_TO_MBUS_WINS)
	{
		mvOsPrintf("mvAhbToMbusWinEnable: ERR. Invalid winNum %d\n", winNum);
		return MV_NOT_SUPPORTED;
	}

	/* Internal registers bar can't be disable or enabled */
	if (winNum == MV_AHB_TO_MBUS_INTREG_WIN)
	{
		return (enable ? MV_OK : MV_ERROR);
	}

    if (enable == MV_TRUE)
    {
		/* enable the window */
		MV_REG_BIT_SET(AHB_TO_MBUS_WIN_CTRL_REG(winNum), ATMWCR_WIN_ENABLE);
    }
    else
    {   /* Disable address decode winNum window                             */
		MV_REG_BIT_RESET(AHB_TO_MBUS_WIN_CTRL_REG(winNum), ATMWCR_WIN_ENABLE);
    }

	return MV_OK;
}


/*******************************************************************************
* mvAhbToMbusWinRemap - Set CPU remap register for address windows.
*
* DESCRIPTION:
*       After a CPU address hits one of PCI address decode windows there is an
*       option to remap the address to a different one. For example, CPU
*       executes a read from PCI winNum window address 0x1200.0000. This
*       can be modified so the address on the PCI bus would be 0x1400.0000
*       Using the PCI address remap mechanism.
*
* INPUT:
*       winNum      - Peripheral winNum enumerator. Must be a PCI winNum.
*       pAddrDecWin - CPU winNum window information data structure.
*                     Note that caller has to fill in the base field only. The
*                     size field is ignored.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if winNum is not a PCI one, MV_OK otherwise.
*
*******************************************************************************/
MV_U32 mvAhbToMbusWinRemap(MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
    MV_U32 baseAddr;
	AHB_TO_MBUS_REMAP_REG_OFFS remapRegOffs;

    MV_U32 effectiveBaseAddress=0,
		   baseAddrValue=0,windowSizeValue=0;


	/* Get registers offsets of given winNum 		*/
	if (MV_NO_SUCH == ahbToMbusRemapRegOffsGet(winNum, &remapRegOffs))
	{
		return 0xffffffff;
	}

	/* 1) Set address remap low */
    baseAddr = pAddrWin->baseLow;

    /* Check base address aligment 					*/
	/*
	if (MV_IS_NOT_ALIGN(baseAddr, ATMWRLR_REMAP_LOW_ALIGNMENT))
	{
        mvOsPrintf("mvAhbToMbusPciRemap: Warning. Target base 0x%x unaligned\n",
																baseAddr);
        return MV_ERROR;
	}
	*/

	/* BaseLow[31:16] => base register [31:16] 		*/
	baseAddr = baseAddr & ATMWRLR_REMAP_LOW_MASK;

    MV_REG_WRITE(remapRegOffs.lowRegOffs, baseAddr);

	MV_REG_WRITE(remapRegOffs.highRegOffs, pAddrWin->baseHigh);


	baseAddrValue = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(winNum));
	windowSizeValue = MV_REG_READ(AHB_TO_MBUS_WIN_CTRL_REG(winNum));

	baseAddrValue &= ATMWBR_BASE_MASK;
	windowSizeValue &=ATMWCR_WIN_SIZE_MASK;

   /* Start calculating the effective Base Address */
   effectiveBaseAddress = baseAddrValue ;

   /* The effective base address will be combined from the chopped (if any)
	  remap value (according to the size value and remap mechanism) and the
	  window's base address */
   effectiveBaseAddress |= (((windowSizeValue) | 0xffff) & pAddrWin->baseLow);
   /* If the effectiveBaseAddress exceed the window boundaries return an
	  invalid value. */

   if (effectiveBaseAddress > (baseAddrValue + (windowSizeValue | 0xffff)))
   {
		mvOsPrintf("mvAhbToMbusPciRemap: Error\n");
		return 0xffffffff;
   }

	return effectiveBaseAddress;


}
/*******************************************************************************
* mvAhbToMbusWinTargetSwap - Swap AhbToMbus windows between targets
*
* DESCRIPTION:
*
* INPUT:
*       target1      - CPU Interface target 1
*       target2      - CPU Interface target 2
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if targets are illigal, or if one of the targets is not
*	    associated to a valid window .
*       MV_OK otherwise.
*
*******************************************************************************/


MV_STATUS mvAhbToMbusWinTargetSwap(MV_TARGET target1,MV_TARGET target2)
{
	MV_U32 winNum1,winNum2;
	MV_AHB_TO_MBUS_DEC_WIN winDec1,winDec2,winDecTemp;
	AHB_TO_MBUS_REMAP_REG_OFFS remapRegs1,remapRegs2;
	MV_U32 remapBaseLow1=0,remapBaseLow2=0;
	MV_U32 remapBaseHigh1=0,remapBaseHigh2=0;


	/* Check parameters */
	if (target1 >= MAX_TARGETS)
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: target %d is Illigal\n", target1);
		return MV_ERROR;
	}

	if (target2 >= MAX_TARGETS)
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: target %d is Illigal\n", target1);
		return MV_ERROR;
	}


    /* get window associated with this target */
	winNum1 = mvAhbToMbusWinTargetGet(target1);

	if (winNum1 == 0xffffffff)
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: target %d has illigal win %d\n",
					target1,winNum1);
		return MV_ERROR;

	}

    /* get window associated with this target */
	winNum2 = mvAhbToMbusWinTargetGet(target2);

	if (winNum2 == 0xffffffff)
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: target %d has illigal win %d\n",
					target2,winNum2);
		return MV_ERROR;

	}

	/* now Get original values of both Windows */
	if (MV_OK != mvAhbToMbusWinGet(winNum1,&winDec1))
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: mvAhbToMbusWinGet failed win %d\n",
					winNum1);
		return MV_ERROR;

	}
	if (MV_OK != mvAhbToMbusWinGet(winNum2,&winDec2))
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: mvAhbToMbusWinGet failed win %d\n",
					winNum2);
		return MV_ERROR;

	}


	/* disable both windows */
	if (MV_OK != mvAhbToMbusWinEnable(winNum1,MV_FALSE))
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: failed to enable window %d\n",
					winNum1);
		return MV_ERROR;

	}
	if (MV_OK != mvAhbToMbusWinEnable(winNum2,MV_FALSE))
	{
		mvOsPrintf("mvAhbToMbusWinTargetSwap: failed to enable windo %d\n",
					winNum2);
		return MV_ERROR;

	}


	/* now swap targets */

	/* first save winDec2 values */
	winDecTemp.addrWin.baseHigh = winDec2.addrWin.baseHigh;
	winDecTemp.addrWin.baseLow = winDec2.addrWin.baseLow;
	winDecTemp.addrWin.size = winDec2.addrWin.size;
	winDecTemp.enable = winDec2.enable;
	winDecTemp.target = winDec2.target;

	/* winDec2 = winDec1 */
	winDec2.addrWin.baseHigh = winDec1.addrWin.baseHigh;
	winDec2.addrWin.baseLow = winDec1.addrWin.baseLow;
	winDec2.addrWin.size = winDec1.addrWin.size;
	winDec2.enable = winDec1.enable;
	winDec2.target = winDec1.target;


	/* winDec1 = winDecTemp */
	winDec1.addrWin.baseHigh = winDecTemp.addrWin.baseHigh;
	winDec1.addrWin.baseLow = winDecTemp.addrWin.baseLow;
	winDec1.addrWin.size = winDecTemp.addrWin.size;
	winDec1.enable = winDecTemp.enable;
	winDec1.target = winDecTemp.target;


	/* now set the new values */


    mvAhbToMbusWinSet(winNum1,&winDec1);
	mvAhbToMbusWinSet(winNum2,&winDec2);





	/* now we will treat the remap windows if exist */


	/* now check if one or both windows has a remap window
	as well after the swap ! */

	/* if a window had a remap value differnt than the base value
	before the swap , then after the swap the remap value will be
	equal to the base value unless both windows has a remap windows*/

	/* first get old values */
	if (MV_NO_SUCH != ahbToMbusRemapRegOffsGet(winNum1,&remapRegs1))
	{
		remapBaseLow1 = MV_REG_READ(remapRegs1.lowRegOffs);
	    remapBaseHigh1 = MV_REG_READ(remapRegs1.highRegOffs);

	}
	if (MV_NO_SUCH != ahbToMbusRemapRegOffsGet(winNum2,&remapRegs2))
	{
		remapBaseLow2 = MV_REG_READ(remapRegs2.lowRegOffs);
	    remapBaseHigh2 = MV_REG_READ(remapRegs2.highRegOffs);


	}

	/* now do the swap */
	if (MV_NO_SUCH != ahbToMbusRemapRegOffsGet(winNum1,&remapRegs1))
	{
		if (MV_NO_SUCH != ahbToMbusRemapRegOffsGet(winNum2,&remapRegs2))
		{
			/* Two windows has a remap !!! so swap */

			MV_REG_WRITE(remapRegs2.highRegOffs,remapBaseHigh1);
			MV_REG_WRITE(remapRegs2.lowRegOffs,remapBaseLow1);

			MV_REG_WRITE(remapRegs1.highRegOffs,remapBaseHigh2);
			MV_REG_WRITE(remapRegs1.lowRegOffs,remapBaseLow2);



		}
		else
		{
			/* remap == base */
			MV_REG_WRITE(remapRegs1.highRegOffs,winDec1.addrWin.baseHigh);
			MV_REG_WRITE(remapRegs1.lowRegOffs,winDec1.addrWin.baseLow);

		}

	}
	else if (MV_NO_SUCH != ahbToMbusRemapRegOffsGet(winNum2,&remapRegs2))
	{
		/* remap == base */
		MV_REG_WRITE(remapRegs2.highRegOffs,winDec2.addrWin.baseHigh);
		MV_REG_WRITE(remapRegs2.lowRegOffs,winDec2.addrWin.baseLow);

	}



	return MV_OK;


}



#if defined(MV_88F1181)

/*******************************************************************************
* mvAhbToMbusXbarCtrlSet - Set The CPU master Xbar arbitration.
*
* DESCRIPTION:
*       This function sets CPU Mbus Arbiter
*
* INPUT:
*       pPizzaArbArray - A priority Structure describing 16 "pizza slices". At
*                    each clock cycle, the crossbar arbiter samples all
*                    requests and gives the bus to the next agent according
*                    to the "pizza".
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if paramers to function invalid.
*
*******************************************************************************/
MV_STATUS  mvMbusArbSet(MV_MBUS_ARB_TARGET *pPizzaArbArray)
{
	MV_U32 sliceNum;
	MV_U32 xbarCtrl = 0;
	MV_MBUS_ARB_TARGET xbarTarget;

	/* 1) Set crossbar control low register */
	for (sliceNum = 0; sliceNum < MRLR_SLICE_NUM; sliceNum++)
	{
		xbarTarget = pPizzaArbArray[sliceNum];

		/* sliceNum parameter check */
		if (xbarTarget > MAX_MBUS_ARB_TARGETS)
		{
			mvOsPrintf("mvAhbToMbusXbarCtrlSet: ERR. Can't set Target %d\n",
																  xbarTarget);
			return MV_ERROR;
		}
		xbarCtrl |= (xbarTarget << MRLR_LOW_ARB_OFFS(sliceNum));
	}
	/* Write to crossbar control low register */
    MV_REG_WRITE(MBUS_ARBITER_LOW_REG, xbarCtrl);

	xbarCtrl = 0;

	/* 2) Set crossbar control high register */
	for (sliceNum = MRLR_SLICE_NUM;
		 sliceNum < MRLR_SLICE_NUM+MRHR_SLICE_NUM;
		 sliceNum++)
	{

		xbarTarget = pPizzaArbArray[sliceNum];

		/* sliceNum parameter check */
		if (xbarTarget > MAX_MBUS_ARB_TARGETS)
		{
			mvOsPrintf("mvAhbToMbusXbarCtrlSet: ERR. Can't set Target %d\n",
																  xbarTarget);
			return MV_ERROR;
		}
		xbarCtrl |= (xbarTarget << MRHR_HIGH_ARB_OFFS(sliceNum));
	}
	/* Write to crossbar control high register */
    MV_REG_WRITE(MBUS_ARBITER_HIGH_REG, xbarCtrl);

	return MV_OK;
}

/*******************************************************************************
* mvMbusArbCtrlSet - Set MBus Arbiter control register
*
* DESCRIPTION:
*
* INPUT:
*       ctrl  - pointer to MV_MBUS_ARB_CTRL register
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if paramers to function invalid.
*
*******************************************************************************/
MV_STATUS mvMbusArbCtrlSet(MV_MBUS_ARB_CTRL *ctrl)
{

	if (ctrl->highPrio == MV_FALSE)
	{
		MV_REG_BIT_RESET(MBUS_ARBITER_CTRL_REG, MACR_ARB_ARM_TOP);
	}
	else
	{
		MV_REG_BIT_SET(MBUS_ARBITER_CTRL_REG, MACR_ARB_ARM_TOP);
	}

	if (ctrl->fixedRoundRobin == MV_FALSE)
	{
		MV_REG_BIT_RESET(MBUS_ARBITER_CTRL_REG, MACR_ARB_TARGET_FIXED);
	}
	else
	{
		MV_REG_BIT_SET(MBUS_ARBITER_CTRL_REG, MACR_ARB_TARGET_FIXED);
	}

	if (ctrl->starvEn == MV_FALSE)
	{
		MV_REG_BIT_RESET(MBUS_ARBITER_CTRL_REG, MACR_ARB_REQ_CTRL_EN);
	}
	else
	{
		MV_REG_BIT_SET(MBUS_ARBITER_CTRL_REG, MACR_ARB_REQ_CTRL_EN);
	}

	return MV_OK;
}

/*******************************************************************************
* mvMbusArbCtrlGet - Get MBus Arbiter control register
*
* DESCRIPTION:
*
* INPUT:
*       ctrl  - pointer to MV_MBUS_ARB_CTRL register
*
* OUTPUT:
*       ctrl  - pointer to MV_MBUS_ARB_CTRL register
*
* RETURN:
*       MV_ERROR if paramers to function invalid.
*
*******************************************************************************/
MV_STATUS mvMbusArbCtrlGet(MV_MBUS_ARB_CTRL *ctrl)
{

	MV_U32 ctrlReg = MV_REG_READ(MBUS_ARBITER_CTRL_REG);

	if (ctrlReg & MACR_ARB_ARM_TOP)
	{
		ctrl->highPrio = MV_TRUE;
	}
	else
	{
		ctrl->highPrio = MV_FALSE;
	}

	if (ctrlReg & MACR_ARB_TARGET_FIXED)
	{
		ctrl->fixedRoundRobin = MV_TRUE;
	}
	else
	{
		ctrl->fixedRoundRobin = MV_FALSE;
	}

	if (ctrlReg & MACR_ARB_REQ_CTRL_EN)
	{
		ctrl->starvEn = MV_TRUE;
	}
	else
	{
		ctrl->starvEn = MV_FALSE;
	}


	return MV_OK;
}

#endif  /* #if defined(MV_88F1181) */



/*******************************************************************************
* ahbToMbusRemapRegOffsGet - Get CPU address remap register offsets
*
* DESCRIPTION:
* 		CPU to PCI address remap registers offsets are inconsecutive.
*		This function returns PCI address remap registers offsets.
*
* INPUT:
*       winNum - Address decode window number. See MV_U32 enumerator.
*
* OUTPUT:
*       None.
*
* RETURN:
*		MV_ERROR if winNum is not a PCI one.
*
*******************************************************************************/
static MV_STATUS ahbToMbusRemapRegOffsGet(MV_U32 winNum,
									AHB_TO_MBUS_REMAP_REG_OFFS *pRemapRegs)
{
	switch (winNum)
	{
		case 0:
        case 1:
			pRemapRegs->lowRegOffs  = AHB_TO_MBUS_WIN_REMAP_LOW_REG(winNum);
			pRemapRegs->highRegOffs = AHB_TO_MBUS_WIN_REMAP_HIGH_REG(winNum);
			break;
		case 2:
		case 3:
			if((mvCtrlModelGet() == MV_5281_DEV_ID) ||
				(mvCtrlModelGet() == MV_1281_DEV_ID) ||
				(mvCtrlModelGet() == MV_6183_DEV_ID) ||
               (mvCtrlModelGet() == MV_6183L_DEV_ID))
			{
				pRemapRegs->lowRegOffs  = AHB_TO_MBUS_WIN_REMAP_LOW_REG(winNum);
				pRemapRegs->highRegOffs = AHB_TO_MBUS_WIN_REMAP_HIGH_REG(winNum);
				break;
			}
			else
			{
				pRemapRegs->lowRegOffs  = 0;
				pRemapRegs->highRegOffs = 0;

				DB(mvOsPrintf("ahbToMbusRemapRegOffsGet: ERR. Invalid winNum %d\n",
							winNum));
				return MV_NO_SUCH;
			}
		default:
		{
			pRemapRegs->lowRegOffs  = 0;
			pRemapRegs->highRegOffs = 0;

			DB(mvOsPrintf("ahbToMbusRemapRegOffsGet: ERR. Invalid winNum %d\n",
						winNum));
		   	return MV_NO_SUCH;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvAhbToMbusAddDecShow - Print the AHB to MBus bridge address decode map.
*
* DESCRIPTION:
*		This function print the CPU address decode map.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvAhbToMbusAddDecShow(MV_VOID)
{
	MV_AHB_TO_MBUS_DEC_WIN win;
	MV_U32 winNum;
	mvOsOutput( "\n" );
	mvOsOutput( "AHB To MBUS Bridge:\n" );
	mvOsOutput( "-------------------\n" );

	for( winNum = 0; winNum < MAX_AHB_TO_MBUS_WINS; winNum++ )
	{
		memset( &win, 0, sizeof(MV_AHB_TO_MBUS_DEC_WIN) );

		mvOsOutput( "win%d - ", winNum );

		if( mvAhbToMbusWinGet( winNum, &win ) == MV_OK )
		{
			if( win.enable )
			{
				mvOsOutput( "%s base %08x, ",
				mvCtrlTargetNameGet(win.target), win.addrWin.baseLow );
                		mvOsOutput( "...." );
				mvSizePrint( win.addrWin.size );

				mvOsOutput( "\n" );

            }
			else
				mvOsOutput( "disable\n" );
		}
	}

}

