#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-diag
PKG_RELEASE:=11

include $(INCLUDE_DIR)/package.mk

define KernelPackage/diag
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_brcm47xx @!LINUX_3_14
  TITLE:=Driver for router LEDs and Buttons
  FILES:=$(PKG_BUILD_DIR)/diag.ko
  AUTOLOAD:=$(call AutoLoad,05,diag,1)
endef

define KernelPackage/diag/description
 Kernel modules for Broadcom LED and Button driver
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
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,diag))
