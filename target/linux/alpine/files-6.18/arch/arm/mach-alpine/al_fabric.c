/*
 * Annapurna labs fabric.
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include <linux/dma-map-ops.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/irqchip/chained_irq.h>
#include <asm/pgtable.h>
#include <mach/al_fabric.h>
#include "al_hal_nb_regs.h"
#include "al_init_sys_fabric.h"

static struct of_device_id of_fabric_table[] = {
	{.compatible = "annapurna-labs,al-fabric"},
	{ /* end of list */ },
};

static struct of_device_id of_nb_table[] = {
	{.compatible = "annapurna-labs,al-nb-service"},
	{ /* end of list */ },
};

static struct of_device_id of_ccu_table[] = {
	{.compatible = "annapurna-labs,al-ccu"},
	{ /* end of list */ },
};

struct sys_fabric_irq_struct {
	unsigned int		idx;
	void __iomem		*regs_base;
	unsigned int		irq_cause_base;
	struct irq_chip_generic	*irq_gc;
};

static struct sys_fabric_irq_struct	sf_irq_arr[AL_FABRIC_INSTANCE_N];

int al_fabric_hwcc;

static int al_fabric_plat_device_notifier(struct notifier_block *nb,
				       unsigned long event, void *__dev)
{
	struct device *dev = __dev;

	if (event != BUS_NOTIFY_ADD_DEVICE)
		return NOTIFY_DONE;

#ifdef CONFIG_ARM_HWCC_FLAG
	dev->archdata.hwcc = al_fabric_hwcc;
#endif
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

#ifdef CONFIG_ARM_HWCC_FLAG
	dev->archdata.hwcc = al_fabric_hwcc;
#endif
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

static void sf_irq_handler(struct irq_desc *desc)
{
	unsigned long pending, mask;
	int offset;
	struct sys_fabric_irq_struct *chip = irq_desc_get_handler_data(desc);
	struct irq_chip *irqchip = irq_desc_get_chip(desc);
	struct al_nb_regs *nb_regs = chip->regs_base;

	chained_irq_enter(irqchip, desc);

	mask = chip->irq_gc->mask_cache;
	pending = readl(&nb_regs->global.nb_int_cause) & mask;

	/* deassert pending edge-triggered irqs */
	writel(~(pending & ~NB_GLOBAL_NB_INT_CAUSE_LEVEL_IRQ_MASK),
			&nb_regs->global.nb_int_cause);

	/* handle pending irqs */
	if (likely(pending)) {
		int fabric_irq_base = al_fabric_get_cause_irq(chip->idx, 0);
		for_each_set_bit(offset, &pending, AL_FABRIC_IRQ_N)
			generic_handle_irq(fabric_irq_base + offset);
	}

	/* deassert pending level-triggered irqs */
	writel(~(pending & NB_GLOBAL_NB_INT_CAUSE_LEVEL_IRQ_MASK),
			&nb_regs->global.nb_int_cause);

	chained_irq_exit(irqchip, desc);
}

static void init_sf_irq_gc(struct sys_fabric_irq_struct *sfi)
{
	struct irq_chip_type *ct;

	sfi->irq_gc = irq_alloc_generic_chip("alpine_sf_irq", 1,
			sfi->irq_cause_base, sfi->regs_base, handle_simple_irq);
	sfi->irq_gc->private = sfi;

	ct = sfi->irq_gc->chip_types;
	ct->chip.irq_mask = irq_gc_mask_clr_bit;
	ct->chip.irq_unmask = irq_gc_mask_set_bit;
	ct->regs.mask = offsetof(struct al_nb_regs,
			cpun_config_status[sfi->idx].local_cause_mask);

	/* clear the no request field so irq can be requested */
	irq_setup_generic_chip(sfi->irq_gc, IRQ_MSK(AL_FABRIC_IRQ_N),
			IRQ_GC_INIT_MASK_CACHE, IRQ_NOREQUEST, 0);
}

static int init_sf_irq_struct(struct sys_fabric_irq_struct *sfi_arr,
		unsigned int idx, void __iomem *regs_base)
{
	int ret;

	pr_debug("[%s] entered with idx = %d, regs_base = %p\n",
			__func__, idx, regs_base);
	sfi_arr[idx].idx = idx;
	sfi_arr[idx].regs_base = regs_base;
	/* allocate irq descriptors for the cause interrupts */
	ret = irq_alloc_descs(-1, 0, AL_FABRIC_IRQ_N, -1);
	if (ret < 0) {
		pr_err("[%s] Failed to allocate IRQ descriptors\n", __func__);
		return ret;
	}
	sfi_arr[idx].irq_cause_base = ret;
	init_sf_irq_gc(&sfi_arr[idx]);
	return 0;
}

int al_fabric_get_cause_irq(unsigned int idx, int irq)
{
	return sf_irq_arr[idx].irq_cause_base + irq;
}

int al_fabric_hwcc_enabled(void)
{
	return al_fabric_hwcc;
}
EXPORT_SYMBOL(al_fabric_hwcc_enabled);

int __init al_fabric_init(void)
{
	struct device_node *ccu_node;
	struct device_node *nb_node;
	void __iomem *nb_base_address;
	void __iomem *ccu_address;
	void __iomem *nb_service_base_address;
	u32 prop;
	bool dev_ord_relax;
	int nb_serv_irq[AL_FABRIC_INSTANCE_N];
	int i, ret;

	pr_info("Initializing System Fabric\n");

	nb_node = of_find_matching_node(NULL, of_nb_table);
	ccu_node = of_find_matching_node(NULL, of_ccu_table);


	if (!nb_node)
		return -EINVAL;

	if (ccu_node) {
		/* new devicetree */
		ccu_address = of_iomap(ccu_node, 0);
		BUG_ON(!ccu_address);

		dev_ord_relax =
			!of_property_read_u32(nb_node, "dev_ord_relax", &prop)
			&& prop;
	} else {
		/* old devicetree */
		ccu_node = of_find_matching_node(NULL, of_fabric_table);
		if (!ccu_node)
			return -EINVAL;
		nb_base_address = of_iomap(ccu_node, 0);
		BUG_ON(!nb_base_address);
		ccu_address = nb_base_address + 0x90000;

		dev_ord_relax =
			!of_property_read_u32(ccu_node, "dev_ord_relax", &prop)
			&& prop;
	}
	if (ccu_node && of_device_is_available(ccu_node)) {
		al_fabric_hwcc = !of_property_read_u32(ccu_node, "io_coherency", &prop)
			&& prop;

		if (al_fabric_hwcc)
			printk("Enabling IO Cache Coherency.\n");

		al_ccu_init(ccu_address, al_fabric_hwcc);

		bus_register_notifier(&platform_bus_type,
					&al_fabric_plat_device_nb);
		bus_register_notifier(&pci_bus_type,
					&al_fabric_pci_device_nb);
	}

	if (nb_node) {
		nb_service_base_address = of_iomap(nb_node, 0);
		BUG_ON(!nb_service_base_address);
		al_nbservice_init(nb_service_base_address, dev_ord_relax);

		for (i = 0 ; i < AL_FABRIC_INSTANCE_N ; ++i) {
			ret = init_sf_irq_struct(sf_irq_arr, i,
					nb_service_base_address);
			if (ret < 0) {
				pr_err("[%s] Failed to initialize sys-fabric "
					"irq struct\n", __func__);
				return ret;
			}
			nb_serv_irq[i] = irq_of_parse_and_map(nb_node, i);
			irq_set_chained_handler(nb_serv_irq[i], sf_irq_handler);
			ret = irq_set_handler_data(nb_serv_irq[i],
					&sf_irq_arr[i]);
			if (ret < 0) {
				pr_err("[%s] Failed to set irq handler data\n"
					, __func__);
				return ret;
			}
		}

	}

	return 0;
}

