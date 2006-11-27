# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 5314 2006-10-27 19:14:43Z nico $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt2570
PKG_VERSION:=1.1.0-b2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rt2400
PKG_MD5SUM:=f4131d670920a878b4d4a0f5d4d8b93a
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt2570
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for ralink usb wireless chipsets
  DEPENDS:=@USB_SUPPORT +wireless-tools
  DESCRIPTION:=\
	This package contains a driver for ralink rt2570 usb chipsets.
  URL:=http://rt2x00.serialmonkey.com/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
		$(PKG_BUILD_DIR)/Module/rt2570.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rt2570)
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)/Module" \
		CC="$(TARGET_CC)" \
		ARCH="$(LINUX_KARCH)" \
		PWD="$(PKG_BUILD_DIR)/Module" \
		KERNDIR="$(LINUX_DIR)" \
		PATCHLEVEL=" $(patsubst 2.%,%,$(KERNEL))"
endef

$(eval $(call KernelPackage,rt2570))
