#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-group-virtualcolor
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-group-virtualcolor
  SUBMENU:=LED modules
  TITLE:=led virtual color driver
  FILES:= \
	$(PKG_BUILD_DIR)/leds-group-virtualcolor.ko
  AUTOLOAD:=$(call AutoProbe,leds-group-virtualcolor,1)
endef

define KernelPackage/leds-group-virtualcolor/description
  monocromatic virtual led driver
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-group-virtualcolor))
