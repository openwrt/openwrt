# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt2570
PKG_VERSION:=1.1.0-b2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rt2400
PKG_MD5SUM:=f4131d670920a878b4d4a0f5d4d8b93a

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt2570
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Ralink RT2570 usb wireless chipsets
  DEPENDS:=@LINUX_2_4 @USB_SUPPORT +wireless-tools
  URL:=http://rt2x00.serialmonkey.com/
  FILES:=$(PKG_BUILD_DIR)/Module/rt2570.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rt2570)
endef

define KernelPackage/rt2570/description
 This package contains a driver for Ralink RT2570 usb chipsets.
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)/Module" \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		ARCH="$(LINUX_KARCH)" \
		PWD="$(PKG_BUILD_DIR)/Module" \
		KERNDIR="$(LINUX_DIR)" \
		PATCHLEVEL="$(patsubst 2.%,%,$(KERNEL))"
endef

$(eval $(call KernelPackage,rt2570))
