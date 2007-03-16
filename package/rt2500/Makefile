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
PKG_VERSION:=1.1.0-b4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rt2400
PKG_MD5SUM:=83b8b9a091705c08d99268479f3b3b6a

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt2500
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for ralink pci wireless chipsets
  DEPENDS:=@LINUX_2_4 @USB_SUPPORT +wireless-tools
  DESCRIPTION:=\
	This package contains a driver for ralink rt2500 pci chipsets.
  URL:=http://rt2x00.serialmonkey.com/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
		$(PKG_BUILD_DIR)/Module/rt2500.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rt2500)
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
