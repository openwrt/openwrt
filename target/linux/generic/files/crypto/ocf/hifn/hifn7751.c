/*	$OpenBSD: hifn7751.c,v 1.120 2002/05/17 00:33:34 deraadt Exp $	*/

/*-
 * Invertex AEON / Hifn 7751 driver
 * Copyright (c) 1999 Invertex Inc. All rights reserved.
 * Copyright (c) 1999 Theo de Raadt
 * Copyright (c) 2000-2001 Network Security Technologies, Inc.
 *			http://www.netsec.net
 * Copyright (c) 2003 Hifn Inc.
 *
 * This driver is based on a previous driver by Invertex, for which they
 * requested:  Please send any comments, feedback, bug-fixes, or feature
 * requests to software@invertex.com.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
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
 *
__FBSDID("$FreeBSD: src/sys/dev/hifn/hifn7751.c,v 1.40 2007/03/21 03:42:49 sam Exp $");
 */

/*
 * Driver for various Hifn encryption processors.
 */
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/module.h>
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
#include <linux/version.h>
#include <linux/skbuff.h>
#include <asm/io.h>

#include <cryptodev.h>
#include <uio.h>
#include <hifn/hifn7751reg.h>
#include <hifn/hifn7751var.h>

#if 1
#define	DPRINTF(a...)	if (hifn_debug) { \
							printk("%s: ", sc ? \
								device_get_nameunit(sc->sc_dev) : "hifn"); \
							printk(a); \
						} else
#else
#define	DPRINTF(a...)
#endif

static inline int
pci_get_revid(struct pci_dev *dev)
{
	u8 rid = 0;
	pci_read_config_byte(dev, PCI_REVISION_ID, &rid);
	return rid;
}

static	struct hifn_stats hifnstats;

#define	debug hifn_debug
int hifn_debug = 0;
module_param(hifn_debug, int, 0644);
MODULE_PARM_DESC(hifn_debug, "Enable debug");

int hifn_maxbatch = 1;
module_param(hifn_maxbatch, int, 0644);
MODULE_PARM_DESC(hifn_maxbatch, "max ops to batch w/o interrupt");

int hifn_cache_linesize = 0x10;
module_param(hifn_cache_linesize, int, 0444);
MODULE_PARM_DESC(hifn_cache_linesize, "PCI config cache line size");

#ifdef MODULE_PARM
char *hifn_pllconfig = NULL;
MODULE_PARM(hifn_pllconfig, "s");
#else
char hifn_pllconfig[32]; /* This setting is RO after loading */
module_param_string(hifn_pllconfig, hifn_pllconfig, 32, 0444);
#endif
MODULE_PARM_DESC(hifn_pllconfig, "PLL config, ie., pci66, ext33, ...");

#ifdef HIFN_VULCANDEV
#include <sys/conf.h>
#include <sys/uio.h>

static struct cdevsw vulcanpk_cdevsw; /* forward declaration */
#endif

/*
 * Prototypes and count for the pci_device structure
 */
static	int  hifn_probe(struct pci_dev *dev, const struct pci_device_id *ent);
static	void hifn_remove(struct pci_dev *dev);

static	int hifn_newsession(device_t, u_int32_t *, struct cryptoini *);
static	int hifn_freesession(device_t, u_int64_t);
static	int hifn_process(device_t, struct cryptop *, int);

static device_method_t hifn_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	hifn_newsession),
	DEVMETHOD(cryptodev_freesession,hifn_freesession),
	DEVMETHOD(cryptodev_process,	hifn_process),
};

static	void hifn_reset_board(struct hifn_softc *, int);
static	void hifn_reset_puc(struct hifn_softc *);
static	void hifn_puc_wait(struct hifn_softc *);
static	int hifn_enable_crypto(struct hifn_softc *);
static	void hifn_set_retry(struct hifn_softc *sc);
static	void hifn_init_dma(struct hifn_softc *);
static	void hifn_init_pci_registers(struct hifn_softc *);
static	int hifn_sramsize(struct hifn_softc *);
static	int hifn_dramsize(struct hifn_softc *);
static	int hifn_ramtype(struct hifn_softc *);
static	void hifn_sessions(struct hifn_softc *);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
static irqreturn_t hifn_intr(int irq, void *arg);
#else
static irqreturn_t hifn_intr(int irq, void *arg, struct pt_regs *regs);
#endif
static	u_int hifn_write_command(struct hifn_command *, u_int8_t *);
static	u_int32_t hifn_next_signature(u_int32_t a, u_int cnt);
static	void hifn_callback(struct hifn_softc *, struct hifn_command *, u_int8_t *);
static	int hifn_crypto(struct hifn_softc *, struct hifn_command *, struct cryptop *, int);
static	int hifn_readramaddr(struct hifn_softc *, int, u_int8_t *);
static	int hifn_writeramaddr(struct hifn_softc *, int, u_int8_t *);
static	int hifn_dmamap_load_src(struct hifn_softc *, struct hifn_command *);
static	int hifn_dmamap_load_dst(struct hifn_softc *, struct hifn_command *);
static	int hifn_init_pubrng(struct hifn_softc *);
static	void hifn_tick(unsigned long arg);
static	void hifn_abort(struct hifn_softc *);
static	void hifn_alloc_slot(struct hifn_softc *, int *, int *, int *, int *);

static	void hifn_write_reg_0(struct hifn_softc *, bus_size_t, u_int32_t);
static	void hifn_write_reg_1(struct hifn_softc *, bus_size_t, u_int32_t);

#ifdef CONFIG_OCF_RANDOMHARVEST
static	int hifn_read_random(void *arg, u_int32_t *buf, int len);
#endif

#define HIFN_MAX_CHIPS	8
static struct hifn_softc *hifn_chip_idx[HIFN_MAX_CHIPS];

static __inline u_int32_t
READ_REG_0(struct hifn_softc *sc, bus_size_t reg)
{
	u_int32_t v = readl(sc->sc_bar0 + reg);
	sc->sc_bar0_lastreg = (bus_size_t) -1;
	return (v);
}
#define	WRITE_REG_0(sc, reg, val)	hifn_write_reg_0(sc, reg, val)

static __inline u_int32_t
READ_REG_1(struct hifn_softc *sc, bus_size_t reg)
{
	u_int32_t v = readl(sc->sc_bar1 + reg);
	sc->sc_bar1_lastreg = (bus_size_t) -1;
	return (v);
}
#define	WRITE_REG_1(sc, reg, val)	hifn_write_reg_1(sc, reg, val)

/*
 * map in a given buffer (great on some arches :-)
 */

static int
pci_map_uio(struct hifn_softc *sc, struct hifn_operand *buf, struct uio *uio)
{
	struct iovec *iov = uio->uio_iov;

	DPRINTF("%s()\n", __FUNCTION__);

	buf->mapsize = 0;
	for (buf->nsegs = 0; buf->nsegs < uio->uio_iovcnt; ) {
		buf->segs[buf->nsegs].ds_addr = pci_map_single(sc->sc_pcidev,
				iov->iov_base, iov->iov_len,
				PCI_DMA_BIDIRECTIONAL);
		buf->segs[buf->nsegs].ds_len = iov->iov_len;
		buf->mapsize += iov->iov_len;
		iov++;
		buf->nsegs++;
	}
	/* identify this buffer by the first segment */
	buf->map = (void *) buf->segs[0].ds_addr;
	return(0);
}

/*
 * map in a given sk_buff
 */

static int
pci_map_skb(struct hifn_softc *sc,struct hifn_operand *buf,struct sk_buff *skb)
{
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	buf->mapsize = 0;

	buf->segs[0].ds_addr = pci_map_single(sc->sc_pcidev,
			skb->data, skb_headlen(skb), PCI_DMA_BIDIRECTIONAL);
	buf->segs[0].ds_len = skb_headlen(skb);
	buf->mapsize += buf->segs[0].ds_len;

	buf->nsegs = 1;

	for (i = 0; i < skb_shinfo(skb)->nr_frags; ) {
		buf->segs[buf->nsegs].ds_len = skb_shinfo(skb)->frags[i].size;
		buf->segs[buf->nsegs].ds_addr = pci_map_single(sc->sc_pcidev,
				page_address(skb_shinfo(skb)->frags[i].page) +
					skb_shinfo(skb)->frags[i].page_offset,
				buf->segs[buf->nsegs].ds_len, PCI_DMA_BIDIRECTIONAL);
		buf->mapsize += buf->segs[buf->nsegs].ds_len;
		buf->nsegs++;
	}

	/* identify this buffer by the first segment */
	buf->map = (void *) buf->segs[0].ds_addr;
	return(0);
}

/*
 * map in a given contiguous buffer
 */

static int
pci_map_buf(struct hifn_softc *sc,struct hifn_operand *buf, void *b, int len)
{
	DPRINTF("%s()\n", __FUNCTION__);

	buf->mapsize = 0;
	buf->segs[0].ds_addr = pci_map_single(sc->sc_pcidev,
			b, len, PCI_DMA_BIDIRECTIONAL);
	buf->segs[0].ds_len = len;
	buf->mapsize += buf->segs[0].ds_len;
	buf->nsegs = 1;

	/* identify this buffer by the first segment */
	buf->map = (void *) buf->segs[0].ds_addr;
	return(0);
}

#if 0 /* not needed at this time */
static void
pci_sync_iov(struct hifn_softc *sc, struct hifn_operand *buf)
{
	int i;

	DPRINTF("%s()\n", __FUNCTION__);
	for (i = 0; i < buf->nsegs; i++)
		pci_dma_sync_single_for_cpu(sc->sc_pcidev, buf->segs[i].ds_addr,
				buf->segs[i].ds_len, PCI_DMA_BIDIRECTIONAL);
}
#endif

static void
pci_unmap_buf(struct hifn_softc *sc, struct hifn_operand *buf)
{
	int i;
	DPRINTF("%s()\n", __FUNCTION__);
	for (i = 0; i < buf->nsegs; i++) {
		pci_unmap_single(sc->sc_pcidev, buf->segs[i].ds_addr,
				buf->segs[i].ds_len, PCI_DMA_BIDIRECTIONAL);
		buf->segs[i].ds_addr = 0;
		buf->segs[i].ds_len = 0;
	}
	buf->nsegs = 0;
	buf->mapsize = 0;
	buf->map = 0;
}

static const char*
hifn_partname(struct hifn_softc *sc)
{
	/* XXX sprintf numbers when not decoded */
	switch (pci_get_vendor(sc->sc_pcidev)) {
	case PCI_VENDOR_HIFN:
		switch (pci_get_device(sc->sc_pcidev)) {
		case PCI_PRODUCT_HIFN_6500:	return "Hifn 6500";
		case PCI_PRODUCT_HIFN_7751:	return "Hifn 7751";
		case PCI_PRODUCT_HIFN_7811:	return "Hifn 7811";
		case PCI_PRODUCT_HIFN_7951:	return "Hifn 7951";
		case PCI_PRODUCT_HIFN_7955:	return "Hifn 7955";
		case PCI_PRODUCT_HIFN_7956:	return "Hifn 7956";
		}
		return "Hifn unknown-part";
	case PCI_VENDOR_INVERTEX:
		switch (pci_get_device(sc->sc_pcidev)) {
		case PCI_PRODUCT_INVERTEX_AEON:	return "Invertex AEON";
		}
		return "Invertex unknown-part";
	case PCI_VENDOR_NETSEC:
		switch (pci_get_device(sc->sc_pcidev)) {
		case PCI_PRODUCT_NETSEC_7751:	return "NetSec 7751";
		}
		return "NetSec unknown-part";
	}
	return "Unknown-vendor unknown-part";
}

static u_int
checkmaxmin(struct pci_dev *dev, const char *what, u_int v, u_int min, u_int max)
{
	struct hifn_softc *sc = pci_get_drvdata(dev);
	if (v > max) {
		device_printf(sc->sc_dev, "Warning, %s %u out of range, "
			"using max %u\n", what, v, max);
		v = max;
	} else if (v < min) {
		device_printf(sc->sc_dev, "Warning, %s %u out of range, "
			"using min %u\n", what, v, min);
		v = min;
	}
	return v;
}

/*
 * Select PLL configuration for 795x parts.  This is complicated in
 * that we cannot determine the optimal parameters without user input.
 * The reference clock is derived from an external clock through a
 * multiplier.  The external clock is either the host bus (i.e. PCI)
 * or an external clock generator.  When using the PCI bus we assume
 * the clock is either 33 or 66 MHz; for an external source we cannot
 * tell the speed.
 *
 * PLL configuration is done with a string: "pci" for PCI bus, or "ext"
 * for an external source, followed by the frequency.  We calculate
 * the appropriate multiplier and PLL register contents accordingly.
 * When no configuration is given we default to "pci66" since that
 * always will allow the card to work.  If a card is using the PCI
 * bus clock and in a 33MHz slot then it will be operating at half
 * speed until the correct information is provided.
 *
 * We use a default setting of "ext66" because according to Mike Ham
 * of HiFn, almost every board in existence has an external crystal
 * populated at 66Mhz. Using PCI can be a problem on modern motherboards,
 * because PCI33 can have clocks from 0 to 33Mhz, and some have
 * non-PCI-compliant spread-spectrum clocks, which can confuse the pll.
 */
static void
hifn_getpllconfig(struct pci_dev *dev, u_int *pll)
{
	const char *pllspec = hifn_pllconfig;
	u_int freq, mul, fl, fh;
	u_int32_t pllconfig;
	char *nxt;

	if (pllspec == NULL)
		pllspec = "ext66";
	fl = 33, fh = 66;
	pllconfig = 0;
	if (strncmp(pllspec, "ext", 3) == 0) {
		pllspec += 3;
		pllconfig |= HIFN_PLL_REF_SEL;
		switch (pci_get_device(dev)) {
		case PCI_PRODUCT_HIFN_7955:
		case PCI_PRODUCT_HIFN_7956:
			fl = 20, fh = 100;
			break;
#ifdef notyet
		case PCI_PRODUCT_HIFN_7954:
			fl = 20, fh = 66;
			break;
#endif
		}
	} else if (strncmp(pllspec, "pci", 3) == 0)
		pllspec += 3;
	freq = strtoul(pllspec, &nxt, 10);
	if (nxt == pllspec)
		freq = 66;
	else
		freq = checkmaxmin(dev, "frequency", freq, fl, fh);
	/*
	 * Calculate multiplier.  We target a Fck of 266 MHz,
	 * allowing only even values, possibly rounded down.
	 * Multipliers > 8 must set the charge pump current.
	 */
	mul = checkmaxmin(dev, "PLL divisor", (266 / freq) &~ 1, 2, 12);
	pllconfig |= (mul / 2 - 1) << HIFN_PLL_ND_SHIFT;
	if (mul > 8)
		pllconfig |= HIFN_PLL_IS;
	*pll = pllconfig;
}

/*
 * Attach an interface that successfully probed.
 */
static int
hifn_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	struct hifn_softc *sc = NULL;
	char rbase;
	u_int16_t ena, rev;
	int rseg, rc;
	unsigned long mem_start, mem_len;
	static int num_chips = 0;

	DPRINTF("%s()\n", __FUNCTION__);

	if (pci_enable_device(dev) < 0)
		return(-ENODEV);

#ifdef CONFIG_HAVE_PCI_SET_MWI
	if (pci_set_mwi(dev))
		return(-ENODEV);
#endif

	if (!dev->irq) {
		printk("hifn: found device with no IRQ assigned. check BIOS settings!");
		pci_disable_device(dev);
		return(-ENODEV);
	}

	sc = (struct hifn_softc *) kmalloc(sizeof(*sc), GFP_KERNEL);
	if (!sc)
		return(-ENOMEM);
	memset(sc, 0, sizeof(*sc));

	softc_device_init(sc, "hifn", num_chips, hifn_methods);

	sc->sc_pcidev = dev;
	sc->sc_irq = -1;
	sc->sc_cid = -1;
	sc->sc_num = num_chips++;
	if (sc->sc_num < HIFN_MAX_CHIPS)
		hifn_chip_idx[sc->sc_num] = sc;

	pci_set_drvdata(sc->sc_pcidev, sc);

	spin_lock_init(&sc->sc_mtx);

	/* XXX handle power management */

	/*
	 * The 7951 and 795x have a random number generator and
	 * public key support; note this.
	 */
	if (pci_get_vendor(dev) == PCI_VENDOR_HIFN &&
	    (pci_get_device(dev) == PCI_PRODUCT_HIFN_7951 ||
	     pci_get_device(dev) == PCI_PRODUCT_HIFN_7955 ||
	     pci_get_device(dev) == PCI_PRODUCT_HIFN_7956))
		sc->sc_flags = HIFN_HAS_RNG | HIFN_HAS_PUBLIC;
	/*
	 * The 7811 has a random number generator and
	 * we also note it's identity 'cuz of some quirks.
	 */
	if (pci_get_vendor(dev) == PCI_VENDOR_HIFN &&
	    pci_get_device(dev) == PCI_PRODUCT_HIFN_7811)
		sc->sc_flags |= HIFN_IS_7811 | HIFN_HAS_RNG;

	/*
	 * The 795x parts support AES.
	 */
	if (pci_get_vendor(dev) == PCI_VENDOR_HIFN &&
	    (pci_get_device(dev) == PCI_PRODUCT_HIFN_7955 ||
	     pci_get_device(dev) == PCI_PRODUCT_HIFN_7956)) {
		sc->sc_flags |= HIFN_IS_7956 | HIFN_HAS_AES;
		/*
		 * Select PLL configuration.  This depends on the
		 * bus and board design and must be manually configured
		 * if the default setting is unacceptable.
		 */
		hifn_getpllconfig(dev, &sc->sc_pllconfig);
	}

	/*
	 * Setup PCI resources. Note that we record the bus
	 * tag and handle for each register mapping, this is
	 * used by the READ_REG_0, WRITE_REG_0, READ_REG_1,
	 * and WRITE_REG_1 macros throughout the driver.
	 */
	mem_start = pci_resource_start(sc->sc_pcidev, 0);
	mem_len   = pci_resource_len(sc->sc_pcidev, 0);
	sc->sc_bar0 = (ocf_iomem_t) ioremap(mem_start, mem_len);
	if (!sc->sc_bar0) {
		device_printf(sc->sc_dev, "cannot map bar%d register space\n", 0);
		goto fail;
	}
	sc->sc_bar0_lastreg = (bus_size_t) -1;

	mem_start = pci_resource_start(sc->sc_pcidev, 1);
	mem_len   = pci_resource_len(sc->sc_pcidev, 1);
	sc->sc_bar1 = (ocf_iomem_t) ioremap(mem_start, mem_len);
	if (!sc->sc_bar1) {
		device_printf(sc->sc_dev, "cannot map bar%d register space\n", 1);
		goto fail;
	}
	sc->sc_bar1_lastreg = (bus_size_t) -1;

	/* fix up the bus size */
	if (pci_set_dma_mask(dev, DMA_32BIT_MASK)) {
		device_printf(sc->sc_dev, "No usable DMA configuration, aborting.\n");
		goto fail;
	}
	if (pci_set_consistent_dma_mask(dev, DMA_32BIT_MASK)) {
		device_printf(sc->sc_dev,
				"No usable consistent DMA configuration, aborting.\n");
		goto fail;
	}

	hifn_set_retry(sc);

	/*
	 * Setup the area where the Hifn DMA's descriptors
	 * and associated data structures.
	 */
	sc->sc_dma = (struct hifn_dma *) pci_alloc_consistent(dev,
			sizeof(*sc->sc_dma),
			&sc->sc_dma_physaddr);
	if (!sc->sc_dma) {
		device_printf(sc->sc_dev, "cannot alloc sc_dma\n");
		goto fail;
	}
	bzero(sc->sc_dma, sizeof(*sc->sc_dma));

	/*
	 * Reset the board and do the ``secret handshake''
	 * to enable the crypto support.  Then complete the
	 * initialization procedure by setting up the interrupt
	 * and hooking in to the system crypto support so we'll
	 * get used for system services like the crypto device,
	 * IPsec, RNG device, etc.
	 */
	hifn_reset_board(sc, 0);

	if (hifn_enable_crypto(sc) != 0) {
		device_printf(sc->sc_dev, "crypto enabling failed\n");
		goto fail;
	}
	hifn_reset_puc(sc);

	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);

	pci_set_master(sc->sc_pcidev);

	/* XXX can't dynamically determine ram type for 795x; force dram */
	if (sc->sc_flags & HIFN_IS_7956)
		sc->sc_drammodel = 1;
	else if (hifn_ramtype(sc))
		goto fail;

	if (sc->sc_drammodel == 0)
		hifn_sramsize(sc);
	else
		hifn_dramsize(sc);

	/*
	 * Workaround for NetSec 7751 rev A: half ram size because two
	 * of the address lines were left floating
	 */
	if (pci_get_vendor(dev) == PCI_VENDOR_NETSEC &&
	    pci_get_device(dev) == PCI_PRODUCT_NETSEC_7751 &&
	    pci_get_revid(dev) == 0x61)	/*XXX???*/
		sc->sc_ramsize >>= 1;

	/*
	 * Arrange the interrupt line.
	 */
	rc = request_irq(dev->irq, hifn_intr, IRQF_SHARED, "hifn", sc);
	if (rc) {
		device_printf(sc->sc_dev, "could not map interrupt: %d\n", rc);
		goto fail;
	}
	sc->sc_irq = dev->irq;

	hifn_sessions(sc);

	/*
	 * NB: Keep only the low 16 bits; this masks the chip id
	 *     from the 7951.
	 */
	rev = READ_REG_1(sc, HIFN_1_REVID) & 0xffff;

	rseg = sc->sc_ramsize / 1024;
	rbase = 'K';
	if (sc->sc_ramsize >= (1024 * 1024)) {
		rbase = 'M';
		rseg /= 1024;
	}
	device_printf(sc->sc_dev, "%s, rev %u, %d%cB %cram",
		hifn_partname(sc), rev,
		rseg, rbase, sc->sc_drammodel ? 'd' : 's');
	if (sc->sc_flags & HIFN_IS_7956)
		printf(", pll=0x%x<%s clk, %ux mult>",
			sc->sc_pllconfig,
			sc->sc_pllconfig & HIFN_PLL_REF_SEL ? "ext" : "pci",
			2 + 2*((sc->sc_pllconfig & HIFN_PLL_ND) >> 11));
	printf("\n");

	sc->sc_cid = crypto_get_driverid(softc_get_device(sc),CRYPTOCAP_F_HARDWARE);
	if (sc->sc_cid < 0) {
		device_printf(sc->sc_dev, "could not get crypto driver id\n");
		goto fail;
	}

	WRITE_REG_0(sc, HIFN_0_PUCNFG,
	    READ_REG_0(sc, HIFN_0_PUCNFG) | HIFN_PUCNFG_CHIPID);
	ena = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

	switch (ena) {
	case HIFN_PUSTAT_ENA_2:
		crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_ARC4, 0, 0);
		if (sc->sc_flags & HIFN_HAS_AES)
			crypto_register(sc->sc_cid, CRYPTO_AES_CBC, 0, 0);
		/*FALLTHROUGH*/
	case HIFN_PUSTAT_ENA_1:
		crypto_register(sc->sc_cid, CRYPTO_MD5, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_SHA1, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_MD5_HMAC, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_SHA1_HMAC, 0, 0);
		crypto_register(sc->sc_cid, CRYPTO_DES_CBC, 0, 0);
		break;
	}

	if (sc->sc_flags & (HIFN_HAS_PUBLIC | HIFN_HAS_RNG))
		hifn_init_pubrng(sc);

	init_timer(&sc->sc_tickto);
	sc->sc_tickto.function = hifn_tick;
	sc->sc_tickto.data = (unsigned long) sc->sc_num;
	mod_timer(&sc->sc_tickto, jiffies + HZ);

	return (0);

fail:
    if (sc->sc_cid >= 0)
        crypto_unregister_all(sc->sc_cid);
    if (sc->sc_irq != -1)
        free_irq(sc->sc_irq, sc);
    if (sc->sc_dma) {
		/* Turn off DMA polling */
		WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
			HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

        pci_free_consistent(sc->sc_pcidev,
				sizeof(*sc->sc_dma),
                sc->sc_dma, sc->sc_dma_physaddr);
	}
    kfree(sc);
	return (-ENXIO);
}

/*
 * Detach an interface that successfully probed.
 */
static void
hifn_remove(struct pci_dev *dev)
{
	struct hifn_softc *sc = pci_get_drvdata(dev);
	unsigned long l_flags;

	DPRINTF("%s()\n", __FUNCTION__);

	KASSERT(sc != NULL, ("hifn_detach: null software carrier!"));

	/* disable interrupts */
	HIFN_LOCK(sc);
	WRITE_REG_1(sc, HIFN_1_DMA_IER, 0);
	HIFN_UNLOCK(sc);

	/*XXX other resources */
	del_timer_sync(&sc->sc_tickto);

	/* Turn off DMA polling */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

	crypto_unregister_all(sc->sc_cid);

	free_irq(sc->sc_irq, sc);

	pci_free_consistent(sc->sc_pcidev, sizeof(*sc->sc_dma),
                sc->sc_dma, sc->sc_dma_physaddr);
}


static int
hifn_init_pubrng(struct hifn_softc *sc)
{
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	if ((sc->sc_flags & HIFN_IS_7811) == 0) {
		/* Reset 7951 public key/rng engine */
		WRITE_REG_1(sc, HIFN_1_PUB_RESET,
		    READ_REG_1(sc, HIFN_1_PUB_RESET) | HIFN_PUBRST_RESET);

		for (i = 0; i < 100; i++) {
			DELAY(1000);
			if ((READ_REG_1(sc, HIFN_1_PUB_RESET) &
			    HIFN_PUBRST_RESET) == 0)
				break;
		}

		if (i == 100) {
			device_printf(sc->sc_dev, "public key init failed\n");
			return (1);
		}
	}

	/* Enable the rng, if available */
#ifdef CONFIG_OCF_RANDOMHARVEST
	if (sc->sc_flags & HIFN_HAS_RNG) {
		if (sc->sc_flags & HIFN_IS_7811) {
			u_int32_t r;
			r = READ_REG_1(sc, HIFN_1_7811_RNGENA);
			if (r & HIFN_7811_RNGENA_ENA) {
				r &= ~HIFN_7811_RNGENA_ENA;
				WRITE_REG_1(sc, HIFN_1_7811_RNGENA, r);
			}
			WRITE_REG_1(sc, HIFN_1_7811_RNGCFG,
			    HIFN_7811_RNGCFG_DEFL);
			r |= HIFN_7811_RNGENA_ENA;
			WRITE_REG_1(sc, HIFN_1_7811_RNGENA, r);
		} else
			WRITE_REG_1(sc, HIFN_1_RNG_CONFIG,
			    READ_REG_1(sc, HIFN_1_RNG_CONFIG) |
			    HIFN_RNGCFG_ENA);

		sc->sc_rngfirst = 1;
		crypto_rregister(sc->sc_cid, hifn_read_random, sc);
	}
#endif

	/* Enable public key engine, if available */
	if (sc->sc_flags & HIFN_HAS_PUBLIC) {
		WRITE_REG_1(sc, HIFN_1_PUB_IEN, HIFN_PUBIEN_DONE);
		sc->sc_dmaier |= HIFN_DMAIER_PUBDONE;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
#ifdef HIFN_VULCANDEV
		sc->sc_pkdev = make_dev(&vulcanpk_cdevsw, 0, 
					UID_ROOT, GID_WHEEL, 0666,
					"vulcanpk");
		sc->sc_pkdev->si_drv1 = sc;
#endif
	}

	return (0);
}

#ifdef CONFIG_OCF_RANDOMHARVEST
static int
hifn_read_random(void *arg, u_int32_t *buf, int len)
{
	struct hifn_softc *sc = (struct hifn_softc *) arg;
	u_int32_t sts;
	int i, rc = 0;

	if (len <= 0)
		return rc;

	if (sc->sc_flags & HIFN_IS_7811) {
		/* ONLY VALID ON 7811!!!! */
		for (i = 0; i < 5; i++) {
			sts = READ_REG_1(sc, HIFN_1_7811_RNGSTS);
			if (sts & HIFN_7811_RNGSTS_UFL) {
				device_printf(sc->sc_dev,
					      "RNG underflow: disabling\n");
				/* DAVIDM perhaps return -1 */
				break;
			}
			if ((sts & HIFN_7811_RNGSTS_RDY) == 0)
				break;

			/*
			 * There are at least two words in the RNG FIFO
			 * at this point.
			 */
			if (rc < len)
				buf[rc++] = READ_REG_1(sc, HIFN_1_7811_RNGDAT);
			if (rc < len)
				buf[rc++] = READ_REG_1(sc, HIFN_1_7811_RNGDAT);
		}
	} else
		buf[rc++] = READ_REG_1(sc, HIFN_1_RNG_DATA);

	/* NB: discard first data read */
	if (sc->sc_rngfirst) {
		sc->sc_rngfirst = 0;
		rc = 0;
	}

	return(rc);
}
#endif /* CONFIG_OCF_RANDOMHARVEST */

static void
hifn_puc_wait(struct hifn_softc *sc)
{
	int i;
	int reg = HIFN_0_PUCTRL;

	if (sc->sc_flags & HIFN_IS_7956) {
		reg = HIFN_0_PUCTRL2;
	}

	for (i = 5000; i > 0; i--) {
		DELAY(1);
		if (!(READ_REG_0(sc, reg) & HIFN_PUCTRL_RESET))
			break;
	}
	if (!i)
		device_printf(sc->sc_dev, "proc unit did not reset(0x%x)\n",
				READ_REG_0(sc, HIFN_0_PUCTRL));
}

/*
 * Reset the processing unit.
 */
static void
hifn_reset_puc(struct hifn_softc *sc)
{
	/* Reset processing unit */
	int reg = HIFN_0_PUCTRL;

	if (sc->sc_flags & HIFN_IS_7956) {
		reg = HIFN_0_PUCTRL2;
	}
	WRITE_REG_0(sc, reg, HIFN_PUCTRL_DMAENA);

	hifn_puc_wait(sc);
}

/*
 * Set the Retry and TRDY registers; note that we set them to
 * zero because the 7811 locks up when forced to retry (section
 * 3.6 of "Specification Update SU-0014-04".  Not clear if we
 * should do this for all Hifn parts, but it doesn't seem to hurt.
 */
static void
hifn_set_retry(struct hifn_softc *sc)
{
	DPRINTF("%s()\n", __FUNCTION__);
	/* NB: RETRY only responds to 8-bit reads/writes */
	pci_write_config_byte(sc->sc_pcidev, HIFN_RETRY_TIMEOUT, 0);
	pci_write_config_dword(sc->sc_pcidev, HIFN_TRDY_TIMEOUT, 0);
	/* piggy back the cache line setting here */
	pci_write_config_byte(sc->sc_pcidev, PCI_CACHE_LINE_SIZE, hifn_cache_linesize);
}

/*
 * Resets the board.  Values in the regesters are left as is
 * from the reset (i.e. initial values are assigned elsewhere).
 */
static void
hifn_reset_board(struct hifn_softc *sc, int full)
{
	u_int32_t reg;

	DPRINTF("%s()\n", __FUNCTION__);
	/*
	 * Set polling in the DMA configuration register to zero.  0x7 avoids
	 * resetting the board and zeros out the other fields.
	 */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

	/*
	 * Now that polling has been disabled, we have to wait 1 ms
	 * before resetting the board.
	 */
	DELAY(1000);

	/* Reset the DMA unit */
	if (full) {
		WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MODE);
		DELAY(1000);
	} else {
		WRITE_REG_1(sc, HIFN_1_DMA_CNFG,
		    HIFN_DMACNFG_MODE | HIFN_DMACNFG_MSTRESET);
		hifn_reset_puc(sc);
	}

	KASSERT(sc->sc_dma != NULL, ("hifn_reset_board: null DMA tag!"));
	bzero(sc->sc_dma, sizeof(*sc->sc_dma));

	/* Bring dma unit out of reset */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);

	hifn_puc_wait(sc);
	hifn_set_retry(sc);

	if (sc->sc_flags & HIFN_IS_7811) {
		for (reg = 0; reg < 1000; reg++) {
			if (READ_REG_1(sc, HIFN_1_7811_MIPSRST) &
			    HIFN_MIPSRST_CRAMINIT)
				break;
			DELAY(1000);
		}
		if (reg == 1000)
			device_printf(sc->sc_dev, ": cram init timeout\n");
	} else {
	  /* set up DMA configuration register #2 */
	  /* turn off all PK and BAR0 swaps */
	  WRITE_REG_1(sc, HIFN_1_DMA_CNFG2,
		      (3 << HIFN_DMACNFG2_INIT_WRITE_BURST_SHIFT)|
		      (3 << HIFN_DMACNFG2_INIT_READ_BURST_SHIFT)|
		      (2 << HIFN_DMACNFG2_TGT_WRITE_BURST_SHIFT)|
		      (2 << HIFN_DMACNFG2_TGT_READ_BURST_SHIFT));
	}
}

static u_int32_t
hifn_next_signature(u_int32_t a, u_int cnt)
{
	int i;
	u_int32_t v;

	for (i = 0; i < cnt; i++) {

		/* get the parity */
		v = a & 0x80080125;
		v ^= v >> 16;
		v ^= v >> 8;
		v ^= v >> 4;
		v ^= v >> 2;
		v ^= v >> 1;

		a = (v & 1) ^ (a << 1);
	}

	return a;
}


/*
 * Checks to see if crypto is already enabled.  If crypto isn't enable,
 * "hifn_enable_crypto" is called to enable it.  The check is important,
 * as enabling crypto twice will lock the board.
 */
static int 
hifn_enable_crypto(struct hifn_softc *sc)
{
	u_int32_t dmacfg, ramcfg, encl, addr, i;
	char offtbl[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					  0x00, 0x00, 0x00, 0x00 };

	DPRINTF("%s()\n", __FUNCTION__);

	ramcfg = READ_REG_0(sc, HIFN_0_PUCNFG);
	dmacfg = READ_REG_1(sc, HIFN_1_DMA_CNFG);

	/*
	 * The RAM config register's encrypt level bit needs to be set before
	 * every read performed on the encryption level register.
	 */
	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg | HIFN_PUCNFG_CHIPID);

	encl = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

	/*
	 * Make sure we don't re-unlock.  Two unlocks kills chip until the
	 * next reboot.
	 */
	if (encl == HIFN_PUSTAT_ENA_1 || encl == HIFN_PUSTAT_ENA_2) {
#ifdef HIFN_DEBUG
		if (hifn_debug)
			device_printf(sc->sc_dev,
			    "Strong crypto already enabled!\n");
#endif
		goto report;
	}

	if (encl != 0 && encl != HIFN_PUSTAT_ENA_0) {
#ifdef HIFN_DEBUG
		if (hifn_debug)
			device_printf(sc->sc_dev,
			      "Unknown encryption level 0x%x\n", encl);
#endif
		return 1;
	}

	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_UNLOCK |
	    HIFN_DMACNFG_MSTRESET | HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);
	DELAY(1000);
	addr = READ_REG_1(sc, HIFN_UNLOCK_SECRET1);
	DELAY(1000);
	WRITE_REG_1(sc, HIFN_UNLOCK_SECRET2, 0);
	DELAY(1000);

	for (i = 0; i <= 12; i++) {
		addr = hifn_next_signature(addr, offtbl[i] + 0x101);
		WRITE_REG_1(sc, HIFN_UNLOCK_SECRET2, addr);

		DELAY(1000);
	}

	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg | HIFN_PUCNFG_CHIPID);
	encl = READ_REG_0(sc, HIFN_0_PUSTAT) & HIFN_PUSTAT_CHIPENA;

#ifdef HIFN_DEBUG
	if (hifn_debug) {
		if (encl != HIFN_PUSTAT_ENA_1 && encl != HIFN_PUSTAT_ENA_2)
			device_printf(sc->sc_dev, "Engine is permanently "
				"locked until next system reset!\n");
		else
			device_printf(sc->sc_dev, "Engine enabled "
				"successfully!\n");
	}
#endif

report:
	WRITE_REG_0(sc, HIFN_0_PUCNFG, ramcfg);
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, dmacfg);

	switch (encl) {
	case HIFN_PUSTAT_ENA_1:
	case HIFN_PUSTAT_ENA_2:
		break;
	case HIFN_PUSTAT_ENA_0:
	default:
		device_printf(sc->sc_dev, "disabled\n");
		break;
	}

	return 0;
}

/*
 * Give initial values to the registers listed in the "Register Space"
 * section of the HIFN Software Development reference manual.
 */
static void 
hifn_init_pci_registers(struct hifn_softc *sc)
{
	DPRINTF("%s()\n", __FUNCTION__);

	/* write fixed values needed by the Initialization registers */
	WRITE_REG_0(sc, HIFN_0_PUCTRL, HIFN_PUCTRL_DMAENA);
	WRITE_REG_0(sc, HIFN_0_FIFOCNFG, HIFN_FIFOCNFG_THRESHOLD);
	WRITE_REG_0(sc, HIFN_0_PUIER, HIFN_PUIER_DSTOVER);

	/* write all 4 ring address registers */
	WRITE_REG_1(sc, HIFN_1_DMA_CRAR, sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, cmdr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_SRAR, sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, srcr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_DRAR, sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, dstr[0]));
	WRITE_REG_1(sc, HIFN_1_DMA_RRAR, sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, resr[0]));

	DELAY(2000);

	/* write status register */
	WRITE_REG_1(sc, HIFN_1_DMA_CSR,
	    HIFN_DMACSR_D_CTRL_DIS | HIFN_DMACSR_R_CTRL_DIS |
	    HIFN_DMACSR_S_CTRL_DIS | HIFN_DMACSR_C_CTRL_DIS |
	    HIFN_DMACSR_D_ABORT | HIFN_DMACSR_D_DONE | HIFN_DMACSR_D_LAST |
	    HIFN_DMACSR_D_WAIT | HIFN_DMACSR_D_OVER |
	    HIFN_DMACSR_R_ABORT | HIFN_DMACSR_R_DONE | HIFN_DMACSR_R_LAST |
	    HIFN_DMACSR_R_WAIT | HIFN_DMACSR_R_OVER |
	    HIFN_DMACSR_S_ABORT | HIFN_DMACSR_S_DONE | HIFN_DMACSR_S_LAST |
	    HIFN_DMACSR_S_WAIT |
	    HIFN_DMACSR_C_ABORT | HIFN_DMACSR_C_DONE | HIFN_DMACSR_C_LAST |
	    HIFN_DMACSR_C_WAIT |
	    HIFN_DMACSR_ENGINE |
	    ((sc->sc_flags & HIFN_HAS_PUBLIC) ?
		HIFN_DMACSR_PUBDONE : 0) |
	    ((sc->sc_flags & HIFN_IS_7811) ?
		HIFN_DMACSR_ILLW | HIFN_DMACSR_ILLR : 0));

	sc->sc_d_busy = sc->sc_r_busy = sc->sc_s_busy = sc->sc_c_busy = 0;
	sc->sc_dmaier |= HIFN_DMAIER_R_DONE | HIFN_DMAIER_C_ABORT |
	    HIFN_DMAIER_D_OVER | HIFN_DMAIER_R_OVER |
	    HIFN_DMAIER_S_ABORT | HIFN_DMAIER_D_ABORT | HIFN_DMAIER_R_ABORT |
	    ((sc->sc_flags & HIFN_IS_7811) ?
		HIFN_DMAIER_ILLW | HIFN_DMAIER_ILLR : 0);
	sc->sc_dmaier &= ~HIFN_DMAIER_C_WAIT;
	WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);


	if (sc->sc_flags & HIFN_IS_7956) {
		u_int32_t pll;

		WRITE_REG_0(sc, HIFN_0_PUCNFG, HIFN_PUCNFG_COMPSING |
		    HIFN_PUCNFG_TCALLPHASES |
		    HIFN_PUCNFG_TCDRVTOTEM | HIFN_PUCNFG_BUS32);

		/* turn off the clocks and insure bypass is set */
		pll = READ_REG_1(sc, HIFN_1_PLL);
		pll = (pll &~ (HIFN_PLL_PK_CLK_SEL | HIFN_PLL_PE_CLK_SEL))
		  | HIFN_PLL_BP | HIFN_PLL_MBSET;
		WRITE_REG_1(sc, HIFN_1_PLL, pll);
		DELAY(10*1000);		/* 10ms */

		/* change configuration */
		pll = (pll &~ HIFN_PLL_CONFIG) | sc->sc_pllconfig;
		WRITE_REG_1(sc, HIFN_1_PLL, pll);
		DELAY(10*1000);		/* 10ms */

		/* disable bypass */
		pll &= ~HIFN_PLL_BP;
		WRITE_REG_1(sc, HIFN_1_PLL, pll);
		/* enable clocks with new configuration */
		pll |= HIFN_PLL_PK_CLK_SEL | HIFN_PLL_PE_CLK_SEL;
		WRITE_REG_1(sc, HIFN_1_PLL, pll);
	} else {
		WRITE_REG_0(sc, HIFN_0_PUCNFG, HIFN_PUCNFG_COMPSING |
		    HIFN_PUCNFG_DRFR_128 | HIFN_PUCNFG_TCALLPHASES |
		    HIFN_PUCNFG_TCDRVTOTEM | HIFN_PUCNFG_BUS32 |
		    (sc->sc_drammodel ? HIFN_PUCNFG_DRAM : HIFN_PUCNFG_SRAM));
	}

	WRITE_REG_0(sc, HIFN_0_PUISR, HIFN_PUISR_DSTOVER);
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE | HIFN_DMACNFG_LAST |
	    ((HIFN_POLL_FREQUENCY << 16 ) & HIFN_DMACNFG_POLLFREQ) |
	    ((HIFN_POLL_SCALAR << 8) & HIFN_DMACNFG_POLLINVAL));
}

/*
 * The maximum number of sessions supported by the card
 * is dependent on the amount of context ram, which
 * encryption algorithms are enabled, and how compression
 * is configured.  This should be configured before this
 * routine is called.
 */
static void
hifn_sessions(struct hifn_softc *sc)
{
	u_int32_t pucnfg;
	int ctxsize;

	DPRINTF("%s()\n", __FUNCTION__);

	pucnfg = READ_REG_0(sc, HIFN_0_PUCNFG);

	if (pucnfg & HIFN_PUCNFG_COMPSING) {
		if (pucnfg & HIFN_PUCNFG_ENCCNFG)
			ctxsize = 128;
		else
			ctxsize = 512;
		/*
		 * 7955/7956 has internal context memory of 32K
		 */
		if (sc->sc_flags & HIFN_IS_7956)
			sc->sc_maxses = 32768 / ctxsize;
		else
			sc->sc_maxses = 1 +
			    ((sc->sc_ramsize - 32768) / ctxsize);
	} else
		sc->sc_maxses = sc->sc_ramsize / 16384;

	if (sc->sc_maxses > 2048)
		sc->sc_maxses = 2048;
}

/*
 * Determine ram type (sram or dram).  Board should be just out of a reset
 * state when this is called.
 */
static int
hifn_ramtype(struct hifn_softc *sc)
{
	u_int8_t data[8], dataexpect[8];
	int i;

	for (i = 0; i < sizeof(data); i++)
		data[i] = dataexpect[i] = 0x55;
	if (hifn_writeramaddr(sc, 0, data))
		return (-1);
	if (hifn_readramaddr(sc, 0, data))
		return (-1);
	if (bcmp(data, dataexpect, sizeof(data)) != 0) {
		sc->sc_drammodel = 1;
		return (0);
	}

	for (i = 0; i < sizeof(data); i++)
		data[i] = dataexpect[i] = 0xaa;
	if (hifn_writeramaddr(sc, 0, data))
		return (-1);
	if (hifn_readramaddr(sc, 0, data))
		return (-1);
	if (bcmp(data, dataexpect, sizeof(data)) != 0) {
		sc->sc_drammodel = 1;
		return (0);
	}

	return (0);
}

#define	HIFN_SRAM_MAX		(32 << 20)
#define	HIFN_SRAM_STEP_SIZE	16384
#define	HIFN_SRAM_GRANULARITY	(HIFN_SRAM_MAX / HIFN_SRAM_STEP_SIZE)

static int
hifn_sramsize(struct hifn_softc *sc)
{
	u_int32_t a;
	u_int8_t data[8];
	u_int8_t dataexpect[sizeof(data)];
	int32_t i;

	for (i = 0; i < sizeof(data); i++)
		data[i] = dataexpect[i] = i ^ 0x5a;

	for (i = HIFN_SRAM_GRANULARITY - 1; i >= 0; i--) {
		a = i * HIFN_SRAM_STEP_SIZE;
		bcopy(&i, data, sizeof(i));
		hifn_writeramaddr(sc, a, data);
	}

	for (i = 0; i < HIFN_SRAM_GRANULARITY; i++) {
		a = i * HIFN_SRAM_STEP_SIZE;
		bcopy(&i, dataexpect, sizeof(i));
		if (hifn_readramaddr(sc, a, data) < 0)
			return (0);
		if (bcmp(data, dataexpect, sizeof(data)) != 0)
			return (0);
		sc->sc_ramsize = a + HIFN_SRAM_STEP_SIZE;
	}

	return (0);
}

/*
 * XXX For dram boards, one should really try all of the
 * HIFN_PUCNFG_DSZ_*'s.  This just assumes that PUCNFG
 * is already set up correctly.
 */
static int
hifn_dramsize(struct hifn_softc *sc)
{
	u_int32_t cnfg;

	if (sc->sc_flags & HIFN_IS_7956) {
		/*
		 * 7955/7956 have a fixed internal ram of only 32K.
		 */
		sc->sc_ramsize = 32768;
	} else {
		cnfg = READ_REG_0(sc, HIFN_0_PUCNFG) &
		    HIFN_PUCNFG_DRAMMASK;
		sc->sc_ramsize = 1 << ((cnfg >> 13) + 18);
	}
	return (0);
}

static void
hifn_alloc_slot(struct hifn_softc *sc, int *cmdp, int *srcp, int *dstp, int *resp)
{
	struct hifn_dma *dma = sc->sc_dma;

	DPRINTF("%s()\n", __FUNCTION__);

	if (dma->cmdi == HIFN_D_CMD_RSIZE) {
		dma->cmdi = 0;
		dma->cmdr[HIFN_D_CMD_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->cmdr[HIFN_D_CMD_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_CMDR_SYNC(sc, HIFN_D_CMD_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	}
	*cmdp = dma->cmdi++;
	dma->cmdk = dma->cmdi;

	if (dma->srci == HIFN_D_SRC_RSIZE) {
		dma->srci = 0;
		dma->srcr[HIFN_D_SRC_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->srcr[HIFN_D_SRC_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_SRCR_SYNC(sc, HIFN_D_SRC_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	}
	*srcp = dma->srci++;
	dma->srck = dma->srci;

	if (dma->dsti == HIFN_D_DST_RSIZE) {
		dma->dsti = 0;
		dma->dstr[HIFN_D_DST_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->dstr[HIFN_D_DST_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_DSTR_SYNC(sc, HIFN_D_DST_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	}
	*dstp = dma->dsti++;
	dma->dstk = dma->dsti;

	if (dma->resi == HIFN_D_RES_RSIZE) {
		dma->resi = 0;
		dma->resr[HIFN_D_RES_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->resr[HIFN_D_RES_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_RESR_SYNC(sc, HIFN_D_RES_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	}
	*resp = dma->resi++;
	dma->resk = dma->resi;
}

static int
hifn_writeramaddr(struct hifn_softc *sc, int addr, u_int8_t *data)
{
	struct hifn_dma *dma = sc->sc_dma;
	hifn_base_command_t wc;
	const u_int32_t masks = HIFN_D_VALID | HIFN_D_LAST | HIFN_D_MASKDONEIRQ;
	int r, cmdi, resi, srci, dsti;

	DPRINTF("%s()\n", __FUNCTION__);

	wc.masks = htole16(3 << 13);
	wc.session_num = htole16(addr >> 14);
	wc.total_source_count = htole16(8);
	wc.total_dest_count = htole16(addr & 0x3fff);

	hifn_alloc_slot(sc, &cmdi, &srci, &dsti, &resi);

	WRITE_REG_1(sc, HIFN_1_DMA_CSR,
	    HIFN_DMACSR_C_CTRL_ENA | HIFN_DMACSR_S_CTRL_ENA |
	    HIFN_DMACSR_D_CTRL_ENA | HIFN_DMACSR_R_CTRL_ENA);

	/* build write command */
	bzero(dma->command_bufs[cmdi], HIFN_MAX_COMMAND);
	*(hifn_base_command_t *)dma->command_bufs[cmdi] = wc;
	bcopy(data, &dma->test_src, sizeof(dma->test_src));

	dma->srcr[srci].p = htole32(sc->sc_dma_physaddr
	    + offsetof(struct hifn_dma, test_src));
	dma->dstr[dsti].p = htole32(sc->sc_dma_physaddr
	    + offsetof(struct hifn_dma, test_dst));

	dma->cmdr[cmdi].l = htole32(16 | masks);
	dma->srcr[srci].l = htole32(8 | masks);
	dma->dstr[dsti].l = htole32(4 | masks);
	dma->resr[resi].l = htole32(4 | masks);

	for (r = 10000; r >= 0; r--) {
		DELAY(10);
		if ((dma->resr[resi].l & htole32(HIFN_D_VALID)) == 0)
			break;
	}
	if (r == 0) {
		device_printf(sc->sc_dev, "writeramaddr -- "
		    "result[%d](addr %d) still valid\n", resi, addr);
		r = -1;
		return (-1);
	} else
		r = 0;

	WRITE_REG_1(sc, HIFN_1_DMA_CSR,
	    HIFN_DMACSR_C_CTRL_DIS | HIFN_DMACSR_S_CTRL_DIS |
	    HIFN_DMACSR_D_CTRL_DIS | HIFN_DMACSR_R_CTRL_DIS);

	return (r);
}

static int
hifn_readramaddr(struct hifn_softc *sc, int addr, u_int8_t *data)
{
	struct hifn_dma *dma = sc->sc_dma;
	hifn_base_command_t rc;
	const u_int32_t masks = HIFN_D_VALID | HIFN_D_LAST | HIFN_D_MASKDONEIRQ;
	int r, cmdi, srci, dsti, resi;

	DPRINTF("%s()\n", __FUNCTION__);

	rc.masks = htole16(2 << 13);
	rc.session_num = htole16(addr >> 14);
	rc.total_source_count = htole16(addr & 0x3fff);
	rc.total_dest_count = htole16(8);

	hifn_alloc_slot(sc, &cmdi, &srci, &dsti, &resi);

	WRITE_REG_1(sc, HIFN_1_DMA_CSR,
	    HIFN_DMACSR_C_CTRL_ENA | HIFN_DMACSR_S_CTRL_ENA |
	    HIFN_DMACSR_D_CTRL_ENA | HIFN_DMACSR_R_CTRL_ENA);

	bzero(dma->command_bufs[cmdi], HIFN_MAX_COMMAND);
	*(hifn_base_command_t *)dma->command_bufs[cmdi] = rc;

	dma->srcr[srci].p = htole32(sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, test_src));
	dma->test_src = 0;
	dma->dstr[dsti].p =  htole32(sc->sc_dma_physaddr +
	    offsetof(struct hifn_dma, test_dst));
	dma->test_dst = 0;
	dma->cmdr[cmdi].l = htole32(8 | masks);
	dma->srcr[srci].l = htole32(8 | masks);
	dma->dstr[dsti].l = htole32(8 | masks);
	dma->resr[resi].l = htole32(HIFN_MAX_RESULT | masks);

	for (r = 10000; r >= 0; r--) {
		DELAY(10);
		if ((dma->resr[resi].l & htole32(HIFN_D_VALID)) == 0)
			break;
	}
	if (r == 0) {
		device_printf(sc->sc_dev, "readramaddr -- "
		    "result[%d](addr %d) still valid\n", resi, addr);
		r = -1;
	} else {
		r = 0;
		bcopy(&dma->test_dst, data, sizeof(dma->test_dst));
	}

	WRITE_REG_1(sc, HIFN_1_DMA_CSR,
	    HIFN_DMACSR_C_CTRL_DIS | HIFN_DMACSR_S_CTRL_DIS |
	    HIFN_DMACSR_D_CTRL_DIS | HIFN_DMACSR_R_CTRL_DIS);

	return (r);
}

/*
 * Initialize the descriptor rings.
 */
static void 
hifn_init_dma(struct hifn_softc *sc)
{
	struct hifn_dma *dma = sc->sc_dma;
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	hifn_set_retry(sc);

	/* initialize static pointer values */
	for (i = 0; i < HIFN_D_CMD_RSIZE; i++)
		dma->cmdr[i].p = htole32(sc->sc_dma_physaddr +
		    offsetof(struct hifn_dma, command_bufs[i][0]));
	for (i = 0; i < HIFN_D_RES_RSIZE; i++)
		dma->resr[i].p = htole32(sc->sc_dma_physaddr +
		    offsetof(struct hifn_dma, result_bufs[i][0]));

	dma->cmdr[HIFN_D_CMD_RSIZE].p =
	    htole32(sc->sc_dma_physaddr + offsetof(struct hifn_dma, cmdr[0]));
	dma->srcr[HIFN_D_SRC_RSIZE].p =
	    htole32(sc->sc_dma_physaddr + offsetof(struct hifn_dma, srcr[0]));
	dma->dstr[HIFN_D_DST_RSIZE].p =
	    htole32(sc->sc_dma_physaddr + offsetof(struct hifn_dma, dstr[0]));
	dma->resr[HIFN_D_RES_RSIZE].p =
	    htole32(sc->sc_dma_physaddr + offsetof(struct hifn_dma, resr[0]));

	dma->cmdu = dma->srcu = dma->dstu = dma->resu = 0;
	dma->cmdi = dma->srci = dma->dsti = dma->resi = 0;
	dma->cmdk = dma->srck = dma->dstk = dma->resk = 0;
}

/*
 * Writes out the raw command buffer space.  Returns the
 * command buffer size.
 */
static u_int
hifn_write_command(struct hifn_command *cmd, u_int8_t *buf)
{
	struct hifn_softc *sc = NULL;
	u_int8_t *buf_pos;
	hifn_base_command_t *base_cmd;
	hifn_mac_command_t *mac_cmd;
	hifn_crypt_command_t *cry_cmd;
	int using_mac, using_crypt, len, ivlen;
	u_int32_t dlen, slen;

	DPRINTF("%s()\n", __FUNCTION__);

	buf_pos = buf;
	using_mac = cmd->base_masks & HIFN_BASE_CMD_MAC;
	using_crypt = cmd->base_masks & HIFN_BASE_CMD_CRYPT;

	base_cmd = (hifn_base_command_t *)buf_pos;
	base_cmd->masks = htole16(cmd->base_masks);
	slen = cmd->src_mapsize;
	if (cmd->sloplen)
		dlen = cmd->dst_mapsize - cmd->sloplen + sizeof(u_int32_t);
	else
		dlen = cmd->dst_mapsize;
	base_cmd->total_source_count = htole16(slen & HIFN_BASE_CMD_LENMASK_LO);
	base_cmd->total_dest_count = htole16(dlen & HIFN_BASE_CMD_LENMASK_LO);
	dlen >>= 16;
	slen >>= 16;
	base_cmd->session_num = htole16(
	    ((slen << HIFN_BASE_CMD_SRCLEN_S) & HIFN_BASE_CMD_SRCLEN_M) |
	    ((dlen << HIFN_BASE_CMD_DSTLEN_S) & HIFN_BASE_CMD_DSTLEN_M));
	buf_pos += sizeof(hifn_base_command_t);

	if (using_mac) {
		mac_cmd = (hifn_mac_command_t *)buf_pos;
		dlen = cmd->maccrd->crd_len;
		mac_cmd->source_count = htole16(dlen & 0xffff);
		dlen >>= 16;
		mac_cmd->masks = htole16(cmd->mac_masks |
		    ((dlen << HIFN_MAC_CMD_SRCLEN_S) & HIFN_MAC_CMD_SRCLEN_M));
		mac_cmd->header_skip = htole16(cmd->maccrd->crd_skip);
		mac_cmd->reserved = 0;
		buf_pos += sizeof(hifn_mac_command_t);
	}

	if (using_crypt) {
		cry_cmd = (hifn_crypt_command_t *)buf_pos;
		dlen = cmd->enccrd->crd_len;
		cry_cmd->source_count = htole16(dlen & 0xffff);
		dlen >>= 16;
		cry_cmd->masks = htole16(cmd->cry_masks |
		    ((dlen << HIFN_CRYPT_CMD_SRCLEN_S) & HIFN_CRYPT_CMD_SRCLEN_M));
		cry_cmd->header_skip = htole16(cmd->enccrd->crd_skip);
		cry_cmd->reserved = 0;
		buf_pos += sizeof(hifn_crypt_command_t);
	}

	if (using_mac && cmd->mac_masks & HIFN_MAC_CMD_NEW_KEY) {
		bcopy(cmd->mac, buf_pos, HIFN_MAC_KEY_LENGTH);
		buf_pos += HIFN_MAC_KEY_LENGTH;
	}

	if (using_crypt && cmd->cry_masks & HIFN_CRYPT_CMD_NEW_KEY) {
		switch (cmd->cry_masks & HIFN_CRYPT_CMD_ALG_MASK) {
		case HIFN_CRYPT_CMD_ALG_3DES:
			bcopy(cmd->ck, buf_pos, HIFN_3DES_KEY_LENGTH);
			buf_pos += HIFN_3DES_KEY_LENGTH;
			break;
		case HIFN_CRYPT_CMD_ALG_DES:
			bcopy(cmd->ck, buf_pos, HIFN_DES_KEY_LENGTH);
			buf_pos += HIFN_DES_KEY_LENGTH;
			break;
		case HIFN_CRYPT_CMD_ALG_RC4:
			len = 256;
			do {
				int clen;

				clen = MIN(cmd->cklen, len);
				bcopy(cmd->ck, buf_pos, clen);
				len -= clen;
				buf_pos += clen;
			} while (len > 0);
			bzero(buf_pos, 4);
			buf_pos += 4;
			break;
		case HIFN_CRYPT_CMD_ALG_AES:
			/*
			 * AES keys are variable 128, 192 and
			 * 256 bits (16, 24 and 32 bytes).
			 */
			bcopy(cmd->ck, buf_pos, cmd->cklen);
			buf_pos += cmd->cklen;
			break;
		}
	}

	if (using_crypt && cmd->cry_masks & HIFN_CRYPT_CMD_NEW_IV) {
		switch (cmd->cry_masks & HIFN_CRYPT_CMD_ALG_MASK) {
		case HIFN_CRYPT_CMD_ALG_AES:
			ivlen = HIFN_AES_IV_LENGTH;
			break;
		default:
			ivlen = HIFN_IV_LENGTH;
			break;
		}
		bcopy(cmd->iv, buf_pos, ivlen);
		buf_pos += ivlen;
	}

	if ((cmd->base_masks & (HIFN_BASE_CMD_MAC|HIFN_BASE_CMD_CRYPT)) == 0) {
		bzero(buf_pos, 8);
		buf_pos += 8;
	}

	return (buf_pos - buf);
}

static int
hifn_dmamap_aligned(struct hifn_operand *op)
{
	struct hifn_softc *sc = NULL;
	int i;

	DPRINTF("%s()\n", __FUNCTION__);

	for (i = 0; i < op->nsegs; i++) {
		if (op->segs[i].ds_addr & 3)
			return (0);
		if ((i != (op->nsegs - 1)) && (op->segs[i].ds_len & 3))
			return (0);
	}
	return (1);
}

static __inline int
hifn_dmamap_dstwrap(struct hifn_softc *sc, int idx)
{
	struct hifn_dma *dma = sc->sc_dma;

	if (++idx == HIFN_D_DST_RSIZE) {
		dma->dstr[idx].l = htole32(HIFN_D_VALID | HIFN_D_JUMP |
		    HIFN_D_MASKDONEIRQ);
		HIFN_DSTR_SYNC(sc, idx,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
		idx = 0;
	}
	return (idx);
}

static int
hifn_dmamap_load_dst(struct hifn_softc *sc, struct hifn_command *cmd)
{
	struct hifn_dma *dma = sc->sc_dma;
	struct hifn_operand *dst = &cmd->dst;
	u_int32_t p, l;
	int idx, used = 0, i;

	DPRINTF("%s()\n", __FUNCTION__);

	idx = dma->dsti;
	for (i = 0; i < dst->nsegs - 1; i++) {
		dma->dstr[idx].p = htole32(dst->segs[i].ds_addr);
		dma->dstr[idx].l = htole32(HIFN_D_MASKDONEIRQ | dst->segs[i].ds_len);
		wmb();
		dma->dstr[idx].l |= htole32(HIFN_D_VALID);
		HIFN_DSTR_SYNC(sc, idx,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
		used++;

		idx = hifn_dmamap_dstwrap(sc, idx);
	}

	if (cmd->sloplen == 0) {
		p = dst->segs[i].ds_addr;
		l = HIFN_D_MASKDONEIRQ | HIFN_D_LAST |
		    dst->segs[i].ds_len;
	} else {
		p = sc->sc_dma_physaddr +
		    offsetof(struct hifn_dma, slop[cmd->slopidx]);
		l = HIFN_D_MASKDONEIRQ | HIFN_D_LAST |
		    sizeof(u_int32_t);

		if ((dst->segs[i].ds_len - cmd->sloplen) != 0) {
			dma->dstr[idx].p = htole32(dst->segs[i].ds_addr);
			dma->dstr[idx].l = htole32(HIFN_D_MASKDONEIRQ |
			    (dst->segs[i].ds_len - cmd->sloplen));
			wmb();
			dma->dstr[idx].l |= htole32(HIFN_D_VALID);
			HIFN_DSTR_SYNC(sc, idx,
			    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
			used++;

			idx = hifn_dmamap_dstwrap(sc, idx);
		}
	}
	dma->dstr[idx].p = htole32(p);
	dma->dstr[idx].l = htole32(l);
	wmb();
	dma->dstr[idx].l |= htole32(HIFN_D_VALID);
	HIFN_DSTR_SYNC(sc, idx, BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
	used++;

	idx = hifn_dmamap_dstwrap(sc, idx);

	dma->dsti = idx;
	dma->dstu += used;
	return (idx);
}

static __inline int
hifn_dmamap_srcwrap(struct hifn_softc *sc, int idx)
{
	struct hifn_dma *dma = sc->sc_dma;

	if (++idx == HIFN_D_SRC_RSIZE) {
		dma->srcr[idx].l = htole32(HIFN_D_VALID |
		    HIFN_D_JUMP | HIFN_D_MASKDONEIRQ);
		HIFN_SRCR_SYNC(sc, HIFN_D_SRC_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
		idx = 0;
	}
	return (idx);
}

static int
hifn_dmamap_load_src(struct hifn_softc *sc, struct hifn_command *cmd)
{
	struct hifn_dma *dma = sc->sc_dma;
	struct hifn_operand *src = &cmd->src;
	int idx, i;
	u_int32_t last = 0;

	DPRINTF("%s()\n", __FUNCTION__);

	idx = dma->srci;
	for (i = 0; i < src->nsegs; i++) {
		if (i == src->nsegs - 1)
			last = HIFN_D_LAST;

		dma->srcr[idx].p = htole32(src->segs[i].ds_addr);
		dma->srcr[idx].l = htole32(src->segs[i].ds_len |
		    HIFN_D_MASKDONEIRQ | last);
		wmb();
		dma->srcr[idx].l |= htole32(HIFN_D_VALID);
		HIFN_SRCR_SYNC(sc, idx,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);

		idx = hifn_dmamap_srcwrap(sc, idx);
	}
	dma->srci = idx;
	dma->srcu += src->nsegs;
	return (idx);
} 


static int 
hifn_crypto(
	struct hifn_softc *sc,
	struct hifn_command *cmd,
	struct cryptop *crp,
	int hint)
{
	struct	hifn_dma *dma = sc->sc_dma;
	u_int32_t cmdlen, csr;
	int cmdi, resi, err = 0;
	unsigned long l_flags;

	DPRINTF("%s()\n", __FUNCTION__);

	/*
	 * need 1 cmd, and 1 res
	 *
	 * NB: check this first since it's easy.
	 */
	HIFN_LOCK(sc);
	if ((dma->cmdu + 1) > HIFN_D_CMD_RSIZE ||
	    (dma->resu + 1) > HIFN_D_RES_RSIZE) {
#ifdef HIFN_DEBUG
		if (hifn_debug) {
			device_printf(sc->sc_dev,
				"cmd/result exhaustion, cmdu %u resu %u\n",
				dma->cmdu, dma->resu);
		}
#endif
		hifnstats.hst_nomem_cr++;
		sc->sc_needwakeup |= CRYPTO_SYMQ;
		HIFN_UNLOCK(sc);
		return (ERESTART);
	}

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		if (pci_map_skb(sc, &cmd->src, cmd->src_skb)) {
			hifnstats.hst_nomem_load++;
			err = ENOMEM;
			goto err_srcmap1;
		}
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		if (pci_map_uio(sc, &cmd->src, cmd->src_io)) {
			hifnstats.hst_nomem_load++;
			err = ENOMEM;
			goto err_srcmap1;
		}
	} else {
		if (pci_map_buf(sc, &cmd->src, cmd->src_buf, crp->crp_ilen)) {
			hifnstats.hst_nomem_load++;
			err = ENOMEM;
			goto err_srcmap1;
		}
	}

	if (hifn_dmamap_aligned(&cmd->src)) {
		cmd->sloplen = cmd->src_mapsize & 3;
		cmd->dst = cmd->src;
	} else {
		if (crp->crp_flags & CRYPTO_F_IOV) {
			DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
			err = EINVAL;
			goto err_srcmap;
		} else if (crp->crp_flags & CRYPTO_F_SKBUF) {
#ifdef NOTYET
			int totlen, len;
			struct mbuf *m, *m0, *mlast;

			KASSERT(cmd->dst_m == cmd->src_m,
				("hifn_crypto: dst_m initialized improperly"));
			hifnstats.hst_unaligned++;
			/*
			 * Source is not aligned on a longword boundary.
			 * Copy the data to insure alignment.  If we fail
			 * to allocate mbufs or clusters while doing this
			 * we return ERESTART so the operation is requeued
			 * at the crypto later, but only if there are
			 * ops already posted to the hardware; otherwise we
			 * have no guarantee that we'll be re-entered.
			 */
			totlen = cmd->src_mapsize;
			if (cmd->src_m->m_flags & M_PKTHDR) {
				len = MHLEN;
				MGETHDR(m0, M_DONTWAIT, MT_DATA);
				if (m0 && !m_dup_pkthdr(m0, cmd->src_m, M_DONTWAIT)) {
					m_free(m0);
					m0 = NULL;
				}
			} else {
				len = MLEN;
				MGET(m0, M_DONTWAIT, MT_DATA);
			}
			if (m0 == NULL) {
				hifnstats.hst_nomem_mbuf++;
				err = dma->cmdu ? ERESTART : ENOMEM;
				goto err_srcmap;
			}
			if (totlen >= MINCLSIZE) {
				MCLGET(m0, M_DONTWAIT);
				if ((m0->m_flags & M_EXT) == 0) {
					hifnstats.hst_nomem_mcl++;
					err = dma->cmdu ? ERESTART : ENOMEM;
					m_freem(m0);
					goto err_srcmap;
				}
				len = MCLBYTES;
			}
			totlen -= len;
			m0->m_pkthdr.len = m0->m_len = len;
			mlast = m0;

			while (totlen > 0) {
				MGET(m, M_DONTWAIT, MT_DATA);
				if (m == NULL) {
					hifnstats.hst_nomem_mbuf++;
					err = dma->cmdu ? ERESTART : ENOMEM;
					m_freem(m0);
					goto err_srcmap;
				}
				len = MLEN;
				if (totlen >= MINCLSIZE) {
					MCLGET(m, M_DONTWAIT);
					if ((m->m_flags & M_EXT) == 0) {
						hifnstats.hst_nomem_mcl++;
						err = dma->cmdu ? ERESTART : ENOMEM;
						mlast->m_next = m;
						m_freem(m0);
						goto err_srcmap;
					}
					len = MCLBYTES;
				}

				m->m_len = len;
				m0->m_pkthdr.len += len;
				totlen -= len;

				mlast->m_next = m;
				mlast = m;
			}
			cmd->dst_m = m0;
#else
			device_printf(sc->sc_dev,
					"%s,%d: CRYPTO_F_SKBUF unaligned not implemented\n",
					__FILE__, __LINE__);
			err = EINVAL;
			goto err_srcmap;
#endif
		} else {
			device_printf(sc->sc_dev,
					"%s,%d: unaligned contig buffers not implemented\n",
					__FILE__, __LINE__);
			err = EINVAL;
			goto err_srcmap;
		}
	}

	if (cmd->dst_map == NULL) {
		if (crp->crp_flags & CRYPTO_F_SKBUF) {
			if (pci_map_skb(sc, &cmd->dst, cmd->dst_skb)) {
				hifnstats.hst_nomem_map++;
				err = ENOMEM;
				goto err_dstmap1;
			}
		} else if (crp->crp_flags & CRYPTO_F_IOV) {
			if (pci_map_uio(sc, &cmd->dst, cmd->dst_io)) {
				hifnstats.hst_nomem_load++;
				err = ENOMEM;
				goto err_dstmap1;
			}
		} else {
			if (pci_map_buf(sc, &cmd->dst, cmd->dst_buf, crp->crp_ilen)) {
				hifnstats.hst_nomem_load++;
				err = ENOMEM;
				goto err_dstmap1;
			}
		}
	}

#ifdef HIFN_DEBUG
	if (hifn_debug) {
		device_printf(sc->sc_dev,
		    "Entering cmd: stat %8x ien %8x u %d/%d/%d/%d n %d/%d\n",
		    READ_REG_1(sc, HIFN_1_DMA_CSR),
		    READ_REG_1(sc, HIFN_1_DMA_IER),
		    dma->cmdu, dma->srcu, dma->dstu, dma->resu,
		    cmd->src_nsegs, cmd->dst_nsegs);
	}
#endif

#if 0
	if (cmd->src_map == cmd->dst_map) {
		bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
		    BUS_DMASYNC_PREWRITE|BUS_DMASYNC_PREREAD);
	} else {
		bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
		    BUS_DMASYNC_PREWRITE);
		bus_dmamap_sync(sc->sc_dmat, cmd->dst_map,
		    BUS_DMASYNC_PREREAD);
	}
#endif

	/*
	 * need N src, and N dst
	 */
	if ((dma->srcu + cmd->src_nsegs) > HIFN_D_SRC_RSIZE ||
	    (dma->dstu + cmd->dst_nsegs + 1) > HIFN_D_DST_RSIZE) {
#ifdef HIFN_DEBUG
		if (hifn_debug) {
			device_printf(sc->sc_dev,
				"src/dst exhaustion, srcu %u+%u dstu %u+%u\n",
				dma->srcu, cmd->src_nsegs,
				dma->dstu, cmd->dst_nsegs);
		}
#endif
		hifnstats.hst_nomem_sd++;
		err = ERESTART;
		goto err_dstmap;
	}

	if (dma->cmdi == HIFN_D_CMD_RSIZE) {
		dma->cmdi = 0;
		dma->cmdr[HIFN_D_CMD_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->cmdr[HIFN_D_CMD_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_CMDR_SYNC(sc, HIFN_D_CMD_RSIZE,
		    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	}
	cmdi = dma->cmdi++;
	cmdlen = hifn_write_command(cmd, dma->command_bufs[cmdi]);
	HIFN_CMD_SYNC(sc, cmdi, BUS_DMASYNC_PREWRITE);

	/* .p for command/result already set */
	dma->cmdr[cmdi].l = htole32(cmdlen | HIFN_D_LAST |
	    HIFN_D_MASKDONEIRQ);
	wmb();
	dma->cmdr[cmdi].l |= htole32(HIFN_D_VALID);
	HIFN_CMDR_SYNC(sc, cmdi,
	    BUS_DMASYNC_PREWRITE | BUS_DMASYNC_PREREAD);
	dma->cmdu++;

	/*
	 * We don't worry about missing an interrupt (which a "command wait"
	 * interrupt salvages us from), unless there is more than one command
	 * in the queue.
	 */
	if (dma->cmdu > 1) {
		sc->sc_dmaier |= HIFN_DMAIER_C_WAIT;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
	}

	hifnstats.hst_ipackets++;
	hifnstats.hst_ibytes += cmd->src_mapsize;

	hifn_dmamap_load_src(sc, cmd);

	/*
	 * Unlike other descriptors, we don't mask done interrupt from
	 * result descriptor.
	 */
#ifdef HIFN_DEBUG
	if (hifn_debug)
		device_printf(sc->sc_dev, "load res\n");
#endif
	if (dma->resi == HIFN_D_RES_RSIZE) {
		dma->resi = 0;
		dma->resr[HIFN_D_RES_RSIZE].l = htole32(HIFN_D_JUMP|HIFN_D_MASKDONEIRQ);
		wmb();
		dma->resr[HIFN_D_RES_RSIZE].l |= htole32(HIFN_D_VALID);
		HIFN_RESR_SYNC(sc, HIFN_D_RES_RSIZE,
		    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
	}
	resi = dma->resi++;
	KASSERT(dma->hifn_commands[resi] == NULL,
		("hifn_crypto: command slot %u busy", resi));
	dma->hifn_commands[resi] = cmd;
	HIFN_RES_SYNC(sc, resi, BUS_DMASYNC_PREREAD);
	if ((hint & CRYPTO_HINT_MORE) && sc->sc_curbatch < hifn_maxbatch) {
		dma->resr[resi].l = htole32(HIFN_MAX_RESULT |
		    HIFN_D_LAST | HIFN_D_MASKDONEIRQ);
		wmb();
		dma->resr[resi].l |= htole32(HIFN_D_VALID);
		sc->sc_curbatch++;
		if (sc->sc_curbatch > hifnstats.hst_maxbatch)
			hifnstats.hst_maxbatch = sc->sc_curbatch;
		hifnstats.hst_totbatch++;
	} else {
		dma->resr[resi].l = htole32(HIFN_MAX_RESULT | HIFN_D_LAST);
		wmb();
		dma->resr[resi].l |= htole32(HIFN_D_VALID);
		sc->sc_curbatch = 0;
	}
	HIFN_RESR_SYNC(sc, resi,
	    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
	dma->resu++;

	if (cmd->sloplen)
		cmd->slopidx = resi;

	hifn_dmamap_load_dst(sc, cmd);

	csr = 0;
	if (sc->sc_c_busy == 0) {
		csr |= HIFN_DMACSR_C_CTRL_ENA;
		sc->sc_c_busy = 1;
	}
	if (sc->sc_s_busy == 0) {
		csr |= HIFN_DMACSR_S_CTRL_ENA;
		sc->sc_s_busy = 1;
	}
	if (sc->sc_r_busy == 0) {
		csr |= HIFN_DMACSR_R_CTRL_ENA;
		sc->sc_r_busy = 1;
	}
	if (sc->sc_d_busy == 0) {
		csr |= HIFN_DMACSR_D_CTRL_ENA;
		sc->sc_d_busy = 1;
	}
	if (csr)
		WRITE_REG_1(sc, HIFN_1_DMA_CSR, csr);

#ifdef HIFN_DEBUG
	if (hifn_debug) {
		device_printf(sc->sc_dev, "command: stat %8x ier %8x\n",
		    READ_REG_1(sc, HIFN_1_DMA_CSR),
		    READ_REG_1(sc, HIFN_1_DMA_IER));
	}
#endif

	sc->sc_active = 5;
	HIFN_UNLOCK(sc);
	KASSERT(err == 0, ("hifn_crypto: success with error %u", err));
	return (err);		/* success */

err_dstmap:
	if (cmd->src_map != cmd->dst_map)
		pci_unmap_buf(sc, &cmd->dst);
err_dstmap1:
err_srcmap:
	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		if (cmd->src_skb != cmd->dst_skb)
#ifdef NOTYET
			m_freem(cmd->dst_m);
#else
			device_printf(sc->sc_dev,
					"%s,%d: CRYPTO_F_SKBUF src != dst not implemented\n",
					__FILE__, __LINE__);
#endif
	}
	pci_unmap_buf(sc, &cmd->src);
err_srcmap1:
	HIFN_UNLOCK(sc);
	return (err);
}

static void
hifn_tick(unsigned long arg)
{
	struct hifn_softc *sc;
	unsigned long l_flags;

	if (arg >= HIFN_MAX_CHIPS)
		return;
	sc = hifn_chip_idx[arg];
	if (!sc)
		return;

	HIFN_LOCK(sc);
	if (sc->sc_active == 0) {
		struct hifn_dma *dma = sc->sc_dma;
		u_int32_t r = 0;

		if (dma->cmdu == 0 && sc->sc_c_busy) {
			sc->sc_c_busy = 0;
			r |= HIFN_DMACSR_C_CTRL_DIS;
		}
		if (dma->srcu == 0 && sc->sc_s_busy) {
			sc->sc_s_busy = 0;
			r |= HIFN_DMACSR_S_CTRL_DIS;
		}
		if (dma->dstu == 0 && sc->sc_d_busy) {
			sc->sc_d_busy = 0;
			r |= HIFN_DMACSR_D_CTRL_DIS;
		}
		if (dma->resu == 0 && sc->sc_r_busy) {
			sc->sc_r_busy = 0;
			r |= HIFN_DMACSR_R_CTRL_DIS;
		}
		if (r)
			WRITE_REG_1(sc, HIFN_1_DMA_CSR, r);
	} else
		sc->sc_active--;
	HIFN_UNLOCK(sc);
	mod_timer(&sc->sc_tickto, jiffies + HZ);
}

static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
hifn_intr(int irq, void *arg)
#else
hifn_intr(int irq, void *arg, struct pt_regs *regs)
#endif
{
	struct hifn_softc *sc = arg;
	struct hifn_dma *dma;
	u_int32_t dmacsr, restart;
	int i, u;
	unsigned long l_flags;

	dmacsr = READ_REG_1(sc, HIFN_1_DMA_CSR);

	/* Nothing in the DMA unit interrupted */
	if ((dmacsr & sc->sc_dmaier) == 0)
		return IRQ_NONE;

	HIFN_LOCK(sc);

	dma = sc->sc_dma;

#ifdef HIFN_DEBUG
	if (hifn_debug) {
		device_printf(sc->sc_dev,
		    "irq: stat %08x ien %08x damier %08x i %d/%d/%d/%d k %d/%d/%d/%d u %d/%d/%d/%d\n",
		    dmacsr, READ_REG_1(sc, HIFN_1_DMA_IER), sc->sc_dmaier,
		    dma->cmdi, dma->srci, dma->dsti, dma->resi,
		    dma->cmdk, dma->srck, dma->dstk, dma->resk,
		    dma->cmdu, dma->srcu, dma->dstu, dma->resu);
	}
#endif

	WRITE_REG_1(sc, HIFN_1_DMA_CSR, dmacsr & sc->sc_dmaier);

	if ((sc->sc_flags & HIFN_HAS_PUBLIC) &&
	    (dmacsr & HIFN_DMACSR_PUBDONE))
		WRITE_REG_1(sc, HIFN_1_PUB_STATUS,
		    READ_REG_1(sc, HIFN_1_PUB_STATUS) | HIFN_PUBSTS_DONE);

	restart = dmacsr & (HIFN_DMACSR_D_OVER | HIFN_DMACSR_R_OVER);
	if (restart)
		device_printf(sc->sc_dev, "overrun %x\n", dmacsr);

	if (sc->sc_flags & HIFN_IS_7811) {
		if (dmacsr & HIFN_DMACSR_ILLR)
			device_printf(sc->sc_dev, "illegal read\n");
		if (dmacsr & HIFN_DMACSR_ILLW)
			device_printf(sc->sc_dev, "illegal write\n");
	}

	restart = dmacsr & (HIFN_DMACSR_C_ABORT | HIFN_DMACSR_S_ABORT |
	    HIFN_DMACSR_D_ABORT | HIFN_DMACSR_R_ABORT);
	if (restart) {
		device_printf(sc->sc_dev, "abort, resetting.\n");
		hifnstats.hst_abort++;
		hifn_abort(sc);
		HIFN_UNLOCK(sc);
		return IRQ_HANDLED;
	}

	if ((dmacsr & HIFN_DMACSR_C_WAIT) && (dma->cmdu == 0)) {
		/*
		 * If no slots to process and we receive a "waiting on
		 * command" interrupt, we disable the "waiting on command"
		 * (by clearing it).
		 */
		sc->sc_dmaier &= ~HIFN_DMAIER_C_WAIT;
		WRITE_REG_1(sc, HIFN_1_DMA_IER, sc->sc_dmaier);
	}

	/* clear the rings */
	i = dma->resk; u = dma->resu;
	while (u != 0) {
		HIFN_RESR_SYNC(sc, i,
		    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
		if (dma->resr[i].l & htole32(HIFN_D_VALID)) {
			HIFN_RESR_SYNC(sc, i,
			    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
			break;
		}

		if (i != HIFN_D_RES_RSIZE) {
			struct hifn_command *cmd;
			u_int8_t *macbuf = NULL;

			HIFN_RES_SYNC(sc, i, BUS_DMASYNC_POSTREAD);
			cmd = dma->hifn_commands[i];
			KASSERT(cmd != NULL,
				("hifn_intr: null command slot %u", i));
			dma->hifn_commands[i] = NULL;

			if (cmd->base_masks & HIFN_BASE_CMD_MAC) {
				macbuf = dma->result_bufs[i];
				macbuf += 12;
			}

			hifn_callback(sc, cmd, macbuf);
			hifnstats.hst_opackets++;
			u--;
		}

		if (++i == (HIFN_D_RES_RSIZE + 1))
			i = 0;
	}
	dma->resk = i; dma->resu = u;

	i = dma->srck; u = dma->srcu;
	while (u != 0) {
		if (i == HIFN_D_SRC_RSIZE)
			i = 0;
		HIFN_SRCR_SYNC(sc, i,
		    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
		if (dma->srcr[i].l & htole32(HIFN_D_VALID)) {
			HIFN_SRCR_SYNC(sc, i,
			    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
			break;
		}
		i++, u--;
	}
	dma->srck = i; dma->srcu = u;

	i = dma->cmdk; u = dma->cmdu;
	while (u != 0) {
		HIFN_CMDR_SYNC(sc, i,
		    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
		if (dma->cmdr[i].l & htole32(HIFN_D_VALID)) {
			HIFN_CMDR_SYNC(sc, i,
			    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
			break;
		}
		if (i != HIFN_D_CMD_RSIZE) {
			u--;
			HIFN_CMD_SYNC(sc, i, BUS_DMASYNC_POSTWRITE);
		}
		if (++i == (HIFN_D_CMD_RSIZE + 1))
			i = 0;
	}
	dma->cmdk = i; dma->cmdu = u;

	HIFN_UNLOCK(sc);

	if (sc->sc_needwakeup) {		/* XXX check high watermark */
		int wakeup = sc->sc_needwakeup & (CRYPTO_SYMQ|CRYPTO_ASYMQ);
#ifdef HIFN_DEBUG
		if (hifn_debug)
			device_printf(sc->sc_dev,
				"wakeup crypto (%x) u %d/%d/%d/%d\n",
				sc->sc_needwakeup,
				dma->cmdu, dma->srcu, dma->dstu, dma->resu);
#endif
		sc->sc_needwakeup &= ~wakeup;
		crypto_unblock(sc->sc_cid, wakeup);
	}

	return IRQ_HANDLED;
}

/*
 * Allocate a new 'session' and return an encoded session id.  'sidp'
 * contains our registration id, and should contain an encoded session
 * id on successful allocation.
 */
static int
hifn_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
	struct hifn_softc *sc = device_get_softc(dev);
	struct cryptoini *c;
	int mac = 0, cry = 0, sesn;
	struct hifn_session *ses = NULL;
	unsigned long l_flags;

	DPRINTF("%s()\n", __FUNCTION__);

	KASSERT(sc != NULL, ("hifn_newsession: null softc"));
	if (sidp == NULL || cri == NULL || sc == NULL) {
		DPRINTF("%s,%d: %s - EINVAL\n", __FILE__, __LINE__, __FUNCTION__);
		return (EINVAL);
	}

	HIFN_LOCK(sc);
	if (sc->sc_sessions == NULL) {
		ses = sc->sc_sessions = (struct hifn_session *)kmalloc(sizeof(*ses),
				SLAB_ATOMIC);
		if (ses == NULL) {
			HIFN_UNLOCK(sc);
			return (ENOMEM);
		}
		sesn = 0;
		sc->sc_nsessions = 1;
	} else {
		for (sesn = 0; sesn < sc->sc_nsessions; sesn++) {
			if (!sc->sc_sessions[sesn].hs_used) {
				ses = &sc->sc_sessions[sesn];
				break;
			}
		}

		if (ses == NULL) {
			sesn = sc->sc_nsessions;
			ses = (struct hifn_session *)kmalloc((sesn + 1) * sizeof(*ses),
					SLAB_ATOMIC);
			if (ses == NULL) {
				HIFN_UNLOCK(sc);
				return (ENOMEM);
			}
			bcopy(sc->sc_sessions, ses, sesn * sizeof(*ses));
			bzero(sc->sc_sessions, sesn * sizeof(*ses));
			kfree(sc->sc_sessions);
			sc->sc_sessions = ses;
			ses = &sc->sc_sessions[sesn];
			sc->sc_nsessions++;
		}
	}
	HIFN_UNLOCK(sc);

	bzero(ses, sizeof(*ses));
	ses->hs_used = 1;

	for (c = cri; c != NULL; c = c->cri_next) {
		switch (c->cri_alg) {
		case CRYPTO_MD5:
		case CRYPTO_SHA1:
		case CRYPTO_MD5_HMAC:
		case CRYPTO_SHA1_HMAC:
			if (mac) {
				DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
				return (EINVAL);
			}
			mac = 1;
			ses->hs_mlen = c->cri_mlen;
			if (ses->hs_mlen == 0) {
				switch (c->cri_alg) {
				case CRYPTO_MD5:
				case CRYPTO_MD5_HMAC:
					ses->hs_mlen = 16;
					break;
				case CRYPTO_SHA1:
				case CRYPTO_SHA1_HMAC:
					ses->hs_mlen = 20;
					break;
				}
			}
			break;
		case CRYPTO_DES_CBC:
		case CRYPTO_3DES_CBC:
		case CRYPTO_AES_CBC:
			/* XXX this may read fewer, does it matter? */
			read_random(ses->hs_iv,
				c->cri_alg == CRYPTO_AES_CBC ?
					HIFN_AES_IV_LENGTH : HIFN_IV_LENGTH);
			/*FALLTHROUGH*/
		case CRYPTO_ARC4:
			if (cry) {
				DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
				return (EINVAL);
			}
			cry = 1;
			break;
		default:
			DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
			return (EINVAL);
		}
	}
	if (mac == 0 && cry == 0) {
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		return (EINVAL);
	}

	*sidp = HIFN_SID(device_get_unit(sc->sc_dev), sesn);

	return (0);
}

/*
 * Deallocate a session.
 * XXX this routine should run a zero'd mac/encrypt key into context ram.
 * XXX to blow away any keys already stored there.
 */
static int
hifn_freesession(device_t dev, u_int64_t tid)
{
	struct hifn_softc *sc = device_get_softc(dev);
	int session, error;
	u_int32_t sid = CRYPTO_SESID2LID(tid);
	unsigned long l_flags;

	DPRINTF("%s()\n", __FUNCTION__);

	KASSERT(sc != NULL, ("hifn_freesession: null softc"));
	if (sc == NULL) {
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		return (EINVAL);
	}

	HIFN_LOCK(sc);
	session = HIFN_SESSION(sid);
	if (session < sc->sc_nsessions) {
		bzero(&sc->sc_sessions[session], sizeof(struct hifn_session));
		error = 0;
	} else {
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		error = EINVAL;
	}
	HIFN_UNLOCK(sc);

	return (error);
}

static int
hifn_process(device_t dev, struct cryptop *crp, int hint)
{
	struct hifn_softc *sc = device_get_softc(dev);
	struct hifn_command *cmd = NULL;
	int session, err, ivlen;
	struct cryptodesc *crd1, *crd2, *maccrd, *enccrd;

	DPRINTF("%s()\n", __FUNCTION__);

	if (crp == NULL || crp->crp_callback == NULL) {
		hifnstats.hst_invalid++;
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		return (EINVAL);
	}
	session = HIFN_SESSION(crp->crp_sid);

	if (sc == NULL || session >= sc->sc_nsessions) {
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		err = EINVAL;
		goto errout;
	}

	cmd = kmalloc(sizeof(struct hifn_command), SLAB_ATOMIC);
	if (cmd == NULL) {
		hifnstats.hst_nomem++;
		err = ENOMEM;
		goto errout;
	}
	memset(cmd, 0, sizeof(*cmd));

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		cmd->src_skb = (struct sk_buff *)crp->crp_buf;
		cmd->dst_skb = (struct sk_buff *)crp->crp_buf;
	} else if (crp->crp_flags & CRYPTO_F_IOV) {
		cmd->src_io = (struct uio *)crp->crp_buf;
		cmd->dst_io = (struct uio *)crp->crp_buf;
	} else {
		cmd->src_buf = crp->crp_buf;
		cmd->dst_buf = crp->crp_buf;
	}

	crd1 = crp->crp_desc;
	if (crd1 == NULL) {
		DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
		err = EINVAL;
		goto errout;
	}
	crd2 = crd1->crd_next;

	if (crd2 == NULL) {
		if (crd1->crd_alg == CRYPTO_MD5_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1_HMAC ||
		    crd1->crd_alg == CRYPTO_SHA1 ||
		    crd1->crd_alg == CRYPTO_MD5) {
			maccrd = crd1;
			enccrd = NULL;
		} else if (crd1->crd_alg == CRYPTO_DES_CBC ||
		    crd1->crd_alg == CRYPTO_3DES_CBC ||
		    crd1->crd_alg == CRYPTO_AES_CBC ||
		    crd1->crd_alg == CRYPTO_ARC4) {
			if ((crd1->crd_flags & CRD_F_ENCRYPT) == 0)
				cmd->base_masks |= HIFN_BASE_CMD_DECODE;
			maccrd = NULL;
			enccrd = crd1;
		} else {
			DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
			err = EINVAL;
			goto errout;
		}
	} else {
		if ((crd1->crd_alg == CRYPTO_MD5_HMAC ||
                     crd1->crd_alg == CRYPTO_SHA1_HMAC ||
                     crd1->crd_alg == CRYPTO_MD5 ||
                     crd1->crd_alg == CRYPTO_SHA1) &&
		    (crd2->crd_alg == CRYPTO_DES_CBC ||
		     crd2->crd_alg == CRYPTO_3DES_CBC ||
		     crd2->crd_alg == CRYPTO_AES_CBC ||
		     crd2->crd_alg == CRYPTO_ARC4) &&
		    ((crd2->crd_flags & CRD_F_ENCRYPT) == 0)) {
			cmd->base_masks = HIFN_BASE_CMD_DECODE;
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
			/*
			 * We cannot order the 7751 as requested
			 */
			DPRINTF("%s,%d: %s %d,%d,%d - EINVAL\n",__FILE__,__LINE__,__FUNCTION__, crd1->crd_alg, crd2->crd_alg, crd1->crd_flags & CRD_F_ENCRYPT);
			err = EINVAL;
			goto errout;
		}
	}

	if (enccrd) {
		cmd->enccrd = enccrd;
		cmd->base_masks |= HIFN_BASE_CMD_CRYPT;
		switch (enccrd->crd_alg) {
		case CRYPTO_ARC4:
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_RC4;
			break;
		case CRYPTO_DES_CBC:
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_DES |
			    HIFN_CRYPT_CMD_MODE_CBC |
			    HIFN_CRYPT_CMD_NEW_IV;
			break;
		case CRYPTO_3DES_CBC:
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_3DES |
			    HIFN_CRYPT_CMD_MODE_CBC |
			    HIFN_CRYPT_CMD_NEW_IV;
			break;
		case CRYPTO_AES_CBC:
			cmd->cry_masks |= HIFN_CRYPT_CMD_ALG_AES |
			    HIFN_CRYPT_CMD_MODE_CBC |
			    HIFN_CRYPT_CMD_NEW_IV;
			break;
		default:
			DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
			err = EINVAL;
			goto errout;
		}
		if (enccrd->crd_alg != CRYPTO_ARC4) {
			ivlen = ((enccrd->crd_alg == CRYPTO_AES_CBC) ?
				HIFN_AES_IV_LENGTH : HIFN_IV_LENGTH);
			if (enccrd->crd_flags & CRD_F_ENCRYPT) {
				if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
					bcopy(enccrd->crd_iv, cmd->iv, ivlen);
				else
					bcopy(sc->sc_sessions[session].hs_iv,
					    cmd->iv, ivlen);

				if ((enccrd->crd_flags & CRD_F_IV_PRESENT)
				    == 0) {
					crypto_copyback(crp->crp_flags,
					    crp->crp_buf, enccrd->crd_inject,
					    ivlen, cmd->iv);
				}
			} else {
				if (enccrd->crd_flags & CRD_F_IV_EXPLICIT)
					bcopy(enccrd->crd_iv, cmd->iv, ivlen);
				else {
					crypto_copydata(crp->crp_flags,
					    crp->crp_buf, enccrd->crd_inject,
					    ivlen, cmd->iv);
				}
			}
		}

		if (enccrd->crd_flags & CRD_F_KEY_EXPLICIT)
			cmd->cry_masks |= HIFN_CRYPT_CMD_NEW_KEY;
		cmd->ck = enccrd->crd_key;
		cmd->cklen = enccrd->crd_klen >> 3;
		cmd->cry_masks |= HIFN_CRYPT_CMD_NEW_KEY;

		/* 
		 * Need to specify the size for the AES key in the masks.
		 */
		if ((cmd->cry_masks & HIFN_CRYPT_CMD_ALG_MASK) ==
		    HIFN_CRYPT_CMD_ALG_AES) {
			switch (cmd->cklen) {
			case 16:
				cmd->cry_masks |= HIFN_CRYPT_CMD_KSZ_128;
				break;
			case 24:
				cmd->cry_masks |= HIFN_CRYPT_CMD_KSZ_192;
				break;
			case 32:
				cmd->cry_masks |= HIFN_CRYPT_CMD_KSZ_256;
				break;
			default:
				DPRINTF("%s,%d: %s - EINVAL\n",__FILE__,__LINE__,__FUNCTION__);
				err = EINVAL;
				goto errout;
			}
		}
	}

	if (maccrd) {
		cmd->maccrd = maccrd;
		cmd->base_masks |= HIFN_BASE_CMD_MAC;

		switch (maccrd->crd_alg) {
		case CRYPTO_MD5:
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_MD5 |
			    HIFN_MAC_CMD_RESULT | HIFN_MAC_CMD_MODE_HASH |
			    HIFN_MAC_CMD_POS_IPSEC;
                       break;
		case CRYPTO_MD5_HMAC:
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_MD5 |
			    HIFN_MAC_CMD_RESULT | HIFN_MAC_CMD_MODE_HMAC |
			    HIFN_MAC_CMD_POS_IPSEC | HIFN_MAC_CMD_TRUNC;
			break;
		case CRYPTO_SHA1:
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_SHA1 |
			    HIFN_MAC_CMD_RESULT | HIFN_MAC_CMD_MODE_HASH |
			    HIFN_MAC_CMD_POS_IPSEC;
			break;
		case CRYPTO_SHA1_HMAC:
			cmd->mac_masks |= HIFN_MAC_CMD_ALG_SHA1 |
			    HIFN_MAC_CMD_RESULT | HIFN_MAC_CMD_MODE_HMAC |
			    HIFN_MAC_CMD_POS_IPSEC | HIFN_MAC_CMD_TRUNC;
			break;
		}

		if (maccrd->crd_alg == CRYPTO_SHA1_HMAC ||
		     maccrd->crd_alg == CRYPTO_MD5_HMAC) {
			cmd->mac_masks |= HIFN_MAC_CMD_NEW_KEY;
			bcopy(maccrd->crd_key, cmd->mac, maccrd->crd_klen >> 3);
			bzero(cmd->mac + (maccrd->crd_klen >> 3),
			    HIFN_MAC_KEY_LENGTH - (maccrd->crd_klen >> 3));
		}
	}

	cmd->crp = crp;
	cmd->session_num = session;
	cmd->softc = sc;

	err = hifn_crypto(sc, cmd, crp, hint);
	if (!err) {
		return 0;
	} else if (err == ERESTART) {
		/*
		 * There weren't enough resources to dispatch the request
		 * to the part.  Notify the caller so they'll requeue this
		 * request and resubmit it again soon.
		 */
#ifdef HIFN_DEBUG
		if (hifn_debug)
			device_printf(sc->sc_dev, "requeue request\n");
#endif
		kfree(cmd);
		sc->sc_needwakeup |= CRYPTO_SYMQ;
		return (err);
	}

errout:
	if (cmd != NULL)
		kfree(cmd);
	if (err == EINVAL)
		hifnstats.hst_invalid++;
	else
		hifnstats.hst_nomem++;
	crp->crp_etype = err;
	crypto_done(crp);
	return (err);
}

static void
hifn_abort(struct hifn_softc *sc)
{
	struct hifn_dma *dma = sc->sc_dma;
	struct hifn_command *cmd;
	struct cryptop *crp;
	int i, u;

	DPRINTF("%s()\n", __FUNCTION__);

	i = dma->resk; u = dma->resu;
	while (u != 0) {
		cmd = dma->hifn_commands[i];
		KASSERT(cmd != NULL, ("hifn_abort: null command slot %u", i));
		dma->hifn_commands[i] = NULL;
		crp = cmd->crp;

		if ((dma->resr[i].l & htole32(HIFN_D_VALID)) == 0) {
			/* Salvage what we can. */
			u_int8_t *macbuf;

			if (cmd->base_masks & HIFN_BASE_CMD_MAC) {
				macbuf = dma->result_bufs[i];
				macbuf += 12;
			} else
				macbuf = NULL;
			hifnstats.hst_opackets++;
			hifn_callback(sc, cmd, macbuf);
		} else {
#if 0
			if (cmd->src_map == cmd->dst_map) {
				bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
				    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
			} else {
				bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
				    BUS_DMASYNC_POSTWRITE);
				bus_dmamap_sync(sc->sc_dmat, cmd->dst_map,
				    BUS_DMASYNC_POSTREAD);
			}
#endif

			if (cmd->src_skb != cmd->dst_skb) {
#ifdef NOTYET
				m_freem(cmd->src_m);
				crp->crp_buf = (caddr_t)cmd->dst_m;
#else
				device_printf(sc->sc_dev,
						"%s,%d: CRYPTO_F_SKBUF src != dst not implemented\n",
						__FILE__, __LINE__);
#endif
			}

			/* non-shared buffers cannot be restarted */
			if (cmd->src_map != cmd->dst_map) {
				/*
				 * XXX should be EAGAIN, delayed until
				 * after the reset.
				 */
				crp->crp_etype = ENOMEM;
				pci_unmap_buf(sc, &cmd->dst);
			} else
				crp->crp_etype = ENOMEM;

			pci_unmap_buf(sc, &cmd->src);

			kfree(cmd);
			if (crp->crp_etype != EAGAIN)
				crypto_done(crp);
		}

		if (++i == HIFN_D_RES_RSIZE)
			i = 0;
		u--;
	}
	dma->resk = i; dma->resu = u;

	hifn_reset_board(sc, 1);
	hifn_init_dma(sc);
	hifn_init_pci_registers(sc);
}

static void
hifn_callback(struct hifn_softc *sc, struct hifn_command *cmd, u_int8_t *macbuf)
{
	struct hifn_dma *dma = sc->sc_dma;
	struct cryptop *crp = cmd->crp;
	struct cryptodesc *crd;
	int i, u, ivlen;

	DPRINTF("%s()\n", __FUNCTION__);

#if 0
	if (cmd->src_map == cmd->dst_map) {
		bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
		    BUS_DMASYNC_POSTWRITE | BUS_DMASYNC_POSTREAD);
	} else {
		bus_dmamap_sync(sc->sc_dmat, cmd->src_map,
		    BUS_DMASYNC_POSTWRITE);
		bus_dmamap_sync(sc->sc_dmat, cmd->dst_map,
		    BUS_DMASYNC_POSTREAD);
	}
#endif

	if (crp->crp_flags & CRYPTO_F_SKBUF) {
		if (cmd->src_skb != cmd->dst_skb) {
#ifdef NOTYET
			crp->crp_buf = (caddr_t)cmd->dst_m;
			totlen = cmd->src_mapsize;
			for (m = cmd->dst_m; m != NULL; m = m->m_next) {
				if (totlen < m->m_len) {
					m->m_len = totlen;
					totlen = 0;
				} else
					totlen -= m->m_len;
			}
			cmd->dst_m->m_pkthdr.len = cmd->src_m->m_pkthdr.len;
			m_freem(cmd->src_m);
#else
			device_printf(sc->sc_dev,
					"%s,%d: CRYPTO_F_SKBUF src != dst not implemented\n",
					__FILE__, __LINE__);
#endif
		}
	}

	if (cmd->sloplen != 0) {
		crypto_copyback(crp->crp_flags, crp->crp_buf,
		    cmd->src_mapsize - cmd->sloplen, cmd->sloplen,
		    (caddr_t)&dma->slop[cmd->slopidx]);
	}

	i = dma->dstk; u = dma->dstu;
	while (u != 0) {
		if (i == HIFN_D_DST_RSIZE)
			i = 0;
#if 0
		bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
		    BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
#endif
		if (dma->dstr[i].l & htole32(HIFN_D_VALID)) {
#if 0
			bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap,
			    BUS_DMASYNC_PREREAD | BUS_DMASYNC_PREWRITE);
#endif
			break;
		}
		i++, u--;
	}
	dma->dstk = i; dma->dstu = u;

	hifnstats.hst_obytes += cmd->dst_mapsize;

	if ((cmd->base_masks & (HIFN_BASE_CMD_CRYPT | HIFN_BASE_CMD_DECODE)) ==
	    HIFN_BASE_CMD_CRYPT) {
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
			if (crd->crd_alg != CRYPTO_DES_CBC &&
			    crd->crd_alg != CRYPTO_3DES_CBC &&
			    crd->crd_alg != CRYPTO_AES_CBC)
				continue;
			ivlen = ((crd->crd_alg == CRYPTO_AES_CBC) ?
				HIFN_AES_IV_LENGTH : HIFN_IV_LENGTH);
			crypto_copydata(crp->crp_flags, crp->crp_buf,
			    crd->crd_skip + crd->crd_len - ivlen, ivlen,
			    cmd->softc->sc_sessions[cmd->session_num].hs_iv);
			break;
		}
	}

	if (macbuf != NULL) {
		for (crd = crp->crp_desc; crd; crd = crd->crd_next) {
                        int len;

			if (crd->crd_alg != CRYPTO_MD5 &&
			    crd->crd_alg != CRYPTO_SHA1 &&
			    crd->crd_alg != CRYPTO_MD5_HMAC &&
			    crd->crd_alg != CRYPTO_SHA1_HMAC) {
				continue;
			}
			len = cmd->softc->sc_sessions[cmd->session_num].hs_mlen;
			crypto_copyback(crp->crp_flags, crp->crp_buf,
			    crd->crd_inject, len, macbuf);
			break;
		}
	}

	if (cmd->src_map != cmd->dst_map)
		pci_unmap_buf(sc, &cmd->dst);
	pci_unmap_buf(sc, &cmd->src);
	kfree(cmd);
	crypto_done(crp);
}

/*
 * 7811 PB3 rev/2 parts lock-up on burst writes to Group 0
 * and Group 1 registers; avoid conditions that could create
 * burst writes by doing a read in between the writes.
 *
 * NB: The read we interpose is always to the same register;
 *     we do this because reading from an arbitrary (e.g. last)
 *     register may not always work.
 */
static void
hifn_write_reg_0(struct hifn_softc *sc, bus_size_t reg, u_int32_t val)
{
	if (sc->sc_flags & HIFN_IS_7811) {
		if (sc->sc_bar0_lastreg == reg - 4)
			readl(sc->sc_bar0 + HIFN_0_PUCNFG);
		sc->sc_bar0_lastreg = reg;
	}
	writel(val, sc->sc_bar0 + reg);
}

static void
hifn_write_reg_1(struct hifn_softc *sc, bus_size_t reg, u_int32_t val)
{
	if (sc->sc_flags & HIFN_IS_7811) {
		if (sc->sc_bar1_lastreg == reg - 4)
			readl(sc->sc_bar1 + HIFN_1_REVID);
		sc->sc_bar1_lastreg = reg;
	}
	writel(val, sc->sc_bar1 + reg);
}


static struct pci_device_id hifn_pci_tbl[] = {
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7951,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7955,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7956,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_NETSEC, PCI_PRODUCT_NETSEC_7751,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_INVERTEX, PCI_PRODUCT_INVERTEX_AEON,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7811,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	/*
	 * Other vendors share this PCI ID as well, such as
	 * http://www.powercrypt.com, and obviously they also
	 * use the same key.
	 */
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7751,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ 0, 0, 0, 0, 0, 0, }
};
MODULE_DEVICE_TABLE(pci, hifn_pci_tbl);

static struct pci_driver hifn_driver = {
	.name         = "hifn",
	.id_table     = hifn_pci_tbl,
	.probe        =	hifn_probe,
	.remove       = hifn_remove,
	/* add PM stuff here one day */
};

static int __init hifn_init (void)
{
	struct hifn_softc *sc = NULL;
	int rc;

	DPRINTF("%s(%p)\n", __FUNCTION__, hifn_init);

	rc = pci_register_driver(&hifn_driver);
	pci_register_driver_compat(&hifn_driver, rc);

	return rc;
}

static void __exit hifn_exit (void)
{
	pci_unregister_driver(&hifn_driver);
}

module_init(hifn_init);
module_exit(hifn_exit);

MODULE_LICENSE("BSD");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("OCF driver for hifn PCI crypto devices");
