# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

EMENU:=Other modules

define KernelPackage/crypto
  TITLE:=CryptoAPI modules
  KCONFIG:= \
	$(CONFIG_CRYPTO_HMAC) \
	$(CONFIG_CRYPTO_NULL) \
	$(CONFIG_CRYPTO_MD4) \
	$(CONFIG_CRYPTO_MD5) \
	$(CONFIG_CRYPTO_SHA1) \
	$(CONFIG_CRYPTO_SHA256) \
	$(CONFIG_CRYPTO_SHA512) \
	$(CONFIG_CRYPTO_WP512) \
	$(CONFIG_CRYPTO_TGR192) \
	$(CONFIG_CRYPTO_DES) \
	$(CONFIG_CRYPTO_BLOWFISH) \
	$(CONFIG_CRYPTO_TWOFISH) \
	$(CONFIG_CRYPTO_SERPENT) \
	$(CONFIG_CRYPTO_AES) \
	$(CONFIG_CRYPTO_CAST5) \
	$(CONFIG_CRYPTO_CAST6) \
	$(CONFIG_CRYPTO_TEA) \
	$(CONFIG_CRYPTO_ARC4) \
	$(CONFIG_CRYPTO_KHAZAD) \
	$(CONFIG_CRYPTO_ANUBIS) \
	$(CONFIG_CRYPTO_DEFLATE) \
	$(CONFIG_CRYPTO_MICHAEL_MIC) \
	$(CONFIG_CRYPTO_CRC32C)
  FILES:=$(MODULES_DIR)/kernel/crypto/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
endef
$(eval $(call KernelPackage,crypto))

define KernelPackage/ide-core
  TITLE:=Kernel support for IDE
  DESCRIPTION:=\
	Useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	- ide-core \\\
	- ide-detect \\\
	- ide-disk
  KCONFIG:=$(CONFIG_IDE)
  FILES:=$(MODULES_DIR)/kernel/drivers/ide/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-detect ide-disk)
endef
$(eval $(call KernelPackage,ide-core))

define KernelPackage/ide-pdc202xx
  TITLE:=PDC202xx IDE driver
  KCONFIG:=$(CONFIG_BLK_DEV_PDC202XX_OLD)
  FILES:=$(MODULES_DIR)/kernel/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef
$(eval $(call KernelPackage,ide-pdc202xx))


define KernelPackage/lp
  TITLE:=Parallel port and line printer support
  KCONFIG:=$(CONFIG_PARPORT)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/parport/parport.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/parport/parport_*.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,50, \
  	parport \
  	parport_splink \
  	lp \
  )
endef
$(eval $(call KernelPackage,lp))



