/*
 * Hifn HIPP-I/HIPP-II (7855/8155) driver.
 * Copyright (c) 2006 Michael Richardson <mcr@xelerance.com> * 
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
 * Effort sponsored by Hifn inc.
 *
 */

#ifndef __HIFNHIPPVAR_H__
#define __HIFNHIPPVAR_H__

#define HIPP_MAX_CHIPS 8

/*
 * Holds data specific to a single Hifn HIPP-I board.
 */
struct hipp_softc {
	softc_device_decl		 sc_dev;

	struct pci_dev		*sc_pcidev;	/* device backpointer */
	ocf_iomem_t             sc_bar[5];
	caddr_t		        sc_barphy[5];   /* physical address */
	int			sc_num;		/* for multiple devs */
	spinlock_t		sc_mtx;		/* per-instance lock */
	int32_t			sc_cid;
	int			sc_irq;

#if 0

	u_int32_t		sc_dmaier;
	u_int32_t		sc_drammodel;	/* 1=dram, 0=sram */
	u_int32_t		sc_pllconfig;	/* 7954/7955/7956 PLL config */

	struct hifn_dma		*sc_dma;
	dma_addr_t		sc_dma_physaddr;/* physical address of sc_dma */

	int			sc_dmansegs;
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
	struct miscdevice       sc_miscdev;
#endif
};

#define	HIPP_LOCK(_sc)		spin_lock_irqsave(&(_sc)->sc_mtx, l_flags)
#define	HIPP_UNLOCK(_sc)	spin_unlock_irqrestore(&(_sc)->sc_mtx, l_flags)

#endif /* __HIFNHIPPVAR_H__ */
