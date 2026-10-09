/*
 * Annapurna Labs DMA Linux driver
 * Copyright(c) 2011 Annapurna Labs.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include "al_dma.h"
#include "al_dma_sysfs.h"

MODULE_VERSION(AL_DMA_VERSION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annapurna Labs");

#define DRV_NAME "al_dma"

enum {
	/* BAR's are enumerated in terms of pci_resource_start() terms */
	AL_DMA_UDMA_BAR		= 0,
	AL_DMA_APP_BAR		= 4,
};

static int al_dma_pci_probe(
	struct pci_dev			*pdev,
	const struct pci_device_id	*id);

static void al_dma_pci_remove(
	struct pci_dev	*pdev);

static void al_dma_pci_shutdown(
	struct pci_dev	*pdev);

static const struct pci_device_id al_dma_pci_tbl[] = {
	{ PCI_VDEVICE(AMAZON_ANNAPURNA_LABS, PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_RAID_DMA) },
	{ PCI_VDEVICE(AMAZON_ANNAPURNA_LABS, PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_RAID_DMA_VF) },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, al_dma_pci_tbl);

static struct pci_driver al_dma_pci_driver = {
	.name		= DRV_NAME,
	.id_table	= al_dma_pci_tbl,
	.probe		= al_dma_pci_probe,
	.remove		= al_dma_pci_remove,
	.shutdown	= al_dma_pci_shutdown,
};

/******************************************************************************
 *****************************************************************************/
static int al_dma_pci_probe(
	struct pci_dev			*pdev,
	const struct pci_device_id	*id)
{
	int status = 0;

	void __iomem * const *iomap;
	struct device *dev = &pdev->dev;
	struct al_dma_device *device;
	int bar_reg;
	u16 dev_id;
	u8 rev_id;

	dev_dbg(dev, "%s(%p, %p)\n", __func__, pdev, id);

	pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);
	pci_read_config_byte(pdev, PCI_REVISION_ID, &rev_id);

	/* Perform FLR on a related function */
	al_dma_flr(pdev);

	status = pcim_enable_device(pdev);
	if (status) {
		pr_err("%s: pcim_enable_device failed!\n", __func__);
		goto done;
	}

	bar_reg = pdev->is_physfn ?
		(1 << AL_DMA_UDMA_BAR) | (1 << AL_DMA_APP_BAR) :
		(1 << AL_DMA_UDMA_BAR);

	status = pcim_iomap_regions(
		pdev,
		bar_reg,
		DRV_NAME);
	if (status) {
		pr_err("%s: pcim_iomap_regions failed!\n", __func__);
		goto done;
	}

	iomap = pcim_iomap_table(pdev);
	if (!iomap) {
		status = -ENOMEM;
		goto done;
	}

	status = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(40));
	if (status)
		goto done;

	device = devm_kzalloc(dev, sizeof(struct al_dma_device), GFP_KERNEL);
	if (!device) {
		status = -ENOMEM;
		goto done;
	}

	device->pdev = pdev;
	device->dev_id = dev_id;
	device->rev_id = rev_id;

	pci_set_master(pdev);
	pci_set_drvdata(pdev, device);
	dev_set_drvdata(dev, device);

	device->common.dev = &pdev->dev;

#ifdef CONFIG_AL_DMA_PCI_IOV
	if (PCI_FUNC(pdev->devfn) == 0) {
		status = pci_enable_sriov(pdev, 1);
		if (status) {
			dev_err(dev, "%s: pci_enable_sriov failed, status %d\n",
					__func__, status);
		}
	}
#endif

	if (pdev->is_physfn) {
		status = al_dma_core_init(
			device,
			iomap[AL_DMA_UDMA_BAR],
			iomap[AL_DMA_APP_BAR]);
		if (status) {
			dev_err(dev, "%s: al_dma_core_init failed\n", __func__);
			goto done;
		}

		status = al_dma_sysfs_init(dev);
		if (status) {
			dev_err(dev, "%s: al_dma_sysfs_init failed\n", __func__);
			goto err_sysfs_init;
		}
	}
	else {
		status = al_dma_fast_init(
				device,
				iomap[AL_DMA_UDMA_BAR]);
		if (status) {
			dev_err(dev, "%s: al_dma_fast_init failed\n", __func__);
			goto done;
		}
	}

	goto done;

err_sysfs_init:
	al_dma_core_terminate(device);

done:
	return status;
}

/******************************************************************************
 *****************************************************************************/
static void al_dma_pci_remove(struct pci_dev *pdev)
{
	struct al_dma_device *device = pci_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	if (!device)
		return;

	dev_dbg(&pdev->dev, "Removing dma\n");

	if (pdev->is_physfn) {
		al_dma_sysfs_terminate(dev);

		al_dma_core_terminate(device);
	} else {
		al_dma_fast_terminate(device);
	}

}

static void al_dma_pci_shutdown(struct pci_dev *pdev)
{
	/* Don't call for physfn as its removal is not fully implement yet */
	if (!pdev->is_physfn)
		al_dma_pci_remove(pdev);
}

/******************************************************************************
 *****************************************************************************/
static int __init al_dma_init_module(void)
{
	int err;

	pr_info(
		"%s: Annapurna Labs DMA Driver %s\n",
		DRV_NAME,
		AL_DMA_VERSION);

	err = pci_register_driver(&al_dma_pci_driver);

	return err;
}
module_init(al_dma_init_module);

/******************************************************************************
 *****************************************************************************/
static void __exit al_dma_exit_module(void)
{
	pci_unregister_driver(&al_dma_pci_driver);
}
module_exit(al_dma_exit_module);
