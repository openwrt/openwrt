#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/addrspace.h>
#include <linux/vmalloc.h>
#include <ifxmips.h>
#include <ifxmips_irq.h>
#include <ifxmips_cgu.h>

#define IFXMIPS_PCI_MEM_BASE    0x18000000
#define IFXMIPS_PCI_MEM_SIZE    0x02000000
#define IFXMIPS_PCI_IO_BASE     0x1AE00000
#define IFXMIPS_PCI_IO_SIZE     0x00200000

extern int ifxmips_pci_read_config_dword(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *val);
extern int ifxmips_pci_write_config_dword(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 val);

struct pci_ops ifxmips_pci_ops =
{
	.read = ifxmips_pci_read_config_dword,
	.write = ifxmips_pci_write_config_dword
};

static struct resource pci_io_resource =
{
	.name = "io pci IO space",
	.start = IFXMIPS_PCI_IO_BASE,
	.end = IFXMIPS_PCI_IO_BASE + IFXMIPS_PCI_IO_SIZE - 1,
	.flags = IORESOURCE_IO
};

static struct resource pci_mem_resource =
{
	.name = "ext pci memory space",
	.start = IFXMIPS_PCI_MEM_BASE,
	.end = IFXMIPS_PCI_MEM_BASE + IFXMIPS_PCI_MEM_SIZE - 1,
	.flags = IORESOURCE_MEM
};

static struct pci_controller ifxmips_pci_controller =
{
	.pci_ops = &ifxmips_pci_ops,
	.mem_resource = &pci_mem_resource,
	.mem_offset	= 0x00000000UL,
	.io_resource = &pci_io_resource,
	.io_offset	= 0x00000000UL,
};

/* the cpu can can generate the 33Mhz or rely on an external clock the cgu needs the
   proper setting, otherwise the cpu hangs. we have no way of runtime detecting this */
u32 ifxmips_pci_mapped_cfg;
int ifxmips_pci_external_clock = 0;

static int __init
ifxmips_pci_set_external_clk(char *str)
{
	printk("cgu: setting up external pci clock\n");
	ifxmips_pci_external_clock = 1;
	return 1;
}
__setup("pci_external_clk", ifxmips_pci_set_external_clk);

int
pcibios_plat_dev_init(struct pci_dev *dev)
{
	u8 pin;

	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);
	switch(pin)
	{
		case 0:
			break;
		case 1:
			//falling edge level triggered:0x4, low level:0xc, rising edge:0x2
			ifxmips_w32(ifxmips_r32(IFXMIPS_EBU_PCC_CON) | 0xc, IFXMIPS_EBU_PCC_CON);
			ifxmips_w32(ifxmips_r32(IFXMIPS_EBU_PCC_IEN) | 0x10, IFXMIPS_EBU_PCC_IEN);
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

static void __init
ifxmips_pci_startup(void)
{
	u32 temp_buffer;

	cgu_setup_pci_clk(ifxmips_pci_external_clock);

	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_OUT) | (1 << 5), IFXMIPS_GPIO_P1_OUT);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_OD) | (1 << 5), IFXMIPS_GPIO_P1_OD);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_DIR) | (1 << 5), IFXMIPS_GPIO_P1_DIR);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_ALTSEL1) & ~(1 << 5), IFXMIPS_GPIO_P1_ALTSEL1);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_ALTSEL0) & ~(1 << 5), IFXMIPS_GPIO_P1_ALTSEL0);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_DIR) & ~0x2000, IFXMIPS_GPIO_P1_DIR);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_DIR) | 0x4000, IFXMIPS_GPIO_P1_DIR);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_ALTSEL1) & ~0x6000, IFXMIPS_GPIO_P1_ALTSEL1);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_ALTSEL0) | 0x6000, IFXMIPS_GPIO_P1_ALTSEL0);
	/* enable auto-switching between PCI and EBU */
	ifxmips_w32(0xa, PCI_CR_CLK_CTRL);
	/* busy, i.e. configuration is not done, PCI access has to be retried */
	ifxmips_w32(ifxmips_r32(PCI_CR_PCI_MOD) & ~(1 << 24), PCI_CR_PCI_MOD);
	wmb ();
	/* BUS Master/IO/MEM access */
	ifxmips_w32(ifxmips_r32(PCI_CS_STS_CMD) | 7, PCI_CS_STS_CMD);

	/* enable external 2 PCI masters */
	temp_buffer = ifxmips_r32(PCI_CR_PC_ARB);
	temp_buffer &= (~(0xf << 16));
	/* enable internal arbiter */
	temp_buffer |= (1 << INTERNAL_ARB_ENABLE_BIT);
	/* enable internal PCI master reqest */
	temp_buffer &= (~(3 << PCI_MASTER0_REQ_MASK_2BITS));

	/* enable EBU reqest */
	temp_buffer &= (~(3 << PCI_MASTER1_REQ_MASK_2BITS));

	/* enable all external masters request */
	temp_buffer &= (~(3 << PCI_MASTER2_REQ_MASK_2BITS));
	ifxmips_w32(temp_buffer, PCI_CR_PC_ARB);
	wmb ();

	ifxmips_w32(0x18000000, PCI_CR_FCI_ADDR_MAP0);
	ifxmips_w32(0x18400000, PCI_CR_FCI_ADDR_MAP1);
	ifxmips_w32(0x18800000, PCI_CR_FCI_ADDR_MAP2);
	ifxmips_w32(0x18c00000, PCI_CR_FCI_ADDR_MAP3);
	ifxmips_w32(0x19000000, PCI_CR_FCI_ADDR_MAP4);
	ifxmips_w32(0x19400000, PCI_CR_FCI_ADDR_MAP5);
	ifxmips_w32(0x19800000, PCI_CR_FCI_ADDR_MAP6);
	ifxmips_w32(0x19c00000, PCI_CR_FCI_ADDR_MAP7);
	ifxmips_w32(0x1ae00000, PCI_CR_FCI_ADDR_MAP11hg);
	ifxmips_w32(0x0e000008, PCI_CR_BAR11MASK);
	ifxmips_w32(0, PCI_CR_PCI_ADDR_MAP11);
	ifxmips_w32(0, PCI_CS_BASE_ADDR1);
#ifdef CONFIG_SWAP_IO_SPACE
	/* both TX and RX endian swap are enabled */
	ifxmips_w32(ifxmips_r32(PCI_CR_PCI_EOI) | 3, PCI_CR_PCI_EOI);
	wmb ();
#endif
	/*TODO: disable BAR2 & BAR3 - why was this in the origianl infineon code */
	ifxmips_w32(ifxmips_r32(PCI_CR_BAR12MASK) | 0x80000000, PCI_CR_BAR12MASK);
	ifxmips_w32(ifxmips_r32(PCI_CR_BAR13MASK) | 0x80000000, PCI_CR_BAR13MASK);
	/*use 8 dw burst length */
	ifxmips_w32(0x303, PCI_CR_FCI_BURST_LENGTH);
	ifxmips_w32(ifxmips_r32(PCI_CR_PCI_MOD) | (1 << 24), PCI_CR_PCI_MOD);
	wmb();
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_OUT) & ~(1 << 5), IFXMIPS_GPIO_P1_OUT);
	wmb();
	mdelay(1);
	ifxmips_w32(ifxmips_r32(IFXMIPS_GPIO_P1_OUT) | (1 << 5), IFXMIPS_GPIO_P1_OUT);
}

int __init
pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin){
	switch(slot)
	{
		case 13:
			/* IDSEL = AD29 --> USB Host Controller */
			return (INT_NUM_IM1_IRL0 + 17);
		case 14:
			/* IDSEL = AD30 --> mini PCI connector */
			return (INT_NUM_IM0_IRL0 + 22);
		default:
			printk("Warning: no IRQ found for PCI device in slot %d, pin %d\n", slot, pin);
			return 0;
	}
}

int __init
pcibios_init(void)
{
	extern int pci_probe_only;

	pci_probe_only = 0;
	printk("PCI: Probing PCI hardware on host bus 0.\n");
	ifxmips_pci_startup ();
	ifxmips_pci_mapped_cfg = (u32)ioremap_nocache(0x17000000, 0x800 * 16);
	printk("IFXMips PCI mapped to 0x%08lX\n", (unsigned long)ifxmips_pci_mapped_cfg);
	ifxmips_pci_controller.io_map_base = (unsigned long)ioremap(IFXMIPS_PCI_IO_BASE, IFXMIPS_PCI_IO_SIZE - 1);
	printk("IFXMips PCI I/O mapped to 0x%08lX\n", (unsigned long)ifxmips_pci_controller.io_map_base);
	register_pci_controller(&ifxmips_pci_controller);
	return 0;
}

arch_initcall(pcibios_init);
