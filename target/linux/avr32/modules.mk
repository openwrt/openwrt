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


define KernelPackage/mmc-atmelmci
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Amtel MMC Support
  DEPENDS:=@TARGET_avr32 +kmod-mmc
  KCONFIG:=CONFIG_MMC_ATMELMCI
  FILES:=$(LINUX_DIR)/drivers/mmc/host/atmel-mci.ko
  AUTOLOAD:=$(call AutoLoad,90,atmel-mci)
endef

define KernelPackage/mmc-atmelmci/description
 Kernel support for  Atmel Multimedia Card Interface.
endef

$(eval $(call KernelPackage,mmc-atmelmci,1))

