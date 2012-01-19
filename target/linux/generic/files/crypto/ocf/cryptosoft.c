/*
 * An OCF module that uses the linux kernel cryptoapi, based on the
 * original cryptosoft for BSD by Angelos D. Keromytis (angelos@cis.upenn.edu)
 * but is mostly unrecognisable,
 *
 * Written by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2004-2011 David McCullough
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
 * ---------------------------------------------------------------------------
 */

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) && !defined(AUTOCONF_INCLUDED)
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/crypto.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/random.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10)
#include <linux/scatterlist.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
#include <crypto/hash.h>
#endif

#include <cryptodev.h>
#include <uio.h>

struct {
	softc_device_decl	sc_dev;
} swcr_softc;

#define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)

#define SW_TYPE_CIPHER		0x01
#define SW_TYPE_HMAC		0x02
#define SW_TYPE_HASH		0x04
#define SW_TYPE_COMP		0x08
#define SW_TYPE_BLKCIPHER	0x10
#define SW_TYPE_ALG_MASK	0x1f

#define SW_TYPE_ASYNC		0x8000

#define SW_TYPE_INUSE		0x10000000

/* We change some of the above if we have an async interface */

#define SW_TYPE_ALG_AMASK	(SW_TYPE_ALG_MASK | SW_TYPE_ASYNC)

#define SW_TYPE_ABLKCIPHER	(SW_TYPE_BLKCIPHER | SW_TYPE_ASYNC)
#define SW_TYPE_AHASH		(SW_TYPE_HASH | SW_TYPE_ASYNC)
#define SW_TYPE_AHMAC		(SW_TYPE_HMAC | SW_TYPE_ASYNC)

#define SCATTERLIST_MAX 16

struct swcr_data {
	struct work_struct  workq;
	int					sw_type;
	int					sw_alg;
	struct crypto_tfm	*sw_tfm;
	spinlock_t			sw_tfm_lock;
	union {
		struct {
			char *sw_key;
			int  sw_klen;
			int  sw_mlen;
		} hmac;
		void *sw_comp_buf;
	} u;
	struct swcr_data	*sw_next;
};

struct swcr_req {
	struct swcr_data	*sw_head;
	struct swcr_data	*sw;
	struct cryptop		*crp;
	struct cryptodesc	*crd;
	struct scatterlist	 sg[SCATTERLIST_MAX];
	unsigned char		 iv[EALG_MAX_BLOCK_LEN];
	char				 result[HASH_MAX_LEN];
	void				*crypto_req;
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
static kmem_cache_t *swcr_req_cache;
#else
static struct kmem_cache *swcr_req_cache;
#endif

#ifndef CRYPTO_TFM_MODE_CBC
/*
 * As of linux-2.6.21 this is no longer defined, and presumably no longer
 * needed to be passed into the crypto core code.
 */
#define	CRYPTO_TFM_MODE_CBC	0
#define	CRYPTO_TFM_MODE_ECB	0
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
	/*
	 * Linux 2.6.19 introduced a new Crypto API, setup macro's to convert new
	 * API into old API.
	 */

	/* Symmetric/Block Cipher */
	struct blkcipher_desc
	{
		struct crypto_tfm *tfm;
		void *info;
	};
	#define ecb(X)								#X , CRYPTO_TFM_MODE_ECB
	#define cbc(X)								#X , CRYPTO_TFM_MODE_CBC
	#define crypto_has_blkcipher(X, Y, Z)		crypto_alg_available(X, 0)
	#define crypto_blkcipher_cast(X)			X
	#define crypto_blkcipher_tfm(X)				X
	#define crypto_alloc_blkcipher(X, Y, Z)		crypto_alloc_tfm(X, mode)
	#define crypto_blkcipher_ivsize(X)			crypto_tfm_alg_ivsize(X)
	#define crypto_blkcipher_blocksize(X)		crypto_tfm_alg_blocksize(X)
	#define crypto_blkcipher_setkey(X, Y, Z)	crypto_cipher_setkey(X, Y, Z)
	#define crypto_blkcipher_encrypt_iv(W, X, Y, Z)	\
				crypto_cipher_encrypt_iv((W)->tfm, X, Y, Z, (u8 *)((W)->info))
	#define crypto_blkcipher_decrypt_iv(W, X, Y, Z)	\
				crypto_cipher_decrypt_iv((W)->tfm, X, Y, Z, (u8 *)((W)->info))
	#define crypto_blkcipher_set_flags(x, y)	/* nop */
	#define crypto_free_blkcipher(x)			crypto_free_tfm(x)
	#define crypto_free_comp					crypto_free_tfm
	#define crypto_free_hash					crypto_free_tfm

	/* Hash/HMAC/Digest */
	struct hash_desc
	{
		struct crypto_tfm *tfm;
	};
	#define hmac(X)							#X , 0
	#define crypto_has_hash(X, Y, Z)		crypto_alg_available(X, 0)
	#define crypto_hash_cast(X)				X
	#define crypto_hash_tfm(X)				X
	#define crypto_alloc_hash(X, Y, Z)		crypto_alloc_tfm(X, mode)
	#define crypto_hash_digestsize(X)		crypto_tfm_alg_digestsize(X)
	#define crypto_hash_digest(W, X, Y, Z)	\
				crypto_digest_digest((W)->tfm, X, sg_num, Z)

	/* Asymmetric Cipher */
	#define crypto_has_cipher(X, Y, Z)		crypto_alg_available(X, 0)

	/* Compression */
	#define crypto_has_comp(X, Y, Z)		crypto_alg_available(X, 0)
	#define crypto_comp_tfm(X)				X
	#define crypto_comp_cast(X)				X
	#define crypto_alloc_comp(X, Y, Z)		crypto_alloc_tfm(X, mode)
	#define plain(X)	#X , 0
#else
	#define ecb(X)	"ecb(" #X ")" , 0
	#define cbc(X)	"cbc(" #X ")" , 0
	#define hmac(X)	"hmac(" #X ")" , 0
	#define plain(X)	#X , 0
#endif /* if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
/* no ablkcipher in older kernels */
#define crypto_alloc_ablkcipher(a,b,c)		(NULL)
#define crypto_ablkcipher_tfm(x)			((struct crypto_tfm *)(x))
#define crypto_ablkcipher_set_flags(a, b)	/* nop */
#define crypto_ablkcipher_setkey(x, y, z)	(-EINVAL)
#define	crypto_has_ablkcipher(a,b,c)		(0)
#else
#define	HAVE_ABLKCIPHER
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
/* no ahash in older kernels */
#define crypto_ahash_tfm(x)					((struct crypto_tfm *)(x))
#define	crypto_alloc_ahash(a,b,c)			(NULL)
#define	crypto_ahash_digestsize(x)			0
#else
#define	HAVE_AHASH
#endif

struct crypto_details {
	char *alg_name;
	int mode;
	int sw_type;
};

static struct crypto_details crypto_details[] = {
	[CRYPTO_DES_CBC]         = { cbc(des),          SW_TYPE_BLKCIPHER, },
	[CRYPTO_3DES_CBC]        = { cbc(des3_ede),     SW_TYPE_BLKCIPHER, },
	[CRYPTO_BLF_CBC]         = { cbc(blowfish),     SW_TYPE_BLKCIPHER, },
	[CRYPTO_CAST_CBC]        = { cbc(cast5),        SW_TYPE_BLKCIPHER, },
	[CRYPTO_SKIPJACK_CBC]    = { cbc(skipjack),     SW_TYPE_BLKCIPHER, },
	[CRYPTO_MD5_HMAC]        = { hmac(md5),         SW_TYPE_HMAC, },
	[CRYPTO_SHA1_HMAC]       = { hmac(sha1),        SW_TYPE_HMAC, },
	[CRYPTO_RIPEMD160_HMAC]  = { hmac(ripemd160),   SW_TYPE_HMAC, },
	[CRYPTO_MD5_KPDK]        = { plain(md5-kpdk),   SW_TYPE_HASH, },
	[CRYPTO_SHA1_KPDK]       = { plain(sha1-kpdk),  SW_TYPE_HASH, },
	[CRYPTO_AES_CBC]         = { cbc(aes),          SW_TYPE_BLKCIPHER, },
	[CRYPTO_ARC4]            = { ecb(arc4),         SW_TYPE_BLKCIPHER, },
	[CRYPTO_MD5]             = { plain(md5),        SW_TYPE_HASH, },
	[CRYPTO_SHA1]            = { plain(sha1),       SW_TYPE_HASH, },
	[CRYPTO_NULL_HMAC]       = { hmac(digest_null), SW_TYPE_HMAC, },
	[CRYPTO_NULL_CBC]        = { cbc(cipher_null),  SW_TYPE_BLKCIPHER, },
	[CRYPTO_DEFLATE_COMP]    = { plain(deflate),    SW_TYPE_COMP, },
	[CRYPTO_SHA2_256_HMAC]   = { hmac(sha256),      SW_TYPE_HMAC, },
	[CRYPTO_SHA2_384_HMAC]   = { hmac(sha384),      SW_TYPE_HMAC, },
	[CRYPTO_SHA2_512_HMAC]   = { hmac(sha512),      SW_TYPE_HMAC, },
	[CRYPTO_CAMELLIA_CBC]    = { cbc(camellia),     SW_TYPE_BLKCIPHER, },
	[CRYPTO_SHA2_256]        = { plain(sha256),     SW_TYPE_HASH, },
	[CRYPTO_SHA2_384]        = { plain(sha384),     SW_TYPE_HASH, },
	[CRYPTO_SHA2_512]        = { plain(sha512),     SW_TYPE_HASH, },
	[CRYPTO_RIPEMD160]       = { plain(ripemd160),  SW_TYPE_HASH, },
};

int32_t swcr_id = -1;
module_param(swcr_id, int, 0444);
MODULE_PARM_DESC(swcr_id, "Read-Only OCF ID for cryptosoft driver");

int swcr_fail_if_compression_grows = 1;
module_param(swcr_fail_if_compression_grows, int, 0644);
MODULE_PARM_DESC(swcr_fail_if_compression_grows,
                "Treat compression that results in more data as a failure");

int swcr_no_ahash = 0;
module_param(swcr_no_ahash, int, 0644);
MODULE_PARM_DESC(swcr_no_ahash,
                "Do not use async hash/hmac even if available");

int swcr_no_ablk = 0;
module_param(swcr_no_ablk, int, 0644);
MODULE_PARM_DESC(swcr_no_ablk,
                "Do not use async blk ciphers even if available");

static struct swcr_data **swcr_sessions = NULL;
static u_int32_t swcr_sesnum = 0;

static	int swcr_process(device_t, struct cryptop *, int);
static	int swcr_newsession(device_t, u_int32_t *, struct cryptoini *);
static	int swcr_freesession(device_t, u_int64_t);

static device_method_t swcr_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	swcr_newsession),
	DEVMETHOD(cryptodev_freesession,swcr_freesession),
	DEVMETHOD(cryptodev_process,	swcr_process),
};

#define debug swcr_debug
int swcr_debug = 0;
module_param(swcr_debug, int, 0644);
MODULE_PARM_DESC(swcr_debug, "Enable debug");

static void swcr_process_req(struct swcr_req *req);

/*
 * somethings just need to be run with user context no matter whether
 * the kernel compression libs use vmalloc/vfree for example.
 */

typedef struct {
	struct work_struct wq;
	void	(*func)(void *arg);
	void	*arg;
} execute_later_t;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void
doing_it_now(struct work_struct *wq)
{
	execute_later_t *w = container_of(wq, execute_later_t, wq);
	(w->func)(w->arg);
	kfree(w);
}
#else
static void
doing_it_now(void *arg)
{
	execute_later_t *w = (execute_later_t *) arg;
	(w->func)(w->arg);
	kfree(w);
}
#endif

static void
execute_later(void (fn)(void *), void *arg)
{
	execute_later_t *w;

	w = (execute_later_t *) kmalloc(sizeof(execute_later_t), SLAB_ATOMIC);
	if (w) {
		memset(w, '\0', sizeof(w));
		w->func = fn;
		w->arg = arg;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		INIT_WORK(&w->wq, doing_it_now);
#else
		INIT_WORK(&w->wq, doing_it_now, w);
#endif
		schedule_work(&w->wq);
	}
}

/*
 * Generate a new software session.
 */
static int
swcr_newsession(device_t dev, u_int32_t *sid, struct cryptoini *cri)
{
	struct swcr_data **swd;
	u_int32_t i;
	int error;
	char *algo;
	int mode;

	dprintk("%s()\n", __FUNCTION__);
	if (sid == NULL || cri == NULL) {
		dprintk("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	if (swcr_sessions) {
		for (i = 1; i < swcr_sesnum; i++)
			if (swcr_sessions[i] == NULL)
				break;
	} else
		i = 1;		/* NB: to silence compiler warning */

	if (swcr_sessions == NULL || i == swcr_sesnum) {
		if (swcr_sessions == NULL) {
			i = 1; /* We leave swcr_sessions[0] empty */
			swcr_sesnum = CRYPTO_SW_SESSIONS;
		} else
			swcr_sesnum *= 2;

		swd = kmalloc(swcr_sesnum * sizeof(struct swcr_data *), SLAB_ATOMIC);
		if (swd == NULL) {
			/* Reset session number */
			if (swcr_sesnum == CRYPTO_SW_SESSIONS)
				swcr_sesnum = 0;
			else
				swcr_sesnum /= 2;
			dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
			return ENOBUFS;
		}
		memset(swd, 0, swcr_sesnum * sizeof(struct swcr_data *));

		/* Copy existing sessions */
		if (swcr_sessions) {
			memcpy(swd, swcr_sessions,
			    (swcr_sesnum / 2) * sizeof(struct swcr_data *));
			kfree(swcr_sessions);
		}

		swcr_sessions = swd;
	}

	swd = &swcr_sessions[i];
	*sid = i;

	while (cri) {
		*swd = (struct swcr_data *) kmalloc(sizeof(struct swcr_data),
				SLAB_ATOMIC);
		if (*swd == NULL) {
			swcr_freesession(NULL, i);
			dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
			return ENOBUFS;
		}
		memset(*swd, 0, sizeof(struct swcr_data));

		if (cri->cri_alg < 0 ||
				cri->cri_alg>=sizeof(crypto_details)/sizeof(crypto_details[0])){
			printk("cryptosoft: Unknown algorithm 0x%x\n", cri->cri_alg);
			swcr_freesession(NULL, i);
			return EINVAL;
		}

		algo = crypto_details[cri->cri_alg].alg_name;
		if (!algo || !*algo) {
			printk("cryptosoft: Unsupported algorithm 0x%x\n", cri->cri_alg);
			swcr_freesession(NULL, i);
			return EINVAL;
		}

		mode = crypto_details[cri->cri_alg].mode;
		(*swd)->sw_type = crypto_details[cri->cri_alg].sw_type;
		(*swd)->sw_alg = cri->cri_alg;

		spin_lock_init(&(*swd)->sw_tfm_lock);

		/* Algorithm specific configuration */
		switch (cri->cri_alg) {
		case CRYPTO_NULL_CBC:
			cri->cri_klen = 0; /* make it work with crypto API */
			break;
		default:
			break;
		}

		if ((*swd)->sw_type & SW_TYPE_BLKCIPHER) {
			dprintk("%s crypto_alloc_*blkcipher(%s, 0x%x)\n", __FUNCTION__,
					algo, mode);

			/* try async first */
			(*swd)->sw_tfm = swcr_no_ablk ? NULL :
					crypto_ablkcipher_tfm(crypto_alloc_ablkcipher(algo, 0, 0));
			if ((*swd)->sw_tfm && !IS_ERR((*swd)->sw_tfm)) {
				dprintk("%s %s cipher is async\n", __FUNCTION__, algo);
				(*swd)->sw_type |= SW_TYPE_ASYNC;
			} else {
				(*swd)->sw_tfm = crypto_blkcipher_tfm(
						crypto_alloc_blkcipher(algo, 0, CRYPTO_ALG_ASYNC));
				if ((*swd)->sw_tfm && !IS_ERR((*swd)->sw_tfm))
					dprintk("%s %s cipher is sync\n", __FUNCTION__, algo);
			}
			if (!(*swd)->sw_tfm || IS_ERR((*swd)->sw_tfm)) {
				int err;
				dprintk("cryptosoft: crypto_alloc_blkcipher failed(%s, 0x%x)\n",
						algo,mode);
				err = IS_ERR((*swd)->sw_tfm) ? -(PTR_ERR((*swd)->sw_tfm)) : EINVAL;
				(*swd)->sw_tfm = NULL; /* ensure NULL */
				swcr_freesession(NULL, i);
				return err;
			}

			if (debug) {
				dprintk("%s key:cri->cri_klen=%d,(cri->cri_klen + 7)/8=%d",
						__FUNCTION__, cri->cri_klen, (cri->cri_klen + 7) / 8);
				for (i = 0; i < (cri->cri_klen + 7) / 8; i++)
					dprintk("%s0x%x", (i % 8) ? " " : "\n    ",
							cri->cri_key[i] & 0xff);
				dprintk("\n");
			}
			if ((*swd)->sw_type & SW_TYPE_ASYNC) {
				/* OCF doesn't enforce keys */
				crypto_ablkcipher_set_flags(
						__crypto_ablkcipher_cast((*swd)->sw_tfm),
							CRYPTO_TFM_REQ_WEAK_KEY);
				error = crypto_ablkcipher_setkey(
							__crypto_ablkcipher_cast((*swd)->sw_tfm),
								cri->cri_key, (cri->cri_klen + 7) / 8);
			} else {
				/* OCF doesn't enforce keys */
				crypto_blkcipher_set_flags(
						crypto_blkcipher_cast((*swd)->sw_tfm),
							CRYPTO_TFM_REQ_WEAK_KEY);
				error = crypto_blkcipher_setkey(
							crypto_blkcipher_cast((*swd)->sw_tfm),
								cri->cri_key, (cri->cri_klen + 7) / 8);
			}
			if (error) {
				printk("cryptosoft: setkey failed %d (crt_flags=0x%x)\n", error,
						(*swd)->sw_tfm->crt_flags);
				swcr_freesession(NULL, i);
				return error;
			}
		} else if ((*swd)->sw_type & (SW_TYPE_HMAC | SW_TYPE_HASH)) {
			dprintk("%s crypto_alloc_*hash(%s, 0x%x)\n", __FUNCTION__,
					algo, mode);

			/* try async first */
			(*swd)->sw_tfm = swcr_no_ahash ? NULL :
					crypto_ahash_tfm(crypto_alloc_ahash(algo, 0, 0));
			if ((*swd)->sw_tfm) {
				dprintk("%s %s hash is async\n", __FUNCTION__, algo);
				(*swd)->sw_type |= SW_TYPE_ASYNC;
			} else {
				dprintk("%s %s hash is sync\n", __FUNCTION__, algo);
				(*swd)->sw_tfm = crypto_hash_tfm(
						crypto_alloc_hash(algo, 0, CRYPTO_ALG_ASYNC));
			}

			if (!(*swd)->sw_tfm) {
				dprintk("cryptosoft: crypto_alloc_hash failed(%s,0x%x)\n",
						algo, mode);
				swcr_freesession(NULL, i);
				return EINVAL;
			}

			(*swd)->u.hmac.sw_klen = (cri->cri_klen + 7) / 8;
			(*swd)->u.hmac.sw_key = (char *)kmalloc((*swd)->u.hmac.sw_klen,
					SLAB_ATOMIC);
			if ((*swd)->u.hmac.sw_key == NULL) {
				swcr_freesession(NULL, i);
				dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
				return ENOBUFS;
			}
			memcpy((*swd)->u.hmac.sw_key, cri->cri_key, (*swd)->u.hmac.sw_klen);
			if (cri->cri_mlen) {
				(*swd)->u.hmac.sw_mlen = cri->cri_mlen;
			} else if ((*swd)->sw_type & SW_TYPE_ASYNC) {
				(*swd)->u.hmac.sw_mlen = crypto_ahash_digestsize(
						__crypto_ahash_cast((*swd)->sw_tfm));
			} else  {
				(*swd)->u.hmac.sw_mlen = crypto_hash_digestsize(
						crypto_hash_cast((*swd)->sw_tfm));
			}
		} else if ((*swd)->sw_type & SW_TYPE_COMP) {
			(*swd)->sw_tfm = crypto_comp_tfm(
					crypto_alloc_comp(algo, 0, CRYPTO_ALG_ASYNC));
			if (!(*swd)->sw_tfm) {
				dprintk("cryptosoft: crypto_alloc_comp failed(%s,0x%x)\n",
						algo, mode);
				swcr_freesession(NULL, i);
				return EINVAL;
			}
			(*swd)->u.sw_comp_buf = kmalloc(CRYPTO_MAX_DATA_LEN, SLAB_ATOMIC);
			if ((*swd)->u.sw_comp_buf == NULL) {
				swcr_freesession(NULL, i);
				dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
				return ENOBUFS;
			}
		} else {
			printk("cryptosoft: Unhandled sw_type %d\n", (*swd)->sw_type);
			swcr_freesession(NULL, i);
			return EINVAL;
		}

		cri = cri->cri_next;
		swd = &((*swd)->sw_next);
	}
	return 0;
}

/*
 * Free a session.
 */
static int
swcr_freesession(device_t dev, u_int64_t tid)
{
	struct swcr_data *swd;
	u_int32_t sid = CRYPTO_SESID2LID(tid);

	dprintk("%s()\n", __FUNCTION__);
	if (sid > swcr_sesnum || swcr_sessions == NULL ||
			swcr_sessions[sid] == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return(EINVAL);
	}

	/* Silently accept and return */
	if (sid == 0)
		return(0);

	while ((swd = swcr_sessions[sid]) != NULL) {
		swcr_sessions[sid] = swd->sw_next;
		if (swd->sw_tfm) {
			switch (swd->sw_type & SW_TYPE_ALG_AMASK) {
#ifdef HAVE_AHASH
			case SW_TYPE_AHMAC:
			case SW_TYPE_AHASH:
				crypto_free_ahash(__crypto_ahash_cast(swd->sw_tfm));
				break;
#endif
#ifdef HAVE_ABLKCIPHER
			case SW_TYPE_ABLKCIPHER:
				crypto_free_ablkcipher(__crypto_ablkcipher_cast(swd->sw_tfm));
				break;
#endif
			case SW_TYPE_BLKCIPHER:
				crypto_free_blkcipher(crypto_blkcipher_cast(swd->sw_tfm));
				break;
			case SW_TYPE_HMAC:
			case SW_TYPE_HASH:
				crypto_free_hash(crypto_hash_cast(swd->sw_tfm));
				break;
			case SW_TYPE_COMP:
				if (in_interrupt())
					execute_later((void (*)(void *))crypto_free_comp, (void *)crypto_comp_cast(swd->sw_tfm));
				else
					crypto_free_comp(crypto_comp_cast(swd->sw_tfm));
				break;
			default:
				crypto_free_tfm(swd->sw_tfm);
				break;
			}
			swd->sw_tfm = NULL;
		}
		if (swd->sw_type & SW_TYPE_COMP) {
			if (swd->u.sw_comp_buf)
				kfree(swd->u.sw_comp_buf);
		} else {
			if (swd->u.hmac.sw_key)
				kfree(swd->u.hmac.sw_key);
		}
		kfree(swd);
	}
	return 0;
}

static void swcr_process_req_complete(struct swcr_req *req)
{
	dprintk("%s()\n", __FUNCTION__);

	if (req->sw->sw_type & SW_TYPE_INUSE) {
		unsigned long flags;
		spin_lock_irqsave(&req->sw->sw_tfm_lock, flags);
		req->sw->sw_type &= ~SW_TYPE_INUSE;
		spin_unlock_irqrestore(&req->sw->sw_tfm_lock, flags);
	}

	if (req->crp->crp_etype)
		goto done;

	switch (req->sw->sw_type & SW_TYPE_ALG_AMASK) {
#if defined(HAVE_AHASH)
	case SW_TYPE_AHMAC:
	case SW_TYPE_AHASH:
		crypto_copyback(req->crp->crp_flags, req->crp->crp_buf,
				req->crd->crd_inject, req->sw->u.hmac.sw_mlen, req->result);
		ahash_request_free(req->crypto_req);
		break;
#endif
#if defined(HAVE_ABLKCIPHER)
	case SW_TYPE_ABLKCIPHER:
		ablkcipher_request_free(req->crypto_req);
		break;
#endif
	case SW_TYPE_CIPHER:
	case SW_TYPE_HMAC:
	case SW_TYPE_HASH:
	case SW_TYPE_COMP:
	case SW_TYPE_BLKCIPHER:
		break;
	default:
		req->crp->crp_etype = EINVAL;
		goto done;
	}

	req->crd = req->crd->crd_next;
	if (req->crd) {
		swcr_process_req(req);
		return;
	}

done:
	dprintk("%s crypto_done %p\n", __FUNCTION__, req);
	crypto_done(req->crp);
	kmem_cache_free(swcr_req_cache, req);
}

#if defined(HAVE_ABLKCIPHER) || defined(HAVE_AHASH)
static void swcr_process_callback(struct crypto_async_request *creq, int err)
{
	struct swcr_req *req = creq->data;

	dprintk("%s()\n", __FUNCTION__);
	if (err) {
		if (err == -EINPROGRESS)
			return;
		dprintk("%s() fail %d\n", __FUNCTION__, -err);
		req->crp->crp_etype = -err;
	}

	swcr_process_req_complete(req);
}
#endif /* defined(HAVE_ABLKCIPHER) || defined(HAVE_AHASH) */


static void swcr_process_req(struct swcr_req *req)
{
	struct swcr_data *sw;
	struct cryptop *crp = req->crp;
	struct cryptodesc *crd = req->crd;
	struct sk_buff *skb = (struct sk_buff *) crp->crp_buf;
	struct uio *uiop = (struct uio *) crp->crp_buf;
	int sg_num, sg_len, skip;

	dprintk("%s()\n", __FUNCTION__);

	/*
	 * Find the crypto context.
	 *
	 * XXX Note that the logic here prevents us from having
	 * XXX the same algorithm multiple times in a session
	 * XXX (or rather, we can but it won't give us the right
	 * XXX results). To do that, we'd need some way of differentiating
	 * XXX between the various instances of an algorithm (so we can
	 * XXX locate the correct crypto context).
	 */
	for (sw = req->sw_head; sw && sw->sw_alg != crd->crd_alg; sw = sw->sw_next)
		;

	/* No such context ? */
	if (sw == NULL) {
		crp->crp_etype = EINVAL;
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		goto done;
	}

	/*
	 * for some types we need to ensure only one user as info is stored in
	 * the tfm during an operation that can get corrupted
	 */
	switch (sw->sw_type & SW_TYPE_ALG_AMASK) {
#ifdef HAVE_AHASH
	case SW_TYPE_AHMAC:
	case SW_TYPE_AHASH:
#endif
	case SW_TYPE_HMAC:
	case SW_TYPE_HASH: {
		unsigned long flags;
		spin_lock_irqsave(&sw->sw_tfm_lock, flags);
		if (sw->sw_type & SW_TYPE_INUSE) {
			spin_unlock_irqrestore(&sw->sw_tfm_lock, flags);
			execute_later((void (*)(void *))swcr_process_req, (void *)req);
			return;
		}
		sw->sw_type |= SW_TYPE_INUSE;
		spin_unlock_irqrestore(&sw->sw_tfm_lock, flags);
		} break;
	}

	req->sw = sw;
	skip = crd->crd_skip;

	/*
	 * setup the SG list skip from the start of the buffer
	 */
	memset(req->sg, 0, sizeof(req->sg));
	sg_init_table(req->sg, SCATTERLIST_MAX);
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		int i, len;

		sg_num = 0;
		sg_len = 0;

		if (skip < skb_headlen(skb)) {
			len = skb_headlen(skb) - skip;
			if (len + sg_len > crd->crd_len)
				len = crd->crd_len - sg_len;
			sg_set_page(&req->sg[sg_num],
				virt_to_page(skb->data + skip), len,
				offset_in_page(skb->data + skip));
			sg_len += len;
			sg_num++;
			skip = 0;
		} else
			skip -= skb_headlen(skb);

		for (i = 0; sg_len < crd->crd_len &&
					i < skb_shinfo(skb)->nr_frags &&
					sg_num < SCATTERLIST_MAX; i++) {
			if (skip < skb_shinfo(skb)->frags[i].size) {
				len = skb_shinfo(skb)->frags[i].size - skip;
				if (len + sg_len > crd->crd_len)
					len = crd->crd_len - sg_len;
				sg_set_page(&req->sg[sg_num],
					skb_frag_page(&skb_shinfo(skb)->frags[i]),
					len,
					skb_shinfo(skb)->frags[i].page_offset + skip);
				sg_len += len;
				sg_num++;
				skip = 0;
			} else
				skip -= skb_shinfo(skb)->frags[i].size;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		int len;

		sg_len = 0;
		for (sg_num = 0; sg_len < crd->crd_len &&
				sg_num < uiop->uio_iovcnt &&
				sg_num < SCATTERLIST_MAX; sg_num++) {
			if (skip <= uiop->uio_iov[sg_num].iov_len) {
				len = uiop->uio_iov[sg_num].iov_len - skip;
				if (len + sg_len > crd->crd_len)
					len = crd->crd_len - sg_len;
				sg_set_page(&req->sg[sg_num],
					virt_to_page(uiop->uio_iov[sg_num].iov_base+skip),
					len,
					offset_in_page(uiop->uio_iov[sg_num].iov_base+skip));
				sg_len += len;
				skip = 0;
			} else 
				skip -= uiop->uio_iov[sg_num].iov_len;
		}
	} else {
		sg_len = (crp->crp_ilen - skip);
		if (sg_len > crd->crd_len)
			sg_len = crd->crd_len;
		sg_set_page(&req->sg[0], virt_to_page(crp->crp_buf + skip),
			sg_len, offset_in_page(crp->crp_buf + skip));
		sg_num = 1;
	}

	switch (sw->sw_type & SW_TYPE_ALG_AMASK) {

#ifdef HAVE_AHASH
	case SW_TYPE_AHMAC:
	case SW_TYPE_AHASH:
		{
		int ret;

		/* check we have room for the result */
		if (crp->crp_ilen - crd->crd_inject < sw->u.hmac.sw_mlen) {
			dprintk("cryptosoft: EINVAL crp_ilen=%d, len=%d, inject=%d "
					"digestsize=%d\n", crp->crp_ilen, crd->crd_skip + sg_len,
					crd->crd_inject, sw->u.hmac.sw_mlen);
			crp->crp_etype = EINVAL;
			goto done;
		}

		req->crypto_req =
				ahash_request_alloc(__crypto_ahash_cast(sw->sw_tfm),GFP_ATOMIC);
		if (!req->crypto_req) {
			crp->crp_etype = ENOMEM;
			dprintk("%s,%d: ENOMEM ahash_request_alloc", __FILE__, __LINE__);
			goto done;
		}

		ahash_request_set_callback(req->crypto_req,
				CRYPTO_TFM_REQ_MAY_BACKLOG, swcr_process_callback, req);

		memset(req->result, 0, sizeof(req->result));

		if (sw->sw_type & SW_TYPE_AHMAC)
			crypto_ahash_setkey(__crypto_ahash_cast(sw->sw_tfm),
					sw->u.hmac.sw_key, sw->u.hmac.sw_klen);
		ahash_request_set_crypt(req->crypto_req, req->sg, req->result, sg_len);
		ret = crypto_ahash_digest(req->crypto_req);
		switch (ret) {
		case -EINPROGRESS:
		case -EBUSY:
			return;
		default:
		case 0:
			dprintk("hash OP %s %d\n", ret ? "failed" : "success", ret);
			crp->crp_etype = ret;
			goto done;
		}
		} break;
#endif /* HAVE_AHASH */

#ifdef HAVE_ABLKCIPHER
	case SW_TYPE_ABLKCIPHER: {
		int ret;
		unsigned char *ivp = req->iv;
		int ivsize = 
			crypto_ablkcipher_ivsize(__crypto_ablkcipher_cast(sw->sw_tfm));

		if (sg_len < crypto_ablkcipher_blocksize(
				__crypto_ablkcipher_cast(sw->sw_tfm))) {
			crp->crp_etype = EINVAL;
			dprintk("%s,%d: EINVAL len %d < %d\n", __FILE__, __LINE__,
					sg_len, crypto_ablkcipher_blocksize(
						__crypto_ablkcipher_cast(sw->sw_tfm)));
			goto done;
		}

		if (ivsize > sizeof(req->iv)) {
			crp->crp_etype = EINVAL;
			dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
			goto done;
		}

		req->crypto_req = ablkcipher_request_alloc(
				__crypto_ablkcipher_cast(sw->sw_tfm), GFP_ATOMIC);
		if (!req->crypto_req) {
			crp->crp_etype = ENOMEM;
			dprintk("%s,%d: ENOMEM ablkcipher_request_alloc",
					__FILE__, __LINE__);
			goto done;
		}

		ablkcipher_request_set_callback(req->crypto_req,
				CRYPTO_TFM_REQ_MAY_BACKLOG, swcr_process_callback, req);

		if (crd->crd_flags & CRD_F_KEY_EXPLICIT) {
			int i, error;

			if (debug) {
				dprintk("%s key:", __FUNCTION__);
				for (i = 0; i < (crd->crd_klen + 7) / 8; i++)
					dprintk("%s0x%x", (i % 8) ? " " : "\n    ",
							crd->crd_key[i] & 0xff);
				dprintk("\n");
			}
			/* OCF doesn't enforce keys */
			crypto_ablkcipher_set_flags(__crypto_ablkcipher_cast(sw->sw_tfm),
					CRYPTO_TFM_REQ_WEAK_KEY);
			error = crypto_ablkcipher_setkey(
						__crypto_ablkcipher_cast(sw->sw_tfm), crd->crd_key,
						(crd->crd_klen + 7) / 8);
			if (error) {
				dprintk("cryptosoft: setkey failed %d (crt_flags=0x%x)\n",
						error, sw->sw_tfm->crt_flags);
				crp->crp_etype = -error;
			}
		}

		if (crd->crd_flags & CRD_F_ENCRYPT) { /* encrypt */

			if (crd->crd_flags & CRD_F_IV_EXPLICIT)
				ivp = crd->crd_iv;
			else
				get_random_bytes(ivp, ivsize);
			/*
			 * do we have to copy the IV back to the buffer ?
			 */
			if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				crypto_copyback(crp->crp_flags, crp->crp_buf,
						crd->crd_inject, ivsize, (caddr_t)ivp);
			}
			ablkcipher_request_set_crypt(req->crypto_req, req->sg, req->sg,
					sg_len, ivp);
			ret = crypto_ablkcipher_encrypt(req->crypto_req);

		} else { /*decrypt */

			if (crd->crd_flags & CRD_F_IV_EXPLICIT)
				ivp = crd->crd_iv;
			else
				crypto_copydata(crp->crp_flags, crp->crp_buf,
						crd->crd_inject, ivsize, (caddr_t)ivp);
			ablkcipher_request_set_crypt(req->crypto_req, req->sg, req->sg,
					sg_len, ivp);
			ret = crypto_ablkcipher_decrypt(req->crypto_req);
		}

		switch (ret) {
		case -EINPROGRESS:
		case -EBUSY:
			return;
		default:
		case 0:
			dprintk("crypto OP %s %d\n", ret ? "failed" : "success", ret);
			crp->crp_etype = ret;
			goto done;
		}
		} break;
#endif /* HAVE_ABLKCIPHER */

	case SW_TYPE_BLKCIPHER: {
		unsigned char iv[EALG_MAX_BLOCK_LEN];
		unsigned char *ivp = iv;
		struct blkcipher_desc desc;
		int ivsize = crypto_blkcipher_ivsize(crypto_blkcipher_cast(sw->sw_tfm));

		if (sg_len < crypto_blkcipher_blocksize(
				crypto_blkcipher_cast(sw->sw_tfm))) {
			crp->crp_etype = EINVAL;
			dprintk("%s,%d: EINVAL len %d < %d\n", __FILE__, __LINE__,
					sg_len, crypto_blkcipher_blocksize(
						crypto_blkcipher_cast(sw->sw_tfm)));
			goto done;
		}

		if (ivsize > sizeof(iv)) {
			crp->crp_etype = EINVAL;
			dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
			goto done;
		}

		if (crd->crd_flags & CRD_F_KEY_EXPLICIT) {
			int i, error;

			if (debug) {
				dprintk("%s key:", __FUNCTION__);
				for (i = 0; i < (crd->crd_klen + 7) / 8; i++)
					dprintk("%s0x%x", (i % 8) ? " " : "\n    ",
							crd->crd_key[i] & 0xff);
				dprintk("\n");
			}
			/* OCF doesn't enforce keys */
			crypto_blkcipher_set_flags(crypto_blkcipher_cast(sw->sw_tfm),
					CRYPTO_TFM_REQ_WEAK_KEY);
			error = crypto_blkcipher_setkey(
						crypto_blkcipher_cast(sw->sw_tfm), crd->crd_key,
						(crd->crd_klen + 7) / 8);
			if (error) {
				dprintk("cryptosoft: setkey failed %d (crt_flags=0x%x)\n",
						error, sw->sw_tfm->crt_flags);
				crp->crp_etype = -error;
			}
		}

		memset(&desc, 0, sizeof(desc));
		desc.tfm = crypto_blkcipher_cast(sw->sw_tfm);

		if (crd->crd_flags & CRD_F_ENCRYPT) { /* encrypt */

			if (crd->crd_flags & CRD_F_IV_EXPLICIT) {
				ivp = crd->crd_iv;
			} else {
				get_random_bytes(ivp, ivsize);
			}
			/*
			 * do we have to copy the IV back to the buffer ?
			 */
			if ((crd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				crypto_copyback(crp->crp_flags, crp->crp_buf,
						crd->crd_inject, ivsize, (caddr_t)ivp);
			}
			desc.info = ivp;
			crypto_blkcipher_encrypt_iv(&desc, req->sg, req->sg, sg_len);

		} else { /*decrypt */

			if (crd->crd_flags & CRD_F_IV_EXPLICIT) {
				ivp = crd->crd_iv;
			} else {
				crypto_copydata(crp->crp_flags, crp->crp_buf,
						crd->crd_inject, ivsize, (caddr_t)ivp);
			}
			desc.info = ivp;
			crypto_blkcipher_decrypt_iv(&desc, req->sg, req->sg, sg_len);
		}
		} break;

	case SW_TYPE_HMAC:
	case SW_TYPE_HASH:
		{
		char result[HASH_MAX_LEN];
		struct hash_desc desc;

		/* check we have room for the result */
		if (crp->crp_ilen - crd->crd_inject < sw->u.hmac.sw_mlen) {
			dprintk("cryptosoft: EINVAL crp_ilen=%d, len=%d, inject=%d "
					"digestsize=%d\n", crp->crp_ilen, crd->crd_skip + sg_len,
					crd->crd_inject, sw->u.hmac.sw_mlen);
			crp->crp_etype = EINVAL;
			goto done;
		}

		memset(&desc, 0, sizeof(desc));
		desc.tfm = crypto_hash_cast(sw->sw_tfm);

		memset(result, 0, sizeof(result));

		if (sw->sw_type & SW_TYPE_HMAC) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
			crypto_hmac(sw->sw_tfm, sw->u.hmac.sw_key, &sw->u.hmac.sw_klen,
					req->sg, sg_num, result);
#else
			crypto_hash_setkey(desc.tfm, sw->u.hmac.sw_key,
					sw->u.hmac.sw_klen);
			crypto_hash_digest(&desc, req->sg, sg_len, result);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) */
			
		} else { /* SW_TYPE_HASH */
			crypto_hash_digest(&desc, req->sg, sg_len, result);
		}

		crypto_copyback(crp->crp_flags, crp->crp_buf,
				crd->crd_inject, sw->u.hmac.sw_mlen, result);
		}
		break;

	case SW_TYPE_COMP: {
		void *ibuf = NULL;
		void *obuf = sw->u.sw_comp_buf;
		int ilen = sg_len, olen = CRYPTO_MAX_DATA_LEN;
		int ret = 0;

		/*
		 * we need to use an additional copy if there is more than one
		 * input chunk since the kernel comp routines do not handle
		 * SG yet.  Otherwise we just use the input buffer as is.
		 * Rather than allocate another buffer we just split the tmp
		 * buffer we already have.
		 * Perhaps we should just use zlib directly ?
		 */
		if (sg_num > 1) {
			int blk;

			ibuf = obuf;
			for (blk = 0; blk < sg_num; blk++) {
				memcpy(obuf, sg_virt(&req->sg[blk]),
						req->sg[blk].length);
				obuf += req->sg[blk].length;
			}
			olen -= sg_len;
		} else
			ibuf = sg_virt(&req->sg[0]);

		if (crd->crd_flags & CRD_F_ENCRYPT) { /* compress */
			ret = crypto_comp_compress(crypto_comp_cast(sw->sw_tfm),
					ibuf, ilen, obuf, &olen);
			if (!ret && olen > crd->crd_len) {
				dprintk("cryptosoft: ERANGE compress %d into %d\n",
						crd->crd_len, olen);
				if (swcr_fail_if_compression_grows)
					ret = ERANGE;
			}
		} else { /* decompress */
			ret = crypto_comp_decompress(crypto_comp_cast(sw->sw_tfm),
					ibuf, ilen, obuf, &olen);
			if (!ret && (olen + crd->crd_inject) > crp->crp_olen) {
				dprintk("cryptosoft: ETOOSMALL decompress %d into %d, "
						"space for %d,at offset %d\n",
						crd->crd_len, olen, crp->crp_olen, crd->crd_inject);
				ret = ETOOSMALL;
			}
		}
		if (ret)
			dprintk("%s,%d: ret = %d\n", __FILE__, __LINE__, ret);

		/*
		 * on success copy result back,
		 * linux crpyto API returns -errno,  we need to fix that
		 */
		crp->crp_etype = ret < 0 ? -ret : ret;
		if (ret == 0) {
			/* copy back the result and return it's size */
			crypto_copyback(crp->crp_flags, crp->crp_buf,
					crd->crd_inject, olen, obuf);
			crp->crp_olen = olen;
		}
		} break;

	default:
		/* Unknown/unsupported algorithm */
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	}

done:
	swcr_process_req_complete(req);
}


/*
 * Process a crypto request.
 */
static int
swcr_process(device_t dev, struct cryptop *crp, int hint)
{
	struct swcr_req *req = NULL;
	u_int32_t lid;

	dprintk("%s()\n", __FUNCTION__);
	/* Sanity check */
	if (crp == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	crp->crp_etype = 0;

	if (crp->crp_desc == NULL || crp->crp_buf == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	}

	lid = crp->crp_sid & 0xffffffff;
	if (lid >= swcr_sesnum || lid == 0 || swcr_sessions == NULL ||
			swcr_sessions[lid] == NULL) {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}

	/*
	 * do some error checking outside of the loop for SKB and IOV processing
	 * this leaves us with valid skb or uiop pointers for later
	 */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		struct sk_buff *skb = (struct sk_buff *) crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags >= SCATTERLIST_MAX) {
			printk("%s,%d: %d nr_frags > SCATTERLIST_MAX", __FILE__, __LINE__,
					skb_shinfo(skb)->nr_frags);
			goto done;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		struct uio *uiop = (struct uio *) crp->crp_buf;
		if (uiop->uio_iovcnt > SCATTERLIST_MAX) {
			printk("%s,%d: %d uio_iovcnt > SCATTERLIST_MAX", __FILE__, __LINE__,
					uiop->uio_iovcnt);
			goto done;
		}
	}

	/*
	 * setup a new request ready for queuing
	 */
	req = kmem_cache_alloc(swcr_req_cache, SLAB_ATOMIC);
	if (req == NULL) {
		dprintk("%s,%d: ENOMEM\n", __FILE__, __LINE__);
		crp->crp_etype = ENOMEM;
		goto done;
	}
	memset(req, 0, sizeof(*req));

	req->sw_head = swcr_sessions[lid];
	req->crp = crp;
	req->crd = crp->crp_desc;

	swcr_process_req(req);
	return 0;

done:
	crypto_done(crp);
	if (req)
		kmem_cache_free(swcr_req_cache, req);
	return 0;
}


static int
cryptosoft_init(void)
{
	int i, sw_type, mode;
	char *algo;

	dprintk("%s(%p)\n", __FUNCTION__, cryptosoft_init);

	swcr_req_cache = kmem_cache_create("cryptosoft_req",
				sizeof(struct swcr_req), 0, SLAB_HWCACHE_ALIGN, NULL
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
				, NULL
#endif
				);
	if (!swcr_req_cache) {
		printk("cryptosoft: failed to create request cache\n");
		return -ENOENT;
	}

	softc_device_init(&swcr_softc, "cryptosoft", 0, swcr_methods);

	swcr_id = crypto_get_driverid(softc_get_device(&swcr_softc),
			CRYPTOCAP_F_SOFTWARE | CRYPTOCAP_F_SYNC);
	if (swcr_id < 0) {
		printk("cryptosoft: Software crypto device cannot initialize!");
		return -ENODEV;
	}

#define	REGISTER(alg) \
		crypto_register(swcr_id, alg, 0,0)

	for (i = 0; i < sizeof(crypto_details)/sizeof(crypto_details[0]); i++) {
		int found;
		
		algo = crypto_details[i].alg_name;
		if (!algo || !*algo) {
			dprintk("%s:Algorithm %d not supported\n", __FUNCTION__, i);
			continue;
		}

		mode = crypto_details[i].mode;
		sw_type = crypto_details[i].sw_type;

		found = 0;
		switch (sw_type & SW_TYPE_ALG_MASK) {
		case SW_TYPE_CIPHER:
			found = crypto_has_cipher(algo, 0, CRYPTO_ALG_ASYNC);
			break;
		case SW_TYPE_HMAC:
			found = crypto_has_hash(algo, 0, swcr_no_ahash?CRYPTO_ALG_ASYNC:0);
			break;
		case SW_TYPE_HASH:
			found = crypto_has_hash(algo, 0, swcr_no_ahash?CRYPTO_ALG_ASYNC:0);
			break;
		case SW_TYPE_COMP:
			found = crypto_has_comp(algo, 0, CRYPTO_ALG_ASYNC);
			break;
		case SW_TYPE_BLKCIPHER:
			found = crypto_has_blkcipher(algo, 0, CRYPTO_ALG_ASYNC);
			if (!found && !swcr_no_ablk)
				found = crypto_has_ablkcipher(algo, 0, 0);
			break;
		}
		if (found) {
			REGISTER(i);
		} else {
			dprintk("%s:Algorithm Type %d not supported (algorithm %d:'%s')\n",
					__FUNCTION__, sw_type, i, algo);
		}
	}
	return 0;
}

static void
cryptosoft_exit(void)
{
	dprintk("%s()\n", __FUNCTION__);
	crypto_unregister_all(swcr_id);
	swcr_id = -1;
	kmem_cache_destroy(swcr_req_cache);
}

late_initcall(cryptosoft_init);
module_exit(cryptosoft_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("Cryptosoft (OCF module for kernel crypto)");
