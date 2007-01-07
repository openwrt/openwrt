# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt61
PKG_VERSION:=1.1.0-b1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/rt61-$(PKG_VERSION)/
PKG_SOURCE:=rt61-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rt2400
PKG_MD5SUM:=3f2600400a3d6d878a5d27d0a2e47eed
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt61
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for ralink MiMo pci chipsets 
  DEPENDS:=@LINUX_2_4 @PCI_SUPPORT +wireless-tools
  DESCRIPTION:=\
        This package contains a driver for ralink rt61xx pci chipsets.
  URL:=http://rt2x00.serialmonkey.com/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
                $(PKG_BUILD_DIR)/Module/rt61.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,rt61)
endef

LINUX_PATCHLEVEL:=$(shell echo "$(LINUX_VERSION)" | cut -d. -f2)

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		PATCHLEVEL="$(LINUX_PATCHLEVEL)" \
		KERNDIR="$(LINUX_DIR)" \
		SUBDIRS="$(PKG_BUILD_DIR)/Module" \
		modules
endef

$(eval $(call KernelPackage,rt61))
