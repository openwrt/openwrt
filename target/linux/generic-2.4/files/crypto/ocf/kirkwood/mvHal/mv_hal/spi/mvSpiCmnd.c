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

/*#define MV_DEBUG*/
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif


/*******************************************************************************
* mvSpiReadAndWrite - Read and Write a buffer simultanuousely
*
* DESCRIPTION:
*       Transmit and receive a buffer over the SPI in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits.
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
MV_STATUS mvSpiReadAndWrite(MV_U8* pRxBuff, MV_U8* pTxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;

    /* check for null parameters */
    if ((pRxBuff == NULL) || (pTxBuff == NULL) || (buffSize == 0))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

	/* First assert the chip select */
	mvSpiCsAssert();

    ret = mvSpiReadWrite(pRxBuff, pTxBuff, buffSize);

	/* Finally deassert the chip select */
	mvSpiCsDeassert();

	return ret;
}

/*******************************************************************************
* mvSpiWriteThenWrite - Serialize a command followed by the data over the TX line
*
* DESCRIPTION:
*       Assert the chip select line. Transmit the command buffer followed by
*       the data buffer. Then deassert the CS line.
*
* INPUT:
*       pCmndBuff: Pointer to the command buffer to transmit
*       cmndSize: length of the command size
*		pTxDataBuff: Pointer to the data buffer to transmit
*		txDataSize: length of the data buffer
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS	mvSpiWriteThenWrite (MV_U8* pCmndBuff, MV_U32 cmndSize, MV_U8* pTxDataBuff,
                                 MV_U32 txDataSize)
{
    MV_STATUS ret = MV_OK, tempRet;

    /* check for null parameters */
#ifndef CONFIG_MARVELL
    if(NULL == pTxDataBuff)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }
#endif

    if (pCmndBuff == NULL)
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

	/* First assert the chip select */
	mvSpiCsAssert();

    /* first write the command */
    if ((cmndSize) && (pCmndBuff != NULL))
    {
        if ((tempRet = mvSpiWrite(pCmndBuff, cmndSize)) != MV_OK)
            ret = tempRet;
    }

    /* Then write the data buffer */
#ifndef CONFIG_MARVELL
    if (txDataSize)
#else
    if ((txDataSize) && (pTxDataBuff != NULL))
#endif
    {
        if ((tempRet = mvSpiWrite(pTxDataBuff, txDataSize)) != MV_OK)
            ret = tempRet;
    }

	/* Finally deassert the chip select */
	mvSpiCsDeassert();

	return ret;
}

/*******************************************************************************
* mvSpiWriteThenRead - Serialize a command then read a data buffer
*
* DESCRIPTION:
*       Assert the chip select line. Transmit the command buffer then read
*       the data buffer. Then deassert the CS line.
*
* INPUT:
*       pCmndBuff: Pointer to the command buffer to transmit
*       cmndSize: length of the command size
*		pRxDataBuff: Pointer to the buffer to read the data in
*		txDataSize: length of the data buffer
*
* OUTPUT:
*		pRxDataBuff: Pointer to the buffer holding the data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiWriteThenRead (MV_U8* pCmndBuff, MV_U32 cmndSize, MV_U8* pRxDataBuff,
                              MV_U32 rxDataSize,MV_U32 dummyBytesToRead)
{
    MV_STATUS ret = MV_OK, tempRet;
    MV_U8   dummyByte;

    /* check for null parameters */
    if ((pCmndBuff == NULL) && (pRxDataBuff == NULL))
    {
        mvOsPrintf("%s ERROR: Null pointer parameter!\n", __FUNCTION__);
        return MV_BAD_PARAM;
    }

	/* First assert the chip select */
	mvSpiCsAssert();

    /* first write the command */
    if ((cmndSize) && (pCmndBuff != NULL))
    {
        if ((tempRet = mvSpiWrite(pCmndBuff, cmndSize)) != MV_OK)
            ret = tempRet;
    }

    /* Read dummy bytes before real data.   */
    while(dummyBytesToRead)
    {
        mvSpiRead(&dummyByte,1);
        dummyBytesToRead--;
    }

    /* Then write the data buffer */
    if ((rxDataSize) && (pRxDataBuff != NULL))
    {
        if ((tempRet = mvSpiRead(pRxDataBuff, rxDataSize)) != MV_OK)
            ret = tempRet;
    }

	/* Finally deassert the chip select */
	mvSpiCsDeassert();

	return ret;
}

