/*
 * crypto/ocf/talitos/talitos.c
 *
 * An OCF-Linux module that uses Freescale's SEC to do the crypto.
 * Based on crypto/ocf/hifn and crypto/ocf/safe OCF drivers
 *
 * Copyright (c) 2006 Freescale Semiconductor, Inc.
 *
 * This code written by Kim A. B. Phillips <kim.phillips@freescale.com>
 * some code copied from files with the following:
 * Copyright (C) 2004-2007 David McCullough <david_mccullough@mcafee.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------------
 *
 * NOTES:
 *
 * The Freescale SEC (also known as 'talitos') resides on the
 * internal bus, and runs asynchronous to the processor core.  It has
 * a wide gamut of cryptographic acceleration features, including single-
 * pass IPsec (also known as algorithm chaining).  To properly utilize 
 * all of the SEC's performance enhancing features, further reworking 
 * of higher level code (framework, applications) will be necessary.
 *
 * The following table shows which SEC version is present in which devices:
 * 
 * Devices       SEC version
 *
 * 8272, 8248    SEC 1.0
 * 885, 875      SEC 1.2
 * 8555E, 8541E  SEC 2.0
 * 8349E         SEC 2.01
 * 8548E         SEC 2.1
 *
 * The following table shows the features offered by each SEC version:
 *
 * 	                       Max.   chan-
 * version  Bus I/F       Clock  nels  DEU AESU AFEU MDEU PKEU RNG KEU
 *
 * SEC 1.0  internal 64b  100MHz   4     1    1    1    1    1   1   0
 * SEC 1.2  internal 32b   66MHz   1     1    1    0    1    0   0   0
 * SEC 2.0  internal 64b  166MHz   4     1    1    1    1    1   1   0
 * SEC 2.01 internal 64b  166MHz   4     1    1    1    1    1   1   0
 * SEC 2.1  internal 64b  333MHz   4     1    1    1    1    1   1   1
 *
 * Each execution unit in the SEC has two modes of execution; channel and
 * slave/debug.  This driver employs the channel infrastructure in the
 * device for convenience.  Only the RNG is directly accessed due to the
 * convenience of its random fifo pool.  The relationship between the
 * channels and execution units is depicted in the following diagram:
 *
 *    -------   ------------
 * ---| ch0 |---|          |
 *    -------   |          |
 *              |          |------+-------+-------+-------+------------
 *    -------   |          |      |       |       |       |           |
 * ---| ch1 |---|          |      |       |       |       |           |
 *    -------   |          |   ------  ------  ------  ------      ------
 *              |controller|   |DEU |  |AESU|  |MDEU|  |PKEU| ...  |RNG |
 *    -------   |          |   ------  ------  ------  ------      ------
 * ---| ch2 |---|          |      |       |       |       |           |
 *    -------   |          |      |       |       |       |           |
 *              |          |------+-------+-------+-------+------------
 *    -------   |          |
 * ---| ch3 |---|          |
 *    -------   ------------
 *
 * Channel ch0 may drive an aes operation to the aes unit (AESU),
 * and, at the same time, ch1 may drive a message digest operation
 * to the mdeu. Each channel has an input descriptor FIFO, and the 
 * FIFO can contain, e.g. on the 8541E, up to 24 entries, before a
 * a buffer overrun error is triggered. The controller is responsible
 * for fetching the data from descriptor pointers, and passing the 
 * data to the appropriate EUs. The controller also writes the 
 * cryptographic operation's result to memory. The SEC notifies 
 * completion by triggering an interrupt and/or setting the 1st byte 
 * of the hdr field to 0xff.
 *
 * TODO:
 * o support more algorithms
 * o support more versions of the SEC
 * o add support for linux 2.4
 * o scatter-gather (sg) support
 * o add support for public key ops (PKEU)
 * o add statistics
 */

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <asm/scatterlist.h>
#include <linux/dma-mapping.h>  /* dma_map_single() */
#include <linux/moduleparam.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15)
#include <linux/platform_device.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
#include <linux/of_platform.h>
#endif

#include <cryptodev.h>
#include <uio.h>

#define DRV_NAME "talitos" 

#include "talitos_dev.h"
#include "talitos_soft.h"

#define read_random(p,l) get_random_bytes(p,l)

const char talitos_driver_name[] = "Talitos OCF";
const char talitos_driver_version[] = "0.2";

static int talitos_newsession(device_t dev, u_int32_t *sidp,
								struct cryptoini *cri);
static int talitos_freesession(device_t dev, u_int64_t tid);
static int talitos_process(device_t dev, struct cryptop *crp, int hint);
static void dump_talitos_status(struct talitos_softc *sc);
static int talitos_submit(struct talitos_softc *sc, struct talitos_desc *td, 
								int chsel);
static void talitos_doneprocessing(struct talitos_softc *sc);
static void talitos_init_device(struct talitos_softc *sc);
static void talitos_reset_device_master(struct talitos_softc *sc);
static void talitos_reset_device(struct talitos_softc *sc);
static void talitos_errorprocessing(struct talitos_softc *sc);
#ifdef CONFIG_PPC_MERGE
static int talitos_probe(struct of_device *ofdev, const struct of_device_id *match);
static int talitos_remove(struct of_device *ofdev);
#else
static int talitos_probe(struct platform_device *pdev);
static int talitos_remove(struct platform_device *pdev);
#endif
#ifdef CONFIG_OCF_RANDOMHARVEST
static int talitos_read_random(void *arg, u_int32_t *buf, int maxwords);
static void talitos_rng_init(struct talitos_softc *sc);
#endif

static device_method_t talitos_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	talitos_newsession),
	DEVMETHOD(cryptodev_freesession,talitos_freesession),
	DEVMETHOD(cryptodev_process,	talitos_process),
};

#define debug talitos_debug
int talitos_debug = 0;
module_param(talitos_debug, int, 0644);
MODULE_PARM_DESC(talitos_debug, "Enable debug");

static inline void talitos_write(volatile unsigned *addr, u32 val)
{
        out_be32(addr, val);
}

static inline u32 talitos_read(volatile unsigned *addr)
{
        u32 val;
        val = in_be32(addr);
        return val;
}

static void dump_talitos_status(struct talitos_softc *sc)
{
	unsigned int v, v_hi, i, *ptr;
	v = talitos_read(sc->sc_base_addr + TALITOS_MCR);
	v_hi = talitos_read(sc->sc_base_addr + TALITOS_MCR_HI);
	printk(KERN_INFO "%s: MCR          0x%08x_%08x\n",
			device_get_nameunit(sc->sc_cdev), v, v_hi);
	v = talitos_read(sc->sc_base_addr + TALITOS_IMR);
	v_hi = talitos_read(sc->sc_base_addr + TALITOS_IMR_HI);
	printk(KERN_INFO "%s: IMR          0x%08x_%08x\n",
			device_get_nameunit(sc->sc_cdev), v, v_hi);
	v = talitos_read(sc->sc_base_addr + TALITOS_ISR);
	v_hi = talitos_read(sc->sc_base_addr + TALITOS_ISR_HI);
	printk(KERN_INFO "%s: ISR          0x%08x_%08x\n",
			device_get_nameunit(sc->sc_cdev), v, v_hi);
	for (i = 0; i < sc->sc_num_channels; i++) { 
		v = talitos_read(sc->sc_base_addr + i*TALITOS_CH_OFFSET + 
			TALITOS_CH_CDPR);
		v_hi = talitos_read(sc->sc_base_addr + i*TALITOS_CH_OFFSET + 
			TALITOS_CH_CDPR_HI);
		printk(KERN_INFO "%s: CDPR     ch%d 0x%08x_%08x\n", 
				device_get_nameunit(sc->sc_cdev), i, v, v_hi);
	}
	for (i = 0; i < sc->sc_num_channels; i++) { 
		v = talitos_read(sc->sc_base_addr + i*TALITOS_CH_OFFSET + 
			TALITOS_CH_CCPSR);
		v_hi = talitos_read(sc->sc_base_addr + i*TALITOS_CH_OFFSET + 
			TALITOS_CH_CCPSR_HI);
		printk(KERN_INFO "%s: CCPSR    ch%d 0x%08x_%08x\n", 
				device_get_nameunit(sc->sc_cdev), i, v, v_hi);
	}
	ptr = sc->sc_base_addr + TALITOS_CH_DESCBUF;
	for (i = 0; i < 16; i++) { 
		v = talitos_read(ptr++); v_hi = talitos_read(ptr++);
		printk(KERN_INFO "%s: DESCBUF  ch0 0x%08x_%08x (tdp%02d)\n", 
				device_get_nameunit(sc->sc_cdev), v, v_hi, i);
	}
	return;
}


#ifdef CONFIG_OCF_RANDOMHARVEST
/* 
 * pull random numbers off the RNG FIFO, not exceeding amount available
 */
static int
talitos_read_random(void *arg, u_int32_t *buf, int maxwords)
{
	struct talitos_softc *sc = (struct talitos_softc *) arg;
	int rc;
	u_int32_t v;

	DPRINTF("%s()\n", __FUNCTION__);

	/* check for things like FIFO underflow */
	v = talitos_read(sc->sc_base_addr + TALITOS_RNGISR_HI);
	if (unlikely(v)) {
		printk(KERN_ERR "%s: RNGISR_HI error %08x\n",
				device_get_nameunit(sc->sc_cdev), v);
		return 0;
	}
	/*
	 * OFL is number of available 64-bit words, 
	 * shift and convert to a 32-bit word count
	 */
	v = talitos_read(sc->sc_base_addr + TALITOS_RNGSR_HI);
	v = (v & TALITOS_RNGSR_HI_OFL) >> (16 - 1);
	if (maxwords > v)
		maxwords = v;
	for (rc = 0; rc < maxwords; rc++) {
		buf[rc] = talitos_read(sc->sc_base_addr + 
			TALITOS_RNG_FIFO + rc*sizeof(u_int32_t));
	}
	if (maxwords & 1) {
		/* 
		 * RNG will complain with an AE in the RNGISR
		 * if we don't complete the pairs of 32-bit reads
		 * to its 64-bit register based FIFO
		 */
		v = talitos_read(sc->sc_base_addr + 
			TALITOS_RNG_FIFO + rc*sizeof(u_int32_t));
	}

	return rc;
}

static void
talitos_rng_init(struct talitos_softc *sc)
{
	u_int32_t v;

	DPRINTF("%s()\n", __FUNCTION__);
	/* reset RNG EU */
	v = talitos_read(sc->sc_base_addr + TALITOS_RNGRCR_HI);
	v |= TALITOS_RNGRCR_HI_SR;
	talitos_write(sc->sc_base_addr + TALITOS_RNGRCR_HI, v);
	while ((talitos_read(sc->sc_base_addr + TALITOS_RNGSR_HI) 
		& TALITOS_RNGSR_HI_RD) == 0)
			cpu_relax();
	/*
	 * we tell the RNG to start filling the RNG FIFO
	 * by writing the RNGDSR 
	 */
	v = talitos_read(sc->sc_base_addr + TALITOS_RNGDSR_HI);
	talitos_write(sc->sc_base_addr + TALITOS_RNGDSR_HI, v);
	/*
	 * 64 bits of data will be pushed onto the FIFO every 
	 * 256 SEC cycles until the FIFO is full.  The RNG then 
	 * attempts to keep the FIFO full.
	 */
	v = talitos_read(sc->sc_base_addr + TALITOS_RNGISR_HI);
	if (v) {
		printk(KERN_ERR "%s: RNGISR_HI error %08x\n",
			device_get_nameunit(sc->sc_cdev), v);
		return;
	}
	/*
	 * n.b. we need to add a FIPS test here - if the RNG is going 
	 * to fail, it's going to fail at reset time
	 */
	return;
}
#endif /* CONFIG_OCF_RANDOMHARVEST */

/*
 * Generate a new software session.
 */
static int
talitos_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
	struct cryptoini *c, *encini = NULL, *macini = NULL;
	struct talitos_softc *sc = device_get_softc(dev);
	struct talitos_session *ses = NULL;
	int sesn;

	DPRINTF("%s()\n", __FUNCTION__);
	if (sidp == NULL || cri == NULL || sc == NULL) {
		DPRINTF("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}
	for (c = cri; c != NULL; c = c->cri_next) {
		if (c->cri_alg == CRYPTO_MD5 ||
		    c->cri_alg == CRYPTO_MD5_HMAC ||
		    c->cri_alg == CRYPTO_SHA1 ||
		    c->cri_alg == CRYPTO_SHA1_HMAC ||
		    c->cri_alg == CRYPTO_NULL_HMAC) {
			if (macini)
				return EINVAL;
			macini = c;
		} else if (c->cri_alg == CRYPTO_DES_CBC ||
		    c->cri_alg == CRYPTO_3DES_CBC ||
		    c->cri_alg == CRYPTO_AES_CBC ||
		    c->cri_alg == CRYPTO_NULL_CBC) {
			if (encini)
				return EINVAL;
			encini = c;
		} else {
			DPRINTF("UNKNOWN c->cri_alg %d\n", encini->cri_alg);
			return EINVAL;
		}
	}
	if (encini == NULL && macini == NULL)
		return EINVAL;
	if (encini) {	
		/* validate key length */
		switch (encini->cri_alg) {
		case CRYPTO_DES_CBC:
			if (encini->cri_klen != 64)
				return EINVAL;
			break;
		case CRYPTO_3DES_CBC:
			if (encini->cri_klen != 192) {
				return EINVAL;
			}
			break;
		case CRYPTO_AES_CBC:
			if (encini->cri_klen != 128 &&
			    encini->cri_klen != 192 &&
			    encini->cri_klen != 256)
				return EINVAL;
			break;
		default:
			DPRINTF("UNKNOWN encini->cri_alg %d\n", 
				encini->cri_alg);
			return EINVAL;
		}
	}

	if (sc->sc_sessions == NULL) {
		ses = sc->sc_sessions = (struct talitos_session *)
			kmalloc(sizeof(struct talitos_session), SLAB_ATOMIC);
		if (ses == NULL)
			return ENOMEM;
		memset(ses, 0, sizeof(struct talitos_session));
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
			/* allocating session */
			sesn = sc->sc_nsessions;
			ses = (struct talitos_session *) kmalloc(
				(sesn + 1) * sizeof(struct talitos_session), 
				SLAB_ATOMIC);
			if (ses == NULL)
				return ENOMEM;
			memset(ses, 0,
				(sesn + 1) * sizeof(struct talitos_session));
			memcpy(ses, sc->sc_sessions, 
				sesn * sizeof(struct talitos_session));
			memset(sc->sc_sessions, 0,
				sesn * sizeof(struct talitos_session));
			kfree(sc->sc_sessions);
			sc->sc_sessions = ses;
			ses = &sc->sc_sessions[sesn];
			sc->sc_nsessions++;
		}
	}

	ses->ses_used = 1;

	if (encini) {
		/* get an IV */
		/* XXX may read fewer than requested */
		read_random(ses->ses_iv, sizeof(ses->ses_iv));

		ses->ses_klen = (encini->cri_klen + 7) / 8;
		memcpy(ses->ses_key, encini->cri_key, ses->ses_klen);
		if (macini) {
			/* doing hash on top of cipher */
			ses->ses_hmac_len = (macini->cri_klen + 7) / 8;
			memcpy(ses->ses_hmac, macini->cri_key,
				ses->ses_hmac_len);
		}
	} else if (macini) {
		/* doing hash */
		ses->ses_klen = (macini->cri_klen + 7) / 8;
		memcpy(ses->ses_key, macini->cri_key, ses->ses_klen);
	}

	/* back compat way of determining MSC result len */
	if (macini) {
		ses->ses_mlen = macini->cri_mlen;
		if (ses->ses_mlen == 0) {
			if (macini->cri_alg == CRYPTO_MD5_HMAC)
				ses->ses_mlen = MD5_HASH_LEN;
			else
				ses->ses_mlen = SHA1_HASH_LEN;
		}
	}

	/* really should make up a template td here, 
	 * and only fill things like i/o and direction in process() */

	/* assign session ID */
	*sidp = TALITOS_SID(sc->sc_num, sesn);
	return 0;
}

/*
 * Deallocate a session.
 */
static int
talitos_freesession(device_t dev, u_int64_t tid)
{
	struct talitos_softc *sc = device_get_softc(dev);
	int session, ret;
	u_int32_t sid = ((u_int32_t) tid) & 0xffffffff;

	if (sc == NULL)
		return EINVAL;
	session = TALITOS_SESSION(sid);
	if (session < sc->sc_nsessions) {
		memset(&sc->sc_sessions[session], 0,
			sizeof(sc->sc_sessions[session]));
		ret = 0;
	} else
		ret = EINVAL;
	return ret;
}

/*
 * launch device processing - it will come back with done notification 
 * in the form of an interrupt and/or HDR_DONE_BITS in header 
 */
static int 
talitos_submit(
	struct talitos_softc *sc,
	struct talitos_desc *td,
	int chsel)
{
	u_int32_t v;

	v = dma_map_single(NULL, td, sizeof(*td), DMA_TO_DEVICE);
	talitos_write(sc->sc_base_addr + 
		chsel*TALITOS_CH_OFFSET + TALITOS_CH_FF, 0);
	talitos_write(sc->sc_base_addr + 
		chsel*TALITOS_CH_OFFSET + TALITOS_CH_FF_HI, v);
	return 0;
}

static int
talitos_process(device_t dev, struct cryptop *crp, int hint)
{
	int i, err = 0, ivsize;
	struct talitos_softc *sc = device_get_softc(dev);
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;
	caddr_t iv;
	struct talitos_session *ses;
	struct talitos_desc *td;
	unsigned long flags;
	/* descriptor mappings */
	int hmac_key, hmac_data, cipher_iv, cipher_key, 
		in_fifo, out_fifo, cipher_iv_out;
	static int chsel = -1;

	DPRINTF("%s()\n", __FUNCTION__);

	if (crp == NULL || crp->crp_callback == NULL || sc == NULL) {
		return EINVAL;
	}
	crp->crp_etype = 0;
	if (TALITOS_SESSION(crp->crp_sid) >= sc->sc_nsessions) {
		return EINVAL;
	}

	ses = &sc->sc_sessions[TALITOS_SESSION(crp->crp_sid)];

        /* enter the channel scheduler */ 
	spin_lock_irqsave(&sc->sc_chnfifolock[sc->sc_num_channels], flags);

	/* reuse channel that already had/has requests for the required EU */
	for (i = 0; i < sc->sc_num_channels; i++) {
		if (sc->sc_chnlastalg[i] == crp->crp_desc->crd_alg)
			break;
	}
	if (i == sc->sc_num_channels) {
		/*
		 * haven't seen this algo the last sc_num_channels or more
		 * use round robin in this case
	 	 * nb: sc->sc_num_channels must be power of 2 
		 */
		chsel = (chsel + 1) & (sc->sc_num_channels - 1);
	} else {
		/*
		 * matches channel with same target execution unit; 
		 * use same channel in this case
		 */
		chsel = i;
	}
	sc->sc_chnlastalg[chsel] = crp->crp_desc->crd_alg;

        /* release the channel scheduler lock */ 
	spin_unlock_irqrestore(&sc->sc_chnfifolock[sc->sc_num_channels], flags);

	/* acquire the selected channel fifo lock */
	spin_lock_irqsave(&sc->sc_chnfifolock[chsel], flags);

	/* find and reserve next available descriptor-cryptop pair */
	for (i = 0; i < sc->sc_chfifo_len; i++) {
		if (sc->sc_chnfifo[chsel][i].cf_desc.hdr == 0) {
			/* 
			 * ensure correct descriptor formation by
			 * avoiding inadvertently setting "optional" entries
			 * e.g. not using "optional" dptr2 for MD/HMAC descs
			 */
			memset(&sc->sc_chnfifo[chsel][i].cf_desc,
				0, sizeof(*td));
			/* reserve it with done notification request bit */
			sc->sc_chnfifo[chsel][i].cf_desc.hdr |= 
				TALITOS_DONE_NOTIFY;
			break;
		}
	}
	spin_unlock_irqrestore(&sc->sc_chnfifolock[chsel], flags);

	if (i == sc->sc_chfifo_len) {
		/* fifo full */
		err = ERESTART;
		goto errout;
	}
	
	td = &sc->sc_chnfifo[chsel][i].cf_desc;
	sc->sc_chnfifo[chsel][i].cf_crp = crp;

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		err = EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;
	/* prevent compiler warning */
	hmac_key = 0;
	hmac_data = 0;
	if (crd2 == NULL) {
		td->hdr |= TD_TYPE_COMMON_NONSNOOP_NO_AFEU;
		/* assign descriptor dword ptr mappings for this desc. type */
		cipher_iv = 1;
		cipher_key = 2;
		in_fifo = 3;
		cipher_iv_out = 5;
		if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1 ||
		    crd1->crd_alg == CRYPTO_MD5) {
			out_fifo = 5;
			maccrd = crd1;
			enccrd = NULL;
		} else if (crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC ||
		    crd1->crd_alg == CRYPTO_AES_CBC ||
		    crd1->crd_alg == CRYPTO_ARC4) {
			out_fifo = 4;
			maccrd = NULL;
			enccrd = crd1;
		} else {
			DPRINTF("UNKNOWN crd1->crd_alg %d\n", crd1->crd_alg);
			err = EINVAL;
			goto errout;
		}
	} else {
		if (sc->sc_desc_types & TALITOS_HAS_DT_IPSEC_ESP) {
			td->hdr |= TD_TYPE_IPSEC_ESP;
		} else {
			DPRINTF("unimplemented: multiple descriptor ipsec\n");
			err = EINVAL;
			goto errout;
		}
		/* assign descriptor dword ptr mappings for this desc. type */
		hmac_key = 0;
		hmac_data = 1;
		cipher_iv = 2;
		cipher_key = 3;
		in_fifo = 4;
		out_fifo = 5;
		cipher_iv_out = 6;
		if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
                     crd1->crd_alg == CRYPTO_SHA1_HMAC ||
                     crd1->crd_alg == CRYPTO_MD5 ||
                     crd1->crd_alg == CRYPTO_SHA1) &&
		    (crd2->crd_alg == CRYPTO_DES_CBC ||
		     crd2->crd_alg == CRYPTO_3DES_CBC ||
		     crd2->crd_alg == CRYPTO_AES_CBC ||
		     crd2->crd_alg == CRYPTO_ARC4) &&
		    ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
			maccrd = crd1;
			enccrd = crd2;
		} else if ((crd1->crd_alg == CRYPTO_DES_CBC ||
		     crd1->crd_alg == CRYPTO_ARC4 ||
		     crd1->crd_alg == CRYPTO_3DES_CBC ||
		     crd1->crd_alg == CRYPTO_AES_CBC) &&
		    (crd2->crd_alg == CRYPTO_MD5_HMAC ||
                     crd2->crd_alg == CRYPTO_SHA1_HMAC ||
                     crd2->crd_alg == CRYPTO_MD5 ||
                     crd2->crd_alg == CRYPTO_SHA1) &&
		    (crd1->crd_flags & CRD_F_ENCRYPT)) {
			enccrd = crd1;
			maccrd = crd2;
		} else {
			/* We cannot order the SEC as requested */
			printk("%s: cannot do the order\n",
					device_get_nameunit(sc->sc_cdev));
			err = EINVAL;
			goto errout;
		}
	}
	/* assign in_fifo and out_fifo based on input/output struct type */
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		/* using SKB buffers */
		struct sk_buff *skb = (struct sk_buff *)crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags) {
			printk("%s: skb frags unimplemented\n",
					device_get_nameunit(sc->sc_cdev));
			err = EINVAL;
			goto errout;
		}
		td->ptr[in_fifo].ptr = dma_map_single(NULL, skb->data, 
			skb->len, DMA_TO_DEVICE);
		td->ptr[in_fifo].len = skb->len;
		td->ptr[out_fifo].ptr = dma_map_single(NULL, skb->data, 
			skb->len, DMA_TO_DEVICE);
		td->ptr[out_fifo].len = skb->len;
		td->ptr[hmac_data].ptr = dma_map_single(NULL, skb->data,
			skb->len, DMA_TO_DEVICE);
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		/* using IOV buffers */
		struct uio *uiop = (struct uio *)crp->crp_buf;
		if (uiop->uio_iovcnt > 1) {
			printk("%s: iov frags unimplemented\n",
					device_get_nameunit(sc->sc_cdev));
			err = EINVAL;
			goto errout;
		}
		td->ptr[in_fifo].ptr = dma_map_single(NULL,
			uiop->uio_iov->iov_base, crp->crp_ilen, DMA_TO_DEVICE);
		td->ptr[in_fifo].len = crp->crp_ilen;
		/* crp_olen is never set; always use crp_ilen */
		td->ptr[out_fifo].ptr = dma_map_single(NULL,
			uiop->uio_iov->iov_base,
			crp->crp_ilen, DMA_TO_DEVICE);
		td->ptr[out_fifo].len = crp->crp_ilen;
	} else {
		/* using contig buffers */
		td->ptr[in_fifo].ptr = dma_map_single(NULL,
			crp->crp_buf, crp->crp_ilen, DMA_TO_DEVICE);
		td->ptr[in_fifo].len = crp->crp_ilen;
		td->ptr[out_fifo].ptr = dma_map_single(NULL,
			crp->crp_buf, crp->crp_ilen, DMA_TO_DEVICE);
		td->ptr[out_fifo].len = crp->crp_ilen;
	}
	if (enccrd) {
		switch (enccrd->crd_alg) {
		case CRYPTO_3DES_CBC:
			td->hdr |= TALITOS_MODE0_DEU_3DES;
			/* FALLTHROUGH */
		case CRYPTO_DES_CBC:
			td->hdr |= TALITOS_SEL0_DEU
				|  TALITOS_MODE0_DEU_CBC;
			if (enccrd->crd_flags & CRD_F_ENCRYPT)
				td->hdr |= TALITOS_MODE0_DEU_ENC;
			ivsize = 2*sizeof(u_int32_t);
			DPRINTF("%cDES ses %d ch %d len %d\n",
				(td->hdr & TALITOS_MODE0_DEU_3DES)?'3':'1',
				(u32)TALITOS_SESSION(crp->crp_sid),
				chsel, td->ptr[in_fifo].len);
			break;
		case CRYPTO_AES_CBC:
			td->hdr |= TALITOS_SEL0_AESU
				|  TALITOS_MODE0_AESU_CBC;
			if (enccrd->crd_flags & CRD_F_ENCRYPT)
				td->hdr |= TALITOS_MODE0_AESU_ENC;
			ivsize = 4*sizeof(u_int32_t);
			DPRINTF("AES  ses %d ch %d len %d\n",
				(u32)TALITOS_SESSION(crp->crp_sid),
				chsel, td->ptr[in_fifo].len);
			break;
		default:
			printk("%s: unimplemented enccrd->crd_alg %d\n",
					device_get_nameunit(sc->sc_cdev), enccrd->crd_alg);
			err = EINVAL;
			goto errout;
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
			td->hdr |= TALITOS_DIR_OUTBOUND; 
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				iv = enccrd->crd_iv;
			else
				iv = (caddr_t) ses->ses_iv;
			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				crypto_copyback(crp->crp_flags, crp->crp_buf,
				    enccrd->crd_inject, ivsize, iv);
			}
		} else {
			td->hdr |= TALITOS_DIR_INBOUND; 
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT) {
				iv = enccrd->crd_iv;
				bcopy(enccrd->crd_iv, iv, ivsize);
			} else {
				iv = (caddr_t) ses->ses_iv;
				crypto_copydata(crp->crp_flags, crp->crp_buf,
				    enccrd->crd_inject, ivsize, iv);
			}
		}
		td->ptr[cipher_iv].ptr = dma_map_single(NULL, iv, ivsize, 
			DMA_TO_DEVICE);
		td->ptr[cipher_iv].len = ivsize;
		/*
		 * we don't need the cipher iv out length/pointer
		 * field to do ESP IPsec. Therefore we set the len field as 0,
		 * which tells the SEC not to do anything with this len/ptr
		 * field. Previously, when length/pointer as pointing to iv,
		 * it gave us corruption of packets.
		 */
		td->ptr[cipher_iv_out].len = 0;
	}
	if (enccrd && maccrd) {
		/* this is ipsec only for now */
		td->hdr |= TALITOS_SEL1_MDEU
			|  TALITOS_MODE1_MDEU_INIT
			|  TALITOS_MODE1_MDEU_PAD;
		switch (maccrd->crd_alg) {
			case	CRYPTO_MD5:	
				td->hdr |= TALITOS_MODE1_MDEU_MD5;
				break;
			case	CRYPTO_MD5_HMAC:	
				td->hdr |= TALITOS_MODE1_MDEU_MD5_HMAC;
				break;
			case	CRYPTO_SHA1:	
				td->hdr |= TALITOS_MODE1_MDEU_SHA1;
				break;
			case	CRYPTO_SHA1_HMAC:	
				td->hdr |= TALITOS_MODE1_MDEU_SHA1_HMAC;
				break;
			default:
				/* We cannot order the SEC as requested */
				printk("%s: cannot do the order\n",
						device_get_nameunit(sc->sc_cdev));
				err = EINVAL;
				goto errout;
		}
		if ((maccrd->crd_alg == CRYPTO_MD5_HMAC) ||
		   (maccrd->crd_alg == CRYPTO_SHA1_HMAC)) {
			/*
			 * The offset from hash data to the start of
			 * crypt data is the difference in the skips.
			 */
			/* ipsec only for now */
			td->ptr[hmac_key].ptr = dma_map_single(NULL, 
				ses->ses_hmac, ses->ses_hmac_len, DMA_TO_DEVICE);
			td->ptr[hmac_key].len = ses->ses_hmac_len;
			td->ptr[in_fifo].ptr  += enccrd->crd_skip;
			td->ptr[in_fifo].len  =  enccrd->crd_len;
			td->ptr[out_fifo].ptr += enccrd->crd_skip;
			td->ptr[out_fifo].len =  enccrd->crd_len;
			/* bytes of HMAC to postpend to ciphertext */
			td->ptr[out_fifo].extent =  ses->ses_mlen;
			td->ptr[hmac_data].ptr += maccrd->crd_skip; 
			td->ptr[hmac_data].len = enccrd->crd_skip - maccrd->crd_skip;
		}
		if (enccrd->crd_flags & CRD_F_KEY_EXPLICIT) {
			printk("%s: CRD_F_KEY_EXPLICIT unimplemented\n",
					device_get_nameunit(sc->sc_cdev));
		}
	}
	if (!enccrd && maccrd) {
		/* single MD5 or SHA */
		td->hdr |= TALITOS_SEL0_MDEU
				|  TALITOS_MODE0_MDEU_INIT
				|  TALITOS_MODE0_MDEU_PAD;
		switch (maccrd->crd_alg) {
			case	CRYPTO_MD5:	
				td->hdr |= TALITOS_MODE0_MDEU_MD5;
				DPRINTF("MD5  ses %d ch %d len %d\n",
					(u32)TALITOS_SESSION(crp->crp_sid), 
					chsel, td->ptr[in_fifo].len);
				break;
			case	CRYPTO_MD5_HMAC:	
				td->hdr |= TALITOS_MODE0_MDEU_MD5_HMAC;
				break;
			case	CRYPTO_SHA1:	
				td->hdr |= TALITOS_MODE0_MDEU_SHA1;
				DPRINTF("SHA1 ses %d ch %d len %d\n",
					(u32)TALITOS_SESSION(crp->crp_sid), 
					chsel, td->ptr[in_fifo].len);
				break;
			case	CRYPTO_SHA1_HMAC:	
				td->hdr |= TALITOS_MODE0_MDEU_SHA1_HMAC;
				break;
			default:
				/* We cannot order the SEC as requested */
				DPRINTF("cannot do the order\n");
				err = EINVAL;
				goto errout;
		}

		if (crp->crp_flags & CRYPTO_F_IOV)
			td->ptr[out_fifo].ptr += maccrd->crd_inject;

		if ((maccrd->crd_alg == CRYPTO_MD5_HMAC) ||
		   (maccrd->crd_alg == CRYPTO_SHA1_HMAC)) {
			td->ptr[hmac_key].ptr = dma_map_single(NULL, 
				ses->ses_hmac, ses->ses_hmac_len, 
				DMA_TO_DEVICE);
			td->ptr[hmac_key].len = ses->ses_hmac_len;
		}
	} 
	else {
		/* using process key (session data has duplicate) */
		td->ptr[cipher_key].ptr = dma_map_single(NULL, 
			enccrd->crd_key, (enccrd->crd_klen + 7) / 8, 
			DMA_TO_DEVICE);
		td->ptr[cipher_key].len = (enccrd->crd_klen + 7) / 8;
	}
	/* descriptor complete - GO! */
	return talitos_submit(sc, td, chsel);

errout:
	if (err != ERESTART) {
		crp->crp_etype = err;
		crypto_done(crp);
	}
	return err;
}

/* go through all channels descriptors, notifying OCF what has 
 * _and_hasn't_ successfully completed and reset the device 
 * (otherwise it's up to decoding desc hdrs!)
 */
static void talitos_errorprocessing(struct talitos_softc *sc)
{
	unsigned long flags;
	int i, j;

	/* disable further scheduling until under control */
	spin_lock_irqsave(&sc->sc_chnfifolock[sc->sc_num_channels], flags);

	if (debug) dump_talitos_status(sc);
	/* go through descriptors, try and salvage those successfully done, 
	 * and EIO those that weren't
	 */
	for (i = 0; i < sc->sc_num_channels; i++) {
		spin_lock_irqsave(&sc->sc_chnfifolock[i], flags);
		for (j = 0; j < sc->sc_chfifo_len; j++) {
			if (sc->sc_chnfifo[i][j].cf_desc.hdr) {
				if ((sc->sc_chnfifo[i][j].cf_desc.hdr 
					& TALITOS_HDR_DONE_BITS) 
					!= TALITOS_HDR_DONE_BITS) {
					/* this one didn't finish */
					/* signify in crp->etype */
					sc->sc_chnfifo[i][j].cf_crp->crp_etype 
						= EIO;
				}
			} else
				continue; /* free entry */
			/* either way, notify ocf */
			crypto_done(sc->sc_chnfifo[i][j].cf_crp);
			/* and tag it available again
			 *
			 * memset to ensure correct descriptor formation by
			 * avoiding inadvertently setting "optional" entries
			 * e.g. not using "optional" dptr2 MD/HMAC processing
			 */
			memset(&sc->sc_chnfifo[i][j].cf_desc,
				0, sizeof(struct talitos_desc));
		}
		spin_unlock_irqrestore(&sc->sc_chnfifolock[i], flags);
	}
	/* reset and initialize the SEC h/w device */
	talitos_reset_device(sc);
	talitos_init_device(sc);
#ifdef CONFIG_OCF_RANDOMHARVEST
	if (sc->sc_exec_units & TALITOS_HAS_EU_RNG)
		talitos_rng_init(sc);
#endif

	/* Okay. Stand by. */
	spin_unlock_irqrestore(&sc->sc_chnfifolock[sc->sc_num_channels], flags);

	return;
}

/* go through all channels descriptors, notifying OCF what's been done */
static void talitos_doneprocessing(struct talitos_softc *sc)
{
	unsigned long flags;
	int i, j;

	/* go through descriptors looking for done bits */
	for (i = 0; i < sc->sc_num_channels; i++) {
		spin_lock_irqsave(&sc->sc_chnfifolock[i], flags);
		for (j = 0; j < sc->sc_chfifo_len; j++) {
			/* descriptor has done bits set? */
			if ((sc->sc_chnfifo[i][j].cf_desc.hdr 
				& TALITOS_HDR_DONE_BITS) 
				== TALITOS_HDR_DONE_BITS) {
				/* notify ocf */
				crypto_done(sc->sc_chnfifo[i][j].cf_crp);
				/* and tag it available again
				 *
				 * memset to ensure correct descriptor formation by
				 * avoiding inadvertently setting "optional" entries
				 * e.g. not using "optional" dptr2 MD/HMAC processing
				 */
				memset(&sc->sc_chnfifo[i][j].cf_desc,
					0, sizeof(struct talitos_desc));
			}
		}
		spin_unlock_irqrestore(&sc->sc_chnfifolock[i], flags);
	}
	return;
}

static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
talitos_intr(int irq, void *arg)
#else
talitos_intr(int irq, void *arg, struct pt_regs *regs)
#endif
{
	struct talitos_softc *sc = arg;
	u_int32_t v, v_hi;
	
	/* ack */
	v = talitos_read(sc->sc_base_addr + TALITOS_ISR);
	v_hi = talitos_read(sc->sc_base_addr + TALITOS_ISR_HI);
	talitos_write(sc->sc_base_addr + TALITOS_ICR, v);
	talitos_write(sc->sc_base_addr + TALITOS_ICR_HI, v_hi);

	if (unlikely(v & TALITOS_ISR_ERROR)) {
		/* Okay, Houston, we've had a problem here. */
		printk(KERN_DEBUG "%s: got error interrupt - ISR 0x%08x_%08x\n",
				device_get_nameunit(sc->sc_cdev), v, v_hi);
		talitos_errorprocessing(sc);
	} else
	if (likely(v & TALITOS_ISR_DONE)) {
		talitos_doneprocessing(sc);
	}
	return IRQ_HANDLED;
}

/*
 * Initialize registers we need to touch only once.
 */
static void
talitos_init_device(struct talitos_softc *sc)
{
	u_int32_t v;
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	/* init all channels */
	for (i = 0; i < sc->sc_num_channels; i++) {
		v = talitos_read(sc->sc_base_addr + 
			i*TALITOS_CH_OFFSET + TALITOS_CH_CCCR_HI);
		v |= TALITOS_CH_CCCR_HI_CDWE
		  |  TALITOS_CH_CCCR_HI_CDIE;  /* invoke interrupt if done */
		talitos_write(sc->sc_base_addr + 
			i*TALITOS_CH_OFFSET + TALITOS_CH_CCCR_HI, v);
	}
	/* enable all interrupts */
	v = talitos_read(sc->sc_base_addr + TALITOS_IMR);
	v |= TALITOS_IMR_ALL;
	talitos_write(sc->sc_base_addr + TALITOS_IMR, v);
	v = talitos_read(sc->sc_base_addr + TALITOS_IMR_HI);
	v |= TALITOS_IMR_HI_ERRONLY;
	talitos_write(sc->sc_base_addr + TALITOS_IMR_HI, v);
	return;
}

/*
 * set the master reset bit on the device.
 */
static void
talitos_reset_device_master(struct talitos_softc *sc)
{
	u_int32_t v;

	/* Reset the device by writing 1 to MCR:SWR and waiting 'til cleared */
	v = talitos_read(sc->sc_base_addr + TALITOS_MCR);
	talitos_write(sc->sc_base_addr + TALITOS_MCR, v | TALITOS_MCR_SWR);

	while (talitos_read(sc->sc_base_addr + TALITOS_MCR) & TALITOS_MCR_SWR)
		cpu_relax();

	return;
}

/*
 * Resets the device.  Values in the registers are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
static void
talitos_reset_device(struct talitos_softc *sc)
{
	u_int32_t v;
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	/*
	 * Master reset
	 * errata documentation: warning: certain SEC interrupts 
	 * are not fully cleared by writing the MCR:SWR bit, 
	 * set bit twice to completely reset 
	 */
	talitos_reset_device_master(sc);	/* once */
	talitos_reset_device_master(sc);	/* and once again */
	
	/* reset all channels */
	for (i = 0; i < sc->sc_num_channels; i++) {
		v = talitos_read(sc->sc_base_addr + i*TALITOS_CH_OFFSET +
			TALITOS_CH_CCCR);
		talitos_write(sc->sc_base_addr + i*TALITOS_CH_OFFSET +
			TALITOS_CH_CCCR, v | TALITOS_CH_CCCR_RESET);
	}
}

/* Set up the crypto device structure, private data,
 * and anything else we need before we start */
#ifdef CONFIG_PPC_MERGE
static int talitos_probe(struct of_device *ofdev, const struct of_device_id *match)
#else
static int talitos_probe(struct platform_device *pdev)
#endif
{
	struct talitos_softc *sc = NULL;
	struct resource *r;
#ifdef CONFIG_PPC_MERGE
	struct device *device = &ofdev->dev;
	struct device_node *np = ofdev->node;
	const unsigned int *prop;
	int err;
	struct resource res;
#endif
	static int num_chips = 0;
	int rc;
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	sc = (struct talitos_softc *) kmalloc(sizeof(*sc), GFP_KERNEL);
	if (!sc)
		return -ENOMEM;
	memset(sc, 0, sizeof(*sc));

	softc_device_init(sc, DRV_NAME, num_chips, talitos_methods);

	sc->sc_irq = -1;
	sc->sc_cid = -1;
#ifndef CONFIG_PPC_MERGE
	sc->sc_dev = pdev;
#endif
	sc->sc_num = num_chips++;

#ifdef CONFIG_PPC_MERGE
	dev_set_drvdata(device, sc);
#else
	platform_set_drvdata(sc->sc_dev, sc);
#endif

	/* get the irq line */
#ifdef CONFIG_PPC_MERGE
	err = of_address_to_resource(np, 0, &res);
	if (err)
		return -EINVAL;
	r = &res;

	sc->sc_irq = irq_of_parse_and_map(np, 0);
#else
	/* get a pointer to the register memory */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	sc->sc_irq = platform_get_irq(pdev, 0);
#endif
	rc = request_irq(sc->sc_irq, talitos_intr, 0,
			device_get_nameunit(sc->sc_cdev), sc);
	if (rc) {
		printk(KERN_ERR "%s: failed to hook irq %d\n", 
				device_get_nameunit(sc->sc_cdev), sc->sc_irq);
		sc->sc_irq = -1;
		goto out;
	}

	sc->sc_base_addr = (ocf_iomem_t) ioremap(r->start, (r->end - r->start));
	if (!sc->sc_base_addr) {
		printk(KERN_ERR "%s: failed to ioremap\n",
				device_get_nameunit(sc->sc_cdev));
		goto out;
	}

	/* figure out our SEC's properties and capabilities */
	sc->sc_chiprev = (u64)talitos_read(sc->sc_base_addr + TALITOS_ID) << 32
		 | talitos_read(sc->sc_base_addr + TALITOS_ID_HI);
	DPRINTF("sec id 0x%llx\n", sc->sc_chiprev);

#ifdef CONFIG_PPC_MERGE
	/* get SEC properties from device tree, defaulting to SEC 2.0 */

	prop = of_get_property(np, "num-channels", NULL);
	sc->sc_num_channels = prop ? *prop : TALITOS_NCHANNELS_SEC_2_0;

	prop = of_get_property(np, "channel-fifo-len", NULL);
	sc->sc_chfifo_len = prop ? *prop : TALITOS_CHFIFOLEN_SEC_2_0;

	prop = of_get_property(np, "exec-units-mask", NULL);
	sc->sc_exec_units = prop ? *prop : TALITOS_HAS_EUS_SEC_2_0;

	prop = of_get_property(np, "descriptor-types-mask", NULL);
	sc->sc_desc_types = prop ? *prop : TALITOS_HAS_DESCTYPES_SEC_2_0;
#else
	/* bulk should go away with openfirmware flat device tree support */
	if (sc->sc_chiprev & TALITOS_ID_SEC_2_0) {
		sc->sc_num_channels = TALITOS_NCHANNELS_SEC_2_0;
		sc->sc_chfifo_len = TALITOS_CHFIFOLEN_SEC_2_0;
		sc->sc_exec_units = TALITOS_HAS_EUS_SEC_2_0;
		sc->sc_desc_types = TALITOS_HAS_DESCTYPES_SEC_2_0;
	} else {
		printk(KERN_ERR "%s: failed to id device\n",
				device_get_nameunit(sc->sc_cdev));
		goto out;
	}
#endif

	/* + 1 is for the meta-channel lock used by the channel scheduler */
	sc->sc_chnfifolock = (spinlock_t *) kmalloc(
		(sc->sc_num_channels + 1) * sizeof(spinlock_t), GFP_KERNEL);
	if (!sc->sc_chnfifolock)
		goto out;
	for (i = 0; i < sc->sc_num_channels + 1; i++) {
		spin_lock_init(&sc->sc_chnfifolock[i]);
	}

	sc->sc_chnlastalg = (int *) kmalloc(
		sc->sc_num_channels * sizeof(int), GFP_KERNEL);
	if (!sc->sc_chnlastalg)
		goto out;
	memset(sc->sc_chnlastalg, 0, sc->sc_num_channels * sizeof(int));

	sc->sc_chnfifo = (struct desc_cryptop_pair **) kmalloc(
		sc->sc_num_channels * sizeof(struct desc_cryptop_pair *), 
		GFP_KERNEL);
	if (!sc->sc_chnfifo)
		goto out;
	for (i = 0; i < sc->sc_num_channels; i++) {
		sc->sc_chnfifo[i] = (struct desc_cryptop_pair *) kmalloc(
			sc->sc_chfifo_len * sizeof(struct desc_cryptop_pair), 
			GFP_KERNEL);
		if (!sc->sc_chnfifo[i])
			goto out;
		memset(sc->sc_chnfifo[i], 0, 
			sc->sc_chfifo_len * sizeof(struct desc_cryptop_pair));
	}

	/* reset and initialize the SEC h/w device */
	talitos_reset_device(sc);
	talitos_init_device(sc);

	sc->sc_cid = crypto_get_driverid(softc_get_device(sc),CRYPTOCAP_F_HARDWARE);
	if (sc->sc_cid < 0) {
		printk(KERN_ERR "%s: could not get crypto driver id\n",
				device_get_nameunit(sc->sc_cdev));
		goto out;
	}

	/* register algorithms with the framework */
	printk("%s:", device_get_nameunit(sc->sc_cdev));

	if (sc->sc_exec_units & TALITOS_HAS_EU_RNG)  {
		printk(" rng");
#ifdef CONFIG_OCF_RANDOMHARVEST
		talitos_rng_init(sc);
		crypto_rregister(sc->sc_cid, talitos_read_random, sc);
#endif
	}
	if (sc->sc_exec_units & TALITOS_HAS_EU_DEU) {
		printk(" des/3des");
		crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0);
	}
	if (sc->sc_exec_units & TALITOS_HAS_EU_AESU) {
		printk(" aes");
		crypto_register(sc->sc_cid, CRYPTO_AES_CBC, 0, 0);
	}
	if (sc->sc_exec_units & TALITOS_HAS_EU_MDEU) {
		printk(" md5");
		crypto_register(sc->sc_cid, CRYPTO_MD5, 0, 0);
		/* HMAC support only with IPsec for now */
		crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0);
		printk(" sha1");
		crypto_register(sc->sc_cid, CRYPTO_SHA1, 0, 0);
		/* HMAC support only with IPsec for now */
		crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0);
	}
	printk("\n");
	return 0;

out:
#ifndef CONFIG_PPC_MERGE
	talitos_remove(pdev);
#endif
	return -ENOMEM;
}

#ifdef CONFIG_PPC_MERGE
static int talitos_remove(struct of_device *ofdev)
#else
static int talitos_remove(struct platform_device *pdev)
#endif
{
#ifdef CONFIG_PPC_MERGE
	struct talitos_softc *sc = dev_get_drvdata(&ofdev->dev);
#else
	struct talitos_softc *sc = platform_get_drvdata(pdev);
#endif
	int i;

	DPRINTF("%s()\n", __FUNCTION__);
	if (sc->sc_cid >= 0)
		crypto_unregister_all(sc->sc_cid);
	if (sc->sc_chnfifo) {
		for (i = 0; i < sc->sc_num_channels; i++)
			if (sc->sc_chnfifo[i])
				kfree(sc->sc_chnfifo[i]);
		kfree(sc->sc_chnfifo);
	}
	if (sc->sc_chnlastalg)
		kfree(sc->sc_chnlastalg);
	if (sc->sc_chnfifolock)
		kfree(sc->sc_chnfifolock);
	if (sc->sc_irq != -1)
		free_irq(sc->sc_irq, sc);
	if (sc->sc_base_addr)
		iounmap((void *) sc->sc_base_addr);
	kfree(sc);
	return 0;
}

#ifdef CONFIG_PPC_MERGE
static struct of_device_id talitos_match[] = {
	{
		.type = "crypto",
		.compatible = "talitos",
	},
	{},
};

MODULE_DEVICE_TABLE(of, talitos_match);

static struct of_platform_driver talitos_driver = {
	.name		= DRV_NAME,
	.match_table	= talitos_match,
	.probe		= talitos_probe,
	.remove		= talitos_remove,
};

static int __init talitos_init(void)
{
	return of_register_platform_driver(&talitos_driver);
}

static void __exit talitos_exit(void)
{
	of_unregister_platform_driver(&talitos_driver);
}
#else
/* Structure for a platform device driver */
static struct platform_driver talitos_driver = {
	.probe = talitos_probe,
	.remove = talitos_remove,
	.driver = {
		.name = "fsl-sec2",
	}
};

static int __init talitos_init(void)
{
	return platform_driver_register(&talitos_driver);
}

static void __exit talitos_exit(void)
{
	platform_driver_unregister(&talitos_driver);
}
#endif

module_init(talitos_init);
module_exit(talitos_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("kim.phillips@freescale.com");
MODULE_DESCRIPTION("OCF driver for Freescale SEC (talitos)");
