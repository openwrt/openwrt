/*
 * A loadable module that benchmarks the OCF crypto speed from kernel space.
 *
 * Copyright (C) 2004-2010 David McCullough <david_mccullough@mcafee.com>
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


#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) && !defined(AUTOCONF_INCLUDED)
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <cryptodev.h>

#ifdef I_HAVE_AN_XSCALE_WITH_INTEL_SDK
#define BENCH_IXP_ACCESS_LIB 1
#endif
#ifdef BENCH_IXP_ACCESS_LIB
#include <IxTypes.h>
#include <IxOsBuffMgt.h>
#include <IxNpeDl.h>
#include <IxCryptoAcc.h>
#include <IxQMgr.h>
#include <IxOsServices.h>
#include <IxOsCacheMMU.h>
#endif

/*
 * support for access lib version 1.4
 */
#ifndef IX_MBUF_PRIV
#define IX_MBUF_PRIV(x) ((x)->priv)
#endif

/*
 * the number of simultaneously active requests
 */
static int request_q_len = 40;
module_param(request_q_len, int, 0);
MODULE_PARM_DESC(request_q_len, "Number of outstanding requests");

/*
 * how many requests we want to have processed
 */
static int request_num = 1024;
module_param(request_num, int, 0);
MODULE_PARM_DESC(request_num, "run for at least this many requests");

/*
 * the size of each request
 */
static int request_size = 1488;
module_param(request_size, int, 0);
MODULE_PARM_DESC(request_size, "size of each request");

/*
 * OCF batching of requests
 */
static int request_batch = 1;
module_param(request_batch, int, 0);
MODULE_PARM_DESC(request_batch, "enable OCF request batching");

/*
 * OCF immediate callback on completion
 */
static int request_cbimm = 1;
module_param(request_cbimm, int, 0);
MODULE_PARM_DESC(request_cbimm, "enable OCF immediate callback on completion");

/*
 * a structure for each request
 */
typedef struct  {
	struct work_struct work;
#ifdef BENCH_IXP_ACCESS_LIB
	IX_MBUF mbuf;
#endif
	unsigned char *buffer;
} request_t;

static request_t *requests;

static spinlock_t ocfbench_counter_lock;
static int outstanding;
static int total;

/*************************************************************************/
/*
 * OCF benchmark routines
 */

static uint64_t ocf_cryptoid;
static unsigned long jstart, jstop;

static int ocf_init(void);
static int ocf_cb(struct cryptop *crp);
static void ocf_request(void *arg);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void ocf_request_wq(struct work_struct *work);
#endif

static int
ocf_init(void)
{
	int error;
	struct cryptoini crie, cria;
	struct cryptodesc crda, crde;

	memset(&crie, 0, sizeof(crie));
	memset(&cria, 0, sizeof(cria));
	memset(&crde, 0, sizeof(crde));
	memset(&crda, 0, sizeof(crda));

	cria.cri_alg  = CRYPTO_SHA1_HMAC;
	cria.cri_klen = 20 * 8;
	cria.cri_key  = "0123456789abcdefghij";

	//crie.cri_alg  = CRYPTO_3DES_CBC;
	crie.cri_alg  = CRYPTO_AES_CBC;
	crie.cri_klen = 24 * 8;
	crie.cri_key  = "0123456789abcdefghijklmn";

	crie.cri_next = &cria;

	error = crypto_newsession(&ocf_cryptoid, &crie,
				CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SOFTWARE);
	if (error) {
		printk("crypto_newsession failed %d\n", error);
		return -1;
	}
	return 0;
}

static int
ocf_cb(struct cryptop *crp)
{
	request_t *r = (request_t *) crp->crp_opaque;
	unsigned long flags;

	if (crp->crp_etype)
		printk("Error in OCF processing: %d\n", crp->crp_etype);
	crypto_freereq(crp);
	crp = NULL;

	/* do all requests  but take at least 1 second */
	spin_lock_irqsave(&ocfbench_counter_lock, flags);
	total++;
	if (total > request_num && jstart + HZ < jiffies) {
		outstanding--;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		return 0;
	}
	spin_unlock_irqrestore(&ocfbench_counter_lock, flags);

	schedule_work(&r->work);
	return 0;
}


static void
ocf_request(void *arg)
{
	request_t *r = arg;
	struct cryptop *crp = crypto_getreq(2);
	struct cryptodesc *crde, *crda;
	unsigned long flags;

	if (!crp) {
		spin_lock_irqsave(&ocfbench_counter_lock, flags);
		outstanding--;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		return;
	}

	crde = crp->crp_desc;
	crda = crde->crd_next;

	crda->crd_skip = 0;
	crda->crd_flags = 0;
	crda->crd_len = request_size;
	crda->crd_inject = request_size;
	crda->crd_alg = CRYPTO_SHA1_HMAC;
	crda->crd_key = "0123456789abcdefghij";
	crda->crd_klen = 20 * 8;

	crde->crd_skip = 0;
	crde->crd_flags = CRD_F_IV_EXPLICIT | CRD_F_ENCRYPT;
	crde->crd_len = request_size;
	crde->crd_inject = request_size;
	//crde->crd_alg = CRYPTO_3DES_CBC;
	crde->crd_alg = CRYPTO_AES_CBC;
	crde->crd_key = "0123456789abcdefghijklmn";
	crde->crd_klen = 24 * 8;

	crp->crp_ilen = request_size + 64;
	crp->crp_flags = 0;
	if (request_batch)
		crp->crp_flags |= CRYPTO_F_BATCH;
	if (request_cbimm)
		crp->crp_flags |= CRYPTO_F_CBIMM;
	crp->crp_buf = (caddr_t) r->buffer;
	crp->crp_callback = ocf_cb;
	crp->crp_sid = ocf_cryptoid;
	crp->crp_opaque = (caddr_t) r;
	crypto_dispatch(crp);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void
ocf_request_wq(struct work_struct *work)
{
	request_t *r = container_of(work, request_t, work);
	ocf_request(r);
}
#endif

static void
ocf_done(void)
{
	crypto_freesession(ocf_cryptoid);
}

/*************************************************************************/
#ifdef BENCH_IXP_ACCESS_LIB
/*************************************************************************/
/*
 * CryptoAcc benchmark routines
 */

static IxCryptoAccCtx ixp_ctx;
static UINT32 ixp_ctx_id;
static IX_MBUF ixp_pri;
static IX_MBUF ixp_sec;
static int ixp_registered = 0;

static void ixp_register_cb(UINT32 ctx_id, IX_MBUF *bufp,
					IxCryptoAccStatus status);
static void ixp_perform_cb(UINT32 ctx_id, IX_MBUF *sbufp, IX_MBUF *dbufp,
					IxCryptoAccStatus status);
static void ixp_request(void *arg);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void ixp_request_wq(struct work_struct *work);
#endif

static int
ixp_init(void)
{
	IxCryptoAccStatus status;

	ixp_ctx.cipherCtx.cipherAlgo = IX_CRYPTO_ACC_CIPHER_3DES;
	ixp_ctx.cipherCtx.cipherMode = IX_CRYPTO_ACC_MODE_CBC;
	ixp_ctx.cipherCtx.cipherKeyLen = 24;
	ixp_ctx.cipherCtx.cipherBlockLen = IX_CRYPTO_ACC_DES_BLOCK_64;
	ixp_ctx.cipherCtx.cipherInitialVectorLen = IX_CRYPTO_ACC_DES_IV_64;
	memcpy(ixp_ctx.cipherCtx.key.cipherKey, "0123456789abcdefghijklmn", 24);

	ixp_ctx.authCtx.authAlgo = IX_CRYPTO_ACC_AUTH_SHA1;
	ixp_ctx.authCtx.authDigestLen = 12;
	ixp_ctx.authCtx.aadLen = 0;
	ixp_ctx.authCtx.authKeyLen = 20;
	memcpy(ixp_ctx.authCtx.key.authKey, "0123456789abcdefghij", 20);

	ixp_ctx.useDifferentSrcAndDestMbufs = 0;
	ixp_ctx.operation = IX_CRYPTO_ACC_OP_ENCRYPT_AUTH ;

	IX_MBUF_MLEN(&ixp_pri)  = IX_MBUF_PKT_LEN(&ixp_pri) = 128;
	IX_MBUF_MDATA(&ixp_pri) = (unsigned char *) kmalloc(128, SLAB_ATOMIC);
	IX_MBUF_MLEN(&ixp_sec)  = IX_MBUF_PKT_LEN(&ixp_sec) = 128;
	IX_MBUF_MDATA(&ixp_sec) = (unsigned char *) kmalloc(128, SLAB_ATOMIC);

	status = ixCryptoAccCtxRegister(&ixp_ctx, &ixp_pri, &ixp_sec,
			ixp_register_cb, ixp_perform_cb, &ixp_ctx_id);

	if (IX_CRYPTO_ACC_STATUS_SUCCESS == status) {
		while (!ixp_registered)
			schedule();
		return ixp_registered < 0 ? -1 : 0;
	}

	printk("ixp: ixCryptoAccCtxRegister failed %d\n", status);
	return -1;
}

static void
ixp_register_cb(UINT32 ctx_id, IX_MBUF *bufp, IxCryptoAccStatus status)
{
	if (bufp) {
		IX_MBUF_MLEN(bufp) = IX_MBUF_PKT_LEN(bufp) = 0;
		kfree(IX_MBUF_MDATA(bufp));
		IX_MBUF_MDATA(bufp) = NULL;
	}

	if (IX_CRYPTO_ACC_STATUS_WAIT == status)
		return;
	if (IX_CRYPTO_ACC_STATUS_SUCCESS == status)
		ixp_registered = 1;
	else
		ixp_registered = -1;
}

static void
ixp_perform_cb(
	UINT32 ctx_id,
	IX_MBUF *sbufp,
	IX_MBUF *dbufp,
	IxCryptoAccStatus status)
{
	request_t *r = NULL;
	unsigned long flags;

	/* do all requests  but take at least 1 second */
	spin_lock_irqsave(&ocfbench_counter_lock, flags);
	total++;
	if (total > request_num && jstart + HZ < jiffies) {
		outstanding--;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		return;
	}

	if (!sbufp || !(r = IX_MBUF_PRIV(sbufp))) {
		printk("crappo %p %p\n", sbufp, r);
		outstanding--;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		return;
	}
	spin_unlock_irqrestore(&ocfbench_counter_lock, flags);

	schedule_work(&r->work);
}

static void
ixp_request(void *arg)
{
	request_t *r = arg;
	IxCryptoAccStatus status;
	unsigned long flags;

	memset(&r->mbuf, 0, sizeof(r->mbuf));
	IX_MBUF_MLEN(&r->mbuf) = IX_MBUF_PKT_LEN(&r->mbuf) = request_size + 64;
	IX_MBUF_MDATA(&r->mbuf) = r->buffer;
	IX_MBUF_PRIV(&r->mbuf) = r;
	status = ixCryptoAccAuthCryptPerform(ixp_ctx_id, &r->mbuf, NULL,
			0, request_size, 0, request_size, request_size, r->buffer);
	if (IX_CRYPTO_ACC_STATUS_SUCCESS != status) {
		printk("status1 = %d\n", status);
		spin_lock_irqsave(&ocfbench_counter_lock, flags);
		outstanding--;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		return;
	}
	return;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void
ixp_request_wq(struct work_struct *work)
{
	request_t *r = container_of(work, request_t, work);
	ixp_request(r);
}
#endif

static void
ixp_done(void)
{
	/* we should free the session here but I am lazy :-) */
}

/*************************************************************************/
#endif /* BENCH_IXP_ACCESS_LIB */
/*************************************************************************/

int
ocfbench_init(void)
{
	int i;
	unsigned long mbps;
	unsigned long flags;

	printk("Crypto Speed tests\n");

	requests = kmalloc(sizeof(request_t) * request_q_len, GFP_KERNEL);
	if (!requests) {
		printk("malloc failed\n");
		return -EINVAL;
	}

	for (i = 0; i < request_q_len; i++) {
		/* +64 for return data */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		INIT_WORK(&requests[i].work, ocf_request_wq);
#else
		INIT_WORK(&requests[i].work, ocf_request, &requests[i]);
#endif
		requests[i].buffer = kmalloc(request_size + 128, GFP_DMA);
		if (!requests[i].buffer) {
			printk("malloc failed\n");
			return -EINVAL;
		}
		memset(requests[i].buffer, '0' + i, request_size + 128);
	}

	/*
	 * OCF benchmark
	 */
	printk("OCF: testing ...\n");
	if (ocf_init() == -1)
		return -EINVAL;

	spin_lock_init(&ocfbench_counter_lock);
	total = outstanding = 0;
	jstart = jiffies;
	for (i = 0; i < request_q_len; i++) {
		spin_lock_irqsave(&ocfbench_counter_lock, flags);
		outstanding++;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		ocf_request(&requests[i]);
	}
	while (outstanding > 0)
		schedule();
	jstop = jiffies;

	mbps = 0;
	if (jstop > jstart) {
		mbps = (unsigned long) total * (unsigned long) request_size * 8;
		mbps /= ((jstop - jstart) * 1000) / HZ;
	}
	printk("OCF: %d requests of %d bytes in %d jiffies (%d.%03d Mbps)\n",
			total, request_size, (int)(jstop - jstart),
			((int)mbps) / 1000, ((int)mbps) % 1000);
	ocf_done();

#ifdef BENCH_IXP_ACCESS_LIB
	/*
	 * IXP benchmark
	 */
	printk("IXP: testing ...\n");
	ixp_init();
	total = outstanding = 0;
	jstart = jiffies;
	for (i = 0; i < request_q_len; i++) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		INIT_WORK(&requests[i].work, ixp_request_wq);
#else
		INIT_WORK(&requests[i].work, ixp_request, &requests[i]);
#endif
		spin_lock_irqsave(&ocfbench_counter_lock, flags);
		outstanding++;
		spin_unlock_irqrestore(&ocfbench_counter_lock, flags);
		ixp_request(&requests[i]);
	}
	while (outstanding > 0)
		schedule();
	jstop = jiffies;

	mbps = 0;
	if (jstop > jstart) {
		mbps = (unsigned long) total * (unsigned long) request_size * 8;
		mbps /= ((jstop - jstart) * 1000) / HZ;
	}
	printk("IXP: %d requests of %d bytes in %d jiffies (%d.%03d Mbps)\n",
			total, request_size, jstop - jstart,
			((int)mbps) / 1000, ((int)mbps) % 1000);
	ixp_done();
#endif /* BENCH_IXP_ACCESS_LIB */

	for (i = 0; i < request_q_len; i++)
		kfree(requests[i].buffer);
	kfree(requests);
	return -EINVAL; /* always fail to load so it can be re-run quickly ;-) */
}

static void __exit ocfbench_exit(void)
{
}

module_init(ocfbench_init);
module_exit(ocfbench_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("Benchmark various in-kernel crypto speeds");
