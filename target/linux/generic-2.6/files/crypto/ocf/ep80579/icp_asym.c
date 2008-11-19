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

#include "icp_ocf.h"

/*The following define values (containing the word 'INDEX') are used to find
the index of each input buffer of the crypto_kop struct (see OCF cryptodev.h).
These values were found through analysis of the OCF OpenSSL patch. If the
calling program uses different input buffer positions, these defines will have
to be changed.*/

/*DIFFIE HELLMAN buffer index values*/
#define ICP_DH_KRP_PARAM_PRIME_INDEX				(0)
#define ICP_DH_KRP_PARAM_BASE_INDEX				(1)
#define ICP_DH_KRP_PARAM_PRIVATE_VALUE_INDEX			(2)
#define ICP_DH_KRP_PARAM_RESULT_INDEX				(3)

/*MOD EXP buffer index values*/
#define ICP_MOD_EXP_KRP_PARAM_BASE_INDEX			(0)
#define ICP_MOD_EXP_KRP_PARAM_EXPONENT_INDEX			(1)
#define ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX			(2)
#define ICP_MOD_EXP_KRP_PARAM_RESULT_INDEX			(3)

#define SINGLE_BYTE_VALUE					(4)

/*MOD EXP CRT buffer index values*/
#define ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_P_INDEX			(0)
#define ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_Q_INDEX			(1)
#define ICP_MOD_EXP_CRT_KRP_PARAM_I_INDEX			(2)
#define ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DP_INDEX		(3)
#define ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DQ_INDEX		(4)
#define ICP_MOD_EXP_CRT_KRP_PARAM_COEFF_QINV_INDEX		(5)
#define ICP_MOD_EXP_CRT_KRP_PARAM_RESULT_INDEX			(6)

/*DSA sign buffer index values*/
#define ICP_DSA_SIGN_KRP_PARAM_DGST_INDEX			(0)
#define ICP_DSA_SIGN_KRP_PARAM_PRIME_P_INDEX			(1)
#define ICP_DSA_SIGN_KRP_PARAM_PRIME_Q_INDEX			(2)
#define ICP_DSA_SIGN_KRP_PARAM_G_INDEX				(3)
#define ICP_DSA_SIGN_KRP_PARAM_X_INDEX				(4)
#define ICP_DSA_SIGN_KRP_PARAM_R_RESULT_INDEX			(5)
#define ICP_DSA_SIGN_KRP_PARAM_S_RESULT_INDEX			(6)

/*DSA verify buffer index values*/
#define ICP_DSA_VERIFY_KRP_PARAM_DGST_INDEX			(0)
#define ICP_DSA_VERIFY_KRP_PARAM_PRIME_P_INDEX			(1)
#define ICP_DSA_VERIFY_KRP_PARAM_PRIME_Q_INDEX			(2)
#define ICP_DSA_VERIFY_KRP_PARAM_G_INDEX			(3)
#define ICP_DSA_VERIFY_KRP_PARAM_PUBKEY_INDEX			(4)
#define ICP_DSA_VERIFY_KRP_PARAM_SIG_R_INDEX			(5)
#define ICP_DSA_VERIFY_KRP_PARAM_SIG_S_INDEX			(6)

/*DSA sign prime Q vs random number K size check values*/
#define DONT_RUN_LESS_THAN_CHECK				(0)
#define FAIL_A_IS_GREATER_THAN_B				(1)
#define FAIL_A_IS_EQUAL_TO_B					(1)
#define SUCCESS_A_IS_LESS_THAN_B				(0)
#define DSA_SIGN_RAND_GEN_VAL_CHECK_MAX_ITERATIONS		(500)

/* We need to set a cryptokp success value just in case it is set or allocated
   and not set to zero outside of this module */
#define CRYPTO_OP_SUCCESS					(0)

static int icp_ocfDrvDHComputeKey(struct cryptkop *krp);

static int icp_ocfDrvModExp(struct cryptkop *krp);

static int icp_ocfDrvModExpCRT(struct cryptkop *krp);

static int
icp_ocfDrvCheckALessThanB(CpaFlatBuffer * pK, CpaFlatBuffer * pQ, int *doCheck);

static int icp_ocfDrvDsaSign(struct cryptkop *krp);

static int icp_ocfDrvDsaVerify(struct cryptkop *krp);

static void
icp_ocfDrvDhP1CallBack(void *callbackTag,
		       CpaStatus status,
		       void *pOpData, CpaFlatBuffer * pLocalOctetStringPV);

static void
icp_ocfDrvModExpCallBack(void *callbackTag,
			 CpaStatus status,
			 void *pOpData, CpaFlatBuffer * pResult);

static void
icp_ocfDrvModExpCRTCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData, CpaFlatBuffer * pOutputData);

static void
icp_ocfDrvDsaVerifyCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData, CpaBoolean verifyStatus);

static void
icp_ocfDrvDsaRSSignCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData,
			    CpaBoolean protocolStatus,
			    CpaFlatBuffer * pR, CpaFlatBuffer * pS);

/* Name        : icp_ocfDrvPkeProcess
 *
 * Description : This function will choose which PKE process to follow
 * based on the input arguments
 */
int icp_ocfDrvPkeProcess(device_t dev, struct cryptkop *krp, int hint)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;

	if (NULL == krp) {
		DPRINTK("%s(): Invalid input parameters, cryptkop = %p\n",
			__FUNCTION__, krp);
		return EINVAL;
	}

	if (CPA_TRUE == atomic_read(&icp_ocfDrvIsExiting)) {
		krp->krp_status = ECANCELED;
		return ECANCELED;
	}

	switch (krp->krp_op) {
	case CRK_DH_COMPUTE_KEY:
		DPRINTK("%s() doing DH_COMPUTE_KEY\n", __FUNCTION__);
		lacStatus = icp_ocfDrvDHComputeKey(krp);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): icp_ocfDrvDHComputeKey failed "
				"(%d).\n", __FUNCTION__, lacStatus);
			krp->krp_status = ECANCELED;
			return ECANCELED;
		}

		break;

	case CRK_MOD_EXP:
		DPRINTK("%s() doing MOD_EXP \n", __FUNCTION__);
		lacStatus = icp_ocfDrvModExp(krp);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): icp_ocfDrvModExp failed (%d).\n",
				__FUNCTION__, lacStatus);
			krp->krp_status = ECANCELED;
			return ECANCELED;
		}

		break;

	case CRK_MOD_EXP_CRT:
		DPRINTK("%s() doing MOD_EXP_CRT \n", __FUNCTION__);
		lacStatus = icp_ocfDrvModExpCRT(krp);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): icp_ocfDrvModExpCRT "
				"failed (%d).\n", __FUNCTION__, lacStatus);
			krp->krp_status = ECANCELED;
			return ECANCELED;
		}

		break;

	case CRK_DSA_SIGN:
		DPRINTK("%s() doing DSA_SIGN \n", __FUNCTION__);
		lacStatus = icp_ocfDrvDsaSign(krp);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): icp_ocfDrvDsaSign "
				"failed (%d).\n", __FUNCTION__, lacStatus);
			krp->krp_status = ECANCELED;
			return ECANCELED;
		}

		break;

	case CRK_DSA_VERIFY:
		DPRINTK("%s() doing DSA_VERIFY \n", __FUNCTION__);
		lacStatus = icp_ocfDrvDsaVerify(krp);
		if (CPA_STATUS_SUCCESS != lacStatus) {
			EPRINTK("%s(): icp_ocfDrvDsaVerify "
				"failed (%d).\n", __FUNCTION__, lacStatus);
			krp->krp_status = ECANCELED;
			return ECANCELED;
		}

		break;

	default:
		EPRINTK("%s(): Asymettric function not "
			"supported (%d).\n", __FUNCTION__, krp->krp_op);
		krp->krp_status = EOPNOTSUPP;
		return EOPNOTSUPP;
	}

	return ICP_OCF_DRV_STATUS_SUCCESS;
}

/* Name        : icp_ocfDrvSwapBytes
 *
 * Description : This function is used to swap the byte order of a buffer.
 * It has been seen that in general we are passed little endian byte order
 * buffers, but LAC only accepts big endian byte order buffers.
 */
static void inline
icp_ocfDrvSwapBytes(u_int8_t * num, u_int32_t buff_len_bytes)
{

	int i;
	u_int8_t *end_ptr;
	u_int8_t hold_val;

	end_ptr = num + (buff_len_bytes - 1);
	buff_len_bytes = buff_len_bytes >> 1;
	for (i = 0; i < buff_len_bytes; i++) {
		hold_val = *num;
		*num = *end_ptr;
		num++;
		*end_ptr = hold_val;
		end_ptr--;
	}
}

/* Name        : icp_ocfDrvDHComputeKey
 *
 * Description : This function will map Diffie Hellman calls from OCF
 * to the LAC API. OCF uses this function for Diffie Hellman Phase1 and
 * Phase2. LAC has a separate Diffie Hellman Phase2 call, however both phases
 * break down to a modular exponentiation.
 */
static int icp_ocfDrvDHComputeKey(struct cryptkop *krp)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	void *callbackTag = NULL;
	CpaCyDhPhase1KeyGenOpData *pPhase1OpData = NULL;
	CpaFlatBuffer *pLocalOctetStringPV = NULL;
	uint32_t dh_prime_len_bytes = 0, dh_prime_len_bits = 0;

	/* Input checks - check prime is a multiple of 8 bits to allow for
	   allocation later */
	dh_prime_len_bits =
	    (krp->krp_param[ICP_DH_KRP_PARAM_PRIME_INDEX].crp_nbits);

	/* LAC can reject prime lengths based on prime key sizes, we just
	   need to make sure we can allocate space for the base and
	   exponent buffers correctly */
	if ((dh_prime_len_bits % NUM_BITS_IN_BYTE) != 0) {
		APRINTK("%s(): Warning Prime number buffer size is not a "
			"multiple of 8 bits\n", __FUNCTION__);
	}

	/* Result storage space should be the same size as the prime as this
	   value can take up the same amount of storage space */
	if (dh_prime_len_bits !=
	    krp->krp_param[ICP_DH_KRP_PARAM_RESULT_INDEX].crp_nbits) {
		DPRINTK("%s(): Return Buffer must be the same size "
			"as the Prime buffer\n", __FUNCTION__);
		krp->krp_status = EINVAL;
		return EINVAL;
	}
	/* Switch to size in bytes */
	BITS_TO_BYTES(dh_prime_len_bytes, dh_prime_len_bits);

	callbackTag = krp;

	pPhase1OpData = kmem_cache_zalloc(drvDH_zone, GFP_KERNEL);
	if (NULL == pPhase1OpData) {
		APRINTK("%s():Failed to get memory for key gen data\n",
			__FUNCTION__);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	pLocalOctetStringPV = kmem_cache_zalloc(drvFlatBuffer_zone, GFP_KERNEL);
	if (NULL == pLocalOctetStringPV) {
		APRINTK("%s():Failed to get memory for pLocalOctetStringPV\n",
			__FUNCTION__);
		kmem_cache_free(drvDH_zone, pPhase1OpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	/* Link parameters */
	pPhase1OpData->primeP.pData =
	    krp->krp_param[ICP_DH_KRP_PARAM_PRIME_INDEX].crp_p;

	pPhase1OpData->primeP.dataLenInBytes = dh_prime_len_bytes;

	icp_ocfDrvSwapBytes(pPhase1OpData->primeP.pData, dh_prime_len_bytes);

	pPhase1OpData->baseG.pData =
	    krp->krp_param[ICP_DH_KRP_PARAM_BASE_INDEX].crp_p;

	BITS_TO_BYTES(pPhase1OpData->baseG.dataLenInBytes,
		      krp->krp_param[ICP_DH_KRP_PARAM_BASE_INDEX].crp_nbits);

	icp_ocfDrvSwapBytes(pPhase1OpData->baseG.pData,
			    pPhase1OpData->baseG.dataLenInBytes);

	pPhase1OpData->privateValueX.pData =
	    krp->krp_param[ICP_DH_KRP_PARAM_PRIVATE_VALUE_INDEX].crp_p;

	BITS_TO_BYTES(pPhase1OpData->privateValueX.dataLenInBytes,
		      krp->krp_param[ICP_DH_KRP_PARAM_PRIVATE_VALUE_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(pPhase1OpData->privateValueX.pData,
			    pPhase1OpData->privateValueX.dataLenInBytes);

	/* Output parameters */
	pLocalOctetStringPV->pData =
	    krp->krp_param[ICP_DH_KRP_PARAM_RESULT_INDEX].crp_p;

	BITS_TO_BYTES(pLocalOctetStringPV->dataLenInBytes,
		      krp->krp_param[ICP_DH_KRP_PARAM_RESULT_INDEX].crp_nbits);

	lacStatus = cpaCyDhKeyGenPhase1(CPA_INSTANCE_HANDLE_SINGLE,
					icp_ocfDrvDhP1CallBack,
					callbackTag, pPhase1OpData,
					pLocalOctetStringPV);

	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): DH Phase 1 Key Gen failed (%d).\n",
			__FUNCTION__, lacStatus);
		icp_ocfDrvFreeFlatBuffer(pLocalOctetStringPV);
		kmem_cache_free(drvDH_zone, pPhase1OpData);
	}

	return lacStatus;
}

/* Name        : icp_ocfDrvModExp
 *
 * Description : This function will map ordinary Modular Exponentiation calls
 * from OCF to the LAC API.
 *
 */
static int icp_ocfDrvModExp(struct cryptkop *krp)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	void *callbackTag = NULL;
	CpaCyLnModExpOpData *pModExpOpData = NULL;
	CpaFlatBuffer *pResult = NULL;

	if ((krp->krp_param[ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX].crp_nbits %
	     NUM_BITS_IN_BYTE) != 0) {
		DPRINTK("%s(): Warning - modulus buffer size (%d) is not a "
			"multiple of 8 bits\n", __FUNCTION__,
			krp->krp_param[ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX].
			crp_nbits);
	}

	/* Result storage space should be the same size as the prime as this
	   value can take up the same amount of storage space */
	if (krp->krp_param[ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX].crp_nbits >
	    krp->krp_param[ICP_MOD_EXP_KRP_PARAM_RESULT_INDEX].crp_nbits) {
		APRINTK("%s(): Return Buffer size must be the same or"
			" greater than the Modulus buffer\n", __FUNCTION__);
		krp->krp_status = EINVAL;
		return EINVAL;
	}

	callbackTag = krp;

	pModExpOpData = kmem_cache_zalloc(drvLnModExp_zone, GFP_KERNEL);
	if (NULL == pModExpOpData) {
		APRINTK("%s():Failed to get memory for key gen data\n",
			__FUNCTION__);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	pResult = kmem_cache_zalloc(drvFlatBuffer_zone, GFP_KERNEL);
	if (NULL == pResult) {
		APRINTK("%s():Failed to get memory for ModExp result\n",
			__FUNCTION__);
		kmem_cache_free(drvLnModExp_zone, pModExpOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	/* Link parameters */
	pModExpOpData->modulus.pData =
	    krp->krp_param[ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX].crp_p;
	BITS_TO_BYTES(pModExpOpData->modulus.dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_KRP_PARAM_MODULUS_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(pModExpOpData->modulus.pData,
			    pModExpOpData->modulus.dataLenInBytes);

	/*OCF patch to Openswan Pluto regularly sends the base value as 2
	   bits in size. In this case, it has been found it is better to
	   use the base size memory space as the input buffer (if the number
	   is in bits is less than a byte, the number of bits is the input
	   value) */
	if (krp->krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].crp_nbits <
	    NUM_BITS_IN_BYTE) {
		DPRINTK("%s : base is small (%d)\n", __FUNCTION__, krp->
			krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].crp_nbits);
		pModExpOpData->base.dataLenInBytes = SINGLE_BYTE_VALUE;
		pModExpOpData->base.pData =
		    (uint8_t *) & (krp->
				   krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].
				   crp_nbits);
		*((uint32_t *) pModExpOpData->base.pData) =
		    htonl(*((uint32_t *) pModExpOpData->base.pData));

	} else {

		DPRINTK("%s : base is big (%d)\n", __FUNCTION__, krp->
			krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].crp_nbits);
		pModExpOpData->base.pData =
		    krp->krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].crp_p;
		BITS_TO_BYTES(pModExpOpData->base.dataLenInBytes,
			      krp->krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].
			      crp_nbits);
		icp_ocfDrvSwapBytes(pModExpOpData->base.pData,
				    pModExpOpData->base.dataLenInBytes);
	}

	pModExpOpData->exponent.pData =
	    krp->krp_param[ICP_MOD_EXP_KRP_PARAM_EXPONENT_INDEX].crp_p;
	BITS_TO_BYTES(pModExpOpData->exponent.dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_KRP_PARAM_EXPONENT_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(pModExpOpData->exponent.pData,
			    pModExpOpData->exponent.dataLenInBytes);
	/* Output parameters */
	pResult->pData =
	    krp->krp_param[ICP_MOD_EXP_KRP_PARAM_RESULT_INDEX].crp_p,
	    BITS_TO_BYTES(pResult->dataLenInBytes,
			  krp->krp_param[ICP_MOD_EXP_KRP_PARAM_RESULT_INDEX].
			  crp_nbits);

	lacStatus = cpaCyLnModExp(CPA_INSTANCE_HANDLE_SINGLE,
				  icp_ocfDrvModExpCallBack,
				  callbackTag, pModExpOpData, pResult);

	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): Mod Exp Operation failed (%d).\n",
			__FUNCTION__, lacStatus);
		krp->krp_status = ECANCELED;
		icp_ocfDrvFreeFlatBuffer(pResult);
		kmem_cache_free(drvLnModExp_zone, pModExpOpData);
	}

	return lacStatus;
}

/* Name        : icp_ocfDrvModExpCRT
 *
 * Description : This function will map ordinary Modular Exponentiation Chinese
 * Remainder Theorem implementaion calls from OCF to the LAC API.
 *
 * Note : Mod Exp CRT for this driver is accelerated through LAC RSA type 2
 * decrypt operation. Therefore P and Q input values must always be prime
 * numbers. Although basic primality checks are done in LAC, it is up to the
 * user to do any correct prime number checking before passing the inputs.
 */

static int icp_ocfDrvModExpCRT(struct cryptkop *krp)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	CpaCyRsaDecryptOpData *rsaDecryptOpData = NULL;
	void *callbackTag = NULL;
	CpaFlatBuffer *pOutputData = NULL;

	/*Parameter input checks are all done by LAC, no need to repeat
	   them here. */
	callbackTag = krp;

	rsaDecryptOpData = kmem_cache_zalloc(drvRSADecrypt_zone, GFP_KERNEL);
	if (NULL == rsaDecryptOpData) {
		APRINTK("%s():Failed to get memory"
			" for MOD EXP CRT Op data struct\n", __FUNCTION__);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	rsaDecryptOpData->pRecipientPrivateKey
	    = kmem_cache_zalloc(drvRSAPrivateKey_zone, GFP_KERNEL);
	if (NULL == rsaDecryptOpData->pRecipientPrivateKey) {
		APRINTK("%s():Failed to get memory for MOD EXP CRT"
			" private key values struct\n", __FUNCTION__);
		kmem_cache_free(drvRSADecrypt_zone, rsaDecryptOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	rsaDecryptOpData->pRecipientPrivateKey->
	    version = CPA_CY_RSA_VERSION_TWO_PRIME;
	rsaDecryptOpData->pRecipientPrivateKey->
	    privateKeyRepType = CPA_CY_RSA_PRIVATE_KEY_REP_TYPE_2;

	pOutputData = kmem_cache_zalloc(drvFlatBuffer_zone, GFP_KERNEL);
	if (NULL == pOutputData) {
		APRINTK("%s():Failed to get memory"
			" for MOD EXP CRT output data\n", __FUNCTION__);
		kmem_cache_free(drvRSAPrivateKey_zone,
				rsaDecryptOpData->pRecipientPrivateKey);
		kmem_cache_free(drvRSADecrypt_zone, rsaDecryptOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	rsaDecryptOpData->pRecipientPrivateKey->
	    version = CPA_CY_RSA_VERSION_TWO_PRIME;
	rsaDecryptOpData->pRecipientPrivateKey->
	    privateKeyRepType = CPA_CY_RSA_PRIVATE_KEY_REP_TYPE_2;

	/* Link parameters */
	rsaDecryptOpData->inputData.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_I_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->inputData.dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_I_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->inputData.pData,
			    rsaDecryptOpData->inputData.dataLenInBytes);

	rsaDecryptOpData->pRecipientPrivateKey->privateKeyRep2.prime1P.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_P_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->pRecipientPrivateKey->privateKeyRep2.
		      prime1P.dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_P_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.prime1P.pData,
			    rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.prime1P.dataLenInBytes);

	rsaDecryptOpData->pRecipientPrivateKey->privateKeyRep2.prime2Q.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_Q_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->pRecipientPrivateKey->privateKeyRep2.
		      prime2Q.dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_PRIME_Q_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.prime2Q.pData,
			    rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.prime2Q.dataLenInBytes);

	rsaDecryptOpData->pRecipientPrivateKey->
	    privateKeyRep2.exponent1Dp.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DP_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->pRecipientPrivateKey->privateKeyRep2.
		      exponent1Dp.dataLenInBytes,
		      krp->
		      krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DP_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.exponent1Dp.pData,
			    rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.exponent1Dp.dataLenInBytes);

	rsaDecryptOpData->pRecipientPrivateKey->
	    privateKeyRep2.exponent2Dq.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DQ_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->pRecipientPrivateKey->
		      privateKeyRep2.exponent2Dq.dataLenInBytes,
		      krp->
		      krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_EXPONENT_DQ_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.exponent2Dq.pData,
			    rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.exponent2Dq.dataLenInBytes);

	rsaDecryptOpData->pRecipientPrivateKey->
	    privateKeyRep2.coefficientQInv.pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_COEFF_QINV_INDEX].crp_p;
	BITS_TO_BYTES(rsaDecryptOpData->pRecipientPrivateKey->
		      privateKeyRep2.coefficientQInv.dataLenInBytes,
		      krp->
		      krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_COEFF_QINV_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.coefficientQInv.pData,
			    rsaDecryptOpData->pRecipientPrivateKey->
			    privateKeyRep2.coefficientQInv.dataLenInBytes);

	/* Output Parameter */
	pOutputData->pData =
	    krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_RESULT_INDEX].crp_p;
	BITS_TO_BYTES(pOutputData->dataLenInBytes,
		      krp->krp_param[ICP_MOD_EXP_CRT_KRP_PARAM_RESULT_INDEX].
		      crp_nbits);

	lacStatus = cpaCyRsaDecrypt(CPA_INSTANCE_HANDLE_SINGLE,
				    icp_ocfDrvModExpCRTCallBack,
				    callbackTag, rsaDecryptOpData, pOutputData);

	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): Mod Exp CRT Operation failed (%d).\n",
			__FUNCTION__, lacStatus);
		krp->krp_status = ECANCELED;
		icp_ocfDrvFreeFlatBuffer(pOutputData);
		kmem_cache_free(drvRSAPrivateKey_zone,
				rsaDecryptOpData->pRecipientPrivateKey);
		kmem_cache_free(drvRSADecrypt_zone, rsaDecryptOpData);
	}

	return lacStatus;
}

/* Name        : icp_ocfDrvCheckALessThanB
 *
 * Description : This function will check whether the first argument is less
 * than the second. It is used to check whether the DSA RS sign Random K
 * value is less than the Prime Q value (as defined in the specification)
 *
 */
static int
icp_ocfDrvCheckALessThanB(CpaFlatBuffer * pK, CpaFlatBuffer * pQ, int *doCheck)
{

	uint8_t *MSB_K = pK->pData;
	uint8_t *MSB_Q = pQ->pData;
	uint32_t buffer_lengths_in_bytes = pQ->dataLenInBytes;

	if (DONT_RUN_LESS_THAN_CHECK == *doCheck) {
		return FAIL_A_IS_GREATER_THAN_B;
	}

/*Check MSBs
if A == B, check next MSB
if A > B, return A_IS_GREATER_THAN_B
if A < B, return A_IS_LESS_THAN_B (success)
*/
	while (*MSB_K == *MSB_Q) {
		MSB_K++;
		MSB_Q++;

		buffer_lengths_in_bytes--;
		if (0 == buffer_lengths_in_bytes) {
			DPRINTK("%s() Buffers have equal value!!\n",
				__FUNCTION__);
			return FAIL_A_IS_EQUAL_TO_B;
		}

	}

	if (*MSB_K < *MSB_Q) {
		return SUCCESS_A_IS_LESS_THAN_B;
	} else {
		return FAIL_A_IS_GREATER_THAN_B;
	}

}

/* Name        : icp_ocfDrvDsaSign
 *
 * Description : This function will map DSA RS Sign from OCF to the LAC API.
 *
 * NOTE: From looking at OCF patch to OpenSSL and even the number of input
 * parameters, OCF expects us to generate the random seed value. This value
 * is generated and passed to LAC, however the number is discared in the
 * callback and not returned to the user.
 */
static int icp_ocfDrvDsaSign(struct cryptkop *krp)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	CpaCyDsaRSSignOpData *dsaRsSignOpData = NULL;
	void *callbackTag = NULL;
	CpaCyRandGenOpData randGenOpData;
	int primeQSizeInBytes = 0;
	int doCheck = 0;
	CpaFlatBuffer randData;
	CpaBoolean protocolStatus = CPA_FALSE;
	CpaFlatBuffer *pR = NULL;
	CpaFlatBuffer *pS = NULL;

	callbackTag = krp;

	BITS_TO_BYTES(primeQSizeInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_PRIME_Q_INDEX].
		      crp_nbits);

	if (DSA_RS_SIGN_PRIMEQ_SIZE_IN_BYTES != primeQSizeInBytes) {
		APRINTK("%s(): DSA PRIME Q size not equal to the "
			"FIPS defined 20bytes, = %d\n",
			__FUNCTION__, primeQSizeInBytes);
		krp->krp_status = EDOM;
		return EDOM;
	}

	dsaRsSignOpData = kmem_cache_zalloc(drvDSARSSign_zone, GFP_KERNEL);
	if (NULL == dsaRsSignOpData) {
		APRINTK("%s():Failed to get memory"
			" for DSA RS Sign Op data struct\n", __FUNCTION__);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	dsaRsSignOpData->K.pData =
	    kmem_cache_alloc(drvDSARSSignKValue_zone, GFP_ATOMIC);

	if (NULL == dsaRsSignOpData->K.pData) {
		APRINTK("%s():Failed to get memory"
			" for DSA RS Sign Op Random value\n", __FUNCTION__);
		kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	pR = kmem_cache_zalloc(drvFlatBuffer_zone, GFP_KERNEL);
	if (NULL == pR) {
		APRINTK("%s():Failed to get memory"
			" for DSA signature R\n", __FUNCTION__);
		kmem_cache_free(drvDSARSSignKValue_zone,
				dsaRsSignOpData->K.pData);
		kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	pS = kmem_cache_zalloc(drvFlatBuffer_zone, GFP_KERNEL);
	if (NULL == pS) {
		APRINTK("%s():Failed to get memory"
			" for DSA signature S\n", __FUNCTION__);
		icp_ocfDrvFreeFlatBuffer(pR);
		kmem_cache_free(drvDSARSSignKValue_zone,
				dsaRsSignOpData->K.pData);
		kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	/*link prime number parameter for ease of processing */
	dsaRsSignOpData->P.pData =
	    krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_PRIME_P_INDEX].crp_p;
	BITS_TO_BYTES(dsaRsSignOpData->P.dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_PRIME_P_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(dsaRsSignOpData->P.pData,
			    dsaRsSignOpData->P.dataLenInBytes);

	dsaRsSignOpData->Q.pData =
	    krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_PRIME_Q_INDEX].crp_p;
	BITS_TO_BYTES(dsaRsSignOpData->Q.dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_PRIME_Q_INDEX].
		      crp_nbits);

	icp_ocfDrvSwapBytes(dsaRsSignOpData->Q.pData,
			    dsaRsSignOpData->Q.dataLenInBytes);

	/*generate random number with equal buffer size to Prime value Q,
	   but value less than Q */
	dsaRsSignOpData->K.dataLenInBytes = dsaRsSignOpData->Q.dataLenInBytes;

	randGenOpData.generateBits = CPA_TRUE;
	randGenOpData.lenInBytes = dsaRsSignOpData->K.dataLenInBytes;

	icp_ocfDrvPtrAndLenToFlatBuffer(dsaRsSignOpData->K.pData,
					dsaRsSignOpData->K.dataLenInBytes,
					&randData);

	doCheck = 0;
	while (icp_ocfDrvCheckALessThanB(&(dsaRsSignOpData->K),
					 &(dsaRsSignOpData->Q), &doCheck)) {

		if (CPA_STATUS_SUCCESS
		    != cpaCyRandGen(CPA_INSTANCE_HANDLE_SINGLE,
				    NULL, NULL, &randGenOpData, &randData)) {
			APRINTK("%s(): ERROR - Failed to generate DSA RS Sign K"
				"value\n", __FUNCTION__);
			icp_ocfDrvFreeFlatBuffer(pS);
			icp_ocfDrvFreeFlatBuffer(pR);
			kmem_cache_free(drvDSARSSignKValue_zone,
					dsaRsSignOpData->K.pData);
			kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
			krp->krp_status = EAGAIN;
			return EAGAIN;
		}

		doCheck++;
		if (DSA_SIGN_RAND_GEN_VAL_CHECK_MAX_ITERATIONS == doCheck) {
			APRINTK("%s(): ERROR - Failed to find DSA RS Sign K "
				"value less than Q value\n", __FUNCTION__);
			icp_ocfDrvFreeFlatBuffer(pS);
			icp_ocfDrvFreeFlatBuffer(pR);
			kmem_cache_free(drvDSARSSignKValue_zone,
					dsaRsSignOpData->K.pData);
			kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
			krp->krp_status = EAGAIN;
			return EAGAIN;
		}

	}
	/*Rand Data - no need to swap bytes for pK */

	/* Link parameters */
	dsaRsSignOpData->G.pData =
	    krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_G_INDEX].crp_p;
	BITS_TO_BYTES(dsaRsSignOpData->G.dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_G_INDEX].crp_nbits);

	icp_ocfDrvSwapBytes(dsaRsSignOpData->G.pData,
			    dsaRsSignOpData->G.dataLenInBytes);

	dsaRsSignOpData->X.pData =
	    krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_X_INDEX].crp_p;
	BITS_TO_BYTES(dsaRsSignOpData->X.dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_X_INDEX].crp_nbits);
	icp_ocfDrvSwapBytes(dsaRsSignOpData->X.pData,
			    dsaRsSignOpData->X.dataLenInBytes);

	dsaRsSignOpData->M.pData =
	    krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_DGST_INDEX].crp_p;
	BITS_TO_BYTES(dsaRsSignOpData->M.dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_DGST_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaRsSignOpData->M.pData,
			    dsaRsSignOpData->M.dataLenInBytes);

	/* Output Parameters */
	pS->pData = krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_S_RESULT_INDEX].crp_p;
	BITS_TO_BYTES(pS->dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_S_RESULT_INDEX].
		      crp_nbits);

	pR->pData = krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_R_RESULT_INDEX].crp_p;
	BITS_TO_BYTES(pR->dataLenInBytes,
		      krp->krp_param[ICP_DSA_SIGN_KRP_PARAM_R_RESULT_INDEX].
		      crp_nbits);

	lacStatus = cpaCyDsaSignRS(CPA_INSTANCE_HANDLE_SINGLE,
				   icp_ocfDrvDsaRSSignCallBack,
				   callbackTag, dsaRsSignOpData,
				   &protocolStatus, pR, pS);

	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): DSA RS Sign Operation failed (%d).\n",
			__FUNCTION__, lacStatus);
		krp->krp_status = ECANCELED;
		icp_ocfDrvFreeFlatBuffer(pS);
		icp_ocfDrvFreeFlatBuffer(pR);
		kmem_cache_free(drvDSARSSignKValue_zone,
				dsaRsSignOpData->K.pData);
		kmem_cache_free(drvDSARSSign_zone, dsaRsSignOpData);
	}

	return lacStatus;
}

/* Name        : icp_ocfDrvDsaVerify
 *
 * Description : This function will map DSA RS Verify from OCF to the LAC API.
 *
 */
static int icp_ocfDrvDsaVerify(struct cryptkop *krp)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	CpaCyDsaVerifyOpData *dsaVerifyOpData = NULL;
	void *callbackTag = NULL;
	CpaBoolean verifyStatus = CPA_FALSE;

	callbackTag = krp;

	dsaVerifyOpData = kmem_cache_zalloc(drvDSAVerify_zone, GFP_KERNEL);
	if (NULL == dsaVerifyOpData) {
		APRINTK("%s():Failed to get memory"
			" for DSA Verify Op data struct\n", __FUNCTION__);
		krp->krp_status = ENOMEM;
		return ENOMEM;
	}

	/* Link parameters */
	dsaVerifyOpData->P.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PRIME_P_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->P.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PRIME_P_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->P.pData,
			    dsaVerifyOpData->P.dataLenInBytes);

	dsaVerifyOpData->Q.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PRIME_Q_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->Q.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PRIME_Q_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->Q.pData,
			    dsaVerifyOpData->Q.dataLenInBytes);

	dsaVerifyOpData->G.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_G_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->G.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_G_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->G.pData,
			    dsaVerifyOpData->G.dataLenInBytes);

	dsaVerifyOpData->Y.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PUBKEY_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->Y.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_PUBKEY_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->Y.pData,
			    dsaVerifyOpData->Y.dataLenInBytes);

	dsaVerifyOpData->M.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_DGST_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->M.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_DGST_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->M.pData,
			    dsaVerifyOpData->M.dataLenInBytes);

	dsaVerifyOpData->R.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_SIG_R_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->R.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_SIG_R_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->R.pData,
			    dsaVerifyOpData->R.dataLenInBytes);

	dsaVerifyOpData->S.pData =
	    krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_SIG_S_INDEX].crp_p;
	BITS_TO_BYTES(dsaVerifyOpData->S.dataLenInBytes,
		      krp->krp_param[ICP_DSA_VERIFY_KRP_PARAM_SIG_S_INDEX].
		      crp_nbits);
	icp_ocfDrvSwapBytes(dsaVerifyOpData->S.pData,
			    dsaVerifyOpData->S.dataLenInBytes);

	lacStatus = cpaCyDsaVerify(CPA_INSTANCE_HANDLE_SINGLE,
				   icp_ocfDrvDsaVerifyCallBack,
				   callbackTag, dsaVerifyOpData, &verifyStatus);

	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): DSA Verify Operation failed (%d).\n",
			__FUNCTION__, lacStatus);
		kmem_cache_free(drvDSAVerify_zone, dsaVerifyOpData);
		krp->krp_status = ECANCELED;
	}

	return lacStatus;
}

/* Name        : icp_ocfDrvReadRandom
 *
 * Description : This function will map RNG functionality calls from OCF
 * to the LAC API.
 */
int icp_ocfDrvReadRandom(void *arg, uint32_t * buf, int maxwords)
{
	CpaStatus lacStatus = CPA_STATUS_SUCCESS;
	CpaCyRandGenOpData randGenOpData;
	CpaFlatBuffer randData;

	if (NULL == buf) {
		APRINTK("%s(): Invalid input parameters\n", __FUNCTION__);
		return EINVAL;
	}

	/* maxwords here is number of integers to generate data for */
	randGenOpData.generateBits = CPA_TRUE;

	randGenOpData.lenInBytes = maxwords * sizeof(uint32_t);

	icp_ocfDrvPtrAndLenToFlatBuffer((Cpa8U *) buf,
					randGenOpData.lenInBytes, &randData);

	lacStatus = cpaCyRandGen(CPA_INSTANCE_HANDLE_SINGLE,
				 NULL, NULL, &randGenOpData, &randData);
	if (CPA_STATUS_SUCCESS != lacStatus) {
		EPRINTK("%s(): icp_LacSymRandGen failed (%d). \n",
			__FUNCTION__, lacStatus);
		return RETURN_RAND_NUM_GEN_FAILED;
	}

	return randGenOpData.lenInBytes / sizeof(uint32_t);
}

/* Name        : icp_ocfDrvDhP1Callback
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the DH operation.
 */
static void
icp_ocfDrvDhP1CallBack(void *callbackTag,
		       CpaStatus status,
		       void *pOpData, CpaFlatBuffer * pLocalOctetStringPV)
{
	struct cryptkop *krp = NULL;
	CpaCyDhPhase1KeyGenOpData *pPhase1OpData = NULL;

	if (NULL == callbackTag) {
		DPRINTK("%s(): Invalid input parameters - "
			"callbackTag data is NULL\n", __FUNCTION__);
		return;
	}
	krp = (struct cryptkop *)callbackTag;

	if (NULL == pOpData) {
		DPRINTK("%s(): Invalid input parameters - "
			"Operation Data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}
	pPhase1OpData = (CpaCyDhPhase1KeyGenOpData *) pOpData;

	if (NULL == pLocalOctetStringPV) {
		DPRINTK("%s(): Invalid input parameters - "
			"pLocalOctetStringPV Data is NULL\n", __FUNCTION__);
		memset(pPhase1OpData, 0, sizeof(CpaCyDhPhase1KeyGenOpData));
		kmem_cache_free(drvDH_zone, pPhase1OpData);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}

	if (CPA_STATUS_SUCCESS == status) {
		krp->krp_status = CRYPTO_OP_SUCCESS;
	} else {
		APRINTK("%s(): Diffie Hellman Phase1 Key Gen failed - "
			"Operation Status = %d\n", __FUNCTION__, status);
		krp->krp_status = ECANCELED;
	}

	icp_ocfDrvSwapBytes(pLocalOctetStringPV->pData,
			    pLocalOctetStringPV->dataLenInBytes);

	icp_ocfDrvFreeFlatBuffer(pLocalOctetStringPV);
	memset(pPhase1OpData, 0, sizeof(CpaCyDhPhase1KeyGenOpData));
	kmem_cache_free(drvDH_zone, pPhase1OpData);

	crypto_kdone(krp);

	return;
}

/* Name        : icp_ocfDrvModExpCallBack
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the Mod Exp operation.
 */
static void
icp_ocfDrvModExpCallBack(void *callbackTag,
			 CpaStatus status,
			 void *pOpdata, CpaFlatBuffer * pResult)
{
	struct cryptkop *krp = NULL;
	CpaCyLnModExpOpData *pLnModExpOpData = NULL;

	if (NULL == callbackTag) {
		DPRINTK("%s(): Invalid input parameters - "
			"callbackTag data is NULL\n", __FUNCTION__);
		return;
	}
	krp = (struct cryptkop *)callbackTag;

	if (NULL == pOpdata) {
		DPRINTK("%s(): Invalid Mod Exp input parameters - "
			"Operation Data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}
	pLnModExpOpData = (CpaCyLnModExpOpData *) pOpdata;

	if (NULL == pResult) {
		DPRINTK("%s(): Invalid input parameters - "
			"pResult data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		memset(pLnModExpOpData, 0, sizeof(CpaCyLnModExpOpData));
		kmem_cache_free(drvLnModExp_zone, pLnModExpOpData);
		crypto_kdone(krp);
		return;
	}

	if (CPA_STATUS_SUCCESS == status) {
		krp->krp_status = CRYPTO_OP_SUCCESS;
	} else {
		APRINTK("%s(): LAC Mod Exp Operation failed - "
			"Operation Status = %d\n", __FUNCTION__, status);
		krp->krp_status = ECANCELED;
	}

	icp_ocfDrvSwapBytes(pResult->pData, pResult->dataLenInBytes);

	/*switch base size value back to original */
	if (pLnModExpOpData->base.pData ==
	    (uint8_t *) & (krp->
			   krp_param[ICP_MOD_EXP_KRP_PARAM_BASE_INDEX].
			   crp_nbits)) {
		*((uint32_t *) pLnModExpOpData->base.pData) =
		    ntohl(*((uint32_t *) pLnModExpOpData->base.pData));
	}
	icp_ocfDrvFreeFlatBuffer(pResult);
	memset(pLnModExpOpData, 0, sizeof(CpaCyLnModExpOpData));
	kmem_cache_free(drvLnModExp_zone, pLnModExpOpData);

	crypto_kdone(krp);

	return;

}

/* Name        : icp_ocfDrvModExpCRTCallBack
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the Mod Exp CRT operation.
 */
static void
icp_ocfDrvModExpCRTCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData, CpaFlatBuffer * pOutputData)
{
	struct cryptkop *krp = NULL;
	CpaCyRsaDecryptOpData *pDecryptData = NULL;

	if (NULL == callbackTag) {
		DPRINTK("%s(): Invalid input parameters - "
			"callbackTag data is NULL\n", __FUNCTION__);
		return;
	}

	krp = (struct cryptkop *)callbackTag;

	if (NULL == pOpData) {
		DPRINTK("%s(): Invalid input parameters - "
			"Operation Data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}
	pDecryptData = (CpaCyRsaDecryptOpData *) pOpData;

	if (NULL == pOutputData) {
		DPRINTK("%s(): Invalid input parameter - "
			"pOutputData is NULL\n", __FUNCTION__);
		memset(pDecryptData->pRecipientPrivateKey, 0,
		       sizeof(CpaCyRsaPrivateKey));
		kmem_cache_free(drvRSAPrivateKey_zone,
				pDecryptData->pRecipientPrivateKey);
		memset(pDecryptData, 0, sizeof(CpaCyRsaDecryptOpData));
		kmem_cache_free(drvRSADecrypt_zone, pDecryptData);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}

	if (CPA_STATUS_SUCCESS == status) {
		krp->krp_status = CRYPTO_OP_SUCCESS;
	} else {
		APRINTK("%s(): LAC Mod Exp CRT operation failed - "
			"Operation Status = %d\n", __FUNCTION__, status);
		krp->krp_status = ECANCELED;
	}

	icp_ocfDrvSwapBytes(pOutputData->pData, pOutputData->dataLenInBytes);

	icp_ocfDrvFreeFlatBuffer(pOutputData);
	memset(pDecryptData->pRecipientPrivateKey, 0,
	       sizeof(CpaCyRsaPrivateKey));
	kmem_cache_free(drvRSAPrivateKey_zone,
			pDecryptData->pRecipientPrivateKey);
	memset(pDecryptData, 0, sizeof(CpaCyRsaDecryptOpData));
	kmem_cache_free(drvRSADecrypt_zone, pDecryptData);

	crypto_kdone(krp);

	return;
}

/* Name        : icp_ocfDrvDsaRSSignCallBack
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the DSA RS sign operation.
 */
static void
icp_ocfDrvDsaRSSignCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData,
			    CpaBoolean protocolStatus,
			    CpaFlatBuffer * pR, CpaFlatBuffer * pS)
{
	struct cryptkop *krp = NULL;
	CpaCyDsaRSSignOpData *pSignData = NULL;

	if (NULL == callbackTag) {
		DPRINTK("%s(): Invalid input parameters - "
			"callbackTag data is NULL\n", __FUNCTION__);
		return;
	}

	krp = (struct cryptkop *)callbackTag;

	if (NULL == pOpData) {
		DPRINTK("%s(): Invalid input parameters - "
			"Operation Data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}
	pSignData = (CpaCyDsaRSSignOpData *) pOpData;

	if (NULL == pR) {
		DPRINTK("%s(): Invalid input parameter - "
			"pR sign is NULL\n", __FUNCTION__);
		icp_ocfDrvFreeFlatBuffer(pS);
		kmem_cache_free(drvDSARSSign_zone, pSignData);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}

	if (NULL == pS) {
		DPRINTK("%s(): Invalid input parameter - "
			"pS sign is NULL\n", __FUNCTION__);
		icp_ocfDrvFreeFlatBuffer(pR);
		kmem_cache_free(drvDSARSSign_zone, pSignData);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}

	if (CPA_STATUS_SUCCESS != status) {
		APRINTK("%s(): LAC DSA RS Sign operation failed - "
			"Operation Status = %d\n", __FUNCTION__, status);
		krp->krp_status = ECANCELED;
	} else {
		krp->krp_status = CRYPTO_OP_SUCCESS;

		if (CPA_TRUE != protocolStatus) {
			DPRINTK("%s(): LAC DSA RS Sign operation failed due "
				"to protocol error\n", __FUNCTION__);
			krp->krp_status = EIO;
		}
	}

	/* Swap bytes only when the callback status is successful and
	   protocolStatus is set to true */
	if (CPA_STATUS_SUCCESS == status && CPA_TRUE == protocolStatus) {
		icp_ocfDrvSwapBytes(pR->pData, pR->dataLenInBytes);
		icp_ocfDrvSwapBytes(pS->pData, pS->dataLenInBytes);
	}

	icp_ocfDrvFreeFlatBuffer(pR);
	icp_ocfDrvFreeFlatBuffer(pS);
	memset(pSignData->K.pData, 0, pSignData->K.dataLenInBytes);
	kmem_cache_free(drvDSARSSignKValue_zone, pSignData->K.pData);
	memset(pSignData, 0, sizeof(CpaCyDsaRSSignOpData));
	kmem_cache_free(drvDSARSSign_zone, pSignData);
	crypto_kdone(krp);

	return;
}

/* Name        : icp_ocfDrvDsaVerifyCallback
 *
 * Description : When this function returns it signifies that the LAC
 * component has completed the DSA Verify operation.
 */
static void
icp_ocfDrvDsaVerifyCallBack(void *callbackTag,
			    CpaStatus status,
			    void *pOpData, CpaBoolean verifyStatus)
{

	struct cryptkop *krp = NULL;
	CpaCyDsaVerifyOpData *pVerData = NULL;

	if (NULL == callbackTag) {
		DPRINTK("%s(): Invalid input parameters - "
			"callbackTag data is NULL\n", __FUNCTION__);
		return;
	}

	krp = (struct cryptkop *)callbackTag;

	if (NULL == pOpData) {
		DPRINTK("%s(): Invalid input parameters - "
			"Operation Data is NULL\n", __FUNCTION__);
		krp->krp_status = ECANCELED;
		crypto_kdone(krp);
		return;
	}
	pVerData = (CpaCyDsaVerifyOpData *) pOpData;

	if (CPA_STATUS_SUCCESS != status) {
		APRINTK("%s(): LAC DSA Verify operation failed - "
			"Operation Status = %d\n", __FUNCTION__, status);
		krp->krp_status = ECANCELED;
	} else {
		krp->krp_status = CRYPTO_OP_SUCCESS;

		if (CPA_TRUE != verifyStatus) {
			DPRINTK("%s(): DSA signature invalid\n", __FUNCTION__);
			krp->krp_status = EIO;
		}
	}

	/* Swap bytes only when the callback status is successful and
	   verifyStatus is set to true */
	/*Just swapping back the key values for now. Possibly all
	   swapped buffers need to be reverted */
	if (CPA_STATUS_SUCCESS == status && CPA_TRUE == verifyStatus) {
		icp_ocfDrvSwapBytes(pVerData->R.pData,
				    pVerData->R.dataLenInBytes);
		icp_ocfDrvSwapBytes(pVerData->S.pData,
				    pVerData->S.dataLenInBytes);
	}

	memset(pVerData, 0, sizeof(CpaCyDsaVerifyOpData));
	kmem_cache_free(drvDSAVerify_zone, pVerData);
	crypto_kdone(krp);

	return;
}
