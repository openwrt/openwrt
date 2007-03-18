# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=acx
PKG_VERSION:=20070101
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://lisas.de/~andi/acx100/
PKG_MD5SUM:=ec6322b9c82781897a9433ef0cefda6f
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
#PKG_CAT:=bzcat
PKG_UNPACK:=bzcat $(DL_DIR)/$(PKG_SOURCE) | \
	$(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS); \
	chmod -R u+w $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/acx
  TITLE:=Driver for TI ACX1xx chipset
  DEPENDS:=@LINUX_2_6 +wireless-tools
  DESCRIPTION:=\
	This package contains a driver for TI ACX1xx 802.11a/b/g chipsets.
  URL:=http://acx100.sourceforge.net/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  SUBMENU:=Wireless Drivers
  FILES:= \
		$(PKG_BUILD_DIR)/acx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,acx)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		CPP="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
		KDIR="$(LINUX_DIR)" \
		CONFIG_ACX=m \
		CONFIG_ACX_PCI=m \
		modules
endef

$(eval $(call KernelPackage,acx))
