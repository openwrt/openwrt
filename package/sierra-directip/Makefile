#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sierra-directip
PKG_RELEASE:=10

include $(INCLUDE_DIR)/package.mk

define KernelPackage/usb-sierrawireless-directip
  SUBMENU:=USB Support
  DEPENDS:=+kmod-usb-serial +kmod-usb-net
  TITLE:=Updated Sierra Wireless drivers for DirectIP
  FILES:= \
  	$(PKG_BUILD_DIR)/sierra.ko \
  	$(PKG_BUILD_DIR)/sierra_net.ko
  AUTOLOAD:=$(call AutoLoad,60,sierra sierra_net)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,usb-sierrawireless-directip))
