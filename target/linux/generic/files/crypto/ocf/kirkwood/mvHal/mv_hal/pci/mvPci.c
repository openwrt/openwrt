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
#include "pci/mvPci.h"

#include "ctrlEnv/mvCtrlEnvLib.h"

/* defines  */       
#ifdef MV_DEBUG         
	#define DB(x)	x
#else                
	#define DB(x)    
#endif	             
					 


MV_VOID mvPciHalInit(MV_U32 pciIf, MV_PCI_MOD pciIfmod)
{
        if (MV_PCI_MOD_HOST == pciIfmod)
    {

                mvPciLocalBusNumSet(pciIf, PCI_HOST_BUS_NUM(pciIf));
                mvPciLocalDevNumSet(pciIf, PCI_HOST_DEV_NUM(pciIf));
        
                /* Local device master Enable */
                mvPciMasterEnable(pciIf, MV_TRUE);
        
                /* Local device slave Enable */
                mvPciSlaveEnable(pciIf, mvPciLocalBusNumGet(pciIf),
                                                 mvPciLocalDevNumGet(pciIf), MV_TRUE);
        }
        /* enable CPU-2-PCI ordering */
        MV_REG_BIT_SET(PCI_CMD_REG(0), PCR_CPU_TO_PCI_ORDER_EN);
}

/*******************************************************************************
* mvPciCommandSet - Set PCI comman register value.
*
* DESCRIPTION:
*       This function sets a given PCI interface with its command register 
*       value.
*
* INPUT:
*       pciIf   - PCI interface number.
*       command - 32bit value to be written to comamnd register.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if pciIf is not in range otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciCommandSet(MV_U32 pciIf, MV_U32 command)
{
    MV_U32 locBusNum, locDevNum, regVal;

    locBusNum =  mvPciLocalBusNumGet(pciIf);
    locDevNum =  mvPciLocalDevNumGet(pciIf);

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciCommandSet: ERR. Invalid PCI IF num %d\n", pciIf);
		return MV_BAD_PARAM;
	}

	/* Set command register */
	MV_REG_WRITE(PCI_CMD_REG(pciIf), command);

    /* Upodate device max outstanding split tarnsaction */
    if ((command & PCR_CPU_TO_PCI_ORDER_EN) && 
        (command & PCR_PCI_TO_CPU_ORDER_EN))
    {
        /* Read PCI-X command register */
        regVal = mvPciConfigRead (pciIf, locBusNum, locDevNum, 0, PCIX_COMMAND);
                        
        /* clear bits 22:20 */
        regVal &= 0xff8fffff;

        /* set reset value */
        regVal |= (0x3 << 20);
        
        /* Write back the value */
        mvPciConfigWrite (pciIf, locBusNum, locDevNum, 0, PCIX_COMMAND, regVal);
    }

	return MV_OK;


}


/*******************************************************************************
* mvPciModeGet - Get PCI interface mode.
*
* DESCRIPTION:
*       This function returns the given PCI interface mode.
*
* INPUT:
*       pciIf   - PCI interface number.
*
* OUTPUT:
*       pPciMode - Pointer to PCI mode structure.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciModeGet(MV_U32 pciIf, MV_PCI_MODE *pPciMode)
{
	MV_U32 pciMode;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciModeGet: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_BAD_PARAM;
	}
	if (NULL == pPciMode)
	{
		mvOsPrintf("mvPciModeGet: ERR. pPciMode = NULL  \n");
		return MV_BAD_PARAM;
	}

	/* Read pci mode register */
	pciMode = MV_REG_READ(PCI_MODE_REG(pciIf));
	
	switch (pciMode & PMR_PCI_MODE_MASK)
	{
		case PMR_PCI_MODE_CONV:
            pPciMode->pciType  = MV_PCI_CONV;

			if (MV_REG_READ(PCI_DLL_CTRL_REG(pciIf)) & PDC_DLL_EN)
			{
				pPciMode->pciSpeed = 66000000; /* 66MHZ */
			}
			else
			{
				pPciMode->pciSpeed = 33000000; /* 33MHZ */
			}
			
			break;
		
		case PMR_PCI_MODE_PCIX_66MHZ:	
			pPciMode->pciType  = MV_PCIX;
			pPciMode->pciSpeed = 66000000; /* 66MHZ */	
			break;
		
		case PMR_PCI_MODE_PCIX_100MHZ:	
			pPciMode->pciType  = MV_PCIX;
			pPciMode->pciSpeed = 100000000; /* 100MHZ */	
			break;
		
		case PMR_PCI_MODE_PCIX_133MHZ:	
			pPciMode->pciType  = MV_PCIX;
			pPciMode->pciSpeed = 133000000; /* 133MHZ */	
			break;

		default:
			{
				mvOsPrintf("mvPciModeGet: ERR. Non existing mode !!\n");
				return MV_ERROR;
			}
	}

	switch (pciMode & PMR_PCI_64_MASK)
	{
		case PMR_PCI_64_64BIT:
			pPciMode->pciWidth = MV_PCI_64;
			break;
		
		case PMR_PCI_64_32BIT:
            pPciMode->pciWidth = MV_PCI_32;
            break;
		
		default:
			{
				mvOsPrintf("mvPciModeGet: ERR. Non existing mode !!\n");
				return MV_ERROR;
			}
	}
	
	return MV_OK;
}

/*******************************************************************************
* mvPciRetrySet - Set PCI retry counters
*
* DESCRIPTION:
*       This function specifies the number of times the PCI controller 
*       retries a transaction before it quits.
*       Applies to the PCI Master when acting as a requester.
*       Applies to the PCI slave when acting as a completer (PCI-X mode).
*       A 0x00 value means a "retry forever".
*
* INPUT:
*       pciIf   - PCI interface number.
*       counter - Number of times PCI controller retry. Use counter value 
*                 up to PRR_RETRY_CNTR_MAX.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciRetrySet(MV_U32 pciIf, MV_U32 counter)
{
	MV_U32 pciRetry;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciRetrySet: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_BAD_PARAM;
	}

	if (counter >= PRR_RETRY_CNTR_MAX)
	{
		mvOsPrintf("mvPciRetrySet: ERR. Invalid counter: %d\n", counter);
		return MV_BAD_PARAM;

	}

	/* Reading PCI retry register */
    pciRetry  = MV_REG_READ(PCI_RETRY_REG(pciIf));

	pciRetry &= ~PRR_RETRY_CNTR_MASK;

	pciRetry |= (counter << PRR_RETRY_CNTR_OFFS);

	/* write new value */
	MV_REG_WRITE(PCI_RETRY_REG(pciIf), pciRetry);

	return MV_OK;
}


/*******************************************************************************
* mvPciDiscardTimerSet - Set PCI discard timer
*
* DESCRIPTION:
*       This function set PCI discard timer.
*       In conventional PCI mode:
*       Specifies the number of PCLK cycles the PCI slave keeps a non-accessed
*       read buffers (non-completed delayed read) before invalidate the buffer.
*       Set to '0' to disable the timer. The PCI slave waits for delayed 
*       read completion forever.
*       In PCI-X mode:
*       Specifies the number of PCLK cycles the PCI master waits for split
*       completion transaction, before it invalidates the pre-allocated read
*       buffer.
*       Set to '0' to disable the timer. The PCI master waits for split 
*       completion forever.
*       NOTE: Must be set to a number greater than MV_PCI_MAX_DISCARD_CLK, 
*       unless using the "wait for ever" setting 0x0.
*       NOTE: Must not be updated while there are pending read requests.
*
* INPUT:
*       pciIf      - PCI interface number.
*       pClkCycles - Number of PCI clock cycles.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciDiscardTimerSet(MV_U32 pciIf, MV_U32 pClkCycles)
{
	MV_U32 pciDiscardTimer;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciDiscardTimerSet: ERR. Invalid PCI interface %d\n", 
																		pciIf);
		return MV_BAD_PARAM;
	}

	if (pClkCycles >= PDTR_TIMER_MIN)
	{
		mvOsPrintf("mvPciDiscardTimerSet: ERR. Invalid Clk value: %d\n", 	
																   pClkCycles);
		return MV_BAD_PARAM;

	}

	/* Read  PCI Discard Timer */
	pciDiscardTimer  = MV_REG_READ(PCI_DISCARD_TIMER_REG(pciIf));

	pciDiscardTimer &= ~PDTR_TIMER_MASK;

    pciDiscardTimer |= (pClkCycles << PDTR_TIMER_OFFS);

	/* Write new value */
	MV_REG_WRITE(PCI_DISCARD_TIMER_REG(pciIf), pciDiscardTimer);

	return MV_OK;

}

/* PCI Arbiter routines */

/*******************************************************************************
* mvPciArbEnable - PCI arbiter enable/disable
*
* DESCRIPTION:
*       This fuction enable/disables a given PCI interface arbiter.
*       NOTE: Arbiter setting can not be changed while in work. It should only 
*             be set once.
* INPUT:
*       pciIf  - PCI interface number.
*       enable - Enable/disable parameter. If enable = MV_TRUE then enable.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvPciArbEnable(MV_U32 pciIf, MV_BOOL enable)
{
	MV_U32 regVal;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciArbEnable: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_ERROR;
	}
    	
    /* Set PCI Arbiter Control register according to default configuration 	*/
	regVal = MV_REG_READ(PCI_ARBITER_CTRL_REG(pciIf));

	/* Make sure arbiter disabled before changing its values */
	MV_REG_BIT_RESET(PCI_ARBITER_CTRL_REG(pciIf), PACR_ARB_ENABLE); 

	regVal &= ~PCI_ARBITER_CTRL_DEFAULT_MASK;	

	regVal |= PCI_ARBITER_CTRL_DEFAULT;		/* Set default configuration	*/

	if (MV_TRUE == enable)
	{
		regVal |= PACR_ARB_ENABLE; 
	}
	else
	{
		regVal &= ~PACR_ARB_ENABLE; 
	}

	/* Write to register 										            */
	MV_REG_WRITE(PCI_ARBITER_CTRL_REG(pciIf), regVal);

	return MV_OK;	
}


/*******************************************************************************
* mvPciArbParkDis - Disable arbiter parking on agent
*
* DESCRIPTION:
*       This function disables the PCI arbiter from parking on the given agent
*       list.
*
* INPUT:
*       pciIf        - PCI interface number.
*       pciAgentMask - When a bit in the mask is set to '1', parking on 
*                      the associated PCI master is disabled. Mask bit 
*                      refers to bit 0 - 6. For example disable parking on PCI
*                      agent 3 set pciAgentMask 0x4 (bit 3 is set).
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvPciArbParkDis(MV_U32 pciIf, MV_U32 pciAgentMask)
{
	MV_U32 pciArbiterCtrl;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciArbParkDis: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_ERROR;
	}

	/* Reading Arbiter Control register */
	pciArbiterCtrl = MV_REG_READ(PCI_ARBITER_CTRL_REG(pciIf));

	/* Arbiter must be disabled before changing parking */
	MV_REG_BIT_RESET(PCI_ARBITER_CTRL_REG(pciIf), PACR_ARB_ENABLE); 

	/* do the change */
    pciArbiterCtrl &= ~PACR_PARK_DIS_MASK;
	pciArbiterCtrl |= (pciAgentMask << PACR_PARK_DIS_OFFS);

	/* writing new value ( if th earbiter was enabled before the change		*/ 
	/* here it will be reenabled 											*/
	MV_REG_WRITE(PCI_ARBITER_CTRL_REG(pciIf), pciArbiterCtrl);

	return MV_OK;
}


/*******************************************************************************
* mvPciArbBrokDetectSet - Set PCI arbiter broken detection
*
* DESCRIPTION:
*       This function sets the maximum number of cycles that the arbiter 
*       waits for a PCI master to respond to its grant assertion. If a 
*       PCI agent fails to respond within this time, the PCI arbiter aborts 
*       the transaction and performs a new arbitration cycle.
*       NOTE: Value must be greater than '1' for conventional PCI and 
*       greater than '5' for PCI-X.
*
* INPUT:
*       pciIf      - PCI interface number.
*       pClkCycles - Number of PCI clock cycles. If equal to '0' the broken
*                    master detection is disabled.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciArbBrokDetectSet(MV_U32 pciIf, MV_U32 pClkCycles)
{
	MV_U32 pciArbiterCtrl;
	MV_U32 pciMode;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciArbBrokDetectSet: ERR. Invalid PCI interface %d\n", 
																		pciIf);
		return MV_BAD_PARAM;
	}

	/* Checking PCI mode and if pClkCycles is legal value */
	pciMode = MV_REG_READ(PCI_MODE_REG(pciIf));
	pciMode &= PMR_PCI_MODE_MASK;

	if (PMR_PCI_MODE_CONV == pciMode)
	{
		if (pClkCycles < PACR_BROKEN_VAL_CONV_MIN) 
			return MV_ERROR;
	}
	else
	{
		if (pClkCycles < PACR_BROKEN_VAL_PCIX_MIN) 
			return MV_ERROR;
	}

	pClkCycles <<= PACR_BROKEN_VAL_OFFS;

	/* Reading Arbiter Control register */
	pciArbiterCtrl  = MV_REG_READ(PCI_ARBITER_CTRL_REG(pciIf));
	pciArbiterCtrl &= ~PACR_BROKEN_VAL_MASK;
	pciArbiterCtrl |= pClkCycles;

	/* Arbiter must be disabled before changing broken detection */
	MV_REG_BIT_RESET(PCI_ARBITER_CTRL_REG(pciIf), PACR_ARB_ENABLE); 

	/* writing new value ( if th earbiter was enabled before the change 	*/
	/* here it will be reenabled 											*/

	MV_REG_WRITE(PCI_ARBITER_CTRL_REG(pciIf), pciArbiterCtrl);

	return MV_OK;
}

/* PCI configuration space read write */

/*******************************************************************************
* mvPciConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to read from local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit register data, 0xffffffff on error
*
*******************************************************************************/
MV_U32 mvPciConfigRead (MV_U32 pciIf, MV_U32 bus, MV_U32 dev, MV_U32 func, 
                        MV_U32 regOff)
{
	MV_U32 pciData = 0;

	/* Parameter checking   */
	if (PCI_DEFAULT_IF != pciIf)
	{
		if (pciIf >= mvCtrlPciMaxIfGet())
		{
			mvOsPrintf("mvPciConfigRead: ERR. Invalid PCI interface %d\n",pciIf);
			return 0xFFFFFFFF;
		}
	}

	if (dev >= MAX_PCI_DEVICES)
	{
		DB(mvOsPrintf("mvPciConfigRead: ERR. device number illigal %d\n", dev));
		return 0xFFFFFFFF;
	}
	
	if (func >= MAX_PCI_FUNCS)
	{
		DB(mvOsPrintf("mvPciConfigRead: ERR. function number illigal %d\n", func));
		return 0xFFFFFFFF;
	}
	
	if (bus >= MAX_PCI_BUSSES)
	{
		DB(mvOsPrintf("mvPciConfigRead: ERR. bus number illigal %d\n", bus));
		return MV_ERROR;
	}


	/* Creating PCI address to be passed */
	pciData |= (bus << PCAR_BUS_NUM_OFFS);
	pciData |= (dev << PCAR_DEVICE_NUM_OFFS);
	pciData |= (func << PCAR_FUNC_NUM_OFFS);
	pciData |= (regOff & PCAR_REG_NUM_MASK);

	pciData |= PCAR_CONFIG_EN; 
	
	/* Write the address to the PCI configuration address register */
	MV_REG_WRITE(PCI_CONFIG_ADDR_REG(pciIf), pciData);

	/* In order to let the PCI controller absorbed the address of the read 	*/
	/* transaction we perform a validity check that the address was written */
	if(pciData != MV_REG_READ(PCI_CONFIG_ADDR_REG(pciIf)))
	{
		return MV_ERROR;
	}
	/* Read the Data returned in the PCI Data register */
	pciData = MV_REG_READ(PCI_CONFIG_DATA_REG(pciIf));

	return pciData;
}

/*******************************************************************************
* mvPciConfigWrite - Write to configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit write to PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to write to local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*       data    - 32bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciConfigWrite(MV_U32 pciIf, MV_U32 bus, MV_U32 dev, 
                           MV_U32 func, MV_U32 regOff, MV_U32 data)
{
	MV_U32 pciData = 0;

	/* Parameter checking   */
	if (PCI_DEFAULT_IF != pciIf)
	{
		if (pciIf >= mvCtrlPciMaxIfGet())
		{
			mvOsPrintf("mvPciConfigWrite: ERR. Invalid PCI interface %d\n", 
																		pciIf);
			return 0xFFFFFFFF;
		}
	}

	if (dev >= MAX_PCI_DEVICES)
	{
		mvOsPrintf("mvPciConfigWrite: ERR. device number illigal %d\n",dev);
		return MV_BAD_PARAM;
	}

	if (func >= MAX_PCI_FUNCS)
	{
		mvOsPrintf("mvPciConfigWrite: ERR. function number illigal %d\n", func);
		return MV_ERROR;
	}

	if (bus >= MAX_PCI_BUSSES)
	{
		mvOsPrintf("mvPciConfigWrite: ERR. bus number illigal %d\n", bus);
		return MV_ERROR;
	}

	/* Creating PCI address to be passed */
	pciData |= (bus << PCAR_BUS_NUM_OFFS);
	pciData |= (dev << PCAR_DEVICE_NUM_OFFS);
	pciData |= (func << PCAR_FUNC_NUM_OFFS);
	pciData |= (regOff & PCAR_REG_NUM_MASK);

	pciData |= PCAR_CONFIG_EN;
	
	/* Write the address to the PCI configuration address register */
	MV_REG_WRITE(PCI_CONFIG_ADDR_REG(pciIf), pciData);

	/* In order to let the PCI controller absorbed the address of the read 	*/
	/* transaction we perform a validity check that the address was written */
	if(pciData != MV_REG_READ(PCI_CONFIG_ADDR_REG(pciIf)))
	{
		return MV_ERROR;
	}

	/* Write the Data passed to the PCI Data register */
	MV_REG_WRITE(PCI_CONFIG_DATA_REG(pciIf), data);

	return MV_OK;
}

/*******************************************************************************
* mvPciMasterEnable - Enable/disale PCI interface master transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PCI command status 
*       (offset 0x4) to set/reset bit 2. After this bit is set, the PCI 
*       master is allowed to gain ownership on the bus, otherwise it is 
*       incapable to do so.
*
* INPUT:
*       pciIf  - PCI interface number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciMasterEnable(MV_U32 pciIf, MV_BOOL enable)
{
	MV_U32 pciCommandStatus;
	MV_U32 RegOffs;
	MV_U32 localBus;
	MV_U32 localDev;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciMasterEnable: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_ERROR;
	}

	localBus = mvPciLocalBusNumGet(pciIf);
	localDev = mvPciLocalDevNumGet(pciIf);
	
	RegOffs = PCI_STATUS_AND_COMMAND;

	pciCommandStatus = mvPciConfigRead(pciIf, localBus, localDev, 0, RegOffs);

	if (MV_TRUE == enable)
	{
		pciCommandStatus |= PSCR_MASTER_EN;
	}
	else
	{
		pciCommandStatus &= ~PSCR_MASTER_EN;
	}

	mvPciConfigWrite(pciIf, localBus, localDev, 0, RegOffs, pciCommandStatus);

	return MV_OK;
}


/*******************************************************************************
* mvPciSlaveEnable - Enable/disale PCI interface slave transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PCI command status 
*       (offset 0x4) to set/reset bit 0 and 1. After those bits are set, 
*       the PCI slave is allowed to respond to PCI IO space access (bit 0) 
*       and PCI memory space access (bit 1). 
*
* INPUT:
*       pciIf  - PCI interface number.
*       dev     - PCI device number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciSlaveEnable(MV_U32 pciIf, MV_U32 bus, MV_U32 dev, MV_BOOL enable)
{
	MV_U32 pciCommandStatus;
	MV_U32 RegOffs;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciSlaveEnable: ERR. Invalid PCI interface %d\n", pciIf);
		return MV_BAD_PARAM;
	}
	if (dev >= MAX_PCI_DEVICES)
	{
		mvOsPrintf("mvPciLocalDevNumSet: ERR. device number illigal %d\n", dev);
		return MV_BAD_PARAM;

	}

	RegOffs = PCI_STATUS_AND_COMMAND;
	
	pciCommandStatus=mvPciConfigRead(pciIf, bus, dev, 0, RegOffs);

    if (MV_TRUE == enable)
	{
		pciCommandStatus |= (PSCR_IO_EN | PSCR_MEM_EN);
	}
	else                             
	{
		pciCommandStatus &= ~(PSCR_IO_EN | PSCR_MEM_EN);
	}

	mvPciConfigWrite(pciIf, bus, dev, 0, RegOffs, pciCommandStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPciLocalBusNumSet - Set PCI interface local bus number.
*
* DESCRIPTION:
*       This function sets given PCI interface its local bus number.
*       Note: In case the PCI interface is PCI-X, the information is read-only.
*
* INPUT:
*       pciIf  - PCI interface number.
*       busNum - Bus number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PCI interface is PCI-X. 
*       MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciLocalBusNumSet(MV_U32 pciIf, MV_U32 busNum)
{
	MV_U32 pciP2PConfig;
	MV_PCI_MODE pciMode;
	MV_U32 localBus;
	MV_U32 localDev;


	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciLocalBusNumSet: ERR. Invalid PCI interface %d\n",pciIf);
		return MV_BAD_PARAM;
	}
	if (busNum >= MAX_PCI_BUSSES)
	{
		mvOsPrintf("mvPciLocalBusNumSet: ERR. bus number illigal %d\n", busNum);
		return MV_ERROR;

	}

	localBus = mvPciLocalBusNumGet(pciIf);
	localDev = mvPciLocalDevNumGet(pciIf);


	/* PCI interface mode */
	mvPciModeGet(pciIf, &pciMode);

	/* if PCI type is PCI-X then it is not allowed to change the dev number */
	if (MV_PCIX == pciMode.pciType)
	{
		pciP2PConfig = mvPciConfigRead(pciIf, localBus, localDev, 0, PCIX_STATUS );

		pciP2PConfig &= ~PXS_BN_MASK;

		pciP2PConfig |= (busNum << PXS_BN_OFFS) & PXS_BN_MASK;

		mvPciConfigWrite(pciIf, localBus, localDev, 0, PCIX_STATUS,pciP2PConfig );

	}
	else
	{
		pciP2PConfig  = MV_REG_READ(PCI_P2P_CONFIG_REG(pciIf));

		pciP2PConfig &= ~PPCR_BUS_NUM_MASK;

		pciP2PConfig |= (busNum << PPCR_BUS_NUM_OFFS) & PPCR_BUS_NUM_MASK;

		MV_REG_WRITE(PCI_P2P_CONFIG_REG(pciIf), pciP2PConfig);

	}


	return MV_OK;
}


/*******************************************************************************
* mvPciLocalBusNumGet - Get PCI interface local bus number.
*
* DESCRIPTION:
*       This function gets the local bus number of a given PCI interface.
*
* INPUT:
*       pciIf  - PCI interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local bus number.0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPciLocalBusNumGet(MV_U32 pciIf)
{
	MV_U32 pciP2PConfig;

	/* Parameter checking   */
	if (PCI_DEFAULT_IF != pciIf)
	{
		if (pciIf >= mvCtrlPciMaxIfGet())
		{
			mvOsPrintf("mvPciLocalBusNumGet: ERR. Invalid PCI interface %d\n", 
													   					pciIf);
			return 0xFFFFFFFF;
		}
	}

	pciP2PConfig  = MV_REG_READ(PCI_P2P_CONFIG_REG(pciIf));
	pciP2PConfig &= PPCR_BUS_NUM_MASK;
	return (pciP2PConfig >> PPCR_BUS_NUM_OFFS);
}


/*******************************************************************************
* mvPciLocalDevNumSet - Set PCI interface local device number.
*
* DESCRIPTION:
*       This function sets given PCI interface its local device number.
*       Note: In case the PCI interface is PCI-X, the information is read-only.
*
* INPUT:
*       pciIf  - PCI interface number.
*       devNum - Device number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PCI interface is PCI-X. MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciLocalDevNumSet(MV_U32 pciIf, MV_U32 devNum)
{
	MV_U32 pciP2PConfig;
	MV_PCI_MODE pciMode;
	MV_U32 localBus;
	MV_U32 localDev;

	/* Parameter checking   */
	if (pciIf >= mvCtrlPciMaxIfGet())
	{
		mvOsPrintf("mvPciLocalDevNumSet: ERR. Invalid PCI interface %d\n",pciIf);
		return MV_BAD_PARAM;
	}
	if (devNum >= MAX_PCI_DEVICES)
	{
		mvOsPrintf("mvPciLocalDevNumSet: ERR. device number illigal %d\n", 
																	   devNum);
		return MV_BAD_PARAM;

	}
	
	localBus = mvPciLocalBusNumGet(pciIf);
	localDev = mvPciLocalDevNumGet(pciIf);

	/* PCI interface mode */
	mvPciModeGet(pciIf, &pciMode);

	/* if PCI type is PCIX then it is not allowed to change the dev number */
	if (MV_PCIX == pciMode.pciType)
	{
		pciP2PConfig = mvPciConfigRead(pciIf, localBus, localDev, 0, PCIX_STATUS );

		pciP2PConfig &= ~PXS_DN_MASK;

		pciP2PConfig |= (devNum << PXS_DN_OFFS) & PXS_DN_MASK;

		mvPciConfigWrite(pciIf,localBus, localDev, 0, PCIX_STATUS,pciP2PConfig );
	}
	else
	{
		pciP2PConfig  = MV_REG_READ(PCI_P2P_CONFIG_REG(pciIf));

		pciP2PConfig &= ~PPCR_DEV_NUM_MASK;

		pciP2PConfig |= (devNum << PPCR_DEV_NUM_OFFS) & PPCR_DEV_NUM_MASK;

		MV_REG_WRITE(PCI_P2P_CONFIG_REG(pciIf), pciP2PConfig);
	}

	return MV_OK;
}

/*******************************************************************************
* mvPciLocalDevNumGet - Get PCI interface local device number.
*
* DESCRIPTION:
*       This function gets the local device number of a given PCI interface.
*
* INPUT:
*       pciIf  - PCI interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local device number. 0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPciLocalDevNumGet(MV_U32 pciIf)
{
	MV_U32 pciP2PConfig;

	/* Parameter checking   */
	
	if (PCI_DEFAULT_IF != pciIf)
	{
		if (pciIf >= mvCtrlPciMaxIfGet())
		{
			mvOsPrintf("mvPciLocalDevNumGet: ERR. Invalid PCI interface %d\n", 
																   		pciIf);
			return 0xFFFFFFFF;
		}
	}
	
	pciP2PConfig  = MV_REG_READ(PCI_P2P_CONFIG_REG(pciIf));

	pciP2PConfig &= PPCR_DEV_NUM_MASK;

	return (pciP2PConfig >> PPCR_DEV_NUM_OFFS);
}




