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
#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"

#if defined(MV_INCLUDE_PEX)
#include "pex/mvPex.h"
#include "ctrlEnv/sys/mvSysPex.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#include "ctrlEnv/sys/mvSysGbe.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "ctrlEnv/sys/mvSysXor.h"
#endif

#if defined(MV_INCLUDE_SATA)
#include "ctrlEnv/sys/mvSysSata.h"
#endif

#if defined(MV_INCLUDE_USB)
#include "ctrlEnv/sys/mvSysUsb.h"
#endif

#if defined(MV_INCLUDE_AUDIO)
#include "ctrlEnv/sys/mvSysAudio.h"
#endif

#if defined(MV_INCLUDE_CESA)
#include "ctrlEnv/sys/mvSysCesa.h"
#endif

#if defined(MV_INCLUDE_TS)
#include "ctrlEnv/sys/mvSysTs.h"
#endif

/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
#else
	#define DB(x)
#endif	

/*******************************************************************************
* mvCtrlEnvInit - Initialize Marvell controller environment.
*
* DESCRIPTION:
*       This function get environment information and initialize controller
*       internal/external environment. For example
*       1) MPP settings according to board MPP macros.
*		NOTE: It is the user responsibility to shut down all DMA channels
*		in device and disable controller sub units interrupts during 
*		boot process.
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
MV_STATUS mvCtrlEnvInit(MV_VOID)
{
    	MV_U32 mppGroup;
	MV_U32 devId;
	MV_U32 boardId;
	MV_U32 i;
	MV_U32 maxMppGrp = 1;
	MV_U32 mppVal = 0;
	MV_U32 bootVal = 0;
	MV_U32 mppGroupType = 0;
	MV_U32 mppGroup1[][3] = MPP_GROUP_1_TYPE;
	MV_U32 mppGroup2[][3] = MPP_GROUP_2_TYPE;

	devId = mvCtrlModelGet();
	boardId= mvBoardIdGet();

	switch(devId){
		case MV_6281_DEV_ID:
			maxMppGrp = MV_6281_MPP_MAX_GROUP;
			break;
		case MV_6192_DEV_ID:
			maxMppGrp = MV_6192_MPP_MAX_GROUP;
			break;
        case MV_6190_DEV_ID:
            maxMppGrp = MV_6190_MPP_MAX_GROUP;
            break;
		case MV_6180_DEV_ID:
			maxMppGrp = MV_6180_MPP_MAX_GROUP;
			break;		
	}
	
	/* MPP Init */
	/* We split mpp init to 3 phases:
	 * 1. We init mpp[19:0] from the board info. mpp[23:20] will be over write 
	 * in phase 2.
	 * 2. We detect the mpp group type and according the mpp values [35:20].
	 * 3. We detect the mpp group type and according the mpp values [49:36].
	 */
	/* Mpp phase 1 mpp[19:0] */
	/* Read MPP group from board level and assign to MPP register */
	for (mppGroup = 0; mppGroup < 3; mppGroup++)
	{
		mppVal = mvBoardMppGet(mppGroup);
		if (mppGroup == 0)
		{
		    bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
		    if (mvCtrlIsBootFromSPI())
		    {
			mppVal &= ~0xffff;
			bootVal &= 0xffff;
			mppVal |= bootVal;
		    }
		    else if (mvCtrlIsBootFromSPIUseNAND())
		    {
			mppVal &= ~0xf0000000;
			bootVal &= 0xf0000000;
			mppVal |= bootVal;
		    }
		    else if (mvCtrlIsBootFromNAND())
		    {
			mppVal &= ~0xffffff;
			bootVal &= 0xffffff;
			mppVal |= bootVal;
		    }
		}
		
		if (mppGroup == 2)
		{
		    bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
		    if (mvCtrlIsBootFromNAND())
		    {
			mppVal &= ~0xff00;
			bootVal &= 0xff00;
			mppVal |= bootVal;
		    }
		}

		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* Identify MPPs group */
	mvBoardMppGroupIdUpdate();

	/* Update MPPs mux relevent only on Marvell DB */
	if ((boardId == DB_88F6281A_BP_ID) ||
		(boardId == DB_88F6180A_BP_ID))
		mvBoardMppMuxSet();

	mppGroupType = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_1);

	/* Mpp phase 2 */
	/* Read MPP group from board level and assign to MPP register */
    if (devId != MV_6180_DEV_ID)
    {
        i = 0;
    	for (mppGroup = 2; mppGroup < 5; mppGroup++)
    	{
    		if ((mppGroupType == MV_BOARD_OTHER) ||
    			(boardId == RD_88F6281A_ID) ||
    			(boardId == RD_88F6192A_ID) ||
                (boardId == RD_88F6190A_ID) ||
                (boardId == RD_88F6281A_PCAC_ID) ||
                (boardId == SHEEVA_PLUG_ID))
    			mppVal = mvBoardMppGet(mppGroup);
    		else
    		{
    			mppVal = mppGroup1[mppGroupType][i];
    			i++;
    		}
    
    		/* Group 2 is shared mpp[23:16] */
    		if (mppGroup == 2)
    		{
                bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
    			mppVal &= ~0xffff;
    			bootVal &= 0xffff;
    			mppVal |= bootVal;
    		}
    
    		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
    	}
    }

	if ((devId == MV_6192_DEV_ID) || (devId == MV_6190_DEV_ID))
		return MV_OK;
 	
	/* Mpp phase 3 */
	mppGroupType = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_2);
	/* Read MPP group from board level and assign to MPP register */
	i = 0;
	for (mppGroup = 4; mppGroup < 7; mppGroup++)
	{
		if ((mppGroupType == MV_BOARD_OTHER) ||
			(boardId == RD_88F6281A_ID) ||
            (boardId == RD_88F6281A_PCAC_ID) ||
            (boardId == SHEEVA_PLUG_ID))
			mppVal = mvBoardMppGet(mppGroup);
		else
		{
			mppVal = mppGroup2[mppGroupType][i];
			i++;
		}

		/* Group 4 is shared mpp[35:32] */
		if (mppGroup == 4)
		{
            bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			mppVal &= ~0xffff;
			bootVal &= 0xffff;
			mppVal |= bootVal;
		}

		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}
    /* Update SSCG configuration register*/
    if(mvBoardIdGet() == DB_88F6281A_BP_ID || mvBoardIdGet() == DB_88F6192A_BP_ID ||
       mvBoardIdGet() == DB_88F6190A_BP_ID || mvBoardIdGet() == DB_88F6180A_BP_ID)
        MV_REG_WRITE(0x100d8, 0x53);

	return MV_OK;
}

/*******************************************************************************
* mvCtrlMppRegGet - return reg address of mpp group
*
* DESCRIPTION:
*
* INPUT:
*       mppGroup - MPP group.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 - Register address.
*
*******************************************************************************/
MV_U32 mvCtrlMppRegGet(MV_U32 mppGroup)
{
        MV_U32 ret;

        switch(mppGroup){
                case (0):       ret = MPP_CONTROL_REG0;
                                break;
                case (1):       ret = MPP_CONTROL_REG1;
                                break;
                case (2):       ret = MPP_CONTROL_REG2;
                                break;
                case (3):       ret = MPP_CONTROL_REG3;
                                break;
                case (4):       ret = MPP_CONTROL_REG4;
                                break;
                case (5):       ret = MPP_CONTROL_REG5;
                                break;
                case (6):       ret = MPP_CONTROL_REG6;
                                break;
                default:        ret = MPP_CONTROL_REG0;
                                break;
        }
        return ret;
}
#if defined(MV_INCLUDE_PEX) 
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller 
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{

	return MV_PEX_MAX_IF;
}
#endif

#if defined(MV_INCLUDE_GIG_ETH)
/*******************************************************************************
* mvCtrlEthMaxPortGet - Get Marvell controller number of etherent ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of etherent port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of etherent port.
*
*******************************************************************************/
MV_U32 mvCtrlEthMaxPortGet(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_ETH_MAX_PORTS;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_ETH_MAX_PORTS;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_ETH_MAX_PORTS;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_ETH_MAX_PORTS;
			break;		
	}
	return 0;

}
#endif

#if defined(MV_INCLUDE_XOR)
/*******************************************************************************
* mvCtrlXorMaxChanGet - Get Marvell controller number of XOR channels.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR channels.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR channels.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxChanGet(MV_VOID)
{
	return MV_XOR_MAX_CHAN; 
}
#endif

#if defined(MV_INCLUDE_USB)
/*******************************************************************************
* mvCtrlUsbHostMaxGet - Get number of Marvell Usb  controllers
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
*       returns number of Marvell USB  controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsbMaxGet(void)
{
	return MV_USB_MAX_PORTS;
}
#endif


#if defined(MV_INCLUDE_NAND)
/*******************************************************************************
* mvCtrlNandSupport - Return if this controller has integrated NAND flash support
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
*       MV_TRUE if NAND is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlNandSupport(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_NAND;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_NAND;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_NAND;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_NAND;
			break;		
	}
	return 0;

}
#endif

#if defined(MV_INCLUDE_SDIO)
/*******************************************************************************
* mvCtrlSdioSupport - Return if this controller has integrated SDIO flash support
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
*       MV_TRUE if SDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlSdioSupport(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_SDIO;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_SDIO;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_SDIO;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_SDIO;
			break;		
	}
	return 0;

}
#endif

#if defined(MV_INCLUDE_TS)
/*******************************************************************************
* mvCtrlTsSupport - Return if this controller has integrated TS flash support
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
*       MV_TRUE if TS is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlTsSupport(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_TS;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_TS;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_TS;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_TS;
			break;		
	}
	return 0;
}
#endif

#if defined(MV_INCLUDE_AUDIO)
/*******************************************************************************
* mvCtrlAudioSupport - Return if this controller has integrated AUDIO flash support
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
*       MV_TRUE if AUDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlAudioSupport(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_AUDIO;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_AUDIO;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_AUDIO;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_AUDIO;
			break;		
	}
	return 0;

}
#endif

#if defined(MV_INCLUDE_TDM)
/*******************************************************************************
* mvCtrlTdmSupport - Return if this controller has integrated TDM flash support
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
*       MV_TRUE if TDM is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32	  mvCtrlTdmSupport(MV_VOID)
{
	MV_U32 devId;
	
	devId = mvCtrlModelGet();

	switch(devId){
		case MV_6281_DEV_ID:
			return MV_6281_TDM;
			break;
		case MV_6192_DEV_ID:
			return MV_6192_TDM;
			break;
        case MV_6190_DEV_ID:
            return MV_6190_TDM;
            break;
		case MV_6180_DEV_ID:
			return MV_6180_TDM;
			break;		
	}
	return 0;

}
#endif

/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit desscribing Marvell controller ID 
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	MV_U32 devId;
	
	devId = MV_REG_READ(CHIP_BOND_REG);
	devId &= PCKG_OPT_MASK;

	switch(devId){
		case 2:
			return	MV_6281_DEV_ID;
			break;
    case 1:
            if (((MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))& 0xffff0000) >> 16)
                 == MV_6190_DEV_ID)
                return	MV_6190_DEV_ID;
            else
                return	MV_6192_DEV_ID;
			break;
		case 0:
			return	MV_6180_DEV_ID;
			break;
	}

	return 0;
}
/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number 
*
*******************************************************************************/
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U8 revNum;
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID,0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	revNum = (MV_U8)MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0,PCI_CLASS_CODE_AND_REVISION_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return ((revNum & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS);
}

/*******************************************************************************
* mvCtrlNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*       
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlNameGet(char *pNameBuff)
{
	mvOsSPrintf (pNameBuff, "%s%x Rev %d", SOC_NAME_PREFIX, 
				mvCtrlModelGet(), mvCtrlRevGet()); 
	
	return MV_OK;
}

/*******************************************************************************
* mvCtrlModelRevGet - Get Controller Model (Device ID) and Revision
*
* DESCRIPTION:
*       This function returns 32bit value describing both Device ID and Revision
*       as defined in PCI Express Device and Vendor ID Register and device revision
*	    as defined in PCI Express Class Code and Revision ID Register.
     
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing both controller device ID and revision number
*
*******************************************************************************/
MV_U32	mvCtrlModelRevGet(MV_VOID)
{
	return ((mvCtrlModelGet() << 16) | mvCtrlRevGet());
}

/*******************************************************************************
* mvCtrlModelRevNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*       
*       MV_ERROR if informantion can not be read.
*******************************************************************************/

MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff)
{

        switch (mvCtrlModelRevGet())
        {
        case MV_6281_A0_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6281_A0_NAME); 
                break;
        case MV_6192_A0_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6192_A0_NAME); 
                break;
        case MV_6180_A0_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6180_A0_NAME); 
                break;
        case MV_6190_A0_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6190_A0_NAME); 
                break;
        case MV_6281_A1_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6281_A1_NAME);
                break;
        case MV_6192_A1_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6192_A1_NAME);
                break;
        case MV_6180_A1_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6180_A1_NAME);
                break;
        case MV_6190_A1_ID:
                mvOsSPrintf (pNameBuff, "%s",MV_6190_A1_NAME);
                break;
        default:
                mvCtrlNameGet(pNameBuff);
                break;
        }

        return MV_OK;
}


/*******************************************************************************
* ctrlWinOverlapTest - Test address windows for overlaping.
*
* DESCRIPTION:
*       This function checks the given two address windows for overlaping.
*
* INPUT:
*       pAddrWin1 - Address window 1.
*       pAddrWin2 - Address window 2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       
*       MV_TRUE if address window overlaps, MV_FALSE otherwise.
*******************************************************************************/
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2)
{
    MV_U32 winBase1, winBase2;
    MV_U32 winTop1, winTop2;
    
	/* check if we have overflow than 4G*/
	if (((0xffffffff - pAddrWin1->baseLow) < pAddrWin1->size-1)||
	   ((0xffffffff - pAddrWin2->baseLow) < pAddrWin2->size-1))
	{
		return MV_TRUE;
	}

    winBase1 = pAddrWin1->baseLow;
    winBase2 = pAddrWin2->baseLow;
    winTop1  = winBase1 + pAddrWin1->size-1;
    winTop2  = winBase2 + pAddrWin2->size-1;

    
    if (((winBase1 <= winTop2 ) && ( winTop2 <= winTop1)) ||
        ((winBase1 <= winBase2) && (winBase2 <= winTop1)))
    {
        return MV_TRUE;
    }
    else
    {
        return MV_FALSE;
    }
}

/*******************************************************************************
* ctrlWinWithinWinTest - Test address windows for overlaping.
*
* DESCRIPTION:
*       This function checks the given win1 boundries is within
*		win2 boundries.
*
* INPUT:
*       pAddrWin1 - Address window 1.
*       pAddrWin2 - Address window 2.
*
* OUTPUT:
*       None.
*
* RETURN:
*       
*       MV_TRUE if found win1 inside win2, MV_FALSE otherwise.
*******************************************************************************/
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2)
{
    MV_U32 winBase1, winBase2;
    MV_U32 winTop1, winTop2;
    
    winBase1 = pAddrWin1->baseLow;
    winBase2 = pAddrWin2->baseLow;
    winTop1  = winBase1 + pAddrWin1->size -1;
    winTop2  = winBase2 + pAddrWin2->size -1;
    
    if (((winBase1 >= winBase2 ) && ( winBase1 <= winTop2)) ||
        ((winTop1  >= winBase2) && (winTop1 <= winTop2)))
    {
        return MV_TRUE;
    }
    else
    {
        return MV_FALSE;
    }
}

static const char* cntrlName[] = TARGETS_NAME_ARRAY;

/*******************************************************************************
* mvCtrlTargetNameGet - Get Marvell controller target name
*
* DESCRIPTION:
*       This function convert the trget enumeration to string.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Target name (const MV_8 *)
*******************************************************************************/
const MV_8* mvCtrlTargetNameGet( MV_TARGET target )
{

	if (target >= MAX_TARGETS)
	{
		return "target unknown";
	}

	return cntrlName[target];
}

/*******************************************************************************
* mvCtrlAddrDecShow - Print the Controller units address decode map.
*
* DESCRIPTION:
*		This function the Controller units address decode map.
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
MV_VOID mvCtrlAddrDecShow(MV_VOID)
{
    mvCpuIfAddDecShow();
    mvAhbToMbusAddDecShow();
#if defined(MV_INCLUDE_PEX)
	mvPexAddrDecShow();
#endif
#if defined(MV_INCLUDE_USB)
    	mvUsbAddrDecShow();
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	mvEthAddrDecShow();
#endif
#if defined(MV_INCLUDE_XOR)
	mvXorAddrDecShow();
#endif
#if defined(MV_INCLUDE_SATA)
    mvSataAddrDecShow();
#endif
#if defined(MV_INCLUDE_AUDIO)
    mvAudioAddrDecShow();
#endif
#if defined(MV_INCLUDE_TS)
    mvTsuAddrDecShow();
#endif
}

/*******************************************************************************
* ctrlSizeToReg - Extract size value for register assignment.
*
* DESCRIPTION:		
*       Address decode size parameter must be programed from LSB to MSB as
*       sequence of 1's followed by sequence of 0's. The number of 1's 
*       specifies the size of the window in 64 KB granularity (e.g. a 
*       value of 0x00ff specifies 256x64k = 16 MB).
*       This function extract the size value from the size parameter according 
*		to given aligment paramter. For example for size 0x1000000 (16MB) and 
*		aligment 0x10000 (64KB) the function will return 0x00FF.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size register value correspond to size parameter. 
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32	ctrlSizeToReg(MV_U32 size, MV_U32 alignment)
{
	MV_U32 retVal;

	/* Check size parameter alignment		*/
	if ((0 == size) || (MV_IS_NOT_ALIGN(size, alignment)))
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size is zero or not aligned.\n"));
		return -1;
	}
	
	/* Take out the "alignment" portion out of the size parameter */
	alignment--;	/* Now the alignmet is a sequance of '1' (e.g. 0xffff) 		*/
					/* and size is 0x1000000 (16MB) for example	*/
	while(alignment & 1)	/* Check that alignmet LSB is set	*/
	{
		size = (size >> 1); /* If LSB is set, move 'size' one bit to right	*/	
		alignment = (alignment >> 1);
	}
	
	/* If after the alignment first '0' was met we still have '1' in 		*/
	/* it then aligment is invalid (not power of 2) 				*/
	if (alignment)
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", 
			(MV_U32)alignment));
		return -1;
	}

	/* Now the size is shifted right according to aligment: 0x0100			*/
	size--;         /* Now the size is a sequance of '1': 0x00ff 			*/
    
	retVal = size ;
	
	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's		*/
	while(size & 1)	/* Check that LSB is set	*/
	{
		size = (size >> 1); /* If LSB is set, move one bit to the right		*/	
	}

    if (size) /* Sequance of 1's is over. Check that we have no other 1's		*/
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size parameter 0x%x invalid.\n", 
                                                                        size));
		return -1;
	}
	
    return retVal;
	
}

/*******************************************************************************
* ctrlRegToSize - Extract size value from register value.
*
* DESCRIPTION:		
*       This function extract a size value from the register size parameter 
*		according to given aligment paramter. For example for register size 
*		value 0xff and aligment 0x10000 the function will return 0x01000000.
*
* INPUT:
*       regSize   - Size as in register format.	See ctrlSizeToReg.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size. 
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32	ctrlRegToSize(MV_U32 regSize, MV_U32 alignment)
{
   	MV_U32 temp;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's		*/ 
	temp = regSize;		/* Now the size is a sequance of '1': 0x00ff		*/
	
	while(temp & 1)	/* Check that LSB is set					*/
	{
		temp = (temp >> 1); /* If LSB is set, move one bit to the right		*/	
	}

    if (temp) /* Sequance of 1's is over. Check that we have no other 1's		*/
	{
		DB(mvOsPrintf("ctrlRegToSize: ERR. Size parameter 0x%x invalid.\n", 
					regSize));
	   	return -1;
	}
	

	/* Check that aligment is a power of two					*/
	temp = alignment - 1;/* Now the alignmet is a sequance of '1' (0xffff) 		*/
					
	while(temp & 1)	/* Check that alignmet LSB is set				*/
	{
		temp = (temp >> 1); /* If LSB is set, move 'size' one bit to right	*/	
	}
	
	/* If after the 'temp' first '0' was met we still have '1' in 'temp'		*/
	/* then 'temp' is invalid (not power of 2) 					*/
	if (temp)
	{
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", 
					alignment));
		return -1;
	}

	regSize++;      /* Now the size is 0x0100					*/

	/* Add in the "alignment" portion to the register size parameter 		*/
	alignment--;	/* Now the alignmet is a sequance of '1' (e.g. 0xffff) 		*/

	while(alignment & 1)	/* Check that alignmet LSB is set			*/
	{
		regSize   = (regSize << 1); /* LSB is set, move 'size' one bit left	*/	
		alignment = (alignment >> 1);
	}
		
    return regSize;	
}


/*******************************************************************************
* ctrlSizeRegRoundUp - Round up given size 
*
* DESCRIPTION:		
*       This function round up a given size to a size that fits the 
*       restrictions of size format given an aligment parameter.
*		to given aligment paramter. For example for size parameter 0xa1000 and 
*		aligment 0x1000 the function will return 0xFF000.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size value correspond to size in register.  
*******************************************************************************/
MV_U32	ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment)
{
	MV_U32 msbBit = 0;
    MV_U32 retSize;
	
    /* Check if size parameter is already comply with restriction		*/
	if (!(-1 == ctrlSizeToReg(size, alignment)))
	{
		return size;
	}
    
    while(size)
	{
		size = (size >> 1);
        msbBit++;
	}

    retSize = (1 << msbBit);
    
    if (retSize < alignment)
    {
        return alignment;
    }
    else
    {
        return retSize;
    }
}
/*******************************************************************************
* mvCtrlSysRstLengthCounterGet - Return number of milliseconds the reset button 
* 				 was pressed and clear counter
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN: number of milliseconds the reset button was pressed 
*******************************************************************************/
MV_U32	mvCtrlSysRstLengthCounterGet(MV_VOID)
{
	static volatile MV_U32 Count = 0;

	if(!Count) {
		Count = (MV_REG_READ(SYSRST_LENGTH_COUNTER_REG) & SLCR_COUNT_MASK);
		Count = (Count / (MV_BOARD_REFCLK_25MHZ / 1000));
		/* clear counter for next boot */
		MV_REG_BIT_SET(SYSRST_LENGTH_COUNTER_REG, SLCR_CLR_MASK);	
	}

	DB(mvOsPrintf("mvCtrlSysRstLengthCounterGet: Reset button was pressed for %u milliseconds\n", Count));

	return Count;		
}

MV_BOOL	  mvCtrlIsBootFromSPI(MV_VOID)
{
    MV_U32 satr = 0;
    satr = MV_REG_READ(MPP_SAMPLE_AT_RESET);
    if(mvCtrlModelGet() == MV_6180_DEV_ID)
    {
        if (MSAR_BOOT_MODE_6180(satr) == MSAR_BOOT_SPI_WITH_BOOTROM_6180)
            return MV_TRUE;
        else
            return MV_FALSE;
    }
    satr = satr & MSAR_BOOT_MODE_MASK;    
    if (satr == MSAR_BOOT_SPI_WITH_BOOTROM)
        return MV_TRUE;
    else
        return MV_FALSE;
}

MV_BOOL	  mvCtrlIsBootFromSPIUseNAND(MV_VOID)
{
    MV_U32 satr = 0;
    if(mvCtrlModelGet() == MV_6180_DEV_ID)
        return MV_FALSE;
    satr = MV_REG_READ(MPP_SAMPLE_AT_RESET);
    satr = satr & MSAR_BOOT_MODE_MASK;
    
    if (satr == MSAR_BOOT_SPI_USE_NAND_WITH_BOOTROM)
        return MV_TRUE;
    else
        return MV_FALSE;
}

MV_BOOL	  mvCtrlIsBootFromNAND(MV_VOID)
{
    MV_U32 satr = 0;
    satr = MV_REG_READ(MPP_SAMPLE_AT_RESET);
    if(mvCtrlModelGet() == MV_6180_DEV_ID)
    {
        if (MSAR_BOOT_MODE_6180(satr) == MSAR_BOOT_NAND_WITH_BOOTROM_6180)
            return MV_TRUE;
        else
            return MV_FALSE;
    }
    satr = satr & MSAR_BOOT_MODE_MASK;    
    if ((satr == MSAR_BOOT_NAND_WITH_BOOTROM))
        return MV_TRUE;
    else
        return MV_FALSE;
}

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
/*******************************************************************************
* mvCtrlPwrSaveOn - Set Power save mode
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID   mvCtrlPwrSaveOn(MV_VOID)
{
	unsigned long old,temp;
	/* Disable int */
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");

	/* Set SoC in power save */
	MV_REG_BIT_SET(POWER_MNG_CTRL_REG, BIT11);
	/* Wait for int */
	__asm__ __volatile__("mcr    p15, 0, r0, c7, c0, 4");

	/* Enabled int */
	__asm__ __volatile__("msr cpsr_c, %0"
			     :
			     : "r" (old)
			     : "memory");
}



/*******************************************************************************
* mvCtrlPwrSaveOff - Go out of power save mode
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID   mvCtrlPwrSaveOff(MV_VOID)
{
	unsigned long old,temp;
	/* Disable int */
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");

	/* Set SoC in power save */
	MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, BIT11);
	/* Wait for int */
	__asm__ __volatile__("mcr    p15, 0, r0, c7, c0, 4");

	/* Enabled int */
	__asm__ __volatile__("msr cpsr_c, %0"
			     :
			     : "r" (old)
			     : "memory");
}

/*******************************************************************************
* mvCtrlPwrClckSet - Set Power State for specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID   mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	switch (unitId)
    {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));
		}
		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK(index));
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK(index));
		}
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SESTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SESTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_AUDIOSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_AUDIOSTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_TS)
	case TS_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_TSSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_TSSTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);
		}
		else
		{
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);
		}
		break;
#endif

	default:

		break;

	}
}

/*******************************************************************************
* mvCtrlPwrClckGet - Get Power State of specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL		mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(POWER_MNG_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	switch (unitId)
    {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPCLOCK_MASK) == PMC_PEXSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;

		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & PMC_GESTOPCLOCK_MASK(index)) == PMC_GESTOPCLOCK_STOP(index))
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATASTOPCLOCK_MASK(index)) == PMC_SATASTOPCLOCK_STOP(index))
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if ((reg & PMC_SESTOPCLOCK_MASK) == PMC_SESTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USBSTOPCLOCK_MASK) == PMC_USBSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if ((reg & PMC_AUDIOSTOPCLOCK_MASK) == PMC_AUDIOSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_TS)
	case TS_UNIT_ID:
		if ((reg & PMC_TSSTOPCLOCK_MASK) == PMC_TSSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & PMC_SDIOSTOPCLOCK_MASK)== PMC_SDIOSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_UNIT_ID:
		if ((reg & PMC_TDMSTOPCLOCK_MASK) == PMC_TDMSTOPCLOCK_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif

	default:
		state = MV_TRUE;
		break;
	}


	return state;	
}
/*******************************************************************************
* mvCtrlPwrMemSet - Set Power State for memory on specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID   mvCtrlPwrMemSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	switch (unitId)
    {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_PEXSTOPMEM_MASK);
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_PEXSTOPMEM_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_GESTOPMEM_MASK(index));
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_GESTOPMEM_MASK(index));
		}
		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_SATASTOPMEM_MASK(index));
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_SATASTOPMEM_MASK(index));
		}
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_SESTOPMEM_MASK);
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_SESTOPMEM_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_USBSTOPMEM_MASK);
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_USBSTOPMEM_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_AUDIOSTOPMEM_MASK);
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_AUDIOSTOPMEM_MASK);
		}
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if (enable == MV_FALSE)
		{
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_XORSTOPMEM_MASK(index));
		}
		else
		{
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_XORSTOPMEM_MASK(index));
		}
		break;
#endif
	default:

		break;

	}
}

/*******************************************************************************
* mvCtrlPwrMemGet - Get Power State of memory on specific Unit
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL		mvCtrlPwrMemGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	switch (unitId)
    {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPMEM_MASK) == PMC_PEXSTOPMEM_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;

		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & PMC_GESTOPMEM_MASK(index)) == PMC_GESTOPMEM_STOP(index))
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATASTOPMEM_MASK(index)) == PMC_SATASTOPMEM_STOP(index))
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if ((reg & PMC_SESTOPMEM_MASK) == PMC_SESTOPMEM_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USBSTOPMEM_MASK) == PMC_USBSTOPMEM_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_AUDIO)
	case AUDIO_UNIT_ID:
		if ((reg & PMC_AUDIOSTOPMEM_MASK) == PMC_AUDIOSTOPMEM_STOP)
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if ((reg & PMC_XORSTOPMEM_MASK(index)) == PMC_XORSTOPMEM_STOP(index))
		{
			state = MV_FALSE;
		}
		else state = MV_TRUE;
		break;
#endif

	default:
		state = MV_TRUE;
		break;
	}


	return state;	
}
#else
MV_VOID   mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable) {return;}
MV_BOOL	  mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index) {return MV_TRUE;}
#endif /* #if defined(MV_INCLUDE_CLK_PWR_CNTRL) */


/*******************************************************************************
* mvMPPConfigToSPI - Change MPP[3:0] configuration to SPI mode
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_VOID   mvMPPConfigToSPI(MV_VOID)
{
	MV_U32 mppVal = 0;
	MV_U32 bootVal = 0;

    if(!mvCtrlIsBootFromSPIUseNAND())
        return;
    mppVal = 0x00002220; /* Set MPP [3:1] to SPI mode */
    bootVal = MV_REG_READ(mvCtrlMppRegGet(0));
    bootVal &= 0xffff000f;
        mppVal |= bootVal;
    
    MV_REG_WRITE(mvCtrlMppRegGet(0), mppVal);
}


/*******************************************************************************
* mvMPPConfigToDefault - Change MPP[7:0] configuration to default configuration
*
* DESCRIPTION:		
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_VOID   mvMPPConfigToDefault(MV_VOID)
{
	MV_U32 mppVal = 0;
	MV_U32 bootVal = 0;

    if(!mvCtrlIsBootFromSPIUseNAND())
        return;
    mppVal = mvBoardMppGet(0);
    bootVal = MV_REG_READ(mvCtrlMppRegGet(0));
    mppVal &= ~0xffff000f;
    bootVal &= 0xffff000f;
        mppVal |= bootVal;
    
    MV_REG_WRITE(mvCtrlMppRegGet(0), mppVal);
}


