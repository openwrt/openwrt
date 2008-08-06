# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7137 2007-05-08 18:40:02Z nbd $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=spca5xx-le
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tar.bz2
PKG_SOURCE_URL:=http://www.acmesystems.it/download/owrt/
PKG_MD5SUM:=

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/video-spca5xx-le
  SUBMENU:=Video Support
  TITLE:=SPCA5xx based USB webcam support (Light Edition)
  URL:=http://mxhaard.free.fr/
  DEPENDS:=@LINUX_2_6_23||LINUX_2_6_24||LINUX_2_6_25 @USB_SUPPORT +kmod-usb-core kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/spca5xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,spca5xx)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-spca5xx-le))
