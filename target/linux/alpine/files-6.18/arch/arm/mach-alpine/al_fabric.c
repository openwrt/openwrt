// SPDX-License-Identifier: GPL-2.0-or-later
/* Annapurna labs fabric. */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include <linux/dma-map-ops.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <asm/pgtable.h>
#include <mach/al_fabric.h>
#include "al_hal_nb_regs.h"
#include "al_init_sys_fabric.h"

static struct of_device_id of_nb_table[] = {
	{.compatible = "annapurna-labs,al-nb-service"},
	{ /* end of list */ },
};

static struct of_device_id of_ccu_table[] = {
	{.compatible = "annapurna-labs,al-ccu"},
	{ /* end of list */ },
};

int al_fabric_hwcc;

static int al_fabric_plat_device_notifier(struct notifier_block *nb,
				       unsigned long event, void *__dev)
{
	struct device *dev = __dev;

	if (event != BUS_NOTIFY_ADD_DEVICE)
		return NOTIFY_DONE;

	dma_set_coherent_mask(dev, PHYS_MASK);

	if (!al_fabric_hwcc)
		return NOTIFY_OK;

	dev->dma_coherent = true;

	return NOTIFY_OK;
}

static struct notifier_block al_fabric_plat_device_nb = {
	.notifier_call = al_fabric_plat_device_notifier,
};

static int al_fabric_pci_device_notifier(struct notifier_block *nb,
				       unsigned long event, void *__dev)
{
	struct device *dev = __dev;
	struct pci_dev *pdev = to_pci_dev(dev);
	u32 temp;

	if (event != BUS_NOTIFY_BIND_DRIVER)
		return NOTIFY_DONE;

	dma_set_coherent_mask(dev, PHYS_MASK);

	if (!al_fabric_hwcc)
		return NOTIFY_OK;

	dev->dma_coherent = true;

	/* Force the PCIE adapter to set AXI attr to match CC*/
	if(pci_domain_nr(pdev->bus) == 0) {
		pci_read_config_dword(pdev, 0x110 ,&temp);
		temp |= 0x3;
		pci_write_config_dword(pdev, 0x110 ,temp);
		/* Enable cache coherency for VF's (except USB and SATA) */
		if (PCI_SLOT(pdev->devfn) < 6) {
			pci_write_config_dword(pdev, 0x130 ,temp);
			pci_write_config_dword(pdev, 0x150 ,temp);
			pci_write_config_dword(pdev, 0x170 ,temp);
		}

		pci_read_config_dword(pdev, 0x220 ,&temp);
		temp &= ~0xffff;
		temp |= 0x3ff;
		pci_write_config_dword(pdev, 0x220 ,temp);
	}

	return NOTIFY_OK;
}

static struct notifier_block al_fabric_pci_device_nb = {
	.notifier_call = al_fabric_pci_device_notifier,
};

int al_fabric_hwcc_enabled(void)
{
	return al_fabric_hwcc;
}
EXPORT_SYMBOL(al_fabric_hwcc_enabled);

int __init al_fabric_init(void)
{
	struct device_node *ccu_node;
	struct device_node *nb_node;
	void __iomem *ccu_address;
	void __iomem *nb_service_base_address;
	u32 prop;
	bool dev_ord_relax;

	pr_info("Initializing System Fabric\n");

	nb_node = of_find_matching_node(NULL, of_nb_table);
	ccu_node = of_find_matching_node(NULL, of_ccu_table);

	if (!nb_node || !ccu_node)
		return -EINVAL;

	ccu_address = of_iomap(ccu_node, 0);
	BUG_ON(!ccu_address);

	dev_ord_relax =
		!of_property_read_u32(nb_node, "dev_ord_relax", &prop) && prop;

	if (of_device_is_available(ccu_node)) {
		al_fabric_hwcc = !of_property_read_u32(ccu_node, "io_coherency", &prop)
			&& prop;

		if (al_fabric_hwcc)
			pr_info("al_fabric: enabling I/O cache coherency\n");

		al_ccu_init(ccu_address, al_fabric_hwcc);

		bus_register_notifier(&platform_bus_type,
					&al_fabric_plat_device_nb);
		bus_register_notifier(&pci_bus_type,
					&al_fabric_pci_device_nb);
	}

	nb_service_base_address = of_iomap(nb_node, 0);
	BUG_ON(!nb_service_base_address);
	al_nbservice_init(nb_service_base_address, dev_ord_relax);

	return 0;
}
