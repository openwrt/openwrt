#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=leds-fan5646
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-fan5646
  SUBMENU:=LED modules
  TITLE:=Fairchild FAN5646 LED support
  FILES:= \
	$(PKG_BUILD_DIR)/leds-fan5646.ko
  AUTOLOAD:=$(call AutoProbe,leds-fan5646,1)
endef

define KernelPackage/leds-fan5646/description
  LED support for driving FAN5646 using GPIO.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-fan5646))
