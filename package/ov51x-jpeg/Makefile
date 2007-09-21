# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ov51x-jpeg
PKG_VERSION:=1.5.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.rastageeks.org/downloads/ov51x-jpeg/
PKG_MD5SUM:=baee1e0a7a16772e81ea36d8b3ce6127

include $(INCLUDE_DIR)/package.mk

# XXX: remove @!TARGET_* later when we have USB support properly detected on all targets
define KernelPackage/video-ov51x-jpeg
  SUBMENU:=Video Support
  TITLE:=OV51x based USB webcam support (JPEG)
  URL:=http://www.rastageeks.org/ov51x-jpeg/
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core kmod-video-core @!TARGET_atheros @!TARGET_uml
  FILES:=$(PKG_BUILD_DIR)/ov51x-jpeg.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,ov51x-jpeg)
endef

define KernelPackage/video-ov51x-jpeg/description
 Kernel module for OV51x based USB cameras, with in-driver JPEG support.
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-ov51x-jpeg))
