#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mmc-over-gpio
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk


define KernelPackage/mmc-over-gpio
  SUBMENU:=Other modules
  DEPENDS:=@GPIO_SUPPORT +kmod-mmc-spi +kmod-spi-gpio
  KCONFIG:=CONFIG_GPIOMMC CONFIG_CONFIGFS_FS=y
  TITLE:=MMC/SD card over GPIO support
  FILES:=$(LINUX_DIR)/drivers/mmc/host/gpiommc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,93,spi_gpio gpiommc)
endef

define KernelPackage/mmc-over-gpio/description
 Support for driving an MMC/SD card over GPIO pins via SPI.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define KernelPackage/mmc-over-gpio/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mmc_over_gpio.init $(1)/etc/init.d/mmc_over_gpio
endef

$(eval $(call KernelPackage,mmc-over-gpio))
