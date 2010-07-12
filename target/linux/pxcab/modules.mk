#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/axonram
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Axon DDR2 memory device driver
  DEPENDS:=@TARGET_pxcab
  KCONFIG:=CONFIG_AXON_RAM
  FILES:=$(LINUX_DIR)/arch/powerpc/sysdev/axonram.ko
  AUTOLOAD:=$(call AutoLoad,01,axonram)
endef

define KernelPackage/axonram/description
  Kernel support for Axon DDR2 memory device
endef

$(eval $(call KernelPackage,axonram))
