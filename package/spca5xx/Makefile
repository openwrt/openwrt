# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gspcav1
PKG_VERSION:=20070508
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mxhaard.free.fr/spca50x/Download
PKG_MD5SUM:=e18a21b80e1728c6b51b799d1e671f8c

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gspca
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6 kmod-usb-core kmod-videodev @VIDEO_SUPPORT
  TITLE:=Driver for SPCA5xx based USB cameras
  URL:=http://mxhaard.free.fr/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/gspca.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,gspca)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_VERSION="$(LINUX_VERSION)" \
		KERNELDIR="$(LINUX_DIR)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,gspca))
