# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hasivo-mcu-wdt
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define KernelPackage/hasivo-mcu-wdt
  SUBMENU:=Watchdog Timer
  TITLE:=Hasivo MCU watchdog driver
  FILES:=$(PKG_BUILD_DIR)/hasivo-mcu-wdt.ko
  AUTOLOAD:=$(call AutoProbe,hasivo-mcu-wdt,1)
  DEPENDS:=@TARGET_realtek +kmod-i2c-core
endef

define KernelPackage/hasivo-mcu-wdt/description
  Hardware watchdog driver for the external management MCU found on
  Hasivo / Horaco network switches. Registers a Linux watchdog device;
  the kernel watchdog core feeds it automatically via its own timer.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,hasivo-mcu-wdt))
