/*
 * Octeon Crypto for OCF
 *
 * Written by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2009-2010 David McCullough
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
#include <linux/scatterlist.h>

#include <cryptodev.h>
#include <uio.h>

struct {
	softc_device_decl	sc_dev;
} octo_softc;

#define offset_in_page(p) ((unsigned long)(p) & ~PAGE_MASK)

struct octo_sess {
	int					 octo_encalg;
	#define MAX_CIPHER_KEYLEN	64
	char				 octo_enckey[MAX_CIPHER_KEYLEN];
	int					 octo_encklen;

	int					 octo_macalg;
	#define MAX_HASH_KEYLEN	64
	char				 octo_mackey[MAX_HASH_KEYLEN];
	int					 octo_macklen;
	int					 octo_mackey_set;

	int					 octo_mlen;
	int					 octo_ivsize;

	int					(*octo_encrypt)(struct octo_sess *od,
	                      struct scatterlist *sg, int sg_len,
						  int auth_off, int auth_len,
						  int crypt_off, int crypt_len,
						  int icv_off, uint8_t *ivp);
	int					(*octo_decrypt)(struct octo_sess *od,
	                      struct scatterlist *sg, int sg_len,
						  int auth_off, int auth_len,
						  int crypt_off, int crypt_len,
						  int icv_off, uint8_t *ivp);

	uint64_t			 octo_hminner[3];
	uint64_t			 octo_hmouter[3];
};

int32_t octo_id = -1;
module_param(octo_id, int, 0444);
MODULE_PARM_DESC(octo_id, "Read-Only OCF ID for cryptocteon driver");

static struct octo_sess **octo_sessions = NULL;
static u_int32_t octo_sesnum = 0;

static	int octo_process(device_t, struct cryptop *, int);
static	int octo_newsession(device_t, u_int32_t *, struct cryptoini *);
static	int octo_freesession(device_t, u_int64_t);

static device_method_t octo_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	octo_newsession),
	DEVMETHOD(cryptodev_freesession,octo_freesession),
	DEVMETHOD(cryptodev_process,	octo_process),
};

#define debug octo_debug
int octo_debug = 0;
module_param(octo_debug, int, 0644);
MODULE_PARM_DESC(octo_debug, "Enable debug");


#include "cavium_crypto.c"


/*
 * Generate a new octo session.  We artifically limit it to a single
 * hash/cipher or hash-cipher combo just to make it easier, most callers
 * do not expect more than this anyway.
 */
static int
octo_newsession(device_t dev, u_int32_t *sid, struct cryptoini *cri)
{
	struct cryptoini *c, *encini = NULL, *macini = NULL;
	struct octo_sess **ocd;
	int i;

	dprintk("%s()\n", __FUNCTION__);
	if (sid == NULL || cri == NULL) {
		dprintk("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	/*
	 * To keep it simple, we only handle hash, cipher or hash/cipher in a
	 * session,  you cannot currently do multiple ciphers/hashes in one
	 * session even though it would be possibel to code this driver to
	 * handle it.
	 */
	for (i = 0, c = cri; c && i < 2; i++) {
		if (c->cri_alg == CRYPTO_MD5_HMAC ||
				c->cri_alg == CRYPTO_SHA1_HMAC ||
				c->cri_alg == CRYPTO_NULL_HMAC) {
			if (macini) {
				break;
			}
			macini = c;
		}
		if (c->cri_alg == CRYPTO_DES_CBC ||
				c->cri_alg == CRYPTO_3DES_CBC ||
				c->cri_alg == CRYPTO_AES_CBC ||
				c->cri_alg == CRYPTO_NULL_CBC) {
			if (encini) {
				break;
			}
			encini = c;
		}
		c = c->cri_next;
	}
	if (!macini && !encini) {
		dprintk("%s,%d - EINVAL bad cipher/hash or combination\n",
				__FILE__, __LINE__);
		return EINVAL;
	}
	if (c) {
		dprintk("%s,%d - EINVAL cannot handle chained cipher/hash combos\n",
				__FILE__, __LINE__);
		return EINVAL;
	}

	/*
	 * So we have something we can do, lets setup the session
	 */

	if (octo_sessions) {
		for (i = 1; i < octo_sesnum; i++)
			if (octo_sessions[i] == NULL)
				break;
	} else
		i = 1;		/* NB: to silence compiler warning */

	if (octo_sessions == NULL || i == octo_sesnum) {
		if (octo_sessions == NULL) {
			i = 1; /* We leave octo_sessions[0] empty */
			octo_sesnum = CRYPTO_SW_SESSIONS;
		} else
			octo_sesnum *= 2;

		ocd = kmalloc(octo_sesnum * sizeof(struct octo_sess *), SLAB_ATOMIC);
		if (ocd == NULL) {
			/* Reset session number */
			if (octo_sesnum == CRYPTO_SW_SESSIONS)
				octo_sesnum = 0;
			else
				octo_sesnum /= 2;
			dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
			return ENOBUFS;
		}
		memset(ocd, 0, octo_sesnum * sizeof(struct octo_sess *));

		/* Copy existing sessions */
		if (octo_sessions) {
			memcpy(ocd, octo_sessions,
			    (octo_sesnum / 2) * sizeof(struct octo_sess *));
			kfree(octo_sessions);
		}

		octo_sessions = ocd;
	}

	ocd = &octo_sessions[i];
	*sid = i;


	*ocd = (struct octo_sess *) kmalloc(sizeof(struct octo_sess), SLAB_ATOMIC);
	if (*ocd == NULL) {
		octo_freesession(NULL, i);
		dprintk("%s,%d: ENOBUFS\n", __FILE__, __LINE__);
		return ENOBUFS;
	}
	memset(*ocd, 0, sizeof(struct octo_sess));

	if (encini && encini->cri_key) {
		(*ocd)->octo_encklen = (encini->cri_klen + 7) / 8;
		memcpy((*ocd)->octo_enckey, encini->cri_key, (*ocd)->octo_encklen);
	}

	if (macini && macini->cri_key) {
		(*ocd)->octo_macklen = (macini->cri_klen + 7) / 8;
		memcpy((*ocd)->octo_mackey, macini->cri_key, (*ocd)->octo_macklen);
	}

	(*ocd)->octo_mlen = 0;
	if (encini && encini->cri_mlen)
		(*ocd)->octo_mlen = encini->cri_mlen;
	else if (macini && macini->cri_mlen)
		(*ocd)->octo_mlen = macini->cri_mlen;
	else
		(*ocd)->octo_mlen = 12;

	/*
	 * point c at the enc if it exists, otherwise the mac
	 */
	c = encini ? encini : macini;

	switch (c->cri_alg) {
	case CRYPTO_DES_CBC:
	case CRYPTO_3DES_CBC:
		(*ocd)->octo_ivsize  = 8;
		switch (macini ? macini->cri_alg : -1) {
		case CRYPTO_MD5_HMAC:
			(*ocd)->octo_encrypt = octo_des_cbc_md5_encrypt;
			(*ocd)->octo_decrypt = octo_des_cbc_md5_decrypt;
			octo_calc_hash(0, macini->cri_key, (*ocd)->octo_hminner,
					(*ocd)->octo_hmouter);
			break;
		case CRYPTO_SHA1_HMAC:
			(*ocd)->octo_encrypt = octo_des_cbc_sha1_encrypt;
			(*ocd)->octo_decrypt = octo_des_cbc_sha1_decrypt;
			octo_calc_hash(1, macini->cri_key, (*ocd)->octo_hminner,
					(*ocd)->octo_hmouter);
			break;
		case -1:
			(*ocd)->octo_encrypt = octo_des_cbc_encrypt;
			(*ocd)->octo_decrypt = octo_des_cbc_decrypt;
			break;
		default:
			octo_freesession(NULL, i);
			dprintk("%s,%d: EINVALn", __FILE__, __LINE__);
			return EINVAL;
		}
		break;
	case CRYPTO_AES_CBC:
		(*ocd)->octo_ivsize  = 16;
		switch (macini ? macini->cri_alg : -1) {
		case CRYPTO_MD5_HMAC:
			(*ocd)->octo_encrypt = octo_aes_cbc_md5_encrypt;
			(*ocd)->octo_decrypt = octo_aes_cbc_md5_decrypt;
			octo_calc_hash(0, macini->cri_key, (*ocd)->octo_hminner,
					(*ocd)->octo_hmouter);
			break;
		case CRYPTO_SHA1_HMAC:
			(*ocd)->octo_encrypt = octo_aes_cbc_sha1_encrypt;
			(*ocd)->octo_decrypt = octo_aes_cbc_sha1_decrypt;
			octo_calc_hash(1, macini->cri_key, (*ocd)->octo_hminner,
					(*ocd)->octo_hmouter);
			break;
		case -1:
			(*ocd)->octo_encrypt = octo_aes_cbc_encrypt;
			(*ocd)->octo_decrypt = octo_aes_cbc_decrypt;
			break;
		default:
			octo_freesession(NULL, i);
			dprintk("%s,%d: EINVALn", __FILE__, __LINE__);
			return EINVAL;
		}
		break;
	case CRYPTO_MD5_HMAC:
		(*ocd)->octo_encrypt = octo_null_md5_encrypt;
		(*ocd)->octo_decrypt = octo_null_md5_encrypt; /* encrypt == decrypt */
		octo_calc_hash(0, macini->cri_key, (*ocd)->octo_hminner,
				(*ocd)->octo_hmouter);
		break;
	case CRYPTO_SHA1_HMAC:
		(*ocd)->octo_encrypt = octo_null_sha1_encrypt;
		(*ocd)->octo_decrypt = octo_null_sha1_encrypt; /* encrypt == decrypt */
		octo_calc_hash(1, macini->cri_key, (*ocd)->octo_hminner,
				(*ocd)->octo_hmouter);
		break;
	default:
		octo_freesession(NULL, i);
		dprintk("%s,%d: EINVALn", __FILE__, __LINE__);
		return EINVAL;
	}

	(*ocd)->octo_encalg = encini ? encini->cri_alg : -1;
	(*ocd)->octo_macalg = macini ? macini->cri_alg : -1;

	return 0;
}

/*
 * Free a session.
 */
static int
octo_freesession(device_t dev, u_int64_t tid)
{
	u_int32_t sid = CRYPTO_SESID2LID(tid);

	dprintk("%s()\n", __FUNCTION__);
	if (sid > octo_sesnum || octo_sessions == NULL ||
			octo_sessions[sid] == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return(EINVAL);
	}

	/* Silently accept and return */
	if (sid == 0)
		return(0);

	if (octo_sessions[sid])
		kfree(octo_sessions[sid]);
	octo_sessions[sid] = NULL;
	return 0;
}

/*
 * Process a request.
 */
static int
octo_process(device_t dev, struct cryptop *crp, int hint)
{
	struct cryptodesc *crd;
	struct octo_sess *od;
	u_int32_t lid;
#define SCATTERLIST_MAX 16
	struct scatterlist sg[SCATTERLIST_MAX];
	int sg_num, sg_len;
	struct sk_buff *skb = NULL;
	struct uio *uiop = NULL;
	struct cryptodesc *enccrd = NULL, *maccrd = NULL;
	unsigned char *ivp = NULL;
	unsigned char iv_data[HASH_MAX_LEN];
	int auth_off = 0, auth_len = 0, crypt_off = 0, crypt_len = 0, icv_off = 0;

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
	if (lid >= octo_sesnum || lid == 0 || octo_sessions == NULL ||
			octo_sessions[lid] == NULL) {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}
	od = octo_sessions[lid];

	/*
	 * do some error checking outside of the loop for SKB and IOV processing
	 * this leaves us with valid skb or uiop pointers for later
	 */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		skb = (struct sk_buff *) crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags >= SCATTERLIST_MAX) {
			printk("%s,%d: %d nr_frags > SCATTERLIST_MAX", __FILE__, __LINE__,
					skb_shinfo(skb)->nr_frags);
			goto done;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		uiop = (struct uio *) crp->crp_buf;
		if (uiop->uio_iovcnt > SCATTERLIST_MAX) {
			printk("%s,%d: %d uio_iovcnt > SCATTERLIST_MAX", __FILE__, __LINE__,
					uiop->uio_iovcnt);
			goto done;
		}
	}

	/* point our enccrd and maccrd appropriately */
	crd = crp->crp_desc;
	if (crd->crd_alg == od->octo_encalg) enccrd = crd;
	if (crd->crd_alg == od->octo_macalg) maccrd = crd;
	crd = crd->crd_next;
	if (crd) {
		if (crd->crd_alg == od->octo_encalg) enccrd = crd;
		if (crd->crd_alg == od->octo_macalg) maccrd = crd;
		crd = crd->crd_next;
	}
	if (crd) {
		crp->crp_etype = EINVAL;
		dprintk("%s,%d: ENOENT - descriptors do not match session\n",
				__FILE__, __LINE__);
		goto done;
	}

	if (enccrd) {
		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				ivp = enccrd->crd_iv;
			else
				read_random((ivp = iv_data), od->octo_ivsize);
			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0)
				crypto_copyback(crp->crp_flags, crp->crp_buf,
						enccrd->crd_inject, od->octo_ivsize, ivp);
		} else {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT) {
				ivp = enccrd->crd_iv;
			} else {
				ivp = iv_data;
				crypto_copydata(crp->crp_flags, crp->crp_buf,
						enccrd->crd_inject, od->octo_ivsize, (caddr_t) ivp);
			}
		}

		if (maccrd) {
			auth_off = maccrd->crd_skip;
			auth_len = maccrd->crd_len;
			icv_off  = maccrd->crd_inject;
		}

		crypt_off = enccrd->crd_skip;
		crypt_len = enccrd->crd_len;
	} else { /* if (maccrd) */
		auth_off = maccrd->crd_skip;
		auth_len = maccrd->crd_len;
		icv_off  = maccrd->crd_inject;
	}


	/*
	 * setup the SG list to cover the buffer
	 */
	memset(sg, 0, sizeof(sg));
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		int i, len;

		sg_num = 0;
		sg_len = 0;

		len = skb_headlen(skb);
		sg_set_page(&sg[sg_num], virt_to_page(skb->data), len,
				offset_in_page(skb->data));
		sg_len += len;
		sg_num++;

		for (i = 0; i < skb_shinfo(skb)->nr_frags && sg_num < SCATTERLIST_MAX;
				i++) {
			len = skb_shinfo(skb)->frags[i].size;
			sg_set_page(&sg[sg_num], skb_frag_page(&skb_shinfo(skb)->frags[i]),
					len, skb_shinfo(skb)->frags[i].page_offset);
			sg_len += len;
			sg_num++;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		int len;

		sg_len = 0;
		for (sg_num = 0; sg_len < crp->crp_ilen &&
				sg_num < uiop->uio_iovcnt &&
				sg_num < SCATTERLIST_MAX; sg_num++) {
			len = uiop->uio_iov[sg_num].iov_len;
			sg_set_page(&sg[sg_num],
					virt_to_page(uiop->uio_iov[sg_num].iov_base), len,
					offset_in_page(uiop->uio_iov[sg_num].iov_base));
			sg_len += len;
		}
	} else {
		sg_len = crp->crp_ilen;
		sg_set_page(&sg[0], virt_to_page(crp->crp_buf), sg_len,
				offset_in_page(crp->crp_buf));
		sg_num = 1;
	}
	if (sg_num > 0)
		sg_mark_end(&sg[sg_num-1]);

	/*
	 * setup a new explicit key
	 */
	if (enccrd) {
		if (enccrd->crd_flags & CRD_F_KEY_EXPLICIT) {
			od->octo_encklen = (enccrd->crd_klen + 7) / 8;
			memcpy(od->octo_enckey, enccrd->crd_key, od->octo_encklen);
		}
	}
	if (maccrd) {
		if (maccrd->crd_flags & CRD_F_KEY_EXPLICIT) {
			od->octo_macklen = (maccrd->crd_klen + 7) / 8;
			memcpy(od->octo_mackey, maccrd->crd_key, od->octo_macklen);
			od->octo_mackey_set = 0;
		}
		if (!od->octo_mackey_set) {
			octo_calc_hash(maccrd->crd_alg == CRYPTO_MD5_HMAC ? 0 : 1,
				maccrd->crd_key, od->octo_hminner, od->octo_hmouter);
			od->octo_mackey_set = 1;
		}
	}


	if (!enccrd || (enccrd->crd_flags & CRD_F_ENCRYPT))
		(*od->octo_encrypt)(od, sg, sg_len,
				auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);
	else
		(*od->octo_decrypt)(od, sg, sg_len,
				auth_off, auth_len, crypt_off, crypt_len, icv_off, ivp);

done:
	crypto_done(crp);
	return 0;
}

static int
cryptocteon_init(void)
{
	dprintk("%s(%p)\n", __FUNCTION__, cryptocteon_init);

	softc_device_init(&octo_softc, "cryptocteon", 0, octo_methods);

	octo_id = crypto_get_driverid(softc_get_device(&octo_softc),
			CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SYNC);
	if (octo_id < 0) {
		printk("Cryptocteon device cannot initialize!");
		return -ENODEV;
	}

	crypto_register(octo_id, CRYPTO_MD5_HMAC, 0,0);
	crypto_register(octo_id, CRYPTO_SHA1_HMAC, 0,0);
	//crypto_register(octo_id, CRYPTO_MD5, 0,0);
	//crypto_register(octo_id, CRYPTO_SHA1, 0,0);
	crypto_register(octo_id, CRYPTO_DES_CBC, 0,0);
	crypto_register(octo_id, CRYPTO_3DES_CBC, 0,0);
	crypto_register(octo_id, CRYPTO_AES_CBC, 0,0);

	return(0);
}

static void
cryptocteon_exit(void)
{
	dprintk("%s()\n", __FUNCTION__);
	crypto_unregister_all(octo_id);
	octo_id = -1;
}

module_init(cryptocteon_init);
module_exit(cryptocteon_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("Cryptocteon (OCF module for Cavium OCTEON crypto)");
