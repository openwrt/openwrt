#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

I2C_MENU:=I2C support

ModuleConfVar=$(word 1,$(subst :,$(space),$(1)))
ModuleFullPath=$(if $(findstring y,$($(call ModuleConfVar,$(1)))),,$(LINUX_DIR)/$(word 2,$(subst :,$(space),$(1))).ko)
ModuleKconfig=$(foreach mod,$(1),$(call ModuleConfVar,$(mod)))
ModuleFiles=$(foreach mod,$(1),$(call ModuleFullPath,$(mod)))
ModuleAuto=$(call AutoLoad,$(1),$(foreach mod,$(2),$(basename $(notdir $(call ModuleFullPath,$(mod))))),$(3))

define i2c_defaults
  SUBMENU:=$(I2C_MENU)
  KCONFIG:=$(call ModuleKconfig,$(1))
  FILES:=$(call ModuleFiles,$(1))
  AUTOLOAD:=$(call ModuleAuto,$(2),$(1),$(3))
endef

I2C_CORE_MODULES:= \
  CONFIG_I2C:drivers/i2c/i2c-core \
  CONFIG_I2C_CHARDEV:drivers/i2c/i2c-dev

define KernelPackage/i2c-core
  $(call i2c_defaults,$(I2C_CORE_MODULES),51)
  TITLE:=I2C support
  DEPENDS:=@!TARGET_etrax
endef

define KernelPackage/i2c-core/description
 Kernel modules for I2C support
endef

$(eval $(call KernelPackage,i2c-core))


I2C_ALGOBIT_MODULES:= \
  CONFIG_I2C_ALGOBIT:drivers/i2c/algos/i2c-algo-bit

define KernelPackage/i2c-algo-bit
  $(call i2c_defaults,$(I2C_ALGOBIT_MODULES),55)
  TITLE:=I2C bit-banging interfaces
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/i2c-algo-bit/description
 Kernel modules for I2C bit-banging interfaces.
endef

$(eval $(call KernelPackage,i2c-algo-bit))

I2C_TINY_USB_MODULES:= \
  CONFIG_I2C_TINY_USB:drivers/i2c/busses/i2c-tiny-usb

define KernelPackage/i2c-tiny-usb
  $(call i2c_defaults,$(I2C_TINY_USB_MODULES),59)
  TITLE:=I2C Tiny USB adaptor
  DEPENDS:=@USB_SUPPORT kmod-i2c-core
endef

define KernelPackage/i2c-tiny-usb/description
 Kernel module for the I2C Tiny USB adaptor developed
 by Till Harbaum (http://www.harbaum.org/till/i2c_tiny_usb).
endef

$(eval $(call KernelPackage,i2c-tiny-usb))

I2C_MUX_MODULES:= \
  CONFIG_I2C_MUX:drivers/i2c/i2c-mux

define KernelPackage/i2c-mux
  $(call i2c_defaults,$(I2C_MUX_MODULES),51)
  TITLE:=I2C bus multiplexing support
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/i2c-mux/description
 Kernel modules for I2C bus multiplexing support.
endef

$(eval $(call KernelPackage,i2c-mux))

I2C_MUX_GPIO_MODULES:= \
  CONFIG_I2C_MUX_GPIO:drivers/i2c/muxes/gpio-i2cmux

define KernelPackage/i2c-mux-gpio
  $(call i2c_defaults,$(I2C_MUX_GPIO_MODULES),51)
  TITLE:=GPIO-based I2C mux/switches
  DEPENDS:=kmod-i2c-mux
endef

define KernelPackage/i2c-mux-gpio/description
 Kernel modules for GENERIC_GPIO I2C bus mux/switching devices.
endef

$(eval $(call KernelPackage,i2c-mux-gpio))

I2C_MUX_PCA954x_MODULES:= \
  CONFIG_I2C_MUX_PCA954x:drivers/i2c/muxes/pca954x

define KernelPackage/i2c-mux-pca954x
  $(call i2c_defaults,$(I2C_MUX_PCA954x_MODULES),51)
  TITLE:=Philips PCA954x I2C mux/switches
  DEPENDS:=kmod-i2c-mux
endef

define KernelPackage/i2c-mux-pca954x/description
 Kernel modules for PCA954x I2C bus mux/switching devices.
endef

$(eval $(call KernelPackage,i2c-mux-pca954x))
## Support for pca954x seems to be in kernel since 2.6.36

I2C_MUX_PCA9541_MODULES:= \
  CONFIG_I2C_MUX_PCA9541:drivers/i2c/muxes/pca9541

define KernelPackage/i2c-mux-pca9541
  $(call i2c_defaults,$(I2C_MUX_PCA9541_MODULES),51)
  TITLE:=Philips PCA9541 I2C mux/switches
  DEPENDS:=kmod-i2c-mux
endef

define KernelPackage/i2c-mux-pca9541/description
 Kernel modules for PCA9541 I2C bus mux/switching devices.
endef

$(eval $(call KernelPackage,i2c-mux-pca9541))

GPIO_PCA953X_MODULES:= \
  CONFIG_GPIO_PCA953X:drivers/gpio/gpio-pca953x

define KernelPackage/pca953x
  $(call i2c_defaults,$(GPIO_PCA953X_MODULES),51)
  TITLE:=Philips PCA953x I2C GPIO extenders
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/pca953x/description
 Kernel modules for PCA953x I2C GPIO extenders.
endef

$(eval $(call KernelPackage,pca953x))

GPIO_PCF857X_MODULES:= \
  CONFIG_GPIO_PCF857X:drivers/gpio/gpio-pcf857x

define KernelPackage/pcf857x
  $(call i2c_defaults,$(GPIO_PCF857X_MODULES),51)
  TITLE:=Philips PCF857x I2C GPIO extenders
  DEPENDS:=kmod-i2c-core
endef

define KernelPackage/pcf857x/description
 Kernel modules for PCF857x I2C GPIO extenders.
endef

$(eval $(call KernelPackage,pcf857x))
