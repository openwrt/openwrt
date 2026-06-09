#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

MFD_MENU:=MultiFunction Device (MFD) Support
WATCHDOG_MENU:=Watchdog Timer Support

define KernelPackage/hasivo-mcu-wdt
  SUBMENU:=$(WATCHDOG_MENU)
  TITLE:=Hasivo MCU watchdog driver
  KCONFIG:=CONFIG_HASIVO_MCU_WATCHDOG
  FILES:=$(LINUX_DIR)/drivers/watchdog/hasivo-mcu-wdt.ko
  DEPENDS:=@TARGET_realtek +kmod-i2c-core
  AUTOLOAD:=$(call AutoProbe,hasivo-mcu-wdt,1)
endef

define KernelPackage/hasivo-mcu-wdt/description
 Hardware watchdog driver for the external management MCU found on
 Hasivo / Horaco network switches. Registers a Linux watchdog device;
 the kernel watchdog core feeds it automatically via its own timer.
endef

$(eval $(call KernelPackage,hasivo-mcu-wdt))

define KernelPackage/mfd-hasivo-stc8
  SUBMENU:=$(MFD_MENU)
  TITLE:=Hasivo STC8 microcontroller support
  KCONFIG:=CONFIG_MFD_HASIVO_STC8
  FILES:=$(LINUX_DIR)/drivers/mfd/hasivo-stc8-mfd.ko
  DEPENDS:=@TARGET_realtek +kmod-mfd +kmod-regmap-i2c
  AUTOLOAD:=$(call AutoProbe,hasivo-stc8-mfd)
endef

define KernelPackage/mfd-hasivo-stc8/description
 Kernel module for Hasivo STC8 microcontroller support
endef

$(eval $(call KernelPackage,mfd-hasivo-stc8))
