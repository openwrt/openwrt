# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux-uvc
PKG_VERSION:=r131
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://naaa.de/programme/linux-uvc/
PKG_MD5SUM:=e1f5e5c9bdf865726438dc3ff3a5ae60

include $(INCLUDE_DIR)/package.mk

define KernelPackage/video-uvc
  SUBMENU:=Video Support
  TITLE:=USB Video Class (UVC) support
  URL:=http://linux-uvc.berlios.de/
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-core kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/uvcvideo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,uvcvideo)
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-uvc))
