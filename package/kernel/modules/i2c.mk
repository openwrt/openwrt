#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id $

I2C_MENU:=I2C support

define KernelPackage/i2c-core
  SUBMENU:=$(I2C_MENU)
  TITLE:=I2C support
  DEPENDS:=@LINUX_2_6
  KCONFIG:= \
	CONFIG_I2C \
	CONFIG_I2C_CHARDEV
  FILES:= \
	$(LINUX_DIR)/drivers/i2c/i2c-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/i2c/i2c-dev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,51,i2c-core i2c-dev)
endef

define KernelPackage/i2c-core/description
 Kernel modules for I2C support
endef

$(eval $(call KernelPackage,i2c-core))


define KernelPackage/i2c-algo-bit
  SUBMENU:=$(I2C_MENU)
  TITLE:=I2C bit-banging interfaces
  DEPENDS:=kmod-i2c-core
  KCONFIG:=CONFIG_I2C_ALGOBIT
  FILES:=$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-bit.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,i2c-algo-bit)
endef

define KernelPackage/i2c-algo-bit/description
 Kernel modules for I2C bit-banging interfaces.
endef

$(eval $(call KernelPackage,i2c-algo-bit))


define KernelPackage/i2c-algo-pca
  SUBMENU:=$(I2C_MENU)
  TITLE:=I2C PCA 9564 interfaces
  DEPENDS:=kmod-i2c-core
  KCONFIG:=CONFIG_I2C_ALGOPCA
  FILES:=$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-pca.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,i2c-algo-pca)
endef

define KernelPackage/i2c-algo-pca/description
 Kernel modules for I2C PCA 9564 interfaces.
endef

$(eval $(call KernelPackage,i2c-algo-pca))


define KernelPackage/i2c-algo-pcf
  SUBMENU:=$(I2C_MENU)
  TITLE:=I2C PCF 8584 interfaces
  DEPENDS:=kmod-i2c-core
  KCONFIG:=CONFIG_I2C_ALGOPCF
  FILES:=$(LINUX_DIR)/drivers/i2c/algos/i2c-algo-pcf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,55,i2c-algo-pcf)
endef

define KernelPackage/i2c-algo-pcf/description
 Kernel modules for I2C PCF 8584 interfaces
endef

$(eval $(call KernelPackage,i2c-algo-pcf))


define KernelPackage/i2c-gpio
  SUBMENU:=$(I2C_MENU)
  TITLE:=GPIO-based bitbanging I2C
  DEPENDS:=@GPIO_SUPPORT +kmod-i2c-algo-bit
  KCONFIG:=CONFIG_I2C_GPIO
  FILES:=$(LINUX_DIR)/drivers/i2c/busses/i2c-gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,59,i2c-gpio)
endef

define KernelPackage/i2c-gpio/description
 Kernel modules for a very simple bitbanging I2C driver utilizing the
 arch-neutral GPIO API to control the SCL and SDA lines.
endef

$(eval $(call KernelPackage,i2c-gpio))


define KernelPackage/i2c-scx200
  SUBMENU:=$(I2C_MENU)
  TITLE:=Geode SCx200 I2C using GPIO pins
  DEPENDS:=@PCI_SUPPORT @TARGET_x86 +kmod-i2c-algo-bit
  KCONFIG:=CONFIG_SCx200_I2C
  FILES:=$(LINUX_DIR)/drivers/i2c/busses/scx200_i2c.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,59,scx200_i2c)
endef

define KernelPackage/i2c-scx200/description
 Kernel module for I2C using GPIO pins on the Geode SCx200 processors.
endef

$(eval $(call KernelPackage,i2c-scx200))


define KernelPackage/i2c-scx200-acb
  SUBMENU:=$(I2C_MENU)
  TITLE:=Geode SCx200 ACCESS.bus support
  DEPENDS:=@PCI_SUPPORT @TARGET_x86 +kmod-i2c-algo-bit
  KCONFIG:=CONFIG_SCx200_ACB
  FILES:=$(LINUX_DIR)/drivers/i2c/busses/scx200_acb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,59,scx200_acb)
endef

define KernelPackage/i2c-scx200-acb/description
 Kernel module for I2C using the ACCESS.bus controllers on the Geode SCx200
 and SC1100 processors and the CS5535 and CS5536 Geode companion devices.
endef

$(eval $(call KernelPackage,i2c-scx200-acb))

