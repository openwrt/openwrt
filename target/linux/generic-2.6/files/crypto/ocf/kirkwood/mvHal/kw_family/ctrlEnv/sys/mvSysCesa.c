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

#include "mvSysCesa.h"

#if (MV_CESA_VERSION >= 2)
MV_TARGET tdmaAddrDecPrioTable[] =
{
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
#if defined(MV_INCLUDE_PEX)
    PEX0_MEM,
#endif

    TBL_TERM
};

/*******************************************************************************
* mvCesaWinGet - Get TDMA target address window.
*
* DESCRIPTION:
*       Get TDMA target address window.
*
* INPUT:
*       winNum - TDMA target address decode window number.
*
* OUTPUT:
*       pDecWin - TDMA target window data structure.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
static MV_STATUS mvCesaWinGet(MV_U32 winNum, MV_DEC_WIN *pDecWin)
{                                                                                                                         
    MV_DEC_WIN_PARAMS   winParam;
    MV_U32              sizeReg, baseReg;

    /* Parameter checking   */
    if (winNum >= MV_CESA_TDMA_ADDR_DEC_WIN)
    {
        mvOsPrintf("%s : ERR. Invalid winNum %d\n", 
                    __FUNCTION__, winNum);
        return MV_NOT_SUPPORTED;
    }

    baseReg = MV_REG_READ( MV_CESA_TDMA_BASE_ADDR_REG(winNum) );
    sizeReg = MV_REG_READ( MV_CESA_TDMA_WIN_CTRL_REG(winNum) );
 
   /* Check if window is enabled   */
    if(sizeReg & MV_CESA_TDMA_WIN_ENABLE_MASK) 
    {
        pDecWin->enable = MV_TRUE;

        /* Extract window parameters from registers */
        winParam.targetId = (sizeReg & MV_CESA_TDMA_WIN_TARGET_MASK) >> MV_CESA_TDMA_WIN_TARGET_OFFSET; 
        winParam.attrib   = (sizeReg & MV_CESA_TDMA_WIN_ATTR_MASK) >> MV_CESA_TDMA_WIN_ATTR_OFFSET;
        winParam.size     = (sizeReg & MV_CESA_TDMA_WIN_SIZE_MASK) >> MV_CESA_TDMA_WIN_SIZE_OFFSET;
        winParam.baseAddr = (baseReg & MV_CESA_TDMA_WIN_BASE_MASK);

        /* Translate the decode window parameters to address decode struct */
        if (MV_OK != mvCtrlParamsToAddrDec(&winParam, pDecWin))
        {
            mvOsPrintf("Failed to translate register parameters to CESA address" \
                       " decode window structure\n");
            return MV_ERROR;        
        }
    }
    else
    {
        pDecWin->enable = MV_FALSE;
    }
    return MV_OK;
}

/*******************************************************************************
* cesaWinOverlapDetect - Detect CESA TDMA address windows overlapping
*
* DESCRIPTION:
*       An unpredicted behaviur is expected in case TDMA address decode 
*       windows overlapps.
*       This function detects TDMA address decode windows overlapping of a 
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
*       MV_TRUE     - if the given address window overlap current address
*                   decode map, 
*       MV_FALSE    - otherwise, MV_ERROR if reading invalid data 
*                   from registers.
*
*******************************************************************************/
static MV_STATUS cesaWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
    MV_U32          winNumIndex;
    MV_DEC_WIN      addrDecWin;
    
    for(winNumIndex=0; winNumIndex<MV_CESA_TDMA_ADDR_DEC_WIN; winNumIndex++)
    {
        /* Do not check window itself       */
        if (winNumIndex == winNum)
        {
            continue;
        }

        /* Get window parameters    */
        if (MV_OK != mvCesaWinGet(winNumIndex, &addrDecWin))
        {
            mvOsPrintf("%s: ERR. TargetWinGet failed\n", __FUNCTION__);
            return MV_ERROR;
        }

        /* Do not check disabled windows    */
        if(addrDecWin.enable == MV_FALSE)
        {
            continue;
        }

        if (MV_TRUE == ctrlWinOverlapTest(pAddrWin, &(addrDecWin.addrWin)))
        {
            return MV_TRUE;
        }        
    }
    return MV_FALSE;
}

/*******************************************************************************
* mvCesaTdmaWinSet - Set CESA TDMA target address window
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0) 
*       address window, also known as address decode window. 
*       After setting this target window, the CESA TDMA will be able to access the 
*       target within the address window. 
*
* INPUT:
*       winNum      - CESA TDMA target address decode window number.
*       pAddrDecWin - CESA TDMA target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR        - if address window overlapps with other address decode windows.
*       MV_BAD_PARAM    - if base address is invalid parameter or target is 
*                       unknown.
*
*******************************************************************************/
static MV_STATUS mvCesaTdmaWinSet(MV_U32 winNum, MV_DEC_WIN *pDecWin)
{
    MV_DEC_WIN_PARAMS   winParams;
    MV_U32              sizeReg, baseReg;
    
    /* Parameter checking   */
    if (winNum >= MV_CESA_TDMA_ADDR_DEC_WIN)
    {
        mvOsPrintf("mvCesaTdmaWinSet: ERR. Invalid win num %d\n",winNum);
        return MV_BAD_PARAM;
    }

    /* Check if the requested window overlapps with current windows     */
    if (MV_TRUE == cesaWinOverlapDetect(winNum, &pDecWin->addrWin))
    {
        mvOsPrintf("%s: ERR. Window %d overlap\n", __FUNCTION__, winNum);
        return MV_ERROR;
    }

    /* check if address is aligned to the size */
    if(MV_IS_NOT_ALIGN(pDecWin->addrWin.baseLow, pDecWin->addrWin.size))
    {
        mvOsPrintf("mvCesaTdmaWinSet: Error setting CESA TDMA window %d to "\
                   "target %s.\nAddress 0x%08x is unaligned to size 0x%x.\n",
                   winNum,
                   mvCtrlTargetNameGet(pDecWin->target), 
                   pDecWin->addrWin.baseLow,
                   pDecWin->addrWin.size);
        return MV_ERROR;
    }

    if(MV_OK != mvCtrlAddrDecToParams(pDecWin, &winParams))
    {
        mvOsPrintf("%s: mvCtrlAddrDecToParams Failed\n", __FUNCTION__);
        return MV_ERROR;
    }

    /* set Size, Attributes and TargetID */
    sizeReg = (((winParams.targetId << MV_CESA_TDMA_WIN_TARGET_OFFSET) & MV_CESA_TDMA_WIN_TARGET_MASK) |
               ((winParams.attrib   << MV_CESA_TDMA_WIN_ATTR_OFFSET)   & MV_CESA_TDMA_WIN_ATTR_MASK)   |
               ((winParams.size << MV_CESA_TDMA_WIN_SIZE_OFFSET) & MV_CESA_TDMA_WIN_SIZE_MASK));

    if (pDecWin->enable == MV_TRUE)
    {
        sizeReg |= MV_CESA_TDMA_WIN_ENABLE_MASK;
    }
    else
    {
        sizeReg &= ~MV_CESA_TDMA_WIN_ENABLE_MASK;
    }

    /* Update Base value  */
    baseReg = (winParams.baseAddr & MV_CESA_TDMA_WIN_BASE_MASK);     

    MV_REG_WRITE( MV_CESA_TDMA_WIN_CTRL_REG(winNum), sizeReg);
    MV_REG_WRITE( MV_CESA_TDMA_BASE_ADDR_REG(winNum), baseReg);
        
    return MV_OK;
}


static MV_STATUS   mvCesaTdmaAddrDecInit (void)
{
    MV_U32          winNum;
    MV_STATUS       status;
    MV_CPU_DEC_WIN  cpuAddrDecWin;
    MV_DEC_WIN      cesaWin;
    MV_U32          winPrioIndex = 0;

    /* First disable all address decode windows */
    for(winNum=0; winNum<MV_CESA_TDMA_ADDR_DEC_WIN; winNum++)
    {
        MV_REG_BIT_RESET(MV_CESA_TDMA_WIN_CTRL_REG(winNum), MV_CESA_TDMA_WIN_ENABLE_MASK);
    }

    /* Go through all windows in user table until table terminator      */
    winNum = 0;
    while( (tdmaAddrDecPrioTable[winPrioIndex] != TBL_TERM) && 
           (winNum < MV_CESA_TDMA_ADDR_DEC_WIN) )    {

        /* first get attributes from CPU If */
        status = mvCpuIfTargetWinGet(tdmaAddrDecPrioTable[winPrioIndex], 
                                     &cpuAddrDecWin);
        if(MV_NO_SUCH == status){
	    winPrioIndex++;
            continue;
	}

        if (MV_OK != status)
        {
            mvOsPrintf("cesaInit: TargetWinGet failed. winNum=%d, winIdx=%d, target=%d, status=0x%x\n",
                        winNum, winPrioIndex, tdmaAddrDecPrioTable[winPrioIndex], status);
            return MV_ERROR;
        }
        if (cpuAddrDecWin.enable == MV_TRUE)
        {
            cesaWin.addrWin.baseHigh = cpuAddrDecWin.addrWin.baseHigh;
            cesaWin.addrWin.baseLow  = cpuAddrDecWin.addrWin.baseLow;
            cesaWin.addrWin.size     = cpuAddrDecWin.addrWin.size;
            cesaWin.enable           = MV_TRUE;
            cesaWin.target           = tdmaAddrDecPrioTable[winPrioIndex];

#if defined(MV646xx) 
            /* Get the default attributes for that target window */
            mvCtrlDefAttribGet(cesaWin.target, &cesaWin.addrWinAttr);
#endif /* MV646xx */ 

            if(MV_OK != mvCesaTdmaWinSet(winNum, &cesaWin))
            {
                mvOsPrintf("mvCesaTdmaWinSet FAILED: winNum=%d\n",
                           winNum);
                return MV_ERROR;
            }
            winNum++;
        }
        winPrioIndex++;
    }
    return MV_OK;
}
#endif /* MV_CESA_VERSION >= 2 */




MV_STATUS mvCesaInit (int numOfSession, int queueDepth, char* pSramBase, void *osHandle)
{
    MV_U32 cesaCryptEngBase;
    MV_CPU_DEC_WIN addrDecWin;

    if(sizeof(MV_CESA_SRAM_MAP) > MV_CESA_SRAM_SIZE)
    {
        mvOsPrintf("mvCesaInit: Wrong SRAM map - %ld > %d\n",
                sizeof(MV_CESA_SRAM_MAP), MV_CESA_SRAM_SIZE);
        return MV_FAIL;
    }
#if 0
    if (mvCpuIfTargetWinGet(CRYPT_ENG, &addrDecWin) == MV_OK)
        cesaCryptEngBase = addrDecWin.addrWin.baseLow;
    else
    {
        mvOsPrintf("mvCesaInit: ERR. mvCpuIfTargetWinGet failed\n");
        return MV_ERROR;
    }
#else
        cesaCryptEngBase = (MV_U32)pSramBase;
#endif     

#if 0 /* Already done in the platform init */
#if (MV_CESA_VERSION >= 2)
    mvCesaTdmaAddrDecInit();
#endif /* MV_CESA_VERSION >= 2 */
#endif
	return mvCesaHalInit(numOfSession, queueDepth, pSramBase, cesaCryptEngBase, 
			     osHandle);

}
