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
#include "mvOs.h"
#include "sflash/mvSFlash.h"
#include "sflash/mvSFlashSpec.h"
#include "spi/mvSpi.h"
#include "spi/mvSpiCmnd.h"
#include "ctrlEnv/mvCtrlEnvLib.h"

/*#define MV_DEBUG*/
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* Globals */
static MV_SFLASH_DEVICE_PARAMS sflash[] = {
    /* ST M25P32 SPI flash, 4MB, 64 sectors of 64K each */
    {
     MV_M25P_WREN_CMND_OPCD,
     MV_M25P_WRDI_CMND_OPCD,
     MV_M25P_RDID_CMND_OPCD,
     MV_M25P_RDSR_CMND_OPCD,
     MV_M25P_WRSR_CMND_OPCD,
     MV_M25P_READ_CMND_OPCD,
     MV_M25P_FAST_RD_CMND_OPCD,
     MV_M25P_PP_CMND_OPCD,
     MV_M25P_SE_CMND_OPCD,
     MV_M25P_BE_CMND_OPCD,
     MV_M25P_RES_CMND_OPCD,
     MV_SFLASH_NO_SPECIFIC_OPCD,    /* power save not supported */
     MV_M25P32_SECTOR_SIZE,
     MV_M25P32_SECTOR_NUMBER,
     MV_M25P_PAGE_SIZE,
     "ST M25P32",
     MV_M25PXXX_ST_MANF_ID,
     MV_M25P32_DEVICE_ID,
     MV_M25P32_MAX_SPI_FREQ,
     MV_M25P32_MAX_FAST_SPI_FREQ,
     MV_M25P32_FAST_READ_DUMMY_BYTES
    },
    /* ST M25P64 SPI flash, 8MB, 128 sectors of 64K each */
    {
     MV_M25P_WREN_CMND_OPCD,
     MV_M25P_WRDI_CMND_OPCD,
     MV_M25P_RDID_CMND_OPCD,
     MV_M25P_RDSR_CMND_OPCD,
     MV_M25P_WRSR_CMND_OPCD,
     MV_M25P_READ_CMND_OPCD,
     MV_M25P_FAST_RD_CMND_OPCD,
     MV_M25P_PP_CMND_OPCD,
     MV_M25P_SE_CMND_OPCD,
     MV_M25P_BE_CMND_OPCD,
     MV_M25P_RES_CMND_OPCD,
     MV_SFLASH_NO_SPECIFIC_OPCD,    /* power save not supported */
     MV_M25P64_SECTOR_SIZE,
     MV_M25P64_SECTOR_NUMBER,
     MV_M25P_PAGE_SIZE,
     "ST M25P64",
     MV_M25PXXX_ST_MANF_ID,
     MV_M25P64_DEVICE_ID,
     MV_M25P64_MAX_SPI_FREQ,
     MV_M25P64_MAX_FAST_SPI_FREQ,
     MV_M25P64_FAST_READ_DUMMY_BYTES
    },
    /* ST M25P128 SPI flash, 16MB, 64 sectors of 256K each */
    {
     MV_M25P_WREN_CMND_OPCD,
     MV_M25P_WRDI_CMND_OPCD,
     MV_M25P_RDID_CMND_OPCD,
     MV_M25P_RDSR_CMND_OPCD,
     MV_M25P_WRSR_CMND_OPCD,
     MV_M25P_READ_CMND_OPCD,
     MV_M25P_FAST_RD_CMND_OPCD,
     MV_M25P_PP_CMND_OPCD,
     MV_M25P_SE_CMND_OPCD,
     MV_M25P_BE_CMND_OPCD,
     MV_M25P_RES_CMND_OPCD,
     MV_SFLASH_NO_SPECIFIC_OPCD,    /* power save not supported */
     MV_M25P128_SECTOR_SIZE,
     MV_M25P128_SECTOR_NUMBER,
     MV_M25P_PAGE_SIZE,
     "ST M25P128",
     MV_M25PXXX_ST_MANF_ID,
     MV_M25P128_DEVICE_ID,
     MV_M25P128_MAX_SPI_FREQ,
     MV_M25P128_MAX_FAST_SPI_FREQ,
     MV_M25P128_FAST_READ_DUMMY_BYTES
    },
    /* Macronix MXIC MX25L6405 SPI flash, 8MB, 128 sectors of 64K each */
    {
     MV_MX25L_WREN_CMND_OPCD,
     MV_MX25L_WRDI_CMND_OPCD,
     MV_MX25L_RDID_CMND_OPCD,
     MV_MX25L_RDSR_CMND_OPCD,
     MV_MX25L_WRSR_CMND_OPCD,
     MV_MX25L_READ_CMND_OPCD,
     MV_MX25L_FAST_RD_CMND_OPCD,
     MV_MX25L_PP_CMND_OPCD,
     MV_MX25L_SE_CMND_OPCD,
     MV_MX25L_BE_CMND_OPCD,
     MV_MX25L_RES_CMND_OPCD,
     MV_MX25L_DP_CMND_OPCD,
     MV_MX25L6405_SECTOR_SIZE,
     MV_MX25L6405_SECTOR_NUMBER,
     MV_MXIC_PAGE_SIZE,
     "MXIC MX25L6405",
     MV_MXIC_MANF_ID,
     MV_MX25L6405_DEVICE_ID,
     MV_MX25L6405_MAX_SPI_FREQ,
     MV_MX25L6405_MAX_FAST_SPI_FREQ,
     MV_MX25L6405_FAST_READ_DUMMY_BYTES
    },
    /* SPANSION S25FL128P SPI flash, 16MB, 64 sectors of 256K each */
    {
     MV_S25FL_WREN_CMND_OPCD,
     MV_S25FL_WRDI_CMND_OPCD,
     MV_S25FL_RDID_CMND_OPCD,
     MV_S25FL_RDSR_CMND_OPCD,
     MV_S25FL_WRSR_CMND_OPCD,
     MV_S25FL_READ_CMND_OPCD,
     MV_S25FL_FAST_RD_CMND_OPCD,
     MV_S25FL_PP_CMND_OPCD,
     MV_S25FL_SE_CMND_OPCD,
     MV_S25FL_BE_CMND_OPCD,
     MV_S25FL_RES_CMND_OPCD,
     MV_S25FL_DP_CMND_OPCD,
     MV_S25FL128_SECTOR_SIZE,
     MV_S25FL128_SECTOR_NUMBER,
     MV_S25FL_PAGE_SIZE,
     "SPANSION S25FL128",
     MV_SPANSION_MANF_ID,
     MV_S25FL128_DEVICE_ID,
     MV_S25FL128_MAX_SPI_FREQ,
     MV_M25P128_MAX_FAST_SPI_FREQ,
     MV_M25P128_FAST_READ_DUMMY_BYTES
    }
};

/* Static Functions */
static MV_STATUS    mvWriteEnable   (MV_SFLASH_INFO * pFlinfo);
static MV_STATUS    mvStatusRegGet  (MV_SFLASH_INFO * pFlinfo, MV_U8 * pStatReg);
static MV_STATUS    mvStatusRegSet  (MV_SFLASH_INFO * pFlinfo, MV_U8 sr);
static MV_STATUS    mvWaitOnWipClear(MV_SFLASH_INFO * pFlinfo);
static MV_STATUS    mvSFlashPageWr  (MV_SFLASH_INFO * pFlinfo, MV_U32 offset, \
							         MV_U8* pPageBuff, MV_U32 buffSize);
static MV_STATUS    mvSFlashWithDefaultsIdGet (MV_SFLASH_INFO * pFlinfo, \
                                            MV_U8* manId, MV_U16* devId);

/*******************************************************************************
* mvWriteEnable - serialize the write enable sequence
*
* DESCRIPTION:
*       transmit the sequence for write enable
*
********************************************************************************/
static MV_STATUS mvWriteEnable(MV_SFLASH_INFO * pFlinfo)
{
	MV_U8 cmd[MV_SFLASH_WREN_CMND_LENGTH];


    cmd[0] = sflash[pFlinfo->index].opcdWREN;

	return mvSpiWriteThenRead(cmd, MV_SFLASH_WREN_CMND_LENGTH, NULL, 0, 0);
}

/*******************************************************************************
* mvStatusRegGet - Retrieve the value of the status register
*
* DESCRIPTION:
*       perform the RDSR sequence to get the 8bit status register
*
********************************************************************************/
static MV_STATUS mvStatusRegGet(MV_SFLASH_INFO * pFlinfo, MV_U8 * pStatReg)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_RDSR_CMND_LENGTH];
	MV_U8 sr[MV_SFLASH_RDSR_REPLY_LENGTH];




	cmd[0] = sflash[pFlinfo->index].opcdRDSR;

	if ((ret = mvSpiWriteThenRead(cmd, MV_SFLASH_RDSR_CMND_LENGTH, sr,
                                         MV_SFLASH_RDSR_REPLY_LENGTH,0)) != MV_OK)
        return ret;

    *pStatReg = sr[0];

    return MV_OK;
}

/*******************************************************************************
* mvWaitOnWipClear - Block waiting for the WIP (write in progress) to be cleared
*
* DESCRIPTION:
*       Block waiting for the WIP (write in progress) to be cleared
*
********************************************************************************/
static MV_STATUS mvWaitOnWipClear(MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
	MV_U32 i;
    MV_U8 stat;

	for (i=0; i<MV_SFLASH_MAX_WAIT_LOOP; i++)
	{
        if ((ret = mvStatusRegGet(pFlinfo, &stat)) != MV_OK)
            return ret;

		if ((stat & MV_SFLASH_STATUS_REG_WIP_MASK) == 0)
			return MV_OK;
	}

    DB(mvOsPrintf("%s WARNING: Write Timeout!\n", __FUNCTION__);)
	return MV_TIMEOUT;
}

/*******************************************************************************
* mvWaitOnChipEraseDone - Block waiting for the WIP (write in progress) to be
*                         cleared after a chip erase command which is supposed
*                         to take about 2:30 minutes
*
* DESCRIPTION:
*       Block waiting for the WIP (write in progress) to be cleared
*
********************************************************************************/
static MV_STATUS mvWaitOnChipEraseDone(MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
	MV_U32 i;
    MV_U8 stat;

	for (i=0; i<MV_SFLASH_CHIP_ERASE_MAX_WAIT_LOOP; i++)
	{
        if ((ret = mvStatusRegGet(pFlinfo, &stat)) != MV_OK)
            return ret;

		if ((stat & MV_SFLASH_STATUS_REG_WIP_MASK) == 0)
			return MV_OK;
	}

    DB(mvOsPrintf("%s WARNING: Write Timeout!\n", __FUNCTION__);)
	return MV_TIMEOUT;
}

/*******************************************************************************
*  mvStatusRegSet - Set the value of the 8bit status register
*
* DESCRIPTION:
*       Set the value of the 8bit status register
*
********************************************************************************/
static MV_STATUS mvStatusRegSet(MV_SFLASH_INFO * pFlinfo, MV_U8 sr)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_WRSR_CMND_LENGTH];


    /* Issue the Write enable command prior the WRSR command */
	if ((ret = mvWriteEnable(pFlinfo)) != MV_OK)
		return ret;

    /* Write the SR with the new values */
    cmd[0] = sflash[pFlinfo->index].opcdWRSR;
	cmd[1] = sr;

	if ((ret = mvSpiWriteThenRead(cmd, MV_SFLASH_WRSR_CMND_LENGTH, NULL, 0, 0)) != MV_OK)
		return ret;

    if ((ret = mvWaitOnWipClear(pFlinfo)) != MV_OK)
		return ret;

    mvOsDelay(1);

    return MV_OK;
}

/*******************************************************************************
* mvSFlashPageWr - Write up to 256 Bytes in the same page
*
* DESCRIPTION:
*       Write a buffer up to the page size in length provided that the whole address
*		range is within the same page (alligned to page bounderies)
*
*******************************************************************************/
static MV_STATUS mvSFlashPageWr (MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
							     MV_U8* pPageBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_PP_CMND_LENGTH];


    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invalid parameter device index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

	/* check that we do not cross the page bounderies */
    if (((offset & (sflash[pFlinfo->index].pageSize - 1)) + buffSize) >
        sflash[pFlinfo->index].pageSize)
    {
        DB(mvOsPrintf("%s WARNING: Page allignment problem!\n", __FUNCTION__);)
		return MV_OUT_OF_RANGE;
    }

	/* Issue the Write enable command prior the page program command */
	if ((ret = mvWriteEnable(pFlinfo)) != MV_OK)
		return ret;

    cmd[0] = sflash[pFlinfo->index].opcdPP;
	cmd[1] = ((offset >> 16) & 0xFF);
	cmd[2] = ((offset >> 8) & 0xFF);
	cmd[3] = (offset & 0xFF);

	if ((ret = mvSpiWriteThenWrite(cmd, MV_SFLASH_PP_CMND_LENGTH, pPageBuff, buffSize)) != MV_OK)
		return ret;

	if ((ret = mvWaitOnWipClear(pFlinfo)) != MV_OK)
		return ret;

	return MV_OK;
}

/*******************************************************************************
* mvSFlashWithDefaultsIdGet - Try to read the manufacturer and Device IDs from
*       the device using the default RDID opcode and the default WREN opcode.
*
* DESCRIPTION:
*       This is used to detect a generic device that uses the default opcodes
*       for the WREN and RDID.
*
********************************************************************************/
static MV_STATUS mvSFlashWithDefaultsIdGet (MV_SFLASH_INFO * pFlinfo, MV_U8* manId, MV_U16* devId)
{
    MV_STATUS ret;
    MV_U8 cmdRDID[MV_SFLASH_RDID_CMND_LENGTH];
	MV_U8 id[MV_SFLASH_RDID_REPLY_LENGTH];



    /* Use the default RDID opcode to read the IDs */
    cmdRDID[0] = MV_SFLASH_DEFAULT_RDID_OPCD;   /* unknown model try default */
	if ((ret = mvSpiWriteThenRead(cmdRDID, MV_SFLASH_RDID_CMND_LENGTH, id, MV_SFLASH_RDID_REPLY_LENGTH, 0)) != MV_OK)
		return ret;

	*manId = id[0];
	*devId = 0;
	*devId |= (id[1] << 8);
	*devId |= id[2];

	return MV_OK;
}

/*
#####################################################################################
#####################################################################################
*/

/*******************************************************************************
* mvSFlashInit - Initialize the serial flash device
*
* DESCRIPTION:
*       Perform the neccessary initialization and configuration
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*           pFlinfo->baseAddr: base address in fast mode.
*           pFlinfo->index: Index of the flash in the sflash tabel. If the SPI
*                           flash device does not support read Id command with
*                           the standard opcode, then the user should supply this
*                           as an input to skip the autodetection process!!!!
*
* OUTPUT:
*       pFlinfo: pointer to the Flash information structure after detection
*           pFlinfo->manufacturerId: Manufacturer ID
*           pFlinfo->deviceId: Device ID
*           pFlinfo->sectorSize: size of the sector (all sectors are the same).
*           pFlinfo->sectorNumber: number of sectors.
*           pFlinfo->pageSize: size of the page.
*           pFlinfo->index: Index of the detected flash in the sflash tabel
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashInit (MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
    MV_U8 manf;
    MV_U16 dev;
    MV_U32 indx;
    MV_BOOL detectFlag = MV_FALSE;

    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Initialize the SPI interface with low frequency to make sure that the read ID succeeds */
    if ((ret = mvSpiInit(MV_SFLASH_BASIC_SPI_FREQ)) != MV_OK)
    {
        mvOsPrintf("%s ERROR: Failed to initialize the SPI interface!\n", __FUNCTION__);
        return ret;
    }

    /* First try to read the Manufacturer and Device IDs */
    if ((ret = mvSFlashIdGet(pFlinfo, &manf, &dev)) != MV_OK)
    {
        mvOsPrintf("%s ERROR: Failed to get the SFlash ID!\n", __FUNCTION__);
        return ret;
    }

    /* loop over the whole table and look for the appropriate SFLASH */
    for (indx=0; indx<MV_ARRAY_SIZE(sflash); indx++)
    {
        if ((manf == sflash[indx].manufacturerId) && (dev == sflash[indx].deviceId))
        {
            pFlinfo->manufacturerId = manf;
            pFlinfo->deviceId = dev;
            pFlinfo->index = indx;
            detectFlag = MV_TRUE;
        }
    }

    if(!detectFlag)
    {
        mvOsPrintf("%s ERROR: Unknown SPI flash device!\n", __FUNCTION__);
        return MV_FAIL;
    }

    /* fill the info based on the model detected */
    pFlinfo->sectorSize = sflash[pFlinfo->index].sectorSize;
    pFlinfo->sectorNumber = sflash[pFlinfo->index].sectorNumber;
    pFlinfo->pageSize = sflash[pFlinfo->index].pageSize;

    /* Set the SPI frequency to the MAX allowed for the device for best performance */
    if ((ret = mvSpiBaudRateSet(sflash[pFlinfo->index].spiMaxFreq)) != MV_OK)
    {
        mvOsPrintf("%s ERROR: Failed to set the SPI frequency!\n", __FUNCTION__);
        return ret;
    }

    /* As default lock the SR */
    if ((ret = mvSFlashStatRegLock(pFlinfo, MV_TRUE)) != MV_OK)
        return ret;

	return MV_OK;
}

/*******************************************************************************
* mvSFlashSectorErase - Erasse a single sector of the serial flash
*
* DESCRIPTION:
*       Issue the erase sector command and address
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		secNumber: sector Number to erase (0 -> (sectorNumber-1))
*
* OUTPUT:
*       None
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashSectorErase (MV_SFLASH_INFO * pFlinfo, MV_U32 secNumber)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_SE_CMND_LENGTH];

    MV_U32 secAddr = (secNumber * pFlinfo->sectorSize);
#if 0
    MV_U32 i;
    MV_U32 * pW = (MV_U32*) (secAddr + pFlinfo->baseAddr);
    MV_U32 erasedWord = 0xFFFFFFFF;
    MV_U32 wordsPerSector = (pFlinfo->sectorSize / sizeof(MV_U32));
    MV_BOOL eraseNeeded = MV_FALSE;
#endif
    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* check that the sector number is valid */
    if (secNumber >= pFlinfo->sectorNumber)
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter sector number!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }
    
    /* we don't want to access SPI in direct mode from in-direct API, 
	becasue of timing issue between CS asserts. */
#if 0
    /* First compare to FF and check if erase is needed */
    for (i=0; i<wordsPerSector; i++)
    {
        if (memcmp(pW, &erasedWord, sizeof(MV_U32)) != 0)
        {
            eraseNeeded = MV_TRUE;
            break;
        }

        ++pW;
    }
    if (!eraseNeeded)
        return MV_OK;
#endif

    cmd[0] = sflash[pFlinfo->index].opcdSE;
	cmd[1] = ((secAddr >> 16) & 0xFF);
	cmd[2] = ((secAddr >> 8) & 0xFF);
	cmd[3] = (secAddr & 0xFF);

	/* Issue the Write enable command prior the sector erase command */
	if ((ret = mvWriteEnable(pFlinfo)) != MV_OK)
		return ret;

	if ((ret = mvSpiWriteThenWrite(cmd, MV_SFLASH_SE_CMND_LENGTH, NULL, 0)) != MV_OK)
		return ret;

	if ((ret = mvWaitOnWipClear(pFlinfo)) != MV_OK)
		return ret;

	return MV_OK;
}

/*******************************************************************************
* mvSFlashChipErase - Erasse the whole serial flash
*
* DESCRIPTION:
*       Issue the bulk (chip) erase command
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*
* OUTPUT:
*       None
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashChipErase (MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_BE_CMND_LENGTH];


    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    cmd[0] = sflash[pFlinfo->index].opcdBE;

	/* Issue the Write enable command prior the Bulk erase command */
	if ((ret = mvWriteEnable(pFlinfo)) != MV_OK)
		return ret;

    if ((ret = mvSpiWriteThenWrite(cmd, MV_SFLASH_BE_CMND_LENGTH, NULL, 0)) != MV_OK)
		return ret;

	if ((ret = mvWaitOnChipEraseDone(pFlinfo)) != MV_OK)
		return ret;

	return MV_OK;
}

/*******************************************************************************
* mvSFlashBlockRd - Read from the serial flash
*
* DESCRIPTION:
*       Issue the read command and address then perfom the needed read
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		offset: byte offset with the flash to start reading from
*		pReadBuff: pointer to the buffer to read the data in
*		buffSize: size of the buffer to read.
*
* OUTPUT:
*       pReadBuff: pointer to the buffer containing the read data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashBlockRd (MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
						   MV_U8* pReadBuff, MV_U32 buffSize)
{
	MV_U8 cmd[MV_SFLASH_READ_CMND_LENGTH];


    /* check for NULL pointer */
    if ((pFlinfo == NULL) || (pReadBuff == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    cmd[0] = sflash[pFlinfo->index].opcdREAD;
	cmd[1] = ((offset >> 16) & 0xFF);
	cmd[2] = ((offset >> 8) & 0xFF);
	cmd[3] = (offset & 0xFF);

	return mvSpiWriteThenRead(cmd, MV_SFLASH_READ_CMND_LENGTH, pReadBuff, buffSize, 0);
}

/*******************************************************************************
* mvSFlashFastBlockRd - Fast read from the serial flash
*
* DESCRIPTION:
*       Issue the fast read command and address then perfom the needed read
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		offset: byte offset with the flash to start reading from
*		pReadBuff: pointer to the buffer to read the data in
*		buffSize: size of the buffer to read.
*
* OUTPUT:
*       pReadBuff: pointer to the buffer containing the read data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashFastBlockRd (MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
						       MV_U8* pReadBuff, MV_U32 buffSize)
{
    MV_U8 cmd[MV_SFLASH_READ_CMND_LENGTH];
    MV_STATUS ret;

    /* check for NULL pointer */
    if ((pFlinfo == NULL) || (pReadBuff == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* Set the SPI frequency to the MAX allowed for fast-read operations */
    mvOsPrintf("Setting freq to %d.\n",sflash[pFlinfo->index].spiMaxFastFreq);
    if ((ret = mvSpiBaudRateSet(sflash[pFlinfo->index].spiMaxFastFreq)) != MV_OK)
    {
        mvOsPrintf("%s ERROR: Failed to set the SPI fast frequency!\n", __FUNCTION__);
        return ret;
    }

    cmd[0] = sflash[pFlinfo->index].opcdFSTRD;
    cmd[1] = ((offset >> 16) & 0xFF);
    cmd[2] = ((offset >> 8) & 0xFF);
    cmd[3] = (offset & 0xFF);


    ret = mvSpiWriteThenRead(cmd, MV_SFLASH_READ_CMND_LENGTH, pReadBuff, buffSize,
                             sflash[pFlinfo->index].spiFastRdDummyBytes);

    /* Reset the SPI frequency to the MAX allowed for the device for best performance */
    if ((ret = mvSpiBaudRateSet(sflash[pFlinfo->index].spiMaxFreq)) != MV_OK)
    {
        mvOsPrintf("%s ERROR: Failed to set the SPI frequency!\n", __FUNCTION__);
        return ret;
    }

    return ret;
}


/*******************************************************************************
* mvSFlashBlockWr - Write a buffer with any size
*
* DESCRIPTION:
*       write regardless of the page boundaries and size limit per Page
*		program command
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		offset: byte offset within the flash region
*		pWriteBuff: pointer to the buffer holding the data to program
*		buffSize: size of the buffer to write
*
* OUTPUT:
*       None
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashBlockWr (MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
						   MV_U8* pWriteBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U32 data2write	= buffSize;
    MV_U32 preAllOffset = (offset & MV_SFLASH_PAGE_ALLIGN_MASK(MV_M25P_PAGE_SIZE));
    MV_U32 preAllSz		= (preAllOffset ? (MV_M25P_PAGE_SIZE - preAllOffset) : 0);
	MV_U32 writeOffset	= offset;

    /* check for NULL pointer */
#ifndef CONFIG_MARVELL
    if(NULL == pWriteBuff)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }
#endif

    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

	/* check that the buffer size does not exceed the flash size */
    if ((offset + buffSize) > mvSFlashSizeGet(pFlinfo))
    {
        DB(mvOsPrintf("%s WARNING: Write exceeds flash size!\n", __FUNCTION__);)
	    return MV_OUT_OF_RANGE;
    }

	/* check if the total block size is less than the first chunk remainder */
	if (data2write < preAllSz)
		preAllSz = data2write;

	/* check if programing does not start at a 64byte alligned offset */
	if (preAllSz)
	{
		if ((ret = mvSFlashPageWr(pFlinfo, writeOffset, pWriteBuff, preAllSz)) != MV_OK)
			return ret;

		/* increment pointers and counters */
		writeOffset += preAllSz;
		data2write -= preAllSz;
		pWriteBuff += preAllSz;
	}

	/* program the data that fits in complete page chunks */
	while (data2write >= sflash[pFlinfo->index].pageSize)
	{
		if ((ret = mvSFlashPageWr(pFlinfo, writeOffset, pWriteBuff, sflash[pFlinfo->index].pageSize)) != MV_OK)
			return ret;

		/* increment pointers and counters */
		writeOffset += sflash[pFlinfo->index].pageSize;
		data2write -= sflash[pFlinfo->index].pageSize;
		pWriteBuff += sflash[pFlinfo->index].pageSize;
	}

	/* program the last partial chunk */
	if (data2write)
	{
		if ((ret = mvSFlashPageWr(pFlinfo, writeOffset, pWriteBuff, data2write)) != MV_OK)
			return ret;
	}

	return MV_OK;
}

/*******************************************************************************
* mvSFlashIdGet - Get the manufacturer and device IDs.
*
* DESCRIPTION:
*       Get the Manufacturer and device IDs from the serial flash through
*		writing the RDID command then reading 3 bytes of data. In case that
*       this command was called for the first time in order to detect the
*       manufacturer and device IDs, then the default RDID opcode will be used
*       unless the device index is indicated by the user (in case the SPI flash
*       does not use the default RDID opcode).
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		pManId: pointer to the 8bit variable to hold the manufacturing ID
*		pDevId: pointer to the 16bit variable to hold the device ID
*
* OUTPUT:
*		pManId: pointer to the 8bit variable holding the manufacturing ID
*		pDevId: pointer to the 16bit variable holding the device ID
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashIdGet (MV_SFLASH_INFO * pFlinfo, MV_U8* pManId, MV_U16* pDevId)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_RDID_CMND_LENGTH];
	MV_U8 id[MV_SFLASH_RDID_REPLY_LENGTH];



    /* check for NULL pointer */
    if ((pFlinfo == NULL) || (pManId == NULL) || (pDevId == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
        return mvSFlashWithDefaultsIdGet(pFlinfo, pManId, pDevId);
    else
        cmd[0] = sflash[pFlinfo->index].opcdRDID;

	if ((ret = mvSpiWriteThenRead(cmd, MV_SFLASH_RDID_CMND_LENGTH, id, MV_SFLASH_RDID_REPLY_LENGTH, 0)) != MV_OK)
		return ret;

	*pManId = id[0];
	*pDevId = 0;
	*pDevId |= (id[1] << 8);
	*pDevId |= id[2];

	return MV_OK;
}

/*******************************************************************************
* mvSFlashWpRegionSet - Set the Write-Protected region
*
* DESCRIPTION:
*       Set the Write-Protected region
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		wpRegion: which region will be protected
*
* OUTPUT:
*       None
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashWpRegionSet (MV_SFLASH_INFO * pFlinfo, MV_SFLASH_WP_REGION wpRegion)
{
    MV_U8 wpMask;

    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* Check if the chip is an ST flash; then WP supports only 3 bits */
    if (pFlinfo->manufacturerId == MV_M25PXXX_ST_MANF_ID)
    {
        switch (wpRegion)
        {
            case MV_WP_NONE:
                wpMask = MV_M25P_STATUS_BP_NONE;
                break;

            case MV_WP_UPR_1OF128:
                DB(mvOsPrintf("%s WARNING: Invaild option for this flash chip!\n", __FUNCTION__);)
                return MV_NOT_SUPPORTED;

            case MV_WP_UPR_1OF64:
                wpMask = MV_M25P_STATUS_BP_1_OF_64;
                break;

            case MV_WP_UPR_1OF32:
                wpMask = MV_M25P_STATUS_BP_1_OF_32;
                break;

            case MV_WP_UPR_1OF16:
                wpMask = MV_M25P_STATUS_BP_1_OF_16;
                break;

            case MV_WP_UPR_1OF8:
                wpMask = MV_M25P_STATUS_BP_1_OF_8;
                break;

            case MV_WP_UPR_1OF4:
                wpMask = MV_M25P_STATUS_BP_1_OF_4;
                break;

            case MV_WP_UPR_1OF2:
                wpMask = MV_M25P_STATUS_BP_1_OF_2;
                break;

            case MV_WP_ALL:
                wpMask = MV_M25P_STATUS_BP_ALL;
                break;

            default:
                DB(mvOsPrintf("%s WARNING: Invaild parameter WP region!\n", __FUNCTION__);)
                return MV_BAD_PARAM;
        }
    }
    /* check if the manufacturer is MXIC then the WP is 4bits */
    else if (pFlinfo->manufacturerId == MV_MXIC_MANF_ID)
    {
        switch (wpRegion)
        {
            case MV_WP_NONE:
                wpMask = MV_MX25L_STATUS_BP_NONE;
                break;

            case MV_WP_UPR_1OF128:
                wpMask = MV_MX25L_STATUS_BP_1_OF_128;
                break;

            case MV_WP_UPR_1OF64:
                wpMask = MV_MX25L_STATUS_BP_1_OF_64;
                break;

            case MV_WP_UPR_1OF32:
                wpMask = MV_MX25L_STATUS_BP_1_OF_32;
                break;

            case MV_WP_UPR_1OF16:
                wpMask = MV_MX25L_STATUS_BP_1_OF_16;
                break;

            case MV_WP_UPR_1OF8:
                wpMask = MV_MX25L_STATUS_BP_1_OF_8;
                break;

            case MV_WP_UPR_1OF4:
                wpMask = MV_MX25L_STATUS_BP_1_OF_4;
                break;

            case MV_WP_UPR_1OF2:
                wpMask = MV_MX25L_STATUS_BP_1_OF_2;
                break;

            case MV_WP_ALL:
                wpMask = MV_MX25L_STATUS_BP_ALL;
                break;

            default:
                DB(mvOsPrintf("%s WARNING: Invaild parameter WP region!\n", __FUNCTION__);)
                return MV_BAD_PARAM;
        }
    }
    /* check if the manufacturer is SPANSION then the WP is 4bits */
    else if (pFlinfo->manufacturerId == MV_SPANSION_MANF_ID)
    {
        switch (wpRegion)
        {
            case MV_WP_NONE:
                wpMask = MV_S25FL_STATUS_BP_NONE;
                break;

            case MV_WP_UPR_1OF128:
                DB(mvOsPrintf("%s WARNING: Invaild option for this flash chip!\n", __FUNCTION__);)
                return MV_NOT_SUPPORTED;

            case MV_WP_UPR_1OF64:
                wpMask = MV_S25FL_STATUS_BP_1_OF_64;
                break;

            case MV_WP_UPR_1OF32:
                wpMask = MV_S25FL_STATUS_BP_1_OF_32;
                break;

            case MV_WP_UPR_1OF16:
                wpMask = MV_S25FL_STATUS_BP_1_OF_16;
                break;

            case MV_WP_UPR_1OF8:
                wpMask = MV_S25FL_STATUS_BP_1_OF_8;
                break;

            case MV_WP_UPR_1OF4:
                wpMask = MV_S25FL_STATUS_BP_1_OF_4;
                break;

            case MV_WP_UPR_1OF2:
                wpMask = MV_S25FL_STATUS_BP_1_OF_2;
                break;

            case MV_WP_ALL:
                wpMask = MV_S25FL_STATUS_BP_ALL;
                break;


            default:
                DB(mvOsPrintf("%s WARNING: Invaild parameter WP region!\n", __FUNCTION__);)
                return MV_BAD_PARAM;
        }
    }
    else
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter Manufacturer ID!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* Verify that the SRWD bit is always set - register is s/w locked */
    wpMask |= MV_SFLASH_STATUS_REG_SRWD_MASK;

	return mvStatusRegSet(pFlinfo, wpMask);
}

/*******************************************************************************
* mvSFlashWpRegionGet - Get the Write-Protected region configured
*
* DESCRIPTION:
*       Get from the chip the Write-Protected region configured
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		pWpRegion: pointer to the variable to return the WP region in
*
* OUTPUT:
*		wpRegion: pointer to the variable holding the WP region configured
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashWpRegionGet (MV_SFLASH_INFO * pFlinfo, MV_SFLASH_WP_REGION * pWpRegion)
{
    MV_STATUS ret;
	MV_U8 reg;

    /* check for NULL pointer */
    if ((pFlinfo == NULL) || (pWpRegion == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    if ((ret = mvStatusRegGet(pFlinfo, &reg)) != MV_OK)
        return ret;

    /* Check if the chip is an ST flash; then WP supports only 3 bits */
    if (pFlinfo->manufacturerId == MV_M25PXXX_ST_MANF_ID)
    {
        switch ((reg & MV_M25P_STATUS_REG_WP_MASK))
        {
            case MV_M25P_STATUS_BP_NONE:
                *pWpRegion = MV_WP_NONE;
                break;

            case MV_M25P_STATUS_BP_1_OF_64:
                *pWpRegion = MV_WP_UPR_1OF64;
                break;

            case MV_M25P_STATUS_BP_1_OF_32:
                *pWpRegion = MV_WP_UPR_1OF32;
                break;

            case MV_M25P_STATUS_BP_1_OF_16:
                *pWpRegion = MV_WP_UPR_1OF16;
                break;

            case MV_M25P_STATUS_BP_1_OF_8:
                *pWpRegion = MV_WP_UPR_1OF8;
                break;

            case MV_M25P_STATUS_BP_1_OF_4:
                *pWpRegion = MV_WP_UPR_1OF4;
                break;

            case MV_M25P_STATUS_BP_1_OF_2:
                *pWpRegion = MV_WP_UPR_1OF2;
                break;

            case MV_M25P_STATUS_BP_ALL:
                *pWpRegion = MV_WP_ALL;
                break;

            default:
                DB(mvOsPrintf("%s WARNING: Unidentified WP region in h/w!\n", __FUNCTION__);)
                return MV_BAD_VALUE;
        }
    }
    /* check if the manufacturer is MXIC then the WP is 4bits */
    else if (pFlinfo->manufacturerId == MV_MXIC_MANF_ID)
    {
        switch ((reg & MV_MX25L_STATUS_REG_WP_MASK))
        {
            case MV_MX25L_STATUS_BP_NONE:
                *pWpRegion = MV_WP_NONE;
                break;

            case MV_MX25L_STATUS_BP_1_OF_128:
                *pWpRegion = MV_WP_UPR_1OF128;
                break;

            case MV_MX25L_STATUS_BP_1_OF_64:
                *pWpRegion = MV_WP_UPR_1OF64;
                break;

            case MV_MX25L_STATUS_BP_1_OF_32:
                *pWpRegion = MV_WP_UPR_1OF32;
                break;

            case MV_MX25L_STATUS_BP_1_OF_16:
                *pWpRegion = MV_WP_UPR_1OF16;
                break;

            case MV_MX25L_STATUS_BP_1_OF_8:
                *pWpRegion = MV_WP_UPR_1OF8;
                break;

            case MV_MX25L_STATUS_BP_1_OF_4:
                *pWpRegion = MV_WP_UPR_1OF4;
                break;

            case MV_MX25L_STATUS_BP_1_OF_2:
                *pWpRegion = MV_WP_UPR_1OF2;
                break;

            case MV_MX25L_STATUS_BP_ALL:
                *pWpRegion = MV_WP_ALL;
                break;

            default:
                DB(mvOsPrintf("%s WARNING: Unidentified WP region in h/w!\n", __FUNCTION__);)
                return MV_BAD_VALUE;
        }
    }
    /* Check if the chip is an SPANSION flash; then WP supports only 3 bits */
    else if (pFlinfo->manufacturerId == MV_SPANSION_MANF_ID)
    {
        switch ((reg & MV_S25FL_STATUS_REG_WP_MASK))
        {
            case MV_S25FL_STATUS_BP_NONE:
                *pWpRegion = MV_WP_NONE;
                break;

            case MV_S25FL_STATUS_BP_1_OF_64:
                *pWpRegion = MV_WP_UPR_1OF64;
                break;

            case MV_S25FL_STATUS_BP_1_OF_32:
                *pWpRegion = MV_WP_UPR_1OF32;
                break;

            case MV_S25FL_STATUS_BP_1_OF_16:
                *pWpRegion = MV_WP_UPR_1OF16;
                break;

            case MV_S25FL_STATUS_BP_1_OF_8:
                *pWpRegion = MV_WP_UPR_1OF8;
                break;

            case MV_S25FL_STATUS_BP_1_OF_4:
                *pWpRegion = MV_WP_UPR_1OF4;
                break;

            case MV_S25FL_STATUS_BP_1_OF_2:
                *pWpRegion = MV_WP_UPR_1OF2;
                break;

            case MV_S25FL_STATUS_BP_ALL:
                *pWpRegion = MV_WP_ALL;
                break;

            default:
                DB(mvOsPrintf("%s WARNING: Unidentified WP region in h/w!\n", __FUNCTION__);)
                return MV_BAD_VALUE;
        }
    }
    else
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter Manufacturer ID!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

	return MV_OK;
}

/*******************************************************************************
* mvSFlashStatRegLock - Lock the status register for writing - W/Vpp
*		pin should be low to take effect
*
* DESCRIPTION:
*       Lock the access to the Status Register for writing. This will
*		cause the flash to enter the hardware protection mode if the W/Vpp
*		is low. If the W/Vpp is hi, the chip will be in soft protection mode, but
*		the register will continue to be writable if WREN sequence was used.
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*		srLock: enable/disable (MV_TRUE/MV_FALSE) status registor lock mechanism
*
* OUTPUT:
*       None
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSFlashStatRegLock (MV_SFLASH_INFO * pFlinfo, MV_BOOL srLock)
{
    MV_STATUS ret;
	MV_U8 reg;

    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    if ((ret = mvStatusRegGet(pFlinfo, &reg)) != MV_OK)
        return ret;

	if (srLock)
		reg |= MV_SFLASH_STATUS_REG_SRWD_MASK;
	else
		reg &= ~MV_SFLASH_STATUS_REG_SRWD_MASK;

	return mvStatusRegSet(pFlinfo, reg);
}

/*******************************************************************************
* mvSFlashSizeGet - Get the size of the SPI flash
*
* DESCRIPTION:
*       based on the sector number and size of each sector calculate the total
*       size of the flash memory.
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*
* OUTPUT:
*       None.
*
* RETURN:
*       Size of the flash in bytes.
*
*
*******************************************************************************/
MV_U32 mvSFlashSizeGet (MV_SFLASH_INFO * pFlinfo)
{
    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return 0;
    }

    return (pFlinfo->sectorSize * pFlinfo->sectorNumber);
}

/*******************************************************************************
* mvSFlashPowerSaveEnter - Cause the falsh device to go into power save mode
*
* DESCRIPTION:
*       Enter a special power save mode.
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*
* OUTPUT:
*       None.
*
* RETURN:
*       Size of the flash in bytes.
*
*
*******************************************************************************/
MV_STATUS mvSFlashPowerSaveEnter(MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_DP_CMND_LENGTH];


    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return 0;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* check that power save mode is supported in the specific device */
    if (sflash[pFlinfo->index].opcdPwrSave == MV_SFLASH_NO_SPECIFIC_OPCD)
    {
        DB(mvOsPrintf("%s WARNING: Power save not supported for this device!\n", __FUNCTION__);)
        return MV_NOT_SUPPORTED;
    }

    cmd[0] = sflash[pFlinfo->index].opcdPwrSave;

    if ((ret = mvSpiWriteThenWrite(cmd, MV_SFLASH_DP_CMND_LENGTH, NULL, 0)) != MV_OK)
		return ret;

	return MV_OK;

}

/*******************************************************************************
* mvSFlashPowerSaveExit - Cause the falsh device to exit the power save mode
*
* DESCRIPTION:
*       Exit the deep power save mode.
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*
* OUTPUT:
*       None.
*
* RETURN:
*       Size of the flash in bytes.
*
*
*******************************************************************************/
MV_STATUS mvSFlashPowerSaveExit (MV_SFLASH_INFO * pFlinfo)
{
    MV_STATUS ret;
	MV_U8 cmd[MV_SFLASH_RES_CMND_LENGTH];


    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return 0;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return MV_BAD_PARAM;
    }

    /* check that power save mode is supported in the specific device */
    if (sflash[pFlinfo->index].opcdRES == MV_SFLASH_NO_SPECIFIC_OPCD)
    {
        DB(mvOsPrintf("%s WARNING: Read Electronic Signature not supported for this device!\n", __FUNCTION__);)
        return MV_NOT_SUPPORTED;
    }

    cmd[0] = sflash[pFlinfo->index].opcdRES;

    if ((ret = mvSpiWriteThenWrite(cmd, MV_SFLASH_RES_CMND_LENGTH, NULL, 0)) != MV_OK)
		return ret;

    /* add the delay needed for the device to wake up */
    mvOsDelay(MV_MXIC_DP_EXIT_DELAY);   /* 30 ms */

	return MV_OK;

}

/*******************************************************************************
* mvSFlashModelGet - Retreive the string with the device manufacturer and model
*
* DESCRIPTION:
*       Retreive the string with the device manufacturer and model
*
* INPUT:
*       pFlinfo: pointer to the Flash information structure
*
* OUTPUT:
*       None.
*
* RETURN:
*       pointer to the string indicating the device manufacturer and model
*
*
*******************************************************************************/
const MV_8 * mvSFlashModelGet (MV_SFLASH_INFO * pFlinfo)
{
    static const MV_8 * unknModel = (const MV_8 *)"Unknown";

    /* check for NULL pointer */
    if (pFlinfo == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return 0;
    }

    /* Protection - check if the model was detected */
    if (pFlinfo->index >= MV_ARRAY_SIZE(sflash))
    {
        DB(mvOsPrintf("%s WARNING: Invaild parameter index!\n", __FUNCTION__);)
        return unknModel;
    }

    return sflash[pFlinfo->index].deviceModel;
}

