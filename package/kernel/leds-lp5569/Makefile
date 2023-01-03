#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-lp5569
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-lp5569
  SUBMENU:=LED modules
  TITLE:=LP5569 LED Controller support
  DEPENDS:=+kmod-leds-lp55xx-common
  FILES:= \
	$(PKG_BUILD_DIR)/leds-lp5569.ko
  AUTOLOAD:=$(call AutoLoad,60,leds-lp5569,1)
endef

define KernelPackage/leds-lp5569/description
  LED support LP5569 controller
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-lp5569))
