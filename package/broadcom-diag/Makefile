# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-diag
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/diag
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  DEPENDS:=@LINUX_2_4_BRCM||LINUX_2_6_BRCM
  TITLE:=Driver for router LEDs and Buttons
  VERSION:=$(PKG_RELEASE)+$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/diag.$(LINUX_KMOD_SUFFIX)
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
		EXTRA_CFLAGS="-DBCMDRIVER -I$(LINUX_DIR)/arch/mips/bcm947xx/include" \
		modules
endef
	
$(eval $(call KernelPackage,diag))
