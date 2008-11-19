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
 * An OCF module that uses the API for Intel® QuickAssist Technology to do the
 * cryptography.
 *
 * This driver requires the ICP Access Library that is available from Intel in
 * order to operate.
 */

#include "icp_ocf.h"

/*This is the call back function for all symmetric cryptographic processes.
  Its main functionality is to free driver crypto operation structure and to 
  call back to OCF*/
static void
icp_ocfDrvSymCallBack(void *callbackTag,
		      CpaStatus status,
		      const CpaCySymOp operationType,
		      void *pOpData,
		      CpaBufferList * pDstBuffer, CpaBoolean verifyResult);

/*This function is used to extract crypto processing information from the OCF
  inputs, so as that it may be passed onto LAC*/
static int
icp_ocfDrvProcessDataSetup(struct icp_drvOpData *drvOpData,
			   struct cryptodesc *crp_desc);

/*This function checks whether the crp_desc argument pertains to a digest or a
  cipher operation*/
static int icp_ocfDrvAlgCheck(struct cryptodesc *crp_desc);

/*This function copies all the passed in session context information and stores
  it in a LAC context structure*/
static int
icp_ocfDrvAlgorithmSetup(struct cryptoini *cri,
			 CpaCySymSessionSetupData * lacSessCtx);

/*This top level function is used to find a pointer to where a digest is 
  stored/needs to be inserted. */
static uint8_t *icp_ocfDrvDigestPointerFind(struct icp_drvOpData *drvOpData,
					    struct cryptodesc *crp_desc);

/*This function is called when a digest pointer has to be found within a
  SKBUFF.*/
static inline uint8_t *icp_ocfDrvSkbuffDigestPointerFind(struct icp_drvOpData
							 *drvOpData,
							 int offsetInBytes,
							 uint32_t
							 digestSizeInBytes);

/*The following two functions are called if the SKBUFF digest pointer is not 
  positioned in the linear portion of the buffer (i.e. it is in a linked SKBUFF
   or page fragment).*/
/*This function takes care of the page fragment case.*/
static inline uint8_t *icp_ocfDrvDigestSkbNRFragsCheck(struct sk_buff *skb,
						       struct skb_shared_info
						       *skb_shared,
						       int offsetInBytes,
						       uint32_t
						       digestSizeInBytes);

/*This function takes care of the linked list case.*/
static inline uint8_t *icp_ocfDrvDigestSkbFragListCheck(struct sk_buff *skb,
							struct skb_shared_info
							*skb_shared,
							int offsetInBytes,
							uint32_t
							digestSizeInBytes);

/*This function is used to free an OCF->OCF_DRV session object*/
static void icp_ocfDrvFreeOCFSession(struct icp_drvSessionData *sessionData);

/*max IOV buffs supported in a UIO structure*/
#define NUM_IOV_SUPPORTED		(1)

/* Name        : icp_ocfDrvSymCallBack
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the relevant symmetric operation. 
 *
 * Notes : The callbackTag is a pointer to an icp_drvOpData. This memory
 * object was passed to LAC for the cryptographic processing and contains all
 * the relevant information for cleaning up buffer handles etc. so that the
 * OCF Tolapai Driver portion of this crypto operation can be fully completed.
 */
static void
icp_ocfDrvSymCallBack(void *callbackTag,
		      CpaStatus status,
		      const CpaCySymOp operationType,
		      void *pOpData,
		      CpaBufferList * pDstBuffer, CpaBoolean verifyResult)
{
	struct cryptop *crp = NULL;
	struct icp_drvOpData *temp_drvOpData =
	    (struct icp_drvOpData *)callbackTag;
	uint64_t *tempBasePtr = NULL;
	uint32_t tempLen = 0;

	if (NULL == temp_drvOpData) {
		DPRINTK("%s(): The callback from the LAC component"
			" has failed due to Null userOpaque data"
			"(status == %d).\n", __FUNCTION__, status);
		DPRINTK("%s(): Unable to call OCF back! \n", __FUNCTION__);
		return;
	}

	crp = temp_drvOpData->crp;
	crp->crp_etype = ICP_OCF_DRV_NO_CRYPTO_PROCESS_ERROR;

	if (NULL == pOpData) {
		DPRINTK("%s(): The callback from the LAC component"
			" has failed due to Null Symmetric Op data"
			"(status == %d).\n", __FUNCTION__, status);
		crp->crp_etype = ECANCELED;
		crypto_done(crp);
		return;
	}

	if (NULL == pDstBuffer) {
		DPRINTK("%s(): The callback from the LAC component"
			" has failed due to Null Dst Bufferlist data"
			"(status == %d).\n", __FUNCTION__, status);
		crp->crp_etype = ECANCELED;
		crypto_done(crp);
		return;
	}

	if (CPA_STATUS_SUCCESS == status) {

		if (temp_drvOpData->bufferType == CRYPTO_F_SKBUF) {
			if (ICP_OCF_DRV_STATUS_SUCCESS !=
			    icp_ocfDrvBufferListToSkBuff(pDstBuffer,
							 (struct sk_buff **)
							 &(crp->crp_buf))) {
				EPRINTK("%s(): BufferList to SkBuff "
					"conversion error.\n", __FUNCTION__);
				crp->crp_etype = EPERM;
			}
		} else {
			icp_ocfDrvBufferListToPtrAndLen(pDstBuffer,
							(void **)&tempBasePtr,
							&tempLen);
			crp->crp_olen = (int)tempLen;
		}

	} else {
		DPRINTK("%s(): The callback from the LAC component has failed"
			"(status == %d).\n", __FUNCTION__, status);

		crp->crp_etype = ECANCELED;
	}

	if (temp_drvOpData->numBufferListArray >
	    ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS) {
		kfree(pDstBuffer->pBuffers);
	}
	icp_ocfDrvFreeMetaData(pDstBuffer);
	kmem_cache_free(drvOpData_zone, temp_drvOpData);

	/* Invoke the OCF callback function */
	crypto_done(crp);

	return;
}

/* Name        : icp_ocfDrvNewSession 
 *
 * Description : This function will create a new Driver<->OCF session
 *
 * Notes : LAC session registration happens during the first perform call.
 * That is the first time we know all information about a given session.
 */
int icp_ocfDrvNewSession(device_t dev, uint32_t * sid, struct cryptoini *cri)
{
	struct icp_drvSessionData *sessionData = NULL;
	uint32_t delete_session = 0;

	/* The SID passed in should be our driver ID. We can return the     */
	/* local ID (LID) which is a unique identifier which we can use     */
	/* to differentiate between the encrypt/decrypt LAC session handles */
	if (NULL == sid) {
		EPRINTK("%s(): Invalid input parameters - NULL sid.\n",
			__FUNCTION__);
		return EINVAL;
	}

	if (NULL == cri) {
		EPRINTK("%s(): Invalid input parameters - NULL cryptoini.\n",
			__FUNCTION__);
		return EINVAL;
	}

	if (icp_ocfDrvDriverId != *sid) {
		EPRINTK("%s(): Invalid input parameters - bad driver ID\n",
			__FUNCTION__);
		EPRINTK("\t sid = 0x08%p \n \t cri = 0x08%p \n", sid, cri);
		return EINVAL;
	}

	sessionData = kmem_cache_zalloc(drvSessionData_zone, GFP_ATOMIC);
	if (NULL == sessionData) {
		DPRINTK("%s():No memory for Session Data\n", __FUNCTION__);
		return ENOMEM;
	}

	/*ENTER CRITICAL SECTION */
	spin_lock_bh(&icp_ocfDrvSymSessInfoListSpinlock);
	/*put this check in the spinlock so no new sessions can be added to the
	   linked list when we are exiting */
	if (CPA_TRUE == atomic_read(&icp_ocfDrvIsExiting)) {
		delete_session++;

	} else if (NO_OCF_TO_DRV_MAX_SESSIONS != max_sessions) {
		if (atomic_read(&num_ocf_to_drv_registered_sessions) >=
		    (max_sessions -
		     atomic_read(&lac_session_failed_dereg_count))) {
			delete_session++;
		} else {
			atomic_inc(&num_ocf_to_drv_registered_sessions);
			/* Add to session data linked list */
			list_add(&(sessionData->listNode),
				 &icp_ocfDrvGlobalSymListHead);
		}

	} else if (NO_OCF_TO_DRV_MAX_SESSIONS == max_sessions) {
		list_add(&(sessionData->listNode),
			 &icp_ocfDrvGlobalSymListHead);
	}

	sessionData->inUse = ICP_SESSION_INITIALISED;

	/*EXIT CRITICAL SECTION */
	spin_unlock_bh(&icp_ocfDrvSymSessInfoListSpinlock);

	if (delete_session) {
		DPRINTK("%s():No Session handles available\n", __FUNCTION__);
		kmem_cache_free(drvSessionData_zone, sessionData);
		return EPERM;
	}

	if (ICP_OCF_DRV_STATUS_SUCCESS !=
	    icp_ocfDrvAlgorithmSetup(cri, &(sessionData->lacSessCtx))) {
		DPRINTK("%s():algorithm not supported\n", __FUNCTION__);
		icp_ocfDrvFreeOCFSession(sessionData);
		return EINVAL;
	}

	if (cri->cri_next) {
		if (cri->cri_next->cri_next != NULL) {
			DPRINTK("%s():only two chained algorithms supported\n",
				__FUNCTION__);
			icp_ocfDrvFreeOCFSession(sessionData);
			return EPERM;
		}

		if (ICP_OCF_DRV_STATUS_SUCCESS !=
		    icp_ocfDrvAlgorithmSetup(cri->cri_next,
					     &(sessionData->lacSessCtx))) {
			DPRINTK("%s():second algorithm not supported\n",
				__FUNCTION__);
			icp_ocfDrvFreeOCFSession(sessionData);
			return EINVAL;
		}

		sessionData->lacSessCtx.symOperation =
		    CPA_CY_SYM_OP_ALGORITHM_CHAINING;
	}

	*sid = (uint32_t) sessionData;

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvAlgorithmSetup
 *
 * Description : This function builds the session context data from the
 * information supplied through OCF. Algorithm chain order and whether the
 * session is Encrypt/Decrypt can only be found out at perform time however, so
 * the session is registered with LAC at that time.
 */
static int
icp_ocfDrvAlgorithmSetup(struct cryptoini *cri,
			 CpaCySymSessionSetupData * lacSessCtx)
{

	lacSessCtx->sessionPriority = CPA_CY_PRIORITY_NORMAL;

	switch (cri->cri_alg) {

	case CRYPTO_NULL_CBC:
		DPRINTK("%s(): NULL CBC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_CIPHER;
		lacSessCtx->cipherSetupData.cipherAlgorithm =
		    CPA_CY_SYM_CIPHER_NULL;
		lacSessCtx->cipherSetupData.cipherKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->cipherSetupData.pCipherKey = cri->cri_key;
		break;

	case CRYPTO_DES_CBC:
		DPRINTK("%s(): DES CBC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_CIPHER;
		lacSessCtx->cipherSetupData.cipherAlgorithm =
		    CPA_CY_SYM_CIPHER_DES_CBC;
		lacSessCtx->cipherSetupData.cipherKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->cipherSetupData.pCipherKey = cri->cri_key;
		break;

	case CRYPTO_3DES_CBC:
		DPRINTK("%s(): 3DES CBC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_CIPHER;
		lacSessCtx->cipherSetupData.cipherAlgorithm =
		    CPA_CY_SYM_CIPHER_3DES_CBC;
		lacSessCtx->cipherSetupData.cipherKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->cipherSetupData.pCipherKey = cri->cri_key;
		break;

	case CRYPTO_AES_CBC:
		DPRINTK("%s(): AES CBC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_CIPHER;
		lacSessCtx->cipherSetupData.cipherAlgorithm =
		    CPA_CY_SYM_CIPHER_AES_CBC;
		lacSessCtx->cipherSetupData.cipherKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->cipherSetupData.pCipherKey = cri->cri_key;
		break;

	case CRYPTO_ARC4:
		DPRINTK("%s(): ARC4\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_CIPHER;
		lacSessCtx->cipherSetupData.cipherAlgorithm =
		    CPA_CY_SYM_CIPHER_ARC4;
		lacSessCtx->cipherSetupData.cipherKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->cipherSetupData.pCipherKey = cri->cri_key;
		break;

	case CRYPTO_SHA1:
		DPRINTK("%s(): SHA1\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm = CPA_CY_SYM_HASH_SHA1;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_PLAIN;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA1_DIGEST_SIZE_IN_BYTES);

		break;

	case CRYPTO_SHA1_HMAC:
		DPRINTK("%s(): SHA1_HMAC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm = CPA_CY_SYM_HASH_SHA1;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA1_DIGEST_SIZE_IN_BYTES);
		lacSessCtx->hashSetupData.authModeSetupData.authKey =
		    cri->cri_key;
		lacSessCtx->hashSetupData.authModeSetupData.authKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->hashSetupData.authModeSetupData.aadLenInBytes = 0;

		break;

	case CRYPTO_SHA2_256:
		DPRINTK("%s(): SHA256\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA256;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_PLAIN;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA256_DIGEST_SIZE_IN_BYTES);

		break;

	case CRYPTO_SHA2_256_HMAC:
		DPRINTK("%s(): SHA256_HMAC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA256;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA256_DIGEST_SIZE_IN_BYTES);
		lacSessCtx->hashSetupData.authModeSetupData.authKey =
		    cri->cri_key;
		lacSessCtx->hashSetupData.authModeSetupData.authKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->hashSetupData.authModeSetupData.aadLenInBytes = 0;

		break;

	case CRYPTO_SHA2_384:
		DPRINTK("%s(): SHA384\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA384;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_PLAIN;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA384_DIGEST_SIZE_IN_BYTES);

		break;

	case CRYPTO_SHA2_384_HMAC:
		DPRINTK("%s(): SHA384_HMAC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA384;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA384_DIGEST_SIZE_IN_BYTES);
		lacSessCtx->hashSetupData.authModeSetupData.authKey =
		    cri->cri_key;
		lacSessCtx->hashSetupData.authModeSetupData.authKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->hashSetupData.authModeSetupData.aadLenInBytes = 0;

		break;

	case CRYPTO_SHA2_512:
		DPRINTK("%s(): SHA512\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA512;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_PLAIN;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA512_DIGEST_SIZE_IN_BYTES);

		break;

	case CRYPTO_SHA2_512_HMAC:
		DPRINTK("%s(): SHA512_HMAC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm =
		    CPA_CY_SYM_HASH_SHA512;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_SHA512_DIGEST_SIZE_IN_BYTES);
		lacSessCtx->hashSetupData.authModeSetupData.authKey =
		    cri->cri_key;
		lacSessCtx->hashSetupData.authModeSetupData.authKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->hashSetupData.authModeSetupData.aadLenInBytes = 0;

		break;

	case CRYPTO_MD5:
		DPRINTK("%s(): MD5\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm = CPA_CY_SYM_HASH_MD5;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_PLAIN;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_MD5_DIGEST_SIZE_IN_BYTES);

		break;

	case CRYPTO_MD5_HMAC:
		DPRINTK("%s(): MD5_HMAC\n", __FUNCTION__);
		lacSessCtx->symOperation = CPA_CY_SYM_OP_HASH;
		lacSessCtx->hashSetupData.hashAlgorithm = CPA_CY_SYM_HASH_MD5;
		lacSessCtx->hashSetupData.hashMode = CPA_CY_SYM_HASH_MODE_AUTH;
		lacSessCtx->hashSetupData.digestResultLenInBytes =
		    (cri->cri_mlen ?
		     cri->cri_mlen : ICP_MD5_DIGEST_SIZE_IN_BYTES);
		lacSessCtx->hashSetupData.authModeSetupData.authKey =
		    cri->cri_key;
		lacSessCtx->hashSetupData.authModeSetupData.authKeyLenInBytes =
		    cri->cri_klen / NUM_BITS_IN_BYTE;
		lacSessCtx->hashSetupData.authModeSetupData.aadLenInBytes = 0;

		break;

	default:
		DPRINTK("%s(): ALG Setup FAIL\n", __FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvFreeOCFSession
 *
 * Description : This function deletes all existing Session data representing
 * the Cryptographic session established between OCF and this driver. This
 * also includes freeing the memory allocated for the session context. The
 * session object is also removed from the session linked list.
 */
static void icp_ocfDrvFreeOCFSession(struct icp_drvSessionData *sessionData)
{

	sessionData->inUse = ICP_SESSION_DEREGISTERED;

	/*ENTER CRITICAL SECTION */
	spin_lock_bh(&icp_ocfDrvSymSessInfoListSpinlock);

	if (CPA_TRUE == atomic_read(&icp_ocfDrvIsExiting)) {
		/*If the Driver is exiting, allow that process to
		   handle any deletions */
		/*EXIT CRITICAL SECTION */
		spin_unlock_bh(&icp_ocfDrvSymSessInfoListSpinlock);
		return;
	}

	atomic_dec(&num_ocf_to_drv_registered_sessions);

	list_del(&(sessionData->listNode));

	/*EXIT CRITICAL SECTION */
	spin_unlock_bh(&icp_ocfDrvSymSessInfoListSpinlock);

	if (NULL != sessionData->sessHandle) {
		kfree(sessionData->sessHandle);
	}
	kmem_cache_free(drvSessionData_zone, sessionData);
}

/* Name        : icp_ocfDrvFreeLACSession
 *
 * Description : This attempts to deregister a LAC session. If it fails, the
 * deregistation retry function is called.
 */
int icp_ocfDrvFreeLACSession(device_t dev, uint64_t sid)
{
	CpaCySymSessionCtx sessionToDeregister = NULL;
	struct icp_drvSessionData *sessionData = NULL;
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	int retval = 0;

	sessionData = (struct icp_drvSessionData *)CRYPTO_SESID2LID(sid);
	if (NULL == sessionData) {
		EPRINTK("%s(): OCF Free session called with Null Session ID.\n",
			__FUNCTION__);
		return EINVAL;
	}

	sessionToDeregister = sessionData->sessHandle;

	if (ICP_SESSION_INITIALISED == sessionData->inUse) {
		DPRINTK("%s() Session not registered with LAC\n", __FUNCTION__);
	} else if (NULL == sessionData->sessHandle) {
		EPRINTK
		    ("%s(): OCF Free session called with Null Session Handle.\n",
		     __FUNCTION__);
		return EINVAL;
	} else {
		lacStatus = cpaCySymRemoveSession(CPA_INSTANCE_HANDLE_SINGLE,
						  sessionToDeregister);
		if (CPA_STATUS_RETRY == lacStatus) {
			if (ICP_OCF_DRV_STATUS_SUCCESS !=
			    icp_ocfDrvDeregRetry(&sessionToDeregister)) {
				/* the retry function increments the 
				   dereg failed count */
				DPRINTK("%s(): LAC failed to deregister the "
					"session. (localSessionId= %p)\n",
					__FUNCTION__, sessionToDeregister);
				retval = EPERM;
			}

		} else if (CPA_STATUS_SUCCESS != lacStatus) {
			DPRINTK("%s(): LAC failed to deregister the session. "
				"localSessionId= %p, lacStatus = %d\n",
				__FUNCTION__, sessionToDeregister, lacStatus);
			atomic_inc(&lac_session_failed_dereg_count);
			retval = EPERM;
		}
	}

	icp_ocfDrvFreeOCFSession(sessionData);
	return retval;

}

/* Name        : icp_ocfDrvAlgCheck 
 *
 * Description : This function checks whether the cryptodesc argument pertains
 * to a sym or hash function
 */
static int icp_ocfDrvAlgCheck(struct cryptodesc *crp_desc)
{

	if (crp_desc->crd_alg == CRYPTO_3DES_CBC ||
	    crp_desc->crd_alg == CRYPTO_AES_CBC ||
	    crp_desc->crd_alg == CRYPTO_DES_CBC ||
	    crp_desc->crd_alg == CRYPTO_NULL_CBC ||
	    crp_desc->crd_alg == CRYPTO_ARC4) {
		return ICP_OCF_DRV_ALG_CIPHER;
	}

	return ICP_OCF_DRV_ALG_HASH;
}

/* Name        : icp_ocfDrvSymProcess 
 *
 * Description : This function will map symmetric functionality calls from OCF
 * to the LAC API. It will also allocate memory to store the session context.
 * 
 * Notes: If it is the first perform call for a given session, then a LAC
 * session is registered. After the session is registered, no checks as
 * to whether session paramaters have changed (e.g. alg chain order) are
 * done.
 */
int icp_ocfDrvSymProcess(device_t dev, struct cryptop *crp, int hint)
{
	struct icp_drvSessionData *sessionData = NULL;
	struct icp_drvOpData *drvOpData = NULL;
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	Cpa32U sessionCtxSizeInBytes = 0;
	uint16_t numBufferListArray = 0;

	if (NULL == crp) {
		DPRINTK("%s(): Invalid input parameters, cryptop is NULL\n",
			__FUNCTION__);
		return EINVAL;
	}

	if (NULL == crp->crp_desc) {
		DPRINTK("%s(): Invalid input parameters, no crp_desc attached "
			"to crp\n", __FUNCTION__);
		crp->crp_etype = EINVAL;
		return EINVAL;
	}

	if (NULL == crp->crp_buf) {
		DPRINTK("%s(): Invalid input parameters, no buffer attached "
			"to crp\n", __FUNCTION__);
		crp->crp_etype = EINVAL;
		return EINVAL;
	}

	if (CPA_TRUE == atomic_read(&icp_ocfDrvIsExiting)) {
		crp->crp_etype = EFAULT;
		return EFAULT;
	}

	sessionData = (struct icp_drvSessionData *)
	    (CRYPTO_SESID2LID(crp->crp_sid));
	if (NULL == sessionData) {
		DPRINTK("%s(): Invalid input parameters, Null Session ID \n",
			__FUNCTION__);
		crp->crp_etype = EINVAL;
		return EINVAL;
	}

/*If we get a request against a deregisted session, cancel operation*/
	if (ICP_SESSION_DEREGISTERED == sessionData->inUse) {
		DPRINTK("%s(): Session ID %d was deregistered \n",
			__FUNCTION__, (int)(CRYPTO_SESID2LID(crp->crp_sid)));
		crp->crp_etype = EFAULT;
		return EFAULT;
	}

/*If none of the session states are set, then the session structure was either
  not initialised properly or we are reading from a freed memory area (possible
  due to OCF batch mode not removing queued requests against deregistered 
  sessions*/
	if (ICP_SESSION_INITIALISED != sessionData->inUse &&
	    ICP_SESSION_RUNNING != sessionData->inUse) {
		DPRINTK("%s(): Session - ID %d - not properly initialised or "
			"memory freed back to the kernel \n",
			__FUNCTION__, (int)(CRYPTO_SESID2LID(crp->crp_sid)));
		crp->crp_etype = EINVAL;
		return EINVAL;
	}

	/*For the below checks, remember error checking is already done in LAC.
	   We're not validating inputs subsequent to registration */
	if (sessionData->inUse == ICP_SESSION_INITIALISED) {
		DPRINTK("%s(): Initialising session\n", __FUNCTION__);

		if (NULL != crp->crp_desc->crd_next) {
			if (ICP_OCF_DRV_ALG_CIPHER ==
			    icp_ocfDrvAlgCheck(crp->crp_desc)) {

				sessionData->lacSessCtx.algChainOrder =
				    CPA_CY_SYM_ALG_CHAIN_ORDER_CIPHER_THEN_HASH;

				if (crp->crp_desc->crd_flags & CRD_F_ENCRYPT) {
					sessionData->lacSessCtx.cipherSetupData.
					    cipherDirection =
					    CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT;
				} else {
					sessionData->lacSessCtx.cipherSetupData.
					    cipherDirection =
					    CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT;
				}
			} else {
				sessionData->lacSessCtx.algChainOrder =
				    CPA_CY_SYM_ALG_CHAIN_ORDER_HASH_THEN_CIPHER;

				if (crp->crp_desc->crd_next->crd_flags &
				    CRD_F_ENCRYPT) {
					sessionData->lacSessCtx.cipherSetupData.
					    cipherDirection =
					    CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT;
				} else {
					sessionData->lacSessCtx.cipherSetupData.
					    cipherDirection =
					    CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT;
				}

			}

		} else if (ICP_OCF_DRV_ALG_CIPHER ==
			   icp_ocfDrvAlgCheck(crp->crp_desc)) {
			if (crp->crp_desc->crd_flags & CRD_F_ENCRYPT) {
				sessionData->lacSessCtx.cipherSetupData.
				    cipherDirection =
				    CPA_CY_SYM_CIPHER_DIRECTION_ENCRYPT;
			} else {
				sessionData->lacSessCtx.cipherSetupData.
				    cipherDirection =
				    CPA_CY_SYM_CIPHER_DIRECTION_DECRYPT;
			}

		}

		/*No action required for standalone Auth here */

		/* Allocate memory for SymSessionCtx before the Session Registration */
		lacStatus =
		    cpaCySymSessionCtxGetSize(CPA_INSTANCE_HANDLE_SINGLE,
					      &(sessionData->lacSessCtx),
					      &sessionCtxSizeInBytes);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): cpaCySymSessionCtxGetSize failed - %d\n",
				__FUNCTION__, lacStatus);
			return EINVAL;
		}
		sessionData->sessHandle =
		    kmalloc(sessionCtxSizeInBytes, GFP_ATOMIC);
		if (NULL == sessionData->sessHandle) {
			EPRINTK
			    ("%s(): Failed to get memory for SymSessionCtx\n",
			     __FUNCTION__);
			return ENOMEM;
		}

		lacStatus = cpaCySymInitSession(CPA_INSTANCE_HANDLE_SINGLE,
						icp_ocfDrvSymCallBack,
						&(sessionData->lacSessCtx),
						sessionData->sessHandle);

		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): cpaCySymInitSession failed -%d \n",
				__FUNCTION__, lacStatus);
			return EFAULT;
		}

		sessionData->inUse = ICP_SESSION_RUNNING;
	}

	drvOpData = kmem_cache_zalloc(drvOpData_zone, GFP_ATOMIC);
	if (NULL == drvOpData) {
		EPRINTK("%s():Failed to get memory for drvOpData\n",
			__FUNCTION__);
		crp->crp_etype = ENOMEM;
		return ENOMEM;
	}

	drvOpData->lacOpData.pSessionCtx = sessionData->sessHandle;
	drvOpData->digestSizeInBytes = sessionData->lacSessCtx.hashSetupData.
	    digestResultLenInBytes;
	drvOpData->crp = crp;

	/* Set the default buffer list array memory allocation */
	drvOpData->srcBuffer.pBuffers = drvOpData->bufferListArray;
	drvOpData->numBufferListArray = ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS;

	/* 
	 * Allocate buffer list array memory allocation if the
	 * data fragment is more than the default allocation
	 */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		numBufferListArray = icp_ocfDrvGetSkBuffFrags((struct sk_buff *)
							      crp->crp_buf);
		if (ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS < numBufferListArray) {
			DPRINTK("%s() numBufferListArray more than default\n",
				__FUNCTION__);
			drvOpData->srcBuffer.pBuffers = NULL;
			drvOpData->srcBuffer.pBuffers =
			    kmalloc(numBufferListArray *
				    sizeof(CpaFlatBuffer), GFP_ATOMIC);
			if (NULL == drvOpData->srcBuffer.pBuffers) {
				EPRINTK("%s() Failed to get memory for "
					"pBuffers\n", __FUNCTION__);
				kmem_cache_free(drvOpData_zone, drvOpData);
				crp->crp_etype = ENOMEM;
				return ENOMEM;
			}
			drvOpData->numBufferListArray = numBufferListArray;
		}
	}

	/*
	 * Check the type of buffer structure we got and convert it into
	 * CpaBufferList format.
	 */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		if (ICP_OCF_DRV_STATUS_SUCCESS !=
		    icp_ocfDrvSkBuffToBufferList((struct sk_buff *)crp->crp_buf,
						 &(drvOpData->srcBuffer))) {
			EPRINTK("%s():Failed to translate from SK_BUF "
				"to bufferlist\n", __FUNCTION__);
			crp->crp_etype = EINVAL;
			goto err;
		}

		drvOpData->bufferType = CRYPTO_F_SKBUF;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		/* OCF only supports IOV of one entry. */
		if (NUM_IOV_SUPPORTED ==
		    ((struct uio *)(crp->crp_buf))->uio_iovcnt) {

			icp_ocfDrvPtrAndLenToBufferList(((struct uio *)(crp->
									crp_buf))->
							uio_iov[0].iov_base,
							((struct uio *)(crp->
									crp_buf))->
							uio_iov[0].iov_len,
							&(drvOpData->
							  srcBuffer));

			drvOpData->bufferType = CRYPTO_F_IOV;

		} else {
			DPRINTK("%s():Unable to handle IOVs with lengths of "
				"greater than one!\n", __FUNCTION__);
			crp->crp_etype = EINVAL;
			goto err;
		}

	} else {
		icp_ocfDrvPtrAndLenToBufferList(crp->crp_buf,
						crp->crp_ilen,
						&(drvOpData->srcBuffer));

		drvOpData->bufferType = CRYPTO_BUF_CONTIG;
	}

	if (ICP_OCF_DRV_STATUS_SUCCESS !=
	    icp_ocfDrvProcessDataSetup(drvOpData, drvOpData->crp->crp_desc)) {
		crp->crp_etype = EINVAL;
		goto err;
	}

	if (drvOpData->crp->crp_desc->crd_next != NULL) {
		if (icp_ocfDrvProcessDataSetup(drvOpData, drvOpData->crp->
					       crp_desc->crd_next)) {
			crp->crp_etype = EINVAL;
			goto err;
		}

	}

	/* Allocate srcBuffer's private meta data */
	if (ICP_OCF_DRV_STATUS_SUCCESS !=
	    icp_ocfDrvAllocMetaData(&(drvOpData->srcBuffer), drvOpData)) {
		EPRINTK("%s() icp_ocfDrvAllocMetaData failed\n", __FUNCTION__);
		memset(&(drvOpData->lacOpData), 0, sizeof(CpaCySymOpData));
		crp->crp_etype = EINVAL;
		goto err;
	}

	/* Perform "in-place" crypto operation */
	lacStatus = cpaCySymPerformOp(CPA_INSTANCE_HANDLE_SINGLE,
				      (void *)drvOpData,
				      &(drvOpData->lacOpData),
				      &(drvOpData->srcBuffer),
				      &(drvOpData->srcBuffer),
				      &(drvOpData->verifyResult));
	if (CPA_STATUS_RETRY == lacStatus) {
		DPRINTK("%s(): cpaCySymPerformOp retry, lacStatus = %d\n",
			__FUNCTION__, lacStatus);
		memset(&(drvOpData->lacOpData), 0, sizeof(CpaCySymOpData));
		crp->crp_etype = EINVAL;
		goto err;
	}
	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): cpaCySymPerformOp failed, lacStatus = %d\n",
			__FUNCTION__, lacStatus);
		memset(&(drvOpData->lacOpData), 0, sizeof(CpaCySymOpData));
		crp->crp_etype = EINVAL;
		goto err;
	}

	return 0;		//OCF success status value

      err:
	if (drvOpData->numBufferListArray > ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS) {
		kfree(drvOpData->srcBuffer.pBuffers);
	}
	icp_ocfDrvFreeMetaData(&(drvOpData->srcBuffer));
	kmem_cache_free(drvOpData_zone, drvOpData);

	return crp->crp_etype;
}

/* Name        : icp_ocfDrvProcessDataSetup
 *
 * Description : This function will setup all the cryptographic operation data
 *               that is required by LAC to execute the operation.
 */
static int icp_ocfDrvProcessDataSetup(struct icp_drvOpData *drvOpData,
				      struct cryptodesc *crp_desc)
{
	CpaCyRandGenOpData randGenOpData;
	CpaFlatBuffer randData;

	drvOpData->lacOpData.packetType = CPA_CY_SYM_PACKET_TYPE_FULL;

	/* Convert from the cryptop to the ICP LAC crypto parameters */
	switch (crp_desc->crd_alg) {
	case CRYPTO_NULL_CBC:
		drvOpData->lacOpData.
		    cryptoStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToCipherInBytes = crp_desc->crd_len;
		drvOpData->verifyResult = CPA_FALSE;
		drvOpData->lacOpData.ivLenInBytes = NULL_BLOCK_LEN;
		break;
	case CRYPTO_DES_CBC:
		drvOpData->lacOpData.
		    cryptoStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToCipherInBytes = crp_desc->crd_len;
		drvOpData->verifyResult = CPA_FALSE;
		drvOpData->lacOpData.ivLenInBytes = DES_BLOCK_LEN;
		break;
	case CRYPTO_3DES_CBC:
		drvOpData->lacOpData.
		    cryptoStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToCipherInBytes = crp_desc->crd_len;
		drvOpData->verifyResult = CPA_FALSE;
		drvOpData->lacOpData.ivLenInBytes = DES3_BLOCK_LEN;
		break;
	case CRYPTO_ARC4:
		drvOpData->lacOpData.
		    cryptoStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToCipherInBytes = crp_desc->crd_len;
		drvOpData->verifyResult = CPA_FALSE;
		drvOpData->lacOpData.ivLenInBytes = ARC4_COUNTER_LEN;
		break;
	case CRYPTO_AES_CBC:
		drvOpData->lacOpData.
		    cryptoStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToCipherInBytes = crp_desc->crd_len;
		drvOpData->verifyResult = CPA_FALSE;
		drvOpData->lacOpData.ivLenInBytes = RIJNDAEL128_BLOCK_LEN;
		break;
	case CRYPTO_SHA1:
	case CRYPTO_SHA1_HMAC:
	case CRYPTO_SHA2_256:
	case CRYPTO_SHA2_256_HMAC:
	case CRYPTO_SHA2_384:
	case CRYPTO_SHA2_384_HMAC:
	case CRYPTO_SHA2_512:
	case CRYPTO_SHA2_512_HMAC:
	case CRYPTO_MD5:
	case CRYPTO_MD5_HMAC:
		drvOpData->lacOpData.
		    hashStartSrcOffsetInBytes = crp_desc->crd_skip;
		drvOpData->lacOpData.
		    messageLenToHashInBytes = crp_desc->crd_len;
		drvOpData->lacOpData.
		    pDigestResult =
		    icp_ocfDrvDigestPointerFind(drvOpData, crp_desc);

		if (NULL == drvOpData->lacOpData.pDigestResult) {
			DPRINTK("%s(): ERROR - could not calculate "
				"Digest Result memory address\n", __FUNCTION__);
			return ICP_OCF_DRV_STATUS_FAIL;
		}

		drvOpData->lacOpData.digestVerify = CPA_FALSE;
		break;
	default:
		DPRINTK("%s(): Crypto process error - algorithm not "
			"found \n", __FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}

	/* Figure out what the IV is supposed to be */
	if ((crp_desc->crd_alg == CRYPTO_DES_CBC) ||
	    (crp_desc->crd_alg == CRYPTO_3DES_CBC) ||
	    (crp_desc->crd_alg == CRYPTO_AES_CBC)) {
		/*ARC4 doesn't use an IV */
		if (crp_desc->crd_flags & CRD_F_IV_EXPLICIT) {
			/* Explicit IV provided to OCF */
			drvOpData->lacOpData.pIv = crp_desc->crd_iv;
		} else {
			/* IV is not explicitly provided to OCF */

			/* Point the LAC OP Data IV pointer to our allocated
			   storage location for this session. */
			drvOpData->lacOpData.pIv = drvOpData->ivData;

			if ((crp_desc->crd_flags & CRD_F_ENCRYPT) &&
			    ((crp_desc->crd_flags & CRD_F_IV_PRESENT) == 0)) {

				/* Encrypting - need to create IV */
				randGenOpData.generateBits = CPA_TRUE;
				randGenOpData.lenInBytes = MAX_IV_LEN_IN_BYTES;

				icp_ocfDrvPtrAndLenToFlatBuffer((Cpa8U *)
								drvOpData->
								ivData,
								MAX_IV_LEN_IN_BYTES,
								&randData);

				if (CPA_STATUS_SUCCESS !=
				    cpaCyRandGen(CPA_INSTANCE_HANDLE_SINGLE,
						 NULL, NULL,
						 &randGenOpData, &randData)) {
					DPRINTK("%s(): ERROR - Failed to"
						" generate"
						" Initialisation Vector\n",
						__FUNCTION__);
					return ICP_OCF_DRV_STATUS_FAIL;
				}

				crypto_copyback(drvOpData->crp->
						crp_flags,
						drvOpData->crp->crp_buf,
						crp_desc->crd_inject,
						drvOpData->lacOpData.
						ivLenInBytes,
						(caddr_t) (drvOpData->lacOpData.
							   pIv));
			} else {
				/* Reading IV from buffer */
				crypto_copydata(drvOpData->crp->
						crp_flags,
						drvOpData->crp->crp_buf,
						crp_desc->crd_inject,
						drvOpData->lacOpData.
						ivLenInBytes,
						(caddr_t) (drvOpData->lacOpData.
							   pIv));
			}

		}

	}

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvDigestPointerFind
 *
 * Description : This function is used to find the memory address of where the
 * digest information shall be stored in. Input buffer types are an skbuff, iov
 * or flat buffer. The address is found using the buffer data start address and
 * an offset.
 *
 * Note: In the case of a linux skbuff, the digest address may exist within
 * a memory space linked to from the start buffer. These linked memory spaces
 * must be traversed by the data length offset in order to find the digest start
 * address. Whether there is enough space for the digest must also be checked.
 */

static uint8_t *icp_ocfDrvDigestPointerFind(struct icp_drvOpData *drvOpData,
					    struct cryptodesc *crp_desc)
{

	int offsetInBytes = crp_desc->crd_inject;
	uint32_t digestSizeInBytes = drvOpData->digestSizeInBytes;
	uint8_t *flat_buffer_base = NULL;
	int flat_buffer_length = 0;
	struct sk_buff *skb;

	if (drvOpData->crp->crp_flags & CRYPTO_F_SKBUF) {
		/*check if enough overall space to store hash */
		skb = (struct sk_buff *)(drvOpData->crp->crp_buf);

		if (skb->len < (offsetInBytes + digestSizeInBytes)) {
			DPRINTK("%s() Not enough space for Digest"
				" payload after the offset (%d), "
				"digest size (%d) \n", __FUNCTION__,
				offsetInBytes, digestSizeInBytes);
			return NULL;
		}

		return icp_ocfDrvSkbuffDigestPointerFind(drvOpData,
							 offsetInBytes,
							 digestSizeInBytes);

	} else {
		/* IOV or flat buffer */
		if (drvOpData->crp->crp_flags & CRYPTO_F_IOV) {
			/*single IOV check has already been done */
			flat_buffer_base = ((struct uio *)
					    (drvOpData->crp->crp_buf))->
			    uio_iov[0].iov_base;
			flat_buffer_length = ((struct uio *)
					      (drvOpData->crp->crp_buf))->
			    uio_iov[0].iov_len;
		} else {
			flat_buffer_base = (uint8_t *) drvOpData->crp->crp_buf;
			flat_buffer_length = drvOpData->crp->crp_ilen;
		}

		if (flat_buffer_length < (offsetInBytes + digestSizeInBytes)) {
			DPRINTK("%s() Not enough space for Digest "
				"(IOV/Flat Buffer) \n", __FUNCTION__);
			return NULL;
		} else {
			return (uint8_t *) (flat_buffer_base + offsetInBytes);
		}
	}
	DPRINTK("%s() Should not reach this point\n", __FUNCTION__);
	return NULL;
}

/* Name        : icp_ocfDrvSkbuffDigestPointerFind
 *
 * Description : This function is used by icp_ocfDrvDigestPointerFind to process
 * the non-linear portion of the skbuff if the fragmentation type is a linked
 * list (frag_list is not NULL in the skb_shared_info structure)
 */
static inline uint8_t *icp_ocfDrvSkbuffDigestPointerFind(struct icp_drvOpData
							 *drvOpData,
							 int offsetInBytes,
							 uint32_t
							 digestSizeInBytes)
{

	struct sk_buff *skb = NULL;
	struct skb_shared_info *skb_shared = NULL;

	uint32_t skbuffisnonlinear = 0;

	uint32_t skbheadlen = 0;

	skb = (struct sk_buff *)(drvOpData->crp->crp_buf);
	skbuffisnonlinear = skb_is_nonlinear(skb);

	skbheadlen = skb_headlen(skb);

	/*Linear skb checks */
	if (skbheadlen > offsetInBytes) {

		if (skbheadlen >= (offsetInBytes + digestSizeInBytes)) {
			return (uint8_t *) (skb->data + offsetInBytes);
		} else {
			DPRINTK("%s() Auth payload stretches "
				"accross contiguous memory\n", __FUNCTION__);
			return NULL;
		}
	} else {
		if (skbuffisnonlinear) {
			offsetInBytes -= skbheadlen;
		} else {
			DPRINTK("%s() Offset outside of buffer boundaries\n",
				__FUNCTION__);
			return NULL;
		}
	}

	/*Non Linear checks */
	skb_shared = (struct skb_shared_info *)(skb->end);
	if (unlikely(NULL == skb_shared)) {
		DPRINTK("%s() skbuff shared info stucture is NULL! \n",
			__FUNCTION__);
		return NULL;
	} else if ((0 != skb_shared->nr_frags) &&
		   (skb_shared->frag_list != NULL)) {
		DPRINTK("%s() skbuff nr_frags AND "
			"frag_list not supported \n", __FUNCTION__);
		return NULL;
	}

	/*TCP segmentation more likely than IP fragmentation */
	if (likely(0 != skb_shared->nr_frags)) {
		return icp_ocfDrvDigestSkbNRFragsCheck(skb, skb_shared,
						       offsetInBytes,
						       digestSizeInBytes);
	} else if (skb_shared->frag_list != NULL) {
		return icp_ocfDrvDigestSkbFragListCheck(skb, skb_shared,
							offsetInBytes,
							digestSizeInBytes);
	} else {
		DPRINTK("%s() skbuff is non-linear but does not show any "
			"linked data\n", __FUNCTION__);
		return NULL;
	}

}

/* Name        : icp_ocfDrvDigestSkbNRFragsCheck
 *
 * Description : This function is used by icp_ocfDrvSkbuffDigestPointerFind to
 * process the non-linear portion of the skbuff, if the fragmentation type is
 * page fragments
 */
static inline uint8_t *icp_ocfDrvDigestSkbNRFragsCheck(struct sk_buff *skb,
						       struct skb_shared_info
						       *skb_shared,
						       int offsetInBytes,
						       uint32_t
						       digestSizeInBytes)
{
	int i = 0;
	/*nr_frags starts from 1 */
	if (MAX_SKB_FRAGS < skb_shared->nr_frags) {
		DPRINTK("%s error processing skbuff "
			"page frame -- MAX FRAGS exceeded \n", __FUNCTION__);
		return NULL;
	}

	for (i = 0; i < skb_shared->nr_frags; i++) {

		if (offsetInBytes >= skb_shared->frags[i].size) {
			/*offset still greater than data position */
			offsetInBytes -= skb_shared->frags[i].size;
		} else {
			/* found the page containing start of hash */

			if (NULL == skb_shared->frags[i].page) {
				DPRINTK("%s() Linked page is NULL!\n",
					__FUNCTION__);
				return NULL;
			}

			if (offsetInBytes + digestSizeInBytes >
			    skb_shared->frags[i].size) {
				DPRINTK("%s() Auth payload stretches accross "
					"contiguous memory\n", __FUNCTION__);
				return NULL;
			} else {
				return (uint8_t *) (skb_shared->frags[i].page +
						    skb_shared->frags[i].
						    page_offset +
						    offsetInBytes);
			}
		}
		/*only possible if internal page sizes are set wrong */
		if (offsetInBytes < 0) {
			DPRINTK("%s error processing skbuff page frame "
				"-- offset calculation \n", __FUNCTION__);
			return NULL;
		}
	}
	/*only possible if internal page sizes are set wrong */
	DPRINTK("%s error processing skbuff page frame "
		"-- ran out of page fragments, remaining offset = %d \n",
		__FUNCTION__, offsetInBytes);
	return NULL;

}

/* Name        : icp_ocfDrvDigestSkbFragListCheck
 *
 * Description : This function is used by icp_ocfDrvSkbuffDigestPointerFind to 
 * process the non-linear portion of the skbuff, if the fragmentation type is 
 * a linked list
 * 
 */
static inline uint8_t *icp_ocfDrvDigestSkbFragListCheck(struct sk_buff *skb,
							struct skb_shared_info
							*skb_shared,
							int offsetInBytes,
							uint32_t
							digestSizeInBytes)
{

	struct sk_buff *skb_list = skb_shared->frag_list;
	/*check added for readability */
	if (NULL == skb_list) {
		DPRINTK("%s error processing skbuff "
			"-- no more list! \n", __FUNCTION__);
		return NULL;
	}

	for (; skb_list; skb_list = skb_list->next) {
		if (NULL == skb_list) {
			DPRINTK("%s error processing skbuff "
				"-- no more list! \n", __FUNCTION__);
			return NULL;
		}

		if (offsetInBytes >= skb_list->len) {
			offsetInBytes -= skb_list->len;

		} else {
			if (offsetInBytes + digestSizeInBytes > skb_list->len) {
				DPRINTK("%s() Auth payload stretches accross "
					"contiguous memory\n", __FUNCTION__);
				return NULL;
			} else {
				return (uint8_t *)
				    (skb_list->data + offsetInBytes);
			}

		}

		/*This check is only needed if internal skb_list length values
		   are set wrong. */
		if (0 > offsetInBytes) {
			DPRINTK("%s() error processing skbuff object -- offset "
				"calculation \n", __FUNCTION__);
			return NULL;
		}

	}

	/*catch all for unusual for-loop exit. 
	   This code should never be reached */
	DPRINTK("%s() Catch-All hit! Process error.\n", __FUNCTION__);
	return NULL;
}
