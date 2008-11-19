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
 * An OCF module that uses Intel® QuickAssist Integrated Accelerator to do the 
 * crypto.
 *
 * This driver requires the ICP Access Library that is available from Intel in
 * order to operate.
 */

#include "icp_ocf.h"

#define ICP_OCF_COMP_NAME 			"ICP_OCF"
#define ICP_OCF_VER_MAIN			(2)
#define ICP_OCF_VER_MJR				(0)
#define ICP_OCF_VER_MNR 			(0)

#define MAX_DEREG_RETRIES 			(100)
#define DEFAULT_DEREG_RETRIES 			(10)
#define DEFAULT_DEREG_DELAY_IN_JIFFIES		(10)

/* This defines the maximum number of sessions possible between OCF
   and the OCF Tolapai Driver. If set to zero, there is no limit. */
#define DEFAULT_OCF_TO_DRV_MAX_SESSION_COUNT	(0)
#define NUM_SUPPORTED_CAPABILITIES		(21)

/*Slabs zones*/
struct kmem_cache *drvSessionData_zone = NULL;
struct kmem_cache *drvOpData_zone = NULL;
struct kmem_cache *drvDH_zone = NULL;
struct kmem_cache *drvLnModExp_zone = NULL;
struct kmem_cache *drvRSADecrypt_zone = NULL;
struct kmem_cache *drvRSAPrivateKey_zone = NULL;
struct kmem_cache *drvDSARSSign_zone = NULL;
struct kmem_cache *drvDSARSSignKValue_zone = NULL;
struct kmem_cache *drvDSAVerify_zone = NULL;

/*Slab zones for flatbuffers and bufferlist*/
struct kmem_cache *drvFlatBuffer_zone = NULL;

static int icp_ocfDrvInit(void);
static void icp_ocfDrvExit(void);
static void icp_ocfDrvFreeCaches(void);
static void icp_ocfDrvDeferedFreeLacSessionProcess(void *arg);

int32_t icp_ocfDrvDriverId = INVALID_DRIVER_ID;

/* Module parameter - gives the number of times LAC deregistration shall be
   re-tried */
int num_dereg_retries = DEFAULT_DEREG_RETRIES;

/* Module parameter - gives the delay time in jiffies before a LAC session 
   shall be attempted to be deregistered again */
int dereg_retry_delay_in_jiffies = DEFAULT_DEREG_DELAY_IN_JIFFIES;

/* Module parameter - gives the maximum number of sessions possible between
   OCF and the OCF Tolapai Driver. If set to zero, there is no limit.*/
int max_sessions = DEFAULT_OCF_TO_DRV_MAX_SESSION_COUNT;

/* This is set when the module is removed from the system, no further
   processing can take place if this is set */
atomic_t icp_ocfDrvIsExiting = ATOMIC_INIT(0);

/* This is used to show how many lac sessions were not deregistered*/
atomic_t lac_session_failed_dereg_count = ATOMIC_INIT(0);

/* This is used to track the number of registered sessions between OCF and
 * and the OCF Tolapai driver, when max_session is set to value other than
 * zero. This ensures that the max_session set for the OCF and the driver
 * is equal to the LAC registered sessions */
atomic_t num_ocf_to_drv_registered_sessions = ATOMIC_INIT(0);

/* Head of linked list used to store session data */
struct list_head icp_ocfDrvGlobalSymListHead;
struct list_head icp_ocfDrvGlobalSymListHead_FreeMemList;

spinlock_t icp_ocfDrvSymSessInfoListSpinlock = SPIN_LOCK_UNLOCKED;
rwlock_t icp_kmem_cache_destroy_alloc_lock = RW_LOCK_UNLOCKED;

struct workqueue_struct *icp_ocfDrvFreeLacSessionWorkQ;

struct icp_drvBuffListInfo defBuffListInfo;

static struct {
	softc_device_decl sc_dev;
} icpDev;

static device_method_t icp_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession, icp_ocfDrvNewSession),
	DEVMETHOD(cryptodev_freesession, icp_ocfDrvFreeLACSession),
	DEVMETHOD(cryptodev_process, icp_ocfDrvSymProcess),
	DEVMETHOD(cryptodev_kprocess, icp_ocfDrvPkeProcess),
};

module_param(num_dereg_retries, int, S_IRUGO);
module_param(dereg_retry_delay_in_jiffies, int, S_IRUGO);
module_param(max_sessions, int, S_IRUGO);

MODULE_PARM_DESC(num_dereg_retries,
		 "Number of times to retry LAC Sym Session Deregistration. "
		 "Default 10, Max 100");
MODULE_PARM_DESC(dereg_retry_delay_in_jiffies, "Delay in jiffies "
		 "(added to a schedule() function call) before a LAC Sym "
		 "Session Dereg is retried. Default 10");
MODULE_PARM_DESC(max_sessions, "This sets the maximum number of sessions "
		 "between OCF and this driver. If this value is set to zero, "
		 "max session count checking is disabled. Default is zero(0)");

/* Name        : icp_ocfDrvInit
 *
 * Description : This function will register all the symmetric and asymmetric
 * functionality that will be accelerated by the hardware. It will also
 * get a unique driver ID from the OCF and initialise all slab caches
 */
static int __init icp_ocfDrvInit(void)
{
	int ocfStatus = 0;

	IPRINTK("=== %s ver %d.%d.%d ===\n", ICP_OCF_COMP_NAME,
		ICP_OCF_VER_MAIN, ICP_OCF_VER_MJR, ICP_OCF_VER_MNR);

	if (MAX_DEREG_RETRIES < num_dereg_retries) {
		EPRINTK("Session deregistration retry count set to greater "
			"than %d", MAX_DEREG_RETRIES);
		return -1;
	}

	/* Initialize and Start the Cryptographic component */
	if (CPA_STATUS_SUCCESS !=
	    cpaCyStartInstance(CPA_INSTANCE_HANDLE_SINGLE)) {
		EPRINTK("Failed to initialize and start the instance "
			"of the Cryptographic component.\n");
		return -1;
	}

	/* Set the default size of BufferList to allocate */
	memset(&defBuffListInfo, 0, sizeof(struct icp_drvBuffListInfo));
	if (ICP_OCF_DRV_STATUS_SUCCESS !=
	    icp_ocfDrvBufferListMemInfo(ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS,
					&defBuffListInfo)) {
		EPRINTK("Failed to get bufferlist memory info.\n");
		return -1;
	}

	/*Register OCF Tolapai Driver with OCF */
	memset(&icpDev, 0, sizeof(icpDev));
	softc_device_init(&icpDev, "icp", 0, icp_methods);

	icp_ocfDrvDriverId = crypto_get_driverid(softc_get_device(&icpDev),
						 CRYPTOCAP_F_HARDWARE);

	if (icp_ocfDrvDriverId < 0) {
		EPRINTK("%s : ICP driver failed to register with OCF!\n",
			__FUNCTION__);
		return -ENODEV;
	}

	/*Create all the slab caches used by the OCF Tolapai Driver */
	drvSessionData_zone =
	    ICP_CACHE_CREATE("ICP Session Data", struct icp_drvSessionData);
	ICP_CACHE_NULL_CHECK(drvSessionData_zone);

	/* 
	 * Allocation of the OpData includes the allocation space for meta data.
	 * The memory after the opData structure is reserved for this meta data.
	 */
	drvOpData_zone =
	    kmem_cache_create("ICP Op Data", sizeof(struct icp_drvOpData) +
	            defBuffListInfo.metaSize ,0, SLAB_HWCACHE_ALIGN, NULL, NULL);


	ICP_CACHE_NULL_CHECK(drvOpData_zone);

	drvDH_zone = ICP_CACHE_CREATE("ICP DH data", CpaCyDhPhase1KeyGenOpData);
	ICP_CACHE_NULL_CHECK(drvDH_zone);

	drvLnModExp_zone =
	    ICP_CACHE_CREATE("ICP ModExp data", CpaCyLnModExpOpData);
	ICP_CACHE_NULL_CHECK(drvLnModExp_zone);

	drvRSADecrypt_zone =
	    ICP_CACHE_CREATE("ICP RSA decrypt data", CpaCyRsaDecryptOpData);
	ICP_CACHE_NULL_CHECK(drvRSADecrypt_zone);

	drvRSAPrivateKey_zone =
	    ICP_CACHE_CREATE("ICP RSA private key data", CpaCyRsaPrivateKey);
	ICP_CACHE_NULL_CHECK(drvRSAPrivateKey_zone);

	drvDSARSSign_zone =
	    ICP_CACHE_CREATE("ICP DSA Sign", CpaCyDsaRSSignOpData);
	ICP_CACHE_NULL_CHECK(drvDSARSSign_zone);

	/*too awkward to use a macro here */
	drvDSARSSignKValue_zone =
	    kmem_cache_create("ICP DSA Sign Rand Val",
			      DSA_RS_SIGN_PRIMEQ_SIZE_IN_BYTES, 0,
			      SLAB_HWCACHE_ALIGN, NULL, NULL);
	ICP_CACHE_NULL_CHECK(drvDSARSSignKValue_zone);

	drvDSAVerify_zone =
	    ICP_CACHE_CREATE("ICP DSA Verify", CpaCyDsaVerifyOpData);
	ICP_CACHE_NULL_CHECK(drvDSAVerify_zone);

	drvFlatBuffer_zone =
	    ICP_CACHE_CREATE("ICP Flat Buffers", CpaFlatBuffer);
	ICP_CACHE_NULL_CHECK(drvFlatBuffer_zone);

	/* Register the ICP symmetric crypto support. */
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_NULL_CBC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_DES_CBC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_3DES_CBC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_AES_CBC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_ARC4);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_MD5);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_MD5_HMAC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA1);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA1_HMAC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_256);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_256_HMAC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_384);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_384_HMAC);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_512);
	ICP_REGISTER_SYM_FUNCTIONALITY_WITH_OCF(CRYPTO_SHA2_512_HMAC);

	/* Register the ICP asymmetric algorithm support */
	ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(CRK_DH_COMPUTE_KEY);
	ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(CRK_MOD_EXP);
	ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(CRK_MOD_EXP_CRT);
	ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(CRK_DSA_SIGN);
	ICP_REGISTER_ASYM_FUNCTIONALITY_WITH_OCF(CRK_DSA_VERIFY);

	/* Register the ICP random number generator support */
	if (OCF_REGISTRATION_STATUS_SUCCESS ==
	    crypto_rregister(icp_ocfDrvDriverId, icp_ocfDrvReadRandom, NULL)) {
		ocfStatus++;
	}

	if (OCF_ZERO_FUNCTIONALITY_REGISTERED == ocfStatus) {
		DPRINTK("%s: Failed to register any device capabilities\n",
			__FUNCTION__);
		icp_ocfDrvFreeCaches();
		icp_ocfDrvDriverId = INVALID_DRIVER_ID;
		return -ECANCELED;
	}

	DPRINTK("%s: Registered %d of %d device capabilities\n",
		__FUNCTION__, ocfStatus, NUM_SUPPORTED_CAPABILITIES);

/*Session data linked list used during module exit*/
	INIT_LIST_HEAD(&icp_ocfDrvGlobalSymListHead);
	INIT_LIST_HEAD(&icp_ocfDrvGlobalSymListHead_FreeMemList);

	icp_ocfDrvFreeLacSessionWorkQ =
	    create_singlethread_workqueue("ocfLacDeregWorkQueue");

	return 0;
}

/* Name        : icp_ocfDrvExit
 *
 * Description : This function will deregister all the symmetric sessions
 * registered with the LAC component. It will also deregister all symmetric
 * and asymmetric functionality that can be accelerated by the hardware via OCF
 * and random number generation if it is enabled.
 */
static void icp_ocfDrvExit(void)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	struct icp_drvSessionData *sessionData = NULL;
	struct icp_drvSessionData *tempSessionData = NULL;
	int i, remaining_delay_time_in_jiffies = 0;
	/* There is a possibility of a process or new session command being   */
	/* sent before this variable is incremented. The aim of this variable */
	/* is to stop a loop of calls creating a deadlock situation which     */
	/* would prevent the driver from exiting.                             */

	atomic_inc(&icp_ocfDrvIsExiting);

	/*Existing sessions will be routed to another driver after these calls */
	crypto_unregister_all(icp_ocfDrvDriverId);
	crypto_runregister_all(icp_ocfDrvDriverId);

	/*If any sessions are waiting to be deregistered, do that. This also 
	   flushes the work queue */
	destroy_workqueue(icp_ocfDrvFreeLacSessionWorkQ);

	/*ENTER CRITICAL SECTION */
	spin_lock_bh(&icp_ocfDrvSymSessInfoListSpinlock);
	list_for_each_entry_safe(tempSessionData, sessionData,
				 &icp_ocfDrvGlobalSymListHead, listNode) {
		for (i = 0; i < num_dereg_retries; i++) {
			/*No harm if bad input - LAC will handle error cases */
			if (ICP_SESSION_RUNNING == tempSessionData->inUse) {
				lacStatus =
				    cpaCySymRemoveSession
				    (CPA_INSTANCE_HANDLE_SINGLE,
				     tempSessionData->sessHandle);
				if (CPA_STATUS_SUCCESS == lacStatus) {
					/* Succesfully deregistered */
					break;
				} else if (CPA_STATUS_RETRY != lacStatus) {
					atomic_inc
					    (&lac_session_failed_dereg_count);
					break;
				}

				/*schedule_timout returns the time left for completion if 
				 * this task is set to TASK_INTERRUPTIBLE */
				remaining_delay_time_in_jiffies =
				    dereg_retry_delay_in_jiffies;
				while (0 > remaining_delay_time_in_jiffies) {
					remaining_delay_time_in_jiffies =
					    schedule_timeout
					    (remaining_delay_time_in_jiffies);
				}

				DPRINTK
				    ("%s(): Retry %d to deregistrate the session\n",
				     __FUNCTION__, i);
			}
		}

		/*remove from current list */
		list_del(&(tempSessionData->listNode));
		/*add to free mem linked list */
		list_add(&(tempSessionData->listNode),
			 &icp_ocfDrvGlobalSymListHead_FreeMemList);

	}

	/*EXIT CRITICAL SECTION */
	spin_unlock_bh(&icp_ocfDrvSymSessInfoListSpinlock);

	/*set back to initial values */
	sessionData = NULL;
	/*still have a reference in our list! */
	tempSessionData = NULL;
	/*free memory */
	list_for_each_entry_safe(tempSessionData, sessionData,
				 &icp_ocfDrvGlobalSymListHead_FreeMemList,
				 listNode) {

		list_del(&(tempSessionData->listNode));
		/* Free allocated CpaCySymSessionCtx */
		if (NULL != tempSessionData->sessHandle) {
			kfree(tempSessionData->sessHandle);
		}
		memset(tempSessionData, 0, sizeof(struct icp_drvSessionData));
		kmem_cache_free(drvSessionData_zone, tempSessionData);
	}

	if (0 != atomic_read(&lac_session_failed_dereg_count)) {
		DPRINTK("%s(): %d LAC sessions were not deregistered "
			"correctly. This is not a clean exit! \n",
			__FUNCTION__,
			atomic_read(&lac_session_failed_dereg_count));
	}

	icp_ocfDrvFreeCaches();
	icp_ocfDrvDriverId = INVALID_DRIVER_ID;

	/* Shutdown the Cryptographic component */
	lacStatus = cpaCyStopInstance(CPA_INSTANCE_HANDLE_SINGLE);
	if (CPA_STATUS_SUCCESS != lacStatus) {
		DPRINTK("%s(): Failed to stop instance of the "
			"Cryptographic component.(status == %d)\n",
			__FUNCTION__, lacStatus);
	}

}

/* Name        : icp_ocfDrvFreeCaches
 *
 * Description : This function deregisters all slab caches
 */
static void icp_ocfDrvFreeCaches(void)
{
	if (atomic_read(&icp_ocfDrvIsExiting) != CPA_TRUE) {
		atomic_set(&icp_ocfDrvIsExiting, 1);
	}

	/*Sym Zones */
	ICP_CACHE_DESTROY(drvSessionData_zone);
	ICP_CACHE_DESTROY(drvOpData_zone);

	/*Asym zones */
	ICP_CACHE_DESTROY(drvDH_zone);
	ICP_CACHE_DESTROY(drvLnModExp_zone);
	ICP_CACHE_DESTROY(drvRSADecrypt_zone);
	ICP_CACHE_DESTROY(drvRSAPrivateKey_zone);
	ICP_CACHE_DESTROY(drvDSARSSignKValue_zone);
	ICP_CACHE_DESTROY(drvDSARSSign_zone);
	ICP_CACHE_DESTROY(drvDSAVerify_zone);

	/*FlatBuffer and BufferList Zones */
	ICP_CACHE_DESTROY(drvFlatBuffer_zone);

}

/* Name        : icp_ocfDrvDeregRetry
 *
 * Description : This function will try to farm the session deregistration
 * off to a work queue. If it fails, nothing more can be done and it
 * returns an error
 */

int icp_ocfDrvDeregRetry(CpaCySymSessionCtx sessionToDeregister)
{
	struct icp_ocfDrvFreeLacSession *workstore = NULL;

	DPRINTK("%s(): Retry - Deregistering session (%p)\n",
		__FUNCTION__, sessionToDeregister);

	/*make sure the session is not available to be allocated during this
	   process */
	atomic_inc(&lac_session_failed_dereg_count);

	/*Farm off to work queue */
	workstore =
	    kmalloc(sizeof(struct icp_ocfDrvFreeLacSession), GFP_ATOMIC);
	if (NULL == workstore) {
		DPRINTK("%s(): unable to free session - no memory available "
			"for work queue\n", __FUNCTION__);
		return ENOMEM;
	}

	workstore->sessionToDeregister = sessionToDeregister;

	INIT_WORK(&(workstore->work), icp_ocfDrvDeferedFreeLacSessionProcess,
		  workstore);
	queue_work(icp_ocfDrvFreeLacSessionWorkQ, &(workstore->work));

	return ICP_OCF_DRV_STATUS_SUCCESS;

}

/* Name        : icp_ocfDrvDeferedFreeLacSessionProcess
 *
 * Description : This function will retry (module input parameter)
 * 'num_dereg_retries' times to deregister any symmetric session that recieves a
 * CPA_STATUS_RETRY message from the LAC component. This function is run in
 * Thread context because it is called from a worker thread
 */
static void icp_ocfDrvDeferedFreeLacSessionProcess(void *arg)
{
	struct icp_ocfDrvFreeLacSession *workstore = NULL;
	CpaCySymSessionCtx sessionToDeregister = NULL;
	int i = 0;
	int remaining_delay_time_in_jiffies = 0;
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;

	workstore = (struct icp_ocfDrvFreeLacSession *)arg;
	if (NULL == workstore) {
		DPRINTK("%s() function called with null parameter \n",
			__FUNCTION__);
		return;
	}

	sessionToDeregister = workstore->sessionToDeregister;
	kfree(workstore);

	/*if exiting, give deregistration one more blast only */
	if (atomic_read(&icp_ocfDrvIsExiting) == CPA_TRUE) {
		lacStatus = cpaCySymRemoveSession(CPA_INSTANCE_HANDLE_SINGLE,
						  sessionToDeregister);

		if (lacStatus != CPA_STATUS_SUCCESS) {
			DPRINTK("%s() Failed to Dereg LAC session %p "
				"during module exit\n", __FUNCTION__,
				sessionToDeregister);
			return;
		}

		atomic_dec(&lac_session_failed_dereg_count);
		return;
	}

	for (i = 0; i <= num_dereg_retries; i++) {
		lacStatus = cpaCySymRemoveSession(CPA_INSTANCE_HANDLE_SINGLE,
						  sessionToDeregister);

		if (lacStatus == CPA_STATUS_SUCCESS) {
			atomic_dec(&lac_session_failed_dereg_count);
			return;
		}
		if (lacStatus != CPA_STATUS_RETRY) {
			DPRINTK("%s() Failed to deregister session - lacStatus "
				" = %d", __FUNCTION__, lacStatus);
			break;
		}

		/*schedule_timout returns the time left for completion if this
		   task is set to TASK_INTERRUPTIBLE */
		remaining_delay_time_in_jiffies = dereg_retry_delay_in_jiffies;
		while (0 > remaining_delay_time_in_jiffies) {
			remaining_delay_time_in_jiffies =
			    schedule_timeout(remaining_delay_time_in_jiffies);
		}

	}

	DPRINTK("%s(): Unable to deregister session\n", __FUNCTION__);
	DPRINTK("%s(): Number of unavailable LAC sessions = %d\n", __FUNCTION__,
		atomic_read(&lac_session_failed_dereg_count));
}

/* Name        : icp_ocfDrvPtrAndLenToFlatBuffer 
 *
 * Description : This function converts a "pointer and length" buffer 
 * structure to Fredericksburg Flat Buffer (CpaFlatBuffer) format.
 *
 * This function assumes that the data passed in are valid.
 */
inline void
icp_ocfDrvPtrAndLenToFlatBuffer(void *pData, uint32_t len,
				CpaFlatBuffer * pFlatBuffer)
{
	pFlatBuffer->pData = pData;
	pFlatBuffer->dataLenInBytes = len;
}

/* Name        : icp_ocfDrvSingleSkBuffToFlatBuffer 
 *
 * Description : This function converts a single socket buffer (sk_buff)
 * structure to a Fredericksburg Flat Buffer (CpaFlatBuffer) format.
 *
 * This function assumes that the data passed in are valid.
 */
static inline void
icp_ocfDrvSingleSkBuffToFlatBuffer(struct sk_buff *pSkb,
				   CpaFlatBuffer * pFlatBuffer)
{
	pFlatBuffer->pData = pSkb->data;
	pFlatBuffer->dataLenInBytes = skb_headlen(pSkb);
}

/* Name        : icp_ocfDrvSkBuffToBufferList 
 *
 * Description : This function converts a socket buffer (sk_buff) structure to
 * Fredericksburg Scatter/Gather (CpaBufferList) buffer format.
 *
 * This function assumes that the bufferlist has been allocated with the correct
 * number of buffer arrays.
 * 
 */
inline int
icp_ocfDrvSkBuffToBufferList(struct sk_buff *pSkb, CpaBufferList * bufferList)
{
	CpaFlatBuffer *curFlatBuffer = NULL;
	char *skbuffPageAddr = NULL;
	struct sk_buff *pCurFrag = NULL;
	struct skb_shared_info *pShInfo = NULL;
	uint32_t page_offset = 0, i = 0;

	DPRINTK("%s(): Entry Point\n", __FUNCTION__);

	/*
	 * In all cases, the first skb needs to be translated to FlatBuffer.
	 * Perform a buffer translation for the first skbuff
	 */
	curFlatBuffer = bufferList->pBuffers;
	icp_ocfDrvSingleSkBuffToFlatBuffer(pSkb, curFlatBuffer);

	/* Set the userData to point to the original sk_buff */
	bufferList->pUserData = (void *)pSkb;

	/* We now know we'll have at least one element in the SGL */
	bufferList->numBuffers = 1;

	if (0 == skb_is_nonlinear(pSkb)) {
		/* Is a linear buffer - therefore it's a single skbuff */
		DPRINTK("%s(): Exit Point\n", __FUNCTION__);
		return ICP_OCF_DRV_STATUS_SUCCESS;
	}

	curFlatBuffer++;
	pShInfo = skb_shinfo(pSkb);
	if (pShInfo->frag_list != NULL && pShInfo->nr_frags != 0) {
		EPRINTK("%s():"
			"Translation for a combination of frag_list "
			"and frags[] array not supported!\n", __FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	} else if (pShInfo->frag_list != NULL) {
		/*
		 * Non linear skbuff supported through frag_list 
		 * Perform translation for each fragment (sk_buff)
		 * in the frag_list of the first sk_buff.
		 */
		for (pCurFrag = pShInfo->frag_list;
		     pCurFrag != NULL; pCurFrag = pCurFrag->next) {
			icp_ocfDrvSingleSkBuffToFlatBuffer(pCurFrag,
							   curFlatBuffer);
			curFlatBuffer++;
			bufferList->numBuffers++;
		}
	} else if (pShInfo->nr_frags != 0) {
		/*
		 * Perform translation for each fragment in frags array
		 * and add to the BufferList
		 */
		for (i = 0; i < pShInfo->nr_frags; i++) {
			/* Get the page address and offset of this frag */
			skbuffPageAddr = (char *)pShInfo->frags[i].page;
			page_offset = pShInfo->frags[i].page_offset;

			/* Convert a pointer and length to a flat buffer */
			icp_ocfDrvPtrAndLenToFlatBuffer(skbuffPageAddr +
							page_offset,
							pShInfo->frags[i].size,
							curFlatBuffer);
			curFlatBuffer++;
			bufferList->numBuffers++;
		}
	} else {
		EPRINTK("%s():" "Could not recognize skbuff fragments!\n",
			__FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}

	DPRINTK("%s(): Exit Point\n", __FUNCTION__);
	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvBufferListToSkBuff 
 *
 * Description : This function converts a Fredericksburg Scatter/Gather 
 * (CpaBufferList) buffer format to socket buffer structure.
 */
inline int
icp_ocfDrvBufferListToSkBuff(CpaBufferList * bufferList, struct sk_buff **skb)
{
	DPRINTK("%s(): Entry Point\n", __FUNCTION__);

	/* Retrieve the orignal skbuff */
	*skb = (struct sk_buff *)bufferList->pUserData;
	if (NULL == *skb) {
		EPRINTK("%s():"
			"Error on converting from a BufferList. "
			"The BufferList does not contain an sk_buff.\n",
			__FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}
	DPRINTK("%s(): Exit Point\n", __FUNCTION__);
	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvPtrAndLenToBufferList
 *
 * Description : This function converts a "pointer and length" buffer
 * structure to Fredericksburg Scatter/Gather Buffer (CpaBufferList) format.
 *
 * This function assumes that the data passed in are valid.
 */
inline void
icp_ocfDrvPtrAndLenToBufferList(void *pDataIn, uint32_t length,
				CpaBufferList * pBufferList)
{
	pBufferList->numBuffers = 1;
	pBufferList->pBuffers->pData = pDataIn;
	pBufferList->pBuffers->dataLenInBytes = length;
}

/* Name        : icp_ocfDrvBufferListToPtrAndLen
 *
 * Description : This function converts Fredericksburg Scatter/Gather Buffer
 * (CpaBufferList) format to a "pointer and length" buffer structure.
 *
 * This function assumes that the data passed in are valid.
 */
inline void
icp_ocfDrvBufferListToPtrAndLen(CpaBufferList * pBufferList,
				void **ppDataOut, uint32_t * pLength)
{
	*ppDataOut = pBufferList->pBuffers->pData;
	*pLength = pBufferList->pBuffers->dataLenInBytes;
}

/* Name        : icp_ocfDrvBufferListMemInfo
 *
 * Description : This function will set the number of flat buffers in 
 * bufferlist, the size of memory to allocate for the pPrivateMetaData 
 * member of the CpaBufferList.
 */
int
icp_ocfDrvBufferListMemInfo(uint16_t numBuffers,
			    struct icp_drvBuffListInfo *buffListInfo)
{
	buffListInfo->numBuffers = numBuffers;

	if (CPA_STATUS_SUCCESS !=
	    cpaCyBufferListGetMetaSize(CPA_INSTANCE_HANDLE_SINGLE,
				       buffListInfo->numBuffers,
				       &(buffListInfo->metaSize))) {
		EPRINTK("%s() Failed to get buffer list meta size.\n",
			__FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvGetSkBuffFrags
 *
 * Description : This function will determine the number of 
 * fragments in a socket buffer(sk_buff).
 */
inline uint16_t icp_ocfDrvGetSkBuffFrags(struct sk_buff * pSkb)
{
	uint16_t numFrags = 0;
	struct sk_buff *pCurFrag = NULL;
	struct skb_shared_info *pShInfo = NULL;

	if (NULL == pSkb)
		return 0;

	numFrags = 1;
	if (0 == skb_is_nonlinear(pSkb)) {
		/* Linear buffer - it's a single skbuff */
		return numFrags;
	}

	pShInfo = skb_shinfo(pSkb);
	if (NULL != pShInfo->frag_list && 0 != pShInfo->nr_frags) {
		EPRINTK("%s(): Combination of frag_list "
			"and frags[] array not supported!\n", __FUNCTION__);
		return 0;
	} else if (0 != pShInfo->nr_frags) {
		numFrags += pShInfo->nr_frags;
		return numFrags;
	} else if (NULL != pShInfo->frag_list) {
		for (pCurFrag = pShInfo->frag_list;
		     pCurFrag != NULL; pCurFrag = pCurFrag->next) {
			numFrags++;
		}
		return numFrags;
	} else {
		return 0;
	}
}

/* Name        : icp_ocfDrvFreeFlatBuffer
 *
 * Description : This function will deallocate flat buffer.
 */
inline void icp_ocfDrvFreeFlatBuffer(CpaFlatBuffer * pFlatBuffer)
{
	if (pFlatBuffer != NULL) {
		memset(pFlatBuffer, 0, sizeof(CpaFlatBuffer));
		kmem_cache_free(drvFlatBuffer_zone, pFlatBuffer);
	}
}

/* Name        : icp_ocfDrvAllocMetaData
 *
 * Description : This function will allocate memory for the
 * pPrivateMetaData member of CpaBufferList.
 */
inline int
icp_ocfDrvAllocMetaData(CpaBufferList * pBufferList,
        const struct icp_drvOpData *pOpData)
{
	Cpa32U metaSize = 0;

	if (pBufferList->numBuffers <= ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS){
	    void *pOpDataStartAddr = (void *)pOpData;

	    if (0 == defBuffListInfo.metaSize) {
			pBufferList->pPrivateMetaData = NULL;
			return ICP_OCF_DRV_STATUS_SUCCESS;
		}
		/*
		 * The meta data allocation has been included as part of the 
		 * op data.  It has been pre-allocated in memory just after the
		 * icp_drvOpData structure.
		 */
		pBufferList->pPrivateMetaData = pOpDataStartAddr +
		        sizeof(struct icp_drvOpData);
	} else {
		if (CPA_STATUS_SUCCESS !=
		    cpaCyBufferListGetMetaSize(CPA_INSTANCE_HANDLE_SINGLE,
					       pBufferList->numBuffers,
					       &metaSize)) {
			EPRINTK("%s() Failed to get buffer list meta size.\n",
				__FUNCTION__);
			return ICP_OCF_DRV_STATUS_FAIL;
		}

		if (0 == metaSize) {
			pBufferList->pPrivateMetaData = NULL;
			return ICP_OCF_DRV_STATUS_SUCCESS;
		}

		pBufferList->pPrivateMetaData = kmalloc(metaSize, GFP_ATOMIC);
	}
	if (NULL == pBufferList->pPrivateMetaData) {
		EPRINTK("%s() Failed to allocate pPrivateMetaData.\n",
			__FUNCTION__);
		return ICP_OCF_DRV_STATUS_FAIL;
	}

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvFreeMetaData
 *
 * Description : This function will deallocate pPrivateMetaData memory.
 */
inline void icp_ocfDrvFreeMetaData(CpaBufferList * pBufferList)
{
	if (NULL == pBufferList->pPrivateMetaData) {
		return;
	}

	/*
	 * Only free the meta data if the BufferList has more than 
	 * ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS number of buffers.
	 * Otherwise, the meta data shall be freed when the icp_drvOpData is
	 * freed.
	 */
	if (ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS < pBufferList->numBuffers){
		kfree(pBufferList->pPrivateMetaData);
	}
}

module_init(icp_ocfDrvInit);
module_exit(icp_ocfDrvExit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Intel");
MODULE_DESCRIPTION("OCF Driver for Intel Quick Assist crypto acceleration");
