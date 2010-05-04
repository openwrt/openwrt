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
#include "ddr1_2/mvDramIf.h"
#include "ctrlEnv/sys/mvCpuIf.h"



#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* DRAM bank presence encoding */
#define BANK_PRESENT_CS0				0x1
#define BANK_PRESENT_CS0_CS1			0x3
#define BANK_PRESENT_CS0_CS2			0x5
#define BANK_PRESENT_CS0_CS1_CS2		0x7
#define BANK_PRESENT_CS0_CS2_CS3		0xd
#define BANK_PRESENT_CS0_CS2_CS3_CS4	0xf

/* locals   */
static MV_BOOL sdramIfWinOverlap(MV_TARGET target, MV_ADDR_WIN *pAddrWin);
#if defined(MV_INC_BOARD_DDIM)
static void sdramDDr2OdtConfig(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 dunitCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas);
static MV_U32 sdramModeRegCalc(MV_U32 minCas);
static MV_U32 sdramExtModeRegCalc(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 sdramAddrCtrlRegCalc(MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk);
static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk, 
						 MV_U32 forcedCl);
static MV_U32 sdramTimeCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, 
									  MV_U32 minCas, MV_U32 busClk);
static MV_U32 sdramTimeCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, 
									   MV_U32 busClk);

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
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_STATUS mvDramIfDetect(MV_U32 forcedCl)
{
	MV_U32 retVal = MV_OK;	/* return value */
	MV_DRAM_BANK_INFO bankInfo[MV_DRAM_MAX_CS];
	MV_U32  busClk, size, base = 0, i, temp, deviceW, dimmW;
	MV_U8	minCas;
	MV_DRAM_DEC_WIN dramDecWin;

	dramDecWin.addrWin.baseHigh = 0;

	busClk = mvBoardSysClkGet();
	
	if (0 == busClk)
	{
		mvOsPrintf("Dram: ERR. Can't detect system clock! \n");
		return MV_ERROR;
	}
	
	/* Close DRAM banks except bank 0 (in case code is excecuting from it...) */
#if defined(MV_INCLUDE_SDRAM_CS1)
	for(i= SDRAM_CS1; i < MV_DRAM_MAX_CS; i++)
		mvCpuIfTargetWinEnable(i, MV_FALSE);
#endif

	/* we will use bank 0 as the representative of the all the DRAM banks,  */
	/* since bank 0 must exist.                                             */	
	for(i = 0; i < MV_DRAM_MAX_CS; i++)
	{ 
		/* if Bank exist */
		if(MV_OK == mvDramBankInfoGet(i, &bankInfo[i]))
		{
			/* check it isn't SDRAM */
			if(bankInfo[i].memoryType == MEM_TYPE_SDRAM)
			{
				mvOsPrintf("Dram: ERR. SDRAM type not supported !!!\n");
				return MV_ERROR;
			}
			/* All banks must support registry in order to activate it */
			if(bankInfo[i].registeredAddrAndControlInputs != 
			   bankInfo[0].registeredAddrAndControlInputs)
			{
				mvOsPrintf("Dram: ERR. different Registered settings !!!\n");
				return MV_ERROR;
			}

			/* Init the CPU window decode */
			/* Note that the size in Bank info is in MB units 			*/
			/* Note that the Dimm width might be different then the device DRAM width */
			temp = MV_REG_READ(SDRAM_CONFIG_REG);
			
			deviceW = ((temp & SDRAM_DWIDTH_MASK) == SDRAM_DWIDTH_16BIT )? 16 : 32;
			dimmW = bankInfo[0].dataWidth - (bankInfo[0].dataWidth % 16);
			size = ((bankInfo[i].size << 20) / (dimmW/deviceW)); 

			/* We can not change DRAM window settings while excecuting  	*/
			/* code from it. That is why we skip the DRAM CS[0], saving     */
			/* it to the ROM configuration routine	*/
			if(i == SDRAM_CS0)
			{
				MV_U32 sizeToReg;
				
				/* Translate the given window size to register format */
				sizeToReg = ctrlSizeToReg(size, SCSR_SIZE_ALIGNMENT);

				/* Size parameter validity check. */
				if (-1 == sizeToReg)
				{
					mvOsPrintf("mvCtrlAddrDecToReg: ERR. Win %d size invalid.\n"
							   ,i);
					return MV_BAD_PARAM;
				}
                
				/* Size is located at upper 16 bits */
				sizeToReg <<= SCSR_SIZE_OFFS;

				/* enable it */
				sizeToReg |= SCSR_WIN_EN;

				MV_REG_WRITE(DRAM_BUF_REG0, sizeToReg);
			}
			else
			{
				dramDecWin.addrWin.baseLow = base;
				dramDecWin.addrWin.size = size;
				dramDecWin.enable = MV_TRUE;
				
				if (MV_OK != mvDramIfWinSet(SDRAM_CS0 + i, &dramDecWin))
				{
					mvOsPrintf("Dram: ERR. Fail to set bank %d!!!\n", 
							   SDRAM_CS0 + i);
					return MV_ERROR;
				}
			}
			
			base += size;

			/* update the suportedCasLatencies mask */
			bankInfo[0].suportedCasLatencies &= bankInfo[i].suportedCasLatencies;

		}
		else
		{
			if( i == 0 ) /* bank 0 doesn't exist */
			{
				mvOsPrintf("Dram: ERR. Fail to detect bank 0 !!!\n");
				return MV_ERROR;
			}
			else
			{
				DB(mvOsPrintf("Dram: Could not find bank %d\n", i));
				bankInfo[i].size = 0;     /* Mark this bank as non exist */
			}
		}
	}

	/* calculate minimum CAS */
	minCas = minCasCalc(&bankInfo[0], busClk, forcedCl);
	if (0 == minCas) 
	{
		mvOsOutput("Dram: Warn: Could not find CAS compatible to SysClk %dMhz\n",
				   (busClk / 1000000));

		if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
		{
			minCas = DDR2_CL_4; /* Continue with this CAS */
			mvOsPrintf("Set default CAS latency 4\n");
		}
		else
		{
			minCas = DDR1_CL_3; /* Continue with this CAS */
			mvOsPrintf("Set default CAS latency 3\n");
		}
	}

	/* calc SDRAM_CONFIG_REG  and save it to temp register */
	temp = sdramConfigRegCalc(&bankInfo[0], busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramConfigRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG1, temp);

	/* calc SDRAM_MODE_REG  and save it to temp register */ 
	temp = sdramModeRegCalc(minCas);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramModeRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG2, temp);

	/* calc SDRAM_EXTENDED_MODE_REG  and save it to temp register */ 
	temp = sdramExtModeRegCalc(&bankInfo[0]);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramModeRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG10, temp);

	/* calc D_UNIT_CONTROL_LOW  and save it to temp register */
	temp = dunitCtrlLowRegCalc(&bankInfo[0], minCas); 
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. dunitCtrlLowRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG3, temp); 

	/* calc SDRAM_ADDR_CTRL_REG  and save it to temp register */
	temp = sdramAddrCtrlRegCalc(&bankInfo[0]);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramAddrCtrlRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG4, temp);

	/* calc SDRAM_TIMING_CTRL_LOW_REG  and save it to temp register */
	temp = sdramTimeCtrlLowRegCalc(&bankInfo[0], minCas, busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimeCtrlLowRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG5, temp);

	/* calc SDRAM_TIMING_CTRL_HIGH_REG  and save it to temp register */
	temp = sdramTimeCtrlHighRegCalc(&bankInfo[0], busClk);
	if(-1 == temp)
	{
		mvOsPrintf("Dram: ERR. sdramTimeCtrlHighRegCalc failed !!!\n");
		return MV_ERROR;
	}
	MV_REG_WRITE(DRAM_BUF_REG6, temp);

	/* Config DDR2 On Die Termination (ODT) registers */
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
		sdramDDr2OdtConfig(bankInfo);
	}
	
	/* Note that DDR SDRAM Address/Control and Data pad calibration     */
	/* settings is done in mvSdramIfConfig.s                            */

	return retVal;
}

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
*
* OUTPUT:
*       None
*
* RETURN:
*       The minimum CAS Latency. The function returns 0 if max CAS latency
*		supported by banks is incompatible with system bus clock frequancy.
*
*******************************************************************************/
static MV_U32 minCasCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk, 
						 MV_U32 forcedCl)
{
	MV_U32 count = 1, j;
	MV_U32 busClkPs = 1000000000 / (busClk / 1000);  /* in ps units */
	MV_U32 startBit, stopBit;
	
	/*     DDR 1:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  |  4   | 3.5  |   3  | 2.5  |  2   | 1.5  |   1  * 
			*********************************************************/
	
	/*     DDR 2:
			*******-******-******-******-******-******-******-******* 
			* bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
			*******-******-******-******-******-******-******-******* 
	CAS	=	* TBD  | TBD  |  5   |  4   |  3   |  2   | TBD  | TBD  * 
			*********************************************************/
	
	
	/* If we are asked to use the forced CAL */
	if (forcedCl)
	{
		mvOsPrintf("DRAM: Using forced CL %d.%d\n", (forcedCl / 10), 
													(forcedCl % 10));
	
		if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
		{
			if (forcedCl == 30)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 40)
				pBankInfo->suportedCasLatencies = 0x10;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 4\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x10;
			}
		}
		else
		{
			if (forcedCl == 15)
				pBankInfo->suportedCasLatencies = 0x02;
			else if (forcedCl == 20)
				pBankInfo->suportedCasLatencies = 0x04;
			else if (forcedCl == 25)
				pBankInfo->suportedCasLatencies = 0x08;
			else if (forcedCl == 30)
				pBankInfo->suportedCasLatencies = 0x10;
			else if (forcedCl == 40)
				pBankInfo->suportedCasLatencies = 0x40;
			else
			{
				mvOsPrintf("Forced CL %d.%d not supported. Set default CL 3\n", 
						   (forcedCl / 10), (forcedCl % 10));
				pBankInfo->suportedCasLatencies = 0x10;
			}
		}
	
		return pBankInfo->suportedCasLatencies;        
	}   
	
	/* go over the supported cas mask from Max Cas down and check if the 	*/
	/* SysClk stands in its time requirments.								*/
	
	
	DB(mvOsPrintf("Dram: minCasCalc supported mask = %x busClkPs = %x \n",
								pBankInfo->suportedCasLatencies,busClkPs ));
	for(j = 7; j > 0; j--)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			/* Reset the bits for CL incompatible for the sysClk            */
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
	
	/* SDRAM DDR1 controller supports CL 1.5 to 3.5 */
	/* SDRAM DDR2 controller supports CL 3 to 5     */
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
		startBit = 3;   /* DDR2 support CL start with CL3 (bit 3) */
		stopBit  = 5;   /* DDR2 support CL stops with CL5 (bit 5) */
	}
	else
	{
		startBit = 1;   /* DDR1 support CL start with CL1.5 (bit 3) */
		stopBit  = 4;   /* DDR1 support CL stops with CL3 (bit 4)   */
	}
	
	for(j = startBit; j <= stopBit ; j++)
	{
		if((pBankInfo->suportedCasLatencies >> j) & BIT0 )
		{
			DB(mvOsPrintf("Dram: minCasCalc choose CAS %x \n",(BIT0 << j)));
			return (BIT0 << j);
		}
	}
	
	return 0; 
}

/*******************************************************************************
* sdramConfigRegCalc - Calculate sdram config register
*
* DESCRIPTION: Calculate sdram config register optimized value based
*			on the bank info parameters.
*
* INPUT:
*	pBankInfo - sdram bank parameters
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram config reg value.
*
*******************************************************************************/
static MV_U32 sdramConfigRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 busClk)
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
		mvOsPrintf("DRAM Attribute: Registered address and control inputs.\n");
	}
	
	/* set DDR SDRAM devices configuration */
	sdramConfig &= ~SDRAM_DCFG_MASK;    /* Clear Dcfg field */
	
	switch (pBankInfo->sdramWidth)
	{
		case 8:  /* memory is x8 */
			sdramConfig |= SDRAM_DCFG_X8_DEV;
			DB(mvOsPrintf("Dram: sdramConfigRegCalc SDRAM device width x8\n"));
			break;
		case 16:
			sdramConfig |= SDRAM_DCFG_X16_DEV;
			DB(mvOsPrintf("Dram: sdramConfigRegCalc SDRAM device width x16\n"));
			break;
		default: /* memory width unsupported */
			mvOsPrintf("Dram: ERR. DRAM chip width is unknown!\n");
			return -1;
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
	
	mvOsPrintf("DRAM CAS Latency ");
	
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{            	
		switch (minCas)
		{
			case DDR2_CL_3: 
				sdramMode |= SDRAM_DDR2_CL_3;
				mvOsPrintf("3.\n");
				break;
			case DDR2_CL_4: 
				sdramMode |= SDRAM_DDR2_CL_4;
				mvOsPrintf("4.\n");
				break;
			case DDR2_CL_5: 
				sdramMode |= SDRAM_DDR2_CL_5;
				mvOsPrintf("5.\n");
				break;
			default:
				mvOsPrintf("\nsdramModeRegCalc ERROR: Max. CL out of range\n");
				return -1;
		}
	sdramMode |= DDR2_MODE_REG_DV;
	}
	else	/* DDR1 */
	{
		switch (minCas)
		{			
			case DDR1_CL_1_5: 
				sdramMode |= SDRAM_DDR1_CL_1_5;
				mvOsPrintf("1.5\n");
				break;
			case DDR1_CL_2: 
				sdramMode |= SDRAM_DDR1_CL_2;
				mvOsPrintf("2\n");
				break;            
			case DDR1_CL_2_5: 
				sdramMode |= SDRAM_DDR1_CL_2_5;
				mvOsPrintf("2.5\n");
				break;
			case DDR1_CL_3: 
				sdramMode |= SDRAM_DDR1_CL_3;
				mvOsPrintf("3\n");
				break;
			case DDR1_CL_4: 
				sdramMode |= SDRAM_DDR1_CL_4;
				mvOsPrintf("4\n");
				break;
			default:
				mvOsPrintf("\nsdramModeRegCalc ERROR: Max. CL out of range\n");
				return -1;	
		}
		sdramMode |= DDR1_MODE_REG_DV;		
	}
	
	DB(mvOsPrintf("nsdramModeRegCalc register 0x%x\n", sdramMode ));

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
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram Extended mode reg value.
*
*******************************************************************************/
static MV_U32 sdramExtModeRegCalc(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 populateBanks = 0;
	int bankNum;
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
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
				return DDR_SDRAM_EXT_MODE_CS0_DV;
		
			case(BANK_PRESENT_CS0_CS1):
				return DDR_SDRAM_EXT_MODE_CS0_DV;
		
			case(BANK_PRESENT_CS0_CS2):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS1_CS2):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS2_CS3):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			case(BANK_PRESENT_CS0_CS2_CS3_CS4):
				return DDR_SDRAM_EXT_MODE_CS0_CS2_DV;
		
			default:
				mvOsPrintf("sdramExtModeRegCalc: Invalid DRAM bank presence\n");
				return -1;
		} 
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
static MV_U32 dunitCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, MV_U32 minCas)
{
	MV_U32 dunitCtrlLow;
	
	dunitCtrlLow = MV_REG_READ(SDRAM_DUNIT_CTRL_REG);
	
	/* Clear StBurstDel field */
	dunitCtrlLow &= ~SDRAM_ST_BURST_DEL_MASK;
	
#ifdef MV_88W8660
	/* Clear address/control output timing field */
	dunitCtrlLow &= ~SDRAM_CTRL_POS_RISE;
#endif /* MV_88W8660 */

	DB(mvOsPrintf("Dram: dunitCtrlLowRegCalc\n"));
	
	/* For proper sample of read data set the Dunit Control register's      */
	/* stBurstDel bits [27:24]                                              */
			/********-********-********-********-********-*********
			* CL=1.5 |  CL=2  | CL=2.5 |  CL=3  |  CL=4  |  CL=5  *
			*********-********-********-********-********-*********
Not Reg.	*  0011  |  0011  |  0100  |  0100  |  0101  |  TBD   *
			*********-********-********-********-********-*********
Registered	*  0100  |  0100  |  0101  |  0101  |  0110  |  TBD   *
			*********-********-********-********-********-*********/
    
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
		switch (minCas)
		{
			case DDR2_CL_3: 
					/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE) 		
					dunitCtrlLow |= 0x5 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x4 << SDRAM_ST_BURST_DEL_OFFS;
				break;
			case DDR2_CL_4: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE) 		
					dunitCtrlLow |= 0x6 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x5 << SDRAM_ST_BURST_DEL_OFFS;	
				break;
			default:
				mvOsPrintf("Dram: dunitCtrlLowRegCalc Max. CL out of range %d\n", 
						   minCas);
				return -1;
		}
	}
	else    /* DDR1 */
	{
		switch (minCas)
		{
			case DDR1_CL_1_5: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
					dunitCtrlLow |= 0x4 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x3 << SDRAM_ST_BURST_DEL_OFFS;
				break;
			case DDR1_CL_2: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
					dunitCtrlLow |= 0x4 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x3 << SDRAM_ST_BURST_DEL_OFFS;
				break;
			case DDR1_CL_2_5: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
					dunitCtrlLow |= 0x5 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x4 << SDRAM_ST_BURST_DEL_OFFS;	
				break;
			case DDR1_CL_3: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
					dunitCtrlLow |= 0x5 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x4 << SDRAM_ST_BURST_DEL_OFFS;	
				break;
			case DDR1_CL_4: 
				/* registerd DDR SDRAM? */
				if (pBankInfo->registeredAddrAndControlInputs == MV_TRUE)
					dunitCtrlLow |= 0x6 << SDRAM_ST_BURST_DEL_OFFS;
				else
					dunitCtrlLow |= 0x5 << SDRAM_ST_BURST_DEL_OFFS;	
				break;
			default:
				mvOsPrintf("Dram: dunitCtrlLowRegCalc Max. CL out of range %d\n", 
						   minCas);
				return -1;
	}
	
	}
	DB(mvOsPrintf("Dram: Reg dunit control low = %x\n", dunitCtrlLow ));

	return dunitCtrlLow;
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
static MV_U32 sdramAddrCtrlRegCalc(MV_DRAM_BANK_INFO *pBankInfo)
{
	MV_U32 addrCtrl = 0;
	
	/* Set Address Control register static configuration bits */
	addrCtrl = MV_REG_READ(SDRAM_ADDR_CTRL_REG);
	
	/* Set address control default value */
	addrCtrl |= SDRAM_ADDR_CTRL_DV;  
	
	/* Clear DSize field */
	addrCtrl &= ~SDRAM_DSIZE_MASK;
	
	/* Note that density is in MB units */
	switch (pBankInfo->deviceDensity) 
	{
		case 128:                 /* 128 Mbit */
			DB(mvOsPrintf("DRAM Device Density 128Mbit\n"));
			addrCtrl |= SDRAM_DSIZE_128Mb;
			break;
		case 256:                 /* 256 Mbit */
			DB(mvOsPrintf("DRAM Device Density 256Mbit\n"));
			addrCtrl |= SDRAM_DSIZE_256Mb;
			break;
		case 512:                /* 512 Mbit */
			DB(mvOsPrintf("DRAM Device Density 512Mbit\n"));
			addrCtrl |= SDRAM_DSIZE_512Mb;
			break;
		default:
			mvOsPrintf("Dram: sdramAddrCtrl unsupported RAM-Device size %d\n",
                       pBankInfo->deviceDensity);
			return -1;
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
*       busClk    - Bus clock
*
* OUTPUT:
*       None
*
* RETURN:
*       sdram timinf control low reg value.
*
*******************************************************************************/
static MV_U32 sdramTimeCtrlLowRegCalc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                MV_U32 minCas, MV_U32 busClk)
{
	MV_U32 tRp  = 0;
	MV_U32 tRrd = 0;
	MV_U32 tRcd = 0;
	MV_U32 tRas = 0;
	MV_U32 tWr  = 0;
	MV_U32 tWtr = 0;
	MV_U32 tRtp = 0;
	
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
	/* Micron work around for 133MHz */
	if (busClk == 133)
		tRp += 1;
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
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
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
	}
	else
	{    
		tWr  = ((busClk*SDRAM_TWR) / 1000) + (((busClk*SDRAM_TWR) % 1000)?1:0);
		
		if ((200 == busClk) || ((100 == busClk) && (DDR1_CL_1_5 == minCas)))
		{
			tWtr = 2;
		}
		else
		{
			tWtr = 1;
		}
		
		tRtp = 2; /* Must be set to 0x1 (two cycles) when using DDR1 */
	}
	
	DB(mvOsPrintf("tWtr = %d\n", tWtr));
	
	/* Note: value of 0 in register means one cycle, 1 means two and so on  */
	return (((tRp  - 1) << SDRAM_TRP_OFFS)	|
			((tRrd - 1) << SDRAM_TRRD_OFFS)	|
			((tRcd - 1) << SDRAM_TRCD_OFFS)	|
			((tRas - 1) << SDRAM_TRAS_OFFS)	|
			((tWr  - 1) << SDRAM_TWR_OFFS)	|
			((tWtr - 1) << SDRAM_TWTR_OFFS)	|
			((tRtp - 1) << SDRAM_TRTP_OFFS));
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
*       sdram timinf control high reg value.
*
*******************************************************************************/
static MV_U32 sdramTimeCtrlHighRegCalc(MV_DRAM_BANK_INFO *pBankInfo, 
                                                                MV_U32 busClk)
{
	MV_U32 tRfc;
	MV_U32 timeNs = 0;
	int bankNum;
	MV_U32 sdramTw2wCyc = 0;
	
	busClk = busClk / 1000000;    /* In MHz */
	
	/* tRfc is different for DDR1 and DDR2. */
	if (MV_REG_READ(SDRAM_CONFIG_REG) & SDRAM_DTYPE_DDR2)
	{
		MV_U32 bankNum;
	
		/* Scan all DRAM banks to find maximum timing values */
		for (bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
			timeNs = MV_MAX(timeNs,  pBankInfo[bankNum].minRefreshToActiveCmd);
	}
	else
	{
		if (pBankInfo[0].deviceDensity == _1G)
		{
			timeNs = SDRAM_TRFC_1G;
		}
		else
		{
			if (200 == busClk)
			{
				timeNs = SDRAM_TRFC_64_512M_AT_200MHZ;
			}
			else
			{
				timeNs = SDRAM_TRFC_64_512M;
			}
		}
	}
	
	tRfc = ((busClk * timeNs)  / 1000) + (((busClk * timeNs)  % 1000) ? 1 : 0);
	
	DB(mvOsPrintf("Dram  Timing High: tRfc = %d\n", tRfc));

	
	/* Represent the populate banks in binary form */
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		if (0 != pBankInfo[bankNum].size)
			sdramTw2wCyc++;
	}

	/* If we have more the 1 bank then we need the TW2W in 1 for ODT switch */	
	if (sdramTw2wCyc > 1)
		sdramTw2wCyc = 1;
	else
		sdramTw2wCyc = 0;

	/* Note: value of 0 in register means one cycle, 1 means two and so on  */
	return ((((tRfc - 1) & SDRAM_TRFC_MASK)	<< SDRAM_TRFC_OFFS)		|
			((SDRAM_TR2R_CYC - 1)			<< SDRAM_TR2R_OFFS)		|
			((SDRAM_TR2WW2R_CYC - 1)		<< SDRAM_TR2W_W2R_OFFS)	|
			(((tRfc - 1) >> 4)				<< SDRAM_TRFC_EXT_OFFS)	|
			(sdramTw2wCyc					<< SDRAM_TW2W_OFFS));
	
}

/*******************************************************************************
* sdramDDr2OdtConfig - Set DRAM DDR2 On Die Termination registers.
*
* DESCRIPTION: 
*       This function config DDR2 On Die Termination (ODT) registers.
*	ODT configuration is done according to DIMM presence:
*	
*       Presence	  Ctrl Low    Ctrl High  Dunit Ctrl   Ext Mode  
*	CS0	         0x84210000  0x00000000  0x0000780F  0x00000440 
*	CS0+CS1          0x84210000  0x00000000  0x0000780F  0x00000440 
*	CS0+CS2	    	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS1+CS2	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS2+CS3	 0x030C030C  0x00000000  0x0000740F  0x00000404 
*	CS0+CS1+CS2+CS3  0x030C030C  0x00000000  0x0000740F  0x00000404 
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
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_DV;
			break;
		case(BANK_PRESENT_CS0_CS1):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_DV;
			break;
		case(BANK_PRESENT_CS0_CS2):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS2_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS2_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS2_DV;
			break;
		case(BANK_PRESENT_CS0_CS1_CS2):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS2_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS2_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS2_DV;
			break;
		case(BANK_PRESENT_CS0_CS2_CS3):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS2_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS2_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS2_DV;
			break;
		case(BANK_PRESENT_CS0_CS2_CS3_CS4):
			odtCtrlLow   = DDR2_ODT_CTRL_LOW_CS0_CS2_DV;
			odtCtrlHigh  = DDR2_ODT_CTRL_HIGH_CS0_CS2_DV;
			dunitOdtCtrl = DDR2_DUNIT_ODT_CTRL_CS0_CS2_DV;
			break;
		default:
			mvOsPrintf("sdramDDr2OdtConfig: Invalid DRAM bank presence\n");
			return;
	} 
	MV_REG_WRITE(DRAM_BUF_REG7, odtCtrlLow);
	MV_REG_WRITE(DRAM_BUF_REG8, odtCtrlHigh);
	MV_REG_WRITE(DRAM_BUF_REG9, dunitOdtCtrl);
	return;
}
#endif /* defined(MV_INC_BOARD_DDIM) */

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
	baseReg = MV_REG_READ(SDRAM_BASE_ADDR_REG(target));

	/* read size register */
	sizeReg = MV_REG_READ(SDRAM_SIZE_REG(target));

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
	MV_REG_WRITE(SDRAM_BASE_ADDR_REG(target), baseReg);

	/* Write to address decode Size Register                        	*/
	MV_REG_WRITE(SDRAM_SIZE_REG(target), sizeReg);
	
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
	sizeReg = MV_REG_READ(SDRAM_SIZE_REG(target));
	baseReg = MV_REG_READ(SDRAM_BASE_ADDR_REG(target));

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
MV_STATUS mvDramIfWinEnable(MV_TARGET target,MV_BOOL enable)
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
			MV_REG_BIT_SET(SDRAM_SIZE_REG(target), SCSR_WIN_EN);
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
		MV_REG_BIT_RESET(SDRAM_SIZE_REG(target), SCSR_WIN_EN);
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
MV_32 mvDramIfBankSizeGet(MV_U32 bankNum)
{
    MV_DRAM_DEC_WIN 	addrDecWin;
	
	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(bankNum))
	{
		mvOsPrintf("mvDramIfBankBaseGet: bankNum %d is invalid\n", bankNum);
		return -1;
	}
	/* Get window parameters 	*/
	if (MV_OK != mvDramIfWinGet(bankNum, &addrDecWin))
	{
		mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
		return -1;
	}
	
	if (MV_TRUE == addrDecWin.enable)
	{
		return addrDecWin.addrWin.size;
	}
	else
	{
		return 0;
	}
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
MV_32 mvDramIfSizeGet(MV_VOID)
{
	MV_U32 totalSize = 0, bankSize = 0, bankNum;
	
	for(bankNum = 0; bankNum < MV_DRAM_MAX_CS; bankNum++)
	{
		bankSize = mvDramIfBankSizeGet(bankNum);

		if (-1 == bankSize)
		{
			mvOsPrintf("Dram: mvDramIfSizeGet error with bank %d \n",bankNum);
			return -1;
		}
		else
		{
			totalSize += bankSize;
		}
	}
	
	DB(mvOsPrintf("Dram: Total DRAM size is 0x%x \n",totalSize));
	
	return totalSize;
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
MV_32 mvDramIfBankBaseGet(MV_U32 bankNum)
{
    MV_DRAM_DEC_WIN 	addrDecWin;
	
	/* Check parameters */
	if (!MV_TARGET_IS_DRAM(bankNum))
	{
		mvOsPrintf("mvDramIfBankBaseGet: bankNum %d is invalid\n", bankNum);
		return -1;
	}
	/* Get window parameters 	*/
	if (MV_OK != mvDramIfWinGet(bankNum, &addrDecWin))
	{
		mvOsPrintf("sdramIfWinOverlap: ERR. TargetWinGet failed\n");
		return -1;
	}
	
	if (MV_TRUE == addrDecWin.enable)
	{
		return addrDecWin.addrWin.baseLow;
	}
	else
	{
		return -1;
	}
}


