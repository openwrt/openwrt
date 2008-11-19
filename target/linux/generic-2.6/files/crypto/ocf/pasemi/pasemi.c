/*
 * Copyright (C) 2007 PA Semi, Inc
 *
 * Driver for the PA Semi PWRficient DMA Crypto Engine
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <asm/scatterlist.h>
#include <linux/moduleparam.h>
#include <linux/pci.h>
#include <cryptodev.h>
#include <uio.h>
#include "pasemi_fnu.h"

#define DRV_NAME "pasemi"

#define TIMER_INTERVAL 1000

static void __devexit pasemi_dma_remove(struct pci_dev *pdev);
static struct pasdma_status volatile * dma_status;

static int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Enable debug");

static void pasemi_desc_start(struct pasemi_desc *desc, u64 hdr)
{
	desc->postop = 0;
	desc->quad[0] = hdr;
	desc->quad_cnt = 1;
	desc->size = 1;
}

static void pasemi_desc_build(struct pasemi_desc *desc, u64 val)
{
	desc->quad[desc->quad_cnt++] = val;
	desc->size = (desc->quad_cnt + 1) / 2;
}

static void pasemi_desc_hdr(struct pasemi_desc *desc, u64 hdr)
{
	desc->quad[0] |= hdr;
}

static int pasemi_desc_size(struct pasemi_desc *desc)
{
	return desc->size;
}

static void pasemi_ring_add_desc(
				 struct pasemi_fnu_txring *ring,
				 struct pasemi_desc *desc,
				 struct cryptop *crp) {
	int i;
	int ring_index = 2 * (ring->next_to_fill & (TX_RING_SIZE-1));

	TX_DESC_INFO(ring, ring->next_to_fill).desc_size = desc->size;
	TX_DESC_INFO(ring, ring->next_to_fill).desc_postop = desc->postop;
	TX_DESC_INFO(ring, ring->next_to_fill).cf_crp = crp;

	for (i = 0; i < desc->quad_cnt; i += 2) {
		ring_index = 2 * (ring->next_to_fill & (TX_RING_SIZE-1));
		ring->desc[ring_index] = desc->quad[i];
		ring->desc[ring_index + 1] = desc->quad[i + 1];
		ring->next_to_fill++;
	}

	if (desc->quad_cnt & 1)
		ring->desc[ring_index + 1] = 0;
}

static void pasemi_ring_incr(struct pasemi_softc *sc, int chan_index, int incr)
{
	out_le32(sc->dma_regs + PAS_DMA_TXCHAN_INCR(sc->base_chan + chan_index),
		 incr);
}

/*
 * Generate a new software session.
 */
static int
pasemi_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
	struct cryptoini *c, *encini = NULL, *macini = NULL;
	struct pasemi_softc *sc = device_get_softc(dev);
	struct pasemi_session *ses = NULL, **sespp;
	int sesn, blksz = 0;
	u64 ccmd = 0;
	unsigned long flags;
	struct pasemi_desc init_desc;
	struct pasemi_fnu_txring *txring;

	DPRINTF("%s()\n", __FUNCTION__);
	if (sidp == NULL || cri == NULL || sc == NULL) {
		DPRINTF("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return -EINVAL;
	}
	for (c = cri; c != NULL; c = c->cri_next) {
		if (ALG_IS_SIG(c->cri_alg)) {
			if (macini)
				return -EINVAL;
			macini = c;
		} else if (ALG_IS_CIPHER(c->cri_alg)) {
			if (encini)
				return -EINVAL;
			encini = c;
		} else {
			DPRINTF("UNKNOWN c->cri_alg %d\n", c->cri_alg);
			return -EINVAL;
		}
	}
	if (encini == NULL && macini == NULL)
		return -EINVAL;
	if (encini) {
		/* validate key length */
		switch (encini->cri_alg) {
		case CRYPTO_DES_CBC:
			if (encini->cri_klen != 64)
				return -EINVAL;
			ccmd = DMA_CALGO_DES;
			break;
		case CRYPTO_3DES_CBC:
			if (encini->cri_klen != 192)
				return -EINVAL;
			ccmd = DMA_CALGO_3DES;
			break;
		case CRYPTO_AES_CBC:
			if (encini->cri_klen != 128 &&
			    encini->cri_klen != 192 &&
			    encini->cri_klen != 256)
				return -EINVAL;
			ccmd = DMA_CALGO_AES;
			break;
		case CRYPTO_ARC4:
			if (encini->cri_klen != 128)
				return -EINVAL;
			ccmd = DMA_CALGO_ARC;
			break;
		default:
			DPRINTF("UNKNOWN encini->cri_alg %d\n",
				encini->cri_alg);
			return -EINVAL;
		}
	}

	if (macini) {
		switch (macini->cri_alg) {
		case CRYPTO_MD5:
		case CRYPTO_MD5_HMAC:
			blksz = 16;
			break;
		case CRYPTO_SHA1:
		case CRYPTO_SHA1_HMAC:
			blksz = 20;
			break;
		default:
			DPRINTF("UNKNOWN macini->cri_alg %d\n",
				macini->cri_alg);
			return -EINVAL;
		}
		if (((macini->cri_klen + 7) / 8) > blksz) {
			DPRINTF("key length %d bigger than blksize %d not supported\n",
				((macini->cri_klen + 7) / 8), blksz);
			return -EINVAL;
		}
	}

	for (sesn = 0; sesn < sc->sc_nsessions; sesn++) {
		if (sc->sc_sessions[sesn] == NULL) {
			sc->sc_sessions[sesn] = (struct pasemi_session *)
				kzalloc(sizeof(struct pasemi_session), GFP_ATOMIC);
			ses = sc->sc_sessions[sesn];
			break;
		} else if (sc->sc_sessions[sesn]->used == 0) {
			ses = sc->sc_sessions[sesn];
			break;
		}
	}

	if (ses == NULL) {
		sespp = (struct pasemi_session **)
			kzalloc(sc->sc_nsessions * 2 *
				sizeof(struct pasemi_session *), GFP_ATOMIC);
		if (sespp == NULL)
			return -ENOMEM;
		memcpy(sespp, sc->sc_sessions,
		       sc->sc_nsessions * sizeof(struct pasemi_session *));
		kfree(sc->sc_sessions);
		sc->sc_sessions = sespp;
		sesn = sc->sc_nsessions;
		ses = sc->sc_sessions[sesn] = (struct pasemi_session *)
			kzalloc(sizeof(struct pasemi_session), GFP_ATOMIC);
		if (ses == NULL)
			return -ENOMEM;
		sc->sc_nsessions *= 2;
	}

	ses->used = 1;

	ses->dma_addr = pci_map_single(sc->dma_pdev, (void *) ses->civ,
				       sizeof(struct pasemi_session), DMA_TO_DEVICE);

	/* enter the channel scheduler */
	spin_lock_irqsave(&sc->sc_chnlock, flags);

	/* ARC4 has to be processed by the even channel */
	if (encini && (encini->cri_alg == CRYPTO_ARC4))
		ses->chan = sc->sc_lastchn & ~1;
	else
		ses->chan = sc->sc_lastchn;
	sc->sc_lastchn = (sc->sc_lastchn + 1) % sc->sc_num_channels;

	spin_unlock_irqrestore(&sc->sc_chnlock, flags);

	txring = &sc->tx[ses->chan];

	if (encini) {
		ses->ccmd = ccmd;

		/* get an IV */
		/* XXX may read fewer than requested */
		get_random_bytes(ses->civ, sizeof(ses->civ));

		ses->keysz = (encini->cri_klen - 63) / 64;
		memcpy(ses->key, encini->cri_key, (ses->keysz + 1) * 8);

		pasemi_desc_start(&init_desc,
				  XCT_CTRL_HDR(ses->chan, (encini && macini) ? 0x68 : 0x40, DMA_FN_CIV0));
		pasemi_desc_build(&init_desc,
				  XCT_FUN_SRC_PTR((encini && macini) ? 0x68 : 0x40, ses->dma_addr));
	}
	if (macini) {
		if (macini->cri_alg == CRYPTO_MD5_HMAC ||
		    macini->cri_alg == CRYPTO_SHA1_HMAC)
			memcpy(ses->hkey, macini->cri_key, blksz);
		else {
			/* Load initialization constants(RFC 1321, 3174) */
			ses->hiv[0] = 0x67452301efcdab89ULL;
			ses->hiv[1] = 0x98badcfe10325476ULL;
			ses->hiv[2] = 0xc3d2e1f000000000ULL;
		}
		ses->hseq = 0ULL;
	}

	spin_lock_irqsave(&txring->fill_lock, flags);

	if (((txring->next_to_fill + pasemi_desc_size(&init_desc)) -
	     txring->next_to_clean) > TX_RING_SIZE) {
		spin_unlock_irqrestore(&txring->fill_lock, flags);
		return ERESTART;
	}

	if (encini) {
		pasemi_ring_add_desc(txring, &init_desc, NULL);
		pasemi_ring_incr(sc, ses->chan,
				 pasemi_desc_size(&init_desc));
	}

	txring->sesn = sesn;
	spin_unlock_irqrestore(&txring->fill_lock, flags);

	*sidp = PASEMI_SID(sesn);
	return 0;
}

/*
 * Deallocate a session.
 */
static int
pasemi_freesession(device_t dev, u_int64_t tid)
{
	struct pasemi_softc *sc = device_get_softc(dev);
	int session;
	u_int32_t sid = ((u_int32_t) tid) & 0xffffffff;

	DPRINTF("%s()\n", __FUNCTION__);

	if (sc == NULL)
		return -EINVAL;
	session = PASEMI_SESSION(sid);
	if (session >= sc->sc_nsessions || !sc->sc_sessions[session])
		return -EINVAL;

	pci_unmap_single(sc->dma_pdev,
			 sc->sc_sessions[session]->dma_addr,
			 sizeof(struct pasemi_session), DMA_TO_DEVICE);
	memset(sc->sc_sessions[session], 0,
	       sizeof(struct pasemi_session));

	return 0;
}

static int
pasemi_process(device_t dev, struct cryptop *crp, int hint)
{

	int err = 0, ivsize, srclen = 0, reinit = 0, reinit_size = 0, chsel;
	struct pasemi_softc *sc = device_get_softc(dev);
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;
	caddr_t ivp;
	struct pasemi_desc init_desc, work_desc;
	struct pasemi_session *ses;
	struct sk_buff *skb;
	struct uio *uiop;
	unsigned long flags;
	struct pasemi_fnu_txring *txring;

	DPRINTF("%s()\n", __FUNCTION__);

	if (crp == NULL || crp->crp_callback == NULL || sc == NULL)
		return -EINVAL;

	crp->crp_etype = 0;
	if (PASEMI_SESSION(crp->crp_sid) >= sc->sc_nsessions)
		return -EINVAL;

	ses = sc->sc_sessions[PASEMI_SESSION(crp->crp_sid)];

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		err = -EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;

	if (ALG_IS_SIG(crd1->crd_alg)) {
		maccrd = crd1;
		if (crd2 == NULL)
			enccrd = NULL;
		else if (ALG_IS_CIPHER(crd2->crd_alg) &&
			 (crd2->crd_flags & CRD_F_ENCRYPT) == 0)
			enccrd = crd2;
		else
			goto erralg;
	} else if (ALG_IS_CIPHER(crd1->crd_alg)) {
		enccrd = crd1;
		if (crd2 == NULL)
			maccrd = NULL;
		else if (ALG_IS_SIG(crd2->crd_alg) &&
			 (crd1->crd_flags & CRD_F_ENCRYPT))
			maccrd = crd2;
		else
			goto erralg;
	} else
		goto erralg;

	chsel = ses->chan;

	txring = &sc->tx[chsel];

	if (enccrd && !maccrd) {
		if (enccrd->crd_alg == CRYPTO_ARC4)
			reinit = 1;
		reinit_size = 0x40;
		srclen = crp->crp_ilen;

		pasemi_desc_start(&work_desc, XCT_FUN_O | XCT_FUN_I
				  | XCT_FUN_FUN(chsel));
		if (enccrd->crd_flags & CRD_F_ENCRYPT)
			pasemi_desc_hdr(&work_desc, XCT_FUN_CRM_ENC);
		else
			pasemi_desc_hdr(&work_desc, XCT_FUN_CRM_DEC);
	} else if (enccrd && maccrd) {
		if (enccrd->crd_alg == CRYPTO_ARC4)
			reinit = 1;
		reinit_size = 0x68;

		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			/* Encrypt -> Authenticate */
			pasemi_desc_start(&work_desc, XCT_FUN_O | XCT_FUN_I | XCT_FUN_CRM_ENC_SIG
					  | XCT_FUN_A | XCT_FUN_FUN(chsel));
			srclen = maccrd->crd_skip + maccrd->crd_len;
		} else {
			/* Authenticate -> Decrypt */
			pasemi_desc_start(&work_desc, XCT_FUN_O | XCT_FUN_I | XCT_FUN_CRM_SIG_DEC
					  | XCT_FUN_24BRES | XCT_FUN_FUN(chsel));
			pasemi_desc_build(&work_desc, 0);
			pasemi_desc_build(&work_desc, 0);
			pasemi_desc_build(&work_desc, 0);
			work_desc.postop = PASEMI_CHECK_SIG;
			srclen = crp->crp_ilen;
		}

		pasemi_desc_hdr(&work_desc, XCT_FUN_SHL(maccrd->crd_skip / 4));
		pasemi_desc_hdr(&work_desc, XCT_FUN_CHL(enccrd->crd_skip - maccrd->crd_skip));
	} else if (!enccrd && maccrd) {
		srclen = maccrd->crd_len;

		pasemi_desc_start(&init_desc,
				  XCT_CTRL_HDR(chsel, 0x58, DMA_FN_HKEY0));
		pasemi_desc_build(&init_desc,
				  XCT_FUN_SRC_PTR(0x58, ((struct pasemi_session *)ses->dma_addr)->hkey));

		pasemi_desc_start(&work_desc, XCT_FUN_O | XCT_FUN_I | XCT_FUN_CRM_SIG
				  | XCT_FUN_A | XCT_FUN_FUN(chsel));
	}

	if (enccrd) {
		switch (enccrd->crd_alg) {
		case CRYPTO_3DES_CBC:
			pasemi_desc_hdr(&work_desc, XCT_FUN_ALG_3DES |
					XCT_FUN_BCM_CBC);
			ivsize = sizeof(u64);
			break;
		case CRYPTO_DES_CBC:
			pasemi_desc_hdr(&work_desc, XCT_FUN_ALG_DES |
					XCT_FUN_BCM_CBC);
			ivsize = sizeof(u64);
			break;
		case CRYPTO_AES_CBC:
			pasemi_desc_hdr(&work_desc, XCT_FUN_ALG_AES |
					XCT_FUN_BCM_CBC);
			ivsize = 2 * sizeof(u64);
			break;
		case CRYPTO_ARC4:
			pasemi_desc_hdr(&work_desc, XCT_FUN_ALG_ARC);
			ivsize = 0;
			break;
		default:
			printk(DRV_NAME ": unimplemented enccrd->crd_alg %d\n",
			       enccrd->crd_alg);
			err = -EINVAL;
			goto errout;
		}

		ivp = (ivsize == sizeof(u64)) ? (caddr_t) &ses->civ[1] : (caddr_t) &ses->civ[0];
		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				memcpy(ivp, enccrd->crd_iv, ivsize);
			/* If IV is not present in the buffer already, it has to be copied there */
			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0)
				crypto_copyback(crp->crp_flags, crp->crp_buf,
						enccrd->crd_inject, ivsize, ivp);
		} else {
			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				/* IV is provided expicitly in descriptor */
				memcpy(ivp, enccrd->crd_iv, ivsize);
			else
				/* IV is provided in the packet */
				crypto_copydata(crp->crp_flags, crp->crp_buf,
						enccrd->crd_inject, ivsize,
						ivp);
		}
	}

	if (maccrd) {
		switch (maccrd->crd_alg) {
		case CRYPTO_MD5:
			pasemi_desc_hdr(&work_desc, XCT_FUN_SIG_MD5 |
					XCT_FUN_HSZ((crp->crp_ilen - maccrd->crd_inject) / 4));
			break;
		case CRYPTO_SHA1:
			pasemi_desc_hdr(&work_desc, XCT_FUN_SIG_SHA1 |
					XCT_FUN_HSZ((crp->crp_ilen - maccrd->crd_inject) / 4));
			break;
		case CRYPTO_MD5_HMAC:
			pasemi_desc_hdr(&work_desc, XCT_FUN_SIG_HMAC_MD5 |
					XCT_FUN_HSZ((crp->crp_ilen - maccrd->crd_inject) / 4));
			break;
		case CRYPTO_SHA1_HMAC:
			pasemi_desc_hdr(&work_desc, XCT_FUN_SIG_HMAC_SHA1 |
					XCT_FUN_HSZ((crp->crp_ilen - maccrd->crd_inject) / 4));
			break;
		default:
			printk(DRV_NAME ": unimplemented maccrd->crd_alg %d\n",
			       maccrd->crd_alg);
			err = -EINVAL;
			goto errout;
		}
	}

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		/* using SKB buffers */
		skb = (struct sk_buff *)crp->crp_buf;
		if (skb_shinfo(skb)->nr_frags) {
			printk(DRV_NAME ": skb frags unimplemented\n");
			err = -EINVAL;
			goto errout;
		}
		pasemi_desc_build(
			&work_desc,
			XCT_FUN_DST_PTR(skb->len, pci_map_single(
						sc->dma_pdev, skb->data,
						skb->len, DMA_TO_DEVICE)));
		pasemi_desc_build(
			&work_desc,
			XCT_FUN_SRC_PTR(
				srclen, pci_map_single(
					sc->dma_pdev, skb->data,
					srclen, DMA_TO_DEVICE)));
		pasemi_desc_hdr(&work_desc, XCT_FUN_LLEN(srclen));
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		/* using IOV buffers */
		uiop = (struct uio *)crp->crp_buf;
		if (uiop->uio_iovcnt > 1) {
			printk(DRV_NAME ": iov frags unimplemented\n");
			err = -EINVAL;
			goto errout;
		}

		/* crp_olen is never set; always use crp_ilen */
		pasemi_desc_build(
			&work_desc,
			XCT_FUN_DST_PTR(crp->crp_ilen, pci_map_single(
						sc->dma_pdev,
						uiop->uio_iov->iov_base,
						crp->crp_ilen, DMA_TO_DEVICE)));
		pasemi_desc_hdr(&work_desc, XCT_FUN_LLEN(srclen));

		pasemi_desc_build(
			&work_desc,
			XCT_FUN_SRC_PTR(srclen, pci_map_single(
						sc->dma_pdev,
						uiop->uio_iov->iov_base,
						srclen, DMA_TO_DEVICE)));
	} else {
		/* using contig buffers */
		pasemi_desc_build(
			&work_desc,
			XCT_FUN_DST_PTR(crp->crp_ilen, pci_map_single(
						sc->dma_pdev,
						crp->crp_buf,
						crp->crp_ilen, DMA_TO_DEVICE)));
		pasemi_desc_build(
			&work_desc,
			XCT_FUN_SRC_PTR(srclen, pci_map_single(
						sc->dma_pdev,
						crp->crp_buf, srclen,
						DMA_TO_DEVICE)));
		pasemi_desc_hdr(&work_desc, XCT_FUN_LLEN(srclen));
	}

	spin_lock_irqsave(&txring->fill_lock, flags);

	if (txring->sesn != PASEMI_SESSION(crp->crp_sid)) {
		txring->sesn = PASEMI_SESSION(crp->crp_sid);
		reinit = 1;
	}

	if (enccrd) {
		pasemi_desc_start(&init_desc,
				  XCT_CTRL_HDR(chsel, reinit ? reinit_size : 0x10, DMA_FN_CIV0));
		pasemi_desc_build(&init_desc,
				  XCT_FUN_SRC_PTR(reinit ? reinit_size : 0x10, ses->dma_addr));
	}

	if (((txring->next_to_fill + pasemi_desc_size(&init_desc) +
	      pasemi_desc_size(&work_desc)) -
	     txring->next_to_clean) > TX_RING_SIZE) {
		spin_unlock_irqrestore(&txring->fill_lock, flags);
		err = ERESTART;
		goto errout;
	}

	pasemi_ring_add_desc(txring, &init_desc, NULL);
	pasemi_ring_add_desc(txring, &work_desc, crp);

	pasemi_ring_incr(sc, chsel,
			 pasemi_desc_size(&init_desc) +
			 pasemi_desc_size(&work_desc));

	spin_unlock_irqrestore(&txring->fill_lock, flags);

	mod_timer(&txring->crypto_timer, jiffies + TIMER_INTERVAL);

	return 0;

erralg:
	printk(DRV_NAME ": unsupported algorithm or algorithm order alg1 %d alg2 %d\n",
	       crd1->crd_alg, crd2->crd_alg);
	err = -EINVAL;

errout:
	if (err != ERESTART) {
		crp->crp_etype = err;
		crypto_done(crp);
	}
	return err;
}

static int pasemi_clean_tx(struct pasemi_softc *sc, int chan)
{
	int i, j, ring_idx;
	struct pasemi_fnu_txring *ring = &sc->tx[chan];
	u16 delta_cnt;
	int flags, loops = 10;
	int desc_size;
	struct cryptop *crp;

	spin_lock_irqsave(&ring->clean_lock, flags);

	while ((delta_cnt = (dma_status->tx_sta[sc->base_chan + chan]
			     & PAS_STATUS_PCNT_M) - ring->total_pktcnt)
	       && loops--) {

		for (i = 0; i < delta_cnt; i++) {
			desc_size = TX_DESC_INFO(ring, ring->next_to_clean).desc_size;
			crp = TX_DESC_INFO(ring, ring->next_to_clean).cf_crp;
			if (crp) {
				ring_idx = 2 * (ring->next_to_clean & (TX_RING_SIZE-1));
				if (TX_DESC_INFO(ring, ring->next_to_clean).desc_postop & PASEMI_CHECK_SIG) {
					/* Need to make sure signature matched,
					 * if not - return error */
					if (!(ring->desc[ring_idx + 1] & (1ULL << 63)))
						crp->crp_etype = -EINVAL;
				}
				crypto_done(TX_DESC_INFO(ring,
							 ring->next_to_clean).cf_crp);
				TX_DESC_INFO(ring, ring->next_to_clean).cf_crp = NULL;
				pci_unmap_single(
					sc->dma_pdev,
					XCT_PTR_ADDR_LEN(ring->desc[ring_idx + 1]),
					PCI_DMA_TODEVICE);

				ring->desc[ring_idx] = ring->desc[ring_idx + 1] = 0;

				ring->next_to_clean++;
				for (j = 1; j < desc_size; j++) {
					ring_idx = 2 *
						(ring->next_to_clean &
						 (TX_RING_SIZE-1));
					pci_unmap_single(
						sc->dma_pdev,
						XCT_PTR_ADDR_LEN(ring->desc[ring_idx]),
						PCI_DMA_TODEVICE);
					if (ring->desc[ring_idx + 1])
						pci_unmap_single(
							sc->dma_pdev,
							XCT_PTR_ADDR_LEN(
								ring->desc[
									ring_idx + 1]),
							PCI_DMA_TODEVICE);
					ring->desc[ring_idx] =
						ring->desc[ring_idx + 1] = 0;
					ring->next_to_clean++;
				}
			} else {
				for (j = 0; j < desc_size; j++) {
					ring_idx = 2 * (ring->next_to_clean & (TX_RING_SIZE-1));
					ring->desc[ring_idx] =
						ring->desc[ring_idx + 1] = 0;
					ring->next_to_clean++;
				}
			}
		}

		ring->total_pktcnt += delta_cnt;
	}
	spin_unlock_irqrestore(&ring->clean_lock, flags);

	return 0;
}

static void sweepup_tx(struct pasemi_softc *sc)
{
	int i;

	for (i = 0; i < sc->sc_num_channels; i++)
		pasemi_clean_tx(sc, i);
}

static irqreturn_t pasemi_intr(int irq, void *arg, struct pt_regs *regs)
{
	struct pasemi_softc *sc = arg;
	unsigned int reg;
	int chan = irq - sc->base_irq;
	int chan_index = sc->base_chan + chan;
	u64 stat = dma_status->tx_sta[chan_index];

	DPRINTF("%s()\n", __FUNCTION__);

	if (!(stat & PAS_STATUS_CAUSE_M))
		return IRQ_NONE;

	pasemi_clean_tx(sc, chan);

	stat = dma_status->tx_sta[chan_index];

	reg = PAS_IOB_DMA_TXCH_RESET_PINTC |
		PAS_IOB_DMA_TXCH_RESET_PCNT(sc->tx[chan].total_pktcnt);

	if (stat & PAS_STATUS_SOFT)
		reg |= PAS_IOB_DMA_RXCH_RESET_SINTC;

	out_le32(sc->iob_regs + PAS_IOB_DMA_TXCH_RESET(chan_index), reg);


	return IRQ_HANDLED;
}

static int pasemi_dma_setup_tx_resources(struct pasemi_softc *sc, int chan)
{
	u32 val;
	int chan_index = chan + sc->base_chan;
	int ret;
	struct pasemi_fnu_txring *ring;

	ring = &sc->tx[chan];

	spin_lock_init(&ring->fill_lock);
	spin_lock_init(&ring->clean_lock);

	ring->desc_info = kzalloc(sizeof(struct pasemi_desc_info) *
				  TX_RING_SIZE, GFP_KERNEL);
	if (!ring->desc_info)
		return -ENOMEM;

	/* Allocate descriptors */
	ring->desc = dma_alloc_coherent(&sc->dma_pdev->dev,
					TX_RING_SIZE *
					2 * sizeof(u64),
					&ring->dma, GFP_KERNEL);
	if (!ring->desc)
		return -ENOMEM;

	memset((void *) ring->desc, 0, TX_RING_SIZE * 2 * sizeof(u64));

	out_le32(sc->iob_regs + PAS_IOB_DMA_TXCH_RESET(chan_index), 0x30);

	ring->total_pktcnt = 0;

	out_le32(sc->dma_regs + PAS_DMA_TXCHAN_BASEL(chan_index),
		 PAS_DMA_TXCHAN_BASEL_BRBL(ring->dma));

	val = PAS_DMA_TXCHAN_BASEU_BRBH(ring->dma >> 32);
	val |= PAS_DMA_TXCHAN_BASEU_SIZ(TX_RING_SIZE >> 2);

	out_le32(sc->dma_regs + PAS_DMA_TXCHAN_BASEU(chan_index), val);

	out_le32(sc->dma_regs + PAS_DMA_TXCHAN_CFG(chan_index),
		 PAS_DMA_TXCHAN_CFG_TY_FUNC |
		 PAS_DMA_TXCHAN_CFG_TATTR(chan) |
		 PAS_DMA_TXCHAN_CFG_WT(2));

	/* enable tx channel */
	out_le32(sc->dma_regs +
		 PAS_DMA_TXCHAN_TCMDSTA(chan_index),
		 PAS_DMA_TXCHAN_TCMDSTA_EN);

	out_le32(sc->iob_regs + PAS_IOB_DMA_TXCH_CFG(chan_index),
		 PAS_IOB_DMA_TXCH_CFG_CNTTH(1000));

	ring->next_to_fill = 0;
	ring->next_to_clean = 0;

	snprintf(ring->irq_name, sizeof(ring->irq_name),
		 "%s%d", "crypto", chan);

	ring->irq = irq_create_mapping(NULL, sc->base_irq + chan);
	ret = request_irq(ring->irq, (irq_handler_t)
			  pasemi_intr, IRQF_DISABLED, ring->irq_name, sc);
	if (ret) {
		printk(KERN_ERR DRV_NAME ": failed to hook irq %d ret %d\n",
		       ring->irq, ret);
		ring->irq = -1;
		return ret;
	}

	setup_timer(&ring->crypto_timer, (void *) sweepup_tx, (unsigned long) sc);

	return 0;
}

static device_method_t pasemi_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,		pasemi_newsession),
	DEVMETHOD(cryptodev_freesession,	pasemi_freesession),
	DEVMETHOD(cryptodev_process,		pasemi_process),
};

/* Set up the crypto device structure, private data,
 * and anything else we need before we start */

static int __devinit
pasemi_dma_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct pasemi_softc *sc;
	int ret, i;

	DPRINTF(KERN_ERR "%s()\n", __FUNCTION__);

	sc = kzalloc(sizeof(*sc), GFP_KERNEL);
	if (!sc)
		return -ENOMEM;

	softc_device_init(sc, DRV_NAME, 1, pasemi_methods);

	pci_set_drvdata(pdev, sc);

	spin_lock_init(&sc->sc_chnlock);

	sc->sc_sessions = (struct pasemi_session **)
		kzalloc(PASEMI_INITIAL_SESSIONS *
			sizeof(struct pasemi_session *), GFP_ATOMIC);
	if (sc->sc_sessions == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	sc->sc_nsessions = PASEMI_INITIAL_SESSIONS;
	sc->sc_lastchn = 0;
	sc->base_irq = pdev->irq + 6;
	sc->base_chan = 6;
	sc->sc_cid = -1;
	sc->dma_pdev = pdev;

	sc->iob_pdev = pci_get_device(PCI_VENDOR_ID_PASEMI, 0xa001, NULL);
	if (!sc->iob_pdev) {
		dev_err(&pdev->dev, "Can't find I/O Bridge\n");
		ret = -ENODEV;
		goto out;
	}

	/* This is hardcoded and ugly, but we have some firmware versions
	 * who don't provide the register space in the device tree. Luckily
	 * they are at well-known locations so we can just do the math here.
	 */
	sc->dma_regs =
		ioremap(0xe0000000 + (sc->dma_pdev->devfn << 12), 0x2000);
	sc->iob_regs =
		ioremap(0xe0000000 + (sc->iob_pdev->devfn << 12), 0x2000);
	if (!sc->dma_regs || !sc->iob_regs) {
		dev_err(&pdev->dev, "Can't map registers\n");
		ret = -ENODEV;
		goto out;
	}

	dma_status = __ioremap(0xfd800000, 0x1000, 0);
	if (!dma_status) {
		ret = -ENODEV;
		dev_err(&pdev->dev, "Can't map dmastatus space\n");
		goto out;
	}

	sc->tx = (struct pasemi_fnu_txring *)
		kzalloc(sizeof(struct pasemi_fnu_txring)
			* 8, GFP_KERNEL);
	if (!sc->tx) {
		ret = -ENOMEM;
		goto out;
	}

	/* Initialize the h/w */
	out_le32(sc->dma_regs + PAS_DMA_COM_CFG,
		 (in_le32(sc->dma_regs + PAS_DMA_COM_CFG) |
		  PAS_DMA_COM_CFG_FWF));
	out_le32(sc->dma_regs + PAS_DMA_COM_TXCMD, PAS_DMA_COM_TXCMD_EN);

	for (i = 0; i < PASEMI_FNU_CHANNELS; i++) {
		sc->sc_num_channels++;
		ret = pasemi_dma_setup_tx_resources(sc, i);
		if (ret)
			goto out;
	}

	sc->sc_cid = crypto_get_driverid(softc_get_device(sc),
					 CRYPTOCAP_F_HARDWARE);
	if (sc->sc_cid < 0) {
		printk(KERN_ERR DRV_NAME ": could not get crypto driver id\n");
		ret = -ENXIO;
		goto out;
	}

	/* register algorithms with the framework */
	printk(DRV_NAME ":");

	crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_AES_CBC, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_ARC4, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_SHA1, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_MD5, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0);
	crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0);

	return 0;

out:
	pasemi_dma_remove(pdev);
	return ret;
}

#define MAX_RETRIES 5000

static void pasemi_free_tx_resources(struct pasemi_softc *sc, int chan)
{
	struct pasemi_fnu_txring *ring = &sc->tx[chan];
	int chan_index = chan + sc->base_chan;
	int retries;
	u32 stat;

	/* Stop the channel */
	out_le32(sc->dma_regs +
		 PAS_DMA_TXCHAN_TCMDSTA(chan_index),
		 PAS_DMA_TXCHAN_TCMDSTA_ST);

	for (retries = 0; retries < MAX_RETRIES; retries++) {
		stat = in_le32(sc->dma_regs +
			       PAS_DMA_TXCHAN_TCMDSTA(chan_index));
		if (!(stat & PAS_DMA_TXCHAN_TCMDSTA_ACT))
			break;
		cond_resched();
	}

	if (stat & PAS_DMA_TXCHAN_TCMDSTA_ACT)
		dev_err(&sc->dma_pdev->dev, "Failed to stop tx channel %d\n",
			chan_index);

	/* Disable the channel */
	out_le32(sc->dma_regs +
		 PAS_DMA_TXCHAN_TCMDSTA(chan_index),
		 0);

	if (ring->desc_info)
		kfree((void *) ring->desc_info);
	if (ring->desc)
		dma_free_coherent(&sc->dma_pdev->dev,
				  TX_RING_SIZE *
				  2 * sizeof(u64),
				  (void *) ring->desc, ring->dma);
	if (ring->irq != -1)
		free_irq(ring->irq, sc);

	del_timer(&ring->crypto_timer);
}

static void __devexit pasemi_dma_remove(struct pci_dev *pdev)
{
	struct pasemi_softc *sc = pci_get_drvdata(pdev);
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	if (sc->sc_cid >= 0) {
		crypto_unregister_all(sc->sc_cid);
	}

	if (sc->tx) {
		for (i = 0; i < sc->sc_num_channels; i++)
			pasemi_free_tx_resources(sc, i);

		kfree(sc->tx);
	}
	if (sc->sc_sessions) {
		for (i = 0; i < sc->sc_nsessions; i++)
			kfree(sc->sc_sessions[i]);
		kfree(sc->sc_sessions);
	}
	if (sc->iob_pdev)
		pci_dev_put(sc->iob_pdev);
	if (sc->dma_regs)
		iounmap(sc->dma_regs);
	if (sc->iob_regs)
		iounmap(sc->iob_regs);
	kfree(sc);
}

static struct pci_device_id pasemi_dma_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_PASEMI, 0xa007) },
};

MODULE_DEVICE_TABLE(pci, pasemi_dma_pci_tbl);

static struct pci_driver pasemi_dma_driver = {
	.name		= "pasemi_dma",
	.id_table	= pasemi_dma_pci_tbl,
	.probe		= pasemi_dma_probe,
	.remove		= __devexit_p(pasemi_dma_remove),
};

static void __exit pasemi_dma_cleanup_module(void)
{
	pci_unregister_driver(&pasemi_dma_driver);
	__iounmap(dma_status);
	dma_status = NULL;
}

int pasemi_dma_init_module(void)
{
	return pci_register_driver(&pasemi_dma_driver);
}

module_init(pasemi_dma_init_module);
module_exit(pasemi_dma_cleanup_module);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Egor Martovetsky egor@pasemi.com");
MODULE_DESCRIPTION("OCF driver for PA Semi PWRficient DMA Crypto Engine");
