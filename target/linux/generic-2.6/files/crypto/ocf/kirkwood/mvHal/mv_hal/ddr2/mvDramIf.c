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

#include "ddr2/mvDramIfStaticInit.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* DRAM bank presence encoding */
#define BANK_PRESENT_CS0			    0x1
#define BANK_PRESENT_CS0_CS1			0x3
#define BANK_PRESENT_CS0_CS2			0x5
#define BANK_PRESENT_CS0_CS1_CS2		0x7
#define BANK_PRESENT_CS0_CS2_CS3		0xd
#define BANK_PRESENT_CS0_CS2_CS3_CS4	0xf

/* locals   */
#ifndef MV_STATIC_DRAM_ON_BOARD
static void sdramDDr2OdtConfig(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 dunitCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32  busClk, MV_STATUS TTmode );
static MV_U32 dunitCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32  busClk);
static MV_U32 sdramModeRegCalc(MV_U32 minCas);
static MV_U32 sdramExtModeRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk);
static MV_U32 sdramAddrCtrlRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_DRAM_BANK_INFO *pBankInfoDIMM1);
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_DRAM_BANK_INFO *pBankInfo2, MV_U32 busClk);
static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo,MV_DRAM_BANK_INFO *pBankInfo2, MV_U32 busClk, MV_U32 forcedCl);
static MV_U32 sdramTimeCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimeCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk);
static MV_U32 sdramDdr2TimeLoRegCalc(MV_U32 minCas);
static MV_U32 sdramDdr2TimeHiRegCalc(MV_U32 minCas);
#endif
MV_32 DRAM_CS_Order[MV_DRAM_MAX_CS] = {N_A

#ifdef MV_INCLUDE_SDRAM_CS1
		,N_A
#endif
#ifdef MV_INCLUDE_SDRAM_CS2
		,N_A
#endif
#ifdef MV_INCLUDE_SDRAM_CS3
    ,N_A
#endif
	};
/* Get DRAM size of CS num */
MV_U32 mvDramCsSizeGet(MV_U32 csNum)
{
	MV_DRAM_BANK_INFO bankInfo;
	MV_U32  size, deviceW, dimmW;
#ifdef MV78XX0	
	MV_U32  temp;
#endif

	if(MV_OK == mvDramBankInfoGet(csNum, &bankInfo))
	{
        	if (0 == bankInfo.size)
			return 0;

		/* Note that the Dimm width might be different then the device DRAM width */
#ifdef MV78XX0	
		temp = MV_REG_READ(SDRAM_CONFIG_REG);
		deviceW = ((temp & SDRAM_DWIDTH_MASK) == SDRAM_DWIDTH_32BIT )? 32 : 64;
#else
		deviceW = 16 /* KW family */;
#endif
		dimmW = bankInfo.dataWidth - (bankInfo.dataWidth % 16);
		size = ((bankInfo.size << 20) / (dimmW/deviceW)); 
		return size;
	}
	else
		return 0;
}
/*******************************************************************************
* mvDramIfDetect - Prepare DRAM interface configuration values.
*
* DESCRIPTION:
*       This function implements the full DRAM detection and timing 
*       configuration for best system performance.
*       Since this routine runs from a ROM device (Boot Flash), its stack 
*       resides on RAM, that might be the system DRAM. Changing DRAM 
*       configuration values while keeping vital data in DRAM is risky. That
*       is why the function does not preform the configuration setting but 
*       prepare those in predefined 32bit registers (in this case IDMA 
*       registers are used) for other routine to perform the settings.
*       The function will call for board DRAM SPD information for each DRAM 
*       chip select. The function will then analyze those SPD parameters of 
*       all DRAM banks in order to decide on DRAM configuration compatible 
*       for all DRAM banks.
*       The function will set the CPU DRAM address decode registers.
*       Note: This routine prepares values that will overide configuration of
*       mvDramBasicAsmInit().
*       
* INPUT:
*       forcedCl - Forced CAL Latency. If equal to zero, do not force.
*       eccDisable - Force down the ECC.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvDramIfDetect(MV_U32 forcedCl, MV_BOOL eccDisable)
{
	MV_32 	MV_DRAM_CS_order[MV_DRAM_MAX_CS] = {
		SDRAM_CS0
#ifdef MV_INCLUDE_SDRAM_CS1
		,SDRAM_CS1
#endif
#ifdef MV_INCLUDE_SDRAM_CS2
		,SDRAM_CS2
#endif
#ifdef MV_INCLUDE_SDRAM_CS3
		,SDRAM_CS3
#endif
		};
	MV_U32  busClk, deviceW, dimmW;
	MV_U32 numOfAllDevices = 0;
	MV_STATUS TTMode; 
#ifndef MV_STATIC_DRAM_ON_BOARD
	MV_DRAM_BANK_INFO bankInfo[MV_DRAM_MAX_CS];
	MV_U32  size, base = 0, i, j, temp, busClkPs;
	MV_U8	minCas;
	MV_CPU_DEC_WIN dramDecWin;
	dramDecWin.addrWin.baseHigh = 0;
#endif

	busClk = mvBoardSysClkGet();

	if (0 == busClk)
	{
		mvOsPrintf("Dram: ERR. Can't detect system clock! \n");
		return MV_ERROR;
	}
	
#ifndef MV_STATIC_DRAM_ON_BOARD

	busClkPs = 1000000000 / (busClk / 1000);  /* in ps units */
	/* we will use bank 0 as the representative of the all the DRAM banks,  */
	/* since bank 0 must exist.                                             */	
	for(i = 0; i < MV_DRAM_MAX_CS; i++)
	{ 
		/* if Bank exist */
		if(MV_OK == mvDramBankInfoGet(i, &bankInfo[i]))
		{
			DB(mvOsPrintf("Dram: Find bank %d\n", i));
			/* check it isn't SDRAM */
			if(bankInfo[i].memoryType != MEM_TYPE_DDR2)
			{
				mvOsOutput("Dram: ERR. SDRAM type not supported !!!\n");
				return MV_ERROR;
			}

            		/* All banks must support the Mclk freqency */
			if(bankInfo[i].minCycleTimeAtMaxCasLatPs > busClkPs)
			{
				mvOsOutput("Dram: ERR. Bank %d doesn't support memory clock!!!\n", i);
				return MV_ERROR;
			}

			/* All banks must support registry in order to activate it */
			if(bankInfo[i].registeredAddrAndControlInputs != 
			   bankInfo[0].registeredAddrAndControlInputs)
			{
				mvOsOutput("Dram: ERR. different Registered settings !!!\n");
				return MV_ERROR;
			}

			/* All banks must support same ECC mode */
			if(bankInfo[i].errorCheckType != 
			   bankInfo[0].errorCheckType)
			{
				mvOsOutput("Dram: ERR. different ECC settings !!!\n");
				return MV_ERROR;
			}

		}
		else
		{
			if( i == 0 ) /* bank 0 doesn't exist */
			{
				mvOsOutput("Dram: ERR. Fail to detect bank 0 !!!\n");
				return MV_ERROR;
			}
			else
			{
				DB(mvOsPrintf("Dram: Could not find bank %d\n", i));
				bankInfo[i].size = 0;     /* Mark this bank as non exist */
			}
		}
	}

#ifdef MV_INCLUDE_SDRAM_CS2
	if (bankInfo[SDRAM_CS0].size <  bankInfo[SDRAM_CS2].size)
	{
		MV_DRAM_CS_order[0] = SDRAM_CS2;
		MV_DRAM_CS_order[1] = SDRAM_CS3;
		MV_DRAM_CS_order[2] = SDRAM_CS0;
		MV_DRAM_CS_order[3] = SDRAM_CS1;
		DRAM_CS_Order[0] = SDRAM_CS2;
		DRAM_CS_Order[1] = SDRAM_CS3;
		DRAM_CS_Order[2] = SDRAM_CS0;
		DRAM_CS_Order[3] = SDRAM_CS1;

	}
	else
#endif
	{
		MV_DRAM_CS_order[0] = SDRAM_CS0;
		MV_DRAM_CS_order[1] = SDRAM_CS1;
		DRAM_CS_Order[0] = SDRAM_CS0;
		DRAM_CS_Order[1] = SDRAM_CS1;
#ifdef MV_INCLUDE_SDRAM_CS2
		MV_DRAM_CS_order[2] = SDRAM_CS2;
		MV_DRAM_CS_order[3] = SDRAM_CS3;
		DRAM_CS_Order[2] = SDRAM_CS2;
		DRAM_CS_Order[3] = SDRAM_CS3;
#endif
	}

	for(j = 0; j < MV_DRAM_MAX_CS; j++)
	{
		i = MV_DRAM_CS_order[j];
		
        	if (0 == bankInfo[i].size)
			continue;

			/* Init the CPU window decode */
			/* Note that the Dimm width might be different then the device DRAM width */
#ifdef MV78XX0	
			temp = MV_REG_READ(SDRAM_CONFIG_REG);
			deviceW = ((temp & SDRAM_DWIDTH_MASK) == SDRAM_DWIDTH_32BIT )? 32 : 64;
#else
			deviceW = 16 /* KW family */;
#endif
			dimmW = bankInfo[0].dataWidth - (bankInfo[0].dataWidth % 16);
			size = ((bankInfo[i].size << 20) / (dimmW/deviceW)); 
		
			/* We can not change DRAM window settings while excecuting  	*/
			/* code from it. That is why we skip the DRAM CS[0], saving     */
			/* it to the ROM configuration routine				*/

			numOfAllDevices += bankInfo[i].numberOfDevices;
			if (i == MV_DRAM_CS_order[0])
			{
				MV_U32 sizeToReg;
				/* Translate the given window size to register format		*/
				sizeToReg = ctrlSizeToReg(size, SCSR_SIZE_ALIGNMENT);
				/* Size parameter validity check.                           */
				if (-1 == sizeToReg)
				{
					mvOsOutput("DRAM: mvCtrlAddrDecToReg: ERR. Win %d size invalid.\n"
							   ,i);
					return MV_BAD_PARAM;
				}

				DB(mvOsPrintf("Dram: Bank 0 Size - %x\n",sizeToReg);)
				sizeToReg = (sizeToReg << SCSR_SIZE_OFFS);
				sizeToReg |= SCSR_WIN_EN;
				MV_REG_WRITE(DRAM_BUF_REG0, sizeToReg);
			}
			else
			{
				dramDecWin.addrWin.baseLow = base;
				dramDecWin.addrWin.size = size;
				dramDecWin.enable = MV_TRUE;
				DB(mvOsPrintf("Dram: Enable window %d base 0x%x, size=0x%x\n",i, base, size));
				
				/* Check if the DRAM size is more then 3GByte */
				if (base < 0xC0000000)
				{
					DB(mvOsPrintf("Dram: Enable window %d base 0x%x, size=0x%x\n",i, base, size));
           			if (MV_OK != mvCpuIfTargetWinSet(i, &dramDecWin))
					{
						mvOsPrintf("Dram: ERR. Fail to set bank %d!!!\n", SDRAM_CS0 + i);
						return 	MV_ERROR;
					}
				}
			}
			
			base += size;

			/* update the suportedCasLatencies mask */
			bankInfo[0].suportedCasLatencies &= bankInfo[i].suportedCasLatencies;
	}

	/* calculate minimum CAS */
	minCas = minCasCalc(&bankInfo[0], &bankInfo[2], busClk, forcedCl);
	if (0 == minCas) 
	{
		mvOsOutput("Dram: Warn: Could not find CAS compatible to SysClk %dMhz\n",
				   (busClk / 1000000));

		minCas = DDR2_CL_4; /* Continue with this CAS */
		mvOsOutput("Set default CAS latency 4\n");
	}

	/* calc SDRAM_CONFIG_REG  and save it to temp register */
	temp = sdramConfigRegCalc(&bankInfo[0],&bankInfo[2], busClk);
	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramConfigRegCalc failed !!!\n");
		return MV_ERROR;
	}

	/* check if ECC is enabled by the user */
	if(eccDisable)	
	{	
		/* turn off ECC*/
		temp &= ~BIT18;
	}
	DB(mvOsPrintf("Dram: sdramConfigRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG1, temp);
	
	/* calc SDRAM_MODE_REG  and save it to temp register */ 
	temp = sdramModeRegCalc(minCas);
    	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramModeRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramModeRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG2, temp);

	/* calc SDRAM_EXTENDED_MODE_REG  and save it to temp register */ 
	temp = sdramExtModeRegCalc(&bankInfo[0], busClk);
	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramExtModeRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramExtModeRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG10, temp);

	/* calc D_UNIT_CONTROL_LOW  and save it to temp register */
	TTMode = MV_FALSE;
	DB(mvOsPrintf("Dram: numOfAllDevices = %x\n",numOfAllDevices);)
	if( (numOfAllDevices > 9) && (bankInfo[0].registeredAddrAndControlInputs == MV_FALSE) )
	{
		if ( ( (numOfAllDevices > 9) && (busClk > MV_BOARD_SYSCLK_200MHZ) ) ||
			(numOfAllDevices > 18) )
		{
			mvOsOutput("Enable 2T ");
			TTMode = MV_TRUE;
		}
	}

  	temp = dunitCtrlLowRegCalc(&bankInfo[0], minCas, busClk, TTMode ); 
   	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. dunitCtrlLowRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: dunitCtrlLowRegCalc - %x\n",temp);)
  	MV_REG_WRITE(DRAM_BUF_REG3, temp); 

	/* calc D_UNIT_CONTROL_HIGH  and save it to temp register */
  	temp = dunitCtrlHighRegCalc(&bankInfo[0], busClk); 
   	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. dunitCtrlHighRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: dunitCtrlHighRegCalc - %x\n",temp);)
	/* check if ECC is enabled by the user */
	if(eccDisable)	
	{	
		/* turn off sample stage if no ecc */
		temp &= ~SDRAM__D2P_EN;;
	}
  	MV_REG_WRITE(DRAM_BUF_REG13, temp); 

	/* calc SDRAM_ADDR_CTRL_REG  and save it to temp register */
	temp = sdramAddrCtrlRegCalc(&bankInfo[0],&bankInfo[2]);
    	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramAddrCtrlRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramAddrCtrlRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG4, temp);

	/* calc SDRAM_TIMING_CTRL_LOW_REG  and save it to temp register */
	temp = sdramTimeCtrlLowRegCalc(&bankInfo[0], minCas, busClk);
    	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramTimeCtrlLowRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramTimeCtrlLowRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG5, temp);

	/* calc SDRAM_TIMING_CTRL_HIGH_REG  and save it to temp register */
	temp = sdramTimeCtrlHighRegCalc(&bankInfo[0], busClk);
    	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramTimeCtrlHighRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramTimeCtrlHighRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG6, temp);

	sdramDDr2OdtConfig(bankInfo);

	/* calc DDR2_SDRAM_TIMING_LOW_REG  and save it to temp register */
	temp = sdramDdr2TimeLoRegCalc(minCas);
	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramDdr2TimeLoRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramDdr2TimeLoRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG11, temp);

	/* calc DDR2_SDRAM_TIMING_HIGH_REG  and save it to temp register */
	temp = sdramDdr2TimeHiRegCalc(minCas);
	if(-1 == temp)
	{
		mvOsOutput("Dram: ERR. sdramDdr2TimeHiRegCalc failed !!!\n");
		return MV_ERROR;
	}
	DB(mvOsPrintf("Dram: sdramDdr2TimeHiRegCalc - %x\n",temp);)
	MV_REG_WRITE(DRAM_BUF_REG12, temp);
#endif
	
	/* Note that DDR SDRAM Address/Control and Data pad calibration     */
	/* settings is done in mvSdramIfConfig.s                            */

 	return MV_OK;
}


/*******************************************************************************
* mvDramIfBankBaseGet - Get DRAM interface bank base.
*
* DESCRIPTION:
*       This function returns the 32 bit base address of a given DRAM bank.
*
* INPUT:
*       bankNum - Bank number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM bank size. If bank is disabled or paramter is invalid, the 
*		function returns -1.
*
*******************************************************************************/
MV_U32 mvDramIfBankBaseGet(MV_U32 bankNum)
{
	DB(mvOsPrintf("Dram: mvDramIfBankBaseGet Bank %d base addr is %x \n",
				  bankNum, mvCpuIfTargetWinBaseLowGet(SDRAM_CS0 + bankNum)));
	return mvCpuIfTargetWinBaseLowGet(SDRAM_CS0 + bankNum);
}

/*******************************************************************************
* mvDramIfBankSizeGet - Get DRAM interface bank size.
*
* DESCRIPTION:
*       This function returns the size of a given DRAM bank.
*
* INPUT:
*       bankNum - Bank number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM bank size. If bank is disabled the function return '0'. In case 
*		or paramter is invalid, the function returns -1.
*
*******************************************************************************/
MV_U32 mvDramIfBankSizeGet(MV_U32 bankNum)
{
	DB(mvOsPrintf("Dram: mvDramIfBankSizeGet Bank %d size is %x \n",
				  bankNum, mvCpuIfTargetWinSizeGet(SDRAM_CS0 + bankNum)));
	return mvCpuIfTargetWinSizeGet(SDRAM_CS0 + bankNum);
}


/*******************************************************************************
* mvDramIfSizeGet - Get DRAM interface total size.
*
* DESCRIPTION:
*       This function get the DRAM total size.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       DRAM total size. In case or paramter is invalid, the function 
*		returns -1.
*
*******************************************************************************/
MV_U32 mvDramIfSizeGet(MV_VOID)
{
	MV_U32 size = 0, i;
	
	for(i = 0; i < MV_DRAM_MAX_CS; i++)
		size += mvDramIfBankSizeGet(i);
	
	DB(mvOsPrintf("Dram: mvDramIfSizeGet size is %x \n",size));
	return size;
}

/*******************************************************************************
* mvDramIfSingleBitErrThresholdSet - Set single bit ECC threshold.
*
* DESCRIPTION:
*       The ECC single bit error threshold is the number of single bit 
*       errors to happen before the Dunit generates an interrupt.
*       This function set single bit ECC threshold.
*
* INPUT:
*       threshold - threshold.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if threshold is to big, MV_OK otherwise.
*
*******************************************************************************/
MV_STATUS mvDramIfSingleBitErrThresholdSet(MV_U32 threshold)
{
    MV_U32 regVal; 

    if (threshold > SECR_THRECC_MAX)
    {
        return MV_BAD_PARAM;
    }

    regVal = MV_REG_READ(SDRAM_ECC_CONTROL_REG);
    regVal &= ~SECR_THRECC_MASK;
    regVal |= ((SECR_THRECC(threshold) & SECR_THRECC_MASK));
    MV_REG_WRITE(SDRAM_ECC_CONTROL_REG, regVal);

    return MV_OK;
}

#ifndef MV_STATIC_DRAM_ON_BOARD
/*******************************************************************************
* minCasCalc - Calculate the Minimum CAS latency which can be used.
*
* DESCRIPTION:
*	Calculate the minimum CAS latency that can be used, base on the DRAM
*	parameters and the SDRAM bus Clock freq.
*
* INPUT:
*	busClk    - the DRAM bus Clock.
*	pBankInfo - bank info parameters.
*	forcedCl - Forced CAS Latency multiplied by 10. If equal to zero, do not force.
*
* OUTPUT:
*       None
*
* RETURN:
*       The minimum CAS Latency. The function returns 0 if max CAS latency
*		supported by banks is incompatible with system bus clock frequancy.
*
*******************************************************************************/

static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo,MV_DRAM_BANK_INFO *pBankInfo2, MV_U32 busClk, MV_U32 forcedCl)
{
	MV_U32 count = 1, j;
	MV_U32 busClkPs = 1000000000 / (busClk / 1000);  /* in ps units */
	MV_U32 startBit, stopBit;
	MV_U32 minCas0 = 0, minCas2 = 0;
	
	
	/*     DDR 2:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  | TBD  |  5   |  4   |  3   |  2   | TBD  | TBD  * 
	Disco VI=	* TBD  | TBD  |  5   |  4   |  3   |  TBD   | TBD | TBD * 
	Disco Duo=	* TBD  |   6  |  5   |  4   |  3   |  TBD   | TBD | TBD * 
			*********************************************************/
	
	
	/* If we are asked to use the forced CAL  we change the suported CAL to be forcedCl only */
	if (forcedCl)
	{
		mvOsOutput("DRAM: Using forced CL %d.%d\n", (forcedCl / 10), (forcedCl % 10));
	
			if (forcedCl == 30)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 40)
				pBankInfo->suportedCasLatencies = 0x10;
			else if (forcedCl == 50)
				pBankInfo->suportedCasLatencies = 0x20;
			else if (forcedCl == 60)
				pBankInfo->suportedCasLatencies = 0x40;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 4\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x10;
			}

		return pBankInfo->suportedCasLatencies;        
	}   
	
	/* go over the supported cas mask from Max Cas down and check if the 	*/
	/* SysClk stands in its time requirments.				*/

	DB(mvOsPrintf("Dram: minCasCalc supported mask = %x busClkPs = %x \n",
								pBankInfo->suportedCasLatencies,busClkPs ));
	count = 1;
	for(j = 7; j > 0; j--)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			/* Reset the bits for CL incompatible for the sysClk */
			switch (count)
			{
				case 1: 
					if (pBankInfo->minCycleTimeAtMaxCasLatPs > busClkPs) 
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 2: 
					if (pBankInfo->minCycleTimeAtMaxCasLatMinus1Ps > busClkPs)
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 3: 
					if (pBankInfo->minCycleTimeAtMaxCasLatMinus2Ps > busClkPs)
						pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				default: 
					pBankInfo->suportedCasLatencies &= ~(BIT0 << j);
					break;
			}
		}
	}
	
	DB(mvOsPrintf("Dram: minCasCalc support = %x (after SysCC calc)\n",
											pBankInfo->suportedCasLatencies ));

	count = 1;
	DB(mvOsPrintf("Dram2: minCasCalc supported mask = %x busClkPs = %x \n",
								pBankInfo2->suportedCasLatencies,busClkPs ));
	for(j = 7; j > 0; j--)
	{
		if((pBankInfo2->suportedCasLatencies >> j) & BIT0 )
		{
			/* Reset the bits for CL incompatible for the sysClk */
			switch (count)
			{
				case 1: 
					if (pBankInfo2->minCycleTimeAtMaxCasLatPs > busClkPs) 
						pBankInfo2->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 2: 
					if (pBankInfo2->minCycleTimeAtMaxCasLatMinus1Ps > busClkPs)
						pBankInfo2->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				case 3: 
					if (pBankInfo2->minCycleTimeAtMaxCasLatMinus2Ps > busClkPs)
						pBankInfo2->suportedCasLatencies &= ~(BIT0 << j);
					count++;
					break;
				default: 
					pBankInfo2->suportedCasLatencies &= ~(BIT0 << j);
					break;
			}
		}
	}
	
	DB(mvOsPrintf("Dram2: minCasCalc support = %x (after SysCC calc)\n",
									pBankInfo2->suportedCasLatencies ));

	startBit = 3;   /* DDR2 support CL start with CL3 (bit 3) */
	stopBit  = 6;   /* DDR2 support CL stops with CL6 (bit 6) */

	for(j = startBit; j <= stopBit ; j++)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			DB(mvOsPrintf("Dram: minCasCalc choose CAS %x \n",(BIT0 << j)));
			minCas0 = (BIT0 << j);
			break;
		}
	}

	for(j = startBit; j <= stopBit ; j++)
	{
		if((pBankInfo2->suportedCasLatencies >> j) & BIT0 )
		{
			DB(mvOsPrintf("Dram: minCasCalc choose CAS %x \n",(BIT0 << j)));
			minCas2 = (BIT0 << j);
			break;
		}
	}
	
	if (minCas2 > minCas0)
		return minCas2;
	else
		return minCas0;
	
	return 0; 
}

/*******************************************************************************
* sdramConfigRegCalc - Calculate sdram config register
*
* DESCRIPTION: Calculate sdram config register optimized value based
*			on the bank info parameters.
*
* INPUT:
*	busClk    - the DRAM bus Clock.
*	pBankInfo - sdram bank parameters
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram config reg value.
*
*******************************************************************************/
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo,MV_DRAM_BANK_INFO *pBankInfo2, MV_U32 busClk)
{
	MV_U32 sdramConfig = 0;
	MV_U32 refreshPeriod;
	
	busClk /= 1000000; /* we work with busClk in MHz */
	
	sdramConfig = MV_REG_READ(SDRAM_CONFIG_REG);
	
	/* figure out the memory refresh internal */
	switch (pBankInfo->refreshInterval & 0xf)
	{
		case 0x0: /* refresh period is 15.625 usec */
				refreshPeriod = 15625;
				break;
		case 0x1: /* refresh period is 3.9 usec  	*/
				refreshPeriod = 3900;
				break;
		case 0x2: /* refresh period is 7.8 usec 	*/
				refreshPeriod = 7800;
				break;
		case 0x3: /* refresh period is 31.3 usec	*/
				refreshPeriod = 31300;
				break;
		case 0x4: /* refresh period is 62.5 usec	*/
				refreshPeriod = 62500;
				break;
		case 0x5: /* refresh period is 125 usec 	*/
				refreshPeriod = 125000;
				break;
		default:  /* refresh period undefined 					*/
				mvOsPrintf("Dram: ERR. DRAM refresh period is unknown!\n");
				return -1;
    }
	
	/* Now the refreshPeriod is in register format value */
	refreshPeriod = (busClk * refreshPeriod) / 1000;
	
	DB(mvOsPrintf("Dram: sdramConfigRegCalc calculated refresh interval %0x\n", 
				  refreshPeriod));

	/* make sure the refresh value is only 14 bits */
	if(refreshPeriod > SDRAM_REFRESH_MAX)
	{
		refreshPeriod = SDRAM_REFRESH_MAX;
		DB(mvOsPrintf("Dram: sdramConfigRegCalc adjusted refresh interval %0x\n", 
					  refreshPeriod));
	}
	
	/* Clear the refresh field */
	sdramConfig &= ~SDRAM_REFRESH_MASK;
	
	/* Set new value to refresh field */
	sdramConfig |= (refreshPeriod & SDRAM_REFRESH_MASK);
	
	/*  registered DRAM ? */
	if ( pBankInfo->registeredAddrAndControlInputs )
	{
		/* it's registered DRAM, so set the reg. DRAM bit */
		sdramConfig |= SDRAM_REGISTERED;
		DB(mvOsPrintf("DRAM Attribute: Registered address and control inputs.\n");)
	}

	/* ECC and IERR support */
	sdramConfig &= ~SDRAM_ECC_MASK;    /* Clear ECC field */
	sdramConfig &= ~SDRAM_IERR_MASK;    /* Clear IErr field */

	if ( pBankInfo->errorCheckType ) 
	{
		sdramConfig |= SDRAM_ECC_EN;
		sdramConfig |= SDRAM_IERR_REPORTE; 
                DB(mvOsPrintf("Dram: mvDramIfDetect Enabling ECC\n"));
	}
	else
	{
                sdramConfig |= SDRAM_ECC_DIS;
		sdramConfig |= SDRAM_IERR_IGNORE; 
                DB(mvOsPrintf("Dram: mvDramIfDetect Disabling ECC!\n"));
	}
	/* Set static default settings */
	sdramConfig |= SDRAM_CONFIG_DV;
	
	DB(mvOsPrintf("Dram: sdramConfigRegCalc set sdramConfig to 0x%x\n",
				  sdramConfig));
	
 	return sdramConfig;  
}

/*******************************************************************************
* sdramModeRegCalc - Calculate sdram mode register
*
* DESCRIPTION: Calculate sdram mode register optimized value based
*			on the bank info parameters and the minCas.
*
* INPUT:
*	minCas	  - minimum CAS supported. 
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram mode reg value.
*
*******************************************************************************/
static MV_U32 sdramModeRegCalc(MV_U32 minCas)
{
	MV_U32 sdramMode;
		
	sdramMode = MV_REG_READ(SDRAM_MODE_REG);
	
	/* Clear CAS Latency field */
	sdramMode &= ~SDRAM_CL_MASK;
	
	DB(mvOsPrintf("DRAM CAS Latency ");)
	
		switch (minCas)
		{
			case DDR2_CL_3: 
				sdramMode |= SDRAM_DDR2_CL_3;
				DB(mvOsPrintf("3.\n");)
				break;
			case DDR2_CL_4: 
				sdramMode |= SDRAM_DDR2_CL_4;
				DB(mvOsPrintf("4.\n");)
				break;
			case DDR2_CL_5: 
				sdramMode |= SDRAM_DDR2_CL_5;
				DB(mvOsPrintf("5.\n");)
				break;
			case DDR2_CL_6: 
				sdramMode |= SDRAM_DDR2_CL_6;
				DB(mvOsPrintf("6.\n");)
				break;
			default:
				mvOsOutput("\nsdramModeRegCalc ERROR: Max. CL out of range\n");
				return -1;
        }

	DB(mvOsPrintf("\nsdramModeRegCalc register 0x%x\n", sdramMode ));

	return sdramMode;
}
/*******************************************************************************
* sdramExtModeRegCalc - Calculate sdram Extended mode register
*
* DESCRIPTION: 
*		Return sdram Extended mode register value based
*		on the bank info parameters and bank presence.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*	busClk - DRAM frequency
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram Extended mode reg value.
*
*******************************************************************************/
static MV_U32 sdramExtModeRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk)
{
	MV_U32 populateBanks = 0;
	int bankNum;

		/* Represent the populate banks in binary form */
		for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
		{
			if (0 != pBankInfo[bankNum].size)
			{
				populateBanks |= (1 << bankNum);
			}
		}
	
		switch(populateBanks)
		{
			case(BANK_PRESENT_CS0):
			case(BANK_PRESENT_CS0_CS1):
				return DDR_SDRAM_EXT_MODE_CS0_CS1_DV;
		
			case(BANK_PRESENT_CS0_CS2):
			case(BANK_PRESENT_CS0_CS1_CS2):
			case(BANK_PRESENT_CS0_CS2_CS3):
			case(BANK_PRESENT_CS0_CS2_CS3_CS4):
				if (busClk >= MV_BOARD_SYSCLK_267MHZ)
				    return DDR_SDRAM_EXT_MODE_FAST_CS0_CS1_CS2_CS3_DV;
				else
				    return DDR_SDRAM_EXT_MODE_CS0_CS1_CS2_CS3_DV;
		
			default:
				mvOsOutput("sdramExtModeRegCalc: Invalid DRAM bank presence\n");
				return -1;
		} 
	return 0;
}

/*******************************************************************************
* dunitCtrlLowRegCalc - Calculate sdram dunit control low register
*
* DESCRIPTION: Calculate sdram dunit control low register optimized value based
*			on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*	minCas	  - minimum CAS supported. 
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram dunit control low reg value.
*
*******************************************************************************/
static MV_U32 dunitCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32  busClk, MV_STATUS TTMode)
{
	MV_U32 dunitCtrlLow, cl;
	MV_U32 sbOutR[4]={3,5,7,9} ;
	MV_U32 sbOutU[4]={1,3,5,7} ;
	
    	dunitCtrlLow = MV_REG_READ(SDRAM_DUNIT_CTRL_REG);

        DB(mvOsPrintf("Dram: dunitCtrlLowRegCalc\n"));

	/* Clear StBurstOutDel field */
	dunitCtrlLow &= ~SDRAM_SB_OUT_MASK;
	
	/* Clear StBurstInDel field */
	dunitCtrlLow &= ~SDRAM_SB_IN_MASK;

	/* Clear CtrlPos field */
	dunitCtrlLow &= ~SDRAM_CTRL_POS_MASK;

	/* Clear 2T field */
	dunitCtrlLow &= ~SDRAM_2T_MASK;
	if (TTMode == MV_TRUE)
	{
		dunitCtrlLow |= SDRAM_2T_MODE;
	}
	
	/* For proper sample of read data set the Dunit Control register's      */
	/* stBurstInDel bits [27:24]                                            */
	/*		200MHz - 267MHz None reg  = CL + 1			*/
	/*		200MHz - 267MHz reg	  = CL + 2			*/
	/*		> 267MHz None reg  = CL + 2			*/
	/*		> 267MHz reg	  = CL + 3			*/
	
	/* For proper sample of read data set the Dunit Control register's      */
	/* stBurstOutDel bits [23:20]                                           */
			/********-********-********-********-
			*  CL=3  |  CL=4  |  CL=5  |  CL=6  |
			*********-********-********-********-
	Not Reg.	*  0001  |  0011  |  0101  |  0111  |
			*********-********-********-********-
	Registered	*  0011  |  0101  |  0111  |  1001  |
			*********-********-********-********/
    
		/* Set Dunit Control low default value */
		dunitCtrlLow |= SDRAM_DUNIT_CTRL_LOW_DDR2_DV; 

		switch (minCas)
		{
			case DDR2_CL_3: cl = 3; break;
			case DDR2_CL_4: cl = 4; break;
			case DDR2_CL_5: cl = 5; break;
			case DDR2_CL_6: cl = 6; break;
			default:
				mvOsOutput("Dram: dunitCtrlLowRegCalc Max. CL out of range %d\n", minCas);
				return -1;
		}

		/* registerd DDR SDRAM? */
		if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
		{
			dunitCtrlLow |= (sbOutR[cl-3]) << SDRAM_SB_OUT_DEL_OFFS;
		}
		else
		{
			dunitCtrlLow |= (sbOutU[cl-3]) << SDRAM_SB_OUT_DEL_OFFS;
		}

		DB(mvOsPrintf("\n\ndunitCtrlLowRegCalc: CL = %d, frequencies=%d\n", cl, busClk));

		if (busClk <= MV_BOARD_SYSCLK_267MHZ)
		{
			if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
				cl = cl + 2;
			else
				cl = cl + 1;
		}
		else
		{
			if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
				cl = cl + 3;
			else
				cl = cl + 2;
		}
		
        DB(mvOsPrintf("dunitCtrlLowRegCalc: SDRAM_SB_IN_DEL_OFFS = %d \n", cl));
		dunitCtrlLow |= cl << SDRAM_SB_IN_DEL_OFFS;

	DB(mvOsPrintf("Dram: Reg dunit control low = %x\n", dunitCtrlLow ));

	return dunitCtrlLow;
}  

/*******************************************************************************
* dunitCtrlHighRegCalc - Calculate sdram dunit control high register
*
* DESCRIPTION: Calculate sdram dunit control high register optimized value based
*			on the bus clock.
*
* INPUT:
*	busClk	  - DRAM frequency. 
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram dunit control high reg value.
*
*******************************************************************************/
static MV_U32 dunitCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32  busClk)
{
	MV_U32 dunitCtrlHigh;
	dunitCtrlHigh = MV_REG_READ(SDRAM_DUNIT_CTRL_HI_REG);
	if(busClk > MV_BOARD_SYSCLK_300MHZ) 
		dunitCtrlHigh |= SDRAM__P2D_EN;
	else
		dunitCtrlHigh &= ~SDRAM__P2D_EN;

	if(busClk > MV_BOARD_SYSCLK_267MHZ) 
	    dunitCtrlHigh |= (SDRAM__WR_MESH_DELAY_EN | SDRAM__PUP_ZERO_SKEW_EN | SDRAM__ADD_HALF_FCC_EN);

	/* If ECC support we turn on D2P sample */
	dunitCtrlHigh &= ~SDRAM__D2P_EN;    /* Clear D2P bit */
	if (( pBankInfo->errorCheckType ) && (busClk > MV_BOARD_SYSCLK_267MHZ))
		dunitCtrlHigh |= SDRAM__D2P_EN;

	return dunitCtrlHigh;
}

/*******************************************************************************
* sdramAddrCtrlRegCalc - Calculate sdram address control register
*
* DESCRIPTION: Calculate sdram address control register optimized value based
*			on the bank info parameters and the minCas.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram address control reg value.
*
*******************************************************************************/
static MV_U32 sdramAddrCtrlRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_DRAM_BANK_INFO *pBankInfoDIMM1)
{
	MV_U32 addrCtrl = 0;
	
	if (pBankInfoDIMM1->size)
	{
		switch (pBankInfoDIMM1->sdramWidth)
		{
			case 4:  /* memory is x4 */
				mvOsOutput("sdramAddrCtrlRegCalc: Error - x4 not supported!\n");
				return -1;
				break;
			case 8:  /* memory is x8 */
				addrCtrl |= SDRAM_ADDRSEL_X8(2) | SDRAM_ADDRSEL_X8(3);
				DB(mvOsPrintf("sdramAddrCtrlRegCalc: sdramAddrCtrlRegCalc SDRAM device DIMM2 width x8\n"));
				break;
			case 16:
				addrCtrl |= SDRAM_ADDRSEL_X16(2) | SDRAM_ADDRSEL_X16(3);
				DB(mvOsPrintf("sdramAddrCtrlRegCalc: sdramAddrCtrlRegCalc SDRAM device DIMM2 width x16\n"));
				break;
			default: /* memory width unsupported */
				mvOsOutput("sdramAddrCtrlRegCalc: ERR. DRAM chip width is unknown!\n");
				return -1;
		}
	}

	switch (pBankInfo->sdramWidth)
	{
		case 4:  /* memory is x4 */
			mvOsOutput("sdramAddrCtrlRegCalc: Error - x4 not supported!\n");
			return -1;
			break;
		case 8:  /* memory is x8 */
			addrCtrl |= SDRAM_ADDRSEL_X8(0) | SDRAM_ADDRSEL_X8(1);
			DB(mvOsPrintf("sdramAddrCtrlRegCalc: sdramAddrCtrlRegCalc SDRAM device width x8\n"));
			break;
		case 16:
			addrCtrl |= SDRAM_ADDRSEL_X16(0) | SDRAM_ADDRSEL_X16(1);
			DB(mvOsPrintf("sdramAddrCtrlRegCalc: sdramAddrCtrlRegCalc SDRAM device width x16\n"));
			break;
		default: /* memory width unsupported */
			mvOsOutput("sdramAddrCtrlRegCalc: ERR. DRAM chip width is unknown!\n");
			return -1;
	}

	/* Note that density is in MB units */
	switch (pBankInfo->deviceDensity) 
	{
		case 256:                 /* 256 Mbit */
			DB(mvOsPrintf("DRAM Device Density 256Mbit\n"));
			addrCtrl |= SDRAM_DSIZE_256Mb(0) | SDRAM_DSIZE_256Mb(1);
			break;
		case 512:                /* 512 Mbit */
			DB(mvOsPrintf("DRAM Device Density 512Mbit\n"));
			addrCtrl |= SDRAM_DSIZE_512Mb(0) | SDRAM_DSIZE_512Mb(1);
			break;
		case 1024:                /* 1 Gbit */
			DB(mvOsPrintf("DRAM Device Density 1Gbit\n"));
			addrCtrl |= SDRAM_DSIZE_1Gb(0) | SDRAM_DSIZE_1Gb(1);
			break;
		case 2048:                /* 2 Gbit */
			DB(mvOsPrintf("DRAM Device Density 2Gbit\n"));
			addrCtrl |= SDRAM_DSIZE_2Gb(0) | SDRAM_DSIZE_2Gb(1);
			break;
		default:
			mvOsOutput("Dram: sdramAddrCtrl unsupported RAM-Device size %d\n",
                       pBankInfo->deviceDensity);
			return -1;
        }

	if (pBankInfoDIMM1->size)
	{
		switch (pBankInfoDIMM1->deviceDensity) 
		{
			case 256:                 /* 256 Mbit */
				DB(mvOsPrintf("DIMM2: DRAM Device Density 256Mbit\n"));
				addrCtrl |= SDRAM_DSIZE_256Mb(2) | SDRAM_DSIZE_256Mb(3);
				break;
			case 512:                /* 512 Mbit */
				DB(mvOsPrintf("DIMM2: DRAM Device Density 512Mbit\n"));
				addrCtrl |= SDRAM_DSIZE_512Mb(2) | SDRAM_DSIZE_512Mb(3);
				break;
			case 1024:                /* 1 Gbit */
				DB(mvOsPrintf("DIMM2: DRAM Device Density 1Gbit\n"));
				addrCtrl |= SDRAM_DSIZE_1Gb(2) | SDRAM_DSIZE_1Gb(3);
				break;
			case 2048:                /* 2 Gbit */
				DB(mvOsPrintf("DIMM2: DRAM Device Density 2Gbit\n"));
				addrCtrl |= SDRAM_DSIZE_2Gb(2) | SDRAM_DSIZE_2Gb(3);
				break;
			default:
				mvOsOutput("DIMM2: Dram: sdramAddrCtrl unsupported RAM-Device size %d\n",
						   pBankInfoDIMM1->deviceDensity);
				return -1;
		}
	}
	/* SDRAM address control */
	DB(mvOsPrintf("Dram: setting sdram address control with: %x \n", addrCtrl));

	return addrCtrl;
}

/*******************************************************************************
* sdramTimeCtrlLowRegCalc - Calculate sdram timing control low register
*
* DESCRIPTION: 
*       This function calculates sdram timing control low register 
*       optimized value based on the bank info parameters and the minCas.
*
* INPUT:
*	    pBankInfo - sdram bank parameters
*	minCas	  - minimum CAS supported. 
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing control low reg value.
*
*******************************************************************************/
static MV_U32 sdramTimeCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas, MV_U32 busClk)
{
    MV_U32 tRp  = 0;
    MV_U32 tRrd = 0;
    MV_U32 tRcd = 0;
    MV_U32 tRas = 0;
    MV_U32 tWr  = 0;
    MV_U32 tWtr = 0;
    MV_U32 tRtp = 0;
    MV_U32 timeCtrlLow = 0;
	
    MV_U32 bankNum;
    
    busClk = busClk / 1000000;    /* In MHz */

    /* Scan all DRAM banks to find maximum timing values */
    for (bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
    {
        tRp  = MV_MAX(tRp,  pBankInfo[bankNum].minRowPrechargeTime);
        tRrd = MV_MAX(tRrd, pBankInfo[bankNum].minRowActiveToRowActive);
        tRcd = MV_MAX(tRcd, pBankInfo[bankNum].minRasToCasDelay);
        tRas = MV_MAX(tRas, pBankInfo[bankNum].minRasPulseWidth);
    }

    /* Extract timing (in ns) from SPD value. We ignore the tenth ns part.  */
    /* by shifting the data two bits right.                                 */
    tRp  = tRp  >> 2;    /* For example 0x50 -> 20ns                        */
    tRrd = tRrd >> 2;
    tRcd = tRcd >> 2;
	
    /* Extract clock cycles from time parameter. We need to round up        */
    tRp  = ((busClk * tRp)  / 1000) + (((busClk * tRp)  % 1000) ? 1 : 0);
    DB(mvOsPrintf("Dram  Timing Low: tRp = %d ", tRp));
    tRrd = ((busClk * tRrd) / 1000) + (((busClk * tRrd) % 1000) ? 1 : 0);
	/* JEDEC min reqeirments tRrd = 2 */
	if (tRrd < 2)
		tRrd = 2;
    DB(mvOsPrintf("tRrd = %d ", tRrd));
    tRcd = ((busClk * tRcd) / 1000) + (((busClk * tRcd) % 1000) ? 1 : 0);
    DB(mvOsPrintf("tRcd = %d ", tRcd));
    tRas = ((busClk * tRas) / 1000) + (((busClk * tRas) % 1000) ? 1 : 0);
    DB(mvOsPrintf("tRas = %d ", tRas));

    /* tWr and tWtr is different for DDR1 and DDR2. tRtp is only for DDR2   */
	/* Scan all DRAM banks to find maximum timing values */
	for (bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
	    tWr  = MV_MAX(tWr,  pBankInfo[bankNum].minWriteRecoveryTime);
	    tWtr = MV_MAX(tWtr, pBankInfo[bankNum].minWriteToReadCmdDelay);
	    tRtp = MV_MAX(tRtp, pBankInfo[bankNum].minReadToPrechCmdDelay);
	}
	
	/* Extract timing (in ns) from SPD value. We ignore the tenth ns    */
	/* part by shifting the data two bits right.                        */
	tWr  = tWr  >> 2;    /* For example 0x50 -> 20ns                    */
	tWtr = tWtr >> 2;
	tRtp = tRtp >> 2;
	/* Extract clock cycles from time parameter. We need to round up    */
	tWr  = ((busClk * tWr)  / 1000) + (((busClk * tWr)  % 1000) ? 1 : 0);
	DB(mvOsPrintf("tWr = %d ", tWr));
	tWtr = ((busClk * tWtr) / 1000) + (((busClk * tWtr) % 1000) ? 1 : 0);
	/* JEDEC min reqeirments tWtr = 2 */
	if (tWtr < 2)
		tWtr = 2;
	DB(mvOsPrintf("tWtr = %d ", tWtr));
	tRtp = ((busClk * tRtp) / 1000) + (((busClk * tRtp) % 1000) ? 1 : 0);
	/* JEDEC min reqeirments tRtp = 2 */
	if (tRtp < 2)
	tRtp = 2;
	DB(mvOsPrintf("tRtp = %d ", tRtp));

	/* Note: value of 0 in register means one cycle, 1 means two and so on  */
	timeCtrlLow = (((tRp  - 1) << SDRAM_TRP_OFFS) |
		    ((tRrd - 1) << SDRAM_TRRD_OFFS) |
		    ((tRcd - 1) << SDRAM_TRCD_OFFS) |
		    (((tRas - 1) << SDRAM_TRAS_OFFS) & SDRAM_TRAS_MASK)|
		    ((tWr  - 1) << SDRAM_TWR_OFFS)  |
		    ((tWtr - 1) << SDRAM_TWTR_OFFS)	|
		    ((tRtp - 1) << SDRAM_TRTP_OFFS));
	
	/* Check extended tRas bit */
	if ((tRas - 1) & BIT4)
	    timeCtrlLow |= (1 << SDRAM_EXT_TRAS_OFFS);

	return timeCtrlLow;
}

/*******************************************************************************
* sdramTimeCtrlHighRegCalc - Calculate sdram timing control high register
*
* DESCRIPTION: 
*       This function calculates sdram timing control high register 
*       optimized value based on the bank info parameters and the bus clock.
*
* INPUT:
*	    pBankInfo - sdram bank parameters
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timing control high reg value.
*
*******************************************************************************/
static MV_U32 sdramTimeCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk)
{
	MV_U32 tRfc;
	MV_U32 timingHigh;
	MV_U32 timeNs = 0;
	MV_U32 bankNum;
	
	busClk = busClk / 1000000;    /* In MHz */

	/* Set DDR timing high register static configuration bits */
	timingHigh = MV_REG_READ(SDRAM_TIMING_CTRL_HIGH_REG);
	
	/* Set DDR timing high register default value */
	timingHigh |= SDRAM_TIMING_CTRL_HIGH_REG_DV;  
	
	/* Clear tRfc field */
	timingHigh &= ~SDRAM_TRFC_MASK;

	/* Scan all DRAM banks to find maximum timing values */
	for (bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		timeNs = MV_MAX(timeNs,  pBankInfo[bankNum].minRefreshToActiveCmd);
		DB(mvOsPrintf("Dram:  Timing High: minRefreshToActiveCmd = %d\n", 
				pBankInfo[bankNum].minRefreshToActiveCmd));
	}
	if(busClk >= 333 && mvCtrlModelGet() == MV_78XX0_A1_REV)
    {
        timingHigh |= 0x1 << SDRAM_TR2W_W2R_OFFS;
    }

	tRfc = ((busClk * timeNs)  / 1000) + (((busClk * timeNs)  % 1000) ? 1 : 0);
	/* Note: value of 0 in register means one cycle, 1 means two and so on  */
	DB(mvOsPrintf("Dram:  Timing High: tRfc = %d\n", tRfc));
	timingHigh |= (((tRfc - 1) & SDRAM_TRFC_MASK) << SDRAM_TRFC_OFFS);
	DB(mvOsPrintf("Dram:  Timing High: tRfc = %d\n", tRfc));
	
	/* SDRAM timing high */
	DB(mvOsPrintf("Dram: setting timing high with: %x \n", timingHigh));

	return timingHigh;
}
/*******************************************************************************
* sdramDDr2OdtConfig - Set DRAM DDR2 On Die Termination registers.
*
* DESCRIPTION: 
*       This function config DDR2 On Die Termination (ODT) registers.
*	
* INPUT:
*		pBankInfo - bank info parameters.
*
* OUTPUT:
*       None
*
* RETURN:
*       None
*******************************************************************************/
static void sdramDDr2OdtConfig(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 populateBanks = 0;
	MV_U32 odtCtrlLow, odtCtrlHigh, dunitOdtCtrl;
	int bankNum;
	
	/* Represent the populate banks in binary form */
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		if (0 != pBankInfo[bankNum].size)
		{
				populateBanks |= (1 << bankNum);
			}
		}
	
	switch(populateBanks)
	{
		case(BANK_PRESENT_CS0):
		case(BANK_PRESENT_CS0_CS1):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS1_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS1_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS1_DV;
			break;
		case(BANK_PRESENT_CS0_CS2):
		case(BANK_PRESENT_CS0_CS1_CS2):
		case(BANK_PRESENT_CS0_CS2_CS3):
		case(BANK_PRESENT_CS0_CS2_CS3_CS4):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS1_CS2_CS3_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS1_CS2_CS3_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS1_CS2_CS3_DV;
			break;
		default:
			DB(mvOsPrintf("sdramDDr2OdtConfig: Invalid DRAM bank presence\n"));
			return;
	}
 	/* DDR2 SDRAM ODT ctrl low  */
	DB(mvOsPrintf("Dram: DDR2 setting ODT ctrl low with: %x \n", odtCtrlLow));
	MV_REG_WRITE(DRAM_BUF_REG7, odtCtrlLow);

 	/* DDR2 SDRAM ODT ctrl high  */
	DB(mvOsPrintf("Dram: DDR2 setting ODT ctrl high with: %x \n", odtCtrlHigh));
	MV_REG_WRITE(DRAM_BUF_REG8, odtCtrlHigh);

	/* DDR2 DUNIT ODT ctrl  */
	if ( ((mvCtrlModelGet() == MV_78XX0_DEV_ID) && (mvCtrlRevGet() == MV_78XX0_Y0_REV)) ||
		(mvCtrlModelGet() == MV_76100_DEV_ID) ||
		(mvCtrlModelGet() == MV_78100_DEV_ID) ||
		(mvCtrlModelGet() == MV_78200_DEV_ID) )
		dunitOdtCtrl &= ~(BIT9|BIT8); /* Clear ODT always on */

	DB(mvOsPrintf("DUNIT: DDR2 setting ODT ctrl with: %x \n", dunitOdtCtrl));
	MV_REG_WRITE(DRAM_BUF_REG9, dunitOdtCtrl);
	return;
}
/*******************************************************************************
* sdramDdr2TimeLoRegCalc - Set DDR2 DRAM Timing Low registers.
*
* DESCRIPTION: 
*       This function config DDR2 DRAM Timing low registers.
*	
* INPUT:
*	minCas	  - minimum CAS supported. 
*
* OUTPUT:
*       None
*
* RETURN:
*       DDR2 sdram timing low reg value.
*******************************************************************************/
static MV_U32 sdramDdr2TimeLoRegCalc(MV_U32 minCas)
{
	MV_U8 cl = -1;
	MV_U32 ddr2TimeLoReg;

	/* read and clear the feilds we are going to set */
	ddr2TimeLoReg = MV_REG_READ(SDRAM_DDR2_TIMING_LO_REG);
	ddr2TimeLoReg &= ~(SD2TLR_TODT_ON_RD_MASK	| 
			   SD2TLR_TODT_OFF_RD_MASK	| 
			   SD2TLR_TODT_ON_CTRL_RD_MASK	|
			   SD2TLR_TODT_OFF_CTRL_RD_MASK);

	if( minCas == DDR2_CL_3 )
	{
		cl = 3;
	}
	else if( minCas == DDR2_CL_4 )
	{
		cl = 4;
	}
	else if( minCas == DDR2_CL_5 )
	{
		cl = 5;
	}
	else if( minCas == DDR2_CL_6 )
	{
		cl = 6;
	}
	else
	{
		DB(mvOsPrintf("sdramDdr2TimeLoRegCalc: CAS latency %d unsupported. using CAS latency 4\n",
				minCas));
		cl = 4;
	}

	ddr2TimeLoReg |= ((cl-3) << SD2TLR_TODT_ON_RD_OFFS);
	ddr2TimeLoReg |= ( cl << SD2TLR_TODT_OFF_RD_OFFS);
	ddr2TimeLoReg |= ( cl << SD2TLR_TODT_ON_CTRL_RD_OFFS);
	ddr2TimeLoReg |= ((cl+3) << SD2TLR_TODT_OFF_CTRL_RD_OFFS);

	/* DDR2 SDRAM timing low */
	DB(mvOsPrintf("Dram: DDR2 setting timing low with: %x \n", ddr2TimeLoReg));

	return ddr2TimeLoReg;
}

/*******************************************************************************
* sdramDdr2TimeHiRegCalc - Set DDR2 DRAM Timing High registers.
*
* DESCRIPTION: 
*       This function config DDR2 DRAM Timing high registers.
*	
* INPUT:
*	minCas	  - minimum CAS supported. 
*
* OUTPUT:
*       None
*
* RETURN:
*       DDR2 sdram timing high reg value.
*******************************************************************************/
static MV_U32 sdramDdr2TimeHiRegCalc(MV_U32 minCas)
{
	MV_U8 cl = -1;
	MV_U32 ddr2TimeHiReg;

	/* read and clear the feilds we are going to set */
	ddr2TimeHiReg = MV_REG_READ(SDRAM_DDR2_TIMING_HI_REG);
	ddr2TimeHiReg &= ~(SD2THR_TODT_ON_WR_MASK	|
			   SD2THR_TODT_OFF_WR_MASK	|
			   SD2THR_TODT_ON_CTRL_WR_MASK	|
			   SD2THR_TODT_OFF_CTRL_WR_MASK);

	if( minCas == DDR2_CL_3 )
	{
		cl = 3;
	}
	else if( minCas == DDR2_CL_4 )
	{
		cl = 4;
	}
	else if( minCas == DDR2_CL_5 )
	{
		cl = 5;
	}
	else if( minCas == DDR2_CL_6 )
	{
		cl = 6;
	}
	else
	{
		mvOsOutput("sdramDdr2TimeHiRegCalc: CAS latency %d unsupported. using CAS latency 4\n", 
				minCas);
		cl = 4;
	}

	ddr2TimeHiReg |= ((cl-3) << SD2THR_TODT_ON_WR_OFFS);
	ddr2TimeHiReg |= ( cl << SD2THR_TODT_OFF_WR_OFFS);
	ddr2TimeHiReg |= ( cl << SD2THR_TODT_ON_CTRL_WR_OFFS);
	ddr2TimeHiReg |= ((cl+3) << SD2THR_TODT_OFF_CTRL_WR_OFFS);

	/* DDR2 SDRAM timin high  */
	DB(mvOsPrintf("Dram: DDR2 setting timing high with: %x \n", ddr2TimeHiReg));

	return ddr2TimeHiReg;
}
#endif

/*******************************************************************************
* mvDramIfCalGet - Get CAS Latency
*
* DESCRIPTION: 
*       This function get the CAS Latency.
*
* INPUT:
*       None
*
* OUTPUT:
*       None
*
* RETURN:
*       CAS latency times 10 (to avoid using floating point).
*
*******************************************************************************/
MV_U32 mvDramIfCalGet(void)
{
	MV_U32 sdramCasLat, casLatMask;
	
    casLatMask = (MV_REG_READ(SDRAM_MODE_REG) & SDRAM_CL_MASK);

    switch (casLatMask)
    {
        case SDRAM_DDR2_CL_3: 
            sdramCasLat = 30;
            break;
        case SDRAM_DDR2_CL_4: 
            sdramCasLat = 40;
            break;
        case SDRAM_DDR2_CL_5: 
            sdramCasLat = 50;
            break;
        case SDRAM_DDR2_CL_6: 
            sdramCasLat = 60;
            break;
        default:
            mvOsOutput("mvDramIfCalGet: Err, unknown DDR2 CAL\n");
            return -1;
    }                                  
    
    return sdramCasLat;
}


/*******************************************************************************
* mvDramIfSelfRefreshSet - Put the dram in self refresh mode - 
*
* DESCRIPTION: 
*               add support in power management.
*                          
*
* INPUT:
*       None
*
* OUTPUT:
*       None
*
* RETURN:
*       None
*
*******************************************************************************/

MV_VOID mvDramIfSelfRefreshSet()
{
    MV_U32 operReg;

      operReg =  MV_REG_READ(SDRAM_OPERATION_REG);
      MV_REG_WRITE(SDRAM_OPERATION_REG ,operReg |SDRAM_CMD_SLF_RFRSH);
      /* Read until register is reset to 0 */
      while(MV_REG_READ(SDRAM_OPERATION_REG));
}
/*******************************************************************************
* mvDramIfDimGetSPDversion - return DIMM SPD version.
*
* DESCRIPTION:
*		This function prints the DRAM controller information.
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*		None.
*
*******************************************************************************/
static void mvDramIfDimGetSPDversion(MV_U32 *pMajor, MV_U32 *pMinor, MV_U32 bankNum)
{
	MV_DIMM_INFO dimmInfo;
	if (bankNum >= MV_DRAM_MAX_CS )
	{
		DB(mvOsPrintf("Dram: mvDramIfDimGetSPDversion bad params \n")); 
		return ;
	}
	memset(&dimmInfo,0,sizeof(dimmInfo));
	if ( MV_OK != dimmSpdGet((MV_U32)(bankNum/2), &dimmInfo))
	{
		DB(mvOsPrintf("Dram: ERR dimmSpdGet failed to get dimm info \n"));
		return ;
	}
	*pMajor = dimmInfo.spdRawData[DIMM_SPD_VERSION]/10;
	*pMinor = dimmInfo.spdRawData[DIMM_SPD_VERSION]%10;
}
/*******************************************************************************
* mvDramIfShow - Show DRAM controller information.
*
* DESCRIPTION:
*		This function prints the DRAM controller information.
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*		None.
*
*******************************************************************************/
void mvDramIfShow(void)
{
    int i, sdramCasLat, sdramCsSize;
	MV_U32 Major=0, Minor=0;
    
    mvOsOutput("DRAM Controller info:\n");
    
    mvOsOutput("Total DRAM ");
    mvSizePrint(mvDramIfSizeGet());
    mvOsOutput("\n");

	for(i = 0; i < MV_DRAM_MAX_CS; i++)
	{ 
        sdramCsSize = mvDramIfBankSizeGet(i);
        if (sdramCsSize)
        {
			if (0 == (i & 1))
			{
				mvDramIfDimGetSPDversion(&Major, &Minor,i);
				mvOsOutput("DIMM %d version %d.%d\n", i/2, Major, Minor);
			}
            mvOsOutput("\tDRAM CS[%d] ", i);
            mvSizePrint(sdramCsSize);
            mvOsOutput("\n");
        }
    }
    sdramCasLat = mvDramIfCalGet();

    if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_ECC_EN)
    {
        mvOsOutput("ECC enabled, ");
    }
    else
    {
        mvOsOutput("ECC Disabled, ");
    }
    
    if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_REGISTERED)
    {
        mvOsOutput("Registered DIMM\n");
    }
    else
    {
        mvOsOutput("Non registered DIMM\n");
    }

    mvOsOutput("Configured CAS Latency %d.%d\n", sdramCasLat/10, sdramCasLat%10);
}
/*******************************************************************************
* mvDramIfGetFirstCS - find the  DRAM bank on the lower address
* 
*
* DESCRIPTION:
*       This function return the fisrt CS on address 0
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       SDRAM_CS0 or SDRAM_CS2
*
*******************************************************************************/
MV_U32 mvDramIfGetFirstCS(void)
{
	MV_DRAM_BANK_INFO bankInfo[MV_DRAM_MAX_CS];

	if (DRAM_CS_Order[0] == N_A)
	{
		mvDramBankInfoGet(SDRAM_CS0, &bankInfo[SDRAM_CS0]);
#ifdef MV_INCLUDE_SDRAM_CS2
		mvDramBankInfoGet(SDRAM_CS2, &bankInfo[SDRAM_CS2]);
#endif 
		
#ifdef MV_INCLUDE_SDRAM_CS2
		if (bankInfo[SDRAM_CS0].size <  bankInfo[SDRAM_CS2].size)
		{
			DRAM_CS_Order[0] = SDRAM_CS2;
			DRAM_CS_Order[1] = SDRAM_CS3;
			DRAM_CS_Order[2] = SDRAM_CS0;
			DRAM_CS_Order[3] = SDRAM_CS1;

			return SDRAM_CS2;
		}
#endif
		DRAM_CS_Order[0] = SDRAM_CS0;
		DRAM_CS_Order[1] = SDRAM_CS1;
#ifdef MV_INCLUDE_SDRAM_CS2
		DRAM_CS_Order[2] = SDRAM_CS2;
		DRAM_CS_Order[3] = SDRAM_CS3;
#endif	
		return SDRAM_CS0;
	}
	return DRAM_CS_Order[0];
}
/*******************************************************************************
* mvDramIfGetCSorder - 
* 
*
* DESCRIPTION:
*       This function return the fisrt CS on address 0
*
* INPUT:
*		CS number.
*
* OUTPUT:
*		CS order.
*
* RETURN:
*       SDRAM_CS0 or SDRAM_CS2
* 
* NOTE: mvDramIfGetFirstCS must be caled before this subroutine
*******************************************************************************/
MV_U32 mvDramIfGetCSorder(MV_U32 csOrder )
{
	return DRAM_CS_Order[csOrder];
}

