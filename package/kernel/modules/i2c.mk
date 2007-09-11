#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id $

I2CMENU:=I2C Bus

define KernelPackage/i2c-core
  SUBMENU:=$(I2CMENU)
  TITLE:=I2C support
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_I2C \
	CONFIG_I2C_CHARDEV
  FILES:= \
	$(LINUX_DIR)/drivers/i2c/i2c-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/i2c/i2c-dev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,i2c-core i2c-dev)
endef

define KernelPackage/i2c-core/description
 Kernel modules for I2C support
endef

$(eval $(call KernelPackage,i2c-core))


define KernelPackage/i2c-algos
  SUBMENU:=$(I2CMENU)
  TITLE:=I2C algorithms support
  DEPENDS:=kmod-i2c-core
  KCONFIG:= \
	CONFIG_I2C_ALGOBIT \
	CONFIG_I2C_ALGOPCF \
	CONFIG_I2C_ALGOPCA
  FILES:= \
	$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-bit.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-pcf.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-pca.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,i2c-algo-bit i2c-algo-pcf i2c-algo-pca)
endef

define KernelPackage/i2c-algos/description
 Kernel modules for various I2C algorithms
endef

$(eval $(call KernelPackage,i2c-algos))


define KernelPackage/i2c-scx200
  SUBMENU:=$(I2CMENU)
  TITLE:=SCX200 i2c support
  DEFAULT:=y if TARGET_x86_Soekris
  DEPENDS:=@TARGET_x86_Soekris kmod-i2c-core kmod-i2c-algos
  KCONFIG:= \
	CONFIG_I2C_ISA \
	CONFIG_SCx200_I2C
  FILES:= \
	$(LINUX_DIR)/drivers/i2c/busses/i2c-isa.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/i2c/busses/scx200_i2c.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,i2c-isa scx200_i2c)
endef

define KernelPackage/i2c-scx200/description
 Kernel module for SCX200 I2C bus
endef

$(eval $(call KernelPackage,i2c-scx200))
