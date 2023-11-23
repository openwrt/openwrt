/******************************************************************************
 *
 * Copyright(c) 2019 -  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include "drv_types.h"

void pci_cache_wback(struct pci_dev *hwdev,
			dma_addr_t *bus_addr, size_t size, int direction)
{
	if (NULL != hwdev && NULL != bus_addr)
	  	dma_sync_single_for_device(&hwdev->dev, *bus_addr, size,
					direction);
	else
		RTW_ERR("pcie hwdev handle or bus addr is NULL!\n");
}
void pci_cache_inv(struct pci_dev *hwdev,
			dma_addr_t *bus_addr, size_t size, int direction)
{
	if (NULL != hwdev && NULL != bus_addr)
		dma_sync_single_for_cpu(&hwdev->dev, *bus_addr, size, direction);
	else
		RTW_ERR("pcie hwdev handle or bus addr is NULL!\n");
}
void pci_get_bus_addr(struct pci_dev *hwdev,
			void *vir_addr, dma_addr_t *bus_addr,
			size_t size, int direction)
{
	if (NULL != hwdev) {
		*bus_addr = dma_map_single(&hwdev->dev, vir_addr, size, direction);
	} else {
		RTW_ERR("pcie hwdev handle is NULL!\n");
		*bus_addr = (dma_addr_t)virt_to_phys(vir_addr);
		/*RTW_ERR("Get bus_addr: %x by virt_to_phys()\n", bus_addr);*/
	}
}
void pci_unmap_bus_addr(struct pci_dev *hwdev,
			dma_addr_t *bus_addr, size_t size, int direction)
{
	if (NULL != hwdev && NULL != bus_addr)
		dma_unmap_single(&hwdev->dev, *bus_addr, size, direction);
	else
		RTW_ERR("pcie hwdev handle or bus addr is NULL!\n");
}
void *pci_alloc_cache_mem(struct pci_dev *pdev,
			dma_addr_t *bus_addr, size_t size, int direction)
{
	void *vir_addr = NULL;

	vir_addr = rtw_zmalloc(size);

	if (!vir_addr)
		bus_addr = NULL;
	else
		pci_get_bus_addr(pdev, vir_addr, bus_addr, size, direction);

	return vir_addr;
}
void *pci_alloc_noncache_mem(struct pci_dev *pdev,
			dma_addr_t *bus_addr, size_t size)
{
	void *vir_addr = NULL;

	if (NULL != pdev)
		vir_addr = dma_alloc_coherent(&pdev->dev,
				size, bus_addr,
				(in_atomic() ? GFP_ATOMIC : GFP_KERNEL));
	if (!vir_addr)
		bus_addr = NULL;
	else
		bus_addr = (dma_addr_t *)((((SIZE_PTR)bus_addr + 3) / 4) * 4);

	return vir_addr;
}
void pci_free_cache_mem(struct pci_dev *pdev,
		void *vir_addr, dma_addr_t *bus_addr,
		size_t size, int direction)
{
	pci_unmap_bus_addr(pdev, bus_addr, size, direction);
	rtw_mfree(vir_addr, size);

	vir_addr = NULL;
}
void pci_free_noncache_mem(struct pci_dev *pdev,
		void *vir_addr, dma_addr_t *bus_addr, size_t size)
{
	if (NULL != pdev)
		dma_free_coherent(&pdev->dev, size, vir_addr, *bus_addr);
	vir_addr = NULL;
}
