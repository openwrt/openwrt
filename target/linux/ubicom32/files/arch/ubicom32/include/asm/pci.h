/*
 * arch/ubicom32/include/asm/pci.h
 *   Definitions of PCI operations for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_PCI_H
#define _ASM_UBICOM32_PCI_H

#include <asm/io.h>

/* The PCI address space does equal the physical memory
 * address space.  The networking and block device layers use
 * this boolean for bounce buffer decisions.
 */
#define PCI_DMA_BUS_IS_PHYS	(1)



/*
 * Perform a master read/write to the PCI bus.
 * These functions return a PCI_RESP_xxx code.
 */
extern u8 pci_read_u32(u8 pci_cmd, u32 address, u32 *data);
extern u8 pci_write_u32(u8 pci_cmd, u32 address, u32 data);
extern u8 pci_read_u16(u8 pci_cmd, u32 address, u16 *data);
extern u8 pci_write_u16(u8 pci_cmd, u32 address, u16 data);
extern u8 pci_read_u8(u8 pci_cmd, u32 address, u8 *data);
extern u8 pci_write_u8(u8 pci_cmd, u32 address, u8 data);


#define PCIBIOS_MIN_IO          0x100
#define PCIBIOS_MIN_MEM         0x10000000

#define pcibios_assign_all_busses()	0
#define pcibios_scan_all_fns(a, b)	0
extern void pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region,
	struct resource *res);

extern void pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
	struct pci_bus_region *region);

struct pci_sys_data;
struct pci_bus;

struct hw_pci {
        struct list_head buses;
        int             nr_controllers;
        int             (*setup)(int nr, struct pci_sys_data *);
        struct pci_bus *(*scan)(int nr, struct pci_sys_data *);
        void            (*preinit)(void);
        void            (*postinit)(void);
        u8              (*swizzle)(struct pci_dev *dev, u8 *pin);
        int             (*map_irq)(struct pci_dev *dev, u8 slot, u8 pin);
};

/*
 * Per-controller structure
 */
struct pci_sys_data {
        struct list_head node;
        int             busnr;          /* primary bus number                   */
        u64             mem_offset;     /* bus->cpu memory mapping offset       */
        unsigned long   io_offset;      /* bus->cpu IO mapping offset           */
        struct pci_bus  *bus;           /* PCI bus                              */
        struct resource *resource[3];   /* Primary PCI bus resources            */
                                        /* Bridge swizzling                     */
        u8              (*swizzle)(struct pci_dev *, u8 *);
                                        /* IRQ mapping                          */
        int             (*map_irq)(struct pci_dev *, u8, u8);
        struct hw_pci   *hw;
};

static  inline struct resource *
pcibios_select_root(struct pci_dev *pdev, struct resource *res)
{
        struct resource *root = NULL;

        if (res->flags & IORESOURCE_IO)
                root = &ioport_resource;
        if (res->flags & IORESOURCE_MEM)
                root = &iomem_resource;

        return root;
}

static inline void pcibios_set_master(struct pci_dev *dev)
{
        /* No special bus mastering setup handling */
}
#define HAVE_ARCH_PCI_SET_DMA_MAX_SEGMENT_SIZE 1
#define HAVE_ARCH_PCI_SET_DMA_SEGMENT_BOUNDARY 1

#ifdef CONFIG_PCI
static inline void * pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
                     dma_addr_t *dma_handle)
{
    void *vaddr = kmalloc(size, GFP_KERNEL);
    if(vaddr != NULL) {
        *dma_handle = virt_to_phys(vaddr);
    }
    return vaddr;
}

static  inline int pci_dma_supported(struct pci_dev *hwdev, dma_addr_t mask)
{
	return 1;
}

static  inline void pci_free_consistent(struct pci_dev *hwdev, size_t size,
	void *cpu_addr, dma_addr_t dma_handle)
{
	kfree(cpu_addr);
	return;
}

static inline dma_addr_t pci_map_single(struct pci_dev *hwdev, void *ptr,
	size_t size, int direction)
{
	 return virt_to_phys(ptr);
}

static inline void pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr,
	size_t size, int direction)
{
	 return;
}

static inline dma_addr_t
pci_map_page(struct pci_dev *hwdev, struct page *page,
             unsigned long offset, size_t size, int direction)
{
	 return pci_map_single(hwdev, page_address(page) + offset, size, (int)direction);
}

static inline void
pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address,
               size_t size, int direction)
{
	pci_unmap_single(hwdev, dma_address, size, direction);
}

static inline int
pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
           int nents, int direction)
{
        return nents;
}

static inline void
pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
             int nents, int direction)
{
}

static inline void
pci_dma_sync_sg_for_cpu(struct pci_dev *hwdev, struct scatterlist *sg,
                int nelems, int direction)
{
}

static inline void
pci_dma_sync_sg_for_device(struct pci_dev *hwdev, struct scatterlist *sg,
                int nelems, int direction)
{
}

static inline void
pci_dma_sync_single_for_cpu(struct pci_dev *hwdev, dma_addr_t dma_handle,
                    size_t size, int direction)
{
}

static inline void
pci_dma_sync_single_for_device(struct pci_dev *hwdev, dma_addr_t dma_handle,
                    size_t size, int direction)
{
}

static inline int
pci_dma_mapping_error(struct pci_dev *hwdev, dma_addr_t dma_addr)
{
        return dma_addr == 0;
}
extern void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long max);
extern void pci_iounmap(struct pci_dev *dev, void __iomem *);
#endif

#endif /* _ASM_UBICOM32_PCI_H */
