#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id $

I2CMENU:=I2C Bus

define KernelPackage/i2c-core
  TITLE:=I2C support
  DESCRIPTION:=Kernel modules for i2c support
  SUBMENU:=$(I2CMENU)
  KCONFIG:=$(CONFIG_I2C_CORE) \
	$(CONFIG_I2C_DEV)
  FILES:=$(MODULES_DIR)/kernel/drivers/i2c/*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,i2c-core i2c-dev)
endef
$(eval $(call KernelPackage,i2c-core))

define KernelPackage/i2c-algos
  TITLE:=I2C algorithms support
  DESCRIPTION:=Kernel modules for various i2c algorithms
  SUBMENU:=$(I2CMENU)
  KCONFIG:=$(CONFIG_I2C_ALGOBIT)
  FILES:=$(MODULES_DIR)/kernel/drivers/i2c/algos/*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,i2c-algo-bit i2c-algo-pcf i2c-algo-pca)
endef
$(eval $(call KernelPackage,i2c-algos))

define KernelPackage/i2c-au1x00gpio
  TITLE:=I2C GPIO interface for Au1x00 devices
  DESCRIPTION:=I2C GPIO interface for Au1x00 devices
  DEFAULT:=y if LINUX_2_6_AU1000
  DEPENDS:=+kmod-i2c-core +kmod-i2c-algos
  SUBMENU:=$(I2CMENU)
  KCONFIG:=$(CONFIG_I2C_AU1X00GPIO)
  FILES:=$(MODULES_DIR)/kernel/drivers/i2c/busses/i2c-au1x00gpio.$(LINUX_KMOD_SUFFIX)
 AUTOLOAD:=$(call Autoload,70,i2c-au1x00gpio)
endef
$(eval $(call KernelPackage,i2c-au1x00gpio))

