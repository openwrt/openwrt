/*
 * BCM4710 specific pcmcia routines.
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/config.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/tqueue.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/pci.h>

#include <pcmcia/version.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/ss.h>
#include <pcmcia/bulkmem.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/bus_ops.h>
#include "cs_internal.h"

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>


#include <typedefs.h>
#include <bcmdevs.h>
#include <bcm4710.h>
#include <sbconfig.h>
#include <sbextif.h>

#include "bcm4710pcmcia.h"

/* Use a static var for irq dev_id */
static int bcm47xx_pcmcia_dev_id;

/* Do we think we have a card or not? */
static int bcm47xx_pcmcia_present = 0;


static void bcm4710_pcmcia_reset(void)
{
	extifregs_t *eir;
	unsigned long s;
	uint32 out0, out1, outen;


	eir = (extifregs_t *) ioremap_nocache(BCM4710_REG_EXTIF, sizeof(extifregs_t));

	save_and_cli(s);

	/* Use gpio7 to reset the pcmcia slot */
	outen = readl(&eir->gpio[0].outen);
	outen |= BCM47XX_PCMCIA_RESET;
	out0 = readl(&eir->gpio[0].out);
	out0 &= ~(BCM47XX_PCMCIA_RESET);
	out1 = out0 | BCM47XX_PCMCIA_RESET;

	writel(out0, &eir->gpio[0].out);
	writel(outen, &eir->gpio[0].outen);
	mdelay(1);
	writel(out1, &eir->gpio[0].out);
	mdelay(1);
	writel(out0, &eir->gpio[0].out);

	restore_flags(s);
}


static int bcm4710_pcmcia_init(struct pcmcia_init *init)
{
	struct pci_dev *pdev;
	extifregs_t *eir;
	uint32 outen, intp, intm, tmp;
	uint16 *attrsp;
	int rc = 0, i;
	extern unsigned long bcm4710_cpu_cycle;


	if (!(pdev = pci_find_device(VENDOR_BROADCOM, SB_EXTIF, NULL))) {
		printk(KERN_ERR "bcm4710_pcmcia: extif not found\n");
		return -ENODEV;
	}
	eir = (extifregs_t *) ioremap_nocache(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));

	/* Initialize the pcmcia i/f: 16bit no swap */
	writel(CF_EM_PCMCIA | CF_DS | CF_EN, &eir->pcmcia_config);

#ifdef	notYet

	/* Set the timing for memory accesses */
	tmp = (19 / bcm4710_cpu_cycle) << 24;		/* W3 = 10nS */
	tmp = tmp | ((29 / bcm4710_cpu_cycle) << 16);	/* W2 = 20nS */
	tmp = tmp | ((109 / bcm4710_cpu_cycle) << 8);	/* W1 = 100nS */
	tmp = tmp | (129 / bcm4710_cpu_cycle);		/* W0 = 120nS */
	writel(tmp, &eir->pcmcia_memwait);		/* 0x01020a0c for a 100Mhz clock */

	/* Set the timing for I/O accesses */
	tmp = (19 / bcm4710_cpu_cycle) << 24;		/* W3 = 10nS */
	tmp = tmp | ((29 / bcm4710_cpu_cycle) << 16);	/* W2 = 20nS */
	tmp = tmp | ((109 / bcm4710_cpu_cycle) << 8);	/* W1 = 100nS */
	tmp = tmp | (129 / bcm4710_cpu_cycle);		/* W0 = 120nS */
	writel(tmp, &eir->pcmcia_iowait);		/* 0x01020a0c for a 100Mhz clock */

	/* Set the timing for attribute accesses */
	tmp = (19 / bcm4710_cpu_cycle) << 24;		/* W3 = 10nS */
	tmp = tmp | ((29 / bcm4710_cpu_cycle) << 16);	/* W2 = 20nS */
	tmp = tmp | ((109 / bcm4710_cpu_cycle) << 8);	/* W1 = 100nS */
	tmp = tmp | (129 / bcm4710_cpu_cycle);		/* W0 = 120nS */
	writel(tmp, &eir->pcmcia_attrwait);		/* 0x01020a0c for a 100Mhz clock */

#endif
	/* Make sure gpio0 and gpio5 are inputs */
	outen = readl(&eir->gpio[0].outen);
	outen &= ~(BCM47XX_PCMCIA_WP | BCM47XX_PCMCIA_STSCHG | BCM47XX_PCMCIA_RESET);
	writel(outen, &eir->gpio[0].outen);

	/* Issue a reset to the pcmcia socket */
	bcm4710_pcmcia_reset();

#ifdef	DO_BCM47XX_PCMCIA_INTERRUPTS
	/* Setup gpio5 to be the STSCHG interrupt */
	intp = readl(&eir->gpiointpolarity);
	writel(intp | BCM47XX_PCMCIA_STSCHG, &eir->gpiointpolarity);	/* Active low */
	intm = readl(&eir->gpiointmask);
	writel(intm | BCM47XX_PCMCIA_STSCHG, &eir->gpiointmask);	/* Enable it */
#endif

	DEBUG(2, "bcm4710_pcmcia after reset:\n");
	DEBUG(2, "\textstatus\t= 0x%08x:\n", readl(&eir->extstatus));
	DEBUG(2, "\tpcmcia_config\t= 0x%08x:\n", readl(&eir->pcmcia_config));
	DEBUG(2, "\tpcmcia_memwait\t= 0x%08x:\n", readl(&eir->pcmcia_memwait));
	DEBUG(2, "\tpcmcia_attrwait\t= 0x%08x:\n", readl(&eir->pcmcia_attrwait));
	DEBUG(2, "\tpcmcia_iowait\t= 0x%08x:\n", readl(&eir->pcmcia_iowait));
	DEBUG(2, "\tgpioin\t\t= 0x%08x:\n", readl(&eir->gpioin));
	DEBUG(2, "\tgpio_outen0\t= 0x%08x:\n", readl(&eir->gpio[0].outen));
	DEBUG(2, "\tgpio_out0\t= 0x%08x:\n", readl(&eir->gpio[0].out));
	DEBUG(2, "\tgpiointpolarity\t= 0x%08x:\n", readl(&eir->gpiointpolarity));
	DEBUG(2, "\tgpiointmask\t= 0x%08x:\n", readl(&eir->gpiointmask));

#ifdef	DO_BCM47XX_PCMCIA_INTERRUPTS
	/* Request pcmcia interrupt */
	rc =  request_irq(BCM47XX_PCMCIA_IRQ, init->handler, SA_INTERRUPT,
			  "PCMCIA Interrupt", &bcm47xx_pcmcia_dev_id);
#endif

	attrsp = (uint16 *)ioremap_nocache(EXTIF_PCMCIA_CFGBASE(BCM4710_EXTIF), 0x1000);
	tmp = readw(&attrsp[0]);
	DEBUG(2, "\tattr[0] = 0x%04x\n", tmp);
	if ((tmp == 0x7fff) || (tmp == 0x7f00)) {
		bcm47xx_pcmcia_present = 0;
	} else {
		bcm47xx_pcmcia_present = 1;
	}

	/* There's only one socket */
	return 1;
}

static int bcm4710_pcmcia_shutdown(void)
{
	extifregs_t *eir;
	uint32 intm;

	eir = (extifregs_t *) ioremap_nocache(BCM4710_REG_EXTIF, sizeof(extifregs_t));

	/* Disable the pcmcia i/f */
	writel(0, &eir->pcmcia_config);

	/* Reset gpio's */
	intm = readl(&eir->gpiointmask);
	writel(intm & ~BCM47XX_PCMCIA_STSCHG, &eir->gpiointmask);	/* Disable it */

	free_irq(BCM47XX_PCMCIA_IRQ, &bcm47xx_pcmcia_dev_id);

	return 0;
}

static int 
bcm4710_pcmcia_socket_state(unsigned sock, struct pcmcia_state *state)
{
	extifregs_t *eir;

	eir = (extifregs_t *) ioremap_nocache(BCM4710_REG_EXTIF, sizeof(extifregs_t));


	if (sock != 0) {
		printk(KERN_ERR "bcm4710 socket_state bad sock %d\n", sock);
		return -1;
	}

	if (bcm47xx_pcmcia_present) {
		state->detect = 1;
		state->ready = 1;
		state->bvd1 = 1;
		state->bvd2 = 1;
		state->wrprot = (readl(&eir->gpioin) & BCM47XX_PCMCIA_WP) == BCM47XX_PCMCIA_WP; 
		state->vs_3v = 0;
		state->vs_Xv = 0;
	} else {
		state->detect = 0;
		state->ready = 0;
	}

	return 1;
}


static int bcm4710_pcmcia_get_irq_info(struct pcmcia_irq_info *info)
{
	if (info->sock >= BCM47XX_PCMCIA_MAX_SOCK) return -1;

	info->irq = BCM47XX_PCMCIA_IRQ;		

	return 0;
}


static int 
bcm4710_pcmcia_configure_socket(const struct pcmcia_configure *configure)
{
	if (configure->sock >= BCM47XX_PCMCIA_MAX_SOCK) return -1;


	DEBUG(2, "Vcc %dV Vpp %dV output %d speaker %d reset %d\n", configure->vcc,
	      configure->vpp, configure->output, configure->speaker, configure->reset);

	if ((configure->vcc != 50) || (configure->vpp != 50)) {
		printk("%s: bad Vcc/Vpp (%d:%d)\n", __FUNCTION__, configure->vcc, 
		       configure->vpp);
	}

	if (configure->reset) {
		/* Issue a reset to the pcmcia socket */
		DEBUG(1, "%s: Reseting socket\n", __FUNCTION__);
		bcm4710_pcmcia_reset();
	}


	return 0;
}

struct pcmcia_low_level bcm4710_pcmcia_ops = { 
	bcm4710_pcmcia_init,
	bcm4710_pcmcia_shutdown,
	bcm4710_pcmcia_socket_state,
	bcm4710_pcmcia_get_irq_info,
	bcm4710_pcmcia_configure_socket
};

