# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt2500
PKG_VERSION:=cvs-2007061115
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@openwrt/
PKG_MD5SUM:=67a158b24ec46e37edc4545f51338bb9

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt2500
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Ralink RT2500 pci wireless chipsets
  DEPENDS:=@!BIG_ENDIAN @USB_SUPPORT +wireless-tools
  URL:=http://rt2x00.serialmonkey.com/
  FILES:=$(PKG_BUILD_DIR)/Module/rt2500.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rt2500)
endef

define KernelPackage/rt2500/description
 This package contains a driver for Ralink RT2500 pci chipsets.
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

$(eval $(call KernelPackage,rt2500))
