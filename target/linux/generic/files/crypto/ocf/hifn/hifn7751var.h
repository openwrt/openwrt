/* $FreeBSD: src/sys/dev/hifn/hifn7751var.h,v 1.9 2007/03/21 03:42:49 sam Exp $ */
/*	$OpenBSD: hifn7751var.h,v 1.42 2002/04/08 17:49:42 jason Exp $	*/

/*-
 * Invertex AEON / Hifn 7751 driver
 * Copyright (c) 1999 Invertex Inc. All rights reserved.
 * Copyright (c) 1999 Theo de Raadt
 * Copyright (c) 2000-2001 Network Security Technologies, Inc.
 *			http://www.netsec.net
 *
 * Please send any comments, feedback, bug-fixes, or feature requests to
 * software@invertex.com.
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
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */

#ifndef __HIFN7751VAR_H__
#define __HIFN7751VAR_H__

#ifdef __KERNEL__

/*
 * Some configurable values for the driver.  By default command+result
 * descriptor rings are the same size.  The src+dst descriptor rings
 * are sized at 3.5x the number of potential commands.  Slower parts
 * (e.g. 7951) tend to run out of src descriptors; faster parts (7811)
 * src+cmd/result descriptors.  It's not clear that increasing the size
 * of the descriptor rings helps performance significantly as other
 * factors tend to come into play (e.g. copying misaligned packets).
 */
#define	HIFN_D_CMD_RSIZE	24	/* command descriptors */
#define	HIFN_D_SRC_RSIZE	((HIFN_D_CMD_RSIZE * 7) / 2)	/* source descriptors */
#define	HIFN_D_RES_RSIZE	HIFN_D_CMD_RSIZE	/* result descriptors */
#define	HIFN_D_DST_RSIZE	HIFN_D_SRC_RSIZE	/* destination descriptors */

/*
 *  Length values for cryptography
 */
#define HIFN_DES_KEY_LENGTH		8
#define HIFN_3DES_KEY_LENGTH		24
#define HIFN_MAX_CRYPT_KEY_LENGTH	HIFN_3DES_KEY_LENGTH
#define HIFN_IV_LENGTH			8
#define	HIFN_AES_IV_LENGTH		16
#define HIFN_MAX_IV_LENGTH		HIFN_AES_IV_LENGTH

/*
 *  Length values for authentication
 */
#define HIFN_MAC_KEY_LENGTH		64
#define HIFN_MD5_LENGTH			16
#define HIFN_SHA1_LENGTH		20
#define HIFN_MAC_TRUNC_LENGTH		12

#define MAX_SCATTER 64

/*
 * Data structure to hold all 4 rings and any other ring related data.
 */
struct hifn_dma {
	/*
	 *  Descriptor rings.  We add +1 to the size to accomidate the
	 *  jump descriptor.
	 */
	struct hifn_desc	cmdr[HIFN_D_CMD_RSIZE+1];
	struct hifn_desc	srcr[HIFN_D_SRC_RSIZE+1];
	struct hifn_desc	dstr[HIFN_D_DST_RSIZE+1];
	struct hifn_desc	resr[HIFN_D_RES_RSIZE+1];

	struct hifn_command	*hifn_commands[HIFN_D_RES_RSIZE];

	u_char			command_bufs[HIFN_D_CMD_RSIZE][HIFN_MAX_COMMAND];
	u_char			result_bufs[HIFN_D_CMD_RSIZE][HIFN_MAX_RESULT];
	u_int32_t		slop[HIFN_D_CMD_RSIZE];

	u_int64_t		test_src, test_dst;

	/*
	 *  Our current positions for insertion and removal from the desriptor
	 *  rings. 
	 */
	int			cmdi, srci, dsti, resi;
	volatile int		cmdu, srcu, dstu, resu;
	int			cmdk, srck, dstk, resk;
};

struct hifn_session {
	int hs_used;
	int hs_mlen;
};

#define	HIFN_RING_SYNC(sc, r, i, f)					\
	/* DAVIDM bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_dmamap, (f)) */

#define	HIFN_CMDR_SYNC(sc, i, f)	HIFN_RING_SYNC((sc), cmdr, (i), (f))
#define	HIFN_RESR_SYNC(sc, i, f)	HIFN_RING_SYNC((sc), resr, (i), (f))
#define	HIFN_SRCR_SYNC(sc, i, f)	HIFN_RING_SYNC((sc), srcr, (i), (f))
#define	HIFN_DSTR_SYNC(sc, i, f)	HIFN_RING_SYNC((sc), dstr, (i), (f))

#define	HIFN_CMD_SYNC(sc, i, f)						\
	/* DAVIDM bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_dmamap, (f)) */

#define	HIFN_RES_SYNC(sc, i, f)						\
	/* DAVIDM bus_dmamap_sync((sc)->sc_dmat, (sc)->sc_dmamap, (f)) */

typedef int bus_size_t;

/*
 * Holds data specific to a single HIFN board.
 */
struct hifn_softc {
	softc_device_decl		 sc_dev;

	struct pci_dev		*sc_pcidev;	/* PCI device pointer */
	spinlock_t		sc_mtx;		/* per-instance lock */

	int			sc_num;		/* for multiple devs */

	ocf_iomem_t		sc_bar0;
	bus_size_t		sc_bar0_lastreg;/* bar0 last reg written */
	ocf_iomem_t		sc_bar1;
	bus_size_t		sc_bar1_lastreg;/* bar1 last reg written */

	int			sc_irq;

	u_int32_t		sc_dmaier;
	u_int32_t		sc_drammodel;	/* 1=dram, 0=sram */
	u_int32_t		sc_pllconfig;	/* 7954/7955/7956 PLL config */

	struct hifn_dma		*sc_dma;
	dma_addr_t		sc_dma_physaddr;/* physical address of sc_dma */

	int			sc_dmansegs;
	int32_t			sc_cid;
	int			sc_maxses;
	int			sc_nsessions;
	struct hifn_session	*sc_sessions;
	int			sc_ramsize;
	int			sc_flags;
#define	HIFN_HAS_RNG		0x1	/* includes random number generator */
#define	HIFN_HAS_PUBLIC		0x2	/* includes public key support */
#define	HIFN_HAS_AES		0x4	/* includes AES support */
#define	HIFN_IS_7811		0x8	/* Hifn 7811 part */
#define	HIFN_IS_7956		0x10	/* Hifn 7956/7955 don't have SDRAM */

	struct timer_list	sc_tickto;	/* for managing DMA */

	int			sc_rngfirst;
	int			sc_rnghz;	/* RNG polling frequency */

	int			sc_c_busy;	/* command ring busy */
	int			sc_s_busy;	/* source data ring busy */
	int			sc_d_busy;	/* destination data ring busy */
	int			sc_r_busy;	/* result ring busy */
	int			sc_active;	/* for initial countdown */
	int			sc_needwakeup;	/* ops q'd wating on resources */
	int			sc_curbatch;	/* # ops submitted w/o int */
	int			sc_suspended;
#ifdef HIFN_VULCANDEV
	struct cdev            *sc_pkdev;
#endif
};

#define	HIFN_LOCK(_sc)		spin_lock_irqsave(&(_sc)->sc_mtx, l_flags)
#define	HIFN_UNLOCK(_sc)	spin_unlock_irqrestore(&(_sc)->sc_mtx, l_flags)

/*
 *  hifn_command_t
 *
 *  This is the control structure used to pass commands to hifn_encrypt().
 *
 *  flags
 *  -----
 *  Flags is the bitwise "or" values for command configuration.  A single
 *  encrypt direction needs to be set:
 *
 *	HIFN_ENCODE or HIFN_DECODE
 *
 *  To use cryptography, a single crypto algorithm must be included:
 *
 *	HIFN_CRYPT_3DES or HIFN_CRYPT_DES
 *
 *  To use authentication is used, a single MAC algorithm must be included:
 *
 *	HIFN_MAC_MD5 or HIFN_MAC_SHA1
 *
 *  By default MD5 uses a 16 byte hash and SHA-1 uses a 20 byte hash.
 *  If the value below is set, hash values are truncated or assumed
 *  truncated to 12 bytes:
 *
 *	HIFN_MAC_TRUNC
 *
 *  Keys for encryption and authentication can be sent as part of a command,
 *  or the last key value used with a particular session can be retrieved
 *  and used again if either of these flags are not specified.
 *
 *	HIFN_CRYPT_NEW_KEY, HIFN_MAC_NEW_KEY
 *
 *  session_num
 *  -----------
 *  A number between 0 and 2048 (for DRAM models) or a number between 
 *  0 and 768 (for SRAM models).  Those who don't want to use session
 *  numbers should leave value at zero and send a new crypt key and/or
 *  new MAC key on every command.  If you use session numbers and
 *  don't send a key with a command, the last key sent for that same
 *  session number will be used.
 *
 *  Warning:  Using session numbers and multiboard at the same time
 *            is currently broken.
 *
 *  mbuf
 *  ----
 *  Either fill in the mbuf pointer and npa=0 or
 *	 fill packp[] and packl[] and set npa to > 0
 * 
 *  mac_header_skip
 *  ---------------
 *  The number of bytes of the source_buf that are skipped over before
 *  authentication begins.  This must be a number between 0 and 2^16-1
 *  and can be used by IPsec implementers to skip over IP headers.
 *  *** Value ignored if authentication not used ***
 *
 *  crypt_header_skip
 *  -----------------
 *  The number of bytes of the source_buf that are skipped over before
 *  the cryptographic operation begins.  This must be a number between 0
 *  and 2^16-1.  For IPsec, this number will always be 8 bytes larger
 *  than the auth_header_skip (to skip over the ESP header).
 *  *** Value ignored if cryptography not used ***
 *
 */
struct hifn_operand {
	union {
		struct sk_buff *skb;
		struct uio *io;
		unsigned char *buf;
	} u;
	void		*map;
	bus_size_t	mapsize;
	int		nsegs;
	struct {
	    dma_addr_t  ds_addr;
	    int         ds_len;
	} segs[MAX_SCATTER];
};

struct hifn_command {
	u_int16_t session_num;
	u_int16_t base_masks, cry_masks, mac_masks;
	u_int8_t iv[HIFN_MAX_IV_LENGTH], *ck, mac[HIFN_MAC_KEY_LENGTH];
	int cklen;
	int sloplen, slopidx;

	struct hifn_operand src;
	struct hifn_operand dst;

	struct hifn_softc *softc;
	struct cryptop *crp;
	struct cryptodesc *enccrd, *maccrd;
};

#define	src_skb		src.u.skb
#define	src_io		src.u.io
#define	src_map		src.map
#define	src_mapsize	src.mapsize
#define	src_segs	src.segs
#define	src_nsegs	src.nsegs
#define	src_buf		src.u.buf

#define	dst_skb		dst.u.skb
#define	dst_io		dst.u.io
#define	dst_map		dst.map
#define	dst_mapsize	dst.mapsize
#define	dst_segs	dst.segs
#define	dst_nsegs	dst.nsegs
#define	dst_buf		dst.u.buf

/*
 *  Return values for hifn_crypto()
 */
#define HIFN_CRYPTO_SUCCESS	0
#define HIFN_CRYPTO_BAD_INPUT	(-1)
#define HIFN_CRYPTO_RINGS_FULL	(-2)

/**************************************************************************
 *
 *  Function:  hifn_crypto
 *
 *  Purpose:   Called by external drivers to begin an encryption on the
 *             HIFN board.
 *
 *  Blocking/Non-blocking Issues
 *  ============================
 *  The driver cannot block in hifn_crypto (no calls to tsleep) currently.
 *  hifn_crypto() returns HIFN_CRYPTO_RINGS_FULL if there is not enough
 *  room in any of the rings for the request to proceed.
 *
 *  Return Values
 *  =============
 *  0 for success, negative values on error
 *
 *  Defines for negative error codes are:
 *  
 *    HIFN_CRYPTO_BAD_INPUT  :  The passed in command had invalid settings.
 *    HIFN_CRYPTO_RINGS_FULL :  All DMA rings were full and non-blocking
 *                              behaviour was requested.
 *
 *************************************************************************/

/*
 * Convert back and forth from 'sid' to 'card' and 'session'
 */
#define HIFN_CARD(sid)		(((sid) & 0xf0000000) >> 28)
#define HIFN_SESSION(sid)	((sid) & 0x000007ff)
#define HIFN_SID(crd,ses)	(((crd) << 28) | ((ses) & 0x7ff))

#endif /* _KERNEL */

struct hifn_stats {
	u_int64_t hst_ibytes;
	u_int64_t hst_obytes;
	u_int32_t hst_ipackets;
	u_int32_t hst_opackets;
	u_int32_t hst_invalid;
	u_int32_t hst_nomem;		/* malloc or one of hst_nomem_* */
	u_int32_t hst_abort;
	u_int32_t hst_noirq;		/* IRQ for no reason */
	u_int32_t hst_totbatch;		/* ops submitted w/o interrupt */
	u_int32_t hst_maxbatch;		/* max ops submitted together */
	u_int32_t hst_unaligned;	/* unaligned src caused copy */
	/*
	 * The following divides hst_nomem into more specific buckets.
	 */
	u_int32_t hst_nomem_map;	/* bus_dmamap_create failed */
	u_int32_t hst_nomem_load;	/* bus_dmamap_load_* failed */
	u_int32_t hst_nomem_mbuf;	/* MGET* failed */
	u_int32_t hst_nomem_mcl;	/* MCLGET* failed */
	u_int32_t hst_nomem_cr;		/* out of command/result descriptor */
	u_int32_t hst_nomem_sd;		/* out of src/dst descriptors */
};

#endif /* __HIFN7751VAR_H__ */
