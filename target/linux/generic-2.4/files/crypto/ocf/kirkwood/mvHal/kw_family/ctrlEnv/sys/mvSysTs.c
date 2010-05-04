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


#include "ctrlEnv/sys/mvSysTs.h"


typedef struct _mvTsuDecWin
{
        MV_TARGET     target;
        MV_ADDR_WIN   addrWin; /* An address window*/
        MV_BOOL       enable;  /* Address decode window is enabled/disabled */
 
}MV_TSU_DEC_WIN;


MV_TARGET tsuAddrDecPrioTap[] =
{
#if defined(MV_INCLUDE_PEX)
        PEX0_MEM,
#endif
#if defined(MV_INCLUDE_PCI)
        PCI0_MEM,
#endif
#if defined(MV_INCLUDE_SDRAM_CS0)
        SDRAM_CS0,
#endif
#if defined(MV_INCLUDE_SDRAM_CS1)
        SDRAM_CS1,
#endif
#if defined(MV_INCLUDE_SDRAM_CS2)
        SDRAM_CS2,
#endif
#if defined(MV_INCLUDE_SDRAM_CS3)
        SDRAM_CS3,
#endif
#if defined(MV_INCLUDE_DEVICE_CS0)
        DEVICE_CS0,
#endif
#if defined(MV_INCLUDE_DEVICE_CS1)
        DEVICE_CS1,
#endif
#if defined(MV_INCLUDE_DEVICE_CS2)
        DEVICE_CS2,
#endif
#if defined(MV_INCLUDE_DEVICE_CS3)
        DEVICE_CS3,
#endif
#if defined(MV_INCLUDE_PEX)
        PEX0_IO,
#endif
#if defined(MV_INCLUDE_PCI)
        PCI0_IO,
#endif
        TBL_TERM
};

static MV_STATUS tsuWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin);
static MV_STATUS mvTsuWinSet(MV_U32 winNum, MV_TSU_DEC_WIN *pAddrDecWin);
static MV_STATUS mvTsuWinGet(MV_U32 winNum, MV_TSU_DEC_WIN *pAddrDecWin);
MV_STATUS mvTsuWinEnable(MV_U32 winNum,MV_BOOL enable);

/*******************************************************************************
* mvTsuWinInit
*
* DESCRIPTION:
* 	Initialize the TSU unit address decode windows.
*
* INPUT:
*       None.
* OUTPUT:
*	None.
* RETURN:
*       MV_OK	- on success,
*
*******************************************************************************/
MV_STATUS mvTsuWinInit(void)
{
	MV_U32          winNum, status, winPrioIndex=0;
	MV_TSU_DEC_WIN  tsuWin;
	MV_CPU_DEC_WIN  cpuAddrDecWin;

	/* First disable all address decode windows */
	for(winNum = 0; winNum < TSU_MAX_DECODE_WIN; winNum++)
	{
		MV_REG_BIT_RESET(MV_TSU_WIN_CTRL_REG(winNum),
				 TSU_WIN_CTRL_EN_MASK);
	}

	/* Go through all windows in user table until table terminator      */
	for(winNum = 0; ((tsuAddrDecPrioTap[winPrioIndex] != TBL_TERM) &&
			 (winNum < TSU_MAX_DECODE_WIN));)
	{
		/* first get attributes from CPU If */
		status = mvCpuIfTargetWinGet(tsuAddrDecPrioTap[winPrioIndex],
					     &cpuAddrDecWin);

		if(MV_NO_SUCH == status)
		{
			winPrioIndex++;
			continue;
		}
		if(MV_OK != status)
		{
			mvOsPrintf("mvTsuWinInit: ERR. mvCpuIfTargetWinGet failed\n");
			return MV_ERROR;
		}

		if (cpuAddrDecWin.enable == MV_TRUE)
		{
			tsuWin.addrWin.baseHigh = cpuAddrDecWin.addrWin.baseHigh;
			tsuWin.addrWin.baseLow = cpuAddrDecWin.addrWin.baseLow;
			tsuWin.addrWin.size = cpuAddrDecWin.addrWin.size;
			tsuWin.enable = MV_TRUE;
			tsuWin.target = tsuAddrDecPrioTap[winPrioIndex];

			if(MV_OK != mvTsuWinSet(winNum, &tsuWin))
			{
				mvOsPrintf("mvTsuWinInit: ERR. mvTsuWinSet failed winNum=%d\n",
					   winNum);
				return MV_ERROR;
			}
			winNum++;
		}
		winPrioIndex ++;
	}

	return MV_OK;
}


/*******************************************************************************
* mvTsuWinSet
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0)
*       address window, also known as address decode window.
*       After setting this target window, the TSU will be able to access the
*       target within the address window.
*
* INPUT:
*       winNum      - TSU to target address decode window number.
*       pAddrDecWin - TSU target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR	- if address window overlapps with other address decode 
*			windows.
*       MV_BAD_PARAM	- if base address is invalid parameter or target is
*       		unknown.
*
*******************************************************************************/
MV_STATUS mvTsuWinSet(MV_U32 winNum, MV_TSU_DEC_WIN *pAddrDecWin)
{
	MV_TARGET_ATTRIB    targetAttribs;
	MV_DEC_REGS         decRegs;

	/* Parameter checking   */
	if(winNum >= TSU_MAX_DECODE_WIN)
	{
		mvOsPrintf("mvTsuWinSet: ERR. Invalid win num %d\n",winNum);
		return MV_BAD_PARAM;
	}    

	/* Check if the requested window overlapps with current windows     */
	if(MV_TRUE == tsuWinOverlapDetect(winNum, &pAddrDecWin->addrWin))
	{
		mvOsPrintf("mvTsuWinSet: ERR. Window %d overlap\n", winNum);
		return MV_ERROR;
	}

	/* check if address is aligned to the size */
	if(MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow,pAddrDecWin->addrWin.size))
	{
		mvOsPrintf("mvTsuWinSet: Error setting TSU window %d to target "
			   "%s.\nAddress 0x%08x is unaligned to size 0x%x.\n",
			   winNum, mvCtrlTargetNameGet(pAddrDecWin->target),
			   pAddrDecWin->addrWin.baseLow,
			   pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	decRegs.baseReg = MV_REG_READ(MV_TSU_WIN_BASE_REG(winNum));
	decRegs.sizeReg = MV_REG_READ(MV_TSU_WIN_CTRL_REG(winNum));
    
	if(MV_OK != mvCtrlAddrDecToReg(&(pAddrDecWin->addrWin),&decRegs))
	{
		mvOsPrintf("mvTsuWinSet: mvCtrlAddrDecToReg Failed\n");
		return MV_ERROR;
	}
    
	mvCtrlAttribGet(pAddrDecWin->target,&targetAttribs);
    
	/* set attributes */
	decRegs.sizeReg &= ~TSU_WIN_CTRL_ATTR_MASK;
	decRegs.sizeReg |= targetAttribs.attrib << TSU_WIN_CTRL_ATTR_OFFS;
	/* set target ID */
	decRegs.sizeReg &= ~TSU_WIN_CTRL_TARGET_MASK;
	decRegs.sizeReg |= targetAttribs.targetId << TSU_WIN_CTRL_TARGET_OFFS;

	/* for the safe side we disable the window before writing the new */
	/* values */
	mvTsuWinEnable(winNum, MV_FALSE);
	MV_REG_WRITE(MV_TSU_WIN_CTRL_REG(winNum),decRegs.sizeReg);

	/* Write to address decode Size Register                            */
	MV_REG_WRITE(MV_TSU_WIN_BASE_REG(winNum), decRegs.baseReg);

	/* Enable address decode target window                              */
	if(pAddrDecWin->enable == MV_TRUE)
	{
		mvTsuWinEnable(winNum,MV_TRUE);
	}

	return MV_OK;
}


/*******************************************************************************
* mvTsuWinGet
*
* DESCRIPTION:
*	Get TSU peripheral target address window.
*
* INPUT:
*	winNum - TSU to target address decode window number.
*
* OUTPUT:
*       pAddrDecWin - TSU target window data structure.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvTsuWinGet(MV_U32 winNum, MV_TSU_DEC_WIN *pAddrDecWin)
{
	MV_DEC_REGS decRegs;
	MV_TARGET_ATTRIB targetAttrib;

	/* Parameter checking   */
	if(winNum >= TSU_MAX_DECODE_WIN)
	{
		mvOsPrintf("mvTsuWinGet: ERR. Invalid winNum %d\n", winNum);
		return MV_NOT_SUPPORTED;
	}

	decRegs.baseReg = MV_REG_READ(MV_TSU_WIN_BASE_REG(winNum));                                                                           
	decRegs.sizeReg = MV_REG_READ(MV_TSU_WIN_CTRL_REG(winNum));
 
	if(MV_OK != mvCtrlRegToAddrDec(&decRegs,&(pAddrDecWin->addrWin)))
	{
		mvOsPrintf("mvTsuWinGet: mvCtrlRegToAddrDec Failed \n");
		return MV_ERROR;
	}

	/* attrib and targetId */
	targetAttrib.attrib = 
		(decRegs.sizeReg & TSU_WIN_CTRL_ATTR_MASK) >> TSU_WIN_CTRL_ATTR_OFFS;
	targetAttrib.targetId = 
		(decRegs.sizeReg & TSU_WIN_CTRL_TARGET_MASK) >> TSU_WIN_CTRL_TARGET_OFFS;

	pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

	/* Check if window is enabled   */
	if((MV_REG_READ(MV_TSU_WIN_CTRL_REG(winNum)) & TSU_WIN_CTRL_EN_MASK))
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
* mvTsuWinEnable
*
* DESCRIPTION:
*       This function enable/disable a TSU to target address window.
*       According to parameter 'enable' the routine will enable the
*       window, thus enabling TSU accesses (before enabling the window it is
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - TSU to target address decode window number.
*       enable - Enable / disable parameter.
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if decode window number was wrong or enabled window overlapps.
*
*******************************************************************************/
MV_STATUS mvTsuWinEnable(MV_U32 winNum,MV_BOOL enable)
{
	MV_TSU_DEC_WIN addrDecWin;

	/* Parameter checking   */
	if(winNum >= TSU_MAX_DECODE_WIN)
	{
		mvOsPrintf("mvTsuWinEnable: ERR. Invalid winNum%d\n",winNum);
		return MV_ERROR;
	}

	if(enable == MV_TRUE)
	{
		/* First check for overlap with other enabled windows   */
		/* Get current window.					*/
		if(MV_OK != mvTsuWinGet(winNum,&addrDecWin))
		{
			mvOsPrintf("mvTsuWinEnable: ERR. targetWinGet fail\n");
			return MV_ERROR;
		}
		/* Check for overlapping.	*/
		if(MV_FALSE == tsuWinOverlapDetect(winNum,&(addrDecWin.addrWin)))
		{
			/* No Overlap. Enable address decode target window   */
			MV_REG_BIT_SET(MV_TSU_WIN_CTRL_REG(winNum),
				       TSU_WIN_CTRL_EN_MASK);
		}
		else
		{
			/* Overlap detected */
			mvOsPrintf("mvTsuWinEnable: ERR. Overlap detected\n");
			return MV_ERROR;
		}
	}
	else
	{
		/* Disable address decode target window */
		MV_REG_BIT_RESET(MV_TSU_WIN_CTRL_REG(winNum),
				 TSU_WIN_CTRL_EN_MASK);
	}
	return MV_OK;
}

/*******************************************************************************
* mvTsuWinTargetGet
*
* DESCRIPTION:
*	Get Window number associated with target
*
* INPUT:
*	target	- Target ID to get the window number for.
* OUTPUT:
*
* RETURN:
*       window number or 0xFFFFFFFF on error.
*
*******************************************************************************/
MV_U32  mvTsuWinTargetGet(MV_TARGET target)
{
	MV_TSU_DEC_WIN decWin;
	MV_U32 winNum;

	/* Check parameters */
	if(target >= MAX_TARGETS)
	{
		mvOsPrintf("mvTsuWinTargetGet: target %d is Illigal\n", target);
		return 0xffffffff;
	}

	for(winNum = 0; winNum < TSU_MAX_DECODE_WIN; winNum++)
	{
		if(mvTsuWinGet(winNum,&decWin) != MV_OK)
		{
			mvOsPrintf("mvTsuWinGet: window returned error\n");
			return 0xffffffff;
		}

		if (decWin.enable == MV_TRUE)
		{
			if(decWin.target == target)
			{
				return winNum;
			}
		}
	}
	return 0xFFFFFFFF;
}


/*******************************************************************************
* tsuWinOverlapDetect
*
* DESCRIPTION:
*	Detect TSU address windows overlapping
*	An unpredicted behaviur is expected in case TSU address decode
*	windows overlapps.
*	This function detects TSU address decode windows overlapping of a
*	specified window. The function does not check the window itself for
*	overlapping. The function also skipps disabled address decode windows.
*
* INPUT:
*       winNum      - address decode window number.
*       pAddrDecWin - An address decode window struct.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlap current address
*       decode map, MV_FALSE otherwise, MV_ERROR if reading invalid data
*       from registers.
*
*******************************************************************************/
static MV_STATUS tsuWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
	MV_U32              ctrlReg;
	MV_U32              winNumIndex;
	MV_TSU_DEC_WIN      addrDecWin;

	for(winNumIndex = 0; winNumIndex < TSU_MAX_DECODE_WIN; winNumIndex++)
	{
		/* Do not check window itself           */
		if(winNumIndex == winNum)
		{
			continue;
		}

		/* Do not check disabled windows        */
		ctrlReg = MV_REG_READ(MV_TSU_WIN_CTRL_REG(winNumIndex));
		if((ctrlReg & TSU_WIN_CTRL_EN_MASK) == 0)
		{
			continue;
		}

		/* Get window parameters        */
		if (MV_OK != mvTsuWinGet(winNumIndex, &addrDecWin))
		{
			mvOsPrintf("tsuWinOverlapDetect: ERR. mvTsuWinGet failed\n");
			return MV_ERROR;
		}

		if (MV_TRUE == ctrlWinOverlapTest(pAddrWin, &(addrDecWin.addrWin)))
		{
			return MV_TRUE;
		}
	}
	return MV_FALSE;
}


/*******************************************************************************
* mvTsuAddrDecShow
*
* DESCRIPTION:
*	Print the TSU address decode map.
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
void mvTsuAddrDecShow(void)
{
	MV_TSU_DEC_WIN  win;
	int             i;

	if (MV_FALSE == mvCtrlPwrClckGet(TS_UNIT_ID, 0)) 
		return;

	mvOsOutput( "\n" );
	mvOsOutput( "TSU:\n");
	mvOsOutput( "----\n" );

	for(i = 0; i < TSU_MAX_DECODE_WIN; i++)
	{
		memset(&win, 0, sizeof(TSU_MAX_DECODE_WIN));
		mvOsOutput( "win%d - ", i );

		if(mvTsuWinGet(i, &win ) == MV_OK )
		{
			if(win.enable == MV_TRUE)
			{
				mvOsOutput("%s base %08x, ",
					   mvCtrlTargetNameGet(win.target),
					   win.addrWin.baseLow);
				mvOsOutput( "...." );
				mvSizePrint(win.addrWin.size );
				mvOsOutput( "\n" );
			}
			else
			{
				mvOsOutput( "disable\n" );
			}
		}
	}
	return;
}


/*******************************************************************************
* mvTsuInit
*
* DESCRIPTION:
* 	Initialize the TSU unit, and get unit out of reset.
*
* INPUT:
*       coreClock	- The core clock at which the TSU should operate.
*       mode		- The mode on configure the unit into (serial/parallel).
* 	memHandle	- Memory handle used for memory allocations.
* OUTPUT:
*	None.
* RETURN:
*       MV_OK	- on success,
*
*******************************************************************************/
MV_STATUS mvTsuInit(MV_TSU_CORE_CLOCK coreClock, MV_TSU_PORTS_MODE mode,
	            void *osHandle)
{
	MV_STATUS status;

	status = mvTsuWinInit();
	if(status == MV_OK)
		status = mvTsuHalInit(coreClock,mode,osHandle);

	return status;
}
