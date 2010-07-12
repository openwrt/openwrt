#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/atmel-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=AT32AP700x watchdog
  DEPENDS:=@TARGET_avr32
  KCONFIG:=CONFIG_AT32AP700X_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/at32ap700x_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,at32ap700x_wdt)
endef

define KernelPackage/atmel-wdt/description
 AT32AP700x watchdog
endef

$(eval $(call KernelPackage,atmel-wdt))
