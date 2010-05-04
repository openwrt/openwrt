/*
 * An OCF module that uses Intels IXP CryptACC API to do the crypto.
 * This driver requires the IXP400 Access Library that is available
 * from Intel in order to operate (or compile).
 *
 * Written by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2006-2010 David McCullough
 * Copyright (C) 2004-2005 Intel Corporation.
 *
 * LICENSE TERMS
 *
 * The free distribution and use of this software in both source and binary
 * form is allowed (with or without changes) provided that:
 *
 *   1. distributions of this source code include the above copyright
 *      notice, this list of conditions and the following disclaimer;
 *
 *   2. distributions in binary form include the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other associated materials;
 *
 *   3. the copyright holder's name is not used to endorse products
 *      built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this product
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
 */

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/crypto.h>
#include <linux/interrupt.h>
#include <asm/scatterlist.h>

#include <IxTypes.h>
#include <IxOsBuffMgt.h>
#include <IxNpeDl.h>
#include <IxCryptoAcc.h>
#include <IxQMgr.h>
#include <IxOsServices.h>
#include <IxOsCacheMMU.h>

#include <cryptodev.h>
#include <uio.h>

#ifndef IX_MBUF_PRIV
#define IX_MBUF_PRIV(x) ((x)->priv)
#endif

struct ixp_data;

struct ixp_q {
	struct list_head	 ixp_q_list;
	struct ixp_data		*ixp_q_data;
	struct cryptop		*ixp_q_crp;
	struct cryptodesc	*ixp_q_ccrd;
	struct cryptodesc	*ixp_q_acrd;
	IX_MBUF				 ixp_q_mbuf;
	UINT8				*ixp_hash_dest; /* Location for hash in client buffer */
	UINT8				*ixp_hash_src; /* Location of hash in internal buffer */
	unsigned char		 ixp_q_iv_data[IX_CRYPTO_ACC_MAX_CIPHER_IV_LENGTH];
	unsigned char		*ixp_q_iv;
};

struct ixp_data {
	int					 ixp_registered;	/* is the context registered */
	int					 ixp_crd_flags;		/* detect direction changes */

	int					 ixp_cipher_alg;
	int					 ixp_auth_alg;

	UINT32				 ixp_ctx_id;
	UINT32				 ixp_hash_key_id;	/* used when hashing */
	IxCryptoAccCtx		 ixp_ctx;
	IX_MBUF				 ixp_pri_mbuf;
	IX_MBUF				 ixp_sec_mbuf;

	struct work_struct   ixp_pending_work;
	struct work_struct   ixp_registration_work;
	struct list_head	 ixp_q;				/* unprocessed requests */
};

#ifdef __ixp46X

#define	MAX_IOP_SIZE	64	/* words */
#define	MAX_OOP_SIZE	128

#define	MAX_PARAMS		3

struct ixp_pkq {
	struct list_head			 pkq_list;
	struct cryptkop				*pkq_krp;

	IxCryptoAccPkeEauInOperands	 pkq_op;
	IxCryptoAccPkeEauOpResult	 pkq_result;

	UINT32						 pkq_ibuf0[MAX_IOP_SIZE];
	UINT32						 pkq_ibuf1[MAX_IOP_SIZE];
	UINT32						 pkq_ibuf2[MAX_IOP_SIZE];
	UINT32						 pkq_obuf[MAX_OOP_SIZE];
};

static LIST_HEAD(ixp_pkq); /* current PK wait list */
static struct ixp_pkq *ixp_pk_cur;
static spinlock_t ixp_pkq_lock;

#endif /* __ixp46X */

static int ixp_blocked = 0;

static int32_t			 ixp_id = -1;
static struct ixp_data **ixp_sessions = NULL;
static u_int32_t		 ixp_sesnum = 0;

static int ixp_process(device_t, struct cryptop *, int);
static int ixp_newsession(device_t, u_int32_t *, struct cryptoini *);
static int ixp_freesession(device_t, u_int64_t);
#ifdef __ixp46X
static int ixp_kprocess(device_t, struct cryptkop *krp, int hint);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
static kmem_cache_t *qcache;
#else
static struct kmem_cache *qcache;
#endif

#define debug ixp_debug
static int ixp_debug = 0;
module_param(ixp_debug, int, 0644);
MODULE_PARM_DESC(ixp_debug, "Enable debug");

static int ixp_init_crypto = 1;
module_param(ixp_init_crypto, int, 0444); /* RO after load/boot */
MODULE_PARM_DESC(ixp_init_crypto, "Call ixCryptoAccInit (default is 1)");

static void ixp_process_pending(void *arg);
static void ixp_registration(void *arg);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void ixp_process_pending_wq(struct work_struct *work);
static void ixp_registration_wq(struct work_struct *work);
#endif

/*
 * dummy device structure
 */

static struct {
	softc_device_decl	sc_dev;
} ixpdev;

static device_method_t ixp_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	ixp_newsession),
	DEVMETHOD(cryptodev_freesession,ixp_freesession),
	DEVMETHOD(cryptodev_process,	ixp_process),
#ifdef __ixp46X
	DEVMETHOD(cryptodev_kprocess,	ixp_kprocess),
#endif
};

/*
 * Generate a new software session.
 */
static int
ixp_newsession(device_t dev, u_int32_t *sid, struct cryptoini *cri)
{
	struct ixp_data *ixp;
	u_int32_t i;
#define AUTH_LEN(cri, def) \
	(cri->cri_mlen ? cri->cri_mlen : (def))

	dprintk("%s():alg %d\n", __FUNCTION__,cri->cri_alg);
	if (sid == NULL || cri == NULL) {
		dprintk("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	if (ixp_sessions) {
		for (i = 1; i < ixp_sesnum; i++)
			if (ixp_sessions[i] == NULL)
				break;
	} else
		i = 1;		/* NB: to silence compiler warning */

	if (ixp_sessions == NULL || i == ixp_sesnum) {
		struct ixp_data **ixpd;

		if (ixp_sessions == NULL) {
			i = 1; /* We leave ixp_sessions[0] empty */
			ixp_sesnum = CRYPTO_SW_SESSIONS;
		} else
			ixp_sesnum *= 2;

		ixpd = kmalloc(ixp_sesnum * sizeof(struct ixp_data *), SLAB_ATOMIC);
		if (ixpd == NULL) {
			/* Reset session number */
			if (ixp_sesnum == CRYPTO_SW_SESSIONS)
				ixp_sesnum = 0;
			else
				ixp_sesnum /= 2;
			dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
			return ENOBUFS;
		}
		memset(ixpd, 0, ixp_sesnum * sizeof(struct ixp_data *));

		/* Copy existing sessions */
		if (ixp_sessions) {
			memcpy(ixpd, ixp_sessions,
			    (ixp_sesnum / 2) * sizeof(struct ixp_data *));
			kfree(ixp_sessions);
		}

		ixp_sessions = ixpd;
	}

	ixp_sessions[i] = (struct ixp_data *) kmalloc(sizeof(struct ixp_data),
			SLAB_ATOMIC);
	if (ixp_sessions[i] == NULL) {
		ixp_freesession(NULL, i);
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return ENOBUFS;
	}

	*sid = i;

	ixp = ixp_sessions[i];
	memset(ixp, 0, sizeof(*ixp));

	ixp->ixp_cipher_alg = -1;
	ixp->ixp_auth_alg = -1;
	ixp->ixp_ctx_id = -1;
	INIT_LIST_HEAD(&ixp->ixp_q);

	ixp->ixp_ctx.useDifferentSrcAndDestMbufs = 0;

	while (cri) {
		switch (cri->cri_alg) {
		case CRYPTO_DES_CBC:
			ixp->ixp_cipher_alg = cri->cri_alg;
			ixp->ixp_ctx.cipherCtx.cipherAlgo = IX_CRYPTO_ACC_CIPHER_DES;
			ixp->ixp_ctx.cipherCtx.cipherMode = IX_CRYPTO_ACC_MODE_CBC;
			ixp->ixp_ctx.cipherCtx.cipherKeyLen = (cri->cri_klen + 7) / 8;
			ixp->ixp_ctx.cipherCtx.cipherBlockLen = IX_CRYPTO_ACC_DES_BLOCK_64;
			ixp->ixp_ctx.cipherCtx.cipherInitialVectorLen =
						IX_CRYPTO_ACC_DES_IV_64;
			memcpy(ixp->ixp_ctx.cipherCtx.key.cipherKey,
					cri->cri_key, (cri->cri_klen + 7) / 8);
			break;

		case CRYPTO_3DES_CBC:
			ixp->ixp_cipher_alg = cri->cri_alg;
			ixp->ixp_ctx.cipherCtx.cipherAlgo = IX_CRYPTO_ACC_CIPHER_3DES;
			ixp->ixp_ctx.cipherCtx.cipherMode = IX_CRYPTO_ACC_MODE_CBC;
			ixp->ixp_ctx.cipherCtx.cipherKeyLen = (cri->cri_klen + 7) / 8;
			ixp->ixp_ctx.cipherCtx.cipherBlockLen = IX_CRYPTO_ACC_DES_BLOCK_64;
			ixp->ixp_ctx.cipherCtx.cipherInitialVectorLen =
						IX_CRYPTO_ACC_DES_IV_64;
			memcpy(ixp->ixp_ctx.cipherCtx.key.cipherKey,
					cri->cri_key, (cri->cri_klen + 7) / 8);
			break;

		case CRYPTO_RIJNDAEL128_CBC:
			ixp->ixp_cipher_alg = cri->cri_alg;
			ixp->ixp_ctx.cipherCtx.cipherAlgo = IX_CRYPTO_ACC_CIPHER_AES;
			ixp->ixp_ctx.cipherCtx.cipherMode = IX_CRYPTO_ACC_MODE_CBC;
			ixp->ixp_ctx.cipherCtx.cipherKeyLen = (cri->cri_klen + 7) / 8;
			ixp->ixp_ctx.cipherCtx.cipherBlockLen = 16;
			ixp->ixp_ctx.cipherCtx.cipherInitialVectorLen = 16;
			memcpy(ixp->ixp_ctx.cipherCtx.key.cipherKey,
					cri->cri_key, (cri->cri_klen + 7) / 8);
			break;

		case CRYPTO_MD5:
		case CRYPTO_MD5_HMAC:
			ixp->ixp_auth_alg = cri->cri_alg;
			ixp->ixp_ctx.authCtx.authAlgo = IX_CRYPTO_ACC_AUTH_MD5;
			ixp->ixp_ctx.authCtx.authDigestLen = AUTH_LEN(cri, MD5_HASH_LEN);
			ixp->ixp_ctx.authCtx.aadLen = 0;
			/* Only MD5_HMAC needs a key */
			if (cri->cri_alg == CRYPTO_MD5_HMAC) {
				ixp->ixp_ctx.authCtx.authKeyLen = (cri->cri_klen + 7) / 8;
				if (ixp->ixp_ctx.authCtx.authKeyLen >
						sizeof(ixp->ixp_ctx.authCtx.key.authKey)) {
					printk(
						"ixp4xx: Invalid key length for MD5_HMAC - %d bits\n",
							cri->cri_klen);
					ixp_freesession(NULL, i);
					return EINVAL;
				}
				memcpy(ixp->ixp_ctx.authCtx.key.authKey,
						cri->cri_key, (cri->cri_klen + 7) / 8);
			}
			break;

		case CRYPTO_SHA1:
		case CRYPTO_SHA1_HMAC:
			ixp->ixp_auth_alg = cri->cri_alg;
			ixp->ixp_ctx.authCtx.authAlgo = IX_CRYPTO_ACC_AUTH_SHA1;
			ixp->ixp_ctx.authCtx.authDigestLen = AUTH_LEN(cri, SHA1_HASH_LEN);
			ixp->ixp_ctx.authCtx.aadLen = 0;
			/* Only SHA1_HMAC needs a key */
			if (cri->cri_alg == CRYPTO_SHA1_HMAC) {
				ixp->ixp_ctx.authCtx.authKeyLen = (cri->cri_klen + 7) / 8;
				if (ixp->ixp_ctx.authCtx.authKeyLen >
						sizeof(ixp->ixp_ctx.authCtx.key.authKey)) {
					printk(
						"ixp4xx: Invalid key length for SHA1_HMAC - %d bits\n",
							cri->cri_klen);
					ixp_freesession(NULL, i);
					return EINVAL;
				}
				memcpy(ixp->ixp_ctx.authCtx.key.authKey,
						cri->cri_key, (cri->cri_klen + 7) / 8);
			}
			break;

		default:
			printk("ixp: unknown algo 0x%x\n", cri->cri_alg);
			ixp_freesession(NULL, i);
			return EINVAL;
		}
		cri = cri->cri_next;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&ixp->ixp_pending_work, ixp_process_pending_wq);
	INIT_WORK(&ixp->ixp_registration_work, ixp_registration_wq);
#else
	INIT_WORK(&ixp->ixp_pending_work, ixp_process_pending, ixp);
	INIT_WORK(&ixp->ixp_registration_work, ixp_registration, ixp);
#endif

	return 0;
}


/*
 * Free a session.
 */
static int
ixp_freesession(device_t dev, u_int64_t tid)
{
	u_int32_t sid = CRYPTO_SESID2LID(tid);

	dprintk("%s()\n", __FUNCTION__);
	if (sid > ixp_sesnum || ixp_sessions == NULL ||
			ixp_sessions[sid] == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	/* Silently accept and return */
	if (sid == 0)
		return 0;

	if (ixp_sessions[sid]) {
		if (ixp_sessions[sid]->ixp_ctx_id != -1) {
			ixCryptoAccCtxUnregister(ixp_sessions[sid]->ixp_ctx_id);
			ixp_sessions[sid]->ixp_ctx_id = -1;
		}
		kfree(ixp_sessions[sid]);
	}
	ixp_sessions[sid] = NULL;
	if (ixp_blocked) {
		ixp_blocked = 0;
		crypto_unblock(ixp_id, CRYPTO_SYMQ);
	}
	return 0;
}


/*
 * callback for when hash processing is complete
 */

static void
ixp_hash_perform_cb(
	UINT32 hash_key_id,
	IX_MBUF *bufp,
	IxCryptoAccStatus status)
{
	struct ixp_q *q;

	dprintk("%s(%u, %p, 0x%x)\n", __FUNCTION__, hash_key_id, bufp, status);

	if (bufp == NULL) {
		printk("ixp: NULL buf in %s\n", __FUNCTION__);
		return;
	}

	q = IX_MBUF_PRIV(bufp);
	if (q == NULL) {
		printk("ixp: NULL priv in %s\n", __FUNCTION__);
		return;
	}

	if (status == IX_CRYPTO_ACC_STATUS_SUCCESS) {
		/* On success, need to copy hash back into original client buffer */
		memcpy(q->ixp_hash_dest, q->ixp_hash_src,
				(q->ixp_q_data->ixp_auth_alg == CRYPTO_SHA1) ?
					SHA1_HASH_LEN : MD5_HASH_LEN);
	}
	else {
		printk("ixp: hash perform failed status=%d\n", status);
		q->ixp_q_crp->crp_etype = EINVAL;
	}

	/* Free internal buffer used for hashing */
	kfree(IX_MBUF_MDATA(&q->ixp_q_mbuf));

	crypto_done(q->ixp_q_crp);
	kmem_cache_free(qcache, q);
}

/*
 * setup a request and perform it
 */
static void
ixp_q_process(struct ixp_q *q)
{
	IxCryptoAccStatus status;
	struct ixp_data *ixp = q->ixp_q_data;
	int auth_off = 0;
	int auth_len = 0;
	int crypt_off = 0;
	int crypt_len = 0;
	int icv_off = 0;
	char *crypt_func;

	dprintk("%s(%p)\n", __FUNCTION__, q);

	if (q->ixp_q_ccrd) {
		if (q->ixp_q_ccrd->crd_flags & CRD_F_IV_EXPLICIT) {
			q->ixp_q_iv = q->ixp_q_ccrd->crd_iv;
		} else {
			q->ixp_q_iv = q->ixp_q_iv_data;
			crypto_copydata(q->ixp_q_crp->crp_flags, q->ixp_q_crp->crp_buf,
					q->ixp_q_ccrd->crd_inject,
					ixp->ixp_ctx.cipherCtx.cipherInitialVectorLen,
					(caddr_t) q->ixp_q_iv);
		}

		if (q->ixp_q_acrd) {
			auth_off = q->ixp_q_acrd->crd_skip;
			auth_len = q->ixp_q_acrd->crd_len;
			icv_off  = q->ixp_q_acrd->crd_inject;
		}

		crypt_off = q->ixp_q_ccrd->crd_skip;
		crypt_len = q->ixp_q_ccrd->crd_len;
	} else { /* if (q->ixp_q_acrd) */
		auth_off = q->ixp_q_acrd->crd_skip;
		auth_len = q->ixp_q_acrd->crd_len;
		icv_off  = q->ixp_q_acrd->crd_inject;
	}

	if (q->ixp_q_crp->crp_flags & CRYPTO_F_SKBUF) {
		struct sk_buff *skb = (struct sk_buff *) q->ixp_q_crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags) {
			/*
			 * DAVIDM fix this limitation one day by using
			 * a buffer pool and chaining,  it is not currently
			 * needed for current user/kernel space acceleration
			 */
			printk("ixp: Cannot handle fragmented skb's yet !\n");
			q->ixp_q_crp->crp_etype = ENOENT;
			goto done;
		}
		IX_MBUF_MLEN(&q->ixp_q_mbuf) =
				IX_MBUF_PKT_LEN(&q->ixp_q_mbuf) =  skb->len;
		IX_MBUF_MDATA(&q->ixp_q_mbuf) = skb->data;
	} else if (q->ixp_q_crp->crp_flags & CRYPTO_F_IOV) {
		struct uio *uiop = (struct uio *) q->ixp_q_crp->crp_buf;
		if (uiop->uio_iovcnt != 1) {
			/*
			 * DAVIDM fix this limitation one day by using
			 * a buffer pool and chaining,  it is not currently
			 * needed for current user/kernel space acceleration
			 */
			printk("ixp: Cannot handle more than 1 iovec yet !\n");
			q->ixp_q_crp->crp_etype = ENOENT;
			goto done;
		}
		IX_MBUF_MLEN(&q->ixp_q_mbuf) =
				IX_MBUF_PKT_LEN(&q->ixp_q_mbuf) = uiop->uio_iov[0].iov_len;
		IX_MBUF_MDATA(&q->ixp_q_mbuf) = uiop->uio_iov[0].iov_base;
	} else /* contig buffer */ {
		IX_MBUF_MLEN(&q->ixp_q_mbuf)  =
				IX_MBUF_PKT_LEN(&q->ixp_q_mbuf) = q->ixp_q_crp->crp_ilen;
		IX_MBUF_MDATA(&q->ixp_q_mbuf) = q->ixp_q_crp->crp_buf;
	}

	IX_MBUF_PRIV(&q->ixp_q_mbuf) = q;

	if (ixp->ixp_auth_alg == CRYPTO_SHA1 || ixp->ixp_auth_alg == CRYPTO_MD5) {
		/*
		 * For SHA1 and MD5 hash, need to create an internal buffer that is big
		 * enough to hold the original data + the appropriate padding for the
		 * hash algorithm.
		 */
		UINT8 *tbuf = NULL;

		IX_MBUF_MLEN(&q->ixp_q_mbuf) = IX_MBUF_PKT_LEN(&q->ixp_q_mbuf) =
			((IX_MBUF_MLEN(&q->ixp_q_mbuf) * 8) + 72 + 511) / 8;
		tbuf = kmalloc(IX_MBUF_MLEN(&q->ixp_q_mbuf), SLAB_ATOMIC);
		
		if (IX_MBUF_MDATA(&q->ixp_q_mbuf) == NULL) {
			printk("ixp: kmalloc(%u, SLAB_ATOMIC) failed\n",
					IX_MBUF_MLEN(&q->ixp_q_mbuf));
			q->ixp_q_crp->crp_etype = ENOMEM;
			goto done;
		}
		memcpy(tbuf, &(IX_MBUF_MDATA(&q->ixp_q_mbuf))[auth_off], auth_len);

		/* Set location in client buffer to copy hash into */
		q->ixp_hash_dest =
			&(IX_MBUF_MDATA(&q->ixp_q_mbuf))[auth_off + auth_len];

		IX_MBUF_MDATA(&q->ixp_q_mbuf) = tbuf;

		/* Set location in internal buffer for where hash starts */
		q->ixp_hash_src = &(IX_MBUF_MDATA(&q->ixp_q_mbuf))[auth_len];

		crypt_func = "ixCryptoAccHashPerform";
		status = ixCryptoAccHashPerform(ixp->ixp_ctx.authCtx.authAlgo,
				&q->ixp_q_mbuf, ixp_hash_perform_cb, 0, auth_len, auth_len,
				&ixp->ixp_hash_key_id);
	}
	else {
		crypt_func = "ixCryptoAccAuthCryptPerform";
		status = ixCryptoAccAuthCryptPerform(ixp->ixp_ctx_id, &q->ixp_q_mbuf,
			NULL, auth_off, auth_len, crypt_off, crypt_len, icv_off,
			q->ixp_q_iv);
	}

	if (IX_CRYPTO_ACC_STATUS_SUCCESS == status)
		return;

	if (IX_CRYPTO_ACC_STATUS_QUEUE_FULL == status) {
		q->ixp_q_crp->crp_etype = ENOMEM;
		goto done;
	}

	printk("ixp: %s failed %u\n", crypt_func, status);
	q->ixp_q_crp->crp_etype = EINVAL;

done:
	crypto_done(q->ixp_q_crp);
	kmem_cache_free(qcache, q);
}


/*
 * because we cannot process the Q from the Register callback
 * we do it here on a task Q.
 */

static void
ixp_process_pending(void *arg)
{
	struct ixp_data *ixp = arg;
	struct ixp_q *q = NULL;

	dprintk("%s(%p)\n", __FUNCTION__, arg);

	if (!ixp)
		return;

	while (!list_empty(&ixp->ixp_q)) {
		q = list_entry(ixp->ixp_q.next, struct ixp_q, ixp_q_list);
		list_del(&q->ixp_q_list);
		ixp_q_process(q);
	}
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void
ixp_process_pending_wq(struct work_struct *work)
{
	struct ixp_data *ixp = container_of(work, struct ixp_data, ixp_pending_work);
	ixp_process_pending(ixp);
}
#endif

/*
 * callback for when context registration is complete
 */

static void
ixp_register_cb(UINT32 ctx_id, IX_MBUF *bufp, IxCryptoAccStatus status)
{
	int i;
	struct ixp_data *ixp;
	struct ixp_q *q;

	dprintk("%s(%d, %p, %d)\n", __FUNCTION__, ctx_id, bufp, status);

	/*
	 * free any buffer passed in to this routine
	 */
	if (bufp) {
		IX_MBUF_MLEN(bufp) = IX_MBUF_PKT_LEN(bufp) = 0;
		kfree(IX_MBUF_MDATA(bufp));
		IX_MBUF_MDATA(bufp) = NULL;
	}

	for (i = 0; i < ixp_sesnum; i++) {
		ixp = ixp_sessions[i];
		if (ixp && ixp->ixp_ctx_id == ctx_id)
			break;
	}
	if (i >= ixp_sesnum) {
		printk("ixp: invalid context id %d\n", ctx_id);
		return;
	}

	if (IX_CRYPTO_ACC_STATUS_WAIT == status) {
		/* this is normal to free the first of two buffers */
		dprintk("ixp: register not finished yet.\n");
		return;
	}

	if (IX_CRYPTO_ACC_STATUS_SUCCESS != status) {
		printk("ixp: register failed 0x%x\n", status);
		while (!list_empty(&ixp->ixp_q)) {
			q = list_entry(ixp->ixp_q.next, struct ixp_q, ixp_q_list);
			list_del(&q->ixp_q_list);
			q->ixp_q_crp->crp_etype = EINVAL;
			crypto_done(q->ixp_q_crp);
			kmem_cache_free(qcache, q);
		}
		return;
	}

	/*
	 * we are now registered,  we cannot start processing the Q here
	 * or we get strange errors with AES (DES/3DES seem to be ok).
	 */
	ixp->ixp_registered = 1;
	schedule_work(&ixp->ixp_pending_work);
}


/*
 * callback for when data processing is complete
 */

static void
ixp_perform_cb(
	UINT32 ctx_id,
	IX_MBUF *sbufp,
	IX_MBUF *dbufp,
	IxCryptoAccStatus status)
{
	struct ixp_q *q;

	dprintk("%s(%d, %p, %p, 0x%x)\n", __FUNCTION__, ctx_id, sbufp,
			dbufp, status);

	if (sbufp == NULL) {
		printk("ixp: NULL sbuf in ixp_perform_cb\n");
		return;
	}

	q = IX_MBUF_PRIV(sbufp);
	if (q == NULL) {
		printk("ixp: NULL priv in ixp_perform_cb\n");
		return;
	}

	if (status != IX_CRYPTO_ACC_STATUS_SUCCESS) {
		printk("ixp: perform failed status=%d\n", status);
		q->ixp_q_crp->crp_etype = EINVAL;
	}

	crypto_done(q->ixp_q_crp);
	kmem_cache_free(qcache, q);
}


/*
 * registration is not callable at IRQ time,  so we defer
 * to a task queue,  this routines completes the registration for us
 * when the task queue runs
 *
 * Unfortunately this means we cannot tell OCF that the driver is blocked,
 * we do that on the next request.
 */

static void
ixp_registration(void *arg)
{
	struct ixp_data *ixp = arg;
	struct ixp_q *q = NULL;
	IX_MBUF *pri = NULL, *sec = NULL;
	int status = IX_CRYPTO_ACC_STATUS_SUCCESS;

	if (!ixp) {
		printk("ixp: ixp_registration with no arg\n");
		return;
	}

	if (ixp->ixp_ctx_id != -1) {
		ixCryptoAccCtxUnregister(ixp->ixp_ctx_id);
		ixp->ixp_ctx_id = -1;
	}

	if (list_empty(&ixp->ixp_q)) {
		printk("ixp: ixp_registration with no Q\n");
		return;
	}

	/*
	 * setup the primary and secondary buffers
	 */
	q = list_entry(ixp->ixp_q.next, struct ixp_q, ixp_q_list);
	if (q->ixp_q_acrd) {
		pri = &ixp->ixp_pri_mbuf;
		sec = &ixp->ixp_sec_mbuf;
		IX_MBUF_MLEN(pri)  = IX_MBUF_PKT_LEN(pri) = 128;
		IX_MBUF_MDATA(pri) = (unsigned char *) kmalloc(128, SLAB_ATOMIC);
		IX_MBUF_MLEN(sec)  = IX_MBUF_PKT_LEN(sec) = 128;
		IX_MBUF_MDATA(sec) = (unsigned char *) kmalloc(128, SLAB_ATOMIC);
	}

	/* Only need to register if a crypt op or HMAC op */
	if (!(ixp->ixp_auth_alg == CRYPTO_SHA1 ||
				ixp->ixp_auth_alg == CRYPTO_MD5)) {
		status = ixCryptoAccCtxRegister(
					&ixp->ixp_ctx,
					pri, sec,
					ixp_register_cb,
					ixp_perform_cb,
					&ixp->ixp_ctx_id);
	}
	else {
		/* Otherwise we start processing pending q */
		schedule_work(&ixp->ixp_pending_work);
	}

	if (IX_CRYPTO_ACC_STATUS_SUCCESS == status)
		return;

	if (IX_CRYPTO_ACC_STATUS_EXCEED_MAX_TUNNELS == status) {
		printk("ixp: ixCryptoAccCtxRegister failed (out of tunnels)\n");
		ixp_blocked = 1;
		/* perhaps we should return EGAIN on queued ops ? */
		return;
	}

	printk("ixp: ixCryptoAccCtxRegister failed %d\n", status);
	ixp->ixp_ctx_id = -1;

	/*
	 * everything waiting is toasted
	 */
	while (!list_empty(&ixp->ixp_q)) {
		q = list_entry(ixp->ixp_q.next, struct ixp_q, ixp_q_list);
		list_del(&q->ixp_q_list);
		q->ixp_q_crp->crp_etype = ENOENT;
		crypto_done(q->ixp_q_crp);
		kmem_cache_free(qcache, q);
	}
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void
ixp_registration_wq(struct work_struct *work)
{
	struct ixp_data *ixp = container_of(work, struct ixp_data,
								ixp_registration_work);
	ixp_registration(ixp);
}
#endif

/*
 * Process a request.
 */
static int
ixp_process(device_t dev, struct cryptop *crp, int hint)
{
	struct ixp_data *ixp;
	unsigned int lid;
	struct ixp_q *q = NULL;
	int status;

	dprintk("%s()\n", __FUNCTION__);

	/* Sanity check */
	if (crp == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	crp->crp_etype = 0;

	if (ixp_blocked)
		return ERESTART;

	if (crp->crp_desc == NULL || crp->crp_buf == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	}

	/*
	 * find the session we are using
	 */

	lid = crp->crp_sid & 0xffffffff;
	if (lid >= ixp_sesnum || lid == 0 || ixp_sessions == NULL ||
			ixp_sessions[lid] == NULL) {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}
	ixp = ixp_sessions[lid];

	/*
	 * setup a new request ready for queuing
	 */
	q = kmem_cache_alloc(qcache, SLAB_ATOMIC);
	if (q == NULL) {
		dprintk("%s,%d: ENOMEM\n", __FILE__, __LINE__);
		crp->crp_etype = ENOMEM;
		goto done;
	}
	/*
	 * save some cycles by only zeroing the important bits
	 */
	memset(&q->ixp_q_mbuf, 0, sizeof(q->ixp_q_mbuf));
	q->ixp_q_ccrd = NULL;
	q->ixp_q_acrd = NULL;
	q->ixp_q_crp = crp;
	q->ixp_q_data = ixp;

	/*
	 * point the cipher and auth descriptors appropriately
	 * check that we have something to do
	 */
	if (crp->crp_desc->crd_alg == ixp->ixp_cipher_alg)
		q->ixp_q_ccrd = crp->crp_desc;
	else if (crp->crp_desc->crd_alg == ixp->ixp_auth_alg)
		q->ixp_q_acrd = crp->crp_desc;
	else {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: bad desc match: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}
	if (crp->crp_desc->crd_next) {
		if (crp->crp_desc->crd_next->crd_alg == ixp->ixp_cipher_alg)
			q->ixp_q_ccrd = crp->crp_desc->crd_next;
		else if (crp->crp_desc->crd_next->crd_alg == ixp->ixp_auth_alg)
			q->ixp_q_acrd = crp->crp_desc->crd_next;
		else {
			crp->crp_etype = ENOENT;
			dprintk("%s,%d: bad desc match: ENOENT\n", __FILE__, __LINE__);
			goto done;
		}
	}

	/*
	 * If there is a direction change for this context then we mark it as
	 * unregistered and re-register is for the new direction.  This is not
	 * a very expensive operation and currently only tends to happen when
	 * user-space application are doing benchmarks
	 *
	 * DM - we should be checking for pending requests before unregistering.
	 */
	if (q->ixp_q_ccrd && ixp->ixp_registered &&
			ixp->ixp_crd_flags != (q->ixp_q_ccrd->crd_flags & CRD_F_ENCRYPT)) {
		dprintk("%s - detected direction change on session\n", __FUNCTION__);
		ixp->ixp_registered = 0;
	}

	/*
	 * if we are registered,  call straight into the perform code
	 */
	if (ixp->ixp_registered) {
		ixp_q_process(q);
		return 0;
	}

	/*
	 * the only part of the context not set in newsession is the direction
	 * dependent parts
	 */
	if (q->ixp_q_ccrd) {
		ixp->ixp_crd_flags = (q->ixp_q_ccrd->crd_flags & CRD_F_ENCRYPT);
		if (q->ixp_q_ccrd->crd_flags & CRD_F_ENCRYPT) {
			ixp->ixp_ctx.operation = q->ixp_q_acrd ?
					IX_CRYPTO_ACC_OP_ENCRYPT_AUTH : IX_CRYPTO_ACC_OP_ENCRYPT;
		} else {
			ixp->ixp_ctx.operation = q->ixp_q_acrd ?
					IX_CRYPTO_ACC_OP_AUTH_DECRYPT : IX_CRYPTO_ACC_OP_DECRYPT;
		}
	} else {
		/* q->ixp_q_acrd must be set if we are here */
		ixp->ixp_ctx.operation = IX_CRYPTO_ACC_OP_AUTH_CALC;
	}

	status = list_empty(&ixp->ixp_q);
	list_add_tail(&q->ixp_q_list, &ixp->ixp_q);
	if (status)
		schedule_work(&ixp->ixp_registration_work);
	return 0;

done:
	if (q)
		kmem_cache_free(qcache, q);
	crypto_done(crp);
	return 0;
}


#ifdef __ixp46X
/*
 * key processing support for the ixp465
 */


/*
 * copy a BN (LE) into a buffer (BE) an fill out the op appropriately
 * assume zeroed and only copy bits that are significant
 */

static int
ixp_copy_ibuf(struct crparam *p, IxCryptoAccPkeEauOperand *op, UINT32 *buf)
{
	unsigned char *src = (unsigned char *) p->crp_p;
	unsigned char *dst;
	int len, bits = p->crp_nbits;

	dprintk("%s()\n", __FUNCTION__);

	if (bits > MAX_IOP_SIZE * sizeof(UINT32) * 8) {
		dprintk("%s - ibuf too big (%d > %d)\n", __FUNCTION__,
				bits, MAX_IOP_SIZE * sizeof(UINT32) * 8);
		return -1;
	}

	len = (bits + 31) / 32; /* the number UINT32's needed */

	dst = (unsigned char *) &buf[len];
	dst--;

	while (bits > 0) {
		*dst-- = *src++;
		bits -= 8;
	}

#if 0 /* no need to zero remaining bits as it is done during request alloc */
	while (dst > (unsigned char *) buf)
		*dst-- = '\0';
#endif

	op->pData = buf;
	op->dataLen = len;
	return 0;
}

/*
 * copy out the result,  be as forgiving as we can about small output buffers
 */

static int
ixp_copy_obuf(struct crparam *p, IxCryptoAccPkeEauOpResult *op, UINT32 *buf)
{
	unsigned char *dst = (unsigned char *) p->crp_p;
	unsigned char *src = (unsigned char *) buf;
	int len, z, bits = p->crp_nbits;

	dprintk("%s()\n", __FUNCTION__);

	len = op->dataLen * sizeof(UINT32);

	/* skip leading zeroes to be small buffer friendly */
	z = 0;
	while (z < len && src[z] == '\0')
		z++;

	src += len;
	src--;
	len -= z;

	while (len > 0 && bits > 0) {
		*dst++ = *src--;
		len--;
		bits -= 8;
	}

	while (bits > 0) {
		*dst++ = '\0';
		bits -= 8;
	}

	if (len > 0) {
		dprintk("%s - obuf is %d (z=%d, ob=%d) bytes too small\n",
				__FUNCTION__, len, z, p->crp_nbits / 8);
		return -1;
	}

	return 0;
}


/*
 * the parameter offsets for exp_mod
 */

#define IXP_PARAM_BASE 0
#define IXP_PARAM_EXP  1
#define IXP_PARAM_MOD  2
#define IXP_PARAM_RES  3

/*
 * key processing complete callback,  is also used to start processing
 * by passing a NULL for pResult
 */

static void
ixp_kperform_cb(
	IxCryptoAccPkeEauOperation operation,
	IxCryptoAccPkeEauOpResult *pResult,
	BOOL carryOrBorrow,
	IxCryptoAccStatus status)
{
	struct ixp_pkq *q, *tmp;
	unsigned long flags;

	dprintk("%s(0x%x, %p, %d, 0x%x)\n", __FUNCTION__, operation, pResult,
			carryOrBorrow, status);

	/* handle a completed request */
	if (pResult) {
		if (ixp_pk_cur && &ixp_pk_cur->pkq_result == pResult) {
			q = ixp_pk_cur;
			if (status != IX_CRYPTO_ACC_STATUS_SUCCESS) {
				dprintk("%s() - op failed 0x%x\n", __FUNCTION__, status);
				q->pkq_krp->krp_status = ERANGE; /* could do better */
			} else {
				/* copy out the result */
				if (ixp_copy_obuf(&q->pkq_krp->krp_param[IXP_PARAM_RES],
						&q->pkq_result, q->pkq_obuf))
					q->pkq_krp->krp_status = ERANGE;
			}
			crypto_kdone(q->pkq_krp);
			kfree(q);
			ixp_pk_cur = NULL;
		} else
			printk("%s - callback with invalid result pointer\n", __FUNCTION__);
	}

	spin_lock_irqsave(&ixp_pkq_lock, flags);
	if (ixp_pk_cur || list_empty(&ixp_pkq)) {
		spin_unlock_irqrestore(&ixp_pkq_lock, flags);
		return;
	}

	list_for_each_entry_safe(q, tmp, &ixp_pkq, pkq_list) {

		list_del(&q->pkq_list);
		ixp_pk_cur = q;

		spin_unlock_irqrestore(&ixp_pkq_lock, flags);

		status = ixCryptoAccPkeEauPerform(
				IX_CRYPTO_ACC_OP_EAU_MOD_EXP,
				&q->pkq_op,
				ixp_kperform_cb,
				&q->pkq_result);
	
		if (status == IX_CRYPTO_ACC_STATUS_SUCCESS) {
			dprintk("%s() - ixCryptoAccPkeEauPerform SUCCESS\n", __FUNCTION__);
			return; /* callback will return here for callback */
		} else if (status == IX_CRYPTO_ACC_STATUS_RETRY) {
			printk("%s() - ixCryptoAccPkeEauPerform RETRY\n", __FUNCTION__);
		} else {
			printk("%s() - ixCryptoAccPkeEauPerform failed %d\n",
					__FUNCTION__, status);
		}
		q->pkq_krp->krp_status = ERANGE; /* could do better */
		crypto_kdone(q->pkq_krp);
		kfree(q);
		spin_lock_irqsave(&ixp_pkq_lock, flags);
	}
	spin_unlock_irqrestore(&ixp_pkq_lock, flags);
}


static int
ixp_kprocess(device_t dev, struct cryptkop *krp, int hint)
{
	struct ixp_pkq *q;
	int rc = 0;
	unsigned long flags;

	dprintk("%s l1=%d l2=%d l3=%d l4=%d\n", __FUNCTION__,
			krp->krp_param[IXP_PARAM_BASE].crp_nbits,
			krp->krp_param[IXP_PARAM_EXP].crp_nbits,
			krp->krp_param[IXP_PARAM_MOD].crp_nbits,
			krp->krp_param[IXP_PARAM_RES].crp_nbits);


	if (krp->krp_op != CRK_MOD_EXP) {
		krp->krp_status = EOPNOTSUPP;
		goto err;
	}

	q = (struct ixp_pkq *) kmalloc(sizeof(*q), GFP_KERNEL);
	if (q == NULL) {
		krp->krp_status = ENOMEM;
		goto err;
	}

	/*
	 * The PKE engine does not appear to zero the output buffer
	 * appropriately, so we need to do it all here.
	 */
	memset(q, 0, sizeof(*q));

	q->pkq_krp = krp;
	INIT_LIST_HEAD(&q->pkq_list);

	if (ixp_copy_ibuf(&krp->krp_param[IXP_PARAM_BASE], &q->pkq_op.modExpOpr.M,
			q->pkq_ibuf0))
		rc = 1;
	if (!rc && ixp_copy_ibuf(&krp->krp_param[IXP_PARAM_EXP],
				&q->pkq_op.modExpOpr.e, q->pkq_ibuf1))
		rc = 2;
	if (!rc && ixp_copy_ibuf(&krp->krp_param[IXP_PARAM_MOD],
				&q->pkq_op.modExpOpr.N, q->pkq_ibuf2))
		rc = 3;

	if (rc) {
		kfree(q);
		krp->krp_status = ERANGE;
		goto err;
	}

	q->pkq_result.pData           = q->pkq_obuf;
	q->pkq_result.dataLen         =
			(krp->krp_param[IXP_PARAM_RES].crp_nbits + 31) / 32;

	spin_lock_irqsave(&ixp_pkq_lock, flags);
	list_add_tail(&q->pkq_list, &ixp_pkq);
	spin_unlock_irqrestore(&ixp_pkq_lock, flags);

	if (!ixp_pk_cur)
		ixp_kperform_cb(0, NULL, 0, 0);
	return (0);

err:
	crypto_kdone(krp);
	return (0);
}



#ifdef CONFIG_OCF_RANDOMHARVEST
/*
 * We run the random number generator output through SHA so that it
 * is FIPS compliant.
 */

static volatile int sha_done = 0;
static unsigned char sha_digest[20];

static void
ixp_hash_cb(UINT8 *digest, IxCryptoAccStatus status)
{
	dprintk("%s(%p, %d)\n", __FUNCTION__, digest, status);
	if (sha_digest != digest)
		printk("digest error\n");
	if (IX_CRYPTO_ACC_STATUS_SUCCESS == status)
		sha_done = 1;
	else
		sha_done = -status;
}

static int
ixp_read_random(void *arg, u_int32_t *buf, int maxwords)
{
	IxCryptoAccStatus status;
	int i, n, rc;

	dprintk("%s(%p, %d)\n", __FUNCTION__, buf, maxwords);
	memset(buf, 0, maxwords * sizeof(*buf));
	status = ixCryptoAccPkePseudoRandomNumberGet(maxwords, buf);
	if (status != IX_CRYPTO_ACC_STATUS_SUCCESS) {
		dprintk("%s: ixCryptoAccPkePseudoRandomNumberGet failed %d\n",
				__FUNCTION__, status);
		return 0;
	}

	/*
	 * run the random data through SHA to make it look more random
	 */

	n = sizeof(sha_digest); /* process digest bytes at a time */

	rc = 0;
	for (i = 0; i < maxwords; i += n / sizeof(*buf)) {
		if ((maxwords - i) * sizeof(*buf) < n)
			n = (maxwords - i) * sizeof(*buf);
		sha_done = 0;
		status = ixCryptoAccPkeHashPerform(IX_CRYPTO_ACC_AUTH_SHA1,
				(UINT8 *) &buf[i], n, ixp_hash_cb, sha_digest);
		if (status != IX_CRYPTO_ACC_STATUS_SUCCESS) {
			dprintk("ixCryptoAccPkeHashPerform failed %d\n", status);
			return -EIO;
		}
		while (!sha_done)
			schedule();
		if (sha_done < 0) {
			dprintk("ixCryptoAccPkeHashPerform failed CB %d\n", -sha_done);
			return 0;
		}
		memcpy(&buf[i], sha_digest, n);
		rc += n / sizeof(*buf);;
	}

	return rc;
}
#endif /* CONFIG_OCF_RANDOMHARVEST */

#endif /* __ixp46X */



/*
 * our driver startup and shutdown routines
 */

static int
ixp_init(void)
{
	dprintk("%s(%p)\n", __FUNCTION__, ixp_init);

	if (ixp_init_crypto && ixCryptoAccInit() != IX_CRYPTO_ACC_STATUS_SUCCESS)
		printk("ixCryptoAccInit failed, assuming already initialised!\n");

	qcache = kmem_cache_create("ixp4xx_q", sizeof(struct ixp_q), 0,
				SLAB_HWCACHE_ALIGN, NULL
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
				, NULL
#endif
				  );
	if (!qcache) {
		printk("failed to create Qcache\n");
		return -ENOENT;
	}

	memset(&ixpdev, 0, sizeof(ixpdev));
	softc_device_init(&ixpdev, "ixp4xx", 0, ixp_methods);

	ixp_id = crypto_get_driverid(softc_get_device(&ixpdev),
				CRYPTOCAP_F_HARDWARE);
	if (ixp_id < 0)
		panic("IXP/OCF crypto device cannot initialize!");

#define	REGISTER(alg) \
	crypto_register(ixp_id,alg,0,0)

	REGISTER(CRYPTO_DES_CBC);
	REGISTER(CRYPTO_3DES_CBC);
	REGISTER(CRYPTO_RIJNDAEL128_CBC);
#ifdef CONFIG_OCF_IXP4XX_SHA1_MD5
	REGISTER(CRYPTO_MD5);
	REGISTER(CRYPTO_SHA1);
#endif
	REGISTER(CRYPTO_MD5_HMAC);
	REGISTER(CRYPTO_SHA1_HMAC);
#undef REGISTER

#ifdef __ixp46X
	spin_lock_init(&ixp_pkq_lock);
	/*
	 * we do not enable the go fast options here as they can potentially
	 * allow timing based attacks
	 *
	 * http://www.openssl.org/news/secadv_20030219.txt
	 */
	ixCryptoAccPkeEauExpConfig(0, 0);
	crypto_kregister(ixp_id, CRK_MOD_EXP, 0);
#ifdef CONFIG_OCF_RANDOMHARVEST
	crypto_rregister(ixp_id, ixp_read_random, NULL);
#endif
#endif

	return 0;
}

static void
ixp_exit(void)
{
	dprintk("%s()\n", __FUNCTION__);
	crypto_unregister_all(ixp_id);
	ixp_id = -1;
	kmem_cache_destroy(qcache);
	qcache = NULL;
}

module_init(ixp_init);
module_exit(ixp_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("David McCullough <dmccullough@cyberguard.com>");
MODULE_DESCRIPTION("ixp (OCF module for IXP4xx crypto)");
