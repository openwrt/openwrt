/*
 *  Annapurna Labs PCI host bridge device tree driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * - This driver for both internal PCIe bus and for external PCIe ports
 *   (in Root-Complex mode).
 * - The driver requires PCI_DOMAINS as each port registered as a pci domain
 * - for the external PCIe ports, the following applies:
 *	- Configuration access to bus 0 device 0 are routed to the configuration
 *	  space header register that found in the host bridge.
 *	- The driver assumes the controller link is initialized by the
 *	  bootloader.
 */
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>

#include <asm/mach/pci.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include "core.h"
#include <mach/al_hal_iomap.h>
#include <mach/al_hal_pcie.h>

#include "../../drivers/pci/pci.h"

enum al_pci_type {
  AL_PCI_TYPE_INTERNAL = 0,
  AL_PCI_TYPE_EXTERNAL = 1,
};

/* PCI bridge private data */
struct al_pcie_pd {
  struct device *dev;
  enum al_pci_type type;
  struct resource ecam;
  struct resource mem;
  struct resource io;
  struct resource realio;
  struct resource regs;
  struct resource busn;

  void __iomem *ecam_base;
  void __iomem *regs_base;

  void __iomem *local_bridge_config_space;
  unsigned int index;
  /* lock configuration access as we change the target_bus */
  spinlock_t conf_lock;
  /*HAL structure*/
  struct al_pcie_port pcie_port;
  struct al_pcie_link_status status;
  u8 target_bus;
};

static inline struct al_pcie_pd *sys_to_pcie(struct pci_sys_data *sys) {
  return sys->private_data;
}

static int al_pcie_enable_controller(struct al_pcie_pd *pcie) {
  if (pcie->type == AL_PCI_TYPE_INTERNAL)
    return 0;

  al_pcie_handle_init(&pcie->pcie_port, pcie->regs_base, pcie->index);
  if (al_pcie_function_type_get(&pcie->pcie_port) != AL_PCIE_FUNCTION_MODE_RC) {
    dev_err(pcie->dev, "controller is not configured to Root-Complex mode\n");
    return -ENOSYS;
  }

  return 0;
}

static bool al_pcie_port_check_link(struct al_pcie_pd *pcie) {
  struct al_pcie_link_status *status = &pcie->status;
  int rc;

  if (pcie->type == AL_PCI_TYPE_INTERNAL)
    return true;

  rc = al_pcie_link_status(&pcie->pcie_port, status);
  if (rc < 0) {
    dev_err(pcie->dev, "failed to get pcie link status\n");
    return false;
  }
  if (status->link_up == AL_FALSE) {
    dev_info(pcie->dev, "link %u down\n", pcie->index);
    return false;
  }
  dev_info(pcie->dev, "link up: speed Gen %d width x%x\n", status->speed,
           status->lanes);

  return true;
}

/* prepare controller for issueing IO transactions*/
static int al_pcie_io_prepare(struct al_pcie_pd *pcie) {
  struct al_pcie_port *pcie_port = &pcie->pcie_port;
  if (pcie->type == AL_PCI_TYPE_INTERNAL) {
    return 0;
  } else {
    struct al_pcie_atu_region io_atu_region = {
        .enable = AL_TRUE,
        .direction = al_pcie_atu_dir_outbound,
        .index = 0,
        .base_addr = (uint64_t)pcie->io.start,
        .limit = (uint64_t)pcie->io.start + resource_size(&pcie->io) - 1,
        .target_addr = (uint64_t)pcie->realio
                           .start, /* the address that matches will be
                                      translated to this address + offset */
        .invert_matching = AL_FALSE,
        .tlp_type = AL_PCIE_TLP_TYPE_IO, /* pcie tlp type*/
        .attr = 0,                       /* pcie frame header attr field*/
        /* outbound specific params */
        .msg_code = 0, /* pcie message code */
        .cfg_shift_mode = AL_FALSE,
        /* inbound specific params*/
    };

    dev_dbg(pcie->dev, "%s: base %llx, limit %llx, target %llx\n", __func__,
            io_atu_region.base_addr, io_atu_region.limit,
            io_atu_region.target_addr);
    al_pcie_atu_region_set(pcie_port, &io_atu_region);
  }

  return 0;
}

#ifdef CONFIG_AL_PCIE_RMN_1010
/* prepare controller for issuing mem transactions */
static int al_pcie_mem_prepare(struct al_pcie_pd *pcie) {
  struct al_pcie_port *pcie_port = &pcie->pcie_port;
  if (pcie->type == AL_PCI_TYPE_INTERNAL) {
    return 0;
  } else {
    struct al_pcie_atu_region mem_atu_region;

    /*
     * This region is meant to insure all accesses to this section
     * will be always with type memory (accessing from DMA may
     * change the type to IO).
     */
    mem_atu_region.enable = AL_TRUE;
    mem_atu_region.direction = al_pcie_atu_dir_outbound;
    mem_atu_region.index = 1;
    mem_atu_region.base_addr = pcie->mem.start;
    mem_atu_region.limit = pcie->mem.end;
    mem_atu_region.target_addr = pcie->mem.start;
    mem_atu_region.invert_matching = AL_FALSE;
    mem_atu_region.tlp_type = AL_PCIE_TLP_TYPE_MEM; /* pcie tlp type*/
    mem_atu_region.attr = 0;     /* pcie frame header attr field*/
    mem_atu_region.msg_code = 0; /* pcie message code */
    mem_atu_region.cfg_shift_mode = AL_FALSE;
    mem_atu_region.bar_number = 0; /* not used */
    mem_atu_region.match_mode = 0; /* address match mode */
    mem_atu_region.enable_attr_match_mode = AL_FALSE;
    mem_atu_region.enable_msg_match_mode = AL_FALSE;

    dev_dbg(pcie->dev, "%s: base %llx, limit %llx, target %llx\n", __func__,
            mem_atu_region.base_addr, mem_atu_region.limit,
            mem_atu_region.target_addr);

    al_pcie_atu_region_set(pcie_port, &mem_atu_region);
  }

  return 0;
}
#endif

/* prepare controller for issueing CFG transactions*/
static int al_pcie_cfg_prepare(struct al_pcie_pd *pcie) {
  struct al_pcie_port *pcie_port = &pcie->pcie_port;

  if (pcie->type == AL_PCI_TYPE_INTERNAL)
    return 0;

  spin_lock_init(&pcie->conf_lock);
  pcie->target_bus = 1;
  /*
   * force the controller to set the pci bus in the TLP to
   * pcie->target_bus no matter what is the bus portion of the ECAM addess
   * is.
   */
  al_pcie_target_bus_set(pcie_port, pcie->target_bus, 0xFF);

  /* the bus connected to the controller always enumberated as bus 1*/
  al_pcie_secondary_bus_set(pcie_port, 1);
  /* set subordinary to max value */
  al_pcie_subordinary_bus_set(pcie_port, 0xff);

  return 0;
}

/* Get ECAM address according to bus, device, function, and offset */
static void __iomem *al_pcie_cfg_addr(struct al_pcie_pd *pcie,
                                      struct pci_bus *bus, unsigned int devfn,
                                      int offset) {
  void __iomem *ecam_base = pcie->ecam_base;
  unsigned int busnr = bus->number;
  int slot = PCI_SLOT(devfn);
  void __iomem *ret_val;

  /* Trap out illegal values */
  if (busnr > 255)
    BUG();
  if (devfn > 255)
    BUG();

  ret_val = (ecam_base +
             ((PCI_SLOT(devfn) << 15) | (PCI_FUNC(devfn) << 12) | offset));
  if (pcie->type == AL_PCI_TYPE_INTERNAL)
    return ret_val;

  /* If there is no link, just show the PCI bridge. */
  if ((pcie->status.link_up == AL_FALSE) && (busnr > 0 || slot > 0))
    return NULL;

  if (busnr == 0) {
    if (slot > 0)
      return NULL;
    ret_val = pcie->local_bridge_config_space;
    ret_val += offset;
  } else {
    if (busnr != pcie->target_bus) {
      dev_dbg(pcie->dev, "change target bus number from %d to %d\n",
              pcie->target_bus, busnr);
      pcie->target_bus = busnr;
      al_pcie_target_bus_set(&pcie->pcie_port, pcie->target_bus, 0xFF);
    }
  }
  return ret_val;
}

/* PCI config space read */
static int al_read_config(struct pci_bus *bus, unsigned int devfn, int where,
                          int size, u32 *val) {
  struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
  int rc = PCIBIOS_SUCCESSFUL;
  unsigned long flags;
  void __iomem *addr;
  u32 v = 0xffffffff;

  dev_dbg(pcie->dev, "read_config from %d size %d dev (domain %d) %d:%d:%d\n",
          where, size, pcie->index, bus->number, PCI_SLOT(devfn),
          PCI_FUNC(devfn));

  spin_lock_irqsave(&pcie->conf_lock, flags);

  addr = al_pcie_cfg_addr(pcie, bus, devfn, where);
  dev_dbg(pcie->dev, " read address %p\n", addr);

  if (addr) {
    switch (size) {
    case 1:
      v = readb(addr);
      break;
    case 2:
      v = readw(addr);
      break;
    case 4:
      v = readl(addr);
      break;
    default:
      rc = PCIBIOS_BAD_REGISTER_NUMBER;
    }
  } else {
    rc = PCIBIOS_DEVICE_NOT_FOUND;
  }

  spin_unlock_irqrestore(&pcie->conf_lock, flags);
  *val = v;
  pr_debug("read_config_byte read %#x\n", *val);
  return rc;
}

/* PCI config space write */
static int al_write_config(struct pci_bus *bus, unsigned int devfn, int where,
                           int size, u32 val) {
  struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
  int rc = PCIBIOS_SUCCESSFUL;
  unsigned long flags;
  void __iomem *addr;

  dev_dbg(pcie->dev,
          "write_config_byte %#x to %d size %d dev (domain %d) %d:%d:%d\n", val,
          where, size, pcie->index, bus->number, PCI_SLOT(devfn),
          PCI_FUNC(devfn));

  spin_lock_irqsave(&pcie->conf_lock, flags);

  addr = al_pcie_cfg_addr(pcie, bus, devfn, where);
  dev_dbg(pcie->dev, " write address %p\n", addr);
  if (addr) {
    switch (size) {
    case 1:
      writeb((u8)val, addr);
      break;

    case 2:
      writew((u16)val, addr);
      break;

    case 4:
      writel(val, addr);
      break;
    default:
      rc = PCIBIOS_BAD_REGISTER_NUMBER;
    }
  } else {
    rc = PCIBIOS_DEVICE_NOT_FOUND;
  }
  spin_unlock_irqrestore(&pcie->conf_lock, flags);
  return rc;
}

/* PCI bridge config space read/write operations */
static struct pci_ops al_pcie_ops = {
    .read = al_read_config,
    .write = al_write_config,
};

/* PCI config space read */
static int al_internal_read_config(struct pci_bus *bus, unsigned int devfn,
                                   int where, int size, u32 *val) {
  struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
  void __iomem *addr = al_pcie_cfg_addr(pcie, bus, devfn, where & ~3);
  u32 v;

  //	printk(KERN_EMERG "read_config from %d size %d dev %d:%d:%d\n", where,
  //size, 		 bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

  switch (size) {
  case 1:
    v = readl(addr);
    v = (v >> ((where & 0x3) * 8)) & 0xff;
    break;
  case 2:
    v = readl(addr);
    v = (v >> ((where & 0x3) * 8)) & 0xffff;
    break;
  default:
    v = readl(addr);
    break;
  }

  *val = v;
  pr_debug("read_config_byte read %#x\n", *val);
  return PCIBIOS_SUCCESSFUL;
}

/* PCI config space write */
static int al_internal_write_config(struct pci_bus *bus, unsigned int devfn,
                                    int where, int size, u32 val) {
  struct al_pcie_pd *pcie = sys_to_pcie(bus->sysdata);
  void __iomem *addr = al_pcie_cfg_addr(pcie, bus, devfn, where);

  pr_debug("write_config %#x to %d size %d dev %d:%d:%d\n", val, where, size,
           bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

  switch (size) {
  case 1:
    writeb((u8)val, addr);
    break;
  case 2:
    writew((u16)val, addr);
    break;
  case 4:
    writel(val, addr);
    break;
  }

  return PCIBIOS_SUCCESSFUL;
}

/* PCI bridge config space read/write operations */
static struct pci_ops al_internal_pcie_ops = {
    .read = al_internal_read_config,
    .write = al_internal_write_config,
};

static int al_pcie_parse_bus_range(struct device_node *np,
                                   struct resource *res) {
  u32 bus_range[2];
  int err;

  err = of_property_read_u32_array(np, "bus-range", bus_range,
                                   ARRAY_SIZE(bus_range));
  if (err)
    return err;

  res->name = np->name;
  res->start = bus_range[0];
  res->end = bus_range[1];
  res->flags = IORESOURCE_BUS;

  return 0;
}

static int al_pcie_setup(int nr, struct pci_sys_data *sys) {
  struct al_pcie_pd *pcie = sys_to_pcie(sys);

  if (pcie->type == AL_PCI_TYPE_EXTERNAL)
    pci_add_resource_offset(&sys->resources, &pcie->realio, sys->io_offset);

  pci_add_resource_offset(&sys->resources, &pcie->mem, sys->mem_offset);
  pci_add_resource(&sys->resources, &pcie->busn);

  return 1;
}

static int al_pcie_parse_dt(struct al_pcie_pd *pcie) {
  struct device_node *np = pcie->dev->of_node;
  struct of_pci_range_iter iter;
  int err;
  static int index;

  if (pcie->type == AL_PCI_TYPE_EXTERNAL) {
    /* Get registers resources */
    err = of_address_to_resource(np, 0, &pcie->regs);
    if (err < 0) {
      dev_dbg(pcie->dev, "of_address_to_resource(): %d\n", err);
      return err;
    }
    dev_dbg(pcie->dev, " regs %pR\n", &pcie->regs);
    pcie->regs_base = devm_ioremap_resource(pcie->dev, &pcie->regs);
    if (!pcie->regs_base)
      return -EADDRNOTAVAIL;
    /* set the base address of the configuration space of the local
     * bridge
     */
    pcie->local_bridge_config_space = pcie->regs_base + 0x2000;
  }
  /* Get the ECAM, I/O and memory ranges from DT */
  for_each_of_pci_range_compat(&iter, np) {
    unsigned long restype = iter.flags & IORESOURCE_TYPE_BITS;
    if (restype == 0) {
      range_iter_fill_resource(iter, np, &pcie->ecam);
      pcie->ecam.flags = IORESOURCE_MEM;
      pcie->ecam.name = "ECAM";
    }
    if (restype == IORESOURCE_IO) {
      range_iter_fill_resource(iter, np, &pcie->io);
      range_iter_fill_resource(iter, np, &pcie->realio);
      pcie->realio.start = iter.pci_addr;
      pcie->realio.end = iter.pci_addr + iter.size - 1;
      pcie->io.name = "I/O";

      pci_remap_iospace(&pcie->realio, iter.cpu_addr);
    }
    if (restype == IORESOURCE_MEM) {
      range_iter_fill_resource(iter, np, &pcie->mem);
      pcie->mem.name = "MEM";
    }
  }

  /* map ecam space */
  dev_dbg(pcie->dev, " ecam %pr\n", &pcie->ecam);
  pcie->ecam_base = devm_ioremap_resource(pcie->dev, &pcie->ecam);
  if (!pcie->ecam_base)
    return -EADDRNOTAVAIL;

  err = al_pcie_parse_bus_range(np, &pcie->busn);
  if (err < 0) {
    dev_err(pcie->dev, "failed to parse bus-range property: %d\n", err);
    pcie->busn.name = np->name;
    pcie->busn.start = 1;
    pcie->busn.end = 0xff;
    pcie->busn.flags = IORESOURCE_BUS;
  }
  pcie->index = index++;
  return 0;
}

/* map the specified device/slot/pin to an IRQ */
static int al_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin) {
  return of_irq_parse_and_map_pci(dev, slot, pin);
}

static int al_pcie_scan_bus(int nr, struct pci_host_bridge *bridge) {
  struct pci_sys_data *sys = pci_host_bridge_priv(bridge);
  struct al_pcie_pd *pcie = sys_to_pcie(sys);

  list_splice_init(&sys->resources, &bridge->windows);
  bridge->dev.parent = pcie->dev;
  bridge->sysdata = sys;
  bridge->busnr = sys->busnr;

  if (pcie->type == AL_PCI_TYPE_INTERNAL)
    bridge->ops = &al_internal_pcie_ops;
  else
    bridge->ops = &al_pcie_ops;

  return pci_scan_root_bus_bridge(bridge);
}

/*
 * Fixup function to make sure Max Paylod Size and MaxReadReq
 * are set based on host bridge Max capabilities.
 */

extern int pcie_bus_configure_set(struct pci_dev *dev, void *data);
static void al_pci_fixup(struct pci_dev *dev) {
  u8 smpss = 0;
  pcie_bus_configure_set(dev, &smpss);
}
DECLARE_PCI_FIXUP_HEADER(PCI_ANY_ID, PCI_ANY_ID, al_pci_fixup);

static int al_pcie_add_host_bridge(struct al_pcie_pd *pcie) {
  struct hw_pci hw;

  memset(&hw, 0, sizeof(hw));

  hw.nr_controllers = 1;
  //	hw.domain = pcie->index;
  hw.private_data = (void **)&pcie;
  hw.setup = al_pcie_setup;
  hw.scan = al_pcie_scan_bus;
  hw.map_irq = al_pcie_map_irq;

  pci_common_init(&hw);

  return 0;
}

static const struct of_device_id al_pcie_of_match[] = {
    {.compatible = "annapurna-labs,al-internal-pcie",
     .data = (void *)AL_PCI_TYPE_INTERNAL},
    {.compatible = "annapurna-labs,al-pci",
     .data = (void *)AL_PCI_TYPE_EXTERNAL},
    {},
};

extern uint64_t al_pcie_read_addr_start[AL_SB_PCIE_NUM];
extern uint64_t al_pcie_read_addr_end[AL_SB_PCIE_NUM];
extern uint64_t al_pcie_write_addr_start[AL_SB_PCIE_NUM];
extern uint64_t al_pcie_write_addr_end[AL_SB_PCIE_NUM];
extern bool al_pcie_address_valid[AL_SB_PCIE_NUM];

static int al_pcie_probe(struct platform_device *pdev) {
  enum al_pci_type type;
  const struct of_device_id *of_id;
  struct al_pcie_pd *pcie;
  int err;

  of_id = of_match_device(al_pcie_of_match, &pdev->dev);
  if (of_id)
    type = (enum al_pci_type)of_id->data;
  else {
    return -1;
  }

  pcie = devm_kzalloc(&pdev->dev, sizeof(*pcie), GFP_KERNEL);
  if (!pcie)
    return -ENOMEM;

  pcie->type = type;
  pcie->dev = &pdev->dev;

  err = al_pcie_parse_dt(pcie);
  if (err < 0)
    return err;

  err = al_pcie_enable_controller(pcie);
  if (err)
    goto err;

  al_pcie_port_check_link(pcie);

  al_pcie_cfg_prepare(pcie);

  al_pcie_io_prepare(pcie);

#ifdef CONFIG_AL_PCIE_RMN_1010
  al_pcie_mem_prepare(pcie);
  if (pcie->type != AL_PCI_TYPE_INTERNAL) {

    al_pcie_read_addr_start[pcie->index] = min(pcie->mem.start, pcie->io.start);
    al_pcie_read_addr_end[pcie->index] = max(pcie->mem.end, pcie->io.end);

    al_pcie_write_addr_start[pcie->index] = pcie->io.start;
    al_pcie_write_addr_end[pcie->index] = pcie->io.end;

    al_pcie_address_valid[pcie->index] = true;

    dev_info(&pdev->dev, "%s: [pcie %d] use DMA for read from %llx to %llx\n",
             __func__, pcie->index, al_pcie_read_addr_start[pcie->index],
             al_pcie_read_addr_end[pcie->index]);

    dev_info(&pdev->dev, "%s: [pcie %d] use DMA for write from %llx to %llx\n",
             __func__, pcie->index, al_pcie_write_addr_start[pcie->index],
             al_pcie_write_addr_end[pcie->index]);

    /*
     * set an axi IO bar to make the accesses to this addresses
     * with size of 4 bytes. (access from DMA will be 16 Bytes minimum)
     */
    al_pcie_axi_io_config(&pcie->pcie_port,
                          al_pcie_read_addr_start[pcie->index],
                          al_pcie_read_addr_end[pcie->index]);
  }
#endif

  /* Configure IOCC for external PCIE */
  if (pcie->type != AL_PCI_TYPE_INTERNAL) {
    if (pdev->dev.archdata.hwcc) {
      printk("Configuring PCIE for IOCC\n");
      al_pcie_port_snoop_config(&pcie->pcie_port, 1);
    }
  }
  udelay(1000);
  err = al_pcie_add_host_bridge(pcie);
  if (err < 0) {
    dev_err(&pdev->dev, "failed to enable PCIe controller: %d\n", err);
    goto enable_err;
  }

  platform_set_drvdata(pdev, pcie);
  return 0;
enable_err:
err:
  return err;
}

static struct platform_driver al_pcie_driver = {
    .driver =
        {
            .name = "al-pcie",
            .owner = THIS_MODULE,
            .of_match_table = of_match_ptr(al_pcie_of_match),
        },
    .probe = al_pcie_probe,
};
module_platform_driver(al_pcie_driver);
