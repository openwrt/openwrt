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

#include "mvSysTdm.h"


/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif	

static MV_TARGET tdmAddrDecPrioTap[] =
{
        PEX0_MEM,
        SDRAM_CS0,
        SDRAM_CS1,
        SDRAM_CS2,
        SDRAM_CS3,
        DEVICE_CS0,
        DEVICE_CS1,
        DEVICE_CS2,
        DEV_BOOCS,
        PEX0_IO,
        TBL_TERM
};

static MV_STATUS tdmWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin);

/*******************************************************************************
* mvTdmWinInit - Initialize TDM address decode windows 
*
* DESCRIPTION:
*               This function initialize TDM window decode unit. It set the 
*               default address decode
*               windows of the unit.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if setting fail.
*******************************************************************************/

MV_STATUS mvTdmWinInit(void)
{
	MV_U32 		winNum;
	MV_U32		winPrioIndex = 0;
	MV_CPU_DEC_WIN cpuAddrDecWin;
	MV_TDM_DEC_WIN tdmWin;
	MV_STATUS status;

	/*Disable all windows*/	
	for (winNum = 0; winNum < TDM_MBUS_MAX_WIN; winNum++)
	{
		mvTdmWinEnable(winNum, MV_FALSE);
	}

	for (winNum = 0; ((tdmAddrDecPrioTap[winPrioIndex] != TBL_TERM) && 
					  (winNum < TDM_MBUS_MAX_WIN)); )
	{	
		status = mvCpuIfTargetWinGet(tdmAddrDecPrioTap[winPrioIndex], 
									 &cpuAddrDecWin);
        if (MV_NO_SUCH == status)
        {
            winPrioIndex++;
            continue;
        }
		if (MV_OK != status)
		{
			mvOsPrintf("mvTdmInit: ERR. mvCpuIfTargetWinGet failed\n");
			return MV_ERROR;
		}

        if (cpuAddrDecWin.enable == MV_TRUE)
		{
			tdmWin.addrWin.baseHigh = cpuAddrDecWin.addrWin.baseHigh;
			tdmWin.addrWin.baseLow = cpuAddrDecWin.addrWin.baseLow;
			tdmWin.addrWin.size = cpuAddrDecWin.addrWin.size;
			tdmWin.enable = MV_TRUE;
		    tdmWin.target = tdmAddrDecPrioTap[winPrioIndex];
		    if (MV_OK != mvTdmWinSet(winNum, &tdmWin))
		    {
			    return MV_ERROR;
		    }
		    winNum++;
		}
		winPrioIndex++;			
    }
	return MV_OK;
}

/*******************************************************************************
* mvTdmWinSet - Set TDM target address window
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0)
*       address window, also known as address decode window.
*       After setting this target window, the TDM will be able to access the
*       target within the address window.
*
* INPUT:
*       winNum      - TDM to target address decode window number.
*       pAddrDecWin - TDM target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if address window overlapps with other address decode windows.
*       MV_BAD_PARAM if base address is invalid parameter or target is
*       unknown.
*
*******************************************************************************/

MV_STATUS mvTdmWinSet(MV_U32 winNum, MV_TDM_DEC_WIN *pAddrDecWin)
{
	MV_TARGET_ATTRIB targetAttribs;
	MV_DEC_REGS decRegs;
	MV_U32 ctrlReg = 0;

    /* Parameter checking   */
    if (winNum >= TDM_MBUS_MAX_WIN)
    {
		mvOsPrintf("mvTdmWinSet: ERR. Invalid win num %d\n",winNum);
        return MV_BAD_PARAM;
    }
    
    /* Check if the requested window overlapps with current windows         */
    if (MV_TRUE == tdmWinOverlapDetect(winNum, &pAddrDecWin->addrWin))
   	{
       	mvOsPrintf("mvTdmWinSet: ERR. Window %d overlap\n", winNum);
		return MV_ERROR;
	}

	/* check if address is aligned to the size */
	if (MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size))
	{
		mvOsPrintf("mvTdmWinSet: Error setting TDM window %d to "\
				   "target %s.\nAddress 0x%08x is unaligned to size 0x%x.\n",
				   winNum,
				   mvCtrlTargetNameGet(pAddrDecWin->target), 
				   pAddrDecWin->addrWin.baseLow,
				   pAddrDecWin->addrWin.size);
		return MV_ERROR;
	}

	decRegs.baseReg = MV_REG_READ(TDM_WIN_BASE_REG(winNum));
	decRegs.sizeReg = (MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_SIZE_MASK) >>  TDM_WIN_SIZE_OFFS;

	if (MV_OK != mvCtrlAddrDecToReg(&(pAddrDecWin->addrWin),&decRegs))
	{
			mvOsPrintf("mvTdmWinSet: mvCtrlAddrDecToReg Failed\n");
			return MV_ERROR;
	}
	
	mvCtrlAttribGet(pAddrDecWin->target, &targetAttribs);
	
	/* for the safe side we disable the window before writing the new
	values */
	mvTdmWinEnable(winNum, MV_FALSE);

	ctrlReg |= (targetAttribs.attrib << TDM_WIN_ATTRIB_OFFS);
	ctrlReg |= (targetAttribs.targetId << TDM_WIN_TARGET_OFFS);
	ctrlReg |= (decRegs.sizeReg & TDM_WIN_SIZE_MASK);

	/* Write to address base and control registers  */
	MV_REG_WRITE(TDM_WIN_BASE_REG(winNum), decRegs.baseReg);
	MV_REG_WRITE(TDM_WIN_CTRL_REG(winNum), ctrlReg);  
	/* Enable address decode target window  */
	if (pAddrDecWin->enable == MV_TRUE)
	{
		mvTdmWinEnable(winNum, MV_TRUE);
	}    
	return MV_OK;
}

/*******************************************************************************
* mvTdmWinGet - Get peripheral target address window.
*
* DESCRIPTION:
*               Get TDM peripheral target address window.
*
* INPUT:
*       winNum - TDM to target address decode window number.
*
* OUTPUT:
*       pAddrDecWin - TDM target window data structure.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/

MV_STATUS mvTdmWinGet(MV_U32 winNum, MV_TDM_DEC_WIN *pAddrDecWin)
{

	MV_DEC_REGS decRegs;
	MV_TARGET_ATTRIB targetAttrib;

	/* Parameter checking   */
	if (winNum >= TDM_MBUS_MAX_WIN)
	{
		mvOsPrintf("mvTdmWinGet: ERR. Invalid winNum %d\n", winNum);
		return MV_NOT_SUPPORTED;
	}
	
	decRegs.baseReg =  MV_REG_READ(TDM_WIN_BASE_REG(winNum));                                                                           
	decRegs.sizeReg = (MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_SIZE_MASK) >>  TDM_WIN_SIZE_OFFS;
 
	if (MV_OK != mvCtrlRegToAddrDec(&decRegs,&(pAddrDecWin->addrWin)))
	{
		mvOsPrintf("mvTdmWinGet: mvCtrlRegToAddrDec Failed \n");
		return MV_ERROR;
	}
	 
	/* attrib and targetId */
	targetAttrib.attrib = 
		(MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_ATTRIB_MASK) >>  TDM_WIN_ATTRIB_OFFS;
	targetAttrib.targetId = 
		(MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_TARGET_MASK) >>  TDM_WIN_TARGET_OFFS;
	 
	pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

	/* Check if window is enabled   */
	if (MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_ENABLE_MASK)
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
* mvTdmWinEnable - Enable/disable a TDM to target address window
*
* DESCRIPTION:
*       This function enable/disable a TDM to target address window.
*       According to parameter 'enable' the routine will enable the
*       window, thus enabling TDM accesses (before enabling the window it is
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - TDM to target address decode window number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if decode window number was wrong or enabled window overlapps.
*
*******************************************************************************/
MV_STATUS mvTdmWinEnable(int winNum, MV_BOOL enable)
{
	MV_TDM_DEC_WIN addrDecWin;

	if (MV_TRUE == enable)
	{
		if (winNum >= TDM_MBUS_MAX_WIN)
		{
			mvOsPrintf("mvTdmWinEnable:ERR. Invalid winNum%d\n",winNum);
			return MV_ERROR;
		}	
		
		/* First check for overlap with other enabled windows				*/
		/* Get current window */
		if (MV_OK != mvTdmWinGet(winNum, &addrDecWin))
		{
			mvOsPrintf("mvTdmWinEnable:ERR. targetWinGet fail\n");
			return MV_ERROR;
		}
		/* Check for overlapping */
		if (MV_FALSE == tdmWinOverlapDetect(winNum, &(addrDecWin.addrWin)))
		{
			/* No Overlap. Enable address decode target window */
			MV_REG_BIT_SET(TDM_WIN_CTRL_REG(winNum), TDM_WIN_ENABLE_MASK);
		}
		else
		{   /* Overlap detected	*/
			mvOsPrintf("mvTdmWinEnable:ERR. Overlap detected\n");
			return MV_ERROR;
		}
	}
	else
	{
		MV_REG_BIT_RESET(TDM_WIN_CTRL_REG(winNum), TDM_WIN_ENABLE_MASK);	
	}
	return MV_OK;
}


/*******************************************************************************
* tdmWinOverlapDetect - Detect TDM address windows overlapping
*
* DESCRIPTION:
*       An unpredicted behaviour is expected in case TDM address decode 
*       windows overlapps.
*       This function detects TDM address decode windows overlapping of a 
*       specified window. The function does not check the window itself for 
*       overlapping. The function also skipps disabled address decode windows.
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
static MV_STATUS tdmWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
    MV_U32      	winNumIndex;
    MV_TDM_DEC_WIN  	addrDecWin;

    for (winNumIndex = 0; winNumIndex < TDM_MBUS_MAX_WIN; winNumIndex++)
    {
		/* Do not check window itself		*/
        if (winNumIndex == winNum)
		{
			continue;
		}
		/* Do not check disabled windows	*/
		if (MV_REG_READ(TDM_WIN_CTRL_REG(winNum)) & TDM_WIN_ENABLE_MASK)
		{
			/* Get window parameters 	*/
			if (MV_OK != mvTdmWinGet(winNumIndex, &addrDecWin))
			{
				DB(mvOsPrintf("dmaWinOverlapDetect: ERR. TargetWinGet failed\n"));
            		return MV_ERROR;
			}

			if (MV_TRUE == ctrlWinOverlapTest(pAddrWin, &(addrDecWin.addrWin)))
			{
				return MV_TRUE;
			}        
		}
    }
	return MV_FALSE;
}

/*******************************************************************************
* mvTdmAddrDecShow - Print the TDM address decode map.
*
* DESCRIPTION:
*       This function print the TDM address decode map.
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
MV_VOID mvTdmAddrDecShow(MV_VOID)
{
	MV_TDM_DEC_WIN win;
	int i;

	mvOsOutput( "\n" );
	mvOsOutput( "TDM:\n" );
	mvOsOutput( "----\n" );

	for( i = 0; i < TDM_MBUS_MAX_WIN; i++ )
	{
		memset( &win, 0, sizeof(MV_TDM_DEC_WIN) );

		mvOsOutput( "win%d - ", i );

		if (mvTdmWinGet(i, &win ) == MV_OK )
		{
			if( win.enable )
			{
                mvOsOutput( "%s base %08x, ",
                mvCtrlTargetNameGet(win.target), win.addrWin.baseLow);
                mvOsOutput( "...." );
                mvSizePrint( win.addrWin.size );
				mvOsOutput( "\n" );
			}
			else
				mvOsOutput( "disable\n" );
		}
	}
}

