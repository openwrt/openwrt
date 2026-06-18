/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux driver
  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <linux/log2.h>
#include <linux/uaccess.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/platform_device.h>

#include "ep.h"
#include "aca.h"
#include "misc.h"

#define DC_EP_DBG

#define MAJ	2
#define MIN	1
#define BUILD	0
#define DRV_VERSION __stringify(MAJ) "." __stringify(MIN) "." \
	__stringify(BUILD) "-k"

static bool pcie_switch_exist;
module_param(pcie_switch_exist, bool, 0644);
MODULE_PARM_DESC(pcie_switch_exist, "pcie switch existed or not");

static const char dc_ep_driver_name[] = "vrx518";
static const char dc_ep_driver_version[] = DRV_VERSION;
static const char dc_ep_driver_string[] =
			"Intel(R) SmartPHY DSL(VRX518) PCIe EP/ACA Driver";
static const char dc_ep_copyright[] =
				"Copyright (c) 2016 Intel Corporation.";

static struct dc_ep_info g_dc_ep_info;
static DEFINE_SPINLOCK(dc_ep_lock);

static inline void reset_assert_device(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_ep_assert_device(dev->priv, bits);
}

static inline void reset_deassert_device(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_ep_deassert_device(dev->priv, bits);
}

static inline void icu_disable_intr(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_ep_icu_dis_intr(dev->priv, bits);
}

static inline void icu_enable_intr(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_ep_icu_en_intr(dev->priv, bits);
}

static inline int reset_device(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_reset_device(dev->priv, bits);
}

static inline int clk_on(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_clk_on(dev->priv, bits);
}

static inline int clk_off(struct dc_ep_dev *dev, u32 bits)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_clk_off(dev->priv, bits);
}

static inline int clk_set(struct dc_ep_dev *dev, u32 sysclk, u32 ppeclk)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_clk_set(dev->priv, sysclk, ppeclk);
}

static inline int clk_get(struct dc_ep_dev *dev, u32 *sysclk, u32 *ppeclk)
{
	if (WARN_ON(!dev || !sysclk || !ppeclk))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_clk_get(dev->priv, sysclk, ppeclk);
}

static inline int gpio_dir(struct dc_ep_dev *dev, u32 gpio, int dir)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_dir(dev->priv, gpio, dir);
}

static inline int gpio_set(struct dc_ep_dev *dev, u32 gpio, int val)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_set(dev->priv, gpio, val);
}

static inline int gpio_get(struct dc_ep_dev *dev, u32 gpio, int *val)
{
	if (WARN_ON(!dev || !val))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_get(dev->priv, gpio, val);
}

static inline int pinmux_set(struct dc_ep_dev *dev, u32 gpio, int func)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_pinmux_set(dev->priv, gpio, func);
}

static inline int pinmux_get(struct dc_ep_dev *dev, u32 gpio, int *func)
{
	if (WARN_ON(!dev || !func))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_pinmux_get(dev->priv, gpio, func);
}

static inline int gpio_pupd_set(struct dc_ep_dev *dev, u32 gpio, u32 val)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_pupd_set(dev->priv, gpio, val);
}

static inline int gpio_od_set(struct dc_ep_dev *dev, u32 gpio, int val)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_od_set(dev->priv, gpio, val);
}

static inline int gpio_src_set(struct dc_ep_dev *dev, u32 gpio, int val)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_src_set(dev->priv, gpio, val);
}

static inline int gpio_dcc_set(struct dc_ep_dev *dev, u32 gpio, u32 val)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_ep_gpio_dcc_set(dev->priv, gpio, val);
}

static inline int aca_start(struct dc_ep_dev *dev, u32 func, int start)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_aca_start(dev->priv, func, start);
}

static inline int aca_stop(struct dc_ep_dev *dev, u32 *func, int reset)
{
	if (WARN_ON(!dev || !func))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_aca_stop(dev->priv, func, reset);
}

static inline int aca_init(struct dc_ep_dev *dev, struct aca_param *aca,
	struct aca_modem_param *mdm)
{
	if (WARN_ON(!dev || !aca))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_aca_init(dev->priv, aca, mdm);
}

static inline void aca_event_addr_get(struct dc_ep_dev *dev,
	struct aca_event_reg_addr *regs)
{
	if (WARN_ON(!dev || !regs))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_aca_event_addr_get(dev->priv, regs);
}

static inline u32 umt_msg_addr(struct dc_ep_dev *dev, u32 endian, u32 type)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return aca_umt_msg_addr(dev->priv, endian, type);
}

static inline void aca_txin_sub_ack(struct dc_ep_dev *dev, u32 val)
{
	if (WARN_ON(!dev))
		return;
	if (WARN_ON(!dev->priv))
		return;

	dc_aca_txin_sub_ack(dev->priv, val);
}

static inline u32 aca_txin_hd_cnt(struct dc_ep_dev *dev)
{
	if (WARN_ON(!dev))
		return -EINVAL;
	if (WARN_ON(!dev->priv))
		return -EINVAL;

	return dc_aca_txin_hd_cnt(dev->priv);
}

static const struct aca_hw_ops dc_ep_hw_ops = {
	.reset_assert = reset_assert_device,
	.reset_deassert = reset_deassert_device,
	.reset_device = reset_device,
	.icu_en = icu_enable_intr,
	.icu_mask = icu_disable_intr,
	.clk_on = clk_on,
	.clk_off = clk_off,
	.clk_set = clk_set,
	.clk_get = clk_get,
	.gpio_dir = gpio_dir,
	.gpio_set = gpio_set,
	.gpio_get = gpio_get,
	.pinmux_set = pinmux_set,
	.pinmux_get = pinmux_get,
	.gpio_pupd_set = gpio_pupd_set,
	.gpio_od_set = gpio_od_set,
	.gpio_src_set = gpio_src_set,
	.gpio_dcc_set = gpio_dcc_set,
	.aca_start = aca_start,
	.aca_stop = aca_stop,
	.aca_init = aca_init,
	.aca_event_addr_get = aca_event_addr_get,
	.umt_msg_addr = umt_msg_addr,
	.aca_txin_ack_sub = aca_txin_sub_ack,
	.aca_txin_hd_cnt = aca_txin_hd_cnt,
};

int dc_ep_dev_num_get(int *dev_num)
{
	if ((g_dc_ep_info.dev_num <= 0)
		|| (g_dc_ep_info.dev_num > DC_EP_MAX_NUM))
		return -EIO;

	*dev_num = g_dc_ep_info.dev_num;
	return 0;
}
EXPORT_SYMBOL_GPL(dc_ep_dev_num_get);

int dc_ep_dev_info_req(int dev_idx, enum dc_ep_int module,
			struct dc_ep_dev *dev)
{
	int i;
	struct dc_ep_priv *priv;

	if ((dev_idx < 0) || (dev_idx >= DC_EP_MAX_NUM)) {
		dev_err(dev->dev, "%s invalid device index %d\n",
			__func__, dev_idx);
		return -EIO;
	}

	priv = &g_dc_ep_info.pcie_ep[dev_idx];
	if (atomic_read(&priv->refcnt) >= DC_EP_MAX_REFCNT) {
		dev_err(dev->dev,
			"%s mismatch request/release module usage\n", __func__);
		return -EIO;
	}

	switch (module) {
	case DC_EP_INT_PPE:
		dev->irq = priv->irq_base;
		if (priv->msi_mode == DC_EP_8_MSI_MODE) {
			dev->aca_tx_irq = priv->irq_base + 7;
			dev->aca_rx_irq = priv->irq_base + 6;
		} else if (priv->msi_mode == DC_EP_4_MSI_MODE) {
			dev->aca_tx_irq = priv->irq_base + 2;
			dev->aca_rx_irq = priv->irq_base + 3;
		} else {
			dev_err(dev->dev, "%s ACA should never occur\n",
				__func__);
		}
		break;
	case DC_EP_INT_MEI:
		dev->irq = priv->irq_base + 1;
		break;
	default:
		dev->irq = priv->irq_base;
		break;
	}

	dev->dev = priv->dev;
	dev->membase = priv->mem;
	dev->phy_membase = priv->phymem;
	dev->peer_num = priv->peer_num;
	for (i = 0; i < dev->peer_num; i++) {
		dev->peer_membase[i] = priv->peer_mem[i];
		dev->peer_phy_membase[i] = priv->peer_phymem[i];
	}
	dev->switch_attached = priv->switch_attached;
	dev->priv = priv;
	dev->hw_ops = &dc_ep_hw_ops;
	atomic_inc(&priv->refcnt);
	return 0;
}
EXPORT_SYMBOL_GPL(dc_ep_dev_info_req);

int dc_ep_dev_info_release(int dev_idx)
{
	struct dc_ep_priv *priv;

	if ((dev_idx < 0) || (dev_idx >= DC_EP_MAX_NUM)) {
		pr_err("%s invalid device index %d\n",
			__func__, dev_idx);
		return -EIO;
	}

	priv = &g_dc_ep_info.pcie_ep[dev_idx];
	if (atomic_read(&priv->refcnt) <= 0) {
		pr_err("%s mismatch request/release module usage\n",
			__func__);
		return -EIO;
	}

	atomic_dec(&priv->refcnt);
	return 0;
}
EXPORT_SYMBOL_GPL(dc_ep_dev_info_release);

static int pci_msi_vec_set(struct pci_dev *dev, int nvec)
{
	int pos;
	u16 msgctl;

	if (!is_power_of_2(nvec))
		return -EINVAL;

	pos = pci_find_capability(dev, PCI_CAP_ID_MSI);
	if (!pos)
		return -EINVAL;

	pci_read_config_word(dev, pos + PCI_MSI_FLAGS, &msgctl);
	msgctl &= ~PCI_MSI_FLAGS_QSIZE;
	msgctl |= ((ffs(nvec) - 1) << 4);
	pci_write_config_word(dev, pos + PCI_MSI_FLAGS, msgctl);
	pci_read_config_word(dev, pos + PCI_MSI_FLAGS, &msgctl);
	return 0;
}

static int dc_ep_msi_enable(struct pci_dev *pdev, int nvec)
{
	int err;
	struct dc_ep_priv *priv = pci_get_drvdata(pdev);

	/* NB, ICU initailize first */
	dc_ep_icu_init(priv);

	err = pci_msi_vec_set(pdev, nvec);
	if (err) {
		dev_err(&pdev->dev, "%s: Failed to set maximum MSI vector\n",
			__func__);
		return -EIO;
	}

	err = pci_enable_msi_exact(pdev, nvec);
	if (err) {
		dev_err(&pdev->dev,
			"%s: Failed to enable MSI interrupts error code: %d\n",
			__func__, err);
		return -EIO;
	}
	return 0;
}

static void dc_ep_info_xchange(struct pci_dev *pdev, int card_num)
{
	/* More cards supported, exchange address information
	 * For example, suppose three cards dected.
	 * 0, <1, 2>
	 * 1, <0, 2>
	 * 2, <0, 1>
	 * For four cards detected
	 * 0, <1, 2, 3>
	 * 1, <0, 2, 3>
	 * 2, <0, 1, 3>
	 * 3, <0, 1, 2>
	 * and etc
	 */
	int i, j, k;
	int peer_num;
#ifdef DC_EP_DBG
	struct dc_ep_priv *priv;
#endif /* DC_EP_DBG */
	spin_lock(&dc_ep_lock);
	if (card_num > 1) {
		peer_num = card_num - 1;
		for (i = 0; i < card_num; i++) {
			struct dc_ep_priv *ep = &g_dc_ep_info.pcie_ep[i];
			j = 0;
			k = 0;
			ep->peer_num = peer_num;
			do {
				struct dc_ep_priv *partner;

				if (j == i) {
					j++;
					continue;
				}
				partner = &g_dc_ep_info.pcie_ep[j];
				ep->peer_mem[k] = partner->mem;
				ep->peer_phymem[k] = partner->phymem;
				ep->peer_memsize[k] = partner->memsize;
				k++;
				j++;
			} while ((k < peer_num) && (j < card_num));
		}
	}
	spin_unlock(&dc_ep_lock);

#ifdef DC_EP_DBG
	dev_dbg(&pdev->dev, "Total cards found %d\n", card_num);
	/* Dump detailed debug information */
	for (i = 0; i < card_num; i++) {
		priv = &g_dc_ep_info.pcie_ep[i];
		dev_dbg(&pdev->dev, "card %d attached\n", priv->ep_idx);
		dev_dbg(&pdev->dev, "irq base %d irq numbers %d\n",
			priv->irq_base, priv->irq_num);
		dev_dbg(&pdev->dev,
			"its own phymem 0x%08x mem 0x%p size 0x%08x\n",
			priv->phymem, priv->mem, priv->memsize);
		if (card_num > 1) {
			for (j = 0; j < priv->peer_num; j++)
				dev_dbg(&pdev->dev,
				"its peer phymem 0x%08x mem 0x%p size 0x%08x\n",
				priv->peer_phymem[j],
				priv->peer_mem[j], priv->peer_memsize[j]);
		}
	}
#endif /* DC_EP_DBG */
}

static int pci_msi_vec_num(struct pci_dev *dev)
{
	int ret;
	u16 msgctl;

	if (!dev->msi_cap)
		return -EINVAL;

	pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &msgctl);
	ret = 1 << ((msgctl & PCI_MSI_FLAGS_QMASK) >> 1);

	return ret;
}

static int dc_ep_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int ret;
	int nvec;
	bool switch_exist;
	int current_ep;
	unsigned long phymem;
	void __iomem *mem;
	size_t memsize;
	int msi_mode;
	static int cards_found;
#ifndef CONFIG_OF
	struct pcie_ep_adapter *adapter;
#endif
	struct dc_ep_priv *priv;

	ret = pci_enable_device(pdev);
	if (ret) {
		dev_err(&pdev->dev, "can't enable PCI device %d\n", ret);
		goto err_pci;
	}

	/* Physical address */
	ret = pci_request_region(pdev, DC_EP_BAR_NUM, dc_ep_driver_name);
	if (ret) {
		dev_err(&pdev->dev, "PCI MMIO reservation error: %d\n", ret);
		goto err_device;
	}

	/* Target structures have a limit of 32 bit DMA pointers.
	 * DMA pointers can be wider than 32 bits by default on some systems.
	 */
	ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(&pdev->dev, "32-bit DMA not available: %d\n", ret);
		goto err_region;
	}

	ret = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(&pdev->dev, "cannot enable 32-bit consistent DMA\n");
		goto err_region;
	}

	/* Set bus master bit in PCI_COMMAND to enable DMA */
	pci_set_master(pdev);
	/* NB, some delay may need due to BME reset */
	udelay(1);

	/* Arrange for access to Target SoC registers. */
	mem = pci_iomap(pdev, DC_EP_BAR_NUM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "PCI iomap error\n");
		ret = -EIO;
		goto err_master;
	}
	phymem = pci_resource_start(pdev, DC_EP_BAR_NUM);
	memsize = pci_resource_len(pdev, DC_EP_BAR_NUM);

	nvec = pci_msi_vec_num(pdev);
	/* Overwrite maximum vector number according to
	 * the specific requirement
	 */
	if ((DC_PCIE_SWITCH_ATTACH > 0) || pcie_switch_exist)
		switch_exist = true;
	else
		switch_exist = false;
	/* Always use 4 vector mode */
	nvec = DC_EP_DEFAULT_MSI_VECTOR;
	msi_mode = DC_EP_4_MSI_MODE;

	current_ep = cards_found++;
	priv = &g_dc_ep_info.pcie_ep[current_ep];
	memset(priv, 0, sizeof(*priv));
	pci_set_drvdata(pdev, priv);

	/* Collect basic info for further operations */
	spin_lock(&dc_ep_lock);
	g_dc_ep_info.dev_num = cards_found;
	atomic_set(&priv->refcnt, 0);
	priv->pdev = pdev;
	priv->device_id = pdev->device;
	priv->dev = &pdev->dev;
	priv->ep_idx = current_ep;
	priv->mem = mem;
	priv->phymem = phymem;
	priv->memsize = memsize;
	priv->irq_num = nvec;
	priv->switch_attached = switch_exist;
	priv->msi_mode = msi_mode;
	spin_unlock(&dc_ep_lock);

	ret = dc_ep_msi_enable(pdev, nvec);
	if (ret)
		goto err_iomap;

	spin_lock(&dc_ep_lock);
	priv->irq_base = pdev->irq;
	spin_unlock(&dc_ep_lock);

#ifndef CONFIG_OF
	adapter = kmalloc(sizeof(struct pcie_ep_adapter), GFP_KERNEL);
	if (adapter == NULL)
		goto err_iomap;
	pci_set_drvdata(pdev, adapter);
	adapter->mei_dev = platform_device_register_data(&pdev->dev, "mei_cpe",
							 PLATFORM_DEVID_AUTO,
							 NULL, 0);
	if (IS_ERR(adapter->mei_dev)) {
		dev_err(&pdev->dev, "can not register mei device, err: %li, ignore this\n",
			PTR_ERR(adapter->mei_dev));
		goto err_msi;
	}
#endif
	dc_ep_info_xchange(pdev, cards_found);
	/* Disable output clock to save power */
	dc_ep_clkod_disable(priv);
	dc_aca_info_init(priv);
	return 0;
#ifndef CONFIG_OF
err_msi:
	kfree(adapter);
#endif
err_iomap:
	pci_iounmap(pdev, mem);
err_master:
	pci_clear_master(pdev);
err_region:
	pci_release_region(pdev, DC_EP_BAR_NUM);
err_device:
	pci_disable_device(pdev);
err_pci:
	return ret;
}

static void dc_ep_remove(struct pci_dev *pdev)
{
	struct dc_ep_priv *priv = pci_get_drvdata(pdev);

#ifndef CONFIG_OF
	struct pcie_ep_adapter *adapter =
		(struct pcie_ep_adapter *) pci_get_drvdata(pdev);

	platform_device_unregister(adapter->mei_dev);
#endif
	if (priv == NULL)
		return;

	if (atomic_read(&priv->refcnt) != 0) {
		dev_err(&pdev->dev, "%s still being used, can't remove\n",
			__func__);
		return;
	}
	dc_aca_free_fw_file(priv);
	dc_aca_shutdown(priv);
	dc_ep_icu_disable(priv);
	pci_iounmap(pdev, priv->mem);
	pci_release_region(pdev, DC_EP_BAR_NUM);
	pci_disable_msi(pdev);
	wmb();
	pci_clear_master(pdev);
	pci_disable_device(pdev);
}

static const struct pci_device_id dc_ep_id_table[] = {
	{0x8086, 0x09a9, PCI_ANY_ID, PCI_ANY_ID}, /* VRX518 */
	{0},
};

MODULE_DEVICE_TABLE(pci, dc_ep_id_table);

static struct pci_driver dc_ep_driver = {
	.name = (char *)dc_ep_driver_name,
	.id_table = dc_ep_id_table,
	.probe = dc_ep_probe,
	.remove = dc_ep_remove,
	.shutdown = dc_ep_remove,
	/* PM not supported */
	/* AER is controlled by RC */
};

static int __init dc_ep_init(void)
{
	pr_info("%s - version %s\n",
		dc_ep_driver_string, dc_ep_driver_version);

	pr_info("%s\n", dc_ep_copyright);
	memset(&g_dc_ep_info, 0, sizeof(struct dc_ep_info));

	if (pci_register_driver(&dc_ep_driver) < 0) {
		pr_err("%s: No devices found, driver not installed.\n",
			__func__);
		return -ENODEV;
	}
	return 0;
}
module_init(dc_ep_init);

static void __exit dc_ep_exit(void)
{
	pci_unregister_driver(&dc_ep_driver);

	pr_info("%s: %s driver unloaded\n", __func__,
		dc_ep_driver_name);
}
module_exit(dc_ep_exit);

MODULE_AUTHOR("Intel Corporation, <Chuanhua.lei@intel.com>");
MODULE_DESCRIPTION("Intel(R) SmartPHY PCIe EP/ACA Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
