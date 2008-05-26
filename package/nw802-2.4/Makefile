# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=nw802-2.4
PKG_VERSION:=0.0.99
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/nw802
PKG_MD5SUM:=bf3512efaa48cfa972b16318011c1963

include $(INCLUDE_DIR)/package.mk

define KernelPackage/video-nw8xx
  SUBMENU:=Video Support
  TITLE:=NW8xx based USB webcam support
  URL:=http://nw802.sourceforge.net/
  DEPENDS:=@LINUX_2_6 @USB_SUPPORT +kmod-usb-video kmod-video-core
  FILES:=$(PKG_BUILD_DIR)/nw8xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,70,nw8xx)
endef

define KernelPackage/video-nw8xx/description
 Kernel support for NW8xx based USB cameras.
endef

define Build/Compile
	ln -sf $(LINUX_DIR)/drivers/media/video/usbvideo/usbvideo.h \
		$(PKG_BUILD_DIR)/
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,video-nw8xx))
