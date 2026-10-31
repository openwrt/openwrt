/*
 * drivers/crypto/al_crypto_main.c
 *
 * Annapurna Labs Crypto driver - pci enumeration and init invocation
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
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

#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <mach/al_hal_unit_adapter_regs.h>

#include "al_crypto.h"
#include "al_crypto_module_params.h"

MODULE_VERSION(AL_CRYPTO_VERSION);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Annapurna Labs");
MODULE_DESCRIPTION("Annapurna Labs crypto acceleration driver");

#define DRV_NAME "al_crypto"
#define MAX_HW_DESCS_PER_SW_DECS 4

enum {
  /* BAR's are enumerated in terms of pci_resource_start() terms */
  AL_CRYPTO_UDMA_BAR = 0,
  AL_CRYPTO_APP_BAR = 4
};

static int al_crypto_pci_probe(struct pci_dev *pdev,
                               const struct pci_device_id *id);

static void al_crypto_pci_remove(struct pci_dev *pdev);

static void al_crypto_flr(struct pci_dev *pdev);

static const struct pci_device_id al_crypto_pci_tbl[] = {
    {
        PCI_VDEVICE(AMAZON_ANNAPURNA_LABS,
                    PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_CRYPTO),
    },
    {
        PCI_VDEVICE(AMAZON_ANNAPURNA_LABS,
                    PCI_DEVICE_ID_AMAZON_ANNAPURNA_LABS_CRYPTO_VF),
    },
    {
        0,
    }};
MODULE_DEVICE_TABLE(pci, al_crypto_pci_tbl);

static struct pci_driver al_crypto_pci_driver = {
    .name = DRV_NAME,
    .id_table = al_crypto_pci_tbl,
    .probe = al_crypto_pci_probe,
    .remove = al_crypto_pci_remove,
};

/******************************************************************************
 *****************************************************************************/
static int al_crypto_pci_probe(struct pci_dev *pdev,
                               const struct pci_device_id *id) {
  int status = 0;
  int sriov_crc_channels = al_crypto_get_crc_channels();
  void __iomem *const *iomap;
  struct device *dev = &pdev->dev;
  struct al_crypto_device *device;
  int bar_reg;

  dev_dbg(dev, "%s(%p, %p)\n", __func__, pdev, id);

  if (min(al_crypto_get_rx_descs_order(), al_crypto_get_tx_descs_order()) <
      (MAX_HW_DESCS_PER_SW_DECS + al_crypto_get_ring_alloc_order())) {
    dev_err(dev,
            "%s: Too small HW Q can lead to unexpected behavior "
            "upon queue overflow\n",
            __func__);
  }

  al_crypto_flr(pdev);

  status = pcim_enable_device(pdev);
  if (status) {
    pr_err("%s: pcim_enable_device failed!\n", __func__);
    goto done;
  }

  bar_reg = pdev->is_physfn
                ? (1 << AL_CRYPTO_UDMA_BAR) | (1 << AL_CRYPTO_APP_BAR)
                : (1 << AL_CRYPTO_UDMA_BAR);

  status = pcim_iomap_regions(pdev, bar_reg, DRV_NAME);
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

  device = devm_kzalloc(dev, sizeof(struct al_crypto_device), GFP_KERNEL);
  if (!device) {
    status = -ENOMEM;
    goto done;
  }

  device->pdev = pdev;

  pci_set_master(pdev);
  pci_set_drvdata(pdev, device);
  dev_set_drvdata(dev, device);

#ifdef CONFIG_BTRFS_AL_FAST_CRC_DMA
  BUG_ON(!al_crypto_get_use_virtual_function());
#endif
  /*
   * When VF is used the PF is dedicated to crc and the VF is dedicated
   * to crypto
   */
  if (al_crypto_get_use_virtual_function()) {
    if (pdev->is_physfn && (pci_sriov_get_totalvfs(pdev) > 0))
      sriov_crc_channels = 0;
    else if (pdev->is_virtfn) {
      sriov_crc_channels = al_crypto_get_max_channels();
#ifdef CONFIG_BTRFS_AL_FAST_CRC_DMA
      BUG_ON(sriov_crc_channels < NR_CPUS);
#endif
    }
  }

  device->max_channels = al_crypto_get_max_channels();
  device->crc_channels = sriov_crc_channels;

  if (al_crypto_get_use_virtual_function() && pdev->is_physfn &&
      (pci_sriov_get_totalvfs(pdev) > 0)) {
    status = pci_enable_sriov(pdev, 1);
    if (status) {
      dev_err(dev, "%s: pci_enable_sriov failed, status %d\n", __func__,
              status);
      goto done;
    }
  }

  status =
      al_crypto_core_init(device, iomap[AL_CRYPTO_UDMA_BAR],
                          pdev->is_physfn ? iomap[AL_CRYPTO_APP_BAR] : NULL);
  if (status) {
    dev_err(dev, "%s: al_crypto_core_init failed\n", __func__);
    goto done;
  }

  status = al_crypto_sysfs_init(device);
  if (status) {
    dev_err(dev, "%s: al_dma_sysfs_init failed\n", __func__);
    goto err_sysfs_init;
  }

  if (device->crc_channels < device->max_channels) {
    status = al_crypto_skcipher_init(device);
    if (status) {
      dev_err(dev, "%s: al_crypto_skcipher_init failed\n", __func__);
      goto err_skcipher_init;
    }

    status = al_crypto_aead_init(device);
    if (status) {
      dev_err(dev, "%s: al_crypto_aead_init failed\n", __func__);
      goto err_aead_init;
    }

    status = al_crypto_hash_init(device);
    if (status) {
      dev_err(dev, "%s: al_crypto_hash_init failed\n", __func__);
      goto err_hash_init;
    }
  } else
    dev_info(dev,
             "%s: Skipping skcipher/aead/hash initialization, "
             "no allocated channels\n",
             __func__);

  if (device->crc_channels > 0) {
    status = al_crypto_crc_init(device);
    if (status) {
      dev_err(dev, "%s: al_crypto_hash_init failed\n", __func__);
      goto err_crc_init;
    }
  } else
    dev_info(dev,
             "%s: Skipping crc initialization, "
             "no allocated channels\n",
             __func__);

  goto done;

err_crc_init:
  al_crypto_hash_terminate(device);
err_hash_init:
  al_crypto_skcipher_terminate(device);
err_aead_init:
  al_crypto_aead_terminate(device);
err_skcipher_init:
  al_crypto_sysfs_terminate(device);
err_sysfs_init:
  al_crypto_core_terminate(device);
done:
  return status;
}

static int al_crypto_read_pcie_config(void *handle, int where, uint32_t *val) {
  /* handle is a pointer to the pci_dev */
  pci_read_config_dword((struct pci_dev *)handle, where, val);
  return 0;
}

static int al_crypto_write_pcie_config(void *handle, int where, uint32_t val) {
  /* handle is a pointer to the pci_dev */
  pci_write_config_dword((struct pci_dev *)handle, where, val);
  return 0;
}

static int al_crypto_write_pcie_flr(void *handle) {
  /* handle is a pointer to the pci_dev */
  __pci_reset_function_locked((struct pci_dev *)handle);
  udelay(1000);
  return 0;
}

/******************************************************************************
 *****************************************************************************/
static inline void al_crypto_flr(struct pci_dev *pdev) {
  al_pcie_perform_flr(al_crypto_read_pcie_config, al_crypto_write_pcie_config,
                      al_crypto_write_pcie_flr, pdev);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_pci_remove(struct pci_dev *pdev) {
  struct al_crypto_device *device = pci_get_drvdata(pdev);

  if (!device)
    return;

  dev_dbg(&pdev->dev, "Removing dma\n");

  if (device->pdev->is_physfn)
    pci_disable_sriov(device->pdev);

  al_crypto_crc_terminate(device);

  al_crypto_hash_terminate(device);

  al_crypto_sysfs_terminate(device);

  al_crypto_aead_terminate(device);

  al_crypto_skcipher_terminate(device);

  al_crypto_core_terminate(device);

  pci_disable_device(pdev);
}

/******************************************************************************
 *****************************************************************************/
static int __init al_crypto_init_module(void) {
  int err;

  pr_info("%s: Annapurna Labs Crypto Driver %s\n", DRV_NAME, AL_CRYPTO_VERSION);

  err = pci_register_driver(&al_crypto_pci_driver);

  return err;
}
module_init(al_crypto_init_module);

/******************************************************************************
 *****************************************************************************/
static void __exit al_crypto_exit_module(void) {
  pci_unregister_driver(&al_crypto_pci_driver);
}
module_exit(al_crypto_exit_module);
