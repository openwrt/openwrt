#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

MFD_MENU:=MultiFunction Device (MFD) Support

define KernelPackage/mfd-hasivo-stc8
  SUBMENU:=$(MFD_MENU)
  TITLE:=Hasivo STC8 microcontroller support
  KCONFIG:=CONFIG_MFD_HASIVO_STC8
  FILES:=$(LINUX_DIR)/drivers/mfd/hasivo-stc8-mfd.ko
  DEPENDS:=@TARGET_realtek +kmod-mfd +kmod-regmap-i2c
  AUTOLOAD:=$(call AutoProbe,hasivo-stc8-mfd)
endef

define KernelPackage/mfd-hasivo-stc8/description
 Kernel module for Hasivo STC8 microcontroller support
endef

$(eval $(call KernelPackage,mfd-hasivo-stc8))
