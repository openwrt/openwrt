#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-gca230718
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-gca230718
  SUBMENU:=LED modules
  TITLE:=GCA230718 LED support (e.g. for D-Link M30)
  FILES:= \
	$(PKG_BUILD_DIR)/leds-gca230718.ko
  AUTOLOAD:=$(call AutoProbe,leds-gca230718,1)
  DEPENDS:=@TARGET_mediatek_filogic
endef

define KernelPackage/leds-gca230718/description
  GCA230718 LED support (e.g. for D-Link M30) using I2C.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-gca230718))
