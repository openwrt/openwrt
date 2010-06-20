/*-
 * Driver for Hifn HIPP-I/II chipset
 * Copyright (c) 2006 Michael Richardson <mcr@xelerance.com>
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
 * Effort sponsored by Hifn Inc.
 *
 */

/*
 * Driver for various Hifn encryption processors.
 */
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
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
#include <linux/uio.h>
#include <linux/sysfs.h>
#include <linux/miscdevice.h>
#include <asm/io.h>

#include <cryptodev.h>

#include "hifnHIPPreg.h"
#include "hifnHIPPvar.h"

#if 1
#define	DPRINTF(a...)	if (hipp_debug) { \
							printk("%s: ", sc ? \
								device_get_nameunit(sc->sc_dev) : "hifn"); \
							printk(a); \
						} else
#else
#define	DPRINTF(a...)
#endif

typedef int bus_size_t;

static inline int
pci_get_revid(struct pci_dev *dev)
{
	u8 rid = 0;
	pci_read_config_byte(dev, PCI_REVISION_ID, &rid);
	return rid;
}

#define debug hipp_debug
int hipp_debug = 0;
module_param(hipp_debug, int, 0644);
MODULE_PARM_DESC(hipp_debug, "Enable debug");

int hipp_maxbatch = 1;
module_param(hipp_maxbatch, int, 0644);
MODULE_PARM_DESC(hipp_maxbatch, "max ops to batch w/o interrupt");

static	int  hipp_probe(struct pci_dev *dev, const struct pci_device_id *ent);
static	void hipp_remove(struct pci_dev *dev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
static irqreturn_t hipp_intr(int irq, void *arg);
#else
static irqreturn_t hipp_intr(int irq, void *arg, struct pt_regs *regs);
#endif

static int hipp_num_chips = 0;
static struct hipp_softc *hipp_chip_idx[HIPP_MAX_CHIPS];

static	int hipp_newsession(device_t, u_int32_t *, struct cryptoini *);
static	int hipp_freesession(device_t, u_int64_t);
static	int hipp_process(device_t, struct cryptop *, int);

static device_method_t hipp_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	hipp_newsession),
	DEVMETHOD(cryptodev_freesession,hipp_freesession),
	DEVMETHOD(cryptodev_process,	hipp_process),
};

static __inline u_int32_t
READ_REG(struct hipp_softc *sc, unsigned int barno, bus_size_t reg)
{
	u_int32_t v = readl(sc->sc_bar[barno] + reg);
	//sc->sc_bar0_lastreg = (bus_size_t) -1;
	return (v);
}
static __inline void
WRITE_REG(struct hipp_softc *sc, unsigned int barno, bus_size_t reg, u_int32_t val)
{
	writel(val, sc->sc_bar[barno] + reg);
}

#define READ_REG_0(sc, reg)         READ_REG(sc, 0, reg)
#define WRITE_REG_0(sc, reg, val)   WRITE_REG(sc,0, reg, val)
#define READ_REG_1(sc, reg)         READ_REG(sc, 1, reg)
#define WRITE_REG_1(sc, reg, val)   WRITE_REG(sc,1, reg, val)

static int
hipp_newsession(device_t dev, u_int32_t *sidp, struct cryptoini *cri)
{
	return EINVAL;
}

static int
hipp_freesession(device_t dev, u_int64_t tid)
{
	return EINVAL;
}

static int
hipp_process(device_t dev, struct cryptop *crp, int hint)
{
	return EINVAL;
}

static const char*
hipp_partname(struct hipp_softc *sc, char buf[128], size_t blen)
{
	char *n = NULL;

	switch (pci_get_vendor(sc->sc_pcidev)) {
	case PCI_VENDOR_HIFN:
		switch (pci_get_device(sc->sc_pcidev)) {
		case PCI_PRODUCT_HIFN_7855:	n = "Hifn 7855";
		case PCI_PRODUCT_HIFN_8155:	n = "Hifn 8155";
		case PCI_PRODUCT_HIFN_6500:	n = "Hifn 6500";
		}
	}

	if(n==NULL) {
		snprintf(buf, blen, "VID=%02x,PID=%02x",
			 pci_get_vendor(sc->sc_pcidev),
			 pci_get_device(sc->sc_pcidev));
	} else {
		buf[0]='\0';
		strncat(buf, n, blen);
	}
	return buf;
}

struct hipp_fs_entry {
	struct attribute attr;
	/* other stuff */
};


static ssize_t
cryptoid_show(struct device *dev,
	      struct device_attribute *attr,
	      char *buf)						
{								
	struct hipp_softc *sc;					

	sc = pci_get_drvdata(to_pci_dev (dev));
	return sprintf (buf, "%d\n", sc->sc_cid);
}

struct device_attribute hipp_dev_cryptoid = __ATTR_RO(cryptoid);

/*
 * Attach an interface that successfully probed.
 */
static int
hipp_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	struct hipp_softc *sc = NULL;
	int i;
	//char rbase;
	//u_int16_t ena;
	int rev;
	//int rseg;
	int rc;

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

	sc = (struct hipp_softc *) kmalloc(sizeof(*sc), GFP_KERNEL);
	if (!sc)
		return(-ENOMEM);
	memset(sc, 0, sizeof(*sc));

	softc_device_init(sc, "hifn-hipp", hipp_num_chips, hipp_methods);

	sc->sc_pcidev = dev;
	sc->sc_irq = -1;
	sc->sc_cid = -1;
	sc->sc_num = hipp_num_chips++;

	if (sc->sc_num < HIPP_MAX_CHIPS)
		hipp_chip_idx[sc->sc_num] = sc;

	pci_set_drvdata(sc->sc_pcidev, sc);

	spin_lock_init(&sc->sc_mtx);

	/*
	 * Setup PCI resources.
	 * The READ_REG_0, WRITE_REG_0, READ_REG_1,
	 * and WRITE_REG_1 macros throughout the driver are used
	 * to permit better debugging.
	 */
	for(i=0; i<4; i++) {
		unsigned long mem_start, mem_len;
		mem_start = pci_resource_start(sc->sc_pcidev, i);
		mem_len   = pci_resource_len(sc->sc_pcidev, i);
		sc->sc_barphy[i] = (caddr_t)mem_start;
		sc->sc_bar[i] = (ocf_iomem_t) ioremap(mem_start, mem_len);
		if (!sc->sc_bar[i]) {
			device_printf(sc->sc_dev, "cannot map bar%d register space\n", i);
			goto fail;
		}
	}

	//hipp_reset_board(sc, 0);
	pci_set_master(sc->sc_pcidev);

	/*
	 * Arrange the interrupt line.
	 */
	rc = request_irq(dev->irq, hipp_intr, IRQF_SHARED, "hifn", sc);
	if (rc) {
		device_printf(sc->sc_dev, "could not map interrupt: %d\n", rc);
		goto fail;
	}
	sc->sc_irq = dev->irq;

	rev = READ_REG_1(sc, HIPP_1_REVID) & 0xffff;

	{
		char b[32];
		device_printf(sc->sc_dev, "%s, rev %u",
			      hipp_partname(sc, b, sizeof(b)), rev);
	}

#if 0
	if (sc->sc_flags & HIFN_IS_7956)
		printf(", pll=0x%x<%s clk, %ux mult>",
			sc->sc_pllconfig,
			sc->sc_pllconfig & HIFN_PLL_REF_SEL ? "ext" : "pci",
			2 + 2*((sc->sc_pllconfig & HIFN_PLL_ND) >> 11));
#endif
	printf("\n");

	sc->sc_cid = crypto_get_driverid(softc_get_device(sc),CRYPTOCAP_F_HARDWARE);
	if (sc->sc_cid < 0) {
		device_printf(sc->sc_dev, "could not get crypto driver id\n");
		goto fail;
	}

#if 0 /* cannot work with a non-GPL module */
	/* make a sysfs entry to let the world know what entry we got */
	sysfs_create_file(&sc->sc_pcidev->dev.kobj, &hipp_dev_cryptoid.attr);
#endif

#if 0
	init_timer(&sc->sc_tickto);
	sc->sc_tickto.function = hifn_tick;
	sc->sc_tickto.data = (unsigned long) sc->sc_num;
	mod_timer(&sc->sc_tickto, jiffies + HZ);
#endif

#if 0 /* no code here yet ?? */
	crypto_register(sc->sc_cid, CRYPTO_3DES_CBC, 0, 0);
#endif

	return (0);

fail:
	if (sc->sc_cid >= 0)
		crypto_unregister_all(sc->sc_cid);
	if (sc->sc_irq != -1)
		free_irq(sc->sc_irq, sc);
	
#if 0
	if (sc->sc_dma) {
		/* Turn off DMA polling */
		WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
			    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);
		
		pci_free_consistent(sc->sc_pcidev,
				    sizeof(*sc->sc_dma),
				    sc->sc_dma, sc->sc_dma_physaddr);
	}
#endif
	kfree(sc);
	return (-ENXIO);
}

/*
 * Detach an interface that successfully probed.
 */
static void
hipp_remove(struct pci_dev *dev)
{
	struct hipp_softc *sc = pci_get_drvdata(dev);
	unsigned long l_flags;

	DPRINTF("%s()\n", __FUNCTION__);

	/* disable interrupts */
	HIPP_LOCK(sc);

#if 0
	WRITE_REG_1(sc, HIFN_1_DMA_IER, 0);
	HIFN_UNLOCK(sc);

	/*XXX other resources */
	del_timer_sync(&sc->sc_tickto);

	/* Turn off DMA polling */
	WRITE_REG_1(sc, HIFN_1_DMA_CNFG, HIFN_DMACNFG_MSTRESET |
	    HIFN_DMACNFG_DMARESET | HIFN_DMACNFG_MODE);
#endif

	crypto_unregister_all(sc->sc_cid);

	free_irq(sc->sc_irq, sc);

#if 0
	pci_free_consistent(sc->sc_pcidev, sizeof(*sc->sc_dma),
                sc->sc_dma, sc->sc_dma_physaddr);
#endif
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
static irqreturn_t hipp_intr(int irq, void *arg)
#else
static irqreturn_t hipp_intr(int irq, void *arg, struct pt_regs *regs)
#endif
{
	struct hipp_softc *sc = arg;

	sc = sc; /* shut up compiler */

	return IRQ_HANDLED;
}

static struct pci_device_id hipp_pci_tbl[] = {
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_7855,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ PCI_VENDOR_HIFN, PCI_PRODUCT_HIFN_8155,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, hipp_pci_tbl);

static struct pci_driver hipp_driver = {
	.name         = "hipp",
	.id_table     = hipp_pci_tbl,
	.probe        =	hipp_probe,
	.remove       = hipp_remove,
	/* add PM stuff here one day */
};

static int __init hipp_init (void)
{
	struct hipp_softc *sc = NULL;
	int rc;

	DPRINTF("%s(%p)\n", __FUNCTION__, hipp_init);

	rc = pci_register_driver(&hipp_driver);
	pci_register_driver_compat(&hipp_driver, rc);

	return rc;
}

static void __exit hipp_exit (void)
{
	pci_unregister_driver(&hipp_driver);
}

module_init(hipp_init);
module_exit(hipp_exit);

MODULE_LICENSE("BSD");
MODULE_AUTHOR("Michael Richardson <mcr@xelerance.com>");
MODULE_DESCRIPTION("OCF driver for hifn HIPP-I/II PCI crypto devices");
