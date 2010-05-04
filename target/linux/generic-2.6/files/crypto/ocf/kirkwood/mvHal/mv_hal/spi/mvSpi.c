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

#include "spi/mvSpi.h"
#include "spi/mvSpiSpec.h"

#include "ctrlEnv/mvCtrlEnvLib.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#define mvOsPrintf printf
#else
#define DB(x)
#endif


/*******************************************************************************
* mvSpi16bitDataTxRx - Transmt and receive data
*
* DESCRIPTION:
*       Tx data and block waiting for data to be transmitted
*
********************************************************************************/
static MV_STATUS mvSpi16bitDataTxRx (MV_U16 txData, MV_U16 * pRxData)
{
    MV_U32 i;
    MV_BOOL ready = MV_FALSE;

    /* First clear the bit in the interrupt cause register */
    MV_REG_WRITE(MV_SPI_INT_CAUSE_REG, 0x0);

    /* Transmit data */
    MV_REG_WRITE(MV_SPI_DATA_OUT_REG, MV_16BIT_LE(txData));

    /* wait with timeout for memory ready */
    for (i=0; i<MV_SPI_WAIT_RDY_MAX_LOOP; i++)
    {
        if (MV_REG_READ(MV_SPI_INT_CAUSE_REG))
        {
            ready = MV_TRUE;
            break;
        }
#ifdef MV_SPI_SLEEP_ON_WAIT
        mvOsSleep(1);
#endif /* MV_SPI_SLEEP_ON_WAIT */
    }

    if (!ready)
        return MV_TIMEOUT;

    /* check that the RX data is needed */
    if (pRxData)
    {
    	if ((MV_U32)pRxData &  0x1) /* check if address is not alligned to 16bit */
    	{
#if defined(MV_CPU_LE)
    		/* perform the data write to the buffer in two stages with 8bit each */
    		MV_U8 * bptr = (MV_U8*)pRxData;
    		MV_U16 data = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG));
    		*bptr = (data & 0xFF);
    		++bptr;
    		*bptr = ((data >> 8) & 0xFF);

#elif defined(MV_CPU_BE)

    		/* perform the data write to the buffer in two stages with 8bit each */
    		MV_U8 * bptr = (MV_U8 *)pRxData;
    		MV_U16 data = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG));
    		*bptr = ((data >> 8) & 0xFF);
    		++bptr;
    		*bptr = (data & 0xFF);

#else
    #error "CPU endianess isn't defined!\n"
#endif

    	}
    	else
        	*pRxData = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG));
    }

    return MV_OK;
}


/*******************************************************************************
* mvSpi8bitDataTxRx - Transmt and receive data (8bits)
*
* DESCRIPTION:
*       Tx data and block waiting for data to be transmitted
*
********************************************************************************/
static MV_STATUS mvSpi8bitDataTxRx (MV_U8 txData, MV_U8 * pRxData)
{
    MV_U32 i;
    MV_BOOL ready = MV_FALSE;

    /* First clear the bit in the interrupt cause register */
    MV_REG_WRITE(MV_SPI_INT_CAUSE_REG, 0x0);

    /* Transmit data */
    MV_REG_WRITE(MV_SPI_DATA_OUT_REG, txData);

    /* wait with timeout for memory ready */
    for (i=0; i<MV_SPI_WAIT_RDY_MAX_LOOP; i++)
    {
        if (MV_REG_READ(MV_SPI_INT_CAUSE_REG))
        {
            ready = MV_TRUE;
            break;
        }
#ifdef MV_SPI_SLEEP_ON_WAIT
        mvOsSleep(1);
#endif /* MV_SPI_SLEEP_ON_WAIT */
    }

    if (!ready)
        return MV_TIMEOUT;

    /* check that the RX data is needed */
    if (pRxData)
    	*pRxData = MV_REG_READ(MV_SPI_DATA_IN_REG);

    return MV_OK;
}

/*
#####################################################################################
#####################################################################################
*/

/*******************************************************************************
* mvSpiInit - Initialize the SPI controller
*
* DESCRIPTION:
*       Perform the neccessary initialization in order to be able to send an
*		receive over the SPI interface.
*
* INPUT:
*       serialBaudRate: Baud rate (SPI clock frequency)
*		use16BitMode: Whether to use 2bytes (MV_TRUE) or 1bytes (MV_FALSE)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiInit	(MV_U32 serialBaudRate)
{
    MV_STATUS ret;

    /* Set the serial clock */
    if ((ret = mvSpiBaudRateSet(serialBaudRate)) != MV_OK)
        return ret;

    /* For devices in which the SPI is muxed on the MPP with other interfaces*/
    mvMPPConfigToSPI();

	/* Configure the default SPI mode to be 16bit */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* Fix ac timing on SPI in 6183, 6183L and 78x00 only */
	if ( (mvCtrlModelGet() == MV_6183_DEV_ID) ||
		 (mvCtrlModelGet() == MV_6183L_DEV_ID) ||
		(mvCtrlModelGet() == MV_78100_DEV_ID) ||
		(mvCtrlModelGet() == MV_78200_DEV_ID) ||
		(mvCtrlModelGet() == MV_76100_DEV_ID))
	    MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG, BIT14);

    /* Verify that the CS is deasserted */
    mvSpiCsDeassert();

    return MV_OK;
}

/*******************************************************************************
* mvSpiBaudRateSet - Set the Frequency of the SPI clock
*
* DESCRIPTION:
*       Set the Prescale bits to adapt to the requested baud rate (the clock
*       used for thr SPI).
*
* INPUT:
*       serialBaudRate: Baud rate (SPI clock frequency)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiBaudRateSet (MV_U32 serialBaudRate)
{
    MV_U8 i;
	/* MV_U8 preScale[32] = {1, 1, 2, 3, 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
						  2, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
	*/
	MV_U8 preScale[14] = { 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
	MV_U8 bestPrescaleIndx = 100;
	MV_U32 minBaudOffset = 0xFFFFFFFF;
	MV_U32 cpuClk = mvBoardTclkGet(); /*mvCpuPclkGet();*/
	MV_U32 tempReg;

	/* Find the best prescale configuration - less or equal */
	for (i=0; i<14; i++)
	{
		/* check for higher - irrelevent */
		if ((cpuClk / preScale[i]) > serialBaudRate)
			continue;

		/* check for exact fit */
		if ((cpuClk / preScale[i]) == serialBaudRate)
		{
			bestPrescaleIndx = i;
			break;
		}

		/* check if this is better than the previous one */
		if ((serialBaudRate - (cpuClk / preScale[i])) < minBaudOffset)
		{
			minBaudOffset = (serialBaudRate - (cpuClk / preScale[i]));
			bestPrescaleIndx = i;
		}
	}

	if (bestPrescaleIndx > 14)
    {
        mvOsPrintf("%s ERROR: SPI baud rate prescale error!\n", __FUNCTION__);
		return MV_OUT_OF_RANGE;
    }

	/* configure the Prescale */
	tempReg = MV_REG_READ(MV_SPI_IF_CONFIG_REG);
	tempReg = ((tempReg & ~MV_SPI_CLK_PRESCALE_MASK) | (bestPrescaleIndx + 0x12));
	MV_REG_WRITE(MV_SPI_IF_CONFIG_REG, tempReg);

    return MV_OK;
}

/*******************************************************************************
* mvSpiCsAssert - Assert the Chip Select pin indicating a new transfer
*
* DESCRIPTION:
*       Assert The chip select - used to select an external SPI device
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
********************************************************************************/
MV_VOID mvSpiCsAssert(MV_VOID)
{
    /* For devices in which the SPI is muxed on the MPP with other interfaces*/
    mvMPPConfigToSPI();
    mvOsUDelay(1);
    MV_REG_BIT_SET(MV_SPI_IF_CTRL_REG, MV_SPI_CS_ENABLE_MASK);
}

/*******************************************************************************
* mvSpiCsDeassert - DeAssert the Chip Select pin indicating the end of a
*				  SPI transfer sequence
*
* DESCRIPTION:
*       DeAssert the chip select pin
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
********************************************************************************/
MV_VOID mvSpiCsDeassert(MV_VOID)
{
	MV_REG_BIT_RESET(MV_SPI_IF_CTRL_REG, MV_SPI_CS_ENABLE_MASK);

    /* For devices in which the SPI is muxed on the MPP with other interfaces*/
    mvMPPConfigToDefault();
}

/*******************************************************************************
* mvSpiRead - Read a buffer over the SPI interface
*
* DESCRIPTION:
*       Receive (read) a buffer over the SPI interface in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. Chip select is not
*       handled at this level.
*
* INPUT:
*		pRxBuff: Pointer to the buffer to hold the received data
*		buffSize: length of the pRxBuff
*
* OUTPUT:
*		pRxBuff: Pointer to the buffer with the received data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiRead	(MV_U8* pRxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U32 bytesLeft = buffSize;
	MV_U16* rxPtr = (MV_U16*)pRxBuff;

    /* check for null parameters */
    if (pRxBuff == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Check that the buffer pointer and the buffer size are 16bit aligned */
    if ((((MV_U32)buffSize & 1) == 0) && (((MV_U32)pRxBuff & 1) == 0))
    {
	/* Verify that the SPI mode is in 16bit mode */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX as long we have complete 16bit chunks */
	while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE)
	{
		/* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi16bitDataTxRx(MV_SPI_DUMMY_WRITE_16BITS, rxPtr)) != MV_OK)
			return ret;

		/* increment the pointers */
		rxPtr++;
		bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
	}

    }
    else
    {
	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */
	while (bytesLeft > 0)
	{
        	/* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi8bitDataTxRx(MV_SPI_DUMMY_WRITE_8BITS, pRxBuff)) != MV_OK)
			return ret;
		/* increment the pointers */
		pRxBuff++;
		bytesLeft--;
	}
    }

	return MV_OK;
}

/*******************************************************************************
* mvSpiWrite - Transmit a buffer over the SPI interface
*
* DESCRIPTION:
*       Transmit a buffer over the SPI interface in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. No chip select
*       action is taken.
*
* INPUT:
*		pTxBuff: Pointer to the buffer holding the TX data
*		buffSize: length of the pTxBuff
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiWrite(MV_U8* pTxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U32 bytesLeft = buffSize;
	MV_U16* txPtr = (MV_U16*)pTxBuff;

    /* check for null parameters */
    if (pTxBuff == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Check that the buffer pointer and the buffer size are 16bit aligned */
    if ((((MV_U32)buffSize & 1) == 0) && (((MV_U32)pTxBuff & 1) == 0))
    {
	/* Verify that the SPI mode is in 16bit mode */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX as long we have complete 16bit chunks */
	while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE)
	{
        /* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi16bitDataTxRx(*txPtr, NULL)) != MV_OK)
			return ret;

		/* increment the pointers */
		txPtr++;
		bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
	}
    }
    else
    {

	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */
	while (bytesLeft > 0)
	{
		/* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi8bitDataTxRx(*pTxBuff, NULL)) != MV_OK)
			return ret;

		/* increment the pointers */
		pTxBuff++;
		bytesLeft--;
	}
    }

	return MV_OK;
}


/*******************************************************************************
* mvSpiReadWrite - Read and Write a buffer simultanuosely
*
* DESCRIPTION:
*       Transmit and receive a buffer over the SPI in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. The SPI chip
*       select is not handled implicitely.
*
* INPUT:
*       pRxBuff: Pointer to the buffer to write the RX info in
*		pTxBuff: Pointer to the buffer holding the TX info
*		buffSize: length of both the pTxBuff and pRxBuff
*
* OUTPUT:
*       pRxBuff: Pointer of the buffer holding the RX data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiReadWrite(MV_U8* pRxBuff, MV_U8* pTxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
    MV_U32 bytesLeft = buffSize;
    MV_U16* txPtr = (MV_U16*)pTxBuff;
    MV_U16* rxPtr = (MV_U16*)pRxBuff;

    /* check for null parameters */
    if ((pRxBuff == NULL) || (pTxBuff == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

    /* Check that the buffer pointer and the buffer size are 16bit aligned */
    if ((((MV_U32)buffSize & 1) == 0) && (((MV_U32)pTxBuff & 1) == 0) && (((MV_U32)pRxBuff & 1) == 0))
    {
	/* Verify that the SPI mode is in 16bit mode */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX as long we have complete 16bit chunks */
	while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE)
	{
        /* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi16bitDataTxRx(*txPtr, rxPtr)) != MV_OK)
			return ret;

		/* increment the pointers */
		txPtr++;
		rxPtr++;
		bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
	}
    }
    else
    {
	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG, MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */
	while (bytesLeft > 0)
	{
		/* Transmitted and wait for the transfer to be completed */
		if ( (ret = mvSpi8bitDataTxRx(*pTxBuff, pRxBuff) ) != MV_OK)
			return ret;
		pRxBuff++;
		pTxBuff++;
		bytesLeft--;
	}
    }

	return MV_OK;
}


