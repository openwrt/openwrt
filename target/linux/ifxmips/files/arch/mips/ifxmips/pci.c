#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/addrspace.h>
#include <linux/vmalloc.h>

#define IFXMIPS_PCI_MEM_BASE    0x18000000
#define IFXMIPS_PCI_MEM_SIZE    0x02000000
#define IFXMIPS_PCI_IO_BASE     0x1AE00000
#define IFXMIPS_PCI_IO_SIZE     0x00200000

#define IFXMIPS_PCI_CFG_BUSNUM_SHF 16
#define IFXMIPS_PCI_CFG_DEVNUM_SHF 11
#define IFXMIPS_PCI_CFG_FUNNUM_SHF 8

#define PCI_ACCESS_READ  0
#define PCI_ACCESS_WRITE 1

//#define CONFIG_IFXMIPS_PCI_HW_SWAP 1

static int ifxmips_pci_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val);
static int ifxmips_pci_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val);

struct pci_ops ifxmips_pci_ops = {
	.read = ifxmips_pci_read_config_dword,
	.write = ifxmips_pci_write_config_dword
};

static struct resource pci_io_resource = {
	.name = "io pci IO space",
	.start = IFXMIPS_PCI_IO_BASE,
	.end = IFXMIPS_PCI_IO_BASE + IFXMIPS_PCI_IO_SIZE - 1,
	.flags = IORESOURCE_IO
};

static struct resource pci_mem_resource = {
	.name = "ext pci memory space",
	.start = IFXMIPS_PCI_MEM_BASE,
	.end = IFXMIPS_PCI_MEM_BASE + IFXMIPS_PCI_MEM_SIZE - 1,
	.flags = IORESOURCE_MEM
};

static struct pci_controller ifxmips_pci_controller = {
	.pci_ops = &ifxmips_pci_ops,
	.mem_resource = &pci_mem_resource,
	.mem_offset	= 0x00000000UL,
	.io_resource = &pci_io_resource,
	.io_offset	= 0x00000000UL,
};

static u32 ifxmips_pci_mapped_cfg;

static int
ifxmips_pci_config_access(unsigned char access_type,
		struct pci_bus *bus, unsigned int devfn, unsigned int where, u32 *data)
{
	unsigned long cfg_base;
	unsigned long flags;

	u32 temp;

	/* IFXMips support slot from 0 to 15 */
	/* dev_fn 0&0x68 (AD29) is ifxmips itself */
	if ((bus->number != 0) || ((devfn & 0xf8) > 0x78)
			|| ((devfn & 0xf8) == 0) || ((devfn & 0xf8) == 0x68))
		return 1;

	local_irq_save(flags);

	cfg_base = ifxmips_pci_mapped_cfg;
	cfg_base |= (bus->number << IFXMIPS_PCI_CFG_BUSNUM_SHF) | (devfn <<
			IFXMIPS_PCI_CFG_FUNNUM_SHF) | (where & ~0x3);

	/* Perform access */
	if (access_type == PCI_ACCESS_WRITE)
	{
#ifdef CONFIG_IFXMIPS_PCI_HW_SWAP
		writel(swab32(*data), ((u32*)cfg_base));
#else
		writel(*data, ((u32*)cfg_base));
#endif
	} else {
		*data = readl(((u32*)(cfg_base)));
#ifdef CONFIG_IFXMIPS_PCI_HW_SWAP
		*data = swab32(*data);
#endif
	}
	wmb();

	/* clean possible Master abort */
	cfg_base = (ifxmips_pci_mapped_cfg | (0x0 << IFXMIPS_PCI_CFG_FUNNUM_SHF)) + 4;
	temp = readl(((u32*)(cfg_base)));
#ifdef CONFIG_IFXMIPS_PCI_HW_SWAP
	temp = swab32 (temp);
#endif
	cfg_base = (ifxmips_pci_mapped_cfg | (0x68 << IFXMIPS_PCI_CFG_FUNNUM_SHF)) + 4;
	writel(temp, ((u32*)cfg_base));

	local_irq_restore(flags);

	if (((*data) == 0xffffffff) && (access_type == PCI_ACCESS_READ))
		return 1;

	return 0;
}

static int ifxmips_pci_read_config_dword(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 * val)
{
	u32 data = 0;

	if (ifxmips_pci_config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	if (size == 1)
		*val = (data >> ((where & 3) << 3)) & 0xff;
	else if (size == 2)
		*val = (data >> ((where & 3) << 3)) & 0xffff;
	else
		*val = data;

	return PCIBIOS_SUCCESSFUL;
}

static int ifxmips_pci_write_config_dword(struct pci_bus *bus, unsigned int devfn,
		int where, int size, u32 val)
{
	u32 data = 0;

	if (size == 4)
	{
		data = val;
	} else {
		if (ifxmips_pci_config_access(PCI_ACCESS_READ, bus, devfn, where, &data))
			return PCIBIOS_DEVICE_NOT_FOUND;

		if (size == 1)
			data = (data & ~(0xff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
		else if (size == 2)
			data = (data & ~(0xffff << ((where & 3) << 3))) |
				(val << ((where & 3) << 3));
	}

	if (ifxmips_pci_config_access(PCI_ACCESS_WRITE, bus, devfn, where, &data))
		return PCIBIOS_DEVICE_NOT_FOUND;

	return PCIBIOS_SUCCESSFUL;
}


int pcibios_plat_dev_init(struct pci_dev *dev){
	u8 pin;

	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);

	switch(pin) {
		case 0:
			break;
		case 1:
			//falling edge level triggered:0x4, low level:0xc, rising edge:0x2
			writel(readl(IFXMIPS_EBU_PCC_CON) | 0xc, IFXMIPS_EBU_PCC_CON);
			writel(readl(IFXMIPS_EBU_PCC_IEN) | 0x10, IFXMIPS_EBU_PCC_IEN);
			break;
		case 2:
		case 3:
		case 4:
			printk ("WARNING: interrupt pin %d not supported yet!\n", pin);
		default:
			printk ("WARNING: invalid interrupt pin %d\n", pin);
			return 1;
	}

	return 0;
}

static void __init ifxmips_pci_startup (void){
	/*initialize the first PCI device--ifxmips itself */
	u32 temp_buffer;
	/*TODO: trigger reset */
	writel(readl(IFXMIPS_CGU_IFCCR) & ~0xf00000, IFXMIPS_CGU_IFCCR);
	writel(readl(IFXMIPS_CGU_IFCCR) | 0x800000, IFXMIPS_CGU_IFCCR);
	/* PCIS of IF_CLK of CGU   : 1 =>PCI Clock output
	   0 =>clock input
	   PADsel of PCI_CR of CGU : 1 =>From CGU
	   : 0 =>From pad
	 */
	writel(readl(IFXMIPS_CGU_IFCCR) | (1 << 16), IFXMIPS_CGU_IFCCR);
	writel((1 << 31) | (1 << 30), IFXMIPS_CGU_PCICR);

	/* prepare GPIO */
	/* PCI_RST: P1.5 ALT 01 */
	//pliu20060613: start
	writel(readl(IFXMIPS_GPIO_P1_OUT) | (1 << 5), IFXMIPS_GPIO_P1_OUT);
	writel(readl(IFXMIPS_GPIO_P1_OD) | (1 << 5), IFXMIPS_GPIO_P1_OD);
	writel(readl(IFXMIPS_GPIO_P1_DIR) | (1 << 5), IFXMIPS_GPIO_P1_DIR);
	writel(readl(IFXMIPS_GPIO_P1_ALTSEL1) & ~(1 << 5), IFXMIPS_GPIO_P1_ALTSEL1);
	writel(readl(IFXMIPS_GPIO_P1_ALTSEL0) & ~(1 << 5), IFXMIPS_GPIO_P1_ALTSEL0);
	//pliu20060613: end
	/* PCI_REQ1: P1.13 ALT 01 */
	/* PCI_GNT1: P1.14 ALT 01 */
	writel(readl(IFXMIPS_GPIO_P1_DIR) & ~0x2000, IFXMIPS_GPIO_P1_DIR);
	writel(readl(IFXMIPS_GPIO_P1_DIR) | 0x4000, IFXMIPS_GPIO_P1_DIR);
	writel(readl(IFXMIPS_GPIO_P1_ALTSEL1) & ~0x6000, IFXMIPS_GPIO_P1_ALTSEL1);
	writel(readl(IFXMIPS_GPIO_P1_ALTSEL0) | 0x6000, IFXMIPS_GPIO_P1_ALTSEL0);
	/* PCI_REQ2: P1.15 ALT 10 */
	/* PCI_GNT2: P1.7 ALT 10 */


	/* enable auto-switching between PCI and EBU */
	writel(0xa, PCI_CR_CLK_CTRL);
	/* busy, i.e. configuration is not done, PCI access has to be retried */
	writel(readl(PCI_CR_PCI_MOD) & ~(1 << 24), PCI_CR_PCI_MOD);
	wmb ();
	/* BUS Master/IO/MEM access */
	writel(readl(PCI_CS_STS_CMD) | 7, PCI_CS_STS_CMD);

	temp_buffer = readl(PCI_CR_PC_ARB);
	/* enable external 2 PCI masters */
	temp_buffer &= (~(0xf << 16));
	/* enable internal arbiter */
	temp_buffer |= (1 << INTERNAL_ARB_ENABLE_BIT);
	/* enable internal PCI master reqest */
	temp_buffer &= (~(3 << PCI_MASTER0_REQ_MASK_2BITS));

	/* enable EBU reqest */
	temp_buffer &= (~(3 << PCI_MASTER1_REQ_MASK_2BITS));

	/* enable all external masters request */
	temp_buffer &= (~(3 << PCI_MASTER2_REQ_MASK_2BITS));
	writel(temp_buffer, PCI_CR_PC_ARB);

	wmb ();

	/* FPI ==> PCI MEM address mapping */
	/* base: 0xb8000000 == > 0x18000000 */
	/* size: 8x4M = 32M */
	writel(0x18000000, PCI_CR_FCI_ADDR_MAP0);
	writel(0x18400000, PCI_CR_FCI_ADDR_MAP1);
	writel(0x18800000, PCI_CR_FCI_ADDR_MAP2);
	writel(0x18c00000, PCI_CR_FCI_ADDR_MAP3);
	writel(0x19000000, PCI_CR_FCI_ADDR_MAP4);
	writel(0x19400000, PCI_CR_FCI_ADDR_MAP5);
	writel(0x19800000, PCI_CR_FCI_ADDR_MAP6);
	writel(0x19c00000, PCI_CR_FCI_ADDR_MAP7);

	/* FPI ==> PCI IO address mapping */
	/* base: 0xbAE00000 == > 0xbAE00000 */
	/* size: 2M */
	writel(0x1ae00000, PCI_CR_FCI_ADDR_MAP11hg);

	/* PCI ==> FPI address mapping */
	/* base: 0x0 ==> 0x0 */
	/* size: 32M */
	/* BAR1 32M map to SDR address */
	writel(0x0e000008, PCI_CR_BAR11MASK);
	writel(0, PCI_CR_PCI_ADDR_MAP11);
	writel(0, PCI_CS_BASE_ADDR1);
#ifdef CONFIG_IFXMIPS_PCI_HW_SWAP
	/* both TX and RX endian swap are enabled */
	writel(readl(PCI_CR_PCI_EOI) | 3, PCI_CR_PCI_EOI);
	wmb ();
#endif
	/*TODO: disable BAR2 & BAR3 - why was this in the origianl infineon code */
	writel(readl(PCI_CR_BAR12MASK) | 0x80000000, PCI_CR_BAR12MASK);
	writel(readl(PCI_CR_BAR13MASK) | 0x80000000, PCI_CR_BAR13MASK);
	/*use 8 dw burse length */
	writel(0x303, PCI_CR_FCI_BURST_LENGTH);

	writel(readl(PCI_CR_PCI_MOD) | (1 << 24), PCI_CR_PCI_MOD);
	wmb();
	writel(readl(IFXMIPS_GPIO_P1_OUT) & ~(1 << 5), IFXMIPS_GPIO_P1_OUT);
	wmb();
	mdelay (1);
	writel(readl(IFXMIPS_GPIO_P1_OUT) | (1 << 5), IFXMIPS_GPIO_P1_OUT);
}

int __init pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin){
	switch (slot) {
		case 13:
			/* IDSEL = AD29 --> USB Host Controller */
			return (INT_NUM_IM1_IRL0 + 17);
		case 14:
			/* IDSEL = AD30 --> mini PCI connector */
			//return (INT_NUM_IM1_IRL0 + 14);
			return (INT_NUM_IM0_IRL0 + 22);
		default:
			printk("Warning: no IRQ found for PCI device in slot %d, pin %d\n", slot, pin);
			return 0;
	}
}

int pcibios_init(void){
	extern int pci_probe_only;

	pci_probe_only = 0;
	printk ("PCI: Probing PCI hardware on host bus 0.\n");

	ifxmips_pci_startup ();

	//	IFXMIPS_PCI_REG32(PCI_CR_CLK_CTRL_REG) &= (~8);
	ifxmips_pci_mapped_cfg = ioremap_nocache(0x17000000, 0x800 * 16);
	printk("IFXMips PCI mapped to 0x%08X\n", (unsigned long)ifxmips_pci_mapped_cfg);

	ifxmips_pci_controller.io_map_base = (unsigned long)ioremap(IFXMIPS_PCI_IO_BASE, IFXMIPS_PCI_IO_SIZE - 1);

	printk("IFXMips PCI I/O mapped to 0x%08X\n", (unsigned long)ifxmips_pci_controller.io_map_base);

	register_pci_controller(&ifxmips_pci_controller);

	return 0;
}

arch_initcall(pcibios_init);
