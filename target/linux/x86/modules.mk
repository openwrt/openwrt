#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/cs5535
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=NSC/AMD CS5535 chipset support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_BLK_DEV_CS5535
  FILES=$(LINUX_DIR)/drivers/ide/cs5535.ko
  AUTOLOAD:=$(call AutoLoad,30,cs5535,1)
  $(call AddDepends/ide)
endef

define KernelPackage/cs5535/description
  Kernel module for the NSC/AMD CS5535 companion chip
endef

$(eval $(call KernelPackage,cs5535))


define KernelPackage/cs5536
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=AMD CS5536 chipset support
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_BLK_DEV_CS5536
  FILES=$(LINUX_DIR)/drivers/ide/cs5536.ko
  AUTOLOAD:=$(call AutoLoad,30,cs5536,1)
  $(call AddDepends/ide)
endef

define KernelPackage/cs5536/description
  Kernel module for the AMD CS5536 Geode LX companion chip
endef

$(eval $(call KernelPackage,cs5536))


define KernelPackage/pata-cs5535
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=CS5535 PATA support
  DEPENDS:=@TARGET_x86 @PCI_SUPPORT
  KCONFIG:=CONFIG_PATA_CS5535
  FILES=$(LINUX_DIR)/drivers/ata/pata_cs5535.ko
  AUTOLOAD:=$(call AutoLoad,30,pata_cs5535,1)
  $(call AddDepends/ata)
endef

define KernelPackage/pata-cs5535/description
  Kernel module for the NSC/AMD CS5535 companion chip
endef

$(eval $(call KernelPackage,pata-cs5535))


define KernelPackage/pata-cs5536
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=CS5536 PATA support
  DEPENDS:=@TARGET_x86 @PCI_SUPPORT
  KCONFIG:=CONFIG_PATA_CS5536
  FILES=$(LINUX_DIR)/drivers/ata/pata_cs5536.ko
  AUTOLOAD:=$(call AutoLoad,30,pata_cs5536,1)
  $(call AddDepends/ata)
endef

define KernelPackage/pata-cs5536/description
  Kernel module for the AMD CS5536 Geode LX companion chip
endef

$(eval $(call KernelPackage,pata-cs5536))
