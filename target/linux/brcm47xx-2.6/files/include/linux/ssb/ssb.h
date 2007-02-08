#ifndef LINUX_SSB_H_
#define LINUX_SSB_H_
#ifdef __KERNEL__

#include <linux/device.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/spinlock.h>

#include <linux/ssb/ssb_regs.h>


struct pcmcia_device;
struct ssb_bus;
struct ssb_driver;


struct ssb_sprom_r1 {
	u16 pci_spid;		/* Subsystem Product ID for PCI */
	u16 pci_svid;		/* Subsystem Vendor ID for PCI */
	u16 pci_pid;		/* Product ID for PCI */
	u8 il0mac[6];		/* MAC address for 802.11b/g */
	u8 et0mac[6];		/* MAC address for Ethernet */
	u8 et1mac[6];		/* MAC address for 802.11a */
	u8 et0phyaddr:5;	/* MII address for enet0 */
	u8 et1phyaddr:5;	/* MII address for enet1 */
	u8 et0mdcport:1;	/* MDIO for enet0 */
	u8 et1mdcport:1;	/* MDIO for enet1 */
	u8 board_rev;		/* Board revision */
	u8 country_code:4;	/* Country Code */
	u8 antenna_a:2;		/* Antenna 0/1 available for A-PHY */
	u8 antenna_bg:2;	/* Antenna 0/1 available for B-PHY and G-PHY */
	u16 pa0b0;
	u16 pa0b1;
	u16 pa0b2;
	u16 pa1b0;
	u16 pa1b1;
	u16 pa1b2;
	u8 gpio0;		/* GPIO pin 0 */
	u8 gpio1;		/* GPIO pin 1 */
	u8 gpio2;		/* GPIO pin 2 */
	u8 gpio3;		/* GPIO pin 3 */
	u16 maxpwr_a;		/* A-PHY Power Amplifier Max Power (in dBm Q5.2) */
	u16 maxpwr_bg;		/* B/G-PHY Power Amplifier Max Power (in dBm Q5.2) */
	u8 itssi_a;		/* Idle TSSI Target for A-PHY */
	u8 itssi_bg;		/* Idle TSSI Target for B/G-PHY */
	u16 boardflags_lo;	/* Boardflags (low 16 bits) */
	u8 antenna_gain_a;	/* A-PHY Antenna gain (in dBm Q5.2) */
	u8 antenna_gain_bg;	/* B/G-PHY Antenna gain (in dBm Q5.2) */
	u8 oem[8];		/* OEM string (rev 1 only) */
};

struct ssb_sprom_r2 {
	u16 boardflags_hi;	/* Boardflags (high 16 bits) */
	u8 maxpwr_a_lo;		/* A-PHY Max Power Low */
	u8 maxpwr_a_hi;		/* A-PHY Max Power High */
	u16 pa1lob0;		/* A-PHY PA Low Settings */
	u16 pa1lob1;		/* A-PHY PA Low Settings */
	u16 pa1lob2;		/* A-PHY PA Low Settings */
	u16 pa1hib0;		/* A-PHY PA High Settings */
	u16 pa1hib1;		/* A-PHY PA High Settings */
	u16 pa1hib2;		/* A-PHY PA High Settings */
	u8 ofdm_pwr_off;	/* OFDM Power Offset from CCK Level */
	u8 country_str[2];	/* Two char Country Code */
};

struct ssb_sprom_r3 {
	u32 ofdmapo;		/* A-PHY OFDM Mid Power Offset */
	u32 ofdmalpo;		/* A-PHY OFDM Low Power Offset */
	u32 ofdmahpo;		/* A-PHY OFDM High Power Offset */
	u8 gpioldc_on_cnt;	/* GPIO LED Powersave Duty Cycle ON count */
	u8 gpioldc_off_cnt;	/* GPIO LED Powersave Duty Cycle OFF count */
	u8 cckpo_1M:4;		/* CCK Power Offset for Rate 1M */
	u8 cckpo_2M:4;		/* CCK Power Offset for Rate 2M */
	u8 cckpo_55M:4;		/* CCK Power Offset for Rate 5.5M */
	u8 cckpo_11M:4;		/* CCK Power Offset for Rate 11M */
	u32 ofdmgpo;		/* G-PHY OFDM Power Offset */
};

struct ssb_sprom_r4 {
	/* TODO */
};

struct ssb_sprom {
	u8 revision;
	u8 crc;
	/* The valid r# fields are selected by the "revision".
	 * Revision 3 and lower inherit from lower revisions.
	 */
	union {
		struct {
			struct ssb_sprom_r1 r1;
			struct ssb_sprom_r2 r2;
			struct ssb_sprom_r3 r3;
		};
		struct ssb_sprom_r4 r4;
	};
};


/* Core-ID values. */
#define SSB_DEV_CHIPCOMMON	0x800
#define SSB_DEV_ILINE20		0x801
#define SSB_DEV_SDRAM		0x803
#define SSB_DEV_PCI		0x804
#define SSB_DEV_MIPS		0x805
#define SSB_DEV_ETHERNET	0x806
#define SSB_DEV_V90		0x807
#define SSB_DEV_USB11_HOSTDEV	0x808
#define SSB_DEV_ADSL		0x809
#define SSB_DEV_ILINE100	0x80A
#define SSB_DEV_IPSEC		0x80B
#define SSB_DEV_PCMCIA		0x80D
#define SSB_DEV_INTERNAL_MEM	0x80E
#define SSB_DEV_MEMC_SDRAM	0x80F
#define SSB_DEV_EXTIF		0x811
#define SSB_DEV_80211		0x812
#define SSB_DEV_MIPS_3302	0x816
#define SSB_DEV_USB11_HOST	0x817
#define SSB_DEV_USB11_DEV	0x818
#define SSB_DEV_USB20_HOST	0x819
#define SSB_DEV_USB20_DEV	0x81A
#define SSB_DEV_SDIO_HOST	0x81B
#define SSB_DEV_ROBOSWITCH	0x81C
#define SSB_DEV_PARA_ATA	0x81D
#define SSB_DEV_SATA_XORDMA	0x81E
#define SSB_DEV_ETHERNET_GBIT	0x81F
#define SSB_DEV_PCIE		0x820
#define SSB_DEV_MIMO_PHY	0x821
#define SSB_DEV_SRAM_CTRLR	0x822
#define SSB_DEV_MINI_MACPHY	0x823
#define SSB_DEV_ARM_1176	0x824
#define SSB_DEV_ARM_7TDMI	0x825

/* Vendor-ID values */
#define SSB_VENDOR_BROADCOM	0x4243

struct ssb_device_id {
	u16 vendor;
	u16 coreid;
	u8 revision;
};
#define SSB_DEVICE(_vendor, _coreid, _revision)  \
	{ .vendor = _vendor, .coreid = _coreid, .revision = _revision, }
#define SSB_DEVTABLE_END  \
	{ 0, },

#define SSB_ANY_VENDOR		0xFFFF
#define SSB_ANY_ID		0xFFFF
#define SSB_ANY_REV		0xFF


struct ssb_device {
	struct device dev;
	struct ssb_bus *bus;
	struct ssb_device_id id;

	u8 core_index;
	unsigned int irq;
	void *drvdata;		/* Per-device data */
	void *devtypedata;	/* Per-devicetype (eg 802.11) data */
};
#define dev_to_ssb_dev(_dev) container_of(_dev, struct ssb_device, dev)

/* Device specific user data */
static inline
void ssb_set_drvdata(struct ssb_device *dev, void *data)
{
	dev->drvdata = data;
}
static inline
void * ssb_get_drvdata(struct ssb_device *dev)
{
	return dev->drvdata;
}

/* Devicetype specific user data. This is per device-type (not per device) */
void ssb_set_devtypedata(struct ssb_device *dev, void *data);
static inline
void * ssb_get_devtypedata(struct ssb_device *dev)
{
	return dev->devtypedata;
}

struct ssb_bus_ops {
	u16 (*read16)(struct ssb_device *dev, u16 offset);
	u32 (*read32)(struct ssb_device *dev, u16 offset);
	void (*write16)(struct ssb_device *dev, u16 offset, u16 value);
	void (*write32)(struct ssb_device *dev, u16 offset, u32 value);
};


struct ssb_driver {
	const char *name;
	const struct ssb_device_id *id_table;

	int (*probe)(struct ssb_device *dev, const struct ssb_device_id *id);
	void (*remove)(struct ssb_device *dev);
	int (*suspend)(struct ssb_device *dev, pm_message_t state);
	int (*resume)(struct ssb_device *dev);
	void (*shutdown)(struct ssb_device *dev);

	struct device_driver drv;
};
#define drv_to_ssb_drv(_drv) container_of(_drv, struct ssb_driver, drv)

extern int __ssb_driver_register(struct ssb_driver *drv, struct module *owner);
static inline int ssb_driver_register(struct ssb_driver *drv)
{
	return __ssb_driver_register(drv, THIS_MODULE);
}
extern void ssb_driver_unregister(struct ssb_driver *drv);




enum ssb_bustype {
	SSB_BUSTYPE_SSB,	/* This SSB bus is the system bus */
	SSB_BUSTYPE_PCI,	/* SSB is connected to PCI bus */
	SSB_BUSTYPE_PCMCIA,	/* SSB is connected to PCMCIA bus */
	//TODO SSB_BUSTYPE_JTAG,
};

/* board_vendor */
#define SSB_BOARDVENDOR_BCM	0x14E4	/* Broadcom */
#define SSB_BOARDVENDOR_DELL	0x1028	/* Dell */
#define SSB_BOARDVENDOR_HP	0x0E11	/* HP */
/* board_type */
#define SSB_BOARD_BCM94306MP	0x0418
#define SSB_BOARD_BCM4309G	0x0421
#define SSB_BOARD_BCM4306CB	0x0417
#define SSB_BOARD_BCM4309MP	0x040C
#define SSB_BOARD_MP4318	0x044A
#define SSB_BOARD_BU4306	0x0416
#define SSB_BOARD_BU4309	0x040A
/* chip_package */
#define SSB_CHIPPACK_BCM4712S	1	/* Small 200pin 4712 */
#define SSB_CHIPPACK_BCM4712M	2	/* Medium 225pin 4712 */
#define SSB_CHIPPACK_BCM4712L	0	/* Large 340pin 4712 */

static inline u16 ssb_read16(struct ssb_device *dev, u16 offset);
static inline u32 ssb_read32(struct ssb_device *dev, u16 offset);
static inline void ssb_write16(struct ssb_device *dev, u16 offset, u16 value);
static inline void ssb_write32(struct ssb_device *dev, u16 offset, u32 value);
static inline u32 ssb_write32_masked(struct ssb_device *dev, u16 offset, u32 mask, u32 value);

#include <linux/ssb/ssb_driver_chipcommon.h>
#include <linux/ssb/ssb_driver_mips.h>
#include <linux/ssb/ssb_driver_extif.h>
#include <linux/ssb/ssb_driver_pci.h>

struct ssb_bus {
	/* The MMIO area. */
	void __iomem *mmio;

	const struct ssb_bus_ops *ops;

	/* The core in the basic address register window. (PCI bus only) */
	struct ssb_device *mapped_device;
	/* Currently mapped PCMCIA segment. (bustype == SSB_BUSTYPE_PCMCIA only) */
	u8 mapped_pcmcia_seg;
	/* Lock for core and segment switching. */
	spinlock_t bar_lock;

	/* The bus this backplane is running on. */
	enum ssb_bustype bustype;
	/* Pointer to the PCI bus (only valid if bustype == SSB_BUSTYPE_PCI). */
	struct pci_dev *host_pci;
	/* Pointer to the PCMCIA device (only if bustype == SSB_BUSTYPE_PCMCIA). */
	struct pcmcia_device *host_pcmcia;

	/* ID information about the PCB. */
	u16 board_vendor;
	u16 board_type;
	u16 board_rev;
	/* ID information about the Chip. */
	u16 chip_id;
	u16 chip_rev;
	u8 chip_package;

	/* Contents of the SPROM.
	 * If there is no sprom (not on PCI-bus), this is emulated. */
	struct ssb_sprom sprom;

	/* List of devices (cores) on the backplane. */
	struct ssb_device devices[SSB_MAX_NR_CORES];
	u8 nr_devices;

	/* Reference count. Number of suspended devices. */
	u8 suspend_cnt;

	/* Software ID number for this bus. */
	int busnumber;

	/* The ChipCommon device (if available). */
	struct ssb_chipcommon chipco;
	/* The PCI-core device (if available). */
	struct ssb_pcicore pcicore;
	/* The MIPS-core device (if available). */
	struct ssb_mipscore mipscore;
	/* The EXTif-core device (if available). */
	struct ssb_extif extif;

	/* Internal. */
	struct list_head list;
};

extern int ssb_bus_ssbbus_register(struct ssb_bus *bus,
				   unsigned long baseaddr,
				   void (*fill_sprom)(struct ssb_sprom *sprom));
#ifdef CONFIG_SSB_PCIHOST
extern int ssb_bus_pcibus_register(struct ssb_bus *bus,
				   struct pci_dev *host_pci);
#endif /* CONFIG_SSB_PCIHOST */
#ifdef CONFIG_SSB_PCMCIAHOST
extern int ssb_bus_pcmciabus_register(struct ssb_bus *bus,
				      struct pcmcia_device *pcmcia_dev,
				      unsigned long baseaddr,
				      void (*fill_sprom)(struct ssb_sprom *sprom));
#endif /* CONFIG_SSB_PCMCIAHOST */

extern void ssb_bus_unregister(struct ssb_bus *bus);

extern u32 ssb_clockspeed(struct ssb_bus *bus);

int ssb_device_is_enabled(struct ssb_device *dev);
void ssb_device_enable(struct ssb_device *dev, u32 core_specific_flags);
void ssb_device_disable(struct ssb_device *dev, u32 core_specific_flags);


static inline u16 ssb_read16(struct ssb_device *dev, u16 offset)
{
	return dev->bus->ops->read16(dev, offset);
}
static inline u32 ssb_read32(struct ssb_device *dev, u16 offset)
{
	return dev->bus->ops->read32(dev, offset);
}
static inline void ssb_write16(struct ssb_device *dev, u16 offset, u16 value)
{
	dev->bus->ops->write16(dev, offset, value);
}
static inline void ssb_write32(struct ssb_device *dev, u16 offset, u32 value)
{
	dev->bus->ops->write32(dev, offset, value);
}

static inline u32 ssb_write32_masked(struct ssb_device *dev,
				  u16 offset,
				  u32 mask,
				  u32 value)
{
	value &= mask;
	value |= ssb_read32(dev, offset) & ~mask;
	ssb_write32(dev, offset, value);
	return value;
}


/* Translation (routing) bits that need to be ORed to DMA
 * addresses before they are given to a device. */
extern u32 ssb_dma_translation(struct ssb_device *dev);
#define SSB_DMA_TRANSLATION_MASK	0xC0000000
#define SSB_DMA_TRANSLATION_SHIFT	30

extern int ssb_dma_set_mask(struct ssb_device *ssb_dev, u64 mask);


/* Various helper functions */
extern u32 ssb_admatch_base(u32 adm);
extern u32 ssb_admatch_size(u32 adm);


#endif /* __KERNEL__ */
#endif /* LINUX_SSB_H_ */
