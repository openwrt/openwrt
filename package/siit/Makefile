# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=siit
PKG_VERSION:=1.1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/siit
  SUBMENU:=Network Devices
  TITLE:=Stateless IP ICMP Translation Algorithm
  DEPENDS:=@!LINUX_2_6_31 @(!(TARGET_ps3||TARGET_pxcab)||BROKEN)
  FILES:=$(PKG_BUILD_DIR)/siit.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,siit)
endef

define KernelPackage/siit/description
 Stateless IP ICMP Translation Algorithm
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp src/Makefile src/siit.h src/siit.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		KERNELDIR=$(LINUX_DIR) \
		CC="$(TARGET_CC)" \
		modules
endef

$(eval $(call KernelPackage,siit))
