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
PKG_REV:=215
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://svn.berlios.de/svnroot/repos/linux-uvc/linux-uvc/trunk/
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

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
	echo "#define DRIVER_VERSION	\"SVN $(PKG_VERSION)\"" > "$(PKG_BUILD_DIR)/version.h"
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-uvc))
