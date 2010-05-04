/*************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or 
 *   redistributing this file, you may do so under either license.
 * 
 *   GPL LICENSE SUMMARY
 * 
 *   Copyright(c) 2007,2008,2009 Intel Corporation. All rights reserved.
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
 *   Copyright(c) 2007,2008,2009 Intel Corporation. All rights reserved.
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
 *  version: Security.L.1.0.2-229
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

#define ICP_OCF_COMP_NAME                       "ICP_OCF"
#define ICP_OCF_VER_MAIN                        (2)
#define ICP_OCF_VER_MJR                         (1)
#define ICP_OCF_VER_MNR                         (0)

#define MAX_DEREG_RETRIES                       (100)
#define DEFAULT_DEREG_RETRIES 			(10)
#define DEFAULT_DEREG_DELAY_IN_JIFFIES		(10)

/* This defines the maximum number of sessions possible between OCF
   and the OCF EP80579 Driver. If set to zero, there is no limit. */
#define DEFAULT_OCF_TO_DRV_MAX_SESSION_COUNT    (0)
#define NUM_SUPPORTED_CAPABILITIES              (21)

/*Slab zone names*/
#define ICP_SESSION_DATA_NAME   "icp_ocf.SesDat"
#define ICP_OP_DATA_NAME        "icp_ocf.OpDat"
#define ICP_DH_NAME             "icp_ocf.DH"
#define ICP_MODEXP_NAME         "icp_ocf.ModExp"
#define ICP_RSA_DECRYPT_NAME    "icp_ocf.RSAdec"
#define ICP_RSA_PKEY_NAME       "icp_ocf.RSApk"
#define ICP_DSA_SIGN_NAME       "icp_ocf.DSAsg"
#define ICP_DSA_VER_NAME        "icp_ocf.DSAver"
#define ICP_RAND_VAL_NAME       "icp_ocf.DSArnd"
#define ICP_FLAT_BUFF_NAME      "icp_ocf.FB"

/*Slabs zones*/
icp_kmem_cache drvSessionData_zone = NULL;
icp_kmem_cache drvOpData_zone = NULL;
icp_kmem_cache drvDH_zone = NULL;
icp_kmem_cache drvLnModExp_zone = NULL;
icp_kmem_cache drvRSADecrypt_zone = NULL;
icp_kmem_cache drvRSAPrivateKey_zone = NULL;
icp_kmem_cache drvDSARSSign_zone = NULL;
icp_kmem_cache drvDSARSSignKValue_zone = NULL;
icp_kmem_cache drvDSAVerify_zone = NULL;

/*Slab zones for flatbuffers and bufferlist*/
icp_kmem_cache drvFlatBuffer_zone = NULL;

static inline int icp_cache_null_check(void)
{
	return (drvSessionData_zone && drvOpData_zone
		&& drvDH_zone && drvLnModExp_zone && drvRSADecrypt_zone
		&& drvRSAPrivateKey_zone && drvDSARSSign_zone
		&& drvDSARSSign_zone && drvDSARSSignKValue_zone
		&& drvDSAVerify_zone && drvFlatBuffer_zone);
}

/*Function to free all allocated slab caches before exiting the module*/
static void icp_ocfDrvFreeCaches(void);

int32_t icp_ocfDrvDriverId = INVALID_DRIVER_ID;

/* Module parameter - gives the number of times LAC deregistration shall be
   re-tried */
int num_dereg_retries = DEFAULT_DEREG_RETRIES;

/* Module parameter - gives the delay time in jiffies before a LAC session 
   shall be attempted to be deregistered again */
int dereg_retry_delay_in_jiffies = DEFAULT_DEREG_DELAY_IN_JIFFIES;

/* Module parameter - gives the maximum number of sessions possible between
   OCF and the OCF EP80579 Driver. If set to zero, there is no limit.*/
int max_sessions = DEFAULT_OCF_TO_DRV_MAX_SESSION_COUNT;

/* This is set when the module is removed from the system, no further
   processing can take place if this is set */
icp_atomic_t icp_ocfDrvIsExiting = ICP_ATOMIC_INIT(0);

/* This is used to show how many lac sessions were not deregistered*/
icp_atomic_t lac_session_failed_dereg_count = ICP_ATOMIC_INIT(0);

/* This is used to track the number of registered sessions between OCF and
 * and the OCF EP80579 driver, when max_session is set to value other than
 * zero. This ensures that the max_session set for the OCF and the driver
 * is equal to the LAC registered sessions */
icp_atomic_t num_ocf_to_drv_registered_sessions = ICP_ATOMIC_INIT(0);

/* Head of linked list used to store session data */
icp_drvSessionListHead_t icp_ocfDrvGlobalSymListHead;
icp_drvSessionListHead_t icp_ocfDrvGlobalSymListHead_FreeMemList;

icp_spinlock_t icp_ocfDrvSymSessInfoListSpinlock;

/*Below pointer is only used in linux, FreeBSD uses the name to
create its own variable name*/
icp_workqueue *icp_ocfDrvFreeLacSessionWorkQ = NULL;
ICP_WORKQUEUE_DEFINE_THREAD(icp_ocfDrvFreeLacSessionWorkQ);

struct icp_drvBuffListInfo defBuffListInfo;

/* Name        : icp_ocfDrvInit
 *
 * Description : This function will register all the symmetric and asymmetric
 * functionality that will be accelerated by the hardware. It will also
 * get a unique driver ID from the OCF and initialise all slab caches
 */
ICP_MODULE_INIT_FUNC(icp_ocfDrvInit)
{
	int ocfStatus = 0;

	IPRINTK("=== %s ver %d.%d.%d ===\n", ICP_OCF_COMP_NAME,
		ICP_OCF_VER_MAIN, ICP_OCF_VER_MJR, ICP_OCF_VER_MNR);

	if (MAX_DEREG_RETRIES < num_dereg_retries) {
		EPRINTK("Session deregistration retry count set to greater "
			"than %d", MAX_DEREG_RETRIES);
		icp_module_return_code(EINVAL);
	}

	/* Initialize and Start the Cryptographic component */
	if (CPA_STATUS_SUCCESS !=
	    cpaCyStartInstance(CPA_INSTANCE_HANDLE_SINGLE)) {
		EPRINTK("Failed to initialize and start the instance "
			"of the Cryptographic component.\n");
		return icp_module_return_code(EINVAL);
	}

	icp_spin_lock_init(&icp_ocfDrvSymSessInfoListSpinlock);

	/* Set the default size of BufferList to allocate */
	memset(&defBuffListInfo, 0, sizeof(struct icp_drvBuffListInfo));
	if (ICP_OCF_DRV_STATUS_SUCCESS !=
	    icp_ocfDrvBufferListMemInfo(ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS,
					&defBuffListInfo)) {
		EPRINTK("Failed to get bufferlist memory info.\n");
		return icp_module_return_code(ENOMEM);
	}

	/*Register OCF EP80579 Driver with OCF */
	icp_ocfDrvDriverId = ICP_CRYPTO_GET_DRIVERID();

	if (icp_ocfDrvDriverId < 0) {
		EPRINTK("%s : ICP driver failed to register with OCF!\n",
			__FUNCTION__);
		return icp_module_return_code(ENODEV);
	}

	/*Create all the slab caches used by the OCF EP80579 Driver */
	drvSessionData_zone =
	    ICP_CACHE_CREATE(ICP_SESSION_DATA_NAME, struct icp_drvSessionData);

	/* 
	 * Allocation of the OpData includes the allocation space for meta data.
	 * The memory after the opData structure is reserved for this meta data.
	 */
	drvOpData_zone =
	    icp_kmem_cache_create(ICP_OP_DATA_NAME,
				  sizeof(struct icp_drvOpData) +
				  defBuffListInfo.metaSize,
				  ICP_KERNEL_CACHE_ALIGN,
				  ICP_KERNEL_CACHE_NOINIT);

	drvDH_zone = ICP_CACHE_CREATE(ICP_DH_NAME, CpaCyDhPhase1KeyGenOpData);

	drvLnModExp_zone =
	    ICP_CACHE_CREATE(ICP_MODEXP_NAME, CpaCyLnModExpOpData);

	drvRSADecrypt_zone =
	    ICP_CACHE_CREATE(ICP_RSA_DECRYPT_NAME, CpaCyRsaDecryptOpData);

	drvRSAPrivateKey_zone =
	    ICP_CACHE_CREATE(ICP_RSA_PKEY_NAME, CpaCyRsaPrivateKey);

	drvDSARSSign_zone =
	    ICP_CACHE_CREATE(ICP_DSA_SIGN_NAME, CpaCyDsaRSSignOpData);

	/*too awkward to use a macro here */
	drvDSARSSignKValue_zone =
	    ICP_CACHE_CREATE(ICP_RAND_VAL_NAME,
			     DSA_RS_SIGN_PRIMEQ_SIZE_IN_BYTES);

	drvDSAVerify_zone =
	    ICP_CACHE_CREATE(ICP_DSA_VER_NAME, CpaCyDsaVerifyOpData);

	drvFlatBuffer_zone =
	    ICP_CACHE_CREATE(ICP_FLAT_BUFF_NAME, CpaFlatBuffer);

	if (0 == icp_cache_null_check()) {
		icp_ocfDrvFreeCaches();
		EPRINTK("%s() line %d: Not enough memory!\n",
			__FUNCTION__, __LINE__);
		return ENOMEM;
	}

	/* Register the ICP symmetric crypto support. */
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_NULL_CBC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_DES_CBC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_3DES_CBC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_AES_CBC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_ARC4, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_MD5, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_MD5_HMAC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA1, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA1_HMAC, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_256, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_256_HMAC,
			     ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_384, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_384_HMAC,
			     ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_512, ocfStatus);
	ICP_REG_SYM_WITH_OCF(icp_ocfDrvDriverId, CRYPTO_SHA2_512_HMAC,
			     ocfStatus);

	/* Register the ICP asymmetric algorithm support */
	ICP_REG_ASYM_WITH_OCF(icp_ocfDrvDriverId, CRK_DH_COMPUTE_KEY,
			      ocfStatus);
	ICP_REG_ASYM_WITH_OCF(icp_ocfDrvDriverId, CRK_MOD_EXP, ocfStatus);
	ICP_REG_ASYM_WITH_OCF(icp_ocfDrvDriverId, CRK_MOD_EXP_CRT, ocfStatus);
	ICP_REG_ASYM_WITH_OCF(icp_ocfDrvDriverId, CRK_DSA_SIGN, ocfStatus);
	ICP_REG_ASYM_WITH_OCF(icp_ocfDrvDriverId, CRK_DSA_VERIFY, ocfStatus);

	/* Register the ICP random number generator support */
	ICP_REG_RAND_WITH_OCF(icp_ocfDrvDriverId,
			      icp_ocfDrvReadRandom, NULL, ocfStatus);

	if (OCF_ZERO_FUNCTIONALITY_REGISTERED == ocfStatus) {
		DPRINTK("%s: Failed to register any device capabilities\n",
			__FUNCTION__);
		icp_ocfDrvFreeCaches();
		icp_ocfDrvDriverId = INVALID_DRIVER_ID;
		return icp_module_return_code(ECANCELED);
	}

	DPRINTK("%s: Registered %d of %d device capabilities\n",
		__FUNCTION__, ocfStatus, NUM_SUPPORTED_CAPABILITIES);

	/*Session data linked list used during module exit */
	ICP_INIT_LIST_HEAD(&icp_ocfDrvGlobalSymListHead);
	ICP_INIT_LIST_HEAD(&icp_ocfDrvGlobalSymListHead_FreeMemList);

	ICP_WORKQUEUE_CREATE(icp_ocfDrvFreeLacSessionWorkQ, "icpwq");
	if (ICP_WORKQUEUE_NULL_CHECK(icp_ocfDrvFreeLacSessionWorkQ)) {
		EPRINTK("%s: Failed to create single "
			"thread workqueue\n", __FUNCTION__);
		icp_ocfDrvFreeCaches();
		icp_ocfDrvDriverId = INVALID_DRIVER_ID;
		return icp_module_return_code(ENOMEM);
	}

	return icp_module_return_code(0);
}

/* Name        : icp_ocfDrvExit
 *
 * Description : This function will deregister all the symmetric sessions
 * registered with the LAC component. It will also deregister all symmetric
 * and asymmetric functionality that can be accelerated by the hardware via OCF
 * and random number generation if it is enabled.
 */
ICP_MODULE_EXIT_FUNC(icp_ocfDrvExit)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	struct icp_drvSessionData *sessionData = NULL;
	struct icp_drvSessionData *tempSessionData = NULL;
	int i, remaining_delay_time_in_jiffies = 0;

	/* For FreeBSD the invariant macro below makes function to return     */
	/* with EBUSY value in the case of any session which has been regi-   */
	/* stered with LAC not being deregistered.                            */
	/* The Linux implementation is empty since it is purely to compensate */
	/* for a limitation of the FreeBSD 7.1 Opencrypto framework.          */

    ICP_MODULE_EXIT_INV();

	/* There is a possibility of a process or new session command being   */
	/* sent before this variable is incremented. The aim of this variable */
	/* is to stop a loop of calls creating a deadlock situation which     */
	/* would prevent the driver from exiting.                             */
	icp_atomic_set(&icp_ocfDrvIsExiting, 1);

	/*Existing sessions will be routed to another driver after these calls */
	crypto_unregister_all(icp_ocfDrvDriverId);
	crypto_runregister_all(icp_ocfDrvDriverId);

	if (ICP_WORKQUEUE_NULL_CHECK(icp_ocfDrvFreeLacSessionWorkQ)) {
		DPRINTK("%s: workqueue already "
			"destroyed, therefore module exit "
			" function already called. Exiting.\n", __FUNCTION__);
		return ICP_MODULE_EXIT_FUNC_RETURN_VAL;
	}
	/*If any sessions are waiting to be deregistered, do that. This also 
	   flushes the work queue */
	ICP_WORKQUEUE_DESTROY(icp_ocfDrvFreeLacSessionWorkQ);

	/*ENTER CRITICAL SECTION */
	icp_spin_lockbh_lock(&icp_ocfDrvSymSessInfoListSpinlock);

	ICP_LIST_FOR_EACH_ENTRY_SAFE(tempSessionData, sessionData,
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
					icp_atomic_inc
					    (&lac_session_failed_dereg_count);
					break;
				}

				/*schedule_timout returns the time left for completion if 
				 * this task is set to TASK_INTERRUPTIBLE */
				remaining_delay_time_in_jiffies =
				    dereg_retry_delay_in_jiffies;
				while (0 > remaining_delay_time_in_jiffies) {
					remaining_delay_time_in_jiffies =
					    icp_schedule_timeout
					    (&icp_ocfDrvSymSessInfoListSpinlock,
					     remaining_delay_time_in_jiffies);
				}

				DPRINTK
				    ("%s(): Retry %d to deregistrate the session\n",
				     __FUNCTION__, i);
			}
		}

		/*remove from current list */
		ICP_LIST_DEL(tempSessionData, listNode);
		/*add to free mem linked list */
		ICP_LIST_ADD(tempSessionData,
			     &icp_ocfDrvGlobalSymListHead_FreeMemList,
			     listNode);

	}

	/*EXIT CRITICAL SECTION */
	icp_spin_lockbh_unlock(&icp_ocfDrvSymSessInfoListSpinlock);

	/*set back to initial values */
	sessionData = NULL;
	/*still have a reference in our list! */
	tempSessionData = NULL;
	/*free memory */

	ICP_LIST_FOR_EACH_ENTRY_SAFE(tempSessionData, sessionData,
				     &icp_ocfDrvGlobalSymListHead_FreeMemList,
				     listNode) {

		ICP_LIST_DEL(tempSessionData, listNode);
		/* Free allocated CpaCySymSessionCtx */
		if (NULL != tempSessionData->sessHandle) {
			icp_kfree(tempSessionData->sessHandle);
		}
		memset(tempSessionData, 0, sizeof(struct icp_drvSessionData));
		ICP_CACHE_FREE(drvSessionData_zone, tempSessionData);
	}

	if (0 != icp_atomic_read(&lac_session_failed_dereg_count)) {
		DPRINTK("%s(): %d LAC sessions were not deregistered "
			"correctly. This is not a clean exit! \n",
			__FUNCTION__,
			icp_atomic_read(&lac_session_failed_dereg_count));
	}

	icp_ocfDrvFreeCaches();
	icp_ocfDrvDriverId = INVALID_DRIVER_ID;

	icp_spin_lock_destroy(&icp_ocfDrvSymSessInfoListSpinlock);

	/* Shutdown the Cryptographic component */
	lacStatus = cpaCyStopInstance(CPA_INSTANCE_HANDLE_SINGLE);
	if (CPA_STATUS_SUCCESS != lacStatus) {
		DPRINTK("%s(): Failed to stop instance of the "
			"Cryptographic component.(status == %d)\n",
			__FUNCTION__, lacStatus);
	}

	return ICP_MODULE_EXIT_FUNC_RETURN_VAL;
}

/* Name        : icp_ocfDrvFreeCaches
 *
 * Description : This function deregisters all slab caches
 */
static void icp_ocfDrvFreeCaches(void)
{
	icp_atomic_set(&icp_ocfDrvIsExiting, 1);

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
	icp_atomic_inc(&lac_session_failed_dereg_count);

	/*Farm off to work queue */
	workstore =
	    icp_kmalloc(sizeof(struct icp_ocfDrvFreeLacSession), ICP_M_NOWAIT);
	if (NULL == workstore) {
		DPRINTK("%s(): unable to free session - no memory available "
			"for work queue\n", __FUNCTION__);
		return ENOMEM;
	}

	workstore->sessionToDeregister = sessionToDeregister;

	icp_init_work(&(workstore->work),
		      icp_ocfDrvDeferedFreeLacSessionTaskFn, workstore);

	ICP_WORKQUEUE_ENQUEUE(icp_ocfDrvFreeLacSessionWorkQ,
			      &(workstore->work));

	return ICP_OCF_DRV_STATUS_SUCCESS;

}

/* Name        : icp_ocfDrvDeferedFreeLacSessionProcess
 *
 * Description : This function will retry (module input parameter)
 * 'num_dereg_retries' times to deregister any symmetric session that recieves a
 * CPA_STATUS_RETRY message from the LAC component. This function is run in
 * Thread context because it is called from a worker thread
 */
void icp_ocfDrvDeferedFreeLacSessionProcess(void *arg)
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
	icp_kfree(workstore);

	/*if exiting, give deregistration one more blast only */
	if (icp_atomic_read(&icp_ocfDrvIsExiting) == CPA_TRUE) {
		lacStatus = cpaCySymRemoveSession(CPA_INSTANCE_HANDLE_SINGLE,
						  sessionToDeregister);

		if (lacStatus != CPA_STATUS_SUCCESS) {
			DPRINTK("%s() Failed to Dereg LAC session %p "
				"during module exit\n", __FUNCTION__,
				sessionToDeregister);
			return;
		}

		icp_atomic_dec(&lac_session_failed_dereg_count);
		return;
	}

	for (i = 0; i <= num_dereg_retries; i++) {
		lacStatus = cpaCySymRemoveSession(CPA_INSTANCE_HANDLE_SINGLE,
						  sessionToDeregister);

		if (lacStatus == CPA_STATUS_SUCCESS) {
			icp_atomic_dec(&lac_session_failed_dereg_count);
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
		while (0 < remaining_delay_time_in_jiffies) {
			remaining_delay_time_in_jiffies =
			    icp_schedule_timeout(NULL,
						 remaining_delay_time_in_jiffies);
		}

	}

	DPRINTK("%s(): Unable to deregister session\n", __FUNCTION__);
	DPRINTK("%s(): Number of unavailable LAC sessions = %d\n", __FUNCTION__,
		icp_atomic_read(&lac_session_failed_dereg_count));
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

/* Name        : icp_ocfDrvFreeFlatBuffer
 *
 * Description : This function will deallocate flat buffer.
 */
inline void icp_ocfDrvFreeFlatBuffer(CpaFlatBuffer * pFlatBuffer)
{
	if (pFlatBuffer != NULL) {
		memset(pFlatBuffer, 0, sizeof(CpaFlatBuffer));
		ICP_CACHE_FREE(drvFlatBuffer_zone, pFlatBuffer);
	}
}

/* Name        : icp_ocfDrvAllocMetaData
 *
 * Description : This function will allocate memory for the
 * pPrivateMetaData member of CpaBufferList.
 */
inline int
icp_ocfDrvAllocMetaData(CpaBufferList * pBufferList,
			struct icp_drvOpData *pOpData)
{
	Cpa32U metaSize = 0;

	if (pBufferList->numBuffers <= ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS) {
		uint8_t *pOpDataStartAddr = (uint8_t *) pOpData;

		if (0 == defBuffListInfo.metaSize) {
			pBufferList->pPrivateMetaData = NULL;
			return ICP_OCF_DRV_STATUS_SUCCESS;
		}
		/*
		 * The meta data allocation has been included as part of the 
		 * op data.  It has been pre-allocated in memory just after the
		 * icp_drvOpData structure.
		 */
		pBufferList->pPrivateMetaData = (void *)(pOpDataStartAddr +
							 sizeof(struct
								icp_drvOpData));
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

		pBufferList->pPrivateMetaData =
		    icp_kmalloc(metaSize, ICP_M_NOWAIT);
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
	if (ICP_OCF_DRV_DEFAULT_BUFFLIST_ARRAYS < pBufferList->numBuffers) {
		icp_kfree(pBufferList->pPrivateMetaData);
	}
}

/* Module declaration, init and exit functions */
ICP_DECLARE_MODULE(icp_ocf, icp_ocfDrvInit, icp_ocfDrvExit);
ICP_MODULE_DESCRIPTION("OCF Driver for Intel Quick Assist crypto acceleration");
ICP_MODULE_VERSION(icp_ocf, ICP_OCF_VER_MJR);
ICP_MODULE_LICENSE("Dual BSD/GPL");
ICP_MODULE_AUTHOR("Intel");

/* Module parameters */
ICP_MODULE_PARAM_INT(icp_ocf, num_dereg_retries,
		     "Number of times to retry LAC Sym Session Deregistration. "
		     "Default 10, Max 100");
ICP_MODULE_PARAM_INT(icp_ocf, dereg_retry_delay_in_jiffies, "Delay in jiffies "
		     "(added to a schedule() function call) before a LAC Sym "
		     "Session Dereg is retried. Default 10");
ICP_MODULE_PARAM_INT(icp_ocf, max_sessions,
		     "This sets the maximum number of sessions "
		     "between OCF and this driver. If this value is set to zero,"
		     "max session count checking is disabled. Default is zero(0)");

/* Module dependencies */
#define MODULE_MIN_VER	1
#define CRYPTO_MAX_VER	3
#define LAC_MAX_VER	2

ICP_MODULE_DEPEND(icp_ocf, crypto, MODULE_MIN_VER, MODULE_MIN_VER,
		  CRYPTO_MAX_VER);
ICP_MODULE_DEPEND(icp_ocf, cryptodev, MODULE_MIN_VER, MODULE_MIN_VER,
		  CRYPTO_MAX_VER);
ICP_MODULE_DEPEND(icp_ocf, icp_crypto, MODULE_MIN_VER, MODULE_MIN_VER,
		  LAC_MAX_VER);
