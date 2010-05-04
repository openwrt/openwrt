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
#ifndef __INCmvTwsiH
#define __INCmvTwsiH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* need to update this includes */
#include "twsi/mvTwsiSpec.h"
#include "ctrlEnv/mvCtrlEnvLib.h"


/* The TWSI interface supports both 7-bit and 10-bit addressing.            */
/* This enumerator describes addressing type.                               */
typedef enum _mvTwsiAddrType
{
    ADDR7_BIT,                      /* 7 bit address    */
    ADDR10_BIT                      /* 10 bit address   */
}MV_TWSI_ADDR_TYPE;

/* This structure describes TWSI address.                                   */
typedef struct _mvTwsiAddr
{
    MV_U32              address;    /* address          */
    MV_TWSI_ADDR_TYPE   type;       /* Address type     */
}MV_TWSI_ADDR;

/* This structure describes a TWSI slave.                                   */
typedef struct _mvTwsiSlave
{
    MV_TWSI_ADDR	slaveAddr;
    MV_BOOL 		validOffset;		/* whether the slave has offset (i.e. Eeprom  etc.) 	*/
    MV_U32		offset;		/* offset in the slave.					*/
    MV_BOOL 		moreThen256;	/* whether the ofset is bigger then 256 		*/
}MV_TWSI_SLAVE;

/* This enumerator describes TWSI protocol commands.                        */
typedef enum _mvTwsiCmd
{
    MV_TWSI_WRITE,   /* TWSI write command - 0 according to spec   */
    MV_TWSI_READ   /* TWSI read command  - 1 according to spec */
}MV_TWSI_CMD;

MV_STATUS mvTwsiStartBitSet(MV_U8 chanNum);
MV_STATUS mvTwsiStopBitSet(MV_U8 chanNum);
MV_STATUS mvTwsiAddrSet(MV_U8 chanNum, MV_TWSI_ADDR *twsiAddr, MV_TWSI_CMD command);

MV_U32 mvTwsiInit(MV_U8 chanNum, MV_KHZ frequancy, MV_U32 Tclk, MV_TWSI_ADDR *twsiAddr, MV_BOOL generalCallEnable); 
MV_STATUS mvTwsiRead (MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize);
MV_STATUS mvTwsiWrite(MV_U8 chanNum, MV_TWSI_SLAVE *twsiSlave, MV_U8 *pBlock, MV_U32 blockSize);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvTwsiH */

