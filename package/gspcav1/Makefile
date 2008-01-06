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
PKG_VERSION:=20071224
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mxhaard.free.fr/spca50x/Download
PKG_MD5SUM:=14853ba1f4edc1e685039fca56e5ebf

include $(INCLUDE_DIR)/package.mk

define KernelPackage/video-gspca
  SUBMENU:=Video Support
  TITLE:=SPCA5xx based USB webcam support
  URL:=http://mxhaard.free.fr/
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/gspca.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,gspca)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-gspca))
