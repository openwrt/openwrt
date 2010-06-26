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


#include "mvTwsi.h"
#include "mvTwsiSpec.h"
#include "cpu/mvCpu.h"


/*#define MV_DEBUG*/
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

static MV_VOID twsiIntFlgClr(MV_U8 chanNum);
static MV_BOOL twsiMainIntGet(MV_U8 chanNum);
static MV_VOID twsiAckBitSet(MV_U8 chanNum);
static MV_U32 twsiStsGet(MV_U8 chanNum);
static MV_VOID twsiReset(MV_U8 chanNum);
static MV_STATUS twsiAddr7BitSet(MV_U8 chanNum, MV_U32 deviceAddress,MV_TWSI_CMD command);
static MV_STATUS twsiAddr10BitSet(MV_U8 chanNum, MV_U32 deviceAddress,MV_TWSI_CMD command);
static MV_STATUS twsiDataTransmit(MV_U8 chanNum, MV_U8 *pBlock, MV_U32 blockSize);
static MV_STATUS twsiDataReceive(MV_U8 chanNum, MV_U8 *pBlock, MV_U32 blockSize);
static MV_STATUS twsiTargetOffsSet(MV_U8 chanNum, MV_U32 offset,MV_BOOL moreThen256);


static MV_BOOL twsiTimeoutChk(MV_U32 timeout, const MV_8 *pString)
{
	if(timeout >= TWSI_TIMEOUT_VALUE)
	{
		DB(mvOsPrintf("%s",pString));
		return MV_TRUE;
	}
	return MV_FALSE;
	
}
/*******************************************************************************
* mvTwsiStartBitSet - Set start bit on the bus
*
* DESCRIPTION:
*       This routine sets the start bit on the TWSI bus. 
*       The routine first checks for interrupt flag condition, then it sets 
*       the start bit  in the TWSI Control register. 
*       If the interrupt flag condition check previously was set, the function 
*       will clear it.
*       The function then wait for the start bit to be cleared by the HW. 
*       Then it waits for the interrupt flag to be set and eventually, the 
*       TWSI status is checked to be 0x8 or 0x10(repeated start bit).
*
* INPUT:
*       chanNum - TWSI channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK is start bit was set successfuly on the bus.
*       MV_FAIL if interrupt flag was set before setting start bit.
*
*******************************************************************************/
MV_STATUS mvTwsiStartBitSet(MV_U8 chanNum)
{
	MV_BOOL isIntFlag = MV_FALSE;
	MV_U32 timeout, temp;

	DB(mvOsPrintf("TWSI: mvTwsiStartBitSet \n"));
	/* check Int flag */
    	if(twsiMainIntGet(chanNum))
		isIntFlag = MV_TRUE;
	/* set start Bit */
    	temp = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), temp | TWSI_CONTROL_START_BIT);
	
	/* in case that the int flag was set before i.e. repeated start bit */
	if(isIntFlag){
		DB(mvOsPrintf("TWSI: mvTwsiStartBitSet repeated start Bit\n"));
		twsiIntFlgClr(chanNum);
	}
	
   	/* wait for interrupt */
	timeout = 0;
	while(!twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));
	
	/* check for timeout */	
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: mvTwsiStartBitSet ERROR - Start Clear bit TimeOut .\n"))
		return MV_TIMEOUT;


	/* check that start bit went down */
	if((MV_REG_READ(TWSI_CONTROL_REG(chanNum)) & TWSI_CONTROL_START_BIT) != 0)
	{
		mvOsPrintf("TWSI: mvTwsiStartBitSet ERROR - start bit didn't went down\n");
		return MV_FAIL;
	}	

	/* check the status */
	temp = twsiStsGet(chanNum);
	if(( temp != TWSI_START_CON_TRA ) && ( temp != TWSI_REPEATED_START_CON_TRA ))
	  {
		mvOsPrintf("TWSI: mvTwsiStartBitSet ERROR - status %x after Set Start Bit. \n",temp);
		return MV_FAIL;
	}

	return MV_OK;	

}

/*******************************************************************************
* mvTwsiStopBitSet - Set stop bit on the bus
*
* DESCRIPTION:
*       This routine set the stop bit on the TWSI bus. 
*       The function then wait for the stop bit to be cleared by the HW. 
*       Finally the function checks for status of 0xF8.
*
* INPUT:
*	chanNum - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE is stop bit was set successfuly on the bus.
*
*******************************************************************************/
MV_STATUS mvTwsiStopBitSet(MV_U8 chanNum)
{
    	MV_U32	timeout, temp;

    	/* Generate stop bit */
	temp = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
    	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), temp | TWSI_CONTROL_STOP_BIT);

	twsiIntFlgClr(chanNum);
		
   	/* wait for stop bit to come down */
	timeout = 0;
	while( ((MV_REG_READ(TWSI_CONTROL_REG(chanNum)) & TWSI_CONTROL_STOP_BIT) != 0) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: mvTwsiStopBitSet ERROR - Stop bit TimeOut .\n"))
		return MV_TIMEOUT;
	
	/* check that the stop bit went down */
	if((MV_REG_READ(TWSI_CONTROL_REG(chanNum)) & TWSI_CONTROL_STOP_BIT) != 0)	
	{
		mvOsPrintf("TWSI: mvTwsiStopBitSet ERROR - stop bit didn't went down. \n");
		return MV_FAIL;
	}
	
	/* check the status */
	temp = twsiStsGet(chanNum);
	if( temp != TWSI_NO_REL_STS_INT_FLAG_IS_KEPT_0){
		mvOsPrintf("TWSI: mvTwsiStopBitSet ERROR - status %x after Stop Bit. \n", temp);
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* twsiMainIntGet - Get twsi bit from main Interrupt cause.
*
* DESCRIPTION:
*       This routine returns the twsi interrupt flag value.
*       
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE is interrupt flag is set, MV_FALSE otherwise.
*
*******************************************************************************/
static MV_BOOL twsiMainIntGet(MV_U8 chanNum)
{
	MV_U32 temp;
	
	/* get the int flag bit */

	temp = MV_REG_READ(TWSI_CPU_MAIN_INT_CAUSE_REG);
	if (temp & (TWSI0_CPU_MAIN_INT_BIT << chanNum))
	    return MV_TRUE;
    
	return MV_FALSE;
}
/*******************************************************************************
* twsiIntFlgClr - Clear Interrupt flag.
*
* DESCRIPTION:
*       This routine clears the interrupt flag. It does NOT poll the interrupt
*       to make sure the clear. After clearing the interrupt, it waits for at 
*       least 1 miliseconds.
*
* INPUT:
*	chanNum - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static MV_VOID twsiIntFlgClr(MV_U8 chanNum)
{
	MV_U32 temp;

	/* wait for 1 mili to prevent TWSI register write after write problems */
   	mvOsDelay(1);
	/* clear the int flag bit */
	temp = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
    	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum),temp & ~(TWSI_CONTROL_INT_FLAG_SET));

	/* wait for 1 mili sec for the clear to take effect */
   	mvOsDelay(1);
	
	return;
}


/*******************************************************************************
* twsiAckBitSet - Set acknowledge bit on the bus
*
* DESCRIPTION:
*       This routine set the acknowledge bit on the TWSI bus.
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
static MV_VOID twsiAckBitSet(MV_U8 chanNum)
{
	MV_U32 temp;

	/*Set the Ack bit */
	temp = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
    	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), temp | TWSI_CONTROL_ACK);

	/* Add delay of 1ms */
	mvOsDelay(1);
	return;
}


/*******************************************************************************
* twsiInit - Initialize TWSI interface
*
* DESCRIPTION:
*       This routine:
*	-Reset the TWSI.
*	-Initialize the TWSI clock baud rate according to given frequancy
*	 parameter based on Tclk frequancy and enables TWSI slave.
*       -Set the ack bit.
*	-Assign the TWSI slave address according to the TWSI address Type.
*       
*
* INPUT:
*	chanNum - TWSI channel
*       frequancy - TWSI frequancy in KHz. (up to 100KHZ)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Actual frequancy.
*
*******************************************************************************/
MV_U32 mvTwsiInit(MV_U8 chanNum, MV_HZ frequancy, MV_U32 Tclk, MV_TWSI_ADDR *pTwsiAddr, MV_BOOL generalCallEnable)
{
    	MV_U32	n,m,freq,margin,minMargin = 0xffffffff;
	MV_U32	power;
    	MV_U32	actualFreq = 0,actualN = 0,actualM = 0,val;

	if(frequancy > 100000)
	{
		mvOsPrintf("Warning TWSI frequancy is too high, please use up tp 100Khz. \n");
	}

	DB(mvOsPrintf("TWSI: mvTwsiInit - Tclk = %d freq = %d\n",Tclk,frequancy));
    	/* Calucalte N and M for the TWSI clock baud rate */
    	for(n = 0 ; n < 8 ; n++)
    	{
        	for(m = 0 ; m < 16 ; m++)
        	{
            		power = 2 << n; /* power = 2^(n+1) */
            		freq = Tclk/(10*(m+1)*power);
            		margin = MV_ABS(frequancy - freq);
            		if(margin < minMargin)
            		{
                		minMargin   = margin;
                		actualFreq  = freq;
                		actualN     = n;
                		actualM     = m;
            		}
        	}
		}
	DB(mvOsPrintf("TWSI: mvTwsiInit - actN %d actM %d actFreq %d\n",actualN , actualM, actualFreq));
	/* Reset the TWSI logic */
	twsiReset(chanNum);

	/* Set the baud rate */
	val = ((actualM<< TWSI_BAUD_RATE_M_OFFS) | actualN << TWSI_BAUD_RATE_N_OFFS);
    	MV_REG_WRITE(TWSI_STATUS_BAUDE_RATE_REG(chanNum),val);

    	/* Enable the TWSI and slave */
	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), TWSI_CONTROL_ENA | TWSI_CONTROL_ACK); 

	/* set the TWSI slave address */
	if( pTwsiAddr->type == ADDR10_BIT )/* 10 Bit deviceAddress */
    	{
		/* writing the 2 most significant bits of the 10 bit address*/
		val = ((pTwsiAddr->address & TWSI_SLAVE_ADDR_10BIT_MASK) >> TWSI_SLAVE_ADDR_10BIT_OFFS );
		/* bits 7:3 must be 0x11110 */
		val |= TWSI_SLAVE_ADDR_10BIT_CONST;
		/* set GCE bit */
		if(generalCallEnable)
			val |= TWSI_SLAVE_ADDR_GCE_ENA;
		/* write slave address */
		MV_REG_WRITE(TWSI_SLAVE_ADDR_REG(chanNum),val);

         	/* writing the 8 least significant bits of the 10 bit address*/
        	val = (pTwsiAddr->address << TWSI_EXTENDED_SLAVE_OFFS) & TWSI_EXTENDED_SLAVE_MASK;  
        	MV_REG_WRITE(TWSI_EXTENDED_SLAVE_ADDR_REG(chanNum), val);
    	}
    	else /*7 bit address*/
    	{
		/* set the 7 Bits address */
        	MV_REG_WRITE(TWSI_EXTENDED_SLAVE_ADDR_REG(chanNum),0x0);
		val = (pTwsiAddr->address << TWSI_SLAVE_ADDR_7BIT_OFFS) & TWSI_SLAVE_ADDR_7BIT_MASK;
        	MV_REG_WRITE(TWSI_SLAVE_ADDR_REG(chanNum), val);
    	}

	/* unmask twsi int */
    val = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
	MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), val | TWSI_CONTROL_INT_ENA);
	/* Add delay of 1ms */
	mvOsDelay(1);
	
   return actualFreq;
} 


/*******************************************************************************
* twsiStsGet - Get the TWSI status value.
*
* DESCRIPTION:
*       This routine returns the TWSI status value.
*
* INPUT:
*	chanNum - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 - the TWSI status.
*
*******************************************************************************/
static MV_U32 twsiStsGet(MV_U8 chanNum)
{
    return MV_REG_READ(TWSI_STATUS_BAUDE_RATE_REG(chanNum));

}

/*******************************************************************************
* twsiReset - Reset the TWSI.
*
* DESCRIPTION:
*       Resets the TWSI logic and sets all TWSI registers to their reset values.
*
* INPUT:
*      chanNum - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*
*******************************************************************************/
static MV_VOID twsiReset(MV_U8 chanNum)
{
    	/* Reset the TWSI logic */
    	MV_REG_WRITE(TWSI_SOFT_RESET_REG(chanNum),0);

	/* wait for 2 mili sec */
   	mvOsDelay(2);

	return;
}




/******************************* POLICY ****************************************/



/*******************************************************************************
* mvTwsiAddrSet - Set address on TWSI bus.
*
* DESCRIPTION:
*       This function Set address (7 or 10 Bit address) on the Twsi Bus.
*
* INPUT:
*	chanNum - TWSI channel
*       pTwsiAddr - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
MV_STATUS mvTwsiAddrSet(MV_U8 chanNum, MV_TWSI_ADDR *pTwsiAddr, MV_TWSI_CMD command)
{
	DB(mvOsPrintf("TWSI: mvTwsiAddr7BitSet addr %x , type %d, cmd is %s\n",pTwsiAddr->address,\
		 			pTwsiAddr->type, ((command==MV_TWSI_WRITE)?"Write":"Read") ));
	/* 10 Bit address */
	if(pTwsiAddr->type == ADDR10_BIT)
	{
		return twsiAddr10BitSet(chanNum, pTwsiAddr->address,command);
	}
	/* 7 Bit address */
	else
	{
		return twsiAddr7BitSet(chanNum, pTwsiAddr->address,command);
	}

}

/*******************************************************************************
* twsiAddr10BitSet - Set 10 Bit address on TWSI bus.
*
* DESCRIPTION:
*       There are two address phases:
*       1) Write '11110' to data register bits [7:3] and 10-bit address MSB 
*          (bits [9:8]) to data register bits [2:1] plus a write(0) or read(1) bit 
*          to the Data register. Then it clears interrupt flag which drive 
*          the address on the TWSI bus. The function then waits for interrupt 
*          flag to be active and status 0x18 (write) or 0x40 (read) to be set.
*       2) write the rest of 10-bit address to data register and clears 
*          interrupt flag which drive the address on the TWSI bus. The 
*          function then waits for interrupt flag to be active and status 
*          0xD0 (write) or 0xE0 (read) to be set. 
*
* INPUT:
*	chanNum - TWSI channel
*       deviceAddress - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static MV_STATUS twsiAddr10BitSet(MV_U8 chanNum, MV_U32 deviceAddress,MV_TWSI_CMD command)
{
	MV_U32 val,timeout;

	/* writing the 2 most significant bits of the 10 bit address*/
	val = ((deviceAddress & TWSI_DATA_ADDR_10BIT_MASK) >> TWSI_DATA_ADDR_10BIT_OFFS );
	/* bits 7:3 must be 0x11110 */
	val |= TWSI_DATA_ADDR_10BIT_CONST;
	/* set command */
	val |= command;
	MV_REG_WRITE(TWSI_DATA_REG(chanNum), val);
	/* WA add a delay */
	mvOsDelay(1);

	/* clear Int flag */
	twsiIntFlgClr(chanNum);

	/* wait for Int to be Set */
	timeout = 0;
	while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiAddr10BitSet ERROR - 1st addr (10Bit) Int TimeOut.\n"))
		return MV_TIMEOUT;
	
	/* check the status */
	val = twsiStsGet(chanNum);
	if(( (val != TWSI_AD_PLS_RD_BIT_TRA_ACK_REC) && (command == MV_TWSI_READ ) ) || 
	   ( (val != TWSI_AD_PLS_WR_BIT_TRA_ACK_REC) && (command == MV_TWSI_WRITE) ))
	{
		mvOsPrintf("TWSI: twsiAddr10BitSet ERROR - status %x 1st addr (10 Bit) in %s mode.\n"\
						,val, ((command==MV_TWSI_WRITE)?"Write":"Read") );
		return MV_FAIL;
	}

	/* set 	8 LSB of the address */
	val = (deviceAddress << TWSI_DATA_ADDR_7BIT_OFFS) & TWSI_DATA_ADDR_7BIT_MASK;
	MV_REG_WRITE(TWSI_DATA_REG(chanNum), val);

	/* clear Int flag */
	twsiIntFlgClr(chanNum);

	/* wait for Int to be Set */
	timeout = 0;
	while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiAddr10BitSet ERROR - 2nd (10 Bit) Int TimOut.\n"))
		return MV_TIMEOUT;
	
	/* check the status */
	val = twsiStsGet(chanNum);
	if(( (val != TWSI_SEC_AD_PLS_RD_BIT_TRA_ACK_REC) && (command == MV_TWSI_READ ) ) || 
	   ( (val != TWSI_SEC_AD_PLS_WR_BIT_TRA_ACK_REC) && (command == MV_TWSI_WRITE) ))
	{
		mvOsPrintf("TWSI: twsiAddr10BitSet ERROR - status %x 2nd addr(10 Bit) in %s mode.\n"\
						,val, ((command==MV_TWSI_WRITE)?"Write":"Read") );
		return MV_FAIL;
	}
	
	return MV_OK;
}

/*******************************************************************************
* twsiAddr7BitSet - Set 7 Bit address on TWSI bus.
*
* DESCRIPTION:
*       This function writes 7 bit address plus a write or read bit to the 
*       Data register. Then it clears interrupt flag which drive the address on 
*       the TWSI bus. The function then waits for interrupt flag to be active
*       and status 0x18 (write) or 0x40 (read) to be set.
*
* INPUT:
*	chanNum - TWSI channel
*       deviceAddress - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static MV_STATUS twsiAddr7BitSet(MV_U8 chanNum, MV_U32 deviceAddress,MV_TWSI_CMD command)
{
	MV_U32 val,timeout;

	/* set the address */
	val = (deviceAddress << TWSI_DATA_ADDR_7BIT_OFFS) & TWSI_DATA_ADDR_7BIT_MASK;
	/* set command */
	val |= command;	
	MV_REG_WRITE(TWSI_DATA_REG(chanNum), val);
	/* WA add a delay */
	mvOsDelay(1);

	/* clear Int flag */
	twsiIntFlgClr(chanNum);

	/* wait for Int to be Set */
	timeout = 0;
	while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiAddr7BitSet ERROR - Addr (7 Bit) int TimeOut.\n"))
		return MV_TIMEOUT;
	
	/* check the status */
	val = twsiStsGet(chanNum);
	if(( (val != TWSI_AD_PLS_RD_BIT_TRA_ACK_REC) && (command == MV_TWSI_READ ) ) || 
	   ( (val != TWSI_AD_PLS_WR_BIT_TRA_ACK_REC) && (command == MV_TWSI_WRITE) ))
	{
		/* only in debug, since in boot we try to read the SPD of both DRAM, and we don't
			want error messeges in case DIMM doesn't exist. */
		DB(mvOsPrintf("TWSI: twsiAddr7BitSet ERROR - status %x addr (7 Bit) in %s mode.\n"\
						,val,((command==MV_TWSI_WRITE)?"Write":"Read") ));
		return MV_FAIL;
	}
	
	return MV_OK;
}

/*******************************************************************************
* twsiDataWrite - Trnasmit a data block over TWSI bus.
*
* DESCRIPTION:
*       This function writes a given data block to TWSI bus in 8 bit granularity.
*	first The function waits for interrupt flag to be active then
*       For each 8-bit data:
*        The function writes data to data register. It then clears 
*        interrupt flag which drives the data on the TWSI bus. 
*        The function then waits for interrupt flag to be active and status 
*        0x28 to be set. 
*      
*
* INPUT:
*	chanNum - TWSI channel
*       pBlock - Data block.
*	blockSize - number of chars in pBlock.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if transmiting the block completed succesfully,
*	MV_BAD_PARAM - if pBlock is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static MV_STATUS twsiDataTransmit(MV_U8 chanNum, MV_U8 *pBlock, MV_U32 blockSize)
{
	MV_U32 timeout, temp, blockSizeWr = blockSize;

	if(NULL == pBlock)
		return MV_BAD_PARAM;

	/* wait for Int to be Set */
	timeout = 0;
	while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiDataTransmit ERROR - Read Data Int TimeOut.\n"))
		return MV_TIMEOUT;

	while(blockSizeWr)
	{
		/* write the data*/
		MV_REG_WRITE(TWSI_DATA_REG(chanNum),(MV_U32)*pBlock);
		DB(mvOsPrintf("TWSI: twsiDataTransmit place = %d write %x \n",\
						blockSize - blockSizeWr, *pBlock));
		pBlock++;
		blockSizeWr--;

		twsiIntFlgClr(chanNum);

		/* wait for Int to be Set */
		timeout = 0;
		while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

		/* check for timeout */
		if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiDataTransmit ERROR - Read Data Int TimeOut.\n"))
			return MV_TIMEOUT;

		/* check the status */
		temp = twsiStsGet(chanNum);
		if(temp != TWSI_M_TRAN_DATA_BYTE_ACK_REC) 
		{
			mvOsPrintf("TWSI: twsiDataTransmit ERROR - status %x in write trans\n",temp);
			return MV_FAIL;
		}
		
	}

	return MV_OK;
}

/*******************************************************************************
* twsiDataReceive - Receive data block from TWSI bus.
*
* DESCRIPTION:
*       This function receive data block from TWSI bus in 8bit granularity 
*       into pBlock buffer.
*	first The function waits for interrupt flag to be active then
*       For each 8-bit data:
*        It clears the interrupt flag which allows the next data to be 
*        received from TWSI bus.
*	 The function waits for interrupt flag to be active,
*	 and status reg is 0x50. 
*	 Then the function reads data from data register, and copies it to 
*	 the given buffer. 
*
* INPUT:
*	chanNum - TWSI channel
*       blockSize - number of bytes to read.
*
* OUTPUT:
*       pBlock - Data block.
*
* RETURN:
*       MV_OK - if receive transaction completed succesfully,
*	MV_BAD_PARAM - if pBlock is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static MV_STATUS twsiDataReceive(MV_U8 chanNum, MV_U8 *pBlock, MV_U32 blockSize)
{
	MV_U32 timeout, temp, blockSizeRd = blockSize;
	if(NULL == pBlock)
		return MV_BAD_PARAM;

	/* wait for Int to be Set */
	timeout = 0;
	while( !twsiMainIntGet(chanNum) && (timeout++ < TWSI_TIMEOUT_VALUE));

	/* check for timeout */
	if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiDataReceive ERROR - Read Data int Time out .\n"))
		return MV_TIMEOUT;

	while(blockSizeRd)
	{
		if(blockSizeRd == 1)
		{
			/* clear ack and Int flag */
			temp = MV_REG_READ(TWSI_CONTROL_REG(chanNum));
			temp &=  ~(TWSI_CONTROL_ACK);
			MV_REG_WRITE(TWSI_CONTROL_REG(chanNum), temp);
		}
		twsiIntFlgClr(chanNum);
		/* wait for Int to be Set */
		timeout = 0;
		while( (!twsiMainIntGet(chanNum)) && (timeout++ < TWSI_TIMEOUT_VALUE));

		/* check for timeout */
		if(MV_TRUE == twsiTimeoutChk(timeout,"TWSI: twsiDataReceive ERROR - Read Data Int Time out .\n"))
			return MV_TIMEOUT;

		/* check the status */
		temp = twsiStsGet(chanNum);
		if((temp != TWSI_M_REC_RD_DATA_ACK_TRA) && (blockSizeRd !=1))
		{
			mvOsPrintf("TWSI: twsiDataReceive ERROR - status %x in read trans \n",temp);
			return MV_FAIL;
		}
		else if((temp != TWSI_M_REC_RD_DATA_ACK_NOT_TRA) && (blockSizeRd ==1))
		{
			mvOsPrintf("TWSI: twsiDataReceive ERROR - status %x in Rd Terminate\n",temp);
			return MV_FAIL;
		}
		
		/* read the data*/
		*pBlock = (MV_U8)MV_REG_READ(TWSI_DATA_REG(chanNum));
		DB(mvOsPrintf("TWSI: twsiDataReceive  place %d read %x \n",\
						blockSize - blockSizeRd,*pBlock));
		pBlock++;
		blockSizeRd--;
	}

	return MV_OK;
}



/*******************************************************************************
* twsiTargetOffsSet - Set TWST target offset on TWSI bus.
*
* DESCRIPTION:
*       The function support TWSI targets that have inside address space (for
*       example EEPROMs). The function:
*       1) Convert the given offset into pBlock and size.
*		in case the offset should be set to a TWSI slave which support 
*		more then 256 bytes offset, the offset setting will be done
*		in 2 transactions.
*       2) Use twsiDataTransmit to place those on the bus.
*
* INPUT:
*	chanNum - TWSI channel
*       offset - offset to be set on the EEPROM device.
*	moreThen256 - whether the EEPROM device support more then 256 byte offset. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the offset completed succesfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static MV_STATUS twsiTargetOffsSet(MV_U8 chanNum, MV_U32 offset, MV_BOOL moreThen256)
{
	MV_U8 offBlock[2];
	MV_U32 offSize;

	if(moreThen256 == MV_TRUE)
	{
		offBlock[0] = (offset >> 8) & 0xff;
		offBlock[1] = offset & 0xff;
		offSize = 2;
	}
	else
	{
		offBlock[0] = offset & 0xff;
		offSize = 1;
	}
	DB(mvOsPrintf("TWSI: twsiTargetOffsSet offSize = %x addr1 = %x addr2 = %x\n",\
							offSize,offBlock[0],offBlock[1]));
	return twsiDataTransmit(chanNum, offBlock, offSize);

}

/*******************************************************************************
* mvTwsiRead - Read data block from a TWSI Slave.
*
* DESCRIPTION:
*       The function calls the following functions:
*       -) mvTwsiStartBitSet();
*	if(EEPROM device)
*       	-) mvTwsiAddrSet(w);
*       	-) twsiTargetOffsSet();
*       	-) mvTwsiStartBitSet();
*       -) mvTwsiAddrSet(r);
*       -) twsiDataReceive();
*       -) mvTwsiStopBitSet();
*
* INPUT:
*	chanNum - TWSI channel
*      	pTwsiSlave - Twsi Slave structure. 
*       blockSize - number of bytes to read.	
*
* OUTPUT:
*      	pBlock - Data block.
*
* RETURN:
*       MV_OK - if EEPROM read transaction completed succesfully,
* 	MV_BAD_PARAM - if pBlock is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
MV_STATUS mvTwsiRead(MV_U8 chanNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	if((NULL == pBlock) || (NULL == pTwsiSlave))
		return MV_BAD_PARAM;
	if(MV_OK != mvTwsiStartBitSet(chanNum))
	{
		mvTwsiStopBitSet(chanNum);
		 return MV_FAIL;
	}
	
	DB(mvOsPrintf("TWSI: mvTwsiEepromRead after mvTwsiStartBitSet\n"));
	
	/* in case offset exsist (i.e. eeprom ) */
	if(MV_TRUE == pTwsiSlave->validOffset)
	{
		if(MV_OK != mvTwsiAddrSet(chanNum, &(pTwsiSlave->slaveAddr), MV_TWSI_WRITE)) 
		{
			mvTwsiStopBitSet(chanNum);
			return MV_FAIL;
		} 
		DB(mvOsPrintf("TWSI: mvTwsiEepromRead after mvTwsiAddrSet\n"));
		if(MV_OK != twsiTargetOffsSet(chanNum, pTwsiSlave->offset, pTwsiSlave->moreThen256)) 
		{
			mvTwsiStopBitSet(chanNum);
			return MV_FAIL;
		}
		DB(mvOsPrintf("TWSI: mvTwsiEepromRead after twsiTargetOffsSet\n"));
		if(MV_OK != mvTwsiStartBitSet(chanNum)) 
		{
			mvTwsiStopBitSet(chanNum);
			return MV_FAIL;
		}
		DB(mvOsPrintf("TWSI: mvTwsiEepromRead after mvTwsiStartBitSet\n"));
	}
	if(MV_OK != mvTwsiAddrSet(chanNum, &(pTwsiSlave->slaveAddr), MV_TWSI_READ)) 
	{
		mvTwsiStopBitSet(chanNum);
		return MV_FAIL;
	} 
	DB(mvOsPrintf("TWSI: mvTwsiEepromRead after mvTwsiAddrSet\n"));
	if(MV_OK != twsiDataReceive(chanNum, pBlock, blockSize))
	{
		mvTwsiStopBitSet(chanNum);
		return MV_FAIL;
	}
	DB(mvOsPrintf("TWSI: mvTwsiEepromRead after twsiDataReceive\n"));

	if(MV_OK != mvTwsiStopBitSet(chanNum))
	{
		return MV_FAIL;
	}

	twsiAckBitSet(chanNum);

	DB(mvOsPrintf("TWSI: mvTwsiEepromRead after mvTwsiStopBitSet\n"));

	return MV_OK;
}

/*******************************************************************************
* mvTwsiWrite - Write data block to a TWSI Slave.
*
* DESCRIPTION:
*       The function calls the following functions:
*       -) mvTwsiStartBitSet();
*       -) mvTwsiAddrSet();
*	-)if(EEPROM device)
*       	-) twsiTargetOffsSet();
*       -) twsiDataTransmit();
*       -) mvTwsiStopBitSet();
*
* INPUT:
*	chanNum - TWSI channel
*      	eepromAddress - eeprom address. 
*       blockSize - number of bytes to write.	
*      	pBlock - Data block.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_OK - if EEPROM read transaction completed succesfully.
*	MV_BAD_PARAM - if pBlock is NULL,
*	MV_FAIL otherwmise.
*
* NOTE: Part of the EEPROM, required that the offset will be aligned to the
*	max write burst supported.
*******************************************************************************/
MV_STATUS mvTwsiWrite(MV_U8 chanNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *pBlock, MV_U32 blockSize)
{
	if((NULL == pBlock) || (NULL == pTwsiSlave))
		return MV_BAD_PARAM;

	if(MV_OK != mvTwsiStartBitSet(chanNum)) 
	{
		mvTwsiStopBitSet(chanNum);
		return MV_FAIL;
	}

	DB(mvOsPrintf("TWSI: mvTwsiEepromWrite after mvTwsiStartBitSet\n"));
	if(MV_OK != mvTwsiAddrSet(chanNum, &(pTwsiSlave->slaveAddr), MV_TWSI_WRITE))
	{
		mvTwsiStopBitSet(chanNum);
		return MV_FAIL;
	}
	DB(mvOsPrintf("TWSI :mvTwsiEepromWrite after mvTwsiAddrSet\n"));

	/* in case offset exsist (i.e. eeprom ) */
	if(MV_TRUE == pTwsiSlave->validOffset)
	{
		if(MV_OK != twsiTargetOffsSet(chanNum, pTwsiSlave->offset, pTwsiSlave->moreThen256)) 
		{
			mvTwsiStopBitSet(chanNum);
			return MV_FAIL;
		}
		DB(mvOsPrintf("TWSI: mvTwsiEepromWrite after twsiTargetOffsSet\n"));
	}
	if(MV_OK != twsiDataTransmit(chanNum, pBlock, blockSize)) 
	{
		mvTwsiStopBitSet(chanNum);
		return MV_FAIL;
	}
	DB(mvOsPrintf("TWSI: mvTwsiEepromWrite after twsiDataTransmit\n"));
	if(MV_OK != mvTwsiStopBitSet(chanNum)) 
	{
		return MV_FAIL;
	}
	DB(mvOsPrintf("TWSI: mvTwsiEepromWrite after mvTwsiStopBitSet\n"));

	return MV_OK;
}
