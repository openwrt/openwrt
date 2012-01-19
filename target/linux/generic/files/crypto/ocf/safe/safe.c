/*-
 * Linux port done by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2004-2010 David McCullough
 * The license and original author are listed below.
 *
 * Copyright (c) 2003 Sam Leffler, Errno Consulting
 * Copyright (c) 2003 Global Technology Associates, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
__FBSDID("$FreeBSD: src/sys/dev/safe/safe.c,v 1.18 2007/03/21 03:42:50 sam Exp $");
 */

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) && !defined(AUTOCONF_INCLUDED)
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <asm/io.h>

/*
 * SafeNet SafeXcel-1141 hardware crypto accelerator
 */

#include <cryptodev.h>
#include <uio.h>
#include <safe/safereg.h>
#include <safe/safevar.h>

#if 1
#define	DPRINTF(a)	do { \
						if (debug) { \
							printk("%s: ", sc ? \
								device_get_nameunit(sc->sc_dev) : "safe"); \
							printk a; \
						} \
					} while (0)
#else
#define	DPRINTF(a)
#endif

/*
 * until we find a cleaner way, include the BSD md5/sha1 code
 * here
 */
#define HMAC_HACK 1
#ifdef HMAC_HACK
#include <safe/hmachack.h>
#include <safe/md5.h>
#include <safe/md5.c>
#include <safe/sha1.h>
#include <safe/sha1.c>
#endif /* HMAC_HACK */

/* add proc entry for this */
struct safe_stats safestats;

#define debug safe_debug
int safe_debug = 0;
module_param(safe_debug, int, 0644);
MODULE_PARM_DESC(safe_debug, "Enable debug");

static	void safe_callback(struct safe_softc *, struct safe_ringentry *);
static	void safe_feed(struct safe_softc *, struct safe_ringentry *);
#if defined(CONFIG_OCF_RANDOMHARVEST) && !defined(SAFE_NO_RNG)
static	void safe_rng_init(struct safe_softc *);
int safe_rngbufsize = 8;		/* 32 bytes each read  */
module_param(safe_rngbufsize, int, 0644);
MODULE_PARM_DESC(safe_rngbufsize, "RNG polling buffer size (32-bit words)");
int safe_rngmaxalarm = 8;		/* max alarms before reset */
module_param(safe_rngmaxalarm, int, 0644);
MODULE_PARM_DESC(safe_rngmaxalarm, "RNG max alarms before reset");
#endif /* SAFE_NO_RNG */

static void safe_totalreset(struct safe_softc *sc);
static int safe_dmamap_aligned(struct safe_softc *sc, const struct safe_operand *op);
static int safe_dmamap_uniform(struct safe_softc *sc, const struct safe_operand *op);
static int safe_free_entry(struct safe_softc *sc, struct safe_ringentry *re);
static int safe_kprocess(device_t dev, struct cryptkop *krp, int hint);
static int safe_kstart(struct safe_softc *sc);
static int safe_ksigbits(struct safe_softc *sc, struct crparam *cr);
static void safe_kfeed(struct safe_softc *sc);
static void safe_kpoll(unsigned long arg);
static void safe_kload_reg(struct safe_softc *sc, u_int32_t off,
								u_int32_t len, struct crparam *n);

static	int safe_newsession(device_t, u_int32_t *, struct cryptoini *);
static	int safe_freesession(device_t, u_int64_t);
static	int safe_process(device_t, struct cryptop *, int);

static device_method_t safe_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	safe_newsession),
	DEVMETHOD(cryptodev_freesession,safe_freesession),
	DEVMETHOD(cryptodev_process,	safe_process),
	DEVMETHOD(cryptodev_kprocess,	safe_kprocess),
};

#define	READ_REG(sc,r)			readl((sc)->sc_base_addr + (r))
#define WRITE_REG(sc,r,val)		writel((val), (sc)->sc_base_addr + (r))

#define SAFE_MAX_CHIPS 8
static struct safe_softc *safe_chip_idx[SAFE_MAX_CHIPS];

/*
 * split our buffers up into safe DMAable byte fragments to avoid lockup
 * bug in 1141 HW on rev 1.0.
 */

static int
pci_map_linear(
	struct safe_softc *sc,
	struct safe_operand *buf,
	void *addr,
	int len)
{
	dma_addr_t tmp;
	int chunk, tlen = len;

	tmp = pci_map_single(sc->sc_pcidev, addr, len, PCI_DMA_BIDIRECTIONAL);

	buf->mapsize += len;
	while (len > 0) {
		chunk = (len > sc->sc_max_dsize) ? sc->sc_max_dsize : len;
		buf->segs[buf->nsegs].ds_addr = tmp;
		buf->segs[buf->nsegs].ds_len  = chunk;
		buf->segs[buf->nsegs].ds_tlen = tlen;
		buf->nsegs++;
		tmp  += chunk;
		len  -= chunk;
		tlen = 0;
	}
	return 0;
}

/*
 * map in a given uio buffer (great on some arches :-)
 */

static int
pci_map_uio(struct safe_softc *sc, struct safe_operand *buf, struct uio *uio)
{
	struct iovec *iov = uio->uio_iov;
	int n;

	DPRINTF(("%s()\n", __FUNCTION__));

	buf->mapsize = 0;
	buf->nsegs = 0;

	for (n = 0; n < uio->uio_iovcnt; n++) {
		pci_map_linear(sc, buf, iov->iov_base, iov->iov_len);
		iov++;
	}

	/* identify this buffer by the first segment */
	buf->map = (void *) buf->segs[0].ds_addr;
	return(0);
}

/*
 * map in a given sk_buff
 */

static int
pci_map_skb(struct safe_softc *sc,struct safe_operand *buf,struct sk_buff *skb)
{
	int i;

	DPRINTF(("%s()\n", __FUNCTION__));

	buf->mapsize = 0;
	buf->nsegs = 0;

	pci_map_linear(sc, buf, skb->data, skb_headlen(skb));

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		pci_map_linear(sc, buf,
				page_address(skb_frag_page(&skb_shinfo(skb)->frags[i])) +
				                        skb_shinfo(skb)->frags[i].page_offset,
				skb_shinfo(skb)->frags[i].size);
	}

	/* identify this buffer by the first segment */
	buf->map = (void *) buf->segs[0].ds_addr;
	return(0);
}


#if 0 /* not needed at this time */
static void
pci_sync_operand(struct safe_softc *sc, struct safe_operand *buf)
{
	int i;

	DPRINTF(("%s()\n", __FUNCTION__));
	for (i = 0; i < buf->nsegs; i++)
		pci_dma_sync_single_for_cpu(sc->sc_pcidev, buf->segs[i].ds_addr,
				buf->segs[i].ds_len, PCI_DMA_BIDIRECTIONAL);
}
#endif

static void
pci_unmap_operand(struct safe_softc *sc, struct safe_operand *buf)
{
	int i;
	DPRINTF(("%s()\n", __FUNCTION__));
	for (i = 0; i < buf->nsegs; i++) {
		if (buf->segs[i].ds_tlen) {
			DPRINTF(("%s - unmap %d 0x%x %d\n", __FUNCTION__, i, buf->segs[i].ds_addr, buf->segs[i].ds_tlen));
			pci_unmap_single(sc->sc_pcidev, buf->segs[i].ds_addr,
					buf->segs[i].ds_tlen, PCI_DMA_BIDIRECTIONAL);
			DPRINTF(("%s - unmap %d 0x%x %d done\n", __FUNCTION__, i, buf->segs[i].ds_addr, buf->segs[i].ds_tlen));
		}
		buf->segs[i].ds_addr = 0;
		buf->segs[i].ds_len = 0;
		buf->segs[i].ds_tlen = 0;
	}
	buf->nsegs = 0;
	buf->mapsize = 0;
	buf->map = 0;
}


/*
 * SafeXcel Interrupt routine
 */
static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
safe_intr(int irq, void *arg)
#else
safe_intr(int irq, void *arg, struct pt_regs *regs)
#endif
{
	struct safe_softc *sc = arg;
	int stat;
	unsigned long flags;

	stat = READ_REG(sc, SAFE_HM_STAT);

	DPRINTF(("%s(stat=0x%x)\n", __FUNCTION__, stat));

	if (stat == 0)		/* shared irq, not for us */
		return IRQ_NONE;

	WRITE_REG(sc, SAFE_HI_CLR, stat);	/* IACK */

	if ((stat & SAFE_INT_PE_DDONE)) {
		/*
		 * Descriptor(s) done; scan the ring and
		 * process completed operations.
		 */
		spin_lock_irqsave(&sc->sc_ringmtx, flags);
		while (sc->sc_back != sc->sc_front) {
			struct safe_ringentry *re = sc->sc_back;

#ifdef SAFE_DEBUG
			if (debug) {
				safe_dump_ringstate(sc, __func__);
				safe_dump_request(sc, __func__, re);
			}
#endif
			/*
			 * safe_process marks ring entries that were allocated
			 * but not used with a csr of zero.  This insures the
			 * ring front pointer never needs to be set backwards
			 * in the event that an entry is allocated but not used
			 * because of a setup error.
			 */
			DPRINTF(("%s re->re_desc.d_csr=0x%x\n", __FUNCTION__, re->re_desc.d_csr));
			if (re->re_desc.d_csr != 0) {
				if (!SAFE_PE_CSR_IS_DONE(re->re_desc.d_csr)) {
					DPRINTF(("%s !CSR_IS_DONE\n", __FUNCTION__));
					break;
				}
				if (!SAFE_PE_LEN_IS_DONE(re->re_desc.d_len)) {
					DPRINTF(("%s !LEN_IS_DONE\n", __FUNCTION__));
					break;
				}
				sc->sc_nqchip--;
				safe_callback(sc, re);
			}
			if (++(sc->sc_back) == sc->sc_ringtop)
				sc->sc_back = sc->sc_ring;
		}
		spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
	}

	/*
	 * Check to see if we got any DMA Error
	 */
	if (stat & SAFE_INT_PE_ERROR) {
		printk("%s: dmaerr dmastat %08x\n", device_get_nameunit(sc->sc_dev),
				(int)READ_REG(sc, SAFE_PE_DMASTAT));
		safestats.st_dmaerr++;
		safe_totalreset(sc);
#if 0
		safe_feed(sc);
#endif
	}

	if (sc->sc_needwakeup) {		/* XXX check high watermark */
		int wakeup = sc->sc_needwakeup & (CRYPTO_SYMQ|CRYPTO_ASYMQ);
		DPRINTF(("%s: wakeup crypto %x\n", __func__,
			sc->sc_needwakeup));
		sc->sc_needwakeup &= ~wakeup;
		crypto_unblock(sc->sc_cid, wakeup);
	}
	
	return IRQ_HANDLED;
}

/*
 * safe_feed() - post a request to chip
 */
static void
safe_feed(struct safe_softc *sc, struct safe_ringentry *re)
{
	DPRINTF(("%s()\n", __FUNCTION__));
#ifdef SAFE_DEBUG
	if (debug) {
		safe_dump_ringstate(sc, __func__);
		safe_dump_request(sc, __func__, re);
	}
#endif
	sc->sc_nqchip++;
	if (sc->sc_nqchip > safestats.st_maxqchip)
		safestats.st_maxqchip = sc->sc_nqchip;
	/* poke h/w to check descriptor ring, any value can be written */
	WRITE_REG(sc, SAFE_HI_RD_DESCR, 0);
}

#define	N(a)	(sizeof(a) / sizeof (a[0]))
static void
safe_setup_enckey(struct safe_session *ses, caddr_t key)
{
	int i;

	bcopy(key, ses->ses_key, ses->ses_klen / 8);

	/* PE is little-endian, insure proper byte order */
	for (i = 0; i < N(ses->ses_key); i++)
		ses->ses_key[i] = htole32(ses->ses_key[i]);
}

static void
safe_setup_mackey(struct safe_session *ses, int algo, caddr_t key, int klen)
{
#ifdef HMAC_HACK
	MD5_CTX md5ctx;
	SHA1_CTX sha1ctx;
	int i;


	for (i = 0; i < klen; i++)
		key[i] ^= HMAC_IPAD_VAL;

	if (algo == CRYPTO_MD5_HMAC) {
		MD5Init(&md5ctx);
		MD5Update(&md5ctx, key, klen);
		MD5Update(&md5ctx, hmac_ipad_buffer, MD5_HMAC_BLOCK_LEN - klen);
		bcopy(md5ctx.md5_st8, ses->ses_hminner, sizeof(md5ctx.md5_st8));
	} else {
		SHA1Init(&sha1ctx);
		SHA1Update(&sha1ctx, key, klen);
		SHA1Update(&sha1ctx, hmac_ipad_buffer,
		    SHA1_HMAC_BLOCK_LEN - klen);
		bcopy(sha1ctx.h.b32, ses->ses_hminner, sizeof(sha1ctx.h.b32));
	}

	for (i = 0; i < klen; i++)
		key[i] ^= (HMAC_IPAD_VAL ^ HMAC_OPAD_VAL);

	if (algo == CRYPTO_MD5_HMAC) {
		MD5Init(&md5ctx);
		MD5Update(&md5ctx, key, klen);
		MD5Update(&md5ctx, hmac_opad_buffer, MD5_HMAC_BLOCK_LEN - klen);
		bcopy(md5ctx.md5_st8, ses->ses_hmouter, sizeof(md5ctx.md5_st8));
	} else {
		SHA1Init(&sha1ctx);
		SHA1Update(&sha1ctx, key, klen);
		SHA1Update(&sha1ctx, hmac_opad_buffer,
		    SHA1_HMAC_BLOCK_LEN - klen);
		bcopy(sha1ctx.h.b32, ses->ses_hmouter, sizeof(sha1ctx.h.b32));
	}

	for (i = 0; i < klen; i++)
		key[i] ^= HMAC_OPAD_VAL;

#if 0
	/*
	 * this code prevents SHA working on a BE host,
	 * so it is obviously wrong.  I think the byte
	 * swap setup we do with the chip fixes this for us
	 */

	/* PE is little-endian, insure proper byte order */
	for (i = 0; i < N(ses->ses_hminner); i++) {
		ses->ses_hminner[i] = htole32(ses->ses_hminner[i]);
		ses->ses_hmouter[i] = htole32(ses->ses_hmouter[i]);
	}
#endif
#else /* HMAC_HACK */
	printk("safe: md5/sha not implemented\n");
#endif /* HMAC_HACK */
}
#undef N

/*
 * Allocate a new 'session' and return an encoded session id.  'sidp'
 * contains our registration id, and should contain an encoded session
 * id on successful allocation.
 */
static int
safe_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
	struct safe_softc *sc = device_get_softc(dev);
	struct cryptoini *c, *encini = NULL, *macini = NULL;
	struct safe_session *ses = NULL;
	int sesn;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (sidp == NULL || cri == NULL || sc == NULL)
		return (EINVAL);

	for (c = cri; c != NULL; c = c->cri_next) {
		if (c->cri_alg == CRYPTO_MD5_HMAC ||
		    c->cri_alg == CRYPTO_SHA1_HMAC ||
		    c->cri_alg == CRYPTO_NULL_HMAC) {
			if (macini)
				return (EINVAL);
			macini = c;
		} else if (c->cri_alg == CRYPTO_DES_CBC ||
		    c->cri_alg == CRYPTO_3DES_CBC ||
		    c->cri_alg == CRYPTO_AES_CBC ||
		    c->cri_alg == CRYPTO_NULL_CBC) {
			if (encini)
				return (EINVAL);
			encini = c;
		} else
			return (EINVAL);
	}
	if (encini == NULL && macini == NULL)
		return (EINVAL);
	if (encini) {			/* validate key length */
		switch (encini->cri_alg) {
		case CRYPTO_DES_CBC:
			if (encini->cri_klen != 64)
				return (EINVAL);
			break;
		case CRYPTO_3DES_CBC:
			if (encini->cri_klen != 192)
				return (EINVAL);
			break;
		case CRYPTO_AES_CBC:
			if (encini->cri_klen != 128 &&
			    encini->cri_klen != 192 &&
			    encini->cri_klen != 256)
				return (EINVAL);
			break;
		}
	}

	if (sc->sc_sessions == NULL) {
		ses = sc->sc_sessions = (struct safe_session *)
			kmalloc(sizeof(struct safe_session), SLAB_ATOMIC);
		if (ses == NULL)
			return (ENOMEM);
		memset(ses, 0, sizeof(struct safe_session));
		sesn = 0;
		sc->sc_nsessions = 1;
	} else {
		for (sesn = 0; sesn < sc->sc_nsessions; sesn++) {
			if (sc->sc_sessions[sesn].ses_used == 0) {
				ses = &sc->sc_sessions[sesn];
				break;
			}
		}

		if (ses == NULL) {
			sesn = sc->sc_nsessions;
			ses = (struct safe_session *)
				kmalloc((sesn + 1) * sizeof(struct safe_session), SLAB_ATOMIC);
			if (ses == NULL)
				return (ENOMEM);
			memset(ses, 0, (sesn + 1) * sizeof(struct safe_session));
			bcopy(sc->sc_sessions, ses, sesn *
			    sizeof(struct safe_session));
			bzero(sc->sc_sessions, sesn *
			    sizeof(struct safe_session));
			kfree(sc->sc_sessions);
			sc->sc_sessions = ses;
			ses = &sc->sc_sessions[sesn];
			sc->sc_nsessions++;
		}
	}

	bzero(ses, sizeof(struct safe_session));
	ses->ses_used = 1;

	if (encini) {
		ses->ses_klen = encini->cri_klen;
		if (encini->cri_key != NULL)
			safe_setup_enckey(ses, encini->cri_key);
	}

	if (macini) {
		ses->ses_mlen = macini->cri_mlen;
		if (ses->ses_mlen == 0) {
			if (macini->cri_alg == CRYPTO_MD5_HMAC)
				ses->ses_mlen = MD5_HASH_LEN;
			else
				ses->ses_mlen = SHA1_HASH_LEN;
		}

		if (macini->cri_key != NULL) {
			safe_setup_mackey(ses, macini->cri_alg, macini->cri_key,
			    macini->cri_klen / 8);
		}
	}

	*sidp = SAFE_SID(device_get_unit(sc->sc_dev), sesn);
	return (0);
}

/*
 * Deallocate a session.
 */
static int
safe_freesession(device_t dev, u_int64_t tid)
{
	struct safe_softc *sc = device_get_softc(dev);
	int session, ret;
	u_int32_t sid = ((u_int32_t) tid) & 0xffffffff;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (sc == NULL)
		return (EINVAL);

	session = SAFE_SESSION(sid);
	if (session < sc->sc_nsessions) {
		bzero(&sc->sc_sessions[session], sizeof(sc->sc_sessions[session]));
		ret = 0;
	} else
		ret = EINVAL;
	return (ret);
}


static int
safe_process(device_t dev, struct cryptop *crp, int hint)
{
	struct safe_softc *sc = device_get_softc(dev);
	int err = 0, i, nicealign, uniform;
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;
	int bypass, oplen, ivsize;
	caddr_t iv;
	int16_t coffset;
	struct safe_session *ses;
	struct safe_ringentry *re;
	struct safe_sarec *sa;
	struct safe_pdesc *pd;
	u_int32_t cmd0, cmd1, staterec, rand_iv[4];
	unsigned long flags;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (crp == NULL || crp->crp_callback == NULL || sc == NULL) {
		safestats.st_invalid++;
		return (EINVAL);
	}
	if (SAFE_SESSION(crp->crp_sid) >= sc->sc_nsessions) {
		safestats.st_badsession++;
		return (EINVAL);
	}

	spin_lock_irqsave(&sc->sc_ringmtx, flags);
	if (sc->sc_front == sc->sc_back && sc->sc_nqchip != 0) {
		safestats.st_ringfull++;
		sc->sc_needwakeup |= CRYPTO_SYMQ;
		spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
		return (ERESTART);
	}
	re = sc->sc_front;

	staterec = re->re_sa.sa_staterec;	/* save */
	/* NB: zero everything but the PE descriptor */
	bzero(&re->re_sa, sizeof(struct safe_ringentry) - sizeof(re->re_desc));
	re->re_sa.sa_staterec = staterec;	/* restore */

	re->re_crp = crp;
	re->re_sesn = SAFE_SESSION(crp->crp_sid);

	re->re_src.nsegs = 0;
	re->re_dst.nsegs = 0;

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		re->re_src_skb = (struct sk_buff *)crp->crp_buf;
		re->re_dst_skb = (struct sk_buff *)crp->crp_buf;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		re->re_src_io = (struct uio *)crp->crp_buf;
		re->re_dst_io = (struct uio *)crp->crp_buf;
	} else {
		safestats.st_badflags++;
		err = EINVAL;
		goto errout;	/* XXX we don't handle contiguous blocks! */
	}

	sa = &re->re_sa;
	ses = &sc->sc_sessions[re->re_sesn];

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		safestats.st_nodesc++;
		err = EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;

	cmd0 = SAFE_SA_CMD0_BASIC;		/* basic group operation */
	cmd1 = 0;
	if (crd2 == NULL) {
		if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC ||
		    crd1->crd_alg == CRYPTO_NULL_HMAC) {
			maccrd = crd1;
			enccrd = NULL;
			cmd0 |= SAFE_SA_CMD0_OP_HASH;
		} else if (crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC ||
		    crd1->crd_alg == CRYPTO_AES_CBC ||
		    crd1->crd_alg == CRYPTO_NULL_CBC) {
			maccrd = NULL;
			enccrd = crd1;
			cmd0 |= SAFE_SA_CMD0_OP_CRYPT;
		} else {
			safestats.st_badalg++;
			err = EINVAL;
			goto errout;
		}
	} else {
		if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC ||
		    crd1->crd_alg == CRYPTO_NULL_HMAC) &&
		    (crd2->crd_alg == CRYPTO_DES_CBC ||
			crd2->crd_alg == CRYPTO_3DES_CBC ||
		        crd2->crd_alg == CRYPTO_AES_CBC ||
		        crd2->crd_alg == CRYPTO_NULL_CBC) &&
		    ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
			maccrd = crd1;
			enccrd = crd2;
		} else if ((crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC ||
		    crd1->crd_alg == CRYPTO_AES_CBC ||
		    crd1->crd_alg == CRYPTO_NULL_CBC) &&
		    (crd2->crd_alg == CRYPTO_MD5_HMAC ||
			crd2->crd_alg == CRYPTO_SHA1_HMAC ||
			crd2->crd_alg == CRYPTO_NULL_HMAC) &&
		    (crd1->crd_flags & CRD_F_ENCRYPT)) {
			enccrd = crd1;
			maccrd = crd2;
		} else {
			safestats.st_badalg++;
			err = EINVAL;
			goto errout;
		}
		cmd0 |= SAFE_SA_CMD0_OP_BOTH;
	}

	if (enccrd) {
		if (enccrd->crd_flags & CRD_F_KEY_EXPLICIT)
			safe_setup_enckey(ses, enccrd->crd_key);

		if (enccrd->crd_alg == CRYPTO_DES_CBC) {
			cmd0 |= SAFE_SA_CMD0_DES;
			cmd1 |= SAFE_SA_CMD1_CBC;
			ivsize = 2*sizeof(u_int32_t);
		} else if (enccrd->crd_alg == CRYPTO_3DES_CBC) {
			cmd0 |= SAFE_SA_CMD0_3DES;
			cmd1 |= SAFE_SA_CMD1_CBC;
			ivsize = 2*sizeof(u_int32_t);
		} else if (enccrd->crd_alg == CRYPTO_AES_CBC) {
			cmd0 |= SAFE_SA_CMD0_AES;
			cmd1 |= SAFE_SA_CMD1_CBC;
			if (ses->ses_klen == 128)
			     cmd1 |=  SAFE_SA_CMD1_AES128;
			else if (ses->ses_klen == 192)
			     cmd1 |=  SAFE_SA_CMD1_AES192;
			else
			     cmd1 |=  SAFE_SA_CMD1_AES256;
			ivsize = 4*sizeof(u_int32_t);
		} else {
			cmd0 |= SAFE_SA_CMD0_CRYPT_NULL;
			ivsize = 0;
		}

		/*
		 * Setup encrypt/decrypt state.  When using basic ops
		 * we can't use an inline IV because hash/crypt offset
		 * must be from the end of the IV to the start of the
		 * crypt data and this leaves out the preceding header
		 * from the hash calculation.  Instead we place the IV
		 * in the state record and set the hash/crypt offset to
		 * copy both the header+IV.
		 */
		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			cmd0 |= SAFE_SA_CMD0_OUTBOUND;

			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				iv = enccrd->crd_iv;
			else
				read_random((iv = (caddr_t) &rand_iv[0]), sizeof(rand_iv));
			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				crypto_copyback(crp->crp_flags, crp->crp_buf,
				    enccrd->crd_inject, ivsize, iv);
			}
			bcopy(iv, re->re_sastate.sa_saved_iv, ivsize);
			/* make iv LE */
			for (i = 0; i < ivsize/sizeof(re->re_sastate.sa_saved_iv[0]); i++)
				re->re_sastate.sa_saved_iv[i] =
					cpu_to_le32(re->re_sastate.sa_saved_iv[i]);
			cmd0 |= SAFE_SA_CMD0_IVLD_STATE | SAFE_SA_CMD0_SAVEIV;
			re->re_flags |= SAFE_QFLAGS_COPYOUTIV;
		} else {
			cmd0 |= SAFE_SA_CMD0_INBOUND;

			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT) {
				bcopy(enccrd->crd_iv,
					re->re_sastate.sa_saved_iv, ivsize);
			} else {
				crypto_copydata(crp->crp_flags, crp->crp_buf,
				    enccrd->crd_inject, ivsize,
				    (caddr_t)re->re_sastate.sa_saved_iv);
			}
			/* make iv LE */
			for (i = 0; i < ivsize/sizeof(re->re_sastate.sa_saved_iv[0]); i++)
				re->re_sastate.sa_saved_iv[i] =
					cpu_to_le32(re->re_sastate.sa_saved_iv[i]);
			cmd0 |= SAFE_SA_CMD0_IVLD_STATE;
		}
		/*
		 * For basic encryption use the zero pad algorithm.
		 * This pads results to an 8-byte boundary and
		 * suppresses padding verification for inbound (i.e.
		 * decrypt) operations.
		 *
		 * NB: Not sure if the 8-byte pad boundary is a problem.
		 */
		cmd0 |= SAFE_SA_CMD0_PAD_ZERO;

		/* XXX assert key bufs have the same size */
		bcopy(ses->ses_key, sa->sa_key, sizeof(sa->sa_key));
	}

	if (maccrd) {
		if (maccrd->crd_flags & CRD_F_KEY_EXPLICIT) {
			safe_setup_mackey(ses, maccrd->crd_alg,
			    maccrd->crd_key, maccrd->crd_klen / 8);
		}

		if (maccrd->crd_alg == CRYPTO_MD5_HMAC) {
			cmd0 |= SAFE_SA_CMD0_MD5;
			cmd1 |= SAFE_SA_CMD1_HMAC;	/* NB: enable HMAC */
		} else if (maccrd->crd_alg == CRYPTO_SHA1_HMAC) {
			cmd0 |= SAFE_SA_CMD0_SHA1;
			cmd1 |= SAFE_SA_CMD1_HMAC;	/* NB: enable HMAC */
		} else {
			cmd0 |= SAFE_SA_CMD0_HASH_NULL;
		}
		/*
		 * Digest data is loaded from the SA and the hash
		 * result is saved to the state block where we
		 * retrieve it for return to the caller.
		 */
		/* XXX assert digest bufs have the same size */
		bcopy(ses->ses_hminner, sa->sa_indigest,
			sizeof(sa->sa_indigest));
		bcopy(ses->ses_hmouter, sa->sa_outdigest,
			sizeof(sa->sa_outdigest));

		cmd0 |= SAFE_SA_CMD0_HSLD_SA | SAFE_SA_CMD0_SAVEHASH;
		re->re_flags |= SAFE_QFLAGS_COPYOUTICV;
	}

	if (enccrd && maccrd) {
		/*
		 * The offset from hash data to the start of
		 * crypt data is the difference in the skips.
		 */
		bypass = maccrd->crd_skip;
		coffset = enccrd->crd_skip - maccrd->crd_skip;
		if (coffset < 0) {
			DPRINTF(("%s: hash does not precede crypt; "
				"mac skip %u enc skip %u\n",
				__func__, maccrd->crd_skip, enccrd->crd_skip));
			safestats.st_skipmismatch++;
			err = EINVAL;
			goto errout;
		}
		oplen = enccrd->crd_skip + enccrd->crd_len;
		if (maccrd->crd_skip + maccrd->crd_len != oplen) {
			DPRINTF(("%s: hash amount %u != crypt amount %u\n",
				__func__, maccrd->crd_skip + maccrd->crd_len,
				oplen));
			safestats.st_lenmismatch++;
			err = EINVAL;
			goto errout;
		}
#ifdef SAFE_DEBUG
		if (debug) {
			printf("mac: skip %d, len %d, inject %d\n",
			    maccrd->crd_skip, maccrd->crd_len,
			    maccrd->crd_inject);
			printf("enc: skip %d, len %d, inject %d\n",
			    enccrd->crd_skip, enccrd->crd_len,
			    enccrd->crd_inject);
			printf("bypass %d coffset %d oplen %d\n",
				bypass, coffset, oplen);
		}
#endif
		if (coffset & 3) {	/* offset must be 32-bit aligned */
			DPRINTF(("%s: coffset %u misaligned\n",
				__func__, coffset));
			safestats.st_coffmisaligned++;
			err = EINVAL;
			goto errout;
		}
		coffset >>= 2;
		if (coffset > 255) {	/* offset must be <256 dwords */
			DPRINTF(("%s: coffset %u too big\n",
				__func__, coffset));
			safestats.st_cofftoobig++;
			err = EINVAL;
			goto errout;
		}
		/*
		 * Tell the hardware to copy the header to the output.
		 * The header is defined as the data from the end of
		 * the bypass to the start of data to be encrypted. 
		 * Typically this is the inline IV.  Note that you need
		 * to do this even if src+dst are the same; it appears
		 * that w/o this bit the crypted data is written
		 * immediately after the bypass data.
		 */
		cmd1 |= SAFE_SA_CMD1_HDRCOPY;
		/*
		 * Disable IP header mutable bit handling.  This is
		 * needed to get correct HMAC calculations.
		 */
		cmd1 |= SAFE_SA_CMD1_MUTABLE;
	} else {
		if (enccrd) {
			bypass = enccrd->crd_skip;
			oplen = bypass + enccrd->crd_len;
		} else {
			bypass = maccrd->crd_skip;
			oplen = bypass + maccrd->crd_len;
		}
		coffset = 0;
	}
	/* XXX verify multiple of 4 when using s/g */
	if (bypass > 96) {		/* bypass offset must be <= 96 bytes */
		DPRINTF(("%s: bypass %u too big\n", __func__, bypass));
		safestats.st_bypasstoobig++;
		err = EINVAL;
		goto errout;
	}

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		if (pci_map_skb(sc, &re->re_src, re->re_src_skb)) {
			safestats.st_noload++;
			err = ENOMEM;
			goto errout;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		if (pci_map_uio(sc, &re->re_src, re->re_src_io)) {
			safestats.st_noload++;
			err = ENOMEM;
			goto errout;
		}
	}
	nicealign = safe_dmamap_aligned(sc, &re->re_src);
	uniform = safe_dmamap_uniform(sc, &re->re_src);

	DPRINTF(("src nicealign %u uniform %u nsegs %u\n",
		nicealign, uniform, re->re_src.nsegs));
	if (re->re_src.nsegs > 1) {
		re->re_desc.d_src = sc->sc_spalloc.dma_paddr +
			((caddr_t) sc->sc_spfree - (caddr_t) sc->sc_spring);
		for (i = 0; i < re->re_src_nsegs; i++) {
			/* NB: no need to check if there's space */
			pd = sc->sc_spfree;
			if (++(sc->sc_spfree) == sc->sc_springtop)
				sc->sc_spfree = sc->sc_spring;

			KASSERT((pd->pd_flags&3) == 0 ||
				(pd->pd_flags&3) == SAFE_PD_DONE,
				("bogus source particle descriptor; flags %x",
				pd->pd_flags));
			pd->pd_addr = re->re_src_segs[i].ds_addr;
			pd->pd_size = re->re_src_segs[i].ds_len;
			pd->pd_flags = SAFE_PD_READY;
		}
		cmd0 |= SAFE_SA_CMD0_IGATHER;
	} else {
		/*
		 * No need for gather, reference the operand directly.
		 */
		re->re_desc.d_src = re->re_src_segs[0].ds_addr;
	}

	if (enccrd == NULL && maccrd != NULL) {
		/*
		 * Hash op; no destination needed.
		 */
	} else {
		if (crp->crp_flags & (CRYPTO_F_IOV|CRYPTO_F_SKBUF)) {
			if (!nicealign) {
				safestats.st_iovmisaligned++;
				err = EINVAL;
				goto errout;
			}
			if (uniform != 1) {
				device_printf(sc->sc_dev, "!uniform source\n");
				if (!uniform) {
					/*
					 * There's no way to handle the DMA
					 * requirements with this uio.  We
					 * could create a separate DMA area for
					 * the result and then copy it back,
					 * but for now we just bail and return
					 * an error.  Note that uio requests
					 * > SAFE_MAX_DSIZE are handled because
					 * the DMA map and segment list for the
					 * destination wil result in a
					 * destination particle list that does
					 * the necessary scatter DMA.
					 */ 
					safestats.st_iovnotuniform++;
					err = EINVAL;
					goto errout;
				}
			} else
				re->re_dst = re->re_src;
		} else {
			safestats.st_badflags++;
			err = EINVAL;
			goto errout;
		}

		if (re->re_dst.nsegs > 1) {
			re->re_desc.d_dst = sc->sc_dpalloc.dma_paddr +
			    ((caddr_t) sc->sc_dpfree - (caddr_t) sc->sc_dpring);
			for (i = 0; i < re->re_dst_nsegs; i++) {
				pd = sc->sc_dpfree;
				KASSERT((pd->pd_flags&3) == 0 ||
					(pd->pd_flags&3) == SAFE_PD_DONE,
					("bogus dest particle descriptor; flags %x",
						pd->pd_flags));
				if (++(sc->sc_dpfree) == sc->sc_dpringtop)
					sc->sc_dpfree = sc->sc_dpring;
				pd->pd_addr = re->re_dst_segs[i].ds_addr;
				pd->pd_flags = SAFE_PD_READY;
			}
			cmd0 |= SAFE_SA_CMD0_OSCATTER;
		} else {
			/*
			 * No need for scatter, reference the operand directly.
			 */
			re->re_desc.d_dst = re->re_dst_segs[0].ds_addr;
		}
	}

	/*
	 * All done with setup; fillin the SA command words
	 * and the packet engine descriptor.  The operation
	 * is now ready for submission to the hardware.
	 */
	sa->sa_cmd0 = cmd0 | SAFE_SA_CMD0_IPCI | SAFE_SA_CMD0_OPCI;
	sa->sa_cmd1 = cmd1
		    | (coffset << SAFE_SA_CMD1_OFFSET_S)
		    | SAFE_SA_CMD1_SAREV1	/* Rev 1 SA data structure */
		    | SAFE_SA_CMD1_SRPCI
		    ;
	/*
	 * NB: the order of writes is important here.  In case the
	 * chip is scanning the ring because of an outstanding request
	 * it might nab this one too.  In that case we need to make
	 * sure the setup is complete before we write the length
	 * field of the descriptor as it signals the descriptor is
	 * ready for processing.
	 */
	re->re_desc.d_csr = SAFE_PE_CSR_READY | SAFE_PE_CSR_SAPCI;
	if (maccrd)
		re->re_desc.d_csr |= SAFE_PE_CSR_LOADSA | SAFE_PE_CSR_HASHFINAL;
	wmb();
	re->re_desc.d_len = oplen
			  | SAFE_PE_LEN_READY
			  | (bypass << SAFE_PE_LEN_BYPASS_S)
			  ;

	safestats.st_ipackets++;
	safestats.st_ibytes += oplen;

	if (++(sc->sc_front) == sc->sc_ringtop)
		sc->sc_front = sc->sc_ring;

	/* XXX honor batching */
	safe_feed(sc, re);
	spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
	return (0);

errout:
	if (re->re_src.map != re->re_dst.map)
		pci_unmap_operand(sc, &re->re_dst);
	if (re->re_src.map)
		pci_unmap_operand(sc, &re->re_src);
	spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
	if (err != ERESTART) {
		crp->crp_etype = err;
		crypto_done(crp);
	} else {
		sc->sc_needwakeup |= CRYPTO_SYMQ;
	}
	return (err);
}

static void
safe_callback(struct safe_softc *sc, struct safe_ringentry *re)
{
	struct cryptop *crp = (struct cryptop *)re->re_crp;
	struct cryptodesc *crd;

	DPRINTF(("%s()\n", __FUNCTION__));

	safestats.st_opackets++;
	safestats.st_obytes += re->re_dst.mapsize;

	if (re->re_desc.d_csr & SAFE_PE_CSR_STATUS) {
		device_printf(sc->sc_dev, "csr 0x%x cmd0 0x%x cmd1 0x%x\n",
			re->re_desc.d_csr,
			re->re_sa.sa_cmd0, re->re_sa.sa_cmd1);
		safestats.st_peoperr++;
		crp->crp_etype = EIO;		/* something more meaningful? */
	}

	if (re->re_dst.map != NULL && re->re_dst.map != re->re_src.map)
		pci_unmap_operand(sc, &re->re_dst);
	pci_unmap_operand(sc, &re->re_src);

	/* 
	 * If result was written to a differet mbuf chain, swap
	 * it in as the return value and reclaim the original.
	 */
	if ((crp->crp_flags & CRYPTO_F_SKBUF) && re->re_src_skb != re->re_dst_skb) {
		device_printf(sc->sc_dev, "no CRYPTO_F_SKBUF swapping support\n");
		/* kfree_skb(skb) */
		/* crp->crp_buf = (caddr_t)re->re_dst_skb */
		return;
	}

	if (re->re_flags & SAFE_QFLAGS_COPYOUTICV) {
		/* copy out ICV result */
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
			if (!(crd->crd_alg == CRYPTO_MD5_HMAC ||
			    crd->crd_alg == CRYPTO_SHA1_HMAC ||
			    crd->crd_alg == CRYPTO_NULL_HMAC))
				continue;
			if (crd->crd_alg == CRYPTO_SHA1_HMAC) {
				/*
				 * SHA-1 ICV's are byte-swapped; fix 'em up
				 * before copy them to their destination.
				 */
				re->re_sastate.sa_saved_indigest[0] =
					cpu_to_be32(re->re_sastate.sa_saved_indigest[0]);
				re->re_sastate.sa_saved_indigest[1] = 
					cpu_to_be32(re->re_sastate.sa_saved_indigest[1]);
				re->re_sastate.sa_saved_indigest[2] =
					cpu_to_be32(re->re_sastate.sa_saved_indigest[2]);
			} else {
				re->re_sastate.sa_saved_indigest[0] =
					cpu_to_le32(re->re_sastate.sa_saved_indigest[0]);
				re->re_sastate.sa_saved_indigest[1] = 
					cpu_to_le32(re->re_sastate.sa_saved_indigest[1]);
				re->re_sastate.sa_saved_indigest[2] =
					cpu_to_le32(re->re_sastate.sa_saved_indigest[2]);
			}
			crypto_copyback(crp->crp_flags, crp->crp_buf,
			    crd->crd_inject,
			    sc->sc_sessions[re->re_sesn].ses_mlen,
			    (caddr_t)re->re_sastate.sa_saved_indigest);
			break;
		}
	}
	crypto_done(crp);
}


#if defined(CONFIG_OCF_RANDOMHARVEST) && !defined(SAFE_NO_RNG)
#define	SAFE_RNG_MAXWAIT	1000

static void
safe_rng_init(struct safe_softc *sc)
{
	u_int32_t w, v;
	int i;

	DPRINTF(("%s()\n", __FUNCTION__));

	WRITE_REG(sc, SAFE_RNG_CTRL, 0);
	/* use default value according to the manual */
	WRITE_REG(sc, SAFE_RNG_CNFG, 0x834);	/* magic from SafeNet */
	WRITE_REG(sc, SAFE_RNG_ALM_CNT, 0);

	/*
	 * There is a bug in rev 1.0 of the 1140 that when the RNG
	 * is brought out of reset the ready status flag does not
	 * work until the RNG has finished its internal initialization.
	 *
	 * So in order to determine the device is through its
	 * initialization we must read the data register, using the
	 * status reg in the read in case it is initialized.  Then read
	 * the data register until it changes from the first read.
	 * Once it changes read the data register until it changes
	 * again.  At this time the RNG is considered initialized. 
	 * This could take between 750ms - 1000ms in time.
	 */
	i = 0;
	w = READ_REG(sc, SAFE_RNG_OUT);
	do {
		v = READ_REG(sc, SAFE_RNG_OUT);
		if (v != w) {
			w = v;
			break;
		}
		DELAY(10);
	} while (++i < SAFE_RNG_MAXWAIT);

	/* Wait Until data changes again */
	i = 0;
	do {
		v = READ_REG(sc, SAFE_RNG_OUT);
		if (v != w)
			break;
		DELAY(10);
	} while (++i < SAFE_RNG_MAXWAIT);
}

static __inline void
safe_rng_disable_short_cycle(struct safe_softc *sc)
{
	DPRINTF(("%s()\n", __FUNCTION__));

	WRITE_REG(sc, SAFE_RNG_CTRL,
		READ_REG(sc, SAFE_RNG_CTRL) &~ SAFE_RNG_CTRL_SHORTEN);
}

static __inline void
safe_rng_enable_short_cycle(struct safe_softc *sc)
{
	DPRINTF(("%s()\n", __FUNCTION__));

	WRITE_REG(sc, SAFE_RNG_CTRL, 
		READ_REG(sc, SAFE_RNG_CTRL) | SAFE_RNG_CTRL_SHORTEN);
}

static __inline u_int32_t
safe_rng_read(struct safe_softc *sc)
{
	int i;

	i = 0;
	while (READ_REG(sc, SAFE_RNG_STAT) != 0 && ++i < SAFE_RNG_MAXWAIT)
		;
	return READ_REG(sc, SAFE_RNG_OUT);
}

static int
safe_read_random(void *arg, u_int32_t *buf, int maxwords)
{
	struct safe_softc *sc = (struct safe_softc *) arg;
	int i, rc;

	DPRINTF(("%s()\n", __FUNCTION__));
	
	safestats.st_rng++;
	/*
	 * Fetch the next block of data.
	 */
	if (maxwords > safe_rngbufsize)
		maxwords = safe_rngbufsize;
	if (maxwords > SAFE_RNG_MAXBUFSIZ)
		maxwords = SAFE_RNG_MAXBUFSIZ;
retry:
	/* read as much as we can */
	for (rc = 0; rc < maxwords; rc++) {
		if (READ_REG(sc, SAFE_RNG_STAT) != 0)
			break;
		buf[rc] = READ_REG(sc, SAFE_RNG_OUT);
	}
	if (rc == 0)
		return 0;
	/*
	 * Check the comparator alarm count and reset the h/w if
	 * it exceeds our threshold.  This guards against the
	 * hardware oscillators resonating with external signals.
	 */
	if (READ_REG(sc, SAFE_RNG_ALM_CNT) > safe_rngmaxalarm) {
		u_int32_t freq_inc, w;

		DPRINTF(("%s: alarm count %u exceeds threshold %u\n", __func__,
			(unsigned)READ_REG(sc, SAFE_RNG_ALM_CNT), safe_rngmaxalarm));
		safestats.st_rngalarm++;
		safe_rng_enable_short_cycle(sc);
		freq_inc = 18;
		for (i = 0; i < 64; i++) {
			w = READ_REG(sc, SAFE_RNG_CNFG);
			freq_inc = ((w + freq_inc) & 0x3fL);
			w = ((w & ~0x3fL) | freq_inc);
			WRITE_REG(sc, SAFE_RNG_CNFG, w);

			WRITE_REG(sc, SAFE_RNG_ALM_CNT, 0);

			(void) safe_rng_read(sc);
			DELAY(25);

			if (READ_REG(sc, SAFE_RNG_ALM_CNT) == 0) {
				safe_rng_disable_short_cycle(sc);
				goto retry;
			}
			freq_inc = 1;
		}
		safe_rng_disable_short_cycle(sc);
	} else
		WRITE_REG(sc, SAFE_RNG_ALM_CNT, 0);

	return(rc);
}
#endif /* defined(CONFIG_OCF_RANDOMHARVEST) && !defined(SAFE_NO_RNG) */


/*
 * Resets the board.  Values in the regesters are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
static void
safe_reset_board(struct safe_softc *sc)
{
	u_int32_t v;
	/*
	 * Reset the device.  The manual says no delay
	 * is needed between marking and clearing reset.
	 */
	DPRINTF(("%s()\n", __FUNCTION__));

	v = READ_REG(sc, SAFE_PE_DMACFG) &~
		(SAFE_PE_DMACFG_PERESET | SAFE_PE_DMACFG_PDRRESET |
		 SAFE_PE_DMACFG_SGRESET);
	WRITE_REG(sc, SAFE_PE_DMACFG, v
				    | SAFE_PE_DMACFG_PERESET
				    | SAFE_PE_DMACFG_PDRRESET
				    | SAFE_PE_DMACFG_SGRESET);
	WRITE_REG(sc, SAFE_PE_DMACFG, v);
}

/*
 * Initialize registers we need to touch only once.
 */
static void
safe_init_board(struct safe_softc *sc)
{
	u_int32_t v, dwords;

	DPRINTF(("%s()\n", __FUNCTION__));

	v = READ_REG(sc, SAFE_PE_DMACFG);
	v &=~ (   SAFE_PE_DMACFG_PEMODE
			| SAFE_PE_DMACFG_FSENA		/* failsafe enable */
			| SAFE_PE_DMACFG_GPRPCI		/* gather ring on PCI */
			| SAFE_PE_DMACFG_SPRPCI		/* scatter ring on PCI */
			| SAFE_PE_DMACFG_ESDESC		/* endian-swap descriptors */
			| SAFE_PE_DMACFG_ESPDESC	/* endian-swap part. desc's */
			| SAFE_PE_DMACFG_ESSA		/* endian-swap SA's */
			| SAFE_PE_DMACFG_ESPACKET	/* swap the packet data */
		  );
	v |= SAFE_PE_DMACFG_FSENA		/* failsafe enable */
	  |  SAFE_PE_DMACFG_GPRPCI		/* gather ring on PCI */
	  |  SAFE_PE_DMACFG_SPRPCI		/* scatter ring on PCI */
	  |  SAFE_PE_DMACFG_ESDESC		/* endian-swap descriptors */
	  |  SAFE_PE_DMACFG_ESPDESC		/* endian-swap part. desc's */
	  |  SAFE_PE_DMACFG_ESSA		/* endian-swap SA's */
#if 0
	  |  SAFE_PE_DMACFG_ESPACKET    /* swap the packet data */
#endif
	  ;
	WRITE_REG(sc, SAFE_PE_DMACFG, v);

#ifdef __BIG_ENDIAN
	/* tell the safenet that we are 4321 and not 1234 */
	WRITE_REG(sc, SAFE_ENDIAN, 0xe4e41b1b);
#endif

	if (sc->sc_chiprev == SAFE_REV(1,0)) {
		/*
		 * Avoid large PCI DMA transfers.  Rev 1.0 has a bug where
		 * "target mode transfers" done while the chip is DMA'ing
		 * >1020 bytes cause the hardware to lockup.  To avoid this
		 * we reduce the max PCI transfer size and use small source
		 * particle descriptors (<= 256 bytes).
		 */
		WRITE_REG(sc, SAFE_DMA_CFG, 256);
		device_printf(sc->sc_dev,
			"Reduce max DMA size to %u words for rev %u.%u WAR\n",
			(unsigned) ((READ_REG(sc, SAFE_DMA_CFG)>>2) & 0xff),
			(unsigned) SAFE_REV_MAJ(sc->sc_chiprev),
			(unsigned) SAFE_REV_MIN(sc->sc_chiprev));
		sc->sc_max_dsize = 256;
	} else {
		sc->sc_max_dsize = SAFE_MAX_DSIZE;
	}

	/* NB: operands+results are overlaid */
	WRITE_REG(sc, SAFE_PE_PDRBASE, sc->sc_ringalloc.dma_paddr);
	WRITE_REG(sc, SAFE_PE_RDRBASE, sc->sc_ringalloc.dma_paddr);
	/*
	 * Configure ring entry size and number of items in the ring.
	 */
	KASSERT((sizeof(struct safe_ringentry) % sizeof(u_int32_t)) == 0,
		("PE ring entry not 32-bit aligned!"));
	dwords = sizeof(struct safe_ringentry) / sizeof(u_int32_t);
	WRITE_REG(sc, SAFE_PE_RINGCFG,
		(dwords << SAFE_PE_RINGCFG_OFFSET_S) | SAFE_MAX_NQUEUE);
	WRITE_REG(sc, SAFE_PE_RINGPOLL, 0);	/* disable polling */

	WRITE_REG(sc, SAFE_PE_GRNGBASE, sc->sc_spalloc.dma_paddr);
	WRITE_REG(sc, SAFE_PE_SRNGBASE, sc->sc_dpalloc.dma_paddr);
	WRITE_REG(sc, SAFE_PE_PARTSIZE,
		(SAFE_TOTAL_DPART<<16) | SAFE_TOTAL_SPART);
	/*
	 * NB: destination particles are fixed size.  We use
	 *     an mbuf cluster and require all results go to
	 *     clusters or smaller.
	 */
	WRITE_REG(sc, SAFE_PE_PARTCFG, sc->sc_max_dsize);

	/* it's now safe to enable PE mode, do it */
	WRITE_REG(sc, SAFE_PE_DMACFG, v | SAFE_PE_DMACFG_PEMODE);

	/*
	 * Configure hardware to use level-triggered interrupts and
	 * to interrupt after each descriptor is processed.
	 */
	WRITE_REG(sc, SAFE_HI_CFG, SAFE_HI_CFG_LEVEL);
	WRITE_REG(sc, SAFE_HI_CLR, 0xffffffff);
	WRITE_REG(sc, SAFE_HI_DESC_CNT, 1);
	WRITE_REG(sc, SAFE_HI_MASK, SAFE_INT_PE_DDONE | SAFE_INT_PE_ERROR);
}


/*
 * Clean up after a chip crash.
 * It is assumed that the caller in splimp()
 */
static void
safe_cleanchip(struct safe_softc *sc)
{
	DPRINTF(("%s()\n", __FUNCTION__));

	if (sc->sc_nqchip != 0) {
		struct safe_ringentry *re = sc->sc_back;

		while (re != sc->sc_front) {
			if (re->re_desc.d_csr != 0)
				safe_free_entry(sc, re);
			if (++re == sc->sc_ringtop)
				re = sc->sc_ring;
		}
		sc->sc_back = re;
		sc->sc_nqchip = 0;
	}
}

/*
 * free a safe_q
 * It is assumed that the caller is within splimp().
 */
static int
safe_free_entry(struct safe_softc *sc, struct safe_ringentry *re)
{
	struct cryptop *crp;

	DPRINTF(("%s()\n", __FUNCTION__));

	/*
	 * Free header MCR
	 */
	if ((re->re_dst_skb != NULL) && (re->re_src_skb != re->re_dst_skb))
#ifdef NOTYET
		m_freem(re->re_dst_m);
#else
		printk("%s,%d: SKB not supported\n", __FILE__, __LINE__);
#endif

	crp = (struct cryptop *)re->re_crp;
	
	re->re_desc.d_csr = 0;
	
	crp->crp_etype = EFAULT;
	crypto_done(crp);
	return(0);
}

/*
 * Routine to reset the chip and clean up.
 * It is assumed that the caller is in splimp()
 */
static void
safe_totalreset(struct safe_softc *sc)
{
	DPRINTF(("%s()\n", __FUNCTION__));

	safe_reset_board(sc);
	safe_init_board(sc);
	safe_cleanchip(sc);
}

/*
 * Is the operand suitable aligned for direct DMA.  Each
 * segment must be aligned on a 32-bit boundary and all
 * but the last segment must be a multiple of 4 bytes.
 */
static int
safe_dmamap_aligned(struct safe_softc *sc, const struct safe_operand *op)
{
	int i;

	DPRINTF(("%s()\n", __FUNCTION__));

	for (i = 0; i < op->nsegs; i++) {
		if (op->segs[i].ds_addr & 3)
			return (0);
		if (i != (op->nsegs - 1) && (op->segs[i].ds_len & 3))
			return (0);
	}
	return (1);
}

/*
 * Is the operand suitable for direct DMA as the destination
 * of an operation.  The hardware requires that each ``particle''
 * but the last in an operation result have the same size.  We
 * fix that size at SAFE_MAX_DSIZE bytes.  This routine returns
 * 0 if some segment is not a multiple of of this size, 1 if all
 * segments are exactly this size, or 2 if segments are at worst
 * a multple of this size.
 */
static int
safe_dmamap_uniform(struct safe_softc *sc, const struct safe_operand *op)
{
	int result = 1;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (op->nsegs > 0) {
		int i;

		for (i = 0; i < op->nsegs-1; i++) {
			if (op->segs[i].ds_len % sc->sc_max_dsize)
				return (0);
			if (op->segs[i].ds_len != sc->sc_max_dsize)
				result = 2;
		}
	}
	return (result);
}

static int
safe_kprocess(device_t dev, struct cryptkop *krp, int hint)
{
	struct safe_softc *sc = device_get_softc(dev);
	struct safe_pkq *q;
	unsigned long flags;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (sc == NULL) {
		krp->krp_status = EINVAL;
		goto err;
	}

	if (krp->krp_op != CRK_MOD_EXP) {
		krp->krp_status = EOPNOTSUPP;
		goto err;
	}

	q = (struct safe_pkq *) kmalloc(sizeof(*q), GFP_KERNEL);
	if (q == NULL) {
		krp->krp_status = ENOMEM;
		goto err;
	}
	memset(q, 0, sizeof(*q));
	q->pkq_krp = krp;
	INIT_LIST_HEAD(&q->pkq_list);

	spin_lock_irqsave(&sc->sc_pkmtx, flags);
	list_add_tail(&q->pkq_list, &sc->sc_pkq);
	safe_kfeed(sc);
	spin_unlock_irqrestore(&sc->sc_pkmtx, flags);
	return (0);

err:
	crypto_kdone(krp);
	return (0);
}

#define	SAFE_CRK_PARAM_BASE	0
#define	SAFE_CRK_PARAM_EXP	1
#define	SAFE_CRK_PARAM_MOD	2

static int
safe_kstart(struct safe_softc *sc)
{
	struct cryptkop *krp = sc->sc_pkq_cur->pkq_krp;
	int exp_bits, mod_bits, base_bits;
	u_int32_t op, a_off, b_off, c_off, d_off;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (krp->krp_iparams < 3 || krp->krp_oparams != 1) {
		krp->krp_status = EINVAL;
		return (1);
	}

	base_bits = safe_ksigbits(sc, &krp->krp_param[SAFE_CRK_PARAM_BASE]);
	if (base_bits > 2048)
		goto too_big;
	if (base_bits <= 0)		/* 5. base not zero */
		goto too_small;

	exp_bits = safe_ksigbits(sc, &krp->krp_param[SAFE_CRK_PARAM_EXP]);
	if (exp_bits > 2048)
		goto too_big;
	if (exp_bits <= 0)		/* 1. exponent word length > 0 */
		goto too_small;		/* 4. exponent not zero */

	mod_bits = safe_ksigbits(sc, &krp->krp_param[SAFE_CRK_PARAM_MOD]);
	if (mod_bits > 2048)
		goto too_big;
	if (mod_bits <= 32)		/* 2. modulus word length > 1 */
		goto too_small;		/* 8. MSW of modulus != zero */
	if (mod_bits < exp_bits)	/* 3 modulus len >= exponent len */
		goto too_small;
	if ((krp->krp_param[SAFE_CRK_PARAM_MOD].crp_p[0] & 1) == 0)
		goto bad_domain;	/* 6. modulus is odd */
	if (mod_bits > krp->krp_param[krp->krp_iparams].crp_nbits)
		goto too_small;		/* make sure result will fit */

	/* 7. modulus > base */
	if (mod_bits < base_bits)
		goto too_small;
	if (mod_bits == base_bits) {
		u_int8_t *basep, *modp;
		int i;

		basep = krp->krp_param[SAFE_CRK_PARAM_BASE].crp_p +
		    ((base_bits + 7) / 8) - 1;
		modp = krp->krp_param[SAFE_CRK_PARAM_MOD].crp_p +
		    ((mod_bits + 7) / 8) - 1;
		
		for (i = 0; i < (mod_bits + 7) / 8; i++, basep--, modp--) {
			if (*modp < *basep)
				goto too_small;
			if (*modp > *basep)
				break;
		}
	}

	/* And on the 9th step, he rested. */

	WRITE_REG(sc, SAFE_PK_A_LEN, (exp_bits + 31) / 32);
	WRITE_REG(sc, SAFE_PK_B_LEN, (mod_bits + 31) / 32);
	if (mod_bits > 1024) {
		op = SAFE_PK_FUNC_EXP4;
		a_off = 0x000;
		b_off = 0x100;
		c_off = 0x200;
		d_off = 0x300;
	} else {
		op = SAFE_PK_FUNC_EXP16;
		a_off = 0x000;
		b_off = 0x080;
		c_off = 0x100;
		d_off = 0x180;
	}
	sc->sc_pk_reslen = b_off - a_off;
	sc->sc_pk_resoff = d_off;

	/* A is exponent, B is modulus, C is base, D is result */
	safe_kload_reg(sc, a_off, b_off - a_off,
	    &krp->krp_param[SAFE_CRK_PARAM_EXP]);
	WRITE_REG(sc, SAFE_PK_A_ADDR, a_off >> 2);
	safe_kload_reg(sc, b_off, b_off - a_off,
	    &krp->krp_param[SAFE_CRK_PARAM_MOD]);
	WRITE_REG(sc, SAFE_PK_B_ADDR, b_off >> 2);
	safe_kload_reg(sc, c_off, b_off - a_off,
	    &krp->krp_param[SAFE_CRK_PARAM_BASE]);
	WRITE_REG(sc, SAFE_PK_C_ADDR, c_off >> 2);
	WRITE_REG(sc, SAFE_PK_D_ADDR, d_off >> 2);

	WRITE_REG(sc, SAFE_PK_FUNC, op | SAFE_PK_FUNC_RUN);

	return (0);

too_big:
	krp->krp_status = E2BIG;
	return (1);
too_small:
	krp->krp_status = ERANGE;
	return (1);
bad_domain:
	krp->krp_status = EDOM;
	return (1);
}

static int
safe_ksigbits(struct safe_softc *sc, struct crparam *cr)
{
	u_int plen = (cr->crp_nbits + 7) / 8;
	int i, sig = plen * 8;
	u_int8_t c, *p = cr->crp_p;

	DPRINTF(("%s()\n", __FUNCTION__));

	for (i = plen - 1; i >= 0; i--) {
		c = p[i];
		if (c != 0) {
			while ((c & 0x80) == 0) {
				sig--;
				c <<= 1;
			}
			break;
		}
		sig -= 8;
	}
	return (sig);
}

static void
safe_kfeed(struct safe_softc *sc)
{
	struct safe_pkq *q, *tmp;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (list_empty(&sc->sc_pkq) && sc->sc_pkq_cur == NULL)
		return;
	if (sc->sc_pkq_cur != NULL)
		return;
	list_for_each_entry_safe(q, tmp, &sc->sc_pkq, pkq_list) {
		sc->sc_pkq_cur = q;
		list_del(&q->pkq_list);
		if (safe_kstart(sc) != 0) {
			crypto_kdone(q->pkq_krp);
			kfree(q);
			sc->sc_pkq_cur = NULL;
		} else {
			/* op started, start polling */
			mod_timer(&sc->sc_pkto, jiffies + 1);
			break;
		}
	}
}

static void
safe_kpoll(unsigned long arg)
{
	struct safe_softc *sc = NULL;
	struct safe_pkq *q;
	struct crparam *res;
	int i;
	u_int32_t buf[64];
	unsigned long flags;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (arg >= SAFE_MAX_CHIPS)
		return;
	sc = safe_chip_idx[arg];
	if (!sc) {
		DPRINTF(("%s() - bad callback\n", __FUNCTION__));
		return;
	}

	spin_lock_irqsave(&sc->sc_pkmtx, flags);
	if (sc->sc_pkq_cur == NULL)
		goto out;
	if (READ_REG(sc, SAFE_PK_FUNC) & SAFE_PK_FUNC_RUN) {
		/* still running, check back later */
		mod_timer(&sc->sc_pkto, jiffies + 1);
		goto out;
	}

	q = sc->sc_pkq_cur;
	res = &q->pkq_krp->krp_param[q->pkq_krp->krp_iparams];
	bzero(buf, sizeof(buf));
	bzero(res->crp_p, (res->crp_nbits + 7) / 8);
	for (i = 0; i < sc->sc_pk_reslen >> 2; i++)
		buf[i] = le32_to_cpu(READ_REG(sc, SAFE_PK_RAM_START +
		    sc->sc_pk_resoff + (i << 2)));
	bcopy(buf, res->crp_p, (res->crp_nbits + 7) / 8);
	/*
	 * reduce the bits that need copying if possible
	 */
	res->crp_nbits = min(res->crp_nbits,sc->sc_pk_reslen * 8);
	res->crp_nbits = safe_ksigbits(sc, res);

	for (i = SAFE_PK_RAM_START; i < SAFE_PK_RAM_END; i += 4)
		WRITE_REG(sc, i, 0);

	crypto_kdone(q->pkq_krp);
	kfree(q);
	sc->sc_pkq_cur = NULL;

	safe_kfeed(sc);
out:
	spin_unlock_irqrestore(&sc->sc_pkmtx, flags);
}

static void
safe_kload_reg(struct safe_softc *sc, u_int32_t off, u_int32_t len,
    struct crparam *n)
{
	u_int32_t buf[64], i;

	DPRINTF(("%s()\n", __FUNCTION__));

	bzero(buf, sizeof(buf));
	bcopy(n->crp_p, buf, (n->crp_nbits + 7) / 8);

	for (i = 0; i < len >> 2; i++)
		WRITE_REG(sc, SAFE_PK_RAM_START + off + (i << 2),
		    cpu_to_le32(buf[i]));
}

#ifdef SAFE_DEBUG
static void
safe_dump_dmastatus(struct safe_softc *sc, const char *tag)
{
	printf("%s: ENDIAN 0x%x SRC 0x%x DST 0x%x STAT 0x%x\n"
		, tag
		, READ_REG(sc, SAFE_DMA_ENDIAN)
		, READ_REG(sc, SAFE_DMA_SRCADDR)
		, READ_REG(sc, SAFE_DMA_DSTADDR)
		, READ_REG(sc, SAFE_DMA_STAT)
	);
}

static void
safe_dump_intrstate(struct safe_softc *sc, const char *tag)
{
	printf("%s: HI_CFG 0x%x HI_MASK 0x%x HI_DESC_CNT 0x%x HU_STAT 0x%x HM_STAT 0x%x\n"
		, tag
		, READ_REG(sc, SAFE_HI_CFG)
		, READ_REG(sc, SAFE_HI_MASK)
		, READ_REG(sc, SAFE_HI_DESC_CNT)
		, READ_REG(sc, SAFE_HU_STAT)
		, READ_REG(sc, SAFE_HM_STAT)
	);
}

static void
safe_dump_ringstate(struct safe_softc *sc, const char *tag)
{
	u_int32_t estat = READ_REG(sc, SAFE_PE_ERNGSTAT);

	/* NB: assume caller has lock on ring */
	printf("%s: ERNGSTAT %x (next %u) back %lu front %lu\n",
		tag,
		estat, (estat >> SAFE_PE_ERNGSTAT_NEXT_S),
		(unsigned long)(sc->sc_back - sc->sc_ring),
		(unsigned long)(sc->sc_front - sc->sc_ring));
}

static void
safe_dump_request(struct safe_softc *sc, const char* tag, struct safe_ringentry *re)
{
	int ix, nsegs;

	ix = re - sc->sc_ring;
	printf("%s: %p (%u): csr %x src %x dst %x sa %x len %x\n"
		, tag
		, re, ix
		, re->re_desc.d_csr
		, re->re_desc.d_src
		, re->re_desc.d_dst
		, re->re_desc.d_sa
		, re->re_desc.d_len
	);
	if (re->re_src.nsegs > 1) {
		ix = (re->re_desc.d_src - sc->sc_spalloc.dma_paddr) /
			sizeof(struct safe_pdesc);
		for (nsegs = re->re_src.nsegs; nsegs; nsegs--) {
			printf(" spd[%u] %p: %p size %u flags %x"
				, ix, &sc->sc_spring[ix]
				, (caddr_t)(uintptr_t) sc->sc_spring[ix].pd_addr
				, sc->sc_spring[ix].pd_size
				, sc->sc_spring[ix].pd_flags
			);
			if (sc->sc_spring[ix].pd_size == 0)
				printf(" (zero!)");
			printf("\n");
			if (++ix == SAFE_TOTAL_SPART)
				ix = 0;
		}
	}
	if (re->re_dst.nsegs > 1) {
		ix = (re->re_desc.d_dst - sc->sc_dpalloc.dma_paddr) /
			sizeof(struct safe_pdesc);
		for (nsegs = re->re_dst.nsegs; nsegs; nsegs--) {
			printf(" dpd[%u] %p: %p flags %x\n"
				, ix, &sc->sc_dpring[ix]
				, (caddr_t)(uintptr_t) sc->sc_dpring[ix].pd_addr
				, sc->sc_dpring[ix].pd_flags
			);
			if (++ix == SAFE_TOTAL_DPART)
				ix = 0;
		}
	}
	printf("sa: cmd0 %08x cmd1 %08x staterec %x\n",
		re->re_sa.sa_cmd0, re->re_sa.sa_cmd1, re->re_sa.sa_staterec);
	printf("sa: key %x %x %x %x %x %x %x %x\n"
		, re->re_sa.sa_key[0]
		, re->re_sa.sa_key[1]
		, re->re_sa.sa_key[2]
		, re->re_sa.sa_key[3]
		, re->re_sa.sa_key[4]
		, re->re_sa.sa_key[5]
		, re->re_sa.sa_key[6]
		, re->re_sa.sa_key[7]
	);
	printf("sa: indigest %x %x %x %x %x\n"
		, re->re_sa.sa_indigest[0]
		, re->re_sa.sa_indigest[1]
		, re->re_sa.sa_indigest[2]
		, re->re_sa.sa_indigest[3]
		, re->re_sa.sa_indigest[4]
	);
	printf("sa: outdigest %x %x %x %x %x\n"
		, re->re_sa.sa_outdigest[0]
		, re->re_sa.sa_outdigest[1]
		, re->re_sa.sa_outdigest[2]
		, re->re_sa.sa_outdigest[3]
		, re->re_sa.sa_outdigest[4]
	);
	printf("sr: iv %x %x %x %x\n"
		, re->re_sastate.sa_saved_iv[0]
		, re->re_sastate.sa_saved_iv[1]
		, re->re_sastate.sa_saved_iv[2]
		, re->re_sastate.sa_saved_iv[3]
	);
	printf("sr: hashbc %u indigest %x %x %x %x %x\n"
		, re->re_sastate.sa_saved_hashbc
		, re->re_sastate.sa_saved_indigest[0]
		, re->re_sastate.sa_saved_indigest[1]
		, re->re_sastate.sa_saved_indigest[2]
		, re->re_sastate.sa_saved_indigest[3]
		, re->re_sastate.sa_saved_indigest[4]
	);
}

static void
safe_dump_ring(struct safe_softc *sc, const char *tag)
{
	unsigned long flags;

	spin_lock_irqsave(&sc->sc_ringmtx, flags);
	printf("\nSafeNet Ring State:\n");
	safe_dump_intrstate(sc, tag);
	safe_dump_dmastatus(sc, tag);
	safe_dump_ringstate(sc, tag);
	if (sc->sc_nqchip) {
		struct safe_ringentry *re = sc->sc_back;
		do {
			safe_dump_request(sc, tag, re);
			if (++re == sc->sc_ringtop)
				re = sc->sc_ring;
		} while (re != sc->sc_front);
	}
	spin_unlock_irqrestore(&sc->sc_ringmtx, flags);
}
#endif /* SAFE_DEBUG */


static int safe_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	struct safe_softc *sc = NULL;
	u32 mem_start, mem_len, cmd;
	int i, rc, devinfo;
	dma_addr_t raddr;
	static int num_chips = 0;

	DPRINTF(("%s()\n", __FUNCTION__));

	if (pci_enable_device(dev) < 0)
		return(-ENODEV);

	if (!dev->irq) {
		printk("safe: found device with no IRQ assigned. check BIOS settings!");
		pci_disable_device(dev);
		return(-ENODEV);
	}

#ifdef HAVE_PCI_SET_MWI
	if (pci_set_mwi(dev)) {
		printk("safe: pci_set_mwi failed!");
		return(-ENODEV);
	}
#endif

	sc = (struct safe_softc *) kmalloc(sizeof(*sc), GFP_KERNEL);
	if (!sc)
		return(-ENOMEM);
	memset(sc, 0, sizeof(*sc));

	softc_device_init(sc, "safe", num_chips, safe_methods);

	sc->sc_irq = -1;
	sc->sc_cid = -1;
	sc->sc_pcidev = dev;
	if (num_chips < SAFE_MAX_CHIPS) {
		safe_chip_idx[device_get_unit(sc->sc_dev)] = sc;
		num_chips++;
	}

	INIT_LIST_HEAD(&sc->sc_pkq);
	spin_lock_init(&sc->sc_pkmtx);

	pci_set_drvdata(sc->sc_pcidev, sc);

	/* we read its hardware registers as memory */
	mem_start = pci_resource_start(sc->sc_pcidev, 0);
	mem_len   = pci_resource_len(sc->sc_pcidev, 0);

	sc->sc_base_addr = (ocf_iomem_t) ioremap(mem_start, mem_len);
	if (!sc->sc_base_addr) {
		device_printf(sc->sc_dev, "failed to ioremap 0x%x-0x%x\n",
				mem_start, mem_start + mem_len - 1);
		goto out;
	}

	/* fix up the bus size */
	if (pci_set_dma_mask(sc->sc_pcidev, DMA_32BIT_MASK)) {
		device_printf(sc->sc_dev, "No usable DMA configuration, aborting.\n");
		goto out;
	}
	if (pci_set_consistent_dma_mask(sc->sc_pcidev, DMA_32BIT_MASK)) {
		device_printf(sc->sc_dev, "No usable consistent DMA configuration, aborting.\n");
		goto out;
	}

	pci_set_master(sc->sc_pcidev);

	pci_read_config_dword(sc->sc_pcidev, PCI_COMMAND, &cmd);

	if (!(cmd & PCI_COMMAND_MEMORY)) {
		device_printf(sc->sc_dev, "failed to enable memory mapping\n");
		goto out;
	}

	if (!(cmd & PCI_COMMAND_MASTER)) {
		device_printf(sc->sc_dev, "failed to enable bus mastering\n");
		goto out;
	}

	rc = request_irq(dev->irq, safe_intr, IRQF_SHARED, "safe", sc);
	if (rc) {
		device_printf(sc->sc_dev, "failed to hook irq %d\n", sc->sc_irq);
		goto out;
	}
	sc->sc_irq = dev->irq;

	sc->sc_chiprev = READ_REG(sc, SAFE_DEVINFO) &
			(SAFE_DEVINFO_REV_MAJ | SAFE_DEVINFO_REV_MIN);

	/*
	 * Allocate packet engine descriptors.
	 */
	sc->sc_ringalloc.dma_vaddr = pci_alloc_consistent(sc->sc_pcidev,
			SAFE_MAX_NQUEUE * sizeof (struct safe_ringentry),
			&sc->sc_ringalloc.dma_paddr);
	if (!sc->sc_ringalloc.dma_vaddr) {
		device_printf(sc->sc_dev, "cannot allocate PE descriptor ring\n");
		goto out;
	}

	/*
	 * Hookup the static portion of all our data structures.
	 */
	sc->sc_ring = (struct safe_ringentry *) sc->sc_ringalloc.dma_vaddr;
	sc->sc_ringtop = sc->sc_ring + SAFE_MAX_NQUEUE;
	sc->sc_front = sc->sc_ring;
	sc->sc_back = sc->sc_ring;
	raddr = sc->sc_ringalloc.dma_paddr;
	bzero(sc->sc_ring, SAFE_MAX_NQUEUE * sizeof(struct safe_ringentry));
	for (i = 0; i < SAFE_MAX_NQUEUE; i++) {
		struct safe_ringentry *re = &sc->sc_ring[i];

		re->re_desc.d_sa = raddr +
			offsetof(struct safe_ringentry, re_sa);
		re->re_sa.sa_staterec = raddr +
			offsetof(struct safe_ringentry, re_sastate);

		raddr += sizeof (struct safe_ringentry);
	}
	spin_lock_init(&sc->sc_ringmtx);

	/*
	 * Allocate scatter and gather particle descriptors.
	 */
	sc->sc_spalloc.dma_vaddr = pci_alloc_consistent(sc->sc_pcidev,
			SAFE_TOTAL_SPART * sizeof (struct safe_pdesc),
			&sc->sc_spalloc.dma_paddr);
	if (!sc->sc_spalloc.dma_vaddr) {
		device_printf(sc->sc_dev, "cannot allocate source particle descriptor ring\n");
		goto out;
	}
	sc->sc_spring = (struct safe_pdesc *) sc->sc_spalloc.dma_vaddr;
	sc->sc_springtop = sc->sc_spring + SAFE_TOTAL_SPART;
	sc->sc_spfree = sc->sc_spring;
	bzero(sc->sc_spring, SAFE_TOTAL_SPART * sizeof(struct safe_pdesc));

	sc->sc_dpalloc.dma_vaddr = pci_alloc_consistent(sc->sc_pcidev,
			SAFE_TOTAL_DPART * sizeof (struct safe_pdesc),
			&sc->sc_dpalloc.dma_paddr);
	if (!sc->sc_dpalloc.dma_vaddr) {
		device_printf(sc->sc_dev, "cannot allocate destination particle descriptor ring\n");
		goto out;
	}
	sc->sc_dpring = (struct safe_pdesc *) sc->sc_dpalloc.dma_vaddr;
	sc->sc_dpringtop = sc->sc_dpring + SAFE_TOTAL_DPART;
	sc->sc_dpfree = sc->sc_dpring;
	bzero(sc->sc_dpring, SAFE_TOTAL_DPART * sizeof(struct safe_pdesc));

	sc->sc_cid = crypto_get_driverid(softc_get_device(sc), CRYPTOCAP_F_HARDWARE);
	if (sc->sc_cid < 0) {
		device_printf(sc->sc_dev, "could not get crypto driver id\n");
		goto out;
	}

	printf("%s:", device_get_nameunit(sc->sc_dev));

	devinfo = READ_REG(sc, SAFE_DEVINFO);
	if (devinfo & SAFE_DEVINFO_RNG) {
		sc->sc_flags |= SAFE_FLAGS_RNG;
		printf(" rng");
	}
	if (devinfo & SAFE_DEVINFO_PKEY) {
		printf(" key");
		sc->sc_flags |= SAFE_FLAGS_KEY;
		crypto_kregister(sc->sc_cid, CRK_MOD_EXP, 0);
#if 0
		crypto_kregister(sc->sc_cid, CRK_MOD_EXP_CRT, 0);
#endif
		init_timer(&sc->sc_pkto);
		sc->sc_pkto.function = safe_kpoll;
		sc->sc_pkto.data = (unsigned long) device_get_unit(sc->sc_dev);
	}
	if (devinfo & SAFE_DEVINFO_DES) {
		printf(" des/3des");
		crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0);
	}
	if (devinfo & SAFE_DEVINFO_AES) {
		printf(" aes");
		crypto_register(sc->sc_cid, CRYPTO_AES_CBC, 0, 0);
	}
	if (devinfo & SAFE_DEVINFO_MD5) {
		printf(" md5");
		crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0);
	}
	if (devinfo & SAFE_DEVINFO_SHA1) {
		printf(" sha1");
		crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0);
	}
	printf(" null");
	crypto_register(sc->sc_cid, CRYPTO_NULL_CBC, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_NULL_HMAC, 0, 0);
	/* XXX other supported algorithms */
	printf("\n");

	safe_reset_board(sc);		/* reset h/w */
	safe_init_board(sc);		/* init h/w */

#if defined(CONFIG_OCF_RANDOMHARVEST) && !defined(SAFE_NO_RNG)
	if (sc->sc_flags & SAFE_FLAGS_RNG) {
		safe_rng_init(sc);
		crypto_rregister(sc->sc_cid, safe_read_random, sc);
	}
#endif /* SAFE_NO_RNG */

	return (0);

out:
	if (sc->sc_cid >= 0)
		crypto_unregister_all(sc->sc_cid);
	if (sc->sc_irq != -1)
		free_irq(sc->sc_irq, sc);
	if (sc->sc_ringalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_MAX_NQUEUE * sizeof (struct safe_ringentry),
				sc->sc_ringalloc.dma_vaddr, sc->sc_ringalloc.dma_paddr);
	if (sc->sc_spalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_TOTAL_DPART * sizeof (struct safe_pdesc),
				sc->sc_spalloc.dma_vaddr, sc->sc_spalloc.dma_paddr);
	if (sc->sc_dpalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_TOTAL_DPART * sizeof (struct safe_pdesc),
				sc->sc_dpalloc.dma_vaddr, sc->sc_dpalloc.dma_paddr);
	kfree(sc);
	return(-ENODEV);
}

static void safe_remove(struct pci_dev *dev)
{
	struct safe_softc *sc = pci_get_drvdata(dev);

	DPRINTF(("%s()\n", __FUNCTION__));

	/* XXX wait/abort active ops */

	WRITE_REG(sc, SAFE_HI_MASK, 0);		/* disable interrupts */

	del_timer_sync(&sc->sc_pkto);

	crypto_unregister_all(sc->sc_cid);

	safe_cleanchip(sc);

	if (sc->sc_irq != -1)
		free_irq(sc->sc_irq, sc);
	if (sc->sc_ringalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_MAX_NQUEUE * sizeof (struct safe_ringentry),
				sc->sc_ringalloc.dma_vaddr, sc->sc_ringalloc.dma_paddr);
	if (sc->sc_spalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_TOTAL_DPART * sizeof (struct safe_pdesc),
				sc->sc_spalloc.dma_vaddr, sc->sc_spalloc.dma_paddr);
	if (sc->sc_dpalloc.dma_vaddr)
		pci_free_consistent(sc->sc_pcidev,
				SAFE_TOTAL_DPART * sizeof (struct safe_pdesc),
				sc->sc_dpalloc.dma_vaddr, sc->sc_dpalloc.dma_paddr);
	sc->sc_irq = -1;
	sc->sc_ringalloc.dma_vaddr = NULL;
	sc->sc_spalloc.dma_vaddr = NULL;
	sc->sc_dpalloc.dma_vaddr = NULL;
}

static struct pci_device_id safe_pci_tbl[] = {
	{ PCI_VENDOR_SAFENET, PCI_PRODUCT_SAFEXCEL,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ },
};
MODULE_DEVICE_TABLE(pci, safe_pci_tbl);

static struct pci_driver safe_driver = {
	.name         = "safe",
	.id_table     = safe_pci_tbl,
	.probe        =	safe_probe,
	.remove       = safe_remove,
	/* add PM stuff here one day */
};

static int __init safe_init (void)
{
	struct safe_softc *sc = NULL;
	int rc;

	DPRINTF(("%s(%p)\n", __FUNCTION__, safe_init));

	rc = pci_register_driver(&safe_driver);
	pci_register_driver_compat(&safe_driver, rc);

	return rc;
}

static void __exit safe_exit (void)
{
	pci_unregister_driver(&safe_driver);
}

module_init(safe_init);
module_exit(safe_exit);

MODULE_LICENSE("BSD");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("OCF driver for safenet PCI crypto devices");
