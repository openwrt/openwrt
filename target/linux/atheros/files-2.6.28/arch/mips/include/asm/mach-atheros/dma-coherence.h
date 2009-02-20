/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006  Ralf Baechle <ralf@linux-mips.org>
 * Copyright (C) 2007  Felix Fietkau <nbd@openwrt.org>
 *
 */
#ifndef __ASM_MACH_GENERIC_DMA_COHERENCE_H
#define __ASM_MACH_GENERIC_DMA_COHERENCE_H

#define PCI_DMA_OFFSET	0x20000000

struct device;

static dma_addr_t plat_map_dma_mem(struct device *dev, void *addr, size_t size)
{
	return virt_to_phys(addr) + (dev != NULL ? PCI_DMA_OFFSET : 0);
}

static dma_addr_t plat_map_dma_mem_page(struct device *dev, struct page *page)
{
	return page_to_phys(page) + (dev != NULL ? PCI_DMA_OFFSET : 0);
}

static unsigned long plat_dma_addr_to_phys(dma_addr_t dma_addr)
{
	return (dma_addr > PCI_DMA_OFFSET ? dma_addr - PCI_DMA_OFFSET : dma_addr);
}

static void plat_unmap_dma_mem(dma_addr_t dma_addr)
{
}

static inline int plat_device_is_coherent(struct device *dev)
{
	return 0;
}

#endif /* __ASM_MACH_GENERIC_DMA_COHERENCE_H */
