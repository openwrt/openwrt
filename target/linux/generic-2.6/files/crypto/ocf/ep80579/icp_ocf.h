/***************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or 
 *   redistributing this file, you may do so under either license.
 * 
 *   GPL LICENSE SUMMARY
 * 
 *   Copyright(c) 2007,2008 Intel Corporation. All rights reserved.
 * 
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of version 2 of the GNU General Public License as
 *   published by the Free Software Foundation.
 * 
 *   This program is distributed in the hope that it will be useful, but 
 *   WITHOUT ANY WARRANTY; without even the implied warranty of 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *   General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License 
 *   along with this program; if not, write to the Free Software 
 *   Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *   The full GNU General Public License is included in this distribution 
 *   in the file called LICENSE.GPL.
 * 
 *   Contact Information:
 *   Intel Corporation
 * 
 *   BSD LICENSE 
 * 
 *   Copyright(c) 2007,2008 Intel Corporation. All rights reserved.
 *   All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without 
 *   modification, are permitted provided that the following conditions 
 *   are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the 
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its 
 *       contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * 
 *  version: Security.L.1.0.130
 *
 ***************************************************************************/

/*
 * OCF drv driver header file for the Intel ICP processor.
 */

#ifndef ICP_OCF_H
#define ICP_OCF_H

#include <linux/crypto.h>
#include <linux/delay.h>
#include <linux/skbuff.h>

#include "cryptodev.h"
#include "uio.h"

#include "cpa.h"
#include "cpa_cy_im.h"
#include "cpa_cy_sym.h"
#include "cpa_cy_rand.h"
#include "cpa_cy_dh.h"
#include "cpa_cy_rsa.h"
#include "cpa_cy_ln.h"
#include "cpa_cy_common.h"
#include "cpa_cy_dsa.h"

#define NUM_BITS_IN_BYTE (8)
#define NUM_BITS_IN_BYTE_MINUS_ONE (NUM_BITS_IN_BYTE -1)
#define INVALID_DRIVER_ID (-1)
#define RETURN_RAND_NUM_GEN_FAILED (-1)

/*This is define means only one operation can be chained to another
(resulting in one chain of two operations)*/
#define MAX_NUM_OF_CHAINED_OPS (1)
/*This is the max block cipher initialisation vector*/
#define MAX_IV_LEN_IN_BYTES (20)
/*This is used to check whether the OCF to this driver session limit has
  been disabled*/
#define NO_OCF_TO_DRV_MAX_SESSIONS		(0)

/*OCF values mapped here*/
#define ICP_SHA1_DIGEST_SIZE_IN_BYTES 		(SHA1_HASH_LEN)
#define ICP_SHA256_DIGEST_SIZE_IN_BYTES 	(SHA2_256_HASH_LEN)
#define ICP_SHA384_DIGEST_SIZE_IN_BYTES 	(SHA2_384_HASH_LEN)
#define ICP_SHA512_DIGEST_SIZE_IN_BYTES 	(SHA2_512_HASH_LEN)
#define ICP_MD5_DIGEST_SIZE_IN_BYTES 		(MD5_HASH_LEN)
#define ARC4_COUNTER_LEN 			(ARC4_BLOCK_LEN)

#define OCF_REGISTRATION_STATUS_SUCCESS 	(0)
#define OCF_ZERO_FUNCTIONALITY_REGISTERED 	(0)
#define ICP_OCF_DRV_NO_CRYPTO_PROCESS_ERROR 	(0)
#define ICP_OCF_DRV_STATUS_SUCCESS 		(0)
#define ICP_OCF_DRV_STATUS_FAIL 		(1)

/*Turn on/off debug options*/
#define ICP_OCF_PRINT_DEBUG_MESSAGES		(0)
#define ICP_OCF_PRINT_KERN_ALERT		(1)
#define ICP_OCF_PRINT_KERN_ERRS			(1)

/*DSA Prime Q size in bytes (as defined in the standard) */
#define DSA_RS_SIGN_PRIMEQ_SIZE_IN_BYTES	(20)

/*MACRO DEFINITIONS*/

#define BITS_TO_BYTES(bytes, bits) 					\
	bytes = (bits + NUM_BITS_IN_BYTE_MINUS_ONE) / NUM_BITS_IN_BYTE

#define ICP_CACHE_CREATE(cache_ID, cache_name) 				\
	kmem_cache_create(cache_ID, sizeof(cache_name),0, 		\
		SLAB_HWCACHE_ALIGN, NULL, NULL);

#define ICP_CACHE_NULL_CHECK(slab_zone)					\
{									\
	if(NULL == slab_zone){ 						\
		icp_ocfDrvFreeCaches(); 				\
		EPRINTK("%s() line %d: Not enough memory!\n", 		\
			__FUNCTION__, __LINE__); 			\
		return ENOMEM; 						\
	}								\
}

#define ICP_CACHE_DESTROY(slab_zone) 	                                \
{                                                                       \
        if(NULL != slab_zone){						\
                kmem_cache_destroy(slab_zone);				\
                slab_zone = NULL;					\
        }								\
}

#define ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(alg)			\
{									\
	if(OCF_REGISTRATION_STATUS_SUCCESS ==				\
		crypto_register(icp_ocfDrvDriverId,			\
				    alg,				\
				    0,					\
				    0)) {				\
		ocfStatus++;						\
	}								\
}

#define ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(alg)			\
{									\
	if(OCF_REGISTRATION_STATUS_SUCCESS ==				\
		crypto_kregister(icp_ocfDrvDriverId,			\
				      alg,				\
				      0)){				\
		ocfStatus++;						\
	}								\
}

#if ICP_OCF_PRINT_DEBUG_MESSAGES == 1
#define DPRINTK(args...)      \
{			      \
                printk(args); \
}

#else				//ICP_OCF_PRINT_DEBUG_MESSAGES == 1

#define DPRINTK(args...)

#endif				//ICP_OCF_PRINT_DEBUG_MESSAGES == 1

#if ICP_OCF_PRINT_KERN_ALERT == 1
#define APRINTK(args...)      						\
{			      						\
       printk(KERN_ALERT args);						\
}

#else				//ICP_OCF_PRINT_KERN_ALERT == 1

#define APRINTK(args...)

#endif				//ICP_OCF_PRINT_KERN_ALERT == 1

#if ICP_OCF_PRINT_KERN_ERRS == 1
#define EPRINTK(args...)      \
{			      \
       printk(KERN_ERR args); \
}

#else				//ICP_OCF_PRINT_KERN_ERRS == 1

#define EPRINTK(args...)

#endif				//ICP_OCF_PRINT_KERN_ERRS == 1

#define IPRINTK(args...)      \
{			      \
      printk(KERN_INFO args); \
}

/*END OF MACRO DEFINITIONS*/

typedef enum {
	ICP_OCF_DRV_ALG_CIPHER = 0,
	ICP_OCF_DRV_ALG_HASH
} icp_ocf_drv_alg_type_t;

/* These are all defined in icp_common.c */
extern atomic_t lac_session_failed_dereg_count;
extern atomic_t icp_ocfDrvIsExiting;
extern atomic_t num_ocf_to_drv_registered_sessions;

/*These are use inputs used in icp_sym.c and icp_common.c
  They are instantiated in icp_common.c*/
extern int max_sessions;

extern int32_t icp_ocfDrvDriverId;
extern struct list_head icp_ocfDrvGlobalSymListHead;
extern struct list_head icp_ocfDrvGlobalSymListHead_FreeMemList;
extern struct workqueue_struct *icp_ocfDrvFreeLacSessionWorkQ;
extern spinlock_t icp_ocfDrvSymSessInfoListSpinlock;
extern rwlock_t icp_kmem_cache_destroy_alloc_lock;

/*Slab zones for symettric functionality, instantiated in icp_common.c*/
extern struct kmem_cache *drvSessionData_zone;
extern struct kmem_cache *drvOpData_zone;

/*Slabs zones for asymettric functionality, instantiated in icp_common.c*/
extern struct kmem_cache *drvDH_zone;
extern struct kmem_cache *drvLnModExp_zone;
extern struct kmem_cache *drvRSADecrypt_zone;
extern struct kmem_cache *drvRSAPrivateKey_zone;
extern struct kmem_cache *drvDSARSSign_zone;
extern struct kmem_cache *drvDSARSSignKValue_zone;
extern struct kmem_cache *drvDSAVerify_zone;

/*Slab zones for flatbuffers and bufferlist*/
extern struct kmem_cache *drvFlatBuffer_zone;

#define ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS     (16)

struct icp_drvBuffListInfo {
	Cpa16U numBuffers;
	Cpa32U metaSize;
	Cpa32U metaOffset;
	Cpa32U buffListSize;
};
extern struct icp_drvBuffListInfo defBuffListInfo;

/*
* This struct is used to keep a reference to the relevant node in the list
* of sessionData structs, to the buffer type required by OCF and to the OCF
* provided crp struct that needs to be returned. All this info is needed in
* the callback function.
*
* IV can sometimes be stored in non-contiguous memory (e.g. skbuff
* linked/frag list, therefore a contiguous memory space for the IV data must be
* created and passed to LAC
*
*/
struct icp_drvOpData {
	CpaCySymOpData lacOpData;
	uint32_t digestSizeInBytes;
	struct cryptop *crp;
	uint8_t bufferType;
	uint8_t ivData[MAX_IV_LEN_IN_BYTES];
	uint16_t numBufferListArray;
	CpaBufferList srcBuffer;
	CpaFlatBuffer bufferListArray[ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS];
	CpaBoolean verifyResult;
};
/*Values used to derisk chances of performs being called against
deregistered sessions (for which the slab page has been reclaimed)
This is not a fix - since page frames are reclaimed from a slab, one cannot
rely on that memory not being re-used by another app.*/
typedef enum {
	ICP_SESSION_INITIALISED = 0x5C5C5C,
	ICP_SESSION_RUNNING = 0x005C00,
	ICP_SESSION_DEREGISTERED = 0xC5C5C5
} usage_derisk;

/*
This is the OCF<->OCF_DRV session object:

1.The first member is a listNode. These session objects are added to a linked
  list in order to make it easier to remove them all at session exit time.
2.The second member is used to give the session object state and derisk the
  possibility of OCF batch calls executing against a deregistered session (as
  described above).
3.The third member is a LAC<->OCF_DRV session handle (initialised with the first
  perform request for that session).
4.The fourth is the LAC session context. All the parameters for this structure
  are only known when the first perform request for this session occurs. That is
  why the OCF Tolapai Driver only registers a new LAC session at perform time
*/
struct icp_drvSessionData {
	struct list_head listNode;
	usage_derisk inUse;
	CpaCySymSessionCtx sessHandle;
	CpaCySymSessionSetupData lacSessCtx;
};

/* This struct is required for deferred session
 deregistration as a work queue function can
 only have one argument*/
struct icp_ocfDrvFreeLacSession {
	CpaCySymSessionCtx sessionToDeregister;
	struct work_struct work;
};

int icp_ocfDrvNewSession(device_t dev, uint32_t * sild, struct cryptoini *cri);

int icp_ocfDrvFreeLACSession(device_t dev, uint64_t sid);

int icp_ocfDrvSymProcess(device_t dev, struct cryptop *crp, int hint);

int icp_ocfDrvPkeProcess(device_t dev, struct cryptkop *krp, int hint);

int icp_ocfDrvReadRandom(void *arg, uint32_t * buf, int maxwords);

int icp_ocfDrvDeregRetry(CpaCySymSessionCtx sessionToDeregister);

int icp_ocfDrvSkBuffToBufferList(struct sk_buff *skb,
				 CpaBufferList * bufferList);

int icp_ocfDrvBufferListToSkBuff(CpaBufferList * bufferList,
				 struct sk_buff **skb);

void icp_ocfDrvPtrAndLenToFlatBuffer(void *pData, uint32_t len,
				     CpaFlatBuffer * pFlatBuffer);

void icp_ocfDrvPtrAndLenToBufferList(void *pDataIn, uint32_t length,
				     CpaBufferList * pBufferList);

void icp_ocfDrvBufferListToPtrAndLen(CpaBufferList * pBufferList,
				     void **ppDataOut, uint32_t * pLength);

int icp_ocfDrvBufferListMemInfo(uint16_t numBuffers,
				struct icp_drvBuffListInfo *buffListInfo);

uint16_t icp_ocfDrvGetSkBuffFrags(struct sk_buff *pSkb);

void icp_ocfDrvFreeFlatBuffer(CpaFlatBuffer * pFlatBuffer);

int icp_ocfDrvAllocMetaData(CpaBufferList * pBufferList, 
                const struct icp_drvOpData *pOpData);

void icp_ocfDrvFreeMetaData(CpaBufferList * pBufferList);

#endif
/* ICP_OCF_H */
