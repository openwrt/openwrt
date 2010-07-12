#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/rdc321x-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RDC321x watchdog
  DEPENDS:=@TARGET_rdc
  KCONFIG:=CONFIG_RDC321X_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/rdc321x_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,rdc321x_wdt)
endef

define KernelPackage/rdc321x-wdt/description
  RDC-321x watchdog driver
endef

$(eval $(call KernelPackage,rdc321x-wdt))

