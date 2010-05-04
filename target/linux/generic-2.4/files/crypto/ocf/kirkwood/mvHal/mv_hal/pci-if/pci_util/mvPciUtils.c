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
#include "mvPciUtils.h"

#include "ctrlEnv/mvCtrlEnvLib.h"

/* #define MV_DEBUG */
/* defines  */
#ifdef MV_DEBUG
	#define DB(x)	x
	#define mvOsPrintf printf
#else
	#define DB(x)
#endif

/*
This module only support scanning of Header type 00h of pci devices
There is no suppotr for Header type 01h of pci devices  ( PCI bridges )
*/


static MV_STATUS pciDetectDevice(MV_U32 pciIf,
   								 MV_U32 bus,
   								 MV_U32 dev,
   								 MV_U32 func,
								 MV_PCI_DEVICE *pPciAgent);

static MV_U32 pciDetectDeviceBars(MV_U32 pciIf,
									MV_U32 bus,
									MV_U32 dev,
									MV_U32 func,
									MV_PCI_DEVICE *pPciAgent);






/*******************************************************************************
* mvPciScan - Scan a PCI interface bus
*
* DESCRIPTION:
* Performs a full scan on a PCI interface and returns all possible details
* on the agents found on the bus.
*
* INPUT:
*       pciIf       - PCI Interface
*       pPciAgents 	- Pointer to an Array of the pci agents to be detected
*		pPciAgentsNum - pPciAgents array maximum number of elements
*
* OUTPUT:
*       pPciAgents - Array of the pci agents detected on the bus
*		pPciAgentsNum - Number of pci agents detected on the bus
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/

MV_STATUS mvPciScan(MV_U32 pciIf,
					MV_PCI_DEVICE *pPciAgents,
					MV_U32 *pPciAgentsNum)
{

	MV_U32 devIndex,funcIndex=0,busIndex=0,detectedDevNum=0;
    MV_U32 localBus=mvPciIfLocalBusNumGet(pciIf);
    MV_PCI_DEVICE *pPciDevice;
	MV_PCI_DEVICE *pMainDevice;

	DB(mvOsPrintf("mvPciScan: PCI interface num %d\n", pciIf));
	/* Parameter checking   */
	if (pciIf >= mvCtrlPexMaxIfGet())
	{
		DB(mvOsPrintf("mvPciScan: ERR. Invalid PCI interface num %d\n", pciIf));
		return MV_BAD_PARAM;
	}
	if (NULL == pPciAgents)
	{
		DB(mvOsPrintf("mvPciScan: ERR. pPciAgents=NULL \n"));
		return MV_BAD_PARAM;
	}
	if (NULL == pPciAgentsNum)
	{
		DB(mvOsPrintf("mvPciScan: ERR. pPciAgentsNum=NULL \n"));
		return MV_BAD_PARAM;
	}


	DB(mvOsPrintf("mvPciScan: PCI interface num %d mvPciMasterEnable\n", pciIf));
	/* Master enable the MV PCI master */
	if (MV_OK != mvPciIfMasterEnable(pciIf,MV_TRUE))
	{
		DB(mvOsPrintf("mvPciScan: ERR. mvPciMasterEnable failed  \n"));
		return MV_ERROR;

	}

	DB(mvOsPrintf("mvPciScan: PCI interface num scan%d\n", pciIf));

	/* go through all busses */
	for (busIndex=localBus ; busIndex < MAX_PCI_BUSSES ; busIndex++)
	{
		/* go through all possible devices on the local bus */
		for (devIndex=0 ; devIndex < MAX_PCI_DEVICES ; devIndex++)
		{
			/* always start with function equal to zero */
			funcIndex=0;

			pPciDevice=&pPciAgents[detectedDevNum];
			DB(mvOsPrintf("mvPciScan: PCI interface num scan%d:%d\n", busIndex, devIndex));

			if (MV_ERROR == pciDetectDevice(pciIf,
										   busIndex,
										   devIndex,
										   funcIndex,
										   pPciDevice))
			{
				/* no device detected , try the next address */
				continue;
			}

			/* We are here ! means we have detected a device*/
			/* always we start with only one function per device */
			pMainDevice = pPciDevice;
			pPciDevice->funtionsNum = 1;


			/* move on */
			detectedDevNum++;


			/* check if we have no more room for a new device */
			if (detectedDevNum == *pPciAgentsNum)
			{
				DB(mvOsPrintf("mvPciScan: ERR. array passed too small \n"));
				return MV_ERROR;
			}

			/* check the detected device if it is a multi functional device then
			scan all device functions*/
			if (pPciDevice->isMultiFunction == MV_TRUE)
			{
				/* start with function number 1 because we have already detected
				function 0 */
				for (funcIndex=1; funcIndex<MAX_PCI_FUNCS ; funcIndex++)
				{
					pPciDevice=&pPciAgents[detectedDevNum];

					if (MV_ERROR == pciDetectDevice(pciIf,
												   busIndex,
												   devIndex,
												   funcIndex,
												   pPciDevice))
					{
						/* no device detected means no more functions !*/
						continue;
					}
					/* We are here ! means we have detected a device */

					/* move on */
					pMainDevice->funtionsNum++;
					detectedDevNum++;

					/* check if we have no more room for a new device */
					if (detectedDevNum == *pPciAgentsNum)
					{
						DB(mvOsPrintf("mvPciScan: ERR. Array too small\n"));
						return MV_ERROR;
					}


				}
			}

		}

	}

	/* return the number of devices actually detected on the bus ! */
	*pPciAgentsNum = detectedDevNum;

	return MV_OK;

}


/*******************************************************************************
* pciDetectDevice - Detect a pci device parameters
*
* DESCRIPTION:
*	This function detect if a pci agent exist on certain address !
*   and if exists then it fills all possible information on the
*   agent
*
* INPUT:
*       pciIf       - PCI Interface
*		bus		-	Bus number
*		dev		- 	Device number
*		func	-	Function number
*
*
*
* OUTPUT:
*       pPciAgent - pointer to the pci agent filled with its information
*
* RETURN:
*       MV_ERROR if no device , MV_OK otherwise
*
*******************************************************************************/

static MV_STATUS pciDetectDevice(MV_U32 pciIf,
   								 MV_U32 bus,
   								 MV_U32 dev,
   								 MV_U32 func,
								 MV_PCI_DEVICE *pPciAgent)
{
	MV_U32 pciData;

	/* no Parameters checking ! because it is static function and it is assumed
	that all parameters were checked in the calling function */


	/* Try read the PCI Vendor ID and Device ID */

	/*  We will scan only ourselves and the PCI slots that exist on the
		board, because we may have a case that we have one slot that has
		a Cardbus connector, and because CardBus answers all IDsels we want
		to scan only this slot and ourseleves.

	*/
	#if defined(MV_INCLUDE_PCI)
	if ((PCI_IF_TYPE_CONVEN_PCIX == mvPciIfTypeGet(pciIf)) &&
					(DB_88F5181_DDR1_PRPMC != mvBoardIdGet()) &&
					(DB_88F5181_DDR1_PEXPCI != mvBoardIdGet()) &&
					(DB_88F5181_DDR1_MNG != mvBoardIdGet()))
    	{

			if (mvBoardIsOurPciSlot(bus, dev) == MV_FALSE)
			{
				return MV_ERROR;
			}
	}
	#endif /* defined(MV_INCLUDE_PCI) */

	pciData = mvPciIfConfigRead(pciIf, bus,dev,func, PCI_DEVICE_AND_VENDOR_ID);

	if (PCI_ERROR_CODE == pciData)
	{
		/* no device exist */
		return MV_ERROR;
	}

	/* we are here ! means a device is detected */

	/* fill basic information */
	pPciAgent->busNumber=bus;
	pPciAgent->deviceNum=dev;
	pPciAgent->function=func;

	/* Fill the PCI Vendor ID and Device ID */

	pPciAgent->venID = (pciData & PDVIR_VEN_ID_MASK) >> PDVIR_VEN_ID_OFFS;
	pPciAgent->deviceID = (pciData & PDVIR_DEV_ID_MASK) >> PDVIR_DEV_ID_OFFS;

	/* Read Status and command */
	pciData = mvPciIfConfigRead(pciIf,
							  bus,dev,func,
							  PCI_STATUS_AND_COMMAND);


	/* Fill related Status and Command information*/

	if (pciData & PSCR_TAR_FAST_BB)
	{
		pPciAgent->isFastB2BCapable = MV_TRUE;
	}
	else
	{
		pPciAgent->isFastB2BCapable = MV_FALSE;
	}

	if (pciData & PSCR_CAP_LIST)
	{
		pPciAgent->isCapListSupport=MV_TRUE;
	}
	else
	{
		pPciAgent->isCapListSupport=MV_FALSE;
	}

	if (pciData & PSCR_66MHZ_EN)
	{
		pPciAgent->is66MHZCapable=MV_TRUE;
	}
	else
	{
		pPciAgent->is66MHZCapable=MV_FALSE;
	}

	/* Read Class Code and Revision */
	pciData = mvPciIfConfigRead(pciIf,
							  bus,dev,func,
							  PCI_CLASS_CODE_AND_REVISION_ID);


	pPciAgent->baseClassCode =
		(pciData & PCCRIR_BASE_CLASS_MASK) >> PCCRIR_BASE_CLASS_OFFS;

	pPciAgent->subClassCode =
		(pciData & PCCRIR_SUB_CLASS_MASK) >> PCCRIR_SUB_CLASS_OFFS;

	pPciAgent->progIf =
		(pciData & PCCRIR_PROGIF_MASK) >> PCCRIR_PROGIF_OFFS;

	pPciAgent->revisionID =
		(pciData & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS;

	/* Read  PCI_BIST_HDR_TYPE_LAT_TMR_CACHE_LINE */
	pciData = mvPciIfConfigRead(pciIf,
							  bus,dev,func,
							  PCI_BIST_HDR_TYPE_LAT_TMR_CACHE_LINE);



	pPciAgent->pciCacheLine=
		(pciData & PBHTLTCLR_CACHELINE_MASK ) >> PBHTLTCLR_CACHELINE_OFFS;
	pPciAgent->pciLatencyTimer=
		(pciData & PBHTLTCLR_LATTIMER_MASK) >> PBHTLTCLR_LATTIMER_OFFS;

	switch (pciData & PBHTLTCLR_HEADER_MASK)
	{
	case PBHTLTCLR_HEADER_STANDARD:

		pPciAgent->pciHeader=MV_PCI_STANDARD;
		break;
	case PBHTLTCLR_HEADER_PCI2PCI_BRIDGE:

		pPciAgent->pciHeader=MV_PCI_PCI2PCI_BRIDGE;
		break;

	}

	if (pciData & PBHTLTCLR_MULTI_FUNC)
	{
		pPciAgent->isMultiFunction=MV_TRUE;
	}
	else
	{
		pPciAgent->isMultiFunction=MV_FALSE;
	}

	if (pciData & PBHTLTCLR_BISTCAP)
	{
		pPciAgent->isBISTCapable=MV_TRUE;
	}
	else
	{
		pPciAgent->isBISTCapable=MV_FALSE;
	}


	/* read this device pci bars */

	pciDetectDeviceBars(pciIf,
					  bus,dev,func,
	 				 pPciAgent);


	/* check if we are bridge*/
	if ((pPciAgent->baseClassCode == PCI_BRIDGE_CLASS)&&
		(pPciAgent->subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
	{

		/* Read  P2P_BUSSES_NUM */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  P2P_BUSSES_NUM);

		pPciAgent->p2pPrimBusNum =
			(pciData & PBM_PRIME_BUS_NUM_MASK) >> PBM_PRIME_BUS_NUM_OFFS;

		pPciAgent->p2pSecBusNum =
			(pciData & PBM_SEC_BUS_NUM_MASK) >> PBM_SEC_BUS_NUM_OFFS;

		pPciAgent->p2pSubBusNum =
			(pciData & PBM_SUB_BUS_NUM_MASK) >> PBM_SUB_BUS_NUM_OFFS;

		pPciAgent->p2pSecLatencyTimer =
			(pciData & PBM_SEC_LAT_TMR_MASK) >> PBM_SEC_LAT_TMR_OFFS;

		/* Read  P2P_IO_BASE_LIMIT_SEC_STATUS */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  P2P_IO_BASE_LIMIT_SEC_STATUS);

		pPciAgent->p2pSecStatus =
			(pciData & PIBLSS_SEC_STATUS_MASK) >> PIBLSS_SEC_STATUS_OFFS;


		pPciAgent->p2pIObase =
			(pciData & PIBLSS_IO_BASE_MASK) << PIBLSS_IO_LIMIT_OFFS;

		/* clear low address (should be zero)*/
		pPciAgent->p2pIObase &= PIBLSS_HIGH_ADDR_MASK;

		pPciAgent->p2pIOLimit =
			(pciData & PIBLSS_IO_LIMIT_MASK);

		/* fill low address with 0xfff */
		pPciAgent->p2pIOLimit |= PIBLSS_LOW_ADDR_MASK;


		switch ((pciData & PIBLSS_ADD_CAP_MASK) >> PIBLSS_ADD_CAP_OFFS)
		{
		case PIBLSS_ADD_CAP_16BIT:

			pPciAgent->bIO32 = MV_FALSE;

			break;
		case PIBLSS_ADD_CAP_32BIT:

			pPciAgent->bIO32 = MV_TRUE;

			/* Read  P2P_IO_BASE_LIMIT_UPPER_16 */
			pciData = mvPciIfConfigRead(pciIf,
									  bus,dev,func,
									  P2P_IO_BASE_LIMIT_UPPER_16);

			pPciAgent->p2pIObase |=
				(pciData & PRBU_IO_UPP_BASE_MASK) << PRBU_IO_UPP_LIMIT_OFFS;


			pPciAgent->p2pIOLimit |=
				(pciData & PRBU_IO_UPP_LIMIT_MASK);

			break;

		}


		/* Read  P2P_MEM_BASE_LIMIT */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  P2P_MEM_BASE_LIMIT);

		pPciAgent->p2pMemBase =
			(pciData & PMBL_MEM_BASE_MASK) << PMBL_MEM_LIMIT_OFFS;

		/* clear low address */
		pPciAgent->p2pMemBase &= PMBL_HIGH_ADDR_MASK;

		pPciAgent->p2pMemLimit =
			(pciData & PMBL_MEM_LIMIT_MASK);

		/* add 0xfffff */
		pPciAgent->p2pMemLimit |= PMBL_LOW_ADDR_MASK;


		/* Read  P2P_PREF_MEM_BASE_LIMIT */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  P2P_PREF_MEM_BASE_LIMIT);


		pPciAgent->p2pPrefMemBase =
			(pciData & PRMBL_PREF_MEM_BASE_MASK) << PRMBL_PREF_MEM_LIMIT_OFFS;

		/* get high address only */
		pPciAgent->p2pPrefMemBase &= PRMBL_HIGH_ADDR_MASK;



		pPciAgent->p2pPrefMemLimit =
			(pciData & PRMBL_PREF_MEM_LIMIT_MASK);

		/* add 0xfffff */
		pPciAgent->p2pPrefMemLimit |= PRMBL_LOW_ADDR_MASK;

		switch (pciData & PRMBL_ADD_CAP_MASK)
		{
		case PRMBL_ADD_CAP_32BIT:

			pPciAgent->bPrefMem64 = MV_FALSE;

			/* Read  P2P_PREF_BASE_UPPER_32 */
			pPciAgent->p2pPrefBaseUpper32Bits = 0;

			/* Read  P2P_PREF_LIMIT_UPPER_32 */
			pPciAgent->p2pPrefLimitUpper32Bits = 0;

			break;
		case PRMBL_ADD_CAP_64BIT:

			pPciAgent->bPrefMem64 = MV_TRUE;

			/* Read  P2P_PREF_BASE_UPPER_32 */
			pPciAgent->p2pPrefBaseUpper32Bits = mvPciIfConfigRead(pciIf,
									  bus,dev,func,
									  P2P_PREF_BASE_UPPER_32);

			/* Read  P2P_PREF_LIMIT_UPPER_32 */
			pPciAgent->p2pPrefLimitUpper32Bits = mvPciIfConfigRead(pciIf,
									  bus,dev,func,
									  P2P_PREF_LIMIT_UPPER_32);

			break;

		}

	}
	else /* no bridge */
	{
		/* Read  PCI_SUBSYS_ID_AND_SUBSYS_VENDOR_ID */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  PCI_SUBSYS_ID_AND_SUBSYS_VENDOR_ID);


		pPciAgent->subSysVenID =
			(pciData & PSISVIR_VENID_MASK) >> PSISVIR_VENID_OFFS;
		pPciAgent->subSysID =
			(pciData & PSISVIR_DEVID_MASK) >> PSISVIR_DEVID_OFFS;


		/* Read  PCI_EXPANSION_ROM_BASE_ADDR_REG */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  PCI_EXPANSION_ROM_BASE_ADDR_REG);


		if (pciData & PERBAR_EXPROMEN)
		{
			pPciAgent->isExpRom = MV_TRUE;
		}
		else
		{
			pPciAgent->isExpRom = MV_FALSE;
		}

		pPciAgent->expRomAddr =
			(pciData & PERBAR_BASE_MASK) >> PERBAR_BASE_OFFS;

	}


	if (MV_TRUE == pPciAgent->isCapListSupport)
	{
		/* Read  PCI_CAPABILTY_LIST_POINTER */
		pciData = mvPciIfConfigRead(pciIf,
								  bus,dev,func,
								  PCI_CAPABILTY_LIST_POINTER);

		pPciAgent->capListPointer =
			(pciData & PCLPR_CAPPTR_MASK) >> PCLPR_CAPPTR_OFFS;

	}

	/* Read  PCI_INTERRUPT_PIN_AND_LINE */
	pciData = mvPciIfConfigRead(pciIf,
							  bus,dev,func,
							  PCI_INTERRUPT_PIN_AND_LINE);


	pPciAgent->irqLine=
		(pciData & PIPLR_INTLINE_MASK) >> PIPLR_INTLINE_OFFS;

	pPciAgent->intPin=
		(MV_PCI_INT_PIN)(pciData & PIPLR_INTPIN_MASK) >> PIPLR_INTPIN_OFFS;

	pPciAgent->minGrant=
		(pciData & PIPLR_MINGRANT_MASK) >> PIPLR_MINGRANT_OFFS;
	pPciAgent->maxLatency=
		(pciData & PIPLR_MAXLATEN_MASK) >> PIPLR_MAXLATEN_OFFS;

	mvPciClassNameGet(pPciAgent->baseClassCode,
					  (MV_8 *)pPciAgent->type);

	return MV_OK;


}

/*******************************************************************************
* pciDetectDeviceBars - Detect a pci device bars
*
* DESCRIPTION:
*	This function detects all pci agent bars
*
* INPUT:
*       pciIf       - PCI Interface
*		bus		-	Bus number
*		dev		- 	Device number
*		func	-	Function number
*
*
*
* OUTPUT:
*       pPciAgent - pointer to the pci agent filled with its information
*
* RETURN:
*       detected bars number
*
*******************************************************************************/
static MV_U32 pciDetectDeviceBars(MV_U32 pciIf,
									MV_U32 bus,
									MV_U32 dev,
									MV_U32 func,
									MV_PCI_DEVICE *pPciAgent)
{
	MV_U32 pciData,barIndex,detectedBar=0;
	MV_U32 tmpBaseHigh=0,tmpBaseLow=0;
	MV_U32 pciMaxBars=0;

	pPciAgent->barsNum=0;

	/* check if we are bridge*/
	if ((pPciAgent->baseClassCode == PCI_BRIDGE_CLASS)&&
		(pPciAgent->subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
	{
		pciMaxBars = 2;
	}
	else /* no bridge */
	{
		pciMaxBars = 6;
	}

	/* read this device pci bars */
	for (barIndex = 0 ; barIndex < pciMaxBars ; barIndex++ )
	{
		/* Read  PCI_MEMORY_BAR_BASE_ADDR */
		tmpBaseLow = pciData = mvPciIfConfigRead(pciIf,
									   bus,dev,func,
						               PCI_MEMORY_BAR_BASE_ADDR(barIndex));

		pPciAgent->pciBar[detectedBar].barOffset =
			PCI_MEMORY_BAR_BASE_ADDR(barIndex);

		/* check if the bar is 32bit or 64bit bar */
		switch (pciData & PBBLR_TYPE_MASK)
		{
		case PBBLR_TYPE_32BIT_ADDR:
			pPciAgent->pciBar[detectedBar].barType = PCI_32BIT_BAR;
			break;
		case PBBLR_TYPE_64BIT_ADDR:
			pPciAgent->pciBar[detectedBar].barType = PCI_64BIT_BAR;
			break;

		}

		/* check if it is memory or IO bar */
		if (pciData & PBBLR_IOSPACE)
		{
			pPciAgent->pciBar[detectedBar].barMapping=PCI_IO_BAR;
		}
		else
		{
			pPciAgent->pciBar[detectedBar].barMapping=PCI_MEMORY_BAR;
		}

		/* if it is memory bar then check if it is prefetchable */
		if (PCI_MEMORY_BAR == pPciAgent->pciBar[detectedBar].barMapping)
		{
			if (pciData & PBBLR_PREFETCH_EN)
			{
				pPciAgent->pciBar[detectedBar].isPrefetchable = MV_TRUE;
			}
			else
			{
				pPciAgent->pciBar[detectedBar].isPrefetchable = MV_FALSE;
			}

            pPciAgent->pciBar[detectedBar].barBaseLow =
				pciData & PBBLR_MEM_BASE_MASK;


		}
		else /* IO Bar */
		{
			pPciAgent->pciBar[detectedBar].barBaseLow =
				pciData & PBBLR_IO_BASE_MASK;

		}

		pPciAgent->pciBar[detectedBar].barBaseHigh=0;

		if (PCI_64BIT_BAR == pPciAgent->pciBar[detectedBar].barType)
		{
			barIndex++;

			tmpBaseHigh = pPciAgent->pciBar[detectedBar].barBaseHigh =
				mvPciIfConfigRead(pciIf,
								bus,dev,func,
								PCI_MEMORY_BAR_BASE_ADDR(barIndex));


		}

		/* calculating full base address (64bit) */
		pPciAgent->pciBar[detectedBar].barBaseAddr =
			(MV_U64)pPciAgent->pciBar[detectedBar].barBaseHigh;

		pPciAgent->pciBar[detectedBar].barBaseAddr <<= 32;

		pPciAgent->pciBar[detectedBar].barBaseAddr |=
			(MV_U64)pPciAgent->pciBar[detectedBar].barBaseLow;



		/* get the sizes of the the bar */

		pPciAgent->pciBar[detectedBar].barSizeHigh=0;

		if ((PCI_64BIT_BAR == pPciAgent->pciBar[detectedBar].barType) &&
			(PCI_MEMORY_BAR == pPciAgent->pciBar[detectedBar].barMapping))

		{
			/* write oxffffffff to the bar to get the size */
			/* start with sizelow ( original value was saved in tmpBaseLow ) */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex-1),
							0xffffffff);

			/* read size */
			pPciAgent->pciBar[detectedBar].barSizeLow =
				mvPciIfConfigRead(pciIf,
								bus,dev,func,
								PCI_MEMORY_BAR_BASE_ADDR(barIndex-1));



			/* restore original value */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex-1),
							tmpBaseLow);


			/* now do the same for BaseHigh */

			/* write oxffffffff to the bar to get the size */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex),
							0xffffffff);

			/* read size */
			pPciAgent->pciBar[detectedBar].barSizeHigh =
				mvPciIfConfigRead(pciIf,
								bus,dev,func,
								PCI_MEMORY_BAR_BASE_ADDR(barIndex));

			/* restore original value */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex),
							tmpBaseHigh);

			if ((0 == pPciAgent->pciBar[detectedBar].barSizeLow)&&
				(0 == pPciAgent->pciBar[detectedBar].barSizeHigh))
			{
				/* this bar is not applicable for this device,
				   ignore all previous settings and check the next bar*/

				/* we though this was a 64bit bar , and it seems this
				   was wrong ! so decrement barIndex */
				barIndex--;
				continue;
			}

			/* calculate the full 64 bit size  */

			if (0 != pPciAgent->pciBar[detectedBar].barSizeHigh)
			{
				pPciAgent->pciBar[detectedBar].barSizeLow &= PBBLR_MEM_BASE_MASK;

				pPciAgent->pciBar[detectedBar].barSizeLow =
					~pPciAgent->pciBar[detectedBar].barSizeLow + 1;

				pPciAgent->pciBar[detectedBar].barSizeHigh = 0;

			}
			else
			{

				pPciAgent->pciBar[detectedBar].barSizeLow &= PBBLR_MEM_BASE_MASK;

				pPciAgent->pciBar[detectedBar].barSizeLow =
					~pPciAgent->pciBar[detectedBar].barSizeLow + 1;

				pPciAgent->pciBar[detectedBar].barSizeHigh = 0;

			}



		}
		else /* 32bit bar */
		{
			/* write oxffffffff to the bar to get the size */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex),
							0xffffffff);

			/* read size */
			pPciAgent->pciBar[detectedBar].barSizeLow =
				mvPciIfConfigRead(pciIf,
								bus,dev,func,
								PCI_MEMORY_BAR_BASE_ADDR(barIndex));

			if (0 == pPciAgent->pciBar[detectedBar].barSizeLow)
			{
				/* this bar is not applicable for this device,
				   ignore all previous settings and check the next bar*/
				continue;
			}


			/* restore original value */
			mvPciIfConfigWrite(pciIf,
							bus,dev,func,
							PCI_MEMORY_BAR_BASE_ADDR(barIndex),
							tmpBaseLow);

		/* calculate size low */

			if (PCI_MEMORY_BAR == pPciAgent->pciBar[detectedBar].barMapping)
			{
				pPciAgent->pciBar[detectedBar].barSizeLow &= PBBLR_MEM_BASE_MASK;
			}
			else
			{
				pPciAgent->pciBar[detectedBar].barSizeLow &= PBBLR_IO_BASE_MASK;
			}

			pPciAgent->pciBar[detectedBar].barSizeLow =
				~pPciAgent->pciBar[detectedBar].barSizeLow + 1;

			pPciAgent->pciBar[detectedBar].barSizeHigh = 0;
			pPciAgent->pciBar[detectedBar].barSize =
				(MV_U64)pPciAgent->pciBar[detectedBar].barSizeLow;


		}

		/* we are here ! this means we have already detected a bar for
		this device , now move on */

		detectedBar++;
		pPciAgent->barsNum++;
	}

	return detectedBar;
}


/*******************************************************************************
* mvPciClassNameGet - get PCI  class name
*
* DESCRIPTION:
*		This function returns the PCI class name
*
* INPUT:
*       baseClassCode       - Base Class Code.
*
* OUTPUT:
*       pType - the class name
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciClassNameGet(MV_U32 baseClassCode, MV_8 *pType)
{

    switch(baseClassCode)
    {
        case 0x0:
            strcpy(pType,"Old generation device");
            break;
        case 0x1:
            strcpy(pType,"Mass storage controller");
            break;
        case 0x2:
            strcpy(pType,"Network controller");
            break;
        case 0x3:
            strcpy(pType,"Display controller");
            break;
        case 0x4:
            strcpy(pType,"Multimedia device");
            break;
        case 0x5:
            strcpy(pType,"Memory controller");
            break;
        case 0x6:
            strcpy(pType,"Bridge Device");
            break;
        case 0x7:
            strcpy(pType,"Simple Communication controllers");
            break;
        case 0x8:
            strcpy(pType,"Base system peripherals");
            break;
        case 0x9:
            strcpy(pType,"Input Devices");
            break;
        case 0xa:
            strcpy(pType,"Docking stations");
            break;
        case 0xb:
            strcpy(pType,"Processors");
            break;
        case 0xc:
            strcpy(pType,"Serial bus controllers");
            break;
        case 0xd:
            strcpy(pType,"Wireless controllers");
            break;
        case 0xe:
            strcpy(pType,"Intelligent I/O controllers");
            break;
        case 0xf:
            strcpy(pType,"Satellite communication controllers");
            break;
        case 0x10:
            strcpy(pType,"Encryption/Decryption controllers");
            break;
        case 0x11:
            strcpy(pType,"Data acquisition and signal processing controllers");
            break;
        default:
            strcpy(pType,"Unknown device");
            break;
    }

	return MV_OK;

}



