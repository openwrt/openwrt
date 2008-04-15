#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

#XXX This package will go away once the stuff is merged into the kernel.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mmc-over-gpio
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk


define KernelPackage/mmc-over-gpio
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6 +kmod-mmc-spi +kmod-spi-gpio @!TARGET_x86
  TITLE:=MMC/SD card over GPIO support
  FILES:=$(PKG_BUILD_DIR)/mmc_over_spigpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,93,spi_gpio mmc_over_spigpio)
endef

define KernelPackage/mmc-over-gpio/description
 Support for driving an MMC/SD card over GPIO pins via SPI.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

define KernelPackage/mmc-over-gpio/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mmc_over_gpio.init $(1)/etc/init.d/mmc_over_gpio
endef

$(eval $(call KernelPackage,mmc-over-gpio))
