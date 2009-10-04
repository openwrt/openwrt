
#ifdef CONFIG_PCI

#define RT2880_PCI_SLOT1_BASE			0x20000000
#define RALINK_PCI_BASE					0xA0440000
#define RT2880_PCI_PCICFG_ADDR			((unsigned long*)(RALINK_PCI_BASE + 0x0000))
#define RT2880_PCI_ARBCTL				((unsigned long*)(RALINK_PCI_BASE + 0x0080))
#define RT2880_PCI_BAR0SETUP_ADDR		((unsigned long*)(RALINK_PCI_BASE + 0x0010))
#define RT2880_PCI_CONFIG_ADDR			((unsigned long*)(RALINK_PCI_BASE + 0x0020))
#define RT2880_PCI_CONFIG_DATA			((unsigned long*)(RALINK_PCI_BASE + 0x0024))
#define RT2880_PCI_MEMBASE				((unsigned long*)(RALINK_PCI_BASE + 0x0028))
#define RT2880_PCI_IOBASE				((unsigned long*)(RALINK_PCI_BASE + 0x002C))
#define RT2880_PCI_IMBASEBAR0_ADDR		((unsigned long*)(RALINK_PCI_BASE + 0x0018))
#define RT2880_PCI_ID					((unsigned long*)(RALINK_PCI_BASE + 0x0030))
#define RT2880_PCI_CLASS				((unsigned long*)(RALINK_PCI_BASE + 0x0034))
#define RT2880_PCI_SUBID				((unsigned long*)(RALINK_PCI_BASE + 0x0038))
#define RT2880_PCI_PCIMSK_ADDR			((unsigned long*)(RALINK_PCI_BASE + 0x000C))

#endif
