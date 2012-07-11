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

#ifndef __mvCesaRegs_h__
#define __mvCesaRegs_h__

#include "mvTypes.h"

typedef struct
{
    /* word 0 */
    MV_U32  config;
    /* word 1 */    
    MV_U16  cryptoSrcOffset;
    MV_U16  cryptoDstOffset;
    /* word 2 */    
    MV_U16  cryptoDataLen;
    MV_U16  reserved1;
    /* word 3 */
    MV_U16  cryptoKeyOffset;
    MV_U16  reserved2;
    /* word 4 */
    MV_U16  cryptoIvOffset;
    MV_U16  cryptoIvBufOffset;
    /* word 5 */
    MV_U16  macSrcOffset;
    MV_U16  macTotalLen;
    /* word 6 */
    MV_U16  macDigestOffset;
    MV_U16  macDataLen;
    /* word 7 */
    MV_U16  macInnerIvOffset;
    MV_U16  macOuterIvOffset;

} MV_CESA_DESC;

/* operation */
typedef enum 
{
    MV_CESA_MAC_ONLY         = 0,
    MV_CESA_CRYPTO_ONLY      = 1,
    MV_CESA_MAC_THEN_CRYPTO  = 2,
    MV_CESA_CRYPTO_THEN_MAC  = 3,
    
    MV_CESA_MAX_OPERATION

} MV_CESA_OPERATION;

#define MV_CESA_OPERATION_OFFSET        0
#define MV_CESA_OPERATION_MASK          (0x3 << MV_CESA_OPERATION_OFFSET)

/* mac algorithm */
typedef enum 
{   
    MV_CESA_MAC_NULL        = 0,
    MV_CESA_MAC_MD5         = 4,
    MV_CESA_MAC_SHA1        = 5,
    MV_CESA_MAC_HMAC_MD5    = 6,
    MV_CESA_MAC_HMAC_SHA1   = 7,

} MV_CESA_MAC_MODE;

#define MV_CESA_MAC_MODE_OFFSET         4
#define MV_CESA_MAC_MODE_MASK           (0x7 << MV_CESA_MAC_MODE_OFFSET)

typedef enum
{
    MV_CESA_MAC_DIGEST_FULL = 0,
    MV_CESA_MAC_DIGEST_96B  = 1,

} MV_CESA_MAC_DIGEST_SIZE;

#define MV_CESA_MAC_DIGEST_SIZE_BIT     7
#define MV_CESA_MAC_DIGEST_SIZE_MASK    (1 << MV_CESA_MAC_DIGEST_SIZE_BIT)


typedef enum 
{
    MV_CESA_CRYPTO_NULL = 0,
    MV_CESA_CRYPTO_DES  = 1,
    MV_CESA_CRYPTO_3DES = 2,
    MV_CESA_CRYPTO_AES  = 3,

} MV_CESA_CRYPTO_ALG;

#define MV_CESA_CRYPTO_ALG_OFFSET       8
#define MV_CESA_CRYPTO_ALG_MASK         (0x3 << MV_CESA_CRYPTO_ALG_OFFSET)


/* direction */
typedef enum 
{
    MV_CESA_DIR_ENCODE = 0,
    MV_CESA_DIR_DECODE = 1,

} MV_CESA_DIRECTION;

#define MV_CESA_DIRECTION_BIT           12
#define MV_CESA_DIRECTION_MASK          (1 << MV_CESA_DIRECTION_BIT)

/* crypto IV mode */
typedef enum 
{
    MV_CESA_CRYPTO_ECB = 0,
    MV_CESA_CRYPTO_CBC = 1,

    /* NO HW Support */
    MV_CESA_CRYPTO_CTR = 10,

} MV_CESA_CRYPTO_MODE;

#define MV_CESA_CRYPTO_MODE_BIT         16
#define MV_CESA_CRYPTO_MODE_MASK        (1 << MV_CESA_CRYPTO_MODE_BIT)         

/* 3DES mode */
typedef enum 
{
    MV_CESA_CRYPTO_3DES_EEE = 0,
    MV_CESA_CRYPTO_3DES_EDE = 1,

} MV_CESA_CRYPTO_3DES_MODE;

#define MV_CESA_CRYPTO_3DES_MODE_BIT    20
#define MV_CESA_CRYPTO_3DES_MODE_MASK   (1 << MV_CESA_CRYPTO_3DES_MODE_BIT)


/* AES Key Length */
typedef enum 
{
    MV_CESA_CRYPTO_AES_KEY_128 = 0,
    MV_CESA_CRYPTO_AES_KEY_192 = 1,
    MV_CESA_CRYPTO_AES_KEY_256 = 2,

} MV_CESA_CRYPTO_AES_KEY_LEN;

#define MV_CESA_CRYPTO_AES_KEY_LEN_OFFSET   24
#define MV_CESA_CRYPTO_AES_KEY_LEN_MASK     (0x3 << MV_CESA_CRYPTO_AES_KEY_LEN_OFFSET)

/* Fragmentation mode */
typedef enum 
{
    MV_CESA_FRAG_NONE   = 0,
    MV_CESA_FRAG_FIRST  = 1,
    MV_CESA_FRAG_LAST   = 2,
    MV_CESA_FRAG_MIDDLE = 3,

} MV_CESA_FRAG_MODE;

#define MV_CESA_FRAG_MODE_OFFSET            30
#define MV_CESA_FRAG_MODE_MASK              (0x3 << MV_CESA_FRAG_MODE_OFFSET)
/*---------------------------------------------------------------------------*/

/********** Security Accelerator Command Register **************/
#define MV_CESA_CMD_REG                     (MV_CESA_REG_BASE + 0xE00)

#define MV_CESA_CMD_CHAN_ENABLE_BIT         0  
#define MV_CESA_CMD_CHAN_ENABLE_MASK        (1 << MV_CESA_CMD_CHAN_ENABLE_BIT)

#define MV_CESA_CMD_CHAN_DISABLE_BIT        2  
#define MV_CESA_CMD_CHAN_DISABLE_MASK       (1 << MV_CESA_CMD_CHAN_DISABLE_BIT)  

/********** Security Accelerator Descriptor Pointers Register **********/
#define MV_CESA_CHAN_DESC_OFFSET_REG        (MV_CESA_REG_BASE + 0xE04)

/********** Security Accelerator Configuration Register **********/
#define MV_CESA_CFG_REG                     (MV_CESA_REG_BASE + 0xE08)

#define MV_CESA_CFG_STOP_DIGEST_ERR_BIT     0
#define MV_CESA_CFG_STOP_DIGEST_ERR_MASK    (1 << MV_CESA_CFG_STOP_DIGEST_ERR_BIT)

#define MV_CESA_CFG_WAIT_DMA_BIT            7
#define MV_CESA_CFG_WAIT_DMA_MASK           (1 << MV_CESA_CFG_WAIT_DMA_BIT)
          
#define MV_CESA_CFG_ACT_DMA_BIT             9
#define MV_CESA_CFG_ACT_DMA_MASK            (1 << MV_CESA_CFG_ACT_DMA_BIT)

#define MV_CESA_CFG_CHAIN_MODE_BIT          11
#define MV_CESA_CFG_CHAIN_MODE_MASK         (1 << MV_CESA_CFG_CHAIN_MODE_BIT)

/********** Security Accelerator Status Register ***********/
#define MV_CESA_STATUS_REG                  (MV_CESA_REG_BASE + 0xE0C)

#define MV_CESA_STATUS_ACTIVE_BIT           0
#define MV_CESA_STATUS_ACTIVE_MASK          (1 << MV_CESA_STATUS_ACTIVE_BIT)

#define MV_CESA_STATUS_DIGEST_ERR_BIT       8
#define MV_CESA_STATUS_DIGEST_ERR_MASK      (1 << MV_CESA_STATUS_DIGEST_ERR_BIT)


/* Cryptographic Engines and Security Accelerator Interrupt Cause Register */
#define MV_CESA_ISR_CAUSE_REG               (MV_CESA_REG_BASE + 0xE20)

/* Cryptographic Engines and Security Accelerator Interrupt Mask Register */
#define MV_CESA_ISR_MASK_REG                (MV_CESA_REG_BASE + 0xE24)

#define MV_CESA_CAUSE_AUTH_MASK             (1 << 0)
#define MV_CESA_CAUSE_DES_MASK              (1 << 1)
#define MV_CESA_CAUSE_AES_ENCR_MASK         (1 << 2)
#define MV_CESA_CAUSE_AES_DECR_MASK         (1 << 3)
#define MV_CESA_CAUSE_DES_ALL_MASK          (1 << 4)

#define MV_CESA_CAUSE_ACC_BIT               5
#define MV_CESA_CAUSE_ACC_MASK              (1 << MV_CESA_CAUSE_ACC_BIT)

#define MV_CESA_CAUSE_ACC_DMA_BIT           7
#define MV_CESA_CAUSE_ACC_DMA_MASK          (1 << MV_CESA_CAUSE_ACC_DMA_BIT)
#define MV_CESA_CAUSE_ACC_DMA_ALL_MASK      (3 << MV_CESA_CAUSE_ACC_DMA_BIT)

#define MV_CESA_CAUSE_DMA_COMPL_BIT         9
#define MV_CESA_CAUSE_DMA_COMPL_MASK        (1 << MV_CESA_CAUSE_DMA_COMPL_BIT)

#define MV_CESA_CAUSE_DMA_OWN_ERR_BIT       10
#define MV_CESA_CAUSE_DMA_OWN_ERR_MASK      (1 < MV_CESA_CAUSE_DMA_OWN_ERR_BIT)

#define MV_CESA_CAUSE_DMA_CHAIN_PKT_BIT     11
#define MV_CESA_CAUSE_DMA_CHAIN_PKT_MASK    (1 < MV_CESA_CAUSE_DMA_CHAIN_PKT_BIT)


#define MV_CESA_AUTH_DATA_IN_REG            (MV_CESA_REG_BASE + 0xd38)
#define MV_CESA_AUTH_BIT_COUNT_LOW_REG      (MV_CESA_REG_BASE + 0xd20)
#define MV_CESA_AUTH_BIT_COUNT_HIGH_REG     (MV_CESA_REG_BASE + 0xd24)

#define MV_CESA_AUTH_INIT_VAL_DIGEST_REG(i) (MV_CESA_REG_BASE + 0xd00 + (i<<2))

#define MV_CESA_AUTH_INIT_VAL_DIGEST_A_REG  (MV_CESA_REG_BASE + 0xd00)
#define MV_CESA_AUTH_INIT_VAL_DIGEST_B_REG  (MV_CESA_REG_BASE + 0xd04)
#define MV_CESA_AUTH_INIT_VAL_DIGEST_C_REG  (MV_CESA_REG_BASE + 0xd08)
#define MV_CESA_AUTH_INIT_VAL_DIGEST_D_REG  (MV_CESA_REG_BASE + 0xd0c)
#define MV_CESA_AUTH_INIT_VAL_DIGEST_E_REG  (MV_CESA_REG_BASE + 0xd10)
#define MV_CESA_AUTH_COMMAND_REG            (MV_CESA_REG_BASE + 0xd18)

#define MV_CESA_AUTH_ALGORITHM_BIT          0
#define MV_CESA_AUTH_ALGORITHM_MD5          (0<<AUTH_ALGORITHM_BIT)
#define MV_CESA_AUTH_ALGORITHM_SHA1         (1<<AUTH_ALGORITHM_BIT)

#define MV_CESA_AUTH_IV_MODE_BIT            1
#define MV_CESA_AUTH_IV_MODE_INIT           (0<<AUTH_IV_MODE_BIT)
#define MV_CESA_AUTH_IV_MODE_CONTINUE       (1<<AUTH_IV_MODE_BIT)

#define MV_CESA_AUTH_DATA_BYTE_SWAP_BIT     2
#define MV_CESA_AUTH_DATA_BYTE_SWAP_MASK    (1<<AUTH_DATA_BYTE_SWAP_BIT)


#define MV_CESA_AUTH_IV_BYTE_SWAP_BIT       4
#define MV_CESA_AUTH_IV_BYTE_SWAP_MASK      (1<<AUTH_IV_BYTE_SWAP_BIT)

#define MV_CESA_AUTH_TERMINATION_BIT        31
#define MV_CESA_AUTH_TERMINATION_MASK       (1<<AUTH_TERMINATION_BIT)


/*************** TDMA Control Register ************************************************/
#define MV_CESA_TDMA_CTRL_REG               (MV_CESA_TDMA_REG_BASE + 0x840)

#define MV_CESA_TDMA_BURST_32B              3   
#define MV_CESA_TDMA_BURST_128B             4   

#define MV_CESA_TDMA_DST_BURST_OFFSET       0
#define MV_CESA_TDMA_DST_BURST_ALL_MASK     (0x7<<MV_CESA_TDMA_DST_BURST_OFFSET)
#define MV_CESA_TDMA_DST_BURST_MASK(burst)  ((burst)<<MV_CESA_TDMA_DST_BURST_OFFSET)

#define MV_CESA_TDMA_OUTSTAND_READ_EN_BIT   4
#define MV_CESA_TDMA_OUTSTAND_READ_EN_MASK  (1<<MV_CESA_TDMA_OUTSTAND_READ_EN_BIT)

#define MV_CESA_TDMA_SRC_BURST_OFFSET       6
#define MV_CESA_TDMA_SRC_BURST_ALL_MASK     (0x7<<MV_CESA_TDMA_SRC_BURST_OFFSET)
#define MV_CESA_TDMA_SRC_BURST_MASK(burst)  ((burst)<<MV_CESA_TDMA_SRC_BURST_OFFSET)

#define MV_CESA_TDMA_CHAIN_MODE_BIT         9
#define MV_CESA_TDMA_NON_CHAIN_MODE_MASK    (1<<MV_CESA_TDMA_CHAIN_MODE_BIT)

#define MV_CESA_TDMA_BYTE_SWAP_BIT	    11
#define MV_CESA_TDMA_BYTE_SWAP_MASK	    (0 << MV_CESA_TDMA_BYTE_SWAP_BIT)
#define MV_CESA_TDMA_NO_BYTE_SWAP_MASK	    (1 << MV_CESA_TDMA_BYTE_SWAP_BIT)		    	

#define MV_CESA_TDMA_ENABLE_BIT		    12
#define MV_CESA_TDMA_ENABLE_MASK            (1<<MV_CESA_TDMA_ENABLE_BIT)
			    
#define MV_CESA_TDMA_FETCH_NEXT_DESC_BIT    13	
#define MV_CESA_TDMA_FETCH_NEXT_DESC_MASK   (1<<MV_CESA_TDMA_FETCH_NEXT_DESC_BIT)	

#define MV_CESA_TDMA_CHAN_ACTIVE_BIT	    14    
#define MV_CESA_TDMA_CHAN_ACTIVE_MASK       (1<<MV_CESA_TDMA_CHAN_ACTIVE_BIT)
/*------------------------------------------------------------------------------------*/

#define MV_CESA_TDMA_BYTE_COUNT_REG         (MV_CESA_TDMA_REG_BASE + 0x800)
#define MV_CESA_TDMA_SRC_ADDR_REG           (MV_CESA_TDMA_REG_BASE + 0x810)
#define MV_CESA_TDMA_DST_ADDR_REG           (MV_CESA_TDMA_REG_BASE + 0x820)
#define MV_CESA_TDMA_NEXT_DESC_PTR_REG      (MV_CESA_TDMA_REG_BASE + 0x830)
#define MV_CESA_TDMA_CURR_DESC_PTR_REG      (MV_CESA_TDMA_REG_BASE + 0x870)

#define MV_CESA_TDMA_ERROR_CAUSE_REG        (MV_CESA_TDMA_REG_BASE + 0x8C0)
#define MV_CESA_TDMA_ERROR_MASK_REG         (MV_CESA_TDMA_REG_BASE + 0x8C4)


#endif /* __mvCesaRegs_h__ */ 

