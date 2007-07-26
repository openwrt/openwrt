# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux-uvc
PKG_VERSION:=r111
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://naaa.de/programme/linux-uvc/
PKG_MD5SUM:=36c742d9dd1cf533fa059a1eeffa3e32

include $(INCLUDE_DIR)/package.mk

define KernelPackage/uvcvideo
  SUBMENU:=USB Support
  DEPENDS:=@LINUX_2_6 kmod-usb-core kmod-videodev @VIDEO_SUPPORT
  TITLE:=Driver for Linux-UVC based USB cameras
  URL:=http://linux-uvc.berlios.de/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_BUILD_DIR)/uvcvideo.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,uvcvideo)
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

$(eval $(call KernelPackage,uvcvideo))
