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

#include "xor/mvXor.h"
#include "mvSysXor.h"

/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif	


static MV_STATUS xorWinOverlapDetect(MV_U32 unit,MV_U32 winNum, MV_ADDR_WIN *pAddrWin);

MV_TARGET xorAddrDecPrioTap[] = 
{
#if defined(MV_INCLUDE_DEVICE_CS0)
    DEVICE_CS0,
#endif
#if defined(MV_INCLUDE_PEX)
	PEX0_MEM,
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
#if defined(MV_INCLUDE_DEVICE_CS1)
    DEVICE_CS1,
#endif
#if defined(MV_INCLUDE_CESA)
   CRYPT_ENG,
#endif 
	TBL_TERM
};
static MV_STATUS mvXorInitWinsUnit (MV_U32 unit)
{
	MV_U32         winNum;
	MV_XOR_DEC_WIN addrDecWin;
	MV_CPU_DEC_WIN cpuAddrDecWin;
	MV_U32          status;
	MV_U32			winPrioIndex=0;

	/* Initiate XOR address decode */

	/* First disable all address decode windows */
	for(winNum = 0; winNum < XOR_MAX_ADDR_DEC_WIN; winNum++)
	{
	    mvXorTargetWinEnable(unit,winNum, MV_FALSE);
	}
	
	/* Go through all windows in user table until table terminator			*/
	for (winNum = 0; ((xorAddrDecPrioTap[winPrioIndex] != TBL_TERM) &&
					(winNum < XOR_MAX_ADDR_DEC_WIN));)
	{
		/* first get attributes from CPU If */
		status = mvCpuIfTargetWinGet(xorAddrDecPrioTap[winPrioIndex], 
									 &cpuAddrDecWin);

        if(MV_NO_SUCH == status)
        {
            winPrioIndex++;
            continue;
        }
		if (MV_OK != status)
		{
            mvOsPrintf("%s: ERR. mvCpuIfTargetWinGet failed\n", __FUNCTION__);
			return MV_ERROR;
		}

	
        if (cpuAddrDecWin.enable == MV_TRUE)
		{

			addrDecWin.target           = xorAddrDecPrioTap[winPrioIndex];
			addrDecWin.addrWin.baseLow  = cpuAddrDecWin.addrWin.baseLow; 
			addrDecWin.addrWin.baseHigh = cpuAddrDecWin.addrWin.baseHigh; 
			addrDecWin.addrWin.size     = cpuAddrDecWin.addrWin.size;    
			addrDecWin.enable           = MV_TRUE;
	
			if (MV_OK != mvXorTargetWinSet(unit,winNum, &addrDecWin))
			{
				DB(mvOsPrintf("mvXorInit: ERR. mvDmaTargetWinSet failed\n"));
				return MV_ERROR;
			}
			winNum++;
		}
		winPrioIndex++;	
        
	}
    
	return MV_OK;
}


/*******************************************************************************
* mvXorInit - Initialize XOR engine
*
* DESCRIPTION:
*		This function initialize XOR unit. It set the default address decode
*		windows of the unit.
*		Note that if the address window is disabled in xorAddrDecMap, the
*		window parameters will be set but the window will remain disabled.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*******************************************************************************/
MV_STATUS mvXorInit (MV_VOID)
{
	MV_U32         i;

	/* Initiate XOR address decode */
	for(i = 0; i < MV_XOR_MAX_UNIT; i++)
	    mvXorInitWinsUnit(i);

	mvXorHalInit(MV_XOR_MAX_CHAN);	
    
	return MV_OK;
}

/*******************************************************************************
* mvXorTargetWinSet - Set XOR target address window
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0) 
*       address window. After setting this target window, the XOR will be 
*       able to access the target within the address window. 
*
* INPUT:
*	    winNum - One of the possible XOR memory decode windows.
*       target - Peripheral target enumerator.
*       base   - Window base address.
*       size   - Window size.
*       enable - Window enable/disable.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvXorTargetWinSet(MV_U32 unit, MV_U32 winNum, MV_XOR_DEC_WIN *pAddrDecWin)
{
    MV_DEC_REGS xorDecRegs;
	MV_TARGET_ATTRIB targetAttribs;
    MV_U32      chan;
    
    /* Parameter checking */
    if (winNum >= XOR_MAX_ADDR_DEC_WIN)
    {
		DB(mvOsPrintf("%s: ERR. Invalid win num %d\n",__FUNCTION__, winNum));
        return MV_BAD_PARAM;
    }
    if (pAddrDecWin == NULL)
    {
        DB(mvOsPrintf("%s: ERR. pAddrDecWin is NULL pointer\n", __FUNCTION__ ));
        return MV_BAD_PTR;
    }                                         
    /* Check if the requested window overlaps with current windows */
    if (MV_TRUE == xorWinOverlapDetect(unit, winNum, &pAddrDecWin->addrWin))
    {
	DB(mvOsPrintf("%s: ERR. Window %d overlap\n",__FUNCTION__,winNum));
	return MV_ERROR;
    }                              

    xorDecRegs.baseReg = MV_REG_READ(XOR_BASE_ADDR_REG(unit,winNum));
    xorDecRegs.sizeReg = MV_REG_READ(XOR_SIZE_MASK_REG(unit,winNum));

    /* Get Base Address and size registers values */
    if(MV_OK != mvCtrlAddrDecToReg(&pAddrDecWin->addrWin, &xorDecRegs))
    {
		DB(mvOsPrintf("%s: ERR. Invalid addr dec window\n",__FUNCTION__));
        return MV_BAD_PARAM;
	}
    

	mvCtrlAttribGet(pAddrDecWin->target,&targetAttribs);

	/* set attributes */
	xorDecRegs.baseReg &= ~XEBARX_ATTR_MASK;
	xorDecRegs.baseReg |= targetAttribs.attrib << XEBARX_ATTR_OFFS;
	/* set target ID */
	xorDecRegs.baseReg &= ~XEBARX_TARGET_MASK;
	xorDecRegs.baseReg |= targetAttribs.targetId << XEBARX_TARGET_OFFS;


    /* Write to address decode Base Address Register */
	MV_REG_WRITE(XOR_BASE_ADDR_REG(unit,winNum), xorDecRegs.baseReg);
    
    /* Write to Size Register */
	MV_REG_WRITE(XOR_SIZE_MASK_REG(unit,winNum), xorDecRegs.sizeReg);
    
    for (chan = 0; chan < MV_XOR_MAX_CHAN_PER_UNIT; chan++)
    {
        if (pAddrDecWin->enable)
        {
            MV_REG_BIT_SET(XOR_WINDOW_CTRL_REG(unit,chan),
                           XEXWCR_WIN_EN_MASK(winNum));
        }
        else
        {
            MV_REG_BIT_RESET(XOR_WINDOW_CTRL_REG(unit,chan),
                             XEXWCR_WIN_EN_MASK(winNum));
        }
    }
    return MV_OK;
}

/*******************************************************************************
* mvXorTargetWinGet - Get xor peripheral target address window.
*
* DESCRIPTION:
*		Get xor peripheral target address window.
*
* INPUT:
*	  winNum - One of the possible XOR memory decode windows.
*
* OUTPUT:
*       base   - Window base address.
*       size   - Window size.
*       enable - window enable/disable.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvXorTargetWinGet(MV_U32 unit,MV_U32 winNum, MV_XOR_DEC_WIN *pAddrDecWin)
{
    MV_DEC_REGS xorDecRegs;
	MV_TARGET_ATTRIB targetAttrib;
    MV_U32      chan=0,chanWinEn;
    
    /* Parameter checking */
    if (winNum >= XOR_MAX_ADDR_DEC_WIN)
    {
		DB(mvOsPrintf("%s: ERR. Invalid win num %d\n",__FUNCTION__ , winNum));
        return MV_ERROR;
    }

    if (NULL == pAddrDecWin)
    {
        DB(mvOsPrintf("%s: ERR. pAddrDecWin is NULL pointer\n", __FUNCTION__ ));
        return MV_BAD_PTR;
    }

    chanWinEn = MV_REG_READ(XOR_WINDOW_CTRL_REG(unit,0)) & XEXWCR_WIN_EN_MASK(winNum);
    
    for (chan = 0; chan < MV_XOR_MAX_CHAN_PER_UNIT; chan++) /* we should scan here all channels per unit */
    {
    	/* Check if enable bit is equal for all channels */
        if ((MV_REG_READ(XOR_WINDOW_CTRL_REG(unit,chan)) & 
             XEXWCR_WIN_EN_MASK(winNum)) != chanWinEn)
        {
            mvOsPrintf("%s: ERR. Window enable field must be equal in "
                              "all channels(chan=%d)\n",__FUNCTION__, chan);
            return MV_ERROR;
        }
    }



	xorDecRegs.baseReg  = MV_REG_READ(XOR_BASE_ADDR_REG(unit,winNum));
	xorDecRegs.sizeReg  = MV_REG_READ(XOR_SIZE_MASK_REG(unit,winNum));

	if (MV_OK != mvCtrlRegToAddrDec(&xorDecRegs, &pAddrDecWin->addrWin))
	{
		mvOsPrintf("%s: ERR. mvCtrlRegToAddrDec failed\n", __FUNCTION__);
		return MV_ERROR;
	}

	/* attrib and targetId */
	targetAttrib.attrib = 
		(xorDecRegs.baseReg & XEBARX_ATTR_MASK) >> XEBARX_ATTR_OFFS;
	targetAttrib.targetId = 
		(xorDecRegs.baseReg & XEBARX_TARGET_MASK) >> XEBARX_TARGET_OFFS;


	pAddrDecWin->target = mvCtrlTargetGet(&targetAttrib);

	if(chanWinEn)
	{
		pAddrDecWin->enable = MV_TRUE;
	}
	else pAddrDecWin->enable = MV_FALSE;
	
    return MV_OK;
}

/*******************************************************************************
* mvXorTargetWinEnable - Enable/disable a Xor address decode window
*
* DESCRIPTION:
*       This function enable/disable a XOR address decode window.
*       if parameter 'enable' == MV_TRUE the routine will enable the 
*       window, thus enabling XOR accesses (before enabling the window it is 
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - Decode window number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvXorTargetWinEnable(MV_U32 unit,MV_U32 winNum, MV_BOOL enable)
{
	MV_XOR_DEC_WIN  addrDecWin;
    MV_U32          chan;
    
	/* Parameter checking   */               
    if (winNum >= XOR_MAX_ADDR_DEC_WIN)
    {
        DB(mvOsPrintf("%s: ERR. Invalid winNum%d\n", __FUNCTION__, winNum));
        return MV_ERROR;
    }

	if (enable == MV_TRUE) 
	{
		/* Get current window */
	    if (MV_OK != mvXorTargetWinGet(unit,winNum, &addrDecWin))
		{
			DB(mvOsPrintf("%s: ERR. targetWinGet fail\n", __FUNCTION__));
			return MV_ERROR;
		}

		/* Check for overlapping */
	    if (MV_TRUE == xorWinOverlapDetect(unit,winNum, &(addrDecWin.addrWin)))
		{
			/* Overlap detected	*/
			DB(mvOsPrintf("%s: ERR. Overlap detected\n", __FUNCTION__));
			return MV_ERROR;
		}

		/* No Overlap. Enable address decode target window */
		for (chan = 0; chan < MV_XOR_MAX_CHAN_PER_UNIT; chan++)
		{
		    MV_REG_BIT_SET(XOR_WINDOW_CTRL_REG(unit,chan),
						   XEXWCR_WIN_EN_MASK(winNum));
		}

	}
	else
	{
		/* Disable address decode target window */

		for (chan = 0; chan < MV_XOR_MAX_CHAN_PER_UNIT; chan++)
		{
		    MV_REG_BIT_RESET(XOR_WINDOW_CTRL_REG(unit,chan),
							 XEXWCR_WIN_EN_MASK(winNum));
		}

	}

	return MV_OK;                  
}

/*******************************************************************************
* mvXorSetProtWinSet - Configure access attributes of a XOR engine
*                               to one of the XOR memory windows.
*
* DESCRIPTION:
*       Each engine can be configured with access attributes for each of the
*       memory spaces. This function sets access attributes 
*       to a given window for the given engine
*
* INPUTS:
*       chan    - One of the possible engines.
*       winNum  - One of the possible XOR memory spaces.
*       access  - Protection access rights.
*       write   - Write rights.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvXorProtWinSet (MV_U32 unit,MV_U32 chan, MV_U32 winNum, MV_BOOL access, 
                           MV_BOOL write)
{
    MV_U32 temp;
    
    /* Parameter checking   */
    if (chan >= MV_XOR_MAX_CHAN_PER_UNIT)
    {
		DB(mvOsPrintf("%s: ERR. Invalid chan num %d\n", __FUNCTION__ , chan));
        return MV_BAD_PARAM;
    }
    if (winNum >= XOR_MAX_ADDR_DEC_WIN)
    {
		DB(mvOsPrintf("%s: ERR. Invalid win num %d\n", __FUNCTION__, winNum));
        return MV_BAD_PARAM;
    }

    temp = MV_REG_READ(XOR_WINDOW_CTRL_REG(unit,chan)) & 
        (~XEXWCR_WIN_ACC_MASK(winNum));

    /* if access is disable */
    if (!access)
    {
        /* disable access */
        temp |= XEXWCR_WIN_ACC_NO_ACC(winNum);
    }
    /* if access is enable */
    else
    {
        /* if write is enable */
        if (write)
        {
            /* enable write */
            temp |= XEXWCR_WIN_ACC_RW(winNum);
        }
        /* if write is disable */
        else
        {
            /* disable write */
            temp |= XEXWCR_WIN_ACC_RO(winNum);
        }
    }
    MV_REG_WRITE(XOR_WINDOW_CTRL_REG(unit,chan),temp);
    return MV_OK;
}

/*******************************************************************************
* mvXorPciRemap - Set XOR remap register for PCI address windows.
*
* DESCRIPTION:
*       only Windows 0-3 can be remapped.
*
* INPUT:
*       winNum      - window number
*       pAddrDecWin  - pointer to address space window structure
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if parameters to function invalid, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvXorPciRemap(MV_U32 unit,MV_U32 winNum, MV_U32 addrHigh)
{
    /* Parameter checking   */
    if (winNum >= XOR_MAX_REMAP_WIN)
    {
		DB(mvOsPrintf("%s: ERR. Invalid win num %d\n", __FUNCTION__, winNum));
        return MV_BAD_PARAM;
    }

    MV_REG_WRITE(XOR_HIGH_ADDR_REMAP_REG(unit,winNum), addrHigh);
    
	return MV_OK;
}

/*******************************************************************************
* xorWinOverlapDetect - Detect XOR address windows overlaping
*
* DESCRIPTION:
*       An unpredicted behaviour is expected in case XOR address decode 
*       windows overlaps.
*       This function detects XOR address decode windows overlaping of a 
*       specified window. The function does not check the window itself for 
*       overlaping. The function also skipps disabled address decode windows.
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
static MV_STATUS xorWinOverlapDetect(MV_U32 unit,MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
	MV_U32 	        baseAddrEnableReg;
	MV_U32          winNumIndex,chan;
	MV_XOR_DEC_WIN  addrDecWin;

	if (pAddrWin == NULL)
	{
		DB(mvOsPrintf("%s: ERR. pAddrWin is NULL pointer\n", __FUNCTION__ ));
		return MV_BAD_PTR;
	}
    
	for (chan = 0; chan < MV_XOR_MAX_CHAN_PER_UNIT; chan++)
	{
		/* Read base address enable register. Do not check disabled windows	*/
	    baseAddrEnableReg = MV_REG_READ(XOR_WINDOW_CTRL_REG(unit,chan));

		for (winNumIndex = 0; winNumIndex < XOR_MAX_ADDR_DEC_WIN; winNumIndex++)
		{
			/* Do not check window itself */
			if (winNumIndex == winNum)
			{
				continue;
			}
    
			/* Do not check disabled windows */
			if ((baseAddrEnableReg & XEXWCR_WIN_EN_MASK(winNumIndex)) == 0)
			{
				continue;
			}
    
			/* Get window parameters */
			if (MV_OK != mvXorTargetWinGet(unit,winNumIndex, &addrDecWin))
			{
				DB(mvOsPrintf("%s: ERR. TargetWinGet failed\n", __FUNCTION__ ));
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

static MV_VOID mvXorAddrDecShowUnit(MV_U32 unit)
{
	MV_XOR_DEC_WIN win;
	int            i;

	mvOsOutput( "\n" );
	mvOsOutput( "XOR %d:\n", unit );
	mvOsOutput( "----\n" );

	for( i = 0; i < XOR_MAX_ADDR_DEC_WIN; i++ )
	{
		memset( &win, 0, sizeof(MV_XOR_DEC_WIN) );

		mvOsOutput( "win%d - ", i );

		if( mvXorTargetWinGet(unit, i, &win ) == MV_OK )
		{
			if( win.enable )
			{
				mvOsOutput( "%s base %x, ",
				mvCtrlTargetNameGet(win.target), win.addrWin.baseLow );

				mvSizePrint( win.addrWin.size );
				
                mvOsOutput( "\n" );
			}
			else
				mvOsOutput( "disable\n" );
		}
	}
}

/*******************************************************************************
* mvXorAddrDecShow - Print the XOR address decode map.
*
* DESCRIPTION:
*		This function print the XOR address decode map.
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
MV_VOID mvXorAddrDecShow(MV_VOID)
{
	int            i;

	for( i = 0; i < MV_XOR_MAX_UNIT; i++ )
	    mvXorAddrDecShowUnit(i);
	
}
