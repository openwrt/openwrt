#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

I2C_FALCON_MODULES:= \
  CONFIG_I2C_FALCON:drivers/i2c/busses/i2c-falcon

define KernelPackage/i2c-falcon-lantiq
  TITLE:=Falcon I2C controller
  $(call i2c_defaults,$(I2C_FALCON_MODULES),52)
  DEPENDS:=kmod-i2c-core @(TARGET_lantiq_falcon||TARGET_lantiq_falcon_stable)
endef

define KernelPackage/i2c-falcon-lantiq/description
  Kernel support for the Falcon I2C controller
endef

$(eval $(call KernelPackage,i2c-falcon-lantiq))

