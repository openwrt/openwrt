/*	$OpenBSD: ubsec.c,v 1.140 2007/10/01 15:34:48 krw Exp $	*/

/*
 * Copyright (c) 2000 Jason L. Wright (jason@thought.net)
 * Copyright (c) 2000 Theo de Raadt (deraadt@openbsd.org)
 * Copyright (c) 2001 Patrik Lindergren (patrik@ipunplugged.com)
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */

#undef UBSEC_DEBUG

/*
 * uBsec 5[56]01, 58xx hardware crypto accelerator
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/device.h>
#include <sys/queue.h>

#include <crypto/cryptodev.h>
#include <crypto/cryptosoft.h>
#include <dev/rndvar.h>
#include <crypto/md5.h>
#include <crypto/sha1.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>

#include <dev/pci/ubsecreg.h>
#include <dev/pci/ubsecvar.h>

/*
 * Prototypes and count for the pci_device structure
 */
int ubsec_probe(struct device *, void *, void *);
void ubsec_attach(struct device *, struct device *, void *);
void ubsec_reset_board(struct ubsec_softc *);
void ubsec_init_board(struct ubsec_softc *);
void ubsec_init_pciregs(struct pci_attach_args *pa);
void ubsec_cleanchip(struct ubsec_softc *);
void ubsec_totalreset(struct ubsec_softc *);
int  ubsec_free_q(struct ubsec_softc*, struct ubsec_q *);

struct cfattach ubsec_ca = {
	sizeof(struct ubsec_softc), ubsec_probe, ubsec_attach,
};

struct cfdriver ubsec_cd = {
	0, "ubsec", DV_DULL
};

int	ubsec_intr(void *);
int	ubsec_newsession(u_int32_t *, struct cryptoini *);
int	ubsec_freesession(u_int64_t);
int	ubsec_process(struct cryptop *);
void	ubsec_callback(struct ubsec_softc *, struct ubsec_q *);
void	ubsec_feed(struct ubsec_softc *);
void	ubsec_mcopy(struct mbuf *, struct mbuf *, int, int);
void	ubsec_callback2(struct ubsec_softc *, struct ubsec_q2 *);
void	ubsec_feed2(struct ubsec_softc *);
void	ubsec_rng(void *);
int	ubsec_dma_malloc(struct ubsec_softc *, bus_size_t,
    struct ubsec_dma_alloc *, int);
void	ubsec_dma_free(struct ubsec_softc *, struct ubsec_dma_alloc *);
int	ubsec_dmamap_aligned(bus_dmamap_t);

int	ubsec_kprocess(struct cryptkop *);
struct ubsec_softc *ubsec_kfind(struct cryptkop *);
int	ubsec_kprocess_modexp_sw(struct ubsec_softc *, struct cryptkop *);
int	ubsec_kprocess_modexp_hw(struct ubsec_softc *, struct cryptkop *);
int	ubsec_kprocess_rsapriv(struct ubsec_softc *, struct cryptkop *);
void	ubsec_kfree(struct ubsec_softc *, struct ubsec_q2 *);
int	ubsec_ksigbits(struct crparam *);
void	ubsec_kshift_r(u_int, u_int8_t *, u_int, u_int8_t *, u_int);
void	ubsec_kshift_l(u_int, u_int8_t *, u_int, u_int8_t *, u_int);

/* DEBUG crap... */
void ubsec_dump_pb(struct ubsec_pktbuf *);
void ubsec_dump_mcr(struct ubsec_mcr *);
void ubsec_dump_ctx2(struct ubsec_ctx_keyop *);

#define	READ_REG(sc,r) \
	bus_space_read_4((sc)->sc_st, (sc)->sc_sh, (r))

#define WRITE_REG(sc,reg,val) \
	bus_space_write_4((sc)->sc_st, (sc)->sc_sh, reg, val)

#define	SWAP32(x) (x) = htole32(ntohl((x)))
#define	HTOLE32(x) (x) = htole32(x)


struct ubsec_stats ubsecstats;

const struct pci_matchid ubsec_devices[] = {
	{ PCI_VENDOR_BLUESTEEL, PCI_PRODUCT_BLUESTEEL_5501 },
	{ PCI_VENDOR_BLUESTEEL, PCI_PRODUCT_BLUESTEEL_5601 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5801 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5802 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5805 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5820 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5821 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5822 },
	{ PCI_VENDOR_BROADCOM, PCI_PRODUCT_BROADCOM_5823 },
	{ PCI_VENDOR_SUN, PCI_PRODUCT_SUN_SCA1K },
	{ PCI_VENDOR_SUN, PCI_PRODUCT_SUN_5821 },
};

int
ubsec_probe(struct device *parent, void *match, void *aux)
{
	return (pci_matchbyid((struct pci_attach_args *)aux, ubsec_devices,
	    sizeof(ubsec_devices)/sizeof(ubsec_devices[0])));
}

void
ubsec_attach(struct device *parent, struct device *self, void *aux)
{
	struct ubsec_softc *sc = (struct ubsec_softc *)self;
	struct pci_attach_args *pa = aux;
	pci_chipset_tag_t pc = pa->pa_pc;
	pci_intr_handle_t ih;
	const char *intrstr = NULL;
	struct ubsec_dma *dmap;
	bus_size_t iosize;
	u_int32_t i;
	int algs[CRYPTO_ALGORITHM_MAX + 1];
	int kalgs[CRK_ALGORITHM_MAX + 1];

	SIMPLEQ_INIT(&sc->sc_queue);
	SIMPLEQ_INIT(&sc->sc_qchip);
	SIMPLEQ_INIT(&sc->sc_queue2);
	SIMPLEQ_INIT(&sc->sc_qchip2);
	SIMPLEQ_INIT(&sc->sc_q2free);

	sc->sc_statmask = BS_STAT_MCR1_DONE | BS_STAT_DMAERR;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_BLUESTEEL &&
	    PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BLUESTEEL_5601)
		sc->sc_flags |= UBS_FLAGS_KEY | UBS_FLAGS_RNG;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_BROADCOM &&
	    (PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5802 ||
	     PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5805))
		sc->sc_flags |= UBS_FLAGS_KEY | UBS_FLAGS_RNG;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_BROADCOM &&
	    (PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5820 ||
	     PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5822 ||
	     PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5823))
		sc->sc_flags |= UBS_FLAGS_KEY | UBS_FLAGS_RNG |
		    UBS_FLAGS_LONGCTX | UBS_FLAGS_HWNORM | UBS_FLAGS_BIGKEY;

	if ((PCI_VENDOR(pa->pa_id) == PCI_VENDOR_BROADCOM &&
	     PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_BROADCOM_5821) ||
	    (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_SUN &&
	     (PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_SUN_SCA1K ||
	      PCI_PRODUCT(pa->pa_id) == PCI_PRODUCT_SUN_5821))) {
		sc->sc_statmask |= BS_STAT_MCR1_ALLEMPTY |
		    BS_STAT_MCR2_ALLEMPTY;
		sc->sc_flags |= UBS_FLAGS_KEY | UBS_FLAGS_RNG |
		    UBS_FLAGS_LONGCTX | UBS_FLAGS_HWNORM | UBS_FLAGS_BIGKEY;
	}

	if (pci_mapreg_map(pa, BS_BAR, PCI_MAPREG_TYPE_MEM, 0,
	    &sc->sc_st, &sc->sc_sh, NULL, &iosize, 0)) {
		printf(": can't find mem space\n");
		return;
	}
	sc->sc_dmat = pa->pa_dmat;

	if (pci_intr_map(pa, &ih)) {
		printf(": couldn't map interrupt\n");
		bus_space_unmap(sc->sc_st, sc->sc_sh, iosize);
		return;
	}
	intrstr = pci_intr_string(pc, ih);
	sc->sc_ih = pci_intr_establish(pc, ih, IPL_NET, ubsec_intr, sc,
	    self->dv_xname);
	if (sc->sc_ih == NULL) {
		printf(": couldn't establish interrupt");
		if (intrstr != NULL)
			printf(" at %s", intrstr);
		printf("\n");
		bus_space_unmap(sc->sc_st, sc->sc_sh, iosize);
		return;
	}

	sc->sc_cid = crypto_get_driverid(0);
	if (sc->sc_cid < 0) {
		pci_intr_disestablish(pc, sc->sc_ih);
		bus_space_unmap(sc->sc_st, sc->sc_sh, iosize);
		return;
	}

	SIMPLEQ_INIT(&sc->sc_freequeue);
	dmap = sc->sc_dmaa;
	for (i = 0; i < UBS_MAX_NQUEUE; i++, dmap++) {
		struct ubsec_q *q;

		q = (struct ubsec_q *)malloc(sizeof(struct ubsec_q),
		    M_DEVBUF, M_NOWAIT);
		if (q == NULL) {
			printf(": can't allocate queue buffers\n");
			break;
		}

		if (ubsec_dma_malloc(sc, sizeof(struct ubsec_dmachunk),
		    &dmap->d_alloc, 0)) {
			printf(": can't allocate dma buffers\n");
			free(q, M_DEVBUF);
			break;
		}
		dmap->d_dma = (struct ubsec_dmachunk *)dmap->d_alloc.dma_vaddr;

		q->q_dma = dmap;
		sc->sc_queuea[i] = q;

		SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
	}

	bzero(algs, sizeof(algs));
	algs[CRYPTO_3DES_CBC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_DES_CBC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_MD5_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	algs[CRYPTO_SHA1_HMAC] = CRYPTO_ALG_FLAG_SUPPORTED;
	crypto_register(sc->sc_cid, algs, ubsec_newsession,
	    ubsec_freesession, ubsec_process);

	/*
	 * Reset Broadcom chip
	 */
	ubsec_reset_board(sc);

	/*
	 * Init Broadcom specific PCI settings
	 */
	ubsec_init_pciregs(pa);

	/*
	 * Init Broadcom chip
	 */
	ubsec_init_board(sc);

	printf(": 3DES MD5 SHA1");

#ifndef UBSEC_NO_RNG
	if (sc->sc_flags & UBS_FLAGS_RNG) {
		sc->sc_statmask |= BS_STAT_MCR2_DONE;

		if (ubsec_dma_malloc(sc, sizeof(struct ubsec_mcr),
		    &sc->sc_rng.rng_q.q_mcr, 0))
			goto skip_rng;

		if (ubsec_dma_malloc(sc, sizeof(struct ubsec_ctx_rngbypass),
		    &sc->sc_rng.rng_q.q_ctx, 0)) {
			ubsec_dma_free(sc, &sc->sc_rng.rng_q.q_mcr);
			goto skip_rng;
		}

		if (ubsec_dma_malloc(sc, sizeof(u_int32_t) *
		    UBSEC_RNG_BUFSIZ, &sc->sc_rng.rng_buf, 0)) {
			ubsec_dma_free(sc, &sc->sc_rng.rng_q.q_ctx);
			ubsec_dma_free(sc, &sc->sc_rng.rng_q.q_mcr);
			goto skip_rng;
		}

		timeout_set(&sc->sc_rngto, ubsec_rng, sc);
		if (hz >= 100)
			sc->sc_rnghz = hz / 100;
		else
			sc->sc_rnghz = 1;
		timeout_add(&sc->sc_rngto, sc->sc_rnghz);
		printf(" RNG");
skip_rng:
	;
	}
#endif /* UBSEC_NO_RNG */

	if (sc->sc_flags & UBS_FLAGS_KEY) {
		sc->sc_statmask |= BS_STAT_MCR2_DONE;

		bzero(kalgs, sizeof(kalgs));
		kalgs[CRK_MOD_EXP] = CRYPTO_ALG_FLAG_SUPPORTED;
#if 0
		kalgs[CRK_MOD_EXP_CRT] = CRYPTO_ALG_FLAG_SUPPORTED;
#endif

		crypto_kregister(sc->sc_cid, kalgs, ubsec_kprocess);
		printf(" PK");
	}

	printf(", %s\n", intrstr);
}

/*
 * UBSEC Interrupt routine
 */
int
ubsec_intr(void *arg)
{
	struct ubsec_softc *sc = arg;
	volatile u_int32_t stat;
	struct ubsec_q *q;
	struct ubsec_dma *dmap;
	int npkts = 0, i;

	stat = READ_REG(sc, BS_STAT);

	stat &= sc->sc_statmask;
	if (stat == 0)
		return (0);

	WRITE_REG(sc, BS_STAT, stat);		/* IACK */

	/*
	 * Check to see if we have any packets waiting for us
	 */
	if ((stat & BS_STAT_MCR1_DONE)) {
		while (!SIMPLEQ_EMPTY(&sc->sc_qchip)) {
			q = SIMPLEQ_FIRST(&sc->sc_qchip);
			dmap = q->q_dma;

			if ((dmap->d_dma->d_mcr.mcr_flags & htole16(UBS_MCR_DONE)) == 0)
				break;

			SIMPLEQ_REMOVE_HEAD(&sc->sc_qchip, q_next);

			npkts = q->q_nstacked_mcrs;
			/*
			 * search for further sc_qchip ubsec_q's that share
			 * the same MCR, and complete them too, they must be
			 * at the top.
			 */
			for (i = 0; i < npkts; i++) {
				if(q->q_stacked_mcr[i])
					ubsec_callback(sc, q->q_stacked_mcr[i]);
				else
					break;
			}
			ubsec_callback(sc, q);
		}

		/*
		 * Don't send any more packet to chip if there has been
		 * a DMAERR.
		 */
		if (!(stat & BS_STAT_DMAERR))
			ubsec_feed(sc);
	}

	/*
	 * Check to see if we have any key setups/rng's waiting for us
	 */
	if ((sc->sc_flags & (UBS_FLAGS_KEY|UBS_FLAGS_RNG)) &&
	    (stat & BS_STAT_MCR2_DONE)) {
		struct ubsec_q2 *q2;
		struct ubsec_mcr *mcr;

		while (!SIMPLEQ_EMPTY(&sc->sc_qchip2)) {
			q2 = SIMPLEQ_FIRST(&sc->sc_qchip2);

			bus_dmamap_sync(sc->sc_dmat, q2->q_mcr.dma_map,
			    0, q2->q_mcr.dma_map->dm_mapsize,
			    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

			mcr = (struct ubsec_mcr *)q2->q_mcr.dma_vaddr;
			if ((mcr->mcr_flags & htole16(UBS_MCR_DONE)) == 0) {
				bus_dmamap_sync(sc->sc_dmat,
				    q2->q_mcr.dma_map, 0,
				    q2->q_mcr.dma_map->dm_mapsize,
				    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);
				break;
			}
			SIMPLEQ_REMOVE_HEAD(&sc->sc_qchip2, q_next);
			ubsec_callback2(sc, q2);
			/*
			 * Don't send any more packet to chip if there has been
			 * a DMAERR.
			 */
			if (!(stat & BS_STAT_DMAERR))
				ubsec_feed2(sc);
		}
	}

	/*
	 * Check to see if we got any DMA Error
	 */
	if (stat & BS_STAT_DMAERR) {
#ifdef UBSEC_DEBUG
		volatile u_int32_t a = READ_REG(sc, BS_ERR);

		printf("%s: dmaerr %s@%08x\n", sc->sc_dv.dv_xname,
		    (a & BS_ERR_READ) ? "read" : "write", a & BS_ERR_ADDR);
#endif /* UBSEC_DEBUG */
		ubsecstats.hst_dmaerr++;
		ubsec_totalreset(sc);
		ubsec_feed(sc);
	}

	return (1);
}

/*
 * ubsec_feed() - aggregate and post requests to chip
 *		  It is assumed that the caller set splnet()
 */
void
ubsec_feed(struct ubsec_softc *sc)
{
#ifdef UBSEC_DEBUG
	static int max;
#endif /* UBSEC_DEBUG */
	struct ubsec_q *q, *q2;
	int npkts, i;
	void *v;
	u_int32_t stat;

	npkts = sc->sc_nqueue;
	if (npkts > UBS_MAX_AGGR)
		npkts = UBS_MAX_AGGR;
	if (npkts < 2)
		goto feed1;

	if ((stat = READ_REG(sc, BS_STAT)) & (BS_STAT_MCR1_FULL | BS_STAT_DMAERR)) {
		if(stat & BS_STAT_DMAERR) {
			ubsec_totalreset(sc);
			ubsecstats.hst_dmaerr++;
		}
		return;
	}

#ifdef UBSEC_DEBUG
	printf("merging %d records\n", npkts);

	/* XXX temporary aggregation statistics reporting code */
	if (max < npkts) {
		max = npkts;
		printf("%s: new max aggregate %d\n", sc->sc_dv.dv_xname, max);
	}
#endif /* UBSEC_DEBUG */

	q = SIMPLEQ_FIRST(&sc->sc_queue);
	SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
	--sc->sc_nqueue;

	bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
	    0, q->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	if (q->q_dst_map != NULL)
		bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
		    0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);

	q->q_nstacked_mcrs = npkts - 1;		/* Number of packets stacked */

	for (i = 0; i < q->q_nstacked_mcrs; i++) {
		q2 = SIMPLEQ_FIRST(&sc->sc_queue);
		bus_dmamap_sync(sc->sc_dmat, q2->q_src_map,
		    0, q2->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
		if (q2->q_dst_map != NULL)
			bus_dmamap_sync(sc->sc_dmat, q2->q_dst_map,
			    0, q2->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);
		SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
		--sc->sc_nqueue;

		v = ((char *)&q2->q_dma->d_dma->d_mcr) + sizeof(struct ubsec_mcr) -
		    sizeof(struct ubsec_mcr_add);
		bcopy(v, &q->q_dma->d_dma->d_mcradd[i], sizeof(struct ubsec_mcr_add));
		q->q_stacked_mcr[i] = q2;
	}
	q->q_dma->d_dma->d_mcr.mcr_pkts = htole16(npkts);
	SIMPLEQ_INSERT_TAIL(&sc->sc_qchip, q, q_next);
	bus_dmamap_sync(sc->sc_dmat, q->q_dma->d_alloc.dma_map,
	    0, q->q_dma->d_alloc.dma_map->dm_mapsize,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
	WRITE_REG(sc, BS_MCR1, q->q_dma->d_alloc.dma_paddr +
	    offsetof(struct ubsec_dmachunk, d_mcr));
	return;

feed1:
	while (!SIMPLEQ_EMPTY(&sc->sc_queue)) {
		if ((stat = READ_REG(sc, BS_STAT)) &
		    (BS_STAT_MCR1_FULL | BS_STAT_DMAERR)) {
			if(stat & BS_STAT_DMAERR) {
				ubsec_totalreset(sc);
				ubsecstats.hst_dmaerr++;
			}
			break;
		}

		q = SIMPLEQ_FIRST(&sc->sc_queue);

		bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
		    0, q->q_src_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
		if (q->q_dst_map != NULL)
			bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
			    0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_PREREAD);
		bus_dmamap_sync(sc->sc_dmat, q->q_dma->d_alloc.dma_map,
		    0, q->q_dma->d_alloc.dma_map->dm_mapsize,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);

		WRITE_REG(sc, BS_MCR1, q->q_dma->d_alloc.dma_paddr +
		    offsetof(struct ubsec_dmachunk, d_mcr));
#ifdef UBSEC_DEBUG
		printf("feed: q->chip %p %08x\n", q,
		    (u_int32_t)q->q_dma->d_alloc.dma_paddr);
#endif /* UBSEC_DEBUG */
		SIMPLEQ_REMOVE_HEAD(&sc->sc_queue, q_next);
		--sc->sc_nqueue;
		SIMPLEQ_INSERT_TAIL(&sc->sc_qchip, q, q_next);
	}
}

/*
 * Allocate a new 'session' and return an encoded session id.  'sidp'
 * contains our registration id, and should contain an encoded session
 * id on successful allocation.
 */
int
ubsec_newsession(u_int32_t *sidp, struct cryptoini *cri)
{
	struct cryptoini *c, *encini = NULL, *macini = NULL;
	struct ubsec_softc *sc = NULL;
	struct ubsec_session *ses = NULL;
	MD5_CTX md5ctx;
	SHA1_CTX sha1ctx;
	int i, sesn;

	if (sidp == NULL || cri == NULL)
		return (EINVAL);

	for (i = 0; i < ubsec_cd.cd_ndevs; i++) {
		sc = ubsec_cd.cd_devs[i];
		if (sc == NULL || sc->sc_cid == (*sidp))
			break;
	}
	if (sc == NULL)
		return (EINVAL);

	for (c = cri; c != NULL; c = c->cri_next) {
		if (c->cri_alg == CRYPTO_MD5_HMAC ||
		    c->cri_alg == CRYPTO_SHA1_HMAC) {
			if (macini)
				return (EINVAL);
			macini = c;
		} else if (c->cri_alg == CRYPTO_DES_CBC ||
		    c->cri_alg == CRYPTO_3DES_CBC) {
			if (encini)
				return (EINVAL);
			encini = c;
		} else
			return (EINVAL);
	}
	if (encini == NULL && macini == NULL)
		return (EINVAL);

	if (sc->sc_sessions == NULL) {
		ses = sc->sc_sessions = (struct ubsec_session *)malloc(
		    sizeof(struct ubsec_session), M_DEVBUF, M_NOWAIT);
		if (ses == NULL)
			return (ENOMEM);
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
			ses = (struct ubsec_session *)malloc((sesn + 1) *
			    sizeof(struct ubsec_session), M_DEVBUF, M_NOWAIT);
			if (ses == NULL)
				return (ENOMEM);
			bcopy(sc->sc_sessions, ses, sesn *
			    sizeof(struct ubsec_session));
			bzero(sc->sc_sessions, sesn *
			    sizeof(struct ubsec_session));
			free(sc->sc_sessions, M_DEVBUF);
			sc->sc_sessions = ses;
			ses = &sc->sc_sessions[sesn];
			sc->sc_nsessions++;
		}
	}

	bzero(ses, sizeof(struct ubsec_session));
	ses->ses_used = 1;
	if (encini) {
		/* get an IV, network byte order */
		arc4random_bytes(ses->ses_iv, sizeof(ses->ses_iv));

		/* Go ahead and compute key in ubsec's byte order */
		if (encini->cri_alg == CRYPTO_DES_CBC) {
			bcopy(encini->cri_key, &ses->ses_deskey[0], 8);
			bcopy(encini->cri_key, &ses->ses_deskey[2], 8);
			bcopy(encini->cri_key, &ses->ses_deskey[4], 8);
		} else
			bcopy(encini->cri_key, ses->ses_deskey, 24);

		SWAP32(ses->ses_deskey[0]);
		SWAP32(ses->ses_deskey[1]);
		SWAP32(ses->ses_deskey[2]);
		SWAP32(ses->ses_deskey[3]);
		SWAP32(ses->ses_deskey[4]);
		SWAP32(ses->ses_deskey[5]);
	}

	if (macini) {
		for (i = 0; i < macini->cri_klen / 8; i++)
			macini->cri_key[i] ^= HMAC_IPAD_VAL;

		if (macini->cri_alg == CRYPTO_MD5_HMAC) {
			MD5Init(&md5ctx);
			MD5Update(&md5ctx, macini->cri_key,
			    macini->cri_klen / 8);
			MD5Update(&md5ctx, hmac_ipad_buffer,
			    HMAC_BLOCK_LEN - (macini->cri_klen / 8));
			bcopy(md5ctx.state, ses->ses_hminner,
			    sizeof(md5ctx.state));
		} else {
			SHA1Init(&sha1ctx);
			SHA1Update(&sha1ctx, macini->cri_key,
			    macini->cri_klen / 8);
			SHA1Update(&sha1ctx, hmac_ipad_buffer,
			    HMAC_BLOCK_LEN - (macini->cri_klen / 8));
			bcopy(sha1ctx.state, ses->ses_hminner,
			    sizeof(sha1ctx.state));
		}

		for (i = 0; i < macini->cri_klen / 8; i++)
			macini->cri_key[i] ^= (HMAC_IPAD_VAL ^ HMAC_OPAD_VAL);

		if (macini->cri_alg == CRYPTO_MD5_HMAC) {
			MD5Init(&md5ctx);
			MD5Update(&md5ctx, macini->cri_key,
			    macini->cri_klen / 8);
			MD5Update(&md5ctx, hmac_opad_buffer,
			    HMAC_BLOCK_LEN - (macini->cri_klen / 8));
			bcopy(md5ctx.state, ses->ses_hmouter,
			    sizeof(md5ctx.state));
		} else {
			SHA1Init(&sha1ctx);
			SHA1Update(&sha1ctx, macini->cri_key,
			    macini->cri_klen / 8);
			SHA1Update(&sha1ctx, hmac_opad_buffer,
			    HMAC_BLOCK_LEN - (macini->cri_klen / 8));
			bcopy(sha1ctx.state, ses->ses_hmouter,
			    sizeof(sha1ctx.state));
		}

		for (i = 0; i < macini->cri_klen / 8; i++)
			macini->cri_key[i] ^= HMAC_OPAD_VAL;
	}

	*sidp = UBSEC_SID(sc->sc_dv.dv_unit, sesn);
	return (0);
}

/*
 * Deallocate a session.
 */
int
ubsec_freesession(u_int64_t tid)
{
	struct ubsec_softc *sc;
	int card, session;
	u_int32_t sid = ((u_int32_t)tid) & 0xffffffff;

	card = UBSEC_CARD(sid);
	if (card >= ubsec_cd.cd_ndevs || ubsec_cd.cd_devs[card] == NULL)
		return (EINVAL);
	sc = ubsec_cd.cd_devs[card];
	session = UBSEC_SESSION(sid);
	bzero(&sc->sc_sessions[session], sizeof(sc->sc_sessions[session]));
	return (0);
}

int
ubsec_process(struct cryptop *crp)
{
	struct ubsec_q *q = NULL;
	int card, err = 0, i, j, s, nicealign;
	struct ubsec_softc *sc;
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;
	int encoffset = 0, macoffset = 0, cpskip, cpoffset;
	int sskip, dskip, stheend, dtheend;
	int16_t coffset;
	struct ubsec_session *ses;
	struct ubsec_pktctx ctx;
	struct ubsec_dma *dmap = NULL;

	if (crp == NULL || crp->crp_callback == NULL) {
		ubsecstats.hst_invalid++;
		return (EINVAL);
	}
	card = UBSEC_CARD(crp->crp_sid);
	if (card >= ubsec_cd.cd_ndevs || ubsec_cd.cd_devs[card] == NULL) {
		ubsecstats.hst_invalid++;
		return (EINVAL);
	}

	sc = ubsec_cd.cd_devs[card];

	s = splnet();

	if (SIMPLEQ_EMPTY(&sc->sc_freequeue)) {
		ubsecstats.hst_queuefull++;
		splx(s);
		err = ENOMEM;
		goto errout2;
	}

	q = SIMPLEQ_FIRST(&sc->sc_freequeue);
	SIMPLEQ_REMOVE_HEAD(&sc->sc_freequeue, q_next);
	splx(s);

	dmap = q->q_dma; /* Save dma pointer */
	bzero(q, sizeof(struct ubsec_q));
	bzero(&ctx, sizeof(ctx));

	q->q_sesn = UBSEC_SESSION(crp->crp_sid);
	q->q_dma = dmap;
	ses = &sc->sc_sessions[q->q_sesn];

	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		q->q_src_m = (struct mbuf *)crp->crp_buf;
		q->q_dst_m = (struct mbuf *)crp->crp_buf;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		q->q_src_io = (struct uio *)crp->crp_buf;
		q->q_dst_io = (struct uio *)crp->crp_buf;
	} else {
		err = EINVAL;
		goto errout;	/* XXX we don't handle contiguous blocks! */
	}

	bzero(&dmap->d_dma->d_mcr, sizeof(struct ubsec_mcr));

	dmap->d_dma->d_mcr.mcr_pkts = htole16(1);
	dmap->d_dma->d_mcr.mcr_flags = 0;
	q->q_crp = crp;

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		err = EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;

	if (crd2 == NULL) {
		if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC) {
			maccrd = crd1;
			enccrd = NULL;
		} else if (crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC) {
			maccrd = NULL;
			enccrd = crd1;
		} else {
			err = EINVAL;
			goto errout;
		}
	} else {
		if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC) &&
		    (crd2->crd_alg == CRYPTO_DES_CBC ||
			crd2->crd_alg == CRYPTO_3DES_CBC) &&
		    ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
			maccrd = crd1;
			enccrd = crd2;
		} else if ((crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC) &&
		    (crd2->crd_alg == CRYPTO_MD5_HMAC ||
			crd2->crd_alg == CRYPTO_SHA1_HMAC) &&
		    (crd1->crd_flags & CRD_F_ENCRYPT)) {
			enccrd = crd1;
			maccrd = crd2;
		} else {
			/*
			 * We cannot order the ubsec as requested
			 */
			err = EINVAL;
			goto errout;
		}
	}

	if (enccrd) {
		encoffset = enccrd->crd_skip;
		ctx.pc_flags |= htole16(UBS_PKTCTX_ENC_3DES);

		if (enccrd->crd_flags & CRD_F_ENCRYPT) {
			q->q_flags |= UBSEC_QFLAGS_COPYOUTIV;

			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				bcopy(enccrd->crd_iv, ctx.pc_iv, 8);
			else {
				ctx.pc_iv[0] = ses->ses_iv[0];
				ctx.pc_iv[1] = ses->ses_iv[1];
			}

			if ((enccrd->crd_flags & CRD_F_IV_PRESENT) == 0) {
				if (crp->crp_flags & CRYPTO_F_IMBUF)
					m_copyback(q->q_src_m,
					    enccrd->crd_inject,
					    8, ctx.pc_iv);
				else if (crp->crp_flags & CRYPTO_F_IOV)
					cuio_copyback(q->q_src_io,
					    enccrd->crd_inject,
					    8, ctx.pc_iv);
			}
		} else {
			ctx.pc_flags |= htole16(UBS_PKTCTX_INBOUND);

			if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
				bcopy(enccrd->crd_iv, ctx.pc_iv, 8);
			else if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata(q->q_src_m, enccrd->crd_inject,
				    8, (caddr_t)ctx.pc_iv);
			else if (crp->crp_flags & CRYPTO_F_IOV)
				cuio_copydata(q->q_src_io,
				    enccrd->crd_inject, 8,
				    (caddr_t)ctx.pc_iv);
		}

		ctx.pc_deskey[0] = ses->ses_deskey[0];
		ctx.pc_deskey[1] = ses->ses_deskey[1];
		ctx.pc_deskey[2] = ses->ses_deskey[2];
		ctx.pc_deskey[3] = ses->ses_deskey[3];
		ctx.pc_deskey[4] = ses->ses_deskey[4];
		ctx.pc_deskey[5] = ses->ses_deskey[5];
		SWAP32(ctx.pc_iv[0]);
		SWAP32(ctx.pc_iv[1]);
	}

	if (maccrd) {
		macoffset = maccrd->crd_skip;

		if (maccrd->crd_alg == CRYPTO_MD5_HMAC)
			ctx.pc_flags |= htole16(UBS_PKTCTX_AUTH_MD5);
		else
			ctx.pc_flags |= htole16(UBS_PKTCTX_AUTH_SHA1);

		for (i = 0; i < 5; i++) {
			ctx.pc_hminner[i] = ses->ses_hminner[i];
			ctx.pc_hmouter[i] = ses->ses_hmouter[i];

			HTOLE32(ctx.pc_hminner[i]);
			HTOLE32(ctx.pc_hmouter[i]);
		}
	}

	if (enccrd && maccrd) {
		/*
		 * ubsec cannot handle packets where the end of encryption
		 * and authentication are not the same, or where the
		 * encrypted part begins before the authenticated part.
		 */
		if (((encoffset + enccrd->crd_len) !=
		    (macoffset + maccrd->crd_len)) ||
		    (enccrd->crd_skip < maccrd->crd_skip)) {
			err = EINVAL;
			goto errout;
		}
		sskip = maccrd->crd_skip;
		cpskip = dskip = enccrd->crd_skip;
		stheend = maccrd->crd_len;
		dtheend = enccrd->crd_len;
		coffset = enccrd->crd_skip - maccrd->crd_skip;
		cpoffset = cpskip + dtheend;
#ifdef UBSEC_DEBUG
		printf("mac: skip %d, len %d, inject %d\n",
 		    maccrd->crd_skip, maccrd->crd_len, maccrd->crd_inject);
		printf("enc: skip %d, len %d, inject %d\n",
		    enccrd->crd_skip, enccrd->crd_len, enccrd->crd_inject);
		printf("src: skip %d, len %d\n", sskip, stheend);
		printf("dst: skip %d, len %d\n", dskip, dtheend);
		printf("ubs: coffset %d, pktlen %d, cpskip %d, cpoffset %d\n",
		    coffset, stheend, cpskip, cpoffset);
#endif
	} else {
		cpskip = dskip = sskip = macoffset + encoffset;
		dtheend = stheend = (enccrd)?enccrd->crd_len:maccrd->crd_len;
		cpoffset = cpskip + dtheend;
		coffset = 0;
	}
	ctx.pc_offset = htole16(coffset >> 2);

	if (bus_dmamap_create(sc->sc_dmat, 0xfff0, UBS_MAX_SCATTER,
		0xfff0, 0, BUS_DMA_NOWAIT, &q->q_src_map) != 0) {
		err = ENOMEM;
		goto errout;
	}
	if (crp->crp_flags & CRYPTO_F_IMBUF) {
		if (bus_dmamap_load_mbuf(sc->sc_dmat, q->q_src_map,
		    q->q_src_m, BUS_DMA_NOWAIT) != 0) {
			bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
			q->q_src_map = NULL;
			err = ENOMEM;
			goto errout;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		if (bus_dmamap_load_uio(sc->sc_dmat, q->q_src_map,
		    q->q_src_io, BUS_DMA_NOWAIT) != 0) {
			bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
			q->q_src_map = NULL;
			err = ENOMEM;
			goto errout;
		}
	}
	nicealign = ubsec_dmamap_aligned(q->q_src_map);

	dmap->d_dma->d_mcr.mcr_pktlen = htole16(stheend);

#ifdef UBSEC_DEBUG
	printf("src skip: %d\n", sskip);
#endif
	for (i = j = 0; i < q->q_src_map->dm_nsegs; i++) {
		struct ubsec_pktbuf *pb;
		bus_size_t packl = q->q_src_map->dm_segs[i].ds_len;
		bus_addr_t packp = q->q_src_map->dm_segs[i].ds_addr;

		if (sskip >= packl) {
			sskip -= packl;
			continue;
		}

		packl -= sskip;
		packp += sskip;
		sskip = 0;

		if (packl > 0xfffc) {
			err = EIO;
			goto errout;
		}

		if (j == 0)
			pb = &dmap->d_dma->d_mcr.mcr_ipktbuf;
		else
			pb = &dmap->d_dma->d_sbuf[j - 1];

		pb->pb_addr = htole32(packp);

		if (stheend) {
			if (packl > stheend) {
				pb->pb_len = htole32(stheend);
				stheend = 0;
			} else {
				pb->pb_len = htole32(packl);
				stheend -= packl;
			}
		} else
			pb->pb_len = htole32(packl);

		if ((i + 1) == q->q_src_map->dm_nsegs)
			pb->pb_next = 0;
		else
			pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
			    offsetof(struct ubsec_dmachunk, d_sbuf[j]));
		j++;
	}

	if (enccrd == NULL && maccrd != NULL) {
		dmap->d_dma->d_mcr.mcr_opktbuf.pb_addr = 0;
		dmap->d_dma->d_mcr.mcr_opktbuf.pb_len = 0;
		dmap->d_dma->d_mcr.mcr_opktbuf.pb_next =
		    htole32(dmap->d_alloc.dma_paddr +
		    offsetof(struct ubsec_dmachunk, d_macbuf[0]));
#ifdef UBSEC_DEBUG
		printf("opkt: %x %x %x\n",
		    dmap->d_dma->d_mcr.mcr_opktbuf.pb_addr,
		    dmap->d_dma->d_mcr.mcr_opktbuf.pb_len,
		    dmap->d_dma->d_mcr.mcr_opktbuf.pb_next);
#endif
	} else {
		if (crp->crp_flags & CRYPTO_F_IOV) {
			if (!nicealign) {
				err = EINVAL;
				goto errout;
			}
			if (bus_dmamap_create(sc->sc_dmat, 0xfff0,
			    UBS_MAX_SCATTER, 0xfff0, 0, BUS_DMA_NOWAIT,
			    &q->q_dst_map) != 0) {
				err = ENOMEM;
				goto errout;
			}
			if (bus_dmamap_load_uio(sc->sc_dmat, q->q_dst_map,
			    q->q_dst_io, BUS_DMA_NOWAIT) != 0) {
				bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
				q->q_dst_map = NULL;
				goto errout;
			}
		} else if (crp->crp_flags & CRYPTO_F_IMBUF) {
			if (nicealign) {
				q->q_dst_m = q->q_src_m;
				q->q_dst_map = q->q_src_map;
			} else {
				int totlen, len;
				struct mbuf *m, *top, **mp;

				totlen = q->q_src_map->dm_mapsize;
				if (q->q_src_m->m_flags & M_PKTHDR) {
					len = MHLEN;
					MGETHDR(m, M_DONTWAIT, MT_DATA);
				} else {
					len = MLEN;
					MGET(m, M_DONTWAIT, MT_DATA);
				}
				if (m == NULL) {
					err = ENOMEM;
					goto errout;
				}
				if (len == MHLEN)
					M_DUP_PKTHDR(m, q->q_src_m);
				if (totlen >= MINCLSIZE) {
					MCLGET(m, M_DONTWAIT);
					if (m->m_flags & M_EXT)
						len = MCLBYTES;
				}
				m->m_len = len;
				top = NULL;
				mp = &top;

				while (totlen > 0) {
					if (top) {
						MGET(m, M_DONTWAIT, MT_DATA);
						if (m == NULL) {
							m_freem(top);
							err = ENOMEM;
							goto errout;
						}
						len = MLEN;
					}
					if (top && totlen >= MINCLSIZE) {
						MCLGET(m, M_DONTWAIT);
						if (m->m_flags & M_EXT)
							len = MCLBYTES;
					}
					m->m_len = len = min(totlen, len);
					totlen -= len;
					*mp = m;
					mp = &m->m_next;
				}
				q->q_dst_m = top;
				ubsec_mcopy(q->q_src_m, q->q_dst_m,
				    cpskip, cpoffset);
				if (bus_dmamap_create(sc->sc_dmat, 0xfff0,
				    UBS_MAX_SCATTER, 0xfff0, 0, BUS_DMA_NOWAIT,
				    &q->q_dst_map) != 0) {
					err = ENOMEM;
					goto errout;
				}
				if (bus_dmamap_load_mbuf(sc->sc_dmat,
				    q->q_dst_map, q->q_dst_m,
				    BUS_DMA_NOWAIT) != 0) {
					bus_dmamap_destroy(sc->sc_dmat,
					q->q_dst_map);
					q->q_dst_map = NULL;
					err = ENOMEM;
					goto errout;
				}
			}
		} else {
			err = EINVAL;
			goto errout;
		}

#ifdef UBSEC_DEBUG
		printf("dst skip: %d\n", dskip);
#endif
		for (i = j = 0; i < q->q_dst_map->dm_nsegs; i++) {
			struct ubsec_pktbuf *pb;
			bus_size_t packl = q->q_dst_map->dm_segs[i].ds_len;
			bus_addr_t packp = q->q_dst_map->dm_segs[i].ds_addr;

			if (dskip >= packl) {
				dskip -= packl;
				continue;
			}

			packl -= dskip;
			packp += dskip;
			dskip = 0;

			if (packl > 0xfffc) {
				err = EIO;
				goto errout;
			}

			if (j == 0)
				pb = &dmap->d_dma->d_mcr.mcr_opktbuf;
			else
				pb = &dmap->d_dma->d_dbuf[j - 1];

			pb->pb_addr = htole32(packp);

			if (dtheend) {
				if (packl > dtheend) {
					pb->pb_len = htole32(dtheend);
					dtheend = 0;
				} else {
					pb->pb_len = htole32(packl);
					dtheend -= packl;
				}
			} else
				pb->pb_len = htole32(packl);

			if ((i + 1) == q->q_dst_map->dm_nsegs) {
				if (maccrd)
					pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
					    offsetof(struct ubsec_dmachunk, d_macbuf[0]));
				else
					pb->pb_next = 0;
			} else
				pb->pb_next = htole32(dmap->d_alloc.dma_paddr +
				    offsetof(struct ubsec_dmachunk, d_dbuf[j]));
			j++;
		}
	}

	dmap->d_dma->d_mcr.mcr_cmdctxp = htole32(dmap->d_alloc.dma_paddr +
	    offsetof(struct ubsec_dmachunk, d_ctx));

	if (sc->sc_flags & UBS_FLAGS_LONGCTX) {
		struct ubsec_pktctx_long *ctxl;

		ctxl = (struct ubsec_pktctx_long *)(dmap->d_alloc.dma_vaddr +
		    offsetof(struct ubsec_dmachunk, d_ctx));
		
		/* transform small context into long context */
		ctxl->pc_len = htole16(sizeof(struct ubsec_pktctx_long));
		ctxl->pc_type = htole16(UBS_PKTCTX_TYPE_IPSEC);
		ctxl->pc_flags = ctx.pc_flags;
		ctxl->pc_offset = ctx.pc_offset;
		for (i = 0; i < 6; i++)
			ctxl->pc_deskey[i] = ctx.pc_deskey[i];
		for (i = 0; i < 5; i++)
			ctxl->pc_hminner[i] = ctx.pc_hminner[i];
		for (i = 0; i < 5; i++)
			ctxl->pc_hmouter[i] = ctx.pc_hmouter[i];   
		ctxl->pc_iv[0] = ctx.pc_iv[0];
		ctxl->pc_iv[1] = ctx.pc_iv[1];
	} else
		bcopy(&ctx, dmap->d_alloc.dma_vaddr +
		    offsetof(struct ubsec_dmachunk, d_ctx),
		    sizeof(struct ubsec_pktctx));

	s = splnet();
	SIMPLEQ_INSERT_TAIL(&sc->sc_queue, q, q_next);
	sc->sc_nqueue++;
	ubsecstats.hst_ipackets++;
	ubsecstats.hst_ibytes += dmap->d_alloc.dma_map->dm_mapsize;
	ubsec_feed(sc);
	splx(s);
	return (0);

errout:
	if (q != NULL) {
		if ((q->q_dst_m != NULL) && (q->q_src_m != q->q_dst_m))
			m_freem(q->q_dst_m);

		if (q->q_dst_map != NULL && q->q_dst_map != q->q_src_map) {
			bus_dmamap_unload(sc->sc_dmat, q->q_dst_map);
			bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
		}
		if (q->q_src_map != NULL) {
			bus_dmamap_unload(sc->sc_dmat, q->q_src_map);
			bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);
		}

		s = splnet();
		SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
		splx(s);
	}
	if (err == EINVAL)
		ubsecstats.hst_invalid++;
	else
		ubsecstats.hst_nomem++;
errout2:
	crp->crp_etype = err;
	crypto_done(crp);
	return (0);
}

void
ubsec_callback(struct ubsec_softc *sc, struct ubsec_q *q)
{
	struct cryptop *crp = (struct cryptop *)q->q_crp;
	struct cryptodesc *crd;
	struct ubsec_dma *dmap = q->q_dma;

	ubsecstats.hst_opackets++;
	ubsecstats.hst_obytes += dmap->d_alloc.dma_size;

	bus_dmamap_sync(sc->sc_dmat, dmap->d_alloc.dma_map, 0,
	    dmap->d_alloc.dma_map->dm_mapsize,
	    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
	if (q->q_dst_map != NULL && q->q_dst_map != q->q_src_map) {
		bus_dmamap_sync(sc->sc_dmat, q->q_dst_map,
		    0, q->q_dst_map->dm_mapsize, BUS_DMASYNC_POSTREAD);
		bus_dmamap_unload(sc->sc_dmat, q->q_dst_map);
		bus_dmamap_destroy(sc->sc_dmat, q->q_dst_map);
	}
	bus_dmamap_sync(sc->sc_dmat, q->q_src_map,
	    0, q->q_src_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);
	bus_dmamap_unload(sc->sc_dmat, q->q_src_map);
	bus_dmamap_destroy(sc->sc_dmat, q->q_src_map);

	if ((crp->crp_flags & CRYPTO_F_IMBUF) && (q->q_src_m != q->q_dst_m)) {
		m_freem(q->q_src_m);
		crp->crp_buf = (caddr_t)q->q_dst_m;
	}

	/* copy out IV for future use */
	if (q->q_flags & UBSEC_QFLAGS_COPYOUTIV) {
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
			if (crd->crd_alg != CRYPTO_DES_CBC &&
			    crd->crd_alg != CRYPTO_3DES_CBC)
				continue;
			if (crp->crp_flags & CRYPTO_F_IMBUF)
				m_copydata((struct mbuf *)crp->crp_buf,
				    crd->crd_skip + crd->crd_len - 8, 8,
				    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);
			else if (crp->crp_flags & CRYPTO_F_IOV) {
				cuio_copydata((struct uio *)crp->crp_buf,
				    crd->crd_skip + crd->crd_len - 8, 8,
				    (caddr_t)sc->sc_sessions[q->q_sesn].ses_iv);
			}
			break;
		}
	}

	for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
		if (crd->crd_alg != CRYPTO_MD5_HMAC &&
		    crd->crd_alg != CRYPTO_SHA1_HMAC)
			continue;
		if (crp->crp_flags & CRYPTO_F_IMBUF)
			m_copyback((struct mbuf *)crp->crp_buf,
			    crd->crd_inject, 12,
			    dmap->d_dma->d_macbuf);
		else if (crp->crp_flags & CRYPTO_F_IOV && crp->crp_mac)
			bcopy((caddr_t)dmap->d_dma->d_macbuf,
			    crp->crp_mac, 12);
		break;
	}
	SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
	crypto_done(crp);
}

void
ubsec_mcopy(struct mbuf *srcm, struct mbuf *dstm, int hoffset, int toffset)
{
	int i, j, dlen, slen;
	caddr_t dptr, sptr;

	j = 0;
	sptr = srcm->m_data;
	slen = srcm->m_len;
	dptr = dstm->m_data;
	dlen = dstm->m_len;

	while (1) {
		for (i = 0; i < min(slen, dlen); i++) {
			if (j < hoffset || j >= toffset)
				*dptr++ = *sptr++;
			slen--;
			dlen--;
			j++;
		}
		if (slen == 0) {
			srcm = srcm->m_next;
			if (srcm == NULL)
				return;
			sptr = srcm->m_data;
			slen = srcm->m_len;
		}
		if (dlen == 0) {
			dstm = dstm->m_next;
			if (dstm == NULL)
				return;
			dptr = dstm->m_data;
			dlen = dstm->m_len;
		}
	}
}

/*
 * feed the key generator, must be called at splnet() or higher.
 */
void
ubsec_feed2(struct ubsec_softc *sc)
{
	struct ubsec_q2 *q;

	while (!SIMPLEQ_EMPTY(&sc->sc_queue2)) {
		if (READ_REG(sc, BS_STAT) & BS_STAT_MCR2_FULL)
			break;
		q = SIMPLEQ_FIRST(&sc->sc_queue2);

		bus_dmamap_sync(sc->sc_dmat, q->q_mcr.dma_map, 0,
		    q->q_mcr.dma_map->dm_mapsize,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
		bus_dmamap_sync(sc->sc_dmat, q->q_ctx.dma_map, 0,
		    q->q_ctx.dma_map->dm_mapsize,
		    BUS_DMASYNC_PREWRITE);

		WRITE_REG(sc, BS_MCR2, q->q_mcr.dma_paddr);
		SIMPLEQ_REMOVE_HEAD(&sc->sc_queue2, q_next);
		--sc->sc_nqueue2;
		SIMPLEQ_INSERT_TAIL(&sc->sc_qchip2, q, q_next);
	}
}

/*
 * Callback for handling random numbers
 */
void
ubsec_callback2(struct ubsec_softc *sc, struct ubsec_q2 *q)
{
	struct cryptkop *krp;
	struct ubsec_ctx_keyop *ctx;

	ctx = (struct ubsec_ctx_keyop *)q->q_ctx.dma_vaddr;
	bus_dmamap_sync(sc->sc_dmat, q->q_ctx.dma_map, 0,
	    q->q_ctx.dma_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);

	switch (q->q_type) {
#ifndef UBSEC_NO_RNG
	case UBS_CTXOP_RNGSHA1:
	case UBS_CTXOP_RNGBYPASS: {
		struct ubsec_q2_rng *rng = (struct ubsec_q2_rng *)q;
		u_int32_t *p;
		int i;

		bus_dmamap_sync(sc->sc_dmat, rng->rng_buf.dma_map, 0,
		    rng->rng_buf.dma_map->dm_mapsize, BUS_DMASYNC_POSTREAD);
		p = (u_int32_t *)rng->rng_buf.dma_vaddr;
		for (i = 0; i < UBSEC_RNG_BUFSIZ; p++, i++)
			add_true_randomness(*p);
		rng->rng_used = 0;
		timeout_add(&sc->sc_rngto, sc->sc_rnghz);
		break;
	}
#endif
	case UBS_CTXOP_MODEXP: {
		struct ubsec_q2_modexp *me = (struct ubsec_q2_modexp *)q;
		u_int rlen, clen;

		krp = me->me_krp;
		rlen = (me->me_modbits + 7) / 8;
		clen = (krp->krp_param[krp->krp_iparams].crp_nbits + 7) / 8;

		bus_dmamap_sync(sc->sc_dmat, me->me_M.dma_map,
		    0, me->me_M.dma_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);
		bus_dmamap_sync(sc->sc_dmat, me->me_E.dma_map,
		    0, me->me_E.dma_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);
		bus_dmamap_sync(sc->sc_dmat, me->me_C.dma_map,
		    0, me->me_C.dma_map->dm_mapsize, BUS_DMASYNC_POSTREAD);
		bus_dmamap_sync(sc->sc_dmat, me->me_epb.dma_map,
		    0, me->me_epb.dma_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);

		if (clen < rlen)
			krp->krp_status = E2BIG;
		else {
			if (sc->sc_flags & UBS_FLAGS_HWNORM) {
				bzero(krp->krp_param[krp->krp_iparams].crp_p,
				    (krp->krp_param[krp->krp_iparams].crp_nbits
					+ 7) / 8);
				bcopy(me->me_C.dma_vaddr,
				    krp->krp_param[krp->krp_iparams].crp_p,
				    (me->me_modbits + 7) / 8);
			} else
				ubsec_kshift_l(me->me_shiftbits,
				    me->me_C.dma_vaddr, me->me_normbits,
				    krp->krp_param[krp->krp_iparams].crp_p,
				    krp->krp_param[krp->krp_iparams].crp_nbits);
		}
		crypto_kdone(krp);

		/* bzero all potentially sensitive data */
		bzero(me->me_E.dma_vaddr, me->me_E.dma_size);
		bzero(me->me_M.dma_vaddr, me->me_M.dma_size);
		bzero(me->me_C.dma_vaddr, me->me_C.dma_size);
		bzero(me->me_q.q_ctx.dma_vaddr, me->me_q.q_ctx.dma_size);

		/* Can't free here, so put us on the free list. */
		SIMPLEQ_INSERT_TAIL(&sc->sc_q2free, &me->me_q, q_next);
		break;
	}
	case UBS_CTXOP_RSAPRIV: {
		struct ubsec_q2_rsapriv *rp = (struct ubsec_q2_rsapriv *)q;
		u_int len;

		krp = rp->rpr_krp;
		bus_dmamap_sync(sc->sc_dmat, rp->rpr_msgin.dma_map, 0,
		    rp->rpr_msgin.dma_map->dm_mapsize, BUS_DMASYNC_POSTWRITE);
		bus_dmamap_sync(sc->sc_dmat, rp->rpr_msgout.dma_map, 0,
		    rp->rpr_msgout.dma_map->dm_mapsize, BUS_DMASYNC_POSTREAD);

		len = (krp->krp_param[UBS_RSAPRIV_PAR_MSGOUT].crp_nbits + 7) / 8;
		bcopy(rp->rpr_msgout.dma_vaddr,
		    krp->krp_param[UBS_RSAPRIV_PAR_MSGOUT].crp_p, len);

		crypto_kdone(krp);

		bzero(rp->rpr_msgin.dma_vaddr, rp->rpr_msgin.dma_size);
		bzero(rp->rpr_msgout.dma_vaddr, rp->rpr_msgout.dma_size);
		bzero(rp->rpr_q.q_ctx.dma_vaddr, rp->rpr_q.q_ctx.dma_size);

		/* Can't free here, so put us on the free list. */
		SIMPLEQ_INSERT_TAIL(&sc->sc_q2free, &rp->rpr_q, q_next);
		break;
	}
	default:
		printf("%s: unknown ctx op: %x\n", sc->sc_dv.dv_xname,
		    letoh16(ctx->ctx_op));
		break;
	}
}

#ifndef UBSEC_NO_RNG
void
ubsec_rng(void *vsc)
{
	struct ubsec_softc *sc = vsc;
	struct ubsec_q2_rng *rng = &sc->sc_rng;
	struct ubsec_mcr *mcr;
	struct ubsec_ctx_rngbypass *ctx;
	int s;

	s = splnet();
	if (rng->rng_used) {
		splx(s);
		return;
	}
	sc->sc_nqueue2++;
	if (sc->sc_nqueue2 >= UBS_MAX_NQUEUE)
		goto out;

	mcr = (struct ubsec_mcr *)rng->rng_q.q_mcr.dma_vaddr;
	ctx = (struct ubsec_ctx_rngbypass *)rng->rng_q.q_ctx.dma_vaddr;

	mcr->mcr_pkts = htole16(1);
	mcr->mcr_flags = 0;
	mcr->mcr_cmdctxp = htole32(rng->rng_q.q_ctx.dma_paddr);
	mcr->mcr_ipktbuf.pb_addr = mcr->mcr_ipktbuf.pb_next = 0;
	mcr->mcr_ipktbuf.pb_len = 0;
	mcr->mcr_reserved = mcr->mcr_pktlen = 0;
	mcr->mcr_opktbuf.pb_addr = htole32(rng->rng_buf.dma_paddr);
	mcr->mcr_opktbuf.pb_len = htole32(((sizeof(u_int32_t) * UBSEC_RNG_BUFSIZ)) &
	    UBS_PKTBUF_LEN);
	mcr->mcr_opktbuf.pb_next = 0;

	ctx->rbp_len = htole16(sizeof(struct ubsec_ctx_rngbypass));
	ctx->rbp_op = htole16(UBS_CTXOP_RNGSHA1);
	rng->rng_q.q_type = UBS_CTXOP_RNGSHA1;

	bus_dmamap_sync(sc->sc_dmat, rng->rng_buf.dma_map, 0,
	    rng->rng_buf.dma_map->dm_mapsize, BUS_DMASYNC_PREREAD);

	SIMPLEQ_INSERT_TAIL(&sc->sc_queue2, &rng->rng_q, q_next);
	rng->rng_used = 1;
	ubsec_feed2(sc);
	splx(s);

	return;

out:
	/*
	 * Something weird happened, generate our own call back.
	 */
	sc->sc_nqueue2--;
	splx(s);
	timeout_add(&sc->sc_rngto, sc->sc_rnghz);
}
#endif /* UBSEC_NO_RNG */

int
ubsec_dma_malloc(struct ubsec_softc *sc, bus_size_t size,
    struct ubsec_dma_alloc *dma, int mapflags)
{
	int r;

	if ((r = bus_dmamem_alloc(sc->sc_dmat, size, PAGE_SIZE, 0,
	    &dma->dma_seg, 1, &dma->dma_nseg, BUS_DMA_NOWAIT)) != 0)
		goto fail_0;

	if ((r = bus_dmamem_map(sc->sc_dmat, &dma->dma_seg, dma->dma_nseg,
	    size, &dma->dma_vaddr, mapflags | BUS_DMA_NOWAIT)) != 0)
		goto fail_1;

	if ((r = bus_dmamap_create(sc->sc_dmat, size, 1, size, 0,
	    BUS_DMA_NOWAIT, &dma->dma_map)) != 0)
		goto fail_2;

	if ((r = bus_dmamap_load(sc->sc_dmat, dma->dma_map, dma->dma_vaddr,
	    size, NULL, BUS_DMA_NOWAIT)) != 0)
		goto fail_3;

	dma->dma_paddr = dma->dma_map->dm_segs[0].ds_addr;
	dma->dma_size = size;
	return (0);

fail_3:
	bus_dmamap_destroy(sc->sc_dmat, dma->dma_map);
fail_2:
	bus_dmamem_unmap(sc->sc_dmat, dma->dma_vaddr, size);
fail_1:
	bus_dmamem_free(sc->sc_dmat, &dma->dma_seg, dma->dma_nseg);
fail_0:
	dma->dma_map = NULL;
	return (r);
}

void
ubsec_dma_free(struct ubsec_softc *sc, struct ubsec_dma_alloc *dma)
{
	bus_dmamap_unload(sc->sc_dmat, dma->dma_map);
	bus_dmamem_unmap(sc->sc_dmat, dma->dma_vaddr, dma->dma_size);
	bus_dmamem_free(sc->sc_dmat, &dma->dma_seg, dma->dma_nseg);
	bus_dmamap_destroy(sc->sc_dmat, dma->dma_map);
}

/*
 * Resets the board.  Values in the regesters are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
void
ubsec_reset_board(struct ubsec_softc *sc)
{
    volatile u_int32_t ctrl;

    ctrl = READ_REG(sc, BS_CTRL);
    ctrl |= BS_CTRL_RESET;
    WRITE_REG(sc, BS_CTRL, ctrl);

    /*
     * Wait aprox. 30 PCI clocks = 900 ns = 0.9 us
     */
    DELAY(10);
}

/*
 * Init Broadcom registers
 */
void
ubsec_init_board(struct ubsec_softc *sc)
{
	u_int32_t ctrl;

	ctrl = READ_REG(sc, BS_CTRL);
	ctrl &= ~(BS_CTRL_BE32 | BS_CTRL_BE64);
	ctrl |= BS_CTRL_LITTLE_ENDIAN | BS_CTRL_MCR1INT;

	if (sc->sc_flags & UBS_FLAGS_KEY)
		ctrl |= BS_CTRL_MCR2INT;
	else
		ctrl &= ~BS_CTRL_MCR2INT;

	if (sc->sc_flags & UBS_FLAGS_HWNORM)
		ctrl &= ~BS_CTRL_SWNORM;

	WRITE_REG(sc, BS_CTRL, ctrl);
}

/*
 * Init Broadcom PCI registers
 */
void
ubsec_init_pciregs(struct pci_attach_args *pa)
{
	pci_chipset_tag_t pc = pa->pa_pc;
	u_int32_t misc;

	/*
	 * This will set the cache line size to 1, this will
	 * force the BCM58xx chip just to do burst read/writes.
	 * Cache line read/writes are to slow
	 */
	misc = pci_conf_read(pc, pa->pa_tag, PCI_BHLC_REG);
	misc = (misc & ~(PCI_CACHELINE_MASK << PCI_CACHELINE_SHIFT))
	    | ((UBS_DEF_CACHELINE & 0xff) << PCI_CACHELINE_SHIFT);
	pci_conf_write(pc, pa->pa_tag, PCI_BHLC_REG, misc);
}

/*
 * Clean up after a chip crash.
 * It is assumed that the caller is in splnet()
 */
void
ubsec_cleanchip(struct ubsec_softc *sc)
{
	struct ubsec_q *q;

	while (!SIMPLEQ_EMPTY(&sc->sc_qchip)) {
		q = SIMPLEQ_FIRST(&sc->sc_qchip);
		SIMPLEQ_REMOVE_HEAD(&sc->sc_qchip, q_next);
		ubsec_free_q(sc, q);
	}
}

/*
 * free a ubsec_q
 * It is assumed that the caller is within splnet()
 */
int
ubsec_free_q(struct ubsec_softc *sc, struct ubsec_q *q)
{
	struct ubsec_q *q2;
	struct cryptop *crp;
	int npkts;
	int i;

	npkts = q->q_nstacked_mcrs;

	for (i = 0; i < npkts; i++) {
		if(q->q_stacked_mcr[i]) {
			q2 = q->q_stacked_mcr[i];

			if ((q2->q_dst_m != NULL) && (q2->q_src_m != q2->q_dst_m)) 
				m_freem(q2->q_dst_m);

			crp = (struct cryptop *)q2->q_crp;
			
			SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q2, q_next);
			
			crp->crp_etype = EFAULT;
			crypto_done(crp);
		} else {
			break;
		}
	}

	/*
	 * Free header MCR
	 */
	if ((q->q_dst_m != NULL) && (q->q_src_m != q->q_dst_m))
		m_freem(q->q_dst_m);

	crp = (struct cryptop *)q->q_crp;
	
	SIMPLEQ_INSERT_TAIL(&sc->sc_freequeue, q, q_next);
	
	crp->crp_etype = EFAULT;
	crypto_done(crp);
	return(0);
}

/*
 * Routine to reset the chip and clean up.
 * It is assumed that the caller is in splnet()
 */
void
ubsec_totalreset(struct ubsec_softc *sc)
{
	ubsec_reset_board(sc);
	ubsec_init_board(sc);
	ubsec_cleanchip(sc);
}

int
ubsec_dmamap_aligned(bus_dmamap_t map)
{
	int i;

	for (i = 0; i < map->dm_nsegs; i++) {
		if (map->dm_segs[i].ds_addr & 3)
			return (0);
		if ((i != (map->dm_nsegs - 1)) &&
		    (map->dm_segs[i].ds_len & 3))
			return (0);
	}
	return (1);
}

struct ubsec_softc *
ubsec_kfind(struct cryptkop *krp)
{
	struct ubsec_softc *sc;
	int i;

	for (i = 0; i < ubsec_cd.cd_ndevs; i++) {
		sc = ubsec_cd.cd_devs[i];
		if (sc == NULL)
			continue;
		if (sc->sc_cid == krp->krp_hid)
			return (sc);
	}
	return (NULL);
}

void
ubsec_kfree(struct ubsec_softc *sc, struct ubsec_q2 *q)
{
	switch (q->q_type) {
	case UBS_CTXOP_MODEXP: {
		struct ubsec_q2_modexp *me = (struct ubsec_q2_modexp *)q;

		ubsec_dma_free(sc, &me->me_q.q_mcr);
		ubsec_dma_free(sc, &me->me_q.q_ctx);
		ubsec_dma_free(sc, &me->me_M);
		ubsec_dma_free(sc, &me->me_E);
		ubsec_dma_free(sc, &me->me_C);
		ubsec_dma_free(sc, &me->me_epb);
		free(me, M_DEVBUF);
		break;
	}
	case UBS_CTXOP_RSAPRIV: {
		struct ubsec_q2_rsapriv *rp = (struct ubsec_q2_rsapriv *)q;

		ubsec_dma_free(sc, &rp->rpr_q.q_mcr);
		ubsec_dma_free(sc, &rp->rpr_q.q_ctx);
		ubsec_dma_free(sc, &rp->rpr_msgin);
		ubsec_dma_free(sc, &rp->rpr_msgout);
		free(rp, M_DEVBUF);
		break;
	}
	default:
		printf("%s: invalid kfree 0x%x\n", sc->sc_dv.dv_xname,
		    q->q_type);
		break;
	}
}

int
ubsec_kprocess(struct cryptkop *krp)
{
	struct ubsec_softc *sc;
	int r;

	if (krp == NULL || krp->krp_callback == NULL)
		return (EINVAL);
	if ((sc = ubsec_kfind(krp)) == NULL)
		return (EINVAL);

	while (!SIMPLEQ_EMPTY(&sc->sc_q2free)) {
		struct ubsec_q2 *q;

		q = SIMPLEQ_FIRST(&sc->sc_q2free);
		SIMPLEQ_REMOVE_HEAD(&sc->sc_q2free, q_next);
		ubsec_kfree(sc, q);
	}

	switch (krp->krp_op) {
	case CRK_MOD_EXP:
		if (sc->sc_flags & UBS_FLAGS_HWNORM)
			r = ubsec_kprocess_modexp_hw(sc, krp);
		else
			r = ubsec_kprocess_modexp_sw(sc, krp);
		break;
	case CRK_MOD_EXP_CRT:
		r = ubsec_kprocess_rsapriv(sc, krp);
		break;
	default:
		printf("%s: kprocess: invalid op 0x%x\n",
		    sc->sc_dv.dv_xname, krp->krp_op);
		krp->krp_status = EOPNOTSUPP;
		crypto_kdone(krp);
		r = 0;
	}
	return (r);
}

/*
 * Start computation of cr[C] = (cr[M] ^ cr[E]) mod cr[N] (sw normalization)
 */
int
ubsec_kprocess_modexp_sw(struct ubsec_softc *sc, struct cryptkop *krp)
{
	struct ubsec_q2_modexp *me;
	struct ubsec_mcr *mcr;
	struct ubsec_ctx_modexp *ctx;
	struct ubsec_pktbuf *epb;
	int err = 0, s;
	u_int nbits, normbits, mbits, shiftbits, ebits;

	me = malloc(sizeof *me, M_DEVBUF, M_NOWAIT | M_ZERO);
	if (me == NULL) {
		err = ENOMEM;
		goto errout;
	}
	me->me_krp = krp;
	me->me_q.q_type = UBS_CTXOP_MODEXP;

	nbits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_N]);
	if (nbits <= 512)
		normbits = 512;
	else if (nbits <= 768)
		normbits = 768;
	else if (nbits <= 1024)
		normbits = 1024;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && nbits <= 1536)
		normbits = 1536;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && nbits <= 2048)
		normbits = 2048;
	else {
		err = E2BIG;
		goto errout;
	}

	shiftbits = normbits - nbits;

	me->me_modbits = nbits;
	me->me_shiftbits = shiftbits;
	me->me_normbits = normbits;

	/* Sanity check: result bits must be >= true modulus bits. */
	if (krp->krp_param[krp->krp_iparams].crp_nbits < nbits) {
		err = ERANGE;
		goto errout;
	}

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_mcr),
	    &me->me_q.q_mcr, 0)) {
		err = ENOMEM;
		goto errout;
	}
	mcr = (struct ubsec_mcr *)me->me_q.q_mcr.dma_vaddr;

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_ctx_modexp),
	    &me->me_q.q_ctx, 0)) {
		err = ENOMEM;
		goto errout;
	}

	mbits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_M]);
	if (mbits > nbits) {
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_M, 0)) {
		err = ENOMEM;
		goto errout;
	}
	ubsec_kshift_r(shiftbits,
	    krp->krp_param[UBS_MODEXP_PAR_M].crp_p, mbits,
	    me->me_M.dma_vaddr, normbits);

	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_C, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(me->me_C.dma_vaddr, me->me_C.dma_size);

	ebits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_E]);
	if (ebits > nbits) {
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_E, 0)) {
		err = ENOMEM;
		goto errout;
	}
	ubsec_kshift_r(shiftbits,
	    krp->krp_param[UBS_MODEXP_PAR_E].crp_p, ebits,
	    me->me_E.dma_vaddr, normbits);

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_pktbuf),
	    &me->me_epb, 0)) {
		err = ENOMEM;
		goto errout;
	}
	epb = (struct ubsec_pktbuf *)me->me_epb.dma_vaddr;
	epb->pb_addr = htole32(me->me_E.dma_paddr);
	epb->pb_next = 0;
	epb->pb_len = htole32(normbits / 8);

#ifdef UBSEC_DEBUG
	printf("Epb ");
	ubsec_dump_pb(epb);
#endif

	mcr->mcr_pkts = htole16(1);
	mcr->mcr_flags = 0;
	mcr->mcr_cmdctxp = htole32(me->me_q.q_ctx.dma_paddr);
	mcr->mcr_reserved = 0;
	mcr->mcr_pktlen = 0;

	mcr->mcr_ipktbuf.pb_addr = htole32(me->me_M.dma_paddr);
	mcr->mcr_ipktbuf.pb_len = htole32(normbits / 8);
	mcr->mcr_ipktbuf.pb_next = htole32(me->me_epb.dma_paddr);

	mcr->mcr_opktbuf.pb_addr = htole32(me->me_C.dma_paddr);
	mcr->mcr_opktbuf.pb_next = 0;
	mcr->mcr_opktbuf.pb_len = htole32(normbits / 8);

#ifdef DIAGNOSTIC
	/* Misaligned output buffer will hang the chip. */
	if ((letoh32(mcr->mcr_opktbuf.pb_addr) & 3) != 0)
		panic("%s: modexp invalid addr 0x%x",
		    sc->sc_dv.dv_xname, letoh32(mcr->mcr_opktbuf.pb_addr));
	if ((letoh32(mcr->mcr_opktbuf.pb_len) & 3) != 0)
		panic("%s: modexp invalid len 0x%x",
		    sc->sc_dv.dv_xname, letoh32(mcr->mcr_opktbuf.pb_len));
#endif

	ctx = (struct ubsec_ctx_modexp *)me->me_q.q_ctx.dma_vaddr;
	bzero(ctx, sizeof(*ctx));
	ubsec_kshift_r(shiftbits,
	    krp->krp_param[UBS_MODEXP_PAR_N].crp_p, nbits,
	    ctx->me_N, normbits);
	ctx->me_len = htole16((normbits / 8) + (4 * sizeof(u_int16_t)));
	ctx->me_op = htole16(UBS_CTXOP_MODEXP);
	ctx->me_E_len = htole16(nbits);
	ctx->me_N_len = htole16(nbits);

#ifdef UBSEC_DEBUG
	ubsec_dump_mcr(mcr);
	ubsec_dump_ctx2((struct ubsec_ctx_keyop *)ctx);
#endif

	/*
	 * ubsec_feed2 will sync mcr and ctx, we just need to sync
	 * everything else.
	 */
	bus_dmamap_sync(sc->sc_dmat, me->me_M.dma_map,
	    0, me->me_M.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	bus_dmamap_sync(sc->sc_dmat, me->me_E.dma_map,
	    0, me->me_E.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	bus_dmamap_sync(sc->sc_dmat, me->me_C.dma_map,
	    0, me->me_C.dma_map->dm_mapsize, BUS_DMASYNC_PREREAD);
	bus_dmamap_sync(sc->sc_dmat, me->me_epb.dma_map,
	    0, me->me_epb.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);

	/* Enqueue and we're done... */
	s = splnet();
	SIMPLEQ_INSERT_TAIL(&sc->sc_queue2, &me->me_q, q_next);
	ubsec_feed2(sc);
	splx(s);

	return (0);

errout:
	if (me != NULL) {
		if (me->me_q.q_mcr.dma_map != NULL)
			ubsec_dma_free(sc, &me->me_q.q_mcr);
		if (me->me_q.q_ctx.dma_map != NULL) {
			bzero(me->me_q.q_ctx.dma_vaddr, me->me_q.q_ctx.dma_size);
			ubsec_dma_free(sc, &me->me_q.q_ctx);
		}
		if (me->me_M.dma_map != NULL) {
			bzero(me->me_M.dma_vaddr, me->me_M.dma_size);
			ubsec_dma_free(sc, &me->me_M);
		}
		if (me->me_E.dma_map != NULL) {
			bzero(me->me_E.dma_vaddr, me->me_E.dma_size);
			ubsec_dma_free(sc, &me->me_E);
		}
		if (me->me_C.dma_map != NULL) {
			bzero(me->me_C.dma_vaddr, me->me_C.dma_size);
			ubsec_dma_free(sc, &me->me_C);
		}
		if (me->me_epb.dma_map != NULL)
			ubsec_dma_free(sc, &me->me_epb);
		free(me, M_DEVBUF);
	}
	krp->krp_status = err;
	crypto_kdone(krp);
	return (0);
}

/*
 * Start computation of cr[C] = (cr[M] ^ cr[E]) mod cr[N] (hw normalization)
 */
int
ubsec_kprocess_modexp_hw(struct ubsec_softc *sc, struct cryptkop *krp)
{
	struct ubsec_q2_modexp *me;
	struct ubsec_mcr *mcr;
	struct ubsec_ctx_modexp *ctx;
	struct ubsec_pktbuf *epb;
	int err = 0, s;
	u_int nbits, normbits, mbits, shiftbits, ebits;

	me = malloc(sizeof *me, M_DEVBUF, M_NOWAIT | M_ZERO);
	if (me == NULL) {
		err = ENOMEM;
		goto errout;
	}
	me->me_krp = krp;
	me->me_q.q_type = UBS_CTXOP_MODEXP;

	nbits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_N]);
	if (nbits <= 512)
		normbits = 512;
	else if (nbits <= 768)
		normbits = 768;
	else if (nbits <= 1024)
		normbits = 1024;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && nbits <= 1536)
		normbits = 1536;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && nbits <= 2048)
		normbits = 2048;
	else {
		err = E2BIG;
		goto errout;
	}

	shiftbits = normbits - nbits;

	/* XXX ??? */
	me->me_modbits = nbits;
	me->me_shiftbits = shiftbits;
	me->me_normbits = normbits;

	/* Sanity check: result bits must be >= true modulus bits. */
	if (krp->krp_param[krp->krp_iparams].crp_nbits < nbits) {
		err = ERANGE;
		goto errout;
	}

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_mcr),
	    &me->me_q.q_mcr, 0)) {
		err = ENOMEM;
		goto errout;
	}
	mcr = (struct ubsec_mcr *)me->me_q.q_mcr.dma_vaddr;

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_ctx_modexp),
	    &me->me_q.q_ctx, 0)) {
		err = ENOMEM;
		goto errout;
	}

	mbits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_M]);
	if (mbits > nbits) {
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_M, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(me->me_M.dma_vaddr, normbits / 8);
	bcopy(krp->krp_param[UBS_MODEXP_PAR_M].crp_p,
	    me->me_M.dma_vaddr, (mbits + 7) / 8);

	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_C, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(me->me_C.dma_vaddr, me->me_C.dma_size);

	ebits = ubsec_ksigbits(&krp->krp_param[UBS_MODEXP_PAR_E]);
	if (ebits > nbits) {
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, normbits / 8, &me->me_E, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(me->me_E.dma_vaddr, normbits / 8);
	bcopy(krp->krp_param[UBS_MODEXP_PAR_E].crp_p,
	    me->me_E.dma_vaddr, (ebits + 7) / 8);

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_pktbuf),
	    &me->me_epb, 0)) {
		err = ENOMEM;
		goto errout;
	}
	epb = (struct ubsec_pktbuf *)me->me_epb.dma_vaddr;
	epb->pb_addr = htole32(me->me_E.dma_paddr);
	epb->pb_next = 0;
	epb->pb_len = htole32((ebits + 7) / 8);

#ifdef UBSEC_DEBUG
	printf("Epb ");
	ubsec_dump_pb(epb);
#endif

	mcr->mcr_pkts = htole16(1);
	mcr->mcr_flags = 0;
	mcr->mcr_cmdctxp = htole32(me->me_q.q_ctx.dma_paddr);
	mcr->mcr_reserved = 0;
	mcr->mcr_pktlen = 0;

	mcr->mcr_ipktbuf.pb_addr = htole32(me->me_M.dma_paddr);
	mcr->mcr_ipktbuf.pb_len = htole32(normbits / 8);
	mcr->mcr_ipktbuf.pb_next = htole32(me->me_epb.dma_paddr);

	mcr->mcr_opktbuf.pb_addr = htole32(me->me_C.dma_paddr);
	mcr->mcr_opktbuf.pb_next = 0;
	mcr->mcr_opktbuf.pb_len = htole32(normbits / 8);

#ifdef DIAGNOSTIC
	/* Misaligned output buffer will hang the chip. */
	if ((letoh32(mcr->mcr_opktbuf.pb_addr) & 3) != 0)
		panic("%s: modexp invalid addr 0x%x",
		    sc->sc_dv.dv_xname, letoh32(mcr->mcr_opktbuf.pb_addr));
	if ((letoh32(mcr->mcr_opktbuf.pb_len) & 3) != 0)
		panic("%s: modexp invalid len 0x%x",
		    sc->sc_dv.dv_xname, letoh32(mcr->mcr_opktbuf.pb_len));
#endif

	ctx = (struct ubsec_ctx_modexp *)me->me_q.q_ctx.dma_vaddr;
	bzero(ctx, sizeof(*ctx));
	bcopy(krp->krp_param[UBS_MODEXP_PAR_N].crp_p, ctx->me_N,
	    (nbits + 7) / 8);
	ctx->me_len = htole16((normbits / 8) + (4 * sizeof(u_int16_t)));
	ctx->me_op = htole16(UBS_CTXOP_MODEXP);
	ctx->me_E_len = htole16(ebits);
	ctx->me_N_len = htole16(nbits);

#ifdef UBSEC_DEBUG
	ubsec_dump_mcr(mcr);
	ubsec_dump_ctx2((struct ubsec_ctx_keyop *)ctx);
#endif

	/*
	 * ubsec_feed2 will sync mcr and ctx, we just need to sync
	 * everything else.
	 */
	bus_dmamap_sync(sc->sc_dmat, me->me_M.dma_map,
	    0, me->me_M.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	bus_dmamap_sync(sc->sc_dmat, me->me_E.dma_map,
	    0, me->me_E.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	bus_dmamap_sync(sc->sc_dmat, me->me_C.dma_map,
	    0, me->me_C.dma_map->dm_mapsize, BUS_DMASYNC_PREREAD);
	bus_dmamap_sync(sc->sc_dmat, me->me_epb.dma_map,
	    0, me->me_epb.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);

	/* Enqueue and we're done... */
	s = splnet();
	SIMPLEQ_INSERT_TAIL(&sc->sc_queue2, &me->me_q, q_next);
	ubsec_feed2(sc);
	splx(s);

	return (0);

errout:
	if (me != NULL) {
		if (me->me_q.q_mcr.dma_map != NULL)
			ubsec_dma_free(sc, &me->me_q.q_mcr);
		if (me->me_q.q_ctx.dma_map != NULL) {
			bzero(me->me_q.q_ctx.dma_vaddr, me->me_q.q_ctx.dma_size);
			ubsec_dma_free(sc, &me->me_q.q_ctx);
		}
		if (me->me_M.dma_map != NULL) {
			bzero(me->me_M.dma_vaddr, me->me_M.dma_size);
			ubsec_dma_free(sc, &me->me_M);
		}
		if (me->me_E.dma_map != NULL) {
			bzero(me->me_E.dma_vaddr, me->me_E.dma_size);
			ubsec_dma_free(sc, &me->me_E);
		}
		if (me->me_C.dma_map != NULL) {
			bzero(me->me_C.dma_vaddr, me->me_C.dma_size);
			ubsec_dma_free(sc, &me->me_C);
		}
		if (me->me_epb.dma_map != NULL)
			ubsec_dma_free(sc, &me->me_epb);
		free(me, M_DEVBUF);
	}
	krp->krp_status = err;
	crypto_kdone(krp);
	return (0);
}

int
ubsec_kprocess_rsapriv(struct ubsec_softc *sc, struct cryptkop *krp)
{
	struct ubsec_q2_rsapriv *rp = NULL;
	struct ubsec_mcr *mcr;
	struct ubsec_ctx_rsapriv *ctx;
	int err = 0, s;
	u_int padlen, msglen;

	msglen = ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_P]);
	padlen = ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_Q]);
	if (msglen > padlen)
		padlen = msglen;

	if (padlen <= 256)
		padlen = 256;
	else if (padlen <= 384)
		padlen = 384;
	else if (padlen <= 512)
		padlen = 512;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && padlen <= 768)
		padlen = 768;
	else if (sc->sc_flags & UBS_FLAGS_BIGKEY && padlen <= 1024)
		padlen = 1024;
	else {
		err = E2BIG;
		goto errout;
	}

	if (ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_DP]) > padlen) {
		err = E2BIG;
		goto errout;
	}

	if (ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_DQ]) > padlen) {
		err = E2BIG;
		goto errout;
	}

	if (ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_PINV]) > padlen) {
		err = E2BIG;
		goto errout;
	}

	rp = malloc(sizeof *rp, M_DEVBUF, M_NOWAIT | M_ZERO);
	if (rp == NULL)
		return (ENOMEM);
	rp->rpr_krp = krp;
	rp->rpr_q.q_type = UBS_CTXOP_RSAPRIV;

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_mcr),
	    &rp->rpr_q.q_mcr, 0)) {
		err = ENOMEM;
		goto errout;
	}
	mcr = (struct ubsec_mcr *)rp->rpr_q.q_mcr.dma_vaddr;

	if (ubsec_dma_malloc(sc, sizeof(struct ubsec_ctx_rsapriv),
	    &rp->rpr_q.q_ctx, 0)) {
		err = ENOMEM;
		goto errout;
	}
	ctx = (struct ubsec_ctx_rsapriv *)rp->rpr_q.q_ctx.dma_vaddr;
	bzero(ctx, sizeof *ctx);

	/* Copy in p */
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_P].crp_p,
	    &ctx->rpr_buf[0 * (padlen / 8)],
	    (krp->krp_param[UBS_RSAPRIV_PAR_P].crp_nbits + 7) / 8);

	/* Copy in q */
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_Q].crp_p,
	    &ctx->rpr_buf[1 * (padlen / 8)],
	    (krp->krp_param[UBS_RSAPRIV_PAR_Q].crp_nbits + 7) / 8);

	/* Copy in dp */
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_DP].crp_p,
	    &ctx->rpr_buf[2 * (padlen / 8)],
	    (krp->krp_param[UBS_RSAPRIV_PAR_DP].crp_nbits + 7) / 8);

	/* Copy in dq */
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_DQ].crp_p,
	    &ctx->rpr_buf[3 * (padlen / 8)],
	    (krp->krp_param[UBS_RSAPRIV_PAR_DQ].crp_nbits + 7) / 8);

	/* Copy in pinv */
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_PINV].crp_p,
	    &ctx->rpr_buf[4 * (padlen / 8)],
	    (krp->krp_param[UBS_RSAPRIV_PAR_PINV].crp_nbits + 7) / 8);

	msglen = padlen * 2;

	/* Copy in input message (aligned buffer/length). */
	if (ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_MSGIN]) > msglen) {
		/* Is this likely? */
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, (msglen + 7) / 8, &rp->rpr_msgin, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(rp->rpr_msgin.dma_vaddr, (msglen + 7) / 8);
	bcopy(krp->krp_param[UBS_RSAPRIV_PAR_MSGIN].crp_p,
	    rp->rpr_msgin.dma_vaddr,
	    (krp->krp_param[UBS_RSAPRIV_PAR_MSGIN].crp_nbits + 7) / 8);

	/* Prepare space for output message (aligned buffer/length). */
	if (ubsec_ksigbits(&krp->krp_param[UBS_RSAPRIV_PAR_MSGOUT]) < msglen) {
		/* Is this likely? */
		err = E2BIG;
		goto errout;
	}
	if (ubsec_dma_malloc(sc, (msglen + 7) / 8, &rp->rpr_msgout, 0)) {
		err = ENOMEM;
		goto errout;
	}
	bzero(rp->rpr_msgout.dma_vaddr, (msglen + 7) / 8);

	mcr->mcr_pkts = htole16(1);
	mcr->mcr_flags = 0;
	mcr->mcr_cmdctxp = htole32(rp->rpr_q.q_ctx.dma_paddr);
	mcr->mcr_ipktbuf.pb_addr = htole32(rp->rpr_msgin.dma_paddr);
	mcr->mcr_ipktbuf.pb_next = 0;
	mcr->mcr_ipktbuf.pb_len = htole32(rp->rpr_msgin.dma_size);
	mcr->mcr_reserved = 0;
	mcr->mcr_pktlen = htole16(msglen);
	mcr->mcr_opktbuf.pb_addr = htole32(rp->rpr_msgout.dma_paddr);
	mcr->mcr_opktbuf.pb_next = 0;
	mcr->mcr_opktbuf.pb_len = htole32(rp->rpr_msgout.dma_size);

#ifdef DIAGNOSTIC
	if (rp->rpr_msgin.dma_paddr & 3 || rp->rpr_msgin.dma_size & 3) {
		panic("%s: rsapriv: invalid msgin %p(0x%x)",
		    sc->sc_dv.dv_xname, rp->rpr_msgin.dma_paddr,
		    rp->rpr_msgin.dma_size);
	}
	if (rp->rpr_msgout.dma_paddr & 3 || rp->rpr_msgout.dma_size & 3) {
		panic("%s: rsapriv: invalid msgout %p(0x%x)",
		    sc->sc_dv.dv_xname, rp->rpr_msgout.dma_paddr,
		    rp->rpr_msgout.dma_size);
	}
#endif

	ctx->rpr_len = (sizeof(u_int16_t) * 4) + (5 * (padlen / 8));
	ctx->rpr_op = htole16(UBS_CTXOP_RSAPRIV);
	ctx->rpr_q_len = htole16(padlen);
	ctx->rpr_p_len = htole16(padlen);

	/*
	 * ubsec_feed2 will sync mcr and ctx, we just need to sync
	 * everything else.
	 */
	bus_dmamap_sync(sc->sc_dmat, rp->rpr_msgin.dma_map,
	    0, rp->rpr_msgin.dma_map->dm_mapsize, BUS_DMASYNC_PREWRITE);
	bus_dmamap_sync(sc->sc_dmat, rp->rpr_msgout.dma_map,
	    0, rp->rpr_msgout.dma_map->dm_mapsize, BUS_DMASYNC_PREREAD);

	/* Enqueue and we're done... */
	s = splnet();
	SIMPLEQ_INSERT_TAIL(&sc->sc_queue2, &rp->rpr_q, q_next);
	ubsec_feed2(sc);
	splx(s);
	return (0);

errout:
	if (rp != NULL) {
		if (rp->rpr_q.q_mcr.dma_map != NULL)
			ubsec_dma_free(sc, &rp->rpr_q.q_mcr);
		if (rp->rpr_msgin.dma_map != NULL) {
			bzero(rp->rpr_msgin.dma_vaddr, rp->rpr_msgin.dma_size);
			ubsec_dma_free(sc, &rp->rpr_msgin);
		}
		if (rp->rpr_msgout.dma_map != NULL) {
			bzero(rp->rpr_msgout.dma_vaddr, rp->rpr_msgout.dma_size);
			ubsec_dma_free(sc, &rp->rpr_msgout);
		}
		free(rp, M_DEVBUF);
	}
	krp->krp_status = err;
	crypto_kdone(krp);
	return (0);
}

void
ubsec_dump_pb(struct ubsec_pktbuf *pb)
{
	printf("addr 0x%x (0x%x) next 0x%x\n",
	    pb->pb_addr, pb->pb_len, pb->pb_next);
}

void
ubsec_dump_ctx2(struct ubsec_ctx_keyop *c)
{
	printf("CTX (0x%x):\n", c->ctx_len);
	switch (letoh16(c->ctx_op)) {
	case UBS_CTXOP_RNGBYPASS:
	case UBS_CTXOP_RNGSHA1:
		break;
	case UBS_CTXOP_MODEXP:
	{
		struct ubsec_ctx_modexp *cx = (void *)c;
		int i, len;

		printf(" Elen %u, Nlen %u\n",
		    letoh16(cx->me_E_len), letoh16(cx->me_N_len));
		len = (cx->me_N_len + 7)/8;
		for (i = 0; i < len; i++)
			printf("%s%02x", (i == 0) ? " N: " : ":", cx->me_N[i]);
		printf("\n");
		break;
	}
	default:
		printf("unknown context: %x\n", c->ctx_op);
	}
	printf("END CTX\n");
}

void
ubsec_dump_mcr(struct ubsec_mcr *mcr)
{
	struct ubsec_mcr_add *ma;
	int i;

	printf("MCR:\n");
	printf(" pkts: %u, flags 0x%x\n",
	    letoh16(mcr->mcr_pkts), letoh16(mcr->mcr_flags));
	ma = (struct ubsec_mcr_add *)&mcr->mcr_cmdctxp;
	for (i = 0; i < letoh16(mcr->mcr_pkts); i++) {
		printf(" %d: ctx 0x%x len 0x%x rsvd 0x%x\n", i,
		    letoh32(ma->mcr_cmdctxp), letoh16(ma->mcr_pktlen),
		    letoh16(ma->mcr_reserved));
		printf(" %d: ipkt ", i);
		ubsec_dump_pb(&ma->mcr_ipktbuf);
		printf(" %d: opkt ", i);
		ubsec_dump_pb(&ma->mcr_opktbuf);
		ma++;
	}
	printf("END MCR\n");
}

/*
 * Return the number of significant bits of a big number.
 */
int
ubsec_ksigbits(struct crparam *cr)
{
	u_int plen = (cr->crp_nbits + 7) / 8;
	int i, sig = plen * 8;
	u_int8_t c, *p = cr->crp_p;

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

void
ubsec_kshift_r(u_int shiftbits, u_int8_t *src, u_int srcbits,
    u_int8_t *dst, u_int dstbits)
{
	u_int slen, dlen;
	int i, si, di, n;

	slen = (srcbits + 7) / 8;
	dlen = (dstbits + 7) / 8;

	for (i = 0; i < slen; i++)
		dst[i] = src[i];
	for (i = 0; i < dlen - slen; i++)
		dst[slen + i] = 0;

	n = shiftbits / 8;
	if (n != 0) {
		si = dlen - n - 1;
		di = dlen - 1;
		while (si >= 0)
			dst[di--] = dst[si--];
		while (di >= 0)
			dst[di--] = 0;
	}

	n = shiftbits % 8;
	if (n != 0) {
		for (i = dlen - 1; i > 0; i--)
			dst[i] = (dst[i] << n) |
			    (dst[i - 1] >> (8 - n));
		dst[0] = dst[0] << n;
	}
}

void
ubsec_kshift_l(u_int shiftbits, u_int8_t *src, u_int srcbits,
    u_int8_t *dst, u_int dstbits)
{
	int slen, dlen, i, n;

	slen = (srcbits + 7) / 8;
	dlen = (dstbits + 7) / 8;

	n = shiftbits / 8;
	for (i = 0; i < slen; i++)
		dst[i] = src[i + n];
	for (i = 0; i < dlen - slen; i++)
		dst[slen + i] = 0;

	n = shiftbits % 8;
	if (n != 0) {
		for (i = 0; i < (dlen - 1); i++)
			dst[i] = (dst[i] >> n) | (dst[i + 1] << (8 - n));
		dst[dlen - 1] = dst[dlen - 1] >> n;
	}
}
