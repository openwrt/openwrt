#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mmc-over-gpio
PKG_RELEASE:=4

include $(INCLUDE_DIR)/package.mk


define KernelPackage/mmc-over-gpio
  SUBMENU:=Other modules
  DEPENDS:=@GPIO_SUPPORT +kmod-mmc-spi +kmod-spi-gpio-old +kmod-fs-configfs
  KCONFIG:=CONFIG_GPIOMMC
  TITLE:=MMC/SD card over GPIO support
  FILES:=$(LINUX_DIR)/drivers/mmc/host/gpiommc.ko
  AUTOLOAD:=$(call AutoLoad,93,gpiommc)
  MENU:=1
endef

define Package/kmod-mmc-over-gpio/config
	menu "Configuration"
		depends PACKAGE_kmod-mmc-over-gpio

	config KMOD_MMC_OVER_GPIO_DI_PIN
		int "GPIO DI (Data-In) pin"
		default 1

	config KMOD_MMC_OVER_GPIO_DO_PIN
		int "GPIO DO (Data-Out) pin"
		default 3

	config KMOD_MMC_OVER_GPIO_CLK_PIN
		int "GPIO CLK (Clock) pin"
		default 4

	config KMOD_MMC_OVER_GPIO_CS_PIN
		int "GPIO CS (Chip-Select) pin"
		default 7

	endmenu
endef

define KernelPackage/mmc-over-gpio/description
 Support for driving an MMC/SD card over GPIO pins via SPI.
endef

define KernelPackage/mmc-over-gpio/conffiles
/etc/config/mmc_over_gpio
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define KernelPackage/mmc-over-gpio/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/mmc_over_gpio.config $(1)/etc/config/mmc_over_gpio
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mmc_over_gpio.init $(1)/etc/init.d/mmc_over_gpio

	$(SED) 's,@GPIO_DI_PIN@,$(CONFIG_KMOD_MMC_OVER_GPIO_DI_PIN),g' \
		-e 's,@GPIO_DO_PIN@,$(CONFIG_KMOD_MMC_OVER_GPIO_DO_PIN),g' \
		-e 's,@GPIO_CLK_PIN@,$(CONFIG_KMOD_MMC_OVER_GPIO_CLK_PIN),g' \
		-e 's,@GPIO_CS_PIN@,$(CONFIG_KMOD_MMC_OVER_GPIO_CS_PIN),g' \
		$(1)/etc/config/mmc_over_gpio
endef

$(eval $(call KernelPackage,mmc-over-gpio))
