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
  SUBMENU:=$(EMENU)
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
	$(MODULES_DIR)/kernel/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,50, \
  	parport \
  	lp \
  )
endef
$(eval $(call KernelPackage,lp))


define KernelPackage/soundcore
  TITLE:=Sound support
  DESCRIPTION:=Kernel modules for sound support
  KCONFIG:=$(CONFIG_SOUND)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,soundcore)
endef

define KernelPackage/soundcore/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/sound/soundcore.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/soundcore/2.6
  FILES:=$(MODULES_DIR)/kernel/sound/soundcore.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,soundcore))


define KernelPackage/loop
  TITLE:=Loopback device support
  DESCRIPTION:=Kernel module for loopback device support
  KCONFIG:=$(CONFIG_BLK_DEV_LOOP)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,loop)
  FILES:=$(MODULES_DIR)/kernel/drivers/block/loop.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,loop))

define KernelPackage/nbd
  TITLE:=Network block device support
  DESCRIPTION:=Kernel module for network block device support
  KCONFIG:=$(CONFIG_BLK_DEV_NBD)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,nbd)
  FILES:=$(MODULES_DIR)/kernel/drivers/block/nbd.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,nbd))


