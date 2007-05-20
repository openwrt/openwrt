/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     PCI initialization for IDT EB434 board
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/idt-boards/rc32434/rc32434.h>
#include <asm/idt-boards/rc32434/rc32434_pci.h> 

#define PCI_ACCESS_READ  0
#define PCI_ACCESS_WRITE 1

#undef DEBUG
#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif
/* define an unsigned array for the PCI registers */
unsigned int korinaCnfgRegs[25] = {
	KORINA_CNFG1,	 KORINA_CNFG2,  KORINA_CNFG3,  KORINA_CNFG4,
	KORINA_CNFG5,	 KORINA_CNFG6,  KORINA_CNFG7,  KORINA_CNFG8,
	KORINA_CNFG9,	 KORINA_CNFG10, KORINA_CNFG11, KORINA_CNFG12,
	KORINA_CNFG13, KORINA_CNFG14, KORINA_CNFG15, KORINA_CNFG16,
	KORINA_CNFG17, KORINA_CNFG18, KORINA_CNFG19, KORINA_CNFG20,
	KORINA_CNFG21, KORINA_CNFG22, KORINA_CNFG23, KORINA_CNFG24
};

static struct resource rc32434_res_pci_mem2;

static struct resource rc32434_res_pci_mem1 = {
	.name = "PCI MEM1",
	.start = 0x50000000,
	.end = 0x5FFFFFFF,
	.flags = IORESOURCE_MEM,
	.child = &rc32434_res_pci_mem2,
};
static struct resource rc32434_res_pci_mem2 = {
	.name = "PCI MEM2",
	.start = 0x60000000,
	.end = 0x6FFFFFFF,
	.flags = IORESOURCE_MEM,
	.parent = &rc32434_res_pci_mem1,
};
static struct resource rc32434_res_pci_io1 = {
	.name = "PCI I/O1",
	.start = 0x18800000,
	.end = 0x188FFFFF,
	.flags = IORESOURCE_IO,
};

extern struct pci_ops rc32434_pci_ops;

struct pci_controller rc32434_controller = {
	.pci_ops = &rc32434_pci_ops,
	.mem_resource = &rc32434_res_pci_mem1,
	.io_resource = &rc32434_res_pci_io1,
	.mem_offset     = 0x00000000UL,
	.io_offset      = 0x00000000UL,
};

extern unsigned int arch_has_pci;

static int __init rc32434_pcibridge_init(void)
{
	
	unsigned int pciConfigAddr = 0;/*used for writing pci config values */
	int	     loopCount=0    ;/*used for the loop */
	
	unsigned int pcicValue, pcicData=0;
	unsigned int dummyRead, pciCntlVal = 0;

	if (!arch_has_pci) return 0;

	printk("PCI: Initializing PCI\n");
	
	/* Disable the IP bus error for PCI scaning */
	pciCntlVal=rc32434_pci->pcic;
	pciCntlVal &= 0xFFFFFF7;
	rc32434_pci->pcic = pciCntlVal;
	
	ioport_resource.start = rc32434_res_pci_io1.start;
	ioport_resource.end = rc32434_res_pci_io1.end;
/*
	iomem_resource.start = rc32434_res_pci_mem1.start;
	iomem_resource.end = rc32434_res_pci_mem1.end;
*/
	
	pcicValue = rc32434_pci->pcic;
	pcicValue = (pcicValue >> PCIM_SHFT) & PCIM_BIT_LEN;
	if (!((pcicValue == PCIM_H_EA) ||
	      (pcicValue == PCIM_H_IA_FIX) ||
	      (pcicValue == PCIM_H_IA_RR))) {
		/* Not in Host Mode, return ERROR */
		return -1;
	}
	
	/* Enables the Idle Grant mode, Arbiter Parking */
	pcicData |=(PCIC_igm_m|PCIC_eap_m|PCIC_en_m);
	rc32434_pci->pcic = pcicData; /* Enable the PCI bus Interface */
	/* Zero out the PCI status & PCI Status Mask */
	for(;;) {
		pcicData = rc32434_pci->pcis;
		if (!(pcicData & PCIS_rip_m))
			break;
	}
	
	rc32434_pci->pcis = 0;
	rc32434_pci->pcism = 0xFFFFFFFF;
	/* Zero out the PCI decoupled registers */
	rc32434_pci->pcidac=0; /* disable PCI decoupled accesses at initialization */
	rc32434_pci->pcidas=0; /* clear the status */
	rc32434_pci->pcidasm=0x0000007F; /* Mask all the interrupts */
	/* Mask PCI Messaging Interrupts */
	rc32434_pci_msg->pciiic = 0;
	rc32434_pci_msg->pciiim = 0xFFFFFFFF;
	rc32434_pci_msg->pciioic = 0;
	rc32434_pci_msg->pciioim = 0;
	
	/* Setup PCILB0 as Memory Window */
	rc32434_pci->pcilba[0].a = (unsigned int) (PCI_ADDR_START);
	
	/* setup the PCI map address as same as the local address */
	
	rc32434_pci->pcilba[0].m = (unsigned int) (PCI_ADDR_START);
	
	/* Setup PCILBA1 as MEM */
#ifdef __MIPSEB__
	rc32434_pci->pcilba[0].c = ( ((SIZE_16MB & 0x1f) << PCILBAC_size_b) | PCILBAC_sb_m);
#else
	rc32434_pci->pcilba[0].c = ( ((SIZE_16MB & 0x1f) << PCILBAC_size_b));
#endif
	dummyRead = rc32434_pci->pcilba[0].c; /* flush the CPU write Buffers */
	
	rc32434_pci->pcilba[1].a = 0x60000000;
	
	rc32434_pci->pcilba[1].m = 0x60000000;
	/* setup PCILBA2 as IO Window*/
#ifdef __MIPSEB__
	rc32434_pci->pcilba[1].c = ( ((SIZE_256MB & 0x1f) << PCILBAC_size_b) |  PCILBAC_sb_m);
#else
	rc32434_pci->pcilba[1].c = ((SIZE_256MB & 0x1f) << PCILBAC_size_b);
#endif
	dummyRead = rc32434_pci->pcilba[1].c; /* flush the CPU write Buffers */
	rc32434_pci->pcilba[2].a = 0x18C00000;
	
	rc32434_pci->pcilba[2].m = 0x18FFFFFF;
	/* setup PCILBA2 as IO Window*/
#ifdef __MIPSEB__
	rc32434_pci->pcilba[2].c = ( ((SIZE_4MB & 0x1f) << PCILBAC_size_b)  |  PCILBAC_sb_m);
#else
	rc32434_pci->pcilba[2].c = ((SIZE_4MB & 0x1f) << PCILBAC_size_b);
#endif  
	
	dummyRead = rc32434_pci->pcilba[2].c; /* flush the CPU write Buffers */
	
	
	rc32434_pci->pcilba[3].a = 0x18800000;
	
	rc32434_pci->pcilba[3].m = 0x18800000;
	/* Setup PCILBA3 as IO Window */
	
#ifdef __MIPSEB__
	rc32434_pci->pcilba[3].c = ( (((SIZE_1MB & 0x1ff) << PCILBAC_size_b) | PCILBAC_msi_m)   |  PCILBAC_sb_m);
#else
	rc32434_pci->pcilba[3].c = (((SIZE_1MB & 0x1ff) << PCILBAC_size_b) | PCILBAC_msi_m);
#endif
	dummyRead = rc32434_pci->pcilba[2].c; /* flush the CPU write Buffers */
	
	pciConfigAddr = (unsigned int)(0x80000004);
	for(loopCount = 0; loopCount < 24; loopCount++){
		rc32434_pci->pcicfga = pciConfigAddr;
		dummyRead = rc32434_pci->pcicfga;
		rc32434_pci->pcicfgd = korinaCnfgRegs[loopCount];
		dummyRead=rc32434_pci->pcicfgd;
		pciConfigAddr += 4;
	}
	rc32434_pci->pcitc=(unsigned int)((PCITC_RTIMER_VAL&0xff) << PCITC_rtimer_b) |
		((PCITC_DTIMER_VAL&0xff)<<PCITC_dtimer_b);
	
	pciCntlVal = rc32434_pci->pcic;
	pciCntlVal &= ~(PCIC_tnr_m);
	rc32434_pci->pcic = pciCntlVal;
	pciCntlVal = rc32434_pci->pcic;
	
	register_pci_controller(&rc32434_controller);
	
	rc32434_sync();  
	return 0;
}

arch_initcall(rc32434_pcibridge_init);

/* Do platform specific device initialization at pci_enable_device() time */
int pcibios_plat_dev_init(struct pci_dev *dev)
{
        return 0;
}
