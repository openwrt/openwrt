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

#include "ddr1_2/mvDram.h"
#include "boardEnv/mvBoardEnvLib.h"

#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

static MV_VOID cpyDimm2BankInfo(MV_DIMM_INFO *pDimmInfo, 
                                            MV_DRAM_BANK_INFO *pBankInfo);
static MV_U32  cas2ps(MV_U8 spd_byte);
/*******************************************************************************
* mvDramBankGet - Get the DRAM bank paramters.
*
* DESCRIPTION:
*       This function retrieves DRAM bank parameters as described in 
*       DRAM_BANK_INFO struct to the controller DRAM unit. In case the board 
*       has its DRAM on DIMMs it will use its EEPROM to extract SPD data
*       from it. Otherwise, if the DRAM is soldered on board, the function 
*       should insert its bank information into MV_DRAM_BANK_INFO struct.
*
* INPUT:
*       bankNum  - Board DRAM bank number.
*
* OUTPUT:
*       pBankInfo  - DRAM bank information struct.
*
* RETURN:
*       MV_FAIL - Bank parameters could not be read.
*
*******************************************************************************/
MV_STATUS mvDramBankInfoGet(MV_U32 bankNum, MV_DRAM_BANK_INFO *pBankInfo)
{
    MV_DIMM_INFO dimmInfo;

    DB(mvOsPrintf("Dram: mvDramBankInfoGet bank %d\n", bankNum)); 
    /* zero pBankInfo structure */
    memset(pBankInfo, 0, sizeof(*pBankInfo));

    if((NULL == pBankInfo) || (bankNum >= MV_DRAM_MAX_CS ))
    {
        DB(mvOsPrintf("Dram: mvDramBankInfoGet bad params \n")); 
        return MV_BAD_PARAM;
    }
    if( MV_OK != dimmSpdGet((MV_U32)(bankNum/2), &dimmInfo))
    {
    DB(mvOsPrintf("Dram: ERR dimmSpdGet failed to get dimm info \n"));
    return MV_FAIL;
    }
    if((dimmInfo.numOfModuleBanks == 1) && ((bankNum % 2) == 1))
    {
    DB(mvOsPrintf("Dram: ERR dimmSpdGet. Can't find DIMM bank 2 \n"));
    return MV_FAIL;
    }

    /* convert Dimm info to Bank info */
    cpyDimm2BankInfo(&dimmInfo, pBankInfo);
    
    return MV_OK;
}

/*******************************************************************************
* cpyDimm2BankInfo - Convert a Dimm info struct into a bank info struct.
*
* DESCRIPTION:
*       Convert a Dimm info struct into a bank info struct.
*
* INPUT:
*       pDimmInfo - DIMM information structure.
*
* OUTPUT:
*       pBankInfo  - DRAM bank information struct.
*
* RETURN:
*       None.
*
*******************************************************************************/
static MV_VOID cpyDimm2BankInfo(MV_DIMM_INFO *pDimmInfo, 
                                                MV_DRAM_BANK_INFO *pBankInfo)
{
    pBankInfo->memoryType = pDimmInfo->memoryType;        

    /* DIMM dimensions */
    pBankInfo->numOfRowAddr = pDimmInfo->numOfRowAddr;
    pBankInfo->numOfColAddr = pDimmInfo->numOfColAddr;
    pBankInfo->dataWidth = pDimmInfo->dataWidth;
    pBankInfo->errorCheckType = pDimmInfo->errorCheckType;             
    pBankInfo->sdramWidth = pDimmInfo->sdramWidth;
    pBankInfo->errorCheckDataWidth = pDimmInfo->errorCheckDataWidth;   
    pBankInfo->numOfBanksOnEachDevice = pDimmInfo->numOfBanksOnEachDevice;
    pBankInfo->suportedCasLatencies = pDimmInfo->suportedCasLatencies;
    pBankInfo->refreshInterval = pDimmInfo->refreshInterval;
 
    /* DIMM timing parameters */
    pBankInfo->minCycleTimeAtMaxCasLatPs = pDimmInfo->minCycleTimeAtMaxCasLatPs;
    pBankInfo->minCycleTimeAtMaxCasLatMinus1Ps = 
                                    pDimmInfo->minCycleTimeAtMaxCasLatMinus1Ps;
    pBankInfo->minCycleTimeAtMaxCasLatMinus2Ps = 
                                    pDimmInfo->minCycleTimeAtMaxCasLatMinus2Ps;

    pBankInfo->minRowPrechargeTime     = pDimmInfo->minRowPrechargeTime;     
    pBankInfo->minRowActiveToRowActive = pDimmInfo->minRowActiveToRowActive;
    pBankInfo->minRasToCasDelay        = pDimmInfo->minRasToCasDelay;       
    pBankInfo->minRasPulseWidth        = pDimmInfo->minRasPulseWidth;       
    pBankInfo->minWriteRecoveryTime    = pDimmInfo->minWriteRecoveryTime;
    pBankInfo->minWriteToReadCmdDelay  = pDimmInfo->minWriteToReadCmdDelay;
    pBankInfo->minReadToPrechCmdDelay  = pDimmInfo->minReadToPrechCmdDelay;
    pBankInfo->minRefreshToActiveCmd   = pDimmInfo->minRefreshToActiveCmd;
               
    /* Parameters calculated from the extracted DIMM information */
    pBankInfo->size = pDimmInfo->size/pDimmInfo->numOfModuleBanks;
    pBankInfo->deviceDensity = pDimmInfo->deviceDensity;              
    pBankInfo->numberOfDevices = pDimmInfo->numberOfDevices /
                                 pDimmInfo->numOfModuleBanks;
 
    /* DIMM attributes (MV_TRUE for yes) */

    if ((pDimmInfo->memoryType == MEM_TYPE_SDRAM) ||
        (pDimmInfo->memoryType == MEM_TYPE_DDR1)   )
    {   
        if (pDimmInfo->dimmAttributes & BIT1)
            pBankInfo->registeredAddrAndControlInputs = MV_TRUE;
        else
            pBankInfo->registeredAddrAndControlInputs = MV_FALSE;
    }
    else /* pDimmInfo->memoryType == MEM_TYPE_DDR2 */
    {
        if (pDimmInfo->dimmTypeInfo & (BIT0 | BIT4))
            pBankInfo->registeredAddrAndControlInputs = MV_TRUE;
        else
            pBankInfo->registeredAddrAndControlInputs = MV_FALSE;
    }

    return;
}

/*******************************************************************************
* dimmSpdCpy - Cpy SPD parameters from dimm 0 to dimm 1.
*
* DESCRIPTION:
*       Read the DIMM SPD parameters from dimm 0 into dimm 1 SPD.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if function could read DIMM parameters, MV_FALSE otherwise.
*
*******************************************************************************/
MV_STATUS dimmSpdCpy(MV_VOID)
{
    MV_U32 i;
    MV_U32 spdChecksum;
     
    MV_TWSI_SLAVE twsiSlave;
    MV_U8 data[SPD_SIZE];

    /* zero dimmInfo structure */
    memset(data, 0, SPD_SIZE);

    /* read the dimm eeprom */
    DB(mvOsPrintf("DRAM: Read Dimm eeprom\n"));
    twsiSlave.slaveAddr.address = MV_BOARD_DIMM0_I2C_ADDR;
    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.validOffset = MV_TRUE;
    twsiSlave.offset = 0;
    twsiSlave.moreThen256 = MV_FALSE;

    if( MV_OK != mvTwsiRead (MV_BOARD_DIMM_I2C_CHANNEL,
			&twsiSlave, data, SPD_SIZE) )
    {
        DB(mvOsPrintf("DRAM: ERR. no DIMM in dimmNum 0\n"));
        return MV_FAIL;
    }
    DB(puts("DRAM: Reading dimm info succeded.\n"));
    
    /* calculate SPD checksum */
    spdChecksum = 0;
    
    for(i = 0 ; i <= 62 ; i++)
    {
        spdChecksum += data[i];
    }
    
    if ((spdChecksum & 0xff) != data[63])
    {
        DB(mvOsPrintf("DRAM: Warning. Wrong SPD Checksum %2x, expValue=%2x\n",
                            (MV_U32)(spdChecksum & 0xff), data[63]));
    }
    else
    {
        DB(mvOsPrintf("DRAM: SPD Checksum ok!\n"));
    }

    /* copy the SPD content 1:1 into the DIMM 1 SPD */
    twsiSlave.slaveAddr.address = MV_BOARD_DIMM1_I2C_ADDR;
    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.validOffset = MV_TRUE;
    twsiSlave.offset = 0;
    twsiSlave.moreThen256 = MV_FALSE;

    for(i = 0 ; i < SPD_SIZE ; i++)
    {
	twsiSlave.offset = i;
	if( MV_OK != mvTwsiWrite (MV_BOARD_DIMM_I2C_CHANNEL,
	    			&twsiSlave, &data[i], 1) )
	{
	    mvOsPrintf("DRAM: ERR. no DIMM in dimmNum 1 byte %d \n",i);
	    return MV_FAIL;
	}
	mvOsDelay(5);
    }
    
    DB(puts("DRAM: Reading dimm info succeded.\n"));
    return MV_OK;
}

/*******************************************************************************
* dimmSpdGet - Get the SPD parameters.
*
* DESCRIPTION:
*       Read the DIMM SPD parameters into given struct parameter.
*
* INPUT:
*       dimmNum - DIMM number. See MV_BOARD_DIMM_NUM enumerator.
*
* OUTPUT:
*       pDimmInfo - DIMM information structure.
*
* RETURN:
*       MV_TRUE if function could read DIMM parameters, MV_FALSE otherwise.
*
*******************************************************************************/
MV_STATUS dimmSpdGet(MV_U32 dimmNum, MV_DIMM_INFO *pDimmInfo)
{
    MV_U32 i;
    MV_U32 density = 1;
    MV_U32 spdChecksum;
     
    MV_TWSI_SLAVE twsiSlave;
    MV_U8 data[SPD_SIZE];

    if((NULL == pDimmInfo)|| (dimmNum >= MAX_DIMM_NUM))
    {
        DB(mvOsPrintf("Dram: mvDramBankInfoGet bad params \n")); 
        return MV_BAD_PARAM;
    }

    /* zero dimmInfo structure */
    memset(data, 0, SPD_SIZE);

    /* read the dimm eeprom */
    DB(mvOsPrintf("DRAM: Read Dimm eeprom\n"));
    twsiSlave.slaveAddr.address = (dimmNum == 0) ?
                            MV_BOARD_DIMM0_I2C_ADDR : MV_BOARD_DIMM1_I2C_ADDR;
    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.validOffset = MV_TRUE;
    twsiSlave.offset = 0;
    twsiSlave.moreThen256 = MV_FALSE;

    if( MV_OK != mvTwsiRead (MV_BOARD_DIMM_I2C_CHANNEL,
			&twsiSlave, data, SPD_SIZE) )
    {
        DB(mvOsPrintf("DRAM: ERR. no DIMM in dimmNum %d \n", dimmNum));
        return MV_FAIL;
    }
    DB(puts("DRAM: Reading dimm info succeded.\n"));
    
    /* calculate SPD checksum */
    spdChecksum = 0;
    
        for(i = 0 ; i <= 62 ; i++)
        {
        spdChecksum += data[i];
    }
    
    if ((spdChecksum & 0xff) != data[63])
    {
        DB(mvOsPrintf("DRAM: Warning. Wrong SPD Checksum %2x, expValue=%2x\n",
                            (MV_U32)(spdChecksum & 0xff), data[63]));
    }
    else
    {
        DB(mvOsPrintf("DRAM: SPD Checksum ok!\n"));
    }

    /* copy the SPD content 1:1 into the dimmInfo structure*/
    for(i = 0 ; i < SPD_SIZE ; i++)
    {
        pDimmInfo->spdRawData[i] = data[i];
        DB(mvOsPrintf("SPD-EEPROM Byte %3d = %3x (%3d)\n",i, data[i], data[i]));
    }

    DB(mvOsPrintf("DRAM SPD Information:\n"));

    /* Memory type (DDR / SDRAM) */
    switch (data[DIMM_MEM_TYPE])
    {
        case (DIMM_MEM_TYPE_SDRAM):
            pDimmInfo->memoryType = MEM_TYPE_SDRAM;
            DB(mvOsPrintf("DRAM Memeory type SDRAM\n"));
            break;
        case (DIMM_MEM_TYPE_DDR1):
            pDimmInfo->memoryType = MEM_TYPE_DDR1;
            DB(mvOsPrintf("DRAM Memeory type DDR1\n"));
            break;
        case (DIMM_MEM_TYPE_DDR2):
            pDimmInfo->memoryType = MEM_TYPE_DDR2;
            DB(mvOsPrintf("DRAM Memeory type DDR2\n"));
            break;
        default:
            mvOsPrintf("ERROR: Undefined memory type!\n");
            return MV_ERROR;
    }

    
    /* Number Of Row Addresses */
    pDimmInfo->numOfRowAddr = data[DIMM_ROW_NUM];
    DB(mvOsPrintf("DRAM numOfRowAddr[3]         %d\n",pDimmInfo->numOfRowAddr));
        
    /* Number Of Column Addresses */
    pDimmInfo->numOfColAddr = data[DIMM_COL_NUM];
    DB(mvOsPrintf("DRAM numOfColAddr[4]         %d\n",pDimmInfo->numOfColAddr));
        
    /* Number Of Module Banks */
    pDimmInfo->numOfModuleBanks = data[DIMM_MODULE_BANK_NUM];
    DB(mvOsPrintf("DRAM numOfModuleBanks[5]     0x%x\n", 
                                                  pDimmInfo->numOfModuleBanks));
        
    /* Number of module banks encoded differently for DDR2 */
    if (pDimmInfo->memoryType == MEM_TYPE_DDR2)
        pDimmInfo->numOfModuleBanks = (pDimmInfo->numOfModuleBanks & 0x7)+1;

    /* Data Width */
    pDimmInfo->dataWidth = data[DIMM_DATA_WIDTH];
    DB(mvOsPrintf("DRAM dataWidth[6]            0x%x\n", pDimmInfo->dataWidth));
        
    /* Minimum Cycle Time At Max CasLatancy */
    pDimmInfo->minCycleTimeAtMaxCasLatPs = cas2ps(data[DIMM_MIN_CC_AT_MAX_CAS]);

    /* Error Check Type */
    pDimmInfo->errorCheckType = data[DIMM_ERR_CHECK_TYPE];
    DB(mvOsPrintf("DRAM errorCheckType[11]      0x%x\n", 
                                                    pDimmInfo->errorCheckType));

    /* Refresh Interval */
    pDimmInfo->refreshInterval = data[DIMM_REFRESH_INTERVAL];
    DB(mvOsPrintf("DRAM refreshInterval[12]     0x%x\n", 
                                                   pDimmInfo->refreshInterval));
    
    /* Sdram Width */
    pDimmInfo->sdramWidth = data[DIMM_SDRAM_WIDTH];
    DB(mvOsPrintf("DRAM sdramWidth[13]          0x%x\n",pDimmInfo->sdramWidth));
        
    /* Error Check Data Width */
    pDimmInfo->errorCheckDataWidth = data[DIMM_ERR_CHECK_DATA_WIDTH];
    DB(mvOsPrintf("DRAM errorCheckDataWidth[14] 0x%x\n", 
                                               pDimmInfo->errorCheckDataWidth));
    
    /* Burst Length Supported */
    /*     SDRAM/DDR1:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
    burst length =  * Page | TBD  | TBD  | TBD  |  8   |  4   |  2   |   1  * 
                    *********************************************************/ 
    /*     DDR2:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
    burst length =  * Page | TBD  | TBD  | TBD  |  8   |  4   | TBD  | TBD  * 
                    *********************************************************/ 

    pDimmInfo->burstLengthSupported = data[DIMM_BURST_LEN_SUP];
    DB(mvOsPrintf("DRAM burstLengthSupported[16] 0x%x\n", 
                                              pDimmInfo->burstLengthSupported));
    
    /* Number Of Banks On Each Device */
    pDimmInfo->numOfBanksOnEachDevice = data[DIMM_DEV_BANK_NUM];
    DB(mvOsPrintf("DRAM numOfBanksOnEachDevice[17] 0x%x\n", 
                                            pDimmInfo->numOfBanksOnEachDevice));
    
    /* Suported Cas Latencies */
                   
    /*      SDRAM:
            *******-******-******-******-******-******-******-******* 
            * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
            *******-******-******-******-******-******-******-******* 
    CAS =   * TBD  |  7   |  6   |  5   |  4   |  3   |   2  |   1  * 
            ********************************************************/ 

    /*     DDR 1:
            *******-******-******-******-******-******-******-******* 
            * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
            *******-******-******-******-******-******-******-******* 
    CAS =   * TBD  |  4   | 3.5  |   3  | 2.5  |  2   | 1.5  |   1  * 
            *********************************************************/

    /*     DDR 2:
            *******-******-******-******-******-******-******-******* 
            * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
            *******-******-******-******-******-******-******-******* 
    CAS =   * TBD  | TBD  |  5   |  4   |  3   |  2   | TBD  | TBD  * 
            *********************************************************/
    
    pDimmInfo->suportedCasLatencies = data[DIMM_SUP_CAL];
    DB(mvOsPrintf("DRAM suportedCasLatencies[18]    0x%x\n", 
                                              pDimmInfo->suportedCasLatencies));

    /* For DDR2 only, get the DIMM type information */
    if (pDimmInfo->memoryType == MEM_TYPE_DDR2)
    {   
        pDimmInfo->dimmTypeInfo = data[DIMM_DDR2_TYPE_INFORMATION];
        DB(mvOsPrintf("DRAM dimmTypeInfo[20] (DDR2) 0x%x\n", 
                                                      pDimmInfo->dimmTypeInfo));
    }

    /* SDRAM Modules Attributes */
    pDimmInfo->dimmAttributes = data[DIMM_BUF_ADDR_CONT_IN];
    DB(mvOsPrintf("DRAM dimmAttributes[21]          0x%x\n",    
                                                    pDimmInfo->dimmAttributes));
    
    /* Minimum Cycle Time At Max CasLatancy Minus 1*/
    pDimmInfo->minCycleTimeAtMaxCasLatMinus1Ps = 
                                    cas2ps(data[DIMM_MIN_CC_AT_MAX_CAS_MINUS1]);

    /* Minimum Cycle Time At Max CasLatancy Minus 2*/
    pDimmInfo->minCycleTimeAtMaxCasLatMinus2Ps = 
                                    cas2ps(data[DIMM_MIN_CC_AT_MAX_CAS_MINUS2]);

    pDimmInfo->minRowPrechargeTime = data[DIMM_MIN_ROW_PRECHARGE_TIME];
    DB(mvOsPrintf("DRAM minRowPrechargeTime[27]     0x%x\n", 
                                               pDimmInfo->minRowPrechargeTime));
    pDimmInfo->minRowActiveToRowActive = data[DIMM_MIN_ROW_ACTIVE_TO_ROW_ACTIVE];
    DB(mvOsPrintf("DRAM minRowActiveToRowActive[28] 0x%x\n", 
                                           pDimmInfo->minRowActiveToRowActive));
    pDimmInfo->minRasToCasDelay = data[DIMM_MIN_RAS_TO_CAS_DELAY];
    DB(mvOsPrintf("DRAM minRasToCasDelay[29]        0x%x\n", 
                                                  pDimmInfo->minRasToCasDelay));
    pDimmInfo->minRasPulseWidth = data[DIMM_MIN_RAS_PULSE_WIDTH];
    DB(mvOsPrintf("DRAM minRasPulseWidth[30]        0x%x\n", 
                                                  pDimmInfo->minRasPulseWidth));
        
    /* DIMM Bank Density */
    pDimmInfo->dimmBankDensity = data[DIMM_BANK_DENSITY];
    DB(mvOsPrintf("DRAM dimmBankDensity[31]         0x%x\n", 
                                                   pDimmInfo->dimmBankDensity));

    /* Only DDR2 includes Write Recovery Time field. Other SDRAM ignore     */
    pDimmInfo->minWriteRecoveryTime = data[DIMM_MIN_WRITE_RECOVERY_TIME];
    DB(mvOsPrintf("DRAM minWriteRecoveryTime[36]    0x%x\n", 
                                              pDimmInfo->minWriteRecoveryTime));
    
    /* Only DDR2 includes Internal Write To Read Command Delay field.       */
    pDimmInfo->minWriteToReadCmdDelay = data[DIMM_MIN_WRITE_TO_READ_CMD_DELAY];
    DB(mvOsPrintf("DRAM minWriteToReadCmdDelay[37]  0x%x\n", 
                                            pDimmInfo->minWriteToReadCmdDelay));
    
    /* Only DDR2 includes Internal Read To Precharge Command Delay field.   */
    pDimmInfo->minReadToPrechCmdDelay = data[DIMM_MIN_READ_TO_PRECH_CMD_DELAY];
    DB(mvOsPrintf("DRAM minReadToPrechCmdDelay[38]  0x%x\n",    
                                            pDimmInfo->minReadToPrechCmdDelay));
    
    /* Only DDR2 includes Minimum Refresh to Activate/Refresh Command field */
    pDimmInfo->minRefreshToActiveCmd = data[DIMM_MIN_REFRESH_TO_ACTIVATE_CMD];
    DB(mvOsPrintf("DRAM minRefreshToActiveCmd[42]   0x%x\n", 
                                             pDimmInfo->minRefreshToActiveCmd));
                 
    /* calculating the sdram density. Representing device density from      */
    /* bit 20 to allow representation of 4GB and above.                     */
    /* For example, if density is 512Mbit 0x20000000, will be represent in  */
    /* deviceDensity by 0x20000000 >> 16 --> 0x00000200. Another example    */
    /* is density 8GB 0x200000000 >> 16 --> 0x00002000.                     */
    density = (1 << ((pDimmInfo->numOfRowAddr + pDimmInfo->numOfColAddr) - 20));
    pDimmInfo->deviceDensity = density * 
                                pDimmInfo->numOfBanksOnEachDevice * 
                                pDimmInfo->sdramWidth;
    DB(mvOsPrintf("DRAM deviceDensity           %d\n",pDimmInfo->deviceDensity));
    
    /* Number of devices includeing Error correction */
    pDimmInfo->numberOfDevices = (pDimmInfo->dataWidth/pDimmInfo->sdramWidth) * 
                                  pDimmInfo->numOfModuleBanks;
    DB(mvOsPrintf("DRAM numberOfDevices         %d\n",  
                                                   pDimmInfo->numberOfDevices));

    pDimmInfo->size = 0; 

    /* Note that pDimmInfo->size is in MB units */
    if (pDimmInfo->memoryType == MEM_TYPE_SDRAM)
    {
        if (pDimmInfo->dimmBankDensity & BIT0)
            pDimmInfo->size += 1024;                /* Equal to 1GB     */
        else if (pDimmInfo->dimmBankDensity & BIT1)
            pDimmInfo->size += 8;                   /* Equal to 8MB     */
        else if (pDimmInfo->dimmBankDensity & BIT2)
            pDimmInfo->size += 16;                  /* Equal to 16MB    */
        else if (pDimmInfo->dimmBankDensity & BIT3)
            pDimmInfo->size += 32;                  /* Equal to 32MB    */
        else if (pDimmInfo->dimmBankDensity & BIT4)
            pDimmInfo->size += 64;                  /* Equal to 64MB    */
        else if (pDimmInfo->dimmBankDensity & BIT5)
            pDimmInfo->size += 128;                 /* Equal to 128MB   */
        else if (pDimmInfo->dimmBankDensity & BIT6) 
            pDimmInfo->size += 256;                 /* Equal to 256MB   */
        else if (pDimmInfo->dimmBankDensity & BIT7) 
            pDimmInfo->size += 512;                 /* Equal to 512MB   */
    }
    else if (pDimmInfo->memoryType == MEM_TYPE_DDR1)
    {
        if (pDimmInfo->dimmBankDensity & BIT0)
            pDimmInfo->size += 1024;                /* Equal to 1GB     */
        else if (pDimmInfo->dimmBankDensity & BIT1)
            pDimmInfo->size += 2048;                /* Equal to 2GB     */
        else if (pDimmInfo->dimmBankDensity & BIT2)
            pDimmInfo->size += 16;                  /* Equal to 16MB    */
        else if (pDimmInfo->dimmBankDensity & BIT3)
            pDimmInfo->size += 32;                  /* Equal to 32MB    */
        else if (pDimmInfo->dimmBankDensity & BIT4)
            pDimmInfo->size += 64;                  /* Equal to 64MB    */
        else if (pDimmInfo->dimmBankDensity & BIT5)
            pDimmInfo->size += 128;                 /* Equal to 128MB   */
        else if (pDimmInfo->dimmBankDensity & BIT6) 
            pDimmInfo->size += 256;                 /* Equal to 256MB   */
        else if (pDimmInfo->dimmBankDensity & BIT7) 
            pDimmInfo->size += 512;                 /* Equal to 512MB   */
    }
    else /* if (dimmInfo.memoryType == MEM_TYPE_DDR2) */
    {
        if (pDimmInfo->dimmBankDensity & BIT0)
            pDimmInfo->size += 1024;                /* Equal to 1GB     */
        else if (pDimmInfo->dimmBankDensity & BIT1)
            pDimmInfo->size += 2048;                /* Equal to 2GB     */
        else if (pDimmInfo->dimmBankDensity & BIT2)
            pDimmInfo->size += 4096;                /* Equal to 4GB     */
        else if (pDimmInfo->dimmBankDensity & BIT3)
            pDimmInfo->size += 8192;                /* Equal to 8GB     */
        else if (pDimmInfo->dimmBankDensity & BIT4)
            pDimmInfo->size += 16384;               /* Equal to 16GB    */
        else if (pDimmInfo->dimmBankDensity & BIT5)
            pDimmInfo->size += 128;                 /* Equal to 128MB   */
        else if (pDimmInfo->dimmBankDensity & BIT6) 
            pDimmInfo->size += 256;                 /* Equal to 256MB   */
        else if (pDimmInfo->dimmBankDensity & BIT7) 
            pDimmInfo->size += 512;                 /* Equal to 512MB   */
    }
    
    pDimmInfo->size *= pDimmInfo->numOfModuleBanks;

    DB(mvOsPrintf("Dram: dimm size    %dMB \n",pDimmInfo->size));

    return MV_OK;
}

/*******************************************************************************
* dimmSpdPrint - Print the SPD parameters.
*
* DESCRIPTION:
*       Print the Dimm SPD parameters.
*
* INPUT:
*       pDimmInfo - DIMM information structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID dimmSpdPrint(MV_U32 dimmNum)
{
    MV_DIMM_INFO dimmInfo;
    MV_U32  i, temp = 0;
    MV_U32  k, maskLeftOfPoint = 0, maskRightOfPoint = 0;
    MV_U32  rightOfPoint = 0,leftOfPoint = 0, div, time_tmp, shift;
    MV_U32  busClkPs;
    MV_U8   trp_clocks=0, trcd_clocks, tras_clocks, trrd_clocks,
            temp_buf[40], *spdRawData;

    busClkPs = 1000000000 / (mvBoardSysClkGet() / 100);  /* in 10 ps units */

    spdRawData = dimmInfo.spdRawData;
    
    if(MV_OK != dimmSpdGet(dimmNum, &dimmInfo))
    {
        mvOsOutput("ERROR: Could not read SPD information!\n");
        return;
    }

    /* find Manufactura of Dimm Module */
    mvOsOutput("\nManufacturer's JEDEC ID Code:   ");
    for(i = 0 ; i < DIMM_MODULE_MANU_SIZE ; i++)
    {
        mvOsOutput("%x",spdRawData[DIMM_MODULE_MANU_OFFS + i]);
    }
    mvOsOutput("\n");

    /* Manufacturer's Specific Data */
    for(i = 0 ; i < DIMM_MODULE_ID_SIZE ; i++)
    {
        temp_buf[i] = spdRawData[DIMM_MODULE_ID_OFFS + i];
    }
    mvOsOutput("Manufacturer's Specific Data:   %s\n", temp_buf);

    /* Module Part Number */
    for(i = 0 ; i < DIMM_MODULE_VEN_SIZE ; i++)
    {
        temp_buf[i] = spdRawData[DIMM_MODULE_VEN_OFFS + i];
    }
    mvOsOutput("Module Part Number:             %s\n", temp_buf);

    /* Module Serial Number */
    for(i = 0; i < sizeof(MV_U32); i++)
    {
    	temp |= spdRawData[95+i] << 8*i;
    }
    mvOsOutput("DIMM Serial No.                 %ld (%lx)\n", (long)temp, 
                                    (long)temp);

    /* find Manufac-Data of Dimm Module */
    mvOsOutput("Manufactoring Date:             Year 20%d%d/ ww %d%d\n", 
                        ((spdRawData[93] & 0xf0) >> 4), (spdRawData[93] & 0xf), 
                        ((spdRawData[94] & 0xf0) >> 4), (spdRawData[94] & 0xf)); 
    /* find modul_revision of Dimm Module */
    mvOsOutput("Module Revision:                %d.%d\n", 
                                                spdRawData[91], spdRawData[92]); 

    /* find manufac_place of Dimm Module */
    mvOsOutput("manufac_place:                  %d\n", spdRawData[72]);
    
    /* go over the first 35 I2C data bytes */
    for(i = 2 ; i <= 35 ; i++)
       switch(i)
        {
            case 2:  /* Memory type (DDR1/2 / SDRAM) */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                    mvOsOutput("Dram Type is:                   SDRAM\n");
                else if (dimmInfo.memoryType == MEM_TYPE_DDR1)
                    mvOsOutput("Dram Type is:                   SDRAM DDR1\n");
                else if (dimmInfo.memoryType == MEM_TYPE_DDR2)
                    mvOsOutput("Dram Type is:                   SDRAM DDR2\n");
                else
                    mvOsOutput("Dram Type unknown\n");
                break;
/*----------------------------------------------------------------------------*/

            case 3:  /* Number Of Row Addresses */
                mvOsOutput("Module Number of row addresses: %d\n", 
                                                        dimmInfo.numOfRowAddr);
                break;
/*----------------------------------------------------------------------------*/
    
            case 4:  /* Number Of Column Addresses */
                mvOsOutput("Module Number of col addresses: %d\n", 
                                                        dimmInfo.numOfColAddr);
                break;
/*----------------------------------------------------------------------------*/
    
            case 5:  /* Number Of Module Banks */
                mvOsOutput("Number of Banks on Mod.:        %d\n",  
                                                    dimmInfo.numOfModuleBanks);
                break;
/*----------------------------------------------------------------------------*/
    
            case 6:  /* Data Width */
                mvOsOutput("Module Data Width:              %d bit\n",  
                                                           dimmInfo.dataWidth);
                break;
/*----------------------------------------------------------------------------*/
    
            case 8:  /* Voltage Interface */
                switch(spdRawData[i])
                {
                    case 0x0:
                        mvOsOutput("Module is               TTL_5V_TOLERANT\n");
                        break;
                    case 0x1:
                        mvOsOutput("Module is               LVTTL\n");
                        break;
                    case 0x2:
                        mvOsOutput("Module is               HSTL_1_5V\n");
                        break;
                    case 0x3:
                        mvOsOutput("Module is               SSTL_3_3V\n");
                        break;
                    case 0x4:
                        mvOsOutput("Module is               SSTL_2_5V\n");
                        break;
                    case 0x5:
                        if (dimmInfo.memoryType != MEM_TYPE_SDRAM)
                        {
                            mvOsOutput("Module is                 SSTL_1_8V\n");
                            break;
                        }
                    default:
                        mvOsOutput("Module is               VOLTAGE_UNKNOWN\n");
                        break;
                }
                break;
/*----------------------------------------------------------------------------*/
    
            case 9:  /* Minimum Cycle Time At Max CasLatancy */
                leftOfPoint = (spdRawData[i] & 0xf0) >> 4;
                rightOfPoint = (spdRawData[i] & 0x0f) * 10;
                
                /* DDR2 addition of right of point */
                if ((spdRawData[i] & 0x0f) == 0xA)
                {
                    rightOfPoint = 25;
                }
                if ((spdRawData[i] & 0x0f) == 0xB)
                {
                    rightOfPoint = 33;
                }
                if ((spdRawData[i] & 0x0f) == 0xC)
                {
                    rightOfPoint = 66;
                }
                if ((spdRawData[i] & 0x0f) == 0xD)
                {
                    rightOfPoint = 75;
                }
                mvOsOutput("Minimum Cycle Time At Max CL:   %d.%d [ns]\n",
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 10: /* Clock To Data Out */
                div = (dimmInfo.memoryType == MEM_TYPE_SDRAM)? 10:100;
                time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                leftOfPoint     = time_tmp / div;
                rightOfPoint    = time_tmp % div;
                mvOsOutput("Clock To Data Out:              %d.%d [ns]\n",
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 11: /* Error Check Type */
                mvOsOutput("Error Check Type (0=NONE):      %d\n", 
                                                      dimmInfo.errorCheckType);
                break;
/*----------------------------------------------------------------------------*/

            case 12: /* Refresh Interval */
                mvOsOutput("Refresh Rate:                   %x\n", 
                                                     dimmInfo.refreshInterval);
                break;
/*----------------------------------------------------------------------------*/
    
            case 13: /* Sdram Width */
                mvOsOutput("Sdram Width:                    %d bits\n",     
                                                          dimmInfo.sdramWidth);
                break;
/*----------------------------------------------------------------------------*/
    
            case 14: /* Error Check Data Width */
                mvOsOutput("Error Check Data Width:         %d bits\n", 
                                                 dimmInfo.errorCheckDataWidth);
                break;
/*----------------------------------------------------------------------------*/

           case 15: /* Minimum Clock Delay is unsupported */
                if ((dimmInfo.memoryType == MEM_TYPE_SDRAM) ||
                    (dimmInfo.memoryType == MEM_TYPE_DDR1))
                {
                    mvOsOutput("Minimum Clk Delay back to back: %d\n", 
                                                                spdRawData[i]);
                }
                break;
/*----------------------------------------------------------------------------*/
    
            case 16: /* Burst Length Supported */
    /*     SDRAM/DDR1:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
    burst length =  * Page | TBD  | TBD  | TBD  |  8   |  4   |  2   |   1  * 
                    *********************************************************/ 
    /*     DDR2:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
    burst length =  * Page | TBD  | TBD  | TBD  |  8   |  4   | TBD  | TBD  * 
                    *********************************************************/ 
                mvOsOutput("Burst Length Supported: ");
                if ((dimmInfo.memoryType == MEM_TYPE_SDRAM) ||
                    (dimmInfo.memoryType == MEM_TYPE_DDR1))
                {
                    if (dimmInfo.burstLengthSupported & BIT0)
                        mvOsOutput("1, ");
                    if (dimmInfo.burstLengthSupported & BIT1)
                        mvOsOutput("2, ");
                }
                if (dimmInfo.burstLengthSupported & BIT2)
                    mvOsOutput("4, ");
                if (dimmInfo.burstLengthSupported & BIT3) 
                    mvOsOutput("8, ");
                
                mvOsOutput(" Bit \n");
                break;
/*----------------------------------------------------------------------------*/
    
            case 17: /* Number Of Banks On Each Device */
                mvOsOutput("Number Of Banks On Each Chip:   %d\n",  
                                              dimmInfo.numOfBanksOnEachDevice);
                break;
/*----------------------------------------------------------------------------*/
    
            case 18: /* Suported Cas Latencies */
                   
            /*      SDRAM:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
            CAS =   * TBD  |  7   |  6   |  5   |  4   |  3   |   2  |   1  * 
                    ********************************************************/ 

            /*     DDR 1:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
            CAS =   * TBD  |  4   | 3.5  |   3  | 2.5  |  2   | 1.5  |   1  * 
                    *********************************************************/

            /*     DDR 2:
                    *******-******-******-******-******-******-******-******* 
                    * bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 * 
                    *******-******-******-******-******-******-******-******* 
            CAS =   * TBD  | TBD  |  5   |  4   |  3   |  2   | TBD  | TBD  * 
                    *********************************************************/

                mvOsOutput("Suported Cas Latencies: (CL) 			");
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    for (k = 0; k <=7; k++)
                    {
                        if (dimmInfo.suportedCasLatencies & (1 << k))
                            mvOsOutput("%d,             ", k+1);
                    }
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR1)
                {
                    if (dimmInfo.suportedCasLatencies & BIT0)
                        mvOsOutput("1, ");
                    if (dimmInfo.suportedCasLatencies & BIT1)
                        mvOsOutput("1.5, ");
                    if (dimmInfo.suportedCasLatencies & BIT2)
                        mvOsOutput("2, ");
                    if (dimmInfo.suportedCasLatencies & BIT3)
                        mvOsOutput("2.5, ");
                    if (dimmInfo.suportedCasLatencies & BIT4)
                        mvOsOutput("3, ");
                    if (dimmInfo.suportedCasLatencies & BIT5)
                        mvOsOutput("3.5, ");
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR2)
                {
                    if (dimmInfo.suportedCasLatencies & BIT2)
                        mvOsOutput("2, ");
                    if (dimmInfo.suportedCasLatencies & BIT3)
                        mvOsOutput("3, ");
                    if (dimmInfo.suportedCasLatencies & BIT4)
                        mvOsOutput("4, ");
                    if (dimmInfo.suportedCasLatencies & BIT5)
                        mvOsOutput("5, ");		
                }
                else
                    mvOsOutput("?.?, ");		
                mvOsOutput("\n");
                break;
/*----------------------------------------------------------------------------*/
    
            case 20:   /* DDR2 DIMM type info */
                if (dimmInfo.memoryType == MEM_TYPE_DDR2)
                {
                    if (dimmInfo.dimmTypeInfo & (BIT0 | BIT4))
                        mvOsOutput("Registered DIMM (RDIMM)\n");
                    else if (dimmInfo.dimmTypeInfo & (BIT1 | BIT5))
                        mvOsOutput("Unbuffered DIMM (UDIMM)\n");
                    else 
                        mvOsOutput("Unknown DIMM type.\n");
                }

                break;
/*----------------------------------------------------------------------------*/
   
            case 21: /* SDRAM Modules Attributes */
                mvOsOutput("\nModule Attributes (SPD Byte 21): \n");
                
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    if (dimmInfo.dimmAttributes & BIT0)
                        mvOsOutput(" Buffered Addr/Control Input:   Yes\n");
                    else
                        mvOsOutput(" Buffered Addr/Control Input:   No\n");

                    if (dimmInfo.dimmAttributes & BIT1)
                        mvOsOutput(" Registered Addr/Control Input: Yes\n");
                    else
                        mvOsOutput(" Registered Addr/Control Input: No\n");
   
                    if (dimmInfo.dimmAttributes & BIT2)
                        mvOsOutput(" On-Card PLL (clock):           Yes \n");
                    else
                        mvOsOutput(" On-Card PLL (clock):           No \n");

                    if (dimmInfo.dimmAttributes & BIT3)
                        mvOsOutput(" Bufferd DQMB Input:            Yes \n");
                    else
                        mvOsOutput(" Bufferd DQMB Inputs:           No \n");
   
                    if (dimmInfo.dimmAttributes & BIT4)
                        mvOsOutput(" Registered DQMB Inputs:        Yes \n");
                    else
                        mvOsOutput(" Registered DQMB Inputs:        No \n");
 
                    if (dimmInfo.dimmAttributes & BIT5)
                        mvOsOutput(" Differential Clock Input:      Yes \n");
                    else
                        mvOsOutput(" Differential Clock Input:      No \n");
   
                    if (dimmInfo.dimmAttributes & BIT6)
                        mvOsOutput(" redundant Row Addressing:      Yes \n");
                    else
                        mvOsOutput(" redundant Row Addressing:      No \n");
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR1)
                {
                    if (dimmInfo.dimmAttributes & BIT0)
                        mvOsOutput(" Buffered Addr/Control Input:   Yes\n");
                    else 
                        mvOsOutput(" Buffered Addr/Control Input:   No\n");
   
                    if (dimmInfo.dimmAttributes & BIT1)
                        mvOsOutput(" Registered Addr/Control Input: Yes\n");
                    else
                        mvOsOutput(" Registered Addr/Control Input: No\n");
   
                    if (dimmInfo.dimmAttributes & BIT2)
                        mvOsOutput(" On-Card PLL (clock):           Yes \n");
                    else
                        mvOsOutput(" On-Card PLL (clock):           No \n");
            
                    if (dimmInfo.dimmAttributes & BIT3)
                        mvOsOutput(" FET Switch On-Card Enabled:    Yes \n");
                    else
                        mvOsOutput(" FET Switch On-Card Enabled:    No \n");
                    
                    if (dimmInfo.dimmAttributes & BIT4)
                        mvOsOutput(" FET Switch External Enabled:   Yes \n");
                    else
                        mvOsOutput(" FET Switch External Enabled:   No \n");

                    if (dimmInfo.dimmAttributes & BIT5)
                        mvOsOutput(" Differential Clock Input:      Yes \n");
                    else
                        mvOsOutput(" Differential Clock Input:      No \n");
                }
                else /* if (dimmInfo.memoryType == MEM_TYPE_DDR2) */
                {
                    mvOsOutput(" Number of Active Registers on the DIMM: %d\n", 
                                        (dimmInfo.dimmAttributes & 0x3) + 1);
            
                    mvOsOutput(" Number of PLLs on the DIMM: %d\n", 
                                      ((dimmInfo.dimmAttributes) >> 2) & 0x3);
               
                    if (dimmInfo.dimmAttributes & BIT4)
                        mvOsOutput(" FET Switch External Enabled:   Yes \n");
                    else
                        mvOsOutput(" FET Switch External Enabled:   No \n");

                    if (dimmInfo.dimmAttributes & BIT6)
                        mvOsOutput(" Analysis probe installed:      Yes \n");
                    else
                        mvOsOutput(" Analysis probe installed:      No \n");
                }
                
                break;
/*----------------------------------------------------------------------------*/

            case 22: /* Suported AutoPreCharge */
                mvOsOutput("\nModul Attributes (SPD Byte 22): \n");
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    if ( spdRawData[i] & BIT0 )
                        mvOsOutput(" Early Ras Precharge:           Yes \n");
                    else
                        mvOsOutput(" Early Ras Precharge:           No \n");
                                                        
                    if ( spdRawData[i] & BIT1 )                 
                        mvOsOutput(" AutoPreCharge:                 Yes \n");
                    else
                        mvOsOutput(" AutoPreCharge:                 No \n");
                                                            
                    if ( spdRawData[i] & BIT2 )                 
                        mvOsOutput(" Precharge All:                 Yes \n");
                    else
                        mvOsOutput(" Precharge All:                 No \n");
                                                        
                    if ( spdRawData[i] & BIT3 )                 
                        mvOsOutput(" Write 1/ReadBurst:             Yes \n");
                    else
                        mvOsOutput(" Write 1/ReadBurst:             No \n");
                                                        
                    if ( spdRawData[i] & BIT4 )                 
                        mvOsOutput(" lower VCC tolerance:           5%%\n");
                    else
                        mvOsOutput(" lower VCC tolerance:           10%%\n");
                                                        
                    if ( spdRawData[i] & BIT5 )                 
                        mvOsOutput(" upper VCC tolerance:           5%%\n");
                    else
                        mvOsOutput(" upper VCC tolerance:           10%%\n");
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR1)
                {
                    if ( spdRawData[i] & BIT0 )
                        mvOsOutput(" Supports Weak Driver:          Yes \n");
                    else
                        mvOsOutput(" Supports Weak Driver:          No \n");

                    if ( !(spdRawData[i] & BIT4) )
                        mvOsOutput(" lower VCC tolerance:           0.2V\n");
   
                    if ( !(spdRawData[i] & BIT5) )
                        mvOsOutput(" upper VCC tolerance:           0.2V\n");

                    if ( spdRawData[i] & BIT6 )
                        mvOsOutput(" Concurrent Auto Preharge:      Yes \n");
                    else
                        mvOsOutput(" Concurrent Auto Preharge:      No \n");

                    if ( spdRawData[i] & BIT7 )
                        mvOsOutput(" Supports Fast AP:              Yes \n");
                    else
                        mvOsOutput(" Supports Fast AP:              No \n");
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR2)
                {
                    if ( spdRawData[i] & BIT0 )
                        mvOsOutput(" Supports Weak Driver:          Yes \n");
                    else
                        mvOsOutput(" Supports Weak Driver:          No \n");
                }
                break;
/*----------------------------------------------------------------------------*/
    
            case 23:
            /* Minimum Cycle Time At Maximum Cas Latancy Minus 1 (2nd highest CL) */
                leftOfPoint = (spdRawData[i] & 0xf0) >> 4;
                rightOfPoint = (spdRawData[i] & 0x0f) * 10;
                
                /* DDR2 addition of right of point */
                if ((spdRawData[i] & 0x0f) == 0xA)
                {
                    rightOfPoint = 25;
                }
                if ((spdRawData[i] & 0x0f) == 0xB)
                {
                    rightOfPoint = 33;
                }
                if ((spdRawData[i] & 0x0f) == 0xC)
                {
                    rightOfPoint = 66;
                }
                if ((spdRawData[i] & 0x0f) == 0xD)
                {
                    rightOfPoint = 75;
                }

                mvOsOutput("Minimum Cycle Time At 2nd highest CasLatancy"
                           "(0 = Not supported): %d.%d [ns]\n",
                           leftOfPoint, rightOfPoint );
                break;
/*----------------------------------------------------------------------------*/
    
            case 24: /* Clock To Data Out 2nd highest Cas Latency Value*/
                div = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 10:100;
                time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                    ((spdRawData[i] & 0x0f));
                leftOfPoint     = time_tmp / div;
                rightOfPoint    = time_tmp % div;
                mvOsOutput("Clock To Data Out (2nd CL value): 		%d.%d [ns]\n",
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 25: 
            /* Minimum Cycle Time At Maximum Cas Latancy Minus 2 (3rd highest CL) */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    leftOfPoint = (spdRawData[i] & 0xfc) >> 2;
                    rightOfPoint = (spdRawData[i] & 0x3) * 25;
                }
                else    /* DDR1 or DDR2 */ 
                {
                    leftOfPoint = (spdRawData[i] & 0xf0) >> 4;
                    rightOfPoint = (spdRawData[i] & 0x0f) * 10;
                    
                    /* DDR2 addition of right of point */
                    if ((spdRawData[i] & 0x0f) == 0xA)
                    {
                        rightOfPoint = 25;
                    }
                    if ((spdRawData[i] & 0x0f) == 0xB)
                    {
                        rightOfPoint = 33;
                    }
                    if ((spdRawData[i] & 0x0f) == 0xC)
                    {
                        rightOfPoint = 66;
                    }
                    if ((spdRawData[i] & 0x0f) == 0xD)
                    {
                        rightOfPoint = 75;
                    }
                }
                mvOsOutput("Minimum Cycle Time At 3rd highest CasLatancy" 
                           "(0 = Not supported): %d.%d [ns]\n",
                           leftOfPoint, rightOfPoint );
                break;
/*----------------------------------------------------------------------------*/
    
            case 26: /* Clock To Data Out 3rd highest Cas Latency Value*/
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    leftOfPoint = (spdRawData[i] & 0xfc) >> 2;
                    rightOfPoint = (spdRawData[i] & 0x3) * 25;
                }
                else    /* DDR1 or DDR2 */ 
                {
                    time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                    leftOfPoint     = 0;
                    rightOfPoint    = time_tmp;
                }
                mvOsOutput("Clock To Data Out (3rd CL value): 		%d.%2d[ns]\n",
                                                  leftOfPoint, rightOfPoint );
                break;
/*----------------------------------------------------------------------------*/
    
            case 27: /* Minimum Row Precharge Time */
                shift = (dimmInfo.memoryType == MEM_TYPE_SDRAM)? 0:2;
                maskLeftOfPoint  = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0xff : 0xfc;
                maskRightOfPoint = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0x00 : 0x03;
                leftOfPoint = ((spdRawData[i] & maskLeftOfPoint) >> shift);
                rightOfPoint = (spdRawData[i] & maskRightOfPoint)*25;
                temp = ((leftOfPoint*100) + rightOfPoint);/* in 10ps Intervals*/
                trp_clocks = (temp + (busClkPs-1)) /  busClkPs;    
                mvOsOutput("Minimum Row Precharge Time [ns]: 		%d.%d = " 
                           "in Clk cycles %d\n", 
                           leftOfPoint, rightOfPoint, trp_clocks);
                break;
/*----------------------------------------------------------------------------*/
    
            case 28: /* Minimum Row Active to Row Active Time */
                shift = (dimmInfo.memoryType == MEM_TYPE_SDRAM)? 0:2;
                maskLeftOfPoint  = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0xff : 0xfc;
                maskRightOfPoint = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0x00 : 0x03;
                leftOfPoint = ((spdRawData[i] & maskLeftOfPoint) >> shift);
                rightOfPoint = (spdRawData[i] & maskRightOfPoint)*25;
                temp = ((leftOfPoint*100) + rightOfPoint);/* in 100ns Interval*/
                trrd_clocks = (temp + (busClkPs-1)) / busClkPs;
                mvOsOutput("Minimum Row Active -To- Row Active Delay [ns]: " 
                           "%d.%d = in Clk cycles %d\n",
                            leftOfPoint, rightOfPoint, trp_clocks);
                break;
/*----------------------------------------------------------------------------*/
    
            case 29: /* Minimum Ras-To-Cas Delay */
                shift = (dimmInfo.memoryType == MEM_TYPE_SDRAM)? 0:2;
                maskLeftOfPoint  = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0xff : 0xfc;
                maskRightOfPoint = (dimmInfo.memoryType == MEM_TYPE_SDRAM) ? 
                                                                    0x00 : 0x03;
                leftOfPoint = ((spdRawData[i] & maskLeftOfPoint) >> shift);
                rightOfPoint = (spdRawData[i] & maskRightOfPoint)*25;  
                temp = ((leftOfPoint*100) + rightOfPoint);/* in 100ns Interval*/
                trcd_clocks = (temp + (busClkPs-1) )/ busClkPs;
                mvOsOutput("Minimum Ras-To-Cas Delay [ns]: 			%d.%d = "
                           "in Clk cycles %d\n", 
                           leftOfPoint, rightOfPoint, trp_clocks);
                break;
/*----------------------------------------------------------------------------*/
   
            case 30: /* Minimum Ras Pulse Width */
                tras_clocks = (cas2ps(spdRawData[i])+(busClkPs-1)) / busClkPs;
                mvOsOutput("Minimum Ras Pulse Width [ns]: 			%d = "
                           "in Clk cycles %d\n", spdRawData[i], tras_clocks);
                break;
/*----------------------------------------------------------------------------*/
    
            case 31: /* Module Bank Density */
                mvOsOutput("Module Bank Density (more than 1= Multisize-Module):");

                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    if (dimmInfo.dimmBankDensity & BIT0)
                        mvOsOutput("1GB, ");
                    if (dimmInfo.dimmBankDensity & BIT1)
                        mvOsOutput("8MB, ");
                    if (dimmInfo.dimmBankDensity & BIT2)
                        mvOsOutput("16MB, ");
                    if (dimmInfo.dimmBankDensity & BIT3)
                        mvOsOutput("32MB, ");
                    if (dimmInfo.dimmBankDensity & BIT4)
                        mvOsOutput("64MB, ");
                    if (dimmInfo.dimmBankDensity & BIT5)
                        mvOsOutput("128MB, ");
                    if (dimmInfo.dimmBankDensity & BIT6) 
                        mvOsOutput("256MB, ");
                    if (dimmInfo.dimmBankDensity & BIT7) 
                        mvOsOutput("512MB, ");
                }
                else if (dimmInfo.memoryType == MEM_TYPE_DDR1)
                {
                    if (dimmInfo.dimmBankDensity & BIT0)
                        mvOsOutput("1GB, ");
                    if (dimmInfo.dimmBankDensity & BIT1)
                        mvOsOutput("2GB, ");
                    if (dimmInfo.dimmBankDensity & BIT2)
                        mvOsOutput("16MB, ");
                    if (dimmInfo.dimmBankDensity & BIT3)
                        mvOsOutput("32MB, ");
                    if (dimmInfo.dimmBankDensity & BIT4)
                        mvOsOutput("64MB, ");
                    if (dimmInfo.dimmBankDensity & BIT5)
                        mvOsOutput("128MB, ");
                    if (dimmInfo.dimmBankDensity & BIT6) 
                        mvOsOutput("256MB, ");
                    if (dimmInfo.dimmBankDensity & BIT7) 
                        mvOsOutput("512MB, ");
                }
                else /* if (dimmInfo.memoryType == MEM_TYPE_DDR2) */
                {
                    if (dimmInfo.dimmBankDensity & BIT0)
                        mvOsOutput("1GB, ");
                    if (dimmInfo.dimmBankDensity & BIT1)
                        mvOsOutput("2GB, ");
                    if (dimmInfo.dimmBankDensity & BIT2)
                        mvOsOutput("4GB, ");
                    if (dimmInfo.dimmBankDensity & BIT3)
                        mvOsOutput("8GB, ");
                    if (dimmInfo.dimmBankDensity & BIT4)
                        mvOsOutput("16GB, ");
                    if (dimmInfo.dimmBankDensity & BIT5)
                    mvOsOutput("128MB, ");
                        if (dimmInfo.dimmBankDensity & BIT6) 
                    mvOsOutput("256MB, ");
                        if (dimmInfo.dimmBankDensity & BIT7) 
                    mvOsOutput("512MB, ");
                }
                mvOsOutput("\n");
                break;
/*----------------------------------------------------------------------------*/
    
            case 32: /* Address And Command Setup Time (measured in ns/1000) */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    rightOfPoint = (spdRawData[i] & 0x0f);
                    leftOfPoint  = (spdRawData[i] & 0xf0) >> 4;
                    if(leftOfPoint > 7)
                    {
                    leftOfPoint *= -1;
                    }
                }
                else /* DDR1 or DDR2 */
                {
                    time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                    leftOfPoint = time_tmp / 100;
                    rightOfPoint = time_tmp % 100; 
                }
                mvOsOutput("Address And Command Setup Time [ns]: 		%d.%d\n",
                                                     leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 33: /* Address And Command Hold Time */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    rightOfPoint = (spdRawData[i] & 0x0f);
                    leftOfPoint  = (spdRawData[i] & 0xf0) >> 4;
                    if(leftOfPoint > 7)
                    {
                    leftOfPoint *= -1;
                    }
                }
                else /* DDR1 or DDR2 */
                {
                    time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                    leftOfPoint = time_tmp / 100;
                    rightOfPoint = time_tmp % 100;                 
                }
                mvOsOutput("Address And Command Hold Time [ns]: 		%d.%d\n",
                                                   leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 34: /* Data Input Setup Time */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    rightOfPoint = (spdRawData[i] & 0x0f);
                    leftOfPoint  = (spdRawData[i] & 0xf0) >> 4;
                    if(leftOfPoint > 7)
                    {
                        leftOfPoint *= -1;
                    }
                }
                else /* DDR1 or DDR2 */
                {
                    time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                    leftOfPoint = time_tmp / 100;
                    rightOfPoint = time_tmp % 100;                 
                }
                mvOsOutput("Data Input Setup Time [ns]: 			%d.%d\n", 
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 35: /* Data Input Hold Time */
                if (dimmInfo.memoryType == MEM_TYPE_SDRAM)
                {
                    rightOfPoint = (spdRawData[i] & 0x0f);
                    leftOfPoint  = (spdRawData[i] & 0xf0) >> 4;
                    if(leftOfPoint > 7)
                    {
                        leftOfPoint *= -1;
                    }
                }
                else /* DDR1 or DDR2 */
                {
                    time_tmp = (((spdRawData[i] & 0xf0) >> 4)*10) + 
                                                      ((spdRawData[i] & 0x0f));
                    leftOfPoint = time_tmp / 100;
                    rightOfPoint = time_tmp % 100;                 
                }
                mvOsOutput("Data Input Hold Time [ns]: 			%d.%d\n\n", 
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
    
            case 36: /* Relevant for DDR2 only: Write Recovery Time */
                leftOfPoint = ((spdRawData[i] & maskLeftOfPoint) >> 2);
                rightOfPoint = (spdRawData[i] & maskRightOfPoint) * 25;  
                mvOsOutput("Write Recovery Time [ns]: 			%d.%d\n", 
                                                    leftOfPoint, rightOfPoint);
                break;
/*----------------------------------------------------------------------------*/
        }
    
}


/*
 * translate ns.ns/10 coding of SPD timing values
 * into ps unit values
 */
/*******************************************************************************
*  cas2ps - Translate x.y ns parameter to pico-seconds values
*
* DESCRIPTION:
*       This function translates x.y nano seconds to its value in pico seconds.
*       For example 3.75ns will return 3750.
*
* INPUT:
*       spd_byte - DIMM SPD byte.
*
* OUTPUT:
*       None.
*
* RETURN:
*       value in pico seconds.
*
*******************************************************************************/
static MV_U32  cas2ps(MV_U8 spd_byte)
{
    MV_U32 ns, ns10;
    
    /* isolate upper nibble */
    ns = (spd_byte >> 4) & 0x0F;
    /* isolate lower nibble */
    ns10 = (spd_byte & 0x0F);
    
    if( ns10 < 10 ) {
        ns10 *= 10;
    }
    else if( ns10 == 10 )
        ns10 = 25;
    else if( ns10 == 11 )
        ns10 = 33;
    else if( ns10 == 12 )
        ns10 = 66;
    else if( ns10 == 13 )
        ns10 = 75;
    else 
    {
        mvOsOutput("cas2ps Err. unsupported cycle time.\n");
    }
    
    return (ns*1000 + ns10*10);
}

