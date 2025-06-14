#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

SPI_MENU:=SPI Support

define KernelPackage/mmc-spi
  SUBMENU:=$(SPI_MENU)
  TITLE:=MMC/SD over SPI Support
  DEPENDS:=+kmod-mmc +kmod-lib-crc-itu-t +kmod-lib-crc7
  KCONFIG:=CONFIG_MMC_SPI \
          CONFIG_SPI=y \
          CONFIG_SPI_MASTER=y
  FILES:=\
	$(LINUX_DIR)/drivers/mmc/host/of_mmc_spi.ko \
	$(LINUX_DIR)/drivers/mmc/host/mmc_spi.ko
  AUTOLOAD:=$(call AutoProbe,of_mmc_spi mmc_spi)
endef

define KernelPackage/mmc-spi/description
 Kernel support for MMC/SD over SPI
endef

$(eval $(call KernelPackage,mmc-spi))


define KernelPackage/spi-bitbang
  SUBMENU:=$(SPI_MENU)
  TITLE:=Serial Peripheral Interface bitbanging library
  KCONFIG:=CONFIG_SPI_BITBANG \
          CONFIG_SPI=y \
          CONFIG_SPI_MASTER=y
  FILES:=$(LINUX_DIR)/drivers/spi/spi-bitbang.ko
endef

define KernelPackage/spi-bitbang/description
 This package contains the SPI bitbanging library
endef

$(eval $(call KernelPackage,spi-bitbang))


define KernelPackage/spi-gpio
  SUBMENU:=$(SPI_MENU)
  TITLE:=GPIO-based bitbanging SPI Master
  DEPENDS:=@GPIO_SUPPORT +kmod-spi-bitbang
  KCONFIG:=CONFIG_SPI_GPIO
  FILES:=$(LINUX_DIR)/drivers/spi/spi-gpio.ko
  AUTOLOAD:=$(call AutoProbe,spi-gpio)
endef

define KernelPackage/spi-gpio/description
 This package contains the GPIO-based bitbanging SPI Master
endef

$(eval $(call KernelPackage,spi-gpio))

define KernelPackage/spi-dev
  SUBMENU:=$(SPI_MENU)
  TITLE:=User mode SPI device driver
  KCONFIG:=CONFIG_SPI_SPIDEV \
          CONFIG_SPI=y \
          CONFIG_SPI_MASTER=y
  FILES:=$(LINUX_DIR)/drivers/spi/spidev.ko
  AUTOLOAD:=$(call AutoProbe,spidev)
endef

define KernelPackage/spi-dev/description
 This package contains the user mode SPI device driver
endef

$(eval $(call KernelPackage,spi-dev))


define KernelPackage/spi-dw
  SUBMENU:=$(SPI_MENU)
  TITLE:=DesignWare SPI controller driver (core)
  KCONFIG:=\
    CONFIG_SPI=y \
    CONFIG_SPI_DESIGNWARE \
    CONFIG_SPI_DYNAMIC=y \
    CONFIG_SPI_MASTER=y
  FILES:=\
    $(LINUX_DIR)/drivers/spi/spi-dw.ko
  AUTOLOAD:=$(call AutoProbe,spi-dw)
endef

define KernelPackage/spi-dw/description
 This package contains the DesignWare SPI core driver.
endef

$(eval $(call KernelPackage,spi-dw))


define KernelPackage/spi-dw-mmio
  SUBMENU:=$(SPI_MENU)
  TITLE:=DesignWare SPI controller driver (MMIO)
  DEPENDS:=+kmod-spi-dw
  KCONFIG:=\
    CONFIG_SPI_DW_MMIO
  FILES:=\
    $(LINUX_DIR)/drivers/spi/spi-dw-mmio.ko
  AUTOLOAD:=$(call AutoProbe,spi-dw-mmio)
endef

define KernelPackage/spi-dw-mmio/description
 This package contains the DesignWare SPI MMIO driver.
endef

$(eval $(call KernelPackage,spi-dw-mmio))
