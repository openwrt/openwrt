#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

I2C_IMX_MODULES:= \
  CONFIG_I2C_IMX:drivers/i2c/busses/i2c-imx

define KernelPackage/i2c-imx
  $(call i2c_defaults,$(I2C_IMX_MODULES),51)
  TITLE:=Freescale i.MX/MXC
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/i2c-imx/description
 Kernel modules for IMX IIC bus controller
endef

$(eval $(call KernelPackage,i2c-imx))


define KernelPackage/sdhci-of-esdhc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Freescale eSDHC controller support
  DEPENDS:=+kmod-sdhci
  KCONFIG:=CONFIG_MMC_SDHCI_OF_ESDHC
  FILES:=$(LINUX_DIR)/drivers/mmc/host/sdhci-of-esdhc.ko
  AUTOLOAD:=$(call AutoProbe,sdhci-of-esdhc,1)
endef

define KernelPackage/sdhci-of-esdhc/description
 Kernel support for the Freescale eSDHC controller
endef

$(eval $(call KernelPackage,sdhci-of-esdhc))
