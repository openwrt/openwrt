# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-wlcompat
PKG_RELEASE:=4

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/wlcompat/Default
  SUBMENU:=Proprietary BCM43xx WiFi driver
  SUBMENUDEP:=@LINUX_2_4_BRCM
  DEPENDS:=@LINUX_2_4_BRCM @PACKAGE_kmod-brcm-wl||PACKAGE_kmod-brcm-wl-mimo +wireless-tools
  TITLE:=Broadcom wl wrapper module
  DESCRIPTION:=\
	This package contains a wrapper module, that provides Wireless Extension \\\
	support for the proprietary Broadcom wl module.
  VERSION:=$(LINUX_VERSION)+$(BOARD)-$(PKG_RELEASE)
endef

define KernelPackage/wlcompat
  $(call KernelPackage/wlcompat/Default)
  FILES:=$(PKG_BUILD_DIR)/wlcompat.o
  AUTOLOAD:=$(call AutoLoad,50,wlcompat)
endef

define KernelPackage/wlcompat-debug
  $(call KernelPackage/wlcompat/Default)
  TITLE+= (debug)
  DESCRIPTION+=\\\
	\\\
	This is a debugging version.
  FILES:=$(PKG_BUILD_DIR)/wlcompat-debug.o
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DDEBUG -I$(LINUX_DIR)/arch/mips/bcm947xx/include -I$(PKG_BUILD_DIR)/include" \
		modules
	mv $(PKG_BUILD_DIR)/wlcompat.o $(PKG_BUILD_DIR)/wlcompat-debug.o
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-I$(LINUX_DIR)/arch/mips/bcm947xx/include -I$(PKG_BUILD_DIR)/include" \
		modules
endef

$(eval $(call KernelPackage,wlcompat))
$(eval $(call KernelPackage,wlcompat-debug))
