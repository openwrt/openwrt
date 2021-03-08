# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) Jiang Yutang <jiangyutang1978@gmail.com>

define KernelPackage/ahci-qoriq
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Freescale QorIQ AHCI SATA support
  KCONFIG:=CONFIG_AHCI_QORIQ
  FILES:=$(LINUX_DIR)/drivers/ata/ahci_qoriq.ko
  AUTOLOAD:=$(call AutoLoad,40,ahci-qoriq,1)
  $(call AddDepends/ata,+kmod-ata-ahci-platform @TARGET_layerscape)
endef

define KernelPackage/ahci-qoriq/description
 This option enables support for the Freescale QorIQ AHCI SoC's
 onboard AHCI SATA.
endef

$(eval $(call KernelPackage,ahci-qoriq))

define KernelPackage/ppfe
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  TITLE:=Freescale PPFE Driver support
  DEPENDS:=@TARGET_layerscape
  KCONFIG:=CONFIG_FSL_PPFE=y \
  	CONFIG_FSL_PPFE_UTIL_DISABLED=y
  FILES:=$(LINUX_DIR)/drivers/staging/fsl_ppfe/pfe.ko
  AUTOLOAD:=$(call AutoLoad,35,pfe)
endef

define KernelPackage/ppfe/description
 Kernel modules for Freescale PPFE Driver support.
endef

$(eval $(call KernelPackage,ppfe))

define KernelPackage/crypto-hw-caam
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Freescale CAAM
  DEPENDS:=@TARGET_layerscape_armv8_64b +kmod-crypto-aead +kmod-crypto-authenc +kmod-crypto-hash \
           +kmod-crypto-rsa +kmod-crypto-rng +kmod-crypto-des +kmod-random-core
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM \
	CONFIG_CRYPTO_DEV_FSL_CAAM_DEBUG=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_JR \
	CONFIG_CRYPTO_DEV_FSL_CAAM_RINGSIZE=9 \
	CONFIG_CRYPTO_DEV_FSL_CAAM_INTC=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_CRYPTO_API=y \
	CONFIG_CRYPTO_BLKCIPHER=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM_CRYPTO_API_QI=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM_AHASH_API=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM_PKC_API=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM_RNG_API=y \
	CONFIG_CRYPTO_DEV_FSL_CAAM_TK_API=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_RNG_TEST=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_SM=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_SECVIO=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_JR_UIO=n \
	CONFIG_CRYPTO_DEV_FSL_CAAM_DMA=n \
	CONFIG_CRYPTO_DEV_FSL_DPAA2_CAAM
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/caam/error.ko \
	$(LINUX_DIR)/drivers/crypto/caam/caam.ko \
	$(LINUX_DIR)/drivers/crypto/caam/caam_jr.ko \
	$(LINUX_DIR)/drivers/crypto/caam/caamalg_desc.ko \
	$(LINUX_DIR)/drivers/crypto/caam/caamhash_desc.ko \
	$(LINUX_DIR)/drivers/crypto/caam/dpaa2_caam.ko
  AUTOLOAD:=$(call AutoLoad,09,error caam caam_jr caamalg_desc caamhash_desc dpaa2_caam)
endef

$(eval $(call KernelPackage,crypto-hw-caam))
